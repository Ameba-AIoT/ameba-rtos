
/home/auto_jenkins_wlan5/IOT_ATS/repos/release-master/source/amebalite_gcc_project/project_kr4/vsdk/image_mp/target_img2.axf:     file format elf32-littleriscv


Disassembly of section .xip_image2.text:

20058060 <BOOT_InitGP>:
20058060:	0000e197          	auipc	gp,0xe
20058064:	07818193          	addi	gp,gp,120 # 200660d8 <__global_pointer$>
20058068:	8082                	c.jr	ra

2005806a <set_psram_wakeup_mode>:
2005806a:	7159                	c.addi16sp	sp,-112
2005806c:	d686                	c.swsp	ra,108(sp)
2005806e:	d4a2                	c.swsp	s0,104(sp)
20058070:	1880                	c.addi4spn	s0,sp,112
20058072:	f8a42e23          	sw	a0,-100(s0)
20058076:	4100c7b7          	lui	a5,0x4100c
2005807a:	21878793          	addi	a5,a5,536 # 4100c218 <__km4_bd_boot_download_addr__+0x10ffa218>
2005807e:	439c                	c.lw	a5,0(a5)
20058080:	8399                	c.srli	a5,0x6
20058082:	8b8d                	c.andi	a5,3
20058084:	fef42623          	sw	a5,-20(s0)
20058088:	4100d7b7          	lui	a5,0x4100d
2005808c:	e0078793          	addi	a5,a5,-512 # 4100ce00 <__km4_bd_boot_download_addr__+0x10ffae00>
20058090:	fef42423          	sw	a5,-24(s0)
20058094:	410087b7          	lui	a5,0x41008
20058098:	fef42223          	sw	a5,-28(s0)
2005809c:	4601                	c.li	a2,0
2005809e:	4581                	c.li	a1,0
200580a0:	4501                	c.li	a0,0
200580a2:	29e1                	c.jal	2005857a <PSRAM_AutoGating>
200580a4:	600007b7          	lui	a5,0x60000
200580a8:	00078793          	addi	a5,a5,0 # 60000000 <__km4_bd_psram_start__>
200580ac:	fcf42e23          	sw	a5,-36(s0)
200580b0:	02000793          	addi	a5,zero,32
200580b4:	fcf42c23          	sw	a5,-40(s0)
200580b8:	7c1027f3          	csrrs	a5,0x7c1,zero
200580bc:	fcf42a23          	sw	a5,-44(s0)
200580c0:	fd442783          	lw	a5,-44(s0)
200580c4:	1007f793          	andi	a5,a5,256
200580c8:	cbe9                	c.beqz	a5,2005819a <set_psram_wakeup_mode+0x130>
200580ca:	fdc42703          	lw	a4,-36(s0)
200580ce:	57fd                	c.li	a5,-1
200580d0:	02f71963          	bne	a4,a5,20058102 <set_psram_wakeup_mode+0x98>
200580d4:	fd842703          	lw	a4,-40(s0)
200580d8:	57fd                	c.li	a5,-1
200580da:	02f71463          	bne	a4,a5,20058102 <set_psram_wakeup_mode+0x98>
200580de:	7c0467f3          	csrrsi	a5,0x7c0,8
200580e2:	fcf42823          	sw	a5,-48(s0)
200580e6:	0ff0000f          	fence	iorw,iorw
200580ea:	0001                	c.addi	zero,0
200580ec:	0001                	c.addi	zero,0
200580ee:	0001                	c.addi	zero,0
200580f0:	0001                	c.addi	zero,0
200580f2:	0001                	c.addi	zero,0
200580f4:	0001                	c.addi	zero,0
200580f6:	0001                	c.addi	zero,0
200580f8:	0001                	c.addi	zero,0
200580fa:	0001                	c.addi	zero,0
200580fc:	0001                	c.addi	zero,0
200580fe:	0001                	c.addi	zero,0
20058100:	a869                	c.j	2005819a <set_psram_wakeup_mode+0x130>
20058102:	fdc42783          	lw	a5,-36(s0)
20058106:	8bfd                	c.andi	a5,31
20058108:	cf81                	c.beqz	a5,20058120 <set_psram_wakeup_mode+0xb6>
2005810a:	fdc42703          	lw	a4,-36(s0)
2005810e:	9b01                	c.andi	a4,-32
20058110:	fce42623          	sw	a4,-52(s0)
20058114:	fd842703          	lw	a4,-40(s0)
20058118:	97ba                	c.add	a5,a4
2005811a:	fcf42423          	sw	a5,-56(s0)
2005811e:	a809                	c.j	20058130 <set_psram_wakeup_mode+0xc6>
20058120:	fd842783          	lw	a5,-40(s0)
20058124:	fcf42423          	sw	a5,-56(s0)
20058128:	fdc42783          	lw	a5,-36(s0)
2005812c:	fcf42623          	sw	a5,-52(s0)
20058130:	fc842783          	lw	a5,-56(s0)
20058134:	06f05363          	bge	zero,a5,2005819a <set_psram_wakeup_mode+0x130>
20058138:	fcc42783          	lw	a5,-52(s0)
2005813c:	01f7f713          	andi	a4,a5,31
20058140:	fc842783          	lw	a5,-56(s0)
20058144:	97ba                	c.add	a5,a4
20058146:	fcf42223          	sw	a5,-60(s0)
2005814a:	0ff0000f          	fence	iorw,iorw
2005814e:	fcc42783          	lw	a5,-52(s0)
20058152:	fcf42023          	sw	a5,-64(s0)
20058156:	0001                	c.addi	zero,0
20058158:	fc042783          	lw	a5,-64(s0)
2005815c:	02f7f00b          	cache	dwbinv,(a5)
20058160:	fc042783          	lw	a5,-64(s0)
20058164:	02078793          	addi	a5,a5,32
20058168:	fcf42023          	sw	a5,-64(s0)
2005816c:	fc442783          	lw	a5,-60(s0)
20058170:	1781                	c.addi	a5,-32
20058172:	fcf42223          	sw	a5,-60(s0)
20058176:	fc442783          	lw	a5,-60(s0)
2005817a:	fcf04fe3          	blt	zero,a5,20058158 <set_psram_wakeup_mode+0xee>
2005817e:	0ff0000f          	fence	iorw,iorw
20058182:	0001                	c.addi	zero,0
20058184:	0001                	c.addi	zero,0
20058186:	0001                	c.addi	zero,0
20058188:	0001                	c.addi	zero,0
2005818a:	0001                	c.addi	zero,0
2005818c:	0001                	c.addi	zero,0
2005818e:	0001                	c.addi	zero,0
20058190:	0001                	c.addi	zero,0
20058192:	0001                	c.addi	zero,0
20058194:	0001                	c.addi	zero,0
20058196:	0001                	c.addi	zero,0
20058198:	0001                	c.addi	zero,0
2005819a:	600007b7          	lui	a5,0x60000
2005819e:	00078793          	addi	a5,a5,0 # 60000000 <__km4_bd_psram_start__>
200581a2:	439c                	c.lw	a5,0(a5)
200581a4:	fef42023          	sw	a5,-32(s0)
200581a8:	fe442783          	lw	a5,-28(s0)
200581ac:	4b9c                	c.lw	a5,16(a5)
200581ae:	fef42023          	sw	a5,-32(s0)
200581b2:	fe442783          	lw	a5,-28(s0)
200581b6:	fe042703          	lw	a4,-32(s0)
200581ba:	cb98                	c.sw	a4,16(a5)
200581bc:	f9c42783          	lw	a5,-100(s0)
200581c0:	eb81                	c.bnez	a5,200581d0 <set_psram_wakeup_mode+0x166>
200581c2:	09600513          	addi	a0,zero,150
200581c6:	dffaa097          	auipc	ra,0xdffaa
200581ca:	0f4080e7          	jalr	ra,244(ra) # 22ba <DelayUs>
200581ce:	a821                	c.j	200581e6 <set_psram_wakeup_mode+0x17c>
200581d0:	f9c42703          	lw	a4,-100(s0)
200581d4:	4785                	c.li	a5,1
200581d6:	00f71863          	bne	a4,a5,200581e6 <set_psram_wakeup_mode+0x17c>
200581da:	06400513          	addi	a0,zero,100
200581de:	dffaa097          	auipc	ra,0xdffaa
200581e2:	0dc080e7          	jalr	ra,220(ra) # 22ba <DelayUs>
200581e6:	600007b7          	lui	a5,0x60000
200581ea:	00078793          	addi	a5,a5,0 # 60000000 <__km4_bd_psram_start__>
200581ee:	faf42e23          	sw	a5,-68(s0)
200581f2:	02000793          	addi	a5,zero,32
200581f6:	faf42c23          	sw	a5,-72(s0)
200581fa:	7c1027f3          	csrrs	a5,0x7c1,zero
200581fe:	faf42a23          	sw	a5,-76(s0)
20058202:	fb442783          	lw	a5,-76(s0)
20058206:	1007f793          	andi	a5,a5,256
2005820a:	cbe9                	c.beqz	a5,200582dc <set_psram_wakeup_mode+0x272>
2005820c:	fbc42703          	lw	a4,-68(s0)
20058210:	57fd                	c.li	a5,-1
20058212:	02f71963          	bne	a4,a5,20058244 <set_psram_wakeup_mode+0x1da>
20058216:	fb842703          	lw	a4,-72(s0)
2005821a:	57fd                	c.li	a5,-1
2005821c:	02f71463          	bne	a4,a5,20058244 <set_psram_wakeup_mode+0x1da>
20058220:	7c0167f3          	csrrsi	a5,0x7c0,2
20058224:	faf42823          	sw	a5,-80(s0)
20058228:	0ff0000f          	fence	iorw,iorw
2005822c:	0001                	c.addi	zero,0
2005822e:	0001                	c.addi	zero,0
20058230:	0001                	c.addi	zero,0
20058232:	0001                	c.addi	zero,0
20058234:	0001                	c.addi	zero,0
20058236:	0001                	c.addi	zero,0
20058238:	0001                	c.addi	zero,0
2005823a:	0001                	c.addi	zero,0
2005823c:	0001                	c.addi	zero,0
2005823e:	0001                	c.addi	zero,0
20058240:	0001                	c.addi	zero,0
20058242:	a869                	c.j	200582dc <set_psram_wakeup_mode+0x272>
20058244:	fbc42783          	lw	a5,-68(s0)
20058248:	8bfd                	c.andi	a5,31
2005824a:	cf81                	c.beqz	a5,20058262 <set_psram_wakeup_mode+0x1f8>
2005824c:	fbc42703          	lw	a4,-68(s0)
20058250:	9b01                	c.andi	a4,-32
20058252:	fae42623          	sw	a4,-84(s0)
20058256:	fb842703          	lw	a4,-72(s0)
2005825a:	97ba                	c.add	a5,a4
2005825c:	faf42423          	sw	a5,-88(s0)
20058260:	a809                	c.j	20058272 <set_psram_wakeup_mode+0x208>
20058262:	fb842783          	lw	a5,-72(s0)
20058266:	faf42423          	sw	a5,-88(s0)
2005826a:	fbc42783          	lw	a5,-68(s0)
2005826e:	faf42623          	sw	a5,-84(s0)
20058272:	fa842783          	lw	a5,-88(s0)
20058276:	06f05363          	bge	zero,a5,200582dc <set_psram_wakeup_mode+0x272>
2005827a:	fac42783          	lw	a5,-84(s0)
2005827e:	01f7f713          	andi	a4,a5,31
20058282:	fa842783          	lw	a5,-88(s0)
20058286:	97ba                	c.add	a5,a4
20058288:	faf42223          	sw	a5,-92(s0)
2005828c:	0ff0000f          	fence	iorw,iorw
20058290:	fac42783          	lw	a5,-84(s0)
20058294:	faf42023          	sw	a5,-96(s0)
20058298:	0001                	c.addi	zero,0
2005829a:	fa042783          	lw	a5,-96(s0)
2005829e:	00f7f00b          	cache	dinv,(a5)
200582a2:	fa042783          	lw	a5,-96(s0)
200582a6:	02078793          	addi	a5,a5,32
200582aa:	faf42023          	sw	a5,-96(s0)
200582ae:	fa442783          	lw	a5,-92(s0)
200582b2:	1781                	c.addi	a5,-32
200582b4:	faf42223          	sw	a5,-92(s0)
200582b8:	fa442783          	lw	a5,-92(s0)
200582bc:	fcf04fe3          	blt	zero,a5,2005829a <set_psram_wakeup_mode+0x230>
200582c0:	0ff0000f          	fence	iorw,iorw
200582c4:	0001                	c.addi	zero,0
200582c6:	0001                	c.addi	zero,0
200582c8:	0001                	c.addi	zero,0
200582ca:	0001                	c.addi	zero,0
200582cc:	0001                	c.addi	zero,0
200582ce:	0001                	c.addi	zero,0
200582d0:	0001                	c.addi	zero,0
200582d2:	0001                	c.addi	zero,0
200582d4:	0001                	c.addi	zero,0
200582d6:	0001                	c.addi	zero,0
200582d8:	0001                	c.addi	zero,0
200582da:	0001                	c.addi	zero,0
200582dc:	600007b7          	lui	a5,0x60000
200582e0:	00078793          	addi	a5,a5,0 # 60000000 <__km4_bd_psram_start__>
200582e4:	439c                	c.lw	a5,0(a5)
200582e6:	fef42023          	sw	a5,-32(s0)
200582ea:	fe442783          	lw	a5,-28(s0)
200582ee:	439c                	c.lw	a5,0(a5)
200582f0:	83c1                	c.srli	a5,0x10
200582f2:	8b85                	c.andi	a5,1
200582f4:	eb8d                	c.bnez	a5,20058326 <set_psram_wakeup_mode+0x2bc>
200582f6:	fe442783          	lw	a5,-28(s0)
200582fa:	439c                	c.lw	a5,0(a5)
200582fc:	83bd                	c.srli	a5,0xf
200582fe:	8b85                	c.andi	a5,1
20058300:	e39d                	c.bnez	a5,20058326 <set_psram_wakeup_mode+0x2bc>
20058302:	fe442783          	lw	a5,-28(s0)
20058306:	439c                	c.lw	a5,0(a5)
20058308:	83b9                	c.srli	a5,0xe
2005830a:	8b85                	c.andi	a5,1
2005830c:	ef89                	c.bnez	a5,20058326 <set_psram_wakeup_mode+0x2bc>
2005830e:	fe442783          	lw	a5,-28(s0)
20058312:	439c                	c.lw	a5,0(a5)
20058314:	83b5                	c.srli	a5,0xd
20058316:	8b85                	c.andi	a5,1
20058318:	e799                	c.bnez	a5,20058326 <set_psram_wakeup_mode+0x2bc>
2005831a:	fe442783          	lw	a5,-28(s0)
2005831e:	439c                	c.lw	a5,0(a5)
20058320:	83b1                	c.srli	a5,0xc
20058322:	8b85                	c.andi	a5,1
20058324:	cbc5                	c.beqz	a5,200583d4 <set_psram_wakeup_mode+0x36a>
20058326:	fe442783          	lw	a5,-28(s0)
2005832a:	439c                	c.lw	a5,0(a5)
2005832c:	fef42023          	sw	a5,-32(s0)
20058330:	fe442783          	lw	a5,-28(s0)
20058334:	fe042703          	lw	a4,-32(s0)
20058338:	c398                	c.sw	a4,0(a5)
2005833a:	4501                	c.li	a0,0
2005833c:	4c0010ef          	jal	ra,200597fc <PSRAM_calibration>
20058340:	600007b7          	lui	a5,0x60000
20058344:	00078793          	addi	a5,a5,0 # 60000000 <__km4_bd_psram_start__>
20058348:	fe842703          	lw	a4,-24(s0)
2005834c:	14c72703          	lw	a4,332(a4)
20058350:	c398                	c.sw	a4,0(a5)
20058352:	600007b7          	lui	a5,0x60000
20058356:	00078713          	addi	a4,a5,0 # 60000000 <__km4_bd_psram_start__>
2005835a:	000507b7          	lui	a5,0x50
2005835e:	97ba                	c.add	a5,a4
20058360:	873e                	c.mv	a4,a5
20058362:	fe842783          	lw	a5,-24(s0)
20058366:	1507a783          	lw	a5,336(a5) # 50150 <__ap_sram_heap_size+0x10150>
2005836a:	c31c                	c.sw	a5,0(a4)
2005836c:	600007b7          	lui	a5,0x60000
20058370:	00078713          	addi	a4,a5,0 # 60000000 <__km4_bd_psram_start__>
20058374:	001007b7          	lui	a5,0x100
20058378:	97ba                	c.add	a5,a4
2005837a:	873e                	c.mv	a4,a5
2005837c:	fe842783          	lw	a5,-24(s0)
20058380:	1547a783          	lw	a5,340(a5) # 100154 <__ap_sram_heap_size+0xc0154>
20058384:	c31c                	c.sw	a5,0(a4)
20058386:	600007b7          	lui	a5,0x60000
2005838a:	00078713          	addi	a4,a5,0 # 60000000 <__km4_bd_psram_start__>
2005838e:	001507b7          	lui	a5,0x150
20058392:	97ba                	c.add	a5,a4
20058394:	873e                	c.mv	a4,a5
20058396:	fe842783          	lw	a5,-24(s0)
2005839a:	1587a783          	lw	a5,344(a5) # 150158 <__ap_sram_heap_size+0x110158>
2005839e:	c31c                	c.sw	a5,0(a4)
200583a0:	600007b7          	lui	a5,0x60000
200583a4:	00078713          	addi	a4,a5,0 # 60000000 <__km4_bd_psram_start__>
200583a8:	002007b7          	lui	a5,0x200
200583ac:	97ba                	c.add	a5,a4
200583ae:	873e                	c.mv	a4,a5
200583b0:	fe842783          	lw	a5,-24(s0)
200583b4:	15c7a783          	lw	a5,348(a5) # 20015c <__ap_sram_heap_size+0x1c015c>
200583b8:	c31c                	c.sw	a5,0(a4)
200583ba:	600007b7          	lui	a5,0x60000
200583be:	00078713          	addi	a4,a5,0 # 60000000 <__km4_bd_psram_start__>
200583c2:	002507b7          	lui	a5,0x250
200583c6:	97ba                	c.add	a5,a4
200583c8:	873e                	c.mv	a4,a5
200583ca:	fe842783          	lw	a5,-24(s0)
200583ce:	1607a783          	lw	a5,352(a5) # 250160 <__ap_sram_heap_size+0x210160>
200583d2:	c31c                	c.sw	a5,0(a4)
200583d4:	fec42703          	lw	a4,-20(s0)
200583d8:	478d                	c.li	a5,3
200583da:	00f71763          	bne	a4,a5,200583e8 <set_psram_wakeup_mode+0x37e>
200583de:	460d                	c.li	a2,3
200583e0:	45a9                	c.li	a1,10
200583e2:	4505                	c.li	a0,1
200583e4:	2a59                	c.jal	2005857a <PSRAM_AutoGating>
200583e6:	a01d                	c.j	2005840c <set_psram_wakeup_mode+0x3a2>
200583e8:	fec42783          	lw	a5,-20(s0)
200583ec:	c791                	c.beqz	a5,200583f8 <set_psram_wakeup_mode+0x38e>
200583ee:	fec42703          	lw	a4,-20(s0)
200583f2:	4785                	c.li	a5,1
200583f4:	00f71c63          	bne	a4,a5,2005840c <set_psram_wakeup_mode+0x3a2>
200583f8:	fe842783          	lw	a5,-24(s0)
200583fc:	13d7c783          	lbu	a5,317(a5)
20058400:	0ff7f793          	andi	a5,a5,255
20058404:	863e                	c.mv	a2,a5
20058406:	4585                	c.li	a1,1
20058408:	4505                	c.li	a0,1
2005840a:	2a85                	c.jal	2005857a <PSRAM_AutoGating>
2005840c:	0001                	c.addi	zero,0
2005840e:	50b6                	c.lwsp	ra,108(sp)
20058410:	5426                	c.lwsp	s0,104(sp)
20058412:	6165                	c.addi16sp	sp,112
20058414:	8082                	c.jr	ra

20058416 <PSRAM_REG_Read>:
20058416:	410097b7          	lui	a5,0x41009
2005841a:	0287a803          	lw	a6,40(a5) # 41009028 <__km4_bd_boot_download_addr__+0x10ff7028>
2005841e:	00187813          	andi	a6,a6,1
20058422:	fe081ce3          	bne	a6,zero,2005841a <PSRAM_REG_Read+0x4>
20058426:	0007a803          	lw	a6,0(a5)
2005842a:	800008b7          	lui	a7,0x80000
2005842e:	01186833          	or	a6,a6,a7
20058432:	0107a023          	sw	a6,0(a5)
20058436:	00861813          	slli	a6,a2,0x8
2005843a:	0007a423          	sw	zero,8(a5)
2005843e:	00885813          	srli	a6,a6,0x8
20058442:	0107a223          	sw	a6,4(a5)
20058446:	1207a823          	sw	zero,304(a5)
2005844a:	0007a803          	lw	a6,0(a5)
2005844e:	30086813          	ori	a6,a6,768
20058452:	0107a023          	sw	a6,0(a5)
20058456:	ed21                	c.bnez	a0,200584ae <PSRAM_REG_Read+0x98>
20058458:	04000513          	addi	a0,zero,64
2005845c:	06a78023          	sb	a0,96(a5)
20058460:	06a78023          	sb	a0,96(a5)
20058464:	410097b7          	lui	a5,0x41009
20058468:	cb29                	c.beqz	a4,200584ba <PSRAM_REG_Read+0xa4>
2005846a:	4705                	c.li	a4,1
2005846c:	06e78023          	sb	a4,96(a5) # 41009060 <__km4_bd_boot_download_addr__+0x10ff7060>
20058470:	410097b7          	lui	a5,0x41009
20058474:	06078023          	sb	zero,96(a5) # 41009060 <__km4_bd_boot_download_addr__+0x10ff7060>
20058478:	06078023          	sb	zero,96(a5)
2005847c:	0ff5f593          	andi	a1,a1,255
20058480:	4705                	c.li	a4,1
20058482:	06b78023          	sb	a1,96(a5)
20058486:	c798                	c.sw	a4,8(a5)
20058488:	41009737          	lui	a4,0x41009
2005848c:	471c                	c.lw	a5,8(a4)
2005848e:	8b85                	c.andi	a5,1
20058490:	fff5                	c.bnez	a5,2005848c <PSRAM_REG_Read+0x76>
20058492:	41009737          	lui	a4,0x41009
20058496:	02c79563          	bne	a5,a2,200584c0 <PSRAM_REG_Read+0xaa>
2005849a:	41009737          	lui	a4,0x41009
2005849e:	571c                	c.lw	a5,40(a4)
200584a0:	8b85                	c.andi	a5,1
200584a2:	fff5                	c.bnez	a5,2005849e <PSRAM_REG_Read+0x88>
200584a4:	431c                	c.lw	a5,0(a4)
200584a6:	0786                	c.slli	a5,0x1
200584a8:	8385                	c.srli	a5,0x1
200584aa:	c31c                	c.sw	a5,0(a4)
200584ac:	8082                	c.jr	ra
200584ae:	5501                	c.li	a0,-32
200584b0:	06a78023          	sb	a0,96(a5)
200584b4:	06078023          	sb	zero,96(a5)
200584b8:	b775                	c.j	20058464 <PSRAM_REG_Read+0x4e>
200584ba:	06078023          	sb	zero,96(a5)
200584be:	bf4d                	c.j	20058470 <PSRAM_REG_Read+0x5a>
200584c0:	06074503          	lbu	a0,96(a4) # 41009060 <__km4_bd_boot_download_addr__+0x10ff7060>
200584c4:	00f685b3          	add	a1,a3,a5
200584c8:	0785                	c.addi	a5,1
200584ca:	00a58023          	sb	a0,0(a1)
200584ce:	b7e1                	c.j	20058496 <PSRAM_REG_Read+0x80>

200584d0 <PSRAM_REG_Write>:
200584d0:	410097b7          	lui	a5,0x41009
200584d4:	5798                	c.lw	a4,40(a5)
200584d6:	8b05                	c.andi	a4,1
200584d8:	ff75                	c.bnez	a4,200584d4 <PSRAM_REG_Write+0x4>
200584da:	0007a803          	lw	a6,0(a5) # 41009000 <__km4_bd_boot_download_addr__+0x10ff7000>
200584de:	800008b7          	lui	a7,0x80000
200584e2:	01186833          	or	a6,a6,a7
200584e6:	0107a023          	sw	a6,0(a5)
200584ea:	0007a423          	sw	zero,8(a5)
200584ee:	0007a803          	lw	a6,0(a5)
200584f2:	cff87813          	andi	a6,a6,-769
200584f6:	0107a023          	sw	a6,0(a5)
200584fa:	00861813          	slli	a6,a2,0x8
200584fe:	0007a223          	sw	zero,4(a5)
20058502:	00885813          	srli	a6,a6,0x8
20058506:	1307a823          	sw	a6,304(a5)
2005850a:	e929                	c.bnez	a0,2005855c <PSRAM_REG_Write+0x8c>
2005850c:	fc000513          	addi	a0,zero,-64
20058510:	06a78023          	sb	a0,96(a5)
20058514:	06a78023          	sb	a0,96(a5)
20058518:	410097b7          	lui	a5,0x41009
2005851c:	4505                	c.li	a0,1
2005851e:	06a78023          	sb	a0,96(a5) # 41009060 <__km4_bd_boot_download_addr__+0x10ff7060>
20058522:	06078023          	sb	zero,96(a5)
20058526:	06078023          	sb	zero,96(a5)
2005852a:	0ff5f593          	andi	a1,a1,255
2005852e:	06b78023          	sb	a1,96(a5)
20058532:	02c71c63          	bne	a4,a2,2005856a <PSRAM_REG_Write+0x9a>
20058536:	4705                	c.li	a4,1
20058538:	410097b7          	lui	a5,0x41009
2005853c:	c798                	c.sw	a4,8(a5)
2005853e:	41009737          	lui	a4,0x41009
20058542:	471c                	c.lw	a5,8(a4)
20058544:	8b85                	c.andi	a5,1
20058546:	fff5                	c.bnez	a5,20058542 <PSRAM_REG_Write+0x72>
20058548:	41009737          	lui	a4,0x41009
2005854c:	571c                	c.lw	a5,40(a4)
2005854e:	8b85                	c.andi	a5,1
20058550:	fff5                	c.bnez	a5,2005854c <PSRAM_REG_Write+0x7c>
20058552:	431c                	c.lw	a5,0(a4)
20058554:	0786                	c.slli	a5,0x1
20058556:	8385                	c.srli	a5,0x1
20058558:	c31c                	c.sw	a5,0(a4)
2005855a:	8082                	c.jr	ra
2005855c:	06000513          	addi	a0,zero,96
20058560:	06a78023          	sb	a0,96(a5) # 41009060 <__km4_bd_boot_download_addr__+0x10ff7060>
20058564:	06078023          	sb	zero,96(a5)
20058568:	bf45                	c.j	20058518 <PSRAM_REG_Write+0x48>
2005856a:	00e685b3          	add	a1,a3,a4
2005856e:	0005c583          	lbu	a1,0(a1)
20058572:	0705                	c.addi	a4,1
20058574:	06b78023          	sb	a1,96(a5)
20058578:	bf6d                	c.j	20058532 <PSRAM_REG_Write+0x62>

2005857a <PSRAM_AutoGating>:
2005857a:	c139                	c.beqz	a0,200585c0 <PSRAM_AutoGating+0x46>
2005857c:	4100c737          	lui	a4,0x4100c
20058580:	22472783          	lw	a5,548(a4) # 4100c224 <__km4_bd_boot_download_addr__+0x10ffa224>
20058584:	66c1                	c.lui	a3,0x10
20058586:	05c2                	c.slli	a1,0x10
20058588:	8fd5                	c.or	a5,a3
2005858a:	22f72223          	sw	a5,548(a4)
2005858e:	4100d7b7          	lui	a5,0x4100d
20058592:	98078793          	addi	a5,a5,-1664 # 4100c980 <__km4_bd_boot_download_addr__+0x10ffa980>
20058596:	5fd8                	c.lw	a4,60(a5)
20058598:	01ff06b7          	lui	a3,0x1ff0
2005859c:	8df5                	c.and	a1,a3
2005859e:	c00106b7          	lui	a3,0xc0010
200585a2:	16fd                	c.addi	a3,-1
200585a4:	8f75                	c.and	a4,a3
200585a6:	8dd9                	c.or	a1,a4
200585a8:	0666                	c.slli	a2,0x19
200585aa:	3e000737          	lui	a4,0x3e000
200585ae:	8e79                	c.and	a2,a4
200585b0:	8e4d                	c.or	a2,a1
200585b2:	dfd0                	c.sw	a2,60(a5)
200585b4:	5fd8                	c.lw	a4,60(a5)
200585b6:	800006b7          	lui	a3,0x80000
200585ba:	8f55                	c.or	a4,a3
200585bc:	dfd8                	c.sw	a4,60(a5)
200585be:	8082                	c.jr	ra
200585c0:	4100c6b7          	lui	a3,0x4100c
200585c4:	2246a783          	lw	a5,548(a3) # 4100c224 <__km4_bd_boot_download_addr__+0x10ffa224>
200585c8:	7741                	c.lui	a4,0xffff0
200585ca:	177d                	c.addi	a4,-1
200585cc:	8ff9                	c.and	a5,a4
200585ce:	22f6a223          	sw	a5,548(a3)
200585d2:	4100d737          	lui	a4,0x4100d
200585d6:	9bc72783          	lw	a5,-1604(a4) # 4100c9bc <__km4_bd_boot_download_addr__+0x10ffa9bc>
200585da:	0786                	c.slli	a5,0x1
200585dc:	8385                	c.srli	a5,0x1
200585de:	9af72e23          	sw	a5,-1604(a4)
200585e2:	8082                	c.jr	ra

200585e4 <set_psram_sleep_mode>:
200585e4:	1101                	c.addi	sp,-32
200585e6:	ca4a                	c.swsp	s2,20(sp)
200585e8:	c84e                	c.swsp	s3,16(sp)
200585ea:	ce06                	c.swsp	ra,28(sp)
200585ec:	cc26                	c.swsp	s1,24(sp)
200585ee:	4100c7b7          	lui	a5,0x4100c
200585f2:	2187a483          	lw	s1,536(a5) # 4100c218 <__km4_bd_boot_download_addr__+0x10ffa218>
200585f6:	1f100713          	addi	a4,zero,497
200585fa:	410087b7          	lui	a5,0x41008
200585fe:	c398                	c.sw	a4,0(a5)
20058600:	600007b7          	lui	a5,0x60000
20058604:	0007c683          	lbu	a3,0(a5) # 60000000 <__km4_bd_psram_start__>
20058608:	0017c703          	lbu	a4,1(a5)
2005860c:	89aa                	c.mv	s3,a0
2005860e:	0ff6f693          	andi	a3,a3,255
20058612:	0ff77713          	andi	a4,a4,255
20058616:	0722                	c.slli	a4,0x8
20058618:	8ed9                	c.or	a3,a4
2005861a:	0027c703          	lbu	a4,2(a5)
2005861e:	4501                	c.li	a0,0
20058620:	4601                	c.li	a2,0
20058622:	0ff77713          	andi	a4,a4,255
20058626:	0742                	c.slli	a4,0x10
20058628:	8f55                	c.or	a4,a3
2005862a:	0037c683          	lbu	a3,3(a5)
2005862e:	00078793          	addi	a5,a5,0
20058632:	4581                	c.li	a1,0
20058634:	06e2                	c.slli	a3,0x18
20058636:	8ed9                	c.or	a3,a4
20058638:	4100d737          	lui	a4,0x4100d
2005863c:	f0070713          	addi	a4,a4,-256 # 4100cf00 <__km4_bd_boot_download_addr__+0x10ffaf00>
20058640:	c774                	c.sw	a3,76(a4)
20058642:	000506b7          	lui	a3,0x50
20058646:	96be                	c.add	a3,a5
20058648:	4294                	c.lw	a3,0(a3)
2005864a:	8099                	c.srli	s1,0x6
2005864c:	0034f913          	andi	s2,s1,3
20058650:	cb34                	c.sw	a3,80(a4)
20058652:	001006b7          	lui	a3,0x100
20058656:	96be                	c.add	a3,a5
20058658:	4294                	c.lw	a3,0(a3)
2005865a:	cb74                	c.sw	a3,84(a4)
2005865c:	001506b7          	lui	a3,0x150
20058660:	96be                	c.add	a3,a5
20058662:	4294                	c.lw	a3,0(a3)
20058664:	cf34                	c.sw	a3,88(a4)
20058666:	002006b7          	lui	a3,0x200
2005866a:	96be                	c.add	a3,a5
2005866c:	4294                	c.lw	a3,0(a3)
2005866e:	cf74                	c.sw	a3,92(a4)
20058670:	002506b7          	lui	a3,0x250
20058674:	97b6                	c.add	a5,a3
20058676:	439c                	c.lw	a5,0(a5)
20058678:	d33c                	c.sw	a5,96(a4)
2005867a:	3701                	c.jal	2005857a <PSRAM_AutoGating>
2005867c:	4505                	c.li	a0,1
2005867e:	dffaa097          	auipc	ra,0xdffaa
20058682:	c3c080e7          	jalr	ra,-964(ra) # 22ba <DelayUs>
20058686:	02099963          	bne	s3,zero,200586b8 <set_psram_sleep_mode+0xd4>
2005868a:	77fd                	c.lui	a5,0xfffff
2005868c:	0f078793          	addi	a5,a5,240 # fffff0f0 <__ctrace_end__+0x35fff0f0>
20058690:	00f11423          	sh	a5,8(sp)
20058694:	0034                	c.addi4spn	a3,sp,8
20058696:	4609                	c.li	a2,2
20058698:	4599                	c.li	a1,6
2005869a:	4501                	c.li	a0,0
2005869c:	3d15                	c.jal	200584d0 <PSRAM_REG_Write>
2005869e:	478d                	c.li	a5,3
200586a0:	04f91663          	bne	s2,a5,200586ec <set_psram_sleep_mode+0x108>
200586a4:	460d                	c.li	a2,3
200586a6:	45a9                	c.li	a1,10
200586a8:	4505                	c.li	a0,1
200586aa:	3dc1                	c.jal	2005857a <PSRAM_AutoGating>
200586ac:	40f2                	c.lwsp	ra,28(sp)
200586ae:	44e2                	c.lwsp	s1,24(sp)
200586b0:	4952                	c.lwsp	s2,20(sp)
200586b2:	49c2                	c.lwsp	s3,16(sp)
200586b4:	6105                	c.addi16sp	sp,32
200586b6:	8082                	c.jr	ra
200586b8:	4701                	c.li	a4,0
200586ba:	0074                	c.addi4spn	a3,sp,12
200586bc:	4609                	c.li	a2,2
200586be:	4581                	c.li	a1,0
200586c0:	4505                	c.li	a0,1
200586c2:	3b91                	c.jal	20058416 <PSRAM_REG_Read>
200586c4:	00d14703          	lbu	a4,13(sp)
200586c8:	05f00793          	addi	a5,zero,95
200586cc:	00f71c63          	bne	a4,a5,200586e4 <set_psram_sleep_mode+0x100>
200586d0:	6789                	c.lui	a5,0x2
200586d2:	0ff78793          	addi	a5,a5,255 # 20ff <CRYPTO_Init+0x969>
200586d6:	00f11423          	sh	a5,8(sp)
200586da:	0034                	c.addi4spn	a3,sp,8
200586dc:	4609                	c.li	a2,2
200586de:	4585                	c.li	a1,1
200586e0:	4505                	c.li	a0,1
200586e2:	bf6d                	c.j	2005869c <set_psram_sleep_mode+0xb8>
200586e4:	77f9                	c.lui	a5,0xffffe
200586e6:	1ff78793          	addi	a5,a5,511 # ffffe1ff <__ctrace_end__+0x35ffe1ff>
200586ea:	b7f5                	c.j	200586d6 <set_psram_sleep_mode+0xf2>
200586ec:	8889                	c.andi	s1,2
200586ee:	fcdd                	c.bnez	s1,200586ac <set_psram_sleep_mode+0xc8>
200586f0:	4100d7b7          	lui	a5,0x4100d
200586f4:	f3d7c603          	lbu	a2,-195(a5) # 4100cf3d <__km4_bd_boot_download_addr__+0x10ffaf3d>
200586f8:	4585                	c.li	a1,1
200586fa:	0ff67613          	andi	a2,a2,255
200586fe:	b76d                	c.j	200586a8 <set_psram_sleep_mode+0xc4>

20058700 <set_psram_suspend>:
20058700:	4100d7b7          	lui	a5,0x4100d
20058704:	f3c7c703          	lbu	a4,-196(a5) # 4100cf3c <__km4_bd_boot_download_addr__+0x10ffaf3c>
20058708:	0ff77713          	andi	a4,a4,255
2005870c:	cb11                	c.beqz	a4,20058720 <set_psram_suspend+0x20>
2005870e:	e0078793          	addi	a5,a5,-512
20058712:	13c7c783          	lbu	a5,316(a5)
20058716:	4705                	c.li	a4,1
20058718:	0ff7f793          	andi	a5,a5,255
2005871c:	02e79963          	bne	a5,a4,2005874e <set_psram_suspend+0x4e>
20058720:	1141                	c.addi	sp,-16
20058722:	c426                	c.swsp	s1,8(sp)
20058724:	c606                	c.swsp	ra,12(sp)
20058726:	4100d4b7          	lui	s1,0x4100d
2005872a:	f3c4c503          	lbu	a0,-196(s1) # 4100cf3c <__km4_bd_boot_download_addr__+0x10ffaf3c>
2005872e:	0ff57513          	andi	a0,a0,255
20058732:	3d4d                	c.jal	200585e4 <set_psram_sleep_mode>
20058734:	9c04a783          	lw	a5,-1600(s1)
20058738:	7771                	c.lui	a4,0xffffc
2005873a:	177d                	c.addi	a4,-1
2005873c:	8ff9                	c.and	a5,a4
2005873e:	6721                	c.lui	a4,0x8
20058740:	8fd9                	c.or	a5,a4
20058742:	9cf4a023          	sw	a5,-1600(s1)
20058746:	40b2                	c.lwsp	ra,12(sp)
20058748:	44a2                	c.lwsp	s1,8(sp)
2005874a:	0141                	c.addi	sp,16
2005874c:	8082                	c.jr	ra
2005874e:	8082                	c.jr	ra

20058750 <set_psram_resume>:
20058750:	4100d7b7          	lui	a5,0x4100d
20058754:	f3c7c703          	lbu	a4,-196(a5) # 4100cf3c <__km4_bd_boot_download_addr__+0x10ffaf3c>
20058758:	e0078793          	addi	a5,a5,-512
2005875c:	0ff77713          	andi	a4,a4,255
20058760:	cb01                	c.beqz	a4,20058770 <set_psram_resume+0x20>
20058762:	13c7c783          	lbu	a5,316(a5)
20058766:	4705                	c.li	a4,1
20058768:	0ff7f793          	andi	a5,a5,255
2005876c:	02e79263          	bne	a5,a4,20058790 <set_psram_resume+0x40>
20058770:	4100d7b7          	lui	a5,0x4100d
20058774:	9c07a703          	lw	a4,-1600(a5) # 4100c9c0 <__km4_bd_boot_download_addr__+0x10ffa9c0>
20058778:	76e1                	c.lui	a3,0xffff8
2005877a:	16fd                	c.addi	a3,-1
2005877c:	8f75                	c.and	a4,a3
2005877e:	6691                	c.lui	a3,0x4
20058780:	8f55                	c.or	a4,a3
20058782:	9ce7a023          	sw	a4,-1600(a5)
20058786:	f3c7c503          	lbu	a0,-196(a5)
2005878a:	0ff57513          	andi	a0,a0,255
2005878e:	b8f1                	c.j	2005806a <set_psram_wakeup_mode>
20058790:	8082                	c.jr	ra

20058792 <SOCPS_memswap>:
20058792:	00c506b3          	add	a3,a0,a2
20058796:	872e                	c.mv	a4,a1
20058798:	87aa                	c.mv	a5,a0
2005879a:	06d79563          	bne	a5,a3,20058804 <SOCPS_memswap+0x72>
2005879e:	7c1027f3          	csrrs	a5,0x7c1,zero
200587a2:	1007f793          	andi	a5,a5,256
200587a6:	c78d                	c.beqz	a5,200587d0 <SOCPS_memswap+0x3e>
200587a8:	57fd                	c.li	a5,-1
200587aa:	06f51863          	bne	a0,a5,2005881a <SOCPS_memswap+0x88>
200587ae:	06a61663          	bne	a2,a0,2005881a <SOCPS_memswap+0x88>
200587b2:	7c0267f3          	csrrsi	a5,0x7c0,4
200587b6:	0ff0000f          	fence	iorw,iorw
200587ba:	0001                	c.addi	zero,0
200587bc:	0001                	c.addi	zero,0
200587be:	0001                	c.addi	zero,0
200587c0:	0001                	c.addi	zero,0
200587c2:	0001                	c.addi	zero,0
200587c4:	0001                	c.addi	zero,0
200587c6:	0001                	c.addi	zero,0
200587c8:	0001                	c.addi	zero,0
200587ca:	0001                	c.addi	zero,0
200587cc:	0001                	c.addi	zero,0
200587ce:	0001                	c.addi	zero,0
200587d0:	7c1027f3          	csrrs	a5,0x7c1,zero
200587d4:	1007f793          	andi	a5,a5,256
200587d8:	c78d                	c.beqz	a5,20058802 <SOCPS_memswap+0x70>
200587da:	57fd                	c.li	a5,-1
200587dc:	06f59763          	bne	a1,a5,2005884a <SOCPS_memswap+0xb8>
200587e0:	06b61563          	bne	a2,a1,2005884a <SOCPS_memswap+0xb8>
200587e4:	7c0267f3          	csrrsi	a5,0x7c0,4
200587e8:	0ff0000f          	fence	iorw,iorw
200587ec:	0001                	c.addi	zero,0
200587ee:	0001                	c.addi	zero,0
200587f0:	0001                	c.addi	zero,0
200587f2:	0001                	c.addi	zero,0
200587f4:	0001                	c.addi	zero,0
200587f6:	0001                	c.addi	zero,0
200587f8:	0001                	c.addi	zero,0
200587fa:	0001                	c.addi	zero,0
200587fc:	0001                	c.addi	zero,0
200587fe:	0001                	c.addi	zero,0
20058800:	0001                	c.addi	zero,0
20058802:	8082                	c.jr	ra
20058804:	00074883          	lbu	a7,0(a4) # 8000 <irq_disable+0x24>
20058808:	0007c803          	lbu	a6,0(a5)
2005880c:	0705                	c.addi	a4,1
2005880e:	01178023          	sb	a7,0(a5)
20058812:	ff070fa3          	sb	a6,-1(a4)
20058816:	0785                	c.addi	a5,1
20058818:	b749                	c.j	2005879a <SOCPS_memswap+0x8>
2005881a:	01f57793          	andi	a5,a0,31
2005881e:	8732                	c.mv	a4,a2
20058820:	c781                	c.beqz	a5,20058828 <SOCPS_memswap+0x96>
20058822:	9901                	c.andi	a0,-32
20058824:	00f60733          	add	a4,a2,a5
20058828:	fae054e3          	bge	zero,a4,200587d0 <SOCPS_memswap+0x3e>
2005882c:	01f57793          	andi	a5,a0,31
20058830:	97ba                	c.add	a5,a4
20058832:	0ff0000f          	fence	iorw,iorw
20058836:	97aa                	c.add	a5,a0
20058838:	04a5700b          	cache	dwb,(a0)
2005883c:	02050513          	addi	a0,a0,32
20058840:	40a78733          	sub	a4,a5,a0
20058844:	fee04ae3          	blt	zero,a4,20058838 <SOCPS_memswap+0xa6>
20058848:	b7bd                	c.j	200587b6 <SOCPS_memswap+0x24>
2005884a:	01f5f793          	andi	a5,a1,31
2005884e:	c399                	c.beqz	a5,20058854 <SOCPS_memswap+0xc2>
20058850:	9981                	c.andi	a1,-32
20058852:	963e                	c.add	a2,a5
20058854:	fac057e3          	bge	zero,a2,20058802 <SOCPS_memswap+0x70>
20058858:	01f5f793          	andi	a5,a1,31
2005885c:	963e                	c.add	a2,a5
2005885e:	0ff0000f          	fence	iorw,iorw
20058862:	962e                	c.add	a2,a1
20058864:	04b5f00b          	cache	dwb,(a1)
20058868:	02058593          	addi	a1,a1,32
2005886c:	40b607b3          	sub	a5,a2,a1
20058870:	fef04ae3          	blt	zero,a5,20058864 <SOCPS_memswap+0xd2>
20058874:	bf95                	c.j	200587e8 <SOCPS_memswap+0x56>

20058876 <SOCPS_Kr4_StartTbl_Backup>:
20058876:	200055b7          	lui	a1,0x20005
2005887a:	20040537          	lui	a0,0x20040
2005887e:	1141                	c.addi	sp,-16
20058880:	02000613          	addi	a2,zero,32
20058884:	c2058593          	addi	a1,a1,-992 # 20004c20 <__kr4_start_table_backup_addr__>
20058888:	02050513          	addi	a0,a0,32 # 20040020 <__kr4_boot_entry_addr__>
2005888c:	c606                	c.swsp	ra,12(sp)
2005888e:	3711                	c.jal	20058792 <SOCPS_memswap>
20058890:	4100d7b7          	lui	a5,0x4100d
20058894:	4705                	c.li	a4,1
20058896:	f4e783a3          	sb	a4,-185(a5) # 4100cf47 <__km4_bd_boot_download_addr__+0x10ffaf47>
2005889a:	40b2                	c.lwsp	ra,12(sp)
2005889c:	0141                	c.addi	sp,16
2005889e:	8082                	c.jr	ra

200588a0 <SOCPS_Kr4_StartTbl_Restore>:
200588a0:	200055b7          	lui	a1,0x20005
200588a4:	20040537          	lui	a0,0x20040
200588a8:	1141                	c.addi	sp,-16
200588aa:	02000613          	addi	a2,zero,32
200588ae:	c2058593          	addi	a1,a1,-992 # 20004c20 <__kr4_start_table_backup_addr__>
200588b2:	02050513          	addi	a0,a0,32 # 20040020 <__kr4_boot_entry_addr__>
200588b6:	c606                	c.swsp	ra,12(sp)
200588b8:	3de9                	c.jal	20058792 <SOCPS_memswap>
200588ba:	4100d7b7          	lui	a5,0x4100d
200588be:	f40783a3          	sb	zero,-185(a5) # 4100cf47 <__km4_bd_boot_download_addr__+0x10ffaf47>
200588c2:	40b2                	c.lwsp	ra,12(sp)
200588c4:	0141                	c.addi	sp,16
200588c6:	8082                	c.jr	ra

200588c8 <BOOT_SectionInit>:
200588c8:	20058537          	lui	a0,0x20058
200588cc:	04050513          	addi	a0,a0,64 # 20058040 <Img2EntryFun0>
200588d0:	8082                	c.jr	ra

200588d2 <BOOT_SetupFPU>:
200588d2:	301022f3          	csrrs	t0,misa,zero
200588d6:	6341                	c.lui	t1,0x10
200588d8:	02830313          	addi	t1,t1,40 # 10028 <mbedtls_ecp_check_pub_priv+0x14a>
200588dc:	0062f2b3          	and	t0,t0,t1
200588e0:	00028963          	beq	t0,zero,200588f2 <BOOT_SetupFPU+0x20>
200588e4:	300022f3          	csrrs	t0,mstatus,zero
200588e8:	6309                	c.lui	t1,0x2
200588ea:	0062e2b3          	or	t0,t0,t1
200588ee:	30029073          	csrrw	zero,mstatus,t0
200588f2:	8082                	c.jr	ra

200588f4 <BOOT_WakeFromPG>:
200588f4:	1141                	c.addi	sp,-16
200588f6:	c606                	c.swsp	ra,12(sp)
200588f8:	4100c7b7          	lui	a5,0x4100c
200588fc:	22c7a783          	lw	a5,556(a5) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
20058900:	8b89                	c.andi	a5,2
20058902:	cf99                	c.beqz	a5,20058920 <BOOT_WakeFromPG+0x2c>
20058904:	dffae097          	auipc	ra,0xdffae
20058908:	2d4080e7          	jalr	ra,724(ra) # 6bd8 <SYSCFG_OTP_FlashDSleepEn>
2005890c:	c909                	c.beqz	a0,2005891e <BOOT_WakeFromPG+0x2a>
2005890e:	4501                	c.li	a0,0
20058910:	dffab097          	auipc	ra,0xdffab
20058914:	98e080e7          	jalr	ra,-1650(ra) # 329e <FLASH_DeepPowerDown>
20058918:	080007b7          	lui	a5,0x8000
2005891c:	439c                	c.lw	a5,0(a5)
2005891e:	3d0d                	c.jal	20058750 <set_psram_resume>
20058920:	200587b7          	lui	a5,0x20058
20058924:	04078793          	addi	a5,a5,64 # 20058040 <Img2EntryFun0>
20058928:	0057c703          	lbu	a4,5(a5)
2005892c:	0047c683          	lbu	a3,4(a5)
20058930:	40b2                	c.lwsp	ra,12(sp)
20058932:	0722                	c.slli	a4,0x8
20058934:	8ed9                	c.or	a3,a4
20058936:	0067c703          	lbu	a4,6(a5)
2005893a:	0077c783          	lbu	a5,7(a5)
2005893e:	0742                	c.slli	a4,0x10
20058940:	8f55                	c.or	a4,a3
20058942:	07e2                	c.slli	a5,0x18
20058944:	8fd9                	c.or	a5,a4
20058946:	0141                	c.addi	sp,16
20058948:	8782                	c.jr	a5

2005894a <BOOT_CPU_init>:
2005894a:	1141                	c.addi	sp,-16
2005894c:	c606                	c.swsp	ra,12(sp)
2005894e:	0ff0000f          	fence	iorw,iorw
20058952:	0001                	c.addi	zero,0
20058954:	0001                	c.addi	zero,0
20058956:	0001                	c.addi	zero,0
20058958:	0001                	c.addi	zero,0
2005895a:	0001                	c.addi	zero,0
2005895c:	0001                	c.addi	zero,0
2005895e:	0001                	c.addi	zero,0
20058960:	0001                	c.addi	zero,0
20058962:	0001                	c.addi	zero,0
20058964:	0001                	c.addi	zero,0
20058966:	0001                	c.addi	zero,0
20058968:	888827b7          	lui	a5,0x88882
2005896c:	81278793          	addi	a5,a5,-2030 # 88881812 <__ctrace_end__+0xbe881812>
20058970:	7c37a7f3          	csrrs	a5,0x7c3,a5
20058974:	0ff0000f          	fence	iorw,iorw
20058978:	0001                	c.addi	zero,0
2005897a:	0001                	c.addi	zero,0
2005897c:	0001                	c.addi	zero,0
2005897e:	0001                	c.addi	zero,0
20058980:	0001                	c.addi	zero,0
20058982:	0001                	c.addi	zero,0
20058984:	0001                	c.addi	zero,0
20058986:	0001                	c.addi	zero,0
20058988:	0001                	c.addi	zero,0
2005898a:	0001                	c.addi	zero,0
2005898c:	0001                	c.addi	zero,0
2005898e:	7c1027f3          	csrrs	a5,0x7c1,zero
20058992:	1007f793          	andi	a5,a5,256
20058996:	e7b1                	c.bnez	a5,200589e2 <BOOT_CPU_init+0x98>
20058998:	7c1027f3          	csrrs	a5,0x7c1,zero
2005899c:	1007f793          	andi	a5,a5,256
200589a0:	e3a9                	c.bnez	a5,200589e2 <BOOT_CPU_init+0x98>
200589a2:	7c0167f3          	csrrsi	a5,0x7c0,2
200589a6:	0ff0000f          	fence	iorw,iorw
200589aa:	0001                	c.addi	zero,0
200589ac:	0001                	c.addi	zero,0
200589ae:	0001                	c.addi	zero,0
200589b0:	0001                	c.addi	zero,0
200589b2:	0001                	c.addi	zero,0
200589b4:	0001                	c.addi	zero,0
200589b6:	0001                	c.addi	zero,0
200589b8:	0001                	c.addi	zero,0
200589ba:	0001                	c.addi	zero,0
200589bc:	0001                	c.addi	zero,0
200589be:	0001                	c.addi	zero,0
200589c0:	10000793          	addi	a5,zero,256
200589c4:	7c17a7f3          	csrrs	a5,0x7c1,a5
200589c8:	0ff0000f          	fence	iorw,iorw
200589cc:	0001                	c.addi	zero,0
200589ce:	0001                	c.addi	zero,0
200589d0:	0001                	c.addi	zero,0
200589d2:	0001                	c.addi	zero,0
200589d4:	0001                	c.addi	zero,0
200589d6:	0001                	c.addi	zero,0
200589d8:	0001                	c.addi	zero,0
200589da:	0001                	c.addi	zero,0
200589dc:	0001                	c.addi	zero,0
200589de:	0001                	c.addi	zero,0
200589e0:	0001                	c.addi	zero,0
200589e2:	e7eff0ef          	jal	ra,20058060 <BOOT_InitGP>
200589e6:	35f5                	c.jal	200588d2 <BOOT_SetupFPU>
200589e8:	0ff0000f          	fence	iorw,iorw
200589ec:	3569                	c.jal	20058876 <SOCPS_Kr4_StartTbl_Backup>
200589ee:	4100c7b7          	lui	a5,0x4100c
200589f2:	26c7a783          	lw	a5,620(a5) # 4100c26c <__km4_bd_boot_download_addr__+0x10ffa26c>
200589f6:	01000737          	lui	a4,0x1000
200589fa:	8ff9                	c.and	a5,a4
200589fc:	c781                	c.beqz	a5,20058a04 <BOOT_CPU_init+0xba>
200589fe:	40b2                	c.lwsp	ra,12(sp)
20058a00:	0141                	c.addi	sp,16
20058a02:	bdcd                	c.j	200588f4 <BOOT_WakeFromPG>
20058a04:	40b2                	c.lwsp	ra,12(sp)
20058a06:	0141                	c.addi	sp,16
20058a08:	4f30106f          	jal	zero,2005a6fa <BOOT_Image1>

20058a0c <app_start>:
20058a0c:	1141                	c.addi	sp,-16
20058a0e:	c606                	c.swsp	ra,12(sp)
20058a10:	e50ff0ef          	jal	ra,20058060 <BOOT_InitGP>
20058a14:	00018793          	addi	a5,gp,0 # 200660d8 <__global_pointer$>
20058a18:	ea818613          	addi	a2,gp,-344 # 20065f80 <PrevIrqStatus>
20058a1c:	40c78633          	sub	a2,a5,a2
20058a20:	4581                	c.li	a1,0
20058a22:	ea818513          	addi	a0,gp,-344 # 20065f80 <PrevIrqStatus>
20058a26:	dffb1097          	auipc	ra,0xdffb1
20058a2a:	d64080e7          	jalr	ra,-668(ra) # 978a <_memset>
20058a2e:	200687b7          	lui	a5,0x20068
20058a32:	02818613          	addi	a2,gp,40 # 20066100 <ipc_Semaphore>
20058a36:	10078793          	addi	a5,a5,256 # 20068100 <__bdram_heap_buffer_start__>
20058a3a:	40c78633          	sub	a2,a5,a2
20058a3e:	4581                	c.li	a1,0
20058a40:	02818513          	addi	a0,gp,40 # 20066100 <ipc_Semaphore>
20058a44:	dffb1097          	auipc	ra,0xdffb1
20058a48:	d46080e7          	jalr	ra,-698(ra) # 978a <_memset>
20058a4c:	6ad050ef          	jal	ra,2005e8f8 <ChipInfo_GetChipSram>
20058a50:	c919                	c.beqz	a0,20058a66 <app_start+0x5a>
20058a52:	000405b7          	lui	a1,0x40
20058a56:	20080537          	lui	a0,0x20080
20058a5a:	00058593          	addi	a1,a1,0 # 40000 <__ap_sram_heap_size>
20058a5e:	00050513          	addi	a0,a0,0 # 20080000 <__ap_sram_heap_start>
20058a62:	5f9010ef          	jal	ra,2005a85a <os_heap_add>
20058a66:	4d7020ef          	jal	ra,2005b73c <rtos_mem_init>
20058a6a:	2005c7b7          	lui	a5,0x2005c
20058a6e:	c1878793          	addi	a5,a5,-1000 # 2005bc18 <freertos_risc_v_trap_handler>
20058a72:	9bf1                	c.andi	a5,-4
20058a74:	30579073          	csrrw	zero,mtvec,a5
20058a78:	2831                	c.jal	20058a94 <SystemCoreClockUpdate>
20058a7a:	dffae097          	auipc	ra,0xdffae
20058a7e:	24c080e7          	jalr	ra,588(ra) # 6cc6 <SYSTIMER_Init>
20058a82:	2db000ef          	jal	ra,2005955c <pinmap_init>
20058a86:	20b080ef          	jal	ra,20061490 <__libc_init_array>
20058a8a:	40b2                	c.lwsp	ra,12(sp)
20058a8c:	0141                	c.addi	sp,16
20058a8e:	0f50206f          	jal	zero,2005b382 <main>

20058a92 <_init>:
20058a92:	8082                	c.jr	ra

20058a94 <SystemCoreClockUpdate>:
20058a94:	1141                	c.addi	sp,-16
20058a96:	c606                	c.swsp	ra,12(sp)
20058a98:	dffa8097          	auipc	ra,0xdffa8
20058a9c:	07e080e7          	jalr	ra,126(ra) # b16 <CPU_ClkGet>
20058aa0:	40b2                	c.lwsp	ra,12(sp)
20058aa2:	e6a1ac23          	sw	a0,-392(gp) # 20065f50 <SystemCoreClock>
20058aa6:	0141                	c.addi	sp,16
20058aa8:	8082                	c.jr	ra

20058aaa <Systick_Cmd>:
20058aaa:	cd01                	c.beqz	a0,20058ac2 <Systick_Cmd+0x18>
20058aac:	4100c737          	lui	a4,0x4100c
20058ab0:	22c72783          	lw	a5,556(a4) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
20058ab4:	6689                	c.lui	a3,0x2
20058ab6:	8fd5                	c.or	a5,a3
20058ab8:	22f72623          	sw	a5,556(a4)
20058abc:	0ff0000f          	fence	iorw,iorw
20058ac0:	8082                	c.jr	ra
20058ac2:	4100c6b7          	lui	a3,0x4100c
20058ac6:	22c6a783          	lw	a5,556(a3) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
20058aca:	7779                	c.lui	a4,0xffffe
20058acc:	177d                	c.addi	a4,-1
20058ace:	8ff9                	c.and	a5,a4
20058ad0:	22f6a623          	sw	a5,556(a3)
20058ad4:	b7e5                	c.j	20058abc <Systick_Cmd+0x12>

20058ad6 <Systick_State>:
20058ad6:	4100c7b7          	lui	a5,0x4100c
20058ada:	22c7a503          	lw	a0,556(a5) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
20058ade:	8135                	c.srli	a0,0xd
20058ae0:	8905                	c.andi	a0,1
20058ae2:	8082                	c.jr	ra

20058ae4 <CPU_InInterrupt>:
20058ae4:	1141                	c.addi	sp,-16
20058ae6:	c606                	c.swsp	ra,12(sp)
20058ae8:	1ca0d0ef          	jal	ra,20065cb2 <plic_get_active_irq_id>
20058aec:	40b2                	c.lwsp	ra,12(sp)
20058aee:	00a03533          	sltu	a0,zero,a0
20058af2:	0141                	c.addi	sp,16
20058af4:	8082                	c.jr	ra

20058af6 <irq_disable_save>:
20058af6:	30047573          	csrrci	a0,mstatus,8
20058afa:	8921                	c.andi	a0,8
20058afc:	8082                	c.jr	ra

20058afe <irq_enable_restore>:
20058afe:	30052573          	csrrs	a0,mstatus,a0
20058b02:	8082                	c.jr	ra

20058b04 <flash_get_layout_info>:
20058b04:	c2818693          	addi	a3,gp,-984 # 20065d00 <Flash_Layout>
20058b08:	4701                	c.li	a4,0
20058b0a:	c2818793          	addi	a5,gp,-984 # 20065d00 <Flash_Layout>
20058b0e:	0ff00893          	addi	a7,zero,255
20058b12:	0006a803          	lw	a6,0(a3)
20058b16:	01181363          	bne	a6,a7,20058b1c <flash_get_layout_info+0x18>
20058b1a:	8082                	c.jr	ra
20058b1c:	06b1                	c.addi	a3,12
20058b1e:	02a81163          	bne	a6,a0,20058b40 <flash_get_layout_info+0x3c>
20058b22:	c599                	c.beqz	a1,20058b30 <flash_get_layout_info+0x2c>
20058b24:	46b1                	c.li	a3,12
20058b26:	02d706b3          	mul	a3,a4,a3
20058b2a:	96be                	c.add	a3,a5
20058b2c:	42d4                	c.lw	a3,4(a3)
20058b2e:	c194                	c.sw	a3,0(a1)
20058b30:	d66d                	c.beqz	a2,20058b1a <flash_get_layout_info+0x16>
20058b32:	46b1                	c.li	a3,12
20058b34:	02d70733          	mul	a4,a4,a3
20058b38:	97ba                	c.add	a5,a4
20058b3a:	479c                	c.lw	a5,8(a5)
20058b3c:	c21c                	c.sw	a5,0(a2)
20058b3e:	8082                	c.jr	ra
20058b40:	0705                	c.addi	a4,1
20058b42:	bfc1                	c.j	20058b12 <flash_get_layout_info+0xe>

20058b44 <FLASH_Write_Lock>:
20058b44:	1141                	c.addi	sp,-16
20058b46:	c606                	c.swsp	ra,12(sp)
20058b48:	377d                	c.jal	20058af6 <irq_disable_save>
20058b4a:	eaa1a423          	sw	a0,-344(gp) # 20065f80 <PrevIrqStatus>
20058b4e:	55fd                	c.li	a1,-1
20058b50:	4505                	c.li	a0,1
20058b52:	25c1                	c.jal	20059212 <IPC_SEMTake>
20058b54:	dd6d                	c.beqz	a0,20058b4e <FLASH_Write_Lock+0xa>
20058b56:	40b2                	c.lwsp	ra,12(sp)
20058b58:	0141                	c.addi	sp,16
20058b5a:	8082                	c.jr	ra

20058b5c <FLASH_Write_Unlock>:
20058b5c:	1141                	c.addi	sp,-16
20058b5e:	c606                	c.swsp	ra,12(sp)
20058b60:	4505                	c.li	a0,1
20058b62:	79e000ef          	jal	ra,20059300 <IPC_SEMFree>
20058b66:	dd6d                	c.beqz	a0,20058b60 <FLASH_Write_Unlock+0x4>
20058b68:	40b2                	c.lwsp	ra,12(sp)
20058b6a:	ea81a503          	lw	a0,-344(gp) # 20065f80 <PrevIrqStatus>
20058b6e:	0141                	c.addi	sp,16
20058b70:	b779                	c.j	20058afe <irq_enable_restore>

20058b72 <FLASH_ReadStream>:
20058b72:	1141                	c.addi	sp,-16
20058b74:	c422                	c.swsp	s0,8(sp)
20058b76:	c226                	c.swsp	s1,4(sp)
20058b78:	c04a                	c.swsp	s2,0(sp)
20058b7a:	c606                	c.swsp	ra,12(sp)
20058b7c:	84aa                	c.mv	s1,a0
20058b7e:	892e                	c.mv	s2,a1
20058b80:	8432                	c.mv	s0,a2
20058b82:	ea09                	c.bnez	a2,20058b94 <FLASH_ReadStream+0x22>
20058b84:	20062537          	lui	a0,0x20062
20058b88:	0c200593          	addi	a1,zero,194
20058b8c:	88c50513          	addi	a0,a0,-1908 # 2006188c <strtol+0xe>
20058b90:	1af060ef          	jal	ra,2005f53e <io_assert_failed>
20058b94:	3f45                	c.jal	20058b44 <FLASH_Write_Lock>
20058b96:	080005b7          	lui	a1,0x8000
20058b9a:	864a                	c.mv	a2,s2
20058b9c:	95a6                	c.add	a1,s1
20058b9e:	8522                	c.mv	a0,s0
20058ba0:	dffb1097          	auipc	ra,0xdffb1
20058ba4:	a72080e7          	jalr	ra,-1422(ra) # 9612 <_memcpy>
20058ba8:	3f55                	c.jal	20058b5c <FLASH_Write_Unlock>
20058baa:	40b2                	c.lwsp	ra,12(sp)
20058bac:	4422                	c.lwsp	s0,8(sp)
20058bae:	4492                	c.lwsp	s1,4(sp)
20058bb0:	4902                	c.lwsp	s2,0(sp)
20058bb2:	4505                	c.li	a0,1
20058bb4:	0141                	c.addi	sp,16
20058bb6:	8082                	c.jr	ra

20058bb8 <FLASH_WriteStream>:
20058bb8:	7179                	c.addi16sp	sp,-48
20058bba:	c85a                	c.swsp	s6,16(sp)
20058bbc:	00b50b33          	add	s6,a0,a1
20058bc0:	fffb0793          	addi	a5,s6,-1
20058bc4:	f0057713          	andi	a4,a0,-256
20058bc8:	f007f793          	andi	a5,a5,-256
20058bcc:	8f99                	c.sub	a5,a4
20058bce:	d422                	c.swsp	s0,40(sp)
20058bd0:	d226                	c.swsp	s1,36(sp)
20058bd2:	d04a                	c.swsp	s2,32(sp)
20058bd4:	ce4e                	c.swsp	s3,28(sp)
20058bd6:	cc52                	c.swsp	s4,24(sp)
20058bd8:	83a1                	c.srli	a5,0x8
20058bda:	d606                	c.swsp	ra,44(sp)
20058bdc:	ca56                	c.swsp	s5,20(sp)
20058bde:	c65e                	c.swsp	s7,12(sp)
20058be0:	842a                	c.mv	s0,a0
20058be2:	84ae                	c.mv	s1,a1
20058be4:	89b2                	c.mv	s3,a2
20058be6:	00178a13          	addi	s4,a5,1
20058bea:	10070913          	addi	s2,a4,256 # ffffe100 <__ctrace_end__+0x35ffe100>
20058bee:	e391                	c.bnez	a5,20058bf2 <FLASH_WriteStream+0x3a>
20058bf0:	895a                	c.mv	s2,s6
20058bf2:	3f89                	c.jal	20058b44 <FLASH_Write_Lock>
20058bf4:	40890ab3          	sub	s5,s2,s0
20058bf8:	8522                	c.mv	a0,s0
20058bfa:	4b85                	c.li	s7,1
20058bfc:	864e                	c.mv	a2,s3
20058bfe:	85d6                	c.mv	a1,s5
20058c00:	4f30c0ef          	jal	ra,200658f2 <FLASH_TxData>
20058c04:	1a7d                	c.addi	s4,-1
20058c06:	99d6                	c.add	s3,s5
20058c08:	87da                	c.mv	a5,s6
20058c0a:	017a0463          	beq	s4,s7,20058c12 <FLASH_WriteStream+0x5a>
20058c0e:	10090793          	addi	a5,s2,256
20058c12:	41278ab3          	sub	s5,a5,s2
20058c16:	854a                	c.mv	a0,s2
20058c18:	040a1b63          	bne	s4,zero,20058c6e <FLASH_WriteStream+0xb6>
20058c1c:	7c1027f3          	csrrs	a5,0x7c1,zero
20058c20:	1007f793          	andi	a5,a5,256
20058c24:	cb85                	c.beqz	a5,20058c54 <FLASH_WriteStream+0x9c>
20058c26:	08000537          	lui	a0,0x8000
20058c2a:	9522                	c.add	a0,s0
20058c2c:	57fd                	c.li	a5,-1
20058c2e:	04f51263          	bne	a0,a5,20058c72 <FLASH_WriteStream+0xba>
20058c32:	04a49063          	bne	s1,a0,20058c72 <FLASH_WriteStream+0xba>
20058c36:	7c0167f3          	csrrsi	a5,0x7c0,2
20058c3a:	0ff0000f          	fence	iorw,iorw
20058c3e:	0001                	c.addi	zero,0
20058c40:	0001                	c.addi	zero,0
20058c42:	0001                	c.addi	zero,0
20058c44:	0001                	c.addi	zero,0
20058c46:	0001                	c.addi	zero,0
20058c48:	0001                	c.addi	zero,0
20058c4a:	0001                	c.addi	zero,0
20058c4c:	0001                	c.addi	zero,0
20058c4e:	0001                	c.addi	zero,0
20058c50:	0001                	c.addi	zero,0
20058c52:	0001                	c.addi	zero,0
20058c54:	3721                	c.jal	20058b5c <FLASH_Write_Unlock>
20058c56:	50b2                	c.lwsp	ra,44(sp)
20058c58:	5422                	c.lwsp	s0,40(sp)
20058c5a:	5492                	c.lwsp	s1,36(sp)
20058c5c:	5902                	c.lwsp	s2,32(sp)
20058c5e:	49f2                	c.lwsp	s3,28(sp)
20058c60:	4a62                	c.lwsp	s4,24(sp)
20058c62:	4ad2                	c.lwsp	s5,20(sp)
20058c64:	4b42                	c.lwsp	s6,16(sp)
20058c66:	4bb2                	c.lwsp	s7,12(sp)
20058c68:	4505                	c.li	a0,1
20058c6a:	6145                	c.addi16sp	sp,48
20058c6c:	8082                	c.jr	ra
20058c6e:	893e                	c.mv	s2,a5
20058c70:	b771                	c.j	20058bfc <FLASH_WriteStream+0x44>
20058c72:	01f57793          	andi	a5,a0,31
20058c76:	c399                	c.beqz	a5,20058c7c <FLASH_WriteStream+0xc4>
20058c78:	9901                	c.andi	a0,-32
20058c7a:	94be                	c.add	s1,a5
20058c7c:	fc905ce3          	bge	zero,s1,20058c54 <FLASH_WriteStream+0x9c>
20058c80:	01f57593          	andi	a1,a0,31
20058c84:	94ae                	c.add	s1,a1
20058c86:	0ff0000f          	fence	iorw,iorw
20058c8a:	94aa                	c.add	s1,a0
20058c8c:	00a5700b          	cache	dinv,(a0)
20058c90:	02050513          	addi	a0,a0,32 # 8000020 <__ap_sram_heap_size+0x7fc0020>
20058c94:	40a487b3          	sub	a5,s1,a0
20058c98:	fef04ae3          	blt	zero,a5,20058c8c <FLASH_WriteStream+0xd4>
20058c9c:	bf79                	c.j	20058c3a <FLASH_WriteStream+0x82>

20058c9e <FLASH_EraseXIP>:
20058c9e:	1141                	c.addi	sp,-16
20058ca0:	c606                	c.swsp	ra,12(sp)
20058ca2:	c422                	c.swsp	s0,8(sp)
20058ca4:	c226                	c.swsp	s1,4(sp)
20058ca6:	842e                	c.mv	s0,a1
20058ca8:	84aa                	c.mv	s1,a0
20058caa:	3d69                	c.jal	20058b44 <FLASH_Write_Lock>
20058cac:	85a2                	c.mv	a1,s0
20058cae:	8526                	c.mv	a0,s1
20058cb0:	63d0c0ef          	jal	ra,20065aec <FLASH_Erase>
20058cb4:	4789                	c.li	a5,2
20058cb6:	04f49f63          	bne	s1,a5,20058d14 <FLASH_EraseXIP+0x76>
20058cba:	7c1027f3          	csrrs	a5,0x7c1,zero
20058cbe:	1007f793          	andi	a5,a5,256
20058cc2:	c7a1                	c.beqz	a5,20058d0a <FLASH_EraseXIP+0x6c>
20058cc4:	080007b7          	lui	a5,0x8000
20058cc8:	97a2                	c.add	a5,s0
20058cca:	887d                	c.andi	s0,31
20058ccc:	6685                	c.lui	a3,0x1
20058cce:	c019                	c.beqz	s0,20058cd4 <FLASH_EraseXIP+0x36>
20058cd0:	96a2                	c.add	a3,s0
20058cd2:	9b81                	c.andi	a5,-32
20058cd4:	01f7f713          	andi	a4,a5,31
20058cd8:	9736                	c.add	a4,a3
20058cda:	0ff0000f          	fence	iorw,iorw
20058cde:	973e                	c.add	a4,a5
20058ce0:	00f7f00b          	cache	dinv,(a5)
20058ce4:	02078793          	addi	a5,a5,32 # 8000020 <__ap_sram_heap_size+0x7fc0020>
20058ce8:	40f706b3          	sub	a3,a4,a5
20058cec:	fed04ae3          	blt	zero,a3,20058ce0 <FLASH_EraseXIP+0x42>
20058cf0:	0ff0000f          	fence	iorw,iorw
20058cf4:	0001                	c.addi	zero,0
20058cf6:	0001                	c.addi	zero,0
20058cf8:	0001                	c.addi	zero,0
20058cfa:	0001                	c.addi	zero,0
20058cfc:	0001                	c.addi	zero,0
20058cfe:	0001                	c.addi	zero,0
20058d00:	0001                	c.addi	zero,0
20058d02:	0001                	c.addi	zero,0
20058d04:	0001                	c.addi	zero,0
20058d06:	0001                	c.addi	zero,0
20058d08:	0001                	c.addi	zero,0
20058d0a:	4422                	c.lwsp	s0,8(sp)
20058d0c:	40b2                	c.lwsp	ra,12(sp)
20058d0e:	4492                	c.lwsp	s1,4(sp)
20058d10:	0141                	c.addi	sp,16
20058d12:	b5a9                	c.j	20058b5c <FLASH_Write_Unlock>
20058d14:	4785                	c.li	a5,1
20058d16:	02f49e63          	bne	s1,a5,20058d52 <FLASH_EraseXIP+0xb4>
20058d1a:	7c1027f3          	csrrs	a5,0x7c1,zero
20058d1e:	1007f793          	andi	a5,a5,256
20058d22:	d7e5                	c.beqz	a5,20058d0a <FLASH_EraseXIP+0x6c>
20058d24:	080007b7          	lui	a5,0x8000
20058d28:	97a2                	c.add	a5,s0
20058d2a:	887d                	c.andi	s0,31
20058d2c:	66c1                	c.lui	a3,0x10
20058d2e:	c019                	c.beqz	s0,20058d34 <FLASH_EraseXIP+0x96>
20058d30:	96a2                	c.add	a3,s0
20058d32:	9b81                	c.andi	a5,-32
20058d34:	01f7f713          	andi	a4,a5,31
20058d38:	9736                	c.add	a4,a3
20058d3a:	0ff0000f          	fence	iorw,iorw
20058d3e:	973e                	c.add	a4,a5
20058d40:	00f7f00b          	cache	dinv,(a5)
20058d44:	02078793          	addi	a5,a5,32 # 8000020 <__ap_sram_heap_size+0x7fc0020>
20058d48:	40f706b3          	sub	a3,a4,a5
20058d4c:	fed04ae3          	blt	zero,a3,20058d40 <FLASH_EraseXIP+0xa2>
20058d50:	b745                	c.j	20058cf0 <FLASH_EraseXIP+0x52>
20058d52:	7c1027f3          	csrrs	a5,0x7c1,zero
20058d56:	1007f793          	andi	a5,a5,256
20058d5a:	dbc5                	c.beqz	a5,20058d0a <FLASH_EraseXIP+0x6c>
20058d5c:	0ff0000f          	fence	iorw,iorw
20058d60:	080007b7          	lui	a5,0x8000
20058d64:	10000737          	lui	a4,0x10000
20058d68:	00f7f00b          	cache	dinv,(a5)
20058d6c:	02078793          	addi	a5,a5,32 # 8000020 <__ap_sram_heap_size+0x7fc0020>
20058d70:	fee79ce3          	bne	a5,a4,20058d68 <FLASH_EraseXIP+0xca>
20058d74:	bfb5                	c.j	20058cf0 <FLASH_EraseXIP+0x52>

20058d76 <ipc_table_init>:
20058d76:	715d                	c.addi16sp	sp,-80
20058d78:	c2a6                	c.swsp	s1,68(sp)
20058d7a:	ce6e                	c.swsp	s11,28(sp)
20058d7c:	200644b7          	lui	s1,0x20064
20058d80:	20064db7          	lui	s11,0x20064
20058d84:	3ccd8793          	addi	a5,s11,972 # 200643cc <ipc_tickless_table>
20058d88:	47448493          	addi	s1,s1,1140 # 20064474 <lib_pmc_git_rev>
20058d8c:	8c9d                	c.sub	s1,a5
20058d8e:	47f1                	c.li	a5,28
20058d90:	02f4d4b3          	divu	s1,s1,a5
20058d94:	dc52                	c.swsp	s4,56(sp)
20058d96:	41020a37          	lui	s4,0x41020
20058d9a:	c4a2                	c.swsp	s0,72(sp)
20058d9c:	c0ca                	c.swsp	s2,64(sp)
20058d9e:	de4e                	c.swsp	s3,60(sp)
20058da0:	da56                	c.swsp	s5,52(sp)
20058da2:	d85a                	c.swsp	s6,48(sp)
20058da4:	d65e                	c.swsp	s7,44(sp)
20058da6:	d462                	c.swsp	s8,40(sp)
20058da8:	d266                	c.swsp	s9,36(sp)
20058daa:	d06a                	c.swsp	s10,32(sp)
20058dac:	c686                	c.swsp	ra,76(sp)
20058dae:	842a                	c.mv	s0,a0
20058db0:	3ccd8d93          	addi	s11,s11,972
20058db4:	4901                	c.li	s2,0
20058db6:	4985                	c.li	s3,1
20058db8:	4a89                	c.li	s5,2
20058dba:	4100cb37          	lui	s6,0x4100c
20058dbe:	20062bb7          	lui	s7,0x20062
20058dc2:	20062c37          	lui	s8,0x20062
20058dc6:	080a0c93          	addi	s9,s4,128 # 41020080 <__km4_bd_boot_download_addr__+0x1100e080>
20058dca:	100a0d13          	addi	s10,s4,256
20058dce:	02991163          	bne	s2,s1,20058df0 <ipc_table_init+0x7a>
20058dd2:	40b6                	c.lwsp	ra,76(sp)
20058dd4:	4426                	c.lwsp	s0,72(sp)
20058dd6:	4496                	c.lwsp	s1,68(sp)
20058dd8:	4906                	c.lwsp	s2,64(sp)
20058dda:	59f2                	c.lwsp	s3,60(sp)
20058ddc:	5a62                	c.lwsp	s4,56(sp)
20058dde:	5ad2                	c.lwsp	s5,52(sp)
20058de0:	5b42                	c.lwsp	s6,48(sp)
20058de2:	5bb2                	c.lwsp	s7,44(sp)
20058de4:	5c22                	c.lwsp	s8,40(sp)
20058de6:	5c92                	c.lwsp	s9,36(sp)
20058de8:	5d02                	c.lwsp	s10,32(sp)
20058dea:	4df2                	c.lwsp	s11,28(sp)
20058dec:	6161                	c.addi16sp	sp,80
20058dee:	8082                	c.jr	ra
20058df0:	014da683          	lw	a3,20(s11)
20058df4:	018da583          	lw	a1,24(s11)
20058df8:	c689                	c.beqz	a3,20058e02 <ipc_table_init+0x8c>
20058dfa:	02100793          	addi	a5,zero,33
20058dfe:	00f69d63          	bne	a3,a5,20058e18 <ipc_table_init+0xa2>
20058e02:	fef6f713          	andi	a4,a3,-17
20058e06:	07941663          	bne	s0,s9,20058e72 <ipc_table_init+0xfc>
20058e0a:	c305                	c.beqz	a4,20058e2a <ipc_table_init+0xb4>
20058e0c:	4709                	c.li	a4,2
20058e0e:	47c1                	c.li	a5,16
20058e10:	03369663          	bne	a3,s3,20058e3c <ipc_table_init+0xc6>
20058e14:	47e1                	c.li	a5,24
20058e16:	a01d                	c.j	20058e3c <ipc_table_init+0xc6>
20058e18:	ff068713          	addi	a4,a3,-16 # fff0 <mbedtls_ecp_check_pub_priv+0x112>
20058e1c:	9b3d                	c.andi	a4,-17
20058e1e:	eb01                	c.bnez	a4,20058e2e <ipc_table_init+0xb8>
20058e20:	09441163          	bne	s0,s4,20058ea2 <ipc_table_init+0x12c>
20058e24:	47c1                	c.li	a5,16
20058e26:	fef693e3          	bne	a3,a5,20058e0c <ipc_table_init+0x96>
20058e2a:	4709                	c.li	a4,2
20058e2c:	b7e5                	c.j	20058e14 <ipc_table_init+0x9e>
20058e2e:	fef6f713          	andi	a4,a3,-17
20058e32:	03371d63          	bne	a4,s3,20058e6c <ipc_table_init+0xf6>
20058e36:	fda40be3          	beq	s0,s10,20058e0c <ipc_table_init+0x96>
20058e3a:	4781                	c.li	a5,0
20058e3c:	97ae                	c.add	a5,a1
20058e3e:	8522                	c.mv	a0,s0
20058e40:	c63a                	c.swsp	a4,12(sp)
20058e42:	c43e                	c.swsp	a5,8(sp)
20058e44:	2e1d                	c.jal	2005917a <IPC_IERGet>
20058e46:	47a2                	c.lwsp	a5,8(sp)
20058e48:	4732                	c.lwsp	a4,12(sp)
20058e4a:	00f996b3          	sll	a3,s3,a5
20058e4e:	8ee9                	c.and	a3,a0
20058e50:	ca8d                	c.beqz	a3,20058e82 <ipc_table_init+0x10c>
20058e52:	228b2703          	lw	a4,552(s6) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
20058e56:	934b8693          	addi	a3,s7,-1740 # 20061934 <strtol+0xb6>
20058e5a:	04900613          	addi	a2,zero,73
20058e5e:	8319                	c.srli	a4,0x6
20058e60:	8b0d                	c.andi	a4,3
20058e62:	97cc0593          	addi	a1,s8,-1668 # 2006197c <strtol+0xfe>
20058e66:	4511                	c.li	a0,4
20058e68:	008020ef          	jal	ra,2005ae70 <rtk_log_write>
20058e6c:	0905                	c.addi	s2,1
20058e6e:	0df1                	c.addi	s11,28
20058e70:	bfb9                	c.j	20058dce <ipc_table_init+0x58>
20058e72:	cb1d                	c.beqz	a4,20058ea8 <ipc_table_init+0x132>
20058e74:	02000793          	addi	a5,zero,32
20058e78:	4705                	c.li	a4,1
20058e7a:	fcf690e3          	bne	a3,a5,20058e3a <ipc_table_init+0xc4>
20058e7e:	47a1                	c.li	a5,8
20058e80:	bf75                	c.j	20058e3c <ipc_table_init+0xc6>
20058e82:	0ff7f593          	andi	a1,a5,255
20058e86:	01571963          	bne	a4,s5,20058e98 <ipc_table_init+0x122>
20058e8a:	008da683          	lw	a3,8(s11)
20058e8e:	004da603          	lw	a2,4(s11)
20058e92:	8522                	c.mv	a0,s0
20058e94:	2e89                	c.jal	200591e6 <IPC_INTUserHandler>
20058e96:	bfd9                	c.j	20058e6c <ipc_table_init+0xf6>
20058e98:	010da683          	lw	a3,16(s11)
20058e9c:	00cda603          	lw	a2,12(s11)
20058ea0:	bfcd                	c.j	20058e92 <ipc_table_init+0x11c>
20058ea2:	47c1                	c.li	a5,16
20058ea4:	fcf698e3          	bne	a3,a5,20058e74 <ipc_table_init+0xfe>
20058ea8:	4705                	c.li	a4,1
20058eaa:	bfd1                	c.j	20058e7e <ipc_table_init+0x108>

20058eac <IPC_TXHandler>:
20058eac:	1141                	c.addi	sp,-16
20058eae:	c606                	c.swsp	ra,12(sp)
20058eb0:	c422                	c.swsp	s0,8(sp)
20058eb2:	4100c7b7          	lui	a5,0x4100c
20058eb6:	2287a503          	lw	a0,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
20058eba:	8432                	c.mv	s0,a2
20058ebc:	8119                	c.srli	a0,0x6
20058ebe:	890d                	c.andi	a0,3
20058ec0:	215d                	c.jal	20059366 <IPC_GetDevById>
20058ec2:	4601                	c.li	a2,0
20058ec4:	0ff47593          	andi	a1,s0,255
20058ec8:	2c59                	c.jal	2005915e <IPC_INTConfig>
20058eca:	040a                	c.slli	s0,0x2
20058ecc:	02818613          	addi	a2,gp,40 # 20066100 <ipc_Semaphore>
20058ed0:	9432                	c.add	s0,a2
20058ed2:	4008                	c.lw	a0,0(s0)
20058ed4:	c511                	c.beqz	a0,20058ee0 <IPC_TXHandler+0x34>
20058ed6:	4422                	c.lwsp	s0,8(sp)
20058ed8:	40b2                	c.lwsp	ra,12(sp)
20058eda:	0141                	c.addi	sp,16
20058edc:	1df0206f          	jal	zero,2005b8ba <rtos_sema_give>
20058ee0:	40b2                	c.lwsp	ra,12(sp)
20058ee2:	4422                	c.lwsp	s0,8(sp)
20058ee4:	0141                	c.addi	sp,16
20058ee6:	8082                	c.jr	ra

20058ee8 <IPC_wait_idle_NonOS>:
20058ee8:	4785                	c.li	a5,1
20058eea:	00989737          	lui	a4,0x989
20058eee:	86aa                	c.mv	a3,a0
20058ef0:	00b795b3          	sll	a1,a5,a1
20058ef4:	68070713          	addi	a4,a4,1664 # 989680 <__ap_sram_heap_size+0x949680>
20058ef8:	429c                	c.lw	a5,0(a3)
20058efa:	00b7f533          	and	a0,a5,a1
20058efe:	e111                	c.bnez	a0,20058f02 <IPC_wait_idle_NonOS+0x1a>
20058f00:	8082                	c.jr	ra
20058f02:	177d                	c.addi	a4,-1
20058f04:	fb75                	c.bnez	a4,20058ef8 <IPC_wait_idle_NonOS+0x10>
20058f06:	20062537          	lui	a0,0x20062
20058f0a:	1141                	c.addi	sp,-16
20058f0c:	98050513          	addi	a0,a0,-1664 # 20061980 <strtol+0x102>
20058f10:	c606                	c.swsp	ra,12(sp)
20058f12:	13c060ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20058f16:	40b2                	c.lwsp	ra,12(sp)
20058f18:	557d                	c.li	a0,-1
20058f1a:	0141                	c.addi	sp,16
20058f1c:	8082                	c.jr	ra

20058f1e <IPC_wait_idle>:
20058f1e:	1101                	c.addi	sp,-32
20058f20:	ca26                	c.swsp	s1,20(sp)
20058f22:	c84a                	c.swsp	s2,16(sp)
20058f24:	ce06                	c.swsp	ra,28(sp)
20058f26:	cc22                	c.swsp	s0,24(sp)
20058f28:	c64e                	c.swsp	s3,12(sp)
20058f2a:	c452                	c.swsp	s4,8(sp)
20058f2c:	84aa                	c.mv	s1,a0
20058f2e:	892e                	c.mv	s2,a1
20058f30:	3e55                	c.jal	20058ae4 <CPU_InInterrupt>
20058f32:	ed15                	c.bnez	a0,20058f6e <IPC_wait_idle+0x50>
20058f34:	00291993          	slli	s3,s2,0x2
20058f38:	0e818713          	addi	a4,gp,232 # 200661c0 <IPC_IrqHandler>
20058f3c:	974e                	c.add	a4,s3
20058f3e:	431c                	c.lw	a5,0(a4)
20058f40:	c79d                	c.beqz	a5,20058f6e <IPC_wait_idle+0x50>
20058f42:	02818513          	addi	a0,gp,40 # 20066100 <ipc_Semaphore>
20058f46:	954e                	c.add	a0,s3
20058f48:	4118                	c.lw	a4,0(a0)
20058f4a:	02818413          	addi	s0,gp,40 # 20066100 <ipc_Semaphore>
20058f4e:	e319                	c.bnez	a4,20058f54 <IPC_wait_idle+0x36>
20058f50:	0df020ef          	jal	ra,2005b82e <rtos_sema_create_binary>
20058f54:	0ff97a13          	andi	s4,s2,255
20058f58:	85d2                	c.mv	a1,s4
20058f5a:	8526                	c.mv	a0,s1
20058f5c:	4605                	c.li	a2,1
20058f5e:	2401                	c.jal	2005915e <IPC_INTConfig>
20058f60:	013407b3          	add	a5,s0,s3
20058f64:	4388                	c.lw	a0,0(a5)
20058f66:	55fd                	c.li	a1,-1
20058f68:	107020ef          	jal	ra,2005b86e <rtos_sema_take>
20058f6c:	e919                	c.bnez	a0,20058f82 <IPC_wait_idle+0x64>
20058f6e:	4462                	c.lwsp	s0,24(sp)
20058f70:	40f2                	c.lwsp	ra,28(sp)
20058f72:	49b2                	c.lwsp	s3,12(sp)
20058f74:	4a22                	c.lwsp	s4,8(sp)
20058f76:	85ca                	c.mv	a1,s2
20058f78:	8526                	c.mv	a0,s1
20058f7a:	4942                	c.lwsp	s2,16(sp)
20058f7c:	44d2                	c.lwsp	s1,20(sp)
20058f7e:	6105                	c.addi16sp	sp,32
20058f80:	b7a5                	c.j	20058ee8 <IPC_wait_idle_NonOS>
20058f82:	20062537          	lui	a0,0x20062
20058f86:	99850513          	addi	a0,a0,-1640 # 20061998 <strtol+0x11a>
20058f8a:	0c4060ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20058f8e:	8526                	c.mv	a0,s1
20058f90:	85d2                	c.mv	a1,s4
20058f92:	4601                	c.li	a2,0
20058f94:	22e9                	c.jal	2005915e <IPC_INTConfig>
20058f96:	40f2                	c.lwsp	ra,28(sp)
20058f98:	4462                	c.lwsp	s0,24(sp)
20058f9a:	44d2                	c.lwsp	s1,20(sp)
20058f9c:	4942                	c.lwsp	s2,16(sp)
20058f9e:	49b2                	c.lwsp	s3,12(sp)
20058fa0:	4a22                	c.lwsp	s4,8(sp)
20058fa2:	5579                	c.li	a0,-2
20058fa4:	6105                	c.addi16sp	sp,32
20058fa6:	8082                	c.jr	ra

20058fa8 <ipc_send_message>:
20058fa8:	1101                	c.addi	sp,-32
20058faa:	c64e                	c.swsp	s3,12(sp)
20058fac:	c452                	c.swsp	s4,8(sp)
20058fae:	c256                	c.swsp	s5,4(sp)
20058fb0:	ce06                	c.swsp	ra,28(sp)
20058fb2:	cc22                	c.swsp	s0,24(sp)
20058fb4:	ca26                	c.swsp	s1,20(sp)
20058fb6:	c84a                	c.swsp	s2,16(sp)
20058fb8:	fee57793          	andi	a5,a0,-18
20058fbc:	89aa                	c.mv	s3,a0
20058fbe:	8aae                	c.mv	s5,a1
20058fc0:	8a32                	c.mv	s4,a2
20058fc2:	cf91                	c.beqz	a5,20058fde <ipc_send_message+0x36>
20058fc4:	fe050793          	addi	a5,a0,-32
20058fc8:	4705                	c.li	a4,1
20058fca:	00f77a63          	bgeu	a4,a5,20058fde <ipc_send_message+0x36>
20058fce:	20062537          	lui	a0,0x20062
20058fd2:	0ac00593          	addi	a1,zero,172
20058fd6:	9b850513          	addi	a0,a0,-1608 # 200619b8 <strtol+0x13a>
20058fda:	564060ef          	jal	ra,2005f53e <io_assert_failed>
20058fde:	479d                	c.li	a5,7
20058fe0:	0157fa63          	bgeu	a5,s5,20058ff4 <ipc_send_message+0x4c>
20058fe4:	20062537          	lui	a0,0x20062
20058fe8:	0ad00593          	addi	a1,zero,173
20058fec:	9b850513          	addi	a0,a0,-1608 # 200619b8 <strtol+0x13a>
20058ff0:	54e060ef          	jal	ra,2005f53e <io_assert_failed>
20058ff4:	02100793          	addi	a5,zero,33
20058ff8:	0d37eb63          	bltu	a5,s3,200590ce <ipc_send_message+0x126>
20058ffc:	200627b7          	lui	a5,0x20062
20059000:	a8c78793          	addi	a5,a5,-1396 # 20061a8c <CSWTCH.29>
20059004:	97ce                	c.add	a5,s3
20059006:	20062737          	lui	a4,0x20062
2005900a:	0007c583          	lbu	a1,0(a5)
2005900e:	ab070713          	addi	a4,a4,-1360 # 20061ab0 <CSWTCH.30>
20059012:	00299793          	slli	a5,s3,0x2
20059016:	97ba                	c.add	a5,a4
20059018:	4384                	c.lw	s1,0(a5)
2005901a:	409c                	c.lw	a5,0(s1)
2005901c:	95d6                	c.add	a1,s5
2005901e:	4405                	c.li	s0,1
20059020:	00b41433          	sll	s0,s0,a1
20059024:	8fe1                	c.and	a5,s0
20059026:	e7dd                	c.bnez	a5,200590d4 <ipc_send_message+0x12c>
20059028:	34f9                	c.jal	20058af6 <irq_disable_save>
2005902a:	409c                	c.lw	a5,0(s1)
2005902c:	892a                	c.mv	s2,a0
2005902e:	8fe1                	c.and	a5,s0
20059030:	cb89                	c.beqz	a5,20059042 <ipc_send_message+0x9a>
20059032:	20062537          	lui	a0,0x20062
20059036:	8622                	c.mv	a2,s0
20059038:	85ce                	c.mv	a1,s3
2005903a:	a6050513          	addi	a0,a0,-1440 # 20061a60 <strtol+0x1e2>
2005903e:	010060ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059042:	080a0163          	beq	s4,zero,200590c4 <ipc_send_message+0x11c>
20059046:	0039d793          	srli	a5,s3,0x3
2005904a:	8bf9                	c.andi	a5,30
2005904c:	00f9f993          	andi	s3,s3,15
20059050:	97ce                	c.add	a5,s3
20059052:	078e                	c.slli	a5,0x3
20059054:	97d6                	c.add	a5,s5
20059056:	20005737          	lui	a4,0x20005
2005905a:	0792                	c.slli	a5,0x4
2005905c:	90070713          	addi	a4,a4,-1792 # 20004900 <__kr4_ipc_memory_start__>
20059060:	00e789b3          	add	s3,a5,a4
20059064:	4641                	c.li	a2,16
20059066:	85d2                	c.mv	a1,s4
20059068:	854e                	c.mv	a0,s3
2005906a:	dffb1097          	auipc	ra,0xdffb1
2005906e:	a66080e7          	jalr	ra,-1434(ra) # 9ad0 <__wrap_memcpy>
20059072:	7c102773          	csrrs	a4,0x7c1,zero
20059076:	10077713          	andi	a4,a4,256
2005907a:	c729                	c.beqz	a4,200590c4 <ipc_send_message+0x11c>
2005907c:	01f9f713          	andi	a4,s3,31
20059080:	87ce                	c.mv	a5,s3
20059082:	46c1                	c.li	a3,16
20059084:	c709                	c.beqz	a4,2005908e <ipc_send_message+0xe6>
20059086:	01070693          	addi	a3,a4,16
2005908a:	fe09f793          	andi	a5,s3,-32
2005908e:	01f7f713          	andi	a4,a5,31
20059092:	9736                	c.add	a4,a3
20059094:	0ff0000f          	fence	iorw,iorw
20059098:	973e                	c.add	a4,a5
2005909a:	04f7f00b          	cache	dwb,(a5)
2005909e:	02078793          	addi	a5,a5,32
200590a2:	40f706b3          	sub	a3,a4,a5
200590a6:	fed04ae3          	blt	zero,a3,2005909a <ipc_send_message+0xf2>
200590aa:	0ff0000f          	fence	iorw,iorw
200590ae:	0001                	c.addi	zero,0
200590b0:	0001                	c.addi	zero,0
200590b2:	0001                	c.addi	zero,0
200590b4:	0001                	c.addi	zero,0
200590b6:	0001                	c.addi	zero,0
200590b8:	0001                	c.addi	zero,0
200590ba:	0001                	c.addi	zero,0
200590bc:	0001                	c.addi	zero,0
200590be:	0001                	c.addi	zero,0
200590c0:	0001                	c.addi	zero,0
200590c2:	0001                	c.addi	zero,0
200590c4:	c080                	c.sw	s0,0(s1)
200590c6:	854a                	c.mv	a0,s2
200590c8:	3c1d                	c.jal	20058afe <irq_enable_restore>
200590ca:	4781                	c.li	a5,0
200590cc:	a801                	c.j	200590dc <ipc_send_message+0x134>
200590ce:	4481                	c.li	s1,0
200590d0:	4581                	c.li	a1,0
200590d2:	b7a1                	c.j	2005901a <ipc_send_message+0x72>
200590d4:	8526                	c.mv	a0,s1
200590d6:	35a1                	c.jal	20058f1e <IPC_wait_idle>
200590d8:	4785                	c.li	a5,1
200590da:	d539                	c.beqz	a0,20059028 <ipc_send_message+0x80>
200590dc:	40f2                	c.lwsp	ra,28(sp)
200590de:	4462                	c.lwsp	s0,24(sp)
200590e0:	44d2                	c.lwsp	s1,20(sp)
200590e2:	4942                	c.lwsp	s2,16(sp)
200590e4:	49b2                	c.lwsp	s3,12(sp)
200590e6:	4a22                	c.lwsp	s4,8(sp)
200590e8:	4a92                	c.lwsp	s5,4(sp)
200590ea:	853e                	c.mv	a0,a5
200590ec:	6105                	c.addi16sp	sp,32
200590ee:	8082                	c.jr	ra

200590f0 <ipc_get_message>:
200590f0:	00355793          	srli	a5,a0,0x3
200590f4:	8bf9                	c.andi	a5,30
200590f6:	893d                	c.andi	a0,15
200590f8:	953e                	c.add	a0,a5
200590fa:	050e                	c.slli	a0,0x3
200590fc:	952e                	c.add	a0,a1
200590fe:	200055b7          	lui	a1,0x20005
20059102:	90058593          	addi	a1,a1,-1792 # 20004900 <__kr4_ipc_memory_start__>
20059106:	0512                	c.slli	a0,0x4
20059108:	952e                	c.add	a0,a1
2005910a:	7c1027f3          	csrrs	a5,0x7c1,zero
2005910e:	1007f793          	andi	a5,a5,256
20059112:	c7a9                	c.beqz	a5,2005915c <ipc_get_message+0x6c>
20059114:	01f57713          	andi	a4,a0,31
20059118:	87aa                	c.mv	a5,a0
2005911a:	46c1                	c.li	a3,16
2005911c:	c709                	c.beqz	a4,20059126 <ipc_get_message+0x36>
2005911e:	01070693          	addi	a3,a4,16
20059122:	fe057793          	andi	a5,a0,-32
20059126:	01f7f713          	andi	a4,a5,31
2005912a:	9736                	c.add	a4,a3
2005912c:	0ff0000f          	fence	iorw,iorw
20059130:	973e                	c.add	a4,a5
20059132:	00f7f00b          	cache	dinv,(a5)
20059136:	02078793          	addi	a5,a5,32
2005913a:	40f706b3          	sub	a3,a4,a5
2005913e:	fed04ae3          	blt	zero,a3,20059132 <ipc_get_message+0x42>
20059142:	0ff0000f          	fence	iorw,iorw
20059146:	0001                	c.addi	zero,0
20059148:	0001                	c.addi	zero,0
2005914a:	0001                	c.addi	zero,0
2005914c:	0001                	c.addi	zero,0
2005914e:	0001                	c.addi	zero,0
20059150:	0001                	c.addi	zero,0
20059152:	0001                	c.addi	zero,0
20059154:	0001                	c.addi	zero,0
20059156:	0001                	c.addi	zero,0
20059158:	0001                	c.addi	zero,0
2005915a:	0001                	c.addi	zero,0
2005915c:	8082                	c.jr	ra

2005915e <IPC_INTConfig>:
2005915e:	4785                	c.li	a5,1
20059160:	00b795b3          	sll	a1,a5,a1
20059164:	00f61663          	bne	a2,a5,20059170 <IPC_INTConfig+0x12>
20059168:	455c                	c.lw	a5,12(a0)
2005916a:	8ddd                	c.or	a1,a5
2005916c:	c54c                	c.sw	a1,12(a0)
2005916e:	8082                	c.jr	ra
20059170:	455c                	c.lw	a5,12(a0)
20059172:	fff5c593          	xori	a1,a1,-1
20059176:	8dfd                	c.and	a1,a5
20059178:	bfd5                	c.j	2005916c <IPC_INTConfig+0xe>

2005917a <IPC_IERGet>:
2005917a:	4548                	c.lw	a0,12(a0)
2005917c:	8082                	c.jr	ra

2005917e <IPC_INTHandler>:
2005917e:	7179                	c.addi16sp	sp,-48
20059180:	d422                	c.swsp	s0,40(sp)
20059182:	d226                	c.swsp	s1,36(sp)
20059184:	d04a                	c.swsp	s2,32(sp)
20059186:	ce4e                	c.swsp	s3,28(sp)
20059188:	c85a                	c.swsp	s6,16(sp)
2005918a:	c65e                	c.swsp	s7,12(sp)
2005918c:	d606                	c.swsp	ra,44(sp)
2005918e:	cc52                	c.swsp	s4,24(sp)
20059190:	ca56                	c.swsp	s5,20(sp)
20059192:	00852a83          	lw	s5,8(a0)
20059196:	89aa                	c.mv	s3,a0
20059198:	06818913          	addi	s2,gp,104 # 20066140 <IPC_IrqData>
2005919c:	0e818493          	addi	s1,gp,232 # 200661c0 <IPC_IrqHandler>
200591a0:	4401                	c.li	s0,0
200591a2:	4b85                	c.li	s7,1
200591a4:	02000b13          	addi	s6,zero,32
200591a8:	008b9a33          	sll	s4,s7,s0
200591ac:	015a77b3          	and	a5,s4,s5
200591b0:	cb91                	c.beqz	a5,200591c4 <IPC_INTHandler+0x46>
200591b2:	409c                	c.lw	a5,0(s1)
200591b4:	c791                	c.beqz	a5,200591c0 <IPC_INTHandler+0x42>
200591b6:	00092503          	lw	a0,0(s2)
200591ba:	8622                	c.mv	a2,s0
200591bc:	85d6                	c.mv	a1,s5
200591be:	9782                	c.jalr	a5
200591c0:	0149a423          	sw	s4,8(s3)
200591c4:	0405                	c.addi	s0,1
200591c6:	0911                	c.addi	s2,4
200591c8:	0491                	c.addi	s1,4
200591ca:	fd641fe3          	bne	s0,s6,200591a8 <IPC_INTHandler+0x2a>
200591ce:	50b2                	c.lwsp	ra,44(sp)
200591d0:	5422                	c.lwsp	s0,40(sp)
200591d2:	5492                	c.lwsp	s1,36(sp)
200591d4:	5902                	c.lwsp	s2,32(sp)
200591d6:	49f2                	c.lwsp	s3,28(sp)
200591d8:	4a62                	c.lwsp	s4,24(sp)
200591da:	4ad2                	c.lwsp	s5,20(sp)
200591dc:	4b42                	c.lwsp	s6,16(sp)
200591de:	4bb2                	c.lwsp	s7,12(sp)
200591e0:	4501                	c.li	a0,0
200591e2:	6145                	c.addi16sp	sp,48
200591e4:	8082                	c.jr	ra

200591e6 <IPC_INTUserHandler>:
200591e6:	c60d                	c.beqz	a2,20059210 <IPC_INTUserHandler+0x2a>
200591e8:	00259793          	slli	a5,a1,0x2
200591ec:	0e818713          	addi	a4,gp,232 # 200661c0 <IPC_IrqHandler>
200591f0:	883e                	c.mv	a6,a5
200591f2:	973e                	c.add	a4,a5
200591f4:	06818793          	addi	a5,gp,104 # 20066140 <IPC_IrqData>
200591f8:	97c2                	c.add	a5,a6
200591fa:	c394                	c.sw	a3,0(a5)
200591fc:	c310                	c.sw	a2,0(a4)
200591fe:	47bd                	c.li	a5,15
20059200:	00b7f863          	bgeu	a5,a1,20059210 <IPC_INTUserHandler+0x2a>
20059204:	4558                	c.lw	a4,12(a0)
20059206:	4785                	c.li	a5,1
20059208:	00b795b3          	sll	a1,a5,a1
2005920c:	8dd9                	c.or	a1,a4
2005920e:	c54c                	c.sw	a1,12(a0)
20059210:	8082                	c.jr	ra

20059212 <IPC_SEMTake>:
20059212:	7179                	c.addi16sp	sp,-48
20059214:	d422                	c.swsp	s0,40(sp)
20059216:	d226                	c.swsp	s1,36(sp)
20059218:	d04a                	c.swsp	s2,32(sp)
2005921a:	d606                	c.swsp	ra,44(sp)
2005921c:	ce4e                	c.swsp	s3,28(sp)
2005921e:	cc52                	c.swsp	s4,24(sp)
20059220:	ca56                	c.swsp	s5,20(sp)
20059222:	4100c7b7          	lui	a5,0x4100c
20059226:	2287aa03          	lw	s4,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005922a:	e7c18793          	addi	a5,gp,-388 # 20065f54 <PXID_Idx>
2005922e:	97aa                	c.add	a5,a0
20059230:	0007c783          	lbu	a5,0(a5)
20059234:	892a                	c.mv	s2,a0
20059236:	842e                	c.mv	s0,a1
20059238:	e7c18493          	addi	s1,gp,-388 # 20065f54 <PXID_Idx>
2005923c:	eb89                	c.bnez	a5,2005924e <IPC_SEMTake+0x3c>
2005923e:	20062537          	lui	a0,0x20062
20059242:	0bf00593          	addi	a1,zero,191
20059246:	b3850513          	addi	a0,a0,-1224 # 20061b38 <CSWTCH.30+0x88>
2005924a:	2f4060ef          	jal	ra,2005f53e <io_assert_failed>
2005924e:	00291793          	slli	a5,s2,0x2
20059252:	410209b7          	lui	s3,0x41020
20059256:	99be                	c.add	s3,a5
20059258:	0149a783          	lw	a5,20(s3) # 41020014 <__km4_bd_boot_download_addr__+0x1100e014>
2005925c:	83a1                	c.srli	a5,0x8
2005925e:	8b8d                	c.andi	a5,3
20059260:	cb89                	c.beqz	a5,20059272 <IPC_SEMTake+0x60>
20059262:	c42d                	c.beqz	s0,200592cc <IPC_SEMTake+0xba>
20059264:	eac1a783          	lw	a5,-340(gp) # 20065f84 <ipc_delay>
20059268:	c399                	c.beqz	a5,2005926e <IPC_SEMTake+0x5c>
2005926a:	4505                	c.li	a0,1
2005926c:	9782                	c.jalr	a5
2005926e:	147d                	c.addi	s0,-1
20059270:	b7e5                	c.j	20059258 <IPC_SEMTake+0x46>
20059272:	006a5593          	srli	a1,s4,0x6
20059276:	898d                	c.andi	a1,3
20059278:	c62e                	c.swsp	a1,12(sp)
2005927a:	87dff0ef          	jal	ra,20058af6 <irq_disable_save>
2005927e:	01248733          	add	a4,s1,s2
20059282:	0149a783          	lw	a5,20(s3)
20059286:	00074683          	lbu	a3,0(a4)
2005928a:	45b2                	c.lwsp	a1,12(sp)
2005928c:	842a                	c.mv	s0,a0
2005928e:	8fd5                	c.or	a5,a3
20059290:	4007e793          	ori	a5,a5,1024
20059294:	00f9aa23          	sw	a5,20(s3)
20059298:	0149a703          	lw	a4,20(s3)
2005929c:	0149a783          	lw	a5,20(s3)
200592a0:	00158613          	addi	a2,a1,1
200592a4:	8321                	c.srli	a4,0x8
200592a6:	8b0d                	c.andi	a4,3
200592a8:	0ff7f793          	andi	a5,a5,255
200592ac:	00e61463          	bne	a2,a4,200592b4 <IPC_SEMTake+0xa2>
200592b0:	02f68863          	beq	a3,a5,200592e0 <IPC_SEMTake+0xce>
200592b4:	20062537          	lui	a0,0x20062
200592b8:	177d                	c.addi	a4,-1
200592ba:	864a                	c.mv	a2,s2
200592bc:	be050513          	addi	a0,a0,-1056 # 20061be0 <CSWTCH.30+0x130>
200592c0:	58f050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
200592c4:	8522                	c.mv	a0,s0
200592c6:	839ff0ef          	jal	ra,20058afe <irq_enable_restore>
200592ca:	4401                	c.li	s0,0
200592cc:	50b2                	c.lwsp	ra,44(sp)
200592ce:	8522                	c.mv	a0,s0
200592d0:	5422                	c.lwsp	s0,40(sp)
200592d2:	5492                	c.lwsp	s1,36(sp)
200592d4:	5902                	c.lwsp	s2,32(sp)
200592d6:	49f2                	c.lwsp	s3,28(sp)
200592d8:	4a62                	c.lwsp	s4,24(sp)
200592da:	4ad2                	c.lwsp	s5,20(sp)
200592dc:	6145                	c.addi16sp	sp,48
200592de:	8082                	c.jr	ra
200592e0:	0ff00713          	addi	a4,zero,255
200592e4:	4785                	c.li	a5,1
200592e6:	00e68563          	beq	a3,a4,200592f0 <IPC_SEMTake+0xde>
200592ea:	0685                	c.addi	a3,1
200592ec:	0ff6f793          	andi	a5,a3,255
200592f0:	94ca                	c.add	s1,s2
200592f2:	8522                	c.mv	a0,s0
200592f4:	00f48023          	sb	a5,0(s1)
200592f8:	807ff0ef          	jal	ra,20058afe <irq_enable_restore>
200592fc:	4405                	c.li	s0,1
200592fe:	b7f9                	c.j	200592cc <IPC_SEMTake+0xba>

20059300 <IPC_SEMFree>:
20059300:	4100c7b7          	lui	a5,0x4100c
20059304:	2287a583          	lw	a1,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
20059308:	410206b7          	lui	a3,0x41020
2005930c:	00251793          	slli	a5,a0,0x2
20059310:	96be                	c.add	a3,a5
20059312:	4ad8                	c.lw	a4,20(a3)
20059314:	8199                	c.srli	a1,0x6
20059316:	898d                	c.andi	a1,3
20059318:	8321                	c.srli	a4,0x8
2005931a:	4adc                	c.lw	a5,20(a3)
2005931c:	862a                	c.mv	a2,a0
2005931e:	8b0d                	c.andi	a4,3
20059320:	00158513          	addi	a0,a1,1
20059324:	02e50663          	beq	a0,a4,20059350 <IPC_SEMFree+0x50>
20059328:	1141                	c.addi	sp,-16
2005932a:	e7c18693          	addi	a3,gp,-388 # 20065f54 <PXID_Idx>
2005932e:	c606                	c.swsp	ra,12(sp)
20059330:	96b2                	c.add	a3,a2
20059332:	0006c683          	lbu	a3,0(a3) # 41020000 <__km4_bd_boot_download_addr__+0x1100e000>
20059336:	20062537          	lui	a0,0x20062
2005933a:	0ff7f793          	andi	a5,a5,255
2005933e:	177d                	c.addi	a4,-1
20059340:	c2c50513          	addi	a0,a0,-980 # 20061c2c <CSWTCH.30+0x17c>
20059344:	50b050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059348:	40b2                	c.lwsp	ra,12(sp)
2005934a:	4501                	c.li	a0,0
2005934c:	0141                	c.addi	sp,16
2005934e:	8082                	c.jr	ra
20059350:	4adc                	c.lw	a5,20(a3)
20059352:	6705                	c.lui	a4,0x1
20059354:	80070713          	addi	a4,a4,-2048 # 800 <__NVIC_GetVectorArg+0x4e>
20059358:	8fd9                	c.or	a5,a4
2005935a:	cadc                	c.sw	a5,20(a3)
2005935c:	4505                	c.li	a0,1
2005935e:	8082                	c.jr	ra

20059360 <IPC_SEMDelayStub>:
20059360:	eaa1a623          	sw	a0,-340(gp) # 20065f84 <ipc_delay>
20059364:	8082                	c.jr	ra

20059366 <IPC_GetDevById>:
20059366:	4789                	c.li	a5,2
20059368:	02a7f163          	bgeu	a5,a0,2005938a <IPC_GetDevById+0x24>
2005936c:	20062537          	lui	a0,0x20062
20059370:	1141                	c.addi	sp,-16
20059372:	b3850513          	addi	a0,a0,-1224 # 20061b38 <CSWTCH.30+0x88>
20059376:	11b00593          	addi	a1,zero,283
2005937a:	c606                	c.swsp	ra,12(sp)
2005937c:	1c2060ef          	jal	ra,2005f53e <io_assert_failed>
20059380:	40b2                	c.lwsp	ra,12(sp)
20059382:	41020537          	lui	a0,0x41020
20059386:	0141                	c.addi	sp,16
20059388:	8082                	c.jr	ra
2005938a:	4705                	c.li	a4,1
2005938c:	00e50763          	beq	a0,a4,2005939a <IPC_GetDevById+0x34>
20059390:	00f50a63          	beq	a0,a5,200593a4 <IPC_GetDevById+0x3e>
20059394:	41020537          	lui	a0,0x41020
20059398:	8082                	c.jr	ra
2005939a:	41020537          	lui	a0,0x41020
2005939e:	08050513          	addi	a0,a0,128 # 41020080 <__km4_bd_boot_download_addr__+0x1100e080>
200593a2:	8082                	c.jr	ra
200593a4:	41020537          	lui	a0,0x41020
200593a8:	10050513          	addi	a0,a0,256 # 41020100 <__km4_bd_boot_download_addr__+0x1100e100>
200593ac:	8082                	c.jr	ra

200593ae <dsp_status_on>:
200593ae:	4100c7b7          	lui	a5,0x4100c
200593b2:	2107a503          	lw	a0,528(a5) # 4100c210 <__km4_bd_boot_download_addr__+0x10ffa210>
200593b6:	404007b7          	lui	a5,0x40400
200593ba:	8d7d                	c.and	a0,a5
200593bc:	00a03533          	sltu	a0,zero,a0
200593c0:	8082                	c.jr	ra

200593c2 <ota_get_cur_index>:
200593c2:	47b1                	c.li	a5,12
200593c4:	02f50733          	mul	a4,a0,a5
200593c8:	4100d7b7          	lui	a5,0x4100d
200593cc:	1101                	c.addi	sp,-32
200593ce:	a0078793          	addi	a5,a5,-1536 # 4100ca00 <__km4_bd_boot_download_addr__+0x10ffaa00>
200593d2:	ca26                	c.swsp	s1,20(sp)
200593d4:	c84a                	c.swsp	s2,16(sp)
200593d6:	c64e                	c.swsp	s3,12(sp)
200593d8:	ce06                	c.swsp	ra,28(sp)
200593da:	cc22                	c.swsp	s0,24(sp)
200593dc:	97ba                	c.add	a5,a4
200593de:	1087a403          	lw	s0,264(a5)
200593e2:	00351993          	slli	s3,a0,0x3
200593e6:	16818793          	addi	a5,gp,360 # 20066240 <IMG_ADDR>
200593ea:	97ce                	c.add	a5,s3
200593ec:	439c                	c.lw	a5,0(a5)
200593ee:	892a                	c.mv	s2,a0
200593f0:	16818493          	addi	s1,gp,360 # 20066240 <IMG_ADDR>
200593f4:	e7a9                	c.bnez	a5,2005943e <ota_get_cur_index+0x7c>
200593f6:	4601                	c.li	a2,0
200593f8:	16818593          	addi	a1,gp,360 # 20066240 <IMG_ADDR>
200593fc:	4501                	c.li	a0,0
200593fe:	f06ff0ef          	jal	ra,20058b04 <flash_get_layout_info>
20059402:	4601                	c.li	a2,0
20059404:	00448593          	addi	a1,s1,4
20059408:	4505                	c.li	a0,1
2005940a:	efaff0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005940e:	4601                	c.li	a2,0
20059410:	00848593          	addi	a1,s1,8
20059414:	4509                	c.li	a0,2
20059416:	eeeff0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005941a:	4601                	c.li	a2,0
2005941c:	00c48593          	addi	a1,s1,12
20059420:	450d                	c.li	a0,3
20059422:	ee2ff0ef          	jal	ra,20058b04 <flash_get_layout_info>
20059426:	4601                	c.li	a2,0
20059428:	01048593          	addi	a1,s1,16
2005942c:	4511                	c.li	a0,4
2005942e:	ed6ff0ef          	jal	ra,20058b04 <flash_get_layout_info>
20059432:	4601                	c.li	a2,0
20059434:	01448593          	addi	a1,s1,20
20059438:	4511                	c.li	a0,4
2005943a:	ecaff0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005943e:	00147513          	andi	a0,s0,1
20059442:	cd01                	c.beqz	a0,2005945a <ota_get_cur_index+0x98>
20059444:	8025                	c.srli	s0,0x9
20059446:	4785                	c.li	a5,1
20059448:	0416                	c.slli	s0,0x5
2005944a:	00f91f63          	bne	s2,a5,20059468 <ota_get_cur_index+0xa6>
2005944e:	7579                	c.lui	a0,0xffffe
20059450:	942a                	c.add	s0,a0
20059452:	4488                	c.lw	a0,8(s1)
20059454:	8d01                	c.sub	a0,s0
20059456:	00a03533          	sltu	a0,zero,a0
2005945a:	40f2                	c.lwsp	ra,28(sp)
2005945c:	4462                	c.lwsp	s0,24(sp)
2005945e:	44d2                	c.lwsp	s1,20(sp)
20059460:	4942                	c.lwsp	s2,16(sp)
20059462:	49b2                	c.lwsp	s3,12(sp)
20059464:	6105                	c.addi16sp	sp,32
20059466:	8082                	c.jr	ra
20059468:	757d                	c.lui	a0,0xfffff
2005946a:	94ce                	c.add	s1,s3
2005946c:	942a                	c.add	s0,a0
2005946e:	4088                	c.lw	a0,0(s1)
20059470:	b7d5                	c.j	20059454 <ota_get_cur_index+0x92>

20059472 <OTPPowerSwitch.constprop.0>:
20059472:	4100c7b7          	lui	a5,0x4100c
20059476:	4398                	c.lw	a4,0(a5)
20059478:	8b09                	c.andi	a4,2
2005947a:	e709                	c.bnez	a4,20059484 <OTPPowerSwitch.constprop.0+0x12>
2005947c:	4398                	c.lw	a4,0(a5)
2005947e:	00276713          	ori	a4,a4,2
20059482:	c398                	c.sw	a4,0(a5)
20059484:	41000737          	lui	a4,0x41000
20059488:	4b5c                	c.lw	a5,20(a4)
2005948a:	c511                	c.beqz	a0,20059496 <OTPPowerSwitch.constprop.0+0x24>
2005948c:	690006b7          	lui	a3,0x69000
20059490:	8fd5                	c.or	a5,a3
20059492:	cb5c                	c.sw	a5,20(a4)
20059494:	8082                	c.jr	ra
20059496:	07a2                	c.slli	a5,0x8
20059498:	83a1                	c.srli	a5,0x8
2005949a:	bfe5                	c.j	20059492 <OTPPowerSwitch.constprop.0+0x20>

2005949c <OTP_Read8>:
2005949c:	1101                	c.addi	sp,-32
2005949e:	c84a                	c.swsp	s2,16(sp)
200594a0:	ce06                	c.swsp	ra,28(sp)
200594a2:	cc22                	c.swsp	s0,24(sp)
200594a4:	ca26                	c.swsp	s1,20(sp)
200594a6:	c64e                	c.swsp	s3,12(sp)
200594a8:	c452                	c.swsp	s4,8(sp)
200594aa:	c256                	c.swsp	s5,4(sp)
200594ac:	7ff00793          	addi	a5,zero,2047
200594b0:	892e                	c.mv	s2,a1
200594b2:	04a7ec63          	bltu	a5,a0,2005950a <OTP_Read8+0x6e>
200594b6:	84aa                	c.mv	s1,a0
200594b8:	4985                	c.li	s3,1
200594ba:	20062a37          	lui	s4,0x20062
200594be:	3e800593          	addi	a1,zero,1000
200594c2:	4509                	c.li	a0,2
200594c4:	33b9                	c.jal	20059212 <IPC_SEMTake>
200594c6:	842a                	c.mv	s0,a0
200594c8:	05351f63          	bne	a0,s3,20059526 <OTP_Read8+0x8a>
200594cc:	4501                	c.li	a0,0
200594ce:	3755                	c.jal	20059472 <OTPPowerSwitch.constprop.0>
200594d0:	00849793          	slli	a5,s1,0x8
200594d4:	41000737          	lui	a4,0x41000
200594d8:	6995                	c.lui	s3,0x5
200594da:	c71c                	c.sw	a5,8(a4)
200594dc:	4a01                	c.li	s4,0
200594de:	41000ab7          	lui	s5,0x41000
200594e2:	e2098993          	addi	s3,s3,-480 # 4e20 <LOGUART_ClearRxFifo+0x80a>
200594e6:	008aa783          	lw	a5,8(s5) # 41000008 <__km4_bd_boot_download_addr__+0x10fee008>
200594ea:	0407ca63          	blt	a5,zero,2005953e <OTP_Read8+0xa2>
200594ee:	053a1163          	bne	s4,s3,20059530 <OTP_Read8+0x94>
200594f2:	57fd                	c.li	a5,-1
200594f4:	20062537          	lui	a0,0x20062
200594f8:	00f90023          	sb	a5,0(s2)
200594fc:	85a6                	c.mv	a1,s1
200594fe:	c9450513          	addi	a0,a0,-876 # 20061c94 <CSWTCH.30+0x1e4>
20059502:	34d050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059506:	4401                	c.li	s0,0
20059508:	a089                	c.j	2005954a <OTP_Read8+0xae>
2005950a:	57fd                	c.li	a5,-1
2005950c:	00f58023          	sb	a5,0(a1)
20059510:	4401                	c.li	s0,0
20059512:	40f2                	c.lwsp	ra,28(sp)
20059514:	8522                	c.mv	a0,s0
20059516:	4462                	c.lwsp	s0,24(sp)
20059518:	44d2                	c.lwsp	s1,20(sp)
2005951a:	4942                	c.lwsp	s2,16(sp)
2005951c:	49b2                	c.lwsp	s3,12(sp)
2005951e:	4a22                	c.lwsp	s4,8(sp)
20059520:	4a92                	c.lwsp	s5,4(sp)
20059522:	6105                	c.addi16sp	sp,32
20059524:	8082                	c.jr	ra
20059526:	c78a0513          	addi	a0,s4,-904 # 20061c78 <CSWTCH.30+0x1c8>
2005952a:	325050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005952e:	bf41                	c.j	200594be <OTP_Read8+0x22>
20059530:	4515                	c.li	a0,5
20059532:	dffa9097          	auipc	ra,0xdffa9
20059536:	d88080e7          	jalr	ra,-632(ra) # 22ba <DelayUs>
2005953a:	0a05                	c.addi	s4,1
2005953c:	b76d                	c.j	200594e6 <OTP_Read8+0x4a>
2005953e:	fb3a0ae3          	beq	s4,s3,200594f2 <OTP_Read8+0x56>
20059542:	008aa783          	lw	a5,8(s5)
20059546:	00f90023          	sb	a5,0(s2)
2005954a:	41000737          	lui	a4,0x41000
2005954e:	4b5c                	c.lw	a5,20(a4)
20059550:	4509                	c.li	a0,2
20059552:	07a2                	c.slli	a5,0x8
20059554:	83a1                	c.srli	a5,0x8
20059556:	cb5c                	c.sw	a5,20(a4)
20059558:	3365                	c.jal	20059300 <IPC_SEMFree>
2005955a:	bf65                	c.j	20059512 <OTP_Read8+0x76>

2005955c <pinmap_init>:
2005955c:	715d                	c.addi16sp	sp,-80
2005955e:	4100d7b7          	lui	a5,0x4100d
20059562:	c4a2                	c.swsp	s0,72(sp)
20059564:	c2a6                	c.swsp	s1,68(sp)
20059566:	c0ca                	c.swsp	s2,64(sp)
20059568:	de4e                	c.swsp	s3,60(sp)
2005956a:	dc52                	c.swsp	s4,56(sp)
2005956c:	c686                	c.swsp	ra,76(sp)
2005956e:	da56                	c.swsp	s5,52(sp)
20059570:	d85a                	c.swsp	s6,48(sp)
20059572:	d65e                	c.swsp	s7,44(sp)
20059574:	90078793          	addi	a5,a5,-1792 # 4100c900 <__km4_bd_boot_download_addr__+0x10ffa900>
20059578:	4398                	c.lw	a4,0(a5)
2005957a:	200624b7          	lui	s1,0x20062
2005957e:	6941                	c.lui	s2,0x10
20059580:	c03a                	c.swsp	a4,0(sp)
20059582:	4798                	c.lw	a4,8(a5)
20059584:	ca848493          	addi	s1,s1,-856 # 20061ca8 <pmap_func>
20059588:	197d                	c.addi	s2,-1
2005958a:	c43a                	c.swsp	a4,8(sp)
2005958c:	43d8                	c.lw	a4,4(a5)
2005958e:	4a0d                	c.li	s4,3
20059590:	4405                	c.li	s0,1
20059592:	c23a                	c.swsp	a4,4(sp)
20059594:	47d8                	c.lw	a4,12(a5)
20059596:	4989                	c.li	s3,2
20059598:	c63a                	c.swsp	a4,12(sp)
2005959a:	4b98                	c.lw	a4,16(a5)
2005959c:	c83a                	c.swsp	a4,16(sp)
2005959e:	4f98                	c.lw	a4,24(a5)
200595a0:	cc3a                	c.swsp	a4,24(sp)
200595a2:	4bd8                	c.lw	a4,20(a5)
200595a4:	ca3a                	c.swsp	a4,20(sp)
200595a6:	4fdc                	c.lw	a5,28(a5)
200595a8:	ce3e                	c.swsp	a5,28(sp)
200595aa:	0004da83          	lhu	s5,0(s1)
200595ae:	112a8163          	beq	s5,s2,200596b0 <pinmap_init+0x154>
200595b2:	0024cb03          	lbu	s6,2(s1)
200595b6:	0ffafb93          	andi	s7,s5,255
200595ba:	014b1863          	bne	s6,s4,200595ca <pinmap_init+0x6e>
200595be:	4581                	c.li	a1,0
200595c0:	855e                	c.mv	a0,s7
200595c2:	dffac097          	auipc	ra,0xdffac
200595c6:	f54080e7          	jalr	ra,-172(ra) # 5516 <PAD_CMD>
200595ca:	005ad793          	srli	a5,s5,0x5
200595ce:	8b8d                	c.andi	a5,3
200595d0:	01fbf513          	andi	a0,s7,31
200595d4:	048b1963          	bne	s6,s0,20059626 <pinmap_init+0xca>
200595d8:	00279713          	slli	a4,a5,0x2
200595dc:	02070713          	addi	a4,a4,32 # 41000020 <__km4_bd_boot_download_addr__+0x10fee020>
200595e0:	970a                	c.add	a4,sp
200595e2:	f8070713          	addi	a4,a4,-128
200595e6:	5330                	c.lw	a2,96(a4)
200595e8:	00a416b3          	sll	a3,s0,a0
200595ec:	fff6c593          	xori	a1,a3,-1
200595f0:	8e6d                	c.and	a2,a1
200595f2:	d330                	c.sw	a2,96(a4)
200595f4:	5730                	c.lw	a2,104(a4)
200595f6:	8ed1                	c.or	a3,a2
200595f8:	d734                	c.sw	a3,104(a4)
200595fa:	0034c703          	lbu	a4,3(s1)
200595fe:	06871863          	bne	a4,s0,2005966e <pinmap_init+0x112>
20059602:	078a                	c.slli	a5,0x2
20059604:	02078793          	addi	a5,a5,32
20059608:	978a                	c.add	a5,sp
2005960a:	f8078793          	addi	a5,a5,-128
2005960e:	5bb8                	c.lw	a4,112(a5)
20059610:	00a41533          	sll	a0,s0,a0
20059614:	fff54693          	xori	a3,a0,-1
20059618:	8f75                	c.and	a4,a3
2005961a:	dbb8                	c.sw	a4,112(a5)
2005961c:	5fb8                	c.lw	a4,120(a5)
2005961e:	8d59                	c.or	a0,a4
20059620:	dfa8                	c.sw	a0,120(a5)
20059622:	0491                	c.addi	s1,4
20059624:	b759                	c.j	200595aa <pinmap_init+0x4e>
20059626:	033b1363          	bne	s6,s3,2005964c <pinmap_init+0xf0>
2005962a:	00279713          	slli	a4,a5,0x2
2005962e:	02070713          	addi	a4,a4,32
20059632:	970a                	c.add	a4,sp
20059634:	f8070713          	addi	a4,a4,-128
20059638:	5330                	c.lw	a2,96(a4)
2005963a:	00a416b3          	sll	a3,s0,a0
2005963e:	8e55                	c.or	a2,a3
20059640:	d330                	c.sw	a2,96(a4)
20059642:	fff6c693          	xori	a3,a3,-1
20059646:	5730                	c.lw	a2,104(a4)
20059648:	8ef1                	c.and	a3,a2
2005964a:	b77d                	c.j	200595f8 <pinmap_init+0x9c>
2005964c:	fa0b17e3          	bne	s6,zero,200595fa <pinmap_init+0x9e>
20059650:	00279713          	slli	a4,a5,0x2
20059654:	02070713          	addi	a4,a4,32
20059658:	970a                	c.add	a4,sp
2005965a:	f8070713          	addi	a4,a4,-128
2005965e:	5330                	c.lw	a2,96(a4)
20059660:	00a416b3          	sll	a3,s0,a0
20059664:	fff6c693          	xori	a3,a3,-1
20059668:	8e75                	c.and	a2,a3
2005966a:	d330                	c.sw	a2,96(a4)
2005966c:	bfe9                	c.j	20059646 <pinmap_init+0xea>
2005966e:	03371263          	bne	a4,s3,20059692 <pinmap_init+0x136>
20059672:	078a                	c.slli	a5,0x2
20059674:	02078793          	addi	a5,a5,32
20059678:	978a                	c.add	a5,sp
2005967a:	f8078793          	addi	a5,a5,-128
2005967e:	5bb8                	c.lw	a4,112(a5)
20059680:	00a41533          	sll	a0,s0,a0
20059684:	8f49                	c.or	a4,a0
20059686:	dbb8                	c.sw	a4,112(a5)
20059688:	fff54513          	xori	a0,a0,-1
2005968c:	5fb8                	c.lw	a4,120(a5)
2005968e:	8d79                	c.and	a0,a4
20059690:	bf41                	c.j	20059620 <pinmap_init+0xc4>
20059692:	fb41                	c.bnez	a4,20059622 <pinmap_init+0xc6>
20059694:	078a                	c.slli	a5,0x2
20059696:	02078793          	addi	a5,a5,32
2005969a:	978a                	c.add	a5,sp
2005969c:	f8078793          	addi	a5,a5,-128
200596a0:	5bb8                	c.lw	a4,112(a5)
200596a2:	00a41533          	sll	a0,s0,a0
200596a6:	fff54513          	xori	a0,a0,-1
200596aa:	8f69                	c.and	a4,a0
200596ac:	dbb8                	c.sw	a4,112(a5)
200596ae:	bff9                	c.j	2005968c <pinmap_init+0x130>
200596b0:	4702                	c.lwsp	a4,0(sp)
200596b2:	4100d7b7          	lui	a5,0x4100d
200596b6:	90078793          	addi	a5,a5,-1792 # 4100c900 <__km4_bd_boot_download_addr__+0x10ffa900>
200596ba:	c398                	c.sw	a4,0(a5)
200596bc:	4722                	c.lwsp	a4,8(sp)
200596be:	c798                	c.sw	a4,8(a5)
200596c0:	4742                	c.lwsp	a4,16(sp)
200596c2:	cb98                	c.sw	a4,16(a5)
200596c4:	4762                	c.lwsp	a4,24(sp)
200596c6:	cf98                	c.sw	a4,24(a5)
200596c8:	4712                	c.lwsp	a4,4(sp)
200596ca:	c3d8                	c.sw	a4,4(a5)
200596cc:	4732                	c.lwsp	a4,12(sp)
200596ce:	c7d8                	c.sw	a4,12(a5)
200596d0:	4752                	c.lwsp	a4,20(sp)
200596d2:	cbd8                	c.sw	a4,20(a5)
200596d4:	4772                	c.lwsp	a4,28(sp)
200596d6:	cfd8                	c.sw	a4,28(a5)
200596d8:	40b6                	c.lwsp	ra,76(sp)
200596da:	4426                	c.lwsp	s0,72(sp)
200596dc:	4496                	c.lwsp	s1,68(sp)
200596de:	4906                	c.lwsp	s2,64(sp)
200596e0:	59f2                	c.lwsp	s3,60(sp)
200596e2:	5a62                	c.lwsp	s4,56(sp)
200596e4:	5ad2                	c.lwsp	s5,52(sp)
200596e6:	5b42                	c.lwsp	s6,48(sp)
200596e8:	5bb2                	c.lwsp	s7,44(sp)
200596ea:	6161                	c.addi16sp	sp,80
200596ec:	8082                	c.jr	ra

200596ee <PLL_State>:
200596ee:	4709                	c.li	a4,2
200596f0:	87aa                	c.mv	a5,a0
200596f2:	00e51963          	bne	a0,a4,20059704 <PLL_State+0x16>
200596f6:	4100c7b7          	lui	a5,0x4100c
200596fa:	44078793          	addi	a5,a5,1088 # 4100c440 <__km4_bd_boot_download_addr__+0x10ffa440>
200596fe:	5b88                	c.lw	a0,48(a5)
20059700:	8905                	c.andi	a0,1
20059702:	8082                	c.jr	ra
20059704:	470d                	c.li	a4,3
20059706:	4501                	c.li	a0,0
20059708:	00e79863          	bne	a5,a4,20059718 <PLL_State+0x2a>
2005970c:	4100c7b7          	lui	a5,0x4100c
20059710:	44078793          	addi	a5,a5,1088 # 4100c440 <__km4_bd_boot_download_addr__+0x10ffa440>
20059714:	5b88                	c.lw	a0,48(a5)
20059716:	8909                	c.andi	a0,2
20059718:	8082                	c.jr	ra

2005971a <pmu_register_sleep_callback>:
2005971a:	1101                	c.addi	sp,-32
2005971c:	cc22                	c.swsp	s0,24(sp)
2005971e:	ca26                	c.swsp	s1,20(sp)
20059720:	c84a                	c.swsp	s2,16(sp)
20059722:	c64e                	c.swsp	s3,12(sp)
20059724:	c452                	c.swsp	s4,8(sp)
20059726:	ce06                	c.swsp	ra,28(sp)
20059728:	47f9                	c.li	a5,30
2005972a:	842a                	c.mv	s0,a0
2005972c:	892e                	c.mv	s2,a1
2005972e:	8a32                	c.mv	s4,a2
20059730:	84b6                	c.mv	s1,a3
20059732:	89ba                	c.mv	s3,a4
20059734:	00a7fa63          	bgeu	a5,a0,20059748 <pmu_register_sleep_callback+0x2e>
20059738:	20062537          	lui	a0,0x20062
2005973c:	07300593          	addi	a1,zero,115
20059740:	da850513          	addi	a0,a0,-600 # 20061da8 <pmap_func+0x100>
20059744:	5fb050ef          	jal	ra,2005f53e <io_assert_failed>
20059748:	00091b63          	bne	s2,zero,2005975e <pmu_register_sleep_callback+0x44>
2005974c:	e889                	c.bnez	s1,2005975e <pmu_register_sleep_callback+0x44>
2005974e:	20062537          	lui	a0,0x20062
20059752:	07400593          	addi	a1,zero,116
20059756:	da850513          	addi	a0,a0,-600 # 20061da8 <pmap_func+0x100>
2005975a:	5e5050ef          	jal	ra,2005f53e <io_assert_failed>
2005975e:	47d1                	c.li	a5,20
20059760:	02f40733          	mul	a4,s0,a5
20059764:	18018793          	addi	a5,gp,384 # 20066258 <gPsmDdHookInfo>
20059768:	97ba                	c.add	a5,a4
2005976a:	c380                	c.sw	s0,0(a5)
2005976c:	0127a223          	sw	s2,4(a5)
20059770:	0147a423          	sw	s4,8(a5)
20059774:	c7c4                	c.sw	s1,12(a5)
20059776:	0137a823          	sw	s3,16(a5)
2005977a:	40f2                	c.lwsp	ra,28(sp)
2005977c:	4462                	c.lwsp	s0,24(sp)
2005977e:	44d2                	c.lwsp	s1,20(sp)
20059780:	4942                	c.lwsp	s2,16(sp)
20059782:	49b2                	c.lwsp	s3,12(sp)
20059784:	4a22                	c.lwsp	s4,8(sp)
20059786:	6105                	c.addi16sp	sp,32
20059788:	8082                	c.jr	ra

2005978a <pmu_yield_os_check>:
2005978a:	e801a503          	lw	a0,-384(gp) # 20065f58 <system_can_yield>
2005978e:	8082                	c.jr	ra

20059790 <pmu_ready_to_sleep>:
20059790:	1141                	c.addi	sp,-16
20059792:	c606                	c.swsp	ra,12(sp)
20059794:	39c020ef          	jal	ra,2005bb30 <rtos_time_get_current_system_time_ms>
20059798:	eb41a703          	lw	a4,-332(gp) # 20065f8c <sleepwakelock_timeout>
2005979c:	800006b7          	lui	a3,0x80000
200597a0:	87aa                	c.mv	a5,a0
200597a2:	ffe6c693          	xori	a3,a3,-2
200597a6:	00a77e63          	bgeu	a4,a0,200597c2 <pmu_ready_to_sleep+0x32>
200597aa:	40e507b3          	sub	a5,a0,a4
200597ae:	4501                	c.li	a0,0
200597b0:	02f6e063          	bltu	a3,a5,200597d0 <pmu_ready_to_sleep+0x40>
200597b4:	3eed                	c.jal	200593ae <dsp_status_on>
200597b6:	e841a783          	lw	a5,-380(gp) # 20065f5c <wakelock>
200597ba:	8d5d                	c.or	a0,a5
200597bc:	00153513          	sltiu	a0,a0,1
200597c0:	a801                	c.j	200597d0 <pmu_ready_to_sleep+0x40>
200597c2:	40a70633          	sub	a2,a4,a0
200597c6:	fec6e7e3          	bltu	a3,a2,200597b4 <pmu_ready_to_sleep+0x24>
200597ca:	4501                	c.li	a0,0
200597cc:	fee7f4e3          	bgeu	a5,a4,200597b4 <pmu_ready_to_sleep+0x24>
200597d0:	40b2                	c.lwsp	ra,12(sp)
200597d2:	0141                	c.addi	sp,16
200597d4:	8082                	c.jr	ra

200597d6 <pmu_acquire_wakelock>:
200597d6:	1141                	c.addi	sp,-16
200597d8:	c422                	c.swsp	s0,8(sp)
200597da:	c606                	c.swsp	ra,12(sp)
200597dc:	842a                	c.mv	s0,a0
200597de:	b18ff0ef          	jal	ra,20058af6 <irq_disable_save>
200597e2:	e8418713          	addi	a4,gp,-380 # 20065f5c <wakelock>
200597e6:	4314                	c.lw	a3,0(a4)
200597e8:	4785                	c.li	a5,1
200597ea:	008797b3          	sll	a5,a5,s0
200597ee:	8fd5                	c.or	a5,a3
200597f0:	c31c                	c.sw	a5,0(a4)
200597f2:	4422                	c.lwsp	s0,8(sp)
200597f4:	40b2                	c.lwsp	ra,12(sp)
200597f6:	0141                	c.addi	sp,16
200597f8:	b06ff06f          	jal	zero,20058afe <irq_enable_restore>

200597fc <PSRAM_calibration>:
200597fc:	7119                	c.addi16sp	sp,-128
200597fe:	de86                	c.swsp	ra,124(sp)
20059800:	dca6                	c.swsp	s1,120(sp)
20059802:	daca                	c.swsp	s2,116(sp)
20059804:	d8ce                	c.swsp	s3,112(sp)
20059806:	d6d2                	c.swsp	s4,108(sp)
20059808:	d4d6                	c.swsp	s5,104(sp)
2005980a:	d2da                	c.swsp	s6,100(sp)
2005980c:	d0de                	c.swsp	s7,96(sp)
2005980e:	cee2                	c.swsp	s8,92(sp)
20059810:	cce6                	c.swsp	s9,88(sp)
20059812:	caea                	c.swsp	s10,84(sp)
20059814:	c8ee                	c.swsp	s11,80(sp)
20059816:	cc2a                	c.swsp	a0,24(sp)
20059818:	410087b7          	lui	a5,0x41008
2005981c:	43d8                	c.lw	a4,4(a5)
2005981e:	100007b7          	lui	a5,0x10000
20059822:	1781                	c.addi	a5,-32
20059824:	8ff9                	c.and	a5,a4
20059826:	d43e                	c.swsp	a5,40(sp)
20059828:	dc02                	c.swsp	zero,56(sp)
2005982a:	de02                	c.swsp	zero,60(sp)
2005982c:	c082                	c.swsp	zero,64(sp)
2005982e:	c282                	c.swsp	zero,68(sp)
20059830:	c482                	c.swsp	zero,72(sp)
20059832:	c682                	c.swsp	zero,76(sp)
20059834:	7c1027f3          	csrrs	a5,0x7c1,zero
20059838:	1007f793          	andi	a5,a5,256
2005983c:	c385                	c.beqz	a5,2005985c <PSRAM_calibration+0x60>
2005983e:	7c0467f3          	csrrsi	a5,0x7c0,8
20059842:	0ff0000f          	fence	iorw,iorw
20059846:	0001                	c.addi	zero,0
20059848:	0001                	c.addi	zero,0
2005984a:	0001                	c.addi	zero,0
2005984c:	0001                	c.addi	zero,0
2005984e:	0001                	c.addi	zero,0
20059850:	0001                	c.addi	zero,0
20059852:	0001                	c.addi	zero,0
20059854:	0001                	c.addi	zero,0
20059856:	0001                	c.addi	zero,0
20059858:	0001                	c.addi	zero,0
2005985a:	0001                	c.addi	zero,0
2005985c:	10000793          	addi	a5,zero,256
20059860:	7c17b7f3          	csrrc	a5,0x7c1,a5
20059864:	0ff0000f          	fence	iorw,iorw
20059868:	0001                	c.addi	zero,0
2005986a:	0001                	c.addi	zero,0
2005986c:	0001                	c.addi	zero,0
2005986e:	0001                	c.addi	zero,0
20059870:	0001                	c.addi	zero,0
20059872:	0001                	c.addi	zero,0
20059874:	0001                	c.addi	zero,0
20059876:	0001                	c.addi	zero,0
20059878:	0001                	c.addi	zero,0
2005987a:	0001                	c.addi	zero,0
2005987c:	0001                	c.addi	zero,0
2005987e:	7c0467f3          	csrrsi	a5,0x7c0,8
20059882:	0ff0000f          	fence	iorw,iorw
20059886:	0001                	c.addi	zero,0
20059888:	0001                	c.addi	zero,0
2005988a:	0001                	c.addi	zero,0
2005988c:	0001                	c.addi	zero,0
2005988e:	0001                	c.addi	zero,0
20059890:	0001                	c.addi	zero,0
20059892:	0001                	c.addi	zero,0
20059894:	0001                	c.addi	zero,0
20059896:	0001                	c.addi	zero,0
20059898:	0001                	c.addi	zero,0
2005989a:	0001                	c.addi	zero,0
2005989c:	41008737          	lui	a4,0x41008
200598a0:	431c                	c.lw	a5,0(a4)
200598a2:	20062c37          	lui	s8,0x20062
200598a6:	60000d37          	lui	s10,0x60000
200598aa:	9bf9                	c.andi	a5,-2
200598ac:	c31c                	c.sw	a5,0(a4)
200598ae:	4791                	c.li	a5,4
200598b0:	ca3e                	c.swsp	a5,20(sp)
200598b2:	000d0b13          	addi	s6,s10,0 # 60000000 <__km4_bd_psram_start__>
200598b6:	e40c0793          	addi	a5,s8,-448 # 20061e40 <pmap_func+0x198>
200598ba:	00050cb7          	lui	s9,0x50
200598be:	d03e                	c.swsp	a5,32(sp)
200598c0:	019b07b3          	add	a5,s6,s9
200598c4:	597d                	c.li	s2,-1
200598c6:	4981                	c.li	s3,0
200598c8:	5afd                	c.li	s5,-1
200598ca:	54fd                	c.li	s1,-1
200598cc:	4a05                	c.li	s4,1
200598ce:	c9418b93          	addi	s7,gp,-876 # 20065d6c <PSRAM_CALIB_PATTERN>
200598d2:	ce3e                	c.swsp	a5,28(sp)
200598d4:	47e2                	c.lwsp	a5,24(sp)
200598d6:	cf91                	c.beqz	a5,200598f2 <PSRAM_calibration+0xf6>
200598d8:	5782                	c.lwsp	a5,32(sp)
200598da:	200625b7          	lui	a1,0x20062
200598de:	8752                	c.mv	a4,s4
200598e0:	20078693          	addi	a3,a5,512 # 10000200 <__km4_boot_text_start__+0x8001e0>
200598e4:	04900613          	addi	a2,zero,73
200598e8:	e5458593          	addi	a1,a1,-428 # 20061e54 <pmap_func+0x1ac>
200598ec:	4511                	c.li	a0,4
200598ee:	582010ef          	jal	ra,2005ae70 <rtk_log_write>
200598f2:	57a2                	c.lwsp	a5,40(sp)
200598f4:	01ca1d93          	slli	s11,s4,0x1c
200598f8:	5ffd                	c.li	t6,-1
200598fa:	00fde7b3          	or	a5,s11,a5
200598fe:	d23e                	c.swsp	a5,36(sp)
20059900:	4c01                	c.li	s8,0
20059902:	5cfd                	c.li	s9,-1
20059904:	4d81                	c.li	s11,0
20059906:	5792                	c.lwsp	a5,36(sp)
20059908:	d67e                	c.swsp	t6,44(sp)
2005990a:	410086b7          	lui	a3,0x41008
2005990e:	01b7e7b3          	or	a5,a5,s11
20059912:	c2dc                	c.sw	a5,4(a3)
20059914:	000ba783          	lw	a5,0(s7)
20059918:	000d4603          	lbu	a2,0(s10)
2005991c:	4772                	c.lwsp	a4,28(sp)
2005991e:	0ff7f693          	andi	a3,a5,255
20059922:	00dd0023          	sb	a3,0(s10)
20059926:	0087d693          	srli	a3,a5,0x8
2005992a:	0ff6f693          	andi	a3,a3,255
2005992e:	001d4603          	lbu	a2,1(s10)
20059932:	00dd00a3          	sb	a3,1(s10)
20059936:	0107d693          	srli	a3,a5,0x10
2005993a:	0ff6f693          	andi	a3,a3,255
2005993e:	002d4603          	lbu	a2,2(s10)
20059942:	83e1                	c.srli	a5,0x18
20059944:	00dd0123          	sb	a3,2(s10)
20059948:	003d4683          	lbu	a3,3(s10)
2005994c:	00fd01a3          	sb	a5,3(s10)
20059950:	004ba783          	lw	a5,4(s7)
20059954:	00100837          	lui	a6,0x100
20059958:	985a                	c.add	a6,s6
2005995a:	c31c                	c.sw	a5,0(a4)
2005995c:	008ba783          	lw	a5,8(s7)
20059960:	00150537          	lui	a0,0x150
20059964:	955a                	c.add	a0,s6
20059966:	00f82023          	sw	a5,0(a6) # 100000 <__ap_sram_heap_size+0xc0000>
2005996a:	00cba783          	lw	a5,12(s7)
2005996e:	002005b7          	lui	a1,0x200
20059972:	95da                	c.add	a1,s6
20059974:	c11c                	c.sw	a5,0(a0)
20059976:	010ba783          	lw	a5,16(s7)
2005997a:	00250637          	lui	a2,0x250
2005997e:	965a                	c.add	a2,s6
20059980:	c19c                	c.sw	a5,0(a1)
20059982:	014ba783          	lw	a5,20(s7)
20059986:	c21c                	c.sw	a5,0(a2)
20059988:	000d4683          	lbu	a3,0(s10)
2005998c:	001d4783          	lbu	a5,1(s10)
20059990:	0ff6f693          	andi	a3,a3,255
20059994:	0ff7f793          	andi	a5,a5,255
20059998:	07a2                	c.slli	a5,0x8
2005999a:	8edd                	c.or	a3,a5
2005999c:	002d4783          	lbu	a5,2(s10)
200599a0:	0ff7f793          	andi	a5,a5,255
200599a4:	07c2                	c.slli	a5,0x10
200599a6:	8fd5                	c.or	a5,a3
200599a8:	003d4683          	lbu	a3,3(s10)
200599ac:	06e2                	c.slli	a3,0x18
200599ae:	8fd5                	c.or	a5,a3
200599b0:	dc3e                	c.swsp	a5,56(sp)
200599b2:	431c                	c.lw	a5,0(a4)
200599b4:	de3e                	c.swsp	a5,60(sp)
200599b6:	00082783          	lw	a5,0(a6)
200599ba:	c0be                	c.swsp	a5,64(sp)
200599bc:	411c                	c.lw	a5,0(a0)
200599be:	1828                	c.addi4spn	a0,sp,56
200599c0:	c2be                	c.swsp	a5,68(sp)
200599c2:	419c                	c.lw	a5,0(a1)
200599c4:	85de                	c.mv	a1,s7
200599c6:	c4be                	c.swsp	a5,72(sp)
200599c8:	421c                	c.lw	a5,0(a2)
200599ca:	4661                	c.li	a2,24
200599cc:	c6be                	c.swsp	a5,76(sp)
200599ce:	dffb0097          	auipc	ra,0xdffb0
200599d2:	bf2080e7          	jalr	ra,-1038(ra) # 95c0 <_memcmp>
200599d6:	5fb2                	c.lwsp	t6,44(sp)
200599d8:	47e2                	c.lwsp	a5,24(sp)
200599da:	14051563          	bne	a0,zero,20059b24 <PSRAM_calibration+0x328>
200599de:	c79d                	c.beqz	a5,20059a0c <PSRAM_calibration+0x210>
200599e0:	47b6                	c.lwsp	a5,76(sp)
200599e2:	5682                	c.lwsp	a3,32(sp)
200599e4:	4886                	c.lwsp	a7,64(sp)
200599e6:	c43e                	c.swsp	a5,8(sp)
200599e8:	47a6                	c.lwsp	a5,72(sp)
200599ea:	5872                	c.lwsp	a6,60(sp)
200599ec:	200625b7          	lui	a1,0x20062
200599f0:	c23e                	c.swsp	a5,4(sp)
200599f2:	4796                	c.lwsp	a5,68(sp)
200599f4:	876e                	c.mv	a4,s11
200599f6:	21468693          	addi	a3,a3,532 # 41008214 <__km4_bd_boot_download_addr__+0x10ff6214>
200599fa:	c03e                	c.swsp	a5,0(sp)
200599fc:	57e2                	c.lwsp	a5,56(sp)
200599fe:	04900613          	addi	a2,zero,73
20059a02:	e5458593          	addi	a1,a1,-428 # 20061e54 <pmap_func+0x1ac>
20059a06:	4511                	c.li	a0,4
20059a08:	468010ef          	jal	ra,2005ae70 <rtk_log_write>
20059a0c:	57fd                	c.li	a5,-1
20059a0e:	00fc9363          	bne	s9,a5,20059a14 <PSRAM_calibration+0x218>
20059a12:	8cee                	c.mv	s9,s11
20059a14:	47fd                	c.li	a5,31
20059a16:	018c8fb3          	add	t6,s9,s8
20059a1a:	0c05                	c.addi	s8,1
20059a1c:	00fd9863          	bne	s11,a5,20059a2c <PSRAM_calibration+0x230>
20059a20:	0189d663          	bge	s3,s8,20059a2c <PSRAM_calibration+0x230>
20059a24:	8952                	c.mv	s2,s4
20059a26:	89e2                	c.mv	s3,s8
20059a28:	8afe                	c.mv	s5,t6
20059a2a:	84e6                	c.mv	s1,s9
20059a2c:	0d85                	c.addi	s11,1
20059a2e:	02000793          	addi	a5,zero,32
20059a32:	ecfd9ae3          	bne	s11,a5,20059906 <PSRAM_calibration+0x10a>
20059a36:	47d2                	c.lwsp	a5,20(sp)
20059a38:	0a06                	c.slli	s4,0x1
20059a3a:	17fd                	c.addi	a5,-1
20059a3c:	ca3e                	c.swsp	a5,20(sp)
20059a3e:	e8079be3          	bne	a5,zero,200598d4 <PSRAM_calibration+0xd8>
20059a42:	200626b7          	lui	a3,0x20062
20059a46:	200625b7          	lui	a1,0x20062
20059a4a:	88ca                	c.mv	a7,s2
20059a4c:	884e                	c.mv	a6,s3
20059a4e:	87d6                	c.mv	a5,s5
20059a50:	8726                	c.mv	a4,s1
20059a52:	09868693          	addi	a3,a3,152 # 20062098 <pmap_func+0x3f0>
20059a56:	04900613          	addi	a2,zero,73
20059a5a:	e5458593          	addi	a1,a1,-428 # 20061e54 <pmap_func+0x1ac>
20059a5e:	4511                	c.li	a0,4
20059a60:	410010ef          	jal	ra,2005ae70 <rtk_log_write>
20059a64:	7c1027f3          	csrrs	a5,0x7c1,zero
20059a68:	1007f793          	andi	a5,a5,256
20059a6c:	e7b1                	c.bnez	a5,20059ab8 <PSRAM_calibration+0x2bc>
20059a6e:	7c1027f3          	csrrs	a5,0x7c1,zero
20059a72:	1007f793          	andi	a5,a5,256
20059a76:	e3a9                	c.bnez	a5,20059ab8 <PSRAM_calibration+0x2bc>
20059a78:	7c0167f3          	csrrsi	a5,0x7c0,2
20059a7c:	0ff0000f          	fence	iorw,iorw
20059a80:	0001                	c.addi	zero,0
20059a82:	0001                	c.addi	zero,0
20059a84:	0001                	c.addi	zero,0
20059a86:	0001                	c.addi	zero,0
20059a88:	0001                	c.addi	zero,0
20059a8a:	0001                	c.addi	zero,0
20059a8c:	0001                	c.addi	zero,0
20059a8e:	0001                	c.addi	zero,0
20059a90:	0001                	c.addi	zero,0
20059a92:	0001                	c.addi	zero,0
20059a94:	0001                	c.addi	zero,0
20059a96:	10000793          	addi	a5,zero,256
20059a9a:	7c17a7f3          	csrrs	a5,0x7c1,a5
20059a9e:	0ff0000f          	fence	iorw,iorw
20059aa2:	0001                	c.addi	zero,0
20059aa4:	0001                	c.addi	zero,0
20059aa6:	0001                	c.addi	zero,0
20059aa8:	0001                	c.addi	zero,0
20059aaa:	0001                	c.addi	zero,0
20059aac:	0001                	c.addi	zero,0
20059aae:	0001                	c.addi	zero,0
20059ab0:	0001                	c.addi	zero,0
20059ab2:	0001                	c.addi	zero,0
20059ab4:	0001                	c.addi	zero,0
20059ab6:	0001                	c.addi	zero,0
20059ab8:	47a1                	c.li	a5,8
20059aba:	4501                	c.li	a0,0
20059abc:	0537d663          	bge	a5,s3,20059b08 <PSRAM_calibration+0x30c>
20059ac0:	409a8733          	sub	a4,s5,s1
20059ac4:	4609                	c.li	a2,2
20059ac6:	02c74733          	div	a4,a4,a2
20059aca:	94d6                	c.add	s1,s5
20059acc:	6685                	c.lui	a3,0x1
20059ace:	f0068693          	addi	a3,a3,-256 # f00 <CPU_ClkGet+0x3ea>
20059ad2:	0972                	c.slli	s2,0x1c
20059ad4:	4505                	c.li	a0,1
20059ad6:	02c4c4b3          	div	s1,s1,a2
20059ada:	1779                	c.addi	a4,-2
20059adc:	00871793          	slli	a5,a4,0x8
20059ae0:	8ff5                	c.and	a5,a3
20059ae2:	0127e7b3          	or	a5,a5,s2
20059ae6:	0742                	c.slli	a4,0x10
20059ae8:	000f06b7          	lui	a3,0xf0
20059aec:	8f75                	c.and	a4,a3
20059aee:	88fd                	c.andi	s1,31
20059af0:	8fc5                	c.or	a5,s1
20059af2:	8fd9                	c.or	a5,a4
20059af4:	02000737          	lui	a4,0x2000
20059af8:	8fd9                	c.or	a5,a4
20059afa:	41008737          	lui	a4,0x41008
20059afe:	c35c                	c.sw	a5,4(a4)
20059b00:	431c                	c.lw	a5,0(a4)
20059b02:	0017e793          	ori	a5,a5,1
20059b06:	c31c                	c.sw	a5,0(a4)
20059b08:	50f6                	c.lwsp	ra,124(sp)
20059b0a:	54e6                	c.lwsp	s1,120(sp)
20059b0c:	5956                	c.lwsp	s2,116(sp)
20059b0e:	59c6                	c.lwsp	s3,112(sp)
20059b10:	5a36                	c.lwsp	s4,108(sp)
20059b12:	5aa6                	c.lwsp	s5,104(sp)
20059b14:	5b16                	c.lwsp	s6,100(sp)
20059b16:	5b86                	c.lwsp	s7,96(sp)
20059b18:	4c76                	c.lwsp	s8,92(sp)
20059b1a:	4ce6                	c.lwsp	s9,88(sp)
20059b1c:	4d56                	c.lwsp	s10,84(sp)
20059b1e:	4dc6                	c.lwsp	s11,80(sp)
20059b20:	6109                	c.addi16sp	sp,128
20059b22:	8082                	c.jr	ra
20059b24:	cb95                	c.beqz	a5,20059b58 <PSRAM_calibration+0x35c>
20059b26:	47b6                	c.lwsp	a5,76(sp)
20059b28:	4886                	c.lwsp	a7,64(sp)
20059b2a:	5872                	c.lwsp	a6,60(sp)
20059b2c:	c43e                	c.swsp	a5,8(sp)
20059b2e:	47a6                	c.lwsp	a5,72(sp)
20059b30:	200626b7          	lui	a3,0x20062
20059b34:	200625b7          	lui	a1,0x20062
20059b38:	c23e                	c.swsp	a5,4(sp)
20059b3a:	4796                	c.lwsp	a5,68(sp)
20059b3c:	876e                	c.mv	a4,s11
20059b3e:	07468693          	addi	a3,a3,116 # 20062074 <pmap_func+0x3cc>
20059b42:	c03e                	c.swsp	a5,0(sp)
20059b44:	57e2                	c.lwsp	a5,56(sp)
20059b46:	04900613          	addi	a2,zero,73
20059b4a:	e5458593          	addi	a1,a1,-428 # 20061e54 <pmap_func+0x1ac>
20059b4e:	4511                	c.li	a0,4
20059b50:	d67e                	c.swsp	t6,44(sp)
20059b52:	31e010ef          	jal	ra,2005ae70 <rtk_log_write>
20059b56:	5fb2                	c.lwsp	t6,44(sp)
20059b58:	57fd                	c.li	a5,-1
20059b5a:	ecfc89e3          	beq	s9,a5,20059a2c <PSRAM_calibration+0x230>
20059b5e:	0189d663          	bge	s3,s8,20059b6a <PSRAM_calibration+0x36e>
20059b62:	8952                	c.mv	s2,s4
20059b64:	89e2                	c.mv	s3,s8
20059b66:	8afe                	c.mv	s5,t6
20059b68:	84e6                	c.mv	s1,s9
20059b6a:	5ffd                	c.li	t6,-1
20059b6c:	4c01                	c.li	s8,0
20059b6e:	5cfd                	c.li	s9,-1
20059b70:	bd75                	c.j	20059a2c <PSRAM_calibration+0x230>

20059b72 <System_Reset>:
20059b72:	4100c7b7          	lui	a5,0x4100c
20059b76:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
20059b7a:	4709                	c.li	a4,2
20059b7c:	8399                	c.srli	a5,0x6
20059b7e:	8b8d                	c.andi	a5,3
20059b80:	00e78963          	beq	a5,a4,20059b92 <System_Reset+0x20>
20059b84:	4705                	c.li	a4,1
20059b86:	02e78963          	beq	a5,a4,20059bb8 <System_Reset+0x46>
20059b8a:	40000737          	lui	a4,0x40000
20059b8e:	c781                	c.beqz	a5,20059b96 <System_Reset+0x24>
20059b90:	8082                	c.jr	ra
20059b92:	20000737          	lui	a4,0x20000
20059b96:	969696b7          	lui	a3,0x96969
20059b9a:	4100c7b7          	lui	a5,0x4100c
20059b9e:	69668693          	addi	a3,a3,1686 # 96969696 <__ctrace_end__+0xcc969696>
20059ba2:	26d7a223          	sw	a3,612(a5) # 4100c264 <__km4_bd_boot_download_addr__+0x10ffa264>
20059ba6:	26e7a023          	sw	a4,608(a5)
20059baa:	69697737          	lui	a4,0x69697
20059bae:	96970713          	addi	a4,a4,-1687 # 69696969 <__kr4_psram_text_start__+0x9516949>
20059bb2:	26e7a223          	sw	a4,612(a5)
20059bb6:	bfe9                	c.j	20059b90 <System_Reset+0x1e>
20059bb8:	80000737          	lui	a4,0x80000
20059bbc:	bfe9                	c.j	20059b96 <System_Reset+0x24>

20059bbe <SWR_CORE_Vol_Get>:
20059bbe:	4100d7b7          	lui	a5,0x4100d
20059bc2:	f3e7c503          	lbu	a0,-194(a5) # 4100cf3e <__km4_bd_boot_download_addr__+0x10ffaf3e>
20059bc6:	0ff57513          	andi	a0,a0,255
20059bca:	8082                	c.jr	ra

20059bcc <wifi_set_user_config>:
20059bcc:	1141                	c.addi	sp,-16
20059bce:	c422                	c.swsp	s0,8(sp)
20059bd0:	03c00613          	addi	a2,zero,60
20059bd4:	42818413          	addi	s0,gp,1064 # 20066500 <wifi_user_config>
20059bd8:	4581                	c.li	a1,0
20059bda:	42818513          	addi	a0,gp,1064 # 20066500 <wifi_user_config>
20059bde:	c606                	c.swsp	ra,12(sp)
20059be0:	dffb0097          	auipc	ra,0xdffb0
20059be4:	eee080e7          	jalr	ra,-274(ra) # 9ace <__wrap_memset>
20059be8:	50800793          	addi	a5,zero,1288
20059bec:	00f41f23          	sh	a5,30(s0)
20059bf0:	4799                	c.li	a5,6
20059bf2:	d01c                	c.sw	a5,32(s0)
20059bf4:	47a1                	c.li	a5,8
20059bf6:	d05c                	c.sw	a5,36(s0)
20059bf8:	4785                	c.li	a5,1
20059bfa:	00f405a3          	sb	a5,11(s0)
20059bfe:	00f41623          	sh	a5,12(s0)
20059c02:	00f41423          	sh	a5,8(s0)
20059c06:	0b0107b7          	lui	a5,0xb010
20059c0a:	10178793          	addi	a5,a5,257 # b010101 <__ap_sram_heap_size+0xafd0101>
20059c0e:	c81c                	c.sw	a5,16(s0)
20059c10:	4789                	c.li	a5,2
20059c12:	cc1c                	c.sw	a5,24(s0)
20059c14:	00020737          	lui	a4,0x20
20059c18:	10100793          	addi	a5,zero,257
20059c1c:	02f41523          	sh	a5,42(s0)
20059c20:	02f41623          	sh	a5,44(s0)
20059c24:	20070713          	addi	a4,a4,512 # 20200 <__bdram_heap_buffer_size__+0x8300>
20059c28:	4791                	c.li	a5,4
20059c2a:	c018                	c.sw	a4,0(s0)
20059c2c:	02f40723          	sb	a5,46(s0)
20059c30:	02f40823          	sb	a5,48(s0)
20059c34:	070f0737          	lui	a4,0x70f0
20059c38:	6785                	c.lui	a5,0x1
20059c3a:	c058                	c.sw	a4,4(s0)
20059c3c:	38878793          	addi	a5,a5,904 # 1388 <CPU_ClkGet+0x872>
20059c40:	4751                	c.li	a4,20
20059c42:	40b2                	c.lwsp	ra,12(sp)
20059c44:	00040ea3          	sb	zero,29(s0)
20059c48:	00040a23          	sb	zero,20(s0)
20059c4c:	00041b23          	sh	zero,22(s0)
20059c50:	020404a3          	sb	zero,41(s0)
20059c54:	02e40ca3          	sb	a4,57(s0)
20059c58:	d85c                	c.sw	a5,52(s0)
20059c5a:	4422                	c.lwsp	s0,8(sp)
20059c5c:	0141                	c.addi	sp,16
20059c5e:	8082                	c.jr	ra

20059c60 <at_test>:
20059c60:	200625b7          	lui	a1,0x20062
20059c64:	20062537          	lui	a0,0x20062
20059c68:	0d058593          	addi	a1,a1,208 # 200620d0 <pmap_func+0x428>
20059c6c:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
20059c70:	3de0506f          	jal	zero,2005f04e <DiagPrintf_minimal>

20059c74 <at_otaclear>:
20059c74:	1141                	c.addi	sp,-16
20059c76:	c606                	c.swsp	ra,12(sp)
20059c78:	2c0030ef          	jal	ra,2005cf38 <sys_clear_ota_signature>
20059c7c:	40b2                	c.lwsp	ra,12(sp)
20059c7e:	200625b7          	lui	a1,0x20062
20059c82:	20062537          	lui	a0,0x20062
20059c86:	0e458593          	addi	a1,a1,228 # 200620e4 <pmap_func+0x43c>
20059c8a:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
20059c8e:	0141                	c.addi	sp,16
20059c90:	3be0506f          	jal	zero,2005f04e <DiagPrintf_minimal>

20059c94 <at_otarecover>:
20059c94:	1141                	c.addi	sp,-16
20059c96:	c606                	c.swsp	ra,12(sp)
20059c98:	3c2030ef          	jal	ra,2005d05a <sys_recover_ota_signature>
20059c9c:	40b2                	c.lwsp	ra,12(sp)
20059c9e:	200625b7          	lui	a1,0x20062
20059ca2:	20062537          	lui	a0,0x20062
20059ca6:	0f058593          	addi	a1,a1,240 # 200620f0 <pmap_func+0x448>
20059caa:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
20059cae:	0141                	c.addi	sp,16
20059cb0:	39e0506f          	jal	zero,2005f04e <DiagPrintf_minimal>

20059cb4 <at_rst>:
20059cb4:	200625b7          	lui	a1,0x20062
20059cb8:	20062537          	lui	a0,0x20062
20059cbc:	1141                	c.addi	sp,-16
20059cbe:	10058593          	addi	a1,a1,256 # 20062100 <pmap_func+0x458>
20059cc2:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
20059cc6:	c606                	c.swsp	ra,12(sp)
20059cc8:	386050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059ccc:	40b2                	c.lwsp	ra,12(sp)
20059cce:	0141                	c.addi	sp,16
20059cd0:	4c60306f          	jal	zero,2005d196 <sys_reset>

20059cd4 <at_state>:
20059cd4:	bf010113          	addi	sp,sp,-1040
20059cd8:	850a                	c.mv	a0,sp
20059cda:	40112623          	sw	ra,1036(sp)
20059cde:	40812423          	sw	s0,1032(sp)
20059ce2:	400040ef          	jal	ra,2005e0e2 <vTaskList>
20059ce6:	20062537          	lui	a0,0x20062
20059cea:	858a                	c.mv	a1,sp
20059cec:	10850513          	addi	a0,a0,264 # 20062108 <pmap_func+0x460>
20059cf0:	35e050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059cf4:	850a                	c.mv	a0,sp
20059cf6:	5cd000ef          	jal	ra,2005aac2 <vPortGetHeapStats>
20059cfa:	20062537          	lui	a0,0x20062
20059cfe:	11c50513          	addi	a0,a0,284 # 2006211c <pmap_func+0x474>
20059d02:	34c050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059d06:	ecc1a583          	lw	a1,-308(gp) # 20065fa4 <total_heap_size>
20059d0a:	20062537          	lui	a0,0x20062
20059d0e:	12c50513          	addi	a0,a0,300 # 2006212c <pmap_func+0x484>
20059d12:	33c050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059d16:	4582                	c.lwsp	a1,0(sp)
20059d18:	20062537          	lui	a0,0x20062
20059d1c:	14050513          	addi	a0,a0,320 # 20062140 <pmap_func+0x498>
20059d20:	32e050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059d24:	4782                	c.lwsp	a5,0(sp)
20059d26:	ecc1a583          	lw	a1,-308(gp) # 20065fa4 <total_heap_size>
20059d2a:	20062537          	lui	a0,0x20062
20059d2e:	15450513          	addi	a0,a0,340 # 20062154 <pmap_func+0x4ac>
20059d32:	8d9d                	c.sub	a1,a5
20059d34:	31a050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059d38:	47c2                	c.lwsp	a5,16(sp)
20059d3a:	ecc1a583          	lw	a1,-308(gp) # 20065fa4 <total_heap_size>
20059d3e:	20062537          	lui	a0,0x20062
20059d42:	16850513          	addi	a0,a0,360 # 20062168 <pmap_func+0x4c0>
20059d46:	8d9d                	c.sub	a1,a5
20059d48:	306050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059d4c:	200625b7          	lui	a1,0x20062
20059d50:	20062537          	lui	a0,0x20062
20059d54:	17c58593          	addi	a1,a1,380 # 2006217c <pmap_func+0x4d4>
20059d58:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
20059d5c:	2f2050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059d60:	40c12083          	lw	ra,1036(sp)
20059d64:	40812403          	lw	s0,1032(sp)
20059d68:	41010113          	addi	sp,sp,1040
20059d6c:	8082                	c.jr	ra

20059d6e <at_gmr>:
20059d6e:	715d                	c.addi16sp	sp,-80
20059d70:	40000513          	addi	a0,zero,1024
20059d74:	c686                	c.swsp	ra,76(sp)
20059d76:	c4a2                	c.swsp	s0,72(sp)
20059d78:	c2a6                	c.swsp	s1,68(sp)
20059d7a:	1c7010ef          	jal	ra,2005b740 <rtos_mem_malloc>
20059d7e:	3ff00593          	addi	a1,zero,1023
20059d82:	842a                	c.mv	s0,a0
20059d84:	200624b7          	lui	s1,0x20062
20059d88:	1b9040ef          	jal	ra,2005e740 <ChipInfo_GetSocName_ToBuf>
20059d8c:	85a2                	c.mv	a1,s0
20059d8e:	18448513          	addi	a0,s1,388 # 20062184 <pmap_func+0x4dc>
20059d92:	2bc050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059d96:	8522                	c.mv	a0,s0
20059d98:	3ff00593          	addi	a1,zero,1023
20059d9c:	261040ef          	jal	ra,2005e7fc <ChipInfo_GetLibVersion_ToBuf>
20059da0:	85a2                	c.mv	a1,s0
20059da2:	18448513          	addi	a0,s1,388
20059da6:	2a8050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059daa:	8522                	c.mv	a0,s0
20059dac:	1bf010ef          	jal	ra,2005b76a <rtos_mem_free>
20059db0:	200625b7          	lui	a1,0x20062
20059db4:	02000613          	addi	a2,zero,32
20059db8:	18858593          	addi	a1,a1,392 # 20062188 <pmap_func+0x4e0>
20059dbc:	850a                	c.mv	a0,sp
20059dbe:	0a7070ef          	jal	ra,20061664 <strncpy>
20059dc2:	200625b7          	lui	a1,0x20062
20059dc6:	02000613          	addi	a2,zero,32
20059dca:	19058593          	addi	a1,a1,400 # 20062190 <pmap_func+0x4e8>
20059dce:	1008                	c.addi4spn	a0,sp,32
20059dd0:	095070ef          	jal	ra,20061664 <strncpy>
20059dd4:	20062737          	lui	a4,0x20062
20059dd8:	200625b7          	lui	a1,0x20062
20059ddc:	20062537          	lui	a0,0x20062
20059de0:	1014                	c.addi4spn	a3,sp,32
20059de2:	860a                	c.mv	a2,sp
20059de4:	19870713          	addi	a4,a4,408 # 20062198 <pmap_func+0x4f0>
20059de8:	1ac58593          	addi	a1,a1,428 # 200621ac <pmap_func+0x504>
20059dec:	1b450513          	addi	a0,a0,436 # 200621b4 <pmap_func+0x50c>
20059df0:	25e050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059df4:	40b6                	c.lwsp	ra,76(sp)
20059df6:	4426                	c.lwsp	s0,72(sp)
20059df8:	4496                	c.lwsp	s1,68(sp)
20059dfa:	6161                	c.addi16sp	sp,80
20059dfc:	8082                	c.jr	ra

20059dfe <at_log>:
20059dfe:	7119                	c.addi16sp	sp,-128
20059e00:	daa6                	c.swsp	s1,116(sp)
20059e02:	05c00613          	addi	a2,zero,92
20059e06:	84aa                	c.mv	s1,a0
20059e08:	4581                	c.li	a1,0
20059e0a:	0048                	c.addi4spn	a0,sp,4
20059e0c:	dca2                	c.swsp	s0,120(sp)
20059e0e:	d8ca                	c.swsp	s2,112(sp)
20059e10:	de86                	c.swsp	ra,124(sp)
20059e12:	d6ce                	c.swsp	s3,108(sp)
20059e14:	20062437          	lui	s0,0x20062
20059e18:	dffb0097          	auipc	ra,0xdffb0
20059e1c:	cb6080e7          	jalr	ra,-842(ra) # 9ace <__wrap_memset>
20059e20:	20062937          	lui	s2,0x20062
20059e24:	e4a5                	c.bnez	s1,20059e8c <at_log+0x8e>
20059e26:	200626b7          	lui	a3,0x20062
20059e2a:	1c468693          	addi	a3,a3,452 # 200621c4 <pmap_func+0x51c>
20059e2e:	04100613          	addi	a2,zero,65
20059e32:	1d840593          	addi	a1,s0,472 # 200621d8 <pmap_func+0x530>
20059e36:	4505                	c.li	a0,1
20059e38:	038010ef          	jal	ra,2005ae70 <rtk_log_write>
20059e3c:	20062537          	lui	a0,0x20062
20059e40:	4605                	c.li	a2,1
20059e42:	31490593          	addi	a1,s2,788 # 20062314 <pmap_func+0x66c>
20059e46:	31c50513          	addi	a0,a0,796 # 2006231c <pmap_func+0x674>
20059e4a:	204050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059e4e:	20063537          	lui	a0,0x20063
20059e52:	94850513          	addi	a0,a0,-1720 # 20062948 <__FUNCTION__.0+0x208>
20059e56:	1f8050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059e5a:	20062537          	lui	a0,0x20062
20059e5e:	32c50513          	addi	a0,a0,812 # 2006232c <pmap_func+0x684>
20059e62:	1ec050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059e66:	20062537          	lui	a0,0x20062
20059e6a:	35850513          	addi	a0,a0,856 # 20062358 <pmap_func+0x6b0>
20059e6e:	1e0050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059e72:	20062537          	lui	a0,0x20062
20059e76:	39050513          	addi	a0,a0,912 # 20062390 <pmap_func+0x6e8>
20059e7a:	1d4050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059e7e:	20062537          	lui	a0,0x20062
20059e82:	3c450513          	addi	a0,a0,964 # 200623c4 <pmap_func+0x71c>
20059e86:	1c8050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059e8a:	a875                	c.j	20059f46 <at_log+0x148>
20059e8c:	8526                	c.mv	a0,s1
20059e8e:	004c                	c.addi4spn	a1,sp,4
20059e90:	1c2010ef          	jal	ra,2005b052 <parse_param>
20059e94:	4985                	c.li	s3,1
20059e96:	84aa                	c.mv	s1,a0
20059e98:	12a9db63          	bge	s3,a0,20059fce <at_log+0x1d0>
20059e9c:	4522                	c.lwsp	a0,8(sp)
20059e9e:	12050863          	beq	a0,zero,20059fce <at_log+0x1d0>
20059ea2:	5dc070ef          	jal	ra,2006147e <atoi>
20059ea6:	872a                	c.mv	a4,a0
20059ea8:	e555                	c.bnez	a0,20059f54 <at_log+0x156>
20059eaa:	478d                	c.li	a5,3
20059eac:	00f49463          	bne	s1,a5,20059eb4 <at_log+0xb6>
20059eb0:	4532                	c.lwsp	a0,12(sp)
20059eb2:	e511                	c.bnez	a0,20059ebe <at_log+0xc0>
20059eb4:	200626b7          	lui	a3,0x20062
20059eb8:	1dc68693          	addi	a3,a3,476 # 200621dc <pmap_func+0x534>
20059ebc:	bf8d                	c.j	20059e2e <at_log+0x30>
20059ebe:	577000ef          	jal	ra,2005ac34 <rtk_log_level_get>
20059ec2:	45b2                	c.lwsp	a1,12(sp)
20059ec4:	862a                	c.mv	a2,a0
20059ec6:	20062537          	lui	a0,0x20062
20059eca:	1fc50513          	addi	a0,a0,508 # 200621fc <pmap_func+0x554>
20059ece:	180050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059ed2:	200626b7          	lui	a3,0x20062
20059ed6:	29068693          	addi	a3,a3,656 # 20062290 <pmap_func+0x5e8>
20059eda:	04100613          	addi	a2,zero,65
20059ede:	1d840593          	addi	a1,s0,472
20059ee2:	4505                	c.li	a0,1
20059ee4:	78d000ef          	jal	ra,2005ae70 <rtk_log_write>
20059ee8:	200626b7          	lui	a3,0x20062
20059eec:	2ac68693          	addi	a3,a3,684 # 200622ac <pmap_func+0x604>
20059ef0:	04500613          	addi	a2,zero,69
20059ef4:	1d840593          	addi	a1,s0,472
20059ef8:	4509                	c.li	a0,2
20059efa:	777000ef          	jal	ra,2005ae70 <rtk_log_write>
20059efe:	200626b7          	lui	a3,0x20062
20059f02:	2c868693          	addi	a3,a3,712 # 200622c8 <pmap_func+0x620>
20059f06:	05700613          	addi	a2,zero,87
20059f0a:	1d840593          	addi	a1,s0,472
20059f0e:	450d                	c.li	a0,3
20059f10:	761000ef          	jal	ra,2005ae70 <rtk_log_write>
20059f14:	200626b7          	lui	a3,0x20062
20059f18:	1d840593          	addi	a1,s0,472
20059f1c:	2e468693          	addi	a3,a3,740 # 200622e4 <pmap_func+0x63c>
20059f20:	04900613          	addi	a2,zero,73
20059f24:	4511                	c.li	a0,4
20059f26:	74b000ef          	jal	ra,2005ae70 <rtk_log_write>
20059f2a:	20062537          	lui	a0,0x20062
20059f2e:	2fc50513          	addi	a0,a0,764 # 200622fc <pmap_func+0x654>
20059f32:	11c050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059f36:	20062537          	lui	a0,0x20062
20059f3a:	31490593          	addi	a1,s2,788
20059f3e:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
20059f42:	10c050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20059f46:	50f6                	c.lwsp	ra,124(sp)
20059f48:	5466                	c.lwsp	s0,120(sp)
20059f4a:	54d6                	c.lwsp	s1,116(sp)
20059f4c:	5946                	c.lwsp	s2,112(sp)
20059f4e:	59b6                	c.lwsp	s3,108(sp)
20059f50:	6109                	c.addi16sp	sp,128
20059f52:	8082                	c.jr	ra
20059f54:	05351663          	bne	a0,s3,20059fa0 <at_log+0x1a2>
20059f58:	4791                	c.li	a5,4
20059f5a:	00f49663          	bne	s1,a5,20059f66 <at_log+0x168>
20059f5e:	47b2                	c.lwsp	a5,12(sp)
20059f60:	c399                	c.beqz	a5,20059f66 <at_log+0x168>
20059f62:	4542                	c.lwsp	a0,16(sp)
20059f64:	e511                	c.bnez	a0,20059f70 <at_log+0x172>
20059f66:	200626b7          	lui	a3,0x20062
20059f6a:	21068693          	addi	a3,a3,528 # 20062210 <pmap_func+0x568>
20059f6e:	b5c1                	c.j	20059e2e <at_log+0x30>
20059f70:	50e070ef          	jal	ra,2006147e <atoi>
20059f74:	4795                	c.li	a5,5
20059f76:	872a                	c.mv	a4,a0
20059f78:	00a7fe63          	bgeu	a5,a0,20059f94 <at_log+0x196>
20059f7c:	200626b7          	lui	a3,0x20062
20059f80:	23068693          	addi	a3,a3,560 # 20062230 <pmap_func+0x588>
20059f84:	04100613          	addi	a2,zero,65
20059f88:	1d840593          	addi	a1,s0,472
20059f8c:	4505                	c.li	a0,1
20059f8e:	6e3000ef          	jal	ra,2005ae70 <rtk_log_write>
20059f92:	b56d                	c.j	20059e3c <at_log+0x3e>
20059f94:	0ff57593          	andi	a1,a0,255
20059f98:	4532                	c.lwsp	a0,12(sp)
20059f9a:	517000ef          	jal	ra,2005acb0 <rtk_log_level_set>
20059f9e:	bf15                	c.j	20059ed2 <at_log+0xd4>
20059fa0:	4789                	c.li	a5,2
20059fa2:	00f51e63          	bne	a0,a5,20059fbe <at_log+0x1c0>
20059fa6:	00a48763          	beq	s1,a0,20059fb4 <at_log+0x1b6>
20059faa:	200626b7          	lui	a3,0x20062
20059fae:	25068693          	addi	a3,a3,592 # 20062250 <pmap_func+0x5a8>
20059fb2:	bdb5                	c.j	20059e2e <at_log+0x30>
20059fb4:	49418513          	addi	a0,gp,1172 # 2006656c <rtk_log_tag_array>
20059fb8:	409000ef          	jal	ra,2005abc0 <rtk_log_array_print>
20059fbc:	bf19                	c.j	20059ed2 <at_log+0xd4>
20059fbe:	468d                	c.li	a3,3
20059fc0:	00d51863          	bne	a0,a3,20059fd0 <at_log+0x1d2>
20059fc4:	fef493e3          	bne	s1,a5,20059faa <at_log+0x1ac>
20059fc8:	44d000ef          	jal	ra,2005ac14 <rtk_log_array_clear>
20059fcc:	b719                	c.j	20059ed2 <at_log+0xd4>
20059fce:	577d                	c.li	a4,-1
20059fd0:	200626b7          	lui	a3,0x20062
20059fd4:	27468693          	addi	a3,a3,628 # 20062274 <pmap_func+0x5cc>
20059fd8:	b775                	c.j	20059f84 <at_log+0x186>

20059fda <at_rreg>:
20059fda:	7159                	c.addi16sp	sp,-112
20059fdc:	d4a2                	c.swsp	s0,104(sp)
20059fde:	05c00613          	addi	a2,zero,92
20059fe2:	842a                	c.mv	s0,a0
20059fe4:	4581                	c.li	a1,0
20059fe6:	0048                	c.addi4spn	a0,sp,4
20059fe8:	d2a6                	c.swsp	s1,100(sp)
20059fea:	d686                	c.swsp	ra,108(sp)
20059fec:	200624b7          	lui	s1,0x20062
20059ff0:	dffb0097          	auipc	ra,0xdffb0
20059ff4:	ade080e7          	jalr	ra,-1314(ra) # 9ace <__wrap_memset>
20059ff8:	ec05                	c.bnez	s0,2005a030 <at_rreg+0x56>
20059ffa:	200626b7          	lui	a3,0x20062
20059ffe:	200625b7          	lui	a1,0x20062
2005a002:	05700613          	addi	a2,zero,87
2005a006:	1d858593          	addi	a1,a1,472 # 200621d8 <pmap_func+0x530>
2005a00a:	450d                	c.li	a0,3
2005a00c:	3f868693          	addi	a3,a3,1016 # 200623f8 <pmap_func+0x750>
2005a010:	661000ef          	jal	ra,2005ae70 <rtk_log_write>
2005a014:	20062537          	lui	a0,0x20062
2005a018:	4605                	c.li	a2,1
2005a01a:	41448593          	addi	a1,s1,1044 # 20062414 <pmap_func+0x76c>
2005a01e:	31c50513          	addi	a0,a0,796 # 2006231c <pmap_func+0x674>
2005a022:	02c050ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a026:	50b6                	c.lwsp	ra,108(sp)
2005a028:	5426                	c.lwsp	s0,104(sp)
2005a02a:	5496                	c.lwsp	s1,100(sp)
2005a02c:	6165                	c.addi16sp	sp,112
2005a02e:	8082                	c.jr	ra
2005a030:	004c                	c.addi4spn	a1,sp,4
2005a032:	8522                	c.mv	a0,s0
2005a034:	01e010ef          	jal	ra,2005b052 <parse_param>
2005a038:	ffe50713          	addi	a4,a0,-2
2005a03c:	4789                	c.li	a5,2
2005a03e:	fae7eee3          	bltu	a5,a4,20059ffa <at_rreg+0x20>
2005a042:	157d                	c.addi	a0,-1
2005a044:	0542                	c.slli	a0,0x10
2005a046:	002c                	c.addi4spn	a1,sp,8
2005a048:	8141                	c.srli	a0,0x10
2005a04a:	514010ef          	jal	ra,2005b55e <cmd_dump_word>
2005a04e:	20062537          	lui	a0,0x20062
2005a052:	41448593          	addi	a1,s1,1044
2005a056:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
2005a05a:	7f5040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a05e:	b7e1                	c.j	2005a026 <at_rreg+0x4c>

2005a060 <at_wreg>:
2005a060:	7159                	c.addi16sp	sp,-112
2005a062:	d4a2                	c.swsp	s0,104(sp)
2005a064:	05c00613          	addi	a2,zero,92
2005a068:	842a                	c.mv	s0,a0
2005a06a:	4581                	c.li	a1,0
2005a06c:	0048                	c.addi4spn	a0,sp,4
2005a06e:	d2a6                	c.swsp	s1,100(sp)
2005a070:	d686                	c.swsp	ra,108(sp)
2005a072:	200624b7          	lui	s1,0x20062
2005a076:	dffb0097          	auipc	ra,0xdffb0
2005a07a:	a58080e7          	jalr	ra,-1448(ra) # 9ace <__wrap_memset>
2005a07e:	ec05                	c.bnez	s0,2005a0b6 <at_wreg+0x56>
2005a080:	200626b7          	lui	a3,0x20062
2005a084:	200625b7          	lui	a1,0x20062
2005a088:	05700613          	addi	a2,zero,87
2005a08c:	1d858593          	addi	a1,a1,472 # 200621d8 <pmap_func+0x530>
2005a090:	450d                	c.li	a0,3
2005a092:	41c68693          	addi	a3,a3,1052 # 2006241c <pmap_func+0x774>
2005a096:	5db000ef          	jal	ra,2005ae70 <rtk_log_write>
2005a09a:	20062537          	lui	a0,0x20062
2005a09e:	4605                	c.li	a2,1
2005a0a0:	43848593          	addi	a1,s1,1080 # 20062438 <pmap_func+0x790>
2005a0a4:	31c50513          	addi	a0,a0,796 # 2006231c <pmap_func+0x674>
2005a0a8:	7a7040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a0ac:	50b6                	c.lwsp	ra,108(sp)
2005a0ae:	5426                	c.lwsp	s0,104(sp)
2005a0b0:	5496                	c.lwsp	s1,100(sp)
2005a0b2:	6165                	c.addi16sp	sp,112
2005a0b4:	8082                	c.jr	ra
2005a0b6:	004c                	c.addi4spn	a1,sp,4
2005a0b8:	8522                	c.mv	a0,s0
2005a0ba:	799000ef          	jal	ra,2005b052 <parse_param>
2005a0be:	478d                	c.li	a5,3
2005a0c0:	fcf510e3          	bne	a0,a5,2005a080 <at_wreg+0x20>
2005a0c4:	45a2                	c.lwsp	a1,8(sp)
2005a0c6:	4509                	c.li	a0,2
2005a0c8:	3f0010ef          	jal	ra,2005b4b8 <cmd_write_word>
2005a0cc:	20062537          	lui	a0,0x20062
2005a0d0:	43848593          	addi	a1,s1,1080
2005a0d4:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
2005a0d8:	777040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a0dc:	bfc1                	c.j	2005a0ac <at_wreg+0x4c>

2005a0de <find_status>:
2005a0de:	872a                	c.mv	a4,a0
2005a0e0:	4781                	c.li	a5,0
2005a0e2:	00c7c463          	blt	a5,a2,2005a0ea <find_status+0xc>
2005a0e6:	4501                	c.li	a0,0
2005a0e8:	8082                	c.jr	ra
2005a0ea:	852e                	c.mv	a0,a1
2005a0ec:	4594                	c.lw	a3,8(a1)
2005a0ee:	02458593          	addi	a1,a1,36
2005a0f2:	fee68be3          	beq	a3,a4,2005a0e8 <find_status+0xa>
2005a0f6:	0785                	c.addi	a5,1
2005a0f8:	b7ed                	c.j	2005a0e2 <find_status+0x4>

2005a0fa <find_free_delta>:
2005a0fa:	87aa                	c.mv	a5,a0
2005a0fc:	4701                	c.li	a4,0
2005a0fe:	00b74463          	blt	a4,a1,2005a106 <find_free_delta+0xc>
2005a102:	4501                	c.li	a0,0
2005a104:	8082                	c.jr	ra
2005a106:	853e                	c.mv	a0,a5
2005a108:	5794                	c.lw	a3,40(a5)
2005a10a:	03878793          	addi	a5,a5,56
2005a10e:	dafd                	c.beqz	a3,2005a104 <find_free_delta+0xa>
2005a110:	0705                	c.addi	a4,1
2005a112:	b7f5                	c.j	2005a0fe <find_free_delta+0x4>

2005a114 <update_status>:
2005a114:	7179                	c.addi16sp	sp,-48
2005a116:	d226                	c.swsp	s1,36(sp)
2005a118:	ec41a783          	lw	a5,-316(gp) # 20065f9c <task_status>
2005a11c:	6705                	c.lui	a4,0x1
2005a11e:	d606                	c.swsp	ra,44(sp)
2005a120:	d422                	c.swsp	s0,40(sp)
2005a122:	d04a                	c.swsp	s2,32(sp)
2005a124:	ce4e                	c.swsp	s3,28(sp)
2005a126:	cc52                	c.swsp	s4,24(sp)
2005a128:	ca56                	c.swsp	s5,20(sp)
2005a12a:	c85a                	c.swsp	s6,16(sp)
2005a12c:	c65e                	c.swsp	s7,12(sp)
2005a12e:	c462                	c.swsp	s8,8(sp)
2005a130:	00e786b3          	add	a3,a5,a4
2005a134:	2006a603          	lw	a2,512(a3)
2005a138:	ec418493          	addi	s1,gp,-316 # 20065f9c <task_status>
2005a13c:	ea19                	c.bnez	a2,2005a152 <update_status+0x3e>
2005a13e:	2046a603          	lw	a2,516(a3)
2005a142:	ea01                	c.bnez	a2,2005a152 <update_status+0x3e>
2005a144:	90070713          	addi	a4,a4,-1792 # 900 <BKUP_Set+0x5a>
2005a148:	973e                	c.add	a4,a5
2005a14a:	20f6a023          	sw	a5,512(a3)
2005a14e:	20e6a223          	sw	a4,516(a3)
2005a152:	6405                	c.lui	s0,0x1
2005a154:	97a2                	c.add	a5,s0
2005a156:	20078793          	addi	a5,a5,512
2005a15a:	47d8                	c.lw	a4,12(a5)
2005a15c:	43d4                	c.lw	a3,4(a5)
2005a15e:	21040913          	addi	s2,s0,528 # 1210 <CPU_ClkGet+0x6fa>
2005a162:	c798                	c.sw	a4,8(a5)
2005a164:	4398                	c.lw	a4,0(a5)
2005a166:	c394                	c.sw	a3,0(a5)
2005a168:	4a01                	c.li	s4,0
2005a16a:	c3d8                	c.sw	a4,4(a5)
2005a16c:	2d8030ef          	jal	ra,2005d444 <uxTaskGetNumberOfTasks>
2005a170:	409c                	c.lw	a5,0(s1)
2005a172:	85aa                	c.mv	a1,a0
2005a174:	4601                	c.li	a2,0
2005a176:	97a2                	c.add	a5,s0
2005a178:	20a7a623          	sw	a0,524(a5)
2005a17c:	2047a503          	lw	a0,516(a5)
2005a180:	623030ef          	jal	ra,2005dfa2 <uxTaskGetSystemState>
2005a184:	409c                	c.lw	a5,0(s1)
2005a186:	e0040613          	addi	a2,s0,-512
2005a18a:	4581                	c.li	a1,0
2005a18c:	00878733          	add	a4,a5,s0
2005a190:	20a72623          	sw	a0,524(a4)
2005a194:	01278533          	add	a0,a5,s2
2005a198:	dffb0097          	auipc	ra,0xdffb0
2005a19c:	936080e7          	jalr	ra,-1738(ra) # 9ace <__wrap_memset>
2005a1a0:	0004aa83          	lw	s5,0(s1)
2005a1a4:	9456                	c.add	s0,s5
2005a1a6:	20842783          	lw	a5,520(s0)
2005a1aa:	00f05963          	bge	zero,a5,2005a1bc <update_status+0xa8>
2005a1ae:	02400c13          	addi	s8,zero,36
2005a1b2:	9aca                	c.add	s5,s2
2005a1b4:	20c42783          	lw	a5,524(s0)
2005a1b8:	00fa4e63          	blt	s4,a5,2005a1d4 <update_status+0xc0>
2005a1bc:	50b2                	c.lwsp	ra,44(sp)
2005a1be:	5422                	c.lwsp	s0,40(sp)
2005a1c0:	5492                	c.lwsp	s1,36(sp)
2005a1c2:	5902                	c.lwsp	s2,32(sp)
2005a1c4:	49f2                	c.lwsp	s3,28(sp)
2005a1c6:	4a62                	c.lwsp	s4,24(sp)
2005a1c8:	4ad2                	c.lwsp	s5,20(sp)
2005a1ca:	4b42                	c.lwsp	s6,16(sp)
2005a1cc:	4bb2                	c.lwsp	s7,12(sp)
2005a1ce:	4c22                	c.lwsp	s8,8(sp)
2005a1d0:	6145                	c.addi16sp	sp,48
2005a1d2:	8082                	c.jr	ra
2005a1d4:	038a0933          	mul	s2,s4,s8
2005a1d8:	20442b83          	lw	s7,516(s0)
2005a1dc:	20842603          	lw	a2,520(s0)
2005a1e0:	20042583          	lw	a1,512(s0)
2005a1e4:	9bca                	c.add	s7,s2
2005a1e6:	008ba503          	lw	a0,8(s7)
2005a1ea:	3dd5                	c.jal	2005a0de <find_status>
2005a1ec:	04000593          	addi	a1,zero,64
2005a1f0:	8b2a                	c.mv	s6,a0
2005a1f2:	8556                	c.mv	a0,s5
2005a1f4:	3719                	c.jal	2005a0fa <find_free_delta>
2005a1f6:	000ba783          	lw	a5,0(s7)
2005a1fa:	84aa                	c.mv	s1,a0
2005a1fc:	467d                	c.li	a2,31
2005a1fe:	d51c                	c.sw	a5,40(a0)
2005a200:	20442783          	lw	a5,516(s0)
2005a204:	97ca                	c.add	a5,s2
2005a206:	43cc                	c.lw	a1,4(a5)
2005a208:	c14c                	c.sw	a1,4(a0)
2005a20a:	20442783          	lw	a5,516(s0)
2005a20e:	97ca                	c.add	a5,s2
2005a210:	479c                	c.lw	a5,8(a5)
2005a212:	00052023          	sw	zero,0(a0)
2005a216:	0521                	c.addi	a0,8
2005a218:	d15c                	c.sw	a5,36(a0)
2005a21a:	20442783          	lw	a5,516(s0)
2005a21e:	97ca                	c.add	a5,s2
2005a220:	00c7c783          	lbu	a5,12(a5)
2005a224:	02f50623          	sb	a5,44(a0)
2005a228:	20442783          	lw	a5,516(s0)
2005a22c:	97ca                	c.add	a5,s2
2005a22e:	4b9c                	c.lw	a5,16(a5)
2005a230:	d51c                	c.sw	a5,40(a0)
2005a232:	432070ef          	jal	ra,20061664 <strncpy>
2005a236:	c095                	c.beqz	s1,2005a25a <update_status+0x146>
2005a238:	20442783          	lw	a5,516(s0)
2005a23c:	97ca                	c.add	a5,s2
2005a23e:	4f9c                	c.lw	a5,24(a5)
2005a240:	000b0563          	beq	s6,zero,2005a24a <update_status+0x136>
2005a244:	018b2703          	lw	a4,24(s6)
2005a248:	8f99                	c.sub	a5,a4
2005a24a:	c09c                	c.sw	a5,0(s1)
2005a24c:	20442783          	lw	a5,516(s0)
2005a250:	993e                	c.add	s2,a5
2005a252:	00c94783          	lbu	a5,12(s2)
2005a256:	02f48a23          	sb	a5,52(s1)
2005a25a:	0a05                	c.addi	s4,1
2005a25c:	bfa1                	c.j	2005a1b4 <update_status+0xa0>

2005a25e <print_delta>:
2005a25e:	715d                	c.addi16sp	sp,-80
2005a260:	736277b7          	lui	a5,0x73627
2005a264:	c4a2                	c.swsp	s0,72(sp)
2005a266:	25278793          	addi	a5,a5,594 # 73627252 <__kr4_psram_text_start__+0x134a7232>
2005a26a:	842a                	c.mv	s0,a0
2005a26c:	20062737          	lui	a4,0x20062
2005a270:	200626b7          	lui	a3,0x20062
2005a274:	20062637          	lui	a2,0x20062
2005a278:	200625b7          	lui	a1,0x20062
2005a27c:	20062537          	lui	a0,0x20062
2005a280:	c43e                	c.swsp	a5,8(sp)
2005a282:	44070713          	addi	a4,a4,1088 # 20062440 <pmap_func+0x798>
2005a286:	04400793          	addi	a5,zero,68
2005a28a:	44868693          	addi	a3,a3,1096 # 20062448 <pmap_func+0x7a0>
2005a28e:	45060613          	addi	a2,a2,1104 # 20062450 <pmap_func+0x7a8>
2005a292:	45858593          	addi	a1,a1,1112 # 20062458 <pmap_func+0x7b0>
2005a296:	46450513          	addi	a0,a0,1124 # 20062464 <pmap_func+0x7bc>
2005a29a:	c2a6                	c.swsp	s1,68(sp)
2005a29c:	c0ca                	c.swsp	s2,64(sp)
2005a29e:	de4e                	c.swsp	s3,60(sp)
2005a2a0:	dc52                	c.swsp	s4,56(sp)
2005a2a2:	da56                	c.swsp	s5,52(sp)
2005a2a4:	d85a                	c.swsp	s6,48(sp)
2005a2a6:	d65e                	c.swsp	s7,44(sp)
2005a2a8:	ee22                	c.fswsp	fs0,28(sp)
2005a2aa:	00f10623          	sb	a5,12(sp)
2005a2ae:	c686                	c.swsp	ra,76(sp)
2005a2b0:	00011223          	sh	zero,4(sp)
2005a2b4:	00010323          	sb	zero,6(sp)
2005a2b8:	597040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a2bc:	e681a407          	flw	fs0,-408(gp) # 20065f40 <__sdata_start__>
2005a2c0:	6485                	c.lui	s1,0x1
2005a2c2:	4b81                	c.li	s7,0
2005a2c4:	03800993          	addi	s3,zero,56
2005a2c8:	06400b13          	addi	s6,zero,100
2005a2cc:	21848a13          	addi	s4,s1,536 # 1218 <CPU_ClkGet+0x702>
2005a2d0:	20062ab7          	lui	s5,0x20062
2005a2d4:	460d                	c.li	a2,3
2005a2d6:	4581                	c.li	a1,0
2005a2d8:	0048                	c.addi4spn	a0,sp,4
2005a2da:	dffaf097          	auipc	ra,0xdffaf
2005a2de:	7f4080e7          	jalr	ra,2036(ra) # 9ace <__wrap_memset>
2005a2e2:	033b8633          	mul	a2,s7,s3
2005a2e6:	ec41a803          	lw	a6,-316(gp) # 20065f9c <task_status>
2005a2ea:	d0047753          	fcvt.s.w	fa4,s0
2005a2ee:	4529                	c.li	a0,10
2005a2f0:	00c806b3          	add	a3,a6,a2
2005a2f4:	96a6                	c.add	a3,s1
2005a2f6:	2106a783          	lw	a5,528(a3)
2005a2fa:	d007f7d3          	fcvt.s.w	fa5,a5
2005a2fe:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a302:	18e7f7d3          	fdiv.s	fa5,fa5,fa4
2005a306:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a30a:	c0079753          	fcvt.w.s	a4,fa5,rtz
2005a30e:	036767b3          	rem	a5,a4,s6
2005a312:	02a787b3          	mul	a5,a5,a0
2005a316:	0367c5b3          	div	a1,a5,s6
2005a31a:	0367e7b3          	rem	a5,a5,s6
2005a31e:	03058593          	addi	a1,a1,48
2005a322:	00b10223          	sb	a1,4(sp)
2005a326:	02a787b3          	mul	a5,a5,a0
2005a32a:	0367c7b3          	div	a5,a5,s6
2005a32e:	03078793          	addi	a5,a5,48
2005a332:	00f102a3          	sb	a5,5(sp)
2005a336:	2386a783          	lw	a5,568(a3)
2005a33a:	c78d                	c.beqz	a5,2005a364 <print_delta+0x106>
2005a33c:	03674733          	div	a4,a4,s6
2005a340:	2446c503          	lbu	a0,580(a3)
2005a344:	014605b3          	add	a1,a2,s4
2005a348:	2406a683          	lw	a3,576(a3)
2005a34c:	01050793          	addi	a5,a0,16
2005a350:	00278533          	add	a0,a5,sp
2005a354:	ff854603          	lbu	a2,-8(a0)
2005a358:	005c                	c.addi4spn	a5,sp,4
2005a35a:	95c2                	c.add	a1,a6
2005a35c:	474a8513          	addi	a0,s5,1140 # 20062474 <pmap_func+0x7cc>
2005a360:	4ef040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a364:	0b85                	c.addi	s7,1
2005a366:	04000793          	addi	a5,zero,64
2005a36a:	f6fb95e3          	bne	s7,a5,2005a2d4 <print_delta+0x76>
2005a36e:	40b6                	c.lwsp	ra,76(sp)
2005a370:	4426                	c.lwsp	s0,72(sp)
2005a372:	4496                	c.lwsp	s1,68(sp)
2005a374:	4906                	c.lwsp	s2,64(sp)
2005a376:	59f2                	c.lwsp	s3,60(sp)
2005a378:	5a62                	c.lwsp	s4,56(sp)
2005a37a:	5ad2                	c.lwsp	s5,52(sp)
2005a37c:	5b42                	c.lwsp	s6,48(sp)
2005a37e:	5bb2                	c.lwsp	s7,44(sp)
2005a380:	6472                	c.flwsp	fs0,28(sp)
2005a382:	6161                	c.addi16sp	sp,80
2005a384:	8082                	c.jr	ra

2005a386 <cpu_stat_thread>:
2005a386:	7179                	c.addi16sp	sp,-48
2005a388:	c62a                	c.swsp	a0,12(sp)
2005a38a:	4521                	c.li	a0,8
2005a38c:	d226                	c.swsp	s1,36(sp)
2005a38e:	d606                	c.swsp	ra,44(sp)
2005a390:	d422                	c.swsp	s0,40(sp)
2005a392:	d04a                	c.swsp	s2,32(sp)
2005a394:	ce4e                	c.swsp	s3,28(sp)
2005a396:	cc52                	c.swsp	s4,24(sp)
2005a398:	ca56                	c.swsp	s5,20(sp)
2005a39a:	3a6010ef          	jal	ra,2005b740 <rtos_mem_malloc>
2005a39e:	45b2                	c.lwsp	a1,12(sp)
2005a3a0:	ec818493          	addi	s1,gp,-312 # 20065fa0 <top_exit_sema>
2005a3a4:	e131                	c.bnez	a0,2005a3e8 <cpu_stat_thread+0x62>
2005a3a6:	200625b7          	lui	a1,0x20062
2005a3aa:	20062537          	lui	a0,0x20062
2005a3ae:	74058593          	addi	a1,a1,1856 # 20062740 <__FUNCTION__.0>
2005a3b2:	48850513          	addi	a0,a0,1160 # 20062488 <pmap_func+0x7e0>
2005a3b6:	499040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a3ba:	4088                	c.lw	a0,0(s1)
2005a3bc:	ec418413          	addi	s0,gp,-316 # 20065f9c <task_status>
2005a3c0:	498010ef          	jal	ra,2005b858 <rtos_sema_delete>
2005a3c4:	4008                	c.lw	a0,0(s0)
2005a3c6:	0004a023          	sw	zero,0(s1)
2005a3ca:	3a0010ef          	jal	ra,2005b76a <rtos_mem_free>
2005a3ce:	00042023          	sw	zero,0(s0)
2005a3d2:	5422                	c.lwsp	s0,40(sp)
2005a3d4:	50b2                	c.lwsp	ra,44(sp)
2005a3d6:	5492                	c.lwsp	s1,36(sp)
2005a3d8:	5902                	c.lwsp	s2,32(sp)
2005a3da:	49f2                	c.lwsp	s3,28(sp)
2005a3dc:	4a62                	c.lwsp	s4,24(sp)
2005a3de:	4ad2                	c.lwsp	s5,20(sp)
2005a3e0:	4501                	c.li	a0,0
2005a3e2:	6145                	c.addi16sp	sp,48
2005a3e4:	7000106f          	jal	zero,2005bae4 <rtos_task_delete>
2005a3e8:	4621                	c.li	a2,8
2005a3ea:	842a                	c.mv	s0,a0
2005a3ec:	dffaf097          	auipc	ra,0xdffaf
2005a3f0:	6e4080e7          	jalr	ra,1764(ra) # 9ad0 <__wrap_memcpy>
2005a3f4:	410187b7          	lui	a5,0x41018
2005a3f8:	4bdc                	c.lw	a5,20(a5)
2005a3fa:	3e800a93          	addi	s5,zero,1000
2005a3fe:	eaf1ac23          	sw	a5,-328(gp) # 20065f90 <last_tick>
2005a402:	5a7d                	c.li	s4,-1
2005a404:	eb818913          	addi	s2,gp,-328 # 20065f90 <last_tick>
2005a408:	410189b7          	lui	s3,0x41018
2005a40c:	400c                	c.lw	a1,0(s0)
2005a40e:	4088                	c.lw	a0,0(s1)
2005a410:	02ba85b3          	mul	a1,s5,a1
2005a414:	45a010ef          	jal	ra,2005b86e <rtos_sema_take>
2005a418:	03451463          	bne	a0,s4,2005a440 <cpu_stat_thread+0xba>
2005a41c:	39e5                	c.jal	2005a114 <update_status>
2005a41e:	0149a503          	lw	a0,20(s3) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a422:	00092783          	lw	a5,0(s2)
2005a426:	0149a703          	lw	a4,20(s3)
2005a42a:	8d1d                	c.sub	a0,a5
2005a42c:	00e92023          	sw	a4,0(s2)
2005a430:	353d                	c.jal	2005a25e <print_delta>
2005a432:	405c                	c.lw	a5,4(s0)
2005a434:	00f05563          	bge	zero,a5,2005a43e <cpu_stat_thread+0xb8>
2005a438:	17fd                	c.addi	a5,-1
2005a43a:	c05c                	c.sw	a5,4(s0)
2005a43c:	bfc1                	c.j	2005a40c <cpu_stat_thread+0x86>
2005a43e:	f7f9                	c.bnez	a5,2005a40c <cpu_stat_thread+0x86>
2005a440:	8522                	c.mv	a0,s0
2005a442:	328010ef          	jal	ra,2005b76a <rtos_mem_free>
2005a446:	bf95                	c.j	2005a3ba <cpu_stat_thread+0x34>

2005a448 <at_cpuload>:
2005a448:	7119                	c.addi16sp	sp,-128
2005a44a:	dca2                	c.swsp	s0,120(sp)
2005a44c:	05c00613          	addi	a2,zero,92
2005a450:	842a                	c.mv	s0,a0
2005a452:	4581                	c.li	a1,0
2005a454:	0048                	c.addi4spn	a0,sp,4
2005a456:	de86                	c.swsp	ra,124(sp)
2005a458:	d8ca                	c.swsp	s2,112(sp)
2005a45a:	d4d2                	c.swsp	s4,104(sp)
2005a45c:	daa6                	c.swsp	s1,116(sp)
2005a45e:	d6ce                	c.swsp	s3,108(sp)
2005a460:	d2d6                	c.swsp	s5,100(sp)
2005a462:	dffaf097          	auipc	ra,0xdffaf
2005a466:	66c080e7          	jalr	ra,1644(ra) # 9ace <__wrap_memset>
2005a46a:	ebc18793          	addi	a5,gp,-324 # 20065f94 <para_in>
2005a46e:	4705                	c.li	a4,1
2005a470:	c398                	c.sw	a4,0(a5)
2005a472:	004c                	c.addi4spn	a1,sp,4
2005a474:	577d                	c.li	a4,-1
2005a476:	8522                	c.mv	a0,s0
2005a478:	c3d8                	c.sw	a4,4(a5)
2005a47a:	3d9000ef          	jal	ra,2005b052 <parse_param>
2005a47e:	ffe50693          	addi	a3,a0,-2
2005a482:	4709                	c.li	a4,2
2005a484:	20062a37          	lui	s4,0x20062
2005a488:	06d77463          	bgeu	a4,a3,2005a4f0 <at_cpuload+0xa8>
2005a48c:	20062537          	lui	a0,0x20062
2005a490:	49c50513          	addi	a0,a0,1180 # 2006249c <pmap_func+0x7f4>
2005a494:	3bb040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a498:	4405                	c.li	s0,1
2005a49a:	20062537          	lui	a0,0x20062
2005a49e:	8622                	c.mv	a2,s0
2005a4a0:	628a0593          	addi	a1,s4,1576 # 20062628 <pmap_func+0x980>
2005a4a4:	31c50513          	addi	a0,a0,796 # 2006231c <pmap_func+0x674>
2005a4a8:	9875                	c.andi	s0,-3
2005a4aa:	3a5040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a4ae:	1a040163          	beq	s0,zero,2005a650 <at_cpuload+0x208>
2005a4b2:	20063537          	lui	a0,0x20063
2005a4b6:	94850513          	addi	a0,a0,-1720 # 20062948 <__FUNCTION__.0+0x208>
2005a4ba:	395040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a4be:	20062537          	lui	a0,0x20062
2005a4c2:	63450513          	addi	a0,a0,1588 # 20062634 <pmap_func+0x98c>
2005a4c6:	389040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a4ca:	20062537          	lui	a0,0x20062
2005a4ce:	66450513          	addi	a0,a0,1636 # 20062664 <pmap_func+0x9bc>
2005a4d2:	37d040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a4d6:	20062537          	lui	a0,0x20062
2005a4da:	67850513          	addi	a0,a0,1656 # 20062678 <pmap_func+0x9d0>
2005a4de:	371040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a4e2:	20062537          	lui	a0,0x20062
2005a4e6:	69450513          	addi	a0,a0,1684 # 20062694 <pmap_func+0x9ec>
2005a4ea:	365040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a4ee:	a28d                	c.j	2005a650 <at_cpuload+0x208>
2005a4f0:	87aa                	c.mv	a5,a0
2005a4f2:	468d                	c.li	a3,3
2005a4f4:	4522                	c.lwsp	a0,8(sp)
2005a4f6:	ebc18993          	addi	s3,gp,-324 # 20065f94 <para_in>
2005a4fa:	04d79e63          	bne	a5,a3,2005a556 <at_cpuload+0x10e>
2005a4fe:	781060ef          	jal	ra,2006147e <atoi>
2005a502:	842a                	c.mv	s0,a0
2005a504:	4532                	c.lwsp	a0,12(sp)
2005a506:	779060ef          	jal	ra,2006147e <atoi>
2005a50a:	4785                	c.li	a5,1
2005a50c:	00a05663          	bge	zero,a0,2005a518 <at_cpuload+0xd0>
2005a510:	4532                	c.lwsp	a0,12(sp)
2005a512:	76d060ef          	jal	ra,2006147e <atoi>
2005a516:	87aa                	c.mv	a5,a0
2005a518:	00f9a023          	sw	a5,0(s3)
2005a51c:	ec418a93          	addi	s5,gp,-316 # 20065f9c <task_status>
2005a520:	000aa783          	lw	a5,0(s5)
2005a524:	ec418493          	addi	s1,gp,-316 # 20065f9c <task_status>
2005a528:	ebb5                	c.bnez	a5,2005a59c <at_cpuload+0x154>
2005a52a:	6509                	c.lui	a0,0x2
2005a52c:	0541                	c.addi	a0,16
2005a52e:	212010ef          	jal	ra,2005b740 <rtos_mem_malloc>
2005a532:	00aaa023          	sw	a0,0(s5)
2005a536:	e13d                	c.bnez	a0,2005a59c <at_cpuload+0x154>
2005a538:	200626b7          	lui	a3,0x20062
2005a53c:	200625b7          	lui	a1,0x20062
2005a540:	5b868693          	addi	a3,a3,1464 # 200625b8 <pmap_func+0x910>
2005a544:	04900613          	addi	a2,zero,73
2005a548:	1d858593          	addi	a1,a1,472 # 200621d8 <pmap_func+0x530>
2005a54c:	4511                	c.li	a0,4
2005a54e:	123000ef          	jal	ra,2005ae70 <rtk_log_write>
2005a552:	4409                	c.li	s0,2
2005a554:	b799                	c.j	2005a49a <at_cpuload+0x52>
2005a556:	00e79663          	bne	a5,a4,2005a562 <at_cpuload+0x11a>
2005a55a:	725060ef          	jal	ra,2006147e <atoi>
2005a55e:	842a                	c.mv	s0,a0
2005a560:	bf75                	c.j	2005a51c <at_cpuload+0xd4>
2005a562:	71d060ef          	jal	ra,2006147e <atoi>
2005a566:	842a                	c.mv	s0,a0
2005a568:	4532                	c.lwsp	a0,12(sp)
2005a56a:	715060ef          	jal	ra,2006147e <atoi>
2005a56e:	4785                	c.li	a5,1
2005a570:	00a05663          	bge	zero,a0,2005a57c <at_cpuload+0x134>
2005a574:	4532                	c.lwsp	a0,12(sp)
2005a576:	709060ef          	jal	ra,2006147e <atoi>
2005a57a:	87aa                	c.mv	a5,a0
2005a57c:	4542                	c.lwsp	a0,16(sp)
2005a57e:	00f9a023          	sw	a5,0(s3)
2005a582:	6fd060ef          	jal	ra,2006147e <atoi>
2005a586:	57fd                	c.li	a5,-1
2005a588:	00a05763          	bge	zero,a0,2005a596 <at_cpuload+0x14e>
2005a58c:	4542                	c.lwsp	a0,16(sp)
2005a58e:	6f1060ef          	jal	ra,2006147e <atoi>
2005a592:	fff50793          	addi	a5,a0,-1 # 1fff <CRYPTO_Init+0x869>
2005a596:	00f9a223          	sw	a5,4(s3)
2005a59a:	b749                	c.j	2005a51c <at_cpuload+0xd4>
2005a59c:	0049a683          	lw	a3,4(s3)
2005a5a0:	0009a603          	lw	a2,0(s3)
2005a5a4:	20062537          	lui	a0,0x20062
2005a5a8:	85a2                	c.mv	a1,s0
2005a5aa:	5dc50513          	addi	a0,a0,1500 # 200625dc <pmap_func+0x934>
2005a5ae:	2a1040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a5b2:	4789                	c.li	a5,2
2005a5b4:	0af40d63          	beq	s0,a5,2005a66e <at_cpuload+0x226>
2005a5b8:	0087c863          	blt	a5,s0,2005a5c8 <at_cpuload+0x180>
2005a5bc:	c021                	c.beqz	s0,2005a5fc <at_cpuload+0x1b4>
2005a5be:	4785                	c.li	a5,1
2005a5c0:	0af40163          	beq	s0,a5,2005a662 <at_cpuload+0x21a>
2005a5c4:	440d                	c.li	s0,3
2005a5c6:	bdd1                	c.j	2005a49a <at_cpuload+0x52>
2005a5c8:	478d                	c.li	a5,3
2005a5ca:	fef41de3          	bne	s0,a5,2005a5c4 <at_cpuload+0x17c>
2005a5ce:	ec81a783          	lw	a5,-312(gp) # 20065fa0 <top_exit_sema>
2005a5d2:	e7bd                	c.bnez	a5,2005a640 <at_cpuload+0x1f8>
2005a5d4:	b41ff0ef          	jal	ra,2005a114 <update_status>
2005a5d8:	41018937          	lui	s2,0x41018
2005a5dc:	eb818413          	addi	s0,gp,-328 # 20065f90 <last_tick>
2005a5e0:	401c                	c.lw	a5,0(s0)
2005a5e2:	01492503          	lw	a0,20(s2) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a5e6:	8d1d                	c.sub	a0,a5
2005a5e8:	399d                	c.jal	2005a25e <print_delta>
2005a5ea:	01492783          	lw	a5,20(s2)
2005a5ee:	4088                	c.lw	a0,0(s1)
2005a5f0:	c01c                	c.sw	a5,0(s0)
2005a5f2:	178010ef          	jal	ra,2005b76a <rtos_mem_free>
2005a5f6:	0004a023          	sw	zero,0(s1)
2005a5fa:	a099                	c.j	2005a640 <at_cpuload+0x1f8>
2005a5fc:	ec81a783          	lw	a5,-312(gp) # 20065fa0 <top_exit_sema>
2005a600:	e3a1                	c.bnez	a5,2005a640 <at_cpuload+0x1f8>
2005a602:	4088                	c.lw	a0,0(s1)
2005a604:	6609                	c.lui	a2,0x2
2005a606:	0641                	c.addi	a2,16
2005a608:	4581                	c.li	a1,0
2005a60a:	dffaf097          	auipc	ra,0xdffaf
2005a60e:	4c4080e7          	jalr	ra,1220(ra) # 9ace <__wrap_memset>
2005a612:	b03ff0ef          	jal	ra,2005a114 <update_status>
2005a616:	4605                	c.li	a2,1
2005a618:	4581                	c.li	a1,0
2005a61a:	ec818513          	addi	a0,gp,-312 # 20065fa0 <top_exit_sema>
2005a61e:	1ae010ef          	jal	ra,2005b7cc <rtos_sema_create>
2005a622:	2005a637          	lui	a2,0x2005a
2005a626:	200625b7          	lui	a1,0x20062
2005a62a:	47a9                	c.li	a5,10
2005a62c:	6711                	c.lui	a4,0x4
2005a62e:	ebc18693          	addi	a3,gp,-324 # 20065f94 <para_in>
2005a632:	38660613          	addi	a2,a2,902 # 2005a386 <cpu_stat_thread>
2005a636:	61858593          	addi	a1,a1,1560 # 20062618 <pmap_func+0x970>
2005a63a:	4501                	c.li	a0,0
2005a63c:	484010ef          	jal	ra,2005bac0 <rtos_task_create>
2005a640:	20062537          	lui	a0,0x20062
2005a644:	628a0593          	addi	a1,s4,1576
2005a648:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
2005a64c:	203040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a650:	50f6                	c.lwsp	ra,124(sp)
2005a652:	5466                	c.lwsp	s0,120(sp)
2005a654:	54d6                	c.lwsp	s1,116(sp)
2005a656:	5946                	c.lwsp	s2,112(sp)
2005a658:	59b6                	c.lwsp	s3,108(sp)
2005a65a:	5a26                	c.lwsp	s4,104(sp)
2005a65c:	5a96                	c.lwsp	s5,100(sp)
2005a65e:	6109                	c.addi16sp	sp,128
2005a660:	8082                	c.jr	ra
2005a662:	ec81a503          	lw	a0,-312(gp) # 20065fa0 <top_exit_sema>
2005a666:	dd69                	c.beqz	a0,2005a640 <at_cpuload+0x1f8>
2005a668:	252010ef          	jal	ra,2005b8ba <rtos_sema_give>
2005a66c:	bfd1                	c.j	2005a640 <at_cpuload+0x1f8>
2005a66e:	ec81a783          	lw	a5,-312(gp) # 20065fa0 <top_exit_sema>
2005a672:	f7f9                	c.bnez	a5,2005a640 <at_cpuload+0x1f8>
2005a674:	4088                	c.lw	a0,0(s1)
2005a676:	6609                	c.lui	a2,0x2
2005a678:	0641                	c.addi	a2,16
2005a67a:	4581                	c.li	a1,0
2005a67c:	dffaf097          	auipc	ra,0xdffaf
2005a680:	452080e7          	jalr	ra,1106(ra) # 9ace <__wrap_memset>
2005a684:	410187b7          	lui	a5,0x41018
2005a688:	4bd8                	c.lw	a4,20(a5)
2005a68a:	eae1ac23          	sw	a4,-328(gp) # 20065f90 <last_tick>
2005a68e:	a87ff0ef          	jal	ra,2005a114 <update_status>
2005a692:	b77d                	c.j	2005a640 <at_cpuload+0x1f8>

2005a694 <print_system_at>:
2005a694:	1141                	c.addi	sp,-16
2005a696:	c422                	c.swsp	s0,8(sp)
2005a698:	cac18413          	addi	s0,gp,-852 # 20065d84 <at_sys_items>
2005a69c:	c226                	c.swsp	s1,4(sp)
2005a69e:	c04a                	c.swsp	s2,0(sp)
2005a6a0:	c606                	c.swsp	ra,12(sp)
2005a6a2:	0b040493          	addi	s1,s0,176
2005a6a6:	20062937          	lui	s2,0x20062
2005a6aa:	400c                	c.lw	a1,0(s0)
2005a6ac:	6b490513          	addi	a0,s2,1716 # 200626b4 <pmap_func+0xa0c>
2005a6b0:	0441                	c.addi	s0,16
2005a6b2:	19d040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a6b6:	fe941ae3          	bne	s0,s1,2005a6aa <print_system_at+0x16>
2005a6ba:	40b2                	c.lwsp	ra,12(sp)
2005a6bc:	4422                	c.lwsp	s0,8(sp)
2005a6be:	4492                	c.lwsp	s1,4(sp)
2005a6c0:	4902                	c.lwsp	s2,0(sp)
2005a6c2:	0141                	c.addi	sp,16
2005a6c4:	8082                	c.jr	ra

2005a6c6 <at_list>:
2005a6c6:	20062537          	lui	a0,0x20062
2005a6ca:	1141                	c.addi	sp,-16
2005a6cc:	6bc50513          	addi	a0,a0,1724 # 200626bc <pmap_func+0xa14>
2005a6d0:	c606                	c.swsp	ra,12(sp)
2005a6d2:	17d040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005a6d6:	3f7d                	c.jal	2005a694 <print_system_at>
2005a6d8:	40b2                	c.lwsp	ra,12(sp)
2005a6da:	200625b7          	lui	a1,0x20062
2005a6de:	20062537          	lui	a0,0x20062
2005a6e2:	6d458593          	addi	a1,a1,1748 # 200626d4 <pmap_func+0xa2c>
2005a6e6:	0d850513          	addi	a0,a0,216 # 200620d8 <pmap_func+0x430>
2005a6ea:	0141                	c.addi	sp,16
2005a6ec:	1630406f          	jal	zero,2005f04e <DiagPrintf_minimal>

2005a6f0 <at_sys_init>:
2005a6f0:	45ad                	c.li	a1,11
2005a6f2:	cac18513          	addi	a0,gp,-852 # 20065d84 <at_sys_items>
2005a6f6:	0330006f          	jal	zero,2005af28 <log_service_add_table>

2005a6fa <BOOT_Image1>:
2005a6fa:	20000537          	lui	a0,0x20000
2005a6fe:	200017b7          	lui	a5,0x20001
2005a702:	cac78793          	addi	a5,a5,-852 # 20000cac <__rom_bss_end__>
2005a706:	00050613          	addi	a2,a0,0 # 20000000 <UserIrqFunTable>
2005a70a:	1141                	c.addi	sp,-16
2005a70c:	40c78633          	sub	a2,a5,a2
2005a710:	4581                	c.li	a1,0
2005a712:	00050513          	addi	a0,a0,0
2005a716:	c606                	c.swsp	ra,12(sp)
2005a718:	dffaf097          	auipc	ra,0xdffaf
2005a71c:	072080e7          	jalr	ra,114(ra) # 978a <_memset>
2005a720:	dffa6097          	auipc	ra,0xdffa6
2005a724:	3f6080e7          	jalr	ra,1014(ra) # b16 <CPU_ClkGet>
2005a728:	000f47b7          	lui	a5,0xf4
2005a72c:	24078793          	addi	a5,a5,576 # f4240 <__ap_sram_heap_size+0xb4240>
2005a730:	02f55533          	divu	a0,a0,a5
2005a734:	200017b7          	lui	a5,0x20001
2005a738:	86a7a023          	sw	a0,-1952(a5) # 20000860 <RBSS_UDELAY_CLK>
2005a73c:	dffa6097          	auipc	ra,0xdffa6
2005a740:	1e8080e7          	jalr	ra,488(ra) # 924 <BOOT_ROM_InitDebugFlg>
2005a744:	200587b7          	lui	a5,0x20058
2005a748:	04078793          	addi	a5,a5,64 # 20058040 <Img2EntryFun0>
2005a74c:	0017c703          	lbu	a4,1(a5)
2005a750:	0007c683          	lbu	a3,0(a5)
2005a754:	40b2                	c.lwsp	ra,12(sp)
2005a756:	0722                	c.slli	a4,0x8
2005a758:	8ed9                	c.or	a3,a4
2005a75a:	0027c703          	lbu	a4,2(a5)
2005a75e:	0037c783          	lbu	a5,3(a5)
2005a762:	0742                	c.slli	a4,0x10
2005a764:	8f55                	c.or	a4,a3
2005a766:	07e2                	c.slli	a5,0x18
2005a768:	8fd9                	c.or	a5,a4
2005a76a:	0141                	c.addi	sp,16
2005a76c:	8782                	c.jr	a5

2005a76e <flash_read_word>:
2005a76e:	1141                	c.addi	sp,-16
2005a770:	852e                	c.mv	a0,a1
2005a772:	4591                	c.li	a1,4
2005a774:	c606                	c.swsp	ra,12(sp)
2005a776:	bfcfe0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005a77a:	40b2                	c.lwsp	ra,12(sp)
2005a77c:	4505                	c.li	a0,1
2005a77e:	0141                	c.addi	sp,16
2005a780:	8082                	c.jr	ra

2005a782 <os_heap_init>:
2005a782:	1141                	c.addi	sp,-16
2005a784:	c606                	c.swsp	ra,12(sp)
2005a786:	c422                	c.swsp	s0,8(sp)
2005a788:	0800                	c.addi4spn	s0,sp,16
2005a78a:	00000793          	addi	a5,zero,0
2005a78e:	85be                	c.mv	a1,a5
2005a790:	ca0007b7          	lui	a5,0xca000
2005a794:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a798:	20c9                	c.jal	2005a85a <os_heap_add>
2005a79a:	ecc1a703          	lw	a4,-308(gp) # 20065fa4 <total_heap_size>
2005a79e:	00000793          	addi	a5,zero,0
2005a7a2:	973e                	c.add	a4,a5
2005a7a4:	ece1a623          	sw	a4,-308(gp) # 20065fa4 <total_heap_size>
2005a7a8:	67e1                	c.lui	a5,0x18
2005a7aa:	f0078793          	addi	a5,a5,-256 # 17f00 <__bdram_heap_buffer_size__>
2005a7ae:	85be                	c.mv	a1,a5
2005a7b0:	200687b7          	lui	a5,0x20068
2005a7b4:	10078513          	addi	a0,a5,256 # 20068100 <__bdram_heap_buffer_start__>
2005a7b8:	204d                	c.jal	2005a85a <os_heap_add>
2005a7ba:	ecc1a703          	lw	a4,-308(gp) # 20065fa4 <total_heap_size>
2005a7be:	67e1                	c.lui	a5,0x18
2005a7c0:	f0078793          	addi	a5,a5,-256 # 17f00 <__bdram_heap_buffer_size__>
2005a7c4:	973e                	c.add	a4,a5
2005a7c6:	ece1a623          	sw	a4,-308(gp) # 20065fa4 <total_heap_size>
2005a7ca:	00000793          	addi	a5,zero,0
2005a7ce:	85be                	c.mv	a1,a5
2005a7d0:	ca0007b7          	lui	a5,0xca000
2005a7d4:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a7d8:	2049                	c.jal	2005a85a <os_heap_add>
2005a7da:	ecc1a703          	lw	a4,-308(gp) # 20065fa4 <total_heap_size>
2005a7de:	00000793          	addi	a5,zero,0
2005a7e2:	973e                	c.add	a4,a5
2005a7e4:	ece1a623          	sw	a4,-308(gp) # 20065fa4 <total_heap_size>
2005a7e8:	00000793          	addi	a5,zero,0
2005a7ec:	85be                	c.mv	a1,a5
2005a7ee:	ca0007b7          	lui	a5,0xca000
2005a7f2:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a7f6:	2095                	c.jal	2005a85a <os_heap_add>
2005a7f8:	ecc1a703          	lw	a4,-308(gp) # 20065fa4 <total_heap_size>
2005a7fc:	00000793          	addi	a5,zero,0
2005a800:	973e                	c.add	a4,a5
2005a802:	ece1a623          	sw	a4,-308(gp) # 20065fa4 <total_heap_size>
2005a806:	ed01c783          	lbu	a5,-304(gp) # 20065fa8 <heap_index>
2005a80a:	c391                	c.beqz	a5,2005a80e <os_heap_init+0x8c>
2005a80c:	2809                	c.jal	2005a81e <os_heap_sort>
2005a80e:	46418513          	addi	a0,gp,1124 # 2006653c <xHeapRegions>
2005a812:	2c15                	c.jal	2005aa46 <vPortDefineHeapRegions>
2005a814:	0001                	c.addi	zero,0
2005a816:	40b2                	c.lwsp	ra,12(sp)
2005a818:	4422                	c.lwsp	s0,8(sp)
2005a81a:	0141                	c.addi	sp,16
2005a81c:	8082                	c.jr	ra

2005a81e <os_heap_sort>:
2005a81e:	4681                	c.li	a3,0
2005a820:	ed01c703          	lbu	a4,-304(gp) # 20065fa8 <heap_index>
2005a824:	177d                	c.addi	a4,-1
2005a826:	02e6d963          	bge	a3,a4,2005a858 <os_heap_sort+0x3a>
2005a82a:	46418793          	addi	a5,gp,1124 # 2006653c <xHeapRegions>
2005a82e:	4601                	c.li	a2,0
2005a830:	8f15                	c.sub	a4,a3
2005a832:	a839                	c.j	2005a850 <os_heap_sort+0x32>
2005a834:	438c                	c.lw	a1,0(a5)
2005a836:	4788                	c.lw	a0,8(a5)
2005a838:	0605                	c.addi	a2,1
2005a83a:	00b57a63          	bgeu	a0,a1,2005a84e <os_heap_sort+0x30>
2005a83e:	0047a303          	lw	t1,4(a5)
2005a842:	c388                	c.sw	a0,0(a5)
2005a844:	47c8                	c.lw	a0,12(a5)
2005a846:	c78c                	c.sw	a1,8(a5)
2005a848:	0067a623          	sw	t1,12(a5)
2005a84c:	c3c8                	c.sw	a0,4(a5)
2005a84e:	07a1                	c.addi	a5,8
2005a850:	fee642e3          	blt	a2,a4,2005a834 <os_heap_sort+0x16>
2005a854:	0685                	c.addi	a3,1
2005a856:	b7e9                	c.j	2005a820 <os_heap_sort+0x2>
2005a858:	8082                	c.jr	ra

2005a85a <os_heap_add>:
2005a85a:	1141                	c.addi	sp,-16
2005a85c:	c426                	c.swsp	s1,8(sp)
2005a85e:	ed01c703          	lbu	a4,-304(gp) # 20065fa8 <heap_index>
2005a862:	c24a                	c.swsp	s2,4(sp)
2005a864:	c04e                	c.swsp	s3,0(sp)
2005a866:	c606                	c.swsp	ra,12(sp)
2005a868:	4795                	c.li	a5,5
2005a86a:	89aa                	c.mv	s3,a0
2005a86c:	892e                	c.mv	s2,a1
2005a86e:	00e7fa63          	bgeu	a5,a4,2005a882 <os_heap_add+0x28>
2005a872:	20062537          	lui	a0,0x20062
2005a876:	02600593          	addi	a1,zero,38
2005a87a:	75050513          	addi	a0,a0,1872 # 20062750 <__FUNCTION__.0+0x10>
2005a87e:	4c1040ef          	jal	ra,2005f53e <io_assert_failed>
2005a882:	ed01c703          	lbu	a4,-304(gp) # 20065fa8 <heap_index>
2005a886:	46418793          	addi	a5,gp,1124 # 2006653c <xHeapRegions>
2005a88a:	00371693          	slli	a3,a4,0x3
2005a88e:	97b6                	c.add	a5,a3
2005a890:	0137a023          	sw	s3,0(a5)
2005a894:	0127a223          	sw	s2,4(a5)
2005a898:	4501                	c.li	a0,0
2005a89a:	00090663          	beq	s2,zero,2005a8a6 <os_heap_add+0x4c>
2005a89e:	0705                	c.addi	a4,1
2005a8a0:	ece18823          	sb	a4,-304(gp) # 20065fa8 <heap_index>
2005a8a4:	4505                	c.li	a0,1
2005a8a6:	40b2                	c.lwsp	ra,12(sp)
2005a8a8:	44a2                	c.lwsp	s1,8(sp)
2005a8aa:	4912                	c.lwsp	s2,4(sp)
2005a8ac:	4982                	c.lwsp	s3,0(sp)
2005a8ae:	0141                	c.addi	sp,16
2005a8b0:	8082                	c.jr	ra

2005a8b2 <__wrap_malloc>:
2005a8b2:	68f0006f          	jal	zero,2005b740 <rtos_mem_malloc>

2005a8b6 <__wrap_free>:
2005a8b6:	6b50006f          	jal	zero,2005b76a <rtos_mem_free>

2005a8ba <__wrap__free_r>:
2005a8ba:	852e                	c.mv	a0,a1
2005a8bc:	6af0006f          	jal	zero,2005b76a <rtos_mem_free>

2005a8c0 <prvInsertBlockIntoFreeList>:
2005a8c0:	eec18793          	addi	a5,gp,-276 # 20065fc4 <xStart>
2005a8c4:	873e                	c.mv	a4,a5
2005a8c6:	439c                	c.lw	a5,0(a5)
2005a8c8:	fea7eee3          	bltu	a5,a0,2005a8c4 <prvInsertBlockIntoFreeList+0x4>
2005a8cc:	4350                	c.lw	a2,4(a4)
2005a8ce:	00c706b3          	add	a3,a4,a2
2005a8d2:	00d51663          	bne	a0,a3,2005a8de <prvInsertBlockIntoFreeList+0x1e>
2005a8d6:	4154                	c.lw	a3,4(a0)
2005a8d8:	853a                	c.mv	a0,a4
2005a8da:	96b2                	c.add	a3,a2
2005a8dc:	c354                	c.sw	a3,4(a4)
2005a8de:	4154                	c.lw	a3,4(a0)
2005a8e0:	00d50633          	add	a2,a0,a3
2005a8e4:	00c79b63          	bne	a5,a2,2005a8fa <prvInsertBlockIntoFreeList+0x3a>
2005a8e8:	ed41a603          	lw	a2,-300(gp) # 20065fac <pxEnd>
2005a8ec:	00c78763          	beq	a5,a2,2005a8fa <prvInsertBlockIntoFreeList+0x3a>
2005a8f0:	43dc                	c.lw	a5,4(a5)
2005a8f2:	97b6                	c.add	a5,a3
2005a8f4:	c15c                	c.sw	a5,4(a0)
2005a8f6:	431c                	c.lw	a5,0(a4)
2005a8f8:	439c                	c.lw	a5,0(a5)
2005a8fa:	c11c                	c.sw	a5,0(a0)
2005a8fc:	00e50363          	beq	a0,a4,2005a902 <prvInsertBlockIntoFreeList+0x42>
2005a900:	c308                	c.sw	a0,0(a4)
2005a902:	8082                	c.jr	ra

2005a904 <pvPortMalloc>:
2005a904:	1101                	c.addi	sp,-32
2005a906:	cc22                	c.swsp	s0,24(sp)
2005a908:	c452                	c.swsp	s4,8(sp)
2005a90a:	842a                	c.mv	s0,a0
2005a90c:	ce06                	c.swsp	ra,28(sp)
2005a90e:	ca26                	c.swsp	s1,20(sp)
2005a910:	c84a                	c.swsp	s2,16(sp)
2005a912:	c64e                	c.swsp	s3,12(sp)
2005a914:	31f020ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005a918:	ed81aa03          	lw	s4,-296(gp) # 20065fb0 <xBlockAllocatedBit>
2005a91c:	008a77b3          	and	a5,s4,s0
2005a920:	c799                	c.beqz	a5,2005a92e <pvPortMalloc+0x2a>
2005a922:	3be030ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005a926:	26b010ef          	jal	ra,2005c390 <vApplicationMallocFailedHook>
2005a92a:	4901                	c.li	s2,0
2005a92c:	a879                	c.j	2005a9ca <pvPortMalloc+0xc6>
2005a92e:	fff40793          	addi	a5,s0,-1
2005a932:	fde00713          	addi	a4,zero,-34
2005a936:	fef766e3          	bltu	a4,a5,2005a922 <pvPortMalloc+0x1e>
2005a93a:	02040793          	addi	a5,s0,32
2005a93e:	887d                	c.andi	s0,31
2005a940:	c801                	c.beqz	s0,2005a950 <pvPortMalloc+0x4c>
2005a942:	fe07f713          	andi	a4,a5,-32
2005a946:	02070713          	addi	a4,a4,32 # 4020 <GPIO_Direction+0x56>
2005a94a:	fce7fce3          	bgeu	a5,a4,2005a922 <pvPortMalloc+0x1e>
2005a94e:	87ba                	c.mv	a5,a4
2005a950:	edc1a483          	lw	s1,-292(gp) # 20065fb4 <xFreeBytesRemaining>
2005a954:	edc18993          	addi	s3,gp,-292 # 20065fb4 <xFreeBytesRemaining>
2005a958:	fcf4e5e3          	bltu	s1,a5,2005a922 <pvPortMalloc+0x1e>
2005a95c:	eec1a403          	lw	s0,-276(gp) # 20065fc4 <xStart>
2005a960:	eec18713          	addi	a4,gp,-276 # 20065fc4 <xStart>
2005a964:	4054                	c.lw	a3,4(s0)
2005a966:	00f6f463          	bgeu	a3,a5,2005a96e <pvPortMalloc+0x6a>
2005a96a:	4014                	c.lw	a3,0(s0)
2005a96c:	eaa5                	c.bnez	a3,2005a9dc <pvPortMalloc+0xd8>
2005a96e:	ed41a683          	lw	a3,-300(gp) # 20065fac <pxEnd>
2005a972:	fa8688e3          	beq	a3,s0,2005a922 <pvPortMalloc+0x1e>
2005a976:	4014                	c.lw	a3,0(s0)
2005a978:	00072903          	lw	s2,0(a4)
2005a97c:	c314                	c.sw	a3,0(a4)
2005a97e:	4054                	c.lw	a3,4(s0)
2005a980:	04000713          	addi	a4,zero,64
2005a984:	02090913          	addi	s2,s2,32
2005a988:	8e9d                	c.sub	a3,a5
2005a98a:	00d77763          	bgeu	a4,a3,2005a998 <pvPortMalloc+0x94>
2005a98e:	00f40533          	add	a0,s0,a5
2005a992:	c154                	c.sw	a3,4(a0)
2005a994:	c05c                	c.sw	a5,4(s0)
2005a996:	372d                	c.jal	2005a8c0 <prvInsertBlockIntoFreeList>
2005a998:	4058                	c.lw	a4,4(s0)
2005a99a:	ee018693          	addi	a3,gp,-288 # 20065fb8 <xMinimumEverFreeBytesRemaining>
2005a99e:	4290                	c.lw	a2,0(a3)
2005a9a0:	40e487b3          	sub	a5,s1,a4
2005a9a4:	00f9a023          	sw	a5,0(s3)
2005a9a8:	00c7f363          	bgeu	a5,a2,2005a9ae <pvPortMalloc+0xaa>
2005a9ac:	c29c                	c.sw	a5,0(a3)
2005a9ae:	00ea6733          	or	a4,s4,a4
2005a9b2:	ee418793          	addi	a5,gp,-284 # 20065fbc <xNumberOfSuccessfulAllocations>
2005a9b6:	c058                	c.sw	a4,4(s0)
2005a9b8:	4398                	c.lw	a4,0(a5)
2005a9ba:	00042023          	sw	zero,0(s0)
2005a9be:	0705                	c.addi	a4,1
2005a9c0:	c398                	c.sw	a4,0(a5)
2005a9c2:	31e030ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005a9c6:	f60900e3          	beq	s2,zero,2005a926 <pvPortMalloc+0x22>
2005a9ca:	40f2                	c.lwsp	ra,28(sp)
2005a9cc:	4462                	c.lwsp	s0,24(sp)
2005a9ce:	44d2                	c.lwsp	s1,20(sp)
2005a9d0:	49b2                	c.lwsp	s3,12(sp)
2005a9d2:	4a22                	c.lwsp	s4,8(sp)
2005a9d4:	854a                	c.mv	a0,s2
2005a9d6:	4942                	c.lwsp	s2,16(sp)
2005a9d8:	6105                	c.addi16sp	sp,32
2005a9da:	8082                	c.jr	ra
2005a9dc:	8722                	c.mv	a4,s0
2005a9de:	8436                	c.mv	s0,a3
2005a9e0:	b751                	c.j	2005a964 <pvPortMalloc+0x60>

2005a9e2 <vPortFree>:
2005a9e2:	cd31                	c.beqz	a0,2005aa3e <vPortFree+0x5c>
2005a9e4:	1141                	c.addi	sp,-16
2005a9e6:	c422                	c.swsp	s0,8(sp)
2005a9e8:	c606                	c.swsp	ra,12(sp)
2005a9ea:	fe452703          	lw	a4,-28(a0)
2005a9ee:	ed81a783          	lw	a5,-296(gp) # 20065fb0 <xBlockAllocatedBit>
2005a9f2:	842a                	c.mv	s0,a0
2005a9f4:	00f776b3          	and	a3,a4,a5
2005a9f8:	ce9d                	c.beqz	a3,2005aa36 <vPortFree+0x54>
2005a9fa:	fe052683          	lw	a3,-32(a0)
2005a9fe:	ee85                	c.bnez	a3,2005aa36 <vPortFree+0x54>
2005aa00:	fff7c793          	xori	a5,a5,-1
2005aa04:	8ff9                	c.and	a5,a4
2005aa06:	fef52223          	sw	a5,-28(a0)
2005aa0a:	229020ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005aa0e:	edc18793          	addi	a5,gp,-292 # 20065fb4 <xFreeBytesRemaining>
2005aa12:	4394                	c.lw	a3,0(a5)
2005aa14:	fe442703          	lw	a4,-28(s0)
2005aa18:	fe040513          	addi	a0,s0,-32
2005aa1c:	9736                	c.add	a4,a3
2005aa1e:	c398                	c.sw	a4,0(a5)
2005aa20:	3545                	c.jal	2005a8c0 <prvInsertBlockIntoFreeList>
2005aa22:	ee818793          	addi	a5,gp,-280 # 20065fc0 <xNumberOfSuccessfulFrees>
2005aa26:	4398                	c.lw	a4,0(a5)
2005aa28:	0705                	c.addi	a4,1
2005aa2a:	c398                	c.sw	a4,0(a5)
2005aa2c:	4422                	c.lwsp	s0,8(sp)
2005aa2e:	40b2                	c.lwsp	ra,12(sp)
2005aa30:	0141                	c.addi	sp,16
2005aa32:	2ae0306f          	jal	zero,2005dce0 <xTaskResumeAll>
2005aa36:	40b2                	c.lwsp	ra,12(sp)
2005aa38:	4422                	c.lwsp	s0,8(sp)
2005aa3a:	0141                	c.addi	sp,16
2005aa3c:	8082                	c.jr	ra
2005aa3e:	8082                	c.jr	ra

2005aa40 <xPortGetFreeHeapSize>:
2005aa40:	edc1a503          	lw	a0,-292(gp) # 20065fb4 <xFreeBytesRemaining>
2005aa44:	8082                	c.jr	ra

2005aa46 <vPortDefineHeapRegions>:
2005aa46:	ed41a583          	lw	a1,-300(gp) # 20065fac <pxEnd>
2005aa4a:	4701                	c.li	a4,0
2005aa4c:	4301                	c.li	t1,0
2005aa4e:	4681                	c.li	a3,0
2005aa50:	ed418613          	addi	a2,gp,-300 # 20065fac <pxEnd>
2005aa54:	eec18813          	addi	a6,gp,-276 # 20065fc4 <xStart>
2005aa58:	415c                	c.lw	a5,4(a0)
2005aa5a:	ef81                	c.bnez	a5,2005aa72 <vPortDefineHeapRegions+0x2c>
2005aa5c:	c311                	c.beqz	a4,2005aa60 <vPortDefineHeapRegions+0x1a>
2005aa5e:	c20c                	c.sw	a1,0(a2)
2005aa60:	eed1a023          	sw	a3,-288(gp) # 20065fb8 <xMinimumEverFreeBytesRemaining>
2005aa64:	ecd1ae23          	sw	a3,-292(gp) # 20065fb4 <xFreeBytesRemaining>
2005aa68:	80000737          	lui	a4,0x80000
2005aa6c:	ece1ac23          	sw	a4,-296(gp) # 20065fb0 <xBlockAllocatedBit>
2005aa70:	8082                	c.jr	ra
2005aa72:	4118                	c.lw	a4,0(a0)
2005aa74:	01f77893          	andi	a7,a4,31
2005aa78:	00088a63          	beq	a7,zero,2005aa8c <vPortDefineHeapRegions+0x46>
2005aa7c:	01f70893          	addi	a7,a4,31 # 8000001f <__ctrace_end__+0xb600001f>
2005aa80:	fe08f893          	andi	a7,a7,-32
2005aa84:	97ba                	c.add	a5,a4
2005aa86:	411787b3          	sub	a5,a5,a7
2005aa8a:	8746                	c.mv	a4,a7
2005aa8c:	00031663          	bne	t1,zero,2005aa98 <vPortDefineHeapRegions+0x52>
2005aa90:	00e82023          	sw	a4,0(a6)
2005aa94:	00082223          	sw	zero,4(a6)
2005aa98:	97ba                	c.add	a5,a4
2005aa9a:	1781                	c.addi	a5,-32
2005aa9c:	9b81                	c.andi	a5,-32
2005aa9e:	0007a223          	sw	zero,4(a5)
2005aaa2:	0007a023          	sw	zero,0(a5)
2005aaa6:	40e788b3          	sub	a7,a5,a4
2005aaaa:	01172223          	sw	a7,4(a4)
2005aaae:	c31c                	c.sw	a5,0(a4)
2005aab0:	c191                	c.beqz	a1,2005aab4 <vPortDefineHeapRegions+0x6e>
2005aab2:	c198                	c.sw	a4,0(a1)
2005aab4:	4358                	c.lw	a4,4(a4)
2005aab6:	0305                	c.addi	t1,1
2005aab8:	0521                	c.addi	a0,8
2005aaba:	96ba                	c.add	a3,a4
2005aabc:	85be                	c.mv	a1,a5
2005aabe:	4705                	c.li	a4,1
2005aac0:	bf61                	c.j	2005aa58 <vPortDefineHeapRegions+0x12>

2005aac2 <vPortGetHeapStats>:
2005aac2:	1101                	c.addi	sp,-32
2005aac4:	cc22                	c.swsp	s0,24(sp)
2005aac6:	ce06                	c.swsp	ra,28(sp)
2005aac8:	ca26                	c.swsp	s1,20(sp)
2005aaca:	c84a                	c.swsp	s2,16(sp)
2005aacc:	c64e                	c.swsp	s3,12(sp)
2005aace:	842a                	c.mv	s0,a0
2005aad0:	163020ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005aad4:	eec1a783          	lw	a5,-276(gp) # 20065fc4 <xStart>
2005aad8:	cfb9                	c.beqz	a5,2005ab36 <vPortGetHeapStats+0x74>
2005aada:	ed41a683          	lw	a3,-300(gp) # 20065fac <pxEnd>
2005aade:	597d                	c.li	s2,-1
2005aae0:	4981                	c.li	s3,0
2005aae2:	4481                	c.li	s1,0
2005aae4:	43d8                	c.lw	a4,4(a5)
2005aae6:	0485                	c.addi	s1,1
2005aae8:	00e9f363          	bgeu	s3,a4,2005aaee <vPortGetHeapStats+0x2c>
2005aaec:	89ba                	c.mv	s3,a4
2005aaee:	c701                	c.beqz	a4,2005aaf6 <vPortGetHeapStats+0x34>
2005aaf0:	01277363          	bgeu	a4,s2,2005aaf6 <vPortGetHeapStats+0x34>
2005aaf4:	893a                	c.mv	s2,a4
2005aaf6:	439c                	c.lw	a5,0(a5)
2005aaf8:	fef696e3          	bne	a3,a5,2005aae4 <vPortGetHeapStats+0x22>
2005aafc:	1e4030ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005ab00:	01342223          	sw	s3,4(s0)
2005ab04:	01242423          	sw	s2,8(s0)
2005ab08:	c444                	c.sw	s1,12(s0)
2005ab0a:	64f020ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005ab0e:	edc1a783          	lw	a5,-292(gp) # 20065fb4 <xFreeBytesRemaining>
2005ab12:	40f2                	c.lwsp	ra,28(sp)
2005ab14:	44d2                	c.lwsp	s1,20(sp)
2005ab16:	c01c                	c.sw	a5,0(s0)
2005ab18:	ee41a783          	lw	a5,-284(gp) # 20065fbc <xNumberOfSuccessfulAllocations>
2005ab1c:	4942                	c.lwsp	s2,16(sp)
2005ab1e:	49b2                	c.lwsp	s3,12(sp)
2005ab20:	c85c                	c.sw	a5,20(s0)
2005ab22:	ee81a783          	lw	a5,-280(gp) # 20065fc0 <xNumberOfSuccessfulFrees>
2005ab26:	cc1c                	c.sw	a5,24(s0)
2005ab28:	ee01a783          	lw	a5,-288(gp) # 20065fb8 <xMinimumEverFreeBytesRemaining>
2005ab2c:	c81c                	c.sw	a5,16(s0)
2005ab2e:	4462                	c.lwsp	s0,24(sp)
2005ab30:	6105                	c.addi16sp	sp,32
2005ab32:	6410206f          	jal	zero,2005d972 <vTaskExitCritical>
2005ab36:	597d                	c.li	s2,-1
2005ab38:	4981                	c.li	s3,0
2005ab3a:	4481                	c.li	s1,0
2005ab3c:	b7c1                	c.j	2005aafc <vPortGetHeapStats+0x3a>

2005ab3e <vListInitialise>:
2005ab3e:	00850793          	addi	a5,a0,8
2005ab42:	577d                	c.li	a4,-1
2005ab44:	c15c                	c.sw	a5,4(a0)
2005ab46:	c518                	c.sw	a4,8(a0)
2005ab48:	c55c                	c.sw	a5,12(a0)
2005ab4a:	c91c                	c.sw	a5,16(a0)
2005ab4c:	00052023          	sw	zero,0(a0)
2005ab50:	8082                	c.jr	ra

2005ab52 <vListInitialiseItem>:
2005ab52:	00052823          	sw	zero,16(a0)
2005ab56:	8082                	c.jr	ra

2005ab58 <vListInsertEnd>:
2005ab58:	415c                	c.lw	a5,4(a0)
2005ab5a:	c1dc                	c.sw	a5,4(a1)
2005ab5c:	4798                	c.lw	a4,8(a5)
2005ab5e:	c598                	c.sw	a4,8(a1)
2005ab60:	c34c                	c.sw	a1,4(a4)
2005ab62:	c78c                	c.sw	a1,8(a5)
2005ab64:	c988                	c.sw	a0,16(a1)
2005ab66:	411c                	c.lw	a5,0(a0)
2005ab68:	0785                	c.addi	a5,1
2005ab6a:	c11c                	c.sw	a5,0(a0)
2005ab6c:	8082                	c.jr	ra

2005ab6e <vListInsert>:
2005ab6e:	4194                	c.lw	a3,0(a1)
2005ab70:	57fd                	c.li	a5,-1
2005ab72:	00850713          	addi	a4,a0,8
2005ab76:	00f69d63          	bne	a3,a5,2005ab90 <vListInsert+0x22>
2005ab7a:	491c                	c.lw	a5,16(a0)
2005ab7c:	43d8                	c.lw	a4,4(a5)
2005ab7e:	c1d8                	c.sw	a4,4(a1)
2005ab80:	c70c                	c.sw	a1,8(a4)
2005ab82:	c59c                	c.sw	a5,8(a1)
2005ab84:	c3cc                	c.sw	a1,4(a5)
2005ab86:	c988                	c.sw	a0,16(a1)
2005ab88:	411c                	c.lw	a5,0(a0)
2005ab8a:	0785                	c.addi	a5,1
2005ab8c:	c11c                	c.sw	a5,0(a0)
2005ab8e:	8082                	c.jr	ra
2005ab90:	87ba                	c.mv	a5,a4
2005ab92:	4358                	c.lw	a4,4(a4)
2005ab94:	4310                	c.lw	a2,0(a4)
2005ab96:	fec6fde3          	bgeu	a3,a2,2005ab90 <vListInsert+0x22>
2005ab9a:	b7cd                	c.j	2005ab7c <vListInsert+0xe>

2005ab9c <uxListRemove>:
2005ab9c:	4154                	c.lw	a3,4(a0)
2005ab9e:	4518                	c.lw	a4,8(a0)
2005aba0:	491c                	c.lw	a5,16(a0)
2005aba2:	c698                	c.sw	a4,8(a3)
2005aba4:	4154                	c.lw	a3,4(a0)
2005aba6:	c354                	c.sw	a3,4(a4)
2005aba8:	43d8                	c.lw	a4,4(a5)
2005abaa:	00a71463          	bne	a4,a0,2005abb2 <uxListRemove+0x16>
2005abae:	4518                	c.lw	a4,8(a0)
2005abb0:	c3d8                	c.sw	a4,4(a5)
2005abb2:	00052823          	sw	zero,16(a0)
2005abb6:	4398                	c.lw	a4,0(a5)
2005abb8:	177d                	c.addi	a4,-1
2005abba:	c398                	c.sw	a4,0(a5)
2005abbc:	4388                	c.lw	a0,0(a5)
2005abbe:	8082                	c.jr	ra

2005abc0 <rtk_log_array_print>:
2005abc0:	1101                	c.addi	sp,-32
2005abc2:	cc22                	c.swsp	s0,24(sp)
2005abc4:	c84a                	c.swsp	s2,16(sp)
2005abc6:	ce06                	c.swsp	ra,28(sp)
2005abc8:	ca26                	c.swsp	s1,20(sp)
2005abca:	c64e                	c.swsp	s3,12(sp)
2005abcc:	ef418793          	addi	a5,gp,-268 # 20065fcc <rtk_log_entry_count>
2005abd0:	4394                	c.lw	a3,0(a5)
2005abd2:	470d                	c.li	a4,3
2005abd4:	842a                	c.mv	s0,a0
2005abd6:	4911                	c.li	s2,4
2005abd8:	00d76463          	bltu	a4,a3,2005abe0 <rtk_log_array_print+0x20>
2005abdc:	0007a903          	lw	s2,0(a5)
2005abe0:	557d                	c.li	a0,-1
2005abe2:	c801                	c.beqz	s0,2005abf2 <rtk_log_array_print+0x32>
2005abe4:	0405                	c.addi	s0,1
2005abe6:	4481                	c.li	s1,0
2005abe8:	200629b7          	lui	s3,0x20062
2005abec:	01249a63          	bne	s1,s2,2005ac00 <rtk_log_array_print+0x40>
2005abf0:	4501                	c.li	a0,0
2005abf2:	40f2                	c.lwsp	ra,28(sp)
2005abf4:	4462                	c.lwsp	s0,24(sp)
2005abf6:	44d2                	c.lwsp	s1,20(sp)
2005abf8:	4942                	c.lwsp	s2,16(sp)
2005abfa:	49b2                	c.lwsp	s3,12(sp)
2005abfc:	6105                	c.addi16sp	sp,32
2005abfe:	8082                	c.jr	ra
2005ac00:	fff44603          	lbu	a2,-1(s0)
2005ac04:	85a2                	c.mv	a1,s0
2005ac06:	7ec98513          	addi	a0,s3,2028 # 200627ec <__FUNCTION__.0+0xac>
2005ac0a:	444040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005ac0e:	0485                	c.addi	s1,1
2005ac10:	042d                	c.addi	s0,11
2005ac12:	bfe9                	c.j	2005abec <rtk_log_array_print+0x2c>

2005ac14 <rtk_log_array_clear>:
2005ac14:	1141                	c.addi	sp,-16
2005ac16:	02c00613          	addi	a2,zero,44
2005ac1a:	4581                	c.li	a1,0
2005ac1c:	49418513          	addi	a0,gp,1172 # 2006656c <rtk_log_tag_array>
2005ac20:	c606                	c.swsp	ra,12(sp)
2005ac22:	dffaf097          	auipc	ra,0xdffaf
2005ac26:	b68080e7          	jalr	ra,-1176(ra) # 978a <_memset>
2005ac2a:	40b2                	c.lwsp	ra,12(sp)
2005ac2c:	ee01aa23          	sw	zero,-268(gp) # 20065fcc <rtk_log_entry_count>
2005ac30:	0141                	c.addi	sp,16
2005ac32:	8082                	c.jr	ra

2005ac34 <rtk_log_level_get>:
2005ac34:	1101                	c.addi	sp,-32
2005ac36:	c64e                	c.swsp	s3,12(sp)
2005ac38:	c452                	c.swsp	s4,8(sp)
2005ac3a:	ce06                	c.swsp	ra,28(sp)
2005ac3c:	cc22                	c.swsp	s0,24(sp)
2005ac3e:	ca26                	c.swsp	s1,20(sp)
2005ac40:	c84a                	c.swsp	s2,16(sp)
2005ac42:	ef418793          	addi	a5,gp,-268 # 20065fcc <rtk_log_entry_count>
2005ac46:	4394                	c.lw	a3,0(a5)
2005ac48:	470d                	c.li	a4,3
2005ac4a:	89aa                	c.mv	s3,a0
2005ac4c:	4a11                	c.li	s4,4
2005ac4e:	00d76463          	bltu	a4,a3,2005ac56 <rtk_log_level_get+0x22>
2005ac52:	0007aa03          	lw	s4,0(a5)
2005ac56:	00099a63          	bne	s3,zero,2005ac6a <rtk_log_level_get+0x36>
2005ac5a:	20063537          	lui	a0,0x20063
2005ac5e:	05000593          	addi	a1,zero,80
2005ac62:	80050513          	addi	a0,a0,-2048 # 20062800 <__FUNCTION__.0+0xc0>
2005ac66:	0d9040ef          	jal	ra,2005f53e <io_assert_failed>
2005ac6a:	49418913          	addi	s2,gp,1172 # 2006656c <rtk_log_tag_array>
2005ac6e:	0905                	c.addi	s2,1
2005ac70:	4401                	c.li	s0,0
2005ac72:	49418493          	addi	s1,gp,1172 # 2006656c <rtk_log_tag_array>
2005ac76:	01441563          	bne	s0,s4,2005ac80 <rtk_log_level_get+0x4c>
2005ac7a:	e881c503          	lbu	a0,-376(gp) # 20065f60 <rtk_log_default_level>
2005ac7e:	a839                	c.j	2005ac9c <rtk_log_level_get+0x68>
2005ac80:	854a                	c.mv	a0,s2
2005ac82:	85ce                	c.mv	a1,s3
2005ac84:	dffaf097          	auipc	ra,0xdffaf
2005ac88:	ba8080e7          	jalr	ra,-1112(ra) # 982c <_strcmp>
2005ac8c:	092d                	c.addi	s2,11
2005ac8e:	ed19                	c.bnez	a0,2005acac <rtk_log_level_get+0x78>
2005ac90:	47ad                	c.li	a5,11
2005ac92:	02f40433          	mul	s0,s0,a5
2005ac96:	9426                	c.add	s0,s1
2005ac98:	00044503          	lbu	a0,0(s0)
2005ac9c:	40f2                	c.lwsp	ra,28(sp)
2005ac9e:	4462                	c.lwsp	s0,24(sp)
2005aca0:	44d2                	c.lwsp	s1,20(sp)
2005aca2:	4942                	c.lwsp	s2,16(sp)
2005aca4:	49b2                	c.lwsp	s3,12(sp)
2005aca6:	4a22                	c.lwsp	s4,8(sp)
2005aca8:	6105                	c.addi16sp	sp,32
2005acaa:	8082                	c.jr	ra
2005acac:	0405                	c.addi	s0,1
2005acae:	b7e1                	c.j	2005ac76 <rtk_log_level_get+0x42>

2005acb0 <rtk_log_level_set>:
2005acb0:	1101                	c.addi	sp,-32
2005acb2:	cc22                	c.swsp	s0,24(sp)
2005acb4:	ef418793          	addi	a5,gp,-268 # 20065fcc <rtk_log_entry_count>
2005acb8:	c84a                	c.swsp	s2,16(sp)
2005acba:	c64e                	c.swsp	s3,12(sp)
2005acbc:	c05a                	c.swsp	s6,0(sp)
2005acbe:	ce06                	c.swsp	ra,28(sp)
2005acc0:	ca26                	c.swsp	s1,20(sp)
2005acc2:	c452                	c.swsp	s4,8(sp)
2005acc4:	c256                	c.swsp	s5,4(sp)
2005acc6:	4398                	c.lw	a4,0(a5)
2005acc8:	478d                	c.li	a5,3
2005acca:	892a                	c.mv	s2,a0
2005accc:	89ae                	c.mv	s3,a1
2005acce:	ef418413          	addi	s0,gp,-268 # 20065fcc <rtk_log_entry_count>
2005acd2:	4b11                	c.li	s6,4
2005acd4:	00e7e463          	bltu	a5,a4,2005acdc <rtk_log_level_set+0x2c>
2005acd8:	00042b03          	lw	s6,0(s0)
2005acdc:	557d                	c.li	a0,-1
2005acde:	06090b63          	beq	s2,zero,2005ad54 <rtk_log_level_set+0xa4>
2005ace2:	4795                	c.li	a5,5
2005ace4:	0737e863          	bltu	a5,s3,2005ad54 <rtk_log_level_set+0xa4>
2005ace8:	200635b7          	lui	a1,0x20063
2005acec:	89458593          	addi	a1,a1,-1900 # 20062894 <__FUNCTION__.0+0x154>
2005acf0:	854a                	c.mv	a0,s2
2005acf2:	dffaf097          	auipc	ra,0xdffaf
2005acf6:	b3a080e7          	jalr	ra,-1222(ra) # 982c <_strcmp>
2005acfa:	c939                	c.beqz	a0,2005ad50 <rtk_log_level_set+0xa0>
2005acfc:	49418a93          	addi	s5,gp,1172 # 2006656c <rtk_log_tag_array>
2005ad00:	0a85                	c.addi	s5,1
2005ad02:	4a01                	c.li	s4,0
2005ad04:	49418493          	addi	s1,gp,1172 # 2006656c <rtk_log_tag_array>
2005ad08:	076a1063          	bne	s4,s6,2005ad68 <rtk_log_level_set+0xb8>
2005ad0c:	4018                	c.lw	a4,0(s0)
2005ad0e:	478d                	c.li	a5,3
2005ad10:	00e7f863          	bgeu	a5,a4,2005ad20 <rtk_log_level_set+0x70>
2005ad14:	20063537          	lui	a0,0x20063
2005ad18:	89850513          	addi	a0,a0,-1896 # 20062898 <__FUNCTION__.0+0x158>
2005ad1c:	332040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005ad20:	4018                	c.lw	a4,0(s0)
2005ad22:	401c                	c.lw	a5,0(s0)
2005ad24:	452d                	c.li	a0,11
2005ad26:	8b0d                	c.andi	a4,3
2005ad28:	8b8d                	c.andi	a5,3
2005ad2a:	02a70733          	mul	a4,a4,a0
2005ad2e:	4625                	c.li	a2,9
2005ad30:	85ca                	c.mv	a1,s2
2005ad32:	02a787b3          	mul	a5,a5,a0
2005ad36:	9726                	c.add	a4,s1
2005ad38:	01370023          	sb	s3,0(a4)
2005ad3c:	0785                	c.addi	a5,1
2005ad3e:	00f48533          	add	a0,s1,a5
2005ad42:	123060ef          	jal	ra,20061664 <strncpy>
2005ad46:	401c                	c.lw	a5,0(s0)
2005ad48:	4501                	c.li	a0,0
2005ad4a:	0785                	c.addi	a5,1
2005ad4c:	c01c                	c.sw	a5,0(s0)
2005ad4e:	a019                	c.j	2005ad54 <rtk_log_level_set+0xa4>
2005ad50:	e9318423          	sb	s3,-376(gp) # 20065f60 <rtk_log_default_level>
2005ad54:	40f2                	c.lwsp	ra,28(sp)
2005ad56:	4462                	c.lwsp	s0,24(sp)
2005ad58:	44d2                	c.lwsp	s1,20(sp)
2005ad5a:	4942                	c.lwsp	s2,16(sp)
2005ad5c:	49b2                	c.lwsp	s3,12(sp)
2005ad5e:	4a22                	c.lwsp	s4,8(sp)
2005ad60:	4a92                	c.lwsp	s5,4(sp)
2005ad62:	4b02                	c.lwsp	s6,0(sp)
2005ad64:	6105                	c.addi16sp	sp,32
2005ad66:	8082                	c.jr	ra
2005ad68:	8556                	c.mv	a0,s5
2005ad6a:	85ca                	c.mv	a1,s2
2005ad6c:	dffaf097          	auipc	ra,0xdffaf
2005ad70:	ac0080e7          	jalr	ra,-1344(ra) # 982c <_strcmp>
2005ad74:	0aad                	c.addi	s5,11
2005ad76:	e901                	c.bnez	a0,2005ad86 <rtk_log_level_set+0xd6>
2005ad78:	47ad                	c.li	a5,11
2005ad7a:	02fa0a33          	mul	s4,s4,a5
2005ad7e:	94d2                	c.add	s1,s4
2005ad80:	01348023          	sb	s3,0(s1)
2005ad84:	bfc1                	c.j	2005ad54 <rtk_log_level_set+0xa4>
2005ad86:	0a05                	c.addi	s4,1
2005ad88:	b741                	c.j	2005ad08 <rtk_log_level_set+0x58>

2005ad8a <rtk_log_memory_dump_word>:
2005ad8a:	1101                	c.addi	sp,-32
2005ad8c:	cc22                	c.swsp	s0,24(sp)
2005ad8e:	ca26                	c.swsp	s1,20(sp)
2005ad90:	c84a                	c.swsp	s2,16(sp)
2005ad92:	c64e                	c.swsp	s3,12(sp)
2005ad94:	c452                	c.swsp	s4,8(sp)
2005ad96:	c256                	c.swsp	s5,4(sp)
2005ad98:	c05a                	c.swsp	s6,0(sp)
2005ad9a:	ce06                	c.swsp	ra,28(sp)
2005ad9c:	892a                	c.mv	s2,a0
2005ad9e:	89ae                	c.mv	s3,a1
2005ada0:	84aa                	c.mv	s1,a0
2005ada2:	4401                	c.li	s0,0
2005ada4:	20063a37          	lui	s4,0x20063
2005ada8:	20063ab7          	lui	s5,0x20063
2005adac:	20063b37          	lui	s6,0x20063
2005adb0:	03341163          	bne	s0,s3,2005add2 <rtk_log_memory_dump_word+0x48>
2005adb4:	4462                	c.lwsp	s0,24(sp)
2005adb6:	40f2                	c.lwsp	ra,28(sp)
2005adb8:	44d2                	c.lwsp	s1,20(sp)
2005adba:	4942                	c.lwsp	s2,16(sp)
2005adbc:	49b2                	c.lwsp	s3,12(sp)
2005adbe:	4a22                	c.lwsp	s4,8(sp)
2005adc0:	4a92                	c.lwsp	s5,4(sp)
2005adc2:	4b02                	c.lwsp	s6,0(sp)
2005adc4:	20062537          	lui	a0,0x20062
2005adc8:	12850513          	addi	a0,a0,296 # 20062128 <pmap_func+0x480>
2005adcc:	6105                	c.addi16sp	sp,32
2005adce:	2800406f          	jal	zero,2005f04e <DiagPrintf_minimal>
2005add2:	ec11                	c.bnez	s0,2005adee <rtk_log_memory_dump_word+0x64>
2005add4:	85ca                	c.mv	a1,s2
2005add6:	8c4a8513          	addi	a0,s5,-1852 # 200628c4 <__FUNCTION__.0+0x184>
2005adda:	274040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005adde:	408c                	c.lw	a1,0(s1)
2005ade0:	8d8b0513          	addi	a0,s6,-1832 # 200628d8 <__FUNCTION__.0+0x198>
2005ade4:	0405                	c.addi	s0,1
2005ade6:	268040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005adea:	0491                	c.addi	s1,4
2005adec:	b7d1                	c.j	2005adb0 <rtk_log_memory_dump_word+0x26>
2005adee:	00747793          	andi	a5,s0,7
2005adf2:	f7f5                	c.bnez	a5,2005adde <rtk_log_memory_dump_word+0x54>
2005adf4:	85a6                	c.mv	a1,s1
2005adf6:	8cca0513          	addi	a0,s4,-1844 # 200628cc <__FUNCTION__.0+0x18c>
2005adfa:	b7c5                	c.j	2005adda <rtk_log_memory_dump_word+0x50>

2005adfc <rtk_log_memory_dump_byte>:
2005adfc:	1101                	c.addi	sp,-32
2005adfe:	cc22                	c.swsp	s0,24(sp)
2005ae00:	ca26                	c.swsp	s1,20(sp)
2005ae02:	c84a                	c.swsp	s2,16(sp)
2005ae04:	c452                	c.swsp	s4,8(sp)
2005ae06:	c256                	c.swsp	s5,4(sp)
2005ae08:	c05a                	c.swsp	s6,0(sp)
2005ae0a:	ce06                	c.swsp	ra,28(sp)
2005ae0c:	c64e                	c.swsp	s3,12(sp)
2005ae0e:	84aa                	c.mv	s1,a0
2005ae10:	892e                	c.mv	s2,a1
2005ae12:	4401                	c.li	s0,0
2005ae14:	20063a37          	lui	s4,0x20063
2005ae18:	20063ab7          	lui	s5,0x20063
2005ae1c:	20063b37          	lui	s6,0x20063
2005ae20:	03241163          	bne	s0,s2,2005ae42 <rtk_log_memory_dump_byte+0x46>
2005ae24:	4462                	c.lwsp	s0,24(sp)
2005ae26:	40f2                	c.lwsp	ra,28(sp)
2005ae28:	44d2                	c.lwsp	s1,20(sp)
2005ae2a:	4942                	c.lwsp	s2,16(sp)
2005ae2c:	49b2                	c.lwsp	s3,12(sp)
2005ae2e:	4a22                	c.lwsp	s4,8(sp)
2005ae30:	4a92                	c.lwsp	s5,4(sp)
2005ae32:	4b02                	c.lwsp	s6,0(sp)
2005ae34:	20062537          	lui	a0,0x20062
2005ae38:	12850513          	addi	a0,a0,296 # 20062128 <pmap_func+0x480>
2005ae3c:	6105                	c.addi16sp	sp,32
2005ae3e:	2100406f          	jal	zero,2005f04e <DiagPrintf_minimal>
2005ae42:	008489b3          	add	s3,s1,s0
2005ae46:	ec11                	c.bnez	s0,2005ae62 <rtk_log_memory_dump_byte+0x66>
2005ae48:	85a6                	c.mv	a1,s1
2005ae4a:	8c4a8513          	addi	a0,s5,-1852 # 200628c4 <__FUNCTION__.0+0x184>
2005ae4e:	200040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005ae52:	0009c583          	lbu	a1,0(s3)
2005ae56:	8e0b0513          	addi	a0,s6,-1824 # 200628e0 <__FUNCTION__.0+0x1a0>
2005ae5a:	0405                	c.addi	s0,1
2005ae5c:	1f2040ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005ae60:	b7c1                	c.j	2005ae20 <rtk_log_memory_dump_byte+0x24>
2005ae62:	00747793          	andi	a5,s0,7
2005ae66:	f7f5                	c.bnez	a5,2005ae52 <rtk_log_memory_dump_byte+0x56>
2005ae68:	85ce                	c.mv	a1,s3
2005ae6a:	8cca0513          	addi	a0,s4,-1844 # 200628cc <__FUNCTION__.0+0x18c>
2005ae6e:	b7c5                	c.j	2005ae4e <rtk_log_memory_dump_byte+0x52>

2005ae70 <rtk_log_write>:
2005ae70:	7139                	c.addi16sp	sp,-64
2005ae72:	d226                	c.swsp	s1,36(sp)
2005ae74:	84aa                	c.mv	s1,a0
2005ae76:	852e                	c.mv	a0,a1
2005ae78:	d422                	c.swsp	s0,40(sp)
2005ae7a:	d606                	c.swsp	ra,44(sp)
2005ae7c:	c632                	c.swsp	a2,12(sp)
2005ae7e:	8436                	c.mv	s0,a3
2005ae80:	d83a                	c.swsp	a4,48(sp)
2005ae82:	da3e                	c.swsp	a5,52(sp)
2005ae84:	dc42                	c.swsp	a6,56(sp)
2005ae86:	de46                	c.swsp	a7,60(sp)
2005ae88:	c42e                	c.swsp	a1,8(sp)
2005ae8a:	336d                	c.jal	2005ac34 <rtk_log_level_get>
2005ae8c:	02956563          	bltu	a0,s1,2005aeb6 <rtk_log_write+0x46>
2005ae90:	45a2                	c.lwsp	a1,8(sp)
2005ae92:	02300793          	addi	a5,zero,35
2005ae96:	0005c703          	lbu	a4,0(a1)
2005ae9a:	00f70963          	beq	a4,a5,2005aeac <rtk_log_write+0x3c>
2005ae9e:	4632                	c.lwsp	a2,12(sp)
2005aea0:	20063537          	lui	a0,0x20063
2005aea4:	8e850513          	addi	a0,a0,-1816 # 200628e8 <__FUNCTION__.0+0x1a8>
2005aea8:	186040ef          	jal	ra,2005f02e <DiagPrintf>
2005aeac:	180c                	c.addi4spn	a1,sp,48
2005aeae:	8522                	c.mv	a0,s0
2005aeb0:	ce2e                	c.swsp	a1,28(sp)
2005aeb2:	67f030ef          	jal	ra,2005ed30 <DiagVprintf>
2005aeb6:	50b2                	c.lwsp	ra,44(sp)
2005aeb8:	5422                	c.lwsp	s0,40(sp)
2005aeba:	5492                	c.lwsp	s1,36(sp)
2005aebc:	6121                	c.addi16sp	sp,64
2005aebe:	8082                	c.jr	ra

2005aec0 <hash_index>:
2005aec0:	4781                	c.li	a5,0
2005aec2:	08300693          	addi	a3,zero,131
2005aec6:	00054703          	lbu	a4,0(a0)
2005aeca:	e709                	c.bnez	a4,2005aed4 <hash_index+0x14>
2005aecc:	00179513          	slli	a0,a5,0x1
2005aed0:	8105                	c.srli	a0,0x1
2005aed2:	8082                	c.jr	ra
2005aed4:	02d787b3          	mul	a5,a5,a3
2005aed8:	0505                	c.addi	a0,1
2005aeda:	97ba                	c.add	a5,a4
2005aedc:	b7ed                	c.j	2005aec6 <hash_index+0x6>

2005aede <log_add_new_command>:
2005aede:	1141                	c.addi	sp,-16
2005aee0:	c422                	c.swsp	s0,8(sp)
2005aee2:	c606                	c.swsp	ra,12(sp)
2005aee4:	842a                	c.mv	s0,a0
2005aee6:	4108                	c.lw	a0,0(a0)
2005aee8:	3fe1                	c.jal	2005aec0 <hash_index>
2005aeea:	02000793          	addi	a5,zero,32
2005aeee:	02f56533          	rem	a0,a0,a5
2005aef2:	4c018793          	addi	a5,gp,1216 # 20066598 <log_hash>
2005aef6:	00840713          	addi	a4,s0,8
2005aefa:	050e                	c.slli	a0,0x3
2005aefc:	97aa                	c.add	a5,a0
2005aefe:	4394                	c.lw	a3,0(a5)
2005af00:	c2d8                	c.sw	a4,4(a3)
2005af02:	c414                	c.sw	a3,8(s0)
2005af04:	c45c                	c.sw	a5,12(s0)
2005af06:	c398                	c.sw	a4,0(a5)
2005af08:	40b2                	c.lwsp	ra,12(sp)
2005af0a:	4422                	c.lwsp	s0,8(sp)
2005af0c:	0141                	c.addi	sp,16
2005af0e:	8082                	c.jr	ra

2005af10 <log_service_init>:
2005af10:	4c018793          	addi	a5,gp,1216 # 20066598 <log_hash>
2005af14:	10078713          	addi	a4,a5,256
2005af18:	c39c                	c.sw	a5,0(a5)
2005af1a:	c3dc                	c.sw	a5,4(a5)
2005af1c:	07a1                	c.addi	a5,8
2005af1e:	fee79de3          	bne	a5,a4,2005af18 <log_service_init+0x8>
2005af22:	e8c1a783          	lw	a5,-372(gp) # 20065f64 <log_init_table>
2005af26:	8782                	c.jr	a5

2005af28 <log_service_add_table>:
2005af28:	1141                	c.addi	sp,-16
2005af2a:	c422                	c.swsp	s0,8(sp)
2005af2c:	c226                	c.swsp	s1,4(sp)
2005af2e:	c04a                	c.swsp	s2,0(sp)
2005af30:	c606                	c.swsp	ra,12(sp)
2005af32:	892a                	c.mv	s2,a0
2005af34:	84ae                	c.mv	s1,a1
2005af36:	4401                	c.li	s0,0
2005af38:	00944863          	blt	s0,s1,2005af48 <log_service_add_table+0x20>
2005af3c:	40b2                	c.lwsp	ra,12(sp)
2005af3e:	4422                	c.lwsp	s0,8(sp)
2005af40:	4492                	c.lwsp	s1,4(sp)
2005af42:	4902                	c.lwsp	s2,0(sp)
2005af44:	0141                	c.addi	sp,16
2005af46:	8082                	c.jr	ra
2005af48:	00441513          	slli	a0,s0,0x4
2005af4c:	954a                	c.add	a0,s2
2005af4e:	3f41                	c.jal	2005aede <log_add_new_command>
2005af50:	0405                	c.addi	s0,1
2005af52:	b7dd                	c.j	2005af38 <log_service_add_table+0x10>

2005af54 <log_action>:
2005af54:	1141                	c.addi	sp,-16
2005af56:	c422                	c.swsp	s0,8(sp)
2005af58:	c226                	c.swsp	s1,4(sp)
2005af5a:	c04a                	c.swsp	s2,0(sp)
2005af5c:	c606                	c.swsp	ra,12(sp)
2005af5e:	892a                	c.mv	s2,a0
2005af60:	02000413          	addi	s0,zero,32
2005af64:	3fb1                	c.jal	2005aec0 <hash_index>
2005af66:	02856433          	rem	s0,a0,s0
2005af6a:	00341793          	slli	a5,s0,0x3
2005af6e:	4c018413          	addi	s0,gp,1216 # 20066598 <log_hash>
2005af72:	943e                	c.add	s0,a5
2005af74:	4004                	c.lw	s1,0(s0)
2005af76:	00849463          	bne	s1,s0,2005af7e <log_action+0x2a>
2005af7a:	4501                	c.li	a0,0
2005af7c:	a819                	c.j	2005af92 <log_action+0x3e>
2005af7e:	ff84a503          	lw	a0,-8(s1)
2005af82:	85ca                	c.mv	a1,s2
2005af84:	dffaf097          	auipc	ra,0xdffaf
2005af88:	b56080e7          	jalr	ra,-1194(ra) # 9ada <__wrap_strcmp>
2005af8c:	e909                	c.bnez	a0,2005af9e <log_action+0x4a>
2005af8e:	ffc4a503          	lw	a0,-4(s1)
2005af92:	40b2                	c.lwsp	ra,12(sp)
2005af94:	4422                	c.lwsp	s0,8(sp)
2005af96:	4492                	c.lwsp	s1,4(sp)
2005af98:	4902                	c.lwsp	s2,0(sp)
2005af9a:	0141                	c.addi	sp,16
2005af9c:	8082                	c.jr	ra
2005af9e:	4084                	c.lw	s1,0(s1)
2005afa0:	bfd9                	c.j	2005af76 <log_action+0x22>

2005afa2 <log_handler>:
2005afa2:	7135                	c.addi16sp	sp,-160
2005afa4:	cd22                	c.swsp	s0,152(sp)
2005afa6:	1060                	c.addi4spn	s0,sp,44
2005afa8:	cb26                	c.swsp	s1,148(sp)
2005afaa:	06400613          	addi	a2,zero,100
2005afae:	84aa                	c.mv	s1,a0
2005afb0:	4581                	c.li	a1,0
2005afb2:	8522                	c.mv	a0,s0
2005afb4:	cf06                	c.swsp	ra,156(sp)
2005afb6:	dffaf097          	auipc	ra,0xdffaf
2005afba:	b18080e7          	jalr	ra,-1256(ra) # 9ace <__wrap_memset>
2005afbe:	4675                	c.li	a2,29
2005afc0:	4581                	c.li	a1,0
2005afc2:	0068                	c.addi4spn	a0,sp,12
2005afc4:	c222                	c.swsp	s0,4(sp)
2005afc6:	c402                	c.swsp	zero,8(sp)
2005afc8:	dffaf097          	auipc	ra,0xdffaf
2005afcc:	b06080e7          	jalr	ra,-1274(ra) # 9ace <__wrap_memset>
2005afd0:	06300613          	addi	a2,zero,99
2005afd4:	85a6                	c.mv	a1,s1
2005afd6:	8522                	c.mv	a0,s0
2005afd8:	68c060ef          	jal	ra,20061664 <strncpy>
2005afdc:	200635b7          	lui	a1,0x20063
2005afe0:	8f458593          	addi	a1,a1,-1804 # 200628f4 <__FUNCTION__.0+0x1b4>
2005afe4:	0048                	c.addi4spn	a0,sp,4
2005afe6:	6e8060ef          	jal	ra,200616ce <strsep>
2005afea:	200635b7          	lui	a1,0x20063
2005afee:	842a                	c.mv	s0,a0
2005aff0:	94c58593          	addi	a1,a1,-1716 # 2006294c <__FUNCTION__.0+0x20c>
2005aff4:	0048                	c.addi4spn	a0,sp,4
2005aff6:	6d8060ef          	jal	ra,200616ce <strsep>
2005affa:	e419                	c.bnez	s0,2005b008 <log_handler+0x66>
2005affc:	4501                	c.li	a0,0
2005affe:	40fa                	c.lwsp	ra,156(sp)
2005b000:	446a                	c.lwsp	s0,152(sp)
2005b002:	44da                	c.lwsp	s1,148(sp)
2005b004:	610d                	c.addi16sp	sp,160
2005b006:	8082                	c.jr	ra
2005b008:	84aa                	c.mv	s1,a0
2005b00a:	8522                	c.mv	a0,s0
2005b00c:	dffaf097          	auipc	ra,0xdffaf
2005b010:	acc080e7          	jalr	ra,-1332(ra) # 9ad8 <__wrap_strlen>
2005b014:	478d                	c.li	a5,3
2005b016:	fea7f3e3          	bgeu	a5,a0,2005affc <log_handler+0x5a>
2005b01a:	02000613          	addi	a2,zero,32
2005b01e:	85a2                	c.mv	a1,s0
2005b020:	0028                	c.addi4spn	a0,sp,8
2005b022:	642060ef          	jal	ra,20061664 <strncpy>
2005b026:	00814703          	lbu	a4,8(sp)
2005b02a:	04100793          	addi	a5,zero,65
2005b02e:	fcf717e3          	bne	a4,a5,2005affc <log_handler+0x5a>
2005b032:	00914703          	lbu	a4,9(sp)
2005b036:	05400793          	addi	a5,zero,84
2005b03a:	fcf711e3          	bne	a4,a5,2005affc <log_handler+0x5a>
2005b03e:	00a10513          	addi	a0,sp,10
2005b042:	3f09                	c.jal	2005af54 <log_action>
2005b044:	842a                	c.mv	s0,a0
2005b046:	4501                	c.li	a0,0
2005b048:	d85d                	c.beqz	s0,2005affe <log_handler+0x5c>
2005b04a:	8526                	c.mv	a0,s1
2005b04c:	9402                	c.jalr	s0
2005b04e:	8522                	c.mv	a0,s0
2005b050:	b77d                	c.j	2005affe <log_handler+0x5c>

2005b052 <parse_param>:
2005b052:	7175                	c.addi16sp	sp,-144
2005b054:	c326                	c.swsp	s1,132(sp)
2005b056:	c14a                	c.swsp	s2,128(sp)
2005b058:	06400613          	addi	a2,zero,100
2005b05c:	892a                	c.mv	s2,a0
2005b05e:	84ae                	c.mv	s1,a1
2005b060:	0068                	c.addi4spn	a0,sp,12
2005b062:	4581                	c.li	a1,0
2005b064:	c522                	c.swsp	s0,136(sp)
2005b066:	c706                	c.swsp	ra,140(sp)
2005b068:	dece                	c.swsp	s3,124(sp)
2005b06a:	dcd2                	c.swsp	s4,120(sp)
2005b06c:	dad6                	c.swsp	s5,116(sp)
2005b06e:	d8da                	c.swsp	s6,112(sp)
2005b070:	dffaf097          	auipc	ra,0xdffaf
2005b074:	a5e080e7          	jalr	ra,-1442(ra) # 9ace <__wrap_memset>
2005b078:	06400613          	addi	a2,zero,100
2005b07c:	4581                	c.li	a1,0
2005b07e:	5c018513          	addi	a0,gp,1472 # 20066698 <temp_buf.0>
2005b082:	dffaf097          	auipc	ra,0xdffaf
2005b086:	a4c080e7          	jalr	ra,-1460(ra) # 9ace <__wrap_memset>
2005b08a:	0c090e63          	beq	s2,zero,2005b166 <parse_param+0x114>
2005b08e:	85ca                	c.mv	a1,s2
2005b090:	5c018513          	addi	a0,gp,1472 # 20066698 <temp_buf.0>
2005b094:	06300613          	addi	a2,zero,99
2005b098:	5cc060ef          	jal	ra,20061664 <strncpy>
2005b09c:	0491                	c.addi	s1,4
2005b09e:	5c018413          	addi	s0,gp,1472 # 20066698 <temp_buf.0>
2005b0a2:	4905                	c.li	s2,1
2005b0a4:	02c00993          	addi	s3,zero,44
2005b0a8:	02200a13          	addi	s4,zero,34
2005b0ac:	05c00a93          	addi	s5,zero,92
2005b0b0:	4b5d                	c.li	s6,23
2005b0b2:	00044783          	lbu	a5,0(s0)
2005b0b6:	e789                	c.bnez	a5,2005b0c0 <parse_param+0x6e>
2005b0b8:	a8b5                	c.j	2005b134 <parse_param+0xe2>
2005b0ba:	00040023          	sb	zero,0(s0)
2005b0be:	0405                	c.addi	s0,1
2005b0c0:	00044783          	lbu	a5,0(s0)
2005b0c4:	ff378be3          	beq	a5,s3,2005b0ba <parse_param+0x68>
2005b0c8:	fa578713          	addi	a4,a5,-91
2005b0cc:	0fd77713          	andi	a4,a4,253
2005b0d0:	d76d                	c.beqz	a4,2005b0ba <parse_param+0x68>
2005b0d2:	c3ad                	c.beqz	a5,2005b134 <parse_param+0xe2>
2005b0d4:	09479763          	bne	a5,s4,2005b162 <parse_param+0x110>
2005b0d8:	06400613          	addi	a2,zero,100
2005b0dc:	4581                	c.li	a1,0
2005b0de:	0068                	c.addi4spn	a0,sp,12
2005b0e0:	dffaf097          	auipc	ra,0xdffaf
2005b0e4:	9ee080e7          	jalr	ra,-1554(ra) # 9ace <__wrap_memset>
2005b0e8:	00144783          	lbu	a5,1(s0)
2005b0ec:	00040023          	sb	zero,0(s0)
2005b0f0:	c3b1                	c.beqz	a5,2005b134 <parse_param+0xe2>
2005b0f2:	0405                	c.addi	s0,1
2005b0f4:	c080                	c.sw	s0,0(s1)
2005b0f6:	0078                	c.addi4spn	a4,sp,12
2005b0f8:	4601                	c.li	a2,0
2005b0fa:	00044783          	lbu	a5,0(s0)
2005b0fe:	01478363          	beq	a5,s4,2005b104 <parse_param+0xb2>
2005b102:	e7a1                	c.bnez	a5,2005b14a <parse_param+0xf8>
2005b104:	006c                	c.addi4spn	a1,sp,12
2005b106:	40c40533          	sub	a0,s0,a2
2005b10a:	00040023          	sb	zero,0(s0)
2005b10e:	dffaf097          	auipc	ra,0xdffaf
2005b112:	9c2080e7          	jalr	ra,-1598(ra) # 9ad0 <__wrap_memcpy>
2005b116:	0905                	c.addi	s2,1
2005b118:	00144783          	lbu	a5,1(s0)
2005b11c:	0405                	c.addi	s0,1
2005b11e:	01378863          	beq	a5,s3,2005b12e <parse_param+0xdc>
2005b122:	c791                	c.beqz	a5,2005b12e <parse_param+0xdc>
2005b124:	fa578793          	addi	a5,a5,-91
2005b128:	0fd7f793          	andi	a5,a5,253
2005b12c:	f7f5                	c.bnez	a5,2005b118 <parse_param+0xc6>
2005b12e:	0491                	c.addi	s1,4
2005b130:	f96911e3          	bne	s2,s6,2005b0b2 <parse_param+0x60>
2005b134:	40ba                	c.lwsp	ra,140(sp)
2005b136:	442a                	c.lwsp	s0,136(sp)
2005b138:	449a                	c.lwsp	s1,132(sp)
2005b13a:	59f6                	c.lwsp	s3,124(sp)
2005b13c:	5a66                	c.lwsp	s4,120(sp)
2005b13e:	5ad6                	c.lwsp	s5,116(sp)
2005b140:	5b46                	c.lwsp	s6,112(sp)
2005b142:	854a                	c.mv	a0,s2
2005b144:	490a                	c.lwsp	s2,128(sp)
2005b146:	6149                	c.addi16sp	sp,144
2005b148:	8082                	c.jr	ra
2005b14a:	01579463          	bne	a5,s5,2005b152 <parse_param+0x100>
2005b14e:	0605                	c.addi	a2,1
2005b150:	0405                	c.addi	s0,1
2005b152:	00044783          	lbu	a5,0(s0)
2005b156:	0605                	c.addi	a2,1
2005b158:	0405                	c.addi	s0,1
2005b15a:	00f70023          	sb	a5,0(a4)
2005b15e:	0705                	c.addi	a4,1
2005b160:	bf69                	c.j	2005b0fa <parse_param+0xa8>
2005b162:	c080                	c.sw	s0,0(s1)
2005b164:	bf4d                	c.j	2005b116 <parse_param+0xc4>
2005b166:	4905                	c.li	s2,1
2005b168:	b7f1                	c.j	2005b134 <parse_param+0xe2>

2005b16a <mp_commnad_handler>:
2005b16a:	7135                	c.addi16sp	sp,-160
2005b16c:	cd22                	c.swsp	s0,152(sp)
2005b16e:	0800                	c.addi4spn	s0,sp,16
2005b170:	cb26                	c.swsp	s1,148(sp)
2005b172:	08000613          	addi	a2,zero,128
2005b176:	84aa                	c.mv	s1,a0
2005b178:	4581                	c.li	a1,0
2005b17a:	8522                	c.mv	a0,s0
2005b17c:	cf06                	c.swsp	ra,156(sp)
2005b17e:	c622                	c.swsp	s0,12(sp)
2005b180:	dffaf097          	auipc	ra,0xdffaf
2005b184:	94e080e7          	jalr	ra,-1714(ra) # 9ace <__wrap_memset>
2005b188:	85a6                	c.mv	a1,s1
2005b18a:	07f00613          	addi	a2,zero,127
2005b18e:	8522                	c.mv	a0,s0
2005b190:	4d4060ef          	jal	ra,20061664 <strncpy>
2005b194:	200635b7          	lui	a1,0x20063
2005b198:	8d458593          	addi	a1,a1,-1836 # 200628d4 <__FUNCTION__.0+0x194>
2005b19c:	0068                	c.addi4spn	a0,sp,12
2005b19e:	530060ef          	jal	ra,200616ce <strsep>
2005b1a2:	cd15                	c.beqz	a0,2005b1de <mp_commnad_handler+0x74>
2005b1a4:	200635b7          	lui	a1,0x20063
2005b1a8:	8522                	c.mv	a0,s0
2005b1aa:	8f858593          	addi	a1,a1,-1800 # 200628f8 <__FUNCTION__.0+0x1b8>
2005b1ae:	dffaf097          	auipc	ra,0xdffaf
2005b1b2:	92c080e7          	jalr	ra,-1748(ra) # 9ada <__wrap_strcmp>
2005b1b6:	842a                	c.mv	s0,a0
2005b1b8:	e11d                	c.bnez	a0,2005b1de <mp_commnad_handler+0x74>
2005b1ba:	200635b7          	lui	a1,0x20063
2005b1be:	f8858593          	addi	a1,a1,-120 # 20062f88 <__FUNCTION__.0+0x90>
2005b1c2:	0068                	c.addi4spn	a0,sp,12
2005b1c4:	50a060ef          	jal	ra,200616ce <strsep>
2005b1c8:	4605                	c.li	a2,1
2005b1ca:	08000593          	addi	a1,zero,128
2005b1ce:	414050ef          	jal	ra,200605e2 <inic_mp_command>
2005b1d2:	40fa                	c.lwsp	ra,156(sp)
2005b1d4:	8522                	c.mv	a0,s0
2005b1d6:	446a                	c.lwsp	s0,152(sp)
2005b1d8:	44da                	c.lwsp	s1,148(sp)
2005b1da:	610d                	c.addi16sp	sp,160
2005b1dc:	8082                	c.jr	ra
2005b1de:	547d                	c.li	s0,-1
2005b1e0:	bfcd                	c.j	2005b1d2 <mp_commnad_handler+0x68>

2005b1e2 <log_service>:
2005b1e2:	1141                	c.addi	sp,-16
2005b1e4:	c422                	c.swsp	s0,8(sp)
2005b1e6:	c606                	c.swsp	ra,12(sp)
2005b1e8:	842a                	c.mv	s0,a0
2005b1ea:	db9ff0ef          	jal	ra,2005afa2 <log_handler>
2005b1ee:	ed01                	c.bnez	a0,2005b206 <log_service+0x24>
2005b1f0:	8522                	c.mv	a0,s0
2005b1f2:	3fa5                	c.jal	2005b16a <mp_commnad_handler>
2005b1f4:	00055963          	bge	a0,zero,2005b206 <log_service+0x24>
2005b1f8:	20063537          	lui	a0,0x20063
2005b1fc:	85a2                	c.mv	a1,s0
2005b1fe:	90050513          	addi	a0,a0,-1792 # 20062900 <__FUNCTION__.0+0x1c0>
2005b202:	64d030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b206:	23b5                	c.jal	2005b772 <rtos_mem_get_free_heap_size>
2005b208:	85aa                	c.mv	a1,a0
2005b20a:	20063537          	lui	a0,0x20063
2005b20e:	91850513          	addi	a0,a0,-1768 # 20062918 <__FUNCTION__.0+0x1d8>
2005b212:	63d030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b216:	4422                	c.lwsp	s0,8(sp)
2005b218:	40b2                	c.lwsp	ra,12(sp)
2005b21a:	20063537          	lui	a0,0x20063
2005b21e:	94450513          	addi	a0,a0,-1724 # 20062944 <__FUNCTION__.0+0x204>
2005b222:	0141                	c.addi	sp,16
2005b224:	62b0306f          	jal	zero,2005f04e <DiagPrintf_minimal>

2005b228 <app_mbedtls_free_func>:
2005b228:	a389                	c.j	2005b76a <rtos_mem_free>

2005b22a <app_mbedtls_calloc_func>:
2005b22a:	02b50633          	mul	a2,a0,a1
2005b22e:	1101                	c.addi	sp,-32
2005b230:	cc22                	c.swsp	s0,24(sp)
2005b232:	ce06                	c.swsp	ra,28(sp)
2005b234:	8532                	c.mv	a0,a2
2005b236:	c632                	c.swsp	a2,12(sp)
2005b238:	2321                	c.jal	2005b740 <rtos_mem_malloc>
2005b23a:	842a                	c.mv	s0,a0
2005b23c:	c519                	c.beqz	a0,2005b24a <app_mbedtls_calloc_func+0x20>
2005b23e:	4632                	c.lwsp	a2,12(sp)
2005b240:	4581                	c.li	a1,0
2005b242:	dffaf097          	auipc	ra,0xdffaf
2005b246:	88c080e7          	jalr	ra,-1908(ra) # 9ace <__wrap_memset>
2005b24a:	40f2                	c.lwsp	ra,28(sp)
2005b24c:	8522                	c.mv	a0,s0
2005b24e:	4462                	c.lwsp	s0,24(sp)
2005b250:	6105                	c.addi16sp	sp,32
2005b252:	8082                	c.jr	ra

2005b254 <app_IWDG_refresh>:
2005b254:	4100d537          	lui	a0,0x4100d
2005b258:	c0050513          	addi	a0,a0,-1024 # 4100cc00 <__km4_bd_boot_download_addr__+0x10ffac00>
2005b25c:	dffad317          	auipc	t1,0xdffad
2005b260:	fa630067          	jalr	zero,-90(t1) # 8202 <WDG_Refresh>

2005b264 <app_init_debug>:
2005b264:	1141                	c.addi	sp,-16
2005b266:	c606                	c.swsp	ra,12(sp)
2005b268:	dffac097          	auipc	ra,0xdffac
2005b26c:	938080e7          	jalr	ra,-1736(ra) # 6ba0 <SYSCFG_OTP_DisBootLog>
2005b270:	00153513          	sltiu	a0,a0,1
2005b274:	200017b7          	lui	a5,0x20001
2005b278:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b27c:	0506                	c.slli	a0,0x1
2005b27e:	577d                	c.li	a4,-1
2005b280:	c398                	c.sw	a4,0(a5)
2005b282:	0007a223          	sw	zero,4(a5)
2005b286:	c788                	c.sw	a0,8(a5)
2005b288:	0007a623          	sw	zero,12(a5)
2005b28c:	40b2                	c.lwsp	ra,12(sp)
2005b28e:	0141                	c.addi	sp,16
2005b290:	8082                	c.jr	ra

2005b292 <app_mbedtls_rom_init>:
2005b292:	2005b5b7          	lui	a1,0x2005b
2005b296:	2005b537          	lui	a0,0x2005b
2005b29a:	1141                	c.addi	sp,-16
2005b29c:	22858593          	addi	a1,a1,552 # 2005b228 <app_mbedtls_free_func>
2005b2a0:	22a50513          	addi	a0,a0,554 # 2005b22a <app_mbedtls_calloc_func>
2005b2a4:	c606                	c.swsp	ra,12(sp)
2005b2a6:	14d000ef          	jal	ra,2005bbf2 <mbedtls_platform_set_calloc_free>
2005b2aa:	40b2                	c.lwsp	ra,12(sp)
2005b2ac:	ee01ac23          	sw	zero,-264(gp) # 20065fd0 <use_hw_crypto_func>
2005b2b0:	0141                	c.addi	sp,16
2005b2b2:	8082                	c.jr	ra

2005b2b4 <app_pmu_init>:
2005b2b4:	1141                	c.addi	sp,-16
2005b2b6:	200017b7          	lui	a5,0x20001
2005b2ba:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b2be:	c606                	c.swsp	ra,12(sp)
2005b2c0:	4798                	c.lw	a4,8(a5)
2005b2c2:	100006b7          	lui	a3,0x10000
2005b2c6:	4501                	c.li	a0,0
2005b2c8:	8f55                	c.or	a4,a3
2005b2ca:	c798                	c.sw	a4,8(a5)
2005b2cc:	d0afe0ef          	jal	ra,200597d6 <pmu_acquire_wakelock>
2005b2d0:	4100c7b7          	lui	a5,0x4100c
2005b2d4:	22c7a783          	lw	a5,556(a5) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
2005b2d8:	8b89                	c.andi	a5,2
2005b2da:	c38d                	c.beqz	a5,2005b2fc <app_pmu_init+0x48>
2005b2dc:	4509                	c.li	a0,2
2005b2de:	cf8fe0ef          	jal	ra,200597d6 <pmu_acquire_wakelock>
2005b2e2:	8ddfe0ef          	jal	ra,20059bbe <SWR_CORE_Vol_Get>
2005b2e6:	4785                	c.li	a5,1
2005b2e8:	00f51a63          	bne	a0,a5,2005b2fc <app_pmu_init+0x48>
2005b2ec:	8c2fe0ef          	jal	ra,200593ae <dsp_status_on>
2005b2f0:	c511                	c.beqz	a0,2005b2fc <app_pmu_init+0x48>
2005b2f2:	40b2                	c.lwsp	ra,12(sp)
2005b2f4:	4511                	c.li	a0,4
2005b2f6:	0141                	c.addi	sp,16
2005b2f8:	cdefe06f          	jal	zero,200597d6 <pmu_acquire_wakelock>
2005b2fc:	40b2                	c.lwsp	ra,12(sp)
2005b2fe:	0141                	c.addi	sp,16
2005b300:	8082                	c.jr	ra

2005b302 <app_IWDG_int>:
2005b302:	4100c7b7          	lui	a5,0x4100c
2005b306:	479c                	c.lw	a5,8(a5)
2005b308:	c0000737          	lui	a4,0xc0000
2005b30c:	0709                	c.addi	a4,2
2005b30e:	8ff9                	c.and	a5,a4
2005b310:	c7b5                	c.beqz	a5,2005b37c <app_IWDG_int+0x7a>
2005b312:	1101                	c.addi	sp,-32
2005b314:	cc22                	c.swsp	s0,24(sp)
2005b316:	200636b7          	lui	a3,0x20063
2005b31a:	20063437          	lui	s0,0x20063
2005b31e:	95068693          	addi	a3,a3,-1712 # 20062950 <__FUNCTION__.0+0x210>
2005b322:	04900613          	addi	a2,zero,73
2005b326:	96440593          	addi	a1,s0,-1692 # 20062964 <__FUNCTION__.0+0x224>
2005b32a:	4511                	c.li	a0,4
2005b32c:	ce06                	c.swsp	ra,28(sp)
2005b32e:	b43ff0ef          	jal	ra,2005ae70 <rtk_log_write>
2005b332:	2005b7b7          	lui	a5,0x2005b
2005b336:	200635b7          	lui	a1,0x20063
2005b33a:	25478793          	addi	a5,a5,596 # 2005b254 <app_IWDG_refresh>
2005b33e:	4705                	c.li	a4,1
2005b340:	1f400693          	addi	a3,zero,500
2005b344:	4601                	c.li	a2,0
2005b346:	96c58593          	addi	a1,a1,-1684 # 2006296c <__FUNCTION__.0+0x22c>
2005b34a:	0068                	c.addi4spn	a0,sp,12
2005b34c:	c602                	c.swsp	zero,12(sp)
2005b34e:	003000ef          	jal	ra,2005bb50 <rtos_timer_create>
2005b352:	4532                	c.lwsp	a0,12(sp)
2005b354:	e105                	c.bnez	a0,2005b374 <app_IWDG_int+0x72>
2005b356:	200636b7          	lui	a3,0x20063
2005b35a:	97868693          	addi	a3,a3,-1672 # 20062978 <__FUNCTION__.0+0x238>
2005b35e:	04500613          	addi	a2,zero,69
2005b362:	96440593          	addi	a1,s0,-1692
2005b366:	4509                	c.li	a0,2
2005b368:	b09ff0ef          	jal	ra,2005ae70 <rtk_log_write>
2005b36c:	40f2                	c.lwsp	ra,28(sp)
2005b36e:	4462                	c.lwsp	s0,24(sp)
2005b370:	6105                	c.addi16sp	sp,32
2005b372:	8082                	c.jr	ra
2005b374:	4581                	c.li	a1,0
2005b376:	00f000ef          	jal	ra,2005bb84 <rtos_timer_start>
2005b37a:	bfcd                	c.j	2005b36c <app_IWDG_int+0x6a>
2005b37c:	8082                	c.jr	ra

2005b37e <app_pre_example>:
2005b37e:	8082                	c.jr	ra

2005b380 <app_example>:
2005b380:	8082                	c.jr	ra

2005b382 <main>:
2005b382:	20059537          	lui	a0,0x20059
2005b386:	1141                	c.addi	sp,-16
2005b388:	4691                	c.li	a3,4
2005b38a:	41020637          	lui	a2,0x41020
2005b38e:	02b00593          	addi	a1,zero,43
2005b392:	17e50513          	addi	a0,a0,382 # 2005917e <IPC_INTHandler>
2005b396:	c606                	c.swsp	ra,12(sp)
2005b398:	1ec040ef          	jal	ra,2005f584 <irq_register>
2005b39c:	02b00513          	addi	a0,zero,43
2005b3a0:	dffad097          	auipc	ra,0xdffad
2005b3a4:	c38080e7          	jalr	ra,-968(ra) # 7fd8 <irq_enable>
2005b3a8:	35ed                	c.jal	2005b292 <app_mbedtls_rom_init>
2005b3aa:	4581                	c.li	a1,0
2005b3ac:	4501                	c.li	a0,0
2005b3ae:	dffae097          	auipc	ra,0xdffae
2005b3b2:	ea6080e7          	jalr	ra,-346(ra) # 9254 <shell_init_rom>
2005b3b6:	2fd010ef          	jal	ra,2005ceb2 <shell_init_ram>
2005b3ba:	41020537          	lui	a0,0x41020
2005b3be:	9b9fd0ef          	jal	ra,20058d76 <ipc_table_init>
2005b3c2:	2005c537          	lui	a0,0x2005c
2005b3c6:	af650513          	addi	a0,a0,-1290 # 2005baf6 <rtos_time_delay_ms>
2005b3ca:	f97fd0ef          	jal	ra,20059360 <IPC_SEMDelayStub>
2005b3ce:	3f45                	c.jal	2005b37e <app_pre_example>
2005b3d0:	31c030ef          	jal	ra,2005e6ec <wlan_initialize>
2005b3d4:	3d41                	c.jal	2005b264 <app_init_debug>
2005b3d6:	3df9                	c.jal	2005b2b4 <app_pmu_init>
2005b3d8:	372d                	c.jal	2005b302 <app_IWDG_int>
2005b3da:	375d                	c.jal	2005b380 <app_example>
2005b3dc:	200636b7          	lui	a3,0x20063
2005b3e0:	200635b7          	lui	a1,0x20063
2005b3e4:	98c68693          	addi	a3,a3,-1652 # 2006298c <__FUNCTION__.0+0x24c>
2005b3e8:	04900613          	addi	a2,zero,73
2005b3ec:	96458593          	addi	a1,a1,-1692 # 20062964 <__FUNCTION__.0+0x224>
2005b3f0:	4511                	c.li	a0,4
2005b3f2:	a7fff0ef          	jal	ra,2005ae70 <rtk_log_write>
2005b3f6:	694000ef          	jal	ra,2005ba8a <rtos_sched_start>
2005b3fa:	40b2                	c.lwsp	ra,12(sp)
2005b3fc:	4501                	c.li	a0,0
2005b3fe:	0141                	c.addi	sp,16
2005b400:	8082                	c.jr	ra

2005b402 <CmdRamHelp>:
2005b402:	1101                	c.addi	sp,-32
2005b404:	cc22                	c.swsp	s0,24(sp)
2005b406:	ca26                	c.swsp	s1,20(sp)
2005b408:	20064437          	lui	s0,0x20064
2005b40c:	200644b7          	lui	s1,0x20064
2005b410:	38c48793          	addi	a5,s1,908 # 2006438c <shell_cmd_table>
2005b414:	3cc40413          	addi	s0,s0,972 # 200643cc <ipc_tickless_table>
2005b418:	8c1d                	c.sub	s0,a5
2005b41a:	8011                	c.srli	s0,0x4
2005b41c:	20063537          	lui	a0,0x20063
2005b420:	85a2                	c.mv	a1,s0
2005b422:	99c50513          	addi	a0,a0,-1636 # 2006299c <__FUNCTION__.0+0x25c>
2005b426:	c84a                	c.swsp	s2,16(sp)
2005b428:	c64e                	c.swsp	s3,12(sp)
2005b42a:	ce06                	c.swsp	ra,28(sp)
2005b42c:	38c48493          	addi	s1,s1,908
2005b430:	41f030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b434:	4901                	c.li	s2,0
2005b436:	200639b7          	lui	s3,0x20063
2005b43a:	02891163          	bne	s2,s0,2005b45c <CmdRamHelp+0x5a>
2005b43e:	20063537          	lui	a0,0x20063
2005b442:	85ca                	c.mv	a1,s2
2005b444:	9e050513          	addi	a0,a0,-1568 # 200629e0 <__FUNCTION__.0+0x2a0>
2005b448:	407030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b44c:	40f2                	c.lwsp	ra,28(sp)
2005b44e:	4462                	c.lwsp	s0,24(sp)
2005b450:	44d2                	c.lwsp	s1,20(sp)
2005b452:	4942                	c.lwsp	s2,16(sp)
2005b454:	49b2                	c.lwsp	s3,12(sp)
2005b456:	4505                	c.li	a0,1
2005b458:	6105                	c.addi16sp	sp,32
2005b45a:	8082                	c.jr	ra
2005b45c:	44cc                	c.lw	a1,12(s1)
2005b45e:	c589                	c.beqz	a1,2005b468 <CmdRamHelp+0x66>
2005b460:	9dc98513          	addi	a0,s3,-1572 # 200629dc <__FUNCTION__.0+0x29c>
2005b464:	3eb030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b468:	0905                	c.addi	s2,1
2005b46a:	04c1                	c.addi	s1,16
2005b46c:	b7f9                	c.j	2005b43a <CmdRamHelp+0x38>

2005b46e <cmd_reboot>:
2005b46e:	1141                	c.addi	sp,-16
2005b470:	c226                	c.swsp	s1,4(sp)
2005b472:	84aa                	c.mv	s1,a0
2005b474:	20063537          	lui	a0,0x20063
2005b478:	a2050513          	addi	a0,a0,-1504 # 20062a20 <__FUNCTION__.0+0x2e0>
2005b47c:	c422                	c.swsp	s0,8(sp)
2005b47e:	c606                	c.swsp	ra,12(sp)
2005b480:	842e                	c.mv	s0,a1
2005b482:	3cd030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b486:	c08d                	c.beqz	s1,2005b4a8 <cmd_reboot+0x3a>
2005b488:	4008                	c.lw	a0,0(s0)
2005b48a:	200635b7          	lui	a1,0x20063
2005b48e:	a3058593          	addi	a1,a1,-1488 # 20062a30 <__FUNCTION__.0+0x2f0>
2005b492:	dffae097          	auipc	ra,0xdffae
2005b496:	39a080e7          	jalr	ra,922(ra) # 982c <_strcmp>
2005b49a:	e519                	c.bnez	a0,2005b4a8 <cmd_reboot+0x3a>
2005b49c:	20000593          	addi	a1,zero,512
2005b4a0:	dffa5097          	auipc	ra,0xdffa5
2005b4a4:	406080e7          	jalr	ra,1030(ra) # 8a6 <BKUP_Set>
2005b4a8:	ecafe0ef          	jal	ra,20059b72 <System_Reset>
2005b4ac:	40b2                	c.lwsp	ra,12(sp)
2005b4ae:	4422                	c.lwsp	s0,8(sp)
2005b4b0:	4492                	c.lwsp	s1,4(sp)
2005b4b2:	4505                	c.li	a0,1
2005b4b4:	0141                	c.addi	sp,16
2005b4b6:	8082                	c.jr	ra

2005b4b8 <cmd_write_word>:
2005b4b8:	1141                	c.addi	sp,-16
2005b4ba:	c606                	c.swsp	ra,12(sp)
2005b4bc:	c422                	c.swsp	s0,8(sp)
2005b4be:	c226                	c.swsp	s1,4(sp)
2005b4c0:	c04a                	c.swsp	s2,0(sp)
2005b4c2:	892e                	c.mv	s2,a1
2005b4c4:	00092503          	lw	a0,0(s2)
2005b4c8:	4641                	c.li	a2,16
2005b4ca:	4581                	c.li	a1,0
2005b4cc:	dffae097          	auipc	ra,0xdffae
2005b4d0:	46a080e7          	jalr	ra,1130(ra) # 9936 <_strtoul>
2005b4d4:	ffc57413          	andi	s0,a0,-4
2005b4d8:	84aa                	c.mv	s1,a0
2005b4da:	00492503          	lw	a0,4(s2)
2005b4de:	4641                	c.li	a2,16
2005b4e0:	4581                	c.li	a1,0
2005b4e2:	dffae097          	auipc	ra,0xdffae
2005b4e6:	454080e7          	jalr	ra,1108(ra) # 9936 <_strtoul>
2005b4ea:	892a                	c.mv	s2,a0
2005b4ec:	862a                	c.mv	a2,a0
2005b4ee:	20063537          	lui	a0,0x20063
2005b4f2:	85a2                	c.mv	a1,s0
2005b4f4:	a3c50513          	addi	a0,a0,-1476 # 20062a3c <__FUNCTION__.0+0x2fc>
2005b4f8:	357030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b4fc:	01242023          	sw	s2,0(s0)
2005b500:	7c1027f3          	csrrs	a5,0x7c1,zero
2005b504:	1007f793          	andi	a5,a5,256
2005b508:	c7a1                	c.beqz	a5,2005b550 <cmd_write_word+0x98>
2005b50a:	01c4f793          	andi	a5,s1,28
2005b50e:	4711                	c.li	a4,4
2005b510:	c789                	c.beqz	a5,2005b51a <cmd_write_word+0x62>
2005b512:	00478713          	addi	a4,a5,4
2005b516:	fe04f413          	andi	s0,s1,-32
2005b51a:	01f47793          	andi	a5,s0,31
2005b51e:	97ba                	c.add	a5,a4
2005b520:	0ff0000f          	fence	iorw,iorw
2005b524:	97a2                	c.add	a5,s0
2005b526:	0484700b          	cache	dwb,(s0)
2005b52a:	02040413          	addi	s0,s0,32
2005b52e:	40878733          	sub	a4,a5,s0
2005b532:	fee04ae3          	blt	zero,a4,2005b526 <cmd_write_word+0x6e>
2005b536:	0ff0000f          	fence	iorw,iorw
2005b53a:	0001                	c.addi	zero,0
2005b53c:	0001                	c.addi	zero,0
2005b53e:	0001                	c.addi	zero,0
2005b540:	0001                	c.addi	zero,0
2005b542:	0001                	c.addi	zero,0
2005b544:	0001                	c.addi	zero,0
2005b546:	0001                	c.addi	zero,0
2005b548:	0001                	c.addi	zero,0
2005b54a:	0001                	c.addi	zero,0
2005b54c:	0001                	c.addi	zero,0
2005b54e:	0001                	c.addi	zero,0
2005b550:	40b2                	c.lwsp	ra,12(sp)
2005b552:	4422                	c.lwsp	s0,8(sp)
2005b554:	4492                	c.lwsp	s1,4(sp)
2005b556:	4902                	c.lwsp	s2,0(sp)
2005b558:	4501                	c.li	a0,0
2005b55a:	0141                	c.addi	sp,16
2005b55c:	8082                	c.jr	ra

2005b55e <cmd_dump_word>:
2005b55e:	1101                	c.addi	sp,-32
2005b560:	cc22                	c.swsp	s0,24(sp)
2005b562:	c84a                	c.swsp	s2,16(sp)
2005b564:	842a                	c.mv	s0,a0
2005b566:	ce06                	c.swsp	ra,28(sp)
2005b568:	ca26                	c.swsp	s1,20(sp)
2005b56a:	c64e                	c.swsp	s3,12(sp)
2005b56c:	892e                	c.mv	s2,a1
2005b56e:	dffab097          	auipc	ra,0xdffab
2005b572:	5e0080e7          	jalr	ra,1504(ra) # 6b4e <SYSCFG_OTP_RSIPEn>
2005b576:	fff40793          	addi	a5,s0,-1
2005b57a:	07c2                	c.slli	a5,0x10
2005b57c:	83c1                	c.srli	a5,0x10
2005b57e:	4709                	c.li	a4,2
2005b580:	00f77a63          	bgeu	a4,a5,2005b594 <cmd_dump_word+0x36>
2005b584:	20063537          	lui	a0,0x20063
2005b588:	a4c50513          	addi	a0,a0,-1460 # 20062a4c <__FUNCTION__.0+0x30c>
2005b58c:	2c3030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b590:	4401                	c.li	s0,0
2005b592:	a0f9                	c.j	2005b660 <cmd_dump_word+0x102>
2005b594:	478d                	c.li	a5,3
2005b596:	89aa                	c.mv	s3,a0
2005b598:	06f41063          	bne	s0,a5,2005b5f8 <cmd_dump_word+0x9a>
2005b59c:	00892783          	lw	a5,8(s2)
2005b5a0:	0007c403          	lbu	s0,0(a5)
2005b5a4:	0df47413          	andi	s0,s0,223
2005b5a8:	fbe40413          	addi	s0,s0,-66
2005b5ac:	00143413          	sltiu	s0,s0,1
2005b5b0:	00492503          	lw	a0,4(s2)
2005b5b4:	4629                	c.li	a2,10
2005b5b6:	4581                	c.li	a1,0
2005b5b8:	dffae097          	auipc	ra,0xdffae
2005b5bc:	37e080e7          	jalr	ra,894(ra) # 9936 <_strtoul>
2005b5c0:	84aa                	c.mv	s1,a0
2005b5c2:	00092503          	lw	a0,0(s2)
2005b5c6:	4641                	c.li	a2,16
2005b5c8:	4581                	c.li	a1,0
2005b5ca:	dffae097          	auipc	ra,0xdffae
2005b5ce:	36c080e7          	jalr	ra,876(ra) # 9936 <_strtoul>
2005b5d2:	87aa                	c.mv	a5,a0
2005b5d4:	f8000737          	lui	a4,0xf8000
2005b5d8:	9971                	c.andi	a0,-4
2005b5da:	972a                	c.add	a4,a0
2005b5dc:	080006b7          	lui	a3,0x8000
2005b5e0:	02d77463          	bgeu	a4,a3,2005b608 <cmd_dump_word+0xaa>
2005b5e4:	02098263          	beq	s3,zero,2005b608 <cmd_dump_word+0xaa>
2005b5e8:	20063537          	lui	a0,0x20063
2005b5ec:	a6850513          	addi	a0,a0,-1432 # 20062a68 <__FUNCTION__.0+0x328>
2005b5f0:	25f030ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005b5f4:	4405                	c.li	s0,1
2005b5f6:	a0ad                	c.j	2005b660 <cmd_dump_word+0x102>
2005b5f8:	4785                	c.li	a5,1
2005b5fa:	00f40463          	beq	s0,a5,2005b602 <cmd_dump_word+0xa4>
2005b5fe:	4401                	c.li	s0,0
2005b600:	bf45                	c.j	2005b5b0 <cmd_dump_word+0x52>
2005b602:	4401                	c.li	s0,0
2005b604:	4485                	c.li	s1,1
2005b606:	bf75                	c.j	2005b5c2 <cmd_dump_word+0x64>
2005b608:	c43d                	c.beqz	s0,2005b676 <cmd_dump_word+0x118>
2005b60a:	c4f9                	c.beqz	s1,2005b6d8 <cmd_dump_word+0x17a>
2005b60c:	7c102773          	csrrs	a4,0x7c1,zero
2005b610:	10077713          	andi	a4,a4,256
2005b614:	c339                	c.beqz	a4,2005b65a <cmd_dump_word+0xfc>
2005b616:	01c7f713          	andi	a4,a5,28
2005b61a:	cb39                	c.beqz	a4,2005b670 <cmd_dump_word+0x112>
2005b61c:	9b81                	c.andi	a5,-32
2005b61e:	9726                	c.add	a4,s1
2005b620:	02e05d63          	bge	zero,a4,2005b65a <cmd_dump_word+0xfc>
2005b624:	01f7f693          	andi	a3,a5,31
2005b628:	9736                	c.add	a4,a3
2005b62a:	0ff0000f          	fence	iorw,iorw
2005b62e:	973e                	c.add	a4,a5
2005b630:	02f7f00b          	cache	dwbinv,(a5)
2005b634:	02078793          	addi	a5,a5,32
2005b638:	40f706b3          	sub	a3,a4,a5
2005b63c:	fed04ae3          	blt	zero,a3,2005b630 <cmd_dump_word+0xd2>
2005b640:	0ff0000f          	fence	iorw,iorw
2005b644:	0001                	c.addi	zero,0
2005b646:	0001                	c.addi	zero,0
2005b648:	0001                	c.addi	zero,0
2005b64a:	0001                	c.addi	zero,0
2005b64c:	0001                	c.addi	zero,0
2005b64e:	0001                	c.addi	zero,0
2005b650:	0001                	c.addi	zero,0
2005b652:	0001                	c.addi	zero,0
2005b654:	0001                	c.addi	zero,0
2005b656:	0001                	c.addi	zero,0
2005b658:	0001                	c.addi	zero,0
2005b65a:	85a6                	c.mv	a1,s1
2005b65c:	fa0ff0ef          	jal	ra,2005adfc <rtk_log_memory_dump_byte>
2005b660:	40f2                	c.lwsp	ra,28(sp)
2005b662:	8522                	c.mv	a0,s0
2005b664:	4462                	c.lwsp	s0,24(sp)
2005b666:	44d2                	c.lwsp	s1,20(sp)
2005b668:	4942                	c.lwsp	s2,16(sp)
2005b66a:	49b2                	c.lwsp	s3,12(sp)
2005b66c:	6105                	c.addi16sp	sp,32
2005b66e:	8082                	c.jr	ra
2005b670:	8726                	c.mv	a4,s1
2005b672:	87aa                	c.mv	a5,a0
2005b674:	b775                	c.j	2005b620 <cmd_dump_word+0xc2>
2005b676:	c0ad                	c.beqz	s1,2005b6d8 <cmd_dump_word+0x17a>
2005b678:	7c102773          	csrrs	a4,0x7c1,zero
2005b67c:	10077713          	andi	a4,a4,256
2005b680:	cb21                	c.beqz	a4,2005b6d0 <cmd_dump_word+0x172>
2005b682:	01c7f693          	andi	a3,a5,28
2005b686:	00249613          	slli	a2,s1,0x2
2005b68a:	872a                	c.mv	a4,a0
2005b68c:	c681                	c.beqz	a3,2005b694 <cmd_dump_word+0x136>
2005b68e:	fe07f713          	andi	a4,a5,-32
2005b692:	9636                	c.add	a2,a3
2005b694:	02c05e63          	bge	zero,a2,2005b6d0 <cmd_dump_word+0x172>
2005b698:	01f77693          	andi	a3,a4,31
2005b69c:	96b2                	c.add	a3,a2
2005b69e:	0ff0000f          	fence	iorw,iorw
2005b6a2:	87ba                	c.mv	a5,a4
2005b6a4:	9736                	c.add	a4,a3
2005b6a6:	02f7f00b          	cache	dwbinv,(a5)
2005b6aa:	02078793          	addi	a5,a5,32
2005b6ae:	40f706b3          	sub	a3,a4,a5
2005b6b2:	fed04ae3          	blt	zero,a3,2005b6a6 <cmd_dump_word+0x148>
2005b6b6:	0ff0000f          	fence	iorw,iorw
2005b6ba:	0001                	c.addi	zero,0
2005b6bc:	0001                	c.addi	zero,0
2005b6be:	0001                	c.addi	zero,0
2005b6c0:	0001                	c.addi	zero,0
2005b6c2:	0001                	c.addi	zero,0
2005b6c4:	0001                	c.addi	zero,0
2005b6c6:	0001                	c.addi	zero,0
2005b6c8:	0001                	c.addi	zero,0
2005b6ca:	0001                	c.addi	zero,0
2005b6cc:	0001                	c.addi	zero,0
2005b6ce:	0001                	c.addi	zero,0
2005b6d0:	85a6                	c.mv	a1,s1
2005b6d2:	eb8ff0ef          	jal	ra,2005ad8a <rtk_log_memory_dump_word>
2005b6d6:	bf39                	c.j	2005b5f4 <cmd_dump_word+0x96>
2005b6d8:	20063537          	lui	a0,0x20063
2005b6dc:	a8c50513          	addi	a0,a0,-1396 # 20062a8c <__FUNCTION__.0+0x34c>
2005b6e0:	b575                	c.j	2005b58c <cmd_dump_word+0x2e>

2005b6e2 <rtos_critical_is_in_interrupt>:
2005b6e2:	1141                	c.addi	sp,-16
2005b6e4:	c606                	c.swsp	ra,12(sp)
2005b6e6:	5cc0a0ef          	jal	ra,20065cb2 <plic_get_active_irq_id>
2005b6ea:	40b2                	c.lwsp	ra,12(sp)
2005b6ec:	00a03533          	sltu	a0,zero,a0
2005b6f0:	0141                	c.addi	sp,16
2005b6f2:	8082                	c.jr	ra

2005b6f4 <rtos_critical_enter>:
2005b6f4:	1141                	c.addi	sp,-16
2005b6f6:	c606                	c.swsp	ra,12(sp)
2005b6f8:	37ed                	c.jal	2005b6e2 <rtos_critical_is_in_interrupt>
2005b6fa:	c919                	c.beqz	a0,2005b710 <rtos_critical_enter+0x1c>
2005b6fc:	efc18793          	addi	a5,gp,-260 # 20065fd4 <uxCriticalNestingCnt>
2005b700:	0007d703          	lhu	a4,0(a5)
2005b704:	0705                	c.addi	a4,1
2005b706:	00e79023          	sh	a4,0(a5)
2005b70a:	40b2                	c.lwsp	ra,12(sp)
2005b70c:	0141                	c.addi	sp,16
2005b70e:	8082                	c.jr	ra
2005b710:	40b2                	c.lwsp	ra,12(sp)
2005b712:	0141                	c.addi	sp,16
2005b714:	2440206f          	jal	zero,2005d958 <vTaskEnterCritical>

2005b718 <rtos_critical_exit>:
2005b718:	1141                	c.addi	sp,-16
2005b71a:	c606                	c.swsp	ra,12(sp)
2005b71c:	37d9                	c.jal	2005b6e2 <rtos_critical_is_in_interrupt>
2005b71e:	c919                	c.beqz	a0,2005b734 <rtos_critical_exit+0x1c>
2005b720:	efc18793          	addi	a5,gp,-260 # 20065fd4 <uxCriticalNestingCnt>
2005b724:	0007d703          	lhu	a4,0(a5)
2005b728:	177d                	c.addi	a4,-1
2005b72a:	00e79023          	sh	a4,0(a5)
2005b72e:	40b2                	c.lwsp	ra,12(sp)
2005b730:	0141                	c.addi	sp,16
2005b732:	8082                	c.jr	ra
2005b734:	40b2                	c.lwsp	ra,12(sp)
2005b736:	0141                	c.addi	sp,16
2005b738:	23a0206f          	jal	zero,2005d972 <vTaskExitCritical>

2005b73c <rtos_mem_init>:
2005b73c:	846ff06f          	jal	zero,2005a782 <os_heap_init>

2005b740 <rtos_mem_malloc>:
2005b740:	9c4ff06f          	jal	zero,2005a904 <pvPortMalloc>

2005b744 <rtos_mem_zmalloc>:
2005b744:	1101                	c.addi	sp,-32
2005b746:	cc22                	c.swsp	s0,24(sp)
2005b748:	ce06                	c.swsp	ra,28(sp)
2005b74a:	c62a                	c.swsp	a0,12(sp)
2005b74c:	9b8ff0ef          	jal	ra,2005a904 <pvPortMalloc>
2005b750:	842a                	c.mv	s0,a0
2005b752:	c519                	c.beqz	a0,2005b760 <rtos_mem_zmalloc+0x1c>
2005b754:	4632                	c.lwsp	a2,12(sp)
2005b756:	4581                	c.li	a1,0
2005b758:	dffae097          	auipc	ra,0xdffae
2005b75c:	376080e7          	jalr	ra,886(ra) # 9ace <__wrap_memset>
2005b760:	40f2                	c.lwsp	ra,28(sp)
2005b762:	8522                	c.mv	a0,s0
2005b764:	4462                	c.lwsp	s0,24(sp)
2005b766:	6105                	c.addi16sp	sp,32
2005b768:	8082                	c.jr	ra

2005b76a <rtos_mem_free>:
2005b76a:	c119                	c.beqz	a0,2005b770 <rtos_mem_free+0x6>
2005b76c:	a76ff06f          	jal	zero,2005a9e2 <vPortFree>
2005b770:	8082                	c.jr	ra

2005b772 <rtos_mem_get_free_heap_size>:
2005b772:	aceff06f          	jal	zero,2005aa40 <xPortGetFreeHeapSize>

2005b776 <rtos_mutex_create>:
2005b776:	c10d                	c.beqz	a0,2005b798 <rtos_mutex_create+0x22>
2005b778:	1141                	c.addi	sp,-16
2005b77a:	c422                	c.swsp	s0,8(sp)
2005b77c:	842a                	c.mv	s0,a0
2005b77e:	4505                	c.li	a0,1
2005b780:	c606                	c.swsp	ra,12(sp)
2005b782:	3fc010ef          	jal	ra,2005cb7e <xQueueCreateMutex>
2005b786:	c008                	c.sw	a0,0(s0)
2005b788:	40b2                	c.lwsp	ra,12(sp)
2005b78a:	4422                	c.lwsp	s0,8(sp)
2005b78c:	00153513          	sltiu	a0,a0,1
2005b790:	40a00533          	sub	a0,zero,a0
2005b794:	0141                	c.addi	sp,16
2005b796:	8082                	c.jr	ra
2005b798:	557d                	c.li	a0,-1
2005b79a:	8082                	c.jr	ra

2005b79c <rtos_mutex_create_static>:
2005b79c:	1141                	c.addi	sp,-16
2005b79e:	c422                	c.swsp	s0,8(sp)
2005b7a0:	c606                	c.swsp	ra,12(sp)
2005b7a2:	842a                	c.mv	s0,a0
2005b7a4:	2c4d                	c.jal	2005ba56 <__reserved_get_mutex_from_poll>
2005b7a6:	e511                	c.bnez	a0,2005b7b2 <rtos_mutex_create_static+0x16>
2005b7a8:	8522                	c.mv	a0,s0
2005b7aa:	4422                	c.lwsp	s0,8(sp)
2005b7ac:	40b2                	c.lwsp	ra,12(sp)
2005b7ae:	0141                	c.addi	sp,16
2005b7b0:	b7d9                	c.j	2005b776 <rtos_mutex_create>
2005b7b2:	85aa                	c.mv	a1,a0
2005b7b4:	4505                	c.li	a0,1
2005b7b6:	454010ef          	jal	ra,2005cc0a <xQueueCreateMutexStatic>
2005b7ba:	c008                	c.sw	a0,0(s0)
2005b7bc:	40b2                	c.lwsp	ra,12(sp)
2005b7be:	4422                	c.lwsp	s0,8(sp)
2005b7c0:	00153513          	sltiu	a0,a0,1
2005b7c4:	40a00533          	sub	a0,zero,a0
2005b7c8:	0141                	c.addi	sp,16
2005b7ca:	8082                	c.jr	ra

2005b7cc <rtos_sema_create>:
2005b7cc:	1141                	c.addi	sp,-16
2005b7ce:	c422                	c.swsp	s0,8(sp)
2005b7d0:	c606                	c.swsp	ra,12(sp)
2005b7d2:	842a                	c.mv	s0,a0
2005b7d4:	557d                	c.li	a0,-1
2005b7d6:	c809                	c.beqz	s0,2005b7e8 <rtos_sema_create+0x1c>
2005b7d8:	8532                	c.mv	a0,a2
2005b7da:	3d8010ef          	jal	ra,2005cbb2 <xQueueCreateCountingSemaphore>
2005b7de:	c008                	c.sw	a0,0(s0)
2005b7e0:	00153513          	sltiu	a0,a0,1
2005b7e4:	40a00533          	sub	a0,zero,a0
2005b7e8:	40b2                	c.lwsp	ra,12(sp)
2005b7ea:	4422                	c.lwsp	s0,8(sp)
2005b7ec:	0141                	c.addi	sp,16
2005b7ee:	8082                	c.jr	ra

2005b7f0 <rtos_sema_create_static>:
2005b7f0:	1101                	c.addi	sp,-32
2005b7f2:	cc22                	c.swsp	s0,24(sp)
2005b7f4:	ca26                	c.swsp	s1,20(sp)
2005b7f6:	c62e                	c.swsp	a1,12(sp)
2005b7f8:	ce06                	c.swsp	ra,28(sp)
2005b7fa:	842a                	c.mv	s0,a0
2005b7fc:	84b2                	c.mv	s1,a2
2005b7fe:	2c8d                	c.jal	2005ba70 <__reserved_get_sema_from_poll>
2005b800:	45b2                	c.lwsp	a1,12(sp)
2005b802:	e901                	c.bnez	a0,2005b812 <rtos_sema_create_static+0x22>
2005b804:	8522                	c.mv	a0,s0
2005b806:	4462                	c.lwsp	s0,24(sp)
2005b808:	40f2                	c.lwsp	ra,28(sp)
2005b80a:	8626                	c.mv	a2,s1
2005b80c:	44d2                	c.lwsp	s1,20(sp)
2005b80e:	6105                	c.addi16sp	sp,32
2005b810:	bf75                	c.j	2005b7cc <rtos_sema_create>
2005b812:	862a                	c.mv	a2,a0
2005b814:	8526                	c.mv	a0,s1
2005b816:	42c010ef          	jal	ra,2005cc42 <xQueueCreateCountingSemaphoreStatic>
2005b81a:	c008                	c.sw	a0,0(s0)
2005b81c:	40f2                	c.lwsp	ra,28(sp)
2005b81e:	4462                	c.lwsp	s0,24(sp)
2005b820:	00153513          	sltiu	a0,a0,1
2005b824:	44d2                	c.lwsp	s1,20(sp)
2005b826:	40a00533          	sub	a0,zero,a0
2005b82a:	6105                	c.addi16sp	sp,32
2005b82c:	8082                	c.jr	ra

2005b82e <rtos_sema_create_binary>:
2005b82e:	c11d                	c.beqz	a0,2005b854 <rtos_sema_create_binary+0x26>
2005b830:	1141                	c.addi	sp,-16
2005b832:	c422                	c.swsp	s0,8(sp)
2005b834:	460d                	c.li	a2,3
2005b836:	842a                	c.mv	s0,a0
2005b838:	4581                	c.li	a1,0
2005b83a:	4505                	c.li	a0,1
2005b83c:	c606                	c.swsp	ra,12(sp)
2005b83e:	2e8010ef          	jal	ra,2005cb26 <xQueueGenericCreate>
2005b842:	c008                	c.sw	a0,0(s0)
2005b844:	40b2                	c.lwsp	ra,12(sp)
2005b846:	4422                	c.lwsp	s0,8(sp)
2005b848:	00153513          	sltiu	a0,a0,1
2005b84c:	40a00533          	sub	a0,zero,a0
2005b850:	0141                	c.addi	sp,16
2005b852:	8082                	c.jr	ra
2005b854:	557d                	c.li	a0,-1
2005b856:	8082                	c.jr	ra

2005b858 <rtos_sema_delete>:
2005b858:	c909                	c.beqz	a0,2005b86a <rtos_sema_delete+0x12>
2005b85a:	1141                	c.addi	sp,-16
2005b85c:	c606                	c.swsp	ra,12(sp)
2005b85e:	42e010ef          	jal	ra,2005cc8c <vQueueDelete>
2005b862:	40b2                	c.lwsp	ra,12(sp)
2005b864:	4501                	c.li	a0,0
2005b866:	0141                	c.addi	sp,16
2005b868:	8082                	c.jr	ra
2005b86a:	557d                	c.li	a0,-1
2005b86c:	8082                	c.jr	ra

2005b86e <rtos_sema_take>:
2005b86e:	7179                	c.addi16sp	sp,-48
2005b870:	d422                	c.swsp	s0,40(sp)
2005b872:	c62e                	c.swsp	a1,12(sp)
2005b874:	d606                	c.swsp	ra,44(sp)
2005b876:	842a                	c.mv	s0,a0
2005b878:	ce02                	c.swsp	zero,28(sp)
2005b87a:	e69ff0ef          	jal	ra,2005b6e2 <rtos_critical_is_in_interrupt>
2005b87e:	45b2                	c.lwsp	a1,12(sp)
2005b880:	c505                	c.beqz	a0,2005b8a8 <rtos_sema_take+0x3a>
2005b882:	0870                	c.addi4spn	a2,sp,28
2005b884:	4581                	c.li	a1,0
2005b886:	8522                	c.mv	a0,s0
2005b888:	194010ef          	jal	ra,2005ca1c <xQueueReceiveFromISR>
2005b88c:	872a                	c.mv	a4,a0
2005b88e:	4785                	c.li	a5,1
2005b890:	557d                	c.li	a0,-1
2005b892:	00f71763          	bne	a4,a5,2005b8a0 <rtos_sema_take+0x32>
2005b896:	4572                	c.lwsp	a0,28(sp)
2005b898:	c501                	c.beqz	a0,2005b8a0 <rtos_sema_take+0x32>
2005b89a:	4f3010ef          	jal	ra,2005d58c <vTaskSwitchContext>
2005b89e:	4501                	c.li	a0,0
2005b8a0:	50b2                	c.lwsp	ra,44(sp)
2005b8a2:	5422                	c.lwsp	s0,40(sp)
2005b8a4:	6145                	c.addi16sp	sp,48
2005b8a6:	8082                	c.jr	ra
2005b8a8:	8522                	c.mv	a0,s0
2005b8aa:	054010ef          	jal	ra,2005c8fe <xQueueSemaphoreTake>
2005b8ae:	157d                	c.addi	a0,-1
2005b8b0:	00a03533          	sltu	a0,zero,a0
2005b8b4:	40a00533          	sub	a0,zero,a0
2005b8b8:	b7e5                	c.j	2005b8a0 <rtos_sema_take+0x32>

2005b8ba <rtos_sema_give>:
2005b8ba:	7179                	c.addi16sp	sp,-48
2005b8bc:	d422                	c.swsp	s0,40(sp)
2005b8be:	d606                	c.swsp	ra,44(sp)
2005b8c0:	842a                	c.mv	s0,a0
2005b8c2:	ce02                	c.swsp	zero,28(sp)
2005b8c4:	e1fff0ef          	jal	ra,2005b6e2 <rtos_critical_is_in_interrupt>
2005b8c8:	c505                	c.beqz	a0,2005b8f0 <rtos_sema_give+0x36>
2005b8ca:	086c                	c.addi4spn	a1,sp,28
2005b8cc:	8522                	c.mv	a0,s0
2005b8ce:	6ed000ef          	jal	ra,2005c7ba <xQueueGiveFromISR>
2005b8d2:	47f2                	c.lwsp	a5,28(sp)
2005b8d4:	c789                	c.beqz	a5,2005b8de <rtos_sema_give+0x24>
2005b8d6:	c62a                	c.swsp	a0,12(sp)
2005b8d8:	4b5010ef          	jal	ra,2005d58c <vTaskSwitchContext>
2005b8dc:	4532                	c.lwsp	a0,12(sp)
2005b8de:	50b2                	c.lwsp	ra,44(sp)
2005b8e0:	5422                	c.lwsp	s0,40(sp)
2005b8e2:	157d                	c.addi	a0,-1
2005b8e4:	00a03533          	sltu	a0,zero,a0
2005b8e8:	40a00533          	sub	a0,zero,a0
2005b8ec:	6145                	c.addi16sp	sp,48
2005b8ee:	8082                	c.jr	ra
2005b8f0:	4681                	c.li	a3,0
2005b8f2:	4601                	c.li	a2,0
2005b8f4:	4581                	c.li	a1,0
2005b8f6:	8522                	c.mv	a0,s0
2005b8f8:	555000ef          	jal	ra,2005c64c <xQueueGenericSend>
2005b8fc:	b7cd                	c.j	2005b8de <rtos_sema_give+0x24>

2005b8fe <__reserved_get_from_poll>:
2005b8fe:	7179                	c.addi16sp	sp,-48
2005b900:	d226                	c.swsp	s1,36(sp)
2005b902:	d04a                	c.swsp	s2,32(sp)
2005b904:	ce4e                	c.swsp	s3,28(sp)
2005b906:	cc52                	c.swsp	s4,24(sp)
2005b908:	ca56                	c.swsp	s5,20(sp)
2005b90a:	c85a                	c.swsp	s6,16(sp)
2005b90c:	d606                	c.swsp	ra,44(sp)
2005b90e:	d422                	c.swsp	s0,40(sp)
2005b910:	c65e                	c.swsp	s7,12(sp)
2005b912:	8a3e                	c.mv	s4,a5
2005b914:	421c                	c.lw	a5,0(a2)
2005b916:	84aa                	c.mv	s1,a0
2005b918:	8b2e                	c.mv	s6,a1
2005b91a:	8ab2                	c.mv	s5,a2
2005b91c:	8936                	c.mv	s2,a3
2005b91e:	89ba                	c.mv	s3,a4
2005b920:	ef85                	c.bnez	a5,2005b958 <__reserved_get_from_poll+0x5a>
2005b922:	4405                	c.li	s0,1
2005b924:	08851263          	bne	a0,s0,2005b9a8 <__reserved_get_from_poll+0xaa>
2005b928:	f0c18413          	addi	s0,gp,-244 # 20065fe4 <mutex_pool_init_flag>
2005b92c:	401c                	c.lw	a5,0(s0)
2005b92e:	02a78563          	beq	a5,a0,2005b958 <__reserved_get_from_poll+0x5a>
2005b932:	4601                	c.li	a2,0
2005b934:	4581                	c.li	a1,0
2005b936:	62418513          	addi	a0,gp,1572 # 200666fc <mutex_pool>
2005b93a:	dffae097          	auipc	ra,0xdffae
2005b93e:	194080e7          	jalr	ra,404(ra) # 9ace <__wrap_memset>
2005b942:	f001a023          	sw	zero,-256(gp) # 20065fd8 <mutex_buf_used_num>
2005b946:	f3018513          	addi	a0,gp,-208 # 20066008 <wrapper_mutex_buf_list>
2005b94a:	f001a423          	sw	zero,-248(gp) # 20065fe0 <mutex_max_buf_used_num>
2005b94e:	c108                	c.sw	a0,0(a0)
2005b950:	c148                	c.sw	a0,4(a0)
2005b952:	f001a223          	sw	zero,-252(gp) # 20065fdc <mutex_dynamic_num>
2005b956:	c004                	c.sw	s1,0(s0)
2005b958:	d9dff0ef          	jal	ra,2005b6f4 <rtos_critical_enter>
2005b95c:	000b2403          	lw	s0,0(s6)
2005b960:	0a8b1b63          	bne	s6,s0,2005ba16 <__reserved_get_from_poll+0x118>
2005b964:	db5ff0ef          	jal	ra,2005b718 <rtos_critical_exit>
2005b968:	4b01                	c.li	s6,0
2005b96a:	000aa783          	lw	a5,0(s5)
2005b96e:	c791                	c.beqz	a5,2005b97a <__reserved_get_from_poll+0x7c>
2005b970:	00092783          	lw	a5,0(s2)
2005b974:	0785                	c.addi	a5,1
2005b976:	00f92023          	sw	a5,0(s2)
2005b97a:	00092703          	lw	a4,0(s2)
2005b97e:	0009a783          	lw	a5,0(s3)
2005b982:	97ba                	c.add	a5,a4
2005b984:	000a2703          	lw	a4,0(s4)
2005b988:	00f77463          	bgeu	a4,a5,2005b990 <__reserved_get_from_poll+0x92>
2005b98c:	00fa2023          	sw	a5,0(s4)
2005b990:	50b2                	c.lwsp	ra,44(sp)
2005b992:	5422                	c.lwsp	s0,40(sp)
2005b994:	5492                	c.lwsp	s1,36(sp)
2005b996:	5902                	c.lwsp	s2,32(sp)
2005b998:	49f2                	c.lwsp	s3,28(sp)
2005b99a:	4a62                	c.lwsp	s4,24(sp)
2005b99c:	4ad2                	c.lwsp	s5,20(sp)
2005b99e:	4bb2                	c.lwsp	s7,12(sp)
2005b9a0:	855a                	c.mv	a0,s6
2005b9a2:	4b42                	c.lwsp	s6,16(sp)
2005b9a4:	6145                	c.addi16sp	sp,48
2005b9a6:	8082                	c.jr	ra
2005b9a8:	4789                	c.li	a5,2
2005b9aa:	02f51d63          	bne	a0,a5,2005b9e4 <__reserved_get_from_poll+0xe6>
2005b9ae:	f1c18b93          	addi	s7,gp,-228 # 20065ff4 <sema_pool_init_flag>
2005b9b2:	000ba783          	lw	a5,0(s7)
2005b9b6:	fa8781e3          	beq	a5,s0,2005b958 <__reserved_get_from_poll+0x5a>
2005b9ba:	4601                	c.li	a2,0
2005b9bc:	4581                	c.li	a1,0
2005b9be:	62418513          	addi	a0,gp,1572 # 200666fc <mutex_pool>
2005b9c2:	dffae097          	auipc	ra,0xdffae
2005b9c6:	10c080e7          	jalr	ra,268(ra) # 9ace <__wrap_memset>
2005b9ca:	f001a823          	sw	zero,-240(gp) # 20065fe8 <sema_buf_used_num>
2005b9ce:	f3818513          	addi	a0,gp,-200 # 20066010 <wrapper_sema_buf_list>
2005b9d2:	f001ac23          	sw	zero,-232(gp) # 20065ff0 <sema_max_buf_used_num>
2005b9d6:	c108                	c.sw	a0,0(a0)
2005b9d8:	c148                	c.sw	a0,4(a0)
2005b9da:	f001aa23          	sw	zero,-236(gp) # 20065fec <sema_dynamic_num>
2005b9de:	008ba023          	sw	s0,0(s7)
2005b9e2:	bf9d                	c.j	2005b958 <__reserved_get_from_poll+0x5a>
2005b9e4:	f2c18b93          	addi	s7,gp,-212 # 20066004 <timer_pool_init_flag>
2005b9e8:	000ba783          	lw	a5,0(s7)
2005b9ec:	f68786e3          	beq	a5,s0,2005b958 <__reserved_get_from_poll+0x5a>
2005b9f0:	4601                	c.li	a2,0
2005b9f2:	4581                	c.li	a1,0
2005b9f4:	62418513          	addi	a0,gp,1572 # 200666fc <mutex_pool>
2005b9f8:	dffae097          	auipc	ra,0xdffae
2005b9fc:	0d6080e7          	jalr	ra,214(ra) # 9ace <__wrap_memset>
2005ba00:	f201a023          	sw	zero,-224(gp) # 20065ff8 <timer_buf_used_num>
2005ba04:	f4018513          	addi	a0,gp,-192 # 20066018 <wrapper_timer_buf_list>
2005ba08:	f201a423          	sw	zero,-216(gp) # 20066000 <timer_max_buf_used_num>
2005ba0c:	c108                	c.sw	a0,0(a0)
2005ba0e:	c148                	c.sw	a0,4(a0)
2005ba10:	f201a223          	sw	zero,-220(gp) # 20065ffc <timer_dynamic_num>
2005ba14:	b7e9                	c.j	2005b9de <__reserved_get_from_poll+0xe0>
2005ba16:	405c                	c.lw	a5,4(s0)
2005ba18:	4018                	c.lw	a4,0(s0)
2005ba1a:	00840b13          	addi	s6,s0,8
2005ba1e:	c35c                	c.sw	a5,4(a4)
2005ba20:	c398                	c.sw	a4,0(a5)
2005ba22:	c000                	c.sw	s0,0(s0)
2005ba24:	c040                	c.sw	s0,4(s0)
2005ba26:	0009a783          	lw	a5,0(s3)
2005ba2a:	0785                	c.addi	a5,1
2005ba2c:	00f9a023          	sw	a5,0(s3)
2005ba30:	ce9ff0ef          	jal	ra,2005b718 <rtos_critical_exit>
2005ba34:	57e1                	c.li	a5,-8
2005ba36:	f2f40ae3          	beq	s0,a5,2005b96a <__reserved_get_from_poll+0x6c>
2005ba3a:	478d                	c.li	a5,3
2005ba3c:	05000613          	addi	a2,zero,80
2005ba40:	00f49463          	bne	s1,a5,2005ba48 <__reserved_get_from_poll+0x14a>
2005ba44:	02c00613          	addi	a2,zero,44
2005ba48:	4581                	c.li	a1,0
2005ba4a:	855a                	c.mv	a0,s6
2005ba4c:	dffae097          	auipc	ra,0xdffae
2005ba50:	082080e7          	jalr	ra,130(ra) # 9ace <__wrap_memset>
2005ba54:	b71d                	c.j	2005b97a <__reserved_get_from_poll+0x7c>

2005ba56 <__reserved_get_mutex_from_poll>:
2005ba56:	f0818793          	addi	a5,gp,-248 # 20065fe0 <mutex_max_buf_used_num>
2005ba5a:	f0018713          	addi	a4,gp,-256 # 20065fd8 <mutex_buf_used_num>
2005ba5e:	f0418693          	addi	a3,gp,-252 # 20065fdc <mutex_dynamic_num>
2005ba62:	f0c18613          	addi	a2,gp,-244 # 20065fe4 <mutex_pool_init_flag>
2005ba66:	f3018593          	addi	a1,gp,-208 # 20066008 <wrapper_mutex_buf_list>
2005ba6a:	4505                	c.li	a0,1
2005ba6c:	e93ff06f          	jal	zero,2005b8fe <__reserved_get_from_poll>

2005ba70 <__reserved_get_sema_from_poll>:
2005ba70:	f1818793          	addi	a5,gp,-232 # 20065ff0 <sema_max_buf_used_num>
2005ba74:	f1018713          	addi	a4,gp,-240 # 20065fe8 <sema_buf_used_num>
2005ba78:	f1418693          	addi	a3,gp,-236 # 20065fec <sema_dynamic_num>
2005ba7c:	f1c18613          	addi	a2,gp,-228 # 20065ff4 <sema_pool_init_flag>
2005ba80:	f3818593          	addi	a1,gp,-200 # 20066010 <wrapper_sema_buf_list>
2005ba84:	4509                	c.li	a0,2
2005ba86:	e79ff06f          	jal	zero,2005b8fe <__reserved_get_from_poll>

2005ba8a <rtos_sched_start>:
2005ba8a:	1141                	c.addi	sp,-16
2005ba8c:	c606                	c.swsp	ra,12(sp)
2005ba8e:	042020ef          	jal	ra,2005dad0 <vTaskStartScheduler>
2005ba92:	40b2                	c.lwsp	ra,12(sp)
2005ba94:	4501                	c.li	a0,0
2005ba96:	0141                	c.addi	sp,16
2005ba98:	8082                	c.jr	ra

2005ba9a <rtos_sched_get_state>:
2005ba9a:	1141                	c.addi	sp,-16
2005ba9c:	c606                	c.swsp	ra,12(sp)
2005ba9e:	4d9010ef          	jal	ra,2005d776 <xTaskGetSchedulerState>
2005baa2:	4785                	c.li	a5,1
2005baa4:	00f50b63          	beq	a0,a5,2005baba <rtos_sched_get_state+0x20>
2005baa8:	4cf010ef          	jal	ra,2005d776 <xTaskGetSchedulerState>
2005baac:	c519                	c.beqz	a0,2005baba <rtos_sched_get_state+0x20>
2005baae:	4c9010ef          	jal	ra,2005d776 <xTaskGetSchedulerState>
2005bab2:	4789                	c.li	a5,2
2005bab4:	00f50363          	beq	a0,a5,2005baba <rtos_sched_get_state+0x20>
2005bab8:	557d                	c.li	a0,-1
2005baba:	40b2                	c.lwsp	ra,12(sp)
2005babc:	0141                	c.addi	sp,16
2005babe:	8082                	c.jr	ra

2005bac0 <rtos_task_create>:
2005bac0:	882a                	c.mv	a6,a0
2005bac2:	8532                	c.mv	a0,a2
2005bac4:	863a                	c.mv	a2,a4
2005bac6:	1141                	c.addi	sp,-16
2005bac8:	873e                	c.mv	a4,a5
2005baca:	8209                	c.srli	a2,0x2
2005bacc:	87c2                	c.mv	a5,a6
2005bace:	c606                	c.swsp	ra,12(sp)
2005bad0:	068020ef          	jal	ra,2005db38 <xTaskCreate>
2005bad4:	40b2                	c.lwsp	ra,12(sp)
2005bad6:	157d                	c.addi	a0,-1
2005bad8:	00a03533          	sltu	a0,zero,a0
2005badc:	40a00533          	sub	a0,zero,a0
2005bae0:	0141                	c.addi	sp,16
2005bae2:	8082                	c.jr	ra

2005bae4 <rtos_task_delete>:
2005bae4:	1141                	c.addi	sp,-16
2005bae6:	c606                	c.swsp	ra,12(sp)
2005bae8:	0c8020ef          	jal	ra,2005dbb0 <vTaskDelete>
2005baec:	40b2                	c.lwsp	ra,12(sp)
2005baee:	4501                	c.li	a0,0
2005baf0:	0141                	c.addi	sp,16
2005baf2:	8082                	c.jr	ra

2005baf4 <rtos_create_secure_context>:
2005baf4:	8082                	c.jr	ra

2005baf6 <rtos_time_delay_ms>:
2005baf6:	1141                	c.addi	sp,-16
2005baf8:	c422                	c.swsp	s0,8(sp)
2005bafa:	c606                	c.swsp	ra,12(sp)
2005bafc:	842a                	c.mv	s0,a0
2005bafe:	c8dfd0ef          	jal	ra,2005978a <pmu_yield_os_check>
2005bb02:	cd19                	c.beqz	a0,2005bb20 <rtos_time_delay_ms+0x2a>
2005bb04:	473010ef          	jal	ra,2005d776 <xTaskGetSchedulerState>
2005bb08:	4789                	c.li	a5,2
2005bb0a:	00f51b63          	bne	a0,a5,2005bb20 <rtos_time_delay_ms+0x2a>
2005bb0e:	bd5ff0ef          	jal	ra,2005b6e2 <rtos_critical_is_in_interrupt>
2005bb12:	e519                	c.bnez	a0,2005bb20 <rtos_time_delay_ms+0x2a>
2005bb14:	8522                	c.mv	a0,s0
2005bb16:	4422                	c.lwsp	s0,8(sp)
2005bb18:	40b2                	c.lwsp	ra,12(sp)
2005bb1a:	0141                	c.addi	sp,16
2005bb1c:	2ae0206f          	jal	zero,2005ddca <vTaskDelay>
2005bb20:	8522                	c.mv	a0,s0
2005bb22:	4422                	c.lwsp	s0,8(sp)
2005bb24:	40b2                	c.lwsp	ra,12(sp)
2005bb26:	0141                	c.addi	sp,16
2005bb28:	dffa6317          	auipc	t1,0xdffa6
2005bb2c:	7f230067          	jalr	zero,2034(t1) # 231a <DelayMs>

2005bb30 <rtos_time_get_current_system_time_ms>:
2005bb30:	1141                	c.addi	sp,-16
2005bb32:	c606                	c.swsp	ra,12(sp)
2005bb34:	bafff0ef          	jal	ra,2005b6e2 <rtos_critical_is_in_interrupt>
2005bb38:	c509                	c.beqz	a0,2005bb42 <rtos_time_get_current_system_time_ms+0x12>
2005bb3a:	40b2                	c.lwsp	ra,12(sp)
2005bb3c:	0141                	c.addi	sp,16
2005bb3e:	1010106f          	jal	zero,2005d43e <xTaskGetTickCountFromISR>
2005bb42:	c49fd0ef          	jal	ra,2005978a <pmu_yield_os_check>
2005bb46:	d975                	c.beqz	a0,2005bb3a <rtos_time_get_current_system_time_ms+0xa>
2005bb48:	40b2                	c.lwsp	ra,12(sp)
2005bb4a:	0141                	c.addi	sp,16
2005bb4c:	5240206f          	jal	zero,2005e070 <xTaskGetTickCount>

2005bb50 <rtos_timer_create>:
2005bb50:	1141                	c.addi	sp,-16
2005bb52:	c422                	c.swsp	s0,8(sp)
2005bb54:	c606                	c.swsp	ra,12(sp)
2005bb56:	842a                	c.mv	s0,a0
2005bb58:	587d                	c.li	a6,-1
2005bb5a:	c105                	c.beqz	a0,2005bb7a <rtos_timer_create+0x2a>
2005bb5c:	88b2                	c.mv	a7,a2
2005bb5e:	587d                	c.li	a6,-1
2005bb60:	863a                	c.mv	a2,a4
2005bb62:	873e                	c.mv	a4,a5
2005bb64:	cb99                	c.beqz	a5,2005bb7a <rtos_timer_create+0x2a>
2005bb66:	852e                	c.mv	a0,a1
2005bb68:	85b6                	c.mv	a1,a3
2005bb6a:	86c6                	c.mv	a3,a7
2005bb6c:	74a020ef          	jal	ra,2005e2b6 <xTimerCreate>
2005bb70:	c008                	c.sw	a0,0(s0)
2005bb72:	00153513          	sltiu	a0,a0,1
2005bb76:	40a00833          	sub	a6,zero,a0
2005bb7a:	40b2                	c.lwsp	ra,12(sp)
2005bb7c:	4422                	c.lwsp	s0,8(sp)
2005bb7e:	8542                	c.mv	a0,a6
2005bb80:	0141                	c.addi	sp,16
2005bb82:	8082                	c.jr	ra

2005bb84 <rtos_timer_start>:
2005bb84:	e909                	c.bnez	a0,2005bb96 <rtos_timer_start+0x12>
2005bb86:	557d                	c.li	a0,-1
2005bb88:	8082                	c.jr	ra
2005bb8a:	557d                	c.li	a0,-1
2005bb8c:	40f2                	c.lwsp	ra,28(sp)
2005bb8e:	4462                	c.lwsp	s0,24(sp)
2005bb90:	44d2                	c.lwsp	s1,20(sp)
2005bb92:	6105                	c.addi16sp	sp,32
2005bb94:	8082                	c.jr	ra
2005bb96:	1101                	c.addi	sp,-32
2005bb98:	cc22                	c.swsp	s0,24(sp)
2005bb9a:	ca26                	c.swsp	s1,20(sp)
2005bb9c:	ce06                	c.swsp	ra,28(sp)
2005bb9e:	842a                	c.mv	s0,a0
2005bba0:	84ae                	c.mv	s1,a1
2005bba2:	b41ff0ef          	jal	ra,2005b6e2 <rtos_critical_is_in_interrupt>
2005bba6:	c505                	c.beqz	a0,2005bbce <rtos_timer_start+0x4a>
2005bba8:	c602                	c.swsp	zero,12(sp)
2005bbaa:	095010ef          	jal	ra,2005d43e <xTaskGetTickCountFromISR>
2005bbae:	862a                	c.mv	a2,a0
2005bbb0:	4701                	c.li	a4,0
2005bbb2:	0074                	c.addi4spn	a3,sp,12
2005bbb4:	4599                	c.li	a1,6
2005bbb6:	8522                	c.mv	a0,s0
2005bbb8:	766020ef          	jal	ra,2005e31e <xTimerGenericCommand>
2005bbbc:	4785                	c.li	a5,1
2005bbbe:	fcf516e3          	bne	a0,a5,2005bb8a <rtos_timer_start+0x6>
2005bbc2:	4532                	c.lwsp	a0,12(sp)
2005bbc4:	d561                	c.beqz	a0,2005bb8c <rtos_timer_start+0x8>
2005bbc6:	1c7010ef          	jal	ra,2005d58c <vTaskSwitchContext>
2005bbca:	4501                	c.li	a0,0
2005bbcc:	b7c1                	c.j	2005bb8c <rtos_timer_start+0x8>
2005bbce:	4a2020ef          	jal	ra,2005e070 <xTaskGetTickCount>
2005bbd2:	862a                	c.mv	a2,a0
2005bbd4:	8726                	c.mv	a4,s1
2005bbd6:	4681                	c.li	a3,0
2005bbd8:	4585                	c.li	a1,1
2005bbda:	8522                	c.mv	a0,s0
2005bbdc:	742020ef          	jal	ra,2005e31e <xTimerGenericCommand>
2005bbe0:	157d                	c.addi	a0,-1
2005bbe2:	00a03533          	sltu	a0,zero,a0
2005bbe6:	40a00533          	sub	a0,zero,a0
2005bbea:	b74d                	c.j	2005bb8c <rtos_timer_start+0x8>

2005bbec <platform_calloc_uninit>:
2005bbec:	4501                	c.li	a0,0
2005bbee:	8082                	c.jr	ra

2005bbf0 <platform_free_uninit>:
2005bbf0:	8082                	c.jr	ra

2005bbf2 <mbedtls_platform_set_calloc_free>:
2005bbf2:	e8a1a823          	sw	a0,-368(gp) # 20065f68 <mbedtls_calloc_func>
2005bbf6:	e8b1aa23          	sw	a1,-364(gp) # 20065f6c <mbedtls_free_func>
2005bbfa:	200017b7          	lui	a5,0x20001
2005bbfe:	c6c78793          	addi	a5,a5,-916 # 20000c6c <rom_ssl_ram_map>
2005bc02:	20001737          	lui	a4,0x20001
2005bc06:	c388                	c.sw	a0,0(a5)
2005bc08:	c6f72423          	sw	a5,-920(a4) # 20000c68 <p_rom_ssl_ram_map>
2005bc0c:	c3cc                	c.sw	a1,4(a5)
2005bc0e:	4501                	c.li	a0,0
2005bc10:	8082                	c.jr	ra
2005bc12:	0000                	c.unimp
2005bc14:	0000                	c.unimp
	...

2005bc18 <freertos_risc_v_trap_handler>:
2005bc18:	ef810113          	addi	sp,sp,-264
2005bc1c:	c206                	c.swsp	ra,4(sp)
2005bc1e:	ca16                	c.swsp	t0,20(sp)
2005bc20:	cc1a                	c.swsp	t1,24(sp)
2005bc22:	ce1e                	c.swsp	t2,28(sp)
2005bc24:	d022                	c.swsp	s0,32(sp)
2005bc26:	d226                	c.swsp	s1,36(sp)
2005bc28:	d42a                	c.swsp	a0,40(sp)
2005bc2a:	d62e                	c.swsp	a1,44(sp)
2005bc2c:	d832                	c.swsp	a2,48(sp)
2005bc2e:	da36                	c.swsp	a3,52(sp)
2005bc30:	dc3a                	c.swsp	a4,56(sp)
2005bc32:	de3e                	c.swsp	a5,60(sp)
2005bc34:	c0c2                	c.swsp	a6,64(sp)
2005bc36:	c2c6                	c.swsp	a7,68(sp)
2005bc38:	c4ca                	c.swsp	s2,72(sp)
2005bc3a:	c6ce                	c.swsp	s3,76(sp)
2005bc3c:	c8d2                	c.swsp	s4,80(sp)
2005bc3e:	cad6                	c.swsp	s5,84(sp)
2005bc40:	ccda                	c.swsp	s6,88(sp)
2005bc42:	cede                	c.swsp	s7,92(sp)
2005bc44:	d0e2                	c.swsp	s8,96(sp)
2005bc46:	d2e6                	c.swsp	s9,100(sp)
2005bc48:	d4ea                	c.swsp	s10,104(sp)
2005bc4a:	d6ee                	c.swsp	s11,108(sp)
2005bc4c:	d8f2                	c.swsp	t3,112(sp)
2005bc4e:	daf6                	c.swsp	t4,116(sp)
2005bc50:	dcfa                	c.swsp	t5,120(sp)
2005bc52:	defe                	c.swsp	t6,124(sp)
2005bc54:	300022f3          	csrrs	t0,mstatus,zero
2005bc58:	6319                	c.lui	t1,0x6
2005bc5a:	0062f2b3          	and	t0,t0,t1
2005bc5e:	6309                	c.lui	t1,0x2
2005bc60:	04628663          	beq	t0,t1,2005bcac <freertos_risc_v_trap_handler+0x94>
2005bc64:	003022f3          	csrrs	t0,fcsr,zero
2005bc68:	e102                	c.fswsp	ft0,128(sp)
2005bc6a:	e306                	c.fswsp	ft1,132(sp)
2005bc6c:	e50a                	c.fswsp	ft2,136(sp)
2005bc6e:	e70e                	c.fswsp	ft3,140(sp)
2005bc70:	e912                	c.fswsp	ft4,144(sp)
2005bc72:	eb16                	c.fswsp	ft5,148(sp)
2005bc74:	ed1a                	c.fswsp	ft6,152(sp)
2005bc76:	ef1e                	c.fswsp	ft7,156(sp)
2005bc78:	f122                	c.fswsp	fs0,160(sp)
2005bc7a:	f326                	c.fswsp	fs1,164(sp)
2005bc7c:	f52a                	c.fswsp	fa0,168(sp)
2005bc7e:	f72e                	c.fswsp	fa1,172(sp)
2005bc80:	f932                	c.fswsp	fa2,176(sp)
2005bc82:	fb36                	c.fswsp	fa3,180(sp)
2005bc84:	fd3a                	c.fswsp	fa4,184(sp)
2005bc86:	ff3e                	c.fswsp	fa5,188(sp)
2005bc88:	e1c2                	c.fswsp	fa6,192(sp)
2005bc8a:	e3c6                	c.fswsp	fa7,196(sp)
2005bc8c:	e5ca                	c.fswsp	fs2,200(sp)
2005bc8e:	e7ce                	c.fswsp	fs3,204(sp)
2005bc90:	e9d2                	c.fswsp	fs4,208(sp)
2005bc92:	ebd6                	c.fswsp	fs5,212(sp)
2005bc94:	edda                	c.fswsp	fs6,216(sp)
2005bc96:	efde                	c.fswsp	fs7,220(sp)
2005bc98:	f1e2                	c.fswsp	fs8,224(sp)
2005bc9a:	f3e6                	c.fswsp	fs9,228(sp)
2005bc9c:	f5ea                	c.fswsp	fs10,232(sp)
2005bc9e:	f7ee                	c.fswsp	fs11,236(sp)
2005bca0:	f9f2                	c.fswsp	ft8,240(sp)
2005bca2:	fbf6                	c.fswsp	ft9,244(sp)
2005bca4:	fdfa                	c.fswsp	ft10,248(sp)
2005bca6:	fffe                	c.fswsp	ft11,252(sp)
2005bca8:	10512023          	sw	t0,256(sp)
2005bcac:	300022f3          	csrrs	t0,mstatus,zero
2005bcb0:	10512223          	sw	t0,260(sp)
2005bcb4:	34202573          	csrrs	a0,mcause,zero
2005bcb8:	341025f3          	csrrs	a1,mepc,zero

2005bcbc <test_if_asynchronous>:
2005bcbc:	01f55613          	srli	a2,a0,0x1f
2005bcc0:	e225                	c.bnez	a2,2005bd20 <handle_asynchronous>

2005bcc2 <handle_synchronous>:
2005bcc2:	0591                	c.addi	a1,4
2005bcc4:	c02e                	c.swsp	a1,0(sp)

2005bcc6 <test_if_environment_call>:
2005bcc6:	42ad                	c.li	t0,11
2005bcc8:	00a2cd63          	blt	t0,a0,2005bce2 <_crash_dump>
2005bccc:	42a1                	c.li	t0,8
2005bcce:	00554a63          	blt	a0,t0,2005bce2 <_crash_dump>
2005bcd2:	a009                	c.j	2005bcd4 <ecall_yield>

2005bcd4 <ecall_yield>:
2005bcd4:	f641a283          	lw	t0,-156(gp) # 2006603c <pxCurrentTCB>
2005bcd8:	0022a023          	sw	sp,0(t0)
2005bcdc:	e6c1a103          	lw	sp,-404(gp) # 20065f44 <xISRStackTop>
2005bce0:	a0a5                	c.j	2005bd48 <task_context_switch>

2005bce2 <_crash_dump>:
2005bce2:	34102573          	csrrs	a0,mepc,zero
2005bce6:	022c                	c.addi4spn	a1,sp,264
2005bce8:	860a                	c.mv	a2,sp
2005bcea:	c002                	c.swsp	zero,0(sp)
2005bcec:	c42e                	c.swsp	a1,8(sp)
2005bcee:	c60e                	c.swsp	gp,12(sp)
2005bcf0:	c812                	c.swsp	tp,16(sp)
2005bcf2:	e6c1a303          	lw	t1,-404(gp) # 20065f44 <xISRStackTop>
2005bcf6:	0000b397          	auipc	t2,0xb
2005bcfa:	a0a38393          	addi	t2,t2,-1526 # 20066700 <xISRStack>
2005bcfe:	08038393          	addi	t2,t2,128
2005bd02:	00615563          	bge	sp,t1,2005bd0c <not_isrstack>
2005bd06:	00714363          	blt	sp,t2,2005bd0c <not_isrstack>
2005bd0a:	a019                	c.j	2005bd10 <is_isrstack>

2005bd0c <not_isrstack>:
2005bd0c:	e6c1a103          	lw	sp,-404(gp) # 20065f44 <xISRStackTop>

2005bd10 <is_isrstack>:
2005bd10:	00004297          	auipc	t0,0x4
2005bd14:	fa828293          	addi	t0,t0,-88 # 2005fcb8 <crash_dump>
2005bd18:	9282                	c.jalr	t0

2005bd1a <crash_dump_end>:
2005bd1a:	a001                	c.j	2005bd1a <crash_dump_end>

2005bd1c <unrecoverable_error>:
2005bd1c:	9002                	c.ebreak
2005bd1e:	bffd                	c.j	2005bd1c <unrecoverable_error>

2005bd20 <handle_asynchronous>:
2005bd20:	c02e                	c.swsp	a1,0(sp)
2005bd22:	f641a283          	lw	t0,-156(gp) # 2006603c <pxCurrentTCB>
2005bd26:	0022a023          	sw	sp,0(t0)
2005bd2a:	e6c1a103          	lw	sp,-404(gp) # 20065f44 <xISRStackTop>

2005bd2e <nonvec_handle>:
2005bd2e:	4285                	c.li	t0,1
2005bd30:	02fe                	c.slli	t0,0x1f
2005bd32:	00728313          	addi	t1,t0,7
2005bd36:	00651763          	bne	a0,t1,2005bd44 <test_if_external_interrupt>
2005bd3a:	28d9                	c.jal	2005be10 <machine_timer_update>
2005bd3c:	71a010ef          	jal	ra,2005d456 <xTaskIncrementTick>
2005bd40:	c909                	c.beqz	a0,2005bd52 <processed_source>
2005bd42:	a019                	c.j	2005bd48 <task_context_switch>

2005bd44 <test_if_external_interrupt>:
2005bd44:	2211                	c.jal	2005be48 <vPortHandleInterrupt>
2005bd46:	a031                	c.j	2005bd52 <processed_source>

2005bd48 <task_context_switch>:
2005bd48:	045010ef          	jal	ra,2005d58c <vTaskSwitchContext>
2005bd4c:	a019                	c.j	2005bd52 <processed_source>

2005bd4e <as_yet_unhandled>:
2005bd4e:	9002                	c.ebreak
2005bd50:	bffd                	c.j	2005bd4e <as_yet_unhandled>

2005bd52 <processed_source>:
2005bd52:	f641a303          	lw	t1,-156(gp) # 2006603c <pxCurrentTCB>
2005bd56:	00032103          	lw	sp,0(t1) # 2000 <CRYPTO_Init+0x86a>

2005bd5a <restore_regs>:
2005bd5a:	4282                	c.lwsp	t0,0(sp)
2005bd5c:	34129073          	csrrw	zero,mepc,t0
2005bd60:	10412283          	lw	t0,260(sp)
2005bd64:	30029073          	csrrw	zero,mstatus,t0

2005bd68 <_stack_pop>:
2005bd68:	300022f3          	csrrs	t0,mstatus,zero
2005bd6c:	6319                	c.lui	t1,0x6
2005bd6e:	0062f2b3          	and	t0,t0,t1
2005bd72:	6309                	c.lui	t1,0x2
2005bd74:	04628c63          	beq	t0,t1,2005bdcc <_stack_pop+0x64>
2005bd78:	600a                	c.flwsp	ft0,128(sp)
2005bd7a:	609a                	c.flwsp	ft1,132(sp)
2005bd7c:	612a                	c.flwsp	ft2,136(sp)
2005bd7e:	61ba                	c.flwsp	ft3,140(sp)
2005bd80:	624a                	c.flwsp	ft4,144(sp)
2005bd82:	62da                	c.flwsp	ft5,148(sp)
2005bd84:	636a                	c.flwsp	ft6,152(sp)
2005bd86:	63fa                	c.flwsp	ft7,156(sp)
2005bd88:	740a                	c.flwsp	fs0,160(sp)
2005bd8a:	749a                	c.flwsp	fs1,164(sp)
2005bd8c:	752a                	c.flwsp	fa0,168(sp)
2005bd8e:	75ba                	c.flwsp	fa1,172(sp)
2005bd90:	764a                	c.flwsp	fa2,176(sp)
2005bd92:	76da                	c.flwsp	fa3,180(sp)
2005bd94:	776a                	c.flwsp	fa4,184(sp)
2005bd96:	77fa                	c.flwsp	fa5,188(sp)
2005bd98:	680e                	c.flwsp	fa6,192(sp)
2005bd9a:	689e                	c.flwsp	fa7,196(sp)
2005bd9c:	692e                	c.flwsp	fs2,200(sp)
2005bd9e:	69be                	c.flwsp	fs3,204(sp)
2005bda0:	6a4e                	c.flwsp	fs4,208(sp)
2005bda2:	6ade                	c.flwsp	fs5,212(sp)
2005bda4:	6b6e                	c.flwsp	fs6,216(sp)
2005bda6:	6bfe                	c.flwsp	fs7,220(sp)
2005bda8:	7c0e                	c.flwsp	fs8,224(sp)
2005bdaa:	7c9e                	c.flwsp	fs9,228(sp)
2005bdac:	7d2e                	c.flwsp	fs10,232(sp)
2005bdae:	7dbe                	c.flwsp	fs11,236(sp)
2005bdb0:	7e4e                	c.flwsp	ft8,240(sp)
2005bdb2:	7ede                	c.flwsp	ft9,244(sp)
2005bdb4:	7f6e                	c.flwsp	ft10,248(sp)
2005bdb6:	7ffe                	c.flwsp	ft11,252(sp)
2005bdb8:	10012283          	lw	t0,256(sp)
2005bdbc:	00329073          	csrrw	zero,fcsr,t0
2005bdc0:	6319                	c.lui	t1,0x6
2005bdc2:	30033073          	csrrc	zero,mstatus,t1
2005bdc6:	6311                	c.lui	t1,0x4
2005bdc8:	30032073          	csrrs	zero,mstatus,t1
2005bdcc:	4092                	c.lwsp	ra,4(sp)
2005bdce:	42d2                	c.lwsp	t0,20(sp)
2005bdd0:	4362                	c.lwsp	t1,24(sp)
2005bdd2:	43f2                	c.lwsp	t2,28(sp)
2005bdd4:	5402                	c.lwsp	s0,32(sp)
2005bdd6:	5492                	c.lwsp	s1,36(sp)
2005bdd8:	5522                	c.lwsp	a0,40(sp)
2005bdda:	55b2                	c.lwsp	a1,44(sp)
2005bddc:	5642                	c.lwsp	a2,48(sp)
2005bdde:	56d2                	c.lwsp	a3,52(sp)
2005bde0:	5762                	c.lwsp	a4,56(sp)
2005bde2:	57f2                	c.lwsp	a5,60(sp)
2005bde4:	4806                	c.lwsp	a6,64(sp)
2005bde6:	4896                	c.lwsp	a7,68(sp)
2005bde8:	4926                	c.lwsp	s2,72(sp)
2005bdea:	49b6                	c.lwsp	s3,76(sp)
2005bdec:	4a46                	c.lwsp	s4,80(sp)
2005bdee:	4ad6                	c.lwsp	s5,84(sp)
2005bdf0:	4b66                	c.lwsp	s6,88(sp)
2005bdf2:	4bf6                	c.lwsp	s7,92(sp)
2005bdf4:	5c06                	c.lwsp	s8,96(sp)
2005bdf6:	5c96                	c.lwsp	s9,100(sp)
2005bdf8:	5d26                	c.lwsp	s10,104(sp)
2005bdfa:	5db6                	c.lwsp	s11,108(sp)
2005bdfc:	5e46                	c.lwsp	t3,112(sp)
2005bdfe:	5ed6                	c.lwsp	t4,116(sp)
2005be00:	5f66                	c.lwsp	t5,120(sp)
2005be02:	5ff6                	c.lwsp	t6,124(sp)
2005be04:	10810113          	addi	sp,sp,264
2005be08:	30200073          	mret
2005be0c:	00000013          	addi	zero,zero,0

2005be10 <machine_timer_update>:
2005be10:	f481a283          	lw	t0,-184(gp) # 20066020 <pullMachineTimerCompareRegister>
2005be14:	e981a303          	lw	t1,-360(gp) # 20065f70 <pullNextTime>
2005be18:	5efd                	c.li	t4,-1
2005be1a:	00032383          	lw	t2,0(t1) # 4000 <GPIO_Direction+0x36>
2005be1e:	00432e03          	lw	t3,4(t1)
2005be22:	01d2a023          	sw	t4,0(t0)
2005be26:	01c2a223          	sw	t3,4(t0)
2005be2a:	0072a023          	sw	t2,0(t0)
2005be2e:	f581a283          	lw	t0,-168(gp) # 20066030 <uxTimerIncrementsForOneTick>
2005be32:	00728eb3          	add	t4,t0,t2
2005be36:	007ebf33          	sltu	t5,t4,t2
2005be3a:	01ee0fb3          	add	t6,t3,t5
2005be3e:	01d32023          	sw	t4,0(t1)
2005be42:	01f32223          	sw	t6,4(t1)
2005be46:	8082                	c.jr	ra

2005be48 <vPortHandleInterrupt>:
2005be48:	42bd                	c.li	t0,15
2005be4a:	fff2c293          	xori	t0,t0,-1
2005be4e:	00517133          	and	sp,sp,t0
2005be52:	1141                	c.addi	sp,-16
2005be54:	c006                	c.swsp	ra,0(sp)
2005be56:	34202373          	csrrs	t1,mcause,zero
2005be5a:	800003b7          	lui	t2,0x80000
2005be5e:	fff3c393          	xori	t2,t2,-1
2005be62:	007373b3          	and	t2,t1,t2
2005be66:	00038533          	add	a0,t2,zero
2005be6a:	0000a797          	auipc	a5,0xa
2005be6e:	d5a78793          	addi	a5,a5,-678 # 20065bc4 <plic_interrupt_handler>
2005be72:	9782                	c.jalr	a5

2005be74 <vPortHandleInterrupt_Exit>:
2005be74:	4082                	c.lwsp	ra,0(sp)
2005be76:	0141                	c.addi	sp,16
2005be78:	8082                	c.jr	ra
2005be7a:	0001                	c.addi	zero,0
2005be7c:	00000013          	addi	zero,zero,0

2005be80 <xPortStartFirstTask>:
2005be80:	00000297          	auipc	t0,0x0
2005be84:	d9828293          	addi	t0,t0,-616 # 2005bc18 <freertos_risc_v_trap_handler>
2005be88:	30529073          	csrrw	zero,mtvec,t0
2005be8c:	f641a383          	lw	t2,-156(gp) # 2006603c <pxCurrentTCB>
2005be90:	0003a103          	lw	sp,0(t2) # 80000000 <__ctrace_end__+0xb6000000>
2005be94:	300022f3          	csrrs	t0,mstatus,zero
2005be98:	6319                	c.lui	t1,0x6
2005be9a:	0062f2b3          	and	t0,t0,t1
2005be9e:	6309                	c.lui	t1,0x2
2005bea0:	04628c63          	beq	t0,t1,2005bef8 <xPortStartFirstTask+0x78>
2005bea4:	600a                	c.flwsp	ft0,128(sp)
2005bea6:	609a                	c.flwsp	ft1,132(sp)
2005bea8:	612a                	c.flwsp	ft2,136(sp)
2005beaa:	61ba                	c.flwsp	ft3,140(sp)
2005beac:	624a                	c.flwsp	ft4,144(sp)
2005beae:	62da                	c.flwsp	ft5,148(sp)
2005beb0:	636a                	c.flwsp	ft6,152(sp)
2005beb2:	63fa                	c.flwsp	ft7,156(sp)
2005beb4:	740a                	c.flwsp	fs0,160(sp)
2005beb6:	749a                	c.flwsp	fs1,164(sp)
2005beb8:	752a                	c.flwsp	fa0,168(sp)
2005beba:	75ba                	c.flwsp	fa1,172(sp)
2005bebc:	764a                	c.flwsp	fa2,176(sp)
2005bebe:	76da                	c.flwsp	fa3,180(sp)
2005bec0:	776a                	c.flwsp	fa4,184(sp)
2005bec2:	77fa                	c.flwsp	fa5,188(sp)
2005bec4:	680e                	c.flwsp	fa6,192(sp)
2005bec6:	689e                	c.flwsp	fa7,196(sp)
2005bec8:	692e                	c.flwsp	fs2,200(sp)
2005beca:	69be                	c.flwsp	fs3,204(sp)
2005becc:	6a4e                	c.flwsp	fs4,208(sp)
2005bece:	6ade                	c.flwsp	fs5,212(sp)
2005bed0:	6b6e                	c.flwsp	fs6,216(sp)
2005bed2:	6bfe                	c.flwsp	fs7,220(sp)
2005bed4:	7c0e                	c.flwsp	fs8,224(sp)
2005bed6:	7c9e                	c.flwsp	fs9,228(sp)
2005bed8:	7d2e                	c.flwsp	fs10,232(sp)
2005beda:	7dbe                	c.flwsp	fs11,236(sp)
2005bedc:	7e4e                	c.flwsp	ft8,240(sp)
2005bede:	7ede                	c.flwsp	ft9,244(sp)
2005bee0:	7f6e                	c.flwsp	ft10,248(sp)
2005bee2:	7ffe                	c.flwsp	ft11,252(sp)
2005bee4:	10012283          	lw	t0,256(sp)
2005bee8:	00329073          	csrrw	zero,fcsr,t0
2005beec:	6319                	c.lui	t1,0x6
2005beee:	30033073          	csrrc	zero,mstatus,t1
2005bef2:	6311                	c.lui	t1,0x4
2005bef4:	30032073          	csrrs	zero,mstatus,t1
2005bef8:	4092                	c.lwsp	ra,4(sp)
2005befa:	42d2                	c.lwsp	t0,20(sp)
2005befc:	4362                	c.lwsp	t1,24(sp)
2005befe:	43f2                	c.lwsp	t2,28(sp)
2005bf00:	5402                	c.lwsp	s0,32(sp)
2005bf02:	5492                	c.lwsp	s1,36(sp)
2005bf04:	5522                	c.lwsp	a0,40(sp)
2005bf06:	55b2                	c.lwsp	a1,44(sp)
2005bf08:	5642                	c.lwsp	a2,48(sp)
2005bf0a:	56d2                	c.lwsp	a3,52(sp)
2005bf0c:	5762                	c.lwsp	a4,56(sp)
2005bf0e:	57f2                	c.lwsp	a5,60(sp)
2005bf10:	4806                	c.lwsp	a6,64(sp)
2005bf12:	4896                	c.lwsp	a7,68(sp)
2005bf14:	4926                	c.lwsp	s2,72(sp)
2005bf16:	49b6                	c.lwsp	s3,76(sp)
2005bf18:	4a46                	c.lwsp	s4,80(sp)
2005bf1a:	4ad6                	c.lwsp	s5,84(sp)
2005bf1c:	4b66                	c.lwsp	s6,88(sp)
2005bf1e:	4bf6                	c.lwsp	s7,92(sp)
2005bf20:	5c06                	c.lwsp	s8,96(sp)
2005bf22:	5c96                	c.lwsp	s9,100(sp)
2005bf24:	5d26                	c.lwsp	s10,104(sp)
2005bf26:	5db6                	c.lwsp	s11,108(sp)
2005bf28:	5e46                	c.lwsp	t3,112(sp)
2005bf2a:	5ed6                	c.lwsp	t4,116(sp)
2005bf2c:	5f66                	c.lwsp	t5,120(sp)
2005bf2e:	5ff6                	c.lwsp	t6,124(sp)
2005bf30:	10412283          	lw	t0,260(sp)
2005bf34:	0082e293          	ori	t0,t0,8
2005bf38:	30029073          	csrrw	zero,mstatus,t0
2005bf3c:	42d2                	c.lwsp	t0,20(sp)
2005bf3e:	4082                	c.lwsp	ra,0(sp)
2005bf40:	10810113          	addi	sp,sp,264
2005bf44:	8082                	c.jr	ra
2005bf46:	0001                	c.addi	zero,0

2005bf48 <pxPortInitialiseStack>:
2005bf48:	300022f3          	csrrs	t0,mstatus,zero
2005bf4c:	ff72f293          	andi	t0,t0,-9
2005bf50:	18800313          	addi	t1,zero,392
2005bf54:	0312                	c.slli	t1,0x4
2005bf56:	0062e2b3          	or	t0,t0,t1
2005bf5a:	1571                	c.addi	a0,-4
2005bf5c:	00552023          	sw	t0,0(a0)
2005bf60:	f2450513          	addi	a0,a0,-220
2005bf64:	c110                	c.sw	a2,0(a0)
2005bf66:	fdc50513          	addi	a0,a0,-36
2005bf6a:	00052023          	sw	zero,0(a0)
2005bf6e:	4281                	c.li	t0,0

2005bf70 <chip_specific_stack_frame>:
2005bf70:	00028763          	beq	t0,zero,2005bf7e <chip_specific_stack_frame+0xe>
2005bf74:	1571                	c.addi	a0,-4
2005bf76:	00052023          	sw	zero,0(a0)
2005bf7a:	12fd                	c.addi	t0,-1
2005bf7c:	bfd5                	c.j	2005bf70 <chip_specific_stack_frame>
2005bf7e:	1571                	c.addi	a0,-4
2005bf80:	c10c                	c.sw	a1,0(a0)
2005bf82:	8082                	c.jr	ra

2005bf84 <vPortBackupRegs>:
2005bf84:	00152223          	sw	ra,4(a0)
2005bf88:	00252423          	sw	sp,8(a0)
2005bf8c:	00352623          	sw	gp,12(a0)
2005bf90:	00452823          	sw	tp,16(a0)
2005bf94:	00552a23          	sw	t0,20(a0)
2005bf98:	00652c23          	sw	t1,24(a0)
2005bf9c:	00752e23          	sw	t2,28(a0)
2005bfa0:	d100                	c.sw	s0,32(a0)
2005bfa2:	d144                	c.sw	s1,36(a0)
2005bfa4:	d508                	c.sw	a0,40(a0)
2005bfa6:	d54c                	c.sw	a1,44(a0)
2005bfa8:	d910                	c.sw	a2,48(a0)
2005bfaa:	d954                	c.sw	a3,52(a0)
2005bfac:	dd18                	c.sw	a4,56(a0)
2005bfae:	dd5c                	c.sw	a5,60(a0)
2005bfb0:	05052023          	sw	a6,64(a0)
2005bfb4:	05152223          	sw	a7,68(a0)
2005bfb8:	05252423          	sw	s2,72(a0)
2005bfbc:	05352623          	sw	s3,76(a0)
2005bfc0:	05452823          	sw	s4,80(a0)
2005bfc4:	05552a23          	sw	s5,84(a0)
2005bfc8:	05652c23          	sw	s6,88(a0)
2005bfcc:	05752e23          	sw	s7,92(a0)
2005bfd0:	07852023          	sw	s8,96(a0)
2005bfd4:	07952223          	sw	s9,100(a0)
2005bfd8:	07a52423          	sw	s10,104(a0)
2005bfdc:	07b52623          	sw	s11,108(a0)
2005bfe0:	07c52823          	sw	t3,112(a0)
2005bfe4:	07d52a23          	sw	t4,116(a0)
2005bfe8:	07e52c23          	sw	t5,120(a0)
2005bfec:	07f52e23          	sw	t6,124(a0)
2005bff0:	300022f3          	csrrs	t0,mstatus,zero
2005bff4:	0055a023          	sw	t0,0(a1)
2005bff8:	304022f3          	csrrs	t0,mie,zero
2005bffc:	0055a223          	sw	t0,4(a1)
2005c000:	305022f3          	csrrs	t0,mtvec,zero
2005c004:	0055a423          	sw	t0,8(a1)
2005c008:	0015a623          	sw	ra,12(a1)
2005c00c:	8082                	c.jr	ra

2005c00e <vPortBackupfloatRegs>:
2005c00e:	300022f3          	csrrs	t0,mstatus,zero
2005c012:	6319                	c.lui	t1,0x6
2005c014:	0062f2b3          	and	t0,t0,t1
2005c018:	06629e63          	bne	t0,t1,2005c094 <vPortBackupfloatRegs+0x86>
2005c01c:	00052027          	fsw	ft0,0(a0)
2005c020:	00152227          	fsw	ft1,4(a0)
2005c024:	00252427          	fsw	ft2,8(a0)
2005c028:	00352627          	fsw	ft3,12(a0)
2005c02c:	00452827          	fsw	ft4,16(a0)
2005c030:	00552a27          	fsw	ft5,20(a0)
2005c034:	00652c27          	fsw	ft6,24(a0)
2005c038:	00752e27          	fsw	ft7,28(a0)
2005c03c:	f100                	c.fsw	fs0,32(a0)
2005c03e:	f144                	c.fsw	fs1,36(a0)
2005c040:	f508                	c.fsw	fa0,40(a0)
2005c042:	f54c                	c.fsw	fa1,44(a0)
2005c044:	f910                	c.fsw	fa2,48(a0)
2005c046:	f954                	c.fsw	fa3,52(a0)
2005c048:	fd18                	c.fsw	fa4,56(a0)
2005c04a:	fd5c                	c.fsw	fa5,60(a0)
2005c04c:	05052027          	fsw	fa6,64(a0)
2005c050:	05152227          	fsw	fa7,68(a0)
2005c054:	05252427          	fsw	fs2,72(a0)
2005c058:	05352627          	fsw	fs3,76(a0)
2005c05c:	05452827          	fsw	fs4,80(a0)
2005c060:	05552a27          	fsw	fs5,84(a0)
2005c064:	05652c27          	fsw	fs6,88(a0)
2005c068:	05752e27          	fsw	fs7,92(a0)
2005c06c:	07852027          	fsw	fs8,96(a0)
2005c070:	07952227          	fsw	fs9,100(a0)
2005c074:	07a52427          	fsw	fs10,104(a0)
2005c078:	07b52627          	fsw	fs11,108(a0)
2005c07c:	07c52827          	fsw	ft8,112(a0)
2005c080:	07d52a27          	fsw	ft9,116(a0)
2005c084:	07e52c27          	fsw	ft10,120(a0)
2005c088:	07f52e27          	fsw	ft11,124(a0)
2005c08c:	003022f3          	csrrs	t0,fcsr,zero
2005c090:	0055a023          	sw	t0,0(a1)
2005c094:	8082                	c.jr	ra

2005c096 <vPortRestoreRegs>:
2005c096:	00852103          	lw	sp,8(a0)
2005c09a:	00c52183          	lw	gp,12(a0)
2005c09e:	01052203          	lw	tp,16(a0)
2005c0a2:	01452283          	lw	t0,20(a0)
2005c0a6:	01452283          	lw	t0,20(a0)
2005c0aa:	01852303          	lw	t1,24(a0)
2005c0ae:	01c52383          	lw	t2,28(a0)
2005c0b2:	5100                	c.lw	s0,32(a0)
2005c0b4:	5144                	c.lw	s1,36(a0)
2005c0b6:	5508                	c.lw	a0,40(a0)
2005c0b8:	554c                	c.lw	a1,44(a0)
2005c0ba:	5910                	c.lw	a2,48(a0)
2005c0bc:	5954                	c.lw	a3,52(a0)
2005c0be:	5d18                	c.lw	a4,56(a0)
2005c0c0:	5d5c                	c.lw	a5,60(a0)
2005c0c2:	04052803          	lw	a6,64(a0)
2005c0c6:	04452883          	lw	a7,68(a0)
2005c0ca:	04852903          	lw	s2,72(a0)
2005c0ce:	04c52983          	lw	s3,76(a0)
2005c0d2:	05052a03          	lw	s4,80(a0)
2005c0d6:	05452a83          	lw	s5,84(a0)
2005c0da:	05852b03          	lw	s6,88(a0)
2005c0de:	05c52b83          	lw	s7,92(a0)
2005c0e2:	06052c03          	lw	s8,96(a0)
2005c0e6:	06452c83          	lw	s9,100(a0)
2005c0ea:	06852d03          	lw	s10,104(a0)
2005c0ee:	06c52d83          	lw	s11,108(a0)
2005c0f2:	07052e03          	lw	t3,112(a0)
2005c0f6:	07452e83          	lw	t4,116(a0)
2005c0fa:	07852f03          	lw	t5,120(a0)
2005c0fe:	07c52f83          	lw	t6,124(a0)
2005c102:	0005a083          	lw	ra,0(a1)
2005c106:	30009073          	csrrw	zero,mstatus,ra
2005c10a:	0045a083          	lw	ra,4(a1)
2005c10e:	30409073          	csrrw	zero,mie,ra
2005c112:	0085a083          	lw	ra,8(a1)
2005c116:	30509073          	csrrw	zero,mtvec,ra
2005c11a:	00c5a083          	lw	ra,12(a1)
2005c11e:	8082                	c.jr	ra

2005c120 <vPortRestorefloatRegs>:
2005c120:	00062283          	lw	t0,0(a2) # 41020000 <__km4_bd_boot_download_addr__+0x1100e000>
2005c124:	6319                	c.lui	t1,0x6
2005c126:	0062f2b3          	and	t0,t0,t1
2005c12a:	06629e63          	bne	t0,t1,2005c1a6 <vPortRestorefloatRegs+0x86>
2005c12e:	00052007          	flw	ft0,0(a0)
2005c132:	00452087          	flw	ft1,4(a0)
2005c136:	00852107          	flw	ft2,8(a0)
2005c13a:	00c52187          	flw	ft3,12(a0)
2005c13e:	01052207          	flw	ft4,16(a0)
2005c142:	01452287          	flw	ft5,20(a0)
2005c146:	01852307          	flw	ft6,24(a0)
2005c14a:	01c52387          	flw	ft7,28(a0)
2005c14e:	7100                	c.flw	fs0,32(a0)
2005c150:	7144                	c.flw	fs1,36(a0)
2005c152:	7508                	c.flw	fa0,40(a0)
2005c154:	754c                	c.flw	fa1,44(a0)
2005c156:	7910                	c.flw	fa2,48(a0)
2005c158:	7954                	c.flw	fa3,52(a0)
2005c15a:	7d18                	c.flw	fa4,56(a0)
2005c15c:	7d5c                	c.flw	fa5,60(a0)
2005c15e:	04052807          	flw	fa6,64(a0)
2005c162:	04452887          	flw	fa7,68(a0)
2005c166:	04852907          	flw	fs2,72(a0)
2005c16a:	04c52987          	flw	fs3,76(a0)
2005c16e:	05052a07          	flw	fs4,80(a0)
2005c172:	05452a87          	flw	fs5,84(a0)
2005c176:	05852b07          	flw	fs6,88(a0)
2005c17a:	05c52b87          	flw	fs7,92(a0)
2005c17e:	06052c07          	flw	fs8,96(a0)
2005c182:	06452c87          	flw	fs9,100(a0)
2005c186:	06852d07          	flw	fs10,104(a0)
2005c18a:	06c52d87          	flw	fs11,108(a0)
2005c18e:	07052e07          	flw	ft8,112(a0)
2005c192:	07452e87          	flw	ft9,116(a0)
2005c196:	07852f07          	flw	ft10,120(a0)
2005c19a:	07c52f87          	flw	ft11,124(a0)
2005c19e:	0005a283          	lw	t0,0(a1)
2005c1a2:	00329073          	csrrw	zero,fcsr,t0
2005c1a6:	8082                	c.jr	ra

2005c1a8 <vPortBackupPmp>:
2005c1a8:	3b0022f3          	csrrs	t0,pmpaddr0,zero
2005c1ac:	00552023          	sw	t0,0(a0)
2005c1b0:	3b1022f3          	csrrs	t0,pmpaddr1,zero
2005c1b4:	00552223          	sw	t0,4(a0)
2005c1b8:	3b2022f3          	csrrs	t0,pmpaddr2,zero
2005c1bc:	00552423          	sw	t0,8(a0)
2005c1c0:	3b3022f3          	csrrs	t0,pmpaddr3,zero
2005c1c4:	00552623          	sw	t0,12(a0)
2005c1c8:	3b4022f3          	csrrs	t0,pmpaddr4,zero
2005c1cc:	00552823          	sw	t0,16(a0)
2005c1d0:	3b5022f3          	csrrs	t0,pmpaddr5,zero
2005c1d4:	00552a23          	sw	t0,20(a0)
2005c1d8:	3b6022f3          	csrrs	t0,pmpaddr6,zero
2005c1dc:	00552c23          	sw	t0,24(a0)
2005c1e0:	3b7022f3          	csrrs	t0,pmpaddr7,zero
2005c1e4:	00552e23          	sw	t0,28(a0)
2005c1e8:	3b8022f3          	csrrs	t0,pmpaddr8,zero
2005c1ec:	02552023          	sw	t0,32(a0)
2005c1f0:	3b9022f3          	csrrs	t0,pmpaddr9,zero
2005c1f4:	02552223          	sw	t0,36(a0)
2005c1f8:	3ba022f3          	csrrs	t0,pmpaddr10,zero
2005c1fc:	02552423          	sw	t0,40(a0)
2005c200:	3bb022f3          	csrrs	t0,pmpaddr11,zero
2005c204:	02552623          	sw	t0,44(a0)
2005c208:	3bc022f3          	csrrs	t0,pmpaddr12,zero
2005c20c:	02552823          	sw	t0,48(a0)
2005c210:	3bd022f3          	csrrs	t0,pmpaddr13,zero
2005c214:	02552a23          	sw	t0,52(a0)
2005c218:	3be022f3          	csrrs	t0,pmpaddr14,zero
2005c21c:	02552c23          	sw	t0,56(a0)
2005c220:	3bf022f3          	csrrs	t0,pmpaddr15,zero
2005c224:	02552e23          	sw	t0,60(a0)
2005c228:	3a0022f3          	csrrs	t0,pmpcfg0,zero
2005c22c:	04552023          	sw	t0,64(a0)
2005c230:	3a2022f3          	csrrs	t0,pmpcfg2,zero
2005c234:	04552223          	sw	t0,68(a0)
2005c238:	3a1022f3          	csrrs	t0,pmpcfg1,zero
2005c23c:	04552423          	sw	t0,72(a0)
2005c240:	3a3022f3          	csrrs	t0,pmpcfg3,zero
2005c244:	04552623          	sw	t0,76(a0)
2005c248:	8082                	c.jr	ra

2005c24a <vPortRestorePmp>:
2005c24a:	00052283          	lw	t0,0(a0)
2005c24e:	3b029073          	csrrw	zero,pmpaddr0,t0
2005c252:	00452283          	lw	t0,4(a0)
2005c256:	3b129073          	csrrw	zero,pmpaddr1,t0
2005c25a:	00852283          	lw	t0,8(a0)
2005c25e:	3b229073          	csrrw	zero,pmpaddr2,t0
2005c262:	00c52283          	lw	t0,12(a0)
2005c266:	3b329073          	csrrw	zero,pmpaddr3,t0
2005c26a:	01052283          	lw	t0,16(a0)
2005c26e:	3b429073          	csrrw	zero,pmpaddr4,t0
2005c272:	01452283          	lw	t0,20(a0)
2005c276:	3b529073          	csrrw	zero,pmpaddr5,t0
2005c27a:	01852283          	lw	t0,24(a0)
2005c27e:	3b629073          	csrrw	zero,pmpaddr6,t0
2005c282:	01c52283          	lw	t0,28(a0)
2005c286:	3b729073          	csrrw	zero,pmpaddr7,t0
2005c28a:	02052283          	lw	t0,32(a0)
2005c28e:	3b829073          	csrrw	zero,pmpaddr8,t0
2005c292:	02452283          	lw	t0,36(a0)
2005c296:	3b929073          	csrrw	zero,pmpaddr9,t0
2005c29a:	02852283          	lw	t0,40(a0)
2005c29e:	3ba29073          	csrrw	zero,pmpaddr10,t0
2005c2a2:	02c52283          	lw	t0,44(a0)
2005c2a6:	3bb29073          	csrrw	zero,pmpaddr11,t0
2005c2aa:	03052283          	lw	t0,48(a0)
2005c2ae:	3bc29073          	csrrw	zero,pmpaddr12,t0
2005c2b2:	03452283          	lw	t0,52(a0)
2005c2b6:	3bd29073          	csrrw	zero,pmpaddr13,t0
2005c2ba:	03852283          	lw	t0,56(a0)
2005c2be:	3be29073          	csrrw	zero,pmpaddr14,t0
2005c2c2:	03c52283          	lw	t0,60(a0)
2005c2c6:	3bf29073          	csrrw	zero,pmpaddr15,t0
2005c2ca:	04052283          	lw	t0,64(a0)
2005c2ce:	3a029073          	csrrw	zero,pmpcfg0,t0
2005c2d2:	04452283          	lw	t0,68(a0)
2005c2d6:	3a229073          	csrrw	zero,pmpcfg2,t0
2005c2da:	04852283          	lw	t0,72(a0)
2005c2de:	3a129073          	csrrw	zero,pmpcfg1,t0
2005c2e2:	04c52283          	lw	t0,76(a0)
2005c2e6:	3a329073          	csrrw	zero,pmpcfg3,t0
2005c2ea:	8082                	c.jr	ra
2005c2ec:	0000                	c.unimp
2005c2ee:	0000                	c.unimp
	...

2005c2f2 <vPortSetupTimerInterrupt>:
2005c2f2:	e781a783          	lw	a5,-392(gp) # 20065f50 <SystemCoreClock>
2005c2f6:	3e800713          	addi	a4,zero,1000
2005c2fa:	02e7d7b3          	divu	a5,a5,a4
2005c2fe:	1141                	c.addi	sp,-16
2005c300:	f4f1ac23          	sw	a5,-168(gp) # 20066030 <uxTimerIncrementsForOneTick>
2005c304:	f14027f3          	csrrs	a5,mhartid,zero
2005c308:	c63e                	c.swsp	a5,12(sp)
2005c30a:	4632                	c.lwsp	a2,12(sp)
2005c30c:	10000737          	lui	a4,0x10000
2005c310:	00170793          	addi	a5,a4,1 # 10000001 <__km4_boot_text_start__+0x7fffe1>
2005c314:	963e                	c.add	a2,a5
2005c316:	060e                	c.slli	a2,0x3
2005c318:	f4c1a423          	sw	a2,-184(gp) # 20066020 <pullMachineTimerCompareRegister>
2005c31c:	47b2                	c.lwsp	a5,12(sp)
2005c31e:	f5818593          	addi	a1,gp,-168 # 20066030 <uxTimerIncrementsForOneTick>
2005c322:	97ba                	c.add	a5,a4
2005c324:	078e                	c.slli	a5,0x3
2005c326:	f4f1a623          	sw	a5,-180(gp) # 20066024 <pullMachineTimerCounterRegister>
2005c32a:	800007b7          	lui	a5,0x80000
2005c32e:	43c8                	c.lw	a0,4(a5)
2005c330:	4394                	c.lw	a3,0(a5)
2005c332:	43d8                	c.lw	a4,4(a5)
2005c334:	fea71de3          	bne	a4,a0,2005c32e <vPortSetupTimerInterrupt+0x3c>
2005c338:	4188                	c.lw	a0,0(a1)
2005c33a:	9536                	c.add	a0,a3
2005c33c:	c208                	c.sw	a0,0(a2)
2005c33e:	419c                	c.lw	a5,0(a1)
2005c340:	00d536b3          	sltu	a3,a0,a3
2005c344:	96ba                	c.add	a3,a4
2005c346:	00a78733          	add	a4,a5,a0
2005c34a:	c254                	c.sw	a3,4(a2)
2005c34c:	00f737b3          	sltu	a5,a4,a5
2005c350:	f5018613          	addi	a2,gp,-176 # 20066028 <ullNextTime>
2005c354:	96be                	c.add	a3,a5
2005c356:	c218                	c.sw	a4,0(a2)
2005c358:	c254                	c.sw	a3,4(a2)
2005c35a:	0141                	c.addi	sp,16
2005c35c:	8082                	c.jr	ra

2005c35e <xPortStartScheduler>:
2005c35e:	1141                	c.addi	sp,-16
2005c360:	c606                	c.swsp	ra,12(sp)
2005c362:	f91ff0ef          	jal	ra,2005c2f2 <vPortSetupTimerInterrupt>
2005c366:	6785                	c.lui	a5,0x1
2005c368:	88078793          	addi	a5,a5,-1920 # 880 <__NVIC_GetVectorArg+0xce>
2005c36c:	3047a073          	csrrs	zero,mie,a5
2005c370:	b11ff0ef          	jal	ra,2005be80 <xPortStartFirstTask>
2005c374:	40b2                	c.lwsp	ra,12(sp)
2005c376:	4501                	c.li	a0,0
2005c378:	0141                	c.addi	sp,16
2005c37a:	8082                	c.jr	ra

2005c37c <vApplicationTickHook>:
2005c37c:	8082                	c.jr	ra

2005c37e <vApplicationIdleHook>:
2005c37e:	1101                	c.addi	sp,-32
2005c380:	ce06                	c.swsp	ra,28(sp)
2005c382:	ebefe0ef          	jal	ra,2005aa40 <xPortGetFreeHeapSize>
2005c386:	40f2                	c.lwsp	ra,28(sp)
2005c388:	c62a                	c.swsp	a0,12(sp)
2005c38a:	47b2                	c.lwsp	a5,12(sp)
2005c38c:	6105                	c.addi16sp	sp,32
2005c38e:	8082                	c.jr	ra

2005c390 <vApplicationMallocFailedHook>:
2005c390:	1101                	c.addi	sp,-32
2005c392:	ce06                	c.swsp	ra,28(sp)
2005c394:	3e2010ef          	jal	ra,2005d776 <xTaskGetSchedulerState>
2005c398:	4785                	c.li	a5,1
2005c39a:	02f50863          	beq	a0,a5,2005c3ca <vApplicationMallocFailedHook+0x3a>
2005c39e:	4501                	c.li	a0,0
2005c3a0:	0aa010ef          	jal	ra,2005d44a <pcTaskGetName>
2005c3a4:	862a                	c.mv	a2,a0
2005c3a6:	c632                	c.swsp	a2,12(sp)
2005c3a8:	e98fe0ef          	jal	ra,2005aa40 <xPortGetFreeHeapSize>
2005c3ac:	4632                	c.lwsp	a2,12(sp)
2005c3ae:	86aa                	c.mv	a3,a0
2005c3b0:	200635b7          	lui	a1,0x20063
2005c3b4:	20063537          	lui	a0,0x20063
2005c3b8:	cb458593          	addi	a1,a1,-844 # 20062cb4 <__FUNCTION__.0+0x574>
2005c3bc:	cb850513          	addi	a0,a0,-840 # 20062cb8 <__FUNCTION__.0+0x578>
2005c3c0:	48f020ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005c3c4:	30047073          	csrrci	zero,mstatus,8
2005c3c8:	a001                	c.j	2005c3c8 <vApplicationMallocFailedHook+0x38>
2005c3ca:	20063637          	lui	a2,0x20063
2005c3ce:	cac60613          	addi	a2,a2,-852 # 20062cac <__FUNCTION__.0+0x56c>
2005c3d2:	bfd1                	c.j	2005c3a6 <vApplicationMallocFailedHook+0x16>

2005c3d4 <vApplicationStackOverflowHook>:
2005c3d4:	862e                	c.mv	a2,a1
2005c3d6:	20063537          	lui	a0,0x20063
2005c3da:	200635b7          	lui	a1,0x20063
2005c3de:	1141                	c.addi	sp,-16
2005c3e0:	d1c58593          	addi	a1,a1,-740 # 20062d1c <__FUNCTION__.5>
2005c3e4:	cf450513          	addi	a0,a0,-780 # 20062cf4 <__FUNCTION__.0+0x5b4>
2005c3e8:	c606                	c.swsp	ra,12(sp)
2005c3ea:	465020ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005c3ee:	a001                	c.j	2005c3ee <vApplicationStackOverflowHook+0x1a>

2005c3f0 <vApplicationGetIdleTaskMemory>:
2005c3f0:	200677b7          	lui	a5,0x20067
2005c3f4:	70078793          	addi	a5,a5,1792 # 20067700 <xIdleTaskTCB.4>
2005c3f8:	c11c                	c.sw	a5,0(a0)
2005c3fa:	200647b7          	lui	a5,0x20064
2005c3fe:	52078793          	addi	a5,a5,1312 # 20064520 <uxIdleTaskStack.3>
2005c402:	c19c                	c.sw	a5,0(a1)
2005c404:	20000793          	addi	a5,zero,512
2005c408:	c21c                	c.sw	a5,0(a2)
2005c40a:	8082                	c.jr	ra

2005c40c <vApplicationGetTimerTaskMemory>:
2005c40c:	200687b7          	lui	a5,0x20068
2005c410:	86478793          	addi	a5,a5,-1948 # 20067864 <xTimerTaskTCB.2>
2005c414:	c11c                	c.sw	a5,0(a0)
2005c416:	200657b7          	lui	a5,0x20065
2005c41a:	d2078793          	addi	a5,a5,-736 # 20064d20 <uxTimerTaskStack.1>
2005c41e:	c19c                	c.sw	a5,0(a1)
2005c420:	20000793          	addi	a5,zero,512
2005c424:	c21c                	c.sw	a5,0(a2)
2005c426:	8082                	c.jr	ra

2005c428 <vPortSuppressTicksAndSleep>:
2005c428:	1141                	c.addi	sp,-16
2005c42a:	c422                	c.swsp	s0,8(sp)
2005c42c:	4501                	c.li	a0,0
2005c42e:	c606                	c.swsp	ra,12(sp)
2005c430:	e801a023          	sw	zero,-384(gp) # 20065f58 <system_can_yield>
2005c434:	e76fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c438:	300477f3          	csrrci	a5,mstatus,8
2005c43c:	304010ef          	jal	ra,2005d740 <eTaskConfirmSleepModeStatus>
2005c440:	c90d                	c.beqz	a0,2005c472 <vPortSuppressTicksAndSleep+0x4a>
2005c442:	b4efd0ef          	jal	ra,20059790 <pmu_ready_to_sleep>
2005c446:	e515                	c.bnez	a0,2005c472 <vPortSuppressTicksAndSleep+0x4a>
2005c448:	4505                	c.li	a0,1
2005c44a:	e60fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c44e:	0ff0000f          	fence	iorw,iorw
2005c452:	10500073          	wfi
2005c456:	0001                	c.addi	zero,0
2005c458:	0ff0000f          	fence	iorw,iorw
2005c45c:	0001                	c.addi	zero,0
2005c45e:	0001                	c.addi	zero,0
2005c460:	0001                	c.addi	zero,0
2005c462:	0001                	c.addi	zero,0
2005c464:	0001                	c.addi	zero,0
2005c466:	0001                	c.addi	zero,0
2005c468:	0001                	c.addi	zero,0
2005c46a:	0001                	c.addi	zero,0
2005c46c:	0001                	c.addi	zero,0
2005c46e:	0001                	c.addi	zero,0
2005c470:	0001                	c.addi	zero,0
2005c472:	300467f3          	csrrsi	a5,mstatus,8
2005c476:	4505                	c.li	a0,1
2005c478:	e32fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c47c:	4785                	c.li	a5,1
2005c47e:	40b2                	c.lwsp	ra,12(sp)
2005c480:	e8f1a023          	sw	a5,-384(gp) # 20065f58 <system_can_yield>
2005c484:	4422                	c.lwsp	s0,8(sp)
2005c486:	0141                	c.addi	sp,16
2005c488:	8082                	c.jr	ra

2005c48a <prvIsQueueEmpty>:
2005c48a:	1141                	c.addi	sp,-16
2005c48c:	c422                	c.swsp	s0,8(sp)
2005c48e:	842a                	c.mv	s0,a0
2005c490:	c606                	c.swsp	ra,12(sp)
2005c492:	4c6010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c496:	5c00                	c.lw	s0,56(s0)
2005c498:	4da010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c49c:	40b2                	c.lwsp	ra,12(sp)
2005c49e:	00143513          	sltiu	a0,s0,1
2005c4a2:	4422                	c.lwsp	s0,8(sp)
2005c4a4:	0141                	c.addi	sp,16
2005c4a6:	8082                	c.jr	ra

2005c4a8 <prvCopyDataToQueue>:
2005c4a8:	1141                	c.addi	sp,-16
2005c4aa:	c422                	c.swsp	s0,8(sp)
2005c4ac:	c226                	c.swsp	s1,4(sp)
2005c4ae:	c606                	c.swsp	ra,12(sp)
2005c4b0:	c04a                	c.swsp	s2,0(sp)
2005c4b2:	84b2                	c.mv	s1,a2
2005c4b4:	4130                	c.lw	a2,64(a0)
2005c4b6:	03852903          	lw	s2,56(a0)
2005c4ba:	842a                	c.mv	s0,a0
2005c4bc:	e605                	c.bnez	a2,2005c4e4 <prvCopyDataToQueue+0x3c>
2005c4be:	411c                	c.lw	a5,0(a0)
2005c4c0:	4481                	c.li	s1,0
2005c4c2:	e799                	c.bnez	a5,2005c4d0 <prvCopyDataToQueue+0x28>
2005c4c4:	4508                	c.lw	a0,8(a0)
2005c4c6:	378010ef          	jal	ra,2005d83e <xTaskPriorityDisinherit>
2005c4ca:	84aa                	c.mv	s1,a0
2005c4cc:	00042423          	sw	zero,8(s0)
2005c4d0:	0905                	c.addi	s2,1
2005c4d2:	03242c23          	sw	s2,56(s0)
2005c4d6:	40b2                	c.lwsp	ra,12(sp)
2005c4d8:	4422                	c.lwsp	s0,8(sp)
2005c4da:	4902                	c.lwsp	s2,0(sp)
2005c4dc:	8526                	c.mv	a0,s1
2005c4de:	4492                	c.lwsp	s1,4(sp)
2005c4e0:	0141                	c.addi	sp,16
2005c4e2:	8082                	c.jr	ra
2005c4e4:	e085                	c.bnez	s1,2005c504 <prvCopyDataToQueue+0x5c>
2005c4e6:	4148                	c.lw	a0,4(a0)
2005c4e8:	dffad097          	auipc	ra,0xdffad
2005c4ec:	5e8080e7          	jalr	ra,1512(ra) # 9ad0 <__wrap_memcpy>
2005c4f0:	405c                	c.lw	a5,4(s0)
2005c4f2:	4038                	c.lw	a4,64(s0)
2005c4f4:	97ba                	c.add	a5,a4
2005c4f6:	4418                	c.lw	a4,8(s0)
2005c4f8:	c05c                	c.sw	a5,4(s0)
2005c4fa:	fce7ebe3          	bltu	a5,a4,2005c4d0 <prvCopyDataToQueue+0x28>
2005c4fe:	401c                	c.lw	a5,0(s0)
2005c500:	c05c                	c.sw	a5,4(s0)
2005c502:	b7f9                	c.j	2005c4d0 <prvCopyDataToQueue+0x28>
2005c504:	4548                	c.lw	a0,12(a0)
2005c506:	dffad097          	auipc	ra,0xdffad
2005c50a:	5ca080e7          	jalr	ra,1482(ra) # 9ad0 <__wrap_memcpy>
2005c50e:	4038                	c.lw	a4,64(s0)
2005c510:	445c                	c.lw	a5,12(s0)
2005c512:	40e006b3          	sub	a3,zero,a4
2005c516:	8f99                	c.sub	a5,a4
2005c518:	4018                	c.lw	a4,0(s0)
2005c51a:	c45c                	c.sw	a5,12(s0)
2005c51c:	00e7f563          	bgeu	a5,a4,2005c526 <prvCopyDataToQueue+0x7e>
2005c520:	441c                	c.lw	a5,8(s0)
2005c522:	97b6                	c.add	a5,a3
2005c524:	c45c                	c.sw	a5,12(s0)
2005c526:	4789                	c.li	a5,2
2005c528:	00f49763          	bne	s1,a5,2005c536 <prvCopyDataToQueue+0x8e>
2005c52c:	4481                	c.li	s1,0
2005c52e:	fa0901e3          	beq	s2,zero,2005c4d0 <prvCopyDataToQueue+0x28>
2005c532:	197d                	c.addi	s2,-1
2005c534:	bf71                	c.j	2005c4d0 <prvCopyDataToQueue+0x28>
2005c536:	4481                	c.li	s1,0
2005c538:	bf61                	c.j	2005c4d0 <prvCopyDataToQueue+0x28>

2005c53a <prvCopyDataFromQueue>:
2005c53a:	87aa                	c.mv	a5,a0
2005c53c:	43b0                	c.lw	a2,64(a5)
2005c53e:	852e                	c.mv	a0,a1
2005c540:	ce11                	c.beqz	a2,2005c55c <prvCopyDataFromQueue+0x22>
2005c542:	47d8                	c.lw	a4,12(a5)
2005c544:	4794                	c.lw	a3,8(a5)
2005c546:	9732                	c.add	a4,a2
2005c548:	c7d8                	c.sw	a4,12(a5)
2005c54a:	00d76463          	bltu	a4,a3,2005c552 <prvCopyDataFromQueue+0x18>
2005c54e:	4398                	c.lw	a4,0(a5)
2005c550:	c7d8                	c.sw	a4,12(a5)
2005c552:	47cc                	c.lw	a1,12(a5)
2005c554:	dffad317          	auipc	t1,0xdffad
2005c558:	57c30067          	jalr	zero,1404(t1) # 9ad0 <__wrap_memcpy>
2005c55c:	8082                	c.jr	ra

2005c55e <prvUnlockQueue>:
2005c55e:	1141                	c.addi	sp,-16
2005c560:	c422                	c.swsp	s0,8(sp)
2005c562:	842a                	c.mv	s0,a0
2005c564:	c226                	c.swsp	s1,4(sp)
2005c566:	c04a                	c.swsp	s2,0(sp)
2005c568:	c606                	c.swsp	ra,12(sp)
2005c56a:	3ee010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c56e:	04544483          	lbu	s1,69(s0)
2005c572:	02440913          	addi	s2,s0,36
2005c576:	04e2                	c.slli	s1,0x18
2005c578:	84e1                	c.srai	s1,0x18
2005c57a:	02904b63          	blt	zero,s1,2005c5b0 <prvUnlockQueue+0x52>
2005c57e:	57fd                	c.li	a5,-1
2005c580:	04f402a3          	sb	a5,69(s0)
2005c584:	3ee010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c588:	3d0010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c58c:	04444483          	lbu	s1,68(s0)
2005c590:	01040913          	addi	s2,s0,16
2005c594:	04e2                	c.slli	s1,0x18
2005c596:	84e1                	c.srai	s1,0x18
2005c598:	02904863          	blt	zero,s1,2005c5c8 <prvUnlockQueue+0x6a>
2005c59c:	57fd                	c.li	a5,-1
2005c59e:	04f40223          	sb	a5,68(s0)
2005c5a2:	4422                	c.lwsp	s0,8(sp)
2005c5a4:	40b2                	c.lwsp	ra,12(sp)
2005c5a6:	4492                	c.lwsp	s1,4(sp)
2005c5a8:	4902                	c.lwsp	s2,0(sp)
2005c5aa:	0141                	c.addi	sp,16
2005c5ac:	3c60106f          	jal	zero,2005d972 <vTaskExitCritical>
2005c5b0:	505c                	c.lw	a5,36(s0)
2005c5b2:	d7f1                	c.beqz	a5,2005c57e <prvUnlockQueue+0x20>
2005c5b4:	854a                	c.mv	a0,s2
2005c5b6:	0f2010ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005c5ba:	c119                	c.beqz	a0,2005c5c0 <prvUnlockQueue+0x62>
2005c5bc:	17c010ef          	jal	ra,2005d738 <vTaskMissedYield>
2005c5c0:	14fd                	c.addi	s1,-1
2005c5c2:	04e2                	c.slli	s1,0x18
2005c5c4:	84e1                	c.srai	s1,0x18
2005c5c6:	bf55                	c.j	2005c57a <prvUnlockQueue+0x1c>
2005c5c8:	481c                	c.lw	a5,16(s0)
2005c5ca:	dbe9                	c.beqz	a5,2005c59c <prvUnlockQueue+0x3e>
2005c5cc:	854a                	c.mv	a0,s2
2005c5ce:	0da010ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005c5d2:	c119                	c.beqz	a0,2005c5d8 <prvUnlockQueue+0x7a>
2005c5d4:	164010ef          	jal	ra,2005d738 <vTaskMissedYield>
2005c5d8:	14fd                	c.addi	s1,-1
2005c5da:	04e2                	c.slli	s1,0x18
2005c5dc:	84e1                	c.srai	s1,0x18
2005c5de:	bf6d                	c.j	2005c598 <prvUnlockQueue+0x3a>

2005c5e0 <xQueueGenericReset>:
2005c5e0:	1141                	c.addi	sp,-16
2005c5e2:	c422                	c.swsp	s0,8(sp)
2005c5e4:	c226                	c.swsp	s1,4(sp)
2005c5e6:	842a                	c.mv	s0,a0
2005c5e8:	c606                	c.swsp	ra,12(sp)
2005c5ea:	84ae                	c.mv	s1,a1
2005c5ec:	36c010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c5f0:	4034                	c.lw	a3,64(s0)
2005c5f2:	5c5c                	c.lw	a5,60(s0)
2005c5f4:	4018                	c.lw	a4,0(s0)
2005c5f6:	02042c23          	sw	zero,56(s0)
2005c5fa:	02f687b3          	mul	a5,a3,a5
2005c5fe:	c058                	c.sw	a4,4(s0)
2005c600:	00f70633          	add	a2,a4,a5
2005c604:	8f95                	c.sub	a5,a3
2005c606:	97ba                	c.add	a5,a4
2005c608:	c45c                	c.sw	a5,12(s0)
2005c60a:	57fd                	c.li	a5,-1
2005c60c:	04f40223          	sb	a5,68(s0)
2005c610:	c410                	c.sw	a2,8(s0)
2005c612:	04f402a3          	sb	a5,69(s0)
2005c616:	e095                	c.bnez	s1,2005c63a <xQueueGenericReset+0x5a>
2005c618:	481c                	c.lw	a5,16(s0)
2005c61a:	cb81                	c.beqz	a5,2005c62a <xQueueGenericReset+0x4a>
2005c61c:	01040513          	addi	a0,s0,16
2005c620:	088010ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005c624:	c119                	c.beqz	a0,2005c62a <xQueueGenericReset+0x4a>
2005c626:	00000073          	ecall
2005c62a:	348010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c62e:	40b2                	c.lwsp	ra,12(sp)
2005c630:	4422                	c.lwsp	s0,8(sp)
2005c632:	4492                	c.lwsp	s1,4(sp)
2005c634:	4505                	c.li	a0,1
2005c636:	0141                	c.addi	sp,16
2005c638:	8082                	c.jr	ra
2005c63a:	01040513          	addi	a0,s0,16
2005c63e:	d00fe0ef          	jal	ra,2005ab3e <vListInitialise>
2005c642:	02440513          	addi	a0,s0,36
2005c646:	cf8fe0ef          	jal	ra,2005ab3e <vListInitialise>
2005c64a:	b7c5                	c.j	2005c62a <xQueueGenericReset+0x4a>

2005c64c <xQueueGenericSend>:
2005c64c:	7139                	c.addi16sp	sp,-64
2005c64e:	dc22                	c.swsp	s0,56(sp)
2005c650:	da26                	c.swsp	s1,52(sp)
2005c652:	d84a                	c.swsp	s2,48(sp)
2005c654:	d64e                	c.swsp	s3,44(sp)
2005c656:	d452                	c.swsp	s4,40(sp)
2005c658:	d256                	c.swsp	s5,36(sp)
2005c65a:	d05a                	c.swsp	s6,32(sp)
2005c65c:	de06                	c.swsp	ra,60(sp)
2005c65e:	842a                	c.mv	s0,a0
2005c660:	892e                	c.mv	s2,a1
2005c662:	c632                	c.swsp	a2,12(sp)
2005c664:	84b6                	c.mv	s1,a3
2005c666:	4a01                	c.li	s4,0
2005c668:	4a89                	c.li	s5,2
2005c66a:	59fd                	c.li	s3,-1
2005c66c:	01050b13          	addi	s6,a0,16
2005c670:	2e8010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c674:	5c18                	c.lw	a4,56(s0)
2005c676:	5c5c                	c.lw	a5,60(s0)
2005c678:	00f76463          	bltu	a4,a5,2005c680 <xQueueGenericSend+0x34>
2005c67c:	03549d63          	bne	s1,s5,2005c6b6 <xQueueGenericSend+0x6a>
2005c680:	8626                	c.mv	a2,s1
2005c682:	85ca                	c.mv	a1,s2
2005c684:	8522                	c.mv	a0,s0
2005c686:	e23ff0ef          	jal	ra,2005c4a8 <prvCopyDataToQueue>
2005c68a:	505c                	c.lw	a5,36(s0)
2005c68c:	c789                	c.beqz	a5,2005c696 <xQueueGenericSend+0x4a>
2005c68e:	02440513          	addi	a0,s0,36
2005c692:	016010ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005c696:	c119                	c.beqz	a0,2005c69c <xQueueGenericSend+0x50>
2005c698:	00000073          	ecall
2005c69c:	2d6010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c6a0:	4505                	c.li	a0,1
2005c6a2:	50f2                	c.lwsp	ra,60(sp)
2005c6a4:	5462                	c.lwsp	s0,56(sp)
2005c6a6:	54d2                	c.lwsp	s1,52(sp)
2005c6a8:	5942                	c.lwsp	s2,48(sp)
2005c6aa:	59b2                	c.lwsp	s3,44(sp)
2005c6ac:	5a22                	c.lwsp	s4,40(sp)
2005c6ae:	5a92                	c.lwsp	s5,36(sp)
2005c6b0:	5b02                	c.lwsp	s6,32(sp)
2005c6b2:	6121                	c.addi16sp	sp,64
2005c6b4:	8082                	c.jr	ra
2005c6b6:	47b2                	c.lwsp	a5,12(sp)
2005c6b8:	e789                	c.bnez	a5,2005c6c2 <xQueueGenericSend+0x76>
2005c6ba:	2b8010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c6be:	4501                	c.li	a0,0
2005c6c0:	b7cd                	c.j	2005c6a2 <xQueueGenericSend+0x56>
2005c6c2:	000a1563          	bne	s4,zero,2005c6cc <xQueueGenericSend+0x80>
2005c6c6:	0828                	c.addi4spn	a0,sp,24
2005c6c8:	062010ef          	jal	ra,2005d72a <vTaskInternalSetTimeOutState>
2005c6cc:	2a6010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c6d0:	563000ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005c6d4:	284010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c6d8:	04444783          	lbu	a5,68(s0)
2005c6dc:	07e2                	c.slli	a5,0x18
2005c6de:	87e1                	c.srai	a5,0x18
2005c6e0:	01379463          	bne	a5,s3,2005c6e8 <xQueueGenericSend+0x9c>
2005c6e4:	04040223          	sb	zero,68(s0)
2005c6e8:	04544783          	lbu	a5,69(s0)
2005c6ec:	07e2                	c.slli	a5,0x18
2005c6ee:	87e1                	c.srai	a5,0x18
2005c6f0:	01379463          	bne	a5,s3,2005c6f8 <xQueueGenericSend+0xac>
2005c6f4:	040402a3          	sb	zero,69(s0)
2005c6f8:	27a010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c6fc:	006c                	c.addi4spn	a1,sp,12
2005c6fe:	0828                	c.addi4spn	a0,sp,24
2005c700:	18b010ef          	jal	ra,2005e08a <xTaskCheckForTimeOut>
2005c704:	e121                	c.bnez	a0,2005c744 <xQueueGenericSend+0xf8>
2005c706:	252010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c70a:	5c18                	c.lw	a4,56(s0)
2005c70c:	5c5c                	c.lw	a5,60(s0)
2005c70e:	02f71063          	bne	a4,a5,2005c72e <xQueueGenericSend+0xe2>
2005c712:	260010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c716:	45b2                	c.lwsp	a1,12(sp)
2005c718:	855a                	c.mv	a0,s6
2005c71a:	745000ef          	jal	ra,2005d65e <vTaskPlaceOnEventList>
2005c71e:	8522                	c.mv	a0,s0
2005c720:	e3fff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c724:	5bc010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c728:	c919                	c.beqz	a0,2005c73e <xQueueGenericSend+0xf2>
2005c72a:	4a05                	c.li	s4,1
2005c72c:	b791                	c.j	2005c670 <xQueueGenericSend+0x24>
2005c72e:	244010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c732:	8522                	c.mv	a0,s0
2005c734:	e2bff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c738:	5a8010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c73c:	b7fd                	c.j	2005c72a <xQueueGenericSend+0xde>
2005c73e:	00000073          	ecall
2005c742:	b7e5                	c.j	2005c72a <xQueueGenericSend+0xde>
2005c744:	8522                	c.mv	a0,s0
2005c746:	e19ff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c74a:	596010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c74e:	bf85                	c.j	2005c6be <xQueueGenericSend+0x72>

2005c750 <xQueueGenericSendFromISR>:
2005c750:	1141                	c.addi	sp,-16
2005c752:	c422                	c.swsp	s0,8(sp)
2005c754:	c04a                	c.swsp	s2,0(sp)
2005c756:	c606                	c.swsp	ra,12(sp)
2005c758:	c226                	c.swsp	s1,4(sp)
2005c75a:	5d18                	c.lw	a4,56(a0)
2005c75c:	5d5c                	c.lw	a5,60(a0)
2005c75e:	8932                	c.mv	s2,a2
2005c760:	842a                	c.mv	s0,a0
2005c762:	8636                	c.mv	a2,a3
2005c764:	00f76663          	bltu	a4,a5,2005c770 <xQueueGenericSendFromISR+0x20>
2005c768:	4789                	c.li	a5,2
2005c76a:	4501                	c.li	a0,0
2005c76c:	02f69063          	bne	a3,a5,2005c78c <xQueueGenericSendFromISR+0x3c>
2005c770:	04544483          	lbu	s1,69(s0)
2005c774:	5c1c                	c.lw	a5,56(s0)
2005c776:	8522                	c.mv	a0,s0
2005c778:	04e2                	c.slli	s1,0x18
2005c77a:	d2fff0ef          	jal	ra,2005c4a8 <prvCopyDataToQueue>
2005c77e:	84e1                	c.srai	s1,0x18
2005c780:	57fd                	c.li	a5,-1
2005c782:	02f49663          	bne	s1,a5,2005c7ae <xQueueGenericSendFromISR+0x5e>
2005c786:	505c                	c.lw	a5,36(s0)
2005c788:	eb81                	c.bnez	a5,2005c798 <xQueueGenericSendFromISR+0x48>
2005c78a:	4505                	c.li	a0,1
2005c78c:	40b2                	c.lwsp	ra,12(sp)
2005c78e:	4422                	c.lwsp	s0,8(sp)
2005c790:	4492                	c.lwsp	s1,4(sp)
2005c792:	4902                	c.lwsp	s2,0(sp)
2005c794:	0141                	c.addi	sp,16
2005c796:	8082                	c.jr	ra
2005c798:	02440513          	addi	a0,s0,36
2005c79c:	70d000ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005c7a0:	d56d                	c.beqz	a0,2005c78a <xQueueGenericSendFromISR+0x3a>
2005c7a2:	fe0904e3          	beq	s2,zero,2005c78a <xQueueGenericSendFromISR+0x3a>
2005c7a6:	4785                	c.li	a5,1
2005c7a8:	00f92023          	sw	a5,0(s2)
2005c7ac:	bff9                	c.j	2005c78a <xQueueGenericSendFromISR+0x3a>
2005c7ae:	0485                	c.addi	s1,1
2005c7b0:	04e2                	c.slli	s1,0x18
2005c7b2:	84e1                	c.srai	s1,0x18
2005c7b4:	049402a3          	sb	s1,69(s0)
2005c7b8:	bfc9                	c.j	2005c78a <xQueueGenericSendFromISR+0x3a>

2005c7ba <xQueueGiveFromISR>:
2005c7ba:	5d18                	c.lw	a4,56(a0)
2005c7bc:	5d5c                	c.lw	a5,60(a0)
2005c7be:	04f77663          	bgeu	a4,a5,2005c80a <xQueueGiveFromISR+0x50>
2005c7c2:	04554783          	lbu	a5,69(a0)
2005c7c6:	0705                	c.addi	a4,1
2005c7c8:	dd18                	c.sw	a4,56(a0)
2005c7ca:	07e2                	c.slli	a5,0x18
2005c7cc:	87e1                	c.srai	a5,0x18
2005c7ce:	577d                	c.li	a4,-1
2005c7d0:	02e79763          	bne	a5,a4,2005c7fe <xQueueGiveFromISR+0x44>
2005c7d4:	515c                	c.lw	a5,36(a0)
2005c7d6:	e399                	c.bnez	a5,2005c7dc <xQueueGiveFromISR+0x22>
2005c7d8:	4505                	c.li	a0,1
2005c7da:	8082                	c.jr	ra
2005c7dc:	1141                	c.addi	sp,-16
2005c7de:	02450513          	addi	a0,a0,36
2005c7e2:	c422                	c.swsp	s0,8(sp)
2005c7e4:	c606                	c.swsp	ra,12(sp)
2005c7e6:	842e                	c.mv	s0,a1
2005c7e8:	6c1000ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005c7ec:	c501                	c.beqz	a0,2005c7f4 <xQueueGiveFromISR+0x3a>
2005c7ee:	c019                	c.beqz	s0,2005c7f4 <xQueueGiveFromISR+0x3a>
2005c7f0:	4785                	c.li	a5,1
2005c7f2:	c01c                	c.sw	a5,0(s0)
2005c7f4:	40b2                	c.lwsp	ra,12(sp)
2005c7f6:	4422                	c.lwsp	s0,8(sp)
2005c7f8:	4505                	c.li	a0,1
2005c7fa:	0141                	c.addi	sp,16
2005c7fc:	8082                	c.jr	ra
2005c7fe:	0785                	c.addi	a5,1
2005c800:	07e2                	c.slli	a5,0x18
2005c802:	87e1                	c.srai	a5,0x18
2005c804:	04f502a3          	sb	a5,69(a0)
2005c808:	bfc1                	c.j	2005c7d8 <xQueueGiveFromISR+0x1e>
2005c80a:	4501                	c.li	a0,0
2005c80c:	8082                	c.jr	ra

2005c80e <xQueueReceive>:
2005c80e:	7139                	c.addi16sp	sp,-64
2005c810:	dc22                	c.swsp	s0,56(sp)
2005c812:	d84a                	c.swsp	s2,48(sp)
2005c814:	d64e                	c.swsp	s3,44(sp)
2005c816:	d452                	c.swsp	s4,40(sp)
2005c818:	d256                	c.swsp	s5,36(sp)
2005c81a:	de06                	c.swsp	ra,60(sp)
2005c81c:	da26                	c.swsp	s1,52(sp)
2005c81e:	842a                	c.mv	s0,a0
2005c820:	892e                	c.mv	s2,a1
2005c822:	c632                	c.swsp	a2,12(sp)
2005c824:	4a01                	c.li	s4,0
2005c826:	59fd                	c.li	s3,-1
2005c828:	02450a93          	addi	s5,a0,36
2005c82c:	12c010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c830:	5c04                	c.lw	s1,56(s0)
2005c832:	cc85                	c.beqz	s1,2005c86a <xQueueReceive+0x5c>
2005c834:	85ca                	c.mv	a1,s2
2005c836:	8522                	c.mv	a0,s0
2005c838:	14fd                	c.addi	s1,-1
2005c83a:	d01ff0ef          	jal	ra,2005c53a <prvCopyDataFromQueue>
2005c83e:	dc04                	c.sw	s1,56(s0)
2005c840:	481c                	c.lw	a5,16(s0)
2005c842:	cb81                	c.beqz	a5,2005c852 <xQueueReceive+0x44>
2005c844:	01040513          	addi	a0,s0,16
2005c848:	661000ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005c84c:	c119                	c.beqz	a0,2005c852 <xQueueReceive+0x44>
2005c84e:	00000073          	ecall
2005c852:	120010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c856:	4505                	c.li	a0,1
2005c858:	50f2                	c.lwsp	ra,60(sp)
2005c85a:	5462                	c.lwsp	s0,56(sp)
2005c85c:	54d2                	c.lwsp	s1,52(sp)
2005c85e:	5942                	c.lwsp	s2,48(sp)
2005c860:	59b2                	c.lwsp	s3,44(sp)
2005c862:	5a22                	c.lwsp	s4,40(sp)
2005c864:	5a92                	c.lwsp	s5,36(sp)
2005c866:	6121                	c.addi16sp	sp,64
2005c868:	8082                	c.jr	ra
2005c86a:	47b2                	c.lwsp	a5,12(sp)
2005c86c:	e789                	c.bnez	a5,2005c876 <xQueueReceive+0x68>
2005c86e:	104010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c872:	4501                	c.li	a0,0
2005c874:	b7d5                	c.j	2005c858 <xQueueReceive+0x4a>
2005c876:	000a1563          	bne	s4,zero,2005c880 <xQueueReceive+0x72>
2005c87a:	0828                	c.addi4spn	a0,sp,24
2005c87c:	6af000ef          	jal	ra,2005d72a <vTaskInternalSetTimeOutState>
2005c880:	0f2010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c884:	3af000ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005c888:	0d0010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c88c:	04444783          	lbu	a5,68(s0)
2005c890:	07e2                	c.slli	a5,0x18
2005c892:	87e1                	c.srai	a5,0x18
2005c894:	01379463          	bne	a5,s3,2005c89c <xQueueReceive+0x8e>
2005c898:	04040223          	sb	zero,68(s0)
2005c89c:	04544783          	lbu	a5,69(s0)
2005c8a0:	07e2                	c.slli	a5,0x18
2005c8a2:	87e1                	c.srai	a5,0x18
2005c8a4:	01379463          	bne	a5,s3,2005c8ac <xQueueReceive+0x9e>
2005c8a8:	040402a3          	sb	zero,69(s0)
2005c8ac:	0c6010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c8b0:	006c                	c.addi4spn	a1,sp,12
2005c8b2:	0828                	c.addi4spn	a0,sp,24
2005c8b4:	7d6010ef          	jal	ra,2005e08a <xTaskCheckForTimeOut>
2005c8b8:	e90d                	c.bnez	a0,2005c8ea <xQueueReceive+0xdc>
2005c8ba:	8522                	c.mv	a0,s0
2005c8bc:	bcfff0ef          	jal	ra,2005c48a <prvIsQueueEmpty>
2005c8c0:	cd19                	c.beqz	a0,2005c8de <xQueueReceive+0xd0>
2005c8c2:	45b2                	c.lwsp	a1,12(sp)
2005c8c4:	8556                	c.mv	a0,s5
2005c8c6:	599000ef          	jal	ra,2005d65e <vTaskPlaceOnEventList>
2005c8ca:	8522                	c.mv	a0,s0
2005c8cc:	c93ff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c8d0:	410010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c8d4:	e119                	c.bnez	a0,2005c8da <xQueueReceive+0xcc>
2005c8d6:	00000073          	ecall
2005c8da:	4a05                	c.li	s4,1
2005c8dc:	bf81                	c.j	2005c82c <xQueueReceive+0x1e>
2005c8de:	8522                	c.mv	a0,s0
2005c8e0:	c7fff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c8e4:	3fc010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c8e8:	bfcd                	c.j	2005c8da <xQueueReceive+0xcc>
2005c8ea:	8522                	c.mv	a0,s0
2005c8ec:	c73ff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c8f0:	3f0010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c8f4:	8522                	c.mv	a0,s0
2005c8f6:	b95ff0ef          	jal	ra,2005c48a <prvIsQueueEmpty>
2005c8fa:	d165                	c.beqz	a0,2005c8da <xQueueReceive+0xcc>
2005c8fc:	bf9d                	c.j	2005c872 <xQueueReceive+0x64>

2005c8fe <xQueueSemaphoreTake>:
2005c8fe:	7139                	c.addi16sp	sp,-64
2005c900:	dc22                	c.swsp	s0,56(sp)
2005c902:	da26                	c.swsp	s1,52(sp)
2005c904:	d84a                	c.swsp	s2,48(sp)
2005c906:	d64e                	c.swsp	s3,44(sp)
2005c908:	d452                	c.swsp	s4,40(sp)
2005c90a:	de06                	c.swsp	ra,60(sp)
2005c90c:	842a                	c.mv	s0,a0
2005c90e:	c62e                	c.swsp	a1,12(sp)
2005c910:	4481                	c.li	s1,0
2005c912:	4981                	c.li	s3,0
2005c914:	597d                	c.li	s2,-1
2005c916:	02450a13          	addi	s4,a0,36
2005c91a:	03e010ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c91e:	5c1c                	c.lw	a5,56(s0)
2005c920:	cf8d                	c.beqz	a5,2005c95a <xQueueSemaphoreTake+0x5c>
2005c922:	17fd                	c.addi	a5,-1
2005c924:	dc1c                	c.sw	a5,56(s0)
2005c926:	401c                	c.lw	a5,0(s0)
2005c928:	e781                	c.bnez	a5,2005c930 <xQueueSemaphoreTake+0x32>
2005c92a:	083010ef          	jal	ra,2005e1ac <pvTaskIncrementMutexHeldCount>
2005c92e:	c408                	c.sw	a0,8(s0)
2005c930:	481c                	c.lw	a5,16(s0)
2005c932:	cb81                	c.beqz	a5,2005c942 <xQueueSemaphoreTake+0x44>
2005c934:	01040513          	addi	a0,s0,16
2005c938:	571000ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005c93c:	c119                	c.beqz	a0,2005c942 <xQueueSemaphoreTake+0x44>
2005c93e:	00000073          	ecall
2005c942:	030010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c946:	4485                	c.li	s1,1
2005c948:	50f2                	c.lwsp	ra,60(sp)
2005c94a:	5462                	c.lwsp	s0,56(sp)
2005c94c:	5942                	c.lwsp	s2,48(sp)
2005c94e:	59b2                	c.lwsp	s3,44(sp)
2005c950:	5a22                	c.lwsp	s4,40(sp)
2005c952:	8526                	c.mv	a0,s1
2005c954:	54d2                	c.lwsp	s1,52(sp)
2005c956:	6121                	c.addi16sp	sp,64
2005c958:	8082                	c.jr	ra
2005c95a:	47b2                	c.lwsp	a5,12(sp)
2005c95c:	e789                	c.bnez	a5,2005c966 <xQueueSemaphoreTake+0x68>
2005c95e:	014010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c962:	4481                	c.li	s1,0
2005c964:	b7d5                	c.j	2005c948 <xQueueSemaphoreTake+0x4a>
2005c966:	00099563          	bne	s3,zero,2005c970 <xQueueSemaphoreTake+0x72>
2005c96a:	0828                	c.addi4spn	a0,sp,24
2005c96c:	5bf000ef          	jal	ra,2005d72a <vTaskInternalSetTimeOutState>
2005c970:	002010ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c974:	2bf000ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005c978:	7e1000ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c97c:	04444783          	lbu	a5,68(s0)
2005c980:	07e2                	c.slli	a5,0x18
2005c982:	87e1                	c.srai	a5,0x18
2005c984:	01279463          	bne	a5,s2,2005c98c <xQueueSemaphoreTake+0x8e>
2005c988:	04040223          	sb	zero,68(s0)
2005c98c:	04544783          	lbu	a5,69(s0)
2005c990:	07e2                	c.slli	a5,0x18
2005c992:	87e1                	c.srai	a5,0x18
2005c994:	01279463          	bne	a5,s2,2005c99c <xQueueSemaphoreTake+0x9e>
2005c998:	040402a3          	sb	zero,69(s0)
2005c99c:	7d7000ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c9a0:	006c                	c.addi4spn	a1,sp,12
2005c9a2:	0828                	c.addi4spn	a0,sp,24
2005c9a4:	6e6010ef          	jal	ra,2005e08a <xTaskCheckForTimeOut>
2005c9a8:	e139                	c.bnez	a0,2005c9ee <xQueueSemaphoreTake+0xf0>
2005c9aa:	8522                	c.mv	a0,s0
2005c9ac:	adfff0ef          	jal	ra,2005c48a <prvIsQueueEmpty>
2005c9b0:	c90d                	c.beqz	a0,2005c9e2 <xQueueSemaphoreTake+0xe4>
2005c9b2:	401c                	c.lw	a5,0(s0)
2005c9b4:	eb89                	c.bnez	a5,2005c9c6 <xQueueSemaphoreTake+0xc8>
2005c9b6:	7a3000ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005c9ba:	4408                	c.lw	a0,8(s0)
2005c9bc:	5cf000ef          	jal	ra,2005d78a <xTaskPriorityInherit>
2005c9c0:	84aa                	c.mv	s1,a0
2005c9c2:	7b1000ef          	jal	ra,2005d972 <vTaskExitCritical>
2005c9c6:	45b2                	c.lwsp	a1,12(sp)
2005c9c8:	8552                	c.mv	a0,s4
2005c9ca:	495000ef          	jal	ra,2005d65e <vTaskPlaceOnEventList>
2005c9ce:	8522                	c.mv	a0,s0
2005c9d0:	b8fff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c9d4:	30c010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c9d8:	e119                	c.bnez	a0,2005c9de <xQueueSemaphoreTake+0xe0>
2005c9da:	00000073          	ecall
2005c9de:	4985                	c.li	s3,1
2005c9e0:	bf2d                	c.j	2005c91a <xQueueSemaphoreTake+0x1c>
2005c9e2:	8522                	c.mv	a0,s0
2005c9e4:	b7bff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c9e8:	2f8010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c9ec:	bfcd                	c.j	2005c9de <xQueueSemaphoreTake+0xe0>
2005c9ee:	8522                	c.mv	a0,s0
2005c9f0:	b6fff0ef          	jal	ra,2005c55e <prvUnlockQueue>
2005c9f4:	2ec010ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005c9f8:	8522                	c.mv	a0,s0
2005c9fa:	a91ff0ef          	jal	ra,2005c48a <prvIsQueueEmpty>
2005c9fe:	d165                	c.beqz	a0,2005c9de <xQueueSemaphoreTake+0xe0>
2005ca00:	f40484e3          	beq	s1,zero,2005c948 <xQueueSemaphoreTake+0x4a>
2005ca04:	755000ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005ca08:	504c                	c.lw	a1,36(s0)
2005ca0a:	c589                	c.beqz	a1,2005ca14 <xQueueSemaphoreTake+0x116>
2005ca0c:	581c                	c.lw	a5,48(s0)
2005ca0e:	45ad                	c.li	a1,11
2005ca10:	439c                	c.lw	a5,0(a5)
2005ca12:	8d9d                	c.sub	a1,a5
2005ca14:	4408                	c.lw	a0,8(s0)
2005ca16:	6a5000ef          	jal	ra,2005d8ba <vTaskPriorityDisinheritAfterTimeout>
2005ca1a:	b791                	c.j	2005c95e <xQueueSemaphoreTake+0x60>

2005ca1c <xQueueReceiveFromISR>:
2005ca1c:	1101                	c.addi	sp,-32
2005ca1e:	ce06                	c.swsp	ra,28(sp)
2005ca20:	cc22                	c.swsp	s0,24(sp)
2005ca22:	ca26                	c.swsp	s1,20(sp)
2005ca24:	c84a                	c.swsp	s2,16(sp)
2005ca26:	c64e                	c.swsp	s3,12(sp)
2005ca28:	03852983          	lw	s3,56(a0)
2005ca2c:	04098b63          	beq	s3,zero,2005ca82 <xQueueReceiveFromISR+0x66>
2005ca30:	04454483          	lbu	s1,68(a0)
2005ca34:	842a                	c.mv	s0,a0
2005ca36:	8932                	c.mv	s2,a2
2005ca38:	04e2                	c.slli	s1,0x18
2005ca3a:	b01ff0ef          	jal	ra,2005c53a <prvCopyDataFromQueue>
2005ca3e:	19fd                	c.addi	s3,-1
2005ca40:	84e1                	c.srai	s1,0x18
2005ca42:	03342c23          	sw	s3,56(s0)
2005ca46:	57fd                	c.li	a5,-1
2005ca48:	02f49763          	bne	s1,a5,2005ca76 <xQueueReceiveFromISR+0x5a>
2005ca4c:	481c                	c.lw	a5,16(s0)
2005ca4e:	eb89                	c.bnez	a5,2005ca60 <xQueueReceiveFromISR+0x44>
2005ca50:	4505                	c.li	a0,1
2005ca52:	40f2                	c.lwsp	ra,28(sp)
2005ca54:	4462                	c.lwsp	s0,24(sp)
2005ca56:	44d2                	c.lwsp	s1,20(sp)
2005ca58:	4942                	c.lwsp	s2,16(sp)
2005ca5a:	49b2                	c.lwsp	s3,12(sp)
2005ca5c:	6105                	c.addi16sp	sp,32
2005ca5e:	8082                	c.jr	ra
2005ca60:	01040513          	addi	a0,s0,16
2005ca64:	445000ef          	jal	ra,2005d6a8 <xTaskRemoveFromEventList>
2005ca68:	d565                	c.beqz	a0,2005ca50 <xQueueReceiveFromISR+0x34>
2005ca6a:	fe0903e3          	beq	s2,zero,2005ca50 <xQueueReceiveFromISR+0x34>
2005ca6e:	4785                	c.li	a5,1
2005ca70:	00f92023          	sw	a5,0(s2)
2005ca74:	bff1                	c.j	2005ca50 <xQueueReceiveFromISR+0x34>
2005ca76:	0485                	c.addi	s1,1
2005ca78:	04e2                	c.slli	s1,0x18
2005ca7a:	84e1                	c.srai	s1,0x18
2005ca7c:	04940223          	sb	s1,68(s0)
2005ca80:	bfc1                	c.j	2005ca50 <xQueueReceiveFromISR+0x34>
2005ca82:	4501                	c.li	a0,0
2005ca84:	b7f9                	c.j	2005ca52 <xQueueReceiveFromISR+0x36>

2005ca86 <vQueueWaitForMessageRestricted>:
2005ca86:	1141                	c.addi	sp,-16
2005ca88:	c422                	c.swsp	s0,8(sp)
2005ca8a:	c226                	c.swsp	s1,4(sp)
2005ca8c:	c04a                	c.swsp	s2,0(sp)
2005ca8e:	842a                	c.mv	s0,a0
2005ca90:	c606                	c.swsp	ra,12(sp)
2005ca92:	84ae                	c.mv	s1,a1
2005ca94:	8932                	c.mv	s2,a2
2005ca96:	6c3000ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005ca9a:	04444783          	lbu	a5,68(s0)
2005ca9e:	577d                	c.li	a4,-1
2005caa0:	07e2                	c.slli	a5,0x18
2005caa2:	87e1                	c.srai	a5,0x18
2005caa4:	00e79463          	bne	a5,a4,2005caac <vQueueWaitForMessageRestricted+0x26>
2005caa8:	04040223          	sb	zero,68(s0)
2005caac:	04544783          	lbu	a5,69(s0)
2005cab0:	577d                	c.li	a4,-1
2005cab2:	07e2                	c.slli	a5,0x18
2005cab4:	87e1                	c.srai	a5,0x18
2005cab6:	00e79463          	bne	a5,a4,2005cabe <vQueueWaitForMessageRestricted+0x38>
2005caba:	040402a3          	sb	zero,69(s0)
2005cabe:	6b5000ef          	jal	ra,2005d972 <vTaskExitCritical>
2005cac2:	5c1c                	c.lw	a5,56(s0)
2005cac4:	e799                	c.bnez	a5,2005cad2 <vQueueWaitForMessageRestricted+0x4c>
2005cac6:	864a                	c.mv	a2,s2
2005cac8:	85a6                	c.mv	a1,s1
2005caca:	02440513          	addi	a0,s0,36
2005cace:	3b1000ef          	jal	ra,2005d67e <vTaskPlaceOnEventListRestricted>
2005cad2:	8522                	c.mv	a0,s0
2005cad4:	4422                	c.lwsp	s0,8(sp)
2005cad6:	40b2                	c.lwsp	ra,12(sp)
2005cad8:	4492                	c.lwsp	s1,4(sp)
2005cada:	4902                	c.lwsp	s2,0(sp)
2005cadc:	0141                	c.addi	sp,16
2005cade:	a81ff06f          	jal	zero,2005c55e <prvUnlockQueue>

2005cae2 <QueueCreateTrace>:
2005cae2:	f5c1a603          	lw	a2,-164(gp) # 20066034 <Inited.1>
2005cae6:	f5c18693          	addi	a3,gp,-164 # 20066034 <Inited.1>
2005caea:	200687b7          	lui	a5,0x20068
2005caee:	9c878713          	addi	a4,a5,-1592 # 200679c8 <QueueTraceList>
2005caf2:	ee01                	c.bnez	a2,2005cb0a <QueueCreateTrace+0x28>
2005caf4:	9c878793          	addi	a5,a5,-1592
2005caf8:	04070613          	addi	a2,a4,64
2005cafc:	0007a023          	sw	zero,0(a5)
2005cb00:	0791                	c.addi	a5,4
2005cb02:	fec79de3          	bne	a5,a2,2005cafc <QueueCreateTrace+0x1a>
2005cb06:	4785                	c.li	a5,1
2005cb08:	c29c                	c.sw	a5,0(a3)
2005cb0a:	86ba                	c.mv	a3,a4
2005cb0c:	4781                	c.li	a5,0
2005cb0e:	4641                	c.li	a2,16
2005cb10:	428c                	c.lw	a1,0(a3)
2005cb12:	e589                	c.bnez	a1,2005cb1c <QueueCreateTrace+0x3a>
2005cb14:	078a                	c.slli	a5,0x2
2005cb16:	97ba                	c.add	a5,a4
2005cb18:	c388                	c.sw	a0,0(a5)
2005cb1a:	8082                	c.jr	ra
2005cb1c:	0785                	c.addi	a5,1
2005cb1e:	0691                	c.addi	a3,4
2005cb20:	fec798e3          	bne	a5,a2,2005cb10 <QueueCreateTrace+0x2e>
2005cb24:	8082                	c.jr	ra

2005cb26 <xQueueGenericCreate>:
2005cb26:	1101                	c.addi	sp,-32
2005cb28:	c84a                	c.swsp	s2,16(sp)
2005cb2a:	892a                	c.mv	s2,a0
2005cb2c:	02b50533          	mul	a0,a0,a1
2005cb30:	cc22                	c.swsp	s0,24(sp)
2005cb32:	ca26                	c.swsp	s1,20(sp)
2005cb34:	c64e                	c.swsp	s3,12(sp)
2005cb36:	ce06                	c.swsp	ra,28(sp)
2005cb38:	84ae                	c.mv	s1,a1
2005cb3a:	89b2                	c.mv	s3,a2
2005cb3c:	05050513          	addi	a0,a0,80
2005cb40:	dc5fd0ef          	jal	ra,2005a904 <pvPortMalloc>
2005cb44:	842a                	c.mv	s0,a0
2005cb46:	c505                	c.beqz	a0,2005cb6e <xQueueGenericCreate+0x48>
2005cb48:	04050323          	sb	zero,70(a0)
2005cb4c:	87aa                	c.mv	a5,a0
2005cb4e:	c099                	c.beqz	s1,2005cb54 <xQueueGenericCreate+0x2e>
2005cb50:	05050793          	addi	a5,a0,80
2005cb54:	c01c                	c.sw	a5,0(s0)
2005cb56:	03242e23          	sw	s2,60(s0)
2005cb5a:	c024                	c.sw	s1,64(s0)
2005cb5c:	4585                	c.li	a1,1
2005cb5e:	8522                	c.mv	a0,s0
2005cb60:	a81ff0ef          	jal	ra,2005c5e0 <xQueueGenericReset>
2005cb64:	8522                	c.mv	a0,s0
2005cb66:	05340623          	sb	s3,76(s0)
2005cb6a:	f79ff0ef          	jal	ra,2005cae2 <QueueCreateTrace>
2005cb6e:	40f2                	c.lwsp	ra,28(sp)
2005cb70:	8522                	c.mv	a0,s0
2005cb72:	4462                	c.lwsp	s0,24(sp)
2005cb74:	44d2                	c.lwsp	s1,20(sp)
2005cb76:	4942                	c.lwsp	s2,16(sp)
2005cb78:	49b2                	c.lwsp	s3,12(sp)
2005cb7a:	6105                	c.addi16sp	sp,32
2005cb7c:	8082                	c.jr	ra

2005cb7e <xQueueCreateMutex>:
2005cb7e:	1141                	c.addi	sp,-16
2005cb80:	862a                	c.mv	a2,a0
2005cb82:	4581                	c.li	a1,0
2005cb84:	4505                	c.li	a0,1
2005cb86:	c422                	c.swsp	s0,8(sp)
2005cb88:	c606                	c.swsp	ra,12(sp)
2005cb8a:	f9dff0ef          	jal	ra,2005cb26 <xQueueGenericCreate>
2005cb8e:	842a                	c.mv	s0,a0
2005cb90:	cd01                	c.beqz	a0,2005cba8 <xQueueCreateMutex+0x2a>
2005cb92:	00052423          	sw	zero,8(a0)
2005cb96:	00052023          	sw	zero,0(a0)
2005cb9a:	00052623          	sw	zero,12(a0)
2005cb9e:	4681                	c.li	a3,0
2005cba0:	4601                	c.li	a2,0
2005cba2:	4581                	c.li	a1,0
2005cba4:	aa9ff0ef          	jal	ra,2005c64c <xQueueGenericSend>
2005cba8:	40b2                	c.lwsp	ra,12(sp)
2005cbaa:	8522                	c.mv	a0,s0
2005cbac:	4422                	c.lwsp	s0,8(sp)
2005cbae:	0141                	c.addi	sp,16
2005cbb0:	8082                	c.jr	ra

2005cbb2 <xQueueCreateCountingSemaphore>:
2005cbb2:	1141                	c.addi	sp,-16
2005cbb4:	c422                	c.swsp	s0,8(sp)
2005cbb6:	4609                	c.li	a2,2
2005cbb8:	842e                	c.mv	s0,a1
2005cbba:	4581                	c.li	a1,0
2005cbbc:	c606                	c.swsp	ra,12(sp)
2005cbbe:	f69ff0ef          	jal	ra,2005cb26 <xQueueGenericCreate>
2005cbc2:	c111                	c.beqz	a0,2005cbc6 <xQueueCreateCountingSemaphore+0x14>
2005cbc4:	dd00                	c.sw	s0,56(a0)
2005cbc6:	40b2                	c.lwsp	ra,12(sp)
2005cbc8:	4422                	c.lwsp	s0,8(sp)
2005cbca:	0141                	c.addi	sp,16
2005cbcc:	8082                	c.jr	ra

2005cbce <xQueueGenericCreateStatic>:
2005cbce:	1141                	c.addi	sp,-16
2005cbd0:	c422                	c.swsp	s0,8(sp)
2005cbd2:	c606                	c.swsp	ra,12(sp)
2005cbd4:	c226                	c.swsp	s1,4(sp)
2005cbd6:	8436                	c.mv	s0,a3
2005cbd8:	c29d                	c.beqz	a3,2005cbfe <xQueueGenericCreateStatic+0x30>
2005cbda:	4785                	c.li	a5,1
2005cbdc:	04f68323          	sb	a5,70(a3) # 8000046 <__ap_sram_heap_size+0x7fc0046>
2005cbe0:	84ba                	c.mv	s1,a4
2005cbe2:	e191                	c.bnez	a1,2005cbe6 <xQueueGenericCreateStatic+0x18>
2005cbe4:	8636                	c.mv	a2,a3
2005cbe6:	dc48                	c.sw	a0,60(s0)
2005cbe8:	c02c                	c.sw	a1,64(s0)
2005cbea:	c010                	c.sw	a2,0(s0)
2005cbec:	4585                	c.li	a1,1
2005cbee:	8522                	c.mv	a0,s0
2005cbf0:	9f1ff0ef          	jal	ra,2005c5e0 <xQueueGenericReset>
2005cbf4:	8522                	c.mv	a0,s0
2005cbf6:	04940623          	sb	s1,76(s0)
2005cbfa:	ee9ff0ef          	jal	ra,2005cae2 <QueueCreateTrace>
2005cbfe:	40b2                	c.lwsp	ra,12(sp)
2005cc00:	8522                	c.mv	a0,s0
2005cc02:	4422                	c.lwsp	s0,8(sp)
2005cc04:	4492                	c.lwsp	s1,4(sp)
2005cc06:	0141                	c.addi	sp,16
2005cc08:	8082                	c.jr	ra

2005cc0a <xQueueCreateMutexStatic>:
2005cc0a:	1141                	c.addi	sp,-16
2005cc0c:	872a                	c.mv	a4,a0
2005cc0e:	86ae                	c.mv	a3,a1
2005cc10:	4601                	c.li	a2,0
2005cc12:	4581                	c.li	a1,0
2005cc14:	4505                	c.li	a0,1
2005cc16:	c422                	c.swsp	s0,8(sp)
2005cc18:	c606                	c.swsp	ra,12(sp)
2005cc1a:	fb5ff0ef          	jal	ra,2005cbce <xQueueGenericCreateStatic>
2005cc1e:	842a                	c.mv	s0,a0
2005cc20:	cd01                	c.beqz	a0,2005cc38 <xQueueCreateMutexStatic+0x2e>
2005cc22:	00052423          	sw	zero,8(a0)
2005cc26:	00052023          	sw	zero,0(a0)
2005cc2a:	00052623          	sw	zero,12(a0)
2005cc2e:	4681                	c.li	a3,0
2005cc30:	4601                	c.li	a2,0
2005cc32:	4581                	c.li	a1,0
2005cc34:	a19ff0ef          	jal	ra,2005c64c <xQueueGenericSend>
2005cc38:	40b2                	c.lwsp	ra,12(sp)
2005cc3a:	8522                	c.mv	a0,s0
2005cc3c:	4422                	c.lwsp	s0,8(sp)
2005cc3e:	0141                	c.addi	sp,16
2005cc40:	8082                	c.jr	ra

2005cc42 <xQueueCreateCountingSemaphoreStatic>:
2005cc42:	1141                	c.addi	sp,-16
2005cc44:	c422                	c.swsp	s0,8(sp)
2005cc46:	86b2                	c.mv	a3,a2
2005cc48:	842e                	c.mv	s0,a1
2005cc4a:	4709                	c.li	a4,2
2005cc4c:	4601                	c.li	a2,0
2005cc4e:	4581                	c.li	a1,0
2005cc50:	c606                	c.swsp	ra,12(sp)
2005cc52:	f7dff0ef          	jal	ra,2005cbce <xQueueGenericCreateStatic>
2005cc56:	c111                	c.beqz	a0,2005cc5a <xQueueCreateCountingSemaphoreStatic+0x18>
2005cc58:	dd00                	c.sw	s0,56(a0)
2005cc5a:	40b2                	c.lwsp	ra,12(sp)
2005cc5c:	4422                	c.lwsp	s0,8(sp)
2005cc5e:	0141                	c.addi	sp,16
2005cc60:	8082                	c.jr	ra

2005cc62 <QueueDeleteTrace>:
2005cc62:	20068737          	lui	a4,0x20068
2005cc66:	9c870693          	addi	a3,a4,-1592 # 200679c8 <QueueTraceList>
2005cc6a:	4781                	c.li	a5,0
2005cc6c:	9c870713          	addi	a4,a4,-1592
2005cc70:	4641                	c.li	a2,16
2005cc72:	428c                	c.lw	a1,0(a3)
2005cc74:	00a59763          	bne	a1,a0,2005cc82 <QueueDeleteTrace+0x20>
2005cc78:	078a                	c.slli	a5,0x2
2005cc7a:	97ba                	c.add	a5,a4
2005cc7c:	0007a023          	sw	zero,0(a5)
2005cc80:	8082                	c.jr	ra
2005cc82:	0785                	c.addi	a5,1
2005cc84:	0691                	c.addi	a3,4
2005cc86:	fec796e3          	bne	a5,a2,2005cc72 <QueueDeleteTrace+0x10>
2005cc8a:	8082                	c.jr	ra

2005cc8c <vQueueDelete>:
2005cc8c:	1141                	c.addi	sp,-16
2005cc8e:	c422                	c.swsp	s0,8(sp)
2005cc90:	c606                	c.swsp	ra,12(sp)
2005cc92:	842a                	c.mv	s0,a0
2005cc94:	fcfff0ef          	jal	ra,2005cc62 <QueueDeleteTrace>
2005cc98:	04644783          	lbu	a5,70(s0)
2005cc9c:	e799                	c.bnez	a5,2005ccaa <vQueueDelete+0x1e>
2005cc9e:	8522                	c.mv	a0,s0
2005cca0:	4422                	c.lwsp	s0,8(sp)
2005cca2:	40b2                	c.lwsp	ra,12(sp)
2005cca4:	0141                	c.addi	sp,16
2005cca6:	d3dfd06f          	jal	zero,2005a9e2 <vPortFree>
2005ccaa:	40b2                	c.lwsp	ra,12(sp)
2005ccac:	4422                	c.lwsp	s0,8(sp)
2005ccae:	0141                	c.addi	sp,16
2005ccb0:	8082                	c.jr	ra

2005ccb2 <shell_loguratRx_ipc_int>:
2005ccb2:	1141                	c.addi	sp,-16
2005ccb4:	458d                	c.li	a1,3
2005ccb6:	4541                	c.li	a0,16
2005ccb8:	c422                	c.swsp	s0,8(sp)
2005ccba:	c606                	c.swsp	ra,12(sp)
2005ccbc:	20001437          	lui	s0,0x20001
2005ccc0:	c30fc0ef          	jal	ra,200590f0 <ipc_get_message>
2005ccc4:	87aa                	c.mv	a5,a0
2005ccc6:	b4440713          	addi	a4,s0,-1212 # 20000b44 <shell_ctl>
2005ccca:	4708                	c.lw	a0,8(a4)
2005cccc:	43cc                	c.lw	a1,4(a5)
2005ccce:	7c1027f3          	csrrs	a5,0x7c1,zero
2005ccd2:	1007f793          	andi	a5,a5,256
2005ccd6:	b4440413          	addi	s0,s0,-1212
2005ccda:	c3b9                	c.beqz	a5,2005cd20 <shell_loguratRx_ipc_int+0x6e>
2005ccdc:	01f5f713          	andi	a4,a1,31
2005cce0:	c325                	c.beqz	a4,2005cd40 <shell_loguratRx_ipc_int+0x8e>
2005cce2:	08070693          	addi	a3,a4,128
2005cce6:	fe05f793          	andi	a5,a1,-32
2005ccea:	01f7f713          	andi	a4,a5,31
2005ccee:	9736                	c.add	a4,a3
2005ccf0:	0ff0000f          	fence	iorw,iorw
2005ccf4:	973e                	c.add	a4,a5
2005ccf6:	00f7f00b          	cache	dinv,(a5)
2005ccfa:	02078793          	addi	a5,a5,32
2005ccfe:	40f706b3          	sub	a3,a4,a5
2005cd02:	fed04ae3          	blt	zero,a3,2005ccf6 <shell_loguratRx_ipc_int+0x44>
2005cd06:	0ff0000f          	fence	iorw,iorw
2005cd0a:	0001                	c.addi	zero,0
2005cd0c:	0001                	c.addi	zero,0
2005cd0e:	0001                	c.addi	zero,0
2005cd10:	0001                	c.addi	zero,0
2005cd12:	0001                	c.addi	zero,0
2005cd14:	0001                	c.addi	zero,0
2005cd16:	0001                	c.addi	zero,0
2005cd18:	0001                	c.addi	zero,0
2005cd1a:	0001                	c.addi	zero,0
2005cd1c:	0001                	c.addi	zero,0
2005cd1e:	0001                	c.addi	zero,0
2005cd20:	08000613          	addi	a2,zero,128
2005cd24:	dffad097          	auipc	ra,0xdffad
2005cd28:	8ee080e7          	jalr	ra,-1810(ra) # 9612 <_memcpy>
2005cd2c:	4785                	c.li	a5,1
2005cd2e:	00f40223          	sb	a5,4(s0)
2005cd32:	501c                	c.lw	a5,32(s0)
2005cd34:	cb91                	c.beqz	a5,2005cd48 <shell_loguratRx_ipc_int+0x96>
2005cd36:	4c5c                	c.lw	a5,28(s0)
2005cd38:	4422                	c.lwsp	s0,8(sp)
2005cd3a:	40b2                	c.lwsp	ra,12(sp)
2005cd3c:	0141                	c.addi	sp,16
2005cd3e:	8782                	c.jr	a5
2005cd40:	87ae                	c.mv	a5,a1
2005cd42:	08000693          	addi	a3,zero,128
2005cd46:	b755                	c.j	2005ccea <shell_loguratRx_ipc_int+0x38>
2005cd48:	40b2                	c.lwsp	ra,12(sp)
2005cd4a:	4422                	c.lwsp	s0,8(sp)
2005cd4c:	0141                	c.addi	sp,16
2005cd4e:	8082                	c.jr	ra

2005cd50 <shell_give_sema>:
2005cd50:	200017b7          	lui	a5,0x20001
2005cd54:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005cd58:	539c                	c.lw	a5,32(a5)
2005cd5a:	c789                	c.beqz	a5,2005cd64 <shell_give_sema+0x14>
2005cd5c:	f601a503          	lw	a0,-160(gp) # 20066038 <shell_sema>
2005cd60:	b5bfe06f          	jal	zero,2005b8ba <rtos_sema_give>
2005cd64:	8082                	c.jr	ra

2005cd66 <shell_loguartRx_dispatch>:
2005cd66:	1141                	c.addi	sp,-16
2005cd68:	c422                	c.swsp	s0,8(sp)
2005cd6a:	20001437          	lui	s0,0x20001
2005cd6e:	b4440413          	addi	s0,s0,-1212 # 20000b44 <shell_ctl>
2005cd72:	c04a                	c.swsp	s2,0(sp)
2005cd74:	00842903          	lw	s2,8(s0)
2005cd78:	200635b7          	lui	a1,0x20063
2005cd7c:	d3c58593          	addi	a1,a1,-708 # 20062d3c <__FUNCTION__.5+0x20>
2005cd80:	00190513          	addi	a0,s2,1
2005cd84:	c606                	c.swsp	ra,12(sp)
2005cd86:	c226                	c.swsp	s1,4(sp)
2005cd88:	dffad097          	auipc	ra,0xdffad
2005cd8c:	b10080e7          	jalr	ra,-1264(ra) # 9898 <_stricmp>
2005cd90:	ed15                	c.bnez	a0,2005cdcc <shell_loguartRx_dispatch+0x66>
2005cd92:	40000513          	addi	a0,zero,1024
2005cd96:	9abfe0ef          	jal	ra,2005b740 <rtos_mem_malloc>
2005cd9a:	3ff00593          	addi	a1,zero,1023
2005cd9e:	84aa                	c.mv	s1,a0
2005cda0:	25d010ef          	jal	ra,2005e7fc <ChipInfo_GetLibVersion_ToBuf>
2005cda4:	20062537          	lui	a0,0x20062
2005cda8:	85a6                	c.mv	a1,s1
2005cdaa:	18450513          	addi	a0,a0,388 # 20062184 <pmap_func+0x4dc>
2005cdae:	2a0020ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005cdb2:	8526                	c.mv	a0,s1
2005cdb4:	9b7fe0ef          	jal	ra,2005b76a <rtos_mem_free>
2005cdb8:	4601                	c.li	a2,0
2005cdba:	08000593          	addi	a1,zero,128
2005cdbe:	854a                	c.mv	a0,s2
2005cdc0:	dffac097          	auipc	ra,0xdffac
2005cdc4:	0c2080e7          	jalr	ra,194(ra) # 8e82 <shell_array_init>
2005cdc8:	00040223          	sb	zero,4(s0)
2005cdcc:	40b2                	c.lwsp	ra,12(sp)
2005cdce:	4422                	c.lwsp	s0,8(sp)
2005cdd0:	4492                	c.lwsp	s1,4(sp)
2005cdd2:	4902                	c.lwsp	s2,0(sp)
2005cdd4:	0141                	c.addi	sp,16
2005cdd6:	8082                	c.jr	ra

2005cdd8 <shell_task_ram>:
2005cdd8:	715d                	c.addi16sp	sp,-80
2005cdda:	c4a2                	c.swsp	s0,72(sp)
2005cddc:	20001437          	lui	s0,0x20001
2005cde0:	dc52                	c.swsp	s4,56(sp)
2005cde2:	d85a                	c.swsp	s6,48(sp)
2005cde4:	d65e                	c.swsp	s7,44(sp)
2005cde6:	d462                	c.swsp	s8,40(sp)
2005cde8:	b4440793          	addi	a5,s0,-1212 # 20000b44 <shell_ctl>
2005cdec:	c686                	c.swsp	ra,76(sp)
2005cdee:	c2a6                	c.swsp	s1,68(sp)
2005cdf0:	c0ca                	c.swsp	s2,64(sp)
2005cdf2:	de4e                	c.swsp	s3,60(sp)
2005cdf4:	da56                	c.swsp	s5,52(sp)
2005cdf6:	d266                	c.swsp	s9,36(sp)
2005cdf8:	d06a                	c.swsp	s10,32(sp)
2005cdfa:	ce6e                	c.swsp	s11,28(sp)
2005cdfc:	0087aa83          	lw	s5,8(a5)
2005ce00:	4705                	c.li	a4,1
2005ce02:	d398                	c.sw	a4,32(a5)
2005ce04:	40000513          	addi	a0,zero,1024
2005ce08:	20068b37          	lui	s6,0x20068
2005ce0c:	00e78323          	sb	a4,6(a5)
2005ce10:	b4440413          	addi	s0,s0,-1212
2005ce14:	ce1fe0ef          	jal	ra,2005baf4 <rtos_create_secure_context>
2005ce18:	001a8a13          	addi	s4,s5,1
2005ce1c:	8c5a                	c.mv	s8,s6
2005ce1e:	f601a503          	lw	a0,-160(gp) # 20066038 <shell_sema>
2005ce22:	55fd                	c.li	a1,-1
2005ce24:	a4bfe0ef          	jal	ra,2005b86e <rtos_sema_take>
2005ce28:	f3fff0ef          	jal	ra,2005cd66 <shell_loguartRx_dispatch>
2005ce2c:	00444783          	lbu	a5,4(s0)
2005ce30:	0ff7f793          	andi	a5,a5,255
2005ce34:	d7ed                	c.beqz	a5,2005ce1e <shell_task_ram+0x46>
2005ce36:	85d2                	c.mv	a1,s4
2005ce38:	a08b0513          	addi	a0,s6,-1528 # 20067a08 <log_buf>
2005ce3c:	7c6040ef          	jal	ra,20061602 <strcpy>
2005ce40:	8552                	c.mv	a0,s4
2005ce42:	dffac097          	auipc	ra,0xdffac
2005ce46:	058080e7          	jalr	ra,88(ra) # 8e9a <shell_get_argc>
2005ce4a:	892a                	c.mv	s2,a0
2005ce4c:	8552                	c.mv	a0,s4
2005ce4e:	dffac097          	auipc	ra,0xdffac
2005ce52:	086080e7          	jalr	ra,134(ra) # 8ed4 <shell_get_argv>
2005ce56:	00052d83          	lw	s11,0(a0)
2005ce5a:	4804                	c.lw	s1,16(s0)
2005ce5c:	01442d03          	lw	s10,20(s0)
2005ce60:	89aa                	c.mv	s3,a0
2005ce62:	4c81                	c.li	s9,0
2005ce64:	019d0e63          	beq	s10,s9,2005ce80 <shell_task_ram+0xa8>
2005ce68:	408c                	c.lw	a1,0(s1)
2005ce6a:	856e                	c.mv	a0,s11
2005ce6c:	c626                	c.swsp	s1,12(sp)
2005ce6e:	dffad097          	auipc	ra,0xdffad
2005ce72:	a2a080e7          	jalr	ra,-1494(ra) # 9898 <_stricmp>
2005ce76:	47b2                	c.lwsp	a5,12(sp)
2005ce78:	04c1                	c.addi	s1,16
2005ce7a:	e115                	c.bnez	a0,2005ce9e <shell_task_ram+0xc6>
2005ce7c:	479c                	c.lw	a5,8(a5)
2005ce7e:	e395                	c.bnez	a5,2005cea2 <shell_task_ram+0xca>
2005ce80:	a08c0513          	addi	a0,s8,-1528
2005ce84:	b5efe0ef          	jal	ra,2005b1e2 <log_service>
2005ce88:	4601                	c.li	a2,0
2005ce8a:	08000593          	addi	a1,zero,128
2005ce8e:	8556                	c.mv	a0,s5
2005ce90:	dffac097          	auipc	ra,0xdffac
2005ce94:	ff2080e7          	jalr	ra,-14(ra) # 8e82 <shell_array_init>
2005ce98:	00040223          	sb	zero,4(s0)
2005ce9c:	b749                	c.j	2005ce1e <shell_task_ram+0x46>
2005ce9e:	0c85                	c.addi	s9,1
2005cea0:	b7d1                	c.j	2005ce64 <shell_task_ram+0x8c>
2005cea2:	fff90513          	addi	a0,s2,-1
2005cea6:	0542                	c.slli	a0,0x10
2005cea8:	00498593          	addi	a1,s3,4
2005ceac:	8141                	c.srli	a0,0x10
2005ceae:	9782                	c.jalr	a5
2005ceb0:	bfe1                	c.j	2005ce88 <shell_task_ram+0xb0>

2005ceb2 <shell_init_ram>:
2005ceb2:	1141                	c.addi	sp,-16
2005ceb4:	c606                	c.swsp	ra,12(sp)
2005ceb6:	85afe0ef          	jal	ra,2005af10 <log_service_init>
2005ceba:	20064737          	lui	a4,0x20064
2005cebe:	38c70693          	addi	a3,a4,908 # 2006438c <shell_cmd_table>
2005cec2:	20064737          	lui	a4,0x20064
2005cec6:	200017b7          	lui	a5,0x20001
2005ceca:	3cc70713          	addi	a4,a4,972 # 200643cc <ipc_tickless_table>
2005cece:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005ced2:	8f15                	c.sub	a4,a3
2005ced4:	cb94                	c.sw	a3,16(a5)
2005ced6:	8311                	c.srli	a4,0x4
2005ced8:	cbd8                	c.sw	a4,20(a5)
2005ceda:	00078223          	sb	zero,4(a5)
2005cede:	4705                	c.li	a4,1
2005cee0:	00e782a3          	sb	a4,5(a5)
2005cee4:	2005d737          	lui	a4,0x2005d
2005cee8:	d5070713          	addi	a4,a4,-688 # 2005cd50 <shell_give_sema>
2005ceec:	f6018513          	addi	a0,gp,-160 # 20066038 <shell_sema>
2005cef0:	cfd8                	c.sw	a4,28(a5)
2005cef2:	93dfe0ef          	jal	ra,2005b82e <rtos_sema_create_binary>
2005cef6:	2005d637          	lui	a2,0x2005d
2005cefa:	200635b7          	lui	a1,0x20063
2005cefe:	4795                	c.li	a5,5
2005cf00:	6705                	c.lui	a4,0x1
2005cf02:	4681                	c.li	a3,0
2005cf04:	dd860613          	addi	a2,a2,-552 # 2005cdd8 <shell_task_ram>
2005cf08:	d4458593          	addi	a1,a1,-700 # 20062d44 <__FUNCTION__.5+0x28>
2005cf0c:	4501                	c.li	a0,0
2005cf0e:	bb3fe0ef          	jal	ra,2005bac0 <rtos_task_create>
2005cf12:	c105                	c.beqz	a0,2005cf32 <shell_init_ram+0x80>
2005cf14:	40b2                	c.lwsp	ra,12(sp)
2005cf16:	200636b7          	lui	a3,0x20063
2005cf1a:	200635b7          	lui	a1,0x20063
2005cf1e:	d5468693          	addi	a3,a3,-684 # 20062d54 <__FUNCTION__.5+0x38>
2005cf22:	04500613          	addi	a2,zero,69
2005cf26:	d7058593          	addi	a1,a1,-656 # 20062d70 <__FUNCTION__.5+0x54>
2005cf2a:	4509                	c.li	a0,2
2005cf2c:	0141                	c.addi	sp,16
2005cf2e:	f43fd06f          	jal	zero,2005ae70 <rtk_log_write>
2005cf32:	40b2                	c.lwsp	ra,12(sp)
2005cf34:	0141                	c.addi	sp,16
2005cf36:	8082                	c.jr	ra

2005cf38 <sys_clear_ota_signature>:
2005cf38:	7155                	c.addi16sp	sp,-208
2005cf3a:	4601                	c.li	a2,0
2005cf3c:	102c                	c.addi4spn	a1,sp,40
2005cf3e:	4509                	c.li	a0,2
2005cf40:	c786                	c.swsp	ra,204(sp)
2005cf42:	c3a6                	c.swsp	s1,196(sp)
2005cf44:	c5a2                	c.swsp	s0,200(sp)
2005cf46:	c1ca                	c.swsp	s2,192(sp)
2005cf48:	df4e                	c.swsp	s3,188(sp)
2005cf4a:	dd52                	c.swsp	s4,184(sp)
2005cf4c:	c082                	c.swsp	zero,64(sp)
2005cf4e:	c282                	c.swsp	zero,68(sp)
2005cf50:	bb5fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005cf54:	4601                	c.li	a2,0
2005cf56:	106c                	c.addi4spn	a1,sp,44
2005cf58:	450d                	c.li	a0,3
2005cf5a:	babfb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005cf5e:	4505                	c.li	a0,1
2005cf60:	c62fc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005cf64:	0ff57693          	andi	a3,a0,255
2005cf68:	0016c493          	xori	s1,a3,1
2005cf6c:	f80007b7          	lui	a5,0xf8000
2005cf70:	e2e9                	c.bnez	a3,2005d032 <sys_clear_ota_signature+0xfa>
2005cf72:	5722                	c.lwsp	a4,40(sp)
2005cf74:	0ff57813          	andi	a6,a0,255
2005cf78:	97ba                	c.add	a5,a4
2005cf7a:	00281713          	slli	a4,a6,0x2
2005cf7e:	0a070613          	addi	a2,a4,160 # 10a0 <CPU_ClkGet+0x58a>
2005cf82:	080c                	c.addi4spn	a1,sp,16
2005cf84:	962e                	c.add	a2,a1
2005cf86:	f8f62423          	sw	a5,-120(a2)
2005cf8a:	4785                	c.li	a5,1
2005cf8c:	f8000437          	lui	s0,0xf8000
2005cf90:	0af69363          	bne	a3,a5,2005d036 <sys_clear_ota_signature+0xfe>
2005cf94:	57a2                	c.lwsp	a5,40(sp)
2005cf96:	943e                	c.add	s0,a5
2005cf98:	00249793          	slli	a5,s1,0x2
2005cf9c:	0814                	c.addi4spn	a3,sp,16
2005cf9e:	0a078793          	addi	a5,a5,160 # f80000a0 <__ctrace_end__+0x2e0000a0>
2005cfa2:	97b6                	c.add	a5,a3
2005cfa4:	f887a423          	sw	s0,-120(a5)
2005cfa8:	0a070793          	addi	a5,a4,160
2005cfac:	00d78733          	add	a4,a5,a3
2005cfb0:	f8872a03          	lw	s4,-120(a4)
2005cfb4:	0485                	c.addi	s1,1
2005cfb6:	200639b7          	lui	s3,0x20063
2005cfba:	200636b7          	lui	a3,0x20063
2005cfbe:	20063937          	lui	s2,0x20063
2005cfc2:	0805                	c.addi	a6,1
2005cfc4:	4785                	c.li	a5,1
2005cfc6:	e5498713          	addi	a4,s3,-428 # 20062e54 <__func__.1>
2005cfca:	88d2                	c.mv	a7,s4
2005cfcc:	d7868693          	addi	a3,a3,-648 # 20062d78 <__FUNCTION__.5+0x5c>
2005cfd0:	04100613          	addi	a2,zero,65
2005cfd4:	dc890593          	addi	a1,s2,-568 # 20062dc8 <__FUNCTION__.5+0xac>
2005cfd8:	4505                	c.li	a0,1
2005cfda:	c222                	c.swsp	s0,4(sp)
2005cfdc:	c026                	c.swsp	s1,0(sp)
2005cfde:	ce42                	c.swsp	a6,28(sp)
2005cfe0:	e91fd0ef          	jal	ra,2005ae70 <rtk_log_write>
2005cfe4:	1810                	c.addi4spn	a2,sp,48
2005cfe6:	85a2                	c.mv	a1,s0
2005cfe8:	00a8                	c.addi4spn	a0,sp,72
2005cfea:	f84fd0ef          	jal	ra,2005a76e <flash_read_word>
2005cfee:	1850                	c.addi4spn	a2,sp,52
2005cff0:	00440593          	addi	a1,s0,4 # f8000004 <__ctrace_end__+0x2e000004>
2005cff4:	00a8                	c.addi4spn	a0,sp,72
2005cff6:	f78fd0ef          	jal	ra,2005a76e <flash_read_word>
2005cffa:	5742                	c.lwsp	a4,48(sp)
2005cffc:	353937b7          	lui	a5,0x35393
2005d000:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d004:	4872                	c.lwsp	a6,28(sp)
2005d006:	02f71a63          	bne	a4,a5,2005d03a <sys_clear_ota_signature+0x102>
2005d00a:	5752                	c.lwsp	a4,52(sp)
2005d00c:	313137b7          	lui	a5,0x31313
2005d010:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d014:	02f71363          	bne	a4,a5,2005d03a <sys_clear_ota_signature+0x102>
2005d018:	0090                	c.addi4spn	a2,sp,64
2005d01a:	45a1                	c.li	a1,8
2005d01c:	8552                	c.mv	a0,s4
2005d01e:	b9bfb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d022:	40be                	c.lwsp	ra,204(sp)
2005d024:	442e                	c.lwsp	s0,200(sp)
2005d026:	449e                	c.lwsp	s1,196(sp)
2005d028:	490e                	c.lwsp	s2,192(sp)
2005d02a:	59fa                	c.lwsp	s3,188(sp)
2005d02c:	5a6a                	c.lwsp	s4,184(sp)
2005d02e:	6169                	c.addi16sp	sp,208
2005d030:	8082                	c.jr	ra
2005d032:	5732                	c.lwsp	a4,44(sp)
2005d034:	b781                	c.j	2005cf74 <sys_clear_ota_signature+0x3c>
2005d036:	57b2                	c.lwsp	a5,44(sp)
2005d038:	bfb9                	c.j	2005cf96 <sys_clear_ota_signature+0x5e>
2005d03a:	200636b7          	lui	a3,0x20063
2005d03e:	88a6                	c.mv	a7,s1
2005d040:	4785                	c.li	a5,1
2005d042:	e5498713          	addi	a4,s3,-428
2005d046:	dcc68693          	addi	a3,a3,-564 # 20062dcc <__FUNCTION__.5+0xb0>
2005d04a:	04500613          	addi	a2,zero,69
2005d04e:	dc890593          	addi	a1,s2,-568
2005d052:	4509                	c.li	a0,2
2005d054:	e1dfd0ef          	jal	ra,2005ae70 <rtk_log_write>
2005d058:	b7e9                	c.j	2005d022 <sys_clear_ota_signature+0xea>

2005d05a <sys_recover_ota_signature>:
2005d05a:	353937b7          	lui	a5,0x35393
2005d05e:	715d                	c.addi16sp	sp,-80
2005d060:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d064:	d43e                	c.swsp	a5,40(sp)
2005d066:	313137b7          	lui	a5,0x31313
2005d06a:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d06e:	6505                	c.lui	a0,0x1
2005d070:	c2a6                	c.swsp	s1,68(sp)
2005d072:	de4e                	c.swsp	s3,60(sp)
2005d074:	c686                	c.swsp	ra,76(sp)
2005d076:	c4a2                	c.swsp	s0,72(sp)
2005d078:	c0ca                	c.swsp	s2,64(sp)
2005d07a:	d63e                	c.swsp	a5,44(sp)
2005d07c:	837fd0ef          	jal	ra,2005a8b2 <__wrap_malloc>
2005d080:	200639b7          	lui	s3,0x20063
2005d084:	200634b7          	lui	s1,0x20063
2005d088:	e50d                	c.bnez	a0,2005d0b2 <sys_recover_ota_signature+0x58>
2005d08a:	200636b7          	lui	a3,0x20063
2005d08e:	e3898713          	addi	a4,s3,-456 # 20062e38 <__func__.0>
2005d092:	dc848593          	addi	a1,s1,-568 # 20062dc8 <__FUNCTION__.5+0xac>
2005d096:	e1c68693          	addi	a3,a3,-484 # 20062e1c <__FUNCTION__.5+0x100>
2005d09a:	04500613          	addi	a2,zero,69
2005d09e:	4509                	c.li	a0,2
2005d0a0:	dd1fd0ef          	jal	ra,2005ae70 <rtk_log_write>
2005d0a4:	40b6                	c.lwsp	ra,76(sp)
2005d0a6:	4426                	c.lwsp	s0,72(sp)
2005d0a8:	4496                	c.lwsp	s1,68(sp)
2005d0aa:	4906                	c.lwsp	s2,64(sp)
2005d0ac:	59f2                	c.lwsp	s3,60(sp)
2005d0ae:	6161                	c.addi16sp	sp,80
2005d0b0:	8082                	c.jr	ra
2005d0b2:	4601                	c.li	a2,0
2005d0b4:	082c                	c.addi4spn	a1,sp,24
2005d0b6:	842a                	c.mv	s0,a0
2005d0b8:	4509                	c.li	a0,2
2005d0ba:	a4bfb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d0be:	086c                	c.addi4spn	a1,sp,28
2005d0c0:	4601                	c.li	a2,0
2005d0c2:	450d                	c.li	a0,3
2005d0c4:	a41fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d0c8:	4505                	c.li	a0,1
2005d0ca:	af8fc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d0ce:	0ff57593          	andi	a1,a0,255
2005d0d2:	4785                	c.li	a5,1
2005d0d4:	0015c693          	xori	a3,a1,1
2005d0d8:	0af59963          	bne	a1,a5,2005d18a <sys_recover_ota_signature+0x130>
2005d0dc:	4762                	c.lwsp	a4,24(sp)
2005d0de:	f80007b7          	lui	a5,0xf8000
2005d0e2:	00269613          	slli	a2,a3,0x2
2005d0e6:	97ba                	c.add	a5,a4
2005d0e8:	03060713          	addi	a4,a2,48
2005d0ec:	970a                	c.add	a4,sp
2005d0ee:	fef72823          	sw	a5,-16(a4)
2005d0f2:	f80008b7          	lui	a7,0xf8000
2005d0f6:	edd1                	c.bnez	a1,2005d192 <sys_recover_ota_signature+0x138>
2005d0f8:	47e2                	c.lwsp	a5,24(sp)
2005d0fa:	0ff57813          	andi	a6,a0,255
2005d0fe:	98be                	c.add	a7,a5
2005d100:	00281793          	slli	a5,a6,0x2
2005d104:	03078793          	addi	a5,a5,48 # f8000030 <__ctrace_end__+0x2e000030>
2005d108:	978a                	c.add	a5,sp
2005d10a:	ff17a823          	sw	a7,-16(a5)
2005d10e:	03060793          	addi	a5,a2,48
2005d112:	00278633          	add	a2,a5,sp
2005d116:	ff062903          	lw	s2,-16(a2)
2005d11a:	0685                	c.addi	a3,1
2005d11c:	c036                	c.swsp	a3,0(sp)
2005d11e:	200636b7          	lui	a3,0x20063
2005d122:	e3898713          	addi	a4,s3,-456
2005d126:	0805                	c.addi	a6,1
2005d128:	4785                	c.li	a5,1
2005d12a:	d7868693          	addi	a3,a3,-648 # 20062d78 <__FUNCTION__.5+0x5c>
2005d12e:	dc848593          	addi	a1,s1,-568
2005d132:	04100613          	addi	a2,zero,65
2005d136:	4505                	c.li	a0,1
2005d138:	c24a                	c.swsp	s2,4(sp)
2005d13a:	d37fd0ef          	jal	ra,2005ae70 <rtk_log_write>
2005d13e:	8622                	c.mv	a2,s0
2005d140:	6585                	c.lui	a1,0x1
2005d142:	854a                	c.mv	a0,s2
2005d144:	a2ffb0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005d148:	85ca                	c.mv	a1,s2
2005d14a:	4509                	c.li	a0,2
2005d14c:	b53fb0ef          	jal	ra,20058c9e <FLASH_EraseXIP>
2005d150:	4621                	c.li	a2,8
2005d152:	102c                	c.addi4spn	a1,sp,40
2005d154:	8522                	c.mv	a0,s0
2005d156:	dffac097          	auipc	ra,0xdffac
2005d15a:	4bc080e7          	jalr	ra,1212(ra) # 9612 <_memcpy>
2005d15e:	4481                	c.li	s1,0
2005d160:	6985                	c.lui	s3,0x1
2005d162:	01248533          	add	a0,s1,s2
2005d166:	8622                	c.mv	a2,s0
2005d168:	10000593          	addi	a1,zero,256
2005d16c:	10048493          	addi	s1,s1,256
2005d170:	a49fb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d174:	ff3497e3          	bne	s1,s3,2005d162 <sys_recover_ota_signature+0x108>
2005d178:	8522                	c.mv	a0,s0
2005d17a:	4426                	c.lwsp	s0,72(sp)
2005d17c:	40b6                	c.lwsp	ra,76(sp)
2005d17e:	4496                	c.lwsp	s1,68(sp)
2005d180:	4906                	c.lwsp	s2,64(sp)
2005d182:	59f2                	c.lwsp	s3,60(sp)
2005d184:	6161                	c.addi16sp	sp,80
2005d186:	f30fd06f          	jal	zero,2005a8b6 <__wrap_free>
2005d18a:	4772                	c.lwsp	a4,28(sp)
2005d18c:	f80007b7          	lui	a5,0xf8000
2005d190:	bf89                	c.j	2005d0e2 <sys_recover_ota_signature+0x88>
2005d192:	47f2                	c.lwsp	a5,28(sp)
2005d194:	b79d                	c.j	2005d0fa <sys_recover_ota_signature+0xa0>

2005d196 <sys_reset>:
2005d196:	9ddfc06f          	jal	zero,20059b72 <System_Reset>

2005d19a <prvAddCurrentTaskToDelayedList>:
2005d19a:	1101                	c.addi	sp,-32
2005d19c:	ca26                	c.swsp	s1,20(sp)
2005d19e:	cc22                	c.swsp	s0,24(sp)
2005d1a0:	c84a                	c.swsp	s2,16(sp)
2005d1a2:	c64e                	c.swsp	s3,12(sp)
2005d1a4:	ce06                	c.swsp	ra,28(sp)
2005d1a6:	f9c1a983          	lw	s3,-100(gp) # 20066074 <xTickCount>
2005d1aa:	f6418793          	addi	a5,gp,-156 # 2006603c <pxCurrentTCB>
2005d1ae:	842a                	c.mv	s0,a0
2005d1b0:	4388                	c.lw	a0,0(a5)
2005d1b2:	892e                	c.mv	s2,a1
2005d1b4:	0511                	c.addi	a0,4
2005d1b6:	9e7fd0ef          	jal	ra,2005ab9c <uxListRemove>
2005d1ba:	f6418793          	addi	a5,gp,-156 # 2006603c <pxCurrentTCB>
2005d1be:	ed09                	c.bnez	a0,2005d1d8 <prvAddCurrentTaskToDelayedList+0x3e>
2005d1c0:	4398                	c.lw	a4,0(a5)
2005d1c2:	f8818693          	addi	a3,gp,-120 # 20066060 <uxTopReadyPriority>
2005d1c6:	574c                	c.lw	a1,44(a4)
2005d1c8:	4290                	c.lw	a2,0(a3)
2005d1ca:	4705                	c.li	a4,1
2005d1cc:	00b71733          	sll	a4,a4,a1
2005d1d0:	fff74713          	xori	a4,a4,-1
2005d1d4:	8f71                	c.and	a4,a2
2005d1d6:	c298                	c.sw	a4,0(a3)
2005d1d8:	577d                	c.li	a4,-1
2005d1da:	02e41263          	bne	s0,a4,2005d1fe <prvAddCurrentTaskToDelayedList+0x64>
2005d1de:	02090063          	beq	s2,zero,2005d1fe <prvAddCurrentTaskToDelayedList+0x64>
2005d1e2:	438c                	c.lw	a1,0(a5)
2005d1e4:	4462                	c.lwsp	s0,24(sp)
2005d1e6:	40f2                	c.lwsp	ra,28(sp)
2005d1e8:	44d2                	c.lwsp	s1,20(sp)
2005d1ea:	4942                	c.lwsp	s2,16(sp)
2005d1ec:	49b2                	c.lwsp	s3,12(sp)
2005d1ee:	20068537          	lui	a0,0x20068
2005d1f2:	0591                	c.addi	a1,4
2005d1f4:	ba050513          	addi	a0,a0,-1120 # 20067ba0 <xSuspendedTaskList>
2005d1f8:	6105                	c.addi16sp	sp,32
2005d1fa:	95ffd06f          	jal	zero,2005ab58 <vListInsertEnd>
2005d1fe:	4398                	c.lw	a4,0(a5)
2005d200:	944e                	c.add	s0,s3
2005d202:	c340                	c.sw	s0,4(a4)
2005d204:	01347e63          	bgeu	s0,s3,2005d220 <prvAddCurrentTaskToDelayedList+0x86>
2005d208:	f6c1a503          	lw	a0,-148(gp) # 20066044 <pxOverflowDelayedTaskList>
2005d20c:	4462                	c.lwsp	s0,24(sp)
2005d20e:	438c                	c.lw	a1,0(a5)
2005d210:	40f2                	c.lwsp	ra,28(sp)
2005d212:	44d2                	c.lwsp	s1,20(sp)
2005d214:	4942                	c.lwsp	s2,16(sp)
2005d216:	49b2                	c.lwsp	s3,12(sp)
2005d218:	0591                	c.addi	a1,4
2005d21a:	6105                	c.addi16sp	sp,32
2005d21c:	953fd06f          	jal	zero,2005ab6e <vListInsert>
2005d220:	f681a503          	lw	a0,-152(gp) # 20066040 <pxDelayedTaskList>
2005d224:	438c                	c.lw	a1,0(a5)
2005d226:	0591                	c.addi	a1,4
2005d228:	947fd0ef          	jal	ra,2005ab6e <vListInsert>
2005d22c:	f8c18793          	addi	a5,gp,-116 # 20066064 <xNextTaskUnblockTime>
2005d230:	4398                	c.lw	a4,0(a5)
2005d232:	00e47363          	bgeu	s0,a4,2005d238 <prvAddCurrentTaskToDelayedList+0x9e>
2005d236:	c380                	c.sw	s0,0(a5)
2005d238:	40f2                	c.lwsp	ra,28(sp)
2005d23a:	4462                	c.lwsp	s0,24(sp)
2005d23c:	44d2                	c.lwsp	s1,20(sp)
2005d23e:	4942                	c.lwsp	s2,16(sp)
2005d240:	49b2                	c.lwsp	s3,12(sp)
2005d242:	6105                	c.addi16sp	sp,32
2005d244:	8082                	c.jr	ra

2005d246 <prvWriteNameToBuffer>:
2005d246:	1141                	c.addi	sp,-16
2005d248:	c606                	c.swsp	ra,12(sp)
2005d24a:	c422                	c.swsp	s0,8(sp)
2005d24c:	842a                	c.mv	s0,a0
2005d24e:	3b4040ef          	jal	ra,20061602 <strcpy>
2005d252:	8522                	c.mv	a0,s0
2005d254:	dffad097          	auipc	ra,0xdffad
2005d258:	884080e7          	jalr	ra,-1916(ra) # 9ad8 <__wrap_strlen>
2005d25c:	87aa                	c.mv	a5,a0
2005d25e:	4759                	c.li	a4,22
2005d260:	02000693          	addi	a3,zero,32
2005d264:	02f77063          	bgeu	a4,a5,2005d284 <prvWriteNameToBuffer+0x3e>
2005d268:	475d                	c.li	a4,23
2005d26a:	4781                	c.li	a5,0
2005d26c:	00a76463          	bltu	a4,a0,2005d274 <prvWriteNameToBuffer+0x2e>
2005d270:	40a707b3          	sub	a5,a4,a0
2005d274:	953e                	c.add	a0,a5
2005d276:	9522                	c.add	a0,s0
2005d278:	00050023          	sb	zero,0(a0)
2005d27c:	40b2                	c.lwsp	ra,12(sp)
2005d27e:	4422                	c.lwsp	s0,8(sp)
2005d280:	0141                	c.addi	sp,16
2005d282:	8082                	c.jr	ra
2005d284:	00f40633          	add	a2,s0,a5
2005d288:	00d60023          	sb	a3,0(a2)
2005d28c:	0785                	c.addi	a5,1
2005d28e:	bfd9                	c.j	2005d264 <prvWriteNameToBuffer+0x1e>

2005d290 <prvGetExpectedIdleTime>:
2005d290:	f881a703          	lw	a4,-120(gp) # 20066060 <uxTopReadyPriority>
2005d294:	f641a783          	lw	a5,-156(gp) # 2006603c <pxCurrentTCB>
2005d298:	57c8                	c.lw	a0,44(a5)
2005d29a:	e105                	c.bnez	a0,2005d2ba <prvGetExpectedIdleTime+0x2a>
2005d29c:	200687b7          	lui	a5,0x20068
2005d2a0:	a887a683          	lw	a3,-1400(a5) # 20067a88 <pxReadyTasksLists>
2005d2a4:	4785                	c.li	a5,1
2005d2a6:	00d7eb63          	bltu	a5,a3,2005d2bc <prvGetExpectedIdleTime+0x2c>
2005d2aa:	00e7e963          	bltu	a5,a4,2005d2bc <prvGetExpectedIdleTime+0x2c>
2005d2ae:	f8c1a503          	lw	a0,-116(gp) # 20066064 <xNextTaskUnblockTime>
2005d2b2:	f9c1a783          	lw	a5,-100(gp) # 20066074 <xTickCount>
2005d2b6:	8d1d                	c.sub	a0,a5
2005d2b8:	8082                	c.jr	ra
2005d2ba:	4501                	c.li	a0,0
2005d2bc:	8082                	c.jr	ra

2005d2be <prvResetNextTaskUnblockTime>:
2005d2be:	f6818713          	addi	a4,gp,-152 # 20066040 <pxDelayedTaskList>
2005d2c2:	431c                	c.lw	a5,0(a4)
2005d2c4:	4394                	c.lw	a3,0(a5)
2005d2c6:	f8c18793          	addi	a5,gp,-116 # 20066064 <xNextTaskUnblockTime>
2005d2ca:	e681                	c.bnez	a3,2005d2d2 <prvResetNextTaskUnblockTime+0x14>
2005d2cc:	577d                	c.li	a4,-1
2005d2ce:	c398                	c.sw	a4,0(a5)
2005d2d0:	8082                	c.jr	ra
2005d2d2:	4318                	c.lw	a4,0(a4)
2005d2d4:	4758                	c.lw	a4,12(a4)
2005d2d6:	4318                	c.lw	a4,0(a4)
2005d2d8:	bfdd                	c.j	2005d2ce <prvResetNextTaskUnblockTime+0x10>

2005d2da <prvDeleteTCB>:
2005d2da:	1141                	c.addi	sp,-16
2005d2dc:	c422                	c.swsp	s0,8(sp)
2005d2de:	842a                	c.mv	s0,a0
2005d2e0:	06c50513          	addi	a0,a0,108
2005d2e4:	c606                	c.swsp	ra,12(sp)
2005d2e6:	232040ef          	jal	ra,20061518 <_reclaim_reent>
2005d2ea:	16144783          	lbu	a5,353(s0)
2005d2ee:	eb91                	c.bnez	a5,2005d302 <prvDeleteTCB+0x28>
2005d2f0:	5808                	c.lw	a0,48(s0)
2005d2f2:	ef0fd0ef          	jal	ra,2005a9e2 <vPortFree>
2005d2f6:	8522                	c.mv	a0,s0
2005d2f8:	4422                	c.lwsp	s0,8(sp)
2005d2fa:	40b2                	c.lwsp	ra,12(sp)
2005d2fc:	0141                	c.addi	sp,16
2005d2fe:	ee4fd06f          	jal	zero,2005a9e2 <vPortFree>
2005d302:	4705                	c.li	a4,1
2005d304:	fee789e3          	beq	a5,a4,2005d2f6 <prvDeleteTCB+0x1c>
2005d308:	40b2                	c.lwsp	ra,12(sp)
2005d30a:	4422                	c.lwsp	s0,8(sp)
2005d30c:	0141                	c.addi	sp,16
2005d30e:	8082                	c.jr	ra

2005d310 <prvInitialiseNewTask.constprop.0>:
2005d310:	1101                	c.addi	sp,-32
2005d312:	c452                	c.swsp	s4,8(sp)
2005d314:	8a2a                	c.mv	s4,a0
2005d316:	03082503          	lw	a0,48(a6)
2005d31a:	c64e                	c.swsp	s3,12(sp)
2005d31c:	00261993          	slli	s3,a2,0x2
2005d320:	864e                	c.mv	a2,s3
2005d322:	ca26                	c.swsp	s1,20(sp)
2005d324:	84ae                	c.mv	s1,a1
2005d326:	0a500593          	addi	a1,zero,165
2005d32a:	cc22                	c.swsp	s0,24(sp)
2005d32c:	c84a                	c.swsp	s2,16(sp)
2005d32e:	8442                	c.mv	s0,a6
2005d330:	c256                	c.swsp	s5,4(sp)
2005d332:	c05a                	c.swsp	s6,0(sp)
2005d334:	ce06                	c.swsp	ra,28(sp)
2005d336:	8ab6                	c.mv	s5,a3
2005d338:	893a                	c.mv	s2,a4
2005d33a:	8b3e                	c.mv	s6,a5
2005d33c:	dffac097          	auipc	ra,0xdffac
2005d340:	792080e7          	jalr	ra,1938(ra) # 9ace <__wrap_memset>
2005d344:	ffc98613          	addi	a2,s3,-4 # ffc <CPU_ClkGet+0x4e6>
2005d348:	03042983          	lw	s3,48(s0)
2005d34c:	99b2                	c.add	s3,a2
2005d34e:	fe09f993          	andi	s3,s3,-32
2005d352:	05342623          	sw	s3,76(s0)
2005d356:	c8f9                	c.beqz	s1,2005d42c <prvInitialiseNewTask.constprop.0+0x11c>
2005d358:	85a6                	c.mv	a1,s1
2005d35a:	03440793          	addi	a5,s0,52
2005d35e:	01848693          	addi	a3,s1,24
2005d362:	0005c703          	lbu	a4,0(a1) # 1000 <CPU_ClkGet+0x4ea>
2005d366:	00e78023          	sb	a4,0(a5)
2005d36a:	c709                	c.beqz	a4,2005d374 <prvInitialiseNewTask.constprop.0+0x64>
2005d36c:	0585                	c.addi	a1,1
2005d36e:	0785                	c.addi	a5,1
2005d370:	fed599e3          	bne	a1,a3,2005d362 <prvInitialiseNewTask.constprop.0+0x52>
2005d374:	040405a3          	sb	zero,75(s0)
2005d378:	47a9                	c.li	a5,10
2005d37a:	0127f363          	bgeu	a5,s2,2005d380 <prvInitialiseNewTask.constprop.0+0x70>
2005d37e:	4929                	c.li	s2,10
2005d380:	00440513          	addi	a0,s0,4
2005d384:	03242623          	sw	s2,44(s0)
2005d388:	05242e23          	sw	s2,92(s0)
2005d38c:	06042023          	sw	zero,96(s0)
2005d390:	fc2fd0ef          	jal	ra,2005ab52 <vListInitialiseItem>
2005d394:	01840513          	addi	a0,s0,24
2005d398:	fbafd0ef          	jal	ra,2005ab52 <vListInitialiseItem>
2005d39c:	472d                	c.li	a4,11
2005d39e:	412704b3          	sub	s1,a4,s2
2005d3a2:	4611                	c.li	a2,4
2005d3a4:	4581                	c.li	a1,0
2005d3a6:	c800                	c.sw	s0,16(s0)
2005d3a8:	cc04                	c.sw	s1,24(s0)
2005d3aa:	d040                	c.sw	s0,36(s0)
2005d3ac:	04042823          	sw	zero,80(s0)
2005d3b0:	06042223          	sw	zero,100(s0)
2005d3b4:	06042423          	sw	zero,104(s0)
2005d3b8:	15c40513          	addi	a0,s0,348
2005d3bc:	dffac097          	auipc	ra,0xdffac
2005d3c0:	712080e7          	jalr	ra,1810(ra) # 9ace <__wrap_memset>
2005d3c4:	4605                	c.li	a2,1
2005d3c6:	4581                	c.li	a1,0
2005d3c8:	16040513          	addi	a0,s0,352
2005d3cc:	dffac097          	auipc	ra,0xdffac
2005d3d0:	702080e7          	jalr	ra,1794(ra) # 9ace <__wrap_memset>
2005d3d4:	0f000613          	addi	a2,zero,240
2005d3d8:	4581                	c.li	a1,0
2005d3da:	06c40513          	addi	a0,s0,108
2005d3de:	dffac097          	auipc	ra,0xdffac
2005d3e2:	6f0080e7          	jalr	ra,1776(ra) # 9ace <__wrap_memset>
2005d3e6:	200687b7          	lui	a5,0x20068
2005d3ea:	fc878793          	addi	a5,a5,-56 # 20067fc8 <__sf>
2005d3ee:	d83c                	c.sw	a5,112(s0)
2005d3f0:	200687b7          	lui	a5,0x20068
2005d3f4:	03078793          	addi	a5,a5,48 # 20068030 <__sf+0x68>
2005d3f8:	d87c                	c.sw	a5,116(s0)
2005d3fa:	200687b7          	lui	a5,0x20068
2005d3fe:	09878793          	addi	a5,a5,152 # 20068098 <__sf+0xd0>
2005d402:	dc3c                	c.sw	a5,120(s0)
2005d404:	8656                	c.mv	a2,s5
2005d406:	85d2                	c.mv	a1,s4
2005d408:	854e                	c.mv	a0,s3
2005d40a:	b3ffe0ef          	jal	ra,2005bf48 <pxPortInitialiseStack>
2005d40e:	c008                	c.sw	a0,0(s0)
2005d410:	000b0463          	beq	s6,zero,2005d418 <prvInitialiseNewTask.constprop.0+0x108>
2005d414:	008b2023          	sw	s0,0(s6)
2005d418:	40f2                	c.lwsp	ra,28(sp)
2005d41a:	4462                	c.lwsp	s0,24(sp)
2005d41c:	44d2                	c.lwsp	s1,20(sp)
2005d41e:	4942                	c.lwsp	s2,16(sp)
2005d420:	49b2                	c.lwsp	s3,12(sp)
2005d422:	4a22                	c.lwsp	s4,8(sp)
2005d424:	4a92                	c.lwsp	s5,4(sp)
2005d426:	4b02                	c.lwsp	s6,0(sp)
2005d428:	6105                	c.addi16sp	sp,32
2005d42a:	8082                	c.jr	ra
2005d42c:	02040a23          	sb	zero,52(s0)
2005d430:	b7a1                	c.j	2005d378 <prvInitialiseNewTask.constprop.0+0x68>

2005d432 <vTaskSuspendAll>:
2005d432:	f8018793          	addi	a5,gp,-128 # 20066058 <uxSchedulerSuspended>
2005d436:	4398                	c.lw	a4,0(a5)
2005d438:	0705                	c.addi	a4,1
2005d43a:	c398                	c.sw	a4,0(a5)
2005d43c:	8082                	c.jr	ra

2005d43e <xTaskGetTickCountFromISR>:
2005d43e:	f9c1a503          	lw	a0,-100(gp) # 20066074 <xTickCount>
2005d442:	8082                	c.jr	ra

2005d444 <uxTaskGetNumberOfTasks>:
2005d444:	f781a503          	lw	a0,-136(gp) # 20066050 <uxCurrentNumberOfTasks>
2005d448:	8082                	c.jr	ra

2005d44a <pcTaskGetName>:
2005d44a:	e119                	c.bnez	a0,2005d450 <pcTaskGetName+0x6>
2005d44c:	f641a503          	lw	a0,-156(gp) # 2006603c <pxCurrentTCB>
2005d450:	03450513          	addi	a0,a0,52
2005d454:	8082                	c.jr	ra

2005d456 <xTaskIncrementTick>:
2005d456:	f801a783          	lw	a5,-128(gp) # 20066058 <uxSchedulerSuspended>
2005d45a:	7179                	c.addi16sp	sp,-48
2005d45c:	d606                	c.swsp	ra,44(sp)
2005d45e:	d422                	c.swsp	s0,40(sp)
2005d460:	d226                	c.swsp	s1,36(sp)
2005d462:	d04a                	c.swsp	s2,32(sp)
2005d464:	ce4e                	c.swsp	s3,28(sp)
2005d466:	cc52                	c.swsp	s4,24(sp)
2005d468:	ca56                	c.swsp	s5,20(sp)
2005d46a:	c85a                	c.swsp	s6,16(sp)
2005d46c:	c65e                	c.swsp	s7,12(sp)
2005d46e:	c462                	c.swsp	s8,8(sp)
2005d470:	c266                	c.swsp	s9,4(sp)
2005d472:	c06a                	c.swsp	s10,0(sp)
2005d474:	10079363          	bne	a5,zero,2005d57a <xTaskIncrementTick+0x124>
2005d478:	f9c18793          	addi	a5,gp,-100 # 20066074 <xTickCount>
2005d47c:	0007aa03          	lw	s4,0(a5)
2005d480:	0a05                	c.addi	s4,1
2005d482:	0147a023          	sw	s4,0(a5)
2005d486:	020a1163          	bne	s4,zero,2005d4a8 <xTaskIncrementTick+0x52>
2005d48a:	f6818713          	addi	a4,gp,-152 # 20066040 <pxDelayedTaskList>
2005d48e:	f6c18793          	addi	a5,gp,-148 # 20066044 <pxOverflowDelayedTaskList>
2005d492:	4314                	c.lw	a3,0(a4)
2005d494:	4390                	c.lw	a2,0(a5)
2005d496:	c310                	c.sw	a2,0(a4)
2005d498:	c394                	c.sw	a3,0(a5)
2005d49a:	f9018793          	addi	a5,gp,-112 # 20066068 <xNumOfOverflows>
2005d49e:	4398                	c.lw	a4,0(a5)
2005d4a0:	0705                	c.addi	a4,1
2005d4a2:	c398                	c.sw	a4,0(a5)
2005d4a4:	e1bff0ef          	jal	ra,2005d2be <prvResetNextTaskUnblockTime>
2005d4a8:	f8c18793          	addi	a5,gp,-116 # 20066064 <xNextTaskUnblockTime>
2005d4ac:	439c                	c.lw	a5,0(a5)
2005d4ae:	200684b7          	lui	s1,0x20068
2005d4b2:	f8c18993          	addi	s3,gp,-116 # 20066064 <xNextTaskUnblockTime>
2005d4b6:	a8848493          	addi	s1,s1,-1400 # 20067a88 <pxReadyTasksLists>
2005d4ba:	f6418a93          	addi	s5,gp,-156 # 2006603c <pxCurrentTCB>
2005d4be:	4401                	c.li	s0,0
2005d4c0:	04fa7663          	bgeu	s4,a5,2005d50c <xTaskIncrementTick+0xb6>
2005d4c4:	000aa783          	lw	a5,0(s5)
2005d4c8:	4751                	c.li	a4,20
2005d4ca:	57dc                	c.lw	a5,44(a5)
2005d4cc:	02e787b3          	mul	a5,a5,a4
2005d4d0:	94be                	c.add	s1,a5
2005d4d2:	4098                	c.lw	a4,0(s1)
2005d4d4:	4785                	c.li	a5,1
2005d4d6:	00e7f363          	bgeu	a5,a4,2005d4dc <xTaskIncrementTick+0x86>
2005d4da:	4405                	c.li	s0,1
2005d4dc:	f941a783          	lw	a5,-108(gp) # 2006606c <xPendedTicks>
2005d4e0:	e399                	c.bnez	a5,2005d4e6 <xTaskIncrementTick+0x90>
2005d4e2:	e9bfe0ef          	jal	ra,2005c37c <vApplicationTickHook>
2005d4e6:	fa01a783          	lw	a5,-96(gp) # 20066078 <xYieldPending>
2005d4ea:	c391                	c.beqz	a5,2005d4ee <xTaskIncrementTick+0x98>
2005d4ec:	4405                	c.li	s0,1
2005d4ee:	50b2                	c.lwsp	ra,44(sp)
2005d4f0:	8522                	c.mv	a0,s0
2005d4f2:	5422                	c.lwsp	s0,40(sp)
2005d4f4:	5492                	c.lwsp	s1,36(sp)
2005d4f6:	5902                	c.lwsp	s2,32(sp)
2005d4f8:	49f2                	c.lwsp	s3,28(sp)
2005d4fa:	4a62                	c.lwsp	s4,24(sp)
2005d4fc:	4ad2                	c.lwsp	s5,20(sp)
2005d4fe:	4b42                	c.lwsp	s6,16(sp)
2005d500:	4bb2                	c.lwsp	s7,12(sp)
2005d502:	4c22                	c.lwsp	s8,8(sp)
2005d504:	4c92                	c.lwsp	s9,4(sp)
2005d506:	4d02                	c.lwsp	s10,0(sp)
2005d508:	6145                	c.addi16sp	sp,48
2005d50a:	8082                	c.jr	ra
2005d50c:	4c05                	c.li	s8,1
2005d50e:	4cd1                	c.li	s9,20
2005d510:	f6818793          	addi	a5,gp,-152 # 20066040 <pxDelayedTaskList>
2005d514:	4398                	c.lw	a4,0(a5)
2005d516:	4318                	c.lw	a4,0(a4)
2005d518:	e709                	c.bnez	a4,2005d522 <xTaskIncrementTick+0xcc>
2005d51a:	57fd                	c.li	a5,-1
2005d51c:	00f9a023          	sw	a5,0(s3)
2005d520:	b755                	c.j	2005d4c4 <xTaskIncrementTick+0x6e>
2005d522:	439c                	c.lw	a5,0(a5)
2005d524:	47dc                	c.lw	a5,12(a5)
2005d526:	00c7a903          	lw	s2,12(a5)
2005d52a:	00492783          	lw	a5,4(s2)
2005d52e:	fefa67e3          	bltu	s4,a5,2005d51c <xTaskIncrementTick+0xc6>
2005d532:	00490d13          	addi	s10,s2,4
2005d536:	856a                	c.mv	a0,s10
2005d538:	e64fd0ef          	jal	ra,2005ab9c <uxListRemove>
2005d53c:	02892783          	lw	a5,40(s2)
2005d540:	c789                	c.beqz	a5,2005d54a <xTaskIncrementTick+0xf4>
2005d542:	01890513          	addi	a0,s2,24
2005d546:	e56fd0ef          	jal	ra,2005ab9c <uxListRemove>
2005d54a:	02c92503          	lw	a0,44(s2)
2005d54e:	f8818713          	addi	a4,gp,-120 # 20066060 <uxTopReadyPriority>
2005d552:	4314                	c.lw	a3,0(a4)
2005d554:	00ac17b3          	sll	a5,s8,a0
2005d558:	03950533          	mul	a0,a0,s9
2005d55c:	8fd5                	c.or	a5,a3
2005d55e:	85ea                	c.mv	a1,s10
2005d560:	c31c                	c.sw	a5,0(a4)
2005d562:	9526                	c.add	a0,s1
2005d564:	df4fd0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005d568:	000aa783          	lw	a5,0(s5)
2005d56c:	02c92703          	lw	a4,44(s2)
2005d570:	57dc                	c.lw	a5,44(a5)
2005d572:	f8f76fe3          	bltu	a4,a5,2005d510 <xTaskIncrementTick+0xba>
2005d576:	4405                	c.li	s0,1
2005d578:	bf61                	c.j	2005d510 <xTaskIncrementTick+0xba>
2005d57a:	f9418793          	addi	a5,gp,-108 # 2006606c <xPendedTicks>
2005d57e:	4398                	c.lw	a4,0(a5)
2005d580:	4401                	c.li	s0,0
2005d582:	0705                	c.addi	a4,1
2005d584:	c398                	c.sw	a4,0(a5)
2005d586:	df7fe0ef          	jal	ra,2005c37c <vApplicationTickHook>
2005d58a:	b795                	c.j	2005d4ee <xTaskIncrementTick+0x98>

2005d58c <vTaskSwitchContext>:
2005d58c:	f801a703          	lw	a4,-128(gp) # 20066058 <uxSchedulerSuspended>
2005d590:	fa018793          	addi	a5,gp,-96 # 20066078 <xYieldPending>
2005d594:	c701                	c.beqz	a4,2005d59c <vTaskSwitchContext+0x10>
2005d596:	4705                	c.li	a4,1
2005d598:	c398                	c.sw	a4,0(a5)
2005d59a:	8082                	c.jr	ra
2005d59c:	1141                	c.addi	sp,-16
2005d59e:	c422                	c.swsp	s0,8(sp)
2005d5a0:	c606                	c.swsp	ra,12(sp)
2005d5a2:	0007a023          	sw	zero,0(a5)
2005d5a6:	410187b7          	lui	a5,0x41018
2005d5aa:	4bd4                	c.lw	a3,20(a5)
2005d5ac:	f7418713          	addi	a4,gp,-140 # 2006604c <ulTotalRunTime>
2005d5b0:	c314                	c.sw	a3,0(a4)
2005d5b2:	f701a583          	lw	a1,-144(gp) # 20066048 <ulTaskSwitchedInTime>
2005d5b6:	4318                	c.lw	a4,0(a4)
2005d5b8:	f7418793          	addi	a5,gp,-140 # 2006604c <ulTotalRunTime>
2005d5bc:	f7018693          	addi	a3,gp,-144 # 20066048 <ulTaskSwitchedInTime>
2005d5c0:	f6418413          	addi	s0,gp,-156 # 2006603c <pxCurrentTCB>
2005d5c4:	00e5f863          	bgeu	a1,a4,2005d5d4 <vTaskSwitchContext+0x48>
2005d5c8:	4388                	c.lw	a0,0(a5)
2005d5ca:	4010                	c.lw	a2,0(s0)
2005d5cc:	5638                	c.lw	a4,104(a2)
2005d5ce:	8f0d                	c.sub	a4,a1
2005d5d0:	972a                	c.add	a4,a0
2005d5d2:	d638                	c.sw	a4,104(a2)
2005d5d4:	439c                	c.lw	a5,0(a5)
2005d5d6:	a5a5a737          	lui	a4,0xa5a5a
2005d5da:	5a570713          	addi	a4,a4,1445 # a5a5a5a5 <__ctrace_end__+0xdba5a5a5>
2005d5de:	c29c                	c.sw	a5,0(a3)
2005d5e0:	401c                	c.lw	a5,0(s0)
2005d5e2:	5b9c                	c.lw	a5,48(a5)
2005d5e4:	4390                	c.lw	a2,0(a5)
2005d5e6:	00e61b63          	bne	a2,a4,2005d5fc <vTaskSwitchContext+0x70>
2005d5ea:	43d4                	c.lw	a3,4(a5)
2005d5ec:	00c69863          	bne	a3,a2,2005d5fc <vTaskSwitchContext+0x70>
2005d5f0:	4798                	c.lw	a4,8(a5)
2005d5f2:	00d71563          	bne	a4,a3,2005d5fc <vTaskSwitchContext+0x70>
2005d5f6:	47dc                	c.lw	a5,12(a5)
2005d5f8:	00e78863          	beq	a5,a4,2005d608 <vTaskSwitchContext+0x7c>
2005d5fc:	4008                	c.lw	a0,0(s0)
2005d5fe:	400c                	c.lw	a1,0(s0)
2005d600:	03458593          	addi	a1,a1,52
2005d604:	dd1fe0ef          	jal	ra,2005c3d4 <vApplicationStackOverflowHook>
2005d608:	f881a503          	lw	a0,-120(gp) # 20066060 <uxTopReadyPriority>
2005d60c:	798030ef          	jal	ra,20060da4 <__clzsi2>
2005d610:	47fd                	c.li	a5,31
2005d612:	40a78533          	sub	a0,a5,a0
2005d616:	4751                	c.li	a4,20
2005d618:	02e50733          	mul	a4,a0,a4
2005d61c:	200687b7          	lui	a5,0x20068
2005d620:	a8878693          	addi	a3,a5,-1400 # 20067a88 <pxReadyTasksLists>
2005d624:	a8878793          	addi	a5,a5,-1400
2005d628:	00e68633          	add	a2,a3,a4
2005d62c:	424c                	c.lw	a1,4(a2)
2005d62e:	0721                	c.addi	a4,8
2005d630:	9736                	c.add	a4,a3
2005d632:	41cc                	c.lw	a1,4(a1)
2005d634:	c24c                	c.sw	a1,4(a2)
2005d636:	00e59463          	bne	a1,a4,2005d63e <vTaskSwitchContext+0xb2>
2005d63a:	41d8                	c.lw	a4,4(a1)
2005d63c:	c258                	c.sw	a4,4(a2)
2005d63e:	4751                	c.li	a4,20
2005d640:	02e50533          	mul	a0,a0,a4
2005d644:	40b2                	c.lwsp	ra,12(sp)
2005d646:	97aa                	c.add	a5,a0
2005d648:	43dc                	c.lw	a5,4(a5)
2005d64a:	47dc                	c.lw	a5,12(a5)
2005d64c:	c01c                	c.sw	a5,0(s0)
2005d64e:	401c                	c.lw	a5,0(s0)
2005d650:	4422                	c.lwsp	s0,8(sp)
2005d652:	06c78793          	addi	a5,a5,108
2005d656:	eaf1a223          	sw	a5,-348(gp) # 20065f7c <_impure_ptr>
2005d65a:	0141                	c.addi	sp,16
2005d65c:	8082                	c.jr	ra

2005d65e <vTaskPlaceOnEventList>:
2005d65e:	1141                	c.addi	sp,-16
2005d660:	c422                	c.swsp	s0,8(sp)
2005d662:	842e                	c.mv	s0,a1
2005d664:	f641a583          	lw	a1,-156(gp) # 2006603c <pxCurrentTCB>
2005d668:	c606                	c.swsp	ra,12(sp)
2005d66a:	05e1                	c.addi	a1,24
2005d66c:	d02fd0ef          	jal	ra,2005ab6e <vListInsert>
2005d670:	8522                	c.mv	a0,s0
2005d672:	4422                	c.lwsp	s0,8(sp)
2005d674:	40b2                	c.lwsp	ra,12(sp)
2005d676:	4585                	c.li	a1,1
2005d678:	0141                	c.addi	sp,16
2005d67a:	b21ff06f          	jal	zero,2005d19a <prvAddCurrentTaskToDelayedList>

2005d67e <vTaskPlaceOnEventListRestricted>:
2005d67e:	1141                	c.addi	sp,-16
2005d680:	c422                	c.swsp	s0,8(sp)
2005d682:	842e                	c.mv	s0,a1
2005d684:	f641a583          	lw	a1,-156(gp) # 2006603c <pxCurrentTCB>
2005d688:	c226                	c.swsp	s1,4(sp)
2005d68a:	84b2                	c.mv	s1,a2
2005d68c:	05e1                	c.addi	a1,24
2005d68e:	c606                	c.swsp	ra,12(sp)
2005d690:	cc8fd0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005d694:	c091                	c.beqz	s1,2005d698 <vTaskPlaceOnEventListRestricted+0x1a>
2005d696:	547d                	c.li	s0,-1
2005d698:	8522                	c.mv	a0,s0
2005d69a:	4422                	c.lwsp	s0,8(sp)
2005d69c:	40b2                	c.lwsp	ra,12(sp)
2005d69e:	85a6                	c.mv	a1,s1
2005d6a0:	4492                	c.lwsp	s1,4(sp)
2005d6a2:	0141                	c.addi	sp,16
2005d6a4:	af7ff06f          	jal	zero,2005d19a <prvAddCurrentTaskToDelayedList>

2005d6a8 <xTaskRemoveFromEventList>:
2005d6a8:	1101                	c.addi	sp,-32
2005d6aa:	ce06                	c.swsp	ra,28(sp)
2005d6ac:	cc22                	c.swsp	s0,24(sp)
2005d6ae:	455c                	c.lw	a5,12(a0)
2005d6b0:	47c0                	c.lw	s0,12(a5)
2005d6b2:	01840593          	addi	a1,s0,24
2005d6b6:	852e                	c.mv	a0,a1
2005d6b8:	c62e                	c.swsp	a1,12(sp)
2005d6ba:	ce2fd0ef          	jal	ra,2005ab9c <uxListRemove>
2005d6be:	f801a783          	lw	a5,-128(gp) # 20066058 <uxSchedulerSuspended>
2005d6c2:	45b2                	c.lwsp	a1,12(sp)
2005d6c4:	efa1                	c.bnez	a5,2005d71c <xTaskRemoveFromEventList+0x74>
2005d6c6:	00440593          	addi	a1,s0,4
2005d6ca:	852e                	c.mv	a0,a1
2005d6cc:	c62e                	c.swsp	a1,12(sp)
2005d6ce:	ccefd0ef          	jal	ra,2005ab9c <uxListRemove>
2005d6d2:	f8818713          	addi	a4,gp,-120 # 20066060 <uxTopReadyPriority>
2005d6d6:	5448                	c.lw	a0,44(s0)
2005d6d8:	4314                	c.lw	a3,0(a4)
2005d6da:	4785                	c.li	a5,1
2005d6dc:	00a797b3          	sll	a5,a5,a0
2005d6e0:	8fd5                	c.or	a5,a3
2005d6e2:	c31c                	c.sw	a5,0(a4)
2005d6e4:	47d1                	c.li	a5,20
2005d6e6:	02f50533          	mul	a0,a0,a5
2005d6ea:	45b2                	c.lwsp	a1,12(sp)
2005d6ec:	200687b7          	lui	a5,0x20068
2005d6f0:	a8878793          	addi	a5,a5,-1400 # 20067a88 <pxReadyTasksLists>
2005d6f4:	953e                	c.add	a0,a5
2005d6f6:	c62fd0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005d6fa:	bc5ff0ef          	jal	ra,2005d2be <prvResetNextTaskUnblockTime>
2005d6fe:	f641a783          	lw	a5,-156(gp) # 2006603c <pxCurrentTCB>
2005d702:	5458                	c.lw	a4,44(s0)
2005d704:	4501                	c.li	a0,0
2005d706:	57dc                	c.lw	a5,44(a5)
2005d708:	00e7f663          	bgeu	a5,a4,2005d714 <xTaskRemoveFromEventList+0x6c>
2005d70c:	4705                	c.li	a4,1
2005d70e:	fae1a023          	sw	a4,-96(gp) # 20066078 <xYieldPending>
2005d712:	4505                	c.li	a0,1
2005d714:	40f2                	c.lwsp	ra,28(sp)
2005d716:	4462                	c.lwsp	s0,24(sp)
2005d718:	6105                	c.addi16sp	sp,32
2005d71a:	8082                	c.jr	ra
2005d71c:	20068537          	lui	a0,0x20068
2005d720:	b8c50513          	addi	a0,a0,-1140 # 20067b8c <xPendingReadyList>
2005d724:	c34fd0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005d728:	bfd9                	c.j	2005d6fe <xTaskRemoveFromEventList+0x56>

2005d72a <vTaskInternalSetTimeOutState>:
2005d72a:	f901a783          	lw	a5,-112(gp) # 20066068 <xNumOfOverflows>
2005d72e:	c11c                	c.sw	a5,0(a0)
2005d730:	f9c1a783          	lw	a5,-100(gp) # 20066074 <xTickCount>
2005d734:	c15c                	c.sw	a5,4(a0)
2005d736:	8082                	c.jr	ra

2005d738 <vTaskMissedYield>:
2005d738:	4705                	c.li	a4,1
2005d73a:	fae1a023          	sw	a4,-96(gp) # 20066078 <xYieldPending>
2005d73e:	8082                	c.jr	ra

2005d740 <eTaskConfirmSleepModeStatus>:
2005d740:	200687b7          	lui	a5,0x20068
2005d744:	b8c7a783          	lw	a5,-1140(a5) # 20067b8c <xPendingReadyList>
2005d748:	4501                	c.li	a0,0
2005d74a:	e395                	c.bnez	a5,2005d76e <eTaskConfirmSleepModeStatus+0x2e>
2005d74c:	fa01a783          	lw	a5,-96(gp) # 20066078 <xYieldPending>
2005d750:	ef99                	c.bnez	a5,2005d76e <eTaskConfirmSleepModeStatus+0x2e>
2005d752:	f941a783          	lw	a5,-108(gp) # 2006606c <xPendedTicks>
2005d756:	ef81                	c.bnez	a5,2005d76e <eTaskConfirmSleepModeStatus+0x2e>
2005d758:	200687b7          	lui	a5,0x20068
2005d75c:	ba07a703          	lw	a4,-1120(a5) # 20067ba0 <xSuspendedTaskList>
2005d760:	f781a783          	lw	a5,-136(gp) # 20066050 <uxCurrentNumberOfTasks>
2005d764:	4505                	c.li	a0,1
2005d766:	17fd                	c.addi	a5,-1
2005d768:	00f71363          	bne	a4,a5,2005d76e <eTaskConfirmSleepModeStatus+0x2e>
2005d76c:	4509                	c.li	a0,2
2005d76e:	8082                	c.jr	ra

2005d770 <xTaskGetCurrentTaskHandle>:
2005d770:	f641a503          	lw	a0,-156(gp) # 2006603c <pxCurrentTCB>
2005d774:	8082                	c.jr	ra

2005d776 <xTaskGetSchedulerState>:
2005d776:	f981a783          	lw	a5,-104(gp) # 20066070 <xSchedulerRunning>
2005d77a:	4505                	c.li	a0,1
2005d77c:	c791                	c.beqz	a5,2005d788 <xTaskGetSchedulerState+0x12>
2005d77e:	f801a503          	lw	a0,-128(gp) # 20066058 <uxSchedulerSuspended>
2005d782:	00153513          	sltiu	a0,a0,1
2005d786:	0506                	c.slli	a0,0x1
2005d788:	8082                	c.jr	ra

2005d78a <xTaskPriorityInherit>:
2005d78a:	c945                	c.beqz	a0,2005d83a <xTaskPriorityInherit+0xb0>
2005d78c:	1101                	c.addi	sp,-32
2005d78e:	ca26                	c.swsp	s1,20(sp)
2005d790:	cc22                	c.swsp	s0,24(sp)
2005d792:	ce06                	c.swsp	ra,28(sp)
2005d794:	c84a                	c.swsp	s2,16(sp)
2005d796:	f6418713          	addi	a4,gp,-156 # 2006603c <pxCurrentTCB>
2005d79a:	4314                	c.lw	a3,0(a4)
2005d79c:	555c                	c.lw	a5,44(a0)
2005d79e:	842a                	c.mv	s0,a0
2005d7a0:	56d4                	c.lw	a3,44(a3)
2005d7a2:	f6418493          	addi	s1,gp,-156 # 2006603c <pxCurrentTCB>
2005d7a6:	08d7f463          	bgeu	a5,a3,2005d82e <xTaskPriorityInherit+0xa4>
2005d7aa:	4d18                	c.lw	a4,24(a0)
2005d7ac:	00074763          	blt	a4,zero,2005d7ba <xTaskPriorityInherit+0x30>
2005d7b0:	4098                	c.lw	a4,0(s1)
2005d7b2:	5754                	c.lw	a3,44(a4)
2005d7b4:	472d                	c.li	a4,11
2005d7b6:	8f15                	c.sub	a4,a3
2005d7b8:	cd18                	c.sw	a4,24(a0)
2005d7ba:	46d1                	c.li	a3,20
2005d7bc:	02d787b3          	mul	a5,a5,a3
2005d7c0:	20068537          	lui	a0,0x20068
2005d7c4:	a8850713          	addi	a4,a0,-1400 # 20067a88 <pxReadyTasksLists>
2005d7c8:	a8850913          	addi	s2,a0,-1400
2005d7cc:	97ba                	c.add	a5,a4
2005d7ce:	4858                	c.lw	a4,20(s0)
2005d7d0:	04f71b63          	bne	a4,a5,2005d826 <xTaskPriorityInherit+0x9c>
2005d7d4:	00440593          	addi	a1,s0,4
2005d7d8:	852e                	c.mv	a0,a1
2005d7da:	c62e                	c.swsp	a1,12(sp)
2005d7dc:	bc0fd0ef          	jal	ra,2005ab9c <uxListRemove>
2005d7e0:	45b2                	c.lwsp	a1,12(sp)
2005d7e2:	f8818713          	addi	a4,gp,-120 # 20066060 <uxTopReadyPriority>
2005d7e6:	e911                	c.bnez	a0,2005d7fa <xTaskPriorityInherit+0x70>
2005d7e8:	5450                	c.lw	a2,44(s0)
2005d7ea:	4314                	c.lw	a3,0(a4)
2005d7ec:	4785                	c.li	a5,1
2005d7ee:	00c797b3          	sll	a5,a5,a2
2005d7f2:	fff7c793          	xori	a5,a5,-1
2005d7f6:	8ff5                	c.and	a5,a3
2005d7f8:	c31c                	c.sw	a5,0(a4)
2005d7fa:	409c                	c.lw	a5,0(s1)
2005d7fc:	4314                	c.lw	a3,0(a4)
2005d7fe:	57c8                	c.lw	a0,44(a5)
2005d800:	4785                	c.li	a5,1
2005d802:	00a797b3          	sll	a5,a5,a0
2005d806:	8fd5                	c.or	a5,a3
2005d808:	c31c                	c.sw	a5,0(a4)
2005d80a:	47d1                	c.li	a5,20
2005d80c:	d448                	c.sw	a0,44(s0)
2005d80e:	02f50533          	mul	a0,a0,a5
2005d812:	954a                	c.add	a0,s2
2005d814:	b44fd0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005d818:	4505                	c.li	a0,1
2005d81a:	40f2                	c.lwsp	ra,28(sp)
2005d81c:	4462                	c.lwsp	s0,24(sp)
2005d81e:	44d2                	c.lwsp	s1,20(sp)
2005d820:	4942                	c.lwsp	s2,16(sp)
2005d822:	6105                	c.addi16sp	sp,32
2005d824:	8082                	c.jr	ra
2005d826:	409c                	c.lw	a5,0(s1)
2005d828:	57dc                	c.lw	a5,44(a5)
2005d82a:	d45c                	c.sw	a5,44(s0)
2005d82c:	b7f5                	c.j	2005d818 <xTaskPriorityInherit+0x8e>
2005d82e:	431c                	c.lw	a5,0(a4)
2005d830:	4d68                	c.lw	a0,92(a0)
2005d832:	57dc                	c.lw	a5,44(a5)
2005d834:	00f53533          	sltu	a0,a0,a5
2005d838:	b7cd                	c.j	2005d81a <xTaskPriorityInherit+0x90>
2005d83a:	4501                	c.li	a0,0
2005d83c:	8082                	c.jr	ra

2005d83e <xTaskPriorityDisinherit>:
2005d83e:	e901                	c.bnez	a0,2005d84e <xTaskPriorityDisinherit+0x10>
2005d840:	4501                	c.li	a0,0
2005d842:	8082                	c.jr	ra
2005d844:	4501                	c.li	a0,0
2005d846:	40f2                	c.lwsp	ra,28(sp)
2005d848:	4462                	c.lwsp	s0,24(sp)
2005d84a:	6105                	c.addi16sp	sp,32
2005d84c:	8082                	c.jr	ra
2005d84e:	1101                	c.addi	sp,-32
2005d850:	cc22                	c.swsp	s0,24(sp)
2005d852:	ce06                	c.swsp	ra,28(sp)
2005d854:	513c                	c.lw	a5,96(a0)
2005d856:	5554                	c.lw	a3,44(a0)
2005d858:	4d78                	c.lw	a4,92(a0)
2005d85a:	17fd                	c.addi	a5,-1
2005d85c:	d13c                	c.sw	a5,96(a0)
2005d85e:	842a                	c.mv	s0,a0
2005d860:	fee682e3          	beq	a3,a4,2005d844 <xTaskPriorityDisinherit+0x6>
2005d864:	f3e5                	c.bnez	a5,2005d844 <xTaskPriorityDisinherit+0x6>
2005d866:	00450593          	addi	a1,a0,4
2005d86a:	852e                	c.mv	a0,a1
2005d86c:	c62e                	c.swsp	a1,12(sp)
2005d86e:	b2efd0ef          	jal	ra,2005ab9c <uxListRemove>
2005d872:	45b2                	c.lwsp	a1,12(sp)
2005d874:	f8818713          	addi	a4,gp,-120 # 20066060 <uxTopReadyPriority>
2005d878:	e911                	c.bnez	a0,2005d88c <xTaskPriorityDisinherit+0x4e>
2005d87a:	5450                	c.lw	a2,44(s0)
2005d87c:	4314                	c.lw	a3,0(a4)
2005d87e:	4785                	c.li	a5,1
2005d880:	00c797b3          	sll	a5,a5,a2
2005d884:	fff7c793          	xori	a5,a5,-1
2005d888:	8ff5                	c.and	a5,a3
2005d88a:	c31c                	c.sw	a5,0(a4)
2005d88c:	4c68                	c.lw	a0,92(s0)
2005d88e:	47ad                	c.li	a5,11
2005d890:	4314                	c.lw	a3,0(a4)
2005d892:	8f89                	c.sub	a5,a0
2005d894:	cc1c                	c.sw	a5,24(s0)
2005d896:	4785                	c.li	a5,1
2005d898:	00a797b3          	sll	a5,a5,a0
2005d89c:	8fd5                	c.or	a5,a3
2005d89e:	c31c                	c.sw	a5,0(a4)
2005d8a0:	47d1                	c.li	a5,20
2005d8a2:	d448                	c.sw	a0,44(s0)
2005d8a4:	02f50533          	mul	a0,a0,a5
2005d8a8:	200687b7          	lui	a5,0x20068
2005d8ac:	a8878793          	addi	a5,a5,-1400 # 20067a88 <pxReadyTasksLists>
2005d8b0:	953e                	c.add	a0,a5
2005d8b2:	aa6fd0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005d8b6:	4505                	c.li	a0,1
2005d8b8:	b779                	c.j	2005d846 <xTaskPriorityDisinherit+0x8>

2005d8ba <vTaskPriorityDisinheritAfterTimeout>:
2005d8ba:	cd51                	c.beqz	a0,2005d956 <vTaskPriorityDisinheritAfterTimeout+0x9c>
2005d8bc:	1101                	c.addi	sp,-32
2005d8be:	cc22                	c.swsp	s0,24(sp)
2005d8c0:	ce06                	c.swsp	ra,28(sp)
2005d8c2:	ca26                	c.swsp	s1,20(sp)
2005d8c4:	4d7c                	c.lw	a5,92(a0)
2005d8c6:	842a                	c.mv	s0,a0
2005d8c8:	00b7f363          	bgeu	a5,a1,2005d8ce <vTaskPriorityDisinheritAfterTimeout+0x14>
2005d8cc:	87ae                	c.mv	a5,a1
2005d8ce:	5458                	c.lw	a4,44(s0)
2005d8d0:	06f70e63          	beq	a4,a5,2005d94c <vTaskPriorityDisinheritAfterTimeout+0x92>
2005d8d4:	5030                	c.lw	a2,96(s0)
2005d8d6:	4685                	c.li	a3,1
2005d8d8:	06d61a63          	bne	a2,a3,2005d94c <vTaskPriorityDisinheritAfterTimeout+0x92>
2005d8dc:	4c14                	c.lw	a3,24(s0)
2005d8de:	d45c                	c.sw	a5,44(s0)
2005d8e0:	0006c663          	blt	a3,zero,2005d8ec <vTaskPriorityDisinheritAfterTimeout+0x32>
2005d8e4:	46ad                	c.li	a3,11
2005d8e6:	40f687b3          	sub	a5,a3,a5
2005d8ea:	cc1c                	c.sw	a5,24(s0)
2005d8ec:	46d1                	c.li	a3,20
2005d8ee:	02d70733          	mul	a4,a4,a3
2005d8f2:	20068537          	lui	a0,0x20068
2005d8f6:	a8850793          	addi	a5,a0,-1400 # 20067a88 <pxReadyTasksLists>
2005d8fa:	a8850493          	addi	s1,a0,-1400
2005d8fe:	973e                	c.add	a4,a5
2005d900:	485c                	c.lw	a5,20(s0)
2005d902:	04e79563          	bne	a5,a4,2005d94c <vTaskPriorityDisinheritAfterTimeout+0x92>
2005d906:	00440593          	addi	a1,s0,4
2005d90a:	852e                	c.mv	a0,a1
2005d90c:	c62e                	c.swsp	a1,12(sp)
2005d90e:	a8efd0ef          	jal	ra,2005ab9c <uxListRemove>
2005d912:	5454                	c.lw	a3,44(s0)
2005d914:	45b2                	c.lwsp	a1,12(sp)
2005d916:	f8818713          	addi	a4,gp,-120 # 20066060 <uxTopReadyPriority>
2005d91a:	e909                	c.bnez	a0,2005d92c <vTaskPriorityDisinheritAfterTimeout+0x72>
2005d91c:	4310                	c.lw	a2,0(a4)
2005d91e:	4785                	c.li	a5,1
2005d920:	00d797b3          	sll	a5,a5,a3
2005d924:	fff7c793          	xori	a5,a5,-1
2005d928:	8ff1                	c.and	a5,a2
2005d92a:	c31c                	c.sw	a5,0(a4)
2005d92c:	4551                	c.li	a0,20
2005d92e:	02a68533          	mul	a0,a3,a0
2005d932:	4310                	c.lw	a2,0(a4)
2005d934:	4785                	c.li	a5,1
2005d936:	00d797b3          	sll	a5,a5,a3
2005d93a:	8fd1                	c.or	a5,a2
2005d93c:	c31c                	c.sw	a5,0(a4)
2005d93e:	4462                	c.lwsp	s0,24(sp)
2005d940:	40f2                	c.lwsp	ra,28(sp)
2005d942:	9526                	c.add	a0,s1
2005d944:	44d2                	c.lwsp	s1,20(sp)
2005d946:	6105                	c.addi16sp	sp,32
2005d948:	a10fd06f          	jal	zero,2005ab58 <vListInsertEnd>
2005d94c:	40f2                	c.lwsp	ra,28(sp)
2005d94e:	4462                	c.lwsp	s0,24(sp)
2005d950:	44d2                	c.lwsp	s1,20(sp)
2005d952:	6105                	c.addi16sp	sp,32
2005d954:	8082                	c.jr	ra
2005d956:	8082                	c.jr	ra

2005d958 <vTaskEnterCritical>:
2005d958:	30047073          	csrrci	zero,mstatus,8
2005d95c:	f981a783          	lw	a5,-104(gp) # 20066070 <xSchedulerRunning>
2005d960:	cb81                	c.beqz	a5,2005d970 <vTaskEnterCritical+0x18>
2005d962:	f6418793          	addi	a5,gp,-156 # 2006603c <pxCurrentTCB>
2005d966:	4394                	c.lw	a3,0(a5)
2005d968:	439c                	c.lw	a5,0(a5)
2005d96a:	4ab8                	c.lw	a4,80(a3)
2005d96c:	0705                	c.addi	a4,1
2005d96e:	cab8                	c.sw	a4,80(a3)
2005d970:	8082                	c.jr	ra

2005d972 <vTaskExitCritical>:
2005d972:	f981a783          	lw	a5,-104(gp) # 20066070 <xSchedulerRunning>
2005d976:	cf99                	c.beqz	a5,2005d994 <vTaskExitCritical+0x22>
2005d978:	f6418793          	addi	a5,gp,-156 # 2006603c <pxCurrentTCB>
2005d97c:	4398                	c.lw	a4,0(a5)
2005d97e:	4b38                	c.lw	a4,80(a4)
2005d980:	cb11                	c.beqz	a4,2005d994 <vTaskExitCritical+0x22>
2005d982:	4394                	c.lw	a3,0(a5)
2005d984:	439c                	c.lw	a5,0(a5)
2005d986:	4ab8                	c.lw	a4,80(a3)
2005d988:	177d                	c.addi	a4,-1
2005d98a:	cab8                	c.sw	a4,80(a3)
2005d98c:	4bbc                	c.lw	a5,80(a5)
2005d98e:	e399                	c.bnez	a5,2005d994 <vTaskExitCritical+0x22>
2005d990:	30046073          	csrrsi	zero,mstatus,8
2005d994:	8082                	c.jr	ra

2005d996 <prvAddNewTaskToReadyList>:
2005d996:	1101                	c.addi	sp,-32
2005d998:	cc22                	c.swsp	s0,24(sp)
2005d99a:	ca26                	c.swsp	s1,20(sp)
2005d99c:	c84a                	c.swsp	s2,16(sp)
2005d99e:	c64e                	c.swsp	s3,12(sp)
2005d9a0:	ce06                	c.swsp	ra,28(sp)
2005d9a2:	c452                	c.swsp	s4,8(sp)
2005d9a4:	842a                	c.mv	s0,a0
2005d9a6:	fb3ff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005d9aa:	f7818793          	addi	a5,gp,-136 # 20066050 <uxCurrentNumberOfTasks>
2005d9ae:	4398                	c.lw	a4,0(a5)
2005d9b0:	20068937          	lui	s2,0x20068
2005d9b4:	0705                	c.addi	a4,1
2005d9b6:	c398                	c.sw	a4,0(a5)
2005d9b8:	f6418713          	addi	a4,gp,-156 # 2006603c <pxCurrentTCB>
2005d9bc:	4318                	c.lw	a4,0(a4)
2005d9be:	f6418493          	addi	s1,gp,-156 # 2006603c <pxCurrentTCB>
2005d9c2:	a8890993          	addi	s3,s2,-1400 # 20067a88 <pxReadyTasksLists>
2005d9c6:	e369                	c.bnez	a4,2005da88 <prvAddNewTaskToReadyList+0xf2>
2005d9c8:	c080                	c.sw	s0,0(s1)
2005d9ca:	4398                	c.lw	a4,0(a5)
2005d9cc:	4785                	c.li	a5,1
2005d9ce:	06f71263          	bne	a4,a5,2005da32 <prvAddNewTaskToReadyList+0x9c>
2005d9d2:	a8890913          	addi	s2,s2,-1400
2005d9d6:	0dc98a13          	addi	s4,s3,220
2005d9da:	854a                	c.mv	a0,s2
2005d9dc:	0951                	c.addi	s2,20
2005d9de:	960fd0ef          	jal	ra,2005ab3e <vListInitialise>
2005d9e2:	ff2a1ce3          	bne	s4,s2,2005d9da <prvAddNewTaskToReadyList+0x44>
2005d9e6:	20068537          	lui	a0,0x20068
2005d9ea:	b6450a13          	addi	s4,a0,-1180 # 20067b64 <xDelayedTaskList1>
2005d9ee:	b6450513          	addi	a0,a0,-1180
2005d9f2:	94cfd0ef          	jal	ra,2005ab3e <vListInitialise>
2005d9f6:	20068537          	lui	a0,0x20068
2005d9fa:	b7850913          	addi	s2,a0,-1160 # 20067b78 <xDelayedTaskList2>
2005d9fe:	b7850513          	addi	a0,a0,-1160
2005da02:	93cfd0ef          	jal	ra,2005ab3e <vListInitialise>
2005da06:	20068537          	lui	a0,0x20068
2005da0a:	b8c50513          	addi	a0,a0,-1140 # 20067b8c <xPendingReadyList>
2005da0e:	930fd0ef          	jal	ra,2005ab3e <vListInitialise>
2005da12:	20068537          	lui	a0,0x20068
2005da16:	bb450513          	addi	a0,a0,-1100 # 20067bb4 <xTasksWaitingTermination>
2005da1a:	924fd0ef          	jal	ra,2005ab3e <vListInitialise>
2005da1e:	20068537          	lui	a0,0x20068
2005da22:	ba050513          	addi	a0,a0,-1120 # 20067ba0 <xSuspendedTaskList>
2005da26:	918fd0ef          	jal	ra,2005ab3e <vListInitialise>
2005da2a:	f741a423          	sw	s4,-152(gp) # 20066040 <pxDelayedTaskList>
2005da2e:	f721a623          	sw	s2,-148(gp) # 20066044 <pxOverflowDelayedTaskList>
2005da32:	f8418713          	addi	a4,gp,-124 # 2006605c <uxTaskNumber>
2005da36:	431c                	c.lw	a5,0(a4)
2005da38:	5448                	c.lw	a0,44(s0)
2005da3a:	00440593          	addi	a1,s0,4
2005da3e:	0785                	c.addi	a5,1
2005da40:	c31c                	c.sw	a5,0(a4)
2005da42:	f8818713          	addi	a4,gp,-120 # 20066060 <uxTopReadyPriority>
2005da46:	4314                	c.lw	a3,0(a4)
2005da48:	c87c                	c.sw	a5,84(s0)
2005da4a:	4785                	c.li	a5,1
2005da4c:	00a797b3          	sll	a5,a5,a0
2005da50:	8fd5                	c.or	a5,a3
2005da52:	c31c                	c.sw	a5,0(a4)
2005da54:	47d1                	c.li	a5,20
2005da56:	02f50533          	mul	a0,a0,a5
2005da5a:	954e                	c.add	a0,s3
2005da5c:	8fcfd0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005da60:	f13ff0ef          	jal	ra,2005d972 <vTaskExitCritical>
2005da64:	f981a783          	lw	a5,-104(gp) # 20066070 <xSchedulerRunning>
2005da68:	cb81                	c.beqz	a5,2005da78 <prvAddNewTaskToReadyList+0xe2>
2005da6a:	409c                	c.lw	a5,0(s1)
2005da6c:	57d8                	c.lw	a4,44(a5)
2005da6e:	545c                	c.lw	a5,44(s0)
2005da70:	00f77463          	bgeu	a4,a5,2005da78 <prvAddNewTaskToReadyList+0xe2>
2005da74:	00000073          	ecall
2005da78:	40f2                	c.lwsp	ra,28(sp)
2005da7a:	4462                	c.lwsp	s0,24(sp)
2005da7c:	44d2                	c.lwsp	s1,20(sp)
2005da7e:	4942                	c.lwsp	s2,16(sp)
2005da80:	49b2                	c.lwsp	s3,12(sp)
2005da82:	4a22                	c.lwsp	s4,8(sp)
2005da84:	6105                	c.addi16sp	sp,32
2005da86:	8082                	c.jr	ra
2005da88:	f981a783          	lw	a5,-104(gp) # 20066070 <xSchedulerRunning>
2005da8c:	f3dd                	c.bnez	a5,2005da32 <prvAddNewTaskToReadyList+0x9c>
2005da8e:	409c                	c.lw	a5,0(s1)
2005da90:	57d8                	c.lw	a4,44(a5)
2005da92:	545c                	c.lw	a5,44(s0)
2005da94:	f8e7efe3          	bltu	a5,a4,2005da32 <prvAddNewTaskToReadyList+0x9c>
2005da98:	c080                	c.sw	s0,0(s1)
2005da9a:	bf61                	c.j	2005da32 <prvAddNewTaskToReadyList+0x9c>

2005da9c <xTaskCreateStatic>:
2005da9c:	1101                	c.addi	sp,-32
2005da9e:	cc22                	c.swsp	s0,24(sp)
2005daa0:	ce06                	c.swsp	ra,28(sp)
2005daa2:	8442                	c.mv	s0,a6
2005daa4:	00080f63          	beq	a6,zero,2005dac2 <xTaskCreateStatic+0x26>
2005daa8:	c395                	c.beqz	a5,2005dacc <xTaskCreateStatic+0x30>
2005daaa:	02f82823          	sw	a5,48(a6)
2005daae:	4789                	c.li	a5,2
2005dab0:	16f800a3          	sb	a5,353(a6)
2005dab4:	007c                	c.addi4spn	a5,sp,12
2005dab6:	85bff0ef          	jal	ra,2005d310 <prvInitialiseNewTask.constprop.0>
2005daba:	8522                	c.mv	a0,s0
2005dabc:	edbff0ef          	jal	ra,2005d996 <prvAddNewTaskToReadyList>
2005dac0:	4432                	c.lwsp	s0,12(sp)
2005dac2:	40f2                	c.lwsp	ra,28(sp)
2005dac4:	8522                	c.mv	a0,s0
2005dac6:	4462                	c.lwsp	s0,24(sp)
2005dac8:	6105                	c.addi16sp	sp,32
2005daca:	8082                	c.jr	ra
2005dacc:	4401                	c.li	s0,0
2005dace:	bfd5                	c.j	2005dac2 <xTaskCreateStatic+0x26>

2005dad0 <vTaskStartScheduler>:
2005dad0:	1101                	c.addi	sp,-32
2005dad2:	0070                	c.addi4spn	a2,sp,12
2005dad4:	002c                	c.addi4spn	a1,sp,8
2005dad6:	0048                	c.addi4spn	a0,sp,4
2005dad8:	ce06                	c.swsp	ra,28(sp)
2005dada:	c202                	c.swsp	zero,4(sp)
2005dadc:	c402                	c.swsp	zero,8(sp)
2005dade:	913fe0ef          	jal	ra,2005c3f0 <vApplicationGetIdleTaskMemory>
2005dae2:	4812                	c.lwsp	a6,4(sp)
2005dae4:	47a2                	c.lwsp	a5,8(sp)
2005dae6:	4632                	c.lwsp	a2,12(sp)
2005dae8:	200635b7          	lui	a1,0x20063
2005daec:	2005e537          	lui	a0,0x2005e
2005daf0:	4701                	c.li	a4,0
2005daf2:	4681                	c.li	a3,0
2005daf4:	e6c58593          	addi	a1,a1,-404 # 20062e6c <__func__.1+0x18>
2005daf8:	df450513          	addi	a0,a0,-524 # 2005ddf4 <prvIdleTask>
2005dafc:	fa1ff0ef          	jal	ra,2005da9c <xTaskCreateStatic>
2005db00:	c51d                	c.beqz	a0,2005db2e <vTaskStartScheduler+0x5e>
2005db02:	76a000ef          	jal	ra,2005e26c <xTimerCreateTimerTask>
2005db06:	4785                	c.li	a5,1
2005db08:	02f51363          	bne	a0,a5,2005db2e <vTaskStartScheduler+0x5e>
2005db0c:	30047073          	csrrci	zero,mstatus,8
2005db10:	f641a783          	lw	a5,-156(gp) # 2006603c <pxCurrentTCB>
2005db14:	06c78793          	addi	a5,a5,108
2005db18:	eaf1a223          	sw	a5,-348(gp) # 20065f7c <_impure_ptr>
2005db1c:	577d                	c.li	a4,-1
2005db1e:	f8e1a623          	sw	a4,-116(gp) # 20066064 <xNextTaskUnblockTime>
2005db22:	f8a1ac23          	sw	a0,-104(gp) # 20066070 <xSchedulerRunning>
2005db26:	f801ae23          	sw	zero,-100(gp) # 20066074 <xTickCount>
2005db2a:	835fe0ef          	jal	ra,2005c35e <xPortStartScheduler>
2005db2e:	40f2                	c.lwsp	ra,28(sp)
2005db30:	e9c1a783          	lw	a5,-356(gp) # 20065f74 <uxTopUsedPriority>
2005db34:	6105                	c.addi16sp	sp,32
2005db36:	8082                	c.jr	ra

2005db38 <xTaskCreate>:
2005db38:	7179                	c.addi16sp	sp,-48
2005db3a:	ce4e                	c.swsp	s3,28(sp)
2005db3c:	89aa                	c.mv	s3,a0
2005db3e:	00261513          	slli	a0,a2,0x2
2005db42:	d04a                	c.swsp	s2,32(sp)
2005db44:	cc52                	c.swsp	s4,24(sp)
2005db46:	ca56                	c.swsp	s5,20(sp)
2005db48:	c85a                	c.swsp	s6,16(sp)
2005db4a:	c65e                	c.swsp	s7,12(sp)
2005db4c:	d606                	c.swsp	ra,44(sp)
2005db4e:	d422                	c.swsp	s0,40(sp)
2005db50:	d226                	c.swsp	s1,36(sp)
2005db52:	8a2e                	c.mv	s4,a1
2005db54:	8932                	c.mv	s2,a2
2005db56:	8ab6                	c.mv	s5,a3
2005db58:	8b3a                	c.mv	s6,a4
2005db5a:	8bbe                	c.mv	s7,a5
2005db5c:	da9fc0ef          	jal	ra,2005a904 <pvPortMalloc>
2005db60:	c531                	c.beqz	a0,2005dbac <xTaskCreate+0x74>
2005db62:	84aa                	c.mv	s1,a0
2005db64:	16400513          	addi	a0,zero,356
2005db68:	d9dfc0ef          	jal	ra,2005a904 <pvPortMalloc>
2005db6c:	842a                	c.mv	s0,a0
2005db6e:	cd05                	c.beqz	a0,2005dba6 <xTaskCreate+0x6e>
2005db70:	882a                	c.mv	a6,a0
2005db72:	d904                	c.sw	s1,48(a0)
2005db74:	160500a3          	sb	zero,353(a0)
2005db78:	87de                	c.mv	a5,s7
2005db7a:	875a                	c.mv	a4,s6
2005db7c:	86d6                	c.mv	a3,s5
2005db7e:	864a                	c.mv	a2,s2
2005db80:	85d2                	c.mv	a1,s4
2005db82:	854e                	c.mv	a0,s3
2005db84:	f8cff0ef          	jal	ra,2005d310 <prvInitialiseNewTask.constprop.0>
2005db88:	8522                	c.mv	a0,s0
2005db8a:	e0dff0ef          	jal	ra,2005d996 <prvAddNewTaskToReadyList>
2005db8e:	4505                	c.li	a0,1
2005db90:	50b2                	c.lwsp	ra,44(sp)
2005db92:	5422                	c.lwsp	s0,40(sp)
2005db94:	5492                	c.lwsp	s1,36(sp)
2005db96:	5902                	c.lwsp	s2,32(sp)
2005db98:	49f2                	c.lwsp	s3,28(sp)
2005db9a:	4a62                	c.lwsp	s4,24(sp)
2005db9c:	4ad2                	c.lwsp	s5,20(sp)
2005db9e:	4b42                	c.lwsp	s6,16(sp)
2005dba0:	4bb2                	c.lwsp	s7,12(sp)
2005dba2:	6145                	c.addi16sp	sp,48
2005dba4:	8082                	c.jr	ra
2005dba6:	8526                	c.mv	a0,s1
2005dba8:	e3bfc0ef          	jal	ra,2005a9e2 <vPortFree>
2005dbac:	557d                	c.li	a0,-1
2005dbae:	b7cd                	c.j	2005db90 <xTaskCreate+0x58>

2005dbb0 <vTaskDelete>:
2005dbb0:	1141                	c.addi	sp,-16
2005dbb2:	c422                	c.swsp	s0,8(sp)
2005dbb4:	c226                	c.swsp	s1,4(sp)
2005dbb6:	842a                	c.mv	s0,a0
2005dbb8:	c606                	c.swsp	ra,12(sp)
2005dbba:	c04a                	c.swsp	s2,0(sp)
2005dbbc:	f6418493          	addi	s1,gp,-156 # 2006603c <pxCurrentTCB>
2005dbc0:	d99ff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005dbc4:	e011                	c.bnez	s0,2005dbc8 <vTaskDelete+0x18>
2005dbc6:	4080                	c.lw	s0,0(s1)
2005dbc8:	00440913          	addi	s2,s0,4
2005dbcc:	854a                	c.mv	a0,s2
2005dbce:	fcffc0ef          	jal	ra,2005ab9c <uxListRemove>
2005dbd2:	e515                	c.bnez	a0,2005dbfe <vTaskDelete+0x4e>
2005dbd4:	5450                	c.lw	a2,44(s0)
2005dbd6:	4751                	c.li	a4,20
2005dbd8:	02e606b3          	mul	a3,a2,a4
2005dbdc:	20068737          	lui	a4,0x20068
2005dbe0:	a8870713          	addi	a4,a4,-1400 # 20067a88 <pxReadyTasksLists>
2005dbe4:	9736                	c.add	a4,a3
2005dbe6:	431c                	c.lw	a5,0(a4)
2005dbe8:	eb99                	c.bnez	a5,2005dbfe <vTaskDelete+0x4e>
2005dbea:	f8818713          	addi	a4,gp,-120 # 20066060 <uxTopReadyPriority>
2005dbee:	4314                	c.lw	a3,0(a4)
2005dbf0:	4785                	c.li	a5,1
2005dbf2:	00c797b3          	sll	a5,a5,a2
2005dbf6:	fff7c793          	xori	a5,a5,-1
2005dbfa:	8ff5                	c.and	a5,a3
2005dbfc:	c31c                	c.sw	a5,0(a4)
2005dbfe:	541c                	c.lw	a5,40(s0)
2005dc00:	c789                	c.beqz	a5,2005dc0a <vTaskDelete+0x5a>
2005dc02:	01840513          	addi	a0,s0,24
2005dc06:	f97fc0ef          	jal	ra,2005ab9c <uxListRemove>
2005dc0a:	f8418793          	addi	a5,gp,-124 # 2006605c <uxTaskNumber>
2005dc0e:	4398                	c.lw	a4,0(a5)
2005dc10:	0705                	c.addi	a4,1
2005dc12:	c398                	c.sw	a4,0(a5)
2005dc14:	409c                	c.lw	a5,0(s1)
2005dc16:	02879e63          	bne	a5,s0,2005dc52 <vTaskDelete+0xa2>
2005dc1a:	20068537          	lui	a0,0x20068
2005dc1e:	85ca                	c.mv	a1,s2
2005dc20:	bb450513          	addi	a0,a0,-1100 # 20067bb4 <xTasksWaitingTermination>
2005dc24:	f35fc0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005dc28:	f7c18793          	addi	a5,gp,-132 # 20066054 <uxDeletedTasksWaitingCleanUp>
2005dc2c:	4398                	c.lw	a4,0(a5)
2005dc2e:	0705                	c.addi	a4,1
2005dc30:	c398                	c.sw	a4,0(a5)
2005dc32:	d41ff0ef          	jal	ra,2005d972 <vTaskExitCritical>
2005dc36:	f981a783          	lw	a5,-104(gp) # 20066070 <xSchedulerRunning>
2005dc3a:	c791                	c.beqz	a5,2005dc46 <vTaskDelete+0x96>
2005dc3c:	409c                	c.lw	a5,0(s1)
2005dc3e:	00879463          	bne	a5,s0,2005dc46 <vTaskDelete+0x96>
2005dc42:	00000073          	ecall
2005dc46:	40b2                	c.lwsp	ra,12(sp)
2005dc48:	4422                	c.lwsp	s0,8(sp)
2005dc4a:	4492                	c.lwsp	s1,4(sp)
2005dc4c:	4902                	c.lwsp	s2,0(sp)
2005dc4e:	0141                	c.addi	sp,16
2005dc50:	8082                	c.jr	ra
2005dc52:	f7818793          	addi	a5,gp,-136 # 20066050 <uxCurrentNumberOfTasks>
2005dc56:	4398                	c.lw	a4,0(a5)
2005dc58:	8522                	c.mv	a0,s0
2005dc5a:	177d                	c.addi	a4,-1
2005dc5c:	c398                	c.sw	a4,0(a5)
2005dc5e:	e7cff0ef          	jal	ra,2005d2da <prvDeleteTCB>
2005dc62:	e5cff0ef          	jal	ra,2005d2be <prvResetNextTaskUnblockTime>
2005dc66:	b7f1                	c.j	2005dc32 <vTaskDelete+0x82>

2005dc68 <eTaskGetState>:
2005dc68:	f641a783          	lw	a5,-156(gp) # 2006603c <pxCurrentTCB>
2005dc6c:	06a78863          	beq	a5,a0,2005dcdc <eTaskGetState+0x74>
2005dc70:	1101                	c.addi	sp,-32
2005dc72:	ce06                	c.swsp	ra,28(sp)
2005dc74:	cc22                	c.swsp	s0,24(sp)
2005dc76:	ca26                	c.swsp	s1,20(sp)
2005dc78:	842a                	c.mv	s0,a0
2005dc7a:	c84a                	c.swsp	s2,16(sp)
2005dc7c:	c64e                	c.swsp	s3,12(sp)
2005dc7e:	cdbff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005dc82:	f681a983          	lw	s3,-152(gp) # 20066040 <pxDelayedTaskList>
2005dc86:	4844                	c.lw	s1,20(s0)
2005dc88:	f6c1a903          	lw	s2,-148(gp) # 20066044 <pxOverflowDelayedTaskList>
2005dc8c:	ce7ff0ef          	jal	ra,2005d972 <vTaskExitCritical>
2005dc90:	4509                	c.li	a0,2
2005dc92:	03348463          	beq	s1,s3,2005dcba <eTaskGetState+0x52>
2005dc96:	03248263          	beq	s1,s2,2005dcba <eTaskGetState+0x52>
2005dc9a:	200687b7          	lui	a5,0x20068
2005dc9e:	ba078793          	addi	a5,a5,-1120 # 20067ba0 <xSuspendedTaskList>
2005dca2:	02f49363          	bne	s1,a5,2005dcc8 <eTaskGetState+0x60>
2005dca6:	541c                	c.lw	a5,40(s0)
2005dca8:	eb89                	c.bnez	a5,2005dcba <eTaskGetState+0x52>
2005dcaa:	16044783          	lbu	a5,352(s0)
2005dcae:	4705                	c.li	a4,1
2005dcb0:	0ff7f793          	andi	a5,a5,255
2005dcb4:	00e78363          	beq	a5,a4,2005dcba <eTaskGetState+0x52>
2005dcb8:	450d                	c.li	a0,3
2005dcba:	40f2                	c.lwsp	ra,28(sp)
2005dcbc:	4462                	c.lwsp	s0,24(sp)
2005dcbe:	44d2                	c.lwsp	s1,20(sp)
2005dcc0:	4942                	c.lwsp	s2,16(sp)
2005dcc2:	49b2                	c.lwsp	s3,12(sp)
2005dcc4:	6105                	c.addi16sp	sp,32
2005dcc6:	8082                	c.jr	ra
2005dcc8:	200687b7          	lui	a5,0x20068
2005dccc:	bb478793          	addi	a5,a5,-1100 # 20067bb4 <xTasksWaitingTermination>
2005dcd0:	4511                	c.li	a0,4
2005dcd2:	fef484e3          	beq	s1,a5,2005dcba <eTaskGetState+0x52>
2005dcd6:	d0f5                	c.beqz	s1,2005dcba <eTaskGetState+0x52>
2005dcd8:	4505                	c.li	a0,1
2005dcda:	b7c5                	c.j	2005dcba <eTaskGetState+0x52>
2005dcdc:	4501                	c.li	a0,0
2005dcde:	8082                	c.jr	ra

2005dce0 <xTaskResumeAll>:
2005dce0:	7139                	c.addi16sp	sp,-64
2005dce2:	de06                	c.swsp	ra,60(sp)
2005dce4:	dc22                	c.swsp	s0,56(sp)
2005dce6:	da26                	c.swsp	s1,52(sp)
2005dce8:	d84a                	c.swsp	s2,48(sp)
2005dcea:	d64e                	c.swsp	s3,44(sp)
2005dcec:	d452                	c.swsp	s4,40(sp)
2005dcee:	d256                	c.swsp	s5,36(sp)
2005dcf0:	d05a                	c.swsp	s6,32(sp)
2005dcf2:	ce5e                	c.swsp	s7,28(sp)
2005dcf4:	c65ff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005dcf8:	f8018793          	addi	a5,gp,-128 # 20066058 <uxSchedulerSuspended>
2005dcfc:	4398                	c.lw	a4,0(a5)
2005dcfe:	177d                	c.addi	a4,-1
2005dd00:	c398                	c.sw	a4,0(a5)
2005dd02:	439c                	c.lw	a5,0(a5)
2005dd04:	c38d                	c.beqz	a5,2005dd26 <xTaskResumeAll+0x46>
2005dd06:	4501                	c.li	a0,0
2005dd08:	c62a                	c.swsp	a0,12(sp)
2005dd0a:	c69ff0ef          	jal	ra,2005d972 <vTaskExitCritical>
2005dd0e:	50f2                	c.lwsp	ra,60(sp)
2005dd10:	5462                	c.lwsp	s0,56(sp)
2005dd12:	4532                	c.lwsp	a0,12(sp)
2005dd14:	54d2                	c.lwsp	s1,52(sp)
2005dd16:	5942                	c.lwsp	s2,48(sp)
2005dd18:	59b2                	c.lwsp	s3,44(sp)
2005dd1a:	5a22                	c.lwsp	s4,40(sp)
2005dd1c:	5a92                	c.lwsp	s5,36(sp)
2005dd1e:	5b02                	c.lwsp	s6,32(sp)
2005dd20:	4bf2                	c.lwsp	s7,28(sp)
2005dd22:	6121                	c.addi16sp	sp,64
2005dd24:	8082                	c.jr	ra
2005dd26:	f781a783          	lw	a5,-136(gp) # 20066050 <uxCurrentNumberOfTasks>
2005dd2a:	dff1                	c.beqz	a5,2005dd06 <xTaskResumeAll+0x26>
2005dd2c:	200684b7          	lui	s1,0x20068
2005dd30:	20068937          	lui	s2,0x20068
2005dd34:	4401                	c.li	s0,0
2005dd36:	b8c48493          	addi	s1,s1,-1140 # 20067b8c <xPendingReadyList>
2005dd3a:	4985                	c.li	s3,1
2005dd3c:	a8890913          	addi	s2,s2,-1400 # 20067a88 <pxReadyTasksLists>
2005dd40:	4ad1                	c.li	s5,20
2005dd42:	a0b1                	c.j	2005dd8e <xTaskResumeAll+0xae>
2005dd44:	44dc                	c.lw	a5,12(s1)
2005dd46:	47c0                	c.lw	s0,12(a5)
2005dd48:	01840513          	addi	a0,s0,24
2005dd4c:	e51fc0ef          	jal	ra,2005ab9c <uxListRemove>
2005dd50:	00440593          	addi	a1,s0,4
2005dd54:	852e                	c.mv	a0,a1
2005dd56:	c62e                	c.swsp	a1,12(sp)
2005dd58:	e45fc0ef          	jal	ra,2005ab9c <uxListRemove>
2005dd5c:	5448                	c.lw	a0,44(s0)
2005dd5e:	f8818713          	addi	a4,gp,-120 # 20066060 <uxTopReadyPriority>
2005dd62:	4314                	c.lw	a3,0(a4)
2005dd64:	00a997b3          	sll	a5,s3,a0
2005dd68:	03550533          	mul	a0,a0,s5
2005dd6c:	45b2                	c.lwsp	a1,12(sp)
2005dd6e:	8fd5                	c.or	a5,a3
2005dd70:	c31c                	c.sw	a5,0(a4)
2005dd72:	954a                	c.add	a0,s2
2005dd74:	de5fc0ef          	jal	ra,2005ab58 <vListInsertEnd>
2005dd78:	f6418793          	addi	a5,gp,-156 # 2006603c <pxCurrentTCB>
2005dd7c:	439c                	c.lw	a5,0(a5)
2005dd7e:	5458                	c.lw	a4,44(s0)
2005dd80:	57dc                	c.lw	a5,44(a5)
2005dd82:	00f76663          	bltu	a4,a5,2005dd8e <xTaskResumeAll+0xae>
2005dd86:	fa018793          	addi	a5,gp,-96 # 20066078 <xYieldPending>
2005dd8a:	0137a023          	sw	s3,0(a5)
2005dd8e:	409c                	c.lw	a5,0(s1)
2005dd90:	fbd5                	c.bnez	a5,2005dd44 <xTaskResumeAll+0x64>
2005dd92:	c019                	c.beqz	s0,2005dd98 <xTaskResumeAll+0xb8>
2005dd94:	d2aff0ef          	jal	ra,2005d2be <prvResetNextTaskUnblockTime>
2005dd98:	f9418713          	addi	a4,gp,-108 # 2006606c <xPendedTicks>
2005dd9c:	4304                	c.lw	s1,0(a4)
2005dd9e:	f9418413          	addi	s0,gp,-108 # 2006606c <xPendedTicks>
2005dda2:	cc89                	c.beqz	s1,2005ddbc <xTaskResumeAll+0xdc>
2005dda4:	4985                	c.li	s3,1
2005dda6:	eb0ff0ef          	jal	ra,2005d456 <xTaskIncrementTick>
2005ddaa:	c509                	c.beqz	a0,2005ddb4 <xTaskResumeAll+0xd4>
2005ddac:	fa018793          	addi	a5,gp,-96 # 20066078 <xYieldPending>
2005ddb0:	0137a023          	sw	s3,0(a5)
2005ddb4:	14fd                	c.addi	s1,-1
2005ddb6:	f8e5                	c.bnez	s1,2005dda6 <xTaskResumeAll+0xc6>
2005ddb8:	00042023          	sw	zero,0(s0)
2005ddbc:	fa01a783          	lw	a5,-96(gp) # 20066078 <xYieldPending>
2005ddc0:	d3b9                	c.beqz	a5,2005dd06 <xTaskResumeAll+0x26>
2005ddc2:	00000073          	ecall
2005ddc6:	4505                	c.li	a0,1
2005ddc8:	b781                	c.j	2005dd08 <xTaskResumeAll+0x28>

2005ddca <vTaskDelay>:
2005ddca:	e501                	c.bnez	a0,2005ddd2 <vTaskDelay+0x8>
2005ddcc:	00000073          	ecall
2005ddd0:	8082                	c.jr	ra
2005ddd2:	1101                	c.addi	sp,-32
2005ddd4:	ce06                	c.swsp	ra,28(sp)
2005ddd6:	c62a                	c.swsp	a0,12(sp)
2005ddd8:	e5aff0ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005dddc:	4532                	c.lwsp	a0,12(sp)
2005ddde:	4581                	c.li	a1,0
2005dde0:	bbaff0ef          	jal	ra,2005d19a <prvAddCurrentTaskToDelayedList>
2005dde4:	efdff0ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005dde8:	e119                	c.bnez	a0,2005ddee <vTaskDelay+0x24>
2005ddea:	00000073          	ecall
2005ddee:	40f2                	c.lwsp	ra,28(sp)
2005ddf0:	6105                	c.addi16sp	sp,32
2005ddf2:	8082                	c.jr	ra

2005ddf4 <prvIdleTask>:
2005ddf4:	1101                	c.addi	sp,-32
2005ddf6:	ca26                	c.swsp	s1,20(sp)
2005ddf8:	200684b7          	lui	s1,0x20068
2005ddfc:	cc22                	c.swsp	s0,24(sp)
2005ddfe:	c452                	c.swsp	s4,8(sp)
2005de00:	c256                	c.swsp	s5,4(sp)
2005de02:	c05a                	c.swsp	s6,0(sp)
2005de04:	ce06                	c.swsp	ra,28(sp)
2005de06:	c84a                	c.swsp	s2,16(sp)
2005de08:	c64e                	c.swsp	s3,12(sp)
2005de0a:	bb448493          	addi	s1,s1,-1100 # 20067bb4 <xTasksWaitingTermination>
2005de0e:	20068ab7          	lui	s5,0x20068
2005de12:	4405                	c.li	s0,1
2005de14:	f7c18913          	addi	s2,gp,-132 # 20066054 <uxDeletedTasksWaitingCleanUp>
2005de18:	00092783          	lw	a5,0(s2)
2005de1c:	eb8d                	c.bnez	a5,2005de4e <prvIdleTask+0x5a>
2005de1e:	a88a8793          	addi	a5,s5,-1400 # 20067a88 <pxReadyTasksLists>
2005de22:	439c                	c.lw	a5,0(a5)
2005de24:	00f47463          	bgeu	s0,a5,2005de2c <prvIdleTask+0x38>
2005de28:	00000073          	ecall
2005de2c:	d52fe0ef          	jal	ra,2005c37e <vApplicationIdleHook>
2005de30:	c60ff0ef          	jal	ra,2005d290 <prvGetExpectedIdleTime>
2005de34:	fea470e3          	bgeu	s0,a0,2005de14 <prvIdleTask+0x20>
2005de38:	dfaff0ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005de3c:	c54ff0ef          	jal	ra,2005d290 <prvGetExpectedIdleTime>
2005de40:	00a47463          	bgeu	s0,a0,2005de48 <prvIdleTask+0x54>
2005de44:	de4fe0ef          	jal	ra,2005c428 <vPortSuppressTicksAndSleep>
2005de48:	e99ff0ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005de4c:	b7e1                	c.j	2005de14 <prvIdleTask+0x20>
2005de4e:	b0bff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005de52:	44dc                	c.lw	a5,12(s1)
2005de54:	00c7a983          	lw	s3,12(a5)
2005de58:	00498513          	addi	a0,s3,4
2005de5c:	d41fc0ef          	jal	ra,2005ab9c <uxListRemove>
2005de60:	f7818713          	addi	a4,gp,-136 # 20066050 <uxCurrentNumberOfTasks>
2005de64:	431c                	c.lw	a5,0(a4)
2005de66:	17fd                	c.addi	a5,-1
2005de68:	c31c                	c.sw	a5,0(a4)
2005de6a:	00092783          	lw	a5,0(s2)
2005de6e:	17fd                	c.addi	a5,-1
2005de70:	00f92023          	sw	a5,0(s2)
2005de74:	affff0ef          	jal	ra,2005d972 <vTaskExitCritical>
2005de78:	854e                	c.mv	a0,s3
2005de7a:	c60ff0ef          	jal	ra,2005d2da <prvDeleteTCB>
2005de7e:	bf59                	c.j	2005de14 <prvIdleTask+0x20>

2005de80 <vTaskGetInfo>:
2005de80:	1141                	c.addi	sp,-16
2005de82:	c422                	c.swsp	s0,8(sp)
2005de84:	c226                	c.swsp	s1,4(sp)
2005de86:	c04a                	c.swsp	s2,0(sp)
2005de88:	c606                	c.swsp	ra,12(sp)
2005de8a:	842e                	c.mv	s0,a1
2005de8c:	8932                	c.mv	s2,a2
2005de8e:	84aa                	c.mv	s1,a0
2005de90:	e119                	c.bnez	a0,2005de96 <vTaskGetInfo+0x16>
2005de92:	f641a483          	lw	s1,-156(gp) # 2006603c <pxCurrentTCB>
2005de96:	03448793          	addi	a5,s1,52
2005de9a:	c004                	c.sw	s1,0(s0)
2005de9c:	c05c                	c.sw	a5,4(s0)
2005de9e:	54dc                	c.lw	a5,44(s1)
2005dea0:	c81c                	c.sw	a5,16(s0)
2005dea2:	589c                	c.lw	a5,48(s1)
2005dea4:	cc5c                	c.sw	a5,28(s0)
2005dea6:	48fc                	c.lw	a5,84(s1)
2005dea8:	c41c                	c.sw	a5,8(s0)
2005deaa:	4cfc                	c.lw	a5,92(s1)
2005deac:	c85c                	c.sw	a5,20(s0)
2005deae:	54bc                	c.lw	a5,104(s1)
2005deb0:	cc1c                	c.sw	a5,24(s0)
2005deb2:	4795                	c.li	a5,5
2005deb4:	04f68c63          	beq	a3,a5,2005df0c <vTaskGetInfo+0x8c>
2005deb8:	f641a783          	lw	a5,-156(gp) # 2006603c <pxCurrentTCB>
2005debc:	02979963          	bne	a5,s1,2005deee <vTaskGetInfo+0x6e>
2005dec0:	00040623          	sb	zero,12(s0)
2005dec4:	04090c63          	beq	s2,zero,2005df1c <vTaskGetInfo+0x9c>
2005dec8:	5890                	c.lw	a2,48(s1)
2005deca:	4781                	c.li	a5,0
2005decc:	0a500713          	addi	a4,zero,165
2005ded0:	00f606b3          	add	a3,a2,a5
2005ded4:	0006c683          	lbu	a3,0(a3)
2005ded8:	04e68063          	beq	a3,a4,2005df18 <vTaskGetInfo+0x98>
2005dedc:	8389                	c.srli	a5,0x2
2005dede:	02f41023          	sh	a5,32(s0)
2005dee2:	40b2                	c.lwsp	ra,12(sp)
2005dee4:	4422                	c.lwsp	s0,8(sp)
2005dee6:	4492                	c.lwsp	s1,4(sp)
2005dee8:	4902                	c.lwsp	s2,0(sp)
2005deea:	0141                	c.addi	sp,16
2005deec:	8082                	c.jr	ra
2005deee:	00d40623          	sb	a3,12(s0)
2005def2:	478d                	c.li	a5,3
2005def4:	fcf698e3          	bne	a3,a5,2005dec4 <vTaskGetInfo+0x44>
2005def8:	d3aff0ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005defc:	549c                	c.lw	a5,40(s1)
2005defe:	c781                	c.beqz	a5,2005df06 <vTaskGetInfo+0x86>
2005df00:	4789                	c.li	a5,2
2005df02:	00f40623          	sb	a5,12(s0)
2005df06:	ddbff0ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005df0a:	bf6d                	c.j	2005dec4 <vTaskGetInfo+0x44>
2005df0c:	8526                	c.mv	a0,s1
2005df0e:	d5bff0ef          	jal	ra,2005dc68 <eTaskGetState>
2005df12:	00a40623          	sb	a0,12(s0)
2005df16:	b77d                	c.j	2005dec4 <vTaskGetInfo+0x44>
2005df18:	0785                	c.addi	a5,1
2005df1a:	bf5d                	c.j	2005ded0 <vTaskGetInfo+0x50>
2005df1c:	02041023          	sh	zero,32(s0)
2005df20:	b7c9                	c.j	2005dee2 <vTaskGetInfo+0x62>

2005df22 <prvListTasksWithinSingleList>:
2005df22:	7179                	c.addi16sp	sp,-48
2005df24:	d606                	c.swsp	ra,44(sp)
2005df26:	d422                	c.swsp	s0,40(sp)
2005df28:	d226                	c.swsp	s1,36(sp)
2005df2a:	d04a                	c.swsp	s2,32(sp)
2005df2c:	ce4e                	c.swsp	s3,28(sp)
2005df2e:	cc52                	c.swsp	s4,24(sp)
2005df30:	ca56                	c.swsp	s5,20(sp)
2005df32:	c85a                	c.swsp	s6,16(sp)
2005df34:	c65e                	c.swsp	s7,12(sp)
2005df36:	4184                	c.lw	s1,0(a1)
2005df38:	c8a9                	c.beqz	s1,2005df8a <prvListTasksWithinSingleList+0x68>
2005df3a:	41dc                	c.lw	a5,4(a1)
2005df3c:	00858a13          	addi	s4,a1,8
2005df40:	892a                	c.mv	s2,a0
2005df42:	43dc                	c.lw	a5,4(a5)
2005df44:	842e                	c.mv	s0,a1
2005df46:	89b2                	c.mv	s3,a2
2005df48:	c1dc                	c.sw	a5,4(a1)
2005df4a:	01479463          	bne	a5,s4,2005df52 <prvListTasksWithinSingleList+0x30>
2005df4e:	45dc                	c.lw	a5,12(a1)
2005df50:	c1dc                	c.sw	a5,4(a1)
2005df52:	405c                	c.lw	a5,4(s0)
2005df54:	4481                	c.li	s1,0
2005df56:	02400b93          	addi	s7,zero,36
2005df5a:	00c7ab03          	lw	s6,12(a5)
2005df5e:	405c                	c.lw	a5,4(s0)
2005df60:	43dc                	c.lw	a5,4(a5)
2005df62:	c05c                	c.sw	a5,4(s0)
2005df64:	00fa1563          	bne	s4,a5,2005df6e <prvListTasksWithinSingleList+0x4c>
2005df68:	004a2783          	lw	a5,4(s4)
2005df6c:	c05c                	c.sw	a5,4(s0)
2005df6e:	037485b3          	mul	a1,s1,s7
2005df72:	405c                	c.lw	a5,4(s0)
2005df74:	86ce                	c.mv	a3,s3
2005df76:	4605                	c.li	a2,1
2005df78:	00c7aa83          	lw	s5,12(a5)
2005df7c:	0485                	c.addi	s1,1
2005df7e:	8556                	c.mv	a0,s5
2005df80:	95ca                	c.add	a1,s2
2005df82:	effff0ef          	jal	ra,2005de80 <vTaskGetInfo>
2005df86:	fd5b1ce3          	bne	s6,s5,2005df5e <prvListTasksWithinSingleList+0x3c>
2005df8a:	50b2                	c.lwsp	ra,44(sp)
2005df8c:	5422                	c.lwsp	s0,40(sp)
2005df8e:	5902                	c.lwsp	s2,32(sp)
2005df90:	49f2                	c.lwsp	s3,28(sp)
2005df92:	4a62                	c.lwsp	s4,24(sp)
2005df94:	4ad2                	c.lwsp	s5,20(sp)
2005df96:	4b42                	c.lwsp	s6,16(sp)
2005df98:	4bb2                	c.lwsp	s7,12(sp)
2005df9a:	8526                	c.mv	a0,s1
2005df9c:	5492                	c.lwsp	s1,36(sp)
2005df9e:	6145                	c.addi16sp	sp,48
2005dfa0:	8082                	c.jr	ra

2005dfa2 <uxTaskGetSystemState>:
2005dfa2:	1101                	c.addi	sp,-32
2005dfa4:	cc22                	c.swsp	s0,24(sp)
2005dfa6:	c84a                	c.swsp	s2,16(sp)
2005dfa8:	c64e                	c.swsp	s3,12(sp)
2005dfaa:	ce06                	c.swsp	ra,28(sp)
2005dfac:	ca26                	c.swsp	s1,20(sp)
2005dfae:	c452                	c.swsp	s4,8(sp)
2005dfb0:	c256                	c.swsp	s5,4(sp)
2005dfb2:	c05a                	c.swsp	s6,0(sp)
2005dfb4:	892a                	c.mv	s2,a0
2005dfb6:	842e                	c.mv	s0,a1
2005dfb8:	89b2                	c.mv	s3,a2
2005dfba:	c78ff0ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005dfbe:	f781a783          	lw	a5,-136(gp) # 20066050 <uxCurrentNumberOfTasks>
2005dfc2:	0af46563          	bltu	s0,a5,2005e06c <uxTaskGetSystemState+0xca>
2005dfc6:	20068ab7          	lui	s5,0x20068
2005dfca:	4a2d                	c.li	s4,11
2005dfcc:	4401                	c.li	s0,0
2005dfce:	4b51                	c.li	s6,20
2005dfd0:	a88a8a93          	addi	s5,s5,-1400 # 20067a88 <pxReadyTasksLists>
2005dfd4:	02400493          	addi	s1,zero,36
2005dfd8:	1a7d                	c.addi	s4,-1
2005dfda:	02940533          	mul	a0,s0,s1
2005dfde:	4605                	c.li	a2,1
2005dfe0:	036a05b3          	mul	a1,s4,s6
2005dfe4:	954a                	c.add	a0,s2
2005dfe6:	95d6                	c.add	a1,s5
2005dfe8:	f3bff0ef          	jal	ra,2005df22 <prvListTasksWithinSingleList>
2005dfec:	942a                	c.add	s0,a0
2005dfee:	fe0a15e3          	bne	s4,zero,2005dfd8 <uxTaskGetSystemState+0x36>
2005dff2:	02940533          	mul	a0,s0,s1
2005dff6:	f681a583          	lw	a1,-152(gp) # 20066040 <pxDelayedTaskList>
2005dffa:	4609                	c.li	a2,2
2005dffc:	954a                	c.add	a0,s2
2005dffe:	f25ff0ef          	jal	ra,2005df22 <prvListTasksWithinSingleList>
2005e002:	942a                	c.add	s0,a0
2005e004:	02940533          	mul	a0,s0,s1
2005e008:	f6c1a583          	lw	a1,-148(gp) # 20066044 <pxOverflowDelayedTaskList>
2005e00c:	4609                	c.li	a2,2
2005e00e:	954a                	c.add	a0,s2
2005e010:	f13ff0ef          	jal	ra,2005df22 <prvListTasksWithinSingleList>
2005e014:	942a                	c.add	s0,a0
2005e016:	02940533          	mul	a0,s0,s1
2005e01a:	200685b7          	lui	a1,0x20068
2005e01e:	4611                	c.li	a2,4
2005e020:	bb458593          	addi	a1,a1,-1100 # 20067bb4 <xTasksWaitingTermination>
2005e024:	954a                	c.add	a0,s2
2005e026:	efdff0ef          	jal	ra,2005df22 <prvListTasksWithinSingleList>
2005e02a:	942a                	c.add	s0,a0
2005e02c:	029404b3          	mul	s1,s0,s1
2005e030:	200685b7          	lui	a1,0x20068
2005e034:	460d                	c.li	a2,3
2005e036:	ba058593          	addi	a1,a1,-1120 # 20067ba0 <xSuspendedTaskList>
2005e03a:	00990533          	add	a0,s2,s1
2005e03e:	ee5ff0ef          	jal	ra,2005df22 <prvListTasksWithinSingleList>
2005e042:	942a                	c.add	s0,a0
2005e044:	00098763          	beq	s3,zero,2005e052 <uxTaskGetSystemState+0xb0>
2005e048:	410187b7          	lui	a5,0x41018
2005e04c:	4bdc                	c.lw	a5,20(a5)
2005e04e:	00f9a023          	sw	a5,0(s3)
2005e052:	c8fff0ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005e056:	40f2                	c.lwsp	ra,28(sp)
2005e058:	8522                	c.mv	a0,s0
2005e05a:	4462                	c.lwsp	s0,24(sp)
2005e05c:	44d2                	c.lwsp	s1,20(sp)
2005e05e:	4942                	c.lwsp	s2,16(sp)
2005e060:	49b2                	c.lwsp	s3,12(sp)
2005e062:	4a22                	c.lwsp	s4,8(sp)
2005e064:	4a92                	c.lwsp	s5,4(sp)
2005e066:	4b02                	c.lwsp	s6,0(sp)
2005e068:	6105                	c.addi16sp	sp,32
2005e06a:	8082                	c.jr	ra
2005e06c:	4401                	c.li	s0,0
2005e06e:	b7d5                	c.j	2005e052 <uxTaskGetSystemState+0xb0>

2005e070 <xTaskGetTickCount>:
2005e070:	1101                	c.addi	sp,-32
2005e072:	ce06                	c.swsp	ra,28(sp)
2005e074:	8e5ff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005e078:	f9c1a503          	lw	a0,-100(gp) # 20066074 <xTickCount>
2005e07c:	c62a                	c.swsp	a0,12(sp)
2005e07e:	8f5ff0ef          	jal	ra,2005d972 <vTaskExitCritical>
2005e082:	40f2                	c.lwsp	ra,28(sp)
2005e084:	4532                	c.lwsp	a0,12(sp)
2005e086:	6105                	c.addi16sp	sp,32
2005e088:	8082                	c.jr	ra

2005e08a <xTaskCheckForTimeOut>:
2005e08a:	1101                	c.addi	sp,-32
2005e08c:	cc22                	c.swsp	s0,24(sp)
2005e08e:	ce06                	c.swsp	ra,28(sp)
2005e090:	842e                	c.mv	s0,a1
2005e092:	c62a                	c.swsp	a0,12(sp)
2005e094:	8c5ff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005e098:	f9c1a683          	lw	a3,-100(gp) # 20066074 <xTickCount>
2005e09c:	401c                	c.lw	a5,0(s0)
2005e09e:	577d                	c.li	a4,-1
2005e0a0:	02e78f63          	beq	a5,a4,2005e0de <xTaskCheckForTimeOut+0x54>
2005e0a4:	4532                	c.lwsp	a0,12(sp)
2005e0a6:	f901a603          	lw	a2,-112(gp) # 20066068 <xNumOfOverflows>
2005e0aa:	410c                	c.lw	a1,0(a0)
2005e0ac:	4158                	c.lw	a4,4(a0)
2005e0ae:	00c58f63          	beq	a1,a2,2005e0cc <xTaskCheckForTimeOut+0x42>
2005e0b2:	00e6ed63          	bltu	a3,a4,2005e0cc <xTaskCheckForTimeOut+0x42>
2005e0b6:	00042023          	sw	zero,0(s0)
2005e0ba:	4505                	c.li	a0,1
2005e0bc:	c62a                	c.swsp	a0,12(sp)
2005e0be:	8b5ff0ef          	jal	ra,2005d972 <vTaskExitCritical>
2005e0c2:	40f2                	c.lwsp	ra,28(sp)
2005e0c4:	4462                	c.lwsp	s0,24(sp)
2005e0c6:	4532                	c.lwsp	a0,12(sp)
2005e0c8:	6105                	c.addi16sp	sp,32
2005e0ca:	8082                	c.jr	ra
2005e0cc:	40e68633          	sub	a2,a3,a4
2005e0d0:	fef673e3          	bgeu	a2,a5,2005e0b6 <xTaskCheckForTimeOut+0x2c>
2005e0d4:	8f95                	c.sub	a5,a3
2005e0d6:	97ba                	c.add	a5,a4
2005e0d8:	c01c                	c.sw	a5,0(s0)
2005e0da:	e50ff0ef          	jal	ra,2005d72a <vTaskInternalSetTimeOutState>
2005e0de:	4501                	c.li	a0,0
2005e0e0:	bff1                	c.j	2005e0bc <xTaskCheckForTimeOut+0x32>

2005e0e2 <vTaskList>:
2005e0e2:	7139                	c.addi16sp	sp,-64
2005e0e4:	dc22                	c.swsp	s0,56(sp)
2005e0e6:	de06                	c.swsp	ra,60(sp)
2005e0e8:	da26                	c.swsp	s1,52(sp)
2005e0ea:	d84a                	c.swsp	s2,48(sp)
2005e0ec:	d64e                	c.swsp	s3,44(sp)
2005e0ee:	d452                	c.swsp	s4,40(sp)
2005e0f0:	d256                	c.swsp	s5,36(sp)
2005e0f2:	d05a                	c.swsp	s6,32(sp)
2005e0f4:	ce5e                	c.swsp	s7,28(sp)
2005e0f6:	f7818793          	addi	a5,gp,-136 # 20066050 <uxCurrentNumberOfTasks>
2005e0fa:	438c                	c.lw	a1,0(a5)
2005e0fc:	842a                	c.mv	s0,a0
2005e0fe:	00050023          	sb	zero,0(a0)
2005e102:	4388                	c.lw	a0,0(a5)
2005e104:	02400793          	addi	a5,zero,36
2005e108:	c62e                	c.swsp	a1,12(sp)
2005e10a:	02f50533          	mul	a0,a0,a5
2005e10e:	ff6fc0ef          	jal	ra,2005a904 <pvPortMalloc>
2005e112:	c151                	c.beqz	a0,2005e196 <vTaskList+0xb4>
2005e114:	45b2                	c.lwsp	a1,12(sp)
2005e116:	4601                	c.li	a2,0
2005e118:	892a                	c.mv	s2,a0
2005e11a:	e89ff0ef          	jal	ra,2005dfa2 <uxTaskGetSystemState>
2005e11e:	89aa                	c.mv	s3,a0
2005e120:	00490493          	addi	s1,s2,4
2005e124:	4a01                	c.li	s4,0
2005e126:	4b91                	c.li	s7,4
2005e128:	e7018a93          	addi	s5,gp,-400 # 20065f48 <CSWTCH.248>
2005e12c:	20063b37          	lui	s6,0x20063
2005e130:	013a1f63          	bne	s4,s3,2005e14e <vTaskList+0x6c>
2005e134:	5462                	c.lwsp	s0,56(sp)
2005e136:	50f2                	c.lwsp	ra,60(sp)
2005e138:	54d2                	c.lwsp	s1,52(sp)
2005e13a:	59b2                	c.lwsp	s3,44(sp)
2005e13c:	5a22                	c.lwsp	s4,40(sp)
2005e13e:	5a92                	c.lwsp	s5,36(sp)
2005e140:	5b02                	c.lwsp	s6,32(sp)
2005e142:	4bf2                	c.lwsp	s7,28(sp)
2005e144:	854a                	c.mv	a0,s2
2005e146:	5942                	c.lwsp	s2,48(sp)
2005e148:	6121                	c.addi16sp	sp,64
2005e14a:	899fc06f          	jal	zero,2005a9e2 <vPortFree>
2005e14e:	0084c783          	lbu	a5,8(s1)
2005e152:	4681                	c.li	a3,0
2005e154:	00fbe563          	bltu	s7,a5,2005e15e <vTaskList+0x7c>
2005e158:	97d6                	c.add	a5,s5
2005e15a:	0007c683          	lbu	a3,0(a5) # 41018000 <__km4_bd_boot_download_addr__+0x11006000>
2005e15e:	408c                	c.lw	a1,0(s1)
2005e160:	8522                	c.mv	a0,s0
2005e162:	c636                	c.swsp	a3,12(sp)
2005e164:	8e2ff0ef          	jal	ra,2005d246 <prvWriteNameToBuffer>
2005e168:	0044a803          	lw	a6,4(s1)
2005e16c:	01c4d783          	lhu	a5,28(s1)
2005e170:	44d8                	c.lw	a4,12(s1)
2005e172:	46b2                	c.lwsp	a3,12(sp)
2005e174:	e74b0613          	addi	a2,s6,-396 # 20062e74 <__func__.1+0x20>
2005e178:	40000593          	addi	a1,zero,1024
2005e17c:	842a                	c.mv	s0,a0
2005e17e:	078010ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e182:	8522                	c.mv	a0,s0
2005e184:	dffac097          	auipc	ra,0xdffac
2005e188:	954080e7          	jalr	ra,-1708(ra) # 9ad8 <__wrap_strlen>
2005e18c:	942a                	c.add	s0,a0
2005e18e:	0a05                	c.addi	s4,1
2005e190:	02448493          	addi	s1,s1,36
2005e194:	bf71                	c.j	2005e130 <vTaskList+0x4e>
2005e196:	50f2                	c.lwsp	ra,60(sp)
2005e198:	5462                	c.lwsp	s0,56(sp)
2005e19a:	54d2                	c.lwsp	s1,52(sp)
2005e19c:	5942                	c.lwsp	s2,48(sp)
2005e19e:	59b2                	c.lwsp	s3,44(sp)
2005e1a0:	5a22                	c.lwsp	s4,40(sp)
2005e1a2:	5a92                	c.lwsp	s5,36(sp)
2005e1a4:	5b02                	c.lwsp	s6,32(sp)
2005e1a6:	4bf2                	c.lwsp	s7,28(sp)
2005e1a8:	6121                	c.addi16sp	sp,64
2005e1aa:	8082                	c.jr	ra

2005e1ac <pvTaskIncrementMutexHeldCount>:
2005e1ac:	f6418713          	addi	a4,gp,-156 # 2006603c <pxCurrentTCB>
2005e1b0:	4318                	c.lw	a4,0(a4)
2005e1b2:	f6418793          	addi	a5,gp,-156 # 2006603c <pxCurrentTCB>
2005e1b6:	c709                	c.beqz	a4,2005e1c0 <pvTaskIncrementMutexHeldCount+0x14>
2005e1b8:	4394                	c.lw	a3,0(a5)
2005e1ba:	52b8                	c.lw	a4,96(a3)
2005e1bc:	0705                	c.addi	a4,1
2005e1be:	d2b8                	c.sw	a4,96(a3)
2005e1c0:	4388                	c.lw	a0,0(a5)
2005e1c2:	8082                	c.jr	ra

2005e1c4 <prvCheckForValidListAndQueue>:
2005e1c4:	1141                	c.addi	sp,-16
2005e1c6:	c422                	c.swsp	s0,8(sp)
2005e1c8:	c606                	c.swsp	ra,12(sp)
2005e1ca:	c226                	c.swsp	s1,4(sp)
2005e1cc:	c04a                	c.swsp	s2,0(sp)
2005e1ce:	fb018413          	addi	s0,gp,-80 # 20066088 <xTimerQueue>
2005e1d2:	f86ff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005e1d6:	401c                	c.lw	a5,0(s0)
2005e1d8:	e3b9                	c.bnez	a5,2005e21e <prvCheckForValidListAndQueue+0x5a>
2005e1da:	20068537          	lui	a0,0x20068
2005e1de:	c4050913          	addi	s2,a0,-960 # 20067c40 <xActiveTimerList1>
2005e1e2:	c4050513          	addi	a0,a0,-960
2005e1e6:	959fc0ef          	jal	ra,2005ab3e <vListInitialise>
2005e1ea:	20068537          	lui	a0,0x20068
2005e1ee:	c5450493          	addi	s1,a0,-940 # 20067c54 <xActiveTimerList2>
2005e1f2:	c5450513          	addi	a0,a0,-940
2005e1f6:	949fc0ef          	jal	ra,2005ab3e <vListInitialise>
2005e1fa:	200686b7          	lui	a3,0x20068
2005e1fe:	20068637          	lui	a2,0x20068
2005e202:	fb21a223          	sw	s2,-92(gp) # 2006607c <pxCurrentTimerList>
2005e206:	4701                	c.li	a4,0
2005e208:	c6868693          	addi	a3,a3,-920 # 20067c68 <xStaticTimerQueue.0>
2005e20c:	bc860613          	addi	a2,a2,-1080 # 20067bc8 <ucStaticTimerQueueStorage.1>
2005e210:	45b1                	c.li	a1,12
2005e212:	4529                	c.li	a0,10
2005e214:	fa91a423          	sw	s1,-88(gp) # 20066080 <pxOverflowTimerList>
2005e218:	9b7fe0ef          	jal	ra,2005cbce <xQueueGenericCreateStatic>
2005e21c:	c008                	c.sw	a0,0(s0)
2005e21e:	4422                	c.lwsp	s0,8(sp)
2005e220:	40b2                	c.lwsp	ra,12(sp)
2005e222:	4492                	c.lwsp	s1,4(sp)
2005e224:	4902                	c.lwsp	s2,0(sp)
2005e226:	0141                	c.addi	sp,16
2005e228:	f4aff06f          	jal	zero,2005d972 <vTaskExitCritical>

2005e22c <prvInsertTimerInActiveList>:
2005e22c:	1141                	c.addi	sp,-16
2005e22e:	c606                	c.swsp	ra,12(sp)
2005e230:	c14c                	c.sw	a1,4(a0)
2005e232:	c908                	c.sw	a0,16(a0)
2005e234:	87aa                	c.mv	a5,a0
2005e236:	02b66163          	bltu	a2,a1,2005e258 <prvInsertTimerInActiveList+0x2c>
2005e23a:	4d18                	c.lw	a4,24(a0)
2005e23c:	8e15                	c.sub	a2,a3
2005e23e:	4505                	c.li	a0,1
2005e240:	00e67963          	bgeu	a2,a4,2005e252 <prvInsertTimerInActiveList+0x26>
2005e244:	00478593          	addi	a1,a5,4
2005e248:	fa81a503          	lw	a0,-88(gp) # 20066080 <pxOverflowTimerList>
2005e24c:	923fc0ef          	jal	ra,2005ab6e <vListInsert>
2005e250:	4501                	c.li	a0,0
2005e252:	40b2                	c.lwsp	ra,12(sp)
2005e254:	0141                	c.addi	sp,16
2005e256:	8082                	c.jr	ra
2005e258:	00d67563          	bgeu	a2,a3,2005e262 <prvInsertTimerInActiveList+0x36>
2005e25c:	4505                	c.li	a0,1
2005e25e:	fed5fae3          	bgeu	a1,a3,2005e252 <prvInsertTimerInActiveList+0x26>
2005e262:	00478593          	addi	a1,a5,4
2005e266:	fa41a503          	lw	a0,-92(gp) # 2006607c <pxCurrentTimerList>
2005e26a:	b7cd                	c.j	2005e24c <prvInsertTimerInActiveList+0x20>

2005e26c <xTimerCreateTimerTask>:
2005e26c:	1101                	c.addi	sp,-32
2005e26e:	ce06                	c.swsp	ra,28(sp)
2005e270:	f55ff0ef          	jal	ra,2005e1c4 <prvCheckForValidListAndQueue>
2005e274:	fb01a783          	lw	a5,-80(gp) # 20066088 <xTimerQueue>
2005e278:	4501                	c.li	a0,0
2005e27a:	cb9d                	c.beqz	a5,2005e2b0 <xTimerCreateTimerTask+0x44>
2005e27c:	0070                	c.addi4spn	a2,sp,12
2005e27e:	002c                	c.addi4spn	a1,sp,8
2005e280:	0048                	c.addi4spn	a0,sp,4
2005e282:	c202                	c.swsp	zero,4(sp)
2005e284:	c402                	c.swsp	zero,8(sp)
2005e286:	986fe0ef          	jal	ra,2005c40c <vApplicationGetTimerTaskMemory>
2005e28a:	47a2                	c.lwsp	a5,8(sp)
2005e28c:	4812                	c.lwsp	a6,4(sp)
2005e28e:	4632                	c.lwsp	a2,12(sp)
2005e290:	200635b7          	lui	a1,0x20063
2005e294:	2005e537          	lui	a0,0x2005e
2005e298:	4729                	c.li	a4,10
2005e29a:	4681                	c.li	a3,0
2005e29c:	e8458593          	addi	a1,a1,-380 # 20062e84 <__func__.1+0x30>
2005e2a0:	49050513          	addi	a0,a0,1168 # 2005e490 <prvTimerTask>
2005e2a4:	ff8ff0ef          	jal	ra,2005da9c <xTaskCreateStatic>
2005e2a8:	faa1aa23          	sw	a0,-76(gp) # 2006608c <xTimerTaskHandle>
2005e2ac:	00a03533          	sltu	a0,zero,a0
2005e2b0:	40f2                	c.lwsp	ra,28(sp)
2005e2b2:	6105                	c.addi16sp	sp,32
2005e2b4:	8082                	c.jr	ra

2005e2b6 <xTimerCreate>:
2005e2b6:	1101                	c.addi	sp,-32
2005e2b8:	c256                	c.swsp	s5,4(sp)
2005e2ba:	8aaa                	c.mv	s5,a0
2005e2bc:	02c00513          	addi	a0,zero,44
2005e2c0:	cc22                	c.swsp	s0,24(sp)
2005e2c2:	ca26                	c.swsp	s1,20(sp)
2005e2c4:	c84a                	c.swsp	s2,16(sp)
2005e2c6:	c64e                	c.swsp	s3,12(sp)
2005e2c8:	c452                	c.swsp	s4,8(sp)
2005e2ca:	ce06                	c.swsp	ra,28(sp)
2005e2cc:	8a2e                	c.mv	s4,a1
2005e2ce:	84b2                	c.mv	s1,a2
2005e2d0:	89b6                	c.mv	s3,a3
2005e2d2:	893a                	c.mv	s2,a4
2005e2d4:	e30fc0ef          	jal	ra,2005a904 <pvPortMalloc>
2005e2d8:	842a                	c.mv	s0,a0
2005e2da:	c905                	c.beqz	a0,2005e30a <xTimerCreate+0x54>
2005e2dc:	02050423          	sb	zero,40(a0)
2005e2e0:	ee5ff0ef          	jal	ra,2005e1c4 <prvCheckForValidListAndQueue>
2005e2e4:	00440513          	addi	a0,s0,4
2005e2e8:	01542023          	sw	s5,0(s0)
2005e2ec:	01442c23          	sw	s4,24(s0)
2005e2f0:	01342e23          	sw	s3,28(s0)
2005e2f4:	03242023          	sw	s2,32(s0)
2005e2f8:	85bfc0ef          	jal	ra,2005ab52 <vListInitialiseItem>
2005e2fc:	c499                	c.beqz	s1,2005e30a <xTimerCreate+0x54>
2005e2fe:	02844783          	lbu	a5,40(s0)
2005e302:	0047e793          	ori	a5,a5,4
2005e306:	02f40423          	sb	a5,40(s0)
2005e30a:	40f2                	c.lwsp	ra,28(sp)
2005e30c:	8522                	c.mv	a0,s0
2005e30e:	4462                	c.lwsp	s0,24(sp)
2005e310:	44d2                	c.lwsp	s1,20(sp)
2005e312:	4942                	c.lwsp	s2,16(sp)
2005e314:	49b2                	c.lwsp	s3,12(sp)
2005e316:	4a22                	c.lwsp	s4,8(sp)
2005e318:	4a92                	c.lwsp	s5,4(sp)
2005e31a:	6105                	c.addi16sp	sp,32
2005e31c:	8082                	c.jr	ra

2005e31e <xTimerGenericCommand>:
2005e31e:	7139                	c.addi16sp	sp,-64
2005e320:	dc22                	c.swsp	s0,56(sp)
2005e322:	da26                	c.swsp	s1,52(sp)
2005e324:	d84a                	c.swsp	s2,48(sp)
2005e326:	d452                	c.swsp	s4,40(sp)
2005e328:	d256                	c.swsp	s5,36(sp)
2005e32a:	de06                	c.swsp	ra,60(sp)
2005e32c:	d64e                	c.swsp	s3,44(sp)
2005e32e:	842a                	c.mv	s0,a0
2005e330:	84ae                	c.mv	s1,a1
2005e332:	8932                	c.mv	s2,a2
2005e334:	8a36                	c.mv	s4,a3
2005e336:	8aba                	c.mv	s5,a4
2005e338:	c38ff0ef          	jal	ra,2005d770 <xTaskGetCurrentTaskHandle>
2005e33c:	fb41a783          	lw	a5,-76(gp) # 2006608c <xTimerTaskHandle>
2005e340:	06a79163          	bne	a5,a0,2005e3a2 <xTimerGenericCommand+0x84>
2005e344:	ffd48793          	addi	a5,s1,-3
2005e348:	4705                	c.li	a4,1
2005e34a:	04f76c63          	bltu	a4,a5,2005e3a2 <xTimerGenericCommand+0x84>
2005e34e:	d23ff0ef          	jal	ra,2005e070 <xTaskGetTickCount>
2005e352:	485c                	c.lw	a5,20(s0)
2005e354:	86aa                	c.mv	a3,a0
2005e356:	c799                	c.beqz	a5,2005e364 <xTimerGenericCommand+0x46>
2005e358:	c62a                	c.swsp	a0,12(sp)
2005e35a:	00440513          	addi	a0,s0,4
2005e35e:	83ffc0ef          	jal	ra,2005ab9c <uxListRemove>
2005e362:	46b2                	c.lwsp	a3,12(sp)
2005e364:	4711                	c.li	a4,4
2005e366:	02844783          	lbu	a5,40(s0)
2005e36a:	00e48f63          	beq	s1,a4,2005e388 <xTimerGenericCommand+0x6a>
2005e36e:	9bf9                	c.andi	a5,-2
2005e370:	02f40423          	sb	a5,40(s0)
2005e374:	4505                	c.li	a0,1
2005e376:	50f2                	c.lwsp	ra,60(sp)
2005e378:	5462                	c.lwsp	s0,56(sp)
2005e37a:	54d2                	c.lwsp	s1,52(sp)
2005e37c:	5942                	c.lwsp	s2,48(sp)
2005e37e:	59b2                	c.lwsp	s3,44(sp)
2005e380:	5a22                	c.lwsp	s4,40(sp)
2005e382:	5a92                	c.lwsp	s5,36(sp)
2005e384:	6121                	c.addi16sp	sp,64
2005e386:	8082                	c.jr	ra
2005e388:	0017e793          	ori	a5,a5,1
2005e38c:	02f40423          	sb	a5,40(s0)
2005e390:	01242c23          	sw	s2,24(s0)
2005e394:	8636                	c.mv	a2,a3
2005e396:	00d905b3          	add	a1,s2,a3
2005e39a:	8522                	c.mv	a0,s0
2005e39c:	e91ff0ef          	jal	ra,2005e22c <prvInsertTimerInActiveList>
2005e3a0:	bfd1                	c.j	2005e374 <xTimerGenericCommand+0x56>
2005e3a2:	fb018993          	addi	s3,gp,-80 # 20066088 <xTimerQueue>
2005e3a6:	0009a783          	lw	a5,0(s3)
2005e3aa:	4501                	c.li	a0,0
2005e3ac:	d7e9                	c.beqz	a5,2005e376 <xTimerGenericCommand+0x58>
2005e3ae:	ca26                	c.swsp	s1,20(sp)
2005e3b0:	cc4a                	c.swsp	s2,24(sp)
2005e3b2:	ce22                	c.swsp	s0,28(sp)
2005e3b4:	4715                	c.li	a4,5
2005e3b6:	02974163          	blt	a4,s1,2005e3d8 <xTimerGenericCommand+0xba>
2005e3ba:	bbcff0ef          	jal	ra,2005d776 <xTaskGetSchedulerState>
2005e3be:	872a                	c.mv	a4,a0
2005e3c0:	4789                	c.li	a5,2
2005e3c2:	0009a503          	lw	a0,0(s3)
2005e3c6:	4681                	c.li	a3,0
2005e3c8:	8656                	c.mv	a2,s5
2005e3ca:	00f70363          	beq	a4,a5,2005e3d0 <xTimerGenericCommand+0xb2>
2005e3ce:	4601                	c.li	a2,0
2005e3d0:	084c                	c.addi4spn	a1,sp,20
2005e3d2:	a7afe0ef          	jal	ra,2005c64c <xQueueGenericSend>
2005e3d6:	b745                	c.j	2005e376 <xTimerGenericCommand+0x58>
2005e3d8:	4681                	c.li	a3,0
2005e3da:	8652                	c.mv	a2,s4
2005e3dc:	084c                	c.addi4spn	a1,sp,20
2005e3de:	853e                	c.mv	a0,a5
2005e3e0:	b70fe0ef          	jal	ra,2005c750 <xQueueGenericSendFromISR>
2005e3e4:	bf49                	c.j	2005e376 <xTimerGenericCommand+0x58>

2005e3e6 <prvSampleTimeNow>:
2005e3e6:	7179                	c.addi16sp	sp,-48
2005e3e8:	d226                	c.swsp	s1,36(sp)
2005e3ea:	d04a                	c.swsp	s2,32(sp)
2005e3ec:	ce4e                	c.swsp	s3,28(sp)
2005e3ee:	d606                	c.swsp	ra,44(sp)
2005e3f0:	d422                	c.swsp	s0,40(sp)
2005e3f2:	cc52                	c.swsp	s4,24(sp)
2005e3f4:	ca56                	c.swsp	s5,20(sp)
2005e3f6:	89aa                	c.mv	s3,a0
2005e3f8:	c79ff0ef          	jal	ra,2005e070 <xTaskGetTickCount>
2005e3fc:	fac1a783          	lw	a5,-84(gp) # 20066084 <xLastTime.2>
2005e400:	84aa                	c.mv	s1,a0
2005e402:	fac18913          	addi	s2,gp,-84 # 20066084 <xLastTime.2>
2005e406:	08f57263          	bgeu	a0,a5,2005e48a <prvSampleTimeNow+0xa4>
2005e40a:	fa418a13          	addi	s4,gp,-92 # 2006607c <pxCurrentTimerList>
2005e40e:	000a2703          	lw	a4,0(s4)
2005e412:	431c                	c.lw	a5,0(a4)
2005e414:	e795                	c.bnez	a5,2005e440 <prvSampleTimeNow+0x5a>
2005e416:	fa818793          	addi	a5,gp,-88 # 20066080 <pxOverflowTimerList>
2005e41a:	4394                	c.lw	a3,0(a5)
2005e41c:	c398                	c.sw	a4,0(a5)
2005e41e:	4785                	c.li	a5,1
2005e420:	00da2023          	sw	a3,0(s4)
2005e424:	00f9a023          	sw	a5,0(s3)
2005e428:	50b2                	c.lwsp	ra,44(sp)
2005e42a:	5422                	c.lwsp	s0,40(sp)
2005e42c:	00992023          	sw	s1,0(s2)
2005e430:	49f2                	c.lwsp	s3,28(sp)
2005e432:	5902                	c.lwsp	s2,32(sp)
2005e434:	4a62                	c.lwsp	s4,24(sp)
2005e436:	4ad2                	c.lwsp	s5,20(sp)
2005e438:	8526                	c.mv	a0,s1
2005e43a:	5492                	c.lwsp	s1,36(sp)
2005e43c:	6145                	c.addi16sp	sp,48
2005e43e:	8082                	c.jr	ra
2005e440:	475c                	c.lw	a5,12(a4)
2005e442:	47c0                	c.lw	s0,12(a5)
2005e444:	4390                	c.lw	a2,0(a5)
2005e446:	00440593          	addi	a1,s0,4
2005e44a:	852e                	c.mv	a0,a1
2005e44c:	c632                	c.swsp	a2,12(sp)
2005e44e:	c42e                	c.swsp	a1,8(sp)
2005e450:	f4cfc0ef          	jal	ra,2005ab9c <uxListRemove>
2005e454:	501c                	c.lw	a5,32(s0)
2005e456:	8522                	c.mv	a0,s0
2005e458:	9782                	c.jalr	a5
2005e45a:	02844783          	lbu	a5,40(s0)
2005e45e:	45a2                	c.lwsp	a1,8(sp)
2005e460:	4632                	c.lwsp	a2,12(sp)
2005e462:	8b91                	c.andi	a5,4
2005e464:	d3dd                	c.beqz	a5,2005e40a <prvSampleTimeNow+0x24>
2005e466:	4c1c                	c.lw	a5,24(s0)
2005e468:	97b2                	c.add	a5,a2
2005e46a:	00f67963          	bgeu	a2,a5,2005e47c <prvSampleTimeNow+0x96>
2005e46e:	000a2503          	lw	a0,0(s4)
2005e472:	c05c                	c.sw	a5,4(s0)
2005e474:	c800                	c.sw	s0,16(s0)
2005e476:	ef8fc0ef          	jal	ra,2005ab6e <vListInsert>
2005e47a:	bf41                	c.j	2005e40a <prvSampleTimeNow+0x24>
2005e47c:	4701                	c.li	a4,0
2005e47e:	4681                	c.li	a3,0
2005e480:	4581                	c.li	a1,0
2005e482:	8522                	c.mv	a0,s0
2005e484:	e9bff0ef          	jal	ra,2005e31e <xTimerGenericCommand>
2005e488:	b749                	c.j	2005e40a <prvSampleTimeNow+0x24>
2005e48a:	0009a023          	sw	zero,0(s3)
2005e48e:	bf69                	c.j	2005e428 <prvSampleTimeNow+0x42>

2005e490 <prvTimerTask>:
2005e490:	7139                	c.addi16sp	sp,-64
2005e492:	d84a                	c.swsp	s2,48(sp)
2005e494:	20063937          	lui	s2,0x20063
2005e498:	d64e                	c.swsp	s3,44(sp)
2005e49a:	d256                	c.swsp	s5,36(sp)
2005e49c:	d05a                	c.swsp	s6,32(sp)
2005e49e:	de06                	c.swsp	ra,60(sp)
2005e4a0:	dc22                	c.swsp	s0,56(sp)
2005e4a2:	da26                	c.swsp	s1,52(sp)
2005e4a4:	d452                	c.swsp	s4,40(sp)
2005e4a6:	ce5e                	c.swsp	s7,28(sp)
2005e4a8:	cc62                	c.swsp	s8,24(sp)
2005e4aa:	e8c90913          	addi	s2,s2,-372 # 20062e8c <__func__.1+0x38>
2005e4ae:	fa41a783          	lw	a5,-92(gp) # 2006607c <pxCurrentTimerList>
2005e4b2:	fa418c13          	addi	s8,gp,-92 # 2006607c <pxCurrentTimerList>
2005e4b6:	4405                	c.li	s0,1
2005e4b8:	4384                	c.lw	s1,0(a5)
2005e4ba:	c481                	c.beqz	s1,2005e4c2 <prvTimerTask+0x32>
2005e4bc:	47dc                	c.lw	a5,12(a5)
2005e4be:	4401                	c.li	s0,0
2005e4c0:	4384                	c.lw	s1,0(a5)
2005e4c2:	f71fe0ef          	jal	ra,2005d432 <vTaskSuspendAll>
2005e4c6:	0048                	c.addi4spn	a0,sp,4
2005e4c8:	f1fff0ef          	jal	ra,2005e3e6 <prvSampleTimeNow>
2005e4cc:	4792                	c.lwsp	a5,4(sp)
2005e4ce:	8baa                	c.mv	s7,a0
2005e4d0:	fb018a13          	addi	s4,gp,-80 # 20066088 <xTimerQueue>
2005e4d4:	ebc5                	c.bnez	a5,2005e584 <prvTimerTask+0xf4>
2005e4d6:	e449                	c.bnez	s0,2005e560 <prvTimerTask+0xd0>
2005e4d8:	08956963          	bltu	a0,s1,2005e56a <prvTimerTask+0xda>
2005e4dc:	805ff0ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005e4e0:	000c2783          	lw	a5,0(s8)
2005e4e4:	47dc                	c.lw	a5,12(a5)
2005e4e6:	47c0                	c.lw	s0,12(a5)
2005e4e8:	00440513          	addi	a0,s0,4
2005e4ec:	eb0fc0ef          	jal	ra,2005ab9c <uxListRemove>
2005e4f0:	02844783          	lbu	a5,40(s0)
2005e4f4:	0047f713          	andi	a4,a5,4
2005e4f8:	c325                	c.beqz	a4,2005e558 <prvTimerTask+0xc8>
2005e4fa:	4c0c                	c.lw	a1,24(s0)
2005e4fc:	86a6                	c.mv	a3,s1
2005e4fe:	865e                	c.mv	a2,s7
2005e500:	95a6                	c.add	a1,s1
2005e502:	8522                	c.mv	a0,s0
2005e504:	d29ff0ef          	jal	ra,2005e22c <prvInsertTimerInActiveList>
2005e508:	c901                	c.beqz	a0,2005e518 <prvTimerTask+0x88>
2005e50a:	4701                	c.li	a4,0
2005e50c:	4681                	c.li	a3,0
2005e50e:	8626                	c.mv	a2,s1
2005e510:	4581                	c.li	a1,0
2005e512:	8522                	c.mv	a0,s0
2005e514:	e0bff0ef          	jal	ra,2005e31e <xTimerGenericCommand>
2005e518:	501c                	c.lw	a5,32(s0)
2005e51a:	8522                	c.mv	a0,s0
2005e51c:	9782                	c.jalr	a5
2005e51e:	44a5                	c.li	s1,9
2005e520:	000a2503          	lw	a0,0(s4)
2005e524:	4601                	c.li	a2,0
2005e526:	004c                	c.addi4spn	a1,sp,4
2005e528:	ae6fe0ef          	jal	ra,2005c80e <xQueueReceive>
2005e52c:	d149                	c.beqz	a0,2005e4ae <prvTimerTask+0x1e>
2005e52e:	4792                	c.lwsp	a5,4(sp)
2005e530:	fe07c8e3          	blt	a5,zero,2005e520 <prvTimerTask+0x90>
2005e534:	4432                	c.lwsp	s0,12(sp)
2005e536:	485c                	c.lw	a5,20(s0)
2005e538:	c789                	c.beqz	a5,2005e542 <prvTimerTask+0xb2>
2005e53a:	00440513          	addi	a0,s0,4
2005e53e:	e5efc0ef          	jal	ra,2005ab9c <uxListRemove>
2005e542:	850a                	c.mv	a0,sp
2005e544:	ea3ff0ef          	jal	ra,2005e3e6 <prvSampleTimeNow>
2005e548:	4792                	c.lwsp	a5,4(sp)
2005e54a:	862a                	c.mv	a2,a0
2005e54c:	fcf4eae3          	bltu	s1,a5,2005e520 <prvTimerTask+0x90>
2005e550:	078a                	c.slli	a5,0x2
2005e552:	97ca                	c.add	a5,s2
2005e554:	439c                	c.lw	a5,0(a5)
2005e556:	8782                	c.jr	a5
2005e558:	9bf9                	c.andi	a5,-2
2005e55a:	02f40423          	sb	a5,40(s0)
2005e55e:	bf6d                	c.j	2005e518 <prvTimerTask+0x88>
2005e560:	fa81a783          	lw	a5,-88(gp) # 20066080 <pxOverflowTimerList>
2005e564:	4380                	c.lw	s0,0(a5)
2005e566:	00143413          	sltiu	s0,s0,1
2005e56a:	000a2503          	lw	a0,0(s4)
2005e56e:	8622                	c.mv	a2,s0
2005e570:	417485b3          	sub	a1,s1,s7
2005e574:	d12fe0ef          	jal	ra,2005ca86 <vQueueWaitForMessageRestricted>
2005e578:	f68ff0ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005e57c:	f14d                	c.bnez	a0,2005e51e <prvTimerTask+0x8e>
2005e57e:	00000073          	ecall
2005e582:	bf71                	c.j	2005e51e <prvTimerTask+0x8e>
2005e584:	f5cff0ef          	jal	ra,2005dce0 <xTaskResumeAll>
2005e588:	bf59                	c.j	2005e51e <prvTimerTask+0x8e>
2005e58a:	02844783          	lbu	a5,40(s0)
2005e58e:	4c0c                	c.lw	a1,24(s0)
2005e590:	8522                	c.mv	a0,s0
2005e592:	0017e793          	ori	a5,a5,1
2005e596:	02f40423          	sb	a5,40(s0)
2005e59a:	46a2                	c.lwsp	a3,8(sp)
2005e59c:	95b6                	c.add	a1,a3
2005e59e:	c8fff0ef          	jal	ra,2005e22c <prvInsertTimerInActiveList>
2005e5a2:	dd3d                	c.beqz	a0,2005e520 <prvTimerTask+0x90>
2005e5a4:	501c                	c.lw	a5,32(s0)
2005e5a6:	8522                	c.mv	a0,s0
2005e5a8:	9782                	c.jalr	a5
2005e5aa:	02844783          	lbu	a5,40(s0)
2005e5ae:	8b91                	c.andi	a5,4
2005e5b0:	dba5                	c.beqz	a5,2005e520 <prvTimerTask+0x90>
2005e5b2:	4c1c                	c.lw	a5,24(s0)
2005e5b4:	4622                	c.lwsp	a2,8(sp)
2005e5b6:	4701                	c.li	a4,0
2005e5b8:	4681                	c.li	a3,0
2005e5ba:	963e                	c.add	a2,a5
2005e5bc:	4581                	c.li	a1,0
2005e5be:	8522                	c.mv	a0,s0
2005e5c0:	d5fff0ef          	jal	ra,2005e31e <xTimerGenericCommand>
2005e5c4:	bfb1                	c.j	2005e520 <prvTimerTask+0x90>
2005e5c6:	02844783          	lbu	a5,40(s0)
2005e5ca:	9bf9                	c.andi	a5,-2
2005e5cc:	02f40423          	sb	a5,40(s0)
2005e5d0:	bf81                	c.j	2005e520 <prvTimerTask+0x90>
2005e5d2:	02844783          	lbu	a5,40(s0)
2005e5d6:	86aa                	c.mv	a3,a0
2005e5d8:	0017e793          	ori	a5,a5,1
2005e5dc:	02f40423          	sb	a5,40(s0)
2005e5e0:	45a2                	c.lwsp	a1,8(sp)
2005e5e2:	cc0c                	c.sw	a1,24(s0)
2005e5e4:	95aa                	c.add	a1,a0
2005e5e6:	8522                	c.mv	a0,s0
2005e5e8:	c45ff0ef          	jal	ra,2005e22c <prvInsertTimerInActiveList>
2005e5ec:	bf15                	c.j	2005e520 <prvTimerTask+0x90>
2005e5ee:	02844783          	lbu	a5,40(s0)
2005e5f2:	0027f713          	andi	a4,a5,2
2005e5f6:	fb71                	c.bnez	a4,2005e5ca <prvTimerTask+0x13a>
2005e5f8:	8522                	c.mv	a0,s0
2005e5fa:	be8fc0ef          	jal	ra,2005a9e2 <vPortFree>
2005e5fe:	b70d                	c.j	2005e520 <prvTimerTask+0x90>

2005e600 <wifi_fast_connect_enable>:
2005e600:	fc01a023          	sw	zero,-64(gp) # 20066098 <p_wifi_do_fast_connect>
2005e604:	fa01ae23          	sw	zero,-68(gp) # 20066094 <p_store_fast_connect_info>
2005e608:	8082                	c.jr	ra

2005e60a <rtw_indicate_event_handle>:
2005e60a:	4759                	c.li	a4,22
2005e60c:	04a74e63          	blt	a4,a0,2005e668 <rtw_indicate_event_handle+0x5e>
2005e610:	1101                	c.addi	sp,-32
2005e612:	20068737          	lui	a4,0x20068
2005e616:	ca26                	c.swsp	s1,20(sp)
2005e618:	c84a                	c.swsp	s2,16(sp)
2005e61a:	00451493          	slli	s1,a0,0x4
2005e61e:	892e                	c.mv	s2,a1
2005e620:	85b2                	c.mv	a1,a2
2005e622:	8636                	c.mv	a2,a3
2005e624:	cb870693          	addi	a3,a4,-840 # 20067cb8 <event_callback_list>
2005e628:	cc22                	c.swsp	s0,24(sp)
2005e62a:	ce06                	c.swsp	ra,28(sp)
2005e62c:	009687b3          	add	a5,a3,s1
2005e630:	0007a803          	lw	a6,0(a5)
2005e634:	cb870413          	addi	s0,a4,-840
2005e638:	00080963          	beq	a6,zero,2005e64a <rtw_indicate_event_handle+0x40>
2005e63c:	43d4                	c.lw	a3,4(a5)
2005e63e:	c632                	c.swsp	a2,12(sp)
2005e640:	c42e                	c.swsp	a1,8(sp)
2005e642:	854a                	c.mv	a0,s2
2005e644:	9802                	c.jalr	a6
2005e646:	4632                	c.lwsp	a2,12(sp)
2005e648:	45a2                	c.lwsp	a1,8(sp)
2005e64a:	009407b3          	add	a5,s0,s1
2005e64e:	4798                	c.lw	a4,8(a5)
2005e650:	4501                	c.li	a0,0
2005e652:	c709                	c.beqz	a4,2005e65c <rtw_indicate_event_handle+0x52>
2005e654:	47d4                	c.lw	a3,12(a5)
2005e656:	854a                	c.mv	a0,s2
2005e658:	9702                	c.jalr	a4
2005e65a:	4501                	c.li	a0,0
2005e65c:	40f2                	c.lwsp	ra,28(sp)
2005e65e:	4462                	c.lwsp	s0,24(sp)
2005e660:	44d2                	c.lwsp	s1,20(sp)
2005e662:	4942                	c.lwsp	s2,16(sp)
2005e664:	6105                	c.addi16sp	sp,32
2005e666:	8082                	c.jr	ra
2005e668:	5579                	c.li	a0,-2
2005e66a:	8082                	c.jr	ra

2005e66c <wifi_indication>:
2005e66c:	472d                	c.li	a4,11
2005e66e:	00e51863          	bne	a0,a4,2005e67e <wifi_indication+0x12>
2005e672:	fc41a703          	lw	a4,-60(gp) # 2006609c <p_wifi_joinstatus_internal_callback>
2005e676:	c711                	c.beqz	a4,2005e682 <wifi_indication+0x16>
2005e678:	0ff6f513          	andi	a0,a3,255
2005e67c:	8702                	c.jr	a4
2005e67e:	f8dff06f          	jal	zero,2005e60a <rtw_indicate_event_handle>
2005e682:	8082                	c.jr	ra

2005e684 <_init_thread>:
2005e684:	1141                	c.addi	sp,-16
2005e686:	c422                	c.swsp	s0,8(sp)
2005e688:	c606                	c.swsp	ra,12(sp)
2005e68a:	4100c437          	lui	s0,0x4100c
2005e68e:	08442783          	lw	a5,132(s0) # 4100c084 <__km4_bd_boot_download_addr__+0x10ffa084>
2005e692:	08440713          	addi	a4,s0,132
2005e696:	8b85                	c.andi	a5,1
2005e698:	c7b1                	c.beqz	a5,2005e6e4 <_init_thread+0x60>
2005e69a:	431c                	c.lw	a5,0(a4)
2005e69c:	4505                	c.li	a0,1
2005e69e:	9bf9                	c.andi	a5,-2
2005e6a0:	c31c                	c.sw	a5,0(a4)
2005e6a2:	067010ef          	jal	ra,2005ff08 <wifi_on>
2005e6a6:	4505                	c.li	a0,1
2005e6a8:	11d010ef          	jal	ra,2005ffc4 <wifi_config_autoreconnect>
2005e6ac:	8c6fd0ef          	jal	ra,2005b772 <rtos_mem_get_free_heap_size>
2005e6b0:	20063737          	lui	a4,0x20063
2005e6b4:	200636b7          	lui	a3,0x20063
2005e6b8:	200635b7          	lui	a1,0x20063
2005e6bc:	882a                	c.mv	a6,a0
2005e6be:	03500793          	addi	a5,zero,53
2005e6c2:	4511                	c.li	a0,4
2005e6c4:	ef870713          	addi	a4,a4,-264 # 20062ef8 <__FUNCTION__.0>
2005e6c8:	eb468693          	addi	a3,a3,-332 # 20062eb4 <__func__.1+0x60>
2005e6cc:	04900613          	addi	a2,zero,73
2005e6d0:	ed058593          	addi	a1,a1,-304 # 20062ed0 <__func__.1+0x7c>
2005e6d4:	f9cfc0ef          	jal	ra,2005ae70 <rtk_log_write>
2005e6d8:	4422                	c.lwsp	s0,8(sp)
2005e6da:	40b2                	c.lwsp	ra,12(sp)
2005e6dc:	4501                	c.li	a0,0
2005e6de:	0141                	c.addi	sp,16
2005e6e0:	c04fd06f          	jal	zero,2005bae4 <rtos_task_delete>
2005e6e4:	4505                	c.li	a0,1
2005e6e6:	c10fd0ef          	jal	ra,2005baf6 <rtos_time_delay_ms>
2005e6ea:	b755                	c.j	2005e68e <_init_thread+0xa>

2005e6ec <wlan_initialize>:
2005e6ec:	1141                	c.addi	sp,-16
2005e6ee:	c606                	c.swsp	ra,12(sp)
2005e6f0:	08c020ef          	jal	ra,2006077c <inic_host_init>
2005e6f4:	4505                	c.li	a0,1
2005e6f6:	f0bff0ef          	jal	ra,2005e600 <wifi_fast_connect_enable>
2005e6fa:	6705                	c.lui	a4,0x1
2005e6fc:	2005e637          	lui	a2,0x2005e
2005e700:	200635b7          	lui	a1,0x20063
2005e704:	4789                	c.li	a5,2
2005e706:	40070713          	addi	a4,a4,1024 # 1400 <CPU_ClkGet+0x8ea>
2005e70a:	4681                	c.li	a3,0
2005e70c:	68460613          	addi	a2,a2,1668 # 2005e684 <_init_thread>
2005e710:	ed858593          	addi	a1,a1,-296 # 20062ed8 <__func__.1+0x84>
2005e714:	4501                	c.li	a0,0
2005e716:	baafd0ef          	jal	ra,2005bac0 <rtos_task_create>
2005e71a:	c105                	c.beqz	a0,2005e73a <wlan_initialize+0x4e>
2005e71c:	40b2                	c.lwsp	ra,12(sp)
2005e71e:	200636b7          	lui	a3,0x20063
2005e722:	200635b7          	lui	a1,0x20063
2005e726:	ee068693          	addi	a3,a3,-288 # 20062ee0 <__func__.1+0x8c>
2005e72a:	04500613          	addi	a2,zero,69
2005e72e:	ed058593          	addi	a1,a1,-304 # 20062ed0 <__func__.1+0x7c>
2005e732:	4509                	c.li	a0,2
2005e734:	0141                	c.addi	sp,16
2005e736:	f3afc06f          	jal	zero,2005ae70 <rtk_log_write>
2005e73a:	40b2                	c.lwsp	ra,12(sp)
2005e73c:	0141                	c.addi	sp,16
2005e73e:	8082                	c.jr	ra

2005e740 <ChipInfo_GetSocName_ToBuf>:
2005e740:	7139                	c.addi16sp	sp,-64
2005e742:	da26                	c.swsp	s1,52(sp)
2005e744:	84ae                	c.mv	s1,a1
2005e746:	200635b7          	lui	a1,0x20063
2005e74a:	d84a                	c.swsp	s2,48(sp)
2005e74c:	4625                	c.li	a2,9
2005e74e:	892a                	c.mv	s2,a0
2005e750:	00758593          	addi	a1,a1,7 # 20063007 <__FUNCTION__.0+0x10f>
2005e754:	0068                	c.addi4spn	a0,sp,12
2005e756:	dc22                	c.swsp	s0,56(sp)
2005e758:	de06                	c.swsp	ra,60(sp)
2005e75a:	d64e                	c.swsp	s3,44(sp)
2005e75c:	d452                	c.swsp	s4,40(sp)
2005e75e:	dffab097          	auipc	ra,0xdffab
2005e762:	372080e7          	jalr	ra,882(ra) # 9ad0 <__wrap_memcpy>
2005e766:	00010aa3          	sb	zero,21(sp)
2005e76a:	4401                	c.li	s0,0
2005e76c:	c899                	c.beqz	s1,2005e782 <ChipInfo_GetSocName_ToBuf+0x42>
2005e76e:	20063637          	lui	a2,0x20063
2005e772:	0074                	c.addi4spn	a3,sp,12
2005e774:	03860613          	addi	a2,a2,56 # 20063038 <__FUNCTION__.0+0x140>
2005e778:	85a6                	c.mv	a1,s1
2005e77a:	854a                	c.mv	a0,s2
2005e77c:	27b000ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e780:	842a                	c.mv	s0,a0
2005e782:	7f000993          	addi	s3,zero,2032
2005e786:	20063a37          	lui	s4,0x20063
2005e78a:	00b10593          	addi	a1,sp,11
2005e78e:	854e                	c.mv	a0,s3
2005e790:	d0dfa0ef          	jal	ra,2005949c <OTP_Read8>
2005e794:	e131                	c.bnez	a0,2005e7d8 <ChipInfo_GetSocName_ToBuf+0x98>
2005e796:	200636b7          	lui	a3,0x20063
2005e79a:	200635b7          	lui	a1,0x20063
2005e79e:	04068693          	addi	a3,a3,64 # 20063040 <__FUNCTION__.0+0x148>
2005e7a2:	04500613          	addi	a2,zero,69
2005e7a6:	f4c58593          	addi	a1,a1,-180 # 20062f4c <__FUNCTION__.0+0x54>
2005e7aa:	4509                	c.li	a0,2
2005e7ac:	ec4fc0ef          	jal	ra,2005ae70 <rtk_log_write>
2005e7b0:	00947c63          	bgeu	s0,s1,2005e7c8 <ChipInfo_GetSocName_ToBuf+0x88>
2005e7b4:	20063637          	lui	a2,0x20063
2005e7b8:	94860613          	addi	a2,a2,-1720 # 20062948 <__FUNCTION__.0+0x208>
2005e7bc:	408485b3          	sub	a1,s1,s0
2005e7c0:	00890533          	add	a0,s2,s0
2005e7c4:	233000ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e7c8:	50f2                	c.lwsp	ra,60(sp)
2005e7ca:	5462                	c.lwsp	s0,56(sp)
2005e7cc:	54d2                	c.lwsp	s1,52(sp)
2005e7ce:	5942                	c.lwsp	s2,48(sp)
2005e7d0:	59b2                	c.lwsp	s3,44(sp)
2005e7d2:	5a22                	c.lwsp	s4,40(sp)
2005e7d4:	6121                	c.addi16sp	sp,64
2005e7d6:	8082                	c.jr	ra
2005e7d8:	00947d63          	bgeu	s0,s1,2005e7f2 <ChipInfo_GetSocName_ToBuf+0xb2>
2005e7dc:	00b14683          	lbu	a3,11(sp)
2005e7e0:	408485b3          	sub	a1,s1,s0
2005e7e4:	00890533          	add	a0,s2,s0
2005e7e8:	058a0613          	addi	a2,s4,88 # 20063058 <__FUNCTION__.0+0x160>
2005e7ec:	20b000ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e7f0:	942a                	c.add	s0,a0
2005e7f2:	0985                	c.addi	s3,1
2005e7f4:	80098793          	addi	a5,s3,-2048
2005e7f8:	fbc9                	c.bnez	a5,2005e78a <ChipInfo_GetSocName_ToBuf+0x4a>
2005e7fa:	bf5d                	c.j	2005e7b0 <ChipInfo_GetSocName_ToBuf+0x70>

2005e7fc <ChipInfo_GetLibVersion_ToBuf>:
2005e7fc:	1101                	c.addi	sp,-32
2005e7fe:	cc22                	c.swsp	s0,24(sp)
2005e800:	ca26                	c.swsp	s1,20(sp)
2005e802:	c64e                	c.swsp	s3,12(sp)
2005e804:	ce06                	c.swsp	ra,28(sp)
2005e806:	c84a                	c.swsp	s2,16(sp)
2005e808:	c452                	c.swsp	s4,8(sp)
2005e80a:	c256                	c.swsp	s5,4(sp)
2005e80c:	c05a                	c.swsp	s6,0(sp)
2005e80e:	4100c7b7          	lui	a5,0x4100c
2005e812:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005e816:	89aa                	c.mv	s3,a0
2005e818:	84ae                	c.mv	s1,a1
2005e81a:	4401                	c.li	s0,0
2005e81c:	c1b5                	c.beqz	a1,2005e880 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005e81e:	8399                	c.srli	a5,0x6
2005e820:	8b8d                	c.andi	a5,3
2005e822:	e7d9                	c.bnez	a5,2005e8b0 <ChipInfo_GetLibVersion_ToBuf+0xb4>
2005e824:	200636b7          	lui	a3,0x20063
2005e828:	cb468693          	addi	a3,a3,-844 # 20062cb4 <__FUNCTION__.0+0x574>
2005e82c:	20063637          	lui	a2,0x20063
2005e830:	06460613          	addi	a2,a2,100 # 20063064 <__FUNCTION__.0+0x16c>
2005e834:	85a6                	c.mv	a1,s1
2005e836:	854e                	c.mv	a0,s3
2005e838:	1bf000ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e83c:	842a                	c.mv	s0,a0
2005e83e:	04957163          	bgeu	a0,s1,2005e880 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005e842:	200636b7          	lui	a3,0x20063
2005e846:	20063637          	lui	a2,0x20063
2005e84a:	40a485b3          	sub	a1,s1,a0
2005e84e:	07868693          	addi	a3,a3,120 # 20063078 <__FUNCTION__.0+0x180>
2005e852:	08c60613          	addi	a2,a2,140 # 2006308c <__FUNCTION__.0+0x194>
2005e856:	954e                	c.add	a0,s3
2005e858:	19f000ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e85c:	942a                	c.add	s0,a0
2005e85e:	02947163          	bgeu	s0,s1,2005e880 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005e862:	200636b7          	lui	a3,0x20063
2005e866:	20063637          	lui	a2,0x20063
2005e86a:	408485b3          	sub	a1,s1,s0
2005e86e:	00898533          	add	a0,s3,s0
2005e872:	0a068693          	addi	a3,a3,160 # 200630a0 <__FUNCTION__.0+0x1a8>
2005e876:	0e060613          	addi	a2,a2,224 # 200630e0 <__FUNCTION__.0+0x1e8>
2005e87a:	17d000ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e87e:	942a                	c.add	s0,a0
2005e880:	20064937          	lui	s2,0x20064
2005e884:	20064a37          	lui	s4,0x20064
2005e888:	47490913          	addi	s2,s2,1140 # 20064474 <lib_pmc_git_rev>
2005e88c:	504a0a13          	addi	s4,s4,1284 # 20064504 <__frame_dummy_init_array_entry>
2005e890:	20063ab7          	lui	s5,0x20063
2005e894:	20063b37          	lui	s6,0x20063
2005e898:	03496163          	bltu	s2,s4,2005e8ba <ChipInfo_GetLibVersion_ToBuf+0xbe>
2005e89c:	40f2                	c.lwsp	ra,28(sp)
2005e89e:	4462                	c.lwsp	s0,24(sp)
2005e8a0:	44d2                	c.lwsp	s1,20(sp)
2005e8a2:	4942                	c.lwsp	s2,16(sp)
2005e8a4:	49b2                	c.lwsp	s3,12(sp)
2005e8a6:	4a22                	c.lwsp	s4,8(sp)
2005e8a8:	4a92                	c.lwsp	s5,4(sp)
2005e8aa:	4b02                	c.lwsp	s6,0(sp)
2005e8ac:	6105                	c.addi16sp	sp,32
2005e8ae:	8082                	c.jr	ra
2005e8b0:	200636b7          	lui	a3,0x20063
2005e8b4:	06068693          	addi	a3,a3,96 # 20063060 <__FUNCTION__.0+0x168>
2005e8b8:	bf95                	c.j	2005e82c <ChipInfo_GetLibVersion_ToBuf+0x30>
2005e8ba:	00094683          	lbu	a3,0(s2)
2005e8be:	e28d                	c.bnez	a3,2005e8e0 <ChipInfo_GetLibVersion_ToBuf+0xe4>
2005e8c0:	00194783          	lbu	a5,1(s2)
2005e8c4:	0905                	c.addi	s2,1
2005e8c6:	dfed                	c.beqz	a5,2005e8c0 <ChipInfo_GetLibVersion_ToBuf+0xc4>
2005e8c8:	fc9478e3          	bgeu	s0,s1,2005e898 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005e8cc:	948b0613          	addi	a2,s6,-1720 # 20062948 <__FUNCTION__.0+0x208>
2005e8d0:	408485b3          	sub	a1,s1,s0
2005e8d4:	00898533          	add	a0,s3,s0
2005e8d8:	11f000ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e8dc:	942a                	c.add	s0,a0
2005e8de:	bf6d                	c.j	2005e898 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005e8e0:	fa947ce3          	bgeu	s0,s1,2005e898 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005e8e4:	0f0a8613          	addi	a2,s5,240 # 200630f0 <__FUNCTION__.0+0x1f8>
2005e8e8:	408485b3          	sub	a1,s1,s0
2005e8ec:	00898533          	add	a0,s3,s0
2005e8f0:	0905                	c.addi	s2,1
2005e8f2:	105000ef          	jal	ra,2005f1f6 <DiagSnPrintf>
2005e8f6:	b7dd                	c.j	2005e8dc <ChipInfo_GetLibVersion_ToBuf+0xe0>

2005e8f8 <ChipInfo_GetChipSram>:
2005e8f8:	1101                	c.addi	sp,-32
2005e8fa:	ce06                	c.swsp	ra,28(sp)
2005e8fc:	4100c7b7          	lui	a5,0x4100c
2005e900:	2107a783          	lw	a5,528(a5) # 4100c210 <__km4_bd_boot_download_addr__+0x10ffa210>
2005e904:	40400737          	lui	a4,0x40400
2005e908:	8ff9                	c.and	a5,a4
2005e90a:	c39d                	c.beqz	a5,2005e930 <ChipInfo_GetChipSram+0x38>
2005e90c:	00f10593          	addi	a1,sp,15
2005e910:	7fc00513          	addi	a0,zero,2044
2005e914:	b89fa0ef          	jal	ra,2005949c <OTP_Read8>
2005e918:	00f14783          	lbu	a5,15(sp)
2005e91c:	470d                	c.li	a4,3
2005e91e:	8389                	c.srli	a5,0x2
2005e920:	8b8d                	c.andi	a5,3
2005e922:	00e79663          	bne	a5,a4,2005e92e <ChipInfo_GetChipSram+0x36>
2005e926:	4501                	c.li	a0,0
2005e928:	40f2                	c.lwsp	ra,28(sp)
2005e92a:	6105                	c.addi16sp	sp,32
2005e92c:	8082                	c.jr	ra
2005e92e:	dfe5                	c.beqz	a5,2005e926 <ChipInfo_GetChipSram+0x2e>
2005e930:	00f10593          	addi	a1,sp,15
2005e934:	7fc00513          	addi	a0,zero,2044
2005e938:	b65fa0ef          	jal	ra,2005949c <OTP_Read8>
2005e93c:	00f14783          	lbu	a5,15(sp)
2005e940:	470d                	c.li	a4,3
2005e942:	8399                	c.srli	a5,0x6
2005e944:	00e78363          	beq	a5,a4,2005e94a <ChipInfo_GetChipSram+0x52>
2005e948:	fff9                	c.bnez	a5,2005e926 <ChipInfo_GetChipSram+0x2e>
2005e94a:	4505                	c.li	a0,1
2005e94c:	bff1                	c.j	2005e928 <ChipInfo_GetChipSram+0x30>

2005e94e <vTaskStatus>:
2005e94e:	7139                	c.addi16sp	sp,-64
2005e950:	de06                	c.swsp	ra,60(sp)
2005e952:	dc22                	c.swsp	s0,56(sp)
2005e954:	da26                	c.swsp	s1,52(sp)
2005e956:	802ff0ef          	jal	ra,2005d958 <vTaskEnterCritical>
2005e95a:	e17fe0ef          	jal	ra,2005d770 <xTaskGetCurrentTaskHandle>
2005e95e:	4695                	c.li	a3,5
2005e960:	4605                	c.li	a2,1
2005e962:	006c                	c.addi4spn	a1,sp,12
2005e964:	842a                	c.mv	s0,a0
2005e966:	200634b7          	lui	s1,0x20063
2005e96a:	d16ff0ef          	jal	ra,2005de80 <vTaskGetInfo>
2005e96e:	0f448513          	addi	a0,s1,244 # 200630f4 <__FUNCTION__.0+0x1fc>
2005e972:	2d75                	c.jal	2005f02e <DiagPrintf>
2005e974:	45c2                	c.lwsp	a1,16(sp)
2005e976:	20063537          	lui	a0,0x20063
2005e97a:	11850513          	addi	a0,a0,280 # 20063118 <__FUNCTION__.0+0x220>
2005e97e:	2d45                	c.jal	2005f02e <DiagPrintf>
2005e980:	45d2                	c.lwsp	a1,20(sp)
2005e982:	20063537          	lui	a0,0x20063
2005e986:	12c50513          	addi	a0,a0,300 # 2006312c <__FUNCTION__.0+0x234>
2005e98a:	2555                	c.jal	2005f02e <DiagPrintf>
2005e98c:	20063537          	lui	a0,0x20063
2005e990:	85a2                	c.mv	a1,s0
2005e992:	13c50513          	addi	a0,a0,316 # 2006313c <__FUNCTION__.0+0x244>
2005e996:	2d61                	c.jal	2005f02e <DiagPrintf>
2005e998:	01814583          	lbu	a1,24(sp)
2005e99c:	200637b7          	lui	a5,0x20063
2005e9a0:	26078793          	addi	a5,a5,608 # 20063260 <TaskStateString.0>
2005e9a4:	00259713          	slli	a4,a1,0x2
2005e9a8:	97ba                	c.add	a5,a4
2005e9aa:	4390                	c.lw	a2,0(a5)
2005e9ac:	20063537          	lui	a0,0x20063
2005e9b0:	15050513          	addi	a0,a0,336 # 20063150 <__FUNCTION__.0+0x258>
2005e9b4:	2dad                	c.jal	2005f02e <DiagPrintf>
2005e9b6:	5582                	c.lwsp	a1,32(sp)
2005e9b8:	20063537          	lui	a0,0x20063
2005e9bc:	16c50513          	addi	a0,a0,364 # 2006316c <__FUNCTION__.0+0x274>
2005e9c0:	25bd                	c.jal	2005f02e <DiagPrintf>
2005e9c2:	45f2                	c.lwsp	a1,28(sp)
2005e9c4:	20063537          	lui	a0,0x20063
2005e9c8:	18050513          	addi	a0,a0,384 # 20063180 <__FUNCTION__.0+0x288>
2005e9cc:	258d                	c.jal	2005f02e <DiagPrintf>
2005e9ce:	5592                	c.lwsp	a1,36(sp)
2005e9d0:	20063537          	lui	a0,0x20063
2005e9d4:	19850513          	addi	a0,a0,408 # 20063198 <__FUNCTION__.0+0x2a0>
2005e9d8:	2d99                	c.jal	2005f02e <DiagPrintf>
2005e9da:	4474                	c.lw	a3,76(s0)
2005e9dc:	5818                	c.lw	a4,48(s0)
2005e9de:	5622                	c.lwsp	a2,40(sp)
2005e9e0:	400c                	c.lw	a1,0(s0)
2005e9e2:	40e68733          	sub	a4,a3,a4
2005e9e6:	8709                	c.srai	a4,0x2
2005e9e8:	20063537          	lui	a0,0x20063
2005e9ec:	0705                	c.addi	a4,1
2005e9ee:	1b050513          	addi	a0,a0,432 # 200631b0 <__FUNCTION__.0+0x2b8>
2005e9f2:	2d35                	c.jal	2005f02e <DiagPrintf>
2005e9f4:	02c15583          	lhu	a1,44(sp)
2005e9f8:	20063537          	lui	a0,0x20063
2005e9fc:	20050513          	addi	a0,a0,512 # 20063200 <__FUNCTION__.0+0x308>
2005ea00:	253d                	c.jal	2005f02e <DiagPrintf>
2005ea02:	0f448513          	addi	a0,s1,244
2005ea06:	2525                	c.jal	2005f02e <DiagPrintf>
2005ea08:	50f2                	c.lwsp	ra,60(sp)
2005ea0a:	5462                	c.lwsp	s0,56(sp)
2005ea0c:	54d2                	c.lwsp	s1,52(sp)
2005ea0e:	6121                	c.addi16sp	sp,64
2005ea10:	8082                	c.jr	ra

2005ea12 <vTaskCrashCallback>:
2005ea12:	1141                	c.addi	sp,-16
2005ea14:	c606                	c.swsp	ra,12(sp)
2005ea16:	884fd0ef          	jal	ra,2005ba9a <rtos_sched_get_state>
2005ea1a:	4785                	c.li	a5,1
2005ea1c:	00f50663          	beq	a0,a5,2005ea28 <vTaskCrashCallback+0x16>
2005ea20:	40b2                	c.lwsp	ra,12(sp)
2005ea22:	0141                	c.addi	sp,16
2005ea24:	f2bff06f          	jal	zero,2005e94e <vTaskStatus>
2005ea28:	40b2                	c.lwsp	ra,12(sp)
2005ea2a:	0141                	c.addi	sp,16
2005ea2c:	8082                	c.jr	ra

2005ea2e <print_unsigned_num>:
2005ea2e:	715d                	c.addi16sp	sp,-80
2005ea30:	c4a2                	c.swsp	s0,72(sp)
2005ea32:	c2a6                	c.swsp	s1,68(sp)
2005ea34:	c0ca                	c.swsp	s2,64(sp)
2005ea36:	de4e                	c.swsp	s3,60(sp)
2005ea38:	dc52                	c.swsp	s4,56(sp)
2005ea3a:	da56                	c.swsp	s5,52(sp)
2005ea3c:	d85a                	c.swsp	s6,48(sp)
2005ea3e:	d65e                	c.swsp	s7,44(sp)
2005ea40:	d462                	c.swsp	s8,40(sp)
2005ea42:	d266                	c.swsp	s9,36(sp)
2005ea44:	d06a                	c.swsp	s10,32(sp)
2005ea46:	c686                	c.swsp	ra,76(sp)
2005ea48:	8c2a                	c.mv	s8,a0
2005ea4a:	8cae                	c.mv	s9,a1
2005ea4c:	8d32                	c.mv	s10,a2
2005ea4e:	8936                	c.mv	s2,a3
2005ea50:	8bba                	c.mv	s7,a4
2005ea52:	89be                	c.mv	s3,a5
2005ea54:	84c2                	c.mv	s1,a6
2005ea56:	4401                	c.li	s0,0
2005ea58:	4a81                	c.li	s5,0
2005ea5a:	4a25                	c.li	s4,9
2005ea5c:	4b05                	c.li	s6,1
2005ea5e:	866a                	c.mv	a2,s10
2005ea60:	4681                	c.li	a3,0
2005ea62:	8562                	c.mv	a0,s8
2005ea64:	85e6                	c.mv	a1,s9
2005ea66:	6da020ef          	jal	ra,20061140 <__umoddi3>
2005ea6a:	0ff57893          	andi	a7,a0,255
2005ea6e:	04aa6863          	bltu	s4,a0,2005eabe <print_unsigned_num+0x90>
2005ea72:	03088893          	addi	a7,a7,48 # f8000030 <__ctrace_end__+0x2e000030>
2005ea76:	007c                	c.addi4spn	a5,sp,12
2005ea78:	97a2                	c.add	a5,s0
2005ea7a:	0ff8f893          	andi	a7,a7,255
2005ea7e:	01178023          	sb	a7,0(a5)
2005ea82:	866a                	c.mv	a2,s10
2005ea84:	4681                	c.li	a3,0
2005ea86:	8562                	c.mv	a0,s8
2005ea88:	85e6                	c.mv	a1,s9
2005ea8a:	0405                	c.addi	s0,1
2005ea8c:	354020ef          	jal	ra,20060de0 <__udivdi3>
2005ea90:	039a9463          	bne	s5,s9,2005eab8 <print_unsigned_num+0x8a>
2005ea94:	03ac7263          	bgeu	s8,s10,2005eab8 <print_unsigned_num+0x8a>
2005ea98:	c8d5                	c.beqz	s1,2005eb4c <print_unsigned_num+0x11e>
2005ea9a:	03000793          	addi	a5,zero,48
2005ea9e:	1bfd                	c.addi	s7,-1
2005eaa0:	0af91163          	bne	s2,a5,2005eb42 <print_unsigned_num+0x114>
2005eaa4:	02d00513          	addi	a0,zero,45
2005eaa8:	dffa6097          	auipc	ra,0xdffa6
2005eaac:	a9c080e7          	jalr	ra,-1380(ra) # 4544 <LOGUART_PutChar>
2005eab0:	89a6                	c.mv	s3,s1
2005eab2:	09704b63          	blt	zero,s7,2005eb48 <print_unsigned_num+0x11a>
2005eab6:	a0b1                	c.j	2005eb02 <print_unsigned_num+0xd4>
2005eab8:	8c2a                	c.mv	s8,a0
2005eaba:	8cae                	c.mv	s9,a1
2005eabc:	b74d                	c.j	2005ea5e <print_unsigned_num+0x30>
2005eabe:	01699563          	bne	s3,s6,2005eac8 <print_unsigned_num+0x9a>
2005eac2:	03788893          	addi	a7,a7,55
2005eac6:	bf45                	c.j	2005ea76 <print_unsigned_num+0x48>
2005eac8:	05788893          	addi	a7,a7,87
2005eacc:	b76d                	c.j	2005ea76 <print_unsigned_num+0x48>
2005eace:	854a                	c.mv	a0,s2
2005ead0:	dffa6097          	auipc	ra,0xdffa6
2005ead4:	a74080e7          	jalr	ra,-1420(ra) # 4544 <LOGUART_PutChar>
2005ead8:	1a7d                	c.addi	s4,-1
2005eada:	ff444ae3          	blt	s0,s4,2005eace <print_unsigned_num+0xa0>
2005eade:	4781                	c.li	a5,0
2005eae0:	008bc463          	blt	s7,s0,2005eae8 <print_unsigned_num+0xba>
2005eae4:	408b87b3          	sub	a5,s7,s0
2005eae8:	97ce                	c.add	a5,s3
2005eaea:	c0a9                	c.beqz	s1,2005eb2c <print_unsigned_num+0xfe>
2005eaec:	84be                	c.mv	s1,a5
2005eaee:	0df97913          	andi	s2,s2,223
2005eaf2:	00091863          	bne	s2,zero,2005eb02 <print_unsigned_num+0xd4>
2005eaf6:	02d00513          	addi	a0,zero,45
2005eafa:	dffa6097          	auipc	ra,0xdffa6
2005eafe:	a4a080e7          	jalr	ra,-1462(ra) # 4544 <LOGUART_PutChar>
2005eb02:	8922                	c.mv	s2,s0
2005eb04:	59fd                	c.li	s3,-1
2005eb06:	197d                	c.addi	s2,-1
2005eb08:	03391463          	bne	s2,s3,2005eb30 <print_unsigned_num+0x102>
2005eb0c:	00940533          	add	a0,s0,s1
2005eb10:	40b6                	c.lwsp	ra,76(sp)
2005eb12:	4426                	c.lwsp	s0,72(sp)
2005eb14:	4496                	c.lwsp	s1,68(sp)
2005eb16:	4906                	c.lwsp	s2,64(sp)
2005eb18:	59f2                	c.lwsp	s3,60(sp)
2005eb1a:	5a62                	c.lwsp	s4,56(sp)
2005eb1c:	5ad2                	c.lwsp	s5,52(sp)
2005eb1e:	5b42                	c.lwsp	s6,48(sp)
2005eb20:	5bb2                	c.lwsp	s7,44(sp)
2005eb22:	5c22                	c.lwsp	s8,40(sp)
2005eb24:	5c92                	c.lwsp	s9,36(sp)
2005eb26:	5d02                	c.lwsp	s10,32(sp)
2005eb28:	6161                	c.addi16sp	sp,80
2005eb2a:	8082                	c.jr	ra
2005eb2c:	84be                	c.mv	s1,a5
2005eb2e:	bfd1                	c.j	2005eb02 <print_unsigned_num+0xd4>
2005eb30:	007c                	c.addi4spn	a5,sp,12
2005eb32:	97ca                	c.add	a5,s2
2005eb34:	0007c503          	lbu	a0,0(a5)
2005eb38:	dffa6097          	auipc	ra,0xdffa6
2005eb3c:	a0c080e7          	jalr	ra,-1524(ra) # 4544 <LOGUART_PutChar>
2005eb40:	b7d9                	c.j	2005eb06 <print_unsigned_num+0xd8>
2005eb42:	fb7056e3          	bge	zero,s7,2005eaee <print_unsigned_num+0xc0>
2005eb46:	89a6                	c.mv	s3,s1
2005eb48:	8a5e                	c.mv	s4,s7
2005eb4a:	bf41                	c.j	2005eada <print_unsigned_num+0xac>
2005eb4c:	4981                	c.li	s3,0
2005eb4e:	b795                	c.j	2005eab2 <print_unsigned_num+0x84>

2005eb50 <pad_char_control>:
2005eb50:	1141                	c.addi	sp,-16
2005eb52:	c422                	c.swsp	s0,8(sp)
2005eb54:	c226                	c.swsp	s1,4(sp)
2005eb56:	c04a                	c.swsp	s2,0(sp)
2005eb58:	c606                	c.swsp	ra,12(sp)
2005eb5a:	842a                	c.mv	s0,a0
2005eb5c:	892e                	c.mv	s2,a1
2005eb5e:	84aa                	c.mv	s1,a0
2005eb60:	00904c63          	blt	zero,s1,2005eb78 <pad_char_control+0x28>
2005eb64:	8522                	c.mv	a0,s0
2005eb66:	00045363          	bge	s0,zero,2005eb6c <pad_char_control+0x1c>
2005eb6a:	4501                	c.li	a0,0
2005eb6c:	40b2                	c.lwsp	ra,12(sp)
2005eb6e:	4422                	c.lwsp	s0,8(sp)
2005eb70:	4492                	c.lwsp	s1,4(sp)
2005eb72:	4902                	c.lwsp	s2,0(sp)
2005eb74:	0141                	c.addi	sp,16
2005eb76:	8082                	c.jr	ra
2005eb78:	854a                	c.mv	a0,s2
2005eb7a:	dffa6097          	auipc	ra,0xdffa6
2005eb7e:	9ca080e7          	jalr	ra,-1590(ra) # 4544 <LOGUART_PutChar>
2005eb82:	14fd                	c.addi	s1,-1
2005eb84:	bff1                	c.j	2005eb60 <pad_char_control+0x10>

2005eb86 <print_decimal_num>:
2005eb86:	1101                	c.addi	sp,-32
2005eb88:	c64e                	c.swsp	s3,12(sp)
2005eb8a:	200689b7          	lui	s3,0x20068
2005eb8e:	cc22                	c.swsp	s0,24(sp)
2005eb90:	c84a                	c.swsp	s2,16(sp)
2005eb92:	c256                	c.swsp	s5,4(sp)
2005eb94:	ce06                	c.swsp	ra,28(sp)
2005eb96:	8ab2                	c.mv	s5,a2
2005eb98:	ca26                	c.swsp	s1,20(sp)
2005eb9a:	c452                	c.swsp	s4,8(sp)
2005eb9c:	872a                	c.mv	a4,a0
2005eb9e:	8936                	c.mv	s2,a3
2005eba0:	4401                	c.li	s0,0
2005eba2:	4625                	c.li	a2,9
2005eba4:	e2898993          	addi	s3,s3,-472 # 20067e28 <num_buf_temp>
2005eba8:	02b776b3          	remu	a3,a4,a1
2005ebac:	0ff6f793          	andi	a5,a3,255
2005ebb0:	04d66963          	bltu	a2,a3,2005ec02 <print_decimal_num+0x7c>
2005ebb4:	03078793          	addi	a5,a5,48
2005ebb8:	0ff7f793          	andi	a5,a5,255
2005ebbc:	008986b3          	add	a3,s3,s0
2005ebc0:	00f68023          	sb	a5,0(a3)
2005ebc4:	00140493          	addi	s1,s0,1
2005ebc8:	02b757b3          	divu	a5,a4,a1
2005ebcc:	02b77863          	bgeu	a4,a1,2005ebfc <print_decimal_num+0x76>
2005ebd0:	8a4a                	c.mv	s4,s2
2005ebd2:	05204163          	blt	zero,s2,2005ec14 <print_decimal_num+0x8e>
2005ebd6:	4901                	c.li	s2,0
2005ebd8:	14fd                	c.addi	s1,-1
2005ebda:	0404d663          	bge	s1,zero,2005ec26 <print_decimal_num+0xa0>
2005ebde:	00045363          	bge	s0,zero,2005ebe4 <print_decimal_num+0x5e>
2005ebe2:	547d                	c.li	s0,-1
2005ebe4:	0405                	c.addi	s0,1
2005ebe6:	01240533          	add	a0,s0,s2
2005ebea:	40f2                	c.lwsp	ra,28(sp)
2005ebec:	4462                	c.lwsp	s0,24(sp)
2005ebee:	44d2                	c.lwsp	s1,20(sp)
2005ebf0:	4942                	c.lwsp	s2,16(sp)
2005ebf2:	49b2                	c.lwsp	s3,12(sp)
2005ebf4:	4a22                	c.lwsp	s4,8(sp)
2005ebf6:	4a92                	c.lwsp	s5,4(sp)
2005ebf8:	6105                	c.addi16sp	sp,32
2005ebfa:	8082                	c.jr	ra
2005ebfc:	8426                	c.mv	s0,s1
2005ebfe:	873e                	c.mv	a4,a5
2005ec00:	b765                	c.j	2005eba8 <print_decimal_num+0x22>
2005ec02:	05778793          	addi	a5,a5,87
2005ec06:	bf4d                	c.j	2005ebb8 <print_decimal_num+0x32>
2005ec08:	8556                	c.mv	a0,s5
2005ec0a:	dffa6097          	auipc	ra,0xdffa6
2005ec0e:	93a080e7          	jalr	ra,-1734(ra) # 4544 <LOGUART_PutChar>
2005ec12:	1a7d                	c.addi	s4,-1
2005ec14:	ff44cae3          	blt	s1,s4,2005ec08 <print_decimal_num+0x82>
2005ec18:	4781                	c.li	a5,0
2005ec1a:	00994463          	blt	s2,s1,2005ec22 <print_decimal_num+0x9c>
2005ec1e:	409907b3          	sub	a5,s2,s1
2005ec22:	893e                	c.mv	s2,a5
2005ec24:	bf55                	c.j	2005ebd8 <print_decimal_num+0x52>
2005ec26:	009987b3          	add	a5,s3,s1
2005ec2a:	0007c503          	lbu	a0,0(a5)
2005ec2e:	dffa6097          	auipc	ra,0xdffa6
2005ec32:	916080e7          	jalr	ra,-1770(ra) # 4544 <LOGUART_PutChar>
2005ec36:	b74d                	c.j	2005ebd8 <print_decimal_num+0x52>

2005ec38 <pad_num_control>:
2005ec38:	04088163          	beq	a7,zero,2005ec7a <pad_num_control+0x42>
2005ec3c:	1141                	c.addi	sp,-16
2005ec3e:	c226                	c.swsp	s1,4(sp)
2005ec40:	84ba                	c.mv	s1,a4
2005ec42:	4701                	c.li	a4,0
2005ec44:	c422                	c.swsp	s0,8(sp)
2005ec46:	c04a                	c.swsp	s2,0(sp)
2005ec48:	c606                	c.swsp	ra,12(sp)
2005ec4a:	8936                	c.mv	s2,a3
2005ec4c:	de3ff0ef          	jal	ra,2005ea2e <print_unsigned_num>
2005ec50:	842a                	c.mv	s0,a0
2005ec52:	00955d63          	bge	a0,s1,2005ec6c <pad_num_control+0x34>
2005ec56:	02000793          	addi	a5,zero,32
2005ec5a:	00f91963          	bne	s2,a5,2005ec6c <pad_num_control+0x34>
2005ec5e:	02000593          	addi	a1,zero,32
2005ec62:	40a48533          	sub	a0,s1,a0
2005ec66:	eebff0ef          	jal	ra,2005eb50 <pad_char_control>
2005ec6a:	942a                	c.add	s0,a0
2005ec6c:	40b2                	c.lwsp	ra,12(sp)
2005ec6e:	8522                	c.mv	a0,s0
2005ec70:	4422                	c.lwsp	s0,8(sp)
2005ec72:	4492                	c.lwsp	s1,4(sp)
2005ec74:	4902                	c.lwsp	s2,0(sp)
2005ec76:	0141                	c.addi	sp,16
2005ec78:	8082                	c.jr	ra
2005ec7a:	db5ff06f          	jal	zero,2005ea2e <print_unsigned_num>

2005ec7e <_rand>:
2005ec7e:	1141                	c.addi	sp,-16
2005ec80:	450d                	c.li	a0,3
2005ec82:	c606                	c.swsp	ra,12(sp)
2005ec84:	a6bfa0ef          	jal	ra,200596ee <PLL_State>
2005ec88:	c50d                	c.beqz	a0,2005ecb2 <_rand+0x34>
2005ec8a:	65c1                	c.lui	a1,0x10
2005ec8c:	410167b7          	lui	a5,0x41016
2005ec90:	453d                	c.li	a0,15
2005ec92:	4685                	c.li	a3,1
2005ec94:	00158713          	addi	a4,a1,1 # 10001 <mbedtls_ecp_check_pub_priv+0x123>
2005ec98:	47d0                	c.lw	a2,12(a5)
2005ec9a:	00a61763          	bne	a2,a0,2005eca8 <_rand+0x2a>
2005ec9e:	df94                	c.sw	a3,56(a5)
2005eca0:	177d                	c.addi	a4,-1
2005eca2:	fb7d                	c.bnez	a4,2005ec98 <_rand+0x1a>
2005eca4:	df94                	c.sw	a3,56(a5)
2005eca6:	b7fd                	c.j	2005ec94 <_rand+0x16>
2005eca8:	de65                	c.beqz	a2,2005eca0 <_rand+0x22>
2005ecaa:	4b88                	c.lw	a0,16(a5)
2005ecac:	40b2                	c.lwsp	ra,12(sp)
2005ecae:	0141                	c.addi	sp,16
2005ecb0:	8082                	c.jr	ra
2005ecb2:	fb818693          	addi	a3,gp,-72 # 20066090 <rand_first.1>
2005ecb6:	429c                	c.lw	a5,0(a3)
2005ecb8:	20068737          	lui	a4,0x20068
2005ecbc:	e3470713          	addi	a4,a4,-460 # 20067e34 <rand_seed.0>
2005ecc0:	eb91                	c.bnez	a5,2005ecd4 <_rand+0x56>
2005ecc2:	678d                	c.lui	a5,0x3
2005ecc4:	03978793          	addi	a5,a5,57 # 3039 <FLASH_SetSpiMode+0x375>
2005ecc8:	c31c                	c.sw	a5,0(a4)
2005ecca:	c35c                	c.sw	a5,4(a4)
2005eccc:	c71c                	c.sw	a5,8(a4)
2005ecce:	c75c                	c.sw	a5,12(a4)
2005ecd0:	4785                	c.li	a5,1
2005ecd2:	c29c                	c.sw	a5,0(a3)
2005ecd4:	4314                	c.lw	a3,0(a4)
2005ecd6:	fff80537          	lui	a0,0xfff80
2005ecda:	474c                	c.lw	a1,12(a4)
2005ecdc:	00669793          	slli	a5,a3,0x6
2005ece0:	8fb5                	c.xor	a5,a3
2005ece2:	06ca                	c.slli	a3,0x12
2005ece4:	8ee9                	c.and	a3,a0
2005ece6:	83b5                	c.srli	a5,0xd
2005ece8:	8fb5                	c.xor	a5,a3
2005ecea:	4354                	c.lw	a3,4(a4)
2005ecec:	fff00837          	lui	a6,0xfff00
2005ecf0:	c31c                	c.sw	a5,0(a4)
2005ecf2:	00269613          	slli	a2,a3,0x2
2005ecf6:	00c6c533          	xor	a0,a3,a2
2005ecfa:	4714                	c.lw	a3,8(a4)
2005ecfc:	9a01                	c.andi	a2,-32
2005ecfe:	816d                	c.srli	a0,0x1b
2005ed00:	8d31                	c.xor	a0,a2
2005ed02:	00d69613          	slli	a2,a3,0xd
2005ed06:	8e35                	c.xor	a2,a3
2005ed08:	069e                	c.slli	a3,0x7
2005ed0a:	8006f693          	andi	a3,a3,-2048
2005ed0e:	8255                	c.srli	a2,0x15
2005ed10:	8e35                	c.xor	a2,a3
2005ed12:	00359693          	slli	a3,a1,0x3
2005ed16:	8ead                	c.xor	a3,a1
2005ed18:	05b6                	c.slli	a1,0xd
2005ed1a:	c348                	c.sw	a0,4(a4)
2005ed1c:	82b1                	c.srli	a3,0xc
2005ed1e:	0105f5b3          	and	a1,a1,a6
2005ed22:	8d3d                	c.xor	a0,a5
2005ed24:	8ead                	c.xor	a3,a1
2005ed26:	8d31                	c.xor	a0,a2
2005ed28:	c710                	c.sw	a2,8(a4)
2005ed2a:	c754                	c.sw	a3,12(a4)
2005ed2c:	8d35                	c.xor	a0,a3
2005ed2e:	bfbd                	c.j	2005ecac <_rand+0x2e>

2005ed30 <DiagVprintf>:
2005ed30:	7139                	c.addi16sp	sp,-64
2005ed32:	d452                	c.swsp	s4,40(sp)
2005ed34:	20063a37          	lui	s4,0x20063
2005ed38:	dc22                	c.swsp	s0,56(sp)
2005ed3a:	da26                	c.swsp	s1,52(sp)
2005ed3c:	d84a                	c.swsp	s2,48(sp)
2005ed3e:	d64e                	c.swsp	s3,44(sp)
2005ed40:	d256                	c.swsp	s5,36(sp)
2005ed42:	d05a                	c.swsp	s6,32(sp)
2005ed44:	de06                	c.swsp	ra,60(sp)
2005ed46:	ce5e                	c.swsp	s7,28(sp)
2005ed48:	cc62                	c.swsp	s8,24(sp)
2005ed4a:	ca66                	c.swsp	s9,20(sp)
2005ed4c:	892a                	c.mv	s2,a0
2005ed4e:	842e                	c.mv	s0,a1
2005ed50:	4981                	c.li	s3,0
2005ed52:	4481                	c.li	s1,0
2005ed54:	278a0a13          	addi	s4,s4,632 # 20063278 <TaskStateString.0+0x18>
2005ed58:	4aa5                	c.li	s5,9
2005ed5a:	4b29                	c.li	s6,10
2005ed5c:	00094503          	lbu	a0,0(s2)
2005ed60:	ed19                	c.bnez	a0,2005ed7e <DiagVprintf+0x4e>
2005ed62:	50f2                	c.lwsp	ra,60(sp)
2005ed64:	5462                	c.lwsp	s0,56(sp)
2005ed66:	5942                	c.lwsp	s2,48(sp)
2005ed68:	59b2                	c.lwsp	s3,44(sp)
2005ed6a:	5a22                	c.lwsp	s4,40(sp)
2005ed6c:	5a92                	c.lwsp	s5,36(sp)
2005ed6e:	5b02                	c.lwsp	s6,32(sp)
2005ed70:	4bf2                	c.lwsp	s7,28(sp)
2005ed72:	4c62                	c.lwsp	s8,24(sp)
2005ed74:	4cd2                	c.lwsp	s9,20(sp)
2005ed76:	8526                	c.mv	a0,s1
2005ed78:	54d2                	c.lwsp	s1,52(sp)
2005ed7a:	6121                	c.addi16sp	sp,64
2005ed7c:	8082                	c.jr	ra
2005ed7e:	02500793          	addi	a5,zero,37
2005ed82:	0905                	c.addi	s2,1
2005ed84:	28f51f63          	bne	a0,a5,2005f022 <DiagVprintf+0x2f2>
2005ed88:	4881                	c.li	a7,0
2005ed8a:	4701                	c.li	a4,0
2005ed8c:	4681                	c.li	a3,0
2005ed8e:	4601                	c.li	a2,0
2005ed90:	07a00e13          	addi	t3,zero,122
2005ed94:	05700e93          	addi	t4,zero,87
2005ed98:	02200f13          	addi	t5,zero,34
2005ed9c:	03000593          	addi	a1,zero,48
2005eda0:	04300813          	addi	a6,zero,67
2005eda4:	04c00f93          	addi	t6,zero,76
2005eda8:	03900293          	addi	t0,zero,57
2005edac:	02000393          	addi	t2,zero,32
2005edb0:	02500313          	addi	t1,zero,37
2005edb4:	02d00b93          	addi	s7,zero,45
2005edb8:	00094783          	lbu	a5,0(s2)
2005edbc:	02fe6063          	bltu	t3,a5,2005eddc <DiagVprintf+0xac>
2005edc0:	02fee063          	bltu	t4,a5,2005ede0 <DiagVprintf+0xb0>
2005edc4:	22b78663          	beq	a5,a1,2005eff0 <DiagVprintf+0x2c0>
2005edc8:	02f5eb63          	bltu	a1,a5,2005edfe <DiagVprintf+0xce>
2005edcc:	20678963          	beq	a5,t1,2005efde <DiagVprintf+0x2ae>
2005edd0:	02f36263          	bltu	t1,a5,2005edf4 <DiagVprintf+0xc4>
2005edd4:	20078c63          	beq	a5,zero,2005efec <DiagVprintf+0x2bc>
2005edd8:	24778163          	beq	a5,t2,2005f01a <DiagVprintf+0x2ea>
2005eddc:	54fd                	c.li	s1,-1
2005edde:	b751                	c.j	2005ed62 <DiagVprintf+0x32>
2005ede0:	fa878793          	addi	a5,a5,-88
2005ede4:	0ff7f793          	andi	a5,a5,255
2005ede8:	feff6ae3          	bltu	t5,a5,2005eddc <DiagVprintf+0xac>
2005edec:	078a                	c.slli	a5,0x2
2005edee:	97d2                	c.add	a5,s4
2005edf0:	439c                	c.lw	a5,0(a5)
2005edf2:	8782                	c.jr	a5
2005edf4:	ff7794e3          	bne	a5,s7,2005eddc <DiagVprintf+0xac>
2005edf8:	0905                	c.addi	s2,1
2005edfa:	0885                	c.addi	a7,1
2005edfc:	bf75                	c.j	2005edb8 <DiagVprintf+0x88>
2005edfe:	19078363          	beq	a5,a6,2005ef84 <DiagVprintf+0x254>
2005ee02:	02f86563          	bltu	a6,a5,2005ee2c <DiagVprintf+0xfc>
2005ee06:	20f2fb63          	bgeu	t0,a5,2005f01c <DiagVprintf+0x2ec>
2005ee0a:	04200593          	addi	a1,zero,66
2005ee0e:	fcb797e3          	bne	a5,a1,2005eddc <DiagVprintf+0xac>
2005ee12:	4785                	c.li	a5,1
2005ee14:	0ac7d063          	bge	a5,a2,2005eeb4 <DiagVprintf+0x184>
2005ee18:	041d                	c.addi	s0,7
2005ee1a:	9861                	c.andi	s0,-8
2005ee1c:	4008                	c.lw	a0,0(s0)
2005ee1e:	404c                	c.lw	a1,4(s0)
2005ee20:	00840b93          	addi	s7,s0,8
2005ee24:	4801                	c.li	a6,0
2005ee26:	87ce                	c.mv	a5,s3
2005ee28:	4609                	c.li	a2,2
2005ee2a:	a889                	c.j	2005ee7c <DiagVprintf+0x14c>
2005ee2c:	1bf78663          	beq	a5,t6,2005efd8 <DiagVprintf+0x2a8>
2005ee30:	04f00593          	addi	a1,zero,79
2005ee34:	fab794e3          	bne	a5,a1,2005eddc <DiagVprintf+0xac>
2005ee38:	4785                	c.li	a5,1
2005ee3a:	08c7d263          	bge	a5,a2,2005eebe <DiagVprintf+0x18e>
2005ee3e:	041d                	c.addi	s0,7
2005ee40:	9861                	c.andi	s0,-8
2005ee42:	4008                	c.lw	a0,0(s0)
2005ee44:	404c                	c.lw	a1,4(s0)
2005ee46:	00840b93          	addi	s7,s0,8
2005ee4a:	4801                	c.li	a6,0
2005ee4c:	87ce                	c.mv	a5,s3
2005ee4e:	4621                	c.li	a2,8
2005ee50:	a035                	c.j	2005ee7c <DiagVprintf+0x14c>
2005ee52:	4785                	c.li	a5,1
2005ee54:	02c7da63          	bge	a5,a2,2005ee88 <DiagVprintf+0x158>
2005ee58:	041d                	c.addi	s0,7
2005ee5a:	9861                	c.andi	s0,-8
2005ee5c:	4008                	c.lw	a0,0(s0)
2005ee5e:	404c                	c.lw	a1,4(s0)
2005ee60:	00840b93          	addi	s7,s0,8
2005ee64:	0405d163          	bge	a1,zero,2005eea6 <DiagVprintf+0x176>
2005ee68:	00a037b3          	sltu	a5,zero,a0
2005ee6c:	40b005b3          	sub	a1,zero,a1
2005ee70:	40a00533          	sub	a0,zero,a0
2005ee74:	8d9d                	c.sub	a1,a5
2005ee76:	4805                	c.li	a6,1
2005ee78:	87ce                	c.mv	a5,s3
2005ee7a:	4629                	c.li	a2,10
2005ee7c:	dbdff0ef          	jal	ra,2005ec38 <pad_num_control>
2005ee80:	94aa                	c.add	s1,a0
2005ee82:	0905                	c.addi	s2,1
2005ee84:	845e                	c.mv	s0,s7
2005ee86:	bdd9                	c.j	2005ed5c <DiagVprintf+0x2c>
2005ee88:	4008                	c.lw	a0,0(s0)
2005ee8a:	00440b93          	addi	s7,s0,4
2005ee8e:	41f55593          	srai	a1,a0,0x1f
2005ee92:	bfc9                	c.j	2005ee64 <DiagVprintf+0x134>
2005ee94:	4785                	c.li	a5,1
2005ee96:	00c7da63          	bge	a5,a2,2005eeaa <DiagVprintf+0x17a>
2005ee9a:	041d                	c.addi	s0,7
2005ee9c:	9861                	c.andi	s0,-8
2005ee9e:	4008                	c.lw	a0,0(s0)
2005eea0:	404c                	c.lw	a1,4(s0)
2005eea2:	00840b93          	addi	s7,s0,8
2005eea6:	4801                	c.li	a6,0
2005eea8:	bfc1                	c.j	2005ee78 <DiagVprintf+0x148>
2005eeaa:	4008                	c.lw	a0,0(s0)
2005eeac:	4581                	c.li	a1,0
2005eeae:	00440b93          	addi	s7,s0,4
2005eeb2:	bfd5                	c.j	2005eea6 <DiagVprintf+0x176>
2005eeb4:	4008                	c.lw	a0,0(s0)
2005eeb6:	4581                	c.li	a1,0
2005eeb8:	00440b93          	addi	s7,s0,4
2005eebc:	b7a5                	c.j	2005ee24 <DiagVprintf+0xf4>
2005eebe:	4008                	c.lw	a0,0(s0)
2005eec0:	4581                	c.li	a1,0
2005eec2:	00440b93          	addi	s7,s0,4
2005eec6:	b751                	c.j	2005ee4a <DiagVprintf+0x11a>
2005eec8:	4985                	c.li	s3,1
2005eeca:	4785                	c.li	a5,1
2005eecc:	00c7dc63          	bge	a5,a2,2005eee4 <DiagVprintf+0x1b4>
2005eed0:	041d                	c.addi	s0,7
2005eed2:	9861                	c.andi	s0,-8
2005eed4:	4008                	c.lw	a0,0(s0)
2005eed6:	404c                	c.lw	a1,4(s0)
2005eed8:	00840b93          	addi	s7,s0,8
2005eedc:	4801                	c.li	a6,0
2005eede:	87ce                	c.mv	a5,s3
2005eee0:	4641                	c.li	a2,16
2005eee2:	bf69                	c.j	2005ee7c <DiagVprintf+0x14c>
2005eee4:	4008                	c.lw	a0,0(s0)
2005eee6:	4581                	c.li	a1,0
2005eee8:	00440b93          	addi	s7,s0,4
2005eeec:	bfc5                	c.j	2005eedc <DiagVprintf+0x1ac>
2005eeee:	4008                	c.lw	a0,0(s0)
2005eef0:	4801                	c.li	a6,0
2005eef2:	4785                	c.li	a5,1
2005eef4:	4721                	c.li	a4,8
2005eef6:	03000693          	addi	a3,zero,48
2005eefa:	4641                	c.li	a2,16
2005eefc:	4581                	c.li	a1,0
2005eefe:	d3bff0ef          	jal	ra,2005ec38 <pad_num_control>
2005ef02:	00440b93          	addi	s7,s0,4
2005ef06:	94aa                	c.add	s1,a0
2005ef08:	4985                	c.li	s3,1
2005ef0a:	bfa5                	c.j	2005ee82 <DiagVprintf+0x152>
2005ef0c:	00042c83          	lw	s9,0(s0)
2005ef10:	00440b93          	addi	s7,s0,4
2005ef14:	4401                	c.li	s0,0
2005ef16:	008c87b3          	add	a5,s9,s0
2005ef1a:	0007c783          	lbu	a5,0(a5)
2005ef1e:	e7a9                	c.bnez	a5,2005ef68 <DiagVprintf+0x238>
2005ef20:	02089063          	bne	a7,zero,2005ef40 <DiagVprintf+0x210>
2005ef24:	40870533          	sub	a0,a4,s0
2005ef28:	00a05c63          	bge	zero,a0,2005ef40 <DiagVprintf+0x210>
2005ef2c:	85b6                	c.mv	a1,a3
2005ef2e:	c646                	c.swsp	a7,12(sp)
2005ef30:	c43a                	c.swsp	a4,8(sp)
2005ef32:	c236                	c.swsp	a3,4(sp)
2005ef34:	c1dff0ef          	jal	ra,2005eb50 <pad_char_control>
2005ef38:	48b2                	c.lwsp	a7,12(sp)
2005ef3a:	4722                	c.lwsp	a4,8(sp)
2005ef3c:	4692                	c.lwsp	a3,4(sp)
2005ef3e:	94aa                	c.add	s1,a0
2005ef40:	5c7d                	c.li	s8,-1
2005ef42:	000c8863          	beq	s9,zero,2005ef52 <DiagVprintf+0x222>
2005ef46:	4c01                	c.li	s8,0
2005ef48:	018c87b3          	add	a5,s9,s8
2005ef4c:	0007c503          	lbu	a0,0(a5)
2005ef50:	ed11                	c.bnez	a0,2005ef6c <DiagVprintf+0x23c>
2005ef52:	94e2                	c.add	s1,s8
2005ef54:	f20887e3          	beq	a7,zero,2005ee82 <DiagVprintf+0x152>
2005ef58:	40870533          	sub	a0,a4,s0
2005ef5c:	85b6                	c.mv	a1,a3
2005ef5e:	f2a052e3          	bge	zero,a0,2005ee82 <DiagVprintf+0x152>
2005ef62:	befff0ef          	jal	ra,2005eb50 <pad_char_control>
2005ef66:	bf29                	c.j	2005ee80 <DiagVprintf+0x150>
2005ef68:	0405                	c.addi	s0,1
2005ef6a:	b775                	c.j	2005ef16 <DiagVprintf+0x1e6>
2005ef6c:	c646                	c.swsp	a7,12(sp)
2005ef6e:	c43a                	c.swsp	a4,8(sp)
2005ef70:	c236                	c.swsp	a3,4(sp)
2005ef72:	dffa5097          	auipc	ra,0xdffa5
2005ef76:	5d2080e7          	jalr	ra,1490(ra) # 4544 <LOGUART_PutChar>
2005ef7a:	48b2                	c.lwsp	a7,12(sp)
2005ef7c:	4722                	c.lwsp	a4,8(sp)
2005ef7e:	4692                	c.lwsp	a3,4(sp)
2005ef80:	0c05                	c.addi	s8,1
2005ef82:	b7d9                	c.j	2005ef48 <DiagVprintf+0x218>
2005ef84:	00440b93          	addi	s7,s0,4
2005ef88:	00044403          	lbu	s0,0(s0)
2005ef8c:	02089163          	bne	a7,zero,2005efae <DiagVprintf+0x27e>
2005ef90:	4785                	c.li	a5,1
2005ef92:	00e7de63          	bge	a5,a4,2005efae <DiagVprintf+0x27e>
2005ef96:	85b6                	c.mv	a1,a3
2005ef98:	fff70513          	addi	a0,a4,-1
2005ef9c:	c646                	c.swsp	a7,12(sp)
2005ef9e:	c436                	c.swsp	a3,8(sp)
2005efa0:	c23a                	c.swsp	a4,4(sp)
2005efa2:	bafff0ef          	jal	ra,2005eb50 <pad_char_control>
2005efa6:	48b2                	c.lwsp	a7,12(sp)
2005efa8:	46a2                	c.lwsp	a3,8(sp)
2005efaa:	4712                	c.lwsp	a4,4(sp)
2005efac:	94aa                	c.add	s1,a0
2005efae:	8522                	c.mv	a0,s0
2005efb0:	c646                	c.swsp	a7,12(sp)
2005efb2:	c43a                	c.swsp	a4,8(sp)
2005efb4:	c236                	c.swsp	a3,4(sp)
2005efb6:	dffa5097          	auipc	ra,0xdffa5
2005efba:	58e080e7          	jalr	ra,1422(ra) # 4544 <LOGUART_PutChar>
2005efbe:	48b2                	c.lwsp	a7,12(sp)
2005efc0:	0485                	c.addi	s1,1
2005efc2:	ec0880e3          	beq	a7,zero,2005ee82 <DiagVprintf+0x152>
2005efc6:	4722                	c.lwsp	a4,8(sp)
2005efc8:	4785                	c.li	a5,1
2005efca:	eae7dce3          	bge	a5,a4,2005ee82 <DiagVprintf+0x152>
2005efce:	4692                	c.lwsp	a3,4(sp)
2005efd0:	fff70513          	addi	a0,a4,-1
2005efd4:	85b6                	c.mv	a1,a3
2005efd6:	b771                	c.j	2005ef62 <DiagVprintf+0x232>
2005efd8:	0605                	c.addi	a2,1
2005efda:	0905                	c.addi	s2,1
2005efdc:	bbf1                	c.j	2005edb8 <DiagVprintf+0x88>
2005efde:	02500513          	addi	a0,zero,37
2005efe2:	dffa5097          	auipc	ra,0xdffa5
2005efe6:	562080e7          	jalr	ra,1378(ra) # 4544 <LOGUART_PutChar>
2005efea:	0485                	c.addi	s1,1
2005efec:	8ba2                	c.mv	s7,s0
2005efee:	bd51                	c.j	2005ee82 <DiagVprintf+0x152>
2005eff0:	0905                	c.addi	s2,1
2005eff2:	03000693          	addi	a3,zero,48
2005eff6:	4701                	c.li	a4,0
2005eff8:	00094783          	lbu	a5,0(s2)
2005effc:	d60783e3          	beq	a5,zero,2005ed62 <DiagVprintf+0x32>
2005f000:	fd078513          	addi	a0,a5,-48
2005f004:	0ff57513          	andi	a0,a0,255
2005f008:	daaae8e3          	bltu	s5,a0,2005edb8 <DiagVprintf+0x88>
2005f00c:	03670733          	mul	a4,a4,s6
2005f010:	0905                	c.addi	s2,1
2005f012:	fd070713          	addi	a4,a4,-48
2005f016:	973e                	c.add	a4,a5
2005f018:	b7c5                	c.j	2005eff8 <DiagVprintf+0x2c8>
2005f01a:	0905                	c.addi	s2,1
2005f01c:	02000693          	addi	a3,zero,32
2005f020:	bfd9                	c.j	2005eff6 <DiagVprintf+0x2c6>
2005f022:	dffa5097          	auipc	ra,0xdffa5
2005f026:	522080e7          	jalr	ra,1314(ra) # 4544 <LOGUART_PutChar>
2005f02a:	0485                	c.addi	s1,1
2005f02c:	bb05                	c.j	2005ed5c <DiagVprintf+0x2c>

2005f02e <DiagPrintf>:
2005f02e:	7139                	c.addi16sp	sp,-64
2005f030:	d22e                	c.swsp	a1,36(sp)
2005f032:	104c                	c.addi4spn	a1,sp,36
2005f034:	ce06                	c.swsp	ra,28(sp)
2005f036:	d432                	c.swsp	a2,40(sp)
2005f038:	d636                	c.swsp	a3,44(sp)
2005f03a:	d83a                	c.swsp	a4,48(sp)
2005f03c:	da3e                	c.swsp	a5,52(sp)
2005f03e:	dc42                	c.swsp	a6,56(sp)
2005f040:	de46                	c.swsp	a7,60(sp)
2005f042:	c62e                	c.swsp	a1,12(sp)
2005f044:	cedff0ef          	jal	ra,2005ed30 <DiagVprintf>
2005f048:	40f2                	c.lwsp	ra,28(sp)
2005f04a:	6121                	c.addi16sp	sp,64
2005f04c:	8082                	c.jr	ra

2005f04e <DiagPrintf_minimal>:
2005f04e:	711d                	c.addi16sp	sp,-96
2005f050:	d84a                	c.swsp	s2,48(sp)
2005f052:	04410913          	addi	s2,sp,68
2005f056:	dc22                	c.swsp	s0,56(sp)
2005f058:	da26                	c.swsp	s1,52(sp)
2005f05a:	d64e                	c.swsp	s3,44(sp)
2005f05c:	d452                	c.swsp	s4,40(sp)
2005f05e:	d256                	c.swsp	s5,36(sp)
2005f060:	de06                	c.swsp	ra,60(sp)
2005f062:	d05a                	c.swsp	s6,32(sp)
2005f064:	ce5e                	c.swsp	s7,28(sp)
2005f066:	84aa                	c.mv	s1,a0
2005f068:	c2ae                	c.swsp	a1,68(sp)
2005f06a:	c4b2                	c.swsp	a2,72(sp)
2005f06c:	c6b6                	c.swsp	a3,76(sp)
2005f06e:	c8ba                	c.swsp	a4,80(sp)
2005f070:	cabe                	c.swsp	a5,84(sp)
2005f072:	ccc2                	c.swsp	a6,88(sp)
2005f074:	cec6                	c.swsp	a7,92(sp)
2005f076:	c64a                	c.swsp	s2,12(sp)
2005f078:	4401                	c.li	s0,0
2005f07a:	03000993          	addi	s3,zero,48
2005f07e:	4a25                	c.li	s4,9
2005f080:	4aa9                	c.li	s5,10
2005f082:	0004c503          	lbu	a0,0(s1)
2005f086:	ed09                	c.bnez	a0,2005f0a0 <DiagPrintf_minimal+0x52>
2005f088:	50f2                	c.lwsp	ra,60(sp)
2005f08a:	8522                	c.mv	a0,s0
2005f08c:	5462                	c.lwsp	s0,56(sp)
2005f08e:	54d2                	c.lwsp	s1,52(sp)
2005f090:	5942                	c.lwsp	s2,48(sp)
2005f092:	59b2                	c.lwsp	s3,44(sp)
2005f094:	5a22                	c.lwsp	s4,40(sp)
2005f096:	5a92                	c.lwsp	s5,36(sp)
2005f098:	5b02                	c.lwsp	s6,32(sp)
2005f09a:	4bf2                	c.lwsp	s7,28(sp)
2005f09c:	6125                	c.addi16sp	sp,96
2005f09e:	8082                	c.jr	ra
2005f0a0:	02500793          	addi	a5,zero,37
2005f0a4:	0485                	c.addi	s1,1
2005f0a6:	14f51263          	bne	a0,a5,2005f1ea <DiagPrintf_minimal+0x19c>
2005f0aa:	4681                	c.li	a3,0
2005f0ac:	4601                	c.li	a2,0
2005f0ae:	04300513          	addi	a0,zero,67
2005f0b2:	45a1                	c.li	a1,8
2005f0b4:	02000813          	addi	a6,zero,32
2005f0b8:	0004c783          	lbu	a5,0(s1)
2005f0bc:	0ca78463          	beq	a5,a0,2005f184 <DiagPrintf_minimal+0x136>
2005f0c0:	02f56763          	bltu	a0,a5,2005f0ee <DiagPrintf_minimal+0xa0>
2005f0c4:	0f378c63          	beq	a5,s3,2005f1bc <DiagPrintf_minimal+0x16e>
2005f0c8:	00f9ea63          	bltu	s3,a5,2005f0dc <DiagPrintf_minimal+0x8e>
2005f0cc:	11078d63          	beq	a5,a6,2005f1e6 <DiagPrintf_minimal+0x198>
2005f0d0:	02500713          	addi	a4,zero,37
2005f0d4:	0ce78c63          	beq	a5,a4,2005f1ac <DiagPrintf_minimal+0x15e>
2005f0d8:	547d                	c.li	s0,-1
2005f0da:	b77d                	c.j	2005f088 <DiagPrintf_minimal+0x3a>
2005f0dc:	fcf78793          	addi	a5,a5,-49
2005f0e0:	0ff7f793          	andi	a5,a5,255
2005f0e4:	fef5eae3          	bltu	a1,a5,2005f0d8 <DiagPrintf_minimal+0x8a>
2005f0e8:	02000613          	addi	a2,zero,32
2005f0ec:	a8d9                	c.j	2005f1c2 <DiagPrintf_minimal+0x174>
2005f0ee:	07300713          	addi	a4,zero,115
2005f0f2:	04e78263          	beq	a5,a4,2005f136 <DiagPrintf_minimal+0xe8>
2005f0f6:	02f76463          	bltu	a4,a5,2005f11e <DiagPrintf_minimal+0xd0>
2005f0fa:	06300713          	addi	a4,zero,99
2005f0fe:	08e78363          	beq	a5,a4,2005f184 <DiagPrintf_minimal+0x136>
2005f102:	06400713          	addi	a4,zero,100
2005f106:	fce799e3          	bne	a5,a4,2005f0d8 <DiagPrintf_minimal+0x8a>
2005f10a:	00490b13          	addi	s6,s2,4
2005f10e:	45a9                	c.li	a1,10
2005f110:	00092503          	lw	a0,0(s2)
2005f114:	a73ff0ef          	jal	ra,2005eb86 <print_decimal_num>
2005f118:	942a                	c.add	s0,a0
2005f11a:	895a                	c.mv	s2,s6
2005f11c:	a891                	c.j	2005f170 <DiagPrintf_minimal+0x122>
2005f11e:	07500713          	addi	a4,zero,117
2005f122:	fee784e3          	beq	a5,a4,2005f10a <DiagPrintf_minimal+0xbc>
2005f126:	07800713          	addi	a4,zero,120
2005f12a:	fae797e3          	bne	a5,a4,2005f0d8 <DiagPrintf_minimal+0x8a>
2005f12e:	00490b13          	addi	s6,s2,4
2005f132:	45c1                	c.li	a1,16
2005f134:	bff1                	c.j	2005f110 <DiagPrintf_minimal+0xc2>
2005f136:	00092b03          	lw	s6,0(s2)
2005f13a:	00490b93          	addi	s7,s2,4
2005f13e:	4501                	c.li	a0,0
2005f140:	00ab07b3          	add	a5,s6,a0
2005f144:	0007c783          	lbu	a5,0(a5)
2005f148:	e795                	c.bnez	a5,2005f174 <DiagPrintf_minimal+0x126>
2005f14a:	40a68533          	sub	a0,a3,a0
2005f14e:	00a05663          	bge	zero,a0,2005f15a <DiagPrintf_minimal+0x10c>
2005f152:	85b2                	c.mv	a1,a2
2005f154:	9fdff0ef          	jal	ra,2005eb50 <pad_char_control>
2005f158:	942a                	c.add	s0,a0
2005f15a:	597d                	c.li	s2,-1
2005f15c:	000b0863          	beq	s6,zero,2005f16c <DiagPrintf_minimal+0x11e>
2005f160:	4901                	c.li	s2,0
2005f162:	012b07b3          	add	a5,s6,s2
2005f166:	0007c503          	lbu	a0,0(a5)
2005f16a:	e519                	c.bnez	a0,2005f178 <DiagPrintf_minimal+0x12a>
2005f16c:	944a                	c.add	s0,s2
2005f16e:	895e                	c.mv	s2,s7
2005f170:	0485                	c.addi	s1,1
2005f172:	bf01                	c.j	2005f082 <DiagPrintf_minimal+0x34>
2005f174:	0505                	c.addi	a0,1
2005f176:	b7e9                	c.j	2005f140 <DiagPrintf_minimal+0xf2>
2005f178:	dffa5097          	auipc	ra,0xdffa5
2005f17c:	3cc080e7          	jalr	ra,972(ra) # 4544 <LOGUART_PutChar>
2005f180:	0905                	c.addi	s2,1
2005f182:	b7c5                	c.j	2005f162 <DiagPrintf_minimal+0x114>
2005f184:	4785                	c.li	a5,1
2005f186:	00490b13          	addi	s6,s2,4
2005f18a:	00094903          	lbu	s2,0(s2)
2005f18e:	00d7d863          	bge	a5,a3,2005f19e <DiagPrintf_minimal+0x150>
2005f192:	85b2                	c.mv	a1,a2
2005f194:	fff68513          	addi	a0,a3,-1
2005f198:	9b9ff0ef          	jal	ra,2005eb50 <pad_char_control>
2005f19c:	942a                	c.add	s0,a0
2005f19e:	854a                	c.mv	a0,s2
2005f1a0:	dffa5097          	auipc	ra,0xdffa5
2005f1a4:	3a4080e7          	jalr	ra,932(ra) # 4544 <LOGUART_PutChar>
2005f1a8:	0405                	c.addi	s0,1
2005f1aa:	bf85                	c.j	2005f11a <DiagPrintf_minimal+0xcc>
2005f1ac:	02500513          	addi	a0,zero,37
2005f1b0:	dffa5097          	auipc	ra,0xdffa5
2005f1b4:	394080e7          	jalr	ra,916(ra) # 4544 <LOGUART_PutChar>
2005f1b8:	0405                	c.addi	s0,1
2005f1ba:	bf5d                	c.j	2005f170 <DiagPrintf_minimal+0x122>
2005f1bc:	0485                	c.addi	s1,1
2005f1be:	03000613          	addi	a2,zero,48
2005f1c2:	4681                	c.li	a3,0
2005f1c4:	0004c783          	lbu	a5,0(s1)
2005f1c8:	ec0780e3          	beq	a5,zero,2005f088 <DiagPrintf_minimal+0x3a>
2005f1cc:	fd078713          	addi	a4,a5,-48
2005f1d0:	0ff77713          	andi	a4,a4,255
2005f1d4:	eeea62e3          	bltu	s4,a4,2005f0b8 <DiagPrintf_minimal+0x6a>
2005f1d8:	035686b3          	mul	a3,a3,s5
2005f1dc:	0485                	c.addi	s1,1
2005f1de:	fd068693          	addi	a3,a3,-48
2005f1e2:	96be                	c.add	a3,a5
2005f1e4:	b7c5                	c.j	2005f1c4 <DiagPrintf_minimal+0x176>
2005f1e6:	0485                	c.addi	s1,1
2005f1e8:	b701                	c.j	2005f0e8 <DiagPrintf_minimal+0x9a>
2005f1ea:	dffa5097          	auipc	ra,0xdffa5
2005f1ee:	35a080e7          	jalr	ra,858(ra) # 4544 <LOGUART_PutChar>
2005f1f2:	0405                	c.addi	s0,1
2005f1f4:	b579                	c.j	2005f082 <DiagPrintf_minimal+0x34>

2005f1f6 <DiagSnPrintf>:
2005f1f6:	7159                	c.addi16sp	sp,-112
2005f1f8:	c6a2                	c.swsp	s0,76(sp)
2005f1fa:	c4a6                	c.swsp	s1,72(sp)
2005f1fc:	c2ca                	c.swsp	s2,68(sp)
2005f1fe:	c0ce                	c.swsp	s3,64(sp)
2005f200:	de52                	c.swsp	s4,60(sp)
2005f202:	dc56                	c.swsp	s5,56(sp)
2005f204:	da5a                	c.swsp	s6,52(sp)
2005f206:	d85e                	c.swsp	s7,48(sp)
2005f208:	d662                	c.swsp	s8,44(sp)
2005f20a:	d466                	c.swsp	s9,40(sp)
2005f20c:	d26a                	c.swsp	s10,36(sp)
2005f20e:	d06e                	c.swsp	s11,32(sp)
2005f210:	ceb6                	c.swsp	a3,92(sp)
2005f212:	d0ba                	c.swsp	a4,96(sp)
2005f214:	d2be                	c.swsp	a5,100(sp)
2005f216:	d4c2                	c.swsp	a6,104(sp)
2005f218:	d6c6                	c.swsp	a7,108(sp)
2005f21a:	28050463          	beq	a0,zero,2005f4a2 <DiagSnPrintf+0x2ac>
2005f21e:	08fc                	c.addi4spn	a5,sp,92
2005f220:	c43e                	c.swsp	a5,8(sp)
2005f222:	537d                	c.li	t1,-1
2005f224:	c199                	c.beqz	a1,2005f22a <DiagSnPrintf+0x34>
2005f226:	00b50333          	add	t1,a0,a1
2005f22a:	200633b7          	lui	t2,0x20063
2005f22e:	6405                	c.lui	s0,0x1
2005f230:	882a                	c.mv	a6,a0
2005f232:	05000f13          	addi	t5,zero,80
2005f236:	06400913          	addi	s2,zero,100
2005f23a:	06300993          	addi	s3,zero,99
2005f23e:	4fa9                	c.li	t6,10
2005f240:	03000293          	addi	t0,zero,48
2005f244:	02d00a13          	addi	s4,zero,45
2005f248:	30438393          	addi	t2,t2,772 # 20063304 <TaskStateString.0+0xa4>
2005f24c:	4aa5                	c.li	s5,9
2005f24e:	0ef00b13          	addi	s6,zero,239
2005f252:	eff40b93          	addi	s7,s0,-257 # eff <CPU_ClkGet+0x3e9>
2005f256:	6c3d                	c.lui	s8,0xf
2005f258:	7cc1                	c.lui	s9,0xffff0
2005f25a:	00064783          	lbu	a5,0(a2)
2005f25e:	e399                	c.bnez	a5,2005f264 <DiagSnPrintf+0x6e>
2005f260:	8742                	c.mv	a4,a6
2005f262:	a889                	c.j	2005f2b4 <DiagSnPrintf+0xbe>
2005f264:	02500713          	addi	a4,zero,37
2005f268:	00e78b63          	beq	a5,a4,2005f27e <DiagSnPrintf+0x88>
2005f26c:	00180713          	addi	a4,a6,1 # fff00001 <__ctrace_end__+0x35f00001>
2005f270:	00f80023          	sb	a5,0(a6)
2005f274:	04677063          	bgeu	a4,t1,2005f2b4 <DiagSnPrintf+0xbe>
2005f278:	0605                	c.addi	a2,1
2005f27a:	883a                	c.mv	a6,a4
2005f27c:	bff9                	c.j	2005f25a <DiagSnPrintf+0x64>
2005f27e:	4722                	c.lwsp	a4,8(sp)
2005f280:	00164e03          	lbu	t3,1(a2)
2005f284:	07300693          	addi	a3,zero,115
2005f288:	431c                	c.lw	a5,0(a4)
2005f28a:	00160593          	addi	a1,a2,1
2005f28e:	0711                	c.addi	a4,4
2005f290:	04de1463          	bne	t3,a3,2005f2d8 <DiagSnPrintf+0xe2>
2005f294:	c43a                	c.swsp	a4,8(sp)
2005f296:	410787b3          	sub	a5,a5,a6
2005f29a:	8742                	c.mv	a4,a6
2005f29c:	00f706b3          	add	a3,a4,a5
2005f2a0:	0006c683          	lbu	a3,0(a3)
2005f2a4:	e299                	c.bnez	a3,2005f2aa <DiagSnPrintf+0xb4>
2005f2a6:	862e                	c.mv	a2,a1
2005f2a8:	bfc1                	c.j	2005f278 <DiagSnPrintf+0x82>
2005f2aa:	0705                	c.addi	a4,1
2005f2ac:	fed70fa3          	sb	a3,-1(a4)
2005f2b0:	fe6766e3          	bltu	a4,t1,2005f29c <DiagSnPrintf+0xa6>
2005f2b4:	00070023          	sb	zero,0(a4)
2005f2b8:	40a70533          	sub	a0,a4,a0
2005f2bc:	4436                	c.lwsp	s0,76(sp)
2005f2be:	44a6                	c.lwsp	s1,72(sp)
2005f2c0:	4916                	c.lwsp	s2,68(sp)
2005f2c2:	4986                	c.lwsp	s3,64(sp)
2005f2c4:	5a72                	c.lwsp	s4,60(sp)
2005f2c6:	5ae2                	c.lwsp	s5,56(sp)
2005f2c8:	5b52                	c.lwsp	s6,52(sp)
2005f2ca:	5bc2                	c.lwsp	s7,48(sp)
2005f2cc:	5c32                	c.lwsp	s8,44(sp)
2005f2ce:	5ca2                	c.lwsp	s9,40(sp)
2005f2d0:	5d12                	c.lwsp	s10,36(sp)
2005f2d2:	5d82                	c.lwsp	s11,32(sp)
2005f2d4:	6165                	c.addi16sp	sp,112
2005f2d6:	8082                	c.jr	ra
2005f2d8:	c43a                	c.swsp	a4,8(sp)
2005f2da:	0dfe7693          	andi	a3,t3,223
2005f2de:	473d                	c.li	a4,15
2005f2e0:	0de69563          	bne	a3,t5,2005f3aa <DiagSnPrintf+0x1b4>
2005f2e4:	0cf75a63          	bge	a4,a5,2005f3b8 <DiagSnPrintf+0x1c2>
2005f2e8:	46a1                	c.li	a3,8
2005f2ea:	03000893          	addi	a7,zero,48
2005f2ee:	ff078e93          	addi	t4,a5,-16
2005f2f2:	4711                	c.li	a4,4
2005f2f4:	05db7463          	bgeu	s6,t4,2005f33c <DiagSnPrintf+0x146>
2005f2f8:	f0078e93          	addi	t4,a5,-256
2005f2fc:	4721                	c.li	a4,8
2005f2fe:	03dbff63          	bgeu	s7,t4,2005f33c <DiagSnPrintf+0x146>
2005f302:	40878eb3          	sub	t4,a5,s0
2005f306:	4731                	c.li	a4,12
2005f308:	038eea63          	bltu	t4,s8,2005f33c <DiagSnPrintf+0x146>
2005f30c:	01978eb3          	add	t4,a5,s9
2005f310:	000f04b7          	lui	s1,0xf0
2005f314:	4741                	c.li	a4,16
2005f316:	029ee363          	bltu	t4,s1,2005f33c <DiagSnPrintf+0x146>
2005f31a:	fff00eb7          	lui	t4,0xfff00
2005f31e:	9ebe                	c.add	t4,a5
2005f320:	00f004b7          	lui	s1,0xf00
2005f324:	4751                	c.li	a4,20
2005f326:	009eeb63          	bltu	t4,s1,2005f33c <DiagSnPrintf+0x146>
2005f32a:	ff000eb7          	lui	t4,0xff000
2005f32e:	9ebe                	c.add	t4,a5
2005f330:	0f0004b7          	lui	s1,0xf000
2005f334:	4761                	c.li	a4,24
2005f336:	009ee363          	bltu	t4,s1,2005f33c <DiagSnPrintf+0x146>
2005f33a:	4771                	c.li	a4,28
2005f33c:	005e1663          	bne	t3,t0,2005f348 <DiagSnPrintf+0x152>
2005f340:	00260593          	addi	a1,a2,2
2005f344:	03000893          	addi	a7,zero,48
2005f348:	0005c603          	lbu	a2,0(a1)
2005f34c:	44a1                	c.li	s1,8
2005f34e:	fcf60e13          	addi	t3,a2,-49
2005f352:	0ffe7e13          	andi	t3,t3,255
2005f356:	03c4e163          	bltu	s1,t3,2005f378 <DiagSnPrintf+0x182>
2005f35a:	4681                	c.li	a3,0
2005f35c:	03f686b3          	mul	a3,a3,t6
2005f360:	0585                	c.addi	a1,1
2005f362:	96b2                	c.add	a3,a2
2005f364:	0005c603          	lbu	a2,0(a1)
2005f368:	fd068693          	addi	a3,a3,-48
2005f36c:	fd060e13          	addi	t3,a2,-48
2005f370:	0ffe7e13          	andi	t3,t3,255
2005f374:	ffcaf4e3          	bgeu	s5,t3,2005f35c <DiagSnPrintf+0x166>
2005f378:	0005c603          	lbu	a2,0(a1)
2005f37c:	0d767e13          	andi	t3,a2,215
2005f380:	0bee1463          	bne	t3,t5,2005f428 <DiagSnPrintf+0x232>
2005f384:	02067493          	andi	s1,a2,32
2005f388:	0df67613          	andi	a2,a2,223
2005f38c:	8ebe                	c.mv	t4,a5
2005f38e:	00c10e13          	addi	t3,sp,12
2005f392:	01e61a63          	bne	a2,t5,2005f3a6 <DiagSnPrintf+0x1b0>
2005f396:	0584e613          	ori	a2,s1,88
2005f39a:	00510623          	sb	t0,12(sp)
2005f39e:	00c106a3          	sb	a2,13(sp)
2005f3a2:	00e10e13          	addi	t3,sp,14
2005f3a6:	4601                	c.li	a2,0
2005f3a8:	a831                	c.j	2005f3c4 <DiagSnPrintf+0x1ce>
2005f3aa:	4681                	c.li	a3,0
2005f3ac:	02000893          	addi	a7,zero,32
2005f3b0:	f2f74fe3          	blt	a4,a5,2005f2ee <DiagSnPrintf+0xf8>
2005f3b4:	4701                	c.li	a4,0
2005f3b6:	b759                	c.j	2005f33c <DiagSnPrintf+0x146>
2005f3b8:	4701                	c.li	a4,0
2005f3ba:	46a1                	c.li	a3,8
2005f3bc:	b761                	c.j	2005f344 <DiagSnPrintf+0x14e>
2005f3be:	0605                	c.addi	a2,1
2005f3c0:	004ede93          	srli	t4,t4,0x4
2005f3c4:	fe0e9de3          	bne	t4,zero,2005f3be <DiagSnPrintf+0x1c8>
2005f3c8:	8ef2                	c.mv	t4,t3
2005f3ca:	00de0db3          	add	s11,t3,a3
2005f3ce:	41dd8d33          	sub	s10,s11,t4
2005f3d2:	05a64763          	blt	a2,s10,2005f420 <DiagSnPrintf+0x22a>
2005f3d6:	4881                	c.li	a7,0
2005f3d8:	00c6c463          	blt	a3,a2,2005f3e0 <DiagSnPrintf+0x1ea>
2005f3dc:	40c688b3          	sub	a7,a3,a2
2005f3e0:	9e46                	c.add	t3,a7
2005f3e2:	88f2                	c.mv	a7,t3
2005f3e4:	863a                	c.mv	a2,a4
2005f3e6:	40c7d6b3          	sra	a3,a5,a2
2005f3ea:	8abd                	c.andi	a3,15
2005f3ec:	969e                	c.add	a3,t2
2005f3ee:	0006c683          	lbu	a3,0(a3)
2005f3f2:	1671                	c.addi	a2,-4
2005f3f4:	0885                	c.addi	a7,1
2005f3f6:	8ec5                	c.or	a3,s1
2005f3f8:	fed88fa3          	sb	a3,-1(a7)
2005f3fc:	fe0655e3          	bge	a2,zero,2005f3e6 <DiagSnPrintf+0x1f0>
2005f400:	8309                	c.srli	a4,0x2
2005f402:	0705                	c.addi	a4,1
2005f404:	9e3a                	c.add	t3,a4
2005f406:	8742                	c.mv	a4,a6
2005f408:	007c                	c.addi4spn	a5,sp,12
2005f40a:	e9c7fee3          	bgeu	a5,t3,2005f2a6 <DiagSnPrintf+0xb0>
2005f40e:	0007c683          	lbu	a3,0(a5)
2005f412:	0705                	c.addi	a4,1
2005f414:	fed70fa3          	sb	a3,-1(a4)
2005f418:	e8677ee3          	bgeu	a4,t1,2005f2b4 <DiagSnPrintf+0xbe>
2005f41c:	0785                	c.addi	a5,1
2005f41e:	b7f5                	c.j	2005f40a <DiagSnPrintf+0x214>
2005f420:	011e8023          	sb	a7,0(t4) # ff000000 <__ctrace_end__+0x35000000>
2005f424:	0e85                	c.addi	t4,1
2005f426:	b765                	c.j	2005f3ce <DiagSnPrintf+0x1d8>
2005f428:	07261363          	bne	a2,s2,2005f48e <DiagSnPrintf+0x298>
2005f42c:	0407de63          	bge	a5,zero,2005f488 <DiagSnPrintf+0x292>
2005f430:	01410623          	sb	s4,12(sp)
2005f434:	40f007b3          	sub	a5,zero,a5
2005f438:	4885                	c.li	a7,1
2005f43a:	00d10693          	addi	a3,sp,13
2005f43e:	8e36                	c.mv	t3,a3
2005f440:	03f7e633          	rem	a2,a5,t6
2005f444:	0e05                	c.addi	t3,1
2005f446:	03f7c7b3          	div	a5,a5,t6
2005f44a:	03060613          	addi	a2,a2,48
2005f44e:	fece0fa3          	sb	a2,-1(t3)
2005f452:	011e0633          	add	a2,t3,a7
2005f456:	8e15                	c.sub	a2,a3
2005f458:	f7e5                	c.bnez	a5,2005f440 <DiagSnPrintf+0x24a>
2005f45a:	00c04563          	blt	zero,a2,2005f464 <DiagSnPrintf+0x26e>
2005f45e:	005e0023          	sb	t0,0(t3)
2005f462:	0e05                	c.addi	t3,1
2005f464:	1771                	c.addi	a4,-4
2005f466:	167d                	c.addi	a2,-1
2005f468:	fe0759e3          	bge	a4,zero,2005f45a <DiagSnPrintf+0x264>
2005f46c:	87f2                	c.mv	a5,t3
2005f46e:	17fd                	c.addi	a5,-1
2005f470:	f8f6fbe3          	bgeu	a3,a5,2005f406 <DiagSnPrintf+0x210>
2005f474:	0006c603          	lbu	a2,0(a3)
2005f478:	0007c703          	lbu	a4,0(a5)
2005f47c:	0685                	c.addi	a3,1
2005f47e:	00c78023          	sb	a2,0(a5)
2005f482:	fee68fa3          	sb	a4,-1(a3)
2005f486:	b7e5                	c.j	2005f46e <DiagSnPrintf+0x278>
2005f488:	4881                	c.li	a7,0
2005f48a:	0074                	c.addi4spn	a3,sp,12
2005f48c:	bf4d                	c.j	2005f43e <DiagSnPrintf+0x248>
2005f48e:	01361763          	bne	a2,s3,2005f49c <DiagSnPrintf+0x2a6>
2005f492:	00f10623          	sb	a5,12(sp)
2005f496:	00d10e13          	addi	t3,sp,13
2005f49a:	b7b5                	c.j	2005f406 <DiagSnPrintf+0x210>
2005f49c:	00c10623          	sb	a2,12(sp)
2005f4a0:	bfdd                	c.j	2005f496 <DiagSnPrintf+0x2a0>
2005f4a2:	4501                	c.li	a0,0
2005f4a4:	bd21                	c.j	2005f2bc <DiagSnPrintf+0xc6>

2005f4a6 <TRNG_get_random_bytes>:
2005f4a6:	7179                	c.addi16sp	sp,-48
2005f4a8:	d422                	c.swsp	s0,40(sp)
2005f4aa:	d226                	c.swsp	s1,36(sp)
2005f4ac:	d606                	c.swsp	ra,44(sp)
2005f4ae:	d04a                	c.swsp	s2,32(sp)
2005f4b0:	ce4e                	c.swsp	s3,28(sp)
2005f4b2:	4100c7b7          	lui	a5,0x4100c
2005f4b6:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005f4ba:	84aa                	c.mv	s1,a0
2005f4bc:	842e                	c.mv	s0,a1
2005f4be:	8399                	c.srli	a5,0x6
2005f4c0:	8b8d                	c.andi	a5,3
2005f4c2:	ef9d                	c.bnez	a5,2005f500 <TRNG_get_random_bytes+0x5a>
2005f4c4:	00357793          	andi	a5,a0,3
2005f4c8:	cf85                	c.beqz	a5,2005f500 <TRNG_get_random_bytes+0x5a>
2005f4ca:	852e                	c.mv	a0,a1
2005f4cc:	a74fc0ef          	jal	ra,2005b740 <rtos_mem_malloc>
2005f4d0:	85a2                	c.mv	a1,s0
2005f4d2:	892a                	c.mv	s2,a0
2005f4d4:	dffaa097          	auipc	ra,0xdffaa
2005f4d8:	896080e7          	jalr	ra,-1898(ra) # 8d6a <RandBytes_Get>
2005f4dc:	8622                	c.mv	a2,s0
2005f4de:	85ca                	c.mv	a1,s2
2005f4e0:	8526                	c.mv	a0,s1
2005f4e2:	dffaa097          	auipc	ra,0xdffaa
2005f4e6:	5ee080e7          	jalr	ra,1518(ra) # 9ad0 <__wrap_memcpy>
2005f4ea:	854a                	c.mv	a0,s2
2005f4ec:	a7efc0ef          	jal	ra,2005b76a <rtos_mem_free>
2005f4f0:	50b2                	c.lwsp	ra,44(sp)
2005f4f2:	5422                	c.lwsp	s0,40(sp)
2005f4f4:	5492                	c.lwsp	s1,36(sp)
2005f4f6:	5902                	c.lwsp	s2,32(sp)
2005f4f8:	49f2                	c.lwsp	s3,28(sp)
2005f4fa:	4501                	c.li	a0,0
2005f4fc:	6145                	c.addi16sp	sp,48
2005f4fe:	8082                	c.jr	ra
2005f500:	00245913          	srli	s2,s0,0x2
2005f504:	4981                	c.li	s3,0
2005f506:	0329c463          	blt	s3,s2,2005f52e <TRNG_get_random_bytes+0x88>
2005f50a:	5671                	c.li	a2,-4
2005f50c:	02c90633          	mul	a2,s2,a2
2005f510:	9432                	c.add	s0,a2
2005f512:	dc79                	c.beqz	s0,2005f4f0 <TRNG_get_random_bytes+0x4a>
2005f514:	f6aff0ef          	jal	ra,2005ec7e <_rand>
2005f518:	c62a                	c.swsp	a0,12(sp)
2005f51a:	00291513          	slli	a0,s2,0x2
2005f51e:	8622                	c.mv	a2,s0
2005f520:	006c                	c.addi4spn	a1,sp,12
2005f522:	9526                	c.add	a0,s1
2005f524:	dffaa097          	auipc	ra,0xdffaa
2005f528:	0ee080e7          	jalr	ra,238(ra) # 9612 <_memcpy>
2005f52c:	b7d1                	c.j	2005f4f0 <TRNG_get_random_bytes+0x4a>
2005f52e:	f50ff0ef          	jal	ra,2005ec7e <_rand>
2005f532:	00299793          	slli	a5,s3,0x2
2005f536:	97a6                	c.add	a5,s1
2005f538:	c388                	c.sw	a0,0(a5)
2005f53a:	0985                	c.addi	s3,1
2005f53c:	b7e9                	c.j	2005f506 <TRNG_get_random_bytes+0x60>

2005f53e <io_assert_failed>:
2005f53e:	862e                	c.mv	a2,a1
2005f540:	85aa                	c.mv	a1,a0
2005f542:	20063537          	lui	a0,0x20063
2005f546:	1141                	c.addi	sp,-16
2005f548:	31850513          	addi	a0,a0,792 # 20063318 <TaskStateString.0+0xb8>
2005f54c:	c606                	c.swsp	ra,12(sp)
2005f54e:	b01ff0ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005f552:	a001                	c.j	2005f552 <io_assert_failed+0x14>

2005f554 <irq_set_priority>:
2005f554:	471d                	c.li	a4,7
2005f556:	02b77563          	bgeu	a4,a1,2005f580 <irq_set_priority+0x2c>
2005f55a:	862e                	c.mv	a2,a1
2005f55c:	85aa                	c.mv	a1,a0
2005f55e:	20063537          	lui	a0,0x20063
2005f562:	1141                	c.addi	sp,-16
2005f564:	469d                	c.li	a3,7
2005f566:	3f450513          	addi	a0,a0,1012 # 200633f4 <TaskStateString.0+0x194>
2005f56a:	c606                	c.swsp	ra,12(sp)
2005f56c:	ae3ff0ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2005f570:	20063537          	lui	a0,0x20063
2005f574:	44800593          	addi	a1,zero,1096
2005f578:	34c50513          	addi	a0,a0,844 # 2006334c <TaskStateString.0+0xec>
2005f57c:	fc3ff0ef          	jal	ra,2005f53e <io_assert_failed>
2005f580:	73e00067          	jalr	zero,1854(zero) # 0 <Reset_Handler>

2005f584 <irq_register>:
2005f584:	20000737          	lui	a4,0x20000
2005f588:	1141                	c.addi	sp,-16
2005f58a:	00259893          	slli	a7,a1,0x2
2005f58e:	882a                	c.mv	a6,a0
2005f590:	00070713          	addi	a4,a4,0 # 20000000 <UserIrqFunTable>
2005f594:	200007b7          	lui	a5,0x20000
2005f598:	c606                	c.swsp	ra,12(sp)
2005f59a:	9746                	c.add	a4,a7
2005f59c:	00186813          	ori	a6,a6,1
2005f5a0:	1c078793          	addi	a5,a5,448 # 200001c0 <UserIrqDataTable>
2005f5a4:	01072023          	sw	a6,0(a4)
2005f5a8:	97c6                	c.add	a5,a7
2005f5aa:	852e                	c.mv	a0,a1
2005f5ac:	c390                	c.sw	a2,0(a5)
2005f5ae:	85b6                	c.mv	a1,a3
2005f5b0:	fa5ff0ef          	jal	ra,2005f554 <irq_set_priority>
2005f5b4:	40b2                	c.lwsp	ra,12(sp)
2005f5b6:	4505                	c.li	a0,1
2005f5b8:	0141                	c.addi	sp,16
2005f5ba:	8082                	c.jr	ra

2005f5bc <unwind_frame_rv32i>:
2005f5bc:	715d                	c.addi16sp	sp,-80
2005f5be:	c686                	c.swsp	ra,76(sp)
2005f5c0:	c4a2                	c.swsp	s0,72(sp)
2005f5c2:	0880                	c.addi4spn	s0,sp,80
2005f5c4:	faa42e23          	sw	a0,-68(s0)
2005f5c8:	fab42c23          	sw	a1,-72(s0)
2005f5cc:	fe042623          	sw	zero,-20(s0)
2005f5d0:	fe042423          	sw	zero,-24(s0)
2005f5d4:	fbc42783          	lw	a5,-68(s0)
2005f5d8:	43dc                	c.lw	a5,4(a5)
2005f5da:	c789                	c.beqz	a5,2005f5e4 <unwind_frame_rv32i+0x28>
2005f5dc:	fbc42783          	lw	a5,-68(s0)
2005f5e0:	439c                	c.lw	a5,0(a5)
2005f5e2:	e399                	c.bnez	a5,2005f5e8 <unwind_frame_rv32i+0x2c>
2005f5e4:	57fd                	c.li	a5,-1
2005f5e6:	a44d                	c.j	2005f888 <unwind_frame_rv32i+0x2cc>
2005f5e8:	fbc42783          	lw	a5,-68(s0)
2005f5ec:	43dc                	c.lw	a5,4(a5)
2005f5ee:	fef42223          	sw	a5,-28(s0)
2005f5f2:	a435                	c.j	2005f81e <unwind_frame_rv32i+0x262>
2005f5f4:	fe442783          	lw	a5,-28(s0)
2005f5f8:	8b89                	c.andi	a5,2
2005f5fa:	c385                	c.beqz	a5,2005f61a <unwind_frame_rv32i+0x5e>
2005f5fc:	fe442783          	lw	a5,-28(s0)
2005f600:	0007d783          	lhu	a5,0(a5)
2005f604:	873e                	c.mv	a4,a5
2005f606:	fe442783          	lw	a5,-28(s0)
2005f60a:	0789                	c.addi	a5,2
2005f60c:	0007d783          	lhu	a5,0(a5)
2005f610:	07c2                	c.slli	a5,0x10
2005f612:	8fd9                	c.or	a5,a4
2005f614:	fcf42c23          	sw	a5,-40(s0)
2005f618:	a031                	c.j	2005f624 <unwind_frame_rv32i+0x68>
2005f61a:	fe442783          	lw	a5,-28(s0)
2005f61e:	439c                	c.lw	a5,0(a5)
2005f620:	fcf42c23          	sw	a5,-40(s0)
2005f624:	fe442783          	lw	a5,-28(s0)
2005f628:	0007d783          	lhu	a5,0(a5)
2005f62c:	fcf41a23          	sh	a5,-44(s0)
2005f630:	fe442783          	lw	a5,-28(s0)
2005f634:	0789                	c.addi	a5,2
2005f636:	0007d783          	lhu	a5,0(a5)
2005f63a:	fcf41823          	sh	a5,-48(s0)
2005f63e:	fd845783          	lhu	a5,-40(s0)
2005f642:	07f7f793          	andi	a5,a5,127
2005f646:	01079713          	slli	a4,a5,0x10
2005f64a:	8341                	c.srli	a4,0x10
2005f64c:	47cd                	c.li	a5,19
2005f64e:	0af71c63          	bne	a4,a5,2005f706 <unwind_frame_rv32i+0x14a>
2005f652:	fd845703          	lhu	a4,-40(s0)
2005f656:	679d                	c.lui	a5,0x7
2005f658:	8ff9                	c.and	a5,a4
2005f65a:	07c2                	c.slli	a5,0x10
2005f65c:	83c1                	c.srli	a5,0x10
2005f65e:	e7c5                	c.bnez	a5,2005f706 <unwind_frame_rv32i+0x14a>
2005f660:	fd842703          	lw	a4,-40(s0)
2005f664:	6785                	c.lui	a5,0x1
2005f666:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005f66a:	8f7d                	c.and	a4,a5
2005f66c:	10000793          	addi	a5,zero,256
2005f670:	18f71f63          	bne	a4,a5,2005f80e <unwind_frame_rv32i+0x252>
2005f674:	fd842703          	lw	a4,-40(s0)
2005f678:	000f87b7          	lui	a5,0xf8
2005f67c:	8f7d                	c.and	a4,a5
2005f67e:	67c1                	c.lui	a5,0x10
2005f680:	18f71763          	bne	a4,a5,2005f80e <unwind_frame_rv32i+0x252>
2005f684:	fd842783          	lw	a5,-40(s0)
2005f688:	83d1                	c.srli	a5,0x14
2005f68a:	07c2                	c.slli	a5,0x10
2005f68c:	83c1                	c.srli	a5,0x10
2005f68e:	0792                	c.slli	a5,0x4
2005f690:	07c2                	c.slli	a5,0x10
2005f692:	87c1                	c.srai	a5,0x10
2005f694:	8791                	c.srai	a5,0x4
2005f696:	07c2                	c.slli	a5,0x10
2005f698:	87c1                	c.srai	a5,0x10
2005f69a:	873e                	c.mv	a4,a5
2005f69c:	6785                	c.lui	a5,0x1
2005f69e:	17fd                	c.addi	a5,-1
2005f6a0:	00f776b3          	and	a3,a4,a5
2005f6a4:	fcc45703          	lhu	a4,-52(s0)
2005f6a8:	77fd                	c.lui	a5,0xfffff
2005f6aa:	8ff9                	c.and	a5,a4
2005f6ac:	873e                	c.mv	a4,a5
2005f6ae:	87b6                	c.mv	a5,a3
2005f6b0:	8fd9                	c.or	a5,a4
2005f6b2:	fcf41623          	sh	a5,-52(s0)
2005f6b6:	fcc42783          	lw	a5,-52(s0)
2005f6ba:	0792                	c.slli	a5,0x4
2005f6bc:	07c2                	c.slli	a5,0x10
2005f6be:	87c1                	c.srai	a5,0x10
2005f6c0:	8791                	c.srai	a5,0x4
2005f6c2:	07c2                	c.slli	a5,0x10
2005f6c4:	87c1                	c.srai	a5,0x10
2005f6c6:	fef42023          	sw	a5,-32(s0)
2005f6ca:	fe042783          	lw	a5,-32(s0)
2005f6ce:	02f05363          	bge	zero,a5,2005f6f4 <unwind_frame_rv32i+0x138>
2005f6d2:	fe842783          	lw	a5,-24(s0)
2005f6d6:	86be                	c.mv	a3,a5
2005f6d8:	10d00613          	addi	a2,zero,269
2005f6dc:	200637b7          	lui	a5,0x20063
2005f6e0:	61878593          	addi	a1,a5,1560 # 20063618 <__FUNCTION__.3>
2005f6e4:	200637b7          	lui	a5,0x20063
2005f6e8:	41878513          	addi	a0,a5,1048 # 20063418 <TaskStateString.0+0x1b8>
2005f6ec:	943ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005f6f0:	57fd                	c.li	a5,-1
2005f6f2:	aa59                	c.j	2005f888 <unwind_frame_rv32i+0x2cc>
2005f6f4:	fe042783          	lw	a5,-32(s0)
2005f6f8:	0007d463          	bge	a5,zero,2005f700 <unwind_frame_rv32i+0x144>
2005f6fc:	40f007b3          	sub	a5,zero,a5
2005f700:	fef42423          	sw	a5,-24(s0)
2005f704:	a229                	c.j	2005f80e <unwind_frame_rv32i+0x252>
2005f706:	fd845783          	lhu	a5,-40(s0)
2005f70a:	07f7f793          	andi	a5,a5,127
2005f70e:	01079713          	slli	a4,a5,0x10
2005f712:	8341                	c.srli	a4,0x10
2005f714:	02300793          	addi	a5,zero,35
2005f718:	0cf71463          	bne	a4,a5,2005f7e0 <unwind_frame_rv32i+0x224>
2005f71c:	fd845703          	lhu	a4,-40(s0)
2005f720:	679d                	c.lui	a5,0x7
2005f722:	8ff9                	c.and	a5,a4
2005f724:	01079713          	slli	a4,a5,0x10
2005f728:	8341                	c.srli	a4,0x10
2005f72a:	6789                	c.lui	a5,0x2
2005f72c:	0af71a63          	bne	a4,a5,2005f7e0 <unwind_frame_rv32i+0x224>
2005f730:	fd842703          	lw	a4,-40(s0)
2005f734:	000f87b7          	lui	a5,0xf8
2005f738:	8f7d                	c.and	a4,a5
2005f73a:	67c1                	c.lui	a5,0x10
2005f73c:	0cf71b63          	bne	a4,a5,2005f812 <unwind_frame_rv32i+0x256>
2005f740:	fd842703          	lw	a4,-40(s0)
2005f744:	01f007b7          	lui	a5,0x1f00
2005f748:	8f7d                	c.and	a4,a5
2005f74a:	001007b7          	lui	a5,0x100
2005f74e:	0cf71263          	bne	a4,a5,2005f812 <unwind_frame_rv32i+0x256>
2005f752:	fd842783          	lw	a5,-40(s0)
2005f756:	83e5                	c.srli	a5,0x19
2005f758:	0ff7f793          	andi	a5,a5,255
2005f75c:	0796                	c.slli	a5,0x5
2005f75e:	01079713          	slli	a4,a5,0x10
2005f762:	8741                	c.srai	a4,0x10
2005f764:	fd842783          	lw	a5,-40(s0)
2005f768:	839d                	c.srli	a5,0x7
2005f76a:	8bfd                	c.andi	a5,31
2005f76c:	0ff7f793          	andi	a5,a5,255
2005f770:	07c2                	c.slli	a5,0x10
2005f772:	87c1                	c.srai	a5,0x10
2005f774:	8fd9                	c.or	a5,a4
2005f776:	07c2                	c.slli	a5,0x10
2005f778:	87c1                	c.srai	a5,0x10
2005f77a:	0792                	c.slli	a5,0x4
2005f77c:	07c2                	c.slli	a5,0x10
2005f77e:	87c1                	c.srai	a5,0x10
2005f780:	8791                	c.srai	a5,0x4
2005f782:	07c2                	c.slli	a5,0x10
2005f784:	87c1                	c.srai	a5,0x10
2005f786:	873e                	c.mv	a4,a5
2005f788:	6785                	c.lui	a5,0x1
2005f78a:	17fd                	c.addi	a5,-1
2005f78c:	00f776b3          	and	a3,a4,a5
2005f790:	fc845703          	lhu	a4,-56(s0)
2005f794:	77fd                	c.lui	a5,0xfffff
2005f796:	8ff9                	c.and	a5,a4
2005f798:	873e                	c.mv	a4,a5
2005f79a:	87b6                	c.mv	a5,a3
2005f79c:	8fd9                	c.or	a5,a4
2005f79e:	fcf41423          	sh	a5,-56(s0)
2005f7a2:	fc842783          	lw	a5,-56(s0)
2005f7a6:	0792                	c.slli	a5,0x4
2005f7a8:	07c2                	c.slli	a5,0x10
2005f7aa:	87c1                	c.srai	a5,0x10
2005f7ac:	8791                	c.srai	a5,0x4
2005f7ae:	07c2                	c.slli	a5,0x10
2005f7b0:	87c1                	c.srai	a5,0x10
2005f7b2:	fef42623          	sw	a5,-20(s0)
2005f7b6:	fec42783          	lw	a5,-20(s0)
2005f7ba:	0407dc63          	bge	a5,zero,2005f812 <unwind_frame_rv32i+0x256>
2005f7be:	fec42783          	lw	a5,-20(s0)
2005f7c2:	86be                	c.mv	a3,a5
2005f7c4:	11d00613          	addi	a2,zero,285
2005f7c8:	200637b7          	lui	a5,0x20063
2005f7cc:	61878593          	addi	a1,a5,1560 # 20063618 <__FUNCTION__.3>
2005f7d0:	200637b7          	lui	a5,0x20063
2005f7d4:	45478513          	addi	a0,a5,1108 # 20063454 <TaskStateString.0+0x1f4>
2005f7d8:	857ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005f7dc:	57fd                	c.li	a5,-1
2005f7de:	a06d                	c.j	2005f888 <unwind_frame_rv32i+0x2cc>
2005f7e0:	fd842703          	lw	a4,-40(s0)
2005f7e4:	67a1                	c.lui	a5,0x8
2005f7e6:	06778793          	addi	a5,a5,103 # 8067 <irq_unregister+0x41>
2005f7ea:	06f70063          	beq	a4,a5,2005f84a <unwind_frame_rv32i+0x28e>
2005f7ee:	fd445703          	lhu	a4,-44(s0)
2005f7f2:	67a1                	c.lui	a5,0x8
2005f7f4:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005f7f8:	00f70963          	beq	a4,a5,2005f80a <unwind_frame_rv32i+0x24e>
2005f7fc:	fd045703          	lhu	a4,-48(s0)
2005f800:	67a1                	c.lui	a5,0x8
2005f802:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005f806:	00f71763          	bne	a4,a5,2005f814 <unwind_frame_rv32i+0x258>
2005f80a:	57fd                	c.li	a5,-1
2005f80c:	a8b5                	c.j	2005f888 <unwind_frame_rv32i+0x2cc>
2005f80e:	0001                	c.addi	zero,0
2005f810:	a011                	c.j	2005f814 <unwind_frame_rv32i+0x258>
2005f812:	0001                	c.addi	zero,0
2005f814:	fe442783          	lw	a5,-28(s0)
2005f818:	17f1                	c.addi	a5,-4
2005f81a:	fef42223          	sw	a5,-28(s0)
2005f81e:	fb842783          	lw	a5,-72(s0)
2005f822:	078a                	c.slli	a5,0x2
2005f824:	fe442703          	lw	a4,-28(s0)
2005f828:	973e                	c.add	a4,a5
2005f82a:	fbc42783          	lw	a5,-68(s0)
2005f82e:	43dc                	c.lw	a5,4(a5)
2005f830:	00e7fe63          	bgeu	a5,a4,2005f84c <unwind_frame_rv32i+0x290>
2005f834:	fec42783          	lw	a5,-20(s0)
2005f838:	c781                	c.beqz	a5,2005f840 <unwind_frame_rv32i+0x284>
2005f83a:	fe842783          	lw	a5,-24(s0)
2005f83e:	e799                	c.bnez	a5,2005f84c <unwind_frame_rv32i+0x290>
2005f840:	fe442783          	lw	a5,-28(s0)
2005f844:	da0798e3          	bne	a5,zero,2005f5f4 <unwind_frame_rv32i+0x38>
2005f848:	a011                	c.j	2005f84c <unwind_frame_rv32i+0x290>
2005f84a:	0001                	c.addi	zero,0
2005f84c:	fec42783          	lw	a5,-20(s0)
2005f850:	cb91                	c.beqz	a5,2005f864 <unwind_frame_rv32i+0x2a8>
2005f852:	fbc42783          	lw	a5,-68(s0)
2005f856:	4398                	c.lw	a4,0(a5)
2005f858:	fec42783          	lw	a5,-20(s0)
2005f85c:	97ba                	c.add	a5,a4
2005f85e:	439c                	c.lw	a5,0(a5)
2005f860:	873e                	c.mv	a4,a5
2005f862:	a011                	c.j	2005f866 <unwind_frame_rv32i+0x2aa>
2005f864:	4701                	c.li	a4,0
2005f866:	fbc42783          	lw	a5,-68(s0)
2005f86a:	c798                	c.sw	a4,8(a5)
2005f86c:	fe842703          	lw	a4,-24(s0)
2005f870:	fbc42783          	lw	a5,-68(s0)
2005f874:	c7d8                	c.sw	a4,12(a5)
2005f876:	fec42783          	lw	a5,-20(s0)
2005f87a:	c781                	c.beqz	a5,2005f882 <unwind_frame_rv32i+0x2c6>
2005f87c:	fe842783          	lw	a5,-24(s0)
2005f880:	e399                	c.bnez	a5,2005f886 <unwind_frame_rv32i+0x2ca>
2005f882:	57fd                	c.li	a5,-1
2005f884:	a011                	c.j	2005f888 <unwind_frame_rv32i+0x2cc>
2005f886:	4781                	c.li	a5,0
2005f888:	853e                	c.mv	a0,a5
2005f88a:	40b6                	c.lwsp	ra,76(sp)
2005f88c:	4426                	c.lwsp	s0,72(sp)
2005f88e:	6161                	c.addi16sp	sp,80
2005f890:	8082                	c.jr	ra

2005f892 <unwind_frame_rv32c>:
2005f892:	715d                	c.addi16sp	sp,-80
2005f894:	c686                	c.swsp	ra,76(sp)
2005f896:	c4a2                	c.swsp	s0,72(sp)
2005f898:	0880                	c.addi4spn	s0,sp,80
2005f89a:	faa42e23          	sw	a0,-68(s0)
2005f89e:	fab42c23          	sw	a1,-72(s0)
2005f8a2:	fe042623          	sw	zero,-20(s0)
2005f8a6:	fe042423          	sw	zero,-24(s0)
2005f8aa:	fbc42783          	lw	a5,-68(s0)
2005f8ae:	43dc                	c.lw	a5,4(a5)
2005f8b0:	cb91                	c.beqz	a5,2005f8c4 <unwind_frame_rv32c+0x32>
2005f8b2:	fbc42783          	lw	a5,-68(s0)
2005f8b6:	439c                	c.lw	a5,0(a5)
2005f8b8:	c791                	c.beqz	a5,2005f8c4 <unwind_frame_rv32c+0x32>
2005f8ba:	fbc42783          	lw	a5,-68(s0)
2005f8be:	43dc                	c.lw	a5,4(a5)
2005f8c0:	8b85                	c.andi	a5,1
2005f8c2:	c399                	c.beqz	a5,2005f8c8 <unwind_frame_rv32c+0x36>
2005f8c4:	57fd                	c.li	a5,-1
2005f8c6:	ae3d                	c.j	2005fc04 <unwind_frame_rv32c+0x372>
2005f8c8:	fbc42783          	lw	a5,-68(s0)
2005f8cc:	43dc                	c.lw	a5,4(a5)
2005f8ce:	fef42223          	sw	a5,-28(s0)
2005f8d2:	a4e1                	c.j	2005fb9a <unwind_frame_rv32c+0x308>
2005f8d4:	fe442783          	lw	a5,-28(s0)
2005f8d8:	0007d783          	lhu	a5,0(a5)
2005f8dc:	fcf41c23          	sh	a5,-40(s0)
2005f8e0:	fd845703          	lhu	a4,-40(s0)
2005f8e4:	77f9                	c.lui	a5,0xffffe
2005f8e6:	8ff9                	c.and	a5,a4
2005f8e8:	07c2                	c.slli	a5,0x10
2005f8ea:	83c1                	c.srli	a5,0x10
2005f8ec:	e7e9                	c.bnez	a5,2005f9b6 <unwind_frame_rv32c+0x124>
2005f8ee:	fd845783          	lhu	a5,-40(s0)
2005f8f2:	8b8d                	c.andi	a5,3
2005f8f4:	01079713          	slli	a4,a5,0x10
2005f8f8:	8341                	c.srli	a4,0x10
2005f8fa:	4785                	c.li	a5,1
2005f8fc:	0af71d63          	bne	a4,a5,2005f9b6 <unwind_frame_rv32c+0x124>
2005f900:	fd845703          	lhu	a4,-40(s0)
2005f904:	6785                	c.lui	a5,0x1
2005f906:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005f90a:	8ff9                	c.and	a5,a4
2005f90c:	01079713          	slli	a4,a5,0x10
2005f910:	8341                	c.srli	a4,0x10
2005f912:	10000793          	addi	a5,zero,256
2005f916:	0af71063          	bne	a4,a5,2005f9b6 <unwind_frame_rv32c+0x124>
2005f91a:	fd842783          	lw	a5,-40(s0)
2005f91e:	83b1                	c.srli	a5,0xc
2005f920:	8b85                	c.andi	a5,1
2005f922:	0ff7f793          	andi	a5,a5,255
2005f926:	0796                	c.slli	a5,0x5
2005f928:	01879713          	slli	a4,a5,0x18
2005f92c:	8761                	c.srai	a4,0x18
2005f92e:	fd842783          	lw	a5,-40(s0)
2005f932:	8389                	c.srli	a5,0x2
2005f934:	8bfd                	c.andi	a5,31
2005f936:	0ff7f793          	andi	a5,a5,255
2005f93a:	07e2                	c.slli	a5,0x18
2005f93c:	87e1                	c.srai	a5,0x18
2005f93e:	8fd9                	c.or	a5,a4
2005f940:	07e2                	c.slli	a5,0x18
2005f942:	87e1                	c.srai	a5,0x18
2005f944:	078a                	c.slli	a5,0x2
2005f946:	07e2                	c.slli	a5,0x18
2005f948:	87e1                	c.srai	a5,0x18
2005f94a:	8789                	c.srai	a5,0x2
2005f94c:	07e2                	c.slli	a5,0x18
2005f94e:	87e1                	c.srai	a5,0x18
2005f950:	03f7f693          	andi	a3,a5,63
2005f954:	fd444783          	lbu	a5,-44(s0)
2005f958:	fc07f793          	andi	a5,a5,-64
2005f95c:	873e                	c.mv	a4,a5
2005f95e:	87b6                	c.mv	a5,a3
2005f960:	8fd9                	c.or	a5,a4
2005f962:	fcf40a23          	sb	a5,-44(s0)
2005f966:	fd442783          	lw	a5,-44(s0)
2005f96a:	078a                	c.slli	a5,0x2
2005f96c:	07e2                	c.slli	a5,0x18
2005f96e:	87e1                	c.srai	a5,0x18
2005f970:	8789                	c.srai	a5,0x2
2005f972:	07e2                	c.slli	a5,0x18
2005f974:	87e1                	c.srai	a5,0x18
2005f976:	fef42023          	sw	a5,-32(s0)
2005f97a:	fe042783          	lw	a5,-32(s0)
2005f97e:	02f05363          	bge	zero,a5,2005f9a4 <unwind_frame_rv32c+0x112>
2005f982:	fe842783          	lw	a5,-24(s0)
2005f986:	86be                	c.mv	a3,a5
2005f988:	16900613          	addi	a2,zero,361
2005f98c:	200637b7          	lui	a5,0x20063
2005f990:	62c78593          	addi	a1,a5,1580 # 2006362c <__FUNCTION__.2>
2005f994:	200637b7          	lui	a5,0x20063
2005f998:	41878513          	addi	a0,a5,1048 # 20063418 <TaskStateString.0+0x1b8>
2005f99c:	e92ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005f9a0:	57fd                	c.li	a5,-1
2005f9a2:	a48d                	c.j	2005fc04 <unwind_frame_rv32c+0x372>
2005f9a4:	fe042783          	lw	a5,-32(s0)
2005f9a8:	0007d463          	bge	a5,zero,2005f9b0 <unwind_frame_rv32c+0x11e>
2005f9ac:	40f007b3          	sub	a5,zero,a5
2005f9b0:	fef42423          	sw	a5,-24(s0)
2005f9b4:	aaf1                	c.j	2005fb90 <unwind_frame_rv32c+0x2fe>
2005f9b6:	fd845703          	lhu	a4,-40(s0)
2005f9ba:	77f9                	c.lui	a5,0xffffe
2005f9bc:	8ff9                	c.and	a5,a4
2005f9be:	01079713          	slli	a4,a5,0x10
2005f9c2:	8341                	c.srli	a4,0x10
2005f9c4:	6799                	c.lui	a5,0x6
2005f9c6:	10f71e63          	bne	a4,a5,2005fae2 <unwind_frame_rv32c+0x250>
2005f9ca:	fd845783          	lhu	a5,-40(s0)
2005f9ce:	8b8d                	c.andi	a5,3
2005f9d0:	01079713          	slli	a4,a5,0x10
2005f9d4:	8341                	c.srli	a4,0x10
2005f9d6:	4785                	c.li	a5,1
2005f9d8:	10f71563          	bne	a4,a5,2005fae2 <unwind_frame_rv32c+0x250>
2005f9dc:	fd845703          	lhu	a4,-40(s0)
2005f9e0:	6785                	c.lui	a5,0x1
2005f9e2:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005f9e6:	8ff9                	c.and	a5,a4
2005f9e8:	01079713          	slli	a4,a5,0x10
2005f9ec:	8341                	c.srli	a4,0x10
2005f9ee:	10000793          	addi	a5,zero,256
2005f9f2:	0ef71863          	bne	a4,a5,2005fae2 <unwind_frame_rv32c+0x250>
2005f9f6:	fd842783          	lw	a5,-40(s0)
2005f9fa:	83b1                	c.srli	a5,0xc
2005f9fc:	8b85                	c.andi	a5,1
2005f9fe:	0ff7f793          	andi	a5,a5,255
2005fa02:	07a6                	c.slli	a5,0x9
2005fa04:	01079713          	slli	a4,a5,0x10
2005fa08:	8741                	c.srai	a4,0x10
2005fa0a:	fd842783          	lw	a5,-40(s0)
2005fa0e:	8399                	c.srli	a5,0x6
2005fa10:	8b85                	c.andi	a5,1
2005fa12:	0ff7f793          	andi	a5,a5,255
2005fa16:	0792                	c.slli	a5,0x4
2005fa18:	07c2                	c.slli	a5,0x10
2005fa1a:	87c1                	c.srai	a5,0x10
2005fa1c:	8fd9                	c.or	a5,a4
2005fa1e:	01079713          	slli	a4,a5,0x10
2005fa22:	8741                	c.srai	a4,0x10
2005fa24:	fd842783          	lw	a5,-40(s0)
2005fa28:	8395                	c.srli	a5,0x5
2005fa2a:	8b85                	c.andi	a5,1
2005fa2c:	0ff7f793          	andi	a5,a5,255
2005fa30:	079a                	c.slli	a5,0x6
2005fa32:	07c2                	c.slli	a5,0x10
2005fa34:	87c1                	c.srai	a5,0x10
2005fa36:	8fd9                	c.or	a5,a4
2005fa38:	01079713          	slli	a4,a5,0x10
2005fa3c:	8741                	c.srai	a4,0x10
2005fa3e:	fd842783          	lw	a5,-40(s0)
2005fa42:	838d                	c.srli	a5,0x3
2005fa44:	8b8d                	c.andi	a5,3
2005fa46:	0ff7f793          	andi	a5,a5,255
2005fa4a:	079e                	c.slli	a5,0x7
2005fa4c:	07c2                	c.slli	a5,0x10
2005fa4e:	87c1                	c.srai	a5,0x10
2005fa50:	8fd9                	c.or	a5,a4
2005fa52:	01079713          	slli	a4,a5,0x10
2005fa56:	8741                	c.srai	a4,0x10
2005fa58:	fd842783          	lw	a5,-40(s0)
2005fa5c:	8389                	c.srli	a5,0x2
2005fa5e:	8b85                	c.andi	a5,1
2005fa60:	0ff7f793          	andi	a5,a5,255
2005fa64:	0796                	c.slli	a5,0x5
2005fa66:	07c2                	c.slli	a5,0x10
2005fa68:	87c1                	c.srai	a5,0x10
2005fa6a:	8fd9                	c.or	a5,a4
2005fa6c:	07c2                	c.slli	a5,0x10
2005fa6e:	87c1                	c.srai	a5,0x10
2005fa70:	079a                	c.slli	a5,0x6
2005fa72:	07c2                	c.slli	a5,0x10
2005fa74:	87c1                	c.srai	a5,0x10
2005fa76:	8799                	c.srai	a5,0x6
2005fa78:	07c2                	c.slli	a5,0x10
2005fa7a:	87c1                	c.srai	a5,0x10
2005fa7c:	3ff7f693          	andi	a3,a5,1023
2005fa80:	fd045783          	lhu	a5,-48(s0)
2005fa84:	c007f793          	andi	a5,a5,-1024
2005fa88:	873e                	c.mv	a4,a5
2005fa8a:	87b6                	c.mv	a5,a3
2005fa8c:	8fd9                	c.or	a5,a4
2005fa8e:	fcf41823          	sh	a5,-48(s0)
2005fa92:	fd042783          	lw	a5,-48(s0)
2005fa96:	079a                	c.slli	a5,0x6
2005fa98:	07c2                	c.slli	a5,0x10
2005fa9a:	87c1                	c.srai	a5,0x10
2005fa9c:	8799                	c.srai	a5,0x6
2005fa9e:	07c2                	c.slli	a5,0x10
2005faa0:	87c1                	c.srai	a5,0x10
2005faa2:	fcf42e23          	sw	a5,-36(s0)
2005faa6:	fdc42783          	lw	a5,-36(s0)
2005faaa:	02f05363          	bge	zero,a5,2005fad0 <unwind_frame_rv32c+0x23e>
2005faae:	fe842783          	lw	a5,-24(s0)
2005fab2:	86be                	c.mv	a3,a5
2005fab4:	18100613          	addi	a2,zero,385
2005fab8:	200637b7          	lui	a5,0x20063
2005fabc:	62c78593          	addi	a1,a5,1580 # 2006362c <__FUNCTION__.2>
2005fac0:	200637b7          	lui	a5,0x20063
2005fac4:	41878513          	addi	a0,a5,1048 # 20063418 <TaskStateString.0+0x1b8>
2005fac8:	d66ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005facc:	57fd                	c.li	a5,-1
2005face:	aa1d                	c.j	2005fc04 <unwind_frame_rv32c+0x372>
2005fad0:	fdc42783          	lw	a5,-36(s0)
2005fad4:	0007d463          	bge	a5,zero,2005fadc <unwind_frame_rv32c+0x24a>
2005fad8:	40f007b3          	sub	a5,zero,a5
2005fadc:	fef42423          	sw	a5,-24(s0)
2005fae0:	a845                	c.j	2005fb90 <unwind_frame_rv32c+0x2fe>
2005fae2:	fd845703          	lhu	a4,-40(s0)
2005fae6:	77f9                	c.lui	a5,0xffffe
2005fae8:	8ff9                	c.and	a5,a4
2005faea:	01079713          	slli	a4,a5,0x10
2005faee:	8341                	c.srli	a4,0x10
2005faf0:	67b1                	c.lui	a5,0xc
2005faf2:	08f71663          	bne	a4,a5,2005fb7e <unwind_frame_rv32c+0x2ec>
2005faf6:	fd845783          	lhu	a5,-40(s0)
2005fafa:	8b8d                	c.andi	a5,3
2005fafc:	01079713          	slli	a4,a5,0x10
2005fb00:	8341                	c.srli	a4,0x10
2005fb02:	4789                	c.li	a5,2
2005fb04:	06f71d63          	bne	a4,a5,2005fb7e <unwind_frame_rv32c+0x2ec>
2005fb08:	fd845783          	lhu	a5,-40(s0)
2005fb0c:	07c7f793          	andi	a5,a5,124
2005fb10:	01079713          	slli	a4,a5,0x10
2005fb14:	8341                	c.srli	a4,0x10
2005fb16:	4791                	c.li	a5,4
2005fb18:	06f71363          	bne	a4,a5,2005fb7e <unwind_frame_rv32c+0x2ec>
2005fb1c:	fd842783          	lw	a5,-40(s0)
2005fb20:	839d                	c.srli	a5,0x7
2005fb22:	8b8d                	c.andi	a5,3
2005fb24:	0ff7f793          	andi	a5,a5,255
2005fb28:	079a                	c.slli	a5,0x6
2005fb2a:	01879713          	slli	a4,a5,0x18
2005fb2e:	8761                	c.srai	a4,0x18
2005fb30:	fd842783          	lw	a5,-40(s0)
2005fb34:	83a5                	c.srli	a5,0x9
2005fb36:	8bbd                	c.andi	a5,15
2005fb38:	0ff7f793          	andi	a5,a5,255
2005fb3c:	078a                	c.slli	a5,0x2
2005fb3e:	07e2                	c.slli	a5,0x18
2005fb40:	87e1                	c.srai	a5,0x18
2005fb42:	8fd9                	c.or	a5,a4
2005fb44:	07e2                	c.slli	a5,0x18
2005fb46:	87e1                	c.srai	a5,0x18
2005fb48:	fcf40623          	sb	a5,-52(s0)
2005fb4c:	fcc40783          	lb	a5,-52(s0)
2005fb50:	fef42623          	sw	a5,-20(s0)
2005fb54:	fec42783          	lw	a5,-20(s0)
2005fb58:	0207db63          	bge	a5,zero,2005fb8e <unwind_frame_rv32c+0x2fc>
2005fb5c:	fec42783          	lw	a5,-20(s0)
2005fb60:	86be                	c.mv	a3,a5
2005fb62:	19100613          	addi	a2,zero,401
2005fb66:	200637b7          	lui	a5,0x20063
2005fb6a:	62c78593          	addi	a1,a5,1580 # 2006362c <__FUNCTION__.2>
2005fb6e:	200637b7          	lui	a5,0x20063
2005fb72:	45478513          	addi	a0,a5,1108 # 20063454 <TaskStateString.0+0x1f4>
2005fb76:	cb8ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fb7a:	57fd                	c.li	a5,-1
2005fb7c:	a061                	c.j	2005fc04 <unwind_frame_rv32c+0x372>
2005fb7e:	fd845703          	lhu	a4,-40(s0)
2005fb82:	67a1                	c.lui	a5,0x8
2005fb84:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fb88:	02f70f63          	beq	a4,a5,2005fbc6 <unwind_frame_rv32c+0x334>
2005fb8c:	a011                	c.j	2005fb90 <unwind_frame_rv32c+0x2fe>
2005fb8e:	0001                	c.addi	zero,0
2005fb90:	fe442783          	lw	a5,-28(s0)
2005fb94:	17f9                	c.addi	a5,-2
2005fb96:	fef42223          	sw	a5,-28(s0)
2005fb9a:	fb842783          	lw	a5,-72(s0)
2005fb9e:	0786                	c.slli	a5,0x1
2005fba0:	fe442703          	lw	a4,-28(s0)
2005fba4:	973e                	c.add	a4,a5
2005fba6:	fbc42783          	lw	a5,-68(s0)
2005fbaa:	43dc                	c.lw	a5,4(a5)
2005fbac:	00e7fe63          	bgeu	a5,a4,2005fbc8 <unwind_frame_rv32c+0x336>
2005fbb0:	fec42783          	lw	a5,-20(s0)
2005fbb4:	c781                	c.beqz	a5,2005fbbc <unwind_frame_rv32c+0x32a>
2005fbb6:	fe842783          	lw	a5,-24(s0)
2005fbba:	e799                	c.bnez	a5,2005fbc8 <unwind_frame_rv32c+0x336>
2005fbbc:	fe442783          	lw	a5,-28(s0)
2005fbc0:	d0079ae3          	bne	a5,zero,2005f8d4 <unwind_frame_rv32c+0x42>
2005fbc4:	a011                	c.j	2005fbc8 <unwind_frame_rv32c+0x336>
2005fbc6:	0001                	c.addi	zero,0
2005fbc8:	fec42783          	lw	a5,-20(s0)
2005fbcc:	cb91                	c.beqz	a5,2005fbe0 <unwind_frame_rv32c+0x34e>
2005fbce:	fbc42783          	lw	a5,-68(s0)
2005fbd2:	4398                	c.lw	a4,0(a5)
2005fbd4:	fec42783          	lw	a5,-20(s0)
2005fbd8:	97ba                	c.add	a5,a4
2005fbda:	439c                	c.lw	a5,0(a5)
2005fbdc:	873e                	c.mv	a4,a5
2005fbde:	a011                	c.j	2005fbe2 <unwind_frame_rv32c+0x350>
2005fbe0:	4701                	c.li	a4,0
2005fbe2:	fbc42783          	lw	a5,-68(s0)
2005fbe6:	c798                	c.sw	a4,8(a5)
2005fbe8:	fe842703          	lw	a4,-24(s0)
2005fbec:	fbc42783          	lw	a5,-68(s0)
2005fbf0:	c7d8                	c.sw	a4,12(a5)
2005fbf2:	fec42783          	lw	a5,-20(s0)
2005fbf6:	c781                	c.beqz	a5,2005fbfe <unwind_frame_rv32c+0x36c>
2005fbf8:	fe842783          	lw	a5,-24(s0)
2005fbfc:	e399                	c.bnez	a5,2005fc02 <unwind_frame_rv32c+0x370>
2005fbfe:	57fd                	c.li	a5,-1
2005fc00:	a011                	c.j	2005fc04 <unwind_frame_rv32c+0x372>
2005fc02:	4781                	c.li	a5,0
2005fc04:	853e                	c.mv	a0,a5
2005fc06:	40b6                	c.lwsp	ra,76(sp)
2005fc08:	4426                	c.lwsp	s0,72(sp)
2005fc0a:	6161                	c.addi16sp	sp,80
2005fc0c:	8082                	c.jr	ra

2005fc0e <do_stack_backtrace>:
2005fc0e:	7179                	c.addi16sp	sp,-48
2005fc10:	ce52                	c.swsp	s4,28(sp)
2005fc12:	d606                	c.swsp	ra,44(sp)
2005fc14:	d426                	c.swsp	s1,40(sp)
2005fc16:	d24a                	c.swsp	s2,36(sp)
2005fc18:	d04e                	c.swsp	s3,32(sp)
2005fc1a:	cc56                	c.swsp	s5,24(sp)
2005fc1c:	c402                	c.swsp	zero,8(sp)
2005fc1e:	c602                	c.swsp	zero,12(sp)
2005fc20:	c02e                	c.swsp	a1,0(sp)
2005fc22:	8a32                	c.mv	s4,a2
2005fc24:	c525                	c.beqz	a0,2005fc8c <do_stack_backtrace+0x7e>
2005fc26:	c1bd                	c.beqz	a1,2005fc8c <do_stack_backtrace+0x7e>
2005fc28:	c22a                	c.swsp	a0,4(sp)
2005fc2a:	4712                	c.lwsp	a4,4(sp)
2005fc2c:	4682                	c.lwsp	a3,0(sp)
2005fc2e:	200634b7          	lui	s1,0x20063
2005fc32:	200639b7          	lui	s3,0x20063
2005fc36:	48c48513          	addi	a0,s1,1164 # 2006348c <TaskStateString.0+0x22c>
2005fc3a:	1cc00613          	addi	a2,zero,460
2005fc3e:	64098593          	addi	a1,s3,1600 # 20063640 <__FUNCTION__.1>
2005fc42:	48c48493          	addi	s1,s1,1164
2005fc46:	be8ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fc4a:	4901                	c.li	s2,0
2005fc4c:	04848493          	addi	s1,s1,72
2005fc50:	05490b63          	beq	s2,s4,2005fca6 <do_stack_backtrace+0x98>
2005fc54:	40000593          	addi	a1,zero,1024
2005fc58:	850a                	c.mv	a0,sp
2005fc5a:	963ff0ef          	jal	ra,2005f5bc <unwind_frame_rv32i>
2005fc5e:	ed15                	c.bnez	a0,2005fc9a <do_stack_backtrace+0x8c>
2005fc60:	4792                	c.lwsp	a5,4(sp)
2005fc62:	4702                	c.lwsp	a4,0(sp)
2005fc64:	48a2                	c.lwsp	a7,8(sp)
2005fc66:	4832                	c.lwsp	a6,12(sp)
2005fc68:	86ca                	c.mv	a3,s2
2005fc6a:	1d700613          	addi	a2,zero,471
2005fc6e:	64098593          	addi	a1,s3,1600
2005fc72:	8526                	c.mv	a0,s1
2005fc74:	bbaff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fc78:	47a2                	c.lwsp	a5,8(sp)
2005fc7a:	4732                	c.lwsp	a4,12(sp)
2005fc7c:	00190a93          	addi	s5,s2,1
2005fc80:	c23e                	c.swsp	a5,4(sp)
2005fc82:	4782                	c.lwsp	a5,0(sp)
2005fc84:	8956                	c.mv	s2,s5
2005fc86:	97ba                	c.add	a5,a4
2005fc88:	c03e                	c.swsp	a5,0(sp)
2005fc8a:	b7d9                	c.j	2005fc50 <do_stack_backtrace+0x42>
2005fc8c:	00010793          	addi	a5,sp,0
2005fc90:	00000717          	auipc	a4,0x0
2005fc94:	c23a                	c.swsp	a4,4(sp)
2005fc96:	c03e                	c.swsp	a5,0(sp)
2005fc98:	bf49                	c.j	2005fc2a <do_stack_backtrace+0x1c>
2005fc9a:	40000593          	addi	a1,zero,1024
2005fc9e:	850a                	c.mv	a0,sp
2005fca0:	bf3ff0ef          	jal	ra,2005f892 <unwind_frame_rv32c>
2005fca4:	dd55                	c.beqz	a0,2005fc60 <do_stack_backtrace+0x52>
2005fca6:	50b2                	c.lwsp	ra,44(sp)
2005fca8:	54a2                	c.lwsp	s1,40(sp)
2005fcaa:	5982                	c.lwsp	s3,32(sp)
2005fcac:	4a72                	c.lwsp	s4,28(sp)
2005fcae:	4ae2                	c.lwsp	s5,24(sp)
2005fcb0:	854a                	c.mv	a0,s2
2005fcb2:	5912                	c.lwsp	s2,36(sp)
2005fcb4:	6145                	c.addi16sp	sp,48
2005fcb6:	8082                	c.jr	ra

2005fcb8 <crash_dump>:
2005fcb8:	711d                	c.addi16sp	sp,-96
2005fcba:	c8ca                	c.swsp	s2,80(sp)
2005fcbc:	c6ce                	c.swsp	s3,76(sp)
2005fcbe:	c2d6                	c.swsp	s5,68(sp)
2005fcc0:	ce86                	c.swsp	ra,92(sp)
2005fcc2:	cca2                	c.swsp	s0,88(sp)
2005fcc4:	caa6                	c.swsp	s1,84(sp)
2005fcc6:	c4d2                	c.swsp	s4,72(sp)
2005fcc8:	c0da                	c.swsp	s6,64(sp)
2005fcca:	de5e                	c.swsp	s7,60(sp)
2005fccc:	dc62                	c.swsp	s8,56(sp)
2005fcce:	da66                	c.swsp	s9,52(sp)
2005fcd0:	d86a                	c.swsp	s10,48(sp)
2005fcd2:	d66e                	c.swsp	s11,44(sp)
2005fcd4:	89aa                	c.mv	s3,a0
2005fcd6:	892e                	c.mv	s2,a1
2005fcd8:	8ab2                	c.mv	s5,a2
2005fcda:	34002c73          	csrrs	s8,mscratch,zero
2005fcde:	34102cf3          	csrrs	s9,mepc,zero
2005fce2:	34202a73          	csrrs	s4,mcause,zero
2005fce6:	34302bf3          	csrrs	s7,mtval,zero
2005fcea:	20063437          	lui	s0,0x20063
2005fcee:	65440513          	addi	a0,s0,1620 # 20063654 <__FUNCTION__.1+0x14>
2005fcf2:	200644b7          	lui	s1,0x20064
2005fcf6:	11200613          	addi	a2,zero,274
2005fcfa:	df048593          	addi	a1,s1,-528 # 20063df0 <__FUNCTION__.0>
2005fcfe:	0551                	c.addi	a0,20
2005fd00:	b2eff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fd04:	ea01a783          	lw	a5,-352(gp) # 20065f78 <crash_task_info>
2005fd08:	65440413          	addi	s0,s0,1620
2005fd0c:	c399                	c.beqz	a5,2005fd12 <crash_dump+0x5a>
2005fd0e:	8556                	c.mv	a0,s5
2005fd10:	9782                	c.jalr	a5
2005fd12:	473d                	c.li	a4,15
2005fd14:	87a2                	c.mv	a5,s0
2005fd16:	01476a63          	bltu	a4,s4,2005fd2a <crash_dump+0x72>
2005fd1a:	20064737          	lui	a4,0x20064
2005fd1e:	002a1793          	slli	a5,s4,0x2
2005fd22:	f1470713          	addi	a4,a4,-236 # 20063f14 <exception_code_string>
2005fd26:	97ba                	c.add	a5,a4
2005fd28:	439c                	c.lw	a5,0(a5)
2005fd2a:	8752                	c.mv	a4,s4
2005fd2c:	86ce                	c.mv	a3,s3
2005fd2e:	11700613          	addi	a2,zero,279
2005fd32:	df048593          	addi	a1,s1,-528
2005fd36:	05440513          	addi	a0,s0,84
2005fd3a:	af4ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fd3e:	47bd                	c.li	a5,15
2005fd40:	1547f463          	bgeu	a5,s4,2005fe88 <crash_dump+0x1d0>
2005fd44:	12b00613          	addi	a2,zero,299
2005fd48:	df048593          	addi	a1,s1,-528
2005fd4c:	0a440513          	addi	a0,s0,164
2005fd50:	adeff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fd54:	86e2                	c.mv	a3,s8
2005fd56:	12c00613          	addi	a2,zero,300
2005fd5a:	df048593          	addi	a1,s1,-528
2005fd5e:	0e840513          	addi	a0,s0,232
2005fd62:	accff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fd66:	86e6                	c.mv	a3,s9
2005fd68:	12d00613          	addi	a2,zero,301
2005fd6c:	df048593          	addi	a1,s1,-528
2005fd70:	11840513          	addi	a0,s0,280
2005fd74:	abaff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fd78:	86d2                	c.mv	a3,s4
2005fd7a:	12e00613          	addi	a2,zero,302
2005fd7e:	df048593          	addi	a1,s1,-528
2005fd82:	14840513          	addi	a0,s0,328
2005fd86:	aa8ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fd8a:	86de                	c.mv	a3,s7
2005fd8c:	12f00613          	addi	a2,zero,303
2005fd90:	df048593          	addi	a1,s1,-528
2005fd94:	17840513          	addi	a0,s0,376
2005fd98:	20064b37          	lui	s6,0x20064
2005fd9c:	a92ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fda0:	f54b0b13          	addi	s6,s6,-172 # 20063f54 <register_string>
2005fda4:	4a01                	c.li	s4,0
2005fda6:	23440c13          	addi	s8,s0,564
2005fdaa:	02000b93          	addi	s7,zero,32
2005fdae:	002a1793          	slli	a5,s4,0x2
2005fdb2:	97d6                	c.add	a5,s5
2005fdb4:	000b2703          	lw	a4,0(s6)
2005fdb8:	439c                	c.lw	a5,0(a5)
2005fdba:	86d2                	c.mv	a3,s4
2005fdbc:	13400613          	addi	a2,zero,308
2005fdc0:	df048593          	addi	a1,s1,-528
2005fdc4:	8562                	c.mv	a0,s8
2005fdc6:	0a05                	c.addi	s4,1
2005fdc8:	a66ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fdcc:	0b11                	c.addi	s6,4
2005fdce:	ff7a10e3          	bne	s4,s7,2005fdae <crash_dump+0xf6>
2005fdd2:	15a00613          	addi	a2,zero,346
2005fdd6:	df048593          	addi	a1,s1,-528
2005fdda:	26440513          	addi	a0,s0,612
2005fdde:	a50ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fde2:	4629                	c.li	a2,10
2005fde4:	85ca                	c.mv	a1,s2
2005fde6:	854e                	c.mv	a0,s3
2005fde8:	e27ff0ef          	jal	ra,2005fc0e <do_stack_backtrace>
2005fdec:	e921                	c.bnez	a0,2005fe3c <crash_dump+0x184>
2005fdee:	40000593          	addi	a1,zero,1024
2005fdf2:	850a                	c.mv	a0,sp
2005fdf4:	c24e                	c.swsp	s3,4(sp)
2005fdf6:	ca4e                	c.swsp	s3,20(sp)
2005fdf8:	c402                	c.swsp	zero,8(sp)
2005fdfa:	c602                	c.swsp	zero,12(sp)
2005fdfc:	c04a                	c.swsp	s2,0(sp)
2005fdfe:	cc02                	c.swsp	zero,24(sp)
2005fe00:	ce02                	c.swsp	zero,28(sp)
2005fe02:	c84a                	c.swsp	s2,16(sp)
2005fe04:	fb8ff0ef          	jal	ra,2005f5bc <unwind_frame_rv32i>
2005fe08:	40000593          	addi	a1,zero,1024
2005fe0c:	0808                	c.addi4spn	a0,sp,16
2005fe0e:	a85ff0ef          	jal	ra,2005f892 <unwind_frame_rv32c>
2005fe12:	46b2                	c.lwsp	a3,12(sp)
2005fe14:	4772                	c.lwsp	a4,28(sp)
2005fe16:	89b6                	c.mv	s3,a3
2005fe18:	e291                	c.bnez	a3,2005fe1c <crash_dump+0x164>
2005fe1a:	89ba                	c.mv	s3,a4
2005fe1c:	16c00613          	addi	a2,zero,364
2005fe20:	df048593          	addi	a1,s1,-528
2005fe24:	2a440513          	addi	a0,s0,676
2005fe28:	a06ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fe2c:	004aa503          	lw	a0,4(s5)
2005fe30:	ffc9f593          	andi	a1,s3,-4
2005fe34:	4629                	c.li	a2,10
2005fe36:	95ca                	c.add	a1,s2
2005fe38:	dd7ff0ef          	jal	ra,2005fc0e <do_stack_backtrace>
2005fe3c:	df048593          	addi	a1,s1,-528
2005fe40:	17000613          	addi	a2,zero,368
2005fe44:	30c40513          	addi	a0,s0,780
2005fe48:	9e6ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fe4c:	df048593          	addi	a1,s1,-528
2005fe50:	18300613          	addi	a2,zero,387
2005fe54:	35440513          	addi	a0,s0,852
2005fe58:	9d6ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fe5c:	39c40513          	addi	a0,s0,924
2005fe60:	4466                	c.lwsp	s0,88(sp)
2005fe62:	40f6                	c.lwsp	ra,92(sp)
2005fe64:	4946                	c.lwsp	s2,80(sp)
2005fe66:	49b6                	c.lwsp	s3,76(sp)
2005fe68:	4a26                	c.lwsp	s4,72(sp)
2005fe6a:	4a96                	c.lwsp	s5,68(sp)
2005fe6c:	4b06                	c.lwsp	s6,64(sp)
2005fe6e:	5bf2                	c.lwsp	s7,60(sp)
2005fe70:	5c62                	c.lwsp	s8,56(sp)
2005fe72:	5cd2                	c.lwsp	s9,52(sp)
2005fe74:	5d42                	c.lwsp	s10,48(sp)
2005fe76:	5db2                	c.lwsp	s11,44(sp)
2005fe78:	df048593          	addi	a1,s1,-528
2005fe7c:	44d6                	c.lwsp	s1,84(sp)
2005fe7e:	18400613          	addi	a2,zero,388
2005fe82:	6125                	c.addi16sp	sp,96
2005fe84:	9aaff06f          	jal	zero,2005f02e <DiagPrintf>
2005fe88:	20064737          	lui	a4,0x20064
2005fe8c:	002a1793          	slli	a5,s4,0x2
2005fe90:	ed470713          	addi	a4,a4,-300 # 20063ed4 <exception_cause_string_tbl>
2005fe94:	97ba                	c.add	a5,a4
2005fe96:	0007ab03          	lw	s6,0(a5)
2005fe9a:	ea0b05e3          	beq	s6,zero,2005fd44 <crash_dump+0x8c>
2005fe9e:	11d00613          	addi	a2,zero,285
2005fea2:	df048593          	addi	a1,s1,-528
2005fea6:	1a840513          	addi	a0,s0,424
2005feaa:	984ff0ef          	jal	ra,2005f02e <DiagPrintf>
2005feae:	21040d13          	addi	s10,s0,528
2005feb2:	1e040d93          	addi	s11,s0,480
2005feb6:	000b2683          	lw	a3,0(s6)
2005feba:	e80685e3          	beq	a3,zero,2005fd44 <crash_dump+0x8c>
2005febe:	004b2703          	lw	a4,4(s6)
2005fec2:	cb19                	c.beqz	a4,2005fed8 <crash_dump+0x220>
2005fec4:	87de                	c.mv	a5,s7
2005fec6:	12000613          	addi	a2,zero,288
2005feca:	df048593          	addi	a1,s1,-528
2005fece:	856e                	c.mv	a0,s11
2005fed0:	95eff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fed4:	0b21                	c.addi	s6,8
2005fed6:	b7c5                	c.j	2005feb6 <crash_dump+0x1fe>
2005fed8:	12300613          	addi	a2,zero,291
2005fedc:	df048593          	addi	a1,s1,-528
2005fee0:	856a                	c.mv	a0,s10
2005fee2:	94cff0ef          	jal	ra,2005f02e <DiagPrintf>
2005fee6:	b7fd                	c.j	2005fed4 <crash_dump+0x21c>

2005fee8 <_my_calloc>:
2005fee8:	02b50533          	mul	a0,a0,a1
2005feec:	859fb06f          	jal	zero,2005b744 <rtos_mem_zmalloc>

2005fef0 <_my_free>:
2005fef0:	87bfb06f          	jal	zero,2005b76a <rtos_mem_free>

2005fef4 <_my_random>:
2005fef4:	1141                	c.addi	sp,-16
2005fef6:	852e                	c.mv	a0,a1
2005fef8:	85b2                	c.mv	a1,a2
2005fefa:	c606                	c.swsp	ra,12(sp)
2005fefc:	daaff0ef          	jal	ra,2005f4a6 <TRNG_get_random_bytes>
2005ff00:	40b2                	c.lwsp	ra,12(sp)
2005ff02:	4501                	c.li	a0,0
2005ff04:	0141                	c.addi	sp,16
2005ff06:	8082                	c.jr	ra

2005ff08 <wifi_on>:
2005ff08:	1101                	c.addi	sp,-32
2005ff0a:	ce06                	c.swsp	ra,28(sp)
2005ff0c:	cc22                	c.swsp	s0,24(sp)
2005ff0e:	ca26                	c.swsp	s1,20(sp)
2005ff10:	842a                	c.mv	s0,a0
2005ff12:	c84a                	c.swsp	s2,16(sp)
2005ff14:	cb9f90ef          	jal	ra,20059bcc <wifi_set_user_config>
2005ff18:	3e5000ef          	jal	ra,20060afc <inic_msg_q_init>
2005ff1c:	42818793          	addi	a5,gp,1064 # 20066500 <wifi_user_config>
2005ff20:	7c102773          	csrrs	a4,0x7c1,zero
2005ff24:	10077713          	andi	a4,a4,256
2005ff28:	c715                	c.beqz	a4,2005ff54 <wifi_on+0x4c>
2005ff2a:	0ff0000f          	fence	iorw,iorw
2005ff2e:	04f7f00b          	cache	dwb,(a5)
2005ff32:	02078713          	addi	a4,a5,32
2005ff36:	04e7700b          	cache	dwb,(a4)
2005ff3a:	0ff0000f          	fence	iorw,iorw
2005ff3e:	0001                	c.addi	zero,0
2005ff40:	0001                	c.addi	zero,0
2005ff42:	0001                	c.addi	zero,0
2005ff44:	0001                	c.addi	zero,0
2005ff46:	0001                	c.addi	zero,0
2005ff48:	0001                	c.addi	zero,0
2005ff4a:	0001                	c.addi	zero,0
2005ff4c:	0001                	c.addi	zero,0
2005ff4e:	0001                	c.addi	zero,0
2005ff50:	0001                	c.addi	zero,0
2005ff52:	0001                	c.addi	zero,0
2005ff54:	4605                	c.li	a2,1
2005ff56:	006c                	c.addi4spn	a1,sp,12
2005ff58:	451d                	c.li	a0,7
2005ff5a:	c63e                	c.swsp	a5,12(sp)
2005ff5c:	2909                	c.jal	2006036e <inic_api_host_message_send>
2005ff5e:	200687b7          	lui	a5,0x20068
2005ff62:	fa078793          	addi	a5,a5,-96 # 20067fa0 <wifi_rom_func_map>
2005ff66:	fef1a623          	sw	a5,-20(gp) # 200660c4 <p_wifi_rom_func_map>
2005ff6a:	20060737          	lui	a4,0x20060
2005ff6e:	ee870713          	addi	a4,a4,-280 # 2005fee8 <_my_calloc>
2005ff72:	c398                	c.sw	a4,0(a5)
2005ff74:	20060737          	lui	a4,0x20060
2005ff78:	ef070713          	addi	a4,a4,-272 # 2005fef0 <_my_free>
2005ff7c:	c3d8                	c.sw	a4,4(a5)
2005ff7e:	20060737          	lui	a4,0x20060
2005ff82:	ef470713          	addi	a4,a4,-268 # 2005fef4 <_my_random>
2005ff86:	c798                	c.sw	a4,8(a5)
2005ff88:	003000ef          	jal	ra,2006078a <inic_host_init_skb>
2005ff8c:	4605                	c.li	a2,1
2005ff8e:	006c                	c.addi4spn	a1,sp,12
2005ff90:	4525                	c.li	a0,9
2005ff92:	c622                	c.swsp	s0,12(sp)
2005ff94:	2ee9                	c.jal	2006036e <inic_api_host_message_send>
2005ff96:	fd418793          	addi	a5,gp,-44 # 200660ac <wifi_boot.0>
2005ff9a:	4398                	c.lw	a4,0(a5)
2005ff9c:	84aa                	c.mv	s1,a0
2005ff9e:	ef01                	c.bnez	a4,2005ffb6 <wifi_on+0xae>
2005ffa0:	4905                	c.li	s2,1
2005ffa2:	0127a023          	sw	s2,0(a5)
2005ffa6:	537000ef          	jal	ra,20060cdc <init_timer_wrapper>
2005ffaa:	fc01a783          	lw	a5,-64(gp) # 20066098 <p_wifi_do_fast_connect>
2005ffae:	c781                	c.beqz	a5,2005ffb6 <wifi_on+0xae>
2005ffb0:	01241363          	bne	s0,s2,2005ffb6 <wifi_on+0xae>
2005ffb4:	9782                	c.jalr	a5
2005ffb6:	40f2                	c.lwsp	ra,28(sp)
2005ffb8:	4462                	c.lwsp	s0,24(sp)
2005ffba:	4942                	c.lwsp	s2,16(sp)
2005ffbc:	8526                	c.mv	a0,s1
2005ffbe:	44d2                	c.lwsp	s1,20(sp)
2005ffc0:	6105                	c.addi16sp	sp,32
2005ffc2:	8082                	c.jr	ra

2005ffc4 <wifi_config_autoreconnect>:
2005ffc4:	557d                	c.li	a0,-1
2005ffc6:	8082                	c.jr	ra

2005ffc8 <_inic_ipc_ip_addr_update_in_wowlan>:
2005ffc8:	4505                	c.li	a0,1
2005ffca:	8082                	c.jr	ra

2005ffcc <inic_api_host_task_h>:
2005ffcc:	7179                	c.addi16sp	sp,-48
2005ffce:	d226                	c.swsp	s1,36(sp)
2005ffd0:	200644b7          	lui	s1,0x20064
2005ffd4:	d04a                	c.swsp	s2,32(sp)
2005ffd6:	ce4e                	c.swsp	s3,28(sp)
2005ffd8:	cc52                	c.swsp	s4,24(sp)
2005ffda:	ca56                	c.swsp	s5,20(sp)
2005ffdc:	c85a                	c.swsp	s6,16(sp)
2005ffde:	c65e                	c.swsp	s7,12(sp)
2005ffe0:	d606                	c.swsp	ra,44(sp)
2005ffe2:	d422                	c.swsp	s0,40(sp)
2005ffe4:	20064a37          	lui	s4,0x20064
2005ffe8:	00848493          	addi	s1,s1,8 # 20064008 <register_string+0xb4>
2005ffec:	fe01a503          	lw	a0,-32(gp) # 200660b8 <g_host_inic_api_task_wake_sema>
2005fff0:	55fd                	c.li	a1,-1
2005fff2:	87dfb0ef          	jal	ra,2005b86e <rtos_sema_take>
2005fff6:	459d                	c.li	a1,7
2005fff8:	4541                	c.li	a0,16
2005fffa:	8f6f90ef          	jal	ra,200590f0 <ipc_get_message>
2005fffe:	4140                	c.lw	s0,4(a0)
20060000:	7c1027f3          	csrrs	a5,0x7c1,zero
20060004:	1007f793          	andi	a5,a5,256
20060008:	c3b9                	c.beqz	a5,2006004e <inic_api_host_task_h+0x82>
2006000a:	01f47713          	andi	a4,s0,31
2006000e:	cb21                	c.beqz	a4,2006005e <inic_api_host_task_h+0x92>
20060010:	04070693          	addi	a3,a4,64
20060014:	fe047793          	andi	a5,s0,-32
20060018:	01f7f713          	andi	a4,a5,31
2006001c:	9736                	c.add	a4,a3
2006001e:	0ff0000f          	fence	iorw,iorw
20060022:	973e                	c.add	a4,a5
20060024:	00f7f00b          	cache	dinv,(a5)
20060028:	02078793          	addi	a5,a5,32
2006002c:	40f706b3          	sub	a3,a4,a5
20060030:	fed04ae3          	blt	zero,a3,20060024 <inic_api_host_task_h+0x58>
20060034:	0ff0000f          	fence	iorw,iorw
20060038:	0001                	c.addi	zero,0
2006003a:	0001                	c.addi	zero,0
2006003c:	0001                	c.addi	zero,0
2006003e:	0001                	c.addi	zero,0
20060040:	0001                	c.addi	zero,0
20060042:	0001                	c.addi	zero,0
20060044:	0001                	c.addi	zero,0
20060046:	0001                	c.addi	zero,0
20060048:	0001                	c.addi	zero,0
2006004a:	0001                	c.addi	zero,0
2006004c:	0001                	c.addi	zero,0
2006004e:	ec01                	c.bnez	s0,20060066 <inic_api_host_task_h+0x9a>
20060050:	20064537          	lui	a0,0x20064
20060054:	fd450513          	addi	a0,a0,-44 # 20063fd4 <register_string+0x80>
20060058:	ff7fe0ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2006005c:	bf41                	c.j	2005ffec <inic_api_host_task_h+0x20>
2006005e:	87a2                	c.mv	a5,s0
20060060:	04000693          	addi	a3,zero,64
20060064:	bf55                	c.j	20060018 <inic_api_host_task_h+0x4c>
20060066:	400c                	c.lw	a1,0(s0)
20060068:	4721                	c.li	a4,8
2006006a:	fff58793          	addi	a5,a1,-1
2006006e:	2ef76763          	bltu	a4,a5,2006035c <inic_api_host_task_h+0x390>
20060072:	078a                	c.slli	a5,0x2
20060074:	97a6                	c.add	a5,s1
20060076:	439c                	c.lw	a5,0(a5)
20060078:	8782                	c.jr	a5
2006007a:	fd01a783          	lw	a5,-48(gp) # 200660a8 <scan_user_callback_ptr>
2006007e:	c3ad                	c.beqz	a5,200600e0 <inic_api_host_task_h+0x114>
20060080:	440c                	c.lw	a1,8(s0)
20060082:	4048                	c.lw	a0,4(s0)
20060084:	9782                	c.jalr	a5
20060086:	a8a9                	c.j	200600e0 <inic_api_host_task_h+0x114>
20060088:	4048                	c.lw	a0,4(s0)
2006008a:	440c                	c.lw	a1,8(s0)
2006008c:	7c1027f3          	csrrs	a5,0x7c1,zero
20060090:	1007f793          	andi	a5,a5,256
20060094:	c3b9                	c.beqz	a5,200600da <inic_api_host_task_h+0x10e>
20060096:	01f57713          	andi	a4,a0,31
2006009a:	cf59                	c.beqz	a4,20060138 <inic_api_host_task_h+0x16c>
2006009c:	03570693          	addi	a3,a4,53
200600a0:	fe057793          	andi	a5,a0,-32
200600a4:	01f7f713          	andi	a4,a5,31
200600a8:	9736                	c.add	a4,a3
200600aa:	0ff0000f          	fence	iorw,iorw
200600ae:	973e                	c.add	a4,a5
200600b0:	00f7f00b          	cache	dinv,(a5)
200600b4:	02078793          	addi	a5,a5,32
200600b8:	40f706b3          	sub	a3,a4,a5
200600bc:	fed04ae3          	blt	zero,a3,200600b0 <inic_api_host_task_h+0xe4>
200600c0:	0ff0000f          	fence	iorw,iorw
200600c4:	0001                	c.addi	zero,0
200600c6:	0001                	c.addi	zero,0
200600c8:	0001                	c.addi	zero,0
200600ca:	0001                	c.addi	zero,0
200600cc:	0001                	c.addi	zero,0
200600ce:	0001                	c.addi	zero,0
200600d0:	0001                	c.addi	zero,0
200600d2:	0001                	c.addi	zero,0
200600d4:	0001                	c.addi	zero,0
200600d6:	0001                	c.addi	zero,0
200600d8:	0001                	c.addi	zero,0
200600da:	fcc1a783          	lw	a5,-52(gp) # 200660a4 <scan_each_report_user_callback_ptr>
200600de:	f3dd                	c.bnez	a5,20060084 <inic_api_host_task_h+0xb8>
200600e0:	00042023          	sw	zero,0(s0)
200600e4:	7c1027f3          	csrrs	a5,0x7c1,zero
200600e8:	1007f793          	andi	a5,a5,256
200600ec:	f00780e3          	beq	a5,zero,2005ffec <inic_api_host_task_h+0x20>
200600f0:	01f47793          	andi	a5,s0,31
200600f4:	04000713          	addi	a4,zero,64
200600f8:	c781                	c.beqz	a5,20060100 <inic_api_host_task_h+0x134>
200600fa:	04078713          	addi	a4,a5,64
200600fe:	9801                	c.andi	s0,-32
20060100:	01f47793          	andi	a5,s0,31
20060104:	97ba                	c.add	a5,a4
20060106:	0ff0000f          	fence	iorw,iorw
2006010a:	97a2                	c.add	a5,s0
2006010c:	0484700b          	cache	dwb,(s0)
20060110:	02040413          	addi	s0,s0,32
20060114:	40878733          	sub	a4,a5,s0
20060118:	fee04ae3          	blt	zero,a4,2006010c <inic_api_host_task_h+0x140>
2006011c:	0ff0000f          	fence	iorw,iorw
20060120:	0001                	c.addi	zero,0
20060122:	0001                	c.addi	zero,0
20060124:	0001                	c.addi	zero,0
20060126:	0001                	c.addi	zero,0
20060128:	0001                	c.addi	zero,0
2006012a:	0001                	c.addi	zero,0
2006012c:	0001                	c.addi	zero,0
2006012e:	0001                	c.addi	zero,0
20060130:	0001                	c.addi	zero,0
20060132:	0001                	c.addi	zero,0
20060134:	0001                	c.addi	zero,0
20060136:	bd5d                	c.j	2005ffec <inic_api_host_task_h+0x20>
20060138:	87aa                	c.mv	a5,a0
2006013a:	03500693          	addi	a3,zero,53
2006013e:	b79d                	c.j	200600a4 <inic_api_host_task_h+0xd8>
20060140:	4418                	c.lw	a4,8(s0)
20060142:	444c                	c.lw	a1,12(s0)
20060144:	481c                	c.lw	a5,16(s0)
20060146:	4854                	c.lw	a3,20(s0)
20060148:	7c102673          	csrrs	a2,0x7c1,zero
2006014c:	10067613          	andi	a2,a2,256
20060150:	c60d                	c.beqz	a2,2006017a <inic_api_host_task_h+0x1ae>
20060152:	00b77633          	and	a2,a4,a1
20060156:	557d                	c.li	a0,-1
20060158:	04a61b63          	bne	a2,a0,200601ae <inic_api_host_task_h+0x1e2>
2006015c:	7c016773          	csrrsi	a4,0x7c0,2
20060160:	0ff0000f          	fence	iorw,iorw
20060164:	0001                	c.addi	zero,0
20060166:	0001                	c.addi	zero,0
20060168:	0001                	c.addi	zero,0
2006016a:	0001                	c.addi	zero,0
2006016c:	0001                	c.addi	zero,0
2006016e:	0001                	c.addi	zero,0
20060170:	0001                	c.addi	zero,0
20060172:	0001                	c.addi	zero,0
20060174:	0001                	c.addi	zero,0
20060176:	0001                	c.addi	zero,0
20060178:	0001                	c.addi	zero,0
2006017a:	7c102773          	csrrs	a4,0x7c1,zero
2006017e:	10077713          	andi	a4,a4,256
20060182:	df39                	c.beqz	a4,200600e0 <inic_api_host_task_h+0x114>
20060184:	00d7f733          	and	a4,a5,a3
20060188:	567d                	c.li	a2,-1
2006018a:	04c71863          	bne	a4,a2,200601da <inic_api_host_task_h+0x20e>
2006018e:	7c0167f3          	csrrsi	a5,0x7c0,2
20060192:	0ff0000f          	fence	iorw,iorw
20060196:	0001                	c.addi	zero,0
20060198:	0001                	c.addi	zero,0
2006019a:	0001                	c.addi	zero,0
2006019c:	0001                	c.addi	zero,0
2006019e:	0001                	c.addi	zero,0
200601a0:	0001                	c.addi	zero,0
200601a2:	0001                	c.addi	zero,0
200601a4:	0001                	c.addi	zero,0
200601a6:	0001                	c.addi	zero,0
200601a8:	0001                	c.addi	zero,0
200601aa:	0001                	c.addi	zero,0
200601ac:	bf15                	c.j	200600e0 <inic_api_host_task_h+0x114>
200601ae:	01f77613          	andi	a2,a4,31
200601b2:	c219                	c.beqz	a2,200601b8 <inic_api_host_task_h+0x1ec>
200601b4:	9b01                	c.andi	a4,-32
200601b6:	95b2                	c.add	a1,a2
200601b8:	fcb051e3          	bge	zero,a1,2006017a <inic_api_host_task_h+0x1ae>
200601bc:	01f77613          	andi	a2,a4,31
200601c0:	962e                	c.add	a2,a1
200601c2:	0ff0000f          	fence	iorw,iorw
200601c6:	963a                	c.add	a2,a4
200601c8:	00e7700b          	cache	dinv,(a4)
200601cc:	02070713          	addi	a4,a4,32
200601d0:	40e605b3          	sub	a1,a2,a4
200601d4:	feb04ae3          	blt	zero,a1,200601c8 <inic_api_host_task_h+0x1fc>
200601d8:	b761                	c.j	20060160 <inic_api_host_task_h+0x194>
200601da:	01f7f713          	andi	a4,a5,31
200601de:	c319                	c.beqz	a4,200601e4 <inic_api_host_task_h+0x218>
200601e0:	9b81                	c.andi	a5,-32
200601e2:	96ba                	c.add	a3,a4
200601e4:	eed05ee3          	bge	zero,a3,200600e0 <inic_api_host_task_h+0x114>
200601e8:	01f7f713          	andi	a4,a5,31
200601ec:	9736                	c.add	a4,a3
200601ee:	0ff0000f          	fence	iorw,iorw
200601f2:	973e                	c.add	a4,a5
200601f4:	00f7f00b          	cache	dinv,(a5)
200601f8:	02078793          	addi	a5,a5,32
200601fc:	40f706b3          	sub	a3,a4,a5
20060200:	fed04ae3          	blt	zero,a3,200601f4 <inic_api_host_task_h+0x228>
20060204:	b779                	c.j	20060192 <inic_api_host_task_h+0x1c6>
20060206:	fd81a783          	lw	a5,-40(gp) # 200660b0 <p_ap_channel_switch_callback>
2006020a:	ec078be3          	beq	a5,zero,200600e0 <inic_api_host_task_h+0x114>
2006020e:	00840583          	lb	a1,8(s0)
20060212:	00444503          	lbu	a0,4(s0)
20060216:	9782                	c.jalr	a5
20060218:	b5e1                	c.j	200600e0 <inic_api_host_task_h+0x114>
2006021a:	440c                	c.lw	a1,8(s0)
2006021c:	4450                	c.lw	a2,12(s0)
2006021e:	00444503          	lbu	a0,4(s0)
20060222:	4814                	c.lw	a3,16(s0)
20060224:	87ae                	c.mv	a5,a1
20060226:	8832                	c.mv	a6,a2
20060228:	7c102773          	csrrs	a4,0x7c1,zero
2006022c:	10077713          	andi	a4,a4,256
20060230:	c70d                	c.beqz	a4,2006025a <inic_api_host_task_h+0x28e>
20060232:	00c5f733          	and	a4,a1,a2
20060236:	58fd                	c.li	a7,-1
20060238:	03171463          	bne	a4,a7,20060260 <inic_api_host_task_h+0x294>
2006023c:	7c0167f3          	csrrsi	a5,0x7c0,2
20060240:	0ff0000f          	fence	iorw,iorw
20060244:	0001                	c.addi	zero,0
20060246:	0001                	c.addi	zero,0
20060248:	0001                	c.addi	zero,0
2006024a:	0001                	c.addi	zero,0
2006024c:	0001                	c.addi	zero,0
2006024e:	0001                	c.addi	zero,0
20060250:	0001                	c.addi	zero,0
20060252:	0001                	c.addi	zero,0
20060254:	0001                	c.addi	zero,0
20060256:	0001                	c.addi	zero,0
20060258:	0001                	c.addi	zero,0
2006025a:	c12fe0ef          	jal	ra,2005e66c <wifi_indication>
2006025e:	b549                	c.j	200600e0 <inic_api_host_task_h+0x114>
20060260:	01f5f713          	andi	a4,a1,31
20060264:	c709                	c.beqz	a4,2006026e <inic_api_host_task_h+0x2a2>
20060266:	fe05f793          	andi	a5,a1,-32
2006026a:	00e60833          	add	a6,a2,a4
2006026e:	ff0056e3          	bge	zero,a6,2006025a <inic_api_host_task_h+0x28e>
20060272:	01f7f713          	andi	a4,a5,31
20060276:	9742                	c.add	a4,a6
20060278:	0ff0000f          	fence	iorw,iorw
2006027c:	973e                	c.add	a4,a5
2006027e:	00f7f00b          	cache	dinv,(a5)
20060282:	02078793          	addi	a5,a5,32
20060286:	40f70833          	sub	a6,a4,a5
2006028a:	ff004ae3          	blt	zero,a6,2006027e <inic_api_host_task_h+0x2b2>
2006028e:	bf4d                	c.j	20060240 <inic_api_host_task_h+0x274>
20060290:	fc81a783          	lw	a5,-56(gp) # 200660a0 <promisc_user_callback_ptr>
20060294:	4501                	c.li	a0,0
20060296:	cbc1                	c.beqz	a5,20060326 <inic_api_host_task_h+0x35a>
20060298:	4058                	c.lw	a4,4(s0)
2006029a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006029e:	1007f793          	andi	a5,a5,256
200602a2:	c3b9                	c.beqz	a5,200602e8 <inic_api_host_task_h+0x31c>
200602a4:	01f77693          	andi	a3,a4,31
200602a8:	c2c9                	c.beqz	a3,2006032a <inic_api_host_task_h+0x35e>
200602aa:	01068613          	addi	a2,a3,16
200602ae:	fe077793          	andi	a5,a4,-32
200602b2:	01f7f693          	andi	a3,a5,31
200602b6:	96b2                	c.add	a3,a2
200602b8:	0ff0000f          	fence	iorw,iorw
200602bc:	96be                	c.add	a3,a5
200602be:	00f7f00b          	cache	dinv,(a5)
200602c2:	02078793          	addi	a5,a5,32
200602c6:	40f68633          	sub	a2,a3,a5
200602ca:	fec04ae3          	blt	zero,a2,200602be <inic_api_host_task_h+0x2f2>
200602ce:	0ff0000f          	fence	iorw,iorw
200602d2:	0001                	c.addi	zero,0
200602d4:	0001                	c.addi	zero,0
200602d6:	0001                	c.addi	zero,0
200602d8:	0001                	c.addi	zero,0
200602da:	0001                	c.addi	zero,0
200602dc:	0001                	c.addi	zero,0
200602de:	0001                	c.addi	zero,0
200602e0:	0001                	c.addi	zero,0
200602e2:	0001                	c.addi	zero,0
200602e4:	0001                	c.addi	zero,0
200602e6:	0001                	c.addi	zero,0
200602e8:	471c                	c.lw	a5,8(a4)
200602ea:	4754                	c.lw	a3,12(a4)
200602ec:	7c102773          	csrrs	a4,0x7c1,zero
200602f0:	10077713          	andi	a4,a4,256
200602f4:	c70d                	c.beqz	a4,2006031e <inic_api_host_task_h+0x352>
200602f6:	577d                	c.li	a4,-1
200602f8:	02e79c63          	bne	a5,a4,20060330 <inic_api_host_task_h+0x364>
200602fc:	02f69a63          	bne	a3,a5,20060330 <inic_api_host_task_h+0x364>
20060300:	7c0167f3          	csrrsi	a5,0x7c0,2
20060304:	0ff0000f          	fence	iorw,iorw
20060308:	0001                	c.addi	zero,0
2006030a:	0001                	c.addi	zero,0
2006030c:	0001                	c.addi	zero,0
2006030e:	0001                	c.addi	zero,0
20060310:	0001                	c.addi	zero,0
20060312:	0001                	c.addi	zero,0
20060314:	0001                	c.addi	zero,0
20060316:	0001                	c.addi	zero,0
20060318:	0001                	c.addi	zero,0
2006031a:	0001                	c.addi	zero,0
2006031c:	0001                	c.addi	zero,0
2006031e:	fc81a783          	lw	a5,-56(gp) # 200660a0 <promisc_user_callback_ptr>
20060322:	4048                	c.lw	a0,4(s0)
20060324:	9782                	c.jalr	a5
20060326:	d008                	c.sw	a0,32(s0)
20060328:	bb65                	c.j	200600e0 <inic_api_host_task_h+0x114>
2006032a:	87ba                	c.mv	a5,a4
2006032c:	4641                	c.li	a2,16
2006032e:	b751                	c.j	200602b2 <inic_api_host_task_h+0x2e6>
20060330:	01f7f713          	andi	a4,a5,31
20060334:	c319                	c.beqz	a4,2006033a <inic_api_host_task_h+0x36e>
20060336:	9b81                	c.andi	a5,-32
20060338:	96ba                	c.add	a3,a4
2006033a:	fed052e3          	bge	zero,a3,2006031e <inic_api_host_task_h+0x352>
2006033e:	01f7f713          	andi	a4,a5,31
20060342:	9736                	c.add	a4,a3
20060344:	0ff0000f          	fence	iorw,iorw
20060348:	973e                	c.add	a4,a5
2006034a:	00f7f00b          	cache	dinv,(a5)
2006034e:	02078793          	addi	a5,a5,32
20060352:	40f706b3          	sub	a3,a4,a5
20060356:	fed04ae3          	blt	zero,a3,2006034a <inic_api_host_task_h+0x37e>
2006035a:	b76d                	c.j	20060304 <inic_api_host_task_h+0x338>
2006035c:	feca0513          	addi	a0,s4,-20 # 20063fec <register_string+0x98>
20060360:	ceffe0ef          	jal	ra,2005f04e <DiagPrintf_minimal>
20060364:	bbb5                	c.j	200600e0 <inic_api_host_task_h+0x114>

20060366 <inic_api_host_int_hdl>:
20060366:	fe01a503          	lw	a0,-32(gp) # 200660b8 <g_host_inic_api_task_wake_sema>
2006036a:	d50fb06f          	jal	zero,2005b8ba <rtos_sema_give>

2006036e <inic_api_host_message_send>:
2006036e:	7139                	c.addi16sp	sp,-64
20060370:	d452                	c.swsp	s4,40(sp)
20060372:	d05a                	c.swsp	s6,32(sp)
20060374:	8b2a                	c.mv	s6,a0
20060376:	fdc1a503          	lw	a0,-36(gp) # 200660b4 <g_host_inic_api_message_send_sema>
2006037a:	ce5e                	c.swsp	s7,28(sp)
2006037c:	20068bb7          	lui	s7,0x20068
20060380:	da26                	c.swsp	s1,52(sp)
20060382:	d64e                	c.swsp	s3,44(sp)
20060384:	ec0b8493          	addi	s1,s7,-320 # 20067ec0 <g_host_ipc_api_request_info>
20060388:	89ae                	c.mv	s3,a1
2006038a:	55fd                	c.li	a1,-1
2006038c:	d256                	c.swsp	s5,36(sp)
2006038e:	ca66                	c.swsp	s9,20(sp)
20060390:	c86a                	c.swsp	s10,16(sp)
20060392:	c66e                	c.swsp	s11,12(sp)
20060394:	de06                	c.swsp	ra,60(sp)
20060396:	dc22                	c.swsp	s0,56(sp)
20060398:	d84a                	c.swsp	s2,48(sp)
2006039a:	cc62                	c.swsp	s8,24(sp)
2006039c:	8ab2                	c.mv	s5,a2
2006039e:	fdc18a13          	addi	s4,gp,-36 # 200660b4 <g_host_inic_api_message_send_sema>
200603a2:	cccfb0ef          	jal	ra,2005b86e <rtos_sema_take>
200603a6:	02048d13          	addi	s10,s1,32
200603aa:	3e800d93          	addi	s11,zero,1000
200603ae:	20064cb7          	lui	s9,0x20064
200603b2:	3e800c13          	addi	s8,zero,1000
200603b6:	4401                	c.li	s0,0
200603b8:	7c1027f3          	csrrs	a5,0x7c1,zero
200603bc:	1007f793          	andi	a5,a5,256
200603c0:	ec0b8913          	addi	s2,s7,-320
200603c4:	c785                	c.beqz	a5,200603ec <inic_api_host_message_send+0x7e>
200603c6:	0ff0000f          	fence	iorw,iorw
200603ca:	0094f00b          	cache	dinv,(s1)
200603ce:	01ad700b          	cache	dinv,(s10)
200603d2:	0ff0000f          	fence	iorw,iorw
200603d6:	0001                	c.addi	zero,0
200603d8:	0001                	c.addi	zero,0
200603da:	0001                	c.addi	zero,0
200603dc:	0001                	c.addi	zero,0
200603de:	0001                	c.addi	zero,0
200603e0:	0001                	c.addi	zero,0
200603e2:	0001                	c.addi	zero,0
200603e4:	0001                	c.addi	zero,0
200603e6:	0001                	c.addi	zero,0
200603e8:	0001                	c.addi	zero,0
200603ea:	0001                	c.addi	zero,0
200603ec:	00092783          	lw	a5,0(s2)
200603f0:	cf99                	c.beqz	a5,2006040e <inic_api_host_message_send+0xa0>
200603f2:	4505                	c.li	a0,1
200603f4:	0405                	c.addi	s0,1
200603f6:	1c7d                	c.addi	s8,-1
200603f8:	efefb0ef          	jal	ra,2005baf6 <rtos_time_delay_ms>
200603fc:	03b46433          	rem	s0,s0,s11
20060400:	fa0c1ce3          	bne	s8,zero,200603b8 <inic_api_host_message_send+0x4a>
20060404:	02cc8513          	addi	a0,s9,44 # 2006402c <register_string+0xd8>
20060408:	c47fe0ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2006040c:	b75d                	c.j	200603b2 <inic_api_host_message_send+0x44>
2006040e:	04000613          	addi	a2,zero,64
20060412:	4581                	c.li	a1,0
20060414:	ec0b8513          	addi	a0,s7,-320
20060418:	dffa9097          	auipc	ra,0xdffa9
2006041c:	6b6080e7          	jalr	ra,1718(ra) # 9ace <__wrap_memset>
20060420:	01692023          	sw	s6,0(s2)
20060424:	00098b63          	beq	s3,zero,2006043a <inic_api_host_message_send+0xcc>
20060428:	002a9613          	slli	a2,s5,0x2
2006042c:	85ce                	c.mv	a1,s3
2006042e:	00490513          	addi	a0,s2,4
20060432:	dffa9097          	auipc	ra,0xdffa9
20060436:	69e080e7          	jalr	ra,1694(ra) # 9ad0 <__wrap_memcpy>
2006043a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006043e:	1007f793          	andi	a5,a5,256
20060442:	c795                	c.beqz	a5,2006046e <inic_api_host_message_send+0x100>
20060444:	0ff0000f          	fence	iorw,iorw
20060448:	0494f00b          	cache	dwb,(s1)
2006044c:	02048793          	addi	a5,s1,32
20060450:	04f7f00b          	cache	dwb,(a5)
20060454:	0ff0000f          	fence	iorw,iorw
20060458:	0001                	c.addi	zero,0
2006045a:	0001                	c.addi	zero,0
2006045c:	0001                	c.addi	zero,0
2006045e:	0001                	c.addi	zero,0
20060460:	0001                	c.addi	zero,0
20060462:	0001                	c.addi	zero,0
20060464:	0001                	c.addi	zero,0
20060466:	0001                	c.addi	zero,0
20060468:	0001                	c.addi	zero,0
2006046a:	0001                	c.addi	zero,0
2006046c:	0001                	c.addi	zero,0
2006046e:	200689b7          	lui	s3,0x20068
20060472:	4641                	c.li	a2,16
20060474:	4581                	c.li	a1,0
20060476:	e8098513          	addi	a0,s3,-384 # 20067e80 <g_host_ipc_api_msg>
2006047a:	dffa9097          	auipc	ra,0xdffa9
2006047e:	654080e7          	jalr	ra,1620(ra) # 9ace <__wrap_memset>
20060482:	e8098a93          	addi	s5,s3,-384
20060486:	04000793          	addi	a5,zero,64
2006048a:	009aa223          	sw	s1,4(s5)
2006048e:	000aa023          	sw	zero,0(s5)
20060492:	00faa423          	sw	a5,8(s5)
20060496:	7c1027f3          	csrrs	a5,0x7c1,zero
2006049a:	1007f793          	andi	a5,a5,256
2006049e:	c395                	c.beqz	a5,200604c2 <inic_api_host_message_send+0x154>
200604a0:	0ff0000f          	fence	iorw,iorw
200604a4:	055af00b          	cache	dwb,(s5)
200604a8:	0ff0000f          	fence	iorw,iorw
200604ac:	0001                	c.addi	zero,0
200604ae:	0001                	c.addi	zero,0
200604b0:	0001                	c.addi	zero,0
200604b2:	0001                	c.addi	zero,0
200604b4:	0001                	c.addi	zero,0
200604b6:	0001                	c.addi	zero,0
200604b8:	0001                	c.addi	zero,0
200604ba:	0001                	c.addi	zero,0
200604bc:	0001                	c.addi	zero,0
200604be:	0001                	c.addi	zero,0
200604c0:	0001                	c.addi	zero,0
200604c2:	e8098613          	addi	a2,s3,-384
200604c6:	459d                	c.li	a1,7
200604c8:	4501                	c.li	a0,0
200604ca:	adff80ef          	jal	ra,20058fa8 <ipc_send_message>
200604ce:	02048b13          	addi	s6,s1,32
200604d2:	3e800b93          	addi	s7,zero,1000
200604d6:	20064ab7          	lui	s5,0x20064
200604da:	200649b7          	lui	s3,0x20064
200604de:	00092783          	lw	a5,0(s2)
200604e2:	c7b9                	c.beqz	a5,20060530 <inic_api_host_message_send+0x1c2>
200604e4:	4505                	c.li	a0,1
200604e6:	e10fb0ef          	jal	ra,2005baf6 <rtos_time_delay_ms>
200604ea:	7c1027f3          	csrrs	a5,0x7c1,zero
200604ee:	1007f793          	andi	a5,a5,256
200604f2:	c785                	c.beqz	a5,2006051a <inic_api_host_message_send+0x1ac>
200604f4:	0ff0000f          	fence	iorw,iorw
200604f8:	0094f00b          	cache	dinv,(s1)
200604fc:	016b700b          	cache	dinv,(s6)
20060500:	0ff0000f          	fence	iorw,iorw
20060504:	0001                	c.addi	zero,0
20060506:	0001                	c.addi	zero,0
20060508:	0001                	c.addi	zero,0
2006050a:	0001                	c.addi	zero,0
2006050c:	0001                	c.addi	zero,0
2006050e:	0001                	c.addi	zero,0
20060510:	0001                	c.addi	zero,0
20060512:	0001                	c.addi	zero,0
20060514:	0001                	c.addi	zero,0
20060516:	0001                	c.addi	zero,0
20060518:	0001                	c.addi	zero,0
2006051a:	0405                	c.addi	s0,1
2006051c:	03746433          	rem	s0,s0,s7
20060520:	fc5d                	c.bnez	s0,200604de <inic_api_host_message_send+0x170>
20060522:	0cca8593          	addi	a1,s5,204 # 200640cc <__func__.0>
20060526:	04498513          	addi	a0,s3,68 # 20064044 <register_string+0xf0>
2006052a:	b25fe0ef          	jal	ra,2005f04e <DiagPrintf_minimal>
2006052e:	bf45                	c.j	200604de <inic_api_host_message_send+0x170>
20060530:	000a2503          	lw	a0,0(s4)
20060534:	02892403          	lw	s0,40(s2)
20060538:	b82fb0ef          	jal	ra,2005b8ba <rtos_sema_give>
2006053c:	50f2                	c.lwsp	ra,60(sp)
2006053e:	8522                	c.mv	a0,s0
20060540:	5462                	c.lwsp	s0,56(sp)
20060542:	54d2                	c.lwsp	s1,52(sp)
20060544:	5942                	c.lwsp	s2,48(sp)
20060546:	59b2                	c.lwsp	s3,44(sp)
20060548:	5a22                	c.lwsp	s4,40(sp)
2006054a:	5a92                	c.lwsp	s5,36(sp)
2006054c:	5b02                	c.lwsp	s6,32(sp)
2006054e:	4bf2                	c.lwsp	s7,28(sp)
20060550:	4c62                	c.lwsp	s8,24(sp)
20060552:	4cd2                	c.lwsp	s9,20(sp)
20060554:	4d42                	c.lwsp	s10,16(sp)
20060556:	4db2                	c.lwsp	s11,12(sp)
20060558:	6121                	c.addi16sp	sp,64
2006055a:	8082                	c.jr	ra

2006055c <inic_api_init_host>:
2006055c:	1141                	c.addi	sp,-16
2006055e:	567d                	c.li	a2,-1
20060560:	4581                	c.li	a1,0
20060562:	fe018513          	addi	a0,gp,-32 # 200660b8 <g_host_inic_api_task_wake_sema>
20060566:	c606                	c.swsp	ra,12(sp)
20060568:	c422                	c.swsp	s0,8(sp)
2006056a:	a86fb0ef          	jal	ra,2005b7f0 <rtos_sema_create_static>
2006056e:	567d                	c.li	a2,-1
20060570:	4581                	c.li	a1,0
20060572:	fdc18413          	addi	s0,gp,-36 # 200660b4 <g_host_inic_api_message_send_sema>
20060576:	fdc18513          	addi	a0,gp,-36 # 200660b4 <g_host_inic_api_message_send_sema>
2006057a:	a76fb0ef          	jal	ra,2005b7f0 <rtos_sema_create_static>
2006057e:	4008                	c.lw	a0,0(s0)
20060580:	b3afb0ef          	jal	ra,2005b8ba <rtos_sema_give>
20060584:	200605b7          	lui	a1,0x20060
20060588:	4701                	c.li	a4,0
2006058a:	4681                	c.li	a3,0
2006058c:	4601                	c.li	a2,0
2006058e:	fc858593          	addi	a1,a1,-56 # 2005ffc8 <_inic_ipc_ip_addr_update_in_wowlan>
20060592:	4505                	c.li	a0,1
20060594:	986f90ef          	jal	ra,2005971a <pmu_register_sleep_callback>
20060598:	6705                	c.lui	a4,0x1
2006059a:	20060637          	lui	a2,0x20060
2006059e:	200645b7          	lui	a1,0x20064
200605a2:	478d                	c.li	a5,3
200605a4:	ce070713          	addi	a4,a4,-800 # ce0 <CPU_ClkGet+0x1ca>
200605a8:	4681                	c.li	a3,0
200605aa:	fcc60613          	addi	a2,a2,-52 # 2005ffcc <inic_api_host_task_h>
200605ae:	06458593          	addi	a1,a1,100 # 20064064 <register_string+0x110>
200605b2:	4501                	c.li	a0,0
200605b4:	d0cfb0ef          	jal	ra,2005bac0 <rtos_task_create>
200605b8:	c10d                	c.beqz	a0,200605da <inic_api_init_host+0x7e>
200605ba:	4422                	c.lwsp	s0,8(sp)
200605bc:	40b2                	c.lwsp	ra,12(sp)
200605be:	200646b7          	lui	a3,0x20064
200605c2:	200645b7          	lui	a1,0x20064
200605c6:	07868693          	addi	a3,a3,120 # 20064078 <register_string+0x124>
200605ca:	04500613          	addi	a2,zero,69
200605ce:	09458593          	addi	a1,a1,148 # 20064094 <register_string+0x140>
200605d2:	4509                	c.li	a0,2
200605d4:	0141                	c.addi	sp,16
200605d6:	89bfa06f          	jal	zero,2005ae70 <rtk_log_write>
200605da:	40b2                	c.lwsp	ra,12(sp)
200605dc:	4422                	c.lwsp	s0,8(sp)
200605de:	0141                	c.addi	sp,16
200605e0:	8082                	c.jr	ra

200605e2 <inic_mp_command>:
200605e2:	7179                	c.addi16sp	sp,-48
200605e4:	ce4e                	c.swsp	s3,28(sp)
200605e6:	89aa                	c.mv	s3,a0
200605e8:	6505                	c.lui	a0,0x1
200605ea:	d04a                	c.swsp	s2,32(sp)
200605ec:	cc52                	c.swsp	s4,24(sp)
200605ee:	d606                	c.swsp	ra,44(sp)
200605f0:	d422                	c.swsp	s0,40(sp)
200605f2:	d226                	c.swsp	s1,36(sp)
200605f4:	892e                	c.mv	s2,a1
200605f6:	8a32                	c.mv	s4,a2
200605f8:	94cfb0ef          	jal	ra,2005b744 <rtos_mem_zmalloc>
200605fc:	e50d                	c.bnez	a0,20060626 <inic_mp_command+0x44>
200605fe:	5422                	c.lwsp	s0,40(sp)
20060600:	50b2                	c.lwsp	ra,44(sp)
20060602:	5492                	c.lwsp	s1,36(sp)
20060604:	5902                	c.lwsp	s2,32(sp)
20060606:	49f2                	c.lwsp	s3,28(sp)
20060608:	4a62                	c.lwsp	s4,24(sp)
2006060a:	200646b7          	lui	a3,0x20064
2006060e:	200645b7          	lui	a1,0x20064
20060612:	09c68693          	addi	a3,a3,156 # 2006409c <register_string+0x148>
20060616:	04500613          	addi	a2,zero,69
2006061a:	09458593          	addi	a1,a1,148 # 20064094 <register_string+0x140>
2006061e:	4509                	c.li	a0,2
20060620:	6145                	c.addi16sp	sp,48
20060622:	84ffa06f          	jal	zero,2005ae70 <rtk_log_write>
20060626:	842a                	c.mv	s0,a0
20060628:	87ce                	c.mv	a5,s3
2006062a:	c04e                	c.swsp	s3,0(sp)
2006062c:	c24a                	c.swsp	s2,4(sp)
2006062e:	c452                	c.swsp	s4,8(sp)
20060630:	84aa                	c.mv	s1,a0
20060632:	c62a                	c.swsp	a0,12(sp)
20060634:	7c102773          	csrrs	a4,0x7c1,zero
20060638:	10077713          	andi	a4,a4,256
2006063c:	c721                	c.beqz	a4,20060684 <inic_mp_command+0xa2>
2006063e:	01f57693          	andi	a3,a0,31
20060642:	10068363          	beq	a3,zero,20060748 <inic_mp_command+0x166>
20060646:	6605                	c.lui	a2,0x1
20060648:	9636                	c.add	a2,a3
2006064a:	fe057713          	andi	a4,a0,-32
2006064e:	01f77693          	andi	a3,a4,31
20060652:	96b2                	c.add	a3,a2
20060654:	0ff0000f          	fence	iorw,iorw
20060658:	96ba                	c.add	a3,a4
2006065a:	04e7700b          	cache	dwb,(a4)
2006065e:	02070713          	addi	a4,a4,32
20060662:	40e68633          	sub	a2,a3,a4
20060666:	fec04ae3          	blt	zero,a2,2006065a <inic_mp_command+0x78>
2006066a:	0ff0000f          	fence	iorw,iorw
2006066e:	0001                	c.addi	zero,0
20060670:	0001                	c.addi	zero,0
20060672:	0001                	c.addi	zero,0
20060674:	0001                	c.addi	zero,0
20060676:	0001                	c.addi	zero,0
20060678:	0001                	c.addi	zero,0
2006067a:	0001                	c.addi	zero,0
2006067c:	0001                	c.addi	zero,0
2006067e:	0001                	c.addi	zero,0
20060680:	0001                	c.addi	zero,0
20060682:	0001                	c.addi	zero,0
20060684:	7c102773          	csrrs	a4,0x7c1,zero
20060688:	10077713          	andi	a4,a4,256
2006068c:	c70d                	c.beqz	a4,200606b6 <inic_mp_command+0xd4>
2006068e:	577d                	c.li	a4,-1
20060690:	0ae99f63          	bne	s3,a4,2006074e <inic_mp_command+0x16c>
20060694:	0b391d63          	bne	s2,s3,2006074e <inic_mp_command+0x16c>
20060698:	7c0267f3          	csrrsi	a5,0x7c0,4
2006069c:	0ff0000f          	fence	iorw,iorw
200606a0:	0001                	c.addi	zero,0
200606a2:	0001                	c.addi	zero,0
200606a4:	0001                	c.addi	zero,0
200606a6:	0001                	c.addi	zero,0
200606a8:	0001                	c.addi	zero,0
200606aa:	0001                	c.addi	zero,0
200606ac:	0001                	c.addi	zero,0
200606ae:	0001                	c.addi	zero,0
200606b0:	0001                	c.addi	zero,0
200606b2:	0001                	c.addi	zero,0
200606b4:	0001                	c.addi	zero,0
200606b6:	4611                	c.li	a2,4
200606b8:	858a                	c.mv	a1,sp
200606ba:	04e00513          	addi	a0,zero,78
200606be:	cb1ff0ef          	jal	ra,2006036e <inic_api_host_message_send>
200606c2:	060a0863          	beq	s4,zero,20060732 <inic_mp_command+0x150>
200606c6:	7c1027f3          	csrrs	a5,0x7c1,zero
200606ca:	1007f793          	andi	a5,a5,256
200606ce:	c7a1                	c.beqz	a5,20060716 <inic_mp_command+0x134>
200606d0:	01f47713          	andi	a4,s0,31
200606d4:	6785                	c.lui	a5,0x1
200606d6:	c701                	c.beqz	a4,200606de <inic_mp_command+0xfc>
200606d8:	97ba                	c.add	a5,a4
200606da:	fe047493          	andi	s1,s0,-32
200606de:	01f4f713          	andi	a4,s1,31
200606e2:	973e                	c.add	a4,a5
200606e4:	0ff0000f          	fence	iorw,iorw
200606e8:	87a6                	c.mv	a5,s1
200606ea:	9726                	c.add	a4,s1
200606ec:	00f7f00b          	cache	dinv,(a5)
200606f0:	02078793          	addi	a5,a5,32 # 1020 <CPU_ClkGet+0x50a>
200606f4:	40f706b3          	sub	a3,a4,a5
200606f8:	fed04ae3          	blt	zero,a3,200606ec <inic_mp_command+0x10a>
200606fc:	0ff0000f          	fence	iorw,iorw
20060700:	0001                	c.addi	zero,0
20060702:	0001                	c.addi	zero,0
20060704:	0001                	c.addi	zero,0
20060706:	0001                	c.addi	zero,0
20060708:	0001                	c.addi	zero,0
2006070a:	0001                	c.addi	zero,0
2006070c:	0001                	c.addi	zero,0
2006070e:	0001                	c.addi	zero,0
20060710:	0001                	c.addi	zero,0
20060712:	0001                	c.addi	zero,0
20060714:	0001                	c.addi	zero,0
20060716:	200646b7          	lui	a3,0x20064
2006071a:	200645b7          	lui	a1,0x20064
2006071e:	8722                	c.mv	a4,s0
20060720:	0b468693          	addi	a3,a3,180 # 200640b4 <register_string+0x160>
20060724:	04100613          	addi	a2,zero,65
20060728:	09458593          	addi	a1,a1,148 # 20064094 <register_string+0x140>
2006072c:	4505                	c.li	a0,1
2006072e:	f42fa0ef          	jal	ra,2005ae70 <rtk_log_write>
20060732:	8522                	c.mv	a0,s0
20060734:	836fb0ef          	jal	ra,2005b76a <rtos_mem_free>
20060738:	50b2                	c.lwsp	ra,44(sp)
2006073a:	5422                	c.lwsp	s0,40(sp)
2006073c:	5492                	c.lwsp	s1,36(sp)
2006073e:	5902                	c.lwsp	s2,32(sp)
20060740:	49f2                	c.lwsp	s3,28(sp)
20060742:	4a62                	c.lwsp	s4,24(sp)
20060744:	6145                	c.addi16sp	sp,48
20060746:	8082                	c.jr	ra
20060748:	872a                	c.mv	a4,a0
2006074a:	6605                	c.lui	a2,0x1
2006074c:	b709                	c.j	2006064e <inic_mp_command+0x6c>
2006074e:	01f9f713          	andi	a4,s3,31
20060752:	c701                	c.beqz	a4,2006075a <inic_mp_command+0x178>
20060754:	fe09f793          	andi	a5,s3,-32
20060758:	993a                	c.add	s2,a4
2006075a:	f5205ee3          	bge	zero,s2,200606b6 <inic_mp_command+0xd4>
2006075e:	01f7f593          	andi	a1,a5,31
20060762:	992e                	c.add	s2,a1
20060764:	0ff0000f          	fence	iorw,iorw
20060768:	993e                	c.add	s2,a5
2006076a:	04f7f00b          	cache	dwb,(a5)
2006076e:	02078793          	addi	a5,a5,32
20060772:	40f90733          	sub	a4,s2,a5
20060776:	fee04ae3          	blt	zero,a4,2006076a <inic_mp_command+0x188>
2006077a:	b70d                	c.j	2006069c <inic_mp_command+0xba>

2006077c <inic_host_init>:
2006077c:	1141                	c.addi	sp,-16
2006077e:	c606                	c.swsp	ra,12(sp)
20060780:	2269                	c.jal	2006090a <inic_host_init_priv>
20060782:	40b2                	c.lwsp	ra,12(sp)
20060784:	0141                	c.addi	sp,16
20060786:	dd7ff06f          	jal	zero,2006055c <inic_api_init_host>

2006078a <inic_host_init_skb>:
2006078a:	1101                	c.addi	sp,-32
2006078c:	ca26                	c.swsp	s1,20(sp)
2006078e:	fe81a783          	lw	a5,-24(gp) # 200660c0 <host_skb_info>
20060792:	ce06                	c.swsp	ra,28(sp)
20060794:	cc22                	c.swsp	s0,24(sp)
20060796:	c84a                	c.swsp	s2,16(sp)
20060798:	c64e                	c.swsp	s3,12(sp)
2006079a:	e791                	c.bnez	a5,200607a6 <inic_host_init_skb+0x1c>
2006079c:	fe41a783          	lw	a5,-28(gp) # 200660bc <host_skb_data>
200607a0:	fe418413          	addi	s0,gp,-28 # 200660bc <host_skb_data>
200607a4:	c785                	c.beqz	a5,200607cc <inic_host_init_skb+0x42>
200607a6:	4462                	c.lwsp	s0,24(sp)
200607a8:	40f2                	c.lwsp	ra,28(sp)
200607aa:	44d2                	c.lwsp	s1,20(sp)
200607ac:	4942                	c.lwsp	s2,16(sp)
200607ae:	49b2                	c.lwsp	s3,12(sp)
200607b0:	200646b7          	lui	a3,0x20064
200607b4:	200645b7          	lui	a1,0x20064
200607b8:	0e868693          	addi	a3,a3,232 # 200640e8 <__func__.0+0x1c>
200607bc:	04500613          	addi	a2,zero,69
200607c0:	09458593          	addi	a1,a1,148 # 20064094 <register_string+0x140>
200607c4:	4509                	c.li	a0,2
200607c6:	6105                	c.addi16sp	sp,32
200607c8:	ea8fa06f          	jal	zero,2005ae70 <rtk_log_write>
200607cc:	42818993          	addi	s3,gp,1064 # 20066500 <wifi_user_config>
200607d0:	0249a503          	lw	a0,36(s3)
200607d4:	fe818493          	addi	s1,gp,-24 # 200660c0 <host_skb_info>
200607d8:	42818913          	addi	s2,gp,1064 # 20066500 <wifi_user_config>
200607dc:	051a                	c.slli	a0,0x6
200607de:	f67fa0ef          	jal	ra,2005b744 <rtos_mem_zmalloc>
200607e2:	0249a783          	lw	a5,36(s3)
200607e6:	c088                	c.sw	a0,0(s1)
200607e8:	6a000513          	addi	a0,zero,1696
200607ec:	02f50533          	mul	a0,a0,a5
200607f0:	f55fa0ef          	jal	ra,2005b744 <rtos_mem_zmalloc>
200607f4:	409c                	c.lw	a5,0(s1)
200607f6:	c008                	c.sw	a0,0(s0)
200607f8:	c391                	c.beqz	a5,200607fc <inic_host_init_skb+0x72>
200607fa:	e115                	c.bnez	a0,2006081e <inic_host_init_skb+0x94>
200607fc:	20064737          	lui	a4,0x20064
20060800:	200646b7          	lui	a3,0x20064
20060804:	200645b7          	lui	a1,0x20064
20060808:	13470713          	addi	a4,a4,308 # 20064134 <__func__.1>
2006080c:	10068693          	addi	a3,a3,256 # 20064100 <__func__.0+0x34>
20060810:	04500613          	addi	a2,zero,69
20060814:	09458593          	addi	a1,a1,148 # 20064094 <register_string+0x140>
20060818:	4509                	c.li	a0,2
2006081a:	e56fa0ef          	jal	ra,2005ae70 <rtk_log_write>
2006081e:	4681                	c.li	a3,0
20060820:	6a000593          	addi	a1,zero,1696
20060824:	02492703          	lw	a4,36(s2)
20060828:	409c                	c.lw	a5,0(s1)
2006082a:	0ae6cf63          	blt	a3,a4,200608e8 <inic_host_init_skb+0x15e>
2006082e:	7c1026f3          	csrrs	a3,0x7c1,zero
20060832:	1006f693          	andi	a3,a3,256
20060836:	c6a1                	c.beqz	a3,2006087e <inic_host_init_skb+0xf4>
20060838:	01f7f693          	andi	a3,a5,31
2006083c:	071a                	c.slli	a4,0x6
2006083e:	c299                	c.beqz	a3,20060844 <inic_host_init_skb+0xba>
20060840:	9b81                	c.andi	a5,-32
20060842:	9736                	c.add	a4,a3
20060844:	02e05d63          	bge	zero,a4,2006087e <inic_host_init_skb+0xf4>
20060848:	01f7f693          	andi	a3,a5,31
2006084c:	9736                	c.add	a4,a3
2006084e:	0ff0000f          	fence	iorw,iorw
20060852:	973e                	c.add	a4,a5
20060854:	04f7f00b          	cache	dwb,(a5)
20060858:	02078793          	addi	a5,a5,32
2006085c:	40f706b3          	sub	a3,a4,a5
20060860:	fed04ae3          	blt	zero,a3,20060854 <inic_host_init_skb+0xca>
20060864:	0ff0000f          	fence	iorw,iorw
20060868:	0001                	c.addi	zero,0
2006086a:	0001                	c.addi	zero,0
2006086c:	0001                	c.addi	zero,0
2006086e:	0001                	c.addi	zero,0
20060870:	0001                	c.addi	zero,0
20060872:	0001                	c.addi	zero,0
20060874:	0001                	c.addi	zero,0
20060876:	0001                	c.addi	zero,0
20060878:	0001                	c.addi	zero,0
2006087a:	0001                	c.addi	zero,0
2006087c:	0001                	c.addi	zero,0
2006087e:	401c                	c.lw	a5,0(s0)
20060880:	02492703          	lw	a4,36(s2)
20060884:	7c1026f3          	csrrs	a3,0x7c1,zero
20060888:	1006f693          	andi	a3,a3,256
2006088c:	c6b9                	c.beqz	a3,200608da <inic_host_init_skb+0x150>
2006088e:	6a000693          	addi	a3,zero,1696
20060892:	02d70733          	mul	a4,a4,a3
20060896:	01f7f693          	andi	a3,a5,31
2006089a:	c299                	c.beqz	a3,200608a0 <inic_host_init_skb+0x116>
2006089c:	9b81                	c.andi	a5,-32
2006089e:	9736                	c.add	a4,a3
200608a0:	02e05d63          	bge	zero,a4,200608da <inic_host_init_skb+0x150>
200608a4:	01f7f693          	andi	a3,a5,31
200608a8:	9736                	c.add	a4,a3
200608aa:	0ff0000f          	fence	iorw,iorw
200608ae:	973e                	c.add	a4,a5
200608b0:	04f7f00b          	cache	dwb,(a5)
200608b4:	02078793          	addi	a5,a5,32
200608b8:	40f706b3          	sub	a3,a4,a5
200608bc:	fed04ae3          	blt	zero,a3,200608b0 <inic_host_init_skb+0x126>
200608c0:	0ff0000f          	fence	iorw,iorw
200608c4:	0001                	c.addi	zero,0
200608c6:	0001                	c.addi	zero,0
200608c8:	0001                	c.addi	zero,0
200608ca:	0001                	c.addi	zero,0
200608cc:	0001                	c.addi	zero,0
200608ce:	0001                	c.addi	zero,0
200608d0:	0001                	c.addi	zero,0
200608d2:	0001                	c.addi	zero,0
200608d4:	0001                	c.addi	zero,0
200608d6:	0001                	c.addi	zero,0
200608d8:	0001                	c.addi	zero,0
200608da:	40f2                	c.lwsp	ra,28(sp)
200608dc:	4462                	c.lwsp	s0,24(sp)
200608de:	44d2                	c.lwsp	s1,20(sp)
200608e0:	4942                	c.lwsp	s2,16(sp)
200608e2:	49b2                	c.lwsp	s3,12(sp)
200608e4:	6105                	c.addi16sp	sp,32
200608e6:	8082                	c.jr	ra
200608e8:	02b68633          	mul	a2,a3,a1
200608ec:	00669713          	slli	a4,a3,0x6
200608f0:	97ba                	c.add	a5,a4
200608f2:	c39c                	c.sw	a5,0(a5)
200608f4:	409c                	c.lw	a5,0(s1)
200608f6:	0685                	c.addi	a3,1
200608f8:	97ba                	c.add	a5,a4
200608fa:	4018                	c.lw	a4,0(s0)
200608fc:	c3dc                	c.sw	a5,4(a5)
200608fe:	9732                	c.add	a4,a2
20060900:	c318                	c.sw	a4,0(a4)
20060902:	401c                	c.lw	a5,0(s0)
20060904:	97b2                	c.add	a5,a2
20060906:	c3dc                	c.sw	a5,4(a5)
20060908:	bf31                	c.j	20060824 <inic_host_init_skb+0x9a>

2006090a <inic_host_init_priv>:
2006090a:	1141                	c.addi	sp,-16
2006090c:	c226                	c.swsp	s1,4(sp)
2006090e:	200684b7          	lui	s1,0x20068
20060912:	02800613          	addi	a2,zero,40
20060916:	4581                	c.li	a1,0
20060918:	f0048513          	addi	a0,s1,-256 # 20067f00 <g_inic_host_priv>
2006091c:	c606                	c.swsp	ra,12(sp)
2006091e:	c422                	c.swsp	s0,8(sp)
20060920:	dffa9097          	auipc	ra,0xdffa9
20060924:	1ae080e7          	jalr	ra,430(ra) # 9ace <__wrap_memset>
20060928:	567d                	c.li	a2,-1
2006092a:	4581                	c.li	a1,0
2006092c:	f0048513          	addi	a0,s1,-256
20060930:	f0048413          	addi	s0,s1,-256
20060934:	ebdfa0ef          	jal	ra,2005b7f0 <rtos_sema_create_static>
20060938:	567d                	c.li	a2,-1
2006093a:	4581                	c.li	a1,0
2006093c:	00440513          	addi	a0,s0,4
20060940:	eb1fa0ef          	jal	ra,2005b7f0 <rtos_sema_create_static>
20060944:	567d                	c.li	a2,-1
20060946:	4581                	c.li	a1,0
20060948:	00840513          	addi	a0,s0,8
2006094c:	ea5fa0ef          	jal	ra,2005b7f0 <rtos_sema_create_static>
20060950:	4408                	c.lw	a0,8(s0)
20060952:	f69fa0ef          	jal	ra,2005b8ba <rtos_sema_give>
20060956:	00c40513          	addi	a0,s0,12
2006095a:	26b5                	c.jal	20060cc6 <rtw_init_queue>
2006095c:	40b2                	c.lwsp	ra,12(sp)
2006095e:	00042c23          	sw	zero,24(s0)
20060962:	00042e23          	sw	zero,28(s0)
20060966:	02042023          	sw	zero,32(s0)
2006096a:	02042223          	sw	zero,36(s0)
2006096e:	4422                	c.lwsp	s0,8(sp)
20060970:	4492                	c.lwsp	s1,4(sp)
20060972:	0141                	c.addi	sp,16
20060974:	8082                	c.jr	ra

20060976 <inic_host_trx_event_hdl>:
20060976:	4791                	c.li	a5,4
20060978:	872a                	c.mv	a4,a0
2006097a:	00f50f63          	beq	a0,a5,20060998 <inic_host_trx_event_hdl+0x22>
2006097e:	200646b7          	lui	a3,0x20064
20060982:	200645b7          	lui	a1,0x20064
20060986:	11868693          	addi	a3,a3,280 # 20064118 <__func__.0+0x4c>
2006098a:	04500613          	addi	a2,zero,69
2006098e:	09458593          	addi	a1,a1,148 # 20064094 <register_string+0x140>
20060992:	4509                	c.li	a0,2
20060994:	cdcfa06f          	jal	zero,2005ae70 <rtk_log_write>
20060998:	8082                	c.jr	ra

2006099a <inic_host_trx_int_hdl>:
2006099a:	1141                	c.addi	sp,-16
2006099c:	4599                	c.li	a1,6
2006099e:	4541                	c.li	a0,16
200609a0:	c422                	c.swsp	s0,8(sp)
200609a2:	c606                	c.swsp	ra,12(sp)
200609a4:	f4cf80ef          	jal	ra,200590f0 <ipc_get_message>
200609a8:	4140                	c.lw	s0,4(a0)
200609aa:	7c1027f3          	csrrs	a5,0x7c1,zero
200609ae:	1007f793          	andi	a5,a5,256
200609b2:	c3b9                	c.beqz	a5,200609f8 <inic_host_trx_int_hdl+0x5e>
200609b4:	01f47713          	andi	a4,s0,31
200609b8:	c755                	c.beqz	a4,20060a64 <inic_host_trx_int_hdl+0xca>
200609ba:	04070693          	addi	a3,a4,64
200609be:	fe047793          	andi	a5,s0,-32
200609c2:	01f7f713          	andi	a4,a5,31
200609c6:	9736                	c.add	a4,a3
200609c8:	0ff0000f          	fence	iorw,iorw
200609cc:	973e                	c.add	a4,a5
200609ce:	00f7f00b          	cache	dinv,(a5)
200609d2:	02078793          	addi	a5,a5,32
200609d6:	40f706b3          	sub	a3,a4,a5
200609da:	fed04ae3          	blt	zero,a3,200609ce <inic_host_trx_int_hdl+0x34>
200609de:	0ff0000f          	fence	iorw,iorw
200609e2:	0001                	c.addi	zero,0
200609e4:	0001                	c.addi	zero,0
200609e6:	0001                	c.addi	zero,0
200609e8:	0001                	c.addi	zero,0
200609ea:	0001                	c.addi	zero,0
200609ec:	0001                	c.addi	zero,0
200609ee:	0001                	c.addi	zero,0
200609f0:	0001                	c.addi	zero,0
200609f2:	0001                	c.addi	zero,0
200609f4:	0001                	c.addi	zero,0
200609f6:	0001                	c.addi	zero,0
200609f8:	2455                	c.jal	20060c9c <inic_msg_get_queue_status>
200609fa:	c92d                	c.beqz	a0,20060a6c <inic_host_trx_int_hdl+0xd2>
200609fc:	8522                	c.mv	a0,s0
200609fe:	2af1                	c.jal	20060bda <inic_msg_enqueue>
20060a00:	157d                	c.addi	a0,-1
20060a02:	00a03533          	sltu	a0,zero,a0
20060a06:	c408                	c.sw	a0,8(s0)
20060a08:	00042023          	sw	zero,0(s0)
20060a0c:	7c1027f3          	csrrs	a5,0x7c1,zero
20060a10:	1007f793          	andi	a5,a5,256
20060a14:	c7a1                	c.beqz	a5,20060a5c <inic_host_trx_int_hdl+0xc2>
20060a16:	01f47793          	andi	a5,s0,31
20060a1a:	04000713          	addi	a4,zero,64
20060a1e:	c781                	c.beqz	a5,20060a26 <inic_host_trx_int_hdl+0x8c>
20060a20:	04078713          	addi	a4,a5,64
20060a24:	9801                	c.andi	s0,-32
20060a26:	01f47793          	andi	a5,s0,31
20060a2a:	97ba                	c.add	a5,a4
20060a2c:	0ff0000f          	fence	iorw,iorw
20060a30:	97a2                	c.add	a5,s0
20060a32:	0484700b          	cache	dwb,(s0)
20060a36:	02040413          	addi	s0,s0,32
20060a3a:	40878733          	sub	a4,a5,s0
20060a3e:	fee04ae3          	blt	zero,a4,20060a32 <inic_host_trx_int_hdl+0x98>
20060a42:	0ff0000f          	fence	iorw,iorw
20060a46:	0001                	c.addi	zero,0
20060a48:	0001                	c.addi	zero,0
20060a4a:	0001                	c.addi	zero,0
20060a4c:	0001                	c.addi	zero,0
20060a4e:	0001                	c.addi	zero,0
20060a50:	0001                	c.addi	zero,0
20060a52:	0001                	c.addi	zero,0
20060a54:	0001                	c.addi	zero,0
20060a56:	0001                	c.addi	zero,0
20060a58:	0001                	c.addi	zero,0
20060a5a:	0001                	c.addi	zero,0
20060a5c:	40b2                	c.lwsp	ra,12(sp)
20060a5e:	4422                	c.lwsp	s0,8(sp)
20060a60:	0141                	c.addi	sp,16
20060a62:	8082                	c.jr	ra
20060a64:	87a2                	c.mv	a5,s0
20060a66:	04000693          	addi	a3,zero,64
20060a6a:	bfa1                	c.j	200609c2 <inic_host_trx_int_hdl+0x28>
20060a6c:	00044503          	lbu	a0,0(s0)
20060a70:	00c44603          	lbu	a2,12(s0)
20060a74:	404c                	c.lw	a1,4(s0)
20060a76:	f01ff0ef          	jal	ra,20060976 <inic_host_trx_event_hdl>
20060a7a:	4501                	c.li	a0,0
20060a7c:	b769                	c.j	20060a06 <inic_host_trx_int_hdl+0x6c>

20060a7e <inic_msg_q_task>:
20060a7e:	1101                	c.addi	sp,-32
20060a80:	cc22                	c.swsp	s0,24(sp)
20060a82:	20068437          	lui	s0,0x20068
20060a86:	c84a                	c.swsp	s2,16(sp)
20060a88:	c64e                	c.swsp	s3,12(sp)
20060a8a:	f8040913          	addi	s2,s0,-128 # 20067f80 <g_ipc_msg_q_priv>
20060a8e:	ce06                	c.swsp	ra,28(sp)
20060a90:	ca26                	c.swsp	s1,20(sp)
20060a92:	f8040413          	addi	s0,s0,-128
20060a96:	4985                	c.li	s3,1
20060a98:	00c92503          	lw	a0,12(s2)
20060a9c:	55fd                	c.li	a1,-1
20060a9e:	dd1fa0ef          	jal	ra,2005b86e <rtos_sema_take>
20060aa2:	c53fa0ef          	jal	ra,2005b6f4 <rtos_critical_enter>
20060aa6:	8522                	c.mv	a0,s0
20060aa8:	2425                	c.jal	20060cd0 <rtw_queue_empty>
20060aaa:	03351063          	bne	a0,s3,20060aca <inic_msg_q_task+0x4c>
20060aae:	c6bfa0ef          	jal	ra,2005b718 <rtos_critical_exit>
20060ab2:	01444783          	lbu	a5,20(s0)
20060ab6:	f3ed                	c.bnez	a5,20060a98 <inic_msg_q_task+0x1a>
20060ab8:	4462                	c.lwsp	s0,24(sp)
20060aba:	40f2                	c.lwsp	ra,28(sp)
20060abc:	44d2                	c.lwsp	s1,20(sp)
20060abe:	4942                	c.lwsp	s2,16(sp)
20060ac0:	49b2                	c.lwsp	s3,12(sp)
20060ac2:	4501                	c.li	a0,0
20060ac4:	6105                	c.addi16sp	sp,32
20060ac6:	81efb06f          	jal	zero,2005bae4 <rtos_task_delete>
20060aca:	4004                	c.lw	s1,0(s0)
20060acc:	8526                	c.mv	a0,s1
20060ace:	22ed                	c.jal	20060cb8 <rtw_list_delete>
20060ad0:	c49fa0ef          	jal	ra,2005b718 <rtos_critical_exit>
20060ad4:	dcf9                	c.beqz	s1,20060ab2 <inic_msg_q_task+0x34>
20060ad6:	44c8                	c.lw	a0,12(s1)
20060ad8:	448c                	c.lw	a1,8(s1)
20060ada:	00155613          	srli	a2,a0,0x1
20060ade:	810d                	c.srli	a0,0x3
20060ae0:	8a0d                	c.andi	a2,3
20060ae2:	897d                	c.andi	a0,31
20060ae4:	e93ff0ef          	jal	ra,20060976 <inic_host_trx_event_hdl>
20060ae8:	c0dfa0ef          	jal	ra,2005b6f4 <rtos_critical_enter>
20060aec:	00c4c783          	lbu	a5,12(s1)
20060af0:	9bf9                	c.andi	a5,-2
20060af2:	00f48623          	sb	a5,12(s1)
20060af6:	c23fa0ef          	jal	ra,2005b718 <rtos_critical_exit>
20060afa:	b765                	c.j	20060aa2 <inic_msg_q_task+0x24>

20060afc <inic_msg_q_init>:
20060afc:	1141                	c.addi	sp,-16
20060afe:	c422                	c.swsp	s0,8(sp)
20060b00:	20068437          	lui	s0,0x20068
20060b04:	02000613          	addi	a2,zero,32
20060b08:	4581                	c.li	a1,0
20060b0a:	f8040513          	addi	a0,s0,-128 # 20067f80 <g_ipc_msg_q_priv>
20060b0e:	c606                	c.swsp	ra,12(sp)
20060b10:	c226                	c.swsp	s1,4(sp)
20060b12:	dffa9097          	auipc	ra,0xdffa9
20060b16:	fbc080e7          	jalr	ra,-68(ra) # 9ace <__wrap_memset>
20060b1a:	20068537          	lui	a0,0x20068
20060b1e:	04000613          	addi	a2,zero,64
20060b22:	4581                	c.li	a1,0
20060b24:	f4050513          	addi	a0,a0,-192 # 20067f40 <g_inic_ipc_ex_msg>
20060b28:	dffa9097          	auipc	ra,0xdffa9
20060b2c:	fa6080e7          	jalr	ra,-90(ra) # 9ace <__wrap_memset>
20060b30:	f8040513          	addi	a0,s0,-128
20060b34:	f8040493          	addi	s1,s0,-128
20060b38:	2279                	c.jal	20060cc6 <rtw_init_queue>
20060b3a:	567d                	c.li	a2,-1
20060b3c:	4581                	c.li	a1,0
20060b3e:	00c48513          	addi	a0,s1,12
20060b42:	caffa0ef          	jal	ra,2005b7f0 <rtos_sema_create_static>
20060b46:	567d                	c.li	a2,-1
20060b48:	4581                	c.li	a1,0
20060b4a:	01048513          	addi	a0,s1,16
20060b4e:	ca3fa0ef          	jal	ra,2005b7f0 <rtos_sema_create_static>
20060b52:	4888                	c.lw	a0,16(s1)
20060b54:	f8040413          	addi	s0,s0,-128
20060b58:	d63fa0ef          	jal	ra,2005b8ba <rtos_sema_give>
20060b5c:	4481a503          	lw	a0,1096(gp) # 20066520 <wifi_user_config+0x20>
20060b60:	00a48e23          	sb	a0,28(s1)
20060b64:	0ff57513          	andi	a0,a0,255
20060b68:	0512                	c.slli	a0,0x4
20060b6a:	bdbfa0ef          	jal	ra,2005b744 <rtos_mem_zmalloc>
20060b6e:	cc88                	c.sw	a0,24(s1)
20060b70:	4781                	c.li	a5,0
20060b72:	01c44703          	lbu	a4,28(s0)
20060b76:	04e7c763          	blt	a5,a4,20060bc4 <inic_msg_q_init+0xc8>
20060b7a:	20061637          	lui	a2,0x20061
20060b7e:	200645b7          	lui	a1,0x20064
20060b82:	4799                	c.li	a5,6
20060b84:	31000713          	addi	a4,zero,784
20060b88:	4681                	c.li	a3,0
20060b8a:	a7e60613          	addi	a2,a2,-1410 # 20060a7e <inic_msg_q_task>
20060b8e:	14858593          	addi	a1,a1,328 # 20064148 <__func__.1+0x14>
20060b92:	4501                	c.li	a0,0
20060b94:	f2dfa0ef          	jal	ra,2005bac0 <rtos_task_create>
20060b98:	cd11                	c.beqz	a0,20060bb4 <inic_msg_q_init+0xb8>
20060b9a:	200646b7          	lui	a3,0x20064
20060b9e:	200645b7          	lui	a1,0x20064
20060ba2:	15868693          	addi	a3,a3,344 # 20064158 <__func__.1+0x24>
20060ba6:	04500613          	addi	a2,zero,69
20060baa:	09458593          	addi	a1,a1,148 # 20064094 <register_string+0x140>
20060bae:	4509                	c.li	a0,2
20060bb0:	ac0fa0ef          	jal	ra,2005ae70 <rtk_log_write>
20060bb4:	4785                	c.li	a5,1
20060bb6:	40b2                	c.lwsp	ra,12(sp)
20060bb8:	00f40a23          	sb	a5,20(s0)
20060bbc:	4422                	c.lwsp	s0,8(sp)
20060bbe:	4492                	c.lwsp	s1,4(sp)
20060bc0:	0141                	c.addi	sp,16
20060bc2:	8082                	c.jr	ra
20060bc4:	4c18                	c.lw	a4,24(s0)
20060bc6:	00479693          	slli	a3,a5,0x4
20060bca:	0785                	c.addi	a5,1
20060bcc:	9736                	c.add	a4,a3
20060bce:	00c74683          	lbu	a3,12(a4)
20060bd2:	9af9                	c.andi	a3,-2
20060bd4:	00d70623          	sb	a3,12(a4)
20060bd8:	bf69                	c.j	20060b72 <inic_msg_q_init+0x76>

20060bda <inic_msg_enqueue>:
20060bda:	1101                	c.addi	sp,-32
20060bdc:	ca26                	c.swsp	s1,20(sp)
20060bde:	c84a                	c.swsp	s2,16(sp)
20060be0:	c64e                	c.swsp	s3,12(sp)
20060be2:	200684b7          	lui	s1,0x20068
20060be6:	ce06                	c.swsp	ra,28(sp)
20060be8:	cc22                	c.swsp	s0,24(sp)
20060bea:	89aa                	c.mv	s3,a0
20060bec:	b09fa0ef          	jal	ra,2005b6f4 <rtos_critical_enter>
20060bf0:	f8048793          	addi	a5,s1,-128 # 20067f80 <g_ipc_msg_q_priv>
20060bf4:	01c7c683          	lbu	a3,28(a5)
20060bf8:	4f90                	c.lw	a2,24(a5)
20060bfa:	f8048913          	addi	s2,s1,-128
20060bfe:	4781                	c.li	a5,0
20060c00:	00d7c463          	blt	a5,a3,20060c08 <inic_msg_enqueue+0x2e>
20060c04:	4401                	c.li	s0,0
20060c06:	a829                	c.j	20060c20 <inic_msg_enqueue+0x46>
20060c08:	00479413          	slli	s0,a5,0x4
20060c0c:	9432                	c.add	s0,a2
20060c0e:	00c44703          	lbu	a4,12(s0)
20060c12:	00177593          	andi	a1,a4,1
20060c16:	e1b1                	c.bnez	a1,20060c5a <inic_msg_enqueue+0x80>
20060c18:	00176713          	ori	a4,a4,1
20060c1c:	00e40623          	sb	a4,12(s0)
20060c20:	af9fa0ef          	jal	ra,2005b718 <rtos_critical_exit>
20060c24:	ec0d                	c.bnez	s0,20060c5e <inic_msg_enqueue+0x84>
20060c26:	200646b7          	lui	a3,0x20064
20060c2a:	200645b7          	lui	a1,0x20064
20060c2e:	17068693          	addi	a3,a3,368 # 20064170 <__func__.1+0x3c>
20060c32:	04500613          	addi	a2,zero,69
20060c36:	09458593          	addi	a1,a1,148 # 20064094 <register_string+0x140>
20060c3a:	4509                	c.li	a0,2
20060c3c:	a34fa0ef          	jal	ra,2005ae70 <rtk_log_write>
20060c40:	547d                	c.li	s0,-1
20060c42:	00c92503          	lw	a0,12(s2)
20060c46:	c75fa0ef          	jal	ra,2005b8ba <rtos_sema_give>
20060c4a:	40f2                	c.lwsp	ra,28(sp)
20060c4c:	8522                	c.mv	a0,s0
20060c4e:	4462                	c.lwsp	s0,24(sp)
20060c50:	44d2                	c.lwsp	s1,20(sp)
20060c52:	4942                	c.lwsp	s2,16(sp)
20060c54:	49b2                	c.lwsp	s3,12(sp)
20060c56:	6105                	c.addi16sp	sp,32
20060c58:	8082                	c.jr	ra
20060c5a:	0785                	c.addi	a5,1
20060c5c:	b755                	c.j	20060c00 <inic_msg_enqueue+0x26>
20060c5e:	0009a783          	lw	a5,0(s3)
20060c62:	00379713          	slli	a4,a5,0x3
20060c66:	00c44783          	lbu	a5,12(s0)
20060c6a:	8b9d                	c.andi	a5,7
20060c6c:	8fd9                	c.or	a5,a4
20060c6e:	00f40623          	sb	a5,12(s0)
20060c72:	0049a703          	lw	a4,4(s3)
20060c76:	9be5                	c.andi	a5,-7
20060c78:	c418                	c.sw	a4,8(s0)
20060c7a:	00c9a703          	lw	a4,12(s3)
20060c7e:	8b0d                	c.andi	a4,3
20060c80:	0706                	c.slli	a4,0x1
20060c82:	8fd9                	c.or	a5,a4
20060c84:	00f40623          	sb	a5,12(s0)
20060c88:	a6dfa0ef          	jal	ra,2005b6f4 <rtos_critical_enter>
20060c8c:	8522                	c.mv	a0,s0
20060c8e:	f8048593          	addi	a1,s1,-128
20060c92:	2829                	c.jal	20060cac <rtw_list_insert_tail>
20060c94:	4405                	c.li	s0,1
20060c96:	a83fa0ef          	jal	ra,2005b718 <rtos_critical_exit>
20060c9a:	b765                	c.j	20060c42 <inic_msg_enqueue+0x68>

20060c9c <inic_msg_get_queue_status>:
20060c9c:	200687b7          	lui	a5,0x20068
20060ca0:	f947c503          	lbu	a0,-108(a5) # 20067f94 <g_ipc_msg_q_priv+0x14>
20060ca4:	8082                	c.jr	ra

20060ca6 <rtw_init_listhead>:
20060ca6:	c108                	c.sw	a0,0(a0)
20060ca8:	c148                	c.sw	a0,4(a0)
20060caa:	8082                	c.jr	ra

20060cac <rtw_list_insert_tail>:
20060cac:	41dc                	c.lw	a5,4(a1)
20060cae:	c1c8                	c.sw	a0,4(a1)
20060cb0:	c10c                	c.sw	a1,0(a0)
20060cb2:	c15c                	c.sw	a5,4(a0)
20060cb4:	c388                	c.sw	a0,0(a5)
20060cb6:	8082                	c.jr	ra

20060cb8 <rtw_list_delete>:
20060cb8:	415c                	c.lw	a5,4(a0)
20060cba:	4118                	c.lw	a4,0(a0)
20060cbc:	c35c                	c.sw	a5,4(a4)
20060cbe:	c398                	c.sw	a4,0(a5)
20060cc0:	c108                	c.sw	a0,0(a0)
20060cc2:	c148                	c.sw	a0,4(a0)
20060cc4:	8082                	c.jr	ra

20060cc6 <rtw_init_queue>:
20060cc6:	c108                	c.sw	a0,0(a0)
20060cc8:	c148                	c.sw	a0,4(a0)
20060cca:	0521                	c.addi	a0,8
20060ccc:	ad1fa06f          	jal	zero,2005b79c <rtos_mutex_create_static>

20060cd0 <rtw_queue_empty>:
20060cd0:	411c                	c.lw	a5,0(a0)
20060cd2:	40a78533          	sub	a0,a5,a0
20060cd6:	00153513          	sltiu	a0,a0,1
20060cda:	8082                	c.jr	ra

20060cdc <init_timer_wrapper>:
20060cdc:	1141                	c.addi	sp,-16
20060cde:	ff418513          	addi	a0,gp,-12 # 200660cc <timer_table>
20060ce2:	c606                	c.swsp	ra,12(sp)
20060ce4:	fc3ff0ef          	jal	ra,20060ca6 <rtw_init_listhead>
20060ce8:	40b2                	c.lwsp	ra,12(sp)
20060cea:	fe01ae23          	sw	zero,-4(gp) # 200660d4 <timer_used_num>
20060cee:	fe01a823          	sw	zero,-16(gp) # 200660c8 <max_timer_used_num>
20060cf2:	0141                	c.addi	sp,16
20060cf4:	8082                	c.jr	ra

20060cf6 <deregister_tm_clones>:
20060cf6:	20058537          	lui	a0,0x20058
20060cfa:	200587b7          	lui	a5,0x20058
20060cfe:	06050713          	addi	a4,a0,96 # 20058060 <BOOT_InitGP>
20060d02:	06078793          	addi	a5,a5,96 # 20058060 <BOOT_InitGP>
20060d06:	00e78863          	beq	a5,a4,20060d16 <deregister_tm_clones+0x20>
20060d0a:	00000793          	addi	a5,zero,0
20060d0e:	c781                	c.beqz	a5,20060d16 <deregister_tm_clones+0x20>
20060d10:	06050513          	addi	a0,a0,96
20060d14:	8782                	c.jr	a5
20060d16:	8082                	c.jr	ra

20060d18 <register_tm_clones>:
20060d18:	200587b7          	lui	a5,0x20058
20060d1c:	20058537          	lui	a0,0x20058
20060d20:	06078593          	addi	a1,a5,96 # 20058060 <BOOT_InitGP>
20060d24:	06050793          	addi	a5,a0,96 # 20058060 <BOOT_InitGP>
20060d28:	8d9d                	c.sub	a1,a5
20060d2a:	8589                	c.srai	a1,0x2
20060d2c:	4789                	c.li	a5,2
20060d2e:	02f5c5b3          	div	a1,a1,a5
20060d32:	c599                	c.beqz	a1,20060d40 <register_tm_clones+0x28>
20060d34:	00000793          	addi	a5,zero,0
20060d38:	c781                	c.beqz	a5,20060d40 <register_tm_clones+0x28>
20060d3a:	06050513          	addi	a0,a0,96
20060d3e:	8782                	c.jr	a5
20060d40:	8082                	c.jr	ra

20060d42 <__do_global_dtors_aux>:
20060d42:	200687b7          	lui	a5,0x20068
20060d46:	fac7c703          	lbu	a4,-84(a5) # 20067fac <completed.1>
20060d4a:	eb05                	c.bnez	a4,20060d7a <__do_global_dtors_aux+0x38>
20060d4c:	1141                	c.addi	sp,-16
20060d4e:	c422                	c.swsp	s0,8(sp)
20060d50:	c606                	c.swsp	ra,12(sp)
20060d52:	843e                	c.mv	s0,a5
20060d54:	374d                	c.jal	20060cf6 <deregister_tm_clones>
20060d56:	00000793          	addi	a5,zero,0
20060d5a:	cb89                	c.beqz	a5,20060d6c <__do_global_dtors_aux+0x2a>
20060d5c:	20058537          	lui	a0,0x20058
20060d60:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
20060d64:	00000097          	auipc	ra,0x0
20060d68:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20060d6c:	4785                	c.li	a5,1
20060d6e:	40b2                	c.lwsp	ra,12(sp)
20060d70:	faf40623          	sb	a5,-84(s0)
20060d74:	4422                	c.lwsp	s0,8(sp)
20060d76:	0141                	c.addi	sp,16
20060d78:	8082                	c.jr	ra
20060d7a:	8082                	c.jr	ra

20060d7c <frame_dummy>:
20060d7c:	00000793          	addi	a5,zero,0
20060d80:	c38d                	c.beqz	a5,20060da2 <frame_dummy+0x26>
20060d82:	200685b7          	lui	a1,0x20068
20060d86:	20058537          	lui	a0,0x20058
20060d8a:	1141                	c.addi	sp,-16
20060d8c:	fb058593          	addi	a1,a1,-80 # 20067fb0 <object.0>
20060d90:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
20060d94:	c606                	c.swsp	ra,12(sp)
20060d96:	00000097          	auipc	ra,0x0
20060d9a:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20060d9e:	40b2                	c.lwsp	ra,12(sp)
20060da0:	0141                	c.addi	sp,16
20060da2:	bf9d                	c.j	20060d18 <register_tm_clones>

20060da4 <__clzsi2>:
20060da4:	67c1                	c.lui	a5,0x10
20060da6:	02f57663          	bgeu	a0,a5,20060dd2 <__clzsi2+0x2e>
20060daa:	0ff00793          	addi	a5,zero,255
20060dae:	00a7b7b3          	sltu	a5,a5,a0
20060db2:	078e                	c.slli	a5,0x3
20060db4:	20064737          	lui	a4,0x20064
20060db8:	02000693          	addi	a3,zero,32
20060dbc:	8e9d                	c.sub	a3,a5
20060dbe:	00f55533          	srl	a0,a0,a5
20060dc2:	18870793          	addi	a5,a4,392 # 20064188 <__clz_tab>
20060dc6:	953e                	c.add	a0,a5
20060dc8:	00054503          	lbu	a0,0(a0)
20060dcc:	40a68533          	sub	a0,a3,a0
20060dd0:	8082                	c.jr	ra
20060dd2:	01000737          	lui	a4,0x1000
20060dd6:	47c1                	c.li	a5,16
20060dd8:	fce56ee3          	bltu	a0,a4,20060db4 <__clzsi2+0x10>
20060ddc:	47e1                	c.li	a5,24
20060dde:	bfd9                	c.j	20060db4 <__clzsi2+0x10>

20060de0 <__udivdi3>:
20060de0:	88aa                	c.mv	a7,a0
20060de2:	87ae                	c.mv	a5,a1
20060de4:	8832                	c.mv	a6,a2
20060de6:	8536                	c.mv	a0,a3
20060de8:	8346                	c.mv	t1,a7
20060dea:	20069663          	bne	a3,zero,20060ff6 <__udivdi3+0x216>
20060dee:	200646b7          	lui	a3,0x20064
20060df2:	18868693          	addi	a3,a3,392 # 20064188 <__clz_tab>
20060df6:	0ac5ff63          	bgeu	a1,a2,20060eb4 <__udivdi3+0xd4>
20060dfa:	6741                	c.lui	a4,0x10
20060dfc:	0ae67563          	bgeu	a2,a4,20060ea6 <__udivdi3+0xc6>
20060e00:	0ff00713          	addi	a4,zero,255
20060e04:	00c73733          	sltu	a4,a4,a2
20060e08:	070e                	c.slli	a4,0x3
20060e0a:	00e65533          	srl	a0,a2,a4
20060e0e:	96aa                	c.add	a3,a0
20060e10:	0006c683          	lbu	a3,0(a3)
20060e14:	02000513          	addi	a0,zero,32
20060e18:	9736                	c.add	a4,a3
20060e1a:	40e506b3          	sub	a3,a0,a4
20060e1e:	00e50b63          	beq	a0,a4,20060e34 <__udivdi3+0x54>
20060e22:	00d795b3          	sll	a1,a5,a3
20060e26:	00e8d733          	srl	a4,a7,a4
20060e2a:	00d61833          	sll	a6,a2,a3
20060e2e:	8dd9                	c.or	a1,a4
20060e30:	00d89333          	sll	t1,a7,a3
20060e34:	01085893          	srli	a7,a6,0x10
20060e38:	0315d6b3          	divu	a3,a1,a7
20060e3c:	01081613          	slli	a2,a6,0x10
20060e40:	8241                	c.srli	a2,0x10
20060e42:	01035793          	srli	a5,t1,0x10
20060e46:	0315f733          	remu	a4,a1,a7
20060e4a:	8536                	c.mv	a0,a3
20060e4c:	02d605b3          	mul	a1,a2,a3
20060e50:	0742                	c.slli	a4,0x10
20060e52:	8fd9                	c.or	a5,a4
20060e54:	00b7fc63          	bgeu	a5,a1,20060e6c <__udivdi3+0x8c>
20060e58:	97c2                	c.add	a5,a6
20060e5a:	fff68513          	addi	a0,a3,-1
20060e5e:	0107e763          	bltu	a5,a6,20060e6c <__udivdi3+0x8c>
20060e62:	00b7f563          	bgeu	a5,a1,20060e6c <__udivdi3+0x8c>
20060e66:	ffe68513          	addi	a0,a3,-2
20060e6a:	97c2                	c.add	a5,a6
20060e6c:	8f8d                	c.sub	a5,a1
20060e6e:	0317f733          	remu	a4,a5,a7
20060e72:	0342                	c.slli	t1,0x10
20060e74:	01035313          	srli	t1,t1,0x10
20060e78:	0317d7b3          	divu	a5,a5,a7
20060e7c:	0742                	c.slli	a4,0x10
20060e7e:	00676333          	or	t1,a4,t1
20060e82:	02f606b3          	mul	a3,a2,a5
20060e86:	863e                	c.mv	a2,a5
20060e88:	00d37b63          	bgeu	t1,a3,20060e9e <__udivdi3+0xbe>
20060e8c:	9342                	c.add	t1,a6
20060e8e:	fff78613          	addi	a2,a5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20060e92:	01036663          	bltu	t1,a6,20060e9e <__udivdi3+0xbe>
20060e96:	00d37463          	bgeu	t1,a3,20060e9e <__udivdi3+0xbe>
20060e9a:	ffe78613          	addi	a2,a5,-2
20060e9e:	0542                	c.slli	a0,0x10
20060ea0:	8d51                	c.or	a0,a2
20060ea2:	4581                	c.li	a1,0
20060ea4:	a85d                	c.j	20060f5a <__udivdi3+0x17a>
20060ea6:	01000537          	lui	a0,0x1000
20060eaa:	4741                	c.li	a4,16
20060eac:	f4a66fe3          	bltu	a2,a0,20060e0a <__udivdi3+0x2a>
20060eb0:	4761                	c.li	a4,24
20060eb2:	bfa1                	c.j	20060e0a <__udivdi3+0x2a>
20060eb4:	e601                	c.bnez	a2,20060ebc <__udivdi3+0xdc>
20060eb6:	4705                	c.li	a4,1
20060eb8:	02c75833          	divu	a6,a4,a2
20060ebc:	6741                	c.lui	a4,0x10
20060ebe:	08e87f63          	bgeu	a6,a4,20060f5c <__udivdi3+0x17c>
20060ec2:	0ff00713          	addi	a4,zero,255
20060ec6:	01077363          	bgeu	a4,a6,20060ecc <__udivdi3+0xec>
20060eca:	4521                	c.li	a0,8
20060ecc:	00a85733          	srl	a4,a6,a0
20060ed0:	96ba                	c.add	a3,a4
20060ed2:	0006c703          	lbu	a4,0(a3)
20060ed6:	02000613          	addi	a2,zero,32
20060eda:	972a                	c.add	a4,a0
20060edc:	40e606b3          	sub	a3,a2,a4
20060ee0:	08e61563          	bne	a2,a4,20060f6a <__udivdi3+0x18a>
20060ee4:	410787b3          	sub	a5,a5,a6
20060ee8:	4585                	c.li	a1,1
20060eea:	01085893          	srli	a7,a6,0x10
20060eee:	01081613          	slli	a2,a6,0x10
20060ef2:	8241                	c.srli	a2,0x10
20060ef4:	01035713          	srli	a4,t1,0x10
20060ef8:	0317f6b3          	remu	a3,a5,a7
20060efc:	0317d7b3          	divu	a5,a5,a7
20060f00:	06c2                	c.slli	a3,0x10
20060f02:	8f55                	c.or	a4,a3
20060f04:	02f60e33          	mul	t3,a2,a5
20060f08:	853e                	c.mv	a0,a5
20060f0a:	01c77c63          	bgeu	a4,t3,20060f22 <__udivdi3+0x142>
20060f0e:	9742                	c.add	a4,a6
20060f10:	fff78513          	addi	a0,a5,-1
20060f14:	01076763          	bltu	a4,a6,20060f22 <__udivdi3+0x142>
20060f18:	01c77563          	bgeu	a4,t3,20060f22 <__udivdi3+0x142>
20060f1c:	ffe78513          	addi	a0,a5,-2
20060f20:	9742                	c.add	a4,a6
20060f22:	41c70733          	sub	a4,a4,t3
20060f26:	031777b3          	remu	a5,a4,a7
20060f2a:	0342                	c.slli	t1,0x10
20060f2c:	01035313          	srli	t1,t1,0x10
20060f30:	03175733          	divu	a4,a4,a7
20060f34:	07c2                	c.slli	a5,0x10
20060f36:	0067e333          	or	t1,a5,t1
20060f3a:	02e606b3          	mul	a3,a2,a4
20060f3e:	863a                	c.mv	a2,a4
20060f40:	00d37b63          	bgeu	t1,a3,20060f56 <__udivdi3+0x176>
20060f44:	9342                	c.add	t1,a6
20060f46:	fff70613          	addi	a2,a4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20060f4a:	01036663          	bltu	t1,a6,20060f56 <__udivdi3+0x176>
20060f4e:	00d37463          	bgeu	t1,a3,20060f56 <__udivdi3+0x176>
20060f52:	ffe70613          	addi	a2,a4,-2
20060f56:	0542                	c.slli	a0,0x10
20060f58:	8d51                	c.or	a0,a2
20060f5a:	8082                	c.jr	ra
20060f5c:	01000737          	lui	a4,0x1000
20060f60:	4541                	c.li	a0,16
20060f62:	f6e865e3          	bltu	a6,a4,20060ecc <__udivdi3+0xec>
20060f66:	4561                	c.li	a0,24
20060f68:	b795                	c.j	20060ecc <__udivdi3+0xec>
20060f6a:	00d81833          	sll	a6,a6,a3
20060f6e:	00e7d533          	srl	a0,a5,a4
20060f72:	00d89333          	sll	t1,a7,a3
20060f76:	00d797b3          	sll	a5,a5,a3
20060f7a:	00e8d733          	srl	a4,a7,a4
20060f7e:	01085893          	srli	a7,a6,0x10
20060f82:	00f76633          	or	a2,a4,a5
20060f86:	03157733          	remu	a4,a0,a7
20060f8a:	01081793          	slli	a5,a6,0x10
20060f8e:	83c1                	c.srli	a5,0x10
20060f90:	01065593          	srli	a1,a2,0x10
20060f94:	03155533          	divu	a0,a0,a7
20060f98:	0742                	c.slli	a4,0x10
20060f9a:	8f4d                	c.or	a4,a1
20060f9c:	02a786b3          	mul	a3,a5,a0
20060fa0:	85aa                	c.mv	a1,a0
20060fa2:	00d77c63          	bgeu	a4,a3,20060fba <__udivdi3+0x1da>
20060fa6:	9742                	c.add	a4,a6
20060fa8:	fff50593          	addi	a1,a0,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20060fac:	01076763          	bltu	a4,a6,20060fba <__udivdi3+0x1da>
20060fb0:	00d77563          	bgeu	a4,a3,20060fba <__udivdi3+0x1da>
20060fb4:	ffe50593          	addi	a1,a0,-2
20060fb8:	9742                	c.add	a4,a6
20060fba:	40d706b3          	sub	a3,a4,a3
20060fbe:	0316f733          	remu	a4,a3,a7
20060fc2:	0642                	c.slli	a2,0x10
20060fc4:	8241                	c.srli	a2,0x10
20060fc6:	0316d6b3          	divu	a3,a3,a7
20060fca:	0742                	c.slli	a4,0x10
20060fcc:	02d78533          	mul	a0,a5,a3
20060fd0:	00c767b3          	or	a5,a4,a2
20060fd4:	8736                	c.mv	a4,a3
20060fd6:	00a7fc63          	bgeu	a5,a0,20060fee <__udivdi3+0x20e>
20060fda:	97c2                	c.add	a5,a6
20060fdc:	fff68713          	addi	a4,a3,-1
20060fe0:	0107e763          	bltu	a5,a6,20060fee <__udivdi3+0x20e>
20060fe4:	00a7f563          	bgeu	a5,a0,20060fee <__udivdi3+0x20e>
20060fe8:	ffe68713          	addi	a4,a3,-2
20060fec:	97c2                	c.add	a5,a6
20060fee:	05c2                	c.slli	a1,0x10
20060ff0:	8f89                	c.sub	a5,a0
20060ff2:	8dd9                	c.or	a1,a4
20060ff4:	bddd                	c.j	20060eea <__udivdi3+0x10a>
20060ff6:	14d5e263          	bltu	a1,a3,2006113a <__udivdi3+0x35a>
20060ffa:	6741                	c.lui	a4,0x10
20060ffc:	02e6ff63          	bgeu	a3,a4,2006103a <__udivdi3+0x25a>
20061000:	0ff00713          	addi	a4,zero,255
20061004:	00d735b3          	sltu	a1,a4,a3
20061008:	058e                	c.slli	a1,0x3
2006100a:	20064737          	lui	a4,0x20064
2006100e:	00b6d533          	srl	a0,a3,a1
20061012:	18870713          	addi	a4,a4,392 # 20064188 <__clz_tab>
20061016:	972a                	c.add	a4,a0
20061018:	00074703          	lbu	a4,0(a4)
2006101c:	02000513          	addi	a0,zero,32
20061020:	972e                	c.add	a4,a1
20061022:	40e505b3          	sub	a1,a0,a4
20061026:	02e51163          	bne	a0,a4,20061048 <__udivdi3+0x268>
2006102a:	4505                	c.li	a0,1
2006102c:	f2f6e7e3          	bltu	a3,a5,20060f5a <__udivdi3+0x17a>
20061030:	00c8b533          	sltu	a0,a7,a2
20061034:	00154513          	xori	a0,a0,1
20061038:	b70d                	c.j	20060f5a <__udivdi3+0x17a>
2006103a:	01000737          	lui	a4,0x1000
2006103e:	45c1                	c.li	a1,16
20061040:	fce6e5e3          	bltu	a3,a4,2006100a <__udivdi3+0x22a>
20061044:	45e1                	c.li	a1,24
20061046:	b7d1                	c.j	2006100a <__udivdi3+0x22a>
20061048:	00e65833          	srl	a6,a2,a4
2006104c:	00b696b3          	sll	a3,a3,a1
20061050:	00d86833          	or	a6,a6,a3
20061054:	00e7de33          	srl	t3,a5,a4
20061058:	01085e93          	srli	t4,a6,0x10
2006105c:	03de76b3          	remu	a3,t3,t4
20061060:	00b797b3          	sll	a5,a5,a1
20061064:	00e8d733          	srl	a4,a7,a4
20061068:	00b61333          	sll	t1,a2,a1
2006106c:	00f76633          	or	a2,a4,a5
20061070:	01081793          	slli	a5,a6,0x10
20061074:	83c1                	c.srli	a5,0x10
20061076:	01065713          	srli	a4,a2,0x10
2006107a:	03de5e33          	divu	t3,t3,t4
2006107e:	06c2                	c.slli	a3,0x10
20061080:	8f55                	c.or	a4,a3
20061082:	03c78f33          	mul	t5,a5,t3
20061086:	8572                	c.mv	a0,t3
20061088:	01e77c63          	bgeu	a4,t5,200610a0 <__udivdi3+0x2c0>
2006108c:	9742                	c.add	a4,a6
2006108e:	fffe0513          	addi	a0,t3,-1
20061092:	01076763          	bltu	a4,a6,200610a0 <__udivdi3+0x2c0>
20061096:	01e77563          	bgeu	a4,t5,200610a0 <__udivdi3+0x2c0>
2006109a:	ffee0513          	addi	a0,t3,-2
2006109e:	9742                	c.add	a4,a6
200610a0:	41e70733          	sub	a4,a4,t5
200610a4:	03d776b3          	remu	a3,a4,t4
200610a8:	03d75733          	divu	a4,a4,t4
200610ac:	06c2                	c.slli	a3,0x10
200610ae:	02e78e33          	mul	t3,a5,a4
200610b2:	01061793          	slli	a5,a2,0x10
200610b6:	83c1                	c.srli	a5,0x10
200610b8:	8fd5                	c.or	a5,a3
200610ba:	863a                	c.mv	a2,a4
200610bc:	01c7fc63          	bgeu	a5,t3,200610d4 <__udivdi3+0x2f4>
200610c0:	97c2                	c.add	a5,a6
200610c2:	fff70613          	addi	a2,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
200610c6:	0107e763          	bltu	a5,a6,200610d4 <__udivdi3+0x2f4>
200610ca:	01c7f563          	bgeu	a5,t3,200610d4 <__udivdi3+0x2f4>
200610ce:	ffe70613          	addi	a2,a4,-2
200610d2:	97c2                	c.add	a5,a6
200610d4:	0542                	c.slli	a0,0x10
200610d6:	6ec1                	c.lui	t4,0x10
200610d8:	8d51                	c.or	a0,a2
200610da:	fffe8693          	addi	a3,t4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
200610de:	00d57833          	and	a6,a0,a3
200610e2:	01055613          	srli	a2,a0,0x10
200610e6:	00d376b3          	and	a3,t1,a3
200610ea:	01035313          	srli	t1,t1,0x10
200610ee:	41c787b3          	sub	a5,a5,t3
200610f2:	02d80e33          	mul	t3,a6,a3
200610f6:	02d606b3          	mul	a3,a2,a3
200610fa:	010e5713          	srli	a4,t3,0x10
200610fe:	02680833          	mul	a6,a6,t1
20061102:	9836                	c.add	a6,a3
20061104:	9742                	c.add	a4,a6
20061106:	02660633          	mul	a2,a2,t1
2006110a:	00d77363          	bgeu	a4,a3,20061110 <__udivdi3+0x330>
2006110e:	9676                	c.add	a2,t4
20061110:	01075693          	srli	a3,a4,0x10
20061114:	9636                	c.add	a2,a3
20061116:	02c7e063          	bltu	a5,a2,20061136 <__udivdi3+0x356>
2006111a:	d8c794e3          	bne	a5,a2,20060ea2 <__udivdi3+0xc2>
2006111e:	67c1                	c.lui	a5,0x10
20061120:	17fd                	c.addi	a5,-1
20061122:	8f7d                	c.and	a4,a5
20061124:	0742                	c.slli	a4,0x10
20061126:	00fe7e33          	and	t3,t3,a5
2006112a:	00b898b3          	sll	a7,a7,a1
2006112e:	9772                	c.add	a4,t3
20061130:	4581                	c.li	a1,0
20061132:	e2e8f4e3          	bgeu	a7,a4,20060f5a <__udivdi3+0x17a>
20061136:	157d                	c.addi	a0,-1
20061138:	b3ad                	c.j	20060ea2 <__udivdi3+0xc2>
2006113a:	4581                	c.li	a1,0
2006113c:	4501                	c.li	a0,0
2006113e:	bd31                	c.j	20060f5a <__udivdi3+0x17a>

20061140 <__umoddi3>:
20061140:	8eaa                	c.mv	t4,a0
20061142:	832e                	c.mv	t1,a1
20061144:	8832                	c.mv	a6,a2
20061146:	87b6                	c.mv	a5,a3
20061148:	1c069263          	bne	a3,zero,2006130c <__umoddi3+0x1cc>
2006114c:	20064737          	lui	a4,0x20064
20061150:	18870713          	addi	a4,a4,392 # 20064188 <__clz_tab>
20061154:	0ac5f563          	bgeu	a1,a2,200611fe <__umoddi3+0xbe>
20061158:	66c1                	c.lui	a3,0x10
2006115a:	08d67b63          	bgeu	a2,a3,200611f0 <__umoddi3+0xb0>
2006115e:	0ff00693          	addi	a3,zero,255
20061162:	00c6f363          	bgeu	a3,a2,20061168 <__umoddi3+0x28>
20061166:	47a1                	c.li	a5,8
20061168:	00f656b3          	srl	a3,a2,a5
2006116c:	9736                	c.add	a4,a3
2006116e:	00074703          	lbu	a4,0(a4)
20061172:	97ba                	c.add	a5,a4
20061174:	02000713          	addi	a4,zero,32
20061178:	40f708b3          	sub	a7,a4,a5
2006117c:	00f70b63          	beq	a4,a5,20061192 <__umoddi3+0x52>
20061180:	011315b3          	sll	a1,t1,a7
20061184:	00fed7b3          	srl	a5,t4,a5
20061188:	01161833          	sll	a6,a2,a7
2006118c:	8ddd                	c.or	a1,a5
2006118e:	011e9533          	sll	a0,t4,a7
20061192:	01085693          	srli	a3,a6,0x10
20061196:	02d5f7b3          	remu	a5,a1,a3
2006119a:	01081613          	slli	a2,a6,0x10
2006119e:	8241                	c.srli	a2,0x10
200611a0:	01055713          	srli	a4,a0,0x10
200611a4:	02d5d5b3          	divu	a1,a1,a3
200611a8:	07c2                	c.slli	a5,0x10
200611aa:	8f5d                	c.or	a4,a5
200611ac:	02b605b3          	mul	a1,a2,a1
200611b0:	00b77863          	bgeu	a4,a1,200611c0 <__umoddi3+0x80>
200611b4:	9742                	c.add	a4,a6
200611b6:	01076563          	bltu	a4,a6,200611c0 <__umoddi3+0x80>
200611ba:	00b77363          	bgeu	a4,a1,200611c0 <__umoddi3+0x80>
200611be:	9742                	c.add	a4,a6
200611c0:	8f0d                	c.sub	a4,a1
200611c2:	02d777b3          	remu	a5,a4,a3
200611c6:	0542                	c.slli	a0,0x10
200611c8:	8141                	c.srli	a0,0x10
200611ca:	02d75733          	divu	a4,a4,a3
200611ce:	07c2                	c.slli	a5,0x10
200611d0:	8d5d                	c.or	a0,a5
200611d2:	02e60733          	mul	a4,a2,a4
200611d6:	00e57863          	bgeu	a0,a4,200611e6 <__umoddi3+0xa6>
200611da:	9542                	c.add	a0,a6
200611dc:	01056563          	bltu	a0,a6,200611e6 <__umoddi3+0xa6>
200611e0:	00e57363          	bgeu	a0,a4,200611e6 <__umoddi3+0xa6>
200611e4:	9542                	c.add	a0,a6
200611e6:	8d19                	c.sub	a0,a4
200611e8:	01155533          	srl	a0,a0,a7
200611ec:	4581                	c.li	a1,0
200611ee:	8082                	c.jr	ra
200611f0:	010006b7          	lui	a3,0x1000
200611f4:	47c1                	c.li	a5,16
200611f6:	f6d669e3          	bltu	a2,a3,20061168 <__umoddi3+0x28>
200611fa:	47e1                	c.li	a5,24
200611fc:	b7b5                	c.j	20061168 <__umoddi3+0x28>
200611fe:	e601                	c.bnez	a2,20061206 <__umoddi3+0xc6>
20061200:	4685                	c.li	a3,1
20061202:	02c6d833          	divu	a6,a3,a2
20061206:	66c1                	c.lui	a3,0x10
20061208:	0ed87b63          	bgeu	a6,a3,200612fe <__umoddi3+0x1be>
2006120c:	0ff00693          	addi	a3,zero,255
20061210:	0106f363          	bgeu	a3,a6,20061216 <__umoddi3+0xd6>
20061214:	47a1                	c.li	a5,8
20061216:	00f856b3          	srl	a3,a6,a5
2006121a:	9736                	c.add	a4,a3
2006121c:	00074603          	lbu	a2,0(a4)
20061220:	410305b3          	sub	a1,t1,a6
20061224:	963e                	c.add	a2,a5
20061226:	02000793          	addi	a5,zero,32
2006122a:	40c788b3          	sub	a7,a5,a2
2006122e:	06c78a63          	beq	a5,a2,200612a2 <__umoddi3+0x162>
20061232:	01181833          	sll	a6,a6,a7
20061236:	00c357b3          	srl	a5,t1,a2
2006123a:	011315b3          	sll	a1,t1,a7
2006123e:	01085313          	srli	t1,a6,0x10
20061242:	00ced633          	srl	a2,t4,a2
20061246:	8e4d                	c.or	a2,a1
20061248:	01081593          	slli	a1,a6,0x10
2006124c:	81c1                	c.srli	a1,0x10
2006124e:	011e9533          	sll	a0,t4,a7
20061252:	0267d6b3          	divu	a3,a5,t1
20061256:	0267f733          	remu	a4,a5,t1
2006125a:	01065793          	srli	a5,a2,0x10
2006125e:	02d586b3          	mul	a3,a1,a3
20061262:	0742                	c.slli	a4,0x10
20061264:	8f5d                	c.or	a4,a5
20061266:	00d77863          	bgeu	a4,a3,20061276 <__umoddi3+0x136>
2006126a:	9742                	c.add	a4,a6
2006126c:	01076563          	bltu	a4,a6,20061276 <__umoddi3+0x136>
20061270:	00d77363          	bgeu	a4,a3,20061276 <__umoddi3+0x136>
20061274:	9742                	c.add	a4,a6
20061276:	40d706b3          	sub	a3,a4,a3
2006127a:	0266f733          	remu	a4,a3,t1
2006127e:	0642                	c.slli	a2,0x10
20061280:	8241                	c.srli	a2,0x10
20061282:	0266d6b3          	divu	a3,a3,t1
20061286:	0742                	c.slli	a4,0x10
20061288:	02d586b3          	mul	a3,a1,a3
2006128c:	00c765b3          	or	a1,a4,a2
20061290:	00d5f863          	bgeu	a1,a3,200612a0 <__umoddi3+0x160>
20061294:	95c2                	c.add	a1,a6
20061296:	0105e563          	bltu	a1,a6,200612a0 <__umoddi3+0x160>
2006129a:	00d5f363          	bgeu	a1,a3,200612a0 <__umoddi3+0x160>
2006129e:	95c2                	c.add	a1,a6
200612a0:	8d95                	c.sub	a1,a3
200612a2:	01085693          	srli	a3,a6,0x10
200612a6:	02d5f733          	remu	a4,a1,a3
200612aa:	01081793          	slli	a5,a6,0x10
200612ae:	83c1                	c.srli	a5,0x10
200612b0:	01055613          	srli	a2,a0,0x10
200612b4:	02d5d5b3          	divu	a1,a1,a3
200612b8:	0742                	c.slli	a4,0x10
200612ba:	8f51                	c.or	a4,a2
200612bc:	02b785b3          	mul	a1,a5,a1
200612c0:	00b77863          	bgeu	a4,a1,200612d0 <__umoddi3+0x190>
200612c4:	9742                	c.add	a4,a6
200612c6:	01076563          	bltu	a4,a6,200612d0 <__umoddi3+0x190>
200612ca:	00b77363          	bgeu	a4,a1,200612d0 <__umoddi3+0x190>
200612ce:	9742                	c.add	a4,a6
200612d0:	40b705b3          	sub	a1,a4,a1
200612d4:	02d5f733          	remu	a4,a1,a3
200612d8:	0542                	c.slli	a0,0x10
200612da:	8141                	c.srli	a0,0x10
200612dc:	02d5d5b3          	divu	a1,a1,a3
200612e0:	02b785b3          	mul	a1,a5,a1
200612e4:	01071793          	slli	a5,a4,0x10
200612e8:	8d5d                	c.or	a0,a5
200612ea:	00b57863          	bgeu	a0,a1,200612fa <__umoddi3+0x1ba>
200612ee:	9542                	c.add	a0,a6
200612f0:	01056563          	bltu	a0,a6,200612fa <__umoddi3+0x1ba>
200612f4:	00b57363          	bgeu	a0,a1,200612fa <__umoddi3+0x1ba>
200612f8:	9542                	c.add	a0,a6
200612fa:	8d0d                	c.sub	a0,a1
200612fc:	b5f5                	c.j	200611e8 <__umoddi3+0xa8>
200612fe:	010006b7          	lui	a3,0x1000
20061302:	47c1                	c.li	a5,16
20061304:	f0d869e3          	bltu	a6,a3,20061216 <__umoddi3+0xd6>
20061308:	47e1                	c.li	a5,24
2006130a:	b731                	c.j	20061216 <__umoddi3+0xd6>
2006130c:	eed5e1e3          	bltu	a1,a3,200611ee <__umoddi3+0xae>
20061310:	67c1                	c.lui	a5,0x10
20061312:	04f6f463          	bgeu	a3,a5,2006135a <__umoddi3+0x21a>
20061316:	0ff00893          	addi	a7,zero,255
2006131a:	00d8b7b3          	sltu	a5,a7,a3
2006131e:	078e                	c.slli	a5,0x3
20061320:	20064737          	lui	a4,0x20064
20061324:	00f6d833          	srl	a6,a3,a5
20061328:	18870713          	addi	a4,a4,392 # 20064188 <__clz_tab>
2006132c:	9742                	c.add	a4,a6
2006132e:	00074883          	lbu	a7,0(a4)
20061332:	98be                	c.add	a7,a5
20061334:	02000793          	addi	a5,zero,32
20061338:	41178833          	sub	a6,a5,a7
2006133c:	03179663          	bne	a5,a7,20061368 <__umoddi3+0x228>
20061340:	0066e463          	bltu	a3,t1,20061348 <__umoddi3+0x208>
20061344:	eacee5e3          	bltu	t4,a2,200611ee <__umoddi3+0xae>
20061348:	40ce8533          	sub	a0,t4,a2
2006134c:	40d305b3          	sub	a1,t1,a3
20061350:	00aebeb3          	sltu	t4,t4,a0
20061354:	41d585b3          	sub	a1,a1,t4
20061358:	bd59                	c.j	200611ee <__umoddi3+0xae>
2006135a:	01000737          	lui	a4,0x1000
2006135e:	47c1                	c.li	a5,16
20061360:	fce6e0e3          	bltu	a3,a4,20061320 <__umoddi3+0x1e0>
20061364:	47e1                	c.li	a5,24
20061366:	bf6d                	c.j	20061320 <__umoddi3+0x1e0>
20061368:	011657b3          	srl	a5,a2,a7
2006136c:	010696b3          	sll	a3,a3,a6
20061370:	00d7ee33          	or	t3,a5,a3
20061374:	01135f33          	srl	t5,t1,a7
20061378:	011ed7b3          	srl	a5,t4,a7
2006137c:	010e9533          	sll	a0,t4,a6
20061380:	010e5e93          	srli	t4,t3,0x10
20061384:	010315b3          	sll	a1,t1,a6
20061388:	03df5333          	divu	t1,t5,t4
2006138c:	8fcd                	c.or	a5,a1
2006138e:	010e1593          	slli	a1,t3,0x10
20061392:	81c1                	c.srli	a1,0x10
20061394:	0107d713          	srli	a4,a5,0x10
20061398:	01061633          	sll	a2,a2,a6
2006139c:	03df76b3          	remu	a3,t5,t4
200613a0:	02658f33          	mul	t5,a1,t1
200613a4:	06c2                	c.slli	a3,0x10
200613a6:	8f55                	c.or	a4,a3
200613a8:	869a                	c.mv	a3,t1
200613aa:	01e77c63          	bgeu	a4,t5,200613c2 <__umoddi3+0x282>
200613ae:	9772                	c.add	a4,t3
200613b0:	fff30693          	addi	a3,t1,-1
200613b4:	01c76763          	bltu	a4,t3,200613c2 <__umoddi3+0x282>
200613b8:	01e77563          	bgeu	a4,t5,200613c2 <__umoddi3+0x282>
200613bc:	ffe30693          	addi	a3,t1,-2
200613c0:	9772                	c.add	a4,t3
200613c2:	41e70733          	sub	a4,a4,t5
200613c6:	03d77333          	remu	t1,a4,t4
200613ca:	07c2                	c.slli	a5,0x10
200613cc:	83c1                	c.srli	a5,0x10
200613ce:	03d75733          	divu	a4,a4,t4
200613d2:	0342                	c.slli	t1,0x10
200613d4:	00f36333          	or	t1,t1,a5
200613d8:	02e585b3          	mul	a1,a1,a4
200613dc:	87ba                	c.mv	a5,a4
200613de:	00b37c63          	bgeu	t1,a1,200613f6 <__umoddi3+0x2b6>
200613e2:	9372                	c.add	t1,t3
200613e4:	fff70793          	addi	a5,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
200613e8:	01c36763          	bltu	t1,t3,200613f6 <__umoddi3+0x2b6>
200613ec:	00b37563          	bgeu	t1,a1,200613f6 <__umoddi3+0x2b6>
200613f0:	ffe70793          	addi	a5,a4,-2
200613f4:	9372                	c.add	t1,t3
200613f6:	06c2                	c.slli	a3,0x10
200613f8:	6f41                	c.lui	t5,0x10
200613fa:	8edd                	c.or	a3,a5
200613fc:	40b305b3          	sub	a1,t1,a1
20061400:	ffff0313          	addi	t1,t5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061404:	0066feb3          	and	t4,a3,t1
20061408:	01065713          	srli	a4,a2,0x10
2006140c:	82c1                	c.srli	a3,0x10
2006140e:	00667333          	and	t1,a2,t1
20061412:	026e87b3          	mul	a5,t4,t1
20061416:	02668333          	mul	t1,a3,t1
2006141a:	02ee8eb3          	mul	t4,t4,a4
2006141e:	02e686b3          	mul	a3,a3,a4
20061422:	9e9a                	c.add	t4,t1
20061424:	0107d713          	srli	a4,a5,0x10
20061428:	9776                	c.add	a4,t4
2006142a:	00677363          	bgeu	a4,t1,20061430 <__umoddi3+0x2f0>
2006142e:	96fa                	c.add	a3,t5
20061430:	01075313          	srli	t1,a4,0x10
20061434:	969a                	c.add	a3,t1
20061436:	6341                	c.lui	t1,0x10
20061438:	137d                	c.addi	t1,-1
2006143a:	00677733          	and	a4,a4,t1
2006143e:	0742                	c.slli	a4,0x10
20061440:	0067f7b3          	and	a5,a5,t1
20061444:	97ba                	c.add	a5,a4
20061446:	00d5e663          	bltu	a1,a3,20061452 <__umoddi3+0x312>
2006144a:	00d59b63          	bne	a1,a3,20061460 <__umoddi3+0x320>
2006144e:	00f57963          	bgeu	a0,a5,20061460 <__umoddi3+0x320>
20061452:	40c78633          	sub	a2,a5,a2
20061456:	00c7b7b3          	sltu	a5,a5,a2
2006145a:	97f2                	c.add	a5,t3
2006145c:	8e9d                	c.sub	a3,a5
2006145e:	87b2                	c.mv	a5,a2
20061460:	40f507b3          	sub	a5,a0,a5
20061464:	00f53533          	sltu	a0,a0,a5
20061468:	8d95                	c.sub	a1,a3
2006146a:	8d89                	c.sub	a1,a0
2006146c:	011598b3          	sll	a7,a1,a7
20061470:	0107d533          	srl	a0,a5,a6
20061474:	00a8e533          	or	a0,a7,a0
20061478:	0105d5b3          	srl	a1,a1,a6
2006147c:	bb8d                	c.j	200611ee <__umoddi3+0xae>

2006147e <atoi>:
2006147e:	4629                	c.li	a2,10
20061480:	4581                	c.li	a1,0
20061482:	aef5                	c.j	2006187e <strtol>

20061484 <_atoi_r>:
20061484:	46a9                	c.li	a3,10
20061486:	4601                	c.li	a2,0
20061488:	a6e5                	c.j	20061870 <_strtol_r>

2006148a <__errno>:
2006148a:	ea41a503          	lw	a0,-348(gp) # 20065f7c <_impure_ptr>
2006148e:	8082                	c.jr	ra

20061490 <__libc_init_array>:
20061490:	1141                	c.addi	sp,-16
20061492:	c422                	c.swsp	s0,8(sp)
20061494:	c226                	c.swsp	s1,4(sp)
20061496:	20064437          	lui	s0,0x20064
2006149a:	200644b7          	lui	s1,0x20064
2006149e:	50448793          	addi	a5,s1,1284 # 20064504 <__frame_dummy_init_array_entry>
200614a2:	50440413          	addi	s0,s0,1284 # 20064504 <__frame_dummy_init_array_entry>
200614a6:	8c1d                	c.sub	s0,a5
200614a8:	c04a                	c.swsp	s2,0(sp)
200614aa:	c606                	c.swsp	ra,12(sp)
200614ac:	8409                	c.srai	s0,0x2
200614ae:	50448493          	addi	s1,s1,1284
200614b2:	4901                	c.li	s2,0
200614b4:	02891763          	bne	s2,s0,200614e2 <__libc_init_array+0x52>
200614b8:	200644b7          	lui	s1,0x20064
200614bc:	20064437          	lui	s0,0x20064
200614c0:	50448793          	addi	a5,s1,1284 # 20064504 <__frame_dummy_init_array_entry>
200614c4:	50840413          	addi	s0,s0,1288 # 20064508 <__do_global_dtors_aux_fini_array_entry>
200614c8:	8c1d                	c.sub	s0,a5
200614ca:	8409                	c.srai	s0,0x2
200614cc:	50448493          	addi	s1,s1,1284
200614d0:	4901                	c.li	s2,0
200614d2:	00891d63          	bne	s2,s0,200614ec <__libc_init_array+0x5c>
200614d6:	40b2                	c.lwsp	ra,12(sp)
200614d8:	4422                	c.lwsp	s0,8(sp)
200614da:	4492                	c.lwsp	s1,4(sp)
200614dc:	4902                	c.lwsp	s2,0(sp)
200614de:	0141                	c.addi	sp,16
200614e0:	8082                	c.jr	ra
200614e2:	409c                	c.lw	a5,0(s1)
200614e4:	0905                	c.addi	s2,1
200614e6:	0491                	c.addi	s1,4
200614e8:	9782                	c.jalr	a5
200614ea:	b7e9                	c.j	200614b4 <__libc_init_array+0x24>
200614ec:	409c                	c.lw	a5,0(s1)
200614ee:	0905                	c.addi	s2,1
200614f0:	0491                	c.addi	s1,4
200614f2:	9782                	c.jalr	a5
200614f4:	bff9                	c.j	200614d2 <__libc_init_array+0x42>

200614f6 <cleanup_glue>:
200614f6:	1141                	c.addi	sp,-16
200614f8:	c422                	c.swsp	s0,8(sp)
200614fa:	842e                	c.mv	s0,a1
200614fc:	418c                	c.lw	a1,0(a1)
200614fe:	c226                	c.swsp	s1,4(sp)
20061500:	c606                	c.swsp	ra,12(sp)
20061502:	84aa                	c.mv	s1,a0
20061504:	c191                	c.beqz	a1,20061508 <cleanup_glue+0x12>
20061506:	3fc5                	c.jal	200614f6 <cleanup_glue>
20061508:	85a2                	c.mv	a1,s0
2006150a:	4422                	c.lwsp	s0,8(sp)
2006150c:	40b2                	c.lwsp	ra,12(sp)
2006150e:	8526                	c.mv	a0,s1
20061510:	4492                	c.lwsp	s1,4(sp)
20061512:	0141                	c.addi	sp,16
20061514:	ba6f906f          	jal	zero,2005a8ba <__wrap__free_r>

20061518 <_reclaim_reent>:
20061518:	ea41a783          	lw	a5,-348(gp) # 20065f7c <_impure_ptr>
2006151c:	0ea78263          	beq	a5,a0,20061600 <_reclaim_reent+0xe8>
20061520:	515c                	c.lw	a5,36(a0)
20061522:	1101                	c.addi	sp,-32
20061524:	cc22                	c.swsp	s0,24(sp)
20061526:	ce06                	c.swsp	ra,28(sp)
20061528:	ca26                	c.swsp	s1,20(sp)
2006152a:	c84a                	c.swsp	s2,16(sp)
2006152c:	c64e                	c.swsp	s3,12(sp)
2006152e:	842a                	c.mv	s0,a0
20061530:	cf81                	c.beqz	a5,20061548 <_reclaim_reent+0x30>
20061532:	47dc                	c.lw	a5,12(a5)
20061534:	4481                	c.li	s1,0
20061536:	08000913          	addi	s2,zero,128
2006153a:	efc9                	c.bnez	a5,200615d4 <_reclaim_reent+0xbc>
2006153c:	505c                	c.lw	a5,36(s0)
2006153e:	438c                	c.lw	a1,0(a5)
20061540:	c581                	c.beqz	a1,20061548 <_reclaim_reent+0x30>
20061542:	8522                	c.mv	a0,s0
20061544:	b76f90ef          	jal	ra,2005a8ba <__wrap__free_r>
20061548:	484c                	c.lw	a1,20(s0)
2006154a:	c581                	c.beqz	a1,20061552 <_reclaim_reent+0x3a>
2006154c:	8522                	c.mv	a0,s0
2006154e:	b6cf90ef          	jal	ra,2005a8ba <__wrap__free_r>
20061552:	504c                	c.lw	a1,36(s0)
20061554:	c581                	c.beqz	a1,2006155c <_reclaim_reent+0x44>
20061556:	8522                	c.mv	a0,s0
20061558:	b62f90ef          	jal	ra,2005a8ba <__wrap__free_r>
2006155c:	5c0c                	c.lw	a1,56(s0)
2006155e:	c581                	c.beqz	a1,20061566 <_reclaim_reent+0x4e>
20061560:	8522                	c.mv	a0,s0
20061562:	b58f90ef          	jal	ra,2005a8ba <__wrap__free_r>
20061566:	5c4c                	c.lw	a1,60(s0)
20061568:	c581                	c.beqz	a1,20061570 <_reclaim_reent+0x58>
2006156a:	8522                	c.mv	a0,s0
2006156c:	b4ef90ef          	jal	ra,2005a8ba <__wrap__free_r>
20061570:	402c                	c.lw	a1,64(s0)
20061572:	c581                	c.beqz	a1,2006157a <_reclaim_reent+0x62>
20061574:	8522                	c.mv	a0,s0
20061576:	b44f90ef          	jal	ra,2005a8ba <__wrap__free_r>
2006157a:	0ec42583          	lw	a1,236(s0)
2006157e:	c581                	c.beqz	a1,20061586 <_reclaim_reent+0x6e>
20061580:	8522                	c.mv	a0,s0
20061582:	b38f90ef          	jal	ra,2005a8ba <__wrap__free_r>
20061586:	0e842583          	lw	a1,232(s0)
2006158a:	c581                	c.beqz	a1,20061592 <_reclaim_reent+0x7a>
2006158c:	8522                	c.mv	a0,s0
2006158e:	b2cf90ef          	jal	ra,2005a8ba <__wrap__free_r>
20061592:	443c                	c.lw	a5,72(s0)
20061594:	c799                	c.beqz	a5,200615a2 <_reclaim_reent+0x8a>
20061596:	0887a583          	lw	a1,136(a5) # 10088 <mbedtls_ecp_check_pub_priv+0x1aa>
2006159a:	c581                	c.beqz	a1,200615a2 <_reclaim_reent+0x8a>
2006159c:	8522                	c.mv	a0,s0
2006159e:	b1cf90ef          	jal	ra,2005a8ba <__wrap__free_r>
200615a2:	584c                	c.lw	a1,52(s0)
200615a4:	c581                	c.beqz	a1,200615ac <_reclaim_reent+0x94>
200615a6:	8522                	c.mv	a0,s0
200615a8:	b12f90ef          	jal	ra,2005a8ba <__wrap__free_r>
200615ac:	4c1c                	c.lw	a5,24(s0)
200615ae:	c3b1                	c.beqz	a5,200615f2 <_reclaim_reent+0xda>
200615b0:	541c                	c.lw	a5,40(s0)
200615b2:	8522                	c.mv	a0,s0
200615b4:	9782                	c.jalr	a5
200615b6:	0d842583          	lw	a1,216(s0)
200615ba:	cd85                	c.beqz	a1,200615f2 <_reclaim_reent+0xda>
200615bc:	8522                	c.mv	a0,s0
200615be:	4462                	c.lwsp	s0,24(sp)
200615c0:	40f2                	c.lwsp	ra,28(sp)
200615c2:	44d2                	c.lwsp	s1,20(sp)
200615c4:	4942                	c.lwsp	s2,16(sp)
200615c6:	49b2                	c.lwsp	s3,12(sp)
200615c8:	6105                	c.addi16sp	sp,32
200615ca:	b735                	c.j	200614f6 <cleanup_glue>
200615cc:	95a6                	c.add	a1,s1
200615ce:	418c                	c.lw	a1,0(a1)
200615d0:	e991                	c.bnez	a1,200615e4 <_reclaim_reent+0xcc>
200615d2:	0491                	c.addi	s1,4
200615d4:	505c                	c.lw	a5,36(s0)
200615d6:	47cc                	c.lw	a1,12(a5)
200615d8:	ff249ae3          	bne	s1,s2,200615cc <_reclaim_reent+0xb4>
200615dc:	8522                	c.mv	a0,s0
200615de:	adcf90ef          	jal	ra,2005a8ba <__wrap__free_r>
200615e2:	bfa9                	c.j	2006153c <_reclaim_reent+0x24>
200615e4:	0005a983          	lw	s3,0(a1)
200615e8:	8522                	c.mv	a0,s0
200615ea:	ad0f90ef          	jal	ra,2005a8ba <__wrap__free_r>
200615ee:	85ce                	c.mv	a1,s3
200615f0:	b7c5                	c.j	200615d0 <_reclaim_reent+0xb8>
200615f2:	40f2                	c.lwsp	ra,28(sp)
200615f4:	4462                	c.lwsp	s0,24(sp)
200615f6:	44d2                	c.lwsp	s1,20(sp)
200615f8:	4942                	c.lwsp	s2,16(sp)
200615fa:	49b2                	c.lwsp	s3,12(sp)
200615fc:	6105                	c.addi16sp	sp,32
200615fe:	8082                	c.jr	ra
20061600:	8082                	c.jr	ra

20061602 <strcpy>:
20061602:	00b56733          	or	a4,a0,a1
20061606:	8b0d                	c.andi	a4,3
20061608:	87aa                	c.mv	a5,a0
2006160a:	e729                	c.bnez	a4,20061654 <strcpy+0x52>
2006160c:	7f7f86b7          	lui	a3,0x7f7f8
20061610:	f7f68693          	addi	a3,a3,-129 # 7f7f7f7f <__kr4_psram_text_start__+0x1f677f5f>
20061614:	587d                	c.li	a6,-1
20061616:	4190                	c.lw	a2,0(a1)
20061618:	00d67733          	and	a4,a2,a3
2006161c:	9736                	c.add	a4,a3
2006161e:	8f51                	c.or	a4,a2
20061620:	8f55                	c.or	a4,a3
20061622:	03070463          	beq	a4,a6,2006164a <strcpy+0x48>
20061626:	0005c603          	lbu	a2,0(a1)
2006162a:	0015c683          	lbu	a3,1(a1)
2006162e:	0025c703          	lbu	a4,2(a1)
20061632:	00c78023          	sb	a2,0(a5)
20061636:	c615                	c.beqz	a2,20061662 <strcpy+0x60>
20061638:	00d780a3          	sb	a3,1(a5)
2006163c:	c29d                	c.beqz	a3,20061662 <strcpy+0x60>
2006163e:	00e78123          	sb	a4,2(a5)
20061642:	c305                	c.beqz	a4,20061662 <strcpy+0x60>
20061644:	000781a3          	sb	zero,3(a5)
20061648:	8082                	c.jr	ra
2006164a:	0791                	c.addi	a5,4
2006164c:	0591                	c.addi	a1,4
2006164e:	fec7ae23          	sw	a2,-4(a5)
20061652:	b7d1                	c.j	20061616 <strcpy+0x14>
20061654:	0005c703          	lbu	a4,0(a1)
20061658:	0785                	c.addi	a5,1
2006165a:	0585                	c.addi	a1,1
2006165c:	fee78fa3          	sb	a4,-1(a5)
20061660:	fb75                	c.bnez	a4,20061654 <strcpy+0x52>
20061662:	8082                	c.jr	ra

20061664 <strncpy>:
20061664:	00a5e733          	or	a4,a1,a0
20061668:	8b0d                	c.andi	a4,3
2006166a:	87aa                	c.mv	a5,a0
2006166c:	e701                	c.bnez	a4,20061674 <strncpy+0x10>
2006166e:	470d                	c.li	a4,3
20061670:	04c76163          	bltu	a4,a2,200616b2 <strncpy+0x4e>
20061674:	ca09                	c.beqz	a2,20061686 <strncpy+0x22>
20061676:	0005c703          	lbu	a4,0(a1)
2006167a:	0585                	c.addi	a1,1
2006167c:	0785                	c.addi	a5,1
2006167e:	fee78fa3          	sb	a4,-1(a5)
20061682:	167d                	c.addi	a2,-1
20061684:	fb65                	c.bnez	a4,20061674 <strncpy+0x10>
20061686:	963e                	c.add	a2,a5
20061688:	02f61f63          	bne	a2,a5,200616c6 <strncpy+0x62>
2006168c:	8082                	c.jr	ra
2006168e:	1671                	c.addi	a2,-4
20061690:	0107a023          	sw	a6,0(a5)
20061694:	0791                	c.addi	a5,4
20061696:	0591                	c.addi	a1,4
20061698:	fcce7ee3          	bgeu	t3,a2,20061674 <strncpy+0x10>
2006169c:	0005a803          	lw	a6,0(a1)
200616a0:	01180733          	add	a4,a6,a7
200616a4:	fff84313          	xori	t1,a6,-1
200616a8:	00677733          	and	a4,a4,t1
200616ac:	8f75                	c.and	a4,a3
200616ae:	d365                	c.beqz	a4,2006168e <strncpy+0x2a>
200616b0:	b7d1                	c.j	20061674 <strncpy+0x10>
200616b2:	feff08b7          	lui	a7,0xfeff0
200616b6:	808086b7          	lui	a3,0x80808
200616ba:	eff88893          	addi	a7,a7,-257 # fefefeff <__ctrace_end__+0x34fefeff>
200616be:	08068693          	addi	a3,a3,128 # 80808080 <__ctrace_end__+0xb6808080>
200616c2:	4e0d                	c.li	t3,3
200616c4:	bfe1                	c.j	2006169c <strncpy+0x38>
200616c6:	0785                	c.addi	a5,1
200616c8:	fe078fa3          	sb	zero,-1(a5)
200616cc:	bf75                	c.j	20061688 <strncpy+0x24>

200616ce <strsep>:
200616ce:	862a                	c.mv	a2,a0
200616d0:	4108                	c.lw	a0,0(a0)
200616d2:	4681                	c.li	a3,0
200616d4:	a009                	c.j	200616d6 <__strtok_r>

200616d6 <__strtok_r>:
200616d6:	e119                	c.bnez	a0,200616dc <__strtok_r+0x6>
200616d8:	4208                	c.lw	a0,0(a2)
200616da:	c939                	c.beqz	a0,20061730 <__strtok_r+0x5a>
200616dc:	87aa                	c.mv	a5,a0
200616de:	853e                	c.mv	a0,a5
200616e0:	0007c803          	lbu	a6,0(a5)
200616e4:	0785                	c.addi	a5,1
200616e6:	872e                	c.mv	a4,a1
200616e8:	00074883          	lbu	a7,0(a4)
200616ec:	0705                	c.addi	a4,1
200616ee:	00089863          	bne	a7,zero,200616fe <__strtok_r+0x28>
200616f2:	02081063          	bne	a6,zero,20061712 <__strtok_r+0x3c>
200616f6:	00062023          	sw	zero,0(a2)
200616fa:	4501                	c.li	a0,0
200616fc:	8082                	c.jr	ra
200616fe:	ff1815e3          	bne	a6,a7,200616e8 <__strtok_r+0x12>
20061702:	fef1                	c.bnez	a3,200616de <__strtok_r+0x8>
20061704:	c21c                	c.sw	a5,0(a2)
20061706:	00050023          	sb	zero,0(a0)
2006170a:	8082                	c.jr	ra
2006170c:	4781                	c.li	a5,0
2006170e:	a839                	c.j	2006172c <__strtok_r+0x56>
20061710:	e689                	c.bnez	a3,2006171a <__strtok_r+0x44>
20061712:	0007c803          	lbu	a6,0(a5)
20061716:	0785                	c.addi	a5,1
20061718:	872e                	c.mv	a4,a1
2006171a:	00074683          	lbu	a3,0(a4)
2006171e:	0705                	c.addi	a4,1
20061720:	fed818e3          	bne	a6,a3,20061710 <__strtok_r+0x3a>
20061724:	fe0804e3          	beq	a6,zero,2006170c <__strtok_r+0x36>
20061728:	fe078fa3          	sb	zero,-1(a5)
2006172c:	c21c                	c.sw	a5,0(a2)
2006172e:	8082                	c.jr	ra
20061730:	8082                	c.jr	ra

20061732 <strtok_r>:
20061732:	4685                	c.li	a3,1
20061734:	b74d                	c.j	200616d6 <__strtok_r>

20061736 <_strtol_l.constprop.0>:
20061736:	4785                	c.li	a5,1
20061738:	0af68863          	beq	a3,a5,200617e8 <_strtol_l.constprop.0+0xb2>
2006173c:	02400793          	addi	a5,zero,36
20061740:	872e                	c.mv	a4,a1
20061742:	0ad7e363          	bltu	a5,a3,200617e8 <_strtol_l.constprop.0+0xb2>
20061746:	20064837          	lui	a6,0x20064
2006174a:	8eaa                	c.mv	t4,a0
2006174c:	28980813          	addi	a6,a6,649 # 20064289 <_ctype_+0x1>
20061750:	88ba                	c.mv	a7,a4
20061752:	00074783          	lbu	a5,0(a4)
20061756:	0705                	c.addi	a4,1
20061758:	01078533          	add	a0,a5,a6
2006175c:	00054503          	lbu	a0,0(a0)
20061760:	8921                	c.andi	a0,8
20061762:	f57d                	c.bnez	a0,20061750 <_strtol_l.constprop.0+0x1a>
20061764:	02d00513          	addi	a0,zero,45
20061768:	08a79a63          	bne	a5,a0,200617fc <_strtol_l.constprop.0+0xc6>
2006176c:	00074783          	lbu	a5,0(a4)
20061770:	4e05                	c.li	t3,1
20061772:	00288713          	addi	a4,a7,2
20061776:	c6f5                	c.beqz	a3,20061862 <_strtol_l.constprop.0+0x12c>
20061778:	4541                	c.li	a0,16
2006177a:	02a69263          	bne	a3,a0,2006179e <_strtol_l.constprop.0+0x68>
2006177e:	03000513          	addi	a0,zero,48
20061782:	00a79e63          	bne	a5,a0,2006179e <_strtol_l.constprop.0+0x68>
20061786:	00074783          	lbu	a5,0(a4)
2006178a:	05800513          	addi	a0,zero,88
2006178e:	0df7f793          	andi	a5,a5,223
20061792:	0ca79363          	bne	a5,a0,20061858 <_strtol_l.constprop.0+0x122>
20061796:	00174783          	lbu	a5,1(a4)
2006179a:	46c1                	c.li	a3,16
2006179c:	0709                	c.addi	a4,2
2006179e:	800008b7          	lui	a7,0x80000
200617a2:	fff8c893          	xori	a7,a7,-1
200617a6:	98f2                	c.add	a7,t3
200617a8:	02d8f2b3          	remu	t0,a7,a3
200617ac:	4301                	c.li	t1,0
200617ae:	4501                	c.li	a0,0
200617b0:	43a5                	c.li	t2,9
200617b2:	4fe5                	c.li	t6,25
200617b4:	02d8df33          	divu	t5,a7,a3
200617b8:	fd078813          	addi	a6,a5,-48
200617bc:	0503ea63          	bltu	t2,a6,20061810 <_strtol_l.constprop.0+0xda>
200617c0:	87c2                	c.mv	a5,a6
200617c2:	06d7d763          	bge	a5,a3,20061830 <_strtol_l.constprop.0+0xfa>
200617c6:	06034363          	blt	t1,zero,2006182c <_strtol_l.constprop.0+0xf6>
200617ca:	537d                	c.li	t1,-1
200617cc:	00af6a63          	bltu	t5,a0,200617e0 <_strtol_l.constprop.0+0xaa>
200617d0:	00af1463          	bne	t5,a0,200617d8 <_strtol_l.constprop.0+0xa2>
200617d4:	00f2c663          	blt	t0,a5,200617e0 <_strtol_l.constprop.0+0xaa>
200617d8:	4305                	c.li	t1,1
200617da:	02a68533          	mul	a0,a3,a0
200617de:	953e                	c.add	a0,a5
200617e0:	0705                	c.addi	a4,1
200617e2:	fff74783          	lbu	a5,-1(a4)
200617e6:	bfc9                	c.j	200617b8 <_strtol_l.constprop.0+0x82>
200617e8:	1141                	c.addi	sp,-16
200617ea:	c606                	c.swsp	ra,12(sp)
200617ec:	c9fff0ef          	jal	ra,2006148a <__errno>
200617f0:	40b2                	c.lwsp	ra,12(sp)
200617f2:	47d9                	c.li	a5,22
200617f4:	c11c                	c.sw	a5,0(a0)
200617f6:	4501                	c.li	a0,0
200617f8:	0141                	c.addi	sp,16
200617fa:	8082                	c.jr	ra
200617fc:	02b00513          	addi	a0,zero,43
20061800:	4e01                	c.li	t3,0
20061802:	f6a79ae3          	bne	a5,a0,20061776 <_strtol_l.constprop.0+0x40>
20061806:	00074783          	lbu	a5,0(a4)
2006180a:	00288713          	addi	a4,a7,2 # 80000002 <__ctrace_end__+0xb6000002>
2006180e:	b7a5                	c.j	20061776 <_strtol_l.constprop.0+0x40>
20061810:	fbf78813          	addi	a6,a5,-65
20061814:	010fe563          	bltu	t6,a6,2006181e <_strtol_l.constprop.0+0xe8>
20061818:	fc978793          	addi	a5,a5,-55
2006181c:	b75d                	c.j	200617c2 <_strtol_l.constprop.0+0x8c>
2006181e:	f9f78813          	addi	a6,a5,-97
20061822:	010fe763          	bltu	t6,a6,20061830 <_strtol_l.constprop.0+0xfa>
20061826:	fa978793          	addi	a5,a5,-87
2006182a:	bf61                	c.j	200617c2 <_strtol_l.constprop.0+0x8c>
2006182c:	537d                	c.li	t1,-1
2006182e:	bf4d                	c.j	200617e0 <_strtol_l.constprop.0+0xaa>
20061830:	00035963          	bge	t1,zero,20061842 <_strtol_l.constprop.0+0x10c>
20061834:	02200793          	addi	a5,zero,34
20061838:	00fea023          	sw	a5,0(t4)
2006183c:	8546                	c.mv	a0,a7
2006183e:	ea09                	c.bnez	a2,20061850 <_strtol_l.constprop.0+0x11a>
20061840:	8082                	c.jr	ra
20061842:	000e0463          	beq	t3,zero,2006184a <_strtol_l.constprop.0+0x114>
20061846:	40a00533          	sub	a0,zero,a0
2006184a:	c215                	c.beqz	a2,2006186e <_strtol_l.constprop.0+0x138>
2006184c:	00030463          	beq	t1,zero,20061854 <_strtol_l.constprop.0+0x11e>
20061850:	fff70593          	addi	a1,a4,-1
20061854:	c20c                	c.sw	a1,0(a2)
20061856:	8082                	c.jr	ra
20061858:	03000793          	addi	a5,zero,48
2006185c:	f2a9                	c.bnez	a3,2006179e <_strtol_l.constprop.0+0x68>
2006185e:	46a1                	c.li	a3,8
20061860:	bf3d                	c.j	2006179e <_strtol_l.constprop.0+0x68>
20061862:	03000513          	addi	a0,zero,48
20061866:	f2a780e3          	beq	a5,a0,20061786 <_strtol_l.constprop.0+0x50>
2006186a:	46a9                	c.li	a3,10
2006186c:	bf0d                	c.j	2006179e <_strtol_l.constprop.0+0x68>
2006186e:	8082                	c.jr	ra

20061870 <_strtol_r>:
20061870:	b5d9                	c.j	20061736 <_strtol_l.constprop.0>

20061872 <strtol_l>:
20061872:	86b2                	c.mv	a3,a2
20061874:	862e                	c.mv	a2,a1
20061876:	85aa                	c.mv	a1,a0
20061878:	ea41a503          	lw	a0,-348(gp) # 20065f7c <_impure_ptr>
2006187c:	bd6d                	c.j	20061736 <_strtol_l.constprop.0>

2006187e <strtol>:
2006187e:	86b2                	c.mv	a3,a2
20061880:	862e                	c.mv	a2,a1
20061882:	85aa                	c.mv	a1,a0
20061884:	ea41a503          	lw	a0,-348(gp) # 20065f7c <_impure_ptr>
20061888:	b57d                	c.j	20061736 <_strtol_l.constprop.0>
2006188a:	0000                	c.unimp
2006188c:	6d6f682f          	0x6d6f682f
20061890:	2f65                	c.jal	20062048 <pmap_func+0x3a0>
20061892:	7561                	c.lui	a0,0xffff8
20061894:	6f74                	c.flw	fa3,92(a4)
20061896:	6a5f 6e65 696b      	0x696b6e656a5f
2006189c:	736e                	c.flwsp	ft6,248(sp)
2006189e:	775f 616c 356e      	0x356e616c775f
200618a4:	544f492f          	0x544f492f
200618a8:	415f 5354 722f      	0x722f5354415f
200618ae:	7065                	c.lui	zero,0xffff9
200618b0:	722f736f          	jal	t1,20158fd2 <__ap_sram_heap_start+0xd8fd2>
200618b4:	6c65                	c.lui	s8,0x19
200618b6:	6165                	c.addi16sp	sp,112
200618b8:	6d2d6573          	csrrsi	a0,0x6d2,26
200618bc:	7361                	c.lui	t1,0xffff8
200618be:	6574                	c.flw	fa3,76(a0)
200618c0:	2f72                	c.fldsp	ft10,280(sp)
200618c2:	72756f73          	csrrsi	t5,0x727,10
200618c6:	612f6563          	bltu	t5,s2,20061ed0 <pmap_func+0x228>
200618ca:	656d                	c.lui	a0,0x1b
200618cc:	6162                	c.flwsp	ft2,24(sp)
200618ce:	696c                	c.flw	fa1,84(a0)
200618d0:	6574                	c.flw	fa3,76(a0)
200618d2:	675f 6363 705f      	0x705f6363675f
200618d8:	6f72                	c.flwsp	ft10,28(sp)
200618da:	656a                	c.flwsp	fa0,152(sp)
200618dc:	702f7463          	bgeu	t5,sp,20061fe4 <pmap_func+0x33c>
200618e0:	6f72                	c.flwsp	ft10,28(sp)
200618e2:	656a                	c.flwsp	fa0,152(sp)
200618e4:	6b5f7463          	bgeu	t5,s5,20061f8c <pmap_func+0x2e4>
200618e8:	3472                	c.fldsp	fs0,312(sp)
200618ea:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
200618ee:	2e2e2f6b          	0x2e2e2f6b
200618f2:	2f2e2e2f          	0x2f2e2e2f
200618f6:	2e2e                	c.fldsp	ft8,200(sp)
200618f8:	6d6f632f          	0x6d6f632f
200618fc:	6f70                	c.flw	fa2,92(a4)
200618fe:	656e                	c.flwsp	fa0,216(sp)
20061900:	746e                	c.flwsp	fs0,248(sp)
20061902:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
20061906:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
2006190a:	6162                	c.flwsp	ft2,24(sp)
2006190c:	696c                	c.flw	fa1,84(a0)
2006190e:	6574                	c.flw	fa3,76(a0)
20061910:	6c77662f          	0x6c77662f
20061914:	6269                	c.lui	tp,0x1a
20061916:	6d61722f          	0x6d61722f
2006191a:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20061920:	2f6e                	c.fldsp	ft10,216(sp)
20061922:	6d61                	c.lui	s10,0x18
20061924:	6265                	c.lui	tp,0x19
20061926:	5f61                	c.li	t5,-8
20061928:	6c66                	c.flwsp	fs8,88(sp)
2006192a:	7361                	c.lui	t1,0xffff8
2006192c:	5f68                	c.lw	a0,124(a4)
2006192e:	6172                	c.flwsp	ft2,28(sp)
20061930:	2e6d                	c.jal	20061cea <pmap_func+0x42>
20061932:	68430063          	beq	t1,tp,20061fb2 <pmap_func+0x30a>
20061936:	6e61                	c.lui	t3,0x18
20061938:	656e                	c.flwsp	fa0,216(sp)
2006193a:	206c                	c.fld	fa1,192(s0)
2006193c:	666e6f43          	fmadd.q	ft10,ft8,ft6,fa2,unknown
20061940:	696c                	c.flw	fa1,84(a0)
20061942:	66207463          	bgeu	zero,sp,20061faa <pmap_func+0x302>
20061946:	4320726f          	jal	tp,20068d78 <__bdram_heap_buffer_start__+0xc78>
2006194a:	5550                	c.lw	a2,44(a0)
2006194c:	2520                	c.fld	fs0,72(a0)
2006194e:	756c                	c.flw	fa1,108(a0)
20061950:	4320                	c.lw	s0,64(a4)
20061952:	6168                	c.flw	fa0,68(a0)
20061954:	6e6e                	c.flwsp	ft8,216(sp)
20061956:	6c65                	c.lui	s8,0x19
20061958:	2520                	c.fld	fs0,72(a0)
2006195a:	756c                	c.flw	fa1,108(a0)
2006195c:	2120                	c.fld	fs0,64(a0)
2006195e:	4920                	c.lw	s0,80(a0)
20061960:	726f6e67          	0x726f6e67
20061964:	2065                	c.jal	20061a0c <strtol+0x18e>
20061966:	6649                	c.lui	a2,0x12
20061968:	4320                	c.lw	s0,64(a4)
2006196a:	5550                	c.lw	a2,44(a0)
2006196c:	4820                	c.lw	s0,80(s0)
2006196e:	7361                	c.lui	t1,0xffff8
20061970:	5220                	c.lw	s0,96(a2)
20061972:	7365                	c.lui	t1,0xffff9
20061974:	7465                	c.lui	s0,0xffff9
20061976:	0a0d                	c.addi	s4,3
20061978:	0000                	c.unimp
2006197a:	0000                	c.unimp
2006197c:	5049                	c.li	zero,-14
2006197e:	49200043          	fmadd.s	ft0,ft0,fs2,fs1,rne
20061982:	4350                	c.lw	a2,4(a4)
20061984:	5220                	c.lw	s0,96(a2)
20061986:	7165                	c.addi16sp	sp,-400
20061988:	6575                	c.lui	a0,0x1d
2006198a:	54207473          	csrrci	s0,0x542,0
2006198e:	6d69                	c.lui	s10,0x1a
20061990:	6f65                	c.lui	t5,0x19
20061992:	7475                	c.lui	s0,0xffffd
20061994:	0a0d                	c.addi	s4,3
20061996:	0000                	c.unimp
20061998:	4920                	c.lw	s0,80(a0)
2006199a:	4350                	c.lw	a2,4(a4)
2006199c:	4720                	c.lw	s0,72(a4)
2006199e:	7465                	c.lui	s0,0xffff9
200619a0:	5320                	c.lw	s0,96(a4)
200619a2:	6d65                	c.lui	s10,0x19
200619a4:	7061                	c.lui	zero,0xffff8
200619a6:	6f68                	c.flw	fa0,92(a4)
200619a8:	6572                	c.flwsp	fa0,28(sp)
200619aa:	5420                	c.lw	s0,104(s0)
200619ac:	6d69                	c.lui	s10,0x1a
200619ae:	6f65                	c.lui	t5,0x19
200619b0:	7475                	c.lui	s0,0xffffd
200619b2:	0a0d                	c.addi	s4,3
200619b4:	0000                	c.unimp
200619b6:	0000                	c.unimp
200619b8:	6d6f682f          	0x6d6f682f
200619bc:	2f65                	c.jal	20062174 <pmap_func+0x4cc>
200619be:	7561                	c.lui	a0,0xffff8
200619c0:	6f74                	c.flw	fa3,92(a4)
200619c2:	6a5f 6e65 696b      	0x696b6e656a5f
200619c8:	736e                	c.flwsp	ft6,248(sp)
200619ca:	775f 616c 356e      	0x356e616c775f
200619d0:	544f492f          	0x544f492f
200619d4:	415f 5354 722f      	0x722f5354415f
200619da:	7065                	c.lui	zero,0xffff9
200619dc:	722f736f          	jal	t1,201590fe <__ap_sram_heap_start+0xd90fe>
200619e0:	6c65                	c.lui	s8,0x19
200619e2:	6165                	c.addi16sp	sp,112
200619e4:	6d2d6573          	csrrsi	a0,0x6d2,26
200619e8:	7361                	c.lui	t1,0xffff8
200619ea:	6574                	c.flw	fa3,76(a0)
200619ec:	2f72                	c.fldsp	ft10,280(sp)
200619ee:	72756f73          	csrrsi	t5,0x727,10
200619f2:	612f6563          	bltu	t5,s2,20061ffc <pmap_func+0x354>
200619f6:	656d                	c.lui	a0,0x1b
200619f8:	6162                	c.flwsp	ft2,24(sp)
200619fa:	696c                	c.flw	fa1,84(a0)
200619fc:	6574                	c.flw	fa3,76(a0)
200619fe:	675f 6363 705f      	0x705f6363675f
20061a04:	6f72                	c.flwsp	ft10,28(sp)
20061a06:	656a                	c.flwsp	fa0,152(sp)
20061a08:	702f7463          	bgeu	t5,sp,20062110 <pmap_func+0x468>
20061a0c:	6f72                	c.flwsp	ft10,28(sp)
20061a0e:	656a                	c.flwsp	fa0,152(sp)
20061a10:	6b5f7463          	bgeu	t5,s5,200620b8 <pmap_func+0x410>
20061a14:	3472                	c.fldsp	fs0,312(sp)
20061a16:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
20061a1a:	2e2e2f6b          	0x2e2e2f6b
20061a1e:	2f2e2e2f          	0x2f2e2e2f
20061a22:	2e2e                	c.fldsp	ft8,200(sp)
20061a24:	6d6f632f          	0x6d6f632f
20061a28:	6f70                	c.flw	fa2,92(a4)
20061a2a:	656e                	c.flwsp	fa0,216(sp)
20061a2c:	746e                	c.flwsp	fs0,248(sp)
20061a2e:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
20061a32:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
20061a36:	6162                	c.flwsp	ft2,24(sp)
20061a38:	696c                	c.flw	fa1,84(a0)
20061a3a:	6574                	c.flw	fa3,76(a0)
20061a3c:	6c77662f          	0x6c77662f
20061a40:	6269                	c.lui	tp,0x1a
20061a42:	6d61722f          	0x6d61722f
20061a46:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20061a4c:	2f6e                	c.fldsp	ft10,216(sp)
20061a4e:	6d61                	c.lui	s10,0x18
20061a50:	6265                	c.lui	tp,0x19
20061a52:	5f61                	c.li	t5,-8
20061a54:	7069                	c.lui	zero,0xffffa
20061a56:	70615f63          	bge	sp,t1,20062174 <pmap_func+0x4cc>
20061a5a:	2e69                	c.jal	20061df4 <pmap_func+0x14c>
20061a5c:	00000063          	beq	zero,zero,20061a5c <strtol+0x1de>
20061a60:	5049                	c.li	zero,-14
20061a62:	69445f43          	fmadd.s	ft10,fs0,fs4,fa3,unknown
20061a66:	3a72                	c.fldsp	fs4,312(sp)
20061a68:	6425                	c.lui	s0,0x9
20061a6a:	202c                	c.fld	fa1,64(s0)
20061a6c:	5049                	c.li	zero,-14
20061a6e:	58545f43          	fmadd.s	ft10,fs0,ft5,fa1,unknown
20061a72:	445f 5441 2041      	0x20415441445f
20061a78:	7830                	c.flw	fa2,112(s0)
20061a7a:	7825                	c.lui	a6,0xfffe9
20061a7c:	4e20                	c.lw	s0,88(a2)
20061a7e:	4320746f          	jal	s0,20068eb0 <__bdram_heap_buffer_start__+0xdb0>
20061a82:	656c                	c.flw	fa1,76(a0)
20061a84:	6e61                	c.lui	t3,0x18
20061a86:	0d21                	c.addi	s10,8
20061a88:	000a                	c.slli	zero,0x2
	...

20061a8c <CSWTCH.29>:
20061a8c:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20061a9c:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20061aac:	0008 0000                                   ....

20061ab0 <CSWTCH.30>:
20061ab0:	0000 4102 0000 4102 0000 0000 0000 0000     ...A...A........
	...
20061af0:	0080 4102 0080 4102 0000 0000 0000 0000     ...A...A........
	...
20061b30:	0100 4102 0100 4102 682f 6d6f 2f65 7561     ...A...A/home/au
20061b40:	6f74 6a5f 6e65 696b 736e 775f 616c 356e     to_jenkins_wlan5
20061b50:	492f 544f 415f 5354 722f 7065 736f 722f     /IOT_ATS/repos/r
20061b60:	6c65 6165 6573 6d2d 7361 6574 2f72 6f73     elease-master/so
20061b70:	7275 6563 612f 656d 6162 696c 6574 675f     urce/amebalite_g
20061b80:	6363 705f 6f72 656a 7463 702f 6f72 656a     cc_project/proje
20061b90:	7463 6b5f 3472 762f 6473 2f6b 2e2e 2e2f     ct_kr4/vsdk/../.
20061ba0:	2f2e 2e2e 632f 6d6f 6f70 656e 746e 732f     ./../component/s
20061bb0:	636f 612f 656d 6162 696c 6574 662f 6c77     oc/amebalite/fwl
20061bc0:	6269 722f 6d61 635f 6d6f 6f6d 2f6e 6d61     ib/ram_common/am
20061bd0:	6265 5f61 7069 5f63 6172 2e6d 0063 0000     eba_ipc_ram.c...
20061be0:	5043 2055 6425 7420 6b61 2065 6573 616d     CPU %d take sema
20061bf0:	2520 2064 6f66 2072 7870 6469 2520 2064      %d for pxid %d 
20061c00:	6166 6c69 203a 7063 6975 2064 6425 7020     fail: cpuid %d p
20061c10:	6978 2064 6425 6820 7361 7420 6b61 6e65     xid %d has taken
20061c20:	7420 6568 7320 6d65 0a61 0000 5043 2055      the sema...CPU 
20061c30:	6425 6620 6572 2065 6573 616d 2520 2064     %d free sema %d 
20061c40:	6f66 2072 7870 6469 2520 2064 6166 6c69     for pxid %d fail
20061c50:	203a 7063 6975 2064 6425 202c 7870 6469     : cpuid %d, pxid
20061c60:	2520 2064 7369 7420 6b61 6e69 2067 6874      %d is taking th
20061c70:	2065 6573 616d 000a 544f 2050 6572 6461     e sema..OTP read
20061c80:	6720 7465 6820 2077 6573 616d 6620 6961      get hw sema fai
20061c90:	0a6c 0000 544f 5f50 6552 6461 2038 6146     l...OTP_Read8 Fa
20061ca0:	6c69 2520 2078 000a                         il %x ..

20061ca8 <pmap_func>:
20061ca8:	0000 0101 0001 0101 0002 0102 0003 0102     ................
20061cb8:	0004 0202 0005 0102 0006 0202 0007 0102     ................
20061cc8:	0008 0101 0009 0101 000a 0101 000b 0101     ................
20061cd8:	000c 0101 000d 0101 000e 0101 000f 0101     ................
20061ce8:	0010 0101 0011 0101 0012 0000 0013 0202     ................
20061cf8:	0014 0202 0015 0101 0016 0000 0017 0101     ................
20061d08:	0018 0101 0019 0101 001a 0101 001b 0101     ................
20061d18:	001c 0101 001d 0101 001e 0101 001f 0101     ................
20061d28:	0020 0101 0021 0101 0022 0101 0023 0101      ...!..."...#...
20061d38:	0024 0101 0025 0101 0026 0101 0027 0101     $...%...&...'...
20061d48:	0028 0101 0029 0101 002a 0101 002b 0100     (...)...*...+...
20061d58:	002c 0100 002d 0100 002e 0100 002f 0100     ,...-......./...
20061d68:	0030 0100 0031 0100 0032 0100 0033 0100     0...1...2...3...
20061d78:	ffff ffff 766e 6369 695f 7172 2520 756c     ....nvic_irq %lu
20061d88:	4620 6e75 4e20 746f 4120 7373 6769 216e      Fun Not Assign!
20061d98:	2121 2121 000a 0000 4c50 4349 0000 0000     !!!!....PLIC....
20061da8:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20061db8:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20061dc8:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20061dd8:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20061de8:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20061df8:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20061e08:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20061e18:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
20061e28:	696c 6574 6d2f 7369 2f63 6d61 6265 5f61     lite/misc/ameba_
20061e38:	6d70 2e75 0063 0000 6e49 6176 696c 2064     pmu.c...Invalid 
20061e48:	7350 6172 436d 6b6c 000a 0000 5350 4152     PsramClk....PSRA
20061e58:	004d 0000 7370 6172 206d 7974 6570 6920     M...psram type i
20061e68:	2073 7830 7825 000a 5353 4549 524e 3009     s 0x%x..SSIENR.0
20061e78:	2578 3d78 7830 7825 000a 0000 5443 4c52     x%x=0x%x....CTRL
20061e88:	3052 3009 2578 3d78 7830 7825 000a 0000     R0.0x%x=0x%x....
20061e98:	4142 4455 0952 7830 7825 303d 2578 0a78     BAUDR.0x%x=0x%x.
20061ea8:	0000 0000 4156 494c 5f44 4d43 0944 7830     ....VALID_CMD.0x
20061eb8:	7825 303d 2578 0a78 0000 0000 5443 4c52     %x=0x%x.....CTRL
20061ec8:	3252 3009 2578 3d78 7830 7825 000a 0000     R2.0x%x=0x%x....
20061ed8:	4544 4956 4543 495f 464e 094f 7830 7825     DEVICE_INFO.0x%x
20061ee8:	303d 2578 0a78 0000 5054 3052 3009 2578     =0x%x...TPR0.0x%
20061ef8:	3d78 7830 7825 000a 4552 4441 465f 5341     x=0x%x..READ_FAS
20061f08:	5f54 4953 474e 454c 3009 2578 3d78 7830     T_SINGLE.0x%x=0x
20061f18:	7825 000a 5257 5449 5f45 4953 4e47 454c     %x..WRITE_SIGNLE
20061f28:	3009 2578 3d78 7830 7825 000a 5541 4f54     .0x%x=0x%x..AUTO
20061f38:	4c5f 4e45 5447 0948 7830 7825 303d 2578     _LENGTH.0x%x=0x%
20061f48:	0a78 0000 5355 5245 4c5f 4e45 5447 0948     x...USER_LENGTH.
20061f58:	7830 7825 303d 2578 0a78 0000 5541 4f54     0x%x=0x%x...AUTO
20061f68:	4c5f 4e45 5447 3248 3009 2578 3d78 7830     _LENGTH2.0x%x=0x
20061f78:	7825 000a 5253 3009 2578 3d78 7830 7825     %x..SR.0x%x=0x%x
20061f88:	000a 0000 5350 4850 5f59 4143 5f4c 4150     ....PSPHY_CAL_PA
20061f98:	0952 7830 7825 303d 2578 0a78 0000 0000     R.0x%x=0x%x.....
20061fa8:	5350 4850 5f59 4850 5f59 5443 4c52 3009     PSPHY_PHY_CTRL.0
20061fb8:	2578 3d78 7830 7825 000a 0000 5350 4850     x%x=0x%x....PSPH
20061fc8:	5f59 4244 5f47 4553 094c 7830 7825 303d     Y_DBG_SEL.0x%x=0
20061fd8:	2578 0a78 0000 0000 5249 5b30 5d31 303d     x%x.....IR0[1]=0
20061fe8:	2578 0a78 0000 0000 5249 5b30 5d30 303d     x%x.....IR0[0]=0
20061ff8:	2578 0a78 0000 0000 5243 5b30 5d31 303d     x%x.....CR0[1]=0
20062008:	2578 0a78 0000 0000 5243 5b30 5d30 303d     x%x.....CR0[0]=0
20062018:	2578 0a78 0000 0000 5243 5b31 5d31 303d     x%x.....CR1[1]=0
20062028:	2578 0a78 0000 0000 5243 5b31 5d30 303d     x%x.....CR1[0]=0
20062038:	2578 0a78 0000 0000 3d3d 703d 6168 6573     x%x.....===phase
20062048:	2520 786c 3d20 3d3d 3d3d 000a 6b6f 2520      %lx =====..ok %
20062058:	786c 2520 786c 2520 786c 2520 786c 2520     lx %lx %lx %lx %
20062068:	786c 2520 786c 2520 786c 000a 6166 6c69     lx %lx %lx..fail
20062078:	2520 786c 2520 786c 2520 786c 2520 786c      %lx %lx %lx %lx
20062088:	2520 786c 2520 786c 2520 786c 000a 0000      %lx %lx %lx....
20062098:	6143 4e6c 696d 206e 203d 7825 4320 6c61     CalNmin = %x Cal
200620a8:	6d4e 7861 3d20 2520 2078 6957 646e 776f     Nmax = %x Window
200620b8:	6953 657a 3d20 2520 2078 6870 7361 3a65     Size = %x phase:
200620c8:	2520 2078 000a 0000 542b 5345 3a54 0000      %x ....+TEST:..
200620d8:	0a0d 7325 4b4f 0a0d 0000 0000 4f2b 4154     ..%sOK......+OTA
200620e8:	4c43 4145 3a52 0000 4f2b 4154 4552 4f43     CLEAR:..+OTARECO
200620f8:	4556 3a52 0000 0000 522b 5453 003a 0000     VER:....+RST:...
20062108:	6154 6b73 4c20 7369 3a74 0a20 250d 0a73     Task List: ..%s.
20062118:	000d 0000 6548 7061 7453 7461 3a73 0d20     ....HeapStats: .
20062128:	000a 0000 6f54 6174 206c 6548 7061 093a     ....Total Heap:.
20062138:	7525 0a0d 0000 0000 6548 7061 4620 6572     %u......Heap Fre
20062148:	2065 6f4e 3a77 2509 0d75 000a 6548 7061     e Now:.%u...Heap
20062158:	5520 6573 2064 6f4e 3a77 2509 0d75 000a      Used Now:.%u...
20062168:	6548 7061 5520 6573 2064 614d 3a78 2509     Heap Used Max:.%
20062178:	0d75 000a 532b 4154 4554 003a 7325 0000     u...+STATE:.%s..
20062188:	3056 2e32 2e31 0031 3376 352e 0000 0000     V02.1.1.v3.5....
20062198:	3032 3432 302f 2f36 3430 312d 3a34 3435     2024/06/04-14:54
200621a8:	323a 0031 472b 524d 003a 0000 0a0d 7325     :21.+GMR:.....%s
200621b8:	7325 252c 2c73 7325 0a0d 0000 4c5b 474f     %s,%s,%s....[LOG
200621c8:	205d 5245 4f52 2052 7261 3a67 0d20 000a     ] ERROR arg: ...
200621d8:	0023 0000 4c5b 474f 205d 6e49 6176 696c     #...[LOG] Invali
200621e8:	2064 6567 2074 6170 6172 656d 6574 7372     d get parameters
200621f8:	0d2e 000a 253c 3e73 6c20 7665 6c65 6920     ....<%s> level i
20062208:	2073 6425 0a0d 0000 4c5b 474f 205d 6e49     s %d....[LOG] In
20062218:	6176 696c 2064 6573 2074 6170 6172 656d     valid set parame
20062228:	6574 7372 0d2e 000a 4c5b 474f 205d 6e49     ters....[LOG] In
20062238:	6176 696c 2064 6f6c 2067 656c 6576 206c     valid log level 
20062248:	6425 0d2e 000a 0000 4c5b 474f 205d 6e49     %d......[LOG] In
20062258:	6176 696c 2064 6170 6172 656d 6574 2072     valid parameter 
20062268:	756e 626d 7265 0d2e 000a 0000 4c5b 474f     number......[LOG
20062278:	205d 6e49 6176 696c 2064 6567 5f74 6573     ] Invalid get_se
20062288:	2074 6425 0d2e 000a 4c5b 474f 205d 6554     t %d....[LOG] Te
20062298:	7473 6120 776c 7961 2073 656c 6576 0d6c     st always level.
200622a8:	000a 0000 4c5b 474f 205d 6554 7473 6520     ....[LOG] Test e
200622b8:	7272 726f 6c20 7665 6c65 0a0d 0000 0000     rror level......
200622c8:	4c5b 474f 205d 6554 7473 7720 7261 696e     [LOG] Test warni
200622d8:	676e 6c20 7665 6c65 0a0d 0000 4c5b 474f     ng level....[LOG
200622e8:	205d 6554 7473 6920 666e 206f 656c 6576     ] Test info leve
200622f8:	0d6c 000a 4c5b 474f 205d 6554 7473 4c20     l...[LOG] Test L
20062308:	474f 495f 4554 534d 0a0d 0000 4c2b 474f     OG_ITEMS....+LOG
20062318:	003a 0000 0a0d 7325 5245 4f52 3a52 6425     :.....%sERROR:%d
20062328:	0a0d 0000 5441 4c2b 474f 3c3d 6567 5f74     ....AT+LOG=<get_
20062338:	6573 3e74 3c2c 6f6d 7564 656c 5b3e 3c2c     set>,<module>[,<
20062348:	6f6c 5f67 656c 6576 3e6c 0d5d 000a 0000     log_level>].....
20062358:	3c09 6567 5f74 6573 3e74 093a 2d30 6567     .<get_set>:.0-ge
20062368:	2c74 3120 732d 7465 202c 2d32 7270 6e69     t, 1-set, 2-prin
20062378:	2074 6c61 2c6c 3320 632d 656c 7261 6120     t all, 3-clear a
20062388:	6c6c 0a0d 0000 0000 3c09 6f6d 7564 656c     ll.......<module
20062398:	3a3e 2a09 652d 6361 2068 6f6d 7564 656c     >:.*-each module
200623a8:	202c 746f 6568 7372 732d 6570 6963 6966     , others-specifi
200623b8:	2063 6f6d 7564 656c 0a0d 0000 3c09 6f6c     c module.....<lo
200623c8:	5f67 656c 6576 3e6c 093a 305b 352c 2c5d     g_level>:.[0,5],
200623d8:	6f20 6c6e 2079 7061 6c70 6369 6261 656c      only applicable
200623e8:	6620 726f 7320 7465 6d20 646f 0d65 000a      for set mode...
200623f8:	525b 4552 5d47 4520 7272 726f 7020 7261     [RREG] Error par
20062408:	6d61 7465 7265 0d73 000a 0000 522b 4552     ameters.....+RRE
20062418:	3a47 0000 575b 4552 5d47 4520 7272 726f     G:..[WREG] Error
20062428:	7020 7261 6d61 7465 7265 0d73 000a 0000      parameters.....
20062438:	572b 4552 3a47 0000 5043 2555 0000 0000     +WREG:..CPU%....
20062448:	7270 6f69 0000 0000 7473 7461 0065 0000     prio....state...
20062458:	6174 6b73 6e20 6d61 0065 0000 7325 2509     task name...%s.%
20062468:	0973 7325 2509 0a73 000d 0000 7325 2509     s.%s.%s.....%s.%
20062478:	0963 6425 2509 2e64 7325 0a0d 0000 0000     c.%d.%d.%s......
20062488:	7325 6d20 6c61 6f6c 2063 6166 6c69 6465     %s malloc failed
20062498:	0a0d 0000 745b 706f 555d 6173 6567 203a     ....[top]Usage: 
200624a8:	6f74 3d70 6f6d 6564 742c 6d69 2c65 6f63     top=mode,time,co
200624b8:	6e75 0a74 200d 6f6d 6564 203a 2c30 7320     unt.. mode: 0, s
200624c8:	6174 7472 6320 756f 746e 6320 7570 7520     tart count cpu u
200624d8:	6173 6567 6520 6576 7972 5b20 6974 656d     sage every [time
200624e8:	205d 6573 6f63 646e 0d2e 200a 6f6d 6564     ] second... mode
200624f8:	203a 2c31 7320 6f74 2070 6f6d 6564 3020     : 1, stop mode 0
20062508:	0d2e 200a 6f6d 6564 203a 3a32 7320 6174     ... mode: 2: sta
20062518:	7472 6320 756f 746e 6320 7570 7520 6173     rt count cpu usa
20062528:	6567 0d2e 200a 6f6d 6564 203a 3a33 7320     ge... mode: 3: s
20062538:	6f74 2070 6f6d 6564 3220 0d2e 200a 6974     top mode 2... ti
20062548:	656d 203a 5043 2055 7473 7461 7369 6974     me: CPU statisti
20062558:	7363 6920 746e 7265 6176 2e6c 6544 6166     cs interval.Defa
20062568:	6c75 2074 2e31 7520 696e 2874 2973 0d20     ult 1. unit(s) .
20062578:	200a 6f63 6e75 3a74 4320 5550 7320 6174     . count: CPU sta
20062588:	6974 7473 6369 2073 6f63 6e75 2c74 6420     tistics count, d
20062598:	6665 7561 746c 7520 746e 6c69 7320 6f74     efault until sto
200625a8:	2070 726f 3120 322c 332c 2e2e 002e 0000     p or 1,2,3......
200625b8:	654d 6f6d 7972 6620 6961 756c 6572 6620     Memory failure f
200625c8:	726f 7420 7361 5f6b 7473 7461 7375 0a0d     or task_status..
200625d8:	0000 0000 7563 7272 6e65 2074 6f6d 6564     ....current mode
200625e8:	203a 6425 7420 6d69 3a65 2520 2864 2973     : %d time: %d(s)
200625f8:	6320 756f 746e 203a 6425 2820 312d 6620      count: %d (-1 f
20062608:	726f 6920 666e 6e69 7469 2965 0a0d 0000     or infinite)....
20062618:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062628:	432b 5550 4f4c 4441 003a 0000 5441 432b     +CPULOAD:...AT+C
20062638:	5550 4f4c 4441 3c3d 6f6d 6564 5b3e 3c2c     PULOAD=<mode>[,<
20062648:	6974 656d 695f 746e 6176 3e6c 3c2c 6f63     time_intval>,<co
20062658:	6e75 6574 3e72 0d5d 000a 0000 3c09 6f6d     unter>]......<mo
20062668:	6564 3a3e 5b09 2c30 5d32 0a0d 0000 0000     de>:.[0,2]......
20062678:	3c09 6974 656d 695f 746e 6176 3e6c 093a     .<time_intval>:.
20062688:	6e49 7320 6365 0a0d 0000 0000 3c09 6f63     In sec.......<co
20062698:	6e75 6574 3e72 3009 6d20 6165 736e 6920     unter>.0 means i
200626a8:	666e 6e69 7469 0a0d 0000 0000 5441 7325     nfinit......AT%s
200626b8:	0a0d 0000 6f43 6d6d 6e6f 4120 2054 6f43     ....Common AT Co
200626c8:	6d6d 6e61 3a64 0a0d 0000 0000 4c2b 5349     mmand:......+LIS
200626d8:	3a54 0000 4f2b 4154 4c43 4145 0052 0000     T:..+OTACLEAR...
200626e8:	4f2b 4154 4552 4f43 4556 0052 432b 5550     +OTARECOVER.+CPU
200626f8:	4f4c 4441 0000 0000 542b 5345 0054 0000     LOAD....+TEST...
20062708:	4c2b 5349 0054 0000 522b 5453 0000 0000     +LIST...+RST....
20062718:	532b 4154 4554 0000 472b 524d 0000 0000     +STATE..+GMR....
20062728:	4c2b 474f 0000 0000 522b 4552 0047 0000     +LOG....+RREG...
20062738:	572b 4552 0047 0000                         +WREG...

20062740 <__FUNCTION__.0>:
20062740:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062750:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20062760:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20062770:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20062780:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20062790:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
200627a0:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
200627b0:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
200627c0:	6f70 656e 746e 6f2f 2f73 7266 6565 7472     ponent/os/freert
200627d0:	736f 662f 6572 7265 6f74 5f73 6568 7061     os/freertos_heap
200627e0:	5f35 6f63 666e 6769 632e 0000 255b 5d73     5_config.c..[%s]
200627f0:	6c20 7665 6c65 3d20 2520 0a64 0000 0000      level = %d.....
20062800:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20062810:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20062820:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20062830:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20062840:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20062850:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20062860:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20062870:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
20062880:	696c 6574 732f 6c77 6269 6c2f 676f 632e     lite/swlib/log.c
20062890:	0000 0000 002a 0000 6143 6863 2065 7261     ....*...Cache ar
200628a0:	6172 2079 7369 6620 6c75 2c6c 6120 646e     ray is full, and
200628b0:	7220 7065 616c 6563 6f20 646c 6520 746e      replace old ent
200628c0:	7972 000a 255b 3830 5d78 0020 0a0d 255b     ry..[%08x] ...[%
200628d0:	3830 5d78 0020 0000 3025 7838 0020 0000     08x] ...%08x ...
200628e0:	3025 7832 0020 0000 255b 2d73 6325 205d     %02x ...[%s-%c] 
200628f0:	0000 0000 003d 0000 7769 7270 7669 0000     ....=...iwpriv..
20062900:	0a0d 6e75 6e6b 776f 206e 6f63 6d6d 6e61     ..unknown comman
20062910:	2064 2527 2773 0000 0d0a 4d5b 4d45 205d     d '%s'....[MEM] 
20062920:	6641 6574 2072 6f64 6320 646d 202c 7661     After do cmd, av
20062930:	6961 616c 6c62 2065 6568 7061 2520 0a64     ailable heap %d.
20062940:	000d 0000 0a0d 230a 0a0d 0000 0000 0000     .......#........
20062950:	5749 4744 7220 6665 6572 6873 6f20 216e     IWDG refresh on!
20062960:	000a 0000 414d 4e49 0000 0000 4457 5f47     ....MAIN....WDG_
20062970:	6954 656d 0072 0000 5749 4744 7220 6665     Timer...IWDG ref
20062980:	6572 6873 6520 7272 726f 000a 524b 2034     resh error..KR4 
20062990:	534f 5320 4154 5452 0a20 0000 2d2d 2d2d     OS START ...----
200629a0:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 202d 4554     ------------- TE
200629b0:	5453 4320 4d4f 414d 444e 4d20 444f 2045     ST COMMAND MODE 
200629c0:	4548 504c 2520 2d64 2d2d 2d2d 2d2d 2d2d     HELP %d---------
200629d0:	2d2d 2d2d 2d2d 2d2d 0a2d 0000 7325 000a     ---------...%s..
200629e0:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
200629f0:	202d 4554 5453 4320 4d4f 414d 444e 4d20     - TEST COMMAND M
20062a00:	444f 2045 4e45 2044 2520 2d78 2d2d 2d2d     ODE END  %x-----
20062a10:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 0a2d 0000     -------------...
20062a20:	6552 6f62 746f 6e69 2067 2e2e 0a2e 000d     Rebooting ......
20062a30:	6175 7472 7562 6e72 0000 0000 255b 3830     uartburn....[%08
20062a40:	5d78 2520 3830 2078 000a 0000 7257 6e6f     x] %08x ....Wron
20062a50:	2067 7261 7567 656d 746e 6e20 6d75 6562     g argument numbe
20062a60:	2172 0a0d 0000 0000 5352 5049 6520 616e     r!......RSIP ena
20062a70:	6c62 6465 202c 6143 276e 2074 6552 6461     bled, Can't Read
20062a80:	4620 616c 6873 0a2e 0000 0000 7257 6e6f      Flash......Wron
20062a90:	2067 6d63 2164 0a0d 0000 0000 003f 0000     g cmd!......?...
20062aa0:	4809 4c45 2050 4028 293f 3a20 0a20 0909     .HELP (@?) : ...
20062ab0:	5020 6972 746e 6820 6c65 2070 656d 7373      Print help mess
20062ac0:	6761 000a 5744 0000 4009 5744 3c20 6441     ag..DW...@DW <Ad
20062ad0:	7264 7365 2c73 4c20 6e65 7467 3e68 090a     dress, Length>..
20062ae0:	2009 7544 706d 6d20 6d65 726f 2079 6f77     . Dump memory wo
20062af0:	6472 6f20 2072 6552 6461 4820 2077 6f77     rd or Read Hw wo
20062b00:	6472 7220 6765 7369 6574 3b72 0a20 0909     rd register; ...
20062b10:	4420 2057 613c 6464 3e72 0920 2009 7544      DW <addr> .. Du
20062b20:	706d 6f20 6c6e 2079 6e6f 2065 6f77 6472     mp only one word
20062b30:	6120 2074 6874 2065 6173 656d 7420 6d69      at the same tim
20062b40:	2c65 7520 696e 3a74 3420 7942 6574 0a73     e, unit: 4Bytes.
20062b50:	0909 4420 2057 613c 6464 3e72 3c20 656c     .. DW <addr> <le
20062b60:	3e6e 0920 4420 6d75 2070 6874 2065 7073     n> . Dump the sp
20062b70:	6365 6669 6569 2064 656c 676e 6874 6f20     ecified length o
20062b80:	2066 6874 2065 6f77 6472 202c 6e75 7469     f the word, unit
20062b90:	203a 4234 7479 7365 090a 2009 5744 3c20     : 4Bytes... DW <
20062ba0:	6461 7264 203e 6c3c 6e65 203e 2062 4420     addr> <len> b  D
20062bb0:	6d75 2070 6874 2065 7073 6365 6669 6569     ump the specifie
20062bc0:	2064 656c 676e 6874 6f20 2066 6874 2065     d length of the 
20062bd0:	7962 6574 202c 6e75 7469 203a 4231 7479     byte, unit: 1Byt
20062be0:	0a65 0000 5745 0000 4009 5745 3c20 6441     e...EW...@EW <Ad
20062bf0:	7264 7365 2c73 4820 7865 0a3e 0909 5720     dress, Hex>... W
20062c00:	6972 6574 6d20 6d65 726f 2079 7764 726f     rite memory dwor
20062c10:	2064 726f 5720 6972 6574 4820 2077 7764     d or Write Hw dw
20062c20:	726f 2064 6572 6967 7473 7265 0a20 0909     ord register ...
20062c30:	4320 6e61 7720 6972 6574 6f20 6c6e 2079      Can write only 
20062c40:	6e6f 2065 7764 726f 2064 7461 7420 6568     one dword at the
20062c50:	7320 6d61 2065 6974 656d 0a20 0909 4520      same time ... E
20062c60:	3a78 4520 2057 6441 7264 7365 2073 6156     x: EW Address Va
20062c70:	756c 2065 000a 0000 4552 4f42 544f 0000     lue ....REBOOT..
20062c80:	4009 4552 4f42 544f 0a20 0909 7220 6265     .@REBOOT ... reb
20062c90:	6f6f 2074 090a 2009 6572 6f62 746f 7520     oot ... reboot u
20062ca0:	7261 6274 7275 206e 000a 0000 6f4e 7354     artburn ....NoTs
20062cb0:	006b 0000 524b 0034 614d 6c6c 636f 6620     k...KR4.Malloc f
20062cc0:	6961 656c 2e64 4320 726f 3a65 255b 5d73     ailed. Core:[%s]
20062cd0:	202c 6154 6b73 5b3a 7325 2c5d 5b20 7266     , Task:[%s], [fr
20062ce0:	6565 6820 6165 2070 6973 657a 203a 6425     ee heap size: %d
20062cf0:	0d5d 000a 0d0a 255b 5d73 5320 4154 4b43     ].....[%s] STACK
20062d00:	4f20 4556 4652 4f4c 2057 202d 6154 6b73      OVERFLOW - Task
20062d10:	614e 656d 2528 2973 0d0a 0000               Name(%s)....

20062d1c <__FUNCTION__.5>:
20062d1c:	4176 7070 696c 6163 6974 6e6f 7453 6361     vApplicationStac
20062d2c:	4f6b 6576 6672 6f6c 4877 6f6f 006b 0000     kOverflowHook...
20062d3c:	5441 3f53 0000 0000 4f4c 5547 5241 5f54     ATS?....LOGUART_
20062d4c:	4154 4b53 0000 0000 7243 6165 6574 4c20     TASK....Create L
20062d5c:	676f 5520 5241 2054 6154 6b73 4520 7272     og UART Task Err
20062d6c:	2121 000a 4853 4c45 004c 0000 255b 5d73     !!..SHELL...[%s]
20062d7c:	4920 474d 4449 203a 6425 202c 7563 7272      IMGID: %d, curr
20062d8c:	6e65 2074 544f 2541 2064 6441 7264 7365     ent OTA%d Addres
20062d9c:	3a73 3020 2578 3830 2c78 7420 7261 6567     s: 0x%08x, targe
20062dac:	2074 544f 2541 2064 6441 7264 7365 3a73     t OTA%d Address:
20062dbc:	3020 2578 3830 0a78 0000 0000 5953 0053      0x%08x.....SYS.
20062dcc:	255b 5d73 4920 474d 4449 203a 6425 202c     [%s] IMGID: %d, 
20062ddc:	7563 7272 6e65 2074 6966 6d72 6177 6572     current firmware
20062dec:	6920 2073 544f 2541 2c64 7420 7261 6567      is OTA%d, targe
20062dfc:	2074 6966 6d72 6177 6572 4f20 4154 6425     t firmware OTA%d
20062e0c:	6920 2073 6e69 6176 696c 0a64 0000 0000      is invalid.....
20062e1c:	255b 5d73 6220 6361 756b 2070 616d 6c6c     [%s] backup mall
20062e2c:	636f 6620 6961 646c 6465 000a               oc failded..

20062e38 <__func__.0>:
20062e38:	7973 5f73 6572 6f63 6576 5f72 746f 5f61     sys_recover_ota_
20062e48:	6973 6e67 7461 7275 0065 0000               signature...

20062e54 <__func__.1>:
20062e54:	7973 5f73 6c63 6165 5f72 746f 5f61 6973     sys_clear_ota_si
20062e64:	6e67 7461 7275 0065 4449 454c 0000 0000     gnature.IDLE....
20062e74:	2509 0963 6425 2509 0964 6425 0a0d 0000     .%c.%d.%d.%d....
20062e84:	6d54 2072 7653 0063 e58a 2005 e58a 2005     Tmr Svc.... ... 
20062e94:	e58a 2005 e5c6 2005 e5d2 2005 e5ee 2005     ... ... ... ... 
20062ea4:	e58a 2005 e58a 2005 e5c6 2005 e5d2 2005     ... ... ... ... 
20062eb4:	7325 2528 2964 202c 7641 6961 616c 6c62     %s(%d), Availabl
20062ec4:	2065 6568 7061 2520 0a64 0000 4c57 4e41     e heap %d...WLAN
20062ed4:	0000 0000 6e69 7469 0000 0000 6c77 6e61     ....init....wlan
20062ee4:	695f 696e 6974 6c61 7a69 2065 6166 6c69     _initialize fail
20062ef4:	6465 000a                                   ed..

20062ef8 <__FUNCTION__.0>:
20062ef8:	695f 696e 5f74 6874 6572 6461 0000 0000     _init_thread....
20062f08:	6f46 2072 6564 7562 2067 6170 6b63 6761     For debug packag
20062f18:	2e65 5020 656c 7361 2065 6573 656c 7463     e. Please select
20062f28:	7020 7273 6d61 7420 7079 2065 6361 6f63      psram type acco
20062f38:	6472 6e69 2067 6f74 7920 756f 2072 6863     rding to your ch
20062f48:	7069 000a 4843 5049 4e49 4f46 0000 0000     ip..CHIPINFO....
20062f58:	3230 6620 726f 5720 2042 3233 624d 300a     02 for WB 32Mb.0
20062f68:	2034 6f66 2072 4257 3120 3832 624d 300a     4 for WB 128Mb.0
20062f78:	2033 6f66 2072 4257 3220 3635 624d 0a0a     3 for WB 256Mb..
20062f88:	0000 0000 682f 6d6f 2f65 7561 6f74 6a5f     ..../home/auto_j
20062f98:	6e65 696b 736e 775f 616c 356e 492f 544f     enkins_wlan5/IOT
20062fa8:	415f 5354 722f 7065 736f 722f 6c65 6165     _ATS/repos/relea
20062fb8:	6573 6d2d 7361 6574 2f72 6f73 7275 6563     se-master/source
20062fc8:	612f 656d 6162 696c 6574 675f 6363 705f     /amebalite_gcc_p
20062fd8:	6f72 656a 7463 702f 6f72 656a 7463 6b5f     roject/project_k
20062fe8:	3472 762f 6473 2f6b 2e2e 2e2f 2f2e 2e2e     r4/vsdk/../../..
20062ff8:	632f 6d6f 6f70 656e 746e 732f 636f 612f     /component/soc/a
20063008:	656d 6162 696c 6574 6c2f 6269 722f 6d61     mebalite/lib/ram
20063018:	635f 6d6f 6f6d 2f6e 6d61 6265 5f61 6863     _common/ameba_ch
20063028:	7069 6e69 6f66 6c5f 6269 632e 0000 0000     ipinfo_lib.c....
20063038:	7325 203a 0000 0000 5245 3a52 4720 7465     %s: ....ERR: Get
20063048:	5320 636f 4e20 6d61 2065 6146 6c69 000a      Soc Name Fail..
20063058:	3025 7832 0000 0000 4d4b 0034 7325 4c20     %02x....KM4.%s L
20063068:	6269 5620 7265 6973 6e6f 0a3a 0000 0000     ib Version:.....
20063078:	3032 3432 302f 2f36 3430 312d 3a34 3435     2024/06/04-14:54
20063088:	303a 0031 4f43 504d 4c49 2045 4954 454d     :01.COMPILE TIME
20063098:	203a 7325 000a 0000 6367 2063 6576 7372     : %s....gcc vers
200630a8:	6f69 206e 3031 332e 312e 3220 3230 3032     ion 10.3.1 20220
200630b8:	3335 2030 5228 6165 746c 6b65 5620 4453     530 (Realtek VSD
200630c8:	2d4b 3031 332e 312e 4220 6975 646c 3420     K-10.3.1 Build 4
200630d8:	3730 2b33 2029 0000 4f43 504d 4c49 5245     073+) ..COMPILER
200630e8:	203a 7325 000a 0000 6325 0000 2d2d 2d2d     : %s....%c..----
200630f8:	2d2d 2d2d 2d2d 2d2d 6154 6b73 4920 666e     --------Task Inf
20063108:	2d6f 2d2d 2d2d 2d2d 2d2d 2d2d 0d2d 000a     o------------...
20063118:	6146 6c75 2074 6e6f 7420 7361 206b 253c     Fault on task <%
20063128:	3e73 000a 6154 6b73 4920 3a44 2520 0d64     s>..Task ID: %d.
20063138:	000a 0000 6154 6b73 5420 4243 303a 2578     ....Task TCB:0x%
20063148:	3830 786c 0a0d 0000 7543 7272 6e65 2074     08lx....Current 
20063158:	7453 7461 3a65 2520 2064 2528 2973 0a0d     State: %d (%s)..
20063168:	0000 0000 6142 6573 5020 6972 726f 7469     ....Base Priorit
20063178:	3a79 2520 0d64 000a 7543 7272 6e65 2074     y: %d...Current 
20063188:	7250 6f69 6972 7974 203a 6425 0a0d 0000     Priority: %d....
20063198:	7552 206e 6954 656d 4320 756f 746e 7265     Run Time Counter
200631a8:	203a 6c25 0d75 000a 7453 6361 546b 706f     : %lu...StackTop
200631b8:	203a 7830 3025 6c38 2c78 5320 6174 6b63     : 0x%08lx, Stack
200631c8:	6142 6573 203a 7830 3025 6c38 2c78 5320     Base: 0x%08lx, S
200631d8:	6174 6b63 6e45 3a64 3020 2578 3830 786c     tackEnd: 0x%08lx
200631e8:	202c 7453 6361 536b 7a69 3d65 6c25 2875     , StackSize=%lu(
200631f8:	6f77 6472 0d29 000a 7453 6361 206b 6948     word)...Stack Hi
20063208:	6867 5720 7461 7265 614d 6b72 203a 6425     gh WaterMark: %d
20063218:	7728 726f 2964 0a0d 0000 0000 7552 6e6e     (word)......Runn
20063228:	6e69 0067 6552 6461 2079 202f 6550 646e     ing.Ready / Pend
20063238:	6e69 0067 6c42 636f 656b 0064 7553 7073     ing.Blocked.Susp
20063248:	6e65 6564 0064 0000 6544 656c 6574 0064     ended...Deleted.
20063258:	6e49 6176 696c 0064                         Invalid.

20063260 <TaskStateString.0>:
20063260:	3224 2006 322c 2006 323c 2006 3244 2006     $2. ,2. <2. D2. 
20063270:	3250 2006 3258 2006 eec8 2005 eddc 2005     P2. X2. ... ... 
20063280:	eddc 2005 eddc 2005 eddc 2005 eddc 2005     ... ... ... ... 
20063290:	eddc 2005 eddc 2005 eddc 2005 eddc 2005     ... ... ... ... 
200632a0:	ee12 2005 ef84 2005 ee52 2005 eddc 2005     ... ... R.. ... 
200632b0:	eddc 2005 eddc 2005 eddc 2005 ee52 2005     ... ... ... R.. 
200632c0:	eddc 2005 eddc 2005 efd8 2005 eddc 2005     ... ... ... ... 
200632d0:	eddc 2005 ee38 2005 eeee 2005 eddc 2005     ... 8.. ... ... 
200632e0:	eddc 2005 ef0c 2005 eddc 2005 ee94 2005     ... ... ... ... 
200632f0:	eddc 2005 eddc 2005 eeca 2005 eddc 2005     ... ... ... ... 
20063300:	efda 2005 3130 3332 3534 3736 3938 4241     ... 0123456789AB
20063310:	4443 4645 0000 0000 6f69 6420 6972 6576     CDEF....io drive
20063320:	2072 6170 6172 656d 6574 7372 6520 7272     r parameters err
20063330:	726f 2021 6966 656c 6e5f 6d61 3a65 2520     or! file_name: %
20063340:	2c73 6c20 6e69 3a65 2520 0064 682f 6d6f     s, line: %d./hom
20063350:	2f65 7561 6f74 6a5f 6e65 696b 736e 775f     e/auto_jenkins_w
20063360:	616c 356e 492f 544f 415f 5354 722f 7065     lan5/IOT_ATS/rep
20063370:	736f 722f 6c65 6165 6573 6d2d 7361 6574     os/release-maste
20063380:	2f72 6f73 7275 6563 612f 656d 6162 696c     r/source/amebali
20063390:	6574 675f 6363 705f 6f72 656a 7463 702f     te_gcc_project/p
200633a0:	6f72 656a 7463 6b5f 3472 762f 6473 2f6b     roject_kr4/vsdk/
200633b0:	2e2e 2e2f 2f2e 2e2e 632f 6d6f 6f70 656e     ../../../compone
200633c0:	746e 732f 636f 612f 656d 6162 696c 6574     nt/soc/amebalite
200633d0:	6c2f 6269 722f 6d61 635f 6d6f 6f6d 2f6e     /lib/ram_common/
200633e0:	6d61 6265 5f61 6f72 5f6d 6170 6374 2e68     ameba_rom_patch.
200633f0:	0063 0000 7269 5b71 6425 205d 7270 6f69     c...irq[%d] prio
20063400:	6972 7974 2520 2078 6873 6c61 206c 3d3c     rity %x shall <=
20063410:	2520 0a64 0000 0000 5b1b 3b31 3333 5b6d      %d......[1;33m[
20063420:	5245 4f52 5d52 5b20 7325 3a3a 6425 205d     ERROR] [%s::%d] 
20063430:	7473 6361 5f6b 6973 657a 2528 2964 6920     stack_size(%d) i
20063440:	2073 6f70 6973 6974 6576 3f20 0a3f 5b1b     s positive ??..[
20063450:	6d30 0000 5b1b 3b31 3333 5b6d 5245 4f52     0m...[1;33m[ERRO
20063460:	5d52 5b20 7325 3a3a 6425 205d 6172 6f5f     R] [%s::%d] ra_o
20063470:	6666 6573 2874 6425 2029 7369 6e20 6765     ffset(%d) is neg
20063480:	7461 7669 3f65 1b3f 305b 006d 5b1b 3b31     ative??.[0m..[1;
20063490:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
200634a0:	6425 205d 7453 7261 2074 7473 6361 206b     %d] Start stack 
200634b0:	6162 6b63 7274 6361 6e69 2067 6f66 2072     backtracing for 
200634c0:	7073 2520 2c70 7020 2063 7025 1b0a 305b     sp %p, pc %p..[0
200634d0:	006d 0000 5b1b 3b31 3333 5b6d 5254 4341     m....[1;33m[TRAC
200634e0:	5d45 5b20 7325 3a3a 6425 205d 665b 6172     E] [%s::%d] [fra
200634f0:	656d 2320 6425 205d 7073 3e2d 2520 2c70     me #%d] sp-> %p,
20063500:	7020 2d63 203e 7025 202c 7473 6361 5f6b      pc-> %p, stack_
20063510:	6973 657a 3e2d 2520 2c64 7220 2d61 203e     size-> %d, ra-> 
20063520:	7025 1b0a 305b 006d 5b1b 3b31 3333 5b6d     %p..[0m..[1;33m[
20063530:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063540:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20063550:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20063560:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20063570:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063580:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20063590:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
200635a0:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
200635b0:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
200635c0:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
200635d0:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
200635e0:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
200635f0:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
20063600:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
20063610:	1b0a 305b 006d 0000                         ..[0m...

20063618 <__FUNCTION__.3>:
20063618:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063628:	6932 0000                                   2i..

2006362c <__FUNCTION__.2>:
2006362c:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
2006363c:	6332 0000                                   2c..

20063640 <__FUNCTION__.1>:
20063640:	6f64 735f 6174 6b63 625f 6361 746b 6172     do_stack_backtra
20063650:	6563 0000 6e55 6e6b 776f 206e 7865 6563     ce..Unknown exce
20063660:	7470 6f69 006e 0000 5b1b 3b31 3333 5b6d     ption....[1;33m[
20063670:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063680:	3d3d 3d3d 3d3d 3d3d 3d3d 4320 6172 6873     ========== Crash
20063690:	4420 6d75 2070 3d3d 3d3d 3d3d 3d3d 3d3d      Dump ==========
200636a0:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
200636b0:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200636c0:	7845 6563 7470 6f69 206e 6163 6775 7468     Exception caught
200636d0:	6f20 206e 7025 7720 7469 2068 6572 7361      on %p with reas
200636e0:	6e6f 5b20 7830 7825 205d 3e2d 5b20 7325     on [0x%x] -> [%s
200636f0:	0a5d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     ]..[0m...[1;33m[
20063700:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063710:	3d3d 3d3d 3d3d 3d3d 3d3d 5220 6765 7369     ========== Regis
20063720:	6574 2072 7544 706d 3d20 3d3d 3d3d 3d3d     ter Dump =======
20063730:	3d3d 0a3d 5b1b 6d30 0000 0000 5b1b 3b31     ===..[0m.....[1;
20063740:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063750:	6425 205d 6d5b 6373 6172 6374 5d68 3020     %d] [mscratch] 0
20063760:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063770:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063780:	6425 205d 6d5b 7065 5d63 2020 2020 3020     %d] [mepc]     0
20063790:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
200637a0:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
200637b0:	6425 205d 6d5b 6163 7375 5d65 2020 3020     %d] [mcause]   0
200637c0:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
200637d0:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
200637e0:	6425 205d 6d5b 7674 6c61 205d 2020 3020     %d] [mtval]    0
200637f0:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063800:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063810:	6425 205d 7845 6563 7470 6f69 206e 6163     %d] Exception ca
20063820:	7375 2065 6f63 6c75 2064 6562 0d3a 1b0a     use could be:...
20063830:	305b 006d 5b1b 3b31 3333 5b6d 5254 4341     [0m..[1;33m[TRAC
20063840:	5d45 5b20 7325 3a3a 6425 205d 2509 2c73     E] [%s::%d] .%s,
20063850:	2520 2073 203d 7830 7825 0a0d 5b1b 6d30      %s = 0x%x...[0m
20063860:	0000 0000 5b1b 3b31 3333 5b6d 5254 4341     .....[1;33m[TRAC
20063870:	5d45 5b20 7325 3a3a 6425 205d 2509 0d73     E] [%s::%d] .%s.
20063880:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063890:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200638a0:	785b 6425 2d20 203e 7325 205d 7830 3025     [x%d -> %s] 0x%0
200638b0:	7838 1b0a 305b 006d 5b1b 3b31 3333 5b6d     8x..[0m..[1;33m[
200638c0:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200638d0:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
200638e0:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
200638f0:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20063900:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063910:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20063920:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20063930:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20063940:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20063950:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20063960:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063970:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
20063980:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
20063990:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
200639a0:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
200639b0:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200639c0:	3d3d 3d3d 3d3d 3d3d 3d3d 4520 646e 6f20     ========== End o
200639d0:	2066 7243 7361 2068 7544 706d 3d20 3d3d     f Crash Dump ===
200639e0:	3d3d 3d3d 3d3d 0a3d 5b1b 6d30 0000 0000     =======..[0m....
200639f0:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063a00:	7325 3a3a 6425 205d 0a0d 0a0d 5b1b 6d30     %s::%d] .....[0m
20063a10:	0000 0000 6f53 6572 412f 4f4d 7020 6761     ....Sore/AMO pag
20063a20:	2065 6177 6b6c 6320 6568 6b63 6620 6961     e walk check fai
20063a30:	006c 0000 7453 726f 2065 6461 7264 7365     l...Store addres
20063a40:	0073 0000 6f4c 6461 7020 6761 2065 6177     s...Load page wa
20063a50:	6b6c 6320 6568 6b63 6620 6961 006c 0000     lk check fail...
20063a60:	6f4c 6461 6120 6464 6572 7373 0000 0000     Load address....
20063a70:	6e49 7473 7572 7463 6f69 206e 6170 6567     Instruction page
20063a80:	7720 6c61 206b 6863 6365 206b 6166 6c69      walk check fail
20063a90:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
20063aa0:	6461 7264 7365 0073 7253 726f 2f65 4d41     address.Srore/AM
20063ab0:	204f 4d50 2050 6863 6365 206b 6166 6c69     O PMP check fail
20063ac0:	0000 0000 7453 726f 2f65 4d41 204f 6170     ....Store/AMO pa
20063ad0:	6567 7720 6c61 206b 7562 2073 7265 6f72     ge walk bus erro
20063ae0:	0072 0000 7453 726f 2065 7562 2073 7265     r...Store bus er
20063af0:	6f72 0072 7453 726f 2f65 4d41 204f 6461     ror.Store/AMO ad
20063b00:	7264 7365 2073 696d 6173 696c 6e67 6465     dress misaligned
20063b10:	0000 0000 6f4c 6461 5020 504d 6320 6568     ....Load PMP che
20063b20:	6b63 6620 6961 006c 6f4c 6461 5020 6761     ck fail.Load Pag
20063b30:	2065 6177 6b6c 6220 7375 6520 7272 726f     e walk bus error
20063b40:	0000 0000 6f4c 6461 6220 7375 6520 7272     ....Load bus err
20063b50:	726f 0000 6f4c 6461 6120 6464 6572 7373     or..Load address
20063b60:	6d20 7369 6c61 6769 656e 0064 6c49 656c      misaligned.Ille
20063b70:	6167 206c 6e69 7473 7572 7463 6f69 006e     gal instruction.
20063b80:	6e49 7473 7572 7463 6f69 206e 706f 6f63     Instruction opco
20063b90:	6564 0000 6e49 7473 7572 7463 6f69 206e     de..Instruction 
20063ba0:	4d50 2050 6863 6365 206b 6166 6c69 0000     PMP check fail..
20063bb0:	6e49 7473 7572 7463 6f69 206e 6150 6567     Instruction Page
20063bc0:	7720 6c61 206b 7562 2073 7265 6f72 0072      walk bus error.
20063bd0:	6e49 7473 7572 7463 6f69 206e 7562 2073     Instruction bus 
20063be0:	7265 6f72 0072 0000 7242 6e61 6863 7420     error...Branch t
20063bf0:	7261 6567 2074 696d 6173 696c 6e67 6465     arget misaligned
20063c00:	0000 0000 6154 6772 7465 6120 6464 6572     ....Target addre
20063c10:	7373 0000 6e49 7473 7572 7463 6f69 206e     ss..Instruction 
20063c20:	6461 7264 7365 2073 696d 6173 696c 6e67     address misalign
20063c30:	6465 0000 657a 6f72 0000 0000 6172 0000     ed..zero....ra..
20063c40:	7073 0000 7067 0000 7074 0000 3074 0000     sp..gp..tp..t0..
20063c50:	3174 0000 3274 0000 3073 662f 0070 0000     t1..t2..s0/fp...
20063c60:	3173 0000 3061 0000 3161 0000 3261 0000     s1..a0..a1..a2..
20063c70:	3361 0000 3461 0000 3561 0000 3661 0000     a3..a4..a5..a6..
20063c80:	3761 0000 3273 0000 3373 0000 3473 0000     a7..s2..s3..s4..
20063c90:	3573 0000 3673 0000 3773 0000 3873 0000     s5..s6..s7..s8..
20063ca0:	3973 0000 3173 0030 3173 0031 3374 0000     s9..s10.s11.t3..
20063cb0:	3474 0000 3574 0000 3674 0000 6e49 7473     t4..t5..t6..Inst
20063cc0:	7572 7463 6f69 206e 6361 6563 7373 6620     ruction access f
20063cd0:	7561 746c 0000 0000 7242 6165 706b 696f     ault....Breakpoi
20063ce0:	746e 0000 6f4c 6461 6120 6464 7365 2073     nt..Load addess 
20063cf0:	696d 6173 696c 6e67 6465 0000 6f4c 6461     misaligned..Load
20063d00:	6120 6363 7365 2073 6166 6c75 0074 0000      access fault...
20063d10:	7453 726f 2065 6461 7264 7365 2073 696d     Store address mi
20063d20:	6173 696c 6e67 6465 0000 0000 7453 726f     saligned....Stor
20063d30:	2065 6461 7264 7365 2073 6166 6c75 0074     e address fault.
20063d40:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
20063d50:	6620 6f72 206d 2055 6f6d 6564 0000 0000      from U mode....
20063d60:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
20063d70:	6620 6f72 206d 2053 6f6d 6564 0000 0000      from S mode....
20063d80:	6552 6573 7672 6465 6520 6378 7065 6974     Reserved excepti
20063d90:	6e6f 0000 6e45 6976 6f72 6d6e 6e65 2074     on..Environment 
20063da0:	6163 6c6c 6620 6f72 206d 204d 6f6d 6564     call from M mode
20063db0:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
20063dc0:	6170 6567 6620 7561 746c 0000 6f4c 6461     page fault..Load
20063dd0:	7020 6761 2065 6166 6c75 0074 7453 726f      page fault.Stor
20063de0:	2065 6170 6567 6620 7561 746c 0000 0000     e page fault....

20063df0 <__FUNCTION__.0>:
20063df0:	7263 7361 5f68 7564 706d 0000               crash_dump..

20063dfc <exception_cause_0_string>:
20063dfc:	3be8 2006 3c04 2006 3c14 2006 3a94 2006     .;. .<. .<. .:. 
	...

20063e14 <exception_cause_12_string>:
20063e14:	3a70 2006 3a94 2006 0000 0000 0000 0000     p:. .:. ........

20063e24 <exception_cause_13_string>:
20063e24:	3a44 2006 3a60 2006 0000 0000 0000 0000     D:. `:. ........

20063e34 <exception_cause_15_string>:
20063e34:	3a14 2006 3a34 2006 0000 0000 0000 0000     .:. 4:. ........

20063e44 <exception_cause_1_string>:
20063e44:	3b94 2006 3a94 2006 3bb0 2006 3a94 2006     .;. .:. .;. .:. 
20063e54:	3bd0 2006 3a94 2006 0000 0000 0000 0000     .;. .:. ........

20063e64 <exception_cause_2_string>:
20063e64:	3b6c 2006 3b80 2006 0000 0000 0000 0000     l;. .;. ........

20063e74 <exception_cause_4_string>:
20063e74:	3b54 2006 3a60 2006 0000 0000 0000 0000     T;. `:. ........

20063e84 <exception_cause_5_string>:
20063e84:	3b14 2006 3a60 2006 3b28 2006 3a60 2006     .;. `:. (;. `:. 
20063e94:	3b44 2006 0000 0000 0000 0000 0000 0000     D;. ............

20063ea4 <exception_cause_6_string>:
20063ea4:	3af4 2006 3a34 2006 0000 0000 0000 0000     .:. 4:. ........

20063eb4 <exception_cause_7_string>:
20063eb4:	3aa8 2006 3a34 2006 3ac4 2006 3a34 2006     .:. 4:. .:. 4:. 
20063ec4:	3ae4 2006 0000 0000 0000 0000 0000 0000     .:. ............

20063ed4 <exception_cause_string_tbl>:
20063ed4:	3dfc 2006 3e44 2006 3e64 2006 0000 0000     .=. D>. d>. ....
20063ee4:	3e74 2006 3e84 2006 3ea4 2006 3eb4 2006     t>. .>. .>. .>. 
	...
20063f04:	3e14 2006 3e24 2006 0000 0000 3e34 2006     .>. $>. ....4>. 

20063f14 <exception_code_string>:
20063f14:	3c14 2006 3cbc 2006 3b6c 2006 3cd8 2006     .<. .<. l;. .<. 
20063f24:	3ce4 2006 3cfc 2006 3d10 2006 3d2c 2006     .<. .<. .=. ,=. 
20063f34:	3d40 2006 3d60 2006 3d80 2006 3d94 2006     @=. `=. .=. .=. 
20063f44:	3db4 2006 3dcc 2006 0000 0000 3ddc 2006     .=. .=. .....=. 

20063f54 <register_string>:
20063f54:	3c34 2006 3c3c 2006 3c40 2006 3c44 2006     4<. <<. @<. D<. 
20063f64:	3c48 2006 3c4c 2006 3c50 2006 3c54 2006     H<. L<. P<. T<. 
20063f74:	3c58 2006 3c60 2006 3c64 2006 3c68 2006     X<. `<. d<. h<. 
20063f84:	3c6c 2006 3c70 2006 3c74 2006 3c78 2006     l<. p<. t<. x<. 
20063f94:	3c7c 2006 3c80 2006 3c84 2006 3c88 2006     |<. .<. .<. .<. 
20063fa4:	3c8c 2006 3c90 2006 3c94 2006 3c98 2006     .<. .<. .<. .<. 
20063fb4:	3c9c 2006 3ca0 2006 3ca4 2006 3ca8 2006     .<. .<. .<. .<. 
20063fc4:	3cac 2006 3cb0 2006 3cb4 2006 3cb8 2006     .<. .<. .<. .<. 
20063fd4:	4544 2056 5049 2043 5041 2049 736d 2067     DEV IPC API msg 
20063fe4:	554e 4c4c 000a 0000 6f48 7473 4120 4950     NULL....Host API
20063ff4:	5520 6b6e 6f6e 6e77 6520 7476 2528 2978      Unknown evt(%x)
20064004:	000a 0000 007a 2006 0088 2006 0140 2006     ....z.. ... @.. 
20064014:	00e0 2006 0206 2006 021a 2006 0290 2006     ... ... ... ... 
20064024:	00e0 2006 00e0 2006 616c 7473 6920 696e     ... ... last ini
20064034:	2063 7069 2063 6f6e 2074 6468 206c 000a     c ipc not hdl ..
20064044:	7348 4d74 6773 6553 646e 7720 6961 2074     HstMsgSend wait 
20064054:	6e69 6369 6920 6370 6420 6e6f 0a65 0000     inic ipc done...
20064064:	6e69 6369 615f 6970 685f 736f 5f74 6174     inic_api_host_ta
20064074:	6b73 0000 7243 6165 6574 6120 6970 685f     sk..Create api_h
20064084:	736f 5f74 6174 6b73 4520 7272 000a 0000     ost_task Err....
20064094:	4e49 4349 0000 0000 6e69 6369 6d5f 5f70     INIC....inic_mp_
200640a4:	6f63 6d6d 6e61 2064 6166 6c69 000a 0000     command fail....
200640b4:	7250 7669 7461 2065 654d 7373 6761 3a65     Private Message:
200640c4:	2520 0a73 0000 0000                          %s.....

200640cc <__func__.0>:
200640cc:	6e69 6369 615f 6970 685f 736f 5f74 656d     inic_api_host_me
200640dc:	7373 6761 5f65 6573 646e 0000 6f68 7473     ssage_send..host
200640ec:	735f 626b 785f 2078 6f6e 2074 7266 6565     _skb_xx not free
200640fc:	000a 0000 7325 3e3d 6b73 2062 616d 6c6c     ....%s=>skb mall
2006410c:	636f 6620 6961 216c 0d0a 0000 6f48 7473     oc fail!....Host
2006411c:	5520 6b6e 6f6e 6e77 6520 6576 746e 2528      Unknown event(%
2006412c:	786c 2129 0d0a 0000                         lx)!....

20064134 <__func__.1>:
20064134:	6e69 6369 685f 736f 5f74 6e69 7469 735f     inic_host_init_s
20064144:	626b 0000 6e69 6369 6d5f 6773 715f 745f     kb..inic_msg_q_t
20064154:	7361 006b 7243 6165 6574 6d20 6773 715f     ask.Create msg_q
20064164:	745f 7361 206b 7245 2172 000a 4f4e 6220     _task Err!..NO b
20064174:	6675 6620 726f 6e20 7765 6e20 646f 7365     uf for new nodes
20064184:	0a21 0000                                   !...

20064188 <__clz_tab>:
20064188:	0100 0202 0303 0303 0404 0404 0404 0404     ................
20064198:	0505 0505 0505 0505 0505 0505 0505 0505     ................
200641a8:	0606 0606 0606 0606 0606 0606 0606 0606     ................
200641b8:	0606 0606 0606 0606 0606 0606 0606 0606     ................
200641c8:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200641d8:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200641e8:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200641f8:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064208:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064218:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064228:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064238:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064248:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064258:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064268:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064278:	0808 0808 0808 0808 0808 0808 0808 0808     ................

20064288 <_ctype_>:
20064288:	2000 2020 2020 2020 2020 2828 2828 2028     .         ((((( 
20064298:	2020 2020 2020 2020 2020 2020 2020 2020                     
200642a8:	8820 1010 1010 1010 1010 1010 1010 1010      ...............
200642b8:	0410 0404 0404 0404 0404 1004 1010 1010     ................
200642c8:	1010 4141 4141 4141 0101 0101 0101 0101     ..AAAAAA........
200642d8:	0101 0101 0101 0101 0101 0101 1010 1010     ................
200642e8:	1010 4242 4242 4242 0202 0202 0202 0202     ..BBBBBB........
200642f8:	0202 0202 0202 0202 0202 0202 1010 1010     ................
20064308:	0020 0000 0000 0000 0000 0000 0000 0000      ...............
	...

2006438c <shell_cmd_table>:
2006438c:	2a9c 2006 0000 0000 b402 2005 2aa0 2006     .*. ....... .*. 
2006439c:	2ac4 2006 0002 0000 b55e 2005 2ac8 2006     .*. ....^.. .*. 
200643ac:	2be4 2006 0002 0000 b4b8 2005 2be8 2006     .+. ....... .+. 
200643bc:	2c78 2006 0004 0000 b46e 2005 2c80 2006     x,. ....n.. .,. 

200643cc <ipc_tickless_table>:
200643cc:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
200643dc:	0000 0000 0010 0000 0000 0000               ............

200643e8 <ipc_kr4cdsp_table>:
200643e8:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
200643f8:	0000 0000 0020 0000 0000 0000               .... .......

20064404 <ipc_KM4WKR4_table>:
20064404:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064414:	0000 0000 0010 0000 0002 0000               ............

20064420 <ipc_shell_table>:
20064420:	0001 0000 ccb2 2005 0000 0000 8eac 2005     ....... ....... 
20064430:	0000 0000 0010 0000 0003 0000               ............

2006443c <ipc_api_host_table>:
2006443c:	0000 0000 0366 2006 0000 0000 8eac 2005     ....f.. ....... 
2006444c:	0000 0000 0010 0000 0007 0000               ............

20064458 <ipc_host_event_table>:
20064458:	0000 0000 099a 2006 0000 0000 8eac 2005     ....... ....... 
20064468:	0000 0000 0010 0000 0006 0000               ............

20064474 <lib_pmc_git_rev>:
20064474:	6c20 6269 705f 636d 765f 7265 645f 6537      lib_pmc_ver_d7e
20064484:	6439 3435 3336 5f38 3032 3432 302f 2f36     9d54638_2024/06/
20064494:	3430 312d 3a34 3435 303a 0037               04-14:54:07.

200644a0 <lib_wifi_com_sec_git_rev>:
200644a0:	6c20 6269 775f 6669 5f69 6f63 5f6d 6573      lib_wifi_com_se
200644b0:	5f63 6576 5f72 3764 3965 3564 3634 3833     c_ver_d7e9d54638
200644c0:	325f 3230 2f34 3630 302f 2d34 3431 353a     _2024/06/04-14:5
200644d0:	3a34 3530 0000 0000                         4:05....

200644d8 <lib_wps_git_rev>:
200644d8:	6c20 6269 775f 7370 765f 7265 645f 6537      lib_wps_ver_d7e
200644e8:	6439 3435 3336 5f38 3032 3432 302f 2f36     9d54638_2024/06/
200644f8:	3430 312d 3a34 3435 303a 0039               04-14:54:09.

20064504 <__frame_dummy_init_array_entry>:
20064504:	0d7c 2006                                   |.. 

20064508 <__do_global_dtors_aux_fini_array_entry>:
20064508:	0d42 2006                                   B.. 

2006450c <__fini_array_end>:
	...

Disassembly of section .sram_image2.text.data:

20065520 <FLASH_ClockSwitch>:
20065520:	1101                	c.addi	sp,-32
20065522:	ca26                	c.swsp	s1,20(sp)
20065524:	c452                	c.swsp	s4,8(sp)
20065526:	ce06                	c.swsp	ra,28(sp)
20065528:	cc22                	c.swsp	s0,24(sp)
2006552a:	c84a                	c.swsp	s2,16(sp)
2006552c:	c64e                	c.swsp	s3,12(sp)
2006552e:	c256                	c.swsp	s5,4(sp)
20065530:	84aa                	c.mv	s1,a0
20065532:	8a2e                	c.mv	s4,a1
20065534:	c1dd                	c.beqz	a1,200655da <FLASH_ClockSwitch+0xba>
20065536:	da0f30ef          	jal	ra,20058ad6 <Systick_State>
2006553a:	89aa                	c.mv	s3,a0
2006553c:	dbaf30ef          	jal	ra,20058af6 <irq_disable_save>
20065540:	892a                	c.mv	s2,a0
20065542:	4501                	c.li	a0,0
20065544:	d66f30ef          	jal	ra,20058aaa <Systick_Cmd>
20065548:	20001ab7          	lui	s5,0x20001
2006554c:	4789                	c.li	a5,2
2006554e:	864a8413          	addi	s0,s5,-1948 # 20000864 <flash_init_para>
20065552:	08f49e63          	bne	s1,a5,200655ee <FLASH_ClockSwitch+0xce>
20065556:	4509                	c.li	a0,2
20065558:	dffa0097          	auipc	ra,0xdffa0
2006555c:	4e6080e7          	jalr	ra,1254(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065560:	4100c6b7          	lui	a3,0x4100c
20065564:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065568:	7775                	c.lui	a4,0xffffd
2006556a:	177d                	c.addi	a4,-1
2006556c:	8ff9                	c.and	a5,a4
2006556e:	24f6ae23          	sw	a5,604(a3)
20065572:	06344783          	lbu	a5,99(s0) # 9063 <shell_get_argv+0x18f>
20065576:	c7ad                	c.beqz	a5,200655e0 <FLASH_ClockSwitch+0xc0>
20065578:	4501                	c.li	a0,0
2006557a:	dff9e097          	auipc	ra,0xdff9e
2006557e:	400080e7          	jalr	ra,1024(ra) # 397a <FLASH_CalibrationNewCmd>
20065582:	4505                	c.li	a0,1
20065584:	dff9e097          	auipc	ra,0xdff9e
20065588:	1b4080e7          	jalr	ra,436(ra) # 3738 <FLASH_CalibrationPLLSel>
2006558c:	dff9e097          	auipc	ra,0xdff9e
20065590:	21e080e7          	jalr	ra,542(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065594:	4501                	c.li	a0,0
20065596:	dff9e097          	auipc	ra,0xdff9e
2006559a:	1a2080e7          	jalr	ra,418(ra) # 3738 <FLASH_CalibrationPLLSel>
2006559e:	dff9e097          	auipc	ra,0xdff9e
200655a2:	20c080e7          	jalr	ra,524(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
200655a6:	04040ea3          	sb	zero,93(s0)
200655aa:	00444583          	lbu	a1,4(s0)
200655ae:	864a8513          	addi	a0,s5,-1948
200655b2:	dff9d097          	auipc	ra,0xdff9d
200655b6:	712080e7          	jalr	ra,1810(ra) # 2cc4 <FLASH_SetSpiMode>
200655ba:	0c0a0c63          	beq	s4,zero,20065692 <FLASH_ClockSwitch+0x172>
200655be:	854e                	c.mv	a0,s3
200655c0:	ceaf30ef          	jal	ra,20058aaa <Systick_Cmd>
200655c4:	4462                	c.lwsp	s0,24(sp)
200655c6:	40f2                	c.lwsp	ra,28(sp)
200655c8:	44d2                	c.lwsp	s1,20(sp)
200655ca:	49b2                	c.lwsp	s3,12(sp)
200655cc:	4a22                	c.lwsp	s4,8(sp)
200655ce:	4a92                	c.lwsp	s5,4(sp)
200655d0:	854a                	c.mv	a0,s2
200655d2:	4942                	c.lwsp	s2,16(sp)
200655d4:	6105                	c.addi16sp	sp,32
200655d6:	d28f306f          	jal	zero,20058afe <irq_enable_restore>
200655da:	4901                	c.li	s2,0
200655dc:	4981                	c.li	s3,0
200655de:	b7ad                	c.j	20065548 <FLASH_ClockSwitch+0x28>
200655e0:	4581                	c.li	a1,0
200655e2:	4501                	c.li	a0,0
200655e4:	dff9e097          	auipc	ra,0xdff9e
200655e8:	59a080e7          	jalr	ra,1434(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
200655ec:	bf59                	c.j	20065582 <FLASH_ClockSwitch+0x62>
200655ee:	ccb5                	c.beqz	s1,2006566a <FLASH_ClockSwitch+0x14a>
200655f0:	4501                	c.li	a0,0
200655f2:	dff9e097          	auipc	ra,0xdff9e
200655f6:	146080e7          	jalr	ra,326(ra) # 3738 <FLASH_CalibrationPLLSel>
200655fa:	dff9e097          	auipc	ra,0xdff9e
200655fe:	152080e7          	jalr	ra,338(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065602:	4505                	c.li	a0,1
20065604:	dff9e097          	auipc	ra,0xdff9e
20065608:	134080e7          	jalr	ra,308(ra) # 3738 <FLASH_CalibrationPLLSel>
2006560c:	dff9e097          	auipc	ra,0xdff9e
20065610:	140080e7          	jalr	ra,320(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065614:	06344503          	lbu	a0,99(s0)
20065618:	891d                	c.andi	a0,7
2006561a:	dff9e097          	auipc	ra,0xdff9e
2006561e:	210080e7          	jalr	ra,528(ra) # 382a <FLASH_CalibrationPLLPS_Shift>
20065622:	4100c737          	lui	a4,0x4100c
20065626:	25c72783          	lw	a5,604(a4) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
2006562a:	668d                	c.lui	a3,0x3
2006562c:	8fd5                	c.or	a5,a3
2006562e:	24f72e23          	sw	a5,604(a4)
20065632:	4100c6b7          	lui	a3,0x4100c
20065636:	470d                	c.li	a4,3
20065638:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
2006563c:	8b8d                	c.andi	a5,3
2006563e:	fee79de3          	bne	a5,a4,20065638 <FLASH_ClockSwitch+0x118>
20065642:	06344783          	lbu	a5,99(s0)
20065646:	cf8d                	c.beqz	a5,20065680 <FLASH_ClockSwitch+0x160>
20065648:	4505                	c.li	a0,1
2006564a:	dff9e097          	auipc	ra,0xdff9e
2006564e:	330080e7          	jalr	ra,816(ra) # 397a <FLASH_CalibrationNewCmd>
20065652:	4505                	c.li	a0,1
20065654:	e091                	c.bnez	s1,20065658 <FLASH_ClockSwitch+0x138>
20065656:	4501                	c.li	a0,0
20065658:	dffa0097          	auipc	ra,0xdffa0
2006565c:	3e6080e7          	jalr	ra,998(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065660:	05c44783          	lbu	a5,92(s0)
20065664:	04f40ea3          	sb	a5,93(s0)
20065668:	b789                	c.j	200655aa <FLASH_ClockSwitch+0x8a>
2006566a:	4505                	c.li	a0,1
2006566c:	dff9e097          	auipc	ra,0xdff9e
20065670:	0cc080e7          	jalr	ra,204(ra) # 3738 <FLASH_CalibrationPLLSel>
20065674:	dff9e097          	auipc	ra,0xdff9e
20065678:	0d8080e7          	jalr	ra,216(ra) # 374c <FLASH_Calibration_PSPLL_Open>
2006567c:	4501                	c.li	a0,0
2006567e:	b759                	c.j	20065604 <FLASH_ClockSwitch+0xe4>
20065680:	05c44503          	lbu	a0,92(s0)
20065684:	4585                	c.li	a1,1
20065686:	1579                	c.addi	a0,-2
20065688:	dff9e097          	auipc	ra,0xdff9e
2006568c:	4f6080e7          	jalr	ra,1270(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
20065690:	b7c9                	c.j	20065652 <FLASH_ClockSwitch+0x132>
20065692:	40f2                	c.lwsp	ra,28(sp)
20065694:	4462                	c.lwsp	s0,24(sp)
20065696:	44d2                	c.lwsp	s1,20(sp)
20065698:	4942                	c.lwsp	s2,16(sp)
2006569a:	49b2                	c.lwsp	s3,12(sp)
2006569c:	4a22                	c.lwsp	s4,8(sp)
2006569e:	4a92                	c.lwsp	s5,4(sp)
200656a0:	6105                	c.addi16sp	sp,32
200656a2:	8082                	c.jr	ra

200656a4 <FLASH_UserMode_Exit>:
200656a4:	4100a737          	lui	a4,0x4100a
200656a8:	431c                	c.lw	a5,0(a4)
200656aa:	0786                	c.slli	a5,0x1
200656ac:	8385                	c.srli	a5,0x1
200656ae:	c31c                	c.sw	a5,0(a4)
200656b0:	431c                	c.lw	a5,0(a4)
200656b2:	fe07cfe3          	blt	a5,zero,200656b0 <FLASH_UserMode_Exit+0xc>
200656b6:	0ff0000f          	fence	iorw,iorw
200656ba:	0001                	c.addi	zero,0
200656bc:	0001                	c.addi	zero,0
200656be:	0001                	c.addi	zero,0
200656c0:	0001                	c.addi	zero,0
200656c2:	0001                	c.addi	zero,0
200656c4:	0001                	c.addi	zero,0
200656c6:	0001                	c.addi	zero,0
200656c8:	0001                	c.addi	zero,0
200656ca:	0001                	c.addi	zero,0
200656cc:	0001                	c.addi	zero,0
200656ce:	0001                	c.addi	zero,0
200656d0:	8082                	c.jr	ra

200656d2 <FLASH_WaitBusy_InUserMode>:
200656d2:	7179                	c.addi16sp	sp,-48
200656d4:	d422                	c.swsp	s0,40(sp)
200656d6:	20001437          	lui	s0,0x20001
200656da:	d226                	c.swsp	s1,36(sp)
200656dc:	d04a                	c.swsp	s2,32(sp)
200656de:	ce4e                	c.swsp	s3,28(sp)
200656e0:	cc52                	c.swsp	s4,24(sp)
200656e2:	ca56                	c.swsp	s5,20(sp)
200656e4:	d606                	c.swsp	ra,44(sp)
200656e6:	84aa                	c.mv	s1,a0
200656e8:	000107a3          	sb	zero,15(sp)
200656ec:	4781                	c.li	a5,0
200656ee:	4911                	c.li	s2,4
200656f0:	4985                	c.li	s3,1
200656f2:	4a0d                	c.li	s4,3
200656f4:	86440413          	addi	s0,s0,-1948 # 20000864 <flash_init_para>
200656f8:	4100aab7          	lui	s5,0x4100a
200656fc:	ec91                	c.bnez	s1,20065718 <FLASH_WaitBusy_InUserMode+0x46>
200656fe:	028aa783          	lw	a5,40(s5) # 4100a028 <__km4_bd_boot_download_addr__+0x10ff8028>
20065702:	8b85                	c.andi	a5,1
20065704:	ffe5                	c.bnez	a5,200656fc <FLASH_WaitBusy_InUserMode+0x2a>
20065706:	50b2                	c.lwsp	ra,44(sp)
20065708:	5422                	c.lwsp	s0,40(sp)
2006570a:	5492                	c.lwsp	s1,36(sp)
2006570c:	5902                	c.lwsp	s2,32(sp)
2006570e:	49f2                	c.lwsp	s3,28(sp)
20065710:	4a62                	c.lwsp	s4,24(sp)
20065712:	4ad2                	c.lwsp	s5,20(sp)
20065714:	6145                	c.addi16sp	sp,48
20065716:	8082                	c.jr	ra
20065718:	01249563          	bne	s1,s2,20065722 <FLASH_WaitBusy_InUserMode+0x50>
2006571c:	008aa783          	lw	a5,8(s5)
20065720:	b7cd                	c.j	20065702 <FLASH_WaitBusy_InUserMode+0x30>
20065722:	01349d63          	bne	s1,s3,2006573c <FLASH_WaitBusy_InUserMode+0x6a>
20065726:	05244503          	lbu	a0,82(s0)
2006572a:	00f10613          	addi	a2,sp,15
2006572e:	4585                	c.li	a1,1
20065730:	288d                	c.jal	200657a2 <FLASH_RxCmd_InUserMode>
20065732:	00f14783          	lbu	a5,15(sp)
20065736:	4818                	c.lw	a4,16(s0)
20065738:	8ff9                	c.and	a5,a4
2006573a:	b7e9                	c.j	20065704 <FLASH_WaitBusy_InUserMode+0x32>
2006573c:	fd4494e3          	bne	s1,s4,20065704 <FLASH_WaitBusy_InUserMode+0x32>
20065740:	05244503          	lbu	a0,82(s0)
20065744:	00f10613          	addi	a2,sp,15
20065748:	4585                	c.li	a1,1
2006574a:	28a1                	c.jal	200657a2 <FLASH_RxCmd_InUserMode>
2006574c:	00f14703          	lbu	a4,15(sp)
20065750:	4854                	c.lw	a3,20(s0)
20065752:	4785                	c.li	a5,1
20065754:	8ef9                	c.and	a3,a4
20065756:	d6dd                	c.beqz	a3,20065704 <FLASH_WaitBusy_InUserMode+0x32>
20065758:	481c                	c.lw	a5,16(s0)
2006575a:	8ff9                	c.and	a5,a4
2006575c:	00f037b3          	sltu	a5,zero,a5
20065760:	b755                	c.j	20065704 <FLASH_WaitBusy_InUserMode+0x32>

20065762 <FLASH_UserMode_Enter>:
20065762:	1141                	c.addi	sp,-16
20065764:	c606                	c.swsp	ra,12(sp)
20065766:	0ff0000f          	fence	iorw,iorw
2006576a:	0ff0000f          	fence	iorw,iorw
2006576e:	0001                	c.addi	zero,0
20065770:	0001                	c.addi	zero,0
20065772:	0001                	c.addi	zero,0
20065774:	0001                	c.addi	zero,0
20065776:	0001                	c.addi	zero,0
20065778:	0001                	c.addi	zero,0
2006577a:	0001                	c.addi	zero,0
2006577c:	0001                	c.addi	zero,0
2006577e:	0001                	c.addi	zero,0
20065780:	0001                	c.addi	zero,0
20065782:	0001                	c.addi	zero,0
20065784:	4501                	c.li	a0,0
20065786:	37b1                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
20065788:	4100a737          	lui	a4,0x4100a
2006578c:	431c                	c.lw	a5,0(a4)
2006578e:	800006b7          	lui	a3,0x80000
20065792:	8fd5                	c.or	a5,a3
20065794:	c31c                	c.sw	a5,0(a4)
20065796:	431c                	c.lw	a5,0(a4)
20065798:	fe07dfe3          	bge	a5,zero,20065796 <FLASH_UserMode_Enter+0x34>
2006579c:	40b2                	c.lwsp	ra,12(sp)
2006579e:	0141                	c.addi	sp,16
200657a0:	8082                	c.jr	ra

200657a2 <FLASH_RxCmd_InUserMode>:
200657a2:	1101                	c.addi	sp,-32
200657a4:	cc22                	c.swsp	s0,24(sp)
200657a6:	20001437          	lui	s0,0x20001
200657aa:	ca26                	c.swsp	s1,20(sp)
200657ac:	c64e                	c.swsp	s3,12(sp)
200657ae:	84ae                	c.mv	s1,a1
200657b0:	89aa                	c.mv	s3,a0
200657b2:	4581                	c.li	a1,0
200657b4:	86440513          	addi	a0,s0,-1948 # 20000864 <flash_init_para>
200657b8:	c84a                	c.swsp	s2,16(sp)
200657ba:	ce06                	c.swsp	ra,28(sp)
200657bc:	8932                	c.mv	s2,a2
200657be:	dff9d097          	auipc	ra,0xdff9d
200657c2:	506080e7          	jalr	ra,1286(ra) # 2cc4 <FLASH_SetSpiMode>
200657c6:	4100a7b7          	lui	a5,0x4100a
200657ca:	10078613          	addi	a2,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
200657ce:	4e18                	c.lw	a4,24(a2)
200657d0:	fff106b7          	lui	a3,0xfff10
200657d4:	fff68593          	addi	a1,a3,-1 # fff0ffff <__ctrace_end__+0x35f0ffff>
200657d8:	8f6d                	c.and	a4,a1
200657da:	ce18                	c.sw	a4,24(a2)
200657dc:	00849713          	slli	a4,s1,0x8
200657e0:	8321                	c.srli	a4,0x8
200657e2:	c3d8                	c.sw	a4,4(a5)
200657e4:	1207a823          	sw	zero,304(a5)
200657e8:	4398                	c.lw	a4,0(a5)
200657ea:	cff68693          	addi	a3,a3,-769
200657ee:	8f75                	c.and	a4,a3
200657f0:	30076713          	ori	a4,a4,768
200657f4:	c398                	c.sw	a4,0(a5)
200657f6:	07378023          	sb	s3,96(a5)
200657fa:	4705                	c.li	a4,1
200657fc:	c798                	c.sw	a4,8(a5)
200657fe:	4100a6b7          	lui	a3,0x4100a
20065802:	4781                	c.li	a5,0
20065804:	02979463          	bne	a5,s1,2006582c <FLASH_RxCmd_InUserMode+0x8a>
20065808:	4511                	c.li	a0,4
2006580a:	35e1                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
2006580c:	86440793          	addi	a5,s0,-1948
20065810:	86440513          	addi	a0,s0,-1948
20065814:	4462                	c.lwsp	s0,24(sp)
20065816:	40f2                	c.lwsp	ra,28(sp)
20065818:	44d2                	c.lwsp	s1,20(sp)
2006581a:	4942                	c.lwsp	s2,16(sp)
2006581c:	49b2                	c.lwsp	s3,12(sp)
2006581e:	0047c583          	lbu	a1,4(a5)
20065822:	6105                	c.addi16sp	sp,32
20065824:	dff9d317          	auipc	t1,0xdff9d
20065828:	4a030067          	jalr	zero,1184(t1) # 2cc4 <FLASH_SetSpiMode>
2006582c:	5698                	c.lw	a4,40(a3)
2006582e:	8b21                	c.andi	a4,8
20065830:	db71                	c.beqz	a4,20065804 <FLASH_RxCmd_InUserMode+0x62>
20065832:	0606c603          	lbu	a2,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20065836:	00f90733          	add	a4,s2,a5
2006583a:	0785                	c.addi	a5,1
2006583c:	00c70023          	sb	a2,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20065840:	b7d1                	c.j	20065804 <FLASH_RxCmd_InUserMode+0x62>

20065842 <FLASH_RxCmd>:
20065842:	1101                	c.addi	sp,-32
20065844:	ce06                	c.swsp	ra,28(sp)
20065846:	c62a                	c.swsp	a0,12(sp)
20065848:	c42e                	c.swsp	a1,8(sp)
2006584a:	c232                	c.swsp	a2,4(sp)
2006584c:	3f19                	c.jal	20065762 <FLASH_UserMode_Enter>
2006584e:	4612                	c.lwsp	a2,4(sp)
20065850:	45a2                	c.lwsp	a1,8(sp)
20065852:	4532                	c.lwsp	a0,12(sp)
20065854:	37b9                	c.jal	200657a2 <FLASH_RxCmd_InUserMode>
20065856:	40f2                	c.lwsp	ra,28(sp)
20065858:	6105                	c.addi16sp	sp,32
2006585a:	b5a9                	c.j	200656a4 <FLASH_UserMode_Exit>

2006585c <FLASH_TxCmd_InUserMode>:
2006585c:	4100a7b7          	lui	a5,0x4100a
20065860:	1141                	c.addi	sp,-16
20065862:	c606                	c.swsp	ra,12(sp)
20065864:	c422                	c.swsp	s0,8(sp)
20065866:	c226                	c.swsp	s1,4(sp)
20065868:	c04a                	c.swsp	s2,0(sp)
2006586a:	10078713          	addi	a4,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
2006586e:	01872903          	lw	s2,24(a4)
20065872:	4384                	c.lw	s1,0(a5)
20065874:	4394                	c.lw	a3,0(a5)
20065876:	ffc10837          	lui	a6,0xffc10
2006587a:	cff80813          	addi	a6,a6,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
2006587e:	0106f6b3          	and	a3,a3,a6
20065882:	c394                	c.sw	a3,0(a5)
20065884:	4f14                	c.lw	a3,24(a4)
20065886:	fff10837          	lui	a6,0xfff10
2006588a:	187d                	c.addi	a6,-1
2006588c:	0106f6b3          	and	a3,a3,a6
20065890:	cf14                	c.sw	a3,24(a4)
20065892:	0007a223          	sw	zero,4(a5)
20065896:	db0c                	c.sw	a1,48(a4)
20065898:	06a78023          	sb	a0,96(a5)
2006589c:	4100a737          	lui	a4,0x4100a
200658a0:	4781                	c.li	a5,0
200658a2:	02b7e163          	bltu	a5,a1,200658c4 <FLASH_TxCmd_InUserMode+0x68>
200658a6:	4100a437          	lui	s0,0x4100a
200658aa:	4785                	c.li	a5,1
200658ac:	c41c                	c.sw	a5,8(s0)
200658ae:	4511                	c.li	a0,4
200658b0:	350d                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
200658b2:	11242c23          	sw	s2,280(s0) # 4100a118 <__km4_bd_boot_download_addr__+0x10ff8118>
200658b6:	c004                	c.sw	s1,0(s0)
200658b8:	40b2                	c.lwsp	ra,12(sp)
200658ba:	4422                	c.lwsp	s0,8(sp)
200658bc:	4492                	c.lwsp	s1,4(sp)
200658be:	4902                	c.lwsp	s2,0(sp)
200658c0:	0141                	c.addi	sp,16
200658c2:	8082                	c.jr	ra
200658c4:	00f606b3          	add	a3,a2,a5
200658c8:	0006c683          	lbu	a3,0(a3)
200658cc:	0785                	c.addi	a5,1
200658ce:	06d70023          	sb	a3,96(a4) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
200658d2:	bfc1                	c.j	200658a2 <FLASH_TxCmd_InUserMode+0x46>

200658d4 <FLASH_WriteEn_InUserMode>:
200658d4:	1141                	c.addi	sp,-16
200658d6:	4505                	c.li	a0,1
200658d8:	c606                	c.swsp	ra,12(sp)
200658da:	3be5                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
200658dc:	200017b7          	lui	a5,0x20001
200658e0:	8b47c503          	lbu	a0,-1868(a5) # 200008b4 <flash_init_para+0x50>
200658e4:	4601                	c.li	a2,0
200658e6:	4581                	c.li	a1,0
200658e8:	3f95                	c.jal	2006585c <FLASH_TxCmd_InUserMode>
200658ea:	40b2                	c.lwsp	ra,12(sp)
200658ec:	450d                	c.li	a0,3
200658ee:	0141                	c.addi	sp,16
200658f0:	b3cd                	c.j	200656d2 <FLASH_WaitBusy_InUserMode>

200658f2 <FLASH_TxData>:
200658f2:	1101                	c.addi	sp,-32
200658f4:	0ff57793          	andi	a5,a0,255
200658f8:	cc22                	c.swsp	s0,24(sp)
200658fa:	ca26                	c.swsp	s1,20(sp)
200658fc:	c84a                	c.swsp	s2,16(sp)
200658fe:	ce06                	c.swsp	ra,28(sp)
20065900:	c64e                	c.swsp	s3,12(sp)
20065902:	97ae                	c.add	a5,a1
20065904:	10000713          	addi	a4,zero,256
20065908:	842a                	c.mv	s0,a0
2006590a:	84ae                	c.mv	s1,a1
2006590c:	8932                	c.mv	s2,a2
2006590e:	00f77a63          	bgeu	a4,a5,20065922 <FLASH_TxData+0x30>
20065912:	20062537          	lui	a0,0x20062
20065916:	23a00593          	addi	a1,zero,570
2006591a:	88c50513          	addi	a0,a0,-1908 # 2006188c <strtol+0xe>
2006591e:	c21f90ef          	jal	ra,2005f53e <io_assert_failed>
20065922:	3581                	c.jal	20065762 <FLASH_UserMode_Enter>
20065924:	3f45                	c.jal	200658d4 <FLASH_WriteEn_InUserMode>
20065926:	4100a7b7          	lui	a5,0x4100a
2006592a:	0007a983          	lw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
2006592e:	4398                	c.lw	a4,0(a5)
20065930:	ffc106b7          	lui	a3,0xffc10
20065934:	cff68693          	addi	a3,a3,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
20065938:	8f75                	c.and	a4,a3
2006593a:	200016b7          	lui	a3,0x20001
2006593e:	c398                	c.sw	a4,0(a5)
20065940:	86468693          	addi	a3,a3,-1948 # 20000864 <flash_init_para>
20065944:	0646c703          	lbu	a4,100(a3)
20065948:	1187a603          	lw	a2,280(a5)
2006594c:	000f05b7          	lui	a1,0xf0
20065950:	0742                	c.slli	a4,0x10
20065952:	8f6d                	c.and	a4,a1
20065954:	fff105b7          	lui	a1,0xfff10
20065958:	15fd                	c.addi	a1,-1
2006595a:	8e6d                	c.and	a2,a1
2006595c:	8f51                	c.or	a4,a2
2006595e:	10e7ac23          	sw	a4,280(a5)
20065962:	00849713          	slli	a4,s1,0x8
20065966:	0007a223          	sw	zero,4(a5)
2006596a:	8321                	c.srli	a4,0x8
2006596c:	12e7a823          	sw	a4,304(a5)
20065970:	4709                	c.li	a4,2
20065972:	06e78023          	sb	a4,96(a5)
20065976:	0646c683          	lbu	a3,100(a3)
2006597a:	4711                	c.li	a4,4
2006597c:	00e69663          	bne	a3,a4,20065988 <FLASH_TxData+0x96>
20065980:	01845713          	srli	a4,s0,0x18
20065984:	06e78023          	sb	a4,96(a5)
20065988:	01045713          	srli	a4,s0,0x10
2006598c:	4100a7b7          	lui	a5,0x4100a
20065990:	0ff77713          	andi	a4,a4,255
20065994:	06e78023          	sb	a4,96(a5) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20065998:	00845713          	srli	a4,s0,0x8
2006599c:	0ff77713          	andi	a4,a4,255
200659a0:	06e78023          	sb	a4,96(a5)
200659a4:	0ff47413          	andi	s0,s0,255
200659a8:	06878023          	sb	s0,96(a5)
200659ac:	4100a6b7          	lui	a3,0x4100a
200659b0:	4781                	c.li	a5,0
200659b2:	00978563          	beq	a5,s1,200659bc <FLASH_TxData+0xca>
200659b6:	5698                	c.lw	a4,40(a3)
200659b8:	8b09                	c.andi	a4,2
200659ba:	eb05                	c.bnez	a4,200659ea <FLASH_TxData+0xf8>
200659bc:	4685                	c.li	a3,1
200659be:	4100a737          	lui	a4,0x4100a
200659c2:	c714                	c.sw	a3,8(a4)
200659c4:	4100a6b7          	lui	a3,0x4100a
200659c8:	0297e963          	bltu	a5,s1,200659fa <FLASH_TxData+0x108>
200659cc:	4511                	c.li	a0,4
200659ce:	3311                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
200659d0:	4100a7b7          	lui	a5,0x4100a
200659d4:	0137a023          	sw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200659d8:	4505                	c.li	a0,1
200659da:	39e5                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
200659dc:	4462                	c.lwsp	s0,24(sp)
200659de:	40f2                	c.lwsp	ra,28(sp)
200659e0:	44d2                	c.lwsp	s1,20(sp)
200659e2:	4942                	c.lwsp	s2,16(sp)
200659e4:	49b2                	c.lwsp	s3,12(sp)
200659e6:	6105                	c.addi16sp	sp,32
200659e8:	b975                	c.j	200656a4 <FLASH_UserMode_Exit>
200659ea:	00f90733          	add	a4,s2,a5
200659ee:	00074703          	lbu	a4,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200659f2:	0785                	c.addi	a5,1
200659f4:	06e68023          	sb	a4,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
200659f8:	bf6d                	c.j	200659b2 <FLASH_TxData+0xc0>
200659fa:	5698                	c.lw	a4,40(a3)
200659fc:	8b09                	c.andi	a4,2
200659fe:	d769                	c.beqz	a4,200659c8 <FLASH_TxData+0xd6>
20065a00:	00f90733          	add	a4,s2,a5
20065a04:	00074703          	lbu	a4,0(a4)
20065a08:	0785                	c.addi	a5,1
20065a0a:	06e68023          	sb	a4,96(a3)
20065a0e:	bf6d                	c.j	200659c8 <FLASH_TxData+0xd6>

20065a10 <FLASH_SetStatus>:
20065a10:	1101                	c.addi	sp,-32
20065a12:	ce06                	c.swsp	ra,28(sp)
20065a14:	cc22                	c.swsp	s0,24(sp)
20065a16:	c62a                	c.swsp	a0,12(sp)
20065a18:	842e                	c.mv	s0,a1
20065a1a:	c432                	c.swsp	a2,8(sp)
20065a1c:	3399                	c.jal	20065762 <FLASH_UserMode_Enter>
20065a1e:	3d5d                	c.jal	200658d4 <FLASH_WriteEn_InUserMode>
20065a20:	4622                	c.lwsp	a2,8(sp)
20065a22:	4532                	c.lwsp	a0,12(sp)
20065a24:	0ff47593          	andi	a1,s0,255
20065a28:	3d15                	c.jal	2006585c <FLASH_TxCmd_InUserMode>
20065a2a:	4505                	c.li	a0,1
20065a2c:	315d                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
20065a2e:	4462                	c.lwsp	s0,24(sp)
20065a30:	40f2                	c.lwsp	ra,28(sp)
20065a32:	6105                	c.addi16sp	sp,32
20065a34:	b985                	c.j	200656a4 <FLASH_UserMode_Exit>

20065a36 <FLASH_SetStatusBits>:
20065a36:	7179                	c.addi16sp	sp,-48
20065a38:	d606                	c.swsp	ra,44(sp)
20065a3a:	d422                	c.swsp	s0,40(sp)
20065a3c:	d226                	c.swsp	s1,36(sp)
20065a3e:	ce4e                	c.swsp	s3,28(sp)
20065a40:	842a                	c.mv	s0,a0
20065a42:	89ae                	c.mv	s3,a1
20065a44:	200014b7          	lui	s1,0x20001
20065a48:	d04a                	c.swsp	s2,32(sp)
20065a4a:	3b21                	c.jal	20065762 <FLASH_UserMode_Enter>
20065a4c:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
20065a50:	0527c503          	lbu	a0,82(a5)
20065a54:	0070                	c.addi4spn	a2,sp,12
20065a56:	4585                	c.li	a1,1
20065a58:	33a9                	c.jal	200657a2 <FLASH_RxCmd_InUserMode>
20065a5a:	4785                	c.li	a5,1
20065a5c:	00c14703          	lbu	a4,12(sp)
20065a60:	86448493          	addi	s1,s1,-1948
20065a64:	04f99d63          	bne	s3,a5,20065abe <FLASH_SetStatusBits+0x88>
20065a68:	00e467b3          	or	a5,s0,a4
20065a6c:	0ff7f793          	andi	a5,a5,255
20065a70:	00f10623          	sb	a5,12(sp)
20065a74:	4c9c                	c.lw	a5,24(s1)
20065a76:	4905                	c.li	s2,1
20065a78:	c385                	c.beqz	a5,20065a98 <FLASH_SetStatusBits+0x62>
20065a7a:	0534c503          	lbu	a0,83(s1)
20065a7e:	00d10613          	addi	a2,sp,13
20065a82:	4585                	c.li	a1,1
20065a84:	3b39                	c.jal	200657a2 <FLASH_RxCmd_InUserMode>
20065a86:	00d14503          	lbu	a0,13(sp)
20065a8a:	8021                	c.srli	s0,0x8
20065a8c:	03299d63          	bne	s3,s2,20065ac6 <FLASH_SetStatusBits+0x90>
20065a90:	8c49                	c.or	s0,a0
20065a92:	008106a3          	sb	s0,13(sp)
20065a96:	4909                	c.li	s2,2
20065a98:	0554c783          	lbu	a5,85(s1)
20065a9c:	eb8d                	c.bnez	a5,20065ace <FLASH_SetStatusBits+0x98>
20065a9e:	3d1d                	c.jal	200658d4 <FLASH_WriteEn_InUserMode>
20065aa0:	0544c503          	lbu	a0,84(s1)
20065aa4:	0070                	c.addi4spn	a2,sp,12
20065aa6:	85ca                	c.mv	a1,s2
20065aa8:	3b55                	c.jal	2006585c <FLASH_TxCmd_InUserMode>
20065aaa:	4505                	c.li	a0,1
20065aac:	311d                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
20065aae:	3edd                	c.jal	200656a4 <FLASH_UserMode_Exit>
20065ab0:	50b2                	c.lwsp	ra,44(sp)
20065ab2:	5422                	c.lwsp	s0,40(sp)
20065ab4:	5492                	c.lwsp	s1,36(sp)
20065ab6:	5902                	c.lwsp	s2,32(sp)
20065ab8:	49f2                	c.lwsp	s3,28(sp)
20065aba:	6145                	c.addi16sp	sp,48
20065abc:	8082                	c.jr	ra
20065abe:	fff44793          	xori	a5,s0,-1
20065ac2:	8ff9                	c.and	a5,a4
20065ac4:	b765                	c.j	20065a6c <FLASH_SetStatusBits+0x36>
20065ac6:	fff44413          	xori	s0,s0,-1
20065aca:	8c69                	c.and	s0,a0
20065acc:	b7d9                	c.j	20065a92 <FLASH_SetStatusBits+0x5c>
20065ace:	3519                	c.jal	200658d4 <FLASH_WriteEn_InUserMode>
20065ad0:	0544c503          	lbu	a0,84(s1)
20065ad4:	0070                	c.addi4spn	a2,sp,12
20065ad6:	4585                	c.li	a1,1
20065ad8:	3351                	c.jal	2006585c <FLASH_TxCmd_InUserMode>
20065ada:	4505                	c.li	a0,1
20065adc:	3edd                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
20065ade:	3bdd                	c.jal	200658d4 <FLASH_WriteEn_InUserMode>
20065ae0:	0554c503          	lbu	a0,85(s1)
20065ae4:	00d10613          	addi	a2,sp,13
20065ae8:	4585                	c.li	a1,1
20065aea:	bf7d                	c.j	20065aa8 <FLASH_SetStatusBits+0x72>

20065aec <FLASH_Erase>:
20065aec:	7179                	c.addi16sp	sp,-48
20065aee:	d422                	c.swsp	s0,40(sp)
20065af0:	d04a                	c.swsp	s2,32(sp)
20065af2:	d606                	c.swsp	ra,44(sp)
20065af4:	d226                	c.swsp	s1,36(sp)
20065af6:	4789                	c.li	a5,2
20065af8:	892a                	c.mv	s2,a0
20065afa:	842e                	c.mv	s0,a1
20065afc:	00a7fa63          	bgeu	a5,a0,20065b10 <FLASH_Erase+0x24>
20065b00:	20062537          	lui	a0,0x20062
20065b04:	2d700593          	addi	a1,zero,727
20065b08:	88c50513          	addi	a0,a0,-1908 # 2006188c <strtol+0xe>
20065b0c:	a33f90ef          	jal	ra,2005f53e <io_assert_failed>
20065b10:	200014b7          	lui	s1,0x20001
20065b14:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
20065b18:	0647c583          	lbu	a1,100(a5)
20065b1c:	00845713          	srli	a4,s0,0x8
20065b20:	01045793          	srli	a5,s0,0x10
20065b24:	4611                	c.li	a2,4
20065b26:	0ff7f793          	andi	a5,a5,255
20065b2a:	0ff77713          	andi	a4,a4,255
20065b2e:	0ff47693          	andi	a3,s0,255
20065b32:	86448493          	addi	s1,s1,-1948
20065b36:	04c59463          	bne	a1,a2,20065b7e <FLASH_Erase+0x92>
20065b3a:	01845713          	srli	a4,s0,0x18
20065b3e:	00f10ea3          	sb	a5,29(sp)
20065b42:	00841793          	slli	a5,s0,0x8
20065b46:	0442                	c.slli	s0,0x10
20065b48:	8041                	c.srli	s0,0x10
20065b4a:	8021                	c.srli	s0,0x8
20065b4c:	8c5d                	c.or	s0,a5
20065b4e:	00e10e23          	sb	a4,28(sp)
20065b52:	00811f23          	sh	s0,30(sp)
20065b56:	c62e                	c.swsp	a1,12(sp)
20065b58:	3129                	c.jal	20065762 <FLASH_UserMode_Enter>
20065b5a:	3bad                	c.jal	200658d4 <FLASH_WriteEn_InUserMode>
20065b5c:	45b2                	c.lwsp	a1,12(sp)
20065b5e:	02091863          	bne	s2,zero,20065b8e <FLASH_Erase+0xa2>
20065b62:	0564c503          	lbu	a0,86(s1)
20065b66:	4601                	c.li	a2,0
20065b68:	4581                	c.li	a1,0
20065b6a:	39cd                	c.jal	2006585c <FLASH_TxCmd_InUserMode>
20065b6c:	4505                	c.li	a0,1
20065b6e:	3695                	c.jal	200656d2 <FLASH_WaitBusy_InUserMode>
20065b70:	3e15                	c.jal	200656a4 <FLASH_UserMode_Exit>
20065b72:	50b2                	c.lwsp	ra,44(sp)
20065b74:	5422                	c.lwsp	s0,40(sp)
20065b76:	5492                	c.lwsp	s1,36(sp)
20065b78:	5902                	c.lwsp	s2,32(sp)
20065b7a:	6145                	c.addi16sp	sp,48
20065b7c:	8082                	c.jr	ra
20065b7e:	00f10e23          	sb	a5,28(sp)
20065b82:	00e10ea3          	sb	a4,29(sp)
20065b86:	00d10f23          	sb	a3,30(sp)
20065b8a:	458d                	c.li	a1,3
20065b8c:	b7e9                	c.j	20065b56 <FLASH_Erase+0x6a>
20065b8e:	4785                	c.li	a5,1
20065b90:	00f91663          	bne	s2,a5,20065b9c <FLASH_Erase+0xb0>
20065b94:	0574c503          	lbu	a0,87(s1)
20065b98:	0870                	c.addi4spn	a2,sp,28
20065b9a:	bfc1                	c.j	20065b6a <FLASH_Erase+0x7e>
20065b9c:	4789                	c.li	a5,2
20065b9e:	fcf917e3          	bne	s2,a5,20065b6c <FLASH_Erase+0x80>
20065ba2:	0584c503          	lbu	a0,88(s1)
20065ba6:	0870                	c.addi4spn	a2,sp,28
20065ba8:	b7c9                	c.j	20065b6a <FLASH_Erase+0x7e>

20065baa <plic_get_pending>:
20065baa:	f1402773          	csrrs	a4,mhartid,zero
20065bae:	00251793          	slli	a5,a0,0x2
20065bb2:	80001537          	lui	a0,0x80001
20065bb6:	c0050513          	addi	a0,a0,-1024 # 80000c00 <__ctrace_end__+0xb6000c00>
20065bba:	97aa                	c.add	a5,a0
20065bbc:	0722                	c.slli	a4,0x8
20065bbe:	97ba                	c.add	a5,a4
20065bc0:	4388                	c.lw	a0,0(a5)
20065bc2:	8082                	c.jr	ra

20065bc4 <plic_interrupt_handler>:
20065bc4:	7179                	c.addi16sp	sp,-48
20065bc6:	d422                	c.swsp	s0,40(sp)
20065bc8:	d226                	c.swsp	s1,36(sp)
20065bca:	ce4e                	c.swsp	s3,28(sp)
20065bcc:	d606                	c.swsp	ra,44(sp)
20065bce:	d04a                	c.swsp	s2,32(sp)
20065bd0:	0044                	c.addi4spn	s1,sp,4
20065bd2:	4401                	c.li	s0,0
20065bd4:	498d                	c.li	s3,3
20065bd6:	8522                	c.mv	a0,s0
20065bd8:	3fc9                	c.jal	20065baa <plic_get_pending>
20065bda:	892a                	c.mv	s2,a0
20065bdc:	8522                	c.mv	a0,s0
20065bde:	dffa0097          	auipc	ra,0xdffa0
20065be2:	b44080e7          	jalr	ra,-1212(ra) # 5722 <plic_get_enable>
20065be6:	c088                	c.sw	a0,0(s1)
20065be8:	00a975b3          	and	a1,s2,a0
20065bec:	8522                	c.mv	a0,s0
20065bee:	0405                	c.addi	s0,1
20065bf0:	dffa0097          	auipc	ra,0xdffa0
20065bf4:	b1a080e7          	jalr	ra,-1254(ra) # 570a <plic_set_enable>
20065bf8:	0491                	c.addi	s1,4
20065bfa:	fd341ee3          	bne	s0,s3,20065bd6 <plic_interrupt_handler+0x12>
20065bfe:	0ff0000f          	fence	iorw,iorw
20065c02:	dffa0097          	auipc	ra,0xdffa0
20065c06:	bd4080e7          	jalr	ra,-1068(ra) # 57d6 <plic_claim_irq>
20065c0a:	4592                	c.lwsp	a1,4(sp)
20065c0c:	eb018493          	addi	s1,gp,-336 # 20065f88 <plic_irq_id>
20065c10:	c088                	c.sw	a0,0(s1)
20065c12:	4501                	c.li	a0,0
20065c14:	dffa0097          	auipc	ra,0xdffa0
20065c18:	af6080e7          	jalr	ra,-1290(ra) # 570a <plic_set_enable>
20065c1c:	45a2                	c.lwsp	a1,8(sp)
20065c1e:	4505                	c.li	a0,1
20065c20:	eb018413          	addi	s0,gp,-336 # 20065f88 <plic_irq_id>
20065c24:	dffa0097          	auipc	ra,0xdffa0
20065c28:	ae6080e7          	jalr	ra,-1306(ra) # 570a <plic_set_enable>
20065c2c:	45b2                	c.lwsp	a1,12(sp)
20065c2e:	4509                	c.li	a0,2
20065c30:	dffa0097          	auipc	ra,0xdffa0
20065c34:	ada080e7          	jalr	ra,-1318(ra) # 570a <plic_set_enable>
20065c38:	4084                	c.lw	s1,0(s1)
20065c3a:	c4b1                	c.beqz	s1,20065c86 <plic_interrupt_handler+0xc2>
20065c3c:	02048493          	addi	s1,s1,32
20065c40:	8526                	c.mv	a0,s1
20065c42:	77e000e7          	jalr	ra,1918(zero) # 0 <Reset_Handler>
20065c46:	892a                	c.mv	s2,a0
20065c48:	c531                	c.beqz	a0,20065c94 <plic_interrupt_handler+0xd0>
20065c4a:	dffa0097          	auipc	ra,0xdffa0
20065c4e:	bc8080e7          	jalr	ra,-1080(ra) # 5812 <plic_get_threshold>
20065c52:	89aa                	c.mv	s3,a0
20065c54:	4008                	c.lw	a0,0(s0)
20065c56:	dffa0097          	auipc	ra,0xdffa0
20065c5a:	b50080e7          	jalr	ra,-1200(ra) # 57a6 <plic_get_irq_priority>
20065c5e:	dffa0097          	auipc	ra,0xdffa0
20065c62:	b9c080e7          	jalr	ra,-1124(ra) # 57fa <plic_set_threshold>
20065c66:	8526                	c.mv	a0,s1
20065c68:	7b2000e7          	jalr	ra,1970(zero) # 0 <Reset_Handler>
20065c6c:	9902                	c.jalr	s2
20065c6e:	854e                	c.mv	a0,s3
20065c70:	dffa0097          	auipc	ra,0xdffa0
20065c74:	b8a080e7          	jalr	ra,-1142(ra) # 57fa <plic_set_threshold>
20065c78:	4008                	c.lw	a0,0(s0)
20065c7a:	dffa0097          	auipc	ra,0xdffa0
20065c7e:	b6e080e7          	jalr	ra,-1170(ra) # 57e8 <plic_complete_irq>
20065c82:	00042023          	sw	zero,0(s0)
20065c86:	50b2                	c.lwsp	ra,44(sp)
20065c88:	5422                	c.lwsp	s0,40(sp)
20065c8a:	5492                	c.lwsp	s1,36(sp)
20065c8c:	5902                	c.lwsp	s2,32(sp)
20065c8e:	49f2                	c.lwsp	s3,28(sp)
20065c90:	6145                	c.addi16sp	sp,48
20065c92:	8082                	c.jr	ra
20065c94:	200626b7          	lui	a3,0x20062
20065c98:	200625b7          	lui	a1,0x20062
20065c9c:	8726                	c.mv	a4,s1
20065c9e:	d7c68693          	addi	a3,a3,-644 # 20061d7c <pmap_func+0xd4>
20065ca2:	05700613          	addi	a2,zero,87
20065ca6:	da058593          	addi	a1,a1,-608 # 20061da0 <pmap_func+0xf8>
20065caa:	450d                	c.li	a0,3
20065cac:	9c4f50ef          	jal	ra,2005ae70 <rtk_log_write>
20065cb0:	b7e1                	c.j	20065c78 <plic_interrupt_handler+0xb4>

20065cb2 <plic_get_active_irq_id>:
20065cb2:	eb01a503          	lw	a0,-336(gp) # 20065f88 <plic_irq_id>
20065cb6:	8082                	c.jr	ra

20065cb8 <__NVIC_GetActive_IRQ>:
20065cb8:	06f00793          	addi	a5,zero,111
20065cbc:	00a7d463          	bge	a5,a0,20065cc4 <__NVIC_GetActive_IRQ+0xc>
20065cc0:	4501                	c.li	a0,0
20065cc2:	8082                	c.jr	ra
20065cc4:	34202773          	csrrs	a4,mcause,zero
20065cc8:	00171793          	slli	a5,a4,0x1
20065ccc:	8385                	c.srli	a5,0x1
20065cce:	00055763          	bge	a0,zero,20065cdc <__NVIC_GetActive_IRQ+0x24>
20065cd2:	0541                	c.addi	a0,16
20065cd4:	fe0746e3          	blt	a4,zero,20065cc0 <__NVIC_GetActive_IRQ+0x8>
20065cd8:	8d1d                	c.sub	a0,a5
20065cda:	a821                	c.j	20065cf2 <__NVIC_GetActive_IRQ+0x3a>
20065cdc:	fe0752e3          	bge	a4,zero,20065cc0 <__NVIC_GetActive_IRQ+0x8>
20065ce0:	477d                	c.li	a4,31
20065ce2:	fea75be3          	bge	a4,a0,20065cd8 <__NVIC_GetActive_IRQ+0x20>
20065ce6:	eb01a783          	lw	a5,-336(gp) # 20065f88 <plic_irq_id>
20065cea:	02078793          	addi	a5,a5,32
20065cee:	40a78533          	sub	a0,a5,a0
20065cf2:	00153513          	sltiu	a0,a0,1
20065cf6:	8082                	c.jr	ra

20065cf8 <__sram_image2_end__>:
	...

20065d00 <Flash_Layout>:
20065d00:	0000 0000 0000 0800 3fff 0801 0002 0000     .........?......
20065d10:	4000 0801 3fff 081f 0001 0000 0000 0820     .@...?........ .
20065d20:	3fff 0821 0003 0000 4000 0821 3fff 083f     .?!......@!..??.
20065d30:	0004 0000 0000 0840 ffff 086f 0005 0000     ......@...o.....
20065d40:	0000 0870 2fff 0870 0006 0000 3000 0870     ..p../p......0p.
20065d50:	2fff 0872 0008 0000 ffff ffff ffff ffff     ./r.............
20065d60:	00ff 0000 ffff ffff ffff ffff               ............

20065d6c <PSRAM_CALIB_PATTERN>:
20065d6c:	3344 1122 a55a a55a 5aa5 5aa5 2211 4433     D3".Z.Z..Z.Z."3D
20065d7c:	6996 9669 9669 6996                         .ii.i..i

20065d84 <at_sys_items>:
20065d84:	26dc 2006 9c74 2005 0000 0000 0000 0000     .&. t.. ........
20065d94:	26e8 2006 9c94 2005 0000 0000 0000 0000     .&. ... ........
20065da4:	26f4 2006 a448 2005 0000 0000 0000 0000     .&. H.. ........
20065db4:	2700 2006 9c60 2005 0000 0000 0000 0000     .'. `.. ........
20065dc4:	2708 2006 a6c6 2005 0000 0000 0000 0000     .'. ... ........
20065dd4:	2710 2006 9cb4 2005 0000 0000 0000 0000     .'. ... ........
20065de4:	2718 2006 9cd4 2005 0000 0000 0000 0000     .'. ... ........
20065df4:	2720 2006 9d6e 2005 0000 0000 0000 0000      '. n.. ........
20065e04:	2728 2006 9dfe 2005 0000 0000 0000 0000     ('. ... ........
20065e14:	2730 2006 9fda 2005 0000 0000 0000 0000     0'. ... ........
20065e24:	2738 2006 a060 2005 0000 0000 0000 0000     8'. `.. ........

20065e34 <impure_data>:
20065e34:	0000 0000 7fc8 2006 8030 2006 8098 2006     ....... 0.. ... 
	...

20065f40 <__sdata_start__>:
20065f40:	0000                	c.unimp
20065f42:	42c8                	c.lw	a0,4(a3)

20065f44 <xISRStackTop>:
20065f44:	7700 2006                                   .w. 

20065f48 <CSWTCH.248>:
20065f48:	5258 5342 0044 0000                         XRBSD...

20065f50 <SystemCoreClock>:
20065f50:	d180 08f0                                   ....

20065f54 <PXID_Idx>:
20065f54:	0101 0101                                   ....

20065f58 <system_can_yield>:
20065f58:	0001 0000                                   ....

20065f5c <wakelock>:
20065f5c:	0001 0000                                   ....

20065f60 <rtk_log_default_level>:
20065f60:	0004 0000                                   ....

20065f64 <log_init_table>:
20065f64:	a6f0 2005                                   ... 

20065f68 <mbedtls_calloc_func>:
20065f68:	bbec 2005                                   ... 

20065f6c <mbedtls_free_func>:
20065f6c:	bbf0 2005                                   ... 

20065f70 <pullNextTime>:
20065f70:	6028 2006                                   (`. 

20065f74 <uxTopUsedPriority>:
20065f74:	000a 0000                                   ....

20065f78 <crash_task_info>:
20065f78:	ea12 2005                                   ... 

20065f7c <_impure_ptr>:
20065f7c:	5e34 2006                                   4^. 
