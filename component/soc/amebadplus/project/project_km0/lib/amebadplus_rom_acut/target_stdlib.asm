
/home/zjian_zhang/Dplus_Z/project/realtek_amebaDplus_va0_example/GCC-RELEASE/project_km0/asdk/image/target_stdlib.axf:     file format elf32-littlearm


Disassembly of section .rom.stdlib.text:

00006000 <stdlib_entry_table>:
    6000:	d9 67 00 00 2d 66 00 00                             .g..-f..

00006008 <test_strcat>:
    6008:	b580      	push	{r7, lr}
    600a:	b086      	sub	sp, #24
    600c:	af00      	add	r7, sp, #0
    600e:	1d3b      	adds	r3, r7, #4
    6010:	f247 4273 	movw	r2, #29811	; 0x7473
    6014:	601a      	str	r2, [r3, #0]
    6016:	3304      	adds	r3, #4
    6018:	2210      	movs	r2, #16
    601a:	2100      	movs	r1, #0
    601c:	0018      	movs	r0, r3
    601e:	f000 fc0d 	bl	683c <memset>
    6022:	003a      	movs	r2, r7
    6024:	f247 4373 	movw	r3, #29811	; 0x7473
    6028:	f2c0 0329 	movt	r3, #41	; 0x29
    602c:	6013      	str	r3, [r2, #0]
    602e:	1d3a      	adds	r2, r7, #4
    6030:	f249 4318 	movw	r3, #37912	; 0x9418
    6034:	f2c0 0300 	movt	r3, #0
    6038:	0011      	movs	r1, r2
    603a:	0018      	movs	r0, r3
    603c:	f000 fc19 	bl	6872 <iprintf>
    6040:	003a      	movs	r2, r7
    6042:	1d3b      	adds	r3, r7, #4
    6044:	0011      	movs	r1, r2
    6046:	0018      	movs	r0, r3
    6048:	f000 fe52 	bl	6cf0 <strcat>
    604c:	0002      	movs	r2, r0
    604e:	f249 4320 	movw	r3, #37920	; 0x9420
    6052:	f2c0 0300 	movt	r3, #0
    6056:	0011      	movs	r1, r2
    6058:	0018      	movs	r0, r3
    605a:	f000 fc0a 	bl	6872 <iprintf>
    605e:	bf00      	nop
    6060:	46bd      	mov	sp, r7
    6062:	b006      	add	sp, #24
    6064:	bd80      	pop	{r7, pc}
    6066:	bf00      	nop

00006068 <test_strchr>:
    6068:	b580      	push	{r7, lr}
    606a:	b082      	sub	sp, #8
    606c:	af00      	add	r7, sp, #0
    606e:	f249 4328 	movw	r3, #37928	; 0x9428
    6072:	f2c0 0300 	movt	r3, #0
    6076:	607b      	str	r3, [r7, #4]
    6078:	687b      	ldr	r3, [r7, #4]
    607a:	2135      	movs	r1, #53	; 0x35
    607c:	0018      	movs	r0, r3
    607e:	f000 fe44 	bl	6d0a <strchr>
    6082:	0003      	movs	r3, r0
    6084:	603b      	str	r3, [r7, #0]
    6086:	683b      	ldr	r3, [r7, #0]
    6088:	0018      	movs	r0, r3
    608a:	f000 fce9 	bl	6a60 <puts>
    608e:	bf00      	nop
    6090:	46bd      	mov	sp, r7
    6092:	b002      	add	sp, #8
    6094:	bd80      	pop	{r7, pc}
    6096:	bf00      	nop

00006098 <test_strcpy>:
    6098:	b590      	push	{r4, r7, lr}
    609a:	b089      	sub	sp, #36	; 0x24
    609c:	af00      	add	r7, sp, #0
    609e:	240c      	movs	r4, #12
    60a0:	193b      	adds	r3, r7, r4
    60a2:	f247 4273 	movw	r2, #29811	; 0x7473
    60a6:	601a      	str	r2, [r3, #0]
    60a8:	3304      	adds	r3, #4
    60aa:	2210      	movs	r2, #16
    60ac:	2100      	movs	r1, #0
    60ae:	0018      	movs	r0, r3
    60b0:	f000 fbc4 	bl	683c <memset>
    60b4:	1d3a      	adds	r2, r7, #4
    60b6:	f249 4334 	movw	r3, #37940	; 0x9434
    60ba:	f2c0 0300 	movt	r3, #0
    60be:	6819      	ldr	r1, [r3, #0]
    60c0:	6011      	str	r1, [r2, #0]
    60c2:	889b      	ldrh	r3, [r3, #4]
    60c4:	8093      	strh	r3, [r2, #4]
    60c6:	193a      	adds	r2, r7, r4
    60c8:	f249 4318 	movw	r3, #37912	; 0x9418
    60cc:	f2c0 0300 	movt	r3, #0
    60d0:	0011      	movs	r1, r2
    60d2:	0018      	movs	r0, r3
    60d4:	f000 fbcd 	bl	6872 <iprintf>
    60d8:	1d3a      	adds	r2, r7, #4
    60da:	193b      	adds	r3, r7, r4
    60dc:	0011      	movs	r1, r2
    60de:	0018      	movs	r0, r3
    60e0:	f000 fe20 	bl	6d24 <strcpy>
    60e4:	0002      	movs	r2, r0
    60e6:	f249 432c 	movw	r3, #37932	; 0x942c
    60ea:	f2c0 0300 	movt	r3, #0
    60ee:	0011      	movs	r1, r2
    60f0:	0018      	movs	r0, r3
    60f2:	f000 fbbe 	bl	6872 <iprintf>
    60f6:	bf00      	nop
    60f8:	46bd      	mov	sp, r7
    60fa:	b009      	add	sp, #36	; 0x24
    60fc:	bd90      	pop	{r4, r7, pc}
    60fe:	bf00      	nop

00006100 <test_strncat>:
    6100:	b590      	push	{r4, r7, lr}
    6102:	b08d      	sub	sp, #52	; 0x34
    6104:	af00      	add	r7, sp, #0
    6106:	2410      	movs	r4, #16
    6108:	193b      	adds	r3, r7, r4
    610a:	2273      	movs	r2, #115	; 0x73
    610c:	601a      	str	r2, [r3, #0]
    610e:	3304      	adds	r3, #4
    6110:	221a      	movs	r2, #26
    6112:	2100      	movs	r1, #0
    6114:	0018      	movs	r0, r3
    6116:	f000 fb91 	bl	683c <memset>
    611a:	1d3a      	adds	r2, r7, #4
    611c:	f249 433c 	movw	r3, #37948	; 0x943c
    6120:	f2c0 0300 	movt	r3, #0
    6124:	cb03      	ldmia	r3!, {r0, r1}
    6126:	c203      	stmia	r2!, {r0, r1}
    6128:	881b      	ldrh	r3, [r3, #0]
    612a:	8013      	strh	r3, [r2, #0]
    612c:	193a      	adds	r2, r7, r4
    612e:	f249 4318 	movw	r3, #37912	; 0x9418
    6132:	f2c0 0300 	movt	r3, #0
    6136:	0011      	movs	r1, r2
    6138:	0018      	movs	r0, r3
    613a:	f000 fb9a 	bl	6872 <iprintf>
    613e:	1d39      	adds	r1, r7, #4
    6140:	193b      	adds	r3, r7, r4
    6142:	220a      	movs	r2, #10
    6144:	0018      	movs	r0, r3
    6146:	f000 fdfc 	bl	6d42 <strncat>
    614a:	0002      	movs	r2, r0
    614c:	f249 4320 	movw	r3, #37920	; 0x9420
    6150:	f2c0 0300 	movt	r3, #0
    6154:	0011      	movs	r1, r2
    6156:	0018      	movs	r0, r3
    6158:	f000 fb8b 	bl	6872 <iprintf>
    615c:	bf00      	nop
    615e:	46bd      	mov	sp, r7
    6160:	b00d      	add	sp, #52	; 0x34
    6162:	bd90      	pop	{r4, r7, pc}

00006164 <test_strncmp>:
    6164:	b580      	push	{r7, lr}
    6166:	b082      	sub	sp, #8
    6168:	af00      	add	r7, sp, #0
    616a:	f249 4348 	movw	r3, #37960	; 0x9448
    616e:	f2c0 0300 	movt	r3, #0
    6172:	607b      	str	r3, [r7, #4]
    6174:	f249 434c 	movw	r3, #37964	; 0x944c
    6178:	f2c0 0300 	movt	r3, #0
    617c:	603b      	str	r3, [r7, #0]
    617e:	6839      	ldr	r1, [r7, #0]
    6180:	687b      	ldr	r3, [r7, #4]
    6182:	2206      	movs	r2, #6
    6184:	0018      	movs	r0, r3
    6186:	f000 fdee 	bl	6d66 <strncmp>
    618a:	0002      	movs	r2, r0
    618c:	f249 4350 	movw	r3, #37968	; 0x9450
    6190:	f2c0 0300 	movt	r3, #0
    6194:	0011      	movs	r1, r2
    6196:	0018      	movs	r0, r3
    6198:	f000 fb6b 	bl	6872 <iprintf>
    619c:	bf00      	nop
    619e:	46bd      	mov	sp, r7
    61a0:	b002      	add	sp, #8
    61a2:	bd80      	pop	{r7, pc}

000061a4 <test_strncpy>:
    61a4:	b580      	push	{r7, lr}
    61a6:	b086      	sub	sp, #24
    61a8:	af00      	add	r7, sp, #0
    61aa:	1d3b      	adds	r3, r7, #4
    61ac:	f243 1273 	movw	r2, #12659	; 0x3173
    61b0:	601a      	str	r2, [r3, #0]
    61b2:	3304      	adds	r3, #4
    61b4:	2210      	movs	r2, #16
    61b6:	2100      	movs	r1, #0
    61b8:	0018      	movs	r0, r3
    61ba:	f000 fb3f 	bl	683c <memset>
    61be:	003a      	movs	r2, r7
    61c0:	f249 4364 	movw	r3, #37988	; 0x9464
    61c4:	f2c0 0300 	movt	r3, #0
    61c8:	8819      	ldrh	r1, [r3, #0]
    61ca:	8011      	strh	r1, [r2, #0]
    61cc:	789b      	ldrb	r3, [r3, #2]
    61ce:	7093      	strb	r3, [r2, #2]
    61d0:	1d3a      	adds	r2, r7, #4
    61d2:	f249 4318 	movw	r3, #37912	; 0x9418
    61d6:	f2c0 0300 	movt	r3, #0
    61da:	0011      	movs	r1, r2
    61dc:	0018      	movs	r0, r3
    61de:	f000 fb48 	bl	6872 <iprintf>
    61e2:	0039      	movs	r1, r7
    61e4:	1d3b      	adds	r3, r7, #4
    61e6:	2209      	movs	r2, #9
    61e8:	0018      	movs	r0, r3
    61ea:	f000 fdce 	bl	6d8a <strncpy>
    61ee:	0002      	movs	r2, r0
    61f0:	f249 4320 	movw	r3, #37920	; 0x9420
    61f4:	f2c0 0300 	movt	r3, #0
    61f8:	0011      	movs	r1, r2
    61fa:	0018      	movs	r0, r3
    61fc:	f000 fb39 	bl	6872 <iprintf>
    6200:	bf00      	nop
    6202:	46bd      	mov	sp, r7
    6204:	b006      	add	sp, #24
    6206:	bd80      	pop	{r7, pc}

00006208 <test_strsep>:
    6208:	b580      	push	{r7, lr}
    620a:	b086      	sub	sp, #24
    620c:	af00      	add	r7, sp, #0
    620e:	2008      	movs	r0, #8
    6210:	183a      	adds	r2, r7, r0
    6212:	f249 4370 	movw	r3, #38000	; 0x9470
    6216:	f2c0 0300 	movt	r3, #0
    621a:	6819      	ldr	r1, [r3, #0]
    621c:	6011      	str	r1, [r2, #0]
    621e:	889b      	ldrh	r3, [r3, #4]
    6220:	8093      	strh	r3, [r2, #4]
    6222:	f249 4368 	movw	r3, #37992	; 0x9468
    6226:	f2c0 0300 	movt	r3, #0
    622a:	617b      	str	r3, [r7, #20]
    622c:	183b      	adds	r3, r7, r0
    622e:	607b      	str	r3, [r7, #4]
    6230:	e008      	b.n	6244 <test_strsep+0x3c>
    6232:	693a      	ldr	r2, [r7, #16]
    6234:	f249 436c 	movw	r3, #37996	; 0x946c
    6238:	f2c0 0300 	movt	r3, #0
    623c:	0011      	movs	r1, r2
    623e:	0018      	movs	r0, r3
    6240:	f000 fb17 	bl	6872 <iprintf>
    6244:	697a      	ldr	r2, [r7, #20]
    6246:	1d3b      	adds	r3, r7, #4
    6248:	0011      	movs	r1, r2
    624a:	0018      	movs	r0, r3
    624c:	f000 fdc8 	bl	6de0 <strsep>
    6250:	0003      	movs	r3, r0
    6252:	613b      	str	r3, [r7, #16]
    6254:	693b      	ldr	r3, [r7, #16]
    6256:	2b00      	cmp	r3, #0
    6258:	d1eb      	bne.n	6232 <test_strsep+0x2a>
    625a:	bf00      	nop
    625c:	bf00      	nop
    625e:	46bd      	mov	sp, r7
    6260:	b006      	add	sp, #24
    6262:	bd80      	pop	{r7, pc}

00006264 <test_strstr>:
    6264:	b580      	push	{r7, lr}
    6266:	b082      	sub	sp, #8
    6268:	af00      	add	r7, sp, #0
    626a:	f249 4328 	movw	r3, #37928	; 0x9428
    626e:	f2c0 0300 	movt	r3, #0
    6272:	607b      	str	r3, [r7, #4]
    6274:	f249 4378 	movw	r3, #38008	; 0x9478
    6278:	f2c0 0300 	movt	r3, #0
    627c:	687a      	ldr	r2, [r7, #4]
    627e:	0019      	movs	r1, r3
    6280:	0010      	movs	r0, r2
    6282:	f000 fdb4 	bl	6dee <strstr>
    6286:	0003      	movs	r3, r0
    6288:	603b      	str	r3, [r7, #0]
    628a:	683b      	ldr	r3, [r7, #0]
    628c:	0018      	movs	r0, r3
    628e:	f000 fbe7 	bl	6a60 <puts>
    6292:	bf00      	nop
    6294:	46bd      	mov	sp, r7
    6296:	b002      	add	sp, #8
    6298:	bd80      	pop	{r7, pc}
    629a:	bf00      	nop

0000629c <test_strtok>:
    629c:	b580      	push	{r7, lr}
    629e:	b084      	sub	sp, #16
    62a0:	af00      	add	r7, sp, #0
    62a2:	003a      	movs	r2, r7
    62a4:	f249 437c 	movw	r3, #38012	; 0x947c
    62a8:	f2c0 0300 	movt	r3, #0
    62ac:	6819      	ldr	r1, [r3, #0]
    62ae:	6011      	str	r1, [r2, #0]
    62b0:	889b      	ldrh	r3, [r3, #4]
    62b2:	8093      	strh	r3, [r2, #4]
    62b4:	f249 4368 	movw	r3, #37992	; 0x9468
    62b8:	f2c0 0300 	movt	r3, #0
    62bc:	60fb      	str	r3, [r7, #12]
    62be:	68fa      	ldr	r2, [r7, #12]
    62c0:	003b      	movs	r3, r7
    62c2:	0011      	movs	r1, r2
    62c4:	0018      	movs	r0, r3
    62c6:	f000 fda7 	bl	6e18 <strtok>
    62ca:	0002      	movs	r2, r0
    62cc:	f249 436c 	movw	r3, #37996	; 0x946c
    62d0:	f2c0 0300 	movt	r3, #0
    62d4:	0011      	movs	r1, r2
    62d6:	0018      	movs	r0, r3
    62d8:	f000 facb 	bl	6872 <iprintf>
    62dc:	e008      	b.n	62f0 <test_strtok+0x54>
    62de:	68ba      	ldr	r2, [r7, #8]
    62e0:	f249 436c 	movw	r3, #37996	; 0x946c
    62e4:	f2c0 0300 	movt	r3, #0
    62e8:	0011      	movs	r1, r2
    62ea:	0018      	movs	r0, r3
    62ec:	f000 fac1 	bl	6872 <iprintf>
    62f0:	68fb      	ldr	r3, [r7, #12]
    62f2:	0019      	movs	r1, r3
    62f4:	2000      	movs	r0, #0
    62f6:	f000 fd8f 	bl	6e18 <strtok>
    62fa:	0003      	movs	r3, r0
    62fc:	60bb      	str	r3, [r7, #8]
    62fe:	68bb      	ldr	r3, [r7, #8]
    6300:	2b00      	cmp	r3, #0
    6302:	d1ec      	bne.n	62de <test_strtok+0x42>
    6304:	bf00      	nop
    6306:	bf00      	nop
    6308:	46bd      	mov	sp, r7
    630a:	b004      	add	sp, #16
    630c:	bd80      	pop	{r7, pc}
    630e:	bf00      	nop

00006310 <test_strpbrk>:
    6310:	b580      	push	{r7, lr}
    6312:	b084      	sub	sp, #16
    6314:	af00      	add	r7, sp, #0
    6316:	2008      	movs	r0, #8
    6318:	183a      	adds	r2, r7, r0
    631a:	f249 4390 	movw	r3, #38032	; 0x9490
    631e:	f2c0 0300 	movt	r3, #0
    6322:	8819      	ldrh	r1, [r3, #0]
    6324:	8011      	strh	r1, [r2, #0]
    6326:	789b      	ldrb	r3, [r3, #2]
    6328:	7093      	strb	r3, [r2, #2]
    632a:	1d3a      	adds	r2, r7, #4
    632c:	f249 4394 	movw	r3, #38036	; 0x9494
    6330:	f2c0 0300 	movt	r3, #0
    6334:	8819      	ldrh	r1, [r3, #0]
    6336:	8011      	strh	r1, [r2, #0]
    6338:	789b      	ldrb	r3, [r3, #2]
    633a:	7093      	strb	r3, [r2, #2]
    633c:	1d3a      	adds	r2, r7, #4
    633e:	183b      	adds	r3, r7, r0
    6340:	0011      	movs	r1, r2
    6342:	0018      	movs	r0, r3
    6344:	f000 fd34 	bl	6db0 <strpbrk>
    6348:	0003      	movs	r3, r0
    634a:	60fb      	str	r3, [r7, #12]
    634c:	68fb      	ldr	r3, [r7, #12]
    634e:	b15b      	cbz	r3, 6368 <test_strpbrk+0x58>
    6350:	68fb      	ldr	r3, [r7, #12]
    6352:	781b      	ldrb	r3, [r3, #0]
    6354:	001a      	movs	r2, r3
    6356:	f249 4384 	movw	r3, #38020	; 0x9484
    635a:	f2c0 0300 	movt	r3, #0
    635e:	0011      	movs	r1, r2
    6360:	0018      	movs	r0, r3
    6362:	f000 fa86 	bl	6872 <iprintf>
    6366:	e006      	b.n	6376 <test_strpbrk+0x66>
    6368:	f249 438c 	movw	r3, #38028	; 0x948c
    636c:	f2c0 0300 	movt	r3, #0
    6370:	0018      	movs	r0, r3
    6372:	f000 fb75 	bl	6a60 <puts>
    6376:	bf00      	nop
    6378:	46bd      	mov	sp, r7
    637a:	b004      	add	sp, #16
    637c:	bd80      	pop	{r7, pc}
    637e:	bf00      	nop

00006380 <test_stratoi>:
    6380:	b580      	push	{r7, lr}
    6382:	b082      	sub	sp, #8
    6384:	af00      	add	r7, sp, #0
    6386:	003a      	movs	r2, r7
    6388:	f249 4390 	movw	r3, #38032	; 0x9490
    638c:	f2c0 0300 	movt	r3, #0
    6390:	8819      	ldrh	r1, [r3, #0]
    6392:	8011      	strh	r1, [r2, #0]
    6394:	789b      	ldrb	r3, [r3, #2]
    6396:	7093      	strb	r3, [r2, #2]
    6398:	003b      	movs	r3, r7
    639a:	0018      	movs	r0, r3
    639c:	f000 fa42 	bl	6824 <atoi>
    63a0:	0003      	movs	r3, r0
    63a2:	607b      	str	r3, [r7, #4]
    63a4:	687a      	ldr	r2, [r7, #4]
    63a6:	f249 4398 	movw	r3, #38040	; 0x9498
    63aa:	f2c0 0300 	movt	r3, #0
    63ae:	0011      	movs	r1, r2
    63b0:	0018      	movs	r0, r3
    63b2:	f000 fa5e 	bl	6872 <iprintf>
    63b6:	bf00      	nop
    63b8:	46bd      	mov	sp, r7
    63ba:	b002      	add	sp, #8
    63bc:	bd80      	pop	{r7, pc}
    63be:	bf00      	nop

000063c0 <test_strtol>:
    63c0:	b580      	push	{r7, lr}
    63c2:	b08a      	sub	sp, #40	; 0x28
    63c4:	af00      	add	r7, sp, #0
    63c6:	1d39      	adds	r1, r7, #4
    63c8:	f249 43bc 	movw	r3, #38076	; 0x94bc
    63cc:	f2c0 0300 	movt	r3, #0
    63d0:	000a      	movs	r2, r1
    63d2:	6818      	ldr	r0, [r3, #0]
    63d4:	6010      	str	r0, [r2, #0]
    63d6:	8898      	ldrh	r0, [r3, #4]
    63d8:	8090      	strh	r0, [r2, #4]
    63da:	799b      	ldrb	r3, [r3, #6]
    63dc:	7193      	strb	r3, [r2, #6]
    63de:	2307      	movs	r3, #7
    63e0:	18cb      	adds	r3, r1, r3
    63e2:	2217      	movs	r2, #23
    63e4:	2100      	movs	r1, #0
    63e6:	0018      	movs	r0, r3
    63e8:	f000 fa28 	bl	683c <memset>
    63ec:	0039      	movs	r1, r7
    63ee:	1d3b      	adds	r3, r7, #4
    63f0:	220a      	movs	r2, #10
    63f2:	0018      	movs	r0, r3
    63f4:	f000 fe10 	bl	7018 <strtol>
    63f8:	0003      	movs	r3, r0
    63fa:	627b      	str	r3, [r7, #36]	; 0x24
    63fc:	6a7a      	ldr	r2, [r7, #36]	; 0x24
    63fe:	f249 43a4 	movw	r3, #38052	; 0x94a4
    6402:	f2c0 0300 	movt	r3, #0
    6406:	0011      	movs	r1, r2
    6408:	0018      	movs	r0, r3
    640a:	f000 fa32 	bl	6872 <iprintf>
    640e:	683a      	ldr	r2, [r7, #0]
    6410:	f249 43b0 	movw	r3, #38064	; 0x94b0
    6414:	f2c0 0300 	movt	r3, #0
    6418:	0011      	movs	r1, r2
    641a:	0018      	movs	r0, r3
    641c:	f000 fa29 	bl	6872 <iprintf>
    6420:	bf00      	nop
    6422:	46bd      	mov	sp, r7
    6424:	b00a      	add	sp, #40	; 0x28
    6426:	bd80      	pop	{r7, pc}

00006428 <_write>:
    6428:	b580      	push	{r7, lr}
    642a:	b086      	sub	sp, #24
    642c:	af00      	add	r7, sp, #0
    642e:	60f8      	str	r0, [r7, #12]
    6430:	60b9      	str	r1, [r7, #8]
    6432:	607a      	str	r2, [r7, #4]
    6434:	2300      	movs	r3, #0
    6436:	617b      	str	r3, [r7, #20]
    6438:	f24a 0300 	movw	r3, #40960	; 0xa000
    643c:	f2c0 0300 	movt	r3, #0
    6440:	613b      	str	r3, [r7, #16]
    6442:	68fb      	ldr	r3, [r7, #12]
    6444:	2b01      	cmp	r3, #1
    6446:	d005      	beq.n	6454 <_write+0x2c>
    6448:	68fb      	ldr	r3, [r7, #12]
    644a:	2b02      	cmp	r3, #2
    644c:	d002      	beq.n	6454 <_write+0x2c>
    644e:	2301      	movs	r3, #1
    6450:	425b      	negs	r3, r3
    6452:	e023      	b.n	649c <_write+0x74>
    6454:	f242 0380 	movw	r3, #8320	; 0x2080
    6458:	f2c2 0300 	movt	r3, #8192	; 0x2000
    645c:	681b      	ldr	r3, [r3, #0]
    645e:	b1cb      	cbz	r3, 6494 <_write+0x6c>
    6460:	f242 0380 	movw	r3, #8320	; 0x2080
    6464:	f2c2 0300 	movt	r3, #8192	; 0x2000
    6468:	681b      	ldr	r3, [r3, #0]
    646a:	6879      	ldr	r1, [r7, #4]
    646c:	68ba      	ldr	r2, [r7, #8]
    646e:	0010      	movs	r0, r2
    6470:	4798      	blx	r3
    6472:	0003      	movs	r3, r0
    6474:	617b      	str	r3, [r7, #20]
    6476:	e010      	b.n	649a <_write+0x72>
    6478:	693b      	ldr	r3, [r7, #16]
    647a:	685a      	ldr	r2, [r3, #4]
    647c:	68bb      	ldr	r3, [r7, #8]
    647e:	1c59      	adds	r1, r3, #1
    6480:	60b9      	str	r1, [r7, #8]
    6482:	781b      	ldrb	r3, [r3, #0]
    6484:	0018      	movs	r0, r3
    6486:	4790      	blx	r2
    6488:	697b      	ldr	r3, [r7, #20]
    648a:	3301      	adds	r3, #1
    648c:	617b      	str	r3, [r7, #20]
    648e:	687b      	ldr	r3, [r7, #4]
    6490:	3b01      	subs	r3, #1
    6492:	607b      	str	r3, [r7, #4]
    6494:	687b      	ldr	r3, [r7, #4]
    6496:	2b00      	cmp	r3, #0
    6498:	dcee      	bgt.n	6478 <_write+0x50>
    649a:	697b      	ldr	r3, [r7, #20]
    649c:	0018      	movs	r0, r3
    649e:	46bd      	mov	sp, r7
    64a0:	b006      	add	sp, #24
    64a2:	bd80      	pop	{r7, pc}

000064a4 <_read>:
    64a4:	b580      	push	{r7, lr}
    64a6:	b088      	sub	sp, #32
    64a8:	af00      	add	r7, sp, #0
    64aa:	60f8      	str	r0, [r7, #12]
    64ac:	60b9      	str	r1, [r7, #8]
    64ae:	607a      	str	r2, [r7, #4]
    64b0:	2300      	movs	r3, #0
    64b2:	61fb      	str	r3, [r7, #28]
    64b4:	f24a 0300 	movw	r3, #40960	; 0xa000
    64b8:	f2c0 0300 	movt	r3, #0
    64bc:	61bb      	str	r3, [r7, #24]
    64be:	68fb      	ldr	r3, [r7, #12]
    64c0:	b113      	cbz	r3, 64c8 <_read+0x24>
    64c2:	2301      	movs	r3, #1
    64c4:	425b      	negs	r3, r3
    64c6:	e031      	b.n	652c <_read+0x88>
    64c8:	f242 0380 	movw	r3, #8320	; 0x2080
    64cc:	f2c2 0300 	movt	r3, #8192	; 0x2000
    64d0:	685b      	ldr	r3, [r3, #4]
    64d2:	b33b      	cbz	r3, 6524 <_read+0x80>
    64d4:	f242 0380 	movw	r3, #8320	; 0x2080
    64d8:	f2c2 0300 	movt	r3, #8192	; 0x2000
    64dc:	685b      	ldr	r3, [r3, #4]
    64de:	6879      	ldr	r1, [r7, #4]
    64e0:	68ba      	ldr	r2, [r7, #8]
    64e2:	0010      	movs	r0, r2
    64e4:	4798      	blx	r3
    64e6:	0003      	movs	r3, r0
    64e8:	61fb      	str	r3, [r7, #28]
    64ea:	e01e      	b.n	652a <_read+0x86>
    64ec:	69bb      	ldr	r3, [r7, #24]
    64ee:	689b      	ldr	r3, [r3, #8]
    64f0:	2001      	movs	r0, #1
    64f2:	4798      	blx	r3
    64f4:	0003      	movs	r3, r0
    64f6:	617b      	str	r3, [r7, #20]
    64f8:	697b      	ldr	r3, [r7, #20]
    64fa:	2b00      	cmp	r3, #0
    64fc:	db02      	blt.n	6504 <_read+0x60>
    64fe:	697b      	ldr	r3, [r7, #20]
    6500:	2b0d      	cmp	r3, #13
    6502:	d103      	bne.n	650c <_read+0x68>
    6504:	68bb      	ldr	r3, [r7, #8]
    6506:	2200      	movs	r2, #0
    6508:	701a      	strb	r2, [r3, #0]
    650a:	e00e      	b.n	652a <_read+0x86>
    650c:	68bb      	ldr	r3, [r7, #8]
    650e:	1c5a      	adds	r2, r3, #1
    6510:	60ba      	str	r2, [r7, #8]
    6512:	697a      	ldr	r2, [r7, #20]
    6514:	b2d2      	uxtb	r2, r2
    6516:	701a      	strb	r2, [r3, #0]
    6518:	69fb      	ldr	r3, [r7, #28]
    651a:	3301      	adds	r3, #1
    651c:	61fb      	str	r3, [r7, #28]
    651e:	687b      	ldr	r3, [r7, #4]
    6520:	3b01      	subs	r3, #1
    6522:	607b      	str	r3, [r7, #4]
    6524:	687b      	ldr	r3, [r7, #4]
    6526:	2b00      	cmp	r3, #0
    6528:	dce0      	bgt.n	64ec <_read+0x48>
    652a:	69fb      	ldr	r3, [r7, #28]
    652c:	0018      	movs	r0, r3
    652e:	46bd      	mov	sp, r7
    6530:	b008      	add	sp, #32
    6532:	bd80      	pop	{r7, pc}

00006534 <rtl_string_test>:
    6534:	b510      	push	{r4, lr}
    6536:	f7ff fd67 	bl	6008 <test_strcat>
    653a:	f7ff fd95 	bl	6068 <test_strchr>
    653e:	f7ff fdab 	bl	6098 <test_strcpy>
    6542:	f7ff fddd 	bl	6100 <test_strncat>
    6546:	f7ff fe0d 	bl	6164 <test_strncmp>
    654a:	f7ff fe2b 	bl	61a4 <test_strncpy>
    654e:	f7ff fe5b 	bl	6208 <test_strsep>
    6552:	f7ff fe87 	bl	6264 <test_strstr>
    6556:	f7ff fea1 	bl	629c <test_strtok>
    655a:	f7ff fed9 	bl	6310 <test_strpbrk>
    655e:	f7ff ff0f 	bl	6380 <test_stratoi>
    6562:	f7ff ff2d 	bl	63c0 <test_strtol>
    6566:	2000      	movs	r0, #0
    6568:	bd10      	pop	{r4, pc}
    656a:	bf00      	nop

0000656c <stdlib_vsnprintf_call>:
    656c:	b40f      	push	{r0, r1, r2, r3}
    656e:	b500      	push	{lr}
    6570:	b085      	sub	sp, #20
    6572:	ab06      	add	r3, sp, #24
    6574:	cb04      	ldmia	r3!, {r2}
    6576:	2106      	movs	r1, #6
    6578:	a802      	add	r0, sp, #8
    657a:	9301      	str	r3, [sp, #4]
    657c:	f000 fd82 	bl	7084 <vsniprintf>
    6580:	b005      	add	sp, #20
    6582:	bc08      	pop	{r3}
    6584:	b004      	add	sp, #16
    6586:	4718      	bx	r3

00006588 <stdlib_printf_call>:
    6588:	b570      	push	{r4, r5, r6, lr}
    658a:	f249 44dc 	movw	r4, #38108	; 0x94dc
    658e:	f2c0 0400 	movt	r4, #0
    6592:	b086      	sub	sp, #24
    6594:	0005      	movs	r5, r0
    6596:	0020      	movs	r0, r4
    6598:	000e      	movs	r6, r1
    659a:	f000 fa61 	bl	6a60 <puts>
    659e:	0020      	movs	r0, r4
    65a0:	0029      	movs	r1, r5
    65a2:	3010      	adds	r0, #16
    65a4:	f000 f965 	bl	6872 <iprintf>
    65a8:	2078      	movs	r0, #120	; 0x78
    65aa:	f000 f987 	bl	68bc <putchar>
    65ae:	2300      	movs	r3, #0
    65b0:	0021      	movs	r1, r4
    65b2:	9303      	str	r3, [sp, #12]
    65b4:	9302      	str	r3, [sp, #8]
    65b6:	9301      	str	r3, [sp, #4]
    65b8:	9300      	str	r3, [sp, #0]
    65ba:	2200      	movs	r2, #0
    65bc:	0030      	movs	r0, r6
    65be:	3114      	adds	r1, #20
    65c0:	f000 fad8 	bl	6b74 <siprintf>
    65c4:	0022      	movs	r2, r4
    65c6:	002b      	movs	r3, r5
    65c8:	3230      	adds	r2, #48	; 0x30
    65ca:	2106      	movs	r1, #6
    65cc:	a804      	add	r0, sp, #16
    65ce:	f000 fa81 	bl	6ad4 <sniprintf>
    65d2:	0020      	movs	r0, r4
    65d4:	2100      	movs	r1, #0
    65d6:	3038      	adds	r0, #56	; 0x38
    65d8:	f7ff ffc8 	bl	656c <stdlib_vsnprintf_call>
    65dc:	b006      	add	sp, #24
    65de:	bd70      	pop	{r4, r5, r6, pc}

000065e0 <stdlib_scanf_call>:
    65e0:	b510      	push	{r4, lr}
    65e2:	2400      	movs	r4, #0
    65e4:	b092      	sub	sp, #72	; 0x48
    65e6:	2218      	movs	r2, #24
    65e8:	2100      	movs	r1, #0
    65ea:	a805      	add	r0, sp, #20
    65ec:	9404      	str	r4, [sp, #16]
    65ee:	f000 f925 	bl	683c <memset>
    65f2:	2218      	movs	r2, #24
    65f4:	2100      	movs	r1, #0
    65f6:	a80c      	add	r0, sp, #48	; 0x30
    65f8:	940b      	str	r4, [sp, #44]	; 0x2c
    65fa:	f000 f91f 	bl	683c <memset>
    65fe:	220f      	movs	r2, #15
    6600:	446a      	add	r2, sp
    6602:	7014      	strb	r4, [r2, #0]
    6604:	f249 44dc 	movw	r4, #38108	; 0x94dc
    6608:	f2c0 0400 	movt	r4, #0
    660c:	0021      	movs	r1, r4
    660e:	0020      	movs	r0, r4
    6610:	ab0b      	add	r3, sp, #44	; 0x2c
    6612:	9300      	str	r3, [sp, #0]
    6614:	3140      	adds	r1, #64	; 0x40
    6616:	ab04      	add	r3, sp, #16
    6618:	3050      	adds	r0, #80	; 0x50
    661a:	f000 facb 	bl	6bb4 <siscanf>
    661e:	0001      	movs	r1, r0
    6620:	0020      	movs	r0, r4
    6622:	3074      	adds	r0, #116	; 0x74
    6624:	f000 f925 	bl	6872 <iprintf>
    6628:	b012      	add	sp, #72	; 0x48
    662a:	bd10      	pop	{r4, pc}

0000662c <stdlib_test>:
    662c:	b510      	push	{r4, lr}
    662e:	2100      	movs	r1, #0
    6630:	2000      	movs	r0, #0
    6632:	f7ff ffa9 	bl	6588 <stdlib_printf_call>
    6636:	f7ff ffd3 	bl	65e0 <stdlib_scanf_call>
    663a:	f7ff fce5 	bl	6008 <test_strcat>
    663e:	f7ff fd13 	bl	6068 <test_strchr>
    6642:	f7ff fd29 	bl	6098 <test_strcpy>
    6646:	f7ff fd5b 	bl	6100 <test_strncat>
    664a:	f7ff fd8b 	bl	6164 <test_strncmp>
    664e:	f7ff fda9 	bl	61a4 <test_strncpy>
    6652:	f7ff fdd9 	bl	6208 <test_strsep>
    6656:	f7ff fe05 	bl	6264 <test_strstr>
    665a:	f7ff fe1f 	bl	629c <test_strtok>
    665e:	f7ff fe57 	bl	6310 <test_strpbrk>
    6662:	f7ff fe8d 	bl	6380 <test_stratoi>
    6666:	f7ff feab 	bl	63c0 <test_strtol>
    666a:	bd10      	pop	{r4, pc}

0000666c <stdlib_memset>:
    666c:	b570      	push	{r4, r5, r6, lr}
    666e:	46c6      	mov	lr, r8
    6670:	b500      	push	{lr}
    6672:	0783      	lsls	r3, r0, #30
    6674:	d04e      	beq.n	6714 <stdlib_memset+0xa8>
    6676:	1e54      	subs	r4, r2, #1
    6678:	2a00      	cmp	r2, #0
    667a:	d048      	beq.n	670e <stdlib_memset+0xa2>
    667c:	0003      	movs	r3, r0
    667e:	2503      	movs	r5, #3
    6680:	b2ce      	uxtb	r6, r1
    6682:	e001      	b.n	6688 <stdlib_memset+0x1c>
    6684:	3c01      	subs	r4, #1
    6686:	d342      	bcc.n	670e <stdlib_memset+0xa2>
    6688:	3301      	adds	r3, #1
    668a:	1e5a      	subs	r2, r3, #1
    668c:	7016      	strb	r6, [r2, #0]
    668e:	422b      	tst	r3, r5
    6690:	d1f8      	bne.n	6684 <stdlib_memset+0x18>
    6692:	2c03      	cmp	r4, #3
    6694:	d934      	bls.n	6700 <stdlib_memset+0x94>
    6696:	22ff      	movs	r2, #255	; 0xff
    6698:	400a      	ands	r2, r1
    669a:	0215      	lsls	r5, r2, #8
    669c:	4315      	orrs	r5, r2
    669e:	042a      	lsls	r2, r5, #16
    66a0:	4315      	orrs	r5, r2
    66a2:	2c0f      	cmp	r4, #15
    66a4:	d939      	bls.n	671a <stdlib_memset+0xae>
    66a6:	0026      	movs	r6, r4
    66a8:	3e10      	subs	r6, #16
    66aa:	0936      	lsrs	r6, r6, #4
    66ac:	46b4      	mov	ip, r6
    66ae:	0136      	lsls	r6, r6, #4
    66b0:	46b0      	mov	r8, r6
    66b2:	001e      	movs	r6, r3
    66b4:	001a      	movs	r2, r3
    66b6:	3610      	adds	r6, #16
    66b8:	4446      	add	r6, r8
    66ba:	6015      	str	r5, [r2, #0]
    66bc:	6055      	str	r5, [r2, #4]
    66be:	6095      	str	r5, [r2, #8]
    66c0:	60d5      	str	r5, [r2, #12]
    66c2:	3210      	adds	r2, #16
    66c4:	42b2      	cmp	r2, r6
    66c6:	d1f8      	bne.n	66ba <stdlib_memset+0x4e>
    66c8:	4662      	mov	r2, ip
    66ca:	3201      	adds	r2, #1
    66cc:	0112      	lsls	r2, r2, #4
    66ce:	189b      	adds	r3, r3, r2
    66d0:	220f      	movs	r2, #15
    66d2:	4022      	ands	r2, r4
    66d4:	4694      	mov	ip, r2
    66d6:	220c      	movs	r2, #12
    66d8:	4022      	ands	r2, r4
    66da:	4664      	mov	r4, ip
    66dc:	b182      	cbz	r2, 6700 <stdlib_memset+0x94>
    66de:	4664      	mov	r4, ip
    66e0:	1f26      	subs	r6, r4, #4
    66e2:	08b6      	lsrs	r6, r6, #2
    66e4:	00b4      	lsls	r4, r6, #2
    66e6:	46a0      	mov	r8, r4
    66e8:	001a      	movs	r2, r3
    66ea:	1d1c      	adds	r4, r3, #4
    66ec:	4444      	add	r4, r8
    66ee:	c220      	stmia	r2!, {r5}
    66f0:	42a2      	cmp	r2, r4
    66f2:	d1fc      	bne.n	66ee <stdlib_memset+0x82>
    66f4:	2403      	movs	r4, #3
    66f6:	4662      	mov	r2, ip
    66f8:	3601      	adds	r6, #1
    66fa:	00b6      	lsls	r6, r6, #2
    66fc:	199b      	adds	r3, r3, r6
    66fe:	4014      	ands	r4, r2
    6700:	b12c      	cbz	r4, 670e <stdlib_memset+0xa2>
    6702:	b2c9      	uxtb	r1, r1
    6704:	191c      	adds	r4, r3, r4
    6706:	7019      	strb	r1, [r3, #0]
    6708:	3301      	adds	r3, #1
    670a:	429c      	cmp	r4, r3
    670c:	d1fb      	bne.n	6706 <stdlib_memset+0x9a>
    670e:	bc40      	pop	{r6}
    6710:	46b0      	mov	r8, r6
    6712:	bd70      	pop	{r4, r5, r6, pc}
    6714:	0003      	movs	r3, r0
    6716:	0014      	movs	r4, r2
    6718:	e7bb      	b.n	6692 <stdlib_memset+0x26>
    671a:	46a4      	mov	ip, r4
    671c:	e7df      	b.n	66de <stdlib_memset+0x72>
    671e:	bf00      	nop

00006720 <stdlib_memcpy>:
    6720:	b570      	push	{r4, r5, r6, lr}
    6722:	46ce      	mov	lr, r9
    6724:	4646      	mov	r6, r8
    6726:	4681      	mov	r9, r0
    6728:	b540      	push	{r6, lr}
    672a:	2a03      	cmp	r2, #3
    672c:	d948      	bls.n	67c0 <stdlib_memcpy+0xa0>
    672e:	2603      	movs	r6, #3
    6730:	000d      	movs	r5, r1
    6732:	0003      	movs	r3, r0
    6734:	4305      	orrs	r5, r0
    6736:	0030      	movs	r0, r6
    6738:	4028      	ands	r0, r5
    673a:	000c      	movs	r4, r1
    673c:	4684      	mov	ip, r0
    673e:	422e      	tst	r6, r5
    6740:	d143      	bne.n	67ca <stdlib_memcpy+0xaa>
    6742:	2a0f      	cmp	r2, #15
    6744:	d944      	bls.n	67d0 <stdlib_memcpy+0xb0>
    6746:	0015      	movs	r5, r2
    6748:	3d10      	subs	r5, #16
    674a:	0928      	lsrs	r0, r5, #4
    674c:	0106      	lsls	r6, r0, #4
    674e:	4680      	mov	r8, r0
    6750:	0030      	movs	r0, r6
    6752:	4448      	add	r0, r9
    6754:	6826      	ldr	r6, [r4, #0]
    6756:	001d      	movs	r5, r3
    6758:	601e      	str	r6, [r3, #0]
    675a:	6866      	ldr	r6, [r4, #4]
    675c:	605e      	str	r6, [r3, #4]
    675e:	68a6      	ldr	r6, [r4, #8]
    6760:	609e      	str	r6, [r3, #8]
    6762:	68e6      	ldr	r6, [r4, #12]
    6764:	3410      	adds	r4, #16
    6766:	60de      	str	r6, [r3, #12]
    6768:	3310      	adds	r3, #16
    676a:	4285      	cmp	r5, r0
    676c:	d1f2      	bne.n	6754 <stdlib_memcpy+0x34>
    676e:	4644      	mov	r4, r8
    6770:	250f      	movs	r5, #15
    6772:	230c      	movs	r3, #12
    6774:	3401      	adds	r4, #1
    6776:	0124      	lsls	r4, r4, #4
    6778:	4015      	ands	r5, r2
    677a:	4013      	ands	r3, r2
    677c:	1909      	adds	r1, r1, r4
    677e:	002a      	movs	r2, r5
    6780:	444c      	add	r4, r9
    6782:	b1f3      	cbz	r3, 67c2 <stdlib_memcpy+0xa2>
    6784:	1f2b      	subs	r3, r5, #4
    6786:	089b      	lsrs	r3, r3, #2
    6788:	3301      	adds	r3, #1
    678a:	009b      	lsls	r3, r3, #2
    678c:	4662      	mov	r2, ip
    678e:	f240 0804 	movw	r8, #4
    6792:	5888      	ldr	r0, [r1, r2]
    6794:	44c4      	add	ip, r8
    6796:	50a0      	str	r0, [r4, r2]
    6798:	459c      	cmp	ip, r3
    679a:	d1f7      	bne.n	678c <stdlib_memcpy+0x6c>
    679c:	2203      	movs	r2, #3
    679e:	402a      	ands	r2, r5
    67a0:	4464      	add	r4, ip
    67a2:	4461      	add	r1, ip
    67a4:	1e55      	subs	r5, r2, #1
    67a6:	b132      	cbz	r2, 67b6 <stdlib_memcpy+0x96>
    67a8:	2300      	movs	r3, #0
    67aa:	5cca      	ldrb	r2, [r1, r3]
    67ac:	54e2      	strb	r2, [r4, r3]
    67ae:	001a      	movs	r2, r3
    67b0:	3301      	adds	r3, #1
    67b2:	4295      	cmp	r5, r2
    67b4:	d1f9      	bne.n	67aa <stdlib_memcpy+0x8a>
    67b6:	4648      	mov	r0, r9
    67b8:	bc60      	pop	{r5, r6}
    67ba:	46b1      	mov	r9, r6
    67bc:	46a8      	mov	r8, r5
    67be:	bd70      	pop	{r4, r5, r6, pc}
    67c0:	0004      	movs	r4, r0
    67c2:	1e55      	subs	r5, r2, #1
    67c4:	2a00      	cmp	r2, #0
    67c6:	d1ef      	bne.n	67a8 <stdlib_memcpy+0x88>
    67c8:	e7f5      	b.n	67b6 <stdlib_memcpy+0x96>
    67ca:	464c      	mov	r4, r9
    67cc:	1e55      	subs	r5, r2, #1
    67ce:	e7eb      	b.n	67a8 <stdlib_memcpy+0x88>
    67d0:	464c      	mov	r4, r9
    67d2:	0015      	movs	r5, r2
    67d4:	e7d6      	b.n	6784 <stdlib_memcpy+0x64>
    67d6:	bf00      	nop

000067d8 <stdlib_init>:
    67d8:	f242 02a4 	movw	r2, #8356	; 0x20a4
    67dc:	f242 0080 	movw	r0, #8320	; 0x2080
    67e0:	f2c2 0200 	movt	r2, #8192	; 0x2000
    67e4:	f2c2 0000 	movt	r0, #8192	; 0x2000
    67e8:	b510      	push	{r4, lr}
    67ea:	2100      	movs	r1, #0
    67ec:	1a12      	subs	r2, r2, r0
    67ee:	f7ff ff3d 	bl	666c <stdlib_memset>
    67f2:	f242 0264 	movw	r2, #8292	; 0x2064
    67f6:	f242 0000 	movw	r0, #8192	; 0x2000
    67fa:	f249 71e0 	movw	r1, #38880	; 0x97e0
    67fe:	f2c2 0000 	movt	r0, #8192	; 0x2000
    6802:	f2c2 0200 	movt	r2, #8192	; 0x2000
    6806:	1a12      	subs	r2, r2, r0
    6808:	f2c0 0100 	movt	r1, #0
    680c:	f7ff ff88 	bl	6720 <stdlib_memcpy>
    6810:	bd10      	pop	{r4, pc}
    6812:	bf00      	nop

00006814 <stdlib_loguart_relocation>:
    6814:	f242 0380 	movw	r3, #8320	; 0x2080
    6818:	f2c2 0300 	movt	r3, #8192	; 0x2000
    681c:	6019      	str	r1, [r3, #0]
    681e:	6058      	str	r0, [r3, #4]
    6820:	4770      	bx	lr
    6822:	bf00      	nop

00006824 <atoi>:
    6824:	b510      	push	{r4, lr}
    6826:	220a      	movs	r2, #10
    6828:	2100      	movs	r1, #0
    682a:	f000 fbf5 	bl	7018 <strtol>
    682e:	bd10      	pop	{r4, pc}

00006830 <_atoi_r>:
    6830:	b510      	push	{r4, lr}
    6832:	230a      	movs	r3, #10
    6834:	2200      	movs	r2, #0
    6836:	f000 fbdf 	bl	6ff8 <_strtol_r>
    683a:	bd10      	pop	{r4, pc}

0000683c <memset>:
    683c:	0003      	movs	r3, r0
    683e:	1882      	adds	r2, r0, r2
    6840:	4293      	cmp	r3, r2
    6842:	d100      	bne.n	6846 <memset+0xa>
    6844:	4770      	bx	lr
    6846:	7019      	strb	r1, [r3, #0]
    6848:	3301      	adds	r3, #1
    684a:	e7f9      	b.n	6840 <memset+0x4>

0000684c <_iprintf_r>:
    684c:	b40e      	push	{r1, r2, r3}
    684e:	b517      	push	{r0, r1, r2, r4, lr}
    6850:	0004      	movs	r4, r0
    6852:	b118      	cbz	r0, 685c <_iprintf_r+0x10>
    6854:	6983      	ldr	r3, [r0, #24]
    6856:	b90b      	cbnz	r3, 685c <_iprintf_r+0x10>
    6858:	f000 fe98 	bl	758c <__sinit>
    685c:	ab06      	add	r3, sp, #24
    685e:	0020      	movs	r0, r4
    6860:	9a05      	ldr	r2, [sp, #20]
    6862:	68a1      	ldr	r1, [r4, #8]
    6864:	9301      	str	r3, [sp, #4]
    6866:	f001 fe15 	bl	8494 <_vfiprintf_r>
    686a:	bc1e      	pop	{r1, r2, r3, r4}
    686c:	bc08      	pop	{r3}
    686e:	b003      	add	sp, #12
    6870:	4718      	bx	r3

00006872 <iprintf>:
    6872:	b40f      	push	{r0, r1, r2, r3}
    6874:	4b09      	ldr	r3, [pc, #36]	; (689c <iprintf+0x2a>)
    6876:	b513      	push	{r0, r1, r4, lr}
    6878:	681c      	ldr	r4, [r3, #0]
    687a:	b124      	cbz	r4, 6886 <iprintf+0x14>
    687c:	69a3      	ldr	r3, [r4, #24]
    687e:	b913      	cbnz	r3, 6886 <iprintf+0x14>
    6880:	0020      	movs	r0, r4
    6882:	f000 fe83 	bl	758c <__sinit>
    6886:	ab05      	add	r3, sp, #20
    6888:	0020      	movs	r0, r4
    688a:	9a04      	ldr	r2, [sp, #16]
    688c:	68a1      	ldr	r1, [r4, #8]
    688e:	9301      	str	r3, [sp, #4]
    6890:	f001 fe00 	bl	8494 <_vfiprintf_r>
    6894:	bc16      	pop	{r1, r2, r4}
    6896:	bc08      	pop	{r3}
    6898:	b004      	add	sp, #16
    689a:	4718      	bx	r3
    689c:	20002000 	.word	0x20002000

000068a0 <_putchar_r>:
    68a0:	b570      	push	{r4, r5, r6, lr}
    68a2:	0004      	movs	r4, r0
    68a4:	000d      	movs	r5, r1
    68a6:	b118      	cbz	r0, 68b0 <_putchar_r+0x10>
    68a8:	6983      	ldr	r3, [r0, #24]
    68aa:	b90b      	cbnz	r3, 68b0 <_putchar_r+0x10>
    68ac:	f000 fe6e 	bl	758c <__sinit>
    68b0:	0029      	movs	r1, r5
    68b2:	0020      	movs	r0, r4
    68b4:	68a2      	ldr	r2, [r4, #8]
    68b6:	f000 f813 	bl	68e0 <_putc_r>
    68ba:	bd70      	pop	{r4, r5, r6, pc}

000068bc <putchar>:
    68bc:	4b07      	ldr	r3, [pc, #28]	; (68dc <putchar+0x20>)
    68be:	b570      	push	{r4, r5, r6, lr}
    68c0:	681c      	ldr	r4, [r3, #0]
    68c2:	0005      	movs	r5, r0
    68c4:	b124      	cbz	r4, 68d0 <putchar+0x14>
    68c6:	69a3      	ldr	r3, [r4, #24]
    68c8:	b913      	cbnz	r3, 68d0 <putchar+0x14>
    68ca:	0020      	movs	r0, r4
    68cc:	f000 fe5e 	bl	758c <__sinit>
    68d0:	0029      	movs	r1, r5
    68d2:	0020      	movs	r0, r4
    68d4:	68a2      	ldr	r2, [r4, #8]
    68d6:	f000 f803 	bl	68e0 <_putc_r>
    68da:	bd70      	pop	{r4, r5, r6, pc}
    68dc:	20002000 	.word	0x20002000

000068e0 <_putc_r>:
    68e0:	b570      	push	{r4, r5, r6, lr}
    68e2:	0006      	movs	r6, r0
    68e4:	000d      	movs	r5, r1
    68e6:	0014      	movs	r4, r2
    68e8:	b118      	cbz	r0, 68f2 <_putc_r+0x12>
    68ea:	6983      	ldr	r3, [r0, #24]
    68ec:	b90b      	cbnz	r3, 68f2 <_putc_r+0x12>
    68ee:	f000 fe4d 	bl	758c <__sinit>
    68f2:	4b1c      	ldr	r3, [pc, #112]	; (6964 <_putc_r+0x84>)
    68f4:	429c      	cmp	r4, r3
    68f6:	d124      	bne.n	6942 <_putc_r+0x62>
    68f8:	6874      	ldr	r4, [r6, #4]
    68fa:	6e63      	ldr	r3, [r4, #100]	; 0x64
    68fc:	07db      	lsls	r3, r3, #31
    68fe:	d405      	bmi.n	690c <_putc_r+0x2c>
    6900:	89a3      	ldrh	r3, [r4, #12]
    6902:	059b      	lsls	r3, r3, #22
    6904:	d402      	bmi.n	690c <_putc_r+0x2c>
    6906:	6da0      	ldr	r0, [r4, #88]	; 0x58
    6908:	f000 ff36 	bl	7778 <__retarget_lock_acquire_recursive>
    690c:	68a3      	ldr	r3, [r4, #8]
    690e:	3b01      	subs	r3, #1
    6910:	60a3      	str	r3, [r4, #8]
    6912:	2b00      	cmp	r3, #0
    6914:	da05      	bge.n	6922 <_putc_r+0x42>
    6916:	69a2      	ldr	r2, [r4, #24]
    6918:	4293      	cmp	r3, r2
    691a:	db1c      	blt.n	6956 <_putc_r+0x76>
    691c:	b2eb      	uxtb	r3, r5
    691e:	2b0a      	cmp	r3, #10
    6920:	d019      	beq.n	6956 <_putc_r+0x76>
    6922:	6823      	ldr	r3, [r4, #0]
    6924:	1c5a      	adds	r2, r3, #1
    6926:	6022      	str	r2, [r4, #0]
    6928:	701d      	strb	r5, [r3, #0]
    692a:	b2ed      	uxtb	r5, r5
    692c:	6e63      	ldr	r3, [r4, #100]	; 0x64
    692e:	07db      	lsls	r3, r3, #31
    6930:	d405      	bmi.n	693e <_putc_r+0x5e>
    6932:	89a3      	ldrh	r3, [r4, #12]
    6934:	059b      	lsls	r3, r3, #22
    6936:	d402      	bmi.n	693e <_putc_r+0x5e>
    6938:	6da0      	ldr	r0, [r4, #88]	; 0x58
    693a:	f000 ff23 	bl	7784 <__retarget_lock_release_recursive>
    693e:	0028      	movs	r0, r5
    6940:	bd70      	pop	{r4, r5, r6, pc}
    6942:	4b09      	ldr	r3, [pc, #36]	; (6968 <_putc_r+0x88>)
    6944:	429c      	cmp	r4, r3
    6946:	d101      	bne.n	694c <_putc_r+0x6c>
    6948:	68b4      	ldr	r4, [r6, #8]
    694a:	e7d6      	b.n	68fa <_putc_r+0x1a>
    694c:	4b07      	ldr	r3, [pc, #28]	; (696c <_putc_r+0x8c>)
    694e:	429c      	cmp	r4, r3
    6950:	d1d3      	bne.n	68fa <_putc_r+0x1a>
    6952:	68f4      	ldr	r4, [r6, #12]
    6954:	e7d1      	b.n	68fa <_putc_r+0x1a>
    6956:	0029      	movs	r1, r5
    6958:	0022      	movs	r2, r4
    695a:	0030      	movs	r0, r6
    695c:	f000 fb9e 	bl	709c <__swbuf_r>
    6960:	0005      	movs	r5, r0
    6962:	e7e3      	b.n	692c <_putc_r+0x4c>
    6964:	00009760 	.word	0x00009760
    6968:	00009740 	.word	0x00009740
    696c:	00009720 	.word	0x00009720

00006970 <putc>:
    6970:	b510      	push	{r4, lr}
    6972:	4b03      	ldr	r3, [pc, #12]	; (6980 <putc+0x10>)
    6974:	000a      	movs	r2, r1
    6976:	0001      	movs	r1, r0
    6978:	6818      	ldr	r0, [r3, #0]
    697a:	f7ff ffb1 	bl	68e0 <_putc_r>
    697e:	bd10      	pop	{r4, pc}
    6980:	20002000 	.word	0x20002000

00006984 <_puts_r>:
    6984:	b570      	push	{r4, r5, r6, lr}
    6986:	0005      	movs	r5, r0
    6988:	000e      	movs	r6, r1
    698a:	b118      	cbz	r0, 6994 <_puts_r+0x10>
    698c:	6983      	ldr	r3, [r0, #24]
    698e:	b90b      	cbnz	r3, 6994 <_puts_r+0x10>
    6990:	f000 fdfc 	bl	758c <__sinit>
    6994:	69ab      	ldr	r3, [r5, #24]
    6996:	68ac      	ldr	r4, [r5, #8]
    6998:	b913      	cbnz	r3, 69a0 <_puts_r+0x1c>
    699a:	0028      	movs	r0, r5
    699c:	f000 fdf6 	bl	758c <__sinit>
    69a0:	4b2c      	ldr	r3, [pc, #176]	; (6a54 <_puts_r+0xd0>)
    69a2:	429c      	cmp	r4, r3
    69a4:	d120      	bne.n	69e8 <_puts_r+0x64>
    69a6:	686c      	ldr	r4, [r5, #4]
    69a8:	6e63      	ldr	r3, [r4, #100]	; 0x64
    69aa:	07db      	lsls	r3, r3, #31
    69ac:	d405      	bmi.n	69ba <_puts_r+0x36>
    69ae:	89a3      	ldrh	r3, [r4, #12]
    69b0:	059b      	lsls	r3, r3, #22
    69b2:	d402      	bmi.n	69ba <_puts_r+0x36>
    69b4:	6da0      	ldr	r0, [r4, #88]	; 0x58
    69b6:	f000 fedf 	bl	7778 <__retarget_lock_acquire_recursive>
    69ba:	89a3      	ldrh	r3, [r4, #12]
    69bc:	071b      	lsls	r3, r3, #28
    69be:	d501      	bpl.n	69c4 <_puts_r+0x40>
    69c0:	6923      	ldr	r3, [r4, #16]
    69c2:	bb43      	cbnz	r3, 6a16 <_puts_r+0x92>
    69c4:	0021      	movs	r1, r4
    69c6:	0028      	movs	r0, r5
    69c8:	f000 fbd6 	bl	7178 <__swsetup_r>
    69cc:	b318      	cbz	r0, 6a16 <_puts_r+0x92>
    69ce:	2501      	movs	r5, #1
    69d0:	426d      	negs	r5, r5
    69d2:	6e63      	ldr	r3, [r4, #100]	; 0x64
    69d4:	07db      	lsls	r3, r3, #31
    69d6:	d405      	bmi.n	69e4 <_puts_r+0x60>
    69d8:	89a3      	ldrh	r3, [r4, #12]
    69da:	059b      	lsls	r3, r3, #22
    69dc:	d402      	bmi.n	69e4 <_puts_r+0x60>
    69de:	6da0      	ldr	r0, [r4, #88]	; 0x58
    69e0:	f000 fed0 	bl	7784 <__retarget_lock_release_recursive>
    69e4:	0028      	movs	r0, r5
    69e6:	bd70      	pop	{r4, r5, r6, pc}
    69e8:	4b1b      	ldr	r3, [pc, #108]	; (6a58 <_puts_r+0xd4>)
    69ea:	429c      	cmp	r4, r3
    69ec:	d101      	bne.n	69f2 <_puts_r+0x6e>
    69ee:	68ac      	ldr	r4, [r5, #8]
    69f0:	e7da      	b.n	69a8 <_puts_r+0x24>
    69f2:	4b1a      	ldr	r3, [pc, #104]	; (6a5c <_puts_r+0xd8>)
    69f4:	429c      	cmp	r4, r3
    69f6:	d1d7      	bne.n	69a8 <_puts_r+0x24>
    69f8:	68ec      	ldr	r4, [r5, #12]
    69fa:	e7d5      	b.n	69a8 <_puts_r+0x24>
    69fc:	3601      	adds	r6, #1
    69fe:	60a3      	str	r3, [r4, #8]
    6a00:	2b00      	cmp	r3, #0
    6a02:	da04      	bge.n	6a0e <_puts_r+0x8a>
    6a04:	69a2      	ldr	r2, [r4, #24]
    6a06:	429a      	cmp	r2, r3
    6a08:	dc16      	bgt.n	6a38 <_puts_r+0xb4>
    6a0a:	290a      	cmp	r1, #10
    6a0c:	d014      	beq.n	6a38 <_puts_r+0xb4>
    6a0e:	6823      	ldr	r3, [r4, #0]
    6a10:	1c5a      	adds	r2, r3, #1
    6a12:	6022      	str	r2, [r4, #0]
    6a14:	7019      	strb	r1, [r3, #0]
    6a16:	68a3      	ldr	r3, [r4, #8]
    6a18:	7831      	ldrb	r1, [r6, #0]
    6a1a:	3b01      	subs	r3, #1
    6a1c:	2900      	cmp	r1, #0
    6a1e:	d1ed      	bne.n	69fc <_puts_r+0x78>
    6a20:	60a3      	str	r3, [r4, #8]
    6a22:	2b00      	cmp	r3, #0
    6a24:	da0f      	bge.n	6a46 <_puts_r+0xc2>
    6a26:	0028      	movs	r0, r5
    6a28:	0022      	movs	r2, r4
    6a2a:	310a      	adds	r1, #10
    6a2c:	f000 fb36 	bl	709c <__swbuf_r>
    6a30:	250a      	movs	r5, #10
    6a32:	1c43      	adds	r3, r0, #1
    6a34:	d1cd      	bne.n	69d2 <_puts_r+0x4e>
    6a36:	e7ca      	b.n	69ce <_puts_r+0x4a>
    6a38:	0022      	movs	r2, r4
    6a3a:	0028      	movs	r0, r5
    6a3c:	f000 fb2e 	bl	709c <__swbuf_r>
    6a40:	1c43      	adds	r3, r0, #1
    6a42:	d1e8      	bne.n	6a16 <_puts_r+0x92>
    6a44:	e7c3      	b.n	69ce <_puts_r+0x4a>
    6a46:	250a      	movs	r5, #10
    6a48:	6823      	ldr	r3, [r4, #0]
    6a4a:	1c5a      	adds	r2, r3, #1
    6a4c:	6022      	str	r2, [r4, #0]
    6a4e:	701d      	strb	r5, [r3, #0]
    6a50:	e7bf      	b.n	69d2 <_puts_r+0x4e>
    6a52:	bf00      	nop
    6a54:	00009760 	.word	0x00009760
    6a58:	00009740 	.word	0x00009740
    6a5c:	00009720 	.word	0x00009720

00006a60 <puts>:
    6a60:	b510      	push	{r4, lr}
    6a62:	4b03      	ldr	r3, [pc, #12]	; (6a70 <puts+0x10>)
    6a64:	0001      	movs	r1, r0
    6a66:	6818      	ldr	r0, [r3, #0]
    6a68:	f7ff ff8c 	bl	6984 <_puts_r>
    6a6c:	bd10      	pop	{r4, pc}
    6a6e:	bf00      	nop
    6a70:	20002000 	.word	0x20002000

00006a74 <_sniprintf_r>:
    6a74:	b408      	push	{r3}
    6a76:	b530      	push	{r4, r5, lr}
    6a78:	0005      	movs	r5, r0
    6a7a:	0014      	movs	r4, r2
    6a7c:	b09c      	sub	sp, #112	; 0x70
    6a7e:	2a00      	cmp	r2, #0
    6a80:	da08      	bge.n	6a94 <_sniprintf_r+0x20>
    6a82:	238b      	movs	r3, #139	; 0x8b
    6a84:	6003      	str	r3, [r0, #0]
    6a86:	2001      	movs	r0, #1
    6a88:	4240      	negs	r0, r0
    6a8a:	b01c      	add	sp, #112	; 0x70
    6a8c:	bc30      	pop	{r4, r5}
    6a8e:	bc08      	pop	{r3}
    6a90:	b001      	add	sp, #4
    6a92:	4718      	bx	r3
    6a94:	f240 2308 	movw	r3, #520	; 0x208
    6a98:	466a      	mov	r2, sp
    6a9a:	9102      	str	r1, [sp, #8]
    6a9c:	8293      	strh	r3, [r2, #20]
    6a9e:	9106      	str	r1, [sp, #24]
    6aa0:	2300      	movs	r3, #0
    6aa2:	b104      	cbz	r4, 6aa6 <_sniprintf_r+0x32>
    6aa4:	1e63      	subs	r3, r4, #1
    6aa6:	9304      	str	r3, [sp, #16]
    6aa8:	9307      	str	r3, [sp, #28]
    6aaa:	2301      	movs	r3, #1
    6aac:	466a      	mov	r2, sp
    6aae:	425b      	negs	r3, r3
    6ab0:	82d3      	strh	r3, [r2, #22]
    6ab2:	0028      	movs	r0, r5
    6ab4:	ab20      	add	r3, sp, #128	; 0x80
    6ab6:	9a1f      	ldr	r2, [sp, #124]	; 0x7c
    6ab8:	a902      	add	r1, sp, #8
    6aba:	9301      	str	r3, [sp, #4]
    6abc:	f001 f86a 	bl	7b94 <_svfiprintf_r>
    6ac0:	1c43      	adds	r3, r0, #1
    6ac2:	da01      	bge.n	6ac8 <_sniprintf_r+0x54>
    6ac4:	238b      	movs	r3, #139	; 0x8b
    6ac6:	602b      	str	r3, [r5, #0]
    6ac8:	2c00      	cmp	r4, #0
    6aca:	d0de      	beq.n	6a8a <_sniprintf_r+0x16>
    6acc:	2300      	movs	r3, #0
    6ace:	9a02      	ldr	r2, [sp, #8]
    6ad0:	7013      	strb	r3, [r2, #0]
    6ad2:	e7da      	b.n	6a8a <_sniprintf_r+0x16>

00006ad4 <sniprintf>:
    6ad4:	b40c      	push	{r2, r3}
    6ad6:	b530      	push	{r4, r5, lr}
    6ad8:	4b17      	ldr	r3, [pc, #92]	; (6b38 <sniprintf+0x64>)
    6ada:	000c      	movs	r4, r1
    6adc:	681d      	ldr	r5, [r3, #0]
    6ade:	b09d      	sub	sp, #116	; 0x74
    6ae0:	2900      	cmp	r1, #0
    6ae2:	da08      	bge.n	6af6 <sniprintf+0x22>
    6ae4:	238b      	movs	r3, #139	; 0x8b
    6ae6:	2001      	movs	r0, #1
    6ae8:	602b      	str	r3, [r5, #0]
    6aea:	4240      	negs	r0, r0
    6aec:	b01d      	add	sp, #116	; 0x74
    6aee:	bc30      	pop	{r4, r5}
    6af0:	bc08      	pop	{r3}
    6af2:	b002      	add	sp, #8
    6af4:	4718      	bx	r3
    6af6:	f240 2308 	movw	r3, #520	; 0x208
    6afa:	466a      	mov	r2, sp
    6afc:	9002      	str	r0, [sp, #8]
    6afe:	8293      	strh	r3, [r2, #20]
    6b00:	9006      	str	r0, [sp, #24]
    6b02:	2300      	movs	r3, #0
    6b04:	b101      	cbz	r1, 6b08 <sniprintf+0x34>
    6b06:	1e4b      	subs	r3, r1, #1
    6b08:	9304      	str	r3, [sp, #16]
    6b0a:	9307      	str	r3, [sp, #28]
    6b0c:	2301      	movs	r3, #1
    6b0e:	466a      	mov	r2, sp
    6b10:	425b      	negs	r3, r3
    6b12:	82d3      	strh	r3, [r2, #22]
    6b14:	0028      	movs	r0, r5
    6b16:	ab21      	add	r3, sp, #132	; 0x84
    6b18:	9a20      	ldr	r2, [sp, #128]	; 0x80
    6b1a:	a902      	add	r1, sp, #8
    6b1c:	9301      	str	r3, [sp, #4]
    6b1e:	f001 f839 	bl	7b94 <_svfiprintf_r>
    6b22:	1c43      	adds	r3, r0, #1
    6b24:	da01      	bge.n	6b2a <sniprintf+0x56>
    6b26:	238b      	movs	r3, #139	; 0x8b
    6b28:	602b      	str	r3, [r5, #0]
    6b2a:	2c00      	cmp	r4, #0
    6b2c:	d0de      	beq.n	6aec <sniprintf+0x18>
    6b2e:	2300      	movs	r3, #0
    6b30:	9a02      	ldr	r2, [sp, #8]
    6b32:	7013      	strb	r3, [r2, #0]
    6b34:	e7da      	b.n	6aec <sniprintf+0x18>
    6b36:	bf00      	nop
    6b38:	20002000 	.word	0x20002000

00006b3c <_siprintf_r>:
    6b3c:	b40c      	push	{r2, r3}
    6b3e:	b500      	push	{lr}
    6b40:	b09d      	sub	sp, #116	; 0x74
    6b42:	9102      	str	r1, [sp, #8]
    6b44:	9106      	str	r1, [sp, #24]
    6b46:	4909      	ldr	r1, [pc, #36]	; (6b6c <_siprintf_r+0x30>)
    6b48:	ab1e      	add	r3, sp, #120	; 0x78
    6b4a:	9107      	str	r1, [sp, #28]
    6b4c:	9104      	str	r1, [sp, #16]
    6b4e:	4908      	ldr	r1, [pc, #32]	; (6b70 <_siprintf_r+0x34>)
    6b50:	cb04      	ldmia	r3!, {r2}
    6b52:	9105      	str	r1, [sp, #20]
    6b54:	a902      	add	r1, sp, #8
    6b56:	9301      	str	r3, [sp, #4]
    6b58:	f001 f81c 	bl	7b94 <_svfiprintf_r>
    6b5c:	2300      	movs	r3, #0
    6b5e:	9a02      	ldr	r2, [sp, #8]
    6b60:	7013      	strb	r3, [r2, #0]
    6b62:	b01d      	add	sp, #116	; 0x74
    6b64:	bc08      	pop	{r3}
    6b66:	b002      	add	sp, #8
    6b68:	4718      	bx	r3
    6b6a:	bf00      	nop
    6b6c:	7fffffff 	.word	0x7fffffff
    6b70:	ffff0208 	.word	0xffff0208

00006b74 <siprintf>:
    6b74:	b40e      	push	{r1, r2, r3}
    6b76:	b500      	push	{lr}
    6b78:	490b      	ldr	r1, [pc, #44]	; (6ba8 <siprintf+0x34>)
    6b7a:	b09c      	sub	sp, #112	; 0x70
    6b7c:	ab1d      	add	r3, sp, #116	; 0x74
    6b7e:	9002      	str	r0, [sp, #8]
    6b80:	9006      	str	r0, [sp, #24]
    6b82:	9107      	str	r1, [sp, #28]
    6b84:	9104      	str	r1, [sp, #16]
    6b86:	4809      	ldr	r0, [pc, #36]	; (6bac <siprintf+0x38>)
    6b88:	4909      	ldr	r1, [pc, #36]	; (6bb0 <siprintf+0x3c>)
    6b8a:	cb04      	ldmia	r3!, {r2}
    6b8c:	9105      	str	r1, [sp, #20]
    6b8e:	6800      	ldr	r0, [r0, #0]
    6b90:	a902      	add	r1, sp, #8
    6b92:	9301      	str	r3, [sp, #4]
    6b94:	f000 fffe 	bl	7b94 <_svfiprintf_r>
    6b98:	2300      	movs	r3, #0
    6b9a:	9a02      	ldr	r2, [sp, #8]
    6b9c:	7013      	strb	r3, [r2, #0]
    6b9e:	b01c      	add	sp, #112	; 0x70
    6ba0:	bc08      	pop	{r3}
    6ba2:	b003      	add	sp, #12
    6ba4:	4718      	bx	r3
    6ba6:	bf00      	nop
    6ba8:	7fffffff 	.word	0x7fffffff
    6bac:	20002000 	.word	0x20002000
    6bb0:	ffff0208 	.word	0xffff0208

00006bb4 <siscanf>:
    6bb4:	b40e      	push	{r1, r2, r3}
    6bb6:	b530      	push	{r4, r5, lr}
    6bb8:	b09c      	sub	sp, #112	; 0x70
    6bba:	466a      	mov	r2, sp
    6bbc:	f240 2304 	movw	r3, #516	; 0x204
    6bc0:	ac1f      	add	r4, sp, #124	; 0x7c
    6bc2:	cc20      	ldmia	r4!, {r5}
    6bc4:	8293      	strh	r3, [r2, #20]
    6bc6:	9002      	str	r0, [sp, #8]
    6bc8:	9006      	str	r0, [sp, #24]
    6bca:	f000 f8b3 	bl	6d34 <strlen>
    6bce:	4b0b      	ldr	r3, [pc, #44]	; (6bfc <siscanf+0x48>)
    6bd0:	466a      	mov	r2, sp
    6bd2:	930b      	str	r3, [sp, #44]	; 0x2c
    6bd4:	2300      	movs	r3, #0
    6bd6:	9003      	str	r0, [sp, #12]
    6bd8:	9007      	str	r0, [sp, #28]
    6bda:	4809      	ldr	r0, [pc, #36]	; (6c00 <siscanf+0x4c>)
    6bdc:	930f      	str	r3, [sp, #60]	; 0x3c
    6bde:	9314      	str	r3, [sp, #80]	; 0x50
    6be0:	3b01      	subs	r3, #1
    6be2:	82d3      	strh	r3, [r2, #22]
    6be4:	a902      	add	r1, sp, #8
    6be6:	0023      	movs	r3, r4
    6be8:	002a      	movs	r2, r5
    6bea:	6800      	ldr	r0, [r0, #0]
    6bec:	9401      	str	r4, [sp, #4]
    6bee:	f001 f926 	bl	7e3e <__ssvfiscanf_r>
    6bf2:	b01c      	add	sp, #112	; 0x70
    6bf4:	bc30      	pop	{r4, r5}
    6bf6:	bc08      	pop	{r3}
    6bf8:	b003      	add	sp, #12
    6bfa:	4718      	bx	r3
    6bfc:	00006c7d 	.word	0x00006c7d
    6c00:	20002000 	.word	0x20002000

00006c04 <_siscanf_r>:
    6c04:	b40c      	push	{r2, r3}
    6c06:	b570      	push	{r4, r5, r6, lr}
    6c08:	b09c      	sub	sp, #112	; 0x70
    6c0a:	466a      	mov	r2, sp
    6c0c:	f240 2304 	movw	r3, #516	; 0x204
    6c10:	ac20      	add	r4, sp, #128	; 0x80
    6c12:	0005      	movs	r5, r0
    6c14:	0008      	movs	r0, r1
    6c16:	cc40      	ldmia	r4!, {r6}
    6c18:	8293      	strh	r3, [r2, #20]
    6c1a:	9102      	str	r1, [sp, #8]
    6c1c:	9106      	str	r1, [sp, #24]
    6c1e:	f000 f889 	bl	6d34 <strlen>
    6c22:	4b0b      	ldr	r3, [pc, #44]	; (6c50 <_siscanf_r+0x4c>)
    6c24:	466a      	mov	r2, sp
    6c26:	930b      	str	r3, [sp, #44]	; 0x2c
    6c28:	2300      	movs	r3, #0
    6c2a:	930f      	str	r3, [sp, #60]	; 0x3c
    6c2c:	9314      	str	r3, [sp, #80]	; 0x50
    6c2e:	3b01      	subs	r3, #1
    6c30:	9003      	str	r0, [sp, #12]
    6c32:	9007      	str	r0, [sp, #28]
    6c34:	82d3      	strh	r3, [r2, #22]
    6c36:	0028      	movs	r0, r5
    6c38:	0023      	movs	r3, r4
    6c3a:	0032      	movs	r2, r6
    6c3c:	a902      	add	r1, sp, #8
    6c3e:	9401      	str	r4, [sp, #4]
    6c40:	f001 f8fd 	bl	7e3e <__ssvfiscanf_r>
    6c44:	b01c      	add	sp, #112	; 0x70
    6c46:	bc70      	pop	{r4, r5, r6}
    6c48:	bc08      	pop	{r3}
    6c4a:	b002      	add	sp, #8
    6c4c:	4718      	bx	r3
    6c4e:	bf00      	nop
    6c50:	00006c7d 	.word	0x00006c7d

00006c54 <__sread>:
    6c54:	b570      	push	{r4, r5, r6, lr}
    6c56:	000c      	movs	r4, r1
    6c58:	250e      	movs	r5, #14
    6c5a:	5f49      	ldrsh	r1, [r1, r5]
    6c5c:	f001 feae 	bl	89bc <_read_r>
    6c60:	2800      	cmp	r0, #0
    6c62:	db03      	blt.n	6c6c <__sread+0x18>
    6c64:	6d63      	ldr	r3, [r4, #84]	; 0x54
    6c66:	181b      	adds	r3, r3, r0
    6c68:	6563      	str	r3, [r4, #84]	; 0x54
    6c6a:	bd70      	pop	{r4, r5, r6, pc}
    6c6c:	89a3      	ldrh	r3, [r4, #12]
    6c6e:	4a02      	ldr	r2, [pc, #8]	; (6c78 <__sread+0x24>)
    6c70:	4013      	ands	r3, r2
    6c72:	81a3      	strh	r3, [r4, #12]
    6c74:	e7f9      	b.n	6c6a <__sread+0x16>
    6c76:	bf00      	nop
    6c78:	ffffefff 	.word	0xffffefff

00006c7c <__seofread>:
    6c7c:	2000      	movs	r0, #0
    6c7e:	4770      	bx	lr

00006c80 <__swrite>:
    6c80:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    6c82:	001f      	movs	r7, r3
    6c84:	898b      	ldrh	r3, [r1, #12]
    6c86:	0005      	movs	r5, r0
    6c88:	000c      	movs	r4, r1
    6c8a:	0016      	movs	r6, r2
    6c8c:	05db      	lsls	r3, r3, #23
    6c8e:	d505      	bpl.n	6c9c <__swrite+0x1c>
    6c90:	230e      	movs	r3, #14
    6c92:	5ec9      	ldrsh	r1, [r1, r3]
    6c94:	2200      	movs	r2, #0
    6c96:	2302      	movs	r3, #2
    6c98:	f000 fd76 	bl	7788 <_lseek_r>
    6c9c:	89a3      	ldrh	r3, [r4, #12]
    6c9e:	4a05      	ldr	r2, [pc, #20]	; (6cb4 <__swrite+0x34>)
    6ca0:	0028      	movs	r0, r5
    6ca2:	4013      	ands	r3, r2
    6ca4:	81a3      	strh	r3, [r4, #12]
    6ca6:	0032      	movs	r2, r6
    6ca8:	230e      	movs	r3, #14
    6caa:	5ee1      	ldrsh	r1, [r4, r3]
    6cac:	003b      	movs	r3, r7
    6cae:	f000 fa51 	bl	7154 <_write_r>
    6cb2:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    6cb4:	ffffefff 	.word	0xffffefff

00006cb8 <__sseek>:
    6cb8:	b570      	push	{r4, r5, r6, lr}
    6cba:	000c      	movs	r4, r1
    6cbc:	250e      	movs	r5, #14
    6cbe:	5f49      	ldrsh	r1, [r1, r5]
    6cc0:	f000 fd62 	bl	7788 <_lseek_r>
    6cc4:	89a3      	ldrh	r3, [r4, #12]
    6cc6:	1c42      	adds	r2, r0, #1
    6cc8:	d103      	bne.n	6cd2 <__sseek+0x1a>
    6cca:	4a05      	ldr	r2, [pc, #20]	; (6ce0 <__sseek+0x28>)
    6ccc:	4013      	ands	r3, r2
    6cce:	81a3      	strh	r3, [r4, #12]
    6cd0:	bd70      	pop	{r4, r5, r6, pc}
    6cd2:	f241 0200 	movw	r2, #4096	; 0x1000
    6cd6:	4313      	orrs	r3, r2
    6cd8:	81a3      	strh	r3, [r4, #12]
    6cda:	6560      	str	r0, [r4, #84]	; 0x54
    6cdc:	e7f8      	b.n	6cd0 <__sseek+0x18>
    6cde:	bf00      	nop
    6ce0:	ffffefff 	.word	0xffffefff

00006ce4 <__sclose>:
    6ce4:	b510      	push	{r4, lr}
    6ce6:	230e      	movs	r3, #14
    6ce8:	5ec9      	ldrsh	r1, [r1, r3]
    6cea:	f000 fadd 	bl	72a8 <_close_r>
    6cee:	bd10      	pop	{r4, pc}

00006cf0 <strcat>:
    6cf0:	0002      	movs	r2, r0
    6cf2:	b510      	push	{r4, lr}
    6cf4:	7813      	ldrb	r3, [r2, #0]
    6cf6:	0014      	movs	r4, r2
    6cf8:	3201      	adds	r2, #1
    6cfa:	2b00      	cmp	r3, #0
    6cfc:	d1fa      	bne.n	6cf4 <strcat+0x4>
    6cfe:	5cca      	ldrb	r2, [r1, r3]
    6d00:	54e2      	strb	r2, [r4, r3]
    6d02:	3301      	adds	r3, #1
    6d04:	2a00      	cmp	r2, #0
    6d06:	d1fa      	bne.n	6cfe <strcat+0xe>
    6d08:	bd10      	pop	{r4, pc}

00006d0a <strchr>:
    6d0a:	b2c9      	uxtb	r1, r1
    6d0c:	7803      	ldrb	r3, [r0, #0]
    6d0e:	b123      	cbz	r3, 6d1a <strchr+0x10>
    6d10:	428b      	cmp	r3, r1
    6d12:	d100      	bne.n	6d16 <strchr+0xc>
    6d14:	4770      	bx	lr
    6d16:	3001      	adds	r0, #1
    6d18:	e7f8      	b.n	6d0c <strchr+0x2>
    6d1a:	424b      	negs	r3, r1
    6d1c:	4159      	adcs	r1, r3
    6d1e:	4249      	negs	r1, r1
    6d20:	4008      	ands	r0, r1
    6d22:	e7f7      	b.n	6d14 <strchr+0xa>

00006d24 <strcpy>:
    6d24:	0003      	movs	r3, r0
    6d26:	780a      	ldrb	r2, [r1, #0]
    6d28:	3101      	adds	r1, #1
    6d2a:	701a      	strb	r2, [r3, #0]
    6d2c:	3301      	adds	r3, #1
    6d2e:	2a00      	cmp	r2, #0
    6d30:	d1f9      	bne.n	6d26 <strcpy+0x2>
    6d32:	4770      	bx	lr

00006d34 <strlen>:
    6d34:	2300      	movs	r3, #0
    6d36:	5cc2      	ldrb	r2, [r0, r3]
    6d38:	3301      	adds	r3, #1
    6d3a:	2a00      	cmp	r2, #0
    6d3c:	d1fb      	bne.n	6d36 <strlen+0x2>
    6d3e:	1e58      	subs	r0, r3, #1
    6d40:	4770      	bx	lr

00006d42 <strncat>:
    6d42:	0003      	movs	r3, r0
    6d44:	b510      	push	{r4, lr}
    6d46:	781c      	ldrb	r4, [r3, #0]
    6d48:	b934      	cbnz	r4, 6d58 <strncat+0x16>
    6d4a:	3a01      	subs	r2, #1
    6d4c:	1c54      	adds	r4, r2, #1
    6d4e:	d002      	beq.n	6d56 <strncat+0x14>
    6d50:	780c      	ldrb	r4, [r1, #0]
    6d52:	701c      	strb	r4, [r3, #0]
    6d54:	b914      	cbnz	r4, 6d5c <strncat+0x1a>
    6d56:	bd10      	pop	{r4, pc}
    6d58:	3301      	adds	r3, #1
    6d5a:	e7f4      	b.n	6d46 <strncat+0x4>
    6d5c:	b902      	cbnz	r2, 6d60 <strncat+0x1e>
    6d5e:	705a      	strb	r2, [r3, #1]
    6d60:	3301      	adds	r3, #1
    6d62:	3101      	adds	r1, #1
    6d64:	e7f1      	b.n	6d4a <strncat+0x8>

00006d66 <strncmp>:
    6d66:	b530      	push	{r4, r5, lr}
    6d68:	0005      	movs	r5, r0
    6d6a:	0010      	movs	r0, r2
    6d6c:	b142      	cbz	r2, 6d80 <strncmp+0x1a>
    6d6e:	2400      	movs	r4, #0
    6d70:	3a01      	subs	r2, #1
    6d72:	5d2b      	ldrb	r3, [r5, r4]
    6d74:	5d08      	ldrb	r0, [r1, r4]
    6d76:	4283      	cmp	r3, r0
    6d78:	d101      	bne.n	6d7e <strncmp+0x18>
    6d7a:	4294      	cmp	r4, r2
    6d7c:	d101      	bne.n	6d82 <strncmp+0x1c>
    6d7e:	1a18      	subs	r0, r3, r0
    6d80:	bd30      	pop	{r4, r5, pc}
    6d82:	3401      	adds	r4, #1
    6d84:	2b00      	cmp	r3, #0
    6d86:	d1f4      	bne.n	6d72 <strncmp+0xc>
    6d88:	e7f9      	b.n	6d7e <strncmp+0x18>

00006d8a <strncpy>:
    6d8a:	0003      	movs	r3, r0
    6d8c:	b530      	push	{r4, r5, lr}
    6d8e:	001d      	movs	r5, r3
    6d90:	b132      	cbz	r2, 6da0 <strncpy+0x16>
    6d92:	780c      	ldrb	r4, [r1, #0]
    6d94:	3a01      	subs	r2, #1
    6d96:	3301      	adds	r3, #1
    6d98:	702c      	strb	r4, [r5, #0]
    6d9a:	3101      	adds	r1, #1
    6d9c:	2c00      	cmp	r4, #0
    6d9e:	d1f6      	bne.n	6d8e <strncpy+0x4>
    6da0:	2100      	movs	r1, #0
    6da2:	189a      	adds	r2, r3, r2
    6da4:	4293      	cmp	r3, r2
    6da6:	d100      	bne.n	6daa <strncpy+0x20>
    6da8:	bd30      	pop	{r4, r5, pc}
    6daa:	7019      	strb	r1, [r3, #0]
    6dac:	3301      	adds	r3, #1
    6dae:	e7f9      	b.n	6da4 <strncpy+0x1a>

00006db0 <strpbrk>:
    6db0:	b510      	push	{r4, lr}
    6db2:	7803      	ldrb	r3, [r0, #0]
    6db4:	b193      	cbz	r3, 6ddc <strpbrk+0x2c>
    6db6:	000b      	movs	r3, r1
    6db8:	7802      	ldrb	r2, [r0, #0]
    6dba:	b96a      	cbnz	r2, 6dd8 <strpbrk+0x28>
    6dbc:	781b      	ldrb	r3, [r3, #0]
    6dbe:	1e5a      	subs	r2, r3, #1
    6dc0:	4193      	sbcs	r3, r2
    6dc2:	425b      	negs	r3, r3
    6dc4:	4018      	ands	r0, r3
    6dc6:	bd10      	pop	{r4, pc}
    6dc8:	4294      	cmp	r4, r2
    6dca:	d0fc      	beq.n	6dc6 <strpbrk+0x16>
    6dcc:	3301      	adds	r3, #1
    6dce:	781c      	ldrb	r4, [r3, #0]
    6dd0:	2c00      	cmp	r4, #0
    6dd2:	d1f9      	bne.n	6dc8 <strpbrk+0x18>
    6dd4:	3001      	adds	r0, #1
    6dd6:	e7ef      	b.n	6db8 <strpbrk+0x8>
    6dd8:	000b      	movs	r3, r1
    6dda:	e7f8      	b.n	6dce <strpbrk+0x1e>
    6ddc:	0018      	movs	r0, r3
    6dde:	e7f2      	b.n	6dc6 <strpbrk+0x16>

00006de0 <strsep>:
    6de0:	b510      	push	{r4, lr}
    6de2:	0002      	movs	r2, r0
    6de4:	2300      	movs	r3, #0
    6de6:	6800      	ldr	r0, [r0, #0]
    6de8:	f000 f848 	bl	6e7c <__strtok_r>
    6dec:	bd10      	pop	{r4, pc}

00006dee <strstr>:
    6dee:	780a      	ldrb	r2, [r1, #0]
    6df0:	b530      	push	{r4, r5, lr}
    6df2:	b962      	cbnz	r2, 6e0e <strstr+0x20>
    6df4:	bd30      	pop	{r4, r5, pc}
    6df6:	429a      	cmp	r2, r3
    6df8:	d108      	bne.n	6e0c <strstr+0x1e>
    6dfa:	2301      	movs	r3, #1
    6dfc:	5ccc      	ldrb	r4, [r1, r3]
    6dfe:	2c00      	cmp	r4, #0
    6e00:	d0f8      	beq.n	6df4 <strstr+0x6>
    6e02:	5cc5      	ldrb	r5, [r0, r3]
    6e04:	42a5      	cmp	r5, r4
    6e06:	d101      	bne.n	6e0c <strstr+0x1e>
    6e08:	3301      	adds	r3, #1
    6e0a:	e7f7      	b.n	6dfc <strstr+0xe>
    6e0c:	3001      	adds	r0, #1
    6e0e:	7803      	ldrb	r3, [r0, #0]
    6e10:	2b00      	cmp	r3, #0
    6e12:	d1f0      	bne.n	6df6 <strstr+0x8>
    6e14:	0018      	movs	r0, r3
    6e16:	e7ed      	b.n	6df4 <strstr+0x6>

00006e18 <strtok>:
    6e18:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    6e1a:	4b15      	ldr	r3, [pc, #84]	; (6e70 <strtok+0x58>)
    6e1c:	0005      	movs	r5, r0
    6e1e:	681f      	ldr	r7, [r3, #0]
    6e20:	000e      	movs	r6, r1
    6e22:	6dbc      	ldr	r4, [r7, #88]	; 0x58
    6e24:	b9ec      	cbnz	r4, 6e62 <strtok+0x4a>
    6e26:	2050      	movs	r0, #80	; 0x50
    6e28:	f000 fd28 	bl	787c <malloc>
    6e2c:	0002      	movs	r2, r0
    6e2e:	65b8      	str	r0, [r7, #88]	; 0x58
    6e30:	b920      	cbnz	r0, 6e3c <strtok+0x24>
    6e32:	2157      	movs	r1, #87	; 0x57
    6e34:	4b0f      	ldr	r3, [pc, #60]	; (6e74 <strtok+0x5c>)
    6e36:	4810      	ldr	r0, [pc, #64]	; (6e78 <strtok+0x60>)
    6e38:	f000 fa12 	bl	7260 <__assert_func>
    6e3c:	6004      	str	r4, [r0, #0]
    6e3e:	6044      	str	r4, [r0, #4]
    6e40:	6084      	str	r4, [r0, #8]
    6e42:	60c4      	str	r4, [r0, #12]
    6e44:	6104      	str	r4, [r0, #16]
    6e46:	6144      	str	r4, [r0, #20]
    6e48:	6184      	str	r4, [r0, #24]
    6e4a:	6284      	str	r4, [r0, #40]	; 0x28
    6e4c:	62c4      	str	r4, [r0, #44]	; 0x2c
    6e4e:	6304      	str	r4, [r0, #48]	; 0x30
    6e50:	6344      	str	r4, [r0, #52]	; 0x34
    6e52:	6384      	str	r4, [r0, #56]	; 0x38
    6e54:	63c4      	str	r4, [r0, #60]	; 0x3c
    6e56:	6404      	str	r4, [r0, #64]	; 0x40
    6e58:	6444      	str	r4, [r0, #68]	; 0x44
    6e5a:	6484      	str	r4, [r0, #72]	; 0x48
    6e5c:	64c4      	str	r4, [r0, #76]	; 0x4c
    6e5e:	7704      	strb	r4, [r0, #28]
    6e60:	6244      	str	r4, [r0, #36]	; 0x24
    6e62:	0031      	movs	r1, r6
    6e64:	0028      	movs	r0, r5
    6e66:	6dba      	ldr	r2, [r7, #88]	; 0x58
    6e68:	2301      	movs	r3, #1
    6e6a:	f000 f807 	bl	6e7c <__strtok_r>
    6e6e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    6e70:	20002000 	.word	0x20002000
    6e74:	00009553 	.word	0x00009553
    6e78:	0000956a 	.word	0x0000956a

00006e7c <__strtok_r>:
    6e7c:	b5f0      	push	{r4, r5, r6, r7, lr}
    6e7e:	b908      	cbnz	r0, 6e84 <__strtok_r+0x8>
    6e80:	6810      	ldr	r0, [r2, #0]
    6e82:	b188      	cbz	r0, 6ea8 <__strtok_r+0x2c>
    6e84:	0004      	movs	r4, r0
    6e86:	0020      	movs	r0, r4
    6e88:	000e      	movs	r6, r1
    6e8a:	7805      	ldrb	r5, [r0, #0]
    6e8c:	3401      	adds	r4, #1
    6e8e:	7837      	ldrb	r7, [r6, #0]
    6e90:	b91f      	cbnz	r7, 6e9a <__strtok_r+0x1e>
    6e92:	b975      	cbnz	r5, 6eb2 <__strtok_r+0x36>
    6e94:	0028      	movs	r0, r5
    6e96:	6015      	str	r5, [r2, #0]
    6e98:	e006      	b.n	6ea8 <__strtok_r+0x2c>
    6e9a:	3601      	adds	r6, #1
    6e9c:	42bd      	cmp	r5, r7
    6e9e:	d1f6      	bne.n	6e8e <__strtok_r+0x12>
    6ea0:	2b00      	cmp	r3, #0
    6ea2:	d1f0      	bne.n	6e86 <__strtok_r+0xa>
    6ea4:	6014      	str	r4, [r2, #0]
    6ea6:	7003      	strb	r3, [r0, #0]
    6ea8:	bdf0      	pop	{r4, r5, r6, r7, pc}
    6eaa:	002f      	movs	r7, r5
    6eac:	e00e      	b.n	6ecc <__strtok_r+0x50>
    6eae:	3301      	adds	r3, #1
    6eb0:	b926      	cbnz	r6, 6ebc <__strtok_r+0x40>
    6eb2:	0023      	movs	r3, r4
    6eb4:	3401      	adds	r4, #1
    6eb6:	781d      	ldrb	r5, [r3, #0]
    6eb8:	0027      	movs	r7, r4
    6eba:	000b      	movs	r3, r1
    6ebc:	781e      	ldrb	r6, [r3, #0]
    6ebe:	42b5      	cmp	r5, r6
    6ec0:	d1f5      	bne.n	6eae <__strtok_r+0x32>
    6ec2:	2d00      	cmp	r5, #0
    6ec4:	d0f1      	beq.n	6eaa <__strtok_r+0x2e>
    6ec6:	2300      	movs	r3, #0
    6ec8:	3c01      	subs	r4, #1
    6eca:	7023      	strb	r3, [r4, #0]
    6ecc:	6017      	str	r7, [r2, #0]
    6ece:	e7eb      	b.n	6ea8 <__strtok_r+0x2c>

00006ed0 <strtok_r>:
    6ed0:	b510      	push	{r4, lr}
    6ed2:	2301      	movs	r3, #1
    6ed4:	f7ff ffd2 	bl	6e7c <__strtok_r>
    6ed8:	bd10      	pop	{r4, pc}
	...

00006edc <_strtol_l.constprop.0>:
    6edc:	b5f0      	push	{r4, r5, r6, r7, lr}
    6ede:	b085      	sub	sp, #20
    6ee0:	9003      	str	r0, [sp, #12]
    6ee2:	9100      	str	r1, [sp, #0]
    6ee4:	9201      	str	r2, [sp, #4]
    6ee6:	2b01      	cmp	r3, #1
    6ee8:	d044      	beq.n	6f74 <_strtol_l.constprop.0+0x98>
    6eea:	0008      	movs	r0, r1
    6eec:	2b24      	cmp	r3, #36	; 0x24
    6eee:	d841      	bhi.n	6f74 <_strtol_l.constprop.0+0x98>
    6ef0:	4a3f      	ldr	r2, [pc, #252]	; (6ff0 <_strtol_l.constprop.0+0x114>)
    6ef2:	2508      	movs	r5, #8
    6ef4:	4694      	mov	ip, r2
    6ef6:	0004      	movs	r4, r0
    6ef8:	4666      	mov	r6, ip
    6efa:	7822      	ldrb	r2, [r4, #0]
    6efc:	3001      	adds	r0, #1
    6efe:	5cb6      	ldrb	r6, [r6, r2]
    6f00:	0001      	movs	r1, r0
    6f02:	0037      	movs	r7, r6
    6f04:	402f      	ands	r7, r5
    6f06:	422e      	tst	r6, r5
    6f08:	d1f5      	bne.n	6ef6 <_strtol_l.constprop.0+0x1a>
    6f0a:	2a2d      	cmp	r2, #45	; 0x2d
    6f0c:	d139      	bne.n	6f82 <_strtol_l.constprop.0+0xa6>
    6f0e:	f240 0c01 	movw	ip, #1
    6f12:	7802      	ldrb	r2, [r0, #0]
    6f14:	1ca1      	adds	r1, r4, #2
    6f16:	2b00      	cmp	r3, #0
    6f18:	d065      	beq.n	6fe6 <_strtol_l.constprop.0+0x10a>
    6f1a:	2b10      	cmp	r3, #16
    6f1c:	d109      	bne.n	6f32 <_strtol_l.constprop.0+0x56>
    6f1e:	2a30      	cmp	r2, #48	; 0x30
    6f20:	d107      	bne.n	6f32 <_strtol_l.constprop.0+0x56>
    6f22:	2020      	movs	r0, #32
    6f24:	780a      	ldrb	r2, [r1, #0]
    6f26:	4382      	bics	r2, r0
    6f28:	2a58      	cmp	r2, #88	; 0x58
    6f2a:	d157      	bne.n	6fdc <_strtol_l.constprop.0+0x100>
    6f2c:	2310      	movs	r3, #16
    6f2e:	784a      	ldrb	r2, [r1, #1]
    6f30:	3102      	adds	r1, #2
    6f32:	4e30      	ldr	r6, [pc, #192]	; (6ff4 <_strtol_l.constprop.0+0x118>)
    6f34:	0018      	movs	r0, r3
    6f36:	4466      	add	r6, ip
    6f38:	fbb6 f7f3 	udiv	r7, r6, r3
    6f3c:	2400      	movs	r4, #0
    6f3e:	4378      	muls	r0, r7
    6f40:	1a30      	subs	r0, r6, r0
    6f42:	9002      	str	r0, [sp, #8]
    6f44:	0020      	movs	r0, r4
    6f46:	0015      	movs	r5, r2
    6f48:	3d30      	subs	r5, #48	; 0x30
    6f4a:	2d09      	cmp	r5, #9
    6f4c:	d81f      	bhi.n	6f8e <_strtol_l.constprop.0+0xb2>
    6f4e:	002a      	movs	r2, r5
    6f50:	4293      	cmp	r3, r2
    6f52:	dd2b      	ble.n	6fac <_strtol_l.constprop.0+0xd0>
    6f54:	2c00      	cmp	r4, #0
    6f56:	db26      	blt.n	6fa6 <_strtol_l.constprop.0+0xca>
    6f58:	2401      	movs	r4, #1
    6f5a:	4264      	negs	r4, r4
    6f5c:	4287      	cmp	r7, r0
    6f5e:	d306      	bcc.n	6f6e <_strtol_l.constprop.0+0x92>
    6f60:	d102      	bne.n	6f68 <_strtol_l.constprop.0+0x8c>
    6f62:	9d02      	ldr	r5, [sp, #8]
    6f64:	4295      	cmp	r5, r2
    6f66:	db02      	blt.n	6f6e <_strtol_l.constprop.0+0x92>
    6f68:	2401      	movs	r4, #1
    6f6a:	4358      	muls	r0, r3
    6f6c:	1810      	adds	r0, r2, r0
    6f6e:	780a      	ldrb	r2, [r1, #0]
    6f70:	3101      	adds	r1, #1
    6f72:	e7e8      	b.n	6f46 <_strtol_l.constprop.0+0x6a>
    6f74:	f000 f9a8 	bl	72c8 <__errno>
    6f78:	2316      	movs	r3, #22
    6f7a:	6003      	str	r3, [r0, #0]
    6f7c:	2000      	movs	r0, #0
    6f7e:	b005      	add	sp, #20
    6f80:	bdf0      	pop	{r4, r5, r6, r7, pc}
    6f82:	46bc      	mov	ip, r7
    6f84:	2a2b      	cmp	r2, #43	; 0x2b
    6f86:	d1c6      	bne.n	6f16 <_strtol_l.constprop.0+0x3a>
    6f88:	7802      	ldrb	r2, [r0, #0]
    6f8a:	1ca1      	adds	r1, r4, #2
    6f8c:	e7c3      	b.n	6f16 <_strtol_l.constprop.0+0x3a>
    6f8e:	0015      	movs	r5, r2
    6f90:	3d41      	subs	r5, #65	; 0x41
    6f92:	2d19      	cmp	r5, #25
    6f94:	d801      	bhi.n	6f9a <_strtol_l.constprop.0+0xbe>
    6f96:	3a37      	subs	r2, #55	; 0x37
    6f98:	e7da      	b.n	6f50 <_strtol_l.constprop.0+0x74>
    6f9a:	0015      	movs	r5, r2
    6f9c:	3d61      	subs	r5, #97	; 0x61
    6f9e:	2d19      	cmp	r5, #25
    6fa0:	d804      	bhi.n	6fac <_strtol_l.constprop.0+0xd0>
    6fa2:	3a57      	subs	r2, #87	; 0x57
    6fa4:	e7d4      	b.n	6f50 <_strtol_l.constprop.0+0x74>
    6fa6:	2401      	movs	r4, #1
    6fa8:	4264      	negs	r4, r4
    6faa:	e7e0      	b.n	6f6e <_strtol_l.constprop.0+0x92>
    6fac:	2c00      	cmp	r4, #0
    6fae:	da09      	bge.n	6fc4 <_strtol_l.constprop.0+0xe8>
    6fb0:	2322      	movs	r3, #34	; 0x22
    6fb2:	9a03      	ldr	r2, [sp, #12]
    6fb4:	0030      	movs	r0, r6
    6fb6:	6013      	str	r3, [r2, #0]
    6fb8:	9b01      	ldr	r3, [sp, #4]
    6fba:	2b00      	cmp	r3, #0
    6fbc:	d0df      	beq.n	6f7e <_strtol_l.constprop.0+0xa2>
    6fbe:	1e4b      	subs	r3, r1, #1
    6fc0:	9300      	str	r3, [sp, #0]
    6fc2:	e007      	b.n	6fd4 <_strtol_l.constprop.0+0xf8>
    6fc4:	4663      	mov	r3, ip
    6fc6:	b103      	cbz	r3, 6fca <_strtol_l.constprop.0+0xee>
    6fc8:	4240      	negs	r0, r0
    6fca:	9b01      	ldr	r3, [sp, #4]
    6fcc:	2b00      	cmp	r3, #0
    6fce:	d0d6      	beq.n	6f7e <_strtol_l.constprop.0+0xa2>
    6fd0:	2c00      	cmp	r4, #0
    6fd2:	d1f4      	bne.n	6fbe <_strtol_l.constprop.0+0xe2>
    6fd4:	9b01      	ldr	r3, [sp, #4]
    6fd6:	9a00      	ldr	r2, [sp, #0]
    6fd8:	601a      	str	r2, [r3, #0]
    6fda:	e7d0      	b.n	6f7e <_strtol_l.constprop.0+0xa2>
    6fdc:	2230      	movs	r2, #48	; 0x30
    6fde:	2b00      	cmp	r3, #0
    6fe0:	d1a7      	bne.n	6f32 <_strtol_l.constprop.0+0x56>
    6fe2:	3308      	adds	r3, #8
    6fe4:	e7a5      	b.n	6f32 <_strtol_l.constprop.0+0x56>
    6fe6:	2a30      	cmp	r2, #48	; 0x30
    6fe8:	d09b      	beq.n	6f22 <_strtol_l.constprop.0+0x46>
    6fea:	230a      	movs	r3, #10
    6fec:	e7a1      	b.n	6f32 <_strtol_l.constprop.0+0x56>
    6fee:	bf00      	nop
    6ff0:	0000961d 	.word	0x0000961d
    6ff4:	7fffffff 	.word	0x7fffffff

00006ff8 <_strtol_r>:
    6ff8:	b510      	push	{r4, lr}
    6ffa:	f7ff ff6f 	bl	6edc <_strtol_l.constprop.0>
    6ffe:	bd10      	pop	{r4, pc}

00007000 <strtol_l>:
    7000:	b510      	push	{r4, lr}
    7002:	0013      	movs	r3, r2
    7004:	000a      	movs	r2, r1
    7006:	0001      	movs	r1, r0
    7008:	4802      	ldr	r0, [pc, #8]	; (7014 <strtol_l+0x14>)
    700a:	6800      	ldr	r0, [r0, #0]
    700c:	f7ff ff66 	bl	6edc <_strtol_l.constprop.0>
    7010:	bd10      	pop	{r4, pc}
    7012:	bf00      	nop
    7014:	20002000 	.word	0x20002000

00007018 <strtol>:
    7018:	b510      	push	{r4, lr}
    701a:	0013      	movs	r3, r2
    701c:	000a      	movs	r2, r1
    701e:	0001      	movs	r1, r0
    7020:	4802      	ldr	r0, [pc, #8]	; (702c <strtol+0x14>)
    7022:	6800      	ldr	r0, [r0, #0]
    7024:	f7ff ff5a 	bl	6edc <_strtol_l.constprop.0>
    7028:	bd10      	pop	{r4, pc}
    702a:	bf00      	nop
    702c:	20002000 	.word	0x20002000

00007030 <_vsniprintf_r>:
    7030:	b530      	push	{r4, r5, lr}
    7032:	0014      	movs	r4, r2
    7034:	0005      	movs	r5, r0
    7036:	001a      	movs	r2, r3
    7038:	b09b      	sub	sp, #108	; 0x6c
    703a:	2c00      	cmp	r4, #0
    703c:	da05      	bge.n	704a <_vsniprintf_r+0x1a>
    703e:	238b      	movs	r3, #139	; 0x8b
    7040:	6003      	str	r3, [r0, #0]
    7042:	2001      	movs	r0, #1
    7044:	4240      	negs	r0, r0
    7046:	b01b      	add	sp, #108	; 0x6c
    7048:	bd30      	pop	{r4, r5, pc}
    704a:	f240 2308 	movw	r3, #520	; 0x208
    704e:	4668      	mov	r0, sp
    7050:	9100      	str	r1, [sp, #0]
    7052:	8183      	strh	r3, [r0, #12]
    7054:	9104      	str	r1, [sp, #16]
    7056:	2300      	movs	r3, #0
    7058:	b104      	cbz	r4, 705c <_vsniprintf_r+0x2c>
    705a:	1e63      	subs	r3, r4, #1
    705c:	9302      	str	r3, [sp, #8]
    705e:	9305      	str	r3, [sp, #20]
    7060:	2301      	movs	r3, #1
    7062:	4669      	mov	r1, sp
    7064:	425b      	negs	r3, r3
    7066:	81cb      	strh	r3, [r1, #14]
    7068:	0028      	movs	r0, r5
    706a:	9b1e      	ldr	r3, [sp, #120]	; 0x78
    706c:	f000 fd92 	bl	7b94 <_svfiprintf_r>
    7070:	1c43      	adds	r3, r0, #1
    7072:	da01      	bge.n	7078 <_vsniprintf_r+0x48>
    7074:	238b      	movs	r3, #139	; 0x8b
    7076:	602b      	str	r3, [r5, #0]
    7078:	2c00      	cmp	r4, #0
    707a:	d0e4      	beq.n	7046 <_vsniprintf_r+0x16>
    707c:	2300      	movs	r3, #0
    707e:	9a00      	ldr	r2, [sp, #0]
    7080:	7013      	strb	r3, [r2, #0]
    7082:	e7e0      	b.n	7046 <_vsniprintf_r+0x16>

00007084 <vsniprintf>:
    7084:	b507      	push	{r0, r1, r2, lr}
    7086:	9300      	str	r3, [sp, #0]
    7088:	0013      	movs	r3, r2
    708a:	000a      	movs	r2, r1
    708c:	0001      	movs	r1, r0
    708e:	4802      	ldr	r0, [pc, #8]	; (7098 <vsniprintf+0x14>)
    7090:	6800      	ldr	r0, [r0, #0]
    7092:	f7ff ffcd 	bl	7030 <_vsniprintf_r>
    7096:	bd0e      	pop	{r1, r2, r3, pc}
    7098:	20002000 	.word	0x20002000

0000709c <__swbuf_r>:
    709c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    709e:	0005      	movs	r5, r0
    70a0:	000e      	movs	r6, r1
    70a2:	0014      	movs	r4, r2
    70a4:	b118      	cbz	r0, 70ae <__swbuf_r+0x12>
    70a6:	6983      	ldr	r3, [r0, #24]
    70a8:	b90b      	cbnz	r3, 70ae <__swbuf_r+0x12>
    70aa:	f000 fa6f 	bl	758c <__sinit>
    70ae:	4b21      	ldr	r3, [pc, #132]	; (7134 <__swbuf_r+0x98>)
    70b0:	429c      	cmp	r4, r3
    70b2:	d12c      	bne.n	710e <__swbuf_r+0x72>
    70b4:	686c      	ldr	r4, [r5, #4]
    70b6:	69a3      	ldr	r3, [r4, #24]
    70b8:	60a3      	str	r3, [r4, #8]
    70ba:	89a3      	ldrh	r3, [r4, #12]
    70bc:	071b      	lsls	r3, r3, #28
    70be:	d530      	bpl.n	7122 <__swbuf_r+0x86>
    70c0:	6923      	ldr	r3, [r4, #16]
    70c2:	2b00      	cmp	r3, #0
    70c4:	d02d      	beq.n	7122 <__swbuf_r+0x86>
    70c6:	6823      	ldr	r3, [r4, #0]
    70c8:	6922      	ldr	r2, [r4, #16]
    70ca:	b2f7      	uxtb	r7, r6
    70cc:	1a98      	subs	r0, r3, r2
    70ce:	6963      	ldr	r3, [r4, #20]
    70d0:	b2f6      	uxtb	r6, r6
    70d2:	4283      	cmp	r3, r0
    70d4:	dc04      	bgt.n	70e0 <__swbuf_r+0x44>
    70d6:	0021      	movs	r1, r4
    70d8:	0028      	movs	r0, r5
    70da:	f000 f985 	bl	73e8 <_fflush_r>
    70de:	bb30      	cbnz	r0, 712e <__swbuf_r+0x92>
    70e0:	68a3      	ldr	r3, [r4, #8]
    70e2:	3001      	adds	r0, #1
    70e4:	3b01      	subs	r3, #1
    70e6:	60a3      	str	r3, [r4, #8]
    70e8:	6823      	ldr	r3, [r4, #0]
    70ea:	1c5a      	adds	r2, r3, #1
    70ec:	6022      	str	r2, [r4, #0]
    70ee:	701f      	strb	r7, [r3, #0]
    70f0:	6963      	ldr	r3, [r4, #20]
    70f2:	4283      	cmp	r3, r0
    70f4:	d004      	beq.n	7100 <__swbuf_r+0x64>
    70f6:	89a3      	ldrh	r3, [r4, #12]
    70f8:	07db      	lsls	r3, r3, #31
    70fa:	d506      	bpl.n	710a <__swbuf_r+0x6e>
    70fc:	2e0a      	cmp	r6, #10
    70fe:	d104      	bne.n	710a <__swbuf_r+0x6e>
    7100:	0021      	movs	r1, r4
    7102:	0028      	movs	r0, r5
    7104:	f000 f970 	bl	73e8 <_fflush_r>
    7108:	b988      	cbnz	r0, 712e <__swbuf_r+0x92>
    710a:	0030      	movs	r0, r6
    710c:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    710e:	4b0a      	ldr	r3, [pc, #40]	; (7138 <__swbuf_r+0x9c>)
    7110:	429c      	cmp	r4, r3
    7112:	d101      	bne.n	7118 <__swbuf_r+0x7c>
    7114:	68ac      	ldr	r4, [r5, #8]
    7116:	e7ce      	b.n	70b6 <__swbuf_r+0x1a>
    7118:	4b08      	ldr	r3, [pc, #32]	; (713c <__swbuf_r+0xa0>)
    711a:	429c      	cmp	r4, r3
    711c:	d1cb      	bne.n	70b6 <__swbuf_r+0x1a>
    711e:	68ec      	ldr	r4, [r5, #12]
    7120:	e7c9      	b.n	70b6 <__swbuf_r+0x1a>
    7122:	0021      	movs	r1, r4
    7124:	0028      	movs	r0, r5
    7126:	f000 f827 	bl	7178 <__swsetup_r>
    712a:	2800      	cmp	r0, #0
    712c:	d0cb      	beq.n	70c6 <__swbuf_r+0x2a>
    712e:	2601      	movs	r6, #1
    7130:	4276      	negs	r6, r6
    7132:	e7ea      	b.n	710a <__swbuf_r+0x6e>
    7134:	00009760 	.word	0x00009760
    7138:	00009740 	.word	0x00009740
    713c:	00009720 	.word	0x00009720

00007140 <__swbuf>:
    7140:	b510      	push	{r4, lr}
    7142:	4b03      	ldr	r3, [pc, #12]	; (7150 <__swbuf+0x10>)
    7144:	000a      	movs	r2, r1
    7146:	0001      	movs	r1, r0
    7148:	6818      	ldr	r0, [r3, #0]
    714a:	f7ff ffa7 	bl	709c <__swbuf_r>
    714e:	bd10      	pop	{r4, pc}
    7150:	20002000 	.word	0x20002000

00007154 <_write_r>:
    7154:	b570      	push	{r4, r5, r6, lr}
    7156:	0004      	movs	r4, r0
    7158:	0008      	movs	r0, r1
    715a:	0011      	movs	r1, r2
    715c:	001a      	movs	r2, r3
    715e:	2300      	movs	r3, #0
    7160:	4d04      	ldr	r5, [pc, #16]	; (7174 <_write_r+0x20>)
    7162:	602b      	str	r3, [r5, #0]
    7164:	f7ff f960 	bl	6428 <_write>
    7168:	1c43      	adds	r3, r0, #1
    716a:	d102      	bne.n	7172 <_write_r+0x1e>
    716c:	682b      	ldr	r3, [r5, #0]
    716e:	b103      	cbz	r3, 7172 <_write_r+0x1e>
    7170:	6023      	str	r3, [r4, #0]
    7172:	bd70      	pop	{r4, r5, r6, pc}
    7174:	2000209c 	.word	0x2000209c

00007178 <__swsetup_r>:
    7178:	4b35      	ldr	r3, [pc, #212]	; (7250 <__swsetup_r+0xd8>)
    717a:	b570      	push	{r4, r5, r6, lr}
    717c:	681d      	ldr	r5, [r3, #0]
    717e:	0006      	movs	r6, r0
    7180:	000c      	movs	r4, r1
    7182:	b125      	cbz	r5, 718e <__swsetup_r+0x16>
    7184:	69ab      	ldr	r3, [r5, #24]
    7186:	b913      	cbnz	r3, 718e <__swsetup_r+0x16>
    7188:	0028      	movs	r0, r5
    718a:	f000 f9ff 	bl	758c <__sinit>
    718e:	4b31      	ldr	r3, [pc, #196]	; (7254 <__swsetup_r+0xdc>)
    7190:	429c      	cmp	r4, r3
    7192:	d10f      	bne.n	71b4 <__swsetup_r+0x3c>
    7194:	686c      	ldr	r4, [r5, #4]
    7196:	230c      	movs	r3, #12
    7198:	5ee2      	ldrsh	r2, [r4, r3]
    719a:	b293      	uxth	r3, r2
    719c:	0711      	lsls	r1, r2, #28
    719e:	d42c      	bmi.n	71fa <__swsetup_r+0x82>
    71a0:	06d9      	lsls	r1, r3, #27
    71a2:	d411      	bmi.n	71c8 <__swsetup_r+0x50>
    71a4:	2309      	movs	r3, #9
    71a6:	2001      	movs	r0, #1
    71a8:	6033      	str	r3, [r6, #0]
    71aa:	3337      	adds	r3, #55	; 0x37
    71ac:	4313      	orrs	r3, r2
    71ae:	81a3      	strh	r3, [r4, #12]
    71b0:	4240      	negs	r0, r0
    71b2:	bd70      	pop	{r4, r5, r6, pc}
    71b4:	4b28      	ldr	r3, [pc, #160]	; (7258 <__swsetup_r+0xe0>)
    71b6:	429c      	cmp	r4, r3
    71b8:	d101      	bne.n	71be <__swsetup_r+0x46>
    71ba:	68ac      	ldr	r4, [r5, #8]
    71bc:	e7eb      	b.n	7196 <__swsetup_r+0x1e>
    71be:	4b27      	ldr	r3, [pc, #156]	; (725c <__swsetup_r+0xe4>)
    71c0:	429c      	cmp	r4, r3
    71c2:	d1e8      	bne.n	7196 <__swsetup_r+0x1e>
    71c4:	68ec      	ldr	r4, [r5, #12]
    71c6:	e7e6      	b.n	7196 <__swsetup_r+0x1e>
    71c8:	075b      	lsls	r3, r3, #29
    71ca:	d512      	bpl.n	71f2 <__swsetup_r+0x7a>
    71cc:	6b61      	ldr	r1, [r4, #52]	; 0x34
    71ce:	b141      	cbz	r1, 71e2 <__swsetup_r+0x6a>
    71d0:	0023      	movs	r3, r4
    71d2:	3344      	adds	r3, #68	; 0x44
    71d4:	4299      	cmp	r1, r3
    71d6:	d002      	beq.n	71de <__swsetup_r+0x66>
    71d8:	0030      	movs	r0, r6
    71da:	f000 fb59 	bl	7890 <_free_r>
    71de:	2300      	movs	r3, #0
    71e0:	6363      	str	r3, [r4, #52]	; 0x34
    71e2:	2224      	movs	r2, #36	; 0x24
    71e4:	89a3      	ldrh	r3, [r4, #12]
    71e6:	4393      	bics	r3, r2
    71e8:	81a3      	strh	r3, [r4, #12]
    71ea:	2300      	movs	r3, #0
    71ec:	6063      	str	r3, [r4, #4]
    71ee:	6923      	ldr	r3, [r4, #16]
    71f0:	6023      	str	r3, [r4, #0]
    71f2:	2308      	movs	r3, #8
    71f4:	89a2      	ldrh	r2, [r4, #12]
    71f6:	4313      	orrs	r3, r2
    71f8:	81a3      	strh	r3, [r4, #12]
    71fa:	6923      	ldr	r3, [r4, #16]
    71fc:	b95b      	cbnz	r3, 7216 <__swsetup_r+0x9e>
    71fe:	f240 2180 	movw	r1, #640	; 0x280
    7202:	f240 2200 	movw	r2, #512	; 0x200
    7206:	89a3      	ldrh	r3, [r4, #12]
    7208:	400b      	ands	r3, r1
    720a:	4293      	cmp	r3, r2
    720c:	d003      	beq.n	7216 <__swsetup_r+0x9e>
    720e:	0021      	movs	r1, r4
    7210:	0030      	movs	r0, r6
    7212:	f000 faf3 	bl	77fc <__smakebuf_r>
    7216:	220c      	movs	r2, #12
    7218:	5ea3      	ldrsh	r3, [r4, r2]
    721a:	2001      	movs	r0, #1
    721c:	001a      	movs	r2, r3
    721e:	b299      	uxth	r1, r3
    7220:	4002      	ands	r2, r0
    7222:	4203      	tst	r3, r0
    7224:	d00f      	beq.n	7246 <__swsetup_r+0xce>
    7226:	2200      	movs	r2, #0
    7228:	60a2      	str	r2, [r4, #8]
    722a:	6962      	ldr	r2, [r4, #20]
    722c:	4252      	negs	r2, r2
    722e:	61a2      	str	r2, [r4, #24]
    7230:	6922      	ldr	r2, [r4, #16]
    7232:	2000      	movs	r0, #0
    7234:	2a00      	cmp	r2, #0
    7236:	d1bc      	bne.n	71b2 <__swsetup_r+0x3a>
    7238:	060a      	lsls	r2, r1, #24
    723a:	d5ba      	bpl.n	71b2 <__swsetup_r+0x3a>
    723c:	2240      	movs	r2, #64	; 0x40
    723e:	4313      	orrs	r3, r2
    7240:	81a3      	strh	r3, [r4, #12]
    7242:	3801      	subs	r0, #1
    7244:	e7b5      	b.n	71b2 <__swsetup_r+0x3a>
    7246:	0788      	lsls	r0, r1, #30
    7248:	d400      	bmi.n	724c <__swsetup_r+0xd4>
    724a:	6962      	ldr	r2, [r4, #20]
    724c:	60a2      	str	r2, [r4, #8]
    724e:	e7ef      	b.n	7230 <__swsetup_r+0xb8>
    7250:	20002000 	.word	0x20002000
    7254:	00009760 	.word	0x00009760
    7258:	00009740 	.word	0x00009740
    725c:	00009720 	.word	0x00009720

00007260 <__assert_func>:
    7260:	b530      	push	{r4, r5, lr}
    7262:	0014      	movs	r4, r2
    7264:	001a      	movs	r2, r3
    7266:	4b09      	ldr	r3, [pc, #36]	; (728c <__assert_func+0x2c>)
    7268:	0005      	movs	r5, r0
    726a:	681b      	ldr	r3, [r3, #0]
    726c:	b085      	sub	sp, #20
    726e:	68d8      	ldr	r0, [r3, #12]
    7270:	b14c      	cbz	r4, 7286 <__assert_func+0x26>
    7272:	4b07      	ldr	r3, [pc, #28]	; (7290 <__assert_func+0x30>)
    7274:	9301      	str	r3, [sp, #4]
    7276:	9100      	str	r1, [sp, #0]
    7278:	002b      	movs	r3, r5
    727a:	4906      	ldr	r1, [pc, #24]	; (7294 <__assert_func+0x34>)
    727c:	9402      	str	r4, [sp, #8]
    727e:	f000 fa2a 	bl	76d6 <fiprintf>
    7282:	f001 fdcf 	bl	8e24 <abort>
    7286:	4b04      	ldr	r3, [pc, #16]	; (7298 <__assert_func+0x38>)
    7288:	001c      	movs	r4, r3
    728a:	e7f3      	b.n	7274 <__assert_func+0x14>
    728c:	20002000 	.word	0x20002000
    7290:	000095e0 	.word	0x000095e0
    7294:	000095ed 	.word	0x000095ed
    7298:	0000961b 	.word	0x0000961b

0000729c <__assert>:
    729c:	0013      	movs	r3, r2
    729e:	b510      	push	{r4, lr}
    72a0:	2200      	movs	r2, #0
    72a2:	f7ff ffdd 	bl	7260 <__assert_func>
    72a6:	bf00      	nop

000072a8 <_close_r>:
    72a8:	2300      	movs	r3, #0
    72aa:	b570      	push	{r4, r5, r6, lr}
    72ac:	4d05      	ldr	r5, [pc, #20]	; (72c4 <_close_r+0x1c>)
    72ae:	0004      	movs	r4, r0
    72b0:	0008      	movs	r0, r1
    72b2:	602b      	str	r3, [r5, #0]
    72b4:	f002 f870 	bl	9398 <_close>
    72b8:	1c43      	adds	r3, r0, #1
    72ba:	d102      	bne.n	72c2 <_close_r+0x1a>
    72bc:	682b      	ldr	r3, [r5, #0]
    72be:	b103      	cbz	r3, 72c2 <_close_r+0x1a>
    72c0:	6023      	str	r3, [r4, #0]
    72c2:	bd70      	pop	{r4, r5, r6, pc}
    72c4:	2000209c 	.word	0x2000209c

000072c8 <__errno>:
    72c8:	4b01      	ldr	r3, [pc, #4]	; (72d0 <__errno+0x8>)
    72ca:	6818      	ldr	r0, [r3, #0]
    72cc:	4770      	bx	lr
    72ce:	bf00      	nop
    72d0:	20002000 	.word	0x20002000

000072d4 <__sflush_r>:
    72d4:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    72d6:	898b      	ldrh	r3, [r1, #12]
    72d8:	0005      	movs	r5, r0
    72da:	000c      	movs	r4, r1
    72dc:	071a      	lsls	r2, r3, #28
    72de:	d45d      	bmi.n	739c <__sflush_r+0xc8>
    72e0:	684a      	ldr	r2, [r1, #4]
    72e2:	2a00      	cmp	r2, #0
    72e4:	dc04      	bgt.n	72f0 <__sflush_r+0x1c>
    72e6:	6c0a      	ldr	r2, [r1, #64]	; 0x40
    72e8:	2a00      	cmp	r2, #0
    72ea:	dc01      	bgt.n	72f0 <__sflush_r+0x1c>
    72ec:	2000      	movs	r0, #0
    72ee:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    72f0:	6ae7      	ldr	r7, [r4, #44]	; 0x2c
    72f2:	2f00      	cmp	r7, #0
    72f4:	d0fa      	beq.n	72ec <__sflush_r+0x18>
    72f6:	2200      	movs	r2, #0
    72f8:	f241 0100 	movw	r1, #4096	; 0x1000
    72fc:	682e      	ldr	r6, [r5, #0]
    72fe:	602a      	str	r2, [r5, #0]
    7300:	001a      	movs	r2, r3
    7302:	400a      	ands	r2, r1
    7304:	420b      	tst	r3, r1
    7306:	d032      	beq.n	736e <__sflush_r+0x9a>
    7308:	6d60      	ldr	r0, [r4, #84]	; 0x54
    730a:	89a3      	ldrh	r3, [r4, #12]
    730c:	075b      	lsls	r3, r3, #29
    730e:	d505      	bpl.n	731c <__sflush_r+0x48>
    7310:	6863      	ldr	r3, [r4, #4]
    7312:	1ac0      	subs	r0, r0, r3
    7314:	6b63      	ldr	r3, [r4, #52]	; 0x34
    7316:	b10b      	cbz	r3, 731c <__sflush_r+0x48>
    7318:	6c23      	ldr	r3, [r4, #64]	; 0x40
    731a:	1ac0      	subs	r0, r0, r3
    731c:	0002      	movs	r2, r0
    731e:	6a21      	ldr	r1, [r4, #32]
    7320:	2300      	movs	r3, #0
    7322:	0028      	movs	r0, r5
    7324:	6ae7      	ldr	r7, [r4, #44]	; 0x2c
    7326:	47b8      	blx	r7
    7328:	89a1      	ldrh	r1, [r4, #12]
    732a:	1c43      	adds	r3, r0, #1
    732c:	d106      	bne.n	733c <__sflush_r+0x68>
    732e:	682b      	ldr	r3, [r5, #0]
    7330:	2b1d      	cmp	r3, #29
    7332:	d830      	bhi.n	7396 <__sflush_r+0xc2>
    7334:	4a2b      	ldr	r2, [pc, #172]	; (73e4 <__sflush_r+0x110>)
    7336:	40da      	lsrs	r2, r3
    7338:	07d3      	lsls	r3, r2, #31
    733a:	d52c      	bpl.n	7396 <__sflush_r+0xc2>
    733c:	2300      	movs	r3, #0
    733e:	6063      	str	r3, [r4, #4]
    7340:	6923      	ldr	r3, [r4, #16]
    7342:	6023      	str	r3, [r4, #0]
    7344:	04cb      	lsls	r3, r1, #19
    7346:	d504      	bpl.n	7352 <__sflush_r+0x7e>
    7348:	1c43      	adds	r3, r0, #1
    734a:	d101      	bne.n	7350 <__sflush_r+0x7c>
    734c:	682b      	ldr	r3, [r5, #0]
    734e:	b903      	cbnz	r3, 7352 <__sflush_r+0x7e>
    7350:	6560      	str	r0, [r4, #84]	; 0x54
    7352:	6b61      	ldr	r1, [r4, #52]	; 0x34
    7354:	602e      	str	r6, [r5, #0]
    7356:	2900      	cmp	r1, #0
    7358:	d0c8      	beq.n	72ec <__sflush_r+0x18>
    735a:	0023      	movs	r3, r4
    735c:	3344      	adds	r3, #68	; 0x44
    735e:	4299      	cmp	r1, r3
    7360:	d002      	beq.n	7368 <__sflush_r+0x94>
    7362:	0028      	movs	r0, r5
    7364:	f000 fa94 	bl	7890 <_free_r>
    7368:	2000      	movs	r0, #0
    736a:	6360      	str	r0, [r4, #52]	; 0x34
    736c:	e7bf      	b.n	72ee <__sflush_r+0x1a>
    736e:	2301      	movs	r3, #1
    7370:	0028      	movs	r0, r5
    7372:	6a21      	ldr	r1, [r4, #32]
    7374:	47b8      	blx	r7
    7376:	1c43      	adds	r3, r0, #1
    7378:	d1c7      	bne.n	730a <__sflush_r+0x36>
    737a:	682b      	ldr	r3, [r5, #0]
    737c:	2b00      	cmp	r3, #0
    737e:	d0c4      	beq.n	730a <__sflush_r+0x36>
    7380:	2b1d      	cmp	r3, #29
    7382:	d001      	beq.n	7388 <__sflush_r+0xb4>
    7384:	2b16      	cmp	r3, #22
    7386:	d101      	bne.n	738c <__sflush_r+0xb8>
    7388:	602e      	str	r6, [r5, #0]
    738a:	e7af      	b.n	72ec <__sflush_r+0x18>
    738c:	2340      	movs	r3, #64	; 0x40
    738e:	89a2      	ldrh	r2, [r4, #12]
    7390:	4313      	orrs	r3, r2
    7392:	81a3      	strh	r3, [r4, #12]
    7394:	e7ab      	b.n	72ee <__sflush_r+0x1a>
    7396:	2340      	movs	r3, #64	; 0x40
    7398:	430b      	orrs	r3, r1
    739a:	e7fa      	b.n	7392 <__sflush_r+0xbe>
    739c:	690f      	ldr	r7, [r1, #16]
    739e:	2f00      	cmp	r7, #0
    73a0:	d0a4      	beq.n	72ec <__sflush_r+0x18>
    73a2:	680a      	ldr	r2, [r1, #0]
    73a4:	600f      	str	r7, [r1, #0]
    73a6:	1bd2      	subs	r2, r2, r7
    73a8:	9201      	str	r2, [sp, #4]
    73aa:	2200      	movs	r2, #0
    73ac:	079b      	lsls	r3, r3, #30
    73ae:	d100      	bne.n	73b2 <__sflush_r+0xde>
    73b0:	694a      	ldr	r2, [r1, #20]
    73b2:	60a2      	str	r2, [r4, #8]
    73b4:	9b01      	ldr	r3, [sp, #4]
    73b6:	2b00      	cmp	r3, #0
    73b8:	dd98      	ble.n	72ec <__sflush_r+0x18>
    73ba:	003a      	movs	r2, r7
    73bc:	0028      	movs	r0, r5
    73be:	9b01      	ldr	r3, [sp, #4]
    73c0:	6a21      	ldr	r1, [r4, #32]
    73c2:	6aa6      	ldr	r6, [r4, #40]	; 0x28
    73c4:	47b0      	blx	r6
    73c6:	2800      	cmp	r0, #0
    73c8:	dc06      	bgt.n	73d8 <__sflush_r+0x104>
    73ca:	2340      	movs	r3, #64	; 0x40
    73cc:	2001      	movs	r0, #1
    73ce:	89a2      	ldrh	r2, [r4, #12]
    73d0:	4240      	negs	r0, r0
    73d2:	4313      	orrs	r3, r2
    73d4:	81a3      	strh	r3, [r4, #12]
    73d6:	e78a      	b.n	72ee <__sflush_r+0x1a>
    73d8:	9b01      	ldr	r3, [sp, #4]
    73da:	183f      	adds	r7, r7, r0
    73dc:	1a1b      	subs	r3, r3, r0
    73de:	9301      	str	r3, [sp, #4]
    73e0:	e7e8      	b.n	73b4 <__sflush_r+0xe0>
    73e2:	bf00      	nop
    73e4:	20400001 	.word	0x20400001

000073e8 <_fflush_r>:
    73e8:	690b      	ldr	r3, [r1, #16]
    73ea:	b570      	push	{r4, r5, r6, lr}
    73ec:	0005      	movs	r5, r0
    73ee:	000c      	movs	r4, r1
    73f0:	b913      	cbnz	r3, 73f8 <_fflush_r+0x10>
    73f2:	2500      	movs	r5, #0
    73f4:	0028      	movs	r0, r5
    73f6:	bd70      	pop	{r4, r5, r6, pc}
    73f8:	b118      	cbz	r0, 7402 <_fflush_r+0x1a>
    73fa:	6983      	ldr	r3, [r0, #24]
    73fc:	b90b      	cbnz	r3, 7402 <_fflush_r+0x1a>
    73fe:	f000 f8c5 	bl	758c <__sinit>
    7402:	4b14      	ldr	r3, [pc, #80]	; (7454 <_fflush_r+0x6c>)
    7404:	429c      	cmp	r4, r3
    7406:	d11b      	bne.n	7440 <_fflush_r+0x58>
    7408:	686c      	ldr	r4, [r5, #4]
    740a:	220c      	movs	r2, #12
    740c:	5ea3      	ldrsh	r3, [r4, r2]
    740e:	2b00      	cmp	r3, #0
    7410:	d0ef      	beq.n	73f2 <_fflush_r+0xa>
    7412:	6e62      	ldr	r2, [r4, #100]	; 0x64
    7414:	07d2      	lsls	r2, r2, #31
    7416:	d404      	bmi.n	7422 <_fflush_r+0x3a>
    7418:	059b      	lsls	r3, r3, #22
    741a:	d402      	bmi.n	7422 <_fflush_r+0x3a>
    741c:	6da0      	ldr	r0, [r4, #88]	; 0x58
    741e:	f000 f9ab 	bl	7778 <__retarget_lock_acquire_recursive>
    7422:	0028      	movs	r0, r5
    7424:	0021      	movs	r1, r4
    7426:	f7ff ff55 	bl	72d4 <__sflush_r>
    742a:	6e63      	ldr	r3, [r4, #100]	; 0x64
    742c:	0005      	movs	r5, r0
    742e:	07db      	lsls	r3, r3, #31
    7430:	d4e0      	bmi.n	73f4 <_fflush_r+0xc>
    7432:	89a3      	ldrh	r3, [r4, #12]
    7434:	059b      	lsls	r3, r3, #22
    7436:	d4dd      	bmi.n	73f4 <_fflush_r+0xc>
    7438:	6da0      	ldr	r0, [r4, #88]	; 0x58
    743a:	f000 f9a3 	bl	7784 <__retarget_lock_release_recursive>
    743e:	e7d9      	b.n	73f4 <_fflush_r+0xc>
    7440:	4b05      	ldr	r3, [pc, #20]	; (7458 <_fflush_r+0x70>)
    7442:	429c      	cmp	r4, r3
    7444:	d101      	bne.n	744a <_fflush_r+0x62>
    7446:	68ac      	ldr	r4, [r5, #8]
    7448:	e7df      	b.n	740a <_fflush_r+0x22>
    744a:	4b04      	ldr	r3, [pc, #16]	; (745c <_fflush_r+0x74>)
    744c:	429c      	cmp	r4, r3
    744e:	d1dc      	bne.n	740a <_fflush_r+0x22>
    7450:	68ec      	ldr	r4, [r5, #12]
    7452:	e7da      	b.n	740a <_fflush_r+0x22>
    7454:	00009760 	.word	0x00009760
    7458:	00009740 	.word	0x00009740
    745c:	00009720 	.word	0x00009720

00007460 <fflush>:
    7460:	0001      	movs	r1, r0
    7462:	b510      	push	{r4, lr}
    7464:	b928      	cbnz	r0, 7472 <fflush+0x12>
    7466:	4b05      	ldr	r3, [pc, #20]	; (747c <fflush+0x1c>)
    7468:	4905      	ldr	r1, [pc, #20]	; (7480 <fflush+0x20>)
    746a:	6818      	ldr	r0, [r3, #0]
    746c:	f000 f95f 	bl	772e <_fwalk_reent>
    7470:	bd10      	pop	{r4, pc}
    7472:	4b04      	ldr	r3, [pc, #16]	; (7484 <fflush+0x24>)
    7474:	6818      	ldr	r0, [r3, #0]
    7476:	f7ff ffb7 	bl	73e8 <_fflush_r>
    747a:	e7f9      	b.n	7470 <fflush+0x10>
    747c:	00009780 	.word	0x00009780
    7480:	000073e9 	.word	0x000073e9
    7484:	20002000 	.word	0x20002000

00007488 <std>:
    7488:	2300      	movs	r3, #0
    748a:	b510      	push	{r4, lr}
    748c:	0004      	movs	r4, r0
    748e:	6003      	str	r3, [r0, #0]
    7490:	6043      	str	r3, [r0, #4]
    7492:	6083      	str	r3, [r0, #8]
    7494:	8181      	strh	r1, [r0, #12]
    7496:	6643      	str	r3, [r0, #100]	; 0x64
    7498:	0019      	movs	r1, r3
    749a:	81c2      	strh	r2, [r0, #14]
    749c:	6103      	str	r3, [r0, #16]
    749e:	6143      	str	r3, [r0, #20]
    74a0:	6183      	str	r3, [r0, #24]
    74a2:	2208      	movs	r2, #8
    74a4:	305c      	adds	r0, #92	; 0x5c
    74a6:	f7ff f9c9 	bl	683c <memset>
    74aa:	4b05      	ldr	r3, [pc, #20]	; (74c0 <std+0x38>)
    74ac:	6224      	str	r4, [r4, #32]
    74ae:	6263      	str	r3, [r4, #36]	; 0x24
    74b0:	4b04      	ldr	r3, [pc, #16]	; (74c4 <std+0x3c>)
    74b2:	62a3      	str	r3, [r4, #40]	; 0x28
    74b4:	4b04      	ldr	r3, [pc, #16]	; (74c8 <std+0x40>)
    74b6:	62e3      	str	r3, [r4, #44]	; 0x2c
    74b8:	4b04      	ldr	r3, [pc, #16]	; (74cc <std+0x44>)
    74ba:	6323      	str	r3, [r4, #48]	; 0x30
    74bc:	bd10      	pop	{r4, pc}
    74be:	bf00      	nop
    74c0:	00006c55 	.word	0x00006c55
    74c4:	00006c81 	.word	0x00006c81
    74c8:	00006cb9 	.word	0x00006cb9
    74cc:	00006ce5 	.word	0x00006ce5

000074d0 <_cleanup_r>:
    74d0:	b510      	push	{r4, lr}
    74d2:	4902      	ldr	r1, [pc, #8]	; (74dc <_cleanup_r+0xc>)
    74d4:	f000 f92b 	bl	772e <_fwalk_reent>
    74d8:	bd10      	pop	{r4, pc}
    74da:	bf00      	nop
    74dc:	000073e9 	.word	0x000073e9

000074e0 <__fp_lock>:
    74e0:	6e43      	ldr	r3, [r0, #100]	; 0x64
    74e2:	b510      	push	{r4, lr}
    74e4:	07db      	lsls	r3, r3, #31
    74e6:	d405      	bmi.n	74f4 <__fp_lock+0x14>
    74e8:	8983      	ldrh	r3, [r0, #12]
    74ea:	059b      	lsls	r3, r3, #22
    74ec:	d402      	bmi.n	74f4 <__fp_lock+0x14>
    74ee:	6d80      	ldr	r0, [r0, #88]	; 0x58
    74f0:	f000 f942 	bl	7778 <__retarget_lock_acquire_recursive>
    74f4:	2000      	movs	r0, #0
    74f6:	bd10      	pop	{r4, pc}

000074f8 <__fp_unlock>:
    74f8:	6e43      	ldr	r3, [r0, #100]	; 0x64
    74fa:	b510      	push	{r4, lr}
    74fc:	07db      	lsls	r3, r3, #31
    74fe:	d405      	bmi.n	750c <__fp_unlock+0x14>
    7500:	8983      	ldrh	r3, [r0, #12]
    7502:	059b      	lsls	r3, r3, #22
    7504:	d402      	bmi.n	750c <__fp_unlock+0x14>
    7506:	6d80      	ldr	r0, [r0, #88]	; 0x58
    7508:	f000 f93c 	bl	7784 <__retarget_lock_release_recursive>
    750c:	2000      	movs	r0, #0
    750e:	bd10      	pop	{r4, pc}

00007510 <__sfmoreglue>:
    7510:	b570      	push	{r4, r5, r6, lr}
    7512:	2568      	movs	r5, #104	; 0x68
    7514:	1e4a      	subs	r2, r1, #1
    7516:	4355      	muls	r5, r2
    7518:	000e      	movs	r6, r1
    751a:	0029      	movs	r1, r5
    751c:	3174      	adds	r1, #116	; 0x74
    751e:	f000 f9fd 	bl	791c <_malloc_r>
    7522:	0004      	movs	r4, r0
    7524:	b140      	cbz	r0, 7538 <__sfmoreglue+0x28>
    7526:	2100      	movs	r1, #0
    7528:	002a      	movs	r2, r5
    752a:	6001      	str	r1, [r0, #0]
    752c:	6046      	str	r6, [r0, #4]
    752e:	300c      	adds	r0, #12
    7530:	60a0      	str	r0, [r4, #8]
    7532:	3268      	adds	r2, #104	; 0x68
    7534:	f7ff f982 	bl	683c <memset>
    7538:	0020      	movs	r0, r4
    753a:	bd70      	pop	{r4, r5, r6, pc}

0000753c <_cleanup>:
    753c:	b510      	push	{r4, lr}
    753e:	4b02      	ldr	r3, [pc, #8]	; (7548 <_cleanup+0xc>)
    7540:	6818      	ldr	r0, [r3, #0]
    7542:	f7ff ffc5 	bl	74d0 <_cleanup_r>
    7546:	bd10      	pop	{r4, pc}
    7548:	00009780 	.word	0x00009780

0000754c <__sfp_lock_acquire>:
    754c:	b510      	push	{r4, lr}
    754e:	4802      	ldr	r0, [pc, #8]	; (7558 <__sfp_lock_acquire+0xc>)
    7550:	f000 f912 	bl	7778 <__retarget_lock_acquire_recursive>
    7554:	bd10      	pop	{r4, pc}
    7556:	bf00      	nop
    7558:	2000208f 	.word	0x2000208f

0000755c <__sfp_lock_release>:
    755c:	b510      	push	{r4, lr}
    755e:	4802      	ldr	r0, [pc, #8]	; (7568 <__sfp_lock_release+0xc>)
    7560:	f000 f910 	bl	7784 <__retarget_lock_release_recursive>
    7564:	bd10      	pop	{r4, pc}
    7566:	bf00      	nop
    7568:	2000208f 	.word	0x2000208f

0000756c <__sinit_lock_acquire>:
    756c:	b510      	push	{r4, lr}
    756e:	4802      	ldr	r0, [pc, #8]	; (7578 <__sinit_lock_acquire+0xc>)
    7570:	f000 f902 	bl	7778 <__retarget_lock_acquire_recursive>
    7574:	bd10      	pop	{r4, pc}
    7576:	bf00      	nop
    7578:	20002090 	.word	0x20002090

0000757c <__sinit_lock_release>:
    757c:	b510      	push	{r4, lr}
    757e:	4802      	ldr	r0, [pc, #8]	; (7588 <__sinit_lock_release+0xc>)
    7580:	f000 f900 	bl	7784 <__retarget_lock_release_recursive>
    7584:	bd10      	pop	{r4, pc}
    7586:	bf00      	nop
    7588:	20002090 	.word	0x20002090

0000758c <__sinit>:
    758c:	b513      	push	{r0, r1, r4, lr}
    758e:	0004      	movs	r4, r0
    7590:	f7ff ffec 	bl	756c <__sinit_lock_acquire>
    7594:	69a3      	ldr	r3, [r4, #24]
    7596:	b113      	cbz	r3, 759e <__sinit+0x12>
    7598:	f7ff fff0 	bl	757c <__sinit_lock_release>
    759c:	bd13      	pop	{r0, r1, r4, pc}
    759e:	64a3      	str	r3, [r4, #72]	; 0x48
    75a0:	64e3      	str	r3, [r4, #76]	; 0x4c
    75a2:	6523      	str	r3, [r4, #80]	; 0x50
    75a4:	4b13      	ldr	r3, [pc, #76]	; (75f4 <__sinit+0x68>)
    75a6:	4a14      	ldr	r2, [pc, #80]	; (75f8 <__sinit+0x6c>)
    75a8:	681b      	ldr	r3, [r3, #0]
    75aa:	62a2      	str	r2, [r4, #40]	; 0x28
    75ac:	9301      	str	r3, [sp, #4]
    75ae:	42a3      	cmp	r3, r4
    75b0:	d101      	bne.n	75b6 <__sinit+0x2a>
    75b2:	2301      	movs	r3, #1
    75b4:	61a3      	str	r3, [r4, #24]
    75b6:	0020      	movs	r0, r4
    75b8:	f000 f820 	bl	75fc <__sfp>
    75bc:	6060      	str	r0, [r4, #4]
    75be:	0020      	movs	r0, r4
    75c0:	f000 f81c 	bl	75fc <__sfp>
    75c4:	60a0      	str	r0, [r4, #8]
    75c6:	0020      	movs	r0, r4
    75c8:	f000 f818 	bl	75fc <__sfp>
    75cc:	2200      	movs	r2, #0
    75ce:	2104      	movs	r1, #4
    75d0:	60e0      	str	r0, [r4, #12]
    75d2:	6860      	ldr	r0, [r4, #4]
    75d4:	f7ff ff58 	bl	7488 <std>
    75d8:	2201      	movs	r2, #1
    75da:	2109      	movs	r1, #9
    75dc:	68a0      	ldr	r0, [r4, #8]
    75de:	f7ff ff53 	bl	7488 <std>
    75e2:	2202      	movs	r2, #2
    75e4:	2112      	movs	r1, #18
    75e6:	68e0      	ldr	r0, [r4, #12]
    75e8:	f7ff ff4e 	bl	7488 <std>
    75ec:	2301      	movs	r3, #1
    75ee:	61a3      	str	r3, [r4, #24]
    75f0:	e7d2      	b.n	7598 <__sinit+0xc>
    75f2:	bf00      	nop
    75f4:	00009780 	.word	0x00009780
    75f8:	000074d1 	.word	0x000074d1

000075fc <__sfp>:
    75fc:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    75fe:	0007      	movs	r7, r0
    7600:	f7ff ffa4 	bl	754c <__sfp_lock_acquire>
    7604:	4b1e      	ldr	r3, [pc, #120]	; (7680 <__sfp+0x84>)
    7606:	681e      	ldr	r6, [r3, #0]
    7608:	69b3      	ldr	r3, [r6, #24]
    760a:	b913      	cbnz	r3, 7612 <__sfp+0x16>
    760c:	0030      	movs	r0, r6
    760e:	f7ff ffbd 	bl	758c <__sinit>
    7612:	3648      	adds	r6, #72	; 0x48
    7614:	68b4      	ldr	r4, [r6, #8]
    7616:	6873      	ldr	r3, [r6, #4]
    7618:	3b01      	subs	r3, #1
    761a:	d503      	bpl.n	7624 <__sfp+0x28>
    761c:	6833      	ldr	r3, [r6, #0]
    761e:	b30b      	cbz	r3, 7664 <__sfp+0x68>
    7620:	6836      	ldr	r6, [r6, #0]
    7622:	e7f7      	b.n	7614 <__sfp+0x18>
    7624:	220c      	movs	r2, #12
    7626:	5ea5      	ldrsh	r5, [r4, r2]
    7628:	b9d5      	cbnz	r5, 7660 <__sfp+0x64>
    762a:	0020      	movs	r0, r4
    762c:	4b15      	ldr	r3, [pc, #84]	; (7684 <__sfp+0x88>)
    762e:	3058      	adds	r0, #88	; 0x58
    7630:	60e3      	str	r3, [r4, #12]
    7632:	6665      	str	r5, [r4, #100]	; 0x64
    7634:	f000 f89c 	bl	7770 <__retarget_lock_init_recursive>
    7638:	f7ff ff90 	bl	755c <__sfp_lock_release>
    763c:	0020      	movs	r0, r4
    763e:	2208      	movs	r2, #8
    7640:	0029      	movs	r1, r5
    7642:	6025      	str	r5, [r4, #0]
    7644:	60a5      	str	r5, [r4, #8]
    7646:	6065      	str	r5, [r4, #4]
    7648:	6125      	str	r5, [r4, #16]
    764a:	6165      	str	r5, [r4, #20]
    764c:	61a5      	str	r5, [r4, #24]
    764e:	305c      	adds	r0, #92	; 0x5c
    7650:	f7ff f8f4 	bl	683c <memset>
    7654:	6365      	str	r5, [r4, #52]	; 0x34
    7656:	63a5      	str	r5, [r4, #56]	; 0x38
    7658:	64a5      	str	r5, [r4, #72]	; 0x48
    765a:	64e5      	str	r5, [r4, #76]	; 0x4c
    765c:	0020      	movs	r0, r4
    765e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    7660:	3468      	adds	r4, #104	; 0x68
    7662:	e7d9      	b.n	7618 <__sfp+0x1c>
    7664:	2104      	movs	r1, #4
    7666:	0038      	movs	r0, r7
    7668:	f7ff ff52 	bl	7510 <__sfmoreglue>
    766c:	0004      	movs	r4, r0
    766e:	6030      	str	r0, [r6, #0]
    7670:	2800      	cmp	r0, #0
    7672:	d1d5      	bne.n	7620 <__sfp+0x24>
    7674:	f7ff ff72 	bl	755c <__sfp_lock_release>
    7678:	230c      	movs	r3, #12
    767a:	603b      	str	r3, [r7, #0]
    767c:	e7ee      	b.n	765c <__sfp+0x60>
    767e:	bf00      	nop
    7680:	00009780 	.word	0x00009780
    7684:	ffff0001 	.word	0xffff0001

00007688 <__fp_lock_all>:
    7688:	b510      	push	{r4, lr}
    768a:	f7ff ff5f 	bl	754c <__sfp_lock_acquire>
    768e:	4b03      	ldr	r3, [pc, #12]	; (769c <__fp_lock_all+0x14>)
    7690:	4903      	ldr	r1, [pc, #12]	; (76a0 <__fp_lock_all+0x18>)
    7692:	6818      	ldr	r0, [r3, #0]
    7694:	f000 f830 	bl	76f8 <_fwalk>
    7698:	bd10      	pop	{r4, pc}
    769a:	bf00      	nop
    769c:	20002000 	.word	0x20002000
    76a0:	000074e1 	.word	0x000074e1

000076a4 <__fp_unlock_all>:
    76a4:	b510      	push	{r4, lr}
    76a6:	4b04      	ldr	r3, [pc, #16]	; (76b8 <__fp_unlock_all+0x14>)
    76a8:	4904      	ldr	r1, [pc, #16]	; (76bc <__fp_unlock_all+0x18>)
    76aa:	6818      	ldr	r0, [r3, #0]
    76ac:	f000 f824 	bl	76f8 <_fwalk>
    76b0:	f7ff ff54 	bl	755c <__sfp_lock_release>
    76b4:	bd10      	pop	{r4, pc}
    76b6:	bf00      	nop
    76b8:	20002000 	.word	0x20002000
    76bc:	000074f9 	.word	0x000074f9

000076c0 <_fiprintf_r>:
    76c0:	b40c      	push	{r2, r3}
    76c2:	b507      	push	{r0, r1, r2, lr}
    76c4:	ab04      	add	r3, sp, #16
    76c6:	cb04      	ldmia	r3!, {r2}
    76c8:	9301      	str	r3, [sp, #4]
    76ca:	f000 fee3 	bl	8494 <_vfiprintf_r>
    76ce:	b003      	add	sp, #12
    76d0:	bc08      	pop	{r3}
    76d2:	b002      	add	sp, #8
    76d4:	4718      	bx	r3

000076d6 <fiprintf>:
    76d6:	b40e      	push	{r1, r2, r3}
    76d8:	b503      	push	{r0, r1, lr}
    76da:	0001      	movs	r1, r0
    76dc:	ab03      	add	r3, sp, #12
    76de:	4805      	ldr	r0, [pc, #20]	; (76f4 <fiprintf+0x1e>)
    76e0:	cb04      	ldmia	r3!, {r2}
    76e2:	6800      	ldr	r0, [r0, #0]
    76e4:	9301      	str	r3, [sp, #4]
    76e6:	f000 fed5 	bl	8494 <_vfiprintf_r>
    76ea:	b002      	add	sp, #8
    76ec:	bc08      	pop	{r3}
    76ee:	b003      	add	sp, #12
    76f0:	4718      	bx	r3
    76f2:	bf00      	nop
    76f4:	20002000 	.word	0x20002000

000076f8 <_fwalk>:
    76f8:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    76fa:	3048      	adds	r0, #72	; 0x48
    76fc:	0004      	movs	r4, r0
    76fe:	2600      	movs	r6, #0
    7700:	9101      	str	r1, [sp, #4]
    7702:	68a5      	ldr	r5, [r4, #8]
    7704:	6867      	ldr	r7, [r4, #4]
    7706:	3f01      	subs	r7, #1
    7708:	d504      	bpl.n	7714 <_fwalk+0x1c>
    770a:	6824      	ldr	r4, [r4, #0]
    770c:	2c00      	cmp	r4, #0
    770e:	d1f8      	bne.n	7702 <_fwalk+0xa>
    7710:	0030      	movs	r0, r6
    7712:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    7714:	89ab      	ldrh	r3, [r5, #12]
    7716:	2b01      	cmp	r3, #1
    7718:	d907      	bls.n	772a <_fwalk+0x32>
    771a:	220e      	movs	r2, #14
    771c:	5eab      	ldrsh	r3, [r5, r2]
    771e:	3301      	adds	r3, #1
    7720:	d003      	beq.n	772a <_fwalk+0x32>
    7722:	0028      	movs	r0, r5
    7724:	9b01      	ldr	r3, [sp, #4]
    7726:	4798      	blx	r3
    7728:	4306      	orrs	r6, r0
    772a:	3568      	adds	r5, #104	; 0x68
    772c:	e7eb      	b.n	7706 <_fwalk+0xe>

0000772e <_fwalk_reent>:
    772e:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    7730:	0004      	movs	r4, r0
    7732:	0006      	movs	r6, r0
    7734:	2700      	movs	r7, #0
    7736:	9101      	str	r1, [sp, #4]
    7738:	3448      	adds	r4, #72	; 0x48
    773a:	6863      	ldr	r3, [r4, #4]
    773c:	68a5      	ldr	r5, [r4, #8]
    773e:	9300      	str	r3, [sp, #0]
    7740:	9b00      	ldr	r3, [sp, #0]
    7742:	3b01      	subs	r3, #1
    7744:	9300      	str	r3, [sp, #0]
    7746:	d504      	bpl.n	7752 <_fwalk_reent+0x24>
    7748:	6824      	ldr	r4, [r4, #0]
    774a:	2c00      	cmp	r4, #0
    774c:	d1f5      	bne.n	773a <_fwalk_reent+0xc>
    774e:	0038      	movs	r0, r7
    7750:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    7752:	89ab      	ldrh	r3, [r5, #12]
    7754:	2b01      	cmp	r3, #1
    7756:	d908      	bls.n	776a <_fwalk_reent+0x3c>
    7758:	220e      	movs	r2, #14
    775a:	5eab      	ldrsh	r3, [r5, r2]
    775c:	3301      	adds	r3, #1
    775e:	d004      	beq.n	776a <_fwalk_reent+0x3c>
    7760:	0029      	movs	r1, r5
    7762:	0030      	movs	r0, r6
    7764:	9b01      	ldr	r3, [sp, #4]
    7766:	4798      	blx	r3
    7768:	4307      	orrs	r7, r0
    776a:	3568      	adds	r5, #104	; 0x68
    776c:	e7e8      	b.n	7740 <_fwalk_reent+0x12>

0000776e <__retarget_lock_init>:
    776e:	4770      	bx	lr

00007770 <__retarget_lock_init_recursive>:
    7770:	4770      	bx	lr

00007772 <__retarget_lock_close>:
    7772:	4770      	bx	lr

00007774 <__retarget_lock_close_recursive>:
    7774:	4770      	bx	lr

00007776 <__retarget_lock_acquire>:
    7776:	4770      	bx	lr

00007778 <__retarget_lock_acquire_recursive>:
    7778:	4770      	bx	lr

0000777a <__retarget_lock_try_acquire>:
    777a:	2001      	movs	r0, #1
    777c:	4770      	bx	lr

0000777e <__retarget_lock_try_acquire_recursive>:
    777e:	2001      	movs	r0, #1
    7780:	4770      	bx	lr

00007782 <__retarget_lock_release>:
    7782:	4770      	bx	lr

00007784 <__retarget_lock_release_recursive>:
    7784:	4770      	bx	lr
	...

00007788 <_lseek_r>:
    7788:	b570      	push	{r4, r5, r6, lr}
    778a:	0004      	movs	r4, r0
    778c:	0008      	movs	r0, r1
    778e:	0011      	movs	r1, r2
    7790:	001a      	movs	r2, r3
    7792:	2300      	movs	r3, #0
    7794:	4d04      	ldr	r5, [pc, #16]	; (77a8 <_lseek_r+0x20>)
    7796:	602b      	str	r3, [r5, #0]
    7798:	f001 fe26 	bl	93e8 <_lseek>
    779c:	1c43      	adds	r3, r0, #1
    779e:	d102      	bne.n	77a6 <_lseek_r+0x1e>
    77a0:	682b      	ldr	r3, [r5, #0]
    77a2:	b103      	cbz	r3, 77a6 <_lseek_r+0x1e>
    77a4:	6023      	str	r3, [r4, #0]
    77a6:	bd70      	pop	{r4, r5, r6, pc}
    77a8:	2000209c 	.word	0x2000209c

000077ac <__swhatbuf_r>:
    77ac:	b570      	push	{r4, r5, r6, lr}
    77ae:	000e      	movs	r6, r1
    77b0:	001d      	movs	r5, r3
    77b2:	230e      	movs	r3, #14
    77b4:	5ec9      	ldrsh	r1, [r1, r3]
    77b6:	0014      	movs	r4, r2
    77b8:	b096      	sub	sp, #88	; 0x58
    77ba:	2900      	cmp	r1, #0
    77bc:	da08      	bge.n	77d0 <__swhatbuf_r+0x24>
    77be:	220c      	movs	r2, #12
    77c0:	5eb3      	ldrsh	r3, [r6, r2]
    77c2:	2200      	movs	r2, #0
    77c4:	602a      	str	r2, [r5, #0]
    77c6:	061b      	lsls	r3, r3, #24
    77c8:	d411      	bmi.n	77ee <__swhatbuf_r+0x42>
    77ca:	f240 4300 	movw	r3, #1024	; 0x400
    77ce:	e00f      	b.n	77f0 <__swhatbuf_r+0x44>
    77d0:	466a      	mov	r2, sp
    77d2:	f001 fb2f 	bl	8e34 <_fstat_r>
    77d6:	2800      	cmp	r0, #0
    77d8:	dbf1      	blt.n	77be <__swhatbuf_r+0x12>
    77da:	f24f 0300 	movw	r3, #61440	; 0xf000
    77de:	9901      	ldr	r1, [sp, #4]
    77e0:	4019      	ands	r1, r3
    77e2:	4b05      	ldr	r3, [pc, #20]	; (77f8 <__swhatbuf_r+0x4c>)
    77e4:	18c9      	adds	r1, r1, r3
    77e6:	424b      	negs	r3, r1
    77e8:	4159      	adcs	r1, r3
    77ea:	6029      	str	r1, [r5, #0]
    77ec:	e7ed      	b.n	77ca <__swhatbuf_r+0x1e>
    77ee:	2340      	movs	r3, #64	; 0x40
    77f0:	2000      	movs	r0, #0
    77f2:	6023      	str	r3, [r4, #0]
    77f4:	b016      	add	sp, #88	; 0x58
    77f6:	bd70      	pop	{r4, r5, r6, pc}
    77f8:	ffffe000 	.word	0xffffe000

000077fc <__smakebuf_r>:
    77fc:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    77fe:	2602      	movs	r6, #2
    7800:	898b      	ldrh	r3, [r1, #12]
    7802:	0005      	movs	r5, r0
    7804:	000c      	movs	r4, r1
    7806:	4233      	tst	r3, r6
    7808:	d006      	beq.n	7818 <__smakebuf_r+0x1c>
    780a:	0023      	movs	r3, r4
    780c:	3347      	adds	r3, #71	; 0x47
    780e:	6023      	str	r3, [r4, #0]
    7810:	6123      	str	r3, [r4, #16]
    7812:	2301      	movs	r3, #1
    7814:	6163      	str	r3, [r4, #20]
    7816:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
    7818:	466a      	mov	r2, sp
    781a:	ab01      	add	r3, sp, #4
    781c:	f7ff ffc6 	bl	77ac <__swhatbuf_r>
    7820:	9900      	ldr	r1, [sp, #0]
    7822:	0007      	movs	r7, r0
    7824:	0028      	movs	r0, r5
    7826:	f000 f879 	bl	791c <_malloc_r>
    782a:	b940      	cbnz	r0, 783e <__smakebuf_r+0x42>
    782c:	220c      	movs	r2, #12
    782e:	5ea3      	ldrsh	r3, [r4, r2]
    7830:	059a      	lsls	r2, r3, #22
    7832:	d4f0      	bmi.n	7816 <__smakebuf_r+0x1a>
    7834:	2203      	movs	r2, #3
    7836:	4393      	bics	r3, r2
    7838:	431e      	orrs	r6, r3
    783a:	81a6      	strh	r6, [r4, #12]
    783c:	e7e5      	b.n	780a <__smakebuf_r+0xe>
    783e:	4b0e      	ldr	r3, [pc, #56]	; (7878 <__smakebuf_r+0x7c>)
    7840:	62ab      	str	r3, [r5, #40]	; 0x28
    7842:	2380      	movs	r3, #128	; 0x80
    7844:	89a2      	ldrh	r2, [r4, #12]
    7846:	6020      	str	r0, [r4, #0]
    7848:	4313      	orrs	r3, r2
    784a:	81a3      	strh	r3, [r4, #12]
    784c:	9b00      	ldr	r3, [sp, #0]
    784e:	6120      	str	r0, [r4, #16]
    7850:	6163      	str	r3, [r4, #20]
    7852:	9b01      	ldr	r3, [sp, #4]
    7854:	b163      	cbz	r3, 7870 <__smakebuf_r+0x74>
    7856:	0028      	movs	r0, r5
    7858:	230e      	movs	r3, #14
    785a:	5ee1      	ldrsh	r1, [r4, r3]
    785c:	f001 fc64 	bl	9128 <_isatty_r>
    7860:	b130      	cbz	r0, 7870 <__smakebuf_r+0x74>
    7862:	2203      	movs	r2, #3
    7864:	89a3      	ldrh	r3, [r4, #12]
    7866:	4393      	bics	r3, r2
    7868:	001a      	movs	r2, r3
    786a:	2301      	movs	r3, #1
    786c:	4313      	orrs	r3, r2
    786e:	81a3      	strh	r3, [r4, #12]
    7870:	89a0      	ldrh	r0, [r4, #12]
    7872:	4307      	orrs	r7, r0
    7874:	81a7      	strh	r7, [r4, #12]
    7876:	e7ce      	b.n	7816 <__smakebuf_r+0x1a>
    7878:	000074d1 	.word	0x000074d1

0000787c <malloc>:
    787c:	b510      	push	{r4, lr}
    787e:	4b03      	ldr	r3, [pc, #12]	; (788c <malloc+0x10>)
    7880:	0001      	movs	r1, r0
    7882:	6818      	ldr	r0, [r3, #0]
    7884:	f000 f84a 	bl	791c <_malloc_r>
    7888:	bd10      	pop	{r4, pc}
    788a:	bf00      	nop
    788c:	20002000 	.word	0x20002000

00007890 <_free_r>:
    7890:	b570      	push	{r4, r5, r6, lr}
    7892:	0005      	movs	r5, r0
    7894:	b179      	cbz	r1, 78b6 <_free_r+0x26>
    7896:	1f0c      	subs	r4, r1, #4
    7898:	6823      	ldr	r3, [r4, #0]
    789a:	2b00      	cmp	r3, #0
    789c:	da00      	bge.n	78a0 <_free_r+0x10>
    789e:	18e4      	adds	r4, r4, r3
    78a0:	0028      	movs	r0, r5
    78a2:	f001 fc79 	bl	9198 <__malloc_lock>
    78a6:	4a1c      	ldr	r2, [pc, #112]	; (7918 <_free_r+0x88>)
    78a8:	6813      	ldr	r3, [r2, #0]
    78aa:	b92b      	cbnz	r3, 78b8 <_free_r+0x28>
    78ac:	6063      	str	r3, [r4, #4]
    78ae:	6014      	str	r4, [r2, #0]
    78b0:	0028      	movs	r0, r5
    78b2:	f001 fc79 	bl	91a8 <__malloc_unlock>
    78b6:	bd70      	pop	{r4, r5, r6, pc}
    78b8:	42a3      	cmp	r3, r4
    78ba:	d908      	bls.n	78ce <_free_r+0x3e>
    78bc:	6821      	ldr	r1, [r4, #0]
    78be:	1860      	adds	r0, r4, r1
    78c0:	4283      	cmp	r3, r0
    78c2:	d1f3      	bne.n	78ac <_free_r+0x1c>
    78c4:	6818      	ldr	r0, [r3, #0]
    78c6:	685b      	ldr	r3, [r3, #4]
    78c8:	1841      	adds	r1, r0, r1
    78ca:	6021      	str	r1, [r4, #0]
    78cc:	e7ee      	b.n	78ac <_free_r+0x1c>
    78ce:	001a      	movs	r2, r3
    78d0:	685b      	ldr	r3, [r3, #4]
    78d2:	b10b      	cbz	r3, 78d8 <_free_r+0x48>
    78d4:	42a3      	cmp	r3, r4
    78d6:	d9fa      	bls.n	78ce <_free_r+0x3e>
    78d8:	6811      	ldr	r1, [r2, #0]
    78da:	1850      	adds	r0, r2, r1
    78dc:	42a0      	cmp	r0, r4
    78de:	d10b      	bne.n	78f8 <_free_r+0x68>
    78e0:	6820      	ldr	r0, [r4, #0]
    78e2:	1809      	adds	r1, r1, r0
    78e4:	1850      	adds	r0, r2, r1
    78e6:	6011      	str	r1, [r2, #0]
    78e8:	4283      	cmp	r3, r0
    78ea:	d1e1      	bne.n	78b0 <_free_r+0x20>
    78ec:	6818      	ldr	r0, [r3, #0]
    78ee:	685b      	ldr	r3, [r3, #4]
    78f0:	1841      	adds	r1, r0, r1
    78f2:	6011      	str	r1, [r2, #0]
    78f4:	6053      	str	r3, [r2, #4]
    78f6:	e7db      	b.n	78b0 <_free_r+0x20>
    78f8:	42a0      	cmp	r0, r4
    78fa:	d902      	bls.n	7902 <_free_r+0x72>
    78fc:	230c      	movs	r3, #12
    78fe:	602b      	str	r3, [r5, #0]
    7900:	e7d6      	b.n	78b0 <_free_r+0x20>
    7902:	6821      	ldr	r1, [r4, #0]
    7904:	1860      	adds	r0, r4, r1
    7906:	4283      	cmp	r3, r0
    7908:	d103      	bne.n	7912 <_free_r+0x82>
    790a:	6818      	ldr	r0, [r3, #0]
    790c:	685b      	ldr	r3, [r3, #4]
    790e:	1841      	adds	r1, r0, r1
    7910:	6021      	str	r1, [r4, #0]
    7912:	6063      	str	r3, [r4, #4]
    7914:	6054      	str	r4, [r2, #4]
    7916:	e7cb      	b.n	78b0 <_free_r+0x20>
    7918:	20002094 	.word	0x20002094

0000791c <_malloc_r>:
    791c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    791e:	2303      	movs	r3, #3
    7920:	1ccd      	adds	r5, r1, #3
    7922:	439d      	bics	r5, r3
    7924:	3508      	adds	r5, #8
    7926:	0006      	movs	r6, r0
    7928:	2d0c      	cmp	r5, #12
    792a:	d21c      	bcs.n	7966 <_malloc_r+0x4a>
    792c:	250c      	movs	r5, #12
    792e:	42a9      	cmp	r1, r5
    7930:	d81b      	bhi.n	796a <_malloc_r+0x4e>
    7932:	4f26      	ldr	r7, [pc, #152]	; (79cc <_malloc_r+0xb0>)
    7934:	0030      	movs	r0, r6
    7936:	f001 fc2f 	bl	9198 <__malloc_lock>
    793a:	683a      	ldr	r2, [r7, #0]
    793c:	0014      	movs	r4, r2
    793e:	b9c4      	cbnz	r4, 7972 <_malloc_r+0x56>
    7940:	687b      	ldr	r3, [r7, #4]
    7942:	b923      	cbnz	r3, 794e <_malloc_r+0x32>
    7944:	0021      	movs	r1, r4
    7946:	0030      	movs	r0, r6
    7948:	f001 f8ae 	bl	8aa8 <_sbrk_r>
    794c:	6078      	str	r0, [r7, #4]
    794e:	0029      	movs	r1, r5
    7950:	0030      	movs	r0, r6
    7952:	f001 f8a9 	bl	8aa8 <_sbrk_r>
    7956:	1c43      	adds	r3, r0, #1
    7958:	d12b      	bne.n	79b2 <_malloc_r+0x96>
    795a:	230c      	movs	r3, #12
    795c:	0030      	movs	r0, r6
    795e:	6033      	str	r3, [r6, #0]
    7960:	f001 fc22 	bl	91a8 <__malloc_unlock>
    7964:	e003      	b.n	796e <_malloc_r+0x52>
    7966:	2d00      	cmp	r5, #0
    7968:	dae1      	bge.n	792e <_malloc_r+0x12>
    796a:	230c      	movs	r3, #12
    796c:	6033      	str	r3, [r6, #0]
    796e:	2000      	movs	r0, #0
    7970:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    7972:	6823      	ldr	r3, [r4, #0]
    7974:	1b5b      	subs	r3, r3, r5
    7976:	d419      	bmi.n	79ac <_malloc_r+0x90>
    7978:	2b0b      	cmp	r3, #11
    797a:	d903      	bls.n	7984 <_malloc_r+0x68>
    797c:	6023      	str	r3, [r4, #0]
    797e:	18e4      	adds	r4, r4, r3
    7980:	6025      	str	r5, [r4, #0]
    7982:	e003      	b.n	798c <_malloc_r+0x70>
    7984:	6863      	ldr	r3, [r4, #4]
    7986:	42a2      	cmp	r2, r4
    7988:	d10e      	bne.n	79a8 <_malloc_r+0x8c>
    798a:	603b      	str	r3, [r7, #0]
    798c:	0030      	movs	r0, r6
    798e:	f001 fc0b 	bl	91a8 <__malloc_unlock>
    7992:	0020      	movs	r0, r4
    7994:	2207      	movs	r2, #7
    7996:	300b      	adds	r0, #11
    7998:	1d23      	adds	r3, r4, #4
    799a:	4390      	bics	r0, r2
    799c:	1ac2      	subs	r2, r0, r3
    799e:	4298      	cmp	r0, r3
    79a0:	d0e6      	beq.n	7970 <_malloc_r+0x54>
    79a2:	1a1b      	subs	r3, r3, r0
    79a4:	50a3      	str	r3, [r4, r2]
    79a6:	e7e3      	b.n	7970 <_malloc_r+0x54>
    79a8:	6053      	str	r3, [r2, #4]
    79aa:	e7ef      	b.n	798c <_malloc_r+0x70>
    79ac:	0022      	movs	r2, r4
    79ae:	6864      	ldr	r4, [r4, #4]
    79b0:	e7c5      	b.n	793e <_malloc_r+0x22>
    79b2:	2303      	movs	r3, #3
    79b4:	1cc4      	adds	r4, r0, #3
    79b6:	439c      	bics	r4, r3
    79b8:	42a0      	cmp	r0, r4
    79ba:	d0e1      	beq.n	7980 <_malloc_r+0x64>
    79bc:	1a21      	subs	r1, r4, r0
    79be:	0030      	movs	r0, r6
    79c0:	f001 f872 	bl	8aa8 <_sbrk_r>
    79c4:	1c43      	adds	r3, r0, #1
    79c6:	d1db      	bne.n	7980 <_malloc_r+0x64>
    79c8:	e7c7      	b.n	795a <_malloc_r+0x3e>
    79ca:	bf00      	nop
    79cc:	20002094 	.word	0x20002094

000079d0 <__ssputs_r>:
    79d0:	b5f0      	push	{r4, r5, r6, r7, lr}
    79d2:	688e      	ldr	r6, [r1, #8]
    79d4:	b085      	sub	sp, #20
    79d6:	0007      	movs	r7, r0
    79d8:	000c      	movs	r4, r1
    79da:	9203      	str	r2, [sp, #12]
    79dc:	9301      	str	r3, [sp, #4]
    79de:	429e      	cmp	r6, r3
    79e0:	d83c      	bhi.n	7a5c <__ssputs_r+0x8c>
    79e2:	f240 4380 	movw	r3, #1152	; 0x480
    79e6:	898a      	ldrh	r2, [r1, #12]
    79e8:	421a      	tst	r2, r3
    79ea:	d034      	beq.n	7a56 <__ssputs_r+0x86>
    79ec:	6909      	ldr	r1, [r1, #16]
    79ee:	6823      	ldr	r3, [r4, #0]
    79f0:	6960      	ldr	r0, [r4, #20]
    79f2:	1a5b      	subs	r3, r3, r1
    79f4:	9302      	str	r3, [sp, #8]
    79f6:	2303      	movs	r3, #3
    79f8:	4343      	muls	r3, r0
    79fa:	0fdd      	lsrs	r5, r3, #31
    79fc:	18ed      	adds	r5, r5, r3
    79fe:	9b01      	ldr	r3, [sp, #4]
    7a00:	9802      	ldr	r0, [sp, #8]
    7a02:	3301      	adds	r3, #1
    7a04:	181b      	adds	r3, r3, r0
    7a06:	106d      	asrs	r5, r5, #1
    7a08:	42ab      	cmp	r3, r5
    7a0a:	d900      	bls.n	7a0e <__ssputs_r+0x3e>
    7a0c:	001d      	movs	r5, r3
    7a0e:	0553      	lsls	r3, r2, #21
    7a10:	d532      	bpl.n	7a78 <__ssputs_r+0xa8>
    7a12:	0029      	movs	r1, r5
    7a14:	0038      	movs	r0, r7
    7a16:	f7ff ff81 	bl	791c <_malloc_r>
    7a1a:	0006      	movs	r6, r0
    7a1c:	b948      	cbnz	r0, 7a32 <__ssputs_r+0x62>
    7a1e:	230c      	movs	r3, #12
    7a20:	603b      	str	r3, [r7, #0]
    7a22:	2340      	movs	r3, #64	; 0x40
    7a24:	2001      	movs	r0, #1
    7a26:	89a2      	ldrh	r2, [r4, #12]
    7a28:	4240      	negs	r0, r0
    7a2a:	4313      	orrs	r3, r2
    7a2c:	81a3      	strh	r3, [r4, #12]
    7a2e:	b005      	add	sp, #20
    7a30:	bdf0      	pop	{r4, r5, r6, r7, pc}
    7a32:	9a02      	ldr	r2, [sp, #8]
    7a34:	6921      	ldr	r1, [r4, #16]
    7a36:	f001 fb92 	bl	915e <memcpy>
    7a3a:	89a3      	ldrh	r3, [r4, #12]
    7a3c:	4a14      	ldr	r2, [pc, #80]	; (7a90 <__ssputs_r+0xc0>)
    7a3e:	401a      	ands	r2, r3
    7a40:	2380      	movs	r3, #128	; 0x80
    7a42:	4313      	orrs	r3, r2
    7a44:	81a3      	strh	r3, [r4, #12]
    7a46:	9b02      	ldr	r3, [sp, #8]
    7a48:	6126      	str	r6, [r4, #16]
    7a4a:	18f6      	adds	r6, r6, r3
    7a4c:	6026      	str	r6, [r4, #0]
    7a4e:	6165      	str	r5, [r4, #20]
    7a50:	9e01      	ldr	r6, [sp, #4]
    7a52:	1aed      	subs	r5, r5, r3
    7a54:	60a5      	str	r5, [r4, #8]
    7a56:	9b01      	ldr	r3, [sp, #4]
    7a58:	429e      	cmp	r6, r3
    7a5a:	d900      	bls.n	7a5e <__ssputs_r+0x8e>
    7a5c:	9e01      	ldr	r6, [sp, #4]
    7a5e:	0032      	movs	r2, r6
    7a60:	9903      	ldr	r1, [sp, #12]
    7a62:	6820      	ldr	r0, [r4, #0]
    7a64:	f001 fb84 	bl	9170 <memmove>
    7a68:	68a3      	ldr	r3, [r4, #8]
    7a6a:	2000      	movs	r0, #0
    7a6c:	1b9b      	subs	r3, r3, r6
    7a6e:	60a3      	str	r3, [r4, #8]
    7a70:	6823      	ldr	r3, [r4, #0]
    7a72:	199e      	adds	r6, r3, r6
    7a74:	6026      	str	r6, [r4, #0]
    7a76:	e7da      	b.n	7a2e <__ssputs_r+0x5e>
    7a78:	002a      	movs	r2, r5
    7a7a:	0038      	movs	r0, r7
    7a7c:	f001 fb9c 	bl	91b8 <_realloc_r>
    7a80:	0006      	movs	r6, r0
    7a82:	2800      	cmp	r0, #0
    7a84:	d1df      	bne.n	7a46 <__ssputs_r+0x76>
    7a86:	0038      	movs	r0, r7
    7a88:	6921      	ldr	r1, [r4, #16]
    7a8a:	f7ff ff01 	bl	7890 <_free_r>
    7a8e:	e7c6      	b.n	7a1e <__ssputs_r+0x4e>
    7a90:	fffffb7f 	.word	0xfffffb7f

00007a94 <__ssprint_r>:
    7a94:	b5f0      	push	{r4, r5, r6, r7, lr}
    7a96:	6893      	ldr	r3, [r2, #8]
    7a98:	b087      	sub	sp, #28
    7a9a:	000c      	movs	r4, r1
    7a9c:	0017      	movs	r7, r2
    7a9e:	9002      	str	r0, [sp, #8]
    7aa0:	2b00      	cmp	r3, #0
    7aa2:	d065      	beq.n	7b70 <__ssprint_r+0xdc>
    7aa4:	6813      	ldr	r3, [r2, #0]
    7aa6:	9303      	str	r3, [sp, #12]
    7aa8:	2300      	movs	r3, #0
    7aaa:	9305      	str	r3, [sp, #20]
    7aac:	9301      	str	r3, [sp, #4]
    7aae:	9b01      	ldr	r3, [sp, #4]
    7ab0:	b36b      	cbz	r3, 7b0e <__ssprint_r+0x7a>
    7ab2:	68a6      	ldr	r6, [r4, #8]
    7ab4:	429e      	cmp	r6, r3
    7ab6:	d849      	bhi.n	7b4c <__ssprint_r+0xb8>
    7ab8:	f240 4380 	movw	r3, #1152	; 0x480
    7abc:	89a2      	ldrh	r2, [r4, #12]
    7abe:	421a      	tst	r2, r3
    7ac0:	d041      	beq.n	7b46 <__ssprint_r+0xb2>
    7ac2:	2003      	movs	r0, #3
    7ac4:	6921      	ldr	r1, [r4, #16]
    7ac6:	6823      	ldr	r3, [r4, #0]
    7ac8:	1a5b      	subs	r3, r3, r1
    7aca:	9304      	str	r3, [sp, #16]
    7acc:	6963      	ldr	r3, [r4, #20]
    7ace:	4343      	muls	r3, r0
    7ad0:	0fdd      	lsrs	r5, r3, #31
    7ad2:	18ed      	adds	r5, r5, r3
    7ad4:	9b04      	ldr	r3, [sp, #16]
    7ad6:	9801      	ldr	r0, [sp, #4]
    7ad8:	3301      	adds	r3, #1
    7ada:	181b      	adds	r3, r3, r0
    7adc:	106d      	asrs	r5, r5, #1
    7ade:	42ab      	cmp	r3, r5
    7ae0:	d900      	bls.n	7ae4 <__ssprint_r+0x50>
    7ae2:	001d      	movs	r5, r3
    7ae4:	0553      	lsls	r3, r2, #21
    7ae6:	d547      	bpl.n	7b78 <__ssprint_r+0xe4>
    7ae8:	0029      	movs	r1, r5
    7aea:	9802      	ldr	r0, [sp, #8]
    7aec:	f7ff ff16 	bl	791c <_malloc_r>
    7af0:	0006      	movs	r6, r0
    7af2:	b9b0      	cbnz	r0, 7b22 <__ssprint_r+0x8e>
    7af4:	230c      	movs	r3, #12
    7af6:	9a02      	ldr	r2, [sp, #8]
    7af8:	2001      	movs	r0, #1
    7afa:	6013      	str	r3, [r2, #0]
    7afc:	2340      	movs	r3, #64	; 0x40
    7afe:	89a2      	ldrh	r2, [r4, #12]
    7b00:	4240      	negs	r0, r0
    7b02:	4313      	orrs	r3, r2
    7b04:	81a3      	strh	r3, [r4, #12]
    7b06:	2300      	movs	r3, #0
    7b08:	60bb      	str	r3, [r7, #8]
    7b0a:	607b      	str	r3, [r7, #4]
    7b0c:	e032      	b.n	7b74 <__ssprint_r+0xe0>
    7b0e:	9b03      	ldr	r3, [sp, #12]
    7b10:	681b      	ldr	r3, [r3, #0]
    7b12:	9305      	str	r3, [sp, #20]
    7b14:	9b03      	ldr	r3, [sp, #12]
    7b16:	685b      	ldr	r3, [r3, #4]
    7b18:	9301      	str	r3, [sp, #4]
    7b1a:	9b03      	ldr	r3, [sp, #12]
    7b1c:	3308      	adds	r3, #8
    7b1e:	9303      	str	r3, [sp, #12]
    7b20:	e7c5      	b.n	7aae <__ssprint_r+0x1a>
    7b22:	9a04      	ldr	r2, [sp, #16]
    7b24:	6921      	ldr	r1, [r4, #16]
    7b26:	f001 fb1a 	bl	915e <memcpy>
    7b2a:	89a3      	ldrh	r3, [r4, #12]
    7b2c:	4a18      	ldr	r2, [pc, #96]	; (7b90 <__ssprint_r+0xfc>)
    7b2e:	401a      	ands	r2, r3
    7b30:	2380      	movs	r3, #128	; 0x80
    7b32:	4313      	orrs	r3, r2
    7b34:	81a3      	strh	r3, [r4, #12]
    7b36:	9b04      	ldr	r3, [sp, #16]
    7b38:	6126      	str	r6, [r4, #16]
    7b3a:	18f6      	adds	r6, r6, r3
    7b3c:	6026      	str	r6, [r4, #0]
    7b3e:	6165      	str	r5, [r4, #20]
    7b40:	9e01      	ldr	r6, [sp, #4]
    7b42:	1aed      	subs	r5, r5, r3
    7b44:	60a5      	str	r5, [r4, #8]
    7b46:	9b01      	ldr	r3, [sp, #4]
    7b48:	429e      	cmp	r6, r3
    7b4a:	d900      	bls.n	7b4e <__ssprint_r+0xba>
    7b4c:	9e01      	ldr	r6, [sp, #4]
    7b4e:	0032      	movs	r2, r6
    7b50:	9905      	ldr	r1, [sp, #20]
    7b52:	6820      	ldr	r0, [r4, #0]
    7b54:	f001 fb0c 	bl	9170 <memmove>
    7b58:	68a3      	ldr	r3, [r4, #8]
    7b5a:	9a01      	ldr	r2, [sp, #4]
    7b5c:	1b9b      	subs	r3, r3, r6
    7b5e:	60a3      	str	r3, [r4, #8]
    7b60:	6823      	ldr	r3, [r4, #0]
    7b62:	199e      	adds	r6, r3, r6
    7b64:	68bb      	ldr	r3, [r7, #8]
    7b66:	6026      	str	r6, [r4, #0]
    7b68:	1a9b      	subs	r3, r3, r2
    7b6a:	60bb      	str	r3, [r7, #8]
    7b6c:	2b00      	cmp	r3, #0
    7b6e:	d1ce      	bne.n	7b0e <__ssprint_r+0x7a>
    7b70:	2000      	movs	r0, #0
    7b72:	6078      	str	r0, [r7, #4]
    7b74:	b007      	add	sp, #28
    7b76:	bdf0      	pop	{r4, r5, r6, r7, pc}
    7b78:	002a      	movs	r2, r5
    7b7a:	9802      	ldr	r0, [sp, #8]
    7b7c:	f001 fb1c 	bl	91b8 <_realloc_r>
    7b80:	0006      	movs	r6, r0
    7b82:	2800      	cmp	r0, #0
    7b84:	d1d7      	bne.n	7b36 <__ssprint_r+0xa2>
    7b86:	6921      	ldr	r1, [r4, #16]
    7b88:	9802      	ldr	r0, [sp, #8]
    7b8a:	f7ff fe81 	bl	7890 <_free_r>
    7b8e:	e7b1      	b.n	7af4 <__ssprint_r+0x60>
    7b90:	fffffb7f 	.word	0xfffffb7f

00007b94 <_svfiprintf_r>:
    7b94:	b5f0      	push	{r4, r5, r6, r7, lr}
    7b96:	b0a1      	sub	sp, #132	; 0x84
    7b98:	9003      	str	r0, [sp, #12]
    7b9a:	001d      	movs	r5, r3
    7b9c:	898b      	ldrh	r3, [r1, #12]
    7b9e:	000f      	movs	r7, r1
    7ba0:	0016      	movs	r6, r2
    7ba2:	061b      	lsls	r3, r3, #24
    7ba4:	d50f      	bpl.n	7bc6 <_svfiprintf_r+0x32>
    7ba6:	690b      	ldr	r3, [r1, #16]
    7ba8:	b96b      	cbnz	r3, 7bc6 <_svfiprintf_r+0x32>
    7baa:	2140      	movs	r1, #64	; 0x40
    7bac:	f7ff feb6 	bl	791c <_malloc_r>
    7bb0:	6038      	str	r0, [r7, #0]
    7bb2:	6138      	str	r0, [r7, #16]
    7bb4:	b928      	cbnz	r0, 7bc2 <_svfiprintf_r+0x2e>
    7bb6:	230c      	movs	r3, #12
    7bb8:	9a03      	ldr	r2, [sp, #12]
    7bba:	3801      	subs	r0, #1
    7bbc:	6013      	str	r3, [r2, #0]
    7bbe:	b021      	add	sp, #132	; 0x84
    7bc0:	bdf0      	pop	{r4, r5, r6, r7, pc}
    7bc2:	2340      	movs	r3, #64	; 0x40
    7bc4:	617b      	str	r3, [r7, #20]
    7bc6:	2300      	movs	r3, #0
    7bc8:	ac08      	add	r4, sp, #32
    7bca:	6163      	str	r3, [r4, #20]
    7bcc:	3320      	adds	r3, #32
    7bce:	7663      	strb	r3, [r4, #25]
    7bd0:	3310      	adds	r3, #16
    7bd2:	76a3      	strb	r3, [r4, #26]
    7bd4:	9507      	str	r5, [sp, #28]
    7bd6:	0035      	movs	r5, r6
    7bd8:	782b      	ldrb	r3, [r5, #0]
    7bda:	b10b      	cbz	r3, 7be0 <_svfiprintf_r+0x4c>
    7bdc:	2b25      	cmp	r3, #37	; 0x25
    7bde:	d147      	bne.n	7c70 <_svfiprintf_r+0xdc>
    7be0:	1bab      	subs	r3, r5, r6
    7be2:	9305      	str	r3, [sp, #20]
    7be4:	42b5      	cmp	r5, r6
    7be6:	d00c      	beq.n	7c02 <_svfiprintf_r+0x6e>
    7be8:	0032      	movs	r2, r6
    7bea:	0039      	movs	r1, r7
    7bec:	9803      	ldr	r0, [sp, #12]
    7bee:	f7ff feef 	bl	79d0 <__ssputs_r>
    7bf2:	1c43      	adds	r3, r0, #1
    7bf4:	d100      	bne.n	7bf8 <_svfiprintf_r+0x64>
    7bf6:	e0b0      	b.n	7d5a <_svfiprintf_r+0x1c6>
    7bf8:	6962      	ldr	r2, [r4, #20]
    7bfa:	9b05      	ldr	r3, [sp, #20]
    7bfc:	4694      	mov	ip, r2
    7bfe:	4463      	add	r3, ip
    7c00:	6163      	str	r3, [r4, #20]
    7c02:	782b      	ldrb	r3, [r5, #0]
    7c04:	2b00      	cmp	r3, #0
    7c06:	d100      	bne.n	7c0a <_svfiprintf_r+0x76>
    7c08:	e0a7      	b.n	7d5a <_svfiprintf_r+0x1c6>
    7c0a:	2201      	movs	r2, #1
    7c0c:	2300      	movs	r3, #0
    7c0e:	4252      	negs	r2, r2
    7c10:	6062      	str	r2, [r4, #4]
    7c12:	a904      	add	r1, sp, #16
    7c14:	3254      	adds	r2, #84	; 0x54
    7c16:	1852      	adds	r2, r2, r1
    7c18:	1c6e      	adds	r6, r5, #1
    7c1a:	6023      	str	r3, [r4, #0]
    7c1c:	60e3      	str	r3, [r4, #12]
    7c1e:	60a3      	str	r3, [r4, #8]
    7c20:	7013      	strb	r3, [r2, #0]
    7c22:	65a3      	str	r3, [r4, #88]	; 0x58
    7c24:	2205      	movs	r2, #5
    7c26:	7831      	ldrb	r1, [r6, #0]
    7c28:	4854      	ldr	r0, [pc, #336]	; (7d7c <_svfiprintf_r+0x1e8>)
    7c2a:	f001 fa8d 	bl	9148 <memchr>
    7c2e:	1c75      	adds	r5, r6, #1
    7c30:	bb00      	cbnz	r0, 7c74 <_svfiprintf_r+0xe0>
    7c32:	6822      	ldr	r2, [r4, #0]
    7c34:	06d3      	lsls	r3, r2, #27
    7c36:	d504      	bpl.n	7c42 <_svfiprintf_r+0xae>
    7c38:	2353      	movs	r3, #83	; 0x53
    7c3a:	a904      	add	r1, sp, #16
    7c3c:	185b      	adds	r3, r3, r1
    7c3e:	2120      	movs	r1, #32
    7c40:	7019      	strb	r1, [r3, #0]
    7c42:	0713      	lsls	r3, r2, #28
    7c44:	d504      	bpl.n	7c50 <_svfiprintf_r+0xbc>
    7c46:	2353      	movs	r3, #83	; 0x53
    7c48:	a904      	add	r1, sp, #16
    7c4a:	185b      	adds	r3, r3, r1
    7c4c:	212b      	movs	r1, #43	; 0x2b
    7c4e:	7019      	strb	r1, [r3, #0]
    7c50:	7833      	ldrb	r3, [r6, #0]
    7c52:	2b2a      	cmp	r3, #42	; 0x2a
    7c54:	d017      	beq.n	7c86 <_svfiprintf_r+0xf2>
    7c56:	0035      	movs	r5, r6
    7c58:	2100      	movs	r1, #0
    7c5a:	f240 0c0a 	movw	ip, #10
    7c5e:	2601      	movs	r6, #1
    7c60:	68e3      	ldr	r3, [r4, #12]
    7c62:	782a      	ldrb	r2, [r5, #0]
    7c64:	1c68      	adds	r0, r5, #1
    7c66:	3a30      	subs	r2, #48	; 0x30
    7c68:	2a09      	cmp	r2, #9
    7c6a:	d94b      	bls.n	7d04 <_svfiprintf_r+0x170>
    7c6c:	b989      	cbnz	r1, 7c92 <_svfiprintf_r+0xfe>
    7c6e:	e017      	b.n	7ca0 <_svfiprintf_r+0x10c>
    7c70:	3501      	adds	r5, #1
    7c72:	e7b1      	b.n	7bd8 <_svfiprintf_r+0x44>
    7c74:	4b41      	ldr	r3, [pc, #260]	; (7d7c <_svfiprintf_r+0x1e8>)
    7c76:	6822      	ldr	r2, [r4, #0]
    7c78:	1ac0      	subs	r0, r0, r3
    7c7a:	2301      	movs	r3, #1
    7c7c:	4083      	lsls	r3, r0
    7c7e:	4313      	orrs	r3, r2
    7c80:	002e      	movs	r6, r5
    7c82:	6023      	str	r3, [r4, #0]
    7c84:	e7ce      	b.n	7c24 <_svfiprintf_r+0x90>
    7c86:	9b07      	ldr	r3, [sp, #28]
    7c88:	1d19      	adds	r1, r3, #4
    7c8a:	681b      	ldr	r3, [r3, #0]
    7c8c:	9107      	str	r1, [sp, #28]
    7c8e:	2b00      	cmp	r3, #0
    7c90:	db01      	blt.n	7c96 <_svfiprintf_r+0x102>
    7c92:	930b      	str	r3, [sp, #44]	; 0x2c
    7c94:	e004      	b.n	7ca0 <_svfiprintf_r+0x10c>
    7c96:	425b      	negs	r3, r3
    7c98:	60e3      	str	r3, [r4, #12]
    7c9a:	2302      	movs	r3, #2
    7c9c:	4313      	orrs	r3, r2
    7c9e:	6023      	str	r3, [r4, #0]
    7ca0:	782b      	ldrb	r3, [r5, #0]
    7ca2:	2b2e      	cmp	r3, #46	; 0x2e
    7ca4:	d10a      	bne.n	7cbc <_svfiprintf_r+0x128>
    7ca6:	786b      	ldrb	r3, [r5, #1]
    7ca8:	2b2a      	cmp	r3, #42	; 0x2a
    7caa:	d134      	bne.n	7d16 <_svfiprintf_r+0x182>
    7cac:	9b07      	ldr	r3, [sp, #28]
    7cae:	3502      	adds	r5, #2
    7cb0:	1d1a      	adds	r2, r3, #4
    7cb2:	681b      	ldr	r3, [r3, #0]
    7cb4:	9207      	str	r2, [sp, #28]
    7cb6:	2b00      	cmp	r3, #0
    7cb8:	db2a      	blt.n	7d10 <_svfiprintf_r+0x17c>
    7cba:	9309      	str	r3, [sp, #36]	; 0x24
    7cbc:	4e30      	ldr	r6, [pc, #192]	; (7d80 <_svfiprintf_r+0x1ec>)
    7cbe:	2203      	movs	r2, #3
    7cc0:	0030      	movs	r0, r6
    7cc2:	7829      	ldrb	r1, [r5, #0]
    7cc4:	f001 fa40 	bl	9148 <memchr>
    7cc8:	b130      	cbz	r0, 7cd8 <_svfiprintf_r+0x144>
    7cca:	2340      	movs	r3, #64	; 0x40
    7ccc:	1b80      	subs	r0, r0, r6
    7cce:	4083      	lsls	r3, r0
    7cd0:	6822      	ldr	r2, [r4, #0]
    7cd2:	3501      	adds	r5, #1
    7cd4:	4313      	orrs	r3, r2
    7cd6:	6023      	str	r3, [r4, #0]
    7cd8:	7829      	ldrb	r1, [r5, #0]
    7cda:	2206      	movs	r2, #6
    7cdc:	4829      	ldr	r0, [pc, #164]	; (7d84 <_svfiprintf_r+0x1f0>)
    7cde:	1c6e      	adds	r6, r5, #1
    7ce0:	7621      	strb	r1, [r4, #24]
    7ce2:	f001 fa31 	bl	9148 <memchr>
    7ce6:	2800      	cmp	r0, #0
    7ce8:	d03f      	beq.n	7d6a <_svfiprintf_r+0x1d6>
    7cea:	4b27      	ldr	r3, [pc, #156]	; (7d88 <_svfiprintf_r+0x1f4>)
    7cec:	bb4b      	cbnz	r3, 7d42 <_svfiprintf_r+0x1ae>
    7cee:	2207      	movs	r2, #7
    7cf0:	9b07      	ldr	r3, [sp, #28]
    7cf2:	3307      	adds	r3, #7
    7cf4:	4393      	bics	r3, r2
    7cf6:	3308      	adds	r3, #8
    7cf8:	9307      	str	r3, [sp, #28]
    7cfa:	6963      	ldr	r3, [r4, #20]
    7cfc:	9a04      	ldr	r2, [sp, #16]
    7cfe:	189b      	adds	r3, r3, r2
    7d00:	6163      	str	r3, [r4, #20]
    7d02:	e768      	b.n	7bd6 <_svfiprintf_r+0x42>
    7d04:	4661      	mov	r1, ip
    7d06:	434b      	muls	r3, r1
    7d08:	0005      	movs	r5, r0
    7d0a:	0031      	movs	r1, r6
    7d0c:	189b      	adds	r3, r3, r2
    7d0e:	e7a8      	b.n	7c62 <_svfiprintf_r+0xce>
    7d10:	2301      	movs	r3, #1
    7d12:	425b      	negs	r3, r3
    7d14:	e7d1      	b.n	7cba <_svfiprintf_r+0x126>
    7d16:	2300      	movs	r3, #0
    7d18:	f240 0c0a 	movw	ip, #10
    7d1c:	001a      	movs	r2, r3
    7d1e:	2601      	movs	r6, #1
    7d20:	3501      	adds	r5, #1
    7d22:	6063      	str	r3, [r4, #4]
    7d24:	7829      	ldrb	r1, [r5, #0]
    7d26:	1c68      	adds	r0, r5, #1
    7d28:	3930      	subs	r1, #48	; 0x30
    7d2a:	2909      	cmp	r1, #9
    7d2c:	d903      	bls.n	7d36 <_svfiprintf_r+0x1a2>
    7d2e:	2b00      	cmp	r3, #0
    7d30:	d0c4      	beq.n	7cbc <_svfiprintf_r+0x128>
    7d32:	9209      	str	r2, [sp, #36]	; 0x24
    7d34:	e7c2      	b.n	7cbc <_svfiprintf_r+0x128>
    7d36:	4663      	mov	r3, ip
    7d38:	435a      	muls	r2, r3
    7d3a:	0005      	movs	r5, r0
    7d3c:	0033      	movs	r3, r6
    7d3e:	1852      	adds	r2, r2, r1
    7d40:	e7f0      	b.n	7d24 <_svfiprintf_r+0x190>
    7d42:	ab07      	add	r3, sp, #28
    7d44:	9300      	str	r3, [sp, #0]
    7d46:	003a      	movs	r2, r7
    7d48:	0021      	movs	r1, r4
    7d4a:	4b10      	ldr	r3, [pc, #64]	; (7d8c <_svfiprintf_r+0x1f8>)
    7d4c:	9803      	ldr	r0, [sp, #12]
    7d4e:	e000      	b.n	7d52 <_svfiprintf_r+0x1be>
    7d50:	bf00      	nop
    7d52:	9004      	str	r0, [sp, #16]
    7d54:	9b04      	ldr	r3, [sp, #16]
    7d56:	3301      	adds	r3, #1
    7d58:	d1cf      	bne.n	7cfa <_svfiprintf_r+0x166>
    7d5a:	89bb      	ldrh	r3, [r7, #12]
    7d5c:	980d      	ldr	r0, [sp, #52]	; 0x34
    7d5e:	065b      	lsls	r3, r3, #25
    7d60:	d400      	bmi.n	7d64 <_svfiprintf_r+0x1d0>
    7d62:	e72c      	b.n	7bbe <_svfiprintf_r+0x2a>
    7d64:	2001      	movs	r0, #1
    7d66:	4240      	negs	r0, r0
    7d68:	e729      	b.n	7bbe <_svfiprintf_r+0x2a>
    7d6a:	ab07      	add	r3, sp, #28
    7d6c:	9300      	str	r3, [sp, #0]
    7d6e:	003a      	movs	r2, r7
    7d70:	0021      	movs	r1, r4
    7d72:	4b06      	ldr	r3, [pc, #24]	; (7d8c <_svfiprintf_r+0x1f8>)
    7d74:	9803      	ldr	r0, [sp, #12]
    7d76:	f000 fa4d 	bl	8214 <_printf_i>
    7d7a:	e7ea      	b.n	7d52 <_svfiprintf_r+0x1be>
    7d7c:	00009784 	.word	0x00009784
    7d80:	0000978a 	.word	0x0000978a
    7d84:	0000978e 	.word	0x0000978e
    7d88:	00000000 	.word	0x00000000
    7d8c:	000079d1 	.word	0x000079d1

00007d90 <_sungetc_r>:
    7d90:	b570      	push	{r4, r5, r6, lr}
    7d92:	0014      	movs	r4, r2
    7d94:	1c4b      	adds	r3, r1, #1
    7d96:	d103      	bne.n	7da0 <_sungetc_r+0x10>
    7d98:	2501      	movs	r5, #1
    7d9a:	426d      	negs	r5, r5
    7d9c:	0028      	movs	r0, r5
    7d9e:	bd70      	pop	{r4, r5, r6, pc}
    7da0:	8993      	ldrh	r3, [r2, #12]
    7da2:	2220      	movs	r2, #32
    7da4:	4393      	bics	r3, r2
    7da6:	6b62      	ldr	r2, [r4, #52]	; 0x34
    7da8:	81a3      	strh	r3, [r4, #12]
    7daa:	b2ce      	uxtb	r6, r1
    7dac:	6863      	ldr	r3, [r4, #4]
    7dae:	b2cd      	uxtb	r5, r1
    7db0:	b182      	cbz	r2, 7dd4 <_sungetc_r+0x44>
    7db2:	6ba2      	ldr	r2, [r4, #56]	; 0x38
    7db4:	429a      	cmp	r2, r3
    7db6:	dd07      	ble.n	7dc8 <_sungetc_r+0x38>
    7db8:	6823      	ldr	r3, [r4, #0]
    7dba:	3b01      	subs	r3, #1
    7dbc:	6023      	str	r3, [r4, #0]
    7dbe:	701e      	strb	r6, [r3, #0]
    7dc0:	6863      	ldr	r3, [r4, #4]
    7dc2:	3301      	adds	r3, #1
    7dc4:	6063      	str	r3, [r4, #4]
    7dc6:	e7e9      	b.n	7d9c <_sungetc_r+0xc>
    7dc8:	0021      	movs	r1, r4
    7dca:	f000 ff51 	bl	8c70 <__submore>
    7dce:	2800      	cmp	r0, #0
    7dd0:	d0f2      	beq.n	7db8 <_sungetc_r+0x28>
    7dd2:	e7e1      	b.n	7d98 <_sungetc_r+0x8>
    7dd4:	6921      	ldr	r1, [r4, #16]
    7dd6:	6822      	ldr	r2, [r4, #0]
    7dd8:	b139      	cbz	r1, 7dea <_sungetc_r+0x5a>
    7dda:	4291      	cmp	r1, r2
    7ddc:	d205      	bcs.n	7dea <_sungetc_r+0x5a>
    7dde:	1e51      	subs	r1, r2, #1
    7de0:	7808      	ldrb	r0, [r1, #0]
    7de2:	42a8      	cmp	r0, r5
    7de4:	d101      	bne.n	7dea <_sungetc_r+0x5a>
    7de6:	6021      	str	r1, [r4, #0]
    7de8:	e7eb      	b.n	7dc2 <_sungetc_r+0x32>
    7dea:	6423      	str	r3, [r4, #64]	; 0x40
    7dec:	0023      	movs	r3, r4
    7dee:	3344      	adds	r3, #68	; 0x44
    7df0:	6363      	str	r3, [r4, #52]	; 0x34
    7df2:	2303      	movs	r3, #3
    7df4:	63a3      	str	r3, [r4, #56]	; 0x38
    7df6:	0023      	movs	r3, r4
    7df8:	3346      	adds	r3, #70	; 0x46
    7dfa:	63e2      	str	r2, [r4, #60]	; 0x3c
    7dfc:	701e      	strb	r6, [r3, #0]
    7dfe:	6023      	str	r3, [r4, #0]
    7e00:	2301      	movs	r3, #1
    7e02:	e7df      	b.n	7dc4 <_sungetc_r+0x34>

00007e04 <__ssrefill_r>:
    7e04:	b510      	push	{r4, lr}
    7e06:	000c      	movs	r4, r1
    7e08:	6b49      	ldr	r1, [r1, #52]	; 0x34
    7e0a:	b169      	cbz	r1, 7e28 <__ssrefill_r+0x24>
    7e0c:	0023      	movs	r3, r4
    7e0e:	3344      	adds	r3, #68	; 0x44
    7e10:	4299      	cmp	r1, r3
    7e12:	d001      	beq.n	7e18 <__ssrefill_r+0x14>
    7e14:	f7ff fd3c 	bl	7890 <_free_r>
    7e18:	2000      	movs	r0, #0
    7e1a:	6c23      	ldr	r3, [r4, #64]	; 0x40
    7e1c:	6360      	str	r0, [r4, #52]	; 0x34
    7e1e:	6063      	str	r3, [r4, #4]
    7e20:	b113      	cbz	r3, 7e28 <__ssrefill_r+0x24>
    7e22:	6be3      	ldr	r3, [r4, #60]	; 0x3c
    7e24:	6023      	str	r3, [r4, #0]
    7e26:	bd10      	pop	{r4, pc}
    7e28:	6923      	ldr	r3, [r4, #16]
    7e2a:	2001      	movs	r0, #1
    7e2c:	6023      	str	r3, [r4, #0]
    7e2e:	2300      	movs	r3, #0
    7e30:	89a2      	ldrh	r2, [r4, #12]
    7e32:	6063      	str	r3, [r4, #4]
    7e34:	3320      	adds	r3, #32
    7e36:	4313      	orrs	r3, r2
    7e38:	81a3      	strh	r3, [r4, #12]
    7e3a:	4240      	negs	r0, r0
    7e3c:	e7f3      	b.n	7e26 <__ssrefill_r+0x22>

00007e3e <__ssvfiscanf_r>:
    7e3e:	b5f0      	push	{r4, r5, r6, r7, lr}
    7e40:	4cb7      	ldr	r4, [pc, #732]	; (8120 <__ssvfiscanf_r+0x2e2>)
    7e42:	0006      	movs	r6, r0
    7e44:	44a5      	add	sp, r4
    7e46:	000c      	movs	r4, r1
    7e48:	2100      	movs	r1, #0
    7e4a:	9146      	str	r1, [sp, #280]	; 0x118
    7e4c:	9147      	str	r1, [sp, #284]	; 0x11c
    7e4e:	a903      	add	r1, sp, #12
    7e50:	9148      	str	r1, [sp, #288]	; 0x120
    7e52:	f240 117c 	movw	r1, #380	; 0x17c
    7e56:	48b3      	ldr	r0, [pc, #716]	; (8124 <__ssvfiscanf_r+0x2e6>)
    7e58:	ad43      	add	r5, sp, #268	; 0x10c
    7e5a:	5068      	str	r0, [r5, r1]
    7e5c:	f240 1580 	movw	r5, #384	; 0x180
    7e60:	49b1      	ldr	r1, [pc, #708]	; (8128 <__ssvfiscanf_r+0x2ea>)
    7e62:	a843      	add	r0, sp, #268	; 0x10c
    7e64:	5141      	str	r1, [r0, r5]
    7e66:	9302      	str	r3, [sp, #8]
    7e68:	7813      	ldrb	r3, [r2, #0]
    7e6a:	2b00      	cmp	r3, #0
    7e6c:	d100      	bne.n	7e70 <__ssvfiscanf_r+0x32>
    7e6e:	e154      	b.n	811a <__ssvfiscanf_r+0x2dc>
    7e70:	49ae      	ldr	r1, [pc, #696]	; (812c <__ssvfiscanf_r+0x2ee>)
    7e72:	2508      	movs	r5, #8
    7e74:	5cc8      	ldrb	r0, [r1, r3]
    7e76:	2108      	movs	r1, #8
    7e78:	1c57      	adds	r7, r2, #1
    7e7a:	4001      	ands	r1, r0
    7e7c:	4228      	tst	r0, r5
    7e7e:	d020      	beq.n	7ec2 <__ssvfiscanf_r+0x84>
    7e80:	6863      	ldr	r3, [r4, #4]
    7e82:	2b00      	cmp	r3, #0
    7e84:	dd12      	ble.n	7eac <__ssvfiscanf_r+0x6e>
    7e86:	6823      	ldr	r3, [r4, #0]
    7e88:	49a8      	ldr	r1, [pc, #672]	; (812c <__ssvfiscanf_r+0x2ee>)
    7e8a:	781a      	ldrb	r2, [r3, #0]
    7e8c:	5c8a      	ldrb	r2, [r1, r2]
    7e8e:	2108      	movs	r1, #8
    7e90:	420a      	tst	r2, r1
    7e92:	d101      	bne.n	7e98 <__ssvfiscanf_r+0x5a>
    7e94:	003a      	movs	r2, r7
    7e96:	e7e7      	b.n	7e68 <__ssvfiscanf_r+0x2a>
    7e98:	9a47      	ldr	r2, [sp, #284]	; 0x11c
    7e9a:	3301      	adds	r3, #1
    7e9c:	9200      	str	r2, [sp, #0]
    7e9e:	3201      	adds	r2, #1
    7ea0:	9247      	str	r2, [sp, #284]	; 0x11c
    7ea2:	6862      	ldr	r2, [r4, #4]
    7ea4:	6023      	str	r3, [r4, #0]
    7ea6:	3a01      	subs	r2, #1
    7ea8:	6062      	str	r2, [r4, #4]
    7eaa:	e7e9      	b.n	7e80 <__ssvfiscanf_r+0x42>
    7eac:	f240 1280 	movw	r2, #384	; 0x180
    7eb0:	ab43      	add	r3, sp, #268	; 0x10c
    7eb2:	589b      	ldr	r3, [r3, r2]
    7eb4:	0021      	movs	r1, r4
    7eb6:	0030      	movs	r0, r6
    7eb8:	9300      	str	r3, [sp, #0]
    7eba:	4798      	blx	r3
    7ebc:	2800      	cmp	r0, #0
    7ebe:	d0e2      	beq.n	7e86 <__ssvfiscanf_r+0x48>
    7ec0:	e7e8      	b.n	7e94 <__ssvfiscanf_r+0x56>
    7ec2:	001d      	movs	r5, r3
    7ec4:	2b25      	cmp	r3, #37	; 0x25
    7ec6:	d163      	bne.n	7f90 <__ssvfiscanf_r+0x152>
    7ec8:	9145      	str	r1, [sp, #276]	; 0x114
    7eca:	9143      	str	r1, [sp, #268]	; 0x10c
    7ecc:	7853      	ldrb	r3, [r2, #1]
    7ece:	2b2a      	cmp	r3, #42	; 0x2a
    7ed0:	d102      	bne.n	7ed8 <__ssvfiscanf_r+0x9a>
    7ed2:	3b1a      	subs	r3, #26
    7ed4:	9343      	str	r3, [sp, #268]	; 0x10c
    7ed6:	1c97      	adds	r7, r2, #2
    7ed8:	003d      	movs	r5, r7
    7eda:	220a      	movs	r2, #10
    7edc:	7829      	ldrb	r1, [r5, #0]
    7ede:	1c6b      	adds	r3, r5, #1
    7ee0:	9300      	str	r3, [sp, #0]
    7ee2:	000b      	movs	r3, r1
    7ee4:	3b30      	subs	r3, #48	; 0x30
    7ee6:	2b09      	cmp	r3, #9
    7ee8:	d91e      	bls.n	7f28 <__ssvfiscanf_r+0xea>
    7eea:	4f91      	ldr	r7, [pc, #580]	; (8130 <__ssvfiscanf_r+0x2f2>)
    7eec:	2203      	movs	r2, #3
    7eee:	0038      	movs	r0, r7
    7ef0:	f001 f92a 	bl	9148 <memchr>
    7ef4:	b138      	cbz	r0, 7f06 <__ssvfiscanf_r+0xc8>
    7ef6:	2301      	movs	r3, #1
    7ef8:	1bc0      	subs	r0, r0, r7
    7efa:	4083      	lsls	r3, r0
    7efc:	9a43      	ldr	r2, [sp, #268]	; 0x10c
    7efe:	9d00      	ldr	r5, [sp, #0]
    7f00:	4313      	orrs	r3, r2
    7f02:	9201      	str	r2, [sp, #4]
    7f04:	9343      	str	r3, [sp, #268]	; 0x10c
    7f06:	1c6f      	adds	r7, r5, #1
    7f08:	782d      	ldrb	r5, [r5, #0]
    7f0a:	2d78      	cmp	r5, #120	; 0x78
    7f0c:	d807      	bhi.n	7f1e <__ssvfiscanf_r+0xe0>
    7f0e:	2d57      	cmp	r5, #87	; 0x57
    7f10:	d812      	bhi.n	7f38 <__ssvfiscanf_r+0xfa>
    7f12:	2d25      	cmp	r5, #37	; 0x25
    7f14:	d03c      	beq.n	7f90 <__ssvfiscanf_r+0x152>
    7f16:	d836      	bhi.n	7f86 <__ssvfiscanf_r+0x148>
    7f18:	2d00      	cmp	r5, #0
    7f1a:	d100      	bne.n	7f1e <__ssvfiscanf_r+0xe0>
    7f1c:	e0fa      	b.n	8114 <__ssvfiscanf_r+0x2d6>
    7f1e:	2303      	movs	r3, #3
    7f20:	9349      	str	r3, [sp, #292]	; 0x124
    7f22:	3307      	adds	r3, #7
    7f24:	9344      	str	r3, [sp, #272]	; 0x110
    7f26:	e06d      	b.n	8004 <__ssvfiscanf_r+0x1c6>
    7f28:	9b45      	ldr	r3, [sp, #276]	; 0x114
    7f2a:	9d00      	ldr	r5, [sp, #0]
    7f2c:	9301      	str	r3, [sp, #4]
    7f2e:	4353      	muls	r3, r2
    7f30:	3b30      	subs	r3, #48	; 0x30
    7f32:	185b      	adds	r3, r3, r1
    7f34:	9345      	str	r3, [sp, #276]	; 0x114
    7f36:	e7d1      	b.n	7edc <__ssvfiscanf_r+0x9e>
    7f38:	0028      	movs	r0, r5
    7f3a:	3858      	subs	r0, #88	; 0x58
    7f3c:	2820      	cmp	r0, #32
    7f3e:	d8ee      	bhi.n	7f1e <__ssvfiscanf_r+0xe0>
    7f40:	f001 fa20 	bl	9384 <__gnu_thumb1_case_shi>
    7f44:	ffed0051 	.word	0xffed0051
    7f48:	0087ffed 	.word	0x0087ffed
    7f4c:	ffedffed 	.word	0xffedffed
    7f50:	ffedffed 	.word	0xffedffed
    7f54:	ffedffed 	.word	0xffedffed
    7f58:	0095ffed 	.word	0x0095ffed
    7f5c:	0024007d 	.word	0x0024007d
    7f60:	00240024 	.word	0x00240024
    7f64:	007fffed 	.word	0x007fffed
    7f68:	ffedffed 	.word	0xffedffed
    7f6c:	ffedffed 	.word	0xffedffed
    7f70:	0083009f 	.word	0x0083009f
    7f74:	ffed004b 	.word	0xffed004b
    7f78:	009dffed 	.word	0x009dffed
    7f7c:	007dffed 	.word	0x007dffed
    7f80:	ffedffed 	.word	0xffedffed
    7f84:	0051      	.short	0x0051
    7f86:	3d45      	subs	r5, #69	; 0x45
    7f88:	2d02      	cmp	r5, #2
    7f8a:	d8c8      	bhi.n	7f1e <__ssvfiscanf_r+0xe0>
    7f8c:	2305      	movs	r3, #5
    7f8e:	e06c      	b.n	806a <__ssvfiscanf_r+0x22c>
    7f90:	6863      	ldr	r3, [r4, #4]
    7f92:	2b00      	cmp	r3, #0
    7f94:	dd0e      	ble.n	7fb4 <__ssvfiscanf_r+0x176>
    7f96:	6823      	ldr	r3, [r4, #0]
    7f98:	781a      	ldrb	r2, [r3, #0]
    7f9a:	42aa      	cmp	r2, r5
    7f9c:	d000      	beq.n	7fa0 <__ssvfiscanf_r+0x162>
    7f9e:	e0bc      	b.n	811a <__ssvfiscanf_r+0x2dc>
    7fa0:	3301      	adds	r3, #1
    7fa2:	6862      	ldr	r2, [r4, #4]
    7fa4:	6023      	str	r3, [r4, #0]
    7fa6:	9b47      	ldr	r3, [sp, #284]	; 0x11c
    7fa8:	3a01      	subs	r2, #1
    7faa:	9300      	str	r3, [sp, #0]
    7fac:	3301      	adds	r3, #1
    7fae:	6062      	str	r2, [r4, #4]
    7fb0:	9347      	str	r3, [sp, #284]	; 0x11c
    7fb2:	e76f      	b.n	7e94 <__ssvfiscanf_r+0x56>
    7fb4:	f240 1280 	movw	r2, #384	; 0x180
    7fb8:	ab43      	add	r3, sp, #268	; 0x10c
    7fba:	589b      	ldr	r3, [r3, r2]
    7fbc:	0021      	movs	r1, r4
    7fbe:	0030      	movs	r0, r6
    7fc0:	9300      	str	r3, [sp, #0]
    7fc2:	4798      	blx	r3
    7fc4:	2800      	cmp	r0, #0
    7fc6:	d0e6      	beq.n	7f96 <__ssvfiscanf_r+0x158>
    7fc8:	9846      	ldr	r0, [sp, #280]	; 0x118
    7fca:	2800      	cmp	r0, #0
    7fcc:	d000      	beq.n	7fd0 <__ssvfiscanf_r+0x192>
    7fce:	e09d      	b.n	810c <__ssvfiscanf_r+0x2ce>
    7fd0:	3801      	subs	r0, #1
    7fd2:	f240 2394 	movw	r3, #660	; 0x294
    7fd6:	449d      	add	sp, r3
    7fd8:	bdf0      	pop	{r4, r5, r6, r7, pc}
    7fda:	9b43      	ldr	r3, [sp, #268]	; 0x10c
    7fdc:	9300      	str	r3, [sp, #0]
    7fde:	2320      	movs	r3, #32
    7fe0:	9a00      	ldr	r2, [sp, #0]
    7fe2:	4313      	orrs	r3, r2
    7fe4:	9343      	str	r3, [sp, #268]	; 0x10c
    7fe6:	9b43      	ldr	r3, [sp, #268]	; 0x10c
    7fe8:	9300      	str	r3, [sp, #0]
    7fea:	f240 2300 	movw	r3, #512	; 0x200
    7fee:	9a00      	ldr	r2, [sp, #0]
    7ff0:	4313      	orrs	r3, r2
    7ff2:	9343      	str	r3, [sp, #268]	; 0x10c
    7ff4:	2310      	movs	r3, #16
    7ff6:	9344      	str	r3, [sp, #272]	; 0x110
    7ff8:	236e      	movs	r3, #110	; 0x6e
    7ffa:	42ab      	cmp	r3, r5
    7ffc:	41ad      	sbcs	r5, r5
    7ffe:	426d      	negs	r5, r5
    8000:	3503      	adds	r5, #3
    8002:	9549      	str	r5, [sp, #292]	; 0x124
    8004:	6863      	ldr	r3, [r4, #4]
    8006:	2b00      	cmp	r3, #0
    8008:	dd4a      	ble.n	80a0 <__ssvfiscanf_r+0x262>
    800a:	9b43      	ldr	r3, [sp, #268]	; 0x10c
    800c:	9300      	str	r3, [sp, #0]
    800e:	065b      	lsls	r3, r3, #25
    8010:	d406      	bmi.n	8020 <__ssvfiscanf_r+0x1e2>
    8012:	6823      	ldr	r3, [r4, #0]
    8014:	4945      	ldr	r1, [pc, #276]	; (812c <__ssvfiscanf_r+0x2ee>)
    8016:	781a      	ldrb	r2, [r3, #0]
    8018:	5c8a      	ldrb	r2, [r1, r2]
    801a:	2108      	movs	r1, #8
    801c:	420a      	tst	r2, r1
    801e:	d14a      	bne.n	80b6 <__ssvfiscanf_r+0x278>
    8020:	9b49      	ldr	r3, [sp, #292]	; 0x124
    8022:	2b02      	cmp	r3, #2
    8024:	dc5e      	bgt.n	80e4 <__ssvfiscanf_r+0x2a6>
    8026:	0022      	movs	r2, r4
    8028:	0030      	movs	r0, r6
    802a:	ab02      	add	r3, sp, #8
    802c:	a943      	add	r1, sp, #268	; 0x10c
    802e:	f000 fb6f 	bl	8710 <_scanf_chars>
    8032:	2801      	cmp	r0, #1
    8034:	d071      	beq.n	811a <__ssvfiscanf_r+0x2dc>
    8036:	2802      	cmp	r0, #2
    8038:	d000      	beq.n	803c <__ssvfiscanf_r+0x1fe>
    803a:	e72b      	b.n	7e94 <__ssvfiscanf_r+0x56>
    803c:	e7c4      	b.n	7fc8 <__ssvfiscanf_r+0x18a>
    803e:	230a      	movs	r3, #10
    8040:	e7d9      	b.n	7ff6 <__ssvfiscanf_r+0x1b8>
    8042:	2300      	movs	r3, #0
    8044:	2503      	movs	r5, #3
    8046:	9344      	str	r3, [sp, #272]	; 0x110
    8048:	e7db      	b.n	8002 <__ssvfiscanf_r+0x1c4>
    804a:	2308      	movs	r3, #8
    804c:	2504      	movs	r5, #4
    804e:	9344      	str	r3, [sp, #272]	; 0x110
    8050:	e7d7      	b.n	8002 <__ssvfiscanf_r+0x1c4>
    8052:	0039      	movs	r1, r7
    8054:	a803      	add	r0, sp, #12
    8056:	f000 fd37 	bl	8ac8 <__sccl>
    805a:	9b43      	ldr	r3, [sp, #268]	; 0x10c
    805c:	0007      	movs	r7, r0
    805e:	9300      	str	r3, [sp, #0]
    8060:	2340      	movs	r3, #64	; 0x40
    8062:	9a00      	ldr	r2, [sp, #0]
    8064:	4313      	orrs	r3, r2
    8066:	9343      	str	r3, [sp, #268]	; 0x10c
    8068:	2301      	movs	r3, #1
    806a:	9349      	str	r3, [sp, #292]	; 0x124
    806c:	e7ca      	b.n	8004 <__ssvfiscanf_r+0x1c6>
    806e:	9b43      	ldr	r3, [sp, #268]	; 0x10c
    8070:	9300      	str	r3, [sp, #0]
    8072:	2340      	movs	r3, #64	; 0x40
    8074:	9a00      	ldr	r2, [sp, #0]
    8076:	4313      	orrs	r3, r2
    8078:	9343      	str	r3, [sp, #268]	; 0x10c
    807a:	2300      	movs	r3, #0
    807c:	e7f5      	b.n	806a <__ssvfiscanf_r+0x22c>
    807e:	2302      	movs	r3, #2
    8080:	e7f3      	b.n	806a <__ssvfiscanf_r+0x22c>
    8082:	9843      	ldr	r0, [sp, #268]	; 0x10c
    8084:	06c3      	lsls	r3, r0, #27
    8086:	d500      	bpl.n	808a <__ssvfiscanf_r+0x24c>
    8088:	e704      	b.n	7e94 <__ssvfiscanf_r+0x56>
    808a:	9b02      	ldr	r3, [sp, #8]
    808c:	9a47      	ldr	r2, [sp, #284]	; 0x11c
    808e:	1d19      	adds	r1, r3, #4
    8090:	9102      	str	r1, [sp, #8]
    8092:	681b      	ldr	r3, [r3, #0]
    8094:	07c5      	lsls	r5, r0, #31
    8096:	d501      	bpl.n	809c <__ssvfiscanf_r+0x25e>
    8098:	801a      	strh	r2, [r3, #0]
    809a:	e6fb      	b.n	7e94 <__ssvfiscanf_r+0x56>
    809c:	601a      	str	r2, [r3, #0]
    809e:	e6f9      	b.n	7e94 <__ssvfiscanf_r+0x56>
    80a0:	f240 1280 	movw	r2, #384	; 0x180
    80a4:	ab43      	add	r3, sp, #268	; 0x10c
    80a6:	589b      	ldr	r3, [r3, r2]
    80a8:	0021      	movs	r1, r4
    80aa:	0030      	movs	r0, r6
    80ac:	9300      	str	r3, [sp, #0]
    80ae:	4798      	blx	r3
    80b0:	2800      	cmp	r0, #0
    80b2:	d0aa      	beq.n	800a <__ssvfiscanf_r+0x1cc>
    80b4:	e788      	b.n	7fc8 <__ssvfiscanf_r+0x18a>
    80b6:	9a47      	ldr	r2, [sp, #284]	; 0x11c
    80b8:	9200      	str	r2, [sp, #0]
    80ba:	3201      	adds	r2, #1
    80bc:	9247      	str	r2, [sp, #284]	; 0x11c
    80be:	6862      	ldr	r2, [r4, #4]
    80c0:	3a01      	subs	r2, #1
    80c2:	6062      	str	r2, [r4, #4]
    80c4:	2a00      	cmp	r2, #0
    80c6:	dd02      	ble.n	80ce <__ssvfiscanf_r+0x290>
    80c8:	3301      	adds	r3, #1
    80ca:	6023      	str	r3, [r4, #0]
    80cc:	e7a1      	b.n	8012 <__ssvfiscanf_r+0x1d4>
    80ce:	f240 1280 	movw	r2, #384	; 0x180
    80d2:	ab43      	add	r3, sp, #268	; 0x10c
    80d4:	589b      	ldr	r3, [r3, r2]
    80d6:	0021      	movs	r1, r4
    80d8:	0030      	movs	r0, r6
    80da:	9300      	str	r3, [sp, #0]
    80dc:	4798      	blx	r3
    80de:	2800      	cmp	r0, #0
    80e0:	d097      	beq.n	8012 <__ssvfiscanf_r+0x1d4>
    80e2:	e771      	b.n	7fc8 <__ssvfiscanf_r+0x18a>
    80e4:	2b04      	cmp	r3, #4
    80e6:	dc06      	bgt.n	80f6 <__ssvfiscanf_r+0x2b8>
    80e8:	0022      	movs	r2, r4
    80ea:	0030      	movs	r0, r6
    80ec:	ab02      	add	r3, sp, #8
    80ee:	a943      	add	r1, sp, #268	; 0x10c
    80f0:	f000 fb66 	bl	87c0 <_scanf_i>
    80f4:	e79d      	b.n	8032 <__ssvfiscanf_r+0x1f4>
    80f6:	4b0f      	ldr	r3, [pc, #60]	; (8134 <__ssvfiscanf_r+0x2f6>)
    80f8:	2b00      	cmp	r3, #0
    80fa:	d100      	bne.n	80fe <__ssvfiscanf_r+0x2c0>
    80fc:	e6ca      	b.n	7e94 <__ssvfiscanf_r+0x56>
    80fe:	0022      	movs	r2, r4
    8100:	0030      	movs	r0, r6
    8102:	ab02      	add	r3, sp, #8
    8104:	a943      	add	r1, sp, #268	; 0x10c
    8106:	e000      	b.n	810a <__ssvfiscanf_r+0x2cc>
    8108:	bf00      	nop
    810a:	e792      	b.n	8032 <__ssvfiscanf_r+0x1f4>
    810c:	89a3      	ldrh	r3, [r4, #12]
    810e:	065b      	lsls	r3, r3, #25
    8110:	d400      	bmi.n	8114 <__ssvfiscanf_r+0x2d6>
    8112:	e75e      	b.n	7fd2 <__ssvfiscanf_r+0x194>
    8114:	2001      	movs	r0, #1
    8116:	4240      	negs	r0, r0
    8118:	e75b      	b.n	7fd2 <__ssvfiscanf_r+0x194>
    811a:	9846      	ldr	r0, [sp, #280]	; 0x118
    811c:	e759      	b.n	7fd2 <__ssvfiscanf_r+0x194>
    811e:	bf00      	nop
    8120:	fffffd6c 	.word	0xfffffd6c
    8124:	00007d91 	.word	0x00007d91
    8128:	00007e05 	.word	0x00007e05
    812c:	0000961d 	.word	0x0000961d
    8130:	0000978a 	.word	0x0000978a
    8134:	00000000 	.word	0x00000000

00008138 <_printf_common>:
    8138:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    813a:	0015      	movs	r5, r2
    813c:	9301      	str	r3, [sp, #4]
    813e:	688a      	ldr	r2, [r1, #8]
    8140:	690b      	ldr	r3, [r1, #16]
    8142:	000c      	movs	r4, r1
    8144:	9000      	str	r0, [sp, #0]
    8146:	4293      	cmp	r3, r2
    8148:	da00      	bge.n	814c <_printf_common+0x14>
    814a:	0013      	movs	r3, r2
    814c:	0022      	movs	r2, r4
    814e:	602b      	str	r3, [r5, #0]
    8150:	3243      	adds	r2, #67	; 0x43
    8152:	7812      	ldrb	r2, [r2, #0]
    8154:	b10a      	cbz	r2, 815a <_printf_common+0x22>
    8156:	3301      	adds	r3, #1
    8158:	602b      	str	r3, [r5, #0]
    815a:	6823      	ldr	r3, [r4, #0]
    815c:	069b      	lsls	r3, r3, #26
    815e:	d502      	bpl.n	8166 <_printf_common+0x2e>
    8160:	682b      	ldr	r3, [r5, #0]
    8162:	3302      	adds	r3, #2
    8164:	602b      	str	r3, [r5, #0]
    8166:	6822      	ldr	r2, [r4, #0]
    8168:	2306      	movs	r3, #6
    816a:	0017      	movs	r7, r2
    816c:	401f      	ands	r7, r3
    816e:	421a      	tst	r2, r3
    8170:	d027      	beq.n	81c2 <_printf_common+0x8a>
    8172:	0023      	movs	r3, r4
    8174:	3343      	adds	r3, #67	; 0x43
    8176:	781b      	ldrb	r3, [r3, #0]
    8178:	1e5a      	subs	r2, r3, #1
    817a:	4193      	sbcs	r3, r2
    817c:	6822      	ldr	r2, [r4, #0]
    817e:	0692      	lsls	r2, r2, #26
    8180:	d430      	bmi.n	81e4 <_printf_common+0xac>
    8182:	0022      	movs	r2, r4
    8184:	9901      	ldr	r1, [sp, #4]
    8186:	9800      	ldr	r0, [sp, #0]
    8188:	9e08      	ldr	r6, [sp, #32]
    818a:	3243      	adds	r2, #67	; 0x43
    818c:	47b0      	blx	r6
    818e:	1c43      	adds	r3, r0, #1
    8190:	d025      	beq.n	81de <_printf_common+0xa6>
    8192:	2306      	movs	r3, #6
    8194:	6820      	ldr	r0, [r4, #0]
    8196:	682a      	ldr	r2, [r5, #0]
    8198:	68e1      	ldr	r1, [r4, #12]
    819a:	2500      	movs	r5, #0
    819c:	4003      	ands	r3, r0
    819e:	2b04      	cmp	r3, #4
    81a0:	d103      	bne.n	81aa <_printf_common+0x72>
    81a2:	1a8d      	subs	r5, r1, r2
    81a4:	43eb      	mvns	r3, r5
    81a6:	17db      	asrs	r3, r3, #31
    81a8:	401d      	ands	r5, r3
    81aa:	68a3      	ldr	r3, [r4, #8]
    81ac:	6922      	ldr	r2, [r4, #16]
    81ae:	4293      	cmp	r3, r2
    81b0:	dd01      	ble.n	81b6 <_printf_common+0x7e>
    81b2:	1a9b      	subs	r3, r3, r2
    81b4:	18ed      	adds	r5, r5, r3
    81b6:	2700      	movs	r7, #0
    81b8:	42bd      	cmp	r5, r7
    81ba:	d120      	bne.n	81fe <_printf_common+0xc6>
    81bc:	2000      	movs	r0, #0
    81be:	e010      	b.n	81e2 <_printf_common+0xaa>
    81c0:	3701      	adds	r7, #1
    81c2:	68e3      	ldr	r3, [r4, #12]
    81c4:	682a      	ldr	r2, [r5, #0]
    81c6:	1a9b      	subs	r3, r3, r2
    81c8:	42bb      	cmp	r3, r7
    81ca:	ddd2      	ble.n	8172 <_printf_common+0x3a>
    81cc:	0022      	movs	r2, r4
    81ce:	2301      	movs	r3, #1
    81d0:	9901      	ldr	r1, [sp, #4]
    81d2:	9800      	ldr	r0, [sp, #0]
    81d4:	9e08      	ldr	r6, [sp, #32]
    81d6:	3219      	adds	r2, #25
    81d8:	47b0      	blx	r6
    81da:	1c43      	adds	r3, r0, #1
    81dc:	d1f0      	bne.n	81c0 <_printf_common+0x88>
    81de:	2001      	movs	r0, #1
    81e0:	4240      	negs	r0, r0
    81e2:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    81e4:	2030      	movs	r0, #48	; 0x30
    81e6:	18e1      	adds	r1, r4, r3
    81e8:	3143      	adds	r1, #67	; 0x43
    81ea:	7008      	strb	r0, [r1, #0]
    81ec:	0021      	movs	r1, r4
    81ee:	1c5a      	adds	r2, r3, #1
    81f0:	3145      	adds	r1, #69	; 0x45
    81f2:	7809      	ldrb	r1, [r1, #0]
    81f4:	18a2      	adds	r2, r4, r2
    81f6:	3243      	adds	r2, #67	; 0x43
    81f8:	3302      	adds	r3, #2
    81fa:	7011      	strb	r1, [r2, #0]
    81fc:	e7c1      	b.n	8182 <_printf_common+0x4a>
    81fe:	0022      	movs	r2, r4
    8200:	2301      	movs	r3, #1
    8202:	9901      	ldr	r1, [sp, #4]
    8204:	9800      	ldr	r0, [sp, #0]
    8206:	9e08      	ldr	r6, [sp, #32]
    8208:	321a      	adds	r2, #26
    820a:	47b0      	blx	r6
    820c:	1c43      	adds	r3, r0, #1
    820e:	d0e6      	beq.n	81de <_printf_common+0xa6>
    8210:	3701      	adds	r7, #1
    8212:	e7d1      	b.n	81b8 <_printf_common+0x80>

00008214 <_printf_i>:
    8214:	b5f0      	push	{r4, r5, r6, r7, lr}
    8216:	000e      	movs	r6, r1
    8218:	b089      	sub	sp, #36	; 0x24
    821a:	9204      	str	r2, [sp, #16]
    821c:	9003      	str	r0, [sp, #12]
    821e:	9305      	str	r3, [sp, #20]
    8220:	000c      	movs	r4, r1
    8222:	7e09      	ldrb	r1, [r1, #24]
    8224:	9a0e      	ldr	r2, [sp, #56]	; 0x38
    8226:	3643      	adds	r6, #67	; 0x43
    8228:	2978      	cmp	r1, #120	; 0x78
    822a:	d807      	bhi.n	823c <_printf_i+0x28>
    822c:	2962      	cmp	r1, #98	; 0x62
    822e:	d809      	bhi.n	8244 <_printf_i+0x30>
    8230:	2900      	cmp	r1, #0
    8232:	d100      	bne.n	8236 <_printf_i+0x22>
    8234:	e0bc      	b.n	83b0 <_printf_i+0x19c>
    8236:	2958      	cmp	r1, #88	; 0x58
    8238:	d100      	bne.n	823c <_printf_i+0x28>
    823a:	e084      	b.n	8346 <_printf_i+0x132>
    823c:	0025      	movs	r5, r4
    823e:	3542      	adds	r5, #66	; 0x42
    8240:	7029      	strb	r1, [r5, #0]
    8242:	e022      	b.n	828a <_printf_i+0x76>
    8244:	0008      	movs	r0, r1
    8246:	3863      	subs	r0, #99	; 0x63
    8248:	2815      	cmp	r0, #21
    824a:	d8f7      	bhi.n	823c <_printf_i+0x28>
    824c:	f001 f89a 	bl	9384 <__gnu_thumb1_case_shi>
    8250:	001f0016 	.word	0x001f0016
    8254:	fff6fff6 	.word	0xfff6fff6
    8258:	fff6fff6 	.word	0xfff6fff6
    825c:	fff6001f 	.word	0xfff6001f
    8260:	fff6fff6 	.word	0xfff6fff6
    8264:	00a3fff6 	.word	0x00a3fff6
    8268:	00920034 	.word	0x00920034
    826c:	fff6fff6 	.word	0xfff6fff6
    8270:	fff600b4 	.word	0xfff600b4
    8274:	fff60034 	.word	0xfff60034
    8278:	0096fff6 	.word	0x0096fff6
    827c:	0025      	movs	r5, r4
    827e:	6813      	ldr	r3, [r2, #0]
    8280:	3542      	adds	r5, #66	; 0x42
    8282:	1d19      	adds	r1, r3, #4
    8284:	681b      	ldr	r3, [r3, #0]
    8286:	6011      	str	r1, [r2, #0]
    8288:	702b      	strb	r3, [r5, #0]
    828a:	2301      	movs	r3, #1
    828c:	e0a1      	b.n	83d2 <_printf_i+0x1be>
    828e:	6813      	ldr	r3, [r2, #0]
    8290:	6820      	ldr	r0, [r4, #0]
    8292:	1d19      	adds	r1, r3, #4
    8294:	0605      	lsls	r5, r0, #24
    8296:	d509      	bpl.n	82ac <_printf_i+0x98>
    8298:	681b      	ldr	r3, [r3, #0]
    829a:	6011      	str	r1, [r2, #0]
    829c:	2b00      	cmp	r3, #0
    829e:	da02      	bge.n	82a6 <_printf_i+0x92>
    82a0:	222d      	movs	r2, #45	; 0x2d
    82a2:	425b      	negs	r3, r3
    82a4:	7032      	strb	r2, [r6, #0]
    82a6:	220a      	movs	r2, #10
    82a8:	495e      	ldr	r1, [pc, #376]	; (8424 <_printf_i+0x210>)
    82aa:	e014      	b.n	82d6 <_printf_i+0xc2>
    82ac:	681b      	ldr	r3, [r3, #0]
    82ae:	6011      	str	r1, [r2, #0]
    82b0:	0640      	lsls	r0, r0, #25
    82b2:	d5f3      	bpl.n	829c <_printf_i+0x88>
    82b4:	b21b      	sxth	r3, r3
    82b6:	e7f1      	b.n	829c <_printf_i+0x88>
    82b8:	6813      	ldr	r3, [r2, #0]
    82ba:	6825      	ldr	r5, [r4, #0]
    82bc:	1d18      	adds	r0, r3, #4
    82be:	6010      	str	r0, [r2, #0]
    82c0:	681b      	ldr	r3, [r3, #0]
    82c2:	062f      	lsls	r7, r5, #24
    82c4:	d53b      	bpl.n	833e <_printf_i+0x12a>
    82c6:	296f      	cmp	r1, #111	; 0x6f
    82c8:	d062      	beq.n	8390 <_printf_i+0x17c>
    82ca:	220a      	movs	r2, #10
    82cc:	4955      	ldr	r1, [pc, #340]	; (8424 <_printf_i+0x210>)
    82ce:	0020      	movs	r0, r4
    82d0:	2500      	movs	r5, #0
    82d2:	3043      	adds	r0, #67	; 0x43
    82d4:	7005      	strb	r5, [r0, #0]
    82d6:	6860      	ldr	r0, [r4, #4]
    82d8:	60a0      	str	r0, [r4, #8]
    82da:	2800      	cmp	r0, #0
    82dc:	db03      	blt.n	82e6 <_printf_i+0xd2>
    82de:	2504      	movs	r5, #4
    82e0:	6827      	ldr	r7, [r4, #0]
    82e2:	43af      	bics	r7, r5
    82e4:	6027      	str	r7, [r4, #0]
    82e6:	b90b      	cbnz	r3, 82ec <_printf_i+0xd8>
    82e8:	0035      	movs	r5, r6
    82ea:	b160      	cbz	r0, 8306 <_printf_i+0xf2>
    82ec:	0035      	movs	r5, r6
    82ee:	fbb3 fcf2 	udiv	ip, r3, r2
    82f2:	4660      	mov	r0, ip
    82f4:	4350      	muls	r0, r2
    82f6:	1a18      	subs	r0, r3, r0
    82f8:	5c08      	ldrb	r0, [r1, r0]
    82fa:	3d01      	subs	r5, #1
    82fc:	7028      	strb	r0, [r5, #0]
    82fe:	0018      	movs	r0, r3
    8300:	4663      	mov	r3, ip
    8302:	4282      	cmp	r2, r0
    8304:	d9f3      	bls.n	82ee <_printf_i+0xda>
    8306:	2a08      	cmp	r2, #8
    8308:	d109      	bne.n	831e <_printf_i+0x10a>
    830a:	6823      	ldr	r3, [r4, #0]
    830c:	07db      	lsls	r3, r3, #31
    830e:	d506      	bpl.n	831e <_printf_i+0x10a>
    8310:	6863      	ldr	r3, [r4, #4]
    8312:	6922      	ldr	r2, [r4, #16]
    8314:	4293      	cmp	r3, r2
    8316:	dc02      	bgt.n	831e <_printf_i+0x10a>
    8318:	2330      	movs	r3, #48	; 0x30
    831a:	3d01      	subs	r5, #1
    831c:	702b      	strb	r3, [r5, #0]
    831e:	1b76      	subs	r6, r6, r5
    8320:	6126      	str	r6, [r4, #16]
    8322:	9b05      	ldr	r3, [sp, #20]
    8324:	0021      	movs	r1, r4
    8326:	9300      	str	r3, [sp, #0]
    8328:	9803      	ldr	r0, [sp, #12]
    832a:	9b04      	ldr	r3, [sp, #16]
    832c:	aa07      	add	r2, sp, #28
    832e:	f7ff ff03 	bl	8138 <_printf_common>
    8332:	1c43      	adds	r3, r0, #1
    8334:	d151      	bne.n	83da <_printf_i+0x1c6>
    8336:	2001      	movs	r0, #1
    8338:	4240      	negs	r0, r0
    833a:	b009      	add	sp, #36	; 0x24
    833c:	bdf0      	pop	{r4, r5, r6, r7, pc}
    833e:	066d      	lsls	r5, r5, #25
    8340:	d5c1      	bpl.n	82c6 <_printf_i+0xb2>
    8342:	b29b      	uxth	r3, r3
    8344:	e7bf      	b.n	82c6 <_printf_i+0xb2>
    8346:	0023      	movs	r3, r4
    8348:	3345      	adds	r3, #69	; 0x45
    834a:	7019      	strb	r1, [r3, #0]
    834c:	4935      	ldr	r1, [pc, #212]	; (8424 <_printf_i+0x210>)
    834e:	6810      	ldr	r0, [r2, #0]
    8350:	6825      	ldr	r5, [r4, #0]
    8352:	c808      	ldmia	r0!, {r3}
    8354:	6010      	str	r0, [r2, #0]
    8356:	062f      	lsls	r7, r5, #24
    8358:	d516      	bpl.n	8388 <_printf_i+0x174>
    835a:	07ea      	lsls	r2, r5, #31
    835c:	d502      	bpl.n	8364 <_printf_i+0x150>
    835e:	2220      	movs	r2, #32
    8360:	4315      	orrs	r5, r2
    8362:	6025      	str	r5, [r4, #0]
    8364:	2210      	movs	r2, #16
    8366:	2b00      	cmp	r3, #0
    8368:	d1b1      	bne.n	82ce <_printf_i+0xba>
    836a:	2020      	movs	r0, #32
    836c:	6825      	ldr	r5, [r4, #0]
    836e:	4385      	bics	r5, r0
    8370:	6025      	str	r5, [r4, #0]
    8372:	e7ac      	b.n	82ce <_printf_i+0xba>
    8374:	2320      	movs	r3, #32
    8376:	6821      	ldr	r1, [r4, #0]
    8378:	430b      	orrs	r3, r1
    837a:	6023      	str	r3, [r4, #0]
    837c:	0023      	movs	r3, r4
    837e:	2178      	movs	r1, #120	; 0x78
    8380:	3345      	adds	r3, #69	; 0x45
    8382:	7019      	strb	r1, [r3, #0]
    8384:	4928      	ldr	r1, [pc, #160]	; (8428 <_printf_i+0x214>)
    8386:	e7e2      	b.n	834e <_printf_i+0x13a>
    8388:	066f      	lsls	r7, r5, #25
    838a:	d5e6      	bpl.n	835a <_printf_i+0x146>
    838c:	b29b      	uxth	r3, r3
    838e:	e7e4      	b.n	835a <_printf_i+0x146>
    8390:	2208      	movs	r2, #8
    8392:	4924      	ldr	r1, [pc, #144]	; (8424 <_printf_i+0x210>)
    8394:	e79b      	b.n	82ce <_printf_i+0xba>
    8396:	6813      	ldr	r3, [r2, #0]
    8398:	6825      	ldr	r5, [r4, #0]
    839a:	1d18      	adds	r0, r3, #4
    839c:	6961      	ldr	r1, [r4, #20]
    839e:	6010      	str	r0, [r2, #0]
    83a0:	681b      	ldr	r3, [r3, #0]
    83a2:	062f      	lsls	r7, r5, #24
    83a4:	d501      	bpl.n	83aa <_printf_i+0x196>
    83a6:	6019      	str	r1, [r3, #0]
    83a8:	e002      	b.n	83b0 <_printf_i+0x19c>
    83aa:	066d      	lsls	r5, r5, #25
    83ac:	d5fb      	bpl.n	83a6 <_printf_i+0x192>
    83ae:	8019      	strh	r1, [r3, #0]
    83b0:	2300      	movs	r3, #0
    83b2:	0035      	movs	r5, r6
    83b4:	6123      	str	r3, [r4, #16]
    83b6:	e7b4      	b.n	8322 <_printf_i+0x10e>
    83b8:	6813      	ldr	r3, [r2, #0]
    83ba:	1d19      	adds	r1, r3, #4
    83bc:	6011      	str	r1, [r2, #0]
    83be:	681d      	ldr	r5, [r3, #0]
    83c0:	2100      	movs	r1, #0
    83c2:	0028      	movs	r0, r5
    83c4:	6862      	ldr	r2, [r4, #4]
    83c6:	f000 febf 	bl	9148 <memchr>
    83ca:	b108      	cbz	r0, 83d0 <_printf_i+0x1bc>
    83cc:	1b40      	subs	r0, r0, r5
    83ce:	6060      	str	r0, [r4, #4]
    83d0:	6863      	ldr	r3, [r4, #4]
    83d2:	6123      	str	r3, [r4, #16]
    83d4:	2300      	movs	r3, #0
    83d6:	7033      	strb	r3, [r6, #0]
    83d8:	e7a3      	b.n	8322 <_printf_i+0x10e>
    83da:	002a      	movs	r2, r5
    83dc:	6923      	ldr	r3, [r4, #16]
    83de:	9904      	ldr	r1, [sp, #16]
    83e0:	9803      	ldr	r0, [sp, #12]
    83e2:	9d05      	ldr	r5, [sp, #20]
    83e4:	47a8      	blx	r5
    83e6:	1c43      	adds	r3, r0, #1
    83e8:	d0a5      	beq.n	8336 <_printf_i+0x122>
    83ea:	6823      	ldr	r3, [r4, #0]
    83ec:	079b      	lsls	r3, r3, #30
    83ee:	d416      	bmi.n	841e <_printf_i+0x20a>
    83f0:	9b07      	ldr	r3, [sp, #28]
    83f2:	68e0      	ldr	r0, [r4, #12]
    83f4:	4298      	cmp	r0, r3
    83f6:	daa0      	bge.n	833a <_printf_i+0x126>
    83f8:	0018      	movs	r0, r3
    83fa:	e79e      	b.n	833a <_printf_i+0x126>
    83fc:	0022      	movs	r2, r4
    83fe:	2301      	movs	r3, #1
    8400:	9904      	ldr	r1, [sp, #16]
    8402:	9803      	ldr	r0, [sp, #12]
    8404:	9e05      	ldr	r6, [sp, #20]
    8406:	3219      	adds	r2, #25
    8408:	47b0      	blx	r6
    840a:	1c43      	adds	r3, r0, #1
    840c:	d100      	bne.n	8410 <_printf_i+0x1fc>
    840e:	e792      	b.n	8336 <_printf_i+0x122>
    8410:	3501      	adds	r5, #1
    8412:	68e3      	ldr	r3, [r4, #12]
    8414:	9a07      	ldr	r2, [sp, #28]
    8416:	1a9b      	subs	r3, r3, r2
    8418:	42ab      	cmp	r3, r5
    841a:	dcef      	bgt.n	83fc <_printf_i+0x1e8>
    841c:	e7e8      	b.n	83f0 <_printf_i+0x1dc>
    841e:	2500      	movs	r5, #0
    8420:	e7f7      	b.n	8412 <_printf_i+0x1fe>
    8422:	bf00      	nop
    8424:	00009795 	.word	0x00009795
    8428:	000097a6 	.word	0x000097a6

0000842c <__sfputc_r>:
    842c:	6893      	ldr	r3, [r2, #8]
    842e:	b510      	push	{r4, lr}
    8430:	3b01      	subs	r3, #1
    8432:	6093      	str	r3, [r2, #8]
    8434:	2b00      	cmp	r3, #0
    8436:	da04      	bge.n	8442 <__sfputc_r+0x16>
    8438:	6994      	ldr	r4, [r2, #24]
    843a:	42a3      	cmp	r3, r4
    843c:	db07      	blt.n	844e <__sfputc_r+0x22>
    843e:	290a      	cmp	r1, #10
    8440:	d005      	beq.n	844e <__sfputc_r+0x22>
    8442:	6813      	ldr	r3, [r2, #0]
    8444:	1c58      	adds	r0, r3, #1
    8446:	6010      	str	r0, [r2, #0]
    8448:	7019      	strb	r1, [r3, #0]
    844a:	0008      	movs	r0, r1
    844c:	bd10      	pop	{r4, pc}
    844e:	f7fe fe25 	bl	709c <__swbuf_r>
    8452:	0001      	movs	r1, r0
    8454:	e7f9      	b.n	844a <__sfputc_r+0x1e>

00008456 <__sfputs_r>:
    8456:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    8458:	0006      	movs	r6, r0
    845a:	000f      	movs	r7, r1
    845c:	0014      	movs	r4, r2
    845e:	18d5      	adds	r5, r2, r3
    8460:	42ac      	cmp	r4, r5
    8462:	d101      	bne.n	8468 <__sfputs_r+0x12>
    8464:	2000      	movs	r0, #0
    8466:	e007      	b.n	8478 <__sfputs_r+0x22>
    8468:	7821      	ldrb	r1, [r4, #0]
    846a:	003a      	movs	r2, r7
    846c:	0030      	movs	r0, r6
    846e:	f7ff ffdd 	bl	842c <__sfputc_r>
    8472:	3401      	adds	r4, #1
    8474:	1c43      	adds	r3, r0, #1
    8476:	d1f3      	bne.n	8460 <__sfputs_r+0xa>
    8478:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

0000847a <__sprint_r>:
    847a:	6893      	ldr	r3, [r2, #8]
    847c:	b510      	push	{r4, lr}
    847e:	0014      	movs	r4, r2
    8480:	b913      	cbnz	r3, 8488 <__sprint_r+0xe>
    8482:	0018      	movs	r0, r3
    8484:	6053      	str	r3, [r2, #4]
    8486:	bd10      	pop	{r4, pc}
    8488:	f000 fce6 	bl	8e58 <__sfvwrite_r>
    848c:	2300      	movs	r3, #0
    848e:	60a3      	str	r3, [r4, #8]
    8490:	6063      	str	r3, [r4, #4]
    8492:	e7f8      	b.n	8486 <__sprint_r+0xc>

00008494 <_vfiprintf_r>:
    8494:	b5f0      	push	{r4, r5, r6, r7, lr}
    8496:	b0a1      	sub	sp, #132	; 0x84
    8498:	0007      	movs	r7, r0
    849a:	000c      	movs	r4, r1
    849c:	001e      	movs	r6, r3
    849e:	9203      	str	r2, [sp, #12]
    84a0:	b118      	cbz	r0, 84aa <_vfiprintf_r+0x16>
    84a2:	6983      	ldr	r3, [r0, #24]
    84a4:	b90b      	cbnz	r3, 84aa <_vfiprintf_r+0x16>
    84a6:	f7ff f871 	bl	758c <__sinit>
    84aa:	4b8b      	ldr	r3, [pc, #556]	; (86d8 <_vfiprintf_r+0x244>)
    84ac:	429c      	cmp	r4, r3
    84ae:	d11a      	bne.n	84e6 <_vfiprintf_r+0x52>
    84b0:	687c      	ldr	r4, [r7, #4]
    84b2:	6e63      	ldr	r3, [r4, #100]	; 0x64
    84b4:	07db      	lsls	r3, r3, #31
    84b6:	d405      	bmi.n	84c4 <_vfiprintf_r+0x30>
    84b8:	89a3      	ldrh	r3, [r4, #12]
    84ba:	059b      	lsls	r3, r3, #22
    84bc:	d402      	bmi.n	84c4 <_vfiprintf_r+0x30>
    84be:	6da0      	ldr	r0, [r4, #88]	; 0x58
    84c0:	f7ff f95a 	bl	7778 <__retarget_lock_acquire_recursive>
    84c4:	89a3      	ldrh	r3, [r4, #12]
    84c6:	071b      	lsls	r3, r3, #28
    84c8:	d501      	bpl.n	84ce <_vfiprintf_r+0x3a>
    84ca:	6923      	ldr	r3, [r4, #16]
    84cc:	b9e3      	cbnz	r3, 8508 <_vfiprintf_r+0x74>
    84ce:	0021      	movs	r1, r4
    84d0:	0038      	movs	r0, r7
    84d2:	f7fe fe51 	bl	7178 <__swsetup_r>
    84d6:	b1b8      	cbz	r0, 8508 <_vfiprintf_r+0x74>
    84d8:	6e63      	ldr	r3, [r4, #100]	; 0x64
    84da:	07db      	lsls	r3, r3, #31
    84dc:	d50d      	bpl.n	84fa <_vfiprintf_r+0x66>
    84de:	2001      	movs	r0, #1
    84e0:	4240      	negs	r0, r0
    84e2:	b021      	add	sp, #132	; 0x84
    84e4:	bdf0      	pop	{r4, r5, r6, r7, pc}
    84e6:	4b7d      	ldr	r3, [pc, #500]	; (86dc <_vfiprintf_r+0x248>)
    84e8:	429c      	cmp	r4, r3
    84ea:	d101      	bne.n	84f0 <_vfiprintf_r+0x5c>
    84ec:	68bc      	ldr	r4, [r7, #8]
    84ee:	e7e0      	b.n	84b2 <_vfiprintf_r+0x1e>
    84f0:	4b7b      	ldr	r3, [pc, #492]	; (86e0 <_vfiprintf_r+0x24c>)
    84f2:	429c      	cmp	r4, r3
    84f4:	d1dd      	bne.n	84b2 <_vfiprintf_r+0x1e>
    84f6:	68fc      	ldr	r4, [r7, #12]
    84f8:	e7db      	b.n	84b2 <_vfiprintf_r+0x1e>
    84fa:	89a3      	ldrh	r3, [r4, #12]
    84fc:	059b      	lsls	r3, r3, #22
    84fe:	d4ee      	bmi.n	84de <_vfiprintf_r+0x4a>
    8500:	6da0      	ldr	r0, [r4, #88]	; 0x58
    8502:	f7ff f93f 	bl	7784 <__retarget_lock_release_recursive>
    8506:	e7ea      	b.n	84de <_vfiprintf_r+0x4a>
    8508:	2300      	movs	r3, #0
    850a:	ad08      	add	r5, sp, #32
    850c:	616b      	str	r3, [r5, #20]
    850e:	3320      	adds	r3, #32
    8510:	766b      	strb	r3, [r5, #25]
    8512:	3310      	adds	r3, #16
    8514:	76ab      	strb	r3, [r5, #26]
    8516:	9607      	str	r6, [sp, #28]
    8518:	9e03      	ldr	r6, [sp, #12]
    851a:	7833      	ldrb	r3, [r6, #0]
    851c:	b10b      	cbz	r3, 8522 <_vfiprintf_r+0x8e>
    851e:	2b25      	cmp	r3, #37	; 0x25
    8520:	d14d      	bne.n	85be <_vfiprintf_r+0x12a>
    8522:	9b03      	ldr	r3, [sp, #12]
    8524:	1af3      	subs	r3, r6, r3
    8526:	9305      	str	r3, [sp, #20]
    8528:	9b03      	ldr	r3, [sp, #12]
    852a:	429e      	cmp	r6, r3
    852c:	d00d      	beq.n	854a <_vfiprintf_r+0xb6>
    852e:	9b05      	ldr	r3, [sp, #20]
    8530:	0021      	movs	r1, r4
    8532:	0038      	movs	r0, r7
    8534:	9a03      	ldr	r2, [sp, #12]
    8536:	f7ff ff8e 	bl	8456 <__sfputs_r>
    853a:	1c43      	adds	r3, r0, #1
    853c:	d100      	bne.n	8540 <_vfiprintf_r+0xac>
    853e:	e0b3      	b.n	86a8 <_vfiprintf_r+0x214>
    8540:	696a      	ldr	r2, [r5, #20]
    8542:	9b05      	ldr	r3, [sp, #20]
    8544:	4694      	mov	ip, r2
    8546:	4463      	add	r3, ip
    8548:	616b      	str	r3, [r5, #20]
    854a:	7833      	ldrb	r3, [r6, #0]
    854c:	2b00      	cmp	r3, #0
    854e:	d100      	bne.n	8552 <_vfiprintf_r+0xbe>
    8550:	e0aa      	b.n	86a8 <_vfiprintf_r+0x214>
    8552:	2201      	movs	r2, #1
    8554:	1c73      	adds	r3, r6, #1
    8556:	9303      	str	r3, [sp, #12]
    8558:	2300      	movs	r3, #0
    855a:	4252      	negs	r2, r2
    855c:	606a      	str	r2, [r5, #4]
    855e:	a904      	add	r1, sp, #16
    8560:	3254      	adds	r2, #84	; 0x54
    8562:	1852      	adds	r2, r2, r1
    8564:	602b      	str	r3, [r5, #0]
    8566:	60eb      	str	r3, [r5, #12]
    8568:	60ab      	str	r3, [r5, #8]
    856a:	7013      	strb	r3, [r2, #0]
    856c:	65ab      	str	r3, [r5, #88]	; 0x58
    856e:	9b03      	ldr	r3, [sp, #12]
    8570:	2205      	movs	r2, #5
    8572:	7819      	ldrb	r1, [r3, #0]
    8574:	485b      	ldr	r0, [pc, #364]	; (86e4 <_vfiprintf_r+0x250>)
    8576:	f000 fde7 	bl	9148 <memchr>
    857a:	9b03      	ldr	r3, [sp, #12]
    857c:	1c5e      	adds	r6, r3, #1
    857e:	bb00      	cbnz	r0, 85c2 <_vfiprintf_r+0x12e>
    8580:	682a      	ldr	r2, [r5, #0]
    8582:	06d3      	lsls	r3, r2, #27
    8584:	d504      	bpl.n	8590 <_vfiprintf_r+0xfc>
    8586:	2353      	movs	r3, #83	; 0x53
    8588:	a904      	add	r1, sp, #16
    858a:	185b      	adds	r3, r3, r1
    858c:	2120      	movs	r1, #32
    858e:	7019      	strb	r1, [r3, #0]
    8590:	0713      	lsls	r3, r2, #28
    8592:	d504      	bpl.n	859e <_vfiprintf_r+0x10a>
    8594:	2353      	movs	r3, #83	; 0x53
    8596:	a904      	add	r1, sp, #16
    8598:	185b      	adds	r3, r3, r1
    859a:	212b      	movs	r1, #43	; 0x2b
    859c:	7019      	strb	r1, [r3, #0]
    859e:	9b03      	ldr	r3, [sp, #12]
    85a0:	781b      	ldrb	r3, [r3, #0]
    85a2:	2b2a      	cmp	r3, #42	; 0x2a
    85a4:	d016      	beq.n	85d4 <_vfiprintf_r+0x140>
    85a6:	2100      	movs	r1, #0
    85a8:	f240 0c0a 	movw	ip, #10
    85ac:	68eb      	ldr	r3, [r5, #12]
    85ae:	9e03      	ldr	r6, [sp, #12]
    85b0:	7832      	ldrb	r2, [r6, #0]
    85b2:	1c70      	adds	r0, r6, #1
    85b4:	3a30      	subs	r2, #48	; 0x30
    85b6:	2a09      	cmp	r2, #9
    85b8:	d94c      	bls.n	8654 <_vfiprintf_r+0x1c0>
    85ba:	b989      	cbnz	r1, 85e0 <_vfiprintf_r+0x14c>
    85bc:	e017      	b.n	85ee <_vfiprintf_r+0x15a>
    85be:	3601      	adds	r6, #1
    85c0:	e7ab      	b.n	851a <_vfiprintf_r+0x86>
    85c2:	4b48      	ldr	r3, [pc, #288]	; (86e4 <_vfiprintf_r+0x250>)
    85c4:	682a      	ldr	r2, [r5, #0]
    85c6:	1ac0      	subs	r0, r0, r3
    85c8:	2301      	movs	r3, #1
    85ca:	4083      	lsls	r3, r0
    85cc:	4313      	orrs	r3, r2
    85ce:	602b      	str	r3, [r5, #0]
    85d0:	9603      	str	r6, [sp, #12]
    85d2:	e7cc      	b.n	856e <_vfiprintf_r+0xda>
    85d4:	9b07      	ldr	r3, [sp, #28]
    85d6:	1d19      	adds	r1, r3, #4
    85d8:	681b      	ldr	r3, [r3, #0]
    85da:	9107      	str	r1, [sp, #28]
    85dc:	2b00      	cmp	r3, #0
    85de:	db01      	blt.n	85e4 <_vfiprintf_r+0x150>
    85e0:	930b      	str	r3, [sp, #44]	; 0x2c
    85e2:	e004      	b.n	85ee <_vfiprintf_r+0x15a>
    85e4:	425b      	negs	r3, r3
    85e6:	60eb      	str	r3, [r5, #12]
    85e8:	2302      	movs	r3, #2
    85ea:	4313      	orrs	r3, r2
    85ec:	602b      	str	r3, [r5, #0]
    85ee:	7833      	ldrb	r3, [r6, #0]
    85f0:	2b2e      	cmp	r3, #46	; 0x2e
    85f2:	d10a      	bne.n	860a <_vfiprintf_r+0x176>
    85f4:	7873      	ldrb	r3, [r6, #1]
    85f6:	2b2a      	cmp	r3, #42	; 0x2a
    85f8:	d135      	bne.n	8666 <_vfiprintf_r+0x1d2>
    85fa:	9b07      	ldr	r3, [sp, #28]
    85fc:	3602      	adds	r6, #2
    85fe:	1d1a      	adds	r2, r3, #4
    8600:	681b      	ldr	r3, [r3, #0]
    8602:	9207      	str	r2, [sp, #28]
    8604:	2b00      	cmp	r3, #0
    8606:	db2b      	blt.n	8660 <_vfiprintf_r+0x1cc>
    8608:	9309      	str	r3, [sp, #36]	; 0x24
    860a:	2203      	movs	r2, #3
    860c:	7831      	ldrb	r1, [r6, #0]
    860e:	4836      	ldr	r0, [pc, #216]	; (86e8 <_vfiprintf_r+0x254>)
    8610:	f000 fd9a 	bl	9148 <memchr>
    8614:	b138      	cbz	r0, 8626 <_vfiprintf_r+0x192>
    8616:	4b34      	ldr	r3, [pc, #208]	; (86e8 <_vfiprintf_r+0x254>)
    8618:	682a      	ldr	r2, [r5, #0]
    861a:	1ac0      	subs	r0, r0, r3
    861c:	2340      	movs	r3, #64	; 0x40
    861e:	4083      	lsls	r3, r0
    8620:	4313      	orrs	r3, r2
    8622:	3601      	adds	r6, #1
    8624:	602b      	str	r3, [r5, #0]
    8626:	7831      	ldrb	r1, [r6, #0]
    8628:	1c73      	adds	r3, r6, #1
    862a:	2206      	movs	r2, #6
    862c:	482f      	ldr	r0, [pc, #188]	; (86ec <_vfiprintf_r+0x258>)
    862e:	9303      	str	r3, [sp, #12]
    8630:	7629      	strb	r1, [r5, #24]
    8632:	f000 fd89 	bl	9148 <memchr>
    8636:	2800      	cmp	r0, #0
    8638:	d045      	beq.n	86c6 <_vfiprintf_r+0x232>
    863a:	4b2d      	ldr	r3, [pc, #180]	; (86f0 <_vfiprintf_r+0x25c>)
    863c:	bb43      	cbnz	r3, 8690 <_vfiprintf_r+0x1fc>
    863e:	2207      	movs	r2, #7
    8640:	9b07      	ldr	r3, [sp, #28]
    8642:	3307      	adds	r3, #7
    8644:	4393      	bics	r3, r2
    8646:	3308      	adds	r3, #8
    8648:	9307      	str	r3, [sp, #28]
    864a:	696b      	ldr	r3, [r5, #20]
    864c:	9a04      	ldr	r2, [sp, #16]
    864e:	189b      	adds	r3, r3, r2
    8650:	616b      	str	r3, [r5, #20]
    8652:	e761      	b.n	8518 <_vfiprintf_r+0x84>
    8654:	4661      	mov	r1, ip
    8656:	434b      	muls	r3, r1
    8658:	0006      	movs	r6, r0
    865a:	2101      	movs	r1, #1
    865c:	189b      	adds	r3, r3, r2
    865e:	e7a7      	b.n	85b0 <_vfiprintf_r+0x11c>
    8660:	2301      	movs	r3, #1
    8662:	425b      	negs	r3, r3
    8664:	e7d0      	b.n	8608 <_vfiprintf_r+0x174>
    8666:	2300      	movs	r3, #0
    8668:	f240 0c0a 	movw	ip, #10
    866c:	001a      	movs	r2, r3
    866e:	3601      	adds	r6, #1
    8670:	606b      	str	r3, [r5, #4]
    8672:	7831      	ldrb	r1, [r6, #0]
    8674:	1c70      	adds	r0, r6, #1
    8676:	3930      	subs	r1, #48	; 0x30
    8678:	2909      	cmp	r1, #9
    867a:	d903      	bls.n	8684 <_vfiprintf_r+0x1f0>
    867c:	2b00      	cmp	r3, #0
    867e:	d0c4      	beq.n	860a <_vfiprintf_r+0x176>
    8680:	9209      	str	r2, [sp, #36]	; 0x24
    8682:	e7c2      	b.n	860a <_vfiprintf_r+0x176>
    8684:	4663      	mov	r3, ip
    8686:	435a      	muls	r2, r3
    8688:	0006      	movs	r6, r0
    868a:	2301      	movs	r3, #1
    868c:	1852      	adds	r2, r2, r1
    868e:	e7f0      	b.n	8672 <_vfiprintf_r+0x1de>
    8690:	ab07      	add	r3, sp, #28
    8692:	9300      	str	r3, [sp, #0]
    8694:	0022      	movs	r2, r4
    8696:	0029      	movs	r1, r5
    8698:	0038      	movs	r0, r7
    869a:	4b16      	ldr	r3, [pc, #88]	; (86f4 <_vfiprintf_r+0x260>)
    869c:	e000      	b.n	86a0 <_vfiprintf_r+0x20c>
    869e:	bf00      	nop
    86a0:	9004      	str	r0, [sp, #16]
    86a2:	9b04      	ldr	r3, [sp, #16]
    86a4:	3301      	adds	r3, #1
    86a6:	d1d0      	bne.n	864a <_vfiprintf_r+0x1b6>
    86a8:	6e63      	ldr	r3, [r4, #100]	; 0x64
    86aa:	07db      	lsls	r3, r3, #31
    86ac:	d405      	bmi.n	86ba <_vfiprintf_r+0x226>
    86ae:	89a3      	ldrh	r3, [r4, #12]
    86b0:	059b      	lsls	r3, r3, #22
    86b2:	d402      	bmi.n	86ba <_vfiprintf_r+0x226>
    86b4:	6da0      	ldr	r0, [r4, #88]	; 0x58
    86b6:	f7ff f865 	bl	7784 <__retarget_lock_release_recursive>
    86ba:	89a3      	ldrh	r3, [r4, #12]
    86bc:	065b      	lsls	r3, r3, #25
    86be:	d500      	bpl.n	86c2 <_vfiprintf_r+0x22e>
    86c0:	e70d      	b.n	84de <_vfiprintf_r+0x4a>
    86c2:	980d      	ldr	r0, [sp, #52]	; 0x34
    86c4:	e70d      	b.n	84e2 <_vfiprintf_r+0x4e>
    86c6:	ab07      	add	r3, sp, #28
    86c8:	9300      	str	r3, [sp, #0]
    86ca:	0022      	movs	r2, r4
    86cc:	0029      	movs	r1, r5
    86ce:	0038      	movs	r0, r7
    86d0:	4b08      	ldr	r3, [pc, #32]	; (86f4 <_vfiprintf_r+0x260>)
    86d2:	f7ff fd9f 	bl	8214 <_printf_i>
    86d6:	e7e3      	b.n	86a0 <_vfiprintf_r+0x20c>
    86d8:	00009760 	.word	0x00009760
    86dc:	00009740 	.word	0x00009740
    86e0:	00009720 	.word	0x00009720
    86e4:	00009784 	.word	0x00009784
    86e8:	0000978a 	.word	0x0000978a
    86ec:	0000978e 	.word	0x0000978e
    86f0:	00000000 	.word	0x00000000
    86f4:	00008457 	.word	0x00008457

000086f8 <vfiprintf>:
    86f8:	b510      	push	{r4, lr}
    86fa:	0013      	movs	r3, r2
    86fc:	000a      	movs	r2, r1
    86fe:	0001      	movs	r1, r0
    8700:	4802      	ldr	r0, [pc, #8]	; (870c <vfiprintf+0x14>)
    8702:	6800      	ldr	r0, [r0, #0]
    8704:	f7ff fec6 	bl	8494 <_vfiprintf_r>
    8708:	bd10      	pop	{r4, pc}
    870a:	bf00      	nop
    870c:	20002000 	.word	0x20002000

00008710 <_scanf_chars>:
    8710:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    8712:	0015      	movs	r5, r2
    8714:	688a      	ldr	r2, [r1, #8]
    8716:	000c      	movs	r4, r1
    8718:	9001      	str	r0, [sp, #4]
    871a:	b922      	cbnz	r2, 8726 <_scanf_chars+0x16>
    871c:	6989      	ldr	r1, [r1, #24]
    871e:	3201      	adds	r2, #1
    8720:	b101      	cbz	r1, 8724 <_scanf_chars+0x14>
    8722:	3a02      	subs	r2, #2
    8724:	60a2      	str	r2, [r4, #8]
    8726:	6822      	ldr	r2, [r4, #0]
    8728:	06d2      	lsls	r2, r2, #27
    872a:	d403      	bmi.n	8734 <_scanf_chars+0x24>
    872c:	681a      	ldr	r2, [r3, #0]
    872e:	1d11      	adds	r1, r2, #4
    8730:	6019      	str	r1, [r3, #0]
    8732:	6817      	ldr	r7, [r2, #0]
    8734:	2600      	movs	r6, #0
    8736:	69a0      	ldr	r0, [r4, #24]
    8738:	b188      	cbz	r0, 875e <_scanf_chars+0x4e>
    873a:	2801      	cmp	r0, #1
    873c:	d106      	bne.n	874c <_scanf_chars+0x3c>
    873e:	682b      	ldr	r3, [r5, #0]
    8740:	6962      	ldr	r2, [r4, #20]
    8742:	781b      	ldrb	r3, [r3, #0]
    8744:	5cd3      	ldrb	r3, [r2, r3]
    8746:	b953      	cbnz	r3, 875e <_scanf_chars+0x4e>
    8748:	b3b6      	cbz	r6, 87b8 <_scanf_chars+0xa8>
    874a:	e025      	b.n	8798 <_scanf_chars+0x88>
    874c:	2802      	cmp	r0, #2
    874e:	d123      	bne.n	8798 <_scanf_chars+0x88>
    8750:	682b      	ldr	r3, [r5, #0]
    8752:	4a1a      	ldr	r2, [pc, #104]	; (87bc <_scanf_chars+0xac>)
    8754:	781b      	ldrb	r3, [r3, #0]
    8756:	5cd3      	ldrb	r3, [r2, r3]
    8758:	2208      	movs	r2, #8
    875a:	4213      	tst	r3, r2
    875c:	d11c      	bne.n	8798 <_scanf_chars+0x88>
    875e:	2210      	movs	r2, #16
    8760:	6823      	ldr	r3, [r4, #0]
    8762:	3601      	adds	r6, #1
    8764:	4213      	tst	r3, r2
    8766:	d103      	bne.n	8770 <_scanf_chars+0x60>
    8768:	682b      	ldr	r3, [r5, #0]
    876a:	781b      	ldrb	r3, [r3, #0]
    876c:	703b      	strb	r3, [r7, #0]
    876e:	3701      	adds	r7, #1
    8770:	682a      	ldr	r2, [r5, #0]
    8772:	686b      	ldr	r3, [r5, #4]
    8774:	3201      	adds	r2, #1
    8776:	602a      	str	r2, [r5, #0]
    8778:	68a2      	ldr	r2, [r4, #8]
    877a:	3b01      	subs	r3, #1
    877c:	3a01      	subs	r2, #1
    877e:	606b      	str	r3, [r5, #4]
    8780:	60a2      	str	r2, [r4, #8]
    8782:	b14a      	cbz	r2, 8798 <_scanf_chars+0x88>
    8784:	2b00      	cmp	r3, #0
    8786:	dcd6      	bgt.n	8736 <_scanf_chars+0x26>
    8788:	f240 1380 	movw	r3, #384	; 0x180
    878c:	0029      	movs	r1, r5
    878e:	58e3      	ldr	r3, [r4, r3]
    8790:	9801      	ldr	r0, [sp, #4]
    8792:	4798      	blx	r3
    8794:	2800      	cmp	r0, #0
    8796:	d0ce      	beq.n	8736 <_scanf_chars+0x26>
    8798:	6822      	ldr	r2, [r4, #0]
    879a:	2310      	movs	r3, #16
    879c:	0011      	movs	r1, r2
    879e:	4019      	ands	r1, r3
    87a0:	421a      	tst	r2, r3
    87a2:	d105      	bne.n	87b0 <_scanf_chars+0xa0>
    87a4:	68e3      	ldr	r3, [r4, #12]
    87a6:	3301      	adds	r3, #1
    87a8:	60e3      	str	r3, [r4, #12]
    87aa:	69a3      	ldr	r3, [r4, #24]
    87ac:	b103      	cbz	r3, 87b0 <_scanf_chars+0xa0>
    87ae:	7039      	strb	r1, [r7, #0]
    87b0:	2000      	movs	r0, #0
    87b2:	6923      	ldr	r3, [r4, #16]
    87b4:	199e      	adds	r6, r3, r6
    87b6:	6126      	str	r6, [r4, #16]
    87b8:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    87ba:	bf00      	nop
    87bc:	0000961d 	.word	0x0000961d

000087c0 <_scanf_i>:
    87c0:	b5f0      	push	{r4, r5, r6, r7, lr}
    87c2:	000c      	movs	r4, r1
    87c4:	b08d      	sub	sp, #52	; 0x34
    87c6:	9302      	str	r3, [sp, #8]
    87c8:	4b76      	ldr	r3, [pc, #472]	; (89a4 <_scanf_i+0x1e4>)
    87ca:	0016      	movs	r6, r2
    87cc:	9005      	str	r0, [sp, #20]
    87ce:	aa09      	add	r2, sp, #36	; 0x24
    87d0:	cb23      	ldmia	r3!, {r0, r1, r5}
    87d2:	c223      	stmia	r2!, {r0, r1, r5}
    87d4:	69a3      	ldr	r3, [r4, #24]
    87d6:	2b03      	cmp	r3, #3
    87d8:	d100      	bne.n	87dc <_scanf_i+0x1c>
    87da:	e083      	b.n	88e4 <_scanf_i+0x124>
    87dc:	4b72      	ldr	r3, [pc, #456]	; (89a8 <_scanf_i+0x1e8>)
    87de:	2000      	movs	r0, #0
    87e0:	f240 125c 	movw	r2, #348	; 0x15c
    87e4:	9306      	str	r3, [sp, #24]
    87e6:	68a3      	ldr	r3, [r4, #8]
    87e8:	9004      	str	r0, [sp, #16]
    87ea:	1e59      	subs	r1, r3, #1
    87ec:	4291      	cmp	r1, r2
    87ee:	d905      	bls.n	87fc <_scanf_i+0x3c>
    87f0:	3b5e      	subs	r3, #94	; 0x5e
    87f2:	3bff      	subs	r3, #255	; 0xff
    87f4:	9304      	str	r3, [sp, #16]
    87f6:	f240 135d 	movw	r3, #349	; 0x15d
    87fa:	60a3      	str	r3, [r4, #8]
    87fc:	0023      	movs	r3, r4
    87fe:	331c      	adds	r3, #28
    8800:	9301      	str	r3, [sp, #4]
    8802:	f640 5300 	movw	r3, #3328	; 0xd00
    8806:	2700      	movs	r7, #0
    8808:	6822      	ldr	r2, [r4, #0]
    880a:	4313      	orrs	r3, r2
    880c:	6023      	str	r3, [r4, #0]
    880e:	9b01      	ldr	r3, [sp, #4]
    8810:	9303      	str	r3, [sp, #12]
    8812:	6833      	ldr	r3, [r6, #0]
    8814:	a809      	add	r0, sp, #36	; 0x24
    8816:	7819      	ldrb	r1, [r3, #0]
    8818:	00bb      	lsls	r3, r7, #2
    881a:	2202      	movs	r2, #2
    881c:	5818      	ldr	r0, [r3, r0]
    881e:	f000 fc93 	bl	9148 <memchr>
    8822:	b348      	cbz	r0, 8878 <_scanf_i+0xb8>
    8824:	2f01      	cmp	r7, #1
    8826:	d15f      	bne.n	88e8 <_scanf_i+0x128>
    8828:	6863      	ldr	r3, [r4, #4]
    882a:	b933      	cbnz	r3, 883a <_scanf_i+0x7a>
    882c:	3308      	adds	r3, #8
    882e:	6822      	ldr	r2, [r4, #0]
    8830:	6063      	str	r3, [r4, #4]
    8832:	33f9      	adds	r3, #249	; 0xf9
    8834:	33ff      	adds	r3, #255	; 0xff
    8836:	4313      	orrs	r3, r2
    8838:	6023      	str	r3, [r4, #0]
    883a:	4b5c      	ldr	r3, [pc, #368]	; (89ac <_scanf_i+0x1ec>)
    883c:	6822      	ldr	r2, [r4, #0]
    883e:	4013      	ands	r3, r2
    8840:	6023      	str	r3, [r4, #0]
    8842:	68a3      	ldr	r3, [r4, #8]
    8844:	1e5a      	subs	r2, r3, #1
    8846:	60a2      	str	r2, [r4, #8]
    8848:	b1b3      	cbz	r3, 8878 <_scanf_i+0xb8>
    884a:	6833      	ldr	r3, [r6, #0]
    884c:	1c5a      	adds	r2, r3, #1
    884e:	6032      	str	r2, [r6, #0]
    8850:	781b      	ldrb	r3, [r3, #0]
    8852:	9a03      	ldr	r2, [sp, #12]
    8854:	7013      	strb	r3, [r2, #0]
    8856:	6873      	ldr	r3, [r6, #4]
    8858:	1c55      	adds	r5, r2, #1
    885a:	3b01      	subs	r3, #1
    885c:	6073      	str	r3, [r6, #4]
    885e:	9503      	str	r5, [sp, #12]
    8860:	2b00      	cmp	r3, #0
    8862:	dc09      	bgt.n	8878 <_scanf_i+0xb8>
    8864:	f240 1380 	movw	r3, #384	; 0x180
    8868:	58e3      	ldr	r3, [r4, r3]
    886a:	0031      	movs	r1, r6
    886c:	9805      	ldr	r0, [sp, #20]
    886e:	9307      	str	r3, [sp, #28]
    8870:	4798      	blx	r3
    8872:	2800      	cmp	r0, #0
    8874:	d000      	beq.n	8878 <_scanf_i+0xb8>
    8876:	e080      	b.n	897a <_scanf_i+0x1ba>
    8878:	3701      	adds	r7, #1
    887a:	2f03      	cmp	r7, #3
    887c:	d1c9      	bne.n	8812 <_scanf_i+0x52>
    887e:	6863      	ldr	r3, [r4, #4]
    8880:	b90b      	cbnz	r3, 8886 <_scanf_i+0xc6>
    8882:	330a      	adds	r3, #10
    8884:	6063      	str	r3, [r4, #4]
    8886:	2110      	movs	r1, #16
    8888:	2700      	movs	r7, #0
    888a:	6863      	ldr	r3, [r4, #4]
    888c:	6960      	ldr	r0, [r4, #20]
    888e:	1ac9      	subs	r1, r1, r3
    8890:	4b47      	ldr	r3, [pc, #284]	; (89b0 <_scanf_i+0x1f0>)
    8892:	18c9      	adds	r1, r1, r3
    8894:	f000 f918 	bl	8ac8 <__sccl>
    8898:	9d03      	ldr	r5, [sp, #12]
    889a:	68a3      	ldr	r3, [r4, #8]
    889c:	6822      	ldr	r2, [r4, #0]
    889e:	9303      	str	r3, [sp, #12]
    88a0:	2b00      	cmp	r3, #0
    88a2:	d041      	beq.n	8928 <_scanf_i+0x168>
    88a4:	6831      	ldr	r1, [r6, #0]
    88a6:	6963      	ldr	r3, [r4, #20]
    88a8:	7808      	ldrb	r0, [r1, #0]
    88aa:	5c1b      	ldrb	r3, [r3, r0]
    88ac:	b3e3      	cbz	r3, 8928 <_scanf_i+0x168>
    88ae:	2830      	cmp	r0, #48	; 0x30
    88b0:	d12a      	bne.n	8908 <_scanf_i+0x148>
    88b2:	f640 0300 	movw	r3, #2048	; 0x800
    88b6:	421a      	tst	r2, r3
    88b8:	d026      	beq.n	8908 <_scanf_i+0x148>
    88ba:	9b04      	ldr	r3, [sp, #16]
    88bc:	3701      	adds	r7, #1
    88be:	b12b      	cbz	r3, 88cc <_scanf_i+0x10c>
    88c0:	001a      	movs	r2, r3
    88c2:	9b03      	ldr	r3, [sp, #12]
    88c4:	3a01      	subs	r2, #1
    88c6:	3301      	adds	r3, #1
    88c8:	9204      	str	r2, [sp, #16]
    88ca:	60a3      	str	r3, [r4, #8]
    88cc:	6873      	ldr	r3, [r6, #4]
    88ce:	3b01      	subs	r3, #1
    88d0:	6073      	str	r3, [r6, #4]
    88d2:	2b00      	cmp	r3, #0
    88d4:	dd1f      	ble.n	8916 <_scanf_i+0x156>
    88d6:	6833      	ldr	r3, [r6, #0]
    88d8:	3301      	adds	r3, #1
    88da:	6033      	str	r3, [r6, #0]
    88dc:	68a3      	ldr	r3, [r4, #8]
    88de:	3b01      	subs	r3, #1
    88e0:	60a3      	str	r3, [r4, #8]
    88e2:	e7da      	b.n	889a <_scanf_i+0xda>
    88e4:	4b33      	ldr	r3, [pc, #204]	; (89b4 <_scanf_i+0x1f4>)
    88e6:	e77a      	b.n	87de <_scanf_i+0x1e>
    88e8:	2f02      	cmp	r7, #2
    88ea:	d1aa      	bne.n	8842 <_scanf_i+0x82>
    88ec:	f240 6100 	movw	r1, #1536	; 0x600
    88f0:	f240 2300 	movw	r3, #512	; 0x200
    88f4:	6822      	ldr	r2, [r4, #0]
    88f6:	4011      	ands	r1, r2
    88f8:	4299      	cmp	r1, r3
    88fa:	d1c0      	bne.n	887e <_scanf_i+0xbe>
    88fc:	3bf1      	subs	r3, #241	; 0xf1
    88fe:	3bff      	subs	r3, #255	; 0xff
    8900:	6063      	str	r3, [r4, #4]
    8902:	33f0      	adds	r3, #240	; 0xf0
    8904:	4313      	orrs	r3, r2
    8906:	e79b      	b.n	8840 <_scanf_i+0x80>
    8908:	4b2b      	ldr	r3, [pc, #172]	; (89b8 <_scanf_i+0x1f8>)
    890a:	4013      	ands	r3, r2
    890c:	6023      	str	r3, [r4, #0]
    890e:	780b      	ldrb	r3, [r1, #0]
    8910:	702b      	strb	r3, [r5, #0]
    8912:	3501      	adds	r5, #1
    8914:	e7da      	b.n	88cc <_scanf_i+0x10c>
    8916:	f240 1380 	movw	r3, #384	; 0x180
    891a:	58e3      	ldr	r3, [r4, r3]
    891c:	0031      	movs	r1, r6
    891e:	9805      	ldr	r0, [sp, #20]
    8920:	9303      	str	r3, [sp, #12]
    8922:	4798      	blx	r3
    8924:	2800      	cmp	r0, #0
    8926:	d0d9      	beq.n	88dc <_scanf_i+0x11c>
    8928:	6823      	ldr	r3, [r4, #0]
    892a:	05db      	lsls	r3, r3, #23
    892c:	d50e      	bpl.n	894c <_scanf_i+0x18c>
    892e:	9b01      	ldr	r3, [sp, #4]
    8930:	429d      	cmp	r5, r3
    8932:	d907      	bls.n	8944 <_scanf_i+0x184>
    8934:	f240 137c 	movw	r3, #380	; 0x17c
    8938:	3d01      	subs	r5, #1
    893a:	0032      	movs	r2, r6
    893c:	7829      	ldrb	r1, [r5, #0]
    893e:	58e3      	ldr	r3, [r4, r3]
    8940:	9805      	ldr	r0, [sp, #20]
    8942:	4798      	blx	r3
    8944:	9b01      	ldr	r3, [sp, #4]
    8946:	2001      	movs	r0, #1
    8948:	429d      	cmp	r5, r3
    894a:	d029      	beq.n	89a0 <_scanf_i+0x1e0>
    894c:	6821      	ldr	r1, [r4, #0]
    894e:	2310      	movs	r3, #16
    8950:	000a      	movs	r2, r1
    8952:	401a      	ands	r2, r3
    8954:	4219      	tst	r1, r3
    8956:	d11c      	bne.n	8992 <_scanf_i+0x1d2>
    8958:	702a      	strb	r2, [r5, #0]
    895a:	6863      	ldr	r3, [r4, #4]
    895c:	9901      	ldr	r1, [sp, #4]
    895e:	9805      	ldr	r0, [sp, #20]
    8960:	9e06      	ldr	r6, [sp, #24]
    8962:	47b0      	blx	r6
    8964:	9b02      	ldr	r3, [sp, #8]
    8966:	6821      	ldr	r1, [r4, #0]
    8968:	681b      	ldr	r3, [r3, #0]
    896a:	068a      	lsls	r2, r1, #26
    896c:	d507      	bpl.n	897e <_scanf_i+0x1be>
    896e:	1d1a      	adds	r2, r3, #4
    8970:	9902      	ldr	r1, [sp, #8]
    8972:	600a      	str	r2, [r1, #0]
    8974:	681b      	ldr	r3, [r3, #0]
    8976:	6018      	str	r0, [r3, #0]
    8978:	e008      	b.n	898c <_scanf_i+0x1cc>
    897a:	2700      	movs	r7, #0
    897c:	e7d4      	b.n	8928 <_scanf_i+0x168>
    897e:	1d1a      	adds	r2, r3, #4
    8980:	07ce      	lsls	r6, r1, #31
    8982:	d5f5      	bpl.n	8970 <_scanf_i+0x1b0>
    8984:	9902      	ldr	r1, [sp, #8]
    8986:	600a      	str	r2, [r1, #0]
    8988:	681b      	ldr	r3, [r3, #0]
    898a:	8018      	strh	r0, [r3, #0]
    898c:	68e3      	ldr	r3, [r4, #12]
    898e:	3301      	adds	r3, #1
    8990:	60e3      	str	r3, [r4, #12]
    8992:	2000      	movs	r0, #0
    8994:	9b01      	ldr	r3, [sp, #4]
    8996:	1aed      	subs	r5, r5, r3
    8998:	6923      	ldr	r3, [r4, #16]
    899a:	19ef      	adds	r7, r5, r7
    899c:	19df      	adds	r7, r3, r7
    899e:	6127      	str	r7, [r4, #16]
    89a0:	b00d      	add	sp, #52	; 0x34
    89a2:	bdf0      	pop	{r4, r5, r6, r7, pc}
    89a4:	000097d4 	.word	0x000097d4
    89a8:	00008c39 	.word	0x00008c39
    89ac:	fffffaff 	.word	0xfffffaff
    89b0:	000097c0 	.word	0x000097c0
    89b4:	00006ff9 	.word	0x00006ff9
    89b8:	fffff6ff 	.word	0xfffff6ff

000089bc <_read_r>:
    89bc:	b570      	push	{r4, r5, r6, lr}
    89be:	0004      	movs	r4, r0
    89c0:	0008      	movs	r0, r1
    89c2:	0011      	movs	r1, r2
    89c4:	001a      	movs	r2, r3
    89c6:	2300      	movs	r3, #0
    89c8:	4d04      	ldr	r5, [pc, #16]	; (89dc <_read_r+0x20>)
    89ca:	602b      	str	r3, [r5, #0]
    89cc:	f7fd fd6a 	bl	64a4 <_read>
    89d0:	1c43      	adds	r3, r0, #1
    89d2:	d102      	bne.n	89da <_read_r+0x1e>
    89d4:	682b      	ldr	r3, [r5, #0]
    89d6:	b103      	cbz	r3, 89da <_read_r+0x1e>
    89d8:	6023      	str	r3, [r4, #0]
    89da:	bd70      	pop	{r4, r5, r6, pc}
    89dc:	2000209c 	.word	0x2000209c

000089e0 <cleanup_glue>:
    89e0:	b570      	push	{r4, r5, r6, lr}
    89e2:	000d      	movs	r5, r1
    89e4:	6809      	ldr	r1, [r1, #0]
    89e6:	0004      	movs	r4, r0
    89e8:	b109      	cbz	r1, 89ee <cleanup_glue+0xe>
    89ea:	f7ff fff9 	bl	89e0 <cleanup_glue>
    89ee:	0029      	movs	r1, r5
    89f0:	0020      	movs	r0, r4
    89f2:	f7fe ff4d 	bl	7890 <_free_r>
    89f6:	bd70      	pop	{r4, r5, r6, pc}

000089f8 <_reclaim_reent>:
    89f8:	4b2a      	ldr	r3, [pc, #168]	; (8aa4 <_reclaim_reent+0xac>)
    89fa:	b570      	push	{r4, r5, r6, lr}
    89fc:	681b      	ldr	r3, [r3, #0]
    89fe:	0004      	movs	r4, r0
    8a00:	4283      	cmp	r3, r0
    8a02:	d03d      	beq.n	8a80 <_reclaim_reent+0x88>
    8a04:	6a43      	ldr	r3, [r0, #36]	; 0x24
    8a06:	b14b      	cbz	r3, 8a1c <_reclaim_reent+0x24>
    8a08:	68db      	ldr	r3, [r3, #12]
    8a0a:	2500      	movs	r5, #0
    8a0c:	2b00      	cmp	r3, #0
    8a0e:	d13b      	bne.n	8a88 <_reclaim_reent+0x90>
    8a10:	6a63      	ldr	r3, [r4, #36]	; 0x24
    8a12:	6819      	ldr	r1, [r3, #0]
    8a14:	b111      	cbz	r1, 8a1c <_reclaim_reent+0x24>
    8a16:	0020      	movs	r0, r4
    8a18:	f7fe ff3a 	bl	7890 <_free_r>
    8a1c:	6961      	ldr	r1, [r4, #20]
    8a1e:	b111      	cbz	r1, 8a26 <_reclaim_reent+0x2e>
    8a20:	0020      	movs	r0, r4
    8a22:	f7fe ff35 	bl	7890 <_free_r>
    8a26:	6a61      	ldr	r1, [r4, #36]	; 0x24
    8a28:	b111      	cbz	r1, 8a30 <_reclaim_reent+0x38>
    8a2a:	0020      	movs	r0, r4
    8a2c:	f7fe ff30 	bl	7890 <_free_r>
    8a30:	6ba1      	ldr	r1, [r4, #56]	; 0x38
    8a32:	b111      	cbz	r1, 8a3a <_reclaim_reent+0x42>
    8a34:	0020      	movs	r0, r4
    8a36:	f7fe ff2b 	bl	7890 <_free_r>
    8a3a:	6be1      	ldr	r1, [r4, #60]	; 0x3c
    8a3c:	b111      	cbz	r1, 8a44 <_reclaim_reent+0x4c>
    8a3e:	0020      	movs	r0, r4
    8a40:	f7fe ff26 	bl	7890 <_free_r>
    8a44:	6c21      	ldr	r1, [r4, #64]	; 0x40
    8a46:	b111      	cbz	r1, 8a4e <_reclaim_reent+0x56>
    8a48:	0020      	movs	r0, r4
    8a4a:	f7fe ff21 	bl	7890 <_free_r>
    8a4e:	6de1      	ldr	r1, [r4, #92]	; 0x5c
    8a50:	b111      	cbz	r1, 8a58 <_reclaim_reent+0x60>
    8a52:	0020      	movs	r0, r4
    8a54:	f7fe ff1c 	bl	7890 <_free_r>
    8a58:	6da1      	ldr	r1, [r4, #88]	; 0x58
    8a5a:	b111      	cbz	r1, 8a62 <_reclaim_reent+0x6a>
    8a5c:	0020      	movs	r0, r4
    8a5e:	f7fe ff17 	bl	7890 <_free_r>
    8a62:	6b61      	ldr	r1, [r4, #52]	; 0x34
    8a64:	b111      	cbz	r1, 8a6c <_reclaim_reent+0x74>
    8a66:	0020      	movs	r0, r4
    8a68:	f7fe ff12 	bl	7890 <_free_r>
    8a6c:	69a3      	ldr	r3, [r4, #24]
    8a6e:	b13b      	cbz	r3, 8a80 <_reclaim_reent+0x88>
    8a70:	0020      	movs	r0, r4
    8a72:	6aa3      	ldr	r3, [r4, #40]	; 0x28
    8a74:	4798      	blx	r3
    8a76:	6ca1      	ldr	r1, [r4, #72]	; 0x48
    8a78:	b111      	cbz	r1, 8a80 <_reclaim_reent+0x88>
    8a7a:	0020      	movs	r0, r4
    8a7c:	f7ff ffb0 	bl	89e0 <cleanup_glue>
    8a80:	bd70      	pop	{r4, r5, r6, pc}
    8a82:	5949      	ldr	r1, [r1, r5]
    8a84:	b941      	cbnz	r1, 8a98 <_reclaim_reent+0xa0>
    8a86:	3504      	adds	r5, #4
    8a88:	6a63      	ldr	r3, [r4, #36]	; 0x24
    8a8a:	68d9      	ldr	r1, [r3, #12]
    8a8c:	2d80      	cmp	r5, #128	; 0x80
    8a8e:	d1f8      	bne.n	8a82 <_reclaim_reent+0x8a>
    8a90:	0020      	movs	r0, r4
    8a92:	f7fe fefd 	bl	7890 <_free_r>
    8a96:	e7bb      	b.n	8a10 <_reclaim_reent+0x18>
    8a98:	680e      	ldr	r6, [r1, #0]
    8a9a:	0020      	movs	r0, r4
    8a9c:	f7fe fef8 	bl	7890 <_free_r>
    8aa0:	0031      	movs	r1, r6
    8aa2:	e7ef      	b.n	8a84 <_reclaim_reent+0x8c>
    8aa4:	20002000 	.word	0x20002000

00008aa8 <_sbrk_r>:
    8aa8:	2300      	movs	r3, #0
    8aaa:	b570      	push	{r4, r5, r6, lr}
    8aac:	4d05      	ldr	r5, [pc, #20]	; (8ac4 <_sbrk_r+0x1c>)
    8aae:	0004      	movs	r4, r0
    8ab0:	0008      	movs	r0, r1
    8ab2:	602b      	str	r3, [r5, #0]
    8ab4:	f000 fca0 	bl	93f8 <_sbrk>
    8ab8:	1c43      	adds	r3, r0, #1
    8aba:	d102      	bne.n	8ac2 <_sbrk_r+0x1a>
    8abc:	682b      	ldr	r3, [r5, #0]
    8abe:	b103      	cbz	r3, 8ac2 <_sbrk_r+0x1a>
    8ac0:	6023      	str	r3, [r4, #0]
    8ac2:	bd70      	pop	{r4, r5, r6, pc}
    8ac4:	2000209c 	.word	0x2000209c

00008ac8 <__sccl>:
    8ac8:	b5f0      	push	{r4, r5, r6, r7, lr}
    8aca:	780b      	ldrb	r3, [r1, #0]
    8acc:	0004      	movs	r4, r0
    8ace:	2b5e      	cmp	r3, #94	; 0x5e
    8ad0:	d00b      	beq.n	8aea <__sccl+0x22>
    8ad2:	1c48      	adds	r0, r1, #1
    8ad4:	2100      	movs	r1, #0
    8ad6:	0022      	movs	r2, r4
    8ad8:	1c65      	adds	r5, r4, #1
    8ada:	35ff      	adds	r5, #255	; 0xff
    8adc:	7011      	strb	r1, [r2, #0]
    8ade:	3201      	adds	r2, #1
    8ae0:	42aa      	cmp	r2, r5
    8ae2:	d1fb      	bne.n	8adc <__sccl+0x14>
    8ae4:	b92b      	cbnz	r3, 8af2 <__sccl+0x2a>
    8ae6:	3801      	subs	r0, #1
    8ae8:	bdf0      	pop	{r4, r5, r6, r7, pc}
    8aea:	784b      	ldrb	r3, [r1, #1]
    8aec:	1c88      	adds	r0, r1, #2
    8aee:	2101      	movs	r1, #1
    8af0:	e7f1      	b.n	8ad6 <__sccl+0xe>
    8af2:	2201      	movs	r2, #1
    8af4:	262d      	movs	r6, #45	; 0x2d
    8af6:	4051      	eors	r1, r2
    8af8:	0002      	movs	r2, r0
    8afa:	54e1      	strb	r1, [r4, r3]
    8afc:	7815      	ldrb	r5, [r2, #0]
    8afe:	1c50      	adds	r0, r2, #1
    8b00:	2d2d      	cmp	r5, #45	; 0x2d
    8b02:	d008      	beq.n	8b16 <__sccl+0x4e>
    8b04:	2d5d      	cmp	r5, #93	; 0x5d
    8b06:	d0ef      	beq.n	8ae8 <__sccl+0x20>
    8b08:	b90d      	cbnz	r5, 8b0e <__sccl+0x46>
    8b0a:	0010      	movs	r0, r2
    8b0c:	e7ec      	b.n	8ae8 <__sccl+0x20>
    8b0e:	002b      	movs	r3, r5
    8b10:	e7f2      	b.n	8af8 <__sccl+0x30>
    8b12:	0033      	movs	r3, r6
    8b14:	e7f0      	b.n	8af8 <__sccl+0x30>
    8b16:	7855      	ldrb	r5, [r2, #1]
    8b18:	2d5d      	cmp	r5, #93	; 0x5d
    8b1a:	d0fa      	beq.n	8b12 <__sccl+0x4a>
    8b1c:	42ab      	cmp	r3, r5
    8b1e:	dcf8      	bgt.n	8b12 <__sccl+0x4a>
    8b20:	0018      	movs	r0, r3
    8b22:	3202      	adds	r2, #2
    8b24:	3001      	adds	r0, #1
    8b26:	5421      	strb	r1, [r4, r0]
    8b28:	4285      	cmp	r5, r0
    8b2a:	dcfb      	bgt.n	8b24 <__sccl+0x5c>
    8b2c:	2000      	movs	r0, #0
    8b2e:	1c5f      	adds	r7, r3, #1
    8b30:	42ab      	cmp	r3, r5
    8b32:	da01      	bge.n	8b38 <__sccl+0x70>
    8b34:	1ae8      	subs	r0, r5, r3
    8b36:	3801      	subs	r0, #1
    8b38:	183b      	adds	r3, r7, r0
    8b3a:	e7df      	b.n	8afc <__sccl+0x34>

00008b3c <_strtoul_l.constprop.0>:
    8b3c:	b5f0      	push	{r4, r5, r6, r7, lr}
    8b3e:	b085      	sub	sp, #20
    8b40:	9201      	str	r2, [sp, #4]
    8b42:	4a3c      	ldr	r2, [pc, #240]	; (8c34 <_strtoul_l.constprop.0+0xf8>)
    8b44:	9003      	str	r0, [sp, #12]
    8b46:	4694      	mov	ip, r2
    8b48:	0008      	movs	r0, r1
    8b4a:	2508      	movs	r5, #8
    8b4c:	9100      	str	r1, [sp, #0]
    8b4e:	0004      	movs	r4, r0
    8b50:	4666      	mov	r6, ip
    8b52:	7822      	ldrb	r2, [r4, #0]
    8b54:	3001      	adds	r0, #1
    8b56:	5cb6      	ldrb	r6, [r6, r2]
    8b58:	0001      	movs	r1, r0
    8b5a:	0037      	movs	r7, r6
    8b5c:	402f      	ands	r7, r5
    8b5e:	422e      	tst	r6, r5
    8b60:	d1f5      	bne.n	8b4e <_strtoul_l.constprop.0+0x12>
    8b62:	2a2d      	cmp	r2, #45	; 0x2d
    8b64:	d131      	bne.n	8bca <_strtoul_l.constprop.0+0x8e>
    8b66:	f240 0c01 	movw	ip, #1
    8b6a:	7802      	ldrb	r2, [r0, #0]
    8b6c:	1ca1      	adds	r1, r4, #2
    8b6e:	2b00      	cmp	r3, #0
    8b70:	d05c      	beq.n	8c2c <_strtoul_l.constprop.0+0xf0>
    8b72:	2b10      	cmp	r3, #16
    8b74:	d109      	bne.n	8b8a <_strtoul_l.constprop.0+0x4e>
    8b76:	2a30      	cmp	r2, #48	; 0x30
    8b78:	d107      	bne.n	8b8a <_strtoul_l.constprop.0+0x4e>
    8b7a:	2020      	movs	r0, #32
    8b7c:	780a      	ldrb	r2, [r1, #0]
    8b7e:	4382      	bics	r2, r0
    8b80:	2a58      	cmp	r2, #88	; 0x58
    8b82:	d14e      	bne.n	8c22 <_strtoul_l.constprop.0+0xe6>
    8b84:	2310      	movs	r3, #16
    8b86:	784a      	ldrb	r2, [r1, #1]
    8b88:	3102      	adds	r1, #2
    8b8a:	2601      	movs	r6, #1
    8b8c:	4276      	negs	r6, r6
    8b8e:	fbb6 f7f3 	udiv	r7, r6, r3
    8b92:	0018      	movs	r0, r3
    8b94:	4378      	muls	r0, r7
    8b96:	2400      	movs	r4, #0
    8b98:	43c0      	mvns	r0, r0
    8b9a:	9002      	str	r0, [sp, #8]
    8b9c:	0020      	movs	r0, r4
    8b9e:	0015      	movs	r5, r2
    8ba0:	3d30      	subs	r5, #48	; 0x30
    8ba2:	2d09      	cmp	r5, #9
    8ba4:	d817      	bhi.n	8bd6 <_strtoul_l.constprop.0+0x9a>
    8ba6:	002a      	movs	r2, r5
    8ba8:	4293      	cmp	r3, r2
    8baa:	dd22      	ble.n	8bf2 <_strtoul_l.constprop.0+0xb6>
    8bac:	2c00      	cmp	r4, #0
    8bae:	db1e      	blt.n	8bee <_strtoul_l.constprop.0+0xb2>
    8bb0:	0034      	movs	r4, r6
    8bb2:	4287      	cmp	r7, r0
    8bb4:	d306      	bcc.n	8bc4 <_strtoul_l.constprop.0+0x88>
    8bb6:	d102      	bne.n	8bbe <_strtoul_l.constprop.0+0x82>
    8bb8:	9d02      	ldr	r5, [sp, #8]
    8bba:	4295      	cmp	r5, r2
    8bbc:	db02      	blt.n	8bc4 <_strtoul_l.constprop.0+0x88>
    8bbe:	2401      	movs	r4, #1
    8bc0:	4358      	muls	r0, r3
    8bc2:	1810      	adds	r0, r2, r0
    8bc4:	780a      	ldrb	r2, [r1, #0]
    8bc6:	3101      	adds	r1, #1
    8bc8:	e7e9      	b.n	8b9e <_strtoul_l.constprop.0+0x62>
    8bca:	46bc      	mov	ip, r7
    8bcc:	2a2b      	cmp	r2, #43	; 0x2b
    8bce:	d1ce      	bne.n	8b6e <_strtoul_l.constprop.0+0x32>
    8bd0:	7802      	ldrb	r2, [r0, #0]
    8bd2:	1ca1      	adds	r1, r4, #2
    8bd4:	e7cb      	b.n	8b6e <_strtoul_l.constprop.0+0x32>
    8bd6:	0015      	movs	r5, r2
    8bd8:	3d41      	subs	r5, #65	; 0x41
    8bda:	2d19      	cmp	r5, #25
    8bdc:	d801      	bhi.n	8be2 <_strtoul_l.constprop.0+0xa6>
    8bde:	3a37      	subs	r2, #55	; 0x37
    8be0:	e7e2      	b.n	8ba8 <_strtoul_l.constprop.0+0x6c>
    8be2:	0015      	movs	r5, r2
    8be4:	3d61      	subs	r5, #97	; 0x61
    8be6:	2d19      	cmp	r5, #25
    8be8:	d803      	bhi.n	8bf2 <_strtoul_l.constprop.0+0xb6>
    8bea:	3a57      	subs	r2, #87	; 0x57
    8bec:	e7dc      	b.n	8ba8 <_strtoul_l.constprop.0+0x6c>
    8bee:	0034      	movs	r4, r6
    8bf0:	e7e8      	b.n	8bc4 <_strtoul_l.constprop.0+0x88>
    8bf2:	2c00      	cmp	r4, #0
    8bf4:	da08      	bge.n	8c08 <_strtoul_l.constprop.0+0xcc>
    8bf6:	2322      	movs	r3, #34	; 0x22
    8bf8:	2001      	movs	r0, #1
    8bfa:	9a03      	ldr	r2, [sp, #12]
    8bfc:	4240      	negs	r0, r0
    8bfe:	6013      	str	r3, [r2, #0]
    8c00:	9b01      	ldr	r3, [sp, #4]
    8c02:	b943      	cbnz	r3, 8c16 <_strtoul_l.constprop.0+0xda>
    8c04:	b005      	add	sp, #20
    8c06:	bdf0      	pop	{r4, r5, r6, r7, pc}
    8c08:	4663      	mov	r3, ip
    8c0a:	b103      	cbz	r3, 8c0e <_strtoul_l.constprop.0+0xd2>
    8c0c:	4240      	negs	r0, r0
    8c0e:	9b01      	ldr	r3, [sp, #4]
    8c10:	2b00      	cmp	r3, #0
    8c12:	d0f7      	beq.n	8c04 <_strtoul_l.constprop.0+0xc8>
    8c14:	b10c      	cbz	r4, 8c1a <_strtoul_l.constprop.0+0xde>
    8c16:	1e4b      	subs	r3, r1, #1
    8c18:	9300      	str	r3, [sp, #0]
    8c1a:	9b01      	ldr	r3, [sp, #4]
    8c1c:	9a00      	ldr	r2, [sp, #0]
    8c1e:	601a      	str	r2, [r3, #0]
    8c20:	e7f0      	b.n	8c04 <_strtoul_l.constprop.0+0xc8>
    8c22:	2230      	movs	r2, #48	; 0x30
    8c24:	2b00      	cmp	r3, #0
    8c26:	d1b0      	bne.n	8b8a <_strtoul_l.constprop.0+0x4e>
    8c28:	3308      	adds	r3, #8
    8c2a:	e7ae      	b.n	8b8a <_strtoul_l.constprop.0+0x4e>
    8c2c:	2a30      	cmp	r2, #48	; 0x30
    8c2e:	d0a4      	beq.n	8b7a <_strtoul_l.constprop.0+0x3e>
    8c30:	230a      	movs	r3, #10
    8c32:	e7aa      	b.n	8b8a <_strtoul_l.constprop.0+0x4e>
    8c34:	0000961d 	.word	0x0000961d

00008c38 <_strtoul_r>:
    8c38:	b510      	push	{r4, lr}
    8c3a:	f7ff ff7f 	bl	8b3c <_strtoul_l.constprop.0>
    8c3e:	bd10      	pop	{r4, pc}

00008c40 <strtoul_l>:
    8c40:	b510      	push	{r4, lr}
    8c42:	0013      	movs	r3, r2
    8c44:	000a      	movs	r2, r1
    8c46:	0001      	movs	r1, r0
    8c48:	4802      	ldr	r0, [pc, #8]	; (8c54 <strtoul_l+0x14>)
    8c4a:	6800      	ldr	r0, [r0, #0]
    8c4c:	f7ff ff76 	bl	8b3c <_strtoul_l.constprop.0>
    8c50:	bd10      	pop	{r4, pc}
    8c52:	bf00      	nop
    8c54:	20002000 	.word	0x20002000

00008c58 <strtoul>:
    8c58:	b510      	push	{r4, lr}
    8c5a:	0013      	movs	r3, r2
    8c5c:	000a      	movs	r2, r1
    8c5e:	0001      	movs	r1, r0
    8c60:	4802      	ldr	r0, [pc, #8]	; (8c6c <strtoul+0x14>)
    8c62:	6800      	ldr	r0, [r0, #0]
    8c64:	f7ff ff6a 	bl	8b3c <_strtoul_l.constprop.0>
    8c68:	bd10      	pop	{r4, pc}
    8c6a:	bf00      	nop
    8c6c:	20002000 	.word	0x20002000

00008c70 <__submore>:
    8c70:	000b      	movs	r3, r1
    8c72:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    8c74:	6b4d      	ldr	r5, [r1, #52]	; 0x34
    8c76:	3344      	adds	r3, #68	; 0x44
    8c78:	000c      	movs	r4, r1
    8c7a:	429d      	cmp	r5, r3
    8c7c:	d11d      	bne.n	8cba <__submore+0x4a>
    8c7e:	f240 4600 	movw	r6, #1024	; 0x400
    8c82:	0031      	movs	r1, r6
    8c84:	f7fe fe4a 	bl	791c <_malloc_r>
    8c88:	b910      	cbnz	r0, 8c90 <__submore+0x20>
    8c8a:	2001      	movs	r0, #1
    8c8c:	4240      	negs	r0, r0
    8c8e:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    8c90:	0023      	movs	r3, r4
    8c92:	6360      	str	r0, [r4, #52]	; 0x34
    8c94:	63a6      	str	r6, [r4, #56]	; 0x38
    8c96:	3346      	adds	r3, #70	; 0x46
    8c98:	781a      	ldrb	r2, [r3, #0]
    8c9a:	f240 33ff 	movw	r3, #1023	; 0x3ff
    8c9e:	54c2      	strb	r2, [r0, r3]
    8ca0:	0023      	movs	r3, r4
    8ca2:	3345      	adds	r3, #69	; 0x45
    8ca4:	781a      	ldrb	r2, [r3, #0]
    8ca6:	f240 33fe 	movw	r3, #1022	; 0x3fe
    8caa:	54c2      	strb	r2, [r0, r3]
    8cac:	782a      	ldrb	r2, [r5, #0]
    8cae:	3b01      	subs	r3, #1
    8cb0:	54c2      	strb	r2, [r0, r3]
    8cb2:	18c0      	adds	r0, r0, r3
    8cb4:	6020      	str	r0, [r4, #0]
    8cb6:	2000      	movs	r0, #0
    8cb8:	e7e9      	b.n	8c8e <__submore+0x1e>
    8cba:	6b8e      	ldr	r6, [r1, #56]	; 0x38
    8cbc:	0029      	movs	r1, r5
    8cbe:	0073      	lsls	r3, r6, #1
    8cc0:	001a      	movs	r2, r3
    8cc2:	9301      	str	r3, [sp, #4]
    8cc4:	f000 fa78 	bl	91b8 <_realloc_r>
    8cc8:	0005      	movs	r5, r0
    8cca:	2800      	cmp	r0, #0
    8ccc:	d0dd      	beq.n	8c8a <__submore+0x1a>
    8cce:	1987      	adds	r7, r0, r6
    8cd0:	0001      	movs	r1, r0
    8cd2:	0032      	movs	r2, r6
    8cd4:	0038      	movs	r0, r7
    8cd6:	f000 fa42 	bl	915e <memcpy>
    8cda:	9b01      	ldr	r3, [sp, #4]
    8cdc:	6027      	str	r7, [r4, #0]
    8cde:	6365      	str	r5, [r4, #52]	; 0x34
    8ce0:	63a3      	str	r3, [r4, #56]	; 0x38
    8ce2:	e7e8      	b.n	8cb6 <__submore+0x46>

00008ce4 <_ungetc_r>:
    8ce4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    8ce6:	0006      	movs	r6, r0
    8ce8:	000d      	movs	r5, r1
    8cea:	0014      	movs	r4, r2
    8cec:	1c4b      	adds	r3, r1, #1
    8cee:	d103      	bne.n	8cf8 <_ungetc_r+0x14>
    8cf0:	2501      	movs	r5, #1
    8cf2:	426d      	negs	r5, r5
    8cf4:	0028      	movs	r0, r5
    8cf6:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    8cf8:	b118      	cbz	r0, 8d02 <_ungetc_r+0x1e>
    8cfa:	6983      	ldr	r3, [r0, #24]
    8cfc:	b90b      	cbnz	r3, 8d02 <_ungetc_r+0x1e>
    8cfe:	f7fe fc45 	bl	758c <__sinit>
    8d02:	4b40      	ldr	r3, [pc, #256]	; (8e04 <_ungetc_r+0x120>)
    8d04:	429c      	cmp	r4, r3
    8d06:	d11c      	bne.n	8d42 <_ungetc_r+0x5e>
    8d08:	6874      	ldr	r4, [r6, #4]
    8d0a:	6e63      	ldr	r3, [r4, #100]	; 0x64
    8d0c:	07db      	lsls	r3, r3, #31
    8d0e:	d405      	bmi.n	8d1c <_ungetc_r+0x38>
    8d10:	89a3      	ldrh	r3, [r4, #12]
    8d12:	059b      	lsls	r3, r3, #22
    8d14:	d402      	bmi.n	8d1c <_ungetc_r+0x38>
    8d16:	6da0      	ldr	r0, [r4, #88]	; 0x58
    8d18:	f7fe fd2e 	bl	7778 <__retarget_lock_acquire_recursive>
    8d1c:	2220      	movs	r2, #32
    8d1e:	89a3      	ldrh	r3, [r4, #12]
    8d20:	4393      	bics	r3, r2
    8d22:	b21b      	sxth	r3, r3
    8d24:	81a3      	strh	r3, [r4, #12]
    8d26:	b29a      	uxth	r2, r3
    8d28:	075b      	lsls	r3, r3, #29
    8d2a:	d42c      	bmi.n	8d86 <_ungetc_r+0xa2>
    8d2c:	06d3      	lsls	r3, r2, #27
    8d2e:	d412      	bmi.n	8d56 <_ungetc_r+0x72>
    8d30:	6e63      	ldr	r3, [r4, #100]	; 0x64
    8d32:	07db      	lsls	r3, r3, #31
    8d34:	d4dc      	bmi.n	8cf0 <_ungetc_r+0xc>
    8d36:	0593      	lsls	r3, r2, #22
    8d38:	d4da      	bmi.n	8cf0 <_ungetc_r+0xc>
    8d3a:	6da0      	ldr	r0, [r4, #88]	; 0x58
    8d3c:	f7fe fd22 	bl	7784 <__retarget_lock_release_recursive>
    8d40:	e7d6      	b.n	8cf0 <_ungetc_r+0xc>
    8d42:	4b31      	ldr	r3, [pc, #196]	; (8e08 <_ungetc_r+0x124>)
    8d44:	429c      	cmp	r4, r3
    8d46:	d101      	bne.n	8d4c <_ungetc_r+0x68>
    8d48:	68b4      	ldr	r4, [r6, #8]
    8d4a:	e7de      	b.n	8d0a <_ungetc_r+0x26>
    8d4c:	4b2f      	ldr	r3, [pc, #188]	; (8e0c <_ungetc_r+0x128>)
    8d4e:	429c      	cmp	r4, r3
    8d50:	d1db      	bne.n	8d0a <_ungetc_r+0x26>
    8d52:	68f4      	ldr	r4, [r6, #12]
    8d54:	e7d9      	b.n	8d0a <_ungetc_r+0x26>
    8d56:	2708      	movs	r7, #8
    8d58:	423a      	tst	r2, r7
    8d5a:	d010      	beq.n	8d7e <_ungetc_r+0x9a>
    8d5c:	0021      	movs	r1, r4
    8d5e:	0030      	movs	r0, r6
    8d60:	f7fe fb42 	bl	73e8 <_fflush_r>
    8d64:	b130      	cbz	r0, 8d74 <_ungetc_r+0x90>
    8d66:	6e63      	ldr	r3, [r4, #100]	; 0x64
    8d68:	07db      	lsls	r3, r3, #31
    8d6a:	d4c1      	bmi.n	8cf0 <_ungetc_r+0xc>
    8d6c:	89a3      	ldrh	r3, [r4, #12]
    8d6e:	059b      	lsls	r3, r3, #22
    8d70:	d4be      	bmi.n	8cf0 <_ungetc_r+0xc>
    8d72:	e7e2      	b.n	8d3a <_ungetc_r+0x56>
    8d74:	89a3      	ldrh	r3, [r4, #12]
    8d76:	60a0      	str	r0, [r4, #8]
    8d78:	43bb      	bics	r3, r7
    8d7a:	81a3      	strh	r3, [r4, #12]
    8d7c:	61a0      	str	r0, [r4, #24]
    8d7e:	2304      	movs	r3, #4
    8d80:	89a2      	ldrh	r2, [r4, #12]
    8d82:	4313      	orrs	r3, r2
    8d84:	81a3      	strh	r3, [r4, #12]
    8d86:	6b62      	ldr	r2, [r4, #52]	; 0x34
    8d88:	b2ef      	uxtb	r7, r5
    8d8a:	6863      	ldr	r3, [r4, #4]
    8d8c:	b2ed      	uxtb	r5, r5
    8d8e:	b1ca      	cbz	r2, 8dc4 <_ungetc_r+0xe0>
    8d90:	6ba2      	ldr	r2, [r4, #56]	; 0x38
    8d92:	429a      	cmp	r2, r3
    8d94:	dc05      	bgt.n	8da2 <_ungetc_r+0xbe>
    8d96:	0021      	movs	r1, r4
    8d98:	0030      	movs	r0, r6
    8d9a:	f7ff ff69 	bl	8c70 <__submore>
    8d9e:	2800      	cmp	r0, #0
    8da0:	d1e1      	bne.n	8d66 <_ungetc_r+0x82>
    8da2:	6823      	ldr	r3, [r4, #0]
    8da4:	3b01      	subs	r3, #1
    8da6:	6023      	str	r3, [r4, #0]
    8da8:	701f      	strb	r7, [r3, #0]
    8daa:	6863      	ldr	r3, [r4, #4]
    8dac:	3301      	adds	r3, #1
    8dae:	6063      	str	r3, [r4, #4]
    8db0:	6e63      	ldr	r3, [r4, #100]	; 0x64
    8db2:	07db      	lsls	r3, r3, #31
    8db4:	d49e      	bmi.n	8cf4 <_ungetc_r+0x10>
    8db6:	89a3      	ldrh	r3, [r4, #12]
    8db8:	059b      	lsls	r3, r3, #22
    8dba:	d49b      	bmi.n	8cf4 <_ungetc_r+0x10>
    8dbc:	6da0      	ldr	r0, [r4, #88]	; 0x58
    8dbe:	f7fe fce1 	bl	7784 <__retarget_lock_release_recursive>
    8dc2:	e797      	b.n	8cf4 <_ungetc_r+0x10>
    8dc4:	2201      	movs	r2, #1
    8dc6:	6e66      	ldr	r6, [r4, #100]	; 0x64
    8dc8:	6920      	ldr	r0, [r4, #16]
    8dca:	6821      	ldr	r1, [r4, #0]
    8dcc:	4032      	ands	r2, r6
    8dce:	b160      	cbz	r0, 8dea <_ungetc_r+0x106>
    8dd0:	4288      	cmp	r0, r1
    8dd2:	d20a      	bcs.n	8dea <_ungetc_r+0x106>
    8dd4:	1e48      	subs	r0, r1, #1
    8dd6:	7806      	ldrb	r6, [r0, #0]
    8dd8:	42ae      	cmp	r6, r5
    8dda:	d106      	bne.n	8dea <_ungetc_r+0x106>
    8ddc:	6020      	str	r0, [r4, #0]
    8dde:	3301      	adds	r3, #1
    8de0:	6063      	str	r3, [r4, #4]
    8de2:	2a00      	cmp	r2, #0
    8de4:	d000      	beq.n	8de8 <_ungetc_r+0x104>
    8de6:	e785      	b.n	8cf4 <_ungetc_r+0x10>
    8de8:	e7e5      	b.n	8db6 <_ungetc_r+0xd2>
    8dea:	6423      	str	r3, [r4, #64]	; 0x40
    8dec:	0023      	movs	r3, r4
    8dee:	3344      	adds	r3, #68	; 0x44
    8df0:	6363      	str	r3, [r4, #52]	; 0x34
    8df2:	2303      	movs	r3, #3
    8df4:	63a3      	str	r3, [r4, #56]	; 0x38
    8df6:	0023      	movs	r3, r4
    8df8:	3346      	adds	r3, #70	; 0x46
    8dfa:	63e1      	str	r1, [r4, #60]	; 0x3c
    8dfc:	701f      	strb	r7, [r3, #0]
    8dfe:	6023      	str	r3, [r4, #0]
    8e00:	2301      	movs	r3, #1
    8e02:	e7ed      	b.n	8de0 <_ungetc_r+0xfc>
    8e04:	00009760 	.word	0x00009760
    8e08:	00009740 	.word	0x00009740
    8e0c:	00009720 	.word	0x00009720

00008e10 <ungetc>:
    8e10:	b510      	push	{r4, lr}
    8e12:	4b03      	ldr	r3, [pc, #12]	; (8e20 <ungetc+0x10>)
    8e14:	000a      	movs	r2, r1
    8e16:	0001      	movs	r1, r0
    8e18:	6818      	ldr	r0, [r3, #0]
    8e1a:	f7ff ff63 	bl	8ce4 <_ungetc_r>
    8e1e:	bd10      	pop	{r4, pc}
    8e20:	20002000 	.word	0x20002000

00008e24 <abort>:
    8e24:	2006      	movs	r0, #6
    8e26:	b510      	push	{r4, lr}
    8e28:	f000 fa69 	bl	92fe <raise>
    8e2c:	2001      	movs	r0, #1
    8e2e:	f000 faf1 	bl	9414 <_exit>
	...

00008e34 <_fstat_r>:
    8e34:	2300      	movs	r3, #0
    8e36:	b570      	push	{r4, r5, r6, lr}
    8e38:	4d06      	ldr	r5, [pc, #24]	; (8e54 <_fstat_r+0x20>)
    8e3a:	0004      	movs	r4, r0
    8e3c:	0008      	movs	r0, r1
    8e3e:	0011      	movs	r1, r2
    8e40:	602b      	str	r3, [r5, #0]
    8e42:	f000 fab1 	bl	93a8 <_fstat>
    8e46:	1c43      	adds	r3, r0, #1
    8e48:	d102      	bne.n	8e50 <_fstat_r+0x1c>
    8e4a:	682b      	ldr	r3, [r5, #0]
    8e4c:	b103      	cbz	r3, 8e50 <_fstat_r+0x1c>
    8e4e:	6023      	str	r3, [r4, #0]
    8e50:	bd70      	pop	{r4, r5, r6, pc}
    8e52:	bf00      	nop
    8e54:	2000209c 	.word	0x2000209c

00008e58 <__sfvwrite_r>:
    8e58:	b5f0      	push	{r4, r5, r6, r7, lr}
    8e5a:	6893      	ldr	r3, [r2, #8]
    8e5c:	b087      	sub	sp, #28
    8e5e:	000c      	movs	r4, r1
    8e60:	0017      	movs	r7, r2
    8e62:	9003      	str	r0, [sp, #12]
    8e64:	b913      	cbnz	r3, 8e6c <__sfvwrite_r+0x14>
    8e66:	2000      	movs	r0, #0
    8e68:	b007      	add	sp, #28
    8e6a:	bdf0      	pop	{r4, r5, r6, r7, pc}
    8e6c:	898b      	ldrh	r3, [r1, #12]
    8e6e:	071b      	lsls	r3, r3, #28
    8e70:	d558      	bpl.n	8f24 <__sfvwrite_r+0xcc>
    8e72:	690b      	ldr	r3, [r1, #16]
    8e74:	2b00      	cmp	r3, #0
    8e76:	d055      	beq.n	8f24 <__sfvwrite_r+0xcc>
    8e78:	683b      	ldr	r3, [r7, #0]
    8e7a:	2202      	movs	r2, #2
    8e7c:	9302      	str	r3, [sp, #8]
    8e7e:	89a3      	ldrh	r3, [r4, #12]
    8e80:	0018      	movs	r0, r3
    8e82:	4010      	ands	r0, r2
    8e84:	4213      	tst	r3, r2
    8e86:	d175      	bne.n	8f74 <__sfvwrite_r+0x11c>
    8e88:	2201      	movs	r2, #1
    8e8a:	2101      	movs	r1, #1
    8e8c:	401a      	ands	r2, r3
    8e8e:	420b      	tst	r3, r1
    8e90:	d100      	bne.n	8e94 <__sfvwrite_r+0x3c>
    8e92:	e0ad      	b.n	8ff0 <__sfvwrite_r+0x198>
    8e94:	0006      	movs	r6, r0
    8e96:	9001      	str	r0, [sp, #4]
    8e98:	9004      	str	r0, [sp, #16]
    8e9a:	9b04      	ldr	r3, [sp, #16]
    8e9c:	2b00      	cmp	r3, #0
    8e9e:	d100      	bne.n	8ea2 <__sfvwrite_r+0x4a>
    8ea0:	e117      	b.n	90d2 <__sfvwrite_r+0x27a>
    8ea2:	b958      	cbnz	r0, 8ebc <__sfvwrite_r+0x64>
    8ea4:	001a      	movs	r2, r3
    8ea6:	210a      	movs	r1, #10
    8ea8:	0030      	movs	r0, r6
    8eaa:	f000 f94d 	bl	9148 <memchr>
    8eae:	9b04      	ldr	r3, [sp, #16]
    8eb0:	3301      	adds	r3, #1
    8eb2:	9301      	str	r3, [sp, #4]
    8eb4:	b110      	cbz	r0, 8ebc <__sfvwrite_r+0x64>
    8eb6:	3001      	adds	r0, #1
    8eb8:	1b83      	subs	r3, r0, r6
    8eba:	9301      	str	r3, [sp, #4]
    8ebc:	9b01      	ldr	r3, [sp, #4]
    8ebe:	9a04      	ldr	r2, [sp, #16]
    8ec0:	9305      	str	r3, [sp, #20]
    8ec2:	4293      	cmp	r3, r2
    8ec4:	d900      	bls.n	8ec8 <__sfvwrite_r+0x70>
    8ec6:	9205      	str	r2, [sp, #20]
    8ec8:	6820      	ldr	r0, [r4, #0]
    8eca:	6922      	ldr	r2, [r4, #16]
    8ecc:	6963      	ldr	r3, [r4, #20]
    8ece:	4290      	cmp	r0, r2
    8ed0:	d800      	bhi.n	8ed4 <__sfvwrite_r+0x7c>
    8ed2:	e107      	b.n	90e4 <__sfvwrite_r+0x28c>
    8ed4:	68a2      	ldr	r2, [r4, #8]
    8ed6:	189d      	adds	r5, r3, r2
    8ed8:	9a05      	ldr	r2, [sp, #20]
    8eda:	42aa      	cmp	r2, r5
    8edc:	dc00      	bgt.n	8ee0 <__sfvwrite_r+0x88>
    8ede:	e101      	b.n	90e4 <__sfvwrite_r+0x28c>
    8ee0:	0031      	movs	r1, r6
    8ee2:	002a      	movs	r2, r5
    8ee4:	f000 f944 	bl	9170 <memmove>
    8ee8:	6823      	ldr	r3, [r4, #0]
    8eea:	0021      	movs	r1, r4
    8eec:	195b      	adds	r3, r3, r5
    8eee:	9803      	ldr	r0, [sp, #12]
    8ef0:	6023      	str	r3, [r4, #0]
    8ef2:	f7fe fa79 	bl	73e8 <_fflush_r>
    8ef6:	2800      	cmp	r0, #0
    8ef8:	d175      	bne.n	8fe6 <__sfvwrite_r+0x18e>
    8efa:	9b01      	ldr	r3, [sp, #4]
    8efc:	2001      	movs	r0, #1
    8efe:	1b5b      	subs	r3, r3, r5
    8f00:	9301      	str	r3, [sp, #4]
    8f02:	b92b      	cbnz	r3, 8f10 <__sfvwrite_r+0xb8>
    8f04:	0021      	movs	r1, r4
    8f06:	9803      	ldr	r0, [sp, #12]
    8f08:	f7fe fa6e 	bl	73e8 <_fflush_r>
    8f0c:	2800      	cmp	r0, #0
    8f0e:	d16a      	bne.n	8fe6 <__sfvwrite_r+0x18e>
    8f10:	9b04      	ldr	r3, [sp, #16]
    8f12:	1976      	adds	r6, r6, r5
    8f14:	1b5b      	subs	r3, r3, r5
    8f16:	9304      	str	r3, [sp, #16]
    8f18:	68bb      	ldr	r3, [r7, #8]
    8f1a:	1b5d      	subs	r5, r3, r5
    8f1c:	60bd      	str	r5, [r7, #8]
    8f1e:	2d00      	cmp	r5, #0
    8f20:	d1bb      	bne.n	8e9a <__sfvwrite_r+0x42>
    8f22:	e7a0      	b.n	8e66 <__sfvwrite_r+0xe>
    8f24:	0021      	movs	r1, r4
    8f26:	9803      	ldr	r0, [sp, #12]
    8f28:	f7fe f926 	bl	7178 <__swsetup_r>
    8f2c:	2800      	cmp	r0, #0
    8f2e:	d0a3      	beq.n	8e78 <__sfvwrite_r+0x20>
    8f30:	2001      	movs	r0, #1
    8f32:	4240      	negs	r0, r0
    8f34:	e798      	b.n	8e68 <__sfvwrite_r+0x10>
    8f36:	9b02      	ldr	r3, [sp, #8]
    8f38:	681b      	ldr	r3, [r3, #0]
    8f3a:	9301      	str	r3, [sp, #4]
    8f3c:	9b02      	ldr	r3, [sp, #8]
    8f3e:	685d      	ldr	r5, [r3, #4]
    8f40:	3308      	adds	r3, #8
    8f42:	9302      	str	r3, [sp, #8]
    8f44:	2d00      	cmp	r5, #0
    8f46:	d0f6      	beq.n	8f36 <__sfvwrite_r+0xde>
    8f48:	4a73      	ldr	r2, [pc, #460]	; (9118 <__sfvwrite_r+0x2c0>)
    8f4a:	002b      	movs	r3, r5
    8f4c:	4295      	cmp	r5, r2
    8f4e:	d900      	bls.n	8f52 <__sfvwrite_r+0xfa>
    8f50:	0013      	movs	r3, r2
    8f52:	9a01      	ldr	r2, [sp, #4]
    8f54:	6a21      	ldr	r1, [r4, #32]
    8f56:	9803      	ldr	r0, [sp, #12]
    8f58:	6aa6      	ldr	r6, [r4, #40]	; 0x28
    8f5a:	47b0      	blx	r6
    8f5c:	2800      	cmp	r0, #0
    8f5e:	dd42      	ble.n	8fe6 <__sfvwrite_r+0x18e>
    8f60:	9b01      	ldr	r3, [sp, #4]
    8f62:	1a2d      	subs	r5, r5, r0
    8f64:	181b      	adds	r3, r3, r0
    8f66:	9301      	str	r3, [sp, #4]
    8f68:	68bb      	ldr	r3, [r7, #8]
    8f6a:	1a18      	subs	r0, r3, r0
    8f6c:	60b8      	str	r0, [r7, #8]
    8f6e:	2800      	cmp	r0, #0
    8f70:	d1e8      	bne.n	8f44 <__sfvwrite_r+0xec>
    8f72:	e778      	b.n	8e66 <__sfvwrite_r+0xe>
    8f74:	2300      	movs	r3, #0
    8f76:	001d      	movs	r5, r3
    8f78:	9301      	str	r3, [sp, #4]
    8f7a:	e7e3      	b.n	8f44 <__sfvwrite_r+0xec>
    8f7c:	9b02      	ldr	r3, [sp, #8]
    8f7e:	681b      	ldr	r3, [r3, #0]
    8f80:	9304      	str	r3, [sp, #16]
    8f82:	9b02      	ldr	r3, [sp, #8]
    8f84:	685b      	ldr	r3, [r3, #4]
    8f86:	9301      	str	r3, [sp, #4]
    8f88:	9b02      	ldr	r3, [sp, #8]
    8f8a:	3308      	adds	r3, #8
    8f8c:	9302      	str	r3, [sp, #8]
    8f8e:	9b01      	ldr	r3, [sp, #4]
    8f90:	2b00      	cmp	r3, #0
    8f92:	d0f3      	beq.n	8f7c <__sfvwrite_r+0x124>
    8f94:	f240 2300 	movw	r3, #512	; 0x200
    8f98:	89a2      	ldrh	r2, [r4, #12]
    8f9a:	6820      	ldr	r0, [r4, #0]
    8f9c:	68a5      	ldr	r5, [r4, #8]
    8f9e:	421a      	tst	r2, r3
    8fa0:	d066      	beq.n	9070 <__sfvwrite_r+0x218>
    8fa2:	9b01      	ldr	r3, [sp, #4]
    8fa4:	429d      	cmp	r5, r3
    8fa6:	d838      	bhi.n	901a <__sfvwrite_r+0x1c2>
    8fa8:	f240 4380 	movw	r3, #1152	; 0x480
    8fac:	421a      	tst	r2, r3
    8fae:	d034      	beq.n	901a <__sfvwrite_r+0x1c2>
    8fb0:	6921      	ldr	r1, [r4, #16]
    8fb2:	1a43      	subs	r3, r0, r1
    8fb4:	2003      	movs	r0, #3
    8fb6:	9305      	str	r3, [sp, #20]
    8fb8:	6963      	ldr	r3, [r4, #20]
    8fba:	4358      	muls	r0, r3
    8fbc:	9b05      	ldr	r3, [sp, #20]
    8fbe:	0fc6      	lsrs	r6, r0, #31
    8fc0:	1836      	adds	r6, r6, r0
    8fc2:	9801      	ldr	r0, [sp, #4]
    8fc4:	3301      	adds	r3, #1
    8fc6:	181b      	adds	r3, r3, r0
    8fc8:	1076      	asrs	r6, r6, #1
    8fca:	42b3      	cmp	r3, r6
    8fcc:	d900      	bls.n	8fd0 <__sfvwrite_r+0x178>
    8fce:	001e      	movs	r6, r3
    8fd0:	0553      	lsls	r3, r2, #21
    8fd2:	d53d      	bpl.n	9050 <__sfvwrite_r+0x1f8>
    8fd4:	0031      	movs	r1, r6
    8fd6:	9803      	ldr	r0, [sp, #12]
    8fd8:	f7fe fca0 	bl	791c <_malloc_r>
    8fdc:	0005      	movs	r5, r0
    8fde:	b950      	cbnz	r0, 8ff6 <__sfvwrite_r+0x19e>
    8fe0:	230c      	movs	r3, #12
    8fe2:	9a03      	ldr	r2, [sp, #12]
    8fe4:	6013      	str	r3, [r2, #0]
    8fe6:	2340      	movs	r3, #64	; 0x40
    8fe8:	89a2      	ldrh	r2, [r4, #12]
    8fea:	4313      	orrs	r3, r2
    8fec:	81a3      	strh	r3, [r4, #12]
    8fee:	e79f      	b.n	8f30 <__sfvwrite_r+0xd8>
    8ff0:	9204      	str	r2, [sp, #16]
    8ff2:	9201      	str	r2, [sp, #4]
    8ff4:	e7cb      	b.n	8f8e <__sfvwrite_r+0x136>
    8ff6:	9a05      	ldr	r2, [sp, #20]
    8ff8:	6921      	ldr	r1, [r4, #16]
    8ffa:	f000 f8b0 	bl	915e <memcpy>
    8ffe:	89a3      	ldrh	r3, [r4, #12]
    9000:	4a46      	ldr	r2, [pc, #280]	; (911c <__sfvwrite_r+0x2c4>)
    9002:	401a      	ands	r2, r3
    9004:	2380      	movs	r3, #128	; 0x80
    9006:	4313      	orrs	r3, r2
    9008:	81a3      	strh	r3, [r4, #12]
    900a:	9b05      	ldr	r3, [sp, #20]
    900c:	6125      	str	r5, [r4, #16]
    900e:	18ed      	adds	r5, r5, r3
    9010:	6025      	str	r5, [r4, #0]
    9012:	6166      	str	r6, [r4, #20]
    9014:	9d01      	ldr	r5, [sp, #4]
    9016:	1af6      	subs	r6, r6, r3
    9018:	60a6      	str	r6, [r4, #8]
    901a:	9e01      	ldr	r6, [sp, #4]
    901c:	42b5      	cmp	r5, r6
    901e:	d900      	bls.n	9022 <__sfvwrite_r+0x1ca>
    9020:	0035      	movs	r5, r6
    9022:	002a      	movs	r2, r5
    9024:	9904      	ldr	r1, [sp, #16]
    9026:	6820      	ldr	r0, [r4, #0]
    9028:	f000 f8a2 	bl	9170 <memmove>
    902c:	68a3      	ldr	r3, [r4, #8]
    902e:	1b5b      	subs	r3, r3, r5
    9030:	60a3      	str	r3, [r4, #8]
    9032:	6823      	ldr	r3, [r4, #0]
    9034:	195d      	adds	r5, r3, r5
    9036:	6025      	str	r5, [r4, #0]
    9038:	9b04      	ldr	r3, [sp, #16]
    903a:	199b      	adds	r3, r3, r6
    903c:	9304      	str	r3, [sp, #16]
    903e:	9b01      	ldr	r3, [sp, #4]
    9040:	1b9b      	subs	r3, r3, r6
    9042:	9301      	str	r3, [sp, #4]
    9044:	68bb      	ldr	r3, [r7, #8]
    9046:	1b9e      	subs	r6, r3, r6
    9048:	60be      	str	r6, [r7, #8]
    904a:	2e00      	cmp	r6, #0
    904c:	d19f      	bne.n	8f8e <__sfvwrite_r+0x136>
    904e:	e70a      	b.n	8e66 <__sfvwrite_r+0xe>
    9050:	0032      	movs	r2, r6
    9052:	9803      	ldr	r0, [sp, #12]
    9054:	f000 f8b0 	bl	91b8 <_realloc_r>
    9058:	0005      	movs	r5, r0
    905a:	2800      	cmp	r0, #0
    905c:	d1d5      	bne.n	900a <__sfvwrite_r+0x1b2>
    905e:	6921      	ldr	r1, [r4, #16]
    9060:	9803      	ldr	r0, [sp, #12]
    9062:	f7fe fc15 	bl	7890 <_free_r>
    9066:	2280      	movs	r2, #128	; 0x80
    9068:	89a3      	ldrh	r3, [r4, #12]
    906a:	4393      	bics	r3, r2
    906c:	81a3      	strh	r3, [r4, #12]
    906e:	e7b7      	b.n	8fe0 <__sfvwrite_r+0x188>
    9070:	6923      	ldr	r3, [r4, #16]
    9072:	4283      	cmp	r3, r0
    9074:	d303      	bcc.n	907e <__sfvwrite_r+0x226>
    9076:	6963      	ldr	r3, [r4, #20]
    9078:	9a01      	ldr	r2, [sp, #4]
    907a:	4293      	cmp	r3, r2
    907c:	d917      	bls.n	90ae <__sfvwrite_r+0x256>
    907e:	9b01      	ldr	r3, [sp, #4]
    9080:	429d      	cmp	r5, r3
    9082:	d900      	bls.n	9086 <__sfvwrite_r+0x22e>
    9084:	001d      	movs	r5, r3
    9086:	002a      	movs	r2, r5
    9088:	9904      	ldr	r1, [sp, #16]
    908a:	f000 f871 	bl	9170 <memmove>
    908e:	68a3      	ldr	r3, [r4, #8]
    9090:	6822      	ldr	r2, [r4, #0]
    9092:	1b5b      	subs	r3, r3, r5
    9094:	1952      	adds	r2, r2, r5
    9096:	002e      	movs	r6, r5
    9098:	60a3      	str	r3, [r4, #8]
    909a:	6022      	str	r2, [r4, #0]
    909c:	2b00      	cmp	r3, #0
    909e:	d1cb      	bne.n	9038 <__sfvwrite_r+0x1e0>
    90a0:	0021      	movs	r1, r4
    90a2:	9803      	ldr	r0, [sp, #12]
    90a4:	f7fe f9a0 	bl	73e8 <_fflush_r>
    90a8:	2800      	cmp	r0, #0
    90aa:	d0c5      	beq.n	9038 <__sfvwrite_r+0x1e0>
    90ac:	e79b      	b.n	8fe6 <__sfvwrite_r+0x18e>
    90ae:	4a1c      	ldr	r2, [pc, #112]	; (9120 <__sfvwrite_r+0x2c8>)
    90b0:	9901      	ldr	r1, [sp, #4]
    90b2:	4291      	cmp	r1, r2
    90b4:	d80b      	bhi.n	90ce <__sfvwrite_r+0x276>
    90b6:	000a      	movs	r2, r1
    90b8:	fb92 f2f3 	sdiv	r2, r2, r3
    90bc:	6a21      	ldr	r1, [r4, #32]
    90be:	4353      	muls	r3, r2
    90c0:	9803      	ldr	r0, [sp, #12]
    90c2:	9a04      	ldr	r2, [sp, #16]
    90c4:	6aa5      	ldr	r5, [r4, #40]	; 0x28
    90c6:	47a8      	blx	r5
    90c8:	1e06      	subs	r6, r0, #0
    90ca:	dcb5      	bgt.n	9038 <__sfvwrite_r+0x1e0>
    90cc:	e78b      	b.n	8fe6 <__sfvwrite_r+0x18e>
    90ce:	4a15      	ldr	r2, [pc, #84]	; (9124 <__sfvwrite_r+0x2cc>)
    90d0:	e7f2      	b.n	90b8 <__sfvwrite_r+0x260>
    90d2:	9b02      	ldr	r3, [sp, #8]
    90d4:	2000      	movs	r0, #0
    90d6:	681e      	ldr	r6, [r3, #0]
    90d8:	685b      	ldr	r3, [r3, #4]
    90da:	9304      	str	r3, [sp, #16]
    90dc:	9b02      	ldr	r3, [sp, #8]
    90de:	3308      	adds	r3, #8
    90e0:	9302      	str	r3, [sp, #8]
    90e2:	e6da      	b.n	8e9a <__sfvwrite_r+0x42>
    90e4:	9a05      	ldr	r2, [sp, #20]
    90e6:	4293      	cmp	r3, r2
    90e8:	dc08      	bgt.n	90fc <__sfvwrite_r+0x2a4>
    90ea:	6aa5      	ldr	r5, [r4, #40]	; 0x28
    90ec:	0032      	movs	r2, r6
    90ee:	6a21      	ldr	r1, [r4, #32]
    90f0:	9803      	ldr	r0, [sp, #12]
    90f2:	47a8      	blx	r5
    90f4:	1e05      	subs	r5, r0, #0
    90f6:	dd00      	ble.n	90fa <__sfvwrite_r+0x2a2>
    90f8:	e6ff      	b.n	8efa <__sfvwrite_r+0xa2>
    90fa:	e774      	b.n	8fe6 <__sfvwrite_r+0x18e>
    90fc:	9a05      	ldr	r2, [sp, #20]
    90fe:	0031      	movs	r1, r6
    9100:	f000 f836 	bl	9170 <memmove>
    9104:	9a05      	ldr	r2, [sp, #20]
    9106:	68a3      	ldr	r3, [r4, #8]
    9108:	0015      	movs	r5, r2
    910a:	1a9b      	subs	r3, r3, r2
    910c:	60a3      	str	r3, [r4, #8]
    910e:	6823      	ldr	r3, [r4, #0]
    9110:	189b      	adds	r3, r3, r2
    9112:	6023      	str	r3, [r4, #0]
    9114:	e6f1      	b.n	8efa <__sfvwrite_r+0xa2>
    9116:	bf00      	nop
    9118:	7ffffc00 	.word	0x7ffffc00
    911c:	fffffb7f 	.word	0xfffffb7f
    9120:	7ffffffe 	.word	0x7ffffffe
    9124:	7fffffff 	.word	0x7fffffff

00009128 <_isatty_r>:
    9128:	2300      	movs	r3, #0
    912a:	b570      	push	{r4, r5, r6, lr}
    912c:	4d05      	ldr	r5, [pc, #20]	; (9144 <_isatty_r+0x1c>)
    912e:	0004      	movs	r4, r0
    9130:	0008      	movs	r0, r1
    9132:	602b      	str	r3, [r5, #0]
    9134:	f000 f948 	bl	93c8 <_isatty>
    9138:	1c43      	adds	r3, r0, #1
    913a:	d102      	bne.n	9142 <_isatty_r+0x1a>
    913c:	682b      	ldr	r3, [r5, #0]
    913e:	b103      	cbz	r3, 9142 <_isatty_r+0x1a>
    9140:	6023      	str	r3, [r4, #0]
    9142:	bd70      	pop	{r4, r5, r6, pc}
    9144:	2000209c 	.word	0x2000209c

00009148 <memchr>:
    9148:	b2c9      	uxtb	r1, r1
    914a:	1882      	adds	r2, r0, r2
    914c:	4290      	cmp	r0, r2
    914e:	d101      	bne.n	9154 <memchr+0xc>
    9150:	2000      	movs	r0, #0
    9152:	4770      	bx	lr
    9154:	7803      	ldrb	r3, [r0, #0]
    9156:	428b      	cmp	r3, r1
    9158:	d0fb      	beq.n	9152 <memchr+0xa>
    915a:	3001      	adds	r0, #1
    915c:	e7f6      	b.n	914c <memchr+0x4>

0000915e <memcpy>:
    915e:	2300      	movs	r3, #0
    9160:	b510      	push	{r4, lr}
    9162:	429a      	cmp	r2, r3
    9164:	d100      	bne.n	9168 <memcpy+0xa>
    9166:	bd10      	pop	{r4, pc}
    9168:	5ccc      	ldrb	r4, [r1, r3]
    916a:	54c4      	strb	r4, [r0, r3]
    916c:	3301      	adds	r3, #1
    916e:	e7f8      	b.n	9162 <memcpy+0x4>

00009170 <memmove>:
    9170:	b510      	push	{r4, lr}
    9172:	4288      	cmp	r0, r1
    9174:	d902      	bls.n	917c <memmove+0xc>
    9176:	188b      	adds	r3, r1, r2
    9178:	4298      	cmp	r0, r3
    917a:	d303      	bcc.n	9184 <memmove+0x14>
    917c:	2300      	movs	r3, #0
    917e:	e007      	b.n	9190 <memmove+0x20>
    9180:	5c8b      	ldrb	r3, [r1, r2]
    9182:	5483      	strb	r3, [r0, r2]
    9184:	3a01      	subs	r2, #1
    9186:	d2fb      	bcs.n	9180 <memmove+0x10>
    9188:	bd10      	pop	{r4, pc}
    918a:	5ccc      	ldrb	r4, [r1, r3]
    918c:	54c4      	strb	r4, [r0, r3]
    918e:	3301      	adds	r3, #1
    9190:	429a      	cmp	r2, r3
    9192:	d1fa      	bne.n	918a <memmove+0x1a>
    9194:	e7f8      	b.n	9188 <memmove+0x18>
	...

00009198 <__malloc_lock>:
    9198:	b510      	push	{r4, lr}
    919a:	4802      	ldr	r0, [pc, #8]	; (91a4 <__malloc_lock+0xc>)
    919c:	f7fe faec 	bl	7778 <__retarget_lock_acquire_recursive>
    91a0:	bd10      	pop	{r4, pc}
    91a2:	bf00      	nop
    91a4:	2000208c 	.word	0x2000208c

000091a8 <__malloc_unlock>:
    91a8:	b510      	push	{r4, lr}
    91aa:	4802      	ldr	r0, [pc, #8]	; (91b4 <__malloc_unlock+0xc>)
    91ac:	f7fe faea 	bl	7784 <__retarget_lock_release_recursive>
    91b0:	bd10      	pop	{r4, pc}
    91b2:	bf00      	nop
    91b4:	2000208c 	.word	0x2000208c

000091b8 <_realloc_r>:
    91b8:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    91ba:	0007      	movs	r7, r0
    91bc:	000e      	movs	r6, r1
    91be:	0014      	movs	r4, r2
    91c0:	b929      	cbnz	r1, 91ce <_realloc_r+0x16>
    91c2:	0011      	movs	r1, r2
    91c4:	f7fe fbaa 	bl	791c <_malloc_r>
    91c8:	0005      	movs	r5, r0
    91ca:	0028      	movs	r0, r5
    91cc:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    91ce:	b91a      	cbnz	r2, 91d8 <_realloc_r+0x20>
    91d0:	f7fe fb5e 	bl	7890 <_free_r>
    91d4:	0025      	movs	r5, r4
    91d6:	e7f8      	b.n	91ca <_realloc_r+0x12>
    91d8:	f000 f8cc 	bl	9374 <_malloc_usable_size_r>
    91dc:	9001      	str	r0, [sp, #4]
    91de:	4284      	cmp	r4, r0
    91e0:	d803      	bhi.n	91ea <_realloc_r+0x32>
    91e2:	0035      	movs	r5, r6
    91e4:	0843      	lsrs	r3, r0, #1
    91e6:	42a3      	cmp	r3, r4
    91e8:	d3ef      	bcc.n	91ca <_realloc_r+0x12>
    91ea:	0021      	movs	r1, r4
    91ec:	0038      	movs	r0, r7
    91ee:	f7fe fb95 	bl	791c <_malloc_r>
    91f2:	0005      	movs	r5, r0
    91f4:	2800      	cmp	r0, #0
    91f6:	d0e8      	beq.n	91ca <_realloc_r+0x12>
    91f8:	9b01      	ldr	r3, [sp, #4]
    91fa:	0022      	movs	r2, r4
    91fc:	429c      	cmp	r4, r3
    91fe:	d900      	bls.n	9202 <_realloc_r+0x4a>
    9200:	001a      	movs	r2, r3
    9202:	0031      	movs	r1, r6
    9204:	0028      	movs	r0, r5
    9206:	f7ff ffaa 	bl	915e <memcpy>
    920a:	0031      	movs	r1, r6
    920c:	0038      	movs	r0, r7
    920e:	f7fe fb3f 	bl	7890 <_free_r>
    9212:	e7da      	b.n	91ca <_realloc_r+0x12>

00009214 <_init_signal_r>:
    9214:	b570      	push	{r4, r5, r6, lr}
    9216:	6c44      	ldr	r4, [r0, #68]	; 0x44
    9218:	0005      	movs	r5, r0
    921a:	b94c      	cbnz	r4, 9230 <_init_signal_r+0x1c>
    921c:	2180      	movs	r1, #128	; 0x80
    921e:	f7fe fb7d 	bl	791c <_malloc_r>
    9222:	6468      	str	r0, [r5, #68]	; 0x44
    9224:	b130      	cbz	r0, 9234 <_init_signal_r+0x20>
    9226:	0003      	movs	r3, r0
    9228:	3380      	adds	r3, #128	; 0x80
    922a:	c010      	stmia	r0!, {r4}
    922c:	4298      	cmp	r0, r3
    922e:	d1fc      	bne.n	922a <_init_signal_r+0x16>
    9230:	2000      	movs	r0, #0
    9232:	bd70      	pop	{r4, r5, r6, pc}
    9234:	2001      	movs	r0, #1
    9236:	4240      	negs	r0, r0
    9238:	e7fb      	b.n	9232 <_init_signal_r+0x1e>

0000923a <_signal_r>:
    923a:	b570      	push	{r4, r5, r6, lr}
    923c:	0004      	movs	r4, r0
    923e:	000d      	movs	r5, r1
    9240:	0016      	movs	r6, r2
    9242:	291f      	cmp	r1, #31
    9244:	d904      	bls.n	9250 <_signal_r+0x16>
    9246:	2316      	movs	r3, #22
    9248:	6003      	str	r3, [r0, #0]
    924a:	2001      	movs	r0, #1
    924c:	4240      	negs	r0, r0
    924e:	e006      	b.n	925e <_signal_r+0x24>
    9250:	6c43      	ldr	r3, [r0, #68]	; 0x44
    9252:	b12b      	cbz	r3, 9260 <_signal_r+0x26>
    9254:	00a9      	lsls	r1, r5, #2
    9256:	6c65      	ldr	r5, [r4, #68]	; 0x44
    9258:	186d      	adds	r5, r5, r1
    925a:	6828      	ldr	r0, [r5, #0]
    925c:	602e      	str	r6, [r5, #0]
    925e:	bd70      	pop	{r4, r5, r6, pc}
    9260:	f7ff ffd8 	bl	9214 <_init_signal_r>
    9264:	2800      	cmp	r0, #0
    9266:	d0f5      	beq.n	9254 <_signal_r+0x1a>
    9268:	e7ef      	b.n	924a <_signal_r+0x10>

0000926a <_raise_r>:
    926a:	b570      	push	{r4, r5, r6, lr}
    926c:	0004      	movs	r4, r0
    926e:	000d      	movs	r5, r1
    9270:	291f      	cmp	r1, #31
    9272:	d904      	bls.n	927e <_raise_r+0x14>
    9274:	2316      	movs	r3, #22
    9276:	6003      	str	r3, [r0, #0]
    9278:	2001      	movs	r0, #1
    927a:	4240      	negs	r0, r0
    927c:	bd70      	pop	{r4, r5, r6, pc}
    927e:	6c43      	ldr	r3, [r0, #68]	; 0x44
    9280:	b11b      	cbz	r3, 928a <_raise_r+0x20>
    9282:	008a      	lsls	r2, r1, #2
    9284:	189b      	adds	r3, r3, r2
    9286:	681a      	ldr	r2, [r3, #0]
    9288:	b942      	cbnz	r2, 929c <_raise_r+0x32>
    928a:	0020      	movs	r0, r4
    928c:	f000 f86e 	bl	936c <_getpid_r>
    9290:	002a      	movs	r2, r5
    9292:	0001      	movs	r1, r0
    9294:	0020      	movs	r0, r4
    9296:	f000 f857 	bl	9348 <_kill_r>
    929a:	e7ef      	b.n	927c <_raise_r+0x12>
    929c:	2000      	movs	r0, #0
    929e:	2a01      	cmp	r2, #1
    92a0:	d0ec      	beq.n	927c <_raise_r+0x12>
    92a2:	1c51      	adds	r1, r2, #1
    92a4:	d103      	bne.n	92ae <_raise_r+0x44>
    92a6:	2316      	movs	r3, #22
    92a8:	3001      	adds	r0, #1
    92aa:	6023      	str	r3, [r4, #0]
    92ac:	e7e6      	b.n	927c <_raise_r+0x12>
    92ae:	2400      	movs	r4, #0
    92b0:	0028      	movs	r0, r5
    92b2:	601c      	str	r4, [r3, #0]
    92b4:	4790      	blx	r2
    92b6:	0020      	movs	r0, r4
    92b8:	e7e0      	b.n	927c <_raise_r+0x12>

000092ba <__sigtramp_r>:
    92ba:	b570      	push	{r4, r5, r6, lr}
    92bc:	0004      	movs	r4, r0
    92be:	000d      	movs	r5, r1
    92c0:	291f      	cmp	r1, #31
    92c2:	d902      	bls.n	92ca <__sigtramp_r+0x10>
    92c4:	2001      	movs	r0, #1
    92c6:	4240      	negs	r0, r0
    92c8:	bd70      	pop	{r4, r5, r6, pc}
    92ca:	6c43      	ldr	r3, [r0, #68]	; 0x44
    92cc:	b193      	cbz	r3, 92f4 <__sigtramp_r+0x3a>
    92ce:	6c62      	ldr	r2, [r4, #68]	; 0x44
    92d0:	00ab      	lsls	r3, r5, #2
    92d2:	18d2      	adds	r2, r2, r3
    92d4:	6813      	ldr	r3, [r2, #0]
    92d6:	2001      	movs	r0, #1
    92d8:	2b00      	cmp	r3, #0
    92da:	d0f5      	beq.n	92c8 <__sigtramp_r+0xe>
    92dc:	2002      	movs	r0, #2
    92de:	1c59      	adds	r1, r3, #1
    92e0:	d0f2      	beq.n	92c8 <__sigtramp_r+0xe>
    92e2:	3001      	adds	r0, #1
    92e4:	2b01      	cmp	r3, #1
    92e6:	d0ef      	beq.n	92c8 <__sigtramp_r+0xe>
    92e8:	2400      	movs	r4, #0
    92ea:	0028      	movs	r0, r5
    92ec:	6014      	str	r4, [r2, #0]
    92ee:	4798      	blx	r3
    92f0:	0020      	movs	r0, r4
    92f2:	e7e9      	b.n	92c8 <__sigtramp_r+0xe>
    92f4:	f7ff ff8e 	bl	9214 <_init_signal_r>
    92f8:	2800      	cmp	r0, #0
    92fa:	d0e8      	beq.n	92ce <__sigtramp_r+0x14>
    92fc:	e7e2      	b.n	92c4 <__sigtramp_r+0xa>

000092fe <raise>:
    92fe:	b510      	push	{r4, lr}
    9300:	4b02      	ldr	r3, [pc, #8]	; (930c <raise+0xe>)
    9302:	0001      	movs	r1, r0
    9304:	6818      	ldr	r0, [r3, #0]
    9306:	f7ff ffb0 	bl	926a <_raise_r>
    930a:	bd10      	pop	{r4, pc}
    930c:	20002000 	.word	0x20002000

00009310 <signal>:
    9310:	b510      	push	{r4, lr}
    9312:	4b03      	ldr	r3, [pc, #12]	; (9320 <signal+0x10>)
    9314:	000a      	movs	r2, r1
    9316:	0001      	movs	r1, r0
    9318:	6818      	ldr	r0, [r3, #0]
    931a:	f7ff ff8e 	bl	923a <_signal_r>
    931e:	bd10      	pop	{r4, pc}
    9320:	20002000 	.word	0x20002000

00009324 <_init_signal>:
    9324:	b510      	push	{r4, lr}
    9326:	4b02      	ldr	r3, [pc, #8]	; (9330 <_init_signal+0xc>)
    9328:	6818      	ldr	r0, [r3, #0]
    932a:	f7ff ff73 	bl	9214 <_init_signal_r>
    932e:	bd10      	pop	{r4, pc}
    9330:	20002000 	.word	0x20002000

00009334 <__sigtramp>:
    9334:	b510      	push	{r4, lr}
    9336:	4b03      	ldr	r3, [pc, #12]	; (9344 <__sigtramp+0x10>)
    9338:	0001      	movs	r1, r0
    933a:	6818      	ldr	r0, [r3, #0]
    933c:	f7ff ffbd 	bl	92ba <__sigtramp_r>
    9340:	bd10      	pop	{r4, pc}
    9342:	bf00      	nop
    9344:	20002000 	.word	0x20002000

00009348 <_kill_r>:
    9348:	2300      	movs	r3, #0
    934a:	b570      	push	{r4, r5, r6, lr}
    934c:	4d06      	ldr	r5, [pc, #24]	; (9368 <_kill_r+0x20>)
    934e:	0004      	movs	r4, r0
    9350:	0008      	movs	r0, r1
    9352:	0011      	movs	r1, r2
    9354:	602b      	str	r3, [r5, #0]
    9356:	f000 f83f 	bl	93d8 <_kill>
    935a:	1c43      	adds	r3, r0, #1
    935c:	d102      	bne.n	9364 <_kill_r+0x1c>
    935e:	682b      	ldr	r3, [r5, #0]
    9360:	b103      	cbz	r3, 9364 <_kill_r+0x1c>
    9362:	6023      	str	r3, [r4, #0]
    9364:	bd70      	pop	{r4, r5, r6, pc}
    9366:	bf00      	nop
    9368:	2000209c 	.word	0x2000209c

0000936c <_getpid_r>:
    936c:	b510      	push	{r4, lr}
    936e:	f000 f823 	bl	93b8 <_getpid>
    9372:	bd10      	pop	{r4, pc}

00009374 <_malloc_usable_size_r>:
    9374:	1f0b      	subs	r3, r1, #4
    9376:	681b      	ldr	r3, [r3, #0]
    9378:	1f18      	subs	r0, r3, #4
    937a:	2b00      	cmp	r3, #0
    937c:	da01      	bge.n	9382 <_malloc_usable_size_r+0xe>
    937e:	580b      	ldr	r3, [r1, r0]
    9380:	18c0      	adds	r0, r0, r3
    9382:	4770      	bx	lr

00009384 <__gnu_thumb1_case_shi>:
    9384:	b403      	push	{r0, r1}
    9386:	4671      	mov	r1, lr
    9388:	0849      	lsrs	r1, r1, #1
    938a:	0040      	lsls	r0, r0, #1
    938c:	0049      	lsls	r1, r1, #1
    938e:	5e09      	ldrsh	r1, [r1, r0]
    9390:	0049      	lsls	r1, r1, #1
    9392:	448e      	add	lr, r1
    9394:	bc03      	pop	{r0, r1}
    9396:	4770      	bx	lr

00009398 <_close>:
    9398:	2258      	movs	r2, #88	; 0x58
    939a:	2001      	movs	r0, #1
    939c:	4b01      	ldr	r3, [pc, #4]	; (93a4 <_close+0xc>)
    939e:	4240      	negs	r0, r0
    93a0:	601a      	str	r2, [r3, #0]
    93a2:	4770      	bx	lr
    93a4:	2000209c 	.word	0x2000209c

000093a8 <_fstat>:
    93a8:	2258      	movs	r2, #88	; 0x58
    93aa:	2001      	movs	r0, #1
    93ac:	4b01      	ldr	r3, [pc, #4]	; (93b4 <_fstat+0xc>)
    93ae:	4240      	negs	r0, r0
    93b0:	601a      	str	r2, [r3, #0]
    93b2:	4770      	bx	lr
    93b4:	2000209c 	.word	0x2000209c

000093b8 <_getpid>:
    93b8:	2258      	movs	r2, #88	; 0x58
    93ba:	2001      	movs	r0, #1
    93bc:	4b01      	ldr	r3, [pc, #4]	; (93c4 <_getpid+0xc>)
    93be:	4240      	negs	r0, r0
    93c0:	601a      	str	r2, [r3, #0]
    93c2:	4770      	bx	lr
    93c4:	2000209c 	.word	0x2000209c

000093c8 <_isatty>:
    93c8:	2258      	movs	r2, #88	; 0x58
    93ca:	4b02      	ldr	r3, [pc, #8]	; (93d4 <_isatty+0xc>)
    93cc:	2000      	movs	r0, #0
    93ce:	601a      	str	r2, [r3, #0]
    93d0:	4770      	bx	lr
    93d2:	bf00      	nop
    93d4:	2000209c 	.word	0x2000209c

000093d8 <_kill>:
    93d8:	2258      	movs	r2, #88	; 0x58
    93da:	2001      	movs	r0, #1
    93dc:	4b01      	ldr	r3, [pc, #4]	; (93e4 <_kill+0xc>)
    93de:	4240      	negs	r0, r0
    93e0:	601a      	str	r2, [r3, #0]
    93e2:	4770      	bx	lr
    93e4:	2000209c 	.word	0x2000209c

000093e8 <_lseek>:
    93e8:	2258      	movs	r2, #88	; 0x58
    93ea:	2001      	movs	r0, #1
    93ec:	4b01      	ldr	r3, [pc, #4]	; (93f4 <_lseek+0xc>)
    93ee:	4240      	negs	r0, r0
    93f0:	601a      	str	r2, [r3, #0]
    93f2:	4770      	bx	lr
    93f4:	2000209c 	.word	0x2000209c

000093f8 <_sbrk>:
    93f8:	4a04      	ldr	r2, [pc, #16]	; (940c <_sbrk+0x14>)
    93fa:	0003      	movs	r3, r0
    93fc:	6811      	ldr	r1, [r2, #0]
    93fe:	b909      	cbnz	r1, 9404 <_sbrk+0xc>
    9400:	4903      	ldr	r1, [pc, #12]	; (9410 <_sbrk+0x18>)
    9402:	6011      	str	r1, [r2, #0]
    9404:	6810      	ldr	r0, [r2, #0]
    9406:	18c3      	adds	r3, r0, r3
    9408:	6013      	str	r3, [r2, #0]
    940a:	4770      	bx	lr
    940c:	200020a0 	.word	0x200020a0
    9410:	200020c0 	.word	0x200020c0

00009414 <_exit>:
    9414:	e7fe      	b.n	9414 <_exit>
    9416:	0000      	movs	r0, r0
    9418:	253a6562 	.word	0x253a6562
    941c:	00000a73 	.word	0x00000a73
    9420:	253a6661 	.word	0x253a6661
    9424:	00000a73 	.word	0x00000a73
    9428:	00003130 	.word	0x00003130
    942c:	3a296661 	.word	0x3a296661
    9430:	000a7325 	.word	0x000a7325
    9434:	32287473 	.word	0x32287473
    9438:	00000029 	.word	0x00000029
    943c:	69727473 	.word	0x69727473
    9440:	3228676e 	.word	0x3228676e
    9444:	00000029 	.word	0x00000029
    9448:	00004261 	.word	0x00004261
    944c:	00006241 	.word	0x00006241
    9450:	6e727473 	.word	0x6e727473
    9454:	28706d63 	.word	0x28706d63
    9458:	62202c61 	.word	0x62202c61
    945c:	25203a29 	.word	0x25203a29
    9460:	00000a64 	.word	0x00000a64
    9464:	00003273 	.word	0x00003273
    9468:	003a2d20 	.word	0x003a2d20
    946c:	00207325 	.word	0x00207325
    9470:	792d6261 	.word	0x792d6261
    9474:	0000003b 	.word	0x0000003b
    9478:	00313039 	.word	0x00313039
    947c:	3b2d6261 	.word	0x3b2d6261
    9480:	0000007a 	.word	0x0000007a
    9484:	25203a66 	.word	0x25203a66
    9488:	00000a63 	.word	0x00000a63
    948c:	00000065 	.word	0x00000065
    9490:	00006261 	.word	0x00006261
    9494:	00003433 	.word	0x00003433
    9498:	3a6d756e 	.word	0x3a6d756e
    949c:	0a642520 	.word	0x0a642520
    94a0:	00000000 	.word	0x00000000
    94a4:	3a6d756e 	.word	0x3a6d756e
    94a8:	646c2520 	.word	0x646c2520
    94ac:	0000000a 	.word	0x0000000a
    94b0:	3a727473 	.word	0x3a727473
    94b4:	73257c20 	.word	0x73257c20
    94b8:	0000007c 	.word	0x0000007c
    94bc:	65743032 	.word	0x65743032
    94c0:	00007473 	.word	0x00007473
	...
    94dc:	5f6c7472 	.word	0x5f6c7472
    94e0:	69727473 	.word	0x69727473
    94e4:	745f676e 	.word	0x745f676e
    94e8:	00747365 	.word	0x00747365
    94ec:	000a6425 	.word	0x000a6425
    94f0:	58323025 	.word	0x58323025
    94f4:	58323025 	.word	0x58323025
    94f8:	58323025 	.word	0x58323025
    94fc:	58323025 	.word	0x58323025
    9500:	58323025 	.word	0x58323025
    9504:	58323025 	.word	0x58323025
    9508:	00000000 	.word	0x00000000
    950c:	75333025 	.word	0x75333025
    9510:	00000000 	.word	0x00000000
    9514:	256c7472 	.word	0x256c7472
    9518:	00783230 	.word	0x00783230
    951c:	3f3f6325 	.word	0x3f3f6325
    9520:	3632253f 	.word	0x3632253f
    9524:	37322563 	.word	0x37322563
    9528:	00000073 	.word	0x00000073
    952c:	3f3f3f20 	.word	0x3f3f3f20
    9530:	3332313f 	.word	0x3332313f
    9534:	37363534 	.word	0x37363534
    9538:	41303938 	.word	0x41303938
    953c:	45444342 	.word	0x45444342
    9540:	69686746 	.word	0x69686746
    9544:	6d6c6b6a 	.word	0x6d6c6b6a
    9548:	73726f6e 	.word	0x73726f6e
    954c:	00000074 	.word	0x00000074
    9550:	6425      	.short	0x6425
    9552:	00          	.byte	0x00
    9553:	52          	.byte	0x52
    9554:	544e4545 	.word	0x544e4545
    9558:	6c616d20 	.word	0x6c616d20
    955c:	20636f6c 	.word	0x20636f6c
    9560:	63637573 	.word	0x63637573
    9564:	65646565 	.word	0x65646565
    9568:	682f0064 	.word	0x682f0064
    956c:	2f656d6f 	.word	0x2f656d6f
    9570:	6b6e656a 	.word	0x6b6e656a
    9574:	2f736e69 	.word	0x2f736e69
    9578:	636c6473 	.word	0x636c6473
    957c:	726f772f 	.word	0x726f772f
    9580:	6170736b 	.word	0x6170736b
    9584:	722f6563 	.word	0x722f6563
    9588:	2d6b6473 	.word	0x2d6b6473
    958c:	656c6572 	.word	0x656c6572
    9590:	2d657361 	.word	0x2d657361
    9594:	776f6c66 	.word	0x776f6c66
    9598:	6975622f 	.word	0x6975622f
    959c:	622f646c 	.word	0x622f646c
    95a0:	646c6975 	.word	0x646c6975
    95a4:	6473612d 	.word	0x6473612d
    95a8:	30312d6b 	.word	0x30312d6b
    95ac:	77656e2d 	.word	0x77656e2d
    95b0:	2d62696c 	.word	0x2d62696c
    95b4:	32383633 	.word	0x32383633
    95b8:	77656e2f 	.word	0x77656e2f
    95bc:	2d62696c 	.word	0x2d62696c
    95c0:	2f312e34 	.word	0x2f312e34
    95c4:	6c77656e 	.word	0x6c77656e
    95c8:	6c2f6269 	.word	0x6c2f6269
    95cc:	2f636269 	.word	0x2f636269
    95d0:	69727473 	.word	0x69727473
    95d4:	732f676e 	.word	0x732f676e
    95d8:	6f747274 	.word	0x6f747274
    95dc:	00632e6b 	.word	0x00632e6b
    95e0:	7566202c 	.word	0x7566202c
    95e4:	6974636e 	.word	0x6974636e
    95e8:	203a6e6f 	.word	0x203a6e6f
    95ec:	73736100 	.word	0x73736100
    95f0:	69747265 	.word	0x69747265
    95f4:	22206e6f 	.word	0x22206e6f
    95f8:	20227325 	.word	0x20227325
    95fc:	6c696166 	.word	0x6c696166
    9600:	203a6465 	.word	0x203a6465
    9604:	656c6966 	.word	0x656c6966
    9608:	73252220 	.word	0x73252220
    960c:	6c202c22 	.word	0x6c202c22
    9610:	20656e69 	.word	0x20656e69
    9614:	73256425 	.word	0x73256425
    9618:	000a7325 	.word	0x000a7325

0000961c <_ctype_>:
    961c:	20202000 20202020 28282020 20282828     .         ((((( 
    962c:	20202020 20202020 20202020 20202020                     
    963c:	10108820 10101010 10101010 10101010      ...............
    964c:	04040410 04040404 10040404 10101010     ................
    965c:	41411010 41414141 01010101 01010101     ..AAAAAA........
    966c:	01010101 01010101 01010101 10101010     ................
    967c:	42421010 42424242 02020202 02020202     ..BBBBBB........
    968c:	02020202 02020202 02020202 10101010     ................
    969c:	00000020 00000000 00000000 00000000      ...............
	...

00009720 <__sf_fake_stderr>:
	...

00009740 <__sf_fake_stdout>:
	...

00009760 <__sf_fake_stdin>:
	...

00009780 <_global_impure_ptr>:
    9780:	20002004 2b302d23 6c680020 6665004c     . . #-0+ .hlL.ef
    9790:	47464567 32313000 36353433 41393837     gEFG.0123456789A
    97a0:	45444342 31300046 35343332 39383736     BCDEF.0123456789
    97b0:	64636261 2b006665 3030002d 00587800     abcdef.+-.00.xX.
    97c0:	61462d41 3938662d 33323130 37363534     A-Fa-f8901234567
    97d0:	0000005d 000097b7 000097ba 000097bd     ]...............
