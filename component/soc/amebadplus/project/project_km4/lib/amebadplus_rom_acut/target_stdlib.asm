
/home/zjian_zhang/Dplus_Z/project/realtek_amebaDplus_va0_example/GCC-RELEASE/project_km4/asdk/image/target_stdlib.axf:     file format elf32-littlearm


Disassembly of section .rom.stdlib.text:

00033000 <stdlib_entry_table>:
   33000:	89 37 03 00 c1 35 03 00                             .7...5..

00033008 <test_strcat>:
   33008:	b580      	push	{r7, lr}
   3300a:	b086      	sub	sp, #24
   3300c:	af00      	add	r7, sp, #0
   3300e:	f247 4373 	movw	r3, #29811	; 0x7473
   33012:	607b      	str	r3, [r7, #4]
   33014:	f107 0308 	add.w	r3, r7, #8
   33018:	2200      	movs	r2, #0
   3301a:	601a      	str	r2, [r3, #0]
   3301c:	605a      	str	r2, [r3, #4]
   3301e:	609a      	str	r2, [r3, #8]
   33020:	60da      	str	r2, [r3, #12]
   33022:	4b0b      	ldr	r3, [pc, #44]	; (33050 <test_strcat+0x48>)
   33024:	603b      	str	r3, [r7, #0]
   33026:	1d3b      	adds	r3, r7, #4
   33028:	4619      	mov	r1, r3
   3302a:	480a      	ldr	r0, [pc, #40]	; (33054 <test_strcat+0x4c>)
   3302c:	f000 fbe2 	bl	337f4 <printf>
   33030:	463a      	mov	r2, r7
   33032:	1d3b      	adds	r3, r7, #4
   33034:	4611      	mov	r1, r2
   33036:	4618      	mov	r0, r3
   33038:	f000 fdbd 	bl	33bb6 <strcat>
   3303c:	4603      	mov	r3, r0
   3303e:	4619      	mov	r1, r3
   33040:	4805      	ldr	r0, [pc, #20]	; (33058 <test_strcat+0x50>)
   33042:	f000 fbd7 	bl	337f4 <printf>
   33046:	bf00      	nop
   33048:	3718      	adds	r7, #24
   3304a:	46bd      	mov	sp, r7
   3304c:	bd80      	pop	{r7, pc}
   3304e:	bf00      	nop
   33050:	00297473 	.word	0x00297473
   33054:	00040394 	.word	0x00040394
   33058:	0004039c 	.word	0x0004039c

0003305c <test_strchr>:
   3305c:	b580      	push	{r7, lr}
   3305e:	b082      	sub	sp, #8
   33060:	af00      	add	r7, sp, #0
   33062:	4b07      	ldr	r3, [pc, #28]	; (33080 <test_strchr+0x24>)
   33064:	607b      	str	r3, [r7, #4]
   33066:	2135      	movs	r1, #53	; 0x35
   33068:	6878      	ldr	r0, [r7, #4]
   3306a:	f000 fdbd 	bl	33be8 <strchr>
   3306e:	6038      	str	r0, [r7, #0]
   33070:	6838      	ldr	r0, [r7, #0]
   33072:	f000 fc5f 	bl	33934 <puts>
   33076:	bf00      	nop
   33078:	3708      	adds	r7, #8
   3307a:	46bd      	mov	sp, r7
   3307c:	bd80      	pop	{r7, pc}
   3307e:	bf00      	nop
   33080:	000403a4 	.word	0x000403a4

00033084 <test_strcpy>:
   33084:	b580      	push	{r7, lr}
   33086:	b088      	sub	sp, #32
   33088:	af00      	add	r7, sp, #0
   3308a:	f247 4373 	movw	r3, #29811	; 0x7473
   3308e:	60fb      	str	r3, [r7, #12]
   33090:	f107 0310 	add.w	r3, r7, #16
   33094:	2200      	movs	r2, #0
   33096:	601a      	str	r2, [r3, #0]
   33098:	605a      	str	r2, [r3, #4]
   3309a:	609a      	str	r2, [r3, #8]
   3309c:	60da      	str	r2, [r3, #12]
   3309e:	4a0e      	ldr	r2, [pc, #56]	; (330d8 <test_strcpy+0x54>)
   330a0:	1d3b      	adds	r3, r7, #4
   330a2:	e892 0003 	ldmia.w	r2, {r0, r1}
   330a6:	6018      	str	r0, [r3, #0]
   330a8:	3304      	adds	r3, #4
   330aa:	8019      	strh	r1, [r3, #0]
   330ac:	f107 030c 	add.w	r3, r7, #12
   330b0:	4619      	mov	r1, r3
   330b2:	480a      	ldr	r0, [pc, #40]	; (330dc <test_strcpy+0x58>)
   330b4:	f000 fb9e 	bl	337f4 <printf>
   330b8:	1d3a      	adds	r2, r7, #4
   330ba:	f107 030c 	add.w	r3, r7, #12
   330be:	4611      	mov	r1, r2
   330c0:	4618      	mov	r0, r3
   330c2:	f000 fdd7 	bl	33c74 <strcpy>
   330c6:	4603      	mov	r3, r0
   330c8:	4619      	mov	r1, r3
   330ca:	4805      	ldr	r0, [pc, #20]	; (330e0 <test_strcpy+0x5c>)
   330cc:	f000 fb92 	bl	337f4 <printf>
   330d0:	bf00      	nop
   330d2:	3720      	adds	r7, #32
   330d4:	46bd      	mov	sp, r7
   330d6:	bd80      	pop	{r7, pc}
   330d8:	000403b0 	.word	0x000403b0
   330dc:	00040394 	.word	0x00040394
   330e0:	000403a8 	.word	0x000403a8

000330e4 <test_strncat>:
   330e4:	b580      	push	{r7, lr}
   330e6:	b08c      	sub	sp, #48	; 0x30
   330e8:	af00      	add	r7, sp, #0
   330ea:	2373      	movs	r3, #115	; 0x73
   330ec:	613b      	str	r3, [r7, #16]
   330ee:	f107 0314 	add.w	r3, r7, #20
   330f2:	2200      	movs	r2, #0
   330f4:	601a      	str	r2, [r3, #0]
   330f6:	605a      	str	r2, [r3, #4]
   330f8:	609a      	str	r2, [r3, #8]
   330fa:	60da      	str	r2, [r3, #12]
   330fc:	611a      	str	r2, [r3, #16]
   330fe:	615a      	str	r2, [r3, #20]
   33100:	831a      	strh	r2, [r3, #24]
   33102:	4a0d      	ldr	r2, [pc, #52]	; (33138 <test_strncat+0x54>)
   33104:	1d3b      	adds	r3, r7, #4
   33106:	ca07      	ldmia	r2, {r0, r1, r2}
   33108:	c303      	stmia	r3!, {r0, r1}
   3310a:	801a      	strh	r2, [r3, #0]
   3310c:	f107 0310 	add.w	r3, r7, #16
   33110:	4619      	mov	r1, r3
   33112:	480a      	ldr	r0, [pc, #40]	; (3313c <test_strncat+0x58>)
   33114:	f000 fb6e 	bl	337f4 <printf>
   33118:	1d39      	adds	r1, r7, #4
   3311a:	f107 0310 	add.w	r3, r7, #16
   3311e:	220a      	movs	r2, #10
   33120:	4618      	mov	r0, r3
   33122:	f000 fe33 	bl	33d8c <strncat>
   33126:	4603      	mov	r3, r0
   33128:	4619      	mov	r1, r3
   3312a:	4805      	ldr	r0, [pc, #20]	; (33140 <test_strncat+0x5c>)
   3312c:	f000 fb62 	bl	337f4 <printf>
   33130:	bf00      	nop
   33132:	3730      	adds	r7, #48	; 0x30
   33134:	46bd      	mov	sp, r7
   33136:	bd80      	pop	{r7, pc}
   33138:	000403b8 	.word	0x000403b8
   3313c:	00040394 	.word	0x00040394
   33140:	0004039c 	.word	0x0004039c

00033144 <test_strncmp>:
   33144:	b580      	push	{r7, lr}
   33146:	b082      	sub	sp, #8
   33148:	af00      	add	r7, sp, #0
   3314a:	4b09      	ldr	r3, [pc, #36]	; (33170 <test_strncmp+0x2c>)
   3314c:	607b      	str	r3, [r7, #4]
   3314e:	4b09      	ldr	r3, [pc, #36]	; (33174 <test_strncmp+0x30>)
   33150:	603b      	str	r3, [r7, #0]
   33152:	2206      	movs	r2, #6
   33154:	6839      	ldr	r1, [r7, #0]
   33156:	6878      	ldr	r0, [r7, #4]
   33158:	f000 fe39 	bl	33dce <strncmp>
   3315c:	4603      	mov	r3, r0
   3315e:	4619      	mov	r1, r3
   33160:	4805      	ldr	r0, [pc, #20]	; (33178 <test_strncmp+0x34>)
   33162:	f000 fb47 	bl	337f4 <printf>
   33166:	bf00      	nop
   33168:	3708      	adds	r7, #8
   3316a:	46bd      	mov	sp, r7
   3316c:	bd80      	pop	{r7, pc}
   3316e:	bf00      	nop
   33170:	000403c4 	.word	0x000403c4
   33174:	000403c8 	.word	0x000403c8
   33178:	000403cc 	.word	0x000403cc

0003317c <test_strncpy>:
   3317c:	b580      	push	{r7, lr}
   3317e:	b086      	sub	sp, #24
   33180:	af00      	add	r7, sp, #0
   33182:	f243 1373 	movw	r3, #12659	; 0x3173
   33186:	607b      	str	r3, [r7, #4]
   33188:	f107 0308 	add.w	r3, r7, #8
   3318c:	2200      	movs	r2, #0
   3318e:	601a      	str	r2, [r3, #0]
   33190:	605a      	str	r2, [r3, #4]
   33192:	609a      	str	r2, [r3, #8]
   33194:	60da      	str	r2, [r3, #12]
   33196:	4a0e      	ldr	r2, [pc, #56]	; (331d0 <test_strncpy+0x54>)
   33198:	463b      	mov	r3, r7
   3319a:	6812      	ldr	r2, [r2, #0]
   3319c:	4611      	mov	r1, r2
   3319e:	8019      	strh	r1, [r3, #0]
   331a0:	3302      	adds	r3, #2
   331a2:	0c12      	lsrs	r2, r2, #16
   331a4:	701a      	strb	r2, [r3, #0]
   331a6:	1d3b      	adds	r3, r7, #4
   331a8:	4619      	mov	r1, r3
   331aa:	480a      	ldr	r0, [pc, #40]	; (331d4 <test_strncpy+0x58>)
   331ac:	f000 fb22 	bl	337f4 <printf>
   331b0:	4639      	mov	r1, r7
   331b2:	1d3b      	adds	r3, r7, #4
   331b4:	2209      	movs	r2, #9
   331b6:	4618      	mov	r0, r3
   331b8:	f000 fe37 	bl	33e2a <strncpy>
   331bc:	4603      	mov	r3, r0
   331be:	4619      	mov	r1, r3
   331c0:	4805      	ldr	r0, [pc, #20]	; (331d8 <test_strncpy+0x5c>)
   331c2:	f000 fb17 	bl	337f4 <printf>
   331c6:	bf00      	nop
   331c8:	3718      	adds	r7, #24
   331ca:	46bd      	mov	sp, r7
   331cc:	bd80      	pop	{r7, pc}
   331ce:	bf00      	nop
   331d0:	000403e0 	.word	0x000403e0
   331d4:	00040394 	.word	0x00040394
   331d8:	0004039c 	.word	0x0004039c

000331dc <test_strsep>:
   331dc:	b580      	push	{r7, lr}
   331de:	b086      	sub	sp, #24
   331e0:	af00      	add	r7, sp, #0
   331e2:	4a10      	ldr	r2, [pc, #64]	; (33224 <test_strsep+0x48>)
   331e4:	f107 0308 	add.w	r3, r7, #8
   331e8:	e892 0003 	ldmia.w	r2, {r0, r1}
   331ec:	6018      	str	r0, [r3, #0]
   331ee:	3304      	adds	r3, #4
   331f0:	8019      	strh	r1, [r3, #0]
   331f2:	4b0d      	ldr	r3, [pc, #52]	; (33228 <test_strsep+0x4c>)
   331f4:	617b      	str	r3, [r7, #20]
   331f6:	f107 0308 	add.w	r3, r7, #8
   331fa:	607b      	str	r3, [r7, #4]
   331fc:	e003      	b.n	33206 <test_strsep+0x2a>
   331fe:	6939      	ldr	r1, [r7, #16]
   33200:	480a      	ldr	r0, [pc, #40]	; (3322c <test_strsep+0x50>)
   33202:	f000 faf7 	bl	337f4 <printf>
   33206:	1d3b      	adds	r3, r7, #4
   33208:	6979      	ldr	r1, [r7, #20]
   3320a:	4618      	mov	r0, r3
   3320c:	f000 fe51 	bl	33eb2 <strsep>
   33210:	6138      	str	r0, [r7, #16]
   33212:	693b      	ldr	r3, [r7, #16]
   33214:	2b00      	cmp	r3, #0
   33216:	d1f2      	bne.n	331fe <test_strsep+0x22>
   33218:	bf00      	nop
   3321a:	bf00      	nop
   3321c:	3718      	adds	r7, #24
   3321e:	46bd      	mov	sp, r7
   33220:	bd80      	pop	{r7, pc}
   33222:	bf00      	nop
   33224:	000403ec 	.word	0x000403ec
   33228:	000403e4 	.word	0x000403e4
   3322c:	000403e8 	.word	0x000403e8

00033230 <test_strstr>:
   33230:	b580      	push	{r7, lr}
   33232:	b082      	sub	sp, #8
   33234:	af00      	add	r7, sp, #0
   33236:	4b07      	ldr	r3, [pc, #28]	; (33254 <test_strstr+0x24>)
   33238:	607b      	str	r3, [r7, #4]
   3323a:	4907      	ldr	r1, [pc, #28]	; (33258 <test_strstr+0x28>)
   3323c:	6878      	ldr	r0, [r7, #4]
   3323e:	f000 ff4a 	bl	340d6 <strstr>
   33242:	6038      	str	r0, [r7, #0]
   33244:	6838      	ldr	r0, [r7, #0]
   33246:	f000 fb75 	bl	33934 <puts>
   3324a:	bf00      	nop
   3324c:	3708      	adds	r7, #8
   3324e:	46bd      	mov	sp, r7
   33250:	bd80      	pop	{r7, pc}
   33252:	bf00      	nop
   33254:	000403a4 	.word	0x000403a4
   33258:	000403f4 	.word	0x000403f4

0003325c <test_strtok>:
   3325c:	b580      	push	{r7, lr}
   3325e:	b084      	sub	sp, #16
   33260:	af00      	add	r7, sp, #0
   33262:	4a12      	ldr	r2, [pc, #72]	; (332ac <test_strtok+0x50>)
   33264:	463b      	mov	r3, r7
   33266:	e892 0003 	ldmia.w	r2, {r0, r1}
   3326a:	6018      	str	r0, [r3, #0]
   3326c:	3304      	adds	r3, #4
   3326e:	8019      	strh	r1, [r3, #0]
   33270:	4b0f      	ldr	r3, [pc, #60]	; (332b0 <test_strtok+0x54>)
   33272:	60fb      	str	r3, [r7, #12]
   33274:	463b      	mov	r3, r7
   33276:	68f9      	ldr	r1, [r7, #12]
   33278:	4618      	mov	r0, r3
   3327a:	f000 ffcb 	bl	34214 <strtok>
   3327e:	4603      	mov	r3, r0
   33280:	4619      	mov	r1, r3
   33282:	480c      	ldr	r0, [pc, #48]	; (332b4 <test_strtok+0x58>)
   33284:	f000 fab6 	bl	337f4 <printf>
   33288:	e003      	b.n	33292 <test_strtok+0x36>
   3328a:	68b9      	ldr	r1, [r7, #8]
   3328c:	4809      	ldr	r0, [pc, #36]	; (332b4 <test_strtok+0x58>)
   3328e:	f000 fab1 	bl	337f4 <printf>
   33292:	68f9      	ldr	r1, [r7, #12]
   33294:	2000      	movs	r0, #0
   33296:	f000 ffbd 	bl	34214 <strtok>
   3329a:	60b8      	str	r0, [r7, #8]
   3329c:	68bb      	ldr	r3, [r7, #8]
   3329e:	2b00      	cmp	r3, #0
   332a0:	d1f3      	bne.n	3328a <test_strtok+0x2e>
   332a2:	bf00      	nop
   332a4:	bf00      	nop
   332a6:	3710      	adds	r7, #16
   332a8:	46bd      	mov	sp, r7
   332aa:	bd80      	pop	{r7, pc}
   332ac:	000403f8 	.word	0x000403f8
   332b0:	000403e4 	.word	0x000403e4
   332b4:	000403e8 	.word	0x000403e8

000332b8 <test_strpbrk>:
   332b8:	b580      	push	{r7, lr}
   332ba:	b084      	sub	sp, #16
   332bc:	af00      	add	r7, sp, #0
   332be:	4a15      	ldr	r2, [pc, #84]	; (33314 <test_strpbrk+0x5c>)
   332c0:	f107 0308 	add.w	r3, r7, #8
   332c4:	6812      	ldr	r2, [r2, #0]
   332c6:	4611      	mov	r1, r2
   332c8:	8019      	strh	r1, [r3, #0]
   332ca:	3302      	adds	r3, #2
   332cc:	0c12      	lsrs	r2, r2, #16
   332ce:	701a      	strb	r2, [r3, #0]
   332d0:	4a11      	ldr	r2, [pc, #68]	; (33318 <test_strpbrk+0x60>)
   332d2:	1d3b      	adds	r3, r7, #4
   332d4:	6812      	ldr	r2, [r2, #0]
   332d6:	4611      	mov	r1, r2
   332d8:	8019      	strh	r1, [r3, #0]
   332da:	3302      	adds	r3, #2
   332dc:	0c12      	lsrs	r2, r2, #16
   332de:	701a      	strb	r2, [r3, #0]
   332e0:	1d3a      	adds	r2, r7, #4
   332e2:	f107 0308 	add.w	r3, r7, #8
   332e6:	4611      	mov	r1, r2
   332e8:	4618      	mov	r0, r3
   332ea:	f000 fdc8 	bl	33e7e <strpbrk>
   332ee:	60f8      	str	r0, [r7, #12]
   332f0:	68fb      	ldr	r3, [r7, #12]
   332f2:	2b00      	cmp	r3, #0
   332f4:	d006      	beq.n	33304 <test_strpbrk+0x4c>
   332f6:	68fb      	ldr	r3, [r7, #12]
   332f8:	781b      	ldrb	r3, [r3, #0]
   332fa:	4619      	mov	r1, r3
   332fc:	4807      	ldr	r0, [pc, #28]	; (3331c <test_strpbrk+0x64>)
   332fe:	f000 fa79 	bl	337f4 <printf>
   33302:	e002      	b.n	3330a <test_strpbrk+0x52>
   33304:	4806      	ldr	r0, [pc, #24]	; (33320 <test_strpbrk+0x68>)
   33306:	f000 fb15 	bl	33934 <puts>
   3330a:	bf00      	nop
   3330c:	3710      	adds	r7, #16
   3330e:	46bd      	mov	sp, r7
   33310:	bd80      	pop	{r7, pc}
   33312:	bf00      	nop
   33314:	0004040c 	.word	0x0004040c
   33318:	00040410 	.word	0x00040410
   3331c:	00040400 	.word	0x00040400
   33320:	00040408 	.word	0x00040408

00033324 <test_stratoi>:
   33324:	b580      	push	{r7, lr}
   33326:	b082      	sub	sp, #8
   33328:	af00      	add	r7, sp, #0
   3332a:	4a0b      	ldr	r2, [pc, #44]	; (33358 <test_stratoi+0x34>)
   3332c:	463b      	mov	r3, r7
   3332e:	6812      	ldr	r2, [r2, #0]
   33330:	4611      	mov	r1, r2
   33332:	8019      	strh	r1, [r3, #0]
   33334:	3302      	adds	r3, #2
   33336:	0c12      	lsrs	r2, r2, #16
   33338:	701a      	strb	r2, [r3, #0]
   3333a:	463b      	mov	r3, r7
   3333c:	4618      	mov	r0, r3
   3333e:	f000 fa43 	bl	337c8 <atoi>
   33342:	4603      	mov	r3, r0
   33344:	607b      	str	r3, [r7, #4]
   33346:	687b      	ldr	r3, [r7, #4]
   33348:	4619      	mov	r1, r3
   3334a:	4804      	ldr	r0, [pc, #16]	; (3335c <test_stratoi+0x38>)
   3334c:	f000 fa52 	bl	337f4 <printf>
   33350:	bf00      	nop
   33352:	3708      	adds	r7, #8
   33354:	46bd      	mov	sp, r7
   33356:	bd80      	pop	{r7, pc}
   33358:	0004040c 	.word	0x0004040c
   3335c:	00040414 	.word	0x00040414

00033360 <test_strtol>:
   33360:	b580      	push	{r7, lr}
   33362:	b08a      	sub	sp, #40	; 0x28
   33364:	af00      	add	r7, sp, #0
   33366:	4a14      	ldr	r2, [pc, #80]	; (333b8 <test_strtol+0x58>)
   33368:	1d3b      	adds	r3, r7, #4
   3336a:	e892 0003 	ldmia.w	r2, {r0, r1}
   3336e:	6018      	str	r0, [r3, #0]
   33370:	3304      	adds	r3, #4
   33372:	8019      	strh	r1, [r3, #0]
   33374:	3302      	adds	r3, #2
   33376:	0c0a      	lsrs	r2, r1, #16
   33378:	701a      	strb	r2, [r3, #0]
   3337a:	f107 030b 	add.w	r3, r7, #11
   3337e:	2200      	movs	r2, #0
   33380:	601a      	str	r2, [r3, #0]
   33382:	605a      	str	r2, [r3, #4]
   33384:	609a      	str	r2, [r3, #8]
   33386:	60da      	str	r2, [r3, #12]
   33388:	611a      	str	r2, [r3, #16]
   3338a:	f8c3 2013 	str.w	r2, [r3, #19]
   3338e:	4639      	mov	r1, r7
   33390:	1d3b      	adds	r3, r7, #4
   33392:	220a      	movs	r2, #10
   33394:	4618      	mov	r0, r3
   33396:	f000 fffd 	bl	34394 <strtol>
   3339a:	6278      	str	r0, [r7, #36]	; 0x24
   3339c:	6a79      	ldr	r1, [r7, #36]	; 0x24
   3339e:	4807      	ldr	r0, [pc, #28]	; (333bc <test_strtol+0x5c>)
   333a0:	f000 fa28 	bl	337f4 <printf>
   333a4:	683b      	ldr	r3, [r7, #0]
   333a6:	4619      	mov	r1, r3
   333a8:	4805      	ldr	r0, [pc, #20]	; (333c0 <test_strtol+0x60>)
   333aa:	f000 fa23 	bl	337f4 <printf>
   333ae:	bf00      	nop
   333b0:	3728      	adds	r7, #40	; 0x28
   333b2:	46bd      	mov	sp, r7
   333b4:	bd80      	pop	{r7, pc}
   333b6:	bf00      	nop
   333b8:	00040438 	.word	0x00040438
   333bc:	00040420 	.word	0x00040420
   333c0:	0004042c 	.word	0x0004042c

000333c4 <_write>:
   333c4:	b580      	push	{r7, lr}
   333c6:	b086      	sub	sp, #24
   333c8:	af00      	add	r7, sp, #0
   333ca:	60f8      	str	r0, [r7, #12]
   333cc:	60b9      	str	r1, [r7, #8]
   333ce:	607a      	str	r2, [r7, #4]
   333d0:	2300      	movs	r3, #0
   333d2:	617b      	str	r3, [r7, #20]
   333d4:	4b15      	ldr	r3, [pc, #84]	; (3342c <_write+0x68>)
   333d6:	613b      	str	r3, [r7, #16]
   333d8:	68fb      	ldr	r3, [r7, #12]
   333da:	2b01      	cmp	r3, #1
   333dc:	d005      	beq.n	333ea <_write+0x26>
   333de:	68fb      	ldr	r3, [r7, #12]
   333e0:	2b02      	cmp	r3, #2
   333e2:	d002      	beq.n	333ea <_write+0x26>
   333e4:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   333e8:	e01c      	b.n	33424 <_write+0x60>
   333ea:	4b11      	ldr	r3, [pc, #68]	; (33430 <_write+0x6c>)
   333ec:	681b      	ldr	r3, [r3, #0]
   333ee:	2b00      	cmp	r3, #0
   333f0:	d014      	beq.n	3341c <_write+0x58>
   333f2:	4b0f      	ldr	r3, [pc, #60]	; (33430 <_write+0x6c>)
   333f4:	681b      	ldr	r3, [r3, #0]
   333f6:	6879      	ldr	r1, [r7, #4]
   333f8:	68b8      	ldr	r0, [r7, #8]
   333fa:	4798      	blx	r3
   333fc:	6178      	str	r0, [r7, #20]
   333fe:	e010      	b.n	33422 <_write+0x5e>
   33400:	693b      	ldr	r3, [r7, #16]
   33402:	685a      	ldr	r2, [r3, #4]
   33404:	68bb      	ldr	r3, [r7, #8]
   33406:	1c59      	adds	r1, r3, #1
   33408:	60b9      	str	r1, [r7, #8]
   3340a:	781b      	ldrb	r3, [r3, #0]
   3340c:	4618      	mov	r0, r3
   3340e:	4790      	blx	r2
   33410:	697b      	ldr	r3, [r7, #20]
   33412:	3301      	adds	r3, #1
   33414:	617b      	str	r3, [r7, #20]
   33416:	687b      	ldr	r3, [r7, #4]
   33418:	3b01      	subs	r3, #1
   3341a:	607b      	str	r3, [r7, #4]
   3341c:	687b      	ldr	r3, [r7, #4]
   3341e:	2b00      	cmp	r3, #0
   33420:	dcee      	bgt.n	33400 <_write+0x3c>
   33422:	697b      	ldr	r3, [r7, #20]
   33424:	4618      	mov	r0, r3
   33426:	3718      	adds	r7, #24
   33428:	46bd      	mov	sp, r7
   3342a:	bd80      	pop	{r7, pc}
   3342c:	00042000 	.word	0x00042000
   33430:	200069c0 	.word	0x200069c0

00033434 <_read>:
   33434:	b580      	push	{r7, lr}
   33436:	b088      	sub	sp, #32
   33438:	af00      	add	r7, sp, #0
   3343a:	60f8      	str	r0, [r7, #12]
   3343c:	60b9      	str	r1, [r7, #8]
   3343e:	607a      	str	r2, [r7, #4]
   33440:	2300      	movs	r3, #0
   33442:	61fb      	str	r3, [r7, #28]
   33444:	4b1b      	ldr	r3, [pc, #108]	; (334b4 <_read+0x80>)
   33446:	61bb      	str	r3, [r7, #24]
   33448:	68fb      	ldr	r3, [r7, #12]
   3344a:	2b00      	cmp	r3, #0
   3344c:	d002      	beq.n	33454 <_read+0x20>
   3344e:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   33452:	e02a      	b.n	334aa <_read+0x76>
   33454:	4b18      	ldr	r3, [pc, #96]	; (334b8 <_read+0x84>)
   33456:	685b      	ldr	r3, [r3, #4]
   33458:	2b00      	cmp	r3, #0
   3345a:	d022      	beq.n	334a2 <_read+0x6e>
   3345c:	4b16      	ldr	r3, [pc, #88]	; (334b8 <_read+0x84>)
   3345e:	685b      	ldr	r3, [r3, #4]
   33460:	6879      	ldr	r1, [r7, #4]
   33462:	68b8      	ldr	r0, [r7, #8]
   33464:	4798      	blx	r3
   33466:	61f8      	str	r0, [r7, #28]
   33468:	e01e      	b.n	334a8 <_read+0x74>
   3346a:	69bb      	ldr	r3, [r7, #24]
   3346c:	689b      	ldr	r3, [r3, #8]
   3346e:	2001      	movs	r0, #1
   33470:	4798      	blx	r3
   33472:	4603      	mov	r3, r0
   33474:	617b      	str	r3, [r7, #20]
   33476:	697b      	ldr	r3, [r7, #20]
   33478:	2b00      	cmp	r3, #0
   3347a:	db02      	blt.n	33482 <_read+0x4e>
   3347c:	697b      	ldr	r3, [r7, #20]
   3347e:	2b0d      	cmp	r3, #13
   33480:	d103      	bne.n	3348a <_read+0x56>
   33482:	68bb      	ldr	r3, [r7, #8]
   33484:	2200      	movs	r2, #0
   33486:	701a      	strb	r2, [r3, #0]
   33488:	e00e      	b.n	334a8 <_read+0x74>
   3348a:	68bb      	ldr	r3, [r7, #8]
   3348c:	1c5a      	adds	r2, r3, #1
   3348e:	60ba      	str	r2, [r7, #8]
   33490:	697a      	ldr	r2, [r7, #20]
   33492:	b2d2      	uxtb	r2, r2
   33494:	701a      	strb	r2, [r3, #0]
   33496:	69fb      	ldr	r3, [r7, #28]
   33498:	3301      	adds	r3, #1
   3349a:	61fb      	str	r3, [r7, #28]
   3349c:	687b      	ldr	r3, [r7, #4]
   3349e:	3b01      	subs	r3, #1
   334a0:	607b      	str	r3, [r7, #4]
   334a2:	687b      	ldr	r3, [r7, #4]
   334a4:	2b00      	cmp	r3, #0
   334a6:	dce0      	bgt.n	3346a <_read+0x36>
   334a8:	69fb      	ldr	r3, [r7, #28]
   334aa:	4618      	mov	r0, r3
   334ac:	3720      	adds	r7, #32
   334ae:	46bd      	mov	sp, r7
   334b0:	bd80      	pop	{r7, pc}
   334b2:	bf00      	nop
   334b4:	00042000 	.word	0x00042000
   334b8:	200069c0 	.word	0x200069c0

000334bc <rtl_string_test>:
   334bc:	b508      	push	{r3, lr}
   334be:	f7ff fda3 	bl	33008 <test_strcat>
   334c2:	f7ff fdcb 	bl	3305c <test_strchr>
   334c6:	f7ff fddd 	bl	33084 <test_strcpy>
   334ca:	f7ff fe0b 	bl	330e4 <test_strncat>
   334ce:	f7ff fe39 	bl	33144 <test_strncmp>
   334d2:	f7ff fe53 	bl	3317c <test_strncpy>
   334d6:	f7ff fe81 	bl	331dc <test_strsep>
   334da:	f7ff fea9 	bl	33230 <test_strstr>
   334de:	f7ff febd 	bl	3325c <test_strtok>
   334e2:	f7ff fee9 	bl	332b8 <test_strpbrk>
   334e6:	f7ff ff1d 	bl	33324 <test_stratoi>
   334ea:	f7ff ff39 	bl	33360 <test_strtol>
   334ee:	2000      	movs	r0, #0
   334f0:	bd08      	pop	{r3, pc}
   334f2:	bf00      	nop

000334f4 <stdlib_vsnprintf_call>:
   334f4:	b40f      	push	{r0, r1, r2, r3}
   334f6:	b500      	push	{lr}
   334f8:	b085      	sub	sp, #20
   334fa:	2106      	movs	r1, #6
   334fc:	ab06      	add	r3, sp, #24
   334fe:	a802      	add	r0, sp, #8
   33500:	f853 2b04 	ldr.w	r2, [r3], #4
   33504:	9301      	str	r3, [sp, #4]
   33506:	f004 fd66 	bl	37fd6 <vsnprintf>
   3350a:	b005      	add	sp, #20
   3350c:	f85d eb04 	ldr.w	lr, [sp], #4
   33510:	b004      	add	sp, #16
   33512:	4770      	bx	lr

00033514 <stdlib_printf_call>:
   33514:	e92d 4170 	stmdb	sp!, {r4, r5, r6, r8, lr}
   33518:	4d14      	ldr	r5, [pc, #80]	; (3356c <stdlib_printf_call+0x58>)
   3351a:	4606      	mov	r6, r0
   3351c:	b087      	sub	sp, #28
   3351e:	2400      	movs	r4, #0
   33520:	4628      	mov	r0, r5
   33522:	4688      	mov	r8, r1
   33524:	f000 fa06 	bl	33934 <puts>
   33528:	4631      	mov	r1, r6
   3352a:	f105 0010 	add.w	r0, r5, #16
   3352e:	f000 f961 	bl	337f4 <printf>
   33532:	2078      	movs	r0, #120	; 0x78
   33534:	f000 f973 	bl	3381e <putchar>
   33538:	4622      	mov	r2, r4
   3353a:	f105 0114 	add.w	r1, r5, #20
   3353e:	4623      	mov	r3, r4
   33540:	4640      	mov	r0, r8
   33542:	e9cd 4402 	strd	r4, r4, [sp, #8]
   33546:	e9cd 4400 	strd	r4, r4, [sp]
   3354a:	f000 fa79 	bl	33a40 <sprintf>
   3354e:	4633      	mov	r3, r6
   33550:	f105 0230 	add.w	r2, r5, #48	; 0x30
   33554:	2106      	movs	r1, #6
   33556:	a804      	add	r0, sp, #16
   33558:	f000 fa23 	bl	339a2 <snprintf>
   3355c:	4621      	mov	r1, r4
   3355e:	f105 0038 	add.w	r0, r5, #56	; 0x38
   33562:	f7ff ffc7 	bl	334f4 <stdlib_vsnprintf_call>
   33566:	b007      	add	sp, #28
   33568:	e8bd 8170 	ldmia.w	sp!, {r4, r5, r6, r8, pc}
   3356c:	00040458 	.word	0x00040458

00033570 <stdlib_scanf_call>:
   33570:	b530      	push	{r4, r5, lr}
   33572:	b093      	sub	sp, #76	; 0x4c
   33574:	2000      	movs	r0, #0
   33576:	4c11      	ldr	r4, [pc, #68]	; (335bc <stdlib_scanf_call+0x4c>)
   33578:	ad0b      	add	r5, sp, #44	; 0x2c
   3357a:	ab04      	add	r3, sp, #16
   3357c:	f10d 020f 	add.w	r2, sp, #15
   33580:	f104 0140 	add.w	r1, r4, #64	; 0x40
   33584:	f88d 000f 	strb.w	r0, [sp, #15]
   33588:	900a      	str	r0, [sp, #40]	; 0x28
   3358a:	9011      	str	r0, [sp, #68]	; 0x44
   3358c:	9500      	str	r5, [sp, #0]
   3358e:	e9cd 0004 	strd	r0, r0, [sp, #16]
   33592:	e9cd 000b 	strd	r0, r0, [sp, #44]	; 0x2c
   33596:	e9cd 0006 	strd	r0, r0, [sp, #24]
   3359a:	e9cd 0008 	strd	r0, r0, [sp, #32]
   3359e:	e9cd 000d 	strd	r0, r0, [sp, #52]	; 0x34
   335a2:	e9cd 000f 	strd	r0, r0, [sp, #60]	; 0x3c
   335a6:	f104 0050 	add.w	r0, r4, #80	; 0x50
   335aa:	f000 fa69 	bl	33a80 <sscanf>
   335ae:	4601      	mov	r1, r0
   335b0:	f104 0074 	add.w	r0, r4, #116	; 0x74
   335b4:	f000 f91e 	bl	337f4 <printf>
   335b8:	b013      	add	sp, #76	; 0x4c
   335ba:	bd30      	pop	{r4, r5, pc}
   335bc:	00040458 	.word	0x00040458

000335c0 <stdlib_test>:
   335c0:	2100      	movs	r1, #0
   335c2:	b508      	push	{r3, lr}
   335c4:	4608      	mov	r0, r1
   335c6:	f7ff ffa5 	bl	33514 <stdlib_printf_call>
   335ca:	f7ff ffd1 	bl	33570 <stdlib_scanf_call>
   335ce:	f7ff fd1b 	bl	33008 <test_strcat>
   335d2:	f7ff fd43 	bl	3305c <test_strchr>
   335d6:	f7ff fd55 	bl	33084 <test_strcpy>
   335da:	f7ff fd83 	bl	330e4 <test_strncat>
   335de:	f7ff fdb1 	bl	33144 <test_strncmp>
   335e2:	f7ff fdcb 	bl	3317c <test_strncpy>
   335e6:	f7ff fdf9 	bl	331dc <test_strsep>
   335ea:	f7ff fe21 	bl	33230 <test_strstr>
   335ee:	f7ff fe35 	bl	3325c <test_strtok>
   335f2:	f7ff fe61 	bl	332b8 <test_strpbrk>
   335f6:	f7ff fe95 	bl	33324 <test_stratoi>
   335fa:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
   335fe:	f7ff beaf 	b.w	33360 <test_strtol>
   33602:	bf00      	nop

00033604 <stdlib_memset>:
   33604:	b570      	push	{r4, r5, r6, lr}
   33606:	0786      	lsls	r6, r0, #30
   33608:	d048      	beq.n	3369c <stdlib_memset+0x98>
   3360a:	1e54      	subs	r4, r2, #1
   3360c:	2a00      	cmp	r2, #0
   3360e:	d03f      	beq.n	33690 <stdlib_memset+0x8c>
   33610:	b2ca      	uxtb	r2, r1
   33612:	4603      	mov	r3, r0
   33614:	e001      	b.n	3361a <stdlib_memset+0x16>
   33616:	3c01      	subs	r4, #1
   33618:	d33a      	bcc.n	33690 <stdlib_memset+0x8c>
   3361a:	f803 2b01 	strb.w	r2, [r3], #1
   3361e:	079d      	lsls	r5, r3, #30
   33620:	d1f9      	bne.n	33616 <stdlib_memset+0x12>
   33622:	2c03      	cmp	r4, #3
   33624:	d92d      	bls.n	33682 <stdlib_memset+0x7e>
   33626:	b2cd      	uxtb	r5, r1
   33628:	2c0f      	cmp	r4, #15
   3362a:	ea45 2505 	orr.w	r5, r5, r5, lsl #8
   3362e:	ea45 4505 	orr.w	r5, r5, r5, lsl #16
   33632:	d936      	bls.n	336a2 <stdlib_memset+0x9e>
   33634:	f1a4 0c10 	sub.w	ip, r4, #16
   33638:	f103 0e20 	add.w	lr, r3, #32
   3363c:	f103 0210 	add.w	r2, r3, #16
   33640:	f02c 060f 	bic.w	r6, ip, #15
   33644:	ea4f 1c1c 	mov.w	ip, ip, lsr #4
   33648:	44b6      	add	lr, r6
   3364a:	e942 5504 	strd	r5, r5, [r2, #-16]
   3364e:	e942 5502 	strd	r5, r5, [r2, #-8]
   33652:	3210      	adds	r2, #16
   33654:	4572      	cmp	r2, lr
   33656:	d1f8      	bne.n	3364a <stdlib_memset+0x46>
   33658:	f10c 0201 	add.w	r2, ip, #1
   3365c:	f014 0f0c 	tst.w	r4, #12
   33660:	f004 0c0f 	and.w	ip, r4, #15
   33664:	eb03 1202 	add.w	r2, r3, r2, lsl #4
   33668:	d013      	beq.n	33692 <stdlib_memset+0x8e>
   3366a:	f1ac 0304 	sub.w	r3, ip, #4
   3366e:	f023 0303 	bic.w	r3, r3, #3
   33672:	3304      	adds	r3, #4
   33674:	4413      	add	r3, r2
   33676:	f842 5b04 	str.w	r5, [r2], #4
   3367a:	4293      	cmp	r3, r2
   3367c:	d1fb      	bne.n	33676 <stdlib_memset+0x72>
   3367e:	f00c 0403 	and.w	r4, ip, #3
   33682:	b12c      	cbz	r4, 33690 <stdlib_memset+0x8c>
   33684:	b2ca      	uxtb	r2, r1
   33686:	441c      	add	r4, r3
   33688:	f803 2b01 	strb.w	r2, [r3], #1
   3368c:	429c      	cmp	r4, r3
   3368e:	d1fb      	bne.n	33688 <stdlib_memset+0x84>
   33690:	bd70      	pop	{r4, r5, r6, pc}
   33692:	4664      	mov	r4, ip
   33694:	4613      	mov	r3, r2
   33696:	2c00      	cmp	r4, #0
   33698:	d1f4      	bne.n	33684 <stdlib_memset+0x80>
   3369a:	e7f9      	b.n	33690 <stdlib_memset+0x8c>
   3369c:	4603      	mov	r3, r0
   3369e:	4614      	mov	r4, r2
   336a0:	e7bf      	b.n	33622 <stdlib_memset+0x1e>
   336a2:	461a      	mov	r2, r3
   336a4:	46a4      	mov	ip, r4
   336a6:	e7e0      	b.n	3366a <stdlib_memset+0x66>

000336a8 <stdlib_memcpy>:
   336a8:	2a03      	cmp	r2, #3
   336aa:	d954      	bls.n	33756 <stdlib_memcpy+0xae>
   336ac:	ea40 0301 	orr.w	r3, r0, r1
   336b0:	079b      	lsls	r3, r3, #30
   336b2:	d15f      	bne.n	33774 <stdlib_memcpy+0xcc>
   336b4:	2a0f      	cmp	r2, #15
   336b6:	b570      	push	{r4, r5, r6, lr}
   336b8:	d962      	bls.n	33780 <stdlib_memcpy+0xd8>
   336ba:	f1a2 0310 	sub.w	r3, r2, #16
   336be:	f101 0c20 	add.w	ip, r1, #32
   336c2:	f101 0e10 	add.w	lr, r1, #16
   336c6:	f100 0410 	add.w	r4, r0, #16
   336ca:	f023 050f 	bic.w	r5, r3, #15
   336ce:	44ac      	add	ip, r5
   336d0:	091d      	lsrs	r5, r3, #4
   336d2:	f85e 3c10 	ldr.w	r3, [lr, #-16]
   336d6:	f10e 0e10 	add.w	lr, lr, #16
   336da:	3410      	adds	r4, #16
   336dc:	f844 3c20 	str.w	r3, [r4, #-32]
   336e0:	45e6      	cmp	lr, ip
   336e2:	f85e 3c1c 	ldr.w	r3, [lr, #-28]
   336e6:	f844 3c1c 	str.w	r3, [r4, #-28]
   336ea:	f85e 3c18 	ldr.w	r3, [lr, #-24]
   336ee:	f844 3c18 	str.w	r3, [r4, #-24]
   336f2:	f85e 3c14 	ldr.w	r3, [lr, #-20]
   336f6:	f844 3c14 	str.w	r3, [r4, #-20]
   336fa:	d1ea      	bne.n	336d2 <stdlib_memcpy+0x2a>
   336fc:	1c6b      	adds	r3, r5, #1
   336fe:	f012 0f0c 	tst.w	r2, #12
   33702:	f002 0c0f 	and.w	ip, r2, #15
   33706:	eb01 1103 	add.w	r1, r1, r3, lsl #4
   3370a:	eb00 1303 	add.w	r3, r0, r3, lsl #4
   3370e:	d035      	beq.n	3377c <stdlib_memcpy+0xd4>
   33710:	f1ac 0404 	sub.w	r4, ip, #4
   33714:	1f1a      	subs	r2, r3, #4
   33716:	460d      	mov	r5, r1
   33718:	f024 0e03 	bic.w	lr, r4, #3
   3371c:	08a4      	lsrs	r4, r4, #2
   3371e:	449e      	add	lr, r3
   33720:	f855 6b04 	ldr.w	r6, [r5], #4
   33724:	f842 6f04 	str.w	r6, [r2, #4]!
   33728:	4572      	cmp	r2, lr
   3372a:	d1f9      	bne.n	33720 <stdlib_memcpy+0x78>
   3372c:	3401      	adds	r4, #1
   3372e:	f00c 0203 	and.w	r2, ip, #3
   33732:	eb03 0384 	add.w	r3, r3, r4, lsl #2
   33736:	eb01 0184 	add.w	r1, r1, r4, lsl #2
   3373a:	f102 3cff 	add.w	ip, r2, #4294967295	; 0xffffffff
   3373e:	b14a      	cbz	r2, 33754 <stdlib_memcpy+0xac>
   33740:	f10c 0c01 	add.w	ip, ip, #1
   33744:	3b01      	subs	r3, #1
   33746:	448c      	add	ip, r1
   33748:	f811 2b01 	ldrb.w	r2, [r1], #1
   3374c:	4561      	cmp	r1, ip
   3374e:	f803 2f01 	strb.w	r2, [r3, #1]!
   33752:	d1f9      	bne.n	33748 <stdlib_memcpy+0xa0>
   33754:	bd70      	pop	{r4, r5, r6, pc}
   33756:	4603      	mov	r3, r0
   33758:	f102 3cff 	add.w	ip, r2, #4294967295	; 0xffffffff
   3375c:	b14a      	cbz	r2, 33772 <stdlib_memcpy+0xca>
   3375e:	f10c 0c01 	add.w	ip, ip, #1
   33762:	3b01      	subs	r3, #1
   33764:	448c      	add	ip, r1
   33766:	f811 2b01 	ldrb.w	r2, [r1], #1
   3376a:	4561      	cmp	r1, ip
   3376c:	f803 2f01 	strb.w	r2, [r3, #1]!
   33770:	d1f9      	bne.n	33766 <stdlib_memcpy+0xbe>
   33772:	4770      	bx	lr
   33774:	f102 3cff 	add.w	ip, r2, #4294967295	; 0xffffffff
   33778:	4603      	mov	r3, r0
   3377a:	e7f0      	b.n	3375e <stdlib_memcpy+0xb6>
   3377c:	4662      	mov	r2, ip
   3377e:	e7dc      	b.n	3373a <stdlib_memcpy+0x92>
   33780:	4603      	mov	r3, r0
   33782:	4694      	mov	ip, r2
   33784:	e7c4      	b.n	33710 <stdlib_memcpy+0x68>
   33786:	bf00      	nop

00033788 <stdlib_init>:
   33788:	4807      	ldr	r0, [pc, #28]	; (337a8 <stdlib_init+0x20>)
   3378a:	2100      	movs	r1, #0
   3378c:	4a07      	ldr	r2, [pc, #28]	; (337ac <stdlib_init+0x24>)
   3378e:	b508      	push	{r3, lr}
   33790:	1a12      	subs	r2, r2, r0
   33792:	f7ff ff37 	bl	33604 <stdlib_memset>
   33796:	4806      	ldr	r0, [pc, #24]	; (337b0 <stdlib_init+0x28>)
   33798:	4a06      	ldr	r2, [pc, #24]	; (337b4 <stdlib_init+0x2c>)
   3379a:	4907      	ldr	r1, [pc, #28]	; (337b8 <stdlib_init+0x30>)
   3379c:	1a12      	subs	r2, r2, r0
   3379e:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
   337a2:	f7ff bf81 	b.w	336a8 <stdlib_memcpy>
   337a6:	bf00      	nop
   337a8:	200069c0 	.word	0x200069c0
   337ac:	20006a14 	.word	0x20006a14
   337b0:	20006000 	.word	0x20006000
   337b4:	200069ae 	.word	0x200069ae
   337b8:	00040a40 	.word	0x00040a40

000337bc <stdlib_loguart_relocation>:
   337bc:	4b01      	ldr	r3, [pc, #4]	; (337c4 <stdlib_loguart_relocation+0x8>)
   337be:	e9c3 1000 	strd	r1, r0, [r3]
   337c2:	4770      	bx	lr
   337c4:	200069c0 	.word	0x200069c0

000337c8 <atoi>:
   337c8:	220a      	movs	r2, #10
   337ca:	2100      	movs	r1, #0
   337cc:	f000 bde2 	b.w	34394 <strtol>

000337d0 <_atoi_r>:
   337d0:	230a      	movs	r3, #10
   337d2:	2200      	movs	r2, #0
   337d4:	f000 bdd4 	b.w	34380 <_strtol_r>

000337d8 <_printf_r>:
   337d8:	b40e      	push	{r1, r2, r3}
   337da:	b503      	push	{r0, r1, lr}
   337dc:	ab03      	add	r3, sp, #12
   337de:	6881      	ldr	r1, [r0, #8]
   337e0:	f853 2b04 	ldr.w	r2, [r3], #4
   337e4:	9301      	str	r3, [sp, #4]
   337e6:	f003 f94f 	bl	36a88 <_vfprintf_r>
   337ea:	b002      	add	sp, #8
   337ec:	f85d eb04 	ldr.w	lr, [sp], #4
   337f0:	b003      	add	sp, #12
   337f2:	4770      	bx	lr

000337f4 <printf>:
   337f4:	b40f      	push	{r0, r1, r2, r3}
   337f6:	b507      	push	{r0, r1, r2, lr}
   337f8:	4906      	ldr	r1, [pc, #24]	; (33814 <printf+0x20>)
   337fa:	ab04      	add	r3, sp, #16
   337fc:	6808      	ldr	r0, [r1, #0]
   337fe:	f853 2b04 	ldr.w	r2, [r3], #4
   33802:	6881      	ldr	r1, [r0, #8]
   33804:	9301      	str	r3, [sp, #4]
   33806:	f003 f93f 	bl	36a88 <_vfprintf_r>
   3380a:	b003      	add	sp, #12
   3380c:	f85d eb04 	ldr.w	lr, [sp], #4
   33810:	b004      	add	sp, #16
   33812:	4770      	bx	lr
   33814:	20006000 	.word	0x20006000

00033818 <_putchar_r>:
   33818:	6882      	ldr	r2, [r0, #8]
   3381a:	f000 b809 	b.w	33830 <_putc_r>

0003381e <putchar>:
   3381e:	4b03      	ldr	r3, [pc, #12]	; (3382c <putchar+0xe>)
   33820:	4601      	mov	r1, r0
   33822:	6818      	ldr	r0, [r3, #0]
   33824:	6882      	ldr	r2, [r0, #8]
   33826:	f000 b803 	b.w	33830 <_putc_r>
   3382a:	bf00      	nop
   3382c:	20006000 	.word	0x20006000

00033830 <_putc_r>:
   33830:	b570      	push	{r4, r5, r6, lr}
   33832:	460d      	mov	r5, r1
   33834:	4614      	mov	r4, r2
   33836:	4606      	mov	r6, r0
   33838:	b118      	cbz	r0, 33842 <_putc_r+0x12>
   3383a:	6b83      	ldr	r3, [r0, #56]	; 0x38
   3383c:	b90b      	cbnz	r3, 33842 <_putc_r+0x12>
   3383e:	f005 fc49 	bl	390d4 <__sinit>
   33842:	6e63      	ldr	r3, [r4, #100]	; 0x64
   33844:	07d8      	lsls	r0, r3, #31
   33846:	d405      	bmi.n	33854 <_putc_r+0x24>
   33848:	89a3      	ldrh	r3, [r4, #12]
   3384a:	0599      	lsls	r1, r3, #22
   3384c:	d402      	bmi.n	33854 <_putc_r+0x24>
   3384e:	6da0      	ldr	r0, [r4, #88]	; 0x58
   33850:	f005 ffa9 	bl	397a6 <__retarget_lock_acquire_recursive>
   33854:	68a3      	ldr	r3, [r4, #8]
   33856:	3b01      	subs	r3, #1
   33858:	2b00      	cmp	r3, #0
   3385a:	60a3      	str	r3, [r4, #8]
   3385c:	da05      	bge.n	3386a <_putc_r+0x3a>
   3385e:	69a2      	ldr	r2, [r4, #24]
   33860:	4293      	cmp	r3, r2
   33862:	db12      	blt.n	3388a <_putc_r+0x5a>
   33864:	b2eb      	uxtb	r3, r5
   33866:	2b0a      	cmp	r3, #10
   33868:	d00f      	beq.n	3388a <_putc_r+0x5a>
   3386a:	6823      	ldr	r3, [r4, #0]
   3386c:	1c5a      	adds	r2, r3, #1
   3386e:	6022      	str	r2, [r4, #0]
   33870:	701d      	strb	r5, [r3, #0]
   33872:	b2ed      	uxtb	r5, r5
   33874:	6e63      	ldr	r3, [r4, #100]	; 0x64
   33876:	07da      	lsls	r2, r3, #31
   33878:	d405      	bmi.n	33886 <_putc_r+0x56>
   3387a:	89a3      	ldrh	r3, [r4, #12]
   3387c:	059b      	lsls	r3, r3, #22
   3387e:	d402      	bmi.n	33886 <_putc_r+0x56>
   33880:	6da0      	ldr	r0, [r4, #88]	; 0x58
   33882:	f005 ff96 	bl	397b2 <__retarget_lock_release_recursive>
   33886:	4628      	mov	r0, r5
   33888:	bd70      	pop	{r4, r5, r6, pc}
   3388a:	4629      	mov	r1, r5
   3388c:	4622      	mov	r2, r4
   3388e:	4630      	mov	r0, r6
   33890:	f004 fbb0 	bl	37ff4 <__swbuf_r>
   33894:	4605      	mov	r5, r0
   33896:	e7ed      	b.n	33874 <_putc_r+0x44>

00033898 <putc>:
   33898:	4b02      	ldr	r3, [pc, #8]	; (338a4 <putc+0xc>)
   3389a:	460a      	mov	r2, r1
   3389c:	4601      	mov	r1, r0
   3389e:	6818      	ldr	r0, [r3, #0]
   338a0:	f7ff bfc6 	b.w	33830 <_putc_r>
   338a4:	20006000 	.word	0x20006000

000338a8 <_puts_r>:
   338a8:	b530      	push	{r4, r5, lr}
   338aa:	4605      	mov	r5, r0
   338ac:	b089      	sub	sp, #36	; 0x24
   338ae:	4608      	mov	r0, r1
   338b0:	460c      	mov	r4, r1
   338b2:	f000 fa3d 	bl	33d30 <strlen>
   338b6:	4b1e      	ldr	r3, [pc, #120]	; (33930 <_puts_r+0x88>)
   338b8:	9306      	str	r3, [sp, #24]
   338ba:	2301      	movs	r3, #1
   338bc:	e9cd 4004 	strd	r4, r0, [sp, #16]
   338c0:	9307      	str	r3, [sp, #28]
   338c2:	4418      	add	r0, r3
   338c4:	ab04      	add	r3, sp, #16
   338c6:	68ac      	ldr	r4, [r5, #8]
   338c8:	9003      	str	r0, [sp, #12]
   338ca:	9301      	str	r3, [sp, #4]
   338cc:	2302      	movs	r3, #2
   338ce:	9302      	str	r3, [sp, #8]
   338d0:	6bab      	ldr	r3, [r5, #56]	; 0x38
   338d2:	b913      	cbnz	r3, 338da <_puts_r+0x32>
   338d4:	4628      	mov	r0, r5
   338d6:	f005 fbfd 	bl	390d4 <__sinit>
   338da:	6e63      	ldr	r3, [r4, #100]	; 0x64
   338dc:	07db      	lsls	r3, r3, #31
   338de:	d405      	bmi.n	338ec <_puts_r+0x44>
   338e0:	89a3      	ldrh	r3, [r4, #12]
   338e2:	0598      	lsls	r0, r3, #22
   338e4:	d402      	bmi.n	338ec <_puts_r+0x44>
   338e6:	6da0      	ldr	r0, [r4, #88]	; 0x58
   338e8:	f005 ff5d 	bl	397a6 <__retarget_lock_acquire_recursive>
   338ec:	f9b4 300c 	ldrsh.w	r3, [r4, #12]
   338f0:	0499      	lsls	r1, r3, #18
   338f2:	d406      	bmi.n	33902 <_puts_r+0x5a>
   338f4:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   338f8:	81a3      	strh	r3, [r4, #12]
   338fa:	6e63      	ldr	r3, [r4, #100]	; 0x64
   338fc:	f423 5300 	bic.w	r3, r3, #8192	; 0x2000
   33900:	6663      	str	r3, [r4, #100]	; 0x64
   33902:	4628      	mov	r0, r5
   33904:	aa01      	add	r2, sp, #4
   33906:	4621      	mov	r1, r4
   33908:	f005 fd86 	bl	39418 <__sfvwrite_r>
   3390c:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3390e:	2800      	cmp	r0, #0
   33910:	bf14      	ite	ne
   33912:	f04f 35ff 	movne.w	r5, #4294967295	; 0xffffffff
   33916:	250a      	moveq	r5, #10
   33918:	07da      	lsls	r2, r3, #31
   3391a:	d405      	bmi.n	33928 <_puts_r+0x80>
   3391c:	89a3      	ldrh	r3, [r4, #12]
   3391e:	059b      	lsls	r3, r3, #22
   33920:	d402      	bmi.n	33928 <_puts_r+0x80>
   33922:	6da0      	ldr	r0, [r4, #88]	; 0x58
   33924:	f005 ff45 	bl	397b2 <__retarget_lock_release_recursive>
   33928:	4628      	mov	r0, r5
   3392a:	b009      	add	sp, #36	; 0x24
   3392c:	bd30      	pop	{r4, r5, pc}
   3392e:	bf00      	nop
   33930:	0004099c 	.word	0x0004099c

00033934 <puts>:
   33934:	4b02      	ldr	r3, [pc, #8]	; (33940 <puts+0xc>)
   33936:	4601      	mov	r1, r0
   33938:	6818      	ldr	r0, [r3, #0]
   3393a:	f7ff bfb5 	b.w	338a8 <_puts_r>
   3393e:	bf00      	nop
   33940:	20006000 	.word	0x20006000

00033944 <_snprintf_r>:
   33944:	b408      	push	{r3}
   33946:	b530      	push	{r4, r5, lr}
   33948:	1e14      	subs	r4, r2, #0
   3394a:	b09c      	sub	sp, #112	; 0x70
   3394c:	4605      	mov	r5, r0
   3394e:	da08      	bge.n	33962 <_snprintf_r+0x1e>
   33950:	238b      	movs	r3, #139	; 0x8b
   33952:	6003      	str	r3, [r0, #0]
   33954:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   33958:	b01c      	add	sp, #112	; 0x70
   3395a:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
   3395e:	b001      	add	sp, #4
   33960:	4770      	bx	lr
   33962:	f44f 7302 	mov.w	r3, #520	; 0x208
   33966:	9102      	str	r1, [sp, #8]
   33968:	9106      	str	r1, [sp, #24]
   3396a:	a902      	add	r1, sp, #8
   3396c:	f8ad 3014 	strh.w	r3, [sp, #20]
   33970:	bf14      	ite	ne
   33972:	f104 33ff 	addne.w	r3, r4, #4294967295	; 0xffffffff
   33976:	4623      	moveq	r3, r4
   33978:	9a1f      	ldr	r2, [sp, #124]	; 0x7c
   3397a:	9304      	str	r3, [sp, #16]
   3397c:	9307      	str	r3, [sp, #28]
   3397e:	f64f 73ff 	movw	r3, #65535	; 0xffff
   33982:	f8ad 3016 	strh.w	r3, [sp, #22]
   33986:	ab20      	add	r3, sp, #128	; 0x80
   33988:	9301      	str	r3, [sp, #4]
   3398a:	f000 fd0d 	bl	343a8 <_svfprintf_r>
   3398e:	1c43      	adds	r3, r0, #1
   33990:	bfbc      	itt	lt
   33992:	238b      	movlt	r3, #139	; 0x8b
   33994:	602b      	strlt	r3, [r5, #0]
   33996:	2c00      	cmp	r4, #0
   33998:	d0de      	beq.n	33958 <_snprintf_r+0x14>
   3399a:	9b02      	ldr	r3, [sp, #8]
   3399c:	2200      	movs	r2, #0
   3399e:	701a      	strb	r2, [r3, #0]
   339a0:	e7da      	b.n	33958 <_snprintf_r+0x14>

000339a2 <snprintf>:
   339a2:	b40c      	push	{r2, r3}
   339a4:	4b17      	ldr	r3, [pc, #92]	; (33a04 <snprintf+0x62>)
   339a6:	b530      	push	{r4, r5, lr}
   339a8:	1e0c      	subs	r4, r1, #0
   339aa:	b09d      	sub	sp, #116	; 0x74
   339ac:	681d      	ldr	r5, [r3, #0]
   339ae:	da08      	bge.n	339c2 <snprintf+0x20>
   339b0:	238b      	movs	r3, #139	; 0x8b
   339b2:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   339b6:	602b      	str	r3, [r5, #0]
   339b8:	b01d      	add	sp, #116	; 0x74
   339ba:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
   339be:	b002      	add	sp, #8
   339c0:	4770      	bx	lr
   339c2:	f44f 7302 	mov.w	r3, #520	; 0x208
   339c6:	9002      	str	r0, [sp, #8]
   339c8:	9006      	str	r0, [sp, #24]
   339ca:	a902      	add	r1, sp, #8
   339cc:	f8ad 3014 	strh.w	r3, [sp, #20]
   339d0:	bf14      	ite	ne
   339d2:	f104 33ff 	addne.w	r3, r4, #4294967295	; 0xffffffff
   339d6:	4623      	moveq	r3, r4
   339d8:	9a20      	ldr	r2, [sp, #128]	; 0x80
   339da:	4628      	mov	r0, r5
   339dc:	9304      	str	r3, [sp, #16]
   339de:	9307      	str	r3, [sp, #28]
   339e0:	f64f 73ff 	movw	r3, #65535	; 0xffff
   339e4:	f8ad 3016 	strh.w	r3, [sp, #22]
   339e8:	ab21      	add	r3, sp, #132	; 0x84
   339ea:	9301      	str	r3, [sp, #4]
   339ec:	f000 fcdc 	bl	343a8 <_svfprintf_r>
   339f0:	1c43      	adds	r3, r0, #1
   339f2:	bfbc      	itt	lt
   339f4:	238b      	movlt	r3, #139	; 0x8b
   339f6:	602b      	strlt	r3, [r5, #0]
   339f8:	2c00      	cmp	r4, #0
   339fa:	d0dd      	beq.n	339b8 <snprintf+0x16>
   339fc:	9b02      	ldr	r3, [sp, #8]
   339fe:	2200      	movs	r2, #0
   33a00:	701a      	strb	r2, [r3, #0]
   33a02:	e7d9      	b.n	339b8 <snprintf+0x16>
   33a04:	20006000 	.word	0x20006000

00033a08 <_sprintf_r>:
   33a08:	b40c      	push	{r2, r3}
   33a0a:	b500      	push	{lr}
   33a0c:	b09d      	sub	sp, #116	; 0x74
   33a0e:	9102      	str	r1, [sp, #8]
   33a10:	ab1e      	add	r3, sp, #120	; 0x78
   33a12:	9106      	str	r1, [sp, #24]
   33a14:	f06f 4100 	mvn.w	r1, #2147483648	; 0x80000000
   33a18:	f853 2b04 	ldr.w	r2, [r3], #4
   33a1c:	9107      	str	r1, [sp, #28]
   33a1e:	9104      	str	r1, [sp, #16]
   33a20:	4906      	ldr	r1, [pc, #24]	; (33a3c <_sprintf_r+0x34>)
   33a22:	9301      	str	r3, [sp, #4]
   33a24:	9105      	str	r1, [sp, #20]
   33a26:	a902      	add	r1, sp, #8
   33a28:	f000 fcbe 	bl	343a8 <_svfprintf_r>
   33a2c:	9b02      	ldr	r3, [sp, #8]
   33a2e:	2200      	movs	r2, #0
   33a30:	701a      	strb	r2, [r3, #0]
   33a32:	b01d      	add	sp, #116	; 0x74
   33a34:	f85d eb04 	ldr.w	lr, [sp], #4
   33a38:	b002      	add	sp, #8
   33a3a:	4770      	bx	lr
   33a3c:	ffff0208 	.word	0xffff0208

00033a40 <sprintf>:
   33a40:	b40e      	push	{r1, r2, r3}
   33a42:	b500      	push	{lr}
   33a44:	b09c      	sub	sp, #112	; 0x70
   33a46:	f06f 4100 	mvn.w	r1, #2147483648	; 0x80000000
   33a4a:	ab1d      	add	r3, sp, #116	; 0x74
   33a4c:	9002      	str	r0, [sp, #8]
   33a4e:	9006      	str	r0, [sp, #24]
   33a50:	9107      	str	r1, [sp, #28]
   33a52:	9104      	str	r1, [sp, #16]
   33a54:	4808      	ldr	r0, [pc, #32]	; (33a78 <sprintf+0x38>)
   33a56:	4909      	ldr	r1, [pc, #36]	; (33a7c <sprintf+0x3c>)
   33a58:	f853 2b04 	ldr.w	r2, [r3], #4
   33a5c:	9105      	str	r1, [sp, #20]
   33a5e:	a902      	add	r1, sp, #8
   33a60:	6800      	ldr	r0, [r0, #0]
   33a62:	9301      	str	r3, [sp, #4]
   33a64:	f000 fca0 	bl	343a8 <_svfprintf_r>
   33a68:	9b02      	ldr	r3, [sp, #8]
   33a6a:	2200      	movs	r2, #0
   33a6c:	701a      	strb	r2, [r3, #0]
   33a6e:	b01c      	add	sp, #112	; 0x70
   33a70:	f85d eb04 	ldr.w	lr, [sp], #4
   33a74:	b003      	add	sp, #12
   33a76:	4770      	bx	lr
   33a78:	20006000 	.word	0x20006000
   33a7c:	ffff0208 	.word	0xffff0208

00033a80 <sscanf>:
   33a80:	b40e      	push	{r1, r2, r3}
   33a82:	b510      	push	{r4, lr}
   33a84:	b09f      	sub	sp, #124	; 0x7c
   33a86:	f44f 7101 	mov.w	r1, #516	; 0x204
   33a8a:	ac21      	add	r4, sp, #132	; 0x84
   33a8c:	f8ad 101c 	strh.w	r1, [sp, #28]
   33a90:	9004      	str	r0, [sp, #16]
   33a92:	f854 2b04 	ldr.w	r2, [r4], #4
   33a96:	9008      	str	r0, [sp, #32]
   33a98:	9201      	str	r2, [sp, #4]
   33a9a:	f000 f949 	bl	33d30 <strlen>
   33a9e:	4b0c      	ldr	r3, [pc, #48]	; (33ad0 <sscanf+0x50>)
   33aa0:	a904      	add	r1, sp, #16
   33aa2:	9005      	str	r0, [sp, #20]
   33aa4:	930c      	str	r3, [sp, #48]	; 0x30
   33aa6:	2300      	movs	r3, #0
   33aa8:	9009      	str	r0, [sp, #36]	; 0x24
   33aaa:	9310      	str	r3, [sp, #64]	; 0x40
   33aac:	9315      	str	r3, [sp, #84]	; 0x54
   33aae:	f64f 73ff 	movw	r3, #65535	; 0xffff
   33ab2:	4808      	ldr	r0, [pc, #32]	; (33ad4 <sscanf+0x54>)
   33ab4:	f8ad 301e 	strh.w	r3, [sp, #30]
   33ab8:	4623      	mov	r3, r4
   33aba:	9a01      	ldr	r2, [sp, #4]
   33abc:	6800      	ldr	r0, [r0, #0]
   33abe:	9403      	str	r4, [sp, #12]
   33ac0:	f001 fe50 	bl	35764 <__ssvfscanf_r>
   33ac4:	b01f      	add	sp, #124	; 0x7c
   33ac6:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
   33aca:	b003      	add	sp, #12
   33acc:	4770      	bx	lr
   33ace:	bf00      	nop
   33ad0:	00033b4f 	.word	0x00033b4f
   33ad4:	20006000 	.word	0x20006000

00033ad8 <_sscanf_r>:
   33ad8:	b40c      	push	{r2, r3}
   33ada:	b530      	push	{r4, r5, lr}
   33adc:	b09f      	sub	sp, #124	; 0x7c
   33ade:	f44f 7301 	mov.w	r3, #516	; 0x204
   33ae2:	4605      	mov	r5, r0
   33ae4:	4608      	mov	r0, r1
   33ae6:	ac22      	add	r4, sp, #136	; 0x88
   33ae8:	f8ad 301c 	strh.w	r3, [sp, #28]
   33aec:	9104      	str	r1, [sp, #16]
   33aee:	f854 2b04 	ldr.w	r2, [r4], #4
   33af2:	9108      	str	r1, [sp, #32]
   33af4:	9201      	str	r2, [sp, #4]
   33af6:	f000 f91b 	bl	33d30 <strlen>
   33afa:	4b0b      	ldr	r3, [pc, #44]	; (33b28 <_sscanf_r+0x50>)
   33afc:	a904      	add	r1, sp, #16
   33afe:	9a01      	ldr	r2, [sp, #4]
   33b00:	930c      	str	r3, [sp, #48]	; 0x30
   33b02:	2300      	movs	r3, #0
   33b04:	9005      	str	r0, [sp, #20]
   33b06:	9310      	str	r3, [sp, #64]	; 0x40
   33b08:	9315      	str	r3, [sp, #84]	; 0x54
   33b0a:	f64f 73ff 	movw	r3, #65535	; 0xffff
   33b0e:	9009      	str	r0, [sp, #36]	; 0x24
   33b10:	4628      	mov	r0, r5
   33b12:	f8ad 301e 	strh.w	r3, [sp, #30]
   33b16:	4623      	mov	r3, r4
   33b18:	9403      	str	r4, [sp, #12]
   33b1a:	f001 fe23 	bl	35764 <__ssvfscanf_r>
   33b1e:	b01f      	add	sp, #124	; 0x7c
   33b20:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
   33b24:	b002      	add	sp, #8
   33b26:	4770      	bx	lr
   33b28:	00033b4f 	.word	0x00033b4f

00033b2c <__sread>:
   33b2c:	b510      	push	{r4, lr}
   33b2e:	460c      	mov	r4, r1
   33b30:	f9b1 100e 	ldrsh.w	r1, [r1, #14]
   33b34:	f006 ff32 	bl	3a99c <_read_r>
   33b38:	2800      	cmp	r0, #0
   33b3a:	bfab      	itete	ge
   33b3c:	6d23      	ldrge	r3, [r4, #80]	; 0x50
   33b3e:	89a3      	ldrhlt	r3, [r4, #12]
   33b40:	181b      	addge	r3, r3, r0
   33b42:	f423 5380 	biclt.w	r3, r3, #4096	; 0x1000
   33b46:	bfac      	ite	ge
   33b48:	6523      	strge	r3, [r4, #80]	; 0x50
   33b4a:	81a3      	strhlt	r3, [r4, #12]
   33b4c:	bd10      	pop	{r4, pc}

00033b4e <__seofread>:
   33b4e:	2000      	movs	r0, #0
   33b50:	4770      	bx	lr

00033b52 <__swrite>:
   33b52:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
   33b56:	461f      	mov	r7, r3
   33b58:	898b      	ldrh	r3, [r1, #12]
   33b5a:	4605      	mov	r5, r0
   33b5c:	460c      	mov	r4, r1
   33b5e:	05db      	lsls	r3, r3, #23
   33b60:	4616      	mov	r6, r2
   33b62:	d505      	bpl.n	33b70 <__swrite+0x1e>
   33b64:	2302      	movs	r3, #2
   33b66:	2200      	movs	r2, #0
   33b68:	f9b1 100e 	ldrsh.w	r1, [r1, #14]
   33b6c:	f005 fe22 	bl	397b4 <_lseek_r>
   33b70:	89a3      	ldrh	r3, [r4, #12]
   33b72:	4632      	mov	r2, r6
   33b74:	f9b4 100e 	ldrsh.w	r1, [r4, #14]
   33b78:	4628      	mov	r0, r5
   33b7a:	f423 5380 	bic.w	r3, r3, #4096	; 0x1000
   33b7e:	81a3      	strh	r3, [r4, #12]
   33b80:	463b      	mov	r3, r7
   33b82:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
   33b86:	f004 ba87 	b.w	38098 <_write_r>

00033b8a <__sseek>:
   33b8a:	b510      	push	{r4, lr}
   33b8c:	460c      	mov	r4, r1
   33b8e:	f9b1 100e 	ldrsh.w	r1, [r1, #14]
   33b92:	f005 fe0f 	bl	397b4 <_lseek_r>
   33b96:	1c43      	adds	r3, r0, #1
   33b98:	89a3      	ldrh	r3, [r4, #12]
   33b9a:	bf15      	itete	ne
   33b9c:	6520      	strne	r0, [r4, #80]	; 0x50
   33b9e:	f423 5380 	biceq.w	r3, r3, #4096	; 0x1000
   33ba2:	f443 5380 	orrne.w	r3, r3, #4096	; 0x1000
   33ba6:	81a3      	strheq	r3, [r4, #12]
   33ba8:	bf18      	it	ne
   33baa:	81a3      	strhne	r3, [r4, #12]
   33bac:	bd10      	pop	{r4, pc}

00033bae <__sclose>:
   33bae:	f9b1 100e 	ldrsh.w	r1, [r1, #14]
   33bb2:	f004 badb 	b.w	3816c <_close_r>

00033bb6 <strcat>:
   33bb6:	b538      	push	{r3, r4, r5, lr}
   33bb8:	0783      	lsls	r3, r0, #30
   33bba:	4604      	mov	r4, r0
   33bbc:	d009      	beq.n	33bd2 <strcat+0x1c>
   33bbe:	4603      	mov	r3, r0
   33bc0:	4618      	mov	r0, r3
   33bc2:	3301      	adds	r3, #1
   33bc4:	7802      	ldrb	r2, [r0, #0]
   33bc6:	2a00      	cmp	r2, #0
   33bc8:	d1fa      	bne.n	33bc0 <strcat+0xa>
   33bca:	f000 f853 	bl	33c74 <strcpy>
   33bce:	4620      	mov	r0, r4
   33bd0:	bd38      	pop	{r3, r4, r5, pc}
   33bd2:	4603      	mov	r3, r0
   33bd4:	f850 5b04 	ldr.w	r5, [r0], #4
   33bd8:	f1a5 3201 	sub.w	r2, r5, #16843009	; 0x1010101
   33bdc:	ea22 0205 	bic.w	r2, r2, r5
   33be0:	f012 3f80 	tst.w	r2, #2155905152	; 0x80808080
   33be4:	d0f5      	beq.n	33bd2 <strcat+0x1c>
   33be6:	e7eb      	b.n	33bc0 <strcat+0xa>

00033be8 <strchr>:
   33be8:	f011 01ff 	ands.w	r1, r1, #255	; 0xff
   33bec:	4603      	mov	r3, r0
   33bee:	b5f0      	push	{r4, r5, r6, r7, lr}
   33bf0:	d11c      	bne.n	33c2c <strchr+0x44>
   33bf2:	0799      	lsls	r1, r3, #30
   33bf4:	4618      	mov	r0, r3
   33bf6:	d10f      	bne.n	33c18 <strchr+0x30>
   33bf8:	6818      	ldr	r0, [r3, #0]
   33bfa:	461a      	mov	r2, r3
   33bfc:	3304      	adds	r3, #4
   33bfe:	f1a0 3101 	sub.w	r1, r0, #16843009	; 0x1010101
   33c02:	ea21 0100 	bic.w	r1, r1, r0
   33c06:	f011 3f80 	tst.w	r1, #2155905152	; 0x80808080
   33c0a:	d0f5      	beq.n	33bf8 <strchr+0x10>
   33c0c:	4610      	mov	r0, r2
   33c0e:	3201      	adds	r2, #1
   33c10:	7803      	ldrb	r3, [r0, #0]
   33c12:	2b00      	cmp	r3, #0
   33c14:	d1fa      	bne.n	33c0c <strchr+0x24>
   33c16:	e003      	b.n	33c20 <strchr+0x38>
   33c18:	7802      	ldrb	r2, [r0, #0]
   33c1a:	3301      	adds	r3, #1
   33c1c:	2a00      	cmp	r2, #0
   33c1e:	d1e8      	bne.n	33bf2 <strchr+0xa>
   33c20:	bdf0      	pop	{r4, r5, r6, r7, pc}
   33c22:	f813 2b01 	ldrb.w	r2, [r3], #1
   33c26:	b302      	cbz	r2, 33c6a <strchr+0x82>
   33c28:	428a      	cmp	r2, r1
   33c2a:	d0f9      	beq.n	33c20 <strchr+0x38>
   33c2c:	079a      	lsls	r2, r3, #30
   33c2e:	4618      	mov	r0, r3
   33c30:	d1f7      	bne.n	33c22 <strchr+0x3a>
   33c32:	ea41 2201 	orr.w	r2, r1, r1, lsl #8
   33c36:	ea42 4002 	orr.w	r0, r2, r2, lsl #16
   33c3a:	681e      	ldr	r6, [r3, #0]
   33c3c:	461c      	mov	r4, r3
   33c3e:	3304      	adds	r3, #4
   33c40:	ea86 0700 	eor.w	r7, r6, r0
   33c44:	f1a6 3201 	sub.w	r2, r6, #16843009	; 0x1010101
   33c48:	f1a7 3501 	sub.w	r5, r7, #16843009	; 0x1010101
   33c4c:	ea22 0206 	bic.w	r2, r2, r6
   33c50:	ea25 0507 	bic.w	r5, r5, r7
   33c54:	432a      	orrs	r2, r5
   33c56:	f012 3f80 	tst.w	r2, #2155905152	; 0x80808080
   33c5a:	d0ee      	beq.n	33c3a <strchr+0x52>
   33c5c:	4620      	mov	r0, r4
   33c5e:	f814 3b01 	ldrb.w	r3, [r4], #1
   33c62:	b123      	cbz	r3, 33c6e <strchr+0x86>
   33c64:	428b      	cmp	r3, r1
   33c66:	d1f9      	bne.n	33c5c <strchr+0x74>
   33c68:	e7da      	b.n	33c20 <strchr+0x38>
   33c6a:	4610      	mov	r0, r2
   33c6c:	e7d8      	b.n	33c20 <strchr+0x38>
   33c6e:	4618      	mov	r0, r3
   33c70:	e7d6      	b.n	33c20 <strchr+0x38>
	...

00033c74 <strcpy>:
   33c74:	ea80 0201 	eor.w	r2, r0, r1
   33c78:	4684      	mov	ip, r0
   33c7a:	f012 0f03 	tst.w	r2, #3
   33c7e:	d14f      	bne.n	33d20 <strcpy+0xac>
   33c80:	f011 0f03 	tst.w	r1, #3
   33c84:	d132      	bne.n	33cec <strcpy+0x78>
   33c86:	f84d 4d04 	str.w	r4, [sp, #-4]!
   33c8a:	f011 0f04 	tst.w	r1, #4
   33c8e:	f851 3b04 	ldr.w	r3, [r1], #4
   33c92:	d00b      	beq.n	33cac <strcpy+0x38>
   33c94:	f1a3 3201 	sub.w	r2, r3, #16843009	; 0x1010101
   33c98:	439a      	bics	r2, r3
   33c9a:	f012 3f80 	tst.w	r2, #2155905152	; 0x80808080
   33c9e:	bf04      	itt	eq
   33ca0:	f84c 3b04 	streq.w	r3, [ip], #4
   33ca4:	f851 3b04 	ldreq.w	r3, [r1], #4
   33ca8:	d116      	bne.n	33cd8 <strcpy+0x64>
   33caa:	bf00      	nop
   33cac:	f851 4b04 	ldr.w	r4, [r1], #4
   33cb0:	f1a3 3201 	sub.w	r2, r3, #16843009	; 0x1010101
   33cb4:	439a      	bics	r2, r3
   33cb6:	f012 3f80 	tst.w	r2, #2155905152	; 0x80808080
   33cba:	f1a4 3201 	sub.w	r2, r4, #16843009	; 0x1010101
   33cbe:	d10b      	bne.n	33cd8 <strcpy+0x64>
   33cc0:	f84c 3b04 	str.w	r3, [ip], #4
   33cc4:	43a2      	bics	r2, r4
   33cc6:	f012 3f80 	tst.w	r2, #2155905152	; 0x80808080
   33cca:	bf04      	itt	eq
   33ccc:	f851 3b04 	ldreq.w	r3, [r1], #4
   33cd0:	f84c 4b04 	streq.w	r4, [ip], #4
   33cd4:	d0ea      	beq.n	33cac <strcpy+0x38>
   33cd6:	4623      	mov	r3, r4
   33cd8:	f80c 3b01 	strb.w	r3, [ip], #1
   33cdc:	f013 0fff 	tst.w	r3, #255	; 0xff
   33ce0:	ea4f 2333 	mov.w	r3, r3, ror #8
   33ce4:	d1f8      	bne.n	33cd8 <strcpy+0x64>
   33ce6:	f85d 4b04 	ldr.w	r4, [sp], #4
   33cea:	4770      	bx	lr
   33cec:	f011 0f01 	tst.w	r1, #1
   33cf0:	d006      	beq.n	33d00 <strcpy+0x8c>
   33cf2:	f811 2b01 	ldrb.w	r2, [r1], #1
   33cf6:	f80c 2b01 	strb.w	r2, [ip], #1
   33cfa:	2a00      	cmp	r2, #0
   33cfc:	bf08      	it	eq
   33cfe:	4770      	bxeq	lr
   33d00:	f011 0f02 	tst.w	r1, #2
   33d04:	d0bf      	beq.n	33c86 <strcpy+0x12>
   33d06:	f831 2b02 	ldrh.w	r2, [r1], #2
   33d0a:	f012 0fff 	tst.w	r2, #255	; 0xff
   33d0e:	bf16      	itet	ne
   33d10:	f82c 2b02 	strhne.w	r2, [ip], #2
   33d14:	f88c 2000 	strbeq.w	r2, [ip]
   33d18:	f412 4f7f 	tstne.w	r2, #65280	; 0xff00
   33d1c:	d1b3      	bne.n	33c86 <strcpy+0x12>
   33d1e:	4770      	bx	lr
   33d20:	f811 2b01 	ldrb.w	r2, [r1], #1
   33d24:	f80c 2b01 	strb.w	r2, [ip], #1
   33d28:	2a00      	cmp	r2, #0
   33d2a:	d1f9      	bne.n	33d20 <strcpy+0xac>
   33d2c:	4770      	bx	lr
   33d2e:	bf00      	nop

00033d30 <strlen>:
   33d30:	f020 0103 	bic.w	r1, r0, #3
   33d34:	f010 0003 	ands.w	r0, r0, #3
   33d38:	f1c0 0000 	rsb	r0, r0, #0
   33d3c:	f851 3b04 	ldr.w	r3, [r1], #4
   33d40:	f100 0c04 	add.w	ip, r0, #4
   33d44:	ea4f 0ccc 	mov.w	ip, ip, lsl #3
   33d48:	f06f 0200 	mvn.w	r2, #0
   33d4c:	bf1c      	itt	ne
   33d4e:	fa22 f20c 	lsrne.w	r2, r2, ip
   33d52:	4313      	orrne	r3, r2
   33d54:	f04f 0c01 	mov.w	ip, #1
   33d58:	ea4c 2c0c 	orr.w	ip, ip, ip, lsl #8
   33d5c:	ea4c 4c0c 	orr.w	ip, ip, ip, lsl #16
   33d60:	eba3 020c 	sub.w	r2, r3, ip
   33d64:	ea22 0203 	bic.w	r2, r2, r3
   33d68:	ea12 12cc 	ands.w	r2, r2, ip, lsl #7
   33d6c:	bf04      	itt	eq
   33d6e:	f851 3b04 	ldreq.w	r3, [r1], #4
   33d72:	3004      	addeq	r0, #4
   33d74:	d0f4      	beq.n	33d60 <strlen+0x30>
   33d76:	f1c2 0100 	rsb	r1, r2, #0
   33d7a:	ea02 0201 	and.w	r2, r2, r1
   33d7e:	fab2 f282 	clz	r2, r2
   33d82:	f1c2 021f 	rsb	r2, r2, #31
   33d86:	eb00 00d2 	add.w	r0, r0, r2, lsr #3
   33d8a:	4770      	bx	lr

00033d8c <strncat>:
   33d8c:	0783      	lsls	r3, r0, #30
   33d8e:	b570      	push	{r4, r5, r6, lr}
   33d90:	4605      	mov	r5, r0
   33d92:	d00d      	beq.n	33db0 <strncat+0x24>
   33d94:	4603      	mov	r3, r0
   33d96:	781d      	ldrb	r5, [r3, #0]
   33d98:	461c      	mov	r4, r3
   33d9a:	3301      	adds	r3, #1
   33d9c:	2d00      	cmp	r5, #0
   33d9e:	d1fa      	bne.n	33d96 <strncat+0xa>
   33da0:	3a01      	subs	r2, #1
   33da2:	d304      	bcc.n	33dae <strncat+0x22>
   33da4:	f811 3b01 	ldrb.w	r3, [r1], #1
   33da8:	f804 3b01 	strb.w	r3, [r4], #1
   33dac:	b95b      	cbnz	r3, 33dc6 <strncat+0x3a>
   33dae:	bd70      	pop	{r4, r5, r6, pc}
   33db0:	462b      	mov	r3, r5
   33db2:	f855 6b04 	ldr.w	r6, [r5], #4
   33db6:	f1a6 3401 	sub.w	r4, r6, #16843009	; 0x1010101
   33dba:	ea24 0406 	bic.w	r4, r4, r6
   33dbe:	f014 3f80 	tst.w	r4, #2155905152	; 0x80808080
   33dc2:	d0f5      	beq.n	33db0 <strncat+0x24>
   33dc4:	e7e7      	b.n	33d96 <strncat+0xa>
   33dc6:	2a00      	cmp	r2, #0
   33dc8:	d1ea      	bne.n	33da0 <strncat+0x14>
   33dca:	7022      	strb	r2, [r4, #0]
   33dcc:	e7e8      	b.n	33da0 <strncat+0x14>

00033dce <strncmp>:
   33dce:	b570      	push	{r4, r5, r6, lr}
   33dd0:	b33a      	cbz	r2, 33e22 <strncmp+0x54>
   33dd2:	ea40 0301 	orr.w	r3, r0, r1
   33dd6:	4605      	mov	r5, r0
   33dd8:	460c      	mov	r4, r1
   33dda:	079b      	lsls	r3, r3, #30
   33ddc:	d012      	beq.n	33e04 <strncmp+0x36>
   33dde:	3901      	subs	r1, #1
   33de0:	1884      	adds	r4, r0, r2
   33de2:	f810 2b01 	ldrb.w	r2, [r0], #1
   33de6:	f811 3f01 	ldrb.w	r3, [r1, #1]!
   33dea:	429a      	cmp	r2, r3
   33dec:	d015      	beq.n	33e1a <strncmp+0x4c>
   33dee:	1ad2      	subs	r2, r2, r3
   33df0:	e017      	b.n	33e22 <strncmp+0x54>
   33df2:	3a04      	subs	r2, #4
   33df4:	d015      	beq.n	33e22 <strncmp+0x54>
   33df6:	f1a3 3101 	sub.w	r1, r3, #16843009	; 0x1010101
   33dfa:	ea21 0303 	bic.w	r3, r1, r3
   33dfe:	f013 3f80 	tst.w	r3, #2155905152	; 0x80808080
   33e02:	d110      	bne.n	33e26 <strncmp+0x58>
   33e04:	2a03      	cmp	r2, #3
   33e06:	4621      	mov	r1, r4
   33e08:	4628      	mov	r0, r5
   33e0a:	d9e8      	bls.n	33dde <strncmp+0x10>
   33e0c:	f855 3b04 	ldr.w	r3, [r5], #4
   33e10:	3404      	adds	r4, #4
   33e12:	680e      	ldr	r6, [r1, #0]
   33e14:	42b3      	cmp	r3, r6
   33e16:	d0ec      	beq.n	33df2 <strncmp+0x24>
   33e18:	e7e1      	b.n	33dde <strncmp+0x10>
   33e1a:	42a0      	cmp	r0, r4
   33e1c:	d003      	beq.n	33e26 <strncmp+0x58>
   33e1e:	2a00      	cmp	r2, #0
   33e20:	d1df      	bne.n	33de2 <strncmp+0x14>
   33e22:	4610      	mov	r0, r2
   33e24:	bd70      	pop	{r4, r5, r6, pc}
   33e26:	2200      	movs	r2, #0
   33e28:	e7fb      	b.n	33e22 <strncmp+0x54>

00033e2a <strncpy>:
   33e2a:	ea40 0301 	orr.w	r3, r0, r1
   33e2e:	f013 0f03 	tst.w	r3, #3
   33e32:	4603      	mov	r3, r0
   33e34:	b570      	push	{r4, r5, r6, lr}
   33e36:	460c      	mov	r4, r1
   33e38:	d101      	bne.n	33e3e <strncpy+0x14>
   33e3a:	2a03      	cmp	r2, #3
   33e3c:	d80d      	bhi.n	33e5a <strncpy+0x30>
   33e3e:	3901      	subs	r1, #1
   33e40:	b132      	cbz	r2, 33e50 <strncpy+0x26>
   33e42:	f811 4f01 	ldrb.w	r4, [r1, #1]!
   33e46:	3a01      	subs	r2, #1
   33e48:	f803 4b01 	strb.w	r4, [r3], #1
   33e4c:	2c00      	cmp	r4, #0
   33e4e:	d1f7      	bne.n	33e40 <strncpy+0x16>
   33e50:	441a      	add	r2, r3
   33e52:	2100      	movs	r1, #0
   33e54:	429a      	cmp	r2, r3
   33e56:	d10f      	bne.n	33e78 <strncpy+0x4e>
   33e58:	bd70      	pop	{r4, r5, r6, pc}
   33e5a:	4621      	mov	r1, r4
   33e5c:	f854 6b04 	ldr.w	r6, [r4], #4
   33e60:	f1a6 3501 	sub.w	r5, r6, #16843009	; 0x1010101
   33e64:	ea25 0506 	bic.w	r5, r5, r6
   33e68:	f015 3f80 	tst.w	r5, #2155905152	; 0x80808080
   33e6c:	d1e7      	bne.n	33e3e <strncpy+0x14>
   33e6e:	3a04      	subs	r2, #4
   33e70:	4621      	mov	r1, r4
   33e72:	f843 6b04 	str.w	r6, [r3], #4
   33e76:	e7e0      	b.n	33e3a <strncpy+0x10>
   33e78:	f803 1b01 	strb.w	r1, [r3], #1
   33e7c:	e7ea      	b.n	33e54 <strncpy+0x2a>

00033e7e <strpbrk>:
   33e7e:	b570      	push	{r4, r5, r6, lr}
   33e80:	7803      	ldrb	r3, [r0, #0]
   33e82:	b1a3      	cbz	r3, 33eae <strpbrk+0x30>
   33e84:	4603      	mov	r3, r0
   33e86:	460c      	mov	r4, r1
   33e88:	4618      	mov	r0, r3
   33e8a:	f813 5b01 	ldrb.w	r5, [r3], #1
   33e8e:	b14d      	cbz	r5, 33ea4 <strpbrk+0x26>
   33e90:	460a      	mov	r2, r1
   33e92:	e001      	b.n	33e98 <strpbrk+0x1a>
   33e94:	42ae      	cmp	r6, r5
   33e96:	d009      	beq.n	33eac <strpbrk+0x2e>
   33e98:	4614      	mov	r4, r2
   33e9a:	f812 6b01 	ldrb.w	r6, [r2], #1
   33e9e:	2e00      	cmp	r6, #0
   33ea0:	d1f8      	bne.n	33e94 <strpbrk+0x16>
   33ea2:	e7f1      	b.n	33e88 <strpbrk+0xa>
   33ea4:	7823      	ldrb	r3, [r4, #0]
   33ea6:	2b00      	cmp	r3, #0
   33ea8:	bf08      	it	eq
   33eaa:	2000      	moveq	r0, #0
   33eac:	bd70      	pop	{r4, r5, r6, pc}
   33eae:	4618      	mov	r0, r3
   33eb0:	e7fc      	b.n	33eac <strpbrk+0x2e>

00033eb2 <strsep>:
   33eb2:	4602      	mov	r2, r0
   33eb4:	2300      	movs	r3, #0
   33eb6:	6800      	ldr	r0, [r0, #0]
   33eb8:	f000 b9b4 	b.w	34224 <__strtok_r>

00033ebc <two_way_long_needle>:
   33ebc:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   33ec0:	f04f 0801 	mov.w	r8, #1
   33ec4:	4689      	mov	r9, r1
   33ec6:	461f      	mov	r7, r3
   33ec8:	f2ad 4d14 	subw	sp, sp, #1044	; 0x414
   33ecc:	4682      	mov	sl, r0
   33ece:	4616      	mov	r6, r2
   33ed0:	4643      	mov	r3, r8
   33ed2:	2100      	movs	r1, #0
   33ed4:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
   33ed8:	185a      	adds	r2, r3, r1
   33eda:	4297      	cmp	r7, r2
   33edc:	d83b      	bhi.n	33f56 <two_way_long_needle+0x9a>
   33ede:	2101      	movs	r1, #1
   33ee0:	2400      	movs	r4, #0
   33ee2:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   33ee6:	460b      	mov	r3, r1
   33ee8:	1918      	adds	r0, r3, r4
   33eea:	4287      	cmp	r7, r0
   33eec:	d849      	bhi.n	33f82 <two_way_long_needle+0xc6>
   33eee:	3201      	adds	r2, #1
   33ef0:	3501      	adds	r5, #1
   33ef2:	2300      	movs	r3, #0
   33ef4:	42aa      	cmp	r2, r5
   33ef6:	bf24      	itt	cs
   33ef8:	4615      	movcs	r5, r2
   33efa:	4688      	movcs	r8, r1
   33efc:	aa04      	add	r2, sp, #16
   33efe:	3301      	adds	r3, #1
   33f00:	f842 7b04 	str.w	r7, [r2], #4
   33f04:	f5b3 7f80 	cmp.w	r3, #256	; 0x100
   33f08:	d1f9      	bne.n	33efe <two_way_long_needle+0x42>
   33f0a:	1e7b      	subs	r3, r7, #1
   33f0c:	4632      	mov	r2, r6
   33f0e:	9301      	str	r3, [sp, #4]
   33f10:	1c59      	adds	r1, r3, #1
   33f12:	d14d      	bne.n	33fb0 <two_way_long_needle+0xf4>
   33f14:	462a      	mov	r2, r5
   33f16:	eb06 0108 	add.w	r1, r6, r8
   33f1a:	4630      	mov	r0, r6
   33f1c:	f005 ff75 	bl	39e0a <memcmp>
   33f20:	2800      	cmp	r0, #0
   33f22:	d070      	beq.n	34006 <two_way_long_needle+0x14a>
   33f24:	eba7 0b05 	sub.w	fp, r7, r5
   33f28:	f04f 0800 	mov.w	r8, #0
   33f2c:	f1c5 0201 	rsb	r2, r5, #1
   33f30:	f447 6100 	orr.w	r1, r7, #2048	; 0x800
   33f34:	45ab      	cmp	fp, r5
   33f36:	bf38      	it	cc
   33f38:	46ab      	movcc	fp, r5
   33f3a:	f10b 0b01 	add.w	fp, fp, #1
   33f3e:	eb0a 0408 	add.w	r4, sl, r8
   33f42:	9b01      	ldr	r3, [sp, #4]
   33f44:	a804      	add	r0, sp, #16
   33f46:	5ce3      	ldrb	r3, [r4, r3]
   33f48:	f850 3023 	ldr.w	r3, [r0, r3, lsl #2]
   33f4c:	2b00      	cmp	r3, #0
   33f4e:	f000 8097 	beq.w	34080 <two_way_long_needle+0x1c4>
   33f52:	4498      	add	r8, r3
   33f54:	e0ab      	b.n	340ae <two_way_long_needle+0x1f2>
   33f56:	1970      	adds	r0, r6, r5
   33f58:	5cb4      	ldrb	r4, [r6, r2]
   33f5a:	5cc0      	ldrb	r0, [r0, r3]
   33f5c:	4284      	cmp	r4, r0
   33f5e:	d203      	bcs.n	33f68 <two_way_long_needle+0xac>
   33f60:	eba2 0805 	sub.w	r8, r2, r5
   33f64:	2301      	movs	r3, #1
   33f66:	e004      	b.n	33f72 <two_way_long_needle+0xb6>
   33f68:	d105      	bne.n	33f76 <two_way_long_needle+0xba>
   33f6a:	4543      	cmp	r3, r8
   33f6c:	d0fa      	beq.n	33f64 <two_way_long_needle+0xa8>
   33f6e:	3301      	adds	r3, #1
   33f70:	460a      	mov	r2, r1
   33f72:	4611      	mov	r1, r2
   33f74:	e7b0      	b.n	33ed8 <two_way_long_needle+0x1c>
   33f76:	f04f 0801 	mov.w	r8, #1
   33f7a:	1c4a      	adds	r2, r1, #1
   33f7c:	460d      	mov	r5, r1
   33f7e:	4643      	mov	r3, r8
   33f80:	e7f7      	b.n	33f72 <two_way_long_needle+0xb6>
   33f82:	eb06 0c02 	add.w	ip, r6, r2
   33f86:	f816 e000 	ldrb.w	lr, [r6, r0]
   33f8a:	f81c c003 	ldrb.w	ip, [ip, r3]
   33f8e:	45e6      	cmp	lr, ip
   33f90:	d902      	bls.n	33f98 <two_way_long_needle+0xdc>
   33f92:	1a81      	subs	r1, r0, r2
   33f94:	2301      	movs	r3, #1
   33f96:	e004      	b.n	33fa2 <two_way_long_needle+0xe6>
   33f98:	d105      	bne.n	33fa6 <two_way_long_needle+0xea>
   33f9a:	428b      	cmp	r3, r1
   33f9c:	d0fa      	beq.n	33f94 <two_way_long_needle+0xd8>
   33f9e:	3301      	adds	r3, #1
   33fa0:	4620      	mov	r0, r4
   33fa2:	4604      	mov	r4, r0
   33fa4:	e7a0      	b.n	33ee8 <two_way_long_needle+0x2c>
   33fa6:	2101      	movs	r1, #1
   33fa8:	1c60      	adds	r0, r4, #1
   33faa:	4622      	mov	r2, r4
   33fac:	460b      	mov	r3, r1
   33fae:	e7f8      	b.n	33fa2 <two_way_long_needle+0xe6>
   33fb0:	f812 1b01 	ldrb.w	r1, [r2], #1
   33fb4:	a804      	add	r0, sp, #16
   33fb6:	f840 3021 	str.w	r3, [r0, r1, lsl #2]
   33fba:	3b01      	subs	r3, #1
   33fbc:	e7a8      	b.n	33f10 <two_way_long_needle+0x54>
   33fbe:	4658      	mov	r0, fp
   33fc0:	eb0a 0204 	add.w	r2, sl, r4
   33fc4:	9b01      	ldr	r3, [sp, #4]
   33fc6:	a904      	add	r1, sp, #16
   33fc8:	5cd3      	ldrb	r3, [r2, r3]
   33fca:	f851 b023 	ldr.w	fp, [r1, r3, lsl #2]
   33fce:	f1bb 0f00 	cmp.w	fp, #0
   33fd2:	d020      	beq.n	34016 <two_way_long_needle+0x15a>
   33fd4:	b118      	cbz	r0, 33fde <two_way_long_needle+0x122>
   33fd6:	45c3      	cmp	fp, r8
   33fd8:	9b02      	ldr	r3, [sp, #8]
   33fda:	bf38      	it	cc
   33fdc:	469b      	movcc	fp, r3
   33fde:	445c      	add	r4, fp
   33fe0:	f04f 0b00 	mov.w	fp, #0
   33fe4:	eba9 0307 	sub.w	r3, r9, r7
   33fe8:	42a3      	cmp	r3, r4
   33fea:	d2e8      	bcs.n	33fbe <two_way_long_needle+0x102>
   33fec:	eb0a 0009 	add.w	r0, sl, r9
   33ff0:	f447 6100 	orr.w	r1, r7, #2048	; 0x800
   33ff4:	f007 f84c 	bl	3b090 <strnlen>
   33ff8:	4481      	add	r9, r0
   33ffa:	eba9 0307 	sub.w	r3, r9, r7
   33ffe:	42a3      	cmp	r3, r4
   34000:	d2dd      	bcs.n	33fbe <two_way_long_needle+0x102>
   34002:	2000      	movs	r0, #0
   34004:	e029      	b.n	3405a <two_way_long_needle+0x19e>
   34006:	eba7 0308 	sub.w	r3, r7, r8
   3400a:	4604      	mov	r4, r0
   3400c:	9302      	str	r3, [sp, #8]
   3400e:	f1c5 0301 	rsb	r3, r5, #1
   34012:	9303      	str	r3, [sp, #12]
   34014:	e7d4      	b.n	33fc0 <two_way_long_needle+0x104>
   34016:	4285      	cmp	r5, r0
   34018:	462b      	mov	r3, r5
   3401a:	bf38      	it	cc
   3401c:	4603      	movcc	r3, r0
   3401e:	9901      	ldr	r1, [sp, #4]
   34020:	428b      	cmp	r3, r1
   34022:	d22f      	bcs.n	34084 <two_way_long_needle+0x1c8>
   34024:	f816 c003 	ldrb.w	ip, [r6, r3]
   34028:	5cd1      	ldrb	r1, [r2, r3]
   3402a:	458c      	cmp	ip, r1
   3402c:	d003      	beq.n	34036 <two_way_long_needle+0x17a>
   3402e:	9a03      	ldr	r2, [sp, #12]
   34030:	4414      	add	r4, r2
   34032:	441c      	add	r4, r3
   34034:	e7d6      	b.n	33fe4 <two_way_long_needle+0x128>
   34036:	3301      	adds	r3, #1
   34038:	e7f1      	b.n	3401e <two_way_long_needle+0x162>
   3403a:	4673      	mov	r3, lr
   3403c:	4283      	cmp	r3, r0
   3403e:	d907      	bls.n	34050 <two_way_long_needle+0x194>
   34040:	f81b cd01 	ldrb.w	ip, [fp, #-1]!
   34044:	f103 3eff 	add.w	lr, r3, #4294967295	; 0xffffffff
   34048:	f812 1d01 	ldrb.w	r1, [r2, #-1]!
   3404c:	458c      	cmp	ip, r1
   3404e:	d0f4      	beq.n	3403a <two_way_long_needle+0x17e>
   34050:	3001      	adds	r0, #1
   34052:	4298      	cmp	r0, r3
   34054:	d905      	bls.n	34062 <two_way_long_needle+0x1a6>
   34056:	eb0a 0004 	add.w	r0, sl, r4
   3405a:	f20d 4d14 	addw	sp, sp, #1044	; 0x414
   3405e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   34062:	4444      	add	r4, r8
   34064:	f8dd b008 	ldr.w	fp, [sp, #8]
   34068:	e7bc      	b.n	33fe4 <two_way_long_needle+0x128>
   3406a:	3301      	adds	r3, #1
   3406c:	9801      	ldr	r0, [sp, #4]
   3406e:	4283      	cmp	r3, r0
   34070:	d210      	bcs.n	34094 <two_way_long_needle+0x1d8>
   34072:	f816 c003 	ldrb.w	ip, [r6, r3]
   34076:	5ce0      	ldrb	r0, [r4, r3]
   34078:	4584      	cmp	ip, r0
   3407a:	d0f6      	beq.n	3406a <two_way_long_needle+0x1ae>
   3407c:	4490      	add	r8, r2
   3407e:	e768      	b.n	33f52 <two_way_long_needle+0x96>
   34080:	462b      	mov	r3, r5
   34082:	e7f3      	b.n	3406c <two_way_long_needle+0x1b0>
   34084:	192a      	adds	r2, r5, r4
   34086:	eb06 0b05 	add.w	fp, r6, r5
   3408a:	462b      	mov	r3, r5
   3408c:	4452      	add	r2, sl
   3408e:	e7d5      	b.n	3403c <two_way_long_needle+0x180>
   34090:	4620      	mov	r0, r4
   34092:	e7e2      	b.n	3405a <two_way_long_needle+0x19e>
   34094:	eb05 0308 	add.w	r3, r5, r8
   34098:	1970      	adds	r0, r6, r5
   3409a:	4453      	add	r3, sl
   3409c:	4286      	cmp	r6, r0
   3409e:	d0f7      	beq.n	34090 <two_way_long_needle+0x1d4>
   340a0:	f810 ed01 	ldrb.w	lr, [r0, #-1]!
   340a4:	f813 cd01 	ldrb.w	ip, [r3, #-1]!
   340a8:	45e6      	cmp	lr, ip
   340aa:	d0f7      	beq.n	3409c <two_way_long_needle+0x1e0>
   340ac:	44d8      	add	r8, fp
   340ae:	eba9 0307 	sub.w	r3, r9, r7
   340b2:	4598      	cmp	r8, r3
   340b4:	f67f af43 	bls.w	33f3e <two_way_long_needle+0x82>
   340b8:	eb0a 0009 	add.w	r0, sl, r9
   340bc:	e9cd 1202 	strd	r1, r2, [sp, #8]
   340c0:	f006 ffe6 	bl	3b090 <strnlen>
   340c4:	4481      	add	r9, r0
   340c6:	eba9 0307 	sub.w	r3, r9, r7
   340ca:	4598      	cmp	r8, r3
   340cc:	e9dd 1202 	ldrd	r1, r2, [sp, #8]
   340d0:	f67f af35 	bls.w	33f3e <two_way_long_needle+0x82>
   340d4:	e795      	b.n	34002 <two_way_long_needle+0x146>

000340d6 <strstr>:
   340d6:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
   340da:	460f      	mov	r7, r1
   340dc:	7809      	ldrb	r1, [r1, #0]
   340de:	b090      	sub	sp, #64	; 0x40
   340e0:	4604      	mov	r4, r0
   340e2:	2900      	cmp	r1, #0
   340e4:	d035      	beq.n	34152 <strstr+0x7c>
   340e6:	787b      	ldrb	r3, [r7, #1]
   340e8:	b923      	cbnz	r3, 340f4 <strstr+0x1e>
   340ea:	b010      	add	sp, #64	; 0x40
   340ec:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
   340f0:	f7ff bd7a 	b.w	33be8 <strchr>
   340f4:	78ba      	ldrb	r2, [r7, #2]
   340f6:	b972      	cbnz	r2, 34116 <strstr+0x40>
   340f8:	ea43 4101 	orr.w	r1, r3, r1, lsl #16
   340fc:	7800      	ldrb	r0, [r0, #0]
   340fe:	4613      	mov	r3, r2
   34100:	b908      	cbnz	r0, 34106 <strstr+0x30>
   34102:	2400      	movs	r4, #0
   34104:	e025      	b.n	34152 <strstr+0x7c>
   34106:	ea40 4303 	orr.w	r3, r0, r3, lsl #16
   3410a:	1c62      	adds	r2, r4, #1
   3410c:	7860      	ldrb	r0, [r4, #1]
   3410e:	4299      	cmp	r1, r3
   34110:	d07c      	beq.n	3420c <strstr+0x136>
   34112:	4614      	mov	r4, r2
   34114:	e7f4      	b.n	34100 <strstr+0x2a>
   34116:	78f8      	ldrb	r0, [r7, #3]
   34118:	b990      	cbnz	r0, 34140 <strstr+0x6a>
   3411a:	041b      	lsls	r3, r3, #16
   3411c:	ea43 6301 	orr.w	r3, r3, r1, lsl #24
   34120:	ea43 2302 	orr.w	r3, r3, r2, lsl #8
   34124:	7822      	ldrb	r2, [r4, #0]
   34126:	2a00      	cmp	r2, #0
   34128:	d0eb      	beq.n	34102 <strstr+0x2c>
   3412a:	ea42 0100 	orr.w	r1, r2, r0
   3412e:	1c65      	adds	r5, r4, #1
   34130:	7862      	ldrb	r2, [r4, #1]
   34132:	ebb3 2f01 	cmp.w	r3, r1, lsl #8
   34136:	ea4f 2001 	mov.w	r0, r1, lsl #8
   3413a:	d069      	beq.n	34210 <strstr+0x13a>
   3413c:	462c      	mov	r4, r5
   3413e:	e7f2      	b.n	34126 <strstr+0x50>
   34140:	793b      	ldrb	r3, [r7, #4]
   34142:	b993      	cbnz	r3, 3416a <strstr+0x94>
   34144:	683a      	ldr	r2, [r7, #0]
   34146:	7821      	ldrb	r1, [r4, #0]
   34148:	ba12      	rev	r2, r2
   3414a:	b159      	cbz	r1, 34164 <strstr+0x8e>
   3414c:	429a      	cmp	r2, r3
   3414e:	d104      	bne.n	3415a <strstr+0x84>
   34150:	3c04      	subs	r4, #4
   34152:	4620      	mov	r0, r4
   34154:	b010      	add	sp, #64	; 0x40
   34156:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
   3415a:	ea41 2303 	orr.w	r3, r1, r3, lsl #8
   3415e:	f814 1f01 	ldrb.w	r1, [r4, #1]!
   34162:	e7f2      	b.n	3414a <strstr+0x74>
   34164:	429a      	cmp	r2, r3
   34166:	d1cc      	bne.n	34102 <strstr+0x2c>
   34168:	e7f2      	b.n	34150 <strstr+0x7a>
   3416a:	4638      	mov	r0, r7
   3416c:	f7ff fde0 	bl	33d30 <strlen>
   34170:	4605      	mov	r5, r0
   34172:	f440 7100 	orr.w	r1, r0, #512	; 0x200
   34176:	4620      	mov	r0, r4
   34178:	f006 ff8a 	bl	3b090 <strnlen>
   3417c:	4285      	cmp	r5, r0
   3417e:	4601      	mov	r1, r0
   34180:	d8bf      	bhi.n	34102 <strstr+0x2c>
   34182:	2dfe      	cmp	r5, #254	; 0xfe
   34184:	d83b      	bhi.n	341fe <strstr+0x128>
   34186:	1b40      	subs	r0, r0, r5
   34188:	1c69      	adds	r1, r5, #1
   3418a:	2240      	movs	r2, #64	; 0x40
   3418c:	1826      	adds	r6, r4, r0
   3418e:	4668      	mov	r0, sp
   34190:	f005 ff2e 	bl	39ff0 <memset>
   34194:	1e79      	subs	r1, r7, #1
   34196:	b2eb      	uxtb	r3, r5
   34198:	b9b3      	cbnz	r3, 341c8 <strstr+0xf2>
   3419a:	3c01      	subs	r4, #1
   3419c:	5d63      	ldrb	r3, [r4, r5]
   3419e:	f003 033f 	and.w	r3, r3, #63	; 0x3f
   341a2:	3340      	adds	r3, #64	; 0x40
   341a4:	446b      	add	r3, sp
   341a6:	f813 0c40 	ldrb.w	r0, [r3, #-64]
   341aa:	4404      	add	r4, r0
   341ac:	42b4      	cmp	r4, r6
   341ae:	d916      	bls.n	341de <strstr+0x108>
   341b0:	5d73      	ldrb	r3, [r6, r5]
   341b2:	1970      	adds	r0, r6, r5
   341b4:	2b00      	cmp	r3, #0
   341b6:	d0a4      	beq.n	34102 <strstr+0x2c>
   341b8:	f44f 6100 	mov.w	r1, #2048	; 0x800
   341bc:	f006 ff68 	bl	3b090 <strnlen>
   341c0:	4406      	add	r6, r0
   341c2:	42b4      	cmp	r4, r6
   341c4:	d9e9      	bls.n	3419a <strstr+0xc4>
   341c6:	e79c      	b.n	34102 <strstr+0x2c>
   341c8:	f811 2f01 	ldrb.w	r2, [r1, #1]!
   341cc:	f002 023f 	and.w	r2, r2, #63	; 0x3f
   341d0:	3240      	adds	r2, #64	; 0x40
   341d2:	446a      	add	r2, sp
   341d4:	f802 3c40 	strb.w	r3, [r2, #-64]
   341d8:	3b01      	subs	r3, #1
   341da:	b2db      	uxtb	r3, r3
   341dc:	e7dc      	b.n	34198 <strstr+0xc2>
   341de:	5d63      	ldrb	r3, [r4, r5]
   341e0:	462a      	mov	r2, r5
   341e2:	4639      	mov	r1, r7
   341e4:	4620      	mov	r0, r4
   341e6:	f003 033f 	and.w	r3, r3, #63	; 0x3f
   341ea:	3340      	adds	r3, #64	; 0x40
   341ec:	446b      	add	r3, sp
   341ee:	f813 8c40 	ldrb.w	r8, [r3, #-64]
   341f2:	f005 fe0a 	bl	39e0a <memcmp>
   341f6:	2800      	cmp	r0, #0
   341f8:	d0ab      	beq.n	34152 <strstr+0x7c>
   341fa:	4444      	add	r4, r8
   341fc:	e7d6      	b.n	341ac <strstr+0xd6>
   341fe:	462b      	mov	r3, r5
   34200:	463a      	mov	r2, r7
   34202:	4620      	mov	r0, r4
   34204:	b010      	add	sp, #64	; 0x40
   34206:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
   3420a:	e657      	b.n	33ebc <two_way_long_needle>
   3420c:	3c01      	subs	r4, #1
   3420e:	e7a0      	b.n	34152 <strstr+0x7c>
   34210:	3c02      	subs	r4, #2
   34212:	e79e      	b.n	34152 <strstr+0x7c>

00034214 <strtok>:
   34214:	4b02      	ldr	r3, [pc, #8]	; (34220 <strtok+0xc>)
   34216:	681a      	ldr	r2, [r3, #0]
   34218:	2301      	movs	r3, #1
   3421a:	325c      	adds	r2, #92	; 0x5c
   3421c:	f000 b802 	b.w	34224 <__strtok_r>
   34220:	20006000 	.word	0x20006000

00034224 <__strtok_r>:
   34224:	b5f0      	push	{r4, r5, r6, r7, lr}
   34226:	b908      	cbnz	r0, 3422c <__strtok_r+0x8>
   34228:	6810      	ldr	r0, [r2, #0]
   3422a:	b188      	cbz	r0, 34250 <__strtok_r+0x2c>
   3422c:	4604      	mov	r4, r0
   3422e:	4620      	mov	r0, r4
   34230:	460f      	mov	r7, r1
   34232:	f814 5b01 	ldrb.w	r5, [r4], #1
   34236:	f817 6b01 	ldrb.w	r6, [r7], #1
   3423a:	b91e      	cbnz	r6, 34244 <__strtok_r+0x20>
   3423c:	b965      	cbnz	r5, 34258 <__strtok_r+0x34>
   3423e:	4628      	mov	r0, r5
   34240:	6015      	str	r5, [r2, #0]
   34242:	e005      	b.n	34250 <__strtok_r+0x2c>
   34244:	42b5      	cmp	r5, r6
   34246:	d1f6      	bne.n	34236 <__strtok_r+0x12>
   34248:	2b00      	cmp	r3, #0
   3424a:	d1f0      	bne.n	3422e <__strtok_r+0xa>
   3424c:	6014      	str	r4, [r2, #0]
   3424e:	7003      	strb	r3, [r0, #0]
   34250:	bdf0      	pop	{r4, r5, r6, r7, pc}
   34252:	461c      	mov	r4, r3
   34254:	e00c      	b.n	34270 <__strtok_r+0x4c>
   34256:	b915      	cbnz	r5, 3425e <__strtok_r+0x3a>
   34258:	f814 3b01 	ldrb.w	r3, [r4], #1
   3425c:	460e      	mov	r6, r1
   3425e:	f816 5b01 	ldrb.w	r5, [r6], #1
   34262:	42ab      	cmp	r3, r5
   34264:	d1f7      	bne.n	34256 <__strtok_r+0x32>
   34266:	2b00      	cmp	r3, #0
   34268:	d0f3      	beq.n	34252 <__strtok_r+0x2e>
   3426a:	2300      	movs	r3, #0
   3426c:	f804 3c01 	strb.w	r3, [r4, #-1]
   34270:	6014      	str	r4, [r2, #0]
   34272:	e7ed      	b.n	34250 <__strtok_r+0x2c>

00034274 <strtok_r>:
   34274:	2301      	movs	r3, #1
   34276:	f7ff bfd5 	b.w	34224 <__strtok_r>
	...

0003427c <_strtol_l.constprop.0>:
   3427c:	2b01      	cmp	r3, #1
   3427e:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
   34282:	d001      	beq.n	34288 <_strtol_l.constprop.0+0xc>
   34284:	2b24      	cmp	r3, #36	; 0x24
   34286:	d906      	bls.n	34296 <_strtol_l.constprop.0+0x1a>
   34288:	f004 fde0 	bl	38e4c <__errno>
   3428c:	2316      	movs	r3, #22
   3428e:	6003      	str	r3, [r0, #0]
   34290:	2000      	movs	r0, #0
   34292:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
   34296:	460d      	mov	r5, r1
   34298:	f8df c0e0 	ldr.w	ip, [pc, #224]	; 3437c <_strtol_l.constprop.0+0x100>
   3429c:	462e      	mov	r6, r5
   3429e:	f815 4b01 	ldrb.w	r4, [r5], #1
   342a2:	f814 700c 	ldrb.w	r7, [r4, ip]
   342a6:	f017 0708 	ands.w	r7, r7, #8
   342aa:	d1f7      	bne.n	3429c <_strtol_l.constprop.0+0x20>
   342ac:	2c2d      	cmp	r4, #45	; 0x2d
   342ae:	d132      	bne.n	34316 <_strtol_l.constprop.0+0x9a>
   342b0:	782c      	ldrb	r4, [r5, #0]
   342b2:	2701      	movs	r7, #1
   342b4:	1cb5      	adds	r5, r6, #2
   342b6:	2b00      	cmp	r3, #0
   342b8:	d05b      	beq.n	34372 <_strtol_l.constprop.0+0xf6>
   342ba:	2b10      	cmp	r3, #16
   342bc:	d109      	bne.n	342d2 <_strtol_l.constprop.0+0x56>
   342be:	2c30      	cmp	r4, #48	; 0x30
   342c0:	d107      	bne.n	342d2 <_strtol_l.constprop.0+0x56>
   342c2:	782c      	ldrb	r4, [r5, #0]
   342c4:	f004 04df 	and.w	r4, r4, #223	; 0xdf
   342c8:	2c58      	cmp	r4, #88	; 0x58
   342ca:	d14d      	bne.n	34368 <_strtol_l.constprop.0+0xec>
   342cc:	786c      	ldrb	r4, [r5, #1]
   342ce:	2310      	movs	r3, #16
   342d0:	3502      	adds	r5, #2
   342d2:	f107 4800 	add.w	r8, r7, #2147483648	; 0x80000000
   342d6:	f04f 0c00 	mov.w	ip, #0
   342da:	f108 38ff 	add.w	r8, r8, #4294967295	; 0xffffffff
   342de:	4666      	mov	r6, ip
   342e0:	fbb8 f9f3 	udiv	r9, r8, r3
   342e4:	fb03 8a19 	mls	sl, r3, r9, r8
   342e8:	f1a4 0e30 	sub.w	lr, r4, #48	; 0x30
   342ec:	f1be 0f09 	cmp.w	lr, #9
   342f0:	d816      	bhi.n	34320 <_strtol_l.constprop.0+0xa4>
   342f2:	4674      	mov	r4, lr
   342f4:	42a3      	cmp	r3, r4
   342f6:	dd24      	ble.n	34342 <_strtol_l.constprop.0+0xc6>
   342f8:	f1bc 0f00 	cmp.w	ip, #0
   342fc:	db1e      	blt.n	3433c <_strtol_l.constprop.0+0xc0>
   342fe:	45b1      	cmp	r9, r6
   34300:	d31c      	bcc.n	3433c <_strtol_l.constprop.0+0xc0>
   34302:	d101      	bne.n	34308 <_strtol_l.constprop.0+0x8c>
   34304:	45a2      	cmp	sl, r4
   34306:	db19      	blt.n	3433c <_strtol_l.constprop.0+0xc0>
   34308:	fb06 4603 	mla	r6, r6, r3, r4
   3430c:	f04f 0c01 	mov.w	ip, #1
   34310:	f815 4b01 	ldrb.w	r4, [r5], #1
   34314:	e7e8      	b.n	342e8 <_strtol_l.constprop.0+0x6c>
   34316:	2c2b      	cmp	r4, #43	; 0x2b
   34318:	bf04      	itt	eq
   3431a:	782c      	ldrbeq	r4, [r5, #0]
   3431c:	1cb5      	addeq	r5, r6, #2
   3431e:	e7ca      	b.n	342b6 <_strtol_l.constprop.0+0x3a>
   34320:	f1a4 0e41 	sub.w	lr, r4, #65	; 0x41
   34324:	f1be 0f19 	cmp.w	lr, #25
   34328:	d801      	bhi.n	3432e <_strtol_l.constprop.0+0xb2>
   3432a:	3c37      	subs	r4, #55	; 0x37
   3432c:	e7e2      	b.n	342f4 <_strtol_l.constprop.0+0x78>
   3432e:	f1a4 0e61 	sub.w	lr, r4, #97	; 0x61
   34332:	f1be 0f19 	cmp.w	lr, #25
   34336:	d804      	bhi.n	34342 <_strtol_l.constprop.0+0xc6>
   34338:	3c57      	subs	r4, #87	; 0x57
   3433a:	e7db      	b.n	342f4 <_strtol_l.constprop.0+0x78>
   3433c:	f04f 3cff 	mov.w	ip, #4294967295	; 0xffffffff
   34340:	e7e6      	b.n	34310 <_strtol_l.constprop.0+0x94>
   34342:	f1bc 0f00 	cmp.w	ip, #0
   34346:	da05      	bge.n	34354 <_strtol_l.constprop.0+0xd8>
   34348:	2322      	movs	r3, #34	; 0x22
   3434a:	4646      	mov	r6, r8
   3434c:	6003      	str	r3, [r0, #0]
   3434e:	b942      	cbnz	r2, 34362 <_strtol_l.constprop.0+0xe6>
   34350:	4630      	mov	r0, r6
   34352:	e79e      	b.n	34292 <_strtol_l.constprop.0+0x16>
   34354:	b107      	cbz	r7, 34358 <_strtol_l.constprop.0+0xdc>
   34356:	4276      	negs	r6, r6
   34358:	2a00      	cmp	r2, #0
   3435a:	d0f9      	beq.n	34350 <_strtol_l.constprop.0+0xd4>
   3435c:	f1bc 0f00 	cmp.w	ip, #0
   34360:	d000      	beq.n	34364 <_strtol_l.constprop.0+0xe8>
   34362:	1e69      	subs	r1, r5, #1
   34364:	6011      	str	r1, [r2, #0]
   34366:	e7f3      	b.n	34350 <_strtol_l.constprop.0+0xd4>
   34368:	2430      	movs	r4, #48	; 0x30
   3436a:	2b00      	cmp	r3, #0
   3436c:	d1b1      	bne.n	342d2 <_strtol_l.constprop.0+0x56>
   3436e:	2308      	movs	r3, #8
   34370:	e7af      	b.n	342d2 <_strtol_l.constprop.0+0x56>
   34372:	2c30      	cmp	r4, #48	; 0x30
   34374:	d0a5      	beq.n	342c2 <_strtol_l.constprop.0+0x46>
   34376:	230a      	movs	r3, #10
   34378:	e7ab      	b.n	342d2 <_strtol_l.constprop.0+0x56>
   3437a:	bf00      	nop
   3437c:	0004056b 	.word	0x0004056b

00034380 <_strtol_r>:
   34380:	e77c      	b.n	3427c <_strtol_l.constprop.0>

00034382 <strtol_l>:
   34382:	4613      	mov	r3, r2
   34384:	460a      	mov	r2, r1
   34386:	4601      	mov	r1, r0
   34388:	4801      	ldr	r0, [pc, #4]	; (34390 <strtol_l+0xe>)
   3438a:	6800      	ldr	r0, [r0, #0]
   3438c:	e776      	b.n	3427c <_strtol_l.constprop.0>
   3438e:	bf00      	nop
   34390:	20006000 	.word	0x20006000

00034394 <strtol>:
   34394:	4613      	mov	r3, r2
   34396:	460a      	mov	r2, r1
   34398:	4601      	mov	r1, r0
   3439a:	4801      	ldr	r0, [pc, #4]	; (343a0 <strtol+0xc>)
   3439c:	6800      	ldr	r0, [r0, #0]
   3439e:	e76d      	b.n	3427c <_strtol_l.constprop.0>
   343a0:	20006000 	.word	0x20006000
   343a4:	00000000 	.word	0x00000000

000343a8 <_svfprintf_r>:
   343a8:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   343ac:	ed2d 8b04 	vpush	{d8-d9}
   343b0:	b0cf      	sub	sp, #316	; 0x13c
   343b2:	4688      	mov	r8, r1
   343b4:	4691      	mov	r9, r2
   343b6:	461e      	mov	r6, r3
   343b8:	4682      	mov	sl, r0
   343ba:	f005 f9b9 	bl	39730 <_localeconv_r>
   343be:	6803      	ldr	r3, [r0, #0]
   343c0:	4618      	mov	r0, r3
   343c2:	9313      	str	r3, [sp, #76]	; 0x4c
   343c4:	f7ff fcb4 	bl	33d30 <strlen>
   343c8:	f8b8 300c 	ldrh.w	r3, [r8, #12]
   343cc:	900d      	str	r0, [sp, #52]	; 0x34
   343ce:	0618      	lsls	r0, r3, #24
   343d0:	d51a      	bpl.n	34408 <_svfprintf_r+0x60>
   343d2:	f8d8 3010 	ldr.w	r3, [r8, #16]
   343d6:	b9bb      	cbnz	r3, 34408 <_svfprintf_r+0x60>
   343d8:	2140      	movs	r1, #64	; 0x40
   343da:	4650      	mov	r0, sl
   343dc:	f005 fa70 	bl	398c0 <_malloc_r>
   343e0:	f8c8 0000 	str.w	r0, [r8]
   343e4:	f8c8 0010 	str.w	r0, [r8, #16]
   343e8:	b958      	cbnz	r0, 34402 <_svfprintf_r+0x5a>
   343ea:	230c      	movs	r3, #12
   343ec:	f8ca 3000 	str.w	r3, [sl]
   343f0:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   343f4:	930c      	str	r3, [sp, #48]	; 0x30
   343f6:	980c      	ldr	r0, [sp, #48]	; 0x30
   343f8:	b04f      	add	sp, #316	; 0x13c
   343fa:	ecbd 8b04 	vpop	{d8-d9}
   343fe:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   34402:	2340      	movs	r3, #64	; 0x40
   34404:	f8c8 3014 	str.w	r3, [r8, #20]
   34408:	2500      	movs	r5, #0
   3440a:	ac25      	add	r4, sp, #148	; 0x94
   3440c:	ed9f 7b92 	vldr	d7, [pc, #584]	; 34658 <_svfprintf_r+0x2b0>
   34410:	9422      	str	r4, [sp, #136]	; 0x88
   34412:	9505      	str	r5, [sp, #20]
   34414:	950a      	str	r5, [sp, #40]	; 0x28
   34416:	9512      	str	r5, [sp, #72]	; 0x48
   34418:	9514      	str	r5, [sp, #80]	; 0x50
   3441a:	950c      	str	r5, [sp, #48]	; 0x30
   3441c:	e9cd 5523 	strd	r5, r5, [sp, #140]	; 0x8c
   34420:	ed8d 7b0e 	vstr	d7, [sp, #56]	; 0x38
   34424:	e9cd 5515 	strd	r5, r5, [sp, #84]	; 0x54
   34428:	464b      	mov	r3, r9
   3442a:	461d      	mov	r5, r3
   3442c:	f813 2b01 	ldrb.w	r2, [r3], #1
   34430:	b10a      	cbz	r2, 34436 <_svfprintf_r+0x8e>
   34432:	2a25      	cmp	r2, #37	; 0x25
   34434:	d1f9      	bne.n	3442a <_svfprintf_r+0x82>
   34436:	ebb5 0709 	subs.w	r7, r5, r9
   3443a:	d00d      	beq.n	34458 <_svfprintf_r+0xb0>
   3443c:	9b24      	ldr	r3, [sp, #144]	; 0x90
   3443e:	443b      	add	r3, r7
   34440:	9324      	str	r3, [sp, #144]	; 0x90
   34442:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   34444:	3301      	adds	r3, #1
   34446:	2b07      	cmp	r3, #7
   34448:	9323      	str	r3, [sp, #140]	; 0x8c
   3444a:	e9c4 9700 	strd	r9, r7, [r4]
   3444e:	dc79      	bgt.n	34544 <_svfprintf_r+0x19c>
   34450:	3408      	adds	r4, #8
   34452:	9b0c      	ldr	r3, [sp, #48]	; 0x30
   34454:	443b      	add	r3, r7
   34456:	930c      	str	r3, [sp, #48]	; 0x30
   34458:	782b      	ldrb	r3, [r5, #0]
   3445a:	2b00      	cmp	r3, #0
   3445c:	f001 8143 	beq.w	356e6 <_svfprintf_r+0x133e>
   34460:	2300      	movs	r3, #0
   34462:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   34466:	3501      	adds	r5, #1
   34468:	270a      	movs	r7, #10
   3446a:	9204      	str	r2, [sp, #16]
   3446c:	469b      	mov	fp, r3
   3446e:	222b      	movs	r2, #43	; 0x2b
   34470:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   34474:	9310      	str	r3, [sp, #64]	; 0x40
   34476:	462b      	mov	r3, r5
   34478:	f813 1b01 	ldrb.w	r1, [r3], #1
   3447c:	9106      	str	r1, [sp, #24]
   3447e:	930b      	str	r3, [sp, #44]	; 0x2c
   34480:	9b06      	ldr	r3, [sp, #24]
   34482:	3b20      	subs	r3, #32
   34484:	2b5a      	cmp	r3, #90	; 0x5a
   34486:	f200 85ba 	bhi.w	34ffe <_svfprintf_r+0xc56>
   3448a:	e8df f013 	tbh	[pc, r3, lsl #1]
   3448e:	007e      	.short	0x007e
   34490:	05b805b8 	.word	0x05b805b8
   34494:	05b80086 	.word	0x05b80086
   34498:	05b805b8 	.word	0x05b805b8
   3449c:	05b80065 	.word	0x05b80065
   344a0:	008905b8 	.word	0x008905b8
   344a4:	05b80093 	.word	0x05b80093
   344a8:	00960090 	.word	0x00960090
   344ac:	00b305b8 	.word	0x00b305b8
   344b0:	00b600b6 	.word	0x00b600b6
   344b4:	00b600b6 	.word	0x00b600b6
   344b8:	00b600b6 	.word	0x00b600b6
   344bc:	00b600b6 	.word	0x00b600b6
   344c0:	05b800b6 	.word	0x05b800b6
   344c4:	05b805b8 	.word	0x05b805b8
   344c8:	05b805b8 	.word	0x05b805b8
   344cc:	05b805b8 	.word	0x05b805b8
   344d0:	05b8012c 	.word	0x05b8012c
   344d4:	00fc00e9 	.word	0x00fc00e9
   344d8:	012c012c 	.word	0x012c012c
   344dc:	05b8012c 	.word	0x05b8012c
   344e0:	05b805b8 	.word	0x05b805b8
   344e4:	00c605b8 	.word	0x00c605b8
   344e8:	05b805b8 	.word	0x05b805b8
   344ec:	05b80494 	.word	0x05b80494
   344f0:	05b805b8 	.word	0x05b805b8
   344f4:	05b804dd 	.word	0x05b804dd
   344f8:	05b804fe 	.word	0x05b804fe
   344fc:	052005b8 	.word	0x052005b8
   34500:	05b805b8 	.word	0x05b805b8
   34504:	05b805b8 	.word	0x05b805b8
   34508:	05b805b8 	.word	0x05b805b8
   3450c:	05b805b8 	.word	0x05b805b8
   34510:	05b8012c 	.word	0x05b8012c
   34514:	00fe00e9 	.word	0x00fe00e9
   34518:	012c012c 	.word	0x012c012c
   3451c:	00c9012c 	.word	0x00c9012c
   34520:	00dd00fe 	.word	0x00dd00fe
   34524:	00d605b8 	.word	0x00d605b8
   34528:	046f05b8 	.word	0x046f05b8
   3452c:	04cc0496 	.word	0x04cc0496
   34530:	05b800dd 	.word	0x05b800dd
   34534:	007c04dd 	.word	0x007c04dd
   34538:	05b80500 	.word	0x05b80500
   3453c:	053f05b8 	.word	0x053f05b8
   34540:	007c05b8 	.word	0x007c05b8
   34544:	aa22      	add	r2, sp, #136	; 0x88
   34546:	4641      	mov	r1, r8
   34548:	4650      	mov	r0, sl
   3454a:	f007 fe75 	bl	3c238 <__ssprint_r>
   3454e:	2800      	cmp	r0, #0
   34550:	f040 8137 	bne.w	347c2 <_svfprintf_r+0x41a>
   34554:	ac25      	add	r4, sp, #148	; 0x94
   34556:	e77c      	b.n	34452 <_svfprintf_r+0xaa>
   34558:	4650      	mov	r0, sl
   3455a:	f005 f8e9 	bl	39730 <_localeconv_r>
   3455e:	6843      	ldr	r3, [r0, #4]
   34560:	4618      	mov	r0, r3
   34562:	9314      	str	r3, [sp, #80]	; 0x50
   34564:	f7ff fbe4 	bl	33d30 <strlen>
   34568:	9012      	str	r0, [sp, #72]	; 0x48
   3456a:	4650      	mov	r0, sl
   3456c:	f005 f8e0 	bl	39730 <_localeconv_r>
   34570:	6883      	ldr	r3, [r0, #8]
   34572:	222b      	movs	r2, #43	; 0x2b
   34574:	930a      	str	r3, [sp, #40]	; 0x28
   34576:	9b12      	ldr	r3, [sp, #72]	; 0x48
   34578:	b12b      	cbz	r3, 34586 <_svfprintf_r+0x1de>
   3457a:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   3457c:	b11b      	cbz	r3, 34586 <_svfprintf_r+0x1de>
   3457e:	781b      	ldrb	r3, [r3, #0]
   34580:	b10b      	cbz	r3, 34586 <_svfprintf_r+0x1de>
   34582:	f44b 6b80 	orr.w	fp, fp, #1024	; 0x400
   34586:	9d0b      	ldr	r5, [sp, #44]	; 0x2c
   34588:	e775      	b.n	34476 <_svfprintf_r+0xce>
   3458a:	f89d 306b 	ldrb.w	r3, [sp, #107]	; 0x6b
   3458e:	2b00      	cmp	r3, #0
   34590:	d1f9      	bne.n	34586 <_svfprintf_r+0x1de>
   34592:	2320      	movs	r3, #32
   34594:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   34598:	e7f5      	b.n	34586 <_svfprintf_r+0x1de>
   3459a:	f04b 0b01 	orr.w	fp, fp, #1
   3459e:	e7f2      	b.n	34586 <_svfprintf_r+0x1de>
   345a0:	f856 3b04 	ldr.w	r3, [r6], #4
   345a4:	2b00      	cmp	r3, #0
   345a6:	9310      	str	r3, [sp, #64]	; 0x40
   345a8:	daed      	bge.n	34586 <_svfprintf_r+0x1de>
   345aa:	425b      	negs	r3, r3
   345ac:	9310      	str	r3, [sp, #64]	; 0x40
   345ae:	f04b 0b04 	orr.w	fp, fp, #4
   345b2:	e7e8      	b.n	34586 <_svfprintf_r+0x1de>
   345b4:	f88d 206b 	strb.w	r2, [sp, #107]	; 0x6b
   345b8:	e7e5      	b.n	34586 <_svfprintf_r+0x1de>
   345ba:	9d0b      	ldr	r5, [sp, #44]	; 0x2c
   345bc:	f815 3b01 	ldrb.w	r3, [r5], #1
   345c0:	2b2a      	cmp	r3, #42	; 0x2a
   345c2:	9306      	str	r3, [sp, #24]
   345c4:	d113      	bne.n	345ee <_svfprintf_r+0x246>
   345c6:	f856 0b04 	ldr.w	r0, [r6], #4
   345ca:	950b      	str	r5, [sp, #44]	; 0x2c
   345cc:	ea40 73e0 	orr.w	r3, r0, r0, asr #31
   345d0:	9304      	str	r3, [sp, #16]
   345d2:	e7d8      	b.n	34586 <_svfprintf_r+0x1de>
   345d4:	9904      	ldr	r1, [sp, #16]
   345d6:	fb07 3301 	mla	r3, r7, r1, r3
   345da:	9304      	str	r3, [sp, #16]
   345dc:	f815 3b01 	ldrb.w	r3, [r5], #1
   345e0:	9306      	str	r3, [sp, #24]
   345e2:	9b06      	ldr	r3, [sp, #24]
   345e4:	3b30      	subs	r3, #48	; 0x30
   345e6:	2b09      	cmp	r3, #9
   345e8:	d9f4      	bls.n	345d4 <_svfprintf_r+0x22c>
   345ea:	950b      	str	r5, [sp, #44]	; 0x2c
   345ec:	e748      	b.n	34480 <_svfprintf_r+0xd8>
   345ee:	2300      	movs	r3, #0
   345f0:	9304      	str	r3, [sp, #16]
   345f2:	e7f6      	b.n	345e2 <_svfprintf_r+0x23a>
   345f4:	f04b 0b80 	orr.w	fp, fp, #128	; 0x80
   345f8:	e7c5      	b.n	34586 <_svfprintf_r+0x1de>
   345fa:	2300      	movs	r3, #0
   345fc:	9d0b      	ldr	r5, [sp, #44]	; 0x2c
   345fe:	9310      	str	r3, [sp, #64]	; 0x40
   34600:	9b06      	ldr	r3, [sp, #24]
   34602:	9910      	ldr	r1, [sp, #64]	; 0x40
   34604:	3b30      	subs	r3, #48	; 0x30
   34606:	fb07 3301 	mla	r3, r7, r1, r3
   3460a:	9310      	str	r3, [sp, #64]	; 0x40
   3460c:	f815 3b01 	ldrb.w	r3, [r5], #1
   34610:	9306      	str	r3, [sp, #24]
   34612:	3b30      	subs	r3, #48	; 0x30
   34614:	2b09      	cmp	r3, #9
   34616:	d9f3      	bls.n	34600 <_svfprintf_r+0x258>
   34618:	e7e7      	b.n	345ea <_svfprintf_r+0x242>
   3461a:	f04b 0b08 	orr.w	fp, fp, #8
   3461e:	e7b2      	b.n	34586 <_svfprintf_r+0x1de>
   34620:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   34622:	781b      	ldrb	r3, [r3, #0]
   34624:	2b68      	cmp	r3, #104	; 0x68
   34626:	bf05      	ittet	eq
   34628:	9b0b      	ldreq	r3, [sp, #44]	; 0x2c
   3462a:	f44b 7b00 	orreq.w	fp, fp, #512	; 0x200
   3462e:	f04b 0b40 	orrne.w	fp, fp, #64	; 0x40
   34632:	3301      	addeq	r3, #1
   34634:	bf08      	it	eq
   34636:	930b      	streq	r3, [sp, #44]	; 0x2c
   34638:	e7a5      	b.n	34586 <_svfprintf_r+0x1de>
   3463a:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   3463c:	781b      	ldrb	r3, [r3, #0]
   3463e:	2b6c      	cmp	r3, #108	; 0x6c
   34640:	d105      	bne.n	3464e <_svfprintf_r+0x2a6>
   34642:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   34644:	3301      	adds	r3, #1
   34646:	930b      	str	r3, [sp, #44]	; 0x2c
   34648:	f04b 0b20 	orr.w	fp, fp, #32
   3464c:	e79b      	b.n	34586 <_svfprintf_r+0x1de>
   3464e:	f04b 0b10 	orr.w	fp, fp, #16
   34652:	e798      	b.n	34586 <_svfprintf_r+0x1de>
   34654:	f3af 8000 	nop.w
	...
   34660:	4632      	mov	r2, r6
   34662:	2000      	movs	r0, #0
   34664:	f852 3b04 	ldr.w	r3, [r2], #4
   34668:	f88d 006b 	strb.w	r0, [sp, #107]	; 0x6b
   3466c:	9207      	str	r2, [sp, #28]
   3466e:	f88d 30d4 	strb.w	r3, [sp, #212]	; 0xd4
   34672:	2301      	movs	r3, #1
   34674:	4606      	mov	r6, r0
   34676:	4605      	mov	r5, r0
   34678:	f10d 09d4 	add.w	r9, sp, #212	; 0xd4
   3467c:	9003      	str	r0, [sp, #12]
   3467e:	9304      	str	r3, [sp, #16]
   34680:	e9cd 0008 	strd	r0, r0, [sp, #32]
   34684:	e1b9      	b.n	349fa <_svfprintf_r+0x652>
   34686:	f04b 0b10 	orr.w	fp, fp, #16
   3468a:	f01b 0f20 	tst.w	fp, #32
   3468e:	d012      	beq.n	346b6 <_svfprintf_r+0x30e>
   34690:	1df3      	adds	r3, r6, #7
   34692:	f023 0307 	bic.w	r3, r3, #7
   34696:	461a      	mov	r2, r3
   34698:	685d      	ldr	r5, [r3, #4]
   3469a:	f852 6b08 	ldr.w	r6, [r2], #8
   3469e:	9207      	str	r2, [sp, #28]
   346a0:	2d00      	cmp	r5, #0
   346a2:	da06      	bge.n	346b2 <_svfprintf_r+0x30a>
   346a4:	4276      	negs	r6, r6
   346a6:	f04f 032d 	mov.w	r3, #45	; 0x2d
   346aa:	eb65 0545 	sbc.w	r5, r5, r5, lsl #1
   346ae:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   346b2:	2301      	movs	r3, #1
   346b4:	e392      	b.n	34ddc <_svfprintf_r+0xa34>
   346b6:	4633      	mov	r3, r6
   346b8:	f01b 0f10 	tst.w	fp, #16
   346bc:	f853 5b04 	ldr.w	r5, [r3], #4
   346c0:	9307      	str	r3, [sp, #28]
   346c2:	d002      	beq.n	346ca <_svfprintf_r+0x322>
   346c4:	462e      	mov	r6, r5
   346c6:	17ed      	asrs	r5, r5, #31
   346c8:	e7ea      	b.n	346a0 <_svfprintf_r+0x2f8>
   346ca:	f01b 0f40 	tst.w	fp, #64	; 0x40
   346ce:	d003      	beq.n	346d8 <_svfprintf_r+0x330>
   346d0:	b22e      	sxth	r6, r5
   346d2:	f345 35c0 	sbfx	r5, r5, #15, #1
   346d6:	e7e3      	b.n	346a0 <_svfprintf_r+0x2f8>
   346d8:	f41b 7f00 	tst.w	fp, #512	; 0x200
   346dc:	d0f2      	beq.n	346c4 <_svfprintf_r+0x31c>
   346de:	b26e      	sxtb	r6, r5
   346e0:	f345 15c0 	sbfx	r5, r5, #7, #1
   346e4:	e7dc      	b.n	346a0 <_svfprintf_r+0x2f8>
   346e6:	3607      	adds	r6, #7
   346e8:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   346ec:	f026 0307 	bic.w	r3, r6, #7
   346f0:	ecb3 7b02 	vldmia	r3!, {d7}
   346f4:	ed8d 7b0e 	vstr	d7, [sp, #56]	; 0x38
   346f8:	9307      	str	r3, [sp, #28]
   346fa:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   346fc:	ee09 3a10 	vmov	s18, r3
   34700:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
   34702:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
   34706:	ee09 3a90 	vmov	s19, r3
   3470a:	4b3a      	ldr	r3, [pc, #232]	; (347f4 <_svfprintf_r+0x44c>)
   3470c:	ec51 0b19 	vmov	r0, r1, d9
   34710:	f00b fbae 	bl	3fe70 <__aeabi_dcmpun>
   34714:	bb10      	cbnz	r0, 3475c <_svfprintf_r+0x3b4>
   34716:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3471a:	4b36      	ldr	r3, [pc, #216]	; (347f4 <_svfprintf_r+0x44c>)
   3471c:	ec51 0b19 	vmov	r0, r1, d9
   34720:	f00b fb88 	bl	3fe34 <__aeabi_dcmple>
   34724:	b9d0      	cbnz	r0, 3475c <_svfprintf_r+0x3b4>
   34726:	2200      	movs	r2, #0
   34728:	2300      	movs	r3, #0
   3472a:	e9dd 010e 	ldrd	r0, r1, [sp, #56]	; 0x38
   3472e:	f00b fb77 	bl	3fe20 <__aeabi_dcmplt>
   34732:	b110      	cbz	r0, 3473a <_svfprintf_r+0x392>
   34734:	232d      	movs	r3, #45	; 0x2d
   34736:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   3473a:	4a2f      	ldr	r2, [pc, #188]	; (347f8 <_svfprintf_r+0x450>)
   3473c:	482f      	ldr	r0, [pc, #188]	; (347fc <_svfprintf_r+0x454>)
   3473e:	9b06      	ldr	r3, [sp, #24]
   34740:	2100      	movs	r1, #0
   34742:	f02b 0b80 	bic.w	fp, fp, #128	; 0x80
   34746:	2b47      	cmp	r3, #71	; 0x47
   34748:	bfd4      	ite	le
   3474a:	4691      	movle	r9, r2
   3474c:	4681      	movgt	r9, r0
   3474e:	2303      	movs	r3, #3
   34750:	e9cd 1303 	strd	r1, r3, [sp, #12]
   34754:	2600      	movs	r6, #0
   34756:	4633      	mov	r3, r6
   34758:	f000 bffb 	b.w	35752 <_svfprintf_r+0x13aa>
   3475c:	e9dd 230e 	ldrd	r2, r3, [sp, #56]	; 0x38
   34760:	4610      	mov	r0, r2
   34762:	4619      	mov	r1, r3
   34764:	f00b fb84 	bl	3fe70 <__aeabi_dcmpun>
   34768:	b140      	cbz	r0, 3477c <_svfprintf_r+0x3d4>
   3476a:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
   3476c:	4a24      	ldr	r2, [pc, #144]	; (34800 <_svfprintf_r+0x458>)
   3476e:	2b00      	cmp	r3, #0
   34770:	4824      	ldr	r0, [pc, #144]	; (34804 <_svfprintf_r+0x45c>)
   34772:	bfbc      	itt	lt
   34774:	232d      	movlt	r3, #45	; 0x2d
   34776:	f88d 306b 	strblt.w	r3, [sp, #107]	; 0x6b
   3477a:	e7e0      	b.n	3473e <_svfprintf_r+0x396>
   3477c:	9b06      	ldr	r3, [sp, #24]
   3477e:	f023 0320 	bic.w	r3, r3, #32
   34782:	2b41      	cmp	r3, #65	; 0x41
   34784:	9308      	str	r3, [sp, #32]
   34786:	d125      	bne.n	347d4 <_svfprintf_r+0x42c>
   34788:	2330      	movs	r3, #48	; 0x30
   3478a:	f04b 0b02 	orr.w	fp, fp, #2
   3478e:	f88d 306c 	strb.w	r3, [sp, #108]	; 0x6c
   34792:	9b06      	ldr	r3, [sp, #24]
   34794:	2b61      	cmp	r3, #97	; 0x61
   34796:	bf0c      	ite	eq
   34798:	2378      	moveq	r3, #120	; 0x78
   3479a:	2358      	movne	r3, #88	; 0x58
   3479c:	f88d 306d 	strb.w	r3, [sp, #109]	; 0x6d
   347a0:	9b04      	ldr	r3, [sp, #16]
   347a2:	2b63      	cmp	r3, #99	; 0x63
   347a4:	dd30      	ble.n	34808 <_svfprintf_r+0x460>
   347a6:	1c59      	adds	r1, r3, #1
   347a8:	4650      	mov	r0, sl
   347aa:	f005 f889 	bl	398c0 <_malloc_r>
   347ae:	4681      	mov	r9, r0
   347b0:	2800      	cmp	r0, #0
   347b2:	f040 81f9 	bne.w	34ba8 <_svfprintf_r+0x800>
   347b6:	f8b8 300c 	ldrh.w	r3, [r8, #12]
   347ba:	f043 0340 	orr.w	r3, r3, #64	; 0x40
   347be:	f8a8 300c 	strh.w	r3, [r8, #12]
   347c2:	f8b8 300c 	ldrh.w	r3, [r8, #12]
   347c6:	f013 0f40 	tst.w	r3, #64	; 0x40
   347ca:	9b0c      	ldr	r3, [sp, #48]	; 0x30
   347cc:	bf18      	it	ne
   347ce:	f04f 33ff 	movne.w	r3, #4294967295	; 0xffffffff
   347d2:	e60f      	b.n	343f4 <_svfprintf_r+0x4c>
   347d4:	9b04      	ldr	r3, [sp, #16]
   347d6:	3301      	adds	r3, #1
   347d8:	f000 81e8 	beq.w	34bac <_svfprintf_r+0x804>
   347dc:	9b08      	ldr	r3, [sp, #32]
   347de:	2b47      	cmp	r3, #71	; 0x47
   347e0:	f040 81e7 	bne.w	34bb2 <_svfprintf_r+0x80a>
   347e4:	9b04      	ldr	r3, [sp, #16]
   347e6:	2b00      	cmp	r3, #0
   347e8:	f040 81e3 	bne.w	34bb2 <_svfprintf_r+0x80a>
   347ec:	9303      	str	r3, [sp, #12]
   347ee:	2301      	movs	r3, #1
   347f0:	9304      	str	r3, [sp, #16]
   347f2:	e00c      	b.n	3480e <_svfprintf_r+0x466>
   347f4:	7fefffff 	.word	0x7fefffff
   347f8:	000404cf 	.word	0x000404cf
   347fc:	000404d3 	.word	0x000404d3
   34800:	000404d7 	.word	0x000404d7
   34804:	000404db 	.word	0x000404db
   34808:	f10d 09d4 	add.w	r9, sp, #212	; 0xd4
   3480c:	9003      	str	r0, [sp, #12]
   3480e:	f44b 7380 	orr.w	r3, fp, #256	; 0x100
   34812:	9311      	str	r3, [sp, #68]	; 0x44
   34814:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
   34816:	2b00      	cmp	r3, #0
   34818:	f280 81cd 	bge.w	34bb6 <_svfprintf_r+0x80e>
   3481c:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3481e:	ee08 3a10 	vmov	s16, r3
   34822:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
   34824:	f103 4300 	add.w	r3, r3, #2147483648	; 0x80000000
   34828:	ee08 3a90 	vmov	s17, r3
   3482c:	232d      	movs	r3, #45	; 0x2d
   3482e:	9318      	str	r3, [sp, #96]	; 0x60
   34830:	9b08      	ldr	r3, [sp, #32]
   34832:	2b41      	cmp	r3, #65	; 0x41
   34834:	f040 81dd 	bne.w	34bf2 <_svfprintf_r+0x84a>
   34838:	a81c      	add	r0, sp, #112	; 0x70
   3483a:	eeb0 0a48 	vmov.f32	s0, s16
   3483e:	eef0 0a68 	vmov.f32	s1, s17
   34842:	f006 fb0f 	bl	3ae64 <frexp>
   34846:	2200      	movs	r2, #0
   34848:	f04f 537f 	mov.w	r3, #1069547520	; 0x3fc00000
   3484c:	ec51 0b10 	vmov	r0, r1, d0
   34850:	f00b f874 	bl	3f93c <__aeabi_dmul>
   34854:	2200      	movs	r2, #0
   34856:	2300      	movs	r3, #0
   34858:	4606      	mov	r6, r0
   3485a:	460f      	mov	r7, r1
   3485c:	f00b fad6 	bl	3fe0c <__aeabi_dcmpeq>
   34860:	b108      	cbz	r0, 34866 <_svfprintf_r+0x4be>
   34862:	2301      	movs	r3, #1
   34864:	931c      	str	r3, [sp, #112]	; 0x70
   34866:	4ba5      	ldr	r3, [pc, #660]	; (34afc <_svfprintf_r+0x754>)
   34868:	464d      	mov	r5, r9
   3486a:	4aa5      	ldr	r2, [pc, #660]	; (34b00 <_svfprintf_r+0x758>)
   3486c:	9906      	ldr	r1, [sp, #24]
   3486e:	2961      	cmp	r1, #97	; 0x61
   34870:	bf18      	it	ne
   34872:	461a      	movne	r2, r3
   34874:	9b04      	ldr	r3, [sp, #16]
   34876:	3b01      	subs	r3, #1
   34878:	9217      	str	r2, [sp, #92]	; 0x5c
   3487a:	9305      	str	r3, [sp, #20]
   3487c:	2200      	movs	r2, #0
   3487e:	4ba1      	ldr	r3, [pc, #644]	; (34b04 <_svfprintf_r+0x75c>)
   34880:	4630      	mov	r0, r6
   34882:	4639      	mov	r1, r7
   34884:	f00b f85a 	bl	3f93c <__aeabi_dmul>
   34888:	460f      	mov	r7, r1
   3488a:	4606      	mov	r6, r0
   3488c:	f00b fb06 	bl	3fe9c <__aeabi_d2iz>
   34890:	9019      	str	r0, [sp, #100]	; 0x64
   34892:	f00a ffe9 	bl	3f868 <__aeabi_i2d>
   34896:	4602      	mov	r2, r0
   34898:	460b      	mov	r3, r1
   3489a:	4630      	mov	r0, r6
   3489c:	4639      	mov	r1, r7
   3489e:	f00a fe95 	bl	3f5cc <__aeabi_dsub>
   348a2:	9a19      	ldr	r2, [sp, #100]	; 0x64
   348a4:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   348a6:	4606      	mov	r6, r0
   348a8:	460f      	mov	r7, r1
   348aa:	5c9b      	ldrb	r3, [r3, r2]
   348ac:	f805 3b01 	strb.w	r3, [r5], #1
   348b0:	9b05      	ldr	r3, [sp, #20]
   348b2:	1c5a      	adds	r2, r3, #1
   348b4:	9309      	str	r3, [sp, #36]	; 0x24
   348b6:	d007      	beq.n	348c8 <_svfprintf_r+0x520>
   348b8:	3b01      	subs	r3, #1
   348ba:	2200      	movs	r2, #0
   348bc:	9305      	str	r3, [sp, #20]
   348be:	2300      	movs	r3, #0
   348c0:	f00b faa4 	bl	3fe0c <__aeabi_dcmpeq>
   348c4:	2800      	cmp	r0, #0
   348c6:	d0d9      	beq.n	3487c <_svfprintf_r+0x4d4>
   348c8:	2200      	movs	r2, #0
   348ca:	4b8f      	ldr	r3, [pc, #572]	; (34b08 <_svfprintf_r+0x760>)
   348cc:	4630      	mov	r0, r6
   348ce:	4639      	mov	r1, r7
   348d0:	f00b fac4 	bl	3fe5c <__aeabi_dcmpgt>
   348d4:	b960      	cbnz	r0, 348f0 <_svfprintf_r+0x548>
   348d6:	2200      	movs	r2, #0
   348d8:	4b8b      	ldr	r3, [pc, #556]	; (34b08 <_svfprintf_r+0x760>)
   348da:	4630      	mov	r0, r6
   348dc:	4639      	mov	r1, r7
   348de:	f00b fa95 	bl	3fe0c <__aeabi_dcmpeq>
   348e2:	2800      	cmp	r0, #0
   348e4:	f000 8180 	beq.w	34be8 <_svfprintf_r+0x840>
   348e8:	9b19      	ldr	r3, [sp, #100]	; 0x64
   348ea:	07db      	lsls	r3, r3, #31
   348ec:	f140 817c 	bpl.w	34be8 <_svfprintf_r+0x840>
   348f0:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   348f2:	2030      	movs	r0, #48	; 0x30
   348f4:	9520      	str	r5, [sp, #128]	; 0x80
   348f6:	7bd9      	ldrb	r1, [r3, #15]
   348f8:	9a20      	ldr	r2, [sp, #128]	; 0x80
   348fa:	1e53      	subs	r3, r2, #1
   348fc:	9320      	str	r3, [sp, #128]	; 0x80
   348fe:	f812 3c01 	ldrb.w	r3, [r2, #-1]
   34902:	428b      	cmp	r3, r1
   34904:	f000 815f 	beq.w	34bc6 <_svfprintf_r+0x81e>
   34908:	2b39      	cmp	r3, #57	; 0x39
   3490a:	bf0b      	itete	eq
   3490c:	9b17      	ldreq	r3, [sp, #92]	; 0x5c
   3490e:	3301      	addne	r3, #1
   34910:	7a9b      	ldrbeq	r3, [r3, #10]
   34912:	b2db      	uxtbne	r3, r3
   34914:	f802 3c01 	strb.w	r3, [r2, #-1]
   34918:	eba5 0309 	sub.w	r3, r5, r9
   3491c:	9d1c      	ldr	r5, [sp, #112]	; 0x70
   3491e:	9305      	str	r3, [sp, #20]
   34920:	9b08      	ldr	r3, [sp, #32]
   34922:	2b47      	cmp	r3, #71	; 0x47
   34924:	f040 81ad 	bne.w	34c82 <_svfprintf_r+0x8da>
   34928:	1cef      	adds	r7, r5, #3
   3492a:	db03      	blt.n	34934 <_svfprintf_r+0x58c>
   3492c:	9b04      	ldr	r3, [sp, #16]
   3492e:	42ab      	cmp	r3, r5
   34930:	f280 81d2 	bge.w	34cd8 <_svfprintf_r+0x930>
   34934:	9b06      	ldr	r3, [sp, #24]
   34936:	3b02      	subs	r3, #2
   34938:	9306      	str	r3, [sp, #24]
   3493a:	9906      	ldr	r1, [sp, #24]
   3493c:	1e6b      	subs	r3, r5, #1
   3493e:	f89d 2018 	ldrb.w	r2, [sp, #24]
   34942:	f021 0120 	bic.w	r1, r1, #32
   34946:	931c      	str	r3, [sp, #112]	; 0x70
   34948:	2941      	cmp	r1, #65	; 0x41
   3494a:	bf05      	ittet	eq
   3494c:	320f      	addeq	r2, #15
   3494e:	2101      	moveq	r1, #1
   34950:	2100      	movne	r1, #0
   34952:	b2d2      	uxtbeq	r2, r2
   34954:	2b00      	cmp	r3, #0
   34956:	bfb8      	it	lt
   34958:	f1c5 0301 	rsblt	r3, r5, #1
   3495c:	f88d 2078 	strb.w	r2, [sp, #120]	; 0x78
   34960:	bfb4      	ite	lt
   34962:	222d      	movlt	r2, #45	; 0x2d
   34964:	222b      	movge	r2, #43	; 0x2b
   34966:	2b09      	cmp	r3, #9
   34968:	f88d 2079 	strb.w	r2, [sp, #121]	; 0x79
   3496c:	f340 81a2 	ble.w	34cb4 <_svfprintf_r+0x90c>
   34970:	f10d 0287 	add.w	r2, sp, #135	; 0x87
   34974:	260a      	movs	r6, #10
   34976:	4611      	mov	r1, r2
   34978:	3a01      	subs	r2, #1
   3497a:	fb93 f5f6 	sdiv	r5, r3, r6
   3497e:	fb06 3015 	mls	r0, r6, r5, r3
   34982:	3030      	adds	r0, #48	; 0x30
   34984:	f801 0c01 	strb.w	r0, [r1, #-1]
   34988:	4618      	mov	r0, r3
   3498a:	462b      	mov	r3, r5
   3498c:	2863      	cmp	r0, #99	; 0x63
   3498e:	dcf2      	bgt.n	34976 <_svfprintf_r+0x5ce>
   34990:	3330      	adds	r3, #48	; 0x30
   34992:	1e88      	subs	r0, r1, #2
   34994:	f10d 0587 	add.w	r5, sp, #135	; 0x87
   34998:	f802 3c01 	strb.w	r3, [r2, #-1]
   3499c:	f10d 027a 	add.w	r2, sp, #122	; 0x7a
   349a0:	4603      	mov	r3, r0
   349a2:	42ab      	cmp	r3, r5
   349a4:	f0c0 8181 	bcc.w	34caa <_svfprintf_r+0x902>
   349a8:	f10d 0289 	add.w	r2, sp, #137	; 0x89
   349ac:	f10d 037a 	add.w	r3, sp, #122	; 0x7a
   349b0:	1a52      	subs	r2, r2, r1
   349b2:	42a8      	cmp	r0, r5
   349b4:	bf88      	it	hi
   349b6:	2200      	movhi	r2, #0
   349b8:	441a      	add	r2, r3
   349ba:	ab1e      	add	r3, sp, #120	; 0x78
   349bc:	1ad3      	subs	r3, r2, r3
   349be:	9a05      	ldr	r2, [sp, #20]
   349c0:	9315      	str	r3, [sp, #84]	; 0x54
   349c2:	2a01      	cmp	r2, #1
   349c4:	4413      	add	r3, r2
   349c6:	9304      	str	r3, [sp, #16]
   349c8:	dc02      	bgt.n	349d0 <_svfprintf_r+0x628>
   349ca:	f01b 0f01 	tst.w	fp, #1
   349ce:	d003      	beq.n	349d8 <_svfprintf_r+0x630>
   349d0:	9b04      	ldr	r3, [sp, #16]
   349d2:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   349d4:	4413      	add	r3, r2
   349d6:	9304      	str	r3, [sp, #16]
   349d8:	f42b 6380 	bic.w	r3, fp, #1024	; 0x400
   349dc:	f443 7380 	orr.w	r3, r3, #256	; 0x100
   349e0:	9311      	str	r3, [sp, #68]	; 0x44
   349e2:	2300      	movs	r3, #0
   349e4:	461d      	mov	r5, r3
   349e6:	e9cd 3308 	strd	r3, r3, [sp, #32]
   349ea:	9b18      	ldr	r3, [sp, #96]	; 0x60
   349ec:	b113      	cbz	r3, 349f4 <_svfprintf_r+0x64c>
   349ee:	232d      	movs	r3, #45	; 0x2d
   349f0:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   349f4:	f8dd b044 	ldr.w	fp, [sp, #68]	; 0x44
   349f8:	2600      	movs	r6, #0
   349fa:	9b04      	ldr	r3, [sp, #16]
   349fc:	42b3      	cmp	r3, r6
   349fe:	bfb8      	it	lt
   34a00:	4633      	movlt	r3, r6
   34a02:	9311      	str	r3, [sp, #68]	; 0x44
   34a04:	f89d 306b 	ldrb.w	r3, [sp, #107]	; 0x6b
   34a08:	b113      	cbz	r3, 34a10 <_svfprintf_r+0x668>
   34a0a:	9b11      	ldr	r3, [sp, #68]	; 0x44
   34a0c:	3301      	adds	r3, #1
   34a0e:	9311      	str	r3, [sp, #68]	; 0x44
   34a10:	f01b 0302 	ands.w	r3, fp, #2
   34a14:	9317      	str	r3, [sp, #92]	; 0x5c
   34a16:	bf1e      	ittt	ne
   34a18:	9b11      	ldrne	r3, [sp, #68]	; 0x44
   34a1a:	3302      	addne	r3, #2
   34a1c:	9311      	strne	r3, [sp, #68]	; 0x44
   34a1e:	f01b 0384 	ands.w	r3, fp, #132	; 0x84
   34a22:	9318      	str	r3, [sp, #96]	; 0x60
   34a24:	d11f      	bne.n	34a66 <_svfprintf_r+0x6be>
   34a26:	e9dd 3210 	ldrd	r3, r2, [sp, #64]	; 0x40
   34a2a:	1a9f      	subs	r7, r3, r2
   34a2c:	2f00      	cmp	r7, #0
   34a2e:	dd1a      	ble.n	34a66 <_svfprintf_r+0x6be>
   34a30:	4836      	ldr	r0, [pc, #216]	; (34b0c <_svfprintf_r+0x764>)
   34a32:	2f10      	cmp	r7, #16
   34a34:	f104 0108 	add.w	r1, r4, #8
   34a38:	6020      	str	r0, [r4, #0]
   34a3a:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   34a3e:	f103 0301 	add.w	r3, r3, #1
   34a42:	f300 82eb 	bgt.w	3501c <_svfprintf_r+0xc74>
   34a46:	6067      	str	r7, [r4, #4]
   34a48:	2b07      	cmp	r3, #7
   34a4a:	4417      	add	r7, r2
   34a4c:	e9cd 3723 	strd	r3, r7, [sp, #140]	; 0x8c
   34a50:	f340 82f7 	ble.w	35042 <_svfprintf_r+0xc9a>
   34a54:	aa22      	add	r2, sp, #136	; 0x88
   34a56:	4641      	mov	r1, r8
   34a58:	4650      	mov	r0, sl
   34a5a:	f007 fbed 	bl	3c238 <__ssprint_r>
   34a5e:	2800      	cmp	r0, #0
   34a60:	f040 861f 	bne.w	356a2 <_svfprintf_r+0x12fa>
   34a64:	ac25      	add	r4, sp, #148	; 0x94
   34a66:	f89d 306b 	ldrb.w	r3, [sp, #107]	; 0x6b
   34a6a:	b173      	cbz	r3, 34a8a <_svfprintf_r+0x6e2>
   34a6c:	f10d 036b 	add.w	r3, sp, #107	; 0x6b
   34a70:	6023      	str	r3, [r4, #0]
   34a72:	2301      	movs	r3, #1
   34a74:	6063      	str	r3, [r4, #4]
   34a76:	9b24      	ldr	r3, [sp, #144]	; 0x90
   34a78:	3301      	adds	r3, #1
   34a7a:	9324      	str	r3, [sp, #144]	; 0x90
   34a7c:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   34a7e:	3301      	adds	r3, #1
   34a80:	2b07      	cmp	r3, #7
   34a82:	9323      	str	r3, [sp, #140]	; 0x8c
   34a84:	f300 82df 	bgt.w	35046 <_svfprintf_r+0xc9e>
   34a88:	3408      	adds	r4, #8
   34a8a:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   34a8c:	b16b      	cbz	r3, 34aaa <_svfprintf_r+0x702>
   34a8e:	ab1b      	add	r3, sp, #108	; 0x6c
   34a90:	6023      	str	r3, [r4, #0]
   34a92:	2302      	movs	r3, #2
   34a94:	6063      	str	r3, [r4, #4]
   34a96:	9b24      	ldr	r3, [sp, #144]	; 0x90
   34a98:	3302      	adds	r3, #2
   34a9a:	9324      	str	r3, [sp, #144]	; 0x90
   34a9c:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   34a9e:	3301      	adds	r3, #1
   34aa0:	2b07      	cmp	r3, #7
   34aa2:	9323      	str	r3, [sp, #140]	; 0x8c
   34aa4:	f300 82d9 	bgt.w	3505a <_svfprintf_r+0xcb2>
   34aa8:	3408      	adds	r4, #8
   34aaa:	9b18      	ldr	r3, [sp, #96]	; 0x60
   34aac:	2b80      	cmp	r3, #128	; 0x80
   34aae:	d11f      	bne.n	34af0 <_svfprintf_r+0x748>
   34ab0:	e9dd 3210 	ldrd	r3, r2, [sp, #64]	; 0x40
   34ab4:	1a9f      	subs	r7, r3, r2
   34ab6:	2f00      	cmp	r7, #0
   34ab8:	dd1a      	ble.n	34af0 <_svfprintf_r+0x748>
   34aba:	4815      	ldr	r0, [pc, #84]	; (34b10 <_svfprintf_r+0x768>)
   34abc:	2f10      	cmp	r7, #16
   34abe:	f104 0108 	add.w	r1, r4, #8
   34ac2:	6020      	str	r0, [r4, #0]
   34ac4:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   34ac8:	f103 0301 	add.w	r3, r3, #1
   34acc:	f300 82cf 	bgt.w	3506e <_svfprintf_r+0xcc6>
   34ad0:	6067      	str	r7, [r4, #4]
   34ad2:	2b07      	cmp	r3, #7
   34ad4:	4417      	add	r7, r2
   34ad6:	e9cd 3723 	strd	r3, r7, [sp, #140]	; 0x8c
   34ada:	f340 82db 	ble.w	35094 <_svfprintf_r+0xcec>
   34ade:	aa22      	add	r2, sp, #136	; 0x88
   34ae0:	4641      	mov	r1, r8
   34ae2:	4650      	mov	r0, sl
   34ae4:	f007 fba8 	bl	3c238 <__ssprint_r>
   34ae8:	2800      	cmp	r0, #0
   34aea:	f040 85da 	bne.w	356a2 <_svfprintf_r+0x12fa>
   34aee:	ac25      	add	r4, sp, #148	; 0x94
   34af0:	9b04      	ldr	r3, [sp, #16]
   34af2:	1af6      	subs	r6, r6, r3
   34af4:	2e00      	cmp	r6, #0
   34af6:	dd27      	ble.n	34b48 <_svfprintf_r+0x7a0>
   34af8:	4f05      	ldr	r7, [pc, #20]	; (34b10 <_svfprintf_r+0x768>)
   34afa:	e00b      	b.n	34b14 <_svfprintf_r+0x76c>
   34afc:	000404f0 	.word	0x000404f0
   34b00:	000404df 	.word	0x000404df
   34b04:	40300000 	.word	0x40300000
   34b08:	3fe00000 	.word	0x3fe00000
   34b0c:	00040503 	.word	0x00040503
   34b10:	00040513 	.word	0x00040513
   34b14:	2e10      	cmp	r6, #16
   34b16:	f104 0108 	add.w	r1, r4, #8
   34b1a:	6027      	str	r7, [r4, #0]
   34b1c:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   34b20:	f103 0301 	add.w	r3, r3, #1
   34b24:	f300 82b8 	bgt.w	35098 <_svfprintf_r+0xcf0>
   34b28:	6066      	str	r6, [r4, #4]
   34b2a:	2b07      	cmp	r3, #7
   34b2c:	4416      	add	r6, r2
   34b2e:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   34b32:	f340 82c4 	ble.w	350be <_svfprintf_r+0xd16>
   34b36:	aa22      	add	r2, sp, #136	; 0x88
   34b38:	4641      	mov	r1, r8
   34b3a:	4650      	mov	r0, sl
   34b3c:	f007 fb7c 	bl	3c238 <__ssprint_r>
   34b40:	2800      	cmp	r0, #0
   34b42:	f040 85ae 	bne.w	356a2 <_svfprintf_r+0x12fa>
   34b46:	ac25      	add	r4, sp, #148	; 0x94
   34b48:	f41b 7f80 	tst.w	fp, #256	; 0x100
   34b4c:	9e24      	ldr	r6, [sp, #144]	; 0x90
   34b4e:	f040 82bd 	bne.w	350cc <_svfprintf_r+0xd24>
   34b52:	9b04      	ldr	r3, [sp, #16]
   34b54:	f8c4 9000 	str.w	r9, [r4]
   34b58:	441e      	add	r6, r3
   34b5a:	6063      	str	r3, [r4, #4]
   34b5c:	9624      	str	r6, [sp, #144]	; 0x90
   34b5e:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   34b60:	3301      	adds	r3, #1
   34b62:	2b07      	cmp	r3, #7
   34b64:	9323      	str	r3, [sp, #140]	; 0x8c
   34b66:	f300 82f6 	bgt.w	35156 <_svfprintf_r+0xdae>
   34b6a:	3408      	adds	r4, #8
   34b6c:	f01b 0f04 	tst.w	fp, #4
   34b70:	f040 8579 	bne.w	35666 <_svfprintf_r+0x12be>
   34b74:	9b0c      	ldr	r3, [sp, #48]	; 0x30
   34b76:	e9dd 2110 	ldrd	r2, r1, [sp, #64]	; 0x40
   34b7a:	428a      	cmp	r2, r1
   34b7c:	bfac      	ite	ge
   34b7e:	189b      	addge	r3, r3, r2
   34b80:	185b      	addlt	r3, r3, r1
   34b82:	930c      	str	r3, [sp, #48]	; 0x30
   34b84:	9b24      	ldr	r3, [sp, #144]	; 0x90
   34b86:	b13b      	cbz	r3, 34b98 <_svfprintf_r+0x7f0>
   34b88:	aa22      	add	r2, sp, #136	; 0x88
   34b8a:	4641      	mov	r1, r8
   34b8c:	4650      	mov	r0, sl
   34b8e:	f007 fb53 	bl	3c238 <__ssprint_r>
   34b92:	2800      	cmp	r0, #0
   34b94:	f040 8585 	bne.w	356a2 <_svfprintf_r+0x12fa>
   34b98:	2300      	movs	r3, #0
   34b9a:	9323      	str	r3, [sp, #140]	; 0x8c
   34b9c:	9b03      	ldr	r3, [sp, #12]
   34b9e:	2b00      	cmp	r3, #0
   34ba0:	f040 859b 	bne.w	356da <_svfprintf_r+0x1332>
   34ba4:	ac25      	add	r4, sp, #148	; 0x94
   34ba6:	e0ec      	b.n	34d82 <_svfprintf_r+0x9da>
   34ba8:	9003      	str	r0, [sp, #12]
   34baa:	e630      	b.n	3480e <_svfprintf_r+0x466>
   34bac:	2306      	movs	r3, #6
   34bae:	9003      	str	r0, [sp, #12]
   34bb0:	e61e      	b.n	347f0 <_svfprintf_r+0x448>
   34bb2:	9003      	str	r0, [sp, #12]
   34bb4:	e62b      	b.n	3480e <_svfprintf_r+0x466>
   34bb6:	ed9d 7b0e 	vldr	d7, [sp, #56]	; 0x38
   34bba:	2300      	movs	r3, #0
   34bbc:	eeb0 8a47 	vmov.f32	s16, s14
   34bc0:	eef0 8a67 	vmov.f32	s17, s15
   34bc4:	e633      	b.n	3482e <_svfprintf_r+0x486>
   34bc6:	f802 0c01 	strb.w	r0, [r2, #-1]
   34bca:	e695      	b.n	348f8 <_svfprintf_r+0x550>
   34bcc:	f803 0b01 	strb.w	r0, [r3], #1
   34bd0:	1aca      	subs	r2, r1, r3
   34bd2:	2a00      	cmp	r2, #0
   34bd4:	dafa      	bge.n	34bcc <_svfprintf_r+0x824>
   34bd6:	9a09      	ldr	r2, [sp, #36]	; 0x24
   34bd8:	9b09      	ldr	r3, [sp, #36]	; 0x24
   34bda:	3201      	adds	r2, #1
   34bdc:	f103 0301 	add.w	r3, r3, #1
   34be0:	bfb8      	it	lt
   34be2:	2300      	movlt	r3, #0
   34be4:	441d      	add	r5, r3
   34be6:	e697      	b.n	34918 <_svfprintf_r+0x570>
   34be8:	9a09      	ldr	r2, [sp, #36]	; 0x24
   34bea:	462b      	mov	r3, r5
   34bec:	2030      	movs	r0, #48	; 0x30
   34bee:	18a9      	adds	r1, r5, r2
   34bf0:	e7ee      	b.n	34bd0 <_svfprintf_r+0x828>
   34bf2:	9b08      	ldr	r3, [sp, #32]
   34bf4:	2b46      	cmp	r3, #70	; 0x46
   34bf6:	d005      	beq.n	34c04 <_svfprintf_r+0x85c>
   34bf8:	2b45      	cmp	r3, #69	; 0x45
   34bfa:	d11b      	bne.n	34c34 <_svfprintf_r+0x88c>
   34bfc:	9b04      	ldr	r3, [sp, #16]
   34bfe:	1c5d      	adds	r5, r3, #1
   34c00:	2102      	movs	r1, #2
   34c02:	e001      	b.n	34c08 <_svfprintf_r+0x860>
   34c04:	9d04      	ldr	r5, [sp, #16]
   34c06:	2103      	movs	r1, #3
   34c08:	ab20      	add	r3, sp, #128	; 0x80
   34c0a:	eeb0 0a48 	vmov.f32	s0, s16
   34c0e:	eef0 0a68 	vmov.f32	s1, s17
   34c12:	462a      	mov	r2, r5
   34c14:	4650      	mov	r0, sl
   34c16:	9301      	str	r3, [sp, #4]
   34c18:	ab1d      	add	r3, sp, #116	; 0x74
   34c1a:	9300      	str	r3, [sp, #0]
   34c1c:	ab1c      	add	r3, sp, #112	; 0x70
   34c1e:	f003 fb48 	bl	382b2 <_dtoa_r>
   34c22:	9b08      	ldr	r3, [sp, #32]
   34c24:	4681      	mov	r9, r0
   34c26:	2b47      	cmp	r3, #71	; 0x47
   34c28:	d106      	bne.n	34c38 <_svfprintf_r+0x890>
   34c2a:	f01b 0f01 	tst.w	fp, #1
   34c2e:	d103      	bne.n	34c38 <_svfprintf_r+0x890>
   34c30:	9d20      	ldr	r5, [sp, #128]	; 0x80
   34c32:	e671      	b.n	34918 <_svfprintf_r+0x570>
   34c34:	9d04      	ldr	r5, [sp, #16]
   34c36:	e7e3      	b.n	34c00 <_svfprintf_r+0x858>
   34c38:	9b08      	ldr	r3, [sp, #32]
   34c3a:	eb09 0605 	add.w	r6, r9, r5
   34c3e:	2b46      	cmp	r3, #70	; 0x46
   34c40:	d10f      	bne.n	34c62 <_svfprintf_r+0x8ba>
   34c42:	f899 3000 	ldrb.w	r3, [r9]
   34c46:	2b30      	cmp	r3, #48	; 0x30
   34c48:	d109      	bne.n	34c5e <_svfprintf_r+0x8b6>
   34c4a:	2200      	movs	r2, #0
   34c4c:	2300      	movs	r3, #0
   34c4e:	ec51 0b18 	vmov	r0, r1, d8
   34c52:	f00b f8db 	bl	3fe0c <__aeabi_dcmpeq>
   34c56:	b910      	cbnz	r0, 34c5e <_svfprintf_r+0x8b6>
   34c58:	f1c5 0501 	rsb	r5, r5, #1
   34c5c:	951c      	str	r5, [sp, #112]	; 0x70
   34c5e:	9b1c      	ldr	r3, [sp, #112]	; 0x70
   34c60:	441e      	add	r6, r3
   34c62:	2200      	movs	r2, #0
   34c64:	2300      	movs	r3, #0
   34c66:	ec51 0b18 	vmov	r0, r1, d8
   34c6a:	f00b f8cf 	bl	3fe0c <__aeabi_dcmpeq>
   34c6e:	b100      	cbz	r0, 34c72 <_svfprintf_r+0x8ca>
   34c70:	9620      	str	r6, [sp, #128]	; 0x80
   34c72:	2230      	movs	r2, #48	; 0x30
   34c74:	9b20      	ldr	r3, [sp, #128]	; 0x80
   34c76:	429e      	cmp	r6, r3
   34c78:	d9da      	bls.n	34c30 <_svfprintf_r+0x888>
   34c7a:	1c59      	adds	r1, r3, #1
   34c7c:	9120      	str	r1, [sp, #128]	; 0x80
   34c7e:	701a      	strb	r2, [r3, #0]
   34c80:	e7f8      	b.n	34c74 <_svfprintf_r+0x8cc>
   34c82:	9b08      	ldr	r3, [sp, #32]
   34c84:	2b46      	cmp	r3, #70	; 0x46
   34c86:	f47f ae58 	bne.w	3493a <_svfprintf_r+0x592>
   34c8a:	f00b 0301 	and.w	r3, fp, #1
   34c8e:	9a04      	ldr	r2, [sp, #16]
   34c90:	2d00      	cmp	r5, #0
   34c92:	ea43 0302 	orr.w	r3, r3, r2
   34c96:	dd1a      	ble.n	34cce <_svfprintf_r+0x926>
   34c98:	2b00      	cmp	r3, #0
   34c9a:	d034      	beq.n	34d06 <_svfprintf_r+0x95e>
   34c9c:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   34c9e:	18eb      	adds	r3, r5, r3
   34ca0:	441a      	add	r2, r3
   34ca2:	9204      	str	r2, [sp, #16]
   34ca4:	2366      	movs	r3, #102	; 0x66
   34ca6:	9306      	str	r3, [sp, #24]
   34ca8:	e033      	b.n	34d12 <_svfprintf_r+0x96a>
   34caa:	f813 6b01 	ldrb.w	r6, [r3], #1
   34cae:	f802 6b01 	strb.w	r6, [r2], #1
   34cb2:	e676      	b.n	349a2 <_svfprintf_r+0x5fa>
   34cb4:	b941      	cbnz	r1, 34cc8 <_svfprintf_r+0x920>
   34cb6:	2230      	movs	r2, #48	; 0x30
   34cb8:	f88d 207a 	strb.w	r2, [sp, #122]	; 0x7a
   34cbc:	f10d 027b 	add.w	r2, sp, #123	; 0x7b
   34cc0:	3330      	adds	r3, #48	; 0x30
   34cc2:	f802 3b01 	strb.w	r3, [r2], #1
   34cc6:	e678      	b.n	349ba <_svfprintf_r+0x612>
   34cc8:	f10d 027a 	add.w	r2, sp, #122	; 0x7a
   34ccc:	e7f8      	b.n	34cc0 <_svfprintf_r+0x918>
   34cce:	b1e3      	cbz	r3, 34d0a <_svfprintf_r+0x962>
   34cd0:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   34cd2:	9a04      	ldr	r2, [sp, #16]
   34cd4:	3301      	adds	r3, #1
   34cd6:	e7e3      	b.n	34ca0 <_svfprintf_r+0x8f8>
   34cd8:	9b05      	ldr	r3, [sp, #20]
   34cda:	429d      	cmp	r5, r3
   34cdc:	db07      	blt.n	34cee <_svfprintf_r+0x946>
   34cde:	f01b 0f01 	tst.w	fp, #1
   34ce2:	d02d      	beq.n	34d40 <_svfprintf_r+0x998>
   34ce4:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   34ce6:	18eb      	adds	r3, r5, r3
   34ce8:	9304      	str	r3, [sp, #16]
   34cea:	2367      	movs	r3, #103	; 0x67
   34cec:	e7db      	b.n	34ca6 <_svfprintf_r+0x8fe>
   34cee:	9b05      	ldr	r3, [sp, #20]
   34cf0:	2d00      	cmp	r5, #0
   34cf2:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   34cf4:	4413      	add	r3, r2
   34cf6:	9304      	str	r3, [sp, #16]
   34cf8:	dcf7      	bgt.n	34cea <_svfprintf_r+0x942>
   34cfa:	9a04      	ldr	r2, [sp, #16]
   34cfc:	f1c5 0301 	rsb	r3, r5, #1
   34d00:	441a      	add	r2, r3
   34d02:	9204      	str	r2, [sp, #16]
   34d04:	e7f1      	b.n	34cea <_svfprintf_r+0x942>
   34d06:	9504      	str	r5, [sp, #16]
   34d08:	e7cc      	b.n	34ca4 <_svfprintf_r+0x8fc>
   34d0a:	2366      	movs	r3, #102	; 0x66
   34d0c:	9306      	str	r3, [sp, #24]
   34d0e:	2301      	movs	r3, #1
   34d10:	9304      	str	r3, [sp, #16]
   34d12:	f41b 6380 	ands.w	r3, fp, #1024	; 0x400
   34d16:	9309      	str	r3, [sp, #36]	; 0x24
   34d18:	d025      	beq.n	34d66 <_svfprintf_r+0x9be>
   34d1a:	2300      	movs	r3, #0
   34d1c:	2d00      	cmp	r5, #0
   34d1e:	e9cd 3308 	strd	r3, r3, [sp, #32]
   34d22:	f77f ae62 	ble.w	349ea <_svfprintf_r+0x642>
   34d26:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   34d28:	781b      	ldrb	r3, [r3, #0]
   34d2a:	2bff      	cmp	r3, #255	; 0xff
   34d2c:	d10a      	bne.n	34d44 <_svfprintf_r+0x99c>
   34d2e:	e9dd 2308 	ldrd	r2, r3, [sp, #32]
   34d32:	9912      	ldr	r1, [sp, #72]	; 0x48
   34d34:	4413      	add	r3, r2
   34d36:	9a04      	ldr	r2, [sp, #16]
   34d38:	fb01 2303 	mla	r3, r1, r3, r2
   34d3c:	9304      	str	r3, [sp, #16]
   34d3e:	e654      	b.n	349ea <_svfprintf_r+0x642>
   34d40:	9504      	str	r5, [sp, #16]
   34d42:	e7d2      	b.n	34cea <_svfprintf_r+0x942>
   34d44:	42ab      	cmp	r3, r5
   34d46:	daf2      	bge.n	34d2e <_svfprintf_r+0x986>
   34d48:	1aed      	subs	r5, r5, r3
   34d4a:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   34d4c:	785b      	ldrb	r3, [r3, #1]
   34d4e:	b133      	cbz	r3, 34d5e <_svfprintf_r+0x9b6>
   34d50:	9b09      	ldr	r3, [sp, #36]	; 0x24
   34d52:	3301      	adds	r3, #1
   34d54:	9309      	str	r3, [sp, #36]	; 0x24
   34d56:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   34d58:	3301      	adds	r3, #1
   34d5a:	930a      	str	r3, [sp, #40]	; 0x28
   34d5c:	e7e3      	b.n	34d26 <_svfprintf_r+0x97e>
   34d5e:	9b08      	ldr	r3, [sp, #32]
   34d60:	3301      	adds	r3, #1
   34d62:	9308      	str	r3, [sp, #32]
   34d64:	e7df      	b.n	34d26 <_svfprintf_r+0x97e>
   34d66:	9b09      	ldr	r3, [sp, #36]	; 0x24
   34d68:	9308      	str	r3, [sp, #32]
   34d6a:	e63e      	b.n	349ea <_svfprintf_r+0x642>
   34d6c:	1d33      	adds	r3, r6, #4
   34d6e:	f01b 0f20 	tst.w	fp, #32
   34d72:	9307      	str	r3, [sp, #28]
   34d74:	d00a      	beq.n	34d8c <_svfprintf_r+0x9e4>
   34d76:	9a0c      	ldr	r2, [sp, #48]	; 0x30
   34d78:	6833      	ldr	r3, [r6, #0]
   34d7a:	17d2      	asrs	r2, r2, #31
   34d7c:	990c      	ldr	r1, [sp, #48]	; 0x30
   34d7e:	e9c3 1200 	strd	r1, r2, [r3]
   34d82:	9e07      	ldr	r6, [sp, #28]
   34d84:	f8dd 902c 	ldr.w	r9, [sp, #44]	; 0x2c
   34d88:	f7ff bb4e 	b.w	34428 <_svfprintf_r+0x80>
   34d8c:	f01b 0f10 	tst.w	fp, #16
   34d90:	d003      	beq.n	34d9a <_svfprintf_r+0x9f2>
   34d92:	6833      	ldr	r3, [r6, #0]
   34d94:	9a0c      	ldr	r2, [sp, #48]	; 0x30
   34d96:	601a      	str	r2, [r3, #0]
   34d98:	e7f3      	b.n	34d82 <_svfprintf_r+0x9da>
   34d9a:	f01b 0f40 	tst.w	fp, #64	; 0x40
   34d9e:	d003      	beq.n	34da8 <_svfprintf_r+0xa00>
   34da0:	6833      	ldr	r3, [r6, #0]
   34da2:	9a0c      	ldr	r2, [sp, #48]	; 0x30
   34da4:	801a      	strh	r2, [r3, #0]
   34da6:	e7ec      	b.n	34d82 <_svfprintf_r+0x9da>
   34da8:	f41b 7f00 	tst.w	fp, #512	; 0x200
   34dac:	d0f1      	beq.n	34d92 <_svfprintf_r+0x9ea>
   34dae:	6833      	ldr	r3, [r6, #0]
   34db0:	9a0c      	ldr	r2, [sp, #48]	; 0x30
   34db2:	701a      	strb	r2, [r3, #0]
   34db4:	e7e5      	b.n	34d82 <_svfprintf_r+0x9da>
   34db6:	f04b 0b10 	orr.w	fp, fp, #16
   34dba:	f01b 0320 	ands.w	r3, fp, #32
   34dbe:	d01f      	beq.n	34e00 <_svfprintf_r+0xa58>
   34dc0:	1df3      	adds	r3, r6, #7
   34dc2:	f023 0307 	bic.w	r3, r3, #7
   34dc6:	461a      	mov	r2, r3
   34dc8:	685d      	ldr	r5, [r3, #4]
   34dca:	f852 6b08 	ldr.w	r6, [r2], #8
   34dce:	9207      	str	r2, [sp, #28]
   34dd0:	f42b 6b80 	bic.w	fp, fp, #1024	; 0x400
   34dd4:	2300      	movs	r3, #0
   34dd6:	2200      	movs	r2, #0
   34dd8:	f88d 206b 	strb.w	r2, [sp, #107]	; 0x6b
   34ddc:	9a04      	ldr	r2, [sp, #16]
   34dde:	3201      	adds	r2, #1
   34de0:	f000 848c 	beq.w	356fc <_svfprintf_r+0x1354>
   34de4:	ea56 0205 	orrs.w	r2, r6, r5
   34de8:	f02b 0780 	bic.w	r7, fp, #128	; 0x80
   34dec:	f040 848b 	bne.w	35706 <_svfprintf_r+0x135e>
   34df0:	9a04      	ldr	r2, [sp, #16]
   34df2:	2a00      	cmp	r2, #0
   34df4:	f000 80f8 	beq.w	34fe8 <_svfprintf_r+0xc40>
   34df8:	2b01      	cmp	r3, #1
   34dfa:	f040 8487 	bne.w	3570c <_svfprintf_r+0x1364>
   34dfe:	e09e      	b.n	34f3e <_svfprintf_r+0xb96>
   34e00:	4632      	mov	r2, r6
   34e02:	f01b 0510 	ands.w	r5, fp, #16
   34e06:	f852 6b04 	ldr.w	r6, [r2], #4
   34e0a:	9207      	str	r2, [sp, #28]
   34e0c:	d001      	beq.n	34e12 <_svfprintf_r+0xa6a>
   34e0e:	461d      	mov	r5, r3
   34e10:	e7de      	b.n	34dd0 <_svfprintf_r+0xa28>
   34e12:	f01b 0340 	ands.w	r3, fp, #64	; 0x40
   34e16:	d001      	beq.n	34e1c <_svfprintf_r+0xa74>
   34e18:	b2b6      	uxth	r6, r6
   34e1a:	e7d9      	b.n	34dd0 <_svfprintf_r+0xa28>
   34e1c:	f41b 7500 	ands.w	r5, fp, #512	; 0x200
   34e20:	d0d6      	beq.n	34dd0 <_svfprintf_r+0xa28>
   34e22:	b2f6      	uxtb	r6, r6
   34e24:	e7f3      	b.n	34e0e <_svfprintf_r+0xa66>
   34e26:	4633      	mov	r3, r6
   34e28:	2278      	movs	r2, #120	; 0x78
   34e2a:	2500      	movs	r5, #0
   34e2c:	f04b 0b02 	orr.w	fp, fp, #2
   34e30:	f853 6b04 	ldr.w	r6, [r3], #4
   34e34:	9206      	str	r2, [sp, #24]
   34e36:	9307      	str	r3, [sp, #28]
   34e38:	f647 0330 	movw	r3, #30768	; 0x7830
   34e3c:	f8ad 306c 	strh.w	r3, [sp, #108]	; 0x6c
   34e40:	4ba0      	ldr	r3, [pc, #640]	; (350c4 <_svfprintf_r+0xd1c>)
   34e42:	9316      	str	r3, [sp, #88]	; 0x58
   34e44:	2302      	movs	r3, #2
   34e46:	e7c6      	b.n	34dd6 <_svfprintf_r+0xa2e>
   34e48:	4633      	mov	r3, r6
   34e4a:	2500      	movs	r5, #0
   34e4c:	f853 9b04 	ldr.w	r9, [r3], #4
   34e50:	f88d 506b 	strb.w	r5, [sp, #107]	; 0x6b
   34e54:	9307      	str	r3, [sp, #28]
   34e56:	9b04      	ldr	r3, [sp, #16]
   34e58:	1c58      	adds	r0, r3, #1
   34e5a:	d010      	beq.n	34e7e <_svfprintf_r+0xad6>
   34e5c:	461a      	mov	r2, r3
   34e5e:	4629      	mov	r1, r5
   34e60:	4648      	mov	r0, r9
   34e62:	f004 ffa1 	bl	39da8 <memchr>
   34e66:	9003      	str	r0, [sp, #12]
   34e68:	2800      	cmp	r0, #0
   34e6a:	f000 80d4 	beq.w	35016 <_svfprintf_r+0xc6e>
   34e6e:	eba0 0309 	sub.w	r3, r0, r9
   34e72:	462e      	mov	r6, r5
   34e74:	e9cd 5303 	strd	r5, r3, [sp, #12]
   34e78:	e9cd 5508 	strd	r5, r5, [sp, #32]
   34e7c:	e5bd      	b.n	349fa <_svfprintf_r+0x652>
   34e7e:	4648      	mov	r0, r9
   34e80:	f7fe ff56 	bl	33d30 <strlen>
   34e84:	e9cd 5003 	strd	r5, r0, [sp, #12]
   34e88:	e464      	b.n	34754 <_svfprintf_r+0x3ac>
   34e8a:	f04b 0b10 	orr.w	fp, fp, #16
   34e8e:	f01b 0320 	ands.w	r3, fp, #32
   34e92:	d009      	beq.n	34ea8 <_svfprintf_r+0xb00>
   34e94:	1df3      	adds	r3, r6, #7
   34e96:	f023 0307 	bic.w	r3, r3, #7
   34e9a:	461a      	mov	r2, r3
   34e9c:	685d      	ldr	r5, [r3, #4]
   34e9e:	f852 6b08 	ldr.w	r6, [r2], #8
   34ea2:	9207      	str	r2, [sp, #28]
   34ea4:	2301      	movs	r3, #1
   34ea6:	e796      	b.n	34dd6 <_svfprintf_r+0xa2e>
   34ea8:	4632      	mov	r2, r6
   34eaa:	f01b 0510 	ands.w	r5, fp, #16
   34eae:	f852 6b04 	ldr.w	r6, [r2], #4
   34eb2:	9207      	str	r2, [sp, #28]
   34eb4:	d001      	beq.n	34eba <_svfprintf_r+0xb12>
   34eb6:	461d      	mov	r5, r3
   34eb8:	e7f4      	b.n	34ea4 <_svfprintf_r+0xafc>
   34eba:	f01b 0340 	ands.w	r3, fp, #64	; 0x40
   34ebe:	d001      	beq.n	34ec4 <_svfprintf_r+0xb1c>
   34ec0:	b2b6      	uxth	r6, r6
   34ec2:	e7ef      	b.n	34ea4 <_svfprintf_r+0xafc>
   34ec4:	f41b 7500 	ands.w	r5, fp, #512	; 0x200
   34ec8:	d0ec      	beq.n	34ea4 <_svfprintf_r+0xafc>
   34eca:	b2f6      	uxtb	r6, r6
   34ecc:	e7f3      	b.n	34eb6 <_svfprintf_r+0xb0e>
   34ece:	4b7e      	ldr	r3, [pc, #504]	; (350c8 <_svfprintf_r+0xd20>)
   34ed0:	9316      	str	r3, [sp, #88]	; 0x58
   34ed2:	f01b 0320 	ands.w	r3, fp, #32
   34ed6:	d01b      	beq.n	34f10 <_svfprintf_r+0xb68>
   34ed8:	1df3      	adds	r3, r6, #7
   34eda:	f023 0307 	bic.w	r3, r3, #7
   34ede:	461a      	mov	r2, r3
   34ee0:	685d      	ldr	r5, [r3, #4]
   34ee2:	f852 6b08 	ldr.w	r6, [r2], #8
   34ee6:	9207      	str	r2, [sp, #28]
   34ee8:	f01b 0f01 	tst.w	fp, #1
   34eec:	d00a      	beq.n	34f04 <_svfprintf_r+0xb5c>
   34eee:	ea56 0305 	orrs.w	r3, r6, r5
   34ef2:	d007      	beq.n	34f04 <_svfprintf_r+0xb5c>
   34ef4:	2330      	movs	r3, #48	; 0x30
   34ef6:	f04b 0b02 	orr.w	fp, fp, #2
   34efa:	f88d 306c 	strb.w	r3, [sp, #108]	; 0x6c
   34efe:	9b06      	ldr	r3, [sp, #24]
   34f00:	f88d 306d 	strb.w	r3, [sp, #109]	; 0x6d
   34f04:	f42b 6b80 	bic.w	fp, fp, #1024	; 0x400
   34f08:	2302      	movs	r3, #2
   34f0a:	e764      	b.n	34dd6 <_svfprintf_r+0xa2e>
   34f0c:	4b6d      	ldr	r3, [pc, #436]	; (350c4 <_svfprintf_r+0xd1c>)
   34f0e:	e7df      	b.n	34ed0 <_svfprintf_r+0xb28>
   34f10:	4632      	mov	r2, r6
   34f12:	f01b 0510 	ands.w	r5, fp, #16
   34f16:	f852 6b04 	ldr.w	r6, [r2], #4
   34f1a:	9207      	str	r2, [sp, #28]
   34f1c:	d001      	beq.n	34f22 <_svfprintf_r+0xb7a>
   34f1e:	461d      	mov	r5, r3
   34f20:	e7e2      	b.n	34ee8 <_svfprintf_r+0xb40>
   34f22:	f01b 0340 	ands.w	r3, fp, #64	; 0x40
   34f26:	d001      	beq.n	34f2c <_svfprintf_r+0xb84>
   34f28:	b2b6      	uxth	r6, r6
   34f2a:	e7dd      	b.n	34ee8 <_svfprintf_r+0xb40>
   34f2c:	f41b 7500 	ands.w	r5, fp, #512	; 0x200
   34f30:	d0da      	beq.n	34ee8 <_svfprintf_r+0xb40>
   34f32:	b2f6      	uxtb	r6, r6
   34f34:	e7f3      	b.n	34f1e <_svfprintf_r+0xb76>
   34f36:	2e0a      	cmp	r6, #10
   34f38:	f175 0300 	sbcs.w	r3, r5, #0
   34f3c:	d205      	bcs.n	34f4a <_svfprintf_r+0xba2>
   34f3e:	3630      	adds	r6, #48	; 0x30
   34f40:	f88d 6137 	strb.w	r6, [sp, #311]	; 0x137
   34f44:	f20d 1937 	addw	r9, sp, #311	; 0x137
   34f48:	e3fb      	b.n	35742 <_svfprintf_r+0x139a>
   34f4a:	2300      	movs	r3, #0
   34f4c:	f50d 7b9c 	add.w	fp, sp, #312	; 0x138
   34f50:	9305      	str	r3, [sp, #20]
   34f52:	f407 6380 	and.w	r3, r7, #1024	; 0x400
   34f56:	9303      	str	r3, [sp, #12]
   34f58:	220a      	movs	r2, #10
   34f5a:	2300      	movs	r3, #0
   34f5c:	4630      	mov	r0, r6
   34f5e:	4629      	mov	r1, r5
   34f60:	f00b f834 	bl	3ffcc <__aeabi_uldivmod>
   34f64:	9b05      	ldr	r3, [sp, #20]
   34f66:	3230      	adds	r2, #48	; 0x30
   34f68:	f10b 39ff 	add.w	r9, fp, #4294967295	; 0xffffffff
   34f6c:	3301      	adds	r3, #1
   34f6e:	f80b 2c01 	strb.w	r2, [fp, #-1]
   34f72:	9305      	str	r3, [sp, #20]
   34f74:	9b03      	ldr	r3, [sp, #12]
   34f76:	b1d3      	cbz	r3, 34fae <_svfprintf_r+0xc06>
   34f78:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   34f7a:	9a05      	ldr	r2, [sp, #20]
   34f7c:	781b      	ldrb	r3, [r3, #0]
   34f7e:	429a      	cmp	r2, r3
   34f80:	d115      	bne.n	34fae <_svfprintf_r+0xc06>
   34f82:	2aff      	cmp	r2, #255	; 0xff
   34f84:	d013      	beq.n	34fae <_svfprintf_r+0xc06>
   34f86:	2e0a      	cmp	r6, #10
   34f88:	f175 0300 	sbcs.w	r3, r5, #0
   34f8c:	d30f      	bcc.n	34fae <_svfprintf_r+0xc06>
   34f8e:	9b12      	ldr	r3, [sp, #72]	; 0x48
   34f90:	9914      	ldr	r1, [sp, #80]	; 0x50
   34f92:	eba9 0903 	sub.w	r9, r9, r3
   34f96:	461a      	mov	r2, r3
   34f98:	4648      	mov	r0, r9
   34f9a:	f7fe ff46 	bl	33e2a <strncpy>
   34f9e:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   34fa0:	785b      	ldrb	r3, [r3, #1]
   34fa2:	b11b      	cbz	r3, 34fac <_svfprintf_r+0xc04>
   34fa4:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   34fa6:	3301      	adds	r3, #1
   34fa8:	930a      	str	r3, [sp, #40]	; 0x28
   34faa:	2300      	movs	r3, #0
   34fac:	9305      	str	r3, [sp, #20]
   34fae:	2300      	movs	r3, #0
   34fb0:	220a      	movs	r2, #10
   34fb2:	4630      	mov	r0, r6
   34fb4:	4629      	mov	r1, r5
   34fb6:	f00b f809 	bl	3ffcc <__aeabi_uldivmod>
   34fba:	2e0a      	cmp	r6, #10
   34fbc:	f175 0300 	sbcs.w	r3, r5, #0
   34fc0:	f0c0 83bf 	bcc.w	35742 <_svfprintf_r+0x139a>
   34fc4:	4606      	mov	r6, r0
   34fc6:	460d      	mov	r5, r1
   34fc8:	46cb      	mov	fp, r9
   34fca:	e7c5      	b.n	34f58 <_svfprintf_r+0xbb0>
   34fcc:	f006 030f 	and.w	r3, r6, #15
   34fd0:	9a16      	ldr	r2, [sp, #88]	; 0x58
   34fd2:	0936      	lsrs	r6, r6, #4
   34fd4:	5cd3      	ldrb	r3, [r2, r3]
   34fd6:	ea46 7605 	orr.w	r6, r6, r5, lsl #28
   34fda:	092d      	lsrs	r5, r5, #4
   34fdc:	f809 3d01 	strb.w	r3, [r9, #-1]!
   34fe0:	ea56 0305 	orrs.w	r3, r6, r5
   34fe4:	d1f2      	bne.n	34fcc <_svfprintf_r+0xc24>
   34fe6:	e3ac      	b.n	35742 <_svfprintf_r+0x139a>
   34fe8:	b933      	cbnz	r3, 34ff8 <_svfprintf_r+0xc50>
   34fea:	f01b 0f01 	tst.w	fp, #1
   34fee:	d003      	beq.n	34ff8 <_svfprintf_r+0xc50>
   34ff0:	2330      	movs	r3, #48	; 0x30
   34ff2:	f88d 3137 	strb.w	r3, [sp, #311]	; 0x137
   34ff6:	e7a5      	b.n	34f44 <_svfprintf_r+0xb9c>
   34ff8:	f50d 799c 	add.w	r9, sp, #312	; 0x138
   34ffc:	e3a1      	b.n	35742 <_svfprintf_r+0x139a>
   34ffe:	9b06      	ldr	r3, [sp, #24]
   35000:	2b00      	cmp	r3, #0
   35002:	f000 8370 	beq.w	356e6 <_svfprintf_r+0x133e>
   35006:	2000      	movs	r0, #0
   35008:	f88d 30d4 	strb.w	r3, [sp, #212]	; 0xd4
   3500c:	9607      	str	r6, [sp, #28]
   3500e:	f88d 006b 	strb.w	r0, [sp, #107]	; 0x6b
   35012:	f7ff bb2e 	b.w	34672 <_svfprintf_r+0x2ca>
   35016:	9e03      	ldr	r6, [sp, #12]
   35018:	f7ff bb9d 	b.w	34756 <_svfprintf_r+0x3ae>
   3501c:	2010      	movs	r0, #16
   3501e:	2b07      	cmp	r3, #7
   35020:	4402      	add	r2, r0
   35022:	6060      	str	r0, [r4, #4]
   35024:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   35028:	dd08      	ble.n	3503c <_svfprintf_r+0xc94>
   3502a:	aa22      	add	r2, sp, #136	; 0x88
   3502c:	4641      	mov	r1, r8
   3502e:	4650      	mov	r0, sl
   35030:	f007 f902 	bl	3c238 <__ssprint_r>
   35034:	2800      	cmp	r0, #0
   35036:	f040 8334 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3503a:	a925      	add	r1, sp, #148	; 0x94
   3503c:	3f10      	subs	r7, #16
   3503e:	460c      	mov	r4, r1
   35040:	e4f6      	b.n	34a30 <_svfprintf_r+0x688>
   35042:	460c      	mov	r4, r1
   35044:	e50f      	b.n	34a66 <_svfprintf_r+0x6be>
   35046:	aa22      	add	r2, sp, #136	; 0x88
   35048:	4641      	mov	r1, r8
   3504a:	4650      	mov	r0, sl
   3504c:	f007 f8f4 	bl	3c238 <__ssprint_r>
   35050:	2800      	cmp	r0, #0
   35052:	f040 8326 	bne.w	356a2 <_svfprintf_r+0x12fa>
   35056:	ac25      	add	r4, sp, #148	; 0x94
   35058:	e517      	b.n	34a8a <_svfprintf_r+0x6e2>
   3505a:	aa22      	add	r2, sp, #136	; 0x88
   3505c:	4641      	mov	r1, r8
   3505e:	4650      	mov	r0, sl
   35060:	f007 f8ea 	bl	3c238 <__ssprint_r>
   35064:	2800      	cmp	r0, #0
   35066:	f040 831c 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3506a:	ac25      	add	r4, sp, #148	; 0x94
   3506c:	e51d      	b.n	34aaa <_svfprintf_r+0x702>
   3506e:	2010      	movs	r0, #16
   35070:	2b07      	cmp	r3, #7
   35072:	4402      	add	r2, r0
   35074:	6060      	str	r0, [r4, #4]
   35076:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   3507a:	dd08      	ble.n	3508e <_svfprintf_r+0xce6>
   3507c:	aa22      	add	r2, sp, #136	; 0x88
   3507e:	4641      	mov	r1, r8
   35080:	4650      	mov	r0, sl
   35082:	f007 f8d9 	bl	3c238 <__ssprint_r>
   35086:	2800      	cmp	r0, #0
   35088:	f040 830b 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3508c:	a925      	add	r1, sp, #148	; 0x94
   3508e:	3f10      	subs	r7, #16
   35090:	460c      	mov	r4, r1
   35092:	e512      	b.n	34aba <_svfprintf_r+0x712>
   35094:	460c      	mov	r4, r1
   35096:	e52b      	b.n	34af0 <_svfprintf_r+0x748>
   35098:	2010      	movs	r0, #16
   3509a:	2b07      	cmp	r3, #7
   3509c:	4402      	add	r2, r0
   3509e:	6060      	str	r0, [r4, #4]
   350a0:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   350a4:	dd08      	ble.n	350b8 <_svfprintf_r+0xd10>
   350a6:	aa22      	add	r2, sp, #136	; 0x88
   350a8:	4641      	mov	r1, r8
   350aa:	4650      	mov	r0, sl
   350ac:	f007 f8c4 	bl	3c238 <__ssprint_r>
   350b0:	2800      	cmp	r0, #0
   350b2:	f040 82f6 	bne.w	356a2 <_svfprintf_r+0x12fa>
   350b6:	a925      	add	r1, sp, #148	; 0x94
   350b8:	3e10      	subs	r6, #16
   350ba:	460c      	mov	r4, r1
   350bc:	e52a      	b.n	34b14 <_svfprintf_r+0x76c>
   350be:	460c      	mov	r4, r1
   350c0:	e542      	b.n	34b48 <_svfprintf_r+0x7a0>
   350c2:	bf00      	nop
   350c4:	000404df 	.word	0x000404df
   350c8:	000404f0 	.word	0x000404f0
   350cc:	9b06      	ldr	r3, [sp, #24]
   350ce:	2b65      	cmp	r3, #101	; 0x65
   350d0:	f340 8230 	ble.w	35534 <_svfprintf_r+0x118c>
   350d4:	2200      	movs	r2, #0
   350d6:	2300      	movs	r3, #0
   350d8:	e9dd 010e 	ldrd	r0, r1, [sp, #56]	; 0x38
   350dc:	f00a fe96 	bl	3fe0c <__aeabi_dcmpeq>
   350e0:	2800      	cmp	r0, #0
   350e2:	d068      	beq.n	351b6 <_svfprintf_r+0xe0e>
   350e4:	4b6d      	ldr	r3, [pc, #436]	; (3529c <_svfprintf_r+0xef4>)
   350e6:	6023      	str	r3, [r4, #0]
   350e8:	2301      	movs	r3, #1
   350ea:	441e      	add	r6, r3
   350ec:	6063      	str	r3, [r4, #4]
   350ee:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   350f0:	9624      	str	r6, [sp, #144]	; 0x90
   350f2:	3301      	adds	r3, #1
   350f4:	2b07      	cmp	r3, #7
   350f6:	9323      	str	r3, [sp, #140]	; 0x8c
   350f8:	dc37      	bgt.n	3516a <_svfprintf_r+0xdc2>
   350fa:	3408      	adds	r4, #8
   350fc:	9b1c      	ldr	r3, [sp, #112]	; 0x70
   350fe:	9a05      	ldr	r2, [sp, #20]
   35100:	4293      	cmp	r3, r2
   35102:	db03      	blt.n	3510c <_svfprintf_r+0xd64>
   35104:	f01b 0f01 	tst.w	fp, #1
   35108:	f43f ad30 	beq.w	34b6c <_svfprintf_r+0x7c4>
   3510c:	9b13      	ldr	r3, [sp, #76]	; 0x4c
   3510e:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   35110:	6023      	str	r3, [r4, #0]
   35112:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   35114:	6063      	str	r3, [r4, #4]
   35116:	9b24      	ldr	r3, [sp, #144]	; 0x90
   35118:	4413      	add	r3, r2
   3511a:	9324      	str	r3, [sp, #144]	; 0x90
   3511c:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   3511e:	3301      	adds	r3, #1
   35120:	2b07      	cmp	r3, #7
   35122:	9323      	str	r3, [sp, #140]	; 0x8c
   35124:	dc2b      	bgt.n	3517e <_svfprintf_r+0xdd6>
   35126:	3408      	adds	r4, #8
   35128:	9b05      	ldr	r3, [sp, #20]
   3512a:	1e5d      	subs	r5, r3, #1
   3512c:	2d00      	cmp	r5, #0
   3512e:	f77f ad1d 	ble.w	34b6c <_svfprintf_r+0x7c4>
   35132:	4e5b      	ldr	r6, [pc, #364]	; (352a0 <_svfprintf_r+0xef8>)
   35134:	2710      	movs	r7, #16
   35136:	2d10      	cmp	r5, #16
   35138:	f104 0108 	add.w	r1, r4, #8
   3513c:	6026      	str	r6, [r4, #0]
   3513e:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   35142:	f103 0301 	add.w	r3, r3, #1
   35146:	dc24      	bgt.n	35192 <_svfprintf_r+0xdea>
   35148:	6065      	str	r5, [r4, #4]
   3514a:	2b07      	cmp	r3, #7
   3514c:	4415      	add	r5, r2
   3514e:	e9cd 3523 	strd	r3, r5, [sp, #140]	; 0x8c
   35152:	f340 8285 	ble.w	35660 <_svfprintf_r+0x12b8>
   35156:	aa22      	add	r2, sp, #136	; 0x88
   35158:	4641      	mov	r1, r8
   3515a:	4650      	mov	r0, sl
   3515c:	f007 f86c 	bl	3c238 <__ssprint_r>
   35160:	2800      	cmp	r0, #0
   35162:	f040 829e 	bne.w	356a2 <_svfprintf_r+0x12fa>
   35166:	ac25      	add	r4, sp, #148	; 0x94
   35168:	e500      	b.n	34b6c <_svfprintf_r+0x7c4>
   3516a:	aa22      	add	r2, sp, #136	; 0x88
   3516c:	4641      	mov	r1, r8
   3516e:	4650      	mov	r0, sl
   35170:	f007 f862 	bl	3c238 <__ssprint_r>
   35174:	2800      	cmp	r0, #0
   35176:	f040 8294 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3517a:	ac25      	add	r4, sp, #148	; 0x94
   3517c:	e7be      	b.n	350fc <_svfprintf_r+0xd54>
   3517e:	aa22      	add	r2, sp, #136	; 0x88
   35180:	4641      	mov	r1, r8
   35182:	4650      	mov	r0, sl
   35184:	f007 f858 	bl	3c238 <__ssprint_r>
   35188:	2800      	cmp	r0, #0
   3518a:	f040 828a 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3518e:	ac25      	add	r4, sp, #148	; 0x94
   35190:	e7ca      	b.n	35128 <_svfprintf_r+0xd80>
   35192:	3210      	adds	r2, #16
   35194:	2b07      	cmp	r3, #7
   35196:	6067      	str	r7, [r4, #4]
   35198:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   3519c:	dd08      	ble.n	351b0 <_svfprintf_r+0xe08>
   3519e:	aa22      	add	r2, sp, #136	; 0x88
   351a0:	4641      	mov	r1, r8
   351a2:	4650      	mov	r0, sl
   351a4:	f007 f848 	bl	3c238 <__ssprint_r>
   351a8:	2800      	cmp	r0, #0
   351aa:	f040 827a 	bne.w	356a2 <_svfprintf_r+0x12fa>
   351ae:	a925      	add	r1, sp, #148	; 0x94
   351b0:	3d10      	subs	r5, #16
   351b2:	460c      	mov	r4, r1
   351b4:	e7bf      	b.n	35136 <_svfprintf_r+0xd8e>
   351b6:	9b1c      	ldr	r3, [sp, #112]	; 0x70
   351b8:	2b00      	cmp	r3, #0
   351ba:	dc73      	bgt.n	352a4 <_svfprintf_r+0xefc>
   351bc:	4b37      	ldr	r3, [pc, #220]	; (3529c <_svfprintf_r+0xef4>)
   351be:	6023      	str	r3, [r4, #0]
   351c0:	2301      	movs	r3, #1
   351c2:	441e      	add	r6, r3
   351c4:	6063      	str	r3, [r4, #4]
   351c6:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   351c8:	9624      	str	r6, [sp, #144]	; 0x90
   351ca:	3301      	adds	r3, #1
   351cc:	2b07      	cmp	r3, #7
   351ce:	9323      	str	r3, [sp, #140]	; 0x8c
   351d0:	dc3d      	bgt.n	3524e <_svfprintf_r+0xea6>
   351d2:	3408      	adds	r4, #8
   351d4:	9905      	ldr	r1, [sp, #20]
   351d6:	9a1c      	ldr	r2, [sp, #112]	; 0x70
   351d8:	9b24      	ldr	r3, [sp, #144]	; 0x90
   351da:	430a      	orrs	r2, r1
   351dc:	f00b 0101 	and.w	r1, fp, #1
   351e0:	430a      	orrs	r2, r1
   351e2:	f43f acc3 	beq.w	34b6c <_svfprintf_r+0x7c4>
   351e6:	9a13      	ldr	r2, [sp, #76]	; 0x4c
   351e8:	6022      	str	r2, [r4, #0]
   351ea:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   351ec:	4413      	add	r3, r2
   351ee:	6062      	str	r2, [r4, #4]
   351f0:	9324      	str	r3, [sp, #144]	; 0x90
   351f2:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   351f4:	3301      	adds	r3, #1
   351f6:	2b07      	cmp	r3, #7
   351f8:	9323      	str	r3, [sp, #140]	; 0x8c
   351fa:	dc32      	bgt.n	35262 <_svfprintf_r+0xeba>
   351fc:	3408      	adds	r4, #8
   351fe:	9d1c      	ldr	r5, [sp, #112]	; 0x70
   35200:	2d00      	cmp	r5, #0
   35202:	da1b      	bge.n	3523c <_svfprintf_r+0xe94>
   35204:	426d      	negs	r5, r5
   35206:	4623      	mov	r3, r4
   35208:	4e25      	ldr	r6, [pc, #148]	; (352a0 <_svfprintf_r+0xef8>)
   3520a:	2710      	movs	r7, #16
   3520c:	2d10      	cmp	r5, #16
   3520e:	f104 0408 	add.w	r4, r4, #8
   35212:	601e      	str	r6, [r3, #0]
   35214:	e9dd 2123 	ldrd	r2, r1, [sp, #140]	; 0x8c
   35218:	f102 0201 	add.w	r2, r2, #1
   3521c:	dc2b      	bgt.n	35276 <_svfprintf_r+0xece>
   3521e:	605d      	str	r5, [r3, #4]
   35220:	2a07      	cmp	r2, #7
   35222:	440d      	add	r5, r1
   35224:	e9cd 2523 	strd	r2, r5, [sp, #140]	; 0x8c
   35228:	dd08      	ble.n	3523c <_svfprintf_r+0xe94>
   3522a:	aa22      	add	r2, sp, #136	; 0x88
   3522c:	4641      	mov	r1, r8
   3522e:	4650      	mov	r0, sl
   35230:	f007 f802 	bl	3c238 <__ssprint_r>
   35234:	2800      	cmp	r0, #0
   35236:	f040 8234 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3523a:	ac25      	add	r4, sp, #148	; 0x94
   3523c:	9b05      	ldr	r3, [sp, #20]
   3523e:	9a05      	ldr	r2, [sp, #20]
   35240:	6063      	str	r3, [r4, #4]
   35242:	9b24      	ldr	r3, [sp, #144]	; 0x90
   35244:	f8c4 9000 	str.w	r9, [r4]
   35248:	4413      	add	r3, r2
   3524a:	9324      	str	r3, [sp, #144]	; 0x90
   3524c:	e487      	b.n	34b5e <_svfprintf_r+0x7b6>
   3524e:	aa22      	add	r2, sp, #136	; 0x88
   35250:	4641      	mov	r1, r8
   35252:	4650      	mov	r0, sl
   35254:	f006 fff0 	bl	3c238 <__ssprint_r>
   35258:	2800      	cmp	r0, #0
   3525a:	f040 8222 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3525e:	ac25      	add	r4, sp, #148	; 0x94
   35260:	e7b8      	b.n	351d4 <_svfprintf_r+0xe2c>
   35262:	aa22      	add	r2, sp, #136	; 0x88
   35264:	4641      	mov	r1, r8
   35266:	4650      	mov	r0, sl
   35268:	f006 ffe6 	bl	3c238 <__ssprint_r>
   3526c:	2800      	cmp	r0, #0
   3526e:	f040 8218 	bne.w	356a2 <_svfprintf_r+0x12fa>
   35272:	ac25      	add	r4, sp, #148	; 0x94
   35274:	e7c3      	b.n	351fe <_svfprintf_r+0xe56>
   35276:	3110      	adds	r1, #16
   35278:	2a07      	cmp	r2, #7
   3527a:	605f      	str	r7, [r3, #4]
   3527c:	e9cd 2123 	strd	r2, r1, [sp, #140]	; 0x8c
   35280:	dd08      	ble.n	35294 <_svfprintf_r+0xeec>
   35282:	aa22      	add	r2, sp, #136	; 0x88
   35284:	4641      	mov	r1, r8
   35286:	4650      	mov	r0, sl
   35288:	f006 ffd6 	bl	3c238 <__ssprint_r>
   3528c:	2800      	cmp	r0, #0
   3528e:	f040 8208 	bne.w	356a2 <_svfprintf_r+0x12fa>
   35292:	ac25      	add	r4, sp, #148	; 0x94
   35294:	3d10      	subs	r5, #16
   35296:	4623      	mov	r3, r4
   35298:	e7b8      	b.n	3520c <_svfprintf_r+0xe64>
   3529a:	bf00      	nop
   3529c:	00040501 	.word	0x00040501
   352a0:	00040513 	.word	0x00040513
   352a4:	9f05      	ldr	r7, [sp, #20]
   352a6:	42af      	cmp	r7, r5
   352a8:	bfa8      	it	ge
   352aa:	462f      	movge	r7, r5
   352ac:	2f00      	cmp	r7, #0
   352ae:	dd0a      	ble.n	352c6 <_svfprintf_r+0xf1e>
   352b0:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   352b2:	443e      	add	r6, r7
   352b4:	3301      	adds	r3, #1
   352b6:	9624      	str	r6, [sp, #144]	; 0x90
   352b8:	2b07      	cmp	r3, #7
   352ba:	9323      	str	r3, [sp, #140]	; 0x8c
   352bc:	e9c4 9700 	strd	r9, r7, [r4]
   352c0:	f300 8085 	bgt.w	353ce <_svfprintf_r+0x1026>
   352c4:	3408      	adds	r4, #8
   352c6:	2f00      	cmp	r7, #0
   352c8:	bfac      	ite	ge
   352ca:	1bee      	subge	r6, r5, r7
   352cc:	462e      	movlt	r6, r5
   352ce:	2e00      	cmp	r6, #0
   352d0:	dd19      	ble.n	35306 <_svfprintf_r+0xf5e>
   352d2:	4f97      	ldr	r7, [pc, #604]	; (35530 <_svfprintf_r+0x1188>)
   352d4:	2e10      	cmp	r6, #16
   352d6:	f104 0108 	add.w	r1, r4, #8
   352da:	6027      	str	r7, [r4, #0]
   352dc:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   352e0:	f103 0301 	add.w	r3, r3, #1
   352e4:	dc7d      	bgt.n	353e2 <_svfprintf_r+0x103a>
   352e6:	6066      	str	r6, [r4, #4]
   352e8:	2b07      	cmp	r3, #7
   352ea:	4416      	add	r6, r2
   352ec:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   352f0:	f340 808a 	ble.w	35408 <_svfprintf_r+0x1060>
   352f4:	aa22      	add	r2, sp, #136	; 0x88
   352f6:	4641      	mov	r1, r8
   352f8:	4650      	mov	r0, sl
   352fa:	f006 ff9d 	bl	3c238 <__ssprint_r>
   352fe:	2800      	cmp	r0, #0
   35300:	f040 81cf 	bne.w	356a2 <_svfprintf_r+0x12fa>
   35304:	ac25      	add	r4, sp, #148	; 0x94
   35306:	f41b 6f80 	tst.w	fp, #1024	; 0x400
   3530a:	444d      	add	r5, r9
   3530c:	d00a      	beq.n	35324 <_svfprintf_r+0xf7c>
   3530e:	9b09      	ldr	r3, [sp, #36]	; 0x24
   35310:	2b00      	cmp	r3, #0
   35312:	d17b      	bne.n	3540c <_svfprintf_r+0x1064>
   35314:	9b08      	ldr	r3, [sp, #32]
   35316:	2b00      	cmp	r3, #0
   35318:	d17b      	bne.n	35412 <_svfprintf_r+0x106a>
   3531a:	9b05      	ldr	r3, [sp, #20]
   3531c:	444b      	add	r3, r9
   3531e:	429d      	cmp	r5, r3
   35320:	bf28      	it	cs
   35322:	461d      	movcs	r5, r3
   35324:	9b1c      	ldr	r3, [sp, #112]	; 0x70
   35326:	9a05      	ldr	r2, [sp, #20]
   35328:	4293      	cmp	r3, r2
   3532a:	db02      	blt.n	35332 <_svfprintf_r+0xf8a>
   3532c:	f01b 0f01 	tst.w	fp, #1
   35330:	d00e      	beq.n	35350 <_svfprintf_r+0xfa8>
   35332:	9b13      	ldr	r3, [sp, #76]	; 0x4c
   35334:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   35336:	6023      	str	r3, [r4, #0]
   35338:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3533a:	6063      	str	r3, [r4, #4]
   3533c:	9b24      	ldr	r3, [sp, #144]	; 0x90
   3533e:	4413      	add	r3, r2
   35340:	9324      	str	r3, [sp, #144]	; 0x90
   35342:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   35344:	3301      	adds	r3, #1
   35346:	2b07      	cmp	r3, #7
   35348:	9323      	str	r3, [sp, #140]	; 0x8c
   3534a:	f300 80dd 	bgt.w	35508 <_svfprintf_r+0x1160>
   3534e:	3408      	adds	r4, #8
   35350:	9b05      	ldr	r3, [sp, #20]
   35352:	9e1c      	ldr	r6, [sp, #112]	; 0x70
   35354:	eb09 0203 	add.w	r2, r9, r3
   35358:	1b9e      	subs	r6, r3, r6
   3535a:	1b52      	subs	r2, r2, r5
   3535c:	4296      	cmp	r6, r2
   3535e:	bfa8      	it	ge
   35360:	4616      	movge	r6, r2
   35362:	2e00      	cmp	r6, #0
   35364:	dd0b      	ble.n	3537e <_svfprintf_r+0xfd6>
   35366:	9b24      	ldr	r3, [sp, #144]	; 0x90
   35368:	4433      	add	r3, r6
   3536a:	9324      	str	r3, [sp, #144]	; 0x90
   3536c:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   3536e:	3301      	adds	r3, #1
   35370:	2b07      	cmp	r3, #7
   35372:	9323      	str	r3, [sp, #140]	; 0x8c
   35374:	e9c4 5600 	strd	r5, r6, [r4]
   35378:	f300 80d0 	bgt.w	3551c <_svfprintf_r+0x1174>
   3537c:	3408      	adds	r4, #8
   3537e:	9d1c      	ldr	r5, [sp, #112]	; 0x70
   35380:	2e00      	cmp	r6, #0
   35382:	9b05      	ldr	r3, [sp, #20]
   35384:	eba3 0505 	sub.w	r5, r3, r5
   35388:	bfa8      	it	ge
   3538a:	1bad      	subge	r5, r5, r6
   3538c:	2d00      	cmp	r5, #0
   3538e:	f77f abed 	ble.w	34b6c <_svfprintf_r+0x7c4>
   35392:	4e67      	ldr	r6, [pc, #412]	; (35530 <_svfprintf_r+0x1188>)
   35394:	2710      	movs	r7, #16
   35396:	2d10      	cmp	r5, #16
   35398:	f104 0108 	add.w	r1, r4, #8
   3539c:	6026      	str	r6, [r4, #0]
   3539e:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   353a2:	f103 0301 	add.w	r3, r3, #1
   353a6:	f77f aecf 	ble.w	35148 <_svfprintf_r+0xda0>
   353aa:	3210      	adds	r2, #16
   353ac:	2b07      	cmp	r3, #7
   353ae:	6067      	str	r7, [r4, #4]
   353b0:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   353b4:	dd08      	ble.n	353c8 <_svfprintf_r+0x1020>
   353b6:	aa22      	add	r2, sp, #136	; 0x88
   353b8:	4641      	mov	r1, r8
   353ba:	4650      	mov	r0, sl
   353bc:	f006 ff3c 	bl	3c238 <__ssprint_r>
   353c0:	2800      	cmp	r0, #0
   353c2:	f040 816e 	bne.w	356a2 <_svfprintf_r+0x12fa>
   353c6:	a925      	add	r1, sp, #148	; 0x94
   353c8:	3d10      	subs	r5, #16
   353ca:	460c      	mov	r4, r1
   353cc:	e7e3      	b.n	35396 <_svfprintf_r+0xfee>
   353ce:	aa22      	add	r2, sp, #136	; 0x88
   353d0:	4641      	mov	r1, r8
   353d2:	4650      	mov	r0, sl
   353d4:	f006 ff30 	bl	3c238 <__ssprint_r>
   353d8:	2800      	cmp	r0, #0
   353da:	f040 8162 	bne.w	356a2 <_svfprintf_r+0x12fa>
   353de:	ac25      	add	r4, sp, #148	; 0x94
   353e0:	e771      	b.n	352c6 <_svfprintf_r+0xf1e>
   353e2:	2010      	movs	r0, #16
   353e4:	2b07      	cmp	r3, #7
   353e6:	4402      	add	r2, r0
   353e8:	6060      	str	r0, [r4, #4]
   353ea:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   353ee:	dd08      	ble.n	35402 <_svfprintf_r+0x105a>
   353f0:	aa22      	add	r2, sp, #136	; 0x88
   353f2:	4641      	mov	r1, r8
   353f4:	4650      	mov	r0, sl
   353f6:	f006 ff1f 	bl	3c238 <__ssprint_r>
   353fa:	2800      	cmp	r0, #0
   353fc:	f040 8151 	bne.w	356a2 <_svfprintf_r+0x12fa>
   35400:	a925      	add	r1, sp, #148	; 0x94
   35402:	3e10      	subs	r6, #16
   35404:	460c      	mov	r4, r1
   35406:	e765      	b.n	352d4 <_svfprintf_r+0xf2c>
   35408:	460c      	mov	r4, r1
   3540a:	e77c      	b.n	35306 <_svfprintf_r+0xf5e>
   3540c:	9b08      	ldr	r3, [sp, #32]
   3540e:	2b00      	cmp	r3, #0
   35410:	d04a      	beq.n	354a8 <_svfprintf_r+0x1100>
   35412:	9b08      	ldr	r3, [sp, #32]
   35414:	3b01      	subs	r3, #1
   35416:	9308      	str	r3, [sp, #32]
   35418:	9b14      	ldr	r3, [sp, #80]	; 0x50
   3541a:	9a12      	ldr	r2, [sp, #72]	; 0x48
   3541c:	6023      	str	r3, [r4, #0]
   3541e:	9b12      	ldr	r3, [sp, #72]	; 0x48
   35420:	6063      	str	r3, [r4, #4]
   35422:	9b24      	ldr	r3, [sp, #144]	; 0x90
   35424:	4413      	add	r3, r2
   35426:	9324      	str	r3, [sp, #144]	; 0x90
   35428:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   3542a:	3301      	adds	r3, #1
   3542c:	2b07      	cmp	r3, #7
   3542e:	9323      	str	r3, [sp, #140]	; 0x8c
   35430:	dc41      	bgt.n	354b6 <_svfprintf_r+0x110e>
   35432:	3408      	adds	r4, #8
   35434:	9b05      	ldr	r3, [sp, #20]
   35436:	9a0a      	ldr	r2, [sp, #40]	; 0x28
   35438:	eb09 0703 	add.w	r7, r9, r3
   3543c:	1b7b      	subs	r3, r7, r5
   3543e:	7817      	ldrb	r7, [r2, #0]
   35440:	429f      	cmp	r7, r3
   35442:	bfa8      	it	ge
   35444:	461f      	movge	r7, r3
   35446:	2f00      	cmp	r7, #0
   35448:	dd0a      	ble.n	35460 <_svfprintf_r+0x10b8>
   3544a:	9b24      	ldr	r3, [sp, #144]	; 0x90
   3544c:	443b      	add	r3, r7
   3544e:	9324      	str	r3, [sp, #144]	; 0x90
   35450:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   35452:	3301      	adds	r3, #1
   35454:	2b07      	cmp	r3, #7
   35456:	9323      	str	r3, [sp, #140]	; 0x8c
   35458:	e9c4 5700 	strd	r5, r7, [r4]
   3545c:	dc35      	bgt.n	354ca <_svfprintf_r+0x1122>
   3545e:	3408      	adds	r4, #8
   35460:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   35462:	2f00      	cmp	r7, #0
   35464:	781e      	ldrb	r6, [r3, #0]
   35466:	bfa8      	it	ge
   35468:	1bf6      	subge	r6, r6, r7
   3546a:	2e00      	cmp	r6, #0
   3546c:	dd18      	ble.n	354a0 <_svfprintf_r+0x10f8>
   3546e:	4830      	ldr	r0, [pc, #192]	; (35530 <_svfprintf_r+0x1188>)
   35470:	2e10      	cmp	r6, #16
   35472:	f104 0108 	add.w	r1, r4, #8
   35476:	6020      	str	r0, [r4, #0]
   35478:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   3547c:	f103 0301 	add.w	r3, r3, #1
   35480:	dc2d      	bgt.n	354de <_svfprintf_r+0x1136>
   35482:	4432      	add	r2, r6
   35484:	2b07      	cmp	r3, #7
   35486:	6066      	str	r6, [r4, #4]
   35488:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   3548c:	dd3a      	ble.n	35504 <_svfprintf_r+0x115c>
   3548e:	aa22      	add	r2, sp, #136	; 0x88
   35490:	4641      	mov	r1, r8
   35492:	4650      	mov	r0, sl
   35494:	f006 fed0 	bl	3c238 <__ssprint_r>
   35498:	2800      	cmp	r0, #0
   3549a:	f040 8102 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3549e:	ac25      	add	r4, sp, #148	; 0x94
   354a0:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   354a2:	781b      	ldrb	r3, [r3, #0]
   354a4:	441d      	add	r5, r3
   354a6:	e732      	b.n	3530e <_svfprintf_r+0xf66>
   354a8:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   354aa:	3b01      	subs	r3, #1
   354ac:	930a      	str	r3, [sp, #40]	; 0x28
   354ae:	9b09      	ldr	r3, [sp, #36]	; 0x24
   354b0:	3b01      	subs	r3, #1
   354b2:	9309      	str	r3, [sp, #36]	; 0x24
   354b4:	e7b0      	b.n	35418 <_svfprintf_r+0x1070>
   354b6:	aa22      	add	r2, sp, #136	; 0x88
   354b8:	4641      	mov	r1, r8
   354ba:	4650      	mov	r0, sl
   354bc:	f006 febc 	bl	3c238 <__ssprint_r>
   354c0:	2800      	cmp	r0, #0
   354c2:	f040 80ee 	bne.w	356a2 <_svfprintf_r+0x12fa>
   354c6:	ac25      	add	r4, sp, #148	; 0x94
   354c8:	e7b4      	b.n	35434 <_svfprintf_r+0x108c>
   354ca:	aa22      	add	r2, sp, #136	; 0x88
   354cc:	4641      	mov	r1, r8
   354ce:	4650      	mov	r0, sl
   354d0:	f006 feb2 	bl	3c238 <__ssprint_r>
   354d4:	2800      	cmp	r0, #0
   354d6:	f040 80e4 	bne.w	356a2 <_svfprintf_r+0x12fa>
   354da:	ac25      	add	r4, sp, #148	; 0x94
   354dc:	e7c0      	b.n	35460 <_svfprintf_r+0x10b8>
   354de:	2010      	movs	r0, #16
   354e0:	2b07      	cmp	r3, #7
   354e2:	4402      	add	r2, r0
   354e4:	6060      	str	r0, [r4, #4]
   354e6:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   354ea:	dd08      	ble.n	354fe <_svfprintf_r+0x1156>
   354ec:	aa22      	add	r2, sp, #136	; 0x88
   354ee:	4641      	mov	r1, r8
   354f0:	4650      	mov	r0, sl
   354f2:	f006 fea1 	bl	3c238 <__ssprint_r>
   354f6:	2800      	cmp	r0, #0
   354f8:	f040 80d3 	bne.w	356a2 <_svfprintf_r+0x12fa>
   354fc:	a925      	add	r1, sp, #148	; 0x94
   354fe:	3e10      	subs	r6, #16
   35500:	460c      	mov	r4, r1
   35502:	e7b4      	b.n	3546e <_svfprintf_r+0x10c6>
   35504:	460c      	mov	r4, r1
   35506:	e7cb      	b.n	354a0 <_svfprintf_r+0x10f8>
   35508:	aa22      	add	r2, sp, #136	; 0x88
   3550a:	4641      	mov	r1, r8
   3550c:	4650      	mov	r0, sl
   3550e:	f006 fe93 	bl	3c238 <__ssprint_r>
   35512:	2800      	cmp	r0, #0
   35514:	f040 80c5 	bne.w	356a2 <_svfprintf_r+0x12fa>
   35518:	ac25      	add	r4, sp, #148	; 0x94
   3551a:	e719      	b.n	35350 <_svfprintf_r+0xfa8>
   3551c:	aa22      	add	r2, sp, #136	; 0x88
   3551e:	4641      	mov	r1, r8
   35520:	4650      	mov	r0, sl
   35522:	f006 fe89 	bl	3c238 <__ssprint_r>
   35526:	2800      	cmp	r0, #0
   35528:	f040 80bb 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3552c:	ac25      	add	r4, sp, #148	; 0x94
   3552e:	e726      	b.n	3537e <_svfprintf_r+0xfd6>
   35530:	00040513 	.word	0x00040513
   35534:	9a05      	ldr	r2, [sp, #20]
   35536:	3601      	adds	r6, #1
   35538:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   3553a:	f104 0508 	add.w	r5, r4, #8
   3553e:	2a01      	cmp	r2, #1
   35540:	f103 0301 	add.w	r3, r3, #1
   35544:	dc02      	bgt.n	3554c <_svfprintf_r+0x11a4>
   35546:	f01b 0f01 	tst.w	fp, #1
   3554a:	d07e      	beq.n	3564a <_svfprintf_r+0x12a2>
   3554c:	2201      	movs	r2, #1
   3554e:	2b07      	cmp	r3, #7
   35550:	f8c4 9000 	str.w	r9, [r4]
   35554:	6062      	str	r2, [r4, #4]
   35556:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   3555a:	dd08      	ble.n	3556e <_svfprintf_r+0x11c6>
   3555c:	aa22      	add	r2, sp, #136	; 0x88
   3555e:	4641      	mov	r1, r8
   35560:	4650      	mov	r0, sl
   35562:	f006 fe69 	bl	3c238 <__ssprint_r>
   35566:	2800      	cmp	r0, #0
   35568:	f040 809b 	bne.w	356a2 <_svfprintf_r+0x12fa>
   3556c:	ad25      	add	r5, sp, #148	; 0x94
   3556e:	9b13      	ldr	r3, [sp, #76]	; 0x4c
   35570:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   35572:	602b      	str	r3, [r5, #0]
   35574:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   35576:	606b      	str	r3, [r5, #4]
   35578:	9b24      	ldr	r3, [sp, #144]	; 0x90
   3557a:	4413      	add	r3, r2
   3557c:	9324      	str	r3, [sp, #144]	; 0x90
   3557e:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   35580:	3301      	adds	r3, #1
   35582:	2b07      	cmp	r3, #7
   35584:	9323      	str	r3, [sp, #140]	; 0x8c
   35586:	dc32      	bgt.n	355ee <_svfprintf_r+0x1246>
   35588:	3508      	adds	r5, #8
   3558a:	9b05      	ldr	r3, [sp, #20]
   3558c:	2200      	movs	r2, #0
   3558e:	1e5c      	subs	r4, r3, #1
   35590:	2300      	movs	r3, #0
   35592:	e9dd 010e 	ldrd	r0, r1, [sp, #56]	; 0x38
   35596:	f00a fc39 	bl	3fe0c <__aeabi_dcmpeq>
   3559a:	2800      	cmp	r0, #0
   3559c:	d130      	bne.n	35600 <_svfprintf_r+0x1258>
   3559e:	9923      	ldr	r1, [sp, #140]	; 0x8c
   355a0:	f109 0001 	add.w	r0, r9, #1
   355a4:	9b24      	ldr	r3, [sp, #144]	; 0x90
   355a6:	3101      	adds	r1, #1
   355a8:	9a05      	ldr	r2, [sp, #20]
   355aa:	3b01      	subs	r3, #1
   355ac:	2907      	cmp	r1, #7
   355ae:	4413      	add	r3, r2
   355b0:	e9c5 0400 	strd	r0, r4, [r5]
   355b4:	e9cd 1323 	strd	r1, r3, [sp, #140]	; 0x8c
   355b8:	dd50      	ble.n	3565c <_svfprintf_r+0x12b4>
   355ba:	aa22      	add	r2, sp, #136	; 0x88
   355bc:	4641      	mov	r1, r8
   355be:	4650      	mov	r0, sl
   355c0:	f006 fe3a 	bl	3c238 <__ssprint_r>
   355c4:	2800      	cmp	r0, #0
   355c6:	d16c      	bne.n	356a2 <_svfprintf_r+0x12fa>
   355c8:	ad25      	add	r5, sp, #148	; 0x94
   355ca:	ab1e      	add	r3, sp, #120	; 0x78
   355cc:	9a15      	ldr	r2, [sp, #84]	; 0x54
   355ce:	602b      	str	r3, [r5, #0]
   355d0:	9b15      	ldr	r3, [sp, #84]	; 0x54
   355d2:	606b      	str	r3, [r5, #4]
   355d4:	9b24      	ldr	r3, [sp, #144]	; 0x90
   355d6:	4413      	add	r3, r2
   355d8:	9324      	str	r3, [sp, #144]	; 0x90
   355da:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   355dc:	3301      	adds	r3, #1
   355de:	2b07      	cmp	r3, #7
   355e0:	9323      	str	r3, [sp, #140]	; 0x8c
   355e2:	f73f adb8 	bgt.w	35156 <_svfprintf_r+0xdae>
   355e6:	f105 0408 	add.w	r4, r5, #8
   355ea:	f7ff babf 	b.w	34b6c <_svfprintf_r+0x7c4>
   355ee:	aa22      	add	r2, sp, #136	; 0x88
   355f0:	4641      	mov	r1, r8
   355f2:	4650      	mov	r0, sl
   355f4:	f006 fe20 	bl	3c238 <__ssprint_r>
   355f8:	2800      	cmp	r0, #0
   355fa:	d152      	bne.n	356a2 <_svfprintf_r+0x12fa>
   355fc:	ad25      	add	r5, sp, #148	; 0x94
   355fe:	e7c4      	b.n	3558a <_svfprintf_r+0x11e2>
   35600:	2c00      	cmp	r4, #0
   35602:	dde2      	ble.n	355ca <_svfprintf_r+0x1222>
   35604:	4e55      	ldr	r6, [pc, #340]	; (3575c <_svfprintf_r+0x13b4>)
   35606:	2710      	movs	r7, #16
   35608:	2c10      	cmp	r4, #16
   3560a:	f105 0108 	add.w	r1, r5, #8
   3560e:	602e      	str	r6, [r5, #0]
   35610:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   35614:	f103 0301 	add.w	r3, r3, #1
   35618:	dc07      	bgt.n	3562a <_svfprintf_r+0x1282>
   3561a:	606c      	str	r4, [r5, #4]
   3561c:	2b07      	cmp	r3, #7
   3561e:	4414      	add	r4, r2
   35620:	e9cd 3423 	strd	r3, r4, [sp, #140]	; 0x8c
   35624:	dcc9      	bgt.n	355ba <_svfprintf_r+0x1212>
   35626:	460d      	mov	r5, r1
   35628:	e7cf      	b.n	355ca <_svfprintf_r+0x1222>
   3562a:	3210      	adds	r2, #16
   3562c:	2b07      	cmp	r3, #7
   3562e:	606f      	str	r7, [r5, #4]
   35630:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   35634:	dd06      	ble.n	35644 <_svfprintf_r+0x129c>
   35636:	aa22      	add	r2, sp, #136	; 0x88
   35638:	4641      	mov	r1, r8
   3563a:	4650      	mov	r0, sl
   3563c:	f006 fdfc 	bl	3c238 <__ssprint_r>
   35640:	bb78      	cbnz	r0, 356a2 <_svfprintf_r+0x12fa>
   35642:	a925      	add	r1, sp, #148	; 0x94
   35644:	3c10      	subs	r4, #16
   35646:	460d      	mov	r5, r1
   35648:	e7de      	b.n	35608 <_svfprintf_r+0x1260>
   3564a:	2201      	movs	r2, #1
   3564c:	2b07      	cmp	r3, #7
   3564e:	f8c4 9000 	str.w	r9, [r4]
   35652:	6062      	str	r2, [r4, #4]
   35654:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   35658:	ddb7      	ble.n	355ca <_svfprintf_r+0x1222>
   3565a:	e7ae      	b.n	355ba <_svfprintf_r+0x1212>
   3565c:	3508      	adds	r5, #8
   3565e:	e7b4      	b.n	355ca <_svfprintf_r+0x1222>
   35660:	460c      	mov	r4, r1
   35662:	f7ff ba83 	b.w	34b6c <_svfprintf_r+0x7c4>
   35666:	e9dd 3210 	ldrd	r3, r2, [sp, #64]	; 0x40
   3566a:	1a9d      	subs	r5, r3, r2
   3566c:	2d00      	cmp	r5, #0
   3566e:	f77f aa81 	ble.w	34b74 <_svfprintf_r+0x7cc>
   35672:	4e3b      	ldr	r6, [pc, #236]	; (35760 <_svfprintf_r+0x13b8>)
   35674:	2710      	movs	r7, #16
   35676:	2d10      	cmp	r5, #16
   35678:	6026      	str	r6, [r4, #0]
   3567a:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   3567e:	f103 0301 	add.w	r3, r3, #1
   35682:	dc18      	bgt.n	356b6 <_svfprintf_r+0x130e>
   35684:	6065      	str	r5, [r4, #4]
   35686:	2b07      	cmp	r3, #7
   35688:	4415      	add	r5, r2
   3568a:	e9cd 3523 	strd	r3, r5, [sp, #140]	; 0x8c
   3568e:	f77f aa71 	ble.w	34b74 <_svfprintf_r+0x7cc>
   35692:	aa22      	add	r2, sp, #136	; 0x88
   35694:	4641      	mov	r1, r8
   35696:	4650      	mov	r0, sl
   35698:	f006 fdce 	bl	3c238 <__ssprint_r>
   3569c:	2800      	cmp	r0, #0
   3569e:	f43f aa69 	beq.w	34b74 <_svfprintf_r+0x7cc>
   356a2:	9b03      	ldr	r3, [sp, #12]
   356a4:	2b00      	cmp	r3, #0
   356a6:	f43f a88c 	beq.w	347c2 <_svfprintf_r+0x41a>
   356aa:	4619      	mov	r1, r3
   356ac:	4650      	mov	r0, sl
   356ae:	f003 fdf3 	bl	39298 <_free_r>
   356b2:	f7ff b886 	b.w	347c2 <_svfprintf_r+0x41a>
   356b6:	3210      	adds	r2, #16
   356b8:	2b07      	cmp	r3, #7
   356ba:	6067      	str	r7, [r4, #4]
   356bc:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   356c0:	dc02      	bgt.n	356c8 <_svfprintf_r+0x1320>
   356c2:	3408      	adds	r4, #8
   356c4:	3d10      	subs	r5, #16
   356c6:	e7d6      	b.n	35676 <_svfprintf_r+0x12ce>
   356c8:	aa22      	add	r2, sp, #136	; 0x88
   356ca:	4641      	mov	r1, r8
   356cc:	4650      	mov	r0, sl
   356ce:	f006 fdb3 	bl	3c238 <__ssprint_r>
   356d2:	2800      	cmp	r0, #0
   356d4:	d1e5      	bne.n	356a2 <_svfprintf_r+0x12fa>
   356d6:	ac25      	add	r4, sp, #148	; 0x94
   356d8:	e7f4      	b.n	356c4 <_svfprintf_r+0x131c>
   356da:	9903      	ldr	r1, [sp, #12]
   356dc:	4650      	mov	r0, sl
   356de:	f003 fddb 	bl	39298 <_free_r>
   356e2:	f7ff ba5f 	b.w	34ba4 <_svfprintf_r+0x7fc>
   356e6:	9b24      	ldr	r3, [sp, #144]	; 0x90
   356e8:	2b00      	cmp	r3, #0
   356ea:	f43f a86a 	beq.w	347c2 <_svfprintf_r+0x41a>
   356ee:	aa22      	add	r2, sp, #136	; 0x88
   356f0:	4641      	mov	r1, r8
   356f2:	4650      	mov	r0, sl
   356f4:	f006 fda0 	bl	3c238 <__ssprint_r>
   356f8:	f7ff b863 	b.w	347c2 <_svfprintf_r+0x41a>
   356fc:	ea56 0205 	orrs.w	r2, r6, r5
   35700:	465f      	mov	r7, fp
   35702:	f43f ab79 	beq.w	34df8 <_svfprintf_r+0xa50>
   35706:	2b01      	cmp	r3, #1
   35708:	f43f ac15 	beq.w	34f36 <_svfprintf_r+0xb8e>
   3570c:	2b02      	cmp	r3, #2
   3570e:	f50d 799c 	add.w	r9, sp, #312	; 0x138
   35712:	f43f ac5b 	beq.w	34fcc <_svfprintf_r+0xc24>
   35716:	f006 0307 	and.w	r3, r6, #7
   3571a:	08f6      	lsrs	r6, r6, #3
   3571c:	464a      	mov	r2, r9
   3571e:	ea46 7645 	orr.w	r6, r6, r5, lsl #29
   35722:	3330      	adds	r3, #48	; 0x30
   35724:	08ed      	lsrs	r5, r5, #3
   35726:	f809 3d01 	strb.w	r3, [r9, #-1]!
   3572a:	ea56 0105 	orrs.w	r1, r6, r5
   3572e:	d1f2      	bne.n	35716 <_svfprintf_r+0x136e>
   35730:	07f9      	lsls	r1, r7, #31
   35732:	d506      	bpl.n	35742 <_svfprintf_r+0x139a>
   35734:	2b30      	cmp	r3, #48	; 0x30
   35736:	d004      	beq.n	35742 <_svfprintf_r+0x139a>
   35738:	2330      	movs	r3, #48	; 0x30
   3573a:	f809 3c01 	strb.w	r3, [r9, #-1]
   3573e:	f1a2 0902 	sub.w	r9, r2, #2
   35742:	ab4e      	add	r3, sp, #312	; 0x138
   35744:	9e04      	ldr	r6, [sp, #16]
   35746:	46bb      	mov	fp, r7
   35748:	eba3 0309 	sub.w	r3, r3, r9
   3574c:	9304      	str	r3, [sp, #16]
   3574e:	2300      	movs	r3, #0
   35750:	9303      	str	r3, [sp, #12]
   35752:	461d      	mov	r5, r3
   35754:	e9cd 3308 	strd	r3, r3, [sp, #32]
   35758:	f7ff b94f 	b.w	349fa <_svfprintf_r+0x652>
   3575c:	00040513 	.word	0x00040513
   35760:	00040503 	.word	0x00040503

00035764 <__ssvfscanf_r>:
   35764:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   35768:	ed2d 8b02 	vpush	{d8}
   3576c:	f5ad 7d2f 	sub.w	sp, sp, #700	; 0x2bc
   35770:	461e      	mov	r6, r3
   35772:	ee08 0a10 	vmov	s16, r0
   35776:	460c      	mov	r4, r1
   35778:	f9b1 300c 	ldrsh.w	r3, [r1, #12]
   3577c:	af02      	add	r7, sp, #8
   3577e:	049d      	lsls	r5, r3, #18
   35780:	617a      	str	r2, [r7, #20]
   35782:	d406      	bmi.n	35792 <__ssvfscanf_r+0x2e>
   35784:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   35788:	818b      	strh	r3, [r1, #12]
   3578a:	6e4b      	ldr	r3, [r1, #100]	; 0x64
   3578c:	f423 5300 	bic.w	r3, r3, #8192	; 0x2000
   35790:	664b      	str	r3, [r1, #100]	; 0x64
   35792:	2300      	movs	r3, #0
   35794:	61bb      	str	r3, [r7, #24]
   35796:	637b      	str	r3, [r7, #52]	; 0x34
   35798:	633b      	str	r3, [r7, #48]	; 0x30
   3579a:	627b      	str	r3, [r7, #36]	; 0x24
   3579c:	e9c7 3307 	strd	r3, r3, [r7, #28]
   357a0:	697a      	ldr	r2, [r7, #20]
   357a2:	f812 3b01 	ldrb.w	r3, [r2], #1
   357a6:	617a      	str	r2, [r7, #20]
   357a8:	63fb      	str	r3, [r7, #60]	; 0x3c
   357aa:	2b00      	cmp	r3, #0
   357ac:	f000 80d6 	beq.w	3595c <__ssvfscanf_r+0x1f8>
   357b0:	4a85      	ldr	r2, [pc, #532]	; (359c8 <__ssvfscanf_r+0x264>)
   357b2:	f813 8002 	ldrb.w	r8, [r3, r2]
   357b6:	f018 0808 	ands.w	r8, r8, #8
   357ba:	d019      	beq.n	357f0 <__ssvfscanf_r+0x8c>
   357bc:	6863      	ldr	r3, [r4, #4]
   357be:	2b00      	cmp	r3, #0
   357c0:	dd0e      	ble.n	357e0 <__ssvfscanf_r+0x7c>
   357c2:	6823      	ldr	r3, [r4, #0]
   357c4:	4980      	ldr	r1, [pc, #512]	; (359c8 <__ssvfscanf_r+0x264>)
   357c6:	781a      	ldrb	r2, [r3, #0]
   357c8:	5c8a      	ldrb	r2, [r1, r2]
   357ca:	0710      	lsls	r0, r2, #28
   357cc:	d5e8      	bpl.n	357a0 <__ssvfscanf_r+0x3c>
   357ce:	6b3a      	ldr	r2, [r7, #48]	; 0x30
   357d0:	3301      	adds	r3, #1
   357d2:	3201      	adds	r2, #1
   357d4:	6023      	str	r3, [r4, #0]
   357d6:	633a      	str	r2, [r7, #48]	; 0x30
   357d8:	6862      	ldr	r2, [r4, #4]
   357da:	3a01      	subs	r2, #1
   357dc:	6062      	str	r2, [r4, #4]
   357de:	e7ed      	b.n	357bc <__ssvfscanf_r+0x58>
   357e0:	4621      	mov	r1, r4
   357e2:	ee18 0a10 	vmov	r0, s16
   357e6:	f007 fa36 	bl	3cc56 <__ssrefill_r>
   357ea:	2800      	cmp	r0, #0
   357ec:	d0e9      	beq.n	357c2 <__ssvfscanf_r+0x5e>
   357ee:	e7d7      	b.n	357a0 <__ssvfscanf_r+0x3c>
   357f0:	2b25      	cmp	r3, #37	; 0x25
   357f2:	d165      	bne.n	358c0 <__ssvfscanf_r+0x15c>
   357f4:	697b      	ldr	r3, [r7, #20]
   357f6:	46c1      	mov	r9, r8
   357f8:	220a      	movs	r2, #10
   357fa:	f8c7 802c 	str.w	r8, [r7, #44]	; 0x2c
   357fe:	6979      	ldr	r1, [r7, #20]
   35800:	781d      	ldrb	r5, [r3, #0]
   35802:	3101      	adds	r1, #1
   35804:	6179      	str	r1, [r7, #20]
   35806:	f1a5 0125 	sub.w	r1, r5, #37	; 0x25
   3580a:	2955      	cmp	r1, #85	; 0x55
   3580c:	f200 816e 	bhi.w	35aec <__ssvfscanf_r+0x388>
   35810:	e8df f011 	tbh	[pc, r1, lsl #1]
   35814:	016c0056 	.word	0x016c0056
   35818:	016c016c 	.word	0x016c016c
   3581c:	007d016c 	.word	0x007d016c
   35820:	016c016c 	.word	0x016c016c
   35824:	016c016c 	.word	0x016c016c
   35828:	00ba016c 	.word	0x00ba016c
   3582c:	00ba00ba 	.word	0x00ba00ba
   35830:	00ba00ba 	.word	0x00ba00ba
   35834:	00ba00ba 	.word	0x00ba00ba
   35838:	00ba00ba 	.word	0x00ba00ba
   3583c:	016c00ba 	.word	0x016c00ba
   35840:	016c016c 	.word	0x016c016c
   35844:	016c016c 	.word	0x016c016c
   35848:	016c016c 	.word	0x016c016c
   3584c:	016c00e0 	.word	0x016c00e0
   35850:	00c40104 	.word	0x00c40104
   35854:	00e000e0 	.word	0x00e000e0
   35858:	016c00e0 	.word	0x016c00e0
   3585c:	016c016c 	.word	0x016c016c
   35860:	00a1016c 	.word	0x00a1016c
   35864:	016c016c 	.word	0x016c016c
   35868:	016c00c8 	.word	0x016c00c8
   3586c:	016c016c 	.word	0x016c016c
   35870:	016c00f6 	.word	0x016c00f6
   35874:	016c016c 	.word	0x016c016c
   35878:	00d4016c 	.word	0x00d4016c
   3587c:	016c016c 	.word	0x016c016c
   35880:	016c00fa 	.word	0x016c00fa
   35884:	016c016c 	.word	0x016c016c
   35888:	016c016c 	.word	0x016c016c
   3588c:	016c00e0 	.word	0x016c00e0
   35890:	00c60106 	.word	0x00c60106
   35894:	00e000e0 	.word	0x00e000e0
   35898:	009400e0 	.word	0x009400e0
   3589c:	00a1012e 	.word	0x00a1012e
   358a0:	0086016c 	.word	0x0086016c
   358a4:	010d00ad 	.word	0x010d00ad
   358a8:	010a00ca 	.word	0x010a00ca
   358ac:	016c016c 	.word	0x016c016c
   358b0:	00a900f8 	.word	0x00a900f8
   358b4:	016c00d0 	.word	0x016c00d0
   358b8:	00d4016c 	.word	0x00d4016c
   358bc:	00a9016c 	.word	0x00a9016c
   358c0:	6863      	ldr	r3, [r4, #4]
   358c2:	2b00      	cmp	r3, #0
   358c4:	dd0f      	ble.n	358e6 <__ssvfscanf_r+0x182>
   358c6:	6823      	ldr	r3, [r4, #0]
   358c8:	697a      	ldr	r2, [r7, #20]
   358ca:	7819      	ldrb	r1, [r3, #0]
   358cc:	f812 2c01 	ldrb.w	r2, [r2, #-1]
   358d0:	4291      	cmp	r1, r2
   358d2:	d143      	bne.n	3595c <__ssvfscanf_r+0x1f8>
   358d4:	3301      	adds	r3, #1
   358d6:	6862      	ldr	r2, [r4, #4]
   358d8:	6023      	str	r3, [r4, #0]
   358da:	3a01      	subs	r2, #1
   358dc:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   358de:	6062      	str	r2, [r4, #4]
   358e0:	3301      	adds	r3, #1
   358e2:	633b      	str	r3, [r7, #48]	; 0x30
   358e4:	e75c      	b.n	357a0 <__ssvfscanf_r+0x3c>
   358e6:	4621      	mov	r1, r4
   358e8:	ee18 0a10 	vmov	r0, s16
   358ec:	f007 f9b3 	bl	3cc56 <__ssrefill_r>
   358f0:	2800      	cmp	r0, #0
   358f2:	d0e8      	beq.n	358c6 <__ssvfscanf_r+0x162>
   358f4:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   358f6:	b113      	cbz	r3, 358fe <__ssvfscanf_r+0x19a>
   358f8:	89a3      	ldrh	r3, [r4, #12]
   358fa:	065b      	lsls	r3, r3, #25
   358fc:	d52e      	bpl.n	3595c <__ssvfscanf_r+0x1f8>
   358fe:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35900:	2b00      	cmp	r3, #0
   35902:	f040 80f8 	bne.w	35af6 <__ssvfscanf_r+0x392>
   35906:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3590a:	627b      	str	r3, [r7, #36]	; 0x24
   3590c:	e103      	b.n	35b16 <__ssvfscanf_r+0x3b2>
   3590e:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   35910:	ea53 0309 	orrs.w	r3, r3, r9
   35914:	62fb      	str	r3, [r7, #44]	; 0x2c
   35916:	d121      	bne.n	3595c <__ssvfscanf_r+0x1f8>
   35918:	f04f 0910 	mov.w	r9, #16
   3591c:	697b      	ldr	r3, [r7, #20]
   3591e:	e76e      	b.n	357fe <__ssvfscanf_r+0x9a>
   35920:	f019 0f0f 	tst.w	r9, #15
   35924:	d11a      	bne.n	3595c <__ssvfscanf_r+0x1f8>
   35926:	7859      	ldrb	r1, [r3, #1]
   35928:	296c      	cmp	r1, #108	; 0x6c
   3592a:	d104      	bne.n	35936 <__ssvfscanf_r+0x1d2>
   3592c:	3302      	adds	r3, #2
   3592e:	617b      	str	r3, [r7, #20]
   35930:	f049 0902 	orr.w	r9, r9, #2
   35934:	e7f2      	b.n	3591c <__ssvfscanf_r+0x1b8>
   35936:	f049 0901 	orr.w	r9, r9, #1
   3593a:	e7ef      	b.n	3591c <__ssvfscanf_r+0x1b8>
   3593c:	f019 0f0f 	tst.w	r9, #15
   35940:	d10c      	bne.n	3595c <__ssvfscanf_r+0x1f8>
   35942:	7859      	ldrb	r1, [r3, #1]
   35944:	2968      	cmp	r1, #104	; 0x68
   35946:	bf05      	ittet	eq
   35948:	3302      	addeq	r3, #2
   3594a:	f049 0908 	orreq.w	r9, r9, #8
   3594e:	f049 0904 	orrne.w	r9, r9, #4
   35952:	617b      	streq	r3, [r7, #20]
   35954:	e7e2      	b.n	3591c <__ssvfscanf_r+0x1b8>
   35956:	f019 0f0f 	tst.w	r9, #15
   3595a:	d0e9      	beq.n	35930 <__ssvfscanf_r+0x1cc>
   3595c:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   3595e:	2b00      	cmp	r3, #0
   35960:	f040 80d4 	bne.w	35b0c <__ssvfscanf_r+0x3a8>
   35964:	e0d7      	b.n	35b16 <__ssvfscanf_r+0x3b2>
   35966:	f019 0f0f 	tst.w	r9, #15
   3596a:	d0d7      	beq.n	3591c <__ssvfscanf_r+0x1b8>
   3596c:	e7f6      	b.n	3595c <__ssvfscanf_r+0x1f8>
   3596e:	f019 038f 	ands.w	r3, r9, #143	; 0x8f
   35972:	d1f3      	bne.n	3595c <__ssvfscanf_r+0x1f8>
   35974:	6b79      	ldr	r1, [r7, #52]	; 0x34
   35976:	b921      	cbnz	r1, 35982 <__ssvfscanf_r+0x21e>
   35978:	b082      	sub	sp, #8
   3597a:	a902      	add	r1, sp, #8
   3597c:	6379      	str	r1, [r7, #52]	; 0x34
   3597e:	e9c1 3300 	strd	r3, r3, [r1]
   35982:	f049 0980 	orr.w	r9, r9, #128	; 0x80
   35986:	e7c9      	b.n	3591c <__ssvfscanf_r+0x1b8>
   35988:	f019 0f8f 	tst.w	r9, #143	; 0x8f
   3598c:	d1e6      	bne.n	3595c <__ssvfscanf_r+0x1f8>
   3598e:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   35990:	fb02 5503 	mla	r5, r2, r3, r5
   35994:	f1a5 0330 	sub.w	r3, r5, #48	; 0x30
   35998:	62fb      	str	r3, [r7, #44]	; 0x2c
   3599a:	e7bf      	b.n	3591c <__ssvfscanf_r+0x1b8>
   3599c:	f049 0901 	orr.w	r9, r9, #1
   359a0:	4b0a      	ldr	r3, [pc, #40]	; (359cc <__ssvfscanf_r+0x268>)
   359a2:	e008      	b.n	359b6 <__ssvfscanf_r+0x252>
   359a4:	f049 0901 	orr.w	r9, r9, #1
   359a8:	4b09      	ldr	r3, [pc, #36]	; (359d0 <__ssvfscanf_r+0x26c>)
   359aa:	61bb      	str	r3, [r7, #24]
   359ac:	2308      	movs	r3, #8
   359ae:	2503      	movs	r5, #3
   359b0:	61fb      	str	r3, [r7, #28]
   359b2:	e010      	b.n	359d6 <__ssvfscanf_r+0x272>
   359b4:	4b06      	ldr	r3, [pc, #24]	; (359d0 <__ssvfscanf_r+0x26c>)
   359b6:	61bb      	str	r3, [r7, #24]
   359b8:	230a      	movs	r3, #10
   359ba:	e7f8      	b.n	359ae <__ssvfscanf_r+0x24a>
   359bc:	f449 7900 	orr.w	r9, r9, #512	; 0x200
   359c0:	4b03      	ldr	r3, [pc, #12]	; (359d0 <__ssvfscanf_r+0x26c>)
   359c2:	61bb      	str	r3, [r7, #24]
   359c4:	2310      	movs	r3, #16
   359c6:	e7f2      	b.n	359ae <__ssvfscanf_r+0x24a>
   359c8:	0004056b 	.word	0x0004056b
   359cc:	00034381 	.word	0x00034381
   359d0:	0003c215 	.word	0x0003c215
   359d4:	2504      	movs	r5, #4
   359d6:	6863      	ldr	r3, [r4, #4]
   359d8:	2b00      	cmp	r3, #0
   359da:	dd4d      	ble.n	35a78 <__ssvfscanf_r+0x314>
   359dc:	f019 0f40 	tst.w	r9, #64	; 0x40
   359e0:	d105      	bne.n	359ee <__ssvfscanf_r+0x28a>
   359e2:	6823      	ldr	r3, [r4, #0]
   359e4:	49a9      	ldr	r1, [pc, #676]	; (35c8c <__ssvfscanf_r+0x528>)
   359e6:	781a      	ldrb	r2, [r3, #0]
   359e8:	5c52      	ldrb	r2, [r2, r1]
   359ea:	0711      	lsls	r1, r2, #28
   359ec:	d44c      	bmi.n	35a88 <__ssvfscanf_r+0x324>
   359ee:	3d01      	subs	r5, #1
   359f0:	2d03      	cmp	r5, #3
   359f2:	d85c      	bhi.n	35aae <__ssvfscanf_r+0x34a>
   359f4:	e8df f015 	tbh	[pc, r5, lsl #1]
   359f8:	039201d4 	.word	0x039201d4
   359fc:	062a0538 	.word	0x062a0538
   35a00:	f049 0901 	orr.w	r9, r9, #1
   35a04:	2502      	movs	r5, #2
   35a06:	e7e6      	b.n	359d6 <__ssvfscanf_r+0x272>
   35a08:	6979      	ldr	r1, [r7, #20]
   35a0a:	f107 0050 	add.w	r0, r7, #80	; 0x50
   35a0e:	f005 f9eb 	bl	3ade8 <__sccl>
   35a12:	f049 0940 	orr.w	r9, r9, #64	; 0x40
   35a16:	2501      	movs	r5, #1
   35a18:	6178      	str	r0, [r7, #20]
   35a1a:	e7dc      	b.n	359d6 <__ssvfscanf_r+0x272>
   35a1c:	f049 0901 	orr.w	r9, r9, #1
   35a20:	f049 0940 	orr.w	r9, r9, #64	; 0x40
   35a24:	2500      	movs	r5, #0
   35a26:	e7d6      	b.n	359d6 <__ssvfscanf_r+0x272>
   35a28:	f449 7908 	orr.w	r9, r9, #544	; 0x220
   35a2c:	e7c8      	b.n	359c0 <__ssvfscanf_r+0x25c>
   35a2e:	f019 0f10 	tst.w	r9, #16
   35a32:	f47f aeb5 	bne.w	357a0 <__ssvfscanf_r+0x3c>
   35a36:	f019 0f08 	tst.w	r9, #8
   35a3a:	f856 3b04 	ldr.w	r3, [r6], #4
   35a3e:	d002      	beq.n	35a46 <__ssvfscanf_r+0x2e2>
   35a40:	6b3a      	ldr	r2, [r7, #48]	; 0x30
   35a42:	701a      	strb	r2, [r3, #0]
   35a44:	e6ac      	b.n	357a0 <__ssvfscanf_r+0x3c>
   35a46:	f019 0f04 	tst.w	r9, #4
   35a4a:	d002      	beq.n	35a52 <__ssvfscanf_r+0x2ee>
   35a4c:	6b3a      	ldr	r2, [r7, #48]	; 0x30
   35a4e:	801a      	strh	r2, [r3, #0]
   35a50:	e6a6      	b.n	357a0 <__ssvfscanf_r+0x3c>
   35a52:	f019 0f01 	tst.w	r9, #1
   35a56:	d002      	beq.n	35a5e <__ssvfscanf_r+0x2fa>
   35a58:	6b3a      	ldr	r2, [r7, #48]	; 0x30
   35a5a:	601a      	str	r2, [r3, #0]
   35a5c:	e6a0      	b.n	357a0 <__ssvfscanf_r+0x3c>
   35a5e:	f019 0f02 	tst.w	r9, #2
   35a62:	d0f9      	beq.n	35a58 <__ssvfscanf_r+0x2f4>
   35a64:	6b3a      	ldr	r2, [r7, #48]	; 0x30
   35a66:	6b39      	ldr	r1, [r7, #48]	; 0x30
   35a68:	17d2      	asrs	r2, r2, #31
   35a6a:	e9c3 1200 	strd	r1, r2, [r3]
   35a6e:	e697      	b.n	357a0 <__ssvfscanf_r+0x3c>
   35a70:	4b87      	ldr	r3, [pc, #540]	; (35c90 <__ssvfscanf_r+0x52c>)
   35a72:	61bb      	str	r3, [r7, #24]
   35a74:	2300      	movs	r3, #0
   35a76:	e79a      	b.n	359ae <__ssvfscanf_r+0x24a>
   35a78:	4621      	mov	r1, r4
   35a7a:	ee18 0a10 	vmov	r0, s16
   35a7e:	f007 f8ea 	bl	3cc56 <__ssrefill_r>
   35a82:	2800      	cmp	r0, #0
   35a84:	d0aa      	beq.n	359dc <__ssvfscanf_r+0x278>
   35a86:	e735      	b.n	358f4 <__ssvfscanf_r+0x190>
   35a88:	6b3a      	ldr	r2, [r7, #48]	; 0x30
   35a8a:	3201      	adds	r2, #1
   35a8c:	633a      	str	r2, [r7, #48]	; 0x30
   35a8e:	6862      	ldr	r2, [r4, #4]
   35a90:	3a01      	subs	r2, #1
   35a92:	2a00      	cmp	r2, #0
   35a94:	6062      	str	r2, [r4, #4]
   35a96:	dd02      	ble.n	35a9e <__ssvfscanf_r+0x33a>
   35a98:	3301      	adds	r3, #1
   35a9a:	6023      	str	r3, [r4, #0]
   35a9c:	e7a1      	b.n	359e2 <__ssvfscanf_r+0x27e>
   35a9e:	4621      	mov	r1, r4
   35aa0:	ee18 0a10 	vmov	r0, s16
   35aa4:	f007 f8d7 	bl	3cc56 <__ssrefill_r>
   35aa8:	2800      	cmp	r0, #0
   35aaa:	d09a      	beq.n	359e2 <__ssvfscanf_r+0x27e>
   35aac:	e722      	b.n	358f4 <__ssvfscanf_r+0x190>
   35aae:	6afd      	ldr	r5, [r7, #44]	; 0x2c
   35ab0:	f009 0b10 	and.w	fp, r9, #16
   35ab4:	2d01      	cmp	r5, #1
   35ab6:	bf38      	it	cc
   35ab8:	2501      	movcc	r5, #1
   35aba:	f019 0801 	ands.w	r8, r9, #1
   35abe:	f000 80f7 	beq.w	35cb0 <__ssvfscanf_r+0x54c>
   35ac2:	f1bb 0f00 	cmp.w	fp, #0
   35ac6:	f040 80b5 	bne.w	35c34 <__ssvfscanf_r+0x4d0>
   35aca:	4633      	mov	r3, r6
   35acc:	f019 0f80 	tst.w	r9, #128	; 0x80
   35ad0:	f853 6b04 	ldr.w	r6, [r3], #4
   35ad4:	62fb      	str	r3, [r7, #44]	; 0x2c
   35ad6:	f000 80b4 	beq.w	35c42 <__ssvfscanf_r+0x4de>
   35ada:	b13e      	cbz	r6, 35aec <__ssvfscanf_r+0x388>
   35adc:	2080      	movs	r0, #128	; 0x80
   35ade:	f003 fee7 	bl	398b0 <malloc>
   35ae2:	4680      	mov	r8, r0
   35ae4:	b9f8      	cbnz	r0, 35b26 <__ssvfscanf_r+0x3c2>
   35ae6:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   35aea:	627b      	str	r3, [r7, #36]	; 0x24
   35aec:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35aee:	b193      	cbz	r3, 35b16 <__ssvfscanf_r+0x3b2>
   35af0:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   35af2:	3301      	adds	r3, #1
   35af4:	d10a      	bne.n	35b0c <__ssvfscanf_r+0x3a8>
   35af6:	2400      	movs	r4, #0
   35af8:	4626      	mov	r6, r4
   35afa:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35afc:	681d      	ldr	r5, [r3, #0]
   35afe:	88db      	ldrh	r3, [r3, #6]
   35b00:	42a3      	cmp	r3, r4
   35b02:	f300 87b0 	bgt.w	36a66 <__ssvfscanf_r+0x1302>
   35b06:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   35b0a:	627b      	str	r3, [r7, #36]	; 0x24
   35b0c:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35b0e:	6818      	ldr	r0, [r3, #0]
   35b10:	b108      	cbz	r0, 35b16 <__ssvfscanf_r+0x3b2>
   35b12:	f003 fb69 	bl	391e8 <free>
   35b16:	6a78      	ldr	r0, [r7, #36]	; 0x24
   35b18:	f507 772d 	add.w	r7, r7, #692	; 0x2b4
   35b1c:	46bd      	mov	sp, r7
   35b1e:	ecbd 8b02 	vpop	{d8}
   35b22:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   35b26:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35b28:	6030      	str	r0, [r6, #0]
   35b2a:	f8b3 9006 	ldrh.w	r9, [r3, #6]
   35b2e:	889b      	ldrh	r3, [r3, #4]
   35b30:	4599      	cmp	r9, r3
   35b32:	d312      	bcc.n	35b5a <__ssvfscanf_r+0x3f6>
   35b34:	f64f 72f6 	movw	r2, #65526	; 0xfff6
   35b38:	4293      	cmp	r3, r2
   35b3a:	d8d4      	bhi.n	35ae6 <__ssvfscanf_r+0x382>
   35b3c:	3308      	adds	r3, #8
   35b3e:	fa1f fa83 	uxth.w	sl, r3
   35b42:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35b44:	ea4f 018a 	mov.w	r1, sl, lsl #2
   35b48:	6818      	ldr	r0, [r3, #0]
   35b4a:	f004 ff39 	bl	3a9c0 <realloc>
   35b4e:	2800      	cmp	r0, #0
   35b50:	d0c9      	beq.n	35ae6 <__ssvfscanf_r+0x382>
   35b52:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35b54:	6018      	str	r0, [r3, #0]
   35b56:	f8a3 a004 	strh.w	sl, [r3, #4]
   35b5a:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35b5c:	f109 0201 	add.w	r2, r9, #1
   35b60:	6b79      	ldr	r1, [r7, #52]	; 0x34
   35b62:	46b2      	mov	sl, r6
   35b64:	681b      	ldr	r3, [r3, #0]
   35b66:	80ca      	strh	r2, [r1, #6]
   35b68:	f843 6029 	str.w	r6, [r3, r9, lsl #2]
   35b6c:	4646      	mov	r6, r8
   35b6e:	f04f 0920 	mov.w	r9, #32
   35b72:	2200      	movs	r2, #0
   35b74:	62ba      	str	r2, [r7, #40]	; 0x28
   35b76:	f003 fe03 	bl	39780 <__locale_mb_cur_max>
   35b7a:	6aba      	ldr	r2, [r7, #40]	; 0x28
   35b7c:	4290      	cmp	r0, r2
   35b7e:	f43f aeb9 	beq.w	358f4 <__ssvfscanf_r+0x190>
   35b82:	6821      	ldr	r1, [r4, #0]
   35b84:	f507 7ca8 	add.w	ip, r7, #336	; 0x150
   35b88:	1c53      	adds	r3, r2, #1
   35b8a:	f811 0b01 	ldrb.w	r0, [r1], #1
   35b8e:	f80c 0002 	strb.w	r0, [ip, r2]
   35b92:	6862      	ldr	r2, [r4, #4]
   35b94:	3a01      	subs	r2, #1
   35b96:	e9c4 1200 	strd	r1, r2, [r4]
   35b9a:	6a3a      	ldr	r2, [r7, #32]
   35b9c:	2a03      	cmp	r2, #3
   35b9e:	d102      	bne.n	35ba6 <__ssvfscanf_r+0x442>
   35ba0:	6cba      	ldr	r2, [r7, #72]	; 0x48
   35ba2:	2a04      	cmp	r2, #4
   35ba4:	d007      	beq.n	35bb6 <__ssvfscanf_r+0x452>
   35ba6:	2208      	movs	r2, #8
   35ba8:	2100      	movs	r1, #0
   35baa:	f107 0048 	add.w	r0, r7, #72	; 0x48
   35bae:	62bb      	str	r3, [r7, #40]	; 0x28
   35bb0:	f004 fa1e 	bl	39ff0 <memset>
   35bb4:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35bb6:	f107 0248 	add.w	r2, r7, #72	; 0x48
   35bba:	4631      	mov	r1, r6
   35bbc:	ee18 0a10 	vmov	r0, s16
   35bc0:	62bb      	str	r3, [r7, #40]	; 0x28
   35bc2:	9200      	str	r2, [sp, #0]
   35bc4:	f507 72a8 	add.w	r2, r7, #336	; 0x150
   35bc8:	f004 f8aa 	bl	39d20 <_mbrtowc_r>
   35bcc:	1c42      	adds	r2, r0, #1
   35bce:	6238      	str	r0, [r7, #32]
   35bd0:	f43f ae90 	beq.w	358f4 <__ssvfscanf_r+0x190>
   35bd4:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35bd6:	2800      	cmp	r0, #0
   35bd8:	d137      	bne.n	35c4a <__ssvfscanf_r+0x4e6>
   35bda:	f1bb 0f00 	cmp.w	fp, #0
   35bde:	d101      	bne.n	35be4 <__ssvfscanf_r+0x480>
   35be0:	f8c6 b000 	str.w	fp, [r6]
   35be4:	6b3a      	ldr	r2, [r7, #48]	; 0x30
   35be6:	441a      	add	r2, r3
   35be8:	6a3b      	ldr	r3, [r7, #32]
   35bea:	2b03      	cmp	r3, #3
   35bec:	633a      	str	r2, [r7, #48]	; 0x30
   35bee:	d102      	bne.n	35bf6 <__ssvfscanf_r+0x492>
   35bf0:	6cbb      	ldr	r3, [r7, #72]	; 0x48
   35bf2:	2b04      	cmp	r3, #4
   35bf4:	d000      	beq.n	35bf8 <__ssvfscanf_r+0x494>
   35bf6:	3d01      	subs	r5, #1
   35bf8:	f1bb 0f00 	cmp.w	fp, #0
   35bfc:	d118      	bne.n	35c30 <__ssvfscanf_r+0x4cc>
   35bfe:	f1ba 0f00 	cmp.w	sl, #0
   35c02:	d014      	beq.n	35c2e <__ssvfscanf_r+0x4ca>
   35c04:	eba6 0308 	sub.w	r3, r6, r8
   35c08:	ebb9 0fa3 	cmp.w	r9, r3, asr #2
   35c0c:	d80f      	bhi.n	35c2e <__ssvfscanf_r+0x4ca>
   35c0e:	4640      	mov	r0, r8
   35c10:	ea4f 01c9 	mov.w	r1, r9, lsl #3
   35c14:	62bb      	str	r3, [r7, #40]	; 0x28
   35c16:	f004 fed3 	bl	3a9c0 <realloc>
   35c1a:	4680      	mov	r8, r0
   35c1c:	2800      	cmp	r0, #0
   35c1e:	f43f af62 	beq.w	35ae6 <__ssvfscanf_r+0x382>
   35c22:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35c24:	ea4f 0949 	mov.w	r9, r9, lsl #1
   35c28:	f8ca 0000 	str.w	r0, [sl]
   35c2c:	18c6      	adds	r6, r0, r3
   35c2e:	3604      	adds	r6, #4
   35c30:	2300      	movs	r3, #0
   35c32:	e00d      	b.n	35c50 <__ssvfscanf_r+0x4ec>
   35c34:	f04f 0900 	mov.w	r9, #0
   35c38:	62fe      	str	r6, [r7, #44]	; 0x2c
   35c3a:	46c8      	mov	r8, r9
   35c3c:	46ca      	mov	sl, r9
   35c3e:	464e      	mov	r6, r9
   35c40:	e797      	b.n	35b72 <__ssvfscanf_r+0x40e>
   35c42:	46d9      	mov	r9, fp
   35c44:	46d8      	mov	r8, fp
   35c46:	46da      	mov	sl, fp
   35c48:	e793      	b.n	35b72 <__ssvfscanf_r+0x40e>
   35c4a:	6a3a      	ldr	r2, [r7, #32]
   35c4c:	3202      	adds	r2, #2
   35c4e:	d1c9      	bne.n	35be4 <__ssvfscanf_r+0x480>
   35c50:	6862      	ldr	r2, [r4, #4]
   35c52:	2a00      	cmp	r2, #0
   35c54:	dc15      	bgt.n	35c82 <__ssvfscanf_r+0x51e>
   35c56:	4621      	mov	r1, r4
   35c58:	ee18 0a10 	vmov	r0, s16
   35c5c:	62bb      	str	r3, [r7, #40]	; 0x28
   35c5e:	f006 fffa 	bl	3cc56 <__ssrefill_r>
   35c62:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35c64:	b168      	cbz	r0, 35c82 <__ssvfscanf_r+0x51e>
   35c66:	2b00      	cmp	r3, #0
   35c68:	f47f ae44 	bne.w	358f4 <__ssvfscanf_r+0x190>
   35c6c:	f1ba 0f00 	cmp.w	sl, #0
   35c70:	d110      	bne.n	35c94 <__ssvfscanf_r+0x530>
   35c72:	f1bb 0f00 	cmp.w	fp, #0
   35c76:	d102      	bne.n	35c7e <__ssvfscanf_r+0x51a>
   35c78:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   35c7a:	3301      	adds	r3, #1
   35c7c:	627b      	str	r3, [r7, #36]	; 0x24
   35c7e:	6afe      	ldr	r6, [r7, #44]	; 0x2c
   35c80:	e58e      	b.n	357a0 <__ssvfscanf_r+0x3c>
   35c82:	2d00      	cmp	r5, #0
   35c84:	d0f2      	beq.n	35c6c <__ssvfscanf_r+0x508>
   35c86:	461a      	mov	r2, r3
   35c88:	e774      	b.n	35b74 <__ssvfscanf_r+0x410>
   35c8a:	bf00      	nop
   35c8c:	0004056b 	.word	0x0004056b
   35c90:	00034381 	.word	0x00034381
   35c94:	eba6 0108 	sub.w	r1, r6, r8
   35c98:	ebb9 0fa1 	cmp.w	r9, r1, asr #2
   35c9c:	d9e9      	bls.n	35c72 <__ssvfscanf_r+0x50e>
   35c9e:	f8da 0000 	ldr.w	r0, [sl]
   35ca2:	f004 fe8d 	bl	3a9c0 <realloc>
   35ca6:	2800      	cmp	r0, #0
   35ca8:	d0e3      	beq.n	35c72 <__ssvfscanf_r+0x50e>
   35caa:	f8ca 0000 	str.w	r0, [sl]
   35cae:	e7e0      	b.n	35c72 <__ssvfscanf_r+0x50e>
   35cb0:	f1bb 0f00 	cmp.w	fp, #0
   35cb4:	d01b      	beq.n	35cee <__ssvfscanf_r+0x58a>
   35cb6:	e9d4 2300 	ldrd	r2, r3, [r4]
   35cba:	429d      	cmp	r5, r3
   35cbc:	dd11      	ble.n	35ce2 <__ssvfscanf_r+0x57e>
   35cbe:	4498      	add	r8, r3
   35cc0:	1aed      	subs	r5, r5, r3
   35cc2:	4413      	add	r3, r2
   35cc4:	4621      	mov	r1, r4
   35cc6:	ee18 0a10 	vmov	r0, s16
   35cca:	6023      	str	r3, [r4, #0]
   35ccc:	f006 ffc3 	bl	3cc56 <__ssrefill_r>
   35cd0:	2800      	cmp	r0, #0
   35cd2:	d0f0      	beq.n	35cb6 <__ssvfscanf_r+0x552>
   35cd4:	f1b8 0f00 	cmp.w	r8, #0
   35cd8:	f43f ae0c 	beq.w	358f4 <__ssvfscanf_r+0x190>
   35cdc:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   35cde:	4443      	add	r3, r8
   35ce0:	e5ff      	b.n	358e2 <__ssvfscanf_r+0x17e>
   35ce2:	1b5b      	subs	r3, r3, r5
   35ce4:	442a      	add	r2, r5
   35ce6:	44a8      	add	r8, r5
   35ce8:	6063      	str	r3, [r4, #4]
   35cea:	6022      	str	r2, [r4, #0]
   35cec:	e7f6      	b.n	35cdc <__ssvfscanf_r+0x578>
   35cee:	46b0      	mov	r8, r6
   35cf0:	f019 0980 	ands.w	r9, r9, #128	; 0x80
   35cf4:	f858 6b04 	ldr.w	r6, [r8], #4
   35cf8:	d031      	beq.n	35d5e <__ssvfscanf_r+0x5fa>
   35cfa:	2e00      	cmp	r6, #0
   35cfc:	f43f ae2e 	beq.w	3595c <__ssvfscanf_r+0x1f8>
   35d00:	4628      	mov	r0, r5
   35d02:	f003 fdd5 	bl	398b0 <malloc>
   35d06:	4682      	mov	sl, r0
   35d08:	2800      	cmp	r0, #0
   35d0a:	f43f adf8 	beq.w	358fe <__ssvfscanf_r+0x19a>
   35d0e:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35d10:	6030      	str	r0, [r6, #0]
   35d12:	f8b3 b006 	ldrh.w	fp, [r3, #6]
   35d16:	f8b3 9004 	ldrh.w	r9, [r3, #4]
   35d1a:	45cb      	cmp	fp, r9
   35d1c:	d315      	bcc.n	35d4a <__ssvfscanf_r+0x5e6>
   35d1e:	f64f 73f6 	movw	r3, #65526	; 0xfff6
   35d22:	4599      	cmp	r9, r3
   35d24:	f63f aee7 	bhi.w	35af6 <__ssvfscanf_r+0x392>
   35d28:	f109 0908 	add.w	r9, r9, #8
   35d2c:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35d2e:	fa1f f989 	uxth.w	r9, r9
   35d32:	6818      	ldr	r0, [r3, #0]
   35d34:	ea4f 0189 	mov.w	r1, r9, lsl #2
   35d38:	f004 fe42 	bl	3a9c0 <realloc>
   35d3c:	2800      	cmp	r0, #0
   35d3e:	f43f aeda 	beq.w	35af6 <__ssvfscanf_r+0x392>
   35d42:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35d44:	6018      	str	r0, [r3, #0]
   35d46:	f8a3 9004 	strh.w	r9, [r3, #4]
   35d4a:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35d4c:	f10b 0201 	add.w	r2, fp, #1
   35d50:	6b79      	ldr	r1, [r7, #52]	; 0x34
   35d52:	46b1      	mov	r9, r6
   35d54:	681b      	ldr	r3, [r3, #0]
   35d56:	80ca      	strh	r2, [r1, #6]
   35d58:	f843 602b 	str.w	r6, [r3, fp, lsl #2]
   35d5c:	4656      	mov	r6, sl
   35d5e:	4631      	mov	r1, r6
   35d60:	462b      	mov	r3, r5
   35d62:	2201      	movs	r2, #1
   35d64:	ee18 0a10 	vmov	r0, s16
   35d68:	9400      	str	r4, [sp, #0]
   35d6a:	f006 ff91 	bl	3cc90 <_sfread_r>
   35d6e:	4606      	mov	r6, r0
   35d70:	2800      	cmp	r0, #0
   35d72:	f43f adbf 	beq.w	358f4 <__ssvfscanf_r+0x190>
   35d76:	f1b9 0f00 	cmp.w	r9, #0
   35d7a:	d009      	beq.n	35d90 <__ssvfscanf_r+0x62c>
   35d7c:	4285      	cmp	r5, r0
   35d7e:	d907      	bls.n	35d90 <__ssvfscanf_r+0x62c>
   35d80:	4601      	mov	r1, r0
   35d82:	f8d9 0000 	ldr.w	r0, [r9]
   35d86:	f004 fe1b 	bl	3a9c0 <realloc>
   35d8a:	b108      	cbz	r0, 35d90 <__ssvfscanf_r+0x62c>
   35d8c:	f8c9 0000 	str.w	r0, [r9]
   35d90:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   35d92:	4433      	add	r3, r6
   35d94:	4646      	mov	r6, r8
   35d96:	633b      	str	r3, [r7, #48]	; 0x30
   35d98:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   35d9a:	3301      	adds	r3, #1
   35d9c:	627b      	str	r3, [r7, #36]	; 0x24
   35d9e:	e4ff      	b.n	357a0 <__ssvfscanf_r+0x3c>
   35da0:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   35da2:	f009 0b10 	and.w	fp, r9, #16
   35da6:	2b00      	cmp	r3, #0
   35da8:	bf08      	it	eq
   35daa:	f04f 33ff 	moveq.w	r3, #4294967295	; 0xffffffff
   35dae:	f019 0501 	ands.w	r5, r9, #1
   35db2:	62fb      	str	r3, [r7, #44]	; 0x2c
   35db4:	f000 8107 	beq.w	35fc6 <__ssvfscanf_r+0x862>
   35db8:	f1bb 0f00 	cmp.w	fp, #0
   35dbc:	f040 80a7 	bne.w	35f0e <__ssvfscanf_r+0x7aa>
   35dc0:	4633      	mov	r3, r6
   35dc2:	f019 0f80 	tst.w	r9, #128	; 0x80
   35dc6:	f853 5b04 	ldr.w	r5, [r3], #4
   35dca:	613b      	str	r3, [r7, #16]
   35dcc:	f000 80a8 	beq.w	35f20 <__ssvfscanf_r+0x7bc>
   35dd0:	2d00      	cmp	r5, #0
   35dd2:	f43f ae8b 	beq.w	35aec <__ssvfscanf_r+0x388>
   35dd6:	2080      	movs	r0, #128	; 0x80
   35dd8:	f003 fd6a 	bl	398b0 <malloc>
   35ddc:	4680      	mov	r8, r0
   35dde:	2800      	cmp	r0, #0
   35de0:	f43f ae81 	beq.w	35ae6 <__ssvfscanf_r+0x382>
   35de4:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35de6:	6028      	str	r0, [r5, #0]
   35de8:	88de      	ldrh	r6, [r3, #6]
   35dea:	889b      	ldrh	r3, [r3, #4]
   35dec:	429e      	cmp	r6, r3
   35dee:	d314      	bcc.n	35e1a <__ssvfscanf_r+0x6b6>
   35df0:	f64f 72f6 	movw	r2, #65526	; 0xfff6
   35df4:	4293      	cmp	r3, r2
   35df6:	f63f ae76 	bhi.w	35ae6 <__ssvfscanf_r+0x382>
   35dfa:	3308      	adds	r3, #8
   35dfc:	fa1f f983 	uxth.w	r9, r3
   35e00:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35e02:	ea4f 0189 	mov.w	r1, r9, lsl #2
   35e06:	6818      	ldr	r0, [r3, #0]
   35e08:	f004 fdda 	bl	3a9c0 <realloc>
   35e0c:	2800      	cmp	r0, #0
   35e0e:	f43f ae6a 	beq.w	35ae6 <__ssvfscanf_r+0x382>
   35e12:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35e14:	6018      	str	r0, [r3, #0]
   35e16:	f8a3 9004 	strh.w	r9, [r3, #4]
   35e1a:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   35e1c:	1c72      	adds	r2, r6, #1
   35e1e:	6b79      	ldr	r1, [r7, #52]	; 0x34
   35e20:	f04f 0a20 	mov.w	sl, #32
   35e24:	681b      	ldr	r3, [r3, #0]
   35e26:	62bd      	str	r5, [r7, #40]	; 0x28
   35e28:	f843 5026 	str.w	r5, [r3, r6, lsl #2]
   35e2c:	4645      	mov	r5, r8
   35e2e:	80ca      	strh	r2, [r1, #6]
   35e30:	2600      	movs	r6, #0
   35e32:	f003 fca5 	bl	39780 <__locale_mb_cur_max>
   35e36:	42b0      	cmp	r0, r6
   35e38:	f43f ad5c 	beq.w	358f4 <__ssvfscanf_r+0x190>
   35e3c:	6823      	ldr	r3, [r4, #0]
   35e3e:	f507 71a8 	add.w	r1, r7, #336	; 0x150
   35e42:	f106 0901 	add.w	r9, r6, #1
   35e46:	f813 2b01 	ldrb.w	r2, [r3], #1
   35e4a:	558a      	strb	r2, [r1, r6]
   35e4c:	6862      	ldr	r2, [r4, #4]
   35e4e:	3a01      	subs	r2, #1
   35e50:	e9c4 3200 	strd	r3, r2, [r4]
   35e54:	6a3b      	ldr	r3, [r7, #32]
   35e56:	2b03      	cmp	r3, #3
   35e58:	d102      	bne.n	35e60 <__ssvfscanf_r+0x6fc>
   35e5a:	6c3b      	ldr	r3, [r7, #64]	; 0x40
   35e5c:	2b04      	cmp	r3, #4
   35e5e:	d005      	beq.n	35e6c <__ssvfscanf_r+0x708>
   35e60:	2208      	movs	r2, #8
   35e62:	2100      	movs	r1, #0
   35e64:	f107 0040 	add.w	r0, r7, #64	; 0x40
   35e68:	f004 f8c2 	bl	39ff0 <memset>
   35e6c:	f107 0340 	add.w	r3, r7, #64	; 0x40
   35e70:	f507 72a8 	add.w	r2, r7, #336	; 0x150
   35e74:	4629      	mov	r1, r5
   35e76:	ee18 0a10 	vmov	r0, s16
   35e7a:	9300      	str	r3, [sp, #0]
   35e7c:	464b      	mov	r3, r9
   35e7e:	f003 ff4f 	bl	39d20 <_mbrtowc_r>
   35e82:	4603      	mov	r3, r0
   35e84:	6238      	str	r0, [r7, #32]
   35e86:	1c58      	adds	r0, r3, #1
   35e88:	f43f ad34 	beq.w	358f4 <__ssvfscanf_r+0x190>
   35e8c:	2b00      	cmp	r3, #0
   35e8e:	d14c      	bne.n	35f2a <__ssvfscanf_r+0x7c6>
   35e90:	602b      	str	r3, [r5, #0]
   35e92:	682e      	ldr	r6, [r5, #0]
   35e94:	1c71      	adds	r1, r6, #1
   35e96:	d059      	beq.n	35f4c <__ssvfscanf_r+0x7e8>
   35e98:	2208      	movs	r2, #8
   35e9a:	2100      	movs	r1, #0
   35e9c:	f107 0048 	add.w	r0, r7, #72	; 0x48
   35ea0:	f004 f8a6 	bl	39ff0 <memset>
   35ea4:	4b9c      	ldr	r3, [pc, #624]	; (36118 <__ssvfscanf_r+0x9b4>)
   35ea6:	4632      	mov	r2, r6
   35ea8:	f107 0138 	add.w	r1, r7, #56	; 0x38
   35eac:	f8d3 30e0 	ldr.w	r3, [r3, #224]	; 0xe0
   35eb0:	ee18 0a10 	vmov	r0, s16
   35eb4:	60fb      	str	r3, [r7, #12]
   35eb6:	f107 0348 	add.w	r3, r7, #72	; 0x48
   35eba:	68fe      	ldr	r6, [r7, #12]
   35ebc:	47b0      	blx	r6
   35ebe:	2801      	cmp	r0, #1
   35ec0:	d146      	bne.n	35f50 <__ssvfscanf_r+0x7ec>
   35ec2:	f897 2038 	ldrb.w	r2, [r7, #56]	; 0x38
   35ec6:	f107 0350 	add.w	r3, r7, #80	; 0x50
   35eca:	5c9b      	ldrb	r3, [r3, r2]
   35ecc:	2b00      	cmp	r3, #0
   35ece:	d14b      	bne.n	35f68 <__ssvfscanf_r+0x804>
   35ed0:	f507 76a8 	add.w	r6, r7, #336	; 0x150
   35ed4:	f1b9 0f00 	cmp.w	r9, #0
   35ed8:	d13c      	bne.n	35f54 <__ssvfscanf_r+0x7f0>
   35eda:	f1bb 0f00 	cmp.w	fp, #0
   35ede:	d114      	bne.n	35f0a <__ssvfscanf_r+0x7a6>
   35ee0:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35ee2:	f8c5 b000 	str.w	fp, [r5]
   35ee6:	b16b      	cbz	r3, 35f04 <__ssvfscanf_r+0x7a0>
   35ee8:	eba5 0108 	sub.w	r1, r5, r8
   35eec:	108b      	asrs	r3, r1, #2
   35eee:	3301      	adds	r3, #1
   35ef0:	459a      	cmp	sl, r3
   35ef2:	d907      	bls.n	35f04 <__ssvfscanf_r+0x7a0>
   35ef4:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35ef6:	3104      	adds	r1, #4
   35ef8:	6818      	ldr	r0, [r3, #0]
   35efa:	f004 fd61 	bl	3a9c0 <realloc>
   35efe:	b108      	cbz	r0, 35f04 <__ssvfscanf_r+0x7a0>
   35f00:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35f02:	6018      	str	r0, [r3, #0]
   35f04:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   35f06:	3301      	adds	r3, #1
   35f08:	627b      	str	r3, [r7, #36]	; 0x24
   35f0a:	693e      	ldr	r6, [r7, #16]
   35f0c:	e448      	b.n	357a0 <__ssvfscanf_r+0x3c>
   35f0e:	f04f 0a00 	mov.w	sl, #0
   35f12:	f107 053c 	add.w	r5, r7, #60	; 0x3c
   35f16:	613e      	str	r6, [r7, #16]
   35f18:	46d0      	mov	r8, sl
   35f1a:	f8c7 a028 	str.w	sl, [r7, #40]	; 0x28
   35f1e:	e787      	b.n	35e30 <__ssvfscanf_r+0x6cc>
   35f20:	46da      	mov	sl, fp
   35f22:	46d8      	mov	r8, fp
   35f24:	f8c7 b028 	str.w	fp, [r7, #40]	; 0x28
   35f28:	e782      	b.n	35e30 <__ssvfscanf_r+0x6cc>
   35f2a:	6a3b      	ldr	r3, [r7, #32]
   35f2c:	3302      	adds	r3, #2
   35f2e:	d1b0      	bne.n	35e92 <__ssvfscanf_r+0x72e>
   35f30:	6863      	ldr	r3, [r4, #4]
   35f32:	2b00      	cmp	r3, #0
   35f34:	dc42      	bgt.n	35fbc <__ssvfscanf_r+0x858>
   35f36:	4621      	mov	r1, r4
   35f38:	ee18 0a10 	vmov	r0, s16
   35f3c:	f006 fe8b 	bl	3cc56 <__ssrefill_r>
   35f40:	2800      	cmp	r0, #0
   35f42:	d03b      	beq.n	35fbc <__ssvfscanf_r+0x858>
   35f44:	f1b9 0f00 	cmp.w	r9, #0
   35f48:	d0c7      	beq.n	35eda <__ssvfscanf_r+0x776>
   35f4a:	e4d3      	b.n	358f4 <__ssvfscanf_r+0x190>
   35f4c:	4632      	mov	r2, r6
   35f4e:	e7ba      	b.n	35ec6 <__ssvfscanf_r+0x762>
   35f50:	2200      	movs	r2, #0
   35f52:	e7b8      	b.n	35ec6 <__ssvfscanf_r+0x762>
   35f54:	f109 39ff 	add.w	r9, r9, #4294967295	; 0xffffffff
   35f58:	4622      	mov	r2, r4
   35f5a:	ee18 0a10 	vmov	r0, s16
   35f5e:	f816 1009 	ldrb.w	r1, [r6, r9]
   35f62:	f006 fe3b 	bl	3cbdc <_sungetc_r>
   35f66:	e7b5      	b.n	35ed4 <__ssvfscanf_r+0x770>
   35f68:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   35f6a:	444b      	add	r3, r9
   35f6c:	633b      	str	r3, [r7, #48]	; 0x30
   35f6e:	6a3b      	ldr	r3, [r7, #32]
   35f70:	2b03      	cmp	r3, #3
   35f72:	d102      	bne.n	35f7a <__ssvfscanf_r+0x816>
   35f74:	6c3b      	ldr	r3, [r7, #64]	; 0x40
   35f76:	2b04      	cmp	r3, #4
   35f78:	d002      	beq.n	35f80 <__ssvfscanf_r+0x81c>
   35f7a:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   35f7c:	3b01      	subs	r3, #1
   35f7e:	62fb      	str	r3, [r7, #44]	; 0x2c
   35f80:	f1bb 0f00 	cmp.w	fp, #0
   35f84:	d117      	bne.n	35fb6 <__ssvfscanf_r+0x852>
   35f86:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35f88:	3504      	adds	r5, #4
   35f8a:	b1a3      	cbz	r3, 35fb6 <__ssvfscanf_r+0x852>
   35f8c:	eba5 0608 	sub.w	r6, r5, r8
   35f90:	ebba 0fa6 	cmp.w	sl, r6, asr #2
   35f94:	d80d      	bhi.n	35fb2 <__ssvfscanf_r+0x84e>
   35f96:	4640      	mov	r0, r8
   35f98:	ea4f 01ca 	mov.w	r1, sl, lsl #3
   35f9c:	f004 fd10 	bl	3a9c0 <realloc>
   35fa0:	4680      	mov	r8, r0
   35fa2:	2800      	cmp	r0, #0
   35fa4:	f43f ad9f 	beq.w	35ae6 <__ssvfscanf_r+0x382>
   35fa8:	6abb      	ldr	r3, [r7, #40]	; 0x28
   35faa:	1985      	adds	r5, r0, r6
   35fac:	ea4f 0a4a 	mov.w	sl, sl, lsl #1
   35fb0:	6018      	str	r0, [r3, #0]
   35fb2:	46d9      	mov	r9, fp
   35fb4:	e7bc      	b.n	35f30 <__ssvfscanf_r+0x7cc>
   35fb6:	f04f 0900 	mov.w	r9, #0
   35fba:	e7b9      	b.n	35f30 <__ssvfscanf_r+0x7cc>
   35fbc:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   35fbe:	2b00      	cmp	r3, #0
   35fc0:	d08b      	beq.n	35eda <__ssvfscanf_r+0x776>
   35fc2:	464e      	mov	r6, r9
   35fc4:	e735      	b.n	35e32 <__ssvfscanf_r+0x6ce>
   35fc6:	f1bb 0f00 	cmp.w	fp, #0
   35fca:	d01d      	beq.n	36008 <__ssvfscanf_r+0x8a4>
   35fcc:	f107 0850 	add.w	r8, r7, #80	; 0x50
   35fd0:	6823      	ldr	r3, [r4, #0]
   35fd2:	781a      	ldrb	r2, [r3, #0]
   35fd4:	f818 2002 	ldrb.w	r2, [r8, r2]
   35fd8:	b90a      	cbnz	r2, 35fde <__ssvfscanf_r+0x87a>
   35fda:	b995      	cbnz	r5, 36002 <__ssvfscanf_r+0x89e>
   35fdc:	e4be      	b.n	3595c <__ssvfscanf_r+0x1f8>
   35fde:	3301      	adds	r3, #1
   35fe0:	6862      	ldr	r2, [r4, #4]
   35fe2:	3501      	adds	r5, #1
   35fe4:	6023      	str	r3, [r4, #0]
   35fe6:	3a01      	subs	r2, #1
   35fe8:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   35fea:	6062      	str	r2, [r4, #4]
   35fec:	429d      	cmp	r5, r3
   35fee:	d008      	beq.n	36002 <__ssvfscanf_r+0x89e>
   35ff0:	2a00      	cmp	r2, #0
   35ff2:	dced      	bgt.n	35fd0 <__ssvfscanf_r+0x86c>
   35ff4:	4621      	mov	r1, r4
   35ff6:	ee18 0a10 	vmov	r0, s16
   35ffa:	f006 fe2c 	bl	3cc56 <__ssrefill_r>
   35ffe:	2800      	cmp	r0, #0
   36000:	d0e6      	beq.n	35fd0 <__ssvfscanf_r+0x86c>
   36002:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   36004:	442b      	add	r3, r5
   36006:	e46c      	b.n	358e2 <__ssvfscanf_r+0x17e>
   36008:	46b3      	mov	fp, r6
   3600a:	f019 0980 	ands.w	r9, r9, #128	; 0x80
   3600e:	f85b 6b04 	ldr.w	r6, [fp], #4
   36012:	d07f      	beq.n	36114 <__ssvfscanf_r+0x9b0>
   36014:	2e00      	cmp	r6, #0
   36016:	f43f aca1 	beq.w	3595c <__ssvfscanf_r+0x1f8>
   3601a:	2020      	movs	r0, #32
   3601c:	f003 fc48 	bl	398b0 <malloc>
   36020:	4680      	mov	r8, r0
   36022:	2800      	cmp	r0, #0
   36024:	f43f ac6b 	beq.w	358fe <__ssvfscanf_r+0x19a>
   36028:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   3602a:	6030      	str	r0, [r6, #0]
   3602c:	f8b3 9006 	ldrh.w	r9, [r3, #6]
   36030:	889d      	ldrh	r5, [r3, #4]
   36032:	45a9      	cmp	r9, r5
   36034:	d311      	bcc.n	3605a <__ssvfscanf_r+0x8f6>
   36036:	f64f 73f6 	movw	r3, #65526	; 0xfff6
   3603a:	429d      	cmp	r5, r3
   3603c:	f63f ad5b 	bhi.w	35af6 <__ssvfscanf_r+0x392>
   36040:	3508      	adds	r5, #8
   36042:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   36044:	b2ad      	uxth	r5, r5
   36046:	6818      	ldr	r0, [r3, #0]
   36048:	00a9      	lsls	r1, r5, #2
   3604a:	f004 fcb9 	bl	3a9c0 <realloc>
   3604e:	2800      	cmp	r0, #0
   36050:	f43f ad51 	beq.w	35af6 <__ssvfscanf_r+0x392>
   36054:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   36056:	6018      	str	r0, [r3, #0]
   36058:	809d      	strh	r5, [r3, #4]
   3605a:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   3605c:	f109 0201 	add.w	r2, r9, #1
   36060:	6b79      	ldr	r1, [r7, #52]	; 0x34
   36062:	46b2      	mov	sl, r6
   36064:	681b      	ldr	r3, [r3, #0]
   36066:	80ca      	strh	r2, [r1, #6]
   36068:	f843 6029 	str.w	r6, [r3, r9, lsl #2]
   3606c:	4646      	mov	r6, r8
   3606e:	f04f 0920 	mov.w	r9, #32
   36072:	4635      	mov	r5, r6
   36074:	f107 0850 	add.w	r8, r7, #80	; 0x50
   36078:	6823      	ldr	r3, [r4, #0]
   3607a:	781a      	ldrb	r2, [r3, #0]
   3607c:	f818 2002 	ldrb.w	r2, [r8, r2]
   36080:	b36a      	cbz	r2, 360de <__ssvfscanf_r+0x97a>
   36082:	6862      	ldr	r2, [r4, #4]
   36084:	3a01      	subs	r2, #1
   36086:	6062      	str	r2, [r4, #4]
   36088:	1c5a      	adds	r2, r3, #1
   3608a:	6022      	str	r2, [r4, #0]
   3608c:	781b      	ldrb	r3, [r3, #0]
   3608e:	f806 3b01 	strb.w	r3, [r6], #1
   36092:	f1ba 0f00 	cmp.w	sl, #0
   36096:	d011      	beq.n	360bc <__ssvfscanf_r+0x958>
   36098:	1b73      	subs	r3, r6, r5
   3609a:	454b      	cmp	r3, r9
   3609c:	d30e      	bcc.n	360bc <__ssvfscanf_r+0x958>
   3609e:	ea4f 0949 	mov.w	r9, r9, lsl #1
   360a2:	4628      	mov	r0, r5
   360a4:	62bb      	str	r3, [r7, #40]	; 0x28
   360a6:	4649      	mov	r1, r9
   360a8:	f004 fc8a 	bl	3a9c0 <realloc>
   360ac:	4605      	mov	r5, r0
   360ae:	2800      	cmp	r0, #0
   360b0:	f43f ac25 	beq.w	358fe <__ssvfscanf_r+0x19a>
   360b4:	6abb      	ldr	r3, [r7, #40]	; 0x28
   360b6:	f8ca 0000 	str.w	r0, [sl]
   360ba:	18c6      	adds	r6, r0, r3
   360bc:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   360be:	3b01      	subs	r3, #1
   360c0:	62fb      	str	r3, [r7, #44]	; 0x2c
   360c2:	d00c      	beq.n	360de <__ssvfscanf_r+0x97a>
   360c4:	6863      	ldr	r3, [r4, #4]
   360c6:	2b00      	cmp	r3, #0
   360c8:	dcd6      	bgt.n	36078 <__ssvfscanf_r+0x914>
   360ca:	4621      	mov	r1, r4
   360cc:	ee18 0a10 	vmov	r0, s16
   360d0:	f006 fdc1 	bl	3cc56 <__ssrefill_r>
   360d4:	2800      	cmp	r0, #0
   360d6:	d0cf      	beq.n	36078 <__ssvfscanf_r+0x914>
   360d8:	42ae      	cmp	r6, r5
   360da:	f43f ac0b 	beq.w	358f4 <__ssvfscanf_r+0x190>
   360de:	1b75      	subs	r5, r6, r5
   360e0:	f43f ac3c 	beq.w	3595c <__ssvfscanf_r+0x1f8>
   360e4:	2300      	movs	r3, #0
   360e6:	7033      	strb	r3, [r6, #0]
   360e8:	f1ba 0f00 	cmp.w	sl, #0
   360ec:	d009      	beq.n	36102 <__ssvfscanf_r+0x99e>
   360ee:	1c69      	adds	r1, r5, #1
   360f0:	4589      	cmp	r9, r1
   360f2:	d906      	bls.n	36102 <__ssvfscanf_r+0x99e>
   360f4:	f8da 0000 	ldr.w	r0, [sl]
   360f8:	f004 fc62 	bl	3a9c0 <realloc>
   360fc:	b108      	cbz	r0, 36102 <__ssvfscanf_r+0x99e>
   360fe:	f8ca 0000 	str.w	r0, [sl]
   36102:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   36104:	3301      	adds	r3, #1
   36106:	627b      	str	r3, [r7, #36]	; 0x24
   36108:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   3610a:	442b      	add	r3, r5
   3610c:	633b      	str	r3, [r7, #48]	; 0x30
   3610e:	465e      	mov	r6, fp
   36110:	f7ff bb46 	b.w	357a0 <__ssvfscanf_r+0x3c>
   36114:	46ca      	mov	sl, r9
   36116:	e7ac      	b.n	36072 <__ssvfscanf_r+0x90e>
   36118:	20006430 	.word	0x20006430
   3611c:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3611e:	f009 0810 	and.w	r8, r9, #16
   36122:	2b00      	cmp	r3, #0
   36124:	bf08      	it	eq
   36126:	f04f 33ff 	moveq.w	r3, #4294967295	; 0xffffffff
   3612a:	f019 0501 	ands.w	r5, r9, #1
   3612e:	62fb      	str	r3, [r7, #44]	; 0x2c
   36130:	f000 80fb 	beq.w	3632a <__ssvfscanf_r+0xbc6>
   36134:	f1b8 0f00 	cmp.w	r8, #0
   36138:	f040 80a0 	bne.w	3627c <__ssvfscanf_r+0xb18>
   3613c:	4633      	mov	r3, r6
   3613e:	f019 0f80 	tst.w	r9, #128	; 0x80
   36142:	f853 6b04 	ldr.w	r6, [r3], #4
   36146:	62bb      	str	r3, [r7, #40]	; 0x28
   36148:	f000 80a0 	beq.w	3628c <__ssvfscanf_r+0xb28>
   3614c:	2e00      	cmp	r6, #0
   3614e:	f43f accd 	beq.w	35aec <__ssvfscanf_r+0x388>
   36152:	2080      	movs	r0, #128	; 0x80
   36154:	f003 fbac 	bl	398b0 <malloc>
   36158:	4605      	mov	r5, r0
   3615a:	2800      	cmp	r0, #0
   3615c:	f43f acc3 	beq.w	35ae6 <__ssvfscanf_r+0x382>
   36160:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   36162:	6030      	str	r0, [r6, #0]
   36164:	f8b3 9006 	ldrh.w	r9, [r3, #6]
   36168:	889b      	ldrh	r3, [r3, #4]
   3616a:	4599      	cmp	r9, r3
   3616c:	d314      	bcc.n	36198 <__ssvfscanf_r+0xa34>
   3616e:	f64f 72f6 	movw	r2, #65526	; 0xfff6
   36172:	4293      	cmp	r3, r2
   36174:	f63f acb7 	bhi.w	35ae6 <__ssvfscanf_r+0x382>
   36178:	3308      	adds	r3, #8
   3617a:	fa1f fa83 	uxth.w	sl, r3
   3617e:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   36180:	ea4f 018a 	mov.w	r1, sl, lsl #2
   36184:	6818      	ldr	r0, [r3, #0]
   36186:	f004 fc1b 	bl	3a9c0 <realloc>
   3618a:	2800      	cmp	r0, #0
   3618c:	f43f acab 	beq.w	35ae6 <__ssvfscanf_r+0x382>
   36190:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   36192:	6018      	str	r0, [r3, #0]
   36194:	f8a3 a004 	strh.w	sl, [r3, #4]
   36198:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   3619a:	f109 0201 	add.w	r2, r9, #1
   3619e:	6b79      	ldr	r1, [r7, #52]	; 0x34
   361a0:	46b2      	mov	sl, r6
   361a2:	681b      	ldr	r3, [r3, #0]
   361a4:	80ca      	strh	r2, [r1, #6]
   361a6:	f843 6029 	str.w	r6, [r3, r9, lsl #2]
   361aa:	462e      	mov	r6, r5
   361ac:	f04f 0920 	mov.w	r9, #32
   361b0:	2200      	movs	r2, #0
   361b2:	f107 0b48 	add.w	fp, r7, #72	; 0x48
   361b6:	6823      	ldr	r3, [r4, #0]
   361b8:	49aa      	ldr	r1, [pc, #680]	; (36464 <__ssvfscanf_r+0xd00>)
   361ba:	781b      	ldrb	r3, [r3, #0]
   361bc:	5c5b      	ldrb	r3, [r3, r1]
   361be:	0718      	lsls	r0, r3, #28
   361c0:	d441      	bmi.n	36246 <__ssvfscanf_r+0xae2>
   361c2:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   361c4:	2b00      	cmp	r3, #0
   361c6:	d03e      	beq.n	36246 <__ssvfscanf_r+0xae2>
   361c8:	613a      	str	r2, [r7, #16]
   361ca:	f003 fad9 	bl	39780 <__locale_mb_cur_max>
   361ce:	693a      	ldr	r2, [r7, #16]
   361d0:	4290      	cmp	r0, r2
   361d2:	f43f ab8f 	beq.w	358f4 <__ssvfscanf_r+0x190>
   361d6:	6821      	ldr	r1, [r4, #0]
   361d8:	f507 7ca8 	add.w	ip, r7, #336	; 0x150
   361dc:	1c53      	adds	r3, r2, #1
   361de:	f811 0b01 	ldrb.w	r0, [r1], #1
   361e2:	f80c 0002 	strb.w	r0, [ip, r2]
   361e6:	6862      	ldr	r2, [r4, #4]
   361e8:	3a01      	subs	r2, #1
   361ea:	e9c4 1200 	strd	r1, r2, [r4]
   361ee:	6a3a      	ldr	r2, [r7, #32]
   361f0:	2a03      	cmp	r2, #3
   361f2:	d103      	bne.n	361fc <__ssvfscanf_r+0xa98>
   361f4:	f8db 2000 	ldr.w	r2, [fp]
   361f8:	2a04      	cmp	r2, #4
   361fa:	d006      	beq.n	3620a <__ssvfscanf_r+0xaa6>
   361fc:	2208      	movs	r2, #8
   361fe:	2100      	movs	r1, #0
   36200:	4658      	mov	r0, fp
   36202:	623b      	str	r3, [r7, #32]
   36204:	f003 fef4 	bl	39ff0 <memset>
   36208:	6a3b      	ldr	r3, [r7, #32]
   3620a:	f507 72a8 	add.w	r2, r7, #336	; 0x150
   3620e:	4631      	mov	r1, r6
   36210:	ee18 0a10 	vmov	r0, s16
   36214:	f8cd b000 	str.w	fp, [sp]
   36218:	613b      	str	r3, [r7, #16]
   3621a:	f003 fd81 	bl	39d20 <_mbrtowc_r>
   3621e:	1c42      	adds	r2, r0, #1
   36220:	6238      	str	r0, [r7, #32]
   36222:	f43f ab67 	beq.w	358f4 <__ssvfscanf_r+0x190>
   36226:	693b      	ldr	r3, [r7, #16]
   36228:	2800      	cmp	r0, #0
   3622a:	d133      	bne.n	36294 <__ssvfscanf_r+0xb30>
   3622c:	6030      	str	r0, [r6, #0]
   3622e:	6830      	ldr	r0, [r6, #0]
   36230:	613b      	str	r3, [r7, #16]
   36232:	f003 fa77 	bl	39724 <iswspace>
   36236:	693b      	ldr	r3, [r7, #16]
   36238:	4602      	mov	r2, r0
   3623a:	2800      	cmp	r0, #0
   3623c:	d049      	beq.n	362d2 <__ssvfscanf_r+0xb6e>
   3623e:	f507 7ba8 	add.w	fp, r7, #336	; 0x150
   36242:	2b00      	cmp	r3, #0
   36244:	d13a      	bne.n	362bc <__ssvfscanf_r+0xb58>
   36246:	f1b8 0f00 	cmp.w	r8, #0
   3624a:	d114      	bne.n	36276 <__ssvfscanf_r+0xb12>
   3624c:	f8c6 8000 	str.w	r8, [r6]
   36250:	f1ba 0f00 	cmp.w	sl, #0
   36254:	d00c      	beq.n	36270 <__ssvfscanf_r+0xb0c>
   36256:	1b71      	subs	r1, r6, r5
   36258:	108b      	asrs	r3, r1, #2
   3625a:	3301      	adds	r3, #1
   3625c:	4599      	cmp	r9, r3
   3625e:	d907      	bls.n	36270 <__ssvfscanf_r+0xb0c>
   36260:	3104      	adds	r1, #4
   36262:	f8da 0000 	ldr.w	r0, [sl]
   36266:	f004 fbab 	bl	3a9c0 <realloc>
   3626a:	b108      	cbz	r0, 36270 <__ssvfscanf_r+0xb0c>
   3626c:	f8ca 0000 	str.w	r0, [sl]
   36270:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   36272:	3301      	adds	r3, #1
   36274:	627b      	str	r3, [r7, #36]	; 0x24
   36276:	6abe      	ldr	r6, [r7, #40]	; 0x28
   36278:	f7ff ba92 	b.w	357a0 <__ssvfscanf_r+0x3c>
   3627c:	f04f 0900 	mov.w	r9, #0
   36280:	62be      	str	r6, [r7, #40]	; 0x28
   36282:	f107 063c 	add.w	r6, r7, #60	; 0x3c
   36286:	464d      	mov	r5, r9
   36288:	46ca      	mov	sl, r9
   3628a:	e791      	b.n	361b0 <__ssvfscanf_r+0xa4c>
   3628c:	46c1      	mov	r9, r8
   3628e:	4645      	mov	r5, r8
   36290:	46c2      	mov	sl, r8
   36292:	e78d      	b.n	361b0 <__ssvfscanf_r+0xa4c>
   36294:	6a3a      	ldr	r2, [r7, #32]
   36296:	3202      	adds	r2, #2
   36298:	d1c9      	bne.n	3622e <__ssvfscanf_r+0xaca>
   3629a:	461a      	mov	r2, r3
   3629c:	6863      	ldr	r3, [r4, #4]
   3629e:	2b00      	cmp	r3, #0
   362a0:	dc89      	bgt.n	361b6 <__ssvfscanf_r+0xa52>
   362a2:	4621      	mov	r1, r4
   362a4:	ee18 0a10 	vmov	r0, s16
   362a8:	613a      	str	r2, [r7, #16]
   362aa:	f006 fcd4 	bl	3cc56 <__ssrefill_r>
   362ae:	693a      	ldr	r2, [r7, #16]
   362b0:	2800      	cmp	r0, #0
   362b2:	d080      	beq.n	361b6 <__ssvfscanf_r+0xa52>
   362b4:	2a00      	cmp	r2, #0
   362b6:	f47f ab1d 	bne.w	358f4 <__ssvfscanf_r+0x190>
   362ba:	e7c4      	b.n	36246 <__ssvfscanf_r+0xae2>
   362bc:	3b01      	subs	r3, #1
   362be:	4622      	mov	r2, r4
   362c0:	ee18 0a10 	vmov	r0, s16
   362c4:	f81b 1003 	ldrb.w	r1, [fp, r3]
   362c8:	62fb      	str	r3, [r7, #44]	; 0x2c
   362ca:	f006 fc87 	bl	3cbdc <_sungetc_r>
   362ce:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   362d0:	e7b7      	b.n	36242 <__ssvfscanf_r+0xade>
   362d2:	6b39      	ldr	r1, [r7, #48]	; 0x30
   362d4:	4419      	add	r1, r3
   362d6:	6a3b      	ldr	r3, [r7, #32]
   362d8:	2b03      	cmp	r3, #3
   362da:	6339      	str	r1, [r7, #48]	; 0x30
   362dc:	d103      	bne.n	362e6 <__ssvfscanf_r+0xb82>
   362de:	f8db 3000 	ldr.w	r3, [fp]
   362e2:	2b04      	cmp	r3, #4
   362e4:	d002      	beq.n	362ec <__ssvfscanf_r+0xb88>
   362e6:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   362e8:	3b01      	subs	r3, #1
   362ea:	62fb      	str	r3, [r7, #44]	; 0x2c
   362ec:	f1b8 0f00 	cmp.w	r8, #0
   362f0:	d1d4      	bne.n	3629c <__ssvfscanf_r+0xb38>
   362f2:	3604      	adds	r6, #4
   362f4:	f1ba 0f00 	cmp.w	sl, #0
   362f8:	d015      	beq.n	36326 <__ssvfscanf_r+0xbc2>
   362fa:	1b73      	subs	r3, r6, r5
   362fc:	ebb9 0fa3 	cmp.w	r9, r3, asr #2
   36300:	d80f      	bhi.n	36322 <__ssvfscanf_r+0xbbe>
   36302:	4628      	mov	r0, r5
   36304:	ea4f 01c9 	mov.w	r1, r9, lsl #3
   36308:	613b      	str	r3, [r7, #16]
   3630a:	f004 fb59 	bl	3a9c0 <realloc>
   3630e:	4605      	mov	r5, r0
   36310:	2800      	cmp	r0, #0
   36312:	f43f abe8 	beq.w	35ae6 <__ssvfscanf_r+0x382>
   36316:	693b      	ldr	r3, [r7, #16]
   36318:	ea4f 0949 	mov.w	r9, r9, lsl #1
   3631c:	f8ca 0000 	str.w	r0, [sl]
   36320:	18c6      	adds	r6, r0, r3
   36322:	4642      	mov	r2, r8
   36324:	e7ba      	b.n	3629c <__ssvfscanf_r+0xb38>
   36326:	4652      	mov	r2, sl
   36328:	e7b8      	b.n	3629c <__ssvfscanf_r+0xb38>
   3632a:	f1b8 0f00 	cmp.w	r8, #0
   3632e:	d01a      	beq.n	36366 <__ssvfscanf_r+0xc02>
   36330:	6823      	ldr	r3, [r4, #0]
   36332:	494c      	ldr	r1, [pc, #304]	; (36464 <__ssvfscanf_r+0xd00>)
   36334:	781a      	ldrb	r2, [r3, #0]
   36336:	5c52      	ldrb	r2, [r2, r1]
   36338:	0711      	lsls	r1, r2, #28
   3633a:	f53f ae62 	bmi.w	36002 <__ssvfscanf_r+0x89e>
   3633e:	3301      	adds	r3, #1
   36340:	6862      	ldr	r2, [r4, #4]
   36342:	3501      	adds	r5, #1
   36344:	6023      	str	r3, [r4, #0]
   36346:	3a01      	subs	r2, #1
   36348:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3634a:	6062      	str	r2, [r4, #4]
   3634c:	429d      	cmp	r5, r3
   3634e:	f43f ae58 	beq.w	36002 <__ssvfscanf_r+0x89e>
   36352:	2a00      	cmp	r2, #0
   36354:	dcec      	bgt.n	36330 <__ssvfscanf_r+0xbcc>
   36356:	4621      	mov	r1, r4
   36358:	ee18 0a10 	vmov	r0, s16
   3635c:	f006 fc7b 	bl	3cc56 <__ssrefill_r>
   36360:	2800      	cmp	r0, #0
   36362:	d0e5      	beq.n	36330 <__ssvfscanf_r+0xbcc>
   36364:	e64d      	b.n	36002 <__ssvfscanf_r+0x89e>
   36366:	46b3      	mov	fp, r6
   36368:	f019 0580 	ands.w	r5, r9, #128	; 0x80
   3636c:	f85b 6b04 	ldr.w	r6, [fp], #4
   36370:	d076      	beq.n	36460 <__ssvfscanf_r+0xcfc>
   36372:	2e00      	cmp	r6, #0
   36374:	f43f aaf2 	beq.w	3595c <__ssvfscanf_r+0x1f8>
   36378:	2020      	movs	r0, #32
   3637a:	f003 fa99 	bl	398b0 <malloc>
   3637e:	4680      	mov	r8, r0
   36380:	2800      	cmp	r0, #0
   36382:	f43f aabc 	beq.w	358fe <__ssvfscanf_r+0x19a>
   36386:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   36388:	6030      	str	r0, [r6, #0]
   3638a:	f8b3 9006 	ldrh.w	r9, [r3, #6]
   3638e:	889d      	ldrh	r5, [r3, #4]
   36390:	45a9      	cmp	r9, r5
   36392:	d311      	bcc.n	363b8 <__ssvfscanf_r+0xc54>
   36394:	f64f 73f6 	movw	r3, #65526	; 0xfff6
   36398:	429d      	cmp	r5, r3
   3639a:	f63f abac 	bhi.w	35af6 <__ssvfscanf_r+0x392>
   3639e:	3508      	adds	r5, #8
   363a0:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   363a2:	b2ad      	uxth	r5, r5
   363a4:	6818      	ldr	r0, [r3, #0]
   363a6:	00a9      	lsls	r1, r5, #2
   363a8:	f004 fb0a 	bl	3a9c0 <realloc>
   363ac:	2800      	cmp	r0, #0
   363ae:	f43f aba2 	beq.w	35af6 <__ssvfscanf_r+0x392>
   363b2:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   363b4:	6018      	str	r0, [r3, #0]
   363b6:	809d      	strh	r5, [r3, #4]
   363b8:	6b7b      	ldr	r3, [r7, #52]	; 0x34
   363ba:	f109 0201 	add.w	r2, r9, #1
   363be:	6b79      	ldr	r1, [r7, #52]	; 0x34
   363c0:	2520      	movs	r5, #32
   363c2:	681b      	ldr	r3, [r3, #0]
   363c4:	80ca      	strh	r2, [r1, #6]
   363c6:	f843 6029 	str.w	r6, [r3, r9, lsl #2]
   363ca:	46b1      	mov	r9, r6
   363cc:	4646      	mov	r6, r8
   363ce:	46b0      	mov	r8, r6
   363d0:	6823      	ldr	r3, [r4, #0]
   363d2:	4924      	ldr	r1, [pc, #144]	; (36464 <__ssvfscanf_r+0xd00>)
   363d4:	781a      	ldrb	r2, [r3, #0]
   363d6:	5c52      	ldrb	r2, [r2, r1]
   363d8:	0712      	lsls	r2, r2, #28
   363da:	d429      	bmi.n	36430 <__ssvfscanf_r+0xccc>
   363dc:	6862      	ldr	r2, [r4, #4]
   363de:	3a01      	subs	r2, #1
   363e0:	6062      	str	r2, [r4, #4]
   363e2:	1c5a      	adds	r2, r3, #1
   363e4:	6022      	str	r2, [r4, #0]
   363e6:	781b      	ldrb	r3, [r3, #0]
   363e8:	f806 3b01 	strb.w	r3, [r6], #1
   363ec:	f1b9 0f00 	cmp.w	r9, #0
   363f0:	d010      	beq.n	36414 <__ssvfscanf_r+0xcb0>
   363f2:	eba6 0a08 	sub.w	sl, r6, r8
   363f6:	45aa      	cmp	sl, r5
   363f8:	d30c      	bcc.n	36414 <__ssvfscanf_r+0xcb0>
   363fa:	006d      	lsls	r5, r5, #1
   363fc:	4640      	mov	r0, r8
   363fe:	4629      	mov	r1, r5
   36400:	f004 fade 	bl	3a9c0 <realloc>
   36404:	4680      	mov	r8, r0
   36406:	2800      	cmp	r0, #0
   36408:	f43f aa79 	beq.w	358fe <__ssvfscanf_r+0x19a>
   3640c:	eb00 060a 	add.w	r6, r0, sl
   36410:	f8c9 0000 	str.w	r0, [r9]
   36414:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   36416:	3b01      	subs	r3, #1
   36418:	62fb      	str	r3, [r7, #44]	; 0x2c
   3641a:	d009      	beq.n	36430 <__ssvfscanf_r+0xccc>
   3641c:	6863      	ldr	r3, [r4, #4]
   3641e:	2b00      	cmp	r3, #0
   36420:	dcd6      	bgt.n	363d0 <__ssvfscanf_r+0xc6c>
   36422:	4621      	mov	r1, r4
   36424:	ee18 0a10 	vmov	r0, s16
   36428:	f006 fc15 	bl	3cc56 <__ssrefill_r>
   3642c:	2800      	cmp	r0, #0
   3642e:	d0cf      	beq.n	363d0 <__ssvfscanf_r+0xc6c>
   36430:	2300      	movs	r3, #0
   36432:	7033      	strb	r3, [r6, #0]
   36434:	eba6 0608 	sub.w	r6, r6, r8
   36438:	f1b9 0f00 	cmp.w	r9, #0
   3643c:	d009      	beq.n	36452 <__ssvfscanf_r+0xcee>
   3643e:	1c71      	adds	r1, r6, #1
   36440:	428d      	cmp	r5, r1
   36442:	d906      	bls.n	36452 <__ssvfscanf_r+0xcee>
   36444:	f8d9 0000 	ldr.w	r0, [r9]
   36448:	f004 faba 	bl	3a9c0 <realloc>
   3644c:	b108      	cbz	r0, 36452 <__ssvfscanf_r+0xcee>
   3644e:	f8c9 0000 	str.w	r0, [r9]
   36452:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   36454:	4433      	add	r3, r6
   36456:	633b      	str	r3, [r7, #48]	; 0x30
   36458:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3645a:	3301      	adds	r3, #1
   3645c:	627b      	str	r3, [r7, #36]	; 0x24
   3645e:	e656      	b.n	3610e <__ssvfscanf_r+0x9aa>
   36460:	46a9      	mov	r9, r5
   36462:	e7b4      	b.n	363ce <__ssvfscanf_r+0xc6a>
   36464:	0004056b 	.word	0x0004056b
   36468:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3646a:	f04f 0a00 	mov.w	sl, #0
   3646e:	f507 78a8 	add.w	r8, r7, #336	; 0x150
   36472:	3b01      	subs	r3, #1
   36474:	f5b3 7fae 	cmp.w	r3, #348	; 0x15c
   36478:	bf85      	ittet	hi
   3647a:	6afb      	ldrhi	r3, [r7, #44]	; 0x2c
   3647c:	f46f 70ae 	mvnhi.w	r0, #348	; 0x15c
   36480:	f04f 0b00 	movls.w	fp, #0
   36484:	eb03 0b00 	addhi.w	fp, r3, r0
   36488:	bf84      	itt	hi
   3648a:	f240 135d 	movwhi	r3, #349	; 0x15d
   3648e:	62fb      	strhi	r3, [r7, #44]	; 0x2c
   36490:	f449 6358 	orr.w	r3, r9, #3456	; 0xd80
   36494:	f8df 947c 	ldr.w	r9, [pc, #1148]	; 36914 <__ssvfscanf_r+0x11b0>
   36498:	62bb      	str	r3, [r7, #40]	; 0x28
   3649a:	6822      	ldr	r2, [r4, #0]
   3649c:	7813      	ldrb	r3, [r2, #0]
   3649e:	2b39      	cmp	r3, #57	; 0x39
   364a0:	d80f      	bhi.n	364c2 <__ssvfscanf_r+0xd5e>
   364a2:	2b2a      	cmp	r3, #42	; 0x2a
   364a4:	d91a      	bls.n	364dc <__ssvfscanf_r+0xd78>
   364a6:	f1a3 012b 	sub.w	r1, r3, #43	; 0x2b
   364aa:	290e      	cmp	r1, #14
   364ac:	d816      	bhi.n	364dc <__ssvfscanf_r+0xd78>
   364ae:	e8df f001 	tbb	[pc, r1]
   364b2:	157f      	.short	0x157f
   364b4:	4215157f 	.word	0x4215157f
   364b8:	70707070 	.word	0x70707070
   364bc:	78707070 	.word	0x78707070
   364c0:	78          	.byte	0x78
   364c1:	00          	.byte	0x00
   364c2:	2b66      	cmp	r3, #102	; 0x66
   364c4:	d835      	bhi.n	36532 <__ssvfscanf_r+0xdce>
   364c6:	2b60      	cmp	r3, #96	; 0x60
   364c8:	d803      	bhi.n	364d2 <__ssvfscanf_r+0xd6e>
   364ca:	2b46      	cmp	r3, #70	; 0x46
   364cc:	d804      	bhi.n	364d8 <__ssvfscanf_r+0xd74>
   364ce:	2b40      	cmp	r3, #64	; 0x40
   364d0:	d904      	bls.n	364dc <__ssvfscanf_r+0xd78>
   364d2:	69f9      	ldr	r1, [r7, #28]
   364d4:	290a      	cmp	r1, #10
   364d6:	e069      	b.n	365ac <__ssvfscanf_r+0xe48>
   364d8:	2b58      	cmp	r3, #88	; 0x58
   364da:	d06f      	beq.n	365bc <__ssvfscanf_r+0xe58>
   364dc:	6abb      	ldr	r3, [r7, #40]	; 0x28
   364de:	05db      	lsls	r3, r3, #23
   364e0:	d512      	bpl.n	36508 <__ssvfscanf_r+0xda4>
   364e2:	f507 73a8 	add.w	r3, r7, #336	; 0x150
   364e6:	4598      	cmp	r8, r3
   364e8:	d909      	bls.n	364fe <__ssvfscanf_r+0xd9a>
   364ea:	f108 39ff 	add.w	r9, r8, #4294967295	; 0xffffffff
   364ee:	f818 1c01 	ldrb.w	r1, [r8, #-1]
   364f2:	4622      	mov	r2, r4
   364f4:	ee18 0a10 	vmov	r0, s16
   364f8:	46c8      	mov	r8, r9
   364fa:	f006 fb6f 	bl	3cbdc <_sungetc_r>
   364fe:	f507 73a8 	add.w	r3, r7, #336	; 0x150
   36502:	4598      	cmp	r8, r3
   36504:	f43f aa2a 	beq.w	3595c <__ssvfscanf_r+0x1f8>
   36508:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3650a:	f013 0210 	ands.w	r2, r3, #16
   3650e:	d175      	bne.n	365fc <__ssvfscanf_r+0xe98>
   36510:	69fb      	ldr	r3, [r7, #28]
   36512:	f507 71a8 	add.w	r1, r7, #336	; 0x150
   36516:	69bd      	ldr	r5, [r7, #24]
   36518:	ee18 0a10 	vmov	r0, s16
   3651c:	f888 2000 	strb.w	r2, [r8]
   36520:	47a8      	blx	r5
   36522:	6abb      	ldr	r3, [r7, #40]	; 0x28
   36524:	f106 0904 	add.w	r9, r6, #4
   36528:	069d      	lsls	r5, r3, #26
   3652a:	d55e      	bpl.n	365ea <__ssvfscanf_r+0xe86>
   3652c:	6833      	ldr	r3, [r6, #0]
   3652e:	6018      	str	r0, [r3, #0]
   36530:	e060      	b.n	365f4 <__ssvfscanf_r+0xe90>
   36532:	2b78      	cmp	r3, #120	; 0x78
   36534:	e7d1      	b.n	364da <__ssvfscanf_r+0xd76>
   36536:	6ab9      	ldr	r1, [r7, #40]	; 0x28
   36538:	0509      	lsls	r1, r1, #20
   3653a:	d50d      	bpl.n	36558 <__ssvfscanf_r+0xdf4>
   3653c:	69f9      	ldr	r1, [r7, #28]
   3653e:	b929      	cbnz	r1, 3654c <__ssvfscanf_r+0xde8>
   36540:	6ab9      	ldr	r1, [r7, #40]	; 0x28
   36542:	f441 7100 	orr.w	r1, r1, #512	; 0x200
   36546:	62b9      	str	r1, [r7, #40]	; 0x28
   36548:	2108      	movs	r1, #8
   3654a:	61f9      	str	r1, [r7, #28]
   3654c:	6ab9      	ldr	r1, [r7, #40]	; 0x28
   3654e:	054d      	lsls	r5, r1, #21
   36550:	d505      	bpl.n	3655e <__ssvfscanf_r+0xdfa>
   36552:	f421 61b0 	bic.w	r1, r1, #1408	; 0x580
   36556:	62b9      	str	r1, [r7, #40]	; 0x28
   36558:	f808 3b01 	strb.w	r3, [r8], #1
   3655c:	e00d      	b.n	3657a <__ssvfscanf_r+0xe16>
   3655e:	6abb      	ldr	r3, [r7, #40]	; 0x28
   36560:	f423 7360 	bic.w	r3, r3, #896	; 0x380
   36564:	62bb      	str	r3, [r7, #40]	; 0x28
   36566:	f1bb 0f00 	cmp.w	fp, #0
   3656a:	d004      	beq.n	36576 <__ssvfscanf_r+0xe12>
   3656c:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3656e:	f10b 3bff 	add.w	fp, fp, #4294967295	; 0xffffffff
   36572:	3301      	adds	r3, #1
   36574:	62fb      	str	r3, [r7, #44]	; 0x2c
   36576:	f10a 0a01 	add.w	sl, sl, #1
   3657a:	6863      	ldr	r3, [r4, #4]
   3657c:	3b01      	subs	r3, #1
   3657e:	2b00      	cmp	r3, #0
   36580:	6063      	str	r3, [r4, #4]
   36582:	dd2a      	ble.n	365da <__ssvfscanf_r+0xe76>
   36584:	3201      	adds	r2, #1
   36586:	6022      	str	r2, [r4, #0]
   36588:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3658a:	3b01      	subs	r3, #1
   3658c:	62fb      	str	r3, [r7, #44]	; 0x2c
   3658e:	d184      	bne.n	3649a <__ssvfscanf_r+0xd36>
   36590:	e7a4      	b.n	364dc <__ssvfscanf_r+0xd78>
   36592:	69f9      	ldr	r1, [r7, #28]
   36594:	f939 1011 	ldrsh.w	r1, [r9, r1, lsl #1]
   36598:	61f9      	str	r1, [r7, #28]
   3659a:	6ab9      	ldr	r1, [r7, #40]	; 0x28
   3659c:	f421 6138 	bic.w	r1, r1, #2944	; 0xb80
   365a0:	e7d9      	b.n	36556 <__ssvfscanf_r+0xdf2>
   365a2:	69f9      	ldr	r1, [r7, #28]
   365a4:	f939 1011 	ldrsh.w	r1, [r9, r1, lsl #1]
   365a8:	2908      	cmp	r1, #8
   365aa:	61f9      	str	r1, [r7, #28]
   365ac:	dcf5      	bgt.n	3659a <__ssvfscanf_r+0xe36>
   365ae:	e795      	b.n	364dc <__ssvfscanf_r+0xd78>
   365b0:	6ab9      	ldr	r1, [r7, #40]	; 0x28
   365b2:	0608      	lsls	r0, r1, #24
   365b4:	d592      	bpl.n	364dc <__ssvfscanf_r+0xd78>
   365b6:	f021 0180 	bic.w	r1, r1, #128	; 0x80
   365ba:	e7cc      	b.n	36556 <__ssvfscanf_r+0xdf2>
   365bc:	6ab9      	ldr	r1, [r7, #40]	; 0x28
   365be:	f401 61c0 	and.w	r1, r1, #1536	; 0x600
   365c2:	f5b1 7f00 	cmp.w	r1, #512	; 0x200
   365c6:	d189      	bne.n	364dc <__ssvfscanf_r+0xd78>
   365c8:	6ab9      	ldr	r1, [r7, #40]	; 0x28
   365ca:	f421 7500 	bic.w	r5, r1, #512	; 0x200
   365ce:	f445 61a0 	orr.w	r1, r5, #1280	; 0x500
   365d2:	62b9      	str	r1, [r7, #40]	; 0x28
   365d4:	2110      	movs	r1, #16
   365d6:	61f9      	str	r1, [r7, #28]
   365d8:	e7be      	b.n	36558 <__ssvfscanf_r+0xdf4>
   365da:	4621      	mov	r1, r4
   365dc:	ee18 0a10 	vmov	r0, s16
   365e0:	f006 fb39 	bl	3cc56 <__ssrefill_r>
   365e4:	2800      	cmp	r0, #0
   365e6:	d0cf      	beq.n	36588 <__ssvfscanf_r+0xe24>
   365e8:	e778      	b.n	364dc <__ssvfscanf_r+0xd78>
   365ea:	6abb      	ldr	r3, [r7, #40]	; 0x28
   365ec:	0719      	lsls	r1, r3, #28
   365ee:	d50e      	bpl.n	3660e <__ssvfscanf_r+0xeaa>
   365f0:	6833      	ldr	r3, [r6, #0]
   365f2:	7018      	strb	r0, [r3, #0]
   365f4:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   365f6:	464e      	mov	r6, r9
   365f8:	3301      	adds	r3, #1
   365fa:	627b      	str	r3, [r7, #36]	; 0x24
   365fc:	f507 73a8 	add.w	r3, r7, #336	; 0x150
   36600:	eba8 0803 	sub.w	r8, r8, r3
   36604:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   36606:	44c2      	add	sl, r8
   36608:	4453      	add	r3, sl
   3660a:	f7ff b96a 	b.w	358e2 <__ssvfscanf_r+0x17e>
   3660e:	6abb      	ldr	r3, [r7, #40]	; 0x28
   36610:	075a      	lsls	r2, r3, #29
   36612:	d502      	bpl.n	3661a <__ssvfscanf_r+0xeb6>
   36614:	6833      	ldr	r3, [r6, #0]
   36616:	8018      	strh	r0, [r3, #0]
   36618:	e7ec      	b.n	365f4 <__ssvfscanf_r+0xe90>
   3661a:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3661c:	f013 0201 	ands.w	r2, r3, #1
   36620:	d184      	bne.n	3652c <__ssvfscanf_r+0xdc8>
   36622:	6abb      	ldr	r3, [r7, #40]	; 0x28
   36624:	079b      	lsls	r3, r3, #30
   36626:	d581      	bpl.n	3652c <__ssvfscanf_r+0xdc8>
   36628:	4bb9      	ldr	r3, [pc, #740]	; (36910 <__ssvfscanf_r+0x11ac>)
   3662a:	ee18 0a10 	vmov	r0, s16
   3662e:	69b9      	ldr	r1, [r7, #24]
   36630:	4299      	cmp	r1, r3
   36632:	69fb      	ldr	r3, [r7, #28]
   36634:	f507 71a8 	add.w	r1, r7, #336	; 0x150
   36638:	d105      	bne.n	36646 <__ssvfscanf_r+0xee2>
   3663a:	f005 fd67 	bl	3c10c <_strtoull_r>
   3663e:	6833      	ldr	r3, [r6, #0]
   36640:	e9c3 0100 	strd	r0, r1, [r3]
   36644:	e7d6      	b.n	365f4 <__ssvfscanf_r+0xe90>
   36646:	f005 fcb5 	bl	3bfb4 <_strtoll_r>
   3664a:	e7f8      	b.n	3663e <__ssvfscanf_r+0xeda>
   3664c:	ee18 0a10 	vmov	r0, s16
   36650:	f04f 0b00 	mov.w	fp, #0
   36654:	f003 f86c 	bl	39730 <_localeconv_r>
   36658:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3665a:	6802      	ldr	r2, [r0, #0]
   3665c:	46da      	mov	sl, fp
   3665e:	3b01      	subs	r3, #1
   36660:	f8c7 b028 	str.w	fp, [r7, #40]	; 0x28
   36664:	f5b3 7fae 	cmp.w	r3, #348	; 0x15c
   36668:	bf83      	ittte	hi
   3666a:	6afb      	ldrhi	r3, [r7, #44]	; 0x2c
   3666c:	f46f 75ae 	mvnhi.w	r5, #348	; 0x15c
   36670:	f240 115d 	movwhi	r1, #349	; 0x15d
   36674:	2300      	movls	r3, #0
   36676:	bf88      	it	hi
   36678:	195b      	addhi	r3, r3, r5
   3667a:	f449 65f0 	orr.w	r5, r9, #1920	; 0x780
   3667e:	f507 79a8 	add.w	r9, r7, #336	; 0x150
   36682:	bf88      	it	hi
   36684:	62f9      	strhi	r1, [r7, #44]	; 0x2c
   36686:	e9c7 bb03 	strd	fp, fp, [r7, #12]
   3668a:	6821      	ldr	r1, [r4, #0]
   3668c:	60b9      	str	r1, [r7, #8]
   3668e:	7809      	ldrb	r1, [r1, #0]
   36690:	294e      	cmp	r1, #78	; 0x4e
   36692:	d861      	bhi.n	36758 <__ssvfscanf_r+0xff4>
   36694:	2940      	cmp	r1, #64	; 0x40
   36696:	d837      	bhi.n	36708 <__ssvfscanf_r+0xfa4>
   36698:	f1a1 0c2b 	sub.w	ip, r1, #43	; 0x2b
   3669c:	fa5f fe8c 	uxtb.w	lr, ip
   366a0:	f1be 0f0e 	cmp.w	lr, #14
   366a4:	d933      	bls.n	3670e <__ssvfscanf_r+0xfaa>
   366a6:	f892 c000 	ldrb.w	ip, [r2]
   366aa:	458c      	cmp	ip, r1
   366ac:	f040 80b5 	bne.w	3681a <__ssvfscanf_r+0x10b6>
   366b0:	05a8      	lsls	r0, r5, #22
   366b2:	f140 80b2 	bpl.w	3681a <__ssvfscanf_r+0x10b6>
   366b6:	f425 7520 	bic.w	r5, r5, #640	; 0x280
   366ba:	f8c7 a028 	str.w	sl, [r7, #40]	; 0x28
   366be:	e083      	b.n	367c8 <__ssvfscanf_r+0x1064>
   366c0:	f1a1 0c61 	sub.w	ip, r1, #97	; 0x61
   366c4:	f1bc 0f0d 	cmp.w	ip, #13
   366c8:	d8ed      	bhi.n	366a6 <__ssvfscanf_r+0xf42>
   366ca:	a001      	add	r0, pc, #4	; (adr r0, 366d0 <__ssvfscanf_r+0xf6c>)
   366cc:	f850 f02c 	ldr.w	pc, [r0, ip, lsl #2]
   366d0:	00036815 	.word	0x00036815
   366d4:	000366a7 	.word	0x000366a7
   366d8:	000366a7 	.word	0x000366a7
   366dc:	000366a7 	.word	0x000366a7
   366e0:	0003688d 	.word	0x0003688d
   366e4:	00036875 	.word	0x00036875
   366e8:	000366a7 	.word	0x000366a7
   366ec:	000366a7 	.word	0x000366a7
   366f0:	00036849 	.word	0x00036849
   366f4:	000366a7 	.word	0x000366a7
   366f8:	000366a7 	.word	0x000366a7
   366fc:	000366a7 	.word	0x000366a7
   36700:	000366a7 	.word	0x000366a7
   36704:	000367d9 	.word	0x000367d9
   36708:	f1a1 0c41 	sub.w	ip, r1, #65	; 0x41
   3670c:	e7da      	b.n	366c4 <__ssvfscanf_r+0xf60>
   3670e:	f1bc 0f0e 	cmp.w	ip, #14
   36712:	d8c8      	bhi.n	366a6 <__ssvfscanf_r+0xf42>
   36714:	a001      	add	r0, pc, #4	; (adr r0, 3671c <__ssvfscanf_r+0xfb8>)
   36716:	f850 f02c 	ldr.w	pc, [r0, ip, lsl #2]
   3671a:	bf00      	nop
   3671c:	000367cf 	.word	0x000367cf
   36720:	000366a7 	.word	0x000366a7
   36724:	000367cf 	.word	0x000367cf
   36728:	000366a7 	.word	0x000366a7
   3672c:	000366a7 	.word	0x000366a7
   36730:	00036781 	.word	0x00036781
   36734:	000367bf 	.word	0x000367bf
   36738:	000367bf 	.word	0x000367bf
   3673c:	000367bf 	.word	0x000367bf
   36740:	000367bf 	.word	0x000367bf
   36744:	000367bf 	.word	0x000367bf
   36748:	000367bf 	.word	0x000367bf
   3674c:	000367bf 	.word	0x000367bf
   36750:	000367bf 	.word	0x000367bf
   36754:	000367bf 	.word	0x000367bf
   36758:	296e      	cmp	r1, #110	; 0x6e
   3675a:	d80c      	bhi.n	36776 <__ssvfscanf_r+0x1012>
   3675c:	2960      	cmp	r1, #96	; 0x60
   3675e:	d8af      	bhi.n	366c0 <__ssvfscanf_r+0xf5c>
   36760:	2954      	cmp	r1, #84	; 0x54
   36762:	f000 808d 	beq.w	36880 <__ssvfscanf_r+0x111c>
   36766:	2959      	cmp	r1, #89	; 0x59
   36768:	d19d      	bne.n	366a6 <__ssvfscanf_r+0xf42>
   3676a:	f1b8 0f07 	cmp.w	r8, #7
   3676e:	d154      	bne.n	3681a <__ssvfscanf_r+0x10b6>
   36770:	f04f 0808 	mov.w	r8, #8
   36774:	e028      	b.n	367c8 <__ssvfscanf_r+0x1064>
   36776:	2974      	cmp	r1, #116	; 0x74
   36778:	f000 8082 	beq.w	36880 <__ssvfscanf_r+0x111c>
   3677c:	2979      	cmp	r1, #121	; 0x79
   3677e:	e7f3      	b.n	36768 <__ssvfscanf_r+0x1004>
   36780:	05e8      	lsls	r0, r5, #23
   36782:	d51c      	bpl.n	367be <__ssvfscanf_r+0x105a>
   36784:	f025 0580 	bic.w	r5, r5, #128	; 0x80
   36788:	f10a 0a01 	add.w	sl, sl, #1
   3678c:	b11b      	cbz	r3, 36796 <__ssvfscanf_r+0x1032>
   3678e:	6af9      	ldr	r1, [r7, #44]	; 0x2c
   36790:	3b01      	subs	r3, #1
   36792:	3101      	adds	r1, #1
   36794:	62f9      	str	r1, [r7, #44]	; 0x2c
   36796:	6af9      	ldr	r1, [r7, #44]	; 0x2c
   36798:	3901      	subs	r1, #1
   3679a:	62f9      	str	r1, [r7, #44]	; 0x2c
   3679c:	6b39      	ldr	r1, [r7, #48]	; 0x30
   3679e:	3101      	adds	r1, #1
   367a0:	6339      	str	r1, [r7, #48]	; 0x30
   367a2:	6861      	ldr	r1, [r4, #4]
   367a4:	3901      	subs	r1, #1
   367a6:	2900      	cmp	r1, #0
   367a8:	6061      	str	r1, [r4, #4]
   367aa:	f340 808f 	ble.w	368cc <__ssvfscanf_r+0x1168>
   367ae:	68b8      	ldr	r0, [r7, #8]
   367b0:	3001      	adds	r0, #1
   367b2:	6020      	str	r0, [r4, #0]
   367b4:	6af9      	ldr	r1, [r7, #44]	; 0x2c
   367b6:	2900      	cmp	r1, #0
   367b8:	f47f af67 	bne.w	3668a <__ssvfscanf_r+0xf26>
   367bc:	e02d      	b.n	3681a <__ssvfscanf_r+0x10b6>
   367be:	eb1b 0f08 	cmn.w	fp, r8
   367c2:	d12a      	bne.n	3681a <__ssvfscanf_r+0x10b6>
   367c4:	f425 75c0 	bic.w	r5, r5, #384	; 0x180
   367c8:	f809 1b01 	strb.w	r1, [r9], #1
   367cc:	e7e3      	b.n	36796 <__ssvfscanf_r+0x1032>
   367ce:	0628      	lsls	r0, r5, #24
   367d0:	d523      	bpl.n	3681a <__ssvfscanf_r+0x10b6>
   367d2:	f025 0580 	bic.w	r5, r5, #128	; 0x80
   367d6:	e7f7      	b.n	367c8 <__ssvfscanf_r+0x1064>
   367d8:	f1bb 0f00 	cmp.w	fp, #0
   367dc:	d10c      	bne.n	367f8 <__ssvfscanf_r+0x1094>
   367de:	f1ba 0f00 	cmp.w	sl, #0
   367e2:	d10c      	bne.n	367fe <__ssvfscanf_r+0x109a>
   367e4:	f405 6ce0 	and.w	ip, r5, #1792	; 0x700
   367e8:	f5bc 6fe0 	cmp.w	ip, #1792	; 0x700
   367ec:	d107      	bne.n	367fe <__ssvfscanf_r+0x109a>
   367ee:	f425 65f0 	bic.w	r5, r5, #1920	; 0x780
   367f2:	f04f 0b01 	mov.w	fp, #1
   367f6:	e7e7      	b.n	367c8 <__ssvfscanf_r+0x1064>
   367f8:	f1bb 0f02 	cmp.w	fp, #2
   367fc:	d060      	beq.n	368c0 <__ssvfscanf_r+0x115c>
   367fe:	f1b8 0f01 	cmp.w	r8, #1
   36802:	d002      	beq.n	3680a <__ssvfscanf_r+0x10a6>
   36804:	f1b8 0f04 	cmp.w	r8, #4
   36808:	d107      	bne.n	3681a <__ssvfscanf_r+0x10b6>
   3680a:	f108 0801 	add.w	r8, r8, #1
   3680e:	fa5f f888 	uxtb.w	r8, r8
   36812:	e7d9      	b.n	367c8 <__ssvfscanf_r+0x1064>
   36814:	f1bb 0f01 	cmp.w	fp, #1
   36818:	d055      	beq.n	368c6 <__ssvfscanf_r+0x1162>
   3681a:	f1ba 0f00 	cmp.w	sl, #0
   3681e:	d001      	beq.n	36824 <__ssvfscanf_r+0x10c0>
   36820:	f425 7580 	bic.w	r5, r5, #256	; 0x100
   36824:	f10b 3bff 	add.w	fp, fp, #4294967295	; 0xffffffff
   36828:	f1bb 0f01 	cmp.w	fp, #1
   3682c:	d85b      	bhi.n	368e6 <__ssvfscanf_r+0x1182>
   3682e:	f507 75a8 	add.w	r5, r7, #336	; 0x150
   36832:	45a9      	cmp	r9, r5
   36834:	f67f a892 	bls.w	3595c <__ssvfscanf_r+0x1f8>
   36838:	4622      	mov	r2, r4
   3683a:	f819 1d01 	ldrb.w	r1, [r9, #-1]!
   3683e:	ee18 0a10 	vmov	r0, s16
   36842:	f006 f9cb 	bl	3cbdc <_sungetc_r>
   36846:	e7f4      	b.n	36832 <__ssvfscanf_r+0x10ce>
   36848:	f1b8 0f00 	cmp.w	r8, #0
   3684c:	d10c      	bne.n	36868 <__ssvfscanf_r+0x1104>
   3684e:	f1ba 0f00 	cmp.w	sl, #0
   36852:	d1e5      	bne.n	36820 <__ssvfscanf_r+0x10bc>
   36854:	f405 6ce0 	and.w	ip, r5, #1792	; 0x700
   36858:	f5bc 6fe0 	cmp.w	ip, #1792	; 0x700
   3685c:	d1e2      	bne.n	36824 <__ssvfscanf_r+0x10c0>
   3685e:	f425 65f0 	bic.w	r5, r5, #1920	; 0x780
   36862:	f04f 0801 	mov.w	r8, #1
   36866:	e7af      	b.n	367c8 <__ssvfscanf_r+0x1064>
   36868:	f1b8 0f03 	cmp.w	r8, #3
   3686c:	d0cd      	beq.n	3680a <__ssvfscanf_r+0x10a6>
   3686e:	f1b8 0f05 	cmp.w	r8, #5
   36872:	e7c9      	b.n	36808 <__ssvfscanf_r+0x10a4>
   36874:	f1b8 0f02 	cmp.w	r8, #2
   36878:	d1cf      	bne.n	3681a <__ssvfscanf_r+0x10b6>
   3687a:	f04f 0803 	mov.w	r8, #3
   3687e:	e7a3      	b.n	367c8 <__ssvfscanf_r+0x1064>
   36880:	f1b8 0f06 	cmp.w	r8, #6
   36884:	d1c9      	bne.n	3681a <__ssvfscanf_r+0x10b6>
   36886:	f04f 0807 	mov.w	r8, #7
   3688a:	e79d      	b.n	367c8 <__ssvfscanf_r+0x1064>
   3688c:	f405 6ca0 	and.w	ip, r5, #1280	; 0x500
   36890:	f5bc 6f80 	cmp.w	ip, #1024	; 0x400
   36894:	d004      	beq.n	368a0 <__ssvfscanf_r+0x113c>
   36896:	0568      	lsls	r0, r5, #21
   36898:	d5bf      	bpl.n	3681a <__ssvfscanf_r+0x10b6>
   3689a:	f1ba 0f00 	cmp.w	sl, #0
   3689e:	d0c1      	beq.n	36824 <__ssvfscanf_r+0x10c0>
   368a0:	05a8      	lsls	r0, r5, #22
   368a2:	f425 65f0 	bic.w	r5, r5, #1920	; 0x780
   368a6:	bf58      	it	pl
   368a8:	6ab8      	ldrpl	r0, [r7, #40]	; 0x28
   368aa:	f445 75c0 	orr.w	r5, r5, #384	; 0x180
   368ae:	bf58      	it	pl
   368b0:	ebaa 0000 	subpl.w	r0, sl, r0
   368b4:	f04f 0a00 	mov.w	sl, #0
   368b8:	bf58      	it	pl
   368ba:	e9c7 9003 	strdpl	r9, r0, [r7, #12]
   368be:	e783      	b.n	367c8 <__ssvfscanf_r+0x1064>
   368c0:	f04f 0b03 	mov.w	fp, #3
   368c4:	e780      	b.n	367c8 <__ssvfscanf_r+0x1064>
   368c6:	f04f 0b02 	mov.w	fp, #2
   368ca:	e77d      	b.n	367c8 <__ssvfscanf_r+0x1064>
   368cc:	4621      	mov	r1, r4
   368ce:	ee18 0a10 	vmov	r0, s16
   368d2:	e9c7 2301 	strd	r2, r3, [r7, #4]
   368d6:	f006 f9be 	bl	3cc56 <__ssrefill_r>
   368da:	e9d7 2301 	ldrd	r2, r3, [r7, #4]
   368de:	2800      	cmp	r0, #0
   368e0:	f43f af68 	beq.w	367b4 <__ssvfscanf_r+0x1050>
   368e4:	e799      	b.n	3681a <__ssvfscanf_r+0x10b6>
   368e6:	f108 33ff 	add.w	r3, r8, #4294967295	; 0xffffffff
   368ea:	2b06      	cmp	r3, #6
   368ec:	d82e      	bhi.n	3694c <__ssvfscanf_r+0x11e8>
   368ee:	f1b8 0f02 	cmp.w	r8, #2
   368f2:	d83c      	bhi.n	3696e <__ssvfscanf_r+0x120a>
   368f4:	f507 75a8 	add.w	r5, r7, #336	; 0x150
   368f8:	45a9      	cmp	r9, r5
   368fa:	f67f a82f 	bls.w	3595c <__ssvfscanf_r+0x1f8>
   368fe:	4622      	mov	r2, r4
   36900:	f819 1d01 	ldrb.w	r1, [r9, #-1]!
   36904:	ee18 0a10 	vmov	r0, s16
   36908:	f006 f968 	bl	3cbdc <_sungetc_r>
   3690c:	e7f4      	b.n	368f8 <__ssvfscanf_r+0x1194>
   3690e:	bf00      	nop
   36910:	0003c215 	.word	0x0003c215
   36914:	00040528 	.word	0x00040528
   36918:	f813 1d01 	ldrb.w	r1, [r3, #-1]!
   3691c:	4622      	mov	r2, r4
   3691e:	ee18 0a10 	vmov	r0, s16
   36922:	62fb      	str	r3, [r7, #44]	; 0x2c
   36924:	f006 f95a 	bl	3cbdc <_sungetc_r>
   36928:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3692a:	f10b 3bff 	add.w	fp, fp, #4294967295	; 0xffffffff
   3692e:	fa5f fb8b 	uxtb.w	fp, fp
   36932:	f1bb 0f02 	cmp.w	fp, #2
   36936:	d1ef      	bne.n	36918 <__ssvfscanf_r+0x11b4>
   36938:	f1a8 0803 	sub.w	r8, r8, #3
   3693c:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   3693e:	fa5f f888 	uxtb.w	r8, r8
   36942:	eba3 0308 	sub.w	r3, r3, r8
   36946:	eba9 0908 	sub.w	r9, r9, r8
   3694a:	633b      	str	r3, [r7, #48]	; 0x30
   3694c:	05eb      	lsls	r3, r5, #23
   3694e:	d530      	bpl.n	369b2 <__ssvfscanf_r+0x124e>
   36950:	0568      	lsls	r0, r5, #21
   36952:	d50f      	bpl.n	36974 <__ssvfscanf_r+0x1210>
   36954:	f507 75a8 	add.w	r5, r7, #336	; 0x150
   36958:	45a9      	cmp	r9, r5
   3695a:	f67e afff 	bls.w	3595c <__ssvfscanf_r+0x1f8>
   3695e:	4622      	mov	r2, r4
   36960:	f819 1d01 	ldrb.w	r1, [r9, #-1]!
   36964:	ee18 0a10 	vmov	r0, s16
   36968:	f006 f938 	bl	3cbdc <_sungetc_r>
   3696c:	e7f4      	b.n	36958 <__ssvfscanf_r+0x11f4>
   3696e:	46c3      	mov	fp, r8
   36970:	464b      	mov	r3, r9
   36972:	e7da      	b.n	3692a <__ssvfscanf_r+0x11c6>
   36974:	f819 1c01 	ldrb.w	r1, [r9, #-1]
   36978:	f109 38ff 	add.w	r8, r9, #4294967295	; 0xffffffff
   3697c:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   3697e:	2965      	cmp	r1, #101	; 0x65
   36980:	f103 3bff 	add.w	fp, r3, #4294967295	; 0xffffffff
   36984:	d00d      	beq.n	369a2 <__ssvfscanf_r+0x123e>
   36986:	2945      	cmp	r1, #69	; 0x45
   36988:	d00b      	beq.n	369a2 <__ssvfscanf_r+0x123e>
   3698a:	4622      	mov	r2, r4
   3698c:	ee18 0a10 	vmov	r0, s16
   36990:	f006 f924 	bl	3cbdc <_sungetc_r>
   36994:	6b3b      	ldr	r3, [r7, #48]	; 0x30
   36996:	f1a9 0802 	sub.w	r8, r9, #2
   3699a:	f819 1c02 	ldrb.w	r1, [r9, #-2]
   3699e:	f1a3 0b02 	sub.w	fp, r3, #2
   369a2:	46c1      	mov	r9, r8
   369a4:	4622      	mov	r2, r4
   369a6:	ee18 0a10 	vmov	r0, s16
   369aa:	f006 f917 	bl	3cbdc <_sungetc_r>
   369ae:	f8c7 b030 	str.w	fp, [r7, #48]	; 0x30
   369b2:	f015 0210 	ands.w	r2, r5, #16
   369b6:	f47e aef3 	bne.w	357a0 <__ssvfscanf_r+0x3c>
   369ba:	f405 63c0 	and.w	r3, r5, #1536	; 0x600
   369be:	f889 2000 	strb.w	r2, [r9]
   369c2:	f5b3 6f80 	cmp.w	r3, #1024	; 0x400
   369c6:	d11a      	bne.n	369fe <__ssvfscanf_r+0x129a>
   369c8:	6abb      	ldr	r3, [r7, #40]	; 0x28
   369ca:	4553      	cmp	r3, sl
   369cc:	eba3 020a 	sub.w	r2, r3, sl
   369d0:	d123      	bne.n	36a1a <__ssvfscanf_r+0x12b6>
   369d2:	f507 71a8 	add.w	r1, r7, #336	; 0x150
   369d6:	2200      	movs	r2, #0
   369d8:	ee18 0a10 	vmov	r0, s16
   369dc:	f106 0a04 	add.w	sl, r6, #4
   369e0:	f005 f980 	bl	3bce4 <_strtod_r>
   369e4:	07e9      	lsls	r1, r5, #31
   369e6:	ec59 8b10 	vmov	r8, r9, d0
   369ea:	d521      	bpl.n	36a30 <__ssvfscanf_r+0x12cc>
   369ec:	6833      	ldr	r3, [r6, #0]
   369ee:	ed83 0b00 	vstr	d0, [r3]
   369f2:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   369f4:	4656      	mov	r6, sl
   369f6:	3301      	adds	r3, #1
   369f8:	627b      	str	r3, [r7, #36]	; 0x24
   369fa:	f7fe bed1 	b.w	357a0 <__ssvfscanf_r+0x3c>
   369fe:	693b      	ldr	r3, [r7, #16]
   36a00:	2b00      	cmp	r3, #0
   36a02:	d0e6      	beq.n	369d2 <__ssvfscanf_r+0x126e>
   36a04:	68f9      	ldr	r1, [r7, #12]
   36a06:	230a      	movs	r3, #10
   36a08:	ee18 0a10 	vmov	r0, s16
   36a0c:	3101      	adds	r1, #1
   36a0e:	f7fd fcb7 	bl	34380 <_strtol_r>
   36a12:	693b      	ldr	r3, [r7, #16]
   36a14:	f8d7 900c 	ldr.w	r9, [r7, #12]
   36a18:	1ac2      	subs	r2, r0, r3
   36a1a:	f207 23a3 	addw	r3, r7, #675	; 0x2a3
   36a1e:	4917      	ldr	r1, [pc, #92]	; (36a7c <__ssvfscanf_r+0x1318>)
   36a20:	4599      	cmp	r9, r3
   36a22:	bf28      	it	cs
   36a24:	f207 29a2 	addwcs	r9, r7, #674	; 0x2a2
   36a28:	4648      	mov	r0, r9
   36a2a:	f7fd f809 	bl	33a40 <sprintf>
   36a2e:	e7d0      	b.n	369d2 <__ssvfscanf_r+0x126e>
   36a30:	07aa      	lsls	r2, r5, #30
   36a32:	6836      	ldr	r6, [r6, #0]
   36a34:	d502      	bpl.n	36a3c <__ssvfscanf_r+0x12d8>
   36a36:	ed86 0b00 	vstr	d0, [r6]
   36a3a:	e7da      	b.n	369f2 <__ssvfscanf_r+0x128e>
   36a3c:	ee10 2a10 	vmov	r2, s0
   36a40:	464b      	mov	r3, r9
   36a42:	ee10 0a10 	vmov	r0, s0
   36a46:	4649      	mov	r1, r9
   36a48:	f009 fa12 	bl	3fe70 <__aeabi_dcmpun>
   36a4c:	b128      	cbz	r0, 36a5a <__ssvfscanf_r+0x12f6>
   36a4e:	480c      	ldr	r0, [pc, #48]	; (36a80 <__ssvfscanf_r+0x131c>)
   36a50:	f004 fa02 	bl	3ae58 <nanf>
   36a54:	ed86 0a00 	vstr	s0, [r6]
   36a58:	e7cb      	b.n	369f2 <__ssvfscanf_r+0x128e>
   36a5a:	4640      	mov	r0, r8
   36a5c:	4649      	mov	r1, r9
   36a5e:	f009 fa65 	bl	3ff2c <__aeabi_d2f>
   36a62:	6030      	str	r0, [r6, #0]
   36a64:	e7c5      	b.n	369f2 <__ssvfscanf_r+0x128e>
   36a66:	f855 3024 	ldr.w	r3, [r5, r4, lsl #2]
   36a6a:	6818      	ldr	r0, [r3, #0]
   36a6c:	f002 fbbc 	bl	391e8 <free>
   36a70:	f855 3024 	ldr.w	r3, [r5, r4, lsl #2]
   36a74:	3401      	adds	r4, #1
   36a76:	601e      	str	r6, [r3, #0]
   36a78:	f7ff b83f 	b.w	35afa <__ssvfscanf_r+0x396>
   36a7c:	00040523 	.word	0x00040523
   36a80:	0004099d 	.word	0x0004099d
   36a84:	00000000 	.word	0x00000000

00036a88 <_vfprintf_r>:
   36a88:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   36a8c:	ed2d 8b04 	vpush	{d8-d9}
   36a90:	b0cf      	sub	sp, #316	; 0x13c
   36a92:	4689      	mov	r9, r1
   36a94:	4690      	mov	r8, r2
   36a96:	461c      	mov	r4, r3
   36a98:	461e      	mov	r6, r3
   36a9a:	4682      	mov	sl, r0
   36a9c:	f002 fe48 	bl	39730 <_localeconv_r>
   36aa0:	6803      	ldr	r3, [r0, #0]
   36aa2:	4618      	mov	r0, r3
   36aa4:	9313      	str	r3, [sp, #76]	; 0x4c
   36aa6:	f7fd f943 	bl	33d30 <strlen>
   36aaa:	900e      	str	r0, [sp, #56]	; 0x38
   36aac:	f1ba 0f00 	cmp.w	sl, #0
   36ab0:	d005      	beq.n	36abe <_vfprintf_r+0x36>
   36ab2:	f8da 3038 	ldr.w	r3, [sl, #56]	; 0x38
   36ab6:	b913      	cbnz	r3, 36abe <_vfprintf_r+0x36>
   36ab8:	4650      	mov	r0, sl
   36aba:	f002 fb0b 	bl	390d4 <__sinit>
   36abe:	f8d9 3064 	ldr.w	r3, [r9, #100]	; 0x64
   36ac2:	07d8      	lsls	r0, r3, #31
   36ac4:	d407      	bmi.n	36ad6 <_vfprintf_r+0x4e>
   36ac6:	f8b9 300c 	ldrh.w	r3, [r9, #12]
   36aca:	0599      	lsls	r1, r3, #22
   36acc:	d403      	bmi.n	36ad6 <_vfprintf_r+0x4e>
   36ace:	f8d9 0058 	ldr.w	r0, [r9, #88]	; 0x58
   36ad2:	f002 fe68 	bl	397a6 <__retarget_lock_acquire_recursive>
   36ad6:	f9b9 300c 	ldrsh.w	r3, [r9, #12]
   36ada:	049a      	lsls	r2, r3, #18
   36adc:	d409      	bmi.n	36af2 <_vfprintf_r+0x6a>
   36ade:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   36ae2:	f8a9 300c 	strh.w	r3, [r9, #12]
   36ae6:	f8d9 3064 	ldr.w	r3, [r9, #100]	; 0x64
   36aea:	f423 5300 	bic.w	r3, r3, #8192	; 0x2000
   36aee:	f8c9 3064 	str.w	r3, [r9, #100]	; 0x64
   36af2:	f8b9 300c 	ldrh.w	r3, [r9, #12]
   36af6:	071b      	lsls	r3, r3, #28
   36af8:	d502      	bpl.n	36b00 <_vfprintf_r+0x78>
   36afa:	f8d9 3010 	ldr.w	r3, [r9, #16]
   36afe:	b9d3      	cbnz	r3, 36b36 <_vfprintf_r+0xae>
   36b00:	4649      	mov	r1, r9
   36b02:	4650      	mov	r0, sl
   36b04:	f001 fada 	bl	380bc <__swsetup_r>
   36b08:	b1a8      	cbz	r0, 36b36 <_vfprintf_r+0xae>
   36b0a:	f8d9 3064 	ldr.w	r3, [r9, #100]	; 0x64
   36b0e:	07df      	lsls	r7, r3, #31
   36b10:	d508      	bpl.n	36b24 <_vfprintf_r+0x9c>
   36b12:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   36b16:	930f      	str	r3, [sp, #60]	; 0x3c
   36b18:	980f      	ldr	r0, [sp, #60]	; 0x3c
   36b1a:	b04f      	add	sp, #316	; 0x13c
   36b1c:	ecbd 8b04 	vpop	{d8-d9}
   36b20:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   36b24:	f8b9 300c 	ldrh.w	r3, [r9, #12]
   36b28:	059e      	lsls	r6, r3, #22
   36b2a:	d4f2      	bmi.n	36b12 <_vfprintf_r+0x8a>
   36b2c:	f8d9 0058 	ldr.w	r0, [r9, #88]	; 0x58
   36b30:	f002 fe3f 	bl	397b2 <__retarget_lock_release_recursive>
   36b34:	e7ed      	b.n	36b12 <_vfprintf_r+0x8a>
   36b36:	f8b9 300c 	ldrh.w	r3, [r9, #12]
   36b3a:	f003 021a 	and.w	r2, r3, #26
   36b3e:	2a0a      	cmp	r2, #10
   36b40:	d118      	bne.n	36b74 <_vfprintf_r+0xec>
   36b42:	f9b9 200e 	ldrsh.w	r2, [r9, #14]
   36b46:	2a00      	cmp	r2, #0
   36b48:	db14      	blt.n	36b74 <_vfprintf_r+0xec>
   36b4a:	f8d9 2064 	ldr.w	r2, [r9, #100]	; 0x64
   36b4e:	07d5      	lsls	r5, r2, #31
   36b50:	d405      	bmi.n	36b5e <_vfprintf_r+0xd6>
   36b52:	0598      	lsls	r0, r3, #22
   36b54:	d403      	bmi.n	36b5e <_vfprintf_r+0xd6>
   36b56:	f8d9 0058 	ldr.w	r0, [r9, #88]	; 0x58
   36b5a:	f002 fe2a 	bl	397b2 <__retarget_lock_release_recursive>
   36b5e:	4623      	mov	r3, r4
   36b60:	4642      	mov	r2, r8
   36b62:	4649      	mov	r1, r9
   36b64:	4650      	mov	r0, sl
   36b66:	b04f      	add	sp, #316	; 0x13c
   36b68:	ecbd 8b04 	vpop	{d8-d9}
   36b6c:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   36b70:	f001 b9c6 	b.w	37f00 <__sbprintf>
   36b74:	ed9f 7b92 	vldr	d7, [pc, #584]	; 36dc0 <_vfprintf_r+0x338>
   36b78:	2500      	movs	r5, #0
   36b7a:	ac25      	add	r4, sp, #148	; 0x94
   36b7c:	e9cd 5523 	strd	r5, r5, [sp, #140]	; 0x8c
   36b80:	9422      	str	r4, [sp, #136]	; 0x88
   36b82:	9505      	str	r5, [sp, #20]
   36b84:	950a      	str	r5, [sp, #40]	; 0x28
   36b86:	9512      	str	r5, [sp, #72]	; 0x48
   36b88:	9514      	str	r5, [sp, #80]	; 0x50
   36b8a:	950f      	str	r5, [sp, #60]	; 0x3c
   36b8c:	ed8d 7b0c 	vstr	d7, [sp, #48]	; 0x30
   36b90:	e9cd 5515 	strd	r5, r5, [sp, #84]	; 0x54
   36b94:	4643      	mov	r3, r8
   36b96:	461d      	mov	r5, r3
   36b98:	f813 2b01 	ldrb.w	r2, [r3], #1
   36b9c:	b10a      	cbz	r2, 36ba2 <_vfprintf_r+0x11a>
   36b9e:	2a25      	cmp	r2, #37	; 0x25
   36ba0:	d1f9      	bne.n	36b96 <_vfprintf_r+0x10e>
   36ba2:	ebb5 0708 	subs.w	r7, r5, r8
   36ba6:	d00d      	beq.n	36bc4 <_vfprintf_r+0x13c>
   36ba8:	9b24      	ldr	r3, [sp, #144]	; 0x90
   36baa:	443b      	add	r3, r7
   36bac:	9324      	str	r3, [sp, #144]	; 0x90
   36bae:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   36bb0:	3301      	adds	r3, #1
   36bb2:	2b07      	cmp	r3, #7
   36bb4:	9323      	str	r3, [sp, #140]	; 0x8c
   36bb6:	e9c4 8700 	strd	r8, r7, [r4]
   36bba:	dc79      	bgt.n	36cb0 <_vfprintf_r+0x228>
   36bbc:	3408      	adds	r4, #8
   36bbe:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
   36bc0:	443b      	add	r3, r7
   36bc2:	930f      	str	r3, [sp, #60]	; 0x3c
   36bc4:	782b      	ldrb	r3, [r5, #0]
   36bc6:	2b00      	cmp	r3, #0
   36bc8:	f001 814d 	beq.w	37e66 <_vfprintf_r+0x13de>
   36bcc:	2300      	movs	r3, #0
   36bce:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   36bd2:	3501      	adds	r5, #1
   36bd4:	270a      	movs	r7, #10
   36bd6:	469b      	mov	fp, r3
   36bd8:	212b      	movs	r1, #43	; 0x2b
   36bda:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   36bde:	9204      	str	r2, [sp, #16]
   36be0:	9310      	str	r3, [sp, #64]	; 0x40
   36be2:	462b      	mov	r3, r5
   36be4:	f813 2b01 	ldrb.w	r2, [r3], #1
   36be8:	9206      	str	r2, [sp, #24]
   36bea:	930b      	str	r3, [sp, #44]	; 0x2c
   36bec:	9b06      	ldr	r3, [sp, #24]
   36bee:	3b20      	subs	r3, #32
   36bf0:	2b5a      	cmp	r3, #90	; 0x5a
   36bf2:	f200 85c3 	bhi.w	3777c <_vfprintf_r+0xcf4>
   36bf6:	e8df f013 	tbh	[pc, r3, lsl #1]
   36bfa:	007e      	.short	0x007e
   36bfc:	05c105c1 	.word	0x05c105c1
   36c00:	05c10086 	.word	0x05c10086
   36c04:	05c105c1 	.word	0x05c105c1
   36c08:	05c10065 	.word	0x05c10065
   36c0c:	008905c1 	.word	0x008905c1
   36c10:	05c10093 	.word	0x05c10093
   36c14:	00960090 	.word	0x00960090
   36c18:	00b205c1 	.word	0x00b205c1
   36c1c:	00b500b5 	.word	0x00b500b5
   36c20:	00b500b5 	.word	0x00b500b5
   36c24:	00b500b5 	.word	0x00b500b5
   36c28:	00b500b5 	.word	0x00b500b5
   36c2c:	05c100b5 	.word	0x05c100b5
   36c30:	05c105c1 	.word	0x05c105c1
   36c34:	05c105c1 	.word	0x05c105c1
   36c38:	05c105c1 	.word	0x05c105c1
   36c3c:	05c1012a 	.word	0x05c1012a
   36c40:	00fa00e7 	.word	0x00fa00e7
   36c44:	012a012a 	.word	0x012a012a
   36c48:	05c1012a 	.word	0x05c1012a
   36c4c:	05c105c1 	.word	0x05c105c1
   36c50:	00c505c1 	.word	0x00c505c1
   36c54:	05c105c1 	.word	0x05c105c1
   36c58:	05c1049c 	.word	0x05c1049c
   36c5c:	05c105c1 	.word	0x05c105c1
   36c60:	05c104e5 	.word	0x05c104e5
   36c64:	05c10506 	.word	0x05c10506
   36c68:	052805c1 	.word	0x052805c1
   36c6c:	05c105c1 	.word	0x05c105c1
   36c70:	05c105c1 	.word	0x05c105c1
   36c74:	05c105c1 	.word	0x05c105c1
   36c78:	05c105c1 	.word	0x05c105c1
   36c7c:	05c1012a 	.word	0x05c1012a
   36c80:	00fc00e7 	.word	0x00fc00e7
   36c84:	012a012a 	.word	0x012a012a
   36c88:	00c8012a 	.word	0x00c8012a
   36c8c:	00dc00fc 	.word	0x00dc00fc
   36c90:	00d505c1 	.word	0x00d505c1
   36c94:	047705c1 	.word	0x047705c1
   36c98:	04d4049e 	.word	0x04d4049e
   36c9c:	05c100dc 	.word	0x05c100dc
   36ca0:	007c04e5 	.word	0x007c04e5
   36ca4:	05c10508 	.word	0x05c10508
   36ca8:	054705c1 	.word	0x054705c1
   36cac:	007c05c1 	.word	0x007c05c1
   36cb0:	aa22      	add	r2, sp, #136	; 0x88
   36cb2:	4649      	mov	r1, r9
   36cb4:	4650      	mov	r0, sl
   36cb6:	f007 f859 	bl	3dd6c <__sprint_r>
   36cba:	2800      	cmp	r0, #0
   36cbc:	f040 8135 	bne.w	36f2a <_vfprintf_r+0x4a2>
   36cc0:	ac25      	add	r4, sp, #148	; 0x94
   36cc2:	e77c      	b.n	36bbe <_vfprintf_r+0x136>
   36cc4:	4650      	mov	r0, sl
   36cc6:	f002 fd33 	bl	39730 <_localeconv_r>
   36cca:	6843      	ldr	r3, [r0, #4]
   36ccc:	4618      	mov	r0, r3
   36cce:	9314      	str	r3, [sp, #80]	; 0x50
   36cd0:	f7fd f82e 	bl	33d30 <strlen>
   36cd4:	9012      	str	r0, [sp, #72]	; 0x48
   36cd6:	4650      	mov	r0, sl
   36cd8:	f002 fd2a 	bl	39730 <_localeconv_r>
   36cdc:	6883      	ldr	r3, [r0, #8]
   36cde:	212b      	movs	r1, #43	; 0x2b
   36ce0:	930a      	str	r3, [sp, #40]	; 0x28
   36ce2:	9b12      	ldr	r3, [sp, #72]	; 0x48
   36ce4:	b12b      	cbz	r3, 36cf2 <_vfprintf_r+0x26a>
   36ce6:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   36ce8:	b11b      	cbz	r3, 36cf2 <_vfprintf_r+0x26a>
   36cea:	781b      	ldrb	r3, [r3, #0]
   36cec:	b10b      	cbz	r3, 36cf2 <_vfprintf_r+0x26a>
   36cee:	f44b 6b80 	orr.w	fp, fp, #1024	; 0x400
   36cf2:	9d0b      	ldr	r5, [sp, #44]	; 0x2c
   36cf4:	e775      	b.n	36be2 <_vfprintf_r+0x15a>
   36cf6:	f89d 306b 	ldrb.w	r3, [sp, #107]	; 0x6b
   36cfa:	2b00      	cmp	r3, #0
   36cfc:	d1f9      	bne.n	36cf2 <_vfprintf_r+0x26a>
   36cfe:	2320      	movs	r3, #32
   36d00:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   36d04:	e7f5      	b.n	36cf2 <_vfprintf_r+0x26a>
   36d06:	f04b 0b01 	orr.w	fp, fp, #1
   36d0a:	e7f2      	b.n	36cf2 <_vfprintf_r+0x26a>
   36d0c:	f856 3b04 	ldr.w	r3, [r6], #4
   36d10:	2b00      	cmp	r3, #0
   36d12:	9310      	str	r3, [sp, #64]	; 0x40
   36d14:	daed      	bge.n	36cf2 <_vfprintf_r+0x26a>
   36d16:	425b      	negs	r3, r3
   36d18:	9310      	str	r3, [sp, #64]	; 0x40
   36d1a:	f04b 0b04 	orr.w	fp, fp, #4
   36d1e:	e7e8      	b.n	36cf2 <_vfprintf_r+0x26a>
   36d20:	f88d 106b 	strb.w	r1, [sp, #107]	; 0x6b
   36d24:	e7e5      	b.n	36cf2 <_vfprintf_r+0x26a>
   36d26:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   36d28:	f813 2b01 	ldrb.w	r2, [r3], #1
   36d2c:	2a2a      	cmp	r2, #42	; 0x2a
   36d2e:	9206      	str	r2, [sp, #24]
   36d30:	d112      	bne.n	36d58 <_vfprintf_r+0x2d0>
   36d32:	f856 0b04 	ldr.w	r0, [r6], #4
   36d36:	930b      	str	r3, [sp, #44]	; 0x2c
   36d38:	ea40 72e0 	orr.w	r2, r0, r0, asr #31
   36d3c:	9204      	str	r2, [sp, #16]
   36d3e:	e7d8      	b.n	36cf2 <_vfprintf_r+0x26a>
   36d40:	9804      	ldr	r0, [sp, #16]
   36d42:	fb07 2200 	mla	r2, r7, r0, r2
   36d46:	9204      	str	r2, [sp, #16]
   36d48:	f813 2b01 	ldrb.w	r2, [r3], #1
   36d4c:	9206      	str	r2, [sp, #24]
   36d4e:	9a06      	ldr	r2, [sp, #24]
   36d50:	3a30      	subs	r2, #48	; 0x30
   36d52:	2a09      	cmp	r2, #9
   36d54:	d9f4      	bls.n	36d40 <_vfprintf_r+0x2b8>
   36d56:	e748      	b.n	36bea <_vfprintf_r+0x162>
   36d58:	2200      	movs	r2, #0
   36d5a:	9204      	str	r2, [sp, #16]
   36d5c:	e7f7      	b.n	36d4e <_vfprintf_r+0x2c6>
   36d5e:	f04b 0b80 	orr.w	fp, fp, #128	; 0x80
   36d62:	e7c6      	b.n	36cf2 <_vfprintf_r+0x26a>
   36d64:	2200      	movs	r2, #0
   36d66:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   36d68:	9210      	str	r2, [sp, #64]	; 0x40
   36d6a:	9a06      	ldr	r2, [sp, #24]
   36d6c:	9810      	ldr	r0, [sp, #64]	; 0x40
   36d6e:	3a30      	subs	r2, #48	; 0x30
   36d70:	fb07 2200 	mla	r2, r7, r0, r2
   36d74:	9210      	str	r2, [sp, #64]	; 0x40
   36d76:	f813 2b01 	ldrb.w	r2, [r3], #1
   36d7a:	9206      	str	r2, [sp, #24]
   36d7c:	3a30      	subs	r2, #48	; 0x30
   36d7e:	2a09      	cmp	r2, #9
   36d80:	d9f3      	bls.n	36d6a <_vfprintf_r+0x2e2>
   36d82:	e732      	b.n	36bea <_vfprintf_r+0x162>
   36d84:	f04b 0b08 	orr.w	fp, fp, #8
   36d88:	e7b3      	b.n	36cf2 <_vfprintf_r+0x26a>
   36d8a:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   36d8c:	781b      	ldrb	r3, [r3, #0]
   36d8e:	2b68      	cmp	r3, #104	; 0x68
   36d90:	bf05      	ittet	eq
   36d92:	9b0b      	ldreq	r3, [sp, #44]	; 0x2c
   36d94:	f44b 7b00 	orreq.w	fp, fp, #512	; 0x200
   36d98:	f04b 0b40 	orrne.w	fp, fp, #64	; 0x40
   36d9c:	3301      	addeq	r3, #1
   36d9e:	bf08      	it	eq
   36da0:	930b      	streq	r3, [sp, #44]	; 0x2c
   36da2:	e7a6      	b.n	36cf2 <_vfprintf_r+0x26a>
   36da4:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   36da6:	781b      	ldrb	r3, [r3, #0]
   36da8:	2b6c      	cmp	r3, #108	; 0x6c
   36daa:	d105      	bne.n	36db8 <_vfprintf_r+0x330>
   36dac:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   36dae:	3301      	adds	r3, #1
   36db0:	930b      	str	r3, [sp, #44]	; 0x2c
   36db2:	f04b 0b20 	orr.w	fp, fp, #32
   36db6:	e79c      	b.n	36cf2 <_vfprintf_r+0x26a>
   36db8:	f04b 0b10 	orr.w	fp, fp, #16
   36dbc:	e799      	b.n	36cf2 <_vfprintf_r+0x26a>
   36dbe:	bf00      	nop
	...
   36dc8:	4632      	mov	r2, r6
   36dca:	2000      	movs	r0, #0
   36dcc:	f852 3b04 	ldr.w	r3, [r2], #4
   36dd0:	f88d 006b 	strb.w	r0, [sp, #107]	; 0x6b
   36dd4:	9207      	str	r2, [sp, #28]
   36dd6:	f88d 30d4 	strb.w	r3, [sp, #212]	; 0xd4
   36dda:	2301      	movs	r3, #1
   36ddc:	4606      	mov	r6, r0
   36dde:	4605      	mov	r5, r0
   36de0:	f10d 08d4 	add.w	r8, sp, #212	; 0xd4
   36de4:	9003      	str	r0, [sp, #12]
   36de6:	9304      	str	r3, [sp, #16]
   36de8:	e9cd 0008 	strd	r0, r0, [sp, #32]
   36dec:	e1c3      	b.n	37176 <_vfprintf_r+0x6ee>
   36dee:	f04b 0b10 	orr.w	fp, fp, #16
   36df2:	f01b 0f20 	tst.w	fp, #32
   36df6:	d012      	beq.n	36e1e <_vfprintf_r+0x396>
   36df8:	3607      	adds	r6, #7
   36dfa:	f026 0307 	bic.w	r3, r6, #7
   36dfe:	461a      	mov	r2, r3
   36e00:	685d      	ldr	r5, [r3, #4]
   36e02:	f852 6b08 	ldr.w	r6, [r2], #8
   36e06:	9207      	str	r2, [sp, #28]
   36e08:	2d00      	cmp	r5, #0
   36e0a:	da06      	bge.n	36e1a <_vfprintf_r+0x392>
   36e0c:	4276      	negs	r6, r6
   36e0e:	f04f 032d 	mov.w	r3, #45	; 0x2d
   36e12:	eb65 0545 	sbc.w	r5, r5, r5, lsl #1
   36e16:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   36e1a:	2301      	movs	r3, #1
   36e1c:	e39c      	b.n	37558 <_vfprintf_r+0xad0>
   36e1e:	4633      	mov	r3, r6
   36e20:	f01b 0f10 	tst.w	fp, #16
   36e24:	f853 5b04 	ldr.w	r5, [r3], #4
   36e28:	9307      	str	r3, [sp, #28]
   36e2a:	d002      	beq.n	36e32 <_vfprintf_r+0x3aa>
   36e2c:	462e      	mov	r6, r5
   36e2e:	17ed      	asrs	r5, r5, #31
   36e30:	e7ea      	b.n	36e08 <_vfprintf_r+0x380>
   36e32:	f01b 0f40 	tst.w	fp, #64	; 0x40
   36e36:	d003      	beq.n	36e40 <_vfprintf_r+0x3b8>
   36e38:	b22e      	sxth	r6, r5
   36e3a:	f345 35c0 	sbfx	r5, r5, #15, #1
   36e3e:	e7e3      	b.n	36e08 <_vfprintf_r+0x380>
   36e40:	f41b 7f00 	tst.w	fp, #512	; 0x200
   36e44:	d0f2      	beq.n	36e2c <_vfprintf_r+0x3a4>
   36e46:	b26e      	sxtb	r6, r5
   36e48:	f345 15c0 	sbfx	r5, r5, #7, #1
   36e4c:	e7dc      	b.n	36e08 <_vfprintf_r+0x380>
   36e4e:	3607      	adds	r6, #7
   36e50:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   36e54:	f026 0307 	bic.w	r3, r6, #7
   36e58:	ecb3 7b02 	vldmia	r3!, {d7}
   36e5c:	ed8d 7b0c 	vstr	d7, [sp, #48]	; 0x30
   36e60:	9307      	str	r3, [sp, #28]
   36e62:	9b0c      	ldr	r3, [sp, #48]	; 0x30
   36e64:	ee09 3a10 	vmov	s18, r3
   36e68:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   36e6a:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
   36e6e:	ee09 3a90 	vmov	s19, r3
   36e72:	4b3f      	ldr	r3, [pc, #252]	; (36f70 <_vfprintf_r+0x4e8>)
   36e74:	ec51 0b19 	vmov	r0, r1, d9
   36e78:	f008 fffa 	bl	3fe70 <__aeabi_dcmpun>
   36e7c:	bb10      	cbnz	r0, 36ec4 <_vfprintf_r+0x43c>
   36e7e:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   36e82:	4b3b      	ldr	r3, [pc, #236]	; (36f70 <_vfprintf_r+0x4e8>)
   36e84:	ec51 0b19 	vmov	r0, r1, d9
   36e88:	f008 ffd4 	bl	3fe34 <__aeabi_dcmple>
   36e8c:	b9d0      	cbnz	r0, 36ec4 <_vfprintf_r+0x43c>
   36e8e:	2200      	movs	r2, #0
   36e90:	2300      	movs	r3, #0
   36e92:	e9dd 010c 	ldrd	r0, r1, [sp, #48]	; 0x30
   36e96:	f008 ffc3 	bl	3fe20 <__aeabi_dcmplt>
   36e9a:	b110      	cbz	r0, 36ea2 <_vfprintf_r+0x41a>
   36e9c:	232d      	movs	r3, #45	; 0x2d
   36e9e:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   36ea2:	4a34      	ldr	r2, [pc, #208]	; (36f74 <_vfprintf_r+0x4ec>)
   36ea4:	4834      	ldr	r0, [pc, #208]	; (36f78 <_vfprintf_r+0x4f0>)
   36ea6:	9b06      	ldr	r3, [sp, #24]
   36ea8:	2100      	movs	r1, #0
   36eaa:	f02b 0b80 	bic.w	fp, fp, #128	; 0x80
   36eae:	2b47      	cmp	r3, #71	; 0x47
   36eb0:	bfd4      	ite	le
   36eb2:	4690      	movle	r8, r2
   36eb4:	4680      	movgt	r8, r0
   36eb6:	2303      	movs	r3, #3
   36eb8:	e9cd 1303 	strd	r1, r3, [sp, #12]
   36ebc:	2600      	movs	r6, #0
   36ebe:	4633      	mov	r3, r6
   36ec0:	f001 b80b 	b.w	37eda <_vfprintf_r+0x1452>
   36ec4:	e9dd 230c 	ldrd	r2, r3, [sp, #48]	; 0x30
   36ec8:	4610      	mov	r0, r2
   36eca:	4619      	mov	r1, r3
   36ecc:	f008 ffd0 	bl	3fe70 <__aeabi_dcmpun>
   36ed0:	b140      	cbz	r0, 36ee4 <_vfprintf_r+0x45c>
   36ed2:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   36ed4:	4a29      	ldr	r2, [pc, #164]	; (36f7c <_vfprintf_r+0x4f4>)
   36ed6:	2b00      	cmp	r3, #0
   36ed8:	4829      	ldr	r0, [pc, #164]	; (36f80 <_vfprintf_r+0x4f8>)
   36eda:	bfbc      	itt	lt
   36edc:	232d      	movlt	r3, #45	; 0x2d
   36ede:	f88d 306b 	strblt.w	r3, [sp, #107]	; 0x6b
   36ee2:	e7e0      	b.n	36ea6 <_vfprintf_r+0x41e>
   36ee4:	9b06      	ldr	r3, [sp, #24]
   36ee6:	f023 0320 	bic.w	r3, r3, #32
   36eea:	2b41      	cmp	r3, #65	; 0x41
   36eec:	9308      	str	r3, [sp, #32]
   36eee:	d12e      	bne.n	36f4e <_vfprintf_r+0x4c6>
   36ef0:	2330      	movs	r3, #48	; 0x30
   36ef2:	f04b 0b02 	orr.w	fp, fp, #2
   36ef6:	f88d 306c 	strb.w	r3, [sp, #108]	; 0x6c
   36efa:	9b06      	ldr	r3, [sp, #24]
   36efc:	2b61      	cmp	r3, #97	; 0x61
   36efe:	bf0c      	ite	eq
   36f00:	2378      	moveq	r3, #120	; 0x78
   36f02:	2358      	movne	r3, #88	; 0x58
   36f04:	f88d 306d 	strb.w	r3, [sp, #109]	; 0x6d
   36f08:	9b04      	ldr	r3, [sp, #16]
   36f0a:	2b63      	cmp	r3, #99	; 0x63
   36f0c:	dd3a      	ble.n	36f84 <_vfprintf_r+0x4fc>
   36f0e:	1c59      	adds	r1, r3, #1
   36f10:	4650      	mov	r0, sl
   36f12:	f002 fcd5 	bl	398c0 <_malloc_r>
   36f16:	4680      	mov	r8, r0
   36f18:	2800      	cmp	r0, #0
   36f1a:	f040 8203 	bne.w	37324 <_vfprintf_r+0x89c>
   36f1e:	f8b9 300c 	ldrh.w	r3, [r9, #12]
   36f22:	f043 0340 	orr.w	r3, r3, #64	; 0x40
   36f26:	f8a9 300c 	strh.w	r3, [r9, #12]
   36f2a:	f8d9 3064 	ldr.w	r3, [r9, #100]	; 0x64
   36f2e:	07d9      	lsls	r1, r3, #31
   36f30:	d407      	bmi.n	36f42 <_vfprintf_r+0x4ba>
   36f32:	f8b9 300c 	ldrh.w	r3, [r9, #12]
   36f36:	059a      	lsls	r2, r3, #22
   36f38:	d403      	bmi.n	36f42 <_vfprintf_r+0x4ba>
   36f3a:	f8d9 0058 	ldr.w	r0, [r9, #88]	; 0x58
   36f3e:	f002 fc38 	bl	397b2 <__retarget_lock_release_recursive>
   36f42:	f8b9 300c 	ldrh.w	r3, [r9, #12]
   36f46:	065b      	lsls	r3, r3, #25
   36f48:	f57f ade6 	bpl.w	36b18 <_vfprintf_r+0x90>
   36f4c:	e5e1      	b.n	36b12 <_vfprintf_r+0x8a>
   36f4e:	9b04      	ldr	r3, [sp, #16]
   36f50:	3301      	adds	r3, #1
   36f52:	f000 81e9 	beq.w	37328 <_vfprintf_r+0x8a0>
   36f56:	9b08      	ldr	r3, [sp, #32]
   36f58:	2b47      	cmp	r3, #71	; 0x47
   36f5a:	f040 81e8 	bne.w	3732e <_vfprintf_r+0x8a6>
   36f5e:	9b04      	ldr	r3, [sp, #16]
   36f60:	2b00      	cmp	r3, #0
   36f62:	f040 81e4 	bne.w	3732e <_vfprintf_r+0x8a6>
   36f66:	9303      	str	r3, [sp, #12]
   36f68:	2301      	movs	r3, #1
   36f6a:	9304      	str	r3, [sp, #16]
   36f6c:	e00d      	b.n	36f8a <_vfprintf_r+0x502>
   36f6e:	bf00      	nop
   36f70:	7fefffff 	.word	0x7fefffff
   36f74:	000404cf 	.word	0x000404cf
   36f78:	000404d3 	.word	0x000404d3
   36f7c:	000404d7 	.word	0x000404d7
   36f80:	000404db 	.word	0x000404db
   36f84:	f10d 08d4 	add.w	r8, sp, #212	; 0xd4
   36f88:	9003      	str	r0, [sp, #12]
   36f8a:	f44b 7380 	orr.w	r3, fp, #256	; 0x100
   36f8e:	9311      	str	r3, [sp, #68]	; 0x44
   36f90:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   36f92:	2b00      	cmp	r3, #0
   36f94:	f280 81cd 	bge.w	37332 <_vfprintf_r+0x8aa>
   36f98:	9b0c      	ldr	r3, [sp, #48]	; 0x30
   36f9a:	ee08 3a10 	vmov	s16, r3
   36f9e:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   36fa0:	f103 4300 	add.w	r3, r3, #2147483648	; 0x80000000
   36fa4:	ee08 3a90 	vmov	s17, r3
   36fa8:	232d      	movs	r3, #45	; 0x2d
   36faa:	9318      	str	r3, [sp, #96]	; 0x60
   36fac:	9b08      	ldr	r3, [sp, #32]
   36fae:	2b41      	cmp	r3, #65	; 0x41
   36fb0:	f040 81dd 	bne.w	3736e <_vfprintf_r+0x8e6>
   36fb4:	a81c      	add	r0, sp, #112	; 0x70
   36fb6:	eeb0 0a48 	vmov.f32	s0, s16
   36fba:	eef0 0a68 	vmov.f32	s1, s17
   36fbe:	f003 ff51 	bl	3ae64 <frexp>
   36fc2:	2200      	movs	r2, #0
   36fc4:	f04f 537f 	mov.w	r3, #1069547520	; 0x3fc00000
   36fc8:	ec51 0b10 	vmov	r0, r1, d0
   36fcc:	f008 fcb6 	bl	3f93c <__aeabi_dmul>
   36fd0:	2200      	movs	r2, #0
   36fd2:	2300      	movs	r3, #0
   36fd4:	4606      	mov	r6, r0
   36fd6:	460f      	mov	r7, r1
   36fd8:	f008 ff18 	bl	3fe0c <__aeabi_dcmpeq>
   36fdc:	b108      	cbz	r0, 36fe2 <_vfprintf_r+0x55a>
   36fde:	2301      	movs	r3, #1
   36fe0:	931c      	str	r3, [sp, #112]	; 0x70
   36fe2:	4ba5      	ldr	r3, [pc, #660]	; (37278 <_vfprintf_r+0x7f0>)
   36fe4:	4645      	mov	r5, r8
   36fe6:	4aa5      	ldr	r2, [pc, #660]	; (3727c <_vfprintf_r+0x7f4>)
   36fe8:	9906      	ldr	r1, [sp, #24]
   36fea:	2961      	cmp	r1, #97	; 0x61
   36fec:	bf18      	it	ne
   36fee:	461a      	movne	r2, r3
   36ff0:	9b04      	ldr	r3, [sp, #16]
   36ff2:	3b01      	subs	r3, #1
   36ff4:	9217      	str	r2, [sp, #92]	; 0x5c
   36ff6:	9305      	str	r3, [sp, #20]
   36ff8:	2200      	movs	r2, #0
   36ffa:	4ba1      	ldr	r3, [pc, #644]	; (37280 <_vfprintf_r+0x7f8>)
   36ffc:	4630      	mov	r0, r6
   36ffe:	4639      	mov	r1, r7
   37000:	f008 fc9c 	bl	3f93c <__aeabi_dmul>
   37004:	460f      	mov	r7, r1
   37006:	4606      	mov	r6, r0
   37008:	f008 ff48 	bl	3fe9c <__aeabi_d2iz>
   3700c:	9019      	str	r0, [sp, #100]	; 0x64
   3700e:	f008 fc2b 	bl	3f868 <__aeabi_i2d>
   37012:	4602      	mov	r2, r0
   37014:	460b      	mov	r3, r1
   37016:	4630      	mov	r0, r6
   37018:	4639      	mov	r1, r7
   3701a:	f008 fad7 	bl	3f5cc <__aeabi_dsub>
   3701e:	9a19      	ldr	r2, [sp, #100]	; 0x64
   37020:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   37022:	4606      	mov	r6, r0
   37024:	460f      	mov	r7, r1
   37026:	5c9b      	ldrb	r3, [r3, r2]
   37028:	f805 3b01 	strb.w	r3, [r5], #1
   3702c:	9b05      	ldr	r3, [sp, #20]
   3702e:	1c5a      	adds	r2, r3, #1
   37030:	9309      	str	r3, [sp, #36]	; 0x24
   37032:	d007      	beq.n	37044 <_vfprintf_r+0x5bc>
   37034:	3b01      	subs	r3, #1
   37036:	2200      	movs	r2, #0
   37038:	9305      	str	r3, [sp, #20]
   3703a:	2300      	movs	r3, #0
   3703c:	f008 fee6 	bl	3fe0c <__aeabi_dcmpeq>
   37040:	2800      	cmp	r0, #0
   37042:	d0d9      	beq.n	36ff8 <_vfprintf_r+0x570>
   37044:	2200      	movs	r2, #0
   37046:	4b8f      	ldr	r3, [pc, #572]	; (37284 <_vfprintf_r+0x7fc>)
   37048:	4630      	mov	r0, r6
   3704a:	4639      	mov	r1, r7
   3704c:	f008 ff06 	bl	3fe5c <__aeabi_dcmpgt>
   37050:	b960      	cbnz	r0, 3706c <_vfprintf_r+0x5e4>
   37052:	2200      	movs	r2, #0
   37054:	4b8b      	ldr	r3, [pc, #556]	; (37284 <_vfprintf_r+0x7fc>)
   37056:	4630      	mov	r0, r6
   37058:	4639      	mov	r1, r7
   3705a:	f008 fed7 	bl	3fe0c <__aeabi_dcmpeq>
   3705e:	2800      	cmp	r0, #0
   37060:	f000 8180 	beq.w	37364 <_vfprintf_r+0x8dc>
   37064:	9b19      	ldr	r3, [sp, #100]	; 0x64
   37066:	07da      	lsls	r2, r3, #31
   37068:	f140 817c 	bpl.w	37364 <_vfprintf_r+0x8dc>
   3706c:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3706e:	2030      	movs	r0, #48	; 0x30
   37070:	9520      	str	r5, [sp, #128]	; 0x80
   37072:	7bd9      	ldrb	r1, [r3, #15]
   37074:	9a20      	ldr	r2, [sp, #128]	; 0x80
   37076:	1e53      	subs	r3, r2, #1
   37078:	9320      	str	r3, [sp, #128]	; 0x80
   3707a:	f812 3c01 	ldrb.w	r3, [r2, #-1]
   3707e:	428b      	cmp	r3, r1
   37080:	f000 815f 	beq.w	37342 <_vfprintf_r+0x8ba>
   37084:	2b39      	cmp	r3, #57	; 0x39
   37086:	bf0b      	itete	eq
   37088:	9b17      	ldreq	r3, [sp, #92]	; 0x5c
   3708a:	3301      	addne	r3, #1
   3708c:	7a9b      	ldrbeq	r3, [r3, #10]
   3708e:	b2db      	uxtbne	r3, r3
   37090:	f802 3c01 	strb.w	r3, [r2, #-1]
   37094:	eba5 0308 	sub.w	r3, r5, r8
   37098:	9d1c      	ldr	r5, [sp, #112]	; 0x70
   3709a:	9305      	str	r3, [sp, #20]
   3709c:	9b08      	ldr	r3, [sp, #32]
   3709e:	2b47      	cmp	r3, #71	; 0x47
   370a0:	f040 81ad 	bne.w	373fe <_vfprintf_r+0x976>
   370a4:	1ceb      	adds	r3, r5, #3
   370a6:	db03      	blt.n	370b0 <_vfprintf_r+0x628>
   370a8:	9b04      	ldr	r3, [sp, #16]
   370aa:	42ab      	cmp	r3, r5
   370ac:	f280 81d2 	bge.w	37454 <_vfprintf_r+0x9cc>
   370b0:	9b06      	ldr	r3, [sp, #24]
   370b2:	3b02      	subs	r3, #2
   370b4:	9306      	str	r3, [sp, #24]
   370b6:	9906      	ldr	r1, [sp, #24]
   370b8:	1e6b      	subs	r3, r5, #1
   370ba:	f89d 2018 	ldrb.w	r2, [sp, #24]
   370be:	f021 0120 	bic.w	r1, r1, #32
   370c2:	931c      	str	r3, [sp, #112]	; 0x70
   370c4:	2941      	cmp	r1, #65	; 0x41
   370c6:	bf05      	ittet	eq
   370c8:	320f      	addeq	r2, #15
   370ca:	2101      	moveq	r1, #1
   370cc:	2100      	movne	r1, #0
   370ce:	b2d2      	uxtbeq	r2, r2
   370d0:	2b00      	cmp	r3, #0
   370d2:	bfb8      	it	lt
   370d4:	f1c5 0301 	rsblt	r3, r5, #1
   370d8:	f88d 2078 	strb.w	r2, [sp, #120]	; 0x78
   370dc:	bfb4      	ite	lt
   370de:	222d      	movlt	r2, #45	; 0x2d
   370e0:	222b      	movge	r2, #43	; 0x2b
   370e2:	2b09      	cmp	r3, #9
   370e4:	f88d 2079 	strb.w	r2, [sp, #121]	; 0x79
   370e8:	f340 81a2 	ble.w	37430 <_vfprintf_r+0x9a8>
   370ec:	f10d 0287 	add.w	r2, sp, #135	; 0x87
   370f0:	260a      	movs	r6, #10
   370f2:	4611      	mov	r1, r2
   370f4:	3a01      	subs	r2, #1
   370f6:	fb93 f5f6 	sdiv	r5, r3, r6
   370fa:	fb06 3015 	mls	r0, r6, r5, r3
   370fe:	3030      	adds	r0, #48	; 0x30
   37100:	f801 0c01 	strb.w	r0, [r1, #-1]
   37104:	4618      	mov	r0, r3
   37106:	462b      	mov	r3, r5
   37108:	2863      	cmp	r0, #99	; 0x63
   3710a:	dcf2      	bgt.n	370f2 <_vfprintf_r+0x66a>
   3710c:	3330      	adds	r3, #48	; 0x30
   3710e:	1e88      	subs	r0, r1, #2
   37110:	f10d 0587 	add.w	r5, sp, #135	; 0x87
   37114:	f802 3c01 	strb.w	r3, [r2, #-1]
   37118:	f10d 027a 	add.w	r2, sp, #122	; 0x7a
   3711c:	4603      	mov	r3, r0
   3711e:	42ab      	cmp	r3, r5
   37120:	f0c0 8181 	bcc.w	37426 <_vfprintf_r+0x99e>
   37124:	f10d 0289 	add.w	r2, sp, #137	; 0x89
   37128:	f10d 037a 	add.w	r3, sp, #122	; 0x7a
   3712c:	1a52      	subs	r2, r2, r1
   3712e:	42a8      	cmp	r0, r5
   37130:	bf88      	it	hi
   37132:	2200      	movhi	r2, #0
   37134:	441a      	add	r2, r3
   37136:	ab1e      	add	r3, sp, #120	; 0x78
   37138:	1ad3      	subs	r3, r2, r3
   3713a:	9a05      	ldr	r2, [sp, #20]
   3713c:	9315      	str	r3, [sp, #84]	; 0x54
   3713e:	2a01      	cmp	r2, #1
   37140:	4413      	add	r3, r2
   37142:	9304      	str	r3, [sp, #16]
   37144:	dc02      	bgt.n	3714c <_vfprintf_r+0x6c4>
   37146:	f01b 0f01 	tst.w	fp, #1
   3714a:	d003      	beq.n	37154 <_vfprintf_r+0x6cc>
   3714c:	9b04      	ldr	r3, [sp, #16]
   3714e:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   37150:	4413      	add	r3, r2
   37152:	9304      	str	r3, [sp, #16]
   37154:	f42b 6380 	bic.w	r3, fp, #1024	; 0x400
   37158:	f443 7380 	orr.w	r3, r3, #256	; 0x100
   3715c:	9311      	str	r3, [sp, #68]	; 0x44
   3715e:	2300      	movs	r3, #0
   37160:	461d      	mov	r5, r3
   37162:	e9cd 3308 	strd	r3, r3, [sp, #32]
   37166:	9b18      	ldr	r3, [sp, #96]	; 0x60
   37168:	b113      	cbz	r3, 37170 <_vfprintf_r+0x6e8>
   3716a:	232d      	movs	r3, #45	; 0x2d
   3716c:	f88d 306b 	strb.w	r3, [sp, #107]	; 0x6b
   37170:	f8dd b044 	ldr.w	fp, [sp, #68]	; 0x44
   37174:	2600      	movs	r6, #0
   37176:	9b04      	ldr	r3, [sp, #16]
   37178:	42b3      	cmp	r3, r6
   3717a:	bfb8      	it	lt
   3717c:	4633      	movlt	r3, r6
   3717e:	9311      	str	r3, [sp, #68]	; 0x44
   37180:	f89d 306b 	ldrb.w	r3, [sp, #107]	; 0x6b
   37184:	b113      	cbz	r3, 3718c <_vfprintf_r+0x704>
   37186:	9b11      	ldr	r3, [sp, #68]	; 0x44
   37188:	3301      	adds	r3, #1
   3718a:	9311      	str	r3, [sp, #68]	; 0x44
   3718c:	f01b 0302 	ands.w	r3, fp, #2
   37190:	9317      	str	r3, [sp, #92]	; 0x5c
   37192:	bf1e      	ittt	ne
   37194:	9b11      	ldrne	r3, [sp, #68]	; 0x44
   37196:	3302      	addne	r3, #2
   37198:	9311      	strne	r3, [sp, #68]	; 0x44
   3719a:	f01b 0384 	ands.w	r3, fp, #132	; 0x84
   3719e:	9318      	str	r3, [sp, #96]	; 0x60
   371a0:	d11f      	bne.n	371e2 <_vfprintf_r+0x75a>
   371a2:	e9dd 3210 	ldrd	r3, r2, [sp, #64]	; 0x40
   371a6:	1a9f      	subs	r7, r3, r2
   371a8:	2f00      	cmp	r7, #0
   371aa:	dd1a      	ble.n	371e2 <_vfprintf_r+0x75a>
   371ac:	4836      	ldr	r0, [pc, #216]	; (37288 <_vfprintf_r+0x800>)
   371ae:	2f10      	cmp	r7, #16
   371b0:	f104 0108 	add.w	r1, r4, #8
   371b4:	6020      	str	r0, [r4, #0]
   371b6:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   371ba:	f103 0301 	add.w	r3, r3, #1
   371be:	f300 82ec 	bgt.w	3779a <_vfprintf_r+0xd12>
   371c2:	443a      	add	r2, r7
   371c4:	2b07      	cmp	r3, #7
   371c6:	6067      	str	r7, [r4, #4]
   371c8:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   371cc:	f340 82f8 	ble.w	377c0 <_vfprintf_r+0xd38>
   371d0:	aa22      	add	r2, sp, #136	; 0x88
   371d2:	4649      	mov	r1, r9
   371d4:	4650      	mov	r0, sl
   371d6:	f006 fdc9 	bl	3dd6c <__sprint_r>
   371da:	2800      	cmp	r0, #0
   371dc:	f040 8621 	bne.w	37e22 <_vfprintf_r+0x139a>
   371e0:	ac25      	add	r4, sp, #148	; 0x94
   371e2:	f89d 306b 	ldrb.w	r3, [sp, #107]	; 0x6b
   371e6:	b173      	cbz	r3, 37206 <_vfprintf_r+0x77e>
   371e8:	f10d 036b 	add.w	r3, sp, #107	; 0x6b
   371ec:	6023      	str	r3, [r4, #0]
   371ee:	2301      	movs	r3, #1
   371f0:	6063      	str	r3, [r4, #4]
   371f2:	9b24      	ldr	r3, [sp, #144]	; 0x90
   371f4:	3301      	adds	r3, #1
   371f6:	9324      	str	r3, [sp, #144]	; 0x90
   371f8:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   371fa:	3301      	adds	r3, #1
   371fc:	2b07      	cmp	r3, #7
   371fe:	9323      	str	r3, [sp, #140]	; 0x8c
   37200:	f300 82e0 	bgt.w	377c4 <_vfprintf_r+0xd3c>
   37204:	3408      	adds	r4, #8
   37206:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   37208:	b16b      	cbz	r3, 37226 <_vfprintf_r+0x79e>
   3720a:	ab1b      	add	r3, sp, #108	; 0x6c
   3720c:	6023      	str	r3, [r4, #0]
   3720e:	2302      	movs	r3, #2
   37210:	6063      	str	r3, [r4, #4]
   37212:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37214:	3302      	adds	r3, #2
   37216:	9324      	str	r3, [sp, #144]	; 0x90
   37218:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   3721a:	3301      	adds	r3, #1
   3721c:	2b07      	cmp	r3, #7
   3721e:	9323      	str	r3, [sp, #140]	; 0x8c
   37220:	f300 82da 	bgt.w	377d8 <_vfprintf_r+0xd50>
   37224:	3408      	adds	r4, #8
   37226:	9b18      	ldr	r3, [sp, #96]	; 0x60
   37228:	2b80      	cmp	r3, #128	; 0x80
   3722a:	d11f      	bne.n	3726c <_vfprintf_r+0x7e4>
   3722c:	e9dd 3210 	ldrd	r3, r2, [sp, #64]	; 0x40
   37230:	1a9f      	subs	r7, r3, r2
   37232:	2f00      	cmp	r7, #0
   37234:	dd1a      	ble.n	3726c <_vfprintf_r+0x7e4>
   37236:	4815      	ldr	r0, [pc, #84]	; (3728c <_vfprintf_r+0x804>)
   37238:	2f10      	cmp	r7, #16
   3723a:	f104 0108 	add.w	r1, r4, #8
   3723e:	6020      	str	r0, [r4, #0]
   37240:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   37244:	f103 0301 	add.w	r3, r3, #1
   37248:	f300 82d0 	bgt.w	377ec <_vfprintf_r+0xd64>
   3724c:	6067      	str	r7, [r4, #4]
   3724e:	2b07      	cmp	r3, #7
   37250:	4417      	add	r7, r2
   37252:	e9cd 3723 	strd	r3, r7, [sp, #140]	; 0x8c
   37256:	f340 82dc 	ble.w	37812 <_vfprintf_r+0xd8a>
   3725a:	aa22      	add	r2, sp, #136	; 0x88
   3725c:	4649      	mov	r1, r9
   3725e:	4650      	mov	r0, sl
   37260:	f006 fd84 	bl	3dd6c <__sprint_r>
   37264:	2800      	cmp	r0, #0
   37266:	f040 85dc 	bne.w	37e22 <_vfprintf_r+0x139a>
   3726a:	ac25      	add	r4, sp, #148	; 0x94
   3726c:	9b04      	ldr	r3, [sp, #16]
   3726e:	1af6      	subs	r6, r6, r3
   37270:	2e00      	cmp	r6, #0
   37272:	dd27      	ble.n	372c4 <_vfprintf_r+0x83c>
   37274:	4f05      	ldr	r7, [pc, #20]	; (3728c <_vfprintf_r+0x804>)
   37276:	e00b      	b.n	37290 <_vfprintf_r+0x808>
   37278:	000404f0 	.word	0x000404f0
   3727c:	000404df 	.word	0x000404df
   37280:	40300000 	.word	0x40300000
   37284:	3fe00000 	.word	0x3fe00000
   37288:	0004054a 	.word	0x0004054a
   3728c:	0004055a 	.word	0x0004055a
   37290:	2e10      	cmp	r6, #16
   37292:	f104 0108 	add.w	r1, r4, #8
   37296:	6027      	str	r7, [r4, #0]
   37298:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   3729c:	f103 0301 	add.w	r3, r3, #1
   372a0:	f300 82b9 	bgt.w	37816 <_vfprintf_r+0xd8e>
   372a4:	6066      	str	r6, [r4, #4]
   372a6:	2b07      	cmp	r3, #7
   372a8:	4416      	add	r6, r2
   372aa:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   372ae:	f340 82c5 	ble.w	3783c <_vfprintf_r+0xdb4>
   372b2:	aa22      	add	r2, sp, #136	; 0x88
   372b4:	4649      	mov	r1, r9
   372b6:	4650      	mov	r0, sl
   372b8:	f006 fd58 	bl	3dd6c <__sprint_r>
   372bc:	2800      	cmp	r0, #0
   372be:	f040 85b0 	bne.w	37e22 <_vfprintf_r+0x139a>
   372c2:	ac25      	add	r4, sp, #148	; 0x94
   372c4:	f41b 7f80 	tst.w	fp, #256	; 0x100
   372c8:	9e24      	ldr	r6, [sp, #144]	; 0x90
   372ca:	f040 82bd 	bne.w	37848 <_vfprintf_r+0xdc0>
   372ce:	9b04      	ldr	r3, [sp, #16]
   372d0:	f8c4 8000 	str.w	r8, [r4]
   372d4:	441e      	add	r6, r3
   372d6:	6063      	str	r3, [r4, #4]
   372d8:	9624      	str	r6, [sp, #144]	; 0x90
   372da:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   372dc:	3301      	adds	r3, #1
   372de:	2b07      	cmp	r3, #7
   372e0:	9323      	str	r3, [sp, #140]	; 0x8c
   372e2:	f300 82f6 	bgt.w	378d2 <_vfprintf_r+0xe4a>
   372e6:	3408      	adds	r4, #8
   372e8:	f01b 0f04 	tst.w	fp, #4
   372ec:	f040 857b 	bne.w	37de6 <_vfprintf_r+0x135e>
   372f0:	9911      	ldr	r1, [sp, #68]	; 0x44
   372f2:	e9dd 320f 	ldrd	r3, r2, [sp, #60]	; 0x3c
   372f6:	428a      	cmp	r2, r1
   372f8:	bfac      	ite	ge
   372fa:	189b      	addge	r3, r3, r2
   372fc:	185b      	addlt	r3, r3, r1
   372fe:	930f      	str	r3, [sp, #60]	; 0x3c
   37300:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37302:	b13b      	cbz	r3, 37314 <_vfprintf_r+0x88c>
   37304:	aa22      	add	r2, sp, #136	; 0x88
   37306:	4649      	mov	r1, r9
   37308:	4650      	mov	r0, sl
   3730a:	f006 fd2f 	bl	3dd6c <__sprint_r>
   3730e:	2800      	cmp	r0, #0
   37310:	f040 8587 	bne.w	37e22 <_vfprintf_r+0x139a>
   37314:	2300      	movs	r3, #0
   37316:	9323      	str	r3, [sp, #140]	; 0x8c
   37318:	9b03      	ldr	r3, [sp, #12]
   3731a:	2b00      	cmp	r3, #0
   3731c:	f040 859d 	bne.w	37e5a <_vfprintf_r+0x13d2>
   37320:	ac25      	add	r4, sp, #148	; 0x94
   37322:	e0ec      	b.n	374fe <_vfprintf_r+0xa76>
   37324:	9003      	str	r0, [sp, #12]
   37326:	e630      	b.n	36f8a <_vfprintf_r+0x502>
   37328:	2306      	movs	r3, #6
   3732a:	9003      	str	r0, [sp, #12]
   3732c:	e61d      	b.n	36f6a <_vfprintf_r+0x4e2>
   3732e:	9003      	str	r0, [sp, #12]
   37330:	e62b      	b.n	36f8a <_vfprintf_r+0x502>
   37332:	ed9d 7b0c 	vldr	d7, [sp, #48]	; 0x30
   37336:	2300      	movs	r3, #0
   37338:	eeb0 8a47 	vmov.f32	s16, s14
   3733c:	eef0 8a67 	vmov.f32	s17, s15
   37340:	e633      	b.n	36faa <_vfprintf_r+0x522>
   37342:	f802 0c01 	strb.w	r0, [r2, #-1]
   37346:	e695      	b.n	37074 <_vfprintf_r+0x5ec>
   37348:	f803 0b01 	strb.w	r0, [r3], #1
   3734c:	1aca      	subs	r2, r1, r3
   3734e:	2a00      	cmp	r2, #0
   37350:	dafa      	bge.n	37348 <_vfprintf_r+0x8c0>
   37352:	9a09      	ldr	r2, [sp, #36]	; 0x24
   37354:	9b09      	ldr	r3, [sp, #36]	; 0x24
   37356:	3201      	adds	r2, #1
   37358:	f103 0301 	add.w	r3, r3, #1
   3735c:	bfb8      	it	lt
   3735e:	2300      	movlt	r3, #0
   37360:	441d      	add	r5, r3
   37362:	e697      	b.n	37094 <_vfprintf_r+0x60c>
   37364:	9a09      	ldr	r2, [sp, #36]	; 0x24
   37366:	462b      	mov	r3, r5
   37368:	2030      	movs	r0, #48	; 0x30
   3736a:	18a9      	adds	r1, r5, r2
   3736c:	e7ee      	b.n	3734c <_vfprintf_r+0x8c4>
   3736e:	9b08      	ldr	r3, [sp, #32]
   37370:	2b46      	cmp	r3, #70	; 0x46
   37372:	d005      	beq.n	37380 <_vfprintf_r+0x8f8>
   37374:	2b45      	cmp	r3, #69	; 0x45
   37376:	d11b      	bne.n	373b0 <_vfprintf_r+0x928>
   37378:	9b04      	ldr	r3, [sp, #16]
   3737a:	1c5d      	adds	r5, r3, #1
   3737c:	2102      	movs	r1, #2
   3737e:	e001      	b.n	37384 <_vfprintf_r+0x8fc>
   37380:	9d04      	ldr	r5, [sp, #16]
   37382:	2103      	movs	r1, #3
   37384:	ab20      	add	r3, sp, #128	; 0x80
   37386:	eeb0 0a48 	vmov.f32	s0, s16
   3738a:	eef0 0a68 	vmov.f32	s1, s17
   3738e:	462a      	mov	r2, r5
   37390:	4650      	mov	r0, sl
   37392:	9301      	str	r3, [sp, #4]
   37394:	ab1d      	add	r3, sp, #116	; 0x74
   37396:	9300      	str	r3, [sp, #0]
   37398:	ab1c      	add	r3, sp, #112	; 0x70
   3739a:	f000 ff8a 	bl	382b2 <_dtoa_r>
   3739e:	9b08      	ldr	r3, [sp, #32]
   373a0:	4680      	mov	r8, r0
   373a2:	2b47      	cmp	r3, #71	; 0x47
   373a4:	d106      	bne.n	373b4 <_vfprintf_r+0x92c>
   373a6:	f01b 0f01 	tst.w	fp, #1
   373aa:	d103      	bne.n	373b4 <_vfprintf_r+0x92c>
   373ac:	9d20      	ldr	r5, [sp, #128]	; 0x80
   373ae:	e671      	b.n	37094 <_vfprintf_r+0x60c>
   373b0:	9d04      	ldr	r5, [sp, #16]
   373b2:	e7e3      	b.n	3737c <_vfprintf_r+0x8f4>
   373b4:	9b08      	ldr	r3, [sp, #32]
   373b6:	eb08 0605 	add.w	r6, r8, r5
   373ba:	2b46      	cmp	r3, #70	; 0x46
   373bc:	d10f      	bne.n	373de <_vfprintf_r+0x956>
   373be:	f898 3000 	ldrb.w	r3, [r8]
   373c2:	2b30      	cmp	r3, #48	; 0x30
   373c4:	d109      	bne.n	373da <_vfprintf_r+0x952>
   373c6:	2200      	movs	r2, #0
   373c8:	2300      	movs	r3, #0
   373ca:	ec51 0b18 	vmov	r0, r1, d8
   373ce:	f008 fd1d 	bl	3fe0c <__aeabi_dcmpeq>
   373d2:	b910      	cbnz	r0, 373da <_vfprintf_r+0x952>
   373d4:	f1c5 0501 	rsb	r5, r5, #1
   373d8:	951c      	str	r5, [sp, #112]	; 0x70
   373da:	9b1c      	ldr	r3, [sp, #112]	; 0x70
   373dc:	441e      	add	r6, r3
   373de:	2200      	movs	r2, #0
   373e0:	2300      	movs	r3, #0
   373e2:	ec51 0b18 	vmov	r0, r1, d8
   373e6:	f008 fd11 	bl	3fe0c <__aeabi_dcmpeq>
   373ea:	b100      	cbz	r0, 373ee <_vfprintf_r+0x966>
   373ec:	9620      	str	r6, [sp, #128]	; 0x80
   373ee:	2230      	movs	r2, #48	; 0x30
   373f0:	9b20      	ldr	r3, [sp, #128]	; 0x80
   373f2:	429e      	cmp	r6, r3
   373f4:	d9da      	bls.n	373ac <_vfprintf_r+0x924>
   373f6:	1c59      	adds	r1, r3, #1
   373f8:	9120      	str	r1, [sp, #128]	; 0x80
   373fa:	701a      	strb	r2, [r3, #0]
   373fc:	e7f8      	b.n	373f0 <_vfprintf_r+0x968>
   373fe:	9b08      	ldr	r3, [sp, #32]
   37400:	2b46      	cmp	r3, #70	; 0x46
   37402:	f47f ae58 	bne.w	370b6 <_vfprintf_r+0x62e>
   37406:	f00b 0301 	and.w	r3, fp, #1
   3740a:	9a04      	ldr	r2, [sp, #16]
   3740c:	2d00      	cmp	r5, #0
   3740e:	ea43 0302 	orr.w	r3, r3, r2
   37412:	dd1a      	ble.n	3744a <_vfprintf_r+0x9c2>
   37414:	2b00      	cmp	r3, #0
   37416:	d034      	beq.n	37482 <_vfprintf_r+0x9fa>
   37418:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3741a:	18eb      	adds	r3, r5, r3
   3741c:	441a      	add	r2, r3
   3741e:	9204      	str	r2, [sp, #16]
   37420:	2366      	movs	r3, #102	; 0x66
   37422:	9306      	str	r3, [sp, #24]
   37424:	e033      	b.n	3748e <_vfprintf_r+0xa06>
   37426:	f813 6b01 	ldrb.w	r6, [r3], #1
   3742a:	f802 6b01 	strb.w	r6, [r2], #1
   3742e:	e676      	b.n	3711e <_vfprintf_r+0x696>
   37430:	b941      	cbnz	r1, 37444 <_vfprintf_r+0x9bc>
   37432:	2230      	movs	r2, #48	; 0x30
   37434:	f88d 207a 	strb.w	r2, [sp, #122]	; 0x7a
   37438:	f10d 027b 	add.w	r2, sp, #123	; 0x7b
   3743c:	3330      	adds	r3, #48	; 0x30
   3743e:	f802 3b01 	strb.w	r3, [r2], #1
   37442:	e678      	b.n	37136 <_vfprintf_r+0x6ae>
   37444:	f10d 027a 	add.w	r2, sp, #122	; 0x7a
   37448:	e7f8      	b.n	3743c <_vfprintf_r+0x9b4>
   3744a:	b1e3      	cbz	r3, 37486 <_vfprintf_r+0x9fe>
   3744c:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3744e:	9a04      	ldr	r2, [sp, #16]
   37450:	3301      	adds	r3, #1
   37452:	e7e3      	b.n	3741c <_vfprintf_r+0x994>
   37454:	9b05      	ldr	r3, [sp, #20]
   37456:	42ab      	cmp	r3, r5
   37458:	dc07      	bgt.n	3746a <_vfprintf_r+0x9e2>
   3745a:	f01b 0f01 	tst.w	fp, #1
   3745e:	d02d      	beq.n	374bc <_vfprintf_r+0xa34>
   37460:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   37462:	18eb      	adds	r3, r5, r3
   37464:	9304      	str	r3, [sp, #16]
   37466:	2367      	movs	r3, #103	; 0x67
   37468:	e7db      	b.n	37422 <_vfprintf_r+0x99a>
   3746a:	9b05      	ldr	r3, [sp, #20]
   3746c:	2d00      	cmp	r5, #0
   3746e:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   37470:	4413      	add	r3, r2
   37472:	9304      	str	r3, [sp, #16]
   37474:	dcf7      	bgt.n	37466 <_vfprintf_r+0x9de>
   37476:	9a04      	ldr	r2, [sp, #16]
   37478:	f1c5 0301 	rsb	r3, r5, #1
   3747c:	441a      	add	r2, r3
   3747e:	9204      	str	r2, [sp, #16]
   37480:	e7f1      	b.n	37466 <_vfprintf_r+0x9de>
   37482:	9504      	str	r5, [sp, #16]
   37484:	e7cc      	b.n	37420 <_vfprintf_r+0x998>
   37486:	2366      	movs	r3, #102	; 0x66
   37488:	9306      	str	r3, [sp, #24]
   3748a:	2301      	movs	r3, #1
   3748c:	9304      	str	r3, [sp, #16]
   3748e:	f41b 6380 	ands.w	r3, fp, #1024	; 0x400
   37492:	9309      	str	r3, [sp, #36]	; 0x24
   37494:	d025      	beq.n	374e2 <_vfprintf_r+0xa5a>
   37496:	2300      	movs	r3, #0
   37498:	2d00      	cmp	r5, #0
   3749a:	e9cd 3308 	strd	r3, r3, [sp, #32]
   3749e:	f77f ae62 	ble.w	37166 <_vfprintf_r+0x6de>
   374a2:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   374a4:	781b      	ldrb	r3, [r3, #0]
   374a6:	2bff      	cmp	r3, #255	; 0xff
   374a8:	d10a      	bne.n	374c0 <_vfprintf_r+0xa38>
   374aa:	e9dd 2308 	ldrd	r2, r3, [sp, #32]
   374ae:	9912      	ldr	r1, [sp, #72]	; 0x48
   374b0:	4413      	add	r3, r2
   374b2:	9a04      	ldr	r2, [sp, #16]
   374b4:	fb01 2303 	mla	r3, r1, r3, r2
   374b8:	9304      	str	r3, [sp, #16]
   374ba:	e654      	b.n	37166 <_vfprintf_r+0x6de>
   374bc:	9504      	str	r5, [sp, #16]
   374be:	e7d2      	b.n	37466 <_vfprintf_r+0x9de>
   374c0:	42ab      	cmp	r3, r5
   374c2:	daf2      	bge.n	374aa <_vfprintf_r+0xa22>
   374c4:	1aed      	subs	r5, r5, r3
   374c6:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   374c8:	785b      	ldrb	r3, [r3, #1]
   374ca:	b133      	cbz	r3, 374da <_vfprintf_r+0xa52>
   374cc:	9b09      	ldr	r3, [sp, #36]	; 0x24
   374ce:	3301      	adds	r3, #1
   374d0:	9309      	str	r3, [sp, #36]	; 0x24
   374d2:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   374d4:	3301      	adds	r3, #1
   374d6:	930a      	str	r3, [sp, #40]	; 0x28
   374d8:	e7e3      	b.n	374a2 <_vfprintf_r+0xa1a>
   374da:	9b08      	ldr	r3, [sp, #32]
   374dc:	3301      	adds	r3, #1
   374de:	9308      	str	r3, [sp, #32]
   374e0:	e7df      	b.n	374a2 <_vfprintf_r+0xa1a>
   374e2:	9b09      	ldr	r3, [sp, #36]	; 0x24
   374e4:	9308      	str	r3, [sp, #32]
   374e6:	e63e      	b.n	37166 <_vfprintf_r+0x6de>
   374e8:	1d33      	adds	r3, r6, #4
   374ea:	f01b 0f20 	tst.w	fp, #32
   374ee:	9307      	str	r3, [sp, #28]
   374f0:	d00a      	beq.n	37508 <_vfprintf_r+0xa80>
   374f2:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
   374f4:	6833      	ldr	r3, [r6, #0]
   374f6:	17d2      	asrs	r2, r2, #31
   374f8:	990f      	ldr	r1, [sp, #60]	; 0x3c
   374fa:	e9c3 1200 	strd	r1, r2, [r3]
   374fe:	9e07      	ldr	r6, [sp, #28]
   37500:	f8dd 802c 	ldr.w	r8, [sp, #44]	; 0x2c
   37504:	f7ff bb46 	b.w	36b94 <_vfprintf_r+0x10c>
   37508:	f01b 0f10 	tst.w	fp, #16
   3750c:	d003      	beq.n	37516 <_vfprintf_r+0xa8e>
   3750e:	6833      	ldr	r3, [r6, #0]
   37510:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
   37512:	601a      	str	r2, [r3, #0]
   37514:	e7f3      	b.n	374fe <_vfprintf_r+0xa76>
   37516:	f01b 0f40 	tst.w	fp, #64	; 0x40
   3751a:	d003      	beq.n	37524 <_vfprintf_r+0xa9c>
   3751c:	6833      	ldr	r3, [r6, #0]
   3751e:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
   37520:	801a      	strh	r2, [r3, #0]
   37522:	e7ec      	b.n	374fe <_vfprintf_r+0xa76>
   37524:	f41b 7f00 	tst.w	fp, #512	; 0x200
   37528:	d0f1      	beq.n	3750e <_vfprintf_r+0xa86>
   3752a:	6833      	ldr	r3, [r6, #0]
   3752c:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
   3752e:	701a      	strb	r2, [r3, #0]
   37530:	e7e5      	b.n	374fe <_vfprintf_r+0xa76>
   37532:	f04b 0b10 	orr.w	fp, fp, #16
   37536:	f01b 0320 	ands.w	r3, fp, #32
   3753a:	d01f      	beq.n	3757c <_vfprintf_r+0xaf4>
   3753c:	3607      	adds	r6, #7
   3753e:	f026 0307 	bic.w	r3, r6, #7
   37542:	461a      	mov	r2, r3
   37544:	685d      	ldr	r5, [r3, #4]
   37546:	f852 6b08 	ldr.w	r6, [r2], #8
   3754a:	9207      	str	r2, [sp, #28]
   3754c:	f42b 6b80 	bic.w	fp, fp, #1024	; 0x400
   37550:	2300      	movs	r3, #0
   37552:	2200      	movs	r2, #0
   37554:	f88d 206b 	strb.w	r2, [sp, #107]	; 0x6b
   37558:	9a04      	ldr	r2, [sp, #16]
   3755a:	3201      	adds	r2, #1
   3755c:	f000 8492 	beq.w	37e84 <_vfprintf_r+0x13fc>
   37560:	ea56 0205 	orrs.w	r2, r6, r5
   37564:	f02b 0780 	bic.w	r7, fp, #128	; 0x80
   37568:	f040 8491 	bne.w	37e8e <_vfprintf_r+0x1406>
   3756c:	9a04      	ldr	r2, [sp, #16]
   3756e:	2a00      	cmp	r2, #0
   37570:	f000 80f9 	beq.w	37766 <_vfprintf_r+0xcde>
   37574:	2b01      	cmp	r3, #1
   37576:	f040 848d 	bne.w	37e94 <_vfprintf_r+0x140c>
   3757a:	e09e      	b.n	376ba <_vfprintf_r+0xc32>
   3757c:	4632      	mov	r2, r6
   3757e:	f01b 0510 	ands.w	r5, fp, #16
   37582:	f852 6b04 	ldr.w	r6, [r2], #4
   37586:	9207      	str	r2, [sp, #28]
   37588:	d001      	beq.n	3758e <_vfprintf_r+0xb06>
   3758a:	461d      	mov	r5, r3
   3758c:	e7de      	b.n	3754c <_vfprintf_r+0xac4>
   3758e:	f01b 0340 	ands.w	r3, fp, #64	; 0x40
   37592:	d001      	beq.n	37598 <_vfprintf_r+0xb10>
   37594:	b2b6      	uxth	r6, r6
   37596:	e7d9      	b.n	3754c <_vfprintf_r+0xac4>
   37598:	f41b 7500 	ands.w	r5, fp, #512	; 0x200
   3759c:	d0d6      	beq.n	3754c <_vfprintf_r+0xac4>
   3759e:	b2f6      	uxtb	r6, r6
   375a0:	e7f3      	b.n	3758a <_vfprintf_r+0xb02>
   375a2:	4633      	mov	r3, r6
   375a4:	2278      	movs	r2, #120	; 0x78
   375a6:	2500      	movs	r5, #0
   375a8:	f04b 0b02 	orr.w	fp, fp, #2
   375ac:	f853 6b04 	ldr.w	r6, [r3], #4
   375b0:	9206      	str	r2, [sp, #24]
   375b2:	9307      	str	r3, [sp, #28]
   375b4:	f647 0330 	movw	r3, #30768	; 0x7830
   375b8:	f8ad 306c 	strh.w	r3, [sp, #108]	; 0x6c
   375bc:	4ba0      	ldr	r3, [pc, #640]	; (37840 <_vfprintf_r+0xdb8>)
   375be:	9316      	str	r3, [sp, #88]	; 0x58
   375c0:	2302      	movs	r3, #2
   375c2:	e7c6      	b.n	37552 <_vfprintf_r+0xaca>
   375c4:	4633      	mov	r3, r6
   375c6:	2500      	movs	r5, #0
   375c8:	f853 8b04 	ldr.w	r8, [r3], #4
   375cc:	f88d 506b 	strb.w	r5, [sp, #107]	; 0x6b
   375d0:	9307      	str	r3, [sp, #28]
   375d2:	9b04      	ldr	r3, [sp, #16]
   375d4:	1c5e      	adds	r6, r3, #1
   375d6:	d010      	beq.n	375fa <_vfprintf_r+0xb72>
   375d8:	461a      	mov	r2, r3
   375da:	4629      	mov	r1, r5
   375dc:	4640      	mov	r0, r8
   375de:	f002 fbe3 	bl	39da8 <memchr>
   375e2:	9003      	str	r0, [sp, #12]
   375e4:	2800      	cmp	r0, #0
   375e6:	f000 80d5 	beq.w	37794 <_vfprintf_r+0xd0c>
   375ea:	eba0 0308 	sub.w	r3, r0, r8
   375ee:	462e      	mov	r6, r5
   375f0:	e9cd 5303 	strd	r5, r3, [sp, #12]
   375f4:	e9cd 5508 	strd	r5, r5, [sp, #32]
   375f8:	e5bd      	b.n	37176 <_vfprintf_r+0x6ee>
   375fa:	4640      	mov	r0, r8
   375fc:	f7fc fb98 	bl	33d30 <strlen>
   37600:	e9cd 5003 	strd	r5, r0, [sp, #12]
   37604:	e45a      	b.n	36ebc <_vfprintf_r+0x434>
   37606:	f04b 0b10 	orr.w	fp, fp, #16
   3760a:	f01b 0320 	ands.w	r3, fp, #32
   3760e:	d009      	beq.n	37624 <_vfprintf_r+0xb9c>
   37610:	3607      	adds	r6, #7
   37612:	f026 0307 	bic.w	r3, r6, #7
   37616:	461a      	mov	r2, r3
   37618:	685d      	ldr	r5, [r3, #4]
   3761a:	f852 6b08 	ldr.w	r6, [r2], #8
   3761e:	9207      	str	r2, [sp, #28]
   37620:	2301      	movs	r3, #1
   37622:	e796      	b.n	37552 <_vfprintf_r+0xaca>
   37624:	4632      	mov	r2, r6
   37626:	f01b 0510 	ands.w	r5, fp, #16
   3762a:	f852 6b04 	ldr.w	r6, [r2], #4
   3762e:	9207      	str	r2, [sp, #28]
   37630:	d001      	beq.n	37636 <_vfprintf_r+0xbae>
   37632:	461d      	mov	r5, r3
   37634:	e7f4      	b.n	37620 <_vfprintf_r+0xb98>
   37636:	f01b 0340 	ands.w	r3, fp, #64	; 0x40
   3763a:	d001      	beq.n	37640 <_vfprintf_r+0xbb8>
   3763c:	b2b6      	uxth	r6, r6
   3763e:	e7ef      	b.n	37620 <_vfprintf_r+0xb98>
   37640:	f41b 7500 	ands.w	r5, fp, #512	; 0x200
   37644:	d0ec      	beq.n	37620 <_vfprintf_r+0xb98>
   37646:	b2f6      	uxtb	r6, r6
   37648:	e7f3      	b.n	37632 <_vfprintf_r+0xbaa>
   3764a:	4b7e      	ldr	r3, [pc, #504]	; (37844 <_vfprintf_r+0xdbc>)
   3764c:	9316      	str	r3, [sp, #88]	; 0x58
   3764e:	f01b 0320 	ands.w	r3, fp, #32
   37652:	d01b      	beq.n	3768c <_vfprintf_r+0xc04>
   37654:	3607      	adds	r6, #7
   37656:	f026 0307 	bic.w	r3, r6, #7
   3765a:	461a      	mov	r2, r3
   3765c:	685d      	ldr	r5, [r3, #4]
   3765e:	f852 6b08 	ldr.w	r6, [r2], #8
   37662:	9207      	str	r2, [sp, #28]
   37664:	f01b 0f01 	tst.w	fp, #1
   37668:	d00a      	beq.n	37680 <_vfprintf_r+0xbf8>
   3766a:	ea56 0305 	orrs.w	r3, r6, r5
   3766e:	d007      	beq.n	37680 <_vfprintf_r+0xbf8>
   37670:	2330      	movs	r3, #48	; 0x30
   37672:	f04b 0b02 	orr.w	fp, fp, #2
   37676:	f88d 306c 	strb.w	r3, [sp, #108]	; 0x6c
   3767a:	9b06      	ldr	r3, [sp, #24]
   3767c:	f88d 306d 	strb.w	r3, [sp, #109]	; 0x6d
   37680:	f42b 6b80 	bic.w	fp, fp, #1024	; 0x400
   37684:	2302      	movs	r3, #2
   37686:	e764      	b.n	37552 <_vfprintf_r+0xaca>
   37688:	4b6d      	ldr	r3, [pc, #436]	; (37840 <_vfprintf_r+0xdb8>)
   3768a:	e7df      	b.n	3764c <_vfprintf_r+0xbc4>
   3768c:	4632      	mov	r2, r6
   3768e:	f01b 0510 	ands.w	r5, fp, #16
   37692:	f852 6b04 	ldr.w	r6, [r2], #4
   37696:	9207      	str	r2, [sp, #28]
   37698:	d001      	beq.n	3769e <_vfprintf_r+0xc16>
   3769a:	461d      	mov	r5, r3
   3769c:	e7e2      	b.n	37664 <_vfprintf_r+0xbdc>
   3769e:	f01b 0340 	ands.w	r3, fp, #64	; 0x40
   376a2:	d001      	beq.n	376a8 <_vfprintf_r+0xc20>
   376a4:	b2b6      	uxth	r6, r6
   376a6:	e7dd      	b.n	37664 <_vfprintf_r+0xbdc>
   376a8:	f41b 7500 	ands.w	r5, fp, #512	; 0x200
   376ac:	d0da      	beq.n	37664 <_vfprintf_r+0xbdc>
   376ae:	b2f6      	uxtb	r6, r6
   376b0:	e7f3      	b.n	3769a <_vfprintf_r+0xc12>
   376b2:	2e0a      	cmp	r6, #10
   376b4:	f175 0300 	sbcs.w	r3, r5, #0
   376b8:	d206      	bcs.n	376c8 <_vfprintf_r+0xc40>
   376ba:	3630      	adds	r6, #48	; 0x30
   376bc:	f88d 6137 	strb.w	r6, [sp, #311]	; 0x137
   376c0:	f20d 1837 	addw	r8, sp, #311	; 0x137
   376c4:	f000 bc01 	b.w	37eca <_vfprintf_r+0x1442>
   376c8:	2300      	movs	r3, #0
   376ca:	f50d 7b9c 	add.w	fp, sp, #312	; 0x138
   376ce:	9305      	str	r3, [sp, #20]
   376d0:	f407 6380 	and.w	r3, r7, #1024	; 0x400
   376d4:	9303      	str	r3, [sp, #12]
   376d6:	220a      	movs	r2, #10
   376d8:	2300      	movs	r3, #0
   376da:	4630      	mov	r0, r6
   376dc:	4629      	mov	r1, r5
   376de:	f008 fc75 	bl	3ffcc <__aeabi_uldivmod>
   376e2:	9b05      	ldr	r3, [sp, #20]
   376e4:	3230      	adds	r2, #48	; 0x30
   376e6:	f10b 38ff 	add.w	r8, fp, #4294967295	; 0xffffffff
   376ea:	3301      	adds	r3, #1
   376ec:	f80b 2c01 	strb.w	r2, [fp, #-1]
   376f0:	9305      	str	r3, [sp, #20]
   376f2:	9b03      	ldr	r3, [sp, #12]
   376f4:	b1d3      	cbz	r3, 3772c <_vfprintf_r+0xca4>
   376f6:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   376f8:	9a05      	ldr	r2, [sp, #20]
   376fa:	781b      	ldrb	r3, [r3, #0]
   376fc:	429a      	cmp	r2, r3
   376fe:	d115      	bne.n	3772c <_vfprintf_r+0xca4>
   37700:	2aff      	cmp	r2, #255	; 0xff
   37702:	d013      	beq.n	3772c <_vfprintf_r+0xca4>
   37704:	2e0a      	cmp	r6, #10
   37706:	f175 0300 	sbcs.w	r3, r5, #0
   3770a:	d30f      	bcc.n	3772c <_vfprintf_r+0xca4>
   3770c:	9b12      	ldr	r3, [sp, #72]	; 0x48
   3770e:	9914      	ldr	r1, [sp, #80]	; 0x50
   37710:	eba8 0803 	sub.w	r8, r8, r3
   37714:	461a      	mov	r2, r3
   37716:	4640      	mov	r0, r8
   37718:	f7fc fb87 	bl	33e2a <strncpy>
   3771c:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   3771e:	785b      	ldrb	r3, [r3, #1]
   37720:	b11b      	cbz	r3, 3772a <_vfprintf_r+0xca2>
   37722:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   37724:	3301      	adds	r3, #1
   37726:	930a      	str	r3, [sp, #40]	; 0x28
   37728:	2300      	movs	r3, #0
   3772a:	9305      	str	r3, [sp, #20]
   3772c:	2300      	movs	r3, #0
   3772e:	220a      	movs	r2, #10
   37730:	4630      	mov	r0, r6
   37732:	4629      	mov	r1, r5
   37734:	f008 fc4a 	bl	3ffcc <__aeabi_uldivmod>
   37738:	2e0a      	cmp	r6, #10
   3773a:	f175 0300 	sbcs.w	r3, r5, #0
   3773e:	f0c0 83c4 	bcc.w	37eca <_vfprintf_r+0x1442>
   37742:	4606      	mov	r6, r0
   37744:	460d      	mov	r5, r1
   37746:	46c3      	mov	fp, r8
   37748:	e7c5      	b.n	376d6 <_vfprintf_r+0xc4e>
   3774a:	f006 030f 	and.w	r3, r6, #15
   3774e:	9a16      	ldr	r2, [sp, #88]	; 0x58
   37750:	0936      	lsrs	r6, r6, #4
   37752:	5cd3      	ldrb	r3, [r2, r3]
   37754:	ea46 7605 	orr.w	r6, r6, r5, lsl #28
   37758:	092d      	lsrs	r5, r5, #4
   3775a:	f808 3d01 	strb.w	r3, [r8, #-1]!
   3775e:	ea56 0305 	orrs.w	r3, r6, r5
   37762:	d1f2      	bne.n	3774a <_vfprintf_r+0xcc2>
   37764:	e3b1      	b.n	37eca <_vfprintf_r+0x1442>
   37766:	b933      	cbnz	r3, 37776 <_vfprintf_r+0xcee>
   37768:	f01b 0f01 	tst.w	fp, #1
   3776c:	d003      	beq.n	37776 <_vfprintf_r+0xcee>
   3776e:	2330      	movs	r3, #48	; 0x30
   37770:	f88d 3137 	strb.w	r3, [sp, #311]	; 0x137
   37774:	e7a4      	b.n	376c0 <_vfprintf_r+0xc38>
   37776:	f50d 789c 	add.w	r8, sp, #312	; 0x138
   3777a:	e3a6      	b.n	37eca <_vfprintf_r+0x1442>
   3777c:	9b06      	ldr	r3, [sp, #24]
   3777e:	2b00      	cmp	r3, #0
   37780:	f000 8371 	beq.w	37e66 <_vfprintf_r+0x13de>
   37784:	2000      	movs	r0, #0
   37786:	f88d 30d4 	strb.w	r3, [sp, #212]	; 0xd4
   3778a:	9607      	str	r6, [sp, #28]
   3778c:	f88d 006b 	strb.w	r0, [sp, #107]	; 0x6b
   37790:	f7ff bb23 	b.w	36dda <_vfprintf_r+0x352>
   37794:	9e03      	ldr	r6, [sp, #12]
   37796:	f7ff bb92 	b.w	36ebe <_vfprintf_r+0x436>
   3779a:	2010      	movs	r0, #16
   3779c:	2b07      	cmp	r3, #7
   3779e:	4402      	add	r2, r0
   377a0:	6060      	str	r0, [r4, #4]
   377a2:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   377a6:	dd08      	ble.n	377ba <_vfprintf_r+0xd32>
   377a8:	aa22      	add	r2, sp, #136	; 0x88
   377aa:	4649      	mov	r1, r9
   377ac:	4650      	mov	r0, sl
   377ae:	f006 fadd 	bl	3dd6c <__sprint_r>
   377b2:	2800      	cmp	r0, #0
   377b4:	f040 8335 	bne.w	37e22 <_vfprintf_r+0x139a>
   377b8:	a925      	add	r1, sp, #148	; 0x94
   377ba:	3f10      	subs	r7, #16
   377bc:	460c      	mov	r4, r1
   377be:	e4f5      	b.n	371ac <_vfprintf_r+0x724>
   377c0:	460c      	mov	r4, r1
   377c2:	e50e      	b.n	371e2 <_vfprintf_r+0x75a>
   377c4:	aa22      	add	r2, sp, #136	; 0x88
   377c6:	4649      	mov	r1, r9
   377c8:	4650      	mov	r0, sl
   377ca:	f006 facf 	bl	3dd6c <__sprint_r>
   377ce:	2800      	cmp	r0, #0
   377d0:	f040 8327 	bne.w	37e22 <_vfprintf_r+0x139a>
   377d4:	ac25      	add	r4, sp, #148	; 0x94
   377d6:	e516      	b.n	37206 <_vfprintf_r+0x77e>
   377d8:	aa22      	add	r2, sp, #136	; 0x88
   377da:	4649      	mov	r1, r9
   377dc:	4650      	mov	r0, sl
   377de:	f006 fac5 	bl	3dd6c <__sprint_r>
   377e2:	2800      	cmp	r0, #0
   377e4:	f040 831d 	bne.w	37e22 <_vfprintf_r+0x139a>
   377e8:	ac25      	add	r4, sp, #148	; 0x94
   377ea:	e51c      	b.n	37226 <_vfprintf_r+0x79e>
   377ec:	2010      	movs	r0, #16
   377ee:	2b07      	cmp	r3, #7
   377f0:	4402      	add	r2, r0
   377f2:	6060      	str	r0, [r4, #4]
   377f4:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   377f8:	dd08      	ble.n	3780c <_vfprintf_r+0xd84>
   377fa:	aa22      	add	r2, sp, #136	; 0x88
   377fc:	4649      	mov	r1, r9
   377fe:	4650      	mov	r0, sl
   37800:	f006 fab4 	bl	3dd6c <__sprint_r>
   37804:	2800      	cmp	r0, #0
   37806:	f040 830c 	bne.w	37e22 <_vfprintf_r+0x139a>
   3780a:	a925      	add	r1, sp, #148	; 0x94
   3780c:	3f10      	subs	r7, #16
   3780e:	460c      	mov	r4, r1
   37810:	e511      	b.n	37236 <_vfprintf_r+0x7ae>
   37812:	460c      	mov	r4, r1
   37814:	e52a      	b.n	3726c <_vfprintf_r+0x7e4>
   37816:	2010      	movs	r0, #16
   37818:	2b07      	cmp	r3, #7
   3781a:	4402      	add	r2, r0
   3781c:	6060      	str	r0, [r4, #4]
   3781e:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   37822:	dd08      	ble.n	37836 <_vfprintf_r+0xdae>
   37824:	aa22      	add	r2, sp, #136	; 0x88
   37826:	4649      	mov	r1, r9
   37828:	4650      	mov	r0, sl
   3782a:	f006 fa9f 	bl	3dd6c <__sprint_r>
   3782e:	2800      	cmp	r0, #0
   37830:	f040 82f7 	bne.w	37e22 <_vfprintf_r+0x139a>
   37834:	a925      	add	r1, sp, #148	; 0x94
   37836:	3e10      	subs	r6, #16
   37838:	460c      	mov	r4, r1
   3783a:	e529      	b.n	37290 <_vfprintf_r+0x808>
   3783c:	460c      	mov	r4, r1
   3783e:	e541      	b.n	372c4 <_vfprintf_r+0x83c>
   37840:	000404df 	.word	0x000404df
   37844:	000404f0 	.word	0x000404f0
   37848:	9b06      	ldr	r3, [sp, #24]
   3784a:	2b65      	cmp	r3, #101	; 0x65
   3784c:	f340 8232 	ble.w	37cb4 <_vfprintf_r+0x122c>
   37850:	2200      	movs	r2, #0
   37852:	2300      	movs	r3, #0
   37854:	e9dd 010c 	ldrd	r0, r1, [sp, #48]	; 0x30
   37858:	f008 fad8 	bl	3fe0c <__aeabi_dcmpeq>
   3785c:	2800      	cmp	r0, #0
   3785e:	d068      	beq.n	37932 <_vfprintf_r+0xeaa>
   37860:	4b6d      	ldr	r3, [pc, #436]	; (37a18 <_vfprintf_r+0xf90>)
   37862:	6023      	str	r3, [r4, #0]
   37864:	2301      	movs	r3, #1
   37866:	441e      	add	r6, r3
   37868:	6063      	str	r3, [r4, #4]
   3786a:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   3786c:	9624      	str	r6, [sp, #144]	; 0x90
   3786e:	3301      	adds	r3, #1
   37870:	2b07      	cmp	r3, #7
   37872:	9323      	str	r3, [sp, #140]	; 0x8c
   37874:	dc37      	bgt.n	378e6 <_vfprintf_r+0xe5e>
   37876:	3408      	adds	r4, #8
   37878:	9b1c      	ldr	r3, [sp, #112]	; 0x70
   3787a:	9a05      	ldr	r2, [sp, #20]
   3787c:	4293      	cmp	r3, r2
   3787e:	db03      	blt.n	37888 <_vfprintf_r+0xe00>
   37880:	f01b 0f01 	tst.w	fp, #1
   37884:	f43f ad30 	beq.w	372e8 <_vfprintf_r+0x860>
   37888:	9b13      	ldr	r3, [sp, #76]	; 0x4c
   3788a:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   3788c:	6023      	str	r3, [r4, #0]
   3788e:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   37890:	6063      	str	r3, [r4, #4]
   37892:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37894:	4413      	add	r3, r2
   37896:	9324      	str	r3, [sp, #144]	; 0x90
   37898:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   3789a:	3301      	adds	r3, #1
   3789c:	2b07      	cmp	r3, #7
   3789e:	9323      	str	r3, [sp, #140]	; 0x8c
   378a0:	dc2b      	bgt.n	378fa <_vfprintf_r+0xe72>
   378a2:	3408      	adds	r4, #8
   378a4:	9b05      	ldr	r3, [sp, #20]
   378a6:	1e5d      	subs	r5, r3, #1
   378a8:	2d00      	cmp	r5, #0
   378aa:	f77f ad1d 	ble.w	372e8 <_vfprintf_r+0x860>
   378ae:	4e5b      	ldr	r6, [pc, #364]	; (37a1c <_vfprintf_r+0xf94>)
   378b0:	2710      	movs	r7, #16
   378b2:	2d10      	cmp	r5, #16
   378b4:	f104 0108 	add.w	r1, r4, #8
   378b8:	6026      	str	r6, [r4, #0]
   378ba:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   378be:	f103 0301 	add.w	r3, r3, #1
   378c2:	dc24      	bgt.n	3790e <_vfprintf_r+0xe86>
   378c4:	442a      	add	r2, r5
   378c6:	6065      	str	r5, [r4, #4]
   378c8:	9224      	str	r2, [sp, #144]	; 0x90
   378ca:	2b07      	cmp	r3, #7
   378cc:	9323      	str	r3, [sp, #140]	; 0x8c
   378ce:	f340 8287 	ble.w	37de0 <_vfprintf_r+0x1358>
   378d2:	aa22      	add	r2, sp, #136	; 0x88
   378d4:	4649      	mov	r1, r9
   378d6:	4650      	mov	r0, sl
   378d8:	f006 fa48 	bl	3dd6c <__sprint_r>
   378dc:	2800      	cmp	r0, #0
   378de:	f040 82a0 	bne.w	37e22 <_vfprintf_r+0x139a>
   378e2:	ac25      	add	r4, sp, #148	; 0x94
   378e4:	e500      	b.n	372e8 <_vfprintf_r+0x860>
   378e6:	aa22      	add	r2, sp, #136	; 0x88
   378e8:	4649      	mov	r1, r9
   378ea:	4650      	mov	r0, sl
   378ec:	f006 fa3e 	bl	3dd6c <__sprint_r>
   378f0:	2800      	cmp	r0, #0
   378f2:	f040 8296 	bne.w	37e22 <_vfprintf_r+0x139a>
   378f6:	ac25      	add	r4, sp, #148	; 0x94
   378f8:	e7be      	b.n	37878 <_vfprintf_r+0xdf0>
   378fa:	aa22      	add	r2, sp, #136	; 0x88
   378fc:	4649      	mov	r1, r9
   378fe:	4650      	mov	r0, sl
   37900:	f006 fa34 	bl	3dd6c <__sprint_r>
   37904:	2800      	cmp	r0, #0
   37906:	f040 828c 	bne.w	37e22 <_vfprintf_r+0x139a>
   3790a:	ac25      	add	r4, sp, #148	; 0x94
   3790c:	e7ca      	b.n	378a4 <_vfprintf_r+0xe1c>
   3790e:	3210      	adds	r2, #16
   37910:	2b07      	cmp	r3, #7
   37912:	6067      	str	r7, [r4, #4]
   37914:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   37918:	dd08      	ble.n	3792c <_vfprintf_r+0xea4>
   3791a:	aa22      	add	r2, sp, #136	; 0x88
   3791c:	4649      	mov	r1, r9
   3791e:	4650      	mov	r0, sl
   37920:	f006 fa24 	bl	3dd6c <__sprint_r>
   37924:	2800      	cmp	r0, #0
   37926:	f040 827c 	bne.w	37e22 <_vfprintf_r+0x139a>
   3792a:	a925      	add	r1, sp, #148	; 0x94
   3792c:	3d10      	subs	r5, #16
   3792e:	460c      	mov	r4, r1
   37930:	e7bf      	b.n	378b2 <_vfprintf_r+0xe2a>
   37932:	9b1c      	ldr	r3, [sp, #112]	; 0x70
   37934:	2b00      	cmp	r3, #0
   37936:	dc73      	bgt.n	37a20 <_vfprintf_r+0xf98>
   37938:	4b37      	ldr	r3, [pc, #220]	; (37a18 <_vfprintf_r+0xf90>)
   3793a:	6023      	str	r3, [r4, #0]
   3793c:	2301      	movs	r3, #1
   3793e:	441e      	add	r6, r3
   37940:	6063      	str	r3, [r4, #4]
   37942:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37944:	9624      	str	r6, [sp, #144]	; 0x90
   37946:	3301      	adds	r3, #1
   37948:	2b07      	cmp	r3, #7
   3794a:	9323      	str	r3, [sp, #140]	; 0x8c
   3794c:	dc3d      	bgt.n	379ca <_vfprintf_r+0xf42>
   3794e:	3408      	adds	r4, #8
   37950:	9905      	ldr	r1, [sp, #20]
   37952:	9a1c      	ldr	r2, [sp, #112]	; 0x70
   37954:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37956:	430a      	orrs	r2, r1
   37958:	f00b 0101 	and.w	r1, fp, #1
   3795c:	430a      	orrs	r2, r1
   3795e:	f43f acc3 	beq.w	372e8 <_vfprintf_r+0x860>
   37962:	9a13      	ldr	r2, [sp, #76]	; 0x4c
   37964:	6022      	str	r2, [r4, #0]
   37966:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   37968:	4413      	add	r3, r2
   3796a:	6062      	str	r2, [r4, #4]
   3796c:	9324      	str	r3, [sp, #144]	; 0x90
   3796e:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37970:	3301      	adds	r3, #1
   37972:	2b07      	cmp	r3, #7
   37974:	9323      	str	r3, [sp, #140]	; 0x8c
   37976:	dc32      	bgt.n	379de <_vfprintf_r+0xf56>
   37978:	3408      	adds	r4, #8
   3797a:	9d1c      	ldr	r5, [sp, #112]	; 0x70
   3797c:	2d00      	cmp	r5, #0
   3797e:	da1b      	bge.n	379b8 <_vfprintf_r+0xf30>
   37980:	426d      	negs	r5, r5
   37982:	4623      	mov	r3, r4
   37984:	4e25      	ldr	r6, [pc, #148]	; (37a1c <_vfprintf_r+0xf94>)
   37986:	2710      	movs	r7, #16
   37988:	2d10      	cmp	r5, #16
   3798a:	f104 0408 	add.w	r4, r4, #8
   3798e:	601e      	str	r6, [r3, #0]
   37990:	e9dd 2123 	ldrd	r2, r1, [sp, #140]	; 0x8c
   37994:	f102 0201 	add.w	r2, r2, #1
   37998:	dc2b      	bgt.n	379f2 <_vfprintf_r+0xf6a>
   3799a:	605d      	str	r5, [r3, #4]
   3799c:	2a07      	cmp	r2, #7
   3799e:	440d      	add	r5, r1
   379a0:	e9cd 2523 	strd	r2, r5, [sp, #140]	; 0x8c
   379a4:	dd08      	ble.n	379b8 <_vfprintf_r+0xf30>
   379a6:	aa22      	add	r2, sp, #136	; 0x88
   379a8:	4649      	mov	r1, r9
   379aa:	4650      	mov	r0, sl
   379ac:	f006 f9de 	bl	3dd6c <__sprint_r>
   379b0:	2800      	cmp	r0, #0
   379b2:	f040 8236 	bne.w	37e22 <_vfprintf_r+0x139a>
   379b6:	ac25      	add	r4, sp, #148	; 0x94
   379b8:	9b05      	ldr	r3, [sp, #20]
   379ba:	9a05      	ldr	r2, [sp, #20]
   379bc:	6063      	str	r3, [r4, #4]
   379be:	9b24      	ldr	r3, [sp, #144]	; 0x90
   379c0:	f8c4 8000 	str.w	r8, [r4]
   379c4:	4413      	add	r3, r2
   379c6:	9324      	str	r3, [sp, #144]	; 0x90
   379c8:	e487      	b.n	372da <_vfprintf_r+0x852>
   379ca:	aa22      	add	r2, sp, #136	; 0x88
   379cc:	4649      	mov	r1, r9
   379ce:	4650      	mov	r0, sl
   379d0:	f006 f9cc 	bl	3dd6c <__sprint_r>
   379d4:	2800      	cmp	r0, #0
   379d6:	f040 8224 	bne.w	37e22 <_vfprintf_r+0x139a>
   379da:	ac25      	add	r4, sp, #148	; 0x94
   379dc:	e7b8      	b.n	37950 <_vfprintf_r+0xec8>
   379de:	aa22      	add	r2, sp, #136	; 0x88
   379e0:	4649      	mov	r1, r9
   379e2:	4650      	mov	r0, sl
   379e4:	f006 f9c2 	bl	3dd6c <__sprint_r>
   379e8:	2800      	cmp	r0, #0
   379ea:	f040 821a 	bne.w	37e22 <_vfprintf_r+0x139a>
   379ee:	ac25      	add	r4, sp, #148	; 0x94
   379f0:	e7c3      	b.n	3797a <_vfprintf_r+0xef2>
   379f2:	3110      	adds	r1, #16
   379f4:	2a07      	cmp	r2, #7
   379f6:	605f      	str	r7, [r3, #4]
   379f8:	e9cd 2123 	strd	r2, r1, [sp, #140]	; 0x8c
   379fc:	dd08      	ble.n	37a10 <_vfprintf_r+0xf88>
   379fe:	aa22      	add	r2, sp, #136	; 0x88
   37a00:	4649      	mov	r1, r9
   37a02:	4650      	mov	r0, sl
   37a04:	f006 f9b2 	bl	3dd6c <__sprint_r>
   37a08:	2800      	cmp	r0, #0
   37a0a:	f040 820a 	bne.w	37e22 <_vfprintf_r+0x139a>
   37a0e:	ac25      	add	r4, sp, #148	; 0x94
   37a10:	3d10      	subs	r5, #16
   37a12:	4623      	mov	r3, r4
   37a14:	e7b8      	b.n	37988 <_vfprintf_r+0xf00>
   37a16:	bf00      	nop
   37a18:	00040501 	.word	0x00040501
   37a1c:	0004055a 	.word	0x0004055a
   37a20:	9f05      	ldr	r7, [sp, #20]
   37a22:	42af      	cmp	r7, r5
   37a24:	bfa8      	it	ge
   37a26:	462f      	movge	r7, r5
   37a28:	2f00      	cmp	r7, #0
   37a2a:	dd09      	ble.n	37a40 <_vfprintf_r+0xfb8>
   37a2c:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37a2e:	443e      	add	r6, r7
   37a30:	3301      	adds	r3, #1
   37a32:	9624      	str	r6, [sp, #144]	; 0x90
   37a34:	2b07      	cmp	r3, #7
   37a36:	9323      	str	r3, [sp, #140]	; 0x8c
   37a38:	e9c4 8700 	strd	r8, r7, [r4]
   37a3c:	dc75      	bgt.n	37b2a <_vfprintf_r+0x10a2>
   37a3e:	3408      	adds	r4, #8
   37a40:	2f00      	cmp	r7, #0
   37a42:	bfac      	ite	ge
   37a44:	1bee      	subge	r6, r5, r7
   37a46:	462e      	movlt	r6, r5
   37a48:	2e00      	cmp	r6, #0
   37a4a:	dd18      	ble.n	37a7e <_vfprintf_r+0xff6>
   37a4c:	4f98      	ldr	r7, [pc, #608]	; (37cb0 <_vfprintf_r+0x1228>)
   37a4e:	2e10      	cmp	r6, #16
   37a50:	f104 0108 	add.w	r1, r4, #8
   37a54:	6027      	str	r7, [r4, #0]
   37a56:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   37a5a:	f103 0301 	add.w	r3, r3, #1
   37a5e:	dc6e      	bgt.n	37b3e <_vfprintf_r+0x10b6>
   37a60:	6066      	str	r6, [r4, #4]
   37a62:	2b07      	cmp	r3, #7
   37a64:	4416      	add	r6, r2
   37a66:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   37a6a:	dd7b      	ble.n	37b64 <_vfprintf_r+0x10dc>
   37a6c:	aa22      	add	r2, sp, #136	; 0x88
   37a6e:	4649      	mov	r1, r9
   37a70:	4650      	mov	r0, sl
   37a72:	f006 f97b 	bl	3dd6c <__sprint_r>
   37a76:	2800      	cmp	r0, #0
   37a78:	f040 81d3 	bne.w	37e22 <_vfprintf_r+0x139a>
   37a7c:	ac25      	add	r4, sp, #148	; 0x94
   37a7e:	f41b 6f80 	tst.w	fp, #1024	; 0x400
   37a82:	4445      	add	r5, r8
   37a84:	d00a      	beq.n	37a9c <_vfprintf_r+0x1014>
   37a86:	9b09      	ldr	r3, [sp, #36]	; 0x24
   37a88:	2b00      	cmp	r3, #0
   37a8a:	d16d      	bne.n	37b68 <_vfprintf_r+0x10e0>
   37a8c:	9b08      	ldr	r3, [sp, #32]
   37a8e:	2b00      	cmp	r3, #0
   37a90:	d16d      	bne.n	37b6e <_vfprintf_r+0x10e6>
   37a92:	9b05      	ldr	r3, [sp, #20]
   37a94:	4443      	add	r3, r8
   37a96:	429d      	cmp	r5, r3
   37a98:	bf28      	it	cs
   37a9a:	461d      	movcs	r5, r3
   37a9c:	9b1c      	ldr	r3, [sp, #112]	; 0x70
   37a9e:	9a05      	ldr	r2, [sp, #20]
   37aa0:	4293      	cmp	r3, r2
   37aa2:	db02      	blt.n	37aaa <_vfprintf_r+0x1022>
   37aa4:	f01b 0f01 	tst.w	fp, #1
   37aa8:	d00e      	beq.n	37ac8 <_vfprintf_r+0x1040>
   37aaa:	9b13      	ldr	r3, [sp, #76]	; 0x4c
   37aac:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   37aae:	6023      	str	r3, [r4, #0]
   37ab0:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   37ab2:	6063      	str	r3, [r4, #4]
   37ab4:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37ab6:	4413      	add	r3, r2
   37ab8:	9324      	str	r3, [sp, #144]	; 0x90
   37aba:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37abc:	3301      	adds	r3, #1
   37abe:	2b07      	cmp	r3, #7
   37ac0:	9323      	str	r3, [sp, #140]	; 0x8c
   37ac2:	f300 80cf 	bgt.w	37c64 <_vfprintf_r+0x11dc>
   37ac6:	3408      	adds	r4, #8
   37ac8:	9b05      	ldr	r3, [sp, #20]
   37aca:	9e1c      	ldr	r6, [sp, #112]	; 0x70
   37acc:	eb08 0203 	add.w	r2, r8, r3
   37ad0:	1b9e      	subs	r6, r3, r6
   37ad2:	1b52      	subs	r2, r2, r5
   37ad4:	4296      	cmp	r6, r2
   37ad6:	bfa8      	it	ge
   37ad8:	4616      	movge	r6, r2
   37ada:	2e00      	cmp	r6, #0
   37adc:	dd0b      	ble.n	37af6 <_vfprintf_r+0x106e>
   37ade:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37ae0:	4433      	add	r3, r6
   37ae2:	9324      	str	r3, [sp, #144]	; 0x90
   37ae4:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37ae6:	3301      	adds	r3, #1
   37ae8:	2b07      	cmp	r3, #7
   37aea:	9323      	str	r3, [sp, #140]	; 0x8c
   37aec:	e9c4 5600 	strd	r5, r6, [r4]
   37af0:	f300 80c2 	bgt.w	37c78 <_vfprintf_r+0x11f0>
   37af4:	3408      	adds	r4, #8
   37af6:	9d1c      	ldr	r5, [sp, #112]	; 0x70
   37af8:	2e00      	cmp	r6, #0
   37afa:	9b05      	ldr	r3, [sp, #20]
   37afc:	eba3 0505 	sub.w	r5, r3, r5
   37b00:	bfa8      	it	ge
   37b02:	1bad      	subge	r5, r5, r6
   37b04:	2d00      	cmp	r5, #0
   37b06:	f77f abef 	ble.w	372e8 <_vfprintf_r+0x860>
   37b0a:	4e69      	ldr	r6, [pc, #420]	; (37cb0 <_vfprintf_r+0x1228>)
   37b0c:	2710      	movs	r7, #16
   37b0e:	2d10      	cmp	r5, #16
   37b10:	f104 0108 	add.w	r1, r4, #8
   37b14:	6026      	str	r6, [r4, #0]
   37b16:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   37b1a:	f103 0301 	add.w	r3, r3, #1
   37b1e:	f300 80b5 	bgt.w	37c8c <_vfprintf_r+0x1204>
   37b22:	6065      	str	r5, [r4, #4]
   37b24:	4415      	add	r5, r2
   37b26:	9524      	str	r5, [sp, #144]	; 0x90
   37b28:	e6cf      	b.n	378ca <_vfprintf_r+0xe42>
   37b2a:	aa22      	add	r2, sp, #136	; 0x88
   37b2c:	4649      	mov	r1, r9
   37b2e:	4650      	mov	r0, sl
   37b30:	f006 f91c 	bl	3dd6c <__sprint_r>
   37b34:	2800      	cmp	r0, #0
   37b36:	f040 8174 	bne.w	37e22 <_vfprintf_r+0x139a>
   37b3a:	ac25      	add	r4, sp, #148	; 0x94
   37b3c:	e780      	b.n	37a40 <_vfprintf_r+0xfb8>
   37b3e:	2010      	movs	r0, #16
   37b40:	2b07      	cmp	r3, #7
   37b42:	4402      	add	r2, r0
   37b44:	6060      	str	r0, [r4, #4]
   37b46:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   37b4a:	dd08      	ble.n	37b5e <_vfprintf_r+0x10d6>
   37b4c:	aa22      	add	r2, sp, #136	; 0x88
   37b4e:	4649      	mov	r1, r9
   37b50:	4650      	mov	r0, sl
   37b52:	f006 f90b 	bl	3dd6c <__sprint_r>
   37b56:	2800      	cmp	r0, #0
   37b58:	f040 8163 	bne.w	37e22 <_vfprintf_r+0x139a>
   37b5c:	a925      	add	r1, sp, #148	; 0x94
   37b5e:	3e10      	subs	r6, #16
   37b60:	460c      	mov	r4, r1
   37b62:	e774      	b.n	37a4e <_vfprintf_r+0xfc6>
   37b64:	460c      	mov	r4, r1
   37b66:	e78a      	b.n	37a7e <_vfprintf_r+0xff6>
   37b68:	9b08      	ldr	r3, [sp, #32]
   37b6a:	2b00      	cmp	r3, #0
   37b6c:	d04a      	beq.n	37c04 <_vfprintf_r+0x117c>
   37b6e:	9b08      	ldr	r3, [sp, #32]
   37b70:	3b01      	subs	r3, #1
   37b72:	9308      	str	r3, [sp, #32]
   37b74:	9b14      	ldr	r3, [sp, #80]	; 0x50
   37b76:	9a12      	ldr	r2, [sp, #72]	; 0x48
   37b78:	6023      	str	r3, [r4, #0]
   37b7a:	9b12      	ldr	r3, [sp, #72]	; 0x48
   37b7c:	6063      	str	r3, [r4, #4]
   37b7e:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37b80:	4413      	add	r3, r2
   37b82:	9324      	str	r3, [sp, #144]	; 0x90
   37b84:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37b86:	3301      	adds	r3, #1
   37b88:	2b07      	cmp	r3, #7
   37b8a:	9323      	str	r3, [sp, #140]	; 0x8c
   37b8c:	dc41      	bgt.n	37c12 <_vfprintf_r+0x118a>
   37b8e:	3408      	adds	r4, #8
   37b90:	9b05      	ldr	r3, [sp, #20]
   37b92:	9a0a      	ldr	r2, [sp, #40]	; 0x28
   37b94:	eb08 0703 	add.w	r7, r8, r3
   37b98:	1b7b      	subs	r3, r7, r5
   37b9a:	7817      	ldrb	r7, [r2, #0]
   37b9c:	429f      	cmp	r7, r3
   37b9e:	bfa8      	it	ge
   37ba0:	461f      	movge	r7, r3
   37ba2:	2f00      	cmp	r7, #0
   37ba4:	dd0a      	ble.n	37bbc <_vfprintf_r+0x1134>
   37ba6:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37ba8:	443b      	add	r3, r7
   37baa:	9324      	str	r3, [sp, #144]	; 0x90
   37bac:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37bae:	3301      	adds	r3, #1
   37bb0:	2b07      	cmp	r3, #7
   37bb2:	9323      	str	r3, [sp, #140]	; 0x8c
   37bb4:	e9c4 5700 	strd	r5, r7, [r4]
   37bb8:	dc35      	bgt.n	37c26 <_vfprintf_r+0x119e>
   37bba:	3408      	adds	r4, #8
   37bbc:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   37bbe:	2f00      	cmp	r7, #0
   37bc0:	781e      	ldrb	r6, [r3, #0]
   37bc2:	bfa8      	it	ge
   37bc4:	1bf6      	subge	r6, r6, r7
   37bc6:	2e00      	cmp	r6, #0
   37bc8:	dd18      	ble.n	37bfc <_vfprintf_r+0x1174>
   37bca:	4839      	ldr	r0, [pc, #228]	; (37cb0 <_vfprintf_r+0x1228>)
   37bcc:	2e10      	cmp	r6, #16
   37bce:	f104 0108 	add.w	r1, r4, #8
   37bd2:	6020      	str	r0, [r4, #0]
   37bd4:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   37bd8:	f103 0301 	add.w	r3, r3, #1
   37bdc:	dc2d      	bgt.n	37c3a <_vfprintf_r+0x11b2>
   37bde:	6066      	str	r6, [r4, #4]
   37be0:	2b07      	cmp	r3, #7
   37be2:	4416      	add	r6, r2
   37be4:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   37be8:	dd3a      	ble.n	37c60 <_vfprintf_r+0x11d8>
   37bea:	aa22      	add	r2, sp, #136	; 0x88
   37bec:	4649      	mov	r1, r9
   37bee:	4650      	mov	r0, sl
   37bf0:	f006 f8bc 	bl	3dd6c <__sprint_r>
   37bf4:	2800      	cmp	r0, #0
   37bf6:	f040 8114 	bne.w	37e22 <_vfprintf_r+0x139a>
   37bfa:	ac25      	add	r4, sp, #148	; 0x94
   37bfc:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   37bfe:	781b      	ldrb	r3, [r3, #0]
   37c00:	441d      	add	r5, r3
   37c02:	e740      	b.n	37a86 <_vfprintf_r+0xffe>
   37c04:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   37c06:	3b01      	subs	r3, #1
   37c08:	930a      	str	r3, [sp, #40]	; 0x28
   37c0a:	9b09      	ldr	r3, [sp, #36]	; 0x24
   37c0c:	3b01      	subs	r3, #1
   37c0e:	9309      	str	r3, [sp, #36]	; 0x24
   37c10:	e7b0      	b.n	37b74 <_vfprintf_r+0x10ec>
   37c12:	aa22      	add	r2, sp, #136	; 0x88
   37c14:	4649      	mov	r1, r9
   37c16:	4650      	mov	r0, sl
   37c18:	f006 f8a8 	bl	3dd6c <__sprint_r>
   37c1c:	2800      	cmp	r0, #0
   37c1e:	f040 8100 	bne.w	37e22 <_vfprintf_r+0x139a>
   37c22:	ac25      	add	r4, sp, #148	; 0x94
   37c24:	e7b4      	b.n	37b90 <_vfprintf_r+0x1108>
   37c26:	aa22      	add	r2, sp, #136	; 0x88
   37c28:	4649      	mov	r1, r9
   37c2a:	4650      	mov	r0, sl
   37c2c:	f006 f89e 	bl	3dd6c <__sprint_r>
   37c30:	2800      	cmp	r0, #0
   37c32:	f040 80f6 	bne.w	37e22 <_vfprintf_r+0x139a>
   37c36:	ac25      	add	r4, sp, #148	; 0x94
   37c38:	e7c0      	b.n	37bbc <_vfprintf_r+0x1134>
   37c3a:	2010      	movs	r0, #16
   37c3c:	2b07      	cmp	r3, #7
   37c3e:	4402      	add	r2, r0
   37c40:	6060      	str	r0, [r4, #4]
   37c42:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   37c46:	dd08      	ble.n	37c5a <_vfprintf_r+0x11d2>
   37c48:	aa22      	add	r2, sp, #136	; 0x88
   37c4a:	4649      	mov	r1, r9
   37c4c:	4650      	mov	r0, sl
   37c4e:	f006 f88d 	bl	3dd6c <__sprint_r>
   37c52:	2800      	cmp	r0, #0
   37c54:	f040 80e5 	bne.w	37e22 <_vfprintf_r+0x139a>
   37c58:	a925      	add	r1, sp, #148	; 0x94
   37c5a:	3e10      	subs	r6, #16
   37c5c:	460c      	mov	r4, r1
   37c5e:	e7b4      	b.n	37bca <_vfprintf_r+0x1142>
   37c60:	460c      	mov	r4, r1
   37c62:	e7cb      	b.n	37bfc <_vfprintf_r+0x1174>
   37c64:	aa22      	add	r2, sp, #136	; 0x88
   37c66:	4649      	mov	r1, r9
   37c68:	4650      	mov	r0, sl
   37c6a:	f006 f87f 	bl	3dd6c <__sprint_r>
   37c6e:	2800      	cmp	r0, #0
   37c70:	f040 80d7 	bne.w	37e22 <_vfprintf_r+0x139a>
   37c74:	ac25      	add	r4, sp, #148	; 0x94
   37c76:	e727      	b.n	37ac8 <_vfprintf_r+0x1040>
   37c78:	aa22      	add	r2, sp, #136	; 0x88
   37c7a:	4649      	mov	r1, r9
   37c7c:	4650      	mov	r0, sl
   37c7e:	f006 f875 	bl	3dd6c <__sprint_r>
   37c82:	2800      	cmp	r0, #0
   37c84:	f040 80cd 	bne.w	37e22 <_vfprintf_r+0x139a>
   37c88:	ac25      	add	r4, sp, #148	; 0x94
   37c8a:	e734      	b.n	37af6 <_vfprintf_r+0x106e>
   37c8c:	3210      	adds	r2, #16
   37c8e:	2b07      	cmp	r3, #7
   37c90:	6067      	str	r7, [r4, #4]
   37c92:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   37c96:	dd08      	ble.n	37caa <_vfprintf_r+0x1222>
   37c98:	aa22      	add	r2, sp, #136	; 0x88
   37c9a:	4649      	mov	r1, r9
   37c9c:	4650      	mov	r0, sl
   37c9e:	f006 f865 	bl	3dd6c <__sprint_r>
   37ca2:	2800      	cmp	r0, #0
   37ca4:	f040 80bd 	bne.w	37e22 <_vfprintf_r+0x139a>
   37ca8:	a925      	add	r1, sp, #148	; 0x94
   37caa:	3d10      	subs	r5, #16
   37cac:	460c      	mov	r4, r1
   37cae:	e72e      	b.n	37b0e <_vfprintf_r+0x1086>
   37cb0:	0004055a 	.word	0x0004055a
   37cb4:	9a05      	ldr	r2, [sp, #20]
   37cb6:	3601      	adds	r6, #1
   37cb8:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37cba:	f104 0508 	add.w	r5, r4, #8
   37cbe:	2a01      	cmp	r2, #1
   37cc0:	f103 0301 	add.w	r3, r3, #1
   37cc4:	dc02      	bgt.n	37ccc <_vfprintf_r+0x1244>
   37cc6:	f01b 0f01 	tst.w	fp, #1
   37cca:	d07e      	beq.n	37dca <_vfprintf_r+0x1342>
   37ccc:	2201      	movs	r2, #1
   37cce:	2b07      	cmp	r3, #7
   37cd0:	f8c4 8000 	str.w	r8, [r4]
   37cd4:	6062      	str	r2, [r4, #4]
   37cd6:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   37cda:	dd08      	ble.n	37cee <_vfprintf_r+0x1266>
   37cdc:	aa22      	add	r2, sp, #136	; 0x88
   37cde:	4649      	mov	r1, r9
   37ce0:	4650      	mov	r0, sl
   37ce2:	f006 f843 	bl	3dd6c <__sprint_r>
   37ce6:	2800      	cmp	r0, #0
   37ce8:	f040 809b 	bne.w	37e22 <_vfprintf_r+0x139a>
   37cec:	ad25      	add	r5, sp, #148	; 0x94
   37cee:	9b13      	ldr	r3, [sp, #76]	; 0x4c
   37cf0:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   37cf2:	602b      	str	r3, [r5, #0]
   37cf4:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   37cf6:	606b      	str	r3, [r5, #4]
   37cf8:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37cfa:	4413      	add	r3, r2
   37cfc:	9324      	str	r3, [sp, #144]	; 0x90
   37cfe:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37d00:	3301      	adds	r3, #1
   37d02:	2b07      	cmp	r3, #7
   37d04:	9323      	str	r3, [sp, #140]	; 0x8c
   37d06:	dc32      	bgt.n	37d6e <_vfprintf_r+0x12e6>
   37d08:	3508      	adds	r5, #8
   37d0a:	9b05      	ldr	r3, [sp, #20]
   37d0c:	2200      	movs	r2, #0
   37d0e:	1e5c      	subs	r4, r3, #1
   37d10:	2300      	movs	r3, #0
   37d12:	e9dd 010c 	ldrd	r0, r1, [sp, #48]	; 0x30
   37d16:	f008 f879 	bl	3fe0c <__aeabi_dcmpeq>
   37d1a:	2800      	cmp	r0, #0
   37d1c:	d130      	bne.n	37d80 <_vfprintf_r+0x12f8>
   37d1e:	9923      	ldr	r1, [sp, #140]	; 0x8c
   37d20:	f108 0001 	add.w	r0, r8, #1
   37d24:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37d26:	3101      	adds	r1, #1
   37d28:	9a05      	ldr	r2, [sp, #20]
   37d2a:	3b01      	subs	r3, #1
   37d2c:	2907      	cmp	r1, #7
   37d2e:	4413      	add	r3, r2
   37d30:	e9c5 0400 	strd	r0, r4, [r5]
   37d34:	e9cd 1323 	strd	r1, r3, [sp, #140]	; 0x8c
   37d38:	dd50      	ble.n	37ddc <_vfprintf_r+0x1354>
   37d3a:	aa22      	add	r2, sp, #136	; 0x88
   37d3c:	4649      	mov	r1, r9
   37d3e:	4650      	mov	r0, sl
   37d40:	f006 f814 	bl	3dd6c <__sprint_r>
   37d44:	2800      	cmp	r0, #0
   37d46:	d16c      	bne.n	37e22 <_vfprintf_r+0x139a>
   37d48:	ad25      	add	r5, sp, #148	; 0x94
   37d4a:	ab1e      	add	r3, sp, #120	; 0x78
   37d4c:	9a15      	ldr	r2, [sp, #84]	; 0x54
   37d4e:	602b      	str	r3, [r5, #0]
   37d50:	9b15      	ldr	r3, [sp, #84]	; 0x54
   37d52:	606b      	str	r3, [r5, #4]
   37d54:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37d56:	4413      	add	r3, r2
   37d58:	9324      	str	r3, [sp, #144]	; 0x90
   37d5a:	9b23      	ldr	r3, [sp, #140]	; 0x8c
   37d5c:	3301      	adds	r3, #1
   37d5e:	2b07      	cmp	r3, #7
   37d60:	9323      	str	r3, [sp, #140]	; 0x8c
   37d62:	f73f adb6 	bgt.w	378d2 <_vfprintf_r+0xe4a>
   37d66:	f105 0408 	add.w	r4, r5, #8
   37d6a:	f7ff babd 	b.w	372e8 <_vfprintf_r+0x860>
   37d6e:	aa22      	add	r2, sp, #136	; 0x88
   37d70:	4649      	mov	r1, r9
   37d72:	4650      	mov	r0, sl
   37d74:	f005 fffa 	bl	3dd6c <__sprint_r>
   37d78:	2800      	cmp	r0, #0
   37d7a:	d152      	bne.n	37e22 <_vfprintf_r+0x139a>
   37d7c:	ad25      	add	r5, sp, #148	; 0x94
   37d7e:	e7c4      	b.n	37d0a <_vfprintf_r+0x1282>
   37d80:	2c00      	cmp	r4, #0
   37d82:	dde2      	ble.n	37d4a <_vfprintf_r+0x12c2>
   37d84:	4e57      	ldr	r6, [pc, #348]	; (37ee4 <_vfprintf_r+0x145c>)
   37d86:	2710      	movs	r7, #16
   37d88:	2c10      	cmp	r4, #16
   37d8a:	f105 0108 	add.w	r1, r5, #8
   37d8e:	602e      	str	r6, [r5, #0]
   37d90:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   37d94:	f103 0301 	add.w	r3, r3, #1
   37d98:	dc07      	bgt.n	37daa <_vfprintf_r+0x1322>
   37d9a:	606c      	str	r4, [r5, #4]
   37d9c:	2b07      	cmp	r3, #7
   37d9e:	4414      	add	r4, r2
   37da0:	e9cd 3423 	strd	r3, r4, [sp, #140]	; 0x8c
   37da4:	dcc9      	bgt.n	37d3a <_vfprintf_r+0x12b2>
   37da6:	460d      	mov	r5, r1
   37da8:	e7cf      	b.n	37d4a <_vfprintf_r+0x12c2>
   37daa:	3210      	adds	r2, #16
   37dac:	2b07      	cmp	r3, #7
   37dae:	606f      	str	r7, [r5, #4]
   37db0:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   37db4:	dd06      	ble.n	37dc4 <_vfprintf_r+0x133c>
   37db6:	aa22      	add	r2, sp, #136	; 0x88
   37db8:	4649      	mov	r1, r9
   37dba:	4650      	mov	r0, sl
   37dbc:	f005 ffd6 	bl	3dd6c <__sprint_r>
   37dc0:	bb78      	cbnz	r0, 37e22 <_vfprintf_r+0x139a>
   37dc2:	a925      	add	r1, sp, #148	; 0x94
   37dc4:	3c10      	subs	r4, #16
   37dc6:	460d      	mov	r5, r1
   37dc8:	e7de      	b.n	37d88 <_vfprintf_r+0x1300>
   37dca:	2201      	movs	r2, #1
   37dcc:	2b07      	cmp	r3, #7
   37dce:	f8c4 8000 	str.w	r8, [r4]
   37dd2:	6062      	str	r2, [r4, #4]
   37dd4:	e9cd 3623 	strd	r3, r6, [sp, #140]	; 0x8c
   37dd8:	ddb7      	ble.n	37d4a <_vfprintf_r+0x12c2>
   37dda:	e7ae      	b.n	37d3a <_vfprintf_r+0x12b2>
   37ddc:	3508      	adds	r5, #8
   37dde:	e7b4      	b.n	37d4a <_vfprintf_r+0x12c2>
   37de0:	460c      	mov	r4, r1
   37de2:	f7ff ba81 	b.w	372e8 <_vfprintf_r+0x860>
   37de6:	e9dd 3210 	ldrd	r3, r2, [sp, #64]	; 0x40
   37dea:	1a9d      	subs	r5, r3, r2
   37dec:	2d00      	cmp	r5, #0
   37dee:	f77f aa7f 	ble.w	372f0 <_vfprintf_r+0x868>
   37df2:	4e3d      	ldr	r6, [pc, #244]	; (37ee8 <_vfprintf_r+0x1460>)
   37df4:	2710      	movs	r7, #16
   37df6:	2d10      	cmp	r5, #16
   37df8:	6026      	str	r6, [r4, #0]
   37dfa:	e9dd 3223 	ldrd	r3, r2, [sp, #140]	; 0x8c
   37dfe:	f103 0301 	add.w	r3, r3, #1
   37e02:	dc18      	bgt.n	37e36 <_vfprintf_r+0x13ae>
   37e04:	6065      	str	r5, [r4, #4]
   37e06:	2b07      	cmp	r3, #7
   37e08:	4415      	add	r5, r2
   37e0a:	e9cd 3523 	strd	r3, r5, [sp, #140]	; 0x8c
   37e0e:	f77f aa6f 	ble.w	372f0 <_vfprintf_r+0x868>
   37e12:	aa22      	add	r2, sp, #136	; 0x88
   37e14:	4649      	mov	r1, r9
   37e16:	4650      	mov	r0, sl
   37e18:	f005 ffa8 	bl	3dd6c <__sprint_r>
   37e1c:	2800      	cmp	r0, #0
   37e1e:	f43f aa67 	beq.w	372f0 <_vfprintf_r+0x868>
   37e22:	9b03      	ldr	r3, [sp, #12]
   37e24:	2b00      	cmp	r3, #0
   37e26:	f43f a880 	beq.w	36f2a <_vfprintf_r+0x4a2>
   37e2a:	4619      	mov	r1, r3
   37e2c:	4650      	mov	r0, sl
   37e2e:	f001 fa33 	bl	39298 <_free_r>
   37e32:	f7ff b87a 	b.w	36f2a <_vfprintf_r+0x4a2>
   37e36:	3210      	adds	r2, #16
   37e38:	2b07      	cmp	r3, #7
   37e3a:	6067      	str	r7, [r4, #4]
   37e3c:	e9cd 3223 	strd	r3, r2, [sp, #140]	; 0x8c
   37e40:	dc02      	bgt.n	37e48 <_vfprintf_r+0x13c0>
   37e42:	3408      	adds	r4, #8
   37e44:	3d10      	subs	r5, #16
   37e46:	e7d6      	b.n	37df6 <_vfprintf_r+0x136e>
   37e48:	aa22      	add	r2, sp, #136	; 0x88
   37e4a:	4649      	mov	r1, r9
   37e4c:	4650      	mov	r0, sl
   37e4e:	f005 ff8d 	bl	3dd6c <__sprint_r>
   37e52:	2800      	cmp	r0, #0
   37e54:	d1e5      	bne.n	37e22 <_vfprintf_r+0x139a>
   37e56:	ac25      	add	r4, sp, #148	; 0x94
   37e58:	e7f4      	b.n	37e44 <_vfprintf_r+0x13bc>
   37e5a:	9903      	ldr	r1, [sp, #12]
   37e5c:	4650      	mov	r0, sl
   37e5e:	f001 fa1b 	bl	39298 <_free_r>
   37e62:	f7ff ba5d 	b.w	37320 <_vfprintf_r+0x898>
   37e66:	9b24      	ldr	r3, [sp, #144]	; 0x90
   37e68:	b91b      	cbnz	r3, 37e72 <_vfprintf_r+0x13ea>
   37e6a:	2300      	movs	r3, #0
   37e6c:	9323      	str	r3, [sp, #140]	; 0x8c
   37e6e:	f7ff b85c 	b.w	36f2a <_vfprintf_r+0x4a2>
   37e72:	aa22      	add	r2, sp, #136	; 0x88
   37e74:	4649      	mov	r1, r9
   37e76:	4650      	mov	r0, sl
   37e78:	f005 ff78 	bl	3dd6c <__sprint_r>
   37e7c:	2800      	cmp	r0, #0
   37e7e:	d0f4      	beq.n	37e6a <_vfprintf_r+0x13e2>
   37e80:	f7ff b853 	b.w	36f2a <_vfprintf_r+0x4a2>
   37e84:	ea56 0205 	orrs.w	r2, r6, r5
   37e88:	465f      	mov	r7, fp
   37e8a:	f43f ab73 	beq.w	37574 <_vfprintf_r+0xaec>
   37e8e:	2b01      	cmp	r3, #1
   37e90:	f43f ac0f 	beq.w	376b2 <_vfprintf_r+0xc2a>
   37e94:	2b02      	cmp	r3, #2
   37e96:	f50d 789c 	add.w	r8, sp, #312	; 0x138
   37e9a:	f43f ac56 	beq.w	3774a <_vfprintf_r+0xcc2>
   37e9e:	f006 0307 	and.w	r3, r6, #7
   37ea2:	08f6      	lsrs	r6, r6, #3
   37ea4:	4642      	mov	r2, r8
   37ea6:	ea46 7645 	orr.w	r6, r6, r5, lsl #29
   37eaa:	3330      	adds	r3, #48	; 0x30
   37eac:	08ed      	lsrs	r5, r5, #3
   37eae:	f808 3d01 	strb.w	r3, [r8, #-1]!
   37eb2:	ea56 0105 	orrs.w	r1, r6, r5
   37eb6:	d1f2      	bne.n	37e9e <_vfprintf_r+0x1416>
   37eb8:	07f8      	lsls	r0, r7, #31
   37eba:	d506      	bpl.n	37eca <_vfprintf_r+0x1442>
   37ebc:	2b30      	cmp	r3, #48	; 0x30
   37ebe:	d004      	beq.n	37eca <_vfprintf_r+0x1442>
   37ec0:	2330      	movs	r3, #48	; 0x30
   37ec2:	f808 3c01 	strb.w	r3, [r8, #-1]
   37ec6:	f1a2 0802 	sub.w	r8, r2, #2
   37eca:	ab4e      	add	r3, sp, #312	; 0x138
   37ecc:	9e04      	ldr	r6, [sp, #16]
   37ece:	46bb      	mov	fp, r7
   37ed0:	eba3 0308 	sub.w	r3, r3, r8
   37ed4:	9304      	str	r3, [sp, #16]
   37ed6:	2300      	movs	r3, #0
   37ed8:	9303      	str	r3, [sp, #12]
   37eda:	461d      	mov	r5, r3
   37edc:	e9cd 3308 	strd	r3, r3, [sp, #32]
   37ee0:	f7ff b949 	b.w	37176 <_vfprintf_r+0x6ee>
   37ee4:	0004055a 	.word	0x0004055a
   37ee8:	0004054a 	.word	0x0004054a

00037eec <vfprintf>:
   37eec:	4613      	mov	r3, r2
   37eee:	460a      	mov	r2, r1
   37ef0:	4601      	mov	r1, r0
   37ef2:	4802      	ldr	r0, [pc, #8]	; (37efc <vfprintf+0x10>)
   37ef4:	6800      	ldr	r0, [r0, #0]
   37ef6:	f7fe bdc7 	b.w	36a88 <_vfprintf_r>
   37efa:	bf00      	nop
   37efc:	20006000 	.word	0x20006000

00037f00 <__sbprintf>:
   37f00:	b570      	push	{r4, r5, r6, lr}
   37f02:	460c      	mov	r4, r1
   37f04:	8989      	ldrh	r1, [r1, #12]
   37f06:	f5ad 6d8e 	sub.w	sp, sp, #1136	; 0x470
   37f0a:	4606      	mov	r6, r0
   37f0c:	f021 0102 	bic.w	r1, r1, #2
   37f10:	a818      	add	r0, sp, #96	; 0x60
   37f12:	f8ad 1014 	strh.w	r1, [sp, #20]
   37f16:	6e61      	ldr	r1, [r4, #100]	; 0x64
   37f18:	911b      	str	r1, [sp, #108]	; 0x6c
   37f1a:	89e1      	ldrh	r1, [r4, #14]
   37f1c:	f8ad 1016 	strh.w	r1, [sp, #22]
   37f20:	69e1      	ldr	r1, [r4, #28]
   37f22:	9109      	str	r1, [sp, #36]	; 0x24
   37f24:	6a61      	ldr	r1, [r4, #36]	; 0x24
   37f26:	910b      	str	r1, [sp, #44]	; 0x2c
   37f28:	a91c      	add	r1, sp, #112	; 0x70
   37f2a:	9102      	str	r1, [sp, #8]
   37f2c:	9106      	str	r1, [sp, #24]
   37f2e:	f44f 6180 	mov.w	r1, #1024	; 0x400
   37f32:	9104      	str	r1, [sp, #16]
   37f34:	9107      	str	r1, [sp, #28]
   37f36:	2100      	movs	r1, #0
   37f38:	e9cd 3200 	strd	r3, r2, [sp]
   37f3c:	9108      	str	r1, [sp, #32]
   37f3e:	f001 fc2e 	bl	3979e <__retarget_lock_init_recursive>
   37f42:	a902      	add	r1, sp, #8
   37f44:	4630      	mov	r0, r6
   37f46:	e9dd 3200 	ldrd	r3, r2, [sp]
   37f4a:	f7fe fd9d 	bl	36a88 <_vfprintf_r>
   37f4e:	1e05      	subs	r5, r0, #0
   37f50:	db07      	blt.n	37f62 <__sbprintf+0x62>
   37f52:	a902      	add	r1, sp, #8
   37f54:	4630      	mov	r0, r6
   37f56:	f001 f80d 	bl	38f74 <_fflush_r>
   37f5a:	2800      	cmp	r0, #0
   37f5c:	bf18      	it	ne
   37f5e:	f04f 35ff 	movne.w	r5, #4294967295	; 0xffffffff
   37f62:	f8bd 3014 	ldrh.w	r3, [sp, #20]
   37f66:	9818      	ldr	r0, [sp, #96]	; 0x60
   37f68:	065b      	lsls	r3, r3, #25
   37f6a:	bf42      	ittt	mi
   37f6c:	89a3      	ldrhmi	r3, [r4, #12]
   37f6e:	f043 0340 	orrmi.w	r3, r3, #64	; 0x40
   37f72:	81a3      	strhmi	r3, [r4, #12]
   37f74:	f001 fc15 	bl	397a2 <__retarget_lock_close_recursive>
   37f78:	4628      	mov	r0, r5
   37f7a:	f50d 6d8e 	add.w	sp, sp, #1136	; 0x470
   37f7e:	bd70      	pop	{r4, r5, r6, pc}

00037f80 <_vsnprintf_r>:
   37f80:	b530      	push	{r4, r5, lr}
   37f82:	4614      	mov	r4, r2
   37f84:	b09b      	sub	sp, #108	; 0x6c
   37f86:	4605      	mov	r5, r0
   37f88:	461a      	mov	r2, r3
   37f8a:	2c00      	cmp	r4, #0
   37f8c:	da05      	bge.n	37f9a <_vsnprintf_r+0x1a>
   37f8e:	238b      	movs	r3, #139	; 0x8b
   37f90:	6003      	str	r3, [r0, #0]
   37f92:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   37f96:	b01b      	add	sp, #108	; 0x6c
   37f98:	bd30      	pop	{r4, r5, pc}
   37f9a:	f44f 7302 	mov.w	r3, #520	; 0x208
   37f9e:	9100      	str	r1, [sp, #0]
   37fa0:	9104      	str	r1, [sp, #16]
   37fa2:	4669      	mov	r1, sp
   37fa4:	f8ad 300c 	strh.w	r3, [sp, #12]
   37fa8:	bf14      	ite	ne
   37faa:	f104 33ff 	addne.w	r3, r4, #4294967295	; 0xffffffff
   37fae:	4623      	moveq	r3, r4
   37fb0:	9302      	str	r3, [sp, #8]
   37fb2:	9305      	str	r3, [sp, #20]
   37fb4:	f64f 73ff 	movw	r3, #65535	; 0xffff
   37fb8:	f8ad 300e 	strh.w	r3, [sp, #14]
   37fbc:	9b1e      	ldr	r3, [sp, #120]	; 0x78
   37fbe:	f7fc f9f3 	bl	343a8 <_svfprintf_r>
   37fc2:	1c43      	adds	r3, r0, #1
   37fc4:	bfbc      	itt	lt
   37fc6:	238b      	movlt	r3, #139	; 0x8b
   37fc8:	602b      	strlt	r3, [r5, #0]
   37fca:	2c00      	cmp	r4, #0
   37fcc:	d0e3      	beq.n	37f96 <_vsnprintf_r+0x16>
   37fce:	9b00      	ldr	r3, [sp, #0]
   37fd0:	2200      	movs	r2, #0
   37fd2:	701a      	strb	r2, [r3, #0]
   37fd4:	e7df      	b.n	37f96 <_vsnprintf_r+0x16>

00037fd6 <vsnprintf>:
   37fd6:	b507      	push	{r0, r1, r2, lr}
   37fd8:	9300      	str	r3, [sp, #0]
   37fda:	4613      	mov	r3, r2
   37fdc:	460a      	mov	r2, r1
   37fde:	4601      	mov	r1, r0
   37fe0:	4803      	ldr	r0, [pc, #12]	; (37ff0 <vsnprintf+0x1a>)
   37fe2:	6800      	ldr	r0, [r0, #0]
   37fe4:	f7ff ffcc 	bl	37f80 <_vsnprintf_r>
   37fe8:	b003      	add	sp, #12
   37fea:	f85d fb04 	ldr.w	pc, [sp], #4
   37fee:	bf00      	nop
   37ff0:	20006000 	.word	0x20006000

00037ff4 <__swbuf_r>:
   37ff4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
   37ff6:	460e      	mov	r6, r1
   37ff8:	4614      	mov	r4, r2
   37ffa:	4605      	mov	r5, r0
   37ffc:	b118      	cbz	r0, 38006 <__swbuf_r+0x12>
   37ffe:	6b83      	ldr	r3, [r0, #56]	; 0x38
   38000:	b90b      	cbnz	r3, 38006 <__swbuf_r+0x12>
   38002:	f001 f867 	bl	390d4 <__sinit>
   38006:	69a3      	ldr	r3, [r4, #24]
   38008:	60a3      	str	r3, [r4, #8]
   3800a:	89a3      	ldrh	r3, [r4, #12]
   3800c:	0719      	lsls	r1, r3, #28
   3800e:	d529      	bpl.n	38064 <__swbuf_r+0x70>
   38010:	6923      	ldr	r3, [r4, #16]
   38012:	b33b      	cbz	r3, 38064 <__swbuf_r+0x70>
   38014:	f9b4 300c 	ldrsh.w	r3, [r4, #12]
   38018:	b2f6      	uxtb	r6, r6
   3801a:	049a      	lsls	r2, r3, #18
   3801c:	4637      	mov	r7, r6
   3801e:	d52a      	bpl.n	38076 <__swbuf_r+0x82>
   38020:	6823      	ldr	r3, [r4, #0]
   38022:	6920      	ldr	r0, [r4, #16]
   38024:	1a18      	subs	r0, r3, r0
   38026:	6963      	ldr	r3, [r4, #20]
   38028:	4283      	cmp	r3, r0
   3802a:	dc04      	bgt.n	38036 <__swbuf_r+0x42>
   3802c:	4621      	mov	r1, r4
   3802e:	4628      	mov	r0, r5
   38030:	f000 ffa0 	bl	38f74 <_fflush_r>
   38034:	b9e0      	cbnz	r0, 38070 <__swbuf_r+0x7c>
   38036:	68a3      	ldr	r3, [r4, #8]
   38038:	3b01      	subs	r3, #1
   3803a:	60a3      	str	r3, [r4, #8]
   3803c:	6823      	ldr	r3, [r4, #0]
   3803e:	1c5a      	adds	r2, r3, #1
   38040:	6022      	str	r2, [r4, #0]
   38042:	701e      	strb	r6, [r3, #0]
   38044:	1c43      	adds	r3, r0, #1
   38046:	6962      	ldr	r2, [r4, #20]
   38048:	429a      	cmp	r2, r3
   3804a:	d004      	beq.n	38056 <__swbuf_r+0x62>
   3804c:	89a3      	ldrh	r3, [r4, #12]
   3804e:	07db      	lsls	r3, r3, #31
   38050:	d506      	bpl.n	38060 <__swbuf_r+0x6c>
   38052:	2e0a      	cmp	r6, #10
   38054:	d104      	bne.n	38060 <__swbuf_r+0x6c>
   38056:	4621      	mov	r1, r4
   38058:	4628      	mov	r0, r5
   3805a:	f000 ff8b 	bl	38f74 <_fflush_r>
   3805e:	b938      	cbnz	r0, 38070 <__swbuf_r+0x7c>
   38060:	4638      	mov	r0, r7
   38062:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
   38064:	4621      	mov	r1, r4
   38066:	4628      	mov	r0, r5
   38068:	f000 f828 	bl	380bc <__swsetup_r>
   3806c:	2800      	cmp	r0, #0
   3806e:	d0d1      	beq.n	38014 <__swbuf_r+0x20>
   38070:	f04f 37ff 	mov.w	r7, #4294967295	; 0xffffffff
   38074:	e7f4      	b.n	38060 <__swbuf_r+0x6c>
   38076:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   3807a:	81a3      	strh	r3, [r4, #12]
   3807c:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3807e:	f423 5300 	bic.w	r3, r3, #8192	; 0x2000
   38082:	6663      	str	r3, [r4, #100]	; 0x64
   38084:	e7cc      	b.n	38020 <__swbuf_r+0x2c>

00038086 <__swbuf>:
   38086:	4b03      	ldr	r3, [pc, #12]	; (38094 <__swbuf+0xe>)
   38088:	460a      	mov	r2, r1
   3808a:	4601      	mov	r1, r0
   3808c:	6818      	ldr	r0, [r3, #0]
   3808e:	f7ff bfb1 	b.w	37ff4 <__swbuf_r>
   38092:	bf00      	nop
   38094:	20006000 	.word	0x20006000

00038098 <_write_r>:
   38098:	b538      	push	{r3, r4, r5, lr}
   3809a:	4604      	mov	r4, r0
   3809c:	4d06      	ldr	r5, [pc, #24]	; (380b8 <_write_r+0x20>)
   3809e:	4608      	mov	r0, r1
   380a0:	4611      	mov	r1, r2
   380a2:	2200      	movs	r2, #0
   380a4:	602a      	str	r2, [r5, #0]
   380a6:	461a      	mov	r2, r3
   380a8:	f7fb f98c 	bl	333c4 <_write>
   380ac:	1c43      	adds	r3, r0, #1
   380ae:	d102      	bne.n	380b6 <_write_r+0x1e>
   380b0:	682b      	ldr	r3, [r5, #0]
   380b2:	b103      	cbz	r3, 380b6 <_write_r+0x1e>
   380b4:	6023      	str	r3, [r4, #0]
   380b6:	bd38      	pop	{r3, r4, r5, pc}
   380b8:	20006a0c 	.word	0x20006a0c

000380bc <__swsetup_r>:
   380bc:	b538      	push	{r3, r4, r5, lr}
   380be:	4b2a      	ldr	r3, [pc, #168]	; (38168 <__swsetup_r+0xac>)
   380c0:	4605      	mov	r5, r0
   380c2:	460c      	mov	r4, r1
   380c4:	6818      	ldr	r0, [r3, #0]
   380c6:	b118      	cbz	r0, 380d0 <__swsetup_r+0x14>
   380c8:	6b83      	ldr	r3, [r0, #56]	; 0x38
   380ca:	b90b      	cbnz	r3, 380d0 <__swsetup_r+0x14>
   380cc:	f001 f802 	bl	390d4 <__sinit>
   380d0:	89a3      	ldrh	r3, [r4, #12]
   380d2:	f9b4 200c 	ldrsh.w	r2, [r4, #12]
   380d6:	0718      	lsls	r0, r3, #28
   380d8:	d422      	bmi.n	38120 <__swsetup_r+0x64>
   380da:	06d9      	lsls	r1, r3, #27
   380dc:	d407      	bmi.n	380ee <__swsetup_r+0x32>
   380de:	2309      	movs	r3, #9
   380e0:	602b      	str	r3, [r5, #0]
   380e2:	f042 0340 	orr.w	r3, r2, #64	; 0x40
   380e6:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   380ea:	81a3      	strh	r3, [r4, #12]
   380ec:	e034      	b.n	38158 <__swsetup_r+0x9c>
   380ee:	0758      	lsls	r0, r3, #29
   380f0:	d512      	bpl.n	38118 <__swsetup_r+0x5c>
   380f2:	6b21      	ldr	r1, [r4, #48]	; 0x30
   380f4:	b141      	cbz	r1, 38108 <__swsetup_r+0x4c>
   380f6:	f104 0340 	add.w	r3, r4, #64	; 0x40
   380fa:	4299      	cmp	r1, r3
   380fc:	d002      	beq.n	38104 <__swsetup_r+0x48>
   380fe:	4628      	mov	r0, r5
   38100:	f001 f8ca 	bl	39298 <_free_r>
   38104:	2300      	movs	r3, #0
   38106:	6323      	str	r3, [r4, #48]	; 0x30
   38108:	89a3      	ldrh	r3, [r4, #12]
   3810a:	f023 0324 	bic.w	r3, r3, #36	; 0x24
   3810e:	81a3      	strh	r3, [r4, #12]
   38110:	2300      	movs	r3, #0
   38112:	6063      	str	r3, [r4, #4]
   38114:	6923      	ldr	r3, [r4, #16]
   38116:	6023      	str	r3, [r4, #0]
   38118:	89a3      	ldrh	r3, [r4, #12]
   3811a:	f043 0308 	orr.w	r3, r3, #8
   3811e:	81a3      	strh	r3, [r4, #12]
   38120:	6923      	ldr	r3, [r4, #16]
   38122:	b94b      	cbnz	r3, 38138 <__swsetup_r+0x7c>
   38124:	89a3      	ldrh	r3, [r4, #12]
   38126:	f403 7320 	and.w	r3, r3, #640	; 0x280
   3812a:	f5b3 7f00 	cmp.w	r3, #512	; 0x200
   3812e:	d003      	beq.n	38138 <__swsetup_r+0x7c>
   38130:	4621      	mov	r1, r4
   38132:	4628      	mov	r0, r5
   38134:	f001 fb7c 	bl	39830 <__smakebuf_r>
   38138:	89a0      	ldrh	r0, [r4, #12]
   3813a:	f9b4 200c 	ldrsh.w	r2, [r4, #12]
   3813e:	f010 0301 	ands.w	r3, r0, #1
   38142:	d00a      	beq.n	3815a <__swsetup_r+0x9e>
   38144:	2300      	movs	r3, #0
   38146:	60a3      	str	r3, [r4, #8]
   38148:	6963      	ldr	r3, [r4, #20]
   3814a:	425b      	negs	r3, r3
   3814c:	61a3      	str	r3, [r4, #24]
   3814e:	6923      	ldr	r3, [r4, #16]
   38150:	b943      	cbnz	r3, 38164 <__swsetup_r+0xa8>
   38152:	f010 0080 	ands.w	r0, r0, #128	; 0x80
   38156:	d1c4      	bne.n	380e2 <__swsetup_r+0x26>
   38158:	bd38      	pop	{r3, r4, r5, pc}
   3815a:	0781      	lsls	r1, r0, #30
   3815c:	bf58      	it	pl
   3815e:	6963      	ldrpl	r3, [r4, #20]
   38160:	60a3      	str	r3, [r4, #8]
   38162:	e7f4      	b.n	3814e <__swsetup_r+0x92>
   38164:	2000      	movs	r0, #0
   38166:	e7f7      	b.n	38158 <__swsetup_r+0x9c>
   38168:	20006000 	.word	0x20006000

0003816c <_close_r>:
   3816c:	b538      	push	{r3, r4, r5, lr}
   3816e:	2300      	movs	r3, #0
   38170:	4d05      	ldr	r5, [pc, #20]	; (38188 <_close_r+0x1c>)
   38172:	4604      	mov	r4, r0
   38174:	4608      	mov	r0, r1
   38176:	602b      	str	r3, [r5, #0]
   38178:	f008 f8cc 	bl	40314 <_close>
   3817c:	1c43      	adds	r3, r0, #1
   3817e:	d102      	bne.n	38186 <_close_r+0x1a>
   38180:	682b      	ldr	r3, [r5, #0]
   38182:	b103      	cbz	r3, 38186 <_close_r+0x1a>
   38184:	6023      	str	r3, [r4, #0]
   38186:	bd38      	pop	{r3, r4, r5, pc}
   38188:	20006a0c 	.word	0x20006a0c
   3818c:	00000000 	.word	0x00000000

00038190 <quorem>:
   38190:	e92d 4ff7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   38194:	6903      	ldr	r3, [r0, #16]
   38196:	4607      	mov	r7, r0
   38198:	690c      	ldr	r4, [r1, #16]
   3819a:	42a3      	cmp	r3, r4
   3819c:	f2c0 8087 	blt.w	382ae <quorem+0x11e>
   381a0:	3c01      	subs	r4, #1
   381a2:	f100 0514 	add.w	r5, r0, #20
   381a6:	f101 0814 	add.w	r8, r1, #20
   381aa:	eb05 0384 	add.w	r3, r5, r4, lsl #2
   381ae:	f855 2024 	ldr.w	r2, [r5, r4, lsl #2]
   381b2:	ea4f 0b84 	mov.w	fp, r4, lsl #2
   381b6:	9301      	str	r3, [sp, #4]
   381b8:	eb08 0984 	add.w	r9, r8, r4, lsl #2
   381bc:	f858 3024 	ldr.w	r3, [r8, r4, lsl #2]
   381c0:	3301      	adds	r3, #1
   381c2:	429a      	cmp	r2, r3
   381c4:	fbb2 f6f3 	udiv	r6, r2, r3
   381c8:	d335      	bcc.n	38236 <quorem+0xa6>
   381ca:	f04f 0e00 	mov.w	lr, #0
   381ce:	4640      	mov	r0, r8
   381d0:	46ac      	mov	ip, r5
   381d2:	46f2      	mov	sl, lr
   381d4:	f850 2b04 	ldr.w	r2, [r0], #4
   381d8:	b293      	uxth	r3, r2
   381da:	4581      	cmp	r9, r0
   381dc:	ea4f 4212 	mov.w	r2, r2, lsr #16
   381e0:	fb06 e303 	mla	r3, r6, r3, lr
   381e4:	ea4f 4e13 	mov.w	lr, r3, lsr #16
   381e8:	b29b      	uxth	r3, r3
   381ea:	fb06 e202 	mla	r2, r6, r2, lr
   381ee:	ebaa 0303 	sub.w	r3, sl, r3
   381f2:	f8dc a000 	ldr.w	sl, [ip]
   381f6:	ea4f 4e12 	mov.w	lr, r2, lsr #16
   381fa:	fa1f fa8a 	uxth.w	sl, sl
   381fe:	b292      	uxth	r2, r2
   38200:	4453      	add	r3, sl
   38202:	9300      	str	r3, [sp, #0]
   38204:	f8dc 3000 	ldr.w	r3, [ip]
   38208:	ebc2 4213 	rsb	r2, r2, r3, lsr #16
   3820c:	9b00      	ldr	r3, [sp, #0]
   3820e:	eb02 4223 	add.w	r2, r2, r3, asr #16
   38212:	f8bd 3000 	ldrh.w	r3, [sp]
   38216:	ea43 4302 	orr.w	r3, r3, r2, lsl #16
   3821a:	ea4f 4a22 	mov.w	sl, r2, asr #16
   3821e:	f84c 3b04 	str.w	r3, [ip], #4
   38222:	d2d7      	bcs.n	381d4 <quorem+0x44>
   38224:	f855 300b 	ldr.w	r3, [r5, fp]
   38228:	b92b      	cbnz	r3, 38236 <quorem+0xa6>
   3822a:	9b01      	ldr	r3, [sp, #4]
   3822c:	3b04      	subs	r3, #4
   3822e:	429d      	cmp	r5, r3
   38230:	461a      	mov	r2, r3
   38232:	d330      	bcc.n	38296 <quorem+0x106>
   38234:	613c      	str	r4, [r7, #16]
   38236:	4638      	mov	r0, r7
   38238:	f002 f99a 	bl	3a570 <__mcmp>
   3823c:	2800      	cmp	r0, #0
   3823e:	db26      	blt.n	3828e <quorem+0xfe>
   38240:	3601      	adds	r6, #1
   38242:	4628      	mov	r0, r5
   38244:	f04f 0c00 	mov.w	ip, #0
   38248:	f858 1b04 	ldr.w	r1, [r8], #4
   3824c:	f8d0 e000 	ldr.w	lr, [r0]
   38250:	b28b      	uxth	r3, r1
   38252:	45c1      	cmp	r9, r8
   38254:	fa1f f28e 	uxth.w	r2, lr
   38258:	ebac 0303 	sub.w	r3, ip, r3
   3825c:	4413      	add	r3, r2
   3825e:	ea4f 4211 	mov.w	r2, r1, lsr #16
   38262:	ebc2 421e 	rsb	r2, r2, lr, lsr #16
   38266:	eb02 4223 	add.w	r2, r2, r3, asr #16
   3826a:	b29b      	uxth	r3, r3
   3826c:	ea4f 4c22 	mov.w	ip, r2, asr #16
   38270:	ea43 4302 	orr.w	r3, r3, r2, lsl #16
   38274:	f840 3b04 	str.w	r3, [r0], #4
   38278:	d2e6      	bcs.n	38248 <quorem+0xb8>
   3827a:	f855 2024 	ldr.w	r2, [r5, r4, lsl #2]
   3827e:	eb05 0384 	add.w	r3, r5, r4, lsl #2
   38282:	b922      	cbnz	r2, 3828e <quorem+0xfe>
   38284:	3b04      	subs	r3, #4
   38286:	429d      	cmp	r5, r3
   38288:	461a      	mov	r2, r3
   3828a:	d30a      	bcc.n	382a2 <quorem+0x112>
   3828c:	613c      	str	r4, [r7, #16]
   3828e:	4630      	mov	r0, r6
   38290:	b003      	add	sp, #12
   38292:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   38296:	6812      	ldr	r2, [r2, #0]
   38298:	3b04      	subs	r3, #4
   3829a:	2a00      	cmp	r2, #0
   3829c:	d1ca      	bne.n	38234 <quorem+0xa4>
   3829e:	3c01      	subs	r4, #1
   382a0:	e7c5      	b.n	3822e <quorem+0x9e>
   382a2:	6812      	ldr	r2, [r2, #0]
   382a4:	3b04      	subs	r3, #4
   382a6:	2a00      	cmp	r2, #0
   382a8:	d1f0      	bne.n	3828c <quorem+0xfc>
   382aa:	3c01      	subs	r4, #1
   382ac:	e7eb      	b.n	38286 <quorem+0xf6>
   382ae:	2000      	movs	r0, #0
   382b0:	e7ee      	b.n	38290 <quorem+0x100>

000382b2 <_dtoa_r>:
   382b2:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   382b6:	ed2d 8b04 	vpush	{d8-d9}
   382ba:	b093      	sub	sp, #76	; 0x4c
   382bc:	4604      	mov	r4, r0
   382be:	ee10 aa10 	vmov	sl, s0
   382c2:	9106      	str	r1, [sp, #24]
   382c4:	6c01      	ldr	r1, [r0, #64]	; 0x40
   382c6:	9d20      	ldr	r5, [sp, #128]	; 0x80
   382c8:	9209      	str	r2, [sp, #36]	; 0x24
   382ca:	930c      	str	r3, [sp, #48]	; 0x30
   382cc:	ec57 6b10 	vmov	r6, r7, d0
   382d0:	46bb      	mov	fp, r7
   382d2:	b141      	cbz	r1, 382e6 <_dtoa_r+0x34>
   382d4:	6c42      	ldr	r2, [r0, #68]	; 0x44
   382d6:	2301      	movs	r3, #1
   382d8:	4093      	lsls	r3, r2
   382da:	604a      	str	r2, [r1, #4]
   382dc:	608b      	str	r3, [r1, #8]
   382de:	f001 fef0 	bl	3a0c2 <_Bfree>
   382e2:	2300      	movs	r3, #0
   382e4:	6423      	str	r3, [r4, #64]	; 0x40
   382e6:	1e3b      	subs	r3, r7, #0
   382e8:	bfad      	iteet	ge
   382ea:	2300      	movge	r3, #0
   382ec:	2201      	movlt	r2, #1
   382ee:	f023 4b00 	biclt.w	fp, r3, #2147483648	; 0x80000000
   382f2:	602b      	strge	r3, [r5, #0]
   382f4:	4ba0      	ldr	r3, [pc, #640]	; (38578 <_dtoa_r+0x2c6>)
   382f6:	bfb8      	it	lt
   382f8:	602a      	strlt	r2, [r5, #0]
   382fa:	ea33 030b 	bics.w	r3, r3, fp
   382fe:	d11b      	bne.n	38338 <_dtoa_r+0x86>
   38300:	f242 730f 	movw	r3, #9999	; 0x270f
   38304:	9a0c      	ldr	r2, [sp, #48]	; 0x30
   38306:	6013      	str	r3, [r2, #0]
   38308:	f3cb 0313 	ubfx	r3, fp, #0, #20
   3830c:	4333      	orrs	r3, r6
   3830e:	f000 8588 	beq.w	38e22 <_dtoa_r+0xb70>
   38312:	9b21      	ldr	r3, [sp, #132]	; 0x84
   38314:	b90b      	cbnz	r3, 3831a <_dtoa_r+0x68>
   38316:	4b99      	ldr	r3, [pc, #612]	; (3857c <_dtoa_r+0x2ca>)
   38318:	e022      	b.n	38360 <_dtoa_r+0xae>
   3831a:	4b98      	ldr	r3, [pc, #608]	; (3857c <_dtoa_r+0x2ca>)
   3831c:	9301      	str	r3, [sp, #4]
   3831e:	3303      	adds	r3, #3
   38320:	9a21      	ldr	r2, [sp, #132]	; 0x84
   38322:	6013      	str	r3, [r2, #0]
   38324:	9801      	ldr	r0, [sp, #4]
   38326:	b013      	add	sp, #76	; 0x4c
   38328:	ecbd 8b04 	vpop	{d8-d9}
   3832c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   38330:	4b93      	ldr	r3, [pc, #588]	; (38580 <_dtoa_r+0x2ce>)
   38332:	9301      	str	r3, [sp, #4]
   38334:	3308      	adds	r3, #8
   38336:	e7f3      	b.n	38320 <_dtoa_r+0x6e>
   38338:	2200      	movs	r2, #0
   3833a:	2300      	movs	r3, #0
   3833c:	4650      	mov	r0, sl
   3833e:	4659      	mov	r1, fp
   38340:	f007 fd64 	bl	3fe0c <__aeabi_dcmpeq>
   38344:	4680      	mov	r8, r0
   38346:	ec4b ab19 	vmov	d9, sl, fp
   3834a:	b158      	cbz	r0, 38364 <_dtoa_r+0xb2>
   3834c:	2301      	movs	r3, #1
   3834e:	9a0c      	ldr	r2, [sp, #48]	; 0x30
   38350:	6013      	str	r3, [r2, #0]
   38352:	9b21      	ldr	r3, [sp, #132]	; 0x84
   38354:	2b00      	cmp	r3, #0
   38356:	f000 8561 	beq.w	38e1c <_dtoa_r+0xb6a>
   3835a:	488a      	ldr	r0, [pc, #552]	; (38584 <_dtoa_r+0x2d2>)
   3835c:	6018      	str	r0, [r3, #0]
   3835e:	1e43      	subs	r3, r0, #1
   38360:	9301      	str	r3, [sp, #4]
   38362:	e7df      	b.n	38324 <_dtoa_r+0x72>
   38364:	f3cb 550a 	ubfx	r5, fp, #20, #11
   38368:	aa10      	add	r2, sp, #64	; 0x40
   3836a:	a911      	add	r1, sp, #68	; 0x44
   3836c:	4620      	mov	r0, r4
   3836e:	ec4b ab10 	vmov	d0, sl, fp
   38372:	f002 fa23 	bl	3a7bc <__d2b>
   38376:	ee08 0a10 	vmov	s16, r0
   3837a:	2d00      	cmp	r5, #0
   3837c:	d07f      	beq.n	3847e <_dtoa_r+0x1cc>
   3837e:	ee19 3a90 	vmov	r3, s19
   38382:	4656      	mov	r6, sl
   38384:	f2a5 35ff 	subw	r5, r5, #1023	; 0x3ff
   38388:	f8cd 8038 	str.w	r8, [sp, #56]	; 0x38
   3838c:	f3c3 0313 	ubfx	r3, r3, #0, #20
   38390:	f043 577f 	orr.w	r7, r3, #1069547520	; 0x3fc00000
   38394:	f447 1740 	orr.w	r7, r7, #3145728	; 0x300000
   38398:	2200      	movs	r2, #0
   3839a:	4b7b      	ldr	r3, [pc, #492]	; (38588 <_dtoa_r+0x2d6>)
   3839c:	4630      	mov	r0, r6
   3839e:	4639      	mov	r1, r7
   383a0:	f007 f914 	bl	3f5cc <__aeabi_dsub>
   383a4:	a36e      	add	r3, pc, #440	; (adr r3, 38560 <_dtoa_r+0x2ae>)
   383a6:	e9d3 2300 	ldrd	r2, r3, [r3]
   383aa:	f007 fac7 	bl	3f93c <__aeabi_dmul>
   383ae:	a36e      	add	r3, pc, #440	; (adr r3, 38568 <_dtoa_r+0x2b6>)
   383b0:	e9d3 2300 	ldrd	r2, r3, [r3]
   383b4:	f007 f90c 	bl	3f5d0 <__adddf3>
   383b8:	4606      	mov	r6, r0
   383ba:	460f      	mov	r7, r1
   383bc:	4628      	mov	r0, r5
   383be:	f007 fa53 	bl	3f868 <__aeabi_i2d>
   383c2:	a36b      	add	r3, pc, #428	; (adr r3, 38570 <_dtoa_r+0x2be>)
   383c4:	e9d3 2300 	ldrd	r2, r3, [r3]
   383c8:	f007 fab8 	bl	3f93c <__aeabi_dmul>
   383cc:	4602      	mov	r2, r0
   383ce:	460b      	mov	r3, r1
   383d0:	4630      	mov	r0, r6
   383d2:	4639      	mov	r1, r7
   383d4:	f007 f8fc 	bl	3f5d0 <__adddf3>
   383d8:	4606      	mov	r6, r0
   383da:	460f      	mov	r7, r1
   383dc:	f007 fd5e 	bl	3fe9c <__aeabi_d2iz>
   383e0:	2200      	movs	r2, #0
   383e2:	9000      	str	r0, [sp, #0]
   383e4:	2300      	movs	r3, #0
   383e6:	4630      	mov	r0, r6
   383e8:	4639      	mov	r1, r7
   383ea:	f007 fd19 	bl	3fe20 <__aeabi_dcmplt>
   383ee:	b150      	cbz	r0, 38406 <_dtoa_r+0x154>
   383f0:	9800      	ldr	r0, [sp, #0]
   383f2:	f007 fa39 	bl	3f868 <__aeabi_i2d>
   383f6:	4632      	mov	r2, r6
   383f8:	463b      	mov	r3, r7
   383fa:	f007 fd07 	bl	3fe0c <__aeabi_dcmpeq>
   383fe:	b910      	cbnz	r0, 38406 <_dtoa_r+0x154>
   38400:	9b00      	ldr	r3, [sp, #0]
   38402:	3b01      	subs	r3, #1
   38404:	9300      	str	r3, [sp, #0]
   38406:	9b00      	ldr	r3, [sp, #0]
   38408:	2b16      	cmp	r3, #22
   3840a:	d857      	bhi.n	384bc <_dtoa_r+0x20a>
   3840c:	9a00      	ldr	r2, [sp, #0]
   3840e:	4b5f      	ldr	r3, [pc, #380]	; (3858c <_dtoa_r+0x2da>)
   38410:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
   38414:	ec51 0b19 	vmov	r0, r1, d9
   38418:	e9d3 2300 	ldrd	r2, r3, [r3]
   3841c:	f007 fd00 	bl	3fe20 <__aeabi_dcmplt>
   38420:	2800      	cmp	r0, #0
   38422:	d04d      	beq.n	384c0 <_dtoa_r+0x20e>
   38424:	9b00      	ldr	r3, [sp, #0]
   38426:	3b01      	subs	r3, #1
   38428:	9300      	str	r3, [sp, #0]
   3842a:	2300      	movs	r3, #0
   3842c:	930b      	str	r3, [sp, #44]	; 0x2c
   3842e:	9b10      	ldr	r3, [sp, #64]	; 0x40
   38430:	1b5d      	subs	r5, r3, r5
   38432:	1e6b      	subs	r3, r5, #1
   38434:	9305      	str	r3, [sp, #20]
   38436:	bf49      	itett	mi
   38438:	f1c5 0301 	rsbmi	r3, r5, #1
   3843c:	2300      	movpl	r3, #0
   3843e:	9304      	strmi	r3, [sp, #16]
   38440:	2300      	movmi	r3, #0
   38442:	bf54      	ite	pl
   38444:	9304      	strpl	r3, [sp, #16]
   38446:	9305      	strmi	r3, [sp, #20]
   38448:	9b00      	ldr	r3, [sp, #0]
   3844a:	2b00      	cmp	r3, #0
   3844c:	db3a      	blt.n	384c4 <_dtoa_r+0x212>
   3844e:	9b05      	ldr	r3, [sp, #20]
   38450:	9a00      	ldr	r2, [sp, #0]
   38452:	4413      	add	r3, r2
   38454:	920a      	str	r2, [sp, #40]	; 0x28
   38456:	9305      	str	r3, [sp, #20]
   38458:	2300      	movs	r3, #0
   3845a:	9307      	str	r3, [sp, #28]
   3845c:	9b06      	ldr	r3, [sp, #24]
   3845e:	2b09      	cmp	r3, #9
   38460:	d868      	bhi.n	38534 <_dtoa_r+0x282>
   38462:	2b05      	cmp	r3, #5
   38464:	bfc5      	ittet	gt
   38466:	3b04      	subgt	r3, #4
   38468:	2500      	movgt	r5, #0
   3846a:	2501      	movle	r5, #1
   3846c:	9306      	strgt	r3, [sp, #24]
   3846e:	9b06      	ldr	r3, [sp, #24]
   38470:	3b02      	subs	r3, #2
   38472:	2b03      	cmp	r3, #3
   38474:	d869      	bhi.n	3854a <_dtoa_r+0x298>
   38476:	e8df f003 	tbb	[pc, r3]
   3847a:	3b2e      	.short	0x3b2e
   3847c:	5b39      	.short	0x5b39
   3847e:	e9dd 5310 	ldrd	r5, r3, [sp, #64]	; 0x40
   38482:	441d      	add	r5, r3
   38484:	f205 4332 	addw	r3, r5, #1074	; 0x432
   38488:	2b20      	cmp	r3, #32
   3848a:	bfc6      	itte	gt
   3848c:	f1c3 0340 	rsbgt	r3, r3, #64	; 0x40
   38490:	f205 4012 	addwgt	r0, r5, #1042	; 0x412
   38494:	f1c3 0320 	rsble	r3, r3, #32
   38498:	f105 35ff 	add.w	r5, r5, #4294967295	; 0xffffffff
   3849c:	bfc5      	ittet	gt
   3849e:	fa0b f303 	lslgt.w	r3, fp, r3
   384a2:	fa26 f000 	lsrgt.w	r0, r6, r0
   384a6:	fa06 f003 	lslle.w	r0, r6, r3
   384aa:	4318      	orrgt	r0, r3
   384ac:	f007 f9cc 	bl	3f848 <__aeabi_ui2d>
   384b0:	2301      	movs	r3, #1
   384b2:	4606      	mov	r6, r0
   384b4:	f1a1 77f8 	sub.w	r7, r1, #32505856	; 0x1f00000
   384b8:	930e      	str	r3, [sp, #56]	; 0x38
   384ba:	e76d      	b.n	38398 <_dtoa_r+0xe6>
   384bc:	2301      	movs	r3, #1
   384be:	e7b5      	b.n	3842c <_dtoa_r+0x17a>
   384c0:	900b      	str	r0, [sp, #44]	; 0x2c
   384c2:	e7b4      	b.n	3842e <_dtoa_r+0x17c>
   384c4:	9b04      	ldr	r3, [sp, #16]
   384c6:	9a00      	ldr	r2, [sp, #0]
   384c8:	1a9b      	subs	r3, r3, r2
   384ca:	9304      	str	r3, [sp, #16]
   384cc:	4253      	negs	r3, r2
   384ce:	9307      	str	r3, [sp, #28]
   384d0:	2300      	movs	r3, #0
   384d2:	930a      	str	r3, [sp, #40]	; 0x28
   384d4:	e7c2      	b.n	3845c <_dtoa_r+0x1aa>
   384d6:	2300      	movs	r3, #0
   384d8:	9308      	str	r3, [sp, #32]
   384da:	9b09      	ldr	r3, [sp, #36]	; 0x24
   384dc:	2b00      	cmp	r3, #0
   384de:	dc37      	bgt.n	38550 <_dtoa_r+0x29e>
   384e0:	2301      	movs	r3, #1
   384e2:	461a      	mov	r2, r3
   384e4:	e9cd 3302 	strd	r3, r3, [sp, #8]
   384e8:	9209      	str	r2, [sp, #36]	; 0x24
   384ea:	e00c      	b.n	38506 <_dtoa_r+0x254>
   384ec:	2301      	movs	r3, #1
   384ee:	e7f3      	b.n	384d8 <_dtoa_r+0x226>
   384f0:	2300      	movs	r3, #0
   384f2:	9a09      	ldr	r2, [sp, #36]	; 0x24
   384f4:	9308      	str	r3, [sp, #32]
   384f6:	9b00      	ldr	r3, [sp, #0]
   384f8:	4413      	add	r3, r2
   384fa:	9302      	str	r3, [sp, #8]
   384fc:	3301      	adds	r3, #1
   384fe:	2b01      	cmp	r3, #1
   38500:	9303      	str	r3, [sp, #12]
   38502:	bfb8      	it	lt
   38504:	2301      	movlt	r3, #1
   38506:	2200      	movs	r2, #0
   38508:	6462      	str	r2, [r4, #68]	; 0x44
   3850a:	2204      	movs	r2, #4
   3850c:	f102 0014 	add.w	r0, r2, #20
   38510:	6c61      	ldr	r1, [r4, #68]	; 0x44
   38512:	4298      	cmp	r0, r3
   38514:	d920      	bls.n	38558 <_dtoa_r+0x2a6>
   38516:	4620      	mov	r0, r4
   38518:	f001 fdae 	bl	3a078 <_Balloc>
   3851c:	9001      	str	r0, [sp, #4]
   3851e:	2800      	cmp	r0, #0
   38520:	d13a      	bne.n	38598 <_dtoa_r+0x2e6>
   38522:	4b1b      	ldr	r3, [pc, #108]	; (38590 <_dtoa_r+0x2de>)
   38524:	4602      	mov	r2, r0
   38526:	f44f 71d5 	mov.w	r1, #426	; 0x1aa
   3852a:	481a      	ldr	r0, [pc, #104]	; (38594 <_dtoa_r+0x2e2>)
   3852c:	f006 f948 	bl	3e7c0 <__assert_func>
   38530:	2301      	movs	r3, #1
   38532:	e7de      	b.n	384f2 <_dtoa_r+0x240>
   38534:	2501      	movs	r5, #1
   38536:	2300      	movs	r3, #0
   38538:	9508      	str	r5, [sp, #32]
   3853a:	9306      	str	r3, [sp, #24]
   3853c:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   38540:	2200      	movs	r2, #0
   38542:	e9cd 3302 	strd	r3, r3, [sp, #8]
   38546:	2312      	movs	r3, #18
   38548:	e7ce      	b.n	384e8 <_dtoa_r+0x236>
   3854a:	2301      	movs	r3, #1
   3854c:	9308      	str	r3, [sp, #32]
   3854e:	e7f5      	b.n	3853c <_dtoa_r+0x28a>
   38550:	9b09      	ldr	r3, [sp, #36]	; 0x24
   38552:	e9cd 3302 	strd	r3, r3, [sp, #8]
   38556:	e7d6      	b.n	38506 <_dtoa_r+0x254>
   38558:	3101      	adds	r1, #1
   3855a:	0052      	lsls	r2, r2, #1
   3855c:	6461      	str	r1, [r4, #68]	; 0x44
   3855e:	e7d5      	b.n	3850c <_dtoa_r+0x25a>
   38560:	636f4361 	.word	0x636f4361
   38564:	3fd287a7 	.word	0x3fd287a7
   38568:	8b60c8b3 	.word	0x8b60c8b3
   3856c:	3fc68a28 	.word	0x3fc68a28
   38570:	509f79fb 	.word	0x509f79fb
   38574:	3fd34413 	.word	0x3fd34413
   38578:	7ff00000 	.word	0x7ff00000
   3857c:	0004066b 	.word	0x0004066b
   38580:	0004066f 	.word	0x0004066f
   38584:	00040502 	.word	0x00040502
   38588:	3ff80000 	.word	0x3ff80000
   3858c:	00040798 	.word	0x00040798
   38590:	00040678 	.word	0x00040678
   38594:	00040689 	.word	0x00040689
   38598:	9b01      	ldr	r3, [sp, #4]
   3859a:	6423      	str	r3, [r4, #64]	; 0x40
   3859c:	9b03      	ldr	r3, [sp, #12]
   3859e:	2b0e      	cmp	r3, #14
   385a0:	f200 809d 	bhi.w	386de <_dtoa_r+0x42c>
   385a4:	2d00      	cmp	r5, #0
   385a6:	f000 809a 	beq.w	386de <_dtoa_r+0x42c>
   385aa:	9b00      	ldr	r3, [sp, #0]
   385ac:	2b00      	cmp	r3, #0
   385ae:	dd32      	ble.n	38616 <_dtoa_r+0x364>
   385b0:	f003 030f 	and.w	r3, r3, #15
   385b4:	4ab6      	ldr	r2, [pc, #728]	; (38890 <_dtoa_r+0x5de>)
   385b6:	eb02 03c3 	add.w	r3, r2, r3, lsl #3
   385ba:	e9d3 8900 	ldrd	r8, r9, [r3]
   385be:	9b00      	ldr	r3, [sp, #0]
   385c0:	05d8      	lsls	r0, r3, #23
   385c2:	ea4f 1723 	mov.w	r7, r3, asr #4
   385c6:	d516      	bpl.n	385f6 <_dtoa_r+0x344>
   385c8:	4bb2      	ldr	r3, [pc, #712]	; (38894 <_dtoa_r+0x5e2>)
   385ca:	f007 070f 	and.w	r7, r7, #15
   385ce:	2503      	movs	r5, #3
   385d0:	e9d3 2308 	ldrd	r2, r3, [r3, #32]
   385d4:	ec51 0b19 	vmov	r0, r1, d9
   385d8:	f007 fada 	bl	3fb90 <__aeabi_ddiv>
   385dc:	4682      	mov	sl, r0
   385de:	468b      	mov	fp, r1
   385e0:	4eac      	ldr	r6, [pc, #688]	; (38894 <_dtoa_r+0x5e2>)
   385e2:	b957      	cbnz	r7, 385fa <_dtoa_r+0x348>
   385e4:	4642      	mov	r2, r8
   385e6:	464b      	mov	r3, r9
   385e8:	4650      	mov	r0, sl
   385ea:	4659      	mov	r1, fp
   385ec:	f007 fad0 	bl	3fb90 <__aeabi_ddiv>
   385f0:	4682      	mov	sl, r0
   385f2:	468b      	mov	fp, r1
   385f4:	e028      	b.n	38648 <_dtoa_r+0x396>
   385f6:	2502      	movs	r5, #2
   385f8:	e7f2      	b.n	385e0 <_dtoa_r+0x32e>
   385fa:	07f9      	lsls	r1, r7, #31
   385fc:	d508      	bpl.n	38610 <_dtoa_r+0x35e>
   385fe:	4640      	mov	r0, r8
   38600:	4649      	mov	r1, r9
   38602:	3501      	adds	r5, #1
   38604:	e9d6 2300 	ldrd	r2, r3, [r6]
   38608:	f007 f998 	bl	3f93c <__aeabi_dmul>
   3860c:	4680      	mov	r8, r0
   3860e:	4689      	mov	r9, r1
   38610:	107f      	asrs	r7, r7, #1
   38612:	3608      	adds	r6, #8
   38614:	e7e5      	b.n	385e2 <_dtoa_r+0x330>
   38616:	f000 809b 	beq.w	38750 <_dtoa_r+0x49e>
   3861a:	9b00      	ldr	r3, [sp, #0]
   3861c:	2502      	movs	r5, #2
   3861e:	4f9d      	ldr	r7, [pc, #628]	; (38894 <_dtoa_r+0x5e2>)
   38620:	425e      	negs	r6, r3
   38622:	4b9b      	ldr	r3, [pc, #620]	; (38890 <_dtoa_r+0x5de>)
   38624:	f006 020f 	and.w	r2, r6, #15
   38628:	1136      	asrs	r6, r6, #4
   3862a:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
   3862e:	ec51 0b19 	vmov	r0, r1, d9
   38632:	e9d3 2300 	ldrd	r2, r3, [r3]
   38636:	f007 f981 	bl	3f93c <__aeabi_dmul>
   3863a:	2300      	movs	r3, #0
   3863c:	4682      	mov	sl, r0
   3863e:	468b      	mov	fp, r1
   38640:	2e00      	cmp	r6, #0
   38642:	d17a      	bne.n	3873a <_dtoa_r+0x488>
   38644:	2b00      	cmp	r3, #0
   38646:	d1d3      	bne.n	385f0 <_dtoa_r+0x33e>
   38648:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   3864a:	2b00      	cmp	r3, #0
   3864c:	f000 8082 	beq.w	38754 <_dtoa_r+0x4a2>
   38650:	2200      	movs	r2, #0
   38652:	4b91      	ldr	r3, [pc, #580]	; (38898 <_dtoa_r+0x5e6>)
   38654:	4650      	mov	r0, sl
   38656:	4659      	mov	r1, fp
   38658:	f007 fbe2 	bl	3fe20 <__aeabi_dcmplt>
   3865c:	2800      	cmp	r0, #0
   3865e:	d079      	beq.n	38754 <_dtoa_r+0x4a2>
   38660:	9b03      	ldr	r3, [sp, #12]
   38662:	2b00      	cmp	r3, #0
   38664:	d076      	beq.n	38754 <_dtoa_r+0x4a2>
   38666:	9b02      	ldr	r3, [sp, #8]
   38668:	2b00      	cmp	r3, #0
   3866a:	dd36      	ble.n	386da <_dtoa_r+0x428>
   3866c:	9b00      	ldr	r3, [sp, #0]
   3866e:	4650      	mov	r0, sl
   38670:	4659      	mov	r1, fp
   38672:	2200      	movs	r2, #0
   38674:	1e5f      	subs	r7, r3, #1
   38676:	4b89      	ldr	r3, [pc, #548]	; (3889c <_dtoa_r+0x5ea>)
   38678:	f007 f960 	bl	3f93c <__aeabi_dmul>
   3867c:	3501      	adds	r5, #1
   3867e:	4682      	mov	sl, r0
   38680:	468b      	mov	fp, r1
   38682:	9e02      	ldr	r6, [sp, #8]
   38684:	4628      	mov	r0, r5
   38686:	46d0      	mov	r8, sl
   38688:	f007 f8ee 	bl	3f868 <__aeabi_i2d>
   3868c:	4652      	mov	r2, sl
   3868e:	465b      	mov	r3, fp
   38690:	46d9      	mov	r9, fp
   38692:	f007 f953 	bl	3f93c <__aeabi_dmul>
   38696:	2200      	movs	r2, #0
   38698:	4b81      	ldr	r3, [pc, #516]	; (388a0 <_dtoa_r+0x5ee>)
   3869a:	f006 ff99 	bl	3f5d0 <__adddf3>
   3869e:	4682      	mov	sl, r0
   386a0:	f1a1 7b50 	sub.w	fp, r1, #54525952	; 0x3400000
   386a4:	2e00      	cmp	r6, #0
   386a6:	d158      	bne.n	3875a <_dtoa_r+0x4a8>
   386a8:	2200      	movs	r2, #0
   386aa:	4b7e      	ldr	r3, [pc, #504]	; (388a4 <_dtoa_r+0x5f2>)
   386ac:	4640      	mov	r0, r8
   386ae:	4649      	mov	r1, r9
   386b0:	f006 ff8c 	bl	3f5cc <__aeabi_dsub>
   386b4:	4652      	mov	r2, sl
   386b6:	465b      	mov	r3, fp
   386b8:	4680      	mov	r8, r0
   386ba:	4689      	mov	r9, r1
   386bc:	f007 fbce 	bl	3fe5c <__aeabi_dcmpgt>
   386c0:	2800      	cmp	r0, #0
   386c2:	f040 8294 	bne.w	38bee <_dtoa_r+0x93c>
   386c6:	4652      	mov	r2, sl
   386c8:	f10b 4300 	add.w	r3, fp, #2147483648	; 0x80000000
   386cc:	4640      	mov	r0, r8
   386ce:	4649      	mov	r1, r9
   386d0:	f007 fba6 	bl	3fe20 <__aeabi_dcmplt>
   386d4:	2800      	cmp	r0, #0
   386d6:	f040 8288 	bne.w	38bea <_dtoa_r+0x938>
   386da:	ec5b ab19 	vmov	sl, fp, d9
   386de:	9b11      	ldr	r3, [sp, #68]	; 0x44
   386e0:	2b00      	cmp	r3, #0
   386e2:	f2c0 8149 	blt.w	38978 <_dtoa_r+0x6c6>
   386e6:	9a00      	ldr	r2, [sp, #0]
   386e8:	2a0e      	cmp	r2, #14
   386ea:	f300 8145 	bgt.w	38978 <_dtoa_r+0x6c6>
   386ee:	4b68      	ldr	r3, [pc, #416]	; (38890 <_dtoa_r+0x5de>)
   386f0:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
   386f4:	e9d3 8900 	ldrd	r8, r9, [r3]
   386f8:	9b09      	ldr	r3, [sp, #36]	; 0x24
   386fa:	2b00      	cmp	r3, #0
   386fc:	f280 80d6 	bge.w	388ac <_dtoa_r+0x5fa>
   38700:	9b03      	ldr	r3, [sp, #12]
   38702:	2b00      	cmp	r3, #0
   38704:	f300 80d2 	bgt.w	388ac <_dtoa_r+0x5fa>
   38708:	f040 826e 	bne.w	38be8 <_dtoa_r+0x936>
   3870c:	2200      	movs	r2, #0
   3870e:	4b65      	ldr	r3, [pc, #404]	; (388a4 <_dtoa_r+0x5f2>)
   38710:	4640      	mov	r0, r8
   38712:	4649      	mov	r1, r9
   38714:	f007 f912 	bl	3f93c <__aeabi_dmul>
   38718:	4652      	mov	r2, sl
   3871a:	465b      	mov	r3, fp
   3871c:	f007 fb94 	bl	3fe48 <__aeabi_dcmpge>
   38720:	9e03      	ldr	r6, [sp, #12]
   38722:	4637      	mov	r7, r6
   38724:	2800      	cmp	r0, #0
   38726:	f040 8244 	bne.w	38bb2 <_dtoa_r+0x900>
   3872a:	2331      	movs	r3, #49	; 0x31
   3872c:	9d01      	ldr	r5, [sp, #4]
   3872e:	f805 3b01 	strb.w	r3, [r5], #1
   38732:	9b00      	ldr	r3, [sp, #0]
   38734:	3301      	adds	r3, #1
   38736:	9300      	str	r3, [sp, #0]
   38738:	e23f      	b.n	38bba <_dtoa_r+0x908>
   3873a:	07f2      	lsls	r2, r6, #31
   3873c:	d505      	bpl.n	3874a <_dtoa_r+0x498>
   3873e:	3501      	adds	r5, #1
   38740:	e9d7 2300 	ldrd	r2, r3, [r7]
   38744:	f007 f8fa 	bl	3f93c <__aeabi_dmul>
   38748:	2301      	movs	r3, #1
   3874a:	1076      	asrs	r6, r6, #1
   3874c:	3708      	adds	r7, #8
   3874e:	e777      	b.n	38640 <_dtoa_r+0x38e>
   38750:	2502      	movs	r5, #2
   38752:	e779      	b.n	38648 <_dtoa_r+0x396>
   38754:	9f00      	ldr	r7, [sp, #0]
   38756:	9e03      	ldr	r6, [sp, #12]
   38758:	e794      	b.n	38684 <_dtoa_r+0x3d2>
   3875a:	9901      	ldr	r1, [sp, #4]
   3875c:	4b4c      	ldr	r3, [pc, #304]	; (38890 <_dtoa_r+0x5de>)
   3875e:	4431      	add	r1, r6
   38760:	eb03 03c6 	add.w	r3, r3, r6, lsl #3
   38764:	910d      	str	r1, [sp, #52]	; 0x34
   38766:	9908      	ldr	r1, [sp, #32]
   38768:	e953 2302 	ldrd	r2, r3, [r3, #-8]
   3876c:	2900      	cmp	r1, #0
   3876e:	d043      	beq.n	387f8 <_dtoa_r+0x546>
   38770:	2000      	movs	r0, #0
   38772:	494d      	ldr	r1, [pc, #308]	; (388a8 <_dtoa_r+0x5f6>)
   38774:	f007 fa0c 	bl	3fb90 <__aeabi_ddiv>
   38778:	4652      	mov	r2, sl
   3877a:	465b      	mov	r3, fp
   3877c:	f006 ff26 	bl	3f5cc <__aeabi_dsub>
   38780:	9d01      	ldr	r5, [sp, #4]
   38782:	4682      	mov	sl, r0
   38784:	468b      	mov	fp, r1
   38786:	4649      	mov	r1, r9
   38788:	4640      	mov	r0, r8
   3878a:	f007 fb87 	bl	3fe9c <__aeabi_d2iz>
   3878e:	4606      	mov	r6, r0
   38790:	f007 f86a 	bl	3f868 <__aeabi_i2d>
   38794:	4602      	mov	r2, r0
   38796:	3630      	adds	r6, #48	; 0x30
   38798:	460b      	mov	r3, r1
   3879a:	4640      	mov	r0, r8
   3879c:	4649      	mov	r1, r9
   3879e:	f006 ff15 	bl	3f5cc <__aeabi_dsub>
   387a2:	4652      	mov	r2, sl
   387a4:	465b      	mov	r3, fp
   387a6:	f805 6b01 	strb.w	r6, [r5], #1
   387aa:	4680      	mov	r8, r0
   387ac:	4689      	mov	r9, r1
   387ae:	f007 fb37 	bl	3fe20 <__aeabi_dcmplt>
   387b2:	2800      	cmp	r0, #0
   387b4:	d163      	bne.n	3887e <_dtoa_r+0x5cc>
   387b6:	4642      	mov	r2, r8
   387b8:	464b      	mov	r3, r9
   387ba:	2000      	movs	r0, #0
   387bc:	4936      	ldr	r1, [pc, #216]	; (38898 <_dtoa_r+0x5e6>)
   387be:	f006 ff05 	bl	3f5cc <__aeabi_dsub>
   387c2:	4652      	mov	r2, sl
   387c4:	465b      	mov	r3, fp
   387c6:	f007 fb2b 	bl	3fe20 <__aeabi_dcmplt>
   387ca:	2800      	cmp	r0, #0
   387cc:	f040 80b6 	bne.w	3893c <_dtoa_r+0x68a>
   387d0:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   387d2:	429d      	cmp	r5, r3
   387d4:	d081      	beq.n	386da <_dtoa_r+0x428>
   387d6:	2200      	movs	r2, #0
   387d8:	4b30      	ldr	r3, [pc, #192]	; (3889c <_dtoa_r+0x5ea>)
   387da:	4650      	mov	r0, sl
   387dc:	4659      	mov	r1, fp
   387de:	f007 f8ad 	bl	3f93c <__aeabi_dmul>
   387e2:	2200      	movs	r2, #0
   387e4:	4682      	mov	sl, r0
   387e6:	468b      	mov	fp, r1
   387e8:	4640      	mov	r0, r8
   387ea:	4649      	mov	r1, r9
   387ec:	4b2b      	ldr	r3, [pc, #172]	; (3889c <_dtoa_r+0x5ea>)
   387ee:	f007 f8a5 	bl	3f93c <__aeabi_dmul>
   387f2:	4680      	mov	r8, r0
   387f4:	4689      	mov	r9, r1
   387f6:	e7c6      	b.n	38786 <_dtoa_r+0x4d4>
   387f8:	4650      	mov	r0, sl
   387fa:	4659      	mov	r1, fp
   387fc:	f007 f89e 	bl	3f93c <__aeabi_dmul>
   38800:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   38802:	4682      	mov	sl, r0
   38804:	468b      	mov	fp, r1
   38806:	9d01      	ldr	r5, [sp, #4]
   38808:	930f      	str	r3, [sp, #60]	; 0x3c
   3880a:	4649      	mov	r1, r9
   3880c:	4640      	mov	r0, r8
   3880e:	f007 fb45 	bl	3fe9c <__aeabi_d2iz>
   38812:	4606      	mov	r6, r0
   38814:	f007 f828 	bl	3f868 <__aeabi_i2d>
   38818:	4602      	mov	r2, r0
   3881a:	460b      	mov	r3, r1
   3881c:	4640      	mov	r0, r8
   3881e:	4649      	mov	r1, r9
   38820:	3630      	adds	r6, #48	; 0x30
   38822:	f006 fed3 	bl	3f5cc <__aeabi_dsub>
   38826:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   38828:	f805 6b01 	strb.w	r6, [r5], #1
   3882c:	4680      	mov	r8, r0
   3882e:	429d      	cmp	r5, r3
   38830:	4689      	mov	r9, r1
   38832:	f04f 0200 	mov.w	r2, #0
   38836:	d124      	bne.n	38882 <_dtoa_r+0x5d0>
   38838:	4b1b      	ldr	r3, [pc, #108]	; (388a8 <_dtoa_r+0x5f6>)
   3883a:	4650      	mov	r0, sl
   3883c:	4659      	mov	r1, fp
   3883e:	f006 fec7 	bl	3f5d0 <__adddf3>
   38842:	4602      	mov	r2, r0
   38844:	460b      	mov	r3, r1
   38846:	4640      	mov	r0, r8
   38848:	4649      	mov	r1, r9
   3884a:	f007 fb07 	bl	3fe5c <__aeabi_dcmpgt>
   3884e:	2800      	cmp	r0, #0
   38850:	d174      	bne.n	3893c <_dtoa_r+0x68a>
   38852:	4652      	mov	r2, sl
   38854:	465b      	mov	r3, fp
   38856:	2000      	movs	r0, #0
   38858:	4913      	ldr	r1, [pc, #76]	; (388a8 <_dtoa_r+0x5f6>)
   3885a:	f006 feb7 	bl	3f5cc <__aeabi_dsub>
   3885e:	4602      	mov	r2, r0
   38860:	460b      	mov	r3, r1
   38862:	4640      	mov	r0, r8
   38864:	4649      	mov	r1, r9
   38866:	f007 fadb 	bl	3fe20 <__aeabi_dcmplt>
   3886a:	2800      	cmp	r0, #0
   3886c:	f43f af35 	beq.w	386da <_dtoa_r+0x428>
   38870:	9d0f      	ldr	r5, [sp, #60]	; 0x3c
   38872:	1e6b      	subs	r3, r5, #1
   38874:	930f      	str	r3, [sp, #60]	; 0x3c
   38876:	f815 3c01 	ldrb.w	r3, [r5, #-1]
   3887a:	2b30      	cmp	r3, #48	; 0x30
   3887c:	d0f8      	beq.n	38870 <_dtoa_r+0x5be>
   3887e:	9700      	str	r7, [sp, #0]
   38880:	e04a      	b.n	38918 <_dtoa_r+0x666>
   38882:	4b06      	ldr	r3, [pc, #24]	; (3889c <_dtoa_r+0x5ea>)
   38884:	f007 f85a 	bl	3f93c <__aeabi_dmul>
   38888:	4680      	mov	r8, r0
   3888a:	4689      	mov	r9, r1
   3888c:	e7bd      	b.n	3880a <_dtoa_r+0x558>
   3888e:	bf00      	nop
   38890:	00040798 	.word	0x00040798
   38894:	00040888 	.word	0x00040888
   38898:	3ff00000 	.word	0x3ff00000
   3889c:	40240000 	.word	0x40240000
   388a0:	401c0000 	.word	0x401c0000
   388a4:	40140000 	.word	0x40140000
   388a8:	3fe00000 	.word	0x3fe00000
   388ac:	4656      	mov	r6, sl
   388ae:	465f      	mov	r7, fp
   388b0:	9d01      	ldr	r5, [sp, #4]
   388b2:	4642      	mov	r2, r8
   388b4:	464b      	mov	r3, r9
   388b6:	4630      	mov	r0, r6
   388b8:	4639      	mov	r1, r7
   388ba:	f007 f969 	bl	3fb90 <__aeabi_ddiv>
   388be:	f007 faed 	bl	3fe9c <__aeabi_d2iz>
   388c2:	4682      	mov	sl, r0
   388c4:	f006 ffd0 	bl	3f868 <__aeabi_i2d>
   388c8:	4642      	mov	r2, r8
   388ca:	464b      	mov	r3, r9
   388cc:	f007 f836 	bl	3f93c <__aeabi_dmul>
   388d0:	4602      	mov	r2, r0
   388d2:	4630      	mov	r0, r6
   388d4:	f10a 0630 	add.w	r6, sl, #48	; 0x30
   388d8:	460b      	mov	r3, r1
   388da:	4639      	mov	r1, r7
   388dc:	f006 fe76 	bl	3f5cc <__aeabi_dsub>
   388e0:	f805 6b01 	strb.w	r6, [r5], #1
   388e4:	9e01      	ldr	r6, [sp, #4]
   388e6:	4602      	mov	r2, r0
   388e8:	9f03      	ldr	r7, [sp, #12]
   388ea:	460b      	mov	r3, r1
   388ec:	1bae      	subs	r6, r5, r6
   388ee:	42b7      	cmp	r7, r6
   388f0:	d135      	bne.n	3895e <_dtoa_r+0x6ac>
   388f2:	f006 fe6d 	bl	3f5d0 <__adddf3>
   388f6:	4642      	mov	r2, r8
   388f8:	464b      	mov	r3, r9
   388fa:	4606      	mov	r6, r0
   388fc:	460f      	mov	r7, r1
   388fe:	f007 faad 	bl	3fe5c <__aeabi_dcmpgt>
   38902:	b9d0      	cbnz	r0, 3893a <_dtoa_r+0x688>
   38904:	4642      	mov	r2, r8
   38906:	464b      	mov	r3, r9
   38908:	4630      	mov	r0, r6
   3890a:	4639      	mov	r1, r7
   3890c:	f007 fa7e 	bl	3fe0c <__aeabi_dcmpeq>
   38910:	b110      	cbz	r0, 38918 <_dtoa_r+0x666>
   38912:	f01a 0f01 	tst.w	sl, #1
   38916:	d110      	bne.n	3893a <_dtoa_r+0x688>
   38918:	4620      	mov	r0, r4
   3891a:	ee18 1a10 	vmov	r1, s16
   3891e:	f001 fbd0 	bl	3a0c2 <_Bfree>
   38922:	2300      	movs	r3, #0
   38924:	9800      	ldr	r0, [sp, #0]
   38926:	702b      	strb	r3, [r5, #0]
   38928:	3001      	adds	r0, #1
   3892a:	9b0c      	ldr	r3, [sp, #48]	; 0x30
   3892c:	6018      	str	r0, [r3, #0]
   3892e:	9b21      	ldr	r3, [sp, #132]	; 0x84
   38930:	2b00      	cmp	r3, #0
   38932:	f43f acf7 	beq.w	38324 <_dtoa_r+0x72>
   38936:	601d      	str	r5, [r3, #0]
   38938:	e4f4      	b.n	38324 <_dtoa_r+0x72>
   3893a:	9f00      	ldr	r7, [sp, #0]
   3893c:	462b      	mov	r3, r5
   3893e:	461d      	mov	r5, r3
   38940:	f813 2d01 	ldrb.w	r2, [r3, #-1]!
   38944:	2a39      	cmp	r2, #57	; 0x39
   38946:	d106      	bne.n	38956 <_dtoa_r+0x6a4>
   38948:	9a01      	ldr	r2, [sp, #4]
   3894a:	429a      	cmp	r2, r3
   3894c:	d1f7      	bne.n	3893e <_dtoa_r+0x68c>
   3894e:	2230      	movs	r2, #48	; 0x30
   38950:	9901      	ldr	r1, [sp, #4]
   38952:	3701      	adds	r7, #1
   38954:	700a      	strb	r2, [r1, #0]
   38956:	781a      	ldrb	r2, [r3, #0]
   38958:	3201      	adds	r2, #1
   3895a:	701a      	strb	r2, [r3, #0]
   3895c:	e78f      	b.n	3887e <_dtoa_r+0x5cc>
   3895e:	2200      	movs	r2, #0
   38960:	4ba4      	ldr	r3, [pc, #656]	; (38bf4 <_dtoa_r+0x942>)
   38962:	f006 ffeb 	bl	3f93c <__aeabi_dmul>
   38966:	2200      	movs	r2, #0
   38968:	2300      	movs	r3, #0
   3896a:	4606      	mov	r6, r0
   3896c:	460f      	mov	r7, r1
   3896e:	f007 fa4d 	bl	3fe0c <__aeabi_dcmpeq>
   38972:	2800      	cmp	r0, #0
   38974:	d09d      	beq.n	388b2 <_dtoa_r+0x600>
   38976:	e7cf      	b.n	38918 <_dtoa_r+0x666>
   38978:	9a08      	ldr	r2, [sp, #32]
   3897a:	2a00      	cmp	r2, #0
   3897c:	f000 80d5 	beq.w	38b2a <_dtoa_r+0x878>
   38980:	9a06      	ldr	r2, [sp, #24]
   38982:	2a01      	cmp	r2, #1
   38984:	f300 80b9 	bgt.w	38afa <_dtoa_r+0x848>
   38988:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   3898a:	2a00      	cmp	r2, #0
   3898c:	f000 80b1 	beq.w	38af2 <_dtoa_r+0x840>
   38990:	f203 4333 	addw	r3, r3, #1075	; 0x433
   38994:	9e07      	ldr	r6, [sp, #28]
   38996:	9d04      	ldr	r5, [sp, #16]
   38998:	9a04      	ldr	r2, [sp, #16]
   3899a:	2101      	movs	r1, #1
   3899c:	4620      	mov	r0, r4
   3899e:	441a      	add	r2, r3
   389a0:	9204      	str	r2, [sp, #16]
   389a2:	9a05      	ldr	r2, [sp, #20]
   389a4:	441a      	add	r2, r3
   389a6:	9205      	str	r2, [sp, #20]
   389a8:	f001 fc75 	bl	3a296 <__i2b>
   389ac:	4607      	mov	r7, r0
   389ae:	2d00      	cmp	r5, #0
   389b0:	dd0c      	ble.n	389cc <_dtoa_r+0x71a>
   389b2:	9b05      	ldr	r3, [sp, #20]
   389b4:	2b00      	cmp	r3, #0
   389b6:	dd09      	ble.n	389cc <_dtoa_r+0x71a>
   389b8:	42ab      	cmp	r3, r5
   389ba:	9a04      	ldr	r2, [sp, #16]
   389bc:	bfa8      	it	ge
   389be:	462b      	movge	r3, r5
   389c0:	1ad2      	subs	r2, r2, r3
   389c2:	1aed      	subs	r5, r5, r3
   389c4:	9204      	str	r2, [sp, #16]
   389c6:	9a05      	ldr	r2, [sp, #20]
   389c8:	1ad3      	subs	r3, r2, r3
   389ca:	9305      	str	r3, [sp, #20]
   389cc:	9b07      	ldr	r3, [sp, #28]
   389ce:	b31b      	cbz	r3, 38a18 <_dtoa_r+0x766>
   389d0:	9b08      	ldr	r3, [sp, #32]
   389d2:	2b00      	cmp	r3, #0
   389d4:	f000 80ad 	beq.w	38b32 <_dtoa_r+0x880>
   389d8:	2e00      	cmp	r6, #0
   389da:	dd13      	ble.n	38a04 <_dtoa_r+0x752>
   389dc:	4639      	mov	r1, r7
   389de:	4632      	mov	r2, r6
   389e0:	4620      	mov	r0, r4
   389e2:	f001 fd15 	bl	3a410 <__pow5mult>
   389e6:	ee18 2a10 	vmov	r2, s16
   389ea:	4601      	mov	r1, r0
   389ec:	4607      	mov	r7, r0
   389ee:	4620      	mov	r0, r4
   389f0:	f001 fc66 	bl	3a2c0 <__multiply>
   389f4:	4680      	mov	r8, r0
   389f6:	ee18 1a10 	vmov	r1, s16
   389fa:	4620      	mov	r0, r4
   389fc:	ee08 8a10 	vmov	s16, r8
   38a00:	f001 fb5f 	bl	3a0c2 <_Bfree>
   38a04:	9b07      	ldr	r3, [sp, #28]
   38a06:	1b9a      	subs	r2, r3, r6
   38a08:	d006      	beq.n	38a18 <_dtoa_r+0x766>
   38a0a:	ee18 1a10 	vmov	r1, s16
   38a0e:	4620      	mov	r0, r4
   38a10:	f001 fcfe 	bl	3a410 <__pow5mult>
   38a14:	ee08 0a10 	vmov	s16, r0
   38a18:	2101      	movs	r1, #1
   38a1a:	4620      	mov	r0, r4
   38a1c:	f001 fc3b 	bl	3a296 <__i2b>
   38a20:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   38a22:	4606      	mov	r6, r0
   38a24:	2b00      	cmp	r3, #0
   38a26:	f340 8086 	ble.w	38b36 <_dtoa_r+0x884>
   38a2a:	461a      	mov	r2, r3
   38a2c:	4601      	mov	r1, r0
   38a2e:	4620      	mov	r0, r4
   38a30:	f001 fcee 	bl	3a410 <__pow5mult>
   38a34:	9b06      	ldr	r3, [sp, #24]
   38a36:	4606      	mov	r6, r0
   38a38:	2b01      	cmp	r3, #1
   38a3a:	dd7f      	ble.n	38b3c <_dtoa_r+0x88a>
   38a3c:	f04f 0800 	mov.w	r8, #0
   38a40:	6933      	ldr	r3, [r6, #16]
   38a42:	eb06 0383 	add.w	r3, r6, r3, lsl #2
   38a46:	6918      	ldr	r0, [r3, #16]
   38a48:	f001 fbd6 	bl	3a1f8 <__hi0bits>
   38a4c:	f1c0 0020 	rsb	r0, r0, #32
   38a50:	9b05      	ldr	r3, [sp, #20]
   38a52:	4418      	add	r0, r3
   38a54:	f010 001f 	ands.w	r0, r0, #31
   38a58:	f000 8091 	beq.w	38b7e <_dtoa_r+0x8cc>
   38a5c:	f1c0 0320 	rsb	r3, r0, #32
   38a60:	2b04      	cmp	r3, #4
   38a62:	f340 8089 	ble.w	38b78 <_dtoa_r+0x8c6>
   38a66:	f1c0 001c 	rsb	r0, r0, #28
   38a6a:	9b04      	ldr	r3, [sp, #16]
   38a6c:	4405      	add	r5, r0
   38a6e:	4403      	add	r3, r0
   38a70:	9304      	str	r3, [sp, #16]
   38a72:	9b05      	ldr	r3, [sp, #20]
   38a74:	4403      	add	r3, r0
   38a76:	9305      	str	r3, [sp, #20]
   38a78:	9b04      	ldr	r3, [sp, #16]
   38a7a:	2b00      	cmp	r3, #0
   38a7c:	dd07      	ble.n	38a8e <_dtoa_r+0x7dc>
   38a7e:	ee18 1a10 	vmov	r1, s16
   38a82:	461a      	mov	r2, r3
   38a84:	4620      	mov	r0, r4
   38a86:	f001 fd03 	bl	3a490 <__lshift>
   38a8a:	ee08 0a10 	vmov	s16, r0
   38a8e:	9b05      	ldr	r3, [sp, #20]
   38a90:	2b00      	cmp	r3, #0
   38a92:	dd05      	ble.n	38aa0 <_dtoa_r+0x7ee>
   38a94:	4631      	mov	r1, r6
   38a96:	461a      	mov	r2, r3
   38a98:	4620      	mov	r0, r4
   38a9a:	f001 fcf9 	bl	3a490 <__lshift>
   38a9e:	4606      	mov	r6, r0
   38aa0:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   38aa2:	2b00      	cmp	r3, #0
   38aa4:	d06d      	beq.n	38b82 <_dtoa_r+0x8d0>
   38aa6:	4631      	mov	r1, r6
   38aa8:	ee18 0a10 	vmov	r0, s16
   38aac:	f001 fd60 	bl	3a570 <__mcmp>
   38ab0:	2800      	cmp	r0, #0
   38ab2:	da66      	bge.n	38b82 <_dtoa_r+0x8d0>
   38ab4:	9b00      	ldr	r3, [sp, #0]
   38ab6:	ee18 1a10 	vmov	r1, s16
   38aba:	220a      	movs	r2, #10
   38abc:	4620      	mov	r0, r4
   38abe:	3b01      	subs	r3, #1
   38ac0:	9300      	str	r3, [sp, #0]
   38ac2:	2300      	movs	r3, #0
   38ac4:	f001 fb06 	bl	3a0d4 <__multadd>
   38ac8:	9b08      	ldr	r3, [sp, #32]
   38aca:	ee08 0a10 	vmov	s16, r0
   38ace:	2b00      	cmp	r3, #0
   38ad0:	f000 81ae 	beq.w	38e30 <_dtoa_r+0xb7e>
   38ad4:	2300      	movs	r3, #0
   38ad6:	4639      	mov	r1, r7
   38ad8:	220a      	movs	r2, #10
   38ada:	4620      	mov	r0, r4
   38adc:	f001 fafa 	bl	3a0d4 <__multadd>
   38ae0:	9b02      	ldr	r3, [sp, #8]
   38ae2:	4607      	mov	r7, r0
   38ae4:	2b00      	cmp	r3, #0
   38ae6:	f300 808d 	bgt.w	38c04 <_dtoa_r+0x952>
   38aea:	9b06      	ldr	r3, [sp, #24]
   38aec:	2b02      	cmp	r3, #2
   38aee:	dc50      	bgt.n	38b92 <_dtoa_r+0x8e0>
   38af0:	e088      	b.n	38c04 <_dtoa_r+0x952>
   38af2:	9b10      	ldr	r3, [sp, #64]	; 0x40
   38af4:	f1c3 0336 	rsb	r3, r3, #54	; 0x36
   38af8:	e74c      	b.n	38994 <_dtoa_r+0x6e2>
   38afa:	9b03      	ldr	r3, [sp, #12]
   38afc:	1e5e      	subs	r6, r3, #1
   38afe:	9b07      	ldr	r3, [sp, #28]
   38b00:	42b3      	cmp	r3, r6
   38b02:	bfb7      	itett	lt
   38b04:	9b07      	ldrlt	r3, [sp, #28]
   38b06:	1b9e      	subge	r6, r3, r6
   38b08:	9607      	strlt	r6, [sp, #28]
   38b0a:	1af2      	sublt	r2, r6, r3
   38b0c:	bfbf      	itttt	lt
   38b0e:	9b0a      	ldrlt	r3, [sp, #40]	; 0x28
   38b10:	2600      	movlt	r6, #0
   38b12:	189b      	addlt	r3, r3, r2
   38b14:	930a      	strlt	r3, [sp, #40]	; 0x28
   38b16:	9b03      	ldr	r3, [sp, #12]
   38b18:	2b00      	cmp	r3, #0
   38b1a:	bfb7      	itett	lt
   38b1c:	e9dd 2303 	ldrdlt	r2, r3, [sp, #12]
   38b20:	e9dd 3503 	ldrdge	r3, r5, [sp, #12]
   38b24:	1a9d      	sublt	r5, r3, r2
   38b26:	2300      	movlt	r3, #0
   38b28:	e736      	b.n	38998 <_dtoa_r+0x6e6>
   38b2a:	9e07      	ldr	r6, [sp, #28]
   38b2c:	9d04      	ldr	r5, [sp, #16]
   38b2e:	9f08      	ldr	r7, [sp, #32]
   38b30:	e73d      	b.n	389ae <_dtoa_r+0x6fc>
   38b32:	9a07      	ldr	r2, [sp, #28]
   38b34:	e769      	b.n	38a0a <_dtoa_r+0x758>
   38b36:	9b06      	ldr	r3, [sp, #24]
   38b38:	2b01      	cmp	r3, #1
   38b3a:	dc18      	bgt.n	38b6e <_dtoa_r+0x8bc>
   38b3c:	f1ba 0f00 	cmp.w	sl, #0
   38b40:	d115      	bne.n	38b6e <_dtoa_r+0x8bc>
   38b42:	f3cb 0313 	ubfx	r3, fp, #0, #20
   38b46:	b993      	cbnz	r3, 38b6e <_dtoa_r+0x8bc>
   38b48:	f02b 4300 	bic.w	r3, fp, #2147483648	; 0x80000000
   38b4c:	0d1b      	lsrs	r3, r3, #20
   38b4e:	051b      	lsls	r3, r3, #20
   38b50:	b183      	cbz	r3, 38b74 <_dtoa_r+0x8c2>
   38b52:	9b04      	ldr	r3, [sp, #16]
   38b54:	f04f 0801 	mov.w	r8, #1
   38b58:	3301      	adds	r3, #1
   38b5a:	9304      	str	r3, [sp, #16]
   38b5c:	9b05      	ldr	r3, [sp, #20]
   38b5e:	3301      	adds	r3, #1
   38b60:	9305      	str	r3, [sp, #20]
   38b62:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   38b64:	2b00      	cmp	r3, #0
   38b66:	f47f af6b 	bne.w	38a40 <_dtoa_r+0x78e>
   38b6a:	2001      	movs	r0, #1
   38b6c:	e770      	b.n	38a50 <_dtoa_r+0x79e>
   38b6e:	f04f 0800 	mov.w	r8, #0
   38b72:	e7f6      	b.n	38b62 <_dtoa_r+0x8b0>
   38b74:	4698      	mov	r8, r3
   38b76:	e7f4      	b.n	38b62 <_dtoa_r+0x8b0>
   38b78:	f43f af7e 	beq.w	38a78 <_dtoa_r+0x7c6>
   38b7c:	4618      	mov	r0, r3
   38b7e:	301c      	adds	r0, #28
   38b80:	e773      	b.n	38a6a <_dtoa_r+0x7b8>
   38b82:	9b03      	ldr	r3, [sp, #12]
   38b84:	2b00      	cmp	r3, #0
   38b86:	dc37      	bgt.n	38bf8 <_dtoa_r+0x946>
   38b88:	9b06      	ldr	r3, [sp, #24]
   38b8a:	2b02      	cmp	r3, #2
   38b8c:	dd34      	ble.n	38bf8 <_dtoa_r+0x946>
   38b8e:	9b03      	ldr	r3, [sp, #12]
   38b90:	9302      	str	r3, [sp, #8]
   38b92:	9b02      	ldr	r3, [sp, #8]
   38b94:	b96b      	cbnz	r3, 38bb2 <_dtoa_r+0x900>
   38b96:	4631      	mov	r1, r6
   38b98:	2205      	movs	r2, #5
   38b9a:	4620      	mov	r0, r4
   38b9c:	f001 fa9a 	bl	3a0d4 <__multadd>
   38ba0:	4601      	mov	r1, r0
   38ba2:	4606      	mov	r6, r0
   38ba4:	ee18 0a10 	vmov	r0, s16
   38ba8:	f001 fce2 	bl	3a570 <__mcmp>
   38bac:	2800      	cmp	r0, #0
   38bae:	f73f adbc 	bgt.w	3872a <_dtoa_r+0x478>
   38bb2:	9b09      	ldr	r3, [sp, #36]	; 0x24
   38bb4:	9d01      	ldr	r5, [sp, #4]
   38bb6:	43db      	mvns	r3, r3
   38bb8:	9300      	str	r3, [sp, #0]
   38bba:	f04f 0800 	mov.w	r8, #0
   38bbe:	4631      	mov	r1, r6
   38bc0:	4620      	mov	r0, r4
   38bc2:	f001 fa7e 	bl	3a0c2 <_Bfree>
   38bc6:	2f00      	cmp	r7, #0
   38bc8:	f43f aea6 	beq.w	38918 <_dtoa_r+0x666>
   38bcc:	f1b8 0f00 	cmp.w	r8, #0
   38bd0:	d005      	beq.n	38bde <_dtoa_r+0x92c>
   38bd2:	45b8      	cmp	r8, r7
   38bd4:	d003      	beq.n	38bde <_dtoa_r+0x92c>
   38bd6:	4641      	mov	r1, r8
   38bd8:	4620      	mov	r0, r4
   38bda:	f001 fa72 	bl	3a0c2 <_Bfree>
   38bde:	4639      	mov	r1, r7
   38be0:	4620      	mov	r0, r4
   38be2:	f001 fa6e 	bl	3a0c2 <_Bfree>
   38be6:	e697      	b.n	38918 <_dtoa_r+0x666>
   38be8:	2600      	movs	r6, #0
   38bea:	4637      	mov	r7, r6
   38bec:	e7e1      	b.n	38bb2 <_dtoa_r+0x900>
   38bee:	9700      	str	r7, [sp, #0]
   38bf0:	4637      	mov	r7, r6
   38bf2:	e59a      	b.n	3872a <_dtoa_r+0x478>
   38bf4:	40240000 	.word	0x40240000
   38bf8:	9b08      	ldr	r3, [sp, #32]
   38bfa:	2b00      	cmp	r3, #0
   38bfc:	f000 80c9 	beq.w	38d92 <_dtoa_r+0xae0>
   38c00:	9b03      	ldr	r3, [sp, #12]
   38c02:	9302      	str	r3, [sp, #8]
   38c04:	2d00      	cmp	r5, #0
   38c06:	dd05      	ble.n	38c14 <_dtoa_r+0x962>
   38c08:	4639      	mov	r1, r7
   38c0a:	462a      	mov	r2, r5
   38c0c:	4620      	mov	r0, r4
   38c0e:	f001 fc3f 	bl	3a490 <__lshift>
   38c12:	4607      	mov	r7, r0
   38c14:	f1b8 0f00 	cmp.w	r8, #0
   38c18:	d05a      	beq.n	38cd0 <_dtoa_r+0xa1e>
   38c1a:	6879      	ldr	r1, [r7, #4]
   38c1c:	4620      	mov	r0, r4
   38c1e:	f001 fa2b 	bl	3a078 <_Balloc>
   38c22:	4605      	mov	r5, r0
   38c24:	b920      	cbnz	r0, 38c30 <_dtoa_r+0x97e>
   38c26:	4b86      	ldr	r3, [pc, #536]	; (38e40 <_dtoa_r+0xb8e>)
   38c28:	4602      	mov	r2, r0
   38c2a:	f240 21ea 	movw	r1, #746	; 0x2ea
   38c2e:	e47c      	b.n	3852a <_dtoa_r+0x278>
   38c30:	693a      	ldr	r2, [r7, #16]
   38c32:	f107 010c 	add.w	r1, r7, #12
   38c36:	300c      	adds	r0, #12
   38c38:	3202      	adds	r2, #2
   38c3a:	0092      	lsls	r2, r2, #2
   38c3c:	f001 f90c 	bl	39e58 <memcpy>
   38c40:	2201      	movs	r2, #1
   38c42:	4629      	mov	r1, r5
   38c44:	4620      	mov	r0, r4
   38c46:	f001 fc23 	bl	3a490 <__lshift>
   38c4a:	9b01      	ldr	r3, [sp, #4]
   38c4c:	46b8      	mov	r8, r7
   38c4e:	4607      	mov	r7, r0
   38c50:	f103 0901 	add.w	r9, r3, #1
   38c54:	e9dd 2301 	ldrd	r2, r3, [sp, #4]
   38c58:	4413      	add	r3, r2
   38c5a:	9305      	str	r3, [sp, #20]
   38c5c:	f00a 0301 	and.w	r3, sl, #1
   38c60:	9304      	str	r3, [sp, #16]
   38c62:	4631      	mov	r1, r6
   38c64:	ee18 0a10 	vmov	r0, s16
   38c68:	f7ff fa92 	bl	38190 <quorem>
   38c6c:	4641      	mov	r1, r8
   38c6e:	9002      	str	r0, [sp, #8]
   38c70:	f100 0a30 	add.w	sl, r0, #48	; 0x30
   38c74:	ee18 0a10 	vmov	r0, s16
   38c78:	f109 3bff 	add.w	fp, r9, #4294967295	; 0xffffffff
   38c7c:	f001 fc78 	bl	3a570 <__mcmp>
   38c80:	463a      	mov	r2, r7
   38c82:	9003      	str	r0, [sp, #12]
   38c84:	4631      	mov	r1, r6
   38c86:	4620      	mov	r0, r4
   38c88:	f001 fc8e 	bl	3a5a8 <__mdiff>
   38c8c:	68c2      	ldr	r2, [r0, #12]
   38c8e:	4605      	mov	r5, r0
   38c90:	bb02      	cbnz	r2, 38cd4 <_dtoa_r+0xa22>
   38c92:	4601      	mov	r1, r0
   38c94:	ee18 0a10 	vmov	r0, s16
   38c98:	f001 fc6a 	bl	3a570 <__mcmp>
   38c9c:	4602      	mov	r2, r0
   38c9e:	4629      	mov	r1, r5
   38ca0:	4620      	mov	r0, r4
   38ca2:	9207      	str	r2, [sp, #28]
   38ca4:	f001 fa0d 	bl	3a0c2 <_Bfree>
   38ca8:	464d      	mov	r5, r9
   38caa:	e9dd 3206 	ldrd	r3, r2, [sp, #24]
   38cae:	ea43 0102 	orr.w	r1, r3, r2
   38cb2:	9b04      	ldr	r3, [sp, #16]
   38cb4:	430b      	orrs	r3, r1
   38cb6:	d10f      	bne.n	38cd8 <_dtoa_r+0xa26>
   38cb8:	f1ba 0f39 	cmp.w	sl, #57	; 0x39
   38cbc:	d02a      	beq.n	38d14 <_dtoa_r+0xa62>
   38cbe:	9b03      	ldr	r3, [sp, #12]
   38cc0:	2b00      	cmp	r3, #0
   38cc2:	dd02      	ble.n	38cca <_dtoa_r+0xa18>
   38cc4:	9b02      	ldr	r3, [sp, #8]
   38cc6:	f103 0a31 	add.w	sl, r3, #49	; 0x31
   38cca:	f88b a000 	strb.w	sl, [fp]
   38cce:	e776      	b.n	38bbe <_dtoa_r+0x90c>
   38cd0:	4638      	mov	r0, r7
   38cd2:	e7ba      	b.n	38c4a <_dtoa_r+0x998>
   38cd4:	2201      	movs	r2, #1
   38cd6:	e7e2      	b.n	38c9e <_dtoa_r+0x9ec>
   38cd8:	9b03      	ldr	r3, [sp, #12]
   38cda:	2b00      	cmp	r3, #0
   38cdc:	db04      	blt.n	38ce8 <_dtoa_r+0xa36>
   38cde:	9906      	ldr	r1, [sp, #24]
   38ce0:	430b      	orrs	r3, r1
   38ce2:	9904      	ldr	r1, [sp, #16]
   38ce4:	430b      	orrs	r3, r1
   38ce6:	d122      	bne.n	38d2e <_dtoa_r+0xa7c>
   38ce8:	2a00      	cmp	r2, #0
   38cea:	ddee      	ble.n	38cca <_dtoa_r+0xa18>
   38cec:	ee18 1a10 	vmov	r1, s16
   38cf0:	2201      	movs	r2, #1
   38cf2:	4620      	mov	r0, r4
   38cf4:	f001 fbcc 	bl	3a490 <__lshift>
   38cf8:	4631      	mov	r1, r6
   38cfa:	ee08 0a10 	vmov	s16, r0
   38cfe:	f001 fc37 	bl	3a570 <__mcmp>
   38d02:	2800      	cmp	r0, #0
   38d04:	dc03      	bgt.n	38d0e <_dtoa_r+0xa5c>
   38d06:	d1e0      	bne.n	38cca <_dtoa_r+0xa18>
   38d08:	f01a 0f01 	tst.w	sl, #1
   38d0c:	d0dd      	beq.n	38cca <_dtoa_r+0xa18>
   38d0e:	f1ba 0f39 	cmp.w	sl, #57	; 0x39
   38d12:	d1d7      	bne.n	38cc4 <_dtoa_r+0xa12>
   38d14:	2339      	movs	r3, #57	; 0x39
   38d16:	f88b 3000 	strb.w	r3, [fp]
   38d1a:	462b      	mov	r3, r5
   38d1c:	461d      	mov	r5, r3
   38d1e:	3b01      	subs	r3, #1
   38d20:	f815 2c01 	ldrb.w	r2, [r5, #-1]
   38d24:	2a39      	cmp	r2, #57	; 0x39
   38d26:	d070      	beq.n	38e0a <_dtoa_r+0xb58>
   38d28:	3201      	adds	r2, #1
   38d2a:	701a      	strb	r2, [r3, #0]
   38d2c:	e747      	b.n	38bbe <_dtoa_r+0x90c>
   38d2e:	2a00      	cmp	r2, #0
   38d30:	dd07      	ble.n	38d42 <_dtoa_r+0xa90>
   38d32:	f1ba 0f39 	cmp.w	sl, #57	; 0x39
   38d36:	d0ed      	beq.n	38d14 <_dtoa_r+0xa62>
   38d38:	f10a 0301 	add.w	r3, sl, #1
   38d3c:	f88b 3000 	strb.w	r3, [fp]
   38d40:	e73d      	b.n	38bbe <_dtoa_r+0x90c>
   38d42:	9b05      	ldr	r3, [sp, #20]
   38d44:	f809 ac01 	strb.w	sl, [r9, #-1]
   38d48:	4599      	cmp	r9, r3
   38d4a:	d046      	beq.n	38dda <_dtoa_r+0xb28>
   38d4c:	ee18 1a10 	vmov	r1, s16
   38d50:	2300      	movs	r3, #0
   38d52:	220a      	movs	r2, #10
   38d54:	4620      	mov	r0, r4
   38d56:	f001 f9bd 	bl	3a0d4 <__multadd>
   38d5a:	45b8      	cmp	r8, r7
   38d5c:	ee08 0a10 	vmov	s16, r0
   38d60:	f04f 0300 	mov.w	r3, #0
   38d64:	f04f 020a 	mov.w	r2, #10
   38d68:	4641      	mov	r1, r8
   38d6a:	4620      	mov	r0, r4
   38d6c:	d106      	bne.n	38d7c <_dtoa_r+0xaca>
   38d6e:	f001 f9b1 	bl	3a0d4 <__multadd>
   38d72:	4680      	mov	r8, r0
   38d74:	4607      	mov	r7, r0
   38d76:	f109 0901 	add.w	r9, r9, #1
   38d7a:	e772      	b.n	38c62 <_dtoa_r+0x9b0>
   38d7c:	f001 f9aa 	bl	3a0d4 <__multadd>
   38d80:	4639      	mov	r1, r7
   38d82:	4680      	mov	r8, r0
   38d84:	2300      	movs	r3, #0
   38d86:	220a      	movs	r2, #10
   38d88:	4620      	mov	r0, r4
   38d8a:	f001 f9a3 	bl	3a0d4 <__multadd>
   38d8e:	4607      	mov	r7, r0
   38d90:	e7f1      	b.n	38d76 <_dtoa_r+0xac4>
   38d92:	9b03      	ldr	r3, [sp, #12]
   38d94:	9302      	str	r3, [sp, #8]
   38d96:	9d01      	ldr	r5, [sp, #4]
   38d98:	4631      	mov	r1, r6
   38d9a:	ee18 0a10 	vmov	r0, s16
   38d9e:	f7ff f9f7 	bl	38190 <quorem>
   38da2:	9b01      	ldr	r3, [sp, #4]
   38da4:	f100 0a30 	add.w	sl, r0, #48	; 0x30
   38da8:	f805 ab01 	strb.w	sl, [r5], #1
   38dac:	1aea      	subs	r2, r5, r3
   38dae:	9b02      	ldr	r3, [sp, #8]
   38db0:	4293      	cmp	r3, r2
   38db2:	dd09      	ble.n	38dc8 <_dtoa_r+0xb16>
   38db4:	ee18 1a10 	vmov	r1, s16
   38db8:	2300      	movs	r3, #0
   38dba:	220a      	movs	r2, #10
   38dbc:	4620      	mov	r0, r4
   38dbe:	f001 f989 	bl	3a0d4 <__multadd>
   38dc2:	ee08 0a10 	vmov	s16, r0
   38dc6:	e7e7      	b.n	38d98 <_dtoa_r+0xae6>
   38dc8:	9b02      	ldr	r3, [sp, #8]
   38dca:	f04f 0800 	mov.w	r8, #0
   38dce:	2b00      	cmp	r3, #0
   38dd0:	bfcc      	ite	gt
   38dd2:	461d      	movgt	r5, r3
   38dd4:	2501      	movle	r5, #1
   38dd6:	9b01      	ldr	r3, [sp, #4]
   38dd8:	441d      	add	r5, r3
   38dda:	ee18 1a10 	vmov	r1, s16
   38dde:	2201      	movs	r2, #1
   38de0:	4620      	mov	r0, r4
   38de2:	f001 fb55 	bl	3a490 <__lshift>
   38de6:	4631      	mov	r1, r6
   38de8:	ee08 0a10 	vmov	s16, r0
   38dec:	f001 fbc0 	bl	3a570 <__mcmp>
   38df0:	2800      	cmp	r0, #0
   38df2:	dc92      	bgt.n	38d1a <_dtoa_r+0xa68>
   38df4:	d102      	bne.n	38dfc <_dtoa_r+0xb4a>
   38df6:	f01a 0f01 	tst.w	sl, #1
   38dfa:	d18e      	bne.n	38d1a <_dtoa_r+0xa68>
   38dfc:	462b      	mov	r3, r5
   38dfe:	461d      	mov	r5, r3
   38e00:	f813 2d01 	ldrb.w	r2, [r3, #-1]!
   38e04:	2a30      	cmp	r2, #48	; 0x30
   38e06:	d0fa      	beq.n	38dfe <_dtoa_r+0xb4c>
   38e08:	e6d9      	b.n	38bbe <_dtoa_r+0x90c>
   38e0a:	9a01      	ldr	r2, [sp, #4]
   38e0c:	429a      	cmp	r2, r3
   38e0e:	d185      	bne.n	38d1c <_dtoa_r+0xa6a>
   38e10:	9b00      	ldr	r3, [sp, #0]
   38e12:	3301      	adds	r3, #1
   38e14:	9300      	str	r3, [sp, #0]
   38e16:	2331      	movs	r3, #49	; 0x31
   38e18:	7013      	strb	r3, [r2, #0]
   38e1a:	e6d0      	b.n	38bbe <_dtoa_r+0x90c>
   38e1c:	4b09      	ldr	r3, [pc, #36]	; (38e44 <_dtoa_r+0xb92>)
   38e1e:	f7ff ba9f 	b.w	38360 <_dtoa_r+0xae>
   38e22:	9b21      	ldr	r3, [sp, #132]	; 0x84
   38e24:	2b00      	cmp	r3, #0
   38e26:	f47f aa83 	bne.w	38330 <_dtoa_r+0x7e>
   38e2a:	4b07      	ldr	r3, [pc, #28]	; (38e48 <_dtoa_r+0xb96>)
   38e2c:	f7ff ba98 	b.w	38360 <_dtoa_r+0xae>
   38e30:	9b02      	ldr	r3, [sp, #8]
   38e32:	2b00      	cmp	r3, #0
   38e34:	dcaf      	bgt.n	38d96 <_dtoa_r+0xae4>
   38e36:	9b06      	ldr	r3, [sp, #24]
   38e38:	2b02      	cmp	r3, #2
   38e3a:	f73f aeaa 	bgt.w	38b92 <_dtoa_r+0x8e0>
   38e3e:	e7aa      	b.n	38d96 <_dtoa_r+0xae4>
   38e40:	00040678 	.word	0x00040678
   38e44:	00040501 	.word	0x00040501
   38e48:	0004066f 	.word	0x0004066f

00038e4c <__errno>:
   38e4c:	4b01      	ldr	r3, [pc, #4]	; (38e54 <__errno+0x8>)
   38e4e:	6818      	ldr	r0, [r3, #0]
   38e50:	4770      	bx	lr
   38e52:	bf00      	nop
   38e54:	20006000 	.word	0x20006000

00038e58 <__sflush_r>:
   38e58:	898b      	ldrh	r3, [r1, #12]
   38e5a:	f9b1 200c 	ldrsh.w	r2, [r1, #12]
   38e5e:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
   38e62:	4605      	mov	r5, r0
   38e64:	0718      	lsls	r0, r3, #28
   38e66:	460c      	mov	r4, r1
   38e68:	d45f      	bmi.n	38f2a <__sflush_r+0xd2>
   38e6a:	684b      	ldr	r3, [r1, #4]
   38e6c:	f442 6200 	orr.w	r2, r2, #2048	; 0x800
   38e70:	2b00      	cmp	r3, #0
   38e72:	818a      	strh	r2, [r1, #12]
   38e74:	dc05      	bgt.n	38e82 <__sflush_r+0x2a>
   38e76:	6bcb      	ldr	r3, [r1, #60]	; 0x3c
   38e78:	2b00      	cmp	r3, #0
   38e7a:	dc02      	bgt.n	38e82 <__sflush_r+0x2a>
   38e7c:	2000      	movs	r0, #0
   38e7e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
   38e82:	6aa6      	ldr	r6, [r4, #40]	; 0x28
   38e84:	2e00      	cmp	r6, #0
   38e86:	d0f9      	beq.n	38e7c <__sflush_r+0x24>
   38e88:	2300      	movs	r3, #0
   38e8a:	f412 5280 	ands.w	r2, r2, #4096	; 0x1000
   38e8e:	682f      	ldr	r7, [r5, #0]
   38e90:	602b      	str	r3, [r5, #0]
   38e92:	d036      	beq.n	38f02 <__sflush_r+0xaa>
   38e94:	6d20      	ldr	r0, [r4, #80]	; 0x50
   38e96:	89a3      	ldrh	r3, [r4, #12]
   38e98:	075a      	lsls	r2, r3, #29
   38e9a:	d505      	bpl.n	38ea8 <__sflush_r+0x50>
   38e9c:	6863      	ldr	r3, [r4, #4]
   38e9e:	1ac0      	subs	r0, r0, r3
   38ea0:	6b23      	ldr	r3, [r4, #48]	; 0x30
   38ea2:	b10b      	cbz	r3, 38ea8 <__sflush_r+0x50>
   38ea4:	6be3      	ldr	r3, [r4, #60]	; 0x3c
   38ea6:	1ac0      	subs	r0, r0, r3
   38ea8:	2300      	movs	r3, #0
   38eaa:	4602      	mov	r2, r0
   38eac:	6aa6      	ldr	r6, [r4, #40]	; 0x28
   38eae:	4628      	mov	r0, r5
   38eb0:	69e1      	ldr	r1, [r4, #28]
   38eb2:	47b0      	blx	r6
   38eb4:	1c43      	adds	r3, r0, #1
   38eb6:	89a3      	ldrh	r3, [r4, #12]
   38eb8:	d106      	bne.n	38ec8 <__sflush_r+0x70>
   38eba:	6829      	ldr	r1, [r5, #0]
   38ebc:	291d      	cmp	r1, #29
   38ebe:	d830      	bhi.n	38f22 <__sflush_r+0xca>
   38ec0:	4a2b      	ldr	r2, [pc, #172]	; (38f70 <__sflush_r+0x118>)
   38ec2:	40ca      	lsrs	r2, r1
   38ec4:	07d6      	lsls	r6, r2, #31
   38ec6:	d52c      	bpl.n	38f22 <__sflush_r+0xca>
   38ec8:	f423 6300 	bic.w	r3, r3, #2048	; 0x800
   38ecc:	2200      	movs	r2, #0
   38ece:	b21b      	sxth	r3, r3
   38ed0:	6062      	str	r2, [r4, #4]
   38ed2:	6922      	ldr	r2, [r4, #16]
   38ed4:	04d9      	lsls	r1, r3, #19
   38ed6:	81a3      	strh	r3, [r4, #12]
   38ed8:	6022      	str	r2, [r4, #0]
   38eda:	d504      	bpl.n	38ee6 <__sflush_r+0x8e>
   38edc:	1c42      	adds	r2, r0, #1
   38ede:	d101      	bne.n	38ee4 <__sflush_r+0x8c>
   38ee0:	682b      	ldr	r3, [r5, #0]
   38ee2:	b903      	cbnz	r3, 38ee6 <__sflush_r+0x8e>
   38ee4:	6520      	str	r0, [r4, #80]	; 0x50
   38ee6:	6b21      	ldr	r1, [r4, #48]	; 0x30
   38ee8:	602f      	str	r7, [r5, #0]
   38eea:	2900      	cmp	r1, #0
   38eec:	d0c6      	beq.n	38e7c <__sflush_r+0x24>
   38eee:	f104 0340 	add.w	r3, r4, #64	; 0x40
   38ef2:	4299      	cmp	r1, r3
   38ef4:	d002      	beq.n	38efc <__sflush_r+0xa4>
   38ef6:	4628      	mov	r0, r5
   38ef8:	f000 f9ce 	bl	39298 <_free_r>
   38efc:	2000      	movs	r0, #0
   38efe:	6320      	str	r0, [r4, #48]	; 0x30
   38f00:	e7bd      	b.n	38e7e <__sflush_r+0x26>
   38f02:	69e1      	ldr	r1, [r4, #28]
   38f04:	2301      	movs	r3, #1
   38f06:	4628      	mov	r0, r5
   38f08:	47b0      	blx	r6
   38f0a:	1c41      	adds	r1, r0, #1
   38f0c:	d1c3      	bne.n	38e96 <__sflush_r+0x3e>
   38f0e:	682b      	ldr	r3, [r5, #0]
   38f10:	2b00      	cmp	r3, #0
   38f12:	d0c0      	beq.n	38e96 <__sflush_r+0x3e>
   38f14:	2b1d      	cmp	r3, #29
   38f16:	d001      	beq.n	38f1c <__sflush_r+0xc4>
   38f18:	2b16      	cmp	r3, #22
   38f1a:	d101      	bne.n	38f20 <__sflush_r+0xc8>
   38f1c:	602f      	str	r7, [r5, #0]
   38f1e:	e7ad      	b.n	38e7c <__sflush_r+0x24>
   38f20:	89a3      	ldrh	r3, [r4, #12]
   38f22:	f043 0340 	orr.w	r3, r3, #64	; 0x40
   38f26:	81a3      	strh	r3, [r4, #12]
   38f28:	e7a9      	b.n	38e7e <__sflush_r+0x26>
   38f2a:	690f      	ldr	r7, [r1, #16]
   38f2c:	2f00      	cmp	r7, #0
   38f2e:	d0a5      	beq.n	38e7c <__sflush_r+0x24>
   38f30:	079b      	lsls	r3, r3, #30
   38f32:	680e      	ldr	r6, [r1, #0]
   38f34:	600f      	str	r7, [r1, #0]
   38f36:	bf0c      	ite	eq
   38f38:	694b      	ldreq	r3, [r1, #20]
   38f3a:	2300      	movne	r3, #0
   38f3c:	eba6 0807 	sub.w	r8, r6, r7
   38f40:	608b      	str	r3, [r1, #8]
   38f42:	f1b8 0f00 	cmp.w	r8, #0
   38f46:	dd99      	ble.n	38e7c <__sflush_r+0x24>
   38f48:	4643      	mov	r3, r8
   38f4a:	463a      	mov	r2, r7
   38f4c:	69e1      	ldr	r1, [r4, #28]
   38f4e:	4628      	mov	r0, r5
   38f50:	6a66      	ldr	r6, [r4, #36]	; 0x24
   38f52:	47b0      	blx	r6
   38f54:	2800      	cmp	r0, #0
   38f56:	dc06      	bgt.n	38f66 <__sflush_r+0x10e>
   38f58:	89a3      	ldrh	r3, [r4, #12]
   38f5a:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   38f5e:	f043 0340 	orr.w	r3, r3, #64	; 0x40
   38f62:	81a3      	strh	r3, [r4, #12]
   38f64:	e78b      	b.n	38e7e <__sflush_r+0x26>
   38f66:	4407      	add	r7, r0
   38f68:	eba8 0800 	sub.w	r8, r8, r0
   38f6c:	e7e9      	b.n	38f42 <__sflush_r+0xea>
   38f6e:	bf00      	nop
   38f70:	20400001 	.word	0x20400001

00038f74 <_fflush_r>:
   38f74:	b538      	push	{r3, r4, r5, lr}
   38f76:	460c      	mov	r4, r1
   38f78:	4605      	mov	r5, r0
   38f7a:	b118      	cbz	r0, 38f84 <_fflush_r+0x10>
   38f7c:	6b83      	ldr	r3, [r0, #56]	; 0x38
   38f7e:	b90b      	cbnz	r3, 38f84 <_fflush_r+0x10>
   38f80:	f000 f8a8 	bl	390d4 <__sinit>
   38f84:	f9b4 300c 	ldrsh.w	r3, [r4, #12]
   38f88:	b1bb      	cbz	r3, 38fba <_fflush_r+0x46>
   38f8a:	6e62      	ldr	r2, [r4, #100]	; 0x64
   38f8c:	07d0      	lsls	r0, r2, #31
   38f8e:	d404      	bmi.n	38f9a <_fflush_r+0x26>
   38f90:	0599      	lsls	r1, r3, #22
   38f92:	d402      	bmi.n	38f9a <_fflush_r+0x26>
   38f94:	6da0      	ldr	r0, [r4, #88]	; 0x58
   38f96:	f000 fc06 	bl	397a6 <__retarget_lock_acquire_recursive>
   38f9a:	4628      	mov	r0, r5
   38f9c:	4621      	mov	r1, r4
   38f9e:	f7ff ff5b 	bl	38e58 <__sflush_r>
   38fa2:	6e63      	ldr	r3, [r4, #100]	; 0x64
   38fa4:	4605      	mov	r5, r0
   38fa6:	07da      	lsls	r2, r3, #31
   38fa8:	d405      	bmi.n	38fb6 <_fflush_r+0x42>
   38faa:	89a3      	ldrh	r3, [r4, #12]
   38fac:	059b      	lsls	r3, r3, #22
   38fae:	d402      	bmi.n	38fb6 <_fflush_r+0x42>
   38fb0:	6da0      	ldr	r0, [r4, #88]	; 0x58
   38fb2:	f000 fbfe 	bl	397b2 <__retarget_lock_release_recursive>
   38fb6:	4628      	mov	r0, r5
   38fb8:	bd38      	pop	{r3, r4, r5, pc}
   38fba:	461d      	mov	r5, r3
   38fbc:	e7fb      	b.n	38fb6 <_fflush_r+0x42>

00038fbe <fflush>:
   38fbe:	4601      	mov	r1, r0
   38fc0:	b920      	cbnz	r0, 38fcc <fflush+0xe>
   38fc2:	4b04      	ldr	r3, [pc, #16]	; (38fd4 <fflush+0x16>)
   38fc4:	4904      	ldr	r1, [pc, #16]	; (38fd8 <fflush+0x1a>)
   38fc6:	6818      	ldr	r0, [r3, #0]
   38fc8:	f000 bb8d 	b.w	396e6 <_fwalk_reent>
   38fcc:	4b03      	ldr	r3, [pc, #12]	; (38fdc <fflush+0x1e>)
   38fce:	6818      	ldr	r0, [r3, #0]
   38fd0:	f7ff bfd0 	b.w	38f74 <_fflush_r>
   38fd4:	00040700 	.word	0x00040700
   38fd8:	00038f75 	.word	0x00038f75
   38fdc:	20006000 	.word	0x20006000

00038fe0 <std>:
   38fe0:	2300      	movs	r3, #0
   38fe2:	b510      	push	{r4, lr}
   38fe4:	4604      	mov	r4, r0
   38fe6:	6083      	str	r3, [r0, #8]
   38fe8:	8181      	strh	r1, [r0, #12]
   38fea:	4619      	mov	r1, r3
   38fec:	6643      	str	r3, [r0, #100]	; 0x64
   38fee:	81c2      	strh	r2, [r0, #14]
   38ff0:	2208      	movs	r2, #8
   38ff2:	6183      	str	r3, [r0, #24]
   38ff4:	e9c0 3300 	strd	r3, r3, [r0]
   38ff8:	e9c0 3304 	strd	r3, r3, [r0, #16]
   38ffc:	305c      	adds	r0, #92	; 0x5c
   38ffe:	f000 fff7 	bl	39ff0 <memset>
   39002:	4b07      	ldr	r3, [pc, #28]	; (39020 <std+0x40>)
   39004:	f104 0058 	add.w	r0, r4, #88	; 0x58
   39008:	61e4      	str	r4, [r4, #28]
   3900a:	6223      	str	r3, [r4, #32]
   3900c:	4b05      	ldr	r3, [pc, #20]	; (39024 <std+0x44>)
   3900e:	6263      	str	r3, [r4, #36]	; 0x24
   39010:	4b05      	ldr	r3, [pc, #20]	; (39028 <std+0x48>)
   39012:	62a3      	str	r3, [r4, #40]	; 0x28
   39014:	4b05      	ldr	r3, [pc, #20]	; (3902c <std+0x4c>)
   39016:	62e3      	str	r3, [r4, #44]	; 0x2c
   39018:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
   3901c:	f000 bbbf 	b.w	3979e <__retarget_lock_init_recursive>
   39020:	00033b2d 	.word	0x00033b2d
   39024:	00033b53 	.word	0x00033b53
   39028:	00033b8b 	.word	0x00033b8b
   3902c:	00033baf 	.word	0x00033baf

00039030 <_cleanup_r>:
   39030:	4901      	ldr	r1, [pc, #4]	; (39038 <_cleanup_r+0x8>)
   39032:	f000 bb58 	b.w	396e6 <_fwalk_reent>
   39036:	bf00      	nop
   39038:	0003e875 	.word	0x0003e875

0003903c <__fp_lock>:
   3903c:	b508      	push	{r3, lr}
   3903e:	6e43      	ldr	r3, [r0, #100]	; 0x64
   39040:	07da      	lsls	r2, r3, #31
   39042:	d405      	bmi.n	39050 <__fp_lock+0x14>
   39044:	8983      	ldrh	r3, [r0, #12]
   39046:	059b      	lsls	r3, r3, #22
   39048:	d402      	bmi.n	39050 <__fp_lock+0x14>
   3904a:	6d80      	ldr	r0, [r0, #88]	; 0x58
   3904c:	f000 fbab 	bl	397a6 <__retarget_lock_acquire_recursive>
   39050:	2000      	movs	r0, #0
   39052:	bd08      	pop	{r3, pc}

00039054 <__fp_unlock>:
   39054:	b508      	push	{r3, lr}
   39056:	6e43      	ldr	r3, [r0, #100]	; 0x64
   39058:	07da      	lsls	r2, r3, #31
   3905a:	d405      	bmi.n	39068 <__fp_unlock+0x14>
   3905c:	8983      	ldrh	r3, [r0, #12]
   3905e:	059b      	lsls	r3, r3, #22
   39060:	d402      	bmi.n	39068 <__fp_unlock+0x14>
   39062:	6d80      	ldr	r0, [r0, #88]	; 0x58
   39064:	f000 fba5 	bl	397b2 <__retarget_lock_release_recursive>
   39068:	2000      	movs	r0, #0
   3906a:	bd08      	pop	{r3, pc}

0003906c <__sfmoreglue>:
   3906c:	b570      	push	{r4, r5, r6, lr}
   3906e:	2268      	movs	r2, #104	; 0x68
   39070:	1e4d      	subs	r5, r1, #1
   39072:	460e      	mov	r6, r1
   39074:	4355      	muls	r5, r2
   39076:	f105 0174 	add.w	r1, r5, #116	; 0x74
   3907a:	f000 fc21 	bl	398c0 <_malloc_r>
   3907e:	4604      	mov	r4, r0
   39080:	b140      	cbz	r0, 39094 <__sfmoreglue+0x28>
   39082:	2100      	movs	r1, #0
   39084:	f105 0268 	add.w	r2, r5, #104	; 0x68
   39088:	e9c0 1600 	strd	r1, r6, [r0]
   3908c:	300c      	adds	r0, #12
   3908e:	60a0      	str	r0, [r4, #8]
   39090:	f000 ffae 	bl	39ff0 <memset>
   39094:	4620      	mov	r0, r4
   39096:	bd70      	pop	{r4, r5, r6, pc}

00039098 <_cleanup>:
   39098:	4b01      	ldr	r3, [pc, #4]	; (390a0 <_cleanup+0x8>)
   3909a:	6818      	ldr	r0, [r3, #0]
   3909c:	f7ff bfc8 	b.w	39030 <_cleanup_r>
   390a0:	00040700 	.word	0x00040700

000390a4 <__sfp_lock_acquire>:
   390a4:	4801      	ldr	r0, [pc, #4]	; (390ac <__sfp_lock_acquire+0x8>)
   390a6:	f000 bb7e 	b.w	397a6 <__retarget_lock_acquire_recursive>
   390aa:	bf00      	nop
   390ac:	200069d3 	.word	0x200069d3

000390b0 <__sfp_lock_release>:
   390b0:	4801      	ldr	r0, [pc, #4]	; (390b8 <__sfp_lock_release+0x8>)
   390b2:	f000 bb7e 	b.w	397b2 <__retarget_lock_release_recursive>
   390b6:	bf00      	nop
   390b8:	200069d3 	.word	0x200069d3

000390bc <__sinit_lock_acquire>:
   390bc:	4801      	ldr	r0, [pc, #4]	; (390c4 <__sinit_lock_acquire+0x8>)
   390be:	f000 bb72 	b.w	397a6 <__retarget_lock_acquire_recursive>
   390c2:	bf00      	nop
   390c4:	200069d4 	.word	0x200069d4

000390c8 <__sinit_lock_release>:
   390c8:	4801      	ldr	r0, [pc, #4]	; (390d0 <__sinit_lock_release+0x8>)
   390ca:	f000 bb72 	b.w	397b2 <__retarget_lock_release_recursive>
   390ce:	bf00      	nop
   390d0:	200069d4 	.word	0x200069d4

000390d4 <__sinit>:
   390d4:	b510      	push	{r4, lr}
   390d6:	4604      	mov	r4, r0
   390d8:	f7ff fff0 	bl	390bc <__sinit_lock_acquire>
   390dc:	6ba2      	ldr	r2, [r4, #56]	; 0x38
   390de:	b11a      	cbz	r2, 390e8 <__sinit+0x14>
   390e0:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
   390e4:	f7ff bff0 	b.w	390c8 <__sinit_lock_release>
   390e8:	4b0d      	ldr	r3, [pc, #52]	; (39120 <__sinit+0x4c>)
   390ea:	2104      	movs	r1, #4
   390ec:	6860      	ldr	r0, [r4, #4]
   390ee:	63e3      	str	r3, [r4, #60]	; 0x3c
   390f0:	2303      	movs	r3, #3
   390f2:	f8c4 22e0 	str.w	r2, [r4, #736]	; 0x2e0
   390f6:	f8c4 32e4 	str.w	r3, [r4, #740]	; 0x2e4
   390fa:	f504 733b 	add.w	r3, r4, #748	; 0x2ec
   390fe:	f8c4 32e8 	str.w	r3, [r4, #744]	; 0x2e8
   39102:	f7ff ff6d 	bl	38fe0 <std>
   39106:	2201      	movs	r2, #1
   39108:	2109      	movs	r1, #9
   3910a:	68a0      	ldr	r0, [r4, #8]
   3910c:	f7ff ff68 	bl	38fe0 <std>
   39110:	2202      	movs	r2, #2
   39112:	2112      	movs	r1, #18
   39114:	68e0      	ldr	r0, [r4, #12]
   39116:	f7ff ff63 	bl	38fe0 <std>
   3911a:	2301      	movs	r3, #1
   3911c:	63a3      	str	r3, [r4, #56]	; 0x38
   3911e:	e7df      	b.n	390e0 <__sinit+0xc>
   39120:	00039031 	.word	0x00039031

00039124 <__sfp>:
   39124:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
   39126:	4607      	mov	r7, r0
   39128:	f7ff ffbc 	bl	390a4 <__sfp_lock_acquire>
   3912c:	4b1e      	ldr	r3, [pc, #120]	; (391a8 <__sfp+0x84>)
   3912e:	681e      	ldr	r6, [r3, #0]
   39130:	6bb3      	ldr	r3, [r6, #56]	; 0x38
   39132:	b913      	cbnz	r3, 3913a <__sfp+0x16>
   39134:	4630      	mov	r0, r6
   39136:	f7ff ffcd 	bl	390d4 <__sinit>
   3913a:	f506 7638 	add.w	r6, r6, #736	; 0x2e0
   3913e:	e9d6 3401 	ldrd	r3, r4, [r6, #4]
   39142:	3b01      	subs	r3, #1
   39144:	d503      	bpl.n	3914e <__sfp+0x2a>
   39146:	6833      	ldr	r3, [r6, #0]
   39148:	b30b      	cbz	r3, 3918e <__sfp+0x6a>
   3914a:	6836      	ldr	r6, [r6, #0]
   3914c:	e7f7      	b.n	3913e <__sfp+0x1a>
   3914e:	f9b4 500c 	ldrsh.w	r5, [r4, #12]
   39152:	b9d5      	cbnz	r5, 3918a <__sfp+0x66>
   39154:	4b15      	ldr	r3, [pc, #84]	; (391ac <__sfp+0x88>)
   39156:	f104 0058 	add.w	r0, r4, #88	; 0x58
   3915a:	6665      	str	r5, [r4, #100]	; 0x64
   3915c:	60e3      	str	r3, [r4, #12]
   3915e:	f000 fb1e 	bl	3979e <__retarget_lock_init_recursive>
   39162:	f7ff ffa5 	bl	390b0 <__sfp_lock_release>
   39166:	2208      	movs	r2, #8
   39168:	4629      	mov	r1, r5
   3916a:	f104 005c 	add.w	r0, r4, #92	; 0x5c
   3916e:	6025      	str	r5, [r4, #0]
   39170:	61a5      	str	r5, [r4, #24]
   39172:	e9c4 5501 	strd	r5, r5, [r4, #4]
   39176:	e9c4 5504 	strd	r5, r5, [r4, #16]
   3917a:	f000 ff39 	bl	39ff0 <memset>
   3917e:	e9c4 550c 	strd	r5, r5, [r4, #48]	; 0x30
   39182:	e9c4 5511 	strd	r5, r5, [r4, #68]	; 0x44
   39186:	4620      	mov	r0, r4
   39188:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
   3918a:	3468      	adds	r4, #104	; 0x68
   3918c:	e7d9      	b.n	39142 <__sfp+0x1e>
   3918e:	2104      	movs	r1, #4
   39190:	4638      	mov	r0, r7
   39192:	f7ff ff6b 	bl	3906c <__sfmoreglue>
   39196:	4604      	mov	r4, r0
   39198:	6030      	str	r0, [r6, #0]
   3919a:	2800      	cmp	r0, #0
   3919c:	d1d5      	bne.n	3914a <__sfp+0x26>
   3919e:	f7ff ff87 	bl	390b0 <__sfp_lock_release>
   391a2:	230c      	movs	r3, #12
   391a4:	603b      	str	r3, [r7, #0]
   391a6:	e7ee      	b.n	39186 <__sfp+0x62>
   391a8:	00040700 	.word	0x00040700
   391ac:	ffff0001 	.word	0xffff0001

000391b0 <__fp_lock_all>:
   391b0:	b508      	push	{r3, lr}
   391b2:	f7ff ff77 	bl	390a4 <__sfp_lock_acquire>
   391b6:	4b03      	ldr	r3, [pc, #12]	; (391c4 <__fp_lock_all+0x14>)
   391b8:	4903      	ldr	r1, [pc, #12]	; (391c8 <__fp_lock_all+0x18>)
   391ba:	6818      	ldr	r0, [r3, #0]
   391bc:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
   391c0:	f000 ba74 	b.w	396ac <_fwalk>
   391c4:	20006000 	.word	0x20006000
   391c8:	0003903d 	.word	0x0003903d

000391cc <__fp_unlock_all>:
   391cc:	b508      	push	{r3, lr}
   391ce:	4b04      	ldr	r3, [pc, #16]	; (391e0 <__fp_unlock_all+0x14>)
   391d0:	4904      	ldr	r1, [pc, #16]	; (391e4 <__fp_unlock_all+0x18>)
   391d2:	6818      	ldr	r0, [r3, #0]
   391d4:	f000 fa6a 	bl	396ac <_fwalk>
   391d8:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
   391dc:	f7ff bf68 	b.w	390b0 <__sfp_lock_release>
   391e0:	20006000 	.word	0x20006000
   391e4:	00039055 	.word	0x00039055

000391e8 <free>:
   391e8:	4b02      	ldr	r3, [pc, #8]	; (391f4 <free+0xc>)
   391ea:	4601      	mov	r1, r0
   391ec:	6818      	ldr	r0, [r3, #0]
   391ee:	f000 b853 	b.w	39298 <_free_r>
   391f2:	bf00      	nop
   391f4:	20006000 	.word	0x20006000

000391f8 <_malloc_trim_r>:
   391f8:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
   391fa:	4f24      	ldr	r7, [pc, #144]	; (3928c <_malloc_trim_r+0x94>)
   391fc:	460c      	mov	r4, r1
   391fe:	4606      	mov	r6, r0
   39200:	f000 ff2e 	bl	3a060 <__malloc_lock>
   39204:	68bb      	ldr	r3, [r7, #8]
   39206:	f5c4 647e 	rsb	r4, r4, #4064	; 0xfe0
   3920a:	685d      	ldr	r5, [r3, #4]
   3920c:	340f      	adds	r4, #15
   3920e:	f025 0503 	bic.w	r5, r5, #3
   39212:	442c      	add	r4, r5
   39214:	f424 647f 	bic.w	r4, r4, #4080	; 0xff0
   39218:	f024 040f 	bic.w	r4, r4, #15
   3921c:	f5a4 5480 	sub.w	r4, r4, #4096	; 0x1000
   39220:	f5b4 5f80 	cmp.w	r4, #4096	; 0x1000
   39224:	da04      	bge.n	39230 <_malloc_trim_r+0x38>
   39226:	4630      	mov	r0, r6
   39228:	f000 ff20 	bl	3a06c <__malloc_unlock>
   3922c:	2000      	movs	r0, #0
   3922e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
   39230:	2100      	movs	r1, #0
   39232:	4630      	mov	r0, r6
   39234:	f001 fdc8 	bl	3adc8 <_sbrk_r>
   39238:	68bb      	ldr	r3, [r7, #8]
   3923a:	442b      	add	r3, r5
   3923c:	4298      	cmp	r0, r3
   3923e:	d1f2      	bne.n	39226 <_malloc_trim_r+0x2e>
   39240:	4261      	negs	r1, r4
   39242:	4630      	mov	r0, r6
   39244:	f001 fdc0 	bl	3adc8 <_sbrk_r>
   39248:	3001      	adds	r0, #1
   3924a:	d110      	bne.n	3926e <_malloc_trim_r+0x76>
   3924c:	2100      	movs	r1, #0
   3924e:	4630      	mov	r0, r6
   39250:	f001 fdba 	bl	3adc8 <_sbrk_r>
   39254:	68ba      	ldr	r2, [r7, #8]
   39256:	1a83      	subs	r3, r0, r2
   39258:	2b0f      	cmp	r3, #15
   3925a:	dde4      	ble.n	39226 <_malloc_trim_r+0x2e>
   3925c:	490c      	ldr	r1, [pc, #48]	; (39290 <_malloc_trim_r+0x98>)
   3925e:	f043 0301 	orr.w	r3, r3, #1
   39262:	6809      	ldr	r1, [r1, #0]
   39264:	6053      	str	r3, [r2, #4]
   39266:	1a40      	subs	r0, r0, r1
   39268:	490a      	ldr	r1, [pc, #40]	; (39294 <_malloc_trim_r+0x9c>)
   3926a:	6008      	str	r0, [r1, #0]
   3926c:	e7db      	b.n	39226 <_malloc_trim_r+0x2e>
   3926e:	1b2d      	subs	r5, r5, r4
   39270:	68bb      	ldr	r3, [r7, #8]
   39272:	4a08      	ldr	r2, [pc, #32]	; (39294 <_malloc_trim_r+0x9c>)
   39274:	4630      	mov	r0, r6
   39276:	f045 0501 	orr.w	r5, r5, #1
   3927a:	605d      	str	r5, [r3, #4]
   3927c:	6813      	ldr	r3, [r2, #0]
   3927e:	1b1b      	subs	r3, r3, r4
   39280:	6013      	str	r3, [r2, #0]
   39282:	f000 fef3 	bl	3a06c <__malloc_unlock>
   39286:	2001      	movs	r0, #1
   39288:	e7d1      	b.n	3922e <_malloc_trim_r+0x36>
   3928a:	bf00      	nop
   3928c:	2000659c 	.word	0x2000659c
   39290:	200069a4 	.word	0x200069a4
   39294:	200069dc 	.word	0x200069dc

00039298 <_free_r>:
   39298:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
   3929a:	4605      	mov	r5, r0
   3929c:	460f      	mov	r7, r1
   3929e:	2900      	cmp	r1, #0
   392a0:	f000 80b1 	beq.w	39406 <_free_r+0x16e>
   392a4:	f000 fedc 	bl	3a060 <__malloc_lock>
   392a8:	f857 2c04 	ldr.w	r2, [r7, #-4]
   392ac:	f1a7 0308 	sub.w	r3, r7, #8
   392b0:	4855      	ldr	r0, [pc, #340]	; (39408 <_free_r+0x170>)
   392b2:	f022 0401 	bic.w	r4, r2, #1
   392b6:	f002 0201 	and.w	r2, r2, #1
   392ba:	6881      	ldr	r1, [r0, #8]
   392bc:	eb03 0c04 	add.w	ip, r3, r4
   392c0:	f8dc 6004 	ldr.w	r6, [ip, #4]
   392c4:	4561      	cmp	r1, ip
   392c6:	f026 0603 	bic.w	r6, r6, #3
   392ca:	d11b      	bne.n	39304 <_free_r+0x6c>
   392cc:	4434      	add	r4, r6
   392ce:	b93a      	cbnz	r2, 392e0 <_free_r+0x48>
   392d0:	f857 2c08 	ldr.w	r2, [r7, #-8]
   392d4:	1a9b      	subs	r3, r3, r2
   392d6:	4414      	add	r4, r2
   392d8:	e9d3 1202 	ldrd	r1, r2, [r3, #8]
   392dc:	60ca      	str	r2, [r1, #12]
   392de:	6091      	str	r1, [r2, #8]
   392e0:	f044 0201 	orr.w	r2, r4, #1
   392e4:	605a      	str	r2, [r3, #4]
   392e6:	6083      	str	r3, [r0, #8]
   392e8:	4b48      	ldr	r3, [pc, #288]	; (3940c <_free_r+0x174>)
   392ea:	681b      	ldr	r3, [r3, #0]
   392ec:	42a3      	cmp	r3, r4
   392ee:	d804      	bhi.n	392fa <_free_r+0x62>
   392f0:	4b47      	ldr	r3, [pc, #284]	; (39410 <_free_r+0x178>)
   392f2:	4628      	mov	r0, r5
   392f4:	6819      	ldr	r1, [r3, #0]
   392f6:	f7ff ff7f 	bl	391f8 <_malloc_trim_r>
   392fa:	4628      	mov	r0, r5
   392fc:	e8bd 40f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, lr}
   39300:	f000 beb4 	b.w	3a06c <__malloc_unlock>
   39304:	f8cc 6004 	str.w	r6, [ip, #4]
   39308:	2a00      	cmp	r2, #0
   3930a:	d138      	bne.n	3937e <_free_r+0xe6>
   3930c:	f857 1c08 	ldr.w	r1, [r7, #-8]
   39310:	f100 0708 	add.w	r7, r0, #8
   39314:	1a5b      	subs	r3, r3, r1
   39316:	440c      	add	r4, r1
   39318:	6899      	ldr	r1, [r3, #8]
   3931a:	42b9      	cmp	r1, r7
   3931c:	d031      	beq.n	39382 <_free_r+0xea>
   3931e:	68df      	ldr	r7, [r3, #12]
   39320:	60cf      	str	r7, [r1, #12]
   39322:	60b9      	str	r1, [r7, #8]
   39324:	eb0c 0106 	add.w	r1, ip, r6
   39328:	6849      	ldr	r1, [r1, #4]
   3932a:	07c9      	lsls	r1, r1, #31
   3932c:	d40b      	bmi.n	39346 <_free_r+0xae>
   3932e:	4434      	add	r4, r6
   39330:	f8dc 1008 	ldr.w	r1, [ip, #8]
   39334:	bb3a      	cbnz	r2, 39386 <_free_r+0xee>
   39336:	4e37      	ldr	r6, [pc, #220]	; (39414 <_free_r+0x17c>)
   39338:	42b1      	cmp	r1, r6
   3933a:	d124      	bne.n	39386 <_free_r+0xee>
   3933c:	2201      	movs	r2, #1
   3933e:	e9c0 3304 	strd	r3, r3, [r0, #16]
   39342:	e9c3 1102 	strd	r1, r1, [r3, #8]
   39346:	f044 0101 	orr.w	r1, r4, #1
   3934a:	6059      	str	r1, [r3, #4]
   3934c:	511c      	str	r4, [r3, r4]
   3934e:	2a00      	cmp	r2, #0
   39350:	d1d3      	bne.n	392fa <_free_r+0x62>
   39352:	f5b4 7f00 	cmp.w	r4, #512	; 0x200
   39356:	d21b      	bcs.n	39390 <_free_r+0xf8>
   39358:	0961      	lsrs	r1, r4, #5
   3935a:	08e2      	lsrs	r2, r4, #3
   3935c:	2401      	movs	r4, #1
   3935e:	3201      	adds	r2, #1
   39360:	408c      	lsls	r4, r1
   39362:	6841      	ldr	r1, [r0, #4]
   39364:	430c      	orrs	r4, r1
   39366:	eb00 01c2 	add.w	r1, r0, r2, lsl #3
   3936a:	6044      	str	r4, [r0, #4]
   3936c:	3908      	subs	r1, #8
   3936e:	f850 4032 	ldr.w	r4, [r0, r2, lsl #3]
   39372:	e9c3 4102 	strd	r4, r1, [r3, #8]
   39376:	f840 3032 	str.w	r3, [r0, r2, lsl #3]
   3937a:	60e3      	str	r3, [r4, #12]
   3937c:	e7bd      	b.n	392fa <_free_r+0x62>
   3937e:	2200      	movs	r2, #0
   39380:	e7d0      	b.n	39324 <_free_r+0x8c>
   39382:	2201      	movs	r2, #1
   39384:	e7ce      	b.n	39324 <_free_r+0x8c>
   39386:	f8dc 600c 	ldr.w	r6, [ip, #12]
   3938a:	60ce      	str	r6, [r1, #12]
   3938c:	60b1      	str	r1, [r6, #8]
   3938e:	e7da      	b.n	39346 <_free_r+0xae>
   39390:	f5b4 6f20 	cmp.w	r4, #2560	; 0xa00
   39394:	ea4f 2254 	mov.w	r2, r4, lsr #9
   39398:	d214      	bcs.n	393c4 <_free_r+0x12c>
   3939a:	09a2      	lsrs	r2, r4, #6
   3939c:	3238      	adds	r2, #56	; 0x38
   3939e:	1c51      	adds	r1, r2, #1
   393a0:	eb00 06c2 	add.w	r6, r0, r2, lsl #3
   393a4:	f850 1031 	ldr.w	r1, [r0, r1, lsl #3]
   393a8:	428e      	cmp	r6, r1
   393aa:	d125      	bne.n	393f8 <_free_r+0x160>
   393ac:	2401      	movs	r4, #1
   393ae:	1092      	asrs	r2, r2, #2
   393b0:	fa04 f202 	lsl.w	r2, r4, r2
   393b4:	6844      	ldr	r4, [r0, #4]
   393b6:	4322      	orrs	r2, r4
   393b8:	6042      	str	r2, [r0, #4]
   393ba:	e9c3 1602 	strd	r1, r6, [r3, #8]
   393be:	60b3      	str	r3, [r6, #8]
   393c0:	60cb      	str	r3, [r1, #12]
   393c2:	e79a      	b.n	392fa <_free_r+0x62>
   393c4:	2a14      	cmp	r2, #20
   393c6:	d801      	bhi.n	393cc <_free_r+0x134>
   393c8:	325b      	adds	r2, #91	; 0x5b
   393ca:	e7e8      	b.n	3939e <_free_r+0x106>
   393cc:	2a54      	cmp	r2, #84	; 0x54
   393ce:	d802      	bhi.n	393d6 <_free_r+0x13e>
   393d0:	0b22      	lsrs	r2, r4, #12
   393d2:	326e      	adds	r2, #110	; 0x6e
   393d4:	e7e3      	b.n	3939e <_free_r+0x106>
   393d6:	f5b2 7faa 	cmp.w	r2, #340	; 0x154
   393da:	d802      	bhi.n	393e2 <_free_r+0x14a>
   393dc:	0be2      	lsrs	r2, r4, #15
   393de:	3277      	adds	r2, #119	; 0x77
   393e0:	e7dd      	b.n	3939e <_free_r+0x106>
   393e2:	f240 5154 	movw	r1, #1364	; 0x554
   393e6:	428a      	cmp	r2, r1
   393e8:	bf96      	itet	ls
   393ea:	0ca2      	lsrls	r2, r4, #18
   393ec:	227e      	movhi	r2, #126	; 0x7e
   393ee:	327c      	addls	r2, #124	; 0x7c
   393f0:	e7d5      	b.n	3939e <_free_r+0x106>
   393f2:	6889      	ldr	r1, [r1, #8]
   393f4:	428e      	cmp	r6, r1
   393f6:	d004      	beq.n	39402 <_free_r+0x16a>
   393f8:	684a      	ldr	r2, [r1, #4]
   393fa:	f022 0203 	bic.w	r2, r2, #3
   393fe:	42a2      	cmp	r2, r4
   39400:	d8f7      	bhi.n	393f2 <_free_r+0x15a>
   39402:	68ce      	ldr	r6, [r1, #12]
   39404:	e7d9      	b.n	393ba <_free_r+0x122>
   39406:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
   39408:	2000659c 	.word	0x2000659c
   3940c:	200069a8 	.word	0x200069a8
   39410:	200069d8 	.word	0x200069d8
   39414:	200065a4 	.word	0x200065a4

00039418 <__sfvwrite_r>:
   39418:	6893      	ldr	r3, [r2, #8]
   3941a:	e92d 4ff7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3941e:	4606      	mov	r6, r0
   39420:	460c      	mov	r4, r1
   39422:	4690      	mov	r8, r2
   39424:	b91b      	cbnz	r3, 3942e <__sfvwrite_r+0x16>
   39426:	2000      	movs	r0, #0
   39428:	b003      	add	sp, #12
   3942a:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3942e:	898b      	ldrh	r3, [r1, #12]
   39430:	0718      	lsls	r0, r3, #28
   39432:	d550      	bpl.n	394d6 <__sfvwrite_r+0xbe>
   39434:	690b      	ldr	r3, [r1, #16]
   39436:	2b00      	cmp	r3, #0
   39438:	d04d      	beq.n	394d6 <__sfvwrite_r+0xbe>
   3943a:	89a3      	ldrh	r3, [r4, #12]
   3943c:	f8d8 7000 	ldr.w	r7, [r8]
   39440:	f013 0902 	ands.w	r9, r3, #2
   39444:	d16c      	bne.n	39520 <__sfvwrite_r+0x108>
   39446:	f013 0301 	ands.w	r3, r3, #1
   3944a:	f000 809c 	beq.w	39586 <__sfvwrite_r+0x16e>
   3944e:	4648      	mov	r0, r9
   39450:	46ca      	mov	sl, r9
   39452:	46cb      	mov	fp, r9
   39454:	f1bb 0f00 	cmp.w	fp, #0
   39458:	f000 8103 	beq.w	39662 <__sfvwrite_r+0x24a>
   3945c:	b950      	cbnz	r0, 39474 <__sfvwrite_r+0x5c>
   3945e:	465a      	mov	r2, fp
   39460:	210a      	movs	r1, #10
   39462:	4650      	mov	r0, sl
   39464:	f000 fca0 	bl	39da8 <memchr>
   39468:	2800      	cmp	r0, #0
   3946a:	f000 80ff 	beq.w	3966c <__sfvwrite_r+0x254>
   3946e:	3001      	adds	r0, #1
   39470:	eba0 090a 	sub.w	r9, r0, sl
   39474:	45d9      	cmp	r9, fp
   39476:	6820      	ldr	r0, [r4, #0]
   39478:	6921      	ldr	r1, [r4, #16]
   3947a:	464a      	mov	r2, r9
   3947c:	bf28      	it	cs
   3947e:	465a      	movcs	r2, fp
   39480:	6963      	ldr	r3, [r4, #20]
   39482:	4288      	cmp	r0, r1
   39484:	f240 80f5 	bls.w	39672 <__sfvwrite_r+0x25a>
   39488:	68a5      	ldr	r5, [r4, #8]
   3948a:	441d      	add	r5, r3
   3948c:	42aa      	cmp	r2, r5
   3948e:	f340 80f0 	ble.w	39672 <__sfvwrite_r+0x25a>
   39492:	4651      	mov	r1, sl
   39494:	462a      	mov	r2, r5
   39496:	f000 fd55 	bl	39f44 <memmove>
   3949a:	6823      	ldr	r3, [r4, #0]
   3949c:	4621      	mov	r1, r4
   3949e:	4630      	mov	r0, r6
   394a0:	442b      	add	r3, r5
   394a2:	6023      	str	r3, [r4, #0]
   394a4:	f7ff fd66 	bl	38f74 <_fflush_r>
   394a8:	2800      	cmp	r0, #0
   394aa:	d167      	bne.n	3957c <__sfvwrite_r+0x164>
   394ac:	ebb9 0905 	subs.w	r9, r9, r5
   394b0:	f040 80f7 	bne.w	396a2 <__sfvwrite_r+0x28a>
   394b4:	4621      	mov	r1, r4
   394b6:	4630      	mov	r0, r6
   394b8:	f7ff fd5c 	bl	38f74 <_fflush_r>
   394bc:	2800      	cmp	r0, #0
   394be:	d15d      	bne.n	3957c <__sfvwrite_r+0x164>
   394c0:	f8d8 2008 	ldr.w	r2, [r8, #8]
   394c4:	44aa      	add	sl, r5
   394c6:	ebab 0b05 	sub.w	fp, fp, r5
   394ca:	1b55      	subs	r5, r2, r5
   394cc:	f8c8 5008 	str.w	r5, [r8, #8]
   394d0:	2d00      	cmp	r5, #0
   394d2:	d1bf      	bne.n	39454 <__sfvwrite_r+0x3c>
   394d4:	e7a7      	b.n	39426 <__sfvwrite_r+0xe>
   394d6:	4621      	mov	r1, r4
   394d8:	4630      	mov	r0, r6
   394da:	f7fe fdef 	bl	380bc <__swsetup_r>
   394de:	2800      	cmp	r0, #0
   394e0:	d0ab      	beq.n	3943a <__sfvwrite_r+0x22>
   394e2:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   394e6:	e79f      	b.n	39428 <__sfvwrite_r+0x10>
   394e8:	e9d7 b900 	ldrd	fp, r9, [r7]
   394ec:	3708      	adds	r7, #8
   394ee:	f1b9 0f00 	cmp.w	r9, #0
   394f2:	d0f9      	beq.n	394e8 <__sfvwrite_r+0xd0>
   394f4:	45d1      	cmp	r9, sl
   394f6:	464b      	mov	r3, r9
   394f8:	465a      	mov	r2, fp
   394fa:	69e1      	ldr	r1, [r4, #28]
   394fc:	bf28      	it	cs
   394fe:	4653      	movcs	r3, sl
   39500:	4630      	mov	r0, r6
   39502:	6a65      	ldr	r5, [r4, #36]	; 0x24
   39504:	47a8      	blx	r5
   39506:	2800      	cmp	r0, #0
   39508:	dd38      	ble.n	3957c <__sfvwrite_r+0x164>
   3950a:	f8d8 3008 	ldr.w	r3, [r8, #8]
   3950e:	4483      	add	fp, r0
   39510:	eba9 0900 	sub.w	r9, r9, r0
   39514:	1a18      	subs	r0, r3, r0
   39516:	f8c8 0008 	str.w	r0, [r8, #8]
   3951a:	2800      	cmp	r0, #0
   3951c:	d1e7      	bne.n	394ee <__sfvwrite_r+0xd6>
   3951e:	e782      	b.n	39426 <__sfvwrite_r+0xe>
   39520:	f04f 0b00 	mov.w	fp, #0
   39524:	f8df a180 	ldr.w	sl, [pc, #384]	; 396a8 <__sfvwrite_r+0x290>
   39528:	46d9      	mov	r9, fp
   3952a:	e7e0      	b.n	394ee <__sfvwrite_r+0xd6>
   3952c:	e9d7 9a00 	ldrd	r9, sl, [r7]
   39530:	3708      	adds	r7, #8
   39532:	f1ba 0f00 	cmp.w	sl, #0
   39536:	d0f9      	beq.n	3952c <__sfvwrite_r+0x114>
   39538:	89a3      	ldrh	r3, [r4, #12]
   3953a:	6820      	ldr	r0, [r4, #0]
   3953c:	0599      	lsls	r1, r3, #22
   3953e:	68a2      	ldr	r2, [r4, #8]
   39540:	d563      	bpl.n	3960a <__sfvwrite_r+0x1f2>
   39542:	4552      	cmp	r2, sl
   39544:	d836      	bhi.n	395b4 <__sfvwrite_r+0x19c>
   39546:	f413 6f90 	tst.w	r3, #1152	; 0x480
   3954a:	d033      	beq.n	395b4 <__sfvwrite_r+0x19c>
   3954c:	6921      	ldr	r1, [r4, #16]
   3954e:	6965      	ldr	r5, [r4, #20]
   39550:	eba0 0b01 	sub.w	fp, r0, r1
   39554:	eb05 0545 	add.w	r5, r5, r5, lsl #1
   39558:	f10b 0201 	add.w	r2, fp, #1
   3955c:	eb05 75d5 	add.w	r5, r5, r5, lsr #31
   39560:	4452      	add	r2, sl
   39562:	106d      	asrs	r5, r5, #1
   39564:	4295      	cmp	r5, r2
   39566:	bf38      	it	cc
   39568:	4615      	movcc	r5, r2
   3956a:	055b      	lsls	r3, r3, #21
   3956c:	d53d      	bpl.n	395ea <__sfvwrite_r+0x1d2>
   3956e:	4629      	mov	r1, r5
   39570:	4630      	mov	r0, r6
   39572:	f000 f9a5 	bl	398c0 <_malloc_r>
   39576:	b948      	cbnz	r0, 3958c <__sfvwrite_r+0x174>
   39578:	230c      	movs	r3, #12
   3957a:	6033      	str	r3, [r6, #0]
   3957c:	89a3      	ldrh	r3, [r4, #12]
   3957e:	f043 0340 	orr.w	r3, r3, #64	; 0x40
   39582:	81a3      	strh	r3, [r4, #12]
   39584:	e7ad      	b.n	394e2 <__sfvwrite_r+0xca>
   39586:	4699      	mov	r9, r3
   39588:	469a      	mov	sl, r3
   3958a:	e7d2      	b.n	39532 <__sfvwrite_r+0x11a>
   3958c:	465a      	mov	r2, fp
   3958e:	6921      	ldr	r1, [r4, #16]
   39590:	9001      	str	r0, [sp, #4]
   39592:	f000 fc61 	bl	39e58 <memcpy>
   39596:	89a2      	ldrh	r2, [r4, #12]
   39598:	9b01      	ldr	r3, [sp, #4]
   3959a:	f422 6290 	bic.w	r2, r2, #1152	; 0x480
   3959e:	f042 0280 	orr.w	r2, r2, #128	; 0x80
   395a2:	81a2      	strh	r2, [r4, #12]
   395a4:	6123      	str	r3, [r4, #16]
   395a6:	4652      	mov	r2, sl
   395a8:	445b      	add	r3, fp
   395aa:	6165      	str	r5, [r4, #20]
   395ac:	eba5 050b 	sub.w	r5, r5, fp
   395b0:	6023      	str	r3, [r4, #0]
   395b2:	60a5      	str	r5, [r4, #8]
   395b4:	4552      	cmp	r2, sl
   395b6:	4649      	mov	r1, r9
   395b8:	6820      	ldr	r0, [r4, #0]
   395ba:	4655      	mov	r5, sl
   395bc:	bf28      	it	cs
   395be:	4652      	movcs	r2, sl
   395c0:	9201      	str	r2, [sp, #4]
   395c2:	f000 fcbf 	bl	39f44 <memmove>
   395c6:	68a3      	ldr	r3, [r4, #8]
   395c8:	9a01      	ldr	r2, [sp, #4]
   395ca:	1a9b      	subs	r3, r3, r2
   395cc:	60a3      	str	r3, [r4, #8]
   395ce:	6823      	ldr	r3, [r4, #0]
   395d0:	441a      	add	r2, r3
   395d2:	6022      	str	r2, [r4, #0]
   395d4:	f8d8 0008 	ldr.w	r0, [r8, #8]
   395d8:	44a9      	add	r9, r5
   395da:	ebaa 0a05 	sub.w	sl, sl, r5
   395de:	1b45      	subs	r5, r0, r5
   395e0:	f8c8 5008 	str.w	r5, [r8, #8]
   395e4:	2d00      	cmp	r5, #0
   395e6:	d1a4      	bne.n	39532 <__sfvwrite_r+0x11a>
   395e8:	e71d      	b.n	39426 <__sfvwrite_r+0xe>
   395ea:	462a      	mov	r2, r5
   395ec:	4630      	mov	r0, r6
   395ee:	f001 f9ef 	bl	3a9d0 <_realloc_r>
   395f2:	4603      	mov	r3, r0
   395f4:	2800      	cmp	r0, #0
   395f6:	d1d5      	bne.n	395a4 <__sfvwrite_r+0x18c>
   395f8:	6921      	ldr	r1, [r4, #16]
   395fa:	4630      	mov	r0, r6
   395fc:	f7ff fe4c 	bl	39298 <_free_r>
   39600:	89a3      	ldrh	r3, [r4, #12]
   39602:	f023 0380 	bic.w	r3, r3, #128	; 0x80
   39606:	81a3      	strh	r3, [r4, #12]
   39608:	e7b6      	b.n	39578 <__sfvwrite_r+0x160>
   3960a:	6923      	ldr	r3, [r4, #16]
   3960c:	4283      	cmp	r3, r0
   3960e:	d302      	bcc.n	39616 <__sfvwrite_r+0x1fe>
   39610:	6961      	ldr	r1, [r4, #20]
   39612:	4551      	cmp	r1, sl
   39614:	d915      	bls.n	39642 <__sfvwrite_r+0x22a>
   39616:	4552      	cmp	r2, sl
   39618:	4649      	mov	r1, r9
   3961a:	bf28      	it	cs
   3961c:	4652      	movcs	r2, sl
   3961e:	4615      	mov	r5, r2
   39620:	f000 fc90 	bl	39f44 <memmove>
   39624:	68a3      	ldr	r3, [r4, #8]
   39626:	6822      	ldr	r2, [r4, #0]
   39628:	1b5b      	subs	r3, r3, r5
   3962a:	442a      	add	r2, r5
   3962c:	60a3      	str	r3, [r4, #8]
   3962e:	6022      	str	r2, [r4, #0]
   39630:	2b00      	cmp	r3, #0
   39632:	d1cf      	bne.n	395d4 <__sfvwrite_r+0x1bc>
   39634:	4621      	mov	r1, r4
   39636:	4630      	mov	r0, r6
   39638:	f7ff fc9c 	bl	38f74 <_fflush_r>
   3963c:	2800      	cmp	r0, #0
   3963e:	d0c9      	beq.n	395d4 <__sfvwrite_r+0x1bc>
   39640:	e79c      	b.n	3957c <__sfvwrite_r+0x164>
   39642:	f06f 4300 	mvn.w	r3, #2147483648	; 0x80000000
   39646:	6a65      	ldr	r5, [r4, #36]	; 0x24
   39648:	464a      	mov	r2, r9
   3964a:	4630      	mov	r0, r6
   3964c:	459a      	cmp	sl, r3
   3964e:	bf38      	it	cc
   39650:	4653      	movcc	r3, sl
   39652:	fb93 f3f1 	sdiv	r3, r3, r1
   39656:	434b      	muls	r3, r1
   39658:	69e1      	ldr	r1, [r4, #28]
   3965a:	47a8      	blx	r5
   3965c:	1e05      	subs	r5, r0, #0
   3965e:	dcb9      	bgt.n	395d4 <__sfvwrite_r+0x1bc>
   39660:	e78c      	b.n	3957c <__sfvwrite_r+0x164>
   39662:	e9d7 ab00 	ldrd	sl, fp, [r7]
   39666:	2000      	movs	r0, #0
   39668:	3708      	adds	r7, #8
   3966a:	e6f3      	b.n	39454 <__sfvwrite_r+0x3c>
   3966c:	f10b 0901 	add.w	r9, fp, #1
   39670:	e700      	b.n	39474 <__sfvwrite_r+0x5c>
   39672:	4293      	cmp	r3, r2
   39674:	dc08      	bgt.n	39688 <__sfvwrite_r+0x270>
   39676:	6a65      	ldr	r5, [r4, #36]	; 0x24
   39678:	4652      	mov	r2, sl
   3967a:	69e1      	ldr	r1, [r4, #28]
   3967c:	4630      	mov	r0, r6
   3967e:	47a8      	blx	r5
   39680:	1e05      	subs	r5, r0, #0
   39682:	f73f af13 	bgt.w	394ac <__sfvwrite_r+0x94>
   39686:	e779      	b.n	3957c <__sfvwrite_r+0x164>
   39688:	4651      	mov	r1, sl
   3968a:	9201      	str	r2, [sp, #4]
   3968c:	f000 fc5a 	bl	39f44 <memmove>
   39690:	9a01      	ldr	r2, [sp, #4]
   39692:	68a3      	ldr	r3, [r4, #8]
   39694:	4615      	mov	r5, r2
   39696:	1a9b      	subs	r3, r3, r2
   39698:	60a3      	str	r3, [r4, #8]
   3969a:	6823      	ldr	r3, [r4, #0]
   3969c:	4413      	add	r3, r2
   3969e:	6023      	str	r3, [r4, #0]
   396a0:	e704      	b.n	394ac <__sfvwrite_r+0x94>
   396a2:	2001      	movs	r0, #1
   396a4:	e70c      	b.n	394c0 <__sfvwrite_r+0xa8>
   396a6:	bf00      	nop
   396a8:	7ffffc00 	.word	0x7ffffc00

000396ac <_fwalk>:
   396ac:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
   396b0:	460f      	mov	r7, r1
   396b2:	f500 7438 	add.w	r4, r0, #736	; 0x2e0
   396b6:	2600      	movs	r6, #0
   396b8:	e9d4 8501 	ldrd	r8, r5, [r4, #4]
   396bc:	f1b8 0801 	subs.w	r8, r8, #1
   396c0:	d505      	bpl.n	396ce <_fwalk+0x22>
   396c2:	6824      	ldr	r4, [r4, #0]
   396c4:	2c00      	cmp	r4, #0
   396c6:	d1f7      	bne.n	396b8 <_fwalk+0xc>
   396c8:	4630      	mov	r0, r6
   396ca:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
   396ce:	89ab      	ldrh	r3, [r5, #12]
   396d0:	2b01      	cmp	r3, #1
   396d2:	d906      	bls.n	396e2 <_fwalk+0x36>
   396d4:	f9b5 300e 	ldrsh.w	r3, [r5, #14]
   396d8:	3301      	adds	r3, #1
   396da:	d002      	beq.n	396e2 <_fwalk+0x36>
   396dc:	4628      	mov	r0, r5
   396de:	47b8      	blx	r7
   396e0:	4306      	orrs	r6, r0
   396e2:	3568      	adds	r5, #104	; 0x68
   396e4:	e7ea      	b.n	396bc <_fwalk+0x10>

000396e6 <_fwalk_reent>:
   396e6:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
   396ea:	4606      	mov	r6, r0
   396ec:	4688      	mov	r8, r1
   396ee:	f500 7438 	add.w	r4, r0, #736	; 0x2e0
   396f2:	2700      	movs	r7, #0
   396f4:	e9d4 9501 	ldrd	r9, r5, [r4, #4]
   396f8:	f1b9 0901 	subs.w	r9, r9, #1
   396fc:	d505      	bpl.n	3970a <_fwalk_reent+0x24>
   396fe:	6824      	ldr	r4, [r4, #0]
   39700:	2c00      	cmp	r4, #0
   39702:	d1f7      	bne.n	396f4 <_fwalk_reent+0xe>
   39704:	4638      	mov	r0, r7
   39706:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
   3970a:	89ab      	ldrh	r3, [r5, #12]
   3970c:	2b01      	cmp	r3, #1
   3970e:	d907      	bls.n	39720 <_fwalk_reent+0x3a>
   39710:	f9b5 300e 	ldrsh.w	r3, [r5, #14]
   39714:	3301      	adds	r3, #1
   39716:	d003      	beq.n	39720 <_fwalk_reent+0x3a>
   39718:	4629      	mov	r1, r5
   3971a:	4630      	mov	r0, r6
   3971c:	47c0      	blx	r8
   3971e:	4307      	orrs	r7, r0
   39720:	3568      	adds	r5, #104	; 0x68
   39722:	e7e9      	b.n	396f8 <_fwalk_reent+0x12>

00039724 <iswspace>:
   39724:	2100      	movs	r1, #0
   39726:	f005 bd35 	b.w	3f194 <iswspace_l>
	...

0003972c <__localeconv_l>:
   3972c:	30f0      	adds	r0, #240	; 0xf0
   3972e:	4770      	bx	lr

00039730 <_localeconv_r>:
   39730:	4800      	ldr	r0, [pc, #0]	; (39734 <_localeconv_r+0x4>)
   39732:	4770      	bx	lr
   39734:	20006520 	.word	0x20006520

00039738 <localeconv>:
   39738:	4800      	ldr	r0, [pc, #0]	; (3973c <localeconv+0x4>)
   3973a:	4770      	bx	lr
   3973c:	20006520 	.word	0x20006520

00039740 <_setlocale_r>:
   39740:	b510      	push	{r4, lr}
   39742:	4614      	mov	r4, r2
   39744:	b90a      	cbnz	r2, 3974a <_setlocale_r+0xa>
   39746:	480b      	ldr	r0, [pc, #44]	; (39774 <_setlocale_r+0x34>)
   39748:	bd10      	pop	{r4, pc}
   3974a:	490b      	ldr	r1, [pc, #44]	; (39778 <_setlocale_r+0x38>)
   3974c:	4610      	mov	r0, r2
   3974e:	f001 fbc1 	bl	3aed4 <strcmp>
   39752:	2800      	cmp	r0, #0
   39754:	d0f7      	beq.n	39746 <_setlocale_r+0x6>
   39756:	4907      	ldr	r1, [pc, #28]	; (39774 <_setlocale_r+0x34>)
   39758:	4620      	mov	r0, r4
   3975a:	f001 fbbb 	bl	3aed4 <strcmp>
   3975e:	2800      	cmp	r0, #0
   39760:	d0f1      	beq.n	39746 <_setlocale_r+0x6>
   39762:	4906      	ldr	r1, [pc, #24]	; (3977c <_setlocale_r+0x3c>)
   39764:	4620      	mov	r0, r4
   39766:	f001 fbb5 	bl	3aed4 <strcmp>
   3976a:	2800      	cmp	r0, #0
   3976c:	d0eb      	beq.n	39746 <_setlocale_r+0x6>
   3976e:	2000      	movs	r0, #0
   39770:	e7ea      	b.n	39748 <_setlocale_r+0x8>
   39772:	bf00      	nop
   39774:	00040704 	.word	0x00040704
   39778:	00040706 	.word	0x00040706
   3977c:	0004099d 	.word	0x0004099d

00039780 <__locale_mb_cur_max>:
   39780:	4b01      	ldr	r3, [pc, #4]	; (39788 <__locale_mb_cur_max+0x8>)
   39782:	f893 0128 	ldrb.w	r0, [r3, #296]	; 0x128
   39786:	4770      	bx	lr
   39788:	20006430 	.word	0x20006430

0003978c <setlocale>:
   3978c:	4b02      	ldr	r3, [pc, #8]	; (39798 <setlocale+0xc>)
   3978e:	460a      	mov	r2, r1
   39790:	4601      	mov	r1, r0
   39792:	6818      	ldr	r0, [r3, #0]
   39794:	f7ff bfd4 	b.w	39740 <_setlocale_r>
   39798:	20006000 	.word	0x20006000

0003979c <__retarget_lock_init>:
   3979c:	4770      	bx	lr

0003979e <__retarget_lock_init_recursive>:
   3979e:	4770      	bx	lr

000397a0 <__retarget_lock_close>:
   397a0:	4770      	bx	lr

000397a2 <__retarget_lock_close_recursive>:
   397a2:	4770      	bx	lr

000397a4 <__retarget_lock_acquire>:
   397a4:	4770      	bx	lr

000397a6 <__retarget_lock_acquire_recursive>:
   397a6:	4770      	bx	lr

000397a8 <__retarget_lock_try_acquire>:
   397a8:	2001      	movs	r0, #1
   397aa:	4770      	bx	lr

000397ac <__retarget_lock_try_acquire_recursive>:
   397ac:	2001      	movs	r0, #1
   397ae:	4770      	bx	lr

000397b0 <__retarget_lock_release>:
   397b0:	4770      	bx	lr

000397b2 <__retarget_lock_release_recursive>:
   397b2:	4770      	bx	lr

000397b4 <_lseek_r>:
   397b4:	b538      	push	{r3, r4, r5, lr}
   397b6:	4604      	mov	r4, r0
   397b8:	4d06      	ldr	r5, [pc, #24]	; (397d4 <_lseek_r+0x20>)
   397ba:	4608      	mov	r0, r1
   397bc:	4611      	mov	r1, r2
   397be:	2200      	movs	r2, #0
   397c0:	602a      	str	r2, [r5, #0]
   397c2:	461a      	mov	r2, r3
   397c4:	f006 fdce 	bl	40364 <_lseek>
   397c8:	1c43      	adds	r3, r0, #1
   397ca:	d102      	bne.n	397d2 <_lseek_r+0x1e>
   397cc:	682b      	ldr	r3, [r5, #0]
   397ce:	b103      	cbz	r3, 397d2 <_lseek_r+0x1e>
   397d0:	6023      	str	r3, [r4, #0]
   397d2:	bd38      	pop	{r3, r4, r5, pc}
   397d4:	20006a0c 	.word	0x20006a0c

000397d8 <__swhatbuf_r>:
   397d8:	b570      	push	{r4, r5, r6, lr}
   397da:	460e      	mov	r6, r1
   397dc:	f9b1 100e 	ldrsh.w	r1, [r1, #14]
   397e0:	b096      	sub	sp, #88	; 0x58
   397e2:	4614      	mov	r4, r2
   397e4:	2900      	cmp	r1, #0
   397e6:	461d      	mov	r5, r3
   397e8:	da0a      	bge.n	39800 <__swhatbuf_r+0x28>
   397ea:	f9b6 100c 	ldrsh.w	r1, [r6, #12]
   397ee:	2300      	movs	r3, #0
   397f0:	f011 0080 	ands.w	r0, r1, #128	; 0x80
   397f4:	602b      	str	r3, [r5, #0]
   397f6:	d116      	bne.n	39826 <__swhatbuf_r+0x4e>
   397f8:	f44f 6380 	mov.w	r3, #1024	; 0x400
   397fc:	6023      	str	r3, [r4, #0]
   397fe:	e015      	b.n	3982c <__swhatbuf_r+0x54>
   39800:	466a      	mov	r2, sp
   39802:	f005 f937 	bl	3ea74 <_fstat_r>
   39806:	2800      	cmp	r0, #0
   39808:	dbef      	blt.n	397ea <__swhatbuf_r+0x12>
   3980a:	9a01      	ldr	r2, [sp, #4]
   3980c:	f44f 6000 	mov.w	r0, #2048	; 0x800
   39810:	f402 4270 	and.w	r2, r2, #61440	; 0xf000
   39814:	f5a2 5300 	sub.w	r3, r2, #8192	; 0x2000
   39818:	425a      	negs	r2, r3
   3981a:	415a      	adcs	r2, r3
   3981c:	f44f 6380 	mov.w	r3, #1024	; 0x400
   39820:	602a      	str	r2, [r5, #0]
   39822:	6023      	str	r3, [r4, #0]
   39824:	e002      	b.n	3982c <__swhatbuf_r+0x54>
   39826:	2240      	movs	r2, #64	; 0x40
   39828:	4618      	mov	r0, r3
   3982a:	6022      	str	r2, [r4, #0]
   3982c:	b016      	add	sp, #88	; 0x58
   3982e:	bd70      	pop	{r4, r5, r6, pc}

00039830 <__smakebuf_r>:
   39830:	898b      	ldrh	r3, [r1, #12]
   39832:	b573      	push	{r0, r1, r4, r5, r6, lr}
   39834:	079d      	lsls	r5, r3, #30
   39836:	4606      	mov	r6, r0
   39838:	460c      	mov	r4, r1
   3983a:	d507      	bpl.n	3984c <__smakebuf_r+0x1c>
   3983c:	f104 0343 	add.w	r3, r4, #67	; 0x43
   39840:	6023      	str	r3, [r4, #0]
   39842:	6123      	str	r3, [r4, #16]
   39844:	2301      	movs	r3, #1
   39846:	6163      	str	r3, [r4, #20]
   39848:	b002      	add	sp, #8
   3984a:	bd70      	pop	{r4, r5, r6, pc}
   3984c:	ab01      	add	r3, sp, #4
   3984e:	466a      	mov	r2, sp
   39850:	f7ff ffc2 	bl	397d8 <__swhatbuf_r>
   39854:	9900      	ldr	r1, [sp, #0]
   39856:	4605      	mov	r5, r0
   39858:	4630      	mov	r0, r6
   3985a:	f000 f831 	bl	398c0 <_malloc_r>
   3985e:	b948      	cbnz	r0, 39874 <__smakebuf_r+0x44>
   39860:	f9b4 300c 	ldrsh.w	r3, [r4, #12]
   39864:	059a      	lsls	r2, r3, #22
   39866:	d4ef      	bmi.n	39848 <__smakebuf_r+0x18>
   39868:	f023 0303 	bic.w	r3, r3, #3
   3986c:	f043 0302 	orr.w	r3, r3, #2
   39870:	81a3      	strh	r3, [r4, #12]
   39872:	e7e3      	b.n	3983c <__smakebuf_r+0xc>
   39874:	4b0d      	ldr	r3, [pc, #52]	; (398ac <__smakebuf_r+0x7c>)
   39876:	63f3      	str	r3, [r6, #60]	; 0x3c
   39878:	89a3      	ldrh	r3, [r4, #12]
   3987a:	6020      	str	r0, [r4, #0]
   3987c:	f043 0380 	orr.w	r3, r3, #128	; 0x80
   39880:	6120      	str	r0, [r4, #16]
   39882:	81a3      	strh	r3, [r4, #12]
   39884:	9b00      	ldr	r3, [sp, #0]
   39886:	6163      	str	r3, [r4, #20]
   39888:	9b01      	ldr	r3, [sp, #4]
   3988a:	b15b      	cbz	r3, 398a4 <__smakebuf_r+0x74>
   3988c:	f9b4 100e 	ldrsh.w	r1, [r4, #14]
   39890:	4630      	mov	r0, r6
   39892:	f005 fc6f 	bl	3f174 <_isatty_r>
   39896:	b128      	cbz	r0, 398a4 <__smakebuf_r+0x74>
   39898:	89a3      	ldrh	r3, [r4, #12]
   3989a:	f023 0303 	bic.w	r3, r3, #3
   3989e:	f043 0301 	orr.w	r3, r3, #1
   398a2:	81a3      	strh	r3, [r4, #12]
   398a4:	89a0      	ldrh	r0, [r4, #12]
   398a6:	4305      	orrs	r5, r0
   398a8:	81a5      	strh	r5, [r4, #12]
   398aa:	e7cd      	b.n	39848 <__smakebuf_r+0x18>
   398ac:	00039031 	.word	0x00039031

000398b0 <malloc>:
   398b0:	4b02      	ldr	r3, [pc, #8]	; (398bc <malloc+0xc>)
   398b2:	4601      	mov	r1, r0
   398b4:	6818      	ldr	r0, [r3, #0]
   398b6:	f000 b803 	b.w	398c0 <_malloc_r>
   398ba:	bf00      	nop
   398bc:	20006000 	.word	0x20006000

000398c0 <_malloc_r>:
   398c0:	f101 030b 	add.w	r3, r1, #11
   398c4:	2b16      	cmp	r3, #22
   398c6:	e92d 4ff7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   398ca:	4605      	mov	r5, r0
   398cc:	d906      	bls.n	398dc <_malloc_r+0x1c>
   398ce:	f033 0707 	bics.w	r7, r3, #7
   398d2:	d504      	bpl.n	398de <_malloc_r+0x1e>
   398d4:	230c      	movs	r3, #12
   398d6:	602b      	str	r3, [r5, #0]
   398d8:	2400      	movs	r4, #0
   398da:	e19b      	b.n	39c14 <_malloc_r+0x354>
   398dc:	2710      	movs	r7, #16
   398de:	42b9      	cmp	r1, r7
   398e0:	d8f8      	bhi.n	398d4 <_malloc_r+0x14>
   398e2:	4628      	mov	r0, r5
   398e4:	4eb1      	ldr	r6, [pc, #708]	; (39bac <_malloc_r+0x2ec>)
   398e6:	f000 fbbb 	bl	3a060 <__malloc_lock>
   398ea:	f5b7 7ffc 	cmp.w	r7, #504	; 0x1f8
   398ee:	d236      	bcs.n	3995e <_malloc_r+0x9e>
   398f0:	f107 0208 	add.w	r2, r7, #8
   398f4:	08fb      	lsrs	r3, r7, #3
   398f6:	4432      	add	r2, r6
   398f8:	6854      	ldr	r4, [r2, #4]
   398fa:	f1a2 0108 	sub.w	r1, r2, #8
   398fe:	428c      	cmp	r4, r1
   39900:	d102      	bne.n	39908 <_malloc_r+0x48>
   39902:	68d4      	ldr	r4, [r2, #12]
   39904:	42a2      	cmp	r2, r4
   39906:	d010      	beq.n	3992a <_malloc_r+0x6a>
   39908:	6863      	ldr	r3, [r4, #4]
   3990a:	f023 0303 	bic.w	r3, r3, #3
   3990e:	e9d4 1202 	ldrd	r1, r2, [r4, #8]
   39912:	4423      	add	r3, r4
   39914:	60ca      	str	r2, [r1, #12]
   39916:	6091      	str	r1, [r2, #8]
   39918:	685a      	ldr	r2, [r3, #4]
   3991a:	f042 0201 	orr.w	r2, r2, #1
   3991e:	605a      	str	r2, [r3, #4]
   39920:	4628      	mov	r0, r5
   39922:	3408      	adds	r4, #8
   39924:	f000 fba2 	bl	3a06c <__malloc_unlock>
   39928:	e174      	b.n	39c14 <_malloc_r+0x354>
   3992a:	3302      	adds	r3, #2
   3992c:	6934      	ldr	r4, [r6, #16]
   3992e:	49a0      	ldr	r1, [pc, #640]	; (39bb0 <_malloc_r+0x2f0>)
   39930:	428c      	cmp	r4, r1
   39932:	d077      	beq.n	39a24 <_malloc_r+0x164>
   39934:	6862      	ldr	r2, [r4, #4]
   39936:	f022 0c03 	bic.w	ip, r2, #3
   3993a:	ebac 0007 	sub.w	r0, ip, r7
   3993e:	280f      	cmp	r0, #15
   39940:	dd48      	ble.n	399d4 <_malloc_r+0x114>
   39942:	19e2      	adds	r2, r4, r7
   39944:	f040 0301 	orr.w	r3, r0, #1
   39948:	f047 0701 	orr.w	r7, r7, #1
   3994c:	6067      	str	r7, [r4, #4]
   3994e:	e9c6 2204 	strd	r2, r2, [r6, #16]
   39952:	e9c2 1102 	strd	r1, r1, [r2, #8]
   39956:	6053      	str	r3, [r2, #4]
   39958:	f844 000c 	str.w	r0, [r4, ip]
   3995c:	e7e0      	b.n	39920 <_malloc_r+0x60>
   3995e:	0a7b      	lsrs	r3, r7, #9
   39960:	d02a      	beq.n	399b8 <_malloc_r+0xf8>
   39962:	2b04      	cmp	r3, #4
   39964:	d812      	bhi.n	3998c <_malloc_r+0xcc>
   39966:	09bb      	lsrs	r3, r7, #6
   39968:	3338      	adds	r3, #56	; 0x38
   3996a:	1c5a      	adds	r2, r3, #1
   3996c:	eb06 02c2 	add.w	r2, r6, r2, lsl #3
   39970:	f1a2 0c08 	sub.w	ip, r2, #8
   39974:	6854      	ldr	r4, [r2, #4]
   39976:	4564      	cmp	r4, ip
   39978:	d006      	beq.n	39988 <_malloc_r+0xc8>
   3997a:	6862      	ldr	r2, [r4, #4]
   3997c:	f022 0203 	bic.w	r2, r2, #3
   39980:	1bd0      	subs	r0, r2, r7
   39982:	280f      	cmp	r0, #15
   39984:	dd1c      	ble.n	399c0 <_malloc_r+0x100>
   39986:	3b01      	subs	r3, #1
   39988:	3301      	adds	r3, #1
   3998a:	e7cf      	b.n	3992c <_malloc_r+0x6c>
   3998c:	2b14      	cmp	r3, #20
   3998e:	d801      	bhi.n	39994 <_malloc_r+0xd4>
   39990:	335b      	adds	r3, #91	; 0x5b
   39992:	e7ea      	b.n	3996a <_malloc_r+0xaa>
   39994:	2b54      	cmp	r3, #84	; 0x54
   39996:	d802      	bhi.n	3999e <_malloc_r+0xde>
   39998:	0b3b      	lsrs	r3, r7, #12
   3999a:	336e      	adds	r3, #110	; 0x6e
   3999c:	e7e5      	b.n	3996a <_malloc_r+0xaa>
   3999e:	f5b3 7faa 	cmp.w	r3, #340	; 0x154
   399a2:	d802      	bhi.n	399aa <_malloc_r+0xea>
   399a4:	0bfb      	lsrs	r3, r7, #15
   399a6:	3377      	adds	r3, #119	; 0x77
   399a8:	e7df      	b.n	3996a <_malloc_r+0xaa>
   399aa:	f240 5254 	movw	r2, #1364	; 0x554
   399ae:	4293      	cmp	r3, r2
   399b0:	d804      	bhi.n	399bc <_malloc_r+0xfc>
   399b2:	0cbb      	lsrs	r3, r7, #18
   399b4:	337c      	adds	r3, #124	; 0x7c
   399b6:	e7d8      	b.n	3996a <_malloc_r+0xaa>
   399b8:	233f      	movs	r3, #63	; 0x3f
   399ba:	e7d6      	b.n	3996a <_malloc_r+0xaa>
   399bc:	237e      	movs	r3, #126	; 0x7e
   399be:	e7d4      	b.n	3996a <_malloc_r+0xaa>
   399c0:	2800      	cmp	r0, #0
   399c2:	68e1      	ldr	r1, [r4, #12]
   399c4:	db04      	blt.n	399d0 <_malloc_r+0x110>
   399c6:	68a3      	ldr	r3, [r4, #8]
   399c8:	60d9      	str	r1, [r3, #12]
   399ca:	608b      	str	r3, [r1, #8]
   399cc:	18a3      	adds	r3, r4, r2
   399ce:	e7a3      	b.n	39918 <_malloc_r+0x58>
   399d0:	460c      	mov	r4, r1
   399d2:	e7d0      	b.n	39976 <_malloc_r+0xb6>
   399d4:	2800      	cmp	r0, #0
   399d6:	e9c6 1104 	strd	r1, r1, [r6, #16]
   399da:	db07      	blt.n	399ec <_malloc_r+0x12c>
   399dc:	44a4      	add	ip, r4
   399de:	f8dc 3004 	ldr.w	r3, [ip, #4]
   399e2:	f043 0301 	orr.w	r3, r3, #1
   399e6:	f8cc 3004 	str.w	r3, [ip, #4]
   399ea:	e799      	b.n	39920 <_malloc_r+0x60>
   399ec:	f5bc 7f00 	cmp.w	ip, #512	; 0x200
   399f0:	6870      	ldr	r0, [r6, #4]
   399f2:	f080 8096 	bcs.w	39b22 <_malloc_r+0x262>
   399f6:	ea4f 02dc 	mov.w	r2, ip, lsr #3
   399fa:	ea4f 1e5c 	mov.w	lr, ip, lsr #5
   399fe:	f04f 0c01 	mov.w	ip, #1
   39a02:	3201      	adds	r2, #1
   39a04:	fa0c fc0e 	lsl.w	ip, ip, lr
   39a08:	ea4c 0000 	orr.w	r0, ip, r0
   39a0c:	f856 c032 	ldr.w	ip, [r6, r2, lsl #3]
   39a10:	6070      	str	r0, [r6, #4]
   39a12:	eb06 00c2 	add.w	r0, r6, r2, lsl #3
   39a16:	3808      	subs	r0, #8
   39a18:	e9c4 c002 	strd	ip, r0, [r4, #8]
   39a1c:	f846 4032 	str.w	r4, [r6, r2, lsl #3]
   39a20:	f8cc 400c 	str.w	r4, [ip, #12]
   39a24:	2001      	movs	r0, #1
   39a26:	109a      	asrs	r2, r3, #2
   39a28:	fa00 f202 	lsl.w	r2, r0, r2
   39a2c:	6870      	ldr	r0, [r6, #4]
   39a2e:	4290      	cmp	r0, r2
   39a30:	d325      	bcc.n	39a7e <_malloc_r+0x1be>
   39a32:	4210      	tst	r0, r2
   39a34:	d105      	bne.n	39a42 <_malloc_r+0x182>
   39a36:	f023 0303 	bic.w	r3, r3, #3
   39a3a:	0052      	lsls	r2, r2, #1
   39a3c:	3304      	adds	r3, #4
   39a3e:	4210      	tst	r0, r2
   39a40:	d0fb      	beq.n	39a3a <_malloc_r+0x17a>
   39a42:	eb06 08c3 	add.w	r8, r6, r3, lsl #3
   39a46:	469e      	mov	lr, r3
   39a48:	46c1      	mov	r9, r8
   39a4a:	f8d9 400c 	ldr.w	r4, [r9, #12]
   39a4e:	454c      	cmp	r4, r9
   39a50:	f040 80b2 	bne.w	39bb8 <_malloc_r+0x2f8>
   39a54:	f10e 0e01 	add.w	lr, lr, #1
   39a58:	f109 0908 	add.w	r9, r9, #8
   39a5c:	f01e 0f03 	tst.w	lr, #3
   39a60:	d1f3      	bne.n	39a4a <_malloc_r+0x18a>
   39a62:	0798      	lsls	r0, r3, #30
   39a64:	f040 80dc 	bne.w	39c20 <_malloc_r+0x360>
   39a68:	6873      	ldr	r3, [r6, #4]
   39a6a:	ea23 0302 	bic.w	r3, r3, r2
   39a6e:	6073      	str	r3, [r6, #4]
   39a70:	0052      	lsls	r2, r2, #1
   39a72:	6870      	ldr	r0, [r6, #4]
   39a74:	4290      	cmp	r0, r2
   39a76:	d302      	bcc.n	39a7e <_malloc_r+0x1be>
   39a78:	2a00      	cmp	r2, #0
   39a7a:	f040 80dd 	bne.w	39c38 <_malloc_r+0x378>
   39a7e:	f8d6 b008 	ldr.w	fp, [r6, #8]
   39a82:	f8db 3004 	ldr.w	r3, [fp, #4]
   39a86:	f023 0903 	bic.w	r9, r3, #3
   39a8a:	45b9      	cmp	r9, r7
   39a8c:	d304      	bcc.n	39a98 <_malloc_r+0x1d8>
   39a8e:	eba9 0207 	sub.w	r2, r9, r7
   39a92:	2a0f      	cmp	r2, #15
   39a94:	f300 813a 	bgt.w	39d0c <_malloc_r+0x44c>
   39a98:	f8df a118 	ldr.w	sl, [pc, #280]	; 39bb4 <_malloc_r+0x2f4>
   39a9c:	eb0b 0309 	add.w	r3, fp, r9
   39aa0:	f8d6 2408 	ldr.w	r2, [r6, #1032]	; 0x408
   39aa4:	4628      	mov	r0, r5
   39aa6:	f8da 4000 	ldr.w	r4, [sl]
   39aaa:	3201      	adds	r2, #1
   39aac:	9300      	str	r3, [sp, #0]
   39aae:	443c      	add	r4, r7
   39ab0:	bf17      	itett	ne
   39ab2:	f504 5480 	addne.w	r4, r4, #4096	; 0x1000
   39ab6:	3410      	addeq	r4, #16
   39ab8:	340f      	addne	r4, #15
   39aba:	f424 647f 	bicne.w	r4, r4, #4080	; 0xff0
   39abe:	bf18      	it	ne
   39ac0:	f024 040f 	bicne.w	r4, r4, #15
   39ac4:	4621      	mov	r1, r4
   39ac6:	f001 f97f 	bl	3adc8 <_sbrk_r>
   39aca:	f1b0 3fff 	cmp.w	r0, #4294967295	; 0xffffffff
   39ace:	4680      	mov	r8, r0
   39ad0:	f000 80f0 	beq.w	39cb4 <_malloc_r+0x3f4>
   39ad4:	9b00      	ldr	r3, [sp, #0]
   39ad6:	4283      	cmp	r3, r0
   39ad8:	d902      	bls.n	39ae0 <_malloc_r+0x220>
   39ada:	45b3      	cmp	fp, r6
   39adc:	f040 80ea 	bne.w	39cb4 <_malloc_r+0x3f4>
   39ae0:	f8da 1004 	ldr.w	r1, [sl, #4]
   39ae4:	4543      	cmp	r3, r8
   39ae6:	4421      	add	r1, r4
   39ae8:	f8ca 1004 	str.w	r1, [sl, #4]
   39aec:	f040 80a6 	bne.w	39c3c <_malloc_r+0x37c>
   39af0:	f3c3 020b 	ubfx	r2, r3, #0, #12
   39af4:	2a00      	cmp	r2, #0
   39af6:	f040 80a1 	bne.w	39c3c <_malloc_r+0x37c>
   39afa:	444c      	add	r4, r9
   39afc:	68b3      	ldr	r3, [r6, #8]
   39afe:	f044 0401 	orr.w	r4, r4, #1
   39b02:	605c      	str	r4, [r3, #4]
   39b04:	f8da 3004 	ldr.w	r3, [sl, #4]
   39b08:	f8da 202c 	ldr.w	r2, [sl, #44]	; 0x2c
   39b0c:	4293      	cmp	r3, r2
   39b0e:	f8da 2030 	ldr.w	r2, [sl, #48]	; 0x30
   39b12:	bf88      	it	hi
   39b14:	f8ca 302c 	strhi.w	r3, [sl, #44]	; 0x2c
   39b18:	4293      	cmp	r3, r2
   39b1a:	bf88      	it	hi
   39b1c:	f8ca 3030 	strhi.w	r3, [sl, #48]	; 0x30
   39b20:	e0c8      	b.n	39cb4 <_malloc_r+0x3f4>
   39b22:	f5bc 6f20 	cmp.w	ip, #2560	; 0xa00
   39b26:	ea4f 225c 	mov.w	r2, ip, lsr #9
   39b2a:	d218      	bcs.n	39b5e <_malloc_r+0x29e>
   39b2c:	ea4f 129c 	mov.w	r2, ip, lsr #6
   39b30:	3238      	adds	r2, #56	; 0x38
   39b32:	f102 0e01 	add.w	lr, r2, #1
   39b36:	eb06 08c2 	add.w	r8, r6, r2, lsl #3
   39b3a:	f856 e03e 	ldr.w	lr, [r6, lr, lsl #3]
   39b3e:	45f0      	cmp	r8, lr
   39b40:	d12b      	bne.n	39b9a <_malloc_r+0x2da>
   39b42:	1092      	asrs	r2, r2, #2
   39b44:	f04f 0c01 	mov.w	ip, #1
   39b48:	fa0c f202 	lsl.w	r2, ip, r2
   39b4c:	4310      	orrs	r0, r2
   39b4e:	6070      	str	r0, [r6, #4]
   39b50:	e9c4 e802 	strd	lr, r8, [r4, #8]
   39b54:	f8c8 4008 	str.w	r4, [r8, #8]
   39b58:	f8ce 400c 	str.w	r4, [lr, #12]
   39b5c:	e762      	b.n	39a24 <_malloc_r+0x164>
   39b5e:	2a14      	cmp	r2, #20
   39b60:	d801      	bhi.n	39b66 <_malloc_r+0x2a6>
   39b62:	325b      	adds	r2, #91	; 0x5b
   39b64:	e7e5      	b.n	39b32 <_malloc_r+0x272>
   39b66:	2a54      	cmp	r2, #84	; 0x54
   39b68:	d803      	bhi.n	39b72 <_malloc_r+0x2b2>
   39b6a:	ea4f 321c 	mov.w	r2, ip, lsr #12
   39b6e:	326e      	adds	r2, #110	; 0x6e
   39b70:	e7df      	b.n	39b32 <_malloc_r+0x272>
   39b72:	f5b2 7faa 	cmp.w	r2, #340	; 0x154
   39b76:	d803      	bhi.n	39b80 <_malloc_r+0x2c0>
   39b78:	ea4f 32dc 	mov.w	r2, ip, lsr #15
   39b7c:	3277      	adds	r2, #119	; 0x77
   39b7e:	e7d8      	b.n	39b32 <_malloc_r+0x272>
   39b80:	f240 5e54 	movw	lr, #1364	; 0x554
   39b84:	4572      	cmp	r2, lr
   39b86:	bf96      	itet	ls
   39b88:	ea4f 429c 	movls.w	r2, ip, lsr #18
   39b8c:	227e      	movhi	r2, #126	; 0x7e
   39b8e:	327c      	addls	r2, #124	; 0x7c
   39b90:	e7cf      	b.n	39b32 <_malloc_r+0x272>
   39b92:	f8de e008 	ldr.w	lr, [lr, #8]
   39b96:	45f0      	cmp	r8, lr
   39b98:	d005      	beq.n	39ba6 <_malloc_r+0x2e6>
   39b9a:	f8de 2004 	ldr.w	r2, [lr, #4]
   39b9e:	f022 0203 	bic.w	r2, r2, #3
   39ba2:	4562      	cmp	r2, ip
   39ba4:	d8f5      	bhi.n	39b92 <_malloc_r+0x2d2>
   39ba6:	f8de 800c 	ldr.w	r8, [lr, #12]
   39baa:	e7d1      	b.n	39b50 <_malloc_r+0x290>
   39bac:	2000659c 	.word	0x2000659c
   39bb0:	200065a4 	.word	0x200065a4
   39bb4:	200069d8 	.word	0x200069d8
   39bb8:	6860      	ldr	r0, [r4, #4]
   39bba:	f8d4 c00c 	ldr.w	ip, [r4, #12]
   39bbe:	f020 0003 	bic.w	r0, r0, #3
   39bc2:	eba0 0a07 	sub.w	sl, r0, r7
   39bc6:	f1ba 0f0f 	cmp.w	sl, #15
   39bca:	dd12      	ble.n	39bf2 <_malloc_r+0x332>
   39bcc:	68a3      	ldr	r3, [r4, #8]
   39bce:	19e2      	adds	r2, r4, r7
   39bd0:	f047 0701 	orr.w	r7, r7, #1
   39bd4:	6067      	str	r7, [r4, #4]
   39bd6:	f8c3 c00c 	str.w	ip, [r3, #12]
   39bda:	f8cc 3008 	str.w	r3, [ip, #8]
   39bde:	f04a 0301 	orr.w	r3, sl, #1
   39be2:	e9c6 2204 	strd	r2, r2, [r6, #16]
   39be6:	e9c2 1102 	strd	r1, r1, [r2, #8]
   39bea:	6053      	str	r3, [r2, #4]
   39bec:	f844 a000 	str.w	sl, [r4, r0]
   39bf0:	e696      	b.n	39920 <_malloc_r+0x60>
   39bf2:	f1ba 0f00 	cmp.w	sl, #0
   39bf6:	db11      	blt.n	39c1c <_malloc_r+0x35c>
   39bf8:	4420      	add	r0, r4
   39bfa:	6843      	ldr	r3, [r0, #4]
   39bfc:	f043 0301 	orr.w	r3, r3, #1
   39c00:	6043      	str	r3, [r0, #4]
   39c02:	4628      	mov	r0, r5
   39c04:	f854 3f08 	ldr.w	r3, [r4, #8]!
   39c08:	f8c3 c00c 	str.w	ip, [r3, #12]
   39c0c:	f8cc 3008 	str.w	r3, [ip, #8]
   39c10:	f000 fa2c 	bl	3a06c <__malloc_unlock>
   39c14:	4620      	mov	r0, r4
   39c16:	b003      	add	sp, #12
   39c18:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   39c1c:	4664      	mov	r4, ip
   39c1e:	e716      	b.n	39a4e <_malloc_r+0x18e>
   39c20:	f858 0908 	ldr.w	r0, [r8], #-8
   39c24:	3b01      	subs	r3, #1
   39c26:	4540      	cmp	r0, r8
   39c28:	f43f af1b 	beq.w	39a62 <_malloc_r+0x1a2>
   39c2c:	e720      	b.n	39a70 <_malloc_r+0x1b0>
   39c2e:	3304      	adds	r3, #4
   39c30:	0052      	lsls	r2, r2, #1
   39c32:	4210      	tst	r0, r2
   39c34:	d0fb      	beq.n	39c2e <_malloc_r+0x36e>
   39c36:	e704      	b.n	39a42 <_malloc_r+0x182>
   39c38:	4673      	mov	r3, lr
   39c3a:	e7fa      	b.n	39c32 <_malloc_r+0x372>
   39c3c:	f8d6 2408 	ldr.w	r2, [r6, #1032]	; 0x408
   39c40:	4628      	mov	r0, r5
   39c42:	3201      	adds	r2, #1
   39c44:	bf17      	itett	ne
   39c46:	eba8 0303 	subne.w	r3, r8, r3
   39c4a:	f8c6 8408 	streq.w	r8, [r6, #1032]	; 0x408
   39c4e:	185b      	addne	r3, r3, r1
   39c50:	f8ca 3004 	strne.w	r3, [sl, #4]
   39c54:	f018 0307 	ands.w	r3, r8, #7
   39c58:	bf14      	ite	ne
   39c5a:	f1c3 0108 	rsbne	r1, r3, #8
   39c5e:	4619      	moveq	r1, r3
   39c60:	9301      	str	r3, [sp, #4]
   39c62:	bf18      	it	ne
   39c64:	4488      	addne	r8, r1
   39c66:	4444      	add	r4, r8
   39c68:	1b09      	subs	r1, r1, r4
   39c6a:	f3c1 010b 	ubfx	r1, r1, #0, #12
   39c6e:	9100      	str	r1, [sp, #0]
   39c70:	f001 f8aa 	bl	3adc8 <_sbrk_r>
   39c74:	1c43      	adds	r3, r0, #1
   39c76:	e9dd 1300 	ldrd	r1, r3, [sp]
   39c7a:	d104      	bne.n	39c86 <_malloc_r+0x3c6>
   39c7c:	b343      	cbz	r3, 39cd0 <_malloc_r+0x410>
   39c7e:	f1a3 0008 	sub.w	r0, r3, #8
   39c82:	2100      	movs	r1, #0
   39c84:	4420      	add	r0, r4
   39c86:	eba0 0008 	sub.w	r0, r0, r8
   39c8a:	f8da 3004 	ldr.w	r3, [sl, #4]
   39c8e:	45b3      	cmp	fp, r6
   39c90:	f8c6 8008 	str.w	r8, [r6, #8]
   39c94:	4408      	add	r0, r1
   39c96:	440b      	add	r3, r1
   39c98:	f040 0001 	orr.w	r0, r0, #1
   39c9c:	f8ca 3004 	str.w	r3, [sl, #4]
   39ca0:	f8c8 0004 	str.w	r0, [r8, #4]
   39ca4:	f43f af2e 	beq.w	39b04 <_malloc_r+0x244>
   39ca8:	f1b9 0f0f 	cmp.w	r9, #15
   39cac:	d813      	bhi.n	39cd6 <_malloc_r+0x416>
   39cae:	2301      	movs	r3, #1
   39cb0:	f8c8 3004 	str.w	r3, [r8, #4]
   39cb4:	68b3      	ldr	r3, [r6, #8]
   39cb6:	685b      	ldr	r3, [r3, #4]
   39cb8:	f023 0303 	bic.w	r3, r3, #3
   39cbc:	42bb      	cmp	r3, r7
   39cbe:	eba3 0207 	sub.w	r2, r3, r7
   39cc2:	d301      	bcc.n	39cc8 <_malloc_r+0x408>
   39cc4:	2a0f      	cmp	r2, #15
   39cc6:	dc21      	bgt.n	39d0c <_malloc_r+0x44c>
   39cc8:	4628      	mov	r0, r5
   39cca:	f000 f9cf 	bl	3a06c <__malloc_unlock>
   39cce:	e603      	b.n	398d8 <_malloc_r+0x18>
   39cd0:	4620      	mov	r0, r4
   39cd2:	4619      	mov	r1, r3
   39cd4:	e7d7      	b.n	39c86 <_malloc_r+0x3c6>
   39cd6:	f8db 3004 	ldr.w	r3, [fp, #4]
   39cda:	f1a9 090c 	sub.w	r9, r9, #12
   39cde:	2205      	movs	r2, #5
   39ce0:	f029 0907 	bic.w	r9, r9, #7
   39ce4:	f003 0301 	and.w	r3, r3, #1
   39ce8:	ea43 0309 	orr.w	r3, r3, r9
   39cec:	f1b9 0f0f 	cmp.w	r9, #15
   39cf0:	f8cb 3004 	str.w	r3, [fp, #4]
   39cf4:	eb0b 0309 	add.w	r3, fp, r9
   39cf8:	e9c3 2201 	strd	r2, r2, [r3, #4]
   39cfc:	f67f af02 	bls.w	39b04 <_malloc_r+0x244>
   39d00:	f10b 0108 	add.w	r1, fp, #8
   39d04:	4628      	mov	r0, r5
   39d06:	f7ff fac7 	bl	39298 <_free_r>
   39d0a:	e6fb      	b.n	39b04 <_malloc_r+0x244>
   39d0c:	68b4      	ldr	r4, [r6, #8]
   39d0e:	f047 0301 	orr.w	r3, r7, #1
   39d12:	f042 0201 	orr.w	r2, r2, #1
   39d16:	4427      	add	r7, r4
   39d18:	6063      	str	r3, [r4, #4]
   39d1a:	60b7      	str	r7, [r6, #8]
   39d1c:	607a      	str	r2, [r7, #4]
   39d1e:	e5ff      	b.n	39920 <_malloc_r+0x60>

00039d20 <_mbrtowc_r>:
   39d20:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
   39d22:	9e08      	ldr	r6, [sp, #32]
   39d24:	4605      	mov	r5, r0
   39d26:	4c09      	ldr	r4, [pc, #36]	; (39d4c <_mbrtowc_r+0x2c>)
   39d28:	f8d4 70e4 	ldr.w	r7, [r4, #228]	; 0xe4
   39d2c:	4614      	mov	r4, r2
   39d2e:	9600      	str	r6, [sp, #0]
   39d30:	b912      	cbnz	r2, 39d38 <_mbrtowc_r+0x18>
   39d32:	2301      	movs	r3, #1
   39d34:	4a06      	ldr	r2, [pc, #24]	; (39d50 <_mbrtowc_r+0x30>)
   39d36:	4621      	mov	r1, r4
   39d38:	47b8      	blx	r7
   39d3a:	1c43      	adds	r3, r0, #1
   39d3c:	bf01      	itttt	eq
   39d3e:	2300      	moveq	r3, #0
   39d40:	6033      	streq	r3, [r6, #0]
   39d42:	238a      	moveq	r3, #138	; 0x8a
   39d44:	602b      	streq	r3, [r5, #0]
   39d46:	b003      	add	sp, #12
   39d48:	bdf0      	pop	{r4, r5, r6, r7, pc}
   39d4a:	bf00      	nop
   39d4c:	20006430 	.word	0x20006430
   39d50:	0004099d 	.word	0x0004099d

00039d54 <mbrtowc>:
   39d54:	b507      	push	{r0, r1, r2, lr}
   39d56:	9300      	str	r3, [sp, #0]
   39d58:	4613      	mov	r3, r2
   39d5a:	460a      	mov	r2, r1
   39d5c:	4601      	mov	r1, r0
   39d5e:	4803      	ldr	r0, [pc, #12]	; (39d6c <mbrtowc+0x18>)
   39d60:	6800      	ldr	r0, [r0, #0]
   39d62:	f7ff ffdd 	bl	39d20 <_mbrtowc_r>
   39d66:	b003      	add	sp, #12
   39d68:	f85d fb04 	ldr.w	pc, [sp], #4
   39d6c:	20006000 	.word	0x20006000

00039d70 <_mbtowc_r>:
   39d70:	b410      	push	{r4}
   39d72:	4c03      	ldr	r4, [pc, #12]	; (39d80 <_mbtowc_r+0x10>)
   39d74:	f8d4 40e4 	ldr.w	r4, [r4, #228]	; 0xe4
   39d78:	46a4      	mov	ip, r4
   39d7a:	f85d 4b04 	ldr.w	r4, [sp], #4
   39d7e:	4760      	bx	ip
   39d80:	20006430 	.word	0x20006430

00039d84 <__ascii_mbtowc>:
   39d84:	b082      	sub	sp, #8
   39d86:	b901      	cbnz	r1, 39d8a <__ascii_mbtowc+0x6>
   39d88:	a901      	add	r1, sp, #4
   39d8a:	b142      	cbz	r2, 39d9e <__ascii_mbtowc+0x1a>
   39d8c:	b14b      	cbz	r3, 39da2 <__ascii_mbtowc+0x1e>
   39d8e:	7813      	ldrb	r3, [r2, #0]
   39d90:	600b      	str	r3, [r1, #0]
   39d92:	7812      	ldrb	r2, [r2, #0]
   39d94:	1e10      	subs	r0, r2, #0
   39d96:	bf18      	it	ne
   39d98:	2001      	movne	r0, #1
   39d9a:	b002      	add	sp, #8
   39d9c:	4770      	bx	lr
   39d9e:	4610      	mov	r0, r2
   39da0:	e7fb      	b.n	39d9a <__ascii_mbtowc+0x16>
   39da2:	f06f 0001 	mvn.w	r0, #1
   39da6:	e7f8      	b.n	39d9a <__ascii_mbtowc+0x16>

00039da8 <memchr>:
   39da8:	b2c9      	uxtb	r1, r1
   39daa:	4603      	mov	r3, r0
   39dac:	b570      	push	{r4, r5, r6, lr}
   39dae:	079c      	lsls	r4, r3, #30
   39db0:	4618      	mov	r0, r3
   39db2:	d108      	bne.n	39dc6 <memchr+0x1e>
   39db4:	2a03      	cmp	r2, #3
   39db6:	d80d      	bhi.n	39dd4 <memchr+0x2c>
   39db8:	4603      	mov	r3, r0
   39dba:	4402      	add	r2, r0
   39dbc:	4293      	cmp	r3, r2
   39dbe:	4618      	mov	r0, r3
   39dc0:	d11c      	bne.n	39dfc <memchr+0x54>
   39dc2:	2000      	movs	r0, #0
   39dc4:	e005      	b.n	39dd2 <memchr+0x2a>
   39dc6:	b1f2      	cbz	r2, 39e06 <memchr+0x5e>
   39dc8:	7804      	ldrb	r4, [r0, #0]
   39dca:	3301      	adds	r3, #1
   39dcc:	3a01      	subs	r2, #1
   39dce:	428c      	cmp	r4, r1
   39dd0:	d1ed      	bne.n	39dae <memchr+0x6>
   39dd2:	bd70      	pop	{r4, r5, r6, pc}
   39dd4:	ea41 2001 	orr.w	r0, r1, r1, lsl #8
   39dd8:	ea40 4400 	orr.w	r4, r0, r0, lsl #16
   39ddc:	4618      	mov	r0, r3
   39dde:	3304      	adds	r3, #4
   39de0:	6805      	ldr	r5, [r0, #0]
   39de2:	4065      	eors	r5, r4
   39de4:	f1a5 3601 	sub.w	r6, r5, #16843009	; 0x1010101
   39de8:	ea26 0505 	bic.w	r5, r6, r5
   39dec:	f015 3f80 	tst.w	r5, #2155905152	; 0x80808080
   39df0:	d1e2      	bne.n	39db8 <memchr+0x10>
   39df2:	3a04      	subs	r2, #4
   39df4:	4618      	mov	r0, r3
   39df6:	2a03      	cmp	r2, #3
   39df8:	d8f0      	bhi.n	39ddc <memchr+0x34>
   39dfa:	e7dd      	b.n	39db8 <memchr+0x10>
   39dfc:	7804      	ldrb	r4, [r0, #0]
   39dfe:	3301      	adds	r3, #1
   39e00:	428c      	cmp	r4, r1
   39e02:	d1db      	bne.n	39dbc <memchr+0x14>
   39e04:	e7e5      	b.n	39dd2 <memchr+0x2a>
   39e06:	4610      	mov	r0, r2
   39e08:	e7e3      	b.n	39dd2 <memchr+0x2a>

00039e0a <memcmp>:
   39e0a:	2a03      	cmp	r2, #3
   39e0c:	b570      	push	{r4, r5, r6, lr}
   39e0e:	d905      	bls.n	39e1c <memcmp+0x12>
   39e10:	ea40 0501 	orr.w	r5, r0, r1
   39e14:	4604      	mov	r4, r0
   39e16:	460b      	mov	r3, r1
   39e18:	07ad      	lsls	r5, r5, #30
   39e1a:	d005      	beq.n	39e28 <memcmp+0x1e>
   39e1c:	3901      	subs	r1, #1
   39e1e:	4402      	add	r2, r0
   39e20:	4290      	cmp	r0, r2
   39e22:	d10f      	bne.n	39e44 <memcmp+0x3a>
   39e24:	2000      	movs	r0, #0
   39e26:	e013      	b.n	39e50 <memcmp+0x46>
   39e28:	4619      	mov	r1, r3
   39e2a:	4620      	mov	r0, r4
   39e2c:	3304      	adds	r3, #4
   39e2e:	3404      	adds	r4, #4
   39e30:	6806      	ldr	r6, [r0, #0]
   39e32:	680d      	ldr	r5, [r1, #0]
   39e34:	42ae      	cmp	r6, r5
   39e36:	d1f1      	bne.n	39e1c <memcmp+0x12>
   39e38:	3a04      	subs	r2, #4
   39e3a:	4620      	mov	r0, r4
   39e3c:	4619      	mov	r1, r3
   39e3e:	2a03      	cmp	r2, #3
   39e40:	d8f2      	bhi.n	39e28 <memcmp+0x1e>
   39e42:	e7eb      	b.n	39e1c <memcmp+0x12>
   39e44:	7803      	ldrb	r3, [r0, #0]
   39e46:	f811 4f01 	ldrb.w	r4, [r1, #1]!
   39e4a:	42a3      	cmp	r3, r4
   39e4c:	d001      	beq.n	39e52 <memcmp+0x48>
   39e4e:	1b18      	subs	r0, r3, r4
   39e50:	bd70      	pop	{r4, r5, r6, pc}
   39e52:	3001      	adds	r0, #1
   39e54:	e7e4      	b.n	39e20 <memcmp+0x16>
	...

00039e58 <memcpy>:
   39e58:	4684      	mov	ip, r0
   39e5a:	ea41 0300 	orr.w	r3, r1, r0
   39e5e:	f013 0303 	ands.w	r3, r3, #3
   39e62:	d149      	bne.n	39ef8 <memcpy+0xa0>
   39e64:	3a40      	subs	r2, #64	; 0x40
   39e66:	d323      	bcc.n	39eb0 <memcpy+0x58>
   39e68:	680b      	ldr	r3, [r1, #0]
   39e6a:	6003      	str	r3, [r0, #0]
   39e6c:	684b      	ldr	r3, [r1, #4]
   39e6e:	6043      	str	r3, [r0, #4]
   39e70:	688b      	ldr	r3, [r1, #8]
   39e72:	6083      	str	r3, [r0, #8]
   39e74:	68cb      	ldr	r3, [r1, #12]
   39e76:	60c3      	str	r3, [r0, #12]
   39e78:	690b      	ldr	r3, [r1, #16]
   39e7a:	6103      	str	r3, [r0, #16]
   39e7c:	694b      	ldr	r3, [r1, #20]
   39e7e:	6143      	str	r3, [r0, #20]
   39e80:	698b      	ldr	r3, [r1, #24]
   39e82:	6183      	str	r3, [r0, #24]
   39e84:	69cb      	ldr	r3, [r1, #28]
   39e86:	61c3      	str	r3, [r0, #28]
   39e88:	6a0b      	ldr	r3, [r1, #32]
   39e8a:	6203      	str	r3, [r0, #32]
   39e8c:	6a4b      	ldr	r3, [r1, #36]	; 0x24
   39e8e:	6243      	str	r3, [r0, #36]	; 0x24
   39e90:	6a8b      	ldr	r3, [r1, #40]	; 0x28
   39e92:	6283      	str	r3, [r0, #40]	; 0x28
   39e94:	6acb      	ldr	r3, [r1, #44]	; 0x2c
   39e96:	62c3      	str	r3, [r0, #44]	; 0x2c
   39e98:	6b0b      	ldr	r3, [r1, #48]	; 0x30
   39e9a:	6303      	str	r3, [r0, #48]	; 0x30
   39e9c:	6b4b      	ldr	r3, [r1, #52]	; 0x34
   39e9e:	6343      	str	r3, [r0, #52]	; 0x34
   39ea0:	6b8b      	ldr	r3, [r1, #56]	; 0x38
   39ea2:	6383      	str	r3, [r0, #56]	; 0x38
   39ea4:	6bcb      	ldr	r3, [r1, #60]	; 0x3c
   39ea6:	63c3      	str	r3, [r0, #60]	; 0x3c
   39ea8:	3040      	adds	r0, #64	; 0x40
   39eaa:	3140      	adds	r1, #64	; 0x40
   39eac:	3a40      	subs	r2, #64	; 0x40
   39eae:	d2db      	bcs.n	39e68 <memcpy+0x10>
   39eb0:	3230      	adds	r2, #48	; 0x30
   39eb2:	d30b      	bcc.n	39ecc <memcpy+0x74>
   39eb4:	680b      	ldr	r3, [r1, #0]
   39eb6:	6003      	str	r3, [r0, #0]
   39eb8:	684b      	ldr	r3, [r1, #4]
   39eba:	6043      	str	r3, [r0, #4]
   39ebc:	688b      	ldr	r3, [r1, #8]
   39ebe:	6083      	str	r3, [r0, #8]
   39ec0:	68cb      	ldr	r3, [r1, #12]
   39ec2:	60c3      	str	r3, [r0, #12]
   39ec4:	3010      	adds	r0, #16
   39ec6:	3110      	adds	r1, #16
   39ec8:	3a10      	subs	r2, #16
   39eca:	d2f3      	bcs.n	39eb4 <memcpy+0x5c>
   39ecc:	320c      	adds	r2, #12
   39ece:	d305      	bcc.n	39edc <memcpy+0x84>
   39ed0:	f851 3b04 	ldr.w	r3, [r1], #4
   39ed4:	f840 3b04 	str.w	r3, [r0], #4
   39ed8:	3a04      	subs	r2, #4
   39eda:	d2f9      	bcs.n	39ed0 <memcpy+0x78>
   39edc:	3204      	adds	r2, #4
   39ede:	d008      	beq.n	39ef2 <memcpy+0x9a>
   39ee0:	07d2      	lsls	r2, r2, #31
   39ee2:	bf1c      	itt	ne
   39ee4:	f811 3b01 	ldrbne.w	r3, [r1], #1
   39ee8:	f800 3b01 	strbne.w	r3, [r0], #1
   39eec:	d301      	bcc.n	39ef2 <memcpy+0x9a>
   39eee:	880b      	ldrh	r3, [r1, #0]
   39ef0:	8003      	strh	r3, [r0, #0]
   39ef2:	4660      	mov	r0, ip
   39ef4:	4770      	bx	lr
   39ef6:	bf00      	nop
   39ef8:	2a08      	cmp	r2, #8
   39efa:	d313      	bcc.n	39f24 <memcpy+0xcc>
   39efc:	078b      	lsls	r3, r1, #30
   39efe:	d0b1      	beq.n	39e64 <memcpy+0xc>
   39f00:	f010 0303 	ands.w	r3, r0, #3
   39f04:	d0ae      	beq.n	39e64 <memcpy+0xc>
   39f06:	f1c3 0304 	rsb	r3, r3, #4
   39f0a:	1ad2      	subs	r2, r2, r3
   39f0c:	07db      	lsls	r3, r3, #31
   39f0e:	bf1c      	itt	ne
   39f10:	f811 3b01 	ldrbne.w	r3, [r1], #1
   39f14:	f800 3b01 	strbne.w	r3, [r0], #1
   39f18:	d3a4      	bcc.n	39e64 <memcpy+0xc>
   39f1a:	f831 3b02 	ldrh.w	r3, [r1], #2
   39f1e:	f820 3b02 	strh.w	r3, [r0], #2
   39f22:	e79f      	b.n	39e64 <memcpy+0xc>
   39f24:	3a04      	subs	r2, #4
   39f26:	d3d9      	bcc.n	39edc <memcpy+0x84>
   39f28:	3a01      	subs	r2, #1
   39f2a:	f811 3b01 	ldrb.w	r3, [r1], #1
   39f2e:	f800 3b01 	strb.w	r3, [r0], #1
   39f32:	d2f9      	bcs.n	39f28 <memcpy+0xd0>
   39f34:	780b      	ldrb	r3, [r1, #0]
   39f36:	7003      	strb	r3, [r0, #0]
   39f38:	784b      	ldrb	r3, [r1, #1]
   39f3a:	7043      	strb	r3, [r0, #1]
   39f3c:	788b      	ldrb	r3, [r1, #2]
   39f3e:	7083      	strb	r3, [r0, #2]
   39f40:	4660      	mov	r0, ip
   39f42:	4770      	bx	lr

00039f44 <memmove>:
   39f44:	4288      	cmp	r0, r1
   39f46:	b5f0      	push	{r4, r5, r6, r7, lr}
   39f48:	d90c      	bls.n	39f64 <memmove+0x20>
   39f4a:	188b      	adds	r3, r1, r2
   39f4c:	4283      	cmp	r3, r0
   39f4e:	d909      	bls.n	39f64 <memmove+0x20>
   39f50:	4402      	add	r2, r0
   39f52:	4601      	mov	r1, r0
   39f54:	428a      	cmp	r2, r1
   39f56:	d100      	bne.n	39f5a <memmove+0x16>
   39f58:	bdf0      	pop	{r4, r5, r6, r7, pc}
   39f5a:	f813 4d01 	ldrb.w	r4, [r3, #-1]!
   39f5e:	f802 4d01 	strb.w	r4, [r2, #-1]!
   39f62:	e7f7      	b.n	39f54 <memmove+0x10>
   39f64:	2a0f      	cmp	r2, #15
   39f66:	d903      	bls.n	39f70 <memmove+0x2c>
   39f68:	ea41 0300 	orr.w	r3, r1, r0
   39f6c:	079b      	lsls	r3, r3, #30
   39f6e:	d009      	beq.n	39f84 <memmove+0x40>
   39f70:	4603      	mov	r3, r0
   39f72:	3b01      	subs	r3, #1
   39f74:	440a      	add	r2, r1
   39f76:	4291      	cmp	r1, r2
   39f78:	d0ee      	beq.n	39f58 <memmove+0x14>
   39f7a:	f811 4b01 	ldrb.w	r4, [r1], #1
   39f7e:	f803 4f01 	strb.w	r4, [r3, #1]!
   39f82:	e7f8      	b.n	39f76 <memmove+0x32>
   39f84:	f101 0410 	add.w	r4, r1, #16
   39f88:	f100 0310 	add.w	r3, r0, #16
   39f8c:	4615      	mov	r5, r2
   39f8e:	f854 6c10 	ldr.w	r6, [r4, #-16]
   39f92:	3d10      	subs	r5, #16
   39f94:	3410      	adds	r4, #16
   39f96:	3310      	adds	r3, #16
   39f98:	f843 6c20 	str.w	r6, [r3, #-32]
   39f9c:	2d0f      	cmp	r5, #15
   39f9e:	f854 6c1c 	ldr.w	r6, [r4, #-28]
   39fa2:	f843 6c1c 	str.w	r6, [r3, #-28]
   39fa6:	f854 6c18 	ldr.w	r6, [r4, #-24]
   39faa:	f843 6c18 	str.w	r6, [r3, #-24]
   39fae:	f854 6c14 	ldr.w	r6, [r4, #-20]
   39fb2:	f843 6c14 	str.w	r6, [r3, #-20]
   39fb6:	d8ea      	bhi.n	39f8e <memmove+0x4a>
   39fb8:	f1a2 0310 	sub.w	r3, r2, #16
   39fbc:	f002 050c 	and.w	r5, r2, #12
   39fc0:	f002 070f 	and.w	r7, r2, #15
   39fc4:	f3c2 0c81 	ubfx	ip, r2, #2, #2
   39fc8:	f023 030f 	bic.w	r3, r3, #15
   39fcc:	3310      	adds	r3, #16
   39fce:	18cc      	adds	r4, r1, r3
   39fd0:	4403      	add	r3, r0
   39fd2:	1961      	adds	r1, r4, r5
   39fd4:	1f1e      	subs	r6, r3, #4
   39fd6:	428c      	cmp	r4, r1
   39fd8:	d105      	bne.n	39fe6 <memmove+0xa2>
   39fda:	f06f 0203 	mvn.w	r2, #3
   39fde:	442b      	add	r3, r5
   39fe0:	fb02 720c 	mla	r2, r2, ip, r7
   39fe4:	e7c5      	b.n	39f72 <memmove+0x2e>
   39fe6:	f854 2b04 	ldr.w	r2, [r4], #4
   39fea:	f846 2f04 	str.w	r2, [r6, #4]!
   39fee:	e7f2      	b.n	39fd6 <memmove+0x92>

00039ff0 <memset>:
   39ff0:	4603      	mov	r3, r0
   39ff2:	b570      	push	{r4, r5, r6, lr}
   39ff4:	079c      	lsls	r4, r3, #30
   39ff6:	461d      	mov	r5, r3
   39ff8:	d120      	bne.n	3a03c <memset+0x4c>
   39ffa:	2a03      	cmp	r2, #3
   39ffc:	d91a      	bls.n	3a034 <memset+0x44>
   39ffe:	b2cc      	uxtb	r4, r1
   3a000:	4616      	mov	r6, r2
   3a002:	ea44 2404 	orr.w	r4, r4, r4, lsl #8
   3a006:	ea44 4404 	orr.w	r4, r4, r4, lsl #16
   3a00a:	2e0f      	cmp	r6, #15
   3a00c:	f105 0510 	add.w	r5, r5, #16
   3a010:	d81a      	bhi.n	3a048 <memset+0x58>
   3a012:	0915      	lsrs	r5, r2, #4
   3a014:	f06f 060f 	mvn.w	r6, #15
   3a018:	fb06 2205 	mla	r2, r6, r5, r2
   3a01c:	eb03 1505 	add.w	r5, r3, r5, lsl #4
   3a020:	f022 0303 	bic.w	r3, r2, #3
   3a024:	0896      	lsrs	r6, r2, #2
   3a026:	442b      	add	r3, r5
   3a028:	429d      	cmp	r5, r3
   3a02a:	d113      	bne.n	3a054 <memset+0x64>
   3a02c:	f06f 0403 	mvn.w	r4, #3
   3a030:	fb04 2206 	mla	r2, r4, r6, r2
   3a034:	441a      	add	r2, r3
   3a036:	4293      	cmp	r3, r2
   3a038:	d10f      	bne.n	3a05a <memset+0x6a>
   3a03a:	bd70      	pop	{r4, r5, r6, pc}
   3a03c:	2a00      	cmp	r2, #0
   3a03e:	d0fc      	beq.n	3a03a <memset+0x4a>
   3a040:	3a01      	subs	r2, #1
   3a042:	f803 1b01 	strb.w	r1, [r3], #1
   3a046:	e7d5      	b.n	39ff4 <memset+0x4>
   3a048:	3e10      	subs	r6, #16
   3a04a:	e945 4404 	strd	r4, r4, [r5, #-16]
   3a04e:	e945 4402 	strd	r4, r4, [r5, #-8]
   3a052:	e7da      	b.n	3a00a <memset+0x1a>
   3a054:	f845 4b04 	str.w	r4, [r5], #4
   3a058:	e7e6      	b.n	3a028 <memset+0x38>
   3a05a:	f803 1b01 	strb.w	r1, [r3], #1
   3a05e:	e7ea      	b.n	3a036 <memset+0x46>

0003a060 <__malloc_lock>:
   3a060:	4801      	ldr	r0, [pc, #4]	; (3a068 <__malloc_lock+0x8>)
   3a062:	f7ff bba0 	b.w	397a6 <__retarget_lock_acquire_recursive>
   3a066:	bf00      	nop
   3a068:	200069d0 	.word	0x200069d0

0003a06c <__malloc_unlock>:
   3a06c:	4801      	ldr	r0, [pc, #4]	; (3a074 <__malloc_unlock+0x8>)
   3a06e:	f7ff bba0 	b.w	397b2 <__retarget_lock_release_recursive>
   3a072:	bf00      	nop
   3a074:	200069d0 	.word	0x200069d0

0003a078 <_Balloc>:
   3a078:	6cc3      	ldr	r3, [r0, #76]	; 0x4c
   3a07a:	b570      	push	{r4, r5, r6, lr}
   3a07c:	4605      	mov	r5, r0
   3a07e:	460c      	mov	r4, r1
   3a080:	b17b      	cbz	r3, 3a0a2 <_Balloc+0x2a>
   3a082:	6ceb      	ldr	r3, [r5, #76]	; 0x4c
   3a084:	f853 0024 	ldr.w	r0, [r3, r4, lsl #2]
   3a088:	b9a0      	cbnz	r0, 3a0b4 <_Balloc+0x3c>
   3a08a:	2101      	movs	r1, #1
   3a08c:	4628      	mov	r0, r5
   3a08e:	fa01 f604 	lsl.w	r6, r1, r4
   3a092:	1d72      	adds	r2, r6, #5
   3a094:	0092      	lsls	r2, r2, #2
   3a096:	f004 fbb7 	bl	3e808 <_calloc_r>
   3a09a:	b148      	cbz	r0, 3a0b0 <_Balloc+0x38>
   3a09c:	e9c0 4601 	strd	r4, r6, [r0, #4]
   3a0a0:	e00b      	b.n	3a0ba <_Balloc+0x42>
   3a0a2:	2221      	movs	r2, #33	; 0x21
   3a0a4:	2104      	movs	r1, #4
   3a0a6:	f004 fbaf 	bl	3e808 <_calloc_r>
   3a0aa:	64e8      	str	r0, [r5, #76]	; 0x4c
   3a0ac:	2800      	cmp	r0, #0
   3a0ae:	d1e8      	bne.n	3a082 <_Balloc+0xa>
   3a0b0:	2000      	movs	r0, #0
   3a0b2:	bd70      	pop	{r4, r5, r6, pc}
   3a0b4:	6802      	ldr	r2, [r0, #0]
   3a0b6:	f843 2024 	str.w	r2, [r3, r4, lsl #2]
   3a0ba:	2300      	movs	r3, #0
   3a0bc:	e9c0 3303 	strd	r3, r3, [r0, #12]
   3a0c0:	e7f7      	b.n	3a0b2 <_Balloc+0x3a>

0003a0c2 <_Bfree>:
   3a0c2:	b131      	cbz	r1, 3a0d2 <_Bfree+0x10>
   3a0c4:	6cc3      	ldr	r3, [r0, #76]	; 0x4c
   3a0c6:	684a      	ldr	r2, [r1, #4]
   3a0c8:	f853 0022 	ldr.w	r0, [r3, r2, lsl #2]
   3a0cc:	6008      	str	r0, [r1, #0]
   3a0ce:	f843 1022 	str.w	r1, [r3, r2, lsl #2]
   3a0d2:	4770      	bx	lr

0003a0d4 <__multadd>:
   3a0d4:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
   3a0d8:	f101 0c14 	add.w	ip, r1, #20
   3a0dc:	4607      	mov	r7, r0
   3a0de:	460c      	mov	r4, r1
   3a0e0:	461e      	mov	r6, r3
   3a0e2:	690d      	ldr	r5, [r1, #16]
   3a0e4:	2000      	movs	r0, #0
   3a0e6:	f8dc 3000 	ldr.w	r3, [ip]
   3a0ea:	3001      	adds	r0, #1
   3a0ec:	b299      	uxth	r1, r3
   3a0ee:	4285      	cmp	r5, r0
   3a0f0:	fb02 6101 	mla	r1, r2, r1, r6
   3a0f4:	ea4f 4613 	mov.w	r6, r3, lsr #16
   3a0f8:	ea4f 4311 	mov.w	r3, r1, lsr #16
   3a0fc:	b289      	uxth	r1, r1
   3a0fe:	fb02 3306 	mla	r3, r2, r6, r3
   3a102:	eb01 4103 	add.w	r1, r1, r3, lsl #16
   3a106:	ea4f 4613 	mov.w	r6, r3, lsr #16
   3a10a:	f84c 1b04 	str.w	r1, [ip], #4
   3a10e:	dcea      	bgt.n	3a0e6 <__multadd+0x12>
   3a110:	b30e      	cbz	r6, 3a156 <__multadd+0x82>
   3a112:	68a3      	ldr	r3, [r4, #8]
   3a114:	42ab      	cmp	r3, r5
   3a116:	dc19      	bgt.n	3a14c <__multadd+0x78>
   3a118:	6861      	ldr	r1, [r4, #4]
   3a11a:	4638      	mov	r0, r7
   3a11c:	3101      	adds	r1, #1
   3a11e:	f7ff ffab 	bl	3a078 <_Balloc>
   3a122:	4680      	mov	r8, r0
   3a124:	b928      	cbnz	r0, 3a132 <__multadd+0x5e>
   3a126:	4602      	mov	r2, r0
   3a128:	4b0c      	ldr	r3, [pc, #48]	; (3a15c <__multadd+0x88>)
   3a12a:	21b5      	movs	r1, #181	; 0xb5
   3a12c:	480c      	ldr	r0, [pc, #48]	; (3a160 <__multadd+0x8c>)
   3a12e:	f004 fb47 	bl	3e7c0 <__assert_func>
   3a132:	6922      	ldr	r2, [r4, #16]
   3a134:	f104 010c 	add.w	r1, r4, #12
   3a138:	300c      	adds	r0, #12
   3a13a:	3202      	adds	r2, #2
   3a13c:	0092      	lsls	r2, r2, #2
   3a13e:	f7ff fe8b 	bl	39e58 <memcpy>
   3a142:	4621      	mov	r1, r4
   3a144:	4644      	mov	r4, r8
   3a146:	4638      	mov	r0, r7
   3a148:	f7ff ffbb 	bl	3a0c2 <_Bfree>
   3a14c:	eb04 0385 	add.w	r3, r4, r5, lsl #2
   3a150:	3501      	adds	r5, #1
   3a152:	615e      	str	r6, [r3, #20]
   3a154:	6125      	str	r5, [r4, #16]
   3a156:	4620      	mov	r0, r4
   3a158:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
   3a15c:	00040678 	.word	0x00040678
   3a160:	0004070e 	.word	0x0004070e

0003a164 <__s2b>:
   3a164:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
   3a168:	4615      	mov	r5, r2
   3a16a:	461f      	mov	r7, r3
   3a16c:	2209      	movs	r2, #9
   3a16e:	3308      	adds	r3, #8
   3a170:	460c      	mov	r4, r1
   3a172:	4606      	mov	r6, r0
   3a174:	2100      	movs	r1, #0
   3a176:	fb93 f3f2 	sdiv	r3, r3, r2
   3a17a:	2201      	movs	r2, #1
   3a17c:	429a      	cmp	r2, r3
   3a17e:	db09      	blt.n	3a194 <__s2b+0x30>
   3a180:	4630      	mov	r0, r6
   3a182:	f7ff ff79 	bl	3a078 <_Balloc>
   3a186:	b940      	cbnz	r0, 3a19a <__s2b+0x36>
   3a188:	4602      	mov	r2, r0
   3a18a:	4b19      	ldr	r3, [pc, #100]	; (3a1f0 <__s2b+0x8c>)
   3a18c:	21ce      	movs	r1, #206	; 0xce
   3a18e:	4819      	ldr	r0, [pc, #100]	; (3a1f4 <__s2b+0x90>)
   3a190:	f004 fb16 	bl	3e7c0 <__assert_func>
   3a194:	0052      	lsls	r2, r2, #1
   3a196:	3101      	adds	r1, #1
   3a198:	e7f0      	b.n	3a17c <__s2b+0x18>
   3a19a:	9b08      	ldr	r3, [sp, #32]
   3a19c:	2d09      	cmp	r5, #9
   3a19e:	6143      	str	r3, [r0, #20]
   3a1a0:	f04f 0301 	mov.w	r3, #1
   3a1a4:	6103      	str	r3, [r0, #16]
   3a1a6:	dd16      	ble.n	3a1d6 <__s2b+0x72>
   3a1a8:	f104 0909 	add.w	r9, r4, #9
   3a1ac:	442c      	add	r4, r5
   3a1ae:	46c8      	mov	r8, r9
   3a1b0:	f818 3b01 	ldrb.w	r3, [r8], #1
   3a1b4:	4601      	mov	r1, r0
   3a1b6:	220a      	movs	r2, #10
   3a1b8:	4630      	mov	r0, r6
   3a1ba:	3b30      	subs	r3, #48	; 0x30
   3a1bc:	f7ff ff8a 	bl	3a0d4 <__multadd>
   3a1c0:	45a0      	cmp	r8, r4
   3a1c2:	d1f5      	bne.n	3a1b0 <__s2b+0x4c>
   3a1c4:	f1a5 0408 	sub.w	r4, r5, #8
   3a1c8:	444c      	add	r4, r9
   3a1ca:	1b2d      	subs	r5, r5, r4
   3a1cc:	1963      	adds	r3, r4, r5
   3a1ce:	42bb      	cmp	r3, r7
   3a1d0:	db04      	blt.n	3a1dc <__s2b+0x78>
   3a1d2:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
   3a1d6:	340a      	adds	r4, #10
   3a1d8:	2509      	movs	r5, #9
   3a1da:	e7f6      	b.n	3a1ca <__s2b+0x66>
   3a1dc:	f814 3b01 	ldrb.w	r3, [r4], #1
   3a1e0:	4601      	mov	r1, r0
   3a1e2:	220a      	movs	r2, #10
   3a1e4:	4630      	mov	r0, r6
   3a1e6:	3b30      	subs	r3, #48	; 0x30
   3a1e8:	f7ff ff74 	bl	3a0d4 <__multadd>
   3a1ec:	e7ee      	b.n	3a1cc <__s2b+0x68>
   3a1ee:	bf00      	nop
   3a1f0:	00040678 	.word	0x00040678
   3a1f4:	0004070e 	.word	0x0004070e

0003a1f8 <__hi0bits>:
   3a1f8:	0c03      	lsrs	r3, r0, #16
   3a1fa:	041b      	lsls	r3, r3, #16
   3a1fc:	b9d3      	cbnz	r3, 3a234 <__hi0bits+0x3c>
   3a1fe:	0400      	lsls	r0, r0, #16
   3a200:	2310      	movs	r3, #16
   3a202:	f010 4f7f 	tst.w	r0, #4278190080	; 0xff000000
   3a206:	bf04      	itt	eq
   3a208:	0200      	lsleq	r0, r0, #8
   3a20a:	3308      	addeq	r3, #8
   3a20c:	f010 4f70 	tst.w	r0, #4026531840	; 0xf0000000
   3a210:	bf04      	itt	eq
   3a212:	0100      	lsleq	r0, r0, #4
   3a214:	3304      	addeq	r3, #4
   3a216:	f010 4f40 	tst.w	r0, #3221225472	; 0xc0000000
   3a21a:	bf04      	itt	eq
   3a21c:	0080      	lsleq	r0, r0, #2
   3a21e:	3302      	addeq	r3, #2
   3a220:	2800      	cmp	r0, #0
   3a222:	db05      	blt.n	3a230 <__hi0bits+0x38>
   3a224:	f010 4f80 	tst.w	r0, #1073741824	; 0x40000000
   3a228:	f103 0301 	add.w	r3, r3, #1
   3a22c:	bf08      	it	eq
   3a22e:	2320      	moveq	r3, #32
   3a230:	4618      	mov	r0, r3
   3a232:	4770      	bx	lr
   3a234:	2300      	movs	r3, #0
   3a236:	e7e4      	b.n	3a202 <__hi0bits+0xa>

0003a238 <__lo0bits>:
   3a238:	6803      	ldr	r3, [r0, #0]
   3a23a:	4601      	mov	r1, r0
   3a23c:	f013 0207 	ands.w	r2, r3, #7
   3a240:	d00b      	beq.n	3a25a <__lo0bits+0x22>
   3a242:	07da      	lsls	r2, r3, #31
   3a244:	d423      	bmi.n	3a28e <__lo0bits+0x56>
   3a246:	0798      	lsls	r0, r3, #30
   3a248:	bf47      	ittee	mi
   3a24a:	085b      	lsrmi	r3, r3, #1
   3a24c:	2001      	movmi	r0, #1
   3a24e:	089b      	lsrpl	r3, r3, #2
   3a250:	2002      	movpl	r0, #2
   3a252:	bf4c      	ite	mi
   3a254:	600b      	strmi	r3, [r1, #0]
   3a256:	600b      	strpl	r3, [r1, #0]
   3a258:	4770      	bx	lr
   3a25a:	b298      	uxth	r0, r3
   3a25c:	b9a8      	cbnz	r0, 3a28a <__lo0bits+0x52>
   3a25e:	0c1b      	lsrs	r3, r3, #16
   3a260:	2010      	movs	r0, #16
   3a262:	b2da      	uxtb	r2, r3
   3a264:	b90a      	cbnz	r2, 3a26a <__lo0bits+0x32>
   3a266:	3008      	adds	r0, #8
   3a268:	0a1b      	lsrs	r3, r3, #8
   3a26a:	071a      	lsls	r2, r3, #28
   3a26c:	bf04      	itt	eq
   3a26e:	091b      	lsreq	r3, r3, #4
   3a270:	3004      	addeq	r0, #4
   3a272:	079a      	lsls	r2, r3, #30
   3a274:	bf04      	itt	eq
   3a276:	089b      	lsreq	r3, r3, #2
   3a278:	3002      	addeq	r0, #2
   3a27a:	07da      	lsls	r2, r3, #31
   3a27c:	d403      	bmi.n	3a286 <__lo0bits+0x4e>
   3a27e:	085b      	lsrs	r3, r3, #1
   3a280:	f100 0001 	add.w	r0, r0, #1
   3a284:	d005      	beq.n	3a292 <__lo0bits+0x5a>
   3a286:	600b      	str	r3, [r1, #0]
   3a288:	4770      	bx	lr
   3a28a:	4610      	mov	r0, r2
   3a28c:	e7e9      	b.n	3a262 <__lo0bits+0x2a>
   3a28e:	2000      	movs	r0, #0
   3a290:	4770      	bx	lr
   3a292:	2020      	movs	r0, #32
   3a294:	4770      	bx	lr

0003a296 <__i2b>:
   3a296:	b510      	push	{r4, lr}
   3a298:	460c      	mov	r4, r1
   3a29a:	2101      	movs	r1, #1
   3a29c:	f7ff feec 	bl	3a078 <_Balloc>
   3a2a0:	4602      	mov	r2, r0
   3a2a2:	b928      	cbnz	r0, 3a2b0 <__i2b+0x1a>
   3a2a4:	4b04      	ldr	r3, [pc, #16]	; (3a2b8 <__i2b+0x22>)
   3a2a6:	f44f 71a0 	mov.w	r1, #320	; 0x140
   3a2aa:	4804      	ldr	r0, [pc, #16]	; (3a2bc <__i2b+0x26>)
   3a2ac:	f004 fa88 	bl	3e7c0 <__assert_func>
   3a2b0:	2301      	movs	r3, #1
   3a2b2:	6144      	str	r4, [r0, #20]
   3a2b4:	6103      	str	r3, [r0, #16]
   3a2b6:	bd10      	pop	{r4, pc}
   3a2b8:	00040678 	.word	0x00040678
   3a2bc:	0004070e 	.word	0x0004070e

0003a2c0 <__multiply>:
   3a2c0:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3a2c4:	4691      	mov	r9, r2
   3a2c6:	690a      	ldr	r2, [r1, #16]
   3a2c8:	460c      	mov	r4, r1
   3a2ca:	b085      	sub	sp, #20
   3a2cc:	f8d9 3010 	ldr.w	r3, [r9, #16]
   3a2d0:	429a      	cmp	r2, r3
   3a2d2:	bfbe      	ittt	lt
   3a2d4:	460b      	movlt	r3, r1
   3a2d6:	464c      	movlt	r4, r9
   3a2d8:	4699      	movlt	r9, r3
   3a2da:	6927      	ldr	r7, [r4, #16]
   3a2dc:	68a3      	ldr	r3, [r4, #8]
   3a2de:	f8d9 a010 	ldr.w	sl, [r9, #16]
   3a2e2:	6861      	ldr	r1, [r4, #4]
   3a2e4:	eb07 060a 	add.w	r6, r7, sl
   3a2e8:	42b3      	cmp	r3, r6
   3a2ea:	bfb8      	it	lt
   3a2ec:	3101      	addlt	r1, #1
   3a2ee:	f7ff fec3 	bl	3a078 <_Balloc>
   3a2f2:	b930      	cbnz	r0, 3a302 <__multiply+0x42>
   3a2f4:	4602      	mov	r2, r0
   3a2f6:	4b44      	ldr	r3, [pc, #272]	; (3a408 <__multiply+0x148>)
   3a2f8:	f240 115d 	movw	r1, #349	; 0x15d
   3a2fc:	4843      	ldr	r0, [pc, #268]	; (3a40c <__multiply+0x14c>)
   3a2fe:	f004 fa5f 	bl	3e7c0 <__assert_func>
   3a302:	f100 0514 	add.w	r5, r0, #20
   3a306:	2200      	movs	r2, #0
   3a308:	eb05 0886 	add.w	r8, r5, r6, lsl #2
   3a30c:	462b      	mov	r3, r5
   3a30e:	4543      	cmp	r3, r8
   3a310:	d321      	bcc.n	3a356 <__multiply+0x96>
   3a312:	f104 0314 	add.w	r3, r4, #20
   3a316:	f104 0115 	add.w	r1, r4, #21
   3a31a:	eb03 0787 	add.w	r7, r3, r7, lsl #2
   3a31e:	f109 0314 	add.w	r3, r9, #20
   3a322:	eb03 028a 	add.w	r2, r3, sl, lsl #2
   3a326:	9202      	str	r2, [sp, #8]
   3a328:	1b3a      	subs	r2, r7, r4
   3a32a:	3a15      	subs	r2, #21
   3a32c:	f022 0203 	bic.w	r2, r2, #3
   3a330:	3204      	adds	r2, #4
   3a332:	428f      	cmp	r7, r1
   3a334:	bf38      	it	cc
   3a336:	2204      	movcc	r2, #4
   3a338:	9201      	str	r2, [sp, #4]
   3a33a:	9a02      	ldr	r2, [sp, #8]
   3a33c:	9303      	str	r3, [sp, #12]
   3a33e:	429a      	cmp	r2, r3
   3a340:	d80c      	bhi.n	3a35c <__multiply+0x9c>
   3a342:	2e00      	cmp	r6, #0
   3a344:	dd03      	ble.n	3a34e <__multiply+0x8e>
   3a346:	f858 3d04 	ldr.w	r3, [r8, #-4]!
   3a34a:	2b00      	cmp	r3, #0
   3a34c:	d05a      	beq.n	3a404 <__multiply+0x144>
   3a34e:	6106      	str	r6, [r0, #16]
   3a350:	b005      	add	sp, #20
   3a352:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3a356:	f843 2b04 	str.w	r2, [r3], #4
   3a35a:	e7d8      	b.n	3a30e <__multiply+0x4e>
   3a35c:	f8b3 a000 	ldrh.w	sl, [r3]
   3a360:	f1ba 0f00 	cmp.w	sl, #0
   3a364:	d024      	beq.n	3a3b0 <__multiply+0xf0>
   3a366:	f104 0e14 	add.w	lr, r4, #20
   3a36a:	46a9      	mov	r9, r5
   3a36c:	f04f 0c00 	mov.w	ip, #0
   3a370:	f85e 2b04 	ldr.w	r2, [lr], #4
   3a374:	f8d9 1000 	ldr.w	r1, [r9]
   3a378:	fa1f fb82 	uxth.w	fp, r2
   3a37c:	4577      	cmp	r7, lr
   3a37e:	b289      	uxth	r1, r1
   3a380:	fb0a 110b 	mla	r1, sl, fp, r1
   3a384:	ea4f 4b12 	mov.w	fp, r2, lsr #16
   3a388:	f8d9 2000 	ldr.w	r2, [r9]
   3a38c:	4461      	add	r1, ip
   3a38e:	ea4f 4c12 	mov.w	ip, r2, lsr #16
   3a392:	fb0a c20b 	mla	r2, sl, fp, ip
   3a396:	eb02 4211 	add.w	r2, r2, r1, lsr #16
   3a39a:	b289      	uxth	r1, r1
   3a39c:	ea4f 4c12 	mov.w	ip, r2, lsr #16
   3a3a0:	ea41 4102 	orr.w	r1, r1, r2, lsl #16
   3a3a4:	f849 1b04 	str.w	r1, [r9], #4
   3a3a8:	d8e2      	bhi.n	3a370 <__multiply+0xb0>
   3a3aa:	9a01      	ldr	r2, [sp, #4]
   3a3ac:	f845 c002 	str.w	ip, [r5, r2]
   3a3b0:	9a03      	ldr	r2, [sp, #12]
   3a3b2:	3304      	adds	r3, #4
   3a3b4:	f8b2 9002 	ldrh.w	r9, [r2, #2]
   3a3b8:	f1b9 0f00 	cmp.w	r9, #0
   3a3bc:	d020      	beq.n	3a400 <__multiply+0x140>
   3a3be:	6829      	ldr	r1, [r5, #0]
   3a3c0:	f104 0c14 	add.w	ip, r4, #20
   3a3c4:	46ae      	mov	lr, r5
   3a3c6:	f04f 0a00 	mov.w	sl, #0
   3a3ca:	f8bc b000 	ldrh.w	fp, [ip]
   3a3ce:	b289      	uxth	r1, r1
   3a3d0:	f8be 2002 	ldrh.w	r2, [lr, #2]
   3a3d4:	fb09 220b 	mla	r2, r9, fp, r2
   3a3d8:	4492      	add	sl, r2
   3a3da:	ea41 410a 	orr.w	r1, r1, sl, lsl #16
   3a3de:	f84e 1b04 	str.w	r1, [lr], #4
   3a3e2:	f85c 2b04 	ldr.w	r2, [ip], #4
   3a3e6:	f8be 1000 	ldrh.w	r1, [lr]
   3a3ea:	0c12      	lsrs	r2, r2, #16
   3a3ec:	4567      	cmp	r7, ip
   3a3ee:	fb09 1102 	mla	r1, r9, r2, r1
   3a3f2:	eb01 411a 	add.w	r1, r1, sl, lsr #16
   3a3f6:	ea4f 4a11 	mov.w	sl, r1, lsr #16
   3a3fa:	d8e6      	bhi.n	3a3ca <__multiply+0x10a>
   3a3fc:	9a01      	ldr	r2, [sp, #4]
   3a3fe:	50a9      	str	r1, [r5, r2]
   3a400:	3504      	adds	r5, #4
   3a402:	e79a      	b.n	3a33a <__multiply+0x7a>
   3a404:	3e01      	subs	r6, #1
   3a406:	e79c      	b.n	3a342 <__multiply+0x82>
   3a408:	00040678 	.word	0x00040678
   3a40c:	0004070e 	.word	0x0004070e

0003a410 <__pow5mult>:
   3a410:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
   3a414:	4615      	mov	r5, r2
   3a416:	f012 0203 	ands.w	r2, r2, #3
   3a41a:	4606      	mov	r6, r0
   3a41c:	460f      	mov	r7, r1
   3a41e:	d007      	beq.n	3a430 <__pow5mult+0x20>
   3a420:	3a01      	subs	r2, #1
   3a422:	4c1a      	ldr	r4, [pc, #104]	; (3a48c <__pow5mult+0x7c>)
   3a424:	2300      	movs	r3, #0
   3a426:	f854 2022 	ldr.w	r2, [r4, r2, lsl #2]
   3a42a:	f7ff fe53 	bl	3a0d4 <__multadd>
   3a42e:	4607      	mov	r7, r0
   3a430:	10ad      	asrs	r5, r5, #2
   3a432:	d027      	beq.n	3a484 <__pow5mult+0x74>
   3a434:	6cb4      	ldr	r4, [r6, #72]	; 0x48
   3a436:	b944      	cbnz	r4, 3a44a <__pow5mult+0x3a>
   3a438:	f240 2171 	movw	r1, #625	; 0x271
   3a43c:	4630      	mov	r0, r6
   3a43e:	f7ff ff2a 	bl	3a296 <__i2b>
   3a442:	2300      	movs	r3, #0
   3a444:	4604      	mov	r4, r0
   3a446:	64b0      	str	r0, [r6, #72]	; 0x48
   3a448:	6003      	str	r3, [r0, #0]
   3a44a:	f04f 0900 	mov.w	r9, #0
   3a44e:	07eb      	lsls	r3, r5, #31
   3a450:	d50a      	bpl.n	3a468 <__pow5mult+0x58>
   3a452:	4639      	mov	r1, r7
   3a454:	4622      	mov	r2, r4
   3a456:	4630      	mov	r0, r6
   3a458:	f7ff ff32 	bl	3a2c0 <__multiply>
   3a45c:	4680      	mov	r8, r0
   3a45e:	4639      	mov	r1, r7
   3a460:	4630      	mov	r0, r6
   3a462:	4647      	mov	r7, r8
   3a464:	f7ff fe2d 	bl	3a0c2 <_Bfree>
   3a468:	106d      	asrs	r5, r5, #1
   3a46a:	d00b      	beq.n	3a484 <__pow5mult+0x74>
   3a46c:	6820      	ldr	r0, [r4, #0]
   3a46e:	b938      	cbnz	r0, 3a480 <__pow5mult+0x70>
   3a470:	4622      	mov	r2, r4
   3a472:	4621      	mov	r1, r4
   3a474:	4630      	mov	r0, r6
   3a476:	f7ff ff23 	bl	3a2c0 <__multiply>
   3a47a:	6020      	str	r0, [r4, #0]
   3a47c:	f8c0 9000 	str.w	r9, [r0]
   3a480:	4604      	mov	r4, r0
   3a482:	e7e4      	b.n	3a44e <__pow5mult+0x3e>
   3a484:	4638      	mov	r0, r7
   3a486:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
   3a48a:	bf00      	nop
   3a48c:	00040788 	.word	0x00040788

0003a490 <__lshift>:
   3a490:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
   3a494:	460c      	mov	r4, r1
   3a496:	4607      	mov	r7, r0
   3a498:	4691      	mov	r9, r2
   3a49a:	ea4f 1a62 	mov.w	sl, r2, asr #5
   3a49e:	6923      	ldr	r3, [r4, #16]
   3a4a0:	6849      	ldr	r1, [r1, #4]
   3a4a2:	eb03 1862 	add.w	r8, r3, r2, asr #5
   3a4a6:	68a3      	ldr	r3, [r4, #8]
   3a4a8:	f108 0601 	add.w	r6, r8, #1
   3a4ac:	42b3      	cmp	r3, r6
   3a4ae:	db0b      	blt.n	3a4c8 <__lshift+0x38>
   3a4b0:	4638      	mov	r0, r7
   3a4b2:	f7ff fde1 	bl	3a078 <_Balloc>
   3a4b6:	4605      	mov	r5, r0
   3a4b8:	b948      	cbnz	r0, 3a4ce <__lshift+0x3e>
   3a4ba:	4602      	mov	r2, r0
   3a4bc:	4b2a      	ldr	r3, [pc, #168]	; (3a568 <__lshift+0xd8>)
   3a4be:	f240 11d9 	movw	r1, #473	; 0x1d9
   3a4c2:	482a      	ldr	r0, [pc, #168]	; (3a56c <__lshift+0xdc>)
   3a4c4:	f004 f97c 	bl	3e7c0 <__assert_func>
   3a4c8:	3101      	adds	r1, #1
   3a4ca:	005b      	lsls	r3, r3, #1
   3a4cc:	e7ee      	b.n	3a4ac <__lshift+0x1c>
   3a4ce:	2300      	movs	r3, #0
   3a4d0:	f100 0114 	add.w	r1, r0, #20
   3a4d4:	f100 0210 	add.w	r2, r0, #16
   3a4d8:	4618      	mov	r0, r3
   3a4da:	4553      	cmp	r3, sl
   3a4dc:	db37      	blt.n	3a54e <__lshift+0xbe>
   3a4de:	ea2a 7aea 	bic.w	sl, sl, sl, asr #31
   3a4e2:	f104 0314 	add.w	r3, r4, #20
   3a4e6:	6920      	ldr	r0, [r4, #16]
   3a4e8:	f019 091f 	ands.w	r9, r9, #31
   3a4ec:	eb01 018a 	add.w	r1, r1, sl, lsl #2
   3a4f0:	eb03 0080 	add.w	r0, r3, r0, lsl #2
   3a4f4:	d02f      	beq.n	3a556 <__lshift+0xc6>
   3a4f6:	f1c9 0e20 	rsb	lr, r9, #32
   3a4fa:	468a      	mov	sl, r1
   3a4fc:	f04f 0c00 	mov.w	ip, #0
   3a500:	681a      	ldr	r2, [r3, #0]
   3a502:	fa02 f209 	lsl.w	r2, r2, r9
   3a506:	ea42 020c 	orr.w	r2, r2, ip
   3a50a:	f84a 2b04 	str.w	r2, [sl], #4
   3a50e:	f853 2b04 	ldr.w	r2, [r3], #4
   3a512:	4298      	cmp	r0, r3
   3a514:	fa22 fc0e 	lsr.w	ip, r2, lr
   3a518:	d8f2      	bhi.n	3a500 <__lshift+0x70>
   3a51a:	1b03      	subs	r3, r0, r4
   3a51c:	f104 0215 	add.w	r2, r4, #21
   3a520:	3b15      	subs	r3, #21
   3a522:	f023 0303 	bic.w	r3, r3, #3
   3a526:	3304      	adds	r3, #4
   3a528:	4290      	cmp	r0, r2
   3a52a:	bf38      	it	cc
   3a52c:	2304      	movcc	r3, #4
   3a52e:	f841 c003 	str.w	ip, [r1, r3]
   3a532:	f1bc 0f00 	cmp.w	ip, #0
   3a536:	d001      	beq.n	3a53c <__lshift+0xac>
   3a538:	f108 0602 	add.w	r6, r8, #2
   3a53c:	3e01      	subs	r6, #1
   3a53e:	4638      	mov	r0, r7
   3a540:	4621      	mov	r1, r4
   3a542:	612e      	str	r6, [r5, #16]
   3a544:	f7ff fdbd 	bl	3a0c2 <_Bfree>
   3a548:	4628      	mov	r0, r5
   3a54a:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
   3a54e:	3301      	adds	r3, #1
   3a550:	f842 0f04 	str.w	r0, [r2, #4]!
   3a554:	e7c1      	b.n	3a4da <__lshift+0x4a>
   3a556:	3904      	subs	r1, #4
   3a558:	f853 2b04 	ldr.w	r2, [r3], #4
   3a55c:	4298      	cmp	r0, r3
   3a55e:	f841 2f04 	str.w	r2, [r1, #4]!
   3a562:	d8f9      	bhi.n	3a558 <__lshift+0xc8>
   3a564:	e7ea      	b.n	3a53c <__lshift+0xac>
   3a566:	bf00      	nop
   3a568:	00040678 	.word	0x00040678
   3a56c:	0004070e 	.word	0x0004070e

0003a570 <__mcmp>:
   3a570:	6902      	ldr	r2, [r0, #16]
   3a572:	b530      	push	{r4, r5, lr}
   3a574:	690c      	ldr	r4, [r1, #16]
   3a576:	1b12      	subs	r2, r2, r4
   3a578:	d10e      	bne.n	3a598 <__mcmp+0x28>
   3a57a:	f100 0314 	add.w	r3, r0, #20
   3a57e:	3114      	adds	r1, #20
   3a580:	eb03 0084 	add.w	r0, r3, r4, lsl #2
   3a584:	eb01 0184 	add.w	r1, r1, r4, lsl #2
   3a588:	f850 5d04 	ldr.w	r5, [r0, #-4]!
   3a58c:	f851 4d04 	ldr.w	r4, [r1, #-4]!
   3a590:	42a5      	cmp	r5, r4
   3a592:	d003      	beq.n	3a59c <__mcmp+0x2c>
   3a594:	d305      	bcc.n	3a5a2 <__mcmp+0x32>
   3a596:	2201      	movs	r2, #1
   3a598:	4610      	mov	r0, r2
   3a59a:	bd30      	pop	{r4, r5, pc}
   3a59c:	4283      	cmp	r3, r0
   3a59e:	d3f3      	bcc.n	3a588 <__mcmp+0x18>
   3a5a0:	e7fa      	b.n	3a598 <__mcmp+0x28>
   3a5a2:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3a5a6:	e7f7      	b.n	3a598 <__mcmp+0x28>

0003a5a8 <__mdiff>:
   3a5a8:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3a5ac:	460c      	mov	r4, r1
   3a5ae:	4606      	mov	r6, r0
   3a5b0:	4611      	mov	r1, r2
   3a5b2:	4692      	mov	sl, r2
   3a5b4:	4620      	mov	r0, r4
   3a5b6:	f7ff ffdb 	bl	3a570 <__mcmp>
   3a5ba:	1e05      	subs	r5, r0, #0
   3a5bc:	d110      	bne.n	3a5e0 <__mdiff+0x38>
   3a5be:	4629      	mov	r1, r5
   3a5c0:	4630      	mov	r0, r6
   3a5c2:	f7ff fd59 	bl	3a078 <_Balloc>
   3a5c6:	b930      	cbnz	r0, 3a5d6 <__mdiff+0x2e>
   3a5c8:	4b3d      	ldr	r3, [pc, #244]	; (3a6c0 <__mdiff+0x118>)
   3a5ca:	4602      	mov	r2, r0
   3a5cc:	f240 2132 	movw	r1, #562	; 0x232
   3a5d0:	483c      	ldr	r0, [pc, #240]	; (3a6c4 <__mdiff+0x11c>)
   3a5d2:	f004 f8f5 	bl	3e7c0 <__assert_func>
   3a5d6:	2301      	movs	r3, #1
   3a5d8:	e9c0 3504 	strd	r3, r5, [r0, #16]
   3a5dc:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3a5e0:	bfa4      	itt	ge
   3a5e2:	4653      	movge	r3, sl
   3a5e4:	46a2      	movge	sl, r4
   3a5e6:	4630      	mov	r0, r6
   3a5e8:	bfa8      	it	ge
   3a5ea:	2500      	movge	r5, #0
   3a5ec:	f8da 1004 	ldr.w	r1, [sl, #4]
   3a5f0:	bfac      	ite	ge
   3a5f2:	461c      	movge	r4, r3
   3a5f4:	2501      	movlt	r5, #1
   3a5f6:	f7ff fd3f 	bl	3a078 <_Balloc>
   3a5fa:	b920      	cbnz	r0, 3a606 <__mdiff+0x5e>
   3a5fc:	4b30      	ldr	r3, [pc, #192]	; (3a6c0 <__mdiff+0x118>)
   3a5fe:	4602      	mov	r2, r0
   3a600:	f44f 7110 	mov.w	r1, #576	; 0x240
   3a604:	e7e4      	b.n	3a5d0 <__mdiff+0x28>
   3a606:	f8da 7010 	ldr.w	r7, [sl, #16]
   3a60a:	f104 0914 	add.w	r9, r4, #20
   3a60e:	6926      	ldr	r6, [r4, #16]
   3a610:	f100 0814 	add.w	r8, r0, #20
   3a614:	60c5      	str	r5, [r0, #12]
   3a616:	f10a 0514 	add.w	r5, sl, #20
   3a61a:	f10a 0210 	add.w	r2, sl, #16
   3a61e:	eb09 0686 	add.w	r6, r9, r6, lsl #2
   3a622:	eb05 0e87 	add.w	lr, r5, r7, lsl #2
   3a626:	46c2      	mov	sl, r8
   3a628:	f04f 0c00 	mov.w	ip, #0
   3a62c:	f852 bf04 	ldr.w	fp, [r2, #4]!
   3a630:	f859 3b04 	ldr.w	r3, [r9], #4
   3a634:	fa1f f18b 	uxth.w	r1, fp
   3a638:	454e      	cmp	r6, r9
   3a63a:	448c      	add	ip, r1
   3a63c:	b299      	uxth	r1, r3
   3a63e:	ea4f 4313 	mov.w	r3, r3, lsr #16
   3a642:	ebac 0101 	sub.w	r1, ip, r1
   3a646:	ebc3 431b 	rsb	r3, r3, fp, lsr #16
   3a64a:	eb03 4321 	add.w	r3, r3, r1, asr #16
   3a64e:	b289      	uxth	r1, r1
   3a650:	ea4f 4c23 	mov.w	ip, r3, asr #16
   3a654:	ea41 4303 	orr.w	r3, r1, r3, lsl #16
   3a658:	f84a 3b04 	str.w	r3, [sl], #4
   3a65c:	d8e6      	bhi.n	3a62c <__mdiff+0x84>
   3a65e:	1b33      	subs	r3, r6, r4
   3a660:	3415      	adds	r4, #21
   3a662:	3b15      	subs	r3, #21
   3a664:	f023 0303 	bic.w	r3, r3, #3
   3a668:	3304      	adds	r3, #4
   3a66a:	42a6      	cmp	r6, r4
   3a66c:	bf38      	it	cc
   3a66e:	2304      	movcc	r3, #4
   3a670:	441d      	add	r5, r3
   3a672:	4443      	add	r3, r8
   3a674:	462c      	mov	r4, r5
   3a676:	461e      	mov	r6, r3
   3a678:	4574      	cmp	r4, lr
   3a67a:	d30e      	bcc.n	3a69a <__mdiff+0xf2>
   3a67c:	f10e 0203 	add.w	r2, lr, #3
   3a680:	1b52      	subs	r2, r2, r5
   3a682:	3d03      	subs	r5, #3
   3a684:	f022 0203 	bic.w	r2, r2, #3
   3a688:	45ae      	cmp	lr, r5
   3a68a:	bf38      	it	cc
   3a68c:	2200      	movcc	r2, #0
   3a68e:	441a      	add	r2, r3
   3a690:	f852 3d04 	ldr.w	r3, [r2, #-4]!
   3a694:	b18b      	cbz	r3, 3a6ba <__mdiff+0x112>
   3a696:	6107      	str	r7, [r0, #16]
   3a698:	e7a0      	b.n	3a5dc <__mdiff+0x34>
   3a69a:	f854 8b04 	ldr.w	r8, [r4], #4
   3a69e:	fa1f f288 	uxth.w	r2, r8
   3a6a2:	4462      	add	r2, ip
   3a6a4:	1411      	asrs	r1, r2, #16
   3a6a6:	b292      	uxth	r2, r2
   3a6a8:	eb01 4118 	add.w	r1, r1, r8, lsr #16
   3a6ac:	ea42 4201 	orr.w	r2, r2, r1, lsl #16
   3a6b0:	ea4f 4c21 	mov.w	ip, r1, asr #16
   3a6b4:	f846 2b04 	str.w	r2, [r6], #4
   3a6b8:	e7de      	b.n	3a678 <__mdiff+0xd0>
   3a6ba:	3f01      	subs	r7, #1
   3a6bc:	e7e8      	b.n	3a690 <__mdiff+0xe8>
   3a6be:	bf00      	nop
   3a6c0:	00040678 	.word	0x00040678
   3a6c4:	0004070e 	.word	0x0004070e

0003a6c8 <__ulp>:
   3a6c8:	b082      	sub	sp, #8
   3a6ca:	4913      	ldr	r1, [pc, #76]	; (3a718 <__ulp+0x50>)
   3a6cc:	ed8d 0b00 	vstr	d0, [sp]
   3a6d0:	9b01      	ldr	r3, [sp, #4]
   3a6d2:	4019      	ands	r1, r3
   3a6d4:	f1a1 7150 	sub.w	r1, r1, #54525952	; 0x3400000
   3a6d8:	2900      	cmp	r1, #0
   3a6da:	dd05      	ble.n	3a6e8 <__ulp+0x20>
   3a6dc:	2200      	movs	r2, #0
   3a6de:	460b      	mov	r3, r1
   3a6e0:	ec43 2b10 	vmov	d0, r2, r3
   3a6e4:	b002      	add	sp, #8
   3a6e6:	4770      	bx	lr
   3a6e8:	4249      	negs	r1, r1
   3a6ea:	2200      	movs	r2, #0
   3a6ec:	2300      	movs	r3, #0
   3a6ee:	f1b1 7fa0 	cmp.w	r1, #20971520	; 0x1400000
   3a6f2:	ea4f 5021 	mov.w	r0, r1, asr #20
   3a6f6:	da04      	bge.n	3a702 <__ulp+0x3a>
   3a6f8:	f44f 2100 	mov.w	r1, #524288	; 0x80000
   3a6fc:	fa41 f300 	asr.w	r3, r1, r0
   3a700:	e7ee      	b.n	3a6e0 <__ulp+0x18>
   3a702:	f1a0 0114 	sub.w	r1, r0, #20
   3a706:	291e      	cmp	r1, #30
   3a708:	bfd6      	itet	le
   3a70a:	f04f 4000 	movle.w	r0, #2147483648	; 0x80000000
   3a70e:	2101      	movgt	r1, #1
   3a710:	fa20 f101 	lsrle.w	r1, r0, r1
   3a714:	460a      	mov	r2, r1
   3a716:	e7e3      	b.n	3a6e0 <__ulp+0x18>
   3a718:	7ff00000 	.word	0x7ff00000

0003a71c <__b2d>:
   3a71c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
   3a71e:	6905      	ldr	r5, [r0, #16]
   3a720:	f100 0714 	add.w	r7, r0, #20
   3a724:	eb07 0585 	add.w	r5, r7, r5, lsl #2
   3a728:	f855 4c04 	ldr.w	r4, [r5, #-4]
   3a72c:	1f2e      	subs	r6, r5, #4
   3a72e:	4620      	mov	r0, r4
   3a730:	f7ff fd62 	bl	3a1f8 <__hi0bits>
   3a734:	f1c0 0320 	rsb	r3, r0, #32
   3a738:	280a      	cmp	r0, #10
   3a73a:	f8df c07c 	ldr.w	ip, [pc, #124]	; 3a7b8 <__b2d+0x9c>
   3a73e:	600b      	str	r3, [r1, #0]
   3a740:	dc15      	bgt.n	3a76e <__b2d+0x52>
   3a742:	f1c0 0e0b 	rsb	lr, r0, #11
   3a746:	42b7      	cmp	r7, r6
   3a748:	f100 0015 	add.w	r0, r0, #21
   3a74c:	fa24 f10e 	lsr.w	r1, r4, lr
   3a750:	fa04 f000 	lsl.w	r0, r4, r0
   3a754:	ea41 030c 	orr.w	r3, r1, ip
   3a758:	bf34      	ite	cc
   3a75a:	f855 1c08 	ldrcc.w	r1, [r5, #-8]
   3a75e:	2100      	movcs	r1, #0
   3a760:	fa21 f10e 	lsr.w	r1, r1, lr
   3a764:	ea40 0201 	orr.w	r2, r0, r1
   3a768:	ec43 2b10 	vmov	d0, r2, r3
   3a76c:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
   3a76e:	42b7      	cmp	r7, r6
   3a770:	bf3a      	itte	cc
   3a772:	f1a5 0608 	subcc.w	r6, r5, #8
   3a776:	f855 1c08 	ldrcc.w	r1, [r5, #-8]
   3a77a:	2100      	movcs	r1, #0
   3a77c:	380b      	subs	r0, #11
   3a77e:	d017      	beq.n	3a7b0 <__b2d+0x94>
   3a780:	f1c0 0c20 	rsb	ip, r0, #32
   3a784:	fa04 f500 	lsl.w	r5, r4, r0
   3a788:	42be      	cmp	r6, r7
   3a78a:	fa01 f000 	lsl.w	r0, r1, r0
   3a78e:	fa21 f40c 	lsr.w	r4, r1, ip
   3a792:	ea45 0504 	orr.w	r5, r5, r4
   3a796:	bf8c      	ite	hi
   3a798:	f856 4c04 	ldrhi.w	r4, [r6, #-4]
   3a79c:	2400      	movls	r4, #0
   3a79e:	f045 537f 	orr.w	r3, r5, #1069547520	; 0x3fc00000
   3a7a2:	fa24 f40c 	lsr.w	r4, r4, ip
   3a7a6:	f443 1340 	orr.w	r3, r3, #3145728	; 0x300000
   3a7aa:	ea40 0204 	orr.w	r2, r0, r4
   3a7ae:	e7db      	b.n	3a768 <__b2d+0x4c>
   3a7b0:	ea44 030c 	orr.w	r3, r4, ip
   3a7b4:	460a      	mov	r2, r1
   3a7b6:	e7d7      	b.n	3a768 <__b2d+0x4c>
   3a7b8:	3ff00000 	.word	0x3ff00000

0003a7bc <__d2b>:
   3a7bc:	e92d 43f7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, lr}
   3a7c0:	4689      	mov	r9, r1
   3a7c2:	2101      	movs	r1, #1
   3a7c4:	4690      	mov	r8, r2
   3a7c6:	ec57 6b10 	vmov	r6, r7, d0
   3a7ca:	f7ff fc55 	bl	3a078 <_Balloc>
   3a7ce:	4604      	mov	r4, r0
   3a7d0:	b930      	cbnz	r0, 3a7e0 <__d2b+0x24>
   3a7d2:	4602      	mov	r2, r0
   3a7d4:	4b25      	ldr	r3, [pc, #148]	; (3a86c <__d2b+0xb0>)
   3a7d6:	f240 310a 	movw	r1, #778	; 0x30a
   3a7da:	4825      	ldr	r0, [pc, #148]	; (3a870 <__d2b+0xb4>)
   3a7dc:	f003 fff0 	bl	3e7c0 <__assert_func>
   3a7e0:	f3c7 550a 	ubfx	r5, r7, #20, #11
   3a7e4:	f3c7 0313 	ubfx	r3, r7, #0, #20
   3a7e8:	bb35      	cbnz	r5, 3a838 <__d2b+0x7c>
   3a7ea:	2e00      	cmp	r6, #0
   3a7ec:	9301      	str	r3, [sp, #4]
   3a7ee:	d028      	beq.n	3a842 <__d2b+0x86>
   3a7f0:	4668      	mov	r0, sp
   3a7f2:	9600      	str	r6, [sp, #0]
   3a7f4:	f7ff fd20 	bl	3a238 <__lo0bits>
   3a7f8:	9900      	ldr	r1, [sp, #0]
   3a7fa:	b300      	cbz	r0, 3a83e <__d2b+0x82>
   3a7fc:	9a01      	ldr	r2, [sp, #4]
   3a7fe:	f1c0 0320 	rsb	r3, r0, #32
   3a802:	fa02 f303 	lsl.w	r3, r2, r3
   3a806:	40c2      	lsrs	r2, r0
   3a808:	430b      	orrs	r3, r1
   3a80a:	9201      	str	r2, [sp, #4]
   3a80c:	6163      	str	r3, [r4, #20]
   3a80e:	9b01      	ldr	r3, [sp, #4]
   3a810:	2b00      	cmp	r3, #0
   3a812:	61a3      	str	r3, [r4, #24]
   3a814:	bf14      	ite	ne
   3a816:	2202      	movne	r2, #2
   3a818:	2201      	moveq	r2, #1
   3a81a:	6122      	str	r2, [r4, #16]
   3a81c:	b1d5      	cbz	r5, 3a854 <__d2b+0x98>
   3a81e:	f2a5 4533 	subw	r5, r5, #1075	; 0x433
   3a822:	4405      	add	r5, r0
   3a824:	f1c0 0035 	rsb	r0, r0, #53	; 0x35
   3a828:	f8c9 5000 	str.w	r5, [r9]
   3a82c:	f8c8 0000 	str.w	r0, [r8]
   3a830:	4620      	mov	r0, r4
   3a832:	b003      	add	sp, #12
   3a834:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
   3a838:	f443 1380 	orr.w	r3, r3, #1048576	; 0x100000
   3a83c:	e7d5      	b.n	3a7ea <__d2b+0x2e>
   3a83e:	6161      	str	r1, [r4, #20]
   3a840:	e7e5      	b.n	3a80e <__d2b+0x52>
   3a842:	a801      	add	r0, sp, #4
   3a844:	f7ff fcf8 	bl	3a238 <__lo0bits>
   3a848:	9b01      	ldr	r3, [sp, #4]
   3a84a:	2201      	movs	r2, #1
   3a84c:	3020      	adds	r0, #32
   3a84e:	6163      	str	r3, [r4, #20]
   3a850:	6122      	str	r2, [r4, #16]
   3a852:	e7e3      	b.n	3a81c <__d2b+0x60>
   3a854:	f2a0 4032 	subw	r0, r0, #1074	; 0x432
   3a858:	eb04 0382 	add.w	r3, r4, r2, lsl #2
   3a85c:	f8c9 0000 	str.w	r0, [r9]
   3a860:	6918      	ldr	r0, [r3, #16]
   3a862:	f7ff fcc9 	bl	3a1f8 <__hi0bits>
   3a866:	ebc0 1042 	rsb	r0, r0, r2, lsl #5
   3a86a:	e7df      	b.n	3a82c <__d2b+0x70>
   3a86c:	00040678 	.word	0x00040678
   3a870:	0004070e 	.word	0x0004070e

0003a874 <__ratio>:
   3a874:	e92d 4ff7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3a878:	4688      	mov	r8, r1
   3a87a:	4681      	mov	r9, r0
   3a87c:	4669      	mov	r1, sp
   3a87e:	f7ff ff4d 	bl	3a71c <__b2d>
   3a882:	a901      	add	r1, sp, #4
   3a884:	4640      	mov	r0, r8
   3a886:	ec55 4b10 	vmov	r4, r5, d0
   3a88a:	f7ff ff47 	bl	3a71c <__b2d>
   3a88e:	f8d9 3010 	ldr.w	r3, [r9, #16]
   3a892:	f8d8 2010 	ldr.w	r2, [r8, #16]
   3a896:	462f      	mov	r7, r5
   3a898:	eba3 0c02 	sub.w	ip, r3, r2
   3a89c:	e9dd 3200 	ldrd	r3, r2, [sp]
   3a8a0:	ec51 0b10 	vmov	r0, r1, d0
   3a8a4:	1a9b      	subs	r3, r3, r2
   3a8a6:	468b      	mov	fp, r1
   3a8a8:	4620      	mov	r0, r4
   3a8aa:	eb03 134c 	add.w	r3, r3, ip, lsl #5
   3a8ae:	2b00      	cmp	r3, #0
   3a8b0:	bfd5      	itete	le
   3a8b2:	460a      	movle	r2, r1
   3a8b4:	462a      	movgt	r2, r5
   3a8b6:	ebc3 3303 	rsble	r3, r3, r3, lsl #12
   3a8ba:	eb02 5703 	addgt.w	r7, r2, r3, lsl #20
   3a8be:	bfd8      	it	le
   3a8c0:	eb02 5b03 	addle.w	fp, r2, r3, lsl #20
   3a8c4:	ee10 2a10 	vmov	r2, s0
   3a8c8:	4639      	mov	r1, r7
   3a8ca:	465b      	mov	r3, fp
   3a8cc:	f005 f960 	bl	3fb90 <__aeabi_ddiv>
   3a8d0:	ec41 0b10 	vmov	d0, r0, r1
   3a8d4:	b003      	add	sp, #12
   3a8d6:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}

0003a8da <_mprec_log10>:
   3a8da:	2817      	cmp	r0, #23
   3a8dc:	b5d0      	push	{r4, r6, r7, lr}
   3a8de:	4604      	mov	r4, r0
   3a8e0:	dc07      	bgt.n	3a8f2 <_mprec_log10+0x18>
   3a8e2:	4809      	ldr	r0, [pc, #36]	; (3a908 <_mprec_log10+0x2e>)
   3a8e4:	eb00 04c4 	add.w	r4, r0, r4, lsl #3
   3a8e8:	e9d4 0104 	ldrd	r0, r1, [r4, #16]
   3a8ec:	ec41 0b10 	vmov	d0, r0, r1
   3a8f0:	bdd0      	pop	{r4, r6, r7, pc}
   3a8f2:	2000      	movs	r0, #0
   3a8f4:	4905      	ldr	r1, [pc, #20]	; (3a90c <_mprec_log10+0x32>)
   3a8f6:	2600      	movs	r6, #0
   3a8f8:	4f05      	ldr	r7, [pc, #20]	; (3a910 <_mprec_log10+0x36>)
   3a8fa:	4632      	mov	r2, r6
   3a8fc:	463b      	mov	r3, r7
   3a8fe:	f005 f81d 	bl	3f93c <__aeabi_dmul>
   3a902:	3c01      	subs	r4, #1
   3a904:	d1f9      	bne.n	3a8fa <_mprec_log10+0x20>
   3a906:	e7f1      	b.n	3a8ec <_mprec_log10+0x12>
   3a908:	00040788 	.word	0x00040788
   3a90c:	3ff00000 	.word	0x3ff00000
   3a910:	40240000 	.word	0x40240000

0003a914 <__copybits>:
   3a914:	3901      	subs	r1, #1
   3a916:	f102 0314 	add.w	r3, r2, #20
   3a91a:	1149      	asrs	r1, r1, #5
   3a91c:	b570      	push	{r4, r5, r6, lr}
   3a91e:	3101      	adds	r1, #1
   3a920:	6914      	ldr	r4, [r2, #16]
   3a922:	1f05      	subs	r5, r0, #4
   3a924:	eb00 0181 	add.w	r1, r0, r1, lsl #2
   3a928:	eb03 0484 	add.w	r4, r3, r4, lsl #2
   3a92c:	42a3      	cmp	r3, r4
   3a92e:	d30c      	bcc.n	3a94a <__copybits+0x36>
   3a930:	1aa3      	subs	r3, r4, r2
   3a932:	3211      	adds	r2, #17
   3a934:	3b11      	subs	r3, #17
   3a936:	f023 0303 	bic.w	r3, r3, #3
   3a93a:	42a2      	cmp	r2, r4
   3a93c:	bf88      	it	hi
   3a93e:	2300      	movhi	r3, #0
   3a940:	4418      	add	r0, r3
   3a942:	2300      	movs	r3, #0
   3a944:	4288      	cmp	r0, r1
   3a946:	d305      	bcc.n	3a954 <__copybits+0x40>
   3a948:	bd70      	pop	{r4, r5, r6, pc}
   3a94a:	f853 6b04 	ldr.w	r6, [r3], #4
   3a94e:	f845 6f04 	str.w	r6, [r5, #4]!
   3a952:	e7eb      	b.n	3a92c <__copybits+0x18>
   3a954:	f840 3b04 	str.w	r3, [r0], #4
   3a958:	e7f4      	b.n	3a944 <__copybits+0x30>

0003a95a <__any_on>:
   3a95a:	f100 0214 	add.w	r2, r0, #20
   3a95e:	114b      	asrs	r3, r1, #5
   3a960:	6900      	ldr	r0, [r0, #16]
   3a962:	4298      	cmp	r0, r3
   3a964:	b510      	push	{r4, lr}
   3a966:	db11      	blt.n	3a98c <__any_on+0x32>
   3a968:	dd0a      	ble.n	3a980 <__any_on+0x26>
   3a96a:	f011 011f 	ands.w	r1, r1, #31
   3a96e:	d007      	beq.n	3a980 <__any_on+0x26>
   3a970:	f852 4023 	ldr.w	r4, [r2, r3, lsl #2]
   3a974:	fa24 f001 	lsr.w	r0, r4, r1
   3a978:	fa00 f101 	lsl.w	r1, r0, r1
   3a97c:	428c      	cmp	r4, r1
   3a97e:	d10b      	bne.n	3a998 <__any_on+0x3e>
   3a980:	eb02 0383 	add.w	r3, r2, r3, lsl #2
   3a984:	4293      	cmp	r3, r2
   3a986:	d803      	bhi.n	3a990 <__any_on+0x36>
   3a988:	2000      	movs	r0, #0
   3a98a:	bd10      	pop	{r4, pc}
   3a98c:	4603      	mov	r3, r0
   3a98e:	e7f7      	b.n	3a980 <__any_on+0x26>
   3a990:	f853 1d04 	ldr.w	r1, [r3, #-4]!
   3a994:	2900      	cmp	r1, #0
   3a996:	d0f5      	beq.n	3a984 <__any_on+0x2a>
   3a998:	2001      	movs	r0, #1
   3a99a:	e7f6      	b.n	3a98a <__any_on+0x30>

0003a99c <_read_r>:
   3a99c:	b538      	push	{r3, r4, r5, lr}
   3a99e:	4604      	mov	r4, r0
   3a9a0:	4d06      	ldr	r5, [pc, #24]	; (3a9bc <_read_r+0x20>)
   3a9a2:	4608      	mov	r0, r1
   3a9a4:	4611      	mov	r1, r2
   3a9a6:	2200      	movs	r2, #0
   3a9a8:	602a      	str	r2, [r5, #0]
   3a9aa:	461a      	mov	r2, r3
   3a9ac:	f7f8 fd42 	bl	33434 <_read>
   3a9b0:	1c43      	adds	r3, r0, #1
   3a9b2:	d102      	bne.n	3a9ba <_read_r+0x1e>
   3a9b4:	682b      	ldr	r3, [r5, #0]
   3a9b6:	b103      	cbz	r3, 3a9ba <_read_r+0x1e>
   3a9b8:	6023      	str	r3, [r4, #0]
   3a9ba:	bd38      	pop	{r3, r4, r5, pc}
   3a9bc:	20006a0c 	.word	0x20006a0c

0003a9c0 <realloc>:
   3a9c0:	4b02      	ldr	r3, [pc, #8]	; (3a9cc <realloc+0xc>)
   3a9c2:	460a      	mov	r2, r1
   3a9c4:	4601      	mov	r1, r0
   3a9c6:	6818      	ldr	r0, [r3, #0]
   3a9c8:	f000 b802 	b.w	3a9d0 <_realloc_r>
   3a9cc:	20006000 	.word	0x20006000

0003a9d0 <_realloc_r>:
   3a9d0:	e92d 4ff7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3a9d4:	4681      	mov	r9, r0
   3a9d6:	460c      	mov	r4, r1
   3a9d8:	b929      	cbnz	r1, 3a9e6 <_realloc_r+0x16>
   3a9da:	4611      	mov	r1, r2
   3a9dc:	b003      	add	sp, #12
   3a9de:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3a9e2:	f7fe bf6d 	b.w	398c0 <_malloc_r>
   3a9e6:	9201      	str	r2, [sp, #4]
   3a9e8:	f7ff fb3a 	bl	3a060 <__malloc_lock>
   3a9ec:	9a01      	ldr	r2, [sp, #4]
   3a9ee:	f102 080b 	add.w	r8, r2, #11
   3a9f2:	f1b8 0f16 	cmp.w	r8, #22
   3a9f6:	d90b      	bls.n	3aa10 <_realloc_r+0x40>
   3a9f8:	f038 0807 	bics.w	r8, r8, #7
   3a9fc:	d50a      	bpl.n	3aa14 <_realloc_r+0x44>
   3a9fe:	230c      	movs	r3, #12
   3aa00:	f04f 0b00 	mov.w	fp, #0
   3aa04:	f8c9 3000 	str.w	r3, [r9]
   3aa08:	4658      	mov	r0, fp
   3aa0a:	b003      	add	sp, #12
   3aa0c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3aa10:	f04f 0810 	mov.w	r8, #16
   3aa14:	4590      	cmp	r8, r2
   3aa16:	d3f2      	bcc.n	3a9fe <_realloc_r+0x2e>
   3aa18:	f854 5c04 	ldr.w	r5, [r4, #-4]
   3aa1c:	f1a4 0a08 	sub.w	sl, r4, #8
   3aa20:	f025 0603 	bic.w	r6, r5, #3
   3aa24:	45b0      	cmp	r8, r6
   3aa26:	f340 8176 	ble.w	3ad16 <_realloc_r+0x346>
   3aa2a:	499e      	ldr	r1, [pc, #632]	; (3aca4 <_realloc_r+0x2d4>)
   3aa2c:	eb0a 0306 	add.w	r3, sl, r6
   3aa30:	f8d1 c008 	ldr.w	ip, [r1, #8]
   3aa34:	6859      	ldr	r1, [r3, #4]
   3aa36:	459c      	cmp	ip, r3
   3aa38:	d005      	beq.n	3aa46 <_realloc_r+0x76>
   3aa3a:	f021 0001 	bic.w	r0, r1, #1
   3aa3e:	4418      	add	r0, r3
   3aa40:	6840      	ldr	r0, [r0, #4]
   3aa42:	07c7      	lsls	r7, r0, #31
   3aa44:	d427      	bmi.n	3aa96 <_realloc_r+0xc6>
   3aa46:	f021 0103 	bic.w	r1, r1, #3
   3aa4a:	459c      	cmp	ip, r3
   3aa4c:	eb06 0701 	add.w	r7, r6, r1
   3aa50:	d119      	bne.n	3aa86 <_realloc_r+0xb6>
   3aa52:	f108 0010 	add.w	r0, r8, #16
   3aa56:	42b8      	cmp	r0, r7
   3aa58:	dc1f      	bgt.n	3aa9a <_realloc_r+0xca>
   3aa5a:	eba7 0708 	sub.w	r7, r7, r8
   3aa5e:	eb0a 0308 	add.w	r3, sl, r8
   3aa62:	4a90      	ldr	r2, [pc, #576]	; (3aca4 <_realloc_r+0x2d4>)
   3aa64:	4648      	mov	r0, r9
   3aa66:	f047 0701 	orr.w	r7, r7, #1
   3aa6a:	46a3      	mov	fp, r4
   3aa6c:	6093      	str	r3, [r2, #8]
   3aa6e:	605f      	str	r7, [r3, #4]
   3aa70:	f854 3c04 	ldr.w	r3, [r4, #-4]
   3aa74:	f003 0301 	and.w	r3, r3, #1
   3aa78:	ea43 0308 	orr.w	r3, r3, r8
   3aa7c:	f844 3c04 	str.w	r3, [r4, #-4]
   3aa80:	f7ff faf4 	bl	3a06c <__malloc_unlock>
   3aa84:	e7c0      	b.n	3aa08 <_realloc_r+0x38>
   3aa86:	45b8      	cmp	r8, r7
   3aa88:	dc07      	bgt.n	3aa9a <_realloc_r+0xca>
   3aa8a:	e9d3 3202 	ldrd	r3, r2, [r3, #8]
   3aa8e:	60da      	str	r2, [r3, #12]
   3aa90:	6093      	str	r3, [r2, #8]
   3aa92:	4655      	mov	r5, sl
   3aa94:	e080      	b.n	3ab98 <_realloc_r+0x1c8>
   3aa96:	2100      	movs	r1, #0
   3aa98:	460b      	mov	r3, r1
   3aa9a:	07e8      	lsls	r0, r5, #31
   3aa9c:	f100 80ea 	bmi.w	3ac74 <_realloc_r+0x2a4>
   3aaa0:	f854 5c08 	ldr.w	r5, [r4, #-8]
   3aaa4:	ebaa 0505 	sub.w	r5, sl, r5
   3aaa8:	6868      	ldr	r0, [r5, #4]
   3aaaa:	f020 0003 	bic.w	r0, r0, #3
   3aaae:	eb00 0b06 	add.w	fp, r0, r6
   3aab2:	2b00      	cmp	r3, #0
   3aab4:	f000 80a8 	beq.w	3ac08 <_realloc_r+0x238>
   3aab8:	459c      	cmp	ip, r3
   3aaba:	eb01 070b 	add.w	r7, r1, fp
   3aabe:	d14b      	bne.n	3ab58 <_realloc_r+0x188>
   3aac0:	f108 0310 	add.w	r3, r8, #16
   3aac4:	42bb      	cmp	r3, r7
   3aac6:	f300 809f 	bgt.w	3ac08 <_realloc_r+0x238>
   3aaca:	46ab      	mov	fp, r5
   3aacc:	68eb      	ldr	r3, [r5, #12]
   3aace:	f85b 2f08 	ldr.w	r2, [fp, #8]!
   3aad2:	60d3      	str	r3, [r2, #12]
   3aad4:	609a      	str	r2, [r3, #8]
   3aad6:	1f32      	subs	r2, r6, #4
   3aad8:	2a24      	cmp	r2, #36	; 0x24
   3aada:	d838      	bhi.n	3ab4e <_realloc_r+0x17e>
   3aadc:	2a13      	cmp	r2, #19
   3aade:	d934      	bls.n	3ab4a <_realloc_r+0x17a>
   3aae0:	6823      	ldr	r3, [r4, #0]
   3aae2:	2a1b      	cmp	r2, #27
   3aae4:	60ab      	str	r3, [r5, #8]
   3aae6:	6863      	ldr	r3, [r4, #4]
   3aae8:	60eb      	str	r3, [r5, #12]
   3aaea:	d81b      	bhi.n	3ab24 <_realloc_r+0x154>
   3aaec:	3408      	adds	r4, #8
   3aaee:	f105 0310 	add.w	r3, r5, #16
   3aaf2:	6822      	ldr	r2, [r4, #0]
   3aaf4:	601a      	str	r2, [r3, #0]
   3aaf6:	6862      	ldr	r2, [r4, #4]
   3aaf8:	605a      	str	r2, [r3, #4]
   3aafa:	68a2      	ldr	r2, [r4, #8]
   3aafc:	609a      	str	r2, [r3, #8]
   3aafe:	eba7 0708 	sub.w	r7, r7, r8
   3ab02:	eb05 0308 	add.w	r3, r5, r8
   3ab06:	4a67      	ldr	r2, [pc, #412]	; (3aca4 <_realloc_r+0x2d4>)
   3ab08:	f047 0701 	orr.w	r7, r7, #1
   3ab0c:	6093      	str	r3, [r2, #8]
   3ab0e:	605f      	str	r7, [r3, #4]
   3ab10:	686b      	ldr	r3, [r5, #4]
   3ab12:	f003 0301 	and.w	r3, r3, #1
   3ab16:	ea43 0308 	orr.w	r3, r3, r8
   3ab1a:	606b      	str	r3, [r5, #4]
   3ab1c:	4648      	mov	r0, r9
   3ab1e:	f7ff faa5 	bl	3a06c <__malloc_unlock>
   3ab22:	e771      	b.n	3aa08 <_realloc_r+0x38>
   3ab24:	68a3      	ldr	r3, [r4, #8]
   3ab26:	2a24      	cmp	r2, #36	; 0x24
   3ab28:	612b      	str	r3, [r5, #16]
   3ab2a:	68e3      	ldr	r3, [r4, #12]
   3ab2c:	bf18      	it	ne
   3ab2e:	3410      	addne	r4, #16
   3ab30:	616b      	str	r3, [r5, #20]
   3ab32:	bf11      	iteee	ne
   3ab34:	f105 0318 	addne.w	r3, r5, #24
   3ab38:	6923      	ldreq	r3, [r4, #16]
   3ab3a:	61ab      	streq	r3, [r5, #24]
   3ab3c:	f105 0320 	addeq.w	r3, r5, #32
   3ab40:	bf02      	ittt	eq
   3ab42:	6962      	ldreq	r2, [r4, #20]
   3ab44:	3418      	addeq	r4, #24
   3ab46:	61ea      	streq	r2, [r5, #28]
   3ab48:	e7d3      	b.n	3aaf2 <_realloc_r+0x122>
   3ab4a:	465b      	mov	r3, fp
   3ab4c:	e7d1      	b.n	3aaf2 <_realloc_r+0x122>
   3ab4e:	4621      	mov	r1, r4
   3ab50:	4658      	mov	r0, fp
   3ab52:	f7ff f9f7 	bl	39f44 <memmove>
   3ab56:	e7d2      	b.n	3aafe <_realloc_r+0x12e>
   3ab58:	45b8      	cmp	r8, r7
   3ab5a:	dc55      	bgt.n	3ac08 <_realloc_r+0x238>
   3ab5c:	4628      	mov	r0, r5
   3ab5e:	e9d3 3202 	ldrd	r3, r2, [r3, #8]
   3ab62:	60da      	str	r2, [r3, #12]
   3ab64:	6093      	str	r3, [r2, #8]
   3ab66:	68eb      	ldr	r3, [r5, #12]
   3ab68:	f850 2f08 	ldr.w	r2, [r0, #8]!
   3ab6c:	60d3      	str	r3, [r2, #12]
   3ab6e:	609a      	str	r2, [r3, #8]
   3ab70:	1f32      	subs	r2, r6, #4
   3ab72:	2a24      	cmp	r2, #36	; 0x24
   3ab74:	d844      	bhi.n	3ac00 <_realloc_r+0x230>
   3ab76:	2a13      	cmp	r2, #19
   3ab78:	d908      	bls.n	3ab8c <_realloc_r+0x1bc>
   3ab7a:	6823      	ldr	r3, [r4, #0]
   3ab7c:	2a1b      	cmp	r2, #27
   3ab7e:	60ab      	str	r3, [r5, #8]
   3ab80:	6863      	ldr	r3, [r4, #4]
   3ab82:	60eb      	str	r3, [r5, #12]
   3ab84:	d828      	bhi.n	3abd8 <_realloc_r+0x208>
   3ab86:	3408      	adds	r4, #8
   3ab88:	f105 0010 	add.w	r0, r5, #16
   3ab8c:	6823      	ldr	r3, [r4, #0]
   3ab8e:	6003      	str	r3, [r0, #0]
   3ab90:	6863      	ldr	r3, [r4, #4]
   3ab92:	6043      	str	r3, [r0, #4]
   3ab94:	68a3      	ldr	r3, [r4, #8]
   3ab96:	6083      	str	r3, [r0, #8]
   3ab98:	eba7 0008 	sub.w	r0, r7, r8
   3ab9c:	686b      	ldr	r3, [r5, #4]
   3ab9e:	19ea      	adds	r2, r5, r7
   3aba0:	280f      	cmp	r0, #15
   3aba2:	f003 0301 	and.w	r3, r3, #1
   3aba6:	f240 80b8 	bls.w	3ad1a <_realloc_r+0x34a>
   3abaa:	eb05 0108 	add.w	r1, r5, r8
   3abae:	ea48 0303 	orr.w	r3, r8, r3
   3abb2:	f040 0001 	orr.w	r0, r0, #1
   3abb6:	606b      	str	r3, [r5, #4]
   3abb8:	3108      	adds	r1, #8
   3abba:	f841 0c04 	str.w	r0, [r1, #-4]
   3abbe:	4648      	mov	r0, r9
   3abc0:	6853      	ldr	r3, [r2, #4]
   3abc2:	f043 0301 	orr.w	r3, r3, #1
   3abc6:	6053      	str	r3, [r2, #4]
   3abc8:	f7fe fb66 	bl	39298 <_free_r>
   3abcc:	4648      	mov	r0, r9
   3abce:	f105 0b08 	add.w	fp, r5, #8
   3abd2:	f7ff fa4b 	bl	3a06c <__malloc_unlock>
   3abd6:	e717      	b.n	3aa08 <_realloc_r+0x38>
   3abd8:	68a3      	ldr	r3, [r4, #8]
   3abda:	2a24      	cmp	r2, #36	; 0x24
   3abdc:	612b      	str	r3, [r5, #16]
   3abde:	bf18      	it	ne
   3abe0:	f105 0018 	addne.w	r0, r5, #24
   3abe4:	68e3      	ldr	r3, [r4, #12]
   3abe6:	bf0c      	ite	eq
   3abe8:	f105 0020 	addeq.w	r0, r5, #32
   3abec:	3410      	addne	r4, #16
   3abee:	616b      	str	r3, [r5, #20]
   3abf0:	bf01      	itttt	eq
   3abf2:	6923      	ldreq	r3, [r4, #16]
   3abf4:	61ab      	streq	r3, [r5, #24]
   3abf6:	6963      	ldreq	r3, [r4, #20]
   3abf8:	3418      	addeq	r4, #24
   3abfa:	bf08      	it	eq
   3abfc:	61eb      	streq	r3, [r5, #28]
   3abfe:	e7c5      	b.n	3ab8c <_realloc_r+0x1bc>
   3ac00:	4621      	mov	r1, r4
   3ac02:	f7ff f99f 	bl	39f44 <memmove>
   3ac06:	e7c7      	b.n	3ab98 <_realloc_r+0x1c8>
   3ac08:	45d8      	cmp	r8, fp
   3ac0a:	dc33      	bgt.n	3ac74 <_realloc_r+0x2a4>
   3ac0c:	4628      	mov	r0, r5
   3ac0e:	68eb      	ldr	r3, [r5, #12]
   3ac10:	f850 2f08 	ldr.w	r2, [r0, #8]!
   3ac14:	60d3      	str	r3, [r2, #12]
   3ac16:	609a      	str	r2, [r3, #8]
   3ac18:	1f32      	subs	r2, r6, #4
   3ac1a:	2a24      	cmp	r2, #36	; 0x24
   3ac1c:	d826      	bhi.n	3ac6c <_realloc_r+0x29c>
   3ac1e:	2a13      	cmp	r2, #19
   3ac20:	d908      	bls.n	3ac34 <_realloc_r+0x264>
   3ac22:	6823      	ldr	r3, [r4, #0]
   3ac24:	2a1b      	cmp	r2, #27
   3ac26:	60ab      	str	r3, [r5, #8]
   3ac28:	6863      	ldr	r3, [r4, #4]
   3ac2a:	60eb      	str	r3, [r5, #12]
   3ac2c:	d80a      	bhi.n	3ac44 <_realloc_r+0x274>
   3ac2e:	3408      	adds	r4, #8
   3ac30:	f105 0010 	add.w	r0, r5, #16
   3ac34:	6823      	ldr	r3, [r4, #0]
   3ac36:	6003      	str	r3, [r0, #0]
   3ac38:	6863      	ldr	r3, [r4, #4]
   3ac3a:	6043      	str	r3, [r0, #4]
   3ac3c:	68a3      	ldr	r3, [r4, #8]
   3ac3e:	6083      	str	r3, [r0, #8]
   3ac40:	465f      	mov	r7, fp
   3ac42:	e7a9      	b.n	3ab98 <_realloc_r+0x1c8>
   3ac44:	68a3      	ldr	r3, [r4, #8]
   3ac46:	2a24      	cmp	r2, #36	; 0x24
   3ac48:	612b      	str	r3, [r5, #16]
   3ac4a:	bf18      	it	ne
   3ac4c:	f105 0018 	addne.w	r0, r5, #24
   3ac50:	68e3      	ldr	r3, [r4, #12]
   3ac52:	bf0c      	ite	eq
   3ac54:	f105 0020 	addeq.w	r0, r5, #32
   3ac58:	3410      	addne	r4, #16
   3ac5a:	616b      	str	r3, [r5, #20]
   3ac5c:	bf01      	itttt	eq
   3ac5e:	6923      	ldreq	r3, [r4, #16]
   3ac60:	61ab      	streq	r3, [r5, #24]
   3ac62:	6963      	ldreq	r3, [r4, #20]
   3ac64:	3418      	addeq	r4, #24
   3ac66:	bf08      	it	eq
   3ac68:	61eb      	streq	r3, [r5, #28]
   3ac6a:	e7e3      	b.n	3ac34 <_realloc_r+0x264>
   3ac6c:	4621      	mov	r1, r4
   3ac6e:	f7ff f969 	bl	39f44 <memmove>
   3ac72:	e7e5      	b.n	3ac40 <_realloc_r+0x270>
   3ac74:	4611      	mov	r1, r2
   3ac76:	4648      	mov	r0, r9
   3ac78:	f7fe fe22 	bl	398c0 <_malloc_r>
   3ac7c:	4683      	mov	fp, r0
   3ac7e:	2800      	cmp	r0, #0
   3ac80:	f43f af4c 	beq.w	3ab1c <_realloc_r+0x14c>
   3ac84:	f854 3c04 	ldr.w	r3, [r4, #-4]
   3ac88:	f1a0 0208 	sub.w	r2, r0, #8
   3ac8c:	f023 0301 	bic.w	r3, r3, #1
   3ac90:	4453      	add	r3, sl
   3ac92:	4293      	cmp	r3, r2
   3ac94:	d108      	bne.n	3aca8 <_realloc_r+0x2d8>
   3ac96:	f850 7c04 	ldr.w	r7, [r0, #-4]
   3ac9a:	f027 0703 	bic.w	r7, r7, #3
   3ac9e:	4437      	add	r7, r6
   3aca0:	e6f7      	b.n	3aa92 <_realloc_r+0xc2>
   3aca2:	bf00      	nop
   3aca4:	2000659c 	.word	0x2000659c
   3aca8:	1f32      	subs	r2, r6, #4
   3acaa:	2a24      	cmp	r2, #36	; 0x24
   3acac:	d82f      	bhi.n	3ad0e <_realloc_r+0x33e>
   3acae:	2a13      	cmp	r2, #19
   3acb0:	d92a      	bls.n	3ad08 <_realloc_r+0x338>
   3acb2:	6823      	ldr	r3, [r4, #0]
   3acb4:	2a1b      	cmp	r2, #27
   3acb6:	6003      	str	r3, [r0, #0]
   3acb8:	6863      	ldr	r3, [r4, #4]
   3acba:	6043      	str	r3, [r0, #4]
   3acbc:	d80e      	bhi.n	3acdc <_realloc_r+0x30c>
   3acbe:	f104 0208 	add.w	r2, r4, #8
   3acc2:	f100 0308 	add.w	r3, r0, #8
   3acc6:	6811      	ldr	r1, [r2, #0]
   3acc8:	6019      	str	r1, [r3, #0]
   3acca:	6851      	ldr	r1, [r2, #4]
   3accc:	6059      	str	r1, [r3, #4]
   3acce:	6892      	ldr	r2, [r2, #8]
   3acd0:	609a      	str	r2, [r3, #8]
   3acd2:	4621      	mov	r1, r4
   3acd4:	4648      	mov	r0, r9
   3acd6:	f7fe fadf 	bl	39298 <_free_r>
   3acda:	e71f      	b.n	3ab1c <_realloc_r+0x14c>
   3acdc:	68a3      	ldr	r3, [r4, #8]
   3acde:	2a24      	cmp	r2, #36	; 0x24
   3ace0:	6083      	str	r3, [r0, #8]
   3ace2:	bf18      	it	ne
   3ace4:	f104 0210 	addne.w	r2, r4, #16
   3ace8:	68e3      	ldr	r3, [r4, #12]
   3acea:	bf08      	it	eq
   3acec:	f104 0218 	addeq.w	r2, r4, #24
   3acf0:	60c3      	str	r3, [r0, #12]
   3acf2:	bf11      	iteee	ne
   3acf4:	f100 0310 	addne.w	r3, r0, #16
   3acf8:	6923      	ldreq	r3, [r4, #16]
   3acfa:	6103      	streq	r3, [r0, #16]
   3acfc:	f100 0318 	addeq.w	r3, r0, #24
   3ad00:	bf04      	itt	eq
   3ad02:	6961      	ldreq	r1, [r4, #20]
   3ad04:	6141      	streq	r1, [r0, #20]
   3ad06:	e7de      	b.n	3acc6 <_realloc_r+0x2f6>
   3ad08:	4603      	mov	r3, r0
   3ad0a:	4622      	mov	r2, r4
   3ad0c:	e7db      	b.n	3acc6 <_realloc_r+0x2f6>
   3ad0e:	4621      	mov	r1, r4
   3ad10:	f7ff f918 	bl	39f44 <memmove>
   3ad14:	e7dd      	b.n	3acd2 <_realloc_r+0x302>
   3ad16:	4637      	mov	r7, r6
   3ad18:	e6bb      	b.n	3aa92 <_realloc_r+0xc2>
   3ad1a:	431f      	orrs	r7, r3
   3ad1c:	606f      	str	r7, [r5, #4]
   3ad1e:	6853      	ldr	r3, [r2, #4]
   3ad20:	f043 0301 	orr.w	r3, r3, #1
   3ad24:	6053      	str	r3, [r2, #4]
   3ad26:	e751      	b.n	3abcc <_realloc_r+0x1fc>

0003ad28 <cleanup_glue>:
   3ad28:	b538      	push	{r3, r4, r5, lr}
   3ad2a:	460c      	mov	r4, r1
   3ad2c:	6809      	ldr	r1, [r1, #0]
   3ad2e:	4605      	mov	r5, r0
   3ad30:	b109      	cbz	r1, 3ad36 <cleanup_glue+0xe>
   3ad32:	f7ff fff9 	bl	3ad28 <cleanup_glue>
   3ad36:	4621      	mov	r1, r4
   3ad38:	4628      	mov	r0, r5
   3ad3a:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
   3ad3e:	f7fe baab 	b.w	39298 <_free_r>

0003ad42 <_reclaim_reent>:
   3ad42:	4b20      	ldr	r3, [pc, #128]	; (3adc4 <_reclaim_reent+0x82>)
   3ad44:	681b      	ldr	r3, [r3, #0]
   3ad46:	4283      	cmp	r3, r0
   3ad48:	b570      	push	{r4, r5, r6, lr}
   3ad4a:	4604      	mov	r4, r0
   3ad4c:	d039      	beq.n	3adc2 <_reclaim_reent+0x80>
   3ad4e:	6cc3      	ldr	r3, [r0, #76]	; 0x4c
   3ad50:	bb33      	cbnz	r3, 3ada0 <_reclaim_reent+0x5e>
   3ad52:	6c21      	ldr	r1, [r4, #64]	; 0x40
   3ad54:	b111      	cbz	r1, 3ad5c <_reclaim_reent+0x1a>
   3ad56:	4620      	mov	r0, r4
   3ad58:	f7fe fa9e 	bl	39298 <_free_r>
   3ad5c:	f8d4 5148 	ldr.w	r5, [r4, #328]	; 0x148
   3ad60:	bb05      	cbnz	r5, 3ada4 <_reclaim_reent+0x62>
   3ad62:	6d61      	ldr	r1, [r4, #84]	; 0x54
   3ad64:	bb49      	cbnz	r1, 3adba <_reclaim_reent+0x78>
   3ad66:	6ba3      	ldr	r3, [r4, #56]	; 0x38
   3ad68:	b35b      	cbz	r3, 3adc2 <_reclaim_reent+0x80>
   3ad6a:	6be3      	ldr	r3, [r4, #60]	; 0x3c
   3ad6c:	4620      	mov	r0, r4
   3ad6e:	4798      	blx	r3
   3ad70:	f8d4 12e0 	ldr.w	r1, [r4, #736]	; 0x2e0
   3ad74:	b329      	cbz	r1, 3adc2 <_reclaim_reent+0x80>
   3ad76:	4620      	mov	r0, r4
   3ad78:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
   3ad7c:	f7ff bfd4 	b.w	3ad28 <cleanup_glue>
   3ad80:	5949      	ldr	r1, [r1, r5]
   3ad82:	b939      	cbnz	r1, 3ad94 <_reclaim_reent+0x52>
   3ad84:	3504      	adds	r5, #4
   3ad86:	2d80      	cmp	r5, #128	; 0x80
   3ad88:	6ce1      	ldr	r1, [r4, #76]	; 0x4c
   3ad8a:	d1f9      	bne.n	3ad80 <_reclaim_reent+0x3e>
   3ad8c:	4620      	mov	r0, r4
   3ad8e:	f7fe fa83 	bl	39298 <_free_r>
   3ad92:	e7de      	b.n	3ad52 <_reclaim_reent+0x10>
   3ad94:	680e      	ldr	r6, [r1, #0]
   3ad96:	4620      	mov	r0, r4
   3ad98:	f7fe fa7e 	bl	39298 <_free_r>
   3ad9c:	4631      	mov	r1, r6
   3ad9e:	e7f0      	b.n	3ad82 <_reclaim_reent+0x40>
   3ada0:	2500      	movs	r5, #0
   3ada2:	e7f0      	b.n	3ad86 <_reclaim_reent+0x44>
   3ada4:	f504 76a6 	add.w	r6, r4, #332	; 0x14c
   3ada8:	42b5      	cmp	r5, r6
   3adaa:	d0da      	beq.n	3ad62 <_reclaim_reent+0x20>
   3adac:	4629      	mov	r1, r5
   3adae:	682d      	ldr	r5, [r5, #0]
   3adb0:	4620      	mov	r0, r4
   3adb2:	f7fe fa71 	bl	39298 <_free_r>
   3adb6:	42ae      	cmp	r6, r5
   3adb8:	e7f7      	b.n	3adaa <_reclaim_reent+0x68>
   3adba:	4620      	mov	r0, r4
   3adbc:	f7fe fa6c 	bl	39298 <_free_r>
   3adc0:	e7d1      	b.n	3ad66 <_reclaim_reent+0x24>
   3adc2:	bd70      	pop	{r4, r5, r6, pc}
   3adc4:	20006000 	.word	0x20006000

0003adc8 <_sbrk_r>:
   3adc8:	b538      	push	{r3, r4, r5, lr}
   3adca:	2300      	movs	r3, #0
   3adcc:	4d05      	ldr	r5, [pc, #20]	; (3ade4 <_sbrk_r+0x1c>)
   3adce:	4604      	mov	r4, r0
   3add0:	4608      	mov	r0, r1
   3add2:	602b      	str	r3, [r5, #0]
   3add4:	f005 face 	bl	40374 <_sbrk>
   3add8:	1c43      	adds	r3, r0, #1
   3adda:	d102      	bne.n	3ade2 <_sbrk_r+0x1a>
   3addc:	682b      	ldr	r3, [r5, #0]
   3adde:	b103      	cbz	r3, 3ade2 <_sbrk_r+0x1a>
   3ade0:	6023      	str	r3, [r4, #0]
   3ade2:	bd38      	pop	{r3, r4, r5, pc}
   3ade4:	20006a0c 	.word	0x20006a0c

0003ade8 <__sccl>:
   3ade8:	b570      	push	{r4, r5, r6, lr}
   3adea:	780b      	ldrb	r3, [r1, #0]
   3adec:	4604      	mov	r4, r0
   3adee:	2b5e      	cmp	r3, #94	; 0x5e
   3adf0:	f104 05ff 	add.w	r5, r4, #255	; 0xff
   3adf4:	bf13      	iteet	ne
   3adf6:	1c48      	addne	r0, r1, #1
   3adf8:	1c88      	addeq	r0, r1, #2
   3adfa:	784b      	ldrbeq	r3, [r1, #1]
   3adfc:	2200      	movne	r2, #0
   3adfe:	bf08      	it	eq
   3ae00:	2201      	moveq	r2, #1
   3ae02:	1e61      	subs	r1, r4, #1
   3ae04:	f801 2f01 	strb.w	r2, [r1, #1]!
   3ae08:	42a9      	cmp	r1, r5
   3ae0a:	d1fb      	bne.n	3ae04 <__sccl+0x1c>
   3ae0c:	b90b      	cbnz	r3, 3ae12 <__sccl+0x2a>
   3ae0e:	3801      	subs	r0, #1
   3ae10:	bd70      	pop	{r4, r5, r6, pc}
   3ae12:	f082 0201 	eor.w	r2, r2, #1
   3ae16:	4605      	mov	r5, r0
   3ae18:	54e2      	strb	r2, [r4, r3]
   3ae1a:	4628      	mov	r0, r5
   3ae1c:	f810 1b01 	ldrb.w	r1, [r0], #1
   3ae20:	292d      	cmp	r1, #45	; 0x2d
   3ae22:	d006      	beq.n	3ae32 <__sccl+0x4a>
   3ae24:	295d      	cmp	r1, #93	; 0x5d
   3ae26:	d0f3      	beq.n	3ae10 <__sccl+0x28>
   3ae28:	b909      	cbnz	r1, 3ae2e <__sccl+0x46>
   3ae2a:	4628      	mov	r0, r5
   3ae2c:	e7f0      	b.n	3ae10 <__sccl+0x28>
   3ae2e:	460b      	mov	r3, r1
   3ae30:	e7f1      	b.n	3ae16 <__sccl+0x2e>
   3ae32:	786e      	ldrb	r6, [r5, #1]
   3ae34:	2e5d      	cmp	r6, #93	; 0x5d
   3ae36:	d0fa      	beq.n	3ae2e <__sccl+0x46>
   3ae38:	42b3      	cmp	r3, r6
   3ae3a:	dcf8      	bgt.n	3ae2e <__sccl+0x46>
   3ae3c:	3502      	adds	r5, #2
   3ae3e:	4619      	mov	r1, r3
   3ae40:	3101      	adds	r1, #1
   3ae42:	428e      	cmp	r6, r1
   3ae44:	5462      	strb	r2, [r4, r1]
   3ae46:	dcfb      	bgt.n	3ae40 <__sccl+0x58>
   3ae48:	1af1      	subs	r1, r6, r3
   3ae4a:	1c58      	adds	r0, r3, #1
   3ae4c:	3901      	subs	r1, #1
   3ae4e:	42b3      	cmp	r3, r6
   3ae50:	bfa8      	it	ge
   3ae52:	2100      	movge	r1, #0
   3ae54:	1843      	adds	r3, r0, r1
   3ae56:	e7e0      	b.n	3ae1a <__sccl+0x32>

0003ae58 <nanf>:
   3ae58:	ed9f 0a01 	vldr	s0, [pc, #4]	; 3ae60 <nanf+0x8>
   3ae5c:	4770      	bx	lr
   3ae5e:	bf00      	nop
   3ae60:	7fc00000 	.word	0x7fc00000

0003ae64 <frexp>:
   3ae64:	2100      	movs	r1, #0
   3ae66:	b570      	push	{r4, r5, r6, lr}
   3ae68:	ec55 4b10 	vmov	r4, r5, d0
   3ae6c:	f025 4200 	bic.w	r2, r5, #2147483648	; 0x80000000
   3ae70:	6001      	str	r1, [r0, #0]
   3ae72:	4915      	ldr	r1, [pc, #84]	; (3aec8 <frexp+0x64>)
   3ae74:	4606      	mov	r6, r0
   3ae76:	462b      	mov	r3, r5
   3ae78:	428a      	cmp	r2, r1
   3ae7a:	dc22      	bgt.n	3aec2 <frexp+0x5e>
   3ae7c:	ee10 1a10 	vmov	r1, s0
   3ae80:	4311      	orrs	r1, r2
   3ae82:	d01e      	beq.n	3aec2 <frexp+0x5e>
   3ae84:	4911      	ldr	r1, [pc, #68]	; (3aecc <frexp+0x68>)
   3ae86:	4029      	ands	r1, r5
   3ae88:	b969      	cbnz	r1, 3aea6 <frexp+0x42>
   3ae8a:	2200      	movs	r2, #0
   3ae8c:	4b10      	ldr	r3, [pc, #64]	; (3aed0 <frexp+0x6c>)
   3ae8e:	ee10 0a10 	vmov	r0, s0
   3ae92:	4629      	mov	r1, r5
   3ae94:	f004 fd52 	bl	3f93c <__aeabi_dmul>
   3ae98:	460b      	mov	r3, r1
   3ae9a:	f021 4200 	bic.w	r2, r1, #2147483648	; 0x80000000
   3ae9e:	4604      	mov	r4, r0
   3aea0:	f06f 0135 	mvn.w	r1, #53	; 0x35
   3aea4:	6031      	str	r1, [r6, #0]
   3aea6:	f023 43ff 	bic.w	r3, r3, #2139095040	; 0x7f800000
   3aeaa:	1512      	asrs	r2, r2, #20
   3aeac:	6831      	ldr	r1, [r6, #0]
   3aeae:	f423 03e0 	bic.w	r3, r3, #7340032	; 0x700000
   3aeb2:	f2a2 32fe 	subw	r2, r2, #1022	; 0x3fe
   3aeb6:	f043 557f 	orr.w	r5, r3, #1069547520	; 0x3fc00000
   3aeba:	440a      	add	r2, r1
   3aebc:	f445 1500 	orr.w	r5, r5, #2097152	; 0x200000
   3aec0:	6032      	str	r2, [r6, #0]
   3aec2:	ec45 4b10 	vmov	d0, r4, r5
   3aec6:	bd70      	pop	{r4, r5, r6, pc}
   3aec8:	7fefffff 	.word	0x7fefffff
   3aecc:	7ff00000 	.word	0x7ff00000
   3aed0:	43500000 	.word	0x43500000

0003aed4 <strcmp>:
   3aed4:	ea80 0c01 	eor.w	ip, r0, r1
   3aed8:	f01c 0f03 	tst.w	ip, #3
   3aedc:	d137      	bne.n	3af4e <strcmp+0x7a>
   3aede:	f010 0c03 	ands.w	ip, r0, #3
   3aee2:	f020 0003 	bic.w	r0, r0, #3
   3aee6:	f021 0103 	bic.w	r1, r1, #3
   3aeea:	f850 2b04 	ldr.w	r2, [r0], #4
   3aeee:	bf08      	it	eq
   3aef0:	f851 3b04 	ldreq.w	r3, [r1], #4
   3aef4:	d00e      	beq.n	3af14 <strcmp+0x40>
   3aef6:	f08c 0c03 	eor.w	ip, ip, #3
   3aefa:	f06f 437f 	mvn.w	r3, #4278190080	; 0xff000000
   3aefe:	ea4f 0ccc 	mov.w	ip, ip, lsl #3
   3af02:	fa23 fc0c 	lsr.w	ip, r3, ip
   3af06:	f851 3b04 	ldr.w	r3, [r1], #4
   3af0a:	ea42 020c 	orr.w	r2, r2, ip
   3af0e:	ea43 030c 	orr.w	r3, r3, ip
   3af12:	bf00      	nop
   3af14:	f1a2 3c01 	sub.w	ip, r2, #16843009	; 0x1010101
   3af18:	429a      	cmp	r2, r3
   3af1a:	bf01      	itttt	eq
   3af1c:	ea2c 0c02 	biceq.w	ip, ip, r2
   3af20:	f01c 3f80 	tsteq.w	ip, #2155905152	; 0x80808080
   3af24:	f850 2b04 	ldreq.w	r2, [r0], #4
   3af28:	f851 3b04 	ldreq.w	r3, [r1], #4
   3af2c:	d0f2      	beq.n	3af14 <strcmp+0x40>
   3af2e:	ea4f 6002 	mov.w	r0, r2, lsl #24
   3af32:	ea4f 2212 	mov.w	r2, r2, lsr #8
   3af36:	2801      	cmp	r0, #1
   3af38:	bf28      	it	cs
   3af3a:	ebb0 6f03 	cmpcs.w	r0, r3, lsl #24
   3af3e:	bf08      	it	eq
   3af40:	0a1b      	lsreq	r3, r3, #8
   3af42:	d0f4      	beq.n	3af2e <strcmp+0x5a>
   3af44:	f003 03ff 	and.w	r3, r3, #255	; 0xff
   3af48:	0e00      	lsrs	r0, r0, #24
   3af4a:	1ac0      	subs	r0, r0, r3
   3af4c:	4770      	bx	lr
   3af4e:	f010 0f03 	tst.w	r0, #3
   3af52:	d00a      	beq.n	3af6a <strcmp+0x96>
   3af54:	f810 2b01 	ldrb.w	r2, [r0], #1
   3af58:	f811 3b01 	ldrb.w	r3, [r1], #1
   3af5c:	2a01      	cmp	r2, #1
   3af5e:	bf28      	it	cs
   3af60:	429a      	cmpcs	r2, r3
   3af62:	d0f4      	beq.n	3af4e <strcmp+0x7a>
   3af64:	eba2 0003 	sub.w	r0, r2, r3
   3af68:	4770      	bx	lr
   3af6a:	f84d 5d04 	str.w	r5, [sp, #-4]!
   3af6e:	f850 2b04 	ldr.w	r2, [r0], #4
   3af72:	f001 0503 	and.w	r5, r1, #3
   3af76:	f021 0103 	bic.w	r1, r1, #3
   3af7a:	f851 3b04 	ldr.w	r3, [r1], #4
   3af7e:	2d02      	cmp	r5, #2
   3af80:	d026      	beq.n	3afd0 <strcmp+0xfc>
   3af82:	d84d      	bhi.n	3b020 <strcmp+0x14c>
   3af84:	f022 457f 	bic.w	r5, r2, #4278190080	; 0xff000000
   3af88:	ebb5 2f13 	cmp.w	r5, r3, lsr #8
   3af8c:	f1a2 3c01 	sub.w	ip, r2, #16843009	; 0x1010101
   3af90:	ea2c 0c02 	bic.w	ip, ip, r2
   3af94:	d10d      	bne.n	3afb2 <strcmp+0xde>
   3af96:	f01c 3c80 	ands.w	ip, ip, #2155905152	; 0x80808080
   3af9a:	bf08      	it	eq
   3af9c:	f851 3b04 	ldreq.w	r3, [r1], #4
   3afa0:	d10a      	bne.n	3afb8 <strcmp+0xe4>
   3afa2:	ea85 0502 	eor.w	r5, r5, r2
   3afa6:	ebb5 6f03 	cmp.w	r5, r3, lsl #24
   3afaa:	d10c      	bne.n	3afc6 <strcmp+0xf2>
   3afac:	f850 2b04 	ldr.w	r2, [r0], #4
   3afb0:	e7e8      	b.n	3af84 <strcmp+0xb0>
   3afb2:	ea4f 2313 	mov.w	r3, r3, lsr #8
   3afb6:	e05b      	b.n	3b070 <strcmp+0x19c>
   3afb8:	f03c 4c7f 	bics.w	ip, ip, #4278190080	; 0xff000000
   3afbc:	d154      	bne.n	3b068 <strcmp+0x194>
   3afbe:	780b      	ldrb	r3, [r1, #0]
   3afc0:	ea4f 6512 	mov.w	r5, r2, lsr #24
   3afc4:	e054      	b.n	3b070 <strcmp+0x19c>
   3afc6:	ea4f 6512 	mov.w	r5, r2, lsr #24
   3afca:	f003 03ff 	and.w	r3, r3, #255	; 0xff
   3afce:	e04f      	b.n	3b070 <strcmp+0x19c>
   3afd0:	ea4f 4502 	mov.w	r5, r2, lsl #16
   3afd4:	f1a2 3c01 	sub.w	ip, r2, #16843009	; 0x1010101
   3afd8:	ea4f 4515 	mov.w	r5, r5, lsr #16
   3afdc:	ea2c 0c02 	bic.w	ip, ip, r2
   3afe0:	ebb5 4f13 	cmp.w	r5, r3, lsr #16
   3afe4:	d118      	bne.n	3b018 <strcmp+0x144>
   3afe6:	f01c 3c80 	ands.w	ip, ip, #2155905152	; 0x80808080
   3afea:	bf08      	it	eq
   3afec:	f851 3b04 	ldreq.w	r3, [r1], #4
   3aff0:	d107      	bne.n	3b002 <strcmp+0x12e>
   3aff2:	ea85 0502 	eor.w	r5, r5, r2
   3aff6:	ebb5 4f03 	cmp.w	r5, r3, lsl #16
   3affa:	d109      	bne.n	3b010 <strcmp+0x13c>
   3affc:	f850 2b04 	ldr.w	r2, [r0], #4
   3b000:	e7e6      	b.n	3afd0 <strcmp+0xfc>
   3b002:	ea5f 4c0c 	movs.w	ip, ip, lsl #16
   3b006:	d12f      	bne.n	3b068 <strcmp+0x194>
   3b008:	880b      	ldrh	r3, [r1, #0]
   3b00a:	ea4f 4512 	mov.w	r5, r2, lsr #16
   3b00e:	e02f      	b.n	3b070 <strcmp+0x19c>
   3b010:	ea4f 4303 	mov.w	r3, r3, lsl #16
   3b014:	ea4f 4512 	mov.w	r5, r2, lsr #16
   3b018:	ea4f 4313 	mov.w	r3, r3, lsr #16
   3b01c:	e028      	b.n	3b070 <strcmp+0x19c>
   3b01e:	bf00      	nop
   3b020:	f002 05ff 	and.w	r5, r2, #255	; 0xff
   3b024:	ebb5 6f13 	cmp.w	r5, r3, lsr #24
   3b028:	f1a2 3c01 	sub.w	ip, r2, #16843009	; 0x1010101
   3b02c:	ea2c 0c02 	bic.w	ip, ip, r2
   3b030:	d10d      	bne.n	3b04e <strcmp+0x17a>
   3b032:	f01c 3c80 	ands.w	ip, ip, #2155905152	; 0x80808080
   3b036:	bf08      	it	eq
   3b038:	f851 3b04 	ldreq.w	r3, [r1], #4
   3b03c:	d10a      	bne.n	3b054 <strcmp+0x180>
   3b03e:	ea85 0502 	eor.w	r5, r5, r2
   3b042:	ebb5 2f03 	cmp.w	r5, r3, lsl #8
   3b046:	d10a      	bne.n	3b05e <strcmp+0x18a>
   3b048:	f850 2b04 	ldr.w	r2, [r0], #4
   3b04c:	e7e8      	b.n	3b020 <strcmp+0x14c>
   3b04e:	ea4f 6313 	mov.w	r3, r3, lsr #24
   3b052:	e00d      	b.n	3b070 <strcmp+0x19c>
   3b054:	f012 0fff 	tst.w	r2, #255	; 0xff
   3b058:	d006      	beq.n	3b068 <strcmp+0x194>
   3b05a:	f851 3b04 	ldr.w	r3, [r1], #4
   3b05e:	ea4f 2512 	mov.w	r5, r2, lsr #8
   3b062:	f023 437f 	bic.w	r3, r3, #4278190080	; 0xff000000
   3b066:	e003      	b.n	3b070 <strcmp+0x19c>
   3b068:	f04f 0000 	mov.w	r0, #0
   3b06c:	bc20      	pop	{r5}
   3b06e:	4770      	bx	lr
   3b070:	f005 02ff 	and.w	r2, r5, #255	; 0xff
   3b074:	f003 00ff 	and.w	r0, r3, #255	; 0xff
   3b078:	2801      	cmp	r0, #1
   3b07a:	bf28      	it	cs
   3b07c:	4290      	cmpcs	r0, r2
   3b07e:	bf04      	itt	eq
   3b080:	0a2d      	lsreq	r5, r5, #8
   3b082:	0a1b      	lsreq	r3, r3, #8
   3b084:	d0f4      	beq.n	3b070 <strcmp+0x19c>
   3b086:	eba2 0000 	sub.w	r0, r2, r0
   3b08a:	bc20      	pop	{r5}
   3b08c:	4770      	bx	lr
   3b08e:	bf00      	nop

0003b090 <strnlen>:
   3b090:	4602      	mov	r2, r0
   3b092:	4401      	add	r1, r0
   3b094:	b510      	push	{r4, lr}
   3b096:	428a      	cmp	r2, r1
   3b098:	4613      	mov	r3, r2
   3b09a:	d003      	beq.n	3b0a4 <strnlen+0x14>
   3b09c:	781c      	ldrb	r4, [r3, #0]
   3b09e:	3201      	adds	r2, #1
   3b0a0:	2c00      	cmp	r4, #0
   3b0a2:	d1f8      	bne.n	3b096 <strnlen+0x6>
   3b0a4:	1a18      	subs	r0, r3, r0
   3b0a6:	bd10      	pop	{r4, pc}

0003b0a8 <sulp>:
   3b0a8:	b570      	push	{r4, r5, r6, lr}
   3b0aa:	4604      	mov	r4, r0
   3b0ac:	460d      	mov	r5, r1
   3b0ae:	4616      	mov	r6, r2
   3b0b0:	ec45 4b10 	vmov	d0, r4, r5
   3b0b4:	f7ff fb08 	bl	3a6c8 <__ulp>
   3b0b8:	ec51 0b10 	vmov	r0, r1, d0
   3b0bc:	b17e      	cbz	r6, 3b0de <sulp+0x36>
   3b0be:	f3c5 530a 	ubfx	r3, r5, #20, #11
   3b0c2:	f1c3 036b 	rsb	r3, r3, #107	; 0x6b
   3b0c6:	2b00      	cmp	r3, #0
   3b0c8:	dd09      	ble.n	3b0de <sulp+0x36>
   3b0ca:	051b      	lsls	r3, r3, #20
   3b0cc:	2400      	movs	r4, #0
   3b0ce:	f103 557f 	add.w	r5, r3, #1069547520	; 0x3fc00000
   3b0d2:	4622      	mov	r2, r4
   3b0d4:	f505 1540 	add.w	r5, r5, #3145728	; 0x300000
   3b0d8:	462b      	mov	r3, r5
   3b0da:	f004 fc2f 	bl	3f93c <__aeabi_dmul>
   3b0de:	bd70      	pop	{r4, r5, r6, pc}

0003b0e0 <_strtod_l>:
   3b0e0:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3b0e4:	461f      	mov	r7, r3
   3b0e6:	2300      	movs	r3, #0
   3b0e8:	4604      	mov	r4, r0
   3b0ea:	4688      	mov	r8, r1
   3b0ec:	f04f 0a00 	mov.w	sl, #0
   3b0f0:	f04f 0b00 	mov.w	fp, #0
   3b0f4:	ed2d 8b02 	vpush	{d8}
   3b0f8:	b09d      	sub	sp, #116	; 0x74
   3b0fa:	9318      	str	r3, [sp, #96]	; 0x60
   3b0fc:	4b9f      	ldr	r3, [pc, #636]	; (3b37c <_strtod_l+0x29c>)
   3b0fe:	9213      	str	r2, [sp, #76]	; 0x4c
   3b100:	681b      	ldr	r3, [r3, #0]
   3b102:	4618      	mov	r0, r3
   3b104:	9305      	str	r3, [sp, #20]
   3b106:	f7f8 fe13 	bl	33d30 <strlen>
   3b10a:	4605      	mov	r5, r0
   3b10c:	f8cd 805c 	str.w	r8, [sp, #92]	; 0x5c
   3b110:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b112:	781a      	ldrb	r2, [r3, #0]
   3b114:	2a2b      	cmp	r2, #43	; 0x2b
   3b116:	d04e      	beq.n	3b1b6 <_strtod_l+0xd6>
   3b118:	d83b      	bhi.n	3b192 <_strtod_l+0xb2>
   3b11a:	2a0d      	cmp	r2, #13
   3b11c:	d834      	bhi.n	3b188 <_strtod_l+0xa8>
   3b11e:	2a08      	cmp	r2, #8
   3b120:	d834      	bhi.n	3b18c <_strtod_l+0xac>
   3b122:	2a00      	cmp	r2, #0
   3b124:	d03e      	beq.n	3b1a4 <_strtod_l+0xc4>
   3b126:	2300      	movs	r3, #0
   3b128:	930a      	str	r3, [sp, #40]	; 0x28
   3b12a:	9e17      	ldr	r6, [sp, #92]	; 0x5c
   3b12c:	7833      	ldrb	r3, [r6, #0]
   3b12e:	2b30      	cmp	r3, #48	; 0x30
   3b130:	f040 80b0 	bne.w	3b294 <_strtod_l+0x1b4>
   3b134:	7873      	ldrb	r3, [r6, #1]
   3b136:	f003 03df 	and.w	r3, r3, #223	; 0xdf
   3b13a:	2b58      	cmp	r3, #88	; 0x58
   3b13c:	d168      	bne.n	3b210 <_strtod_l+0x130>
   3b13e:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   3b140:	a917      	add	r1, sp, #92	; 0x5c
   3b142:	9702      	str	r7, [sp, #8]
   3b144:	4620      	mov	r0, r4
   3b146:	9301      	str	r3, [sp, #4]
   3b148:	ab18      	add	r3, sp, #96	; 0x60
   3b14a:	4a8d      	ldr	r2, [pc, #564]	; (3b380 <_strtod_l+0x2a0>)
   3b14c:	9300      	str	r3, [sp, #0]
   3b14e:	ab19      	add	r3, sp, #100	; 0x64
   3b150:	f003 fd08 	bl	3eb64 <__gethex>
   3b154:	f010 0707 	ands.w	r7, r0, #7
   3b158:	4605      	mov	r5, r0
   3b15a:	d005      	beq.n	3b168 <_strtod_l+0x88>
   3b15c:	2f06      	cmp	r7, #6
   3b15e:	d12c      	bne.n	3b1ba <_strtod_l+0xda>
   3b160:	3601      	adds	r6, #1
   3b162:	2300      	movs	r3, #0
   3b164:	9617      	str	r6, [sp, #92]	; 0x5c
   3b166:	930a      	str	r3, [sp, #40]	; 0x28
   3b168:	9b13      	ldr	r3, [sp, #76]	; 0x4c
   3b16a:	2b00      	cmp	r3, #0
   3b16c:	f040 858e 	bne.w	3bc8c <_strtod_l+0xbac>
   3b170:	9b0a      	ldr	r3, [sp, #40]	; 0x28
   3b172:	b1eb      	cbz	r3, 3b1b0 <_strtod_l+0xd0>
   3b174:	4652      	mov	r2, sl
   3b176:	f10b 4300 	add.w	r3, fp, #2147483648	; 0x80000000
   3b17a:	ec43 2b10 	vmov	d0, r2, r3
   3b17e:	b01d      	add	sp, #116	; 0x74
   3b180:	ecbd 8b02 	vpop	{d8}
   3b184:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3b188:	2a20      	cmp	r2, #32
   3b18a:	d1cc      	bne.n	3b126 <_strtod_l+0x46>
   3b18c:	3301      	adds	r3, #1
   3b18e:	9317      	str	r3, [sp, #92]	; 0x5c
   3b190:	e7be      	b.n	3b110 <_strtod_l+0x30>
   3b192:	2a2d      	cmp	r2, #45	; 0x2d
   3b194:	d1c7      	bne.n	3b126 <_strtod_l+0x46>
   3b196:	2201      	movs	r2, #1
   3b198:	920a      	str	r2, [sp, #40]	; 0x28
   3b19a:	1c5a      	adds	r2, r3, #1
   3b19c:	9217      	str	r2, [sp, #92]	; 0x5c
   3b19e:	785b      	ldrb	r3, [r3, #1]
   3b1a0:	2b00      	cmp	r3, #0
   3b1a2:	d1c2      	bne.n	3b12a <_strtod_l+0x4a>
   3b1a4:	9b13      	ldr	r3, [sp, #76]	; 0x4c
   3b1a6:	f8cd 805c 	str.w	r8, [sp, #92]	; 0x5c
   3b1aa:	2b00      	cmp	r3, #0
   3b1ac:	f040 856c 	bne.w	3bc88 <_strtod_l+0xba8>
   3b1b0:	4652      	mov	r2, sl
   3b1b2:	465b      	mov	r3, fp
   3b1b4:	e7e1      	b.n	3b17a <_strtod_l+0x9a>
   3b1b6:	2200      	movs	r2, #0
   3b1b8:	e7ee      	b.n	3b198 <_strtod_l+0xb8>
   3b1ba:	9a18      	ldr	r2, [sp, #96]	; 0x60
   3b1bc:	b13a      	cbz	r2, 3b1ce <_strtod_l+0xee>
   3b1be:	2135      	movs	r1, #53	; 0x35
   3b1c0:	a81a      	add	r0, sp, #104	; 0x68
   3b1c2:	f7ff fba7 	bl	3a914 <__copybits>
   3b1c6:	9918      	ldr	r1, [sp, #96]	; 0x60
   3b1c8:	4620      	mov	r0, r4
   3b1ca:	f7fe ff7a 	bl	3a0c2 <_Bfree>
   3b1ce:	3f01      	subs	r7, #1
   3b1d0:	2f04      	cmp	r7, #4
   3b1d2:	d806      	bhi.n	3b1e2 <_strtod_l+0x102>
   3b1d4:	e8df f007 	tbb	[pc, r7]
   3b1d8:	1714030a 	.word	0x1714030a
   3b1dc:	0a          	.byte	0x0a
   3b1dd:	00          	.byte	0x00
   3b1de:	e9dd ab1a 	ldrd	sl, fp, [sp, #104]	; 0x68
   3b1e2:	0728      	lsls	r0, r5, #28
   3b1e4:	d5c0      	bpl.n	3b168 <_strtod_l+0x88>
   3b1e6:	f04b 4b00 	orr.w	fp, fp, #2147483648	; 0x80000000
   3b1ea:	e7bd      	b.n	3b168 <_strtod_l+0x88>
   3b1ec:	e9dd a31a 	ldrd	sl, r3, [sp, #104]	; 0x68
   3b1f0:	9a19      	ldr	r2, [sp, #100]	; 0x64
   3b1f2:	f423 1380 	bic.w	r3, r3, #1048576	; 0x100000
   3b1f6:	f202 4233 	addw	r2, r2, #1075	; 0x433
   3b1fa:	ea43 5b02 	orr.w	fp, r3, r2, lsl #20
   3b1fe:	e7f0      	b.n	3b1e2 <_strtod_l+0x102>
   3b200:	f8df b180 	ldr.w	fp, [pc, #384]	; 3b384 <_strtod_l+0x2a4>
   3b204:	e7ed      	b.n	3b1e2 <_strtod_l+0x102>
   3b206:	f06f 4b00 	mvn.w	fp, #2147483648	; 0x80000000
   3b20a:	f04f 3aff 	mov.w	sl, #4294967295	; 0xffffffff
   3b20e:	e7e8      	b.n	3b1e2 <_strtod_l+0x102>
   3b210:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b212:	1c5a      	adds	r2, r3, #1
   3b214:	9217      	str	r2, [sp, #92]	; 0x5c
   3b216:	785b      	ldrb	r3, [r3, #1]
   3b218:	2b30      	cmp	r3, #48	; 0x30
   3b21a:	d0f9      	beq.n	3b210 <_strtod_l+0x130>
   3b21c:	2b00      	cmp	r3, #0
   3b21e:	d0a3      	beq.n	3b168 <_strtod_l+0x88>
   3b220:	2301      	movs	r3, #1
   3b222:	f04f 0900 	mov.w	r9, #0
   3b226:	9304      	str	r3, [sp, #16]
   3b228:	220a      	movs	r2, #10
   3b22a:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b22c:	464f      	mov	r7, r9
   3b22e:	f8cd 901c 	str.w	r9, [sp, #28]
   3b232:	9308      	str	r3, [sp, #32]
   3b234:	9817      	ldr	r0, [sp, #92]	; 0x5c
   3b236:	7806      	ldrb	r6, [r0, #0]
   3b238:	f1a6 0330 	sub.w	r3, r6, #48	; 0x30
   3b23c:	b2d9      	uxtb	r1, r3
   3b23e:	2909      	cmp	r1, #9
   3b240:	d92a      	bls.n	3b298 <_strtod_l+0x1b8>
   3b242:	462a      	mov	r2, r5
   3b244:	9905      	ldr	r1, [sp, #20]
   3b246:	f7f8 fdc2 	bl	33dce <strncmp>
   3b24a:	b3a0      	cbz	r0, 3b2b6 <_strtod_l+0x1d6>
   3b24c:	2000      	movs	r0, #0
   3b24e:	4632      	mov	r2, r6
   3b250:	463d      	mov	r5, r7
   3b252:	4603      	mov	r3, r0
   3b254:	9005      	str	r0, [sp, #20]
   3b256:	2a65      	cmp	r2, #101	; 0x65
   3b258:	d001      	beq.n	3b25e <_strtod_l+0x17e>
   3b25a:	2a45      	cmp	r2, #69	; 0x45
   3b25c:	d118      	bne.n	3b290 <_strtod_l+0x1b0>
   3b25e:	b91d      	cbnz	r5, 3b268 <_strtod_l+0x188>
   3b260:	9a04      	ldr	r2, [sp, #16]
   3b262:	4302      	orrs	r2, r0
   3b264:	d09e      	beq.n	3b1a4 <_strtod_l+0xc4>
   3b266:	2500      	movs	r5, #0
   3b268:	f8dd 805c 	ldr.w	r8, [sp, #92]	; 0x5c
   3b26c:	f108 0201 	add.w	r2, r8, #1
   3b270:	9217      	str	r2, [sp, #92]	; 0x5c
   3b272:	f898 2001 	ldrb.w	r2, [r8, #1]
   3b276:	2a2b      	cmp	r2, #43	; 0x2b
   3b278:	d075      	beq.n	3b366 <_strtod_l+0x286>
   3b27a:	2a2d      	cmp	r2, #45	; 0x2d
   3b27c:	d07b      	beq.n	3b376 <_strtod_l+0x296>
   3b27e:	f04f 0c00 	mov.w	ip, #0
   3b282:	f1a2 0130 	sub.w	r1, r2, #48	; 0x30
   3b286:	2909      	cmp	r1, #9
   3b288:	f240 8082 	bls.w	3b390 <_strtod_l+0x2b0>
   3b28c:	f8cd 805c 	str.w	r8, [sp, #92]	; 0x5c
   3b290:	2600      	movs	r6, #0
   3b292:	e09d      	b.n	3b3d0 <_strtod_l+0x2f0>
   3b294:	2300      	movs	r3, #0
   3b296:	e7c4      	b.n	3b222 <_strtod_l+0x142>
   3b298:	2f08      	cmp	r7, #8
   3b29a:	f100 0001 	add.w	r0, r0, #1
   3b29e:	f107 0701 	add.w	r7, r7, #1
   3b2a2:	bfd4      	ite	le
   3b2a4:	9907      	ldrle	r1, [sp, #28]
   3b2a6:	fb02 3909 	mlagt	r9, r2, r9, r3
   3b2aa:	9017      	str	r0, [sp, #92]	; 0x5c
   3b2ac:	bfdc      	itt	le
   3b2ae:	fb02 3301 	mlale	r3, r2, r1, r3
   3b2b2:	9307      	strle	r3, [sp, #28]
   3b2b4:	e7be      	b.n	3b234 <_strtod_l+0x154>
   3b2b6:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b2b8:	195a      	adds	r2, r3, r5
   3b2ba:	9217      	str	r2, [sp, #92]	; 0x5c
   3b2bc:	5d5a      	ldrb	r2, [r3, r5]
   3b2be:	2f00      	cmp	r7, #0
   3b2c0:	d036      	beq.n	3b330 <_strtod_l+0x250>
   3b2c2:	463d      	mov	r5, r7
   3b2c4:	9005      	str	r0, [sp, #20]
   3b2c6:	f1a2 0330 	sub.w	r3, r2, #48	; 0x30
   3b2ca:	2b09      	cmp	r3, #9
   3b2cc:	d912      	bls.n	3b2f4 <_strtod_l+0x214>
   3b2ce:	2301      	movs	r3, #1
   3b2d0:	e7c1      	b.n	3b256 <_strtod_l+0x176>
   3b2d2:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b2d4:	3001      	adds	r0, #1
   3b2d6:	1c5a      	adds	r2, r3, #1
   3b2d8:	9217      	str	r2, [sp, #92]	; 0x5c
   3b2da:	785a      	ldrb	r2, [r3, #1]
   3b2dc:	2a30      	cmp	r2, #48	; 0x30
   3b2de:	d0f8      	beq.n	3b2d2 <_strtod_l+0x1f2>
   3b2e0:	f1a2 0331 	sub.w	r3, r2, #49	; 0x31
   3b2e4:	2b08      	cmp	r3, #8
   3b2e6:	f200 84d6 	bhi.w	3bc96 <_strtod_l+0xbb6>
   3b2ea:	9005      	str	r0, [sp, #20]
   3b2ec:	2000      	movs	r0, #0
   3b2ee:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b2f0:	4605      	mov	r5, r0
   3b2f2:	9308      	str	r3, [sp, #32]
   3b2f4:	3a30      	subs	r2, #48	; 0x30
   3b2f6:	f100 0301 	add.w	r3, r0, #1
   3b2fa:	d013      	beq.n	3b324 <_strtod_l+0x244>
   3b2fc:	9905      	ldr	r1, [sp, #20]
   3b2fe:	eb00 0e05 	add.w	lr, r0, r5
   3b302:	4419      	add	r1, r3
   3b304:	462b      	mov	r3, r5
   3b306:	9105      	str	r1, [sp, #20]
   3b308:	210a      	movs	r1, #10
   3b30a:	4573      	cmp	r3, lr
   3b30c:	d112      	bne.n	3b334 <_strtod_l+0x254>
   3b30e:	182b      	adds	r3, r5, r0
   3b310:	3501      	adds	r5, #1
   3b312:	2b08      	cmp	r3, #8
   3b314:	4405      	add	r5, r0
   3b316:	dc1c      	bgt.n	3b352 <_strtod_l+0x272>
   3b318:	9907      	ldr	r1, [sp, #28]
   3b31a:	230a      	movs	r3, #10
   3b31c:	fb03 2301 	mla	r3, r3, r1, r2
   3b320:	9307      	str	r3, [sp, #28]
   3b322:	2300      	movs	r3, #0
   3b324:	9a17      	ldr	r2, [sp, #92]	; 0x5c
   3b326:	4618      	mov	r0, r3
   3b328:	1c51      	adds	r1, r2, #1
   3b32a:	9117      	str	r1, [sp, #92]	; 0x5c
   3b32c:	7852      	ldrb	r2, [r2, #1]
   3b32e:	e7ca      	b.n	3b2c6 <_strtod_l+0x1e6>
   3b330:	4638      	mov	r0, r7
   3b332:	e7d3      	b.n	3b2dc <_strtod_l+0x1fc>
   3b334:	2b08      	cmp	r3, #8
   3b336:	dc04      	bgt.n	3b342 <_strtod_l+0x262>
   3b338:	9e07      	ldr	r6, [sp, #28]
   3b33a:	434e      	muls	r6, r1
   3b33c:	9607      	str	r6, [sp, #28]
   3b33e:	3301      	adds	r3, #1
   3b340:	e7e3      	b.n	3b30a <_strtod_l+0x22a>
   3b342:	f103 0c01 	add.w	ip, r3, #1
   3b346:	f1bc 0f10 	cmp.w	ip, #16
   3b34a:	bfd8      	it	le
   3b34c:	fb01 f909 	mulle.w	r9, r1, r9
   3b350:	e7f5      	b.n	3b33e <_strtod_l+0x25e>
   3b352:	2d10      	cmp	r5, #16
   3b354:	bfdc      	itt	le
   3b356:	230a      	movle	r3, #10
   3b358:	fb03 2909 	mlale	r9, r3, r9, r2
   3b35c:	e7e1      	b.n	3b322 <_strtod_l+0x242>
   3b35e:	2300      	movs	r3, #0
   3b360:	9305      	str	r3, [sp, #20]
   3b362:	2301      	movs	r3, #1
   3b364:	e77c      	b.n	3b260 <_strtod_l+0x180>
   3b366:	f04f 0c00 	mov.w	ip, #0
   3b36a:	f108 0202 	add.w	r2, r8, #2
   3b36e:	9217      	str	r2, [sp, #92]	; 0x5c
   3b370:	f898 2002 	ldrb.w	r2, [r8, #2]
   3b374:	e785      	b.n	3b282 <_strtod_l+0x1a2>
   3b376:	f04f 0c01 	mov.w	ip, #1
   3b37a:	e7f6      	b.n	3b36a <_strtod_l+0x28a>
   3b37c:	00040a1c 	.word	0x00040a1c
   3b380:	000408b0 	.word	0x000408b0
   3b384:	7ff00000 	.word	0x7ff00000
   3b388:	9a17      	ldr	r2, [sp, #92]	; 0x5c
   3b38a:	1c51      	adds	r1, r2, #1
   3b38c:	9117      	str	r1, [sp, #92]	; 0x5c
   3b38e:	7852      	ldrb	r2, [r2, #1]
   3b390:	2a30      	cmp	r2, #48	; 0x30
   3b392:	d0f9      	beq.n	3b388 <_strtod_l+0x2a8>
   3b394:	f1a2 0131 	sub.w	r1, r2, #49	; 0x31
   3b398:	2908      	cmp	r1, #8
   3b39a:	f63f af79 	bhi.w	3b290 <_strtod_l+0x1b0>
   3b39e:	f1a2 0e30 	sub.w	lr, r2, #48	; 0x30
   3b3a2:	9a17      	ldr	r2, [sp, #92]	; 0x5c
   3b3a4:	9206      	str	r2, [sp, #24]
   3b3a6:	9a17      	ldr	r2, [sp, #92]	; 0x5c
   3b3a8:	1c51      	adds	r1, r2, #1
   3b3aa:	9117      	str	r1, [sp, #92]	; 0x5c
   3b3ac:	7852      	ldrb	r2, [r2, #1]
   3b3ae:	f1a2 0630 	sub.w	r6, r2, #48	; 0x30
   3b3b2:	2e09      	cmp	r6, #9
   3b3b4:	d937      	bls.n	3b426 <_strtod_l+0x346>
   3b3b6:	9e06      	ldr	r6, [sp, #24]
   3b3b8:	1b89      	subs	r1, r1, r6
   3b3ba:	f644 661f 	movw	r6, #19999	; 0x4e1f
   3b3be:	2908      	cmp	r1, #8
   3b3c0:	dc02      	bgt.n	3b3c8 <_strtod_l+0x2e8>
   3b3c2:	4576      	cmp	r6, lr
   3b3c4:	bfa8      	it	ge
   3b3c6:	4676      	movge	r6, lr
   3b3c8:	f1bc 0f00 	cmp.w	ip, #0
   3b3cc:	d000      	beq.n	3b3d0 <_strtod_l+0x2f0>
   3b3ce:	4276      	negs	r6, r6
   3b3d0:	2d00      	cmp	r5, #0
   3b3d2:	d14d      	bne.n	3b470 <_strtod_l+0x390>
   3b3d4:	9904      	ldr	r1, [sp, #16]
   3b3d6:	4301      	orrs	r1, r0
   3b3d8:	f47f aec6 	bne.w	3b168 <_strtod_l+0x88>
   3b3dc:	2b00      	cmp	r3, #0
   3b3de:	f47f aee1 	bne.w	3b1a4 <_strtod_l+0xc4>
   3b3e2:	2a69      	cmp	r2, #105	; 0x69
   3b3e4:	d027      	beq.n	3b436 <_strtod_l+0x356>
   3b3e6:	dc24      	bgt.n	3b432 <_strtod_l+0x352>
   3b3e8:	2a49      	cmp	r2, #73	; 0x49
   3b3ea:	d024      	beq.n	3b436 <_strtod_l+0x356>
   3b3ec:	2a4e      	cmp	r2, #78	; 0x4e
   3b3ee:	f47f aed9 	bne.w	3b1a4 <_strtod_l+0xc4>
   3b3f2:	499f      	ldr	r1, [pc, #636]	; (3b670 <_strtod_l+0x590>)
   3b3f4:	a817      	add	r0, sp, #92	; 0x5c
   3b3f6:	f003 fe0d 	bl	3f014 <__match>
   3b3fa:	2800      	cmp	r0, #0
   3b3fc:	f43f aed2 	beq.w	3b1a4 <_strtod_l+0xc4>
   3b400:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b402:	781b      	ldrb	r3, [r3, #0]
   3b404:	2b28      	cmp	r3, #40	; 0x28
   3b406:	d12d      	bne.n	3b464 <_strtod_l+0x384>
   3b408:	aa1a      	add	r2, sp, #104	; 0x68
   3b40a:	499a      	ldr	r1, [pc, #616]	; (3b674 <_strtod_l+0x594>)
   3b40c:	a817      	add	r0, sp, #92	; 0x5c
   3b40e:	f003 fe15 	bl	3f03c <__hexnan>
   3b412:	2805      	cmp	r0, #5
   3b414:	d126      	bne.n	3b464 <_strtod_l+0x384>
   3b416:	9b1b      	ldr	r3, [sp, #108]	; 0x6c
   3b418:	f8dd a068 	ldr.w	sl, [sp, #104]	; 0x68
   3b41c:	f043 4bff 	orr.w	fp, r3, #2139095040	; 0x7f800000
   3b420:	f44b 0be0 	orr.w	fp, fp, #7340032	; 0x700000
   3b424:	e6a0      	b.n	3b168 <_strtod_l+0x88>
   3b426:	210a      	movs	r1, #10
   3b428:	fb01 2e0e 	mla	lr, r1, lr, r2
   3b42c:	f1ae 0e30 	sub.w	lr, lr, #48	; 0x30
   3b430:	e7b9      	b.n	3b3a6 <_strtod_l+0x2c6>
   3b432:	2a6e      	cmp	r2, #110	; 0x6e
   3b434:	e7db      	b.n	3b3ee <_strtod_l+0x30e>
   3b436:	4990      	ldr	r1, [pc, #576]	; (3b678 <_strtod_l+0x598>)
   3b438:	a817      	add	r0, sp, #92	; 0x5c
   3b43a:	f003 fdeb 	bl	3f014 <__match>
   3b43e:	2800      	cmp	r0, #0
   3b440:	f43f aeb0 	beq.w	3b1a4 <_strtod_l+0xc4>
   3b444:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b446:	a817      	add	r0, sp, #92	; 0x5c
   3b448:	498c      	ldr	r1, [pc, #560]	; (3b67c <_strtod_l+0x59c>)
   3b44a:	3b01      	subs	r3, #1
   3b44c:	9317      	str	r3, [sp, #92]	; 0x5c
   3b44e:	f003 fde1 	bl	3f014 <__match>
   3b452:	b910      	cbnz	r0, 3b45a <_strtod_l+0x37a>
   3b454:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3b456:	3301      	adds	r3, #1
   3b458:	9317      	str	r3, [sp, #92]	; 0x5c
   3b45a:	f8df b230 	ldr.w	fp, [pc, #560]	; 3b68c <_strtod_l+0x5ac>
   3b45e:	f04f 0a00 	mov.w	sl, #0
   3b462:	e681      	b.n	3b168 <_strtod_l+0x88>
   3b464:	4886      	ldr	r0, [pc, #536]	; (3b680 <_strtod_l+0x5a0>)
   3b466:	f003 fea3 	bl	3f1b0 <nan>
   3b46a:	ec5b ab10 	vmov	sl, fp, d0
   3b46e:	e67b      	b.n	3b168 <_strtod_l+0x88>
   3b470:	9b05      	ldr	r3, [sp, #20]
   3b472:	2f00      	cmp	r7, #0
   3b474:	bf08      	it	eq
   3b476:	462f      	moveq	r7, r5
   3b478:	2d10      	cmp	r5, #16
   3b47a:	9807      	ldr	r0, [sp, #28]
   3b47c:	eba6 0303 	sub.w	r3, r6, r3
   3b480:	46a8      	mov	r8, r5
   3b482:	bfa8      	it	ge
   3b484:	f04f 0810 	movge.w	r8, #16
   3b488:	9306      	str	r3, [sp, #24]
   3b48a:	f004 f9dd 	bl	3f848 <__aeabi_ui2d>
   3b48e:	2d09      	cmp	r5, #9
   3b490:	4682      	mov	sl, r0
   3b492:	468b      	mov	fp, r1
   3b494:	dd13      	ble.n	3b4be <_strtod_l+0x3de>
   3b496:	4b7b      	ldr	r3, [pc, #492]	; (3b684 <_strtod_l+0x5a4>)
   3b498:	eb03 03c8 	add.w	r3, r3, r8, lsl #3
   3b49c:	e953 2312 	ldrd	r2, r3, [r3, #-72]	; 0x48
   3b4a0:	f004 fa4c 	bl	3f93c <__aeabi_dmul>
   3b4a4:	4682      	mov	sl, r0
   3b4a6:	468b      	mov	fp, r1
   3b4a8:	4648      	mov	r0, r9
   3b4aa:	f004 f9cd 	bl	3f848 <__aeabi_ui2d>
   3b4ae:	4602      	mov	r2, r0
   3b4b0:	460b      	mov	r3, r1
   3b4b2:	4650      	mov	r0, sl
   3b4b4:	4659      	mov	r1, fp
   3b4b6:	f004 f88b 	bl	3f5d0 <__adddf3>
   3b4ba:	4682      	mov	sl, r0
   3b4bc:	468b      	mov	fp, r1
   3b4be:	2d0f      	cmp	r5, #15
   3b4c0:	dc38      	bgt.n	3b534 <_strtod_l+0x454>
   3b4c2:	9b06      	ldr	r3, [sp, #24]
   3b4c4:	2b00      	cmp	r3, #0
   3b4c6:	f43f ae4f 	beq.w	3b168 <_strtod_l+0x88>
   3b4ca:	dd24      	ble.n	3b516 <_strtod_l+0x436>
   3b4cc:	2b16      	cmp	r3, #22
   3b4ce:	dc0b      	bgt.n	3b4e8 <_strtod_l+0x408>
   3b4d0:	496c      	ldr	r1, [pc, #432]	; (3b684 <_strtod_l+0x5a4>)
   3b4d2:	4652      	mov	r2, sl
   3b4d4:	eb01 01c3 	add.w	r1, r1, r3, lsl #3
   3b4d8:	465b      	mov	r3, fp
   3b4da:	e9d1 0100 	ldrd	r0, r1, [r1]
   3b4de:	f004 fa2d 	bl	3f93c <__aeabi_dmul>
   3b4e2:	4682      	mov	sl, r0
   3b4e4:	468b      	mov	fp, r1
   3b4e6:	e63f      	b.n	3b168 <_strtod_l+0x88>
   3b4e8:	f1c5 0325 	rsb	r3, r5, #37	; 0x25
   3b4ec:	9a06      	ldr	r2, [sp, #24]
   3b4ee:	4293      	cmp	r3, r2
   3b4f0:	db20      	blt.n	3b534 <_strtod_l+0x454>
   3b4f2:	f1c5 050f 	rsb	r5, r5, #15
   3b4f6:	4c63      	ldr	r4, [pc, #396]	; (3b684 <_strtod_l+0x5a4>)
   3b4f8:	4652      	mov	r2, sl
   3b4fa:	465b      	mov	r3, fp
   3b4fc:	eb04 01c5 	add.w	r1, r4, r5, lsl #3
   3b500:	e9d1 0100 	ldrd	r0, r1, [r1]
   3b504:	f004 fa1a 	bl	3f93c <__aeabi_dmul>
   3b508:	9b06      	ldr	r3, [sp, #24]
   3b50a:	1b5d      	subs	r5, r3, r5
   3b50c:	eb04 04c5 	add.w	r4, r4, r5, lsl #3
   3b510:	e9d4 2300 	ldrd	r2, r3, [r4]
   3b514:	e7e3      	b.n	3b4de <_strtod_l+0x3fe>
   3b516:	9b06      	ldr	r3, [sp, #24]
   3b518:	3316      	adds	r3, #22
   3b51a:	db0b      	blt.n	3b534 <_strtod_l+0x454>
   3b51c:	9b05      	ldr	r3, [sp, #20]
   3b51e:	4650      	mov	r0, sl
   3b520:	4659      	mov	r1, fp
   3b522:	1b9e      	subs	r6, r3, r6
   3b524:	4b57      	ldr	r3, [pc, #348]	; (3b684 <_strtod_l+0x5a4>)
   3b526:	eb03 06c6 	add.w	r6, r3, r6, lsl #3
   3b52a:	e9d6 2300 	ldrd	r2, r3, [r6]
   3b52e:	f004 fb2f 	bl	3fb90 <__aeabi_ddiv>
   3b532:	e7d6      	b.n	3b4e2 <_strtod_l+0x402>
   3b534:	eba5 0808 	sub.w	r8, r5, r8
   3b538:	9b06      	ldr	r3, [sp, #24]
   3b53a:	4498      	add	r8, r3
   3b53c:	f1b8 0f00 	cmp.w	r8, #0
   3b540:	dd74      	ble.n	3b62c <_strtod_l+0x54c>
   3b542:	f018 030f 	ands.w	r3, r8, #15
   3b546:	d00a      	beq.n	3b55e <_strtod_l+0x47e>
   3b548:	494e      	ldr	r1, [pc, #312]	; (3b684 <_strtod_l+0x5a4>)
   3b54a:	4652      	mov	r2, sl
   3b54c:	eb01 01c3 	add.w	r1, r1, r3, lsl #3
   3b550:	465b      	mov	r3, fp
   3b552:	e9d1 0100 	ldrd	r0, r1, [r1]
   3b556:	f004 f9f1 	bl	3f93c <__aeabi_dmul>
   3b55a:	4682      	mov	sl, r0
   3b55c:	468b      	mov	fp, r1
   3b55e:	f038 080f 	bics.w	r8, r8, #15
   3b562:	d04f      	beq.n	3b604 <_strtod_l+0x524>
   3b564:	f5b8 7f9a 	cmp.w	r8, #308	; 0x134
   3b568:	dd22      	ble.n	3b5b0 <_strtod_l+0x4d0>
   3b56a:	2500      	movs	r5, #0
   3b56c:	462e      	mov	r6, r5
   3b56e:	9507      	str	r5, [sp, #28]
   3b570:	9505      	str	r5, [sp, #20]
   3b572:	2322      	movs	r3, #34	; 0x22
   3b574:	f8df b114 	ldr.w	fp, [pc, #276]	; 3b68c <_strtod_l+0x5ac>
   3b578:	f04f 0a00 	mov.w	sl, #0
   3b57c:	6023      	str	r3, [r4, #0]
   3b57e:	9b07      	ldr	r3, [sp, #28]
   3b580:	2b00      	cmp	r3, #0
   3b582:	f43f adf1 	beq.w	3b168 <_strtod_l+0x88>
   3b586:	9918      	ldr	r1, [sp, #96]	; 0x60
   3b588:	4620      	mov	r0, r4
   3b58a:	f7fe fd9a 	bl	3a0c2 <_Bfree>
   3b58e:	9905      	ldr	r1, [sp, #20]
   3b590:	4620      	mov	r0, r4
   3b592:	f7fe fd96 	bl	3a0c2 <_Bfree>
   3b596:	4631      	mov	r1, r6
   3b598:	4620      	mov	r0, r4
   3b59a:	f7fe fd92 	bl	3a0c2 <_Bfree>
   3b59e:	9907      	ldr	r1, [sp, #28]
   3b5a0:	4620      	mov	r0, r4
   3b5a2:	f7fe fd8e 	bl	3a0c2 <_Bfree>
   3b5a6:	4629      	mov	r1, r5
   3b5a8:	4620      	mov	r0, r4
   3b5aa:	f7fe fd8a 	bl	3a0c2 <_Bfree>
   3b5ae:	e5db      	b.n	3b168 <_strtod_l+0x88>
   3b5b0:	4b35      	ldr	r3, [pc, #212]	; (3b688 <_strtod_l+0x5a8>)
   3b5b2:	ea4f 1828 	mov.w	r8, r8, asr #4
   3b5b6:	4650      	mov	r0, sl
   3b5b8:	4659      	mov	r1, fp
   3b5ba:	9304      	str	r3, [sp, #16]
   3b5bc:	2300      	movs	r3, #0
   3b5be:	4699      	mov	r9, r3
   3b5c0:	f1b8 0f01 	cmp.w	r8, #1
   3b5c4:	dc21      	bgt.n	3b60a <_strtod_l+0x52a>
   3b5c6:	b10b      	cbz	r3, 3b5cc <_strtod_l+0x4ec>
   3b5c8:	4682      	mov	sl, r0
   3b5ca:	468b      	mov	fp, r1
   3b5cc:	4b2e      	ldr	r3, [pc, #184]	; (3b688 <_strtod_l+0x5a8>)
   3b5ce:	f1ab 7b54 	sub.w	fp, fp, #55574528	; 0x3500000
   3b5d2:	4652      	mov	r2, sl
   3b5d4:	eb03 09c9 	add.w	r9, r3, r9, lsl #3
   3b5d8:	465b      	mov	r3, fp
   3b5da:	e9d9 0100 	ldrd	r0, r1, [r9]
   3b5de:	f004 f9ad 	bl	3f93c <__aeabi_dmul>
   3b5e2:	4b2a      	ldr	r3, [pc, #168]	; (3b68c <_strtod_l+0x5ac>)
   3b5e4:	460a      	mov	r2, r1
   3b5e6:	4682      	mov	sl, r0
   3b5e8:	400b      	ands	r3, r1
   3b5ea:	4929      	ldr	r1, [pc, #164]	; (3b690 <_strtod_l+0x5b0>)
   3b5ec:	428b      	cmp	r3, r1
   3b5ee:	d8bc      	bhi.n	3b56a <_strtod_l+0x48a>
   3b5f0:	f5a1 1180 	sub.w	r1, r1, #1048576	; 0x100000
   3b5f4:	428b      	cmp	r3, r1
   3b5f6:	bf86      	itte	hi
   3b5f8:	f8df b098 	ldrhi.w	fp, [pc, #152]	; 3b694 <_strtod_l+0x5b4>
   3b5fc:	f04f 3aff 	movhi.w	sl, #4294967295	; 0xffffffff
   3b600:	f102 7b54 	addls.w	fp, r2, #55574528	; 0x3500000
   3b604:	2300      	movs	r3, #0
   3b606:	9304      	str	r3, [sp, #16]
   3b608:	e083      	b.n	3b712 <_strtod_l+0x632>
   3b60a:	f018 0f01 	tst.w	r8, #1
   3b60e:	d005      	beq.n	3b61c <_strtod_l+0x53c>
   3b610:	9b04      	ldr	r3, [sp, #16]
   3b612:	e9d3 2300 	ldrd	r2, r3, [r3]
   3b616:	f004 f991 	bl	3f93c <__aeabi_dmul>
   3b61a:	2301      	movs	r3, #1
   3b61c:	9a04      	ldr	r2, [sp, #16]
   3b61e:	f109 0901 	add.w	r9, r9, #1
   3b622:	ea4f 0868 	mov.w	r8, r8, asr #1
   3b626:	3208      	adds	r2, #8
   3b628:	9204      	str	r2, [sp, #16]
   3b62a:	e7c9      	b.n	3b5c0 <_strtod_l+0x4e0>
   3b62c:	d0ea      	beq.n	3b604 <_strtod_l+0x524>
   3b62e:	f1c8 0800 	rsb	r8, r8, #0
   3b632:	f018 020f 	ands.w	r2, r8, #15
   3b636:	d00a      	beq.n	3b64e <_strtod_l+0x56e>
   3b638:	4b12      	ldr	r3, [pc, #72]	; (3b684 <_strtod_l+0x5a4>)
   3b63a:	4650      	mov	r0, sl
   3b63c:	4659      	mov	r1, fp
   3b63e:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
   3b642:	e9d3 2300 	ldrd	r2, r3, [r3]
   3b646:	f004 faa3 	bl	3fb90 <__aeabi_ddiv>
   3b64a:	4682      	mov	sl, r0
   3b64c:	468b      	mov	fp, r1
   3b64e:	ea5f 1828 	movs.w	r8, r8, asr #4
   3b652:	d0d7      	beq.n	3b604 <_strtod_l+0x524>
   3b654:	f1b8 0f1f 	cmp.w	r8, #31
   3b658:	dd1e      	ble.n	3b698 <_strtod_l+0x5b8>
   3b65a:	2500      	movs	r5, #0
   3b65c:	462e      	mov	r6, r5
   3b65e:	9507      	str	r5, [sp, #28]
   3b660:	9505      	str	r5, [sp, #20]
   3b662:	2322      	movs	r3, #34	; 0x22
   3b664:	f04f 0a00 	mov.w	sl, #0
   3b668:	f04f 0b00 	mov.w	fp, #0
   3b66c:	6023      	str	r3, [r4, #0]
   3b66e:	e786      	b.n	3b57e <_strtod_l+0x49e>
   3b670:	000404dc 	.word	0x000404dc
   3b674:	000408c4 	.word	0x000408c4
   3b678:	000404d4 	.word	0x000404d4
   3b67c:	00040672 	.word	0x00040672
   3b680:	0004099d 	.word	0x0004099d
   3b684:	00040798 	.word	0x00040798
   3b688:	00040888 	.word	0x00040888
   3b68c:	7ff00000 	.word	0x7ff00000
   3b690:	7ca00000 	.word	0x7ca00000
   3b694:	7fefffff 	.word	0x7fefffff
   3b698:	f018 0310 	ands.w	r3, r8, #16
   3b69c:	4650      	mov	r0, sl
   3b69e:	4659      	mov	r1, fp
   3b6a0:	f8df 93a8 	ldr.w	r9, [pc, #936]	; 3ba4c <_strtod_l+0x96c>
   3b6a4:	bf18      	it	ne
   3b6a6:	236a      	movne	r3, #106	; 0x6a
   3b6a8:	9304      	str	r3, [sp, #16]
   3b6aa:	2300      	movs	r3, #0
   3b6ac:	f018 0f01 	tst.w	r8, #1
   3b6b0:	d004      	beq.n	3b6bc <_strtod_l+0x5dc>
   3b6b2:	e9d9 2300 	ldrd	r2, r3, [r9]
   3b6b6:	f004 f941 	bl	3f93c <__aeabi_dmul>
   3b6ba:	2301      	movs	r3, #1
   3b6bc:	ea5f 0868 	movs.w	r8, r8, asr #1
   3b6c0:	f109 0908 	add.w	r9, r9, #8
   3b6c4:	d1f2      	bne.n	3b6ac <_strtod_l+0x5cc>
   3b6c6:	b10b      	cbz	r3, 3b6cc <_strtod_l+0x5ec>
   3b6c8:	4682      	mov	sl, r0
   3b6ca:	468b      	mov	fp, r1
   3b6cc:	9b04      	ldr	r3, [sp, #16]
   3b6ce:	b1c3      	cbz	r3, 3b702 <_strtod_l+0x622>
   3b6d0:	f3cb 520a 	ubfx	r2, fp, #20, #11
   3b6d4:	4659      	mov	r1, fp
   3b6d6:	f1c2 036b 	rsb	r3, r2, #107	; 0x6b
   3b6da:	2b00      	cmp	r3, #0
   3b6dc:	dd11      	ble.n	3b702 <_strtod_l+0x622>
   3b6de:	2b1f      	cmp	r3, #31
   3b6e0:	f340 8121 	ble.w	3b926 <_strtod_l+0x846>
   3b6e4:	2b34      	cmp	r3, #52	; 0x34
   3b6e6:	f04f 0a00 	mov.w	sl, #0
   3b6ea:	bfdb      	ittet	le
   3b6ec:	f1c2 024b 	rsble	r2, r2, #75	; 0x4b
   3b6f0:	f04f 33ff 	movle.w	r3, #4294967295	; 0xffffffff
   3b6f4:	f04f 7b5c 	movgt.w	fp, #57671680	; 0x3700000
   3b6f8:	fa03 f202 	lslle.w	r2, r3, r2
   3b6fc:	bfd8      	it	le
   3b6fe:	ea02 0b01 	andle.w	fp, r2, r1
   3b702:	2200      	movs	r2, #0
   3b704:	2300      	movs	r3, #0
   3b706:	4650      	mov	r0, sl
   3b708:	4659      	mov	r1, fp
   3b70a:	f004 fb7f 	bl	3fe0c <__aeabi_dcmpeq>
   3b70e:	2800      	cmp	r0, #0
   3b710:	d1a3      	bne.n	3b65a <_strtod_l+0x57a>
   3b712:	9b07      	ldr	r3, [sp, #28]
   3b714:	463a      	mov	r2, r7
   3b716:	9908      	ldr	r1, [sp, #32]
   3b718:	4620      	mov	r0, r4
   3b71a:	9300      	str	r3, [sp, #0]
   3b71c:	462b      	mov	r3, r5
   3b71e:	f7fe fd21 	bl	3a164 <__s2b>
   3b722:	9007      	str	r0, [sp, #28]
   3b724:	2800      	cmp	r0, #0
   3b726:	f43f af20 	beq.w	3b56a <_strtod_l+0x48a>
   3b72a:	9b05      	ldr	r3, [sp, #20]
   3b72c:	2500      	movs	r5, #0
   3b72e:	1b9e      	subs	r6, r3, r6
   3b730:	9b06      	ldr	r3, [sp, #24]
   3b732:	2b00      	cmp	r3, #0
   3b734:	bfb4      	ite	lt
   3b736:	4633      	movlt	r3, r6
   3b738:	2300      	movge	r3, #0
   3b73a:	462e      	mov	r6, r5
   3b73c:	930c      	str	r3, [sp, #48]	; 0x30
   3b73e:	9b06      	ldr	r3, [sp, #24]
   3b740:	ea23 73e3 	bic.w	r3, r3, r3, asr #31
   3b744:	9312      	str	r3, [sp, #72]	; 0x48
   3b746:	9b07      	ldr	r3, [sp, #28]
   3b748:	4620      	mov	r0, r4
   3b74a:	6859      	ldr	r1, [r3, #4]
   3b74c:	f7fe fc94 	bl	3a078 <_Balloc>
   3b750:	9005      	str	r0, [sp, #20]
   3b752:	2800      	cmp	r0, #0
   3b754:	f43f af0d 	beq.w	3b572 <_strtod_l+0x492>
   3b758:	9b07      	ldr	r3, [sp, #28]
   3b75a:	300c      	adds	r0, #12
   3b75c:	691a      	ldr	r2, [r3, #16]
   3b75e:	f103 010c 	add.w	r1, r3, #12
   3b762:	3202      	adds	r2, #2
   3b764:	0092      	lsls	r2, r2, #2
   3b766:	f7fe fb77 	bl	39e58 <memcpy>
   3b76a:	aa1a      	add	r2, sp, #104	; 0x68
   3b76c:	a919      	add	r1, sp, #100	; 0x64
   3b76e:	4620      	mov	r0, r4
   3b770:	ec4b ab10 	vmov	d0, sl, fp
   3b774:	f7ff f822 	bl	3a7bc <__d2b>
   3b778:	9018      	str	r0, [sp, #96]	; 0x60
   3b77a:	ec4b ab18 	vmov	d8, sl, fp
   3b77e:	2800      	cmp	r0, #0
   3b780:	f43f aef7 	beq.w	3b572 <_strtod_l+0x492>
   3b784:	2101      	movs	r1, #1
   3b786:	4620      	mov	r0, r4
   3b788:	f7fe fd85 	bl	3a296 <__i2b>
   3b78c:	4606      	mov	r6, r0
   3b78e:	2800      	cmp	r0, #0
   3b790:	f43f aeef 	beq.w	3b572 <_strtod_l+0x492>
   3b794:	9b19      	ldr	r3, [sp, #100]	; 0x64
   3b796:	9904      	ldr	r1, [sp, #16]
   3b798:	2b00      	cmp	r3, #0
   3b79a:	eba3 0801 	sub.w	r8, r3, r1
   3b79e:	bfa9      	itett	ge
   3b7a0:	9a0c      	ldrge	r2, [sp, #48]	; 0x30
   3b7a2:	9a12      	ldrlt	r2, [sp, #72]	; 0x48
   3b7a4:	eb03 0902 	addge.w	r9, r3, r2
   3b7a8:	9f12      	ldrge	r7, [sp, #72]	; 0x48
   3b7aa:	bfb8      	it	lt
   3b7ac:	1ad7      	sublt	r7, r2, r3
   3b7ae:	9a1a      	ldr	r2, [sp, #104]	; 0x68
   3b7b0:	4ba1      	ldr	r3, [pc, #644]	; (3ba38 <_strtod_l+0x958>)
   3b7b2:	4490      	add	r8, r2
   3b7b4:	bfb8      	it	lt
   3b7b6:	f8dd 9030 	ldrlt.w	r9, [sp, #48]	; 0x30
   3b7ba:	f1c2 0236 	rsb	r2, r2, #54	; 0x36
   3b7be:	f108 38ff 	add.w	r8, r8, #4294967295	; 0xffffffff
   3b7c2:	4598      	cmp	r8, r3
   3b7c4:	f280 80c3 	bge.w	3b94e <_strtod_l+0x86e>
   3b7c8:	eba3 0308 	sub.w	r3, r3, r8
   3b7cc:	2101      	movs	r1, #1
   3b7ce:	2b1f      	cmp	r3, #31
   3b7d0:	eba2 0203 	sub.w	r2, r2, r3
   3b7d4:	f300 80ae 	bgt.w	3b934 <_strtod_l+0x854>
   3b7d8:	fa01 f303 	lsl.w	r3, r1, r3
   3b7dc:	930d      	str	r3, [sp, #52]	; 0x34
   3b7de:	2300      	movs	r3, #0
   3b7e0:	9308      	str	r3, [sp, #32]
   3b7e2:	eb09 0802 	add.w	r8, r9, r2
   3b7e6:	9b04      	ldr	r3, [sp, #16]
   3b7e8:	4417      	add	r7, r2
   3b7ea:	45c1      	cmp	r9, r8
   3b7ec:	441f      	add	r7, r3
   3b7ee:	464b      	mov	r3, r9
   3b7f0:	bfa8      	it	ge
   3b7f2:	4643      	movge	r3, r8
   3b7f4:	42bb      	cmp	r3, r7
   3b7f6:	bfa8      	it	ge
   3b7f8:	463b      	movge	r3, r7
   3b7fa:	2b00      	cmp	r3, #0
   3b7fc:	bfc2      	ittt	gt
   3b7fe:	eba8 0803 	subgt.w	r8, r8, r3
   3b802:	1aff      	subgt	r7, r7, r3
   3b804:	eba9 0903 	subgt.w	r9, r9, r3
   3b808:	9b0c      	ldr	r3, [sp, #48]	; 0x30
   3b80a:	2b00      	cmp	r3, #0
   3b80c:	dd17      	ble.n	3b83e <_strtod_l+0x75e>
   3b80e:	4631      	mov	r1, r6
   3b810:	461a      	mov	r2, r3
   3b812:	4620      	mov	r0, r4
   3b814:	f7fe fdfc 	bl	3a410 <__pow5mult>
   3b818:	4606      	mov	r6, r0
   3b81a:	2800      	cmp	r0, #0
   3b81c:	f43f aea9 	beq.w	3b572 <_strtod_l+0x492>
   3b820:	4601      	mov	r1, r0
   3b822:	9a18      	ldr	r2, [sp, #96]	; 0x60
   3b824:	4620      	mov	r0, r4
   3b826:	f7fe fd4b 	bl	3a2c0 <__multiply>
   3b82a:	900b      	str	r0, [sp, #44]	; 0x2c
   3b82c:	2800      	cmp	r0, #0
   3b82e:	f43f aea0 	beq.w	3b572 <_strtod_l+0x492>
   3b832:	9918      	ldr	r1, [sp, #96]	; 0x60
   3b834:	4620      	mov	r0, r4
   3b836:	f7fe fc44 	bl	3a0c2 <_Bfree>
   3b83a:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   3b83c:	9318      	str	r3, [sp, #96]	; 0x60
   3b83e:	f1b8 0f00 	cmp.w	r8, #0
   3b842:	f300 8089 	bgt.w	3b958 <_strtod_l+0x878>
   3b846:	9b06      	ldr	r3, [sp, #24]
   3b848:	2b00      	cmp	r3, #0
   3b84a:	dd08      	ble.n	3b85e <_strtod_l+0x77e>
   3b84c:	9a12      	ldr	r2, [sp, #72]	; 0x48
   3b84e:	4620      	mov	r0, r4
   3b850:	9905      	ldr	r1, [sp, #20]
   3b852:	f7fe fddd 	bl	3a410 <__pow5mult>
   3b856:	9005      	str	r0, [sp, #20]
   3b858:	2800      	cmp	r0, #0
   3b85a:	f43f ae8a 	beq.w	3b572 <_strtod_l+0x492>
   3b85e:	2f00      	cmp	r7, #0
   3b860:	dd08      	ble.n	3b874 <_strtod_l+0x794>
   3b862:	463a      	mov	r2, r7
   3b864:	9905      	ldr	r1, [sp, #20]
   3b866:	4620      	mov	r0, r4
   3b868:	f7fe fe12 	bl	3a490 <__lshift>
   3b86c:	9005      	str	r0, [sp, #20]
   3b86e:	2800      	cmp	r0, #0
   3b870:	f43f ae7f 	beq.w	3b572 <_strtod_l+0x492>
   3b874:	f1b9 0f00 	cmp.w	r9, #0
   3b878:	dd08      	ble.n	3b88c <_strtod_l+0x7ac>
   3b87a:	4631      	mov	r1, r6
   3b87c:	464a      	mov	r2, r9
   3b87e:	4620      	mov	r0, r4
   3b880:	f7fe fe06 	bl	3a490 <__lshift>
   3b884:	4606      	mov	r6, r0
   3b886:	2800      	cmp	r0, #0
   3b888:	f43f ae73 	beq.w	3b572 <_strtod_l+0x492>
   3b88c:	9a05      	ldr	r2, [sp, #20]
   3b88e:	4620      	mov	r0, r4
   3b890:	9918      	ldr	r1, [sp, #96]	; 0x60
   3b892:	f7fe fe89 	bl	3a5a8 <__mdiff>
   3b896:	4605      	mov	r5, r0
   3b898:	2800      	cmp	r0, #0
   3b89a:	f43f ae6a 	beq.w	3b572 <_strtod_l+0x492>
   3b89e:	68c3      	ldr	r3, [r0, #12]
   3b8a0:	4631      	mov	r1, r6
   3b8a2:	930b      	str	r3, [sp, #44]	; 0x2c
   3b8a4:	2300      	movs	r3, #0
   3b8a6:	60c3      	str	r3, [r0, #12]
   3b8a8:	f7fe fe62 	bl	3a570 <__mcmp>
   3b8ac:	2800      	cmp	r0, #0
   3b8ae:	da5d      	bge.n	3b96c <_strtod_l+0x88c>
   3b8b0:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   3b8b2:	ea53 030a 	orrs.w	r3, r3, sl
   3b8b6:	d17f      	bne.n	3b9b8 <_strtod_l+0x8d8>
   3b8b8:	f3cb 0313 	ubfx	r3, fp, #0, #20
   3b8bc:	2b00      	cmp	r3, #0
   3b8be:	d17b      	bne.n	3b9b8 <_strtod_l+0x8d8>
   3b8c0:	f02b 4300 	bic.w	r3, fp, #2147483648	; 0x80000000
   3b8c4:	0d1b      	lsrs	r3, r3, #20
   3b8c6:	051b      	lsls	r3, r3, #20
   3b8c8:	f1b3 6fd6 	cmp.w	r3, #112197632	; 0x6b00000
   3b8cc:	d974      	bls.n	3b9b8 <_strtod_l+0x8d8>
   3b8ce:	696b      	ldr	r3, [r5, #20]
   3b8d0:	b913      	cbnz	r3, 3b8d8 <_strtod_l+0x7f8>
   3b8d2:	692b      	ldr	r3, [r5, #16]
   3b8d4:	2b01      	cmp	r3, #1
   3b8d6:	dd6f      	ble.n	3b9b8 <_strtod_l+0x8d8>
   3b8d8:	4629      	mov	r1, r5
   3b8da:	2201      	movs	r2, #1
   3b8dc:	4620      	mov	r0, r4
   3b8de:	f7fe fdd7 	bl	3a490 <__lshift>
   3b8e2:	4631      	mov	r1, r6
   3b8e4:	4605      	mov	r5, r0
   3b8e6:	f7fe fe43 	bl	3a570 <__mcmp>
   3b8ea:	2800      	cmp	r0, #0
   3b8ec:	dd64      	ble.n	3b9b8 <_strtod_l+0x8d8>
   3b8ee:	9904      	ldr	r1, [sp, #16]
   3b8f0:	465b      	mov	r3, fp
   3b8f2:	4a52      	ldr	r2, [pc, #328]	; (3ba3c <_strtod_l+0x95c>)
   3b8f4:	2900      	cmp	r1, #0
   3b8f6:	d07f      	beq.n	3b9f8 <_strtod_l+0x918>
   3b8f8:	ea02 010b 	and.w	r1, r2, fp
   3b8fc:	f1b1 6fd6 	cmp.w	r1, #112197632	; 0x6b00000
   3b900:	dc7a      	bgt.n	3b9f8 <_strtod_l+0x918>
   3b902:	f1b1 7f5c 	cmp.w	r1, #57671680	; 0x3700000
   3b906:	f77f aeac 	ble.w	3b662 <_strtod_l+0x582>
   3b90a:	4b4d      	ldr	r3, [pc, #308]	; (3ba40 <_strtod_l+0x960>)
   3b90c:	4650      	mov	r0, sl
   3b90e:	4659      	mov	r1, fp
   3b910:	2200      	movs	r2, #0
   3b912:	f004 f813 	bl	3f93c <__aeabi_dmul>
   3b916:	460b      	mov	r3, r1
   3b918:	4682      	mov	sl, r0
   3b91a:	468b      	mov	fp, r1
   3b91c:	4303      	orrs	r3, r0
   3b91e:	bf04      	itt	eq
   3b920:	2322      	moveq	r3, #34	; 0x22
   3b922:	6023      	streq	r3, [r4, #0]
   3b924:	e62f      	b.n	3b586 <_strtod_l+0x4a6>
   3b926:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3b92a:	fa02 f303 	lsl.w	r3, r2, r3
   3b92e:	ea03 0a0a 	and.w	sl, r3, sl
   3b932:	e6e6      	b.n	3b702 <_strtod_l+0x622>
   3b934:	f1c8 487f 	rsb	r8, r8, #4278190080	; 0xff000000
   3b938:	910d      	str	r1, [sp, #52]	; 0x34
   3b93a:	f508 087f 	add.w	r8, r8, #16711680	; 0xff0000
   3b93e:	f508 487b 	add.w	r8, r8, #64256	; 0xfb00
   3b942:	f108 08e2 	add.w	r8, r8, #226	; 0xe2
   3b946:	fa01 f308 	lsl.w	r3, r1, r8
   3b94a:	9308      	str	r3, [sp, #32]
   3b94c:	e749      	b.n	3b7e2 <_strtod_l+0x702>
   3b94e:	2300      	movs	r3, #0
   3b950:	9308      	str	r3, [sp, #32]
   3b952:	2301      	movs	r3, #1
   3b954:	930d      	str	r3, [sp, #52]	; 0x34
   3b956:	e744      	b.n	3b7e2 <_strtod_l+0x702>
   3b958:	4642      	mov	r2, r8
   3b95a:	9918      	ldr	r1, [sp, #96]	; 0x60
   3b95c:	4620      	mov	r0, r4
   3b95e:	f7fe fd97 	bl	3a490 <__lshift>
   3b962:	9018      	str	r0, [sp, #96]	; 0x60
   3b964:	2800      	cmp	r0, #0
   3b966:	f47f af6e 	bne.w	3b846 <_strtod_l+0x766>
   3b96a:	e602      	b.n	3b572 <_strtod_l+0x492>
   3b96c:	465f      	mov	r7, fp
   3b96e:	d16f      	bne.n	3ba50 <_strtod_l+0x970>
   3b970:	9a0b      	ldr	r2, [sp, #44]	; 0x2c
   3b972:	f3cb 0313 	ubfx	r3, fp, #0, #20
   3b976:	b342      	cbz	r2, 3b9ca <_strtod_l+0x8ea>
   3b978:	4a32      	ldr	r2, [pc, #200]	; (3ba44 <_strtod_l+0x964>)
   3b97a:	4293      	cmp	r3, r2
   3b97c:	d128      	bne.n	3b9d0 <_strtod_l+0x8f0>
   3b97e:	9b04      	ldr	r3, [sp, #16]
   3b980:	4651      	mov	r1, sl
   3b982:	b1eb      	cbz	r3, 3b9c0 <_strtod_l+0x8e0>
   3b984:	4b2d      	ldr	r3, [pc, #180]	; (3ba3c <_strtod_l+0x95c>)
   3b986:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3b98a:	403b      	ands	r3, r7
   3b98c:	f1b3 6fd4 	cmp.w	r3, #111149056	; 0x6a00000
   3b990:	d819      	bhi.n	3b9c6 <_strtod_l+0x8e6>
   3b992:	0d1b      	lsrs	r3, r3, #20
   3b994:	f1c3 036b 	rsb	r3, r3, #107	; 0x6b
   3b998:	fa02 f303 	lsl.w	r3, r2, r3
   3b99c:	4299      	cmp	r1, r3
   3b99e:	d117      	bne.n	3b9d0 <_strtod_l+0x8f0>
   3b9a0:	4b29      	ldr	r3, [pc, #164]	; (3ba48 <_strtod_l+0x968>)
   3b9a2:	429f      	cmp	r7, r3
   3b9a4:	d102      	bne.n	3b9ac <_strtod_l+0x8cc>
   3b9a6:	3101      	adds	r1, #1
   3b9a8:	f43f ade3 	beq.w	3b572 <_strtod_l+0x492>
   3b9ac:	4b23      	ldr	r3, [pc, #140]	; (3ba3c <_strtod_l+0x95c>)
   3b9ae:	f04f 0a00 	mov.w	sl, #0
   3b9b2:	403b      	ands	r3, r7
   3b9b4:	f503 1b80 	add.w	fp, r3, #1048576	; 0x100000
   3b9b8:	9b04      	ldr	r3, [sp, #16]
   3b9ba:	2b00      	cmp	r3, #0
   3b9bc:	d1a5      	bne.n	3b90a <_strtod_l+0x82a>
   3b9be:	e5e2      	b.n	3b586 <_strtod_l+0x4a6>
   3b9c0:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3b9c4:	e7ea      	b.n	3b99c <_strtod_l+0x8bc>
   3b9c6:	4613      	mov	r3, r2
   3b9c8:	e7e8      	b.n	3b99c <_strtod_l+0x8bc>
   3b9ca:	ea53 030a 	orrs.w	r3, r3, sl
   3b9ce:	d08e      	beq.n	3b8ee <_strtod_l+0x80e>
   3b9d0:	9b08      	ldr	r3, [sp, #32]
   3b9d2:	b1db      	cbz	r3, 3ba0c <_strtod_l+0x92c>
   3b9d4:	423b      	tst	r3, r7
   3b9d6:	d0ef      	beq.n	3b9b8 <_strtod_l+0x8d8>
   3b9d8:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   3b9da:	4650      	mov	r0, sl
   3b9dc:	9a04      	ldr	r2, [sp, #16]
   3b9de:	4659      	mov	r1, fp
   3b9e0:	b1c3      	cbz	r3, 3ba14 <_strtod_l+0x934>
   3b9e2:	f7ff fb61 	bl	3b0a8 <sulp>
   3b9e6:	4602      	mov	r2, r0
   3b9e8:	460b      	mov	r3, r1
   3b9ea:	ec51 0b18 	vmov	r0, r1, d8
   3b9ee:	f003 fdef 	bl	3f5d0 <__adddf3>
   3b9f2:	4682      	mov	sl, r0
   3b9f4:	468b      	mov	fp, r1
   3b9f6:	e7df      	b.n	3b9b8 <_strtod_l+0x8d8>
   3b9f8:	4013      	ands	r3, r2
   3b9fa:	f04f 3aff 	mov.w	sl, #4294967295	; 0xffffffff
   3b9fe:	f5a3 1380 	sub.w	r3, r3, #1048576	; 0x100000
   3ba02:	ea6f 5b13 	mvn.w	fp, r3, lsr #20
   3ba06:	ea6f 5b0b 	mvn.w	fp, fp, lsl #20
   3ba0a:	e7d5      	b.n	3b9b8 <_strtod_l+0x8d8>
   3ba0c:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3ba0e:	ea13 0f0a 	tst.w	r3, sl
   3ba12:	e7e0      	b.n	3b9d6 <_strtod_l+0x8f6>
   3ba14:	f7ff fb48 	bl	3b0a8 <sulp>
   3ba18:	4602      	mov	r2, r0
   3ba1a:	460b      	mov	r3, r1
   3ba1c:	ec51 0b18 	vmov	r0, r1, d8
   3ba20:	f003 fdd4 	bl	3f5cc <__aeabi_dsub>
   3ba24:	2200      	movs	r2, #0
   3ba26:	2300      	movs	r3, #0
   3ba28:	4682      	mov	sl, r0
   3ba2a:	468b      	mov	fp, r1
   3ba2c:	f004 f9ee 	bl	3fe0c <__aeabi_dcmpeq>
   3ba30:	2800      	cmp	r0, #0
   3ba32:	d0c1      	beq.n	3b9b8 <_strtod_l+0x8d8>
   3ba34:	e615      	b.n	3b662 <_strtod_l+0x582>
   3ba36:	bf00      	nop
   3ba38:	fffffc02 	.word	0xfffffc02
   3ba3c:	7ff00000 	.word	0x7ff00000
   3ba40:	39500000 	.word	0x39500000
   3ba44:	000fffff 	.word	0x000fffff
   3ba48:	7fefffff 	.word	0x7fefffff
   3ba4c:	000408d8 	.word	0x000408d8
   3ba50:	4631      	mov	r1, r6
   3ba52:	4628      	mov	r0, r5
   3ba54:	f7fe ff0e 	bl	3a874 <__ratio>
   3ba58:	2200      	movs	r2, #0
   3ba5a:	f04f 4380 	mov.w	r3, #1073741824	; 0x40000000
   3ba5e:	ee10 0a10 	vmov	r0, s0
   3ba62:	ec59 8b10 	vmov	r8, r9, d0
   3ba66:	4649      	mov	r1, r9
   3ba68:	f004 f9e4 	bl	3fe34 <__aeabi_dcmple>
   3ba6c:	2800      	cmp	r0, #0
   3ba6e:	d07a      	beq.n	3bb66 <_strtod_l+0xa86>
   3ba70:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   3ba72:	2b00      	cmp	r3, #0
   3ba74:	d04a      	beq.n	3bb0c <_strtod_l+0xa2c>
   3ba76:	2200      	movs	r2, #0
   3ba78:	4b93      	ldr	r3, [pc, #588]	; (3bcc8 <_strtod_l+0xbe8>)
   3ba7a:	f04f 0800 	mov.w	r8, #0
   3ba7e:	f8df 9248 	ldr.w	r9, [pc, #584]	; 3bcc8 <_strtod_l+0xbe8>
   3ba82:	e9cd 2308 	strd	r2, r3, [sp, #32]
   3ba86:	4b91      	ldr	r3, [pc, #580]	; (3bccc <_strtod_l+0xbec>)
   3ba88:	403b      	ands	r3, r7
   3ba8a:	930d      	str	r3, [sp, #52]	; 0x34
   3ba8c:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   3ba8e:	4b90      	ldr	r3, [pc, #576]	; (3bcd0 <_strtod_l+0xbf0>)
   3ba90:	429a      	cmp	r2, r3
   3ba92:	f040 80b0 	bne.w	3bbf6 <_strtod_l+0xb16>
   3ba96:	f1a7 7b54 	sub.w	fp, r7, #55574528	; 0x3500000
   3ba9a:	e9dd 0108 	ldrd	r0, r1, [sp, #32]
   3ba9e:	ec4b ab10 	vmov	d0, sl, fp
   3baa2:	e9cd 0108 	strd	r0, r1, [sp, #32]
   3baa6:	f7fe fe0f 	bl	3a6c8 <__ulp>
   3baaa:	e9dd 0108 	ldrd	r0, r1, [sp, #32]
   3baae:	ec53 2b10 	vmov	r2, r3, d0
   3bab2:	f003 ff43 	bl	3f93c <__aeabi_dmul>
   3bab6:	4652      	mov	r2, sl
   3bab8:	465b      	mov	r3, fp
   3baba:	f003 fd89 	bl	3f5d0 <__adddf3>
   3babe:	460b      	mov	r3, r1
   3bac0:	4982      	ldr	r1, [pc, #520]	; (3bccc <_strtod_l+0xbec>)
   3bac2:	4682      	mov	sl, r0
   3bac4:	4a83      	ldr	r2, [pc, #524]	; (3bcd4 <_strtod_l+0xbf4>)
   3bac6:	4019      	ands	r1, r3
   3bac8:	4291      	cmp	r1, r2
   3baca:	d960      	bls.n	3bb8e <_strtod_l+0xaae>
   3bacc:	ee18 3a90 	vmov	r3, s17
   3bad0:	f102 7254 	add.w	r2, r2, #55574528	; 0x3500000
   3bad4:	4293      	cmp	r3, r2
   3bad6:	d104      	bne.n	3bae2 <_strtod_l+0xa02>
   3bad8:	ee18 3a10 	vmov	r3, s16
   3badc:	3301      	adds	r3, #1
   3bade:	f43f ad48 	beq.w	3b572 <_strtod_l+0x492>
   3bae2:	f8df b1fc 	ldr.w	fp, [pc, #508]	; 3bce0 <_strtod_l+0xc00>
   3bae6:	f04f 3aff 	mov.w	sl, #4294967295	; 0xffffffff
   3baea:	9918      	ldr	r1, [sp, #96]	; 0x60
   3baec:	4620      	mov	r0, r4
   3baee:	f7fe fae8 	bl	3a0c2 <_Bfree>
   3baf2:	9905      	ldr	r1, [sp, #20]
   3baf4:	4620      	mov	r0, r4
   3baf6:	f7fe fae4 	bl	3a0c2 <_Bfree>
   3bafa:	4631      	mov	r1, r6
   3bafc:	4620      	mov	r0, r4
   3bafe:	f7fe fae0 	bl	3a0c2 <_Bfree>
   3bb02:	4629      	mov	r1, r5
   3bb04:	4620      	mov	r0, r4
   3bb06:	f7fe fadc 	bl	3a0c2 <_Bfree>
   3bb0a:	e61c      	b.n	3b746 <_strtod_l+0x666>
   3bb0c:	f1ba 0f00 	cmp.w	sl, #0
   3bb10:	d11b      	bne.n	3bb4a <_strtod_l+0xa6a>
   3bb12:	f3cb 0313 	ubfx	r3, fp, #0, #20
   3bb16:	b9f3      	cbnz	r3, 3bb56 <_strtod_l+0xa76>
   3bb18:	2200      	movs	r2, #0
   3bb1a:	4b6b      	ldr	r3, [pc, #428]	; (3bcc8 <_strtod_l+0xbe8>)
   3bb1c:	4640      	mov	r0, r8
   3bb1e:	4649      	mov	r1, r9
   3bb20:	f004 f97e 	bl	3fe20 <__aeabi_dcmplt>
   3bb24:	b9d0      	cbnz	r0, 3bb5c <_strtod_l+0xa7c>
   3bb26:	4640      	mov	r0, r8
   3bb28:	4649      	mov	r1, r9
   3bb2a:	2200      	movs	r2, #0
   3bb2c:	4b6a      	ldr	r3, [pc, #424]	; (3bcd8 <_strtod_l+0xbf8>)
   3bb2e:	f003 ff05 	bl	3f93c <__aeabi_dmul>
   3bb32:	4680      	mov	r8, r0
   3bb34:	4689      	mov	r9, r1
   3bb36:	f109 4300 	add.w	r3, r9, #2147483648	; 0x80000000
   3bb3a:	f8cd 8050 	str.w	r8, [sp, #80]	; 0x50
   3bb3e:	9315      	str	r3, [sp, #84]	; 0x54
   3bb40:	e9dd 2314 	ldrd	r2, r3, [sp, #80]	; 0x50
   3bb44:	e9cd 2308 	strd	r2, r3, [sp, #32]
   3bb48:	e79d      	b.n	3ba86 <_strtod_l+0x9a6>
   3bb4a:	f1ba 0f01 	cmp.w	sl, #1
   3bb4e:	d102      	bne.n	3bb56 <_strtod_l+0xa76>
   3bb50:	2f00      	cmp	r7, #0
   3bb52:	f43f ad86 	beq.w	3b662 <_strtod_l+0x582>
   3bb56:	2200      	movs	r2, #0
   3bb58:	4b60      	ldr	r3, [pc, #384]	; (3bcdc <_strtod_l+0xbfc>)
   3bb5a:	e78e      	b.n	3ba7a <_strtod_l+0x99a>
   3bb5c:	f04f 0800 	mov.w	r8, #0
   3bb60:	f8df 9174 	ldr.w	r9, [pc, #372]	; 3bcd8 <_strtod_l+0xbf8>
   3bb64:	e7e7      	b.n	3bb36 <_strtod_l+0xa56>
   3bb66:	4b5c      	ldr	r3, [pc, #368]	; (3bcd8 <_strtod_l+0xbf8>)
   3bb68:	4640      	mov	r0, r8
   3bb6a:	4649      	mov	r1, r9
   3bb6c:	2200      	movs	r2, #0
   3bb6e:	f003 fee5 	bl	3f93c <__aeabi_dmul>
   3bb72:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   3bb74:	4680      	mov	r8, r0
   3bb76:	4689      	mov	r9, r1
   3bb78:	b933      	cbnz	r3, 3bb88 <_strtod_l+0xaa8>
   3bb7a:	f101 4300 	add.w	r3, r1, #2147483648	; 0x80000000
   3bb7e:	900e      	str	r0, [sp, #56]	; 0x38
   3bb80:	930f      	str	r3, [sp, #60]	; 0x3c
   3bb82:	e9dd 230e 	ldrd	r2, r3, [sp, #56]	; 0x38
   3bb86:	e7dd      	b.n	3bb44 <_strtod_l+0xa64>
   3bb88:	e9cd 890e 	strd	r8, r9, [sp, #56]	; 0x38
   3bb8c:	e7f9      	b.n	3bb82 <_strtod_l+0xaa2>
   3bb8e:	f103 7b54 	add.w	fp, r3, #55574528	; 0x3500000
   3bb92:	9b04      	ldr	r3, [sp, #16]
   3bb94:	2b00      	cmp	r3, #0
   3bb96:	d1a8      	bne.n	3baea <_strtod_l+0xa0a>
   3bb98:	f02b 4300 	bic.w	r3, fp, #2147483648	; 0x80000000
   3bb9c:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   3bb9e:	0d1b      	lsrs	r3, r3, #20
   3bba0:	051b      	lsls	r3, r3, #20
   3bba2:	429a      	cmp	r2, r3
   3bba4:	d1a1      	bne.n	3baea <_strtod_l+0xa0a>
   3bba6:	4640      	mov	r0, r8
   3bba8:	4649      	mov	r1, r9
   3bbaa:	f004 fa27 	bl	3fffc <__aeabi_d2lz>
   3bbae:	f003 fe97 	bl	3f8e0 <__aeabi_l2d>
   3bbb2:	4602      	mov	r2, r0
   3bbb4:	460b      	mov	r3, r1
   3bbb6:	4640      	mov	r0, r8
   3bbb8:	4649      	mov	r1, r9
   3bbba:	f003 fd07 	bl	3f5cc <__aeabi_dsub>
   3bbbe:	f3cb 0313 	ubfx	r3, fp, #0, #20
   3bbc2:	9a0b      	ldr	r2, [sp, #44]	; 0x2c
   3bbc4:	4680      	mov	r8, r0
   3bbc6:	ea43 030a 	orr.w	r3, r3, sl
   3bbca:	4689      	mov	r9, r1
   3bbcc:	4313      	orrs	r3, r2
   3bbce:	d055      	beq.n	3bc7c <_strtod_l+0xb9c>
   3bbd0:	a335      	add	r3, pc, #212	; (adr r3, 3bca8 <_strtod_l+0xbc8>)
   3bbd2:	e9d3 2300 	ldrd	r2, r3, [r3]
   3bbd6:	f004 f923 	bl	3fe20 <__aeabi_dcmplt>
   3bbda:	2800      	cmp	r0, #0
   3bbdc:	f47f acd3 	bne.w	3b586 <_strtod_l+0x4a6>
   3bbe0:	4640      	mov	r0, r8
   3bbe2:	4649      	mov	r1, r9
   3bbe4:	a332      	add	r3, pc, #200	; (adr r3, 3bcb0 <_strtod_l+0xbd0>)
   3bbe6:	e9d3 2300 	ldrd	r2, r3, [r3]
   3bbea:	f004 f937 	bl	3fe5c <__aeabi_dcmpgt>
   3bbee:	2800      	cmp	r0, #0
   3bbf0:	f43f af7b 	beq.w	3baea <_strtod_l+0xa0a>
   3bbf4:	e4c7      	b.n	3b586 <_strtod_l+0x4a6>
   3bbf6:	9b04      	ldr	r3, [sp, #16]
   3bbf8:	b333      	cbz	r3, 3bc48 <_strtod_l+0xb68>
   3bbfa:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3bbfc:	f1b3 6fd4 	cmp.w	r3, #111149056	; 0x6a00000
   3bc00:	d822      	bhi.n	3bc48 <_strtod_l+0xb68>
   3bc02:	4640      	mov	r0, r8
   3bc04:	4649      	mov	r1, r9
   3bc06:	a32c      	add	r3, pc, #176	; (adr r3, 3bcb8 <_strtod_l+0xbd8>)
   3bc08:	e9d3 2300 	ldrd	r2, r3, [r3]
   3bc0c:	f004 f912 	bl	3fe34 <__aeabi_dcmple>
   3bc10:	b1a0      	cbz	r0, 3bc3c <_strtod_l+0xb5c>
   3bc12:	4649      	mov	r1, r9
   3bc14:	4640      	mov	r0, r8
   3bc16:	f004 f969 	bl	3feec <__aeabi_d2uiz>
   3bc1a:	2801      	cmp	r0, #1
   3bc1c:	bf38      	it	cc
   3bc1e:	2001      	movcc	r0, #1
   3bc20:	f003 fe12 	bl	3f848 <__aeabi_ui2d>
   3bc24:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
   3bc26:	4680      	mov	r8, r0
   3bc28:	4689      	mov	r9, r1
   3bc2a:	bb23      	cbnz	r3, 3bc76 <_strtod_l+0xb96>
   3bc2c:	f101 4300 	add.w	r3, r1, #2147483648	; 0x80000000
   3bc30:	9010      	str	r0, [sp, #64]	; 0x40
   3bc32:	9311      	str	r3, [sp, #68]	; 0x44
   3bc34:	e9dd 2310 	ldrd	r2, r3, [sp, #64]	; 0x40
   3bc38:	e9cd 2308 	strd	r2, r3, [sp, #32]
   3bc3c:	9b09      	ldr	r3, [sp, #36]	; 0x24
   3bc3e:	9a0d      	ldr	r2, [sp, #52]	; 0x34
   3bc40:	f103 63d6 	add.w	r3, r3, #112197632	; 0x6b00000
   3bc44:	1a9b      	subs	r3, r3, r2
   3bc46:	9309      	str	r3, [sp, #36]	; 0x24
   3bc48:	eeb0 0a48 	vmov.f32	s0, s16
   3bc4c:	eef0 0a68 	vmov.f32	s1, s17
   3bc50:	e9dd 0108 	ldrd	r0, r1, [sp, #32]
   3bc54:	e9cd 0108 	strd	r0, r1, [sp, #32]
   3bc58:	f7fe fd36 	bl	3a6c8 <__ulp>
   3bc5c:	e9dd 0108 	ldrd	r0, r1, [sp, #32]
   3bc60:	ec53 2b10 	vmov	r2, r3, d0
   3bc64:	f003 fe6a 	bl	3f93c <__aeabi_dmul>
   3bc68:	ec53 2b18 	vmov	r2, r3, d8
   3bc6c:	f003 fcb0 	bl	3f5d0 <__adddf3>
   3bc70:	4682      	mov	sl, r0
   3bc72:	468b      	mov	fp, r1
   3bc74:	e78d      	b.n	3bb92 <_strtod_l+0xab2>
   3bc76:	e9cd 8910 	strd	r8, r9, [sp, #64]	; 0x40
   3bc7a:	e7db      	b.n	3bc34 <_strtod_l+0xb54>
   3bc7c:	a310      	add	r3, pc, #64	; (adr r3, 3bcc0 <_strtod_l+0xbe0>)
   3bc7e:	e9d3 2300 	ldrd	r2, r3, [r3]
   3bc82:	f004 f8cd 	bl	3fe20 <__aeabi_dcmplt>
   3bc86:	e7b2      	b.n	3bbee <_strtod_l+0xb0e>
   3bc88:	2300      	movs	r3, #0
   3bc8a:	930a      	str	r3, [sp, #40]	; 0x28
   3bc8c:	9b17      	ldr	r3, [sp, #92]	; 0x5c
   3bc8e:	9a13      	ldr	r2, [sp, #76]	; 0x4c
   3bc90:	6013      	str	r3, [r2, #0]
   3bc92:	f7ff ba6d 	b.w	3b170 <_strtod_l+0x90>
   3bc96:	2a65      	cmp	r2, #101	; 0x65
   3bc98:	f43f ab61 	beq.w	3b35e <_strtod_l+0x27e>
   3bc9c:	2a45      	cmp	r2, #69	; 0x45
   3bc9e:	f43f ab5e 	beq.w	3b35e <_strtod_l+0x27e>
   3bca2:	2301      	movs	r3, #1
   3bca4:	f7ff bb96 	b.w	3b3d4 <_strtod_l+0x2f4>
   3bca8:	94a03595 	.word	0x94a03595
   3bcac:	3fdfffff 	.word	0x3fdfffff
   3bcb0:	35afe535 	.word	0x35afe535
   3bcb4:	3fe00000 	.word	0x3fe00000
   3bcb8:	ffc00000 	.word	0xffc00000
   3bcbc:	41dfffff 	.word	0x41dfffff
   3bcc0:	94a03595 	.word	0x94a03595
   3bcc4:	3fcfffff 	.word	0x3fcfffff
   3bcc8:	3ff00000 	.word	0x3ff00000
   3bccc:	7ff00000 	.word	0x7ff00000
   3bcd0:	7fe00000 	.word	0x7fe00000
   3bcd4:	7c9fffff 	.word	0x7c9fffff
   3bcd8:	3fe00000 	.word	0x3fe00000
   3bcdc:	bff00000 	.word	0xbff00000
   3bce0:	7fefffff 	.word	0x7fefffff

0003bce4 <_strtod_r>:
   3bce4:	4b01      	ldr	r3, [pc, #4]	; (3bcec <_strtod_r+0x8>)
   3bce6:	f7ff b9fb 	b.w	3b0e0 <_strtod_l>
   3bcea:	bf00      	nop
   3bcec:	20006430 	.word	0x20006430

0003bcf0 <strtod_l>:
   3bcf0:	4613      	mov	r3, r2
   3bcf2:	460a      	mov	r2, r1
   3bcf4:	4601      	mov	r1, r0
   3bcf6:	4802      	ldr	r0, [pc, #8]	; (3bd00 <strtod_l+0x10>)
   3bcf8:	6800      	ldr	r0, [r0, #0]
   3bcfa:	f7ff b9f1 	b.w	3b0e0 <_strtod_l>
   3bcfe:	bf00      	nop
   3bd00:	20006000 	.word	0x20006000

0003bd04 <strtod>:
   3bd04:	460a      	mov	r2, r1
   3bd06:	4601      	mov	r1, r0
   3bd08:	4802      	ldr	r0, [pc, #8]	; (3bd14 <strtod+0x10>)
   3bd0a:	4b03      	ldr	r3, [pc, #12]	; (3bd18 <strtod+0x14>)
   3bd0c:	6800      	ldr	r0, [r0, #0]
   3bd0e:	f7ff b9e7 	b.w	3b0e0 <_strtod_l>
   3bd12:	bf00      	nop
   3bd14:	20006000 	.word	0x20006000
   3bd18:	20006430 	.word	0x20006430

0003bd1c <strtof_l>:
   3bd1c:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
   3bd20:	f8df 80a8 	ldr.w	r8, [pc, #168]	; 3bdcc <strtof_l+0xb0>
   3bd24:	4613      	mov	r3, r2
   3bd26:	460a      	mov	r2, r1
   3bd28:	4601      	mov	r1, r0
   3bd2a:	f8d8 0000 	ldr.w	r0, [r8]
   3bd2e:	ed2d 8b02 	vpush	{d8}
   3bd32:	f7ff f9d5 	bl	3b0e0 <_strtod_l>
   3bd36:	ee10 2a10 	vmov	r2, s0
   3bd3a:	ee10 0a10 	vmov	r0, s0
   3bd3e:	ec55 4b10 	vmov	r4, r5, d0
   3bd42:	462b      	mov	r3, r5
   3bd44:	4629      	mov	r1, r5
   3bd46:	f004 f893 	bl	3fe70 <__aeabi_dcmpun>
   3bd4a:	b190      	cbz	r0, 3bd72 <strtof_l+0x56>
   3bd4c:	2d00      	cmp	r5, #0
   3bd4e:	481c      	ldr	r0, [pc, #112]	; (3bdc0 <strtof_l+0xa4>)
   3bd50:	da09      	bge.n	3bd66 <strtof_l+0x4a>
   3bd52:	f7ff f881 	bl	3ae58 <nanf>
   3bd56:	eeb1 8a40 	vneg.f32	s16, s0
   3bd5a:	eeb0 0a48 	vmov.f32	s0, s16
   3bd5e:	ecbd 8b02 	vpop	{d8}
   3bd62:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
   3bd66:	ecbd 8b02 	vpop	{d8}
   3bd6a:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
   3bd6e:	f7ff b873 	b.w	3ae58 <nanf>
   3bd72:	4620      	mov	r0, r4
   3bd74:	4629      	mov	r1, r5
   3bd76:	f004 f8d9 	bl	3ff2c <__aeabi_d2f>
   3bd7a:	ee08 0a10 	vmov	s16, r0
   3bd7e:	eddf 7a11 	vldr	s15, [pc, #68]	; 3bdc4 <strtof_l+0xa8>
   3bd82:	eeb0 7ac8 	vabs.f32	s14, s16
   3bd86:	eeb4 7a67 	vcmp.f32	s14, s15
   3bd8a:	eef1 fa10 	vmrs	APSR_nzcv, fpscr
   3bd8e:	dde4      	ble.n	3bd5a <strtof_l+0x3e>
   3bd90:	f025 4700 	bic.w	r7, r5, #2147483648	; 0x80000000
   3bd94:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3bd98:	4b0b      	ldr	r3, [pc, #44]	; (3bdc8 <strtof_l+0xac>)
   3bd9a:	4620      	mov	r0, r4
   3bd9c:	4639      	mov	r1, r7
   3bd9e:	f004 f867 	bl	3fe70 <__aeabi_dcmpun>
   3bda2:	b940      	cbnz	r0, 3bdb6 <strtof_l+0x9a>
   3bda4:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3bda8:	4b07      	ldr	r3, [pc, #28]	; (3bdc8 <strtof_l+0xac>)
   3bdaa:	4620      	mov	r0, r4
   3bdac:	4639      	mov	r1, r7
   3bdae:	f004 f841 	bl	3fe34 <__aeabi_dcmple>
   3bdb2:	2800      	cmp	r0, #0
   3bdb4:	d0d1      	beq.n	3bd5a <strtof_l+0x3e>
   3bdb6:	f8d8 3000 	ldr.w	r3, [r8]
   3bdba:	2222      	movs	r2, #34	; 0x22
   3bdbc:	601a      	str	r2, [r3, #0]
   3bdbe:	e7cc      	b.n	3bd5a <strtof_l+0x3e>
   3bdc0:	0004099d 	.word	0x0004099d
   3bdc4:	7f7fffff 	.word	0x7f7fffff
   3bdc8:	7fefffff 	.word	0x7fefffff
   3bdcc:	20006000 	.word	0x20006000

0003bdd0 <strtof>:
   3bdd0:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
   3bdd4:	f8df 80ac 	ldr.w	r8, [pc, #172]	; 3be84 <strtof+0xb4>
   3bdd8:	460a      	mov	r2, r1
   3bdda:	4b26      	ldr	r3, [pc, #152]	; (3be74 <strtof+0xa4>)
   3bddc:	4601      	mov	r1, r0
   3bdde:	f8d8 0000 	ldr.w	r0, [r8]
   3bde2:	ed2d 8b02 	vpush	{d8}
   3bde6:	f7ff f97b 	bl	3b0e0 <_strtod_l>
   3bdea:	ee10 2a10 	vmov	r2, s0
   3bdee:	ee10 0a10 	vmov	r0, s0
   3bdf2:	ec55 4b10 	vmov	r4, r5, d0
   3bdf6:	462b      	mov	r3, r5
   3bdf8:	4629      	mov	r1, r5
   3bdfa:	f004 f839 	bl	3fe70 <__aeabi_dcmpun>
   3bdfe:	b190      	cbz	r0, 3be26 <strtof+0x56>
   3be00:	2d00      	cmp	r5, #0
   3be02:	481d      	ldr	r0, [pc, #116]	; (3be78 <strtof+0xa8>)
   3be04:	da09      	bge.n	3be1a <strtof+0x4a>
   3be06:	f7ff f827 	bl	3ae58 <nanf>
   3be0a:	eeb1 8a40 	vneg.f32	s16, s0
   3be0e:	eeb0 0a48 	vmov.f32	s0, s16
   3be12:	ecbd 8b02 	vpop	{d8}
   3be16:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
   3be1a:	ecbd 8b02 	vpop	{d8}
   3be1e:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
   3be22:	f7ff b819 	b.w	3ae58 <nanf>
   3be26:	4620      	mov	r0, r4
   3be28:	4629      	mov	r1, r5
   3be2a:	f004 f87f 	bl	3ff2c <__aeabi_d2f>
   3be2e:	ee08 0a10 	vmov	s16, r0
   3be32:	eddf 7a12 	vldr	s15, [pc, #72]	; 3be7c <strtof+0xac>
   3be36:	eeb0 7ac8 	vabs.f32	s14, s16
   3be3a:	eeb4 7a67 	vcmp.f32	s14, s15
   3be3e:	eef1 fa10 	vmrs	APSR_nzcv, fpscr
   3be42:	dde4      	ble.n	3be0e <strtof+0x3e>
   3be44:	f025 4700 	bic.w	r7, r5, #2147483648	; 0x80000000
   3be48:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3be4c:	4b0c      	ldr	r3, [pc, #48]	; (3be80 <strtof+0xb0>)
   3be4e:	4620      	mov	r0, r4
   3be50:	4639      	mov	r1, r7
   3be52:	f004 f80d 	bl	3fe70 <__aeabi_dcmpun>
   3be56:	b940      	cbnz	r0, 3be6a <strtof+0x9a>
   3be58:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3be5c:	4b08      	ldr	r3, [pc, #32]	; (3be80 <strtof+0xb0>)
   3be5e:	4620      	mov	r0, r4
   3be60:	4639      	mov	r1, r7
   3be62:	f003 ffe7 	bl	3fe34 <__aeabi_dcmple>
   3be66:	2800      	cmp	r0, #0
   3be68:	d0d1      	beq.n	3be0e <strtof+0x3e>
   3be6a:	f8d8 3000 	ldr.w	r3, [r8]
   3be6e:	2222      	movs	r2, #34	; 0x22
   3be70:	601a      	str	r2, [r3, #0]
   3be72:	e7cc      	b.n	3be0e <strtof+0x3e>
   3be74:	20006430 	.word	0x20006430
   3be78:	0004099d 	.word	0x0004099d
   3be7c:	7f7fffff 	.word	0x7f7fffff
   3be80:	7fefffff 	.word	0x7fefffff
   3be84:	20006000 	.word	0x20006000

0003be88 <_strtoll_l.constprop.0>:
   3be88:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3be8c:	b085      	sub	sp, #20
   3be8e:	4688      	mov	r8, r1
   3be90:	461e      	mov	r6, r3
   3be92:	460d      	mov	r5, r1
   3be94:	9201      	str	r2, [sp, #4]
   3be96:	4a46      	ldr	r2, [pc, #280]	; (3bfb0 <_strtoll_l.constprop.0+0x128>)
   3be98:	9003      	str	r0, [sp, #12]
   3be9a:	462b      	mov	r3, r5
   3be9c:	f815 4b01 	ldrb.w	r4, [r5], #1
   3bea0:	5ca7      	ldrb	r7, [r4, r2]
   3bea2:	f017 0708 	ands.w	r7, r7, #8
   3bea6:	d1f8      	bne.n	3be9a <_strtoll_l.constprop.0+0x12>
   3bea8:	2c2d      	cmp	r4, #45	; 0x2d
   3beaa:	d148      	bne.n	3bf3e <_strtoll_l.constprop.0+0xb6>
   3beac:	782c      	ldrb	r4, [r5, #0]
   3beae:	2701      	movs	r7, #1
   3beb0:	1c9d      	adds	r5, r3, #2
   3beb2:	2e00      	cmp	r6, #0
   3beb4:	d078      	beq.n	3bfa8 <_strtoll_l.constprop.0+0x120>
   3beb6:	2e10      	cmp	r6, #16
   3beb8:	d109      	bne.n	3bece <_strtoll_l.constprop.0+0x46>
   3beba:	2c30      	cmp	r4, #48	; 0x30
   3bebc:	d107      	bne.n	3bece <_strtoll_l.constprop.0+0x46>
   3bebe:	782b      	ldrb	r3, [r5, #0]
   3bec0:	f003 03df 	and.w	r3, r3, #223	; 0xdf
   3bec4:	2b58      	cmp	r3, #88	; 0x58
   3bec6:	d16a      	bne.n	3bf9e <_strtoll_l.constprop.0+0x116>
   3bec8:	786c      	ldrb	r4, [r5, #1]
   3beca:	2610      	movs	r6, #16
   3becc:	3502      	adds	r5, #2
   3bece:	f107 4a00 	add.w	sl, r7, #2147483648	; 0x80000000
   3bed2:	f107 3bff 	add.w	fp, r7, #4294967295	; 0xffffffff
   3bed6:	ea4f 79e6 	mov.w	r9, r6, asr #31
   3beda:	4632      	mov	r2, r6
   3bedc:	f10a 3aff 	add.w	sl, sl, #4294967295	; 0xffffffff
   3bee0:	4658      	mov	r0, fp
   3bee2:	464b      	mov	r3, r9
   3bee4:	4651      	mov	r1, sl
   3bee6:	f004 f871 	bl	3ffcc <__aeabi_uldivmod>
   3beea:	2300      	movs	r3, #0
   3beec:	468c      	mov	ip, r1
   3beee:	9202      	str	r2, [sp, #8]
   3bef0:	4619      	mov	r1, r3
   3bef2:	4602      	mov	r2, r0
   3bef4:	4618      	mov	r0, r3
   3bef6:	f1a4 0e30 	sub.w	lr, r4, #48	; 0x30
   3befa:	f1be 0f09 	cmp.w	lr, #9
   3befe:	d823      	bhi.n	3bf48 <_strtoll_l.constprop.0+0xc0>
   3bf00:	4674      	mov	r4, lr
   3bf02:	42a6      	cmp	r6, r4
   3bf04:	dd31      	ble.n	3bf6a <_strtoll_l.constprop.0+0xe2>
   3bf06:	f1b3 3fff 	cmp.w	r3, #4294967295	; 0xffffffff
   3bf0a:	d015      	beq.n	3bf38 <_strtoll_l.constprop.0+0xb0>
   3bf0c:	4282      	cmp	r2, r0
   3bf0e:	eb7c 0301 	sbcs.w	r3, ip, r1
   3bf12:	d327      	bcc.n	3bf64 <_strtoll_l.constprop.0+0xdc>
   3bf14:	458c      	cmp	ip, r1
   3bf16:	bf08      	it	eq
   3bf18:	4282      	cmpeq	r2, r0
   3bf1a:	d102      	bne.n	3bf22 <_strtoll_l.constprop.0+0x9a>
   3bf1c:	9b02      	ldr	r3, [sp, #8]
   3bf1e:	42a3      	cmp	r3, r4
   3bf20:	db20      	blt.n	3bf64 <_strtoll_l.constprop.0+0xdc>
   3bf22:	4371      	muls	r1, r6
   3bf24:	fb00 1109 	mla	r1, r0, r9, r1
   3bf28:	fba6 0300 	umull	r0, r3, r6, r0
   3bf2c:	4419      	add	r1, r3
   3bf2e:	1820      	adds	r0, r4, r0
   3bf30:	f04f 0301 	mov.w	r3, #1
   3bf34:	eb41 71e4 	adc.w	r1, r1, r4, asr #31
   3bf38:	f815 4b01 	ldrb.w	r4, [r5], #1
   3bf3c:	e7db      	b.n	3bef6 <_strtoll_l.constprop.0+0x6e>
   3bf3e:	2c2b      	cmp	r4, #43	; 0x2b
   3bf40:	bf04      	itt	eq
   3bf42:	782c      	ldrbeq	r4, [r5, #0]
   3bf44:	1c9d      	addeq	r5, r3, #2
   3bf46:	e7b4      	b.n	3beb2 <_strtoll_l.constprop.0+0x2a>
   3bf48:	f1a4 0e41 	sub.w	lr, r4, #65	; 0x41
   3bf4c:	f1be 0f19 	cmp.w	lr, #25
   3bf50:	d801      	bhi.n	3bf56 <_strtoll_l.constprop.0+0xce>
   3bf52:	3c37      	subs	r4, #55	; 0x37
   3bf54:	e7d5      	b.n	3bf02 <_strtoll_l.constprop.0+0x7a>
   3bf56:	f1a4 0e61 	sub.w	lr, r4, #97	; 0x61
   3bf5a:	f1be 0f19 	cmp.w	lr, #25
   3bf5e:	d804      	bhi.n	3bf6a <_strtoll_l.constprop.0+0xe2>
   3bf60:	3c57      	subs	r4, #87	; 0x57
   3bf62:	e7ce      	b.n	3bf02 <_strtoll_l.constprop.0+0x7a>
   3bf64:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3bf68:	e7e6      	b.n	3bf38 <_strtoll_l.constprop.0+0xb0>
   3bf6a:	1c5a      	adds	r2, r3, #1
   3bf6c:	d109      	bne.n	3bf82 <_strtoll_l.constprop.0+0xfa>
   3bf6e:	2322      	movs	r3, #34	; 0x22
   3bf70:	9a03      	ldr	r2, [sp, #12]
   3bf72:	4658      	mov	r0, fp
   3bf74:	4651      	mov	r1, sl
   3bf76:	6013      	str	r3, [r2, #0]
   3bf78:	9b01      	ldr	r3, [sp, #4]
   3bf7a:	b953      	cbnz	r3, 3bf92 <_strtoll_l.constprop.0+0x10a>
   3bf7c:	b005      	add	sp, #20
   3bf7e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3bf82:	b117      	cbz	r7, 3bf8a <_strtoll_l.constprop.0+0x102>
   3bf84:	4240      	negs	r0, r0
   3bf86:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
   3bf8a:	9a01      	ldr	r2, [sp, #4]
   3bf8c:	2a00      	cmp	r2, #0
   3bf8e:	d0f5      	beq.n	3bf7c <_strtoll_l.constprop.0+0xf4>
   3bf90:	b10b      	cbz	r3, 3bf96 <_strtoll_l.constprop.0+0x10e>
   3bf92:	f105 38ff 	add.w	r8, r5, #4294967295	; 0xffffffff
   3bf96:	9b01      	ldr	r3, [sp, #4]
   3bf98:	f8c3 8000 	str.w	r8, [r3]
   3bf9c:	e7ee      	b.n	3bf7c <_strtoll_l.constprop.0+0xf4>
   3bf9e:	2430      	movs	r4, #48	; 0x30
   3bfa0:	2e00      	cmp	r6, #0
   3bfa2:	d194      	bne.n	3bece <_strtoll_l.constprop.0+0x46>
   3bfa4:	2608      	movs	r6, #8
   3bfa6:	e792      	b.n	3bece <_strtoll_l.constprop.0+0x46>
   3bfa8:	2c30      	cmp	r4, #48	; 0x30
   3bfaa:	d088      	beq.n	3bebe <_strtoll_l.constprop.0+0x36>
   3bfac:	260a      	movs	r6, #10
   3bfae:	e78e      	b.n	3bece <_strtoll_l.constprop.0+0x46>
   3bfb0:	0004056b 	.word	0x0004056b

0003bfb4 <_strtoll_r>:
   3bfb4:	e768      	b.n	3be88 <_strtoll_l.constprop.0>

0003bfb6 <strtoll_l>:
   3bfb6:	4613      	mov	r3, r2
   3bfb8:	460a      	mov	r2, r1
   3bfba:	4601      	mov	r1, r0
   3bfbc:	4801      	ldr	r0, [pc, #4]	; (3bfc4 <strtoll_l+0xe>)
   3bfbe:	6800      	ldr	r0, [r0, #0]
   3bfc0:	e762      	b.n	3be88 <_strtoll_l.constprop.0>
   3bfc2:	bf00      	nop
   3bfc4:	20006000 	.word	0x20006000

0003bfc8 <strtoll>:
   3bfc8:	4613      	mov	r3, r2
   3bfca:	460a      	mov	r2, r1
   3bfcc:	4601      	mov	r1, r0
   3bfce:	4801      	ldr	r0, [pc, #4]	; (3bfd4 <strtoll+0xc>)
   3bfd0:	6800      	ldr	r0, [r0, #0]
   3bfd2:	e759      	b.n	3be88 <_strtoll_l.constprop.0>
   3bfd4:	20006000 	.word	0x20006000

0003bfd8 <_strtoull_l.constprop.0>:
   3bfd8:	e92d 4ff7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3bfdc:	4692      	mov	sl, r2
   3bfde:	4689      	mov	r9, r1
   3bfe0:	461d      	mov	r5, r3
   3bfe2:	460e      	mov	r6, r1
   3bfe4:	4a48      	ldr	r2, [pc, #288]	; (3c108 <_strtoull_l.constprop.0+0x130>)
   3bfe6:	9001      	str	r0, [sp, #4]
   3bfe8:	4633      	mov	r3, r6
   3bfea:	f816 4b01 	ldrb.w	r4, [r6], #1
   3bfee:	5ca7      	ldrb	r7, [r4, r2]
   3bff0:	f017 0708 	ands.w	r7, r7, #8
   3bff4:	d1f8      	bne.n	3bfe8 <_strtoull_l.constprop.0+0x10>
   3bff6:	2c2d      	cmp	r4, #45	; 0x2d
   3bff8:	d14b      	bne.n	3c092 <_strtoull_l.constprop.0+0xba>
   3bffa:	7834      	ldrb	r4, [r6, #0]
   3bffc:	2701      	movs	r7, #1
   3bffe:	1c9e      	adds	r6, r3, #2
   3c000:	2d00      	cmp	r5, #0
   3c002:	d07c      	beq.n	3c0fe <_strtoull_l.constprop.0+0x126>
   3c004:	2d10      	cmp	r5, #16
   3c006:	d109      	bne.n	3c01c <_strtoull_l.constprop.0+0x44>
   3c008:	2c30      	cmp	r4, #48	; 0x30
   3c00a:	d107      	bne.n	3c01c <_strtoull_l.constprop.0+0x44>
   3c00c:	7833      	ldrb	r3, [r6, #0]
   3c00e:	f003 03df 	and.w	r3, r3, #223	; 0xdf
   3c012:	2b58      	cmp	r3, #88	; 0x58
   3c014:	d16e      	bne.n	3c0f4 <_strtoull_l.constprop.0+0x11c>
   3c016:	7874      	ldrb	r4, [r6, #1]
   3c018:	2510      	movs	r5, #16
   3c01a:	3602      	adds	r6, #2
   3c01c:	ea4f 7be5 	mov.w	fp, r5, asr #31
   3c020:	462a      	mov	r2, r5
   3c022:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3c026:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
   3c02a:	465b      	mov	r3, fp
   3c02c:	f003 ffce 	bl	3ffcc <__aeabi_uldivmod>
   3c030:	462a      	mov	r2, r5
   3c032:	9000      	str	r0, [sp, #0]
   3c034:	4688      	mov	r8, r1
   3c036:	465b      	mov	r3, fp
   3c038:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3c03c:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
   3c040:	f003 ffc4 	bl	3ffcc <__aeabi_uldivmod>
   3c044:	2300      	movs	r3, #0
   3c046:	4618      	mov	r0, r3
   3c048:	4619      	mov	r1, r3
   3c04a:	f1a4 0c30 	sub.w	ip, r4, #48	; 0x30
   3c04e:	f1bc 0f09 	cmp.w	ip, #9
   3c052:	d823      	bhi.n	3c09c <_strtoull_l.constprop.0+0xc4>
   3c054:	4664      	mov	r4, ip
   3c056:	42a5      	cmp	r5, r4
   3c058:	dd31      	ble.n	3c0be <_strtoull_l.constprop.0+0xe6>
   3c05a:	2b00      	cmp	r3, #0
   3c05c:	db2c      	blt.n	3c0b8 <_strtoull_l.constprop.0+0xe0>
   3c05e:	9b00      	ldr	r3, [sp, #0]
   3c060:	4283      	cmp	r3, r0
   3c062:	eb78 0301 	sbcs.w	r3, r8, r1
   3c066:	d327      	bcc.n	3c0b8 <_strtoull_l.constprop.0+0xe0>
   3c068:	9b00      	ldr	r3, [sp, #0]
   3c06a:	4588      	cmp	r8, r1
   3c06c:	bf08      	it	eq
   3c06e:	4283      	cmpeq	r3, r0
   3c070:	d101      	bne.n	3c076 <_strtoull_l.constprop.0+0x9e>
   3c072:	42a2      	cmp	r2, r4
   3c074:	db20      	blt.n	3c0b8 <_strtoull_l.constprop.0+0xe0>
   3c076:	4369      	muls	r1, r5
   3c078:	fb00 110b 	mla	r1, r0, fp, r1
   3c07c:	fba5 0300 	umull	r0, r3, r5, r0
   3c080:	4419      	add	r1, r3
   3c082:	1820      	adds	r0, r4, r0
   3c084:	f04f 0301 	mov.w	r3, #1
   3c088:	eb41 71e4 	adc.w	r1, r1, r4, asr #31
   3c08c:	f816 4b01 	ldrb.w	r4, [r6], #1
   3c090:	e7db      	b.n	3c04a <_strtoull_l.constprop.0+0x72>
   3c092:	2c2b      	cmp	r4, #43	; 0x2b
   3c094:	bf04      	itt	eq
   3c096:	7834      	ldrbeq	r4, [r6, #0]
   3c098:	1c9e      	addeq	r6, r3, #2
   3c09a:	e7b1      	b.n	3c000 <_strtoull_l.constprop.0+0x28>
   3c09c:	f1a4 0c41 	sub.w	ip, r4, #65	; 0x41
   3c0a0:	f1bc 0f19 	cmp.w	ip, #25
   3c0a4:	d801      	bhi.n	3c0aa <_strtoull_l.constprop.0+0xd2>
   3c0a6:	3c37      	subs	r4, #55	; 0x37
   3c0a8:	e7d5      	b.n	3c056 <_strtoull_l.constprop.0+0x7e>
   3c0aa:	f1a4 0c61 	sub.w	ip, r4, #97	; 0x61
   3c0ae:	f1bc 0f19 	cmp.w	ip, #25
   3c0b2:	d804      	bhi.n	3c0be <_strtoull_l.constprop.0+0xe6>
   3c0b4:	3c57      	subs	r4, #87	; 0x57
   3c0b6:	e7ce      	b.n	3c056 <_strtoull_l.constprop.0+0x7e>
   3c0b8:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3c0bc:	e7e6      	b.n	3c08c <_strtoull_l.constprop.0+0xb4>
   3c0be:	2b00      	cmp	r3, #0
   3c0c0:	da0b      	bge.n	3c0da <_strtoull_l.constprop.0+0x102>
   3c0c2:	2322      	movs	r3, #34	; 0x22
   3c0c4:	9a01      	ldr	r2, [sp, #4]
   3c0c6:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3c0ca:	6013      	str	r3, [r2, #0]
   3c0cc:	4601      	mov	r1, r0
   3c0ce:	f1ba 0f00 	cmp.w	sl, #0
   3c0d2:	d10a      	bne.n	3c0ea <_strtoull_l.constprop.0+0x112>
   3c0d4:	b003      	add	sp, #12
   3c0d6:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3c0da:	b117      	cbz	r7, 3c0e2 <_strtoull_l.constprop.0+0x10a>
   3c0dc:	4240      	negs	r0, r0
   3c0de:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
   3c0e2:	f1ba 0f00 	cmp.w	sl, #0
   3c0e6:	d0f5      	beq.n	3c0d4 <_strtoull_l.constprop.0+0xfc>
   3c0e8:	b10b      	cbz	r3, 3c0ee <_strtoull_l.constprop.0+0x116>
   3c0ea:	f106 39ff 	add.w	r9, r6, #4294967295	; 0xffffffff
   3c0ee:	f8ca 9000 	str.w	r9, [sl]
   3c0f2:	e7ef      	b.n	3c0d4 <_strtoull_l.constprop.0+0xfc>
   3c0f4:	2430      	movs	r4, #48	; 0x30
   3c0f6:	2d00      	cmp	r5, #0
   3c0f8:	d190      	bne.n	3c01c <_strtoull_l.constprop.0+0x44>
   3c0fa:	2508      	movs	r5, #8
   3c0fc:	e78e      	b.n	3c01c <_strtoull_l.constprop.0+0x44>
   3c0fe:	2c30      	cmp	r4, #48	; 0x30
   3c100:	d084      	beq.n	3c00c <_strtoull_l.constprop.0+0x34>
   3c102:	250a      	movs	r5, #10
   3c104:	e78a      	b.n	3c01c <_strtoull_l.constprop.0+0x44>
   3c106:	bf00      	nop
   3c108:	0004056b 	.word	0x0004056b

0003c10c <_strtoull_r>:
   3c10c:	e764      	b.n	3bfd8 <_strtoull_l.constprop.0>

0003c10e <strtoull_l>:
   3c10e:	4613      	mov	r3, r2
   3c110:	460a      	mov	r2, r1
   3c112:	4601      	mov	r1, r0
   3c114:	4801      	ldr	r0, [pc, #4]	; (3c11c <strtoull_l+0xe>)
   3c116:	6800      	ldr	r0, [r0, #0]
   3c118:	e75e      	b.n	3bfd8 <_strtoull_l.constprop.0>
   3c11a:	bf00      	nop
   3c11c:	20006000 	.word	0x20006000

0003c120 <strtoull>:
   3c120:	4613      	mov	r3, r2
   3c122:	460a      	mov	r2, r1
   3c124:	4601      	mov	r1, r0
   3c126:	4801      	ldr	r0, [pc, #4]	; (3c12c <strtoull+0xc>)
   3c128:	6800      	ldr	r0, [r0, #0]
   3c12a:	e755      	b.n	3bfd8 <_strtoull_l.constprop.0>
   3c12c:	20006000 	.word	0x20006000

0003c130 <_strtoul_l.constprop.0>:
   3c130:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
   3c134:	4686      	mov	lr, r0
   3c136:	460d      	mov	r5, r1
   3c138:	4f35      	ldr	r7, [pc, #212]	; (3c210 <_strtoul_l.constprop.0+0xe0>)
   3c13a:	4628      	mov	r0, r5
   3c13c:	f815 4b01 	ldrb.w	r4, [r5], #1
   3c140:	5de6      	ldrb	r6, [r4, r7]
   3c142:	f016 0608 	ands.w	r6, r6, #8
   3c146:	d1f8      	bne.n	3c13a <_strtoul_l.constprop.0+0xa>
   3c148:	2c2d      	cmp	r4, #45	; 0x2d
   3c14a:	d12f      	bne.n	3c1ac <_strtoul_l.constprop.0+0x7c>
   3c14c:	782c      	ldrb	r4, [r5, #0]
   3c14e:	2601      	movs	r6, #1
   3c150:	1c85      	adds	r5, r0, #2
   3c152:	2b00      	cmp	r3, #0
   3c154:	d057      	beq.n	3c206 <_strtoul_l.constprop.0+0xd6>
   3c156:	2b10      	cmp	r3, #16
   3c158:	d109      	bne.n	3c16e <_strtoul_l.constprop.0+0x3e>
   3c15a:	2c30      	cmp	r4, #48	; 0x30
   3c15c:	d107      	bne.n	3c16e <_strtoul_l.constprop.0+0x3e>
   3c15e:	7828      	ldrb	r0, [r5, #0]
   3c160:	f000 00df 	and.w	r0, r0, #223	; 0xdf
   3c164:	2858      	cmp	r0, #88	; 0x58
   3c166:	d149      	bne.n	3c1fc <_strtoul_l.constprop.0+0xcc>
   3c168:	786c      	ldrb	r4, [r5, #1]
   3c16a:	2310      	movs	r3, #16
   3c16c:	3502      	adds	r5, #2
   3c16e:	f04f 38ff 	mov.w	r8, #4294967295	; 0xffffffff
   3c172:	2700      	movs	r7, #0
   3c174:	fbb8 f8f3 	udiv	r8, r8, r3
   3c178:	fb03 f908 	mul.w	r9, r3, r8
   3c17c:	4638      	mov	r0, r7
   3c17e:	ea6f 0909 	mvn.w	r9, r9
   3c182:	f1a4 0c30 	sub.w	ip, r4, #48	; 0x30
   3c186:	f1bc 0f09 	cmp.w	ip, #9
   3c18a:	d814      	bhi.n	3c1b6 <_strtoul_l.constprop.0+0x86>
   3c18c:	4664      	mov	r4, ip
   3c18e:	42a3      	cmp	r3, r4
   3c190:	dd22      	ble.n	3c1d8 <_strtoul_l.constprop.0+0xa8>
   3c192:	2f00      	cmp	r7, #0
   3c194:	db1d      	blt.n	3c1d2 <_strtoul_l.constprop.0+0xa2>
   3c196:	4580      	cmp	r8, r0
   3c198:	d31b      	bcc.n	3c1d2 <_strtoul_l.constprop.0+0xa2>
   3c19a:	d101      	bne.n	3c1a0 <_strtoul_l.constprop.0+0x70>
   3c19c:	45a1      	cmp	r9, r4
   3c19e:	db18      	blt.n	3c1d2 <_strtoul_l.constprop.0+0xa2>
   3c1a0:	fb00 4003 	mla	r0, r0, r3, r4
   3c1a4:	2701      	movs	r7, #1
   3c1a6:	f815 4b01 	ldrb.w	r4, [r5], #1
   3c1aa:	e7ea      	b.n	3c182 <_strtoul_l.constprop.0+0x52>
   3c1ac:	2c2b      	cmp	r4, #43	; 0x2b
   3c1ae:	bf04      	itt	eq
   3c1b0:	782c      	ldrbeq	r4, [r5, #0]
   3c1b2:	1c85      	addeq	r5, r0, #2
   3c1b4:	e7cd      	b.n	3c152 <_strtoul_l.constprop.0+0x22>
   3c1b6:	f1a4 0c41 	sub.w	ip, r4, #65	; 0x41
   3c1ba:	f1bc 0f19 	cmp.w	ip, #25
   3c1be:	d801      	bhi.n	3c1c4 <_strtoul_l.constprop.0+0x94>
   3c1c0:	3c37      	subs	r4, #55	; 0x37
   3c1c2:	e7e4      	b.n	3c18e <_strtoul_l.constprop.0+0x5e>
   3c1c4:	f1a4 0c61 	sub.w	ip, r4, #97	; 0x61
   3c1c8:	f1bc 0f19 	cmp.w	ip, #25
   3c1cc:	d804      	bhi.n	3c1d8 <_strtoul_l.constprop.0+0xa8>
   3c1ce:	3c57      	subs	r4, #87	; 0x57
   3c1d0:	e7dd      	b.n	3c18e <_strtoul_l.constprop.0+0x5e>
   3c1d2:	f04f 37ff 	mov.w	r7, #4294967295	; 0xffffffff
   3c1d6:	e7e6      	b.n	3c1a6 <_strtoul_l.constprop.0+0x76>
   3c1d8:	2f00      	cmp	r7, #0
   3c1da:	da07      	bge.n	3c1ec <_strtoul_l.constprop.0+0xbc>
   3c1dc:	2322      	movs	r3, #34	; 0x22
   3c1de:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3c1e2:	f8ce 3000 	str.w	r3, [lr]
   3c1e6:	b932      	cbnz	r2, 3c1f6 <_strtoul_l.constprop.0+0xc6>
   3c1e8:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
   3c1ec:	b106      	cbz	r6, 3c1f0 <_strtoul_l.constprop.0+0xc0>
   3c1ee:	4240      	negs	r0, r0
   3c1f0:	2a00      	cmp	r2, #0
   3c1f2:	d0f9      	beq.n	3c1e8 <_strtoul_l.constprop.0+0xb8>
   3c1f4:	b107      	cbz	r7, 3c1f8 <_strtoul_l.constprop.0+0xc8>
   3c1f6:	1e69      	subs	r1, r5, #1
   3c1f8:	6011      	str	r1, [r2, #0]
   3c1fa:	e7f5      	b.n	3c1e8 <_strtoul_l.constprop.0+0xb8>
   3c1fc:	2430      	movs	r4, #48	; 0x30
   3c1fe:	2b00      	cmp	r3, #0
   3c200:	d1b5      	bne.n	3c16e <_strtoul_l.constprop.0+0x3e>
   3c202:	2308      	movs	r3, #8
   3c204:	e7b3      	b.n	3c16e <_strtoul_l.constprop.0+0x3e>
   3c206:	2c30      	cmp	r4, #48	; 0x30
   3c208:	d0a9      	beq.n	3c15e <_strtoul_l.constprop.0+0x2e>
   3c20a:	230a      	movs	r3, #10
   3c20c:	e7af      	b.n	3c16e <_strtoul_l.constprop.0+0x3e>
   3c20e:	bf00      	nop
   3c210:	0004056b 	.word	0x0004056b

0003c214 <_strtoul_r>:
   3c214:	e78c      	b.n	3c130 <_strtoul_l.constprop.0>

0003c216 <strtoul_l>:
   3c216:	4613      	mov	r3, r2
   3c218:	460a      	mov	r2, r1
   3c21a:	4601      	mov	r1, r0
   3c21c:	4801      	ldr	r0, [pc, #4]	; (3c224 <strtoul_l+0xe>)
   3c21e:	6800      	ldr	r0, [r0, #0]
   3c220:	e786      	b.n	3c130 <_strtoul_l.constprop.0>
   3c222:	bf00      	nop
   3c224:	20006000 	.word	0x20006000

0003c228 <strtoul>:
   3c228:	4613      	mov	r3, r2
   3c22a:	460a      	mov	r2, r1
   3c22c:	4601      	mov	r1, r0
   3c22e:	4801      	ldr	r0, [pc, #4]	; (3c234 <strtoul+0xc>)
   3c230:	6800      	ldr	r0, [r0, #0]
   3c232:	e77d      	b.n	3c130 <_strtoul_l.constprop.0>
   3c234:	20006000 	.word	0x20006000

0003c238 <__ssprint_r>:
   3c238:	6893      	ldr	r3, [r2, #8]
   3c23a:	e92d 4ff7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3c23e:	4680      	mov	r8, r0
   3c240:	460c      	mov	r4, r1
   3c242:	4617      	mov	r7, r2
   3c244:	2b00      	cmp	r3, #0
   3c246:	d061      	beq.n	3c30c <__ssprint_r+0xd4>
   3c248:	2300      	movs	r3, #0
   3c24a:	f8d2 a000 	ldr.w	sl, [r2]
   3c24e:	469b      	mov	fp, r3
   3c250:	9301      	str	r3, [sp, #4]
   3c252:	f1bb 0f00 	cmp.w	fp, #0
   3c256:	d02b      	beq.n	3c2b0 <__ssprint_r+0x78>
   3c258:	68a6      	ldr	r6, [r4, #8]
   3c25a:	455e      	cmp	r6, fp
   3c25c:	d844      	bhi.n	3c2e8 <__ssprint_r+0xb0>
   3c25e:	89a2      	ldrh	r2, [r4, #12]
   3c260:	f412 6f90 	tst.w	r2, #1152	; 0x480
   3c264:	d03e      	beq.n	3c2e4 <__ssprint_r+0xac>
   3c266:	6820      	ldr	r0, [r4, #0]
   3c268:	6921      	ldr	r1, [r4, #16]
   3c26a:	6965      	ldr	r5, [r4, #20]
   3c26c:	eba0 0901 	sub.w	r9, r0, r1
   3c270:	eb05 0545 	add.w	r5, r5, r5, lsl #1
   3c274:	f109 0001 	add.w	r0, r9, #1
   3c278:	eb05 75d5 	add.w	r5, r5, r5, lsr #31
   3c27c:	4458      	add	r0, fp
   3c27e:	106d      	asrs	r5, r5, #1
   3c280:	4285      	cmp	r5, r0
   3c282:	bf38      	it	cc
   3c284:	4605      	movcc	r5, r0
   3c286:	0553      	lsls	r3, r2, #21
   3c288:	d545      	bpl.n	3c316 <__ssprint_r+0xde>
   3c28a:	4629      	mov	r1, r5
   3c28c:	4640      	mov	r0, r8
   3c28e:	f7fd fb17 	bl	398c0 <_malloc_r>
   3c292:	4606      	mov	r6, r0
   3c294:	b9a0      	cbnz	r0, 3c2c0 <__ssprint_r+0x88>
   3c296:	230c      	movs	r3, #12
   3c298:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3c29c:	f8c8 3000 	str.w	r3, [r8]
   3c2a0:	89a3      	ldrh	r3, [r4, #12]
   3c2a2:	f043 0340 	orr.w	r3, r3, #64	; 0x40
   3c2a6:	81a3      	strh	r3, [r4, #12]
   3c2a8:	2300      	movs	r3, #0
   3c2aa:	e9c7 3301 	strd	r3, r3, [r7, #4]
   3c2ae:	e02f      	b.n	3c310 <__ssprint_r+0xd8>
   3c2b0:	f8da 3000 	ldr.w	r3, [sl]
   3c2b4:	f10a 0a08 	add.w	sl, sl, #8
   3c2b8:	f85a bc04 	ldr.w	fp, [sl, #-4]
   3c2bc:	9301      	str	r3, [sp, #4]
   3c2be:	e7c8      	b.n	3c252 <__ssprint_r+0x1a>
   3c2c0:	464a      	mov	r2, r9
   3c2c2:	6921      	ldr	r1, [r4, #16]
   3c2c4:	f7fd fdc8 	bl	39e58 <memcpy>
   3c2c8:	89a2      	ldrh	r2, [r4, #12]
   3c2ca:	f422 6290 	bic.w	r2, r2, #1152	; 0x480
   3c2ce:	f042 0280 	orr.w	r2, r2, #128	; 0x80
   3c2d2:	81a2      	strh	r2, [r4, #12]
   3c2d4:	6126      	str	r6, [r4, #16]
   3c2d6:	444e      	add	r6, r9
   3c2d8:	6165      	str	r5, [r4, #20]
   3c2da:	eba5 0509 	sub.w	r5, r5, r9
   3c2de:	6026      	str	r6, [r4, #0]
   3c2e0:	465e      	mov	r6, fp
   3c2e2:	60a5      	str	r5, [r4, #8]
   3c2e4:	455e      	cmp	r6, fp
   3c2e6:	d900      	bls.n	3c2ea <__ssprint_r+0xb2>
   3c2e8:	465e      	mov	r6, fp
   3c2ea:	4632      	mov	r2, r6
   3c2ec:	9901      	ldr	r1, [sp, #4]
   3c2ee:	6820      	ldr	r0, [r4, #0]
   3c2f0:	f7fd fe28 	bl	39f44 <memmove>
   3c2f4:	68a2      	ldr	r2, [r4, #8]
   3c2f6:	1b92      	subs	r2, r2, r6
   3c2f8:	60a2      	str	r2, [r4, #8]
   3c2fa:	6822      	ldr	r2, [r4, #0]
   3c2fc:	4432      	add	r2, r6
   3c2fe:	6022      	str	r2, [r4, #0]
   3c300:	68ba      	ldr	r2, [r7, #8]
   3c302:	eba2 030b 	sub.w	r3, r2, fp
   3c306:	60bb      	str	r3, [r7, #8]
   3c308:	2b00      	cmp	r3, #0
   3c30a:	d1d1      	bne.n	3c2b0 <__ssprint_r+0x78>
   3c30c:	2000      	movs	r0, #0
   3c30e:	6078      	str	r0, [r7, #4]
   3c310:	b003      	add	sp, #12
   3c312:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3c316:	462a      	mov	r2, r5
   3c318:	4640      	mov	r0, r8
   3c31a:	f7fe fb59 	bl	3a9d0 <_realloc_r>
   3c31e:	4606      	mov	r6, r0
   3c320:	2800      	cmp	r0, #0
   3c322:	d1d7      	bne.n	3c2d4 <__ssprint_r+0x9c>
   3c324:	6921      	ldr	r1, [r4, #16]
   3c326:	4640      	mov	r0, r8
   3c328:	f7fc ffb6 	bl	39298 <_free_r>
   3c32c:	e7b3      	b.n	3c296 <__ssprint_r+0x5e>

0003c32e <_svfiprintf_r>:
   3c32e:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3c332:	461c      	mov	r4, r3
   3c334:	898b      	ldrh	r3, [r1, #12]
   3c336:	4681      	mov	r9, r0
   3c338:	468b      	mov	fp, r1
   3c33a:	061d      	lsls	r5, r3, #24
   3c33c:	ed2d 8b02 	vpush	{d8}
   3c340:	b0b9      	sub	sp, #228	; 0xe4
   3c342:	9201      	str	r2, [sp, #4]
   3c344:	d518      	bpl.n	3c378 <_svfiprintf_r+0x4a>
   3c346:	690b      	ldr	r3, [r1, #16]
   3c348:	b9b3      	cbnz	r3, 3c378 <_svfiprintf_r+0x4a>
   3c34a:	2140      	movs	r1, #64	; 0x40
   3c34c:	f7fd fab8 	bl	398c0 <_malloc_r>
   3c350:	f8cb 0000 	str.w	r0, [fp]
   3c354:	f8cb 0010 	str.w	r0, [fp, #16]
   3c358:	b958      	cbnz	r0, 3c372 <_svfiprintf_r+0x44>
   3c35a:	230c      	movs	r3, #12
   3c35c:	f8c9 3000 	str.w	r3, [r9]
   3c360:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3c364:	9302      	str	r3, [sp, #8]
   3c366:	9802      	ldr	r0, [sp, #8]
   3c368:	b039      	add	sp, #228	; 0xe4
   3c36a:	ecbd 8b02 	vpop	{d8}
   3c36e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3c372:	2340      	movs	r3, #64	; 0x40
   3c374:	f8cb 3014 	str.w	r3, [fp, #20]
   3c378:	2300      	movs	r3, #0
   3c37a:	ae0f      	add	r6, sp, #60	; 0x3c
   3c37c:	ee08 3a10 	vmov	s16, r3
   3c380:	960c      	str	r6, [sp, #48]	; 0x30
   3c382:	9307      	str	r3, [sp, #28]
   3c384:	9302      	str	r3, [sp, #8]
   3c386:	e9cd 330d 	strd	r3, r3, [sp, #52]	; 0x34
   3c38a:	e9cd 3305 	strd	r3, r3, [sp, #20]
   3c38e:	9b01      	ldr	r3, [sp, #4]
   3c390:	461d      	mov	r5, r3
   3c392:	f813 2b01 	ldrb.w	r2, [r3], #1
   3c396:	b10a      	cbz	r2, 3c39c <_svfiprintf_r+0x6e>
   3c398:	2a25      	cmp	r2, #37	; 0x25
   3c39a:	d1f9      	bne.n	3c390 <_svfiprintf_r+0x62>
   3c39c:	9b01      	ldr	r3, [sp, #4]
   3c39e:	1aef      	subs	r7, r5, r3
   3c3a0:	d00d      	beq.n	3c3be <_svfiprintf_r+0x90>
   3c3a2:	e9c6 3700 	strd	r3, r7, [r6]
   3c3a6:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3c3a8:	443b      	add	r3, r7
   3c3aa:	930e      	str	r3, [sp, #56]	; 0x38
   3c3ac:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3c3ae:	3301      	adds	r3, #1
   3c3b0:	2b07      	cmp	r3, #7
   3c3b2:	930d      	str	r3, [sp, #52]	; 0x34
   3c3b4:	dc76      	bgt.n	3c4a4 <_svfiprintf_r+0x176>
   3c3b6:	3608      	adds	r6, #8
   3c3b8:	9b02      	ldr	r3, [sp, #8]
   3c3ba:	443b      	add	r3, r7
   3c3bc:	9302      	str	r3, [sp, #8]
   3c3be:	782b      	ldrb	r3, [r5, #0]
   3c3c0:	2b00      	cmp	r3, #0
   3c3c2:	f000 83d3 	beq.w	3cb6c <_svfiprintf_r+0x83e>
   3c3c6:	2300      	movs	r3, #0
   3c3c8:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
   3c3cc:	1c6a      	adds	r2, r5, #1
   3c3ce:	469a      	mov	sl, r3
   3c3d0:	f88d 302b 	strb.w	r3, [sp, #43]	; 0x2b
   3c3d4:	9100      	str	r1, [sp, #0]
   3c3d6:	9303      	str	r3, [sp, #12]
   3c3d8:	4613      	mov	r3, r2
   3c3da:	f813 1b01 	ldrb.w	r1, [r3], #1
   3c3de:	9301      	str	r3, [sp, #4]
   3c3e0:	f1a1 0320 	sub.w	r3, r1, #32
   3c3e4:	2b5a      	cmp	r3, #90	; 0x5a
   3c3e6:	f200 8321 	bhi.w	3ca2c <_svfiprintf_r+0x6fe>
   3c3ea:	e8df f013 	tbh	[pc, r3, lsl #1]
   3c3ee:	009b      	.short	0x009b
   3c3f0:	031f031f 	.word	0x031f031f
   3c3f4:	031f00a3 	.word	0x031f00a3
   3c3f8:	031f031f 	.word	0x031f031f
   3c3fc:	031f0082 	.word	0x031f0082
   3c400:	00a6031f 	.word	0x00a6031f
   3c404:	031f00b0 	.word	0x031f00b0
   3c408:	00b200ad 	.word	0x00b200ad
   3c40c:	00ce031f 	.word	0x00ce031f
   3c410:	00d100d1 	.word	0x00d100d1
   3c414:	00d100d1 	.word	0x00d100d1
   3c418:	00d100d1 	.word	0x00d100d1
   3c41c:	00d100d1 	.word	0x00d100d1
   3c420:	031f00d1 	.word	0x031f00d1
   3c424:	031f031f 	.word	0x031f031f
   3c428:	031f031f 	.word	0x031f031f
   3c42c:	031f031f 	.word	0x031f031f
   3c430:	031f031f 	.word	0x031f031f
   3c434:	010900fb 	.word	0x010900fb
   3c438:	031f031f 	.word	0x031f031f
   3c43c:	031f031f 	.word	0x031f031f
   3c440:	031f031f 	.word	0x031f031f
   3c444:	031f031f 	.word	0x031f031f
   3c448:	031f031f 	.word	0x031f031f
   3c44c:	031f0159 	.word	0x031f0159
   3c450:	031f031f 	.word	0x031f031f
   3c454:	031f01a1 	.word	0x031f01a1
   3c458:	031f0280 	.word	0x031f0280
   3c45c:	02a0031f 	.word	0x02a0031f
   3c460:	031f031f 	.word	0x031f031f
   3c464:	031f031f 	.word	0x031f031f
   3c468:	031f031f 	.word	0x031f031f
   3c46c:	031f031f 	.word	0x031f031f
   3c470:	031f031f 	.word	0x031f031f
   3c474:	010b00fb 	.word	0x010b00fb
   3c478:	031f031f 	.word	0x031f031f
   3c47c:	00e1031f 	.word	0x00e1031f
   3c480:	00f5010b 	.word	0x00f5010b
   3c484:	00ee031f 	.word	0x00ee031f
   3c488:	0137031f 	.word	0x0137031f
   3c48c:	0190015b 	.word	0x0190015b
   3c490:	031f00f5 	.word	0x031f00f5
   3c494:	009901a1 	.word	0x009901a1
   3c498:	031f0282 	.word	0x031f0282
   3c49c:	0065031f 	.word	0x0065031f
   3c4a0:	0099031f 	.word	0x0099031f
   3c4a4:	aa0c      	add	r2, sp, #48	; 0x30
   3c4a6:	4659      	mov	r1, fp
   3c4a8:	4648      	mov	r0, r9
   3c4aa:	f7ff fec5 	bl	3c238 <__ssprint_r>
   3c4ae:	2800      	cmp	r0, #0
   3c4b0:	f040 8341 	bne.w	3cb36 <_svfiprintf_r+0x808>
   3c4b4:	ae0f      	add	r6, sp, #60	; 0x3c
   3c4b6:	e77f      	b.n	3c3b8 <_svfiprintf_r+0x8a>
   3c4b8:	4b9c      	ldr	r3, [pc, #624]	; (3c72c <_svfiprintf_r+0x3fe>)
   3c4ba:	9305      	str	r3, [sp, #20]
   3c4bc:	f01a 0320 	ands.w	r3, sl, #32
   3c4c0:	f000 8237 	beq.w	3c932 <_svfiprintf_r+0x604>
   3c4c4:	1de3      	adds	r3, r4, #7
   3c4c6:	f023 0307 	bic.w	r3, r3, #7
   3c4ca:	461f      	mov	r7, r3
   3c4cc:	685d      	ldr	r5, [r3, #4]
   3c4ce:	f857 4b08 	ldr.w	r4, [r7], #8
   3c4d2:	f01a 0f01 	tst.w	sl, #1
   3c4d6:	d009      	beq.n	3c4ec <_svfiprintf_r+0x1be>
   3c4d8:	ea54 0305 	orrs.w	r3, r4, r5
   3c4dc:	bf1f      	itttt	ne
   3c4de:	2330      	movne	r3, #48	; 0x30
   3c4e0:	f04a 0a02 	orrne.w	sl, sl, #2
   3c4e4:	f88d 102d 	strbne.w	r1, [sp, #45]	; 0x2d
   3c4e8:	f88d 302c 	strbne.w	r3, [sp, #44]	; 0x2c
   3c4ec:	f42a 6a80 	bic.w	sl, sl, #1024	; 0x400
   3c4f0:	e119      	b.n	3c726 <_svfiprintf_r+0x3f8>
   3c4f2:	4648      	mov	r0, r9
   3c4f4:	f7fd f91c 	bl	39730 <_localeconv_r>
   3c4f8:	6843      	ldr	r3, [r0, #4]
   3c4fa:	4618      	mov	r0, r3
   3c4fc:	ee08 3a10 	vmov	s16, r3
   3c500:	f7f7 fc16 	bl	33d30 <strlen>
   3c504:	9007      	str	r0, [sp, #28]
   3c506:	4648      	mov	r0, r9
   3c508:	f7fd f912 	bl	39730 <_localeconv_r>
   3c50c:	6883      	ldr	r3, [r0, #8]
   3c50e:	9306      	str	r3, [sp, #24]
   3c510:	9b07      	ldr	r3, [sp, #28]
   3c512:	b12b      	cbz	r3, 3c520 <_svfiprintf_r+0x1f2>
   3c514:	9b06      	ldr	r3, [sp, #24]
   3c516:	b11b      	cbz	r3, 3c520 <_svfiprintf_r+0x1f2>
   3c518:	781b      	ldrb	r3, [r3, #0]
   3c51a:	b10b      	cbz	r3, 3c520 <_svfiprintf_r+0x1f2>
   3c51c:	f44a 6a80 	orr.w	sl, sl, #1024	; 0x400
   3c520:	9a01      	ldr	r2, [sp, #4]
   3c522:	e759      	b.n	3c3d8 <_svfiprintf_r+0xaa>
   3c524:	f89d 302b 	ldrb.w	r3, [sp, #43]	; 0x2b
   3c528:	2b00      	cmp	r3, #0
   3c52a:	d1f9      	bne.n	3c520 <_svfiprintf_r+0x1f2>
   3c52c:	2320      	movs	r3, #32
   3c52e:	f88d 302b 	strb.w	r3, [sp, #43]	; 0x2b
   3c532:	e7f5      	b.n	3c520 <_svfiprintf_r+0x1f2>
   3c534:	f04a 0a01 	orr.w	sl, sl, #1
   3c538:	e7f2      	b.n	3c520 <_svfiprintf_r+0x1f2>
   3c53a:	f854 3b04 	ldr.w	r3, [r4], #4
   3c53e:	2b00      	cmp	r3, #0
   3c540:	9303      	str	r3, [sp, #12]
   3c542:	daed      	bge.n	3c520 <_svfiprintf_r+0x1f2>
   3c544:	425b      	negs	r3, r3
   3c546:	9303      	str	r3, [sp, #12]
   3c548:	f04a 0a04 	orr.w	sl, sl, #4
   3c54c:	e7e8      	b.n	3c520 <_svfiprintf_r+0x1f2>
   3c54e:	232b      	movs	r3, #43	; 0x2b
   3c550:	e7ed      	b.n	3c52e <_svfiprintf_r+0x200>
   3c552:	9a01      	ldr	r2, [sp, #4]
   3c554:	f812 1b01 	ldrb.w	r1, [r2], #1
   3c558:	292a      	cmp	r1, #42	; 0x2a
   3c55a:	d113      	bne.n	3c584 <_svfiprintf_r+0x256>
   3c55c:	f854 0b04 	ldr.w	r0, [r4], #4
   3c560:	ea40 73e0 	orr.w	r3, r0, r0, asr #31
   3c564:	e9cd 3200 	strd	r3, r2, [sp]
   3c568:	e7da      	b.n	3c520 <_svfiprintf_r+0x1f2>
   3c56a:	9900      	ldr	r1, [sp, #0]
   3c56c:	200a      	movs	r0, #10
   3c56e:	fb00 3301 	mla	r3, r0, r1, r3
   3c572:	f812 1b01 	ldrb.w	r1, [r2], #1
   3c576:	9300      	str	r3, [sp, #0]
   3c578:	f1a1 0330 	sub.w	r3, r1, #48	; 0x30
   3c57c:	2b09      	cmp	r3, #9
   3c57e:	d9f4      	bls.n	3c56a <_svfiprintf_r+0x23c>
   3c580:	9201      	str	r2, [sp, #4]
   3c582:	e72d      	b.n	3c3e0 <_svfiprintf_r+0xb2>
   3c584:	2300      	movs	r3, #0
   3c586:	9300      	str	r3, [sp, #0]
   3c588:	e7f6      	b.n	3c578 <_svfiprintf_r+0x24a>
   3c58a:	f04a 0a80 	orr.w	sl, sl, #128	; 0x80
   3c58e:	e7c7      	b.n	3c520 <_svfiprintf_r+0x1f2>
   3c590:	2300      	movs	r3, #0
   3c592:	9a01      	ldr	r2, [sp, #4]
   3c594:	9303      	str	r3, [sp, #12]
   3c596:	3930      	subs	r1, #48	; 0x30
   3c598:	9b03      	ldr	r3, [sp, #12]
   3c59a:	200a      	movs	r0, #10
   3c59c:	fb00 1303 	mla	r3, r0, r3, r1
   3c5a0:	f812 1b01 	ldrb.w	r1, [r2], #1
   3c5a4:	9303      	str	r3, [sp, #12]
   3c5a6:	f1a1 0330 	sub.w	r3, r1, #48	; 0x30
   3c5aa:	2b09      	cmp	r3, #9
   3c5ac:	d9f3      	bls.n	3c596 <_svfiprintf_r+0x268>
   3c5ae:	e7e7      	b.n	3c580 <_svfiprintf_r+0x252>
   3c5b0:	9b01      	ldr	r3, [sp, #4]
   3c5b2:	781b      	ldrb	r3, [r3, #0]
   3c5b4:	2b68      	cmp	r3, #104	; 0x68
   3c5b6:	bf05      	ittet	eq
   3c5b8:	9b01      	ldreq	r3, [sp, #4]
   3c5ba:	f44a 7a00 	orreq.w	sl, sl, #512	; 0x200
   3c5be:	f04a 0a40 	orrne.w	sl, sl, #64	; 0x40
   3c5c2:	3301      	addeq	r3, #1
   3c5c4:	bf08      	it	eq
   3c5c6:	9301      	streq	r3, [sp, #4]
   3c5c8:	e7aa      	b.n	3c520 <_svfiprintf_r+0x1f2>
   3c5ca:	9b01      	ldr	r3, [sp, #4]
   3c5cc:	781b      	ldrb	r3, [r3, #0]
   3c5ce:	2b6c      	cmp	r3, #108	; 0x6c
   3c5d0:	d105      	bne.n	3c5de <_svfiprintf_r+0x2b0>
   3c5d2:	9b01      	ldr	r3, [sp, #4]
   3c5d4:	3301      	adds	r3, #1
   3c5d6:	9301      	str	r3, [sp, #4]
   3c5d8:	f04a 0a20 	orr.w	sl, sl, #32
   3c5dc:	e7a0      	b.n	3c520 <_svfiprintf_r+0x1f2>
   3c5de:	f04a 0a10 	orr.w	sl, sl, #16
   3c5e2:	e79d      	b.n	3c520 <_svfiprintf_r+0x1f2>
   3c5e4:	4627      	mov	r7, r4
   3c5e6:	2100      	movs	r1, #0
   3c5e8:	f857 3b04 	ldr.w	r3, [r7], #4
   3c5ec:	f88d 102b 	strb.w	r1, [sp, #43]	; 0x2b
   3c5f0:	f88d 307c 	strb.w	r3, [sp, #124]	; 0x7c
   3c5f4:	2301      	movs	r3, #1
   3c5f6:	460d      	mov	r5, r1
   3c5f8:	f10d 087c 	add.w	r8, sp, #124	; 0x7c
   3c5fc:	9300      	str	r3, [sp, #0]
   3c5fe:	e0ac      	b.n	3c75a <_svfiprintf_r+0x42c>
   3c600:	f04a 0a10 	orr.w	sl, sl, #16
   3c604:	f01a 0f20 	tst.w	sl, #32
   3c608:	d011      	beq.n	3c62e <_svfiprintf_r+0x300>
   3c60a:	1de3      	adds	r3, r4, #7
   3c60c:	f023 0307 	bic.w	r3, r3, #7
   3c610:	461f      	mov	r7, r3
   3c612:	685d      	ldr	r5, [r3, #4]
   3c614:	f857 4b08 	ldr.w	r4, [r7], #8
   3c618:	2d00      	cmp	r5, #0
   3c61a:	da06      	bge.n	3c62a <_svfiprintf_r+0x2fc>
   3c61c:	4264      	negs	r4, r4
   3c61e:	f04f 032d 	mov.w	r3, #45	; 0x2d
   3c622:	eb65 0545 	sbc.w	r5, r5, r5, lsl #1
   3c626:	f88d 302b 	strb.w	r3, [sp, #43]	; 0x2b
   3c62a:	2301      	movs	r3, #1
   3c62c:	e04a      	b.n	3c6c4 <_svfiprintf_r+0x396>
   3c62e:	4627      	mov	r7, r4
   3c630:	f01a 0f10 	tst.w	sl, #16
   3c634:	f857 5b04 	ldr.w	r5, [r7], #4
   3c638:	d002      	beq.n	3c640 <_svfiprintf_r+0x312>
   3c63a:	462c      	mov	r4, r5
   3c63c:	17ed      	asrs	r5, r5, #31
   3c63e:	e7eb      	b.n	3c618 <_svfiprintf_r+0x2ea>
   3c640:	f01a 0f40 	tst.w	sl, #64	; 0x40
   3c644:	d003      	beq.n	3c64e <_svfiprintf_r+0x320>
   3c646:	b22c      	sxth	r4, r5
   3c648:	f345 35c0 	sbfx	r5, r5, #15, #1
   3c64c:	e7e4      	b.n	3c618 <_svfiprintf_r+0x2ea>
   3c64e:	f41a 7f00 	tst.w	sl, #512	; 0x200
   3c652:	d0f2      	beq.n	3c63a <_svfiprintf_r+0x30c>
   3c654:	b26c      	sxtb	r4, r5
   3c656:	f345 15c0 	sbfx	r5, r5, #7, #1
   3c65a:	e7dd      	b.n	3c618 <_svfiprintf_r+0x2ea>
   3c65c:	f01a 0f20 	tst.w	sl, #32
   3c660:	f104 0704 	add.w	r7, r4, #4
   3c664:	d007      	beq.n	3c676 <_svfiprintf_r+0x348>
   3c666:	9a02      	ldr	r2, [sp, #8]
   3c668:	6823      	ldr	r3, [r4, #0]
   3c66a:	17d2      	asrs	r2, r2, #31
   3c66c:	9902      	ldr	r1, [sp, #8]
   3c66e:	e9c3 1200 	strd	r1, r2, [r3]
   3c672:	463c      	mov	r4, r7
   3c674:	e68b      	b.n	3c38e <_svfiprintf_r+0x60>
   3c676:	f01a 0f10 	tst.w	sl, #16
   3c67a:	d003      	beq.n	3c684 <_svfiprintf_r+0x356>
   3c67c:	6823      	ldr	r3, [r4, #0]
   3c67e:	9a02      	ldr	r2, [sp, #8]
   3c680:	601a      	str	r2, [r3, #0]
   3c682:	e7f6      	b.n	3c672 <_svfiprintf_r+0x344>
   3c684:	f01a 0f40 	tst.w	sl, #64	; 0x40
   3c688:	d003      	beq.n	3c692 <_svfiprintf_r+0x364>
   3c68a:	6823      	ldr	r3, [r4, #0]
   3c68c:	9a02      	ldr	r2, [sp, #8]
   3c68e:	801a      	strh	r2, [r3, #0]
   3c690:	e7ef      	b.n	3c672 <_svfiprintf_r+0x344>
   3c692:	f41a 7f00 	tst.w	sl, #512	; 0x200
   3c696:	d0f1      	beq.n	3c67c <_svfiprintf_r+0x34e>
   3c698:	6823      	ldr	r3, [r4, #0]
   3c69a:	9a02      	ldr	r2, [sp, #8]
   3c69c:	701a      	strb	r2, [r3, #0]
   3c69e:	e7e8      	b.n	3c672 <_svfiprintf_r+0x344>
   3c6a0:	f04a 0a10 	orr.w	sl, sl, #16
   3c6a4:	f01a 0320 	ands.w	r3, sl, #32
   3c6a8:	d01f      	beq.n	3c6ea <_svfiprintf_r+0x3bc>
   3c6aa:	1de3      	adds	r3, r4, #7
   3c6ac:	f023 0307 	bic.w	r3, r3, #7
   3c6b0:	461f      	mov	r7, r3
   3c6b2:	685d      	ldr	r5, [r3, #4]
   3c6b4:	f857 4b08 	ldr.w	r4, [r7], #8
   3c6b8:	f42a 6a80 	bic.w	sl, sl, #1024	; 0x400
   3c6bc:	2300      	movs	r3, #0
   3c6be:	2200      	movs	r2, #0
   3c6c0:	f88d 202b 	strb.w	r2, [sp, #43]	; 0x2b
   3c6c4:	9a00      	ldr	r2, [sp, #0]
   3c6c6:	3201      	adds	r2, #1
   3c6c8:	f000 8259 	beq.w	3cb7e <_svfiprintf_r+0x850>
   3c6cc:	f02a 0280 	bic.w	r2, sl, #128	; 0x80
   3c6d0:	9204      	str	r2, [sp, #16]
   3c6d2:	ea54 0205 	orrs.w	r2, r4, r5
   3c6d6:	f040 8258 	bne.w	3cb8a <_svfiprintf_r+0x85c>
   3c6da:	9a00      	ldr	r2, [sp, #0]
   3c6dc:	2a00      	cmp	r2, #0
   3c6de:	f000 819a 	beq.w	3ca16 <_svfiprintf_r+0x6e8>
   3c6e2:	2b01      	cmp	r3, #1
   3c6e4:	f040 8254 	bne.w	3cb90 <_svfiprintf_r+0x862>
   3c6e8:	e13a      	b.n	3c960 <_svfiprintf_r+0x632>
   3c6ea:	4627      	mov	r7, r4
   3c6ec:	f01a 0510 	ands.w	r5, sl, #16
   3c6f0:	f857 4b04 	ldr.w	r4, [r7], #4
   3c6f4:	d001      	beq.n	3c6fa <_svfiprintf_r+0x3cc>
   3c6f6:	461d      	mov	r5, r3
   3c6f8:	e7de      	b.n	3c6b8 <_svfiprintf_r+0x38a>
   3c6fa:	f01a 0340 	ands.w	r3, sl, #64	; 0x40
   3c6fe:	d001      	beq.n	3c704 <_svfiprintf_r+0x3d6>
   3c700:	b2a4      	uxth	r4, r4
   3c702:	e7d9      	b.n	3c6b8 <_svfiprintf_r+0x38a>
   3c704:	f41a 7500 	ands.w	r5, sl, #512	; 0x200
   3c708:	d0d6      	beq.n	3c6b8 <_svfiprintf_r+0x38a>
   3c70a:	b2e4      	uxtb	r4, r4
   3c70c:	e7f3      	b.n	3c6f6 <_svfiprintf_r+0x3c8>
   3c70e:	f647 0330 	movw	r3, #30768	; 0x7830
   3c712:	4627      	mov	r7, r4
   3c714:	2500      	movs	r5, #0
   3c716:	f04a 0a02 	orr.w	sl, sl, #2
   3c71a:	f8ad 302c 	strh.w	r3, [sp, #44]	; 0x2c
   3c71e:	f857 4b04 	ldr.w	r4, [r7], #4
   3c722:	4b02      	ldr	r3, [pc, #8]	; (3c72c <_svfiprintf_r+0x3fe>)
   3c724:	9305      	str	r3, [sp, #20]
   3c726:	2302      	movs	r3, #2
   3c728:	e7c9      	b.n	3c6be <_svfiprintf_r+0x390>
   3c72a:	bf00      	nop
   3c72c:	000404df 	.word	0x000404df
   3c730:	9b00      	ldr	r3, [sp, #0]
   3c732:	4627      	mov	r7, r4
   3c734:	2500      	movs	r5, #0
   3c736:	1c58      	adds	r0, r3, #1
   3c738:	f857 8b04 	ldr.w	r8, [r7], #4
   3c73c:	f88d 502b 	strb.w	r5, [sp, #43]	; 0x2b
   3c740:	f000 80d0 	beq.w	3c8e4 <_svfiprintf_r+0x5b6>
   3c744:	461a      	mov	r2, r3
   3c746:	4629      	mov	r1, r5
   3c748:	4640      	mov	r0, r8
   3c74a:	f7fd fb2d 	bl	39da8 <memchr>
   3c74e:	2800      	cmp	r0, #0
   3c750:	f000 8176 	beq.w	3ca40 <_svfiprintf_r+0x712>
   3c754:	eba0 0308 	sub.w	r3, r0, r8
   3c758:	9300      	str	r3, [sp, #0]
   3c75a:	9b00      	ldr	r3, [sp, #0]
   3c75c:	42ab      	cmp	r3, r5
   3c75e:	bfb8      	it	lt
   3c760:	462b      	movlt	r3, r5
   3c762:	9304      	str	r3, [sp, #16]
   3c764:	f89d 302b 	ldrb.w	r3, [sp, #43]	; 0x2b
   3c768:	b113      	cbz	r3, 3c770 <_svfiprintf_r+0x442>
   3c76a:	9b04      	ldr	r3, [sp, #16]
   3c76c:	3301      	adds	r3, #1
   3c76e:	9304      	str	r3, [sp, #16]
   3c770:	f01a 0302 	ands.w	r3, sl, #2
   3c774:	9308      	str	r3, [sp, #32]
   3c776:	bf1e      	ittt	ne
   3c778:	9b04      	ldrne	r3, [sp, #16]
   3c77a:	3302      	addne	r3, #2
   3c77c:	9304      	strne	r3, [sp, #16]
   3c77e:	f01a 0384 	ands.w	r3, sl, #132	; 0x84
   3c782:	9309      	str	r3, [sp, #36]	; 0x24
   3c784:	d11f      	bne.n	3c7c6 <_svfiprintf_r+0x498>
   3c786:	e9dd 3203 	ldrd	r3, r2, [sp, #12]
   3c78a:	1a9c      	subs	r4, r3, r2
   3c78c:	2c00      	cmp	r4, #0
   3c78e:	dd1a      	ble.n	3c7c6 <_svfiprintf_r+0x498>
   3c790:	48ac      	ldr	r0, [pc, #688]	; (3ca44 <_svfiprintf_r+0x716>)
   3c792:	2c10      	cmp	r4, #16
   3c794:	f106 0108 	add.w	r1, r6, #8
   3c798:	6030      	str	r0, [r6, #0]
   3c79a:	e9dd 320d 	ldrd	r3, r2, [sp, #52]	; 0x34
   3c79e:	f103 0301 	add.w	r3, r3, #1
   3c7a2:	f300 8155 	bgt.w	3ca50 <_svfiprintf_r+0x722>
   3c7a6:	6074      	str	r4, [r6, #4]
   3c7a8:	2b07      	cmp	r3, #7
   3c7aa:	4414      	add	r4, r2
   3c7ac:	e9cd 340d 	strd	r3, r4, [sp, #52]	; 0x34
   3c7b0:	f340 8160 	ble.w	3ca74 <_svfiprintf_r+0x746>
   3c7b4:	aa0c      	add	r2, sp, #48	; 0x30
   3c7b6:	4659      	mov	r1, fp
   3c7b8:	4648      	mov	r0, r9
   3c7ba:	f7ff fd3d 	bl	3c238 <__ssprint_r>
   3c7be:	2800      	cmp	r0, #0
   3c7c0:	f040 81b9 	bne.w	3cb36 <_svfiprintf_r+0x808>
   3c7c4:	ae0f      	add	r6, sp, #60	; 0x3c
   3c7c6:	f89d 302b 	ldrb.w	r3, [sp, #43]	; 0x2b
   3c7ca:	b17b      	cbz	r3, 3c7ec <_svfiprintf_r+0x4be>
   3c7cc:	f10d 022b 	add.w	r2, sp, #43	; 0x2b
   3c7d0:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3c7d2:	6032      	str	r2, [r6, #0]
   3c7d4:	2201      	movs	r2, #1
   3c7d6:	3301      	adds	r3, #1
   3c7d8:	6072      	str	r2, [r6, #4]
   3c7da:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   3c7dc:	2b07      	cmp	r3, #7
   3c7de:	f102 0201 	add.w	r2, r2, #1
   3c7e2:	e9cd 320d 	strd	r3, r2, [sp, #52]	; 0x34
   3c7e6:	f300 8147 	bgt.w	3ca78 <_svfiprintf_r+0x74a>
   3c7ea:	3608      	adds	r6, #8
   3c7ec:	9b08      	ldr	r3, [sp, #32]
   3c7ee:	b173      	cbz	r3, 3c80e <_svfiprintf_r+0x4e0>
   3c7f0:	aa0b      	add	r2, sp, #44	; 0x2c
   3c7f2:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3c7f4:	6032      	str	r2, [r6, #0]
   3c7f6:	2202      	movs	r2, #2
   3c7f8:	3301      	adds	r3, #1
   3c7fa:	6072      	str	r2, [r6, #4]
   3c7fc:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   3c7fe:	2b07      	cmp	r3, #7
   3c800:	f102 0202 	add.w	r2, r2, #2
   3c804:	e9cd 320d 	strd	r3, r2, [sp, #52]	; 0x34
   3c808:	f300 813f 	bgt.w	3ca8a <_svfiprintf_r+0x75c>
   3c80c:	3608      	adds	r6, #8
   3c80e:	9b09      	ldr	r3, [sp, #36]	; 0x24
   3c810:	2b80      	cmp	r3, #128	; 0x80
   3c812:	d11f      	bne.n	3c854 <_svfiprintf_r+0x526>
   3c814:	e9dd 3203 	ldrd	r3, r2, [sp, #12]
   3c818:	1a9c      	subs	r4, r3, r2
   3c81a:	2c00      	cmp	r4, #0
   3c81c:	dd1a      	ble.n	3c854 <_svfiprintf_r+0x526>
   3c81e:	488a      	ldr	r0, [pc, #552]	; (3ca48 <_svfiprintf_r+0x71a>)
   3c820:	2c10      	cmp	r4, #16
   3c822:	f106 0108 	add.w	r1, r6, #8
   3c826:	6030      	str	r0, [r6, #0]
   3c828:	e9dd 320d 	ldrd	r3, r2, [sp, #52]	; 0x34
   3c82c:	f103 0301 	add.w	r3, r3, #1
   3c830:	f300 8134 	bgt.w	3ca9c <_svfiprintf_r+0x76e>
   3c834:	6074      	str	r4, [r6, #4]
   3c836:	2b07      	cmp	r3, #7
   3c838:	4414      	add	r4, r2
   3c83a:	e9cd 340d 	strd	r3, r4, [sp, #52]	; 0x34
   3c83e:	f340 813f 	ble.w	3cac0 <_svfiprintf_r+0x792>
   3c842:	aa0c      	add	r2, sp, #48	; 0x30
   3c844:	4659      	mov	r1, fp
   3c846:	4648      	mov	r0, r9
   3c848:	f7ff fcf6 	bl	3c238 <__ssprint_r>
   3c84c:	2800      	cmp	r0, #0
   3c84e:	f040 8172 	bne.w	3cb36 <_svfiprintf_r+0x808>
   3c852:	ae0f      	add	r6, sp, #60	; 0x3c
   3c854:	9b00      	ldr	r3, [sp, #0]
   3c856:	1aec      	subs	r4, r5, r3
   3c858:	2c00      	cmp	r4, #0
   3c85a:	dd1a      	ble.n	3c892 <_svfiprintf_r+0x564>
   3c85c:	4d7a      	ldr	r5, [pc, #488]	; (3ca48 <_svfiprintf_r+0x71a>)
   3c85e:	2c10      	cmp	r4, #16
   3c860:	f106 0208 	add.w	r2, r6, #8
   3c864:	6035      	str	r5, [r6, #0]
   3c866:	e9dd 310d 	ldrd	r3, r1, [sp, #52]	; 0x34
   3c86a:	f103 0301 	add.w	r3, r3, #1
   3c86e:	f300 8129 	bgt.w	3cac4 <_svfiprintf_r+0x796>
   3c872:	6074      	str	r4, [r6, #4]
   3c874:	2b07      	cmp	r3, #7
   3c876:	440c      	add	r4, r1
   3c878:	e9cd 340d 	strd	r3, r4, [sp, #52]	; 0x34
   3c87c:	f340 8133 	ble.w	3cae6 <_svfiprintf_r+0x7b8>
   3c880:	aa0c      	add	r2, sp, #48	; 0x30
   3c882:	4659      	mov	r1, fp
   3c884:	4648      	mov	r0, r9
   3c886:	f7ff fcd7 	bl	3c238 <__ssprint_r>
   3c88a:	2800      	cmp	r0, #0
   3c88c:	f040 8153 	bne.w	3cb36 <_svfiprintf_r+0x808>
   3c890:	ae0f      	add	r6, sp, #60	; 0x3c
   3c892:	9b00      	ldr	r3, [sp, #0]
   3c894:	980e      	ldr	r0, [sp, #56]	; 0x38
   3c896:	6073      	str	r3, [r6, #4]
   3c898:	4418      	add	r0, r3
   3c89a:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3c89c:	f8c6 8000 	str.w	r8, [r6]
   3c8a0:	3301      	adds	r3, #1
   3c8a2:	900e      	str	r0, [sp, #56]	; 0x38
   3c8a4:	2b07      	cmp	r3, #7
   3c8a6:	930d      	str	r3, [sp, #52]	; 0x34
   3c8a8:	f300 811f 	bgt.w	3caea <_svfiprintf_r+0x7bc>
   3c8ac:	f106 0308 	add.w	r3, r6, #8
   3c8b0:	f01a 0f04 	tst.w	sl, #4
   3c8b4:	f040 8121 	bne.w	3cafa <_svfiprintf_r+0x7cc>
   3c8b8:	9904      	ldr	r1, [sp, #16]
   3c8ba:	e9dd 3202 	ldrd	r3, r2, [sp, #8]
   3c8be:	428a      	cmp	r2, r1
   3c8c0:	bfac      	ite	ge
   3c8c2:	189b      	addge	r3, r3, r2
   3c8c4:	185b      	addlt	r3, r3, r1
   3c8c6:	9302      	str	r3, [sp, #8]
   3c8c8:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3c8ca:	b13b      	cbz	r3, 3c8dc <_svfiprintf_r+0x5ae>
   3c8cc:	aa0c      	add	r2, sp, #48	; 0x30
   3c8ce:	4659      	mov	r1, fp
   3c8d0:	4648      	mov	r0, r9
   3c8d2:	f7ff fcb1 	bl	3c238 <__ssprint_r>
   3c8d6:	2800      	cmp	r0, #0
   3c8d8:	f040 812d 	bne.w	3cb36 <_svfiprintf_r+0x808>
   3c8dc:	2300      	movs	r3, #0
   3c8de:	ae0f      	add	r6, sp, #60	; 0x3c
   3c8e0:	930d      	str	r3, [sp, #52]	; 0x34
   3c8e2:	e6c6      	b.n	3c672 <_svfiprintf_r+0x344>
   3c8e4:	4640      	mov	r0, r8
   3c8e6:	f7f7 fa23 	bl	33d30 <strlen>
   3c8ea:	9000      	str	r0, [sp, #0]
   3c8ec:	e735      	b.n	3c75a <_svfiprintf_r+0x42c>
   3c8ee:	f04a 0a10 	orr.w	sl, sl, #16
   3c8f2:	f01a 0320 	ands.w	r3, sl, #32
   3c8f6:	d008      	beq.n	3c90a <_svfiprintf_r+0x5dc>
   3c8f8:	1de3      	adds	r3, r4, #7
   3c8fa:	f023 0307 	bic.w	r3, r3, #7
   3c8fe:	461f      	mov	r7, r3
   3c900:	685d      	ldr	r5, [r3, #4]
   3c902:	f857 4b08 	ldr.w	r4, [r7], #8
   3c906:	2301      	movs	r3, #1
   3c908:	e6d9      	b.n	3c6be <_svfiprintf_r+0x390>
   3c90a:	4627      	mov	r7, r4
   3c90c:	f01a 0510 	ands.w	r5, sl, #16
   3c910:	f857 4b04 	ldr.w	r4, [r7], #4
   3c914:	d001      	beq.n	3c91a <_svfiprintf_r+0x5ec>
   3c916:	461d      	mov	r5, r3
   3c918:	e7f5      	b.n	3c906 <_svfiprintf_r+0x5d8>
   3c91a:	f01a 0340 	ands.w	r3, sl, #64	; 0x40
   3c91e:	d001      	beq.n	3c924 <_svfiprintf_r+0x5f6>
   3c920:	b2a4      	uxth	r4, r4
   3c922:	e7f0      	b.n	3c906 <_svfiprintf_r+0x5d8>
   3c924:	f41a 7500 	ands.w	r5, sl, #512	; 0x200
   3c928:	d0ed      	beq.n	3c906 <_svfiprintf_r+0x5d8>
   3c92a:	b2e4      	uxtb	r4, r4
   3c92c:	e7f3      	b.n	3c916 <_svfiprintf_r+0x5e8>
   3c92e:	4b47      	ldr	r3, [pc, #284]	; (3ca4c <_svfiprintf_r+0x71e>)
   3c930:	e5c3      	b.n	3c4ba <_svfiprintf_r+0x18c>
   3c932:	4627      	mov	r7, r4
   3c934:	f01a 0510 	ands.w	r5, sl, #16
   3c938:	f857 4b04 	ldr.w	r4, [r7], #4
   3c93c:	d001      	beq.n	3c942 <_svfiprintf_r+0x614>
   3c93e:	461d      	mov	r5, r3
   3c940:	e5c7      	b.n	3c4d2 <_svfiprintf_r+0x1a4>
   3c942:	f01a 0340 	ands.w	r3, sl, #64	; 0x40
   3c946:	d001      	beq.n	3c94c <_svfiprintf_r+0x61e>
   3c948:	b2a4      	uxth	r4, r4
   3c94a:	e5c2      	b.n	3c4d2 <_svfiprintf_r+0x1a4>
   3c94c:	f41a 7500 	ands.w	r5, sl, #512	; 0x200
   3c950:	f43f adbf 	beq.w	3c4d2 <_svfiprintf_r+0x1a4>
   3c954:	b2e4      	uxtb	r4, r4
   3c956:	e7f2      	b.n	3c93e <_svfiprintf_r+0x610>
   3c958:	2c0a      	cmp	r4, #10
   3c95a:	f175 0300 	sbcs.w	r3, r5, #0
   3c95e:	d205      	bcs.n	3c96c <_svfiprintf_r+0x63e>
   3c960:	3430      	adds	r4, #48	; 0x30
   3c962:	f88d 40df 	strb.w	r4, [sp, #223]	; 0xdf
   3c966:	f10d 08df 	add.w	r8, sp, #223	; 0xdf
   3c96a:	e12d      	b.n	3cbc8 <_svfiprintf_r+0x89a>
   3c96c:	ab38      	add	r3, sp, #224	; 0xe0
   3c96e:	f04f 0a00 	mov.w	sl, #0
   3c972:	9308      	str	r3, [sp, #32]
   3c974:	9b04      	ldr	r3, [sp, #16]
   3c976:	f403 6380 	and.w	r3, r3, #1024	; 0x400
   3c97a:	9309      	str	r3, [sp, #36]	; 0x24
   3c97c:	9b08      	ldr	r3, [sp, #32]
   3c97e:	220a      	movs	r2, #10
   3c980:	4620      	mov	r0, r4
   3c982:	4629      	mov	r1, r5
   3c984:	f103 38ff 	add.w	r8, r3, #4294967295	; 0xffffffff
   3c988:	2300      	movs	r3, #0
   3c98a:	f003 fb1f 	bl	3ffcc <__aeabi_uldivmod>
   3c98e:	9b08      	ldr	r3, [sp, #32]
   3c990:	3230      	adds	r2, #48	; 0x30
   3c992:	f10a 0a01 	add.w	sl, sl, #1
   3c996:	f803 2c01 	strb.w	r2, [r3, #-1]
   3c99a:	9b09      	ldr	r3, [sp, #36]	; 0x24
   3c99c:	b1db      	cbz	r3, 3c9d6 <_svfiprintf_r+0x6a8>
   3c99e:	9b06      	ldr	r3, [sp, #24]
   3c9a0:	781b      	ldrb	r3, [r3, #0]
   3c9a2:	4553      	cmp	r3, sl
   3c9a4:	d117      	bne.n	3c9d6 <_svfiprintf_r+0x6a8>
   3c9a6:	f1ba 0fff 	cmp.w	sl, #255	; 0xff
   3c9aa:	d014      	beq.n	3c9d6 <_svfiprintf_r+0x6a8>
   3c9ac:	2c0a      	cmp	r4, #10
   3c9ae:	f175 0300 	sbcs.w	r3, r5, #0
   3c9b2:	d310      	bcc.n	3c9d6 <_svfiprintf_r+0x6a8>
   3c9b4:	9b07      	ldr	r3, [sp, #28]
   3c9b6:	ee18 1a10 	vmov	r1, s16
   3c9ba:	eba8 0803 	sub.w	r8, r8, r3
   3c9be:	461a      	mov	r2, r3
   3c9c0:	4640      	mov	r0, r8
   3c9c2:	f7f7 fa32 	bl	33e2a <strncpy>
   3c9c6:	9b06      	ldr	r3, [sp, #24]
   3c9c8:	785b      	ldrb	r3, [r3, #1]
   3c9ca:	b1a3      	cbz	r3, 3c9f6 <_svfiprintf_r+0x6c8>
   3c9cc:	9b06      	ldr	r3, [sp, #24]
   3c9ce:	f04f 0a00 	mov.w	sl, #0
   3c9d2:	3301      	adds	r3, #1
   3c9d4:	9306      	str	r3, [sp, #24]
   3c9d6:	2300      	movs	r3, #0
   3c9d8:	220a      	movs	r2, #10
   3c9da:	4620      	mov	r0, r4
   3c9dc:	4629      	mov	r1, r5
   3c9de:	f003 faf5 	bl	3ffcc <__aeabi_uldivmod>
   3c9e2:	2c0a      	cmp	r4, #10
   3c9e4:	f175 0300 	sbcs.w	r3, r5, #0
   3c9e8:	f0c0 80ee 	bcc.w	3cbc8 <_svfiprintf_r+0x89a>
   3c9ec:	4604      	mov	r4, r0
   3c9ee:	460d      	mov	r5, r1
   3c9f0:	f8cd 8020 	str.w	r8, [sp, #32]
   3c9f4:	e7c2      	b.n	3c97c <_svfiprintf_r+0x64e>
   3c9f6:	469a      	mov	sl, r3
   3c9f8:	e7ed      	b.n	3c9d6 <_svfiprintf_r+0x6a8>
   3c9fa:	f004 030f 	and.w	r3, r4, #15
   3c9fe:	9a05      	ldr	r2, [sp, #20]
   3ca00:	0924      	lsrs	r4, r4, #4
   3ca02:	5cd3      	ldrb	r3, [r2, r3]
   3ca04:	ea44 7405 	orr.w	r4, r4, r5, lsl #28
   3ca08:	092d      	lsrs	r5, r5, #4
   3ca0a:	f808 3d01 	strb.w	r3, [r8, #-1]!
   3ca0e:	ea54 0305 	orrs.w	r3, r4, r5
   3ca12:	d1f2      	bne.n	3c9fa <_svfiprintf_r+0x6cc>
   3ca14:	e0d8      	b.n	3cbc8 <_svfiprintf_r+0x89a>
   3ca16:	b933      	cbnz	r3, 3ca26 <_svfiprintf_r+0x6f8>
   3ca18:	f01a 0f01 	tst.w	sl, #1
   3ca1c:	d003      	beq.n	3ca26 <_svfiprintf_r+0x6f8>
   3ca1e:	2330      	movs	r3, #48	; 0x30
   3ca20:	f88d 30df 	strb.w	r3, [sp, #223]	; 0xdf
   3ca24:	e79f      	b.n	3c966 <_svfiprintf_r+0x638>
   3ca26:	f10d 08e0 	add.w	r8, sp, #224	; 0xe0
   3ca2a:	e0cd      	b.n	3cbc8 <_svfiprintf_r+0x89a>
   3ca2c:	2900      	cmp	r1, #0
   3ca2e:	f000 809d 	beq.w	3cb6c <_svfiprintf_r+0x83e>
   3ca32:	f88d 107c 	strb.w	r1, [sp, #124]	; 0x7c
   3ca36:	2100      	movs	r1, #0
   3ca38:	4627      	mov	r7, r4
   3ca3a:	f88d 102b 	strb.w	r1, [sp, #43]	; 0x2b
   3ca3e:	e5d9      	b.n	3c5f4 <_svfiprintf_r+0x2c6>
   3ca40:	4605      	mov	r5, r0
   3ca42:	e68a      	b.n	3c75a <_svfiprintf_r+0x42c>
   3ca44:	00040900 	.word	0x00040900
   3ca48:	00040910 	.word	0x00040910
   3ca4c:	000404f0 	.word	0x000404f0
   3ca50:	2010      	movs	r0, #16
   3ca52:	2b07      	cmp	r3, #7
   3ca54:	4402      	add	r2, r0
   3ca56:	6070      	str	r0, [r6, #4]
   3ca58:	e9cd 320d 	strd	r3, r2, [sp, #52]	; 0x34
   3ca5c:	dd07      	ble.n	3ca6e <_svfiprintf_r+0x740>
   3ca5e:	aa0c      	add	r2, sp, #48	; 0x30
   3ca60:	4659      	mov	r1, fp
   3ca62:	4648      	mov	r0, r9
   3ca64:	f7ff fbe8 	bl	3c238 <__ssprint_r>
   3ca68:	2800      	cmp	r0, #0
   3ca6a:	d164      	bne.n	3cb36 <_svfiprintf_r+0x808>
   3ca6c:	a90f      	add	r1, sp, #60	; 0x3c
   3ca6e:	3c10      	subs	r4, #16
   3ca70:	460e      	mov	r6, r1
   3ca72:	e68d      	b.n	3c790 <_svfiprintf_r+0x462>
   3ca74:	460e      	mov	r6, r1
   3ca76:	e6a6      	b.n	3c7c6 <_svfiprintf_r+0x498>
   3ca78:	aa0c      	add	r2, sp, #48	; 0x30
   3ca7a:	4659      	mov	r1, fp
   3ca7c:	4648      	mov	r0, r9
   3ca7e:	f7ff fbdb 	bl	3c238 <__ssprint_r>
   3ca82:	2800      	cmp	r0, #0
   3ca84:	d157      	bne.n	3cb36 <_svfiprintf_r+0x808>
   3ca86:	ae0f      	add	r6, sp, #60	; 0x3c
   3ca88:	e6b0      	b.n	3c7ec <_svfiprintf_r+0x4be>
   3ca8a:	aa0c      	add	r2, sp, #48	; 0x30
   3ca8c:	4659      	mov	r1, fp
   3ca8e:	4648      	mov	r0, r9
   3ca90:	f7ff fbd2 	bl	3c238 <__ssprint_r>
   3ca94:	2800      	cmp	r0, #0
   3ca96:	d14e      	bne.n	3cb36 <_svfiprintf_r+0x808>
   3ca98:	ae0f      	add	r6, sp, #60	; 0x3c
   3ca9a:	e6b8      	b.n	3c80e <_svfiprintf_r+0x4e0>
   3ca9c:	2010      	movs	r0, #16
   3ca9e:	2b07      	cmp	r3, #7
   3caa0:	4402      	add	r2, r0
   3caa2:	6070      	str	r0, [r6, #4]
   3caa4:	e9cd 320d 	strd	r3, r2, [sp, #52]	; 0x34
   3caa8:	dd07      	ble.n	3caba <_svfiprintf_r+0x78c>
   3caaa:	aa0c      	add	r2, sp, #48	; 0x30
   3caac:	4659      	mov	r1, fp
   3caae:	4648      	mov	r0, r9
   3cab0:	f7ff fbc2 	bl	3c238 <__ssprint_r>
   3cab4:	2800      	cmp	r0, #0
   3cab6:	d13e      	bne.n	3cb36 <_svfiprintf_r+0x808>
   3cab8:	a90f      	add	r1, sp, #60	; 0x3c
   3caba:	3c10      	subs	r4, #16
   3cabc:	460e      	mov	r6, r1
   3cabe:	e6ae      	b.n	3c81e <_svfiprintf_r+0x4f0>
   3cac0:	460e      	mov	r6, r1
   3cac2:	e6c7      	b.n	3c854 <_svfiprintf_r+0x526>
   3cac4:	2010      	movs	r0, #16
   3cac6:	2b07      	cmp	r3, #7
   3cac8:	4401      	add	r1, r0
   3caca:	6070      	str	r0, [r6, #4]
   3cacc:	e9cd 310d 	strd	r3, r1, [sp, #52]	; 0x34
   3cad0:	dd06      	ble.n	3cae0 <_svfiprintf_r+0x7b2>
   3cad2:	aa0c      	add	r2, sp, #48	; 0x30
   3cad4:	4659      	mov	r1, fp
   3cad6:	4648      	mov	r0, r9
   3cad8:	f7ff fbae 	bl	3c238 <__ssprint_r>
   3cadc:	bb58      	cbnz	r0, 3cb36 <_svfiprintf_r+0x808>
   3cade:	aa0f      	add	r2, sp, #60	; 0x3c
   3cae0:	3c10      	subs	r4, #16
   3cae2:	4616      	mov	r6, r2
   3cae4:	e6bb      	b.n	3c85e <_svfiprintf_r+0x530>
   3cae6:	4616      	mov	r6, r2
   3cae8:	e6d3      	b.n	3c892 <_svfiprintf_r+0x564>
   3caea:	aa0c      	add	r2, sp, #48	; 0x30
   3caec:	4659      	mov	r1, fp
   3caee:	4648      	mov	r0, r9
   3caf0:	f7ff fba2 	bl	3c238 <__ssprint_r>
   3caf4:	b9f8      	cbnz	r0, 3cb36 <_svfiprintf_r+0x808>
   3caf6:	ab0f      	add	r3, sp, #60	; 0x3c
   3caf8:	e6da      	b.n	3c8b0 <_svfiprintf_r+0x582>
   3cafa:	e9dd 2103 	ldrd	r2, r1, [sp, #12]
   3cafe:	1a54      	subs	r4, r2, r1
   3cb00:	2c00      	cmp	r4, #0
   3cb02:	f77f aed9 	ble.w	3c8b8 <_svfiprintf_r+0x58a>
   3cb06:	4d34      	ldr	r5, [pc, #208]	; (3cbd8 <_svfiprintf_r+0x8aa>)
   3cb08:	2610      	movs	r6, #16
   3cb0a:	2c10      	cmp	r4, #16
   3cb0c:	601d      	str	r5, [r3, #0]
   3cb0e:	e9dd 210d 	ldrd	r2, r1, [sp, #52]	; 0x34
   3cb12:	f102 0201 	add.w	r2, r2, #1
   3cb16:	dc17      	bgt.n	3cb48 <_svfiprintf_r+0x81a>
   3cb18:	605c      	str	r4, [r3, #4]
   3cb1a:	2a07      	cmp	r2, #7
   3cb1c:	440c      	add	r4, r1
   3cb1e:	e9cd 240d 	strd	r2, r4, [sp, #52]	; 0x34
   3cb22:	f77f aec9 	ble.w	3c8b8 <_svfiprintf_r+0x58a>
   3cb26:	aa0c      	add	r2, sp, #48	; 0x30
   3cb28:	4659      	mov	r1, fp
   3cb2a:	4648      	mov	r0, r9
   3cb2c:	f7ff fb84 	bl	3c238 <__ssprint_r>
   3cb30:	2800      	cmp	r0, #0
   3cb32:	f43f aec1 	beq.w	3c8b8 <_svfiprintf_r+0x58a>
   3cb36:	f8bb 300c 	ldrh.w	r3, [fp, #12]
   3cb3a:	f013 0f40 	tst.w	r3, #64	; 0x40
   3cb3e:	9b02      	ldr	r3, [sp, #8]
   3cb40:	bf18      	it	ne
   3cb42:	f04f 33ff 	movne.w	r3, #4294967295	; 0xffffffff
   3cb46:	e40d      	b.n	3c364 <_svfiprintf_r+0x36>
   3cb48:	3110      	adds	r1, #16
   3cb4a:	2a07      	cmp	r2, #7
   3cb4c:	605e      	str	r6, [r3, #4]
   3cb4e:	e9cd 210d 	strd	r2, r1, [sp, #52]	; 0x34
   3cb52:	dc02      	bgt.n	3cb5a <_svfiprintf_r+0x82c>
   3cb54:	3308      	adds	r3, #8
   3cb56:	3c10      	subs	r4, #16
   3cb58:	e7d7      	b.n	3cb0a <_svfiprintf_r+0x7dc>
   3cb5a:	aa0c      	add	r2, sp, #48	; 0x30
   3cb5c:	4659      	mov	r1, fp
   3cb5e:	4648      	mov	r0, r9
   3cb60:	f7ff fb6a 	bl	3c238 <__ssprint_r>
   3cb64:	2800      	cmp	r0, #0
   3cb66:	d1e6      	bne.n	3cb36 <_svfiprintf_r+0x808>
   3cb68:	ab0f      	add	r3, sp, #60	; 0x3c
   3cb6a:	e7f4      	b.n	3cb56 <_svfiprintf_r+0x828>
   3cb6c:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3cb6e:	2b00      	cmp	r3, #0
   3cb70:	d0e1      	beq.n	3cb36 <_svfiprintf_r+0x808>
   3cb72:	aa0c      	add	r2, sp, #48	; 0x30
   3cb74:	4659      	mov	r1, fp
   3cb76:	4648      	mov	r0, r9
   3cb78:	f7ff fb5e 	bl	3c238 <__ssprint_r>
   3cb7c:	e7db      	b.n	3cb36 <_svfiprintf_r+0x808>
   3cb7e:	ea54 0205 	orrs.w	r2, r4, r5
   3cb82:	f8cd a010 	str.w	sl, [sp, #16]
   3cb86:	f43f adac 	beq.w	3c6e2 <_svfiprintf_r+0x3b4>
   3cb8a:	2b01      	cmp	r3, #1
   3cb8c:	f43f aee4 	beq.w	3c958 <_svfiprintf_r+0x62a>
   3cb90:	2b02      	cmp	r3, #2
   3cb92:	f10d 08e0 	add.w	r8, sp, #224	; 0xe0
   3cb96:	f43f af30 	beq.w	3c9fa <_svfiprintf_r+0x6cc>
   3cb9a:	f004 0307 	and.w	r3, r4, #7
   3cb9e:	08e4      	lsrs	r4, r4, #3
   3cba0:	4642      	mov	r2, r8
   3cba2:	ea44 7445 	orr.w	r4, r4, r5, lsl #29
   3cba6:	3330      	adds	r3, #48	; 0x30
   3cba8:	08ed      	lsrs	r5, r5, #3
   3cbaa:	f808 3d01 	strb.w	r3, [r8, #-1]!
   3cbae:	ea54 0105 	orrs.w	r1, r4, r5
   3cbb2:	d1f2      	bne.n	3cb9a <_svfiprintf_r+0x86c>
   3cbb4:	9904      	ldr	r1, [sp, #16]
   3cbb6:	07c9      	lsls	r1, r1, #31
   3cbb8:	d506      	bpl.n	3cbc8 <_svfiprintf_r+0x89a>
   3cbba:	2b30      	cmp	r3, #48	; 0x30
   3cbbc:	d004      	beq.n	3cbc8 <_svfiprintf_r+0x89a>
   3cbbe:	2330      	movs	r3, #48	; 0x30
   3cbc0:	f808 3c01 	strb.w	r3, [r8, #-1]
   3cbc4:	f1a2 0802 	sub.w	r8, r2, #2
   3cbc8:	ab38      	add	r3, sp, #224	; 0xe0
   3cbca:	9d00      	ldr	r5, [sp, #0]
   3cbcc:	f8dd a010 	ldr.w	sl, [sp, #16]
   3cbd0:	eba3 0308 	sub.w	r3, r3, r8
   3cbd4:	9300      	str	r3, [sp, #0]
   3cbd6:	e5c0      	b.n	3c75a <_svfiprintf_r+0x42c>
   3cbd8:	00040900 	.word	0x00040900

0003cbdc <_sungetc_r>:
   3cbdc:	b538      	push	{r3, r4, r5, lr}
   3cbde:	1c4b      	adds	r3, r1, #1
   3cbe0:	4614      	mov	r4, r2
   3cbe2:	d103      	bne.n	3cbec <_sungetc_r+0x10>
   3cbe4:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
   3cbe8:	4628      	mov	r0, r5
   3cbea:	bd38      	pop	{r3, r4, r5, pc}
   3cbec:	8993      	ldrh	r3, [r2, #12]
   3cbee:	b2cd      	uxtb	r5, r1
   3cbf0:	f023 0320 	bic.w	r3, r3, #32
   3cbf4:	8193      	strh	r3, [r2, #12]
   3cbf6:	6b23      	ldr	r3, [r4, #48]	; 0x30
   3cbf8:	6852      	ldr	r2, [r2, #4]
   3cbfa:	b18b      	cbz	r3, 3cc20 <_sungetc_r+0x44>
   3cbfc:	6b63      	ldr	r3, [r4, #52]	; 0x34
   3cbfe:	4293      	cmp	r3, r2
   3cc00:	dd08      	ble.n	3cc14 <_sungetc_r+0x38>
   3cc02:	6823      	ldr	r3, [r4, #0]
   3cc04:	1e5a      	subs	r2, r3, #1
   3cc06:	6022      	str	r2, [r4, #0]
   3cc08:	f803 5c01 	strb.w	r5, [r3, #-1]
   3cc0c:	6863      	ldr	r3, [r4, #4]
   3cc0e:	3301      	adds	r3, #1
   3cc10:	6063      	str	r3, [r4, #4]
   3cc12:	e7e9      	b.n	3cbe8 <_sungetc_r+0xc>
   3cc14:	4621      	mov	r1, r4
   3cc16:	f000 ffdb 	bl	3dbd0 <__submore>
   3cc1a:	2800      	cmp	r0, #0
   3cc1c:	d0f1      	beq.n	3cc02 <_sungetc_r+0x26>
   3cc1e:	e7e1      	b.n	3cbe4 <_sungetc_r+0x8>
   3cc20:	6921      	ldr	r1, [r4, #16]
   3cc22:	6823      	ldr	r3, [r4, #0]
   3cc24:	b151      	cbz	r1, 3cc3c <_sungetc_r+0x60>
   3cc26:	4299      	cmp	r1, r3
   3cc28:	d208      	bcs.n	3cc3c <_sungetc_r+0x60>
   3cc2a:	f813 1c01 	ldrb.w	r1, [r3, #-1]
   3cc2e:	42a9      	cmp	r1, r5
   3cc30:	d104      	bne.n	3cc3c <_sungetc_r+0x60>
   3cc32:	3b01      	subs	r3, #1
   3cc34:	3201      	adds	r2, #1
   3cc36:	6023      	str	r3, [r4, #0]
   3cc38:	6062      	str	r2, [r4, #4]
   3cc3a:	e7d5      	b.n	3cbe8 <_sungetc_r+0xc>
   3cc3c:	e9c4 320e 	strd	r3, r2, [r4, #56]	; 0x38
   3cc40:	f104 0340 	add.w	r3, r4, #64	; 0x40
   3cc44:	6323      	str	r3, [r4, #48]	; 0x30
   3cc46:	2303      	movs	r3, #3
   3cc48:	6363      	str	r3, [r4, #52]	; 0x34
   3cc4a:	4623      	mov	r3, r4
   3cc4c:	f803 5f42 	strb.w	r5, [r3, #66]!
   3cc50:	6023      	str	r3, [r4, #0]
   3cc52:	2301      	movs	r3, #1
   3cc54:	e7dc      	b.n	3cc10 <_sungetc_r+0x34>

0003cc56 <__ssrefill_r>:
   3cc56:	b510      	push	{r4, lr}
   3cc58:	460c      	mov	r4, r1
   3cc5a:	6b09      	ldr	r1, [r1, #48]	; 0x30
   3cc5c:	b169      	cbz	r1, 3cc7a <__ssrefill_r+0x24>
   3cc5e:	f104 0340 	add.w	r3, r4, #64	; 0x40
   3cc62:	4299      	cmp	r1, r3
   3cc64:	d001      	beq.n	3cc6a <__ssrefill_r+0x14>
   3cc66:	f7fc fb17 	bl	39298 <_free_r>
   3cc6a:	2000      	movs	r0, #0
   3cc6c:	6be3      	ldr	r3, [r4, #60]	; 0x3c
   3cc6e:	6320      	str	r0, [r4, #48]	; 0x30
   3cc70:	6063      	str	r3, [r4, #4]
   3cc72:	b113      	cbz	r3, 3cc7a <__ssrefill_r+0x24>
   3cc74:	6ba3      	ldr	r3, [r4, #56]	; 0x38
   3cc76:	6023      	str	r3, [r4, #0]
   3cc78:	bd10      	pop	{r4, pc}
   3cc7a:	6923      	ldr	r3, [r4, #16]
   3cc7c:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3cc80:	6023      	str	r3, [r4, #0]
   3cc82:	2300      	movs	r3, #0
   3cc84:	6063      	str	r3, [r4, #4]
   3cc86:	89a3      	ldrh	r3, [r4, #12]
   3cc88:	f043 0320 	orr.w	r3, r3, #32
   3cc8c:	81a3      	strh	r3, [r4, #12]
   3cc8e:	e7f3      	b.n	3cc78 <__ssrefill_r+0x22>

0003cc90 <_sfread_r>:
   3cc90:	e92d 4ff7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3cc94:	461f      	mov	r7, r3
   3cc96:	4613      	mov	r3, r2
   3cc98:	4682      	mov	sl, r0
   3cc9a:	4688      	mov	r8, r1
   3cc9c:	437b      	muls	r3, r7
   3cc9e:	4691      	mov	r9, r2
   3cca0:	9c0c      	ldr	r4, [sp, #48]	; 0x30
   3cca2:	d027      	beq.n	3ccf4 <_sfread_r+0x64>
   3cca4:	461e      	mov	r6, r3
   3cca6:	e9d4 1500 	ldrd	r1, r5, [r4]
   3ccaa:	42b5      	cmp	r5, r6
   3ccac:	d30b      	bcc.n	3ccc6 <_sfread_r+0x36>
   3ccae:	4632      	mov	r2, r6
   3ccb0:	4640      	mov	r0, r8
   3ccb2:	f7fd f8d1 	bl	39e58 <memcpy>
   3ccb6:	6862      	ldr	r2, [r4, #4]
   3ccb8:	6821      	ldr	r1, [r4, #0]
   3ccba:	463b      	mov	r3, r7
   3ccbc:	1b92      	subs	r2, r2, r6
   3ccbe:	4431      	add	r1, r6
   3ccc0:	6062      	str	r2, [r4, #4]
   3ccc2:	6021      	str	r1, [r4, #0]
   3ccc4:	e016      	b.n	3ccf4 <_sfread_r+0x64>
   3ccc6:	462a      	mov	r2, r5
   3ccc8:	4640      	mov	r0, r8
   3ccca:	9301      	str	r3, [sp, #4]
   3cccc:	f7fd f8c4 	bl	39e58 <memcpy>
   3ccd0:	6822      	ldr	r2, [r4, #0]
   3ccd2:	2300      	movs	r3, #0
   3ccd4:	4621      	mov	r1, r4
   3ccd6:	4650      	mov	r0, sl
   3ccd8:	442a      	add	r2, r5
   3ccda:	6063      	str	r3, [r4, #4]
   3ccdc:	44a8      	add	r8, r5
   3ccde:	eba6 0b05 	sub.w	fp, r6, r5
   3cce2:	6022      	str	r2, [r4, #0]
   3cce4:	f7ff ffb7 	bl	3cc56 <__ssrefill_r>
   3cce8:	9b01      	ldr	r3, [sp, #4]
   3ccea:	b138      	cbz	r0, 3ccfc <_sfread_r+0x6c>
   3ccec:	18e9      	adds	r1, r5, r3
   3ccee:	1b89      	subs	r1, r1, r6
   3ccf0:	fbb1 f3f9 	udiv	r3, r1, r9
   3ccf4:	4618      	mov	r0, r3
   3ccf6:	b003      	add	sp, #12
   3ccf8:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3ccfc:	465e      	mov	r6, fp
   3ccfe:	e7d2      	b.n	3cca6 <_sfread_r+0x16>

0003cd00 <__ssvfiscanf_r>:
   3cd00:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3cd04:	ed2d 8b02 	vpush	{d8}
   3cd08:	b0df      	sub	sp, #380	; 0x17c
   3cd0a:	461e      	mov	r6, r3
   3cd0c:	ee08 0a10 	vmov	s16, r0
   3cd10:	460c      	mov	r4, r1
   3cd12:	f9b1 300c 	ldrsh.w	r3, [r1, #12]
   3cd16:	af02      	add	r7, sp, #8
   3cd18:	049d      	lsls	r5, r3, #18
   3cd1a:	613a      	str	r2, [r7, #16]
   3cd1c:	d406      	bmi.n	3cd2c <__ssvfiscanf_r+0x2c>
   3cd1e:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   3cd22:	818b      	strh	r3, [r1, #12]
   3cd24:	6e4b      	ldr	r3, [r1, #100]	; 0x64
   3cd26:	f423 5300 	bic.w	r3, r3, #8192	; 0x2000
   3cd2a:	664b      	str	r3, [r1, #100]	; 0x64
   3cd2c:	2300      	movs	r3, #0
   3cd2e:	469b      	mov	fp, r3
   3cd30:	617b      	str	r3, [r7, #20]
   3cd32:	62fb      	str	r3, [r7, #44]	; 0x2c
   3cd34:	e9c7 3307 	strd	r3, r3, [r7, #28]
   3cd38:	627b      	str	r3, [r7, #36]	; 0x24
   3cd3a:	693a      	ldr	r2, [r7, #16]
   3cd3c:	f812 3b01 	ldrb.w	r3, [r2], #1
   3cd40:	613a      	str	r2, [r7, #16]
   3cd42:	637b      	str	r3, [r7, #52]	; 0x34
   3cd44:	2b00      	cmp	r3, #0
   3cd46:	d032      	beq.n	3cdae <__ssvfiscanf_r+0xae>
   3cd48:	4a7e      	ldr	r2, [pc, #504]	; (3cf44 <__ssvfiscanf_r+0x244>)
   3cd4a:	f813 9002 	ldrb.w	r9, [r3, r2]
   3cd4e:	f019 0908 	ands.w	r9, r9, #8
   3cd52:	d019      	beq.n	3cd88 <__ssvfiscanf_r+0x88>
   3cd54:	6863      	ldr	r3, [r4, #4]
   3cd56:	2b00      	cmp	r3, #0
   3cd58:	dd0e      	ble.n	3cd78 <__ssvfiscanf_r+0x78>
   3cd5a:	6823      	ldr	r3, [r4, #0]
   3cd5c:	4979      	ldr	r1, [pc, #484]	; (3cf44 <__ssvfiscanf_r+0x244>)
   3cd5e:	781a      	ldrb	r2, [r3, #0]
   3cd60:	5c8a      	ldrb	r2, [r1, r2]
   3cd62:	0710      	lsls	r0, r2, #28
   3cd64:	d5e9      	bpl.n	3cd3a <__ssvfiscanf_r+0x3a>
   3cd66:	6afa      	ldr	r2, [r7, #44]	; 0x2c
   3cd68:	3301      	adds	r3, #1
   3cd6a:	3201      	adds	r2, #1
   3cd6c:	6023      	str	r3, [r4, #0]
   3cd6e:	62fa      	str	r2, [r7, #44]	; 0x2c
   3cd70:	6862      	ldr	r2, [r4, #4]
   3cd72:	3a01      	subs	r2, #1
   3cd74:	6062      	str	r2, [r4, #4]
   3cd76:	e7ed      	b.n	3cd54 <__ssvfiscanf_r+0x54>
   3cd78:	4621      	mov	r1, r4
   3cd7a:	ee18 0a10 	vmov	r0, s16
   3cd7e:	f7ff ff6a 	bl	3cc56 <__ssrefill_r>
   3cd82:	2800      	cmp	r0, #0
   3cd84:	d0e9      	beq.n	3cd5a <__ssvfiscanf_r+0x5a>
   3cd86:	e7d8      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3cd88:	2b25      	cmp	r3, #37	; 0x25
   3cd8a:	d161      	bne.n	3ce50 <__ssvfiscanf_r+0x150>
   3cd8c:	693b      	ldr	r3, [r7, #16]
   3cd8e:	220a      	movs	r2, #10
   3cd90:	f8c7 9028 	str.w	r9, [r7, #40]	; 0x28
   3cd94:	6939      	ldr	r1, [r7, #16]
   3cd96:	781d      	ldrb	r5, [r3, #0]
   3cd98:	3101      	adds	r1, #1
   3cd9a:	2d7a      	cmp	r5, #122	; 0x7a
   3cd9c:	6139      	str	r1, [r7, #16]
   3cd9e:	d806      	bhi.n	3cdae <__ssvfiscanf_r+0xae>
   3cda0:	2d42      	cmp	r5, #66	; 0x42
   3cda2:	d809      	bhi.n	3cdb8 <__ssvfiscanf_r+0xb8>
   3cda4:	2d2a      	cmp	r5, #42	; 0x2a
   3cda6:	d07a      	beq.n	3ce9e <__ssvfiscanf_r+0x19e>
   3cda8:	d844      	bhi.n	3ce34 <__ssvfiscanf_r+0x134>
   3cdaa:	2d25      	cmp	r5, #37	; 0x25
   3cdac:	d050      	beq.n	3ce50 <__ssvfiscanf_r+0x150>
   3cdae:	f1bb 0f00 	cmp.w	fp, #0
   3cdb2:	f000 8120 	beq.w	3cff6 <__ssvfiscanf_r+0x2f6>
   3cdb6:	e119      	b.n	3cfec <__ssvfiscanf_r+0x2ec>
   3cdb8:	3d43      	subs	r5, #67	; 0x43
   3cdba:	2d37      	cmp	r5, #55	; 0x37
   3cdbc:	f200 8104 	bhi.w	3cfc8 <__ssvfiscanf_r+0x2c8>
   3cdc0:	e8df f015 	tbh	[pc, r5, lsl #1]
   3cdc4:	00aa012b 	.word	0x00aa012b
   3cdc8:	01020102 	.word	0x01020102
   3cdcc:	01020102 	.word	0x01020102
   3cdd0:	01020102 	.word	0x01020102
   3cdd4:	00930102 	.word	0x00930102
   3cdd8:	01020102 	.word	0x01020102
   3cddc:	010200ae 	.word	0x010200ae
   3cde0:	01020102 	.word	0x01020102
   3cde4:	010200c6 	.word	0x010200c6
   3cde8:	01020102 	.word	0x01020102
   3cdec:	00ba0102 	.word	0x00ba0102
   3cdf0:	01020102 	.word	0x01020102
   3cdf4:	01020121 	.word	0x01020121
   3cdf8:	01020102 	.word	0x01020102
   3cdfc:	01020102 	.word	0x01020102
   3ce00:	01020102 	.word	0x01020102
   3ce04:	00ac012d 	.word	0x00ac012d
   3ce08:	01020102 	.word	0x01020102
   3ce0c:	00850102 	.word	0x00850102
   3ce10:	00930155 	.word	0x00930155
   3ce14:	00760102 	.word	0x00760102
   3ce18:	0134009b 	.word	0x0134009b
   3ce1c:	013100b0 	.word	0x013100b0
   3ce20:	01020102 	.word	0x01020102
   3ce24:	009700c8 	.word	0x009700c8
   3ce28:	010200b6 	.word	0x010200b6
   3ce2c:	00ba0102 	.word	0x00ba0102
   3ce30:	00970102 	.word	0x00970102
   3ce34:	f1a5 0330 	sub.w	r3, r5, #48	; 0x30
   3ce38:	2b09      	cmp	r3, #9
   3ce3a:	d8b8      	bhi.n	3cdae <__ssvfiscanf_r+0xae>
   3ce3c:	f019 0f8f 	tst.w	r9, #143	; 0x8f
   3ce40:	d1b5      	bne.n	3cdae <__ssvfiscanf_r+0xae>
   3ce42:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3ce44:	fb02 5503 	mla	r5, r2, r3, r5
   3ce48:	f1a5 0330 	sub.w	r3, r5, #48	; 0x30
   3ce4c:	62bb      	str	r3, [r7, #40]	; 0x28
   3ce4e:	e02d      	b.n	3ceac <__ssvfiscanf_r+0x1ac>
   3ce50:	6863      	ldr	r3, [r4, #4]
   3ce52:	2b00      	cmp	r3, #0
   3ce54:	dd0f      	ble.n	3ce76 <__ssvfiscanf_r+0x176>
   3ce56:	6823      	ldr	r3, [r4, #0]
   3ce58:	693a      	ldr	r2, [r7, #16]
   3ce5a:	7819      	ldrb	r1, [r3, #0]
   3ce5c:	f812 2c01 	ldrb.w	r2, [r2, #-1]
   3ce60:	4291      	cmp	r1, r2
   3ce62:	d1a4      	bne.n	3cdae <__ssvfiscanf_r+0xae>
   3ce64:	3301      	adds	r3, #1
   3ce66:	6862      	ldr	r2, [r4, #4]
   3ce68:	6023      	str	r3, [r4, #0]
   3ce6a:	3a01      	subs	r2, #1
   3ce6c:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3ce6e:	6062      	str	r2, [r4, #4]
   3ce70:	3301      	adds	r3, #1
   3ce72:	62fb      	str	r3, [r7, #44]	; 0x2c
   3ce74:	e761      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3ce76:	4621      	mov	r1, r4
   3ce78:	ee18 0a10 	vmov	r0, s16
   3ce7c:	f7ff feeb 	bl	3cc56 <__ssrefill_r>
   3ce80:	2800      	cmp	r0, #0
   3ce82:	d0e8      	beq.n	3ce56 <__ssvfiscanf_r+0x156>
   3ce84:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3ce86:	b113      	cbz	r3, 3ce8e <__ssvfiscanf_r+0x18e>
   3ce88:	89a3      	ldrh	r3, [r4, #12]
   3ce8a:	065b      	lsls	r3, r3, #25
   3ce8c:	d58f      	bpl.n	3cdae <__ssvfiscanf_r+0xae>
   3ce8e:	f1bb 0f00 	cmp.w	fp, #0
   3ce92:	f040 809f 	bne.w	3cfd4 <__ssvfiscanf_r+0x2d4>
   3ce96:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3ce9a:	627b      	str	r3, [r7, #36]	; 0x24
   3ce9c:	e0ab      	b.n	3cff6 <__ssvfiscanf_r+0x2f6>
   3ce9e:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3cea0:	ea53 0309 	orrs.w	r3, r3, r9
   3cea4:	62bb      	str	r3, [r7, #40]	; 0x28
   3cea6:	d182      	bne.n	3cdae <__ssvfiscanf_r+0xae>
   3cea8:	f04f 0910 	mov.w	r9, #16
   3ceac:	693b      	ldr	r3, [r7, #16]
   3ceae:	e771      	b.n	3cd94 <__ssvfiscanf_r+0x94>
   3ceb0:	f019 0f0f 	tst.w	r9, #15
   3ceb4:	f47f af7b 	bne.w	3cdae <__ssvfiscanf_r+0xae>
   3ceb8:	7859      	ldrb	r1, [r3, #1]
   3ceba:	296c      	cmp	r1, #108	; 0x6c
   3cebc:	d104      	bne.n	3cec8 <__ssvfiscanf_r+0x1c8>
   3cebe:	3302      	adds	r3, #2
   3cec0:	613b      	str	r3, [r7, #16]
   3cec2:	f049 0902 	orr.w	r9, r9, #2
   3cec6:	e7f1      	b.n	3ceac <__ssvfiscanf_r+0x1ac>
   3cec8:	f049 0901 	orr.w	r9, r9, #1
   3cecc:	e7ee      	b.n	3ceac <__ssvfiscanf_r+0x1ac>
   3cece:	f019 0f0f 	tst.w	r9, #15
   3ced2:	f47f af6c 	bne.w	3cdae <__ssvfiscanf_r+0xae>
   3ced6:	7859      	ldrb	r1, [r3, #1]
   3ced8:	2968      	cmp	r1, #104	; 0x68
   3ceda:	bf05      	ittet	eq
   3cedc:	3302      	addeq	r3, #2
   3cede:	f049 0908 	orreq.w	r9, r9, #8
   3cee2:	f049 0904 	orrne.w	r9, r9, #4
   3cee6:	613b      	streq	r3, [r7, #16]
   3cee8:	e7e0      	b.n	3ceac <__ssvfiscanf_r+0x1ac>
   3ceea:	f019 0f0f 	tst.w	r9, #15
   3ceee:	d0e8      	beq.n	3cec2 <__ssvfiscanf_r+0x1c2>
   3cef0:	e75d      	b.n	3cdae <__ssvfiscanf_r+0xae>
   3cef2:	f019 0f0f 	tst.w	r9, #15
   3cef6:	d0d9      	beq.n	3ceac <__ssvfiscanf_r+0x1ac>
   3cef8:	e759      	b.n	3cdae <__ssvfiscanf_r+0xae>
   3cefa:	f019 038f 	ands.w	r3, r9, #143	; 0x8f
   3cefe:	f47f af56 	bne.w	3cdae <__ssvfiscanf_r+0xae>
   3cf02:	f1bb 0f00 	cmp.w	fp, #0
   3cf06:	d104      	bne.n	3cf12 <__ssvfiscanf_r+0x212>
   3cf08:	b082      	sub	sp, #8
   3cf0a:	f10d 0b08 	add.w	fp, sp, #8
   3cf0e:	e9cb 3300 	strd	r3, r3, [fp]
   3cf12:	f049 0980 	orr.w	r9, r9, #128	; 0x80
   3cf16:	e7c9      	b.n	3ceac <__ssvfiscanf_r+0x1ac>
   3cf18:	f049 0901 	orr.w	r9, r9, #1
   3cf1c:	4b0a      	ldr	r3, [pc, #40]	; (3cf48 <__ssvfiscanf_r+0x248>)
   3cf1e:	e008      	b.n	3cf32 <__ssvfiscanf_r+0x232>
   3cf20:	f049 0901 	orr.w	r9, r9, #1
   3cf24:	4b09      	ldr	r3, [pc, #36]	; (3cf4c <__ssvfiscanf_r+0x24c>)
   3cf26:	617b      	str	r3, [r7, #20]
   3cf28:	2308      	movs	r3, #8
   3cf2a:	2503      	movs	r5, #3
   3cf2c:	61fb      	str	r3, [r7, #28]
   3cf2e:	e012      	b.n	3cf56 <__ssvfiscanf_r+0x256>
   3cf30:	4b06      	ldr	r3, [pc, #24]	; (3cf4c <__ssvfiscanf_r+0x24c>)
   3cf32:	617b      	str	r3, [r7, #20]
   3cf34:	230a      	movs	r3, #10
   3cf36:	e7f8      	b.n	3cf2a <__ssvfiscanf_r+0x22a>
   3cf38:	f449 7900 	orr.w	r9, r9, #512	; 0x200
   3cf3c:	4b03      	ldr	r3, [pc, #12]	; (3cf4c <__ssvfiscanf_r+0x24c>)
   3cf3e:	617b      	str	r3, [r7, #20]
   3cf40:	2310      	movs	r3, #16
   3cf42:	e7f2      	b.n	3cf2a <__ssvfiscanf_r+0x22a>
   3cf44:	0004056b 	.word	0x0004056b
   3cf48:	00034381 	.word	0x00034381
   3cf4c:	0003c215 	.word	0x0003c215
   3cf50:	f049 0901 	orr.w	r9, r9, #1
   3cf54:	2502      	movs	r5, #2
   3cf56:	6863      	ldr	r3, [r4, #4]
   3cf58:	2b00      	cmp	r3, #0
   3cf5a:	f340 808c 	ble.w	3d076 <__ssvfiscanf_r+0x376>
   3cf5e:	f019 0f40 	tst.w	r9, #64	; 0x40
   3cf62:	d106      	bne.n	3cf72 <__ssvfiscanf_r+0x272>
   3cf64:	6823      	ldr	r3, [r4, #0]
   3cf66:	49ab      	ldr	r1, [pc, #684]	; (3d214 <__ssvfiscanf_r+0x514>)
   3cf68:	781a      	ldrb	r2, [r3, #0]
   3cf6a:	5c52      	ldrb	r2, [r2, r1]
   3cf6c:	0711      	lsls	r1, r2, #28
   3cf6e:	f100 808b 	bmi.w	3d088 <__ssvfiscanf_r+0x388>
   3cf72:	2d02      	cmp	r5, #2
   3cf74:	f000 8396 	beq.w	3d6a4 <__ssvfiscanf_r+0x9a4>
   3cf78:	2d03      	cmp	r5, #3
   3cf7a:	f000 853b 	beq.w	3d9f4 <__ssvfiscanf_r+0xcf4>
   3cf7e:	f009 0310 	and.w	r3, r9, #16
   3cf82:	2d01      	cmp	r5, #1
   3cf84:	61bb      	str	r3, [r7, #24]
   3cf86:	f009 0301 	and.w	r3, r9, #1
   3cf8a:	f000 81cc 	beq.w	3d326 <__ssvfiscanf_r+0x626>
   3cf8e:	6abd      	ldr	r5, [r7, #40]	; 0x28
   3cf90:	2d01      	cmp	r5, #1
   3cf92:	bf38      	it	cc
   3cf94:	2501      	movcc	r5, #1
   3cf96:	2b00      	cmp	r3, #0
   3cf98:	f000 814e 	beq.w	3d238 <__ssvfiscanf_r+0x538>
   3cf9c:	69bb      	ldr	r3, [r7, #24]
   3cf9e:	2b00      	cmp	r3, #0
   3cfa0:	f040 810c 	bne.w	3d1bc <__ssvfiscanf_r+0x4bc>
   3cfa4:	4633      	mov	r3, r6
   3cfa6:	f019 0f80 	tst.w	r9, #128	; 0x80
   3cfaa:	f853 6b04 	ldr.w	r6, [r3], #4
   3cfae:	62bb      	str	r3, [r7, #40]	; 0x28
   3cfb0:	f000 810b 	beq.w	3d1ca <__ssvfiscanf_r+0x4ca>
   3cfb4:	b146      	cbz	r6, 3cfc8 <__ssvfiscanf_r+0x2c8>
   3cfb6:	2080      	movs	r0, #128	; 0x80
   3cfb8:	f7fc fc7a 	bl	398b0 <malloc>
   3cfbc:	4680      	mov	r8, r0
   3cfbe:	2800      	cmp	r0, #0
   3cfc0:	d176      	bne.n	3d0b0 <__ssvfiscanf_r+0x3b0>
   3cfc2:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3cfc6:	627b      	str	r3, [r7, #36]	; 0x24
   3cfc8:	f1bb 0f00 	cmp.w	fp, #0
   3cfcc:	d013      	beq.n	3cff6 <__ssvfiscanf_r+0x2f6>
   3cfce:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3cfd0:	3301      	adds	r3, #1
   3cfd2:	d10b      	bne.n	3cfec <__ssvfiscanf_r+0x2ec>
   3cfd4:	2400      	movs	r4, #0
   3cfd6:	4626      	mov	r6, r4
   3cfd8:	f8bb 3006 	ldrh.w	r3, [fp, #6]
   3cfdc:	f8db 5000 	ldr.w	r5, [fp]
   3cfe0:	42a3      	cmp	r3, r4
   3cfe2:	f300 85e5 	bgt.w	3dbb0 <__ssvfiscanf_r+0xeb0>
   3cfe6:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3cfea:	627b      	str	r3, [r7, #36]	; 0x24
   3cfec:	f8db 0000 	ldr.w	r0, [fp]
   3cff0:	b108      	cbz	r0, 3cff6 <__ssvfiscanf_r+0x2f6>
   3cff2:	f7fc f8f9 	bl	391e8 <free>
   3cff6:	6a78      	ldr	r0, [r7, #36]	; 0x24
   3cff8:	f507 77ba 	add.w	r7, r7, #372	; 0x174
   3cffc:	46bd      	mov	sp, r7
   3cffe:	ecbd 8b02 	vpop	{d8}
   3d002:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3d006:	6939      	ldr	r1, [r7, #16]
   3d008:	f107 0070 	add.w	r0, r7, #112	; 0x70
   3d00c:	f7fd feec 	bl	3ade8 <__sccl>
   3d010:	f049 0940 	orr.w	r9, r9, #64	; 0x40
   3d014:	2501      	movs	r5, #1
   3d016:	6138      	str	r0, [r7, #16]
   3d018:	e79d      	b.n	3cf56 <__ssvfiscanf_r+0x256>
   3d01a:	f049 0901 	orr.w	r9, r9, #1
   3d01e:	f049 0940 	orr.w	r9, r9, #64	; 0x40
   3d022:	2500      	movs	r5, #0
   3d024:	e797      	b.n	3cf56 <__ssvfiscanf_r+0x256>
   3d026:	f449 7908 	orr.w	r9, r9, #544	; 0x220
   3d02a:	e787      	b.n	3cf3c <__ssvfiscanf_r+0x23c>
   3d02c:	f019 0f10 	tst.w	r9, #16
   3d030:	f47f ae83 	bne.w	3cd3a <__ssvfiscanf_r+0x3a>
   3d034:	f019 0f08 	tst.w	r9, #8
   3d038:	f856 3b04 	ldr.w	r3, [r6], #4
   3d03c:	d002      	beq.n	3d044 <__ssvfiscanf_r+0x344>
   3d03e:	6afa      	ldr	r2, [r7, #44]	; 0x2c
   3d040:	701a      	strb	r2, [r3, #0]
   3d042:	e67a      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3d044:	f019 0f04 	tst.w	r9, #4
   3d048:	d002      	beq.n	3d050 <__ssvfiscanf_r+0x350>
   3d04a:	6afa      	ldr	r2, [r7, #44]	; 0x2c
   3d04c:	801a      	strh	r2, [r3, #0]
   3d04e:	e674      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3d050:	f019 0f01 	tst.w	r9, #1
   3d054:	d002      	beq.n	3d05c <__ssvfiscanf_r+0x35c>
   3d056:	6afa      	ldr	r2, [r7, #44]	; 0x2c
   3d058:	601a      	str	r2, [r3, #0]
   3d05a:	e66e      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3d05c:	f019 0f02 	tst.w	r9, #2
   3d060:	d0f9      	beq.n	3d056 <__ssvfiscanf_r+0x356>
   3d062:	6afa      	ldr	r2, [r7, #44]	; 0x2c
   3d064:	6af9      	ldr	r1, [r7, #44]	; 0x2c
   3d066:	17d2      	asrs	r2, r2, #31
   3d068:	e9c3 1200 	strd	r1, r2, [r3]
   3d06c:	e665      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3d06e:	4b6a      	ldr	r3, [pc, #424]	; (3d218 <__ssvfiscanf_r+0x518>)
   3d070:	617b      	str	r3, [r7, #20]
   3d072:	2300      	movs	r3, #0
   3d074:	e759      	b.n	3cf2a <__ssvfiscanf_r+0x22a>
   3d076:	4621      	mov	r1, r4
   3d078:	ee18 0a10 	vmov	r0, s16
   3d07c:	f7ff fdeb 	bl	3cc56 <__ssrefill_r>
   3d080:	2800      	cmp	r0, #0
   3d082:	f43f af6c 	beq.w	3cf5e <__ssvfiscanf_r+0x25e>
   3d086:	e6fd      	b.n	3ce84 <__ssvfiscanf_r+0x184>
   3d088:	6afa      	ldr	r2, [r7, #44]	; 0x2c
   3d08a:	3201      	adds	r2, #1
   3d08c:	62fa      	str	r2, [r7, #44]	; 0x2c
   3d08e:	6862      	ldr	r2, [r4, #4]
   3d090:	3a01      	subs	r2, #1
   3d092:	2a00      	cmp	r2, #0
   3d094:	6062      	str	r2, [r4, #4]
   3d096:	dd02      	ble.n	3d09e <__ssvfiscanf_r+0x39e>
   3d098:	3301      	adds	r3, #1
   3d09a:	6023      	str	r3, [r4, #0]
   3d09c:	e762      	b.n	3cf64 <__ssvfiscanf_r+0x264>
   3d09e:	4621      	mov	r1, r4
   3d0a0:	ee18 0a10 	vmov	r0, s16
   3d0a4:	f7ff fdd7 	bl	3cc56 <__ssrefill_r>
   3d0a8:	2800      	cmp	r0, #0
   3d0aa:	f43f af5b 	beq.w	3cf64 <__ssvfiscanf_r+0x264>
   3d0ae:	e6e9      	b.n	3ce84 <__ssvfiscanf_r+0x184>
   3d0b0:	f8bb 9006 	ldrh.w	r9, [fp, #6]
   3d0b4:	f8bb 3004 	ldrh.w	r3, [fp, #4]
   3d0b8:	6030      	str	r0, [r6, #0]
   3d0ba:	4599      	cmp	r9, r3
   3d0bc:	d314      	bcc.n	3d0e8 <__ssvfiscanf_r+0x3e8>
   3d0be:	f64f 72f6 	movw	r2, #65526	; 0xfff6
   3d0c2:	4293      	cmp	r3, r2
   3d0c4:	f63f af7d 	bhi.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d0c8:	3308      	adds	r3, #8
   3d0ca:	f8db 0000 	ldr.w	r0, [fp]
   3d0ce:	fa1f fa83 	uxth.w	sl, r3
   3d0d2:	ea4f 018a 	mov.w	r1, sl, lsl #2
   3d0d6:	f7fd fc73 	bl	3a9c0 <realloc>
   3d0da:	2800      	cmp	r0, #0
   3d0dc:	f43f af71 	beq.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d0e0:	f8cb 0000 	str.w	r0, [fp]
   3d0e4:	f8ab a004 	strh.w	sl, [fp, #4]
   3d0e8:	f8db 3000 	ldr.w	r3, [fp]
   3d0ec:	f109 0201 	add.w	r2, r9, #1
   3d0f0:	46b2      	mov	sl, r6
   3d0f2:	f843 6029 	str.w	r6, [r3, r9, lsl #2]
   3d0f6:	4646      	mov	r6, r8
   3d0f8:	f04f 0920 	mov.w	r9, #32
   3d0fc:	f8ab 2006 	strh.w	r2, [fp, #6]
   3d100:	2200      	movs	r2, #0
   3d102:	60fa      	str	r2, [r7, #12]
   3d104:	f7fc fb3c 	bl	39780 <__locale_mb_cur_max>
   3d108:	68fa      	ldr	r2, [r7, #12]
   3d10a:	4290      	cmp	r0, r2
   3d10c:	f43f aeba 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d110:	6821      	ldr	r1, [r4, #0]
   3d112:	f107 0c48 	add.w	ip, r7, #72	; 0x48
   3d116:	1c53      	adds	r3, r2, #1
   3d118:	f811 0b01 	ldrb.w	r0, [r1], #1
   3d11c:	f80c 0002 	strb.w	r0, [ip, r2]
   3d120:	6862      	ldr	r2, [r4, #4]
   3d122:	3a01      	subs	r2, #1
   3d124:	e9c4 1200 	strd	r1, r2, [r4]
   3d128:	6a3a      	ldr	r2, [r7, #32]
   3d12a:	2a03      	cmp	r2, #3
   3d12c:	d102      	bne.n	3d134 <__ssvfiscanf_r+0x434>
   3d12e:	6c3a      	ldr	r2, [r7, #64]	; 0x40
   3d130:	2a04      	cmp	r2, #4
   3d132:	d007      	beq.n	3d144 <__ssvfiscanf_r+0x444>
   3d134:	2208      	movs	r2, #8
   3d136:	2100      	movs	r1, #0
   3d138:	f107 0040 	add.w	r0, r7, #64	; 0x40
   3d13c:	623b      	str	r3, [r7, #32]
   3d13e:	f7fc ff57 	bl	39ff0 <memset>
   3d142:	6a3b      	ldr	r3, [r7, #32]
   3d144:	f107 0240 	add.w	r2, r7, #64	; 0x40
   3d148:	4631      	mov	r1, r6
   3d14a:	ee18 0a10 	vmov	r0, s16
   3d14e:	60fb      	str	r3, [r7, #12]
   3d150:	9200      	str	r2, [sp, #0]
   3d152:	f107 0248 	add.w	r2, r7, #72	; 0x48
   3d156:	f7fc fde3 	bl	39d20 <_mbrtowc_r>
   3d15a:	1c42      	adds	r2, r0, #1
   3d15c:	6238      	str	r0, [r7, #32]
   3d15e:	f43f ae91 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d162:	68fb      	ldr	r3, [r7, #12]
   3d164:	2800      	cmp	r0, #0
   3d166:	d135      	bne.n	3d1d4 <__ssvfiscanf_r+0x4d4>
   3d168:	69ba      	ldr	r2, [r7, #24]
   3d16a:	b902      	cbnz	r2, 3d16e <__ssvfiscanf_r+0x46e>
   3d16c:	6032      	str	r2, [r6, #0]
   3d16e:	6afa      	ldr	r2, [r7, #44]	; 0x2c
   3d170:	441a      	add	r2, r3
   3d172:	6a3b      	ldr	r3, [r7, #32]
   3d174:	2b03      	cmp	r3, #3
   3d176:	62fa      	str	r2, [r7, #44]	; 0x2c
   3d178:	d102      	bne.n	3d180 <__ssvfiscanf_r+0x480>
   3d17a:	6c3b      	ldr	r3, [r7, #64]	; 0x40
   3d17c:	2b04      	cmp	r3, #4
   3d17e:	d000      	beq.n	3d182 <__ssvfiscanf_r+0x482>
   3d180:	3d01      	subs	r5, #1
   3d182:	69bb      	ldr	r3, [r7, #24]
   3d184:	b9c3      	cbnz	r3, 3d1b8 <__ssvfiscanf_r+0x4b8>
   3d186:	f1ba 0f00 	cmp.w	sl, #0
   3d18a:	d014      	beq.n	3d1b6 <__ssvfiscanf_r+0x4b6>
   3d18c:	eba6 0308 	sub.w	r3, r6, r8
   3d190:	ebb9 0fa3 	cmp.w	r9, r3, asr #2
   3d194:	d80f      	bhi.n	3d1b6 <__ssvfiscanf_r+0x4b6>
   3d196:	4640      	mov	r0, r8
   3d198:	ea4f 01c9 	mov.w	r1, r9, lsl #3
   3d19c:	60fb      	str	r3, [r7, #12]
   3d19e:	f7fd fc0f 	bl	3a9c0 <realloc>
   3d1a2:	4680      	mov	r8, r0
   3d1a4:	2800      	cmp	r0, #0
   3d1a6:	f43f af0c 	beq.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d1aa:	68fb      	ldr	r3, [r7, #12]
   3d1ac:	ea4f 0949 	mov.w	r9, r9, lsl #1
   3d1b0:	f8ca 0000 	str.w	r0, [sl]
   3d1b4:	18c6      	adds	r6, r0, r3
   3d1b6:	3604      	adds	r6, #4
   3d1b8:	2200      	movs	r2, #0
   3d1ba:	e00f      	b.n	3d1dc <__ssvfiscanf_r+0x4dc>
   3d1bc:	f04f 0900 	mov.w	r9, #0
   3d1c0:	62be      	str	r6, [r7, #40]	; 0x28
   3d1c2:	46c8      	mov	r8, r9
   3d1c4:	46ca      	mov	sl, r9
   3d1c6:	464e      	mov	r6, r9
   3d1c8:	e79a      	b.n	3d100 <__ssvfiscanf_r+0x400>
   3d1ca:	f8d7 9018 	ldr.w	r9, [r7, #24]
   3d1ce:	46c8      	mov	r8, r9
   3d1d0:	46ca      	mov	sl, r9
   3d1d2:	e795      	b.n	3d100 <__ssvfiscanf_r+0x400>
   3d1d4:	6a3a      	ldr	r2, [r7, #32]
   3d1d6:	3202      	adds	r2, #2
   3d1d8:	d1c9      	bne.n	3d16e <__ssvfiscanf_r+0x46e>
   3d1da:	461a      	mov	r2, r3
   3d1dc:	6863      	ldr	r3, [r4, #4]
   3d1de:	2b00      	cmp	r3, #0
   3d1e0:	dc14      	bgt.n	3d20c <__ssvfiscanf_r+0x50c>
   3d1e2:	4621      	mov	r1, r4
   3d1e4:	ee18 0a10 	vmov	r0, s16
   3d1e8:	60fa      	str	r2, [r7, #12]
   3d1ea:	f7ff fd34 	bl	3cc56 <__ssrefill_r>
   3d1ee:	68fa      	ldr	r2, [r7, #12]
   3d1f0:	b160      	cbz	r0, 3d20c <__ssvfiscanf_r+0x50c>
   3d1f2:	2a00      	cmp	r2, #0
   3d1f4:	f47f ae46 	bne.w	3ce84 <__ssvfiscanf_r+0x184>
   3d1f8:	f1ba 0f00 	cmp.w	sl, #0
   3d1fc:	d10e      	bne.n	3d21c <__ssvfiscanf_r+0x51c>
   3d1fe:	69bb      	ldr	r3, [r7, #24]
   3d200:	b913      	cbnz	r3, 3d208 <__ssvfiscanf_r+0x508>
   3d202:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3d204:	3301      	adds	r3, #1
   3d206:	627b      	str	r3, [r7, #36]	; 0x24
   3d208:	6abe      	ldr	r6, [r7, #40]	; 0x28
   3d20a:	e596      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3d20c:	2d00      	cmp	r5, #0
   3d20e:	f47f af78 	bne.w	3d102 <__ssvfiscanf_r+0x402>
   3d212:	e7f1      	b.n	3d1f8 <__ssvfiscanf_r+0x4f8>
   3d214:	0004056b 	.word	0x0004056b
   3d218:	00034381 	.word	0x00034381
   3d21c:	eba6 0108 	sub.w	r1, r6, r8
   3d220:	ebb9 0fa1 	cmp.w	r9, r1, asr #2
   3d224:	d9eb      	bls.n	3d1fe <__ssvfiscanf_r+0x4fe>
   3d226:	f8da 0000 	ldr.w	r0, [sl]
   3d22a:	f7fd fbc9 	bl	3a9c0 <realloc>
   3d22e:	2800      	cmp	r0, #0
   3d230:	d0e5      	beq.n	3d1fe <__ssvfiscanf_r+0x4fe>
   3d232:	f8ca 0000 	str.w	r0, [sl]
   3d236:	e7e2      	b.n	3d1fe <__ssvfiscanf_r+0x4fe>
   3d238:	69ba      	ldr	r2, [r7, #24]
   3d23a:	b1e2      	cbz	r2, 3d276 <__ssvfiscanf_r+0x576>
   3d23c:	4698      	mov	r8, r3
   3d23e:	e9d4 2300 	ldrd	r2, r3, [r4]
   3d242:	429d      	cmp	r5, r3
   3d244:	dd11      	ble.n	3d26a <__ssvfiscanf_r+0x56a>
   3d246:	4498      	add	r8, r3
   3d248:	1aed      	subs	r5, r5, r3
   3d24a:	4413      	add	r3, r2
   3d24c:	4621      	mov	r1, r4
   3d24e:	ee18 0a10 	vmov	r0, s16
   3d252:	6023      	str	r3, [r4, #0]
   3d254:	f7ff fcff 	bl	3cc56 <__ssrefill_r>
   3d258:	2800      	cmp	r0, #0
   3d25a:	d0f0      	beq.n	3d23e <__ssvfiscanf_r+0x53e>
   3d25c:	f1b8 0f00 	cmp.w	r8, #0
   3d260:	f43f ae10 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d264:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3d266:	4443      	add	r3, r8
   3d268:	e603      	b.n	3ce72 <__ssvfiscanf_r+0x172>
   3d26a:	1b5b      	subs	r3, r3, r5
   3d26c:	442a      	add	r2, r5
   3d26e:	44a8      	add	r8, r5
   3d270:	6063      	str	r3, [r4, #4]
   3d272:	6022      	str	r2, [r4, #0]
   3d274:	e7f6      	b.n	3d264 <__ssvfiscanf_r+0x564>
   3d276:	46b0      	mov	r8, r6
   3d278:	f019 0980 	ands.w	r9, r9, #128	; 0x80
   3d27c:	f858 6b04 	ldr.w	r6, [r8], #4
   3d280:	d030      	beq.n	3d2e4 <__ssvfiscanf_r+0x5e4>
   3d282:	2e00      	cmp	r6, #0
   3d284:	f43f ad93 	beq.w	3cdae <__ssvfiscanf_r+0xae>
   3d288:	4628      	mov	r0, r5
   3d28a:	f7fc fb11 	bl	398b0 <malloc>
   3d28e:	62b8      	str	r0, [r7, #40]	; 0x28
   3d290:	2800      	cmp	r0, #0
   3d292:	f43f adfc 	beq.w	3ce8e <__ssvfiscanf_r+0x18e>
   3d296:	f8bb a006 	ldrh.w	sl, [fp, #6]
   3d29a:	f8bb 9004 	ldrh.w	r9, [fp, #4]
   3d29e:	6030      	str	r0, [r6, #0]
   3d2a0:	45ca      	cmp	sl, r9
   3d2a2:	d315      	bcc.n	3d2d0 <__ssvfiscanf_r+0x5d0>
   3d2a4:	f64f 73f6 	movw	r3, #65526	; 0xfff6
   3d2a8:	4599      	cmp	r9, r3
   3d2aa:	f63f ae93 	bhi.w	3cfd4 <__ssvfiscanf_r+0x2d4>
   3d2ae:	f109 0908 	add.w	r9, r9, #8
   3d2b2:	f8db 0000 	ldr.w	r0, [fp]
   3d2b6:	fa1f f989 	uxth.w	r9, r9
   3d2ba:	ea4f 0189 	mov.w	r1, r9, lsl #2
   3d2be:	f7fd fb7f 	bl	3a9c0 <realloc>
   3d2c2:	2800      	cmp	r0, #0
   3d2c4:	f43f ae86 	beq.w	3cfd4 <__ssvfiscanf_r+0x2d4>
   3d2c8:	f8cb 0000 	str.w	r0, [fp]
   3d2cc:	f8ab 9004 	strh.w	r9, [fp, #4]
   3d2d0:	f8db 3000 	ldr.w	r3, [fp]
   3d2d4:	f10a 0201 	add.w	r2, sl, #1
   3d2d8:	46b1      	mov	r9, r6
   3d2da:	f843 602a 	str.w	r6, [r3, sl, lsl #2]
   3d2de:	6abe      	ldr	r6, [r7, #40]	; 0x28
   3d2e0:	f8ab 2006 	strh.w	r2, [fp, #6]
   3d2e4:	4631      	mov	r1, r6
   3d2e6:	462b      	mov	r3, r5
   3d2e8:	2201      	movs	r2, #1
   3d2ea:	ee18 0a10 	vmov	r0, s16
   3d2ee:	9400      	str	r4, [sp, #0]
   3d2f0:	f7ff fcce 	bl	3cc90 <_sfread_r>
   3d2f4:	4606      	mov	r6, r0
   3d2f6:	2800      	cmp	r0, #0
   3d2f8:	f43f adc4 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d2fc:	f1b9 0f00 	cmp.w	r9, #0
   3d300:	d009      	beq.n	3d316 <__ssvfiscanf_r+0x616>
   3d302:	4285      	cmp	r5, r0
   3d304:	d907      	bls.n	3d316 <__ssvfiscanf_r+0x616>
   3d306:	4601      	mov	r1, r0
   3d308:	f8d9 0000 	ldr.w	r0, [r9]
   3d30c:	f7fd fb58 	bl	3a9c0 <realloc>
   3d310:	b108      	cbz	r0, 3d316 <__ssvfiscanf_r+0x616>
   3d312:	f8c9 0000 	str.w	r0, [r9]
   3d316:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3d318:	4433      	add	r3, r6
   3d31a:	4646      	mov	r6, r8
   3d31c:	62fb      	str	r3, [r7, #44]	; 0x2c
   3d31e:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3d320:	3301      	adds	r3, #1
   3d322:	627b      	str	r3, [r7, #36]	; 0x24
   3d324:	e509      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3d326:	6aba      	ldr	r2, [r7, #40]	; 0x28
   3d328:	461d      	mov	r5, r3
   3d32a:	69bb      	ldr	r3, [r7, #24]
   3d32c:	2a00      	cmp	r2, #0
   3d32e:	bf08      	it	eq
   3d330:	f04f 32ff 	moveq.w	r2, #4294967295	; 0xffffffff
   3d334:	62ba      	str	r2, [r7, #40]	; 0x28
   3d336:	2d00      	cmp	r5, #0
   3d338:	f000 8107 	beq.w	3d54a <__ssvfiscanf_r+0x84a>
   3d33c:	2b00      	cmp	r3, #0
   3d33e:	f040 80a7 	bne.w	3d490 <__ssvfiscanf_r+0x790>
   3d342:	4633      	mov	r3, r6
   3d344:	f019 0f80 	tst.w	r9, #128	; 0x80
   3d348:	f853 5b04 	ldr.w	r5, [r3], #4
   3d34c:	607b      	str	r3, [r7, #4]
   3d34e:	f000 80a8 	beq.w	3d4a2 <__ssvfiscanf_r+0x7a2>
   3d352:	2d00      	cmp	r5, #0
   3d354:	f43f ae38 	beq.w	3cfc8 <__ssvfiscanf_r+0x2c8>
   3d358:	2080      	movs	r0, #128	; 0x80
   3d35a:	f7fc faa9 	bl	398b0 <malloc>
   3d35e:	4680      	mov	r8, r0
   3d360:	2800      	cmp	r0, #0
   3d362:	f43f ae2e 	beq.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d366:	f8bb 6006 	ldrh.w	r6, [fp, #6]
   3d36a:	f8bb 3004 	ldrh.w	r3, [fp, #4]
   3d36e:	6028      	str	r0, [r5, #0]
   3d370:	429e      	cmp	r6, r3
   3d372:	d314      	bcc.n	3d39e <__ssvfiscanf_r+0x69e>
   3d374:	f64f 72f6 	movw	r2, #65526	; 0xfff6
   3d378:	4293      	cmp	r3, r2
   3d37a:	f63f ae22 	bhi.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d37e:	3308      	adds	r3, #8
   3d380:	f8db 0000 	ldr.w	r0, [fp]
   3d384:	fa1f f983 	uxth.w	r9, r3
   3d388:	ea4f 0189 	mov.w	r1, r9, lsl #2
   3d38c:	f7fd fb18 	bl	3a9c0 <realloc>
   3d390:	2800      	cmp	r0, #0
   3d392:	f43f ae16 	beq.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d396:	f8cb 0000 	str.w	r0, [fp]
   3d39a:	f8ab 9004 	strh.w	r9, [fp, #4]
   3d39e:	f8db 3000 	ldr.w	r3, [fp]
   3d3a2:	1c72      	adds	r2, r6, #1
   3d3a4:	60fd      	str	r5, [r7, #12]
   3d3a6:	f04f 0920 	mov.w	r9, #32
   3d3aa:	f843 5026 	str.w	r5, [r3, r6, lsl #2]
   3d3ae:	4645      	mov	r5, r8
   3d3b0:	f8ab 2006 	strh.w	r2, [fp, #6]
   3d3b4:	2600      	movs	r6, #0
   3d3b6:	f7fc f9e3 	bl	39780 <__locale_mb_cur_max>
   3d3ba:	42b0      	cmp	r0, r6
   3d3bc:	f43f ad62 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d3c0:	1c73      	adds	r3, r6, #1
   3d3c2:	f107 0148 	add.w	r1, r7, #72	; 0x48
   3d3c6:	60bb      	str	r3, [r7, #8]
   3d3c8:	6823      	ldr	r3, [r4, #0]
   3d3ca:	f813 2b01 	ldrb.w	r2, [r3], #1
   3d3ce:	558a      	strb	r2, [r1, r6]
   3d3d0:	6862      	ldr	r2, [r4, #4]
   3d3d2:	3a01      	subs	r2, #1
   3d3d4:	e9c4 3200 	strd	r3, r2, [r4]
   3d3d8:	6a3b      	ldr	r3, [r7, #32]
   3d3da:	2b03      	cmp	r3, #3
   3d3dc:	d102      	bne.n	3d3e4 <__ssvfiscanf_r+0x6e4>
   3d3de:	6bbb      	ldr	r3, [r7, #56]	; 0x38
   3d3e0:	2b04      	cmp	r3, #4
   3d3e2:	d005      	beq.n	3d3f0 <__ssvfiscanf_r+0x6f0>
   3d3e4:	2208      	movs	r2, #8
   3d3e6:	2100      	movs	r1, #0
   3d3e8:	f107 0038 	add.w	r0, r7, #56	; 0x38
   3d3ec:	f7fc fe00 	bl	39ff0 <memset>
   3d3f0:	f107 0338 	add.w	r3, r7, #56	; 0x38
   3d3f4:	f107 0248 	add.w	r2, r7, #72	; 0x48
   3d3f8:	4629      	mov	r1, r5
   3d3fa:	ee18 0a10 	vmov	r0, s16
   3d3fe:	9300      	str	r3, [sp, #0]
   3d400:	68bb      	ldr	r3, [r7, #8]
   3d402:	f7fc fc8d 	bl	39d20 <_mbrtowc_r>
   3d406:	4603      	mov	r3, r0
   3d408:	6238      	str	r0, [r7, #32]
   3d40a:	1c58      	adds	r0, r3, #1
   3d40c:	f43f ad3a 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d410:	2b00      	cmp	r3, #0
   3d412:	d14c      	bne.n	3d4ae <__ssvfiscanf_r+0x7ae>
   3d414:	602b      	str	r3, [r5, #0]
   3d416:	682e      	ldr	r6, [r5, #0]
   3d418:	1c71      	adds	r1, r6, #1
   3d41a:	d059      	beq.n	3d4d0 <__ssvfiscanf_r+0x7d0>
   3d41c:	2208      	movs	r2, #8
   3d41e:	2100      	movs	r1, #0
   3d420:	f107 0040 	add.w	r0, r7, #64	; 0x40
   3d424:	f7fc fde4 	bl	39ff0 <memset>
   3d428:	4b9d      	ldr	r3, [pc, #628]	; (3d6a0 <__ssvfiscanf_r+0x9a0>)
   3d42a:	4632      	mov	r2, r6
   3d42c:	f107 0130 	add.w	r1, r7, #48	; 0x30
   3d430:	f8d3 30e0 	ldr.w	r3, [r3, #224]	; 0xe0
   3d434:	ee18 0a10 	vmov	r0, s16
   3d438:	603b      	str	r3, [r7, #0]
   3d43a:	f107 0340 	add.w	r3, r7, #64	; 0x40
   3d43e:	683e      	ldr	r6, [r7, #0]
   3d440:	47b0      	blx	r6
   3d442:	2801      	cmp	r0, #1
   3d444:	d146      	bne.n	3d4d4 <__ssvfiscanf_r+0x7d4>
   3d446:	f897 a030 	ldrb.w	sl, [r7, #48]	; 0x30
   3d44a:	f107 0370 	add.w	r3, r7, #112	; 0x70
   3d44e:	f813 300a 	ldrb.w	r3, [r3, sl]
   3d452:	2b00      	cmp	r3, #0
   3d454:	d14b      	bne.n	3d4ee <__ssvfiscanf_r+0x7ee>
   3d456:	f107 0648 	add.w	r6, r7, #72	; 0x48
   3d45a:	68bb      	ldr	r3, [r7, #8]
   3d45c:	2b00      	cmp	r3, #0
   3d45e:	d13c      	bne.n	3d4da <__ssvfiscanf_r+0x7da>
   3d460:	69bb      	ldr	r3, [r7, #24]
   3d462:	b99b      	cbnz	r3, 3d48c <__ssvfiscanf_r+0x78c>
   3d464:	602b      	str	r3, [r5, #0]
   3d466:	68fb      	ldr	r3, [r7, #12]
   3d468:	b16b      	cbz	r3, 3d486 <__ssvfiscanf_r+0x786>
   3d46a:	eba5 0108 	sub.w	r1, r5, r8
   3d46e:	108b      	asrs	r3, r1, #2
   3d470:	3301      	adds	r3, #1
   3d472:	4599      	cmp	r9, r3
   3d474:	d907      	bls.n	3d486 <__ssvfiscanf_r+0x786>
   3d476:	68fb      	ldr	r3, [r7, #12]
   3d478:	3104      	adds	r1, #4
   3d47a:	6818      	ldr	r0, [r3, #0]
   3d47c:	f7fd faa0 	bl	3a9c0 <realloc>
   3d480:	b108      	cbz	r0, 3d486 <__ssvfiscanf_r+0x786>
   3d482:	68fb      	ldr	r3, [r7, #12]
   3d484:	6018      	str	r0, [r3, #0]
   3d486:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3d488:	3301      	adds	r3, #1
   3d48a:	627b      	str	r3, [r7, #36]	; 0x24
   3d48c:	687e      	ldr	r6, [r7, #4]
   3d48e:	e454      	b.n	3cd3a <__ssvfiscanf_r+0x3a>
   3d490:	f04f 0900 	mov.w	r9, #0
   3d494:	f107 0534 	add.w	r5, r7, #52	; 0x34
   3d498:	607e      	str	r6, [r7, #4]
   3d49a:	46c8      	mov	r8, r9
   3d49c:	f8c7 900c 	str.w	r9, [r7, #12]
   3d4a0:	e788      	b.n	3d3b4 <__ssvfiscanf_r+0x6b4>
   3d4a2:	f8d7 9018 	ldr.w	r9, [r7, #24]
   3d4a6:	46c8      	mov	r8, r9
   3d4a8:	f8c7 900c 	str.w	r9, [r7, #12]
   3d4ac:	e782      	b.n	3d3b4 <__ssvfiscanf_r+0x6b4>
   3d4ae:	6a3b      	ldr	r3, [r7, #32]
   3d4b0:	3302      	adds	r3, #2
   3d4b2:	d1b0      	bne.n	3d416 <__ssvfiscanf_r+0x716>
   3d4b4:	68be      	ldr	r6, [r7, #8]
   3d4b6:	6863      	ldr	r3, [r4, #4]
   3d4b8:	2b00      	cmp	r3, #0
   3d4ba:	dc41      	bgt.n	3d540 <__ssvfiscanf_r+0x840>
   3d4bc:	4621      	mov	r1, r4
   3d4be:	ee18 0a10 	vmov	r0, s16
   3d4c2:	f7ff fbc8 	bl	3cc56 <__ssrefill_r>
   3d4c6:	2800      	cmp	r0, #0
   3d4c8:	d03a      	beq.n	3d540 <__ssvfiscanf_r+0x840>
   3d4ca:	2e00      	cmp	r6, #0
   3d4cc:	d0c8      	beq.n	3d460 <__ssvfiscanf_r+0x760>
   3d4ce:	e4d9      	b.n	3ce84 <__ssvfiscanf_r+0x184>
   3d4d0:	46b2      	mov	sl, r6
   3d4d2:	e7ba      	b.n	3d44a <__ssvfiscanf_r+0x74a>
   3d4d4:	f04f 0a00 	mov.w	sl, #0
   3d4d8:	e7b7      	b.n	3d44a <__ssvfiscanf_r+0x74a>
   3d4da:	68bb      	ldr	r3, [r7, #8]
   3d4dc:	4622      	mov	r2, r4
   3d4de:	ee18 0a10 	vmov	r0, s16
   3d4e2:	3b01      	subs	r3, #1
   3d4e4:	5cf1      	ldrb	r1, [r6, r3]
   3d4e6:	60bb      	str	r3, [r7, #8]
   3d4e8:	f7ff fb78 	bl	3cbdc <_sungetc_r>
   3d4ec:	e7b5      	b.n	3d45a <__ssvfiscanf_r+0x75a>
   3d4ee:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3d4f0:	68ba      	ldr	r2, [r7, #8]
   3d4f2:	4413      	add	r3, r2
   3d4f4:	62fb      	str	r3, [r7, #44]	; 0x2c
   3d4f6:	6a3b      	ldr	r3, [r7, #32]
   3d4f8:	2b03      	cmp	r3, #3
   3d4fa:	d102      	bne.n	3d502 <__ssvfiscanf_r+0x802>
   3d4fc:	6bbb      	ldr	r3, [r7, #56]	; 0x38
   3d4fe:	2b04      	cmp	r3, #4
   3d500:	d002      	beq.n	3d508 <__ssvfiscanf_r+0x808>
   3d502:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d504:	3b01      	subs	r3, #1
   3d506:	62bb      	str	r3, [r7, #40]	; 0x28
   3d508:	69bb      	ldr	r3, [r7, #24]
   3d50a:	b9bb      	cbnz	r3, 3d53c <__ssvfiscanf_r+0x83c>
   3d50c:	68fb      	ldr	r3, [r7, #12]
   3d50e:	3504      	adds	r5, #4
   3d510:	b1a3      	cbz	r3, 3d53c <__ssvfiscanf_r+0x83c>
   3d512:	eba5 0608 	sub.w	r6, r5, r8
   3d516:	ebb9 0fa6 	cmp.w	r9, r6, asr #2
   3d51a:	d80d      	bhi.n	3d538 <__ssvfiscanf_r+0x838>
   3d51c:	4640      	mov	r0, r8
   3d51e:	ea4f 01c9 	mov.w	r1, r9, lsl #3
   3d522:	f7fd fa4d 	bl	3a9c0 <realloc>
   3d526:	4680      	mov	r8, r0
   3d528:	2800      	cmp	r0, #0
   3d52a:	f43f ad4a 	beq.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d52e:	68fb      	ldr	r3, [r7, #12]
   3d530:	1985      	adds	r5, r0, r6
   3d532:	ea4f 0949 	mov.w	r9, r9, lsl #1
   3d536:	6018      	str	r0, [r3, #0]
   3d538:	69be      	ldr	r6, [r7, #24]
   3d53a:	e7bc      	b.n	3d4b6 <__ssvfiscanf_r+0x7b6>
   3d53c:	2600      	movs	r6, #0
   3d53e:	e7ba      	b.n	3d4b6 <__ssvfiscanf_r+0x7b6>
   3d540:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d542:	2b00      	cmp	r3, #0
   3d544:	f47f af37 	bne.w	3d3b6 <__ssvfiscanf_r+0x6b6>
   3d548:	e78a      	b.n	3d460 <__ssvfiscanf_r+0x760>
   3d54a:	b1eb      	cbz	r3, 3d588 <__ssvfiscanf_r+0x888>
   3d54c:	f107 0870 	add.w	r8, r7, #112	; 0x70
   3d550:	6823      	ldr	r3, [r4, #0]
   3d552:	781a      	ldrb	r2, [r3, #0]
   3d554:	f818 2002 	ldrb.w	r2, [r8, r2]
   3d558:	b90a      	cbnz	r2, 3d55e <__ssvfiscanf_r+0x85e>
   3d55a:	b995      	cbnz	r5, 3d582 <__ssvfiscanf_r+0x882>
   3d55c:	e427      	b.n	3cdae <__ssvfiscanf_r+0xae>
   3d55e:	3301      	adds	r3, #1
   3d560:	6862      	ldr	r2, [r4, #4]
   3d562:	3501      	adds	r5, #1
   3d564:	6023      	str	r3, [r4, #0]
   3d566:	3a01      	subs	r2, #1
   3d568:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d56a:	6062      	str	r2, [r4, #4]
   3d56c:	42ab      	cmp	r3, r5
   3d56e:	d008      	beq.n	3d582 <__ssvfiscanf_r+0x882>
   3d570:	2a00      	cmp	r2, #0
   3d572:	dced      	bgt.n	3d550 <__ssvfiscanf_r+0x850>
   3d574:	4621      	mov	r1, r4
   3d576:	ee18 0a10 	vmov	r0, s16
   3d57a:	f7ff fb6c 	bl	3cc56 <__ssrefill_r>
   3d57e:	2800      	cmp	r0, #0
   3d580:	d0e6      	beq.n	3d550 <__ssvfiscanf_r+0x850>
   3d582:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3d584:	442b      	add	r3, r5
   3d586:	e474      	b.n	3ce72 <__ssvfiscanf_r+0x172>
   3d588:	4633      	mov	r3, r6
   3d58a:	f019 0580 	ands.w	r5, r9, #128	; 0x80
   3d58e:	f853 6b04 	ldr.w	r6, [r3], #4
   3d592:	61bb      	str	r3, [r7, #24]
   3d594:	f000 8082 	beq.w	3d69c <__ssvfiscanf_r+0x99c>
   3d598:	2e00      	cmp	r6, #0
   3d59a:	f43f ac08 	beq.w	3cdae <__ssvfiscanf_r+0xae>
   3d59e:	2020      	movs	r0, #32
   3d5a0:	f7fc f986 	bl	398b0 <malloc>
   3d5a4:	4680      	mov	r8, r0
   3d5a6:	2800      	cmp	r0, #0
   3d5a8:	f43f ac71 	beq.w	3ce8e <__ssvfiscanf_r+0x18e>
   3d5ac:	f8bb 9006 	ldrh.w	r9, [fp, #6]
   3d5b0:	f8bb 5004 	ldrh.w	r5, [fp, #4]
   3d5b4:	6030      	str	r0, [r6, #0]
   3d5b6:	45a9      	cmp	r9, r5
   3d5b8:	d312      	bcc.n	3d5e0 <__ssvfiscanf_r+0x8e0>
   3d5ba:	f64f 73f6 	movw	r3, #65526	; 0xfff6
   3d5be:	429d      	cmp	r5, r3
   3d5c0:	f63f ad08 	bhi.w	3cfd4 <__ssvfiscanf_r+0x2d4>
   3d5c4:	3508      	adds	r5, #8
   3d5c6:	f8db 0000 	ldr.w	r0, [fp]
   3d5ca:	b2ad      	uxth	r5, r5
   3d5cc:	00a9      	lsls	r1, r5, #2
   3d5ce:	f7fd f9f7 	bl	3a9c0 <realloc>
   3d5d2:	2800      	cmp	r0, #0
   3d5d4:	f43f acfe 	beq.w	3cfd4 <__ssvfiscanf_r+0x2d4>
   3d5d8:	f8cb 0000 	str.w	r0, [fp]
   3d5dc:	f8ab 5004 	strh.w	r5, [fp, #4]
   3d5e0:	f8db 3000 	ldr.w	r3, [fp]
   3d5e4:	f109 0201 	add.w	r2, r9, #1
   3d5e8:	46b2      	mov	sl, r6
   3d5ea:	2520      	movs	r5, #32
   3d5ec:	f843 6029 	str.w	r6, [r3, r9, lsl #2]
   3d5f0:	4646      	mov	r6, r8
   3d5f2:	f8ab 2006 	strh.w	r2, [fp, #6]
   3d5f6:	46b0      	mov	r8, r6
   3d5f8:	f107 0970 	add.w	r9, r7, #112	; 0x70
   3d5fc:	6823      	ldr	r3, [r4, #0]
   3d5fe:	781a      	ldrb	r2, [r3, #0]
   3d600:	f819 2002 	ldrb.w	r2, [r9, r2]
   3d604:	b36a      	cbz	r2, 3d662 <__ssvfiscanf_r+0x962>
   3d606:	6862      	ldr	r2, [r4, #4]
   3d608:	3a01      	subs	r2, #1
   3d60a:	6062      	str	r2, [r4, #4]
   3d60c:	1c5a      	adds	r2, r3, #1
   3d60e:	6022      	str	r2, [r4, #0]
   3d610:	781b      	ldrb	r3, [r3, #0]
   3d612:	f806 3b01 	strb.w	r3, [r6], #1
   3d616:	f1ba 0f00 	cmp.w	sl, #0
   3d61a:	d011      	beq.n	3d640 <__ssvfiscanf_r+0x940>
   3d61c:	eba6 0308 	sub.w	r3, r6, r8
   3d620:	42ab      	cmp	r3, r5
   3d622:	d30d      	bcc.n	3d640 <__ssvfiscanf_r+0x940>
   3d624:	006d      	lsls	r5, r5, #1
   3d626:	4640      	mov	r0, r8
   3d628:	60fb      	str	r3, [r7, #12]
   3d62a:	4629      	mov	r1, r5
   3d62c:	f7fd f9c8 	bl	3a9c0 <realloc>
   3d630:	4680      	mov	r8, r0
   3d632:	2800      	cmp	r0, #0
   3d634:	f43f ac2b 	beq.w	3ce8e <__ssvfiscanf_r+0x18e>
   3d638:	68fb      	ldr	r3, [r7, #12]
   3d63a:	f8ca 0000 	str.w	r0, [sl]
   3d63e:	18c6      	adds	r6, r0, r3
   3d640:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d642:	3b01      	subs	r3, #1
   3d644:	62bb      	str	r3, [r7, #40]	; 0x28
   3d646:	d00c      	beq.n	3d662 <__ssvfiscanf_r+0x962>
   3d648:	6863      	ldr	r3, [r4, #4]
   3d64a:	2b00      	cmp	r3, #0
   3d64c:	dcd6      	bgt.n	3d5fc <__ssvfiscanf_r+0x8fc>
   3d64e:	4621      	mov	r1, r4
   3d650:	ee18 0a10 	vmov	r0, s16
   3d654:	f7ff faff 	bl	3cc56 <__ssrefill_r>
   3d658:	2800      	cmp	r0, #0
   3d65a:	d0cf      	beq.n	3d5fc <__ssvfiscanf_r+0x8fc>
   3d65c:	4546      	cmp	r6, r8
   3d65e:	f43f ac11 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d662:	ebb6 0808 	subs.w	r8, r6, r8
   3d666:	f43f aba2 	beq.w	3cdae <__ssvfiscanf_r+0xae>
   3d66a:	2300      	movs	r3, #0
   3d66c:	7033      	strb	r3, [r6, #0]
   3d66e:	f1ba 0f00 	cmp.w	sl, #0
   3d672:	d00a      	beq.n	3d68a <__ssvfiscanf_r+0x98a>
   3d674:	f108 0101 	add.w	r1, r8, #1
   3d678:	428d      	cmp	r5, r1
   3d67a:	d906      	bls.n	3d68a <__ssvfiscanf_r+0x98a>
   3d67c:	f8da 0000 	ldr.w	r0, [sl]
   3d680:	f7fd f99e 	bl	3a9c0 <realloc>
   3d684:	b108      	cbz	r0, 3d68a <__ssvfiscanf_r+0x98a>
   3d686:	f8ca 0000 	str.w	r0, [sl]
   3d68a:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3d68c:	3301      	adds	r3, #1
   3d68e:	627b      	str	r3, [r7, #36]	; 0x24
   3d690:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3d692:	4443      	add	r3, r8
   3d694:	62fb      	str	r3, [r7, #44]	; 0x2c
   3d696:	69be      	ldr	r6, [r7, #24]
   3d698:	f7ff bb4f 	b.w	3cd3a <__ssvfiscanf_r+0x3a>
   3d69c:	46aa      	mov	sl, r5
   3d69e:	e7aa      	b.n	3d5f6 <__ssvfiscanf_r+0x8f6>
   3d6a0:	20006430 	.word	0x20006430
   3d6a4:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d6a6:	f009 0810 	and.w	r8, r9, #16
   3d6aa:	2b00      	cmp	r3, #0
   3d6ac:	bf08      	it	eq
   3d6ae:	f04f 33ff 	moveq.w	r3, #4294967295	; 0xffffffff
   3d6b2:	f019 0501 	ands.w	r5, r9, #1
   3d6b6:	62bb      	str	r3, [r7, #40]	; 0x28
   3d6b8:	f000 80f6 	beq.w	3d8a8 <__ssvfiscanf_r+0xba8>
   3d6bc:	f1b8 0f00 	cmp.w	r8, #0
   3d6c0:	f040 809b 	bne.w	3d7fa <__ssvfiscanf_r+0xafa>
   3d6c4:	4633      	mov	r3, r6
   3d6c6:	f019 0f80 	tst.w	r9, #128	; 0x80
   3d6ca:	f853 5b04 	ldr.w	r5, [r3], #4
   3d6ce:	61bb      	str	r3, [r7, #24]
   3d6d0:	f000 809b 	beq.w	3d80a <__ssvfiscanf_r+0xb0a>
   3d6d4:	2d00      	cmp	r5, #0
   3d6d6:	f43f ac77 	beq.w	3cfc8 <__ssvfiscanf_r+0x2c8>
   3d6da:	2080      	movs	r0, #128	; 0x80
   3d6dc:	f7fc f8e8 	bl	398b0 <malloc>
   3d6e0:	4606      	mov	r6, r0
   3d6e2:	2800      	cmp	r0, #0
   3d6e4:	f43f ac6d 	beq.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d6e8:	f8bb 9006 	ldrh.w	r9, [fp, #6]
   3d6ec:	f8bb 3004 	ldrh.w	r3, [fp, #4]
   3d6f0:	6028      	str	r0, [r5, #0]
   3d6f2:	4599      	cmp	r9, r3
   3d6f4:	d314      	bcc.n	3d720 <__ssvfiscanf_r+0xa20>
   3d6f6:	f64f 72f6 	movw	r2, #65526	; 0xfff6
   3d6fa:	4293      	cmp	r3, r2
   3d6fc:	f63f ac61 	bhi.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d700:	3308      	adds	r3, #8
   3d702:	f8db 0000 	ldr.w	r0, [fp]
   3d706:	fa1f fa83 	uxth.w	sl, r3
   3d70a:	ea4f 018a 	mov.w	r1, sl, lsl #2
   3d70e:	f7fd f957 	bl	3a9c0 <realloc>
   3d712:	2800      	cmp	r0, #0
   3d714:	f43f ac55 	beq.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d718:	f8cb 0000 	str.w	r0, [fp]
   3d71c:	f8ab a004 	strh.w	sl, [fp, #4]
   3d720:	f8db 3000 	ldr.w	r3, [fp]
   3d724:	f109 0201 	add.w	r2, r9, #1
   3d728:	46aa      	mov	sl, r5
   3d72a:	f843 5029 	str.w	r5, [r3, r9, lsl #2]
   3d72e:	4635      	mov	r5, r6
   3d730:	f04f 0920 	mov.w	r9, #32
   3d734:	f8ab 2006 	strh.w	r2, [fp, #6]
   3d738:	2200      	movs	r2, #0
   3d73a:	6823      	ldr	r3, [r4, #0]
   3d73c:	49ac      	ldr	r1, [pc, #688]	; (3d9f0 <__ssvfiscanf_r+0xcf0>)
   3d73e:	781b      	ldrb	r3, [r3, #0]
   3d740:	5c5b      	ldrb	r3, [r3, r1]
   3d742:	0718      	lsls	r0, r3, #28
   3d744:	d43f      	bmi.n	3d7c6 <__ssvfiscanf_r+0xac6>
   3d746:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d748:	2b00      	cmp	r3, #0
   3d74a:	d03c      	beq.n	3d7c6 <__ssvfiscanf_r+0xac6>
   3d74c:	60fa      	str	r2, [r7, #12]
   3d74e:	f7fc f817 	bl	39780 <__locale_mb_cur_max>
   3d752:	68fa      	ldr	r2, [r7, #12]
   3d754:	4290      	cmp	r0, r2
   3d756:	f43f ab95 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d75a:	6821      	ldr	r1, [r4, #0]
   3d75c:	f107 0c48 	add.w	ip, r7, #72	; 0x48
   3d760:	1c53      	adds	r3, r2, #1
   3d762:	f811 0b01 	ldrb.w	r0, [r1], #1
   3d766:	f80c 0002 	strb.w	r0, [ip, r2]
   3d76a:	6862      	ldr	r2, [r4, #4]
   3d76c:	3a01      	subs	r2, #1
   3d76e:	e9c4 1200 	strd	r1, r2, [r4]
   3d772:	6a3a      	ldr	r2, [r7, #32]
   3d774:	2a03      	cmp	r2, #3
   3d776:	d102      	bne.n	3d77e <__ssvfiscanf_r+0xa7e>
   3d778:	6c3a      	ldr	r2, [r7, #64]	; 0x40
   3d77a:	2a04      	cmp	r2, #4
   3d77c:	d007      	beq.n	3d78e <__ssvfiscanf_r+0xa8e>
   3d77e:	2208      	movs	r2, #8
   3d780:	2100      	movs	r1, #0
   3d782:	f107 0040 	add.w	r0, r7, #64	; 0x40
   3d786:	623b      	str	r3, [r7, #32]
   3d788:	f7fc fc32 	bl	39ff0 <memset>
   3d78c:	6a3b      	ldr	r3, [r7, #32]
   3d78e:	f107 0240 	add.w	r2, r7, #64	; 0x40
   3d792:	4629      	mov	r1, r5
   3d794:	ee18 0a10 	vmov	r0, s16
   3d798:	60fb      	str	r3, [r7, #12]
   3d79a:	9200      	str	r2, [sp, #0]
   3d79c:	f107 0248 	add.w	r2, r7, #72	; 0x48
   3d7a0:	f7fc fabe 	bl	39d20 <_mbrtowc_r>
   3d7a4:	1c42      	adds	r2, r0, #1
   3d7a6:	6238      	str	r0, [r7, #32]
   3d7a8:	f43f ab6c 	beq.w	3ce84 <__ssvfiscanf_r+0x184>
   3d7ac:	68fb      	ldr	r3, [r7, #12]
   3d7ae:	bb80      	cbnz	r0, 3d812 <__ssvfiscanf_r+0xb12>
   3d7b0:	6028      	str	r0, [r5, #0]
   3d7b2:	6828      	ldr	r0, [r5, #0]
   3d7b4:	60fb      	str	r3, [r7, #12]
   3d7b6:	f7fb ffb5 	bl	39724 <iswspace>
   3d7ba:	68fb      	ldr	r3, [r7, #12]
   3d7bc:	4602      	mov	r2, r0
   3d7be:	2800      	cmp	r0, #0
   3d7c0:	d047      	beq.n	3d852 <__ssvfiscanf_r+0xb52>
   3d7c2:	2b00      	cmp	r3, #0
   3d7c4:	d139      	bne.n	3d83a <__ssvfiscanf_r+0xb3a>
   3d7c6:	f1b8 0f00 	cmp.w	r8, #0
   3d7ca:	f47f af64 	bne.w	3d696 <__ssvfiscanf_r+0x996>
   3d7ce:	f8c5 8000 	str.w	r8, [r5]
   3d7d2:	f1ba 0f00 	cmp.w	sl, #0
   3d7d6:	d00c      	beq.n	3d7f2 <__ssvfiscanf_r+0xaf2>
   3d7d8:	1ba9      	subs	r1, r5, r6
   3d7da:	108b      	asrs	r3, r1, #2
   3d7dc:	3301      	adds	r3, #1
   3d7de:	4599      	cmp	r9, r3
   3d7e0:	d907      	bls.n	3d7f2 <__ssvfiscanf_r+0xaf2>
   3d7e2:	3104      	adds	r1, #4
   3d7e4:	f8da 0000 	ldr.w	r0, [sl]
   3d7e8:	f7fd f8ea 	bl	3a9c0 <realloc>
   3d7ec:	b108      	cbz	r0, 3d7f2 <__ssvfiscanf_r+0xaf2>
   3d7ee:	f8ca 0000 	str.w	r0, [sl]
   3d7f2:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3d7f4:	3301      	adds	r3, #1
   3d7f6:	627b      	str	r3, [r7, #36]	; 0x24
   3d7f8:	e74d      	b.n	3d696 <__ssvfiscanf_r+0x996>
   3d7fa:	f04f 0900 	mov.w	r9, #0
   3d7fe:	61be      	str	r6, [r7, #24]
   3d800:	f107 0534 	add.w	r5, r7, #52	; 0x34
   3d804:	464e      	mov	r6, r9
   3d806:	46ca      	mov	sl, r9
   3d808:	e796      	b.n	3d738 <__ssvfiscanf_r+0xa38>
   3d80a:	46c1      	mov	r9, r8
   3d80c:	4646      	mov	r6, r8
   3d80e:	46c2      	mov	sl, r8
   3d810:	e792      	b.n	3d738 <__ssvfiscanf_r+0xa38>
   3d812:	6a3a      	ldr	r2, [r7, #32]
   3d814:	3202      	adds	r2, #2
   3d816:	d1cc      	bne.n	3d7b2 <__ssvfiscanf_r+0xab2>
   3d818:	461a      	mov	r2, r3
   3d81a:	6863      	ldr	r3, [r4, #4]
   3d81c:	2b00      	cmp	r3, #0
   3d81e:	dc8c      	bgt.n	3d73a <__ssvfiscanf_r+0xa3a>
   3d820:	4621      	mov	r1, r4
   3d822:	ee18 0a10 	vmov	r0, s16
   3d826:	60fa      	str	r2, [r7, #12]
   3d828:	f7ff fa15 	bl	3cc56 <__ssrefill_r>
   3d82c:	68fa      	ldr	r2, [r7, #12]
   3d82e:	2800      	cmp	r0, #0
   3d830:	d083      	beq.n	3d73a <__ssvfiscanf_r+0xa3a>
   3d832:	2a00      	cmp	r2, #0
   3d834:	f47f ab26 	bne.w	3ce84 <__ssvfiscanf_r+0x184>
   3d838:	e7c5      	b.n	3d7c6 <__ssvfiscanf_r+0xac6>
   3d83a:	3b01      	subs	r3, #1
   3d83c:	f107 0148 	add.w	r1, r7, #72	; 0x48
   3d840:	4622      	mov	r2, r4
   3d842:	ee18 0a10 	vmov	r0, s16
   3d846:	5cc9      	ldrb	r1, [r1, r3]
   3d848:	62bb      	str	r3, [r7, #40]	; 0x28
   3d84a:	f7ff f9c7 	bl	3cbdc <_sungetc_r>
   3d84e:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d850:	e7b7      	b.n	3d7c2 <__ssvfiscanf_r+0xac2>
   3d852:	6af9      	ldr	r1, [r7, #44]	; 0x2c
   3d854:	4419      	add	r1, r3
   3d856:	6a3b      	ldr	r3, [r7, #32]
   3d858:	2b03      	cmp	r3, #3
   3d85a:	62f9      	str	r1, [r7, #44]	; 0x2c
   3d85c:	d102      	bne.n	3d864 <__ssvfiscanf_r+0xb64>
   3d85e:	6c3b      	ldr	r3, [r7, #64]	; 0x40
   3d860:	2b04      	cmp	r3, #4
   3d862:	d002      	beq.n	3d86a <__ssvfiscanf_r+0xb6a>
   3d864:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d866:	3b01      	subs	r3, #1
   3d868:	62bb      	str	r3, [r7, #40]	; 0x28
   3d86a:	f1b8 0f00 	cmp.w	r8, #0
   3d86e:	d1d4      	bne.n	3d81a <__ssvfiscanf_r+0xb1a>
   3d870:	3504      	adds	r5, #4
   3d872:	f1ba 0f00 	cmp.w	sl, #0
   3d876:	d015      	beq.n	3d8a4 <__ssvfiscanf_r+0xba4>
   3d878:	1bab      	subs	r3, r5, r6
   3d87a:	ebb9 0fa3 	cmp.w	r9, r3, asr #2
   3d87e:	d80f      	bhi.n	3d8a0 <__ssvfiscanf_r+0xba0>
   3d880:	4630      	mov	r0, r6
   3d882:	ea4f 01c9 	mov.w	r1, r9, lsl #3
   3d886:	60fb      	str	r3, [r7, #12]
   3d888:	f7fd f89a 	bl	3a9c0 <realloc>
   3d88c:	4606      	mov	r6, r0
   3d88e:	2800      	cmp	r0, #0
   3d890:	f43f ab97 	beq.w	3cfc2 <__ssvfiscanf_r+0x2c2>
   3d894:	68fb      	ldr	r3, [r7, #12]
   3d896:	ea4f 0949 	mov.w	r9, r9, lsl #1
   3d89a:	f8ca 0000 	str.w	r0, [sl]
   3d89e:	18c5      	adds	r5, r0, r3
   3d8a0:	4642      	mov	r2, r8
   3d8a2:	e7ba      	b.n	3d81a <__ssvfiscanf_r+0xb1a>
   3d8a4:	4652      	mov	r2, sl
   3d8a6:	e7b8      	b.n	3d81a <__ssvfiscanf_r+0xb1a>
   3d8a8:	f1b8 0f00 	cmp.w	r8, #0
   3d8ac:	d01a      	beq.n	3d8e4 <__ssvfiscanf_r+0xbe4>
   3d8ae:	6823      	ldr	r3, [r4, #0]
   3d8b0:	494f      	ldr	r1, [pc, #316]	; (3d9f0 <__ssvfiscanf_r+0xcf0>)
   3d8b2:	781a      	ldrb	r2, [r3, #0]
   3d8b4:	5c52      	ldrb	r2, [r2, r1]
   3d8b6:	0711      	lsls	r1, r2, #28
   3d8b8:	f53f ae63 	bmi.w	3d582 <__ssvfiscanf_r+0x882>
   3d8bc:	3301      	adds	r3, #1
   3d8be:	6862      	ldr	r2, [r4, #4]
   3d8c0:	3501      	adds	r5, #1
   3d8c2:	6023      	str	r3, [r4, #0]
   3d8c4:	3a01      	subs	r2, #1
   3d8c6:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d8c8:	6062      	str	r2, [r4, #4]
   3d8ca:	429d      	cmp	r5, r3
   3d8cc:	f43f ae59 	beq.w	3d582 <__ssvfiscanf_r+0x882>
   3d8d0:	2a00      	cmp	r2, #0
   3d8d2:	dcec      	bgt.n	3d8ae <__ssvfiscanf_r+0xbae>
   3d8d4:	4621      	mov	r1, r4
   3d8d6:	ee18 0a10 	vmov	r0, s16
   3d8da:	f7ff f9bc 	bl	3cc56 <__ssrefill_r>
   3d8de:	2800      	cmp	r0, #0
   3d8e0:	d0e5      	beq.n	3d8ae <__ssvfiscanf_r+0xbae>
   3d8e2:	e64e      	b.n	3d582 <__ssvfiscanf_r+0x882>
   3d8e4:	f019 0980 	ands.w	r9, r9, #128	; 0x80
   3d8e8:	f856 5b04 	ldr.w	r5, [r6], #4
   3d8ec:	d07d      	beq.n	3d9ea <__ssvfiscanf_r+0xcea>
   3d8ee:	2d00      	cmp	r5, #0
   3d8f0:	f43f aa5d 	beq.w	3cdae <__ssvfiscanf_r+0xae>
   3d8f4:	2020      	movs	r0, #32
   3d8f6:	f7fb ffdb 	bl	398b0 <malloc>
   3d8fa:	4681      	mov	r9, r0
   3d8fc:	2800      	cmp	r0, #0
   3d8fe:	f43f aac6 	beq.w	3ce8e <__ssvfiscanf_r+0x18e>
   3d902:	f8bb a006 	ldrh.w	sl, [fp, #6]
   3d906:	f8bb 8004 	ldrh.w	r8, [fp, #4]
   3d90a:	6028      	str	r0, [r5, #0]
   3d90c:	45c2      	cmp	sl, r8
   3d90e:	d315      	bcc.n	3d93c <__ssvfiscanf_r+0xc3c>
   3d910:	f64f 73f6 	movw	r3, #65526	; 0xfff6
   3d914:	4598      	cmp	r8, r3
   3d916:	f63f ab5d 	bhi.w	3cfd4 <__ssvfiscanf_r+0x2d4>
   3d91a:	f108 0808 	add.w	r8, r8, #8
   3d91e:	f8db 0000 	ldr.w	r0, [fp]
   3d922:	fa1f f888 	uxth.w	r8, r8
   3d926:	ea4f 0188 	mov.w	r1, r8, lsl #2
   3d92a:	f7fd f849 	bl	3a9c0 <realloc>
   3d92e:	2800      	cmp	r0, #0
   3d930:	f43f ab50 	beq.w	3cfd4 <__ssvfiscanf_r+0x2d4>
   3d934:	f8cb 0000 	str.w	r0, [fp]
   3d938:	f8ab 8004 	strh.w	r8, [fp, #4]
   3d93c:	f8db 3000 	ldr.w	r3, [fp]
   3d940:	f10a 0201 	add.w	r2, sl, #1
   3d944:	f843 502a 	str.w	r5, [r3, sl, lsl #2]
   3d948:	46aa      	mov	sl, r5
   3d94a:	464d      	mov	r5, r9
   3d94c:	f04f 0920 	mov.w	r9, #32
   3d950:	f8ab 2006 	strh.w	r2, [fp, #6]
   3d954:	46a8      	mov	r8, r5
   3d956:	6823      	ldr	r3, [r4, #0]
   3d958:	4925      	ldr	r1, [pc, #148]	; (3d9f0 <__ssvfiscanf_r+0xcf0>)
   3d95a:	781a      	ldrb	r2, [r3, #0]
   3d95c:	5c52      	ldrb	r2, [r2, r1]
   3d95e:	0712      	lsls	r2, r2, #28
   3d960:	d42b      	bmi.n	3d9ba <__ssvfiscanf_r+0xcba>
   3d962:	6862      	ldr	r2, [r4, #4]
   3d964:	3a01      	subs	r2, #1
   3d966:	6062      	str	r2, [r4, #4]
   3d968:	1c5a      	adds	r2, r3, #1
   3d96a:	6022      	str	r2, [r4, #0]
   3d96c:	781b      	ldrb	r3, [r3, #0]
   3d96e:	f805 3b01 	strb.w	r3, [r5], #1
   3d972:	f1ba 0f00 	cmp.w	sl, #0
   3d976:	d012      	beq.n	3d99e <__ssvfiscanf_r+0xc9e>
   3d978:	eba5 0308 	sub.w	r3, r5, r8
   3d97c:	454b      	cmp	r3, r9
   3d97e:	d30e      	bcc.n	3d99e <__ssvfiscanf_r+0xc9e>
   3d980:	ea4f 0949 	mov.w	r9, r9, lsl #1
   3d984:	4640      	mov	r0, r8
   3d986:	61bb      	str	r3, [r7, #24]
   3d988:	4649      	mov	r1, r9
   3d98a:	f7fd f819 	bl	3a9c0 <realloc>
   3d98e:	4680      	mov	r8, r0
   3d990:	2800      	cmp	r0, #0
   3d992:	f43f aa7c 	beq.w	3ce8e <__ssvfiscanf_r+0x18e>
   3d996:	69bb      	ldr	r3, [r7, #24]
   3d998:	f8ca 0000 	str.w	r0, [sl]
   3d99c:	18c5      	adds	r5, r0, r3
   3d99e:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d9a0:	3b01      	subs	r3, #1
   3d9a2:	62bb      	str	r3, [r7, #40]	; 0x28
   3d9a4:	d009      	beq.n	3d9ba <__ssvfiscanf_r+0xcba>
   3d9a6:	6863      	ldr	r3, [r4, #4]
   3d9a8:	2b00      	cmp	r3, #0
   3d9aa:	dcd4      	bgt.n	3d956 <__ssvfiscanf_r+0xc56>
   3d9ac:	4621      	mov	r1, r4
   3d9ae:	ee18 0a10 	vmov	r0, s16
   3d9b2:	f7ff f950 	bl	3cc56 <__ssrefill_r>
   3d9b6:	2800      	cmp	r0, #0
   3d9b8:	d0cd      	beq.n	3d956 <__ssvfiscanf_r+0xc56>
   3d9ba:	2300      	movs	r3, #0
   3d9bc:	702b      	strb	r3, [r5, #0]
   3d9be:	eba5 0508 	sub.w	r5, r5, r8
   3d9c2:	f1ba 0f00 	cmp.w	sl, #0
   3d9c6:	d009      	beq.n	3d9dc <__ssvfiscanf_r+0xcdc>
   3d9c8:	1c69      	adds	r1, r5, #1
   3d9ca:	4589      	cmp	r9, r1
   3d9cc:	d906      	bls.n	3d9dc <__ssvfiscanf_r+0xcdc>
   3d9ce:	f8da 0000 	ldr.w	r0, [sl]
   3d9d2:	f7fc fff5 	bl	3a9c0 <realloc>
   3d9d6:	b108      	cbz	r0, 3d9dc <__ssvfiscanf_r+0xcdc>
   3d9d8:	f8ca 0000 	str.w	r0, [sl]
   3d9dc:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3d9de:	442b      	add	r3, r5
   3d9e0:	62fb      	str	r3, [r7, #44]	; 0x2c
   3d9e2:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3d9e4:	3301      	adds	r3, #1
   3d9e6:	f7ff b9a7 	b.w	3cd38 <__ssvfiscanf_r+0x38>
   3d9ea:	46ca      	mov	sl, r9
   3d9ec:	e7b2      	b.n	3d954 <__ssvfiscanf_r+0xc54>
   3d9ee:	bf00      	nop
   3d9f0:	0004056b 	.word	0x0004056b
   3d9f4:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3d9f6:	f449 6958 	orr.w	r9, r9, #3456	; 0xd80
   3d9fa:	f04f 0a00 	mov.w	sl, #0
   3d9fe:	f107 0848 	add.w	r8, r7, #72	; 0x48
   3da02:	3b01      	subs	r3, #1
   3da04:	2b26      	cmp	r3, #38	; 0x26
   3da06:	bf89      	itett	hi
   3da08:	6abb      	ldrhi	r3, [r7, #40]	; 0x28
   3da0a:	2500      	movls	r5, #0
   3da0c:	f1a3 0527 	subhi.w	r5, r3, #39	; 0x27
   3da10:	2327      	movhi	r3, #39	; 0x27
   3da12:	bf88      	it	hi
   3da14:	62bb      	strhi	r3, [r7, #40]	; 0x28
   3da16:	6822      	ldr	r2, [r4, #0]
   3da18:	7813      	ldrb	r3, [r2, #0]
   3da1a:	2b39      	cmp	r3, #57	; 0x39
   3da1c:	d80f      	bhi.n	3da3e <__ssvfiscanf_r+0xd3e>
   3da1e:	2b2a      	cmp	r3, #42	; 0x2a
   3da20:	d91a      	bls.n	3da58 <__ssvfiscanf_r+0xd58>
   3da22:	f1a3 012b 	sub.w	r1, r3, #43	; 0x2b
   3da26:	290e      	cmp	r1, #14
   3da28:	d816      	bhi.n	3da58 <__ssvfiscanf_r+0xd58>
   3da2a:	e8df f001 	tbb	[pc, r1]
   3da2e:	1577      	.short	0x1577
   3da30:	41151577 	.word	0x41151577
   3da34:	67676767 	.word	0x67676767
   3da38:	6f676767 	.word	0x6f676767
   3da3c:	6f          	.byte	0x6f
   3da3d:	00          	.byte	0x00
   3da3e:	2b66      	cmp	r3, #102	; 0x66
   3da40:	d834      	bhi.n	3daac <__ssvfiscanf_r+0xdac>
   3da42:	2b60      	cmp	r3, #96	; 0x60
   3da44:	d803      	bhi.n	3da4e <__ssvfiscanf_r+0xd4e>
   3da46:	2b46      	cmp	r3, #70	; 0x46
   3da48:	d804      	bhi.n	3da54 <__ssvfiscanf_r+0xd54>
   3da4a:	2b40      	cmp	r3, #64	; 0x40
   3da4c:	d904      	bls.n	3da58 <__ssvfiscanf_r+0xd58>
   3da4e:	69f9      	ldr	r1, [r7, #28]
   3da50:	290a      	cmp	r1, #10
   3da52:	e061      	b.n	3db18 <__ssvfiscanf_r+0xe18>
   3da54:	2b58      	cmp	r3, #88	; 0x58
   3da56:	d067      	beq.n	3db28 <__ssvfiscanf_r+0xe28>
   3da58:	f419 7f80 	tst.w	r9, #256	; 0x100
   3da5c:	d012      	beq.n	3da84 <__ssvfiscanf_r+0xd84>
   3da5e:	f107 0348 	add.w	r3, r7, #72	; 0x48
   3da62:	4598      	cmp	r8, r3
   3da64:	d909      	bls.n	3da7a <__ssvfiscanf_r+0xd7a>
   3da66:	f108 35ff 	add.w	r5, r8, #4294967295	; 0xffffffff
   3da6a:	f818 1c01 	ldrb.w	r1, [r8, #-1]
   3da6e:	4622      	mov	r2, r4
   3da70:	ee18 0a10 	vmov	r0, s16
   3da74:	46a8      	mov	r8, r5
   3da76:	f7ff f8b1 	bl	3cbdc <_sungetc_r>
   3da7a:	f107 0348 	add.w	r3, r7, #72	; 0x48
   3da7e:	4598      	cmp	r8, r3
   3da80:	f43f a995 	beq.w	3cdae <__ssvfiscanf_r+0xae>
   3da84:	f019 0210 	ands.w	r2, r9, #16
   3da88:	d16b      	bne.n	3db62 <__ssvfiscanf_r+0xe62>
   3da8a:	697d      	ldr	r5, [r7, #20]
   3da8c:	f107 0148 	add.w	r1, r7, #72	; 0x48
   3da90:	69fb      	ldr	r3, [r7, #28]
   3da92:	ee18 0a10 	vmov	r0, s16
   3da96:	f888 2000 	strb.w	r2, [r8]
   3da9a:	47a8      	blx	r5
   3da9c:	f019 0f20 	tst.w	r9, #32
   3daa0:	f106 0504 	add.w	r5, r6, #4
   3daa4:	d054      	beq.n	3db50 <__ssvfiscanf_r+0xe50>
   3daa6:	6833      	ldr	r3, [r6, #0]
   3daa8:	6018      	str	r0, [r3, #0]
   3daaa:	e056      	b.n	3db5a <__ssvfiscanf_r+0xe5a>
   3daac:	2b78      	cmp	r3, #120	; 0x78
   3daae:	e7d2      	b.n	3da56 <__ssvfiscanf_r+0xd56>
   3dab0:	f419 6f00 	tst.w	r9, #2048	; 0x800
   3dab4:	d00a      	beq.n	3dacc <__ssvfiscanf_r+0xdcc>
   3dab6:	69f9      	ldr	r1, [r7, #28]
   3dab8:	b919      	cbnz	r1, 3dac2 <__ssvfiscanf_r+0xdc2>
   3daba:	2108      	movs	r1, #8
   3dabc:	f449 7900 	orr.w	r9, r9, #512	; 0x200
   3dac0:	61f9      	str	r1, [r7, #28]
   3dac2:	f419 6f80 	tst.w	r9, #1024	; 0x400
   3dac6:	d004      	beq.n	3dad2 <__ssvfiscanf_r+0xdd2>
   3dac8:	f429 69b0 	bic.w	r9, r9, #1408	; 0x580
   3dacc:	f808 3b01 	strb.w	r3, [r8], #1
   3dad0:	e008      	b.n	3dae4 <__ssvfiscanf_r+0xde4>
   3dad2:	f429 7960 	bic.w	r9, r9, #896	; 0x380
   3dad6:	b11d      	cbz	r5, 3dae0 <__ssvfiscanf_r+0xde0>
   3dad8:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3dada:	3d01      	subs	r5, #1
   3dadc:	3301      	adds	r3, #1
   3dade:	62bb      	str	r3, [r7, #40]	; 0x28
   3dae0:	f10a 0a01 	add.w	sl, sl, #1
   3dae4:	6863      	ldr	r3, [r4, #4]
   3dae6:	3b01      	subs	r3, #1
   3dae8:	2b00      	cmp	r3, #0
   3daea:	6063      	str	r3, [r4, #4]
   3daec:	dd28      	ble.n	3db40 <__ssvfiscanf_r+0xe40>
   3daee:	3201      	adds	r2, #1
   3daf0:	6022      	str	r2, [r4, #0]
   3daf2:	6abb      	ldr	r3, [r7, #40]	; 0x28
   3daf4:	3b01      	subs	r3, #1
   3daf6:	62bb      	str	r3, [r7, #40]	; 0x28
   3daf8:	d18d      	bne.n	3da16 <__ssvfiscanf_r+0xd16>
   3dafa:	e7ad      	b.n	3da58 <__ssvfiscanf_r+0xd58>
   3dafc:	69f8      	ldr	r0, [r7, #28]
   3dafe:	4932      	ldr	r1, [pc, #200]	; (3dbc8 <__ssvfiscanf_r+0xec8>)
   3db00:	f931 1010 	ldrsh.w	r1, [r1, r0, lsl #1]
   3db04:	61f9      	str	r1, [r7, #28]
   3db06:	f429 6938 	bic.w	r9, r9, #2944	; 0xb80
   3db0a:	e7df      	b.n	3dacc <__ssvfiscanf_r+0xdcc>
   3db0c:	69f8      	ldr	r0, [r7, #28]
   3db0e:	492e      	ldr	r1, [pc, #184]	; (3dbc8 <__ssvfiscanf_r+0xec8>)
   3db10:	f931 1010 	ldrsh.w	r1, [r1, r0, lsl #1]
   3db14:	2908      	cmp	r1, #8
   3db16:	61f9      	str	r1, [r7, #28]
   3db18:	dcf5      	bgt.n	3db06 <__ssvfiscanf_r+0xe06>
   3db1a:	e79d      	b.n	3da58 <__ssvfiscanf_r+0xd58>
   3db1c:	f019 0f80 	tst.w	r9, #128	; 0x80
   3db20:	d09a      	beq.n	3da58 <__ssvfiscanf_r+0xd58>
   3db22:	f029 0980 	bic.w	r9, r9, #128	; 0x80
   3db26:	e7d1      	b.n	3dacc <__ssvfiscanf_r+0xdcc>
   3db28:	f409 61c0 	and.w	r1, r9, #1536	; 0x600
   3db2c:	f5b1 7f00 	cmp.w	r1, #512	; 0x200
   3db30:	d192      	bne.n	3da58 <__ssvfiscanf_r+0xd58>
   3db32:	f429 7900 	bic.w	r9, r9, #512	; 0x200
   3db36:	2110      	movs	r1, #16
   3db38:	f449 69a0 	orr.w	r9, r9, #1280	; 0x500
   3db3c:	61f9      	str	r1, [r7, #28]
   3db3e:	e7c5      	b.n	3dacc <__ssvfiscanf_r+0xdcc>
   3db40:	4621      	mov	r1, r4
   3db42:	ee18 0a10 	vmov	r0, s16
   3db46:	f7ff f886 	bl	3cc56 <__ssrefill_r>
   3db4a:	2800      	cmp	r0, #0
   3db4c:	d0d1      	beq.n	3daf2 <__ssvfiscanf_r+0xdf2>
   3db4e:	e783      	b.n	3da58 <__ssvfiscanf_r+0xd58>
   3db50:	f019 0f08 	tst.w	r9, #8
   3db54:	d00e      	beq.n	3db74 <__ssvfiscanf_r+0xe74>
   3db56:	6833      	ldr	r3, [r6, #0]
   3db58:	7018      	strb	r0, [r3, #0]
   3db5a:	6a7b      	ldr	r3, [r7, #36]	; 0x24
   3db5c:	462e      	mov	r6, r5
   3db5e:	3301      	adds	r3, #1
   3db60:	627b      	str	r3, [r7, #36]	; 0x24
   3db62:	f107 0348 	add.w	r3, r7, #72	; 0x48
   3db66:	eba8 0803 	sub.w	r8, r8, r3
   3db6a:	6afb      	ldr	r3, [r7, #44]	; 0x2c
   3db6c:	44c2      	add	sl, r8
   3db6e:	4453      	add	r3, sl
   3db70:	f7ff b97f 	b.w	3ce72 <__ssvfiscanf_r+0x172>
   3db74:	f019 0f04 	tst.w	r9, #4
   3db78:	d002      	beq.n	3db80 <__ssvfiscanf_r+0xe80>
   3db7a:	6833      	ldr	r3, [r6, #0]
   3db7c:	8018      	strh	r0, [r3, #0]
   3db7e:	e7ec      	b.n	3db5a <__ssvfiscanf_r+0xe5a>
   3db80:	f019 0201 	ands.w	r2, r9, #1
   3db84:	d18f      	bne.n	3daa6 <__ssvfiscanf_r+0xda6>
   3db86:	f019 0f02 	tst.w	r9, #2
   3db8a:	d08c      	beq.n	3daa6 <__ssvfiscanf_r+0xda6>
   3db8c:	4b0f      	ldr	r3, [pc, #60]	; (3dbcc <__ssvfiscanf_r+0xecc>)
   3db8e:	ee18 0a10 	vmov	r0, s16
   3db92:	6979      	ldr	r1, [r7, #20]
   3db94:	4299      	cmp	r1, r3
   3db96:	69fb      	ldr	r3, [r7, #28]
   3db98:	f107 0148 	add.w	r1, r7, #72	; 0x48
   3db9c:	d105      	bne.n	3dbaa <__ssvfiscanf_r+0xeaa>
   3db9e:	f7fe fab5 	bl	3c10c <_strtoull_r>
   3dba2:	6833      	ldr	r3, [r6, #0]
   3dba4:	e9c3 0100 	strd	r0, r1, [r3]
   3dba8:	e7d7      	b.n	3db5a <__ssvfiscanf_r+0xe5a>
   3dbaa:	f7fe fa03 	bl	3bfb4 <_strtoll_r>
   3dbae:	e7f8      	b.n	3dba2 <__ssvfiscanf_r+0xea2>
   3dbb0:	f855 3024 	ldr.w	r3, [r5, r4, lsl #2]
   3dbb4:	6818      	ldr	r0, [r3, #0]
   3dbb6:	f7fb fb17 	bl	391e8 <free>
   3dbba:	f855 3024 	ldr.w	r3, [r5, r4, lsl #2]
   3dbbe:	3401      	adds	r4, #1
   3dbc0:	601e      	str	r6, [r3, #0]
   3dbc2:	f7ff ba09 	b.w	3cfd8 <__ssvfiscanf_r+0x2d8>
   3dbc6:	bf00      	nop
   3dbc8:	00040920 	.word	0x00040920
   3dbcc:	0003c215 	.word	0x0003c215

0003dbd0 <__submore>:
   3dbd0:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
   3dbd4:	460c      	mov	r4, r1
   3dbd6:	6b09      	ldr	r1, [r1, #48]	; 0x30
   3dbd8:	f104 0340 	add.w	r3, r4, #64	; 0x40
   3dbdc:	4299      	cmp	r1, r3
   3dbde:	d11a      	bne.n	3dc16 <__submore+0x46>
   3dbe0:	f44f 6180 	mov.w	r1, #1024	; 0x400
   3dbe4:	f7fb fe6c 	bl	398c0 <_malloc_r>
   3dbe8:	b918      	cbnz	r0, 3dbf2 <__submore+0x22>
   3dbea:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3dbee:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
   3dbf2:	f44f 6380 	mov.w	r3, #1024	; 0x400
   3dbf6:	6320      	str	r0, [r4, #48]	; 0x30
   3dbf8:	f200 30fd 	addw	r0, r0, #1021	; 0x3fd
   3dbfc:	6363      	str	r3, [r4, #52]	; 0x34
   3dbfe:	f894 3042 	ldrb.w	r3, [r4, #66]	; 0x42
   3dc02:	7083      	strb	r3, [r0, #2]
   3dc04:	f894 3041 	ldrb.w	r3, [r4, #65]	; 0x41
   3dc08:	7043      	strb	r3, [r0, #1]
   3dc0a:	f894 3040 	ldrb.w	r3, [r4, #64]	; 0x40
   3dc0e:	7003      	strb	r3, [r0, #0]
   3dc10:	6020      	str	r0, [r4, #0]
   3dc12:	2000      	movs	r0, #0
   3dc14:	e7eb      	b.n	3dbee <__submore+0x1e>
   3dc16:	6b66      	ldr	r6, [r4, #52]	; 0x34
   3dc18:	0077      	lsls	r7, r6, #1
   3dc1a:	463a      	mov	r2, r7
   3dc1c:	f7fc fed8 	bl	3a9d0 <_realloc_r>
   3dc20:	4605      	mov	r5, r0
   3dc22:	2800      	cmp	r0, #0
   3dc24:	d0e1      	beq.n	3dbea <__submore+0x1a>
   3dc26:	eb00 0806 	add.w	r8, r0, r6
   3dc2a:	4601      	mov	r1, r0
   3dc2c:	4632      	mov	r2, r6
   3dc2e:	4640      	mov	r0, r8
   3dc30:	f7fc f912 	bl	39e58 <memcpy>
   3dc34:	f8c4 8000 	str.w	r8, [r4]
   3dc38:	e9c4 570c 	strd	r5, r7, [r4, #48]	; 0x30
   3dc3c:	e7e9      	b.n	3dc12 <__submore+0x42>

0003dc3e <_ungetc_r>:
   3dc3e:	b570      	push	{r4, r5, r6, lr}
   3dc40:	460d      	mov	r5, r1
   3dc42:	4606      	mov	r6, r0
   3dc44:	4614      	mov	r4, r2
   3dc46:	1c69      	adds	r1, r5, #1
   3dc48:	d103      	bne.n	3dc52 <_ungetc_r+0x14>
   3dc4a:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
   3dc4e:	4628      	mov	r0, r5
   3dc50:	bd70      	pop	{r4, r5, r6, pc}
   3dc52:	b118      	cbz	r0, 3dc5c <_ungetc_r+0x1e>
   3dc54:	6b83      	ldr	r3, [r0, #56]	; 0x38
   3dc56:	b90b      	cbnz	r3, 3dc5c <_ungetc_r+0x1e>
   3dc58:	f7fb fa3c 	bl	390d4 <__sinit>
   3dc5c:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3dc5e:	07da      	lsls	r2, r3, #31
   3dc60:	d405      	bmi.n	3dc6e <_ungetc_r+0x30>
   3dc62:	89a3      	ldrh	r3, [r4, #12]
   3dc64:	059b      	lsls	r3, r3, #22
   3dc66:	d402      	bmi.n	3dc6e <_ungetc_r+0x30>
   3dc68:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3dc6a:	f7fb fd9c 	bl	397a6 <__retarget_lock_acquire_recursive>
   3dc6e:	f9b4 300c 	ldrsh.w	r3, [r4, #12]
   3dc72:	0498      	lsls	r0, r3, #18
   3dc74:	d406      	bmi.n	3dc84 <_ungetc_r+0x46>
   3dc76:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   3dc7a:	81a3      	strh	r3, [r4, #12]
   3dc7c:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3dc7e:	f423 5300 	bic.w	r3, r3, #8192	; 0x2000
   3dc82:	6663      	str	r3, [r4, #100]	; 0x64
   3dc84:	f9b4 300c 	ldrsh.w	r3, [r4, #12]
   3dc88:	f023 0320 	bic.w	r3, r3, #32
   3dc8c:	0759      	lsls	r1, r3, #29
   3dc8e:	b29a      	uxth	r2, r3
   3dc90:	81a3      	strh	r3, [r4, #12]
   3dc92:	d423      	bmi.n	3dcdc <_ungetc_r+0x9e>
   3dc94:	06d3      	lsls	r3, r2, #27
   3dc96:	d409      	bmi.n	3dcac <_ungetc_r+0x6e>
   3dc98:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3dc9a:	07dd      	lsls	r5, r3, #31
   3dc9c:	d4d5      	bmi.n	3dc4a <_ungetc_r+0xc>
   3dc9e:	f412 7f00 	tst.w	r2, #512	; 0x200
   3dca2:	d1d2      	bne.n	3dc4a <_ungetc_r+0xc>
   3dca4:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3dca6:	f7fb fd84 	bl	397b2 <__retarget_lock_release_recursive>
   3dcaa:	e7ce      	b.n	3dc4a <_ungetc_r+0xc>
   3dcac:	0710      	lsls	r0, r2, #28
   3dcae:	d511      	bpl.n	3dcd4 <_ungetc_r+0x96>
   3dcb0:	4621      	mov	r1, r4
   3dcb2:	4630      	mov	r0, r6
   3dcb4:	f7fb f95e 	bl	38f74 <_fflush_r>
   3dcb8:	b130      	cbz	r0, 3dcc8 <_ungetc_r+0x8a>
   3dcba:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3dcbc:	07d9      	lsls	r1, r3, #31
   3dcbe:	d4c4      	bmi.n	3dc4a <_ungetc_r+0xc>
   3dcc0:	89a3      	ldrh	r3, [r4, #12]
   3dcc2:	f413 7f00 	tst.w	r3, #512	; 0x200
   3dcc6:	e7ec      	b.n	3dca2 <_ungetc_r+0x64>
   3dcc8:	89a3      	ldrh	r3, [r4, #12]
   3dcca:	60a0      	str	r0, [r4, #8]
   3dccc:	f023 0308 	bic.w	r3, r3, #8
   3dcd0:	61a0      	str	r0, [r4, #24]
   3dcd2:	81a3      	strh	r3, [r4, #12]
   3dcd4:	89a3      	ldrh	r3, [r4, #12]
   3dcd6:	f043 0304 	orr.w	r3, r3, #4
   3dcda:	81a3      	strh	r3, [r4, #12]
   3dcdc:	6b23      	ldr	r3, [r4, #48]	; 0x30
   3dcde:	b2ed      	uxtb	r5, r5
   3dce0:	6862      	ldr	r2, [r4, #4]
   3dce2:	b1d3      	cbz	r3, 3dd1a <_ungetc_r+0xdc>
   3dce4:	6b63      	ldr	r3, [r4, #52]	; 0x34
   3dce6:	4293      	cmp	r3, r2
   3dce8:	dc05      	bgt.n	3dcf6 <_ungetc_r+0xb8>
   3dcea:	4621      	mov	r1, r4
   3dcec:	4630      	mov	r0, r6
   3dcee:	f7ff ff6f 	bl	3dbd0 <__submore>
   3dcf2:	2800      	cmp	r0, #0
   3dcf4:	d1e1      	bne.n	3dcba <_ungetc_r+0x7c>
   3dcf6:	6823      	ldr	r3, [r4, #0]
   3dcf8:	1e5a      	subs	r2, r3, #1
   3dcfa:	6022      	str	r2, [r4, #0]
   3dcfc:	f803 5c01 	strb.w	r5, [r3, #-1]
   3dd00:	6863      	ldr	r3, [r4, #4]
   3dd02:	3301      	adds	r3, #1
   3dd04:	6063      	str	r3, [r4, #4]
   3dd06:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3dd08:	07da      	lsls	r2, r3, #31
   3dd0a:	d4a0      	bmi.n	3dc4e <_ungetc_r+0x10>
   3dd0c:	89a3      	ldrh	r3, [r4, #12]
   3dd0e:	059b      	lsls	r3, r3, #22
   3dd10:	d49d      	bmi.n	3dc4e <_ungetc_r+0x10>
   3dd12:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3dd14:	f7fb fd4d 	bl	397b2 <__retarget_lock_release_recursive>
   3dd18:	e799      	b.n	3dc4e <_ungetc_r+0x10>
   3dd1a:	6e61      	ldr	r1, [r4, #100]	; 0x64
   3dd1c:	6920      	ldr	r0, [r4, #16]
   3dd1e:	6823      	ldr	r3, [r4, #0]
   3dd20:	f001 0101 	and.w	r1, r1, #1
   3dd24:	b160      	cbz	r0, 3dd40 <_ungetc_r+0x102>
   3dd26:	4298      	cmp	r0, r3
   3dd28:	d20a      	bcs.n	3dd40 <_ungetc_r+0x102>
   3dd2a:	f813 0c01 	ldrb.w	r0, [r3, #-1]
   3dd2e:	4285      	cmp	r5, r0
   3dd30:	d106      	bne.n	3dd40 <_ungetc_r+0x102>
   3dd32:	3b01      	subs	r3, #1
   3dd34:	3201      	adds	r2, #1
   3dd36:	6023      	str	r3, [r4, #0]
   3dd38:	6062      	str	r2, [r4, #4]
   3dd3a:	2900      	cmp	r1, #0
   3dd3c:	d187      	bne.n	3dc4e <_ungetc_r+0x10>
   3dd3e:	e7e5      	b.n	3dd0c <_ungetc_r+0xce>
   3dd40:	e9c4 320e 	strd	r3, r2, [r4, #56]	; 0x38
   3dd44:	f104 0340 	add.w	r3, r4, #64	; 0x40
   3dd48:	6323      	str	r3, [r4, #48]	; 0x30
   3dd4a:	2303      	movs	r3, #3
   3dd4c:	6363      	str	r3, [r4, #52]	; 0x34
   3dd4e:	4623      	mov	r3, r4
   3dd50:	f803 5f42 	strb.w	r5, [r3, #66]!
   3dd54:	6023      	str	r3, [r4, #0]
   3dd56:	2301      	movs	r3, #1
   3dd58:	6063      	str	r3, [r4, #4]
   3dd5a:	e7ee      	b.n	3dd3a <_ungetc_r+0xfc>

0003dd5c <ungetc>:
   3dd5c:	4b02      	ldr	r3, [pc, #8]	; (3dd68 <ungetc+0xc>)
   3dd5e:	460a      	mov	r2, r1
   3dd60:	4601      	mov	r1, r0
   3dd62:	6818      	ldr	r0, [r3, #0]
   3dd64:	f7ff bf6b 	b.w	3dc3e <_ungetc_r>
   3dd68:	20006000 	.word	0x20006000

0003dd6c <__sprint_r>:
   3dd6c:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3dd70:	6893      	ldr	r3, [r2, #8]
   3dd72:	4680      	mov	r8, r0
   3dd74:	460f      	mov	r7, r1
   3dd76:	4614      	mov	r4, r2
   3dd78:	b91b      	cbnz	r3, 3dd82 <__sprint_r+0x16>
   3dd7a:	4618      	mov	r0, r3
   3dd7c:	6053      	str	r3, [r2, #4]
   3dd7e:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3dd82:	6e4b      	ldr	r3, [r1, #100]	; 0x64
   3dd84:	049d      	lsls	r5, r3, #18
   3dd86:	d520      	bpl.n	3ddca <__sprint_r+0x5e>
   3dd88:	6815      	ldr	r5, [r2, #0]
   3dd8a:	3508      	adds	r5, #8
   3dd8c:	f04f 0900 	mov.w	r9, #0
   3dd90:	e955 b602 	ldrd	fp, r6, [r5, #-8]
   3dd94:	ea4f 0a96 	mov.w	sl, r6, lsr #2
   3dd98:	45ca      	cmp	sl, r9
   3dd9a:	dc0b      	bgt.n	3ddb4 <__sprint_r+0x48>
   3dd9c:	68a3      	ldr	r3, [r4, #8]
   3dd9e:	f026 0003 	bic.w	r0, r6, #3
   3dda2:	3508      	adds	r5, #8
   3dda4:	1a18      	subs	r0, r3, r0
   3dda6:	60a0      	str	r0, [r4, #8]
   3dda8:	2800      	cmp	r0, #0
   3ddaa:	d1ef      	bne.n	3dd8c <__sprint_r+0x20>
   3ddac:	2300      	movs	r3, #0
   3ddae:	e9c4 3301 	strd	r3, r3, [r4, #4]
   3ddb2:	e7e4      	b.n	3dd7e <__sprint_r+0x12>
   3ddb4:	463a      	mov	r2, r7
   3ddb6:	f85b 1029 	ldr.w	r1, [fp, r9, lsl #2]
   3ddba:	4640      	mov	r0, r8
   3ddbc:	f000 fe19 	bl	3e9f2 <_fputwc_r>
   3ddc0:	1c43      	adds	r3, r0, #1
   3ddc2:	d0f3      	beq.n	3ddac <__sprint_r+0x40>
   3ddc4:	f109 0901 	add.w	r9, r9, #1
   3ddc8:	e7e6      	b.n	3dd98 <__sprint_r+0x2c>
   3ddca:	f7fb fb25 	bl	39418 <__sfvwrite_r>
   3ddce:	e7ed      	b.n	3ddac <__sprint_r+0x40>

0003ddd0 <_vfiprintf_r>:
   3ddd0:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3ddd4:	ed2d 8b02 	vpush	{d8}
   3ddd8:	b0b9      	sub	sp, #228	; 0xe4
   3ddda:	460f      	mov	r7, r1
   3dddc:	461d      	mov	r5, r3
   3ddde:	461c      	mov	r4, r3
   3dde0:	4681      	mov	r9, r0
   3dde2:	9201      	str	r2, [sp, #4]
   3dde4:	b118      	cbz	r0, 3ddee <_vfiprintf_r+0x1e>
   3dde6:	6b83      	ldr	r3, [r0, #56]	; 0x38
   3dde8:	b90b      	cbnz	r3, 3ddee <_vfiprintf_r+0x1e>
   3ddea:	f7fb f973 	bl	390d4 <__sinit>
   3ddee:	6e7b      	ldr	r3, [r7, #100]	; 0x64
   3ddf0:	07d8      	lsls	r0, r3, #31
   3ddf2:	d405      	bmi.n	3de00 <_vfiprintf_r+0x30>
   3ddf4:	89bb      	ldrh	r3, [r7, #12]
   3ddf6:	0599      	lsls	r1, r3, #22
   3ddf8:	d402      	bmi.n	3de00 <_vfiprintf_r+0x30>
   3ddfa:	6db8      	ldr	r0, [r7, #88]	; 0x58
   3ddfc:	f7fb fcd3 	bl	397a6 <__retarget_lock_acquire_recursive>
   3de00:	f9b7 300c 	ldrsh.w	r3, [r7, #12]
   3de04:	049a      	lsls	r2, r3, #18
   3de06:	d406      	bmi.n	3de16 <_vfiprintf_r+0x46>
   3de08:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   3de0c:	81bb      	strh	r3, [r7, #12]
   3de0e:	6e7b      	ldr	r3, [r7, #100]	; 0x64
   3de10:	f423 5300 	bic.w	r3, r3, #8192	; 0x2000
   3de14:	667b      	str	r3, [r7, #100]	; 0x64
   3de16:	89bb      	ldrh	r3, [r7, #12]
   3de18:	071e      	lsls	r6, r3, #28
   3de1a:	d501      	bpl.n	3de20 <_vfiprintf_r+0x50>
   3de1c:	693b      	ldr	r3, [r7, #16]
   3de1e:	b9bb      	cbnz	r3, 3de50 <_vfiprintf_r+0x80>
   3de20:	4639      	mov	r1, r7
   3de22:	4648      	mov	r0, r9
   3de24:	f7fa f94a 	bl	380bc <__swsetup_r>
   3de28:	b190      	cbz	r0, 3de50 <_vfiprintf_r+0x80>
   3de2a:	6e7b      	ldr	r3, [r7, #100]	; 0x64
   3de2c:	07d8      	lsls	r0, r3, #31
   3de2e:	d508      	bpl.n	3de42 <_vfiprintf_r+0x72>
   3de30:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
   3de34:	9302      	str	r3, [sp, #8]
   3de36:	9802      	ldr	r0, [sp, #8]
   3de38:	b039      	add	sp, #228	; 0xe4
   3de3a:	ecbd 8b02 	vpop	{d8}
   3de3e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3de42:	89bb      	ldrh	r3, [r7, #12]
   3de44:	0599      	lsls	r1, r3, #22
   3de46:	d4f3      	bmi.n	3de30 <_vfiprintf_r+0x60>
   3de48:	6db8      	ldr	r0, [r7, #88]	; 0x58
   3de4a:	f7fb fcb2 	bl	397b2 <__retarget_lock_release_recursive>
   3de4e:	e7ef      	b.n	3de30 <_vfiprintf_r+0x60>
   3de50:	89bb      	ldrh	r3, [r7, #12]
   3de52:	f003 021a 	and.w	r2, r3, #26
   3de56:	2a0a      	cmp	r2, #10
   3de58:	d116      	bne.n	3de88 <_vfiprintf_r+0xb8>
   3de5a:	f9b7 200e 	ldrsh.w	r2, [r7, #14]
   3de5e:	2a00      	cmp	r2, #0
   3de60:	db12      	blt.n	3de88 <_vfiprintf_r+0xb8>
   3de62:	6e7a      	ldr	r2, [r7, #100]	; 0x64
   3de64:	07d2      	lsls	r2, r2, #31
   3de66:	d404      	bmi.n	3de72 <_vfiprintf_r+0xa2>
   3de68:	059e      	lsls	r6, r3, #22
   3de6a:	d402      	bmi.n	3de72 <_vfiprintf_r+0xa2>
   3de6c:	6db8      	ldr	r0, [r7, #88]	; 0x58
   3de6e:	f7fb fca0 	bl	397b2 <__retarget_lock_release_recursive>
   3de72:	462b      	mov	r3, r5
   3de74:	9a01      	ldr	r2, [sp, #4]
   3de76:	4639      	mov	r1, r7
   3de78:	4648      	mov	r0, r9
   3de7a:	b039      	add	sp, #228	; 0xe4
   3de7c:	ecbd 8b02 	vpop	{d8}
   3de80:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3de84:	f000 bc44 	b.w	3e710 <__sbprintf>
   3de88:	2300      	movs	r3, #0
   3de8a:	ae0f      	add	r6, sp, #60	; 0x3c
   3de8c:	ee08 3a10 	vmov	s16, r3
   3de90:	960c      	str	r6, [sp, #48]	; 0x30
   3de92:	9307      	str	r3, [sp, #28]
   3de94:	9302      	str	r3, [sp, #8]
   3de96:	e9cd 330d 	strd	r3, r3, [sp, #52]	; 0x34
   3de9a:	e9cd 3305 	strd	r3, r3, [sp, #20]
   3de9e:	9b01      	ldr	r3, [sp, #4]
   3dea0:	461d      	mov	r5, r3
   3dea2:	f813 2b01 	ldrb.w	r2, [r3], #1
   3dea6:	b10a      	cbz	r2, 3deac <_vfiprintf_r+0xdc>
   3dea8:	2a25      	cmp	r2, #37	; 0x25
   3deaa:	d1f9      	bne.n	3dea0 <_vfiprintf_r+0xd0>
   3deac:	9b01      	ldr	r3, [sp, #4]
   3deae:	ebb5 0803 	subs.w	r8, r5, r3
   3deb2:	d00d      	beq.n	3ded0 <_vfiprintf_r+0x100>
   3deb4:	e9c6 3800 	strd	r3, r8, [r6]
   3deb8:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3deba:	4443      	add	r3, r8
   3debc:	930e      	str	r3, [sp, #56]	; 0x38
   3debe:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3dec0:	3301      	adds	r3, #1
   3dec2:	2b07      	cmp	r3, #7
   3dec4:	930d      	str	r3, [sp, #52]	; 0x34
   3dec6:	dc75      	bgt.n	3dfb4 <_vfiprintf_r+0x1e4>
   3dec8:	3608      	adds	r6, #8
   3deca:	9b02      	ldr	r3, [sp, #8]
   3decc:	4443      	add	r3, r8
   3dece:	9302      	str	r3, [sp, #8]
   3ded0:	782b      	ldrb	r3, [r5, #0]
   3ded2:	2b00      	cmp	r3, #0
   3ded4:	f000 83d6 	beq.w	3e684 <_vfiprintf_r+0x8b4>
   3ded8:	2300      	movs	r3, #0
   3deda:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
   3dede:	1c6a      	adds	r2, r5, #1
   3dee0:	469a      	mov	sl, r3
   3dee2:	f88d 302b 	strb.w	r3, [sp, #43]	; 0x2b
   3dee6:	9100      	str	r1, [sp, #0]
   3dee8:	9303      	str	r3, [sp, #12]
   3deea:	f812 3b01 	ldrb.w	r3, [r2], #1
   3deee:	9201      	str	r2, [sp, #4]
   3def0:	f1a3 0220 	sub.w	r2, r3, #32
   3def4:	2a5a      	cmp	r2, #90	; 0x5a
   3def6:	f200 831f 	bhi.w	3e538 <_vfiprintf_r+0x768>
   3defa:	e8df f012 	tbh	[pc, r2, lsl #1]
   3defe:	009b      	.short	0x009b
   3df00:	031d031d 	.word	0x031d031d
   3df04:	031d00a3 	.word	0x031d00a3
   3df08:	031d031d 	.word	0x031d031d
   3df0c:	031d0082 	.word	0x031d0082
   3df10:	00a6031d 	.word	0x00a6031d
   3df14:	031d00b0 	.word	0x031d00b0
   3df18:	00b200ad 	.word	0x00b200ad
   3df1c:	00cd031d 	.word	0x00cd031d
   3df20:	00d000d0 	.word	0x00d000d0
   3df24:	00d000d0 	.word	0x00d000d0
   3df28:	00d000d0 	.word	0x00d000d0
   3df2c:	00d000d0 	.word	0x00d000d0
   3df30:	031d00d0 	.word	0x031d00d0
   3df34:	031d031d 	.word	0x031d031d
   3df38:	031d031d 	.word	0x031d031d
   3df3c:	031d031d 	.word	0x031d031d
   3df40:	031d031d 	.word	0x031d031d
   3df44:	010800fa 	.word	0x010800fa
   3df48:	031d031d 	.word	0x031d031d
   3df4c:	031d031d 	.word	0x031d031d
   3df50:	031d031d 	.word	0x031d031d
   3df54:	031d031d 	.word	0x031d031d
   3df58:	031d031d 	.word	0x031d031d
   3df5c:	031d0158 	.word	0x031d0158
   3df60:	031d031d 	.word	0x031d031d
   3df64:	031d019f 	.word	0x031d019f
   3df68:	031d027e 	.word	0x031d027e
   3df6c:	029e031d 	.word	0x029e031d
   3df70:	031d031d 	.word	0x031d031d
   3df74:	031d031d 	.word	0x031d031d
   3df78:	031d031d 	.word	0x031d031d
   3df7c:	031d031d 	.word	0x031d031d
   3df80:	031d031d 	.word	0x031d031d
   3df84:	010a00fa 	.word	0x010a00fa
   3df88:	031d031d 	.word	0x031d031d
   3df8c:	00e0031d 	.word	0x00e0031d
   3df90:	00f4010a 	.word	0x00f4010a
   3df94:	00ed031d 	.word	0x00ed031d
   3df98:	0136031d 	.word	0x0136031d
   3df9c:	018f015a 	.word	0x018f015a
   3dfa0:	031d00f4 	.word	0x031d00f4
   3dfa4:	0099019f 	.word	0x0099019f
   3dfa8:	031d0280 	.word	0x031d0280
   3dfac:	0065031d 	.word	0x0065031d
   3dfb0:	0099031d 	.word	0x0099031d
   3dfb4:	aa0c      	add	r2, sp, #48	; 0x30
   3dfb6:	4639      	mov	r1, r7
   3dfb8:	4648      	mov	r0, r9
   3dfba:	f7ff fed7 	bl	3dd6c <__sprint_r>
   3dfbe:	2800      	cmp	r0, #0
   3dfc0:	f040 833f 	bne.w	3e642 <_vfiprintf_r+0x872>
   3dfc4:	ae0f      	add	r6, sp, #60	; 0x3c
   3dfc6:	e780      	b.n	3deca <_vfiprintf_r+0xfa>
   3dfc8:	4a9b      	ldr	r2, [pc, #620]	; (3e238 <_vfiprintf_r+0x468>)
   3dfca:	9205      	str	r2, [sp, #20]
   3dfcc:	f01a 0220 	ands.w	r2, sl, #32
   3dfd0:	f000 8235 	beq.w	3e43e <_vfiprintf_r+0x66e>
   3dfd4:	3407      	adds	r4, #7
   3dfd6:	f024 0207 	bic.w	r2, r4, #7
   3dfda:	4693      	mov	fp, r2
   3dfdc:	6855      	ldr	r5, [r2, #4]
   3dfde:	f85b 4b08 	ldr.w	r4, [fp], #8
   3dfe2:	f01a 0f01 	tst.w	sl, #1
   3dfe6:	d009      	beq.n	3dffc <_vfiprintf_r+0x22c>
   3dfe8:	ea54 0205 	orrs.w	r2, r4, r5
   3dfec:	bf1f      	itttt	ne
   3dfee:	2230      	movne	r2, #48	; 0x30
   3dff0:	f04a 0a02 	orrne.w	sl, sl, #2
   3dff4:	f88d 302d 	strbne.w	r3, [sp, #45]	; 0x2d
   3dff8:	f88d 202c 	strbne.w	r2, [sp, #44]	; 0x2c
   3dffc:	f42a 6a80 	bic.w	sl, sl, #1024	; 0x400
   3e000:	e118      	b.n	3e234 <_vfiprintf_r+0x464>
   3e002:	4648      	mov	r0, r9
   3e004:	f7fb fb94 	bl	39730 <_localeconv_r>
   3e008:	6843      	ldr	r3, [r0, #4]
   3e00a:	4618      	mov	r0, r3
   3e00c:	ee08 3a10 	vmov	s16, r3
   3e010:	f7f5 fe8e 	bl	33d30 <strlen>
   3e014:	9007      	str	r0, [sp, #28]
   3e016:	4648      	mov	r0, r9
   3e018:	f7fb fb8a 	bl	39730 <_localeconv_r>
   3e01c:	6883      	ldr	r3, [r0, #8]
   3e01e:	9306      	str	r3, [sp, #24]
   3e020:	9b07      	ldr	r3, [sp, #28]
   3e022:	b12b      	cbz	r3, 3e030 <_vfiprintf_r+0x260>
   3e024:	9b06      	ldr	r3, [sp, #24]
   3e026:	b11b      	cbz	r3, 3e030 <_vfiprintf_r+0x260>
   3e028:	781b      	ldrb	r3, [r3, #0]
   3e02a:	b10b      	cbz	r3, 3e030 <_vfiprintf_r+0x260>
   3e02c:	f44a 6a80 	orr.w	sl, sl, #1024	; 0x400
   3e030:	9a01      	ldr	r2, [sp, #4]
   3e032:	e75a      	b.n	3deea <_vfiprintf_r+0x11a>
   3e034:	f89d 302b 	ldrb.w	r3, [sp, #43]	; 0x2b
   3e038:	2b00      	cmp	r3, #0
   3e03a:	d1f9      	bne.n	3e030 <_vfiprintf_r+0x260>
   3e03c:	2320      	movs	r3, #32
   3e03e:	f88d 302b 	strb.w	r3, [sp, #43]	; 0x2b
   3e042:	e7f5      	b.n	3e030 <_vfiprintf_r+0x260>
   3e044:	f04a 0a01 	orr.w	sl, sl, #1
   3e048:	e7f2      	b.n	3e030 <_vfiprintf_r+0x260>
   3e04a:	f854 3b04 	ldr.w	r3, [r4], #4
   3e04e:	2b00      	cmp	r3, #0
   3e050:	9303      	str	r3, [sp, #12]
   3e052:	daed      	bge.n	3e030 <_vfiprintf_r+0x260>
   3e054:	425b      	negs	r3, r3
   3e056:	9303      	str	r3, [sp, #12]
   3e058:	f04a 0a04 	orr.w	sl, sl, #4
   3e05c:	e7e8      	b.n	3e030 <_vfiprintf_r+0x260>
   3e05e:	232b      	movs	r3, #43	; 0x2b
   3e060:	e7ed      	b.n	3e03e <_vfiprintf_r+0x26e>
   3e062:	9a01      	ldr	r2, [sp, #4]
   3e064:	f812 3b01 	ldrb.w	r3, [r2], #1
   3e068:	2b2a      	cmp	r3, #42	; 0x2a
   3e06a:	d112      	bne.n	3e092 <_vfiprintf_r+0x2c2>
   3e06c:	f854 0b04 	ldr.w	r0, [r4], #4
   3e070:	ea40 73e0 	orr.w	r3, r0, r0, asr #31
   3e074:	e9cd 3200 	strd	r3, r2, [sp]
   3e078:	e7da      	b.n	3e030 <_vfiprintf_r+0x260>
   3e07a:	9b00      	ldr	r3, [sp, #0]
   3e07c:	200a      	movs	r0, #10
   3e07e:	fb00 1303 	mla	r3, r0, r3, r1
   3e082:	9300      	str	r3, [sp, #0]
   3e084:	f812 3b01 	ldrb.w	r3, [r2], #1
   3e088:	f1a3 0130 	sub.w	r1, r3, #48	; 0x30
   3e08c:	2909      	cmp	r1, #9
   3e08e:	d9f4      	bls.n	3e07a <_vfiprintf_r+0x2aa>
   3e090:	e72d      	b.n	3deee <_vfiprintf_r+0x11e>
   3e092:	2100      	movs	r1, #0
   3e094:	9100      	str	r1, [sp, #0]
   3e096:	e7f7      	b.n	3e088 <_vfiprintf_r+0x2b8>
   3e098:	f04a 0a80 	orr.w	sl, sl, #128	; 0x80
   3e09c:	e7c8      	b.n	3e030 <_vfiprintf_r+0x260>
   3e09e:	2100      	movs	r1, #0
   3e0a0:	9a01      	ldr	r2, [sp, #4]
   3e0a2:	9103      	str	r1, [sp, #12]
   3e0a4:	9903      	ldr	r1, [sp, #12]
   3e0a6:	3b30      	subs	r3, #48	; 0x30
   3e0a8:	200a      	movs	r0, #10
   3e0aa:	fb00 3301 	mla	r3, r0, r1, r3
   3e0ae:	9303      	str	r3, [sp, #12]
   3e0b0:	f812 3b01 	ldrb.w	r3, [r2], #1
   3e0b4:	f1a3 0130 	sub.w	r1, r3, #48	; 0x30
   3e0b8:	2909      	cmp	r1, #9
   3e0ba:	d9f3      	bls.n	3e0a4 <_vfiprintf_r+0x2d4>
   3e0bc:	e717      	b.n	3deee <_vfiprintf_r+0x11e>
   3e0be:	9b01      	ldr	r3, [sp, #4]
   3e0c0:	781b      	ldrb	r3, [r3, #0]
   3e0c2:	2b68      	cmp	r3, #104	; 0x68
   3e0c4:	bf05      	ittet	eq
   3e0c6:	9b01      	ldreq	r3, [sp, #4]
   3e0c8:	f44a 7a00 	orreq.w	sl, sl, #512	; 0x200
   3e0cc:	f04a 0a40 	orrne.w	sl, sl, #64	; 0x40
   3e0d0:	3301      	addeq	r3, #1
   3e0d2:	bf08      	it	eq
   3e0d4:	9301      	streq	r3, [sp, #4]
   3e0d6:	e7ab      	b.n	3e030 <_vfiprintf_r+0x260>
   3e0d8:	9b01      	ldr	r3, [sp, #4]
   3e0da:	781b      	ldrb	r3, [r3, #0]
   3e0dc:	2b6c      	cmp	r3, #108	; 0x6c
   3e0de:	d105      	bne.n	3e0ec <_vfiprintf_r+0x31c>
   3e0e0:	9b01      	ldr	r3, [sp, #4]
   3e0e2:	3301      	adds	r3, #1
   3e0e4:	9301      	str	r3, [sp, #4]
   3e0e6:	f04a 0a20 	orr.w	sl, sl, #32
   3e0ea:	e7a1      	b.n	3e030 <_vfiprintf_r+0x260>
   3e0ec:	f04a 0a10 	orr.w	sl, sl, #16
   3e0f0:	e79e      	b.n	3e030 <_vfiprintf_r+0x260>
   3e0f2:	46a3      	mov	fp, r4
   3e0f4:	2100      	movs	r1, #0
   3e0f6:	f85b 3b04 	ldr.w	r3, [fp], #4
   3e0fa:	f88d 102b 	strb.w	r1, [sp, #43]	; 0x2b
   3e0fe:	f88d 307c 	strb.w	r3, [sp, #124]	; 0x7c
   3e102:	2301      	movs	r3, #1
   3e104:	460d      	mov	r5, r1
   3e106:	f10d 087c 	add.w	r8, sp, #124	; 0x7c
   3e10a:	9300      	str	r3, [sp, #0]
   3e10c:	e0ab      	b.n	3e266 <_vfiprintf_r+0x496>
   3e10e:	f04a 0a10 	orr.w	sl, sl, #16
   3e112:	f01a 0f20 	tst.w	sl, #32
   3e116:	d011      	beq.n	3e13c <_vfiprintf_r+0x36c>
   3e118:	3407      	adds	r4, #7
   3e11a:	f024 0307 	bic.w	r3, r4, #7
   3e11e:	469b      	mov	fp, r3
   3e120:	685d      	ldr	r5, [r3, #4]
   3e122:	f85b 4b08 	ldr.w	r4, [fp], #8
   3e126:	2d00      	cmp	r5, #0
   3e128:	da06      	bge.n	3e138 <_vfiprintf_r+0x368>
   3e12a:	4264      	negs	r4, r4
   3e12c:	f04f 032d 	mov.w	r3, #45	; 0x2d
   3e130:	eb65 0545 	sbc.w	r5, r5, r5, lsl #1
   3e134:	f88d 302b 	strb.w	r3, [sp, #43]	; 0x2b
   3e138:	2301      	movs	r3, #1
   3e13a:	e04a      	b.n	3e1d2 <_vfiprintf_r+0x402>
   3e13c:	46a3      	mov	fp, r4
   3e13e:	f01a 0f10 	tst.w	sl, #16
   3e142:	f85b 5b04 	ldr.w	r5, [fp], #4
   3e146:	d002      	beq.n	3e14e <_vfiprintf_r+0x37e>
   3e148:	462c      	mov	r4, r5
   3e14a:	17ed      	asrs	r5, r5, #31
   3e14c:	e7eb      	b.n	3e126 <_vfiprintf_r+0x356>
   3e14e:	f01a 0f40 	tst.w	sl, #64	; 0x40
   3e152:	d003      	beq.n	3e15c <_vfiprintf_r+0x38c>
   3e154:	b22c      	sxth	r4, r5
   3e156:	f345 35c0 	sbfx	r5, r5, #15, #1
   3e15a:	e7e4      	b.n	3e126 <_vfiprintf_r+0x356>
   3e15c:	f41a 7f00 	tst.w	sl, #512	; 0x200
   3e160:	d0f2      	beq.n	3e148 <_vfiprintf_r+0x378>
   3e162:	b26c      	sxtb	r4, r5
   3e164:	f345 15c0 	sbfx	r5, r5, #7, #1
   3e168:	e7dd      	b.n	3e126 <_vfiprintf_r+0x356>
   3e16a:	f01a 0f20 	tst.w	sl, #32
   3e16e:	f104 0b04 	add.w	fp, r4, #4
   3e172:	d007      	beq.n	3e184 <_vfiprintf_r+0x3b4>
   3e174:	9a02      	ldr	r2, [sp, #8]
   3e176:	6823      	ldr	r3, [r4, #0]
   3e178:	17d2      	asrs	r2, r2, #31
   3e17a:	9902      	ldr	r1, [sp, #8]
   3e17c:	e9c3 1200 	strd	r1, r2, [r3]
   3e180:	465c      	mov	r4, fp
   3e182:	e68c      	b.n	3de9e <_vfiprintf_r+0xce>
   3e184:	f01a 0f10 	tst.w	sl, #16
   3e188:	d003      	beq.n	3e192 <_vfiprintf_r+0x3c2>
   3e18a:	6823      	ldr	r3, [r4, #0]
   3e18c:	9a02      	ldr	r2, [sp, #8]
   3e18e:	601a      	str	r2, [r3, #0]
   3e190:	e7f6      	b.n	3e180 <_vfiprintf_r+0x3b0>
   3e192:	f01a 0f40 	tst.w	sl, #64	; 0x40
   3e196:	d003      	beq.n	3e1a0 <_vfiprintf_r+0x3d0>
   3e198:	6823      	ldr	r3, [r4, #0]
   3e19a:	9a02      	ldr	r2, [sp, #8]
   3e19c:	801a      	strh	r2, [r3, #0]
   3e19e:	e7ef      	b.n	3e180 <_vfiprintf_r+0x3b0>
   3e1a0:	f41a 7f00 	tst.w	sl, #512	; 0x200
   3e1a4:	d0f1      	beq.n	3e18a <_vfiprintf_r+0x3ba>
   3e1a6:	6823      	ldr	r3, [r4, #0]
   3e1a8:	9a02      	ldr	r2, [sp, #8]
   3e1aa:	701a      	strb	r2, [r3, #0]
   3e1ac:	e7e8      	b.n	3e180 <_vfiprintf_r+0x3b0>
   3e1ae:	f04a 0a10 	orr.w	sl, sl, #16
   3e1b2:	f01a 0320 	ands.w	r3, sl, #32
   3e1b6:	d01f      	beq.n	3e1f8 <_vfiprintf_r+0x428>
   3e1b8:	3407      	adds	r4, #7
   3e1ba:	f024 0307 	bic.w	r3, r4, #7
   3e1be:	469b      	mov	fp, r3
   3e1c0:	685d      	ldr	r5, [r3, #4]
   3e1c2:	f85b 4b08 	ldr.w	r4, [fp], #8
   3e1c6:	f42a 6a80 	bic.w	sl, sl, #1024	; 0x400
   3e1ca:	2300      	movs	r3, #0
   3e1cc:	2200      	movs	r2, #0
   3e1ce:	f88d 202b 	strb.w	r2, [sp, #43]	; 0x2b
   3e1d2:	9a00      	ldr	r2, [sp, #0]
   3e1d4:	3201      	adds	r2, #1
   3e1d6:	f000 8262 	beq.w	3e69e <_vfiprintf_r+0x8ce>
   3e1da:	f02a 0280 	bic.w	r2, sl, #128	; 0x80
   3e1de:	9204      	str	r2, [sp, #16]
   3e1e0:	ea54 0205 	orrs.w	r2, r4, r5
   3e1e4:	f040 8261 	bne.w	3e6aa <_vfiprintf_r+0x8da>
   3e1e8:	9a00      	ldr	r2, [sp, #0]
   3e1ea:	2a00      	cmp	r2, #0
   3e1ec:	f000 8199 	beq.w	3e522 <_vfiprintf_r+0x752>
   3e1f0:	2b01      	cmp	r3, #1
   3e1f2:	f040 825d 	bne.w	3e6b0 <_vfiprintf_r+0x8e0>
   3e1f6:	e139      	b.n	3e46c <_vfiprintf_r+0x69c>
   3e1f8:	46a3      	mov	fp, r4
   3e1fa:	f01a 0510 	ands.w	r5, sl, #16
   3e1fe:	f85b 4b04 	ldr.w	r4, [fp], #4
   3e202:	d001      	beq.n	3e208 <_vfiprintf_r+0x438>
   3e204:	461d      	mov	r5, r3
   3e206:	e7de      	b.n	3e1c6 <_vfiprintf_r+0x3f6>
   3e208:	f01a 0340 	ands.w	r3, sl, #64	; 0x40
   3e20c:	d001      	beq.n	3e212 <_vfiprintf_r+0x442>
   3e20e:	b2a4      	uxth	r4, r4
   3e210:	e7d9      	b.n	3e1c6 <_vfiprintf_r+0x3f6>
   3e212:	f41a 7500 	ands.w	r5, sl, #512	; 0x200
   3e216:	d0d6      	beq.n	3e1c6 <_vfiprintf_r+0x3f6>
   3e218:	b2e4      	uxtb	r4, r4
   3e21a:	e7f3      	b.n	3e204 <_vfiprintf_r+0x434>
   3e21c:	f647 0330 	movw	r3, #30768	; 0x7830
   3e220:	46a3      	mov	fp, r4
   3e222:	2500      	movs	r5, #0
   3e224:	f04a 0a02 	orr.w	sl, sl, #2
   3e228:	f8ad 302c 	strh.w	r3, [sp, #44]	; 0x2c
   3e22c:	f85b 4b04 	ldr.w	r4, [fp], #4
   3e230:	4b01      	ldr	r3, [pc, #4]	; (3e238 <_vfiprintf_r+0x468>)
   3e232:	9305      	str	r3, [sp, #20]
   3e234:	2302      	movs	r3, #2
   3e236:	e7c9      	b.n	3e1cc <_vfiprintf_r+0x3fc>
   3e238:	000404df 	.word	0x000404df
   3e23c:	9b00      	ldr	r3, [sp, #0]
   3e23e:	46a3      	mov	fp, r4
   3e240:	2500      	movs	r5, #0
   3e242:	1c5c      	adds	r4, r3, #1
   3e244:	f85b 8b04 	ldr.w	r8, [fp], #4
   3e248:	f88d 502b 	strb.w	r5, [sp, #43]	; 0x2b
   3e24c:	f000 80d0 	beq.w	3e3f0 <_vfiprintf_r+0x620>
   3e250:	461a      	mov	r2, r3
   3e252:	4629      	mov	r1, r5
   3e254:	4640      	mov	r0, r8
   3e256:	f7fb fda7 	bl	39da8 <memchr>
   3e25a:	2800      	cmp	r0, #0
   3e25c:	f000 8176 	beq.w	3e54c <_vfiprintf_r+0x77c>
   3e260:	eba0 0308 	sub.w	r3, r0, r8
   3e264:	9300      	str	r3, [sp, #0]
   3e266:	9b00      	ldr	r3, [sp, #0]
   3e268:	42ab      	cmp	r3, r5
   3e26a:	bfb8      	it	lt
   3e26c:	462b      	movlt	r3, r5
   3e26e:	9304      	str	r3, [sp, #16]
   3e270:	f89d 302b 	ldrb.w	r3, [sp, #43]	; 0x2b
   3e274:	b113      	cbz	r3, 3e27c <_vfiprintf_r+0x4ac>
   3e276:	9b04      	ldr	r3, [sp, #16]
   3e278:	3301      	adds	r3, #1
   3e27a:	9304      	str	r3, [sp, #16]
   3e27c:	f01a 0302 	ands.w	r3, sl, #2
   3e280:	9308      	str	r3, [sp, #32]
   3e282:	bf1e      	ittt	ne
   3e284:	9b04      	ldrne	r3, [sp, #16]
   3e286:	3302      	addne	r3, #2
   3e288:	9304      	strne	r3, [sp, #16]
   3e28a:	f01a 0384 	ands.w	r3, sl, #132	; 0x84
   3e28e:	9309      	str	r3, [sp, #36]	; 0x24
   3e290:	d11f      	bne.n	3e2d2 <_vfiprintf_r+0x502>
   3e292:	e9dd 3203 	ldrd	r3, r2, [sp, #12]
   3e296:	1a9c      	subs	r4, r3, r2
   3e298:	2c00      	cmp	r4, #0
   3e29a:	dd1a      	ble.n	3e2d2 <_vfiprintf_r+0x502>
   3e29c:	48ac      	ldr	r0, [pc, #688]	; (3e550 <_vfiprintf_r+0x780>)
   3e29e:	2c10      	cmp	r4, #16
   3e2a0:	f106 0108 	add.w	r1, r6, #8
   3e2a4:	6030      	str	r0, [r6, #0]
   3e2a6:	e9dd 320d 	ldrd	r3, r2, [sp, #52]	; 0x34
   3e2aa:	f103 0301 	add.w	r3, r3, #1
   3e2ae:	f300 8155 	bgt.w	3e55c <_vfiprintf_r+0x78c>
   3e2b2:	6074      	str	r4, [r6, #4]
   3e2b4:	2b07      	cmp	r3, #7
   3e2b6:	4414      	add	r4, r2
   3e2b8:	e9cd 340d 	strd	r3, r4, [sp, #52]	; 0x34
   3e2bc:	f340 8160 	ble.w	3e580 <_vfiprintf_r+0x7b0>
   3e2c0:	aa0c      	add	r2, sp, #48	; 0x30
   3e2c2:	4639      	mov	r1, r7
   3e2c4:	4648      	mov	r0, r9
   3e2c6:	f7ff fd51 	bl	3dd6c <__sprint_r>
   3e2ca:	2800      	cmp	r0, #0
   3e2cc:	f040 81b9 	bne.w	3e642 <_vfiprintf_r+0x872>
   3e2d0:	ae0f      	add	r6, sp, #60	; 0x3c
   3e2d2:	f89d 302b 	ldrb.w	r3, [sp, #43]	; 0x2b
   3e2d6:	b17b      	cbz	r3, 3e2f8 <_vfiprintf_r+0x528>
   3e2d8:	f10d 022b 	add.w	r2, sp, #43	; 0x2b
   3e2dc:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3e2de:	6032      	str	r2, [r6, #0]
   3e2e0:	2201      	movs	r2, #1
   3e2e2:	3301      	adds	r3, #1
   3e2e4:	6072      	str	r2, [r6, #4]
   3e2e6:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   3e2e8:	2b07      	cmp	r3, #7
   3e2ea:	f102 0201 	add.w	r2, r2, #1
   3e2ee:	e9cd 320d 	strd	r3, r2, [sp, #52]	; 0x34
   3e2f2:	f300 8147 	bgt.w	3e584 <_vfiprintf_r+0x7b4>
   3e2f6:	3608      	adds	r6, #8
   3e2f8:	9b08      	ldr	r3, [sp, #32]
   3e2fa:	b173      	cbz	r3, 3e31a <_vfiprintf_r+0x54a>
   3e2fc:	aa0b      	add	r2, sp, #44	; 0x2c
   3e2fe:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3e300:	6032      	str	r2, [r6, #0]
   3e302:	2202      	movs	r2, #2
   3e304:	3301      	adds	r3, #1
   3e306:	6072      	str	r2, [r6, #4]
   3e308:	9a0e      	ldr	r2, [sp, #56]	; 0x38
   3e30a:	2b07      	cmp	r3, #7
   3e30c:	f102 0202 	add.w	r2, r2, #2
   3e310:	e9cd 320d 	strd	r3, r2, [sp, #52]	; 0x34
   3e314:	f300 813f 	bgt.w	3e596 <_vfiprintf_r+0x7c6>
   3e318:	3608      	adds	r6, #8
   3e31a:	9b09      	ldr	r3, [sp, #36]	; 0x24
   3e31c:	2b80      	cmp	r3, #128	; 0x80
   3e31e:	d11f      	bne.n	3e360 <_vfiprintf_r+0x590>
   3e320:	e9dd 3203 	ldrd	r3, r2, [sp, #12]
   3e324:	1a9c      	subs	r4, r3, r2
   3e326:	2c00      	cmp	r4, #0
   3e328:	dd1a      	ble.n	3e360 <_vfiprintf_r+0x590>
   3e32a:	488a      	ldr	r0, [pc, #552]	; (3e554 <_vfiprintf_r+0x784>)
   3e32c:	2c10      	cmp	r4, #16
   3e32e:	f106 0108 	add.w	r1, r6, #8
   3e332:	6030      	str	r0, [r6, #0]
   3e334:	e9dd 320d 	ldrd	r3, r2, [sp, #52]	; 0x34
   3e338:	f103 0301 	add.w	r3, r3, #1
   3e33c:	f300 8134 	bgt.w	3e5a8 <_vfiprintf_r+0x7d8>
   3e340:	6074      	str	r4, [r6, #4]
   3e342:	2b07      	cmp	r3, #7
   3e344:	4414      	add	r4, r2
   3e346:	e9cd 340d 	strd	r3, r4, [sp, #52]	; 0x34
   3e34a:	f340 813f 	ble.w	3e5cc <_vfiprintf_r+0x7fc>
   3e34e:	aa0c      	add	r2, sp, #48	; 0x30
   3e350:	4639      	mov	r1, r7
   3e352:	4648      	mov	r0, r9
   3e354:	f7ff fd0a 	bl	3dd6c <__sprint_r>
   3e358:	2800      	cmp	r0, #0
   3e35a:	f040 8172 	bne.w	3e642 <_vfiprintf_r+0x872>
   3e35e:	ae0f      	add	r6, sp, #60	; 0x3c
   3e360:	9b00      	ldr	r3, [sp, #0]
   3e362:	1aec      	subs	r4, r5, r3
   3e364:	2c00      	cmp	r4, #0
   3e366:	dd1a      	ble.n	3e39e <_vfiprintf_r+0x5ce>
   3e368:	4d7a      	ldr	r5, [pc, #488]	; (3e554 <_vfiprintf_r+0x784>)
   3e36a:	2c10      	cmp	r4, #16
   3e36c:	f106 0208 	add.w	r2, r6, #8
   3e370:	6035      	str	r5, [r6, #0]
   3e372:	e9dd 310d 	ldrd	r3, r1, [sp, #52]	; 0x34
   3e376:	f103 0301 	add.w	r3, r3, #1
   3e37a:	f300 8129 	bgt.w	3e5d0 <_vfiprintf_r+0x800>
   3e37e:	6074      	str	r4, [r6, #4]
   3e380:	2b07      	cmp	r3, #7
   3e382:	440c      	add	r4, r1
   3e384:	e9cd 340d 	strd	r3, r4, [sp, #52]	; 0x34
   3e388:	f340 8133 	ble.w	3e5f2 <_vfiprintf_r+0x822>
   3e38c:	aa0c      	add	r2, sp, #48	; 0x30
   3e38e:	4639      	mov	r1, r7
   3e390:	4648      	mov	r0, r9
   3e392:	f7ff fceb 	bl	3dd6c <__sprint_r>
   3e396:	2800      	cmp	r0, #0
   3e398:	f040 8153 	bne.w	3e642 <_vfiprintf_r+0x872>
   3e39c:	ae0f      	add	r6, sp, #60	; 0x3c
   3e39e:	9b00      	ldr	r3, [sp, #0]
   3e3a0:	980e      	ldr	r0, [sp, #56]	; 0x38
   3e3a2:	6073      	str	r3, [r6, #4]
   3e3a4:	4418      	add	r0, r3
   3e3a6:	9b0d      	ldr	r3, [sp, #52]	; 0x34
   3e3a8:	f8c6 8000 	str.w	r8, [r6]
   3e3ac:	3301      	adds	r3, #1
   3e3ae:	900e      	str	r0, [sp, #56]	; 0x38
   3e3b0:	2b07      	cmp	r3, #7
   3e3b2:	930d      	str	r3, [sp, #52]	; 0x34
   3e3b4:	f300 811f 	bgt.w	3e5f6 <_vfiprintf_r+0x826>
   3e3b8:	f106 0308 	add.w	r3, r6, #8
   3e3bc:	f01a 0f04 	tst.w	sl, #4
   3e3c0:	f040 8121 	bne.w	3e606 <_vfiprintf_r+0x836>
   3e3c4:	9904      	ldr	r1, [sp, #16]
   3e3c6:	e9dd 3202 	ldrd	r3, r2, [sp, #8]
   3e3ca:	428a      	cmp	r2, r1
   3e3cc:	bfac      	ite	ge
   3e3ce:	189b      	addge	r3, r3, r2
   3e3d0:	185b      	addlt	r3, r3, r1
   3e3d2:	9302      	str	r3, [sp, #8]
   3e3d4:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3e3d6:	b13b      	cbz	r3, 3e3e8 <_vfiprintf_r+0x618>
   3e3d8:	aa0c      	add	r2, sp, #48	; 0x30
   3e3da:	4639      	mov	r1, r7
   3e3dc:	4648      	mov	r0, r9
   3e3de:	f7ff fcc5 	bl	3dd6c <__sprint_r>
   3e3e2:	2800      	cmp	r0, #0
   3e3e4:	f040 812d 	bne.w	3e642 <_vfiprintf_r+0x872>
   3e3e8:	2300      	movs	r3, #0
   3e3ea:	ae0f      	add	r6, sp, #60	; 0x3c
   3e3ec:	930d      	str	r3, [sp, #52]	; 0x34
   3e3ee:	e6c7      	b.n	3e180 <_vfiprintf_r+0x3b0>
   3e3f0:	4640      	mov	r0, r8
   3e3f2:	f7f5 fc9d 	bl	33d30 <strlen>
   3e3f6:	9000      	str	r0, [sp, #0]
   3e3f8:	e735      	b.n	3e266 <_vfiprintf_r+0x496>
   3e3fa:	f04a 0a10 	orr.w	sl, sl, #16
   3e3fe:	f01a 0320 	ands.w	r3, sl, #32
   3e402:	d008      	beq.n	3e416 <_vfiprintf_r+0x646>
   3e404:	3407      	adds	r4, #7
   3e406:	f024 0307 	bic.w	r3, r4, #7
   3e40a:	469b      	mov	fp, r3
   3e40c:	685d      	ldr	r5, [r3, #4]
   3e40e:	f85b 4b08 	ldr.w	r4, [fp], #8
   3e412:	2301      	movs	r3, #1
   3e414:	e6da      	b.n	3e1cc <_vfiprintf_r+0x3fc>
   3e416:	46a3      	mov	fp, r4
   3e418:	f01a 0510 	ands.w	r5, sl, #16
   3e41c:	f85b 4b04 	ldr.w	r4, [fp], #4
   3e420:	d001      	beq.n	3e426 <_vfiprintf_r+0x656>
   3e422:	461d      	mov	r5, r3
   3e424:	e7f5      	b.n	3e412 <_vfiprintf_r+0x642>
   3e426:	f01a 0340 	ands.w	r3, sl, #64	; 0x40
   3e42a:	d001      	beq.n	3e430 <_vfiprintf_r+0x660>
   3e42c:	b2a4      	uxth	r4, r4
   3e42e:	e7f0      	b.n	3e412 <_vfiprintf_r+0x642>
   3e430:	f41a 7500 	ands.w	r5, sl, #512	; 0x200
   3e434:	d0ed      	beq.n	3e412 <_vfiprintf_r+0x642>
   3e436:	b2e4      	uxtb	r4, r4
   3e438:	e7f3      	b.n	3e422 <_vfiprintf_r+0x652>
   3e43a:	4a47      	ldr	r2, [pc, #284]	; (3e558 <_vfiprintf_r+0x788>)
   3e43c:	e5c5      	b.n	3dfca <_vfiprintf_r+0x1fa>
   3e43e:	46a3      	mov	fp, r4
   3e440:	f01a 0510 	ands.w	r5, sl, #16
   3e444:	f85b 4b04 	ldr.w	r4, [fp], #4
   3e448:	d001      	beq.n	3e44e <_vfiprintf_r+0x67e>
   3e44a:	4615      	mov	r5, r2
   3e44c:	e5c9      	b.n	3dfe2 <_vfiprintf_r+0x212>
   3e44e:	f01a 0240 	ands.w	r2, sl, #64	; 0x40
   3e452:	d001      	beq.n	3e458 <_vfiprintf_r+0x688>
   3e454:	b2a4      	uxth	r4, r4
   3e456:	e5c4      	b.n	3dfe2 <_vfiprintf_r+0x212>
   3e458:	f41a 7500 	ands.w	r5, sl, #512	; 0x200
   3e45c:	f43f adc1 	beq.w	3dfe2 <_vfiprintf_r+0x212>
   3e460:	b2e4      	uxtb	r4, r4
   3e462:	e7f2      	b.n	3e44a <_vfiprintf_r+0x67a>
   3e464:	2c0a      	cmp	r4, #10
   3e466:	f175 0300 	sbcs.w	r3, r5, #0
   3e46a:	d205      	bcs.n	3e478 <_vfiprintf_r+0x6a8>
   3e46c:	3430      	adds	r4, #48	; 0x30
   3e46e:	f88d 40df 	strb.w	r4, [sp, #223]	; 0xdf
   3e472:	f10d 08df 	add.w	r8, sp, #223	; 0xdf
   3e476:	e137      	b.n	3e6e8 <_vfiprintf_r+0x918>
   3e478:	ab38      	add	r3, sp, #224	; 0xe0
   3e47a:	f04f 0a00 	mov.w	sl, #0
   3e47e:	9308      	str	r3, [sp, #32]
   3e480:	9b04      	ldr	r3, [sp, #16]
   3e482:	f403 6380 	and.w	r3, r3, #1024	; 0x400
   3e486:	9309      	str	r3, [sp, #36]	; 0x24
   3e488:	9b08      	ldr	r3, [sp, #32]
   3e48a:	220a      	movs	r2, #10
   3e48c:	4620      	mov	r0, r4
   3e48e:	4629      	mov	r1, r5
   3e490:	f103 38ff 	add.w	r8, r3, #4294967295	; 0xffffffff
   3e494:	2300      	movs	r3, #0
   3e496:	f001 fd99 	bl	3ffcc <__aeabi_uldivmod>
   3e49a:	9b08      	ldr	r3, [sp, #32]
   3e49c:	3230      	adds	r2, #48	; 0x30
   3e49e:	f10a 0a01 	add.w	sl, sl, #1
   3e4a2:	f803 2c01 	strb.w	r2, [r3, #-1]
   3e4a6:	9b09      	ldr	r3, [sp, #36]	; 0x24
   3e4a8:	b1db      	cbz	r3, 3e4e2 <_vfiprintf_r+0x712>
   3e4aa:	9b06      	ldr	r3, [sp, #24]
   3e4ac:	781b      	ldrb	r3, [r3, #0]
   3e4ae:	4553      	cmp	r3, sl
   3e4b0:	d117      	bne.n	3e4e2 <_vfiprintf_r+0x712>
   3e4b2:	f1ba 0fff 	cmp.w	sl, #255	; 0xff
   3e4b6:	d014      	beq.n	3e4e2 <_vfiprintf_r+0x712>
   3e4b8:	2c0a      	cmp	r4, #10
   3e4ba:	f175 0300 	sbcs.w	r3, r5, #0
   3e4be:	d310      	bcc.n	3e4e2 <_vfiprintf_r+0x712>
   3e4c0:	9b07      	ldr	r3, [sp, #28]
   3e4c2:	ee18 1a10 	vmov	r1, s16
   3e4c6:	eba8 0803 	sub.w	r8, r8, r3
   3e4ca:	461a      	mov	r2, r3
   3e4cc:	4640      	mov	r0, r8
   3e4ce:	f7f5 fcac 	bl	33e2a <strncpy>
   3e4d2:	9b06      	ldr	r3, [sp, #24]
   3e4d4:	785b      	ldrb	r3, [r3, #1]
   3e4d6:	b1a3      	cbz	r3, 3e502 <_vfiprintf_r+0x732>
   3e4d8:	9b06      	ldr	r3, [sp, #24]
   3e4da:	f04f 0a00 	mov.w	sl, #0
   3e4de:	3301      	adds	r3, #1
   3e4e0:	9306      	str	r3, [sp, #24]
   3e4e2:	2300      	movs	r3, #0
   3e4e4:	220a      	movs	r2, #10
   3e4e6:	4620      	mov	r0, r4
   3e4e8:	4629      	mov	r1, r5
   3e4ea:	f001 fd6f 	bl	3ffcc <__aeabi_uldivmod>
   3e4ee:	2c0a      	cmp	r4, #10
   3e4f0:	f175 0300 	sbcs.w	r3, r5, #0
   3e4f4:	f0c0 80f8 	bcc.w	3e6e8 <_vfiprintf_r+0x918>
   3e4f8:	4604      	mov	r4, r0
   3e4fa:	460d      	mov	r5, r1
   3e4fc:	f8cd 8020 	str.w	r8, [sp, #32]
   3e500:	e7c2      	b.n	3e488 <_vfiprintf_r+0x6b8>
   3e502:	469a      	mov	sl, r3
   3e504:	e7ed      	b.n	3e4e2 <_vfiprintf_r+0x712>
   3e506:	f004 030f 	and.w	r3, r4, #15
   3e50a:	9a05      	ldr	r2, [sp, #20]
   3e50c:	0924      	lsrs	r4, r4, #4
   3e50e:	5cd3      	ldrb	r3, [r2, r3]
   3e510:	ea44 7405 	orr.w	r4, r4, r5, lsl #28
   3e514:	092d      	lsrs	r5, r5, #4
   3e516:	f808 3d01 	strb.w	r3, [r8, #-1]!
   3e51a:	ea54 0305 	orrs.w	r3, r4, r5
   3e51e:	d1f2      	bne.n	3e506 <_vfiprintf_r+0x736>
   3e520:	e0e2      	b.n	3e6e8 <_vfiprintf_r+0x918>
   3e522:	b933      	cbnz	r3, 3e532 <_vfiprintf_r+0x762>
   3e524:	f01a 0f01 	tst.w	sl, #1
   3e528:	d003      	beq.n	3e532 <_vfiprintf_r+0x762>
   3e52a:	2330      	movs	r3, #48	; 0x30
   3e52c:	f88d 30df 	strb.w	r3, [sp, #223]	; 0xdf
   3e530:	e79f      	b.n	3e472 <_vfiprintf_r+0x6a2>
   3e532:	f10d 08e0 	add.w	r8, sp, #224	; 0xe0
   3e536:	e0d7      	b.n	3e6e8 <_vfiprintf_r+0x918>
   3e538:	2b00      	cmp	r3, #0
   3e53a:	f000 80a3 	beq.w	3e684 <_vfiprintf_r+0x8b4>
   3e53e:	2100      	movs	r1, #0
   3e540:	46a3      	mov	fp, r4
   3e542:	f88d 307c 	strb.w	r3, [sp, #124]	; 0x7c
   3e546:	f88d 102b 	strb.w	r1, [sp, #43]	; 0x2b
   3e54a:	e5da      	b.n	3e102 <_vfiprintf_r+0x332>
   3e54c:	4605      	mov	r5, r0
   3e54e:	e68a      	b.n	3e266 <_vfiprintf_r+0x496>
   3e550:	00040942 	.word	0x00040942
   3e554:	00040952 	.word	0x00040952
   3e558:	000404f0 	.word	0x000404f0
   3e55c:	2010      	movs	r0, #16
   3e55e:	2b07      	cmp	r3, #7
   3e560:	4402      	add	r2, r0
   3e562:	6070      	str	r0, [r6, #4]
   3e564:	e9cd 320d 	strd	r3, r2, [sp, #52]	; 0x34
   3e568:	dd07      	ble.n	3e57a <_vfiprintf_r+0x7aa>
   3e56a:	aa0c      	add	r2, sp, #48	; 0x30
   3e56c:	4639      	mov	r1, r7
   3e56e:	4648      	mov	r0, r9
   3e570:	f7ff fbfc 	bl	3dd6c <__sprint_r>
   3e574:	2800      	cmp	r0, #0
   3e576:	d164      	bne.n	3e642 <_vfiprintf_r+0x872>
   3e578:	a90f      	add	r1, sp, #60	; 0x3c
   3e57a:	3c10      	subs	r4, #16
   3e57c:	460e      	mov	r6, r1
   3e57e:	e68d      	b.n	3e29c <_vfiprintf_r+0x4cc>
   3e580:	460e      	mov	r6, r1
   3e582:	e6a6      	b.n	3e2d2 <_vfiprintf_r+0x502>
   3e584:	aa0c      	add	r2, sp, #48	; 0x30
   3e586:	4639      	mov	r1, r7
   3e588:	4648      	mov	r0, r9
   3e58a:	f7ff fbef 	bl	3dd6c <__sprint_r>
   3e58e:	2800      	cmp	r0, #0
   3e590:	d157      	bne.n	3e642 <_vfiprintf_r+0x872>
   3e592:	ae0f      	add	r6, sp, #60	; 0x3c
   3e594:	e6b0      	b.n	3e2f8 <_vfiprintf_r+0x528>
   3e596:	aa0c      	add	r2, sp, #48	; 0x30
   3e598:	4639      	mov	r1, r7
   3e59a:	4648      	mov	r0, r9
   3e59c:	f7ff fbe6 	bl	3dd6c <__sprint_r>
   3e5a0:	2800      	cmp	r0, #0
   3e5a2:	d14e      	bne.n	3e642 <_vfiprintf_r+0x872>
   3e5a4:	ae0f      	add	r6, sp, #60	; 0x3c
   3e5a6:	e6b8      	b.n	3e31a <_vfiprintf_r+0x54a>
   3e5a8:	2010      	movs	r0, #16
   3e5aa:	2b07      	cmp	r3, #7
   3e5ac:	4402      	add	r2, r0
   3e5ae:	6070      	str	r0, [r6, #4]
   3e5b0:	e9cd 320d 	strd	r3, r2, [sp, #52]	; 0x34
   3e5b4:	dd07      	ble.n	3e5c6 <_vfiprintf_r+0x7f6>
   3e5b6:	aa0c      	add	r2, sp, #48	; 0x30
   3e5b8:	4639      	mov	r1, r7
   3e5ba:	4648      	mov	r0, r9
   3e5bc:	f7ff fbd6 	bl	3dd6c <__sprint_r>
   3e5c0:	2800      	cmp	r0, #0
   3e5c2:	d13e      	bne.n	3e642 <_vfiprintf_r+0x872>
   3e5c4:	a90f      	add	r1, sp, #60	; 0x3c
   3e5c6:	3c10      	subs	r4, #16
   3e5c8:	460e      	mov	r6, r1
   3e5ca:	e6ae      	b.n	3e32a <_vfiprintf_r+0x55a>
   3e5cc:	460e      	mov	r6, r1
   3e5ce:	e6c7      	b.n	3e360 <_vfiprintf_r+0x590>
   3e5d0:	2010      	movs	r0, #16
   3e5d2:	2b07      	cmp	r3, #7
   3e5d4:	4401      	add	r1, r0
   3e5d6:	6070      	str	r0, [r6, #4]
   3e5d8:	e9cd 310d 	strd	r3, r1, [sp, #52]	; 0x34
   3e5dc:	dd06      	ble.n	3e5ec <_vfiprintf_r+0x81c>
   3e5de:	aa0c      	add	r2, sp, #48	; 0x30
   3e5e0:	4639      	mov	r1, r7
   3e5e2:	4648      	mov	r0, r9
   3e5e4:	f7ff fbc2 	bl	3dd6c <__sprint_r>
   3e5e8:	bb58      	cbnz	r0, 3e642 <_vfiprintf_r+0x872>
   3e5ea:	aa0f      	add	r2, sp, #60	; 0x3c
   3e5ec:	3c10      	subs	r4, #16
   3e5ee:	4616      	mov	r6, r2
   3e5f0:	e6bb      	b.n	3e36a <_vfiprintf_r+0x59a>
   3e5f2:	4616      	mov	r6, r2
   3e5f4:	e6d3      	b.n	3e39e <_vfiprintf_r+0x5ce>
   3e5f6:	aa0c      	add	r2, sp, #48	; 0x30
   3e5f8:	4639      	mov	r1, r7
   3e5fa:	4648      	mov	r0, r9
   3e5fc:	f7ff fbb6 	bl	3dd6c <__sprint_r>
   3e600:	b9f8      	cbnz	r0, 3e642 <_vfiprintf_r+0x872>
   3e602:	ab0f      	add	r3, sp, #60	; 0x3c
   3e604:	e6da      	b.n	3e3bc <_vfiprintf_r+0x5ec>
   3e606:	e9dd 2103 	ldrd	r2, r1, [sp, #12]
   3e60a:	1a54      	subs	r4, r2, r1
   3e60c:	2c00      	cmp	r4, #0
   3e60e:	f77f aed9 	ble.w	3e3c4 <_vfiprintf_r+0x5f4>
   3e612:	4d39      	ldr	r5, [pc, #228]	; (3e6f8 <_vfiprintf_r+0x928>)
   3e614:	2610      	movs	r6, #16
   3e616:	2c10      	cmp	r4, #16
   3e618:	601d      	str	r5, [r3, #0]
   3e61a:	e9dd 210d 	ldrd	r2, r1, [sp, #52]	; 0x34
   3e61e:	f102 0201 	add.w	r2, r2, #1
   3e622:	dc1d      	bgt.n	3e660 <_vfiprintf_r+0x890>
   3e624:	605c      	str	r4, [r3, #4]
   3e626:	2a07      	cmp	r2, #7
   3e628:	440c      	add	r4, r1
   3e62a:	e9cd 240d 	strd	r2, r4, [sp, #52]	; 0x34
   3e62e:	f77f aec9 	ble.w	3e3c4 <_vfiprintf_r+0x5f4>
   3e632:	aa0c      	add	r2, sp, #48	; 0x30
   3e634:	4639      	mov	r1, r7
   3e636:	4648      	mov	r0, r9
   3e638:	f7ff fb98 	bl	3dd6c <__sprint_r>
   3e63c:	2800      	cmp	r0, #0
   3e63e:	f43f aec1 	beq.w	3e3c4 <_vfiprintf_r+0x5f4>
   3e642:	6e7b      	ldr	r3, [r7, #100]	; 0x64
   3e644:	07d9      	lsls	r1, r3, #31
   3e646:	d405      	bmi.n	3e654 <_vfiprintf_r+0x884>
   3e648:	89bb      	ldrh	r3, [r7, #12]
   3e64a:	059a      	lsls	r2, r3, #22
   3e64c:	d402      	bmi.n	3e654 <_vfiprintf_r+0x884>
   3e64e:	6db8      	ldr	r0, [r7, #88]	; 0x58
   3e650:	f7fb f8af 	bl	397b2 <__retarget_lock_release_recursive>
   3e654:	89bb      	ldrh	r3, [r7, #12]
   3e656:	065b      	lsls	r3, r3, #25
   3e658:	f57f abed 	bpl.w	3de36 <_vfiprintf_r+0x66>
   3e65c:	f7ff bbe8 	b.w	3de30 <_vfiprintf_r+0x60>
   3e660:	3110      	adds	r1, #16
   3e662:	2a07      	cmp	r2, #7
   3e664:	605e      	str	r6, [r3, #4]
   3e666:	e9cd 210d 	strd	r2, r1, [sp, #52]	; 0x34
   3e66a:	dc02      	bgt.n	3e672 <_vfiprintf_r+0x8a2>
   3e66c:	3308      	adds	r3, #8
   3e66e:	3c10      	subs	r4, #16
   3e670:	e7d1      	b.n	3e616 <_vfiprintf_r+0x846>
   3e672:	aa0c      	add	r2, sp, #48	; 0x30
   3e674:	4639      	mov	r1, r7
   3e676:	4648      	mov	r0, r9
   3e678:	f7ff fb78 	bl	3dd6c <__sprint_r>
   3e67c:	2800      	cmp	r0, #0
   3e67e:	d1e0      	bne.n	3e642 <_vfiprintf_r+0x872>
   3e680:	ab0f      	add	r3, sp, #60	; 0x3c
   3e682:	e7f4      	b.n	3e66e <_vfiprintf_r+0x89e>
   3e684:	9b0e      	ldr	r3, [sp, #56]	; 0x38
   3e686:	b913      	cbnz	r3, 3e68e <_vfiprintf_r+0x8be>
   3e688:	2300      	movs	r3, #0
   3e68a:	930d      	str	r3, [sp, #52]	; 0x34
   3e68c:	e7d9      	b.n	3e642 <_vfiprintf_r+0x872>
   3e68e:	aa0c      	add	r2, sp, #48	; 0x30
   3e690:	4639      	mov	r1, r7
   3e692:	4648      	mov	r0, r9
   3e694:	f7ff fb6a 	bl	3dd6c <__sprint_r>
   3e698:	2800      	cmp	r0, #0
   3e69a:	d0f5      	beq.n	3e688 <_vfiprintf_r+0x8b8>
   3e69c:	e7d1      	b.n	3e642 <_vfiprintf_r+0x872>
   3e69e:	ea54 0205 	orrs.w	r2, r4, r5
   3e6a2:	f8cd a010 	str.w	sl, [sp, #16]
   3e6a6:	f43f ada3 	beq.w	3e1f0 <_vfiprintf_r+0x420>
   3e6aa:	2b01      	cmp	r3, #1
   3e6ac:	f43f aeda 	beq.w	3e464 <_vfiprintf_r+0x694>
   3e6b0:	2b02      	cmp	r3, #2
   3e6b2:	f10d 08e0 	add.w	r8, sp, #224	; 0xe0
   3e6b6:	f43f af26 	beq.w	3e506 <_vfiprintf_r+0x736>
   3e6ba:	f004 0307 	and.w	r3, r4, #7
   3e6be:	08e4      	lsrs	r4, r4, #3
   3e6c0:	4642      	mov	r2, r8
   3e6c2:	ea44 7445 	orr.w	r4, r4, r5, lsl #29
   3e6c6:	3330      	adds	r3, #48	; 0x30
   3e6c8:	08ed      	lsrs	r5, r5, #3
   3e6ca:	f808 3d01 	strb.w	r3, [r8, #-1]!
   3e6ce:	ea54 0105 	orrs.w	r1, r4, r5
   3e6d2:	d1f2      	bne.n	3e6ba <_vfiprintf_r+0x8ea>
   3e6d4:	9904      	ldr	r1, [sp, #16]
   3e6d6:	07c8      	lsls	r0, r1, #31
   3e6d8:	d506      	bpl.n	3e6e8 <_vfiprintf_r+0x918>
   3e6da:	2b30      	cmp	r3, #48	; 0x30
   3e6dc:	d004      	beq.n	3e6e8 <_vfiprintf_r+0x918>
   3e6de:	2330      	movs	r3, #48	; 0x30
   3e6e0:	f808 3c01 	strb.w	r3, [r8, #-1]
   3e6e4:	f1a2 0802 	sub.w	r8, r2, #2
   3e6e8:	ab38      	add	r3, sp, #224	; 0xe0
   3e6ea:	9d00      	ldr	r5, [sp, #0]
   3e6ec:	f8dd a010 	ldr.w	sl, [sp, #16]
   3e6f0:	eba3 0308 	sub.w	r3, r3, r8
   3e6f4:	9300      	str	r3, [sp, #0]
   3e6f6:	e5b6      	b.n	3e266 <_vfiprintf_r+0x496>
   3e6f8:	00040942 	.word	0x00040942

0003e6fc <vfiprintf>:
   3e6fc:	4613      	mov	r3, r2
   3e6fe:	460a      	mov	r2, r1
   3e700:	4601      	mov	r1, r0
   3e702:	4802      	ldr	r0, [pc, #8]	; (3e70c <vfiprintf+0x10>)
   3e704:	6800      	ldr	r0, [r0, #0]
   3e706:	f7ff bb63 	b.w	3ddd0 <_vfiprintf_r>
   3e70a:	bf00      	nop
   3e70c:	20006000 	.word	0x20006000

0003e710 <__sbprintf>:
   3e710:	b570      	push	{r4, r5, r6, lr}
   3e712:	460c      	mov	r4, r1
   3e714:	8989      	ldrh	r1, [r1, #12]
   3e716:	f5ad 6d8e 	sub.w	sp, sp, #1136	; 0x470
   3e71a:	4606      	mov	r6, r0
   3e71c:	f021 0102 	bic.w	r1, r1, #2
   3e720:	a818      	add	r0, sp, #96	; 0x60
   3e722:	f8ad 1014 	strh.w	r1, [sp, #20]
   3e726:	6e61      	ldr	r1, [r4, #100]	; 0x64
   3e728:	911b      	str	r1, [sp, #108]	; 0x6c
   3e72a:	89e1      	ldrh	r1, [r4, #14]
   3e72c:	f8ad 1016 	strh.w	r1, [sp, #22]
   3e730:	69e1      	ldr	r1, [r4, #28]
   3e732:	9109      	str	r1, [sp, #36]	; 0x24
   3e734:	6a61      	ldr	r1, [r4, #36]	; 0x24
   3e736:	910b      	str	r1, [sp, #44]	; 0x2c
   3e738:	a91c      	add	r1, sp, #112	; 0x70
   3e73a:	9102      	str	r1, [sp, #8]
   3e73c:	9106      	str	r1, [sp, #24]
   3e73e:	f44f 6180 	mov.w	r1, #1024	; 0x400
   3e742:	9104      	str	r1, [sp, #16]
   3e744:	9107      	str	r1, [sp, #28]
   3e746:	2100      	movs	r1, #0
   3e748:	e9cd 3200 	strd	r3, r2, [sp]
   3e74c:	9108      	str	r1, [sp, #32]
   3e74e:	f7fb f826 	bl	3979e <__retarget_lock_init_recursive>
   3e752:	a902      	add	r1, sp, #8
   3e754:	4630      	mov	r0, r6
   3e756:	e9dd 3200 	ldrd	r3, r2, [sp]
   3e75a:	f7ff fb39 	bl	3ddd0 <_vfiprintf_r>
   3e75e:	1e05      	subs	r5, r0, #0
   3e760:	db07      	blt.n	3e772 <__sbprintf+0x62>
   3e762:	a902      	add	r1, sp, #8
   3e764:	4630      	mov	r0, r6
   3e766:	f7fa fc05 	bl	38f74 <_fflush_r>
   3e76a:	2800      	cmp	r0, #0
   3e76c:	bf18      	it	ne
   3e76e:	f04f 35ff 	movne.w	r5, #4294967295	; 0xffffffff
   3e772:	f8bd 3014 	ldrh.w	r3, [sp, #20]
   3e776:	9818      	ldr	r0, [sp, #96]	; 0x60
   3e778:	065b      	lsls	r3, r3, #25
   3e77a:	bf42      	ittt	mi
   3e77c:	89a3      	ldrhmi	r3, [r4, #12]
   3e77e:	f043 0340 	orrmi.w	r3, r3, #64	; 0x40
   3e782:	81a3      	strhmi	r3, [r4, #12]
   3e784:	f7fb f80d 	bl	397a2 <__retarget_lock_close_recursive>
   3e788:	4628      	mov	r0, r5
   3e78a:	f50d 6d8e 	add.w	sp, sp, #1136	; 0x470
   3e78e:	bd70      	pop	{r4, r5, r6, pc}

0003e790 <_wctomb_r>:
   3e790:	b410      	push	{r4}
   3e792:	4c03      	ldr	r4, [pc, #12]	; (3e7a0 <_wctomb_r+0x10>)
   3e794:	f8d4 40e0 	ldr.w	r4, [r4, #224]	; 0xe0
   3e798:	46a4      	mov	ip, r4
   3e79a:	f85d 4b04 	ldr.w	r4, [sp], #4
   3e79e:	4760      	bx	ip
   3e7a0:	20006430 	.word	0x20006430

0003e7a4 <__ascii_wctomb>:
   3e7a4:	b149      	cbz	r1, 3e7ba <__ascii_wctomb+0x16>
   3e7a6:	2aff      	cmp	r2, #255	; 0xff
   3e7a8:	bf8d      	iteet	hi
   3e7aa:	238a      	movhi	r3, #138	; 0x8a
   3e7ac:	2001      	movls	r0, #1
   3e7ae:	700a      	strbls	r2, [r1, #0]
   3e7b0:	6003      	strhi	r3, [r0, #0]
   3e7b2:	bf88      	it	hi
   3e7b4:	f04f 30ff 	movhi.w	r0, #4294967295	; 0xffffffff
   3e7b8:	4770      	bx	lr
   3e7ba:	4608      	mov	r0, r1
   3e7bc:	4770      	bx	lr
   3e7be:	bf00      	nop

0003e7c0 <__assert_func>:
   3e7c0:	b51f      	push	{r0, r1, r2, r3, r4, lr}
   3e7c2:	4614      	mov	r4, r2
   3e7c4:	461a      	mov	r2, r3
   3e7c6:	4b09      	ldr	r3, [pc, #36]	; (3e7ec <__assert_func+0x2c>)
   3e7c8:	4605      	mov	r5, r0
   3e7ca:	681b      	ldr	r3, [r3, #0]
   3e7cc:	68d8      	ldr	r0, [r3, #12]
   3e7ce:	b14c      	cbz	r4, 3e7e4 <__assert_func+0x24>
   3e7d0:	4b07      	ldr	r3, [pc, #28]	; (3e7f0 <__assert_func+0x30>)
   3e7d2:	9100      	str	r1, [sp, #0]
   3e7d4:	4907      	ldr	r1, [pc, #28]	; (3e7f4 <__assert_func+0x34>)
   3e7d6:	e9cd 3401 	strd	r3, r4, [sp, #4]
   3e7da:	462b      	mov	r3, r5
   3e7dc:	f000 f8b5 	bl	3e94a <fiprintf>
   3e7e0:	f000 fd0e 	bl	3f200 <abort>
   3e7e4:	4b04      	ldr	r3, [pc, #16]	; (3e7f8 <__assert_func+0x38>)
   3e7e6:	461c      	mov	r4, r3
   3e7e8:	e7f3      	b.n	3e7d2 <__assert_func+0x12>
   3e7ea:	bf00      	nop
   3e7ec:	20006000 	.word	0x20006000
   3e7f0:	00040962 	.word	0x00040962
   3e7f4:	0004096f 	.word	0x0004096f
   3e7f8:	0004099d 	.word	0x0004099d

0003e7fc <__assert>:
   3e7fc:	b508      	push	{r3, lr}
   3e7fe:	4613      	mov	r3, r2
   3e800:	2200      	movs	r2, #0
   3e802:	f7ff ffdd 	bl	3e7c0 <__assert_func>
   3e806:	bf00      	nop

0003e808 <_calloc_r>:
   3e808:	b538      	push	{r3, r4, r5, lr}
   3e80a:	fba1 1502 	umull	r1, r5, r1, r2
   3e80e:	b92d      	cbnz	r5, 3e81c <_calloc_r+0x14>
   3e810:	f7fb f856 	bl	398c0 <_malloc_r>
   3e814:	4604      	mov	r4, r0
   3e816:	b938      	cbnz	r0, 3e828 <_calloc_r+0x20>
   3e818:	4620      	mov	r0, r4
   3e81a:	bd38      	pop	{r3, r4, r5, pc}
   3e81c:	f7fa fb16 	bl	38e4c <__errno>
   3e820:	230c      	movs	r3, #12
   3e822:	2400      	movs	r4, #0
   3e824:	6003      	str	r3, [r0, #0]
   3e826:	e7f7      	b.n	3e818 <_calloc_r+0x10>
   3e828:	f850 2c04 	ldr.w	r2, [r0, #-4]
   3e82c:	f022 0203 	bic.w	r2, r2, #3
   3e830:	3a04      	subs	r2, #4
   3e832:	2a24      	cmp	r2, #36	; 0x24
   3e834:	d819      	bhi.n	3e86a <_calloc_r+0x62>
   3e836:	2a13      	cmp	r2, #19
   3e838:	d915      	bls.n	3e866 <_calloc_r+0x5e>
   3e83a:	2a1b      	cmp	r2, #27
   3e83c:	e9c0 5500 	strd	r5, r5, [r0]
   3e840:	d806      	bhi.n	3e850 <_calloc_r+0x48>
   3e842:	f100 0308 	add.w	r3, r0, #8
   3e846:	2200      	movs	r2, #0
   3e848:	e9c3 2200 	strd	r2, r2, [r3]
   3e84c:	609a      	str	r2, [r3, #8]
   3e84e:	e7e3      	b.n	3e818 <_calloc_r+0x10>
   3e850:	2a24      	cmp	r2, #36	; 0x24
   3e852:	e9c0 5502 	strd	r5, r5, [r0, #8]
   3e856:	bf11      	iteee	ne
   3e858:	f100 0310 	addne.w	r3, r0, #16
   3e85c:	f100 0318 	addeq.w	r3, r0, #24
   3e860:	6105      	streq	r5, [r0, #16]
   3e862:	6145      	streq	r5, [r0, #20]
   3e864:	e7ef      	b.n	3e846 <_calloc_r+0x3e>
   3e866:	4603      	mov	r3, r0
   3e868:	e7ed      	b.n	3e846 <_calloc_r+0x3e>
   3e86a:	4629      	mov	r1, r5
   3e86c:	f7fb fbc0 	bl	39ff0 <memset>
   3e870:	e7d2      	b.n	3e818 <_calloc_r+0x10>
	...

0003e874 <_fclose_r>:
   3e874:	b570      	push	{r4, r5, r6, lr}
   3e876:	4606      	mov	r6, r0
   3e878:	460c      	mov	r4, r1
   3e87a:	b911      	cbnz	r1, 3e882 <_fclose_r+0xe>
   3e87c:	2500      	movs	r5, #0
   3e87e:	4628      	mov	r0, r5
   3e880:	bd70      	pop	{r4, r5, r6, pc}
   3e882:	b118      	cbz	r0, 3e88c <_fclose_r+0x18>
   3e884:	6b83      	ldr	r3, [r0, #56]	; 0x38
   3e886:	b90b      	cbnz	r3, 3e88c <_fclose_r+0x18>
   3e888:	f7fa fc24 	bl	390d4 <__sinit>
   3e88c:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3e88e:	07d8      	lsls	r0, r3, #31
   3e890:	d405      	bmi.n	3e89e <_fclose_r+0x2a>
   3e892:	89a3      	ldrh	r3, [r4, #12]
   3e894:	0599      	lsls	r1, r3, #22
   3e896:	d402      	bmi.n	3e89e <_fclose_r+0x2a>
   3e898:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3e89a:	f7fa ff84 	bl	397a6 <__retarget_lock_acquire_recursive>
   3e89e:	f9b4 300c 	ldrsh.w	r3, [r4, #12]
   3e8a2:	b93b      	cbnz	r3, 3e8b4 <_fclose_r+0x40>
   3e8a4:	6e65      	ldr	r5, [r4, #100]	; 0x64
   3e8a6:	f015 0501 	ands.w	r5, r5, #1
   3e8aa:	d1e7      	bne.n	3e87c <_fclose_r+0x8>
   3e8ac:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3e8ae:	f7fa ff80 	bl	397b2 <__retarget_lock_release_recursive>
   3e8b2:	e7e4      	b.n	3e87e <_fclose_r+0xa>
   3e8b4:	4621      	mov	r1, r4
   3e8b6:	4630      	mov	r0, r6
   3e8b8:	f7fa face 	bl	38e58 <__sflush_r>
   3e8bc:	6ae3      	ldr	r3, [r4, #44]	; 0x2c
   3e8be:	4605      	mov	r5, r0
   3e8c0:	b133      	cbz	r3, 3e8d0 <_fclose_r+0x5c>
   3e8c2:	69e1      	ldr	r1, [r4, #28]
   3e8c4:	4630      	mov	r0, r6
   3e8c6:	4798      	blx	r3
   3e8c8:	2800      	cmp	r0, #0
   3e8ca:	bfb8      	it	lt
   3e8cc:	f04f 35ff 	movlt.w	r5, #4294967295	; 0xffffffff
   3e8d0:	89a3      	ldrh	r3, [r4, #12]
   3e8d2:	061a      	lsls	r2, r3, #24
   3e8d4:	d503      	bpl.n	3e8de <_fclose_r+0x6a>
   3e8d6:	6921      	ldr	r1, [r4, #16]
   3e8d8:	4630      	mov	r0, r6
   3e8da:	f7fa fcdd 	bl	39298 <_free_r>
   3e8de:	6b21      	ldr	r1, [r4, #48]	; 0x30
   3e8e0:	b141      	cbz	r1, 3e8f4 <_fclose_r+0x80>
   3e8e2:	f104 0340 	add.w	r3, r4, #64	; 0x40
   3e8e6:	4299      	cmp	r1, r3
   3e8e8:	d002      	beq.n	3e8f0 <_fclose_r+0x7c>
   3e8ea:	4630      	mov	r0, r6
   3e8ec:	f7fa fcd4 	bl	39298 <_free_r>
   3e8f0:	2300      	movs	r3, #0
   3e8f2:	6323      	str	r3, [r4, #48]	; 0x30
   3e8f4:	6c61      	ldr	r1, [r4, #68]	; 0x44
   3e8f6:	b121      	cbz	r1, 3e902 <_fclose_r+0x8e>
   3e8f8:	4630      	mov	r0, r6
   3e8fa:	f7fa fccd 	bl	39298 <_free_r>
   3e8fe:	2300      	movs	r3, #0
   3e900:	6463      	str	r3, [r4, #68]	; 0x44
   3e902:	f7fa fbcf 	bl	390a4 <__sfp_lock_acquire>
   3e906:	2300      	movs	r3, #0
   3e908:	81a3      	strh	r3, [r4, #12]
   3e90a:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3e90c:	07db      	lsls	r3, r3, #31
   3e90e:	d402      	bmi.n	3e916 <_fclose_r+0xa2>
   3e910:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3e912:	f7fa ff4e 	bl	397b2 <__retarget_lock_release_recursive>
   3e916:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3e918:	f7fa ff43 	bl	397a2 <__retarget_lock_close_recursive>
   3e91c:	f7fa fbc8 	bl	390b0 <__sfp_lock_release>
   3e920:	e7ad      	b.n	3e87e <_fclose_r+0xa>

0003e922 <fclose>:
   3e922:	4b02      	ldr	r3, [pc, #8]	; (3e92c <fclose+0xa>)
   3e924:	4601      	mov	r1, r0
   3e926:	6818      	ldr	r0, [r3, #0]
   3e928:	f7ff bfa4 	b.w	3e874 <_fclose_r>
   3e92c:	20006000 	.word	0x20006000

0003e930 <_fiprintf_r>:
   3e930:	b40c      	push	{r2, r3}
   3e932:	b507      	push	{r0, r1, r2, lr}
   3e934:	ab04      	add	r3, sp, #16
   3e936:	f853 2b04 	ldr.w	r2, [r3], #4
   3e93a:	9301      	str	r3, [sp, #4]
   3e93c:	f7ff fa48 	bl	3ddd0 <_vfiprintf_r>
   3e940:	b003      	add	sp, #12
   3e942:	f85d eb04 	ldr.w	lr, [sp], #4
   3e946:	b002      	add	sp, #8
   3e948:	4770      	bx	lr

0003e94a <fiprintf>:
   3e94a:	b40e      	push	{r1, r2, r3}
   3e94c:	b503      	push	{r0, r1, lr}
   3e94e:	ab03      	add	r3, sp, #12
   3e950:	4601      	mov	r1, r0
   3e952:	4806      	ldr	r0, [pc, #24]	; (3e96c <fiprintf+0x22>)
   3e954:	f853 2b04 	ldr.w	r2, [r3], #4
   3e958:	6800      	ldr	r0, [r0, #0]
   3e95a:	9301      	str	r3, [sp, #4]
   3e95c:	f7ff fa38 	bl	3ddd0 <_vfiprintf_r>
   3e960:	b002      	add	sp, #8
   3e962:	f85d eb04 	ldr.w	lr, [sp], #4
   3e966:	b003      	add	sp, #12
   3e968:	4770      	bx	lr
   3e96a:	bf00      	nop
   3e96c:	20006000 	.word	0x20006000

0003e970 <__fputwc>:
   3e970:	e92d 43f7 	stmdb	sp!, {r0, r1, r2, r4, r5, r6, r7, r8, r9, lr}
   3e974:	4680      	mov	r8, r0
   3e976:	460e      	mov	r6, r1
   3e978:	4615      	mov	r5, r2
   3e97a:	f7fa ff01 	bl	39780 <__locale_mb_cur_max>
   3e97e:	2801      	cmp	r0, #1
   3e980:	d11c      	bne.n	3e9bc <__fputwc+0x4c>
   3e982:	1e73      	subs	r3, r6, #1
   3e984:	2bfe      	cmp	r3, #254	; 0xfe
   3e986:	d819      	bhi.n	3e9bc <__fputwc+0x4c>
   3e988:	4604      	mov	r4, r0
   3e98a:	f88d 6004 	strb.w	r6, [sp, #4]
   3e98e:	2700      	movs	r7, #0
   3e990:	f10d 0904 	add.w	r9, sp, #4
   3e994:	42a7      	cmp	r7, r4
   3e996:	d020      	beq.n	3e9da <__fputwc+0x6a>
   3e998:	68ab      	ldr	r3, [r5, #8]
   3e99a:	f817 1009 	ldrb.w	r1, [r7, r9]
   3e99e:	3b01      	subs	r3, #1
   3e9a0:	2b00      	cmp	r3, #0
   3e9a2:	60ab      	str	r3, [r5, #8]
   3e9a4:	da04      	bge.n	3e9b0 <__fputwc+0x40>
   3e9a6:	69aa      	ldr	r2, [r5, #24]
   3e9a8:	4293      	cmp	r3, r2
   3e9aa:	db1a      	blt.n	3e9e2 <__fputwc+0x72>
   3e9ac:	290a      	cmp	r1, #10
   3e9ae:	d018      	beq.n	3e9e2 <__fputwc+0x72>
   3e9b0:	682b      	ldr	r3, [r5, #0]
   3e9b2:	1c5a      	adds	r2, r3, #1
   3e9b4:	602a      	str	r2, [r5, #0]
   3e9b6:	7019      	strb	r1, [r3, #0]
   3e9b8:	3701      	adds	r7, #1
   3e9ba:	e7eb      	b.n	3e994 <__fputwc+0x24>
   3e9bc:	a901      	add	r1, sp, #4
   3e9be:	f105 035c 	add.w	r3, r5, #92	; 0x5c
   3e9c2:	4632      	mov	r2, r6
   3e9c4:	4640      	mov	r0, r8
   3e9c6:	f000 fbfb 	bl	3f1c0 <_wcrtomb_r>
   3e9ca:	1c41      	adds	r1, r0, #1
   3e9cc:	4604      	mov	r4, r0
   3e9ce:	d1de      	bne.n	3e98e <__fputwc+0x1e>
   3e9d0:	89ab      	ldrh	r3, [r5, #12]
   3e9d2:	4606      	mov	r6, r0
   3e9d4:	f043 0340 	orr.w	r3, r3, #64	; 0x40
   3e9d8:	81ab      	strh	r3, [r5, #12]
   3e9da:	4630      	mov	r0, r6
   3e9dc:	b003      	add	sp, #12
   3e9de:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
   3e9e2:	462a      	mov	r2, r5
   3e9e4:	4640      	mov	r0, r8
   3e9e6:	f7f9 fb05 	bl	37ff4 <__swbuf_r>
   3e9ea:	1c42      	adds	r2, r0, #1
   3e9ec:	d1e4      	bne.n	3e9b8 <__fputwc+0x48>
   3e9ee:	4606      	mov	r6, r0
   3e9f0:	e7f3      	b.n	3e9da <__fputwc+0x6a>

0003e9f2 <_fputwc_r>:
   3e9f2:	6e53      	ldr	r3, [r2, #100]	; 0x64
   3e9f4:	b537      	push	{r0, r1, r2, r4, r5, lr}
   3e9f6:	4614      	mov	r4, r2
   3e9f8:	07da      	lsls	r2, r3, #31
   3e9fa:	4605      	mov	r5, r0
   3e9fc:	d407      	bmi.n	3ea0e <_fputwc_r+0x1c>
   3e9fe:	89a3      	ldrh	r3, [r4, #12]
   3ea00:	059b      	lsls	r3, r3, #22
   3ea02:	d404      	bmi.n	3ea0e <_fputwc_r+0x1c>
   3ea04:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3ea06:	9101      	str	r1, [sp, #4]
   3ea08:	f7fa fecd 	bl	397a6 <__retarget_lock_acquire_recursive>
   3ea0c:	9901      	ldr	r1, [sp, #4]
   3ea0e:	f9b4 300c 	ldrsh.w	r3, [r4, #12]
   3ea12:	0498      	lsls	r0, r3, #18
   3ea14:	d406      	bmi.n	3ea24 <_fputwc_r+0x32>
   3ea16:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   3ea1a:	81a3      	strh	r3, [r4, #12]
   3ea1c:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3ea1e:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
   3ea22:	6663      	str	r3, [r4, #100]	; 0x64
   3ea24:	4622      	mov	r2, r4
   3ea26:	4628      	mov	r0, r5
   3ea28:	f7ff ffa2 	bl	3e970 <__fputwc>
   3ea2c:	6e63      	ldr	r3, [r4, #100]	; 0x64
   3ea2e:	4605      	mov	r5, r0
   3ea30:	07da      	lsls	r2, r3, #31
   3ea32:	d405      	bmi.n	3ea40 <_fputwc_r+0x4e>
   3ea34:	89a3      	ldrh	r3, [r4, #12]
   3ea36:	059b      	lsls	r3, r3, #22
   3ea38:	d402      	bmi.n	3ea40 <_fputwc_r+0x4e>
   3ea3a:	6da0      	ldr	r0, [r4, #88]	; 0x58
   3ea3c:	f7fa feb9 	bl	397b2 <__retarget_lock_release_recursive>
   3ea40:	4628      	mov	r0, r5
   3ea42:	b003      	add	sp, #12
   3ea44:	bd30      	pop	{r4, r5, pc}

0003ea46 <fputwc>:
   3ea46:	4b0a      	ldr	r3, [pc, #40]	; (3ea70 <fputwc+0x2a>)
   3ea48:	b537      	push	{r0, r1, r2, r4, r5, lr}
   3ea4a:	681c      	ldr	r4, [r3, #0]
   3ea4c:	4605      	mov	r5, r0
   3ea4e:	460a      	mov	r2, r1
   3ea50:	b134      	cbz	r4, 3ea60 <fputwc+0x1a>
   3ea52:	6ba3      	ldr	r3, [r4, #56]	; 0x38
   3ea54:	b923      	cbnz	r3, 3ea60 <fputwc+0x1a>
   3ea56:	4620      	mov	r0, r4
   3ea58:	9101      	str	r1, [sp, #4]
   3ea5a:	f7fa fb3b 	bl	390d4 <__sinit>
   3ea5e:	9a01      	ldr	r2, [sp, #4]
   3ea60:	4629      	mov	r1, r5
   3ea62:	4620      	mov	r0, r4
   3ea64:	b003      	add	sp, #12
   3ea66:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
   3ea6a:	f7ff bfc2 	b.w	3e9f2 <_fputwc_r>
   3ea6e:	bf00      	nop
   3ea70:	20006000 	.word	0x20006000

0003ea74 <_fstat_r>:
   3ea74:	b538      	push	{r3, r4, r5, lr}
   3ea76:	2300      	movs	r3, #0
   3ea78:	4d06      	ldr	r5, [pc, #24]	; (3ea94 <_fstat_r+0x20>)
   3ea7a:	4604      	mov	r4, r0
   3ea7c:	4608      	mov	r0, r1
   3ea7e:	4611      	mov	r1, r2
   3ea80:	602b      	str	r3, [r5, #0]
   3ea82:	f001 fc4f 	bl	40324 <_fstat>
   3ea86:	1c43      	adds	r3, r0, #1
   3ea88:	d102      	bne.n	3ea90 <_fstat_r+0x1c>
   3ea8a:	682b      	ldr	r3, [r5, #0]
   3ea8c:	b103      	cbz	r3, 3ea90 <_fstat_r+0x1c>
   3ea8e:	6023      	str	r3, [r4, #0]
   3ea90:	bd38      	pop	{r3, r4, r5, pc}
   3ea92:	bf00      	nop
   3ea94:	20006a0c 	.word	0x20006a0c

0003ea98 <rshift>:
   3ea98:	6903      	ldr	r3, [r0, #16]
   3ea9a:	114a      	asrs	r2, r1, #5
   3ea9c:	ebb3 1f61 	cmp.w	r3, r1, asr #5
   3eaa0:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
   3eaa4:	f100 0414 	add.w	r4, r0, #20
   3eaa8:	dd45      	ble.n	3eb36 <rshift+0x9e>
   3eaaa:	f011 011f 	ands.w	r1, r1, #31
   3eaae:	eb04 0683 	add.w	r6, r4, r3, lsl #2
   3eab2:	eb04 0582 	add.w	r5, r4, r2, lsl #2
   3eab6:	d10c      	bne.n	3ead2 <rshift+0x3a>
   3eab8:	f100 0710 	add.w	r7, r0, #16
   3eabc:	4629      	mov	r1, r5
   3eabe:	42b1      	cmp	r1, r6
   3eac0:	d334      	bcc.n	3eb2c <rshift+0x94>
   3eac2:	1a9b      	subs	r3, r3, r2
   3eac4:	1eea      	subs	r2, r5, #3
   3eac6:	009b      	lsls	r3, r3, #2
   3eac8:	4296      	cmp	r6, r2
   3eaca:	bf38      	it	cc
   3eacc:	2300      	movcc	r3, #0
   3eace:	4423      	add	r3, r4
   3ead0:	e015      	b.n	3eafe <rshift+0x66>
   3ead2:	f854 7022 	ldr.w	r7, [r4, r2, lsl #2]
   3ead6:	f1c1 0820 	rsb	r8, r1, #32
   3eada:	f105 0e04 	add.w	lr, r5, #4
   3eade:	46a1      	mov	r9, r4
   3eae0:	40cf      	lsrs	r7, r1
   3eae2:	4576      	cmp	r6, lr
   3eae4:	46f4      	mov	ip, lr
   3eae6:	d815      	bhi.n	3eb14 <rshift+0x7c>
   3eae8:	1a9a      	subs	r2, r3, r2
   3eaea:	3501      	adds	r5, #1
   3eaec:	0092      	lsls	r2, r2, #2
   3eaee:	3a04      	subs	r2, #4
   3eaf0:	42ae      	cmp	r6, r5
   3eaf2:	bf38      	it	cc
   3eaf4:	2200      	movcc	r2, #0
   3eaf6:	18a3      	adds	r3, r4, r2
   3eaf8:	50a7      	str	r7, [r4, r2]
   3eafa:	b107      	cbz	r7, 3eafe <rshift+0x66>
   3eafc:	3304      	adds	r3, #4
   3eafe:	1b1a      	subs	r2, r3, r4
   3eb00:	42a3      	cmp	r3, r4
   3eb02:	ea4f 02a2 	mov.w	r2, r2, asr #2
   3eb06:	bf08      	it	eq
   3eb08:	2300      	moveq	r3, #0
   3eb0a:	6102      	str	r2, [r0, #16]
   3eb0c:	bf08      	it	eq
   3eb0e:	6143      	streq	r3, [r0, #20]
   3eb10:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
   3eb14:	f8dc c000 	ldr.w	ip, [ip]
   3eb18:	fa0c fc08 	lsl.w	ip, ip, r8
   3eb1c:	ea4c 0707 	orr.w	r7, ip, r7
   3eb20:	f849 7b04 	str.w	r7, [r9], #4
   3eb24:	f85e 7b04 	ldr.w	r7, [lr], #4
   3eb28:	40cf      	lsrs	r7, r1
   3eb2a:	e7da      	b.n	3eae2 <rshift+0x4a>
   3eb2c:	f851 cb04 	ldr.w	ip, [r1], #4
   3eb30:	f847 cf04 	str.w	ip, [r7, #4]!
   3eb34:	e7c3      	b.n	3eabe <rshift+0x26>
   3eb36:	4623      	mov	r3, r4
   3eb38:	e7e1      	b.n	3eafe <rshift+0x66>

0003eb3a <__hexdig_fun>:
   3eb3a:	f1a0 0330 	sub.w	r3, r0, #48	; 0x30
   3eb3e:	2b09      	cmp	r3, #9
   3eb40:	d802      	bhi.n	3eb48 <__hexdig_fun+0xe>
   3eb42:	3820      	subs	r0, #32
   3eb44:	b2c0      	uxtb	r0, r0
   3eb46:	4770      	bx	lr
   3eb48:	f1a0 0361 	sub.w	r3, r0, #97	; 0x61
   3eb4c:	2b05      	cmp	r3, #5
   3eb4e:	d801      	bhi.n	3eb54 <__hexdig_fun+0x1a>
   3eb50:	3847      	subs	r0, #71	; 0x47
   3eb52:	e7f7      	b.n	3eb44 <__hexdig_fun+0xa>
   3eb54:	f1a0 0341 	sub.w	r3, r0, #65	; 0x41
   3eb58:	2b05      	cmp	r3, #5
   3eb5a:	d801      	bhi.n	3eb60 <__hexdig_fun+0x26>
   3eb5c:	3827      	subs	r0, #39	; 0x27
   3eb5e:	e7f1      	b.n	3eb44 <__hexdig_fun+0xa>
   3eb60:	2000      	movs	r0, #0
   3eb62:	4770      	bx	lr

0003eb64 <__gethex>:
   3eb64:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3eb68:	ed2d 8b02 	vpush	{d8}
   3eb6c:	b089      	sub	sp, #36	; 0x24
   3eb6e:	ee08 0a10 	vmov	s16, r0
   3eb72:	468b      	mov	fp, r1
   3eb74:	4690      	mov	r8, r2
   3eb76:	9304      	str	r3, [sp, #16]
   3eb78:	4bb2      	ldr	r3, [pc, #712]	; (3ee44 <__gethex+0x2e0>)
   3eb7a:	681b      	ldr	r3, [r3, #0]
   3eb7c:	4618      	mov	r0, r3
   3eb7e:	9301      	str	r3, [sp, #4]
   3eb80:	f7f5 f8d6 	bl	33d30 <strlen>
   3eb84:	9b01      	ldr	r3, [sp, #4]
   3eb86:	f8db 2000 	ldr.w	r2, [fp]
   3eb8a:	4682      	mov	sl, r0
   3eb8c:	4403      	add	r3, r0
   3eb8e:	f813 3c01 	ldrb.w	r3, [r3, #-1]
   3eb92:	9305      	str	r3, [sp, #20]
   3eb94:	1c93      	adds	r3, r2, #2
   3eb96:	f1c2 22ff 	rsb	r2, r2, #4278255360	; 0xff00ff00
   3eb9a:	f502 027f 	add.w	r2, r2, #16711680	; 0xff0000
   3eb9e:	32fe      	adds	r2, #254	; 0xfe
   3eba0:	18d1      	adds	r1, r2, r3
   3eba2:	461f      	mov	r7, r3
   3eba4:	f813 0b01 	ldrb.w	r0, [r3], #1
   3eba8:	9100      	str	r1, [sp, #0]
   3ebaa:	2830      	cmp	r0, #48	; 0x30
   3ebac:	d0f8      	beq.n	3eba0 <__gethex+0x3c>
   3ebae:	f7ff ffc4 	bl	3eb3a <__hexdig_fun>
   3ebb2:	4604      	mov	r4, r0
   3ebb4:	2800      	cmp	r0, #0
   3ebb6:	d139      	bne.n	3ec2c <__gethex+0xc8>
   3ebb8:	4652      	mov	r2, sl
   3ebba:	9901      	ldr	r1, [sp, #4]
   3ebbc:	4638      	mov	r0, r7
   3ebbe:	f7f5 f906 	bl	33dce <strncmp>
   3ebc2:	4605      	mov	r5, r0
   3ebc4:	2800      	cmp	r0, #0
   3ebc6:	d167      	bne.n	3ec98 <__gethex+0x134>
   3ebc8:	f817 000a 	ldrb.w	r0, [r7, sl]
   3ebcc:	eb07 060a 	add.w	r6, r7, sl
   3ebd0:	f7ff ffb3 	bl	3eb3a <__hexdig_fun>
   3ebd4:	2800      	cmp	r0, #0
   3ebd6:	d061      	beq.n	3ec9c <__gethex+0x138>
   3ebd8:	4633      	mov	r3, r6
   3ebda:	7818      	ldrb	r0, [r3, #0]
   3ebdc:	461f      	mov	r7, r3
   3ebde:	3301      	adds	r3, #1
   3ebe0:	2830      	cmp	r0, #48	; 0x30
   3ebe2:	d0fa      	beq.n	3ebda <__gethex+0x76>
   3ebe4:	f7ff ffa9 	bl	3eb3a <__hexdig_fun>
   3ebe8:	fab0 f480 	clz	r4, r0
   3ebec:	2301      	movs	r3, #1
   3ebee:	4635      	mov	r5, r6
   3ebf0:	0964      	lsrs	r4, r4, #5
   3ebf2:	9300      	str	r3, [sp, #0]
   3ebf4:	463a      	mov	r2, r7
   3ebf6:	4616      	mov	r6, r2
   3ebf8:	3201      	adds	r2, #1
   3ebfa:	7830      	ldrb	r0, [r6, #0]
   3ebfc:	f7ff ff9d 	bl	3eb3a <__hexdig_fun>
   3ec00:	2800      	cmp	r0, #0
   3ec02:	d1f8      	bne.n	3ebf6 <__gethex+0x92>
   3ec04:	4652      	mov	r2, sl
   3ec06:	9901      	ldr	r1, [sp, #4]
   3ec08:	4630      	mov	r0, r6
   3ec0a:	f7f5 f8e0 	bl	33dce <strncmp>
   3ec0e:	b980      	cbnz	r0, 3ec32 <__gethex+0xce>
   3ec10:	b94d      	cbnz	r5, 3ec26 <__gethex+0xc2>
   3ec12:	eb06 050a 	add.w	r5, r6, sl
   3ec16:	462a      	mov	r2, r5
   3ec18:	4616      	mov	r6, r2
   3ec1a:	3201      	adds	r2, #1
   3ec1c:	7830      	ldrb	r0, [r6, #0]
   3ec1e:	f7ff ff8c 	bl	3eb3a <__hexdig_fun>
   3ec22:	2800      	cmp	r0, #0
   3ec24:	d1f8      	bne.n	3ec18 <__gethex+0xb4>
   3ec26:	1bad      	subs	r5, r5, r6
   3ec28:	00ad      	lsls	r5, r5, #2
   3ec2a:	e004      	b.n	3ec36 <__gethex+0xd2>
   3ec2c:	2400      	movs	r4, #0
   3ec2e:	4625      	mov	r5, r4
   3ec30:	e7e0      	b.n	3ebf4 <__gethex+0x90>
   3ec32:	2d00      	cmp	r5, #0
   3ec34:	d1f7      	bne.n	3ec26 <__gethex+0xc2>
   3ec36:	7833      	ldrb	r3, [r6, #0]
   3ec38:	f003 03df 	and.w	r3, r3, #223	; 0xdf
   3ec3c:	2b50      	cmp	r3, #80	; 0x50
   3ec3e:	d13b      	bne.n	3ecb8 <__gethex+0x154>
   3ec40:	7873      	ldrb	r3, [r6, #1]
   3ec42:	2b2b      	cmp	r3, #43	; 0x2b
   3ec44:	d02c      	beq.n	3eca0 <__gethex+0x13c>
   3ec46:	2b2d      	cmp	r3, #45	; 0x2d
   3ec48:	d02e      	beq.n	3eca8 <__gethex+0x144>
   3ec4a:	1c71      	adds	r1, r6, #1
   3ec4c:	f04f 0900 	mov.w	r9, #0
   3ec50:	7808      	ldrb	r0, [r1, #0]
   3ec52:	f7ff ff72 	bl	3eb3a <__hexdig_fun>
   3ec56:	1e43      	subs	r3, r0, #1
   3ec58:	b2db      	uxtb	r3, r3
   3ec5a:	2b18      	cmp	r3, #24
   3ec5c:	d82c      	bhi.n	3ecb8 <__gethex+0x154>
   3ec5e:	f1a0 0210 	sub.w	r2, r0, #16
   3ec62:	f811 0f01 	ldrb.w	r0, [r1, #1]!
   3ec66:	f7ff ff68 	bl	3eb3a <__hexdig_fun>
   3ec6a:	1e43      	subs	r3, r0, #1
   3ec6c:	b2db      	uxtb	r3, r3
   3ec6e:	2b18      	cmp	r3, #24
   3ec70:	d91d      	bls.n	3ecae <__gethex+0x14a>
   3ec72:	f1b9 0f00 	cmp.w	r9, #0
   3ec76:	d000      	beq.n	3ec7a <__gethex+0x116>
   3ec78:	4252      	negs	r2, r2
   3ec7a:	4415      	add	r5, r2
   3ec7c:	f8cb 1000 	str.w	r1, [fp]
   3ec80:	b1e4      	cbz	r4, 3ecbc <__gethex+0x158>
   3ec82:	9b00      	ldr	r3, [sp, #0]
   3ec84:	2b00      	cmp	r3, #0
   3ec86:	bf14      	ite	ne
   3ec88:	2700      	movne	r7, #0
   3ec8a:	2706      	moveq	r7, #6
   3ec8c:	4638      	mov	r0, r7
   3ec8e:	b009      	add	sp, #36	; 0x24
   3ec90:	ecbd 8b02 	vpop	{d8}
   3ec94:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   3ec98:	463e      	mov	r6, r7
   3ec9a:	4625      	mov	r5, r4
   3ec9c:	2401      	movs	r4, #1
   3ec9e:	e7ca      	b.n	3ec36 <__gethex+0xd2>
   3eca0:	f04f 0900 	mov.w	r9, #0
   3eca4:	1cb1      	adds	r1, r6, #2
   3eca6:	e7d3      	b.n	3ec50 <__gethex+0xec>
   3eca8:	f04f 0901 	mov.w	r9, #1
   3ecac:	e7fa      	b.n	3eca4 <__gethex+0x140>
   3ecae:	230a      	movs	r3, #10
   3ecb0:	fb03 0202 	mla	r2, r3, r2, r0
   3ecb4:	3a10      	subs	r2, #16
   3ecb6:	e7d4      	b.n	3ec62 <__gethex+0xfe>
   3ecb8:	4631      	mov	r1, r6
   3ecba:	e7df      	b.n	3ec7c <__gethex+0x118>
   3ecbc:	1bf3      	subs	r3, r6, r7
   3ecbe:	4621      	mov	r1, r4
   3ecc0:	3b01      	subs	r3, #1
   3ecc2:	2b07      	cmp	r3, #7
   3ecc4:	dc0b      	bgt.n	3ecde <__gethex+0x17a>
   3ecc6:	ee18 0a10 	vmov	r0, s16
   3ecca:	f7fb f9d5 	bl	3a078 <_Balloc>
   3ecce:	4604      	mov	r4, r0
   3ecd0:	b940      	cbnz	r0, 3ece4 <__gethex+0x180>
   3ecd2:	4b5d      	ldr	r3, [pc, #372]	; (3ee48 <__gethex+0x2e4>)
   3ecd4:	4602      	mov	r2, r0
   3ecd6:	21de      	movs	r1, #222	; 0xde
   3ecd8:	485c      	ldr	r0, [pc, #368]	; (3ee4c <__gethex+0x2e8>)
   3ecda:	f7ff fd71 	bl	3e7c0 <__assert_func>
   3ecde:	3101      	adds	r1, #1
   3ece0:	105b      	asrs	r3, r3, #1
   3ece2:	e7ee      	b.n	3ecc2 <__gethex+0x15e>
   3ece4:	f100 0914 	add.w	r9, r0, #20
   3ece8:	f04f 0b00 	mov.w	fp, #0
   3ecec:	f1ca 0301 	rsb	r3, sl, #1
   3ecf0:	f8cd 9008 	str.w	r9, [sp, #8]
   3ecf4:	f8cd b000 	str.w	fp, [sp]
   3ecf8:	9306      	str	r3, [sp, #24]
   3ecfa:	42b7      	cmp	r7, r6
   3ecfc:	d340      	bcc.n	3ed80 <__gethex+0x21c>
   3ecfe:	9802      	ldr	r0, [sp, #8]
   3ed00:	9b00      	ldr	r3, [sp, #0]
   3ed02:	f840 3b04 	str.w	r3, [r0], #4
   3ed06:	eba0 0009 	sub.w	r0, r0, r9
   3ed0a:	1080      	asrs	r0, r0, #2
   3ed0c:	0146      	lsls	r6, r0, #5
   3ed0e:	6120      	str	r0, [r4, #16]
   3ed10:	4618      	mov	r0, r3
   3ed12:	f7fb fa71 	bl	3a1f8 <__hi0bits>
   3ed16:	1a30      	subs	r0, r6, r0
   3ed18:	f8d8 6000 	ldr.w	r6, [r8]
   3ed1c:	42b0      	cmp	r0, r6
   3ed1e:	dd63      	ble.n	3ede8 <__gethex+0x284>
   3ed20:	1b87      	subs	r7, r0, r6
   3ed22:	4620      	mov	r0, r4
   3ed24:	4639      	mov	r1, r7
   3ed26:	f7fb fe18 	bl	3a95a <__any_on>
   3ed2a:	4682      	mov	sl, r0
   3ed2c:	b1a8      	cbz	r0, 3ed5a <__gethex+0x1f6>
   3ed2e:	1e7b      	subs	r3, r7, #1
   3ed30:	f04f 0a01 	mov.w	sl, #1
   3ed34:	1159      	asrs	r1, r3, #5
   3ed36:	f003 021f 	and.w	r2, r3, #31
   3ed3a:	f859 1021 	ldr.w	r1, [r9, r1, lsl #2]
   3ed3e:	fa0a f202 	lsl.w	r2, sl, r2
   3ed42:	420a      	tst	r2, r1
   3ed44:	d009      	beq.n	3ed5a <__gethex+0x1f6>
   3ed46:	4553      	cmp	r3, sl
   3ed48:	dd05      	ble.n	3ed56 <__gethex+0x1f2>
   3ed4a:	1eb9      	subs	r1, r7, #2
   3ed4c:	4620      	mov	r0, r4
   3ed4e:	f7fb fe04 	bl	3a95a <__any_on>
   3ed52:	2800      	cmp	r0, #0
   3ed54:	d145      	bne.n	3ede2 <__gethex+0x27e>
   3ed56:	f04f 0a02 	mov.w	sl, #2
   3ed5a:	443d      	add	r5, r7
   3ed5c:	4639      	mov	r1, r7
   3ed5e:	4620      	mov	r0, r4
   3ed60:	f7ff fe9a 	bl	3ea98 <rshift>
   3ed64:	f8d8 3008 	ldr.w	r3, [r8, #8]
   3ed68:	42ab      	cmp	r3, r5
   3ed6a:	da4c      	bge.n	3ee06 <__gethex+0x2a2>
   3ed6c:	4621      	mov	r1, r4
   3ed6e:	ee18 0a10 	vmov	r0, s16
   3ed72:	f7fb f9a6 	bl	3a0c2 <_Bfree>
   3ed76:	2300      	movs	r3, #0
   3ed78:	9a14      	ldr	r2, [sp, #80]	; 0x50
   3ed7a:	27a3      	movs	r7, #163	; 0xa3
   3ed7c:	6013      	str	r3, [r2, #0]
   3ed7e:	e785      	b.n	3ec8c <__gethex+0x128>
   3ed80:	1e73      	subs	r3, r6, #1
   3ed82:	9a05      	ldr	r2, [sp, #20]
   3ed84:	9303      	str	r3, [sp, #12]
   3ed86:	f816 3c01 	ldrb.w	r3, [r6, #-1]
   3ed8a:	4293      	cmp	r3, r2
   3ed8c:	d019      	beq.n	3edc2 <__gethex+0x25e>
   3ed8e:	f1bb 0f20 	cmp.w	fp, #32
   3ed92:	d107      	bne.n	3eda4 <__gethex+0x240>
   3ed94:	9b02      	ldr	r3, [sp, #8]
   3ed96:	9a00      	ldr	r2, [sp, #0]
   3ed98:	f843 2b04 	str.w	r2, [r3], #4
   3ed9c:	9302      	str	r3, [sp, #8]
   3ed9e:	2300      	movs	r3, #0
   3eda0:	469b      	mov	fp, r3
   3eda2:	9300      	str	r3, [sp, #0]
   3eda4:	f816 0c01 	ldrb.w	r0, [r6, #-1]
   3eda8:	f7ff fec7 	bl	3eb3a <__hexdig_fun>
   3edac:	f000 000f 	and.w	r0, r0, #15
   3edb0:	9b00      	ldr	r3, [sp, #0]
   3edb2:	fa00 f00b 	lsl.w	r0, r0, fp
   3edb6:	f10b 0b04 	add.w	fp, fp, #4
   3edba:	4303      	orrs	r3, r0
   3edbc:	9300      	str	r3, [sp, #0]
   3edbe:	9b03      	ldr	r3, [sp, #12]
   3edc0:	e00d      	b.n	3edde <__gethex+0x27a>
   3edc2:	9b03      	ldr	r3, [sp, #12]
   3edc4:	9a06      	ldr	r2, [sp, #24]
   3edc6:	4413      	add	r3, r2
   3edc8:	42bb      	cmp	r3, r7
   3edca:	d3e0      	bcc.n	3ed8e <__gethex+0x22a>
   3edcc:	4618      	mov	r0, r3
   3edce:	4652      	mov	r2, sl
   3edd0:	9901      	ldr	r1, [sp, #4]
   3edd2:	9307      	str	r3, [sp, #28]
   3edd4:	f7f4 fffb 	bl	33dce <strncmp>
   3edd8:	9b07      	ldr	r3, [sp, #28]
   3edda:	2800      	cmp	r0, #0
   3eddc:	d1d7      	bne.n	3ed8e <__gethex+0x22a>
   3edde:	461e      	mov	r6, r3
   3ede0:	e78b      	b.n	3ecfa <__gethex+0x196>
   3ede2:	f04f 0a03 	mov.w	sl, #3
   3ede6:	e7b8      	b.n	3ed5a <__gethex+0x1f6>
   3ede8:	da0a      	bge.n	3ee00 <__gethex+0x29c>
   3edea:	1a37      	subs	r7, r6, r0
   3edec:	4621      	mov	r1, r4
   3edee:	ee18 0a10 	vmov	r0, s16
   3edf2:	463a      	mov	r2, r7
   3edf4:	1bed      	subs	r5, r5, r7
   3edf6:	f7fb fb4b 	bl	3a490 <__lshift>
   3edfa:	4604      	mov	r4, r0
   3edfc:	f100 0914 	add.w	r9, r0, #20
   3ee00:	f04f 0a00 	mov.w	sl, #0
   3ee04:	e7ae      	b.n	3ed64 <__gethex+0x200>
   3ee06:	f8d8 0004 	ldr.w	r0, [r8, #4]
   3ee0a:	42a8      	cmp	r0, r5
   3ee0c:	dd72      	ble.n	3eef4 <__gethex+0x390>
   3ee0e:	1b45      	subs	r5, r0, r5
   3ee10:	42ae      	cmp	r6, r5
   3ee12:	dc35      	bgt.n	3ee80 <__gethex+0x31c>
   3ee14:	f8d8 300c 	ldr.w	r3, [r8, #12]
   3ee18:	2b02      	cmp	r3, #2
   3ee1a:	d029      	beq.n	3ee70 <__gethex+0x30c>
   3ee1c:	2b03      	cmp	r3, #3
   3ee1e:	d02b      	beq.n	3ee78 <__gethex+0x314>
   3ee20:	2b01      	cmp	r3, #1
   3ee22:	d11b      	bne.n	3ee5c <__gethex+0x2f8>
   3ee24:	42ae      	cmp	r6, r5
   3ee26:	d119      	bne.n	3ee5c <__gethex+0x2f8>
   3ee28:	2e01      	cmp	r6, #1
   3ee2a:	d111      	bne.n	3ee50 <__gethex+0x2ec>
   3ee2c:	f8d8 3004 	ldr.w	r3, [r8, #4]
   3ee30:	2762      	movs	r7, #98	; 0x62
   3ee32:	9a04      	ldr	r2, [sp, #16]
   3ee34:	6013      	str	r3, [r2, #0]
   3ee36:	2301      	movs	r3, #1
   3ee38:	6123      	str	r3, [r4, #16]
   3ee3a:	f8c9 3000 	str.w	r3, [r9]
   3ee3e:	9b14      	ldr	r3, [sp, #80]	; 0x50
   3ee40:	601c      	str	r4, [r3, #0]
   3ee42:	e723      	b.n	3ec8c <__gethex+0x128>
   3ee44:	00040a1c 	.word	0x00040a1c
   3ee48:	00040678 	.word	0x00040678
   3ee4c:	0004099e 	.word	0x0004099e
   3ee50:	1e71      	subs	r1, r6, #1
   3ee52:	4620      	mov	r0, r4
   3ee54:	f7fb fd81 	bl	3a95a <__any_on>
   3ee58:	2800      	cmp	r0, #0
   3ee5a:	d1e7      	bne.n	3ee2c <__gethex+0x2c8>
   3ee5c:	4621      	mov	r1, r4
   3ee5e:	ee18 0a10 	vmov	r0, s16
   3ee62:	f7fb f92e 	bl	3a0c2 <_Bfree>
   3ee66:	2300      	movs	r3, #0
   3ee68:	9a14      	ldr	r2, [sp, #80]	; 0x50
   3ee6a:	2750      	movs	r7, #80	; 0x50
   3ee6c:	6013      	str	r3, [r2, #0]
   3ee6e:	e70d      	b.n	3ec8c <__gethex+0x128>
   3ee70:	9b15      	ldr	r3, [sp, #84]	; 0x54
   3ee72:	2b00      	cmp	r3, #0
   3ee74:	d1f2      	bne.n	3ee5c <__gethex+0x2f8>
   3ee76:	e7d9      	b.n	3ee2c <__gethex+0x2c8>
   3ee78:	9b15      	ldr	r3, [sp, #84]	; 0x54
   3ee7a:	2b00      	cmp	r3, #0
   3ee7c:	d1d6      	bne.n	3ee2c <__gethex+0x2c8>
   3ee7e:	e7ed      	b.n	3ee5c <__gethex+0x2f8>
   3ee80:	1e6f      	subs	r7, r5, #1
   3ee82:	f1ba 0f00 	cmp.w	sl, #0
   3ee86:	d132      	bne.n	3eeee <__gethex+0x38a>
   3ee88:	b127      	cbz	r7, 3ee94 <__gethex+0x330>
   3ee8a:	4639      	mov	r1, r7
   3ee8c:	4620      	mov	r0, r4
   3ee8e:	f7fb fd64 	bl	3a95a <__any_on>
   3ee92:	4682      	mov	sl, r0
   3ee94:	117b      	asrs	r3, r7, #5
   3ee96:	2101      	movs	r1, #1
   3ee98:	f007 071f 	and.w	r7, r7, #31
   3ee9c:	4620      	mov	r0, r4
   3ee9e:	f859 3023 	ldr.w	r3, [r9, r3, lsl #2]
   3eea2:	1b76      	subs	r6, r6, r5
   3eea4:	fa01 f707 	lsl.w	r7, r1, r7
   3eea8:	4629      	mov	r1, r5
   3eeaa:	421f      	tst	r7, r3
   3eeac:	f04f 0702 	mov.w	r7, #2
   3eeb0:	bf18      	it	ne
   3eeb2:	f04a 0a02 	orrne.w	sl, sl, #2
   3eeb6:	f7ff fdef 	bl	3ea98 <rshift>
   3eeba:	f8d8 5004 	ldr.w	r5, [r8, #4]
   3eebe:	f1ba 0f00 	cmp.w	sl, #0
   3eec2:	d048      	beq.n	3ef56 <__gethex+0x3f2>
   3eec4:	f8d8 300c 	ldr.w	r3, [r8, #12]
   3eec8:	2b02      	cmp	r3, #2
   3eeca:	d015      	beq.n	3eef8 <__gethex+0x394>
   3eecc:	2b03      	cmp	r3, #3
   3eece:	d017      	beq.n	3ef00 <__gethex+0x39c>
   3eed0:	2b01      	cmp	r3, #1
   3eed2:	d109      	bne.n	3eee8 <__gethex+0x384>
   3eed4:	f01a 0f02 	tst.w	sl, #2
   3eed8:	d006      	beq.n	3eee8 <__gethex+0x384>
   3eeda:	f8d9 0000 	ldr.w	r0, [r9]
   3eede:	ea4a 0a00 	orr.w	sl, sl, r0
   3eee2:	f01a 0f01 	tst.w	sl, #1
   3eee6:	d10e      	bne.n	3ef06 <__gethex+0x3a2>
   3eee8:	f047 0710 	orr.w	r7, r7, #16
   3eeec:	e033      	b.n	3ef56 <__gethex+0x3f2>
   3eeee:	f04f 0a01 	mov.w	sl, #1
   3eef2:	e7cf      	b.n	3ee94 <__gethex+0x330>
   3eef4:	2701      	movs	r7, #1
   3eef6:	e7e2      	b.n	3eebe <__gethex+0x35a>
   3eef8:	9b15      	ldr	r3, [sp, #84]	; 0x54
   3eefa:	f1c3 0301 	rsb	r3, r3, #1
   3eefe:	9315      	str	r3, [sp, #84]	; 0x54
   3ef00:	9b15      	ldr	r3, [sp, #84]	; 0x54
   3ef02:	2b00      	cmp	r3, #0
   3ef04:	d0f0      	beq.n	3eee8 <__gethex+0x384>
   3ef06:	f8d4 b010 	ldr.w	fp, [r4, #16]
   3ef0a:	f104 0314 	add.w	r3, r4, #20
   3ef0e:	f04f 0c00 	mov.w	ip, #0
   3ef12:	ea4f 0a8b 	mov.w	sl, fp, lsl #2
   3ef16:	eb03 018b 	add.w	r1, r3, fp, lsl #2
   3ef1a:	4618      	mov	r0, r3
   3ef1c:	f853 2b04 	ldr.w	r2, [r3], #4
   3ef20:	f1b2 3fff 	cmp.w	r2, #4294967295	; 0xffffffff
   3ef24:	d01c      	beq.n	3ef60 <__gethex+0x3fc>
   3ef26:	3201      	adds	r2, #1
   3ef28:	6002      	str	r2, [r0, #0]
   3ef2a:	2f02      	cmp	r7, #2
   3ef2c:	f104 0314 	add.w	r3, r4, #20
   3ef30:	d13f      	bne.n	3efb2 <__gethex+0x44e>
   3ef32:	f8d8 2000 	ldr.w	r2, [r8]
   3ef36:	3a01      	subs	r2, #1
   3ef38:	42b2      	cmp	r2, r6
   3ef3a:	d10a      	bne.n	3ef52 <__gethex+0x3ee>
   3ef3c:	1171      	asrs	r1, r6, #5
   3ef3e:	2201      	movs	r2, #1
   3ef40:	f006 061f 	and.w	r6, r6, #31
   3ef44:	f853 3021 	ldr.w	r3, [r3, r1, lsl #2]
   3ef48:	fa02 f606 	lsl.w	r6, r2, r6
   3ef4c:	421e      	tst	r6, r3
   3ef4e:	bf18      	it	ne
   3ef50:	4617      	movne	r7, r2
   3ef52:	f047 0720 	orr.w	r7, r7, #32
   3ef56:	9b14      	ldr	r3, [sp, #80]	; 0x50
   3ef58:	601c      	str	r4, [r3, #0]
   3ef5a:	9b04      	ldr	r3, [sp, #16]
   3ef5c:	601d      	str	r5, [r3, #0]
   3ef5e:	e695      	b.n	3ec8c <__gethex+0x128>
   3ef60:	4299      	cmp	r1, r3
   3ef62:	f843 cc04 	str.w	ip, [r3, #-4]
   3ef66:	d8d8      	bhi.n	3ef1a <__gethex+0x3b6>
   3ef68:	68a3      	ldr	r3, [r4, #8]
   3ef6a:	459b      	cmp	fp, r3
   3ef6c:	db19      	blt.n	3efa2 <__gethex+0x43e>
   3ef6e:	6861      	ldr	r1, [r4, #4]
   3ef70:	ee18 0a10 	vmov	r0, s16
   3ef74:	3101      	adds	r1, #1
   3ef76:	f7fb f87f 	bl	3a078 <_Balloc>
   3ef7a:	4681      	mov	r9, r0
   3ef7c:	b918      	cbnz	r0, 3ef86 <__gethex+0x422>
   3ef7e:	4b1b      	ldr	r3, [pc, #108]	; (3efec <__gethex+0x488>)
   3ef80:	4602      	mov	r2, r0
   3ef82:	2184      	movs	r1, #132	; 0x84
   3ef84:	e6a8      	b.n	3ecd8 <__gethex+0x174>
   3ef86:	6922      	ldr	r2, [r4, #16]
   3ef88:	f104 010c 	add.w	r1, r4, #12
   3ef8c:	300c      	adds	r0, #12
   3ef8e:	3202      	adds	r2, #2
   3ef90:	0092      	lsls	r2, r2, #2
   3ef92:	f7fa ff61 	bl	39e58 <memcpy>
   3ef96:	4621      	mov	r1, r4
   3ef98:	464c      	mov	r4, r9
   3ef9a:	ee18 0a10 	vmov	r0, s16
   3ef9e:	f7fb f890 	bl	3a0c2 <_Bfree>
   3efa2:	6923      	ldr	r3, [r4, #16]
   3efa4:	1c5a      	adds	r2, r3, #1
   3efa6:	eb04 0383 	add.w	r3, r4, r3, lsl #2
   3efaa:	6122      	str	r2, [r4, #16]
   3efac:	2201      	movs	r2, #1
   3efae:	615a      	str	r2, [r3, #20]
   3efb0:	e7bb      	b.n	3ef2a <__gethex+0x3c6>
   3efb2:	6922      	ldr	r2, [r4, #16]
   3efb4:	455a      	cmp	r2, fp
   3efb6:	dd0b      	ble.n	3efd0 <__gethex+0x46c>
   3efb8:	2101      	movs	r1, #1
   3efba:	4620      	mov	r0, r4
   3efbc:	f7ff fd6c 	bl	3ea98 <rshift>
   3efc0:	3501      	adds	r5, #1
   3efc2:	f8d8 3008 	ldr.w	r3, [r8, #8]
   3efc6:	42ab      	cmp	r3, r5
   3efc8:	f6ff aed0 	blt.w	3ed6c <__gethex+0x208>
   3efcc:	2701      	movs	r7, #1
   3efce:	e7c0      	b.n	3ef52 <__gethex+0x3ee>
   3efd0:	f016 061f 	ands.w	r6, r6, #31
   3efd4:	d0fa      	beq.n	3efcc <__gethex+0x468>
   3efd6:	4453      	add	r3, sl
   3efd8:	f1c6 0620 	rsb	r6, r6, #32
   3efdc:	f853 0c04 	ldr.w	r0, [r3, #-4]
   3efe0:	f7fb f90a 	bl	3a1f8 <__hi0bits>
   3efe4:	42b0      	cmp	r0, r6
   3efe6:	dbe7      	blt.n	3efb8 <__gethex+0x454>
   3efe8:	e7f0      	b.n	3efcc <__gethex+0x468>
   3efea:	bf00      	nop
   3efec:	00040678 	.word	0x00040678

0003eff0 <L_shift>:
   3eff0:	f1c2 0208 	rsb	r2, r2, #8
   3eff4:	0092      	lsls	r2, r2, #2
   3eff6:	b570      	push	{r4, r5, r6, lr}
   3eff8:	f1c2 0620 	rsb	r6, r2, #32
   3effc:	6843      	ldr	r3, [r0, #4]
   3effe:	6804      	ldr	r4, [r0, #0]
   3f000:	fa03 f506 	lsl.w	r5, r3, r6
   3f004:	40d3      	lsrs	r3, r2
   3f006:	432c      	orrs	r4, r5
   3f008:	6004      	str	r4, [r0, #0]
   3f00a:	f840 3f04 	str.w	r3, [r0, #4]!
   3f00e:	4288      	cmp	r0, r1
   3f010:	d3f4      	bcc.n	3effc <L_shift+0xc>
   3f012:	bd70      	pop	{r4, r5, r6, pc}

0003f014 <__match>:
   3f014:	6803      	ldr	r3, [r0, #0]
   3f016:	3301      	adds	r3, #1
   3f018:	b530      	push	{r4, r5, lr}
   3f01a:	f811 4b01 	ldrb.w	r4, [r1], #1
   3f01e:	b914      	cbnz	r4, 3f026 <__match+0x12>
   3f020:	6003      	str	r3, [r0, #0]
   3f022:	2001      	movs	r0, #1
   3f024:	bd30      	pop	{r4, r5, pc}
   3f026:	f813 2b01 	ldrb.w	r2, [r3], #1
   3f02a:	f1a2 0541 	sub.w	r5, r2, #65	; 0x41
   3f02e:	2d19      	cmp	r5, #25
   3f030:	bf98      	it	ls
   3f032:	3220      	addls	r2, #32
   3f034:	42a2      	cmp	r2, r4
   3f036:	d0f0      	beq.n	3f01a <__match+0x6>
   3f038:	2000      	movs	r0, #0
   3f03a:	e7f3      	b.n	3f024 <__match+0x10>

0003f03c <__hexnan>:
   3f03c:	680b      	ldr	r3, [r1, #0]
   3f03e:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   3f042:	115e      	asrs	r6, r3, #5
   3f044:	f013 031f 	ands.w	r3, r3, #31
   3f048:	f04f 0500 	mov.w	r5, #0
   3f04c:	b087      	sub	sp, #28
   3f04e:	eb02 0686 	add.w	r6, r2, r6, lsl #2
   3f052:	4690      	mov	r8, r2
   3f054:	4682      	mov	sl, r0
   3f056:	6802      	ldr	r2, [r0, #0]
   3f058:	bf18      	it	ne
   3f05a:	3604      	addne	r6, #4
   3f05c:	46ab      	mov	fp, r5
   3f05e:	9301      	str	r3, [sp, #4]
   3f060:	1f37      	subs	r7, r6, #4
   3f062:	f846 5c04 	str.w	r5, [r6, #-4]
   3f066:	9502      	str	r5, [sp, #8]
   3f068:	46b9      	mov	r9, r7
   3f06a:	463c      	mov	r4, r7
   3f06c:	1c53      	adds	r3, r2, #1
   3f06e:	7851      	ldrb	r1, [r2, #1]
   3f070:	9303      	str	r3, [sp, #12]
   3f072:	b341      	cbz	r1, 3f0c6 <__hexnan+0x8a>
   3f074:	4608      	mov	r0, r1
   3f076:	9205      	str	r2, [sp, #20]
   3f078:	9104      	str	r1, [sp, #16]
   3f07a:	f7ff fd5e 	bl	3eb3a <__hexdig_fun>
   3f07e:	2800      	cmp	r0, #0
   3f080:	d14f      	bne.n	3f122 <__hexnan+0xe6>
   3f082:	9904      	ldr	r1, [sp, #16]
   3f084:	9a05      	ldr	r2, [sp, #20]
   3f086:	2920      	cmp	r1, #32
   3f088:	d818      	bhi.n	3f0bc <__hexnan+0x80>
   3f08a:	9b02      	ldr	r3, [sp, #8]
   3f08c:	459b      	cmp	fp, r3
   3f08e:	dd13      	ble.n	3f0b8 <__hexnan+0x7c>
   3f090:	454c      	cmp	r4, r9
   3f092:	d206      	bcs.n	3f0a2 <__hexnan+0x66>
   3f094:	2d07      	cmp	r5, #7
   3f096:	dc04      	bgt.n	3f0a2 <__hexnan+0x66>
   3f098:	462a      	mov	r2, r5
   3f09a:	4649      	mov	r1, r9
   3f09c:	4620      	mov	r0, r4
   3f09e:	f7ff ffa7 	bl	3eff0 <L_shift>
   3f0a2:	4544      	cmp	r4, r8
   3f0a4:	d94f      	bls.n	3f146 <__hexnan+0x10a>
   3f0a6:	2300      	movs	r3, #0
   3f0a8:	f1a4 0904 	sub.w	r9, r4, #4
   3f0ac:	f8cd b008 	str.w	fp, [sp, #8]
   3f0b0:	f844 3c04 	str.w	r3, [r4, #-4]
   3f0b4:	461d      	mov	r5, r3
   3f0b6:	464c      	mov	r4, r9
   3f0b8:	9a03      	ldr	r2, [sp, #12]
   3f0ba:	e7d7      	b.n	3f06c <__hexnan+0x30>
   3f0bc:	2929      	cmp	r1, #41	; 0x29
   3f0be:	d155      	bne.n	3f16c <__hexnan+0x130>
   3f0c0:	3202      	adds	r2, #2
   3f0c2:	f8ca 2000 	str.w	r2, [sl]
   3f0c6:	f1bb 0f00 	cmp.w	fp, #0
   3f0ca:	d04f      	beq.n	3f16c <__hexnan+0x130>
   3f0cc:	454c      	cmp	r4, r9
   3f0ce:	d206      	bcs.n	3f0de <__hexnan+0xa2>
   3f0d0:	2d07      	cmp	r5, #7
   3f0d2:	dc04      	bgt.n	3f0de <__hexnan+0xa2>
   3f0d4:	462a      	mov	r2, r5
   3f0d6:	4649      	mov	r1, r9
   3f0d8:	4620      	mov	r0, r4
   3f0da:	f7ff ff89 	bl	3eff0 <L_shift>
   3f0de:	4544      	cmp	r4, r8
   3f0e0:	d933      	bls.n	3f14a <__hexnan+0x10e>
   3f0e2:	f1a8 0204 	sub.w	r2, r8, #4
   3f0e6:	4623      	mov	r3, r4
   3f0e8:	f853 1b04 	ldr.w	r1, [r3], #4
   3f0ec:	429f      	cmp	r7, r3
   3f0ee:	f842 1f04 	str.w	r1, [r2, #4]!
   3f0f2:	d2f9      	bcs.n	3f0e8 <__hexnan+0xac>
   3f0f4:	1b3b      	subs	r3, r7, r4
   3f0f6:	3e03      	subs	r6, #3
   3f0f8:	3401      	adds	r4, #1
   3f0fa:	2200      	movs	r2, #0
   3f0fc:	f023 0303 	bic.w	r3, r3, #3
   3f100:	3304      	adds	r3, #4
   3f102:	42b4      	cmp	r4, r6
   3f104:	bf88      	it	hi
   3f106:	2304      	movhi	r3, #4
   3f108:	4443      	add	r3, r8
   3f10a:	f843 2b04 	str.w	r2, [r3], #4
   3f10e:	429f      	cmp	r7, r3
   3f110:	d2fb      	bcs.n	3f10a <__hexnan+0xce>
   3f112:	683b      	ldr	r3, [r7, #0]
   3f114:	b91b      	cbnz	r3, 3f11e <__hexnan+0xe2>
   3f116:	4547      	cmp	r7, r8
   3f118:	d126      	bne.n	3f168 <__hexnan+0x12c>
   3f11a:	2301      	movs	r3, #1
   3f11c:	603b      	str	r3, [r7, #0]
   3f11e:	2005      	movs	r0, #5
   3f120:	e025      	b.n	3f16e <__hexnan+0x132>
   3f122:	3501      	adds	r5, #1
   3f124:	f10b 0b01 	add.w	fp, fp, #1
   3f128:	2d08      	cmp	r5, #8
   3f12a:	dd05      	ble.n	3f138 <__hexnan+0xfc>
   3f12c:	4544      	cmp	r4, r8
   3f12e:	d9c3      	bls.n	3f0b8 <__hexnan+0x7c>
   3f130:	2300      	movs	r3, #0
   3f132:	3c04      	subs	r4, #4
   3f134:	2501      	movs	r5, #1
   3f136:	6023      	str	r3, [r4, #0]
   3f138:	6822      	ldr	r2, [r4, #0]
   3f13a:	f000 000f 	and.w	r0, r0, #15
   3f13e:	ea40 1202 	orr.w	r2, r0, r2, lsl #4
   3f142:	6022      	str	r2, [r4, #0]
   3f144:	e7b8      	b.n	3f0b8 <__hexnan+0x7c>
   3f146:	2508      	movs	r5, #8
   3f148:	e7b6      	b.n	3f0b8 <__hexnan+0x7c>
   3f14a:	9b01      	ldr	r3, [sp, #4]
   3f14c:	2b00      	cmp	r3, #0
   3f14e:	d0e0      	beq.n	3f112 <__hexnan+0xd6>
   3f150:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
   3f154:	f1c3 0320 	rsb	r3, r3, #32
   3f158:	fa22 f303 	lsr.w	r3, r2, r3
   3f15c:	f856 2c04 	ldr.w	r2, [r6, #-4]
   3f160:	401a      	ands	r2, r3
   3f162:	f846 2c04 	str.w	r2, [r6, #-4]
   3f166:	e7d4      	b.n	3f112 <__hexnan+0xd6>
   3f168:	3f04      	subs	r7, #4
   3f16a:	e7d2      	b.n	3f112 <__hexnan+0xd6>
   3f16c:	2004      	movs	r0, #4
   3f16e:	b007      	add	sp, #28
   3f170:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}

0003f174 <_isatty_r>:
   3f174:	b538      	push	{r3, r4, r5, lr}
   3f176:	2300      	movs	r3, #0
   3f178:	4d05      	ldr	r5, [pc, #20]	; (3f190 <_isatty_r+0x1c>)
   3f17a:	4604      	mov	r4, r0
   3f17c:	4608      	mov	r0, r1
   3f17e:	602b      	str	r3, [r5, #0]
   3f180:	f001 f8e0 	bl	40344 <_isatty>
   3f184:	1c43      	adds	r3, r0, #1
   3f186:	d102      	bne.n	3f18e <_isatty_r+0x1a>
   3f188:	682b      	ldr	r3, [r5, #0]
   3f18a:	b103      	cbz	r3, 3f18e <_isatty_r+0x1a>
   3f18c:	6023      	str	r3, [r4, #0]
   3f18e:	bd38      	pop	{r3, r4, r5, pc}
   3f190:	20006a0c 	.word	0x20006a0c

0003f194 <iswspace_l>:
   3f194:	28ff      	cmp	r0, #255	; 0xff
   3f196:	bf9d      	ittte	ls
   3f198:	4b02      	ldrls	r3, [pc, #8]	; (3f1a4 <iswspace_l+0x10>)
   3f19a:	5cc0      	ldrbls	r0, [r0, r3]
   3f19c:	f000 0008 	andls.w	r0, r0, #8
   3f1a0:	2000      	movhi	r0, #0
   3f1a2:	4770      	bx	lr
   3f1a4:	0004056b 	.word	0x0004056b

0003f1a8 <__numeric_load_locale>:
   3f1a8:	2000      	movs	r0, #0
   3f1aa:	4770      	bx	lr
   3f1ac:	0000      	movs	r0, r0
	...

0003f1b0 <nan>:
   3f1b0:	ed9f 0b01 	vldr	d0, [pc, #4]	; 3f1b8 <nan+0x8>
   3f1b4:	4770      	bx	lr
   3f1b6:	bf00      	nop
   3f1b8:	00000000 	.word	0x00000000
   3f1bc:	7ff80000 	.word	0x7ff80000

0003f1c0 <_wcrtomb_r>:
   3f1c0:	b5f0      	push	{r4, r5, r6, r7, lr}
   3f1c2:	4c09      	ldr	r4, [pc, #36]	; (3f1e8 <_wcrtomb_r+0x28>)
   3f1c4:	b085      	sub	sp, #20
   3f1c6:	4605      	mov	r5, r0
   3f1c8:	461e      	mov	r6, r3
   3f1ca:	f8d4 70e0 	ldr.w	r7, [r4, #224]	; 0xe0
   3f1ce:	b909      	cbnz	r1, 3f1d4 <_wcrtomb_r+0x14>
   3f1d0:	460a      	mov	r2, r1
   3f1d2:	a901      	add	r1, sp, #4
   3f1d4:	47b8      	blx	r7
   3f1d6:	1c43      	adds	r3, r0, #1
   3f1d8:	bf01      	itttt	eq
   3f1da:	2300      	moveq	r3, #0
   3f1dc:	6033      	streq	r3, [r6, #0]
   3f1de:	238a      	moveq	r3, #138	; 0x8a
   3f1e0:	602b      	streq	r3, [r5, #0]
   3f1e2:	b005      	add	sp, #20
   3f1e4:	bdf0      	pop	{r4, r5, r6, r7, pc}
   3f1e6:	bf00      	nop
   3f1e8:	20006430 	.word	0x20006430

0003f1ec <wcrtomb>:
   3f1ec:	4613      	mov	r3, r2
   3f1ee:	460a      	mov	r2, r1
   3f1f0:	4601      	mov	r1, r0
   3f1f2:	4802      	ldr	r0, [pc, #8]	; (3f1fc <wcrtomb+0x10>)
   3f1f4:	6800      	ldr	r0, [r0, #0]
   3f1f6:	f7ff bfe3 	b.w	3f1c0 <_wcrtomb_r>
   3f1fa:	bf00      	nop
   3f1fc:	20006000 	.word	0x20006000

0003f200 <abort>:
   3f200:	2006      	movs	r0, #6
   3f202:	b508      	push	{r3, lr}
   3f204:	f000 f883 	bl	3f30e <raise>
   3f208:	2001      	movs	r0, #1
   3f20a:	f001 f8c1 	bl	40390 <_exit>
	...

0003f210 <_init_signal_r>:
   3f210:	b538      	push	{r3, r4, r5, lr}
   3f212:	f8d0 52dc 	ldr.w	r5, [r0, #732]	; 0x2dc
   3f216:	4604      	mov	r4, r0
   3f218:	b95d      	cbnz	r5, 3f232 <_init_signal_r+0x22>
   3f21a:	2180      	movs	r1, #128	; 0x80
   3f21c:	f7fa fb50 	bl	398c0 <_malloc_r>
   3f220:	f8c4 02dc 	str.w	r0, [r4, #732]	; 0x2dc
   3f224:	b138      	cbz	r0, 3f236 <_init_signal_r+0x26>
   3f226:	1f03      	subs	r3, r0, #4
   3f228:	307c      	adds	r0, #124	; 0x7c
   3f22a:	f843 5f04 	str.w	r5, [r3, #4]!
   3f22e:	4283      	cmp	r3, r0
   3f230:	d1fb      	bne.n	3f22a <_init_signal_r+0x1a>
   3f232:	2000      	movs	r0, #0
   3f234:	bd38      	pop	{r3, r4, r5, pc}
   3f236:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3f23a:	e7fb      	b.n	3f234 <_init_signal_r+0x24>

0003f23c <_signal_r>:
   3f23c:	291f      	cmp	r1, #31
   3f23e:	b570      	push	{r4, r5, r6, lr}
   3f240:	4604      	mov	r4, r0
   3f242:	460d      	mov	r5, r1
   3f244:	4616      	mov	r6, r2
   3f246:	d904      	bls.n	3f252 <_signal_r+0x16>
   3f248:	2316      	movs	r3, #22
   3f24a:	6003      	str	r3, [r0, #0]
   3f24c:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3f250:	e008      	b.n	3f264 <_signal_r+0x28>
   3f252:	f8d0 32dc 	ldr.w	r3, [r0, #732]	; 0x2dc
   3f256:	b133      	cbz	r3, 3f266 <_signal_r+0x2a>
   3f258:	f8d4 32dc 	ldr.w	r3, [r4, #732]	; 0x2dc
   3f25c:	f853 0025 	ldr.w	r0, [r3, r5, lsl #2]
   3f260:	f843 6025 	str.w	r6, [r3, r5, lsl #2]
   3f264:	bd70      	pop	{r4, r5, r6, pc}
   3f266:	f7ff ffd3 	bl	3f210 <_init_signal_r>
   3f26a:	2800      	cmp	r0, #0
   3f26c:	d0f4      	beq.n	3f258 <_signal_r+0x1c>
   3f26e:	e7ed      	b.n	3f24c <_signal_r+0x10>

0003f270 <_raise_r>:
   3f270:	291f      	cmp	r1, #31
   3f272:	b538      	push	{r3, r4, r5, lr}
   3f274:	4604      	mov	r4, r0
   3f276:	460d      	mov	r5, r1
   3f278:	d904      	bls.n	3f284 <_raise_r+0x14>
   3f27a:	2316      	movs	r3, #22
   3f27c:	6003      	str	r3, [r0, #0]
   3f27e:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3f282:	bd38      	pop	{r3, r4, r5, pc}
   3f284:	f8d0 22dc 	ldr.w	r2, [r0, #732]	; 0x2dc
   3f288:	b112      	cbz	r2, 3f290 <_raise_r+0x20>
   3f28a:	f852 3021 	ldr.w	r3, [r2, r1, lsl #2]
   3f28e:	b94b      	cbnz	r3, 3f2a4 <_raise_r+0x34>
   3f290:	4620      	mov	r0, r4
   3f292:	f000 f86b 	bl	3f36c <_getpid_r>
   3f296:	462a      	mov	r2, r5
   3f298:	4601      	mov	r1, r0
   3f29a:	4620      	mov	r0, r4
   3f29c:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
   3f2a0:	f000 b852 	b.w	3f348 <_kill_r>
   3f2a4:	2b01      	cmp	r3, #1
   3f2a6:	d00a      	beq.n	3f2be <_raise_r+0x4e>
   3f2a8:	1c59      	adds	r1, r3, #1
   3f2aa:	d103      	bne.n	3f2b4 <_raise_r+0x44>
   3f2ac:	2316      	movs	r3, #22
   3f2ae:	6003      	str	r3, [r0, #0]
   3f2b0:	2001      	movs	r0, #1
   3f2b2:	e7e6      	b.n	3f282 <_raise_r+0x12>
   3f2b4:	2400      	movs	r4, #0
   3f2b6:	4628      	mov	r0, r5
   3f2b8:	f842 4025 	str.w	r4, [r2, r5, lsl #2]
   3f2bc:	4798      	blx	r3
   3f2be:	2000      	movs	r0, #0
   3f2c0:	e7df      	b.n	3f282 <_raise_r+0x12>

0003f2c2 <__sigtramp_r>:
   3f2c2:	291f      	cmp	r1, #31
   3f2c4:	b538      	push	{r3, r4, r5, lr}
   3f2c6:	4604      	mov	r4, r0
   3f2c8:	460d      	mov	r5, r1
   3f2ca:	d902      	bls.n	3f2d2 <__sigtramp_r+0x10>
   3f2cc:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3f2d0:	bd38      	pop	{r3, r4, r5, pc}
   3f2d2:	f8d0 32dc 	ldr.w	r3, [r0, #732]	; 0x2dc
   3f2d6:	b133      	cbz	r3, 3f2e6 <__sigtramp_r+0x24>
   3f2d8:	f8d4 22dc 	ldr.w	r2, [r4, #732]	; 0x2dc
   3f2dc:	f852 3025 	ldr.w	r3, [r2, r5, lsl #2]
   3f2e0:	b933      	cbnz	r3, 3f2f0 <__sigtramp_r+0x2e>
   3f2e2:	2001      	movs	r0, #1
   3f2e4:	e7f4      	b.n	3f2d0 <__sigtramp_r+0xe>
   3f2e6:	f7ff ff93 	bl	3f210 <_init_signal_r>
   3f2ea:	2800      	cmp	r0, #0
   3f2ec:	d0f4      	beq.n	3f2d8 <__sigtramp_r+0x16>
   3f2ee:	e7ed      	b.n	3f2cc <__sigtramp_r+0xa>
   3f2f0:	1c59      	adds	r1, r3, #1
   3f2f2:	d008      	beq.n	3f306 <__sigtramp_r+0x44>
   3f2f4:	2b01      	cmp	r3, #1
   3f2f6:	d008      	beq.n	3f30a <__sigtramp_r+0x48>
   3f2f8:	2400      	movs	r4, #0
   3f2fa:	4628      	mov	r0, r5
   3f2fc:	f842 4025 	str.w	r4, [r2, r5, lsl #2]
   3f300:	4798      	blx	r3
   3f302:	4620      	mov	r0, r4
   3f304:	e7e4      	b.n	3f2d0 <__sigtramp_r+0xe>
   3f306:	2002      	movs	r0, #2
   3f308:	e7e2      	b.n	3f2d0 <__sigtramp_r+0xe>
   3f30a:	2003      	movs	r0, #3
   3f30c:	e7e0      	b.n	3f2d0 <__sigtramp_r+0xe>

0003f30e <raise>:
   3f30e:	4b02      	ldr	r3, [pc, #8]	; (3f318 <raise+0xa>)
   3f310:	4601      	mov	r1, r0
   3f312:	6818      	ldr	r0, [r3, #0]
   3f314:	f7ff bfac 	b.w	3f270 <_raise_r>
   3f318:	20006000 	.word	0x20006000

0003f31c <signal>:
   3f31c:	4b02      	ldr	r3, [pc, #8]	; (3f328 <signal+0xc>)
   3f31e:	460a      	mov	r2, r1
   3f320:	4601      	mov	r1, r0
   3f322:	6818      	ldr	r0, [r3, #0]
   3f324:	f7ff bf8a 	b.w	3f23c <_signal_r>
   3f328:	20006000 	.word	0x20006000

0003f32c <_init_signal>:
   3f32c:	4b01      	ldr	r3, [pc, #4]	; (3f334 <_init_signal+0x8>)
   3f32e:	6818      	ldr	r0, [r3, #0]
   3f330:	f7ff bf6e 	b.w	3f210 <_init_signal_r>
   3f334:	20006000 	.word	0x20006000

0003f338 <__sigtramp>:
   3f338:	4b02      	ldr	r3, [pc, #8]	; (3f344 <__sigtramp+0xc>)
   3f33a:	4601      	mov	r1, r0
   3f33c:	6818      	ldr	r0, [r3, #0]
   3f33e:	f7ff bfc0 	b.w	3f2c2 <__sigtramp_r>
   3f342:	bf00      	nop
   3f344:	20006000 	.word	0x20006000

0003f348 <_kill_r>:
   3f348:	b538      	push	{r3, r4, r5, lr}
   3f34a:	2300      	movs	r3, #0
   3f34c:	4d06      	ldr	r5, [pc, #24]	; (3f368 <_kill_r+0x20>)
   3f34e:	4604      	mov	r4, r0
   3f350:	4608      	mov	r0, r1
   3f352:	4611      	mov	r1, r2
   3f354:	602b      	str	r3, [r5, #0]
   3f356:	f000 fffd 	bl	40354 <_kill>
   3f35a:	1c43      	adds	r3, r0, #1
   3f35c:	d102      	bne.n	3f364 <_kill_r+0x1c>
   3f35e:	682b      	ldr	r3, [r5, #0]
   3f360:	b103      	cbz	r3, 3f364 <_kill_r+0x1c>
   3f362:	6023      	str	r3, [r4, #0]
   3f364:	bd38      	pop	{r3, r4, r5, pc}
   3f366:	bf00      	nop
   3f368:	20006a0c 	.word	0x20006a0c

0003f36c <_getpid_r>:
   3f36c:	f000 bfe2 	b.w	40334 <_getpid>
   3f370:	b570      	push	{r4, r5, r6, lr}
   3f372:	f04f 0cff 	mov.w	ip, #255	; 0xff
   3f376:	f44c 6ce0 	orr.w	ip, ip, #1792	; 0x700
   3f37a:	ea1c 5411 	ands.w	r4, ip, r1, lsr #20
   3f37e:	bf1d      	ittte	ne
   3f380:	ea1c 5513 	andsne.w	r5, ip, r3, lsr #20
   3f384:	ea94 0f0c 	teqne	r4, ip
   3f388:	ea95 0f0c 	teqne	r5, ip
   3f38c:	f000 f8de 	bleq	3f54c <_getpid_r+0x1e0>
   3f390:	442c      	add	r4, r5
   3f392:	ea81 0603 	eor.w	r6, r1, r3
   3f396:	ea21 514c 	bic.w	r1, r1, ip, lsl #21
   3f39a:	ea23 534c 	bic.w	r3, r3, ip, lsl #21
   3f39e:	ea50 3501 	orrs.w	r5, r0, r1, lsl #12
   3f3a2:	bf18      	it	ne
   3f3a4:	ea52 3503 	orrsne.w	r5, r2, r3, lsl #12
   3f3a8:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
   3f3ac:	f443 1380 	orr.w	r3, r3, #1048576	; 0x100000
   3f3b0:	d038      	beq.n	3f424 <_getpid_r+0xb8>
   3f3b2:	fba0 ce02 	umull	ip, lr, r0, r2
   3f3b6:	f04f 0500 	mov.w	r5, #0
   3f3ba:	fbe1 e502 	umlal	lr, r5, r1, r2
   3f3be:	f006 4200 	and.w	r2, r6, #2147483648	; 0x80000000
   3f3c2:	fbe0 e503 	umlal	lr, r5, r0, r3
   3f3c6:	f04f 0600 	mov.w	r6, #0
   3f3ca:	fbe1 5603 	umlal	r5, r6, r1, r3
   3f3ce:	f09c 0f00 	teq	ip, #0
   3f3d2:	bf18      	it	ne
   3f3d4:	f04e 0e01 	orrne.w	lr, lr, #1
   3f3d8:	f1a4 04ff 	sub.w	r4, r4, #255	; 0xff
   3f3dc:	f5b6 7f00 	cmp.w	r6, #512	; 0x200
   3f3e0:	f564 7440 	sbc.w	r4, r4, #768	; 0x300
   3f3e4:	d204      	bcs.n	3f3f0 <_getpid_r+0x84>
   3f3e6:	ea5f 0e4e 	movs.w	lr, lr, lsl #1
   3f3ea:	416d      	adcs	r5, r5
   3f3ec:	eb46 0606 	adc.w	r6, r6, r6
   3f3f0:	ea42 21c6 	orr.w	r1, r2, r6, lsl #11
   3f3f4:	ea41 5155 	orr.w	r1, r1, r5, lsr #21
   3f3f8:	ea4f 20c5 	mov.w	r0, r5, lsl #11
   3f3fc:	ea40 505e 	orr.w	r0, r0, lr, lsr #21
   3f400:	ea4f 2ece 	mov.w	lr, lr, lsl #11
   3f404:	f1b4 0cfd 	subs.w	ip, r4, #253	; 0xfd
   3f408:	bf88      	it	hi
   3f40a:	f5bc 6fe0 	cmphi.w	ip, #1792	; 0x700
   3f40e:	d81e      	bhi.n	3f44e <_getpid_r+0xe2>
   3f410:	f1be 4f00 	cmp.w	lr, #2147483648	; 0x80000000
   3f414:	bf08      	it	eq
   3f416:	ea5f 0e50 	movseq.w	lr, r0, lsr #1
   3f41a:	f150 0000 	adcs.w	r0, r0, #0
   3f41e:	eb41 5104 	adc.w	r1, r1, r4, lsl #20
   3f422:	bd70      	pop	{r4, r5, r6, pc}
   3f424:	f006 4600 	and.w	r6, r6, #2147483648	; 0x80000000
   3f428:	ea46 0101 	orr.w	r1, r6, r1
   3f42c:	ea40 0002 	orr.w	r0, r0, r2
   3f430:	ea81 0103 	eor.w	r1, r1, r3
   3f434:	ebb4 045c 	subs.w	r4, r4, ip, lsr #1
   3f438:	bfc2      	ittt	gt
   3f43a:	ebd4 050c 	rsbsgt	r5, r4, ip
   3f43e:	ea41 5104 	orrgt.w	r1, r1, r4, lsl #20
   3f442:	bd70      	popgt	{r4, r5, r6, pc}
   3f444:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
   3f448:	f04f 0e00 	mov.w	lr, #0
   3f44c:	3c01      	subs	r4, #1
   3f44e:	f300 80ab 	bgt.w	3f5a8 <_getpid_r+0x23c>
   3f452:	f114 0f36 	cmn.w	r4, #54	; 0x36
   3f456:	bfde      	ittt	le
   3f458:	2000      	movle	r0, #0
   3f45a:	f001 4100 	andle.w	r1, r1, #2147483648	; 0x80000000
   3f45e:	bd70      	pople	{r4, r5, r6, pc}
   3f460:	f1c4 0400 	rsb	r4, r4, #0
   3f464:	3c20      	subs	r4, #32
   3f466:	da35      	bge.n	3f4d4 <_getpid_r+0x168>
   3f468:	340c      	adds	r4, #12
   3f46a:	dc1b      	bgt.n	3f4a4 <_getpid_r+0x138>
   3f46c:	f104 0414 	add.w	r4, r4, #20
   3f470:	f1c4 0520 	rsb	r5, r4, #32
   3f474:	fa00 f305 	lsl.w	r3, r0, r5
   3f478:	fa20 f004 	lsr.w	r0, r0, r4
   3f47c:	fa01 f205 	lsl.w	r2, r1, r5
   3f480:	ea40 0002 	orr.w	r0, r0, r2
   3f484:	f001 4200 	and.w	r2, r1, #2147483648	; 0x80000000
   3f488:	f021 4100 	bic.w	r1, r1, #2147483648	; 0x80000000
   3f48c:	eb10 70d3 	adds.w	r0, r0, r3, lsr #31
   3f490:	fa21 f604 	lsr.w	r6, r1, r4
   3f494:	eb42 0106 	adc.w	r1, r2, r6
   3f498:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
   3f49c:	bf08      	it	eq
   3f49e:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
   3f4a2:	bd70      	pop	{r4, r5, r6, pc}
   3f4a4:	f1c4 040c 	rsb	r4, r4, #12
   3f4a8:	f1c4 0520 	rsb	r5, r4, #32
   3f4ac:	fa00 f304 	lsl.w	r3, r0, r4
   3f4b0:	fa20 f005 	lsr.w	r0, r0, r5
   3f4b4:	fa01 f204 	lsl.w	r2, r1, r4
   3f4b8:	ea40 0002 	orr.w	r0, r0, r2
   3f4bc:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
   3f4c0:	eb10 70d3 	adds.w	r0, r0, r3, lsr #31
   3f4c4:	f141 0100 	adc.w	r1, r1, #0
   3f4c8:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
   3f4cc:	bf08      	it	eq
   3f4ce:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
   3f4d2:	bd70      	pop	{r4, r5, r6, pc}
   3f4d4:	f1c4 0520 	rsb	r5, r4, #32
   3f4d8:	fa00 f205 	lsl.w	r2, r0, r5
   3f4dc:	ea4e 0e02 	orr.w	lr, lr, r2
   3f4e0:	fa20 f304 	lsr.w	r3, r0, r4
   3f4e4:	fa01 f205 	lsl.w	r2, r1, r5
   3f4e8:	ea43 0302 	orr.w	r3, r3, r2
   3f4ec:	fa21 f004 	lsr.w	r0, r1, r4
   3f4f0:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
   3f4f4:	fa21 f204 	lsr.w	r2, r1, r4
   3f4f8:	ea20 0002 	bic.w	r0, r0, r2
   3f4fc:	eb00 70d3 	add.w	r0, r0, r3, lsr #31
   3f500:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
   3f504:	bf08      	it	eq
   3f506:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
   3f50a:	bd70      	pop	{r4, r5, r6, pc}
   3f50c:	f094 0f00 	teq	r4, #0
   3f510:	d10f      	bne.n	3f532 <_getpid_r+0x1c6>
   3f512:	f001 4600 	and.w	r6, r1, #2147483648	; 0x80000000
   3f516:	0040      	lsls	r0, r0, #1
   3f518:	eb41 0101 	adc.w	r1, r1, r1
   3f51c:	f411 1f80 	tst.w	r1, #1048576	; 0x100000
   3f520:	bf08      	it	eq
   3f522:	3c01      	subeq	r4, #1
   3f524:	d0f7      	beq.n	3f516 <_getpid_r+0x1aa>
   3f526:	ea41 0106 	orr.w	r1, r1, r6
   3f52a:	f095 0f00 	teq	r5, #0
   3f52e:	bf18      	it	ne
   3f530:	4770      	bxne	lr
   3f532:	f003 4600 	and.w	r6, r3, #2147483648	; 0x80000000
   3f536:	0052      	lsls	r2, r2, #1
   3f538:	eb43 0303 	adc.w	r3, r3, r3
   3f53c:	f413 1f80 	tst.w	r3, #1048576	; 0x100000
   3f540:	bf08      	it	eq
   3f542:	3d01      	subeq	r5, #1
   3f544:	d0f7      	beq.n	3f536 <_getpid_r+0x1ca>
   3f546:	ea43 0306 	orr.w	r3, r3, r6
   3f54a:	4770      	bx	lr
   3f54c:	ea94 0f0c 	teq	r4, ip
   3f550:	ea0c 5513 	and.w	r5, ip, r3, lsr #20
   3f554:	bf18      	it	ne
   3f556:	ea95 0f0c 	teqne	r5, ip
   3f55a:	d00c      	beq.n	3f576 <_getpid_r+0x20a>
   3f55c:	ea50 0641 	orrs.w	r6, r0, r1, lsl #1
   3f560:	bf18      	it	ne
   3f562:	ea52 0643 	orrsne.w	r6, r2, r3, lsl #1
   3f566:	d1d1      	bne.n	3f50c <_getpid_r+0x1a0>
   3f568:	ea81 0103 	eor.w	r1, r1, r3
   3f56c:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
   3f570:	f04f 0000 	mov.w	r0, #0
   3f574:	bd70      	pop	{r4, r5, r6, pc}
   3f576:	ea50 0641 	orrs.w	r6, r0, r1, lsl #1
   3f57a:	bf06      	itte	eq
   3f57c:	4610      	moveq	r0, r2
   3f57e:	4619      	moveq	r1, r3
   3f580:	ea52 0643 	orrsne.w	r6, r2, r3, lsl #1
   3f584:	d019      	beq.n	3f5ba <_getpid_r+0x24e>
   3f586:	ea94 0f0c 	teq	r4, ip
   3f58a:	d102      	bne.n	3f592 <_getpid_r+0x226>
   3f58c:	ea50 3601 	orrs.w	r6, r0, r1, lsl #12
   3f590:	d113      	bne.n	3f5ba <_getpid_r+0x24e>
   3f592:	ea95 0f0c 	teq	r5, ip
   3f596:	d105      	bne.n	3f5a4 <_getpid_r+0x238>
   3f598:	ea52 3603 	orrs.w	r6, r2, r3, lsl #12
   3f59c:	bf1c      	itt	ne
   3f59e:	4610      	movne	r0, r2
   3f5a0:	4619      	movne	r1, r3
   3f5a2:	d10a      	bne.n	3f5ba <_getpid_r+0x24e>
   3f5a4:	ea81 0103 	eor.w	r1, r1, r3
   3f5a8:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
   3f5ac:	f041 41fe 	orr.w	r1, r1, #2130706432	; 0x7f000000
   3f5b0:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
   3f5b4:	f04f 0000 	mov.w	r0, #0
   3f5b8:	bd70      	pop	{r4, r5, r6, pc}
   3f5ba:	f041 41fe 	orr.w	r1, r1, #2130706432	; 0x7f000000
   3f5be:	f441 0178 	orr.w	r1, r1, #16252928	; 0xf80000
   3f5c2:	bd70      	pop	{r4, r5, r6, pc}

0003f5c4 <__aeabi_drsub>:
   3f5c4:	f081 4100 	eor.w	r1, r1, #2147483648	; 0x80000000
   3f5c8:	e002      	b.n	3f5d0 <__adddf3>
   3f5ca:	bf00      	nop

0003f5cc <__aeabi_dsub>:
   3f5cc:	f083 4300 	eor.w	r3, r3, #2147483648	; 0x80000000

0003f5d0 <__adddf3>:
   3f5d0:	b530      	push	{r4, r5, lr}
   3f5d2:	ea4f 0441 	mov.w	r4, r1, lsl #1
   3f5d6:	ea4f 0543 	mov.w	r5, r3, lsl #1
   3f5da:	ea94 0f05 	teq	r4, r5
   3f5de:	bf08      	it	eq
   3f5e0:	ea90 0f02 	teqeq	r0, r2
   3f5e4:	bf1f      	itttt	ne
   3f5e6:	ea54 0c00 	orrsne.w	ip, r4, r0
   3f5ea:	ea55 0c02 	orrsne.w	ip, r5, r2
   3f5ee:	ea7f 5c64 	mvnsne.w	ip, r4, asr #21
   3f5f2:	ea7f 5c65 	mvnsne.w	ip, r5, asr #21
   3f5f6:	f000 80e2 	beq.w	3f7be <__adddf3+0x1ee>
   3f5fa:	ea4f 5454 	mov.w	r4, r4, lsr #21
   3f5fe:	ebd4 5555 	rsbs	r5, r4, r5, lsr #21
   3f602:	bfb8      	it	lt
   3f604:	426d      	neglt	r5, r5
   3f606:	dd0c      	ble.n	3f622 <__adddf3+0x52>
   3f608:	442c      	add	r4, r5
   3f60a:	ea80 0202 	eor.w	r2, r0, r2
   3f60e:	ea81 0303 	eor.w	r3, r1, r3
   3f612:	ea82 0000 	eor.w	r0, r2, r0
   3f616:	ea83 0101 	eor.w	r1, r3, r1
   3f61a:	ea80 0202 	eor.w	r2, r0, r2
   3f61e:	ea81 0303 	eor.w	r3, r1, r3
   3f622:	2d36      	cmp	r5, #54	; 0x36
   3f624:	bf88      	it	hi
   3f626:	bd30      	pophi	{r4, r5, pc}
   3f628:	f011 4f00 	tst.w	r1, #2147483648	; 0x80000000
   3f62c:	ea4f 3101 	mov.w	r1, r1, lsl #12
   3f630:	f44f 1c80 	mov.w	ip, #1048576	; 0x100000
   3f634:	ea4c 3111 	orr.w	r1, ip, r1, lsr #12
   3f638:	d002      	beq.n	3f640 <__adddf3+0x70>
   3f63a:	4240      	negs	r0, r0
   3f63c:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
   3f640:	f013 4f00 	tst.w	r3, #2147483648	; 0x80000000
   3f644:	ea4f 3303 	mov.w	r3, r3, lsl #12
   3f648:	ea4c 3313 	orr.w	r3, ip, r3, lsr #12
   3f64c:	d002      	beq.n	3f654 <__adddf3+0x84>
   3f64e:	4252      	negs	r2, r2
   3f650:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
   3f654:	ea94 0f05 	teq	r4, r5
   3f658:	f000 80a7 	beq.w	3f7aa <__adddf3+0x1da>
   3f65c:	f1a4 0401 	sub.w	r4, r4, #1
   3f660:	f1d5 0e20 	rsbs	lr, r5, #32
   3f664:	db0d      	blt.n	3f682 <__adddf3+0xb2>
   3f666:	fa02 fc0e 	lsl.w	ip, r2, lr
   3f66a:	fa22 f205 	lsr.w	r2, r2, r5
   3f66e:	1880      	adds	r0, r0, r2
   3f670:	f141 0100 	adc.w	r1, r1, #0
   3f674:	fa03 f20e 	lsl.w	r2, r3, lr
   3f678:	1880      	adds	r0, r0, r2
   3f67a:	fa43 f305 	asr.w	r3, r3, r5
   3f67e:	4159      	adcs	r1, r3
   3f680:	e00e      	b.n	3f6a0 <__adddf3+0xd0>
   3f682:	f1a5 0520 	sub.w	r5, r5, #32
   3f686:	f10e 0e20 	add.w	lr, lr, #32
   3f68a:	2a01      	cmp	r2, #1
   3f68c:	fa03 fc0e 	lsl.w	ip, r3, lr
   3f690:	bf28      	it	cs
   3f692:	f04c 0c02 	orrcs.w	ip, ip, #2
   3f696:	fa43 f305 	asr.w	r3, r3, r5
   3f69a:	18c0      	adds	r0, r0, r3
   3f69c:	eb51 71e3 	adcs.w	r1, r1, r3, asr #31
   3f6a0:	f001 4500 	and.w	r5, r1, #2147483648	; 0x80000000
   3f6a4:	d507      	bpl.n	3f6b6 <__adddf3+0xe6>
   3f6a6:	f04f 0e00 	mov.w	lr, #0
   3f6aa:	f1dc 0c00 	rsbs	ip, ip, #0
   3f6ae:	eb7e 0000 	sbcs.w	r0, lr, r0
   3f6b2:	eb6e 0101 	sbc.w	r1, lr, r1
   3f6b6:	f5b1 1f80 	cmp.w	r1, #1048576	; 0x100000
   3f6ba:	d31b      	bcc.n	3f6f4 <__adddf3+0x124>
   3f6bc:	f5b1 1f00 	cmp.w	r1, #2097152	; 0x200000
   3f6c0:	d30c      	bcc.n	3f6dc <__adddf3+0x10c>
   3f6c2:	0849      	lsrs	r1, r1, #1
   3f6c4:	ea5f 0030 	movs.w	r0, r0, rrx
   3f6c8:	ea4f 0c3c 	mov.w	ip, ip, rrx
   3f6cc:	f104 0401 	add.w	r4, r4, #1
   3f6d0:	ea4f 5244 	mov.w	r2, r4, lsl #21
   3f6d4:	f512 0f80 	cmn.w	r2, #4194304	; 0x400000
   3f6d8:	f080 809a 	bcs.w	3f810 <__adddf3+0x240>
   3f6dc:	f1bc 4f00 	cmp.w	ip, #2147483648	; 0x80000000
   3f6e0:	bf08      	it	eq
   3f6e2:	ea5f 0c50 	movseq.w	ip, r0, lsr #1
   3f6e6:	f150 0000 	adcs.w	r0, r0, #0
   3f6ea:	eb41 5104 	adc.w	r1, r1, r4, lsl #20
   3f6ee:	ea41 0105 	orr.w	r1, r1, r5
   3f6f2:	bd30      	pop	{r4, r5, pc}
   3f6f4:	ea5f 0c4c 	movs.w	ip, ip, lsl #1
   3f6f8:	4140      	adcs	r0, r0
   3f6fa:	eb41 0101 	adc.w	r1, r1, r1
   3f6fe:	3c01      	subs	r4, #1
   3f700:	bf28      	it	cs
   3f702:	f5b1 1f80 	cmpcs.w	r1, #1048576	; 0x100000
   3f706:	d2e9      	bcs.n	3f6dc <__adddf3+0x10c>
   3f708:	f091 0f00 	teq	r1, #0
   3f70c:	bf04      	itt	eq
   3f70e:	4601      	moveq	r1, r0
   3f710:	2000      	moveq	r0, #0
   3f712:	fab1 f381 	clz	r3, r1
   3f716:	bf08      	it	eq
   3f718:	3320      	addeq	r3, #32
   3f71a:	f1a3 030b 	sub.w	r3, r3, #11
   3f71e:	f1b3 0220 	subs.w	r2, r3, #32
   3f722:	da0c      	bge.n	3f73e <__adddf3+0x16e>
   3f724:	320c      	adds	r2, #12
   3f726:	dd08      	ble.n	3f73a <__adddf3+0x16a>
   3f728:	f102 0c14 	add.w	ip, r2, #20
   3f72c:	f1c2 020c 	rsb	r2, r2, #12
   3f730:	fa01 f00c 	lsl.w	r0, r1, ip
   3f734:	fa21 f102 	lsr.w	r1, r1, r2
   3f738:	e00c      	b.n	3f754 <__adddf3+0x184>
   3f73a:	f102 0214 	add.w	r2, r2, #20
   3f73e:	bfd8      	it	le
   3f740:	f1c2 0c20 	rsble	ip, r2, #32
   3f744:	fa01 f102 	lsl.w	r1, r1, r2
   3f748:	fa20 fc0c 	lsr.w	ip, r0, ip
   3f74c:	bfdc      	itt	le
   3f74e:	ea41 010c 	orrle.w	r1, r1, ip
   3f752:	4090      	lslle	r0, r2
   3f754:	1ae4      	subs	r4, r4, r3
   3f756:	bfa2      	ittt	ge
   3f758:	eb01 5104 	addge.w	r1, r1, r4, lsl #20
   3f75c:	4329      	orrge	r1, r5
   3f75e:	bd30      	popge	{r4, r5, pc}
   3f760:	ea6f 0404 	mvn.w	r4, r4
   3f764:	3c1f      	subs	r4, #31
   3f766:	da1c      	bge.n	3f7a2 <__adddf3+0x1d2>
   3f768:	340c      	adds	r4, #12
   3f76a:	dc0e      	bgt.n	3f78a <__adddf3+0x1ba>
   3f76c:	f104 0414 	add.w	r4, r4, #20
   3f770:	f1c4 0220 	rsb	r2, r4, #32
   3f774:	fa20 f004 	lsr.w	r0, r0, r4
   3f778:	fa01 f302 	lsl.w	r3, r1, r2
   3f77c:	ea40 0003 	orr.w	r0, r0, r3
   3f780:	fa21 f304 	lsr.w	r3, r1, r4
   3f784:	ea45 0103 	orr.w	r1, r5, r3
   3f788:	bd30      	pop	{r4, r5, pc}
   3f78a:	f1c4 040c 	rsb	r4, r4, #12
   3f78e:	f1c4 0220 	rsb	r2, r4, #32
   3f792:	fa20 f002 	lsr.w	r0, r0, r2
   3f796:	fa01 f304 	lsl.w	r3, r1, r4
   3f79a:	ea40 0003 	orr.w	r0, r0, r3
   3f79e:	4629      	mov	r1, r5
   3f7a0:	bd30      	pop	{r4, r5, pc}
   3f7a2:	fa21 f004 	lsr.w	r0, r1, r4
   3f7a6:	4629      	mov	r1, r5
   3f7a8:	bd30      	pop	{r4, r5, pc}
   3f7aa:	f094 0f00 	teq	r4, #0
   3f7ae:	f483 1380 	eor.w	r3, r3, #1048576	; 0x100000
   3f7b2:	bf06      	itte	eq
   3f7b4:	f481 1180 	eoreq.w	r1, r1, #1048576	; 0x100000
   3f7b8:	3401      	addeq	r4, #1
   3f7ba:	3d01      	subne	r5, #1
   3f7bc:	e74e      	b.n	3f65c <__adddf3+0x8c>
   3f7be:	ea7f 5c64 	mvns.w	ip, r4, asr #21
   3f7c2:	bf18      	it	ne
   3f7c4:	ea7f 5c65 	mvnsne.w	ip, r5, asr #21
   3f7c8:	d029      	beq.n	3f81e <__adddf3+0x24e>
   3f7ca:	ea94 0f05 	teq	r4, r5
   3f7ce:	bf08      	it	eq
   3f7d0:	ea90 0f02 	teqeq	r0, r2
   3f7d4:	d005      	beq.n	3f7e2 <__adddf3+0x212>
   3f7d6:	ea54 0c00 	orrs.w	ip, r4, r0
   3f7da:	bf04      	itt	eq
   3f7dc:	4619      	moveq	r1, r3
   3f7de:	4610      	moveq	r0, r2
   3f7e0:	bd30      	pop	{r4, r5, pc}
   3f7e2:	ea91 0f03 	teq	r1, r3
   3f7e6:	bf1e      	ittt	ne
   3f7e8:	2100      	movne	r1, #0
   3f7ea:	2000      	movne	r0, #0
   3f7ec:	bd30      	popne	{r4, r5, pc}
   3f7ee:	ea5f 5c54 	movs.w	ip, r4, lsr #21
   3f7f2:	d105      	bne.n	3f800 <__adddf3+0x230>
   3f7f4:	0040      	lsls	r0, r0, #1
   3f7f6:	4149      	adcs	r1, r1
   3f7f8:	bf28      	it	cs
   3f7fa:	f041 4100 	orrcs.w	r1, r1, #2147483648	; 0x80000000
   3f7fe:	bd30      	pop	{r4, r5, pc}
   3f800:	f514 0480 	adds.w	r4, r4, #4194304	; 0x400000
   3f804:	bf3c      	itt	cc
   3f806:	f501 1180 	addcc.w	r1, r1, #1048576	; 0x100000
   3f80a:	bd30      	popcc	{r4, r5, pc}
   3f80c:	f001 4500 	and.w	r5, r1, #2147483648	; 0x80000000
   3f810:	f045 41fe 	orr.w	r1, r5, #2130706432	; 0x7f000000
   3f814:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
   3f818:	f04f 0000 	mov.w	r0, #0
   3f81c:	bd30      	pop	{r4, r5, pc}
   3f81e:	ea7f 5c64 	mvns.w	ip, r4, asr #21
   3f822:	bf1a      	itte	ne
   3f824:	4619      	movne	r1, r3
   3f826:	4610      	movne	r0, r2
   3f828:	ea7f 5c65 	mvnseq.w	ip, r5, asr #21
   3f82c:	bf1c      	itt	ne
   3f82e:	460b      	movne	r3, r1
   3f830:	4602      	movne	r2, r0
   3f832:	ea50 3401 	orrs.w	r4, r0, r1, lsl #12
   3f836:	bf06      	itte	eq
   3f838:	ea52 3503 	orrseq.w	r5, r2, r3, lsl #12
   3f83c:	ea91 0f03 	teqeq	r1, r3
   3f840:	f441 2100 	orrne.w	r1, r1, #524288	; 0x80000
   3f844:	bd30      	pop	{r4, r5, pc}
   3f846:	bf00      	nop

0003f848 <__aeabi_ui2d>:
   3f848:	f090 0f00 	teq	r0, #0
   3f84c:	bf04      	itt	eq
   3f84e:	2100      	moveq	r1, #0
   3f850:	4770      	bxeq	lr
   3f852:	b530      	push	{r4, r5, lr}
   3f854:	f44f 6480 	mov.w	r4, #1024	; 0x400
   3f858:	f104 0432 	add.w	r4, r4, #50	; 0x32
   3f85c:	f04f 0500 	mov.w	r5, #0
   3f860:	f04f 0100 	mov.w	r1, #0
   3f864:	e750      	b.n	3f708 <__adddf3+0x138>
   3f866:	bf00      	nop

0003f868 <__aeabi_i2d>:
   3f868:	f090 0f00 	teq	r0, #0
   3f86c:	bf04      	itt	eq
   3f86e:	2100      	moveq	r1, #0
   3f870:	4770      	bxeq	lr
   3f872:	b530      	push	{r4, r5, lr}
   3f874:	f44f 6480 	mov.w	r4, #1024	; 0x400
   3f878:	f104 0432 	add.w	r4, r4, #50	; 0x32
   3f87c:	f010 4500 	ands.w	r5, r0, #2147483648	; 0x80000000
   3f880:	bf48      	it	mi
   3f882:	4240      	negmi	r0, r0
   3f884:	f04f 0100 	mov.w	r1, #0
   3f888:	e73e      	b.n	3f708 <__adddf3+0x138>
   3f88a:	bf00      	nop

0003f88c <__aeabi_f2d>:
   3f88c:	0042      	lsls	r2, r0, #1
   3f88e:	ea4f 01e2 	mov.w	r1, r2, asr #3
   3f892:	ea4f 0131 	mov.w	r1, r1, rrx
   3f896:	ea4f 7002 	mov.w	r0, r2, lsl #28
   3f89a:	bf1f      	itttt	ne
   3f89c:	f012 437f 	andsne.w	r3, r2, #4278190080	; 0xff000000
   3f8a0:	f093 4f7f 	teqne	r3, #4278190080	; 0xff000000
   3f8a4:	f081 5160 	eorne.w	r1, r1, #939524096	; 0x38000000
   3f8a8:	4770      	bxne	lr
   3f8aa:	f032 427f 	bics.w	r2, r2, #4278190080	; 0xff000000
   3f8ae:	bf08      	it	eq
   3f8b0:	4770      	bxeq	lr
   3f8b2:	f093 4f7f 	teq	r3, #4278190080	; 0xff000000
   3f8b6:	bf04      	itt	eq
   3f8b8:	f441 2100 	orreq.w	r1, r1, #524288	; 0x80000
   3f8bc:	4770      	bxeq	lr
   3f8be:	b530      	push	{r4, r5, lr}
   3f8c0:	f44f 7460 	mov.w	r4, #896	; 0x380
   3f8c4:	f001 4500 	and.w	r5, r1, #2147483648	; 0x80000000
   3f8c8:	f021 4100 	bic.w	r1, r1, #2147483648	; 0x80000000
   3f8cc:	e71c      	b.n	3f708 <__adddf3+0x138>
   3f8ce:	bf00      	nop

0003f8d0 <__aeabi_ul2d>:
   3f8d0:	ea50 0201 	orrs.w	r2, r0, r1
   3f8d4:	bf08      	it	eq
   3f8d6:	4770      	bxeq	lr
   3f8d8:	b530      	push	{r4, r5, lr}
   3f8da:	f04f 0500 	mov.w	r5, #0
   3f8de:	e00a      	b.n	3f8f6 <__aeabi_l2d+0x16>

0003f8e0 <__aeabi_l2d>:
   3f8e0:	ea50 0201 	orrs.w	r2, r0, r1
   3f8e4:	bf08      	it	eq
   3f8e6:	4770      	bxeq	lr
   3f8e8:	b530      	push	{r4, r5, lr}
   3f8ea:	f011 4500 	ands.w	r5, r1, #2147483648	; 0x80000000
   3f8ee:	d502      	bpl.n	3f8f6 <__aeabi_l2d+0x16>
   3f8f0:	4240      	negs	r0, r0
   3f8f2:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
   3f8f6:	f44f 6480 	mov.w	r4, #1024	; 0x400
   3f8fa:	f104 0432 	add.w	r4, r4, #50	; 0x32
   3f8fe:	ea5f 5c91 	movs.w	ip, r1, lsr #22
   3f902:	f43f aed8 	beq.w	3f6b6 <__adddf3+0xe6>
   3f906:	f04f 0203 	mov.w	r2, #3
   3f90a:	ea5f 0cdc 	movs.w	ip, ip, lsr #3
   3f90e:	bf18      	it	ne
   3f910:	3203      	addne	r2, #3
   3f912:	ea5f 0cdc 	movs.w	ip, ip, lsr #3
   3f916:	bf18      	it	ne
   3f918:	3203      	addne	r2, #3
   3f91a:	eb02 02dc 	add.w	r2, r2, ip, lsr #3
   3f91e:	f1c2 0320 	rsb	r3, r2, #32
   3f922:	fa00 fc03 	lsl.w	ip, r0, r3
   3f926:	fa20 f002 	lsr.w	r0, r0, r2
   3f92a:	fa01 fe03 	lsl.w	lr, r1, r3
   3f92e:	ea40 000e 	orr.w	r0, r0, lr
   3f932:	fa21 f102 	lsr.w	r1, r1, r2
   3f936:	4414      	add	r4, r2
   3f938:	e6bd      	b.n	3f6b6 <__adddf3+0xe6>
   3f93a:	bf00      	nop

0003f93c <__aeabi_dmul>:
   3f93c:	b570      	push	{r4, r5, r6, lr}
   3f93e:	f04f 0cff 	mov.w	ip, #255	; 0xff
   3f942:	f44c 6ce0 	orr.w	ip, ip, #1792	; 0x700
   3f946:	ea1c 5411 	ands.w	r4, ip, r1, lsr #20
   3f94a:	bf1d      	ittte	ne
   3f94c:	ea1c 5513 	andsne.w	r5, ip, r3, lsr #20
   3f950:	ea94 0f0c 	teqne	r4, ip
   3f954:	ea95 0f0c 	teqne	r5, ip
   3f958:	f000 f8de 	bleq	3fb18 <__aeabi_dmul+0x1dc>
   3f95c:	442c      	add	r4, r5
   3f95e:	ea81 0603 	eor.w	r6, r1, r3
   3f962:	ea21 514c 	bic.w	r1, r1, ip, lsl #21
   3f966:	ea23 534c 	bic.w	r3, r3, ip, lsl #21
   3f96a:	ea50 3501 	orrs.w	r5, r0, r1, lsl #12
   3f96e:	bf18      	it	ne
   3f970:	ea52 3503 	orrsne.w	r5, r2, r3, lsl #12
   3f974:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
   3f978:	f443 1380 	orr.w	r3, r3, #1048576	; 0x100000
   3f97c:	d038      	beq.n	3f9f0 <__aeabi_dmul+0xb4>
   3f97e:	fba0 ce02 	umull	ip, lr, r0, r2
   3f982:	f04f 0500 	mov.w	r5, #0
   3f986:	fbe1 e502 	umlal	lr, r5, r1, r2
   3f98a:	f006 4200 	and.w	r2, r6, #2147483648	; 0x80000000
   3f98e:	fbe0 e503 	umlal	lr, r5, r0, r3
   3f992:	f04f 0600 	mov.w	r6, #0
   3f996:	fbe1 5603 	umlal	r5, r6, r1, r3
   3f99a:	f09c 0f00 	teq	ip, #0
   3f99e:	bf18      	it	ne
   3f9a0:	f04e 0e01 	orrne.w	lr, lr, #1
   3f9a4:	f1a4 04ff 	sub.w	r4, r4, #255	; 0xff
   3f9a8:	f5b6 7f00 	cmp.w	r6, #512	; 0x200
   3f9ac:	f564 7440 	sbc.w	r4, r4, #768	; 0x300
   3f9b0:	d204      	bcs.n	3f9bc <__aeabi_dmul+0x80>
   3f9b2:	ea5f 0e4e 	movs.w	lr, lr, lsl #1
   3f9b6:	416d      	adcs	r5, r5
   3f9b8:	eb46 0606 	adc.w	r6, r6, r6
   3f9bc:	ea42 21c6 	orr.w	r1, r2, r6, lsl #11
   3f9c0:	ea41 5155 	orr.w	r1, r1, r5, lsr #21
   3f9c4:	ea4f 20c5 	mov.w	r0, r5, lsl #11
   3f9c8:	ea40 505e 	orr.w	r0, r0, lr, lsr #21
   3f9cc:	ea4f 2ece 	mov.w	lr, lr, lsl #11
   3f9d0:	f1b4 0cfd 	subs.w	ip, r4, #253	; 0xfd
   3f9d4:	bf88      	it	hi
   3f9d6:	f5bc 6fe0 	cmphi.w	ip, #1792	; 0x700
   3f9da:	d81e      	bhi.n	3fa1a <__aeabi_dmul+0xde>
   3f9dc:	f1be 4f00 	cmp.w	lr, #2147483648	; 0x80000000
   3f9e0:	bf08      	it	eq
   3f9e2:	ea5f 0e50 	movseq.w	lr, r0, lsr #1
   3f9e6:	f150 0000 	adcs.w	r0, r0, #0
   3f9ea:	eb41 5104 	adc.w	r1, r1, r4, lsl #20
   3f9ee:	bd70      	pop	{r4, r5, r6, pc}
   3f9f0:	f006 4600 	and.w	r6, r6, #2147483648	; 0x80000000
   3f9f4:	ea46 0101 	orr.w	r1, r6, r1
   3f9f8:	ea40 0002 	orr.w	r0, r0, r2
   3f9fc:	ea81 0103 	eor.w	r1, r1, r3
   3fa00:	ebb4 045c 	subs.w	r4, r4, ip, lsr #1
   3fa04:	bfc2      	ittt	gt
   3fa06:	ebd4 050c 	rsbsgt	r5, r4, ip
   3fa0a:	ea41 5104 	orrgt.w	r1, r1, r4, lsl #20
   3fa0e:	bd70      	popgt	{r4, r5, r6, pc}
   3fa10:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
   3fa14:	f04f 0e00 	mov.w	lr, #0
   3fa18:	3c01      	subs	r4, #1
   3fa1a:	f300 80ab 	bgt.w	3fb74 <__aeabi_dmul+0x238>
   3fa1e:	f114 0f36 	cmn.w	r4, #54	; 0x36
   3fa22:	bfde      	ittt	le
   3fa24:	2000      	movle	r0, #0
   3fa26:	f001 4100 	andle.w	r1, r1, #2147483648	; 0x80000000
   3fa2a:	bd70      	pople	{r4, r5, r6, pc}
   3fa2c:	f1c4 0400 	rsb	r4, r4, #0
   3fa30:	3c20      	subs	r4, #32
   3fa32:	da35      	bge.n	3faa0 <__aeabi_dmul+0x164>
   3fa34:	340c      	adds	r4, #12
   3fa36:	dc1b      	bgt.n	3fa70 <__aeabi_dmul+0x134>
   3fa38:	f104 0414 	add.w	r4, r4, #20
   3fa3c:	f1c4 0520 	rsb	r5, r4, #32
   3fa40:	fa00 f305 	lsl.w	r3, r0, r5
   3fa44:	fa20 f004 	lsr.w	r0, r0, r4
   3fa48:	fa01 f205 	lsl.w	r2, r1, r5
   3fa4c:	ea40 0002 	orr.w	r0, r0, r2
   3fa50:	f001 4200 	and.w	r2, r1, #2147483648	; 0x80000000
   3fa54:	f021 4100 	bic.w	r1, r1, #2147483648	; 0x80000000
   3fa58:	eb10 70d3 	adds.w	r0, r0, r3, lsr #31
   3fa5c:	fa21 f604 	lsr.w	r6, r1, r4
   3fa60:	eb42 0106 	adc.w	r1, r2, r6
   3fa64:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
   3fa68:	bf08      	it	eq
   3fa6a:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
   3fa6e:	bd70      	pop	{r4, r5, r6, pc}
   3fa70:	f1c4 040c 	rsb	r4, r4, #12
   3fa74:	f1c4 0520 	rsb	r5, r4, #32
   3fa78:	fa00 f304 	lsl.w	r3, r0, r4
   3fa7c:	fa20 f005 	lsr.w	r0, r0, r5
   3fa80:	fa01 f204 	lsl.w	r2, r1, r4
   3fa84:	ea40 0002 	orr.w	r0, r0, r2
   3fa88:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
   3fa8c:	eb10 70d3 	adds.w	r0, r0, r3, lsr #31
   3fa90:	f141 0100 	adc.w	r1, r1, #0
   3fa94:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
   3fa98:	bf08      	it	eq
   3fa9a:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
   3fa9e:	bd70      	pop	{r4, r5, r6, pc}
   3faa0:	f1c4 0520 	rsb	r5, r4, #32
   3faa4:	fa00 f205 	lsl.w	r2, r0, r5
   3faa8:	ea4e 0e02 	orr.w	lr, lr, r2
   3faac:	fa20 f304 	lsr.w	r3, r0, r4
   3fab0:	fa01 f205 	lsl.w	r2, r1, r5
   3fab4:	ea43 0302 	orr.w	r3, r3, r2
   3fab8:	fa21 f004 	lsr.w	r0, r1, r4
   3fabc:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
   3fac0:	fa21 f204 	lsr.w	r2, r1, r4
   3fac4:	ea20 0002 	bic.w	r0, r0, r2
   3fac8:	eb00 70d3 	add.w	r0, r0, r3, lsr #31
   3facc:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
   3fad0:	bf08      	it	eq
   3fad2:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
   3fad6:	bd70      	pop	{r4, r5, r6, pc}
   3fad8:	f094 0f00 	teq	r4, #0
   3fadc:	d10f      	bne.n	3fafe <__aeabi_dmul+0x1c2>
   3fade:	f001 4600 	and.w	r6, r1, #2147483648	; 0x80000000
   3fae2:	0040      	lsls	r0, r0, #1
   3fae4:	eb41 0101 	adc.w	r1, r1, r1
   3fae8:	f411 1f80 	tst.w	r1, #1048576	; 0x100000
   3faec:	bf08      	it	eq
   3faee:	3c01      	subeq	r4, #1
   3faf0:	d0f7      	beq.n	3fae2 <__aeabi_dmul+0x1a6>
   3faf2:	ea41 0106 	orr.w	r1, r1, r6
   3faf6:	f095 0f00 	teq	r5, #0
   3fafa:	bf18      	it	ne
   3fafc:	4770      	bxne	lr
   3fafe:	f003 4600 	and.w	r6, r3, #2147483648	; 0x80000000
   3fb02:	0052      	lsls	r2, r2, #1
   3fb04:	eb43 0303 	adc.w	r3, r3, r3
   3fb08:	f413 1f80 	tst.w	r3, #1048576	; 0x100000
   3fb0c:	bf08      	it	eq
   3fb0e:	3d01      	subeq	r5, #1
   3fb10:	d0f7      	beq.n	3fb02 <__aeabi_dmul+0x1c6>
   3fb12:	ea43 0306 	orr.w	r3, r3, r6
   3fb16:	4770      	bx	lr
   3fb18:	ea94 0f0c 	teq	r4, ip
   3fb1c:	ea0c 5513 	and.w	r5, ip, r3, lsr #20
   3fb20:	bf18      	it	ne
   3fb22:	ea95 0f0c 	teqne	r5, ip
   3fb26:	d00c      	beq.n	3fb42 <__aeabi_dmul+0x206>
   3fb28:	ea50 0641 	orrs.w	r6, r0, r1, lsl #1
   3fb2c:	bf18      	it	ne
   3fb2e:	ea52 0643 	orrsne.w	r6, r2, r3, lsl #1
   3fb32:	d1d1      	bne.n	3fad8 <__aeabi_dmul+0x19c>
   3fb34:	ea81 0103 	eor.w	r1, r1, r3
   3fb38:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
   3fb3c:	f04f 0000 	mov.w	r0, #0
   3fb40:	bd70      	pop	{r4, r5, r6, pc}
   3fb42:	ea50 0641 	orrs.w	r6, r0, r1, lsl #1
   3fb46:	bf06      	itte	eq
   3fb48:	4610      	moveq	r0, r2
   3fb4a:	4619      	moveq	r1, r3
   3fb4c:	ea52 0643 	orrsne.w	r6, r2, r3, lsl #1
   3fb50:	d019      	beq.n	3fb86 <__aeabi_dmul+0x24a>
   3fb52:	ea94 0f0c 	teq	r4, ip
   3fb56:	d102      	bne.n	3fb5e <__aeabi_dmul+0x222>
   3fb58:	ea50 3601 	orrs.w	r6, r0, r1, lsl #12
   3fb5c:	d113      	bne.n	3fb86 <__aeabi_dmul+0x24a>
   3fb5e:	ea95 0f0c 	teq	r5, ip
   3fb62:	d105      	bne.n	3fb70 <__aeabi_dmul+0x234>
   3fb64:	ea52 3603 	orrs.w	r6, r2, r3, lsl #12
   3fb68:	bf1c      	itt	ne
   3fb6a:	4610      	movne	r0, r2
   3fb6c:	4619      	movne	r1, r3
   3fb6e:	d10a      	bne.n	3fb86 <__aeabi_dmul+0x24a>
   3fb70:	ea81 0103 	eor.w	r1, r1, r3
   3fb74:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
   3fb78:	f041 41fe 	orr.w	r1, r1, #2130706432	; 0x7f000000
   3fb7c:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
   3fb80:	f04f 0000 	mov.w	r0, #0
   3fb84:	bd70      	pop	{r4, r5, r6, pc}
   3fb86:	f041 41fe 	orr.w	r1, r1, #2130706432	; 0x7f000000
   3fb8a:	f441 0178 	orr.w	r1, r1, #16252928	; 0xf80000
   3fb8e:	bd70      	pop	{r4, r5, r6, pc}

0003fb90 <__aeabi_ddiv>:
   3fb90:	b570      	push	{r4, r5, r6, lr}
   3fb92:	f04f 0cff 	mov.w	ip, #255	; 0xff
   3fb96:	f44c 6ce0 	orr.w	ip, ip, #1792	; 0x700
   3fb9a:	ea1c 5411 	ands.w	r4, ip, r1, lsr #20
   3fb9e:	bf1d      	ittte	ne
   3fba0:	ea1c 5513 	andsne.w	r5, ip, r3, lsr #20
   3fba4:	ea94 0f0c 	teqne	r4, ip
   3fba8:	ea95 0f0c 	teqne	r5, ip
   3fbac:	f000 f8a7 	bleq	3fcfe <__aeabi_ddiv+0x16e>
   3fbb0:	eba4 0405 	sub.w	r4, r4, r5
   3fbb4:	ea81 0e03 	eor.w	lr, r1, r3
   3fbb8:	ea52 3503 	orrs.w	r5, r2, r3, lsl #12
   3fbbc:	ea4f 3101 	mov.w	r1, r1, lsl #12
   3fbc0:	f000 8088 	beq.w	3fcd4 <__aeabi_ddiv+0x144>
   3fbc4:	ea4f 3303 	mov.w	r3, r3, lsl #12
   3fbc8:	f04f 5580 	mov.w	r5, #268435456	; 0x10000000
   3fbcc:	ea45 1313 	orr.w	r3, r5, r3, lsr #4
   3fbd0:	ea43 6312 	orr.w	r3, r3, r2, lsr #24
   3fbd4:	ea4f 2202 	mov.w	r2, r2, lsl #8
   3fbd8:	ea45 1511 	orr.w	r5, r5, r1, lsr #4
   3fbdc:	ea45 6510 	orr.w	r5, r5, r0, lsr #24
   3fbe0:	ea4f 2600 	mov.w	r6, r0, lsl #8
   3fbe4:	f00e 4100 	and.w	r1, lr, #2147483648	; 0x80000000
   3fbe8:	429d      	cmp	r5, r3
   3fbea:	bf08      	it	eq
   3fbec:	4296      	cmpeq	r6, r2
   3fbee:	f144 04fd 	adc.w	r4, r4, #253	; 0xfd
   3fbf2:	f504 7440 	add.w	r4, r4, #768	; 0x300
   3fbf6:	d202      	bcs.n	3fbfe <__aeabi_ddiv+0x6e>
   3fbf8:	085b      	lsrs	r3, r3, #1
   3fbfa:	ea4f 0232 	mov.w	r2, r2, rrx
   3fbfe:	1ab6      	subs	r6, r6, r2
   3fc00:	eb65 0503 	sbc.w	r5, r5, r3
   3fc04:	085b      	lsrs	r3, r3, #1
   3fc06:	ea4f 0232 	mov.w	r2, r2, rrx
   3fc0a:	f44f 1080 	mov.w	r0, #1048576	; 0x100000
   3fc0e:	f44f 2c00 	mov.w	ip, #524288	; 0x80000
   3fc12:	ebb6 0e02 	subs.w	lr, r6, r2
   3fc16:	eb75 0e03 	sbcs.w	lr, r5, r3
   3fc1a:	bf22      	ittt	cs
   3fc1c:	1ab6      	subcs	r6, r6, r2
   3fc1e:	4675      	movcs	r5, lr
   3fc20:	ea40 000c 	orrcs.w	r0, r0, ip
   3fc24:	085b      	lsrs	r3, r3, #1
   3fc26:	ea4f 0232 	mov.w	r2, r2, rrx
   3fc2a:	ebb6 0e02 	subs.w	lr, r6, r2
   3fc2e:	eb75 0e03 	sbcs.w	lr, r5, r3
   3fc32:	bf22      	ittt	cs
   3fc34:	1ab6      	subcs	r6, r6, r2
   3fc36:	4675      	movcs	r5, lr
   3fc38:	ea40 005c 	orrcs.w	r0, r0, ip, lsr #1
   3fc3c:	085b      	lsrs	r3, r3, #1
   3fc3e:	ea4f 0232 	mov.w	r2, r2, rrx
   3fc42:	ebb6 0e02 	subs.w	lr, r6, r2
   3fc46:	eb75 0e03 	sbcs.w	lr, r5, r3
   3fc4a:	bf22      	ittt	cs
   3fc4c:	1ab6      	subcs	r6, r6, r2
   3fc4e:	4675      	movcs	r5, lr
   3fc50:	ea40 009c 	orrcs.w	r0, r0, ip, lsr #2
   3fc54:	085b      	lsrs	r3, r3, #1
   3fc56:	ea4f 0232 	mov.w	r2, r2, rrx
   3fc5a:	ebb6 0e02 	subs.w	lr, r6, r2
   3fc5e:	eb75 0e03 	sbcs.w	lr, r5, r3
   3fc62:	bf22      	ittt	cs
   3fc64:	1ab6      	subcs	r6, r6, r2
   3fc66:	4675      	movcs	r5, lr
   3fc68:	ea40 00dc 	orrcs.w	r0, r0, ip, lsr #3
   3fc6c:	ea55 0e06 	orrs.w	lr, r5, r6
   3fc70:	d018      	beq.n	3fca4 <__aeabi_ddiv+0x114>
   3fc72:	ea4f 1505 	mov.w	r5, r5, lsl #4
   3fc76:	ea45 7516 	orr.w	r5, r5, r6, lsr #28
   3fc7a:	ea4f 1606 	mov.w	r6, r6, lsl #4
   3fc7e:	ea4f 03c3 	mov.w	r3, r3, lsl #3
   3fc82:	ea43 7352 	orr.w	r3, r3, r2, lsr #29
   3fc86:	ea4f 02c2 	mov.w	r2, r2, lsl #3
   3fc8a:	ea5f 1c1c 	movs.w	ip, ip, lsr #4
   3fc8e:	d1c0      	bne.n	3fc12 <__aeabi_ddiv+0x82>
   3fc90:	f411 1f80 	tst.w	r1, #1048576	; 0x100000
   3fc94:	d10b      	bne.n	3fcae <__aeabi_ddiv+0x11e>
   3fc96:	ea41 0100 	orr.w	r1, r1, r0
   3fc9a:	f04f 0000 	mov.w	r0, #0
   3fc9e:	f04f 4c00 	mov.w	ip, #2147483648	; 0x80000000
   3fca2:	e7b6      	b.n	3fc12 <__aeabi_ddiv+0x82>
   3fca4:	f411 1f80 	tst.w	r1, #1048576	; 0x100000
   3fca8:	bf04      	itt	eq
   3fcaa:	4301      	orreq	r1, r0
   3fcac:	2000      	moveq	r0, #0
   3fcae:	f1b4 0cfd 	subs.w	ip, r4, #253	; 0xfd
   3fcb2:	bf88      	it	hi
   3fcb4:	f5bc 6fe0 	cmphi.w	ip, #1792	; 0x700
   3fcb8:	f63f aeaf 	bhi.w	3fa1a <__aeabi_dmul+0xde>
   3fcbc:	ebb5 0c03 	subs.w	ip, r5, r3
   3fcc0:	bf04      	itt	eq
   3fcc2:	ebb6 0c02 	subseq.w	ip, r6, r2
   3fcc6:	ea5f 0c50 	movseq.w	ip, r0, lsr #1
   3fcca:	f150 0000 	adcs.w	r0, r0, #0
   3fcce:	eb41 5104 	adc.w	r1, r1, r4, lsl #20
   3fcd2:	bd70      	pop	{r4, r5, r6, pc}
   3fcd4:	f00e 4e00 	and.w	lr, lr, #2147483648	; 0x80000000
   3fcd8:	ea4e 3111 	orr.w	r1, lr, r1, lsr #12
   3fcdc:	eb14 045c 	adds.w	r4, r4, ip, lsr #1
   3fce0:	bfc2      	ittt	gt
   3fce2:	ebd4 050c 	rsbsgt	r5, r4, ip
   3fce6:	ea41 5104 	orrgt.w	r1, r1, r4, lsl #20
   3fcea:	bd70      	popgt	{r4, r5, r6, pc}
   3fcec:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
   3fcf0:	f04f 0e00 	mov.w	lr, #0
   3fcf4:	3c01      	subs	r4, #1
   3fcf6:	e690      	b.n	3fa1a <__aeabi_dmul+0xde>
   3fcf8:	ea45 0e06 	orr.w	lr, r5, r6
   3fcfc:	e68d      	b.n	3fa1a <__aeabi_dmul+0xde>
   3fcfe:	ea0c 5513 	and.w	r5, ip, r3, lsr #20
   3fd02:	ea94 0f0c 	teq	r4, ip
   3fd06:	bf08      	it	eq
   3fd08:	ea95 0f0c 	teqeq	r5, ip
   3fd0c:	f43f af3b 	beq.w	3fb86 <__aeabi_dmul+0x24a>
   3fd10:	ea94 0f0c 	teq	r4, ip
   3fd14:	d10a      	bne.n	3fd2c <__aeabi_ddiv+0x19c>
   3fd16:	ea50 3401 	orrs.w	r4, r0, r1, lsl #12
   3fd1a:	f47f af34 	bne.w	3fb86 <__aeabi_dmul+0x24a>
   3fd1e:	ea95 0f0c 	teq	r5, ip
   3fd22:	f47f af25 	bne.w	3fb70 <__aeabi_dmul+0x234>
   3fd26:	4610      	mov	r0, r2
   3fd28:	4619      	mov	r1, r3
   3fd2a:	e72c      	b.n	3fb86 <__aeabi_dmul+0x24a>
   3fd2c:	ea95 0f0c 	teq	r5, ip
   3fd30:	d106      	bne.n	3fd40 <__aeabi_ddiv+0x1b0>
   3fd32:	ea52 3503 	orrs.w	r5, r2, r3, lsl #12
   3fd36:	f43f aefd 	beq.w	3fb34 <__aeabi_dmul+0x1f8>
   3fd3a:	4610      	mov	r0, r2
   3fd3c:	4619      	mov	r1, r3
   3fd3e:	e722      	b.n	3fb86 <__aeabi_dmul+0x24a>
   3fd40:	ea50 0641 	orrs.w	r6, r0, r1, lsl #1
   3fd44:	bf18      	it	ne
   3fd46:	ea52 0643 	orrsne.w	r6, r2, r3, lsl #1
   3fd4a:	f47f aec5 	bne.w	3fad8 <__aeabi_dmul+0x19c>
   3fd4e:	ea50 0441 	orrs.w	r4, r0, r1, lsl #1
   3fd52:	f47f af0d 	bne.w	3fb70 <__aeabi_dmul+0x234>
   3fd56:	ea52 0543 	orrs.w	r5, r2, r3, lsl #1
   3fd5a:	f47f aeeb 	bne.w	3fb34 <__aeabi_dmul+0x1f8>
   3fd5e:	e712      	b.n	3fb86 <__aeabi_dmul+0x24a>

0003fd60 <__gedf2>:
   3fd60:	f04f 3cff 	mov.w	ip, #4294967295	; 0xffffffff
   3fd64:	e006      	b.n	3fd74 <__cmpdf2+0x4>
   3fd66:	bf00      	nop

0003fd68 <__ledf2>:
   3fd68:	f04f 0c01 	mov.w	ip, #1
   3fd6c:	e002      	b.n	3fd74 <__cmpdf2+0x4>
   3fd6e:	bf00      	nop

0003fd70 <__cmpdf2>:
   3fd70:	f04f 0c01 	mov.w	ip, #1
   3fd74:	f84d cd04 	str.w	ip, [sp, #-4]!
   3fd78:	ea4f 0c41 	mov.w	ip, r1, lsl #1
   3fd7c:	ea7f 5c6c 	mvns.w	ip, ip, asr #21
   3fd80:	ea4f 0c43 	mov.w	ip, r3, lsl #1
   3fd84:	bf18      	it	ne
   3fd86:	ea7f 5c6c 	mvnsne.w	ip, ip, asr #21
   3fd8a:	d01b      	beq.n	3fdc4 <__cmpdf2+0x54>
   3fd8c:	b001      	add	sp, #4
   3fd8e:	ea50 0c41 	orrs.w	ip, r0, r1, lsl #1
   3fd92:	bf0c      	ite	eq
   3fd94:	ea52 0c43 	orrseq.w	ip, r2, r3, lsl #1
   3fd98:	ea91 0f03 	teqne	r1, r3
   3fd9c:	bf02      	ittt	eq
   3fd9e:	ea90 0f02 	teqeq	r0, r2
   3fda2:	2000      	moveq	r0, #0
   3fda4:	4770      	bxeq	lr
   3fda6:	f110 0f00 	cmn.w	r0, #0
   3fdaa:	ea91 0f03 	teq	r1, r3
   3fdae:	bf58      	it	pl
   3fdb0:	4299      	cmppl	r1, r3
   3fdb2:	bf08      	it	eq
   3fdb4:	4290      	cmpeq	r0, r2
   3fdb6:	bf2c      	ite	cs
   3fdb8:	17d8      	asrcs	r0, r3, #31
   3fdba:	ea6f 70e3 	mvncc.w	r0, r3, asr #31
   3fdbe:	f040 0001 	orr.w	r0, r0, #1
   3fdc2:	4770      	bx	lr
   3fdc4:	ea4f 0c41 	mov.w	ip, r1, lsl #1
   3fdc8:	ea7f 5c6c 	mvns.w	ip, ip, asr #21
   3fdcc:	d102      	bne.n	3fdd4 <__cmpdf2+0x64>
   3fdce:	ea50 3c01 	orrs.w	ip, r0, r1, lsl #12
   3fdd2:	d107      	bne.n	3fde4 <__cmpdf2+0x74>
   3fdd4:	ea4f 0c43 	mov.w	ip, r3, lsl #1
   3fdd8:	ea7f 5c6c 	mvns.w	ip, ip, asr #21
   3fddc:	d1d6      	bne.n	3fd8c <__cmpdf2+0x1c>
   3fdde:	ea52 3c03 	orrs.w	ip, r2, r3, lsl #12
   3fde2:	d0d3      	beq.n	3fd8c <__cmpdf2+0x1c>
   3fde4:	f85d 0b04 	ldr.w	r0, [sp], #4
   3fde8:	4770      	bx	lr
   3fdea:	bf00      	nop

0003fdec <__aeabi_cdrcmple>:
   3fdec:	4684      	mov	ip, r0
   3fdee:	4610      	mov	r0, r2
   3fdf0:	4662      	mov	r2, ip
   3fdf2:	468c      	mov	ip, r1
   3fdf4:	4619      	mov	r1, r3
   3fdf6:	4663      	mov	r3, ip
   3fdf8:	e000      	b.n	3fdfc <__aeabi_cdcmpeq>
   3fdfa:	bf00      	nop

0003fdfc <__aeabi_cdcmpeq>:
   3fdfc:	b501      	push	{r0, lr}
   3fdfe:	f7ff ffb7 	bl	3fd70 <__cmpdf2>
   3fe02:	2800      	cmp	r0, #0
   3fe04:	bf48      	it	mi
   3fe06:	f110 0f00 	cmnmi.w	r0, #0
   3fe0a:	bd01      	pop	{r0, pc}

0003fe0c <__aeabi_dcmpeq>:
   3fe0c:	f84d ed08 	str.w	lr, [sp, #-8]!
   3fe10:	f7ff fff4 	bl	3fdfc <__aeabi_cdcmpeq>
   3fe14:	bf0c      	ite	eq
   3fe16:	2001      	moveq	r0, #1
   3fe18:	2000      	movne	r0, #0
   3fe1a:	f85d fb08 	ldr.w	pc, [sp], #8
   3fe1e:	bf00      	nop

0003fe20 <__aeabi_dcmplt>:
   3fe20:	f84d ed08 	str.w	lr, [sp, #-8]!
   3fe24:	f7ff ffea 	bl	3fdfc <__aeabi_cdcmpeq>
   3fe28:	bf34      	ite	cc
   3fe2a:	2001      	movcc	r0, #1
   3fe2c:	2000      	movcs	r0, #0
   3fe2e:	f85d fb08 	ldr.w	pc, [sp], #8
   3fe32:	bf00      	nop

0003fe34 <__aeabi_dcmple>:
   3fe34:	f84d ed08 	str.w	lr, [sp, #-8]!
   3fe38:	f7ff ffe0 	bl	3fdfc <__aeabi_cdcmpeq>
   3fe3c:	bf94      	ite	ls
   3fe3e:	2001      	movls	r0, #1
   3fe40:	2000      	movhi	r0, #0
   3fe42:	f85d fb08 	ldr.w	pc, [sp], #8
   3fe46:	bf00      	nop

0003fe48 <__aeabi_dcmpge>:
   3fe48:	f84d ed08 	str.w	lr, [sp, #-8]!
   3fe4c:	f7ff ffce 	bl	3fdec <__aeabi_cdrcmple>
   3fe50:	bf94      	ite	ls
   3fe52:	2001      	movls	r0, #1
   3fe54:	2000      	movhi	r0, #0
   3fe56:	f85d fb08 	ldr.w	pc, [sp], #8
   3fe5a:	bf00      	nop

0003fe5c <__aeabi_dcmpgt>:
   3fe5c:	f84d ed08 	str.w	lr, [sp, #-8]!
   3fe60:	f7ff ffc4 	bl	3fdec <__aeabi_cdrcmple>
   3fe64:	bf34      	ite	cc
   3fe66:	2001      	movcc	r0, #1
   3fe68:	2000      	movcs	r0, #0
   3fe6a:	f85d fb08 	ldr.w	pc, [sp], #8
   3fe6e:	bf00      	nop

0003fe70 <__aeabi_dcmpun>:
   3fe70:	ea4f 0c41 	mov.w	ip, r1, lsl #1
   3fe74:	ea7f 5c6c 	mvns.w	ip, ip, asr #21
   3fe78:	d102      	bne.n	3fe80 <__aeabi_dcmpun+0x10>
   3fe7a:	ea50 3c01 	orrs.w	ip, r0, r1, lsl #12
   3fe7e:	d10a      	bne.n	3fe96 <__aeabi_dcmpun+0x26>
   3fe80:	ea4f 0c43 	mov.w	ip, r3, lsl #1
   3fe84:	ea7f 5c6c 	mvns.w	ip, ip, asr #21
   3fe88:	d102      	bne.n	3fe90 <__aeabi_dcmpun+0x20>
   3fe8a:	ea52 3c03 	orrs.w	ip, r2, r3, lsl #12
   3fe8e:	d102      	bne.n	3fe96 <__aeabi_dcmpun+0x26>
   3fe90:	f04f 0000 	mov.w	r0, #0
   3fe94:	4770      	bx	lr
   3fe96:	f04f 0001 	mov.w	r0, #1
   3fe9a:	4770      	bx	lr

0003fe9c <__aeabi_d2iz>:
   3fe9c:	ea4f 0241 	mov.w	r2, r1, lsl #1
   3fea0:	f512 1200 	adds.w	r2, r2, #2097152	; 0x200000
   3fea4:	d215      	bcs.n	3fed2 <__aeabi_d2iz+0x36>
   3fea6:	d511      	bpl.n	3fecc <__aeabi_d2iz+0x30>
   3fea8:	f46f 7378 	mvn.w	r3, #992	; 0x3e0
   3feac:	ebb3 5262 	subs.w	r2, r3, r2, asr #21
   3feb0:	d912      	bls.n	3fed8 <__aeabi_d2iz+0x3c>
   3feb2:	ea4f 23c1 	mov.w	r3, r1, lsl #11
   3feb6:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
   3feba:	ea43 5350 	orr.w	r3, r3, r0, lsr #21
   3febe:	f011 4f00 	tst.w	r1, #2147483648	; 0x80000000
   3fec2:	fa23 f002 	lsr.w	r0, r3, r2
   3fec6:	bf18      	it	ne
   3fec8:	4240      	negne	r0, r0
   3feca:	4770      	bx	lr
   3fecc:	f04f 0000 	mov.w	r0, #0
   3fed0:	4770      	bx	lr
   3fed2:	ea50 3001 	orrs.w	r0, r0, r1, lsl #12
   3fed6:	d105      	bne.n	3fee4 <__aeabi_d2iz+0x48>
   3fed8:	f011 4000 	ands.w	r0, r1, #2147483648	; 0x80000000
   3fedc:	bf08      	it	eq
   3fede:	f06f 4000 	mvneq.w	r0, #2147483648	; 0x80000000
   3fee2:	4770      	bx	lr
   3fee4:	f04f 0000 	mov.w	r0, #0
   3fee8:	4770      	bx	lr
   3feea:	bf00      	nop

0003feec <__aeabi_d2uiz>:
   3feec:	004a      	lsls	r2, r1, #1
   3feee:	d211      	bcs.n	3ff14 <__aeabi_d2uiz+0x28>
   3fef0:	f512 1200 	adds.w	r2, r2, #2097152	; 0x200000
   3fef4:	d211      	bcs.n	3ff1a <__aeabi_d2uiz+0x2e>
   3fef6:	d50d      	bpl.n	3ff14 <__aeabi_d2uiz+0x28>
   3fef8:	f46f 7378 	mvn.w	r3, #992	; 0x3e0
   3fefc:	ebb3 5262 	subs.w	r2, r3, r2, asr #21
   3ff00:	d40e      	bmi.n	3ff20 <__aeabi_d2uiz+0x34>
   3ff02:	ea4f 23c1 	mov.w	r3, r1, lsl #11
   3ff06:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
   3ff0a:	ea43 5350 	orr.w	r3, r3, r0, lsr #21
   3ff0e:	fa23 f002 	lsr.w	r0, r3, r2
   3ff12:	4770      	bx	lr
   3ff14:	f04f 0000 	mov.w	r0, #0
   3ff18:	4770      	bx	lr
   3ff1a:	ea50 3001 	orrs.w	r0, r0, r1, lsl #12
   3ff1e:	d102      	bne.n	3ff26 <__aeabi_d2uiz+0x3a>
   3ff20:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   3ff24:	4770      	bx	lr
   3ff26:	f04f 0000 	mov.w	r0, #0
   3ff2a:	4770      	bx	lr

0003ff2c <__aeabi_d2f>:
   3ff2c:	ea4f 0241 	mov.w	r2, r1, lsl #1
   3ff30:	f1b2 43e0 	subs.w	r3, r2, #1879048192	; 0x70000000
   3ff34:	bf24      	itt	cs
   3ff36:	f5b3 1c00 	subscs.w	ip, r3, #2097152	; 0x200000
   3ff3a:	f1dc 5cfe 	rsbscs	ip, ip, #532676608	; 0x1fc00000
   3ff3e:	d90d      	bls.n	3ff5c <__aeabi_d2f+0x30>
   3ff40:	f001 4c00 	and.w	ip, r1, #2147483648	; 0x80000000
   3ff44:	ea4f 02c0 	mov.w	r2, r0, lsl #3
   3ff48:	ea4c 7050 	orr.w	r0, ip, r0, lsr #29
   3ff4c:	f1b2 4f00 	cmp.w	r2, #2147483648	; 0x80000000
   3ff50:	eb40 0083 	adc.w	r0, r0, r3, lsl #2
   3ff54:	bf08      	it	eq
   3ff56:	f020 0001 	biceq.w	r0, r0, #1
   3ff5a:	4770      	bx	lr
   3ff5c:	f011 4f80 	tst.w	r1, #1073741824	; 0x40000000
   3ff60:	d121      	bne.n	3ffa6 <__aeabi_d2f+0x7a>
   3ff62:	f113 7238 	adds.w	r2, r3, #48234496	; 0x2e00000
   3ff66:	bfbc      	itt	lt
   3ff68:	f001 4000 	andlt.w	r0, r1, #2147483648	; 0x80000000
   3ff6c:	4770      	bxlt	lr
   3ff6e:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
   3ff72:	ea4f 5252 	mov.w	r2, r2, lsr #21
   3ff76:	f1c2 0218 	rsb	r2, r2, #24
   3ff7a:	f1c2 0c20 	rsb	ip, r2, #32
   3ff7e:	fa10 f30c 	lsls.w	r3, r0, ip
   3ff82:	fa20 f002 	lsr.w	r0, r0, r2
   3ff86:	bf18      	it	ne
   3ff88:	f040 0001 	orrne.w	r0, r0, #1
   3ff8c:	ea4f 23c1 	mov.w	r3, r1, lsl #11
   3ff90:	ea4f 23d3 	mov.w	r3, r3, lsr #11
   3ff94:	fa03 fc0c 	lsl.w	ip, r3, ip
   3ff98:	ea40 000c 	orr.w	r0, r0, ip
   3ff9c:	fa23 f302 	lsr.w	r3, r3, r2
   3ffa0:	ea4f 0343 	mov.w	r3, r3, lsl #1
   3ffa4:	e7cc      	b.n	3ff40 <__aeabi_d2f+0x14>
   3ffa6:	ea7f 5362 	mvns.w	r3, r2, asr #21
   3ffaa:	d107      	bne.n	3ffbc <__aeabi_d2f+0x90>
   3ffac:	ea50 3301 	orrs.w	r3, r0, r1, lsl #12
   3ffb0:	bf1e      	ittt	ne
   3ffb2:	f04f 40fe 	movne.w	r0, #2130706432	; 0x7f000000
   3ffb6:	f440 0040 	orrne.w	r0, r0, #12582912	; 0xc00000
   3ffba:	4770      	bxne	lr
   3ffbc:	f001 4000 	and.w	r0, r1, #2147483648	; 0x80000000
   3ffc0:	f040 40fe 	orr.w	r0, r0, #2130706432	; 0x7f000000
   3ffc4:	f440 0000 	orr.w	r0, r0, #8388608	; 0x800000
   3ffc8:	4770      	bx	lr
   3ffca:	bf00      	nop

0003ffcc <__aeabi_uldivmod>:
   3ffcc:	b953      	cbnz	r3, 3ffe4 <__aeabi_uldivmod+0x18>
   3ffce:	b94a      	cbnz	r2, 3ffe4 <__aeabi_uldivmod+0x18>
   3ffd0:	2900      	cmp	r1, #0
   3ffd2:	bf08      	it	eq
   3ffd4:	2800      	cmpeq	r0, #0
   3ffd6:	bf1c      	itt	ne
   3ffd8:	f04f 31ff 	movne.w	r1, #4294967295	; 0xffffffff
   3ffdc:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
   3ffe0:	f000 b996 	b.w	40310 <__aeabi_idiv0>
   3ffe4:	f1ad 0c08 	sub.w	ip, sp, #8
   3ffe8:	e96d ce04 	strd	ip, lr, [sp, #-16]!
   3ffec:	f000 f83c 	bl	40068 <__udivmoddi4>
   3fff0:	f8dd e004 	ldr.w	lr, [sp, #4]
   3fff4:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
   3fff8:	b004      	add	sp, #16
   3fffa:	4770      	bx	lr

0003fffc <__aeabi_d2lz>:
   3fffc:	b538      	push	{r3, r4, r5, lr}
   3fffe:	2200      	movs	r2, #0
   40000:	2300      	movs	r3, #0
   40002:	4604      	mov	r4, r0
   40004:	460d      	mov	r5, r1
   40006:	f7ff ff0b 	bl	3fe20 <__aeabi_dcmplt>
   4000a:	b928      	cbnz	r0, 40018 <__aeabi_d2lz+0x1c>
   4000c:	4620      	mov	r0, r4
   4000e:	4629      	mov	r1, r5
   40010:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
   40014:	f000 b80a 	b.w	4002c <__aeabi_d2ulz>
   40018:	4620      	mov	r0, r4
   4001a:	f105 4100 	add.w	r1, r5, #2147483648	; 0x80000000
   4001e:	f000 f805 	bl	4002c <__aeabi_d2ulz>
   40022:	4240      	negs	r0, r0
   40024:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
   40028:	bd38      	pop	{r3, r4, r5, pc}
	...

0004002c <__aeabi_d2ulz>:
   4002c:	b5d0      	push	{r4, r6, r7, lr}
   4002e:	2200      	movs	r2, #0
   40030:	4b0b      	ldr	r3, [pc, #44]	; (40060 <__aeabi_d2ulz+0x34>)
   40032:	4606      	mov	r6, r0
   40034:	460f      	mov	r7, r1
   40036:	f7ff fc81 	bl	3f93c <__aeabi_dmul>
   4003a:	f7ff ff57 	bl	3feec <__aeabi_d2uiz>
   4003e:	4604      	mov	r4, r0
   40040:	f7ff fc02 	bl	3f848 <__aeabi_ui2d>
   40044:	2200      	movs	r2, #0
   40046:	4b07      	ldr	r3, [pc, #28]	; (40064 <__aeabi_d2ulz+0x38>)
   40048:	f7ff fc78 	bl	3f93c <__aeabi_dmul>
   4004c:	4602      	mov	r2, r0
   4004e:	460b      	mov	r3, r1
   40050:	4630      	mov	r0, r6
   40052:	4639      	mov	r1, r7
   40054:	f7ff faba 	bl	3f5cc <__aeabi_dsub>
   40058:	f7ff ff48 	bl	3feec <__aeabi_d2uiz>
   4005c:	4621      	mov	r1, r4
   4005e:	bdd0      	pop	{r4, r6, r7, pc}
   40060:	3df00000 	.word	0x3df00000
   40064:	41f00000 	.word	0x41f00000

00040068 <__udivmoddi4>:
   40068:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4006c:	9e09      	ldr	r6, [sp, #36]	; 0x24
   4006e:	4688      	mov	r8, r1
   40070:	468e      	mov	lr, r1
   40072:	4615      	mov	r5, r2
   40074:	4604      	mov	r4, r0
   40076:	4619      	mov	r1, r3
   40078:	2b00      	cmp	r3, #0
   4007a:	f040 80c6 	bne.w	4020a <__udivmoddi4+0x1a2>
   4007e:	4542      	cmp	r2, r8
   40080:	fab2 f782 	clz	r7, r2
   40084:	d946      	bls.n	40114 <__udivmoddi4+0xac>
   40086:	b14f      	cbz	r7, 4009c <__udivmoddi4+0x34>
   40088:	f1c7 0c20 	rsb	ip, r7, #32
   4008c:	fa08 f307 	lsl.w	r3, r8, r7
   40090:	40bd      	lsls	r5, r7
   40092:	40bc      	lsls	r4, r7
   40094:	fa20 fc0c 	lsr.w	ip, r0, ip
   40098:	ea4c 0e03 	orr.w	lr, ip, r3
   4009c:	ea4f 4815 	mov.w	r8, r5, lsr #16
   400a0:	fa1f fc85 	uxth.w	ip, r5
   400a4:	0c22      	lsrs	r2, r4, #16
   400a6:	fbbe f9f8 	udiv	r9, lr, r8
   400aa:	fb08 e319 	mls	r3, r8, r9, lr
   400ae:	fb09 fa0c 	mul.w	sl, r9, ip
   400b2:	ea42 4303 	orr.w	r3, r2, r3, lsl #16
   400b6:	459a      	cmp	sl, r3
   400b8:	d928      	bls.n	4010c <__udivmoddi4+0xa4>
   400ba:	18eb      	adds	r3, r5, r3
   400bc:	f109 30ff 	add.w	r0, r9, #4294967295	; 0xffffffff
   400c0:	d204      	bcs.n	400cc <__udivmoddi4+0x64>
   400c2:	459a      	cmp	sl, r3
   400c4:	d902      	bls.n	400cc <__udivmoddi4+0x64>
   400c6:	f1a9 0002 	sub.w	r0, r9, #2
   400ca:	442b      	add	r3, r5
   400cc:	eba3 030a 	sub.w	r3, r3, sl
   400d0:	b2a4      	uxth	r4, r4
   400d2:	fbb3 f2f8 	udiv	r2, r3, r8
   400d6:	fb08 3312 	mls	r3, r8, r2, r3
   400da:	fb02 fc0c 	mul.w	ip, r2, ip
   400de:	ea44 4403 	orr.w	r4, r4, r3, lsl #16
   400e2:	45a4      	cmp	ip, r4
   400e4:	d914      	bls.n	40110 <__udivmoddi4+0xa8>
   400e6:	192c      	adds	r4, r5, r4
   400e8:	f102 33ff 	add.w	r3, r2, #4294967295	; 0xffffffff
   400ec:	d203      	bcs.n	400f6 <__udivmoddi4+0x8e>
   400ee:	45a4      	cmp	ip, r4
   400f0:	d901      	bls.n	400f6 <__udivmoddi4+0x8e>
   400f2:	1e93      	subs	r3, r2, #2
   400f4:	442c      	add	r4, r5
   400f6:	eba4 040c 	sub.w	r4, r4, ip
   400fa:	ea43 4000 	orr.w	r0, r3, r0, lsl #16
   400fe:	b11e      	cbz	r6, 40108 <__udivmoddi4+0xa0>
   40100:	40fc      	lsrs	r4, r7
   40102:	2300      	movs	r3, #0
   40104:	6034      	str	r4, [r6, #0]
   40106:	6073      	str	r3, [r6, #4]
   40108:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
   4010c:	4648      	mov	r0, r9
   4010e:	e7dd      	b.n	400cc <__udivmoddi4+0x64>
   40110:	4613      	mov	r3, r2
   40112:	e7f0      	b.n	400f6 <__udivmoddi4+0x8e>
   40114:	b902      	cbnz	r2, 40118 <__udivmoddi4+0xb0>
   40116:	deff      	udf	#255	; 0xff
   40118:	bb8f      	cbnz	r7, 4017e <__udivmoddi4+0x116>
   4011a:	eba8 0302 	sub.w	r3, r8, r2
   4011e:	2101      	movs	r1, #1
   40120:	ea4f 4e15 	mov.w	lr, r5, lsr #16
   40124:	b2aa      	uxth	r2, r5
   40126:	0c20      	lsrs	r0, r4, #16
   40128:	fbb3 fcfe 	udiv	ip, r3, lr
   4012c:	fb0e 331c 	mls	r3, lr, ip, r3
   40130:	fb0c f802 	mul.w	r8, ip, r2
   40134:	ea40 4303 	orr.w	r3, r0, r3, lsl #16
   40138:	4598      	cmp	r8, r3
   4013a:	d962      	bls.n	40202 <__udivmoddi4+0x19a>
   4013c:	18eb      	adds	r3, r5, r3
   4013e:	f10c 30ff 	add.w	r0, ip, #4294967295	; 0xffffffff
   40142:	d204      	bcs.n	4014e <__udivmoddi4+0xe6>
   40144:	4598      	cmp	r8, r3
   40146:	d902      	bls.n	4014e <__udivmoddi4+0xe6>
   40148:	f1ac 0002 	sub.w	r0, ip, #2
   4014c:	442b      	add	r3, r5
   4014e:	eba3 0308 	sub.w	r3, r3, r8
   40152:	b2a4      	uxth	r4, r4
   40154:	fbb3 fcfe 	udiv	ip, r3, lr
   40158:	fb0e 331c 	mls	r3, lr, ip, r3
   4015c:	fb0c f202 	mul.w	r2, ip, r2
   40160:	ea44 4403 	orr.w	r4, r4, r3, lsl #16
   40164:	42a2      	cmp	r2, r4
   40166:	d94e      	bls.n	40206 <__udivmoddi4+0x19e>
   40168:	192c      	adds	r4, r5, r4
   4016a:	f10c 33ff 	add.w	r3, ip, #4294967295	; 0xffffffff
   4016e:	d204      	bcs.n	4017a <__udivmoddi4+0x112>
   40170:	42a2      	cmp	r2, r4
   40172:	d902      	bls.n	4017a <__udivmoddi4+0x112>
   40174:	f1ac 0302 	sub.w	r3, ip, #2
   40178:	442c      	add	r4, r5
   4017a:	1aa4      	subs	r4, r4, r2
   4017c:	e7bd      	b.n	400fa <__udivmoddi4+0x92>
   4017e:	f1c7 0c20 	rsb	ip, r7, #32
   40182:	40bd      	lsls	r5, r7
   40184:	fa08 f307 	lsl.w	r3, r8, r7
   40188:	40bc      	lsls	r4, r7
   4018a:	fa28 f90c 	lsr.w	r9, r8, ip
   4018e:	ea4f 4e15 	mov.w	lr, r5, lsr #16
   40192:	fa20 fc0c 	lsr.w	ip, r0, ip
   40196:	ea4c 0203 	orr.w	r2, ip, r3
   4019a:	b2ab      	uxth	r3, r5
   4019c:	fbb9 fcfe 	udiv	ip, r9, lr
   401a0:	0c11      	lsrs	r1, r2, #16
   401a2:	fb0e 901c 	mls	r0, lr, ip, r9
   401a6:	fb0c f803 	mul.w	r8, ip, r3
   401aa:	ea41 4000 	orr.w	r0, r1, r0, lsl #16
   401ae:	4580      	cmp	r8, r0
   401b0:	d923      	bls.n	401fa <__udivmoddi4+0x192>
   401b2:	1828      	adds	r0, r5, r0
   401b4:	f10c 31ff 	add.w	r1, ip, #4294967295	; 0xffffffff
   401b8:	d204      	bcs.n	401c4 <__udivmoddi4+0x15c>
   401ba:	4580      	cmp	r8, r0
   401bc:	d902      	bls.n	401c4 <__udivmoddi4+0x15c>
   401be:	f1ac 0102 	sub.w	r1, ip, #2
   401c2:	4428      	add	r0, r5
   401c4:	eba0 0008 	sub.w	r0, r0, r8
   401c8:	b292      	uxth	r2, r2
   401ca:	fbb0 fcfe 	udiv	ip, r0, lr
   401ce:	fb0e 001c 	mls	r0, lr, ip, r0
   401d2:	fb0c f803 	mul.w	r8, ip, r3
   401d6:	ea42 4300 	orr.w	r3, r2, r0, lsl #16
   401da:	4598      	cmp	r8, r3
   401dc:	d90f      	bls.n	401fe <__udivmoddi4+0x196>
   401de:	18eb      	adds	r3, r5, r3
   401e0:	f10c 32ff 	add.w	r2, ip, #4294967295	; 0xffffffff
   401e4:	d204      	bcs.n	401f0 <__udivmoddi4+0x188>
   401e6:	4598      	cmp	r8, r3
   401e8:	d902      	bls.n	401f0 <__udivmoddi4+0x188>
   401ea:	f1ac 0202 	sub.w	r2, ip, #2
   401ee:	442b      	add	r3, r5
   401f0:	eba3 0308 	sub.w	r3, r3, r8
   401f4:	ea42 4101 	orr.w	r1, r2, r1, lsl #16
   401f8:	e792      	b.n	40120 <__udivmoddi4+0xb8>
   401fa:	4661      	mov	r1, ip
   401fc:	e7e2      	b.n	401c4 <__udivmoddi4+0x15c>
   401fe:	4662      	mov	r2, ip
   40200:	e7f6      	b.n	401f0 <__udivmoddi4+0x188>
   40202:	4660      	mov	r0, ip
   40204:	e7a3      	b.n	4014e <__udivmoddi4+0xe6>
   40206:	4663      	mov	r3, ip
   40208:	e7b7      	b.n	4017a <__udivmoddi4+0x112>
   4020a:	4543      	cmp	r3, r8
   4020c:	d905      	bls.n	4021a <__udivmoddi4+0x1b2>
   4020e:	b10e      	cbz	r6, 40214 <__udivmoddi4+0x1ac>
   40210:	e9c6 0800 	strd	r0, r8, [r6]
   40214:	2100      	movs	r1, #0
   40216:	4608      	mov	r0, r1
   40218:	e776      	b.n	40108 <__udivmoddi4+0xa0>
   4021a:	fab3 fc83 	clz	ip, r3
   4021e:	f1bc 0f00 	cmp.w	ip, #0
   40222:	d110      	bne.n	40246 <__udivmoddi4+0x1de>
   40224:	4543      	cmp	r3, r8
   40226:	d301      	bcc.n	4022c <__udivmoddi4+0x1c4>
   40228:	4282      	cmp	r2, r0
   4022a:	d80a      	bhi.n	40242 <__udivmoddi4+0x1da>
   4022c:	1a84      	subs	r4, r0, r2
   4022e:	eb68 0303 	sbc.w	r3, r8, r3
   40232:	2001      	movs	r0, #1
   40234:	469e      	mov	lr, r3
   40236:	2e00      	cmp	r6, #0
   40238:	d068      	beq.n	4030c <__udivmoddi4+0x2a4>
   4023a:	e9c6 4e00 	strd	r4, lr, [r6]
   4023e:	2100      	movs	r1, #0
   40240:	e762      	b.n	40108 <__udivmoddi4+0xa0>
   40242:	4660      	mov	r0, ip
   40244:	e7f7      	b.n	40236 <__udivmoddi4+0x1ce>
   40246:	f1cc 0e20 	rsb	lr, ip, #32
   4024a:	fa03 f30c 	lsl.w	r3, r3, ip
   4024e:	fa02 f50c 	lsl.w	r5, r2, ip
   40252:	fa00 f70c 	lsl.w	r7, r0, ip
   40256:	fa22 f40e 	lsr.w	r4, r2, lr
   4025a:	fa28 fb0e 	lsr.w	fp, r8, lr
   4025e:	fa20 f10e 	lsr.w	r1, r0, lr
   40262:	431c      	orrs	r4, r3
   40264:	fa08 f30c 	lsl.w	r3, r8, ip
   40268:	ea4f 4914 	mov.w	r9, r4, lsr #16
   4026c:	4319      	orrs	r1, r3
   4026e:	b2a3      	uxth	r3, r4
   40270:	0c0a      	lsrs	r2, r1, #16
   40272:	fbbb f8f9 	udiv	r8, fp, r9
   40276:	fb09 bb18 	mls	fp, r9, r8, fp
   4027a:	fb08 fa03 	mul.w	sl, r8, r3
   4027e:	ea42 420b 	orr.w	r2, r2, fp, lsl #16
   40282:	4592      	cmp	sl, r2
   40284:	d93e      	bls.n	40304 <__udivmoddi4+0x29c>
   40286:	18a2      	adds	r2, r4, r2
   40288:	f108 30ff 	add.w	r0, r8, #4294967295	; 0xffffffff
   4028c:	d204      	bcs.n	40298 <__udivmoddi4+0x230>
   4028e:	4592      	cmp	sl, r2
   40290:	d902      	bls.n	40298 <__udivmoddi4+0x230>
   40292:	f1a8 0002 	sub.w	r0, r8, #2
   40296:	4422      	add	r2, r4
   40298:	eba2 020a 	sub.w	r2, r2, sl
   4029c:	b289      	uxth	r1, r1
   4029e:	fbb2 f8f9 	udiv	r8, r2, r9
   402a2:	fb09 2218 	mls	r2, r9, r8, r2
   402a6:	fb08 f303 	mul.w	r3, r8, r3
   402aa:	ea41 4202 	orr.w	r2, r1, r2, lsl #16
   402ae:	4293      	cmp	r3, r2
   402b0:	d92a      	bls.n	40308 <__udivmoddi4+0x2a0>
   402b2:	18a2      	adds	r2, r4, r2
   402b4:	f108 31ff 	add.w	r1, r8, #4294967295	; 0xffffffff
   402b8:	d204      	bcs.n	402c4 <__udivmoddi4+0x25c>
   402ba:	4293      	cmp	r3, r2
   402bc:	d902      	bls.n	402c4 <__udivmoddi4+0x25c>
   402be:	f1a8 0102 	sub.w	r1, r8, #2
   402c2:	4422      	add	r2, r4
   402c4:	ea41 4000 	orr.w	r0, r1, r0, lsl #16
   402c8:	1ad2      	subs	r2, r2, r3
   402ca:	fba0 9805 	umull	r9, r8, r0, r5
   402ce:	4542      	cmp	r2, r8
   402d0:	464b      	mov	r3, r9
   402d2:	4641      	mov	r1, r8
   402d4:	d302      	bcc.n	402dc <__udivmoddi4+0x274>
   402d6:	d106      	bne.n	402e6 <__udivmoddi4+0x27e>
   402d8:	454f      	cmp	r7, r9
   402da:	d204      	bcs.n	402e6 <__udivmoddi4+0x27e>
   402dc:	3801      	subs	r0, #1
   402de:	ebb9 0305 	subs.w	r3, r9, r5
   402e2:	eb68 0104 	sbc.w	r1, r8, r4
   402e6:	b18e      	cbz	r6, 4030c <__udivmoddi4+0x2a4>
   402e8:	1afc      	subs	r4, r7, r3
   402ea:	eb62 0701 	sbc.w	r7, r2, r1
   402ee:	fa07 fe0e 	lsl.w	lr, r7, lr
   402f2:	fa24 f30c 	lsr.w	r3, r4, ip
   402f6:	fa27 f70c 	lsr.w	r7, r7, ip
   402fa:	ea4e 0303 	orr.w	r3, lr, r3
   402fe:	e9c6 3700 	strd	r3, r7, [r6]
   40302:	e79c      	b.n	4023e <__udivmoddi4+0x1d6>
   40304:	4640      	mov	r0, r8
   40306:	e7c7      	b.n	40298 <__udivmoddi4+0x230>
   40308:	4641      	mov	r1, r8
   4030a:	e7db      	b.n	402c4 <__udivmoddi4+0x25c>
   4030c:	4631      	mov	r1, r6
   4030e:	e6fb      	b.n	40108 <__udivmoddi4+0xa0>

00040310 <__aeabi_idiv0>:
   40310:	4770      	bx	lr
   40312:	bf00      	nop

00040314 <_close>:
   40314:	4b02      	ldr	r3, [pc, #8]	; (40320 <_close+0xc>)
   40316:	2258      	movs	r2, #88	; 0x58
   40318:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   4031c:	601a      	str	r2, [r3, #0]
   4031e:	4770      	bx	lr
   40320:	20006a0c 	.word	0x20006a0c

00040324 <_fstat>:
   40324:	4b02      	ldr	r3, [pc, #8]	; (40330 <_fstat+0xc>)
   40326:	2258      	movs	r2, #88	; 0x58
   40328:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   4032c:	601a      	str	r2, [r3, #0]
   4032e:	4770      	bx	lr
   40330:	20006a0c 	.word	0x20006a0c

00040334 <_getpid>:
   40334:	4b02      	ldr	r3, [pc, #8]	; (40340 <_getpid+0xc>)
   40336:	2258      	movs	r2, #88	; 0x58
   40338:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   4033c:	601a      	str	r2, [r3, #0]
   4033e:	4770      	bx	lr
   40340:	20006a0c 	.word	0x20006a0c

00040344 <_isatty>:
   40344:	4b02      	ldr	r3, [pc, #8]	; (40350 <_isatty+0xc>)
   40346:	2258      	movs	r2, #88	; 0x58
   40348:	2000      	movs	r0, #0
   4034a:	601a      	str	r2, [r3, #0]
   4034c:	4770      	bx	lr
   4034e:	bf00      	nop
   40350:	20006a0c 	.word	0x20006a0c

00040354 <_kill>:
   40354:	4b02      	ldr	r3, [pc, #8]	; (40360 <_kill+0xc>)
   40356:	2258      	movs	r2, #88	; 0x58
   40358:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   4035c:	601a      	str	r2, [r3, #0]
   4035e:	4770      	bx	lr
   40360:	20006a0c 	.word	0x20006a0c

00040364 <_lseek>:
   40364:	4b02      	ldr	r3, [pc, #8]	; (40370 <_lseek+0xc>)
   40366:	2258      	movs	r2, #88	; 0x58
   40368:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
   4036c:	601a      	str	r2, [r3, #0]
   4036e:	4770      	bx	lr
   40370:	20006a0c 	.word	0x20006a0c

00040374 <_sbrk>:
   40374:	4a04      	ldr	r2, [pc, #16]	; (40388 <_sbrk+0x14>)
   40376:	4603      	mov	r3, r0
   40378:	6811      	ldr	r1, [r2, #0]
   4037a:	b909      	cbnz	r1, 40380 <_sbrk+0xc>
   4037c:	4903      	ldr	r1, [pc, #12]	; (4038c <_sbrk+0x18>)
   4037e:	6011      	str	r1, [r2, #0]
   40380:	6810      	ldr	r0, [r2, #0]
   40382:	4403      	add	r3, r0
   40384:	6013      	str	r3, [r2, #0]
   40386:	4770      	bx	lr
   40388:	20006a10 	.word	0x20006a10
   4038c:	20006a20 	.word	0x20006a20

00040390 <_exit>:
   40390:	e7fe      	b.n	40390 <_exit>
   40392:	0000      	movs	r0, r0
   40394:	253a6562 	.word	0x253a6562
   40398:	00000a73 	.word	0x00000a73
   4039c:	253a6661 	.word	0x253a6661
   403a0:	00000a73 	.word	0x00000a73
   403a4:	00003130 	.word	0x00003130
   403a8:	3a296661 	.word	0x3a296661
   403ac:	000a7325 	.word	0x000a7325
   403b0:	32287473 	.word	0x32287473
   403b4:	00000029 	.word	0x00000029
   403b8:	69727473 	.word	0x69727473
   403bc:	3228676e 	.word	0x3228676e
   403c0:	00000029 	.word	0x00000029
   403c4:	00004261 	.word	0x00004261
   403c8:	00006241 	.word	0x00006241
   403cc:	6e727473 	.word	0x6e727473
   403d0:	28706d63 	.word	0x28706d63
   403d4:	62202c61 	.word	0x62202c61
   403d8:	25203a29 	.word	0x25203a29
   403dc:	00000a64 	.word	0x00000a64
   403e0:	00003273 	.word	0x00003273
   403e4:	003a2d20 	.word	0x003a2d20
   403e8:	00207325 	.word	0x00207325
   403ec:	792d6261 	.word	0x792d6261
   403f0:	0000003b 	.word	0x0000003b
   403f4:	00313039 	.word	0x00313039
   403f8:	3b2d6261 	.word	0x3b2d6261
   403fc:	0000007a 	.word	0x0000007a
   40400:	25203a66 	.word	0x25203a66
   40404:	00000a63 	.word	0x00000a63
   40408:	00000065 	.word	0x00000065
   4040c:	00006261 	.word	0x00006261
   40410:	00003433 	.word	0x00003433
   40414:	3a6d756e 	.word	0x3a6d756e
   40418:	0a642520 	.word	0x0a642520
   4041c:	00000000 	.word	0x00000000
   40420:	3a6d756e 	.word	0x3a6d756e
   40424:	646c2520 	.word	0x646c2520
   40428:	0000000a 	.word	0x0000000a
   4042c:	3a727473 	.word	0x3a727473
   40430:	73257c20 	.word	0x73257c20
   40434:	0000007c 	.word	0x0000007c
   40438:	65743032 	.word	0x65743032
   4043c:	00007473 	.word	0x00007473
	...
   40458:	5f6c7472 	.word	0x5f6c7472
   4045c:	69727473 	.word	0x69727473
   40460:	745f676e 	.word	0x745f676e
   40464:	00747365 	.word	0x00747365
   40468:	000a6425 	.word	0x000a6425
   4046c:	58323025 	.word	0x58323025
   40470:	58323025 	.word	0x58323025
   40474:	58323025 	.word	0x58323025
   40478:	58323025 	.word	0x58323025
   4047c:	58323025 	.word	0x58323025
   40480:	58323025 	.word	0x58323025
   40484:	00000000 	.word	0x00000000
   40488:	75333025 	.word	0x75333025
   4048c:	00000000 	.word	0x00000000
   40490:	256c7472 	.word	0x256c7472
   40494:	00783230 	.word	0x00783230
   40498:	3f3f6325 	.word	0x3f3f6325
   4049c:	3632253f 	.word	0x3632253f
   404a0:	37322563 	.word	0x37322563
   404a4:	00000073 	.word	0x00000073
   404a8:	3f3f3f20 	.word	0x3f3f3f20
   404ac:	3332313f 	.word	0x3332313f
   404b0:	37363534 	.word	0x37363534
   404b4:	41303938 	.word	0x41303938
   404b8:	45444342 	.word	0x45444342
   404bc:	69686746 	.word	0x69686746
   404c0:	6d6c6b6a 	.word	0x6d6c6b6a
   404c4:	73726f6e 	.word	0x73726f6e
   404c8:	00000074 	.word	0x00000074
   404cc:	6425      	.short	0x6425
   404ce:	00          	.byte	0x00
   404cf:	49          	.byte	0x49
   404d0:	6900464e 	.word	0x6900464e
   404d4:	4e00666e 	.word	0x4e00666e
   404d8:	6e004e41 	.word	0x6e004e41
   404dc:	30006e61 	.word	0x30006e61
   404e0:	34333231 	.word	0x34333231
   404e4:	38373635 	.word	0x38373635
   404e8:	63626139 	.word	0x63626139
   404ec:	00666564 	.word	0x00666564
   404f0:	33323130 	.word	0x33323130
   404f4:	37363534 	.word	0x37363534
   404f8:	42413938 	.word	0x42413938
   404fc:	46454443 	.word	0x46454443
   40500:	3000      	.short	0x3000
	...

00040503 <blanks.1>:
   40503:	2020 2020 2020 2020 2020 2020 2020 2020                     

00040513 <zeroes.0>:
   40513:	3030 3030 3030 3030 3030 3030 3030 3030     0000000000000000
   40523:	2565 646c                                    e%ld.

00040528 <basefix.0>:
   40528:	000a 0001 0002 0003 0004 0005 0006 0007     ................
   40538:	0008 0009 000a 000b 000c 000d 000e 000f     ................
   40548:	0010                                        ..

0004054a <blanks.1>:
   4054a:	2020 2020 2020 2020 2020 2020 2020 2020                     

0004055a <zeroes.0>:
   4055a:	3030 3030 3030 3030 3030 3030 3030 3030     0000000000000000

0004056a <_ctype_>:
   4056a:	2000 2020 2020 2020 2020 2828 2828 2028     .         ((((( 
   4057a:	2020 2020 2020 2020 2020 2020 2020 2020                     
   4058a:	8820 1010 1010 1010 1010 1010 1010 1010      ...............
   4059a:	0410 0404 0404 0404 0404 1004 1010 1010     ................
   405aa:	1010 4141 4141 4141 0101 0101 0101 0101     ..AAAAAA........
   405ba:	0101 0101 0101 0101 0101 0101 1010 1010     ................
   405ca:	1010 4242 4242 4242 0202 0202 0202 0202     ..BBBBBB........
   405da:	0202 0202 0202 0202 0202 0202 1010 1010     ................
   405ea:	0020 0000 0000 0000 0000 0000 0000 0000      ...............
	...
   4066a:	4e00 4e61 4900 666e 6e69 7469 0079 6142     .NaN.Infinity.Ba
   4067a:	6c6c 636f 7320 6375 6563 6465 6465 2f00     lloc succeeded./
   4068a:	6f68 656d 6a2f 6e65 696b 736e 732f 6c64     home/jenkins/sdl
   4069a:	2f63 6f77 6b72 7073 6361 2f65 7372 6b64     c/workspace/rsdk
   406aa:	722d 6c65 6165 6573 662d 6f6c 2f77 7562     -release-flow/bu
   406ba:	6c69 2f64 7562 6c69 2d64 7361 6b64 312d     ild/build-asdk-1
   406ca:	2d30 656e 6c77 6269 332d 3836 2f32 656e     0-newlib-3682/ne
   406da:	6c77 6269 342d 312e 6e2f 7765 696c 2f62     wlib-4.1/newlib/
   406ea:	696c 6362 732f 6474 696c 2f62 7464 616f     libc/stdlib/dtoa
   406fa:	632e 0000 0000                              .c....

00040700 <_global_impure_ptr>:
   40700:	6008 2000 0043 4f50 4953 0058 002e 682f     .`. C.POSIX.../h
   40710:	6d6f 2f65 656a 6b6e 6e69 2f73 6473 636c     ome/jenkins/sdlc
   40720:	772f 726f 736b 6170 6563 722f 6473 2d6b     /workspace/rsdk-
   40730:	6572 656c 7361 2d65 6c66 776f 622f 6975     release-flow/bui
   40740:	646c 622f 6975 646c 612d 6473 2d6b 3031     ld/build-asdk-10
   40750:	6e2d 7765 696c 2d62 3633 3238 6e2f 7765     -newlib-3682/new
   40760:	696c 2d62 2e34 2f31 656e 6c77 6269 6c2f     lib-4.1/newlib/l
   40770:	6269 2f63 7473 6c64 6269 6d2f 7270 6365     ibc/stdlib/mprec
   40780:	632e 0000 0000 0000                         .c......

00040788 <p05.0>:
   40788:	0005 0000 0019 0000 007d 0000 0000 0000     ........}.......

00040798 <__mprec_tens>:
   40798:	0000 0000 0000 3ff0 0000 0000 0000 4024     .......?......$@
   407a8:	0000 0000 0000 4059 0000 0000 4000 408f     ......Y@.....@.@
   407b8:	0000 0000 8800 40c3 0000 0000 6a00 40f8     .......@.....j.@
   407c8:	0000 0000 8480 412e 0000 0000 12d0 4163     .......A......cA
   407d8:	0000 0000 d784 4197 0000 0000 cd65 41cd     .......A....e..A
   407e8:	0000 2000 a05f 4202 0000 e800 4876 4237     ... _..B....vH7B
   407f8:	0000 a200 1a94 426d 0000 e540 309c 42a2     ......mB..@..0.B
   40808:	0000 1e90 bcc4 42d6 0000 2634 6bf5 430c     .......B..4&.k.C
   40818:	8000 37e0 c379 4341 a000 85d8 3457 4376     ...7y.AC....W4vC
   40828:	c800 674e c16d 43ab 3d00 6091 58e4 43e1     ..Ngm..C.=.`.X.C
   40838:	8c40 78b5 af1d 4415 ef50 d6e2 1ae4 444b     @..x...DP.....KD
   40848:	d592 064d f0cf 4480 4af6 c7e1 2d02 44b5     ..M....D.J...-.D
   40858:	9db4 79d9 7843 44ea                         ...yCx.D

00040860 <__mprec_tinytens>:
   40860:	89bc 97d8 d2b2 3c9c a733 d5a8 f623 3949     .......<3...#.I9
   40870:	a73d 44f4 0ffd 32a5 979d cf8c ba08 255b     =..D...2......[%
   40880:	6f43 64ac 0628 0ac8                         Co.d(...

00040888 <__mprec_bigtens>:
   40888:	8000 37e0 c379 4341 6e17 b505 b8b5 4693     ...7y.AC.n.....F
   40898:	f9f5 e93f 4f03 4d38 1d32 f930 7748 5a82     ..?..O8M2.0.Hw.Z
   408a8:	bf3c 7f73 4fdd 7515                         <.s..O.u

000408b0 <fpi.1>:
   408b0:	0035 0000 fbce ffff 03cb 0000 0001 0000     5...............
   408c0:	0000 0000                                   ....

000408c4 <fpinan.0>:
   408c4:	0034 0000 fbce ffff 03cb 0000 0001 0000     4...............
   408d4:	0000 0000                                   ....

000408d8 <tinytens>:
   408d8:	89bc 97d8 d2b2 3c9c a733 d5a8 f623 3949     .......<3...#.I9
   408e8:	a73d 44f4 0ffd 32a5 979d cf8c ba08 255b     =..D...2......[%
   408f8:	6f43 64ac 0628 1168                         Co.d(.h.

00040900 <blanks.1>:
   40900:	2020 2020 2020 2020 2020 2020 2020 2020                     

00040910 <zeroes.0>:
   40910:	3030 3030 3030 3030 3030 3030 3030 3030     0000000000000000

00040920 <basefix.0>:
   40920:	000a 0001 0002 0003 0004 0005 0006 0007     ................
   40930:	0008 0009 000a 000b 000c 000d 000e 000f     ................
   40940:	0010                                        ..

00040942 <blanks.1>:
   40942:	2020 2020 2020 2020 2020 2020 2020 2020                     

00040952 <zeroes.0>:
   40952:	3030 3030 3030 3030 3030 3030 3030 3030     0000000000000000
   40962:	202c 7566 636e 6974 6e6f 203a 6100 7373     , function: .ass
   40972:	7265 6974 6e6f 2220 7325 2022 6166 6c69     ertion "%s" fail
   40982:	6465 203a 6966 656c 2220 7325 2c22 6c20     ed: file "%s", l
   40992:	6e69 2065 6425 7325 7325 000a 682f 6d6f     ine %d%s%s../hom
   409a2:	2f65 656a 6b6e 6e69 2f73 6473 636c 772f     e/jenkins/sdlc/w
   409b2:	726f 736b 6170 6563 722f 6473 2d6b 6572     orkspace/rsdk-re
   409c2:	656c 7361 2d65 6c66 776f 622f 6975 646c     lease-flow/build
   409d2:	622f 6975 646c 612d 6473 2d6b 3031 6e2d     /build-asdk-10-n
   409e2:	7765 696c 2d62 3633 3238 6e2f 7765 696c     ewlib-3682/newli
   409f2:	2d62 2e34 2f31 656e 6c77 6269 6c2f 6269     b-4.1/newlib/lib
   40a02:	2f63 7473 6c64 6269 672f 7464 616f 672d     c/stdlib/gdtoa-g
   40a12:	7465 6568 2e78 0063 0000                    ethex.c...

00040a1c <_C_numeric_locale>:
   40a1c:	070c 0004 099d 0004 69ac 2000 0000 0000     .........i. ....
	...
