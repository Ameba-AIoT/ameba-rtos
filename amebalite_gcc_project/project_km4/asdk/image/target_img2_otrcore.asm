
/home/auto_jenkins_wlan5/IOT_ATS/repos/release-master/source/amebalite_gcc_project/project_kr4/vsdk/image_mp/target_img2.axf:     file format elf32-littleriscv


Disassembly of section .xip_image2.text:

20058060 <BOOT_InitGP>:
20058060:	0000f197          	auipc	gp,0xf
20058064:	92818193          	addi	gp,gp,-1752 # 20066988 <__global_pointer$>
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
2005833c:	556010ef          	jal	ra,20059892 <PSRAM_calibration>
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
20058804:	00074883          	lbu	a7,0(a4) # 8000 <__sram_bootloader_mp_size__>
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
20058a08:	02c0206f          	jal	zero,2005aa34 <BOOT_Image1>

20058a0c <app_start>:
20058a0c:	1141                	c.addi	sp,-16
20058a0e:	c606                	c.swsp	ra,12(sp)
20058a10:	e50ff0ef          	jal	ra,20058060 <BOOT_InitGP>
20058a14:	00018793          	addi	a5,gp,0 # 20066988 <__global_pointer$>
20058a18:	e9818613          	addi	a2,gp,-360 # 20066820 <PrevIrqStatus>
20058a1c:	40c78633          	sub	a2,a5,a2
20058a20:	4581                	c.li	a1,0
20058a22:	e9818513          	addi	a0,gp,-360 # 20066820 <PrevIrqStatus>
20058a26:	dffb1097          	auipc	ra,0xdffb1
20058a2a:	d64080e7          	jalr	ra,-668(ra) # 978a <_memset>
20058a2e:	200697b7          	lui	a5,0x20069
20058a32:	03818613          	addi	a2,gp,56 # 200669c0 <ipc_Semaphore>
20058a36:	afc78793          	addi	a5,a5,-1284 # 20068afc <__bss_end__>
20058a3a:	40c78633          	sub	a2,a5,a2
20058a3e:	4581                	c.li	a1,0
20058a40:	03818513          	addi	a0,gp,56 # 200669c0 <ipc_Semaphore>
20058a44:	dffb1097          	auipc	ra,0xdffb1
20058a48:	d46080e7          	jalr	ra,-698(ra) # 978a <_memset>
20058a4c:	1f6060ef          	jal	ra,2005ec42 <ChipInfo_GetChipSram>
20058a50:	c919                	c.beqz	a0,20058a66 <app_start+0x5a>
20058a52:	000405b7          	lui	a1,0x40
20058a56:	20080537          	lui	a0,0x20080
20058a5a:	00058593          	addi	a1,a1,0 # 40000 <__ap_sram_heap_size>
20058a5e:	00050513          	addi	a0,a0,0 # 20080000 <__ap_sram_heap_start>
20058a62:	10a020ef          	jal	ra,2005ab6c <os_heap_add>
20058a66:	48b020ef          	jal	ra,2005b6f0 <rtos_mem_init>
20058a6a:	2005c7b7          	lui	a5,0x2005c
20058a6e:	e8078793          	addi	a5,a5,-384 # 2005be80 <freertos_risc_v_trap_handler>
20058a72:	9bf1                	c.andi	a5,-4
20058a74:	30579073          	csrrw	zero,mtvec,a5
20058a78:	2831                	c.jal	20058a94 <SystemCoreClockUpdate>
20058a7a:	dffae097          	auipc	ra,0xdffae
20058a7e:	24c080e7          	jalr	ra,588(ra) # 6cc6 <SYSTIMER_Init>
20058a82:	2db000ef          	jal	ra,2005955c <pinmap_init>
20058a86:	122090ef          	jal	ra,20061ba8 <__libc_init_array>
20058a8a:	40b2                	c.lwsp	ra,12(sp)
20058a8c:	0141                	c.addi	sp,16
20058a8e:	0a90206f          	jal	zero,2005b336 <main>

20058a92 <_init>:
20058a92:	8082                	c.jr	ra

20058a94 <SystemCoreClockUpdate>:
20058a94:	1141                	c.addi	sp,-16
20058a96:	c606                	c.swsp	ra,12(sp)
20058a98:	dffa8097          	auipc	ra,0xdffa8
20058a9c:	07e080e7          	jalr	ra,126(ra) # b16 <CPU_ClkGet>
20058aa0:	40b2                	c.lwsp	ra,12(sp)
20058aa2:	e6a1a423          	sw	a0,-408(gp) # 200667f0 <SystemCoreClock>
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
20058ae8:	26b0d0ef          	jal	ra,20066552 <plic_get_active_irq_id>
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
20058b04:	c1818693          	addi	a3,gp,-1000 # 200665a0 <Flash_Layout>
20058b08:	4701                	c.li	a4,0
20058b0a:	c1818793          	addi	a5,gp,-1000 # 200665a0 <Flash_Layout>
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
20058b4a:	e8a1ac23          	sw	a0,-360(gp) # 20066820 <PrevIrqStatus>
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
20058b6a:	e981a503          	lw	a0,-360(gp) # 20066820 <PrevIrqStatus>
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
20058b8c:	01050513          	addi	a0,a0,16 # 20062010 <strtol+0xe>
20058b90:	51d060ef          	jal	ra,2005f8ac <io_assert_failed>
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
20058c00:	5920d0ef          	jal	ra,20066192 <FLASH_TxData>
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
20058cb0:	6dc0d0ef          	jal	ra,2006638c <FLASH_Erase>
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
20058d7c:	200654b7          	lui	s1,0x20065
20058d80:	20065db7          	lui	s11,0x20065
20058d84:	c94d8793          	addi	a5,s11,-876 # 20064c94 <ipc_tickless_table>
20058d88:	d3c48493          	addi	s1,s1,-708 # 20064d3c <lib_pmc_git_rev>
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
20058db0:	c94d8d93          	addi	s11,s11,-876
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
20058e56:	0b8b8693          	addi	a3,s7,184 # 200620b8 <strtol+0xb6>
20058e5a:	04900613          	addi	a2,zero,73
20058e5e:	8319                	c.srli	a4,0x6
20058e60:	8b0d                	c.andi	a4,3
20058e62:	100c0593          	addi	a1,s8,256 # 20062100 <strtol+0xfe>
20058e66:	4511                	c.li	a0,4
20058e68:	326020ef          	jal	ra,2005b18e <rtk_log_write>
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
20058ecc:	03818613          	addi	a2,gp,56 # 200669c0 <ipc_Semaphore>
20058ed0:	9432                	c.add	s0,a2
20058ed2:	4008                	c.lw	a0,0(s0)
20058ed4:	c511                	c.beqz	a0,20058ee0 <IPC_TXHandler+0x34>
20058ed6:	4422                	c.lwsp	s0,8(sp)
20058ed8:	40b2                	c.lwsp	ra,12(sp)
20058eda:	0141                	c.addi	sp,16
20058edc:	2930206f          	jal	zero,2005b96e <rtos_sema_give>
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
20058f0c:	10450513          	addi	a0,a0,260 # 20062104 <strtol+0x102>
20058f10:	c606                	c.swsp	ra,12(sp)
20058f12:	494060ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
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
20058f38:	0f818713          	addi	a4,gp,248 # 20066a80 <IPC_IrqHandler>
20058f3c:	974e                	c.add	a4,s3
20058f3e:	431c                	c.lw	a5,0(a4)
20058f40:	c79d                	c.beqz	a5,20058f6e <IPC_wait_idle+0x50>
20058f42:	03818513          	addi	a0,gp,56 # 200669c0 <ipc_Semaphore>
20058f46:	954e                	c.add	a0,s3
20058f48:	4118                	c.lw	a4,0(a0)
20058f4a:	03818413          	addi	s0,gp,56 # 200669c0 <ipc_Semaphore>
20058f4e:	e319                	c.bnez	a4,20058f54 <IPC_wait_idle+0x36>
20058f50:	193020ef          	jal	ra,2005b8e2 <rtos_sema_create_binary>
20058f54:	0ff97a13          	andi	s4,s2,255
20058f58:	85d2                	c.mv	a1,s4
20058f5a:	8526                	c.mv	a0,s1
20058f5c:	4605                	c.li	a2,1
20058f5e:	2401                	c.jal	2005915e <IPC_INTConfig>
20058f60:	013407b3          	add	a5,s0,s3
20058f64:	4388                	c.lw	a0,0(a5)
20058f66:	55fd                	c.li	a1,-1
20058f68:	1bb020ef          	jal	ra,2005b922 <rtos_sema_take>
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
20058f86:	11c50513          	addi	a0,a0,284 # 2006211c <strtol+0x11a>
20058f8a:	41c060ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
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
20058fd6:	13c50513          	addi	a0,a0,316 # 2006213c <strtol+0x13a>
20058fda:	0d3060ef          	jal	ra,2005f8ac <io_assert_failed>
20058fde:	479d                	c.li	a5,7
20058fe0:	0157fa63          	bgeu	a5,s5,20058ff4 <ipc_send_message+0x4c>
20058fe4:	20062537          	lui	a0,0x20062
20058fe8:	0ad00593          	addi	a1,zero,173
20058fec:	13c50513          	addi	a0,a0,316 # 2006213c <strtol+0x13a>
20058ff0:	0bd060ef          	jal	ra,2005f8ac <io_assert_failed>
20058ff4:	02100793          	addi	a5,zero,33
20058ff8:	0d37eb63          	bltu	a5,s3,200590ce <ipc_send_message+0x126>
20058ffc:	200627b7          	lui	a5,0x20062
20059000:	21078793          	addi	a5,a5,528 # 20062210 <CSWTCH.29>
20059004:	97ce                	c.add	a5,s3
20059006:	20062737          	lui	a4,0x20062
2005900a:	0007c583          	lbu	a1,0(a5)
2005900e:	23470713          	addi	a4,a4,564 # 20062234 <CSWTCH.30>
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
2005903a:	1e450513          	addi	a0,a0,484 # 200621e4 <strtol+0x1e2>
2005903e:	368060ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
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
20059198:	07818913          	addi	s2,gp,120 # 20066a00 <IPC_IrqData>
2005919c:	0f818493          	addi	s1,gp,248 # 20066a80 <IPC_IrqHandler>
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
200591ec:	0f818713          	addi	a4,gp,248 # 20066a80 <IPC_IrqHandler>
200591f0:	883e                	c.mv	a6,a5
200591f2:	973e                	c.add	a4,a5
200591f4:	07818793          	addi	a5,gp,120 # 20066a00 <IPC_IrqData>
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
2005922a:	e6c18793          	addi	a5,gp,-404 # 200667f4 <PXID_Idx>
2005922e:	97aa                	c.add	a5,a0
20059230:	0007c783          	lbu	a5,0(a5)
20059234:	892a                	c.mv	s2,a0
20059236:	842e                	c.mv	s0,a1
20059238:	e6c18493          	addi	s1,gp,-404 # 200667f4 <PXID_Idx>
2005923c:	eb89                	c.bnez	a5,2005924e <IPC_SEMTake+0x3c>
2005923e:	20062537          	lui	a0,0x20062
20059242:	0bf00593          	addi	a1,zero,191
20059246:	2bc50513          	addi	a0,a0,700 # 200622bc <CSWTCH.30+0x88>
2005924a:	662060ef          	jal	ra,2005f8ac <io_assert_failed>
2005924e:	00291793          	slli	a5,s2,0x2
20059252:	410209b7          	lui	s3,0x41020
20059256:	99be                	c.add	s3,a5
20059258:	0149a783          	lw	a5,20(s3) # 41020014 <__km4_bd_boot_download_addr__+0x1100e014>
2005925c:	83a1                	c.srli	a5,0x8
2005925e:	8b8d                	c.andi	a5,3
20059260:	cb89                	c.beqz	a5,20059272 <IPC_SEMTake+0x60>
20059262:	c42d                	c.beqz	s0,200592cc <IPC_SEMTake+0xba>
20059264:	e9c1a783          	lw	a5,-356(gp) # 20066824 <ipc_delay>
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
200592bc:	36450513          	addi	a0,a0,868 # 20062364 <CSWTCH.30+0x130>
200592c0:	0e6060ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
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
2005932a:	e6c18693          	addi	a3,gp,-404 # 200667f4 <PXID_Idx>
2005932e:	c606                	c.swsp	ra,12(sp)
20059330:	96b2                	c.add	a3,a2
20059332:	0006c683          	lbu	a3,0(a3) # 41020000 <__km4_bd_boot_download_addr__+0x1100e000>
20059336:	20062537          	lui	a0,0x20062
2005933a:	0ff7f793          	andi	a5,a5,255
2005933e:	177d                	c.addi	a4,-1
20059340:	3b050513          	addi	a0,a0,944 # 200623b0 <CSWTCH.30+0x17c>
20059344:	062060ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
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
20059360:	e8a1ae23          	sw	a0,-356(gp) # 20066824 <ipc_delay>
20059364:	8082                	c.jr	ra

20059366 <IPC_GetDevById>:
20059366:	4789                	c.li	a5,2
20059368:	02a7f163          	bgeu	a5,a0,2005938a <IPC_GetDevById+0x24>
2005936c:	20062537          	lui	a0,0x20062
20059370:	1141                	c.addi	sp,-16
20059372:	2bc50513          	addi	a0,a0,700 # 200622bc <CSWTCH.30+0x88>
20059376:	11b00593          	addi	a1,zero,283
2005937a:	c606                	c.swsp	ra,12(sp)
2005937c:	530060ef          	jal	ra,2005f8ac <io_assert_failed>
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
200593e6:	17818793          	addi	a5,gp,376 # 20066b00 <IMG_ADDR>
200593ea:	97ce                	c.add	a5,s3
200593ec:	439c                	c.lw	a5,0(a5)
200593ee:	892a                	c.mv	s2,a0
200593f0:	17818493          	addi	s1,gp,376 # 20066b00 <IMG_ADDR>
200593f4:	e7a9                	c.bnez	a5,2005943e <ota_get_cur_index+0x7c>
200593f6:	4601                	c.li	a2,0
200593f8:	17818593          	addi	a1,gp,376 # 20066b00 <IMG_ADDR>
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
200594fe:	41850513          	addi	a0,a0,1048 # 20062418 <CSWTCH.30+0x1e4>
20059502:	6a5050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
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
20059526:	3fca0513          	addi	a0,s4,1020 # 200623fc <CSWTCH.30+0x1c8>
2005952a:	67d050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
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
20059584:	42c48493          	addi	s1,s1,1068 # 2006242c <pmap_func>
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

2005971a <pmu_set_sysactive_time>:
2005971a:	1141                	c.addi	sp,-16
2005971c:	c226                	c.swsp	s1,4(sp)
2005971e:	ea81a483          	lw	s1,-344(gp) # 20066830 <sysactive_timeout_flag>
20059722:	c422                	c.swsp	s0,8(sp)
20059724:	c606                	c.swsp	ra,12(sp)
20059726:	eac18693          	addi	a3,gp,-340 # 20066834 <sysactive_timeout_temp>
2005972a:	4298                	c.lw	a4,0(a3)
2005972c:	842a                	c.mv	s0,a0
2005972e:	c899                	c.beqz	s1,20059744 <pmu_set_sysactive_time+0x2a>
20059730:	4485                	c.li	s1,1
20059732:	00a77363          	bgeu	a4,a0,20059738 <pmu_set_sysactive_time+0x1e>
20059736:	c288                	c.sw	a0,0(a3)
20059738:	40b2                	c.lwsp	ra,12(sp)
2005973a:	4422                	c.lwsp	s0,8(sp)
2005973c:	8526                	c.mv	a0,s1
2005973e:	4492                	c.lwsp	s1,4(sp)
20059740:	0141                	c.addi	sp,16
20059742:	8082                	c.jr	ra
20059744:	eac18793          	addi	a5,gp,-340 # 20066834 <sysactive_timeout_temp>
20059748:	00e57363          	bgeu	a0,a4,2005974e <pmu_set_sysactive_time+0x34>
2005974c:	843a                	c.mv	s0,a4
2005974e:	0007a023          	sw	zero,0(a5)
20059752:	4e0020ef          	jal	ra,2005bc32 <rtos_time_get_current_system_time_ms>
20059756:	ea41a783          	lw	a5,-348(gp) # 2006682c <sleepwakelock_timeout>
2005975a:	800006b7          	lui	a3,0x80000
2005975e:	942a                	c.add	s0,a0
20059760:	ea418713          	addi	a4,gp,-348 # 2006682c <sleepwakelock_timeout>
20059764:	ffe6c693          	xori	a3,a3,-2
20059768:	0087f863          	bgeu	a5,s0,20059778 <pmu_set_sysactive_time+0x5e>
2005976c:	40f407b3          	sub	a5,s0,a5
20059770:	fcf6e4e3          	bltu	a3,a5,20059738 <pmu_set_sysactive_time+0x1e>
20059774:	c300                	c.sw	s0,0(a4)
20059776:	b7c9                	c.j	20059738 <pmu_set_sysactive_time+0x1e>
20059778:	40878633          	sub	a2,a5,s0
2005977c:	fec6ece3          	bltu	a3,a2,20059774 <pmu_set_sysactive_time+0x5a>
20059780:	fa879ce3          	bne	a5,s0,20059738 <pmu_set_sysactive_time+0x1e>
20059784:	bfc5                	c.j	20059774 <pmu_set_sysactive_time+0x5a>

20059786 <pmu_register_sleep_callback>:
20059786:	1101                	c.addi	sp,-32
20059788:	cc22                	c.swsp	s0,24(sp)
2005978a:	ca26                	c.swsp	s1,20(sp)
2005978c:	c84a                	c.swsp	s2,16(sp)
2005978e:	c64e                	c.swsp	s3,12(sp)
20059790:	c452                	c.swsp	s4,8(sp)
20059792:	ce06                	c.swsp	ra,28(sp)
20059794:	47f9                	c.li	a5,30
20059796:	842a                	c.mv	s0,a0
20059798:	892e                	c.mv	s2,a1
2005979a:	8a32                	c.mv	s4,a2
2005979c:	84b6                	c.mv	s1,a3
2005979e:	89ba                	c.mv	s3,a4
200597a0:	00a7fa63          	bgeu	a5,a0,200597b4 <pmu_register_sleep_callback+0x2e>
200597a4:	20062537          	lui	a0,0x20062
200597a8:	07300593          	addi	a1,zero,115
200597ac:	52c50513          	addi	a0,a0,1324 # 2006252c <pmap_func+0x100>
200597b0:	0fc060ef          	jal	ra,2005f8ac <io_assert_failed>
200597b4:	00091b63          	bne	s2,zero,200597ca <pmu_register_sleep_callback+0x44>
200597b8:	e889                	c.bnez	s1,200597ca <pmu_register_sleep_callback+0x44>
200597ba:	20062537          	lui	a0,0x20062
200597be:	07400593          	addi	a1,zero,116
200597c2:	52c50513          	addi	a0,a0,1324 # 2006252c <pmap_func+0x100>
200597c6:	0e6060ef          	jal	ra,2005f8ac <io_assert_failed>
200597ca:	47d1                	c.li	a5,20
200597cc:	02f40733          	mul	a4,s0,a5
200597d0:	19018793          	addi	a5,gp,400 # 20066b18 <gPsmDdHookInfo>
200597d4:	97ba                	c.add	a5,a4
200597d6:	c380                	c.sw	s0,0(a5)
200597d8:	0127a223          	sw	s2,4(a5)
200597dc:	0147a423          	sw	s4,8(a5)
200597e0:	c7c4                	c.sw	s1,12(a5)
200597e2:	0137a823          	sw	s3,16(a5)
200597e6:	40f2                	c.lwsp	ra,28(sp)
200597e8:	4462                	c.lwsp	s0,24(sp)
200597ea:	44d2                	c.lwsp	s1,20(sp)
200597ec:	4942                	c.lwsp	s2,16(sp)
200597ee:	49b2                	c.lwsp	s3,12(sp)
200597f0:	4a22                	c.lwsp	s4,8(sp)
200597f2:	6105                	c.addi16sp	sp,32
200597f4:	8082                	c.jr	ra

200597f6 <pmu_yield_os_check>:
200597f6:	e701a503          	lw	a0,-400(gp) # 200667f8 <system_can_yield>
200597fa:	8082                	c.jr	ra

200597fc <pmu_ready_to_sleep>:
200597fc:	1141                	c.addi	sp,-16
200597fe:	c606                	c.swsp	ra,12(sp)
20059800:	432020ef          	jal	ra,2005bc32 <rtos_time_get_current_system_time_ms>
20059804:	ea41a703          	lw	a4,-348(gp) # 2006682c <sleepwakelock_timeout>
20059808:	800006b7          	lui	a3,0x80000
2005980c:	87aa                	c.mv	a5,a0
2005980e:	ffe6c693          	xori	a3,a3,-2
20059812:	00a77e63          	bgeu	a4,a0,2005982e <pmu_ready_to_sleep+0x32>
20059816:	40e507b3          	sub	a5,a0,a4
2005981a:	4501                	c.li	a0,0
2005981c:	02f6e063          	bltu	a3,a5,2005983c <pmu_ready_to_sleep+0x40>
20059820:	3679                	c.jal	200593ae <dsp_status_on>
20059822:	e741a783          	lw	a5,-396(gp) # 200667fc <wakelock>
20059826:	8d5d                	c.or	a0,a5
20059828:	00153513          	sltiu	a0,a0,1
2005982c:	a801                	c.j	2005983c <pmu_ready_to_sleep+0x40>
2005982e:	40a70633          	sub	a2,a4,a0
20059832:	fec6e7e3          	bltu	a3,a2,20059820 <pmu_ready_to_sleep+0x24>
20059836:	4501                	c.li	a0,0
20059838:	fee7f4e3          	bgeu	a5,a4,20059820 <pmu_ready_to_sleep+0x24>
2005983c:	40b2                	c.lwsp	ra,12(sp)
2005983e:	0141                	c.addi	sp,16
20059840:	8082                	c.jr	ra

20059842 <pmu_acquire_wakelock>:
20059842:	1141                	c.addi	sp,-16
20059844:	c422                	c.swsp	s0,8(sp)
20059846:	c606                	c.swsp	ra,12(sp)
20059848:	842a                	c.mv	s0,a0
2005984a:	aacff0ef          	jal	ra,20058af6 <irq_disable_save>
2005984e:	e7418713          	addi	a4,gp,-396 # 200667fc <wakelock>
20059852:	4314                	c.lw	a3,0(a4)
20059854:	4785                	c.li	a5,1
20059856:	008797b3          	sll	a5,a5,s0
2005985a:	8fd5                	c.or	a5,a3
2005985c:	c31c                	c.sw	a5,0(a4)
2005985e:	4422                	c.lwsp	s0,8(sp)
20059860:	40b2                	c.lwsp	ra,12(sp)
20059862:	0141                	c.addi	sp,16
20059864:	a9aff06f          	jal	zero,20058afe <irq_enable_restore>

20059868 <pmu_release_wakelock>:
20059868:	1141                	c.addi	sp,-16
2005986a:	c422                	c.swsp	s0,8(sp)
2005986c:	c606                	c.swsp	ra,12(sp)
2005986e:	842a                	c.mv	s0,a0
20059870:	a86ff0ef          	jal	ra,20058af6 <irq_disable_save>
20059874:	e7418713          	addi	a4,gp,-396 # 200667fc <wakelock>
20059878:	4314                	c.lw	a3,0(a4)
2005987a:	4785                	c.li	a5,1
2005987c:	008797b3          	sll	a5,a5,s0
20059880:	fff7c793          	xori	a5,a5,-1
20059884:	8ff5                	c.and	a5,a3
20059886:	c31c                	c.sw	a5,0(a4)
20059888:	4422                	c.lwsp	s0,8(sp)
2005988a:	40b2                	c.lwsp	ra,12(sp)
2005988c:	0141                	c.addi	sp,16
2005988e:	a70ff06f          	jal	zero,20058afe <irq_enable_restore>

20059892 <PSRAM_calibration>:
20059892:	7119                	c.addi16sp	sp,-128
20059894:	de86                	c.swsp	ra,124(sp)
20059896:	dca6                	c.swsp	s1,120(sp)
20059898:	daca                	c.swsp	s2,116(sp)
2005989a:	d8ce                	c.swsp	s3,112(sp)
2005989c:	d6d2                	c.swsp	s4,108(sp)
2005989e:	d4d6                	c.swsp	s5,104(sp)
200598a0:	d2da                	c.swsp	s6,100(sp)
200598a2:	d0de                	c.swsp	s7,96(sp)
200598a4:	cee2                	c.swsp	s8,92(sp)
200598a6:	cce6                	c.swsp	s9,88(sp)
200598a8:	caea                	c.swsp	s10,84(sp)
200598aa:	c8ee                	c.swsp	s11,80(sp)
200598ac:	cc2a                	c.swsp	a0,24(sp)
200598ae:	410087b7          	lui	a5,0x41008
200598b2:	43d8                	c.lw	a4,4(a5)
200598b4:	100007b7          	lui	a5,0x10000
200598b8:	1781                	c.addi	a5,-32
200598ba:	8ff9                	c.and	a5,a4
200598bc:	d43e                	c.swsp	a5,40(sp)
200598be:	dc02                	c.swsp	zero,56(sp)
200598c0:	de02                	c.swsp	zero,60(sp)
200598c2:	c082                	c.swsp	zero,64(sp)
200598c4:	c282                	c.swsp	zero,68(sp)
200598c6:	c482                	c.swsp	zero,72(sp)
200598c8:	c682                	c.swsp	zero,76(sp)
200598ca:	7c1027f3          	csrrs	a5,0x7c1,zero
200598ce:	1007f793          	andi	a5,a5,256
200598d2:	c385                	c.beqz	a5,200598f2 <PSRAM_calibration+0x60>
200598d4:	7c0467f3          	csrrsi	a5,0x7c0,8
200598d8:	0ff0000f          	fence	iorw,iorw
200598dc:	0001                	c.addi	zero,0
200598de:	0001                	c.addi	zero,0
200598e0:	0001                	c.addi	zero,0
200598e2:	0001                	c.addi	zero,0
200598e4:	0001                	c.addi	zero,0
200598e6:	0001                	c.addi	zero,0
200598e8:	0001                	c.addi	zero,0
200598ea:	0001                	c.addi	zero,0
200598ec:	0001                	c.addi	zero,0
200598ee:	0001                	c.addi	zero,0
200598f0:	0001                	c.addi	zero,0
200598f2:	10000793          	addi	a5,zero,256
200598f6:	7c17b7f3          	csrrc	a5,0x7c1,a5
200598fa:	0ff0000f          	fence	iorw,iorw
200598fe:	0001                	c.addi	zero,0
20059900:	0001                	c.addi	zero,0
20059902:	0001                	c.addi	zero,0
20059904:	0001                	c.addi	zero,0
20059906:	0001                	c.addi	zero,0
20059908:	0001                	c.addi	zero,0
2005990a:	0001                	c.addi	zero,0
2005990c:	0001                	c.addi	zero,0
2005990e:	0001                	c.addi	zero,0
20059910:	0001                	c.addi	zero,0
20059912:	0001                	c.addi	zero,0
20059914:	7c0467f3          	csrrsi	a5,0x7c0,8
20059918:	0ff0000f          	fence	iorw,iorw
2005991c:	0001                	c.addi	zero,0
2005991e:	0001                	c.addi	zero,0
20059920:	0001                	c.addi	zero,0
20059922:	0001                	c.addi	zero,0
20059924:	0001                	c.addi	zero,0
20059926:	0001                	c.addi	zero,0
20059928:	0001                	c.addi	zero,0
2005992a:	0001                	c.addi	zero,0
2005992c:	0001                	c.addi	zero,0
2005992e:	0001                	c.addi	zero,0
20059930:	0001                	c.addi	zero,0
20059932:	41008737          	lui	a4,0x41008
20059936:	431c                	c.lw	a5,0(a4)
20059938:	20062c37          	lui	s8,0x20062
2005993c:	60000d37          	lui	s10,0x60000
20059940:	9bf9                	c.andi	a5,-2
20059942:	c31c                	c.sw	a5,0(a4)
20059944:	4791                	c.li	a5,4
20059946:	ca3e                	c.swsp	a5,20(sp)
20059948:	000d0b13          	addi	s6,s10,0 # 60000000 <__km4_bd_psram_start__>
2005994c:	5c4c0793          	addi	a5,s8,1476 # 200625c4 <pmap_func+0x198>
20059950:	00050cb7          	lui	s9,0x50
20059954:	d03e                	c.swsp	a5,32(sp)
20059956:	019b07b3          	add	a5,s6,s9
2005995a:	597d                	c.li	s2,-1
2005995c:	4981                	c.li	s3,0
2005995e:	5afd                	c.li	s5,-1
20059960:	54fd                	c.li	s1,-1
20059962:	4a05                	c.li	s4,1
20059964:	c8418b93          	addi	s7,gp,-892 # 2006660c <PSRAM_CALIB_PATTERN>
20059968:	ce3e                	c.swsp	a5,28(sp)
2005996a:	47e2                	c.lwsp	a5,24(sp)
2005996c:	cf91                	c.beqz	a5,20059988 <PSRAM_calibration+0xf6>
2005996e:	5782                	c.lwsp	a5,32(sp)
20059970:	200625b7          	lui	a1,0x20062
20059974:	8752                	c.mv	a4,s4
20059976:	20078693          	addi	a3,a5,512 # 10000200 <__km4_boot_text_start__+0x8001e0>
2005997a:	04900613          	addi	a2,zero,73
2005997e:	5d858593          	addi	a1,a1,1496 # 200625d8 <pmap_func+0x1ac>
20059982:	4511                	c.li	a0,4
20059984:	00b010ef          	jal	ra,2005b18e <rtk_log_write>
20059988:	57a2                	c.lwsp	a5,40(sp)
2005998a:	01ca1d93          	slli	s11,s4,0x1c
2005998e:	5ffd                	c.li	t6,-1
20059990:	00fde7b3          	or	a5,s11,a5
20059994:	d23e                	c.swsp	a5,36(sp)
20059996:	4c01                	c.li	s8,0
20059998:	5cfd                	c.li	s9,-1
2005999a:	4d81                	c.li	s11,0
2005999c:	5792                	c.lwsp	a5,36(sp)
2005999e:	d67e                	c.swsp	t6,44(sp)
200599a0:	410086b7          	lui	a3,0x41008
200599a4:	01b7e7b3          	or	a5,a5,s11
200599a8:	c2dc                	c.sw	a5,4(a3)
200599aa:	000ba783          	lw	a5,0(s7)
200599ae:	000d4603          	lbu	a2,0(s10)
200599b2:	4772                	c.lwsp	a4,28(sp)
200599b4:	0ff7f693          	andi	a3,a5,255
200599b8:	00dd0023          	sb	a3,0(s10)
200599bc:	0087d693          	srli	a3,a5,0x8
200599c0:	0ff6f693          	andi	a3,a3,255
200599c4:	001d4603          	lbu	a2,1(s10)
200599c8:	00dd00a3          	sb	a3,1(s10)
200599cc:	0107d693          	srli	a3,a5,0x10
200599d0:	0ff6f693          	andi	a3,a3,255
200599d4:	002d4603          	lbu	a2,2(s10)
200599d8:	83e1                	c.srli	a5,0x18
200599da:	00dd0123          	sb	a3,2(s10)
200599de:	003d4683          	lbu	a3,3(s10)
200599e2:	00fd01a3          	sb	a5,3(s10)
200599e6:	004ba783          	lw	a5,4(s7)
200599ea:	00100837          	lui	a6,0x100
200599ee:	985a                	c.add	a6,s6
200599f0:	c31c                	c.sw	a5,0(a4)
200599f2:	008ba783          	lw	a5,8(s7)
200599f6:	00150537          	lui	a0,0x150
200599fa:	955a                	c.add	a0,s6
200599fc:	00f82023          	sw	a5,0(a6) # 100000 <__ap_sram_heap_size+0xc0000>
20059a00:	00cba783          	lw	a5,12(s7)
20059a04:	002005b7          	lui	a1,0x200
20059a08:	95da                	c.add	a1,s6
20059a0a:	c11c                	c.sw	a5,0(a0)
20059a0c:	010ba783          	lw	a5,16(s7)
20059a10:	00250637          	lui	a2,0x250
20059a14:	965a                	c.add	a2,s6
20059a16:	c19c                	c.sw	a5,0(a1)
20059a18:	014ba783          	lw	a5,20(s7)
20059a1c:	c21c                	c.sw	a5,0(a2)
20059a1e:	000d4683          	lbu	a3,0(s10)
20059a22:	001d4783          	lbu	a5,1(s10)
20059a26:	0ff6f693          	andi	a3,a3,255
20059a2a:	0ff7f793          	andi	a5,a5,255
20059a2e:	07a2                	c.slli	a5,0x8
20059a30:	8edd                	c.or	a3,a5
20059a32:	002d4783          	lbu	a5,2(s10)
20059a36:	0ff7f793          	andi	a5,a5,255
20059a3a:	07c2                	c.slli	a5,0x10
20059a3c:	8fd5                	c.or	a5,a3
20059a3e:	003d4683          	lbu	a3,3(s10)
20059a42:	06e2                	c.slli	a3,0x18
20059a44:	8fd5                	c.or	a5,a3
20059a46:	dc3e                	c.swsp	a5,56(sp)
20059a48:	431c                	c.lw	a5,0(a4)
20059a4a:	de3e                	c.swsp	a5,60(sp)
20059a4c:	00082783          	lw	a5,0(a6)
20059a50:	c0be                	c.swsp	a5,64(sp)
20059a52:	411c                	c.lw	a5,0(a0)
20059a54:	1828                	c.addi4spn	a0,sp,56
20059a56:	c2be                	c.swsp	a5,68(sp)
20059a58:	419c                	c.lw	a5,0(a1)
20059a5a:	85de                	c.mv	a1,s7
20059a5c:	c4be                	c.swsp	a5,72(sp)
20059a5e:	421c                	c.lw	a5,0(a2)
20059a60:	4661                	c.li	a2,24
20059a62:	c6be                	c.swsp	a5,76(sp)
20059a64:	dffb0097          	auipc	ra,0xdffb0
20059a68:	b5c080e7          	jalr	ra,-1188(ra) # 95c0 <_memcmp>
20059a6c:	5fb2                	c.lwsp	t6,44(sp)
20059a6e:	47e2                	c.lwsp	a5,24(sp)
20059a70:	14051563          	bne	a0,zero,20059bba <PSRAM_calibration+0x328>
20059a74:	c79d                	c.beqz	a5,20059aa2 <PSRAM_calibration+0x210>
20059a76:	47b6                	c.lwsp	a5,76(sp)
20059a78:	5682                	c.lwsp	a3,32(sp)
20059a7a:	4886                	c.lwsp	a7,64(sp)
20059a7c:	c43e                	c.swsp	a5,8(sp)
20059a7e:	47a6                	c.lwsp	a5,72(sp)
20059a80:	5872                	c.lwsp	a6,60(sp)
20059a82:	200625b7          	lui	a1,0x20062
20059a86:	c23e                	c.swsp	a5,4(sp)
20059a88:	4796                	c.lwsp	a5,68(sp)
20059a8a:	876e                	c.mv	a4,s11
20059a8c:	21468693          	addi	a3,a3,532 # 41008214 <__km4_bd_boot_download_addr__+0x10ff6214>
20059a90:	c03e                	c.swsp	a5,0(sp)
20059a92:	57e2                	c.lwsp	a5,56(sp)
20059a94:	04900613          	addi	a2,zero,73
20059a98:	5d858593          	addi	a1,a1,1496 # 200625d8 <pmap_func+0x1ac>
20059a9c:	4511                	c.li	a0,4
20059a9e:	6f0010ef          	jal	ra,2005b18e <rtk_log_write>
20059aa2:	57fd                	c.li	a5,-1
20059aa4:	00fc9363          	bne	s9,a5,20059aaa <PSRAM_calibration+0x218>
20059aa8:	8cee                	c.mv	s9,s11
20059aaa:	47fd                	c.li	a5,31
20059aac:	018c8fb3          	add	t6,s9,s8
20059ab0:	0c05                	c.addi	s8,1
20059ab2:	00fd9863          	bne	s11,a5,20059ac2 <PSRAM_calibration+0x230>
20059ab6:	0189d663          	bge	s3,s8,20059ac2 <PSRAM_calibration+0x230>
20059aba:	8952                	c.mv	s2,s4
20059abc:	89e2                	c.mv	s3,s8
20059abe:	8afe                	c.mv	s5,t6
20059ac0:	84e6                	c.mv	s1,s9
20059ac2:	0d85                	c.addi	s11,1
20059ac4:	02000793          	addi	a5,zero,32
20059ac8:	ecfd9ae3          	bne	s11,a5,2005999c <PSRAM_calibration+0x10a>
20059acc:	47d2                	c.lwsp	a5,20(sp)
20059ace:	0a06                	c.slli	s4,0x1
20059ad0:	17fd                	c.addi	a5,-1
20059ad2:	ca3e                	c.swsp	a5,20(sp)
20059ad4:	e8079be3          	bne	a5,zero,2005996a <PSRAM_calibration+0xd8>
20059ad8:	200636b7          	lui	a3,0x20063
20059adc:	200625b7          	lui	a1,0x20062
20059ae0:	88ca                	c.mv	a7,s2
20059ae2:	884e                	c.mv	a6,s3
20059ae4:	87d6                	c.mv	a5,s5
20059ae6:	8726                	c.mv	a4,s1
20059ae8:	81c68693          	addi	a3,a3,-2020 # 2006281c <pmap_func+0x3f0>
20059aec:	04900613          	addi	a2,zero,73
20059af0:	5d858593          	addi	a1,a1,1496 # 200625d8 <pmap_func+0x1ac>
20059af4:	4511                	c.li	a0,4
20059af6:	698010ef          	jal	ra,2005b18e <rtk_log_write>
20059afa:	7c1027f3          	csrrs	a5,0x7c1,zero
20059afe:	1007f793          	andi	a5,a5,256
20059b02:	e7b1                	c.bnez	a5,20059b4e <PSRAM_calibration+0x2bc>
20059b04:	7c1027f3          	csrrs	a5,0x7c1,zero
20059b08:	1007f793          	andi	a5,a5,256
20059b0c:	e3a9                	c.bnez	a5,20059b4e <PSRAM_calibration+0x2bc>
20059b0e:	7c0167f3          	csrrsi	a5,0x7c0,2
20059b12:	0ff0000f          	fence	iorw,iorw
20059b16:	0001                	c.addi	zero,0
20059b18:	0001                	c.addi	zero,0
20059b1a:	0001                	c.addi	zero,0
20059b1c:	0001                	c.addi	zero,0
20059b1e:	0001                	c.addi	zero,0
20059b20:	0001                	c.addi	zero,0
20059b22:	0001                	c.addi	zero,0
20059b24:	0001                	c.addi	zero,0
20059b26:	0001                	c.addi	zero,0
20059b28:	0001                	c.addi	zero,0
20059b2a:	0001                	c.addi	zero,0
20059b2c:	10000793          	addi	a5,zero,256
20059b30:	7c17a7f3          	csrrs	a5,0x7c1,a5
20059b34:	0ff0000f          	fence	iorw,iorw
20059b38:	0001                	c.addi	zero,0
20059b3a:	0001                	c.addi	zero,0
20059b3c:	0001                	c.addi	zero,0
20059b3e:	0001                	c.addi	zero,0
20059b40:	0001                	c.addi	zero,0
20059b42:	0001                	c.addi	zero,0
20059b44:	0001                	c.addi	zero,0
20059b46:	0001                	c.addi	zero,0
20059b48:	0001                	c.addi	zero,0
20059b4a:	0001                	c.addi	zero,0
20059b4c:	0001                	c.addi	zero,0
20059b4e:	47a1                	c.li	a5,8
20059b50:	4501                	c.li	a0,0
20059b52:	0537d663          	bge	a5,s3,20059b9e <PSRAM_calibration+0x30c>
20059b56:	409a8733          	sub	a4,s5,s1
20059b5a:	4609                	c.li	a2,2
20059b5c:	02c74733          	div	a4,a4,a2
20059b60:	94d6                	c.add	s1,s5
20059b62:	6685                	c.lui	a3,0x1
20059b64:	f0068693          	addi	a3,a3,-256 # f00 <CPU_ClkGet+0x3ea>
20059b68:	0972                	c.slli	s2,0x1c
20059b6a:	4505                	c.li	a0,1
20059b6c:	02c4c4b3          	div	s1,s1,a2
20059b70:	1779                	c.addi	a4,-2
20059b72:	00871793          	slli	a5,a4,0x8
20059b76:	8ff5                	c.and	a5,a3
20059b78:	0127e7b3          	or	a5,a5,s2
20059b7c:	0742                	c.slli	a4,0x10
20059b7e:	000f06b7          	lui	a3,0xf0
20059b82:	8f75                	c.and	a4,a3
20059b84:	88fd                	c.andi	s1,31
20059b86:	8fc5                	c.or	a5,s1
20059b88:	8fd9                	c.or	a5,a4
20059b8a:	02000737          	lui	a4,0x2000
20059b8e:	8fd9                	c.or	a5,a4
20059b90:	41008737          	lui	a4,0x41008
20059b94:	c35c                	c.sw	a5,4(a4)
20059b96:	431c                	c.lw	a5,0(a4)
20059b98:	0017e793          	ori	a5,a5,1
20059b9c:	c31c                	c.sw	a5,0(a4)
20059b9e:	50f6                	c.lwsp	ra,124(sp)
20059ba0:	54e6                	c.lwsp	s1,120(sp)
20059ba2:	5956                	c.lwsp	s2,116(sp)
20059ba4:	59c6                	c.lwsp	s3,112(sp)
20059ba6:	5a36                	c.lwsp	s4,108(sp)
20059ba8:	5aa6                	c.lwsp	s5,104(sp)
20059baa:	5b16                	c.lwsp	s6,100(sp)
20059bac:	5b86                	c.lwsp	s7,96(sp)
20059bae:	4c76                	c.lwsp	s8,92(sp)
20059bb0:	4ce6                	c.lwsp	s9,88(sp)
20059bb2:	4d56                	c.lwsp	s10,84(sp)
20059bb4:	4dc6                	c.lwsp	s11,80(sp)
20059bb6:	6109                	c.addi16sp	sp,128
20059bb8:	8082                	c.jr	ra
20059bba:	cb95                	c.beqz	a5,20059bee <PSRAM_calibration+0x35c>
20059bbc:	47b6                	c.lwsp	a5,76(sp)
20059bbe:	4886                	c.lwsp	a7,64(sp)
20059bc0:	5872                	c.lwsp	a6,60(sp)
20059bc2:	c43e                	c.swsp	a5,8(sp)
20059bc4:	47a6                	c.lwsp	a5,72(sp)
20059bc6:	200626b7          	lui	a3,0x20062
20059bca:	200625b7          	lui	a1,0x20062
20059bce:	c23e                	c.swsp	a5,4(sp)
20059bd0:	4796                	c.lwsp	a5,68(sp)
20059bd2:	876e                	c.mv	a4,s11
20059bd4:	7f868693          	addi	a3,a3,2040 # 200627f8 <pmap_func+0x3cc>
20059bd8:	c03e                	c.swsp	a5,0(sp)
20059bda:	57e2                	c.lwsp	a5,56(sp)
20059bdc:	04900613          	addi	a2,zero,73
20059be0:	5d858593          	addi	a1,a1,1496 # 200625d8 <pmap_func+0x1ac>
20059be4:	4511                	c.li	a0,4
20059be6:	d67e                	c.swsp	t6,44(sp)
20059be8:	5a6010ef          	jal	ra,2005b18e <rtk_log_write>
20059bec:	5fb2                	c.lwsp	t6,44(sp)
20059bee:	57fd                	c.li	a5,-1
20059bf0:	ecfc89e3          	beq	s9,a5,20059ac2 <PSRAM_calibration+0x230>
20059bf4:	0189d663          	bge	s3,s8,20059c00 <PSRAM_calibration+0x36e>
20059bf8:	8952                	c.mv	s2,s4
20059bfa:	89e2                	c.mv	s3,s8
20059bfc:	8afe                	c.mv	s5,t6
20059bfe:	84e6                	c.mv	s1,s9
20059c00:	5ffd                	c.li	t6,-1
20059c02:	4c01                	c.li	s8,0
20059c04:	5cfd                	c.li	s9,-1
20059c06:	bd75                	c.j	20059ac2 <PSRAM_calibration+0x230>

20059c08 <System_Reset>:
20059c08:	4100c7b7          	lui	a5,0x4100c
20059c0c:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
20059c10:	4709                	c.li	a4,2
20059c12:	8399                	c.srli	a5,0x6
20059c14:	8b8d                	c.andi	a5,3
20059c16:	00e78963          	beq	a5,a4,20059c28 <System_Reset+0x20>
20059c1a:	4705                	c.li	a4,1
20059c1c:	02e78963          	beq	a5,a4,20059c4e <System_Reset+0x46>
20059c20:	40000737          	lui	a4,0x40000
20059c24:	c781                	c.beqz	a5,20059c2c <System_Reset+0x24>
20059c26:	8082                	c.jr	ra
20059c28:	20000737          	lui	a4,0x20000
20059c2c:	969696b7          	lui	a3,0x96969
20059c30:	4100c7b7          	lui	a5,0x4100c
20059c34:	69668693          	addi	a3,a3,1686 # 96969696 <__ctrace_end__+0xcc969696>
20059c38:	26d7a223          	sw	a3,612(a5) # 4100c264 <__km4_bd_boot_download_addr__+0x10ffa264>
20059c3c:	26e7a023          	sw	a4,608(a5)
20059c40:	69697737          	lui	a4,0x69697
20059c44:	96970713          	addi	a4,a4,-1687 # 69696969 <__kr4_psram_text_start__+0x9516949>
20059c48:	26e7a223          	sw	a4,612(a5)
20059c4c:	bfe9                	c.j	20059c26 <System_Reset+0x1e>
20059c4e:	80000737          	lui	a4,0x80000
20059c52:	bfe9                	c.j	20059c2c <System_Reset+0x24>

20059c54 <SWR_CORE_Vol_Get>:
20059c54:	4100d7b7          	lui	a5,0x4100d
20059c58:	f3e7c503          	lbu	a0,-194(a5) # 4100cf3e <__km4_bd_boot_download_addr__+0x10ffaf3e>
20059c5c:	0ff57513          	andi	a0,a0,255
20059c60:	8082                	c.jr	ra

20059c62 <wifi_set_user_config>:
20059c62:	1141                	c.addi	sp,-16
20059c64:	c422                	c.swsp	s0,8(sp)
20059c66:	04400613          	addi	a2,zero,68
20059c6a:	43818413          	addi	s0,gp,1080 # 20066dc0 <wifi_user_config>
20059c6e:	4581                	c.li	a1,0
20059c70:	43818513          	addi	a0,gp,1080 # 20066dc0 <wifi_user_config>
20059c74:	c606                	c.swsp	ra,12(sp)
20059c76:	dffb0097          	auipc	ra,0xdffb0
20059c7a:	e58080e7          	jalr	ra,-424(ra) # 9ace <__wrap_memset>
20059c7e:	4799                	c.li	a5,6
20059c80:	d05c                	c.sw	a5,36(s0)
20059c82:	47a1                	c.li	a5,8
20059c84:	d41c                	c.sw	a5,40(s0)
20059c86:	50800793          	addi	a5,zero,1288
20059c8a:	00f41f23          	sh	a5,30(s0)
20059c8e:	47a5                	c.li	a5,9
20059c90:	02f40023          	sb	a5,32(s0)
20059c94:	4785                	c.li	a5,1
20059c96:	00f405a3          	sb	a5,11(s0)
20059c9a:	00f41623          	sh	a5,12(s0)
20059c9e:	00f41423          	sh	a5,8(s0)
20059ca2:	0b0107b7          	lui	a5,0xb010
20059ca6:	10178793          	addi	a5,a5,257 # b010101 <__ap_sram_heap_size+0xafd0101>
20059caa:	c81c                	c.sw	a5,16(s0)
20059cac:	4789                	c.li	a5,2
20059cae:	cc1c                	c.sw	a5,24(s0)
20059cb0:	00020737          	lui	a4,0x20
20059cb4:	10100793          	addi	a5,zero,257
20059cb8:	02f41923          	sh	a5,50(s0)
20059cbc:	02f41a23          	sh	a5,52(s0)
20059cc0:	20070713          	addi	a4,a4,512 # 20200 <__bdram_heap_buffer_size__+0x8d00>
20059cc4:	4791                	c.li	a5,4
20059cc6:	c018                	c.sw	a4,0(s0)
20059cc8:	02f40b23          	sb	a5,54(s0)
20059ccc:	02f40c23          	sb	a5,56(s0)
20059cd0:	050f0737          	lui	a4,0x50f0
20059cd4:	6785                	c.lui	a5,0x1
20059cd6:	c058                	c.sw	a4,4(s0)
20059cd8:	38878793          	addi	a5,a5,904 # 1388 <CPU_ClkGet+0x872>
20059cdc:	4751                	c.li	a4,20
20059cde:	40b2                	c.lwsp	ra,12(sp)
20059ce0:	02042623          	sw	zero,44(s0)
20059ce4:	00040ea3          	sb	zero,29(s0)
20059ce8:	00040a23          	sb	zero,20(s0)
20059cec:	00041b23          	sh	zero,22(s0)
20059cf0:	020408a3          	sb	zero,49(s0)
20059cf4:	04e400a3          	sb	a4,65(s0)
20059cf8:	dc5c                	c.sw	a5,60(s0)
20059cfa:	4422                	c.lwsp	s0,8(sp)
20059cfc:	0141                	c.addi	sp,16
20059cfe:	8082                	c.jr	ra

20059d00 <hash_index>:
20059d00:	4781                	c.li	a5,0
20059d02:	08300693          	addi	a3,zero,131
20059d06:	00054703          	lbu	a4,0(a0) # 150000 <__ap_sram_heap_size+0x110000>
20059d0a:	e709                	c.bnez	a4,20059d14 <hash_index+0x14>
20059d0c:	00179513          	slli	a0,a5,0x1
20059d10:	8105                	c.srli	a0,0x1
20059d12:	8082                	c.jr	ra
20059d14:	02d787b3          	mul	a5,a5,a3
20059d18:	0505                	c.addi	a0,1
20059d1a:	97ba                	c.add	a5,a4
20059d1c:	b7ed                	c.j	20059d06 <hash_index+0x6>

20059d1e <log_add_new_command>:
20059d1e:	1141                	c.addi	sp,-16
20059d20:	c422                	c.swsp	s0,8(sp)
20059d22:	c606                	c.swsp	ra,12(sp)
20059d24:	842a                	c.mv	s0,a0
20059d26:	4108                	c.lw	a0,0(a0)
20059d28:	3fe1                	c.jal	20059d00 <hash_index>
20059d2a:	02000793          	addi	a5,zero,32
20059d2e:	02f56533          	rem	a0,a0,a5
20059d32:	47c18793          	addi	a5,gp,1148 # 20066e04 <log_hash>
20059d36:	00840713          	addi	a4,s0,8
20059d3a:	050e                	c.slli	a0,0x3
20059d3c:	97aa                	c.add	a5,a0
20059d3e:	4394                	c.lw	a3,0(a5)
20059d40:	c2d8                	c.sw	a4,4(a3)
20059d42:	c414                	c.sw	a3,8(s0)
20059d44:	c45c                	c.sw	a5,12(s0)
20059d46:	c398                	c.sw	a4,0(a5)
20059d48:	40b2                	c.lwsp	ra,12(sp)
20059d4a:	4422                	c.lwsp	s0,8(sp)
20059d4c:	0141                	c.addi	sp,16
20059d4e:	8082                	c.jr	ra

20059d50 <atcmd_service_init>:
20059d50:	47c18793          	addi	a5,gp,1148 # 20066e04 <log_hash>
20059d54:	10078713          	addi	a4,a5,256
20059d58:	c39c                	c.sw	a5,0(a5)
20059d5a:	c3dc                	c.sw	a5,4(a5)
20059d5c:	07a1                	c.addi	a5,8
20059d5e:	fee79de3          	bne	a5,a4,20059d58 <atcmd_service_init+0x8>
20059d62:	e781a783          	lw	a5,-392(gp) # 20066800 <log_init_table>
20059d66:	8782                	c.jr	a5

20059d68 <atcmd_service_add_table>:
20059d68:	1141                	c.addi	sp,-16
20059d6a:	c422                	c.swsp	s0,8(sp)
20059d6c:	c226                	c.swsp	s1,4(sp)
20059d6e:	c04a                	c.swsp	s2,0(sp)
20059d70:	c606                	c.swsp	ra,12(sp)
20059d72:	892a                	c.mv	s2,a0
20059d74:	84ae                	c.mv	s1,a1
20059d76:	4401                	c.li	s0,0
20059d78:	00944863          	blt	s0,s1,20059d88 <atcmd_service_add_table+0x20>
20059d7c:	40b2                	c.lwsp	ra,12(sp)
20059d7e:	4422                	c.lwsp	s0,8(sp)
20059d80:	4492                	c.lwsp	s1,4(sp)
20059d82:	4902                	c.lwsp	s2,0(sp)
20059d84:	0141                	c.addi	sp,16
20059d86:	8082                	c.jr	ra
20059d88:	00441513          	slli	a0,s0,0x4
20059d8c:	954a                	c.add	a0,s2
20059d8e:	3f41                	c.jal	20059d1e <log_add_new_command>
20059d90:	0405                	c.addi	s0,1
20059d92:	b7dd                	c.j	20059d78 <atcmd_service_add_table+0x10>

20059d94 <atcmd_action>:
20059d94:	1141                	c.addi	sp,-16
20059d96:	c422                	c.swsp	s0,8(sp)
20059d98:	c226                	c.swsp	s1,4(sp)
20059d9a:	c04a                	c.swsp	s2,0(sp)
20059d9c:	c606                	c.swsp	ra,12(sp)
20059d9e:	892a                	c.mv	s2,a0
20059da0:	02000413          	addi	s0,zero,32
20059da4:	3fb1                	c.jal	20059d00 <hash_index>
20059da6:	02856433          	rem	s0,a0,s0
20059daa:	00341793          	slli	a5,s0,0x3
20059dae:	47c18413          	addi	s0,gp,1148 # 20066e04 <log_hash>
20059db2:	943e                	c.add	s0,a5
20059db4:	4004                	c.lw	s1,0(s0)
20059db6:	00849463          	bne	s1,s0,20059dbe <atcmd_action+0x2a>
20059dba:	4501                	c.li	a0,0
20059dbc:	a819                	c.j	20059dd2 <atcmd_action+0x3e>
20059dbe:	ff84a503          	lw	a0,-8(s1)
20059dc2:	85ca                	c.mv	a1,s2
20059dc4:	dffb0097          	auipc	ra,0xdffb0
20059dc8:	d16080e7          	jalr	ra,-746(ra) # 9ada <__wrap_strcmp>
20059dcc:	e909                	c.bnez	a0,20059dde <atcmd_action+0x4a>
20059dce:	ffc4a503          	lw	a0,-4(s1)
20059dd2:	40b2                	c.lwsp	ra,12(sp)
20059dd4:	4422                	c.lwsp	s0,8(sp)
20059dd6:	4492                	c.lwsp	s1,4(sp)
20059dd8:	4902                	c.lwsp	s2,0(sp)
20059dda:	0141                	c.addi	sp,16
20059ddc:	8082                	c.jr	ra
20059dde:	4084                	c.lw	s1,0(s1)
20059de0:	bfd9                	c.j	20059db6 <atcmd_action+0x22>

20059de2 <atcmd_handler>:
20059de2:	7139                	c.addi16sp	sp,-64
20059de4:	dc22                	c.swsp	s0,56(sp)
20059de6:	c42a                	c.swsp	a0,8(sp)
20059de8:	842a                	c.mv	s0,a0
20059dea:	4675                	c.li	a2,29
20059dec:	4581                	c.li	a1,0
20059dee:	0808                	c.addi4spn	a0,sp,16
20059df0:	de06                	c.swsp	ra,60(sp)
20059df2:	da26                	c.swsp	s1,52(sp)
20059df4:	c602                	c.swsp	zero,12(sp)
20059df6:	dffb0097          	auipc	ra,0xdffb0
20059dfa:	cd8080e7          	jalr	ra,-808(ra) # 9ace <__wrap_memset>
20059dfe:	200635b7          	lui	a1,0x20063
20059e02:	4609                	c.li	a2,2
20059e04:	85458593          	addi	a1,a1,-1964 # 20062854 <pmap_func+0x428>
20059e08:	8522                	c.mv	a0,s0
20059e0a:	773070ef          	jal	ra,20061d7c <strncmp>
20059e0e:	c519                	c.beqz	a0,20059e1c <atcmd_handler+0x3a>
20059e10:	4501                	c.li	a0,0
20059e12:	50f2                	c.lwsp	ra,60(sp)
20059e14:	5462                	c.lwsp	s0,56(sp)
20059e16:	54d2                	c.lwsp	s1,52(sp)
20059e18:	6121                	c.addi16sp	sp,64
20059e1a:	8082                	c.jr	ra
20059e1c:	200635b7          	lui	a1,0x20063
20059e20:	85858593          	addi	a1,a1,-1960 # 20062858 <pmap_func+0x42c>
20059e24:	0028                	c.addi4spn	a0,sp,8
20059e26:	02c080ef          	jal	ra,20061e52 <strsep>
20059e2a:	200635b7          	lui	a1,0x20063
20059e2e:	842a                	c.mv	s0,a0
20059e30:	89c58593          	addi	a1,a1,-1892 # 2006289c <pmap_func+0x470>
20059e34:	0028                	c.addi4spn	a0,sp,8
20059e36:	01c080ef          	jal	ra,20061e52 <strsep>
20059e3a:	84aa                	c.mv	s1,a0
20059e3c:	d871                	c.beqz	s0,20059e10 <atcmd_handler+0x2e>
20059e3e:	8522                	c.mv	a0,s0
20059e40:	dffb0097          	auipc	ra,0xdffb0
20059e44:	c98080e7          	jalr	ra,-872(ra) # 9ad8 <__wrap_strlen>
20059e48:	478d                	c.li	a5,3
20059e4a:	fca7f3e3          	bgeu	a5,a0,20059e10 <atcmd_handler+0x2e>
20059e4e:	85a2                	c.mv	a1,s0
20059e50:	02000613          	addi	a2,zero,32
20059e54:	0068                	c.addi4spn	a0,sp,12
20059e56:	793070ef          	jal	ra,20061de8 <strncpy>
20059e5a:	00e10513          	addi	a0,sp,14
20059e5e:	3f1d                	c.jal	20059d94 <atcmd_action>
20059e60:	842a                	c.mv	s0,a0
20059e62:	4501                	c.li	a0,0
20059e64:	d45d                	c.beqz	s0,20059e12 <atcmd_handler+0x30>
20059e66:	8526                	c.mv	a0,s1
20059e68:	9402                	c.jalr	s0
20059e6a:	8522                	c.mv	a0,s0
20059e6c:	b75d                	c.j	20059e12 <atcmd_handler+0x30>

20059e6e <parse_param>:
20059e6e:	7179                	c.addi16sp	sp,-48
20059e70:	d422                	c.swsp	s0,40(sp)
20059e72:	d606                	c.swsp	ra,44(sp)
20059e74:	d226                	c.swsp	s1,36(sp)
20059e76:	d04a                	c.swsp	s2,32(sp)
20059e78:	ce4e                	c.swsp	s3,28(sp)
20059e7a:	4405                	c.li	s0,1
20059e7c:	c505                	c.beqz	a0,20059ea4 <parse_param+0x36>
20059e7e:	84ae                	c.mv	s1,a1
20059e80:	c62a                	c.swsp	a0,12(sp)
20059e82:	4405                	c.li	s0,1
20059e84:	200639b7          	lui	s3,0x20063
20059e88:	495d                	c.li	s2,23
20059e8a:	85c98593          	addi	a1,s3,-1956 # 2006285c <pmap_func+0x430>
20059e8e:	0068                	c.addi4spn	a0,sp,12
20059e90:	7c3070ef          	jal	ra,20061e52 <strsep>
20059e94:	00241793          	slli	a5,s0,0x2
20059e98:	97a6                	c.add	a5,s1
20059e9a:	c388                	c.sw	a0,0(a5)
20059e9c:	c501                	c.beqz	a0,20059ea4 <parse_param+0x36>
20059e9e:	0405                	c.addi	s0,1
20059ea0:	ff2415e3          	bne	s0,s2,20059e8a <parse_param+0x1c>
20059ea4:	50b2                	c.lwsp	ra,44(sp)
20059ea6:	8522                	c.mv	a0,s0
20059ea8:	5422                	c.lwsp	s0,40(sp)
20059eaa:	5492                	c.lwsp	s1,36(sp)
20059eac:	5902                	c.lwsp	s2,32(sp)
20059eae:	49f2                	c.lwsp	s3,28(sp)
20059eb0:	6145                	c.addi16sp	sp,48
20059eb2:	8082                	c.jr	ra

20059eb4 <mp_command_handler>:
20059eb4:	200635b7          	lui	a1,0x20063
20059eb8:	1141                	c.addi	sp,-16
20059eba:	461d                	c.li	a2,7
20059ebc:	86058593          	addi	a1,a1,-1952 # 20062860 <pmap_func+0x434>
20059ec0:	c422                	c.swsp	s0,8(sp)
20059ec2:	c606                	c.swsp	ra,12(sp)
20059ec4:	842a                	c.mv	s0,a0
20059ec6:	6b7070ef          	jal	ra,20061d7c <strncmp>
20059eca:	e129                	c.bnez	a0,20059f0c <mp_command_handler+0x58>
20059ecc:	041d                	c.addi	s0,7
20059ece:	8522                	c.mv	a0,s0
20059ed0:	dffb0097          	auipc	ra,0xdffb0
20059ed4:	c08080e7          	jalr	ra,-1016(ra) # 9ad8 <__wrap_strlen>
20059ed8:	4605                	c.li	a2,1
20059eda:	00150593          	addi	a1,a0,1
20059ede:	8522                	c.mv	a0,s0
20059ee0:	393060ef          	jal	ra,20060a72 <inic_mp_command>
20059ee4:	043010ef          	jal	ra,2005b726 <rtos_mem_get_free_heap_size>
20059ee8:	85aa                	c.mv	a1,a0
20059eea:	20063537          	lui	a0,0x20063
20059eee:	86850513          	addi	a0,a0,-1944 # 20062868 <pmap_func+0x43c>
20059ef2:	4b4050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20059ef6:	20063537          	lui	a0,0x20063
20059efa:	89450513          	addi	a0,a0,-1900 # 20062894 <pmap_func+0x468>
20059efe:	4a8050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20059f02:	4505                	c.li	a0,1
20059f04:	40b2                	c.lwsp	ra,12(sp)
20059f06:	4422                	c.lwsp	s0,8(sp)
20059f08:	0141                	c.addi	sp,16
20059f0a:	8082                	c.jr	ra
20059f0c:	4501                	c.li	a0,0
20059f0e:	bfdd                	c.j	20059f04 <mp_command_handler+0x50>

20059f10 <atcmd_service>:
20059f10:	1141                	c.addi	sp,-16
20059f12:	c606                	c.swsp	ra,12(sp)
20059f14:	35f9                	c.jal	20059de2 <atcmd_handler>
20059f16:	c505                	c.beqz	a0,20059f3e <atcmd_service+0x2e>
20059f18:	00f010ef          	jal	ra,2005b726 <rtos_mem_get_free_heap_size>
20059f1c:	85aa                	c.mv	a1,a0
20059f1e:	20063537          	lui	a0,0x20063
20059f22:	86850513          	addi	a0,a0,-1944 # 20062868 <pmap_func+0x43c>
20059f26:	480050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20059f2a:	20063537          	lui	a0,0x20063
20059f2e:	89450513          	addi	a0,a0,-1900 # 20062894 <pmap_func+0x468>
20059f32:	474050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20059f36:	4505                	c.li	a0,1
20059f38:	40b2                	c.lwsp	ra,12(sp)
20059f3a:	0141                	c.addi	sp,16
20059f3c:	8082                	c.jr	ra
20059f3e:	4501                	c.li	a0,0
20059f40:	bfe5                	c.j	20059f38 <atcmd_service+0x28>

20059f42 <at_otaclear>:
20059f42:	1141                	c.addi	sp,-16
20059f44:	c606                	c.swsp	ra,12(sp)
20059f46:	2b4030ef          	jal	ra,2005d1fa <sys_clear_ota_signature>
20059f4a:	40b2                	c.lwsp	ra,12(sp)
20059f4c:	200635b7          	lui	a1,0x20063
20059f50:	20063537          	lui	a0,0x20063
20059f54:	8a058593          	addi	a1,a1,-1888 # 200628a0 <pmap_func+0x474>
20059f58:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
20059f5c:	0141                	c.addi	sp,16
20059f5e:	4480506f          	jal	zero,2005f3a6 <DiagPrintf_minimal>

20059f62 <at_otarecover>:
20059f62:	1141                	c.addi	sp,-16
20059f64:	c606                	c.swsp	ra,12(sp)
20059f66:	3b6030ef          	jal	ra,2005d31c <sys_recover_ota_signature>
20059f6a:	40b2                	c.lwsp	ra,12(sp)
20059f6c:	200635b7          	lui	a1,0x20063
20059f70:	20063537          	lui	a0,0x20063
20059f74:	8b858593          	addi	a1,a1,-1864 # 200628b8 <pmap_func+0x48c>
20059f78:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
20059f7c:	0141                	c.addi	sp,16
20059f7e:	4280506f          	jal	zero,2005f3a6 <DiagPrintf_minimal>

20059f82 <at_test>:
20059f82:	c521                	c.beqz	a0,20059fca <at_test+0x48>
20059f84:	1141                	c.addi	sp,-16
20059f86:	c606                	c.swsp	ra,12(sp)
20059f88:	c422                	c.swsp	s0,8(sp)
20059f8a:	842a                	c.mv	s0,a0
20059f8c:	dffb0097          	auipc	ra,0xdffb0
20059f90:	b4c080e7          	jalr	ra,-1204(ra) # 9ad8 <__wrap_strlen>
20059f94:	85aa                	c.mv	a1,a0
20059f96:	20063537          	lui	a0,0x20063
20059f9a:	8c850513          	addi	a0,a0,-1848 # 200628c8 <pmap_func+0x49c>
20059f9e:	408050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20059fa2:	20063537          	lui	a0,0x20063
20059fa6:	85a2                	c.mv	a1,s0
20059fa8:	8dc50513          	addi	a0,a0,-1828 # 200628dc <pmap_func+0x4b0>
20059fac:	3fa050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20059fb0:	4422                	c.lwsp	s0,8(sp)
20059fb2:	200635b7          	lui	a1,0x20063
20059fb6:	20063537          	lui	a0,0x20063
20059fba:	40b2                	c.lwsp	ra,12(sp)
20059fbc:	8ec58593          	addi	a1,a1,-1812 # 200628ec <pmap_func+0x4c0>
20059fc0:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
20059fc4:	0141                	c.addi	sp,16
20059fc6:	3e00506f          	jal	zero,2005f3a6 <DiagPrintf_minimal>
20059fca:	200635b7          	lui	a1,0x20063
20059fce:	20063537          	lui	a0,0x20063
20059fd2:	8ec58593          	addi	a1,a1,-1812 # 200628ec <pmap_func+0x4c0>
20059fd6:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
20059fda:	b7f5                	c.j	20059fc6 <at_test+0x44>

20059fdc <at_rst>:
20059fdc:	200635b7          	lui	a1,0x20063
20059fe0:	20063537          	lui	a0,0x20063
20059fe4:	1141                	c.addi	sp,-16
20059fe6:	8f458593          	addi	a1,a1,-1804 # 200628f4 <pmap_func+0x4c8>
20059fea:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
20059fee:	c606                	c.swsp	ra,12(sp)
20059ff0:	3b6050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20059ff4:	40b2                	c.lwsp	ra,12(sp)
20059ff6:	0141                	c.addi	sp,16
20059ff8:	4600306f          	jal	zero,2005d458 <sys_reset>

20059ffc <at_state>:
20059ffc:	bf010113          	addi	sp,sp,-1040
2005a000:	850a                	c.mv	a0,sp
2005a002:	40112623          	sw	ra,1036(sp)
2005a006:	40812423          	sw	s0,1032(sp)
2005a00a:	39a040ef          	jal	ra,2005e3a4 <vTaskList>
2005a00e:	20063537          	lui	a0,0x20063
2005a012:	858a                	c.mv	a1,sp
2005a014:	8fc50513          	addi	a0,a0,-1796 # 200628fc <pmap_func+0x4d0>
2005a018:	38e050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a01c:	850a                	c.mv	a0,sp
2005a01e:	5c3000ef          	jal	ra,2005ade0 <vPortGetHeapStats>
2005a022:	20063537          	lui	a0,0x20063
2005a026:	91050513          	addi	a0,a0,-1776 # 20062910 <pmap_func+0x4e4>
2005a02a:	37c050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a02e:	ec81a583          	lw	a1,-312(gp) # 20066850 <total_heap_size>
2005a032:	20063537          	lui	a0,0x20063
2005a036:	92050513          	addi	a0,a0,-1760 # 20062920 <pmap_func+0x4f4>
2005a03a:	36c050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a03e:	4582                	c.lwsp	a1,0(sp)
2005a040:	20063537          	lui	a0,0x20063
2005a044:	93450513          	addi	a0,a0,-1740 # 20062934 <pmap_func+0x508>
2005a048:	35e050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a04c:	4782                	c.lwsp	a5,0(sp)
2005a04e:	ec81a583          	lw	a1,-312(gp) # 20066850 <total_heap_size>
2005a052:	20063537          	lui	a0,0x20063
2005a056:	94850513          	addi	a0,a0,-1720 # 20062948 <pmap_func+0x51c>
2005a05a:	8d9d                	c.sub	a1,a5
2005a05c:	34a050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a060:	47c2                	c.lwsp	a5,16(sp)
2005a062:	ec81a583          	lw	a1,-312(gp) # 20066850 <total_heap_size>
2005a066:	20063537          	lui	a0,0x20063
2005a06a:	95c50513          	addi	a0,a0,-1700 # 2006295c <pmap_func+0x530>
2005a06e:	8d9d                	c.sub	a1,a5
2005a070:	336050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a074:	200635b7          	lui	a1,0x20063
2005a078:	20063537          	lui	a0,0x20063
2005a07c:	97058593          	addi	a1,a1,-1680 # 20062970 <pmap_func+0x544>
2005a080:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
2005a084:	322050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a088:	40c12083          	lw	ra,1036(sp)
2005a08c:	40812403          	lw	s0,1032(sp)
2005a090:	41010113          	addi	sp,sp,1040
2005a094:	8082                	c.jr	ra

2005a096 <at_gmr>:
2005a096:	715d                	c.addi16sp	sp,-80
2005a098:	40000513          	addi	a0,zero,1024
2005a09c:	c686                	c.swsp	ra,76(sp)
2005a09e:	c4a2                	c.swsp	s0,72(sp)
2005a0a0:	c2a6                	c.swsp	s1,68(sp)
2005a0a2:	652010ef          	jal	ra,2005b6f4 <rtos_mem_malloc>
2005a0a6:	842a                	c.mv	s0,a0
2005a0a8:	20063537          	lui	a0,0x20063
2005a0ac:	4685                	c.li	a3,1
2005a0ae:	4601                	c.li	a2,0
2005a0b0:	4585                	c.li	a1,1
2005a0b2:	97850513          	addi	a0,a0,-1672 # 20062978 <pmap_func+0x54c>
2005a0b6:	2f0050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a0ba:	8522                	c.mv	a0,s0
2005a0bc:	3ff00593          	addi	a1,zero,1023
2005a0c0:	1cb040ef          	jal	ra,2005ea8a <ChipInfo_GetSocName_ToBuf>
2005a0c4:	200634b7          	lui	s1,0x20063
2005a0c8:	85a2                	c.mv	a1,s0
2005a0ca:	99c48513          	addi	a0,s1,-1636 # 2006299c <pmap_func+0x570>
2005a0ce:	2d8050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a0d2:	8522                	c.mv	a0,s0
2005a0d4:	3ff00593          	addi	a1,zero,1023
2005a0d8:	26f040ef          	jal	ra,2005eb46 <ChipInfo_GetLibVersion_ToBuf>
2005a0dc:	85a2                	c.mv	a1,s0
2005a0de:	99c48513          	addi	a0,s1,-1636
2005a0e2:	2c4050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a0e6:	8522                	c.mv	a0,s0
2005a0e8:	636010ef          	jal	ra,2005b71e <rtos_mem_free>
2005a0ec:	200635b7          	lui	a1,0x20063
2005a0f0:	02000613          	addi	a2,zero,32
2005a0f4:	9a058593          	addi	a1,a1,-1632 # 200629a0 <pmap_func+0x574>
2005a0f8:	850a                	c.mv	a0,sp
2005a0fa:	4ef070ef          	jal	ra,20061de8 <strncpy>
2005a0fe:	200635b7          	lui	a1,0x20063
2005a102:	02000613          	addi	a2,zero,32
2005a106:	9a858593          	addi	a1,a1,-1624 # 200629a8 <pmap_func+0x57c>
2005a10a:	1008                	c.addi4spn	a0,sp,32
2005a10c:	4dd070ef          	jal	ra,20061de8 <strncpy>
2005a110:	20063737          	lui	a4,0x20063
2005a114:	200635b7          	lui	a1,0x20063
2005a118:	20063537          	lui	a0,0x20063
2005a11c:	1014                	c.addi4spn	a3,sp,32
2005a11e:	860a                	c.mv	a2,sp
2005a120:	9b070713          	addi	a4,a4,-1616 # 200629b0 <pmap_func+0x584>
2005a124:	9c458593          	addi	a1,a1,-1596 # 200629c4 <pmap_func+0x598>
2005a128:	9cc50513          	addi	a0,a0,-1588 # 200629cc <pmap_func+0x5a0>
2005a12c:	27a050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a130:	40b6                	c.lwsp	ra,76(sp)
2005a132:	4426                	c.lwsp	s0,72(sp)
2005a134:	4496                	c.lwsp	s1,68(sp)
2005a136:	6161                	c.addi16sp	sp,80
2005a138:	8082                	c.jr	ra

2005a13a <at_log>:
2005a13a:	7159                	c.addi16sp	sp,-112
2005a13c:	d2a6                	c.swsp	s1,100(sp)
2005a13e:	05c00613          	addi	a2,zero,92
2005a142:	84aa                	c.mv	s1,a0
2005a144:	4581                	c.li	a1,0
2005a146:	0048                	c.addi4spn	a0,sp,4
2005a148:	d4a2                	c.swsp	s0,104(sp)
2005a14a:	d0ca                	c.swsp	s2,96(sp)
2005a14c:	d686                	c.swsp	ra,108(sp)
2005a14e:	20063437          	lui	s0,0x20063
2005a152:	dffb0097          	auipc	ra,0xdffb0
2005a156:	97c080e7          	jalr	ra,-1668(ra) # 9ace <__wrap_memset>
2005a15a:	20063937          	lui	s2,0x20063
2005a15e:	ec91                	c.bnez	s1,2005a17a <at_log+0x40>
2005a160:	200636b7          	lui	a3,0x20063
2005a164:	9dc68693          	addi	a3,a3,-1572 # 200629dc <pmap_func+0x5b0>
2005a168:	04100613          	addi	a2,zero,65
2005a16c:	9f040593          	addi	a1,s0,-1552 # 200629f0 <pmap_func+0x5c4>
2005a170:	4505                	c.li	a0,1
2005a172:	01c010ef          	jal	ra,2005b18e <rtk_log_write>
2005a176:	4605                	c.li	a2,1
2005a178:	aa35                	c.j	2005a2b4 <at_log+0x17a>
2005a17a:	8526                	c.mv	a0,s1
2005a17c:	004c                	c.addi4spn	a1,sp,4
2005a17e:	39c5                	c.jal	20059e6e <parse_param>
2005a180:	4785                	c.li	a5,1
2005a182:	84aa                	c.mv	s1,a0
2005a184:	1aa7d063          	bge	a5,a0,2005a324 <at_log+0x1ea>
2005a188:	4522                	c.lwsp	a0,8(sp)
2005a18a:	18050d63          	beq	a0,zero,2005a324 <at_log+0x1ea>
2005a18e:	209070ef          	jal	ra,20061b96 <atoi>
2005a192:	0ff57713          	andi	a4,a0,255
2005a196:	4789                	c.li	a5,2
2005a198:	16f70563          	beq	a4,a5,2005a302 <at_log+0x1c8>
2005a19c:	00e7ef63          	bltu	a5,a4,2005a1ba <at_log+0x80>
2005a1a0:	e771                	c.bnez	a4,2005a26c <at_log+0x132>
2005a1a2:	478d                	c.li	a5,3
2005a1a4:	00f49663          	bne	s1,a5,2005a1b0 <at_log+0x76>
2005a1a8:	4532                	c.lwsp	a0,12(sp)
2005a1aa:	00054783          	lbu	a5,0(a0)
2005a1ae:	e78d                	c.bnez	a5,2005a1d8 <at_log+0x9e>
2005a1b0:	200636b7          	lui	a3,0x20063
2005a1b4:	9f468693          	addi	a3,a3,-1548 # 200629f4 <pmap_func+0x5c8>
2005a1b8:	bf45                	c.j	2005a168 <at_log+0x2e>
2005a1ba:	468d                	c.li	a3,3
2005a1bc:	14d70f63          	beq	a4,a3,2005a31a <at_log+0x1e0>
2005a1c0:	200636b7          	lui	a3,0x20063
2005a1c4:	a8868693          	addi	a3,a3,-1400 # 20062a88 <pmap_func+0x65c>
2005a1c8:	04100613          	addi	a2,zero,65
2005a1cc:	9f040593          	addi	a1,s0,-1552
2005a1d0:	4505                	c.li	a0,1
2005a1d2:	7bd000ef          	jal	ra,2005b18e <rtk_log_write>
2005a1d6:	b745                	c.j	2005a176 <at_log+0x3c>
2005a1d8:	57b000ef          	jal	ra,2005af52 <rtk_log_level_get>
2005a1dc:	45b2                	c.lwsp	a1,12(sp)
2005a1de:	862a                	c.mv	a2,a0
2005a1e0:	20063537          	lui	a0,0x20063
2005a1e4:	a1450513          	addi	a0,a0,-1516 # 20062a14 <pmap_func+0x5e8>
2005a1e8:	1be050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a1ec:	200636b7          	lui	a3,0x20063
2005a1f0:	aa468693          	addi	a3,a3,-1372 # 20062aa4 <pmap_func+0x678>
2005a1f4:	04100613          	addi	a2,zero,65
2005a1f8:	9f040593          	addi	a1,s0,-1552
2005a1fc:	4505                	c.li	a0,1
2005a1fe:	791000ef          	jal	ra,2005b18e <rtk_log_write>
2005a202:	200636b7          	lui	a3,0x20063
2005a206:	ac068693          	addi	a3,a3,-1344 # 20062ac0 <pmap_func+0x694>
2005a20a:	04500613          	addi	a2,zero,69
2005a20e:	9f040593          	addi	a1,s0,-1552
2005a212:	4509                	c.li	a0,2
2005a214:	77b000ef          	jal	ra,2005b18e <rtk_log_write>
2005a218:	200636b7          	lui	a3,0x20063
2005a21c:	adc68693          	addi	a3,a3,-1316 # 20062adc <pmap_func+0x6b0>
2005a220:	05700613          	addi	a2,zero,87
2005a224:	9f040593          	addi	a1,s0,-1552
2005a228:	450d                	c.li	a0,3
2005a22a:	765000ef          	jal	ra,2005b18e <rtk_log_write>
2005a22e:	200636b7          	lui	a3,0x20063
2005a232:	9f040593          	addi	a1,s0,-1552
2005a236:	af868693          	addi	a3,a3,-1288 # 20062af8 <pmap_func+0x6cc>
2005a23a:	04900613          	addi	a2,zero,73
2005a23e:	4511                	c.li	a0,4
2005a240:	74f000ef          	jal	ra,2005b18e <rtk_log_write>
2005a244:	20063537          	lui	a0,0x20063
2005a248:	b1050513          	addi	a0,a0,-1264 # 20062b10 <pmap_func+0x6e4>
2005a24c:	15a050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a250:	20063537          	lui	a0,0x20063
2005a254:	b2890593          	addi	a1,s2,-1240 # 20062b28 <pmap_func+0x6fc>
2005a258:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
2005a25c:	14a050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a260:	50b6                	c.lwsp	ra,108(sp)
2005a262:	5426                	c.lwsp	s0,104(sp)
2005a264:	5496                	c.lwsp	s1,100(sp)
2005a266:	5906                	c.lwsp	s2,96(sp)
2005a268:	6165                	c.addi16sp	sp,112
2005a26a:	8082                	c.jr	ra
2005a26c:	4791                	c.li	a5,4
2005a26e:	00f49a63          	bne	s1,a5,2005a282 <at_log+0x148>
2005a272:	47b2                	c.lwsp	a5,12(sp)
2005a274:	0007c783          	lbu	a5,0(a5)
2005a278:	c789                	c.beqz	a5,2005a282 <at_log+0x148>
2005a27a:	4542                	c.lwsp	a0,16(sp)
2005a27c:	00054783          	lbu	a5,0(a0)
2005a280:	e791                	c.bnez	a5,2005a28c <at_log+0x152>
2005a282:	200636b7          	lui	a3,0x20063
2005a286:	a2868693          	addi	a3,a3,-1496 # 20062a28 <pmap_func+0x5fc>
2005a28a:	bdf9                	c.j	2005a168 <at_log+0x2e>
2005a28c:	10b070ef          	jal	ra,20061b96 <atoi>
2005a290:	0ff57593          	andi	a1,a0,255
2005a294:	4532                	c.lwsp	a0,12(sp)
2005a296:	539000ef          	jal	ra,2005afce <rtk_log_level_set>
2005a29a:	d929                	c.beqz	a0,2005a1ec <at_log+0xb2>
2005a29c:	200636b7          	lui	a3,0x20063
2005a2a0:	04100613          	addi	a2,zero,65
2005a2a4:	a4868693          	addi	a3,a3,-1464 # 20062a48 <pmap_func+0x61c>
2005a2a8:	9f040593          	addi	a1,s0,-1552
2005a2ac:	4505                	c.li	a0,1
2005a2ae:	6e1000ef          	jal	ra,2005b18e <rtk_log_write>
2005a2b2:	4609                	c.li	a2,2
2005a2b4:	20063537          	lui	a0,0x20063
2005a2b8:	b2890593          	addi	a1,s2,-1240
2005a2bc:	b3050513          	addi	a0,a0,-1232 # 20062b30 <pmap_func+0x704>
2005a2c0:	0e6050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a2c4:	20063537          	lui	a0,0x20063
2005a2c8:	8d850513          	addi	a0,a0,-1832 # 200628d8 <pmap_func+0x4ac>
2005a2cc:	0da050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a2d0:	20063537          	lui	a0,0x20063
2005a2d4:	b4050513          	addi	a0,a0,-1216 # 20062b40 <pmap_func+0x714>
2005a2d8:	0ce050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a2dc:	20063537          	lui	a0,0x20063
2005a2e0:	b6c50513          	addi	a0,a0,-1172 # 20062b6c <pmap_func+0x740>
2005a2e4:	0c2050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a2e8:	20063537          	lui	a0,0x20063
2005a2ec:	ba450513          	addi	a0,a0,-1116 # 20062ba4 <pmap_func+0x778>
2005a2f0:	0b6050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a2f4:	20063537          	lui	a0,0x20063
2005a2f8:	bd850513          	addi	a0,a0,-1064 # 20062bd8 <pmap_func+0x7ac>
2005a2fc:	0aa050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a300:	b785                	c.j	2005a260 <at_log+0x126>
2005a302:	00e48763          	beq	s1,a4,2005a310 <at_log+0x1d6>
2005a306:	200636b7          	lui	a3,0x20063
2005a30a:	a6468693          	addi	a3,a3,-1436 # 20062a64 <pmap_func+0x638>
2005a30e:	bda9                	c.j	2005a168 <at_log+0x2e>
2005a310:	5ac18513          	addi	a0,gp,1452 # 20066f34 <rtk_log_tag_array>
2005a314:	3cb000ef          	jal	ra,2005aede <rtk_log_array_print>
2005a318:	bdd1                	c.j	2005a1ec <at_log+0xb2>
2005a31a:	fef496e3          	bne	s1,a5,2005a306 <at_log+0x1cc>
2005a31e:	415000ef          	jal	ra,2005af32 <rtk_log_array_clear>
2005a322:	b5e9                	c.j	2005a1ec <at_log+0xb2>
2005a324:	4711                	c.li	a4,4
2005a326:	bd69                	c.j	2005a1c0 <at_log+0x86>

2005a328 <at_rreg>:
2005a328:	7159                	c.addi16sp	sp,-112
2005a32a:	d4a2                	c.swsp	s0,104(sp)
2005a32c:	05c00613          	addi	a2,zero,92
2005a330:	842a                	c.mv	s0,a0
2005a332:	4581                	c.li	a1,0
2005a334:	0048                	c.addi4spn	a0,sp,4
2005a336:	d2a6                	c.swsp	s1,100(sp)
2005a338:	d686                	c.swsp	ra,108(sp)
2005a33a:	200634b7          	lui	s1,0x20063
2005a33e:	dffaf097          	auipc	ra,0xdffaf
2005a342:	790080e7          	jalr	ra,1936(ra) # 9ace <__wrap_memset>
2005a346:	ec05                	c.bnez	s0,2005a37e <at_rreg+0x56>
2005a348:	200636b7          	lui	a3,0x20063
2005a34c:	200635b7          	lui	a1,0x20063
2005a350:	05700613          	addi	a2,zero,87
2005a354:	9f058593          	addi	a1,a1,-1552 # 200629f0 <pmap_func+0x5c4>
2005a358:	450d                	c.li	a0,3
2005a35a:	c0c68693          	addi	a3,a3,-1012 # 20062c0c <pmap_func+0x7e0>
2005a35e:	631000ef          	jal	ra,2005b18e <rtk_log_write>
2005a362:	20063537          	lui	a0,0x20063
2005a366:	4605                	c.li	a2,1
2005a368:	c2848593          	addi	a1,s1,-984 # 20062c28 <pmap_func+0x7fc>
2005a36c:	b3050513          	addi	a0,a0,-1232 # 20062b30 <pmap_func+0x704>
2005a370:	036050ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a374:	50b6                	c.lwsp	ra,108(sp)
2005a376:	5426                	c.lwsp	s0,104(sp)
2005a378:	5496                	c.lwsp	s1,100(sp)
2005a37a:	6165                	c.addi16sp	sp,112
2005a37c:	8082                	c.jr	ra
2005a37e:	004c                	c.addi4spn	a1,sp,4
2005a380:	8522                	c.mv	a0,s0
2005a382:	aedff0ef          	jal	ra,20059e6e <parse_param>
2005a386:	ffe50713          	addi	a4,a0,-2
2005a38a:	4789                	c.li	a5,2
2005a38c:	fae7eee3          	bltu	a5,a4,2005a348 <at_rreg+0x20>
2005a390:	157d                	c.addi	a0,-1
2005a392:	0542                	c.slli	a0,0x10
2005a394:	002c                	c.addi4spn	a1,sp,8
2005a396:	8141                	c.srli	a0,0x10
2005a398:	17a010ef          	jal	ra,2005b512 <cmd_dump_word>
2005a39c:	20063537          	lui	a0,0x20063
2005a3a0:	c2848593          	addi	a1,s1,-984
2005a3a4:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
2005a3a8:	7ff040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a3ac:	b7e1                	c.j	2005a374 <at_rreg+0x4c>

2005a3ae <at_wreg>:
2005a3ae:	7159                	c.addi16sp	sp,-112
2005a3b0:	d4a2                	c.swsp	s0,104(sp)
2005a3b2:	05c00613          	addi	a2,zero,92
2005a3b6:	842a                	c.mv	s0,a0
2005a3b8:	4581                	c.li	a1,0
2005a3ba:	0048                	c.addi4spn	a0,sp,4
2005a3bc:	d2a6                	c.swsp	s1,100(sp)
2005a3be:	d686                	c.swsp	ra,108(sp)
2005a3c0:	200634b7          	lui	s1,0x20063
2005a3c4:	dffaf097          	auipc	ra,0xdffaf
2005a3c8:	70a080e7          	jalr	ra,1802(ra) # 9ace <__wrap_memset>
2005a3cc:	ec05                	c.bnez	s0,2005a404 <at_wreg+0x56>
2005a3ce:	200636b7          	lui	a3,0x20063
2005a3d2:	200635b7          	lui	a1,0x20063
2005a3d6:	05700613          	addi	a2,zero,87
2005a3da:	9f058593          	addi	a1,a1,-1552 # 200629f0 <pmap_func+0x5c4>
2005a3de:	450d                	c.li	a0,3
2005a3e0:	c3068693          	addi	a3,a3,-976 # 20062c30 <pmap_func+0x804>
2005a3e4:	5ab000ef          	jal	ra,2005b18e <rtk_log_write>
2005a3e8:	20063537          	lui	a0,0x20063
2005a3ec:	4605                	c.li	a2,1
2005a3ee:	c4c48593          	addi	a1,s1,-948 # 20062c4c <pmap_func+0x820>
2005a3f2:	b3050513          	addi	a0,a0,-1232 # 20062b30 <pmap_func+0x704>
2005a3f6:	7b1040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a3fa:	50b6                	c.lwsp	ra,108(sp)
2005a3fc:	5426                	c.lwsp	s0,104(sp)
2005a3fe:	5496                	c.lwsp	s1,100(sp)
2005a400:	6165                	c.addi16sp	sp,112
2005a402:	8082                	c.jr	ra
2005a404:	004c                	c.addi4spn	a1,sp,4
2005a406:	8522                	c.mv	a0,s0
2005a408:	a67ff0ef          	jal	ra,20059e6e <parse_param>
2005a40c:	478d                	c.li	a5,3
2005a40e:	fcf510e3          	bne	a0,a5,2005a3ce <at_wreg+0x20>
2005a412:	45a2                	c.lwsp	a1,8(sp)
2005a414:	4509                	c.li	a0,2
2005a416:	056010ef          	jal	ra,2005b46c <cmd_write_word>
2005a41a:	20063537          	lui	a0,0x20063
2005a41e:	c4c48593          	addi	a1,s1,-948
2005a422:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
2005a426:	781040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a42a:	bfc1                	c.j	2005a3fa <at_wreg+0x4c>

2005a42c <find_status>:
2005a42c:	872a                	c.mv	a4,a0
2005a42e:	4781                	c.li	a5,0
2005a430:	00c7c463          	blt	a5,a2,2005a438 <find_status+0xc>
2005a434:	4501                	c.li	a0,0
2005a436:	8082                	c.jr	ra
2005a438:	852e                	c.mv	a0,a1
2005a43a:	4594                	c.lw	a3,8(a1)
2005a43c:	02458593          	addi	a1,a1,36
2005a440:	fee68be3          	beq	a3,a4,2005a436 <find_status+0xa>
2005a444:	0785                	c.addi	a5,1
2005a446:	b7ed                	c.j	2005a430 <find_status+0x4>

2005a448 <find_free_delta>:
2005a448:	87aa                	c.mv	a5,a0
2005a44a:	4701                	c.li	a4,0
2005a44c:	00b74463          	blt	a4,a1,2005a454 <find_free_delta+0xc>
2005a450:	4501                	c.li	a0,0
2005a452:	8082                	c.jr	ra
2005a454:	853e                	c.mv	a0,a5
2005a456:	5794                	c.lw	a3,40(a5)
2005a458:	03878793          	addi	a5,a5,56
2005a45c:	dafd                	c.beqz	a3,2005a452 <find_free_delta+0xa>
2005a45e:	0705                	c.addi	a4,1
2005a460:	b7f5                	c.j	2005a44c <find_free_delta+0x4>

2005a462 <update_status>:
2005a462:	7179                	c.addi16sp	sp,-48
2005a464:	d226                	c.swsp	s1,36(sp)
2005a466:	ebc1a783          	lw	a5,-324(gp) # 20066844 <task_status>
2005a46a:	6705                	c.lui	a4,0x1
2005a46c:	d606                	c.swsp	ra,44(sp)
2005a46e:	d422                	c.swsp	s0,40(sp)
2005a470:	d04a                	c.swsp	s2,32(sp)
2005a472:	ce4e                	c.swsp	s3,28(sp)
2005a474:	cc52                	c.swsp	s4,24(sp)
2005a476:	ca56                	c.swsp	s5,20(sp)
2005a478:	c85a                	c.swsp	s6,16(sp)
2005a47a:	c65e                	c.swsp	s7,12(sp)
2005a47c:	c462                	c.swsp	s8,8(sp)
2005a47e:	00e786b3          	add	a3,a5,a4
2005a482:	2006a603          	lw	a2,512(a3)
2005a486:	ebc18493          	addi	s1,gp,-324 # 20066844 <task_status>
2005a48a:	ea19                	c.bnez	a2,2005a4a0 <update_status+0x3e>
2005a48c:	2046a603          	lw	a2,516(a3)
2005a490:	ea01                	c.bnez	a2,2005a4a0 <update_status+0x3e>
2005a492:	90070713          	addi	a4,a4,-1792 # 900 <BKUP_Set+0x5a>
2005a496:	973e                	c.add	a4,a5
2005a498:	20f6a023          	sw	a5,512(a3)
2005a49c:	20e6a223          	sw	a4,516(a3)
2005a4a0:	6405                	c.lui	s0,0x1
2005a4a2:	97a2                	c.add	a5,s0
2005a4a4:	20078793          	addi	a5,a5,512
2005a4a8:	47d8                	c.lw	a4,12(a5)
2005a4aa:	43d4                	c.lw	a3,4(a5)
2005a4ac:	21040913          	addi	s2,s0,528 # 1210 <CPU_ClkGet+0x6fa>
2005a4b0:	c798                	c.sw	a4,8(a5)
2005a4b2:	4398                	c.lw	a4,0(a5)
2005a4b4:	c394                	c.sw	a3,0(a5)
2005a4b6:	4a01                	c.li	s4,0
2005a4b8:	c3d8                	c.sw	a4,4(a5)
2005a4ba:	24c030ef          	jal	ra,2005d706 <uxTaskGetNumberOfTasks>
2005a4be:	409c                	c.lw	a5,0(s1)
2005a4c0:	85aa                	c.mv	a1,a0
2005a4c2:	4601                	c.li	a2,0
2005a4c4:	97a2                	c.add	a5,s0
2005a4c6:	20a7a623          	sw	a0,524(a5)
2005a4ca:	2047a503          	lw	a0,516(a5)
2005a4ce:	597030ef          	jal	ra,2005e264 <uxTaskGetSystemState>
2005a4d2:	409c                	c.lw	a5,0(s1)
2005a4d4:	e0040613          	addi	a2,s0,-512
2005a4d8:	4581                	c.li	a1,0
2005a4da:	00878733          	add	a4,a5,s0
2005a4de:	20a72623          	sw	a0,524(a4)
2005a4e2:	01278533          	add	a0,a5,s2
2005a4e6:	dffaf097          	auipc	ra,0xdffaf
2005a4ea:	5e8080e7          	jalr	ra,1512(ra) # 9ace <__wrap_memset>
2005a4ee:	0004aa83          	lw	s5,0(s1)
2005a4f2:	9456                	c.add	s0,s5
2005a4f4:	20842783          	lw	a5,520(s0)
2005a4f8:	00f05963          	bge	zero,a5,2005a50a <update_status+0xa8>
2005a4fc:	02400c13          	addi	s8,zero,36
2005a500:	9aca                	c.add	s5,s2
2005a502:	20c42783          	lw	a5,524(s0)
2005a506:	00fa4e63          	blt	s4,a5,2005a522 <update_status+0xc0>
2005a50a:	50b2                	c.lwsp	ra,44(sp)
2005a50c:	5422                	c.lwsp	s0,40(sp)
2005a50e:	5492                	c.lwsp	s1,36(sp)
2005a510:	5902                	c.lwsp	s2,32(sp)
2005a512:	49f2                	c.lwsp	s3,28(sp)
2005a514:	4a62                	c.lwsp	s4,24(sp)
2005a516:	4ad2                	c.lwsp	s5,20(sp)
2005a518:	4b42                	c.lwsp	s6,16(sp)
2005a51a:	4bb2                	c.lwsp	s7,12(sp)
2005a51c:	4c22                	c.lwsp	s8,8(sp)
2005a51e:	6145                	c.addi16sp	sp,48
2005a520:	8082                	c.jr	ra
2005a522:	038a0933          	mul	s2,s4,s8
2005a526:	20442b83          	lw	s7,516(s0)
2005a52a:	20842603          	lw	a2,520(s0)
2005a52e:	20042583          	lw	a1,512(s0)
2005a532:	9bca                	c.add	s7,s2
2005a534:	008ba503          	lw	a0,8(s7)
2005a538:	3dd5                	c.jal	2005a42c <find_status>
2005a53a:	04000593          	addi	a1,zero,64
2005a53e:	8b2a                	c.mv	s6,a0
2005a540:	8556                	c.mv	a0,s5
2005a542:	3719                	c.jal	2005a448 <find_free_delta>
2005a544:	000ba783          	lw	a5,0(s7)
2005a548:	84aa                	c.mv	s1,a0
2005a54a:	467d                	c.li	a2,31
2005a54c:	d51c                	c.sw	a5,40(a0)
2005a54e:	20442783          	lw	a5,516(s0)
2005a552:	97ca                	c.add	a5,s2
2005a554:	43cc                	c.lw	a1,4(a5)
2005a556:	c14c                	c.sw	a1,4(a0)
2005a558:	20442783          	lw	a5,516(s0)
2005a55c:	97ca                	c.add	a5,s2
2005a55e:	479c                	c.lw	a5,8(a5)
2005a560:	00052023          	sw	zero,0(a0)
2005a564:	0521                	c.addi	a0,8
2005a566:	d15c                	c.sw	a5,36(a0)
2005a568:	20442783          	lw	a5,516(s0)
2005a56c:	97ca                	c.add	a5,s2
2005a56e:	00c7c783          	lbu	a5,12(a5)
2005a572:	02f50623          	sb	a5,44(a0)
2005a576:	20442783          	lw	a5,516(s0)
2005a57a:	97ca                	c.add	a5,s2
2005a57c:	4b9c                	c.lw	a5,16(a5)
2005a57e:	d51c                	c.sw	a5,40(a0)
2005a580:	069070ef          	jal	ra,20061de8 <strncpy>
2005a584:	c095                	c.beqz	s1,2005a5a8 <update_status+0x146>
2005a586:	20442783          	lw	a5,516(s0)
2005a58a:	97ca                	c.add	a5,s2
2005a58c:	4f9c                	c.lw	a5,24(a5)
2005a58e:	000b0563          	beq	s6,zero,2005a598 <update_status+0x136>
2005a592:	018b2703          	lw	a4,24(s6)
2005a596:	8f99                	c.sub	a5,a4
2005a598:	c09c                	c.sw	a5,0(s1)
2005a59a:	20442783          	lw	a5,516(s0)
2005a59e:	993e                	c.add	s2,a5
2005a5a0:	00c94783          	lbu	a5,12(s2)
2005a5a4:	02f48a23          	sb	a5,52(s1)
2005a5a8:	0a05                	c.addi	s4,1
2005a5aa:	bfa1                	c.j	2005a502 <update_status+0xa0>

2005a5ac <print_delta>:
2005a5ac:	715d                	c.addi16sp	sp,-80
2005a5ae:	736277b7          	lui	a5,0x73627
2005a5b2:	c4a2                	c.swsp	s0,72(sp)
2005a5b4:	25278793          	addi	a5,a5,594 # 73627252 <__kr4_psram_text_start__+0x134a7232>
2005a5b8:	842a                	c.mv	s0,a0
2005a5ba:	20063737          	lui	a4,0x20063
2005a5be:	200636b7          	lui	a3,0x20063
2005a5c2:	20063637          	lui	a2,0x20063
2005a5c6:	200635b7          	lui	a1,0x20063
2005a5ca:	20063537          	lui	a0,0x20063
2005a5ce:	c43e                	c.swsp	a5,8(sp)
2005a5d0:	c5470713          	addi	a4,a4,-940 # 20062c54 <pmap_func+0x828>
2005a5d4:	04400793          	addi	a5,zero,68
2005a5d8:	c5c68693          	addi	a3,a3,-932 # 20062c5c <pmap_func+0x830>
2005a5dc:	c6460613          	addi	a2,a2,-924 # 20062c64 <pmap_func+0x838>
2005a5e0:	c6c58593          	addi	a1,a1,-916 # 20062c6c <pmap_func+0x840>
2005a5e4:	c7850513          	addi	a0,a0,-904 # 20062c78 <pmap_func+0x84c>
2005a5e8:	c2a6                	c.swsp	s1,68(sp)
2005a5ea:	c0ca                	c.swsp	s2,64(sp)
2005a5ec:	de4e                	c.swsp	s3,60(sp)
2005a5ee:	dc52                	c.swsp	s4,56(sp)
2005a5f0:	da56                	c.swsp	s5,52(sp)
2005a5f2:	d85a                	c.swsp	s6,48(sp)
2005a5f4:	d65e                	c.swsp	s7,44(sp)
2005a5f6:	ee22                	c.fswsp	fs0,28(sp)
2005a5f8:	00f10623          	sb	a5,12(sp)
2005a5fc:	c686                	c.swsp	ra,76(sp)
2005a5fe:	00011223          	sh	zero,4(sp)
2005a602:	00010323          	sb	zero,6(sp)
2005a606:	5a1040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a60a:	e581a407          	flw	fs0,-424(gp) # 200667e0 <__sdata_start__>
2005a60e:	6485                	c.lui	s1,0x1
2005a610:	4b81                	c.li	s7,0
2005a612:	03800993          	addi	s3,zero,56
2005a616:	06400b13          	addi	s6,zero,100
2005a61a:	21848a13          	addi	s4,s1,536 # 1218 <CPU_ClkGet+0x702>
2005a61e:	20063ab7          	lui	s5,0x20063
2005a622:	460d                	c.li	a2,3
2005a624:	4581                	c.li	a1,0
2005a626:	0048                	c.addi4spn	a0,sp,4
2005a628:	dffaf097          	auipc	ra,0xdffaf
2005a62c:	4a6080e7          	jalr	ra,1190(ra) # 9ace <__wrap_memset>
2005a630:	033b8633          	mul	a2,s7,s3
2005a634:	ebc1a803          	lw	a6,-324(gp) # 20066844 <task_status>
2005a638:	d0047753          	fcvt.s.w	fa4,s0
2005a63c:	4529                	c.li	a0,10
2005a63e:	00c806b3          	add	a3,a6,a2
2005a642:	96a6                	c.add	a3,s1
2005a644:	2106a783          	lw	a5,528(a3)
2005a648:	d007f7d3          	fcvt.s.w	fa5,a5
2005a64c:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a650:	18e7f7d3          	fdiv.s	fa5,fa5,fa4
2005a654:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a658:	c0079753          	fcvt.w.s	a4,fa5,rtz
2005a65c:	036767b3          	rem	a5,a4,s6
2005a660:	02a787b3          	mul	a5,a5,a0
2005a664:	0367c5b3          	div	a1,a5,s6
2005a668:	0367e7b3          	rem	a5,a5,s6
2005a66c:	03058593          	addi	a1,a1,48
2005a670:	00b10223          	sb	a1,4(sp)
2005a674:	02a787b3          	mul	a5,a5,a0
2005a678:	0367c7b3          	div	a5,a5,s6
2005a67c:	03078793          	addi	a5,a5,48
2005a680:	00f102a3          	sb	a5,5(sp)
2005a684:	2386a783          	lw	a5,568(a3)
2005a688:	c78d                	c.beqz	a5,2005a6b2 <print_delta+0x106>
2005a68a:	03674733          	div	a4,a4,s6
2005a68e:	2446c503          	lbu	a0,580(a3)
2005a692:	014605b3          	add	a1,a2,s4
2005a696:	2406a683          	lw	a3,576(a3)
2005a69a:	01050793          	addi	a5,a0,16
2005a69e:	00278533          	add	a0,a5,sp
2005a6a2:	ff854603          	lbu	a2,-8(a0)
2005a6a6:	005c                	c.addi4spn	a5,sp,4
2005a6a8:	95c2                	c.add	a1,a6
2005a6aa:	c88a8513          	addi	a0,s5,-888 # 20062c88 <pmap_func+0x85c>
2005a6ae:	4f9040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a6b2:	0b85                	c.addi	s7,1
2005a6b4:	04000793          	addi	a5,zero,64
2005a6b8:	f6fb95e3          	bne	s7,a5,2005a622 <print_delta+0x76>
2005a6bc:	40b6                	c.lwsp	ra,76(sp)
2005a6be:	4426                	c.lwsp	s0,72(sp)
2005a6c0:	4496                	c.lwsp	s1,68(sp)
2005a6c2:	4906                	c.lwsp	s2,64(sp)
2005a6c4:	59f2                	c.lwsp	s3,60(sp)
2005a6c6:	5a62                	c.lwsp	s4,56(sp)
2005a6c8:	5ad2                	c.lwsp	s5,52(sp)
2005a6ca:	5b42                	c.lwsp	s6,48(sp)
2005a6cc:	5bb2                	c.lwsp	s7,44(sp)
2005a6ce:	6472                	c.flwsp	fs0,28(sp)
2005a6d0:	6161                	c.addi16sp	sp,80
2005a6d2:	8082                	c.jr	ra

2005a6d4 <cpu_stat_thread>:
2005a6d4:	7179                	c.addi16sp	sp,-48
2005a6d6:	c62a                	c.swsp	a0,12(sp)
2005a6d8:	4521                	c.li	a0,8
2005a6da:	d226                	c.swsp	s1,36(sp)
2005a6dc:	d606                	c.swsp	ra,44(sp)
2005a6de:	d422                	c.swsp	s0,40(sp)
2005a6e0:	d04a                	c.swsp	s2,32(sp)
2005a6e2:	ce4e                	c.swsp	s3,28(sp)
2005a6e4:	cc52                	c.swsp	s4,24(sp)
2005a6e6:	ca56                	c.swsp	s5,20(sp)
2005a6e8:	00c010ef          	jal	ra,2005b6f4 <rtos_mem_malloc>
2005a6ec:	45b2                	c.lwsp	a1,12(sp)
2005a6ee:	ec018493          	addi	s1,gp,-320 # 20066848 <top_exit_sema>
2005a6f2:	e131                	c.bnez	a0,2005a736 <cpu_stat_thread+0x62>
2005a6f4:	200635b7          	lui	a1,0x20063
2005a6f8:	20063537          	lui	a0,0x20063
2005a6fc:	f5458593          	addi	a1,a1,-172 # 20062f54 <__FUNCTION__.0>
2005a700:	c9c50513          	addi	a0,a0,-868 # 20062c9c <pmap_func+0x870>
2005a704:	4a3040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a708:	4088                	c.lw	a0,0(s1)
2005a70a:	ebc18413          	addi	s0,gp,-324 # 20066844 <task_status>
2005a70e:	1fe010ef          	jal	ra,2005b90c <rtos_sema_delete>
2005a712:	4008                	c.lw	a0,0(s0)
2005a714:	0004a023          	sw	zero,0(s1)
2005a718:	006010ef          	jal	ra,2005b71e <rtos_mem_free>
2005a71c:	00042023          	sw	zero,0(s0)
2005a720:	5422                	c.lwsp	s0,40(sp)
2005a722:	50b2                	c.lwsp	ra,44(sp)
2005a724:	5492                	c.lwsp	s1,36(sp)
2005a726:	5902                	c.lwsp	s2,32(sp)
2005a728:	49f2                	c.lwsp	s3,28(sp)
2005a72a:	4a62                	c.lwsp	s4,24(sp)
2005a72c:	4ad2                	c.lwsp	s5,20(sp)
2005a72e:	4501                	c.li	a0,0
2005a730:	6145                	c.addi16sp	sp,48
2005a732:	4b40106f          	jal	zero,2005bbe6 <rtos_task_delete>
2005a736:	4621                	c.li	a2,8
2005a738:	842a                	c.mv	s0,a0
2005a73a:	dffaf097          	auipc	ra,0xdffaf
2005a73e:	396080e7          	jalr	ra,918(ra) # 9ad0 <__wrap_memcpy>
2005a742:	410187b7          	lui	a5,0x41018
2005a746:	4bdc                	c.lw	a5,20(a5)
2005a748:	3e800a93          	addi	s5,zero,1000
2005a74c:	eaf1a823          	sw	a5,-336(gp) # 20066838 <last_tick>
2005a750:	5a7d                	c.li	s4,-1
2005a752:	eb018913          	addi	s2,gp,-336 # 20066838 <last_tick>
2005a756:	410189b7          	lui	s3,0x41018
2005a75a:	400c                	c.lw	a1,0(s0)
2005a75c:	4088                	c.lw	a0,0(s1)
2005a75e:	02ba85b3          	mul	a1,s5,a1
2005a762:	1c0010ef          	jal	ra,2005b922 <rtos_sema_take>
2005a766:	03451463          	bne	a0,s4,2005a78e <cpu_stat_thread+0xba>
2005a76a:	39e5                	c.jal	2005a462 <update_status>
2005a76c:	0149a503          	lw	a0,20(s3) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a770:	00092783          	lw	a5,0(s2)
2005a774:	0149a703          	lw	a4,20(s3)
2005a778:	8d1d                	c.sub	a0,a5
2005a77a:	00e92023          	sw	a4,0(s2)
2005a77e:	353d                	c.jal	2005a5ac <print_delta>
2005a780:	405c                	c.lw	a5,4(s0)
2005a782:	00f05563          	bge	zero,a5,2005a78c <cpu_stat_thread+0xb8>
2005a786:	17fd                	c.addi	a5,-1
2005a788:	c05c                	c.sw	a5,4(s0)
2005a78a:	bfc1                	c.j	2005a75a <cpu_stat_thread+0x86>
2005a78c:	f7f9                	c.bnez	a5,2005a75a <cpu_stat_thread+0x86>
2005a78e:	8522                	c.mv	a0,s0
2005a790:	78f000ef          	jal	ra,2005b71e <rtos_mem_free>
2005a794:	bf95                	c.j	2005a708 <cpu_stat_thread+0x34>

2005a796 <at_cpuload>:
2005a796:	7119                	c.addi16sp	sp,-128
2005a798:	dca2                	c.swsp	s0,120(sp)
2005a79a:	05c00613          	addi	a2,zero,92
2005a79e:	842a                	c.mv	s0,a0
2005a7a0:	4581                	c.li	a1,0
2005a7a2:	0048                	c.addi4spn	a0,sp,4
2005a7a4:	de86                	c.swsp	ra,124(sp)
2005a7a6:	d8ca                	c.swsp	s2,112(sp)
2005a7a8:	d4d2                	c.swsp	s4,104(sp)
2005a7aa:	daa6                	c.swsp	s1,116(sp)
2005a7ac:	d6ce                	c.swsp	s3,108(sp)
2005a7ae:	d2d6                	c.swsp	s5,100(sp)
2005a7b0:	dffaf097          	auipc	ra,0xdffaf
2005a7b4:	31e080e7          	jalr	ra,798(ra) # 9ace <__wrap_memset>
2005a7b8:	eb418793          	addi	a5,gp,-332 # 2006683c <para_in>
2005a7bc:	4705                	c.li	a4,1
2005a7be:	c398                	c.sw	a4,0(a5)
2005a7c0:	004c                	c.addi4spn	a1,sp,4
2005a7c2:	577d                	c.li	a4,-1
2005a7c4:	8522                	c.mv	a0,s0
2005a7c6:	c3d8                	c.sw	a4,4(a5)
2005a7c8:	ea6ff0ef          	jal	ra,20059e6e <parse_param>
2005a7cc:	ffe50713          	addi	a4,a0,-2
2005a7d0:	4789                	c.li	a5,2
2005a7d2:	20063a37          	lui	s4,0x20063
2005a7d6:	06e7f463          	bgeu	a5,a4,2005a83e <at_cpuload+0xa8>
2005a7da:	20063537          	lui	a0,0x20063
2005a7de:	cb050513          	addi	a0,a0,-848 # 20062cb0 <pmap_func+0x884>
2005a7e2:	3c5040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a7e6:	4405                	c.li	s0,1
2005a7e8:	20063537          	lui	a0,0x20063
2005a7ec:	8622                	c.mv	a2,s0
2005a7ee:	e3ca0593          	addi	a1,s4,-452 # 20062e3c <pmap_func+0xa10>
2005a7f2:	b3050513          	addi	a0,a0,-1232 # 20062b30 <pmap_func+0x704>
2005a7f6:	9875                	c.andi	s0,-3
2005a7f8:	3af040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a7fc:	14040b63          	beq	s0,zero,2005a952 <at_cpuload+0x1bc>
2005a800:	20063537          	lui	a0,0x20063
2005a804:	8d850513          	addi	a0,a0,-1832 # 200628d8 <pmap_func+0x4ac>
2005a808:	39f040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a80c:	20063537          	lui	a0,0x20063
2005a810:	e4850513          	addi	a0,a0,-440 # 20062e48 <pmap_func+0xa1c>
2005a814:	393040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a818:	20063537          	lui	a0,0x20063
2005a81c:	e7850513          	addi	a0,a0,-392 # 20062e78 <pmap_func+0xa4c>
2005a820:	387040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a824:	20063537          	lui	a0,0x20063
2005a828:	e8c50513          	addi	a0,a0,-372 # 20062e8c <pmap_func+0xa60>
2005a82c:	37b040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a830:	20063537          	lui	a0,0x20063
2005a834:	ea850513          	addi	a0,a0,-344 # 20062ea8 <pmap_func+0xa7c>
2005a838:	36f040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a83c:	aa19                	c.j	2005a952 <at_cpuload+0x1bc>
2005a83e:	84aa                	c.mv	s1,a0
2005a840:	4522                	c.lwsp	a0,8(sp)
2005a842:	eb418993          	addi	s3,gp,-332 # 2006683c <para_in>
2005a846:	4411                	c.li	s0,4
2005a848:	00054783          	lbu	a5,0(a0)
2005a84c:	c789                	c.beqz	a5,2005a856 <at_cpuload+0xc0>
2005a84e:	348070ef          	jal	ra,20061b96 <atoi>
2005a852:	0ff57413          	andi	s0,a0,255
2005a856:	4789                	c.li	a5,2
2005a858:	04f48463          	beq	s1,a5,2005a8a0 <at_cpuload+0x10a>
2005a85c:	4532                	c.lwsp	a0,12(sp)
2005a85e:	00054783          	lbu	a5,0(a0)
2005a862:	cf81                	c.beqz	a5,2005a87a <at_cpuload+0xe4>
2005a864:	332070ef          	jal	ra,20061b96 <atoi>
2005a868:	4785                	c.li	a5,1
2005a86a:	00a05663          	bge	zero,a0,2005a876 <at_cpuload+0xe0>
2005a86e:	4532                	c.lwsp	a0,12(sp)
2005a870:	326070ef          	jal	ra,20061b96 <atoi>
2005a874:	87aa                	c.mv	a5,a0
2005a876:	00f9a023          	sw	a5,0(s3)
2005a87a:	4791                	c.li	a5,4
2005a87c:	02f49263          	bne	s1,a5,2005a8a0 <at_cpuload+0x10a>
2005a880:	4542                	c.lwsp	a0,16(sp)
2005a882:	00054783          	lbu	a5,0(a0)
2005a886:	cf89                	c.beqz	a5,2005a8a0 <at_cpuload+0x10a>
2005a888:	30e070ef          	jal	ra,20061b96 <atoi>
2005a88c:	57fd                	c.li	a5,-1
2005a88e:	00a05763          	bge	zero,a0,2005a89c <at_cpuload+0x106>
2005a892:	4542                	c.lwsp	a0,16(sp)
2005a894:	302070ef          	jal	ra,20061b96 <atoi>
2005a898:	fff50793          	addi	a5,a0,-1
2005a89c:	00f9a223          	sw	a5,4(s3)
2005a8a0:	ebc18a93          	addi	s5,gp,-324 # 20066844 <task_status>
2005a8a4:	000aa783          	lw	a5,0(s5)
2005a8a8:	ebc18493          	addi	s1,gp,-324 # 20066844 <task_status>
2005a8ac:	e79d                	c.bnez	a5,2005a8da <at_cpuload+0x144>
2005a8ae:	6509                	c.lui	a0,0x2
2005a8b0:	0541                	c.addi	a0,16
2005a8b2:	643000ef          	jal	ra,2005b6f4 <rtos_mem_malloc>
2005a8b6:	00aaa023          	sw	a0,0(s5)
2005a8ba:	e105                	c.bnez	a0,2005a8da <at_cpuload+0x144>
2005a8bc:	200636b7          	lui	a3,0x20063
2005a8c0:	200635b7          	lui	a1,0x20063
2005a8c4:	dcc68693          	addi	a3,a3,-564 # 20062dcc <pmap_func+0x9a0>
2005a8c8:	04900613          	addi	a2,zero,73
2005a8cc:	9f058593          	addi	a1,a1,-1552 # 200629f0 <pmap_func+0x5c4>
2005a8d0:	4511                	c.li	a0,4
2005a8d2:	0bd000ef          	jal	ra,2005b18e <rtk_log_write>
2005a8d6:	4409                	c.li	s0,2
2005a8d8:	bf01                	c.j	2005a7e8 <at_cpuload+0x52>
2005a8da:	0049a683          	lw	a3,4(s3)
2005a8de:	0009a603          	lw	a2,0(s3)
2005a8e2:	20063537          	lui	a0,0x20063
2005a8e6:	85a2                	c.mv	a1,s0
2005a8e8:	df050513          	addi	a0,a0,-528 # 20062df0 <pmap_func+0x9c4>
2005a8ec:	2bb040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a8f0:	4789                	c.li	a5,2
2005a8f2:	08f40363          	beq	s0,a5,2005a978 <at_cpuload+0x1e2>
2005a8f6:	0687e763          	bltu	a5,s0,2005a964 <at_cpuload+0x1ce>
2005a8fa:	ec018793          	addi	a5,gp,-320 # 20066848 <top_exit_sema>
2005a8fe:	e825                	c.bnez	s0,2005a96e <at_cpuload+0x1d8>
2005a900:	439c                	c.lw	a5,0(a5)
2005a902:	e3a1                	c.bnez	a5,2005a942 <at_cpuload+0x1ac>
2005a904:	4088                	c.lw	a0,0(s1)
2005a906:	6609                	c.lui	a2,0x2
2005a908:	0641                	c.addi	a2,16
2005a90a:	4581                	c.li	a1,0
2005a90c:	dffaf097          	auipc	ra,0xdffaf
2005a910:	1c2080e7          	jalr	ra,450(ra) # 9ace <__wrap_memset>
2005a914:	b4fff0ef          	jal	ra,2005a462 <update_status>
2005a918:	4605                	c.li	a2,1
2005a91a:	4581                	c.li	a1,0
2005a91c:	ec018513          	addi	a0,gp,-320 # 20066848 <top_exit_sema>
2005a920:	761000ef          	jal	ra,2005b880 <rtos_sema_create>
2005a924:	2005a637          	lui	a2,0x2005a
2005a928:	200635b7          	lui	a1,0x20063
2005a92c:	47a9                	c.li	a5,10
2005a92e:	6711                	c.lui	a4,0x4
2005a930:	eb418693          	addi	a3,gp,-332 # 2006683c <para_in>
2005a934:	6d460613          	addi	a2,a2,1748 # 2005a6d4 <cpu_stat_thread>
2005a938:	e2c58593          	addi	a1,a1,-468 # 20062e2c <pmap_func+0xa00>
2005a93c:	4501                	c.li	a0,0
2005a93e:	284010ef          	jal	ra,2005bbc2 <rtos_task_create>
2005a942:	20063537          	lui	a0,0x20063
2005a946:	e3ca0593          	addi	a1,s4,-452
2005a94a:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
2005a94e:	259040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a952:	50f6                	c.lwsp	ra,124(sp)
2005a954:	5466                	c.lwsp	s0,120(sp)
2005a956:	54d6                	c.lwsp	s1,116(sp)
2005a958:	5946                	c.lwsp	s2,112(sp)
2005a95a:	59b6                	c.lwsp	s3,108(sp)
2005a95c:	5a26                	c.lwsp	s4,104(sp)
2005a95e:	5a96                	c.lwsp	s5,100(sp)
2005a960:	6109                	c.addi16sp	sp,128
2005a962:	8082                	c.jr	ra
2005a964:	478d                	c.li	a5,3
2005a966:	02f40c63          	beq	s0,a5,2005a99e <at_cpuload+0x208>
2005a96a:	440d                	c.li	s0,3
2005a96c:	bdb5                	c.j	2005a7e8 <at_cpuload+0x52>
2005a96e:	4388                	c.lw	a0,0(a5)
2005a970:	d969                	c.beqz	a0,2005a942 <at_cpuload+0x1ac>
2005a972:	7fd000ef          	jal	ra,2005b96e <rtos_sema_give>
2005a976:	b7f1                	c.j	2005a942 <at_cpuload+0x1ac>
2005a978:	ec01a783          	lw	a5,-320(gp) # 20066848 <top_exit_sema>
2005a97c:	f3f9                	c.bnez	a5,2005a942 <at_cpuload+0x1ac>
2005a97e:	4088                	c.lw	a0,0(s1)
2005a980:	6609                	c.lui	a2,0x2
2005a982:	0641                	c.addi	a2,16
2005a984:	4581                	c.li	a1,0
2005a986:	dffaf097          	auipc	ra,0xdffaf
2005a98a:	148080e7          	jalr	ra,328(ra) # 9ace <__wrap_memset>
2005a98e:	410187b7          	lui	a5,0x41018
2005a992:	4bd8                	c.lw	a4,20(a5)
2005a994:	eae1a823          	sw	a4,-336(gp) # 20066838 <last_tick>
2005a998:	acbff0ef          	jal	ra,2005a462 <update_status>
2005a99c:	b75d                	c.j	2005a942 <at_cpuload+0x1ac>
2005a99e:	ec01a783          	lw	a5,-320(gp) # 20066848 <top_exit_sema>
2005a9a2:	f3c5                	c.bnez	a5,2005a942 <at_cpuload+0x1ac>
2005a9a4:	abfff0ef          	jal	ra,2005a462 <update_status>
2005a9a8:	41018937          	lui	s2,0x41018
2005a9ac:	eb018413          	addi	s0,gp,-336 # 20066838 <last_tick>
2005a9b0:	401c                	c.lw	a5,0(s0)
2005a9b2:	01492503          	lw	a0,20(s2) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a9b6:	8d1d                	c.sub	a0,a5
2005a9b8:	bf5ff0ef          	jal	ra,2005a5ac <print_delta>
2005a9bc:	01492783          	lw	a5,20(s2)
2005a9c0:	4088                	c.lw	a0,0(s1)
2005a9c2:	c01c                	c.sw	a5,0(s0)
2005a9c4:	55b000ef          	jal	ra,2005b71e <rtos_mem_free>
2005a9c8:	0004a023          	sw	zero,0(s1)
2005a9cc:	bf9d                	c.j	2005a942 <at_cpuload+0x1ac>

2005a9ce <print_system_at>:
2005a9ce:	1141                	c.addi	sp,-16
2005a9d0:	c422                	c.swsp	s0,8(sp)
2005a9d2:	c9c18413          	addi	s0,gp,-868 # 20066624 <at_sys_items>
2005a9d6:	c226                	c.swsp	s1,4(sp)
2005a9d8:	c04a                	c.swsp	s2,0(sp)
2005a9da:	c606                	c.swsp	ra,12(sp)
2005a9dc:	0b040493          	addi	s1,s0,176
2005a9e0:	20063937          	lui	s2,0x20063
2005a9e4:	400c                	c.lw	a1,0(s0)
2005a9e6:	ec890513          	addi	a0,s2,-312 # 20062ec8 <pmap_func+0xa9c>
2005a9ea:	0441                	c.addi	s0,16
2005a9ec:	1bb040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005a9f0:	fe941ae3          	bne	s0,s1,2005a9e4 <print_system_at+0x16>
2005a9f4:	40b2                	c.lwsp	ra,12(sp)
2005a9f6:	4422                	c.lwsp	s0,8(sp)
2005a9f8:	4492                	c.lwsp	s1,4(sp)
2005a9fa:	4902                	c.lwsp	s2,0(sp)
2005a9fc:	0141                	c.addi	sp,16
2005a9fe:	8082                	c.jr	ra

2005aa00 <at_list>:
2005aa00:	20063537          	lui	a0,0x20063
2005aa04:	1141                	c.addi	sp,-16
2005aa06:	ed050513          	addi	a0,a0,-304 # 20062ed0 <pmap_func+0xaa4>
2005aa0a:	c606                	c.swsp	ra,12(sp)
2005aa0c:	19b040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005aa10:	3f7d                	c.jal	2005a9ce <print_system_at>
2005aa12:	40b2                	c.lwsp	ra,12(sp)
2005aa14:	200635b7          	lui	a1,0x20063
2005aa18:	20063537          	lui	a0,0x20063
2005aa1c:	ee858593          	addi	a1,a1,-280 # 20062ee8 <pmap_func+0xabc>
2005aa20:	8ac50513          	addi	a0,a0,-1876 # 200628ac <pmap_func+0x480>
2005aa24:	0141                	c.addi	sp,16
2005aa26:	1810406f          	jal	zero,2005f3a6 <DiagPrintf_minimal>

2005aa2a <at_sys_init>:
2005aa2a:	45ad                	c.li	a1,11
2005aa2c:	c9c18513          	addi	a0,gp,-868 # 20066624 <at_sys_items>
2005aa30:	b38ff06f          	jal	zero,20059d68 <atcmd_service_add_table>

2005aa34 <BOOT_Image1>:
2005aa34:	20000537          	lui	a0,0x20000
2005aa38:	200017b7          	lui	a5,0x20001
2005aa3c:	cac78793          	addi	a5,a5,-852 # 20000cac <__rom_bss_end__>
2005aa40:	00050613          	addi	a2,a0,0 # 20000000 <UserIrqFunTable>
2005aa44:	1141                	c.addi	sp,-16
2005aa46:	40c78633          	sub	a2,a5,a2
2005aa4a:	4581                	c.li	a1,0
2005aa4c:	00050513          	addi	a0,a0,0
2005aa50:	c606                	c.swsp	ra,12(sp)
2005aa52:	dffaf097          	auipc	ra,0xdffaf
2005aa56:	d38080e7          	jalr	ra,-712(ra) # 978a <_memset>
2005aa5a:	dffa6097          	auipc	ra,0xdffa6
2005aa5e:	0bc080e7          	jalr	ra,188(ra) # b16 <CPU_ClkGet>
2005aa62:	000f47b7          	lui	a5,0xf4
2005aa66:	24078793          	addi	a5,a5,576 # f4240 <__ap_sram_heap_size+0xb4240>
2005aa6a:	02f55533          	divu	a0,a0,a5
2005aa6e:	200017b7          	lui	a5,0x20001
2005aa72:	86a7a023          	sw	a0,-1952(a5) # 20000860 <RBSS_UDELAY_CLK>
2005aa76:	dffa6097          	auipc	ra,0xdffa6
2005aa7a:	eae080e7          	jalr	ra,-338(ra) # 924 <BOOT_ROM_InitDebugFlg>
2005aa7e:	200587b7          	lui	a5,0x20058
2005aa82:	04078793          	addi	a5,a5,64 # 20058040 <Img2EntryFun0>
2005aa86:	0017c703          	lbu	a4,1(a5)
2005aa8a:	0007c683          	lbu	a3,0(a5)
2005aa8e:	40b2                	c.lwsp	ra,12(sp)
2005aa90:	0722                	c.slli	a4,0x8
2005aa92:	8ed9                	c.or	a3,a4
2005aa94:	0027c703          	lbu	a4,2(a5)
2005aa98:	0037c783          	lbu	a5,3(a5)
2005aa9c:	0742                	c.slli	a4,0x10
2005aa9e:	8f55                	c.or	a4,a3
2005aaa0:	07e2                	c.slli	a5,0x18
2005aaa2:	8fd9                	c.or	a5,a4
2005aaa4:	0141                	c.addi	sp,16
2005aaa6:	8782                	c.jr	a5

2005aaa8 <flash_read_word>:
2005aaa8:	1141                	c.addi	sp,-16
2005aaaa:	852e                	c.mv	a0,a1
2005aaac:	4591                	c.li	a1,4
2005aaae:	c606                	c.swsp	ra,12(sp)
2005aab0:	8c2fe0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005aab4:	40b2                	c.lwsp	ra,12(sp)
2005aab6:	4505                	c.li	a0,1
2005aab8:	0141                	c.addi	sp,16
2005aaba:	8082                	c.jr	ra

2005aabc <os_heap_init>:
2005aabc:	1141                	c.addi	sp,-16
2005aabe:	c606                	c.swsp	ra,12(sp)
2005aac0:	c422                	c.swsp	s0,8(sp)
2005aac2:	0800                	c.addi4spn	s0,sp,16
2005aac4:	00000793          	addi	a5,zero,0
2005aac8:	85be                	c.mv	a1,a5
2005aaca:	ca0007b7          	lui	a5,0xca000
2005aace:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005aad2:	2869                	c.jal	2005ab6c <os_heap_add>
2005aad4:	67dd                	c.lui	a5,0x17
2005aad6:	50078793          	addi	a5,a5,1280 # 17500 <__bdram_heap_buffer_size__>
2005aada:	85be                	c.mv	a1,a5
2005aadc:	200697b7          	lui	a5,0x20069
2005aae0:	b0078513          	addi	a0,a5,-1280 # 20068b00 <__bdram_heap_buffer_start__>
2005aae4:	2061                	c.jal	2005ab6c <os_heap_add>
2005aae6:	00000793          	addi	a5,zero,0
2005aaea:	85be                	c.mv	a1,a5
2005aaec:	ca0007b7          	lui	a5,0xca000
2005aaf0:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005aaf4:	28a5                	c.jal	2005ab6c <os_heap_add>
2005aaf6:	00000793          	addi	a5,zero,0
2005aafa:	85be                	c.mv	a1,a5
2005aafc:	ca0007b7          	lui	a5,0xca000
2005ab00:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005ab04:	20a5                	c.jal	2005ab6c <os_heap_add>
2005ab06:	67a1                	c.lui	a5,0x8
2005ab08:	00078793          	addi	a5,a5,0 # 8000 <__sram_bootloader_mp_size__>
2005ab0c:	85be                	c.mv	a1,a5
2005ab0e:	200127b7          	lui	a5,0x20012
2005ab12:	00078513          	addi	a0,a5,0 # 20012000 <__sram_bootloader_mp_start__>
2005ab16:	2899                	c.jal	2005ab6c <os_heap_add>
2005ab18:	ec41c783          	lbu	a5,-316(gp) # 2006684c <heap_index>
2005ab1c:	c391                	c.beqz	a5,2005ab20 <os_heap_init+0x64>
2005ab1e:	2809                	c.jal	2005ab30 <os_heap_sort>
2005ab20:	57c18513          	addi	a0,gp,1404 # 20066f04 <xHeapRegions>
2005ab24:	2481                	c.jal	2005ad64 <vPortDefineHeapRegions>
2005ab26:	0001                	c.addi	zero,0
2005ab28:	40b2                	c.lwsp	ra,12(sp)
2005ab2a:	4422                	c.lwsp	s0,8(sp)
2005ab2c:	0141                	c.addi	sp,16
2005ab2e:	8082                	c.jr	ra

2005ab30 <os_heap_sort>:
2005ab30:	4681                	c.li	a3,0
2005ab32:	ec41c703          	lbu	a4,-316(gp) # 2006684c <heap_index>
2005ab36:	177d                	c.addi	a4,-1
2005ab38:	02e6d963          	bge	a3,a4,2005ab6a <os_heap_sort+0x3a>
2005ab3c:	57c18793          	addi	a5,gp,1404 # 20066f04 <xHeapRegions>
2005ab40:	4601                	c.li	a2,0
2005ab42:	8f15                	c.sub	a4,a3
2005ab44:	a839                	c.j	2005ab62 <os_heap_sort+0x32>
2005ab46:	438c                	c.lw	a1,0(a5)
2005ab48:	4788                	c.lw	a0,8(a5)
2005ab4a:	0605                	c.addi	a2,1
2005ab4c:	00b57a63          	bgeu	a0,a1,2005ab60 <os_heap_sort+0x30>
2005ab50:	0047a303          	lw	t1,4(a5)
2005ab54:	c388                	c.sw	a0,0(a5)
2005ab56:	47c8                	c.lw	a0,12(a5)
2005ab58:	c78c                	c.sw	a1,8(a5)
2005ab5a:	0067a623          	sw	t1,12(a5)
2005ab5e:	c3c8                	c.sw	a0,4(a5)
2005ab60:	07a1                	c.addi	a5,8
2005ab62:	fee642e3          	blt	a2,a4,2005ab46 <os_heap_sort+0x16>
2005ab66:	0685                	c.addi	a3,1
2005ab68:	b7e9                	c.j	2005ab32 <os_heap_sort+0x2>
2005ab6a:	8082                	c.jr	ra

2005ab6c <os_heap_add>:
2005ab6c:	1141                	c.addi	sp,-16
2005ab6e:	c426                	c.swsp	s1,8(sp)
2005ab70:	ec41c703          	lbu	a4,-316(gp) # 2006684c <heap_index>
2005ab74:	c24a                	c.swsp	s2,4(sp)
2005ab76:	c04e                	c.swsp	s3,0(sp)
2005ab78:	c606                	c.swsp	ra,12(sp)
2005ab7a:	4795                	c.li	a5,5
2005ab7c:	89aa                	c.mv	s3,a0
2005ab7e:	892e                	c.mv	s2,a1
2005ab80:	00e7fa63          	bgeu	a5,a4,2005ab94 <os_heap_add+0x28>
2005ab84:	20063537          	lui	a0,0x20063
2005ab88:	02600593          	addi	a1,zero,38
2005ab8c:	f6450513          	addi	a0,a0,-156 # 20062f64 <__FUNCTION__.0+0x10>
2005ab90:	51d040ef          	jal	ra,2005f8ac <io_assert_failed>
2005ab94:	ec41c703          	lbu	a4,-316(gp) # 2006684c <heap_index>
2005ab98:	57c18793          	addi	a5,gp,1404 # 20066f04 <xHeapRegions>
2005ab9c:	00371693          	slli	a3,a4,0x3
2005aba0:	97b6                	c.add	a5,a3
2005aba2:	0137a023          	sw	s3,0(a5)
2005aba6:	0127a223          	sw	s2,4(a5)
2005abaa:	4501                	c.li	a0,0
2005abac:	00090c63          	beq	s2,zero,2005abc4 <os_heap_add+0x58>
2005abb0:	ec818793          	addi	a5,gp,-312 # 20066850 <total_heap_size>
2005abb4:	438c                	c.lw	a1,0(a5)
2005abb6:	0705                	c.addi	a4,1
2005abb8:	ece18223          	sb	a4,-316(gp) # 2006684c <heap_index>
2005abbc:	992e                	c.add	s2,a1
2005abbe:	0127a023          	sw	s2,0(a5)
2005abc2:	4505                	c.li	a0,1
2005abc4:	40b2                	c.lwsp	ra,12(sp)
2005abc6:	44a2                	c.lwsp	s1,8(sp)
2005abc8:	4912                	c.lwsp	s2,4(sp)
2005abca:	4982                	c.lwsp	s3,0(sp)
2005abcc:	0141                	c.addi	sp,16
2005abce:	8082                	c.jr	ra

2005abd0 <__wrap_malloc>:
2005abd0:	3250006f          	jal	zero,2005b6f4 <rtos_mem_malloc>

2005abd4 <__wrap_free>:
2005abd4:	34b0006f          	jal	zero,2005b71e <rtos_mem_free>

2005abd8 <__wrap__free_r>:
2005abd8:	852e                	c.mv	a0,a1
2005abda:	3450006f          	jal	zero,2005b71e <rtos_mem_free>

2005abde <prvInsertBlockIntoFreeList>:
2005abde:	ee418793          	addi	a5,gp,-284 # 2006686c <xStart>
2005abe2:	873e                	c.mv	a4,a5
2005abe4:	439c                	c.lw	a5,0(a5)
2005abe6:	fea7eee3          	bltu	a5,a0,2005abe2 <prvInsertBlockIntoFreeList+0x4>
2005abea:	4350                	c.lw	a2,4(a4)
2005abec:	00c706b3          	add	a3,a4,a2
2005abf0:	00d51663          	bne	a0,a3,2005abfc <prvInsertBlockIntoFreeList+0x1e>
2005abf4:	4154                	c.lw	a3,4(a0)
2005abf6:	853a                	c.mv	a0,a4
2005abf8:	96b2                	c.add	a3,a2
2005abfa:	c354                	c.sw	a3,4(a4)
2005abfc:	4154                	c.lw	a3,4(a0)
2005abfe:	00d50633          	add	a2,a0,a3
2005ac02:	00c79b63          	bne	a5,a2,2005ac18 <prvInsertBlockIntoFreeList+0x3a>
2005ac06:	ecc1a603          	lw	a2,-308(gp) # 20066854 <pxEnd>
2005ac0a:	00c78763          	beq	a5,a2,2005ac18 <prvInsertBlockIntoFreeList+0x3a>
2005ac0e:	43dc                	c.lw	a5,4(a5)
2005ac10:	97b6                	c.add	a5,a3
2005ac12:	c15c                	c.sw	a5,4(a0)
2005ac14:	431c                	c.lw	a5,0(a4)
2005ac16:	439c                	c.lw	a5,0(a5)
2005ac18:	c11c                	c.sw	a5,0(a0)
2005ac1a:	00e50363          	beq	a0,a4,2005ac20 <prvInsertBlockIntoFreeList+0x42>
2005ac1e:	c308                	c.sw	a0,0(a4)
2005ac20:	8082                	c.jr	ra

2005ac22 <pvPortMalloc>:
2005ac22:	1101                	c.addi	sp,-32
2005ac24:	cc22                	c.swsp	s0,24(sp)
2005ac26:	c452                	c.swsp	s4,8(sp)
2005ac28:	842a                	c.mv	s0,a0
2005ac2a:	ce06                	c.swsp	ra,28(sp)
2005ac2c:	ca26                	c.swsp	s1,20(sp)
2005ac2e:	c84a                	c.swsp	s2,16(sp)
2005ac30:	c64e                	c.swsp	s3,12(sp)
2005ac32:	2c3020ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005ac36:	ed01aa03          	lw	s4,-304(gp) # 20066858 <xBlockAllocatedBit>
2005ac3a:	008a77b3          	and	a5,s4,s0
2005ac3e:	c799                	c.beqz	a5,2005ac4c <pvPortMalloc+0x2a>
2005ac40:	362030ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005ac44:	1b5010ef          	jal	ra,2005c5f8 <vApplicationMallocFailedHook>
2005ac48:	4901                	c.li	s2,0
2005ac4a:	a879                	c.j	2005ace8 <pvPortMalloc+0xc6>
2005ac4c:	fff40793          	addi	a5,s0,-1
2005ac50:	fde00713          	addi	a4,zero,-34
2005ac54:	fef766e3          	bltu	a4,a5,2005ac40 <pvPortMalloc+0x1e>
2005ac58:	02040793          	addi	a5,s0,32
2005ac5c:	887d                	c.andi	s0,31
2005ac5e:	c801                	c.beqz	s0,2005ac6e <pvPortMalloc+0x4c>
2005ac60:	fe07f713          	andi	a4,a5,-32
2005ac64:	02070713          	addi	a4,a4,32 # 4020 <GPIO_Direction+0x56>
2005ac68:	fce7fce3          	bgeu	a5,a4,2005ac40 <pvPortMalloc+0x1e>
2005ac6c:	87ba                	c.mv	a5,a4
2005ac6e:	ed41a483          	lw	s1,-300(gp) # 2006685c <xFreeBytesRemaining>
2005ac72:	ed418993          	addi	s3,gp,-300 # 2006685c <xFreeBytesRemaining>
2005ac76:	fcf4e5e3          	bltu	s1,a5,2005ac40 <pvPortMalloc+0x1e>
2005ac7a:	ee41a403          	lw	s0,-284(gp) # 2006686c <xStart>
2005ac7e:	ee418713          	addi	a4,gp,-284 # 2006686c <xStart>
2005ac82:	4054                	c.lw	a3,4(s0)
2005ac84:	00f6f463          	bgeu	a3,a5,2005ac8c <pvPortMalloc+0x6a>
2005ac88:	4014                	c.lw	a3,0(s0)
2005ac8a:	eaa5                	c.bnez	a3,2005acfa <pvPortMalloc+0xd8>
2005ac8c:	ecc1a683          	lw	a3,-308(gp) # 20066854 <pxEnd>
2005ac90:	fa8688e3          	beq	a3,s0,2005ac40 <pvPortMalloc+0x1e>
2005ac94:	4014                	c.lw	a3,0(s0)
2005ac96:	00072903          	lw	s2,0(a4)
2005ac9a:	c314                	c.sw	a3,0(a4)
2005ac9c:	4054                	c.lw	a3,4(s0)
2005ac9e:	04000713          	addi	a4,zero,64
2005aca2:	02090913          	addi	s2,s2,32
2005aca6:	8e9d                	c.sub	a3,a5
2005aca8:	00d77763          	bgeu	a4,a3,2005acb6 <pvPortMalloc+0x94>
2005acac:	00f40533          	add	a0,s0,a5
2005acb0:	c154                	c.sw	a3,4(a0)
2005acb2:	c05c                	c.sw	a5,4(s0)
2005acb4:	372d                	c.jal	2005abde <prvInsertBlockIntoFreeList>
2005acb6:	4058                	c.lw	a4,4(s0)
2005acb8:	ed818693          	addi	a3,gp,-296 # 20066860 <xMinimumEverFreeBytesRemaining>
2005acbc:	4290                	c.lw	a2,0(a3)
2005acbe:	40e487b3          	sub	a5,s1,a4
2005acc2:	00f9a023          	sw	a5,0(s3)
2005acc6:	00c7f363          	bgeu	a5,a2,2005accc <pvPortMalloc+0xaa>
2005acca:	c29c                	c.sw	a5,0(a3)
2005accc:	00ea6733          	or	a4,s4,a4
2005acd0:	edc18793          	addi	a5,gp,-292 # 20066864 <xNumberOfSuccessfulAllocations>
2005acd4:	c058                	c.sw	a4,4(s0)
2005acd6:	4398                	c.lw	a4,0(a5)
2005acd8:	00042023          	sw	zero,0(s0)
2005acdc:	0705                	c.addi	a4,1
2005acde:	c398                	c.sw	a4,0(a5)
2005ace0:	2c2030ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005ace4:	f60900e3          	beq	s2,zero,2005ac44 <pvPortMalloc+0x22>
2005ace8:	40f2                	c.lwsp	ra,28(sp)
2005acea:	4462                	c.lwsp	s0,24(sp)
2005acec:	44d2                	c.lwsp	s1,20(sp)
2005acee:	49b2                	c.lwsp	s3,12(sp)
2005acf0:	4a22                	c.lwsp	s4,8(sp)
2005acf2:	854a                	c.mv	a0,s2
2005acf4:	4942                	c.lwsp	s2,16(sp)
2005acf6:	6105                	c.addi16sp	sp,32
2005acf8:	8082                	c.jr	ra
2005acfa:	8722                	c.mv	a4,s0
2005acfc:	8436                	c.mv	s0,a3
2005acfe:	b751                	c.j	2005ac82 <pvPortMalloc+0x60>

2005ad00 <vPortFree>:
2005ad00:	cd31                	c.beqz	a0,2005ad5c <vPortFree+0x5c>
2005ad02:	1141                	c.addi	sp,-16
2005ad04:	c422                	c.swsp	s0,8(sp)
2005ad06:	c606                	c.swsp	ra,12(sp)
2005ad08:	fe452703          	lw	a4,-28(a0)
2005ad0c:	ed01a783          	lw	a5,-304(gp) # 20066858 <xBlockAllocatedBit>
2005ad10:	842a                	c.mv	s0,a0
2005ad12:	00f776b3          	and	a3,a4,a5
2005ad16:	ce9d                	c.beqz	a3,2005ad54 <vPortFree+0x54>
2005ad18:	fe052683          	lw	a3,-32(a0)
2005ad1c:	ee85                	c.bnez	a3,2005ad54 <vPortFree+0x54>
2005ad1e:	fff7c793          	xori	a5,a5,-1
2005ad22:	8ff9                	c.and	a5,a4
2005ad24:	fef52223          	sw	a5,-28(a0)
2005ad28:	1cd020ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005ad2c:	ed418793          	addi	a5,gp,-300 # 2006685c <xFreeBytesRemaining>
2005ad30:	4394                	c.lw	a3,0(a5)
2005ad32:	fe442703          	lw	a4,-28(s0)
2005ad36:	fe040513          	addi	a0,s0,-32
2005ad3a:	9736                	c.add	a4,a3
2005ad3c:	c398                	c.sw	a4,0(a5)
2005ad3e:	3545                	c.jal	2005abde <prvInsertBlockIntoFreeList>
2005ad40:	ee018793          	addi	a5,gp,-288 # 20066868 <xNumberOfSuccessfulFrees>
2005ad44:	4398                	c.lw	a4,0(a5)
2005ad46:	0705                	c.addi	a4,1
2005ad48:	c398                	c.sw	a4,0(a5)
2005ad4a:	4422                	c.lwsp	s0,8(sp)
2005ad4c:	40b2                	c.lwsp	ra,12(sp)
2005ad4e:	0141                	c.addi	sp,16
2005ad50:	2520306f          	jal	zero,2005dfa2 <xTaskResumeAll>
2005ad54:	40b2                	c.lwsp	ra,12(sp)
2005ad56:	4422                	c.lwsp	s0,8(sp)
2005ad58:	0141                	c.addi	sp,16
2005ad5a:	8082                	c.jr	ra
2005ad5c:	8082                	c.jr	ra

2005ad5e <xPortGetFreeHeapSize>:
2005ad5e:	ed41a503          	lw	a0,-300(gp) # 2006685c <xFreeBytesRemaining>
2005ad62:	8082                	c.jr	ra

2005ad64 <vPortDefineHeapRegions>:
2005ad64:	ecc1a583          	lw	a1,-308(gp) # 20066854 <pxEnd>
2005ad68:	4701                	c.li	a4,0
2005ad6a:	4301                	c.li	t1,0
2005ad6c:	4681                	c.li	a3,0
2005ad6e:	ecc18613          	addi	a2,gp,-308 # 20066854 <pxEnd>
2005ad72:	ee418813          	addi	a6,gp,-284 # 2006686c <xStart>
2005ad76:	415c                	c.lw	a5,4(a0)
2005ad78:	ef81                	c.bnez	a5,2005ad90 <vPortDefineHeapRegions+0x2c>
2005ad7a:	c311                	c.beqz	a4,2005ad7e <vPortDefineHeapRegions+0x1a>
2005ad7c:	c20c                	c.sw	a1,0(a2)
2005ad7e:	ecd1ac23          	sw	a3,-296(gp) # 20066860 <xMinimumEverFreeBytesRemaining>
2005ad82:	ecd1aa23          	sw	a3,-300(gp) # 2006685c <xFreeBytesRemaining>
2005ad86:	80000737          	lui	a4,0x80000
2005ad8a:	ece1a823          	sw	a4,-304(gp) # 20066858 <xBlockAllocatedBit>
2005ad8e:	8082                	c.jr	ra
2005ad90:	4118                	c.lw	a4,0(a0)
2005ad92:	01f77893          	andi	a7,a4,31
2005ad96:	00088a63          	beq	a7,zero,2005adaa <vPortDefineHeapRegions+0x46>
2005ad9a:	01f70893          	addi	a7,a4,31 # 8000001f <__ctrace_end__+0xb600001f>
2005ad9e:	fe08f893          	andi	a7,a7,-32
2005ada2:	97ba                	c.add	a5,a4
2005ada4:	411787b3          	sub	a5,a5,a7
2005ada8:	8746                	c.mv	a4,a7
2005adaa:	00031663          	bne	t1,zero,2005adb6 <vPortDefineHeapRegions+0x52>
2005adae:	00e82023          	sw	a4,0(a6)
2005adb2:	00082223          	sw	zero,4(a6)
2005adb6:	97ba                	c.add	a5,a4
2005adb8:	1781                	c.addi	a5,-32
2005adba:	9b81                	c.andi	a5,-32
2005adbc:	0007a223          	sw	zero,4(a5)
2005adc0:	0007a023          	sw	zero,0(a5)
2005adc4:	40e788b3          	sub	a7,a5,a4
2005adc8:	01172223          	sw	a7,4(a4)
2005adcc:	c31c                	c.sw	a5,0(a4)
2005adce:	c191                	c.beqz	a1,2005add2 <vPortDefineHeapRegions+0x6e>
2005add0:	c198                	c.sw	a4,0(a1)
2005add2:	4358                	c.lw	a4,4(a4)
2005add4:	0305                	c.addi	t1,1
2005add6:	0521                	c.addi	a0,8
2005add8:	96ba                	c.add	a3,a4
2005adda:	85be                	c.mv	a1,a5
2005addc:	4705                	c.li	a4,1
2005adde:	bf61                	c.j	2005ad76 <vPortDefineHeapRegions+0x12>

2005ade0 <vPortGetHeapStats>:
2005ade0:	1101                	c.addi	sp,-32
2005ade2:	cc22                	c.swsp	s0,24(sp)
2005ade4:	ce06                	c.swsp	ra,28(sp)
2005ade6:	ca26                	c.swsp	s1,20(sp)
2005ade8:	c84a                	c.swsp	s2,16(sp)
2005adea:	c64e                	c.swsp	s3,12(sp)
2005adec:	842a                	c.mv	s0,a0
2005adee:	107020ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005adf2:	ee41a783          	lw	a5,-284(gp) # 2006686c <xStart>
2005adf6:	cfb9                	c.beqz	a5,2005ae54 <vPortGetHeapStats+0x74>
2005adf8:	ecc1a683          	lw	a3,-308(gp) # 20066854 <pxEnd>
2005adfc:	597d                	c.li	s2,-1
2005adfe:	4981                	c.li	s3,0
2005ae00:	4481                	c.li	s1,0
2005ae02:	43d8                	c.lw	a4,4(a5)
2005ae04:	0485                	c.addi	s1,1
2005ae06:	00e9f363          	bgeu	s3,a4,2005ae0c <vPortGetHeapStats+0x2c>
2005ae0a:	89ba                	c.mv	s3,a4
2005ae0c:	c701                	c.beqz	a4,2005ae14 <vPortGetHeapStats+0x34>
2005ae0e:	01277363          	bgeu	a4,s2,2005ae14 <vPortGetHeapStats+0x34>
2005ae12:	893a                	c.mv	s2,a4
2005ae14:	439c                	c.lw	a5,0(a5)
2005ae16:	fef696e3          	bne	a3,a5,2005ae02 <vPortGetHeapStats+0x22>
2005ae1a:	188030ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005ae1e:	01342223          	sw	s3,4(s0)
2005ae22:	01242423          	sw	s2,8(s0)
2005ae26:	c444                	c.sw	s1,12(s0)
2005ae28:	5f3020ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005ae2c:	ed41a783          	lw	a5,-300(gp) # 2006685c <xFreeBytesRemaining>
2005ae30:	40f2                	c.lwsp	ra,28(sp)
2005ae32:	44d2                	c.lwsp	s1,20(sp)
2005ae34:	c01c                	c.sw	a5,0(s0)
2005ae36:	edc1a783          	lw	a5,-292(gp) # 20066864 <xNumberOfSuccessfulAllocations>
2005ae3a:	4942                	c.lwsp	s2,16(sp)
2005ae3c:	49b2                	c.lwsp	s3,12(sp)
2005ae3e:	c85c                	c.sw	a5,20(s0)
2005ae40:	ee01a783          	lw	a5,-288(gp) # 20066868 <xNumberOfSuccessfulFrees>
2005ae44:	cc1c                	c.sw	a5,24(s0)
2005ae46:	ed81a783          	lw	a5,-296(gp) # 20066860 <xMinimumEverFreeBytesRemaining>
2005ae4a:	c81c                	c.sw	a5,16(s0)
2005ae4c:	4462                	c.lwsp	s0,24(sp)
2005ae4e:	6105                	c.addi16sp	sp,32
2005ae50:	5e50206f          	jal	zero,2005dc34 <vTaskExitCritical>
2005ae54:	597d                	c.li	s2,-1
2005ae56:	4981                	c.li	s3,0
2005ae58:	4481                	c.li	s1,0
2005ae5a:	b7c1                	c.j	2005ae1a <vPortGetHeapStats+0x3a>

2005ae5c <vListInitialise>:
2005ae5c:	00850793          	addi	a5,a0,8
2005ae60:	577d                	c.li	a4,-1
2005ae62:	c15c                	c.sw	a5,4(a0)
2005ae64:	c518                	c.sw	a4,8(a0)
2005ae66:	c55c                	c.sw	a5,12(a0)
2005ae68:	c91c                	c.sw	a5,16(a0)
2005ae6a:	00052023          	sw	zero,0(a0)
2005ae6e:	8082                	c.jr	ra

2005ae70 <vListInitialiseItem>:
2005ae70:	00052823          	sw	zero,16(a0)
2005ae74:	8082                	c.jr	ra

2005ae76 <vListInsertEnd>:
2005ae76:	415c                	c.lw	a5,4(a0)
2005ae78:	c1dc                	c.sw	a5,4(a1)
2005ae7a:	4798                	c.lw	a4,8(a5)
2005ae7c:	c598                	c.sw	a4,8(a1)
2005ae7e:	c34c                	c.sw	a1,4(a4)
2005ae80:	c78c                	c.sw	a1,8(a5)
2005ae82:	c988                	c.sw	a0,16(a1)
2005ae84:	411c                	c.lw	a5,0(a0)
2005ae86:	0785                	c.addi	a5,1
2005ae88:	c11c                	c.sw	a5,0(a0)
2005ae8a:	8082                	c.jr	ra

2005ae8c <vListInsert>:
2005ae8c:	4194                	c.lw	a3,0(a1)
2005ae8e:	57fd                	c.li	a5,-1
2005ae90:	00850713          	addi	a4,a0,8
2005ae94:	00f69d63          	bne	a3,a5,2005aeae <vListInsert+0x22>
2005ae98:	491c                	c.lw	a5,16(a0)
2005ae9a:	43d8                	c.lw	a4,4(a5)
2005ae9c:	c1d8                	c.sw	a4,4(a1)
2005ae9e:	c70c                	c.sw	a1,8(a4)
2005aea0:	c59c                	c.sw	a5,8(a1)
2005aea2:	c3cc                	c.sw	a1,4(a5)
2005aea4:	c988                	c.sw	a0,16(a1)
2005aea6:	411c                	c.lw	a5,0(a0)
2005aea8:	0785                	c.addi	a5,1
2005aeaa:	c11c                	c.sw	a5,0(a0)
2005aeac:	8082                	c.jr	ra
2005aeae:	87ba                	c.mv	a5,a4
2005aeb0:	4358                	c.lw	a4,4(a4)
2005aeb2:	4310                	c.lw	a2,0(a4)
2005aeb4:	fec6fde3          	bgeu	a3,a2,2005aeae <vListInsert+0x22>
2005aeb8:	b7cd                	c.j	2005ae9a <vListInsert+0xe>

2005aeba <uxListRemove>:
2005aeba:	4154                	c.lw	a3,4(a0)
2005aebc:	4518                	c.lw	a4,8(a0)
2005aebe:	491c                	c.lw	a5,16(a0)
2005aec0:	c698                	c.sw	a4,8(a3)
2005aec2:	4154                	c.lw	a3,4(a0)
2005aec4:	c354                	c.sw	a3,4(a4)
2005aec6:	43d8                	c.lw	a4,4(a5)
2005aec8:	00a71463          	bne	a4,a0,2005aed0 <uxListRemove+0x16>
2005aecc:	4518                	c.lw	a4,8(a0)
2005aece:	c3d8                	c.sw	a4,4(a5)
2005aed0:	00052823          	sw	zero,16(a0)
2005aed4:	4398                	c.lw	a4,0(a5)
2005aed6:	177d                	c.addi	a4,-1
2005aed8:	c398                	c.sw	a4,0(a5)
2005aeda:	4388                	c.lw	a0,0(a5)
2005aedc:	8082                	c.jr	ra

2005aede <rtk_log_array_print>:
2005aede:	1101                	c.addi	sp,-32
2005aee0:	cc22                	c.swsp	s0,24(sp)
2005aee2:	c84a                	c.swsp	s2,16(sp)
2005aee4:	ce06                	c.swsp	ra,28(sp)
2005aee6:	ca26                	c.swsp	s1,20(sp)
2005aee8:	c64e                	c.swsp	s3,12(sp)
2005aeea:	eec18793          	addi	a5,gp,-276 # 20066874 <rtk_log_entry_count>
2005aeee:	4394                	c.lw	a3,0(a5)
2005aef0:	470d                	c.li	a4,3
2005aef2:	842a                	c.mv	s0,a0
2005aef4:	4911                	c.li	s2,4
2005aef6:	00d76463          	bltu	a4,a3,2005aefe <rtk_log_array_print+0x20>
2005aefa:	0007a903          	lw	s2,0(a5)
2005aefe:	557d                	c.li	a0,-1
2005af00:	c801                	c.beqz	s0,2005af10 <rtk_log_array_print+0x32>
2005af02:	0405                	c.addi	s0,1
2005af04:	4481                	c.li	s1,0
2005af06:	200639b7          	lui	s3,0x20063
2005af0a:	01249a63          	bne	s1,s2,2005af1e <rtk_log_array_print+0x40>
2005af0e:	4501                	c.li	a0,0
2005af10:	40f2                	c.lwsp	ra,28(sp)
2005af12:	4462                	c.lwsp	s0,24(sp)
2005af14:	44d2                	c.lwsp	s1,20(sp)
2005af16:	4942                	c.lwsp	s2,16(sp)
2005af18:	49b2                	c.lwsp	s3,12(sp)
2005af1a:	6105                	c.addi16sp	sp,32
2005af1c:	8082                	c.jr	ra
2005af1e:	fff44603          	lbu	a2,-1(s0)
2005af22:	85a2                	c.mv	a1,s0
2005af24:	00098513          	addi	a0,s3,0 # 20063000 <__FUNCTION__.0+0xac>
2005af28:	47e040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005af2c:	0485                	c.addi	s1,1
2005af2e:	042d                	c.addi	s0,11
2005af30:	bfe9                	c.j	2005af0a <rtk_log_array_print+0x2c>

2005af32 <rtk_log_array_clear>:
2005af32:	1141                	c.addi	sp,-16
2005af34:	02c00613          	addi	a2,zero,44
2005af38:	4581                	c.li	a1,0
2005af3a:	5ac18513          	addi	a0,gp,1452 # 20066f34 <rtk_log_tag_array>
2005af3e:	c606                	c.swsp	ra,12(sp)
2005af40:	dffaf097          	auipc	ra,0xdffaf
2005af44:	84a080e7          	jalr	ra,-1974(ra) # 978a <_memset>
2005af48:	40b2                	c.lwsp	ra,12(sp)
2005af4a:	ee01a623          	sw	zero,-276(gp) # 20066874 <rtk_log_entry_count>
2005af4e:	0141                	c.addi	sp,16
2005af50:	8082                	c.jr	ra

2005af52 <rtk_log_level_get>:
2005af52:	1101                	c.addi	sp,-32
2005af54:	c64e                	c.swsp	s3,12(sp)
2005af56:	c452                	c.swsp	s4,8(sp)
2005af58:	ce06                	c.swsp	ra,28(sp)
2005af5a:	cc22                	c.swsp	s0,24(sp)
2005af5c:	ca26                	c.swsp	s1,20(sp)
2005af5e:	c84a                	c.swsp	s2,16(sp)
2005af60:	eec18793          	addi	a5,gp,-276 # 20066874 <rtk_log_entry_count>
2005af64:	4394                	c.lw	a3,0(a5)
2005af66:	470d                	c.li	a4,3
2005af68:	89aa                	c.mv	s3,a0
2005af6a:	4a11                	c.li	s4,4
2005af6c:	00d76463          	bltu	a4,a3,2005af74 <rtk_log_level_get+0x22>
2005af70:	0007aa03          	lw	s4,0(a5)
2005af74:	00099a63          	bne	s3,zero,2005af88 <rtk_log_level_get+0x36>
2005af78:	20063537          	lui	a0,0x20063
2005af7c:	05000593          	addi	a1,zero,80
2005af80:	01450513          	addi	a0,a0,20 # 20063014 <__FUNCTION__.0+0xc0>
2005af84:	129040ef          	jal	ra,2005f8ac <io_assert_failed>
2005af88:	5ac18913          	addi	s2,gp,1452 # 20066f34 <rtk_log_tag_array>
2005af8c:	0905                	c.addi	s2,1
2005af8e:	4401                	c.li	s0,0
2005af90:	5ac18493          	addi	s1,gp,1452 # 20066f34 <rtk_log_tag_array>
2005af94:	01441563          	bne	s0,s4,2005af9e <rtk_log_level_get+0x4c>
2005af98:	e7c1c503          	lbu	a0,-388(gp) # 20066804 <rtk_log_default_level>
2005af9c:	a839                	c.j	2005afba <rtk_log_level_get+0x68>
2005af9e:	854a                	c.mv	a0,s2
2005afa0:	85ce                	c.mv	a1,s3
2005afa2:	dffaf097          	auipc	ra,0xdffaf
2005afa6:	88a080e7          	jalr	ra,-1910(ra) # 982c <_strcmp>
2005afaa:	092d                	c.addi	s2,11
2005afac:	ed19                	c.bnez	a0,2005afca <rtk_log_level_get+0x78>
2005afae:	47ad                	c.li	a5,11
2005afb0:	02f40433          	mul	s0,s0,a5
2005afb4:	9426                	c.add	s0,s1
2005afb6:	00044503          	lbu	a0,0(s0)
2005afba:	40f2                	c.lwsp	ra,28(sp)
2005afbc:	4462                	c.lwsp	s0,24(sp)
2005afbe:	44d2                	c.lwsp	s1,20(sp)
2005afc0:	4942                	c.lwsp	s2,16(sp)
2005afc2:	49b2                	c.lwsp	s3,12(sp)
2005afc4:	4a22                	c.lwsp	s4,8(sp)
2005afc6:	6105                	c.addi16sp	sp,32
2005afc8:	8082                	c.jr	ra
2005afca:	0405                	c.addi	s0,1
2005afcc:	b7e1                	c.j	2005af94 <rtk_log_level_get+0x42>

2005afce <rtk_log_level_set>:
2005afce:	1101                	c.addi	sp,-32
2005afd0:	cc22                	c.swsp	s0,24(sp)
2005afd2:	eec18793          	addi	a5,gp,-276 # 20066874 <rtk_log_entry_count>
2005afd6:	c84a                	c.swsp	s2,16(sp)
2005afd8:	c64e                	c.swsp	s3,12(sp)
2005afda:	c05a                	c.swsp	s6,0(sp)
2005afdc:	ce06                	c.swsp	ra,28(sp)
2005afde:	ca26                	c.swsp	s1,20(sp)
2005afe0:	c452                	c.swsp	s4,8(sp)
2005afe2:	c256                	c.swsp	s5,4(sp)
2005afe4:	4398                	c.lw	a4,0(a5)
2005afe6:	478d                	c.li	a5,3
2005afe8:	892a                	c.mv	s2,a0
2005afea:	89ae                	c.mv	s3,a1
2005afec:	eec18413          	addi	s0,gp,-276 # 20066874 <rtk_log_entry_count>
2005aff0:	4b11                	c.li	s6,4
2005aff2:	00e7e463          	bltu	a5,a4,2005affa <rtk_log_level_set+0x2c>
2005aff6:	00042b03          	lw	s6,0(s0)
2005affa:	557d                	c.li	a0,-1
2005affc:	06090b63          	beq	s2,zero,2005b072 <rtk_log_level_set+0xa4>
2005b000:	4795                	c.li	a5,5
2005b002:	0737e863          	bltu	a5,s3,2005b072 <rtk_log_level_set+0xa4>
2005b006:	200635b7          	lui	a1,0x20063
2005b00a:	0a858593          	addi	a1,a1,168 # 200630a8 <__FUNCTION__.0+0x154>
2005b00e:	854a                	c.mv	a0,s2
2005b010:	dffaf097          	auipc	ra,0xdffaf
2005b014:	81c080e7          	jalr	ra,-2020(ra) # 982c <_strcmp>
2005b018:	c939                	c.beqz	a0,2005b06e <rtk_log_level_set+0xa0>
2005b01a:	5ac18a93          	addi	s5,gp,1452 # 20066f34 <rtk_log_tag_array>
2005b01e:	0a85                	c.addi	s5,1
2005b020:	4a01                	c.li	s4,0
2005b022:	5ac18493          	addi	s1,gp,1452 # 20066f34 <rtk_log_tag_array>
2005b026:	076a1063          	bne	s4,s6,2005b086 <rtk_log_level_set+0xb8>
2005b02a:	4018                	c.lw	a4,0(s0)
2005b02c:	478d                	c.li	a5,3
2005b02e:	00e7f863          	bgeu	a5,a4,2005b03e <rtk_log_level_set+0x70>
2005b032:	20063537          	lui	a0,0x20063
2005b036:	0ac50513          	addi	a0,a0,172 # 200630ac <__FUNCTION__.0+0x158>
2005b03a:	36c040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b03e:	4018                	c.lw	a4,0(s0)
2005b040:	401c                	c.lw	a5,0(s0)
2005b042:	452d                	c.li	a0,11
2005b044:	8b0d                	c.andi	a4,3
2005b046:	8b8d                	c.andi	a5,3
2005b048:	02a70733          	mul	a4,a4,a0
2005b04c:	4625                	c.li	a2,9
2005b04e:	85ca                	c.mv	a1,s2
2005b050:	02a787b3          	mul	a5,a5,a0
2005b054:	9726                	c.add	a4,s1
2005b056:	01370023          	sb	s3,0(a4)
2005b05a:	0785                	c.addi	a5,1
2005b05c:	00f48533          	add	a0,s1,a5
2005b060:	589060ef          	jal	ra,20061de8 <strncpy>
2005b064:	401c                	c.lw	a5,0(s0)
2005b066:	4501                	c.li	a0,0
2005b068:	0785                	c.addi	a5,1
2005b06a:	c01c                	c.sw	a5,0(s0)
2005b06c:	a019                	c.j	2005b072 <rtk_log_level_set+0xa4>
2005b06e:	e7318e23          	sb	s3,-388(gp) # 20066804 <rtk_log_default_level>
2005b072:	40f2                	c.lwsp	ra,28(sp)
2005b074:	4462                	c.lwsp	s0,24(sp)
2005b076:	44d2                	c.lwsp	s1,20(sp)
2005b078:	4942                	c.lwsp	s2,16(sp)
2005b07a:	49b2                	c.lwsp	s3,12(sp)
2005b07c:	4a22                	c.lwsp	s4,8(sp)
2005b07e:	4a92                	c.lwsp	s5,4(sp)
2005b080:	4b02                	c.lwsp	s6,0(sp)
2005b082:	6105                	c.addi16sp	sp,32
2005b084:	8082                	c.jr	ra
2005b086:	8556                	c.mv	a0,s5
2005b088:	85ca                	c.mv	a1,s2
2005b08a:	dffae097          	auipc	ra,0xdffae
2005b08e:	7a2080e7          	jalr	ra,1954(ra) # 982c <_strcmp>
2005b092:	0aad                	c.addi	s5,11
2005b094:	e901                	c.bnez	a0,2005b0a4 <rtk_log_level_set+0xd6>
2005b096:	47ad                	c.li	a5,11
2005b098:	02fa0a33          	mul	s4,s4,a5
2005b09c:	94d2                	c.add	s1,s4
2005b09e:	01348023          	sb	s3,0(s1)
2005b0a2:	bfc1                	c.j	2005b072 <rtk_log_level_set+0xa4>
2005b0a4:	0a05                	c.addi	s4,1
2005b0a6:	b741                	c.j	2005b026 <rtk_log_level_set+0x58>

2005b0a8 <rtk_log_memory_dump_word>:
2005b0a8:	1101                	c.addi	sp,-32
2005b0aa:	cc22                	c.swsp	s0,24(sp)
2005b0ac:	ca26                	c.swsp	s1,20(sp)
2005b0ae:	c84a                	c.swsp	s2,16(sp)
2005b0b0:	c64e                	c.swsp	s3,12(sp)
2005b0b2:	c452                	c.swsp	s4,8(sp)
2005b0b4:	c256                	c.swsp	s5,4(sp)
2005b0b6:	c05a                	c.swsp	s6,0(sp)
2005b0b8:	ce06                	c.swsp	ra,28(sp)
2005b0ba:	892a                	c.mv	s2,a0
2005b0bc:	89ae                	c.mv	s3,a1
2005b0be:	84aa                	c.mv	s1,a0
2005b0c0:	4401                	c.li	s0,0
2005b0c2:	20063a37          	lui	s4,0x20063
2005b0c6:	20063ab7          	lui	s5,0x20063
2005b0ca:	20063b37          	lui	s6,0x20063
2005b0ce:	03341163          	bne	s0,s3,2005b0f0 <rtk_log_memory_dump_word+0x48>
2005b0d2:	4462                	c.lwsp	s0,24(sp)
2005b0d4:	40f2                	c.lwsp	ra,28(sp)
2005b0d6:	44d2                	c.lwsp	s1,20(sp)
2005b0d8:	4942                	c.lwsp	s2,16(sp)
2005b0da:	49b2                	c.lwsp	s3,12(sp)
2005b0dc:	4a22                	c.lwsp	s4,8(sp)
2005b0de:	4a92                	c.lwsp	s5,4(sp)
2005b0e0:	4b02                	c.lwsp	s6,0(sp)
2005b0e2:	20063537          	lui	a0,0x20063
2005b0e6:	91c50513          	addi	a0,a0,-1764 # 2006291c <pmap_func+0x4f0>
2005b0ea:	6105                	c.addi16sp	sp,32
2005b0ec:	2ba0406f          	jal	zero,2005f3a6 <DiagPrintf_minimal>
2005b0f0:	ec11                	c.bnez	s0,2005b10c <rtk_log_memory_dump_word+0x64>
2005b0f2:	85ca                	c.mv	a1,s2
2005b0f4:	0d8a8513          	addi	a0,s5,216 # 200630d8 <__FUNCTION__.0+0x184>
2005b0f8:	2ae040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b0fc:	408c                	c.lw	a1,0(s1)
2005b0fe:	0ecb0513          	addi	a0,s6,236 # 200630ec <__FUNCTION__.0+0x198>
2005b102:	0405                	c.addi	s0,1
2005b104:	2a2040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b108:	0491                	c.addi	s1,4
2005b10a:	b7d1                	c.j	2005b0ce <rtk_log_memory_dump_word+0x26>
2005b10c:	00747793          	andi	a5,s0,7
2005b110:	f7f5                	c.bnez	a5,2005b0fc <rtk_log_memory_dump_word+0x54>
2005b112:	85a6                	c.mv	a1,s1
2005b114:	0e0a0513          	addi	a0,s4,224 # 200630e0 <__FUNCTION__.0+0x18c>
2005b118:	b7c5                	c.j	2005b0f8 <rtk_log_memory_dump_word+0x50>

2005b11a <rtk_log_memory_dump_byte>:
2005b11a:	1101                	c.addi	sp,-32
2005b11c:	cc22                	c.swsp	s0,24(sp)
2005b11e:	ca26                	c.swsp	s1,20(sp)
2005b120:	c84a                	c.swsp	s2,16(sp)
2005b122:	c452                	c.swsp	s4,8(sp)
2005b124:	c256                	c.swsp	s5,4(sp)
2005b126:	c05a                	c.swsp	s6,0(sp)
2005b128:	ce06                	c.swsp	ra,28(sp)
2005b12a:	c64e                	c.swsp	s3,12(sp)
2005b12c:	84aa                	c.mv	s1,a0
2005b12e:	892e                	c.mv	s2,a1
2005b130:	4401                	c.li	s0,0
2005b132:	20063a37          	lui	s4,0x20063
2005b136:	20063ab7          	lui	s5,0x20063
2005b13a:	20063b37          	lui	s6,0x20063
2005b13e:	03241163          	bne	s0,s2,2005b160 <rtk_log_memory_dump_byte+0x46>
2005b142:	4462                	c.lwsp	s0,24(sp)
2005b144:	40f2                	c.lwsp	ra,28(sp)
2005b146:	44d2                	c.lwsp	s1,20(sp)
2005b148:	4942                	c.lwsp	s2,16(sp)
2005b14a:	49b2                	c.lwsp	s3,12(sp)
2005b14c:	4a22                	c.lwsp	s4,8(sp)
2005b14e:	4a92                	c.lwsp	s5,4(sp)
2005b150:	4b02                	c.lwsp	s6,0(sp)
2005b152:	20063537          	lui	a0,0x20063
2005b156:	91c50513          	addi	a0,a0,-1764 # 2006291c <pmap_func+0x4f0>
2005b15a:	6105                	c.addi16sp	sp,32
2005b15c:	24a0406f          	jal	zero,2005f3a6 <DiagPrintf_minimal>
2005b160:	008489b3          	add	s3,s1,s0
2005b164:	ec11                	c.bnez	s0,2005b180 <rtk_log_memory_dump_byte+0x66>
2005b166:	85a6                	c.mv	a1,s1
2005b168:	0d8a8513          	addi	a0,s5,216 # 200630d8 <__FUNCTION__.0+0x184>
2005b16c:	23a040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b170:	0009c583          	lbu	a1,0(s3)
2005b174:	0f4b0513          	addi	a0,s6,244 # 200630f4 <__FUNCTION__.0+0x1a0>
2005b178:	0405                	c.addi	s0,1
2005b17a:	22c040ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b17e:	b7c1                	c.j	2005b13e <rtk_log_memory_dump_byte+0x24>
2005b180:	00747793          	andi	a5,s0,7
2005b184:	f7f5                	c.bnez	a5,2005b170 <rtk_log_memory_dump_byte+0x56>
2005b186:	85ce                	c.mv	a1,s3
2005b188:	0e0a0513          	addi	a0,s4,224 # 200630e0 <__FUNCTION__.0+0x18c>
2005b18c:	b7c5                	c.j	2005b16c <rtk_log_memory_dump_byte+0x52>

2005b18e <rtk_log_write>:
2005b18e:	7139                	c.addi16sp	sp,-64
2005b190:	d226                	c.swsp	s1,36(sp)
2005b192:	84aa                	c.mv	s1,a0
2005b194:	852e                	c.mv	a0,a1
2005b196:	d422                	c.swsp	s0,40(sp)
2005b198:	d606                	c.swsp	ra,44(sp)
2005b19a:	c632                	c.swsp	a2,12(sp)
2005b19c:	8436                	c.mv	s0,a3
2005b19e:	d83a                	c.swsp	a4,48(sp)
2005b1a0:	da3e                	c.swsp	a5,52(sp)
2005b1a2:	dc42                	c.swsp	a6,56(sp)
2005b1a4:	de46                	c.swsp	a7,60(sp)
2005b1a6:	c42e                	c.swsp	a1,8(sp)
2005b1a8:	dabff0ef          	jal	ra,2005af52 <rtk_log_level_get>
2005b1ac:	02956563          	bltu	a0,s1,2005b1d6 <rtk_log_write+0x48>
2005b1b0:	45a2                	c.lwsp	a1,8(sp)
2005b1b2:	02300793          	addi	a5,zero,35
2005b1b6:	0005c703          	lbu	a4,0(a1)
2005b1ba:	00f70963          	beq	a4,a5,2005b1cc <rtk_log_write+0x3e>
2005b1be:	4632                	c.lwsp	a2,12(sp)
2005b1c0:	20063537          	lui	a0,0x20063
2005b1c4:	0fc50513          	addi	a0,a0,252 # 200630fc <__FUNCTION__.0+0x1a8>
2005b1c8:	1be040ef          	jal	ra,2005f386 <DiagPrintf>
2005b1cc:	180c                	c.addi4spn	a1,sp,48
2005b1ce:	8522                	c.mv	a0,s0
2005b1d0:	ce2e                	c.swsp	a1,28(sp)
2005b1d2:	6b7030ef          	jal	ra,2005f088 <DiagVprintf>
2005b1d6:	50b2                	c.lwsp	ra,44(sp)
2005b1d8:	5422                	c.lwsp	s0,40(sp)
2005b1da:	5492                	c.lwsp	s1,36(sp)
2005b1dc:	6121                	c.addi16sp	sp,64
2005b1de:	8082                	c.jr	ra

2005b1e0 <app_mbedtls_free_func>:
2005b1e0:	ab3d                	c.j	2005b71e <rtos_mem_free>

2005b1e2 <app_mbedtls_calloc_func>:
2005b1e2:	02b50633          	mul	a2,a0,a1
2005b1e6:	1101                	c.addi	sp,-32
2005b1e8:	cc22                	c.swsp	s0,24(sp)
2005b1ea:	ce06                	c.swsp	ra,28(sp)
2005b1ec:	8532                	c.mv	a0,a2
2005b1ee:	c632                	c.swsp	a2,12(sp)
2005b1f0:	2311                	c.jal	2005b6f4 <rtos_mem_malloc>
2005b1f2:	842a                	c.mv	s0,a0
2005b1f4:	c519                	c.beqz	a0,2005b202 <app_mbedtls_calloc_func+0x20>
2005b1f6:	4632                	c.lwsp	a2,12(sp)
2005b1f8:	4581                	c.li	a1,0
2005b1fa:	dffaf097          	auipc	ra,0xdffaf
2005b1fe:	8d4080e7          	jalr	ra,-1836(ra) # 9ace <__wrap_memset>
2005b202:	40f2                	c.lwsp	ra,28(sp)
2005b204:	8522                	c.mv	a0,s0
2005b206:	4462                	c.lwsp	s0,24(sp)
2005b208:	6105                	c.addi16sp	sp,32
2005b20a:	8082                	c.jr	ra

2005b20c <app_IWDG_refresh>:
2005b20c:	4100d537          	lui	a0,0x4100d
2005b210:	c0050513          	addi	a0,a0,-1024 # 4100cc00 <__km4_bd_boot_download_addr__+0x10ffac00>
2005b214:	dffad317          	auipc	t1,0xdffad
2005b218:	fee30067          	jalr	zero,-18(t1) # 8202 <WDG_Refresh>

2005b21c <app_init_debug>:
2005b21c:	1141                	c.addi	sp,-16
2005b21e:	c606                	c.swsp	ra,12(sp)
2005b220:	dffac097          	auipc	ra,0xdffac
2005b224:	980080e7          	jalr	ra,-1664(ra) # 6ba0 <SYSCFG_OTP_DisBootLog>
2005b228:	00153513          	sltiu	a0,a0,1
2005b22c:	200017b7          	lui	a5,0x20001
2005b230:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b234:	0506                	c.slli	a0,0x1
2005b236:	577d                	c.li	a4,-1
2005b238:	c398                	c.sw	a4,0(a5)
2005b23a:	0007a223          	sw	zero,4(a5)
2005b23e:	c788                	c.sw	a0,8(a5)
2005b240:	0007a623          	sw	zero,12(a5)
2005b244:	40b2                	c.lwsp	ra,12(sp)
2005b246:	0141                	c.addi	sp,16
2005b248:	8082                	c.jr	ra

2005b24a <app_mbedtls_rom_init>:
2005b24a:	2005b5b7          	lui	a1,0x2005b
2005b24e:	2005b537          	lui	a0,0x2005b
2005b252:	1141                	c.addi	sp,-16
2005b254:	1e058593          	addi	a1,a1,480 # 2005b1e0 <app_mbedtls_free_func>
2005b258:	1e250513          	addi	a0,a0,482 # 2005b1e2 <app_mbedtls_calloc_func>
2005b25c:	c606                	c.swsp	ra,12(sp)
2005b25e:	403000ef          	jal	ra,2005be60 <mbedtls_platform_set_calloc_free>
2005b262:	40b2                	c.lwsp	ra,12(sp)
2005b264:	ee01a823          	sw	zero,-272(gp) # 20066878 <use_hw_crypto_func>
2005b268:	0141                	c.addi	sp,16
2005b26a:	8082                	c.jr	ra

2005b26c <app_pmu_init>:
2005b26c:	1141                	c.addi	sp,-16
2005b26e:	200017b7          	lui	a5,0x20001
2005b272:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b276:	c606                	c.swsp	ra,12(sp)
2005b278:	4798                	c.lw	a4,8(a5)
2005b27a:	100006b7          	lui	a3,0x10000
2005b27e:	4501                	c.li	a0,0
2005b280:	8f55                	c.or	a4,a3
2005b282:	c798                	c.sw	a4,8(a5)
2005b284:	dbefe0ef          	jal	ra,20059842 <pmu_acquire_wakelock>
2005b288:	4100c7b7          	lui	a5,0x4100c
2005b28c:	22c7a783          	lw	a5,556(a5) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
2005b290:	8b89                	c.andi	a5,2
2005b292:	c38d                	c.beqz	a5,2005b2b4 <app_pmu_init+0x48>
2005b294:	4509                	c.li	a0,2
2005b296:	dacfe0ef          	jal	ra,20059842 <pmu_acquire_wakelock>
2005b29a:	9bbfe0ef          	jal	ra,20059c54 <SWR_CORE_Vol_Get>
2005b29e:	4785                	c.li	a5,1
2005b2a0:	00f51a63          	bne	a0,a5,2005b2b4 <app_pmu_init+0x48>
2005b2a4:	90afe0ef          	jal	ra,200593ae <dsp_status_on>
2005b2a8:	c511                	c.beqz	a0,2005b2b4 <app_pmu_init+0x48>
2005b2aa:	40b2                	c.lwsp	ra,12(sp)
2005b2ac:	4511                	c.li	a0,4
2005b2ae:	0141                	c.addi	sp,16
2005b2b0:	d92fe06f          	jal	zero,20059842 <pmu_acquire_wakelock>
2005b2b4:	40b2                	c.lwsp	ra,12(sp)
2005b2b6:	0141                	c.addi	sp,16
2005b2b8:	8082                	c.jr	ra

2005b2ba <app_IWDG_int>:
2005b2ba:	4100c7b7          	lui	a5,0x4100c
2005b2be:	479c                	c.lw	a5,8(a5)
2005b2c0:	c0000737          	lui	a4,0xc0000
2005b2c4:	0709                	c.addi	a4,2
2005b2c6:	8ff9                	c.and	a5,a4
2005b2c8:	c7a5                	c.beqz	a5,2005b330 <app_IWDG_int+0x76>
2005b2ca:	1101                	c.addi	sp,-32
2005b2cc:	cc22                	c.swsp	s0,24(sp)
2005b2ce:	200636b7          	lui	a3,0x20063
2005b2d2:	20063437          	lui	s0,0x20063
2005b2d6:	10868693          	addi	a3,a3,264 # 20063108 <__FUNCTION__.0+0x1b4>
2005b2da:	04900613          	addi	a2,zero,73
2005b2de:	11c40593          	addi	a1,s0,284 # 2006311c <__FUNCTION__.0+0x1c8>
2005b2e2:	4511                	c.li	a0,4
2005b2e4:	ce06                	c.swsp	ra,28(sp)
2005b2e6:	3565                	c.jal	2005b18e <rtk_log_write>
2005b2e8:	2005b7b7          	lui	a5,0x2005b
2005b2ec:	200635b7          	lui	a1,0x20063
2005b2f0:	20c78793          	addi	a5,a5,524 # 2005b20c <app_IWDG_refresh>
2005b2f4:	4705                	c.li	a4,1
2005b2f6:	1f400693          	addi	a3,zero,500
2005b2fa:	4601                	c.li	a2,0
2005b2fc:	12458593          	addi	a1,a1,292 # 20063124 <__FUNCTION__.0+0x1d0>
2005b300:	0068                	c.addi4spn	a0,sp,12
2005b302:	c602                	c.swsp	zero,12(sp)
2005b304:	14f000ef          	jal	ra,2005bc52 <rtos_timer_create>
2005b308:	4532                	c.lwsp	a0,12(sp)
2005b30a:	ed19                	c.bnez	a0,2005b328 <app_IWDG_int+0x6e>
2005b30c:	200636b7          	lui	a3,0x20063
2005b310:	13068693          	addi	a3,a3,304 # 20063130 <__FUNCTION__.0+0x1dc>
2005b314:	04500613          	addi	a2,zero,69
2005b318:	11c40593          	addi	a1,s0,284
2005b31c:	4509                	c.li	a0,2
2005b31e:	3d85                	c.jal	2005b18e <rtk_log_write>
2005b320:	40f2                	c.lwsp	ra,28(sp)
2005b322:	4462                	c.lwsp	s0,24(sp)
2005b324:	6105                	c.addi16sp	sp,32
2005b326:	8082                	c.jr	ra
2005b328:	4581                	c.li	a1,0
2005b32a:	207000ef          	jal	ra,2005bd30 <rtos_timer_start>
2005b32e:	bfcd                	c.j	2005b320 <app_IWDG_int+0x66>
2005b330:	8082                	c.jr	ra

2005b332 <app_pre_example>:
2005b332:	8082                	c.jr	ra

2005b334 <app_example>:
2005b334:	8082                	c.jr	ra

2005b336 <main>:
2005b336:	20059537          	lui	a0,0x20059
2005b33a:	1141                	c.addi	sp,-16
2005b33c:	4691                	c.li	a3,4
2005b33e:	41020637          	lui	a2,0x41020
2005b342:	02b00593          	addi	a1,zero,43
2005b346:	17e50513          	addi	a0,a0,382 # 2005917e <IPC_INTHandler>
2005b34a:	c606                	c.swsp	ra,12(sp)
2005b34c:	5a6040ef          	jal	ra,2005f8f2 <irq_register>
2005b350:	02b00513          	addi	a0,zero,43
2005b354:	dffad097          	auipc	ra,0xdffad
2005b358:	c84080e7          	jalr	ra,-892(ra) # 7fd8 <irq_enable>
2005b35c:	35fd                	c.jal	2005b24a <app_mbedtls_rom_init>
2005b35e:	4581                	c.li	a1,0
2005b360:	4501                	c.li	a0,0
2005b362:	dffae097          	auipc	ra,0xdffae
2005b366:	ef2080e7          	jalr	ra,-270(ra) # 9254 <shell_init_rom>
2005b36a:	60b010ef          	jal	ra,2005d174 <shell_init_ram>
2005b36e:	41020537          	lui	a0,0x41020
2005b372:	a05fd0ef          	jal	ra,20058d76 <ipc_table_init>
2005b376:	2005c537          	lui	a0,0x2005c
2005b37a:	bf850513          	addi	a0,a0,-1032 # 2005bbf8 <rtos_time_delay_ms>
2005b37e:	fe3fd0ef          	jal	ra,20059360 <IPC_SEMDelayStub>
2005b382:	3f45                	c.jal	2005b332 <app_pre_example>
2005b384:	6ae030ef          	jal	ra,2005ea32 <wlan_initialize>
2005b388:	3d51                	c.jal	2005b21c <app_init_debug>
2005b38a:	35cd                	c.jal	2005b26c <app_pmu_init>
2005b38c:	373d                	c.jal	2005b2ba <app_IWDG_int>
2005b38e:	375d                	c.jal	2005b334 <app_example>
2005b390:	200636b7          	lui	a3,0x20063
2005b394:	200635b7          	lui	a1,0x20063
2005b398:	14468693          	addi	a3,a3,324 # 20063144 <__FUNCTION__.0+0x1f0>
2005b39c:	04900613          	addi	a2,zero,73
2005b3a0:	11c58593          	addi	a1,a1,284 # 2006311c <__FUNCTION__.0+0x1c8>
2005b3a4:	4511                	c.li	a0,4
2005b3a6:	de9ff0ef          	jal	ra,2005b18e <rtk_log_write>
2005b3aa:	7e2000ef          	jal	ra,2005bb8c <rtos_sched_start>
2005b3ae:	40b2                	c.lwsp	ra,12(sp)
2005b3b0:	4501                	c.li	a0,0
2005b3b2:	0141                	c.addi	sp,16
2005b3b4:	8082                	c.jr	ra

2005b3b6 <CmdRamHelp>:
2005b3b6:	1101                	c.addi	sp,-32
2005b3b8:	cc22                	c.swsp	s0,24(sp)
2005b3ba:	ca26                	c.swsp	s1,20(sp)
2005b3bc:	20065437          	lui	s0,0x20065
2005b3c0:	200654b7          	lui	s1,0x20065
2005b3c4:	c5448793          	addi	a5,s1,-940 # 20064c54 <shell_cmd_table>
2005b3c8:	c9440413          	addi	s0,s0,-876 # 20064c94 <ipc_tickless_table>
2005b3cc:	8c1d                	c.sub	s0,a5
2005b3ce:	8011                	c.srli	s0,0x4
2005b3d0:	20063537          	lui	a0,0x20063
2005b3d4:	85a2                	c.mv	a1,s0
2005b3d6:	15450513          	addi	a0,a0,340 # 20063154 <__FUNCTION__.0+0x200>
2005b3da:	c84a                	c.swsp	s2,16(sp)
2005b3dc:	c64e                	c.swsp	s3,12(sp)
2005b3de:	ce06                	c.swsp	ra,28(sp)
2005b3e0:	c5448493          	addi	s1,s1,-940
2005b3e4:	7c3030ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b3e8:	4901                	c.li	s2,0
2005b3ea:	200639b7          	lui	s3,0x20063
2005b3ee:	02891163          	bne	s2,s0,2005b410 <CmdRamHelp+0x5a>
2005b3f2:	20063537          	lui	a0,0x20063
2005b3f6:	85ca                	c.mv	a1,s2
2005b3f8:	19850513          	addi	a0,a0,408 # 20063198 <__FUNCTION__.0+0x244>
2005b3fc:	7ab030ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b400:	40f2                	c.lwsp	ra,28(sp)
2005b402:	4462                	c.lwsp	s0,24(sp)
2005b404:	44d2                	c.lwsp	s1,20(sp)
2005b406:	4942                	c.lwsp	s2,16(sp)
2005b408:	49b2                	c.lwsp	s3,12(sp)
2005b40a:	4505                	c.li	a0,1
2005b40c:	6105                	c.addi16sp	sp,32
2005b40e:	8082                	c.jr	ra
2005b410:	44cc                	c.lw	a1,12(s1)
2005b412:	c589                	c.beqz	a1,2005b41c <CmdRamHelp+0x66>
2005b414:	19498513          	addi	a0,s3,404 # 20063194 <__FUNCTION__.0+0x240>
2005b418:	78f030ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b41c:	0905                	c.addi	s2,1
2005b41e:	04c1                	c.addi	s1,16
2005b420:	b7f9                	c.j	2005b3ee <CmdRamHelp+0x38>

2005b422 <cmd_reboot>:
2005b422:	1141                	c.addi	sp,-16
2005b424:	c226                	c.swsp	s1,4(sp)
2005b426:	84aa                	c.mv	s1,a0
2005b428:	20063537          	lui	a0,0x20063
2005b42c:	1d850513          	addi	a0,a0,472 # 200631d8 <__FUNCTION__.0+0x284>
2005b430:	c422                	c.swsp	s0,8(sp)
2005b432:	c606                	c.swsp	ra,12(sp)
2005b434:	842e                	c.mv	s0,a1
2005b436:	771030ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b43a:	c08d                	c.beqz	s1,2005b45c <cmd_reboot+0x3a>
2005b43c:	4008                	c.lw	a0,0(s0)
2005b43e:	200635b7          	lui	a1,0x20063
2005b442:	1e858593          	addi	a1,a1,488 # 200631e8 <__FUNCTION__.0+0x294>
2005b446:	dffae097          	auipc	ra,0xdffae
2005b44a:	3e6080e7          	jalr	ra,998(ra) # 982c <_strcmp>
2005b44e:	e519                	c.bnez	a0,2005b45c <cmd_reboot+0x3a>
2005b450:	20000593          	addi	a1,zero,512
2005b454:	dffa5097          	auipc	ra,0xdffa5
2005b458:	452080e7          	jalr	ra,1106(ra) # 8a6 <BKUP_Set>
2005b45c:	facfe0ef          	jal	ra,20059c08 <System_Reset>
2005b460:	40b2                	c.lwsp	ra,12(sp)
2005b462:	4422                	c.lwsp	s0,8(sp)
2005b464:	4492                	c.lwsp	s1,4(sp)
2005b466:	4505                	c.li	a0,1
2005b468:	0141                	c.addi	sp,16
2005b46a:	8082                	c.jr	ra

2005b46c <cmd_write_word>:
2005b46c:	1141                	c.addi	sp,-16
2005b46e:	c606                	c.swsp	ra,12(sp)
2005b470:	c422                	c.swsp	s0,8(sp)
2005b472:	c226                	c.swsp	s1,4(sp)
2005b474:	c04a                	c.swsp	s2,0(sp)
2005b476:	892e                	c.mv	s2,a1
2005b478:	00092503          	lw	a0,0(s2)
2005b47c:	4641                	c.li	a2,16
2005b47e:	4581                	c.li	a1,0
2005b480:	dffae097          	auipc	ra,0xdffae
2005b484:	4b6080e7          	jalr	ra,1206(ra) # 9936 <_strtoul>
2005b488:	ffc57413          	andi	s0,a0,-4
2005b48c:	84aa                	c.mv	s1,a0
2005b48e:	00492503          	lw	a0,4(s2)
2005b492:	4641                	c.li	a2,16
2005b494:	4581                	c.li	a1,0
2005b496:	dffae097          	auipc	ra,0xdffae
2005b49a:	4a0080e7          	jalr	ra,1184(ra) # 9936 <_strtoul>
2005b49e:	892a                	c.mv	s2,a0
2005b4a0:	862a                	c.mv	a2,a0
2005b4a2:	20063537          	lui	a0,0x20063
2005b4a6:	85a2                	c.mv	a1,s0
2005b4a8:	1f450513          	addi	a0,a0,500 # 200631f4 <__FUNCTION__.0+0x2a0>
2005b4ac:	6fb030ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b4b0:	01242023          	sw	s2,0(s0)
2005b4b4:	7c1027f3          	csrrs	a5,0x7c1,zero
2005b4b8:	1007f793          	andi	a5,a5,256
2005b4bc:	c7a1                	c.beqz	a5,2005b504 <cmd_write_word+0x98>
2005b4be:	01c4f793          	andi	a5,s1,28
2005b4c2:	4711                	c.li	a4,4
2005b4c4:	c789                	c.beqz	a5,2005b4ce <cmd_write_word+0x62>
2005b4c6:	00478713          	addi	a4,a5,4
2005b4ca:	fe04f413          	andi	s0,s1,-32
2005b4ce:	01f47793          	andi	a5,s0,31
2005b4d2:	97ba                	c.add	a5,a4
2005b4d4:	0ff0000f          	fence	iorw,iorw
2005b4d8:	97a2                	c.add	a5,s0
2005b4da:	0484700b          	cache	dwb,(s0)
2005b4de:	02040413          	addi	s0,s0,32
2005b4e2:	40878733          	sub	a4,a5,s0
2005b4e6:	fee04ae3          	blt	zero,a4,2005b4da <cmd_write_word+0x6e>
2005b4ea:	0ff0000f          	fence	iorw,iorw
2005b4ee:	0001                	c.addi	zero,0
2005b4f0:	0001                	c.addi	zero,0
2005b4f2:	0001                	c.addi	zero,0
2005b4f4:	0001                	c.addi	zero,0
2005b4f6:	0001                	c.addi	zero,0
2005b4f8:	0001                	c.addi	zero,0
2005b4fa:	0001                	c.addi	zero,0
2005b4fc:	0001                	c.addi	zero,0
2005b4fe:	0001                	c.addi	zero,0
2005b500:	0001                	c.addi	zero,0
2005b502:	0001                	c.addi	zero,0
2005b504:	40b2                	c.lwsp	ra,12(sp)
2005b506:	4422                	c.lwsp	s0,8(sp)
2005b508:	4492                	c.lwsp	s1,4(sp)
2005b50a:	4902                	c.lwsp	s2,0(sp)
2005b50c:	4501                	c.li	a0,0
2005b50e:	0141                	c.addi	sp,16
2005b510:	8082                	c.jr	ra

2005b512 <cmd_dump_word>:
2005b512:	1101                	c.addi	sp,-32
2005b514:	cc22                	c.swsp	s0,24(sp)
2005b516:	c84a                	c.swsp	s2,16(sp)
2005b518:	842a                	c.mv	s0,a0
2005b51a:	ce06                	c.swsp	ra,28(sp)
2005b51c:	ca26                	c.swsp	s1,20(sp)
2005b51e:	c64e                	c.swsp	s3,12(sp)
2005b520:	892e                	c.mv	s2,a1
2005b522:	dffab097          	auipc	ra,0xdffab
2005b526:	62c080e7          	jalr	ra,1580(ra) # 6b4e <SYSCFG_OTP_RSIPEn>
2005b52a:	fff40793          	addi	a5,s0,-1
2005b52e:	07c2                	c.slli	a5,0x10
2005b530:	83c1                	c.srli	a5,0x10
2005b532:	4709                	c.li	a4,2
2005b534:	00f77a63          	bgeu	a4,a5,2005b548 <cmd_dump_word+0x36>
2005b538:	20063537          	lui	a0,0x20063
2005b53c:	20450513          	addi	a0,a0,516 # 20063204 <__FUNCTION__.0+0x2b0>
2005b540:	667030ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b544:	4401                	c.li	s0,0
2005b546:	a0f9                	c.j	2005b614 <cmd_dump_word+0x102>
2005b548:	478d                	c.li	a5,3
2005b54a:	89aa                	c.mv	s3,a0
2005b54c:	06f41063          	bne	s0,a5,2005b5ac <cmd_dump_word+0x9a>
2005b550:	00892783          	lw	a5,8(s2)
2005b554:	0007c403          	lbu	s0,0(a5)
2005b558:	0df47413          	andi	s0,s0,223
2005b55c:	fbe40413          	addi	s0,s0,-66
2005b560:	00143413          	sltiu	s0,s0,1
2005b564:	00492503          	lw	a0,4(s2)
2005b568:	4629                	c.li	a2,10
2005b56a:	4581                	c.li	a1,0
2005b56c:	dffae097          	auipc	ra,0xdffae
2005b570:	3ca080e7          	jalr	ra,970(ra) # 9936 <_strtoul>
2005b574:	84aa                	c.mv	s1,a0
2005b576:	00092503          	lw	a0,0(s2)
2005b57a:	4641                	c.li	a2,16
2005b57c:	4581                	c.li	a1,0
2005b57e:	dffae097          	auipc	ra,0xdffae
2005b582:	3b8080e7          	jalr	ra,952(ra) # 9936 <_strtoul>
2005b586:	87aa                	c.mv	a5,a0
2005b588:	f8000737          	lui	a4,0xf8000
2005b58c:	9971                	c.andi	a0,-4
2005b58e:	972a                	c.add	a4,a0
2005b590:	080006b7          	lui	a3,0x8000
2005b594:	02d77463          	bgeu	a4,a3,2005b5bc <cmd_dump_word+0xaa>
2005b598:	02098263          	beq	s3,zero,2005b5bc <cmd_dump_word+0xaa>
2005b59c:	20063537          	lui	a0,0x20063
2005b5a0:	22050513          	addi	a0,a0,544 # 20063220 <__FUNCTION__.0+0x2cc>
2005b5a4:	603030ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b5a8:	4405                	c.li	s0,1
2005b5aa:	a0ad                	c.j	2005b614 <cmd_dump_word+0x102>
2005b5ac:	4785                	c.li	a5,1
2005b5ae:	00f40463          	beq	s0,a5,2005b5b6 <cmd_dump_word+0xa4>
2005b5b2:	4401                	c.li	s0,0
2005b5b4:	bf45                	c.j	2005b564 <cmd_dump_word+0x52>
2005b5b6:	4401                	c.li	s0,0
2005b5b8:	4485                	c.li	s1,1
2005b5ba:	bf75                	c.j	2005b576 <cmd_dump_word+0x64>
2005b5bc:	c43d                	c.beqz	s0,2005b62a <cmd_dump_word+0x118>
2005b5be:	c4f9                	c.beqz	s1,2005b68c <cmd_dump_word+0x17a>
2005b5c0:	7c102773          	csrrs	a4,0x7c1,zero
2005b5c4:	10077713          	andi	a4,a4,256
2005b5c8:	c339                	c.beqz	a4,2005b60e <cmd_dump_word+0xfc>
2005b5ca:	01c7f713          	andi	a4,a5,28
2005b5ce:	cb39                	c.beqz	a4,2005b624 <cmd_dump_word+0x112>
2005b5d0:	9b81                	c.andi	a5,-32
2005b5d2:	9726                	c.add	a4,s1
2005b5d4:	02e05d63          	bge	zero,a4,2005b60e <cmd_dump_word+0xfc>
2005b5d8:	01f7f693          	andi	a3,a5,31
2005b5dc:	9736                	c.add	a4,a3
2005b5de:	0ff0000f          	fence	iorw,iorw
2005b5e2:	973e                	c.add	a4,a5
2005b5e4:	02f7f00b          	cache	dwbinv,(a5)
2005b5e8:	02078793          	addi	a5,a5,32
2005b5ec:	40f706b3          	sub	a3,a4,a5
2005b5f0:	fed04ae3          	blt	zero,a3,2005b5e4 <cmd_dump_word+0xd2>
2005b5f4:	0ff0000f          	fence	iorw,iorw
2005b5f8:	0001                	c.addi	zero,0
2005b5fa:	0001                	c.addi	zero,0
2005b5fc:	0001                	c.addi	zero,0
2005b5fe:	0001                	c.addi	zero,0
2005b600:	0001                	c.addi	zero,0
2005b602:	0001                	c.addi	zero,0
2005b604:	0001                	c.addi	zero,0
2005b606:	0001                	c.addi	zero,0
2005b608:	0001                	c.addi	zero,0
2005b60a:	0001                	c.addi	zero,0
2005b60c:	0001                	c.addi	zero,0
2005b60e:	85a6                	c.mv	a1,s1
2005b610:	b0bff0ef          	jal	ra,2005b11a <rtk_log_memory_dump_byte>
2005b614:	40f2                	c.lwsp	ra,28(sp)
2005b616:	8522                	c.mv	a0,s0
2005b618:	4462                	c.lwsp	s0,24(sp)
2005b61a:	44d2                	c.lwsp	s1,20(sp)
2005b61c:	4942                	c.lwsp	s2,16(sp)
2005b61e:	49b2                	c.lwsp	s3,12(sp)
2005b620:	6105                	c.addi16sp	sp,32
2005b622:	8082                	c.jr	ra
2005b624:	8726                	c.mv	a4,s1
2005b626:	87aa                	c.mv	a5,a0
2005b628:	b775                	c.j	2005b5d4 <cmd_dump_word+0xc2>
2005b62a:	c0ad                	c.beqz	s1,2005b68c <cmd_dump_word+0x17a>
2005b62c:	7c102773          	csrrs	a4,0x7c1,zero
2005b630:	10077713          	andi	a4,a4,256
2005b634:	cb21                	c.beqz	a4,2005b684 <cmd_dump_word+0x172>
2005b636:	01c7f693          	andi	a3,a5,28
2005b63a:	00249613          	slli	a2,s1,0x2
2005b63e:	872a                	c.mv	a4,a0
2005b640:	c681                	c.beqz	a3,2005b648 <cmd_dump_word+0x136>
2005b642:	fe07f713          	andi	a4,a5,-32
2005b646:	9636                	c.add	a2,a3
2005b648:	02c05e63          	bge	zero,a2,2005b684 <cmd_dump_word+0x172>
2005b64c:	01f77693          	andi	a3,a4,31
2005b650:	96b2                	c.add	a3,a2
2005b652:	0ff0000f          	fence	iorw,iorw
2005b656:	87ba                	c.mv	a5,a4
2005b658:	9736                	c.add	a4,a3
2005b65a:	02f7f00b          	cache	dwbinv,(a5)
2005b65e:	02078793          	addi	a5,a5,32
2005b662:	40f706b3          	sub	a3,a4,a5
2005b666:	fed04ae3          	blt	zero,a3,2005b65a <cmd_dump_word+0x148>
2005b66a:	0ff0000f          	fence	iorw,iorw
2005b66e:	0001                	c.addi	zero,0
2005b670:	0001                	c.addi	zero,0
2005b672:	0001                	c.addi	zero,0
2005b674:	0001                	c.addi	zero,0
2005b676:	0001                	c.addi	zero,0
2005b678:	0001                	c.addi	zero,0
2005b67a:	0001                	c.addi	zero,0
2005b67c:	0001                	c.addi	zero,0
2005b67e:	0001                	c.addi	zero,0
2005b680:	0001                	c.addi	zero,0
2005b682:	0001                	c.addi	zero,0
2005b684:	85a6                	c.mv	a1,s1
2005b686:	a23ff0ef          	jal	ra,2005b0a8 <rtk_log_memory_dump_word>
2005b68a:	bf39                	c.j	2005b5a8 <cmd_dump_word+0x96>
2005b68c:	20063537          	lui	a0,0x20063
2005b690:	24450513          	addi	a0,a0,580 # 20063244 <__FUNCTION__.0+0x2f0>
2005b694:	b575                	c.j	2005b540 <cmd_dump_word+0x2e>

2005b696 <rtos_critical_is_in_interrupt>:
2005b696:	1141                	c.addi	sp,-16
2005b698:	c606                	c.swsp	ra,12(sp)
2005b69a:	6b90a0ef          	jal	ra,20066552 <plic_get_active_irq_id>
2005b69e:	40b2                	c.lwsp	ra,12(sp)
2005b6a0:	00a03533          	sltu	a0,zero,a0
2005b6a4:	0141                	c.addi	sp,16
2005b6a6:	8082                	c.jr	ra

2005b6a8 <rtos_critical_enter>:
2005b6a8:	1141                	c.addi	sp,-16
2005b6aa:	c606                	c.swsp	ra,12(sp)
2005b6ac:	37ed                	c.jal	2005b696 <rtos_critical_is_in_interrupt>
2005b6ae:	c919                	c.beqz	a0,2005b6c4 <rtos_critical_enter+0x1c>
2005b6b0:	ef418793          	addi	a5,gp,-268 # 2006687c <uxCriticalNestingCnt>
2005b6b4:	0007d703          	lhu	a4,0(a5)
2005b6b8:	0705                	c.addi	a4,1
2005b6ba:	00e79023          	sh	a4,0(a5)
2005b6be:	40b2                	c.lwsp	ra,12(sp)
2005b6c0:	0141                	c.addi	sp,16
2005b6c2:	8082                	c.jr	ra
2005b6c4:	40b2                	c.lwsp	ra,12(sp)
2005b6c6:	0141                	c.addi	sp,16
2005b6c8:	5520206f          	jal	zero,2005dc1a <vTaskEnterCritical>

2005b6cc <rtos_critical_exit>:
2005b6cc:	1141                	c.addi	sp,-16
2005b6ce:	c606                	c.swsp	ra,12(sp)
2005b6d0:	37d9                	c.jal	2005b696 <rtos_critical_is_in_interrupt>
2005b6d2:	c919                	c.beqz	a0,2005b6e8 <rtos_critical_exit+0x1c>
2005b6d4:	ef418793          	addi	a5,gp,-268 # 2006687c <uxCriticalNestingCnt>
2005b6d8:	0007d703          	lhu	a4,0(a5)
2005b6dc:	177d                	c.addi	a4,-1
2005b6de:	00e79023          	sh	a4,0(a5)
2005b6e2:	40b2                	c.lwsp	ra,12(sp)
2005b6e4:	0141                	c.addi	sp,16
2005b6e6:	8082                	c.jr	ra
2005b6e8:	40b2                	c.lwsp	ra,12(sp)
2005b6ea:	0141                	c.addi	sp,16
2005b6ec:	5480206f          	jal	zero,2005dc34 <vTaskExitCritical>

2005b6f0 <rtos_mem_init>:
2005b6f0:	bccff06f          	jal	zero,2005aabc <os_heap_init>

2005b6f4 <rtos_mem_malloc>:
2005b6f4:	d2eff06f          	jal	zero,2005ac22 <pvPortMalloc>

2005b6f8 <rtos_mem_zmalloc>:
2005b6f8:	1101                	c.addi	sp,-32
2005b6fa:	cc22                	c.swsp	s0,24(sp)
2005b6fc:	ce06                	c.swsp	ra,28(sp)
2005b6fe:	c62a                	c.swsp	a0,12(sp)
2005b700:	d22ff0ef          	jal	ra,2005ac22 <pvPortMalloc>
2005b704:	842a                	c.mv	s0,a0
2005b706:	c519                	c.beqz	a0,2005b714 <rtos_mem_zmalloc+0x1c>
2005b708:	4632                	c.lwsp	a2,12(sp)
2005b70a:	4581                	c.li	a1,0
2005b70c:	dffae097          	auipc	ra,0xdffae
2005b710:	3c2080e7          	jalr	ra,962(ra) # 9ace <__wrap_memset>
2005b714:	40f2                	c.lwsp	ra,28(sp)
2005b716:	8522                	c.mv	a0,s0
2005b718:	4462                	c.lwsp	s0,24(sp)
2005b71a:	6105                	c.addi16sp	sp,32
2005b71c:	8082                	c.jr	ra

2005b71e <rtos_mem_free>:
2005b71e:	c119                	c.beqz	a0,2005b724 <rtos_mem_free+0x6>
2005b720:	de0ff06f          	jal	zero,2005ad00 <vPortFree>
2005b724:	8082                	c.jr	ra

2005b726 <rtos_mem_get_free_heap_size>:
2005b726:	e38ff06f          	jal	zero,2005ad5e <xPortGetFreeHeapSize>

2005b72a <rtos_mutex_create>:
2005b72a:	c10d                	c.beqz	a0,2005b74c <rtos_mutex_create+0x22>
2005b72c:	1141                	c.addi	sp,-16
2005b72e:	c422                	c.swsp	s0,8(sp)
2005b730:	842a                	c.mv	s0,a0
2005b732:	4505                	c.li	a0,1
2005b734:	c606                	c.swsp	ra,12(sp)
2005b736:	6d4010ef          	jal	ra,2005ce0a <xQueueCreateMutex>
2005b73a:	c008                	c.sw	a0,0(s0)
2005b73c:	40b2                	c.lwsp	ra,12(sp)
2005b73e:	4422                	c.lwsp	s0,8(sp)
2005b740:	00153513          	sltiu	a0,a0,1
2005b744:	40a00533          	sub	a0,zero,a0
2005b748:	0141                	c.addi	sp,16
2005b74a:	8082                	c.jr	ra
2005b74c:	557d                	c.li	a0,-1
2005b74e:	8082                	c.jr	ra

2005b750 <rtos_mutex_create_static>:
2005b750:	1141                	c.addi	sp,-16
2005b752:	c422                	c.swsp	s0,8(sp)
2005b754:	c606                	c.swsp	ra,12(sp)
2005b756:	842a                	c.mv	s0,a0
2005b758:	2e4d                	c.jal	2005bb0a <__reserved_get_mutex_from_poll>
2005b75a:	e511                	c.bnez	a0,2005b766 <rtos_mutex_create_static+0x16>
2005b75c:	8522                	c.mv	a0,s0
2005b75e:	4422                	c.lwsp	s0,8(sp)
2005b760:	40b2                	c.lwsp	ra,12(sp)
2005b762:	0141                	c.addi	sp,16
2005b764:	b7d9                	c.j	2005b72a <rtos_mutex_create>
2005b766:	85aa                	c.mv	a1,a0
2005b768:	4505                	c.li	a0,1
2005b76a:	72c010ef          	jal	ra,2005ce96 <xQueueCreateMutexStatic>
2005b76e:	c008                	c.sw	a0,0(s0)
2005b770:	40b2                	c.lwsp	ra,12(sp)
2005b772:	4422                	c.lwsp	s0,8(sp)
2005b774:	00153513          	sltiu	a0,a0,1
2005b778:	40a00533          	sub	a0,zero,a0
2005b77c:	0141                	c.addi	sp,16
2005b77e:	8082                	c.jr	ra

2005b780 <rtos_mutex_delete>:
2005b780:	c139                	c.beqz	a0,2005b7c6 <rtos_mutex_delete+0x46>
2005b782:	1141                	c.addi	sp,-16
2005b784:	c422                	c.swsp	s0,8(sp)
2005b786:	c606                	c.swsp	ra,12(sp)
2005b788:	842a                	c.mv	s0,a0
2005b78a:	12a010ef          	jal	ra,2005c8b4 <xQueueGetMutexHolder>
2005b78e:	e909                	c.bnez	a0,2005b7a0 <rtos_mutex_delete+0x20>
2005b790:	8522                	c.mv	a0,s0
2005b792:	786010ef          	jal	ra,2005cf18 <vQueueDelete>
2005b796:	4501                	c.li	a0,0
2005b798:	40b2                	c.lwsp	ra,12(sp)
2005b79a:	4422                	c.lwsp	s0,8(sp)
2005b79c:	0141                	c.addi	sp,16
2005b79e:	8082                	c.jr	ra
2005b7a0:	8522                	c.mv	a0,s0
2005b7a2:	776010ef          	jal	ra,2005cf18 <vQueueDelete>
2005b7a6:	4501                	c.li	a0,0
2005b7a8:	765010ef          	jal	ra,2005d70c <pcTaskGetName>
2005b7ac:	85aa                	c.mv	a1,a0
2005b7ae:	20063637          	lui	a2,0x20063
2005b7b2:	20063537          	lui	a0,0x20063
2005b7b6:	4b860613          	addi	a2,a2,1208 # 200634b8 <__FUNCTION__.0>
2005b7ba:	46450513          	addi	a0,a0,1124 # 20063464 <__FUNCTION__.0+0x510>
2005b7be:	3e9030ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005b7c2:	557d                	c.li	a0,-1
2005b7c4:	bfd1                	c.j	2005b798 <rtos_mutex_delete+0x18>
2005b7c6:	557d                	c.li	a0,-1
2005b7c8:	8082                	c.jr	ra

2005b7ca <rtos_mutex_delete_static>:
2005b7ca:	1141                	c.addi	sp,-16
2005b7cc:	c606                	c.swsp	ra,12(sp)
2005b7ce:	c422                	c.swsp	s0,8(sp)
2005b7d0:	c226                	c.swsp	s1,4(sp)
2005b7d2:	84aa                	c.mv	s1,a0
2005b7d4:	3775                	c.jal	2005b780 <rtos_mutex_delete>
2005b7d6:	842a                	c.mv	s0,a0
2005b7d8:	8526                	c.mv	a0,s1
2005b7da:	26a9                	c.jal	2005bb24 <__reserved_release_mutex_to_poll>
2005b7dc:	40b2                	c.lwsp	ra,12(sp)
2005b7de:	8522                	c.mv	a0,s0
2005b7e0:	4422                	c.lwsp	s0,8(sp)
2005b7e2:	4492                	c.lwsp	s1,4(sp)
2005b7e4:	0141                	c.addi	sp,16
2005b7e6:	8082                	c.jr	ra

2005b7e8 <rtos_mutex_take>:
2005b7e8:	7179                	c.addi16sp	sp,-48
2005b7ea:	d422                	c.swsp	s0,40(sp)
2005b7ec:	c62e                	c.swsp	a1,12(sp)
2005b7ee:	d606                	c.swsp	ra,44(sp)
2005b7f0:	842a                	c.mv	s0,a0
2005b7f2:	ce02                	c.swsp	zero,28(sp)
2005b7f4:	ea3ff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005b7f8:	45b2                	c.lwsp	a1,12(sp)
2005b7fa:	c505                	c.beqz	a0,2005b822 <rtos_mutex_take+0x3a>
2005b7fc:	0870                	c.addi4spn	a2,sp,28
2005b7fe:	4581                	c.li	a1,0
2005b800:	8522                	c.mv	a0,s0
2005b802:	4a6010ef          	jal	ra,2005cca8 <xQueueReceiveFromISR>
2005b806:	872a                	c.mv	a4,a0
2005b808:	4785                	c.li	a5,1
2005b80a:	557d                	c.li	a0,-1
2005b80c:	00f71763          	bne	a4,a5,2005b81a <rtos_mutex_take+0x32>
2005b810:	4572                	c.lwsp	a0,28(sp)
2005b812:	c501                	c.beqz	a0,2005b81a <rtos_mutex_take+0x32>
2005b814:	03a020ef          	jal	ra,2005d84e <vTaskSwitchContext>
2005b818:	4501                	c.li	a0,0
2005b81a:	50b2                	c.lwsp	ra,44(sp)
2005b81c:	5422                	c.lwsp	s0,40(sp)
2005b81e:	6145                	c.addi16sp	sp,48
2005b820:	8082                	c.jr	ra
2005b822:	8522                	c.mv	a0,s0
2005b824:	366010ef          	jal	ra,2005cb8a <xQueueSemaphoreTake>
2005b828:	157d                	c.addi	a0,-1
2005b82a:	00a03533          	sltu	a0,zero,a0
2005b82e:	40a00533          	sub	a0,zero,a0
2005b832:	b7e5                	c.j	2005b81a <rtos_mutex_take+0x32>

2005b834 <rtos_mutex_give>:
2005b834:	1101                	c.addi	sp,-32
2005b836:	cc22                	c.swsp	s0,24(sp)
2005b838:	ce06                	c.swsp	ra,28(sp)
2005b83a:	842a                	c.mv	s0,a0
2005b83c:	c602                	c.swsp	zero,12(sp)
2005b83e:	e59ff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005b842:	c11d                	c.beqz	a0,2005b868 <rtos_mutex_give+0x34>
2005b844:	006c                	c.addi4spn	a1,sp,12
2005b846:	8522                	c.mv	a0,s0
2005b848:	1fe010ef          	jal	ra,2005ca46 <xQueueGiveFromISR>
2005b84c:	872a                	c.mv	a4,a0
2005b84e:	4785                	c.li	a5,1
2005b850:	557d                	c.li	a0,-1
2005b852:	00f71763          	bne	a4,a5,2005b860 <rtos_mutex_give+0x2c>
2005b856:	4532                	c.lwsp	a0,12(sp)
2005b858:	c501                	c.beqz	a0,2005b860 <rtos_mutex_give+0x2c>
2005b85a:	7f5010ef          	jal	ra,2005d84e <vTaskSwitchContext>
2005b85e:	4501                	c.li	a0,0
2005b860:	40f2                	c.lwsp	ra,28(sp)
2005b862:	4462                	c.lwsp	s0,24(sp)
2005b864:	6105                	c.addi16sp	sp,32
2005b866:	8082                	c.jr	ra
2005b868:	4681                	c.li	a3,0
2005b86a:	4601                	c.li	a2,0
2005b86c:	4581                	c.li	a1,0
2005b86e:	8522                	c.mv	a0,s0
2005b870:	068010ef          	jal	ra,2005c8d8 <xQueueGenericSend>
2005b874:	157d                	c.addi	a0,-1
2005b876:	00a03533          	sltu	a0,zero,a0
2005b87a:	40a00533          	sub	a0,zero,a0
2005b87e:	b7cd                	c.j	2005b860 <rtos_mutex_give+0x2c>

2005b880 <rtos_sema_create>:
2005b880:	1141                	c.addi	sp,-16
2005b882:	c422                	c.swsp	s0,8(sp)
2005b884:	c606                	c.swsp	ra,12(sp)
2005b886:	842a                	c.mv	s0,a0
2005b888:	557d                	c.li	a0,-1
2005b88a:	c809                	c.beqz	s0,2005b89c <rtos_sema_create+0x1c>
2005b88c:	8532                	c.mv	a0,a2
2005b88e:	5b0010ef          	jal	ra,2005ce3e <xQueueCreateCountingSemaphore>
2005b892:	c008                	c.sw	a0,0(s0)
2005b894:	00153513          	sltiu	a0,a0,1
2005b898:	40a00533          	sub	a0,zero,a0
2005b89c:	40b2                	c.lwsp	ra,12(sp)
2005b89e:	4422                	c.lwsp	s0,8(sp)
2005b8a0:	0141                	c.addi	sp,16
2005b8a2:	8082                	c.jr	ra

2005b8a4 <rtos_sema_create_static>:
2005b8a4:	1101                	c.addi	sp,-32
2005b8a6:	cc22                	c.swsp	s0,24(sp)
2005b8a8:	ca26                	c.swsp	s1,20(sp)
2005b8aa:	c62e                	c.swsp	a1,12(sp)
2005b8ac:	ce06                	c.swsp	ra,28(sp)
2005b8ae:	842a                	c.mv	s0,a0
2005b8b0:	84b2                	c.mv	s1,a2
2005b8b2:	2471                	c.jal	2005bb3e <__reserved_get_sema_from_poll>
2005b8b4:	45b2                	c.lwsp	a1,12(sp)
2005b8b6:	e901                	c.bnez	a0,2005b8c6 <rtos_sema_create_static+0x22>
2005b8b8:	8522                	c.mv	a0,s0
2005b8ba:	4462                	c.lwsp	s0,24(sp)
2005b8bc:	40f2                	c.lwsp	ra,28(sp)
2005b8be:	8626                	c.mv	a2,s1
2005b8c0:	44d2                	c.lwsp	s1,20(sp)
2005b8c2:	6105                	c.addi16sp	sp,32
2005b8c4:	bf75                	c.j	2005b880 <rtos_sema_create>
2005b8c6:	862a                	c.mv	a2,a0
2005b8c8:	8526                	c.mv	a0,s1
2005b8ca:	604010ef          	jal	ra,2005cece <xQueueCreateCountingSemaphoreStatic>
2005b8ce:	c008                	c.sw	a0,0(s0)
2005b8d0:	40f2                	c.lwsp	ra,28(sp)
2005b8d2:	4462                	c.lwsp	s0,24(sp)
2005b8d4:	00153513          	sltiu	a0,a0,1
2005b8d8:	44d2                	c.lwsp	s1,20(sp)
2005b8da:	40a00533          	sub	a0,zero,a0
2005b8de:	6105                	c.addi16sp	sp,32
2005b8e0:	8082                	c.jr	ra

2005b8e2 <rtos_sema_create_binary>:
2005b8e2:	c11d                	c.beqz	a0,2005b908 <rtos_sema_create_binary+0x26>
2005b8e4:	1141                	c.addi	sp,-16
2005b8e6:	c422                	c.swsp	s0,8(sp)
2005b8e8:	460d                	c.li	a2,3
2005b8ea:	842a                	c.mv	s0,a0
2005b8ec:	4581                	c.li	a1,0
2005b8ee:	4505                	c.li	a0,1
2005b8f0:	c606                	c.swsp	ra,12(sp)
2005b8f2:	4c0010ef          	jal	ra,2005cdb2 <xQueueGenericCreate>
2005b8f6:	c008                	c.sw	a0,0(s0)
2005b8f8:	40b2                	c.lwsp	ra,12(sp)
2005b8fa:	4422                	c.lwsp	s0,8(sp)
2005b8fc:	00153513          	sltiu	a0,a0,1
2005b900:	40a00533          	sub	a0,zero,a0
2005b904:	0141                	c.addi	sp,16
2005b906:	8082                	c.jr	ra
2005b908:	557d                	c.li	a0,-1
2005b90a:	8082                	c.jr	ra

2005b90c <rtos_sema_delete>:
2005b90c:	c909                	c.beqz	a0,2005b91e <rtos_sema_delete+0x12>
2005b90e:	1141                	c.addi	sp,-16
2005b910:	c606                	c.swsp	ra,12(sp)
2005b912:	606010ef          	jal	ra,2005cf18 <vQueueDelete>
2005b916:	40b2                	c.lwsp	ra,12(sp)
2005b918:	4501                	c.li	a0,0
2005b91a:	0141                	c.addi	sp,16
2005b91c:	8082                	c.jr	ra
2005b91e:	557d                	c.li	a0,-1
2005b920:	8082                	c.jr	ra

2005b922 <rtos_sema_take>:
2005b922:	7179                	c.addi16sp	sp,-48
2005b924:	d422                	c.swsp	s0,40(sp)
2005b926:	c62e                	c.swsp	a1,12(sp)
2005b928:	d606                	c.swsp	ra,44(sp)
2005b92a:	842a                	c.mv	s0,a0
2005b92c:	ce02                	c.swsp	zero,28(sp)
2005b92e:	d69ff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005b932:	45b2                	c.lwsp	a1,12(sp)
2005b934:	c505                	c.beqz	a0,2005b95c <rtos_sema_take+0x3a>
2005b936:	0870                	c.addi4spn	a2,sp,28
2005b938:	4581                	c.li	a1,0
2005b93a:	8522                	c.mv	a0,s0
2005b93c:	36c010ef          	jal	ra,2005cca8 <xQueueReceiveFromISR>
2005b940:	872a                	c.mv	a4,a0
2005b942:	4785                	c.li	a5,1
2005b944:	557d                	c.li	a0,-1
2005b946:	00f71763          	bne	a4,a5,2005b954 <rtos_sema_take+0x32>
2005b94a:	4572                	c.lwsp	a0,28(sp)
2005b94c:	c501                	c.beqz	a0,2005b954 <rtos_sema_take+0x32>
2005b94e:	701010ef          	jal	ra,2005d84e <vTaskSwitchContext>
2005b952:	4501                	c.li	a0,0
2005b954:	50b2                	c.lwsp	ra,44(sp)
2005b956:	5422                	c.lwsp	s0,40(sp)
2005b958:	6145                	c.addi16sp	sp,48
2005b95a:	8082                	c.jr	ra
2005b95c:	8522                	c.mv	a0,s0
2005b95e:	22c010ef          	jal	ra,2005cb8a <xQueueSemaphoreTake>
2005b962:	157d                	c.addi	a0,-1
2005b964:	00a03533          	sltu	a0,zero,a0
2005b968:	40a00533          	sub	a0,zero,a0
2005b96c:	b7e5                	c.j	2005b954 <rtos_sema_take+0x32>

2005b96e <rtos_sema_give>:
2005b96e:	7179                	c.addi16sp	sp,-48
2005b970:	d422                	c.swsp	s0,40(sp)
2005b972:	d606                	c.swsp	ra,44(sp)
2005b974:	842a                	c.mv	s0,a0
2005b976:	ce02                	c.swsp	zero,28(sp)
2005b978:	d1fff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005b97c:	c505                	c.beqz	a0,2005b9a4 <rtos_sema_give+0x36>
2005b97e:	086c                	c.addi4spn	a1,sp,28
2005b980:	8522                	c.mv	a0,s0
2005b982:	0c4010ef          	jal	ra,2005ca46 <xQueueGiveFromISR>
2005b986:	47f2                	c.lwsp	a5,28(sp)
2005b988:	c789                	c.beqz	a5,2005b992 <rtos_sema_give+0x24>
2005b98a:	c62a                	c.swsp	a0,12(sp)
2005b98c:	6c3010ef          	jal	ra,2005d84e <vTaskSwitchContext>
2005b990:	4532                	c.lwsp	a0,12(sp)
2005b992:	50b2                	c.lwsp	ra,44(sp)
2005b994:	5422                	c.lwsp	s0,40(sp)
2005b996:	157d                	c.addi	a0,-1
2005b998:	00a03533          	sltu	a0,zero,a0
2005b99c:	40a00533          	sub	a0,zero,a0
2005b9a0:	6145                	c.addi16sp	sp,48
2005b9a2:	8082                	c.jr	ra
2005b9a4:	4681                	c.li	a3,0
2005b9a6:	4601                	c.li	a2,0
2005b9a8:	4581                	c.li	a1,0
2005b9aa:	8522                	c.mv	a0,s0
2005b9ac:	72d000ef          	jal	ra,2005c8d8 <xQueueGenericSend>
2005b9b0:	b7cd                	c.j	2005b992 <rtos_sema_give+0x24>

2005b9b2 <__reserved_get_from_poll>:
2005b9b2:	7179                	c.addi16sp	sp,-48
2005b9b4:	d226                	c.swsp	s1,36(sp)
2005b9b6:	d04a                	c.swsp	s2,32(sp)
2005b9b8:	ce4e                	c.swsp	s3,28(sp)
2005b9ba:	cc52                	c.swsp	s4,24(sp)
2005b9bc:	ca56                	c.swsp	s5,20(sp)
2005b9be:	c85a                	c.swsp	s6,16(sp)
2005b9c0:	d606                	c.swsp	ra,44(sp)
2005b9c2:	d422                	c.swsp	s0,40(sp)
2005b9c4:	c65e                	c.swsp	s7,12(sp)
2005b9c6:	8a3e                	c.mv	s4,a5
2005b9c8:	421c                	c.lw	a5,0(a2)
2005b9ca:	84aa                	c.mv	s1,a0
2005b9cc:	8b2e                	c.mv	s6,a1
2005b9ce:	8ab2                	c.mv	s5,a2
2005b9d0:	8936                	c.mv	s2,a3
2005b9d2:	89ba                	c.mv	s3,a4
2005b9d4:	ef85                	c.bnez	a5,2005ba0c <__reserved_get_from_poll+0x5a>
2005b9d6:	4405                	c.li	s0,1
2005b9d8:	08851263          	bne	a0,s0,2005ba5c <__reserved_get_from_poll+0xaa>
2005b9dc:	f0418413          	addi	s0,gp,-252 # 2006688c <mutex_pool_init_flag>
2005b9e0:	401c                	c.lw	a5,0(s0)
2005b9e2:	02a78563          	beq	a5,a0,2005ba0c <__reserved_get_from_poll+0x5a>
2005b9e6:	4601                	c.li	a2,0
2005b9e8:	4581                	c.li	a1,0
2005b9ea:	5d818513          	addi	a0,gp,1496 # 20066f60 <xISRStack>
2005b9ee:	dffae097          	auipc	ra,0xdffae
2005b9f2:	0e0080e7          	jalr	ra,224(ra) # 9ace <__wrap_memset>
2005b9f6:	ee01ac23          	sw	zero,-264(gp) # 20066880 <mutex_buf_used_num>
2005b9fa:	f2818513          	addi	a0,gp,-216 # 200668b0 <wrapper_mutex_buf_list>
2005b9fe:	f001a023          	sw	zero,-256(gp) # 20066888 <mutex_max_buf_used_num>
2005ba02:	c108                	c.sw	a0,0(a0)
2005ba04:	c148                	c.sw	a0,4(a0)
2005ba06:	ee01ae23          	sw	zero,-260(gp) # 20066884 <mutex_dynamic_num>
2005ba0a:	c004                	c.sw	s1,0(s0)
2005ba0c:	c9dff0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
2005ba10:	000b2403          	lw	s0,0(s6)
2005ba14:	0a8b1b63          	bne	s6,s0,2005baca <__reserved_get_from_poll+0x118>
2005ba18:	cb5ff0ef          	jal	ra,2005b6cc <rtos_critical_exit>
2005ba1c:	4b01                	c.li	s6,0
2005ba1e:	000aa783          	lw	a5,0(s5)
2005ba22:	c791                	c.beqz	a5,2005ba2e <__reserved_get_from_poll+0x7c>
2005ba24:	00092783          	lw	a5,0(s2)
2005ba28:	0785                	c.addi	a5,1
2005ba2a:	00f92023          	sw	a5,0(s2)
2005ba2e:	00092703          	lw	a4,0(s2)
2005ba32:	0009a783          	lw	a5,0(s3)
2005ba36:	97ba                	c.add	a5,a4
2005ba38:	000a2703          	lw	a4,0(s4)
2005ba3c:	00f77463          	bgeu	a4,a5,2005ba44 <__reserved_get_from_poll+0x92>
2005ba40:	00fa2023          	sw	a5,0(s4)
2005ba44:	50b2                	c.lwsp	ra,44(sp)
2005ba46:	5422                	c.lwsp	s0,40(sp)
2005ba48:	5492                	c.lwsp	s1,36(sp)
2005ba4a:	5902                	c.lwsp	s2,32(sp)
2005ba4c:	49f2                	c.lwsp	s3,28(sp)
2005ba4e:	4a62                	c.lwsp	s4,24(sp)
2005ba50:	4ad2                	c.lwsp	s5,20(sp)
2005ba52:	4bb2                	c.lwsp	s7,12(sp)
2005ba54:	855a                	c.mv	a0,s6
2005ba56:	4b42                	c.lwsp	s6,16(sp)
2005ba58:	6145                	c.addi16sp	sp,48
2005ba5a:	8082                	c.jr	ra
2005ba5c:	4789                	c.li	a5,2
2005ba5e:	02f51d63          	bne	a0,a5,2005ba98 <__reserved_get_from_poll+0xe6>
2005ba62:	f1418b93          	addi	s7,gp,-236 # 2006689c <sema_pool_init_flag>
2005ba66:	000ba783          	lw	a5,0(s7)
2005ba6a:	fa8781e3          	beq	a5,s0,2005ba0c <__reserved_get_from_poll+0x5a>
2005ba6e:	4601                	c.li	a2,0
2005ba70:	4581                	c.li	a1,0
2005ba72:	5d818513          	addi	a0,gp,1496 # 20066f60 <xISRStack>
2005ba76:	dffae097          	auipc	ra,0xdffae
2005ba7a:	058080e7          	jalr	ra,88(ra) # 9ace <__wrap_memset>
2005ba7e:	f001a423          	sw	zero,-248(gp) # 20066890 <sema_buf_used_num>
2005ba82:	f3018513          	addi	a0,gp,-208 # 200668b8 <wrapper_sema_buf_list>
2005ba86:	f001a823          	sw	zero,-240(gp) # 20066898 <sema_max_buf_used_num>
2005ba8a:	c108                	c.sw	a0,0(a0)
2005ba8c:	c148                	c.sw	a0,4(a0)
2005ba8e:	f001a623          	sw	zero,-244(gp) # 20066894 <sema_dynamic_num>
2005ba92:	008ba023          	sw	s0,0(s7)
2005ba96:	bf9d                	c.j	2005ba0c <__reserved_get_from_poll+0x5a>
2005ba98:	f2418b93          	addi	s7,gp,-220 # 200668ac <timer_pool_init_flag>
2005ba9c:	000ba783          	lw	a5,0(s7)
2005baa0:	f68786e3          	beq	a5,s0,2005ba0c <__reserved_get_from_poll+0x5a>
2005baa4:	4601                	c.li	a2,0
2005baa6:	4581                	c.li	a1,0
2005baa8:	5d818513          	addi	a0,gp,1496 # 20066f60 <xISRStack>
2005baac:	dffae097          	auipc	ra,0xdffae
2005bab0:	022080e7          	jalr	ra,34(ra) # 9ace <__wrap_memset>
2005bab4:	f001ac23          	sw	zero,-232(gp) # 200668a0 <timer_buf_used_num>
2005bab8:	f3818513          	addi	a0,gp,-200 # 200668c0 <wrapper_timer_buf_list>
2005babc:	f201a023          	sw	zero,-224(gp) # 200668a8 <timer_max_buf_used_num>
2005bac0:	c108                	c.sw	a0,0(a0)
2005bac2:	c148                	c.sw	a0,4(a0)
2005bac4:	f001ae23          	sw	zero,-228(gp) # 200668a4 <timer_dynamic_num>
2005bac8:	b7e9                	c.j	2005ba92 <__reserved_get_from_poll+0xe0>
2005baca:	405c                	c.lw	a5,4(s0)
2005bacc:	4018                	c.lw	a4,0(s0)
2005bace:	00840b13          	addi	s6,s0,8
2005bad2:	c35c                	c.sw	a5,4(a4)
2005bad4:	c398                	c.sw	a4,0(a5)
2005bad6:	c000                	c.sw	s0,0(s0)
2005bad8:	c040                	c.sw	s0,4(s0)
2005bada:	0009a783          	lw	a5,0(s3)
2005bade:	0785                	c.addi	a5,1
2005bae0:	00f9a023          	sw	a5,0(s3)
2005bae4:	be9ff0ef          	jal	ra,2005b6cc <rtos_critical_exit>
2005bae8:	57e1                	c.li	a5,-8
2005baea:	f2f40ae3          	beq	s0,a5,2005ba1e <__reserved_get_from_poll+0x6c>
2005baee:	478d                	c.li	a5,3
2005baf0:	05000613          	addi	a2,zero,80
2005baf4:	00f49463          	bne	s1,a5,2005bafc <__reserved_get_from_poll+0x14a>
2005baf8:	02c00613          	addi	a2,zero,44
2005bafc:	4581                	c.li	a1,0
2005bafe:	855a                	c.mv	a0,s6
2005bb00:	dffae097          	auipc	ra,0xdffae
2005bb04:	fce080e7          	jalr	ra,-50(ra) # 9ace <__wrap_memset>
2005bb08:	b71d                	c.j	2005ba2e <__reserved_get_from_poll+0x7c>

2005bb0a <__reserved_get_mutex_from_poll>:
2005bb0a:	f0018793          	addi	a5,gp,-256 # 20066888 <mutex_max_buf_used_num>
2005bb0e:	ef818713          	addi	a4,gp,-264 # 20066880 <mutex_buf_used_num>
2005bb12:	efc18693          	addi	a3,gp,-260 # 20066884 <mutex_dynamic_num>
2005bb16:	f0418613          	addi	a2,gp,-252 # 2006688c <mutex_pool_init_flag>
2005bb1a:	f2818593          	addi	a1,gp,-216 # 200668b0 <wrapper_mutex_buf_list>
2005bb1e:	4505                	c.li	a0,1
2005bb20:	e93ff06f          	jal	zero,2005b9b2 <__reserved_get_from_poll>

2005bb24 <__reserved_release_mutex_to_poll>:
2005bb24:	1141                	c.addi	sp,-16
2005bb26:	c606                	c.swsp	ra,12(sp)
2005bb28:	b81ff0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
2005bb2c:	efc18793          	addi	a5,gp,-260 # 20066884 <mutex_dynamic_num>
2005bb30:	4398                	c.lw	a4,0(a5)
2005bb32:	177d                	c.addi	a4,-1
2005bb34:	c398                	c.sw	a4,0(a5)
2005bb36:	40b2                	c.lwsp	ra,12(sp)
2005bb38:	0141                	c.addi	sp,16
2005bb3a:	b93ff06f          	jal	zero,2005b6cc <rtos_critical_exit>

2005bb3e <__reserved_get_sema_from_poll>:
2005bb3e:	f1018793          	addi	a5,gp,-240 # 20066898 <sema_max_buf_used_num>
2005bb42:	f0818713          	addi	a4,gp,-248 # 20066890 <sema_buf_used_num>
2005bb46:	f0c18693          	addi	a3,gp,-244 # 20066894 <sema_dynamic_num>
2005bb4a:	f1418613          	addi	a2,gp,-236 # 2006689c <sema_pool_init_flag>
2005bb4e:	f3018593          	addi	a1,gp,-208 # 200668b8 <wrapper_sema_buf_list>
2005bb52:	4509                	c.li	a0,2
2005bb54:	e5fff06f          	jal	zero,2005b9b2 <__reserved_get_from_poll>

2005bb58 <__reserved_get_timer_from_poll>:
2005bb58:	f2018793          	addi	a5,gp,-224 # 200668a8 <timer_max_buf_used_num>
2005bb5c:	f1818713          	addi	a4,gp,-232 # 200668a0 <timer_buf_used_num>
2005bb60:	f1c18693          	addi	a3,gp,-228 # 200668a4 <timer_dynamic_num>
2005bb64:	f2418613          	addi	a2,gp,-220 # 200668ac <timer_pool_init_flag>
2005bb68:	f3818593          	addi	a1,gp,-200 # 200668c0 <wrapper_timer_buf_list>
2005bb6c:	450d                	c.li	a0,3
2005bb6e:	e45ff06f          	jal	zero,2005b9b2 <__reserved_get_from_poll>

2005bb72 <__reserved_release_timer_to_poll>:
2005bb72:	1141                	c.addi	sp,-16
2005bb74:	c606                	c.swsp	ra,12(sp)
2005bb76:	b33ff0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
2005bb7a:	f1c18793          	addi	a5,gp,-228 # 200668a4 <timer_dynamic_num>
2005bb7e:	4398                	c.lw	a4,0(a5)
2005bb80:	177d                	c.addi	a4,-1
2005bb82:	c398                	c.sw	a4,0(a5)
2005bb84:	40b2                	c.lwsp	ra,12(sp)
2005bb86:	0141                	c.addi	sp,16
2005bb88:	b45ff06f          	jal	zero,2005b6cc <rtos_critical_exit>

2005bb8c <rtos_sched_start>:
2005bb8c:	1141                	c.addi	sp,-16
2005bb8e:	c606                	c.swsp	ra,12(sp)
2005bb90:	202020ef          	jal	ra,2005dd92 <vTaskStartScheduler>
2005bb94:	40b2                	c.lwsp	ra,12(sp)
2005bb96:	4501                	c.li	a0,0
2005bb98:	0141                	c.addi	sp,16
2005bb9a:	8082                	c.jr	ra

2005bb9c <rtos_sched_get_state>:
2005bb9c:	1141                	c.addi	sp,-16
2005bb9e:	c606                	c.swsp	ra,12(sp)
2005bba0:	699010ef          	jal	ra,2005da38 <xTaskGetSchedulerState>
2005bba4:	4785                	c.li	a5,1
2005bba6:	00f50b63          	beq	a0,a5,2005bbbc <rtos_sched_get_state+0x20>
2005bbaa:	68f010ef          	jal	ra,2005da38 <xTaskGetSchedulerState>
2005bbae:	c519                	c.beqz	a0,2005bbbc <rtos_sched_get_state+0x20>
2005bbb0:	689010ef          	jal	ra,2005da38 <xTaskGetSchedulerState>
2005bbb4:	4789                	c.li	a5,2
2005bbb6:	00f50363          	beq	a0,a5,2005bbbc <rtos_sched_get_state+0x20>
2005bbba:	557d                	c.li	a0,-1
2005bbbc:	40b2                	c.lwsp	ra,12(sp)
2005bbbe:	0141                	c.addi	sp,16
2005bbc0:	8082                	c.jr	ra

2005bbc2 <rtos_task_create>:
2005bbc2:	882a                	c.mv	a6,a0
2005bbc4:	8532                	c.mv	a0,a2
2005bbc6:	863a                	c.mv	a2,a4
2005bbc8:	1141                	c.addi	sp,-16
2005bbca:	873e                	c.mv	a4,a5
2005bbcc:	8209                	c.srli	a2,0x2
2005bbce:	87c2                	c.mv	a5,a6
2005bbd0:	c606                	c.swsp	ra,12(sp)
2005bbd2:	228020ef          	jal	ra,2005ddfa <xTaskCreate>
2005bbd6:	40b2                	c.lwsp	ra,12(sp)
2005bbd8:	157d                	c.addi	a0,-1
2005bbda:	00a03533          	sltu	a0,zero,a0
2005bbde:	40a00533          	sub	a0,zero,a0
2005bbe2:	0141                	c.addi	sp,16
2005bbe4:	8082                	c.jr	ra

2005bbe6 <rtos_task_delete>:
2005bbe6:	1141                	c.addi	sp,-16
2005bbe8:	c606                	c.swsp	ra,12(sp)
2005bbea:	288020ef          	jal	ra,2005de72 <vTaskDelete>
2005bbee:	40b2                	c.lwsp	ra,12(sp)
2005bbf0:	4501                	c.li	a0,0
2005bbf2:	0141                	c.addi	sp,16
2005bbf4:	8082                	c.jr	ra

2005bbf6 <rtos_create_secure_context>:
2005bbf6:	8082                	c.jr	ra

2005bbf8 <rtos_time_delay_ms>:
2005bbf8:	1141                	c.addi	sp,-16
2005bbfa:	c422                	c.swsp	s0,8(sp)
2005bbfc:	c606                	c.swsp	ra,12(sp)
2005bbfe:	842a                	c.mv	s0,a0
2005bc00:	bf7fd0ef          	jal	ra,200597f6 <pmu_yield_os_check>
2005bc04:	cd19                	c.beqz	a0,2005bc22 <rtos_time_delay_ms+0x2a>
2005bc06:	633010ef          	jal	ra,2005da38 <xTaskGetSchedulerState>
2005bc0a:	4789                	c.li	a5,2
2005bc0c:	00f51b63          	bne	a0,a5,2005bc22 <rtos_time_delay_ms+0x2a>
2005bc10:	a87ff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005bc14:	e519                	c.bnez	a0,2005bc22 <rtos_time_delay_ms+0x2a>
2005bc16:	8522                	c.mv	a0,s0
2005bc18:	4422                	c.lwsp	s0,8(sp)
2005bc1a:	40b2                	c.lwsp	ra,12(sp)
2005bc1c:	0141                	c.addi	sp,16
2005bc1e:	46e0206f          	jal	zero,2005e08c <vTaskDelay>
2005bc22:	8522                	c.mv	a0,s0
2005bc24:	4422                	c.lwsp	s0,8(sp)
2005bc26:	40b2                	c.lwsp	ra,12(sp)
2005bc28:	0141                	c.addi	sp,16
2005bc2a:	dffa6317          	auipc	t1,0xdffa6
2005bc2e:	6f030067          	jalr	zero,1776(t1) # 231a <DelayMs>

2005bc32 <rtos_time_get_current_system_time_ms>:
2005bc32:	1141                	c.addi	sp,-16
2005bc34:	c606                	c.swsp	ra,12(sp)
2005bc36:	a61ff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005bc3a:	c509                	c.beqz	a0,2005bc44 <rtos_time_get_current_system_time_ms+0x12>
2005bc3c:	40b2                	c.lwsp	ra,12(sp)
2005bc3e:	0141                	c.addi	sp,16
2005bc40:	2c10106f          	jal	zero,2005d700 <xTaskGetTickCountFromISR>
2005bc44:	bb3fd0ef          	jal	ra,200597f6 <pmu_yield_os_check>
2005bc48:	d975                	c.beqz	a0,2005bc3c <rtos_time_get_current_system_time_ms+0xa>
2005bc4a:	40b2                	c.lwsp	ra,12(sp)
2005bc4c:	0141                	c.addi	sp,16
2005bc4e:	6e40206f          	jal	zero,2005e332 <xTaskGetTickCount>

2005bc52 <rtos_timer_create>:
2005bc52:	1141                	c.addi	sp,-16
2005bc54:	c422                	c.swsp	s0,8(sp)
2005bc56:	c606                	c.swsp	ra,12(sp)
2005bc58:	842a                	c.mv	s0,a0
2005bc5a:	587d                	c.li	a6,-1
2005bc5c:	c105                	c.beqz	a0,2005bc7c <rtos_timer_create+0x2a>
2005bc5e:	88b2                	c.mv	a7,a2
2005bc60:	587d                	c.li	a6,-1
2005bc62:	863a                	c.mv	a2,a4
2005bc64:	873e                	c.mv	a4,a5
2005bc66:	cb99                	c.beqz	a5,2005bc7c <rtos_timer_create+0x2a>
2005bc68:	852e                	c.mv	a0,a1
2005bc6a:	85b6                	c.mv	a1,a3
2005bc6c:	86c6                	c.mv	a3,a7
2005bc6e:	10b020ef          	jal	ra,2005e578 <xTimerCreate>
2005bc72:	c008                	c.sw	a0,0(s0)
2005bc74:	00153513          	sltiu	a0,a0,1
2005bc78:	40a00833          	sub	a6,zero,a0
2005bc7c:	40b2                	c.lwsp	ra,12(sp)
2005bc7e:	4422                	c.lwsp	s0,8(sp)
2005bc80:	8542                	c.mv	a0,a6
2005bc82:	0141                	c.addi	sp,16
2005bc84:	8082                	c.jr	ra

2005bc86 <rtos_timer_create_static>:
2005bc86:	1101                	c.addi	sp,-32
2005bc88:	cc22                	c.swsp	s0,24(sp)
2005bc8a:	ca26                	c.swsp	s1,20(sp)
2005bc8c:	c84a                	c.swsp	s2,16(sp)
2005bc8e:	84ae                	c.mv	s1,a1
2005bc90:	c632                	c.swsp	a2,12(sp)
2005bc92:	893a                	c.mv	s2,a4
2005bc94:	ce06                	c.swsp	ra,28(sp)
2005bc96:	842a                	c.mv	s0,a0
2005bc98:	c436                	c.swsp	a3,8(sp)
2005bc9a:	c23e                	c.swsp	a5,4(sp)
2005bc9c:	ebdff0ef          	jal	ra,2005bb58 <__reserved_get_timer_from_poll>
2005bca0:	4712                	c.lwsp	a4,4(sp)
2005bca2:	45a2                	c.lwsp	a1,8(sp)
2005bca4:	4632                	c.lwsp	a2,12(sp)
2005bca6:	ed09                	c.bnez	a0,2005bcc0 <rtos_timer_create_static+0x3a>
2005bca8:	8522                	c.mv	a0,s0
2005bcaa:	4462                	c.lwsp	s0,24(sp)
2005bcac:	40f2                	c.lwsp	ra,28(sp)
2005bcae:	87ba                	c.mv	a5,a4
2005bcb0:	86ae                	c.mv	a3,a1
2005bcb2:	874a                	c.mv	a4,s2
2005bcb4:	85a6                	c.mv	a1,s1
2005bcb6:	4942                	c.lwsp	s2,16(sp)
2005bcb8:	44d2                	c.lwsp	s1,20(sp)
2005bcba:	6105                	c.addi16sp	sp,32
2005bcbc:	f97ff06f          	jal	zero,2005bc52 <rtos_timer_create>
2005bcc0:	87aa                	c.mv	a5,a0
2005bcc2:	86b2                	c.mv	a3,a2
2005bcc4:	8526                	c.mv	a0,s1
2005bcc6:	864a                	c.mv	a2,s2
2005bcc8:	119020ef          	jal	ra,2005e5e0 <xTimerCreateStatic>
2005bccc:	c008                	c.sw	a0,0(s0)
2005bcce:	40f2                	c.lwsp	ra,28(sp)
2005bcd0:	4462                	c.lwsp	s0,24(sp)
2005bcd2:	00153513          	sltiu	a0,a0,1
2005bcd6:	44d2                	c.lwsp	s1,20(sp)
2005bcd8:	4942                	c.lwsp	s2,16(sp)
2005bcda:	40a00533          	sub	a0,zero,a0
2005bcde:	6105                	c.addi16sp	sp,32
2005bce0:	8082                	c.jr	ra

2005bce2 <rtos_timer_delete>:
2005bce2:	c10d                	c.beqz	a0,2005bd04 <rtos_timer_delete+0x22>
2005bce4:	1141                	c.addi	sp,-16
2005bce6:	872e                	c.mv	a4,a1
2005bce8:	4681                	c.li	a3,0
2005bcea:	4601                	c.li	a2,0
2005bcec:	4595                	c.li	a1,5
2005bcee:	c606                	c.swsp	ra,12(sp)
2005bcf0:	153020ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005bcf4:	40b2                	c.lwsp	ra,12(sp)
2005bcf6:	157d                	c.addi	a0,-1
2005bcf8:	00a03533          	sltu	a0,zero,a0
2005bcfc:	40a00533          	sub	a0,zero,a0
2005bd00:	0141                	c.addi	sp,16
2005bd02:	8082                	c.jr	ra
2005bd04:	557d                	c.li	a0,-1
2005bd06:	8082                	c.jr	ra

2005bd08 <rtos_timer_delete_static>:
2005bd08:	1141                	c.addi	sp,-16
2005bd0a:	c422                	c.swsp	s0,8(sp)
2005bd0c:	c226                	c.swsp	s1,4(sp)
2005bd0e:	c606                	c.swsp	ra,12(sp)
2005bd10:	842a                	c.mv	s0,a0
2005bd12:	84ae                	c.mv	s1,a1
2005bd14:	85a6                	c.mv	a1,s1
2005bd16:	8522                	c.mv	a0,s0
2005bd18:	fcbff0ef          	jal	ra,2005bce2 <rtos_timer_delete>
2005bd1c:	fd65                	c.bnez	a0,2005bd14 <rtos_timer_delete_static+0xc>
2005bd1e:	8522                	c.mv	a0,s0
2005bd20:	e53ff0ef          	jal	ra,2005bb72 <__reserved_release_timer_to_poll>
2005bd24:	40b2                	c.lwsp	ra,12(sp)
2005bd26:	4422                	c.lwsp	s0,8(sp)
2005bd28:	4492                	c.lwsp	s1,4(sp)
2005bd2a:	4501                	c.li	a0,0
2005bd2c:	0141                	c.addi	sp,16
2005bd2e:	8082                	c.jr	ra

2005bd30 <rtos_timer_start>:
2005bd30:	e909                	c.bnez	a0,2005bd42 <rtos_timer_start+0x12>
2005bd32:	557d                	c.li	a0,-1
2005bd34:	8082                	c.jr	ra
2005bd36:	557d                	c.li	a0,-1
2005bd38:	40f2                	c.lwsp	ra,28(sp)
2005bd3a:	4462                	c.lwsp	s0,24(sp)
2005bd3c:	44d2                	c.lwsp	s1,20(sp)
2005bd3e:	6105                	c.addi16sp	sp,32
2005bd40:	8082                	c.jr	ra
2005bd42:	1101                	c.addi	sp,-32
2005bd44:	cc22                	c.swsp	s0,24(sp)
2005bd46:	ca26                	c.swsp	s1,20(sp)
2005bd48:	ce06                	c.swsp	ra,28(sp)
2005bd4a:	842a                	c.mv	s0,a0
2005bd4c:	84ae                	c.mv	s1,a1
2005bd4e:	949ff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005bd52:	c505                	c.beqz	a0,2005bd7a <rtos_timer_start+0x4a>
2005bd54:	c602                	c.swsp	zero,12(sp)
2005bd56:	1ab010ef          	jal	ra,2005d700 <xTaskGetTickCountFromISR>
2005bd5a:	862a                	c.mv	a2,a0
2005bd5c:	4701                	c.li	a4,0
2005bd5e:	0074                	c.addi4spn	a3,sp,12
2005bd60:	4599                	c.li	a1,6
2005bd62:	8522                	c.mv	a0,s0
2005bd64:	0df020ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005bd68:	4785                	c.li	a5,1
2005bd6a:	fcf516e3          	bne	a0,a5,2005bd36 <rtos_timer_start+0x6>
2005bd6e:	4532                	c.lwsp	a0,12(sp)
2005bd70:	d561                	c.beqz	a0,2005bd38 <rtos_timer_start+0x8>
2005bd72:	2dd010ef          	jal	ra,2005d84e <vTaskSwitchContext>
2005bd76:	4501                	c.li	a0,0
2005bd78:	b7c1                	c.j	2005bd38 <rtos_timer_start+0x8>
2005bd7a:	5b8020ef          	jal	ra,2005e332 <xTaskGetTickCount>
2005bd7e:	862a                	c.mv	a2,a0
2005bd80:	8726                	c.mv	a4,s1
2005bd82:	4681                	c.li	a3,0
2005bd84:	4585                	c.li	a1,1
2005bd86:	8522                	c.mv	a0,s0
2005bd88:	0bb020ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005bd8c:	157d                	c.addi	a0,-1
2005bd8e:	00a03533          	sltu	a0,zero,a0
2005bd92:	40a00533          	sub	a0,zero,a0
2005bd96:	b74d                	c.j	2005bd38 <rtos_timer_start+0x8>

2005bd98 <rtos_timer_stop>:
2005bd98:	e901                	c.bnez	a0,2005bda8 <rtos_timer_stop+0x10>
2005bd9a:	557d                	c.li	a0,-1
2005bd9c:	8082                	c.jr	ra
2005bd9e:	557d                	c.li	a0,-1
2005bda0:	50b2                	c.lwsp	ra,44(sp)
2005bda2:	5422                	c.lwsp	s0,40(sp)
2005bda4:	6145                	c.addi16sp	sp,48
2005bda6:	8082                	c.jr	ra
2005bda8:	7179                	c.addi16sp	sp,-48
2005bdaa:	d422                	c.swsp	s0,40(sp)
2005bdac:	d606                	c.swsp	ra,44(sp)
2005bdae:	842a                	c.mv	s0,a0
2005bdb0:	c62e                	c.swsp	a1,12(sp)
2005bdb2:	8e5ff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005bdb6:	4732                	c.lwsp	a4,12(sp)
2005bdb8:	c115                	c.beqz	a0,2005bddc <rtos_timer_stop+0x44>
2005bdba:	4701                	c.li	a4,0
2005bdbc:	0874                	c.addi4spn	a3,sp,28
2005bdbe:	4601                	c.li	a2,0
2005bdc0:	45a1                	c.li	a1,8
2005bdc2:	8522                	c.mv	a0,s0
2005bdc4:	ce02                	c.swsp	zero,28(sp)
2005bdc6:	07d020ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005bdca:	4785                	c.li	a5,1
2005bdcc:	fcf519e3          	bne	a0,a5,2005bd9e <rtos_timer_stop+0x6>
2005bdd0:	4572                	c.lwsp	a0,28(sp)
2005bdd2:	d579                	c.beqz	a0,2005bda0 <rtos_timer_stop+0x8>
2005bdd4:	27b010ef          	jal	ra,2005d84e <vTaskSwitchContext>
2005bdd8:	4501                	c.li	a0,0
2005bdda:	b7d9                	c.j	2005bda0 <rtos_timer_stop+0x8>
2005bddc:	4681                	c.li	a3,0
2005bdde:	4601                	c.li	a2,0
2005bde0:	458d                	c.li	a1,3
2005bde2:	8522                	c.mv	a0,s0
2005bde4:	05f020ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005bde8:	157d                	c.addi	a0,-1
2005bdea:	00a03533          	sltu	a0,zero,a0
2005bdee:	40a00533          	sub	a0,zero,a0
2005bdf2:	b77d                	c.j	2005bda0 <rtos_timer_stop+0x8>

2005bdf4 <rtos_timer_change_period>:
2005bdf4:	e909                	c.bnez	a0,2005be06 <rtos_timer_change_period+0x12>
2005bdf6:	557d                	c.li	a0,-1
2005bdf8:	8082                	c.jr	ra
2005bdfa:	557d                	c.li	a0,-1
2005bdfc:	50b2                	c.lwsp	ra,44(sp)
2005bdfe:	5422                	c.lwsp	s0,40(sp)
2005be00:	5492                	c.lwsp	s1,36(sp)
2005be02:	6145                	c.addi16sp	sp,48
2005be04:	8082                	c.jr	ra
2005be06:	7179                	c.addi16sp	sp,-48
2005be08:	d422                	c.swsp	s0,40(sp)
2005be0a:	d226                	c.swsp	s1,36(sp)
2005be0c:	d606                	c.swsp	ra,44(sp)
2005be0e:	842a                	c.mv	s0,a0
2005be10:	84ae                	c.mv	s1,a1
2005be12:	c632                	c.swsp	a2,12(sp)
2005be14:	883ff0ef          	jal	ra,2005b696 <rtos_critical_is_in_interrupt>
2005be18:	4732                	c.lwsp	a4,12(sp)
2005be1a:	c115                	c.beqz	a0,2005be3e <rtos_timer_change_period+0x4a>
2005be1c:	4701                	c.li	a4,0
2005be1e:	0874                	c.addi4spn	a3,sp,28
2005be20:	8626                	c.mv	a2,s1
2005be22:	45a5                	c.li	a1,9
2005be24:	8522                	c.mv	a0,s0
2005be26:	ce02                	c.swsp	zero,28(sp)
2005be28:	01b020ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005be2c:	4785                	c.li	a5,1
2005be2e:	fcf516e3          	bne	a0,a5,2005bdfa <rtos_timer_change_period+0x6>
2005be32:	4572                	c.lwsp	a0,28(sp)
2005be34:	d561                	c.beqz	a0,2005bdfc <rtos_timer_change_period+0x8>
2005be36:	219010ef          	jal	ra,2005d84e <vTaskSwitchContext>
2005be3a:	4501                	c.li	a0,0
2005be3c:	b7c1                	c.j	2005bdfc <rtos_timer_change_period+0x8>
2005be3e:	4681                	c.li	a3,0
2005be40:	8626                	c.mv	a2,s1
2005be42:	4591                	c.li	a1,4
2005be44:	8522                	c.mv	a0,s0
2005be46:	7fc020ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005be4a:	157d                	c.addi	a0,-1
2005be4c:	00a03533          	sltu	a0,zero,a0
2005be50:	40a00533          	sub	a0,zero,a0
2005be54:	b765                	c.j	2005bdfc <rtos_timer_change_period+0x8>

2005be56 <rtos_timer_is_timer_active>:
2005be56:	2cf0206f          	jal	zero,2005e924 <xTimerIsTimerActive>

2005be5a <platform_calloc_uninit>:
2005be5a:	4501                	c.li	a0,0
2005be5c:	8082                	c.jr	ra

2005be5e <platform_free_uninit>:
2005be5e:	8082                	c.jr	ra

2005be60 <mbedtls_platform_set_calloc_free>:
2005be60:	e8a1a023          	sw	a0,-384(gp) # 20066808 <mbedtls_calloc_func>
2005be64:	e8b1a223          	sw	a1,-380(gp) # 2006680c <mbedtls_free_func>
2005be68:	200017b7          	lui	a5,0x20001
2005be6c:	c6c78793          	addi	a5,a5,-916 # 20000c6c <rom_ssl_ram_map>
2005be70:	20001737          	lui	a4,0x20001
2005be74:	c388                	c.sw	a0,0(a5)
2005be76:	c6f72423          	sw	a5,-920(a4) # 20000c68 <p_rom_ssl_ram_map>
2005be7a:	c3cc                	c.sw	a1,4(a5)
2005be7c:	4501                	c.li	a0,0
2005be7e:	8082                	c.jr	ra

2005be80 <freertos_risc_v_trap_handler>:
2005be80:	ef810113          	addi	sp,sp,-264
2005be84:	c206                	c.swsp	ra,4(sp)
2005be86:	ca16                	c.swsp	t0,20(sp)
2005be88:	cc1a                	c.swsp	t1,24(sp)
2005be8a:	ce1e                	c.swsp	t2,28(sp)
2005be8c:	d022                	c.swsp	s0,32(sp)
2005be8e:	d226                	c.swsp	s1,36(sp)
2005be90:	d42a                	c.swsp	a0,40(sp)
2005be92:	d62e                	c.swsp	a1,44(sp)
2005be94:	d832                	c.swsp	a2,48(sp)
2005be96:	da36                	c.swsp	a3,52(sp)
2005be98:	dc3a                	c.swsp	a4,56(sp)
2005be9a:	de3e                	c.swsp	a5,60(sp)
2005be9c:	c0c2                	c.swsp	a6,64(sp)
2005be9e:	c2c6                	c.swsp	a7,68(sp)
2005bea0:	c4ca                	c.swsp	s2,72(sp)
2005bea2:	c6ce                	c.swsp	s3,76(sp)
2005bea4:	c8d2                	c.swsp	s4,80(sp)
2005bea6:	cad6                	c.swsp	s5,84(sp)
2005bea8:	ccda                	c.swsp	s6,88(sp)
2005beaa:	cede                	c.swsp	s7,92(sp)
2005beac:	d0e2                	c.swsp	s8,96(sp)
2005beae:	d2e6                	c.swsp	s9,100(sp)
2005beb0:	d4ea                	c.swsp	s10,104(sp)
2005beb2:	d6ee                	c.swsp	s11,108(sp)
2005beb4:	d8f2                	c.swsp	t3,112(sp)
2005beb6:	daf6                	c.swsp	t4,116(sp)
2005beb8:	dcfa                	c.swsp	t5,120(sp)
2005beba:	defe                	c.swsp	t6,124(sp)
2005bebc:	300022f3          	csrrs	t0,mstatus,zero
2005bec0:	6319                	c.lui	t1,0x6
2005bec2:	0062f2b3          	and	t0,t0,t1
2005bec6:	6309                	c.lui	t1,0x2
2005bec8:	04628663          	beq	t0,t1,2005bf14 <freertos_risc_v_trap_handler+0x94>
2005becc:	003022f3          	csrrs	t0,fcsr,zero
2005bed0:	e102                	c.fswsp	ft0,128(sp)
2005bed2:	e306                	c.fswsp	ft1,132(sp)
2005bed4:	e50a                	c.fswsp	ft2,136(sp)
2005bed6:	e70e                	c.fswsp	ft3,140(sp)
2005bed8:	e912                	c.fswsp	ft4,144(sp)
2005beda:	eb16                	c.fswsp	ft5,148(sp)
2005bedc:	ed1a                	c.fswsp	ft6,152(sp)
2005bede:	ef1e                	c.fswsp	ft7,156(sp)
2005bee0:	f122                	c.fswsp	fs0,160(sp)
2005bee2:	f326                	c.fswsp	fs1,164(sp)
2005bee4:	f52a                	c.fswsp	fa0,168(sp)
2005bee6:	f72e                	c.fswsp	fa1,172(sp)
2005bee8:	f932                	c.fswsp	fa2,176(sp)
2005beea:	fb36                	c.fswsp	fa3,180(sp)
2005beec:	fd3a                	c.fswsp	fa4,184(sp)
2005beee:	ff3e                	c.fswsp	fa5,188(sp)
2005bef0:	e1c2                	c.fswsp	fa6,192(sp)
2005bef2:	e3c6                	c.fswsp	fa7,196(sp)
2005bef4:	e5ca                	c.fswsp	fs2,200(sp)
2005bef6:	e7ce                	c.fswsp	fs3,204(sp)
2005bef8:	e9d2                	c.fswsp	fs4,208(sp)
2005befa:	ebd6                	c.fswsp	fs5,212(sp)
2005befc:	edda                	c.fswsp	fs6,216(sp)
2005befe:	efde                	c.fswsp	fs7,220(sp)
2005bf00:	f1e2                	c.fswsp	fs8,224(sp)
2005bf02:	f3e6                	c.fswsp	fs9,228(sp)
2005bf04:	f5ea                	c.fswsp	fs10,232(sp)
2005bf06:	f7ee                	c.fswsp	fs11,236(sp)
2005bf08:	f9f2                	c.fswsp	ft8,240(sp)
2005bf0a:	fbf6                	c.fswsp	ft9,244(sp)
2005bf0c:	fdfa                	c.fswsp	ft10,248(sp)
2005bf0e:	fffe                	c.fswsp	ft11,252(sp)
2005bf10:	10512023          	sw	t0,256(sp)
2005bf14:	300022f3          	csrrs	t0,mstatus,zero
2005bf18:	10512223          	sw	t0,260(sp)
2005bf1c:	34202573          	csrrs	a0,mcause,zero
2005bf20:	341025f3          	csrrs	a1,mepc,zero

2005bf24 <test_if_asynchronous>:
2005bf24:	01f55613          	srli	a2,a0,0x1f
2005bf28:	e225                	c.bnez	a2,2005bf88 <handle_asynchronous>

2005bf2a <handle_synchronous>:
2005bf2a:	0591                	c.addi	a1,4
2005bf2c:	c02e                	c.swsp	a1,0(sp)

2005bf2e <test_if_environment_call>:
2005bf2e:	42ad                	c.li	t0,11
2005bf30:	00a2cd63          	blt	t0,a0,2005bf4a <_crash_dump>
2005bf34:	42a1                	c.li	t0,8
2005bf36:	00554a63          	blt	a0,t0,2005bf4a <_crash_dump>
2005bf3a:	a009                	c.j	2005bf3c <ecall_yield>

2005bf3c <ecall_yield>:
2005bf3c:	f5c1a283          	lw	t0,-164(gp) # 200668e4 <pxCurrentTCB>
2005bf40:	0022a023          	sw	sp,0(t0)
2005bf44:	e5c1a103          	lw	sp,-420(gp) # 200667e4 <xISRStackTop>
2005bf48:	a0a5                	c.j	2005bfb0 <task_context_switch>

2005bf4a <_crash_dump>:
2005bf4a:	34102573          	csrrs	a0,mepc,zero
2005bf4e:	022c                	c.addi4spn	a1,sp,264
2005bf50:	860a                	c.mv	a2,sp
2005bf52:	c002                	c.swsp	zero,0(sp)
2005bf54:	c42e                	c.swsp	a1,8(sp)
2005bf56:	c60e                	c.swsp	gp,12(sp)
2005bf58:	c812                	c.swsp	tp,16(sp)
2005bf5a:	e5c1a303          	lw	t1,-420(gp) # 200667e4 <xISRStackTop>
2005bf5e:	0000b397          	auipc	t2,0xb
2005bf62:	00238393          	addi	t2,t2,2 # 20066f60 <xISRStack>
2005bf66:	08038393          	addi	t2,t2,128
2005bf6a:	00615563          	bge	sp,t1,2005bf74 <not_isrstack>
2005bf6e:	00714363          	blt	sp,t2,2005bf74 <not_isrstack>
2005bf72:	a019                	c.j	2005bf78 <is_isrstack>

2005bf74 <not_isrstack>:
2005bf74:	e5c1a103          	lw	sp,-420(gp) # 200667e4 <xISRStackTop>

2005bf78 <is_isrstack>:
2005bf78:	00004297          	auipc	t0,0x4
2005bf7c:	0ae28293          	addi	t0,t0,174 # 20060026 <crash_dump>
2005bf80:	9282                	c.jalr	t0

2005bf82 <crash_dump_end>:
2005bf82:	a001                	c.j	2005bf82 <crash_dump_end>

2005bf84 <unrecoverable_error>:
2005bf84:	9002                	c.ebreak
2005bf86:	bffd                	c.j	2005bf84 <unrecoverable_error>

2005bf88 <handle_asynchronous>:
2005bf88:	c02e                	c.swsp	a1,0(sp)
2005bf8a:	f5c1a283          	lw	t0,-164(gp) # 200668e4 <pxCurrentTCB>
2005bf8e:	0022a023          	sw	sp,0(t0)
2005bf92:	e5c1a103          	lw	sp,-420(gp) # 200667e4 <xISRStackTop>

2005bf96 <nonvec_handle>:
2005bf96:	4285                	c.li	t0,1
2005bf98:	02fe                	c.slli	t0,0x1f
2005bf9a:	00728313          	addi	t1,t0,7
2005bf9e:	00651763          	bne	a0,t1,2005bfac <test_if_external_interrupt>
2005bfa2:	28d9                	c.jal	2005c078 <machine_timer_update>
2005bfa4:	774010ef          	jal	ra,2005d718 <xTaskIncrementTick>
2005bfa8:	c909                	c.beqz	a0,2005bfba <processed_source>
2005bfaa:	a019                	c.j	2005bfb0 <task_context_switch>

2005bfac <test_if_external_interrupt>:
2005bfac:	2211                	c.jal	2005c0b0 <vPortHandleInterrupt>
2005bfae:	a031                	c.j	2005bfba <processed_source>

2005bfb0 <task_context_switch>:
2005bfb0:	09f010ef          	jal	ra,2005d84e <vTaskSwitchContext>
2005bfb4:	a019                	c.j	2005bfba <processed_source>

2005bfb6 <as_yet_unhandled>:
2005bfb6:	9002                	c.ebreak
2005bfb8:	bffd                	c.j	2005bfb6 <as_yet_unhandled>

2005bfba <processed_source>:
2005bfba:	f5c1a303          	lw	t1,-164(gp) # 200668e4 <pxCurrentTCB>
2005bfbe:	00032103          	lw	sp,0(t1) # 2000 <CRYPTO_Init+0x86a>

2005bfc2 <restore_regs>:
2005bfc2:	4282                	c.lwsp	t0,0(sp)
2005bfc4:	34129073          	csrrw	zero,mepc,t0
2005bfc8:	10412283          	lw	t0,260(sp)
2005bfcc:	30029073          	csrrw	zero,mstatus,t0

2005bfd0 <_stack_pop>:
2005bfd0:	300022f3          	csrrs	t0,mstatus,zero
2005bfd4:	6319                	c.lui	t1,0x6
2005bfd6:	0062f2b3          	and	t0,t0,t1
2005bfda:	6309                	c.lui	t1,0x2
2005bfdc:	04628c63          	beq	t0,t1,2005c034 <_stack_pop+0x64>
2005bfe0:	600a                	c.flwsp	ft0,128(sp)
2005bfe2:	609a                	c.flwsp	ft1,132(sp)
2005bfe4:	612a                	c.flwsp	ft2,136(sp)
2005bfe6:	61ba                	c.flwsp	ft3,140(sp)
2005bfe8:	624a                	c.flwsp	ft4,144(sp)
2005bfea:	62da                	c.flwsp	ft5,148(sp)
2005bfec:	636a                	c.flwsp	ft6,152(sp)
2005bfee:	63fa                	c.flwsp	ft7,156(sp)
2005bff0:	740a                	c.flwsp	fs0,160(sp)
2005bff2:	749a                	c.flwsp	fs1,164(sp)
2005bff4:	752a                	c.flwsp	fa0,168(sp)
2005bff6:	75ba                	c.flwsp	fa1,172(sp)
2005bff8:	764a                	c.flwsp	fa2,176(sp)
2005bffa:	76da                	c.flwsp	fa3,180(sp)
2005bffc:	776a                	c.flwsp	fa4,184(sp)
2005bffe:	77fa                	c.flwsp	fa5,188(sp)
2005c000:	680e                	c.flwsp	fa6,192(sp)
2005c002:	689e                	c.flwsp	fa7,196(sp)
2005c004:	692e                	c.flwsp	fs2,200(sp)
2005c006:	69be                	c.flwsp	fs3,204(sp)
2005c008:	6a4e                	c.flwsp	fs4,208(sp)
2005c00a:	6ade                	c.flwsp	fs5,212(sp)
2005c00c:	6b6e                	c.flwsp	fs6,216(sp)
2005c00e:	6bfe                	c.flwsp	fs7,220(sp)
2005c010:	7c0e                	c.flwsp	fs8,224(sp)
2005c012:	7c9e                	c.flwsp	fs9,228(sp)
2005c014:	7d2e                	c.flwsp	fs10,232(sp)
2005c016:	7dbe                	c.flwsp	fs11,236(sp)
2005c018:	7e4e                	c.flwsp	ft8,240(sp)
2005c01a:	7ede                	c.flwsp	ft9,244(sp)
2005c01c:	7f6e                	c.flwsp	ft10,248(sp)
2005c01e:	7ffe                	c.flwsp	ft11,252(sp)
2005c020:	10012283          	lw	t0,256(sp)
2005c024:	00329073          	csrrw	zero,fcsr,t0
2005c028:	6319                	c.lui	t1,0x6
2005c02a:	30033073          	csrrc	zero,mstatus,t1
2005c02e:	6311                	c.lui	t1,0x4
2005c030:	30032073          	csrrs	zero,mstatus,t1
2005c034:	4092                	c.lwsp	ra,4(sp)
2005c036:	42d2                	c.lwsp	t0,20(sp)
2005c038:	4362                	c.lwsp	t1,24(sp)
2005c03a:	43f2                	c.lwsp	t2,28(sp)
2005c03c:	5402                	c.lwsp	s0,32(sp)
2005c03e:	5492                	c.lwsp	s1,36(sp)
2005c040:	5522                	c.lwsp	a0,40(sp)
2005c042:	55b2                	c.lwsp	a1,44(sp)
2005c044:	5642                	c.lwsp	a2,48(sp)
2005c046:	56d2                	c.lwsp	a3,52(sp)
2005c048:	5762                	c.lwsp	a4,56(sp)
2005c04a:	57f2                	c.lwsp	a5,60(sp)
2005c04c:	4806                	c.lwsp	a6,64(sp)
2005c04e:	4896                	c.lwsp	a7,68(sp)
2005c050:	4926                	c.lwsp	s2,72(sp)
2005c052:	49b6                	c.lwsp	s3,76(sp)
2005c054:	4a46                	c.lwsp	s4,80(sp)
2005c056:	4ad6                	c.lwsp	s5,84(sp)
2005c058:	4b66                	c.lwsp	s6,88(sp)
2005c05a:	4bf6                	c.lwsp	s7,92(sp)
2005c05c:	5c06                	c.lwsp	s8,96(sp)
2005c05e:	5c96                	c.lwsp	s9,100(sp)
2005c060:	5d26                	c.lwsp	s10,104(sp)
2005c062:	5db6                	c.lwsp	s11,108(sp)
2005c064:	5e46                	c.lwsp	t3,112(sp)
2005c066:	5ed6                	c.lwsp	t4,116(sp)
2005c068:	5f66                	c.lwsp	t5,120(sp)
2005c06a:	5ff6                	c.lwsp	t6,124(sp)
2005c06c:	10810113          	addi	sp,sp,264
2005c070:	30200073          	mret
2005c074:	00000013          	addi	zero,zero,0

2005c078 <machine_timer_update>:
2005c078:	f401a283          	lw	t0,-192(gp) # 200668c8 <pullMachineTimerCompareRegister>
2005c07c:	e881a303          	lw	t1,-376(gp) # 20066810 <pullNextTime>
2005c080:	5efd                	c.li	t4,-1
2005c082:	00032383          	lw	t2,0(t1) # 4000 <GPIO_Direction+0x36>
2005c086:	00432e03          	lw	t3,4(t1)
2005c08a:	01d2a023          	sw	t4,0(t0)
2005c08e:	01c2a223          	sw	t3,4(t0)
2005c092:	0072a023          	sw	t2,0(t0)
2005c096:	f501a283          	lw	t0,-176(gp) # 200668d8 <uxTimerIncrementsForOneTick>
2005c09a:	00728eb3          	add	t4,t0,t2
2005c09e:	007ebf33          	sltu	t5,t4,t2
2005c0a2:	01ee0fb3          	add	t6,t3,t5
2005c0a6:	01d32023          	sw	t4,0(t1)
2005c0aa:	01f32223          	sw	t6,4(t1)
2005c0ae:	8082                	c.jr	ra

2005c0b0 <vPortHandleInterrupt>:
2005c0b0:	42bd                	c.li	t0,15
2005c0b2:	fff2c293          	xori	t0,t0,-1
2005c0b6:	00517133          	and	sp,sp,t0
2005c0ba:	1141                	c.addi	sp,-16
2005c0bc:	c006                	c.swsp	ra,0(sp)
2005c0be:	34202373          	csrrs	t1,mcause,zero
2005c0c2:	800003b7          	lui	t2,0x80000
2005c0c6:	fff3c393          	xori	t2,t2,-1
2005c0ca:	007373b3          	and	t2,t1,t2
2005c0ce:	00038533          	add	a0,t2,zero
2005c0d2:	0000a797          	auipc	a5,0xa
2005c0d6:	39278793          	addi	a5,a5,914 # 20066464 <plic_interrupt_handler>
2005c0da:	9782                	c.jalr	a5

2005c0dc <vPortHandleInterrupt_Exit>:
2005c0dc:	4082                	c.lwsp	ra,0(sp)
2005c0de:	0141                	c.addi	sp,16
2005c0e0:	8082                	c.jr	ra
2005c0e2:	0001                	c.addi	zero,0
2005c0e4:	00000013          	addi	zero,zero,0

2005c0e8 <xPortStartFirstTask>:
2005c0e8:	00000297          	auipc	t0,0x0
2005c0ec:	d9828293          	addi	t0,t0,-616 # 2005be80 <freertos_risc_v_trap_handler>
2005c0f0:	30529073          	csrrw	zero,mtvec,t0
2005c0f4:	f5c1a383          	lw	t2,-164(gp) # 200668e4 <pxCurrentTCB>
2005c0f8:	0003a103          	lw	sp,0(t2) # 80000000 <__ctrace_end__+0xb6000000>
2005c0fc:	300022f3          	csrrs	t0,mstatus,zero
2005c100:	6319                	c.lui	t1,0x6
2005c102:	0062f2b3          	and	t0,t0,t1
2005c106:	6309                	c.lui	t1,0x2
2005c108:	04628c63          	beq	t0,t1,2005c160 <xPortStartFirstTask+0x78>
2005c10c:	600a                	c.flwsp	ft0,128(sp)
2005c10e:	609a                	c.flwsp	ft1,132(sp)
2005c110:	612a                	c.flwsp	ft2,136(sp)
2005c112:	61ba                	c.flwsp	ft3,140(sp)
2005c114:	624a                	c.flwsp	ft4,144(sp)
2005c116:	62da                	c.flwsp	ft5,148(sp)
2005c118:	636a                	c.flwsp	ft6,152(sp)
2005c11a:	63fa                	c.flwsp	ft7,156(sp)
2005c11c:	740a                	c.flwsp	fs0,160(sp)
2005c11e:	749a                	c.flwsp	fs1,164(sp)
2005c120:	752a                	c.flwsp	fa0,168(sp)
2005c122:	75ba                	c.flwsp	fa1,172(sp)
2005c124:	764a                	c.flwsp	fa2,176(sp)
2005c126:	76da                	c.flwsp	fa3,180(sp)
2005c128:	776a                	c.flwsp	fa4,184(sp)
2005c12a:	77fa                	c.flwsp	fa5,188(sp)
2005c12c:	680e                	c.flwsp	fa6,192(sp)
2005c12e:	689e                	c.flwsp	fa7,196(sp)
2005c130:	692e                	c.flwsp	fs2,200(sp)
2005c132:	69be                	c.flwsp	fs3,204(sp)
2005c134:	6a4e                	c.flwsp	fs4,208(sp)
2005c136:	6ade                	c.flwsp	fs5,212(sp)
2005c138:	6b6e                	c.flwsp	fs6,216(sp)
2005c13a:	6bfe                	c.flwsp	fs7,220(sp)
2005c13c:	7c0e                	c.flwsp	fs8,224(sp)
2005c13e:	7c9e                	c.flwsp	fs9,228(sp)
2005c140:	7d2e                	c.flwsp	fs10,232(sp)
2005c142:	7dbe                	c.flwsp	fs11,236(sp)
2005c144:	7e4e                	c.flwsp	ft8,240(sp)
2005c146:	7ede                	c.flwsp	ft9,244(sp)
2005c148:	7f6e                	c.flwsp	ft10,248(sp)
2005c14a:	7ffe                	c.flwsp	ft11,252(sp)
2005c14c:	10012283          	lw	t0,256(sp)
2005c150:	00329073          	csrrw	zero,fcsr,t0
2005c154:	6319                	c.lui	t1,0x6
2005c156:	30033073          	csrrc	zero,mstatus,t1
2005c15a:	6311                	c.lui	t1,0x4
2005c15c:	30032073          	csrrs	zero,mstatus,t1
2005c160:	4092                	c.lwsp	ra,4(sp)
2005c162:	42d2                	c.lwsp	t0,20(sp)
2005c164:	4362                	c.lwsp	t1,24(sp)
2005c166:	43f2                	c.lwsp	t2,28(sp)
2005c168:	5402                	c.lwsp	s0,32(sp)
2005c16a:	5492                	c.lwsp	s1,36(sp)
2005c16c:	5522                	c.lwsp	a0,40(sp)
2005c16e:	55b2                	c.lwsp	a1,44(sp)
2005c170:	5642                	c.lwsp	a2,48(sp)
2005c172:	56d2                	c.lwsp	a3,52(sp)
2005c174:	5762                	c.lwsp	a4,56(sp)
2005c176:	57f2                	c.lwsp	a5,60(sp)
2005c178:	4806                	c.lwsp	a6,64(sp)
2005c17a:	4896                	c.lwsp	a7,68(sp)
2005c17c:	4926                	c.lwsp	s2,72(sp)
2005c17e:	49b6                	c.lwsp	s3,76(sp)
2005c180:	4a46                	c.lwsp	s4,80(sp)
2005c182:	4ad6                	c.lwsp	s5,84(sp)
2005c184:	4b66                	c.lwsp	s6,88(sp)
2005c186:	4bf6                	c.lwsp	s7,92(sp)
2005c188:	5c06                	c.lwsp	s8,96(sp)
2005c18a:	5c96                	c.lwsp	s9,100(sp)
2005c18c:	5d26                	c.lwsp	s10,104(sp)
2005c18e:	5db6                	c.lwsp	s11,108(sp)
2005c190:	5e46                	c.lwsp	t3,112(sp)
2005c192:	5ed6                	c.lwsp	t4,116(sp)
2005c194:	5f66                	c.lwsp	t5,120(sp)
2005c196:	5ff6                	c.lwsp	t6,124(sp)
2005c198:	10412283          	lw	t0,260(sp)
2005c19c:	0082e293          	ori	t0,t0,8
2005c1a0:	30029073          	csrrw	zero,mstatus,t0
2005c1a4:	42d2                	c.lwsp	t0,20(sp)
2005c1a6:	4082                	c.lwsp	ra,0(sp)
2005c1a8:	10810113          	addi	sp,sp,264
2005c1ac:	8082                	c.jr	ra
2005c1ae:	0001                	c.addi	zero,0

2005c1b0 <pxPortInitialiseStack>:
2005c1b0:	300022f3          	csrrs	t0,mstatus,zero
2005c1b4:	ff72f293          	andi	t0,t0,-9
2005c1b8:	18800313          	addi	t1,zero,392
2005c1bc:	0312                	c.slli	t1,0x4
2005c1be:	0062e2b3          	or	t0,t0,t1
2005c1c2:	1571                	c.addi	a0,-4
2005c1c4:	00552023          	sw	t0,0(a0)
2005c1c8:	f2450513          	addi	a0,a0,-220
2005c1cc:	c110                	c.sw	a2,0(a0)
2005c1ce:	fdc50513          	addi	a0,a0,-36
2005c1d2:	00052023          	sw	zero,0(a0)
2005c1d6:	4281                	c.li	t0,0

2005c1d8 <chip_specific_stack_frame>:
2005c1d8:	00028763          	beq	t0,zero,2005c1e6 <chip_specific_stack_frame+0xe>
2005c1dc:	1571                	c.addi	a0,-4
2005c1de:	00052023          	sw	zero,0(a0)
2005c1e2:	12fd                	c.addi	t0,-1
2005c1e4:	bfd5                	c.j	2005c1d8 <chip_specific_stack_frame>
2005c1e6:	1571                	c.addi	a0,-4
2005c1e8:	c10c                	c.sw	a1,0(a0)
2005c1ea:	8082                	c.jr	ra

2005c1ec <vPortBackupRegs>:
2005c1ec:	00152223          	sw	ra,4(a0)
2005c1f0:	00252423          	sw	sp,8(a0)
2005c1f4:	00352623          	sw	gp,12(a0)
2005c1f8:	00452823          	sw	tp,16(a0)
2005c1fc:	00552a23          	sw	t0,20(a0)
2005c200:	00652c23          	sw	t1,24(a0)
2005c204:	00752e23          	sw	t2,28(a0)
2005c208:	d100                	c.sw	s0,32(a0)
2005c20a:	d144                	c.sw	s1,36(a0)
2005c20c:	d508                	c.sw	a0,40(a0)
2005c20e:	d54c                	c.sw	a1,44(a0)
2005c210:	d910                	c.sw	a2,48(a0)
2005c212:	d954                	c.sw	a3,52(a0)
2005c214:	dd18                	c.sw	a4,56(a0)
2005c216:	dd5c                	c.sw	a5,60(a0)
2005c218:	05052023          	sw	a6,64(a0)
2005c21c:	05152223          	sw	a7,68(a0)
2005c220:	05252423          	sw	s2,72(a0)
2005c224:	05352623          	sw	s3,76(a0)
2005c228:	05452823          	sw	s4,80(a0)
2005c22c:	05552a23          	sw	s5,84(a0)
2005c230:	05652c23          	sw	s6,88(a0)
2005c234:	05752e23          	sw	s7,92(a0)
2005c238:	07852023          	sw	s8,96(a0)
2005c23c:	07952223          	sw	s9,100(a0)
2005c240:	07a52423          	sw	s10,104(a0)
2005c244:	07b52623          	sw	s11,108(a0)
2005c248:	07c52823          	sw	t3,112(a0)
2005c24c:	07d52a23          	sw	t4,116(a0)
2005c250:	07e52c23          	sw	t5,120(a0)
2005c254:	07f52e23          	sw	t6,124(a0)
2005c258:	300022f3          	csrrs	t0,mstatus,zero
2005c25c:	0055a023          	sw	t0,0(a1)
2005c260:	304022f3          	csrrs	t0,mie,zero
2005c264:	0055a223          	sw	t0,4(a1)
2005c268:	305022f3          	csrrs	t0,mtvec,zero
2005c26c:	0055a423          	sw	t0,8(a1)
2005c270:	0015a623          	sw	ra,12(a1)
2005c274:	8082                	c.jr	ra

2005c276 <vPortBackupfloatRegs>:
2005c276:	300022f3          	csrrs	t0,mstatus,zero
2005c27a:	6319                	c.lui	t1,0x6
2005c27c:	0062f2b3          	and	t0,t0,t1
2005c280:	06629e63          	bne	t0,t1,2005c2fc <vPortBackupfloatRegs+0x86>
2005c284:	00052027          	fsw	ft0,0(a0)
2005c288:	00152227          	fsw	ft1,4(a0)
2005c28c:	00252427          	fsw	ft2,8(a0)
2005c290:	00352627          	fsw	ft3,12(a0)
2005c294:	00452827          	fsw	ft4,16(a0)
2005c298:	00552a27          	fsw	ft5,20(a0)
2005c29c:	00652c27          	fsw	ft6,24(a0)
2005c2a0:	00752e27          	fsw	ft7,28(a0)
2005c2a4:	f100                	c.fsw	fs0,32(a0)
2005c2a6:	f144                	c.fsw	fs1,36(a0)
2005c2a8:	f508                	c.fsw	fa0,40(a0)
2005c2aa:	f54c                	c.fsw	fa1,44(a0)
2005c2ac:	f910                	c.fsw	fa2,48(a0)
2005c2ae:	f954                	c.fsw	fa3,52(a0)
2005c2b0:	fd18                	c.fsw	fa4,56(a0)
2005c2b2:	fd5c                	c.fsw	fa5,60(a0)
2005c2b4:	05052027          	fsw	fa6,64(a0)
2005c2b8:	05152227          	fsw	fa7,68(a0)
2005c2bc:	05252427          	fsw	fs2,72(a0)
2005c2c0:	05352627          	fsw	fs3,76(a0)
2005c2c4:	05452827          	fsw	fs4,80(a0)
2005c2c8:	05552a27          	fsw	fs5,84(a0)
2005c2cc:	05652c27          	fsw	fs6,88(a0)
2005c2d0:	05752e27          	fsw	fs7,92(a0)
2005c2d4:	07852027          	fsw	fs8,96(a0)
2005c2d8:	07952227          	fsw	fs9,100(a0)
2005c2dc:	07a52427          	fsw	fs10,104(a0)
2005c2e0:	07b52627          	fsw	fs11,108(a0)
2005c2e4:	07c52827          	fsw	ft8,112(a0)
2005c2e8:	07d52a27          	fsw	ft9,116(a0)
2005c2ec:	07e52c27          	fsw	ft10,120(a0)
2005c2f0:	07f52e27          	fsw	ft11,124(a0)
2005c2f4:	003022f3          	csrrs	t0,fcsr,zero
2005c2f8:	0055a023          	sw	t0,0(a1)
2005c2fc:	8082                	c.jr	ra

2005c2fe <vPortRestoreRegs>:
2005c2fe:	00852103          	lw	sp,8(a0)
2005c302:	00c52183          	lw	gp,12(a0)
2005c306:	01052203          	lw	tp,16(a0)
2005c30a:	01452283          	lw	t0,20(a0)
2005c30e:	01452283          	lw	t0,20(a0)
2005c312:	01852303          	lw	t1,24(a0)
2005c316:	01c52383          	lw	t2,28(a0)
2005c31a:	5100                	c.lw	s0,32(a0)
2005c31c:	5144                	c.lw	s1,36(a0)
2005c31e:	5508                	c.lw	a0,40(a0)
2005c320:	554c                	c.lw	a1,44(a0)
2005c322:	5910                	c.lw	a2,48(a0)
2005c324:	5954                	c.lw	a3,52(a0)
2005c326:	5d18                	c.lw	a4,56(a0)
2005c328:	5d5c                	c.lw	a5,60(a0)
2005c32a:	04052803          	lw	a6,64(a0)
2005c32e:	04452883          	lw	a7,68(a0)
2005c332:	04852903          	lw	s2,72(a0)
2005c336:	04c52983          	lw	s3,76(a0)
2005c33a:	05052a03          	lw	s4,80(a0)
2005c33e:	05452a83          	lw	s5,84(a0)
2005c342:	05852b03          	lw	s6,88(a0)
2005c346:	05c52b83          	lw	s7,92(a0)
2005c34a:	06052c03          	lw	s8,96(a0)
2005c34e:	06452c83          	lw	s9,100(a0)
2005c352:	06852d03          	lw	s10,104(a0)
2005c356:	06c52d83          	lw	s11,108(a0)
2005c35a:	07052e03          	lw	t3,112(a0)
2005c35e:	07452e83          	lw	t4,116(a0)
2005c362:	07852f03          	lw	t5,120(a0)
2005c366:	07c52f83          	lw	t6,124(a0)
2005c36a:	0005a083          	lw	ra,0(a1)
2005c36e:	30009073          	csrrw	zero,mstatus,ra
2005c372:	0045a083          	lw	ra,4(a1)
2005c376:	30409073          	csrrw	zero,mie,ra
2005c37a:	0085a083          	lw	ra,8(a1)
2005c37e:	30509073          	csrrw	zero,mtvec,ra
2005c382:	00c5a083          	lw	ra,12(a1)
2005c386:	8082                	c.jr	ra

2005c388 <vPortRestorefloatRegs>:
2005c388:	00062283          	lw	t0,0(a2)
2005c38c:	6319                	c.lui	t1,0x6
2005c38e:	0062f2b3          	and	t0,t0,t1
2005c392:	06629e63          	bne	t0,t1,2005c40e <vPortRestorefloatRegs+0x86>
2005c396:	00052007          	flw	ft0,0(a0)
2005c39a:	00452087          	flw	ft1,4(a0)
2005c39e:	00852107          	flw	ft2,8(a0)
2005c3a2:	00c52187          	flw	ft3,12(a0)
2005c3a6:	01052207          	flw	ft4,16(a0)
2005c3aa:	01452287          	flw	ft5,20(a0)
2005c3ae:	01852307          	flw	ft6,24(a0)
2005c3b2:	01c52387          	flw	ft7,28(a0)
2005c3b6:	7100                	c.flw	fs0,32(a0)
2005c3b8:	7144                	c.flw	fs1,36(a0)
2005c3ba:	7508                	c.flw	fa0,40(a0)
2005c3bc:	754c                	c.flw	fa1,44(a0)
2005c3be:	7910                	c.flw	fa2,48(a0)
2005c3c0:	7954                	c.flw	fa3,52(a0)
2005c3c2:	7d18                	c.flw	fa4,56(a0)
2005c3c4:	7d5c                	c.flw	fa5,60(a0)
2005c3c6:	04052807          	flw	fa6,64(a0)
2005c3ca:	04452887          	flw	fa7,68(a0)
2005c3ce:	04852907          	flw	fs2,72(a0)
2005c3d2:	04c52987          	flw	fs3,76(a0)
2005c3d6:	05052a07          	flw	fs4,80(a0)
2005c3da:	05452a87          	flw	fs5,84(a0)
2005c3de:	05852b07          	flw	fs6,88(a0)
2005c3e2:	05c52b87          	flw	fs7,92(a0)
2005c3e6:	06052c07          	flw	fs8,96(a0)
2005c3ea:	06452c87          	flw	fs9,100(a0)
2005c3ee:	06852d07          	flw	fs10,104(a0)
2005c3f2:	06c52d87          	flw	fs11,108(a0)
2005c3f6:	07052e07          	flw	ft8,112(a0)
2005c3fa:	07452e87          	flw	ft9,116(a0)
2005c3fe:	07852f07          	flw	ft10,120(a0)
2005c402:	07c52f87          	flw	ft11,124(a0)
2005c406:	0005a283          	lw	t0,0(a1)
2005c40a:	00329073          	csrrw	zero,fcsr,t0
2005c40e:	8082                	c.jr	ra

2005c410 <vPortBackupPmp>:
2005c410:	3b0022f3          	csrrs	t0,pmpaddr0,zero
2005c414:	00552023          	sw	t0,0(a0)
2005c418:	3b1022f3          	csrrs	t0,pmpaddr1,zero
2005c41c:	00552223          	sw	t0,4(a0)
2005c420:	3b2022f3          	csrrs	t0,pmpaddr2,zero
2005c424:	00552423          	sw	t0,8(a0)
2005c428:	3b3022f3          	csrrs	t0,pmpaddr3,zero
2005c42c:	00552623          	sw	t0,12(a0)
2005c430:	3b4022f3          	csrrs	t0,pmpaddr4,zero
2005c434:	00552823          	sw	t0,16(a0)
2005c438:	3b5022f3          	csrrs	t0,pmpaddr5,zero
2005c43c:	00552a23          	sw	t0,20(a0)
2005c440:	3b6022f3          	csrrs	t0,pmpaddr6,zero
2005c444:	00552c23          	sw	t0,24(a0)
2005c448:	3b7022f3          	csrrs	t0,pmpaddr7,zero
2005c44c:	00552e23          	sw	t0,28(a0)
2005c450:	3b8022f3          	csrrs	t0,pmpaddr8,zero
2005c454:	02552023          	sw	t0,32(a0)
2005c458:	3b9022f3          	csrrs	t0,pmpaddr9,zero
2005c45c:	02552223          	sw	t0,36(a0)
2005c460:	3ba022f3          	csrrs	t0,pmpaddr10,zero
2005c464:	02552423          	sw	t0,40(a0)
2005c468:	3bb022f3          	csrrs	t0,pmpaddr11,zero
2005c46c:	02552623          	sw	t0,44(a0)
2005c470:	3bc022f3          	csrrs	t0,pmpaddr12,zero
2005c474:	02552823          	sw	t0,48(a0)
2005c478:	3bd022f3          	csrrs	t0,pmpaddr13,zero
2005c47c:	02552a23          	sw	t0,52(a0)
2005c480:	3be022f3          	csrrs	t0,pmpaddr14,zero
2005c484:	02552c23          	sw	t0,56(a0)
2005c488:	3bf022f3          	csrrs	t0,pmpaddr15,zero
2005c48c:	02552e23          	sw	t0,60(a0)
2005c490:	3a0022f3          	csrrs	t0,pmpcfg0,zero
2005c494:	04552023          	sw	t0,64(a0)
2005c498:	3a2022f3          	csrrs	t0,pmpcfg2,zero
2005c49c:	04552223          	sw	t0,68(a0)
2005c4a0:	3a1022f3          	csrrs	t0,pmpcfg1,zero
2005c4a4:	04552423          	sw	t0,72(a0)
2005c4a8:	3a3022f3          	csrrs	t0,pmpcfg3,zero
2005c4ac:	04552623          	sw	t0,76(a0)
2005c4b0:	8082                	c.jr	ra

2005c4b2 <vPortRestorePmp>:
2005c4b2:	00052283          	lw	t0,0(a0)
2005c4b6:	3b029073          	csrrw	zero,pmpaddr0,t0
2005c4ba:	00452283          	lw	t0,4(a0)
2005c4be:	3b129073          	csrrw	zero,pmpaddr1,t0
2005c4c2:	00852283          	lw	t0,8(a0)
2005c4c6:	3b229073          	csrrw	zero,pmpaddr2,t0
2005c4ca:	00c52283          	lw	t0,12(a0)
2005c4ce:	3b329073          	csrrw	zero,pmpaddr3,t0
2005c4d2:	01052283          	lw	t0,16(a0)
2005c4d6:	3b429073          	csrrw	zero,pmpaddr4,t0
2005c4da:	01452283          	lw	t0,20(a0)
2005c4de:	3b529073          	csrrw	zero,pmpaddr5,t0
2005c4e2:	01852283          	lw	t0,24(a0)
2005c4e6:	3b629073          	csrrw	zero,pmpaddr6,t0
2005c4ea:	01c52283          	lw	t0,28(a0)
2005c4ee:	3b729073          	csrrw	zero,pmpaddr7,t0
2005c4f2:	02052283          	lw	t0,32(a0)
2005c4f6:	3b829073          	csrrw	zero,pmpaddr8,t0
2005c4fa:	02452283          	lw	t0,36(a0)
2005c4fe:	3b929073          	csrrw	zero,pmpaddr9,t0
2005c502:	02852283          	lw	t0,40(a0)
2005c506:	3ba29073          	csrrw	zero,pmpaddr10,t0
2005c50a:	02c52283          	lw	t0,44(a0)
2005c50e:	3bb29073          	csrrw	zero,pmpaddr11,t0
2005c512:	03052283          	lw	t0,48(a0)
2005c516:	3bc29073          	csrrw	zero,pmpaddr12,t0
2005c51a:	03452283          	lw	t0,52(a0)
2005c51e:	3bd29073          	csrrw	zero,pmpaddr13,t0
2005c522:	03852283          	lw	t0,56(a0)
2005c526:	3be29073          	csrrw	zero,pmpaddr14,t0
2005c52a:	03c52283          	lw	t0,60(a0)
2005c52e:	3bf29073          	csrrw	zero,pmpaddr15,t0
2005c532:	04052283          	lw	t0,64(a0)
2005c536:	3a029073          	csrrw	zero,pmpcfg0,t0
2005c53a:	04452283          	lw	t0,68(a0)
2005c53e:	3a229073          	csrrw	zero,pmpcfg2,t0
2005c542:	04852283          	lw	t0,72(a0)
2005c546:	3a129073          	csrrw	zero,pmpcfg1,t0
2005c54a:	04c52283          	lw	t0,76(a0)
2005c54e:	3a329073          	csrrw	zero,pmpcfg3,t0
2005c552:	8082                	c.jr	ra
2005c554:	0000                	c.unimp
2005c556:	0000                	c.unimp
	...

2005c55a <vPortSetupTimerInterrupt>:
2005c55a:	e681a783          	lw	a5,-408(gp) # 200667f0 <SystemCoreClock>
2005c55e:	3e800713          	addi	a4,zero,1000
2005c562:	02e7d7b3          	divu	a5,a5,a4
2005c566:	1141                	c.addi	sp,-16
2005c568:	f4f1a823          	sw	a5,-176(gp) # 200668d8 <uxTimerIncrementsForOneTick>
2005c56c:	f14027f3          	csrrs	a5,mhartid,zero
2005c570:	c63e                	c.swsp	a5,12(sp)
2005c572:	4632                	c.lwsp	a2,12(sp)
2005c574:	10000737          	lui	a4,0x10000
2005c578:	00170793          	addi	a5,a4,1 # 10000001 <__km4_boot_text_start__+0x7fffe1>
2005c57c:	963e                	c.add	a2,a5
2005c57e:	060e                	c.slli	a2,0x3
2005c580:	f4c1a023          	sw	a2,-192(gp) # 200668c8 <pullMachineTimerCompareRegister>
2005c584:	47b2                	c.lwsp	a5,12(sp)
2005c586:	f5018593          	addi	a1,gp,-176 # 200668d8 <uxTimerIncrementsForOneTick>
2005c58a:	97ba                	c.add	a5,a4
2005c58c:	078e                	c.slli	a5,0x3
2005c58e:	f4f1a223          	sw	a5,-188(gp) # 200668cc <pullMachineTimerCounterRegister>
2005c592:	800007b7          	lui	a5,0x80000
2005c596:	43c8                	c.lw	a0,4(a5)
2005c598:	4394                	c.lw	a3,0(a5)
2005c59a:	43d8                	c.lw	a4,4(a5)
2005c59c:	fea71de3          	bne	a4,a0,2005c596 <vPortSetupTimerInterrupt+0x3c>
2005c5a0:	4188                	c.lw	a0,0(a1)
2005c5a2:	9536                	c.add	a0,a3
2005c5a4:	c208                	c.sw	a0,0(a2)
2005c5a6:	419c                	c.lw	a5,0(a1)
2005c5a8:	00d536b3          	sltu	a3,a0,a3
2005c5ac:	96ba                	c.add	a3,a4
2005c5ae:	00a78733          	add	a4,a5,a0
2005c5b2:	c254                	c.sw	a3,4(a2)
2005c5b4:	00f737b3          	sltu	a5,a4,a5
2005c5b8:	f4818613          	addi	a2,gp,-184 # 200668d0 <ullNextTime>
2005c5bc:	96be                	c.add	a3,a5
2005c5be:	c218                	c.sw	a4,0(a2)
2005c5c0:	c254                	c.sw	a3,4(a2)
2005c5c2:	0141                	c.addi	sp,16
2005c5c4:	8082                	c.jr	ra

2005c5c6 <xPortStartScheduler>:
2005c5c6:	1141                	c.addi	sp,-16
2005c5c8:	c606                	c.swsp	ra,12(sp)
2005c5ca:	f91ff0ef          	jal	ra,2005c55a <vPortSetupTimerInterrupt>
2005c5ce:	6785                	c.lui	a5,0x1
2005c5d0:	88078793          	addi	a5,a5,-1920 # 880 <__NVIC_GetVectorArg+0xce>
2005c5d4:	3047a073          	csrrs	zero,mie,a5
2005c5d8:	b11ff0ef          	jal	ra,2005c0e8 <xPortStartFirstTask>
2005c5dc:	40b2                	c.lwsp	ra,12(sp)
2005c5de:	4501                	c.li	a0,0
2005c5e0:	0141                	c.addi	sp,16
2005c5e2:	8082                	c.jr	ra

2005c5e4 <vApplicationTickHook>:
2005c5e4:	8082                	c.jr	ra

2005c5e6 <vApplicationIdleHook>:
2005c5e6:	1101                	c.addi	sp,-32
2005c5e8:	ce06                	c.swsp	ra,28(sp)
2005c5ea:	f74fe0ef          	jal	ra,2005ad5e <xPortGetFreeHeapSize>
2005c5ee:	40f2                	c.lwsp	ra,28(sp)
2005c5f0:	c62a                	c.swsp	a0,12(sp)
2005c5f2:	47b2                	c.lwsp	a5,12(sp)
2005c5f4:	6105                	c.addi16sp	sp,32
2005c5f6:	8082                	c.jr	ra

2005c5f8 <vApplicationMallocFailedHook>:
2005c5f8:	1101                	c.addi	sp,-32
2005c5fa:	ce06                	c.swsp	ra,28(sp)
2005c5fc:	43c010ef          	jal	ra,2005da38 <xTaskGetSchedulerState>
2005c600:	4785                	c.li	a5,1
2005c602:	02f50863          	beq	a0,a5,2005c632 <vApplicationMallocFailedHook+0x3a>
2005c606:	4501                	c.li	a0,0
2005c608:	104010ef          	jal	ra,2005d70c <pcTaskGetName>
2005c60c:	862a                	c.mv	a2,a0
2005c60e:	c632                	c.swsp	a2,12(sp)
2005c610:	f4efe0ef          	jal	ra,2005ad5e <xPortGetFreeHeapSize>
2005c614:	4632                	c.lwsp	a2,12(sp)
2005c616:	86aa                	c.mv	a3,a0
2005c618:	200635b7          	lui	a1,0x20063
2005c61c:	20063537          	lui	a0,0x20063
2005c620:	4d458593          	addi	a1,a1,1236 # 200634d4 <__FUNCTION__.0+0x1c>
2005c624:	4d850513          	addi	a0,a0,1240 # 200634d8 <__FUNCTION__.0+0x20>
2005c628:	57f020ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005c62c:	30047073          	csrrci	zero,mstatus,8
2005c630:	a001                	c.j	2005c630 <vApplicationMallocFailedHook+0x38>
2005c632:	20063637          	lui	a2,0x20063
2005c636:	4cc60613          	addi	a2,a2,1228 # 200634cc <__FUNCTION__.0+0x14>
2005c63a:	bfd1                	c.j	2005c60e <vApplicationMallocFailedHook+0x16>

2005c63c <vApplicationStackOverflowHook>:
2005c63c:	862e                	c.mv	a2,a1
2005c63e:	20063537          	lui	a0,0x20063
2005c642:	200635b7          	lui	a1,0x20063
2005c646:	1141                	c.addi	sp,-16
2005c648:	53c58593          	addi	a1,a1,1340 # 2006353c <__FUNCTION__.5>
2005c64c:	51450513          	addi	a0,a0,1300 # 20063514 <__FUNCTION__.0+0x5c>
2005c650:	c606                	c.swsp	ra,12(sp)
2005c652:	555020ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005c656:	a001                	c.j	2005c656 <vApplicationStackOverflowHook+0x1a>

2005c658 <vApplicationGetIdleTaskMemory>:
2005c658:	200687b7          	lui	a5,0x20068
2005c65c:	f6078793          	addi	a5,a5,-160 # 20067f60 <xIdleTaskTCB.4>
2005c660:	c11c                	c.sw	a5,0(a0)
2005c662:	200657b7          	lui	a5,0x20065
2005c666:	dc078793          	addi	a5,a5,-576 # 20064dc0 <uxIdleTaskStack.3>
2005c66a:	c19c                	c.sw	a5,0(a1)
2005c66c:	20000793          	addi	a5,zero,512
2005c670:	c21c                	c.sw	a5,0(a2)
2005c672:	8082                	c.jr	ra

2005c674 <vApplicationGetTimerTaskMemory>:
2005c674:	200687b7          	lui	a5,0x20068
2005c678:	0c478793          	addi	a5,a5,196 # 200680c4 <xTimerTaskTCB.2>
2005c67c:	c11c                	c.sw	a5,0(a0)
2005c67e:	200657b7          	lui	a5,0x20065
2005c682:	5c078793          	addi	a5,a5,1472 # 200655c0 <uxTimerTaskStack.1>
2005c686:	c19c                	c.sw	a5,0(a1)
2005c688:	20000793          	addi	a5,zero,512
2005c68c:	c21c                	c.sw	a5,0(a2)
2005c68e:	8082                	c.jr	ra

2005c690 <vPortSuppressTicksAndSleep>:
2005c690:	1141                	c.addi	sp,-16
2005c692:	c422                	c.swsp	s0,8(sp)
2005c694:	4501                	c.li	a0,0
2005c696:	c606                	c.swsp	ra,12(sp)
2005c698:	e601a823          	sw	zero,-400(gp) # 200667f8 <system_can_yield>
2005c69c:	c0efc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c6a0:	300477f3          	csrrci	a5,mstatus,8
2005c6a4:	35e010ef          	jal	ra,2005da02 <eTaskConfirmSleepModeStatus>
2005c6a8:	c90d                	c.beqz	a0,2005c6da <vPortSuppressTicksAndSleep+0x4a>
2005c6aa:	952fd0ef          	jal	ra,200597fc <pmu_ready_to_sleep>
2005c6ae:	e515                	c.bnez	a0,2005c6da <vPortSuppressTicksAndSleep+0x4a>
2005c6b0:	4505                	c.li	a0,1
2005c6b2:	bf8fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c6b6:	0ff0000f          	fence	iorw,iorw
2005c6ba:	10500073          	wfi
2005c6be:	0001                	c.addi	zero,0
2005c6c0:	0ff0000f          	fence	iorw,iorw
2005c6c4:	0001                	c.addi	zero,0
2005c6c6:	0001                	c.addi	zero,0
2005c6c8:	0001                	c.addi	zero,0
2005c6ca:	0001                	c.addi	zero,0
2005c6cc:	0001                	c.addi	zero,0
2005c6ce:	0001                	c.addi	zero,0
2005c6d0:	0001                	c.addi	zero,0
2005c6d2:	0001                	c.addi	zero,0
2005c6d4:	0001                	c.addi	zero,0
2005c6d6:	0001                	c.addi	zero,0
2005c6d8:	0001                	c.addi	zero,0
2005c6da:	300467f3          	csrrsi	a5,mstatus,8
2005c6de:	4505                	c.li	a0,1
2005c6e0:	bcafc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c6e4:	4785                	c.li	a5,1
2005c6e6:	40b2                	c.lwsp	ra,12(sp)
2005c6e8:	e6f1a823          	sw	a5,-400(gp) # 200667f8 <system_can_yield>
2005c6ec:	4422                	c.lwsp	s0,8(sp)
2005c6ee:	0141                	c.addi	sp,16
2005c6f0:	8082                	c.jr	ra

2005c6f2 <prvIsQueueEmpty>:
2005c6f2:	1141                	c.addi	sp,-16
2005c6f4:	c422                	c.swsp	s0,8(sp)
2005c6f6:	842a                	c.mv	s0,a0
2005c6f8:	c606                	c.swsp	ra,12(sp)
2005c6fa:	520010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005c6fe:	5c00                	c.lw	s0,56(s0)
2005c700:	534010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c704:	40b2                	c.lwsp	ra,12(sp)
2005c706:	00143513          	sltiu	a0,s0,1
2005c70a:	4422                	c.lwsp	s0,8(sp)
2005c70c:	0141                	c.addi	sp,16
2005c70e:	8082                	c.jr	ra

2005c710 <prvCopyDataToQueue>:
2005c710:	1141                	c.addi	sp,-16
2005c712:	c422                	c.swsp	s0,8(sp)
2005c714:	c226                	c.swsp	s1,4(sp)
2005c716:	c606                	c.swsp	ra,12(sp)
2005c718:	c04a                	c.swsp	s2,0(sp)
2005c71a:	84b2                	c.mv	s1,a2
2005c71c:	4130                	c.lw	a2,64(a0)
2005c71e:	03852903          	lw	s2,56(a0)
2005c722:	842a                	c.mv	s0,a0
2005c724:	e605                	c.bnez	a2,2005c74c <prvCopyDataToQueue+0x3c>
2005c726:	411c                	c.lw	a5,0(a0)
2005c728:	4481                	c.li	s1,0
2005c72a:	e799                	c.bnez	a5,2005c738 <prvCopyDataToQueue+0x28>
2005c72c:	4508                	c.lw	a0,8(a0)
2005c72e:	3d2010ef          	jal	ra,2005db00 <xTaskPriorityDisinherit>
2005c732:	84aa                	c.mv	s1,a0
2005c734:	00042423          	sw	zero,8(s0)
2005c738:	0905                	c.addi	s2,1
2005c73a:	03242c23          	sw	s2,56(s0)
2005c73e:	40b2                	c.lwsp	ra,12(sp)
2005c740:	4422                	c.lwsp	s0,8(sp)
2005c742:	4902                	c.lwsp	s2,0(sp)
2005c744:	8526                	c.mv	a0,s1
2005c746:	4492                	c.lwsp	s1,4(sp)
2005c748:	0141                	c.addi	sp,16
2005c74a:	8082                	c.jr	ra
2005c74c:	e085                	c.bnez	s1,2005c76c <prvCopyDataToQueue+0x5c>
2005c74e:	4148                	c.lw	a0,4(a0)
2005c750:	dffad097          	auipc	ra,0xdffad
2005c754:	380080e7          	jalr	ra,896(ra) # 9ad0 <__wrap_memcpy>
2005c758:	405c                	c.lw	a5,4(s0)
2005c75a:	4038                	c.lw	a4,64(s0)
2005c75c:	97ba                	c.add	a5,a4
2005c75e:	4418                	c.lw	a4,8(s0)
2005c760:	c05c                	c.sw	a5,4(s0)
2005c762:	fce7ebe3          	bltu	a5,a4,2005c738 <prvCopyDataToQueue+0x28>
2005c766:	401c                	c.lw	a5,0(s0)
2005c768:	c05c                	c.sw	a5,4(s0)
2005c76a:	b7f9                	c.j	2005c738 <prvCopyDataToQueue+0x28>
2005c76c:	4548                	c.lw	a0,12(a0)
2005c76e:	dffad097          	auipc	ra,0xdffad
2005c772:	362080e7          	jalr	ra,866(ra) # 9ad0 <__wrap_memcpy>
2005c776:	4038                	c.lw	a4,64(s0)
2005c778:	445c                	c.lw	a5,12(s0)
2005c77a:	40e006b3          	sub	a3,zero,a4
2005c77e:	8f99                	c.sub	a5,a4
2005c780:	4018                	c.lw	a4,0(s0)
2005c782:	c45c                	c.sw	a5,12(s0)
2005c784:	00e7f563          	bgeu	a5,a4,2005c78e <prvCopyDataToQueue+0x7e>
2005c788:	441c                	c.lw	a5,8(s0)
2005c78a:	97b6                	c.add	a5,a3
2005c78c:	c45c                	c.sw	a5,12(s0)
2005c78e:	4789                	c.li	a5,2
2005c790:	00f49763          	bne	s1,a5,2005c79e <prvCopyDataToQueue+0x8e>
2005c794:	4481                	c.li	s1,0
2005c796:	fa0901e3          	beq	s2,zero,2005c738 <prvCopyDataToQueue+0x28>
2005c79a:	197d                	c.addi	s2,-1
2005c79c:	bf71                	c.j	2005c738 <prvCopyDataToQueue+0x28>
2005c79e:	4481                	c.li	s1,0
2005c7a0:	bf61                	c.j	2005c738 <prvCopyDataToQueue+0x28>

2005c7a2 <prvCopyDataFromQueue>:
2005c7a2:	87aa                	c.mv	a5,a0
2005c7a4:	43b0                	c.lw	a2,64(a5)
2005c7a6:	852e                	c.mv	a0,a1
2005c7a8:	ce11                	c.beqz	a2,2005c7c4 <prvCopyDataFromQueue+0x22>
2005c7aa:	47d8                	c.lw	a4,12(a5)
2005c7ac:	4794                	c.lw	a3,8(a5)
2005c7ae:	9732                	c.add	a4,a2
2005c7b0:	c7d8                	c.sw	a4,12(a5)
2005c7b2:	00d76463          	bltu	a4,a3,2005c7ba <prvCopyDataFromQueue+0x18>
2005c7b6:	4398                	c.lw	a4,0(a5)
2005c7b8:	c7d8                	c.sw	a4,12(a5)
2005c7ba:	47cc                	c.lw	a1,12(a5)
2005c7bc:	dffad317          	auipc	t1,0xdffad
2005c7c0:	31430067          	jalr	zero,788(t1) # 9ad0 <__wrap_memcpy>
2005c7c4:	8082                	c.jr	ra

2005c7c6 <prvUnlockQueue>:
2005c7c6:	1141                	c.addi	sp,-16
2005c7c8:	c422                	c.swsp	s0,8(sp)
2005c7ca:	842a                	c.mv	s0,a0
2005c7cc:	c226                	c.swsp	s1,4(sp)
2005c7ce:	c04a                	c.swsp	s2,0(sp)
2005c7d0:	c606                	c.swsp	ra,12(sp)
2005c7d2:	448010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005c7d6:	04544483          	lbu	s1,69(s0)
2005c7da:	02440913          	addi	s2,s0,36
2005c7de:	04e2                	c.slli	s1,0x18
2005c7e0:	84e1                	c.srai	s1,0x18
2005c7e2:	02904b63          	blt	zero,s1,2005c818 <prvUnlockQueue+0x52>
2005c7e6:	57fd                	c.li	a5,-1
2005c7e8:	04f402a3          	sb	a5,69(s0)
2005c7ec:	448010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c7f0:	42a010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005c7f4:	04444483          	lbu	s1,68(s0)
2005c7f8:	01040913          	addi	s2,s0,16
2005c7fc:	04e2                	c.slli	s1,0x18
2005c7fe:	84e1                	c.srai	s1,0x18
2005c800:	02904863          	blt	zero,s1,2005c830 <prvUnlockQueue+0x6a>
2005c804:	57fd                	c.li	a5,-1
2005c806:	04f40223          	sb	a5,68(s0)
2005c80a:	4422                	c.lwsp	s0,8(sp)
2005c80c:	40b2                	c.lwsp	ra,12(sp)
2005c80e:	4492                	c.lwsp	s1,4(sp)
2005c810:	4902                	c.lwsp	s2,0(sp)
2005c812:	0141                	c.addi	sp,16
2005c814:	4200106f          	jal	zero,2005dc34 <vTaskExitCritical>
2005c818:	505c                	c.lw	a5,36(s0)
2005c81a:	d7f1                	c.beqz	a5,2005c7e6 <prvUnlockQueue+0x20>
2005c81c:	854a                	c.mv	a0,s2
2005c81e:	14c010ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005c822:	c119                	c.beqz	a0,2005c828 <prvUnlockQueue+0x62>
2005c824:	1d6010ef          	jal	ra,2005d9fa <vTaskMissedYield>
2005c828:	14fd                	c.addi	s1,-1
2005c82a:	04e2                	c.slli	s1,0x18
2005c82c:	84e1                	c.srai	s1,0x18
2005c82e:	bf55                	c.j	2005c7e2 <prvUnlockQueue+0x1c>
2005c830:	481c                	c.lw	a5,16(s0)
2005c832:	dbe9                	c.beqz	a5,2005c804 <prvUnlockQueue+0x3e>
2005c834:	854a                	c.mv	a0,s2
2005c836:	134010ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005c83a:	c119                	c.beqz	a0,2005c840 <prvUnlockQueue+0x7a>
2005c83c:	1be010ef          	jal	ra,2005d9fa <vTaskMissedYield>
2005c840:	14fd                	c.addi	s1,-1
2005c842:	04e2                	c.slli	s1,0x18
2005c844:	84e1                	c.srai	s1,0x18
2005c846:	bf6d                	c.j	2005c800 <prvUnlockQueue+0x3a>

2005c848 <xQueueGenericReset>:
2005c848:	1141                	c.addi	sp,-16
2005c84a:	c422                	c.swsp	s0,8(sp)
2005c84c:	c226                	c.swsp	s1,4(sp)
2005c84e:	842a                	c.mv	s0,a0
2005c850:	c606                	c.swsp	ra,12(sp)
2005c852:	84ae                	c.mv	s1,a1
2005c854:	3c6010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005c858:	4034                	c.lw	a3,64(s0)
2005c85a:	5c5c                	c.lw	a5,60(s0)
2005c85c:	4018                	c.lw	a4,0(s0)
2005c85e:	02042c23          	sw	zero,56(s0)
2005c862:	02f687b3          	mul	a5,a3,a5
2005c866:	c058                	c.sw	a4,4(s0)
2005c868:	00f70633          	add	a2,a4,a5
2005c86c:	8f95                	c.sub	a5,a3
2005c86e:	97ba                	c.add	a5,a4
2005c870:	c45c                	c.sw	a5,12(s0)
2005c872:	57fd                	c.li	a5,-1
2005c874:	04f40223          	sb	a5,68(s0)
2005c878:	c410                	c.sw	a2,8(s0)
2005c87a:	04f402a3          	sb	a5,69(s0)
2005c87e:	e095                	c.bnez	s1,2005c8a2 <xQueueGenericReset+0x5a>
2005c880:	481c                	c.lw	a5,16(s0)
2005c882:	cb81                	c.beqz	a5,2005c892 <xQueueGenericReset+0x4a>
2005c884:	01040513          	addi	a0,s0,16
2005c888:	0e2010ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005c88c:	c119                	c.beqz	a0,2005c892 <xQueueGenericReset+0x4a>
2005c88e:	00000073          	ecall
2005c892:	3a2010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c896:	40b2                	c.lwsp	ra,12(sp)
2005c898:	4422                	c.lwsp	s0,8(sp)
2005c89a:	4492                	c.lwsp	s1,4(sp)
2005c89c:	4505                	c.li	a0,1
2005c89e:	0141                	c.addi	sp,16
2005c8a0:	8082                	c.jr	ra
2005c8a2:	01040513          	addi	a0,s0,16
2005c8a6:	db6fe0ef          	jal	ra,2005ae5c <vListInitialise>
2005c8aa:	02440513          	addi	a0,s0,36
2005c8ae:	daefe0ef          	jal	ra,2005ae5c <vListInitialise>
2005c8b2:	b7c5                	c.j	2005c892 <xQueueGenericReset+0x4a>

2005c8b4 <xQueueGetMutexHolder>:
2005c8b4:	1101                	c.addi	sp,-32
2005c8b6:	cc22                	c.swsp	s0,24(sp)
2005c8b8:	ce06                	c.swsp	ra,28(sp)
2005c8ba:	842a                	c.mv	s0,a0
2005c8bc:	35e010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005c8c0:	401c                	c.lw	a5,0(s0)
2005c8c2:	4501                	c.li	a0,0
2005c8c4:	e391                	c.bnez	a5,2005c8c8 <xQueueGetMutexHolder+0x14>
2005c8c6:	4408                	c.lw	a0,8(s0)
2005c8c8:	c62a                	c.swsp	a0,12(sp)
2005c8ca:	36a010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c8ce:	40f2                	c.lwsp	ra,28(sp)
2005c8d0:	4462                	c.lwsp	s0,24(sp)
2005c8d2:	4532                	c.lwsp	a0,12(sp)
2005c8d4:	6105                	c.addi16sp	sp,32
2005c8d6:	8082                	c.jr	ra

2005c8d8 <xQueueGenericSend>:
2005c8d8:	7139                	c.addi16sp	sp,-64
2005c8da:	dc22                	c.swsp	s0,56(sp)
2005c8dc:	da26                	c.swsp	s1,52(sp)
2005c8de:	d84a                	c.swsp	s2,48(sp)
2005c8e0:	d64e                	c.swsp	s3,44(sp)
2005c8e2:	d452                	c.swsp	s4,40(sp)
2005c8e4:	d256                	c.swsp	s5,36(sp)
2005c8e6:	d05a                	c.swsp	s6,32(sp)
2005c8e8:	de06                	c.swsp	ra,60(sp)
2005c8ea:	842a                	c.mv	s0,a0
2005c8ec:	892e                	c.mv	s2,a1
2005c8ee:	c632                	c.swsp	a2,12(sp)
2005c8f0:	84b6                	c.mv	s1,a3
2005c8f2:	4a01                	c.li	s4,0
2005c8f4:	4a89                	c.li	s5,2
2005c8f6:	59fd                	c.li	s3,-1
2005c8f8:	01050b13          	addi	s6,a0,16
2005c8fc:	31e010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005c900:	5c18                	c.lw	a4,56(s0)
2005c902:	5c5c                	c.lw	a5,60(s0)
2005c904:	00f76463          	bltu	a4,a5,2005c90c <xQueueGenericSend+0x34>
2005c908:	03549d63          	bne	s1,s5,2005c942 <xQueueGenericSend+0x6a>
2005c90c:	8626                	c.mv	a2,s1
2005c90e:	85ca                	c.mv	a1,s2
2005c910:	8522                	c.mv	a0,s0
2005c912:	dffff0ef          	jal	ra,2005c710 <prvCopyDataToQueue>
2005c916:	505c                	c.lw	a5,36(s0)
2005c918:	c789                	c.beqz	a5,2005c922 <xQueueGenericSend+0x4a>
2005c91a:	02440513          	addi	a0,s0,36
2005c91e:	04c010ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005c922:	c119                	c.beqz	a0,2005c928 <xQueueGenericSend+0x50>
2005c924:	00000073          	ecall
2005c928:	30c010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c92c:	4505                	c.li	a0,1
2005c92e:	50f2                	c.lwsp	ra,60(sp)
2005c930:	5462                	c.lwsp	s0,56(sp)
2005c932:	54d2                	c.lwsp	s1,52(sp)
2005c934:	5942                	c.lwsp	s2,48(sp)
2005c936:	59b2                	c.lwsp	s3,44(sp)
2005c938:	5a22                	c.lwsp	s4,40(sp)
2005c93a:	5a92                	c.lwsp	s5,36(sp)
2005c93c:	5b02                	c.lwsp	s6,32(sp)
2005c93e:	6121                	c.addi16sp	sp,64
2005c940:	8082                	c.jr	ra
2005c942:	47b2                	c.lwsp	a5,12(sp)
2005c944:	e789                	c.bnez	a5,2005c94e <xQueueGenericSend+0x76>
2005c946:	2ee010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c94a:	4501                	c.li	a0,0
2005c94c:	b7cd                	c.j	2005c92e <xQueueGenericSend+0x56>
2005c94e:	000a1563          	bne	s4,zero,2005c958 <xQueueGenericSend+0x80>
2005c952:	0828                	c.addi4spn	a0,sp,24
2005c954:	098010ef          	jal	ra,2005d9ec <vTaskInternalSetTimeOutState>
2005c958:	2dc010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c95c:	599000ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005c960:	2ba010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005c964:	04444783          	lbu	a5,68(s0)
2005c968:	07e2                	c.slli	a5,0x18
2005c96a:	87e1                	c.srai	a5,0x18
2005c96c:	01379463          	bne	a5,s3,2005c974 <xQueueGenericSend+0x9c>
2005c970:	04040223          	sb	zero,68(s0)
2005c974:	04544783          	lbu	a5,69(s0)
2005c978:	07e2                	c.slli	a5,0x18
2005c97a:	87e1                	c.srai	a5,0x18
2005c97c:	01379463          	bne	a5,s3,2005c984 <xQueueGenericSend+0xac>
2005c980:	040402a3          	sb	zero,69(s0)
2005c984:	2b0010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c988:	006c                	c.addi4spn	a1,sp,12
2005c98a:	0828                	c.addi4spn	a0,sp,24
2005c98c:	1c1010ef          	jal	ra,2005e34c <xTaskCheckForTimeOut>
2005c990:	e121                	c.bnez	a0,2005c9d0 <xQueueGenericSend+0xf8>
2005c992:	288010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005c996:	5c18                	c.lw	a4,56(s0)
2005c998:	5c5c                	c.lw	a5,60(s0)
2005c99a:	02f71063          	bne	a4,a5,2005c9ba <xQueueGenericSend+0xe2>
2005c99e:	296010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c9a2:	45b2                	c.lwsp	a1,12(sp)
2005c9a4:	855a                	c.mv	a0,s6
2005c9a6:	77b000ef          	jal	ra,2005d920 <vTaskPlaceOnEventList>
2005c9aa:	8522                	c.mv	a0,s0
2005c9ac:	e1bff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005c9b0:	5f2010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005c9b4:	c919                	c.beqz	a0,2005c9ca <xQueueGenericSend+0xf2>
2005c9b6:	4a05                	c.li	s4,1
2005c9b8:	b791                	c.j	2005c8fc <xQueueGenericSend+0x24>
2005c9ba:	27a010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005c9be:	8522                	c.mv	a0,s0
2005c9c0:	e07ff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005c9c4:	5de010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005c9c8:	b7fd                	c.j	2005c9b6 <xQueueGenericSend+0xde>
2005c9ca:	00000073          	ecall
2005c9ce:	b7e5                	c.j	2005c9b6 <xQueueGenericSend+0xde>
2005c9d0:	8522                	c.mv	a0,s0
2005c9d2:	df5ff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005c9d6:	5cc010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005c9da:	bf85                	c.j	2005c94a <xQueueGenericSend+0x72>

2005c9dc <xQueueGenericSendFromISR>:
2005c9dc:	1141                	c.addi	sp,-16
2005c9de:	c422                	c.swsp	s0,8(sp)
2005c9e0:	c04a                	c.swsp	s2,0(sp)
2005c9e2:	c606                	c.swsp	ra,12(sp)
2005c9e4:	c226                	c.swsp	s1,4(sp)
2005c9e6:	5d18                	c.lw	a4,56(a0)
2005c9e8:	5d5c                	c.lw	a5,60(a0)
2005c9ea:	8932                	c.mv	s2,a2
2005c9ec:	842a                	c.mv	s0,a0
2005c9ee:	8636                	c.mv	a2,a3
2005c9f0:	00f76663          	bltu	a4,a5,2005c9fc <xQueueGenericSendFromISR+0x20>
2005c9f4:	4789                	c.li	a5,2
2005c9f6:	4501                	c.li	a0,0
2005c9f8:	02f69063          	bne	a3,a5,2005ca18 <xQueueGenericSendFromISR+0x3c>
2005c9fc:	04544483          	lbu	s1,69(s0)
2005ca00:	5c1c                	c.lw	a5,56(s0)
2005ca02:	8522                	c.mv	a0,s0
2005ca04:	04e2                	c.slli	s1,0x18
2005ca06:	d0bff0ef          	jal	ra,2005c710 <prvCopyDataToQueue>
2005ca0a:	84e1                	c.srai	s1,0x18
2005ca0c:	57fd                	c.li	a5,-1
2005ca0e:	02f49663          	bne	s1,a5,2005ca3a <xQueueGenericSendFromISR+0x5e>
2005ca12:	505c                	c.lw	a5,36(s0)
2005ca14:	eb81                	c.bnez	a5,2005ca24 <xQueueGenericSendFromISR+0x48>
2005ca16:	4505                	c.li	a0,1
2005ca18:	40b2                	c.lwsp	ra,12(sp)
2005ca1a:	4422                	c.lwsp	s0,8(sp)
2005ca1c:	4492                	c.lwsp	s1,4(sp)
2005ca1e:	4902                	c.lwsp	s2,0(sp)
2005ca20:	0141                	c.addi	sp,16
2005ca22:	8082                	c.jr	ra
2005ca24:	02440513          	addi	a0,s0,36
2005ca28:	743000ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005ca2c:	d56d                	c.beqz	a0,2005ca16 <xQueueGenericSendFromISR+0x3a>
2005ca2e:	fe0904e3          	beq	s2,zero,2005ca16 <xQueueGenericSendFromISR+0x3a>
2005ca32:	4785                	c.li	a5,1
2005ca34:	00f92023          	sw	a5,0(s2)
2005ca38:	bff9                	c.j	2005ca16 <xQueueGenericSendFromISR+0x3a>
2005ca3a:	0485                	c.addi	s1,1
2005ca3c:	04e2                	c.slli	s1,0x18
2005ca3e:	84e1                	c.srai	s1,0x18
2005ca40:	049402a3          	sb	s1,69(s0)
2005ca44:	bfc9                	c.j	2005ca16 <xQueueGenericSendFromISR+0x3a>

2005ca46 <xQueueGiveFromISR>:
2005ca46:	5d18                	c.lw	a4,56(a0)
2005ca48:	5d5c                	c.lw	a5,60(a0)
2005ca4a:	04f77663          	bgeu	a4,a5,2005ca96 <xQueueGiveFromISR+0x50>
2005ca4e:	04554783          	lbu	a5,69(a0)
2005ca52:	0705                	c.addi	a4,1
2005ca54:	dd18                	c.sw	a4,56(a0)
2005ca56:	07e2                	c.slli	a5,0x18
2005ca58:	87e1                	c.srai	a5,0x18
2005ca5a:	577d                	c.li	a4,-1
2005ca5c:	02e79763          	bne	a5,a4,2005ca8a <xQueueGiveFromISR+0x44>
2005ca60:	515c                	c.lw	a5,36(a0)
2005ca62:	e399                	c.bnez	a5,2005ca68 <xQueueGiveFromISR+0x22>
2005ca64:	4505                	c.li	a0,1
2005ca66:	8082                	c.jr	ra
2005ca68:	1141                	c.addi	sp,-16
2005ca6a:	02450513          	addi	a0,a0,36
2005ca6e:	c422                	c.swsp	s0,8(sp)
2005ca70:	c606                	c.swsp	ra,12(sp)
2005ca72:	842e                	c.mv	s0,a1
2005ca74:	6f7000ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005ca78:	c501                	c.beqz	a0,2005ca80 <xQueueGiveFromISR+0x3a>
2005ca7a:	c019                	c.beqz	s0,2005ca80 <xQueueGiveFromISR+0x3a>
2005ca7c:	4785                	c.li	a5,1
2005ca7e:	c01c                	c.sw	a5,0(s0)
2005ca80:	40b2                	c.lwsp	ra,12(sp)
2005ca82:	4422                	c.lwsp	s0,8(sp)
2005ca84:	4505                	c.li	a0,1
2005ca86:	0141                	c.addi	sp,16
2005ca88:	8082                	c.jr	ra
2005ca8a:	0785                	c.addi	a5,1
2005ca8c:	07e2                	c.slli	a5,0x18
2005ca8e:	87e1                	c.srai	a5,0x18
2005ca90:	04f502a3          	sb	a5,69(a0)
2005ca94:	bfc1                	c.j	2005ca64 <xQueueGiveFromISR+0x1e>
2005ca96:	4501                	c.li	a0,0
2005ca98:	8082                	c.jr	ra

2005ca9a <xQueueReceive>:
2005ca9a:	7139                	c.addi16sp	sp,-64
2005ca9c:	dc22                	c.swsp	s0,56(sp)
2005ca9e:	d84a                	c.swsp	s2,48(sp)
2005caa0:	d64e                	c.swsp	s3,44(sp)
2005caa2:	d452                	c.swsp	s4,40(sp)
2005caa4:	d256                	c.swsp	s5,36(sp)
2005caa6:	de06                	c.swsp	ra,60(sp)
2005caa8:	da26                	c.swsp	s1,52(sp)
2005caaa:	842a                	c.mv	s0,a0
2005caac:	892e                	c.mv	s2,a1
2005caae:	c632                	c.swsp	a2,12(sp)
2005cab0:	4a01                	c.li	s4,0
2005cab2:	59fd                	c.li	s3,-1
2005cab4:	02450a93          	addi	s5,a0,36
2005cab8:	162010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005cabc:	5c04                	c.lw	s1,56(s0)
2005cabe:	cc85                	c.beqz	s1,2005caf6 <xQueueReceive+0x5c>
2005cac0:	85ca                	c.mv	a1,s2
2005cac2:	8522                	c.mv	a0,s0
2005cac4:	14fd                	c.addi	s1,-1
2005cac6:	cddff0ef          	jal	ra,2005c7a2 <prvCopyDataFromQueue>
2005caca:	dc04                	c.sw	s1,56(s0)
2005cacc:	481c                	c.lw	a5,16(s0)
2005cace:	cb81                	c.beqz	a5,2005cade <xQueueReceive+0x44>
2005cad0:	01040513          	addi	a0,s0,16
2005cad4:	697000ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005cad8:	c119                	c.beqz	a0,2005cade <xQueueReceive+0x44>
2005cada:	00000073          	ecall
2005cade:	156010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cae2:	4505                	c.li	a0,1
2005cae4:	50f2                	c.lwsp	ra,60(sp)
2005cae6:	5462                	c.lwsp	s0,56(sp)
2005cae8:	54d2                	c.lwsp	s1,52(sp)
2005caea:	5942                	c.lwsp	s2,48(sp)
2005caec:	59b2                	c.lwsp	s3,44(sp)
2005caee:	5a22                	c.lwsp	s4,40(sp)
2005caf0:	5a92                	c.lwsp	s5,36(sp)
2005caf2:	6121                	c.addi16sp	sp,64
2005caf4:	8082                	c.jr	ra
2005caf6:	47b2                	c.lwsp	a5,12(sp)
2005caf8:	e789                	c.bnez	a5,2005cb02 <xQueueReceive+0x68>
2005cafa:	13a010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cafe:	4501                	c.li	a0,0
2005cb00:	b7d5                	c.j	2005cae4 <xQueueReceive+0x4a>
2005cb02:	000a1563          	bne	s4,zero,2005cb0c <xQueueReceive+0x72>
2005cb06:	0828                	c.addi4spn	a0,sp,24
2005cb08:	6e5000ef          	jal	ra,2005d9ec <vTaskInternalSetTimeOutState>
2005cb0c:	128010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cb10:	3e5000ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005cb14:	106010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005cb18:	04444783          	lbu	a5,68(s0)
2005cb1c:	07e2                	c.slli	a5,0x18
2005cb1e:	87e1                	c.srai	a5,0x18
2005cb20:	01379463          	bne	a5,s3,2005cb28 <xQueueReceive+0x8e>
2005cb24:	04040223          	sb	zero,68(s0)
2005cb28:	04544783          	lbu	a5,69(s0)
2005cb2c:	07e2                	c.slli	a5,0x18
2005cb2e:	87e1                	c.srai	a5,0x18
2005cb30:	01379463          	bne	a5,s3,2005cb38 <xQueueReceive+0x9e>
2005cb34:	040402a3          	sb	zero,69(s0)
2005cb38:	0fc010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cb3c:	006c                	c.addi4spn	a1,sp,12
2005cb3e:	0828                	c.addi4spn	a0,sp,24
2005cb40:	00d010ef          	jal	ra,2005e34c <xTaskCheckForTimeOut>
2005cb44:	e90d                	c.bnez	a0,2005cb76 <xQueueReceive+0xdc>
2005cb46:	8522                	c.mv	a0,s0
2005cb48:	babff0ef          	jal	ra,2005c6f2 <prvIsQueueEmpty>
2005cb4c:	cd19                	c.beqz	a0,2005cb6a <xQueueReceive+0xd0>
2005cb4e:	45b2                	c.lwsp	a1,12(sp)
2005cb50:	8556                	c.mv	a0,s5
2005cb52:	5cf000ef          	jal	ra,2005d920 <vTaskPlaceOnEventList>
2005cb56:	8522                	c.mv	a0,s0
2005cb58:	c6fff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005cb5c:	446010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005cb60:	e119                	c.bnez	a0,2005cb66 <xQueueReceive+0xcc>
2005cb62:	00000073          	ecall
2005cb66:	4a05                	c.li	s4,1
2005cb68:	bf81                	c.j	2005cab8 <xQueueReceive+0x1e>
2005cb6a:	8522                	c.mv	a0,s0
2005cb6c:	c5bff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005cb70:	432010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005cb74:	bfcd                	c.j	2005cb66 <xQueueReceive+0xcc>
2005cb76:	8522                	c.mv	a0,s0
2005cb78:	c4fff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005cb7c:	426010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005cb80:	8522                	c.mv	a0,s0
2005cb82:	b71ff0ef          	jal	ra,2005c6f2 <prvIsQueueEmpty>
2005cb86:	d165                	c.beqz	a0,2005cb66 <xQueueReceive+0xcc>
2005cb88:	bf9d                	c.j	2005cafe <xQueueReceive+0x64>

2005cb8a <xQueueSemaphoreTake>:
2005cb8a:	7139                	c.addi16sp	sp,-64
2005cb8c:	dc22                	c.swsp	s0,56(sp)
2005cb8e:	da26                	c.swsp	s1,52(sp)
2005cb90:	d84a                	c.swsp	s2,48(sp)
2005cb92:	d64e                	c.swsp	s3,44(sp)
2005cb94:	d452                	c.swsp	s4,40(sp)
2005cb96:	de06                	c.swsp	ra,60(sp)
2005cb98:	842a                	c.mv	s0,a0
2005cb9a:	c62e                	c.swsp	a1,12(sp)
2005cb9c:	4481                	c.li	s1,0
2005cb9e:	4981                	c.li	s3,0
2005cba0:	597d                	c.li	s2,-1
2005cba2:	02450a13          	addi	s4,a0,36
2005cba6:	074010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005cbaa:	5c1c                	c.lw	a5,56(s0)
2005cbac:	cf8d                	c.beqz	a5,2005cbe6 <xQueueSemaphoreTake+0x5c>
2005cbae:	17fd                	c.addi	a5,-1
2005cbb0:	dc1c                	c.sw	a5,56(s0)
2005cbb2:	401c                	c.lw	a5,0(s0)
2005cbb4:	e781                	c.bnez	a5,2005cbbc <xQueueSemaphoreTake+0x32>
2005cbb6:	0b9010ef          	jal	ra,2005e46e <pvTaskIncrementMutexHeldCount>
2005cbba:	c408                	c.sw	a0,8(s0)
2005cbbc:	481c                	c.lw	a5,16(s0)
2005cbbe:	cb81                	c.beqz	a5,2005cbce <xQueueSemaphoreTake+0x44>
2005cbc0:	01040513          	addi	a0,s0,16
2005cbc4:	5a7000ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005cbc8:	c119                	c.beqz	a0,2005cbce <xQueueSemaphoreTake+0x44>
2005cbca:	00000073          	ecall
2005cbce:	066010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cbd2:	4485                	c.li	s1,1
2005cbd4:	50f2                	c.lwsp	ra,60(sp)
2005cbd6:	5462                	c.lwsp	s0,56(sp)
2005cbd8:	5942                	c.lwsp	s2,48(sp)
2005cbda:	59b2                	c.lwsp	s3,44(sp)
2005cbdc:	5a22                	c.lwsp	s4,40(sp)
2005cbde:	8526                	c.mv	a0,s1
2005cbe0:	54d2                	c.lwsp	s1,52(sp)
2005cbe2:	6121                	c.addi16sp	sp,64
2005cbe4:	8082                	c.jr	ra
2005cbe6:	47b2                	c.lwsp	a5,12(sp)
2005cbe8:	e789                	c.bnez	a5,2005cbf2 <xQueueSemaphoreTake+0x68>
2005cbea:	04a010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cbee:	4481                	c.li	s1,0
2005cbf0:	b7d5                	c.j	2005cbd4 <xQueueSemaphoreTake+0x4a>
2005cbf2:	00099563          	bne	s3,zero,2005cbfc <xQueueSemaphoreTake+0x72>
2005cbf6:	0828                	c.addi4spn	a0,sp,24
2005cbf8:	5f5000ef          	jal	ra,2005d9ec <vTaskInternalSetTimeOutState>
2005cbfc:	038010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cc00:	2f5000ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005cc04:	016010ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005cc08:	04444783          	lbu	a5,68(s0)
2005cc0c:	07e2                	c.slli	a5,0x18
2005cc0e:	87e1                	c.srai	a5,0x18
2005cc10:	01279463          	bne	a5,s2,2005cc18 <xQueueSemaphoreTake+0x8e>
2005cc14:	04040223          	sb	zero,68(s0)
2005cc18:	04544783          	lbu	a5,69(s0)
2005cc1c:	07e2                	c.slli	a5,0x18
2005cc1e:	87e1                	c.srai	a5,0x18
2005cc20:	01279463          	bne	a5,s2,2005cc28 <xQueueSemaphoreTake+0x9e>
2005cc24:	040402a3          	sb	zero,69(s0)
2005cc28:	00c010ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cc2c:	006c                	c.addi4spn	a1,sp,12
2005cc2e:	0828                	c.addi4spn	a0,sp,24
2005cc30:	71c010ef          	jal	ra,2005e34c <xTaskCheckForTimeOut>
2005cc34:	e139                	c.bnez	a0,2005cc7a <xQueueSemaphoreTake+0xf0>
2005cc36:	8522                	c.mv	a0,s0
2005cc38:	abbff0ef          	jal	ra,2005c6f2 <prvIsQueueEmpty>
2005cc3c:	c90d                	c.beqz	a0,2005cc6e <xQueueSemaphoreTake+0xe4>
2005cc3e:	401c                	c.lw	a5,0(s0)
2005cc40:	eb89                	c.bnez	a5,2005cc52 <xQueueSemaphoreTake+0xc8>
2005cc42:	7d9000ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005cc46:	4408                	c.lw	a0,8(s0)
2005cc48:	605000ef          	jal	ra,2005da4c <xTaskPriorityInherit>
2005cc4c:	84aa                	c.mv	s1,a0
2005cc4e:	7e7000ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cc52:	45b2                	c.lwsp	a1,12(sp)
2005cc54:	8552                	c.mv	a0,s4
2005cc56:	4cb000ef          	jal	ra,2005d920 <vTaskPlaceOnEventList>
2005cc5a:	8522                	c.mv	a0,s0
2005cc5c:	b6bff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005cc60:	342010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005cc64:	e119                	c.bnez	a0,2005cc6a <xQueueSemaphoreTake+0xe0>
2005cc66:	00000073          	ecall
2005cc6a:	4985                	c.li	s3,1
2005cc6c:	bf2d                	c.j	2005cba6 <xQueueSemaphoreTake+0x1c>
2005cc6e:	8522                	c.mv	a0,s0
2005cc70:	b57ff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005cc74:	32e010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005cc78:	bfcd                	c.j	2005cc6a <xQueueSemaphoreTake+0xe0>
2005cc7a:	8522                	c.mv	a0,s0
2005cc7c:	b4bff0ef          	jal	ra,2005c7c6 <prvUnlockQueue>
2005cc80:	322010ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005cc84:	8522                	c.mv	a0,s0
2005cc86:	a6dff0ef          	jal	ra,2005c6f2 <prvIsQueueEmpty>
2005cc8a:	d165                	c.beqz	a0,2005cc6a <xQueueSemaphoreTake+0xe0>
2005cc8c:	f40484e3          	beq	s1,zero,2005cbd4 <xQueueSemaphoreTake+0x4a>
2005cc90:	78b000ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005cc94:	504c                	c.lw	a1,36(s0)
2005cc96:	c589                	c.beqz	a1,2005cca0 <xQueueSemaphoreTake+0x116>
2005cc98:	581c                	c.lw	a5,48(s0)
2005cc9a:	45ad                	c.li	a1,11
2005cc9c:	439c                	c.lw	a5,0(a5)
2005cc9e:	8d9d                	c.sub	a1,a5
2005cca0:	4408                	c.lw	a0,8(s0)
2005cca2:	6db000ef          	jal	ra,2005db7c <vTaskPriorityDisinheritAfterTimeout>
2005cca6:	b791                	c.j	2005cbea <xQueueSemaphoreTake+0x60>

2005cca8 <xQueueReceiveFromISR>:
2005cca8:	1101                	c.addi	sp,-32
2005ccaa:	ce06                	c.swsp	ra,28(sp)
2005ccac:	cc22                	c.swsp	s0,24(sp)
2005ccae:	ca26                	c.swsp	s1,20(sp)
2005ccb0:	c84a                	c.swsp	s2,16(sp)
2005ccb2:	c64e                	c.swsp	s3,12(sp)
2005ccb4:	03852983          	lw	s3,56(a0)
2005ccb8:	04098b63          	beq	s3,zero,2005cd0e <xQueueReceiveFromISR+0x66>
2005ccbc:	04454483          	lbu	s1,68(a0)
2005ccc0:	842a                	c.mv	s0,a0
2005ccc2:	8932                	c.mv	s2,a2
2005ccc4:	04e2                	c.slli	s1,0x18
2005ccc6:	addff0ef          	jal	ra,2005c7a2 <prvCopyDataFromQueue>
2005ccca:	19fd                	c.addi	s3,-1
2005cccc:	84e1                	c.srai	s1,0x18
2005ccce:	03342c23          	sw	s3,56(s0)
2005ccd2:	57fd                	c.li	a5,-1
2005ccd4:	02f49763          	bne	s1,a5,2005cd02 <xQueueReceiveFromISR+0x5a>
2005ccd8:	481c                	c.lw	a5,16(s0)
2005ccda:	eb89                	c.bnez	a5,2005ccec <xQueueReceiveFromISR+0x44>
2005ccdc:	4505                	c.li	a0,1
2005ccde:	40f2                	c.lwsp	ra,28(sp)
2005cce0:	4462                	c.lwsp	s0,24(sp)
2005cce2:	44d2                	c.lwsp	s1,20(sp)
2005cce4:	4942                	c.lwsp	s2,16(sp)
2005cce6:	49b2                	c.lwsp	s3,12(sp)
2005cce8:	6105                	c.addi16sp	sp,32
2005ccea:	8082                	c.jr	ra
2005ccec:	01040513          	addi	a0,s0,16
2005ccf0:	47b000ef          	jal	ra,2005d96a <xTaskRemoveFromEventList>
2005ccf4:	d565                	c.beqz	a0,2005ccdc <xQueueReceiveFromISR+0x34>
2005ccf6:	fe0903e3          	beq	s2,zero,2005ccdc <xQueueReceiveFromISR+0x34>
2005ccfa:	4785                	c.li	a5,1
2005ccfc:	00f92023          	sw	a5,0(s2)
2005cd00:	bff1                	c.j	2005ccdc <xQueueReceiveFromISR+0x34>
2005cd02:	0485                	c.addi	s1,1
2005cd04:	04e2                	c.slli	s1,0x18
2005cd06:	84e1                	c.srai	s1,0x18
2005cd08:	04940223          	sb	s1,68(s0)
2005cd0c:	bfc1                	c.j	2005ccdc <xQueueReceiveFromISR+0x34>
2005cd0e:	4501                	c.li	a0,0
2005cd10:	b7f9                	c.j	2005ccde <xQueueReceiveFromISR+0x36>

2005cd12 <vQueueWaitForMessageRestricted>:
2005cd12:	1141                	c.addi	sp,-16
2005cd14:	c422                	c.swsp	s0,8(sp)
2005cd16:	c226                	c.swsp	s1,4(sp)
2005cd18:	c04a                	c.swsp	s2,0(sp)
2005cd1a:	842a                	c.mv	s0,a0
2005cd1c:	c606                	c.swsp	ra,12(sp)
2005cd1e:	84ae                	c.mv	s1,a1
2005cd20:	8932                	c.mv	s2,a2
2005cd22:	6f9000ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005cd26:	04444783          	lbu	a5,68(s0)
2005cd2a:	577d                	c.li	a4,-1
2005cd2c:	07e2                	c.slli	a5,0x18
2005cd2e:	87e1                	c.srai	a5,0x18
2005cd30:	00e79463          	bne	a5,a4,2005cd38 <vQueueWaitForMessageRestricted+0x26>
2005cd34:	04040223          	sb	zero,68(s0)
2005cd38:	04544783          	lbu	a5,69(s0)
2005cd3c:	577d                	c.li	a4,-1
2005cd3e:	07e2                	c.slli	a5,0x18
2005cd40:	87e1                	c.srai	a5,0x18
2005cd42:	00e79463          	bne	a5,a4,2005cd4a <vQueueWaitForMessageRestricted+0x38>
2005cd46:	040402a3          	sb	zero,69(s0)
2005cd4a:	6eb000ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005cd4e:	5c1c                	c.lw	a5,56(s0)
2005cd50:	e799                	c.bnez	a5,2005cd5e <vQueueWaitForMessageRestricted+0x4c>
2005cd52:	864a                	c.mv	a2,s2
2005cd54:	85a6                	c.mv	a1,s1
2005cd56:	02440513          	addi	a0,s0,36
2005cd5a:	3e7000ef          	jal	ra,2005d940 <vTaskPlaceOnEventListRestricted>
2005cd5e:	8522                	c.mv	a0,s0
2005cd60:	4422                	c.lwsp	s0,8(sp)
2005cd62:	40b2                	c.lwsp	ra,12(sp)
2005cd64:	4492                	c.lwsp	s1,4(sp)
2005cd66:	4902                	c.lwsp	s2,0(sp)
2005cd68:	0141                	c.addi	sp,16
2005cd6a:	a5dff06f          	jal	zero,2005c7c6 <prvUnlockQueue>

2005cd6e <QueueCreateTrace>:
2005cd6e:	f541a603          	lw	a2,-172(gp) # 200668dc <Inited.1>
2005cd72:	f5418693          	addi	a3,gp,-172 # 200668dc <Inited.1>
2005cd76:	200687b7          	lui	a5,0x20068
2005cd7a:	22878713          	addi	a4,a5,552 # 20068228 <QueueTraceList>
2005cd7e:	ee01                	c.bnez	a2,2005cd96 <QueueCreateTrace+0x28>
2005cd80:	22878793          	addi	a5,a5,552
2005cd84:	04070613          	addi	a2,a4,64
2005cd88:	0007a023          	sw	zero,0(a5)
2005cd8c:	0791                	c.addi	a5,4
2005cd8e:	fec79de3          	bne	a5,a2,2005cd88 <QueueCreateTrace+0x1a>
2005cd92:	4785                	c.li	a5,1
2005cd94:	c29c                	c.sw	a5,0(a3)
2005cd96:	86ba                	c.mv	a3,a4
2005cd98:	4781                	c.li	a5,0
2005cd9a:	4641                	c.li	a2,16
2005cd9c:	428c                	c.lw	a1,0(a3)
2005cd9e:	e589                	c.bnez	a1,2005cda8 <QueueCreateTrace+0x3a>
2005cda0:	078a                	c.slli	a5,0x2
2005cda2:	97ba                	c.add	a5,a4
2005cda4:	c388                	c.sw	a0,0(a5)
2005cda6:	8082                	c.jr	ra
2005cda8:	0785                	c.addi	a5,1
2005cdaa:	0691                	c.addi	a3,4
2005cdac:	fec798e3          	bne	a5,a2,2005cd9c <QueueCreateTrace+0x2e>
2005cdb0:	8082                	c.jr	ra

2005cdb2 <xQueueGenericCreate>:
2005cdb2:	1101                	c.addi	sp,-32
2005cdb4:	c84a                	c.swsp	s2,16(sp)
2005cdb6:	892a                	c.mv	s2,a0
2005cdb8:	02b50533          	mul	a0,a0,a1
2005cdbc:	cc22                	c.swsp	s0,24(sp)
2005cdbe:	ca26                	c.swsp	s1,20(sp)
2005cdc0:	c64e                	c.swsp	s3,12(sp)
2005cdc2:	ce06                	c.swsp	ra,28(sp)
2005cdc4:	84ae                	c.mv	s1,a1
2005cdc6:	89b2                	c.mv	s3,a2
2005cdc8:	05050513          	addi	a0,a0,80
2005cdcc:	e57fd0ef          	jal	ra,2005ac22 <pvPortMalloc>
2005cdd0:	842a                	c.mv	s0,a0
2005cdd2:	c505                	c.beqz	a0,2005cdfa <xQueueGenericCreate+0x48>
2005cdd4:	04050323          	sb	zero,70(a0)
2005cdd8:	87aa                	c.mv	a5,a0
2005cdda:	c099                	c.beqz	s1,2005cde0 <xQueueGenericCreate+0x2e>
2005cddc:	05050793          	addi	a5,a0,80
2005cde0:	c01c                	c.sw	a5,0(s0)
2005cde2:	03242e23          	sw	s2,60(s0)
2005cde6:	c024                	c.sw	s1,64(s0)
2005cde8:	4585                	c.li	a1,1
2005cdea:	8522                	c.mv	a0,s0
2005cdec:	a5dff0ef          	jal	ra,2005c848 <xQueueGenericReset>
2005cdf0:	8522                	c.mv	a0,s0
2005cdf2:	05340623          	sb	s3,76(s0)
2005cdf6:	f79ff0ef          	jal	ra,2005cd6e <QueueCreateTrace>
2005cdfa:	40f2                	c.lwsp	ra,28(sp)
2005cdfc:	8522                	c.mv	a0,s0
2005cdfe:	4462                	c.lwsp	s0,24(sp)
2005ce00:	44d2                	c.lwsp	s1,20(sp)
2005ce02:	4942                	c.lwsp	s2,16(sp)
2005ce04:	49b2                	c.lwsp	s3,12(sp)
2005ce06:	6105                	c.addi16sp	sp,32
2005ce08:	8082                	c.jr	ra

2005ce0a <xQueueCreateMutex>:
2005ce0a:	1141                	c.addi	sp,-16
2005ce0c:	862a                	c.mv	a2,a0
2005ce0e:	4581                	c.li	a1,0
2005ce10:	4505                	c.li	a0,1
2005ce12:	c422                	c.swsp	s0,8(sp)
2005ce14:	c606                	c.swsp	ra,12(sp)
2005ce16:	f9dff0ef          	jal	ra,2005cdb2 <xQueueGenericCreate>
2005ce1a:	842a                	c.mv	s0,a0
2005ce1c:	cd01                	c.beqz	a0,2005ce34 <xQueueCreateMutex+0x2a>
2005ce1e:	00052423          	sw	zero,8(a0)
2005ce22:	00052023          	sw	zero,0(a0)
2005ce26:	00052623          	sw	zero,12(a0)
2005ce2a:	4681                	c.li	a3,0
2005ce2c:	4601                	c.li	a2,0
2005ce2e:	4581                	c.li	a1,0
2005ce30:	aa9ff0ef          	jal	ra,2005c8d8 <xQueueGenericSend>
2005ce34:	40b2                	c.lwsp	ra,12(sp)
2005ce36:	8522                	c.mv	a0,s0
2005ce38:	4422                	c.lwsp	s0,8(sp)
2005ce3a:	0141                	c.addi	sp,16
2005ce3c:	8082                	c.jr	ra

2005ce3e <xQueueCreateCountingSemaphore>:
2005ce3e:	1141                	c.addi	sp,-16
2005ce40:	c422                	c.swsp	s0,8(sp)
2005ce42:	4609                	c.li	a2,2
2005ce44:	842e                	c.mv	s0,a1
2005ce46:	4581                	c.li	a1,0
2005ce48:	c606                	c.swsp	ra,12(sp)
2005ce4a:	f69ff0ef          	jal	ra,2005cdb2 <xQueueGenericCreate>
2005ce4e:	c111                	c.beqz	a0,2005ce52 <xQueueCreateCountingSemaphore+0x14>
2005ce50:	dd00                	c.sw	s0,56(a0)
2005ce52:	40b2                	c.lwsp	ra,12(sp)
2005ce54:	4422                	c.lwsp	s0,8(sp)
2005ce56:	0141                	c.addi	sp,16
2005ce58:	8082                	c.jr	ra

2005ce5a <xQueueGenericCreateStatic>:
2005ce5a:	1141                	c.addi	sp,-16
2005ce5c:	c422                	c.swsp	s0,8(sp)
2005ce5e:	c606                	c.swsp	ra,12(sp)
2005ce60:	c226                	c.swsp	s1,4(sp)
2005ce62:	8436                	c.mv	s0,a3
2005ce64:	c29d                	c.beqz	a3,2005ce8a <xQueueGenericCreateStatic+0x30>
2005ce66:	4785                	c.li	a5,1
2005ce68:	04f68323          	sb	a5,70(a3) # 8000046 <__ap_sram_heap_size+0x7fc0046>
2005ce6c:	84ba                	c.mv	s1,a4
2005ce6e:	e191                	c.bnez	a1,2005ce72 <xQueueGenericCreateStatic+0x18>
2005ce70:	8636                	c.mv	a2,a3
2005ce72:	dc48                	c.sw	a0,60(s0)
2005ce74:	c02c                	c.sw	a1,64(s0)
2005ce76:	c010                	c.sw	a2,0(s0)
2005ce78:	4585                	c.li	a1,1
2005ce7a:	8522                	c.mv	a0,s0
2005ce7c:	9cdff0ef          	jal	ra,2005c848 <xQueueGenericReset>
2005ce80:	8522                	c.mv	a0,s0
2005ce82:	04940623          	sb	s1,76(s0)
2005ce86:	ee9ff0ef          	jal	ra,2005cd6e <QueueCreateTrace>
2005ce8a:	40b2                	c.lwsp	ra,12(sp)
2005ce8c:	8522                	c.mv	a0,s0
2005ce8e:	4422                	c.lwsp	s0,8(sp)
2005ce90:	4492                	c.lwsp	s1,4(sp)
2005ce92:	0141                	c.addi	sp,16
2005ce94:	8082                	c.jr	ra

2005ce96 <xQueueCreateMutexStatic>:
2005ce96:	1141                	c.addi	sp,-16
2005ce98:	872a                	c.mv	a4,a0
2005ce9a:	86ae                	c.mv	a3,a1
2005ce9c:	4601                	c.li	a2,0
2005ce9e:	4581                	c.li	a1,0
2005cea0:	4505                	c.li	a0,1
2005cea2:	c422                	c.swsp	s0,8(sp)
2005cea4:	c606                	c.swsp	ra,12(sp)
2005cea6:	fb5ff0ef          	jal	ra,2005ce5a <xQueueGenericCreateStatic>
2005ceaa:	842a                	c.mv	s0,a0
2005ceac:	cd01                	c.beqz	a0,2005cec4 <xQueueCreateMutexStatic+0x2e>
2005ceae:	00052423          	sw	zero,8(a0)
2005ceb2:	00052023          	sw	zero,0(a0)
2005ceb6:	00052623          	sw	zero,12(a0)
2005ceba:	4681                	c.li	a3,0
2005cebc:	4601                	c.li	a2,0
2005cebe:	4581                	c.li	a1,0
2005cec0:	a19ff0ef          	jal	ra,2005c8d8 <xQueueGenericSend>
2005cec4:	40b2                	c.lwsp	ra,12(sp)
2005cec6:	8522                	c.mv	a0,s0
2005cec8:	4422                	c.lwsp	s0,8(sp)
2005ceca:	0141                	c.addi	sp,16
2005cecc:	8082                	c.jr	ra

2005cece <xQueueCreateCountingSemaphoreStatic>:
2005cece:	1141                	c.addi	sp,-16
2005ced0:	c422                	c.swsp	s0,8(sp)
2005ced2:	86b2                	c.mv	a3,a2
2005ced4:	842e                	c.mv	s0,a1
2005ced6:	4709                	c.li	a4,2
2005ced8:	4601                	c.li	a2,0
2005ceda:	4581                	c.li	a1,0
2005cedc:	c606                	c.swsp	ra,12(sp)
2005cede:	f7dff0ef          	jal	ra,2005ce5a <xQueueGenericCreateStatic>
2005cee2:	c111                	c.beqz	a0,2005cee6 <xQueueCreateCountingSemaphoreStatic+0x18>
2005cee4:	dd00                	c.sw	s0,56(a0)
2005cee6:	40b2                	c.lwsp	ra,12(sp)
2005cee8:	4422                	c.lwsp	s0,8(sp)
2005ceea:	0141                	c.addi	sp,16
2005ceec:	8082                	c.jr	ra

2005ceee <QueueDeleteTrace>:
2005ceee:	20068737          	lui	a4,0x20068
2005cef2:	22870693          	addi	a3,a4,552 # 20068228 <QueueTraceList>
2005cef6:	4781                	c.li	a5,0
2005cef8:	22870713          	addi	a4,a4,552
2005cefc:	4641                	c.li	a2,16
2005cefe:	428c                	c.lw	a1,0(a3)
2005cf00:	00a59763          	bne	a1,a0,2005cf0e <QueueDeleteTrace+0x20>
2005cf04:	078a                	c.slli	a5,0x2
2005cf06:	97ba                	c.add	a5,a4
2005cf08:	0007a023          	sw	zero,0(a5)
2005cf0c:	8082                	c.jr	ra
2005cf0e:	0785                	c.addi	a5,1
2005cf10:	0691                	c.addi	a3,4
2005cf12:	fec796e3          	bne	a5,a2,2005cefe <QueueDeleteTrace+0x10>
2005cf16:	8082                	c.jr	ra

2005cf18 <vQueueDelete>:
2005cf18:	1141                	c.addi	sp,-16
2005cf1a:	c422                	c.swsp	s0,8(sp)
2005cf1c:	c606                	c.swsp	ra,12(sp)
2005cf1e:	842a                	c.mv	s0,a0
2005cf20:	fcfff0ef          	jal	ra,2005ceee <QueueDeleteTrace>
2005cf24:	04644783          	lbu	a5,70(s0)
2005cf28:	e799                	c.bnez	a5,2005cf36 <vQueueDelete+0x1e>
2005cf2a:	8522                	c.mv	a0,s0
2005cf2c:	4422                	c.lwsp	s0,8(sp)
2005cf2e:	40b2                	c.lwsp	ra,12(sp)
2005cf30:	0141                	c.addi	sp,16
2005cf32:	dcffd06f          	jal	zero,2005ad00 <vPortFree>
2005cf36:	40b2                	c.lwsp	ra,12(sp)
2005cf38:	4422                	c.lwsp	s0,8(sp)
2005cf3a:	0141                	c.addi	sp,16
2005cf3c:	8082                	c.jr	ra

2005cf3e <shell_loguratRx_ipc_int>:
2005cf3e:	1141                	c.addi	sp,-16
2005cf40:	458d                	c.li	a1,3
2005cf42:	4541                	c.li	a0,16
2005cf44:	c422                	c.swsp	s0,8(sp)
2005cf46:	c606                	c.swsp	ra,12(sp)
2005cf48:	20001437          	lui	s0,0x20001
2005cf4c:	9a4fc0ef          	jal	ra,200590f0 <ipc_get_message>
2005cf50:	87aa                	c.mv	a5,a0
2005cf52:	b4440713          	addi	a4,s0,-1212 # 20000b44 <shell_ctl>
2005cf56:	4708                	c.lw	a0,8(a4)
2005cf58:	43cc                	c.lw	a1,4(a5)
2005cf5a:	7c1027f3          	csrrs	a5,0x7c1,zero
2005cf5e:	1007f793          	andi	a5,a5,256
2005cf62:	b4440413          	addi	s0,s0,-1212
2005cf66:	c3b9                	c.beqz	a5,2005cfac <shell_loguratRx_ipc_int+0x6e>
2005cf68:	01f5f713          	andi	a4,a1,31
2005cf6c:	c325                	c.beqz	a4,2005cfcc <shell_loguratRx_ipc_int+0x8e>
2005cf6e:	08070693          	addi	a3,a4,128
2005cf72:	fe05f793          	andi	a5,a1,-32
2005cf76:	01f7f713          	andi	a4,a5,31
2005cf7a:	9736                	c.add	a4,a3
2005cf7c:	0ff0000f          	fence	iorw,iorw
2005cf80:	973e                	c.add	a4,a5
2005cf82:	00f7f00b          	cache	dinv,(a5)
2005cf86:	02078793          	addi	a5,a5,32
2005cf8a:	40f706b3          	sub	a3,a4,a5
2005cf8e:	fed04ae3          	blt	zero,a3,2005cf82 <shell_loguratRx_ipc_int+0x44>
2005cf92:	0ff0000f          	fence	iorw,iorw
2005cf96:	0001                	c.addi	zero,0
2005cf98:	0001                	c.addi	zero,0
2005cf9a:	0001                	c.addi	zero,0
2005cf9c:	0001                	c.addi	zero,0
2005cf9e:	0001                	c.addi	zero,0
2005cfa0:	0001                	c.addi	zero,0
2005cfa2:	0001                	c.addi	zero,0
2005cfa4:	0001                	c.addi	zero,0
2005cfa6:	0001                	c.addi	zero,0
2005cfa8:	0001                	c.addi	zero,0
2005cfaa:	0001                	c.addi	zero,0
2005cfac:	08000613          	addi	a2,zero,128
2005cfb0:	dffac097          	auipc	ra,0xdffac
2005cfb4:	662080e7          	jalr	ra,1634(ra) # 9612 <_memcpy>
2005cfb8:	4785                	c.li	a5,1
2005cfba:	00f40223          	sb	a5,4(s0)
2005cfbe:	501c                	c.lw	a5,32(s0)
2005cfc0:	cb91                	c.beqz	a5,2005cfd4 <shell_loguratRx_ipc_int+0x96>
2005cfc2:	4c5c                	c.lw	a5,28(s0)
2005cfc4:	4422                	c.lwsp	s0,8(sp)
2005cfc6:	40b2                	c.lwsp	ra,12(sp)
2005cfc8:	0141                	c.addi	sp,16
2005cfca:	8782                	c.jr	a5
2005cfcc:	87ae                	c.mv	a5,a1
2005cfce:	08000693          	addi	a3,zero,128
2005cfd2:	b755                	c.j	2005cf76 <shell_loguratRx_ipc_int+0x38>
2005cfd4:	40b2                	c.lwsp	ra,12(sp)
2005cfd6:	4422                	c.lwsp	s0,8(sp)
2005cfd8:	0141                	c.addi	sp,16
2005cfda:	8082                	c.jr	ra

2005cfdc <shell_give_sema>:
2005cfdc:	200017b7          	lui	a5,0x20001
2005cfe0:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005cfe4:	539c                	c.lw	a5,32(a5)
2005cfe6:	c789                	c.beqz	a5,2005cff0 <shell_give_sema+0x14>
2005cfe8:	f581a503          	lw	a0,-168(gp) # 200668e0 <shell_sema>
2005cfec:	983fe06f          	jal	zero,2005b96e <rtos_sema_give>
2005cff0:	8082                	c.jr	ra

2005cff2 <shell_loguartRx_dispatch>:
2005cff2:	1141                	c.addi	sp,-16
2005cff4:	c422                	c.swsp	s0,8(sp)
2005cff6:	20001437          	lui	s0,0x20001
2005cffa:	b4440413          	addi	s0,s0,-1212 # 20000b44 <shell_ctl>
2005cffe:	c04a                	c.swsp	s2,0(sp)
2005d000:	00842903          	lw	s2,8(s0)
2005d004:	200635b7          	lui	a1,0x20063
2005d008:	55c58593          	addi	a1,a1,1372 # 2006355c <__FUNCTION__.5+0x20>
2005d00c:	00190513          	addi	a0,s2,1
2005d010:	c606                	c.swsp	ra,12(sp)
2005d012:	c226                	c.swsp	s1,4(sp)
2005d014:	dffad097          	auipc	ra,0xdffad
2005d018:	884080e7          	jalr	ra,-1916(ra) # 9898 <_stricmp>
2005d01c:	ed15                	c.bnez	a0,2005d058 <shell_loguartRx_dispatch+0x66>
2005d01e:	40000513          	addi	a0,zero,1024
2005d022:	ed2fe0ef          	jal	ra,2005b6f4 <rtos_mem_malloc>
2005d026:	3ff00593          	addi	a1,zero,1023
2005d02a:	84aa                	c.mv	s1,a0
2005d02c:	31b010ef          	jal	ra,2005eb46 <ChipInfo_GetLibVersion_ToBuf>
2005d030:	20063537          	lui	a0,0x20063
2005d034:	85a6                	c.mv	a1,s1
2005d036:	99c50513          	addi	a0,a0,-1636 # 2006299c <pmap_func+0x570>
2005d03a:	36c020ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005d03e:	8526                	c.mv	a0,s1
2005d040:	edefe0ef          	jal	ra,2005b71e <rtos_mem_free>
2005d044:	4601                	c.li	a2,0
2005d046:	08000593          	addi	a1,zero,128
2005d04a:	854a                	c.mv	a0,s2
2005d04c:	dffac097          	auipc	ra,0xdffac
2005d050:	e36080e7          	jalr	ra,-458(ra) # 8e82 <shell_array_init>
2005d054:	00040223          	sb	zero,4(s0)
2005d058:	40b2                	c.lwsp	ra,12(sp)
2005d05a:	4422                	c.lwsp	s0,8(sp)
2005d05c:	4492                	c.lwsp	s1,4(sp)
2005d05e:	4902                	c.lwsp	s2,0(sp)
2005d060:	0141                	c.addi	sp,16
2005d062:	8082                	c.jr	ra

2005d064 <shell_task_ram>:
2005d064:	715d                	c.addi16sp	sp,-80
2005d066:	c4a2                	c.swsp	s0,72(sp)
2005d068:	20001437          	lui	s0,0x20001
2005d06c:	c0ca                	c.swsp	s2,64(sp)
2005d06e:	da56                	c.swsp	s5,52(sp)
2005d070:	d65e                	c.swsp	s7,44(sp)
2005d072:	d462                	c.swsp	s8,40(sp)
2005d074:	d266                	c.swsp	s9,36(sp)
2005d076:	b4440793          	addi	a5,s0,-1212 # 20000b44 <shell_ctl>
2005d07a:	c686                	c.swsp	ra,76(sp)
2005d07c:	c2a6                	c.swsp	s1,68(sp)
2005d07e:	de4e                	c.swsp	s3,60(sp)
2005d080:	dc52                	c.swsp	s4,56(sp)
2005d082:	d85a                	c.swsp	s6,48(sp)
2005d084:	d06a                	c.swsp	s10,32(sp)
2005d086:	ce6e                	c.swsp	s11,28(sp)
2005d088:	0087ab03          	lw	s6,8(a5)
2005d08c:	4705                	c.li	a4,1
2005d08e:	d398                	c.sw	a4,32(a5)
2005d090:	40000513          	addi	a0,zero,1024
2005d094:	00e78323          	sb	a4,6(a5)
2005d098:	b4440413          	addi	s0,s0,-1212
2005d09c:	b5bfe0ef          	jal	ra,2005bbf6 <rtos_create_secure_context>
2005d0a0:	20068ab7          	lui	s5,0x20068
2005d0a4:	001b0913          	addi	s2,s6,1
2005d0a8:	20063c37          	lui	s8,0x20063
2005d0ac:	20063cb7          	lui	s9,0x20063
2005d0b0:	f581a503          	lw	a0,-168(gp) # 200668e0 <shell_sema>
2005d0b4:	55fd                	c.li	a1,-1
2005d0b6:	86dfe0ef          	jal	ra,2005b922 <rtos_sema_take>
2005d0ba:	f39ff0ef          	jal	ra,2005cff2 <shell_loguartRx_dispatch>
2005d0be:	00444783          	lbu	a5,4(s0)
2005d0c2:	0ff7f793          	andi	a5,a5,255
2005d0c6:	d7ed                	c.beqz	a5,2005d0b0 <shell_task_ram+0x4c>
2005d0c8:	4601                	c.li	a2,0
2005d0ca:	07f00593          	addi	a1,zero,127
2005d0ce:	268a8513          	addi	a0,s5,616 # 20068268 <atcmd_buf>
2005d0d2:	dffac097          	auipc	ra,0xdffac
2005d0d6:	db0080e7          	jalr	ra,-592(ra) # 8e82 <shell_array_init>
2005d0da:	85ca                	c.mv	a1,s2
2005d0dc:	268a8513          	addi	a0,s5,616
2005d0e0:	43b040ef          	jal	ra,20061d1a <strcpy>
2005d0e4:	268a8513          	addi	a0,s5,616
2005d0e8:	e29fc0ef          	jal	ra,20059f10 <atcmd_service>
2005d0ec:	ed39                	c.bnez	a0,2005d14a <shell_task_ram+0xe6>
2005d0ee:	854a                	c.mv	a0,s2
2005d0f0:	dc5fc0ef          	jal	ra,20059eb4 <mp_command_handler>
2005d0f4:	e939                	c.bnez	a0,2005d14a <shell_task_ram+0xe6>
2005d0f6:	854a                	c.mv	a0,s2
2005d0f8:	dffac097          	auipc	ra,0xdffac
2005d0fc:	da2080e7          	jalr	ra,-606(ra) # 8e9a <shell_get_argc>
2005d100:	89aa                	c.mv	s3,a0
2005d102:	854a                	c.mv	a0,s2
2005d104:	dffac097          	auipc	ra,0xdffac
2005d108:	dd0080e7          	jalr	ra,-560(ra) # 8ed4 <shell_get_argv>
2005d10c:	411c                	c.lw	a5,0(a0)
2005d10e:	4804                	c.lw	s1,16(s0)
2005d110:	01442d83          	lw	s11,20(s0)
2005d114:	8a2a                	c.mv	s4,a0
2005d116:	4d01                	c.li	s10,0
2005d118:	03ad8063          	beq	s11,s10,2005d138 <shell_task_ram+0xd4>
2005d11c:	408c                	c.lw	a1,0(s1)
2005d11e:	853e                	c.mv	a0,a5
2005d120:	c626                	c.swsp	s1,12(sp)
2005d122:	c43e                	c.swsp	a5,8(sp)
2005d124:	dffac097          	auipc	ra,0xdffac
2005d128:	774080e7          	jalr	ra,1908(ra) # 9898 <_stricmp>
2005d12c:	47a2                	c.lwsp	a5,8(sp)
2005d12e:	4732                	c.lwsp	a4,12(sp)
2005d130:	04c1                	c.addi	s1,16
2005d132:	e51d                	c.bnez	a0,2005d160 <shell_task_ram+0xfc>
2005d134:	471c                	c.lw	a5,8(a4)
2005d136:	e79d                	c.bnez	a5,2005d164 <shell_task_ram+0x100>
2005d138:	85ca                	c.mv	a1,s2
2005d13a:	564c0513          	addi	a0,s8,1380 # 20063564 <__FUNCTION__.5+0x28>
2005d13e:	268020ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005d142:	894c8513          	addi	a0,s9,-1900 # 20062894 <pmap_func+0x468>
2005d146:	260020ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005d14a:	4601                	c.li	a2,0
2005d14c:	08000593          	addi	a1,zero,128
2005d150:	855a                	c.mv	a0,s6
2005d152:	dffac097          	auipc	ra,0xdffac
2005d156:	d30080e7          	jalr	ra,-720(ra) # 8e82 <shell_array_init>
2005d15a:	00040223          	sb	zero,4(s0)
2005d15e:	bf89                	c.j	2005d0b0 <shell_task_ram+0x4c>
2005d160:	0d05                	c.addi	s10,1
2005d162:	bf5d                	c.j	2005d118 <shell_task_ram+0xb4>
2005d164:	fff98513          	addi	a0,s3,-1
2005d168:	0542                	c.slli	a0,0x10
2005d16a:	004a0593          	addi	a1,s4,4
2005d16e:	8141                	c.srli	a0,0x10
2005d170:	9782                	c.jalr	a5
2005d172:	bfe1                	c.j	2005d14a <shell_task_ram+0xe6>

2005d174 <shell_init_ram>:
2005d174:	1141                	c.addi	sp,-16
2005d176:	c606                	c.swsp	ra,12(sp)
2005d178:	bd9fc0ef          	jal	ra,20059d50 <atcmd_service_init>
2005d17c:	20065737          	lui	a4,0x20065
2005d180:	c5470693          	addi	a3,a4,-940 # 20064c54 <shell_cmd_table>
2005d184:	20065737          	lui	a4,0x20065
2005d188:	200017b7          	lui	a5,0x20001
2005d18c:	c9470713          	addi	a4,a4,-876 # 20064c94 <ipc_tickless_table>
2005d190:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005d194:	8f15                	c.sub	a4,a3
2005d196:	cb94                	c.sw	a3,16(a5)
2005d198:	8311                	c.srli	a4,0x4
2005d19a:	cbd8                	c.sw	a4,20(a5)
2005d19c:	00078223          	sb	zero,4(a5)
2005d1a0:	4705                	c.li	a4,1
2005d1a2:	00e782a3          	sb	a4,5(a5)
2005d1a6:	2005d737          	lui	a4,0x2005d
2005d1aa:	fdc70713          	addi	a4,a4,-36 # 2005cfdc <shell_give_sema>
2005d1ae:	f5818513          	addi	a0,gp,-168 # 200668e0 <shell_sema>
2005d1b2:	cfd8                	c.sw	a4,28(a5)
2005d1b4:	f2efe0ef          	jal	ra,2005b8e2 <rtos_sema_create_binary>
2005d1b8:	2005d637          	lui	a2,0x2005d
2005d1bc:	200635b7          	lui	a1,0x20063
2005d1c0:	4795                	c.li	a5,5
2005d1c2:	6705                	c.lui	a4,0x1
2005d1c4:	4681                	c.li	a3,0
2005d1c6:	06460613          	addi	a2,a2,100 # 2005d064 <shell_task_ram>
2005d1ca:	57c58593          	addi	a1,a1,1404 # 2006357c <__FUNCTION__.5+0x40>
2005d1ce:	4501                	c.li	a0,0
2005d1d0:	9f3fe0ef          	jal	ra,2005bbc2 <rtos_task_create>
2005d1d4:	c105                	c.beqz	a0,2005d1f4 <shell_init_ram+0x80>
2005d1d6:	40b2                	c.lwsp	ra,12(sp)
2005d1d8:	200636b7          	lui	a3,0x20063
2005d1dc:	200635b7          	lui	a1,0x20063
2005d1e0:	58c68693          	addi	a3,a3,1420 # 2006358c <__FUNCTION__.5+0x50>
2005d1e4:	04500613          	addi	a2,zero,69
2005d1e8:	5a858593          	addi	a1,a1,1448 # 200635a8 <__FUNCTION__.5+0x6c>
2005d1ec:	4509                	c.li	a0,2
2005d1ee:	0141                	c.addi	sp,16
2005d1f0:	f9ffd06f          	jal	zero,2005b18e <rtk_log_write>
2005d1f4:	40b2                	c.lwsp	ra,12(sp)
2005d1f6:	0141                	c.addi	sp,16
2005d1f8:	8082                	c.jr	ra

2005d1fa <sys_clear_ota_signature>:
2005d1fa:	7155                	c.addi16sp	sp,-208
2005d1fc:	4601                	c.li	a2,0
2005d1fe:	102c                	c.addi4spn	a1,sp,40
2005d200:	4509                	c.li	a0,2
2005d202:	c786                	c.swsp	ra,204(sp)
2005d204:	c3a6                	c.swsp	s1,196(sp)
2005d206:	c5a2                	c.swsp	s0,200(sp)
2005d208:	c1ca                	c.swsp	s2,192(sp)
2005d20a:	df4e                	c.swsp	s3,188(sp)
2005d20c:	dd52                	c.swsp	s4,184(sp)
2005d20e:	c082                	c.swsp	zero,64(sp)
2005d210:	c282                	c.swsp	zero,68(sp)
2005d212:	8f3fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d216:	4601                	c.li	a2,0
2005d218:	106c                	c.addi4spn	a1,sp,44
2005d21a:	450d                	c.li	a0,3
2005d21c:	8e9fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d220:	4505                	c.li	a0,1
2005d222:	9a0fc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d226:	0ff57693          	andi	a3,a0,255
2005d22a:	0016c493          	xori	s1,a3,1
2005d22e:	f80007b7          	lui	a5,0xf8000
2005d232:	e2e9                	c.bnez	a3,2005d2f4 <sys_clear_ota_signature+0xfa>
2005d234:	5722                	c.lwsp	a4,40(sp)
2005d236:	0ff57813          	andi	a6,a0,255
2005d23a:	97ba                	c.add	a5,a4
2005d23c:	00281713          	slli	a4,a6,0x2
2005d240:	0a070613          	addi	a2,a4,160 # 10a0 <CPU_ClkGet+0x58a>
2005d244:	080c                	c.addi4spn	a1,sp,16
2005d246:	962e                	c.add	a2,a1
2005d248:	f8f62423          	sw	a5,-120(a2)
2005d24c:	4785                	c.li	a5,1
2005d24e:	f8000437          	lui	s0,0xf8000
2005d252:	0af69363          	bne	a3,a5,2005d2f8 <sys_clear_ota_signature+0xfe>
2005d256:	57a2                	c.lwsp	a5,40(sp)
2005d258:	943e                	c.add	s0,a5
2005d25a:	00249793          	slli	a5,s1,0x2
2005d25e:	0814                	c.addi4spn	a3,sp,16
2005d260:	0a078793          	addi	a5,a5,160 # f80000a0 <__ctrace_end__+0x2e0000a0>
2005d264:	97b6                	c.add	a5,a3
2005d266:	f887a423          	sw	s0,-120(a5)
2005d26a:	0a070793          	addi	a5,a4,160
2005d26e:	00d78733          	add	a4,a5,a3
2005d272:	f8872a03          	lw	s4,-120(a4)
2005d276:	0485                	c.addi	s1,1
2005d278:	200639b7          	lui	s3,0x20063
2005d27c:	200636b7          	lui	a3,0x20063
2005d280:	20063937          	lui	s2,0x20063
2005d284:	0805                	c.addi	a6,1
2005d286:	4785                	c.li	a5,1
2005d288:	68c98713          	addi	a4,s3,1676 # 2006368c <__func__.1>
2005d28c:	88d2                	c.mv	a7,s4
2005d28e:	5b068693          	addi	a3,a3,1456 # 200635b0 <__FUNCTION__.5+0x74>
2005d292:	04100613          	addi	a2,zero,65
2005d296:	60090593          	addi	a1,s2,1536 # 20063600 <__FUNCTION__.5+0xc4>
2005d29a:	4505                	c.li	a0,1
2005d29c:	c222                	c.swsp	s0,4(sp)
2005d29e:	c026                	c.swsp	s1,0(sp)
2005d2a0:	ce42                	c.swsp	a6,28(sp)
2005d2a2:	eedfd0ef          	jal	ra,2005b18e <rtk_log_write>
2005d2a6:	1810                	c.addi4spn	a2,sp,48
2005d2a8:	85a2                	c.mv	a1,s0
2005d2aa:	00a8                	c.addi4spn	a0,sp,72
2005d2ac:	ffcfd0ef          	jal	ra,2005aaa8 <flash_read_word>
2005d2b0:	1850                	c.addi4spn	a2,sp,52
2005d2b2:	00440593          	addi	a1,s0,4 # f8000004 <__ctrace_end__+0x2e000004>
2005d2b6:	00a8                	c.addi4spn	a0,sp,72
2005d2b8:	ff0fd0ef          	jal	ra,2005aaa8 <flash_read_word>
2005d2bc:	5742                	c.lwsp	a4,48(sp)
2005d2be:	353937b7          	lui	a5,0x35393
2005d2c2:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d2c6:	4872                	c.lwsp	a6,28(sp)
2005d2c8:	02f71a63          	bne	a4,a5,2005d2fc <sys_clear_ota_signature+0x102>
2005d2cc:	5752                	c.lwsp	a4,52(sp)
2005d2ce:	313137b7          	lui	a5,0x31313
2005d2d2:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d2d6:	02f71363          	bne	a4,a5,2005d2fc <sys_clear_ota_signature+0x102>
2005d2da:	0090                	c.addi4spn	a2,sp,64
2005d2dc:	45a1                	c.li	a1,8
2005d2de:	8552                	c.mv	a0,s4
2005d2e0:	8d9fb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d2e4:	40be                	c.lwsp	ra,204(sp)
2005d2e6:	442e                	c.lwsp	s0,200(sp)
2005d2e8:	449e                	c.lwsp	s1,196(sp)
2005d2ea:	490e                	c.lwsp	s2,192(sp)
2005d2ec:	59fa                	c.lwsp	s3,188(sp)
2005d2ee:	5a6a                	c.lwsp	s4,184(sp)
2005d2f0:	6169                	c.addi16sp	sp,208
2005d2f2:	8082                	c.jr	ra
2005d2f4:	5732                	c.lwsp	a4,44(sp)
2005d2f6:	b781                	c.j	2005d236 <sys_clear_ota_signature+0x3c>
2005d2f8:	57b2                	c.lwsp	a5,44(sp)
2005d2fa:	bfb9                	c.j	2005d258 <sys_clear_ota_signature+0x5e>
2005d2fc:	200636b7          	lui	a3,0x20063
2005d300:	88a6                	c.mv	a7,s1
2005d302:	4785                	c.li	a5,1
2005d304:	68c98713          	addi	a4,s3,1676
2005d308:	60468693          	addi	a3,a3,1540 # 20063604 <__FUNCTION__.5+0xc8>
2005d30c:	04500613          	addi	a2,zero,69
2005d310:	60090593          	addi	a1,s2,1536
2005d314:	4509                	c.li	a0,2
2005d316:	e79fd0ef          	jal	ra,2005b18e <rtk_log_write>
2005d31a:	b7e9                	c.j	2005d2e4 <sys_clear_ota_signature+0xea>

2005d31c <sys_recover_ota_signature>:
2005d31c:	353937b7          	lui	a5,0x35393
2005d320:	715d                	c.addi16sp	sp,-80
2005d322:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d326:	d43e                	c.swsp	a5,40(sp)
2005d328:	313137b7          	lui	a5,0x31313
2005d32c:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d330:	6505                	c.lui	a0,0x1
2005d332:	c2a6                	c.swsp	s1,68(sp)
2005d334:	de4e                	c.swsp	s3,60(sp)
2005d336:	c686                	c.swsp	ra,76(sp)
2005d338:	c4a2                	c.swsp	s0,72(sp)
2005d33a:	c0ca                	c.swsp	s2,64(sp)
2005d33c:	d63e                	c.swsp	a5,44(sp)
2005d33e:	893fd0ef          	jal	ra,2005abd0 <__wrap_malloc>
2005d342:	200639b7          	lui	s3,0x20063
2005d346:	200634b7          	lui	s1,0x20063
2005d34a:	e50d                	c.bnez	a0,2005d374 <sys_recover_ota_signature+0x58>
2005d34c:	200636b7          	lui	a3,0x20063
2005d350:	67098713          	addi	a4,s3,1648 # 20063670 <__func__.0>
2005d354:	60048593          	addi	a1,s1,1536 # 20063600 <__FUNCTION__.5+0xc4>
2005d358:	65468693          	addi	a3,a3,1620 # 20063654 <__FUNCTION__.5+0x118>
2005d35c:	04500613          	addi	a2,zero,69
2005d360:	4509                	c.li	a0,2
2005d362:	e2dfd0ef          	jal	ra,2005b18e <rtk_log_write>
2005d366:	40b6                	c.lwsp	ra,76(sp)
2005d368:	4426                	c.lwsp	s0,72(sp)
2005d36a:	4496                	c.lwsp	s1,68(sp)
2005d36c:	4906                	c.lwsp	s2,64(sp)
2005d36e:	59f2                	c.lwsp	s3,60(sp)
2005d370:	6161                	c.addi16sp	sp,80
2005d372:	8082                	c.jr	ra
2005d374:	4601                	c.li	a2,0
2005d376:	082c                	c.addi4spn	a1,sp,24
2005d378:	842a                	c.mv	s0,a0
2005d37a:	4509                	c.li	a0,2
2005d37c:	f88fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d380:	086c                	c.addi4spn	a1,sp,28
2005d382:	4601                	c.li	a2,0
2005d384:	450d                	c.li	a0,3
2005d386:	f7efb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d38a:	4505                	c.li	a0,1
2005d38c:	836fc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d390:	0ff57593          	andi	a1,a0,255
2005d394:	4785                	c.li	a5,1
2005d396:	0015c693          	xori	a3,a1,1
2005d39a:	0af59963          	bne	a1,a5,2005d44c <sys_recover_ota_signature+0x130>
2005d39e:	4762                	c.lwsp	a4,24(sp)
2005d3a0:	f80007b7          	lui	a5,0xf8000
2005d3a4:	00269613          	slli	a2,a3,0x2
2005d3a8:	97ba                	c.add	a5,a4
2005d3aa:	03060713          	addi	a4,a2,48
2005d3ae:	970a                	c.add	a4,sp
2005d3b0:	fef72823          	sw	a5,-16(a4)
2005d3b4:	f80008b7          	lui	a7,0xf8000
2005d3b8:	edd1                	c.bnez	a1,2005d454 <sys_recover_ota_signature+0x138>
2005d3ba:	47e2                	c.lwsp	a5,24(sp)
2005d3bc:	0ff57813          	andi	a6,a0,255
2005d3c0:	98be                	c.add	a7,a5
2005d3c2:	00281793          	slli	a5,a6,0x2
2005d3c6:	03078793          	addi	a5,a5,48 # f8000030 <__ctrace_end__+0x2e000030>
2005d3ca:	978a                	c.add	a5,sp
2005d3cc:	ff17a823          	sw	a7,-16(a5)
2005d3d0:	03060793          	addi	a5,a2,48
2005d3d4:	00278633          	add	a2,a5,sp
2005d3d8:	ff062903          	lw	s2,-16(a2)
2005d3dc:	0685                	c.addi	a3,1
2005d3de:	c036                	c.swsp	a3,0(sp)
2005d3e0:	200636b7          	lui	a3,0x20063
2005d3e4:	67098713          	addi	a4,s3,1648
2005d3e8:	0805                	c.addi	a6,1
2005d3ea:	4785                	c.li	a5,1
2005d3ec:	5b068693          	addi	a3,a3,1456 # 200635b0 <__FUNCTION__.5+0x74>
2005d3f0:	60048593          	addi	a1,s1,1536
2005d3f4:	04100613          	addi	a2,zero,65
2005d3f8:	4505                	c.li	a0,1
2005d3fa:	c24a                	c.swsp	s2,4(sp)
2005d3fc:	d93fd0ef          	jal	ra,2005b18e <rtk_log_write>
2005d400:	8622                	c.mv	a2,s0
2005d402:	6585                	c.lui	a1,0x1
2005d404:	854a                	c.mv	a0,s2
2005d406:	f6cfb0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005d40a:	85ca                	c.mv	a1,s2
2005d40c:	4509                	c.li	a0,2
2005d40e:	891fb0ef          	jal	ra,20058c9e <FLASH_EraseXIP>
2005d412:	4621                	c.li	a2,8
2005d414:	102c                	c.addi4spn	a1,sp,40
2005d416:	8522                	c.mv	a0,s0
2005d418:	dffac097          	auipc	ra,0xdffac
2005d41c:	1fa080e7          	jalr	ra,506(ra) # 9612 <_memcpy>
2005d420:	4481                	c.li	s1,0
2005d422:	6985                	c.lui	s3,0x1
2005d424:	01248533          	add	a0,s1,s2
2005d428:	8622                	c.mv	a2,s0
2005d42a:	10000593          	addi	a1,zero,256
2005d42e:	10048493          	addi	s1,s1,256
2005d432:	f86fb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d436:	ff3497e3          	bne	s1,s3,2005d424 <sys_recover_ota_signature+0x108>
2005d43a:	8522                	c.mv	a0,s0
2005d43c:	4426                	c.lwsp	s0,72(sp)
2005d43e:	40b6                	c.lwsp	ra,76(sp)
2005d440:	4496                	c.lwsp	s1,68(sp)
2005d442:	4906                	c.lwsp	s2,64(sp)
2005d444:	59f2                	c.lwsp	s3,60(sp)
2005d446:	6161                	c.addi16sp	sp,80
2005d448:	f8cfd06f          	jal	zero,2005abd4 <__wrap_free>
2005d44c:	4772                	c.lwsp	a4,28(sp)
2005d44e:	f80007b7          	lui	a5,0xf8000
2005d452:	bf89                	c.j	2005d3a4 <sys_recover_ota_signature+0x88>
2005d454:	47f2                	c.lwsp	a5,28(sp)
2005d456:	b79d                	c.j	2005d3bc <sys_recover_ota_signature+0xa0>

2005d458 <sys_reset>:
2005d458:	fb0fc06f          	jal	zero,20059c08 <System_Reset>

2005d45c <prvAddCurrentTaskToDelayedList>:
2005d45c:	1101                	c.addi	sp,-32
2005d45e:	ca26                	c.swsp	s1,20(sp)
2005d460:	cc22                	c.swsp	s0,24(sp)
2005d462:	c84a                	c.swsp	s2,16(sp)
2005d464:	c64e                	c.swsp	s3,12(sp)
2005d466:	ce06                	c.swsp	ra,28(sp)
2005d468:	f941a983          	lw	s3,-108(gp) # 2006691c <xTickCount>
2005d46c:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005d470:	842a                	c.mv	s0,a0
2005d472:	4388                	c.lw	a0,0(a5)
2005d474:	892e                	c.mv	s2,a1
2005d476:	0511                	c.addi	a0,4
2005d478:	a43fd0ef          	jal	ra,2005aeba <uxListRemove>
2005d47c:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005d480:	ed09                	c.bnez	a0,2005d49a <prvAddCurrentTaskToDelayedList+0x3e>
2005d482:	4398                	c.lw	a4,0(a5)
2005d484:	f8018693          	addi	a3,gp,-128 # 20066908 <uxTopReadyPriority>
2005d488:	574c                	c.lw	a1,44(a4)
2005d48a:	4290                	c.lw	a2,0(a3)
2005d48c:	4705                	c.li	a4,1
2005d48e:	00b71733          	sll	a4,a4,a1
2005d492:	fff74713          	xori	a4,a4,-1
2005d496:	8f71                	c.and	a4,a2
2005d498:	c298                	c.sw	a4,0(a3)
2005d49a:	577d                	c.li	a4,-1
2005d49c:	02e41263          	bne	s0,a4,2005d4c0 <prvAddCurrentTaskToDelayedList+0x64>
2005d4a0:	02090063          	beq	s2,zero,2005d4c0 <prvAddCurrentTaskToDelayedList+0x64>
2005d4a4:	438c                	c.lw	a1,0(a5)
2005d4a6:	4462                	c.lwsp	s0,24(sp)
2005d4a8:	40f2                	c.lwsp	ra,28(sp)
2005d4aa:	44d2                	c.lwsp	s1,20(sp)
2005d4ac:	4942                	c.lwsp	s2,16(sp)
2005d4ae:	49b2                	c.lwsp	s3,12(sp)
2005d4b0:	20068537          	lui	a0,0x20068
2005d4b4:	0591                	c.addi	a1,4
2005d4b6:	40050513          	addi	a0,a0,1024 # 20068400 <xSuspendedTaskList>
2005d4ba:	6105                	c.addi16sp	sp,32
2005d4bc:	9bbfd06f          	jal	zero,2005ae76 <vListInsertEnd>
2005d4c0:	4398                	c.lw	a4,0(a5)
2005d4c2:	944e                	c.add	s0,s3
2005d4c4:	c340                	c.sw	s0,4(a4)
2005d4c6:	01347e63          	bgeu	s0,s3,2005d4e2 <prvAddCurrentTaskToDelayedList+0x86>
2005d4ca:	f641a503          	lw	a0,-156(gp) # 200668ec <pxOverflowDelayedTaskList>
2005d4ce:	4462                	c.lwsp	s0,24(sp)
2005d4d0:	438c                	c.lw	a1,0(a5)
2005d4d2:	40f2                	c.lwsp	ra,28(sp)
2005d4d4:	44d2                	c.lwsp	s1,20(sp)
2005d4d6:	4942                	c.lwsp	s2,16(sp)
2005d4d8:	49b2                	c.lwsp	s3,12(sp)
2005d4da:	0591                	c.addi	a1,4
2005d4dc:	6105                	c.addi16sp	sp,32
2005d4de:	9affd06f          	jal	zero,2005ae8c <vListInsert>
2005d4e2:	f601a503          	lw	a0,-160(gp) # 200668e8 <pxDelayedTaskList>
2005d4e6:	438c                	c.lw	a1,0(a5)
2005d4e8:	0591                	c.addi	a1,4
2005d4ea:	9a3fd0ef          	jal	ra,2005ae8c <vListInsert>
2005d4ee:	f8418793          	addi	a5,gp,-124 # 2006690c <xNextTaskUnblockTime>
2005d4f2:	4398                	c.lw	a4,0(a5)
2005d4f4:	00e47363          	bgeu	s0,a4,2005d4fa <prvAddCurrentTaskToDelayedList+0x9e>
2005d4f8:	c380                	c.sw	s0,0(a5)
2005d4fa:	40f2                	c.lwsp	ra,28(sp)
2005d4fc:	4462                	c.lwsp	s0,24(sp)
2005d4fe:	44d2                	c.lwsp	s1,20(sp)
2005d500:	4942                	c.lwsp	s2,16(sp)
2005d502:	49b2                	c.lwsp	s3,12(sp)
2005d504:	6105                	c.addi16sp	sp,32
2005d506:	8082                	c.jr	ra

2005d508 <prvWriteNameToBuffer>:
2005d508:	1141                	c.addi	sp,-16
2005d50a:	c606                	c.swsp	ra,12(sp)
2005d50c:	c422                	c.swsp	s0,8(sp)
2005d50e:	842a                	c.mv	s0,a0
2005d510:	00b040ef          	jal	ra,20061d1a <strcpy>
2005d514:	8522                	c.mv	a0,s0
2005d516:	dffac097          	auipc	ra,0xdffac
2005d51a:	5c2080e7          	jalr	ra,1474(ra) # 9ad8 <__wrap_strlen>
2005d51e:	87aa                	c.mv	a5,a0
2005d520:	4759                	c.li	a4,22
2005d522:	02000693          	addi	a3,zero,32
2005d526:	02f77063          	bgeu	a4,a5,2005d546 <prvWriteNameToBuffer+0x3e>
2005d52a:	475d                	c.li	a4,23
2005d52c:	4781                	c.li	a5,0
2005d52e:	00a76463          	bltu	a4,a0,2005d536 <prvWriteNameToBuffer+0x2e>
2005d532:	40a707b3          	sub	a5,a4,a0
2005d536:	953e                	c.add	a0,a5
2005d538:	9522                	c.add	a0,s0
2005d53a:	00050023          	sb	zero,0(a0)
2005d53e:	40b2                	c.lwsp	ra,12(sp)
2005d540:	4422                	c.lwsp	s0,8(sp)
2005d542:	0141                	c.addi	sp,16
2005d544:	8082                	c.jr	ra
2005d546:	00f40633          	add	a2,s0,a5
2005d54a:	00d60023          	sb	a3,0(a2)
2005d54e:	0785                	c.addi	a5,1
2005d550:	bfd9                	c.j	2005d526 <prvWriteNameToBuffer+0x1e>

2005d552 <prvGetExpectedIdleTime>:
2005d552:	f801a703          	lw	a4,-128(gp) # 20066908 <uxTopReadyPriority>
2005d556:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005d55a:	57c8                	c.lw	a0,44(a5)
2005d55c:	e105                	c.bnez	a0,2005d57c <prvGetExpectedIdleTime+0x2a>
2005d55e:	200687b7          	lui	a5,0x20068
2005d562:	2e87a683          	lw	a3,744(a5) # 200682e8 <pxReadyTasksLists>
2005d566:	4785                	c.li	a5,1
2005d568:	00d7eb63          	bltu	a5,a3,2005d57e <prvGetExpectedIdleTime+0x2c>
2005d56c:	00e7e963          	bltu	a5,a4,2005d57e <prvGetExpectedIdleTime+0x2c>
2005d570:	f841a503          	lw	a0,-124(gp) # 2006690c <xNextTaskUnblockTime>
2005d574:	f941a783          	lw	a5,-108(gp) # 2006691c <xTickCount>
2005d578:	8d1d                	c.sub	a0,a5
2005d57a:	8082                	c.jr	ra
2005d57c:	4501                	c.li	a0,0
2005d57e:	8082                	c.jr	ra

2005d580 <prvResetNextTaskUnblockTime>:
2005d580:	f6018713          	addi	a4,gp,-160 # 200668e8 <pxDelayedTaskList>
2005d584:	431c                	c.lw	a5,0(a4)
2005d586:	4394                	c.lw	a3,0(a5)
2005d588:	f8418793          	addi	a5,gp,-124 # 2006690c <xNextTaskUnblockTime>
2005d58c:	e681                	c.bnez	a3,2005d594 <prvResetNextTaskUnblockTime+0x14>
2005d58e:	577d                	c.li	a4,-1
2005d590:	c398                	c.sw	a4,0(a5)
2005d592:	8082                	c.jr	ra
2005d594:	4318                	c.lw	a4,0(a4)
2005d596:	4758                	c.lw	a4,12(a4)
2005d598:	4318                	c.lw	a4,0(a4)
2005d59a:	bfdd                	c.j	2005d590 <prvResetNextTaskUnblockTime+0x10>

2005d59c <prvDeleteTCB>:
2005d59c:	1141                	c.addi	sp,-16
2005d59e:	c422                	c.swsp	s0,8(sp)
2005d5a0:	842a                	c.mv	s0,a0
2005d5a2:	06c50513          	addi	a0,a0,108
2005d5a6:	c606                	c.swsp	ra,12(sp)
2005d5a8:	688040ef          	jal	ra,20061c30 <_reclaim_reent>
2005d5ac:	16144783          	lbu	a5,353(s0)
2005d5b0:	eb91                	c.bnez	a5,2005d5c4 <prvDeleteTCB+0x28>
2005d5b2:	5808                	c.lw	a0,48(s0)
2005d5b4:	f4cfd0ef          	jal	ra,2005ad00 <vPortFree>
2005d5b8:	8522                	c.mv	a0,s0
2005d5ba:	4422                	c.lwsp	s0,8(sp)
2005d5bc:	40b2                	c.lwsp	ra,12(sp)
2005d5be:	0141                	c.addi	sp,16
2005d5c0:	f40fd06f          	jal	zero,2005ad00 <vPortFree>
2005d5c4:	4705                	c.li	a4,1
2005d5c6:	fee789e3          	beq	a5,a4,2005d5b8 <prvDeleteTCB+0x1c>
2005d5ca:	40b2                	c.lwsp	ra,12(sp)
2005d5cc:	4422                	c.lwsp	s0,8(sp)
2005d5ce:	0141                	c.addi	sp,16
2005d5d0:	8082                	c.jr	ra

2005d5d2 <prvInitialiseNewTask.constprop.0>:
2005d5d2:	1101                	c.addi	sp,-32
2005d5d4:	c452                	c.swsp	s4,8(sp)
2005d5d6:	8a2a                	c.mv	s4,a0
2005d5d8:	03082503          	lw	a0,48(a6)
2005d5dc:	c64e                	c.swsp	s3,12(sp)
2005d5de:	00261993          	slli	s3,a2,0x2
2005d5e2:	864e                	c.mv	a2,s3
2005d5e4:	ca26                	c.swsp	s1,20(sp)
2005d5e6:	84ae                	c.mv	s1,a1
2005d5e8:	0a500593          	addi	a1,zero,165
2005d5ec:	cc22                	c.swsp	s0,24(sp)
2005d5ee:	c84a                	c.swsp	s2,16(sp)
2005d5f0:	8442                	c.mv	s0,a6
2005d5f2:	c256                	c.swsp	s5,4(sp)
2005d5f4:	c05a                	c.swsp	s6,0(sp)
2005d5f6:	ce06                	c.swsp	ra,28(sp)
2005d5f8:	8ab6                	c.mv	s5,a3
2005d5fa:	893a                	c.mv	s2,a4
2005d5fc:	8b3e                	c.mv	s6,a5
2005d5fe:	dffac097          	auipc	ra,0xdffac
2005d602:	4d0080e7          	jalr	ra,1232(ra) # 9ace <__wrap_memset>
2005d606:	ffc98613          	addi	a2,s3,-4 # ffc <CPU_ClkGet+0x4e6>
2005d60a:	03042983          	lw	s3,48(s0)
2005d60e:	99b2                	c.add	s3,a2
2005d610:	fe09f993          	andi	s3,s3,-32
2005d614:	05342623          	sw	s3,76(s0)
2005d618:	c8f9                	c.beqz	s1,2005d6ee <prvInitialiseNewTask.constprop.0+0x11c>
2005d61a:	85a6                	c.mv	a1,s1
2005d61c:	03440793          	addi	a5,s0,52
2005d620:	01848693          	addi	a3,s1,24
2005d624:	0005c703          	lbu	a4,0(a1) # 1000 <CPU_ClkGet+0x4ea>
2005d628:	00e78023          	sb	a4,0(a5)
2005d62c:	c709                	c.beqz	a4,2005d636 <prvInitialiseNewTask.constprop.0+0x64>
2005d62e:	0585                	c.addi	a1,1
2005d630:	0785                	c.addi	a5,1
2005d632:	fed599e3          	bne	a1,a3,2005d624 <prvInitialiseNewTask.constprop.0+0x52>
2005d636:	040405a3          	sb	zero,75(s0)
2005d63a:	47a9                	c.li	a5,10
2005d63c:	0127f363          	bgeu	a5,s2,2005d642 <prvInitialiseNewTask.constprop.0+0x70>
2005d640:	4929                	c.li	s2,10
2005d642:	00440513          	addi	a0,s0,4
2005d646:	03242623          	sw	s2,44(s0)
2005d64a:	05242e23          	sw	s2,92(s0)
2005d64e:	06042023          	sw	zero,96(s0)
2005d652:	81ffd0ef          	jal	ra,2005ae70 <vListInitialiseItem>
2005d656:	01840513          	addi	a0,s0,24
2005d65a:	817fd0ef          	jal	ra,2005ae70 <vListInitialiseItem>
2005d65e:	472d                	c.li	a4,11
2005d660:	412704b3          	sub	s1,a4,s2
2005d664:	4611                	c.li	a2,4
2005d666:	4581                	c.li	a1,0
2005d668:	c800                	c.sw	s0,16(s0)
2005d66a:	cc04                	c.sw	s1,24(s0)
2005d66c:	d040                	c.sw	s0,36(s0)
2005d66e:	04042823          	sw	zero,80(s0)
2005d672:	06042223          	sw	zero,100(s0)
2005d676:	06042423          	sw	zero,104(s0)
2005d67a:	15c40513          	addi	a0,s0,348
2005d67e:	dffac097          	auipc	ra,0xdffac
2005d682:	450080e7          	jalr	ra,1104(ra) # 9ace <__wrap_memset>
2005d686:	4605                	c.li	a2,1
2005d688:	4581                	c.li	a1,0
2005d68a:	16040513          	addi	a0,s0,352
2005d68e:	dffac097          	auipc	ra,0xdffac
2005d692:	440080e7          	jalr	ra,1088(ra) # 9ace <__wrap_memset>
2005d696:	0f000613          	addi	a2,zero,240
2005d69a:	4581                	c.li	a1,0
2005d69c:	06c40513          	addi	a0,s0,108
2005d6a0:	dffac097          	auipc	ra,0xdffac
2005d6a4:	42e080e7          	jalr	ra,1070(ra) # 9ace <__wrap_memset>
2005d6a8:	200697b7          	lui	a5,0x20069
2005d6ac:	9c478793          	addi	a5,a5,-1596 # 200689c4 <__sf>
2005d6b0:	d83c                	c.sw	a5,112(s0)
2005d6b2:	200697b7          	lui	a5,0x20069
2005d6b6:	a2c78793          	addi	a5,a5,-1492 # 20068a2c <__sf+0x68>
2005d6ba:	d87c                	c.sw	a5,116(s0)
2005d6bc:	200697b7          	lui	a5,0x20069
2005d6c0:	a9478793          	addi	a5,a5,-1388 # 20068a94 <__sf+0xd0>
2005d6c4:	dc3c                	c.sw	a5,120(s0)
2005d6c6:	8656                	c.mv	a2,s5
2005d6c8:	85d2                	c.mv	a1,s4
2005d6ca:	854e                	c.mv	a0,s3
2005d6cc:	ae5fe0ef          	jal	ra,2005c1b0 <pxPortInitialiseStack>
2005d6d0:	c008                	c.sw	a0,0(s0)
2005d6d2:	000b0463          	beq	s6,zero,2005d6da <prvInitialiseNewTask.constprop.0+0x108>
2005d6d6:	008b2023          	sw	s0,0(s6)
2005d6da:	40f2                	c.lwsp	ra,28(sp)
2005d6dc:	4462                	c.lwsp	s0,24(sp)
2005d6de:	44d2                	c.lwsp	s1,20(sp)
2005d6e0:	4942                	c.lwsp	s2,16(sp)
2005d6e2:	49b2                	c.lwsp	s3,12(sp)
2005d6e4:	4a22                	c.lwsp	s4,8(sp)
2005d6e6:	4a92                	c.lwsp	s5,4(sp)
2005d6e8:	4b02                	c.lwsp	s6,0(sp)
2005d6ea:	6105                	c.addi16sp	sp,32
2005d6ec:	8082                	c.jr	ra
2005d6ee:	02040a23          	sb	zero,52(s0)
2005d6f2:	b7a1                	c.j	2005d63a <prvInitialiseNewTask.constprop.0+0x68>

2005d6f4 <vTaskSuspendAll>:
2005d6f4:	f7818793          	addi	a5,gp,-136 # 20066900 <uxSchedulerSuspended>
2005d6f8:	4398                	c.lw	a4,0(a5)
2005d6fa:	0705                	c.addi	a4,1
2005d6fc:	c398                	c.sw	a4,0(a5)
2005d6fe:	8082                	c.jr	ra

2005d700 <xTaskGetTickCountFromISR>:
2005d700:	f941a503          	lw	a0,-108(gp) # 2006691c <xTickCount>
2005d704:	8082                	c.jr	ra

2005d706 <uxTaskGetNumberOfTasks>:
2005d706:	f701a503          	lw	a0,-144(gp) # 200668f8 <uxCurrentNumberOfTasks>
2005d70a:	8082                	c.jr	ra

2005d70c <pcTaskGetName>:
2005d70c:	e119                	c.bnez	a0,2005d712 <pcTaskGetName+0x6>
2005d70e:	f5c1a503          	lw	a0,-164(gp) # 200668e4 <pxCurrentTCB>
2005d712:	03450513          	addi	a0,a0,52
2005d716:	8082                	c.jr	ra

2005d718 <xTaskIncrementTick>:
2005d718:	f781a783          	lw	a5,-136(gp) # 20066900 <uxSchedulerSuspended>
2005d71c:	7179                	c.addi16sp	sp,-48
2005d71e:	d606                	c.swsp	ra,44(sp)
2005d720:	d422                	c.swsp	s0,40(sp)
2005d722:	d226                	c.swsp	s1,36(sp)
2005d724:	d04a                	c.swsp	s2,32(sp)
2005d726:	ce4e                	c.swsp	s3,28(sp)
2005d728:	cc52                	c.swsp	s4,24(sp)
2005d72a:	ca56                	c.swsp	s5,20(sp)
2005d72c:	c85a                	c.swsp	s6,16(sp)
2005d72e:	c65e                	c.swsp	s7,12(sp)
2005d730:	c462                	c.swsp	s8,8(sp)
2005d732:	c266                	c.swsp	s9,4(sp)
2005d734:	c06a                	c.swsp	s10,0(sp)
2005d736:	10079363          	bne	a5,zero,2005d83c <xTaskIncrementTick+0x124>
2005d73a:	f9418793          	addi	a5,gp,-108 # 2006691c <xTickCount>
2005d73e:	0007aa03          	lw	s4,0(a5)
2005d742:	0a05                	c.addi	s4,1
2005d744:	0147a023          	sw	s4,0(a5)
2005d748:	020a1163          	bne	s4,zero,2005d76a <xTaskIncrementTick+0x52>
2005d74c:	f6018713          	addi	a4,gp,-160 # 200668e8 <pxDelayedTaskList>
2005d750:	f6418793          	addi	a5,gp,-156 # 200668ec <pxOverflowDelayedTaskList>
2005d754:	4314                	c.lw	a3,0(a4)
2005d756:	4390                	c.lw	a2,0(a5)
2005d758:	c310                	c.sw	a2,0(a4)
2005d75a:	c394                	c.sw	a3,0(a5)
2005d75c:	f8818793          	addi	a5,gp,-120 # 20066910 <xNumOfOverflows>
2005d760:	4398                	c.lw	a4,0(a5)
2005d762:	0705                	c.addi	a4,1
2005d764:	c398                	c.sw	a4,0(a5)
2005d766:	e1bff0ef          	jal	ra,2005d580 <prvResetNextTaskUnblockTime>
2005d76a:	f8418793          	addi	a5,gp,-124 # 2006690c <xNextTaskUnblockTime>
2005d76e:	439c                	c.lw	a5,0(a5)
2005d770:	200684b7          	lui	s1,0x20068
2005d774:	f8418993          	addi	s3,gp,-124 # 2006690c <xNextTaskUnblockTime>
2005d778:	2e848493          	addi	s1,s1,744 # 200682e8 <pxReadyTasksLists>
2005d77c:	f5c18a93          	addi	s5,gp,-164 # 200668e4 <pxCurrentTCB>
2005d780:	4401                	c.li	s0,0
2005d782:	04fa7663          	bgeu	s4,a5,2005d7ce <xTaskIncrementTick+0xb6>
2005d786:	000aa783          	lw	a5,0(s5)
2005d78a:	4751                	c.li	a4,20
2005d78c:	57dc                	c.lw	a5,44(a5)
2005d78e:	02e787b3          	mul	a5,a5,a4
2005d792:	94be                	c.add	s1,a5
2005d794:	4098                	c.lw	a4,0(s1)
2005d796:	4785                	c.li	a5,1
2005d798:	00e7f363          	bgeu	a5,a4,2005d79e <xTaskIncrementTick+0x86>
2005d79c:	4405                	c.li	s0,1
2005d79e:	f8c1a783          	lw	a5,-116(gp) # 20066914 <xPendedTicks>
2005d7a2:	e399                	c.bnez	a5,2005d7a8 <xTaskIncrementTick+0x90>
2005d7a4:	e41fe0ef          	jal	ra,2005c5e4 <vApplicationTickHook>
2005d7a8:	f981a783          	lw	a5,-104(gp) # 20066920 <xYieldPending>
2005d7ac:	c391                	c.beqz	a5,2005d7b0 <xTaskIncrementTick+0x98>
2005d7ae:	4405                	c.li	s0,1
2005d7b0:	50b2                	c.lwsp	ra,44(sp)
2005d7b2:	8522                	c.mv	a0,s0
2005d7b4:	5422                	c.lwsp	s0,40(sp)
2005d7b6:	5492                	c.lwsp	s1,36(sp)
2005d7b8:	5902                	c.lwsp	s2,32(sp)
2005d7ba:	49f2                	c.lwsp	s3,28(sp)
2005d7bc:	4a62                	c.lwsp	s4,24(sp)
2005d7be:	4ad2                	c.lwsp	s5,20(sp)
2005d7c0:	4b42                	c.lwsp	s6,16(sp)
2005d7c2:	4bb2                	c.lwsp	s7,12(sp)
2005d7c4:	4c22                	c.lwsp	s8,8(sp)
2005d7c6:	4c92                	c.lwsp	s9,4(sp)
2005d7c8:	4d02                	c.lwsp	s10,0(sp)
2005d7ca:	6145                	c.addi16sp	sp,48
2005d7cc:	8082                	c.jr	ra
2005d7ce:	4c05                	c.li	s8,1
2005d7d0:	4cd1                	c.li	s9,20
2005d7d2:	f6018793          	addi	a5,gp,-160 # 200668e8 <pxDelayedTaskList>
2005d7d6:	4398                	c.lw	a4,0(a5)
2005d7d8:	4318                	c.lw	a4,0(a4)
2005d7da:	e709                	c.bnez	a4,2005d7e4 <xTaskIncrementTick+0xcc>
2005d7dc:	57fd                	c.li	a5,-1
2005d7de:	00f9a023          	sw	a5,0(s3)
2005d7e2:	b755                	c.j	2005d786 <xTaskIncrementTick+0x6e>
2005d7e4:	439c                	c.lw	a5,0(a5)
2005d7e6:	47dc                	c.lw	a5,12(a5)
2005d7e8:	00c7a903          	lw	s2,12(a5)
2005d7ec:	00492783          	lw	a5,4(s2)
2005d7f0:	fefa67e3          	bltu	s4,a5,2005d7de <xTaskIncrementTick+0xc6>
2005d7f4:	00490d13          	addi	s10,s2,4
2005d7f8:	856a                	c.mv	a0,s10
2005d7fa:	ec0fd0ef          	jal	ra,2005aeba <uxListRemove>
2005d7fe:	02892783          	lw	a5,40(s2)
2005d802:	c789                	c.beqz	a5,2005d80c <xTaskIncrementTick+0xf4>
2005d804:	01890513          	addi	a0,s2,24
2005d808:	eb2fd0ef          	jal	ra,2005aeba <uxListRemove>
2005d80c:	02c92503          	lw	a0,44(s2)
2005d810:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005d814:	4314                	c.lw	a3,0(a4)
2005d816:	00ac17b3          	sll	a5,s8,a0
2005d81a:	03950533          	mul	a0,a0,s9
2005d81e:	8fd5                	c.or	a5,a3
2005d820:	85ea                	c.mv	a1,s10
2005d822:	c31c                	c.sw	a5,0(a4)
2005d824:	9526                	c.add	a0,s1
2005d826:	e50fd0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005d82a:	000aa783          	lw	a5,0(s5)
2005d82e:	02c92703          	lw	a4,44(s2)
2005d832:	57dc                	c.lw	a5,44(a5)
2005d834:	f8f76fe3          	bltu	a4,a5,2005d7d2 <xTaskIncrementTick+0xba>
2005d838:	4405                	c.li	s0,1
2005d83a:	bf61                	c.j	2005d7d2 <xTaskIncrementTick+0xba>
2005d83c:	f8c18793          	addi	a5,gp,-116 # 20066914 <xPendedTicks>
2005d840:	4398                	c.lw	a4,0(a5)
2005d842:	4401                	c.li	s0,0
2005d844:	0705                	c.addi	a4,1
2005d846:	c398                	c.sw	a4,0(a5)
2005d848:	d9dfe0ef          	jal	ra,2005c5e4 <vApplicationTickHook>
2005d84c:	b795                	c.j	2005d7b0 <xTaskIncrementTick+0x98>

2005d84e <vTaskSwitchContext>:
2005d84e:	f781a703          	lw	a4,-136(gp) # 20066900 <uxSchedulerSuspended>
2005d852:	f9818793          	addi	a5,gp,-104 # 20066920 <xYieldPending>
2005d856:	c701                	c.beqz	a4,2005d85e <vTaskSwitchContext+0x10>
2005d858:	4705                	c.li	a4,1
2005d85a:	c398                	c.sw	a4,0(a5)
2005d85c:	8082                	c.jr	ra
2005d85e:	1141                	c.addi	sp,-16
2005d860:	c422                	c.swsp	s0,8(sp)
2005d862:	c606                	c.swsp	ra,12(sp)
2005d864:	0007a023          	sw	zero,0(a5)
2005d868:	410187b7          	lui	a5,0x41018
2005d86c:	4bd4                	c.lw	a3,20(a5)
2005d86e:	f6c18713          	addi	a4,gp,-148 # 200668f4 <ulTotalRunTime>
2005d872:	c314                	c.sw	a3,0(a4)
2005d874:	f681a583          	lw	a1,-152(gp) # 200668f0 <ulTaskSwitchedInTime>
2005d878:	4318                	c.lw	a4,0(a4)
2005d87a:	f6c18793          	addi	a5,gp,-148 # 200668f4 <ulTotalRunTime>
2005d87e:	f6818693          	addi	a3,gp,-152 # 200668f0 <ulTaskSwitchedInTime>
2005d882:	f5c18413          	addi	s0,gp,-164 # 200668e4 <pxCurrentTCB>
2005d886:	00e5f863          	bgeu	a1,a4,2005d896 <vTaskSwitchContext+0x48>
2005d88a:	4388                	c.lw	a0,0(a5)
2005d88c:	4010                	c.lw	a2,0(s0)
2005d88e:	5638                	c.lw	a4,104(a2)
2005d890:	8f0d                	c.sub	a4,a1
2005d892:	972a                	c.add	a4,a0
2005d894:	d638                	c.sw	a4,104(a2)
2005d896:	439c                	c.lw	a5,0(a5)
2005d898:	a5a5a737          	lui	a4,0xa5a5a
2005d89c:	5a570713          	addi	a4,a4,1445 # a5a5a5a5 <__ctrace_end__+0xdba5a5a5>
2005d8a0:	c29c                	c.sw	a5,0(a3)
2005d8a2:	401c                	c.lw	a5,0(s0)
2005d8a4:	5b9c                	c.lw	a5,48(a5)
2005d8a6:	4390                	c.lw	a2,0(a5)
2005d8a8:	00e61b63          	bne	a2,a4,2005d8be <vTaskSwitchContext+0x70>
2005d8ac:	43d4                	c.lw	a3,4(a5)
2005d8ae:	00c69863          	bne	a3,a2,2005d8be <vTaskSwitchContext+0x70>
2005d8b2:	4798                	c.lw	a4,8(a5)
2005d8b4:	00d71563          	bne	a4,a3,2005d8be <vTaskSwitchContext+0x70>
2005d8b8:	47dc                	c.lw	a5,12(a5)
2005d8ba:	00e78863          	beq	a5,a4,2005d8ca <vTaskSwitchContext+0x7c>
2005d8be:	4008                	c.lw	a0,0(s0)
2005d8c0:	400c                	c.lw	a1,0(s0)
2005d8c2:	03458593          	addi	a1,a1,52
2005d8c6:	d77fe0ef          	jal	ra,2005c63c <vApplicationStackOverflowHook>
2005d8ca:	f801a503          	lw	a0,-128(gp) # 20066908 <uxTopReadyPriority>
2005d8ce:	3ef030ef          	jal	ra,200614bc <__clzsi2>
2005d8d2:	47fd                	c.li	a5,31
2005d8d4:	40a78533          	sub	a0,a5,a0
2005d8d8:	4751                	c.li	a4,20
2005d8da:	02e50733          	mul	a4,a0,a4
2005d8de:	200687b7          	lui	a5,0x20068
2005d8e2:	2e878693          	addi	a3,a5,744 # 200682e8 <pxReadyTasksLists>
2005d8e6:	2e878793          	addi	a5,a5,744
2005d8ea:	00e68633          	add	a2,a3,a4
2005d8ee:	424c                	c.lw	a1,4(a2)
2005d8f0:	0721                	c.addi	a4,8
2005d8f2:	9736                	c.add	a4,a3
2005d8f4:	41cc                	c.lw	a1,4(a1)
2005d8f6:	c24c                	c.sw	a1,4(a2)
2005d8f8:	00e59463          	bne	a1,a4,2005d900 <vTaskSwitchContext+0xb2>
2005d8fc:	41d8                	c.lw	a4,4(a1)
2005d8fe:	c258                	c.sw	a4,4(a2)
2005d900:	4751                	c.li	a4,20
2005d902:	02e50533          	mul	a0,a0,a4
2005d906:	40b2                	c.lwsp	ra,12(sp)
2005d908:	97aa                	c.add	a5,a0
2005d90a:	43dc                	c.lw	a5,4(a5)
2005d90c:	47dc                	c.lw	a5,12(a5)
2005d90e:	c01c                	c.sw	a5,0(s0)
2005d910:	401c                	c.lw	a5,0(s0)
2005d912:	4422                	c.lwsp	s0,8(sp)
2005d914:	06c78793          	addi	a5,a5,108
2005d918:	e8f1aa23          	sw	a5,-364(gp) # 2006681c <_impure_ptr>
2005d91c:	0141                	c.addi	sp,16
2005d91e:	8082                	c.jr	ra

2005d920 <vTaskPlaceOnEventList>:
2005d920:	1141                	c.addi	sp,-16
2005d922:	c422                	c.swsp	s0,8(sp)
2005d924:	842e                	c.mv	s0,a1
2005d926:	f5c1a583          	lw	a1,-164(gp) # 200668e4 <pxCurrentTCB>
2005d92a:	c606                	c.swsp	ra,12(sp)
2005d92c:	05e1                	c.addi	a1,24
2005d92e:	d5efd0ef          	jal	ra,2005ae8c <vListInsert>
2005d932:	8522                	c.mv	a0,s0
2005d934:	4422                	c.lwsp	s0,8(sp)
2005d936:	40b2                	c.lwsp	ra,12(sp)
2005d938:	4585                	c.li	a1,1
2005d93a:	0141                	c.addi	sp,16
2005d93c:	b21ff06f          	jal	zero,2005d45c <prvAddCurrentTaskToDelayedList>

2005d940 <vTaskPlaceOnEventListRestricted>:
2005d940:	1141                	c.addi	sp,-16
2005d942:	c422                	c.swsp	s0,8(sp)
2005d944:	842e                	c.mv	s0,a1
2005d946:	f5c1a583          	lw	a1,-164(gp) # 200668e4 <pxCurrentTCB>
2005d94a:	c226                	c.swsp	s1,4(sp)
2005d94c:	84b2                	c.mv	s1,a2
2005d94e:	05e1                	c.addi	a1,24
2005d950:	c606                	c.swsp	ra,12(sp)
2005d952:	d24fd0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005d956:	c091                	c.beqz	s1,2005d95a <vTaskPlaceOnEventListRestricted+0x1a>
2005d958:	547d                	c.li	s0,-1
2005d95a:	8522                	c.mv	a0,s0
2005d95c:	4422                	c.lwsp	s0,8(sp)
2005d95e:	40b2                	c.lwsp	ra,12(sp)
2005d960:	85a6                	c.mv	a1,s1
2005d962:	4492                	c.lwsp	s1,4(sp)
2005d964:	0141                	c.addi	sp,16
2005d966:	af7ff06f          	jal	zero,2005d45c <prvAddCurrentTaskToDelayedList>

2005d96a <xTaskRemoveFromEventList>:
2005d96a:	1101                	c.addi	sp,-32
2005d96c:	ce06                	c.swsp	ra,28(sp)
2005d96e:	cc22                	c.swsp	s0,24(sp)
2005d970:	455c                	c.lw	a5,12(a0)
2005d972:	47c0                	c.lw	s0,12(a5)
2005d974:	01840593          	addi	a1,s0,24
2005d978:	852e                	c.mv	a0,a1
2005d97a:	c62e                	c.swsp	a1,12(sp)
2005d97c:	d3efd0ef          	jal	ra,2005aeba <uxListRemove>
2005d980:	f781a783          	lw	a5,-136(gp) # 20066900 <uxSchedulerSuspended>
2005d984:	45b2                	c.lwsp	a1,12(sp)
2005d986:	efa1                	c.bnez	a5,2005d9de <xTaskRemoveFromEventList+0x74>
2005d988:	00440593          	addi	a1,s0,4
2005d98c:	852e                	c.mv	a0,a1
2005d98e:	c62e                	c.swsp	a1,12(sp)
2005d990:	d2afd0ef          	jal	ra,2005aeba <uxListRemove>
2005d994:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005d998:	5448                	c.lw	a0,44(s0)
2005d99a:	4314                	c.lw	a3,0(a4)
2005d99c:	4785                	c.li	a5,1
2005d99e:	00a797b3          	sll	a5,a5,a0
2005d9a2:	8fd5                	c.or	a5,a3
2005d9a4:	c31c                	c.sw	a5,0(a4)
2005d9a6:	47d1                	c.li	a5,20
2005d9a8:	02f50533          	mul	a0,a0,a5
2005d9ac:	45b2                	c.lwsp	a1,12(sp)
2005d9ae:	200687b7          	lui	a5,0x20068
2005d9b2:	2e878793          	addi	a5,a5,744 # 200682e8 <pxReadyTasksLists>
2005d9b6:	953e                	c.add	a0,a5
2005d9b8:	cbefd0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005d9bc:	bc5ff0ef          	jal	ra,2005d580 <prvResetNextTaskUnblockTime>
2005d9c0:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005d9c4:	5458                	c.lw	a4,44(s0)
2005d9c6:	4501                	c.li	a0,0
2005d9c8:	57dc                	c.lw	a5,44(a5)
2005d9ca:	00e7f663          	bgeu	a5,a4,2005d9d6 <xTaskRemoveFromEventList+0x6c>
2005d9ce:	4705                	c.li	a4,1
2005d9d0:	f8e1ac23          	sw	a4,-104(gp) # 20066920 <xYieldPending>
2005d9d4:	4505                	c.li	a0,1
2005d9d6:	40f2                	c.lwsp	ra,28(sp)
2005d9d8:	4462                	c.lwsp	s0,24(sp)
2005d9da:	6105                	c.addi16sp	sp,32
2005d9dc:	8082                	c.jr	ra
2005d9de:	20068537          	lui	a0,0x20068
2005d9e2:	3ec50513          	addi	a0,a0,1004 # 200683ec <xPendingReadyList>
2005d9e6:	c90fd0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005d9ea:	bfd9                	c.j	2005d9c0 <xTaskRemoveFromEventList+0x56>

2005d9ec <vTaskInternalSetTimeOutState>:
2005d9ec:	f881a783          	lw	a5,-120(gp) # 20066910 <xNumOfOverflows>
2005d9f0:	c11c                	c.sw	a5,0(a0)
2005d9f2:	f941a783          	lw	a5,-108(gp) # 2006691c <xTickCount>
2005d9f6:	c15c                	c.sw	a5,4(a0)
2005d9f8:	8082                	c.jr	ra

2005d9fa <vTaskMissedYield>:
2005d9fa:	4705                	c.li	a4,1
2005d9fc:	f8e1ac23          	sw	a4,-104(gp) # 20066920 <xYieldPending>
2005da00:	8082                	c.jr	ra

2005da02 <eTaskConfirmSleepModeStatus>:
2005da02:	200687b7          	lui	a5,0x20068
2005da06:	3ec7a783          	lw	a5,1004(a5) # 200683ec <xPendingReadyList>
2005da0a:	4501                	c.li	a0,0
2005da0c:	e395                	c.bnez	a5,2005da30 <eTaskConfirmSleepModeStatus+0x2e>
2005da0e:	f981a783          	lw	a5,-104(gp) # 20066920 <xYieldPending>
2005da12:	ef99                	c.bnez	a5,2005da30 <eTaskConfirmSleepModeStatus+0x2e>
2005da14:	f8c1a783          	lw	a5,-116(gp) # 20066914 <xPendedTicks>
2005da18:	ef81                	c.bnez	a5,2005da30 <eTaskConfirmSleepModeStatus+0x2e>
2005da1a:	200687b7          	lui	a5,0x20068
2005da1e:	4007a703          	lw	a4,1024(a5) # 20068400 <xSuspendedTaskList>
2005da22:	f701a783          	lw	a5,-144(gp) # 200668f8 <uxCurrentNumberOfTasks>
2005da26:	4505                	c.li	a0,1
2005da28:	17fd                	c.addi	a5,-1
2005da2a:	00f71363          	bne	a4,a5,2005da30 <eTaskConfirmSleepModeStatus+0x2e>
2005da2e:	4509                	c.li	a0,2
2005da30:	8082                	c.jr	ra

2005da32 <xTaskGetCurrentTaskHandle>:
2005da32:	f5c1a503          	lw	a0,-164(gp) # 200668e4 <pxCurrentTCB>
2005da36:	8082                	c.jr	ra

2005da38 <xTaskGetSchedulerState>:
2005da38:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005da3c:	4505                	c.li	a0,1
2005da3e:	c791                	c.beqz	a5,2005da4a <xTaskGetSchedulerState+0x12>
2005da40:	f781a503          	lw	a0,-136(gp) # 20066900 <uxSchedulerSuspended>
2005da44:	00153513          	sltiu	a0,a0,1
2005da48:	0506                	c.slli	a0,0x1
2005da4a:	8082                	c.jr	ra

2005da4c <xTaskPriorityInherit>:
2005da4c:	c945                	c.beqz	a0,2005dafc <xTaskPriorityInherit+0xb0>
2005da4e:	1101                	c.addi	sp,-32
2005da50:	ca26                	c.swsp	s1,20(sp)
2005da52:	cc22                	c.swsp	s0,24(sp)
2005da54:	ce06                	c.swsp	ra,28(sp)
2005da56:	c84a                	c.swsp	s2,16(sp)
2005da58:	f5c18713          	addi	a4,gp,-164 # 200668e4 <pxCurrentTCB>
2005da5c:	4314                	c.lw	a3,0(a4)
2005da5e:	555c                	c.lw	a5,44(a0)
2005da60:	842a                	c.mv	s0,a0
2005da62:	56d4                	c.lw	a3,44(a3)
2005da64:	f5c18493          	addi	s1,gp,-164 # 200668e4 <pxCurrentTCB>
2005da68:	08d7f463          	bgeu	a5,a3,2005daf0 <xTaskPriorityInherit+0xa4>
2005da6c:	4d18                	c.lw	a4,24(a0)
2005da6e:	00074763          	blt	a4,zero,2005da7c <xTaskPriorityInherit+0x30>
2005da72:	4098                	c.lw	a4,0(s1)
2005da74:	5754                	c.lw	a3,44(a4)
2005da76:	472d                	c.li	a4,11
2005da78:	8f15                	c.sub	a4,a3
2005da7a:	cd18                	c.sw	a4,24(a0)
2005da7c:	46d1                	c.li	a3,20
2005da7e:	02d787b3          	mul	a5,a5,a3
2005da82:	20068537          	lui	a0,0x20068
2005da86:	2e850713          	addi	a4,a0,744 # 200682e8 <pxReadyTasksLists>
2005da8a:	2e850913          	addi	s2,a0,744
2005da8e:	97ba                	c.add	a5,a4
2005da90:	4858                	c.lw	a4,20(s0)
2005da92:	04f71b63          	bne	a4,a5,2005dae8 <xTaskPriorityInherit+0x9c>
2005da96:	00440593          	addi	a1,s0,4
2005da9a:	852e                	c.mv	a0,a1
2005da9c:	c62e                	c.swsp	a1,12(sp)
2005da9e:	c1cfd0ef          	jal	ra,2005aeba <uxListRemove>
2005daa2:	45b2                	c.lwsp	a1,12(sp)
2005daa4:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005daa8:	e911                	c.bnez	a0,2005dabc <xTaskPriorityInherit+0x70>
2005daaa:	5450                	c.lw	a2,44(s0)
2005daac:	4314                	c.lw	a3,0(a4)
2005daae:	4785                	c.li	a5,1
2005dab0:	00c797b3          	sll	a5,a5,a2
2005dab4:	fff7c793          	xori	a5,a5,-1
2005dab8:	8ff5                	c.and	a5,a3
2005daba:	c31c                	c.sw	a5,0(a4)
2005dabc:	409c                	c.lw	a5,0(s1)
2005dabe:	4314                	c.lw	a3,0(a4)
2005dac0:	57c8                	c.lw	a0,44(a5)
2005dac2:	4785                	c.li	a5,1
2005dac4:	00a797b3          	sll	a5,a5,a0
2005dac8:	8fd5                	c.or	a5,a3
2005daca:	c31c                	c.sw	a5,0(a4)
2005dacc:	47d1                	c.li	a5,20
2005dace:	d448                	c.sw	a0,44(s0)
2005dad0:	02f50533          	mul	a0,a0,a5
2005dad4:	954a                	c.add	a0,s2
2005dad6:	ba0fd0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005dada:	4505                	c.li	a0,1
2005dadc:	40f2                	c.lwsp	ra,28(sp)
2005dade:	4462                	c.lwsp	s0,24(sp)
2005dae0:	44d2                	c.lwsp	s1,20(sp)
2005dae2:	4942                	c.lwsp	s2,16(sp)
2005dae4:	6105                	c.addi16sp	sp,32
2005dae6:	8082                	c.jr	ra
2005dae8:	409c                	c.lw	a5,0(s1)
2005daea:	57dc                	c.lw	a5,44(a5)
2005daec:	d45c                	c.sw	a5,44(s0)
2005daee:	b7f5                	c.j	2005dada <xTaskPriorityInherit+0x8e>
2005daf0:	431c                	c.lw	a5,0(a4)
2005daf2:	4d68                	c.lw	a0,92(a0)
2005daf4:	57dc                	c.lw	a5,44(a5)
2005daf6:	00f53533          	sltu	a0,a0,a5
2005dafa:	b7cd                	c.j	2005dadc <xTaskPriorityInherit+0x90>
2005dafc:	4501                	c.li	a0,0
2005dafe:	8082                	c.jr	ra

2005db00 <xTaskPriorityDisinherit>:
2005db00:	e901                	c.bnez	a0,2005db10 <xTaskPriorityDisinherit+0x10>
2005db02:	4501                	c.li	a0,0
2005db04:	8082                	c.jr	ra
2005db06:	4501                	c.li	a0,0
2005db08:	40f2                	c.lwsp	ra,28(sp)
2005db0a:	4462                	c.lwsp	s0,24(sp)
2005db0c:	6105                	c.addi16sp	sp,32
2005db0e:	8082                	c.jr	ra
2005db10:	1101                	c.addi	sp,-32
2005db12:	cc22                	c.swsp	s0,24(sp)
2005db14:	ce06                	c.swsp	ra,28(sp)
2005db16:	513c                	c.lw	a5,96(a0)
2005db18:	5554                	c.lw	a3,44(a0)
2005db1a:	4d78                	c.lw	a4,92(a0)
2005db1c:	17fd                	c.addi	a5,-1
2005db1e:	d13c                	c.sw	a5,96(a0)
2005db20:	842a                	c.mv	s0,a0
2005db22:	fee682e3          	beq	a3,a4,2005db06 <xTaskPriorityDisinherit+0x6>
2005db26:	f3e5                	c.bnez	a5,2005db06 <xTaskPriorityDisinherit+0x6>
2005db28:	00450593          	addi	a1,a0,4
2005db2c:	852e                	c.mv	a0,a1
2005db2e:	c62e                	c.swsp	a1,12(sp)
2005db30:	b8afd0ef          	jal	ra,2005aeba <uxListRemove>
2005db34:	45b2                	c.lwsp	a1,12(sp)
2005db36:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005db3a:	e911                	c.bnez	a0,2005db4e <xTaskPriorityDisinherit+0x4e>
2005db3c:	5450                	c.lw	a2,44(s0)
2005db3e:	4314                	c.lw	a3,0(a4)
2005db40:	4785                	c.li	a5,1
2005db42:	00c797b3          	sll	a5,a5,a2
2005db46:	fff7c793          	xori	a5,a5,-1
2005db4a:	8ff5                	c.and	a5,a3
2005db4c:	c31c                	c.sw	a5,0(a4)
2005db4e:	4c68                	c.lw	a0,92(s0)
2005db50:	47ad                	c.li	a5,11
2005db52:	4314                	c.lw	a3,0(a4)
2005db54:	8f89                	c.sub	a5,a0
2005db56:	cc1c                	c.sw	a5,24(s0)
2005db58:	4785                	c.li	a5,1
2005db5a:	00a797b3          	sll	a5,a5,a0
2005db5e:	8fd5                	c.or	a5,a3
2005db60:	c31c                	c.sw	a5,0(a4)
2005db62:	47d1                	c.li	a5,20
2005db64:	d448                	c.sw	a0,44(s0)
2005db66:	02f50533          	mul	a0,a0,a5
2005db6a:	200687b7          	lui	a5,0x20068
2005db6e:	2e878793          	addi	a5,a5,744 # 200682e8 <pxReadyTasksLists>
2005db72:	953e                	c.add	a0,a5
2005db74:	b02fd0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005db78:	4505                	c.li	a0,1
2005db7a:	b779                	c.j	2005db08 <xTaskPriorityDisinherit+0x8>

2005db7c <vTaskPriorityDisinheritAfterTimeout>:
2005db7c:	cd51                	c.beqz	a0,2005dc18 <vTaskPriorityDisinheritAfterTimeout+0x9c>
2005db7e:	1101                	c.addi	sp,-32
2005db80:	cc22                	c.swsp	s0,24(sp)
2005db82:	ce06                	c.swsp	ra,28(sp)
2005db84:	ca26                	c.swsp	s1,20(sp)
2005db86:	4d7c                	c.lw	a5,92(a0)
2005db88:	842a                	c.mv	s0,a0
2005db8a:	00b7f363          	bgeu	a5,a1,2005db90 <vTaskPriorityDisinheritAfterTimeout+0x14>
2005db8e:	87ae                	c.mv	a5,a1
2005db90:	5458                	c.lw	a4,44(s0)
2005db92:	06f70e63          	beq	a4,a5,2005dc0e <vTaskPriorityDisinheritAfterTimeout+0x92>
2005db96:	5030                	c.lw	a2,96(s0)
2005db98:	4685                	c.li	a3,1
2005db9a:	06d61a63          	bne	a2,a3,2005dc0e <vTaskPriorityDisinheritAfterTimeout+0x92>
2005db9e:	4c14                	c.lw	a3,24(s0)
2005dba0:	d45c                	c.sw	a5,44(s0)
2005dba2:	0006c663          	blt	a3,zero,2005dbae <vTaskPriorityDisinheritAfterTimeout+0x32>
2005dba6:	46ad                	c.li	a3,11
2005dba8:	40f687b3          	sub	a5,a3,a5
2005dbac:	cc1c                	c.sw	a5,24(s0)
2005dbae:	46d1                	c.li	a3,20
2005dbb0:	02d70733          	mul	a4,a4,a3
2005dbb4:	20068537          	lui	a0,0x20068
2005dbb8:	2e850793          	addi	a5,a0,744 # 200682e8 <pxReadyTasksLists>
2005dbbc:	2e850493          	addi	s1,a0,744
2005dbc0:	973e                	c.add	a4,a5
2005dbc2:	485c                	c.lw	a5,20(s0)
2005dbc4:	04e79563          	bne	a5,a4,2005dc0e <vTaskPriorityDisinheritAfterTimeout+0x92>
2005dbc8:	00440593          	addi	a1,s0,4
2005dbcc:	852e                	c.mv	a0,a1
2005dbce:	c62e                	c.swsp	a1,12(sp)
2005dbd0:	aeafd0ef          	jal	ra,2005aeba <uxListRemove>
2005dbd4:	5454                	c.lw	a3,44(s0)
2005dbd6:	45b2                	c.lwsp	a1,12(sp)
2005dbd8:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005dbdc:	e909                	c.bnez	a0,2005dbee <vTaskPriorityDisinheritAfterTimeout+0x72>
2005dbde:	4310                	c.lw	a2,0(a4)
2005dbe0:	4785                	c.li	a5,1
2005dbe2:	00d797b3          	sll	a5,a5,a3
2005dbe6:	fff7c793          	xori	a5,a5,-1
2005dbea:	8ff1                	c.and	a5,a2
2005dbec:	c31c                	c.sw	a5,0(a4)
2005dbee:	4551                	c.li	a0,20
2005dbf0:	02a68533          	mul	a0,a3,a0
2005dbf4:	4310                	c.lw	a2,0(a4)
2005dbf6:	4785                	c.li	a5,1
2005dbf8:	00d797b3          	sll	a5,a5,a3
2005dbfc:	8fd1                	c.or	a5,a2
2005dbfe:	c31c                	c.sw	a5,0(a4)
2005dc00:	4462                	c.lwsp	s0,24(sp)
2005dc02:	40f2                	c.lwsp	ra,28(sp)
2005dc04:	9526                	c.add	a0,s1
2005dc06:	44d2                	c.lwsp	s1,20(sp)
2005dc08:	6105                	c.addi16sp	sp,32
2005dc0a:	a6cfd06f          	jal	zero,2005ae76 <vListInsertEnd>
2005dc0e:	40f2                	c.lwsp	ra,28(sp)
2005dc10:	4462                	c.lwsp	s0,24(sp)
2005dc12:	44d2                	c.lwsp	s1,20(sp)
2005dc14:	6105                	c.addi16sp	sp,32
2005dc16:	8082                	c.jr	ra
2005dc18:	8082                	c.jr	ra

2005dc1a <vTaskEnterCritical>:
2005dc1a:	30047073          	csrrci	zero,mstatus,8
2005dc1e:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dc22:	cb81                	c.beqz	a5,2005dc32 <vTaskEnterCritical+0x18>
2005dc24:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005dc28:	4394                	c.lw	a3,0(a5)
2005dc2a:	439c                	c.lw	a5,0(a5)
2005dc2c:	4ab8                	c.lw	a4,80(a3)
2005dc2e:	0705                	c.addi	a4,1
2005dc30:	cab8                	c.sw	a4,80(a3)
2005dc32:	8082                	c.jr	ra

2005dc34 <vTaskExitCritical>:
2005dc34:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dc38:	cf99                	c.beqz	a5,2005dc56 <vTaskExitCritical+0x22>
2005dc3a:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005dc3e:	4398                	c.lw	a4,0(a5)
2005dc40:	4b38                	c.lw	a4,80(a4)
2005dc42:	cb11                	c.beqz	a4,2005dc56 <vTaskExitCritical+0x22>
2005dc44:	4394                	c.lw	a3,0(a5)
2005dc46:	439c                	c.lw	a5,0(a5)
2005dc48:	4ab8                	c.lw	a4,80(a3)
2005dc4a:	177d                	c.addi	a4,-1
2005dc4c:	cab8                	c.sw	a4,80(a3)
2005dc4e:	4bbc                	c.lw	a5,80(a5)
2005dc50:	e399                	c.bnez	a5,2005dc56 <vTaskExitCritical+0x22>
2005dc52:	30046073          	csrrsi	zero,mstatus,8
2005dc56:	8082                	c.jr	ra

2005dc58 <prvAddNewTaskToReadyList>:
2005dc58:	1101                	c.addi	sp,-32
2005dc5a:	cc22                	c.swsp	s0,24(sp)
2005dc5c:	ca26                	c.swsp	s1,20(sp)
2005dc5e:	c84a                	c.swsp	s2,16(sp)
2005dc60:	c64e                	c.swsp	s3,12(sp)
2005dc62:	ce06                	c.swsp	ra,28(sp)
2005dc64:	c452                	c.swsp	s4,8(sp)
2005dc66:	842a                	c.mv	s0,a0
2005dc68:	fb3ff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005dc6c:	f7018793          	addi	a5,gp,-144 # 200668f8 <uxCurrentNumberOfTasks>
2005dc70:	4398                	c.lw	a4,0(a5)
2005dc72:	20068937          	lui	s2,0x20068
2005dc76:	0705                	c.addi	a4,1
2005dc78:	c398                	c.sw	a4,0(a5)
2005dc7a:	f5c18713          	addi	a4,gp,-164 # 200668e4 <pxCurrentTCB>
2005dc7e:	4318                	c.lw	a4,0(a4)
2005dc80:	f5c18493          	addi	s1,gp,-164 # 200668e4 <pxCurrentTCB>
2005dc84:	2e890993          	addi	s3,s2,744 # 200682e8 <pxReadyTasksLists>
2005dc88:	e369                	c.bnez	a4,2005dd4a <prvAddNewTaskToReadyList+0xf2>
2005dc8a:	c080                	c.sw	s0,0(s1)
2005dc8c:	4398                	c.lw	a4,0(a5)
2005dc8e:	4785                	c.li	a5,1
2005dc90:	06f71263          	bne	a4,a5,2005dcf4 <prvAddNewTaskToReadyList+0x9c>
2005dc94:	2e890913          	addi	s2,s2,744
2005dc98:	0dc98a13          	addi	s4,s3,220
2005dc9c:	854a                	c.mv	a0,s2
2005dc9e:	0951                	c.addi	s2,20
2005dca0:	9bcfd0ef          	jal	ra,2005ae5c <vListInitialise>
2005dca4:	ff2a1ce3          	bne	s4,s2,2005dc9c <prvAddNewTaskToReadyList+0x44>
2005dca8:	20068537          	lui	a0,0x20068
2005dcac:	3c450a13          	addi	s4,a0,964 # 200683c4 <xDelayedTaskList1>
2005dcb0:	3c450513          	addi	a0,a0,964
2005dcb4:	9a8fd0ef          	jal	ra,2005ae5c <vListInitialise>
2005dcb8:	20068537          	lui	a0,0x20068
2005dcbc:	3d850913          	addi	s2,a0,984 # 200683d8 <xDelayedTaskList2>
2005dcc0:	3d850513          	addi	a0,a0,984
2005dcc4:	998fd0ef          	jal	ra,2005ae5c <vListInitialise>
2005dcc8:	20068537          	lui	a0,0x20068
2005dccc:	3ec50513          	addi	a0,a0,1004 # 200683ec <xPendingReadyList>
2005dcd0:	98cfd0ef          	jal	ra,2005ae5c <vListInitialise>
2005dcd4:	20068537          	lui	a0,0x20068
2005dcd8:	41450513          	addi	a0,a0,1044 # 20068414 <xTasksWaitingTermination>
2005dcdc:	980fd0ef          	jal	ra,2005ae5c <vListInitialise>
2005dce0:	20068537          	lui	a0,0x20068
2005dce4:	40050513          	addi	a0,a0,1024 # 20068400 <xSuspendedTaskList>
2005dce8:	974fd0ef          	jal	ra,2005ae5c <vListInitialise>
2005dcec:	f741a023          	sw	s4,-160(gp) # 200668e8 <pxDelayedTaskList>
2005dcf0:	f721a223          	sw	s2,-156(gp) # 200668ec <pxOverflowDelayedTaskList>
2005dcf4:	f7c18713          	addi	a4,gp,-132 # 20066904 <uxTaskNumber>
2005dcf8:	431c                	c.lw	a5,0(a4)
2005dcfa:	5448                	c.lw	a0,44(s0)
2005dcfc:	00440593          	addi	a1,s0,4
2005dd00:	0785                	c.addi	a5,1
2005dd02:	c31c                	c.sw	a5,0(a4)
2005dd04:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005dd08:	4314                	c.lw	a3,0(a4)
2005dd0a:	c87c                	c.sw	a5,84(s0)
2005dd0c:	4785                	c.li	a5,1
2005dd0e:	00a797b3          	sll	a5,a5,a0
2005dd12:	8fd5                	c.or	a5,a3
2005dd14:	c31c                	c.sw	a5,0(a4)
2005dd16:	47d1                	c.li	a5,20
2005dd18:	02f50533          	mul	a0,a0,a5
2005dd1c:	954e                	c.add	a0,s3
2005dd1e:	958fd0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005dd22:	f13ff0ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005dd26:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dd2a:	cb81                	c.beqz	a5,2005dd3a <prvAddNewTaskToReadyList+0xe2>
2005dd2c:	409c                	c.lw	a5,0(s1)
2005dd2e:	57d8                	c.lw	a4,44(a5)
2005dd30:	545c                	c.lw	a5,44(s0)
2005dd32:	00f77463          	bgeu	a4,a5,2005dd3a <prvAddNewTaskToReadyList+0xe2>
2005dd36:	00000073          	ecall
2005dd3a:	40f2                	c.lwsp	ra,28(sp)
2005dd3c:	4462                	c.lwsp	s0,24(sp)
2005dd3e:	44d2                	c.lwsp	s1,20(sp)
2005dd40:	4942                	c.lwsp	s2,16(sp)
2005dd42:	49b2                	c.lwsp	s3,12(sp)
2005dd44:	4a22                	c.lwsp	s4,8(sp)
2005dd46:	6105                	c.addi16sp	sp,32
2005dd48:	8082                	c.jr	ra
2005dd4a:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dd4e:	f3dd                	c.bnez	a5,2005dcf4 <prvAddNewTaskToReadyList+0x9c>
2005dd50:	409c                	c.lw	a5,0(s1)
2005dd52:	57d8                	c.lw	a4,44(a5)
2005dd54:	545c                	c.lw	a5,44(s0)
2005dd56:	f8e7efe3          	bltu	a5,a4,2005dcf4 <prvAddNewTaskToReadyList+0x9c>
2005dd5a:	c080                	c.sw	s0,0(s1)
2005dd5c:	bf61                	c.j	2005dcf4 <prvAddNewTaskToReadyList+0x9c>

2005dd5e <xTaskCreateStatic>:
2005dd5e:	1101                	c.addi	sp,-32
2005dd60:	cc22                	c.swsp	s0,24(sp)
2005dd62:	ce06                	c.swsp	ra,28(sp)
2005dd64:	8442                	c.mv	s0,a6
2005dd66:	00080f63          	beq	a6,zero,2005dd84 <xTaskCreateStatic+0x26>
2005dd6a:	c395                	c.beqz	a5,2005dd8e <xTaskCreateStatic+0x30>
2005dd6c:	02f82823          	sw	a5,48(a6)
2005dd70:	4789                	c.li	a5,2
2005dd72:	16f800a3          	sb	a5,353(a6)
2005dd76:	007c                	c.addi4spn	a5,sp,12
2005dd78:	85bff0ef          	jal	ra,2005d5d2 <prvInitialiseNewTask.constprop.0>
2005dd7c:	8522                	c.mv	a0,s0
2005dd7e:	edbff0ef          	jal	ra,2005dc58 <prvAddNewTaskToReadyList>
2005dd82:	4432                	c.lwsp	s0,12(sp)
2005dd84:	40f2                	c.lwsp	ra,28(sp)
2005dd86:	8522                	c.mv	a0,s0
2005dd88:	4462                	c.lwsp	s0,24(sp)
2005dd8a:	6105                	c.addi16sp	sp,32
2005dd8c:	8082                	c.jr	ra
2005dd8e:	4401                	c.li	s0,0
2005dd90:	bfd5                	c.j	2005dd84 <xTaskCreateStatic+0x26>

2005dd92 <vTaskStartScheduler>:
2005dd92:	1101                	c.addi	sp,-32
2005dd94:	0070                	c.addi4spn	a2,sp,12
2005dd96:	002c                	c.addi4spn	a1,sp,8
2005dd98:	0048                	c.addi4spn	a0,sp,4
2005dd9a:	ce06                	c.swsp	ra,28(sp)
2005dd9c:	c202                	c.swsp	zero,4(sp)
2005dd9e:	c402                	c.swsp	zero,8(sp)
2005dda0:	8b9fe0ef          	jal	ra,2005c658 <vApplicationGetIdleTaskMemory>
2005dda4:	4812                	c.lwsp	a6,4(sp)
2005dda6:	47a2                	c.lwsp	a5,8(sp)
2005dda8:	4632                	c.lwsp	a2,12(sp)
2005ddaa:	200635b7          	lui	a1,0x20063
2005ddae:	2005e537          	lui	a0,0x2005e
2005ddb2:	4701                	c.li	a4,0
2005ddb4:	4681                	c.li	a3,0
2005ddb6:	6a458593          	addi	a1,a1,1700 # 200636a4 <__func__.1+0x18>
2005ddba:	0b650513          	addi	a0,a0,182 # 2005e0b6 <prvIdleTask>
2005ddbe:	fa1ff0ef          	jal	ra,2005dd5e <xTaskCreateStatic>
2005ddc2:	c51d                	c.beqz	a0,2005ddf0 <vTaskStartScheduler+0x5e>
2005ddc4:	76a000ef          	jal	ra,2005e52e <xTimerCreateTimerTask>
2005ddc8:	4785                	c.li	a5,1
2005ddca:	02f51363          	bne	a0,a5,2005ddf0 <vTaskStartScheduler+0x5e>
2005ddce:	30047073          	csrrci	zero,mstatus,8
2005ddd2:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005ddd6:	06c78793          	addi	a5,a5,108
2005ddda:	e8f1aa23          	sw	a5,-364(gp) # 2006681c <_impure_ptr>
2005ddde:	577d                	c.li	a4,-1
2005dde0:	f8e1a223          	sw	a4,-124(gp) # 2006690c <xNextTaskUnblockTime>
2005dde4:	f8a1a823          	sw	a0,-112(gp) # 20066918 <xSchedulerRunning>
2005dde8:	f801aa23          	sw	zero,-108(gp) # 2006691c <xTickCount>
2005ddec:	fdafe0ef          	jal	ra,2005c5c6 <xPortStartScheduler>
2005ddf0:	40f2                	c.lwsp	ra,28(sp)
2005ddf2:	e8c1a783          	lw	a5,-372(gp) # 20066814 <uxTopUsedPriority>
2005ddf6:	6105                	c.addi16sp	sp,32
2005ddf8:	8082                	c.jr	ra

2005ddfa <xTaskCreate>:
2005ddfa:	7179                	c.addi16sp	sp,-48
2005ddfc:	ce4e                	c.swsp	s3,28(sp)
2005ddfe:	89aa                	c.mv	s3,a0
2005de00:	00261513          	slli	a0,a2,0x2
2005de04:	d04a                	c.swsp	s2,32(sp)
2005de06:	cc52                	c.swsp	s4,24(sp)
2005de08:	ca56                	c.swsp	s5,20(sp)
2005de0a:	c85a                	c.swsp	s6,16(sp)
2005de0c:	c65e                	c.swsp	s7,12(sp)
2005de0e:	d606                	c.swsp	ra,44(sp)
2005de10:	d422                	c.swsp	s0,40(sp)
2005de12:	d226                	c.swsp	s1,36(sp)
2005de14:	8a2e                	c.mv	s4,a1
2005de16:	8932                	c.mv	s2,a2
2005de18:	8ab6                	c.mv	s5,a3
2005de1a:	8b3a                	c.mv	s6,a4
2005de1c:	8bbe                	c.mv	s7,a5
2005de1e:	e05fc0ef          	jal	ra,2005ac22 <pvPortMalloc>
2005de22:	c531                	c.beqz	a0,2005de6e <xTaskCreate+0x74>
2005de24:	84aa                	c.mv	s1,a0
2005de26:	16400513          	addi	a0,zero,356
2005de2a:	df9fc0ef          	jal	ra,2005ac22 <pvPortMalloc>
2005de2e:	842a                	c.mv	s0,a0
2005de30:	cd05                	c.beqz	a0,2005de68 <xTaskCreate+0x6e>
2005de32:	882a                	c.mv	a6,a0
2005de34:	d904                	c.sw	s1,48(a0)
2005de36:	160500a3          	sb	zero,353(a0)
2005de3a:	87de                	c.mv	a5,s7
2005de3c:	875a                	c.mv	a4,s6
2005de3e:	86d6                	c.mv	a3,s5
2005de40:	864a                	c.mv	a2,s2
2005de42:	85d2                	c.mv	a1,s4
2005de44:	854e                	c.mv	a0,s3
2005de46:	f8cff0ef          	jal	ra,2005d5d2 <prvInitialiseNewTask.constprop.0>
2005de4a:	8522                	c.mv	a0,s0
2005de4c:	e0dff0ef          	jal	ra,2005dc58 <prvAddNewTaskToReadyList>
2005de50:	4505                	c.li	a0,1
2005de52:	50b2                	c.lwsp	ra,44(sp)
2005de54:	5422                	c.lwsp	s0,40(sp)
2005de56:	5492                	c.lwsp	s1,36(sp)
2005de58:	5902                	c.lwsp	s2,32(sp)
2005de5a:	49f2                	c.lwsp	s3,28(sp)
2005de5c:	4a62                	c.lwsp	s4,24(sp)
2005de5e:	4ad2                	c.lwsp	s5,20(sp)
2005de60:	4b42                	c.lwsp	s6,16(sp)
2005de62:	4bb2                	c.lwsp	s7,12(sp)
2005de64:	6145                	c.addi16sp	sp,48
2005de66:	8082                	c.jr	ra
2005de68:	8526                	c.mv	a0,s1
2005de6a:	e97fc0ef          	jal	ra,2005ad00 <vPortFree>
2005de6e:	557d                	c.li	a0,-1
2005de70:	b7cd                	c.j	2005de52 <xTaskCreate+0x58>

2005de72 <vTaskDelete>:
2005de72:	1141                	c.addi	sp,-16
2005de74:	c422                	c.swsp	s0,8(sp)
2005de76:	c226                	c.swsp	s1,4(sp)
2005de78:	842a                	c.mv	s0,a0
2005de7a:	c606                	c.swsp	ra,12(sp)
2005de7c:	c04a                	c.swsp	s2,0(sp)
2005de7e:	f5c18493          	addi	s1,gp,-164 # 200668e4 <pxCurrentTCB>
2005de82:	d99ff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005de86:	e011                	c.bnez	s0,2005de8a <vTaskDelete+0x18>
2005de88:	4080                	c.lw	s0,0(s1)
2005de8a:	00440913          	addi	s2,s0,4
2005de8e:	854a                	c.mv	a0,s2
2005de90:	82afd0ef          	jal	ra,2005aeba <uxListRemove>
2005de94:	e515                	c.bnez	a0,2005dec0 <vTaskDelete+0x4e>
2005de96:	5450                	c.lw	a2,44(s0)
2005de98:	4751                	c.li	a4,20
2005de9a:	02e606b3          	mul	a3,a2,a4
2005de9e:	20068737          	lui	a4,0x20068
2005dea2:	2e870713          	addi	a4,a4,744 # 200682e8 <pxReadyTasksLists>
2005dea6:	9736                	c.add	a4,a3
2005dea8:	431c                	c.lw	a5,0(a4)
2005deaa:	eb99                	c.bnez	a5,2005dec0 <vTaskDelete+0x4e>
2005deac:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005deb0:	4314                	c.lw	a3,0(a4)
2005deb2:	4785                	c.li	a5,1
2005deb4:	00c797b3          	sll	a5,a5,a2
2005deb8:	fff7c793          	xori	a5,a5,-1
2005debc:	8ff5                	c.and	a5,a3
2005debe:	c31c                	c.sw	a5,0(a4)
2005dec0:	541c                	c.lw	a5,40(s0)
2005dec2:	c789                	c.beqz	a5,2005decc <vTaskDelete+0x5a>
2005dec4:	01840513          	addi	a0,s0,24
2005dec8:	ff3fc0ef          	jal	ra,2005aeba <uxListRemove>
2005decc:	f7c18793          	addi	a5,gp,-132 # 20066904 <uxTaskNumber>
2005ded0:	4398                	c.lw	a4,0(a5)
2005ded2:	0705                	c.addi	a4,1
2005ded4:	c398                	c.sw	a4,0(a5)
2005ded6:	409c                	c.lw	a5,0(s1)
2005ded8:	02879e63          	bne	a5,s0,2005df14 <vTaskDelete+0xa2>
2005dedc:	20068537          	lui	a0,0x20068
2005dee0:	85ca                	c.mv	a1,s2
2005dee2:	41450513          	addi	a0,a0,1044 # 20068414 <xTasksWaitingTermination>
2005dee6:	f91fc0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005deea:	f7418793          	addi	a5,gp,-140 # 200668fc <uxDeletedTasksWaitingCleanUp>
2005deee:	4398                	c.lw	a4,0(a5)
2005def0:	0705                	c.addi	a4,1
2005def2:	c398                	c.sw	a4,0(a5)
2005def4:	d41ff0ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005def8:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005defc:	c791                	c.beqz	a5,2005df08 <vTaskDelete+0x96>
2005defe:	409c                	c.lw	a5,0(s1)
2005df00:	00879463          	bne	a5,s0,2005df08 <vTaskDelete+0x96>
2005df04:	00000073          	ecall
2005df08:	40b2                	c.lwsp	ra,12(sp)
2005df0a:	4422                	c.lwsp	s0,8(sp)
2005df0c:	4492                	c.lwsp	s1,4(sp)
2005df0e:	4902                	c.lwsp	s2,0(sp)
2005df10:	0141                	c.addi	sp,16
2005df12:	8082                	c.jr	ra
2005df14:	f7018793          	addi	a5,gp,-144 # 200668f8 <uxCurrentNumberOfTasks>
2005df18:	4398                	c.lw	a4,0(a5)
2005df1a:	8522                	c.mv	a0,s0
2005df1c:	177d                	c.addi	a4,-1
2005df1e:	c398                	c.sw	a4,0(a5)
2005df20:	e7cff0ef          	jal	ra,2005d59c <prvDeleteTCB>
2005df24:	e5cff0ef          	jal	ra,2005d580 <prvResetNextTaskUnblockTime>
2005df28:	b7f1                	c.j	2005def4 <vTaskDelete+0x82>

2005df2a <eTaskGetState>:
2005df2a:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005df2e:	06a78863          	beq	a5,a0,2005df9e <eTaskGetState+0x74>
2005df32:	1101                	c.addi	sp,-32
2005df34:	ce06                	c.swsp	ra,28(sp)
2005df36:	cc22                	c.swsp	s0,24(sp)
2005df38:	ca26                	c.swsp	s1,20(sp)
2005df3a:	842a                	c.mv	s0,a0
2005df3c:	c84a                	c.swsp	s2,16(sp)
2005df3e:	c64e                	c.swsp	s3,12(sp)
2005df40:	cdbff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005df44:	f601a983          	lw	s3,-160(gp) # 200668e8 <pxDelayedTaskList>
2005df48:	4844                	c.lw	s1,20(s0)
2005df4a:	f641a903          	lw	s2,-156(gp) # 200668ec <pxOverflowDelayedTaskList>
2005df4e:	ce7ff0ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005df52:	4509                	c.li	a0,2
2005df54:	03348463          	beq	s1,s3,2005df7c <eTaskGetState+0x52>
2005df58:	03248263          	beq	s1,s2,2005df7c <eTaskGetState+0x52>
2005df5c:	200687b7          	lui	a5,0x20068
2005df60:	40078793          	addi	a5,a5,1024 # 20068400 <xSuspendedTaskList>
2005df64:	02f49363          	bne	s1,a5,2005df8a <eTaskGetState+0x60>
2005df68:	541c                	c.lw	a5,40(s0)
2005df6a:	eb89                	c.bnez	a5,2005df7c <eTaskGetState+0x52>
2005df6c:	16044783          	lbu	a5,352(s0)
2005df70:	4705                	c.li	a4,1
2005df72:	0ff7f793          	andi	a5,a5,255
2005df76:	00e78363          	beq	a5,a4,2005df7c <eTaskGetState+0x52>
2005df7a:	450d                	c.li	a0,3
2005df7c:	40f2                	c.lwsp	ra,28(sp)
2005df7e:	4462                	c.lwsp	s0,24(sp)
2005df80:	44d2                	c.lwsp	s1,20(sp)
2005df82:	4942                	c.lwsp	s2,16(sp)
2005df84:	49b2                	c.lwsp	s3,12(sp)
2005df86:	6105                	c.addi16sp	sp,32
2005df88:	8082                	c.jr	ra
2005df8a:	200687b7          	lui	a5,0x20068
2005df8e:	41478793          	addi	a5,a5,1044 # 20068414 <xTasksWaitingTermination>
2005df92:	4511                	c.li	a0,4
2005df94:	fef484e3          	beq	s1,a5,2005df7c <eTaskGetState+0x52>
2005df98:	d0f5                	c.beqz	s1,2005df7c <eTaskGetState+0x52>
2005df9a:	4505                	c.li	a0,1
2005df9c:	b7c5                	c.j	2005df7c <eTaskGetState+0x52>
2005df9e:	4501                	c.li	a0,0
2005dfa0:	8082                	c.jr	ra

2005dfa2 <xTaskResumeAll>:
2005dfa2:	7139                	c.addi16sp	sp,-64
2005dfa4:	de06                	c.swsp	ra,60(sp)
2005dfa6:	dc22                	c.swsp	s0,56(sp)
2005dfa8:	da26                	c.swsp	s1,52(sp)
2005dfaa:	d84a                	c.swsp	s2,48(sp)
2005dfac:	d64e                	c.swsp	s3,44(sp)
2005dfae:	d452                	c.swsp	s4,40(sp)
2005dfb0:	d256                	c.swsp	s5,36(sp)
2005dfb2:	d05a                	c.swsp	s6,32(sp)
2005dfb4:	ce5e                	c.swsp	s7,28(sp)
2005dfb6:	c65ff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005dfba:	f7818793          	addi	a5,gp,-136 # 20066900 <uxSchedulerSuspended>
2005dfbe:	4398                	c.lw	a4,0(a5)
2005dfc0:	177d                	c.addi	a4,-1
2005dfc2:	c398                	c.sw	a4,0(a5)
2005dfc4:	439c                	c.lw	a5,0(a5)
2005dfc6:	c38d                	c.beqz	a5,2005dfe8 <xTaskResumeAll+0x46>
2005dfc8:	4501                	c.li	a0,0
2005dfca:	c62a                	c.swsp	a0,12(sp)
2005dfcc:	c69ff0ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005dfd0:	50f2                	c.lwsp	ra,60(sp)
2005dfd2:	5462                	c.lwsp	s0,56(sp)
2005dfd4:	4532                	c.lwsp	a0,12(sp)
2005dfd6:	54d2                	c.lwsp	s1,52(sp)
2005dfd8:	5942                	c.lwsp	s2,48(sp)
2005dfda:	59b2                	c.lwsp	s3,44(sp)
2005dfdc:	5a22                	c.lwsp	s4,40(sp)
2005dfde:	5a92                	c.lwsp	s5,36(sp)
2005dfe0:	5b02                	c.lwsp	s6,32(sp)
2005dfe2:	4bf2                	c.lwsp	s7,28(sp)
2005dfe4:	6121                	c.addi16sp	sp,64
2005dfe6:	8082                	c.jr	ra
2005dfe8:	f701a783          	lw	a5,-144(gp) # 200668f8 <uxCurrentNumberOfTasks>
2005dfec:	dff1                	c.beqz	a5,2005dfc8 <xTaskResumeAll+0x26>
2005dfee:	200684b7          	lui	s1,0x20068
2005dff2:	20068937          	lui	s2,0x20068
2005dff6:	4401                	c.li	s0,0
2005dff8:	3ec48493          	addi	s1,s1,1004 # 200683ec <xPendingReadyList>
2005dffc:	4985                	c.li	s3,1
2005dffe:	2e890913          	addi	s2,s2,744 # 200682e8 <pxReadyTasksLists>
2005e002:	4ad1                	c.li	s5,20
2005e004:	a0b1                	c.j	2005e050 <xTaskResumeAll+0xae>
2005e006:	44dc                	c.lw	a5,12(s1)
2005e008:	47c0                	c.lw	s0,12(a5)
2005e00a:	01840513          	addi	a0,s0,24
2005e00e:	eadfc0ef          	jal	ra,2005aeba <uxListRemove>
2005e012:	00440593          	addi	a1,s0,4
2005e016:	852e                	c.mv	a0,a1
2005e018:	c62e                	c.swsp	a1,12(sp)
2005e01a:	ea1fc0ef          	jal	ra,2005aeba <uxListRemove>
2005e01e:	5448                	c.lw	a0,44(s0)
2005e020:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005e024:	4314                	c.lw	a3,0(a4)
2005e026:	00a997b3          	sll	a5,s3,a0
2005e02a:	03550533          	mul	a0,a0,s5
2005e02e:	45b2                	c.lwsp	a1,12(sp)
2005e030:	8fd5                	c.or	a5,a3
2005e032:	c31c                	c.sw	a5,0(a4)
2005e034:	954a                	c.add	a0,s2
2005e036:	e41fc0ef          	jal	ra,2005ae76 <vListInsertEnd>
2005e03a:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005e03e:	439c                	c.lw	a5,0(a5)
2005e040:	5458                	c.lw	a4,44(s0)
2005e042:	57dc                	c.lw	a5,44(a5)
2005e044:	00f76663          	bltu	a4,a5,2005e050 <xTaskResumeAll+0xae>
2005e048:	f9818793          	addi	a5,gp,-104 # 20066920 <xYieldPending>
2005e04c:	0137a023          	sw	s3,0(a5)
2005e050:	409c                	c.lw	a5,0(s1)
2005e052:	fbd5                	c.bnez	a5,2005e006 <xTaskResumeAll+0x64>
2005e054:	c019                	c.beqz	s0,2005e05a <xTaskResumeAll+0xb8>
2005e056:	d2aff0ef          	jal	ra,2005d580 <prvResetNextTaskUnblockTime>
2005e05a:	f8c18713          	addi	a4,gp,-116 # 20066914 <xPendedTicks>
2005e05e:	4304                	c.lw	s1,0(a4)
2005e060:	f8c18413          	addi	s0,gp,-116 # 20066914 <xPendedTicks>
2005e064:	cc89                	c.beqz	s1,2005e07e <xTaskResumeAll+0xdc>
2005e066:	4985                	c.li	s3,1
2005e068:	eb0ff0ef          	jal	ra,2005d718 <xTaskIncrementTick>
2005e06c:	c509                	c.beqz	a0,2005e076 <xTaskResumeAll+0xd4>
2005e06e:	f9818793          	addi	a5,gp,-104 # 20066920 <xYieldPending>
2005e072:	0137a023          	sw	s3,0(a5)
2005e076:	14fd                	c.addi	s1,-1
2005e078:	f8e5                	c.bnez	s1,2005e068 <xTaskResumeAll+0xc6>
2005e07a:	00042023          	sw	zero,0(s0)
2005e07e:	f981a783          	lw	a5,-104(gp) # 20066920 <xYieldPending>
2005e082:	d3b9                	c.beqz	a5,2005dfc8 <xTaskResumeAll+0x26>
2005e084:	00000073          	ecall
2005e088:	4505                	c.li	a0,1
2005e08a:	b781                	c.j	2005dfca <xTaskResumeAll+0x28>

2005e08c <vTaskDelay>:
2005e08c:	e501                	c.bnez	a0,2005e094 <vTaskDelay+0x8>
2005e08e:	00000073          	ecall
2005e092:	8082                	c.jr	ra
2005e094:	1101                	c.addi	sp,-32
2005e096:	ce06                	c.swsp	ra,28(sp)
2005e098:	c62a                	c.swsp	a0,12(sp)
2005e09a:	e5aff0ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005e09e:	4532                	c.lwsp	a0,12(sp)
2005e0a0:	4581                	c.li	a1,0
2005e0a2:	bbaff0ef          	jal	ra,2005d45c <prvAddCurrentTaskToDelayedList>
2005e0a6:	efdff0ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005e0aa:	e119                	c.bnez	a0,2005e0b0 <vTaskDelay+0x24>
2005e0ac:	00000073          	ecall
2005e0b0:	40f2                	c.lwsp	ra,28(sp)
2005e0b2:	6105                	c.addi16sp	sp,32
2005e0b4:	8082                	c.jr	ra

2005e0b6 <prvIdleTask>:
2005e0b6:	1101                	c.addi	sp,-32
2005e0b8:	ca26                	c.swsp	s1,20(sp)
2005e0ba:	200684b7          	lui	s1,0x20068
2005e0be:	cc22                	c.swsp	s0,24(sp)
2005e0c0:	c452                	c.swsp	s4,8(sp)
2005e0c2:	c256                	c.swsp	s5,4(sp)
2005e0c4:	c05a                	c.swsp	s6,0(sp)
2005e0c6:	ce06                	c.swsp	ra,28(sp)
2005e0c8:	c84a                	c.swsp	s2,16(sp)
2005e0ca:	c64e                	c.swsp	s3,12(sp)
2005e0cc:	41448493          	addi	s1,s1,1044 # 20068414 <xTasksWaitingTermination>
2005e0d0:	20068ab7          	lui	s5,0x20068
2005e0d4:	4405                	c.li	s0,1
2005e0d6:	f7418913          	addi	s2,gp,-140 # 200668fc <uxDeletedTasksWaitingCleanUp>
2005e0da:	00092783          	lw	a5,0(s2)
2005e0de:	eb8d                	c.bnez	a5,2005e110 <prvIdleTask+0x5a>
2005e0e0:	2e8a8793          	addi	a5,s5,744 # 200682e8 <pxReadyTasksLists>
2005e0e4:	439c                	c.lw	a5,0(a5)
2005e0e6:	00f47463          	bgeu	s0,a5,2005e0ee <prvIdleTask+0x38>
2005e0ea:	00000073          	ecall
2005e0ee:	cf8fe0ef          	jal	ra,2005c5e6 <vApplicationIdleHook>
2005e0f2:	c60ff0ef          	jal	ra,2005d552 <prvGetExpectedIdleTime>
2005e0f6:	fea470e3          	bgeu	s0,a0,2005e0d6 <prvIdleTask+0x20>
2005e0fa:	dfaff0ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005e0fe:	c54ff0ef          	jal	ra,2005d552 <prvGetExpectedIdleTime>
2005e102:	00a47463          	bgeu	s0,a0,2005e10a <prvIdleTask+0x54>
2005e106:	d8afe0ef          	jal	ra,2005c690 <vPortSuppressTicksAndSleep>
2005e10a:	e99ff0ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005e10e:	b7e1                	c.j	2005e0d6 <prvIdleTask+0x20>
2005e110:	b0bff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005e114:	44dc                	c.lw	a5,12(s1)
2005e116:	00c7a983          	lw	s3,12(a5)
2005e11a:	00498513          	addi	a0,s3,4
2005e11e:	d9dfc0ef          	jal	ra,2005aeba <uxListRemove>
2005e122:	f7018713          	addi	a4,gp,-144 # 200668f8 <uxCurrentNumberOfTasks>
2005e126:	431c                	c.lw	a5,0(a4)
2005e128:	17fd                	c.addi	a5,-1
2005e12a:	c31c                	c.sw	a5,0(a4)
2005e12c:	00092783          	lw	a5,0(s2)
2005e130:	17fd                	c.addi	a5,-1
2005e132:	00f92023          	sw	a5,0(s2)
2005e136:	affff0ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005e13a:	854e                	c.mv	a0,s3
2005e13c:	c60ff0ef          	jal	ra,2005d59c <prvDeleteTCB>
2005e140:	bf59                	c.j	2005e0d6 <prvIdleTask+0x20>

2005e142 <vTaskGetInfo>:
2005e142:	1141                	c.addi	sp,-16
2005e144:	c422                	c.swsp	s0,8(sp)
2005e146:	c226                	c.swsp	s1,4(sp)
2005e148:	c04a                	c.swsp	s2,0(sp)
2005e14a:	c606                	c.swsp	ra,12(sp)
2005e14c:	842e                	c.mv	s0,a1
2005e14e:	8932                	c.mv	s2,a2
2005e150:	84aa                	c.mv	s1,a0
2005e152:	e119                	c.bnez	a0,2005e158 <vTaskGetInfo+0x16>
2005e154:	f5c1a483          	lw	s1,-164(gp) # 200668e4 <pxCurrentTCB>
2005e158:	03448793          	addi	a5,s1,52
2005e15c:	c004                	c.sw	s1,0(s0)
2005e15e:	c05c                	c.sw	a5,4(s0)
2005e160:	54dc                	c.lw	a5,44(s1)
2005e162:	c81c                	c.sw	a5,16(s0)
2005e164:	589c                	c.lw	a5,48(s1)
2005e166:	cc5c                	c.sw	a5,28(s0)
2005e168:	48fc                	c.lw	a5,84(s1)
2005e16a:	c41c                	c.sw	a5,8(s0)
2005e16c:	4cfc                	c.lw	a5,92(s1)
2005e16e:	c85c                	c.sw	a5,20(s0)
2005e170:	54bc                	c.lw	a5,104(s1)
2005e172:	cc1c                	c.sw	a5,24(s0)
2005e174:	4795                	c.li	a5,5
2005e176:	04f68c63          	beq	a3,a5,2005e1ce <vTaskGetInfo+0x8c>
2005e17a:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005e17e:	02979963          	bne	a5,s1,2005e1b0 <vTaskGetInfo+0x6e>
2005e182:	00040623          	sb	zero,12(s0)
2005e186:	04090c63          	beq	s2,zero,2005e1de <vTaskGetInfo+0x9c>
2005e18a:	5890                	c.lw	a2,48(s1)
2005e18c:	4781                	c.li	a5,0
2005e18e:	0a500713          	addi	a4,zero,165
2005e192:	00f606b3          	add	a3,a2,a5
2005e196:	0006c683          	lbu	a3,0(a3)
2005e19a:	04e68063          	beq	a3,a4,2005e1da <vTaskGetInfo+0x98>
2005e19e:	8389                	c.srli	a5,0x2
2005e1a0:	02f41023          	sh	a5,32(s0)
2005e1a4:	40b2                	c.lwsp	ra,12(sp)
2005e1a6:	4422                	c.lwsp	s0,8(sp)
2005e1a8:	4492                	c.lwsp	s1,4(sp)
2005e1aa:	4902                	c.lwsp	s2,0(sp)
2005e1ac:	0141                	c.addi	sp,16
2005e1ae:	8082                	c.jr	ra
2005e1b0:	00d40623          	sb	a3,12(s0)
2005e1b4:	478d                	c.li	a5,3
2005e1b6:	fcf698e3          	bne	a3,a5,2005e186 <vTaskGetInfo+0x44>
2005e1ba:	d3aff0ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005e1be:	549c                	c.lw	a5,40(s1)
2005e1c0:	c781                	c.beqz	a5,2005e1c8 <vTaskGetInfo+0x86>
2005e1c2:	4789                	c.li	a5,2
2005e1c4:	00f40623          	sb	a5,12(s0)
2005e1c8:	ddbff0ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005e1cc:	bf6d                	c.j	2005e186 <vTaskGetInfo+0x44>
2005e1ce:	8526                	c.mv	a0,s1
2005e1d0:	d5bff0ef          	jal	ra,2005df2a <eTaskGetState>
2005e1d4:	00a40623          	sb	a0,12(s0)
2005e1d8:	b77d                	c.j	2005e186 <vTaskGetInfo+0x44>
2005e1da:	0785                	c.addi	a5,1
2005e1dc:	bf5d                	c.j	2005e192 <vTaskGetInfo+0x50>
2005e1de:	02041023          	sh	zero,32(s0)
2005e1e2:	b7c9                	c.j	2005e1a4 <vTaskGetInfo+0x62>

2005e1e4 <prvListTasksWithinSingleList>:
2005e1e4:	7179                	c.addi16sp	sp,-48
2005e1e6:	d606                	c.swsp	ra,44(sp)
2005e1e8:	d422                	c.swsp	s0,40(sp)
2005e1ea:	d226                	c.swsp	s1,36(sp)
2005e1ec:	d04a                	c.swsp	s2,32(sp)
2005e1ee:	ce4e                	c.swsp	s3,28(sp)
2005e1f0:	cc52                	c.swsp	s4,24(sp)
2005e1f2:	ca56                	c.swsp	s5,20(sp)
2005e1f4:	c85a                	c.swsp	s6,16(sp)
2005e1f6:	c65e                	c.swsp	s7,12(sp)
2005e1f8:	4184                	c.lw	s1,0(a1)
2005e1fa:	c8a9                	c.beqz	s1,2005e24c <prvListTasksWithinSingleList+0x68>
2005e1fc:	41dc                	c.lw	a5,4(a1)
2005e1fe:	00858a13          	addi	s4,a1,8
2005e202:	892a                	c.mv	s2,a0
2005e204:	43dc                	c.lw	a5,4(a5)
2005e206:	842e                	c.mv	s0,a1
2005e208:	89b2                	c.mv	s3,a2
2005e20a:	c1dc                	c.sw	a5,4(a1)
2005e20c:	01479463          	bne	a5,s4,2005e214 <prvListTasksWithinSingleList+0x30>
2005e210:	45dc                	c.lw	a5,12(a1)
2005e212:	c1dc                	c.sw	a5,4(a1)
2005e214:	405c                	c.lw	a5,4(s0)
2005e216:	4481                	c.li	s1,0
2005e218:	02400b93          	addi	s7,zero,36
2005e21c:	00c7ab03          	lw	s6,12(a5)
2005e220:	405c                	c.lw	a5,4(s0)
2005e222:	43dc                	c.lw	a5,4(a5)
2005e224:	c05c                	c.sw	a5,4(s0)
2005e226:	00fa1563          	bne	s4,a5,2005e230 <prvListTasksWithinSingleList+0x4c>
2005e22a:	004a2783          	lw	a5,4(s4)
2005e22e:	c05c                	c.sw	a5,4(s0)
2005e230:	037485b3          	mul	a1,s1,s7
2005e234:	405c                	c.lw	a5,4(s0)
2005e236:	86ce                	c.mv	a3,s3
2005e238:	4605                	c.li	a2,1
2005e23a:	00c7aa83          	lw	s5,12(a5)
2005e23e:	0485                	c.addi	s1,1
2005e240:	8556                	c.mv	a0,s5
2005e242:	95ca                	c.add	a1,s2
2005e244:	effff0ef          	jal	ra,2005e142 <vTaskGetInfo>
2005e248:	fd5b1ce3          	bne	s6,s5,2005e220 <prvListTasksWithinSingleList+0x3c>
2005e24c:	50b2                	c.lwsp	ra,44(sp)
2005e24e:	5422                	c.lwsp	s0,40(sp)
2005e250:	5902                	c.lwsp	s2,32(sp)
2005e252:	49f2                	c.lwsp	s3,28(sp)
2005e254:	4a62                	c.lwsp	s4,24(sp)
2005e256:	4ad2                	c.lwsp	s5,20(sp)
2005e258:	4b42                	c.lwsp	s6,16(sp)
2005e25a:	4bb2                	c.lwsp	s7,12(sp)
2005e25c:	8526                	c.mv	a0,s1
2005e25e:	5492                	c.lwsp	s1,36(sp)
2005e260:	6145                	c.addi16sp	sp,48
2005e262:	8082                	c.jr	ra

2005e264 <uxTaskGetSystemState>:
2005e264:	1101                	c.addi	sp,-32
2005e266:	cc22                	c.swsp	s0,24(sp)
2005e268:	c84a                	c.swsp	s2,16(sp)
2005e26a:	c64e                	c.swsp	s3,12(sp)
2005e26c:	ce06                	c.swsp	ra,28(sp)
2005e26e:	ca26                	c.swsp	s1,20(sp)
2005e270:	c452                	c.swsp	s4,8(sp)
2005e272:	c256                	c.swsp	s5,4(sp)
2005e274:	c05a                	c.swsp	s6,0(sp)
2005e276:	892a                	c.mv	s2,a0
2005e278:	842e                	c.mv	s0,a1
2005e27a:	89b2                	c.mv	s3,a2
2005e27c:	c78ff0ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005e280:	f701a783          	lw	a5,-144(gp) # 200668f8 <uxCurrentNumberOfTasks>
2005e284:	0af46563          	bltu	s0,a5,2005e32e <uxTaskGetSystemState+0xca>
2005e288:	20068ab7          	lui	s5,0x20068
2005e28c:	4a2d                	c.li	s4,11
2005e28e:	4401                	c.li	s0,0
2005e290:	4b51                	c.li	s6,20
2005e292:	2e8a8a93          	addi	s5,s5,744 # 200682e8 <pxReadyTasksLists>
2005e296:	02400493          	addi	s1,zero,36
2005e29a:	1a7d                	c.addi	s4,-1
2005e29c:	02940533          	mul	a0,s0,s1
2005e2a0:	4605                	c.li	a2,1
2005e2a2:	036a05b3          	mul	a1,s4,s6
2005e2a6:	954a                	c.add	a0,s2
2005e2a8:	95d6                	c.add	a1,s5
2005e2aa:	f3bff0ef          	jal	ra,2005e1e4 <prvListTasksWithinSingleList>
2005e2ae:	942a                	c.add	s0,a0
2005e2b0:	fe0a15e3          	bne	s4,zero,2005e29a <uxTaskGetSystemState+0x36>
2005e2b4:	02940533          	mul	a0,s0,s1
2005e2b8:	f601a583          	lw	a1,-160(gp) # 200668e8 <pxDelayedTaskList>
2005e2bc:	4609                	c.li	a2,2
2005e2be:	954a                	c.add	a0,s2
2005e2c0:	f25ff0ef          	jal	ra,2005e1e4 <prvListTasksWithinSingleList>
2005e2c4:	942a                	c.add	s0,a0
2005e2c6:	02940533          	mul	a0,s0,s1
2005e2ca:	f641a583          	lw	a1,-156(gp) # 200668ec <pxOverflowDelayedTaskList>
2005e2ce:	4609                	c.li	a2,2
2005e2d0:	954a                	c.add	a0,s2
2005e2d2:	f13ff0ef          	jal	ra,2005e1e4 <prvListTasksWithinSingleList>
2005e2d6:	942a                	c.add	s0,a0
2005e2d8:	02940533          	mul	a0,s0,s1
2005e2dc:	200685b7          	lui	a1,0x20068
2005e2e0:	4611                	c.li	a2,4
2005e2e2:	41458593          	addi	a1,a1,1044 # 20068414 <xTasksWaitingTermination>
2005e2e6:	954a                	c.add	a0,s2
2005e2e8:	efdff0ef          	jal	ra,2005e1e4 <prvListTasksWithinSingleList>
2005e2ec:	942a                	c.add	s0,a0
2005e2ee:	029404b3          	mul	s1,s0,s1
2005e2f2:	200685b7          	lui	a1,0x20068
2005e2f6:	460d                	c.li	a2,3
2005e2f8:	40058593          	addi	a1,a1,1024 # 20068400 <xSuspendedTaskList>
2005e2fc:	00990533          	add	a0,s2,s1
2005e300:	ee5ff0ef          	jal	ra,2005e1e4 <prvListTasksWithinSingleList>
2005e304:	942a                	c.add	s0,a0
2005e306:	00098763          	beq	s3,zero,2005e314 <uxTaskGetSystemState+0xb0>
2005e30a:	410187b7          	lui	a5,0x41018
2005e30e:	4bdc                	c.lw	a5,20(a5)
2005e310:	00f9a023          	sw	a5,0(s3)
2005e314:	c8fff0ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005e318:	40f2                	c.lwsp	ra,28(sp)
2005e31a:	8522                	c.mv	a0,s0
2005e31c:	4462                	c.lwsp	s0,24(sp)
2005e31e:	44d2                	c.lwsp	s1,20(sp)
2005e320:	4942                	c.lwsp	s2,16(sp)
2005e322:	49b2                	c.lwsp	s3,12(sp)
2005e324:	4a22                	c.lwsp	s4,8(sp)
2005e326:	4a92                	c.lwsp	s5,4(sp)
2005e328:	4b02                	c.lwsp	s6,0(sp)
2005e32a:	6105                	c.addi16sp	sp,32
2005e32c:	8082                	c.jr	ra
2005e32e:	4401                	c.li	s0,0
2005e330:	b7d5                	c.j	2005e314 <uxTaskGetSystemState+0xb0>

2005e332 <xTaskGetTickCount>:
2005e332:	1101                	c.addi	sp,-32
2005e334:	ce06                	c.swsp	ra,28(sp)
2005e336:	8e5ff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005e33a:	f941a503          	lw	a0,-108(gp) # 2006691c <xTickCount>
2005e33e:	c62a                	c.swsp	a0,12(sp)
2005e340:	8f5ff0ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005e344:	40f2                	c.lwsp	ra,28(sp)
2005e346:	4532                	c.lwsp	a0,12(sp)
2005e348:	6105                	c.addi16sp	sp,32
2005e34a:	8082                	c.jr	ra

2005e34c <xTaskCheckForTimeOut>:
2005e34c:	1101                	c.addi	sp,-32
2005e34e:	cc22                	c.swsp	s0,24(sp)
2005e350:	ce06                	c.swsp	ra,28(sp)
2005e352:	842e                	c.mv	s0,a1
2005e354:	c62a                	c.swsp	a0,12(sp)
2005e356:	8c5ff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005e35a:	f941a683          	lw	a3,-108(gp) # 2006691c <xTickCount>
2005e35e:	401c                	c.lw	a5,0(s0)
2005e360:	577d                	c.li	a4,-1
2005e362:	02e78f63          	beq	a5,a4,2005e3a0 <xTaskCheckForTimeOut+0x54>
2005e366:	4532                	c.lwsp	a0,12(sp)
2005e368:	f881a603          	lw	a2,-120(gp) # 20066910 <xNumOfOverflows>
2005e36c:	410c                	c.lw	a1,0(a0)
2005e36e:	4158                	c.lw	a4,4(a0)
2005e370:	00c58f63          	beq	a1,a2,2005e38e <xTaskCheckForTimeOut+0x42>
2005e374:	00e6ed63          	bltu	a3,a4,2005e38e <xTaskCheckForTimeOut+0x42>
2005e378:	00042023          	sw	zero,0(s0)
2005e37c:	4505                	c.li	a0,1
2005e37e:	c62a                	c.swsp	a0,12(sp)
2005e380:	8b5ff0ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005e384:	40f2                	c.lwsp	ra,28(sp)
2005e386:	4462                	c.lwsp	s0,24(sp)
2005e388:	4532                	c.lwsp	a0,12(sp)
2005e38a:	6105                	c.addi16sp	sp,32
2005e38c:	8082                	c.jr	ra
2005e38e:	40e68633          	sub	a2,a3,a4
2005e392:	fef673e3          	bgeu	a2,a5,2005e378 <xTaskCheckForTimeOut+0x2c>
2005e396:	8f95                	c.sub	a5,a3
2005e398:	97ba                	c.add	a5,a4
2005e39a:	c01c                	c.sw	a5,0(s0)
2005e39c:	e50ff0ef          	jal	ra,2005d9ec <vTaskInternalSetTimeOutState>
2005e3a0:	4501                	c.li	a0,0
2005e3a2:	bff1                	c.j	2005e37e <xTaskCheckForTimeOut+0x32>

2005e3a4 <vTaskList>:
2005e3a4:	7139                	c.addi16sp	sp,-64
2005e3a6:	dc22                	c.swsp	s0,56(sp)
2005e3a8:	de06                	c.swsp	ra,60(sp)
2005e3aa:	da26                	c.swsp	s1,52(sp)
2005e3ac:	d84a                	c.swsp	s2,48(sp)
2005e3ae:	d64e                	c.swsp	s3,44(sp)
2005e3b0:	d452                	c.swsp	s4,40(sp)
2005e3b2:	d256                	c.swsp	s5,36(sp)
2005e3b4:	d05a                	c.swsp	s6,32(sp)
2005e3b6:	ce5e                	c.swsp	s7,28(sp)
2005e3b8:	f7018793          	addi	a5,gp,-144 # 200668f8 <uxCurrentNumberOfTasks>
2005e3bc:	438c                	c.lw	a1,0(a5)
2005e3be:	842a                	c.mv	s0,a0
2005e3c0:	00050023          	sb	zero,0(a0)
2005e3c4:	4388                	c.lw	a0,0(a5)
2005e3c6:	02400793          	addi	a5,zero,36
2005e3ca:	c62e                	c.swsp	a1,12(sp)
2005e3cc:	02f50533          	mul	a0,a0,a5
2005e3d0:	853fc0ef          	jal	ra,2005ac22 <pvPortMalloc>
2005e3d4:	c151                	c.beqz	a0,2005e458 <vTaskList+0xb4>
2005e3d6:	45b2                	c.lwsp	a1,12(sp)
2005e3d8:	4601                	c.li	a2,0
2005e3da:	892a                	c.mv	s2,a0
2005e3dc:	e89ff0ef          	jal	ra,2005e264 <uxTaskGetSystemState>
2005e3e0:	89aa                	c.mv	s3,a0
2005e3e2:	00490493          	addi	s1,s2,4
2005e3e6:	4a01                	c.li	s4,0
2005e3e8:	4b91                	c.li	s7,4
2005e3ea:	e6018a93          	addi	s5,gp,-416 # 200667e8 <CSWTCH.248>
2005e3ee:	20063b37          	lui	s6,0x20063
2005e3f2:	013a1f63          	bne	s4,s3,2005e410 <vTaskList+0x6c>
2005e3f6:	5462                	c.lwsp	s0,56(sp)
2005e3f8:	50f2                	c.lwsp	ra,60(sp)
2005e3fa:	54d2                	c.lwsp	s1,52(sp)
2005e3fc:	59b2                	c.lwsp	s3,44(sp)
2005e3fe:	5a22                	c.lwsp	s4,40(sp)
2005e400:	5a92                	c.lwsp	s5,36(sp)
2005e402:	5b02                	c.lwsp	s6,32(sp)
2005e404:	4bf2                	c.lwsp	s7,28(sp)
2005e406:	854a                	c.mv	a0,s2
2005e408:	5942                	c.lwsp	s2,48(sp)
2005e40a:	6121                	c.addi16sp	sp,64
2005e40c:	8f5fc06f          	jal	zero,2005ad00 <vPortFree>
2005e410:	0084c783          	lbu	a5,8(s1)
2005e414:	4681                	c.li	a3,0
2005e416:	00fbe563          	bltu	s7,a5,2005e420 <vTaskList+0x7c>
2005e41a:	97d6                	c.add	a5,s5
2005e41c:	0007c683          	lbu	a3,0(a5) # 41018000 <__km4_bd_boot_download_addr__+0x11006000>
2005e420:	408c                	c.lw	a1,0(s1)
2005e422:	8522                	c.mv	a0,s0
2005e424:	c636                	c.swsp	a3,12(sp)
2005e426:	8e2ff0ef          	jal	ra,2005d508 <prvWriteNameToBuffer>
2005e42a:	0044a803          	lw	a6,4(s1)
2005e42e:	01c4d783          	lhu	a5,28(s1)
2005e432:	44d8                	c.lw	a4,12(s1)
2005e434:	46b2                	c.lwsp	a3,12(sp)
2005e436:	6acb0613          	addi	a2,s6,1708 # 200636ac <__func__.1+0x20>
2005e43a:	40000593          	addi	a1,zero,1024
2005e43e:	842a                	c.mv	s0,a0
2005e440:	124010ef          	jal	ra,2005f564 <DiagSnPrintf>
2005e444:	8522                	c.mv	a0,s0
2005e446:	dffab097          	auipc	ra,0xdffab
2005e44a:	692080e7          	jalr	ra,1682(ra) # 9ad8 <__wrap_strlen>
2005e44e:	942a                	c.add	s0,a0
2005e450:	0a05                	c.addi	s4,1
2005e452:	02448493          	addi	s1,s1,36
2005e456:	bf71                	c.j	2005e3f2 <vTaskList+0x4e>
2005e458:	50f2                	c.lwsp	ra,60(sp)
2005e45a:	5462                	c.lwsp	s0,56(sp)
2005e45c:	54d2                	c.lwsp	s1,52(sp)
2005e45e:	5942                	c.lwsp	s2,48(sp)
2005e460:	59b2                	c.lwsp	s3,44(sp)
2005e462:	5a22                	c.lwsp	s4,40(sp)
2005e464:	5a92                	c.lwsp	s5,36(sp)
2005e466:	5b02                	c.lwsp	s6,32(sp)
2005e468:	4bf2                	c.lwsp	s7,28(sp)
2005e46a:	6121                	c.addi16sp	sp,64
2005e46c:	8082                	c.jr	ra

2005e46e <pvTaskIncrementMutexHeldCount>:
2005e46e:	f5c18713          	addi	a4,gp,-164 # 200668e4 <pxCurrentTCB>
2005e472:	4318                	c.lw	a4,0(a4)
2005e474:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005e478:	c709                	c.beqz	a4,2005e482 <pvTaskIncrementMutexHeldCount+0x14>
2005e47a:	4394                	c.lw	a3,0(a5)
2005e47c:	52b8                	c.lw	a4,96(a3)
2005e47e:	0705                	c.addi	a4,1
2005e480:	d2b8                	c.sw	a4,96(a3)
2005e482:	4388                	c.lw	a0,0(a5)
2005e484:	8082                	c.jr	ra

2005e486 <prvCheckForValidListAndQueue>:
2005e486:	1141                	c.addi	sp,-16
2005e488:	c422                	c.swsp	s0,8(sp)
2005e48a:	c606                	c.swsp	ra,12(sp)
2005e48c:	c226                	c.swsp	s1,4(sp)
2005e48e:	c04a                	c.swsp	s2,0(sp)
2005e490:	fa818413          	addi	s0,gp,-88 # 20066930 <xTimerQueue>
2005e494:	f86ff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005e498:	401c                	c.lw	a5,0(s0)
2005e49a:	e3b9                	c.bnez	a5,2005e4e0 <prvCheckForValidListAndQueue+0x5a>
2005e49c:	20068537          	lui	a0,0x20068
2005e4a0:	4a050913          	addi	s2,a0,1184 # 200684a0 <xActiveTimerList1>
2005e4a4:	4a050513          	addi	a0,a0,1184
2005e4a8:	9b5fc0ef          	jal	ra,2005ae5c <vListInitialise>
2005e4ac:	20068537          	lui	a0,0x20068
2005e4b0:	4b450493          	addi	s1,a0,1204 # 200684b4 <xActiveTimerList2>
2005e4b4:	4b450513          	addi	a0,a0,1204
2005e4b8:	9a5fc0ef          	jal	ra,2005ae5c <vListInitialise>
2005e4bc:	200686b7          	lui	a3,0x20068
2005e4c0:	20068637          	lui	a2,0x20068
2005e4c4:	f921ae23          	sw	s2,-100(gp) # 20066924 <pxCurrentTimerList>
2005e4c8:	4701                	c.li	a4,0
2005e4ca:	4c868693          	addi	a3,a3,1224 # 200684c8 <xStaticTimerQueue.0>
2005e4ce:	42860613          	addi	a2,a2,1064 # 20068428 <ucStaticTimerQueueStorage.1>
2005e4d2:	45b1                	c.li	a1,12
2005e4d4:	4529                	c.li	a0,10
2005e4d6:	fa91a023          	sw	s1,-96(gp) # 20066928 <pxOverflowTimerList>
2005e4da:	981fe0ef          	jal	ra,2005ce5a <xQueueGenericCreateStatic>
2005e4de:	c008                	c.sw	a0,0(s0)
2005e4e0:	4422                	c.lwsp	s0,8(sp)
2005e4e2:	40b2                	c.lwsp	ra,12(sp)
2005e4e4:	4492                	c.lwsp	s1,4(sp)
2005e4e6:	4902                	c.lwsp	s2,0(sp)
2005e4e8:	0141                	c.addi	sp,16
2005e4ea:	f4aff06f          	jal	zero,2005dc34 <vTaskExitCritical>

2005e4ee <prvInsertTimerInActiveList>:
2005e4ee:	1141                	c.addi	sp,-16
2005e4f0:	c606                	c.swsp	ra,12(sp)
2005e4f2:	c14c                	c.sw	a1,4(a0)
2005e4f4:	c908                	c.sw	a0,16(a0)
2005e4f6:	87aa                	c.mv	a5,a0
2005e4f8:	02b66163          	bltu	a2,a1,2005e51a <prvInsertTimerInActiveList+0x2c>
2005e4fc:	4d18                	c.lw	a4,24(a0)
2005e4fe:	8e15                	c.sub	a2,a3
2005e500:	4505                	c.li	a0,1
2005e502:	00e67963          	bgeu	a2,a4,2005e514 <prvInsertTimerInActiveList+0x26>
2005e506:	00478593          	addi	a1,a5,4
2005e50a:	fa01a503          	lw	a0,-96(gp) # 20066928 <pxOverflowTimerList>
2005e50e:	97ffc0ef          	jal	ra,2005ae8c <vListInsert>
2005e512:	4501                	c.li	a0,0
2005e514:	40b2                	c.lwsp	ra,12(sp)
2005e516:	0141                	c.addi	sp,16
2005e518:	8082                	c.jr	ra
2005e51a:	00d67563          	bgeu	a2,a3,2005e524 <prvInsertTimerInActiveList+0x36>
2005e51e:	4505                	c.li	a0,1
2005e520:	fed5fae3          	bgeu	a1,a3,2005e514 <prvInsertTimerInActiveList+0x26>
2005e524:	00478593          	addi	a1,a5,4
2005e528:	f9c1a503          	lw	a0,-100(gp) # 20066924 <pxCurrentTimerList>
2005e52c:	b7cd                	c.j	2005e50e <prvInsertTimerInActiveList+0x20>

2005e52e <xTimerCreateTimerTask>:
2005e52e:	1101                	c.addi	sp,-32
2005e530:	ce06                	c.swsp	ra,28(sp)
2005e532:	f55ff0ef          	jal	ra,2005e486 <prvCheckForValidListAndQueue>
2005e536:	fa81a783          	lw	a5,-88(gp) # 20066930 <xTimerQueue>
2005e53a:	4501                	c.li	a0,0
2005e53c:	cb9d                	c.beqz	a5,2005e572 <xTimerCreateTimerTask+0x44>
2005e53e:	0070                	c.addi4spn	a2,sp,12
2005e540:	002c                	c.addi4spn	a1,sp,8
2005e542:	0048                	c.addi4spn	a0,sp,4
2005e544:	c202                	c.swsp	zero,4(sp)
2005e546:	c402                	c.swsp	zero,8(sp)
2005e548:	92cfe0ef          	jal	ra,2005c674 <vApplicationGetTimerTaskMemory>
2005e54c:	47a2                	c.lwsp	a5,8(sp)
2005e54e:	4812                	c.lwsp	a6,4(sp)
2005e550:	4632                	c.lwsp	a2,12(sp)
2005e552:	200635b7          	lui	a1,0x20063
2005e556:	2005e537          	lui	a0,0x2005e
2005e55a:	4729                	c.li	a4,10
2005e55c:	4681                	c.li	a3,0
2005e55e:	6bc58593          	addi	a1,a1,1724 # 200636bc <__func__.1+0x30>
2005e562:	7b450513          	addi	a0,a0,1972 # 2005e7b4 <prvTimerTask>
2005e566:	ff8ff0ef          	jal	ra,2005dd5e <xTaskCreateStatic>
2005e56a:	faa1a623          	sw	a0,-84(gp) # 20066934 <xTimerTaskHandle>
2005e56e:	00a03533          	sltu	a0,zero,a0
2005e572:	40f2                	c.lwsp	ra,28(sp)
2005e574:	6105                	c.addi16sp	sp,32
2005e576:	8082                	c.jr	ra

2005e578 <xTimerCreate>:
2005e578:	1101                	c.addi	sp,-32
2005e57a:	c256                	c.swsp	s5,4(sp)
2005e57c:	8aaa                	c.mv	s5,a0
2005e57e:	02c00513          	addi	a0,zero,44
2005e582:	cc22                	c.swsp	s0,24(sp)
2005e584:	ca26                	c.swsp	s1,20(sp)
2005e586:	c84a                	c.swsp	s2,16(sp)
2005e588:	c64e                	c.swsp	s3,12(sp)
2005e58a:	c452                	c.swsp	s4,8(sp)
2005e58c:	ce06                	c.swsp	ra,28(sp)
2005e58e:	8a2e                	c.mv	s4,a1
2005e590:	84b2                	c.mv	s1,a2
2005e592:	89b6                	c.mv	s3,a3
2005e594:	893a                	c.mv	s2,a4
2005e596:	e8cfc0ef          	jal	ra,2005ac22 <pvPortMalloc>
2005e59a:	842a                	c.mv	s0,a0
2005e59c:	c905                	c.beqz	a0,2005e5cc <xTimerCreate+0x54>
2005e59e:	02050423          	sb	zero,40(a0)
2005e5a2:	ee5ff0ef          	jal	ra,2005e486 <prvCheckForValidListAndQueue>
2005e5a6:	00440513          	addi	a0,s0,4
2005e5aa:	01542023          	sw	s5,0(s0)
2005e5ae:	01442c23          	sw	s4,24(s0)
2005e5b2:	01342e23          	sw	s3,28(s0)
2005e5b6:	03242023          	sw	s2,32(s0)
2005e5ba:	8b7fc0ef          	jal	ra,2005ae70 <vListInitialiseItem>
2005e5be:	c499                	c.beqz	s1,2005e5cc <xTimerCreate+0x54>
2005e5c0:	02844783          	lbu	a5,40(s0)
2005e5c4:	0047e793          	ori	a5,a5,4
2005e5c8:	02f40423          	sb	a5,40(s0)
2005e5cc:	40f2                	c.lwsp	ra,28(sp)
2005e5ce:	8522                	c.mv	a0,s0
2005e5d0:	4462                	c.lwsp	s0,24(sp)
2005e5d2:	44d2                	c.lwsp	s1,20(sp)
2005e5d4:	4942                	c.lwsp	s2,16(sp)
2005e5d6:	49b2                	c.lwsp	s3,12(sp)
2005e5d8:	4a22                	c.lwsp	s4,8(sp)
2005e5da:	4a92                	c.lwsp	s5,4(sp)
2005e5dc:	6105                	c.addi16sp	sp,32
2005e5de:	8082                	c.jr	ra

2005e5e0 <xTimerCreateStatic>:
2005e5e0:	1101                	c.addi	sp,-32
2005e5e2:	cc22                	c.swsp	s0,24(sp)
2005e5e4:	ce06                	c.swsp	ra,28(sp)
2005e5e6:	ca26                	c.swsp	s1,20(sp)
2005e5e8:	c84a                	c.swsp	s2,16(sp)
2005e5ea:	c64e                	c.swsp	s3,12(sp)
2005e5ec:	c452                	c.swsp	s4,8(sp)
2005e5ee:	c256                	c.swsp	s5,4(sp)
2005e5f0:	843e                	c.mv	s0,a5
2005e5f2:	cf95                	c.beqz	a5,2005e62e <xTimerCreateStatic+0x4e>
2005e5f4:	4789                	c.li	a5,2
2005e5f6:	02f40423          	sb	a5,40(s0)
2005e5fa:	8aaa                	c.mv	s5,a0
2005e5fc:	8a2e                	c.mv	s4,a1
2005e5fe:	84b2                	c.mv	s1,a2
2005e600:	89b6                	c.mv	s3,a3
2005e602:	893a                	c.mv	s2,a4
2005e604:	e83ff0ef          	jal	ra,2005e486 <prvCheckForValidListAndQueue>
2005e608:	00440513          	addi	a0,s0,4
2005e60c:	01542023          	sw	s5,0(s0)
2005e610:	01442c23          	sw	s4,24(s0)
2005e614:	01342e23          	sw	s3,28(s0)
2005e618:	03242023          	sw	s2,32(s0)
2005e61c:	855fc0ef          	jal	ra,2005ae70 <vListInitialiseItem>
2005e620:	c499                	c.beqz	s1,2005e62e <xTimerCreateStatic+0x4e>
2005e622:	02844783          	lbu	a5,40(s0)
2005e626:	0047e793          	ori	a5,a5,4
2005e62a:	02f40423          	sb	a5,40(s0)
2005e62e:	40f2                	c.lwsp	ra,28(sp)
2005e630:	8522                	c.mv	a0,s0
2005e632:	4462                	c.lwsp	s0,24(sp)
2005e634:	44d2                	c.lwsp	s1,20(sp)
2005e636:	4942                	c.lwsp	s2,16(sp)
2005e638:	49b2                	c.lwsp	s3,12(sp)
2005e63a:	4a22                	c.lwsp	s4,8(sp)
2005e63c:	4a92                	c.lwsp	s5,4(sp)
2005e63e:	6105                	c.addi16sp	sp,32
2005e640:	8082                	c.jr	ra

2005e642 <xTimerGenericCommand>:
2005e642:	7139                	c.addi16sp	sp,-64
2005e644:	dc22                	c.swsp	s0,56(sp)
2005e646:	da26                	c.swsp	s1,52(sp)
2005e648:	d84a                	c.swsp	s2,48(sp)
2005e64a:	d452                	c.swsp	s4,40(sp)
2005e64c:	d256                	c.swsp	s5,36(sp)
2005e64e:	de06                	c.swsp	ra,60(sp)
2005e650:	d64e                	c.swsp	s3,44(sp)
2005e652:	842a                	c.mv	s0,a0
2005e654:	84ae                	c.mv	s1,a1
2005e656:	8932                	c.mv	s2,a2
2005e658:	8a36                	c.mv	s4,a3
2005e65a:	8aba                	c.mv	s5,a4
2005e65c:	bd6ff0ef          	jal	ra,2005da32 <xTaskGetCurrentTaskHandle>
2005e660:	fac1a783          	lw	a5,-84(gp) # 20066934 <xTimerTaskHandle>
2005e664:	06a79163          	bne	a5,a0,2005e6c6 <xTimerGenericCommand+0x84>
2005e668:	ffd48793          	addi	a5,s1,-3
2005e66c:	4705                	c.li	a4,1
2005e66e:	04f76c63          	bltu	a4,a5,2005e6c6 <xTimerGenericCommand+0x84>
2005e672:	cc1ff0ef          	jal	ra,2005e332 <xTaskGetTickCount>
2005e676:	485c                	c.lw	a5,20(s0)
2005e678:	86aa                	c.mv	a3,a0
2005e67a:	c799                	c.beqz	a5,2005e688 <xTimerGenericCommand+0x46>
2005e67c:	c62a                	c.swsp	a0,12(sp)
2005e67e:	00440513          	addi	a0,s0,4
2005e682:	839fc0ef          	jal	ra,2005aeba <uxListRemove>
2005e686:	46b2                	c.lwsp	a3,12(sp)
2005e688:	4711                	c.li	a4,4
2005e68a:	02844783          	lbu	a5,40(s0)
2005e68e:	00e48f63          	beq	s1,a4,2005e6ac <xTimerGenericCommand+0x6a>
2005e692:	9bf9                	c.andi	a5,-2
2005e694:	02f40423          	sb	a5,40(s0)
2005e698:	4505                	c.li	a0,1
2005e69a:	50f2                	c.lwsp	ra,60(sp)
2005e69c:	5462                	c.lwsp	s0,56(sp)
2005e69e:	54d2                	c.lwsp	s1,52(sp)
2005e6a0:	5942                	c.lwsp	s2,48(sp)
2005e6a2:	59b2                	c.lwsp	s3,44(sp)
2005e6a4:	5a22                	c.lwsp	s4,40(sp)
2005e6a6:	5a92                	c.lwsp	s5,36(sp)
2005e6a8:	6121                	c.addi16sp	sp,64
2005e6aa:	8082                	c.jr	ra
2005e6ac:	0017e793          	ori	a5,a5,1
2005e6b0:	02f40423          	sb	a5,40(s0)
2005e6b4:	01242c23          	sw	s2,24(s0)
2005e6b8:	8636                	c.mv	a2,a3
2005e6ba:	00d905b3          	add	a1,s2,a3
2005e6be:	8522                	c.mv	a0,s0
2005e6c0:	e2fff0ef          	jal	ra,2005e4ee <prvInsertTimerInActiveList>
2005e6c4:	bfd1                	c.j	2005e698 <xTimerGenericCommand+0x56>
2005e6c6:	fa818993          	addi	s3,gp,-88 # 20066930 <xTimerQueue>
2005e6ca:	0009a783          	lw	a5,0(s3)
2005e6ce:	4501                	c.li	a0,0
2005e6d0:	d7e9                	c.beqz	a5,2005e69a <xTimerGenericCommand+0x58>
2005e6d2:	ca26                	c.swsp	s1,20(sp)
2005e6d4:	cc4a                	c.swsp	s2,24(sp)
2005e6d6:	ce22                	c.swsp	s0,28(sp)
2005e6d8:	4715                	c.li	a4,5
2005e6da:	02974163          	blt	a4,s1,2005e6fc <xTimerGenericCommand+0xba>
2005e6de:	b5aff0ef          	jal	ra,2005da38 <xTaskGetSchedulerState>
2005e6e2:	872a                	c.mv	a4,a0
2005e6e4:	4789                	c.li	a5,2
2005e6e6:	0009a503          	lw	a0,0(s3)
2005e6ea:	4681                	c.li	a3,0
2005e6ec:	8656                	c.mv	a2,s5
2005e6ee:	00f70363          	beq	a4,a5,2005e6f4 <xTimerGenericCommand+0xb2>
2005e6f2:	4601                	c.li	a2,0
2005e6f4:	084c                	c.addi4spn	a1,sp,20
2005e6f6:	9e2fe0ef          	jal	ra,2005c8d8 <xQueueGenericSend>
2005e6fa:	b745                	c.j	2005e69a <xTimerGenericCommand+0x58>
2005e6fc:	4681                	c.li	a3,0
2005e6fe:	8652                	c.mv	a2,s4
2005e700:	084c                	c.addi4spn	a1,sp,20
2005e702:	853e                	c.mv	a0,a5
2005e704:	ad8fe0ef          	jal	ra,2005c9dc <xQueueGenericSendFromISR>
2005e708:	bf49                	c.j	2005e69a <xTimerGenericCommand+0x58>

2005e70a <prvSampleTimeNow>:
2005e70a:	7179                	c.addi16sp	sp,-48
2005e70c:	d226                	c.swsp	s1,36(sp)
2005e70e:	d04a                	c.swsp	s2,32(sp)
2005e710:	ce4e                	c.swsp	s3,28(sp)
2005e712:	d606                	c.swsp	ra,44(sp)
2005e714:	d422                	c.swsp	s0,40(sp)
2005e716:	cc52                	c.swsp	s4,24(sp)
2005e718:	ca56                	c.swsp	s5,20(sp)
2005e71a:	89aa                	c.mv	s3,a0
2005e71c:	c17ff0ef          	jal	ra,2005e332 <xTaskGetTickCount>
2005e720:	fa41a783          	lw	a5,-92(gp) # 2006692c <xLastTime.2>
2005e724:	84aa                	c.mv	s1,a0
2005e726:	fa418913          	addi	s2,gp,-92 # 2006692c <xLastTime.2>
2005e72a:	08f57263          	bgeu	a0,a5,2005e7ae <prvSampleTimeNow+0xa4>
2005e72e:	f9c18a13          	addi	s4,gp,-100 # 20066924 <pxCurrentTimerList>
2005e732:	000a2703          	lw	a4,0(s4)
2005e736:	431c                	c.lw	a5,0(a4)
2005e738:	e795                	c.bnez	a5,2005e764 <prvSampleTimeNow+0x5a>
2005e73a:	fa018793          	addi	a5,gp,-96 # 20066928 <pxOverflowTimerList>
2005e73e:	4394                	c.lw	a3,0(a5)
2005e740:	c398                	c.sw	a4,0(a5)
2005e742:	4785                	c.li	a5,1
2005e744:	00da2023          	sw	a3,0(s4)
2005e748:	00f9a023          	sw	a5,0(s3)
2005e74c:	50b2                	c.lwsp	ra,44(sp)
2005e74e:	5422                	c.lwsp	s0,40(sp)
2005e750:	00992023          	sw	s1,0(s2)
2005e754:	49f2                	c.lwsp	s3,28(sp)
2005e756:	5902                	c.lwsp	s2,32(sp)
2005e758:	4a62                	c.lwsp	s4,24(sp)
2005e75a:	4ad2                	c.lwsp	s5,20(sp)
2005e75c:	8526                	c.mv	a0,s1
2005e75e:	5492                	c.lwsp	s1,36(sp)
2005e760:	6145                	c.addi16sp	sp,48
2005e762:	8082                	c.jr	ra
2005e764:	475c                	c.lw	a5,12(a4)
2005e766:	47c0                	c.lw	s0,12(a5)
2005e768:	4390                	c.lw	a2,0(a5)
2005e76a:	00440593          	addi	a1,s0,4
2005e76e:	852e                	c.mv	a0,a1
2005e770:	c632                	c.swsp	a2,12(sp)
2005e772:	c42e                	c.swsp	a1,8(sp)
2005e774:	f46fc0ef          	jal	ra,2005aeba <uxListRemove>
2005e778:	501c                	c.lw	a5,32(s0)
2005e77a:	8522                	c.mv	a0,s0
2005e77c:	9782                	c.jalr	a5
2005e77e:	02844783          	lbu	a5,40(s0)
2005e782:	45a2                	c.lwsp	a1,8(sp)
2005e784:	4632                	c.lwsp	a2,12(sp)
2005e786:	8b91                	c.andi	a5,4
2005e788:	d3dd                	c.beqz	a5,2005e72e <prvSampleTimeNow+0x24>
2005e78a:	4c1c                	c.lw	a5,24(s0)
2005e78c:	97b2                	c.add	a5,a2
2005e78e:	00f67963          	bgeu	a2,a5,2005e7a0 <prvSampleTimeNow+0x96>
2005e792:	000a2503          	lw	a0,0(s4)
2005e796:	c05c                	c.sw	a5,4(s0)
2005e798:	c800                	c.sw	s0,16(s0)
2005e79a:	ef2fc0ef          	jal	ra,2005ae8c <vListInsert>
2005e79e:	bf41                	c.j	2005e72e <prvSampleTimeNow+0x24>
2005e7a0:	4701                	c.li	a4,0
2005e7a2:	4681                	c.li	a3,0
2005e7a4:	4581                	c.li	a1,0
2005e7a6:	8522                	c.mv	a0,s0
2005e7a8:	e9bff0ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005e7ac:	b749                	c.j	2005e72e <prvSampleTimeNow+0x24>
2005e7ae:	0009a023          	sw	zero,0(s3)
2005e7b2:	bf69                	c.j	2005e74c <prvSampleTimeNow+0x42>

2005e7b4 <prvTimerTask>:
2005e7b4:	7139                	c.addi16sp	sp,-64
2005e7b6:	d84a                	c.swsp	s2,48(sp)
2005e7b8:	20063937          	lui	s2,0x20063
2005e7bc:	d64e                	c.swsp	s3,44(sp)
2005e7be:	d256                	c.swsp	s5,36(sp)
2005e7c0:	d05a                	c.swsp	s6,32(sp)
2005e7c2:	de06                	c.swsp	ra,60(sp)
2005e7c4:	dc22                	c.swsp	s0,56(sp)
2005e7c6:	da26                	c.swsp	s1,52(sp)
2005e7c8:	d452                	c.swsp	s4,40(sp)
2005e7ca:	ce5e                	c.swsp	s7,28(sp)
2005e7cc:	cc62                	c.swsp	s8,24(sp)
2005e7ce:	6c490913          	addi	s2,s2,1732 # 200636c4 <__func__.1+0x38>
2005e7d2:	f9c1a783          	lw	a5,-100(gp) # 20066924 <pxCurrentTimerList>
2005e7d6:	f9c18c13          	addi	s8,gp,-100 # 20066924 <pxCurrentTimerList>
2005e7da:	4405                	c.li	s0,1
2005e7dc:	4384                	c.lw	s1,0(a5)
2005e7de:	c481                	c.beqz	s1,2005e7e6 <prvTimerTask+0x32>
2005e7e0:	47dc                	c.lw	a5,12(a5)
2005e7e2:	4401                	c.li	s0,0
2005e7e4:	4384                	c.lw	s1,0(a5)
2005e7e6:	f0ffe0ef          	jal	ra,2005d6f4 <vTaskSuspendAll>
2005e7ea:	0048                	c.addi4spn	a0,sp,4
2005e7ec:	f1fff0ef          	jal	ra,2005e70a <prvSampleTimeNow>
2005e7f0:	4792                	c.lwsp	a5,4(sp)
2005e7f2:	8baa                	c.mv	s7,a0
2005e7f4:	fa818a13          	addi	s4,gp,-88 # 20066930 <xTimerQueue>
2005e7f8:	ebc5                	c.bnez	a5,2005e8a8 <prvTimerTask+0xf4>
2005e7fa:	e449                	c.bnez	s0,2005e884 <prvTimerTask+0xd0>
2005e7fc:	08956963          	bltu	a0,s1,2005e88e <prvTimerTask+0xda>
2005e800:	fa2ff0ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005e804:	000c2783          	lw	a5,0(s8)
2005e808:	47dc                	c.lw	a5,12(a5)
2005e80a:	47c0                	c.lw	s0,12(a5)
2005e80c:	00440513          	addi	a0,s0,4
2005e810:	eaafc0ef          	jal	ra,2005aeba <uxListRemove>
2005e814:	02844783          	lbu	a5,40(s0)
2005e818:	0047f713          	andi	a4,a5,4
2005e81c:	c325                	c.beqz	a4,2005e87c <prvTimerTask+0xc8>
2005e81e:	4c0c                	c.lw	a1,24(s0)
2005e820:	86a6                	c.mv	a3,s1
2005e822:	865e                	c.mv	a2,s7
2005e824:	95a6                	c.add	a1,s1
2005e826:	8522                	c.mv	a0,s0
2005e828:	cc7ff0ef          	jal	ra,2005e4ee <prvInsertTimerInActiveList>
2005e82c:	c901                	c.beqz	a0,2005e83c <prvTimerTask+0x88>
2005e82e:	4701                	c.li	a4,0
2005e830:	4681                	c.li	a3,0
2005e832:	8626                	c.mv	a2,s1
2005e834:	4581                	c.li	a1,0
2005e836:	8522                	c.mv	a0,s0
2005e838:	e0bff0ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005e83c:	501c                	c.lw	a5,32(s0)
2005e83e:	8522                	c.mv	a0,s0
2005e840:	9782                	c.jalr	a5
2005e842:	44a5                	c.li	s1,9
2005e844:	000a2503          	lw	a0,0(s4)
2005e848:	4601                	c.li	a2,0
2005e84a:	004c                	c.addi4spn	a1,sp,4
2005e84c:	a4efe0ef          	jal	ra,2005ca9a <xQueueReceive>
2005e850:	d149                	c.beqz	a0,2005e7d2 <prvTimerTask+0x1e>
2005e852:	4792                	c.lwsp	a5,4(sp)
2005e854:	fe07c8e3          	blt	a5,zero,2005e844 <prvTimerTask+0x90>
2005e858:	4432                	c.lwsp	s0,12(sp)
2005e85a:	485c                	c.lw	a5,20(s0)
2005e85c:	c789                	c.beqz	a5,2005e866 <prvTimerTask+0xb2>
2005e85e:	00440513          	addi	a0,s0,4
2005e862:	e58fc0ef          	jal	ra,2005aeba <uxListRemove>
2005e866:	850a                	c.mv	a0,sp
2005e868:	ea3ff0ef          	jal	ra,2005e70a <prvSampleTimeNow>
2005e86c:	4792                	c.lwsp	a5,4(sp)
2005e86e:	862a                	c.mv	a2,a0
2005e870:	fcf4eae3          	bltu	s1,a5,2005e844 <prvTimerTask+0x90>
2005e874:	078a                	c.slli	a5,0x2
2005e876:	97ca                	c.add	a5,s2
2005e878:	439c                	c.lw	a5,0(a5)
2005e87a:	8782                	c.jr	a5
2005e87c:	9bf9                	c.andi	a5,-2
2005e87e:	02f40423          	sb	a5,40(s0)
2005e882:	bf6d                	c.j	2005e83c <prvTimerTask+0x88>
2005e884:	fa01a783          	lw	a5,-96(gp) # 20066928 <pxOverflowTimerList>
2005e888:	4380                	c.lw	s0,0(a5)
2005e88a:	00143413          	sltiu	s0,s0,1
2005e88e:	000a2503          	lw	a0,0(s4)
2005e892:	8622                	c.mv	a2,s0
2005e894:	417485b3          	sub	a1,s1,s7
2005e898:	c7afe0ef          	jal	ra,2005cd12 <vQueueWaitForMessageRestricted>
2005e89c:	f06ff0ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005e8a0:	f14d                	c.bnez	a0,2005e842 <prvTimerTask+0x8e>
2005e8a2:	00000073          	ecall
2005e8a6:	bf71                	c.j	2005e842 <prvTimerTask+0x8e>
2005e8a8:	efaff0ef          	jal	ra,2005dfa2 <xTaskResumeAll>
2005e8ac:	bf59                	c.j	2005e842 <prvTimerTask+0x8e>
2005e8ae:	02844783          	lbu	a5,40(s0)
2005e8b2:	4c0c                	c.lw	a1,24(s0)
2005e8b4:	8522                	c.mv	a0,s0
2005e8b6:	0017e793          	ori	a5,a5,1
2005e8ba:	02f40423          	sb	a5,40(s0)
2005e8be:	46a2                	c.lwsp	a3,8(sp)
2005e8c0:	95b6                	c.add	a1,a3
2005e8c2:	c2dff0ef          	jal	ra,2005e4ee <prvInsertTimerInActiveList>
2005e8c6:	dd3d                	c.beqz	a0,2005e844 <prvTimerTask+0x90>
2005e8c8:	501c                	c.lw	a5,32(s0)
2005e8ca:	8522                	c.mv	a0,s0
2005e8cc:	9782                	c.jalr	a5
2005e8ce:	02844783          	lbu	a5,40(s0)
2005e8d2:	8b91                	c.andi	a5,4
2005e8d4:	dba5                	c.beqz	a5,2005e844 <prvTimerTask+0x90>
2005e8d6:	4c1c                	c.lw	a5,24(s0)
2005e8d8:	4622                	c.lwsp	a2,8(sp)
2005e8da:	4701                	c.li	a4,0
2005e8dc:	4681                	c.li	a3,0
2005e8de:	963e                	c.add	a2,a5
2005e8e0:	4581                	c.li	a1,0
2005e8e2:	8522                	c.mv	a0,s0
2005e8e4:	d5fff0ef          	jal	ra,2005e642 <xTimerGenericCommand>
2005e8e8:	bfb1                	c.j	2005e844 <prvTimerTask+0x90>
2005e8ea:	02844783          	lbu	a5,40(s0)
2005e8ee:	9bf9                	c.andi	a5,-2
2005e8f0:	02f40423          	sb	a5,40(s0)
2005e8f4:	bf81                	c.j	2005e844 <prvTimerTask+0x90>
2005e8f6:	02844783          	lbu	a5,40(s0)
2005e8fa:	86aa                	c.mv	a3,a0
2005e8fc:	0017e793          	ori	a5,a5,1
2005e900:	02f40423          	sb	a5,40(s0)
2005e904:	45a2                	c.lwsp	a1,8(sp)
2005e906:	cc0c                	c.sw	a1,24(s0)
2005e908:	95aa                	c.add	a1,a0
2005e90a:	8522                	c.mv	a0,s0
2005e90c:	be3ff0ef          	jal	ra,2005e4ee <prvInsertTimerInActiveList>
2005e910:	bf15                	c.j	2005e844 <prvTimerTask+0x90>
2005e912:	02844783          	lbu	a5,40(s0)
2005e916:	0027f713          	andi	a4,a5,2
2005e91a:	fb71                	c.bnez	a4,2005e8ee <prvTimerTask+0x13a>
2005e91c:	8522                	c.mv	a0,s0
2005e91e:	be2fc0ef          	jal	ra,2005ad00 <vPortFree>
2005e922:	b70d                	c.j	2005e844 <prvTimerTask+0x90>

2005e924 <xTimerIsTimerActive>:
2005e924:	1101                	c.addi	sp,-32
2005e926:	ce06                	c.swsp	ra,28(sp)
2005e928:	cc22                	c.swsp	s0,24(sp)
2005e92a:	842a                	c.mv	s0,a0
2005e92c:	aeeff0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005e930:	02844503          	lbu	a0,40(s0)
2005e934:	8905                	c.andi	a0,1
2005e936:	c62a                	c.swsp	a0,12(sp)
2005e938:	afcff0ef          	jal	ra,2005dc34 <vTaskExitCritical>
2005e93c:	40f2                	c.lwsp	ra,28(sp)
2005e93e:	4462                	c.lwsp	s0,24(sp)
2005e940:	4532                	c.lwsp	a0,12(sp)
2005e942:	6105                	c.addi16sp	sp,32
2005e944:	8082                	c.jr	ra

2005e946 <wifi_fast_connect_enable>:
2005e946:	fa01ac23          	sw	zero,-72(gp) # 20066940 <p_wifi_do_fast_connect>
2005e94a:	fa01aa23          	sw	zero,-76(gp) # 2006693c <p_store_fast_connect_info>
2005e94e:	8082                	c.jr	ra

2005e950 <rtw_indicate_event_handle>:
2005e950:	4761                	c.li	a4,24
2005e952:	04a74e63          	blt	a4,a0,2005e9ae <rtw_indicate_event_handle+0x5e>
2005e956:	1101                	c.addi	sp,-32
2005e958:	20068737          	lui	a4,0x20068
2005e95c:	ca26                	c.swsp	s1,20(sp)
2005e95e:	c84a                	c.swsp	s2,16(sp)
2005e960:	00451493          	slli	s1,a0,0x4
2005e964:	892e                	c.mv	s2,a1
2005e966:	85b2                	c.mv	a1,a2
2005e968:	8636                	c.mv	a2,a3
2005e96a:	51870693          	addi	a3,a4,1304 # 20068518 <event_callback_list>
2005e96e:	cc22                	c.swsp	s0,24(sp)
2005e970:	ce06                	c.swsp	ra,28(sp)
2005e972:	009687b3          	add	a5,a3,s1
2005e976:	0007a803          	lw	a6,0(a5)
2005e97a:	51870413          	addi	s0,a4,1304
2005e97e:	00080963          	beq	a6,zero,2005e990 <rtw_indicate_event_handle+0x40>
2005e982:	43d4                	c.lw	a3,4(a5)
2005e984:	c632                	c.swsp	a2,12(sp)
2005e986:	c42e                	c.swsp	a1,8(sp)
2005e988:	854a                	c.mv	a0,s2
2005e98a:	9802                	c.jalr	a6
2005e98c:	4632                	c.lwsp	a2,12(sp)
2005e98e:	45a2                	c.lwsp	a1,8(sp)
2005e990:	009407b3          	add	a5,s0,s1
2005e994:	4798                	c.lw	a4,8(a5)
2005e996:	4501                	c.li	a0,0
2005e998:	c709                	c.beqz	a4,2005e9a2 <rtw_indicate_event_handle+0x52>
2005e99a:	47d4                	c.lw	a3,12(a5)
2005e99c:	854a                	c.mv	a0,s2
2005e99e:	9702                	c.jalr	a4
2005e9a0:	4501                	c.li	a0,0
2005e9a2:	40f2                	c.lwsp	ra,28(sp)
2005e9a4:	4462                	c.lwsp	s0,24(sp)
2005e9a6:	44d2                	c.lwsp	s1,20(sp)
2005e9a8:	4942                	c.lwsp	s2,16(sp)
2005e9aa:	6105                	c.addi16sp	sp,32
2005e9ac:	8082                	c.jr	ra
2005e9ae:	5579                	c.li	a0,-2
2005e9b0:	8082                	c.jr	ra

2005e9b2 <wifi_indication>:
2005e9b2:	472d                	c.li	a4,11
2005e9b4:	00e51863          	bne	a0,a4,2005e9c4 <wifi_indication+0x12>
2005e9b8:	fbc1a703          	lw	a4,-68(gp) # 20066944 <p_wifi_joinstatus_internal_callback>
2005e9bc:	c711                	c.beqz	a4,2005e9c8 <wifi_indication+0x16>
2005e9be:	0ff6f513          	andi	a0,a3,255
2005e9c2:	8702                	c.jr	a4
2005e9c4:	f8dff06f          	jal	zero,2005e950 <rtw_indicate_event_handle>
2005e9c8:	8082                	c.jr	ra

2005e9ca <_init_thread>:
2005e9ca:	1141                	c.addi	sp,-16
2005e9cc:	c422                	c.swsp	s0,8(sp)
2005e9ce:	c606                	c.swsp	ra,12(sp)
2005e9d0:	4100c437          	lui	s0,0x4100c
2005e9d4:	08442783          	lw	a5,132(s0) # 4100c084 <__km4_bd_boot_download_addr__+0x10ffa084>
2005e9d8:	08440713          	addi	a4,s0,132
2005e9dc:	8b85                	c.andi	a5,1
2005e9de:	c7b1                	c.beqz	a5,2005ea2a <_init_thread+0x60>
2005e9e0:	431c                	c.lw	a5,0(a4)
2005e9e2:	4505                	c.li	a0,1
2005e9e4:	9bf9                	c.andi	a5,-2
2005e9e6:	c31c                	c.sw	a5,0(a4)
2005e9e8:	17d010ef          	jal	ra,20060364 <wifi_on>
2005e9ec:	4505                	c.li	a0,1
2005e9ee:	211010ef          	jal	ra,200603fe <wifi_config_autoreconnect>
2005e9f2:	d35fc0ef          	jal	ra,2005b726 <rtos_mem_get_free_heap_size>
2005e9f6:	20063737          	lui	a4,0x20063
2005e9fa:	200636b7          	lui	a3,0x20063
2005e9fe:	200635b7          	lui	a1,0x20063
2005ea02:	882a                	c.mv	a6,a0
2005ea04:	03500793          	addi	a5,zero,53
2005ea08:	4511                	c.li	a0,4
2005ea0a:	73070713          	addi	a4,a4,1840 # 20063730 <__FUNCTION__.0>
2005ea0e:	6ec68693          	addi	a3,a3,1772 # 200636ec <__func__.1+0x60>
2005ea12:	04900613          	addi	a2,zero,73
2005ea16:	70858593          	addi	a1,a1,1800 # 20063708 <__func__.1+0x7c>
2005ea1a:	f74fc0ef          	jal	ra,2005b18e <rtk_log_write>
2005ea1e:	4422                	c.lwsp	s0,8(sp)
2005ea20:	40b2                	c.lwsp	ra,12(sp)
2005ea22:	4501                	c.li	a0,0
2005ea24:	0141                	c.addi	sp,16
2005ea26:	9c0fd06f          	jal	zero,2005bbe6 <rtos_task_delete>
2005ea2a:	4505                	c.li	a0,1
2005ea2c:	9ccfd0ef          	jal	ra,2005bbf8 <rtos_time_delay_ms>
2005ea30:	b755                	c.j	2005e9d4 <_init_thread+0xa>

2005ea32 <wlan_initialize>:
2005ea32:	1141                	c.addi	sp,-16
2005ea34:	c606                	c.swsp	ra,12(sp)
2005ea36:	041010ef          	jal	ra,20060276 <wifi_set_rom2flash>
2005ea3a:	1d2020ef          	jal	ra,20060c0c <inic_host_init>
2005ea3e:	4505                	c.li	a0,1
2005ea40:	f07ff0ef          	jal	ra,2005e946 <wifi_fast_connect_enable>
2005ea44:	6705                	c.lui	a4,0x1
2005ea46:	2005f637          	lui	a2,0x2005f
2005ea4a:	200635b7          	lui	a1,0x20063
2005ea4e:	4789                	c.li	a5,2
2005ea50:	40070713          	addi	a4,a4,1024 # 1400 <CPU_ClkGet+0x8ea>
2005ea54:	4681                	c.li	a3,0
2005ea56:	9ca60613          	addi	a2,a2,-1590 # 2005e9ca <_init_thread>
2005ea5a:	71058593          	addi	a1,a1,1808 # 20063710 <__func__.1+0x84>
2005ea5e:	4501                	c.li	a0,0
2005ea60:	962fd0ef          	jal	ra,2005bbc2 <rtos_task_create>
2005ea64:	c105                	c.beqz	a0,2005ea84 <wlan_initialize+0x52>
2005ea66:	40b2                	c.lwsp	ra,12(sp)
2005ea68:	200636b7          	lui	a3,0x20063
2005ea6c:	200635b7          	lui	a1,0x20063
2005ea70:	71868693          	addi	a3,a3,1816 # 20063718 <__func__.1+0x8c>
2005ea74:	04500613          	addi	a2,zero,69
2005ea78:	70858593          	addi	a1,a1,1800 # 20063708 <__func__.1+0x7c>
2005ea7c:	4509                	c.li	a0,2
2005ea7e:	0141                	c.addi	sp,16
2005ea80:	f0efc06f          	jal	zero,2005b18e <rtk_log_write>
2005ea84:	40b2                	c.lwsp	ra,12(sp)
2005ea86:	0141                	c.addi	sp,16
2005ea88:	8082                	c.jr	ra

2005ea8a <ChipInfo_GetSocName_ToBuf>:
2005ea8a:	7139                	c.addi16sp	sp,-64
2005ea8c:	da26                	c.swsp	s1,52(sp)
2005ea8e:	84ae                	c.mv	s1,a1
2005ea90:	200645b7          	lui	a1,0x20064
2005ea94:	d84a                	c.swsp	s2,48(sp)
2005ea96:	4625                	c.li	a2,9
2005ea98:	892a                	c.mv	s2,a0
2005ea9a:	83f58593          	addi	a1,a1,-1985 # 2006383f <__FUNCTION__.0+0x10f>
2005ea9e:	0068                	c.addi4spn	a0,sp,12
2005eaa0:	dc22                	c.swsp	s0,56(sp)
2005eaa2:	de06                	c.swsp	ra,60(sp)
2005eaa4:	d64e                	c.swsp	s3,44(sp)
2005eaa6:	d452                	c.swsp	s4,40(sp)
2005eaa8:	dffab097          	auipc	ra,0xdffab
2005eaac:	028080e7          	jalr	ra,40(ra) # 9ad0 <__wrap_memcpy>
2005eab0:	00010aa3          	sb	zero,21(sp)
2005eab4:	4401                	c.li	s0,0
2005eab6:	c899                	c.beqz	s1,2005eacc <ChipInfo_GetSocName_ToBuf+0x42>
2005eab8:	20064637          	lui	a2,0x20064
2005eabc:	0074                	c.addi4spn	a3,sp,12
2005eabe:	87060613          	addi	a2,a2,-1936 # 20063870 <__FUNCTION__.0+0x140>
2005eac2:	85a6                	c.mv	a1,s1
2005eac4:	854a                	c.mv	a0,s2
2005eac6:	29f000ef          	jal	ra,2005f564 <DiagSnPrintf>
2005eaca:	842a                	c.mv	s0,a0
2005eacc:	7f000993          	addi	s3,zero,2032
2005ead0:	20064a37          	lui	s4,0x20064
2005ead4:	00b10593          	addi	a1,sp,11
2005ead8:	854e                	c.mv	a0,s3
2005eada:	9c3fa0ef          	jal	ra,2005949c <OTP_Read8>
2005eade:	e131                	c.bnez	a0,2005eb22 <ChipInfo_GetSocName_ToBuf+0x98>
2005eae0:	200646b7          	lui	a3,0x20064
2005eae4:	200635b7          	lui	a1,0x20063
2005eae8:	87868693          	addi	a3,a3,-1928 # 20063878 <__FUNCTION__.0+0x148>
2005eaec:	04500613          	addi	a2,zero,69
2005eaf0:	78458593          	addi	a1,a1,1924 # 20063784 <__FUNCTION__.0+0x54>
2005eaf4:	4509                	c.li	a0,2
2005eaf6:	e98fc0ef          	jal	ra,2005b18e <rtk_log_write>
2005eafa:	00947c63          	bgeu	s0,s1,2005eb12 <ChipInfo_GetSocName_ToBuf+0x88>
2005eafe:	20063637          	lui	a2,0x20063
2005eb02:	8d860613          	addi	a2,a2,-1832 # 200628d8 <pmap_func+0x4ac>
2005eb06:	408485b3          	sub	a1,s1,s0
2005eb0a:	00890533          	add	a0,s2,s0
2005eb0e:	257000ef          	jal	ra,2005f564 <DiagSnPrintf>
2005eb12:	50f2                	c.lwsp	ra,60(sp)
2005eb14:	5462                	c.lwsp	s0,56(sp)
2005eb16:	54d2                	c.lwsp	s1,52(sp)
2005eb18:	5942                	c.lwsp	s2,48(sp)
2005eb1a:	59b2                	c.lwsp	s3,44(sp)
2005eb1c:	5a22                	c.lwsp	s4,40(sp)
2005eb1e:	6121                	c.addi16sp	sp,64
2005eb20:	8082                	c.jr	ra
2005eb22:	00947d63          	bgeu	s0,s1,2005eb3c <ChipInfo_GetSocName_ToBuf+0xb2>
2005eb26:	00b14683          	lbu	a3,11(sp)
2005eb2a:	408485b3          	sub	a1,s1,s0
2005eb2e:	00890533          	add	a0,s2,s0
2005eb32:	890a0613          	addi	a2,s4,-1904 # 20063890 <__FUNCTION__.0+0x160>
2005eb36:	22f000ef          	jal	ra,2005f564 <DiagSnPrintf>
2005eb3a:	942a                	c.add	s0,a0
2005eb3c:	0985                	c.addi	s3,1
2005eb3e:	80098793          	addi	a5,s3,-2048
2005eb42:	fbc9                	c.bnez	a5,2005ead4 <ChipInfo_GetSocName_ToBuf+0x4a>
2005eb44:	bf5d                	c.j	2005eafa <ChipInfo_GetSocName_ToBuf+0x70>

2005eb46 <ChipInfo_GetLibVersion_ToBuf>:
2005eb46:	1101                	c.addi	sp,-32
2005eb48:	cc22                	c.swsp	s0,24(sp)
2005eb4a:	ca26                	c.swsp	s1,20(sp)
2005eb4c:	c64e                	c.swsp	s3,12(sp)
2005eb4e:	ce06                	c.swsp	ra,28(sp)
2005eb50:	c84a                	c.swsp	s2,16(sp)
2005eb52:	c452                	c.swsp	s4,8(sp)
2005eb54:	c256                	c.swsp	s5,4(sp)
2005eb56:	c05a                	c.swsp	s6,0(sp)
2005eb58:	4100c7b7          	lui	a5,0x4100c
2005eb5c:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005eb60:	89aa                	c.mv	s3,a0
2005eb62:	84ae                	c.mv	s1,a1
2005eb64:	4401                	c.li	s0,0
2005eb66:	c1b5                	c.beqz	a1,2005ebca <ChipInfo_GetLibVersion_ToBuf+0x84>
2005eb68:	8399                	c.srli	a5,0x6
2005eb6a:	8b8d                	c.andi	a5,3
2005eb6c:	e7d9                	c.bnez	a5,2005ebfa <ChipInfo_GetLibVersion_ToBuf+0xb4>
2005eb6e:	200636b7          	lui	a3,0x20063
2005eb72:	4d468693          	addi	a3,a3,1236 # 200634d4 <__FUNCTION__.0+0x1c>
2005eb76:	20064637          	lui	a2,0x20064
2005eb7a:	89c60613          	addi	a2,a2,-1892 # 2006389c <__FUNCTION__.0+0x16c>
2005eb7e:	85a6                	c.mv	a1,s1
2005eb80:	854e                	c.mv	a0,s3
2005eb82:	1e3000ef          	jal	ra,2005f564 <DiagSnPrintf>
2005eb86:	842a                	c.mv	s0,a0
2005eb88:	04957163          	bgeu	a0,s1,2005ebca <ChipInfo_GetLibVersion_ToBuf+0x84>
2005eb8c:	200646b7          	lui	a3,0x20064
2005eb90:	20064637          	lui	a2,0x20064
2005eb94:	40a485b3          	sub	a1,s1,a0
2005eb98:	8b068693          	addi	a3,a3,-1872 # 200638b0 <__FUNCTION__.0+0x180>
2005eb9c:	8c460613          	addi	a2,a2,-1852 # 200638c4 <__FUNCTION__.0+0x194>
2005eba0:	954e                	c.add	a0,s3
2005eba2:	1c3000ef          	jal	ra,2005f564 <DiagSnPrintf>
2005eba6:	942a                	c.add	s0,a0
2005eba8:	02947163          	bgeu	s0,s1,2005ebca <ChipInfo_GetLibVersion_ToBuf+0x84>
2005ebac:	200646b7          	lui	a3,0x20064
2005ebb0:	20064637          	lui	a2,0x20064
2005ebb4:	408485b3          	sub	a1,s1,s0
2005ebb8:	00898533          	add	a0,s3,s0
2005ebbc:	8d868693          	addi	a3,a3,-1832 # 200638d8 <__FUNCTION__.0+0x1a8>
2005ebc0:	91860613          	addi	a2,a2,-1768 # 20063918 <__FUNCTION__.0+0x1e8>
2005ebc4:	1a1000ef          	jal	ra,2005f564 <DiagSnPrintf>
2005ebc8:	942a                	c.add	s0,a0
2005ebca:	20065937          	lui	s2,0x20065
2005ebce:	20065a37          	lui	s4,0x20065
2005ebd2:	d3c90913          	addi	s2,s2,-708 # 20064d3c <lib_pmc_git_rev>
2005ebd6:	d9da0a13          	addi	s4,s4,-611 # 20064d9d <__git_ver_table_end__>
2005ebda:	20064ab7          	lui	s5,0x20064
2005ebde:	20063b37          	lui	s6,0x20063
2005ebe2:	03496163          	bltu	s2,s4,2005ec04 <ChipInfo_GetLibVersion_ToBuf+0xbe>
2005ebe6:	40f2                	c.lwsp	ra,28(sp)
2005ebe8:	4462                	c.lwsp	s0,24(sp)
2005ebea:	44d2                	c.lwsp	s1,20(sp)
2005ebec:	4942                	c.lwsp	s2,16(sp)
2005ebee:	49b2                	c.lwsp	s3,12(sp)
2005ebf0:	4a22                	c.lwsp	s4,8(sp)
2005ebf2:	4a92                	c.lwsp	s5,4(sp)
2005ebf4:	4b02                	c.lwsp	s6,0(sp)
2005ebf6:	6105                	c.addi16sp	sp,32
2005ebf8:	8082                	c.jr	ra
2005ebfa:	200646b7          	lui	a3,0x20064
2005ebfe:	89868693          	addi	a3,a3,-1896 # 20063898 <__FUNCTION__.0+0x168>
2005ec02:	bf95                	c.j	2005eb76 <ChipInfo_GetLibVersion_ToBuf+0x30>
2005ec04:	00094683          	lbu	a3,0(s2)
2005ec08:	e28d                	c.bnez	a3,2005ec2a <ChipInfo_GetLibVersion_ToBuf+0xe4>
2005ec0a:	00194783          	lbu	a5,1(s2)
2005ec0e:	0905                	c.addi	s2,1
2005ec10:	dfed                	c.beqz	a5,2005ec0a <ChipInfo_GetLibVersion_ToBuf+0xc4>
2005ec12:	fc9478e3          	bgeu	s0,s1,2005ebe2 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ec16:	8d8b0613          	addi	a2,s6,-1832 # 200628d8 <pmap_func+0x4ac>
2005ec1a:	408485b3          	sub	a1,s1,s0
2005ec1e:	00898533          	add	a0,s3,s0
2005ec22:	143000ef          	jal	ra,2005f564 <DiagSnPrintf>
2005ec26:	942a                	c.add	s0,a0
2005ec28:	bf6d                	c.j	2005ebe2 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ec2a:	fa947ce3          	bgeu	s0,s1,2005ebe2 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ec2e:	928a8613          	addi	a2,s5,-1752 # 20063928 <__FUNCTION__.0+0x1f8>
2005ec32:	408485b3          	sub	a1,s1,s0
2005ec36:	00898533          	add	a0,s3,s0
2005ec3a:	0905                	c.addi	s2,1
2005ec3c:	129000ef          	jal	ra,2005f564 <DiagSnPrintf>
2005ec40:	b7dd                	c.j	2005ec26 <ChipInfo_GetLibVersion_ToBuf+0xe0>

2005ec42 <ChipInfo_GetChipSram>:
2005ec42:	1101                	c.addi	sp,-32
2005ec44:	ce06                	c.swsp	ra,28(sp)
2005ec46:	4100c7b7          	lui	a5,0x4100c
2005ec4a:	2107a783          	lw	a5,528(a5) # 4100c210 <__km4_bd_boot_download_addr__+0x10ffa210>
2005ec4e:	40400737          	lui	a4,0x40400
2005ec52:	8ff9                	c.and	a5,a4
2005ec54:	c39d                	c.beqz	a5,2005ec7a <ChipInfo_GetChipSram+0x38>
2005ec56:	00f10593          	addi	a1,sp,15
2005ec5a:	7fc00513          	addi	a0,zero,2044
2005ec5e:	83ffa0ef          	jal	ra,2005949c <OTP_Read8>
2005ec62:	00f14783          	lbu	a5,15(sp)
2005ec66:	470d                	c.li	a4,3
2005ec68:	8389                	c.srli	a5,0x2
2005ec6a:	8b8d                	c.andi	a5,3
2005ec6c:	00e79663          	bne	a5,a4,2005ec78 <ChipInfo_GetChipSram+0x36>
2005ec70:	4501                	c.li	a0,0
2005ec72:	40f2                	c.lwsp	ra,28(sp)
2005ec74:	6105                	c.addi16sp	sp,32
2005ec76:	8082                	c.jr	ra
2005ec78:	dfe5                	c.beqz	a5,2005ec70 <ChipInfo_GetChipSram+0x2e>
2005ec7a:	00f10593          	addi	a1,sp,15
2005ec7e:	7fc00513          	addi	a0,zero,2044
2005ec82:	81bfa0ef          	jal	ra,2005949c <OTP_Read8>
2005ec86:	00f14783          	lbu	a5,15(sp)
2005ec8a:	470d                	c.li	a4,3
2005ec8c:	8399                	c.srli	a5,0x6
2005ec8e:	00e78363          	beq	a5,a4,2005ec94 <ChipInfo_GetChipSram+0x52>
2005ec92:	fff9                	c.bnez	a5,2005ec70 <ChipInfo_GetChipSram+0x2e>
2005ec94:	4505                	c.li	a0,1
2005ec96:	bff1                	c.j	2005ec72 <ChipInfo_GetChipSram+0x30>

2005ec98 <vTaskStatus>:
2005ec98:	7139                	c.addi16sp	sp,-64
2005ec9a:	de06                	c.swsp	ra,60(sp)
2005ec9c:	dc22                	c.swsp	s0,56(sp)
2005ec9e:	da26                	c.swsp	s1,52(sp)
2005eca0:	f7bfe0ef          	jal	ra,2005dc1a <vTaskEnterCritical>
2005eca4:	d8ffe0ef          	jal	ra,2005da32 <xTaskGetCurrentTaskHandle>
2005eca8:	4695                	c.li	a3,5
2005ecaa:	4605                	c.li	a2,1
2005ecac:	006c                	c.addi4spn	a1,sp,12
2005ecae:	842a                	c.mv	s0,a0
2005ecb0:	200644b7          	lui	s1,0x20064
2005ecb4:	c8eff0ef          	jal	ra,2005e142 <vTaskGetInfo>
2005ecb8:	92c48513          	addi	a0,s1,-1748 # 2006392c <__FUNCTION__.0+0x1fc>
2005ecbc:	25e9                	c.jal	2005f386 <DiagPrintf>
2005ecbe:	45c2                	c.lwsp	a1,16(sp)
2005ecc0:	20064537          	lui	a0,0x20064
2005ecc4:	95050513          	addi	a0,a0,-1712 # 20063950 <__FUNCTION__.0+0x220>
2005ecc8:	2d7d                	c.jal	2005f386 <DiagPrintf>
2005ecca:	45d2                	c.lwsp	a1,20(sp)
2005eccc:	20064537          	lui	a0,0x20064
2005ecd0:	96450513          	addi	a0,a0,-1692 # 20063964 <__FUNCTION__.0+0x234>
2005ecd4:	2d4d                	c.jal	2005f386 <DiagPrintf>
2005ecd6:	20064537          	lui	a0,0x20064
2005ecda:	85a2                	c.mv	a1,s0
2005ecdc:	97450513          	addi	a0,a0,-1676 # 20063974 <__FUNCTION__.0+0x244>
2005ece0:	255d                	c.jal	2005f386 <DiagPrintf>
2005ece2:	01814583          	lbu	a1,24(sp)
2005ece6:	200647b7          	lui	a5,0x20064
2005ecea:	a9878793          	addi	a5,a5,-1384 # 20063a98 <TaskStateString.0>
2005ecee:	00259713          	slli	a4,a1,0x2
2005ecf2:	97ba                	c.add	a5,a4
2005ecf4:	4390                	c.lw	a2,0(a5)
2005ecf6:	20064537          	lui	a0,0x20064
2005ecfa:	98850513          	addi	a0,a0,-1656 # 20063988 <__FUNCTION__.0+0x258>
2005ecfe:	2561                	c.jal	2005f386 <DiagPrintf>
2005ed00:	5582                	c.lwsp	a1,32(sp)
2005ed02:	20064537          	lui	a0,0x20064
2005ed06:	9a450513          	addi	a0,a0,-1628 # 200639a4 <__FUNCTION__.0+0x274>
2005ed0a:	2db5                	c.jal	2005f386 <DiagPrintf>
2005ed0c:	45f2                	c.lwsp	a1,28(sp)
2005ed0e:	20064537          	lui	a0,0x20064
2005ed12:	9b850513          	addi	a0,a0,-1608 # 200639b8 <__FUNCTION__.0+0x288>
2005ed16:	2d85                	c.jal	2005f386 <DiagPrintf>
2005ed18:	5592                	c.lwsp	a1,36(sp)
2005ed1a:	20064537          	lui	a0,0x20064
2005ed1e:	9d050513          	addi	a0,a0,-1584 # 200639d0 <__FUNCTION__.0+0x2a0>
2005ed22:	2595                	c.jal	2005f386 <DiagPrintf>
2005ed24:	4474                	c.lw	a3,76(s0)
2005ed26:	5818                	c.lw	a4,48(s0)
2005ed28:	5622                	c.lwsp	a2,40(sp)
2005ed2a:	400c                	c.lw	a1,0(s0)
2005ed2c:	40e68733          	sub	a4,a3,a4
2005ed30:	8709                	c.srai	a4,0x2
2005ed32:	20064537          	lui	a0,0x20064
2005ed36:	0705                	c.addi	a4,1
2005ed38:	9e850513          	addi	a0,a0,-1560 # 200639e8 <__FUNCTION__.0+0x2b8>
2005ed3c:	25a9                	c.jal	2005f386 <DiagPrintf>
2005ed3e:	02c15583          	lhu	a1,44(sp)
2005ed42:	20064537          	lui	a0,0x20064
2005ed46:	a3850513          	addi	a0,a0,-1480 # 20063a38 <__FUNCTION__.0+0x308>
2005ed4a:	2d35                	c.jal	2005f386 <DiagPrintf>
2005ed4c:	92c48513          	addi	a0,s1,-1748
2005ed50:	2d1d                	c.jal	2005f386 <DiagPrintf>
2005ed52:	50f2                	c.lwsp	ra,60(sp)
2005ed54:	5462                	c.lwsp	s0,56(sp)
2005ed56:	54d2                	c.lwsp	s1,52(sp)
2005ed58:	6121                	c.addi16sp	sp,64
2005ed5a:	8082                	c.jr	ra

2005ed5c <vTaskCrashCallback>:
2005ed5c:	1141                	c.addi	sp,-16
2005ed5e:	c606                	c.swsp	ra,12(sp)
2005ed60:	e3dfc0ef          	jal	ra,2005bb9c <rtos_sched_get_state>
2005ed64:	4785                	c.li	a5,1
2005ed66:	00f50663          	beq	a0,a5,2005ed72 <vTaskCrashCallback+0x16>
2005ed6a:	40b2                	c.lwsp	ra,12(sp)
2005ed6c:	0141                	c.addi	sp,16
2005ed6e:	f2bff06f          	jal	zero,2005ec98 <vTaskStatus>
2005ed72:	40b2                	c.lwsp	ra,12(sp)
2005ed74:	0141                	c.addi	sp,16
2005ed76:	8082                	c.jr	ra

2005ed78 <print_unsigned_num>:
2005ed78:	715d                	c.addi16sp	sp,-80
2005ed7a:	c4a2                	c.swsp	s0,72(sp)
2005ed7c:	c2a6                	c.swsp	s1,68(sp)
2005ed7e:	c0ca                	c.swsp	s2,64(sp)
2005ed80:	de4e                	c.swsp	s3,60(sp)
2005ed82:	dc52                	c.swsp	s4,56(sp)
2005ed84:	da56                	c.swsp	s5,52(sp)
2005ed86:	d85a                	c.swsp	s6,48(sp)
2005ed88:	d65e                	c.swsp	s7,44(sp)
2005ed8a:	d462                	c.swsp	s8,40(sp)
2005ed8c:	d266                	c.swsp	s9,36(sp)
2005ed8e:	d06a                	c.swsp	s10,32(sp)
2005ed90:	c686                	c.swsp	ra,76(sp)
2005ed92:	8c2a                	c.mv	s8,a0
2005ed94:	8cae                	c.mv	s9,a1
2005ed96:	8d32                	c.mv	s10,a2
2005ed98:	8936                	c.mv	s2,a3
2005ed9a:	8bba                	c.mv	s7,a4
2005ed9c:	89be                	c.mv	s3,a5
2005ed9e:	84c2                	c.mv	s1,a6
2005eda0:	4401                	c.li	s0,0
2005eda2:	4a81                	c.li	s5,0
2005eda4:	4a25                	c.li	s4,9
2005eda6:	4b05                	c.li	s6,1
2005eda8:	866a                	c.mv	a2,s10
2005edaa:	4681                	c.li	a3,0
2005edac:	8562                	c.mv	a0,s8
2005edae:	85e6                	c.mv	a1,s9
2005edb0:	2a9020ef          	jal	ra,20061858 <__umoddi3>
2005edb4:	0ff57893          	andi	a7,a0,255
2005edb8:	04aa6863          	bltu	s4,a0,2005ee08 <print_unsigned_num+0x90>
2005edbc:	03088893          	addi	a7,a7,48 # f8000030 <__ctrace_end__+0x2e000030>
2005edc0:	007c                	c.addi4spn	a5,sp,12
2005edc2:	97a2                	c.add	a5,s0
2005edc4:	0ff8f893          	andi	a7,a7,255
2005edc8:	01178023          	sb	a7,0(a5)
2005edcc:	866a                	c.mv	a2,s10
2005edce:	4681                	c.li	a3,0
2005edd0:	8562                	c.mv	a0,s8
2005edd2:	85e6                	c.mv	a1,s9
2005edd4:	0405                	c.addi	s0,1
2005edd6:	722020ef          	jal	ra,200614f8 <__udivdi3>
2005edda:	039a9463          	bne	s5,s9,2005ee02 <print_unsigned_num+0x8a>
2005edde:	03ac7263          	bgeu	s8,s10,2005ee02 <print_unsigned_num+0x8a>
2005ede2:	c8d5                	c.beqz	s1,2005ee96 <print_unsigned_num+0x11e>
2005ede4:	03000793          	addi	a5,zero,48
2005ede8:	1bfd                	c.addi	s7,-1
2005edea:	0af91163          	bne	s2,a5,2005ee8c <print_unsigned_num+0x114>
2005edee:	02d00513          	addi	a0,zero,45
2005edf2:	dffa5097          	auipc	ra,0xdffa5
2005edf6:	752080e7          	jalr	ra,1874(ra) # 4544 <LOGUART_PutChar>
2005edfa:	89a6                	c.mv	s3,s1
2005edfc:	09704b63          	blt	zero,s7,2005ee92 <print_unsigned_num+0x11a>
2005ee00:	a0b1                	c.j	2005ee4c <print_unsigned_num+0xd4>
2005ee02:	8c2a                	c.mv	s8,a0
2005ee04:	8cae                	c.mv	s9,a1
2005ee06:	b74d                	c.j	2005eda8 <print_unsigned_num+0x30>
2005ee08:	01699563          	bne	s3,s6,2005ee12 <print_unsigned_num+0x9a>
2005ee0c:	03788893          	addi	a7,a7,55
2005ee10:	bf45                	c.j	2005edc0 <print_unsigned_num+0x48>
2005ee12:	05788893          	addi	a7,a7,87
2005ee16:	b76d                	c.j	2005edc0 <print_unsigned_num+0x48>
2005ee18:	854a                	c.mv	a0,s2
2005ee1a:	dffa5097          	auipc	ra,0xdffa5
2005ee1e:	72a080e7          	jalr	ra,1834(ra) # 4544 <LOGUART_PutChar>
2005ee22:	1a7d                	c.addi	s4,-1
2005ee24:	ff444ae3          	blt	s0,s4,2005ee18 <print_unsigned_num+0xa0>
2005ee28:	4781                	c.li	a5,0
2005ee2a:	008bc463          	blt	s7,s0,2005ee32 <print_unsigned_num+0xba>
2005ee2e:	408b87b3          	sub	a5,s7,s0
2005ee32:	97ce                	c.add	a5,s3
2005ee34:	c0a9                	c.beqz	s1,2005ee76 <print_unsigned_num+0xfe>
2005ee36:	84be                	c.mv	s1,a5
2005ee38:	0df97913          	andi	s2,s2,223
2005ee3c:	00091863          	bne	s2,zero,2005ee4c <print_unsigned_num+0xd4>
2005ee40:	02d00513          	addi	a0,zero,45
2005ee44:	dffa5097          	auipc	ra,0xdffa5
2005ee48:	700080e7          	jalr	ra,1792(ra) # 4544 <LOGUART_PutChar>
2005ee4c:	8922                	c.mv	s2,s0
2005ee4e:	59fd                	c.li	s3,-1
2005ee50:	197d                	c.addi	s2,-1
2005ee52:	03391463          	bne	s2,s3,2005ee7a <print_unsigned_num+0x102>
2005ee56:	00940533          	add	a0,s0,s1
2005ee5a:	40b6                	c.lwsp	ra,76(sp)
2005ee5c:	4426                	c.lwsp	s0,72(sp)
2005ee5e:	4496                	c.lwsp	s1,68(sp)
2005ee60:	4906                	c.lwsp	s2,64(sp)
2005ee62:	59f2                	c.lwsp	s3,60(sp)
2005ee64:	5a62                	c.lwsp	s4,56(sp)
2005ee66:	5ad2                	c.lwsp	s5,52(sp)
2005ee68:	5b42                	c.lwsp	s6,48(sp)
2005ee6a:	5bb2                	c.lwsp	s7,44(sp)
2005ee6c:	5c22                	c.lwsp	s8,40(sp)
2005ee6e:	5c92                	c.lwsp	s9,36(sp)
2005ee70:	5d02                	c.lwsp	s10,32(sp)
2005ee72:	6161                	c.addi16sp	sp,80
2005ee74:	8082                	c.jr	ra
2005ee76:	84be                	c.mv	s1,a5
2005ee78:	bfd1                	c.j	2005ee4c <print_unsigned_num+0xd4>
2005ee7a:	007c                	c.addi4spn	a5,sp,12
2005ee7c:	97ca                	c.add	a5,s2
2005ee7e:	0007c503          	lbu	a0,0(a5)
2005ee82:	dffa5097          	auipc	ra,0xdffa5
2005ee86:	6c2080e7          	jalr	ra,1730(ra) # 4544 <LOGUART_PutChar>
2005ee8a:	b7d9                	c.j	2005ee50 <print_unsigned_num+0xd8>
2005ee8c:	fb7056e3          	bge	zero,s7,2005ee38 <print_unsigned_num+0xc0>
2005ee90:	89a6                	c.mv	s3,s1
2005ee92:	8a5e                	c.mv	s4,s7
2005ee94:	bf41                	c.j	2005ee24 <print_unsigned_num+0xac>
2005ee96:	4981                	c.li	s3,0
2005ee98:	b795                	c.j	2005edfc <print_unsigned_num+0x84>

2005ee9a <pad_char_control>:
2005ee9a:	1141                	c.addi	sp,-16
2005ee9c:	c422                	c.swsp	s0,8(sp)
2005ee9e:	c226                	c.swsp	s1,4(sp)
2005eea0:	c04a                	c.swsp	s2,0(sp)
2005eea2:	c606                	c.swsp	ra,12(sp)
2005eea4:	842a                	c.mv	s0,a0
2005eea6:	892e                	c.mv	s2,a1
2005eea8:	84aa                	c.mv	s1,a0
2005eeaa:	00904c63          	blt	zero,s1,2005eec2 <pad_char_control+0x28>
2005eeae:	8522                	c.mv	a0,s0
2005eeb0:	00045363          	bge	s0,zero,2005eeb6 <pad_char_control+0x1c>
2005eeb4:	4501                	c.li	a0,0
2005eeb6:	40b2                	c.lwsp	ra,12(sp)
2005eeb8:	4422                	c.lwsp	s0,8(sp)
2005eeba:	4492                	c.lwsp	s1,4(sp)
2005eebc:	4902                	c.lwsp	s2,0(sp)
2005eebe:	0141                	c.addi	sp,16
2005eec0:	8082                	c.jr	ra
2005eec2:	854a                	c.mv	a0,s2
2005eec4:	dffa5097          	auipc	ra,0xdffa5
2005eec8:	680080e7          	jalr	ra,1664(ra) # 4544 <LOGUART_PutChar>
2005eecc:	14fd                	c.addi	s1,-1
2005eece:	bff1                	c.j	2005eeaa <pad_char_control+0x10>

2005eed0 <print_decimal_num>:
2005eed0:	1101                	c.addi	sp,-32
2005eed2:	c64e                	c.swsp	s3,12(sp)
2005eed4:	200689b7          	lui	s3,0x20068
2005eed8:	cc22                	c.swsp	s0,24(sp)
2005eeda:	c84a                	c.swsp	s2,16(sp)
2005eedc:	c452                	c.swsp	s4,8(sp)
2005eede:	c05a                	c.swsp	s6,0(sp)
2005eee0:	8936                	c.mv	s2,a3
2005eee2:	ce06                	c.swsp	ra,28(sp)
2005eee4:	ca26                	c.swsp	s1,20(sp)
2005eee6:	c256                	c.swsp	s5,4(sp)
2005eee8:	882a                	c.mv	a6,a0
2005eeea:	8b32                	c.mv	s6,a2
2005eeec:	8a3a                	c.mv	s4,a4
2005eeee:	4401                	c.li	s0,0
2005eef0:	46a5                	c.li	a3,9
2005eef2:	6a898993          	addi	s3,s3,1704 # 200686a8 <num_buf_temp>
2005eef6:	02b87733          	remu	a4,a6,a1
2005eefa:	0ff77793          	andi	a5,a4,255
2005eefe:	04e6ee63          	bltu	a3,a4,2005ef5a <print_decimal_num+0x8a>
2005ef02:	03078793          	addi	a5,a5,48
2005ef06:	0ff7f793          	andi	a5,a5,255
2005ef0a:	00898733          	add	a4,s3,s0
2005ef0e:	00f70023          	sb	a5,0(a4) # 40400000 <__km4_bd_boot_download_addr__+0x103ee000>
2005ef12:	00140493          	addi	s1,s0,1
2005ef16:	02b857b3          	divu	a5,a6,a1
2005ef1a:	02b87d63          	bgeu	a6,a1,2005ef54 <print_decimal_num+0x84>
2005ef1e:	8aca                	c.mv	s5,s2
2005ef20:	05204663          	blt	zero,s2,2005ef6c <print_decimal_num+0x9c>
2005ef24:	4901                	c.li	s2,0
2005ef26:	02d00513          	addi	a0,zero,45
2005ef2a:	040a1e63          	bne	s4,zero,2005ef86 <print_decimal_num+0xb6>
2005ef2e:	14fd                	c.addi	s1,-1
2005ef30:	0404d763          	bge	s1,zero,2005ef7e <print_decimal_num+0xae>
2005ef34:	00045363          	bge	s0,zero,2005ef3a <print_decimal_num+0x6a>
2005ef38:	547d                	c.li	s0,-1
2005ef3a:	0405                	c.addi	s0,1
2005ef3c:	01240533          	add	a0,s0,s2
2005ef40:	40f2                	c.lwsp	ra,28(sp)
2005ef42:	4462                	c.lwsp	s0,24(sp)
2005ef44:	44d2                	c.lwsp	s1,20(sp)
2005ef46:	4942                	c.lwsp	s2,16(sp)
2005ef48:	49b2                	c.lwsp	s3,12(sp)
2005ef4a:	4a22                	c.lwsp	s4,8(sp)
2005ef4c:	4a92                	c.lwsp	s5,4(sp)
2005ef4e:	4b02                	c.lwsp	s6,0(sp)
2005ef50:	6105                	c.addi16sp	sp,32
2005ef52:	8082                	c.jr	ra
2005ef54:	8426                	c.mv	s0,s1
2005ef56:	883e                	c.mv	a6,a5
2005ef58:	bf79                	c.j	2005eef6 <print_decimal_num+0x26>
2005ef5a:	05778793          	addi	a5,a5,87
2005ef5e:	b765                	c.j	2005ef06 <print_decimal_num+0x36>
2005ef60:	855a                	c.mv	a0,s6
2005ef62:	dffa5097          	auipc	ra,0xdffa5
2005ef66:	5e2080e7          	jalr	ra,1506(ra) # 4544 <LOGUART_PutChar>
2005ef6a:	1afd                	c.addi	s5,-1
2005ef6c:	ff54cae3          	blt	s1,s5,2005ef60 <print_decimal_num+0x90>
2005ef70:	4781                	c.li	a5,0
2005ef72:	00994463          	blt	s2,s1,2005ef7a <print_decimal_num+0xaa>
2005ef76:	409907b3          	sub	a5,s2,s1
2005ef7a:	893e                	c.mv	s2,a5
2005ef7c:	b76d                	c.j	2005ef26 <print_decimal_num+0x56>
2005ef7e:	009987b3          	add	a5,s3,s1
2005ef82:	0007c503          	lbu	a0,0(a5)
2005ef86:	dffa5097          	auipc	ra,0xdffa5
2005ef8a:	5be080e7          	jalr	ra,1470(ra) # 4544 <LOGUART_PutChar>
2005ef8e:	b745                	c.j	2005ef2e <print_decimal_num+0x5e>

2005ef90 <pad_num_control>:
2005ef90:	04088163          	beq	a7,zero,2005efd2 <pad_num_control+0x42>
2005ef94:	1141                	c.addi	sp,-16
2005ef96:	c226                	c.swsp	s1,4(sp)
2005ef98:	84ba                	c.mv	s1,a4
2005ef9a:	4701                	c.li	a4,0
2005ef9c:	c422                	c.swsp	s0,8(sp)
2005ef9e:	c04a                	c.swsp	s2,0(sp)
2005efa0:	c606                	c.swsp	ra,12(sp)
2005efa2:	8936                	c.mv	s2,a3
2005efa4:	dd5ff0ef          	jal	ra,2005ed78 <print_unsigned_num>
2005efa8:	842a                	c.mv	s0,a0
2005efaa:	00955d63          	bge	a0,s1,2005efc4 <pad_num_control+0x34>
2005efae:	02000793          	addi	a5,zero,32
2005efb2:	00f91963          	bne	s2,a5,2005efc4 <pad_num_control+0x34>
2005efb6:	02000593          	addi	a1,zero,32
2005efba:	40a48533          	sub	a0,s1,a0
2005efbe:	eddff0ef          	jal	ra,2005ee9a <pad_char_control>
2005efc2:	942a                	c.add	s0,a0
2005efc4:	40b2                	c.lwsp	ra,12(sp)
2005efc6:	8522                	c.mv	a0,s0
2005efc8:	4422                	c.lwsp	s0,8(sp)
2005efca:	4492                	c.lwsp	s1,4(sp)
2005efcc:	4902                	c.lwsp	s2,0(sp)
2005efce:	0141                	c.addi	sp,16
2005efd0:	8082                	c.jr	ra
2005efd2:	da7ff06f          	jal	zero,2005ed78 <print_unsigned_num>

2005efd6 <_rand>:
2005efd6:	1141                	c.addi	sp,-16
2005efd8:	450d                	c.li	a0,3
2005efda:	c606                	c.swsp	ra,12(sp)
2005efdc:	f12fa0ef          	jal	ra,200596ee <PLL_State>
2005efe0:	c50d                	c.beqz	a0,2005f00a <_rand+0x34>
2005efe2:	65c1                	c.lui	a1,0x10
2005efe4:	410167b7          	lui	a5,0x41016
2005efe8:	453d                	c.li	a0,15
2005efea:	4685                	c.li	a3,1
2005efec:	00158713          	addi	a4,a1,1 # 10001 <mbedtls_ecp_check_pub_priv+0x123>
2005eff0:	47d0                	c.lw	a2,12(a5)
2005eff2:	00a61763          	bne	a2,a0,2005f000 <_rand+0x2a>
2005eff6:	df94                	c.sw	a3,56(a5)
2005eff8:	177d                	c.addi	a4,-1
2005effa:	fb7d                	c.bnez	a4,2005eff0 <_rand+0x1a>
2005effc:	df94                	c.sw	a3,56(a5)
2005effe:	b7fd                	c.j	2005efec <_rand+0x16>
2005f000:	de65                	c.beqz	a2,2005eff8 <_rand+0x22>
2005f002:	4b88                	c.lw	a0,16(a5)
2005f004:	40b2                	c.lwsp	ra,12(sp)
2005f006:	0141                	c.addi	sp,16
2005f008:	8082                	c.jr	ra
2005f00a:	fb018693          	addi	a3,gp,-80 # 20066938 <rand_first.1>
2005f00e:	429c                	c.lw	a5,0(a3)
2005f010:	20068737          	lui	a4,0x20068
2005f014:	6b470713          	addi	a4,a4,1716 # 200686b4 <rand_seed.0>
2005f018:	eb91                	c.bnez	a5,2005f02c <_rand+0x56>
2005f01a:	678d                	c.lui	a5,0x3
2005f01c:	03978793          	addi	a5,a5,57 # 3039 <FLASH_SetSpiMode+0x375>
2005f020:	c31c                	c.sw	a5,0(a4)
2005f022:	c35c                	c.sw	a5,4(a4)
2005f024:	c71c                	c.sw	a5,8(a4)
2005f026:	c75c                	c.sw	a5,12(a4)
2005f028:	4785                	c.li	a5,1
2005f02a:	c29c                	c.sw	a5,0(a3)
2005f02c:	4314                	c.lw	a3,0(a4)
2005f02e:	fff80537          	lui	a0,0xfff80
2005f032:	474c                	c.lw	a1,12(a4)
2005f034:	00669793          	slli	a5,a3,0x6
2005f038:	8fb5                	c.xor	a5,a3
2005f03a:	06ca                	c.slli	a3,0x12
2005f03c:	8ee9                	c.and	a3,a0
2005f03e:	83b5                	c.srli	a5,0xd
2005f040:	8fb5                	c.xor	a5,a3
2005f042:	4354                	c.lw	a3,4(a4)
2005f044:	fff00837          	lui	a6,0xfff00
2005f048:	c31c                	c.sw	a5,0(a4)
2005f04a:	00269613          	slli	a2,a3,0x2
2005f04e:	00c6c533          	xor	a0,a3,a2
2005f052:	4714                	c.lw	a3,8(a4)
2005f054:	9a01                	c.andi	a2,-32
2005f056:	816d                	c.srli	a0,0x1b
2005f058:	8d31                	c.xor	a0,a2
2005f05a:	00d69613          	slli	a2,a3,0xd
2005f05e:	8e35                	c.xor	a2,a3
2005f060:	069e                	c.slli	a3,0x7
2005f062:	8006f693          	andi	a3,a3,-2048
2005f066:	8255                	c.srli	a2,0x15
2005f068:	8e35                	c.xor	a2,a3
2005f06a:	00359693          	slli	a3,a1,0x3
2005f06e:	8ead                	c.xor	a3,a1
2005f070:	05b6                	c.slli	a1,0xd
2005f072:	c348                	c.sw	a0,4(a4)
2005f074:	82b1                	c.srli	a3,0xc
2005f076:	0105f5b3          	and	a1,a1,a6
2005f07a:	8d3d                	c.xor	a0,a5
2005f07c:	8ead                	c.xor	a3,a1
2005f07e:	8d31                	c.xor	a0,a2
2005f080:	c710                	c.sw	a2,8(a4)
2005f082:	c754                	c.sw	a3,12(a4)
2005f084:	8d35                	c.xor	a0,a3
2005f086:	bfbd                	c.j	2005f004 <_rand+0x2e>

2005f088 <DiagVprintf>:
2005f088:	7139                	c.addi16sp	sp,-64
2005f08a:	d452                	c.swsp	s4,40(sp)
2005f08c:	20064a37          	lui	s4,0x20064
2005f090:	dc22                	c.swsp	s0,56(sp)
2005f092:	da26                	c.swsp	s1,52(sp)
2005f094:	d84a                	c.swsp	s2,48(sp)
2005f096:	d64e                	c.swsp	s3,44(sp)
2005f098:	d256                	c.swsp	s5,36(sp)
2005f09a:	d05a                	c.swsp	s6,32(sp)
2005f09c:	de06                	c.swsp	ra,60(sp)
2005f09e:	ce5e                	c.swsp	s7,28(sp)
2005f0a0:	cc62                	c.swsp	s8,24(sp)
2005f0a2:	ca66                	c.swsp	s9,20(sp)
2005f0a4:	892a                	c.mv	s2,a0
2005f0a6:	842e                	c.mv	s0,a1
2005f0a8:	4981                	c.li	s3,0
2005f0aa:	4481                	c.li	s1,0
2005f0ac:	ab0a0a13          	addi	s4,s4,-1360 # 20063ab0 <TaskStateString.0+0x18>
2005f0b0:	4aa5                	c.li	s5,9
2005f0b2:	4b29                	c.li	s6,10
2005f0b4:	00094503          	lbu	a0,0(s2)
2005f0b8:	ed19                	c.bnez	a0,2005f0d6 <DiagVprintf+0x4e>
2005f0ba:	50f2                	c.lwsp	ra,60(sp)
2005f0bc:	5462                	c.lwsp	s0,56(sp)
2005f0be:	5942                	c.lwsp	s2,48(sp)
2005f0c0:	59b2                	c.lwsp	s3,44(sp)
2005f0c2:	5a22                	c.lwsp	s4,40(sp)
2005f0c4:	5a92                	c.lwsp	s5,36(sp)
2005f0c6:	5b02                	c.lwsp	s6,32(sp)
2005f0c8:	4bf2                	c.lwsp	s7,28(sp)
2005f0ca:	4c62                	c.lwsp	s8,24(sp)
2005f0cc:	4cd2                	c.lwsp	s9,20(sp)
2005f0ce:	8526                	c.mv	a0,s1
2005f0d0:	54d2                	c.lwsp	s1,52(sp)
2005f0d2:	6121                	c.addi16sp	sp,64
2005f0d4:	8082                	c.jr	ra
2005f0d6:	02500793          	addi	a5,zero,37
2005f0da:	0905                	c.addi	s2,1
2005f0dc:	28f51f63          	bne	a0,a5,2005f37a <DiagVprintf+0x2f2>
2005f0e0:	4881                	c.li	a7,0
2005f0e2:	4701                	c.li	a4,0
2005f0e4:	4681                	c.li	a3,0
2005f0e6:	4601                	c.li	a2,0
2005f0e8:	07a00e13          	addi	t3,zero,122
2005f0ec:	05700e93          	addi	t4,zero,87
2005f0f0:	02200f13          	addi	t5,zero,34
2005f0f4:	03000593          	addi	a1,zero,48
2005f0f8:	04300813          	addi	a6,zero,67
2005f0fc:	04c00f93          	addi	t6,zero,76
2005f100:	03900293          	addi	t0,zero,57
2005f104:	02000393          	addi	t2,zero,32
2005f108:	02500313          	addi	t1,zero,37
2005f10c:	02d00b93          	addi	s7,zero,45
2005f110:	00094783          	lbu	a5,0(s2)
2005f114:	02fe6063          	bltu	t3,a5,2005f134 <DiagVprintf+0xac>
2005f118:	02fee063          	bltu	t4,a5,2005f138 <DiagVprintf+0xb0>
2005f11c:	22b78663          	beq	a5,a1,2005f348 <DiagVprintf+0x2c0>
2005f120:	02f5eb63          	bltu	a1,a5,2005f156 <DiagVprintf+0xce>
2005f124:	20678963          	beq	a5,t1,2005f336 <DiagVprintf+0x2ae>
2005f128:	02f36263          	bltu	t1,a5,2005f14c <DiagVprintf+0xc4>
2005f12c:	20078c63          	beq	a5,zero,2005f344 <DiagVprintf+0x2bc>
2005f130:	24778163          	beq	a5,t2,2005f372 <DiagVprintf+0x2ea>
2005f134:	54fd                	c.li	s1,-1
2005f136:	b751                	c.j	2005f0ba <DiagVprintf+0x32>
2005f138:	fa878793          	addi	a5,a5,-88
2005f13c:	0ff7f793          	andi	a5,a5,255
2005f140:	feff6ae3          	bltu	t5,a5,2005f134 <DiagVprintf+0xac>
2005f144:	078a                	c.slli	a5,0x2
2005f146:	97d2                	c.add	a5,s4
2005f148:	439c                	c.lw	a5,0(a5)
2005f14a:	8782                	c.jr	a5
2005f14c:	ff7794e3          	bne	a5,s7,2005f134 <DiagVprintf+0xac>
2005f150:	0905                	c.addi	s2,1
2005f152:	0885                	c.addi	a7,1
2005f154:	bf75                	c.j	2005f110 <DiagVprintf+0x88>
2005f156:	19078363          	beq	a5,a6,2005f2dc <DiagVprintf+0x254>
2005f15a:	02f86563          	bltu	a6,a5,2005f184 <DiagVprintf+0xfc>
2005f15e:	20f2fb63          	bgeu	t0,a5,2005f374 <DiagVprintf+0x2ec>
2005f162:	04200593          	addi	a1,zero,66
2005f166:	fcb797e3          	bne	a5,a1,2005f134 <DiagVprintf+0xac>
2005f16a:	4785                	c.li	a5,1
2005f16c:	0ac7d063          	bge	a5,a2,2005f20c <DiagVprintf+0x184>
2005f170:	041d                	c.addi	s0,7
2005f172:	9861                	c.andi	s0,-8
2005f174:	4008                	c.lw	a0,0(s0)
2005f176:	404c                	c.lw	a1,4(s0)
2005f178:	00840b93          	addi	s7,s0,8
2005f17c:	4801                	c.li	a6,0
2005f17e:	87ce                	c.mv	a5,s3
2005f180:	4609                	c.li	a2,2
2005f182:	a889                	c.j	2005f1d4 <DiagVprintf+0x14c>
2005f184:	1bf78663          	beq	a5,t6,2005f330 <DiagVprintf+0x2a8>
2005f188:	04f00593          	addi	a1,zero,79
2005f18c:	fab794e3          	bne	a5,a1,2005f134 <DiagVprintf+0xac>
2005f190:	4785                	c.li	a5,1
2005f192:	08c7d263          	bge	a5,a2,2005f216 <DiagVprintf+0x18e>
2005f196:	041d                	c.addi	s0,7
2005f198:	9861                	c.andi	s0,-8
2005f19a:	4008                	c.lw	a0,0(s0)
2005f19c:	404c                	c.lw	a1,4(s0)
2005f19e:	00840b93          	addi	s7,s0,8
2005f1a2:	4801                	c.li	a6,0
2005f1a4:	87ce                	c.mv	a5,s3
2005f1a6:	4621                	c.li	a2,8
2005f1a8:	a035                	c.j	2005f1d4 <DiagVprintf+0x14c>
2005f1aa:	4785                	c.li	a5,1
2005f1ac:	02c7da63          	bge	a5,a2,2005f1e0 <DiagVprintf+0x158>
2005f1b0:	041d                	c.addi	s0,7
2005f1b2:	9861                	c.andi	s0,-8
2005f1b4:	4008                	c.lw	a0,0(s0)
2005f1b6:	404c                	c.lw	a1,4(s0)
2005f1b8:	00840b93          	addi	s7,s0,8
2005f1bc:	0405d163          	bge	a1,zero,2005f1fe <DiagVprintf+0x176>
2005f1c0:	00a037b3          	sltu	a5,zero,a0
2005f1c4:	40b005b3          	sub	a1,zero,a1
2005f1c8:	40a00533          	sub	a0,zero,a0
2005f1cc:	8d9d                	c.sub	a1,a5
2005f1ce:	4805                	c.li	a6,1
2005f1d0:	87ce                	c.mv	a5,s3
2005f1d2:	4629                	c.li	a2,10
2005f1d4:	dbdff0ef          	jal	ra,2005ef90 <pad_num_control>
2005f1d8:	94aa                	c.add	s1,a0
2005f1da:	0905                	c.addi	s2,1
2005f1dc:	845e                	c.mv	s0,s7
2005f1de:	bdd9                	c.j	2005f0b4 <DiagVprintf+0x2c>
2005f1e0:	4008                	c.lw	a0,0(s0)
2005f1e2:	00440b93          	addi	s7,s0,4
2005f1e6:	41f55593          	srai	a1,a0,0x1f
2005f1ea:	bfc9                	c.j	2005f1bc <DiagVprintf+0x134>
2005f1ec:	4785                	c.li	a5,1
2005f1ee:	00c7da63          	bge	a5,a2,2005f202 <DiagVprintf+0x17a>
2005f1f2:	041d                	c.addi	s0,7
2005f1f4:	9861                	c.andi	s0,-8
2005f1f6:	4008                	c.lw	a0,0(s0)
2005f1f8:	404c                	c.lw	a1,4(s0)
2005f1fa:	00840b93          	addi	s7,s0,8
2005f1fe:	4801                	c.li	a6,0
2005f200:	bfc1                	c.j	2005f1d0 <DiagVprintf+0x148>
2005f202:	4008                	c.lw	a0,0(s0)
2005f204:	4581                	c.li	a1,0
2005f206:	00440b93          	addi	s7,s0,4
2005f20a:	bfd5                	c.j	2005f1fe <DiagVprintf+0x176>
2005f20c:	4008                	c.lw	a0,0(s0)
2005f20e:	4581                	c.li	a1,0
2005f210:	00440b93          	addi	s7,s0,4
2005f214:	b7a5                	c.j	2005f17c <DiagVprintf+0xf4>
2005f216:	4008                	c.lw	a0,0(s0)
2005f218:	4581                	c.li	a1,0
2005f21a:	00440b93          	addi	s7,s0,4
2005f21e:	b751                	c.j	2005f1a2 <DiagVprintf+0x11a>
2005f220:	4985                	c.li	s3,1
2005f222:	4785                	c.li	a5,1
2005f224:	00c7dc63          	bge	a5,a2,2005f23c <DiagVprintf+0x1b4>
2005f228:	041d                	c.addi	s0,7
2005f22a:	9861                	c.andi	s0,-8
2005f22c:	4008                	c.lw	a0,0(s0)
2005f22e:	404c                	c.lw	a1,4(s0)
2005f230:	00840b93          	addi	s7,s0,8
2005f234:	4801                	c.li	a6,0
2005f236:	87ce                	c.mv	a5,s3
2005f238:	4641                	c.li	a2,16
2005f23a:	bf69                	c.j	2005f1d4 <DiagVprintf+0x14c>
2005f23c:	4008                	c.lw	a0,0(s0)
2005f23e:	4581                	c.li	a1,0
2005f240:	00440b93          	addi	s7,s0,4
2005f244:	bfc5                	c.j	2005f234 <DiagVprintf+0x1ac>
2005f246:	4008                	c.lw	a0,0(s0)
2005f248:	4801                	c.li	a6,0
2005f24a:	4785                	c.li	a5,1
2005f24c:	4721                	c.li	a4,8
2005f24e:	03000693          	addi	a3,zero,48
2005f252:	4641                	c.li	a2,16
2005f254:	4581                	c.li	a1,0
2005f256:	d3bff0ef          	jal	ra,2005ef90 <pad_num_control>
2005f25a:	00440b93          	addi	s7,s0,4
2005f25e:	94aa                	c.add	s1,a0
2005f260:	4985                	c.li	s3,1
2005f262:	bfa5                	c.j	2005f1da <DiagVprintf+0x152>
2005f264:	00042c83          	lw	s9,0(s0)
2005f268:	00440b93          	addi	s7,s0,4
2005f26c:	4401                	c.li	s0,0
2005f26e:	008c87b3          	add	a5,s9,s0
2005f272:	0007c783          	lbu	a5,0(a5)
2005f276:	e7a9                	c.bnez	a5,2005f2c0 <DiagVprintf+0x238>
2005f278:	02089063          	bne	a7,zero,2005f298 <DiagVprintf+0x210>
2005f27c:	40870533          	sub	a0,a4,s0
2005f280:	00a05c63          	bge	zero,a0,2005f298 <DiagVprintf+0x210>
2005f284:	85b6                	c.mv	a1,a3
2005f286:	c646                	c.swsp	a7,12(sp)
2005f288:	c43a                	c.swsp	a4,8(sp)
2005f28a:	c236                	c.swsp	a3,4(sp)
2005f28c:	c0fff0ef          	jal	ra,2005ee9a <pad_char_control>
2005f290:	48b2                	c.lwsp	a7,12(sp)
2005f292:	4722                	c.lwsp	a4,8(sp)
2005f294:	4692                	c.lwsp	a3,4(sp)
2005f296:	94aa                	c.add	s1,a0
2005f298:	5c7d                	c.li	s8,-1
2005f29a:	000c8863          	beq	s9,zero,2005f2aa <DiagVprintf+0x222>
2005f29e:	4c01                	c.li	s8,0
2005f2a0:	018c87b3          	add	a5,s9,s8
2005f2a4:	0007c503          	lbu	a0,0(a5)
2005f2a8:	ed11                	c.bnez	a0,2005f2c4 <DiagVprintf+0x23c>
2005f2aa:	94e2                	c.add	s1,s8
2005f2ac:	f20887e3          	beq	a7,zero,2005f1da <DiagVprintf+0x152>
2005f2b0:	40870533          	sub	a0,a4,s0
2005f2b4:	85b6                	c.mv	a1,a3
2005f2b6:	f2a052e3          	bge	zero,a0,2005f1da <DiagVprintf+0x152>
2005f2ba:	be1ff0ef          	jal	ra,2005ee9a <pad_char_control>
2005f2be:	bf29                	c.j	2005f1d8 <DiagVprintf+0x150>
2005f2c0:	0405                	c.addi	s0,1
2005f2c2:	b775                	c.j	2005f26e <DiagVprintf+0x1e6>
2005f2c4:	c646                	c.swsp	a7,12(sp)
2005f2c6:	c43a                	c.swsp	a4,8(sp)
2005f2c8:	c236                	c.swsp	a3,4(sp)
2005f2ca:	dffa5097          	auipc	ra,0xdffa5
2005f2ce:	27a080e7          	jalr	ra,634(ra) # 4544 <LOGUART_PutChar>
2005f2d2:	48b2                	c.lwsp	a7,12(sp)
2005f2d4:	4722                	c.lwsp	a4,8(sp)
2005f2d6:	4692                	c.lwsp	a3,4(sp)
2005f2d8:	0c05                	c.addi	s8,1
2005f2da:	b7d9                	c.j	2005f2a0 <DiagVprintf+0x218>
2005f2dc:	00440b93          	addi	s7,s0,4
2005f2e0:	00044403          	lbu	s0,0(s0)
2005f2e4:	02089163          	bne	a7,zero,2005f306 <DiagVprintf+0x27e>
2005f2e8:	4785                	c.li	a5,1
2005f2ea:	00e7de63          	bge	a5,a4,2005f306 <DiagVprintf+0x27e>
2005f2ee:	85b6                	c.mv	a1,a3
2005f2f0:	fff70513          	addi	a0,a4,-1
2005f2f4:	c646                	c.swsp	a7,12(sp)
2005f2f6:	c436                	c.swsp	a3,8(sp)
2005f2f8:	c23a                	c.swsp	a4,4(sp)
2005f2fa:	ba1ff0ef          	jal	ra,2005ee9a <pad_char_control>
2005f2fe:	48b2                	c.lwsp	a7,12(sp)
2005f300:	46a2                	c.lwsp	a3,8(sp)
2005f302:	4712                	c.lwsp	a4,4(sp)
2005f304:	94aa                	c.add	s1,a0
2005f306:	8522                	c.mv	a0,s0
2005f308:	c646                	c.swsp	a7,12(sp)
2005f30a:	c43a                	c.swsp	a4,8(sp)
2005f30c:	c236                	c.swsp	a3,4(sp)
2005f30e:	dffa5097          	auipc	ra,0xdffa5
2005f312:	236080e7          	jalr	ra,566(ra) # 4544 <LOGUART_PutChar>
2005f316:	48b2                	c.lwsp	a7,12(sp)
2005f318:	0485                	c.addi	s1,1
2005f31a:	ec0880e3          	beq	a7,zero,2005f1da <DiagVprintf+0x152>
2005f31e:	4722                	c.lwsp	a4,8(sp)
2005f320:	4785                	c.li	a5,1
2005f322:	eae7dce3          	bge	a5,a4,2005f1da <DiagVprintf+0x152>
2005f326:	4692                	c.lwsp	a3,4(sp)
2005f328:	fff70513          	addi	a0,a4,-1
2005f32c:	85b6                	c.mv	a1,a3
2005f32e:	b771                	c.j	2005f2ba <DiagVprintf+0x232>
2005f330:	0605                	c.addi	a2,1
2005f332:	0905                	c.addi	s2,1
2005f334:	bbf1                	c.j	2005f110 <DiagVprintf+0x88>
2005f336:	02500513          	addi	a0,zero,37
2005f33a:	dffa5097          	auipc	ra,0xdffa5
2005f33e:	20a080e7          	jalr	ra,522(ra) # 4544 <LOGUART_PutChar>
2005f342:	0485                	c.addi	s1,1
2005f344:	8ba2                	c.mv	s7,s0
2005f346:	bd51                	c.j	2005f1da <DiagVprintf+0x152>
2005f348:	0905                	c.addi	s2,1
2005f34a:	03000693          	addi	a3,zero,48
2005f34e:	4701                	c.li	a4,0
2005f350:	00094783          	lbu	a5,0(s2)
2005f354:	d60783e3          	beq	a5,zero,2005f0ba <DiagVprintf+0x32>
2005f358:	fd078513          	addi	a0,a5,-48
2005f35c:	0ff57513          	andi	a0,a0,255
2005f360:	daaae8e3          	bltu	s5,a0,2005f110 <DiagVprintf+0x88>
2005f364:	03670733          	mul	a4,a4,s6
2005f368:	0905                	c.addi	s2,1
2005f36a:	fd070713          	addi	a4,a4,-48
2005f36e:	973e                	c.add	a4,a5
2005f370:	b7c5                	c.j	2005f350 <DiagVprintf+0x2c8>
2005f372:	0905                	c.addi	s2,1
2005f374:	02000693          	addi	a3,zero,32
2005f378:	bfd9                	c.j	2005f34e <DiagVprintf+0x2c6>
2005f37a:	dffa5097          	auipc	ra,0xdffa5
2005f37e:	1ca080e7          	jalr	ra,458(ra) # 4544 <LOGUART_PutChar>
2005f382:	0485                	c.addi	s1,1
2005f384:	bb05                	c.j	2005f0b4 <DiagVprintf+0x2c>

2005f386 <DiagPrintf>:
2005f386:	7139                	c.addi16sp	sp,-64
2005f388:	d22e                	c.swsp	a1,36(sp)
2005f38a:	104c                	c.addi4spn	a1,sp,36
2005f38c:	ce06                	c.swsp	ra,28(sp)
2005f38e:	d432                	c.swsp	a2,40(sp)
2005f390:	d636                	c.swsp	a3,44(sp)
2005f392:	d83a                	c.swsp	a4,48(sp)
2005f394:	da3e                	c.swsp	a5,52(sp)
2005f396:	dc42                	c.swsp	a6,56(sp)
2005f398:	de46                	c.swsp	a7,60(sp)
2005f39a:	c62e                	c.swsp	a1,12(sp)
2005f39c:	cedff0ef          	jal	ra,2005f088 <DiagVprintf>
2005f3a0:	40f2                	c.lwsp	ra,28(sp)
2005f3a2:	6121                	c.addi16sp	sp,64
2005f3a4:	8082                	c.jr	ra

2005f3a6 <DiagPrintf_minimal>:
2005f3a6:	711d                	c.addi16sp	sp,-96
2005f3a8:	da26                	c.swsp	s1,52(sp)
2005f3aa:	00c4                	c.addi4spn	s1,sp,68
2005f3ac:	dc22                	c.swsp	s0,56(sp)
2005f3ae:	d84a                	c.swsp	s2,48(sp)
2005f3b0:	d64e                	c.swsp	s3,44(sp)
2005f3b2:	d452                	c.swsp	s4,40(sp)
2005f3b4:	d256                	c.swsp	s5,36(sp)
2005f3b6:	de06                	c.swsp	ra,60(sp)
2005f3b8:	d05a                	c.swsp	s6,32(sp)
2005f3ba:	ce5e                	c.swsp	s7,28(sp)
2005f3bc:	892a                	c.mv	s2,a0
2005f3be:	c2ae                	c.swsp	a1,68(sp)
2005f3c0:	c4b2                	c.swsp	a2,72(sp)
2005f3c2:	c6b6                	c.swsp	a3,76(sp)
2005f3c4:	c8ba                	c.swsp	a4,80(sp)
2005f3c6:	cabe                	c.swsp	a5,84(sp)
2005f3c8:	ccc2                	c.swsp	a6,88(sp)
2005f3ca:	cec6                	c.swsp	a7,92(sp)
2005f3cc:	c626                	c.swsp	s1,12(sp)
2005f3ce:	4401                	c.li	s0,0
2005f3d0:	03000993          	addi	s3,zero,48
2005f3d4:	4a25                	c.li	s4,9
2005f3d6:	4aa9                	c.li	s5,10
2005f3d8:	00094503          	lbu	a0,0(s2)
2005f3dc:	cd1d                	c.beqz	a0,2005f41a <DiagPrintf_minimal+0x74>
2005f3de:	02500793          	addi	a5,zero,37
2005f3e2:	0905                	c.addi	s2,1
2005f3e4:	16f51a63          	bne	a0,a5,2005f558 <DiagPrintf_minimal+0x1b2>
2005f3e8:	4681                	c.li	a3,0
2005f3ea:	4601                	c.li	a2,0
2005f3ec:	06300893          	addi	a7,zero,99
2005f3f0:	03900593          	addi	a1,zero,57
2005f3f4:	02000313          	addi	t1,zero,32
2005f3f8:	00094783          	lbu	a5,0(s2)
2005f3fc:	05178163          	beq	a5,a7,2005f43e <DiagPrintf_minimal+0x98>
2005f400:	06f8e363          	bltu	a7,a5,2005f466 <DiagPrintf_minimal+0xc0>
2005f404:	13378063          	beq	a5,s3,2005f524 <DiagPrintf_minimal+0x17e>
2005f408:	02f9e563          	bltu	s3,a5,2005f432 <DiagPrintf_minimal+0x8c>
2005f40c:	14678263          	beq	a5,t1,2005f550 <DiagPrintf_minimal+0x1aa>
2005f410:	02500713          	addi	a4,zero,37
2005f414:	10e78063          	beq	a5,a4,2005f514 <DiagPrintf_minimal+0x16e>
2005f418:	547d                	c.li	s0,-1
2005f41a:	50f2                	c.lwsp	ra,60(sp)
2005f41c:	8522                	c.mv	a0,s0
2005f41e:	5462                	c.lwsp	s0,56(sp)
2005f420:	54d2                	c.lwsp	s1,52(sp)
2005f422:	5942                	c.lwsp	s2,48(sp)
2005f424:	59b2                	c.lwsp	s3,44(sp)
2005f426:	5a22                	c.lwsp	s4,40(sp)
2005f428:	5a92                	c.lwsp	s5,36(sp)
2005f42a:	5b02                	c.lwsp	s6,32(sp)
2005f42c:	4bf2                	c.lwsp	s7,28(sp)
2005f42e:	6125                	c.addi16sp	sp,96
2005f430:	8082                	c.jr	ra
2005f432:	12f5f063          	bgeu	a1,a5,2005f552 <DiagPrintf_minimal+0x1ac>
2005f436:	04300713          	addi	a4,zero,67
2005f43a:	fce79fe3          	bne	a5,a4,2005f418 <DiagPrintf_minimal+0x72>
2005f43e:	4785                	c.li	a5,1
2005f440:	00448b13          	addi	s6,s1,4
2005f444:	0004c483          	lbu	s1,0(s1)
2005f448:	00d7d863          	bge	a5,a3,2005f458 <DiagPrintf_minimal+0xb2>
2005f44c:	85b2                	c.mv	a1,a2
2005f44e:	fff68513          	addi	a0,a3,-1
2005f452:	a49ff0ef          	jal	ra,2005ee9a <pad_char_control>
2005f456:	942a                	c.add	s0,a0
2005f458:	8526                	c.mv	a0,s1
2005f45a:	dffa5097          	auipc	ra,0xdffa5
2005f45e:	0ea080e7          	jalr	ra,234(ra) # 4544 <LOGUART_PutChar>
2005f462:	0405                	c.addi	s0,1
2005f464:	a825                	c.j	2005f49c <DiagPrintf_minimal+0xf6>
2005f466:	07300713          	addi	a4,zero,115
2005f46a:	04e78e63          	beq	a5,a4,2005f4c6 <DiagPrintf_minimal+0x120>
2005f46e:	02f76963          	bltu	a4,a5,2005f4a0 <DiagPrintf_minimal+0xfa>
2005f472:	06400713          	addi	a4,zero,100
2005f476:	00e78663          	beq	a5,a4,2005f482 <DiagPrintf_minimal+0xdc>
2005f47a:	06900713          	addi	a4,zero,105
2005f47e:	f8e79de3          	bne	a5,a4,2005f418 <DiagPrintf_minimal+0x72>
2005f482:	4088                	c.lw	a0,0(s1)
2005f484:	00448b13          	addi	s6,s1,4
2005f488:	4701                	c.li	a4,0
2005f48a:	00055563          	bge	a0,zero,2005f494 <DiagPrintf_minimal+0xee>
2005f48e:	40a00533          	sub	a0,zero,a0
2005f492:	4705                	c.li	a4,1
2005f494:	45a9                	c.li	a1,10
2005f496:	a3bff0ef          	jal	ra,2005eed0 <print_decimal_num>
2005f49a:	942a                	c.add	s0,a0
2005f49c:	84da                	c.mv	s1,s6
2005f49e:	a08d                	c.j	2005f500 <DiagPrintf_minimal+0x15a>
2005f4a0:	07500713          	addi	a4,zero,117
2005f4a4:	00e78b63          	beq	a5,a4,2005f4ba <DiagPrintf_minimal+0x114>
2005f4a8:	07800713          	addi	a4,zero,120
2005f4ac:	f6e796e3          	bne	a5,a4,2005f418 <DiagPrintf_minimal+0x72>
2005f4b0:	00448b13          	addi	s6,s1,4
2005f4b4:	4701                	c.li	a4,0
2005f4b6:	45c1                	c.li	a1,16
2005f4b8:	a029                	c.j	2005f4c2 <DiagPrintf_minimal+0x11c>
2005f4ba:	00448b13          	addi	s6,s1,4
2005f4be:	4701                	c.li	a4,0
2005f4c0:	45a9                	c.li	a1,10
2005f4c2:	4088                	c.lw	a0,0(s1)
2005f4c4:	bfc9                	c.j	2005f496 <DiagPrintf_minimal+0xf0>
2005f4c6:	0004ab03          	lw	s6,0(s1)
2005f4ca:	00448b93          	addi	s7,s1,4
2005f4ce:	4781                	c.li	a5,0
2005f4d0:	00fb0733          	add	a4,s6,a5
2005f4d4:	00074703          	lbu	a4,0(a4)
2005f4d8:	e715                	c.bnez	a4,2005f504 <DiagPrintf_minimal+0x15e>
2005f4da:	40f68533          	sub	a0,a3,a5
2005f4de:	00a05663          	bge	zero,a0,2005f4ea <DiagPrintf_minimal+0x144>
2005f4e2:	85b2                	c.mv	a1,a2
2005f4e4:	9b7ff0ef          	jal	ra,2005ee9a <pad_char_control>
2005f4e8:	942a                	c.add	s0,a0
2005f4ea:	54fd                	c.li	s1,-1
2005f4ec:	000b0863          	beq	s6,zero,2005f4fc <DiagPrintf_minimal+0x156>
2005f4f0:	4481                	c.li	s1,0
2005f4f2:	009b07b3          	add	a5,s6,s1
2005f4f6:	0007c503          	lbu	a0,0(a5)
2005f4fa:	e519                	c.bnez	a0,2005f508 <DiagPrintf_minimal+0x162>
2005f4fc:	9426                	c.add	s0,s1
2005f4fe:	84de                	c.mv	s1,s7
2005f500:	0905                	c.addi	s2,1
2005f502:	bdd9                	c.j	2005f3d8 <DiagPrintf_minimal+0x32>
2005f504:	0785                	c.addi	a5,1
2005f506:	b7e9                	c.j	2005f4d0 <DiagPrintf_minimal+0x12a>
2005f508:	dffa5097          	auipc	ra,0xdffa5
2005f50c:	03c080e7          	jalr	ra,60(ra) # 4544 <LOGUART_PutChar>
2005f510:	0485                	c.addi	s1,1
2005f512:	b7c5                	c.j	2005f4f2 <DiagPrintf_minimal+0x14c>
2005f514:	02500513          	addi	a0,zero,37
2005f518:	dffa5097          	auipc	ra,0xdffa5
2005f51c:	02c080e7          	jalr	ra,44(ra) # 4544 <LOGUART_PutChar>
2005f520:	0405                	c.addi	s0,1
2005f522:	bff9                	c.j	2005f500 <DiagPrintf_minimal+0x15a>
2005f524:	0905                	c.addi	s2,1
2005f526:	03000613          	addi	a2,zero,48
2005f52a:	4681                	c.li	a3,0
2005f52c:	00094703          	lbu	a4,0(s2)
2005f530:	ee0705e3          	beq	a4,zero,2005f41a <DiagPrintf_minimal+0x74>
2005f534:	fd070813          	addi	a6,a4,-48
2005f538:	0ff87813          	andi	a6,a6,255
2005f53c:	eb0a6ee3          	bltu	s4,a6,2005f3f8 <DiagPrintf_minimal+0x52>
2005f540:	03568533          	mul	a0,a3,s5
2005f544:	0905                	c.addi	s2,1
2005f546:	fd050513          	addi	a0,a0,-48 # fff7ffd0 <__ctrace_end__+0x35f7ffd0>
2005f54a:	00a706b3          	add	a3,a4,a0
2005f54e:	bff9                	c.j	2005f52c <DiagPrintf_minimal+0x186>
2005f550:	0905                	c.addi	s2,1
2005f552:	02000613          	addi	a2,zero,32
2005f556:	bfd1                	c.j	2005f52a <DiagPrintf_minimal+0x184>
2005f558:	dffa5097          	auipc	ra,0xdffa5
2005f55c:	fec080e7          	jalr	ra,-20(ra) # 4544 <LOGUART_PutChar>
2005f560:	0405                	c.addi	s0,1
2005f562:	bd9d                	c.j	2005f3d8 <DiagPrintf_minimal+0x32>

2005f564 <DiagSnPrintf>:
2005f564:	7159                	c.addi16sp	sp,-112
2005f566:	c6a2                	c.swsp	s0,76(sp)
2005f568:	c4a6                	c.swsp	s1,72(sp)
2005f56a:	c2ca                	c.swsp	s2,68(sp)
2005f56c:	c0ce                	c.swsp	s3,64(sp)
2005f56e:	de52                	c.swsp	s4,60(sp)
2005f570:	dc56                	c.swsp	s5,56(sp)
2005f572:	da5a                	c.swsp	s6,52(sp)
2005f574:	d85e                	c.swsp	s7,48(sp)
2005f576:	d662                	c.swsp	s8,44(sp)
2005f578:	d466                	c.swsp	s9,40(sp)
2005f57a:	d26a                	c.swsp	s10,36(sp)
2005f57c:	d06e                	c.swsp	s11,32(sp)
2005f57e:	ceb6                	c.swsp	a3,92(sp)
2005f580:	d0ba                	c.swsp	a4,96(sp)
2005f582:	d2be                	c.swsp	a5,100(sp)
2005f584:	d4c2                	c.swsp	a6,104(sp)
2005f586:	d6c6                	c.swsp	a7,108(sp)
2005f588:	28050463          	beq	a0,zero,2005f810 <DiagSnPrintf+0x2ac>
2005f58c:	08fc                	c.addi4spn	a5,sp,92
2005f58e:	c43e                	c.swsp	a5,8(sp)
2005f590:	537d                	c.li	t1,-1
2005f592:	c199                	c.beqz	a1,2005f598 <DiagSnPrintf+0x34>
2005f594:	00b50333          	add	t1,a0,a1
2005f598:	200643b7          	lui	t2,0x20064
2005f59c:	6405                	c.lui	s0,0x1
2005f59e:	882a                	c.mv	a6,a0
2005f5a0:	05000f13          	addi	t5,zero,80
2005f5a4:	06400913          	addi	s2,zero,100
2005f5a8:	06300993          	addi	s3,zero,99
2005f5ac:	4fa9                	c.li	t6,10
2005f5ae:	03000293          	addi	t0,zero,48
2005f5b2:	02d00a13          	addi	s4,zero,45
2005f5b6:	b3c38393          	addi	t2,t2,-1220 # 20063b3c <TaskStateString.0+0xa4>
2005f5ba:	4aa5                	c.li	s5,9
2005f5bc:	0ef00b13          	addi	s6,zero,239
2005f5c0:	eff40b93          	addi	s7,s0,-257 # eff <CPU_ClkGet+0x3e9>
2005f5c4:	6c3d                	c.lui	s8,0xf
2005f5c6:	7cc1                	c.lui	s9,0xffff0
2005f5c8:	00064783          	lbu	a5,0(a2)
2005f5cc:	e399                	c.bnez	a5,2005f5d2 <DiagSnPrintf+0x6e>
2005f5ce:	8742                	c.mv	a4,a6
2005f5d0:	a889                	c.j	2005f622 <DiagSnPrintf+0xbe>
2005f5d2:	02500713          	addi	a4,zero,37
2005f5d6:	00e78b63          	beq	a5,a4,2005f5ec <DiagSnPrintf+0x88>
2005f5da:	00180713          	addi	a4,a6,1 # fff00001 <__ctrace_end__+0x35f00001>
2005f5de:	00f80023          	sb	a5,0(a6)
2005f5e2:	04677063          	bgeu	a4,t1,2005f622 <DiagSnPrintf+0xbe>
2005f5e6:	0605                	c.addi	a2,1
2005f5e8:	883a                	c.mv	a6,a4
2005f5ea:	bff9                	c.j	2005f5c8 <DiagSnPrintf+0x64>
2005f5ec:	4722                	c.lwsp	a4,8(sp)
2005f5ee:	00164e03          	lbu	t3,1(a2)
2005f5f2:	07300693          	addi	a3,zero,115
2005f5f6:	431c                	c.lw	a5,0(a4)
2005f5f8:	00160593          	addi	a1,a2,1
2005f5fc:	0711                	c.addi	a4,4
2005f5fe:	04de1463          	bne	t3,a3,2005f646 <DiagSnPrintf+0xe2>
2005f602:	c43a                	c.swsp	a4,8(sp)
2005f604:	410787b3          	sub	a5,a5,a6
2005f608:	8742                	c.mv	a4,a6
2005f60a:	00f706b3          	add	a3,a4,a5
2005f60e:	0006c683          	lbu	a3,0(a3)
2005f612:	e299                	c.bnez	a3,2005f618 <DiagSnPrintf+0xb4>
2005f614:	862e                	c.mv	a2,a1
2005f616:	bfc1                	c.j	2005f5e6 <DiagSnPrintf+0x82>
2005f618:	0705                	c.addi	a4,1
2005f61a:	fed70fa3          	sb	a3,-1(a4)
2005f61e:	fe6766e3          	bltu	a4,t1,2005f60a <DiagSnPrintf+0xa6>
2005f622:	00070023          	sb	zero,0(a4)
2005f626:	40a70533          	sub	a0,a4,a0
2005f62a:	4436                	c.lwsp	s0,76(sp)
2005f62c:	44a6                	c.lwsp	s1,72(sp)
2005f62e:	4916                	c.lwsp	s2,68(sp)
2005f630:	4986                	c.lwsp	s3,64(sp)
2005f632:	5a72                	c.lwsp	s4,60(sp)
2005f634:	5ae2                	c.lwsp	s5,56(sp)
2005f636:	5b52                	c.lwsp	s6,52(sp)
2005f638:	5bc2                	c.lwsp	s7,48(sp)
2005f63a:	5c32                	c.lwsp	s8,44(sp)
2005f63c:	5ca2                	c.lwsp	s9,40(sp)
2005f63e:	5d12                	c.lwsp	s10,36(sp)
2005f640:	5d82                	c.lwsp	s11,32(sp)
2005f642:	6165                	c.addi16sp	sp,112
2005f644:	8082                	c.jr	ra
2005f646:	c43a                	c.swsp	a4,8(sp)
2005f648:	0dfe7693          	andi	a3,t3,223
2005f64c:	473d                	c.li	a4,15
2005f64e:	0de69563          	bne	a3,t5,2005f718 <DiagSnPrintf+0x1b4>
2005f652:	0cf75a63          	bge	a4,a5,2005f726 <DiagSnPrintf+0x1c2>
2005f656:	46a1                	c.li	a3,8
2005f658:	03000893          	addi	a7,zero,48
2005f65c:	ff078e93          	addi	t4,a5,-16
2005f660:	4711                	c.li	a4,4
2005f662:	05db7463          	bgeu	s6,t4,2005f6aa <DiagSnPrintf+0x146>
2005f666:	f0078e93          	addi	t4,a5,-256
2005f66a:	4721                	c.li	a4,8
2005f66c:	03dbff63          	bgeu	s7,t4,2005f6aa <DiagSnPrintf+0x146>
2005f670:	40878eb3          	sub	t4,a5,s0
2005f674:	4731                	c.li	a4,12
2005f676:	038eea63          	bltu	t4,s8,2005f6aa <DiagSnPrintf+0x146>
2005f67a:	01978eb3          	add	t4,a5,s9
2005f67e:	000f04b7          	lui	s1,0xf0
2005f682:	4741                	c.li	a4,16
2005f684:	029ee363          	bltu	t4,s1,2005f6aa <DiagSnPrintf+0x146>
2005f688:	fff00eb7          	lui	t4,0xfff00
2005f68c:	9ebe                	c.add	t4,a5
2005f68e:	00f004b7          	lui	s1,0xf00
2005f692:	4751                	c.li	a4,20
2005f694:	009eeb63          	bltu	t4,s1,2005f6aa <DiagSnPrintf+0x146>
2005f698:	ff000eb7          	lui	t4,0xff000
2005f69c:	9ebe                	c.add	t4,a5
2005f69e:	0f0004b7          	lui	s1,0xf000
2005f6a2:	4761                	c.li	a4,24
2005f6a4:	009ee363          	bltu	t4,s1,2005f6aa <DiagSnPrintf+0x146>
2005f6a8:	4771                	c.li	a4,28
2005f6aa:	005e1663          	bne	t3,t0,2005f6b6 <DiagSnPrintf+0x152>
2005f6ae:	00260593          	addi	a1,a2,2
2005f6b2:	03000893          	addi	a7,zero,48
2005f6b6:	0005c603          	lbu	a2,0(a1)
2005f6ba:	44a1                	c.li	s1,8
2005f6bc:	fcf60e13          	addi	t3,a2,-49
2005f6c0:	0ffe7e13          	andi	t3,t3,255
2005f6c4:	03c4e163          	bltu	s1,t3,2005f6e6 <DiagSnPrintf+0x182>
2005f6c8:	4681                	c.li	a3,0
2005f6ca:	03f686b3          	mul	a3,a3,t6
2005f6ce:	0585                	c.addi	a1,1
2005f6d0:	96b2                	c.add	a3,a2
2005f6d2:	0005c603          	lbu	a2,0(a1)
2005f6d6:	fd068693          	addi	a3,a3,-48
2005f6da:	fd060e13          	addi	t3,a2,-48
2005f6de:	0ffe7e13          	andi	t3,t3,255
2005f6e2:	ffcaf4e3          	bgeu	s5,t3,2005f6ca <DiagSnPrintf+0x166>
2005f6e6:	0005c603          	lbu	a2,0(a1)
2005f6ea:	0d767e13          	andi	t3,a2,215
2005f6ee:	0bee1463          	bne	t3,t5,2005f796 <DiagSnPrintf+0x232>
2005f6f2:	02067493          	andi	s1,a2,32
2005f6f6:	0df67613          	andi	a2,a2,223
2005f6fa:	8ebe                	c.mv	t4,a5
2005f6fc:	00c10e13          	addi	t3,sp,12
2005f700:	01e61a63          	bne	a2,t5,2005f714 <DiagSnPrintf+0x1b0>
2005f704:	0584e613          	ori	a2,s1,88
2005f708:	00510623          	sb	t0,12(sp)
2005f70c:	00c106a3          	sb	a2,13(sp)
2005f710:	00e10e13          	addi	t3,sp,14
2005f714:	4601                	c.li	a2,0
2005f716:	a831                	c.j	2005f732 <DiagSnPrintf+0x1ce>
2005f718:	4681                	c.li	a3,0
2005f71a:	02000893          	addi	a7,zero,32
2005f71e:	f2f74fe3          	blt	a4,a5,2005f65c <DiagSnPrintf+0xf8>
2005f722:	4701                	c.li	a4,0
2005f724:	b759                	c.j	2005f6aa <DiagSnPrintf+0x146>
2005f726:	4701                	c.li	a4,0
2005f728:	46a1                	c.li	a3,8
2005f72a:	b761                	c.j	2005f6b2 <DiagSnPrintf+0x14e>
2005f72c:	0605                	c.addi	a2,1
2005f72e:	004ede93          	srli	t4,t4,0x4
2005f732:	fe0e9de3          	bne	t4,zero,2005f72c <DiagSnPrintf+0x1c8>
2005f736:	8ef2                	c.mv	t4,t3
2005f738:	00de0db3          	add	s11,t3,a3
2005f73c:	41dd8d33          	sub	s10,s11,t4
2005f740:	05a64763          	blt	a2,s10,2005f78e <DiagSnPrintf+0x22a>
2005f744:	4881                	c.li	a7,0
2005f746:	00c6c463          	blt	a3,a2,2005f74e <DiagSnPrintf+0x1ea>
2005f74a:	40c688b3          	sub	a7,a3,a2
2005f74e:	9e46                	c.add	t3,a7
2005f750:	88f2                	c.mv	a7,t3
2005f752:	863a                	c.mv	a2,a4
2005f754:	40c7d6b3          	sra	a3,a5,a2
2005f758:	8abd                	c.andi	a3,15
2005f75a:	969e                	c.add	a3,t2
2005f75c:	0006c683          	lbu	a3,0(a3)
2005f760:	1671                	c.addi	a2,-4
2005f762:	0885                	c.addi	a7,1
2005f764:	8ec5                	c.or	a3,s1
2005f766:	fed88fa3          	sb	a3,-1(a7)
2005f76a:	fe0655e3          	bge	a2,zero,2005f754 <DiagSnPrintf+0x1f0>
2005f76e:	8309                	c.srli	a4,0x2
2005f770:	0705                	c.addi	a4,1
2005f772:	9e3a                	c.add	t3,a4
2005f774:	8742                	c.mv	a4,a6
2005f776:	007c                	c.addi4spn	a5,sp,12
2005f778:	e9c7fee3          	bgeu	a5,t3,2005f614 <DiagSnPrintf+0xb0>
2005f77c:	0007c683          	lbu	a3,0(a5)
2005f780:	0705                	c.addi	a4,1
2005f782:	fed70fa3          	sb	a3,-1(a4)
2005f786:	e8677ee3          	bgeu	a4,t1,2005f622 <DiagSnPrintf+0xbe>
2005f78a:	0785                	c.addi	a5,1
2005f78c:	b7f5                	c.j	2005f778 <DiagSnPrintf+0x214>
2005f78e:	011e8023          	sb	a7,0(t4) # ff000000 <__ctrace_end__+0x35000000>
2005f792:	0e85                	c.addi	t4,1
2005f794:	b765                	c.j	2005f73c <DiagSnPrintf+0x1d8>
2005f796:	07261363          	bne	a2,s2,2005f7fc <DiagSnPrintf+0x298>
2005f79a:	0407de63          	bge	a5,zero,2005f7f6 <DiagSnPrintf+0x292>
2005f79e:	01410623          	sb	s4,12(sp)
2005f7a2:	40f007b3          	sub	a5,zero,a5
2005f7a6:	4885                	c.li	a7,1
2005f7a8:	00d10693          	addi	a3,sp,13
2005f7ac:	8e36                	c.mv	t3,a3
2005f7ae:	03f7e633          	rem	a2,a5,t6
2005f7b2:	0e05                	c.addi	t3,1
2005f7b4:	03f7c7b3          	div	a5,a5,t6
2005f7b8:	03060613          	addi	a2,a2,48
2005f7bc:	fece0fa3          	sb	a2,-1(t3)
2005f7c0:	011e0633          	add	a2,t3,a7
2005f7c4:	8e15                	c.sub	a2,a3
2005f7c6:	f7e5                	c.bnez	a5,2005f7ae <DiagSnPrintf+0x24a>
2005f7c8:	00c04563          	blt	zero,a2,2005f7d2 <DiagSnPrintf+0x26e>
2005f7cc:	005e0023          	sb	t0,0(t3)
2005f7d0:	0e05                	c.addi	t3,1
2005f7d2:	1771                	c.addi	a4,-4
2005f7d4:	167d                	c.addi	a2,-1
2005f7d6:	fe0759e3          	bge	a4,zero,2005f7c8 <DiagSnPrintf+0x264>
2005f7da:	87f2                	c.mv	a5,t3
2005f7dc:	17fd                	c.addi	a5,-1
2005f7de:	f8f6fbe3          	bgeu	a3,a5,2005f774 <DiagSnPrintf+0x210>
2005f7e2:	0006c603          	lbu	a2,0(a3)
2005f7e6:	0007c703          	lbu	a4,0(a5)
2005f7ea:	0685                	c.addi	a3,1
2005f7ec:	00c78023          	sb	a2,0(a5)
2005f7f0:	fee68fa3          	sb	a4,-1(a3)
2005f7f4:	b7e5                	c.j	2005f7dc <DiagSnPrintf+0x278>
2005f7f6:	4881                	c.li	a7,0
2005f7f8:	0074                	c.addi4spn	a3,sp,12
2005f7fa:	bf4d                	c.j	2005f7ac <DiagSnPrintf+0x248>
2005f7fc:	01361763          	bne	a2,s3,2005f80a <DiagSnPrintf+0x2a6>
2005f800:	00f10623          	sb	a5,12(sp)
2005f804:	00d10e13          	addi	t3,sp,13
2005f808:	b7b5                	c.j	2005f774 <DiagSnPrintf+0x210>
2005f80a:	00c10623          	sb	a2,12(sp)
2005f80e:	bfdd                	c.j	2005f804 <DiagSnPrintf+0x2a0>
2005f810:	4501                	c.li	a0,0
2005f812:	bd21                	c.j	2005f62a <DiagSnPrintf+0xc6>

2005f814 <TRNG_get_random_bytes>:
2005f814:	7179                	c.addi16sp	sp,-48
2005f816:	d422                	c.swsp	s0,40(sp)
2005f818:	d226                	c.swsp	s1,36(sp)
2005f81a:	d606                	c.swsp	ra,44(sp)
2005f81c:	d04a                	c.swsp	s2,32(sp)
2005f81e:	ce4e                	c.swsp	s3,28(sp)
2005f820:	4100c7b7          	lui	a5,0x4100c
2005f824:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005f828:	84aa                	c.mv	s1,a0
2005f82a:	842e                	c.mv	s0,a1
2005f82c:	8399                	c.srli	a5,0x6
2005f82e:	8b8d                	c.andi	a5,3
2005f830:	ef9d                	c.bnez	a5,2005f86e <TRNG_get_random_bytes+0x5a>
2005f832:	00357793          	andi	a5,a0,3
2005f836:	cf85                	c.beqz	a5,2005f86e <TRNG_get_random_bytes+0x5a>
2005f838:	852e                	c.mv	a0,a1
2005f83a:	ebbfb0ef          	jal	ra,2005b6f4 <rtos_mem_malloc>
2005f83e:	85a2                	c.mv	a1,s0
2005f840:	892a                	c.mv	s2,a0
2005f842:	dffa9097          	auipc	ra,0xdffa9
2005f846:	528080e7          	jalr	ra,1320(ra) # 8d6a <RandBytes_Get>
2005f84a:	8622                	c.mv	a2,s0
2005f84c:	85ca                	c.mv	a1,s2
2005f84e:	8526                	c.mv	a0,s1
2005f850:	dffaa097          	auipc	ra,0xdffaa
2005f854:	280080e7          	jalr	ra,640(ra) # 9ad0 <__wrap_memcpy>
2005f858:	854a                	c.mv	a0,s2
2005f85a:	ec5fb0ef          	jal	ra,2005b71e <rtos_mem_free>
2005f85e:	50b2                	c.lwsp	ra,44(sp)
2005f860:	5422                	c.lwsp	s0,40(sp)
2005f862:	5492                	c.lwsp	s1,36(sp)
2005f864:	5902                	c.lwsp	s2,32(sp)
2005f866:	49f2                	c.lwsp	s3,28(sp)
2005f868:	4501                	c.li	a0,0
2005f86a:	6145                	c.addi16sp	sp,48
2005f86c:	8082                	c.jr	ra
2005f86e:	00245913          	srli	s2,s0,0x2
2005f872:	4981                	c.li	s3,0
2005f874:	0329c463          	blt	s3,s2,2005f89c <TRNG_get_random_bytes+0x88>
2005f878:	5671                	c.li	a2,-4
2005f87a:	02c90633          	mul	a2,s2,a2
2005f87e:	9432                	c.add	s0,a2
2005f880:	dc79                	c.beqz	s0,2005f85e <TRNG_get_random_bytes+0x4a>
2005f882:	f54ff0ef          	jal	ra,2005efd6 <_rand>
2005f886:	c62a                	c.swsp	a0,12(sp)
2005f888:	00291513          	slli	a0,s2,0x2
2005f88c:	8622                	c.mv	a2,s0
2005f88e:	006c                	c.addi4spn	a1,sp,12
2005f890:	9526                	c.add	a0,s1
2005f892:	dffaa097          	auipc	ra,0xdffaa
2005f896:	d80080e7          	jalr	ra,-640(ra) # 9612 <_memcpy>
2005f89a:	b7d1                	c.j	2005f85e <TRNG_get_random_bytes+0x4a>
2005f89c:	f3aff0ef          	jal	ra,2005efd6 <_rand>
2005f8a0:	00299793          	slli	a5,s3,0x2
2005f8a4:	97a6                	c.add	a5,s1
2005f8a6:	c388                	c.sw	a0,0(a5)
2005f8a8:	0985                	c.addi	s3,1
2005f8aa:	b7e9                	c.j	2005f874 <TRNG_get_random_bytes+0x60>

2005f8ac <io_assert_failed>:
2005f8ac:	862e                	c.mv	a2,a1
2005f8ae:	85aa                	c.mv	a1,a0
2005f8b0:	20064537          	lui	a0,0x20064
2005f8b4:	1141                	c.addi	sp,-16
2005f8b6:	b5050513          	addi	a0,a0,-1200 # 20063b50 <TaskStateString.0+0xb8>
2005f8ba:	c606                	c.swsp	ra,12(sp)
2005f8bc:	aebff0ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005f8c0:	a001                	c.j	2005f8c0 <io_assert_failed+0x14>

2005f8c2 <irq_set_priority>:
2005f8c2:	471d                	c.li	a4,7
2005f8c4:	02b77563          	bgeu	a4,a1,2005f8ee <irq_set_priority+0x2c>
2005f8c8:	862e                	c.mv	a2,a1
2005f8ca:	85aa                	c.mv	a1,a0
2005f8cc:	20064537          	lui	a0,0x20064
2005f8d0:	1141                	c.addi	sp,-16
2005f8d2:	469d                	c.li	a3,7
2005f8d4:	c2c50513          	addi	a0,a0,-980 # 20063c2c <TaskStateString.0+0x194>
2005f8d8:	c606                	c.swsp	ra,12(sp)
2005f8da:	acdff0ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2005f8de:	20064537          	lui	a0,0x20064
2005f8e2:	44800593          	addi	a1,zero,1096
2005f8e6:	b8450513          	addi	a0,a0,-1148 # 20063b84 <TaskStateString.0+0xec>
2005f8ea:	fc3ff0ef          	jal	ra,2005f8ac <io_assert_failed>
2005f8ee:	73e00067          	jalr	zero,1854(zero) # 0 <Reset_Handler>

2005f8f2 <irq_register>:
2005f8f2:	20000737          	lui	a4,0x20000
2005f8f6:	1141                	c.addi	sp,-16
2005f8f8:	00259893          	slli	a7,a1,0x2
2005f8fc:	882a                	c.mv	a6,a0
2005f8fe:	00070713          	addi	a4,a4,0 # 20000000 <UserIrqFunTable>
2005f902:	200007b7          	lui	a5,0x20000
2005f906:	c606                	c.swsp	ra,12(sp)
2005f908:	9746                	c.add	a4,a7
2005f90a:	00186813          	ori	a6,a6,1
2005f90e:	1c078793          	addi	a5,a5,448 # 200001c0 <UserIrqDataTable>
2005f912:	01072023          	sw	a6,0(a4)
2005f916:	97c6                	c.add	a5,a7
2005f918:	852e                	c.mv	a0,a1
2005f91a:	c390                	c.sw	a2,0(a5)
2005f91c:	85b6                	c.mv	a1,a3
2005f91e:	fa5ff0ef          	jal	ra,2005f8c2 <irq_set_priority>
2005f922:	40b2                	c.lwsp	ra,12(sp)
2005f924:	4505                	c.li	a0,1
2005f926:	0141                	c.addi	sp,16
2005f928:	8082                	c.jr	ra

2005f92a <unwind_frame_rv32i>:
2005f92a:	715d                	c.addi16sp	sp,-80
2005f92c:	c686                	c.swsp	ra,76(sp)
2005f92e:	c4a2                	c.swsp	s0,72(sp)
2005f930:	0880                	c.addi4spn	s0,sp,80
2005f932:	faa42e23          	sw	a0,-68(s0)
2005f936:	fab42c23          	sw	a1,-72(s0)
2005f93a:	fe042623          	sw	zero,-20(s0)
2005f93e:	fe042423          	sw	zero,-24(s0)
2005f942:	fbc42783          	lw	a5,-68(s0)
2005f946:	43dc                	c.lw	a5,4(a5)
2005f948:	c789                	c.beqz	a5,2005f952 <unwind_frame_rv32i+0x28>
2005f94a:	fbc42783          	lw	a5,-68(s0)
2005f94e:	439c                	c.lw	a5,0(a5)
2005f950:	e399                	c.bnez	a5,2005f956 <unwind_frame_rv32i+0x2c>
2005f952:	57fd                	c.li	a5,-1
2005f954:	a44d                	c.j	2005fbf6 <unwind_frame_rv32i+0x2cc>
2005f956:	fbc42783          	lw	a5,-68(s0)
2005f95a:	43dc                	c.lw	a5,4(a5)
2005f95c:	fef42223          	sw	a5,-28(s0)
2005f960:	a435                	c.j	2005fb8c <unwind_frame_rv32i+0x262>
2005f962:	fe442783          	lw	a5,-28(s0)
2005f966:	8b89                	c.andi	a5,2
2005f968:	c385                	c.beqz	a5,2005f988 <unwind_frame_rv32i+0x5e>
2005f96a:	fe442783          	lw	a5,-28(s0)
2005f96e:	0007d783          	lhu	a5,0(a5)
2005f972:	873e                	c.mv	a4,a5
2005f974:	fe442783          	lw	a5,-28(s0)
2005f978:	0789                	c.addi	a5,2
2005f97a:	0007d783          	lhu	a5,0(a5)
2005f97e:	07c2                	c.slli	a5,0x10
2005f980:	8fd9                	c.or	a5,a4
2005f982:	fcf42c23          	sw	a5,-40(s0)
2005f986:	a031                	c.j	2005f992 <unwind_frame_rv32i+0x68>
2005f988:	fe442783          	lw	a5,-28(s0)
2005f98c:	439c                	c.lw	a5,0(a5)
2005f98e:	fcf42c23          	sw	a5,-40(s0)
2005f992:	fe442783          	lw	a5,-28(s0)
2005f996:	0007d783          	lhu	a5,0(a5)
2005f99a:	fcf41a23          	sh	a5,-44(s0)
2005f99e:	fe442783          	lw	a5,-28(s0)
2005f9a2:	0789                	c.addi	a5,2
2005f9a4:	0007d783          	lhu	a5,0(a5)
2005f9a8:	fcf41823          	sh	a5,-48(s0)
2005f9ac:	fd845783          	lhu	a5,-40(s0)
2005f9b0:	07f7f793          	andi	a5,a5,127
2005f9b4:	01079713          	slli	a4,a5,0x10
2005f9b8:	8341                	c.srli	a4,0x10
2005f9ba:	47cd                	c.li	a5,19
2005f9bc:	0af71c63          	bne	a4,a5,2005fa74 <unwind_frame_rv32i+0x14a>
2005f9c0:	fd845703          	lhu	a4,-40(s0)
2005f9c4:	679d                	c.lui	a5,0x7
2005f9c6:	8ff9                	c.and	a5,a4
2005f9c8:	07c2                	c.slli	a5,0x10
2005f9ca:	83c1                	c.srli	a5,0x10
2005f9cc:	e7c5                	c.bnez	a5,2005fa74 <unwind_frame_rv32i+0x14a>
2005f9ce:	fd842703          	lw	a4,-40(s0)
2005f9d2:	6785                	c.lui	a5,0x1
2005f9d4:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005f9d8:	8f7d                	c.and	a4,a5
2005f9da:	10000793          	addi	a5,zero,256
2005f9de:	18f71f63          	bne	a4,a5,2005fb7c <unwind_frame_rv32i+0x252>
2005f9e2:	fd842703          	lw	a4,-40(s0)
2005f9e6:	000f87b7          	lui	a5,0xf8
2005f9ea:	8f7d                	c.and	a4,a5
2005f9ec:	67c1                	c.lui	a5,0x10
2005f9ee:	18f71763          	bne	a4,a5,2005fb7c <unwind_frame_rv32i+0x252>
2005f9f2:	fd842783          	lw	a5,-40(s0)
2005f9f6:	83d1                	c.srli	a5,0x14
2005f9f8:	07c2                	c.slli	a5,0x10
2005f9fa:	83c1                	c.srli	a5,0x10
2005f9fc:	0792                	c.slli	a5,0x4
2005f9fe:	07c2                	c.slli	a5,0x10
2005fa00:	87c1                	c.srai	a5,0x10
2005fa02:	8791                	c.srai	a5,0x4
2005fa04:	07c2                	c.slli	a5,0x10
2005fa06:	87c1                	c.srai	a5,0x10
2005fa08:	873e                	c.mv	a4,a5
2005fa0a:	6785                	c.lui	a5,0x1
2005fa0c:	17fd                	c.addi	a5,-1
2005fa0e:	00f776b3          	and	a3,a4,a5
2005fa12:	fcc45703          	lhu	a4,-52(s0)
2005fa16:	77fd                	c.lui	a5,0xfffff
2005fa18:	8ff9                	c.and	a5,a4
2005fa1a:	873e                	c.mv	a4,a5
2005fa1c:	87b6                	c.mv	a5,a3
2005fa1e:	8fd9                	c.or	a5,a4
2005fa20:	fcf41623          	sh	a5,-52(s0)
2005fa24:	fcc42783          	lw	a5,-52(s0)
2005fa28:	0792                	c.slli	a5,0x4
2005fa2a:	07c2                	c.slli	a5,0x10
2005fa2c:	87c1                	c.srai	a5,0x10
2005fa2e:	8791                	c.srai	a5,0x4
2005fa30:	07c2                	c.slli	a5,0x10
2005fa32:	87c1                	c.srai	a5,0x10
2005fa34:	fef42023          	sw	a5,-32(s0)
2005fa38:	fe042783          	lw	a5,-32(s0)
2005fa3c:	02f05363          	bge	zero,a5,2005fa62 <unwind_frame_rv32i+0x138>
2005fa40:	fe842783          	lw	a5,-24(s0)
2005fa44:	86be                	c.mv	a3,a5
2005fa46:	10d00613          	addi	a2,zero,269
2005fa4a:	200647b7          	lui	a5,0x20064
2005fa4e:	e5078593          	addi	a1,a5,-432 # 20063e50 <__FUNCTION__.3>
2005fa52:	200647b7          	lui	a5,0x20064
2005fa56:	c5078513          	addi	a0,a5,-944 # 20063c50 <TaskStateString.0+0x1b8>
2005fa5a:	92dff0ef          	jal	ra,2005f386 <DiagPrintf>
2005fa5e:	57fd                	c.li	a5,-1
2005fa60:	aa59                	c.j	2005fbf6 <unwind_frame_rv32i+0x2cc>
2005fa62:	fe042783          	lw	a5,-32(s0)
2005fa66:	0007d463          	bge	a5,zero,2005fa6e <unwind_frame_rv32i+0x144>
2005fa6a:	40f007b3          	sub	a5,zero,a5
2005fa6e:	fef42423          	sw	a5,-24(s0)
2005fa72:	a229                	c.j	2005fb7c <unwind_frame_rv32i+0x252>
2005fa74:	fd845783          	lhu	a5,-40(s0)
2005fa78:	07f7f793          	andi	a5,a5,127
2005fa7c:	01079713          	slli	a4,a5,0x10
2005fa80:	8341                	c.srli	a4,0x10
2005fa82:	02300793          	addi	a5,zero,35
2005fa86:	0cf71463          	bne	a4,a5,2005fb4e <unwind_frame_rv32i+0x224>
2005fa8a:	fd845703          	lhu	a4,-40(s0)
2005fa8e:	679d                	c.lui	a5,0x7
2005fa90:	8ff9                	c.and	a5,a4
2005fa92:	01079713          	slli	a4,a5,0x10
2005fa96:	8341                	c.srli	a4,0x10
2005fa98:	6789                	c.lui	a5,0x2
2005fa9a:	0af71a63          	bne	a4,a5,2005fb4e <unwind_frame_rv32i+0x224>
2005fa9e:	fd842703          	lw	a4,-40(s0)
2005faa2:	000f87b7          	lui	a5,0xf8
2005faa6:	8f7d                	c.and	a4,a5
2005faa8:	67c1                	c.lui	a5,0x10
2005faaa:	0cf71b63          	bne	a4,a5,2005fb80 <unwind_frame_rv32i+0x256>
2005faae:	fd842703          	lw	a4,-40(s0)
2005fab2:	01f007b7          	lui	a5,0x1f00
2005fab6:	8f7d                	c.and	a4,a5
2005fab8:	001007b7          	lui	a5,0x100
2005fabc:	0cf71263          	bne	a4,a5,2005fb80 <unwind_frame_rv32i+0x256>
2005fac0:	fd842783          	lw	a5,-40(s0)
2005fac4:	83e5                	c.srli	a5,0x19
2005fac6:	0ff7f793          	andi	a5,a5,255
2005faca:	0796                	c.slli	a5,0x5
2005facc:	01079713          	slli	a4,a5,0x10
2005fad0:	8741                	c.srai	a4,0x10
2005fad2:	fd842783          	lw	a5,-40(s0)
2005fad6:	839d                	c.srli	a5,0x7
2005fad8:	8bfd                	c.andi	a5,31
2005fada:	0ff7f793          	andi	a5,a5,255
2005fade:	07c2                	c.slli	a5,0x10
2005fae0:	87c1                	c.srai	a5,0x10
2005fae2:	8fd9                	c.or	a5,a4
2005fae4:	07c2                	c.slli	a5,0x10
2005fae6:	87c1                	c.srai	a5,0x10
2005fae8:	0792                	c.slli	a5,0x4
2005faea:	07c2                	c.slli	a5,0x10
2005faec:	87c1                	c.srai	a5,0x10
2005faee:	8791                	c.srai	a5,0x4
2005faf0:	07c2                	c.slli	a5,0x10
2005faf2:	87c1                	c.srai	a5,0x10
2005faf4:	873e                	c.mv	a4,a5
2005faf6:	6785                	c.lui	a5,0x1
2005faf8:	17fd                	c.addi	a5,-1
2005fafa:	00f776b3          	and	a3,a4,a5
2005fafe:	fc845703          	lhu	a4,-56(s0)
2005fb02:	77fd                	c.lui	a5,0xfffff
2005fb04:	8ff9                	c.and	a5,a4
2005fb06:	873e                	c.mv	a4,a5
2005fb08:	87b6                	c.mv	a5,a3
2005fb0a:	8fd9                	c.or	a5,a4
2005fb0c:	fcf41423          	sh	a5,-56(s0)
2005fb10:	fc842783          	lw	a5,-56(s0)
2005fb14:	0792                	c.slli	a5,0x4
2005fb16:	07c2                	c.slli	a5,0x10
2005fb18:	87c1                	c.srai	a5,0x10
2005fb1a:	8791                	c.srai	a5,0x4
2005fb1c:	07c2                	c.slli	a5,0x10
2005fb1e:	87c1                	c.srai	a5,0x10
2005fb20:	fef42623          	sw	a5,-20(s0)
2005fb24:	fec42783          	lw	a5,-20(s0)
2005fb28:	0407dc63          	bge	a5,zero,2005fb80 <unwind_frame_rv32i+0x256>
2005fb2c:	fec42783          	lw	a5,-20(s0)
2005fb30:	86be                	c.mv	a3,a5
2005fb32:	11d00613          	addi	a2,zero,285
2005fb36:	200647b7          	lui	a5,0x20064
2005fb3a:	e5078593          	addi	a1,a5,-432 # 20063e50 <__FUNCTION__.3>
2005fb3e:	200647b7          	lui	a5,0x20064
2005fb42:	c8c78513          	addi	a0,a5,-884 # 20063c8c <TaskStateString.0+0x1f4>
2005fb46:	841ff0ef          	jal	ra,2005f386 <DiagPrintf>
2005fb4a:	57fd                	c.li	a5,-1
2005fb4c:	a06d                	c.j	2005fbf6 <unwind_frame_rv32i+0x2cc>
2005fb4e:	fd842703          	lw	a4,-40(s0)
2005fb52:	67a1                	c.lui	a5,0x8
2005fb54:	06778793          	addi	a5,a5,103 # 8067 <irq_unregister+0x41>
2005fb58:	06f70063          	beq	a4,a5,2005fbb8 <unwind_frame_rv32i+0x28e>
2005fb5c:	fd445703          	lhu	a4,-44(s0)
2005fb60:	67a1                	c.lui	a5,0x8
2005fb62:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fb66:	00f70963          	beq	a4,a5,2005fb78 <unwind_frame_rv32i+0x24e>
2005fb6a:	fd045703          	lhu	a4,-48(s0)
2005fb6e:	67a1                	c.lui	a5,0x8
2005fb70:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fb74:	00f71763          	bne	a4,a5,2005fb82 <unwind_frame_rv32i+0x258>
2005fb78:	57fd                	c.li	a5,-1
2005fb7a:	a8b5                	c.j	2005fbf6 <unwind_frame_rv32i+0x2cc>
2005fb7c:	0001                	c.addi	zero,0
2005fb7e:	a011                	c.j	2005fb82 <unwind_frame_rv32i+0x258>
2005fb80:	0001                	c.addi	zero,0
2005fb82:	fe442783          	lw	a5,-28(s0)
2005fb86:	17f1                	c.addi	a5,-4
2005fb88:	fef42223          	sw	a5,-28(s0)
2005fb8c:	fb842783          	lw	a5,-72(s0)
2005fb90:	078a                	c.slli	a5,0x2
2005fb92:	fe442703          	lw	a4,-28(s0)
2005fb96:	973e                	c.add	a4,a5
2005fb98:	fbc42783          	lw	a5,-68(s0)
2005fb9c:	43dc                	c.lw	a5,4(a5)
2005fb9e:	00e7fe63          	bgeu	a5,a4,2005fbba <unwind_frame_rv32i+0x290>
2005fba2:	fec42783          	lw	a5,-20(s0)
2005fba6:	c781                	c.beqz	a5,2005fbae <unwind_frame_rv32i+0x284>
2005fba8:	fe842783          	lw	a5,-24(s0)
2005fbac:	e799                	c.bnez	a5,2005fbba <unwind_frame_rv32i+0x290>
2005fbae:	fe442783          	lw	a5,-28(s0)
2005fbb2:	da0798e3          	bne	a5,zero,2005f962 <unwind_frame_rv32i+0x38>
2005fbb6:	a011                	c.j	2005fbba <unwind_frame_rv32i+0x290>
2005fbb8:	0001                	c.addi	zero,0
2005fbba:	fec42783          	lw	a5,-20(s0)
2005fbbe:	cb91                	c.beqz	a5,2005fbd2 <unwind_frame_rv32i+0x2a8>
2005fbc0:	fbc42783          	lw	a5,-68(s0)
2005fbc4:	4398                	c.lw	a4,0(a5)
2005fbc6:	fec42783          	lw	a5,-20(s0)
2005fbca:	97ba                	c.add	a5,a4
2005fbcc:	439c                	c.lw	a5,0(a5)
2005fbce:	873e                	c.mv	a4,a5
2005fbd0:	a011                	c.j	2005fbd4 <unwind_frame_rv32i+0x2aa>
2005fbd2:	4701                	c.li	a4,0
2005fbd4:	fbc42783          	lw	a5,-68(s0)
2005fbd8:	c798                	c.sw	a4,8(a5)
2005fbda:	fe842703          	lw	a4,-24(s0)
2005fbde:	fbc42783          	lw	a5,-68(s0)
2005fbe2:	c7d8                	c.sw	a4,12(a5)
2005fbe4:	fec42783          	lw	a5,-20(s0)
2005fbe8:	c781                	c.beqz	a5,2005fbf0 <unwind_frame_rv32i+0x2c6>
2005fbea:	fe842783          	lw	a5,-24(s0)
2005fbee:	e399                	c.bnez	a5,2005fbf4 <unwind_frame_rv32i+0x2ca>
2005fbf0:	57fd                	c.li	a5,-1
2005fbf2:	a011                	c.j	2005fbf6 <unwind_frame_rv32i+0x2cc>
2005fbf4:	4781                	c.li	a5,0
2005fbf6:	853e                	c.mv	a0,a5
2005fbf8:	40b6                	c.lwsp	ra,76(sp)
2005fbfa:	4426                	c.lwsp	s0,72(sp)
2005fbfc:	6161                	c.addi16sp	sp,80
2005fbfe:	8082                	c.jr	ra

2005fc00 <unwind_frame_rv32c>:
2005fc00:	715d                	c.addi16sp	sp,-80
2005fc02:	c686                	c.swsp	ra,76(sp)
2005fc04:	c4a2                	c.swsp	s0,72(sp)
2005fc06:	0880                	c.addi4spn	s0,sp,80
2005fc08:	faa42e23          	sw	a0,-68(s0)
2005fc0c:	fab42c23          	sw	a1,-72(s0)
2005fc10:	fe042623          	sw	zero,-20(s0)
2005fc14:	fe042423          	sw	zero,-24(s0)
2005fc18:	fbc42783          	lw	a5,-68(s0)
2005fc1c:	43dc                	c.lw	a5,4(a5)
2005fc1e:	cb91                	c.beqz	a5,2005fc32 <unwind_frame_rv32c+0x32>
2005fc20:	fbc42783          	lw	a5,-68(s0)
2005fc24:	439c                	c.lw	a5,0(a5)
2005fc26:	c791                	c.beqz	a5,2005fc32 <unwind_frame_rv32c+0x32>
2005fc28:	fbc42783          	lw	a5,-68(s0)
2005fc2c:	43dc                	c.lw	a5,4(a5)
2005fc2e:	8b85                	c.andi	a5,1
2005fc30:	c399                	c.beqz	a5,2005fc36 <unwind_frame_rv32c+0x36>
2005fc32:	57fd                	c.li	a5,-1
2005fc34:	ae3d                	c.j	2005ff72 <unwind_frame_rv32c+0x372>
2005fc36:	fbc42783          	lw	a5,-68(s0)
2005fc3a:	43dc                	c.lw	a5,4(a5)
2005fc3c:	fef42223          	sw	a5,-28(s0)
2005fc40:	a4e1                	c.j	2005ff08 <unwind_frame_rv32c+0x308>
2005fc42:	fe442783          	lw	a5,-28(s0)
2005fc46:	0007d783          	lhu	a5,0(a5)
2005fc4a:	fcf41c23          	sh	a5,-40(s0)
2005fc4e:	fd845703          	lhu	a4,-40(s0)
2005fc52:	77f9                	c.lui	a5,0xffffe
2005fc54:	8ff9                	c.and	a5,a4
2005fc56:	07c2                	c.slli	a5,0x10
2005fc58:	83c1                	c.srli	a5,0x10
2005fc5a:	e7e9                	c.bnez	a5,2005fd24 <unwind_frame_rv32c+0x124>
2005fc5c:	fd845783          	lhu	a5,-40(s0)
2005fc60:	8b8d                	c.andi	a5,3
2005fc62:	01079713          	slli	a4,a5,0x10
2005fc66:	8341                	c.srli	a4,0x10
2005fc68:	4785                	c.li	a5,1
2005fc6a:	0af71d63          	bne	a4,a5,2005fd24 <unwind_frame_rv32c+0x124>
2005fc6e:	fd845703          	lhu	a4,-40(s0)
2005fc72:	6785                	c.lui	a5,0x1
2005fc74:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fc78:	8ff9                	c.and	a5,a4
2005fc7a:	01079713          	slli	a4,a5,0x10
2005fc7e:	8341                	c.srli	a4,0x10
2005fc80:	10000793          	addi	a5,zero,256
2005fc84:	0af71063          	bne	a4,a5,2005fd24 <unwind_frame_rv32c+0x124>
2005fc88:	fd842783          	lw	a5,-40(s0)
2005fc8c:	83b1                	c.srli	a5,0xc
2005fc8e:	8b85                	c.andi	a5,1
2005fc90:	0ff7f793          	andi	a5,a5,255
2005fc94:	0796                	c.slli	a5,0x5
2005fc96:	01879713          	slli	a4,a5,0x18
2005fc9a:	8761                	c.srai	a4,0x18
2005fc9c:	fd842783          	lw	a5,-40(s0)
2005fca0:	8389                	c.srli	a5,0x2
2005fca2:	8bfd                	c.andi	a5,31
2005fca4:	0ff7f793          	andi	a5,a5,255
2005fca8:	07e2                	c.slli	a5,0x18
2005fcaa:	87e1                	c.srai	a5,0x18
2005fcac:	8fd9                	c.or	a5,a4
2005fcae:	07e2                	c.slli	a5,0x18
2005fcb0:	87e1                	c.srai	a5,0x18
2005fcb2:	078a                	c.slli	a5,0x2
2005fcb4:	07e2                	c.slli	a5,0x18
2005fcb6:	87e1                	c.srai	a5,0x18
2005fcb8:	8789                	c.srai	a5,0x2
2005fcba:	07e2                	c.slli	a5,0x18
2005fcbc:	87e1                	c.srai	a5,0x18
2005fcbe:	03f7f693          	andi	a3,a5,63
2005fcc2:	fd444783          	lbu	a5,-44(s0)
2005fcc6:	fc07f793          	andi	a5,a5,-64
2005fcca:	873e                	c.mv	a4,a5
2005fccc:	87b6                	c.mv	a5,a3
2005fcce:	8fd9                	c.or	a5,a4
2005fcd0:	fcf40a23          	sb	a5,-44(s0)
2005fcd4:	fd442783          	lw	a5,-44(s0)
2005fcd8:	078a                	c.slli	a5,0x2
2005fcda:	07e2                	c.slli	a5,0x18
2005fcdc:	87e1                	c.srai	a5,0x18
2005fcde:	8789                	c.srai	a5,0x2
2005fce0:	07e2                	c.slli	a5,0x18
2005fce2:	87e1                	c.srai	a5,0x18
2005fce4:	fef42023          	sw	a5,-32(s0)
2005fce8:	fe042783          	lw	a5,-32(s0)
2005fcec:	02f05363          	bge	zero,a5,2005fd12 <unwind_frame_rv32c+0x112>
2005fcf0:	fe842783          	lw	a5,-24(s0)
2005fcf4:	86be                	c.mv	a3,a5
2005fcf6:	16900613          	addi	a2,zero,361
2005fcfa:	200647b7          	lui	a5,0x20064
2005fcfe:	e6478593          	addi	a1,a5,-412 # 20063e64 <__FUNCTION__.2>
2005fd02:	200647b7          	lui	a5,0x20064
2005fd06:	c5078513          	addi	a0,a5,-944 # 20063c50 <TaskStateString.0+0x1b8>
2005fd0a:	e7cff0ef          	jal	ra,2005f386 <DiagPrintf>
2005fd0e:	57fd                	c.li	a5,-1
2005fd10:	a48d                	c.j	2005ff72 <unwind_frame_rv32c+0x372>
2005fd12:	fe042783          	lw	a5,-32(s0)
2005fd16:	0007d463          	bge	a5,zero,2005fd1e <unwind_frame_rv32c+0x11e>
2005fd1a:	40f007b3          	sub	a5,zero,a5
2005fd1e:	fef42423          	sw	a5,-24(s0)
2005fd22:	aaf1                	c.j	2005fefe <unwind_frame_rv32c+0x2fe>
2005fd24:	fd845703          	lhu	a4,-40(s0)
2005fd28:	77f9                	c.lui	a5,0xffffe
2005fd2a:	8ff9                	c.and	a5,a4
2005fd2c:	01079713          	slli	a4,a5,0x10
2005fd30:	8341                	c.srli	a4,0x10
2005fd32:	6799                	c.lui	a5,0x6
2005fd34:	10f71e63          	bne	a4,a5,2005fe50 <unwind_frame_rv32c+0x250>
2005fd38:	fd845783          	lhu	a5,-40(s0)
2005fd3c:	8b8d                	c.andi	a5,3
2005fd3e:	01079713          	slli	a4,a5,0x10
2005fd42:	8341                	c.srli	a4,0x10
2005fd44:	4785                	c.li	a5,1
2005fd46:	10f71563          	bne	a4,a5,2005fe50 <unwind_frame_rv32c+0x250>
2005fd4a:	fd845703          	lhu	a4,-40(s0)
2005fd4e:	6785                	c.lui	a5,0x1
2005fd50:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fd54:	8ff9                	c.and	a5,a4
2005fd56:	01079713          	slli	a4,a5,0x10
2005fd5a:	8341                	c.srli	a4,0x10
2005fd5c:	10000793          	addi	a5,zero,256
2005fd60:	0ef71863          	bne	a4,a5,2005fe50 <unwind_frame_rv32c+0x250>
2005fd64:	fd842783          	lw	a5,-40(s0)
2005fd68:	83b1                	c.srli	a5,0xc
2005fd6a:	8b85                	c.andi	a5,1
2005fd6c:	0ff7f793          	andi	a5,a5,255
2005fd70:	07a6                	c.slli	a5,0x9
2005fd72:	01079713          	slli	a4,a5,0x10
2005fd76:	8741                	c.srai	a4,0x10
2005fd78:	fd842783          	lw	a5,-40(s0)
2005fd7c:	8399                	c.srli	a5,0x6
2005fd7e:	8b85                	c.andi	a5,1
2005fd80:	0ff7f793          	andi	a5,a5,255
2005fd84:	0792                	c.slli	a5,0x4
2005fd86:	07c2                	c.slli	a5,0x10
2005fd88:	87c1                	c.srai	a5,0x10
2005fd8a:	8fd9                	c.or	a5,a4
2005fd8c:	01079713          	slli	a4,a5,0x10
2005fd90:	8741                	c.srai	a4,0x10
2005fd92:	fd842783          	lw	a5,-40(s0)
2005fd96:	8395                	c.srli	a5,0x5
2005fd98:	8b85                	c.andi	a5,1
2005fd9a:	0ff7f793          	andi	a5,a5,255
2005fd9e:	079a                	c.slli	a5,0x6
2005fda0:	07c2                	c.slli	a5,0x10
2005fda2:	87c1                	c.srai	a5,0x10
2005fda4:	8fd9                	c.or	a5,a4
2005fda6:	01079713          	slli	a4,a5,0x10
2005fdaa:	8741                	c.srai	a4,0x10
2005fdac:	fd842783          	lw	a5,-40(s0)
2005fdb0:	838d                	c.srli	a5,0x3
2005fdb2:	8b8d                	c.andi	a5,3
2005fdb4:	0ff7f793          	andi	a5,a5,255
2005fdb8:	079e                	c.slli	a5,0x7
2005fdba:	07c2                	c.slli	a5,0x10
2005fdbc:	87c1                	c.srai	a5,0x10
2005fdbe:	8fd9                	c.or	a5,a4
2005fdc0:	01079713          	slli	a4,a5,0x10
2005fdc4:	8741                	c.srai	a4,0x10
2005fdc6:	fd842783          	lw	a5,-40(s0)
2005fdca:	8389                	c.srli	a5,0x2
2005fdcc:	8b85                	c.andi	a5,1
2005fdce:	0ff7f793          	andi	a5,a5,255
2005fdd2:	0796                	c.slli	a5,0x5
2005fdd4:	07c2                	c.slli	a5,0x10
2005fdd6:	87c1                	c.srai	a5,0x10
2005fdd8:	8fd9                	c.or	a5,a4
2005fdda:	07c2                	c.slli	a5,0x10
2005fddc:	87c1                	c.srai	a5,0x10
2005fdde:	079a                	c.slli	a5,0x6
2005fde0:	07c2                	c.slli	a5,0x10
2005fde2:	87c1                	c.srai	a5,0x10
2005fde4:	8799                	c.srai	a5,0x6
2005fde6:	07c2                	c.slli	a5,0x10
2005fde8:	87c1                	c.srai	a5,0x10
2005fdea:	3ff7f693          	andi	a3,a5,1023
2005fdee:	fd045783          	lhu	a5,-48(s0)
2005fdf2:	c007f793          	andi	a5,a5,-1024
2005fdf6:	873e                	c.mv	a4,a5
2005fdf8:	87b6                	c.mv	a5,a3
2005fdfa:	8fd9                	c.or	a5,a4
2005fdfc:	fcf41823          	sh	a5,-48(s0)
2005fe00:	fd042783          	lw	a5,-48(s0)
2005fe04:	079a                	c.slli	a5,0x6
2005fe06:	07c2                	c.slli	a5,0x10
2005fe08:	87c1                	c.srai	a5,0x10
2005fe0a:	8799                	c.srai	a5,0x6
2005fe0c:	07c2                	c.slli	a5,0x10
2005fe0e:	87c1                	c.srai	a5,0x10
2005fe10:	fcf42e23          	sw	a5,-36(s0)
2005fe14:	fdc42783          	lw	a5,-36(s0)
2005fe18:	02f05363          	bge	zero,a5,2005fe3e <unwind_frame_rv32c+0x23e>
2005fe1c:	fe842783          	lw	a5,-24(s0)
2005fe20:	86be                	c.mv	a3,a5
2005fe22:	18100613          	addi	a2,zero,385
2005fe26:	200647b7          	lui	a5,0x20064
2005fe2a:	e6478593          	addi	a1,a5,-412 # 20063e64 <__FUNCTION__.2>
2005fe2e:	200647b7          	lui	a5,0x20064
2005fe32:	c5078513          	addi	a0,a5,-944 # 20063c50 <TaskStateString.0+0x1b8>
2005fe36:	d50ff0ef          	jal	ra,2005f386 <DiagPrintf>
2005fe3a:	57fd                	c.li	a5,-1
2005fe3c:	aa1d                	c.j	2005ff72 <unwind_frame_rv32c+0x372>
2005fe3e:	fdc42783          	lw	a5,-36(s0)
2005fe42:	0007d463          	bge	a5,zero,2005fe4a <unwind_frame_rv32c+0x24a>
2005fe46:	40f007b3          	sub	a5,zero,a5
2005fe4a:	fef42423          	sw	a5,-24(s0)
2005fe4e:	a845                	c.j	2005fefe <unwind_frame_rv32c+0x2fe>
2005fe50:	fd845703          	lhu	a4,-40(s0)
2005fe54:	77f9                	c.lui	a5,0xffffe
2005fe56:	8ff9                	c.and	a5,a4
2005fe58:	01079713          	slli	a4,a5,0x10
2005fe5c:	8341                	c.srli	a4,0x10
2005fe5e:	67b1                	c.lui	a5,0xc
2005fe60:	08f71663          	bne	a4,a5,2005feec <unwind_frame_rv32c+0x2ec>
2005fe64:	fd845783          	lhu	a5,-40(s0)
2005fe68:	8b8d                	c.andi	a5,3
2005fe6a:	01079713          	slli	a4,a5,0x10
2005fe6e:	8341                	c.srli	a4,0x10
2005fe70:	4789                	c.li	a5,2
2005fe72:	06f71d63          	bne	a4,a5,2005feec <unwind_frame_rv32c+0x2ec>
2005fe76:	fd845783          	lhu	a5,-40(s0)
2005fe7a:	07c7f793          	andi	a5,a5,124
2005fe7e:	01079713          	slli	a4,a5,0x10
2005fe82:	8341                	c.srli	a4,0x10
2005fe84:	4791                	c.li	a5,4
2005fe86:	06f71363          	bne	a4,a5,2005feec <unwind_frame_rv32c+0x2ec>
2005fe8a:	fd842783          	lw	a5,-40(s0)
2005fe8e:	839d                	c.srli	a5,0x7
2005fe90:	8b8d                	c.andi	a5,3
2005fe92:	0ff7f793          	andi	a5,a5,255
2005fe96:	079a                	c.slli	a5,0x6
2005fe98:	01879713          	slli	a4,a5,0x18
2005fe9c:	8761                	c.srai	a4,0x18
2005fe9e:	fd842783          	lw	a5,-40(s0)
2005fea2:	83a5                	c.srli	a5,0x9
2005fea4:	8bbd                	c.andi	a5,15
2005fea6:	0ff7f793          	andi	a5,a5,255
2005feaa:	078a                	c.slli	a5,0x2
2005feac:	07e2                	c.slli	a5,0x18
2005feae:	87e1                	c.srai	a5,0x18
2005feb0:	8fd9                	c.or	a5,a4
2005feb2:	07e2                	c.slli	a5,0x18
2005feb4:	87e1                	c.srai	a5,0x18
2005feb6:	fcf40623          	sb	a5,-52(s0)
2005feba:	fcc40783          	lb	a5,-52(s0)
2005febe:	fef42623          	sw	a5,-20(s0)
2005fec2:	fec42783          	lw	a5,-20(s0)
2005fec6:	0207db63          	bge	a5,zero,2005fefc <unwind_frame_rv32c+0x2fc>
2005feca:	fec42783          	lw	a5,-20(s0)
2005fece:	86be                	c.mv	a3,a5
2005fed0:	19100613          	addi	a2,zero,401
2005fed4:	200647b7          	lui	a5,0x20064
2005fed8:	e6478593          	addi	a1,a5,-412 # 20063e64 <__FUNCTION__.2>
2005fedc:	200647b7          	lui	a5,0x20064
2005fee0:	c8c78513          	addi	a0,a5,-884 # 20063c8c <TaskStateString.0+0x1f4>
2005fee4:	ca2ff0ef          	jal	ra,2005f386 <DiagPrintf>
2005fee8:	57fd                	c.li	a5,-1
2005feea:	a061                	c.j	2005ff72 <unwind_frame_rv32c+0x372>
2005feec:	fd845703          	lhu	a4,-40(s0)
2005fef0:	67a1                	c.lui	a5,0x8
2005fef2:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fef6:	02f70f63          	beq	a4,a5,2005ff34 <unwind_frame_rv32c+0x334>
2005fefa:	a011                	c.j	2005fefe <unwind_frame_rv32c+0x2fe>
2005fefc:	0001                	c.addi	zero,0
2005fefe:	fe442783          	lw	a5,-28(s0)
2005ff02:	17f9                	c.addi	a5,-2
2005ff04:	fef42223          	sw	a5,-28(s0)
2005ff08:	fb842783          	lw	a5,-72(s0)
2005ff0c:	0786                	c.slli	a5,0x1
2005ff0e:	fe442703          	lw	a4,-28(s0)
2005ff12:	973e                	c.add	a4,a5
2005ff14:	fbc42783          	lw	a5,-68(s0)
2005ff18:	43dc                	c.lw	a5,4(a5)
2005ff1a:	00e7fe63          	bgeu	a5,a4,2005ff36 <unwind_frame_rv32c+0x336>
2005ff1e:	fec42783          	lw	a5,-20(s0)
2005ff22:	c781                	c.beqz	a5,2005ff2a <unwind_frame_rv32c+0x32a>
2005ff24:	fe842783          	lw	a5,-24(s0)
2005ff28:	e799                	c.bnez	a5,2005ff36 <unwind_frame_rv32c+0x336>
2005ff2a:	fe442783          	lw	a5,-28(s0)
2005ff2e:	d0079ae3          	bne	a5,zero,2005fc42 <unwind_frame_rv32c+0x42>
2005ff32:	a011                	c.j	2005ff36 <unwind_frame_rv32c+0x336>
2005ff34:	0001                	c.addi	zero,0
2005ff36:	fec42783          	lw	a5,-20(s0)
2005ff3a:	cb91                	c.beqz	a5,2005ff4e <unwind_frame_rv32c+0x34e>
2005ff3c:	fbc42783          	lw	a5,-68(s0)
2005ff40:	4398                	c.lw	a4,0(a5)
2005ff42:	fec42783          	lw	a5,-20(s0)
2005ff46:	97ba                	c.add	a5,a4
2005ff48:	439c                	c.lw	a5,0(a5)
2005ff4a:	873e                	c.mv	a4,a5
2005ff4c:	a011                	c.j	2005ff50 <unwind_frame_rv32c+0x350>
2005ff4e:	4701                	c.li	a4,0
2005ff50:	fbc42783          	lw	a5,-68(s0)
2005ff54:	c798                	c.sw	a4,8(a5)
2005ff56:	fe842703          	lw	a4,-24(s0)
2005ff5a:	fbc42783          	lw	a5,-68(s0)
2005ff5e:	c7d8                	c.sw	a4,12(a5)
2005ff60:	fec42783          	lw	a5,-20(s0)
2005ff64:	c781                	c.beqz	a5,2005ff6c <unwind_frame_rv32c+0x36c>
2005ff66:	fe842783          	lw	a5,-24(s0)
2005ff6a:	e399                	c.bnez	a5,2005ff70 <unwind_frame_rv32c+0x370>
2005ff6c:	57fd                	c.li	a5,-1
2005ff6e:	a011                	c.j	2005ff72 <unwind_frame_rv32c+0x372>
2005ff70:	4781                	c.li	a5,0
2005ff72:	853e                	c.mv	a0,a5
2005ff74:	40b6                	c.lwsp	ra,76(sp)
2005ff76:	4426                	c.lwsp	s0,72(sp)
2005ff78:	6161                	c.addi16sp	sp,80
2005ff7a:	8082                	c.jr	ra

2005ff7c <do_stack_backtrace>:
2005ff7c:	7179                	c.addi16sp	sp,-48
2005ff7e:	ce52                	c.swsp	s4,28(sp)
2005ff80:	d606                	c.swsp	ra,44(sp)
2005ff82:	d426                	c.swsp	s1,40(sp)
2005ff84:	d24a                	c.swsp	s2,36(sp)
2005ff86:	d04e                	c.swsp	s3,32(sp)
2005ff88:	cc56                	c.swsp	s5,24(sp)
2005ff8a:	c402                	c.swsp	zero,8(sp)
2005ff8c:	c602                	c.swsp	zero,12(sp)
2005ff8e:	c02e                	c.swsp	a1,0(sp)
2005ff90:	8a32                	c.mv	s4,a2
2005ff92:	c525                	c.beqz	a0,2005fffa <do_stack_backtrace+0x7e>
2005ff94:	c1bd                	c.beqz	a1,2005fffa <do_stack_backtrace+0x7e>
2005ff96:	c22a                	c.swsp	a0,4(sp)
2005ff98:	4712                	c.lwsp	a4,4(sp)
2005ff9a:	4682                	c.lwsp	a3,0(sp)
2005ff9c:	200644b7          	lui	s1,0x20064
2005ffa0:	200649b7          	lui	s3,0x20064
2005ffa4:	cc448513          	addi	a0,s1,-828 # 20063cc4 <TaskStateString.0+0x22c>
2005ffa8:	1cc00613          	addi	a2,zero,460
2005ffac:	e7898593          	addi	a1,s3,-392 # 20063e78 <__FUNCTION__.1>
2005ffb0:	cc448493          	addi	s1,s1,-828
2005ffb4:	bd2ff0ef          	jal	ra,2005f386 <DiagPrintf>
2005ffb8:	4901                	c.li	s2,0
2005ffba:	04848493          	addi	s1,s1,72
2005ffbe:	05490b63          	beq	s2,s4,20060014 <do_stack_backtrace+0x98>
2005ffc2:	40000593          	addi	a1,zero,1024
2005ffc6:	850a                	c.mv	a0,sp
2005ffc8:	963ff0ef          	jal	ra,2005f92a <unwind_frame_rv32i>
2005ffcc:	ed15                	c.bnez	a0,20060008 <do_stack_backtrace+0x8c>
2005ffce:	4792                	c.lwsp	a5,4(sp)
2005ffd0:	4702                	c.lwsp	a4,0(sp)
2005ffd2:	48a2                	c.lwsp	a7,8(sp)
2005ffd4:	4832                	c.lwsp	a6,12(sp)
2005ffd6:	86ca                	c.mv	a3,s2
2005ffd8:	1d700613          	addi	a2,zero,471
2005ffdc:	e7898593          	addi	a1,s3,-392
2005ffe0:	8526                	c.mv	a0,s1
2005ffe2:	ba4ff0ef          	jal	ra,2005f386 <DiagPrintf>
2005ffe6:	47a2                	c.lwsp	a5,8(sp)
2005ffe8:	4732                	c.lwsp	a4,12(sp)
2005ffea:	00190a93          	addi	s5,s2,1
2005ffee:	c23e                	c.swsp	a5,4(sp)
2005fff0:	4782                	c.lwsp	a5,0(sp)
2005fff2:	8956                	c.mv	s2,s5
2005fff4:	97ba                	c.add	a5,a4
2005fff6:	c03e                	c.swsp	a5,0(sp)
2005fff8:	b7d9                	c.j	2005ffbe <do_stack_backtrace+0x42>
2005fffa:	00010793          	addi	a5,sp,0
2005fffe:	00000717          	auipc	a4,0x0
20060002:	c23a                	c.swsp	a4,4(sp)
20060004:	c03e                	c.swsp	a5,0(sp)
20060006:	bf49                	c.j	2005ff98 <do_stack_backtrace+0x1c>
20060008:	40000593          	addi	a1,zero,1024
2006000c:	850a                	c.mv	a0,sp
2006000e:	bf3ff0ef          	jal	ra,2005fc00 <unwind_frame_rv32c>
20060012:	dd55                	c.beqz	a0,2005ffce <do_stack_backtrace+0x52>
20060014:	50b2                	c.lwsp	ra,44(sp)
20060016:	54a2                	c.lwsp	s1,40(sp)
20060018:	5982                	c.lwsp	s3,32(sp)
2006001a:	4a72                	c.lwsp	s4,28(sp)
2006001c:	4ae2                	c.lwsp	s5,24(sp)
2006001e:	854a                	c.mv	a0,s2
20060020:	5912                	c.lwsp	s2,36(sp)
20060022:	6145                	c.addi16sp	sp,48
20060024:	8082                	c.jr	ra

20060026 <crash_dump>:
20060026:	711d                	c.addi16sp	sp,-96
20060028:	c8ca                	c.swsp	s2,80(sp)
2006002a:	c6ce                	c.swsp	s3,76(sp)
2006002c:	c2d6                	c.swsp	s5,68(sp)
2006002e:	ce86                	c.swsp	ra,92(sp)
20060030:	cca2                	c.swsp	s0,88(sp)
20060032:	caa6                	c.swsp	s1,84(sp)
20060034:	c4d2                	c.swsp	s4,72(sp)
20060036:	c0da                	c.swsp	s6,64(sp)
20060038:	de5e                	c.swsp	s7,60(sp)
2006003a:	dc62                	c.swsp	s8,56(sp)
2006003c:	da66                	c.swsp	s9,52(sp)
2006003e:	d86a                	c.swsp	s10,48(sp)
20060040:	d66e                	c.swsp	s11,44(sp)
20060042:	89aa                	c.mv	s3,a0
20060044:	892e                	c.mv	s2,a1
20060046:	8ab2                	c.mv	s5,a2
20060048:	34002c73          	csrrs	s8,mscratch,zero
2006004c:	34102cf3          	csrrs	s9,mepc,zero
20060050:	34202a73          	csrrs	s4,mcause,zero
20060054:	34302bf3          	csrrs	s7,mtval,zero
20060058:	20064437          	lui	s0,0x20064
2006005c:	e8c40513          	addi	a0,s0,-372 # 20063e8c <__FUNCTION__.1+0x14>
20060060:	200644b7          	lui	s1,0x20064
20060064:	11200613          	addi	a2,zero,274
20060068:	62848593          	addi	a1,s1,1576 # 20064628 <__FUNCTION__.0>
2006006c:	0551                	c.addi	a0,20
2006006e:	b18ff0ef          	jal	ra,2005f386 <DiagPrintf>
20060072:	e901a783          	lw	a5,-368(gp) # 20066818 <crash_task_info>
20060076:	e8c40413          	addi	s0,s0,-372
2006007a:	c399                	c.beqz	a5,20060080 <crash_dump+0x5a>
2006007c:	8556                	c.mv	a0,s5
2006007e:	9782                	c.jalr	a5
20060080:	473d                	c.li	a4,15
20060082:	87a2                	c.mv	a5,s0
20060084:	01476a63          	bltu	a4,s4,20060098 <crash_dump+0x72>
20060088:	20064737          	lui	a4,0x20064
2006008c:	002a1793          	slli	a5,s4,0x2
20060090:	74c70713          	addi	a4,a4,1868 # 2006474c <exception_code_string>
20060094:	97ba                	c.add	a5,a4
20060096:	439c                	c.lw	a5,0(a5)
20060098:	8752                	c.mv	a4,s4
2006009a:	86ce                	c.mv	a3,s3
2006009c:	11700613          	addi	a2,zero,279
200600a0:	62848593          	addi	a1,s1,1576
200600a4:	05440513          	addi	a0,s0,84
200600a8:	adeff0ef          	jal	ra,2005f386 <DiagPrintf>
200600ac:	47bd                	c.li	a5,15
200600ae:	1547f463          	bgeu	a5,s4,200601f6 <crash_dump+0x1d0>
200600b2:	12b00613          	addi	a2,zero,299
200600b6:	62848593          	addi	a1,s1,1576
200600ba:	0a440513          	addi	a0,s0,164
200600be:	ac8ff0ef          	jal	ra,2005f386 <DiagPrintf>
200600c2:	86e2                	c.mv	a3,s8
200600c4:	12c00613          	addi	a2,zero,300
200600c8:	62848593          	addi	a1,s1,1576
200600cc:	0e840513          	addi	a0,s0,232
200600d0:	ab6ff0ef          	jal	ra,2005f386 <DiagPrintf>
200600d4:	86e6                	c.mv	a3,s9
200600d6:	12d00613          	addi	a2,zero,301
200600da:	62848593          	addi	a1,s1,1576
200600de:	11840513          	addi	a0,s0,280
200600e2:	aa4ff0ef          	jal	ra,2005f386 <DiagPrintf>
200600e6:	86d2                	c.mv	a3,s4
200600e8:	12e00613          	addi	a2,zero,302
200600ec:	62848593          	addi	a1,s1,1576
200600f0:	14840513          	addi	a0,s0,328
200600f4:	a92ff0ef          	jal	ra,2005f386 <DiagPrintf>
200600f8:	86de                	c.mv	a3,s7
200600fa:	12f00613          	addi	a2,zero,303
200600fe:	62848593          	addi	a1,s1,1576
20060102:	17840513          	addi	a0,s0,376
20060106:	20064b37          	lui	s6,0x20064
2006010a:	a7cff0ef          	jal	ra,2005f386 <DiagPrintf>
2006010e:	78cb0b13          	addi	s6,s6,1932 # 2006478c <register_string>
20060112:	4a01                	c.li	s4,0
20060114:	23440c13          	addi	s8,s0,564
20060118:	02000b93          	addi	s7,zero,32
2006011c:	002a1793          	slli	a5,s4,0x2
20060120:	97d6                	c.add	a5,s5
20060122:	000b2703          	lw	a4,0(s6)
20060126:	439c                	c.lw	a5,0(a5)
20060128:	86d2                	c.mv	a3,s4
2006012a:	13400613          	addi	a2,zero,308
2006012e:	62848593          	addi	a1,s1,1576
20060132:	8562                	c.mv	a0,s8
20060134:	0a05                	c.addi	s4,1
20060136:	a50ff0ef          	jal	ra,2005f386 <DiagPrintf>
2006013a:	0b11                	c.addi	s6,4
2006013c:	ff7a10e3          	bne	s4,s7,2006011c <crash_dump+0xf6>
20060140:	15a00613          	addi	a2,zero,346
20060144:	62848593          	addi	a1,s1,1576
20060148:	26440513          	addi	a0,s0,612
2006014c:	a3aff0ef          	jal	ra,2005f386 <DiagPrintf>
20060150:	4629                	c.li	a2,10
20060152:	85ca                	c.mv	a1,s2
20060154:	854e                	c.mv	a0,s3
20060156:	e27ff0ef          	jal	ra,2005ff7c <do_stack_backtrace>
2006015a:	e921                	c.bnez	a0,200601aa <crash_dump+0x184>
2006015c:	40000593          	addi	a1,zero,1024
20060160:	850a                	c.mv	a0,sp
20060162:	c24e                	c.swsp	s3,4(sp)
20060164:	ca4e                	c.swsp	s3,20(sp)
20060166:	c402                	c.swsp	zero,8(sp)
20060168:	c602                	c.swsp	zero,12(sp)
2006016a:	c04a                	c.swsp	s2,0(sp)
2006016c:	cc02                	c.swsp	zero,24(sp)
2006016e:	ce02                	c.swsp	zero,28(sp)
20060170:	c84a                	c.swsp	s2,16(sp)
20060172:	fb8ff0ef          	jal	ra,2005f92a <unwind_frame_rv32i>
20060176:	40000593          	addi	a1,zero,1024
2006017a:	0808                	c.addi4spn	a0,sp,16
2006017c:	a85ff0ef          	jal	ra,2005fc00 <unwind_frame_rv32c>
20060180:	46b2                	c.lwsp	a3,12(sp)
20060182:	4772                	c.lwsp	a4,28(sp)
20060184:	89b6                	c.mv	s3,a3
20060186:	e291                	c.bnez	a3,2006018a <crash_dump+0x164>
20060188:	89ba                	c.mv	s3,a4
2006018a:	16c00613          	addi	a2,zero,364
2006018e:	62848593          	addi	a1,s1,1576
20060192:	2a440513          	addi	a0,s0,676
20060196:	9f0ff0ef          	jal	ra,2005f386 <DiagPrintf>
2006019a:	004aa503          	lw	a0,4(s5)
2006019e:	ffc9f593          	andi	a1,s3,-4
200601a2:	4629                	c.li	a2,10
200601a4:	95ca                	c.add	a1,s2
200601a6:	dd7ff0ef          	jal	ra,2005ff7c <do_stack_backtrace>
200601aa:	62848593          	addi	a1,s1,1576
200601ae:	17000613          	addi	a2,zero,368
200601b2:	30c40513          	addi	a0,s0,780
200601b6:	9d0ff0ef          	jal	ra,2005f386 <DiagPrintf>
200601ba:	62848593          	addi	a1,s1,1576
200601be:	18300613          	addi	a2,zero,387
200601c2:	35440513          	addi	a0,s0,852
200601c6:	9c0ff0ef          	jal	ra,2005f386 <DiagPrintf>
200601ca:	39c40513          	addi	a0,s0,924
200601ce:	4466                	c.lwsp	s0,88(sp)
200601d0:	40f6                	c.lwsp	ra,92(sp)
200601d2:	4946                	c.lwsp	s2,80(sp)
200601d4:	49b6                	c.lwsp	s3,76(sp)
200601d6:	4a26                	c.lwsp	s4,72(sp)
200601d8:	4a96                	c.lwsp	s5,68(sp)
200601da:	4b06                	c.lwsp	s6,64(sp)
200601dc:	5bf2                	c.lwsp	s7,60(sp)
200601de:	5c62                	c.lwsp	s8,56(sp)
200601e0:	5cd2                	c.lwsp	s9,52(sp)
200601e2:	5d42                	c.lwsp	s10,48(sp)
200601e4:	5db2                	c.lwsp	s11,44(sp)
200601e6:	62848593          	addi	a1,s1,1576
200601ea:	44d6                	c.lwsp	s1,84(sp)
200601ec:	18400613          	addi	a2,zero,388
200601f0:	6125                	c.addi16sp	sp,96
200601f2:	994ff06f          	jal	zero,2005f386 <DiagPrintf>
200601f6:	20064737          	lui	a4,0x20064
200601fa:	002a1793          	slli	a5,s4,0x2
200601fe:	70c70713          	addi	a4,a4,1804 # 2006470c <exception_cause_string_tbl>
20060202:	97ba                	c.add	a5,a4
20060204:	0007ab03          	lw	s6,0(a5)
20060208:	ea0b05e3          	beq	s6,zero,200600b2 <crash_dump+0x8c>
2006020c:	11d00613          	addi	a2,zero,285
20060210:	62848593          	addi	a1,s1,1576
20060214:	1a840513          	addi	a0,s0,424
20060218:	96eff0ef          	jal	ra,2005f386 <DiagPrintf>
2006021c:	21040d13          	addi	s10,s0,528
20060220:	1e040d93          	addi	s11,s0,480
20060224:	000b2683          	lw	a3,0(s6)
20060228:	e80685e3          	beq	a3,zero,200600b2 <crash_dump+0x8c>
2006022c:	004b2703          	lw	a4,4(s6)
20060230:	cb19                	c.beqz	a4,20060246 <crash_dump+0x220>
20060232:	87de                	c.mv	a5,s7
20060234:	12000613          	addi	a2,zero,288
20060238:	62848593          	addi	a1,s1,1576
2006023c:	856e                	c.mv	a0,s11
2006023e:	948ff0ef          	jal	ra,2005f386 <DiagPrintf>
20060242:	0b21                	c.addi	s6,8
20060244:	b7c5                	c.j	20060224 <crash_dump+0x1fe>
20060246:	12300613          	addi	a2,zero,291
2006024a:	62848593          	addi	a1,s1,1576
2006024e:	856a                	c.mv	a0,s10
20060250:	936ff0ef          	jal	ra,2005f386 <DiagPrintf>
20060254:	b7fd                	c.j	20060242 <crash_dump+0x21c>

20060256 <_my_random>:
20060256:	1141                	c.addi	sp,-16
20060258:	852e                	c.mv	a0,a1
2006025a:	85b2                	c.mv	a1,a2
2006025c:	c606                	c.swsp	ra,12(sp)
2006025e:	db6ff0ef          	jal	ra,2005f814 <TRNG_get_random_bytes>
20060262:	40b2                	c.lwsp	ra,12(sp)
20060264:	4501                	c.li	a0,0
20060266:	0141                	c.addi	sp,16
20060268:	8082                	c.jr	ra

2006026a <_my_free>:
2006026a:	cb4fb06f          	jal	zero,2005b71e <rtos_mem_free>

2006026e <_my_calloc>:
2006026e:	02b50533          	mul	a0,a0,a1
20060272:	c86fb06f          	jal	zero,2005b6f8 <rtos_mem_zmalloc>

20060276 <wifi_set_rom2flash>:
20060276:	200697b7          	lui	a5,0x20069
2006027a:	99c78793          	addi	a5,a5,-1636 # 2006899c <wifi_rom_func_map>
2006027e:	fef1a623          	sw	a5,-20(gp) # 20066974 <p_wifi_rom_func_map>
20060282:	20060737          	lui	a4,0x20060
20060286:	26e70713          	addi	a4,a4,622 # 2006026e <_my_calloc>
2006028a:	c398                	c.sw	a4,0(a5)
2006028c:	20060737          	lui	a4,0x20060
20060290:	26a70713          	addi	a4,a4,618 # 2006026a <_my_free>
20060294:	c3d8                	c.sw	a4,4(a5)
20060296:	20060737          	lui	a4,0x20060
2006029a:	25670713          	addi	a4,a4,598 # 20060256 <_my_random>
2006029e:	c798                	c.sw	a4,8(a5)
200602a0:	200697b7          	lui	a5,0x20069
200602a4:	82078793          	addi	a5,a5,-2016 # 20068820 <wifi_rom2flash>
200602a8:	fef1a423          	sw	a5,-24(gp) # 20066970 <p_wifi_rom2flash>
200602ac:	2005c737          	lui	a4,0x2005c
200602b0:	83470713          	addi	a4,a4,-1996 # 2005b834 <rtos_mutex_give>
200602b4:	c3d8                	c.sw	a4,4(a5)
200602b6:	2005b737          	lui	a4,0x2005b
200602ba:	7e870713          	addi	a4,a4,2024 # 2005b7e8 <rtos_mutex_take>
200602be:	c398                	c.sw	a4,0(a5)
200602c0:	2005b737          	lui	a4,0x2005b
200602c4:	7ca70713          	addi	a4,a4,1994 # 2005b7ca <rtos_mutex_delete_static>
200602c8:	d798                	c.sw	a4,40(a5)
200602ca:	2005b737          	lui	a4,0x2005b
200602ce:	75070713          	addi	a4,a4,1872 # 2005b750 <rtos_mutex_create_static>
200602d2:	d7d8                	c.sw	a4,44(a5)
200602d4:	2005c737          	lui	a4,0x2005c
200602d8:	96e70713          	addi	a4,a4,-1682 # 2005b96e <rtos_sema_give>
200602dc:	c7d8                	c.sw	a4,12(a5)
200602de:	2005c737          	lui	a4,0x2005c
200602e2:	92270713          	addi	a4,a4,-1758 # 2005b922 <rtos_sema_take>
200602e6:	cb98                	c.sw	a4,16(a5)
200602e8:	2005b737          	lui	a4,0x2005b
200602ec:	6a870713          	addi	a4,a4,1704 # 2005b6a8 <rtos_critical_enter>
200602f0:	cbd8                	c.sw	a4,20(a5)
200602f2:	2005b737          	lui	a4,0x2005b
200602f6:	6cc70713          	addi	a4,a4,1740 # 2005b6cc <rtos_critical_exit>
200602fa:	cf98                	c.sw	a4,24(a5)
200602fc:	2005c737          	lui	a4,0x2005c
20060300:	bf870713          	addi	a4,a4,-1032 # 2005bbf8 <rtos_time_delay_ms>
20060304:	d3d8                	c.sw	a4,36(a5)
20060306:	2005c737          	lui	a4,0x2005c
2006030a:	c3270713          	addi	a4,a4,-974 # 2005bc32 <rtos_time_get_current_system_time_ms>
2006030e:	c798                	c.sw	a4,8(a5)
20060310:	20061737          	lui	a4,0x20061
20060314:	39670713          	addi	a4,a4,918 # 20061396 <rtw_init_timer>
20060318:	d7b8                	c.sw	a4,104(a5)
2006031a:	20061737          	lui	a4,0x20061
2006031e:	3a070713          	addi	a4,a4,928 # 200613a0 <rtw_set_timer>
20060322:	d7f8                	c.sw	a4,108(a5)
20060324:	20061737          	lui	a4,0x20061
20060328:	3a470713          	addi	a4,a4,932 # 200613a4 <rtw_cancel_timer>
2006032c:	dbb8                	c.sw	a4,112(a5)
2006032e:	20061737          	lui	a4,0x20061
20060332:	3b470713          	addi	a4,a4,948 # 200613b4 <rtw_del_timer>
20060336:	dbf8                	c.sw	a4,116(a5)
20060338:	20059737          	lui	a4,0x20059
2006033c:	71a70713          	addi	a4,a4,1818 # 2005971a <pmu_set_sysactive_time>
20060340:	dff8                	c.sw	a4,124(a5)
20060342:	20061737          	lui	a4,0x20061
20060346:	3ec70713          	addi	a4,a4,1004 # 200613ec <rtw_wakelock_timeout>
2006034a:	08e7a023          	sw	a4,128(a5)
2006034e:	20061737          	lui	a4,0x20061
20060352:	3b870713          	addi	a4,a4,952 # 200613b8 <rtw_acquire_wakelock>
20060356:	db98                	c.sw	a4,48(a5)
20060358:	20061737          	lui	a4,0x20061
2006035c:	3d270713          	addi	a4,a4,978 # 200613d2 <rtw_release_wakelock>
20060360:	dbd8                	c.sw	a4,52(a5)
20060362:	8082                	c.jr	ra

20060364 <wifi_on>:
20060364:	1101                	c.addi	sp,-32
20060366:	ce06                	c.swsp	ra,28(sp)
20060368:	cc22                	c.swsp	s0,24(sp)
2006036a:	ca26                	c.swsp	s1,20(sp)
2006036c:	842a                	c.mv	s0,a0
2006036e:	c84a                	c.swsp	s2,16(sp)
20060370:	8f3f90ef          	jal	ra,20059c62 <wifi_set_user_config>
20060374:	39d000ef          	jal	ra,20060f10 <inic_msg_q_init>
20060378:	43818793          	addi	a5,gp,1080 # 20066dc0 <wifi_user_config>
2006037c:	7c102773          	csrrs	a4,0x7c1,zero
20060380:	10077713          	andi	a4,a4,256
20060384:	cb15                	c.beqz	a4,200603b8 <wifi_on+0x54>
20060386:	0ff0000f          	fence	iorw,iorw
2006038a:	04f7f00b          	cache	dwb,(a5)
2006038e:	02078713          	addi	a4,a5,32
20060392:	04e7700b          	cache	dwb,(a4)
20060396:	04078713          	addi	a4,a5,64
2006039a:	04e7700b          	cache	dwb,(a4)
2006039e:	0ff0000f          	fence	iorw,iorw
200603a2:	0001                	c.addi	zero,0
200603a4:	0001                	c.addi	zero,0
200603a6:	0001                	c.addi	zero,0
200603a8:	0001                	c.addi	zero,0
200603aa:	0001                	c.addi	zero,0
200603ac:	0001                	c.addi	zero,0
200603ae:	0001                	c.addi	zero,0
200603b0:	0001                	c.addi	zero,0
200603b2:	0001                	c.addi	zero,0
200603b4:	0001                	c.addi	zero,0
200603b6:	0001                	c.addi	zero,0
200603b8:	4605                	c.li	a2,1
200603ba:	006c                	c.addi4spn	a1,sp,12
200603bc:	451d                	c.li	a0,7
200603be:	c63e                	c.swsp	a5,12(sp)
200603c0:	292d                	c.jal	200607fa <inic_api_host_message_send>
200603c2:	059000ef          	jal	ra,20060c1a <inic_host_init_skb>
200603c6:	4605                	c.li	a2,1
200603c8:	006c                	c.addi4spn	a1,sp,12
200603ca:	4525                	c.li	a0,9
200603cc:	c622                	c.swsp	s0,12(sp)
200603ce:	2135                	c.jal	200607fa <inic_api_host_message_send>
200603d0:	fcc18793          	addi	a5,gp,-52 # 20066954 <wifi_boot.0>
200603d4:	4398                	c.lw	a4,0(a5)
200603d6:	84aa                	c.mv	s1,a0
200603d8:	ef01                	c.bnez	a4,200603f0 <wifi_on+0x8c>
200603da:	4905                	c.li	s2,1
200603dc:	0127a023          	sw	s2,0(a5)
200603e0:	5b9000ef          	jal	ra,20061198 <init_timer_wrapper>
200603e4:	fb81a783          	lw	a5,-72(gp) # 20066940 <p_wifi_do_fast_connect>
200603e8:	c781                	c.beqz	a5,200603f0 <wifi_on+0x8c>
200603ea:	01241363          	bne	s0,s2,200603f0 <wifi_on+0x8c>
200603ee:	9782                	c.jalr	a5
200603f0:	40f2                	c.lwsp	ra,28(sp)
200603f2:	4462                	c.lwsp	s0,24(sp)
200603f4:	4942                	c.lwsp	s2,16(sp)
200603f6:	8526                	c.mv	a0,s1
200603f8:	44d2                	c.lwsp	s1,20(sp)
200603fa:	6105                	c.addi16sp	sp,32
200603fc:	8082                	c.jr	ra

200603fe <wifi_config_autoreconnect>:
200603fe:	557d                	c.li	a0,-1
20060400:	8082                	c.jr	ra

20060402 <_inic_ipc_ip_addr_update_in_wowlan>:
20060402:	4505                	c.li	a0,1
20060404:	8082                	c.jr	ra

20060406 <inic_api_host_task_h>:
20060406:	7179                	c.addi16sp	sp,-48
20060408:	d226                	c.swsp	s1,36(sp)
2006040a:	200654b7          	lui	s1,0x20065
2006040e:	d04a                	c.swsp	s2,32(sp)
20060410:	ce4e                	c.swsp	s3,28(sp)
20060412:	cc52                	c.swsp	s4,24(sp)
20060414:	ca56                	c.swsp	s5,20(sp)
20060416:	c85a                	c.swsp	s6,16(sp)
20060418:	c65e                	c.swsp	s7,12(sp)
2006041a:	d606                	c.swsp	ra,44(sp)
2006041c:	d422                	c.swsp	s0,40(sp)
2006041e:	20065a37          	lui	s4,0x20065
20060422:	84048493          	addi	s1,s1,-1984 # 20064840 <register_string+0xb4>
20060426:	fd81a503          	lw	a0,-40(gp) # 20066960 <g_host_inic_api_task_wake_sema>
2006042a:	55fd                	c.li	a1,-1
2006042c:	cf6fb0ef          	jal	ra,2005b922 <rtos_sema_take>
20060430:	459d                	c.li	a1,7
20060432:	4541                	c.li	a0,16
20060434:	cbdf80ef          	jal	ra,200590f0 <ipc_get_message>
20060438:	4140                	c.lw	s0,4(a0)
2006043a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006043e:	1007f793          	andi	a5,a5,256
20060442:	c3b9                	c.beqz	a5,20060488 <inic_api_host_task_h+0x82>
20060444:	01f47713          	andi	a4,s0,31
20060448:	cb21                	c.beqz	a4,20060498 <inic_api_host_task_h+0x92>
2006044a:	04070693          	addi	a3,a4,64
2006044e:	fe047793          	andi	a5,s0,-32
20060452:	01f7f713          	andi	a4,a5,31
20060456:	9736                	c.add	a4,a3
20060458:	0ff0000f          	fence	iorw,iorw
2006045c:	973e                	c.add	a4,a5
2006045e:	00f7f00b          	cache	dinv,(a5)
20060462:	02078793          	addi	a5,a5,32
20060466:	40f706b3          	sub	a3,a4,a5
2006046a:	fed04ae3          	blt	zero,a3,2006045e <inic_api_host_task_h+0x58>
2006046e:	0ff0000f          	fence	iorw,iorw
20060472:	0001                	c.addi	zero,0
20060474:	0001                	c.addi	zero,0
20060476:	0001                	c.addi	zero,0
20060478:	0001                	c.addi	zero,0
2006047a:	0001                	c.addi	zero,0
2006047c:	0001                	c.addi	zero,0
2006047e:	0001                	c.addi	zero,0
20060480:	0001                	c.addi	zero,0
20060482:	0001                	c.addi	zero,0
20060484:	0001                	c.addi	zero,0
20060486:	0001                	c.addi	zero,0
20060488:	ec01                	c.bnez	s0,200604a0 <inic_api_host_task_h+0x9a>
2006048a:	20065537          	lui	a0,0x20065
2006048e:	80c50513          	addi	a0,a0,-2036 # 2006480c <register_string+0x80>
20060492:	f15fe0ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20060496:	bf41                	c.j	20060426 <inic_api_host_task_h+0x20>
20060498:	87a2                	c.mv	a5,s0
2006049a:	04000693          	addi	a3,zero,64
2006049e:	bf55                	c.j	20060452 <inic_api_host_task_h+0x4c>
200604a0:	400c                	c.lw	a1,0(s0)
200604a2:	4721                	c.li	a4,8
200604a4:	fff58793          	addi	a5,a1,-1
200604a8:	34f76063          	bltu	a4,a5,200607e8 <inic_api_host_task_h+0x3e2>
200604ac:	078a                	c.slli	a5,0x2
200604ae:	97a6                	c.add	a5,s1
200604b0:	439c                	c.lw	a5,0(a5)
200604b2:	8782                	c.jr	a5
200604b4:	fc81a783          	lw	a5,-56(gp) # 20066950 <scan_user_callback_ptr>
200604b8:	c3ad                	c.beqz	a5,2006051a <inic_api_host_task_h+0x114>
200604ba:	440c                	c.lw	a1,8(s0)
200604bc:	4048                	c.lw	a0,4(s0)
200604be:	9782                	c.jalr	a5
200604c0:	a8a9                	c.j	2006051a <inic_api_host_task_h+0x114>
200604c2:	4048                	c.lw	a0,4(s0)
200604c4:	440c                	c.lw	a1,8(s0)
200604c6:	7c1027f3          	csrrs	a5,0x7c1,zero
200604ca:	1007f793          	andi	a5,a5,256
200604ce:	c3b9                	c.beqz	a5,20060514 <inic_api_host_task_h+0x10e>
200604d0:	01f57713          	andi	a4,a0,31
200604d4:	cf59                	c.beqz	a4,20060572 <inic_api_host_task_h+0x16c>
200604d6:	03e70693          	addi	a3,a4,62
200604da:	fe057793          	andi	a5,a0,-32
200604de:	01f7f713          	andi	a4,a5,31
200604e2:	9736                	c.add	a4,a3
200604e4:	0ff0000f          	fence	iorw,iorw
200604e8:	973e                	c.add	a4,a5
200604ea:	00f7f00b          	cache	dinv,(a5)
200604ee:	02078793          	addi	a5,a5,32
200604f2:	40f706b3          	sub	a3,a4,a5
200604f6:	fed04ae3          	blt	zero,a3,200604ea <inic_api_host_task_h+0xe4>
200604fa:	0ff0000f          	fence	iorw,iorw
200604fe:	0001                	c.addi	zero,0
20060500:	0001                	c.addi	zero,0
20060502:	0001                	c.addi	zero,0
20060504:	0001                	c.addi	zero,0
20060506:	0001                	c.addi	zero,0
20060508:	0001                	c.addi	zero,0
2006050a:	0001                	c.addi	zero,0
2006050c:	0001                	c.addi	zero,0
2006050e:	0001                	c.addi	zero,0
20060510:	0001                	c.addi	zero,0
20060512:	0001                	c.addi	zero,0
20060514:	fc41a783          	lw	a5,-60(gp) # 2006694c <scan_each_report_user_callback_ptr>
20060518:	f3dd                	c.bnez	a5,200604be <inic_api_host_task_h+0xb8>
2006051a:	00042023          	sw	zero,0(s0)
2006051e:	7c1027f3          	csrrs	a5,0x7c1,zero
20060522:	1007f793          	andi	a5,a5,256
20060526:	f00780e3          	beq	a5,zero,20060426 <inic_api_host_task_h+0x20>
2006052a:	01f47793          	andi	a5,s0,31
2006052e:	04000713          	addi	a4,zero,64
20060532:	c781                	c.beqz	a5,2006053a <inic_api_host_task_h+0x134>
20060534:	04078713          	addi	a4,a5,64
20060538:	9801                	c.andi	s0,-32
2006053a:	01f47793          	andi	a5,s0,31
2006053e:	97ba                	c.add	a5,a4
20060540:	0ff0000f          	fence	iorw,iorw
20060544:	97a2                	c.add	a5,s0
20060546:	0484700b          	cache	dwb,(s0)
2006054a:	02040413          	addi	s0,s0,32
2006054e:	40878733          	sub	a4,a5,s0
20060552:	fee04ae3          	blt	zero,a4,20060546 <inic_api_host_task_h+0x140>
20060556:	0ff0000f          	fence	iorw,iorw
2006055a:	0001                	c.addi	zero,0
2006055c:	0001                	c.addi	zero,0
2006055e:	0001                	c.addi	zero,0
20060560:	0001                	c.addi	zero,0
20060562:	0001                	c.addi	zero,0
20060564:	0001                	c.addi	zero,0
20060566:	0001                	c.addi	zero,0
20060568:	0001                	c.addi	zero,0
2006056a:	0001                	c.addi	zero,0
2006056c:	0001                	c.addi	zero,0
2006056e:	0001                	c.addi	zero,0
20060570:	bd5d                	c.j	20060426 <inic_api_host_task_h+0x20>
20060572:	87aa                	c.mv	a5,a0
20060574:	03e00693          	addi	a3,zero,62
20060578:	b79d                	c.j	200604de <inic_api_host_task_h+0xd8>
2006057a:	4418                	c.lw	a4,8(s0)
2006057c:	4448                	c.lw	a0,12(s0)
2006057e:	481c                	c.lw	a5,16(s0)
20060580:	4850                	c.lw	a2,20(s0)
20060582:	5014                	c.lw	a3,32(s0)
20060584:	7c1025f3          	csrrs	a1,0x7c1,zero
20060588:	1005f593          	andi	a1,a1,256
2006058c:	c58d                	c.beqz	a1,200605b6 <inic_api_host_task_h+0x1b0>
2006058e:	00a775b3          	and	a1,a4,a0
20060592:	587d                	c.li	a6,-1
20060594:	0b059363          	bne	a1,a6,2006063a <inic_api_host_task_h+0x234>
20060598:	7c016773          	csrrsi	a4,0x7c0,2
2006059c:	0ff0000f          	fence	iorw,iorw
200605a0:	0001                	c.addi	zero,0
200605a2:	0001                	c.addi	zero,0
200605a4:	0001                	c.addi	zero,0
200605a6:	0001                	c.addi	zero,0
200605a8:	0001                	c.addi	zero,0
200605aa:	0001                	c.addi	zero,0
200605ac:	0001                	c.addi	zero,0
200605ae:	0001                	c.addi	zero,0
200605b0:	0001                	c.addi	zero,0
200605b2:	0001                	c.addi	zero,0
200605b4:	0001                	c.addi	zero,0
200605b6:	7c102773          	csrrs	a4,0x7c1,zero
200605ba:	10077713          	andi	a4,a4,256
200605be:	c70d                	c.beqz	a4,200605e8 <inic_api_host_task_h+0x1e2>
200605c0:	00c7f733          	and	a4,a5,a2
200605c4:	55fd                	c.li	a1,-1
200605c6:	0ab71063          	bne	a4,a1,20060666 <inic_api_host_task_h+0x260>
200605ca:	7c0167f3          	csrrsi	a5,0x7c0,2
200605ce:	0ff0000f          	fence	iorw,iorw
200605d2:	0001                	c.addi	zero,0
200605d4:	0001                	c.addi	zero,0
200605d6:	0001                	c.addi	zero,0
200605d8:	0001                	c.addi	zero,0
200605da:	0001                	c.addi	zero,0
200605dc:	0001                	c.addi	zero,0
200605de:	0001                	c.addi	zero,0
200605e0:	0001                	c.addi	zero,0
200605e2:	0001                	c.addi	zero,0
200605e4:	0001                	c.addi	zero,0
200605e6:	0001                	c.addi	zero,0
200605e8:	7c1027f3          	csrrs	a5,0x7c1,zero
200605ec:	1007f793          	andi	a5,a5,256
200605f0:	d78d                	c.beqz	a5,2006051a <inic_api_host_task_h+0x114>
200605f2:	01f6f713          	andi	a4,a3,31
200605f6:	4799                	c.li	a5,6
200605f8:	c701                	c.beqz	a4,20060600 <inic_api_host_task_h+0x1fa>
200605fa:	00670793          	addi	a5,a4,6
200605fe:	9a81                	c.andi	a3,-32
20060600:	01f6f713          	andi	a4,a3,31
20060604:	973e                	c.add	a4,a5
20060606:	0ff0000f          	fence	iorw,iorw
2006060a:	87b6                	c.mv	a5,a3
2006060c:	9736                	c.add	a4,a3
2006060e:	00f7f00b          	cache	dinv,(a5)
20060612:	02078793          	addi	a5,a5,32
20060616:	40f706b3          	sub	a3,a4,a5
2006061a:	fed04ae3          	blt	zero,a3,2006060e <inic_api_host_task_h+0x208>
2006061e:	0ff0000f          	fence	iorw,iorw
20060622:	0001                	c.addi	zero,0
20060624:	0001                	c.addi	zero,0
20060626:	0001                	c.addi	zero,0
20060628:	0001                	c.addi	zero,0
2006062a:	0001                	c.addi	zero,0
2006062c:	0001                	c.addi	zero,0
2006062e:	0001                	c.addi	zero,0
20060630:	0001                	c.addi	zero,0
20060632:	0001                	c.addi	zero,0
20060634:	0001                	c.addi	zero,0
20060636:	0001                	c.addi	zero,0
20060638:	b5cd                	c.j	2006051a <inic_api_host_task_h+0x114>
2006063a:	01f77593          	andi	a1,a4,31
2006063e:	c199                	c.beqz	a1,20060644 <inic_api_host_task_h+0x23e>
20060640:	9b01                	c.andi	a4,-32
20060642:	952e                	c.add	a0,a1
20060644:	f6a059e3          	bge	zero,a0,200605b6 <inic_api_host_task_h+0x1b0>
20060648:	01f77593          	andi	a1,a4,31
2006064c:	95aa                	c.add	a1,a0
2006064e:	0ff0000f          	fence	iorw,iorw
20060652:	95ba                	c.add	a1,a4
20060654:	00e7700b          	cache	dinv,(a4)
20060658:	02070713          	addi	a4,a4,32
2006065c:	40e58533          	sub	a0,a1,a4
20060660:	fea04ae3          	blt	zero,a0,20060654 <inic_api_host_task_h+0x24e>
20060664:	bf25                	c.j	2006059c <inic_api_host_task_h+0x196>
20060666:	01f7f713          	andi	a4,a5,31
2006066a:	c319                	c.beqz	a4,20060670 <inic_api_host_task_h+0x26a>
2006066c:	9b81                	c.andi	a5,-32
2006066e:	963a                	c.add	a2,a4
20060670:	f6c05ce3          	bge	zero,a2,200605e8 <inic_api_host_task_h+0x1e2>
20060674:	01f7f713          	andi	a4,a5,31
20060678:	9732                	c.add	a4,a2
2006067a:	0ff0000f          	fence	iorw,iorw
2006067e:	973e                	c.add	a4,a5
20060680:	00f7f00b          	cache	dinv,(a5)
20060684:	02078793          	addi	a5,a5,32
20060688:	40f70633          	sub	a2,a4,a5
2006068c:	fec04ae3          	blt	zero,a2,20060680 <inic_api_host_task_h+0x27a>
20060690:	bf3d                	c.j	200605ce <inic_api_host_task_h+0x1c8>
20060692:	fd01a783          	lw	a5,-48(gp) # 20066958 <p_ap_channel_switch_callback>
20060696:	e80782e3          	beq	a5,zero,2006051a <inic_api_host_task_h+0x114>
2006069a:	00840583          	lb	a1,8(s0)
2006069e:	00444503          	lbu	a0,4(s0)
200606a2:	9782                	c.jalr	a5
200606a4:	bd9d                	c.j	2006051a <inic_api_host_task_h+0x114>
200606a6:	440c                	c.lw	a1,8(s0)
200606a8:	4450                	c.lw	a2,12(s0)
200606aa:	00444503          	lbu	a0,4(s0)
200606ae:	4814                	c.lw	a3,16(s0)
200606b0:	87ae                	c.mv	a5,a1
200606b2:	8832                	c.mv	a6,a2
200606b4:	7c102773          	csrrs	a4,0x7c1,zero
200606b8:	10077713          	andi	a4,a4,256
200606bc:	c70d                	c.beqz	a4,200606e6 <inic_api_host_task_h+0x2e0>
200606be:	00c5f733          	and	a4,a1,a2
200606c2:	58fd                	c.li	a7,-1
200606c4:	03171463          	bne	a4,a7,200606ec <inic_api_host_task_h+0x2e6>
200606c8:	7c0167f3          	csrrsi	a5,0x7c0,2
200606cc:	0ff0000f          	fence	iorw,iorw
200606d0:	0001                	c.addi	zero,0
200606d2:	0001                	c.addi	zero,0
200606d4:	0001                	c.addi	zero,0
200606d6:	0001                	c.addi	zero,0
200606d8:	0001                	c.addi	zero,0
200606da:	0001                	c.addi	zero,0
200606dc:	0001                	c.addi	zero,0
200606de:	0001                	c.addi	zero,0
200606e0:	0001                	c.addi	zero,0
200606e2:	0001                	c.addi	zero,0
200606e4:	0001                	c.addi	zero,0
200606e6:	accfe0ef          	jal	ra,2005e9b2 <wifi_indication>
200606ea:	bd05                	c.j	2006051a <inic_api_host_task_h+0x114>
200606ec:	01f5f713          	andi	a4,a1,31
200606f0:	c709                	c.beqz	a4,200606fa <inic_api_host_task_h+0x2f4>
200606f2:	fe05f793          	andi	a5,a1,-32
200606f6:	00e60833          	add	a6,a2,a4
200606fa:	ff0056e3          	bge	zero,a6,200606e6 <inic_api_host_task_h+0x2e0>
200606fe:	01f7f713          	andi	a4,a5,31
20060702:	9742                	c.add	a4,a6
20060704:	0ff0000f          	fence	iorw,iorw
20060708:	973e                	c.add	a4,a5
2006070a:	00f7f00b          	cache	dinv,(a5)
2006070e:	02078793          	addi	a5,a5,32
20060712:	40f70833          	sub	a6,a4,a5
20060716:	ff004ae3          	blt	zero,a6,2006070a <inic_api_host_task_h+0x304>
2006071a:	bf4d                	c.j	200606cc <inic_api_host_task_h+0x2c6>
2006071c:	fc01a783          	lw	a5,-64(gp) # 20066948 <promisc_user_callback_ptr>
20060720:	4501                	c.li	a0,0
20060722:	cbc1                	c.beqz	a5,200607b2 <inic_api_host_task_h+0x3ac>
20060724:	4058                	c.lw	a4,4(s0)
20060726:	7c1027f3          	csrrs	a5,0x7c1,zero
2006072a:	1007f793          	andi	a5,a5,256
2006072e:	c3b9                	c.beqz	a5,20060774 <inic_api_host_task_h+0x36e>
20060730:	01f77693          	andi	a3,a4,31
20060734:	c2c9                	c.beqz	a3,200607b6 <inic_api_host_task_h+0x3b0>
20060736:	01068613          	addi	a2,a3,16
2006073a:	fe077793          	andi	a5,a4,-32
2006073e:	01f7f693          	andi	a3,a5,31
20060742:	96b2                	c.add	a3,a2
20060744:	0ff0000f          	fence	iorw,iorw
20060748:	96be                	c.add	a3,a5
2006074a:	00f7f00b          	cache	dinv,(a5)
2006074e:	02078793          	addi	a5,a5,32
20060752:	40f68633          	sub	a2,a3,a5
20060756:	fec04ae3          	blt	zero,a2,2006074a <inic_api_host_task_h+0x344>
2006075a:	0ff0000f          	fence	iorw,iorw
2006075e:	0001                	c.addi	zero,0
20060760:	0001                	c.addi	zero,0
20060762:	0001                	c.addi	zero,0
20060764:	0001                	c.addi	zero,0
20060766:	0001                	c.addi	zero,0
20060768:	0001                	c.addi	zero,0
2006076a:	0001                	c.addi	zero,0
2006076c:	0001                	c.addi	zero,0
2006076e:	0001                	c.addi	zero,0
20060770:	0001                	c.addi	zero,0
20060772:	0001                	c.addi	zero,0
20060774:	471c                	c.lw	a5,8(a4)
20060776:	4754                	c.lw	a3,12(a4)
20060778:	7c102773          	csrrs	a4,0x7c1,zero
2006077c:	10077713          	andi	a4,a4,256
20060780:	c70d                	c.beqz	a4,200607aa <inic_api_host_task_h+0x3a4>
20060782:	577d                	c.li	a4,-1
20060784:	02e79c63          	bne	a5,a4,200607bc <inic_api_host_task_h+0x3b6>
20060788:	02f69a63          	bne	a3,a5,200607bc <inic_api_host_task_h+0x3b6>
2006078c:	7c0167f3          	csrrsi	a5,0x7c0,2
20060790:	0ff0000f          	fence	iorw,iorw
20060794:	0001                	c.addi	zero,0
20060796:	0001                	c.addi	zero,0
20060798:	0001                	c.addi	zero,0
2006079a:	0001                	c.addi	zero,0
2006079c:	0001                	c.addi	zero,0
2006079e:	0001                	c.addi	zero,0
200607a0:	0001                	c.addi	zero,0
200607a2:	0001                	c.addi	zero,0
200607a4:	0001                	c.addi	zero,0
200607a6:	0001                	c.addi	zero,0
200607a8:	0001                	c.addi	zero,0
200607aa:	fc01a783          	lw	a5,-64(gp) # 20066948 <promisc_user_callback_ptr>
200607ae:	4048                	c.lw	a0,4(s0)
200607b0:	9782                	c.jalr	a5
200607b2:	d048                	c.sw	a0,36(s0)
200607b4:	b39d                	c.j	2006051a <inic_api_host_task_h+0x114>
200607b6:	87ba                	c.mv	a5,a4
200607b8:	4641                	c.li	a2,16
200607ba:	b751                	c.j	2006073e <inic_api_host_task_h+0x338>
200607bc:	01f7f713          	andi	a4,a5,31
200607c0:	c319                	c.beqz	a4,200607c6 <inic_api_host_task_h+0x3c0>
200607c2:	9b81                	c.andi	a5,-32
200607c4:	96ba                	c.add	a3,a4
200607c6:	fed052e3          	bge	zero,a3,200607aa <inic_api_host_task_h+0x3a4>
200607ca:	01f7f713          	andi	a4,a5,31
200607ce:	9736                	c.add	a4,a3
200607d0:	0ff0000f          	fence	iorw,iorw
200607d4:	973e                	c.add	a4,a5
200607d6:	00f7f00b          	cache	dinv,(a5)
200607da:	02078793          	addi	a5,a5,32
200607de:	40f706b3          	sub	a3,a4,a5
200607e2:	fed04ae3          	blt	zero,a3,200607d6 <inic_api_host_task_h+0x3d0>
200607e6:	b76d                	c.j	20060790 <inic_api_host_task_h+0x38a>
200607e8:	824a0513          	addi	a0,s4,-2012 # 20064824 <register_string+0x98>
200607ec:	bbbfe0ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
200607f0:	b32d                	c.j	2006051a <inic_api_host_task_h+0x114>

200607f2 <inic_api_host_int_hdl>:
200607f2:	fd81a503          	lw	a0,-40(gp) # 20066960 <g_host_inic_api_task_wake_sema>
200607f6:	978fb06f          	jal	zero,2005b96e <rtos_sema_give>

200607fa <inic_api_host_message_send>:
200607fa:	7139                	c.addi16sp	sp,-64
200607fc:	d452                	c.swsp	s4,40(sp)
200607fe:	d256                	c.swsp	s5,36(sp)
20060800:	fca1ae23          	sw	a0,-36(gp) # 20066964 <latest_api_id>
20060804:	d05a                	c.swsp	s6,32(sp)
20060806:	8b2a                	c.mv	s6,a0
20060808:	fd41a503          	lw	a0,-44(gp) # 2006695c <g_host_inic_api_message_send_sema>
2006080c:	cc62                	c.swsp	s8,24(sp)
2006080e:	20068c37          	lui	s8,0x20068
20060812:	da26                	c.swsp	s1,52(sp)
20060814:	d64e                	c.swsp	s3,44(sp)
20060816:	ca66                	c.swsp	s9,20(sp)
20060818:	89ae                	c.mv	s3,a1
2006081a:	740c0493          	addi	s1,s8,1856 # 20068740 <g_host_ipc_api_request_info>
2006081e:	55fd                	c.li	a1,-1
20060820:	6c8d                	c.lui	s9,0x3
20060822:	ce5e                	c.swsp	s7,28(sp)
20060824:	c86a                	c.swsp	s10,16(sp)
20060826:	c66e                	c.swsp	s11,12(sp)
20060828:	de06                	c.swsp	ra,60(sp)
2006082a:	dc22                	c.swsp	s0,56(sp)
2006082c:	d84a                	c.swsp	s2,48(sp)
2006082e:	8bb2                	c.mv	s7,a2
20060830:	fdc18a93          	addi	s5,gp,-36 # 20066964 <latest_api_id>
20060834:	8eefb0ef          	jal	ra,2005b922 <rtos_sema_take>
20060838:	fd418a13          	addi	s4,gp,-44 # 2006695c <g_host_inic_api_message_send_sema>
2006083c:	02048d93          	addi	s11,s1,32
20060840:	ee0c8c93          	addi	s9,s9,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
20060844:	20065d37          	lui	s10,0x20065
20060848:	4401                	c.li	s0,0
2006084a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006084e:	1007f793          	andi	a5,a5,256
20060852:	740c0913          	addi	s2,s8,1856
20060856:	c785                	c.beqz	a5,2006087e <inic_api_host_message_send+0x84>
20060858:	0ff0000f          	fence	iorw,iorw
2006085c:	0094f00b          	cache	dinv,(s1)
20060860:	01bdf00b          	cache	dinv,(s11)
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
2006087e:	00092783          	lw	a5,0(s2)
20060882:	cf89                	c.beqz	a5,2006089c <inic_api_host_message_send+0xa2>
20060884:	4505                	c.li	a0,1
20060886:	0405                	c.addi	s0,1
20060888:	b70fb0ef          	jal	ra,2005bbf8 <rtos_time_delay_ms>
2006088c:	03946433          	rem	s0,s0,s9
20060890:	fc4d                	c.bnez	s0,2006084a <inic_api_host_message_send+0x50>
20060892:	864d0513          	addi	a0,s10,-1948 # 20064864 <register_string+0xd8>
20060896:	b11fe0ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
2006089a:	b77d                	c.j	20060848 <inic_api_host_message_send+0x4e>
2006089c:	04000613          	addi	a2,zero,64
200608a0:	4581                	c.li	a1,0
200608a2:	740c0513          	addi	a0,s8,1856
200608a6:	dffa9097          	auipc	ra,0xdffa9
200608aa:	228080e7          	jalr	ra,552(ra) # 9ace <__wrap_memset>
200608ae:	01692023          	sw	s6,0(s2)
200608b2:	00098b63          	beq	s3,zero,200608c8 <inic_api_host_message_send+0xce>
200608b6:	002b9613          	slli	a2,s7,0x2
200608ba:	85ce                	c.mv	a1,s3
200608bc:	00490513          	addi	a0,s2,4
200608c0:	dffa9097          	auipc	ra,0xdffa9
200608c4:	210080e7          	jalr	ra,528(ra) # 9ad0 <__wrap_memcpy>
200608c8:	7c1027f3          	csrrs	a5,0x7c1,zero
200608cc:	1007f793          	andi	a5,a5,256
200608d0:	c795                	c.beqz	a5,200608fc <inic_api_host_message_send+0x102>
200608d2:	0ff0000f          	fence	iorw,iorw
200608d6:	0494f00b          	cache	dwb,(s1)
200608da:	02048793          	addi	a5,s1,32
200608de:	04f7f00b          	cache	dwb,(a5)
200608e2:	0ff0000f          	fence	iorw,iorw
200608e6:	0001                	c.addi	zero,0
200608e8:	0001                	c.addi	zero,0
200608ea:	0001                	c.addi	zero,0
200608ec:	0001                	c.addi	zero,0
200608ee:	0001                	c.addi	zero,0
200608f0:	0001                	c.addi	zero,0
200608f2:	0001                	c.addi	zero,0
200608f4:	0001                	c.addi	zero,0
200608f6:	0001                	c.addi	zero,0
200608f8:	0001                	c.addi	zero,0
200608fa:	0001                	c.addi	zero,0
200608fc:	200689b7          	lui	s3,0x20068
20060900:	4641                	c.li	a2,16
20060902:	4581                	c.li	a1,0
20060904:	70098513          	addi	a0,s3,1792 # 20068700 <g_host_ipc_api_msg>
20060908:	dffa9097          	auipc	ra,0xdffa9
2006090c:	1c6080e7          	jalr	ra,454(ra) # 9ace <__wrap_memset>
20060910:	70098b13          	addi	s6,s3,1792
20060914:	04000793          	addi	a5,zero,64
20060918:	009b2223          	sw	s1,4(s6)
2006091c:	000b2023          	sw	zero,0(s6)
20060920:	00fb2423          	sw	a5,8(s6)
20060924:	7c1027f3          	csrrs	a5,0x7c1,zero
20060928:	1007f793          	andi	a5,a5,256
2006092c:	c395                	c.beqz	a5,20060950 <inic_api_host_message_send+0x156>
2006092e:	0ff0000f          	fence	iorw,iorw
20060932:	056b700b          	cache	dwb,(s6)
20060936:	0ff0000f          	fence	iorw,iorw
2006093a:	0001                	c.addi	zero,0
2006093c:	0001                	c.addi	zero,0
2006093e:	0001                	c.addi	zero,0
20060940:	0001                	c.addi	zero,0
20060942:	0001                	c.addi	zero,0
20060944:	0001                	c.addi	zero,0
20060946:	0001                	c.addi	zero,0
20060948:	0001                	c.addi	zero,0
2006094a:	0001                	c.addi	zero,0
2006094c:	0001                	c.addi	zero,0
2006094e:	0001                	c.addi	zero,0
20060950:	70098613          	addi	a2,s3,1792
20060954:	459d                	c.li	a1,7
20060956:	4501                	c.li	a0,0
20060958:	698d                	c.lui	s3,0x3
2006095a:	e4ef80ef          	jal	ra,20058fa8 <ipc_send_message>
2006095e:	02048b93          	addi	s7,s1,32
20060962:	ee098993          	addi	s3,s3,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
20060966:	20065b37          	lui	s6,0x20065
2006096a:	00092783          	lw	a5,0(s2)
2006096e:	cba9                	c.beqz	a5,200609c0 <inic_api_host_message_send+0x1c6>
20060970:	4505                	c.li	a0,1
20060972:	a86fb0ef          	jal	ra,2005bbf8 <rtos_time_delay_ms>
20060976:	7c1027f3          	csrrs	a5,0x7c1,zero
2006097a:	1007f793          	andi	a5,a5,256
2006097e:	c785                	c.beqz	a5,200609a6 <inic_api_host_message_send+0x1ac>
20060980:	0ff0000f          	fence	iorw,iorw
20060984:	0094f00b          	cache	dinv,(s1)
20060988:	017bf00b          	cache	dinv,(s7)
2006098c:	0ff0000f          	fence	iorw,iorw
20060990:	0001                	c.addi	zero,0
20060992:	0001                	c.addi	zero,0
20060994:	0001                	c.addi	zero,0
20060996:	0001                	c.addi	zero,0
20060998:	0001                	c.addi	zero,0
2006099a:	0001                	c.addi	zero,0
2006099c:	0001                	c.addi	zero,0
2006099e:	0001                	c.addi	zero,0
200609a0:	0001                	c.addi	zero,0
200609a2:	0001                	c.addi	zero,0
200609a4:	0001                	c.addi	zero,0
200609a6:	0405                	c.addi	s0,1
200609a8:	03346433          	rem	s0,s0,s3
200609ac:	fc5d                	c.bnez	s0,2006096a <inic_api_host_message_send+0x170>
200609ae:	000aa603          	lw	a2,0(s5)
200609b2:	00092583          	lw	a1,0(s2)
200609b6:	87cb0513          	addi	a0,s6,-1924 # 2006487c <register_string+0xf0>
200609ba:	9edfe0ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
200609be:	b775                	c.j	2006096a <inic_api_host_message_send+0x170>
200609c0:	000a2503          	lw	a0,0(s4)
200609c4:	02892403          	lw	s0,40(s2)
200609c8:	fa7fa0ef          	jal	ra,2005b96e <rtos_sema_give>
200609cc:	50f2                	c.lwsp	ra,60(sp)
200609ce:	8522                	c.mv	a0,s0
200609d0:	5462                	c.lwsp	s0,56(sp)
200609d2:	54d2                	c.lwsp	s1,52(sp)
200609d4:	5942                	c.lwsp	s2,48(sp)
200609d6:	59b2                	c.lwsp	s3,44(sp)
200609d8:	5a22                	c.lwsp	s4,40(sp)
200609da:	5a92                	c.lwsp	s5,36(sp)
200609dc:	5b02                	c.lwsp	s6,32(sp)
200609de:	4bf2                	c.lwsp	s7,28(sp)
200609e0:	4c62                	c.lwsp	s8,24(sp)
200609e2:	4cd2                	c.lwsp	s9,20(sp)
200609e4:	4d42                	c.lwsp	s10,16(sp)
200609e6:	4db2                	c.lwsp	s11,12(sp)
200609e8:	6121                	c.addi16sp	sp,64
200609ea:	8082                	c.jr	ra

200609ec <inic_api_init_host>:
200609ec:	1141                	c.addi	sp,-16
200609ee:	567d                	c.li	a2,-1
200609f0:	4581                	c.li	a1,0
200609f2:	fd818513          	addi	a0,gp,-40 # 20066960 <g_host_inic_api_task_wake_sema>
200609f6:	c606                	c.swsp	ra,12(sp)
200609f8:	c422                	c.swsp	s0,8(sp)
200609fa:	eabfa0ef          	jal	ra,2005b8a4 <rtos_sema_create_static>
200609fe:	567d                	c.li	a2,-1
20060a00:	4581                	c.li	a1,0
20060a02:	fd418413          	addi	s0,gp,-44 # 2006695c <g_host_inic_api_message_send_sema>
20060a06:	fd418513          	addi	a0,gp,-44 # 2006695c <g_host_inic_api_message_send_sema>
20060a0a:	e9bfa0ef          	jal	ra,2005b8a4 <rtos_sema_create_static>
20060a0e:	4008                	c.lw	a0,0(s0)
20060a10:	f5ffa0ef          	jal	ra,2005b96e <rtos_sema_give>
20060a14:	200605b7          	lui	a1,0x20060
20060a18:	4701                	c.li	a4,0
20060a1a:	4681                	c.li	a3,0
20060a1c:	4601                	c.li	a2,0
20060a1e:	40258593          	addi	a1,a1,1026 # 20060402 <_inic_ipc_ip_addr_update_in_wowlan>
20060a22:	4505                	c.li	a0,1
20060a24:	d63f80ef          	jal	ra,20059786 <pmu_register_sleep_callback>
20060a28:	6705                	c.lui	a4,0x1
20060a2a:	20060637          	lui	a2,0x20060
20060a2e:	200655b7          	lui	a1,0x20065
20060a32:	478d                	c.li	a5,3
20060a34:	ce070713          	addi	a4,a4,-800 # ce0 <CPU_ClkGet+0x1ca>
20060a38:	4681                	c.li	a3,0
20060a3a:	40660613          	addi	a2,a2,1030 # 20060406 <inic_api_host_task_h>
20060a3e:	8a858593          	addi	a1,a1,-1880 # 200648a8 <register_string+0x11c>
20060a42:	4501                	c.li	a0,0
20060a44:	97efb0ef          	jal	ra,2005bbc2 <rtos_task_create>
20060a48:	c10d                	c.beqz	a0,20060a6a <inic_api_init_host+0x7e>
20060a4a:	4422                	c.lwsp	s0,8(sp)
20060a4c:	40b2                	c.lwsp	ra,12(sp)
20060a4e:	200656b7          	lui	a3,0x20065
20060a52:	200655b7          	lui	a1,0x20065
20060a56:	8bc68693          	addi	a3,a3,-1860 # 200648bc <register_string+0x130>
20060a5a:	04500613          	addi	a2,zero,69
20060a5e:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x14c>
20060a62:	4509                	c.li	a0,2
20060a64:	0141                	c.addi	sp,16
20060a66:	f28fa06f          	jal	zero,2005b18e <rtk_log_write>
20060a6a:	40b2                	c.lwsp	ra,12(sp)
20060a6c:	4422                	c.lwsp	s0,8(sp)
20060a6e:	0141                	c.addi	sp,16
20060a70:	8082                	c.jr	ra

20060a72 <inic_mp_command>:
20060a72:	7179                	c.addi16sp	sp,-48
20060a74:	ce4e                	c.swsp	s3,28(sp)
20060a76:	89aa                	c.mv	s3,a0
20060a78:	6505                	c.lui	a0,0x1
20060a7a:	d04a                	c.swsp	s2,32(sp)
20060a7c:	cc52                	c.swsp	s4,24(sp)
20060a7e:	d606                	c.swsp	ra,44(sp)
20060a80:	d422                	c.swsp	s0,40(sp)
20060a82:	d226                	c.swsp	s1,36(sp)
20060a84:	892e                	c.mv	s2,a1
20060a86:	8a32                	c.mv	s4,a2
20060a88:	c71fa0ef          	jal	ra,2005b6f8 <rtos_mem_zmalloc>
20060a8c:	e50d                	c.bnez	a0,20060ab6 <inic_mp_command+0x44>
20060a8e:	5422                	c.lwsp	s0,40(sp)
20060a90:	50b2                	c.lwsp	ra,44(sp)
20060a92:	5492                	c.lwsp	s1,36(sp)
20060a94:	5902                	c.lwsp	s2,32(sp)
20060a96:	49f2                	c.lwsp	s3,28(sp)
20060a98:	4a62                	c.lwsp	s4,24(sp)
20060a9a:	200656b7          	lui	a3,0x20065
20060a9e:	200655b7          	lui	a1,0x20065
20060aa2:	8e068693          	addi	a3,a3,-1824 # 200648e0 <register_string+0x154>
20060aa6:	04500613          	addi	a2,zero,69
20060aaa:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x14c>
20060aae:	4509                	c.li	a0,2
20060ab0:	6145                	c.addi16sp	sp,48
20060ab2:	edcfa06f          	jal	zero,2005b18e <rtk_log_write>
20060ab6:	842a                	c.mv	s0,a0
20060ab8:	87ce                	c.mv	a5,s3
20060aba:	c04e                	c.swsp	s3,0(sp)
20060abc:	c24a                	c.swsp	s2,4(sp)
20060abe:	c452                	c.swsp	s4,8(sp)
20060ac0:	84aa                	c.mv	s1,a0
20060ac2:	c62a                	c.swsp	a0,12(sp)
20060ac4:	7c102773          	csrrs	a4,0x7c1,zero
20060ac8:	10077713          	andi	a4,a4,256
20060acc:	c721                	c.beqz	a4,20060b14 <inic_mp_command+0xa2>
20060ace:	01f57693          	andi	a3,a0,31
20060ad2:	10068363          	beq	a3,zero,20060bd8 <inic_mp_command+0x166>
20060ad6:	6605                	c.lui	a2,0x1
20060ad8:	9636                	c.add	a2,a3
20060ada:	fe057713          	andi	a4,a0,-32
20060ade:	01f77693          	andi	a3,a4,31
20060ae2:	96b2                	c.add	a3,a2
20060ae4:	0ff0000f          	fence	iorw,iorw
20060ae8:	96ba                	c.add	a3,a4
20060aea:	04e7700b          	cache	dwb,(a4)
20060aee:	02070713          	addi	a4,a4,32
20060af2:	40e68633          	sub	a2,a3,a4
20060af6:	fec04ae3          	blt	zero,a2,20060aea <inic_mp_command+0x78>
20060afa:	0ff0000f          	fence	iorw,iorw
20060afe:	0001                	c.addi	zero,0
20060b00:	0001                	c.addi	zero,0
20060b02:	0001                	c.addi	zero,0
20060b04:	0001                	c.addi	zero,0
20060b06:	0001                	c.addi	zero,0
20060b08:	0001                	c.addi	zero,0
20060b0a:	0001                	c.addi	zero,0
20060b0c:	0001                	c.addi	zero,0
20060b0e:	0001                	c.addi	zero,0
20060b10:	0001                	c.addi	zero,0
20060b12:	0001                	c.addi	zero,0
20060b14:	7c102773          	csrrs	a4,0x7c1,zero
20060b18:	10077713          	andi	a4,a4,256
20060b1c:	c70d                	c.beqz	a4,20060b46 <inic_mp_command+0xd4>
20060b1e:	577d                	c.li	a4,-1
20060b20:	0ae99f63          	bne	s3,a4,20060bde <inic_mp_command+0x16c>
20060b24:	0b391d63          	bne	s2,s3,20060bde <inic_mp_command+0x16c>
20060b28:	7c0267f3          	csrrsi	a5,0x7c0,4
20060b2c:	0ff0000f          	fence	iorw,iorw
20060b30:	0001                	c.addi	zero,0
20060b32:	0001                	c.addi	zero,0
20060b34:	0001                	c.addi	zero,0
20060b36:	0001                	c.addi	zero,0
20060b38:	0001                	c.addi	zero,0
20060b3a:	0001                	c.addi	zero,0
20060b3c:	0001                	c.addi	zero,0
20060b3e:	0001                	c.addi	zero,0
20060b40:	0001                	c.addi	zero,0
20060b42:	0001                	c.addi	zero,0
20060b44:	0001                	c.addi	zero,0
20060b46:	650d                	c.lui	a0,0x3
20060b48:	4611                	c.li	a2,4
20060b4a:	858a                	c.mv	a1,sp
20060b4c:	050d                	c.addi	a0,3
20060b4e:	cadff0ef          	jal	ra,200607fa <inic_api_host_message_send>
20060b52:	060a0863          	beq	s4,zero,20060bc2 <inic_mp_command+0x150>
20060b56:	7c1027f3          	csrrs	a5,0x7c1,zero
20060b5a:	1007f793          	andi	a5,a5,256
20060b5e:	c7a1                	c.beqz	a5,20060ba6 <inic_mp_command+0x134>
20060b60:	01f47713          	andi	a4,s0,31
20060b64:	6785                	c.lui	a5,0x1
20060b66:	c701                	c.beqz	a4,20060b6e <inic_mp_command+0xfc>
20060b68:	97ba                	c.add	a5,a4
20060b6a:	fe047493          	andi	s1,s0,-32
20060b6e:	01f4f713          	andi	a4,s1,31
20060b72:	973e                	c.add	a4,a5
20060b74:	0ff0000f          	fence	iorw,iorw
20060b78:	87a6                	c.mv	a5,s1
20060b7a:	9726                	c.add	a4,s1
20060b7c:	00f7f00b          	cache	dinv,(a5)
20060b80:	02078793          	addi	a5,a5,32 # 1020 <CPU_ClkGet+0x50a>
20060b84:	40f706b3          	sub	a3,a4,a5
20060b88:	fed04ae3          	blt	zero,a3,20060b7c <inic_mp_command+0x10a>
20060b8c:	0ff0000f          	fence	iorw,iorw
20060b90:	0001                	c.addi	zero,0
20060b92:	0001                	c.addi	zero,0
20060b94:	0001                	c.addi	zero,0
20060b96:	0001                	c.addi	zero,0
20060b98:	0001                	c.addi	zero,0
20060b9a:	0001                	c.addi	zero,0
20060b9c:	0001                	c.addi	zero,0
20060b9e:	0001                	c.addi	zero,0
20060ba0:	0001                	c.addi	zero,0
20060ba2:	0001                	c.addi	zero,0
20060ba4:	0001                	c.addi	zero,0
20060ba6:	200656b7          	lui	a3,0x20065
20060baa:	200655b7          	lui	a1,0x20065
20060bae:	8722                	c.mv	a4,s0
20060bb0:	8f868693          	addi	a3,a3,-1800 # 200648f8 <register_string+0x16c>
20060bb4:	04100613          	addi	a2,zero,65
20060bb8:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x14c>
20060bbc:	4505                	c.li	a0,1
20060bbe:	dd0fa0ef          	jal	ra,2005b18e <rtk_log_write>
20060bc2:	8522                	c.mv	a0,s0
20060bc4:	b5bfa0ef          	jal	ra,2005b71e <rtos_mem_free>
20060bc8:	50b2                	c.lwsp	ra,44(sp)
20060bca:	5422                	c.lwsp	s0,40(sp)
20060bcc:	5492                	c.lwsp	s1,36(sp)
20060bce:	5902                	c.lwsp	s2,32(sp)
20060bd0:	49f2                	c.lwsp	s3,28(sp)
20060bd2:	4a62                	c.lwsp	s4,24(sp)
20060bd4:	6145                	c.addi16sp	sp,48
20060bd6:	8082                	c.jr	ra
20060bd8:	872a                	c.mv	a4,a0
20060bda:	6605                	c.lui	a2,0x1
20060bdc:	b709                	c.j	20060ade <inic_mp_command+0x6c>
20060bde:	01f9f713          	andi	a4,s3,31
20060be2:	c701                	c.beqz	a4,20060bea <inic_mp_command+0x178>
20060be4:	fe09f793          	andi	a5,s3,-32
20060be8:	993a                	c.add	s2,a4
20060bea:	f5205ee3          	bge	zero,s2,20060b46 <inic_mp_command+0xd4>
20060bee:	01f7f593          	andi	a1,a5,31
20060bf2:	992e                	c.add	s2,a1
20060bf4:	0ff0000f          	fence	iorw,iorw
20060bf8:	993e                	c.add	s2,a5
20060bfa:	04f7f00b          	cache	dwb,(a5)
20060bfe:	02078793          	addi	a5,a5,32
20060c02:	40f90733          	sub	a4,s2,a5
20060c06:	fee04ae3          	blt	zero,a4,20060bfa <inic_mp_command+0x188>
20060c0a:	b70d                	c.j	20060b2c <inic_mp_command+0xba>

20060c0c <inic_host_init>:
20060c0c:	1141                	c.addi	sp,-16
20060c0e:	c606                	c.swsp	ra,12(sp)
20060c10:	2239                	c.jal	20060d1e <inic_host_init_priv>
20060c12:	40b2                	c.lwsp	ra,12(sp)
20060c14:	0141                	c.addi	sp,16
20060c16:	dd7ff06f          	jal	zero,200609ec <inic_api_init_host>

20060c1a <inic_host_init_skb>:
20060c1a:	1141                	c.addi	sp,-16
20060c1c:	c422                	c.swsp	s0,8(sp)
20060c1e:	fe01a783          	lw	a5,-32(gp) # 20066968 <host_skb_buff>
20060c22:	c606                	c.swsp	ra,12(sp)
20060c24:	c226                	c.swsp	s1,4(sp)
20060c26:	c395                	c.beqz	a5,20060c4a <inic_host_init_skb+0x30>
20060c28:	4422                	c.lwsp	s0,8(sp)
20060c2a:	40b2                	c.lwsp	ra,12(sp)
20060c2c:	4492                	c.lwsp	s1,4(sp)
20060c2e:	200656b7          	lui	a3,0x20065
20060c32:	200655b7          	lui	a1,0x20065
20060c36:	91068693          	addi	a3,a3,-1776 # 20064910 <register_string+0x184>
20060c3a:	04500613          	addi	a2,zero,69
20060c3e:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x14c>
20060c42:	4509                	c.li	a0,2
20060c44:	0141                	c.addi	sp,16
20060c46:	d48fa06f          	jal	zero,2005b18e <rtk_log_write>
20060c4a:	66000793          	addi	a5,zero,1632
20060c4e:	fe418713          	addi	a4,gp,-28 # 2006696c <skb_buf_max_size>
20060c52:	c31c                	c.sw	a5,0(a4)
20060c54:	43818793          	addi	a5,gp,1080 # 20066dc0 <wifi_user_config>
20060c58:	57dc                	c.lw	a5,44(a5)
20060c5a:	fe018413          	addi	s0,gp,-32 # 20066968 <host_skb_buff>
20060c5e:	43818493          	addi	s1,gp,1080 # 20066dc0 <wifi_user_config>
20060c62:	c781                	c.beqz	a5,20060c6a <inic_host_init_skb+0x50>
20060c64:	07fd                	c.addi	a5,31
20060c66:	9b81                	c.andi	a5,-32
20060c68:	c31c                	c.sw	a5,0(a4)
20060c6a:	549c                	c.lw	a5,40(s1)
20060c6c:	6c000513          	addi	a0,zero,1728
20060c70:	02f50533          	mul	a0,a0,a5
20060c74:	a85fa0ef          	jal	ra,2005b6f8 <rtos_mem_zmalloc>
20060c78:	c008                	c.sw	a0,0(s0)
20060c7a:	e115                	c.bnez	a0,20060c9e <inic_host_init_skb+0x84>
20060c7c:	20065737          	lui	a4,0x20065
20060c80:	200656b7          	lui	a3,0x20065
20060c84:	200655b7          	lui	a1,0x20065
20060c88:	95c70713          	addi	a4,a4,-1700 # 2006495c <__func__.1>
20060c8c:	92868693          	addi	a3,a3,-1752 # 20064928 <register_string+0x19c>
20060c90:	04500613          	addi	a2,zero,69
20060c94:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x14c>
20060c98:	4509                	c.li	a0,2
20060c9a:	cf4fa0ef          	jal	ra,2005b18e <rtk_log_write>
20060c9e:	4681                	c.li	a3,0
20060ca0:	6c000613          	addi	a2,zero,1728
20060ca4:	5498                	c.lw	a4,40(s1)
20060ca6:	401c                	c.lw	a5,0(s0)
20060ca8:	06e6c263          	blt	a3,a4,20060d0c <inic_host_init_skb+0xf2>
20060cac:	7c1026f3          	csrrs	a3,0x7c1,zero
20060cb0:	1006f693          	andi	a3,a3,256
20060cb4:	c6b9                	c.beqz	a3,20060d02 <inic_host_init_skb+0xe8>
20060cb6:	6c000693          	addi	a3,zero,1728
20060cba:	02d70733          	mul	a4,a4,a3
20060cbe:	01f7f693          	andi	a3,a5,31
20060cc2:	c299                	c.beqz	a3,20060cc8 <inic_host_init_skb+0xae>
20060cc4:	9b81                	c.andi	a5,-32
20060cc6:	9736                	c.add	a4,a3
20060cc8:	02e05d63          	bge	zero,a4,20060d02 <inic_host_init_skb+0xe8>
20060ccc:	01f7f693          	andi	a3,a5,31
20060cd0:	9736                	c.add	a4,a3
20060cd2:	0ff0000f          	fence	iorw,iorw
20060cd6:	973e                	c.add	a4,a5
20060cd8:	04f7f00b          	cache	dwb,(a5)
20060cdc:	02078793          	addi	a5,a5,32
20060ce0:	40f706b3          	sub	a3,a4,a5
20060ce4:	fed04ae3          	blt	zero,a3,20060cd8 <inic_host_init_skb+0xbe>
20060ce8:	0ff0000f          	fence	iorw,iorw
20060cec:	0001                	c.addi	zero,0
20060cee:	0001                	c.addi	zero,0
20060cf0:	0001                	c.addi	zero,0
20060cf2:	0001                	c.addi	zero,0
20060cf4:	0001                	c.addi	zero,0
20060cf6:	0001                	c.addi	zero,0
20060cf8:	0001                	c.addi	zero,0
20060cfa:	0001                	c.addi	zero,0
20060cfc:	0001                	c.addi	zero,0
20060cfe:	0001                	c.addi	zero,0
20060d00:	0001                	c.addi	zero,0
20060d02:	40b2                	c.lwsp	ra,12(sp)
20060d04:	4422                	c.lwsp	s0,8(sp)
20060d06:	4492                	c.lwsp	s1,4(sp)
20060d08:	0141                	c.addi	sp,16
20060d0a:	8082                	c.jr	ra
20060d0c:	02c68733          	mul	a4,a3,a2
20060d10:	0685                	c.addi	a3,1
20060d12:	97ba                	c.add	a5,a4
20060d14:	c39c                	c.sw	a5,0(a5)
20060d16:	401c                	c.lw	a5,0(s0)
20060d18:	97ba                	c.add	a5,a4
20060d1a:	c3dc                	c.sw	a5,4(a5)
20060d1c:	b761                	c.j	20060ca4 <inic_host_init_skb+0x8a>

20060d1e <inic_host_init_priv>:
20060d1e:	1141                	c.addi	sp,-16
20060d20:	c226                	c.swsp	s1,4(sp)
20060d22:	200684b7          	lui	s1,0x20068
20060d26:	02800613          	addi	a2,zero,40
20060d2a:	4581                	c.li	a1,0
20060d2c:	78048513          	addi	a0,s1,1920 # 20068780 <g_inic_host_priv>
20060d30:	c606                	c.swsp	ra,12(sp)
20060d32:	c422                	c.swsp	s0,8(sp)
20060d34:	dffa9097          	auipc	ra,0xdffa9
20060d38:	d9a080e7          	jalr	ra,-614(ra) # 9ace <__wrap_memset>
20060d3c:	567d                	c.li	a2,-1
20060d3e:	4581                	c.li	a1,0
20060d40:	78048513          	addi	a0,s1,1920
20060d44:	78048413          	addi	s0,s1,1920
20060d48:	b5dfa0ef          	jal	ra,2005b8a4 <rtos_sema_create_static>
20060d4c:	567d                	c.li	a2,-1
20060d4e:	4581                	c.li	a1,0
20060d50:	00440513          	addi	a0,s0,4
20060d54:	b51fa0ef          	jal	ra,2005b8a4 <rtos_sema_create_static>
20060d58:	567d                	c.li	a2,-1
20060d5a:	4581                	c.li	a1,0
20060d5c:	00840513          	addi	a0,s0,8
20060d60:	b45fa0ef          	jal	ra,2005b8a4 <rtos_sema_create_static>
20060d64:	4408                	c.lw	a0,8(s0)
20060d66:	c09fa0ef          	jal	ra,2005b96e <rtos_sema_give>
20060d6a:	00c40513          	addi	a0,s0,12
20060d6e:	2ea5                	c.jal	200610e6 <rtw_init_queue>
20060d70:	40b2                	c.lwsp	ra,12(sp)
20060d72:	00042c23          	sw	zero,24(s0)
20060d76:	00042e23          	sw	zero,28(s0)
20060d7a:	02042023          	sw	zero,32(s0)
20060d7e:	02042223          	sw	zero,36(s0)
20060d82:	4422                	c.lwsp	s0,8(sp)
20060d84:	4492                	c.lwsp	s1,4(sp)
20060d86:	0141                	c.addi	sp,16
20060d88:	8082                	c.jr	ra

20060d8a <inic_host_trx_event_hdl>:
20060d8a:	4791                	c.li	a5,4
20060d8c:	872a                	c.mv	a4,a0
20060d8e:	00f50f63          	beq	a0,a5,20060dac <inic_host_trx_event_hdl+0x22>
20060d92:	200656b7          	lui	a3,0x20065
20060d96:	200655b7          	lui	a1,0x20065
20060d9a:	94068693          	addi	a3,a3,-1728 # 20064940 <register_string+0x1b4>
20060d9e:	04500613          	addi	a2,zero,69
20060da2:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x14c>
20060da6:	4509                	c.li	a0,2
20060da8:	be6fa06f          	jal	zero,2005b18e <rtk_log_write>
20060dac:	8082                	c.jr	ra

20060dae <inic_host_trx_int_hdl>:
20060dae:	1141                	c.addi	sp,-16
20060db0:	4599                	c.li	a1,6
20060db2:	4541                	c.li	a0,16
20060db4:	c422                	c.swsp	s0,8(sp)
20060db6:	c606                	c.swsp	ra,12(sp)
20060db8:	b38f80ef          	jal	ra,200590f0 <ipc_get_message>
20060dbc:	4140                	c.lw	s0,4(a0)
20060dbe:	7c1027f3          	csrrs	a5,0x7c1,zero
20060dc2:	1007f793          	andi	a5,a5,256
20060dc6:	c3b9                	c.beqz	a5,20060e0c <inic_host_trx_int_hdl+0x5e>
20060dc8:	01f47713          	andi	a4,s0,31
20060dcc:	c755                	c.beqz	a4,20060e78 <inic_host_trx_int_hdl+0xca>
20060dce:	04070693          	addi	a3,a4,64
20060dd2:	fe047793          	andi	a5,s0,-32
20060dd6:	01f7f713          	andi	a4,a5,31
20060dda:	9736                	c.add	a4,a3
20060ddc:	0ff0000f          	fence	iorw,iorw
20060de0:	973e                	c.add	a4,a5
20060de2:	00f7f00b          	cache	dinv,(a5)
20060de6:	02078793          	addi	a5,a5,32
20060dea:	40f706b3          	sub	a3,a4,a5
20060dee:	fed04ae3          	blt	zero,a3,20060de2 <inic_host_trx_int_hdl+0x34>
20060df2:	0ff0000f          	fence	iorw,iorw
20060df6:	0001                	c.addi	zero,0
20060df8:	0001                	c.addi	zero,0
20060dfa:	0001                	c.addi	zero,0
20060dfc:	0001                	c.addi	zero,0
20060dfe:	0001                	c.addi	zero,0
20060e00:	0001                	c.addi	zero,0
20060e02:	0001                	c.addi	zero,0
20060e04:	0001                	c.addi	zero,0
20060e06:	0001                	c.addi	zero,0
20060e08:	0001                	c.addi	zero,0
20060e0a:	0001                	c.addi	zero,0
20060e0c:	2455                	c.jal	200610b0 <inic_msg_get_queue_status>
20060e0e:	c92d                	c.beqz	a0,20060e80 <inic_host_trx_int_hdl+0xd2>
20060e10:	8522                	c.mv	a0,s0
20060e12:	2af1                	c.jal	20060fee <inic_msg_enqueue>
20060e14:	157d                	c.addi	a0,-1
20060e16:	00a03533          	sltu	a0,zero,a0
20060e1a:	c408                	c.sw	a0,8(s0)
20060e1c:	00042023          	sw	zero,0(s0)
20060e20:	7c1027f3          	csrrs	a5,0x7c1,zero
20060e24:	1007f793          	andi	a5,a5,256
20060e28:	c7a1                	c.beqz	a5,20060e70 <inic_host_trx_int_hdl+0xc2>
20060e2a:	01f47793          	andi	a5,s0,31
20060e2e:	04000713          	addi	a4,zero,64
20060e32:	c781                	c.beqz	a5,20060e3a <inic_host_trx_int_hdl+0x8c>
20060e34:	04078713          	addi	a4,a5,64
20060e38:	9801                	c.andi	s0,-32
20060e3a:	01f47793          	andi	a5,s0,31
20060e3e:	97ba                	c.add	a5,a4
20060e40:	0ff0000f          	fence	iorw,iorw
20060e44:	97a2                	c.add	a5,s0
20060e46:	0484700b          	cache	dwb,(s0)
20060e4a:	02040413          	addi	s0,s0,32
20060e4e:	40878733          	sub	a4,a5,s0
20060e52:	fee04ae3          	blt	zero,a4,20060e46 <inic_host_trx_int_hdl+0x98>
20060e56:	0ff0000f          	fence	iorw,iorw
20060e5a:	0001                	c.addi	zero,0
20060e5c:	0001                	c.addi	zero,0
20060e5e:	0001                	c.addi	zero,0
20060e60:	0001                	c.addi	zero,0
20060e62:	0001                	c.addi	zero,0
20060e64:	0001                	c.addi	zero,0
20060e66:	0001                	c.addi	zero,0
20060e68:	0001                	c.addi	zero,0
20060e6a:	0001                	c.addi	zero,0
20060e6c:	0001                	c.addi	zero,0
20060e6e:	0001                	c.addi	zero,0
20060e70:	40b2                	c.lwsp	ra,12(sp)
20060e72:	4422                	c.lwsp	s0,8(sp)
20060e74:	0141                	c.addi	sp,16
20060e76:	8082                	c.jr	ra
20060e78:	87a2                	c.mv	a5,s0
20060e7a:	04000693          	addi	a3,zero,64
20060e7e:	bfa1                	c.j	20060dd6 <inic_host_trx_int_hdl+0x28>
20060e80:	00044503          	lbu	a0,0(s0)
20060e84:	00c44603          	lbu	a2,12(s0)
20060e88:	404c                	c.lw	a1,4(s0)
20060e8a:	f01ff0ef          	jal	ra,20060d8a <inic_host_trx_event_hdl>
20060e8e:	4501                	c.li	a0,0
20060e90:	b769                	c.j	20060e1a <inic_host_trx_int_hdl+0x6c>

20060e92 <inic_msg_q_task>:
20060e92:	1101                	c.addi	sp,-32
20060e94:	cc22                	c.swsp	s0,24(sp)
20060e96:	20069437          	lui	s0,0x20069
20060e9a:	c84a                	c.swsp	s2,16(sp)
20060e9c:	c64e                	c.swsp	s3,12(sp)
20060e9e:	80040913          	addi	s2,s0,-2048 # 20068800 <g_ipc_msg_q_priv>
20060ea2:	ce06                	c.swsp	ra,28(sp)
20060ea4:	ca26                	c.swsp	s1,20(sp)
20060ea6:	80040413          	addi	s0,s0,-2048
20060eaa:	4985                	c.li	s3,1
20060eac:	00c92503          	lw	a0,12(s2)
20060eb0:	55fd                	c.li	a1,-1
20060eb2:	a71fa0ef          	jal	ra,2005b922 <rtos_sema_take>
20060eb6:	ff2fa0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
20060eba:	8522                	c.mv	a0,s0
20060ebc:	2c25                	c.jal	200610f4 <rtw_queue_empty>
20060ebe:	03351063          	bne	a0,s3,20060ede <inic_msg_q_task+0x4c>
20060ec2:	80bfa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
20060ec6:	01444783          	lbu	a5,20(s0)
20060eca:	f3ed                	c.bnez	a5,20060eac <inic_msg_q_task+0x1a>
20060ecc:	4462                	c.lwsp	s0,24(sp)
20060ece:	40f2                	c.lwsp	ra,28(sp)
20060ed0:	44d2                	c.lwsp	s1,20(sp)
20060ed2:	4942                	c.lwsp	s2,16(sp)
20060ed4:	49b2                	c.lwsp	s3,12(sp)
20060ed6:	4501                	c.li	a0,0
20060ed8:	6105                	c.addi16sp	sp,32
20060eda:	d0dfa06f          	jal	zero,2005bbe6 <rtos_task_delete>
20060ede:	4004                	c.lw	s1,0(s0)
20060ee0:	8526                	c.mv	a0,s1
20060ee2:	2add                	c.jal	200610d8 <rtw_list_delete>
20060ee4:	fe8fa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
20060ee8:	dcf9                	c.beqz	s1,20060ec6 <inic_msg_q_task+0x34>
20060eea:	44c8                	c.lw	a0,12(s1)
20060eec:	448c                	c.lw	a1,8(s1)
20060eee:	00155613          	srli	a2,a0,0x1
20060ef2:	810d                	c.srli	a0,0x3
20060ef4:	8a0d                	c.andi	a2,3
20060ef6:	897d                	c.andi	a0,31
20060ef8:	e93ff0ef          	jal	ra,20060d8a <inic_host_trx_event_hdl>
20060efc:	facfa0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
20060f00:	00c4c783          	lbu	a5,12(s1)
20060f04:	9bf9                	c.andi	a5,-2
20060f06:	00f48623          	sb	a5,12(s1)
20060f0a:	fc2fa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
20060f0e:	b765                	c.j	20060eb6 <inic_msg_q_task+0x24>

20060f10 <inic_msg_q_init>:
20060f10:	1141                	c.addi	sp,-16
20060f12:	c422                	c.swsp	s0,8(sp)
20060f14:	20069437          	lui	s0,0x20069
20060f18:	02000613          	addi	a2,zero,32
20060f1c:	4581                	c.li	a1,0
20060f1e:	80040513          	addi	a0,s0,-2048 # 20068800 <g_ipc_msg_q_priv>
20060f22:	c606                	c.swsp	ra,12(sp)
20060f24:	c226                	c.swsp	s1,4(sp)
20060f26:	dffa9097          	auipc	ra,0xdffa9
20060f2a:	ba8080e7          	jalr	ra,-1112(ra) # 9ace <__wrap_memset>
20060f2e:	20068537          	lui	a0,0x20068
20060f32:	04000613          	addi	a2,zero,64
20060f36:	4581                	c.li	a1,0
20060f38:	7c050513          	addi	a0,a0,1984 # 200687c0 <g_inic_ipc_ex_msg>
20060f3c:	dffa9097          	auipc	ra,0xdffa9
20060f40:	b92080e7          	jalr	ra,-1134(ra) # 9ace <__wrap_memset>
20060f44:	80040513          	addi	a0,s0,-2048
20060f48:	80040493          	addi	s1,s0,-2048
20060f4c:	2a69                	c.jal	200610e6 <rtw_init_queue>
20060f4e:	567d                	c.li	a2,-1
20060f50:	4581                	c.li	a1,0
20060f52:	00c48513          	addi	a0,s1,12
20060f56:	94ffa0ef          	jal	ra,2005b8a4 <rtos_sema_create_static>
20060f5a:	567d                	c.li	a2,-1
20060f5c:	4581                	c.li	a1,0
20060f5e:	01048513          	addi	a0,s1,16
20060f62:	943fa0ef          	jal	ra,2005b8a4 <rtos_sema_create_static>
20060f66:	4888                	c.lw	a0,16(s1)
20060f68:	80040413          	addi	s0,s0,-2048
20060f6c:	a03fa0ef          	jal	ra,2005b96e <rtos_sema_give>
20060f70:	45c1a503          	lw	a0,1116(gp) # 20066de4 <wifi_user_config+0x24>
20060f74:	00a48e23          	sb	a0,28(s1)
20060f78:	0ff57513          	andi	a0,a0,255
20060f7c:	0512                	c.slli	a0,0x4
20060f7e:	f7afa0ef          	jal	ra,2005b6f8 <rtos_mem_zmalloc>
20060f82:	cc88                	c.sw	a0,24(s1)
20060f84:	4781                	c.li	a5,0
20060f86:	01c44703          	lbu	a4,28(s0)
20060f8a:	04e7c763          	blt	a5,a4,20060fd8 <inic_msg_q_init+0xc8>
20060f8e:	20061637          	lui	a2,0x20061
20060f92:	200655b7          	lui	a1,0x20065
20060f96:	4799                	c.li	a5,6
20060f98:	41800713          	addi	a4,zero,1048
20060f9c:	4681                	c.li	a3,0
20060f9e:	e9260613          	addi	a2,a2,-366 # 20060e92 <inic_msg_q_task>
20060fa2:	97058593          	addi	a1,a1,-1680 # 20064970 <__func__.1+0x14>
20060fa6:	4501                	c.li	a0,0
20060fa8:	c1bfa0ef          	jal	ra,2005bbc2 <rtos_task_create>
20060fac:	cd11                	c.beqz	a0,20060fc8 <inic_msg_q_init+0xb8>
20060fae:	200656b7          	lui	a3,0x20065
20060fb2:	200655b7          	lui	a1,0x20065
20060fb6:	98068693          	addi	a3,a3,-1664 # 20064980 <__func__.1+0x24>
20060fba:	04500613          	addi	a2,zero,69
20060fbe:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x14c>
20060fc2:	4509                	c.li	a0,2
20060fc4:	9cafa0ef          	jal	ra,2005b18e <rtk_log_write>
20060fc8:	4785                	c.li	a5,1
20060fca:	40b2                	c.lwsp	ra,12(sp)
20060fcc:	00f40a23          	sb	a5,20(s0)
20060fd0:	4422                	c.lwsp	s0,8(sp)
20060fd2:	4492                	c.lwsp	s1,4(sp)
20060fd4:	0141                	c.addi	sp,16
20060fd6:	8082                	c.jr	ra
20060fd8:	4c18                	c.lw	a4,24(s0)
20060fda:	00479693          	slli	a3,a5,0x4
20060fde:	0785                	c.addi	a5,1
20060fe0:	9736                	c.add	a4,a3
20060fe2:	00c74683          	lbu	a3,12(a4)
20060fe6:	9af9                	c.andi	a3,-2
20060fe8:	00d70623          	sb	a3,12(a4)
20060fec:	bf69                	c.j	20060f86 <inic_msg_q_init+0x76>

20060fee <inic_msg_enqueue>:
20060fee:	1101                	c.addi	sp,-32
20060ff0:	ca26                	c.swsp	s1,20(sp)
20060ff2:	c84a                	c.swsp	s2,16(sp)
20060ff4:	c64e                	c.swsp	s3,12(sp)
20060ff6:	200694b7          	lui	s1,0x20069
20060ffa:	ce06                	c.swsp	ra,28(sp)
20060ffc:	cc22                	c.swsp	s0,24(sp)
20060ffe:	89aa                	c.mv	s3,a0
20061000:	ea8fa0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
20061004:	80048793          	addi	a5,s1,-2048 # 20068800 <g_ipc_msg_q_priv>
20061008:	01c7c683          	lbu	a3,28(a5)
2006100c:	4f90                	c.lw	a2,24(a5)
2006100e:	80048913          	addi	s2,s1,-2048
20061012:	4781                	c.li	a5,0
20061014:	00d7c463          	blt	a5,a3,2006101c <inic_msg_enqueue+0x2e>
20061018:	4401                	c.li	s0,0
2006101a:	a829                	c.j	20061034 <inic_msg_enqueue+0x46>
2006101c:	00479413          	slli	s0,a5,0x4
20061020:	9432                	c.add	s0,a2
20061022:	00c44703          	lbu	a4,12(s0)
20061026:	00177593          	andi	a1,a4,1
2006102a:	e1b1                	c.bnez	a1,2006106e <inic_msg_enqueue+0x80>
2006102c:	00176713          	ori	a4,a4,1
20061030:	00e40623          	sb	a4,12(s0)
20061034:	e98fa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
20061038:	ec0d                	c.bnez	s0,20061072 <inic_msg_enqueue+0x84>
2006103a:	200656b7          	lui	a3,0x20065
2006103e:	200655b7          	lui	a1,0x20065
20061042:	99868693          	addi	a3,a3,-1640 # 20064998 <__func__.1+0x3c>
20061046:	04500613          	addi	a2,zero,69
2006104a:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x14c>
2006104e:	4509                	c.li	a0,2
20061050:	93efa0ef          	jal	ra,2005b18e <rtk_log_write>
20061054:	547d                	c.li	s0,-1
20061056:	00c92503          	lw	a0,12(s2)
2006105a:	915fa0ef          	jal	ra,2005b96e <rtos_sema_give>
2006105e:	40f2                	c.lwsp	ra,28(sp)
20061060:	8522                	c.mv	a0,s0
20061062:	4462                	c.lwsp	s0,24(sp)
20061064:	44d2                	c.lwsp	s1,20(sp)
20061066:	4942                	c.lwsp	s2,16(sp)
20061068:	49b2                	c.lwsp	s3,12(sp)
2006106a:	6105                	c.addi16sp	sp,32
2006106c:	8082                	c.jr	ra
2006106e:	0785                	c.addi	a5,1
20061070:	b755                	c.j	20061014 <inic_msg_enqueue+0x26>
20061072:	0009a783          	lw	a5,0(s3)
20061076:	00379713          	slli	a4,a5,0x3
2006107a:	00c44783          	lbu	a5,12(s0)
2006107e:	8b9d                	c.andi	a5,7
20061080:	8fd9                	c.or	a5,a4
20061082:	00f40623          	sb	a5,12(s0)
20061086:	0049a703          	lw	a4,4(s3)
2006108a:	9be5                	c.andi	a5,-7
2006108c:	c418                	c.sw	a4,8(s0)
2006108e:	00c9a703          	lw	a4,12(s3)
20061092:	8b0d                	c.andi	a4,3
20061094:	0706                	c.slli	a4,0x1
20061096:	8fd9                	c.or	a5,a4
20061098:	00f40623          	sb	a5,12(s0)
2006109c:	e0cfa0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
200610a0:	8522                	c.mv	a0,s0
200610a2:	80048593          	addi	a1,s1,-2048
200610a6:	201d                	c.jal	200610cc <rtw_list_insert_tail>
200610a8:	4405                	c.li	s0,1
200610aa:	e22fa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
200610ae:	b765                	c.j	20061056 <inic_msg_enqueue+0x68>

200610b0 <inic_msg_get_queue_status>:
200610b0:	200697b7          	lui	a5,0x20069
200610b4:	8147c503          	lbu	a0,-2028(a5) # 20068814 <g_ipc_msg_q_priv+0x14>
200610b8:	8082                	c.jr	ra

200610ba <rtw_init_listhead>:
200610ba:	c108                	c.sw	a0,0(a0)
200610bc:	c148                	c.sw	a0,4(a0)
200610be:	8082                	c.jr	ra

200610c0 <rtw_list_insert_head>:
200610c0:	419c                	c.lw	a5,0(a1)
200610c2:	c3c8                	c.sw	a0,4(a5)
200610c4:	c11c                	c.sw	a5,0(a0)
200610c6:	c14c                	c.sw	a1,4(a0)
200610c8:	c188                	c.sw	a0,0(a1)
200610ca:	8082                	c.jr	ra

200610cc <rtw_list_insert_tail>:
200610cc:	41dc                	c.lw	a5,4(a1)
200610ce:	c1c8                	c.sw	a0,4(a1)
200610d0:	c10c                	c.sw	a1,0(a0)
200610d2:	c15c                	c.sw	a5,4(a0)
200610d4:	c388                	c.sw	a0,0(a5)
200610d6:	8082                	c.jr	ra

200610d8 <rtw_list_delete>:
200610d8:	415c                	c.lw	a5,4(a0)
200610da:	4118                	c.lw	a4,0(a0)
200610dc:	c35c                	c.sw	a5,4(a4)
200610de:	c398                	c.sw	a4,0(a5)
200610e0:	c108                	c.sw	a0,0(a0)
200610e2:	c148                	c.sw	a0,4(a0)
200610e4:	8082                	c.jr	ra

200610e6 <rtw_init_queue>:
200610e6:	c108                	c.sw	a0,0(a0)
200610e8:	fe81a783          	lw	a5,-24(gp) # 20066970 <p_wifi_rom2flash>
200610ec:	c148                	c.sw	a0,4(a0)
200610ee:	0521                	c.addi	a0,8
200610f0:	57dc                	c.lw	a5,44(a5)
200610f2:	8782                	c.jr	a5

200610f4 <rtw_queue_empty>:
200610f4:	411c                	c.lw	a5,0(a0)
200610f6:	40a78533          	sub	a0,a5,a0
200610fa:	00153513          	sltiu	a0,a0,1
200610fe:	8082                	c.jr	ra

20061100 <rtw_end_of_queue_search>:
20061100:	8d0d                	c.sub	a0,a1
20061102:	00153513          	sltiu	a0,a0,1
20061106:	8082                	c.jr	ra

20061108 <timer_wrapper>:
20061108:	1101                	c.addi	sp,-32
2006110a:	ca26                	c.swsp	s1,20(sp)
2006110c:	cc22                	c.swsp	s0,24(sp)
2006110e:	c84a                	c.swsp	s2,16(sp)
20061110:	c64e                	c.swsp	s3,12(sp)
20061112:	c452                	c.swsp	s4,8(sp)
20061114:	ce06                	c.swsp	ra,28(sp)
20061116:	89aa                	c.mv	s3,a0
20061118:	d90fa0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
2006111c:	ff41a403          	lw	s0,-12(gp) # 2006697c <timer_table>
20061120:	4901                	c.li	s2,0
20061122:	ff418a13          	addi	s4,gp,-12 # 2006697c <timer_table>
20061126:	85a2                	c.mv	a1,s0
20061128:	ff418513          	addi	a0,gp,-12 # 2006697c <timer_table>
2006112c:	fd5ff0ef          	jal	ra,20061100 <rtw_end_of_queue_search>
20061130:	c90d                	c.beqz	a0,20061162 <timer_wrapper+0x5a>
20061132:	d9afa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
20061136:	03441c63          	bne	s0,s4,2006116e <timer_wrapper+0x66>
2006113a:	4462                	c.lwsp	s0,24(sp)
2006113c:	40f2                	c.lwsp	ra,28(sp)
2006113e:	44d2                	c.lwsp	s1,20(sp)
20061140:	4942                	c.lwsp	s2,16(sp)
20061142:	49b2                	c.lwsp	s3,12(sp)
20061144:	4a22                	c.lwsp	s4,8(sp)
20061146:	200656b7          	lui	a3,0x20065
2006114a:	200655b7          	lui	a1,0x20065
2006114e:	9b068693          	addi	a3,a3,-1616 # 200649b0 <__func__.1+0x54>
20061152:	04500613          	addi	a2,zero,69
20061156:	9e058593          	addi	a1,a1,-1568 # 200649e0 <__func__.1+0x84>
2006115a:	4509                	c.li	a0,2
2006115c:	6105                	c.addi16sp	sp,32
2006115e:	830fa06f          	jal	zero,2005b18e <rtk_log_write>
20061162:	441c                	c.lw	a5,8(s0)
20061164:	8922                	c.mv	s2,s0
20061166:	fd3786e3          	beq	a5,s3,20061132 <timer_wrapper+0x2a>
2006116a:	4000                	c.lw	s0,0(s0)
2006116c:	bf6d                	c.j	20061126 <timer_wrapper+0x1e>
2006116e:	01092783          	lw	a5,16(s2)
20061172:	cb99                	c.beqz	a5,20061188 <timer_wrapper+0x80>
20061174:	4462                	c.lwsp	s0,24(sp)
20061176:	00c92503          	lw	a0,12(s2)
2006117a:	40f2                	c.lwsp	ra,28(sp)
2006117c:	44d2                	c.lwsp	s1,20(sp)
2006117e:	4942                	c.lwsp	s2,16(sp)
20061180:	49b2                	c.lwsp	s3,12(sp)
20061182:	4a22                	c.lwsp	s4,8(sp)
20061184:	6105                	c.addi16sp	sp,32
20061186:	8782                	c.jr	a5
20061188:	40f2                	c.lwsp	ra,28(sp)
2006118a:	4462                	c.lwsp	s0,24(sp)
2006118c:	44d2                	c.lwsp	s1,20(sp)
2006118e:	4942                	c.lwsp	s2,16(sp)
20061190:	49b2                	c.lwsp	s3,12(sp)
20061192:	4a22                	c.lwsp	s4,8(sp)
20061194:	6105                	c.addi16sp	sp,32
20061196:	8082                	c.jr	ra

20061198 <init_timer_wrapper>:
20061198:	1141                	c.addi	sp,-16
2006119a:	ff418513          	addi	a0,gp,-12 # 2006697c <timer_table>
2006119e:	c606                	c.swsp	ra,12(sp)
200611a0:	f1bff0ef          	jal	ra,200610ba <rtw_init_listhead>
200611a4:	40b2                	c.lwsp	ra,12(sp)
200611a6:	fe01ae23          	sw	zero,-4(gp) # 20066984 <timer_used_num>
200611aa:	fe01a823          	sw	zero,-16(gp) # 20066978 <max_timer_used_num>
200611ae:	0141                	c.addi	sp,16
200611b0:	8082                	c.jr	ra

200611b2 <init_timer>:
200611b2:	491c                	c.lw	a5,16(a0)
200611b4:	cbc1                	c.beqz	a5,20061244 <init_timer+0x92>
200611b6:	1141                	c.addi	sp,-16
200611b8:	c422                	c.swsp	s0,8(sp)
200611ba:	c606                	c.swsp	ra,12(sp)
200611bc:	842a                	c.mv	s0,a0
200611be:	4508                	c.lw	a0,8(a0)
200611c0:	e535                	c.bnez	a0,2006122c <init_timer+0x7a>
200611c2:	200617b7          	lui	a5,0x20061
200611c6:	10878793          	addi	a5,a5,264 # 20061108 <timer_wrapper>
200611ca:	4701                	c.li	a4,0
200611cc:	56fd                	c.li	a3,-1
200611ce:	4601                	c.li	a2,0
200611d0:	00840513          	addi	a0,s0,8
200611d4:	ab3fa0ef          	jal	ra,2005bc86 <rtos_timer_create_static>
200611d8:	441c                	c.lw	a5,8(s0)
200611da:	e38d                	c.bnez	a5,200611fc <init_timer+0x4a>
200611dc:	4422                	c.lwsp	s0,8(sp)
200611de:	40b2                	c.lwsp	ra,12(sp)
200611e0:	200656b7          	lui	a3,0x20065
200611e4:	200655b7          	lui	a1,0x20065
200611e8:	9e868693          	addi	a3,a3,-1560 # 200649e8 <__func__.1+0x8c>
200611ec:	04500613          	addi	a2,zero,69
200611f0:	9e058593          	addi	a1,a1,-1568 # 200649e0 <__func__.1+0x84>
200611f4:	4509                	c.li	a0,2
200611f6:	0141                	c.addi	sp,16
200611f8:	f97f906f          	jal	zero,2005b18e <rtk_log_write>
200611fc:	cacfa0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
20061200:	ff418593          	addi	a1,gp,-12 # 2006697c <timer_table>
20061204:	8522                	c.mv	a0,s0
20061206:	ebbff0ef          	jal	ra,200610c0 <rtw_list_insert_head>
2006120a:	cc2fa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
2006120e:	ffc18713          	addi	a4,gp,-4 # 20066984 <timer_used_num>
20061212:	431c                	c.lw	a5,0(a4)
20061214:	0785                	c.addi	a5,1
20061216:	c31c                	c.sw	a5,0(a4)
20061218:	ff018713          	addi	a4,gp,-16 # 20066978 <max_timer_used_num>
2006121c:	4314                	c.lw	a3,0(a4)
2006121e:	00f6d363          	bge	a3,a5,20061224 <init_timer+0x72>
20061222:	c31c                	c.sw	a5,0(a4)
20061224:	40b2                	c.lwsp	ra,12(sp)
20061226:	4422                	c.lwsp	s0,8(sp)
20061228:	0141                	c.addi	sp,16
2006122a:	8082                	c.jr	ra
2006122c:	c2bfa0ef          	jal	ra,2005be56 <rtos_timer_is_timer_active>
20061230:	4785                	c.li	a5,1
20061232:	fef519e3          	bne	a0,a5,20061224 <init_timer+0x72>
20061236:	4408                	c.lw	a0,8(s0)
20061238:	4422                	c.lwsp	s0,8(sp)
2006123a:	40b2                	c.lwsp	ra,12(sp)
2006123c:	55fd                	c.li	a1,-1
2006123e:	0141                	c.addi	sp,16
20061240:	b59fa06f          	jal	zero,2005bd98 <rtos_timer_stop>
20061244:	8082                	c.jr	ra

20061246 <mod_timer>:
20061246:	1141                	c.addi	sp,-16
20061248:	c422                	c.swsp	s0,8(sp)
2006124a:	c226                	c.swsp	s1,4(sp)
2006124c:	c606                	c.swsp	ra,12(sp)
2006124e:	842a                	c.mv	s0,a0
20061250:	4508                	c.lw	a0,8(a0)
20061252:	84ae                	c.mv	s1,a1
20061254:	e915                	c.bnez	a0,20061288 <mod_timer+0x42>
20061256:	20065537          	lui	a0,0x20065
2006125a:	a0050513          	addi	a0,a0,-1536 # 20064a00 <__func__.1+0xa4>
2006125e:	948fe0ef          	jal	ra,2005f3a6 <DiagPrintf_minimal>
20061262:	4408                	c.lw	a0,8(s0)
20061264:	cd05                	c.beqz	a0,2006129c <mod_timer+0x56>
20061266:	567d                	c.li	a2,-1
20061268:	85a6                	c.mv	a1,s1
2006126a:	b8bfa0ef          	jal	ra,2005bdf4 <rtos_timer_change_period>
2006126e:	57fd                	c.li	a5,-1
20061270:	02f51663          	bne	a0,a5,2006129c <mod_timer+0x56>
20061274:	4422                	c.lwsp	s0,8(sp)
20061276:	40b2                	c.lwsp	ra,12(sp)
20061278:	4492                	c.lwsp	s1,4(sp)
2006127a:	20065537          	lui	a0,0x20065
2006127e:	a1850513          	addi	a0,a0,-1512 # 20064a18 <__func__.1+0xbc>
20061282:	0141                	c.addi	sp,16
20061284:	922fe06f          	jal	zero,2005f3a6 <DiagPrintf_minimal>
20061288:	bcffa0ef          	jal	ra,2005be56 <rtos_timer_is_timer_active>
2006128c:	4785                	c.li	a5,1
2006128e:	fcf51ae3          	bne	a0,a5,20061262 <mod_timer+0x1c>
20061292:	4408                	c.lw	a0,8(s0)
20061294:	55fd                	c.li	a1,-1
20061296:	b03fa0ef          	jal	ra,2005bd98 <rtos_timer_stop>
2006129a:	b7e1                	c.j	20061262 <mod_timer+0x1c>
2006129c:	40b2                	c.lwsp	ra,12(sp)
2006129e:	4422                	c.lwsp	s0,8(sp)
200612a0:	4492                	c.lwsp	s1,4(sp)
200612a2:	0141                	c.addi	sp,16
200612a4:	8082                	c.jr	ra

200612a6 <cancel_timer_ex>:
200612a6:	451c                	c.lw	a5,8(a0)
200612a8:	c7ad                	c.beqz	a5,20061312 <cancel_timer_ex+0x6c>
200612aa:	1101                	c.addi	sp,-32
200612ac:	c84a                	c.swsp	s2,16(sp)
200612ae:	cc22                	c.swsp	s0,24(sp)
200612b0:	ca26                	c.swsp	s1,20(sp)
200612b2:	c64e                	c.swsp	s3,12(sp)
200612b4:	ce06                	c.swsp	ra,28(sp)
200612b6:	84aa                	c.mv	s1,a0
200612b8:	bf0fa0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
200612bc:	ff41a403          	lw	s0,-12(gp) # 2006697c <timer_table>
200612c0:	ff418993          	addi	s3,gp,-12 # 2006697c <timer_table>
200612c4:	85a2                	c.mv	a1,s0
200612c6:	ff418513          	addi	a0,gp,-12 # 2006697c <timer_table>
200612ca:	e37ff0ef          	jal	ra,20061100 <rtw_end_of_queue_search>
200612ce:	e509                	c.bnez	a0,200612d8 <cancel_timer_ex+0x32>
200612d0:	4418                	c.lw	a4,8(s0)
200612d2:	449c                	c.lw	a5,8(s1)
200612d4:	02f71463          	bne	a4,a5,200612fc <cancel_timer_ex+0x56>
200612d8:	bf4fa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
200612dc:	4488                	c.lw	a0,8(s1)
200612de:	03341163          	bne	s0,s3,20061300 <cancel_timer_ex+0x5a>
200612e2:	4462                	c.lwsp	s0,24(sp)
200612e4:	40f2                	c.lwsp	ra,28(sp)
200612e6:	44d2                	c.lwsp	s1,20(sp)
200612e8:	4942                	c.lwsp	s2,16(sp)
200612ea:	49b2                	c.lwsp	s3,12(sp)
200612ec:	85aa                	c.mv	a1,a0
200612ee:	20065537          	lui	a0,0x20065
200612f2:	a2850513          	addi	a0,a0,-1496 # 20064a28 <__func__.1+0xcc>
200612f6:	6105                	c.addi16sp	sp,32
200612f8:	8aefe06f          	jal	zero,2005f3a6 <DiagPrintf_minimal>
200612fc:	4000                	c.lw	s0,0(s0)
200612fe:	b7d9                	c.j	200612c4 <cancel_timer_ex+0x1e>
20061300:	4462                	c.lwsp	s0,24(sp)
20061302:	40f2                	c.lwsp	ra,28(sp)
20061304:	44d2                	c.lwsp	s1,20(sp)
20061306:	4942                	c.lwsp	s2,16(sp)
20061308:	49b2                	c.lwsp	s3,12(sp)
2006130a:	55fd                	c.li	a1,-1
2006130c:	6105                	c.addi16sp	sp,32
2006130e:	a8bfa06f          	jal	zero,2005bd98 <rtos_timer_stop>
20061312:	8082                	c.jr	ra

20061314 <del_timer_sync>:
20061314:	451c                	c.lw	a5,8(a0)
20061316:	cfbd                	c.beqz	a5,20061394 <del_timer_sync+0x80>
20061318:	1101                	c.addi	sp,-32
2006131a:	c84a                	c.swsp	s2,16(sp)
2006131c:	cc22                	c.swsp	s0,24(sp)
2006131e:	ca26                	c.swsp	s1,20(sp)
20061320:	c64e                	c.swsp	s3,12(sp)
20061322:	ce06                	c.swsp	ra,28(sp)
20061324:	842a                	c.mv	s0,a0
20061326:	b82fa0ef          	jal	ra,2005b6a8 <rtos_critical_enter>
2006132a:	ff41a483          	lw	s1,-12(gp) # 2006697c <timer_table>
2006132e:	ff418993          	addi	s3,gp,-12 # 2006697c <timer_table>
20061332:	85a6                	c.mv	a1,s1
20061334:	ff418513          	addi	a0,gp,-12 # 2006697c <timer_table>
20061338:	dc9ff0ef          	jal	ra,20061100 <rtw_end_of_queue_search>
2006133c:	e901                	c.bnez	a0,2006134c <del_timer_sync+0x38>
2006133e:	4498                	c.lw	a4,8(s1)
20061340:	441c                	c.lw	a5,8(s0)
20061342:	02f71563          	bne	a4,a5,2006136c <del_timer_sync+0x58>
20061346:	8526                	c.mv	a0,s1
20061348:	d91ff0ef          	jal	ra,200610d8 <rtw_list_delete>
2006134c:	b80fa0ef          	jal	ra,2005b6cc <rtos_critical_exit>
20061350:	03349063          	bne	s1,s3,20061370 <del_timer_sync+0x5c>
20061354:	4462                	c.lwsp	s0,24(sp)
20061356:	40f2                	c.lwsp	ra,28(sp)
20061358:	44d2                	c.lwsp	s1,20(sp)
2006135a:	4942                	c.lwsp	s2,16(sp)
2006135c:	49b2                	c.lwsp	s3,12(sp)
2006135e:	20065537          	lui	a0,0x20065
20061362:	a4050513          	addi	a0,a0,-1472 # 20064a40 <__func__.1+0xe4>
20061366:	6105                	c.addi16sp	sp,32
20061368:	83efe06f          	jal	zero,2005f3a6 <DiagPrintf_minimal>
2006136c:	4084                	c.lw	s1,0(s1)
2006136e:	b7d1                	c.j	20061332 <del_timer_sync+0x1e>
20061370:	4408                	c.lw	a0,8(s0)
20061372:	55fd                	c.li	a1,-1
20061374:	995fa0ef          	jal	ra,2005bd08 <rtos_timer_delete_static>
20061378:	ffc18793          	addi	a5,gp,-4 # 20066984 <timer_used_num>
2006137c:	4398                	c.lw	a4,0(a5)
2006137e:	00042423          	sw	zero,8(s0)
20061382:	177d                	c.addi	a4,-1
20061384:	c398                	c.sw	a4,0(a5)
20061386:	40f2                	c.lwsp	ra,28(sp)
20061388:	4462                	c.lwsp	s0,24(sp)
2006138a:	44d2                	c.lwsp	s1,20(sp)
2006138c:	4942                	c.lwsp	s2,16(sp)
2006138e:	49b2                	c.lwsp	s3,12(sp)
20061390:	6105                	c.addi16sp	sp,32
20061392:	8082                	c.jr	ra
20061394:	8082                	c.jr	ra

20061396 <rtw_init_timer>:
20061396:	c910                	c.sw	a2,16(a0)
20061398:	c554                	c.sw	a3,12(a0)
2006139a:	85ba                	c.mv	a1,a4
2006139c:	e17ff06f          	jal	zero,200611b2 <init_timer>

200613a0 <rtw_set_timer>:
200613a0:	ea7ff06f          	jal	zero,20061246 <mod_timer>

200613a4 <rtw_cancel_timer>:
200613a4:	1141                	c.addi	sp,-16
200613a6:	c606                	c.swsp	ra,12(sp)
200613a8:	effff0ef          	jal	ra,200612a6 <cancel_timer_ex>
200613ac:	40b2                	c.lwsp	ra,12(sp)
200613ae:	4501                	c.li	a0,0
200613b0:	0141                	c.addi	sp,16
200613b2:	8082                	c.jr	ra

200613b4 <rtw_del_timer>:
200613b4:	f61ff06f          	jal	zero,20061314 <del_timer_sync>

200613b8 <rtw_acquire_wakelock>:
200613b8:	1141                	c.addi	sp,-16
200613ba:	c606                	c.swsp	ra,12(sp)
200613bc:	c3af80ef          	jal	ra,200597f6 <pmu_yield_os_check>
200613c0:	c511                	c.beqz	a0,200613cc <rtw_acquire_wakelock+0x14>
200613c2:	40b2                	c.lwsp	ra,12(sp)
200613c4:	4505                	c.li	a0,1
200613c6:	0141                	c.addi	sp,16
200613c8:	c7af806f          	jal	zero,20059842 <pmu_acquire_wakelock>
200613cc:	40b2                	c.lwsp	ra,12(sp)
200613ce:	0141                	c.addi	sp,16
200613d0:	8082                	c.jr	ra

200613d2 <rtw_release_wakelock>:
200613d2:	1141                	c.addi	sp,-16
200613d4:	c606                	c.swsp	ra,12(sp)
200613d6:	c20f80ef          	jal	ra,200597f6 <pmu_yield_os_check>
200613da:	c511                	c.beqz	a0,200613e6 <rtw_release_wakelock+0x14>
200613dc:	40b2                	c.lwsp	ra,12(sp)
200613de:	4505                	c.li	a0,1
200613e0:	0141                	c.addi	sp,16
200613e2:	c86f806f          	jal	zero,20059868 <pmu_release_wakelock>
200613e6:	40b2                	c.lwsp	ra,12(sp)
200613e8:	0141                	c.addi	sp,16
200613ea:	8082                	c.jr	ra

200613ec <rtw_wakelock_timeout>:
200613ec:	1141                	c.addi	sp,-16
200613ee:	c422                	c.swsp	s0,8(sp)
200613f0:	c606                	c.swsp	ra,12(sp)
200613f2:	842a                	c.mv	s0,a0
200613f4:	c02f80ef          	jal	ra,200597f6 <pmu_yield_os_check>
200613f8:	c519                	c.beqz	a0,20061406 <rtw_wakelock_timeout+0x1a>
200613fa:	8522                	c.mv	a0,s0
200613fc:	4422                	c.lwsp	s0,8(sp)
200613fe:	40b2                	c.lwsp	ra,12(sp)
20061400:	0141                	c.addi	sp,16
20061402:	b18f806f          	jal	zero,2005971a <pmu_set_sysactive_time>
20061406:	40b2                	c.lwsp	ra,12(sp)
20061408:	4422                	c.lwsp	s0,8(sp)
2006140a:	0141                	c.addi	sp,16
2006140c:	8082                	c.jr	ra

2006140e <deregister_tm_clones>:
2006140e:	20058537          	lui	a0,0x20058
20061412:	200587b7          	lui	a5,0x20058
20061416:	06050713          	addi	a4,a0,96 # 20058060 <BOOT_InitGP>
2006141a:	06078793          	addi	a5,a5,96 # 20058060 <BOOT_InitGP>
2006141e:	00e78863          	beq	a5,a4,2006142e <deregister_tm_clones+0x20>
20061422:	00000793          	addi	a5,zero,0
20061426:	c781                	c.beqz	a5,2006142e <deregister_tm_clones+0x20>
20061428:	06050513          	addi	a0,a0,96
2006142c:	8782                	c.jr	a5
2006142e:	8082                	c.jr	ra

20061430 <register_tm_clones>:
20061430:	200587b7          	lui	a5,0x20058
20061434:	20058537          	lui	a0,0x20058
20061438:	06078593          	addi	a1,a5,96 # 20058060 <BOOT_InitGP>
2006143c:	06050793          	addi	a5,a0,96 # 20058060 <BOOT_InitGP>
20061440:	8d9d                	c.sub	a1,a5
20061442:	8589                	c.srai	a1,0x2
20061444:	4789                	c.li	a5,2
20061446:	02f5c5b3          	div	a1,a1,a5
2006144a:	c599                	c.beqz	a1,20061458 <register_tm_clones+0x28>
2006144c:	00000793          	addi	a5,zero,0
20061450:	c781                	c.beqz	a5,20061458 <register_tm_clones+0x28>
20061452:	06050513          	addi	a0,a0,96
20061456:	8782                	c.jr	a5
20061458:	8082                	c.jr	ra

2006145a <__do_global_dtors_aux>:
2006145a:	200697b7          	lui	a5,0x20069
2006145e:	9a87c703          	lbu	a4,-1624(a5) # 200689a8 <completed.1>
20061462:	eb05                	c.bnez	a4,20061492 <__do_global_dtors_aux+0x38>
20061464:	1141                	c.addi	sp,-16
20061466:	c422                	c.swsp	s0,8(sp)
20061468:	c606                	c.swsp	ra,12(sp)
2006146a:	843e                	c.mv	s0,a5
2006146c:	374d                	c.jal	2006140e <deregister_tm_clones>
2006146e:	00000793          	addi	a5,zero,0
20061472:	cb89                	c.beqz	a5,20061484 <__do_global_dtors_aux+0x2a>
20061474:	20058537          	lui	a0,0x20058
20061478:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
2006147c:	00000097          	auipc	ra,0x0
20061480:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20061484:	4785                	c.li	a5,1
20061486:	40b2                	c.lwsp	ra,12(sp)
20061488:	9af40423          	sb	a5,-1624(s0)
2006148c:	4422                	c.lwsp	s0,8(sp)
2006148e:	0141                	c.addi	sp,16
20061490:	8082                	c.jr	ra
20061492:	8082                	c.jr	ra

20061494 <frame_dummy>:
20061494:	00000793          	addi	a5,zero,0
20061498:	c38d                	c.beqz	a5,200614ba <frame_dummy+0x26>
2006149a:	200695b7          	lui	a1,0x20069
2006149e:	20058537          	lui	a0,0x20058
200614a2:	1141                	c.addi	sp,-16
200614a4:	9ac58593          	addi	a1,a1,-1620 # 200689ac <object.0>
200614a8:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
200614ac:	c606                	c.swsp	ra,12(sp)
200614ae:	00000097          	auipc	ra,0x0
200614b2:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
200614b6:	40b2                	c.lwsp	ra,12(sp)
200614b8:	0141                	c.addi	sp,16
200614ba:	bf9d                	c.j	20061430 <register_tm_clones>

200614bc <__clzsi2>:
200614bc:	67c1                	c.lui	a5,0x10
200614be:	02f57663          	bgeu	a0,a5,200614ea <__clzsi2+0x2e>
200614c2:	0ff00793          	addi	a5,zero,255
200614c6:	00a7b7b3          	sltu	a5,a5,a0
200614ca:	078e                	c.slli	a5,0x3
200614cc:	20065737          	lui	a4,0x20065
200614d0:	02000693          	addi	a3,zero,32
200614d4:	8e9d                	c.sub	a3,a5
200614d6:	00f55533          	srl	a0,a0,a5
200614da:	a5070793          	addi	a5,a4,-1456 # 20064a50 <__clz_tab>
200614de:	953e                	c.add	a0,a5
200614e0:	00054503          	lbu	a0,0(a0)
200614e4:	40a68533          	sub	a0,a3,a0
200614e8:	8082                	c.jr	ra
200614ea:	01000737          	lui	a4,0x1000
200614ee:	47c1                	c.li	a5,16
200614f0:	fce56ee3          	bltu	a0,a4,200614cc <__clzsi2+0x10>
200614f4:	47e1                	c.li	a5,24
200614f6:	bfd9                	c.j	200614cc <__clzsi2+0x10>

200614f8 <__udivdi3>:
200614f8:	88aa                	c.mv	a7,a0
200614fa:	87ae                	c.mv	a5,a1
200614fc:	8832                	c.mv	a6,a2
200614fe:	8536                	c.mv	a0,a3
20061500:	8346                	c.mv	t1,a7
20061502:	20069663          	bne	a3,zero,2006170e <__udivdi3+0x216>
20061506:	200656b7          	lui	a3,0x20065
2006150a:	a5068693          	addi	a3,a3,-1456 # 20064a50 <__clz_tab>
2006150e:	0ac5ff63          	bgeu	a1,a2,200615cc <__udivdi3+0xd4>
20061512:	6741                	c.lui	a4,0x10
20061514:	0ae67563          	bgeu	a2,a4,200615be <__udivdi3+0xc6>
20061518:	0ff00713          	addi	a4,zero,255
2006151c:	00c73733          	sltu	a4,a4,a2
20061520:	070e                	c.slli	a4,0x3
20061522:	00e65533          	srl	a0,a2,a4
20061526:	96aa                	c.add	a3,a0
20061528:	0006c683          	lbu	a3,0(a3)
2006152c:	02000513          	addi	a0,zero,32
20061530:	9736                	c.add	a4,a3
20061532:	40e506b3          	sub	a3,a0,a4
20061536:	00e50b63          	beq	a0,a4,2006154c <__udivdi3+0x54>
2006153a:	00d795b3          	sll	a1,a5,a3
2006153e:	00e8d733          	srl	a4,a7,a4
20061542:	00d61833          	sll	a6,a2,a3
20061546:	8dd9                	c.or	a1,a4
20061548:	00d89333          	sll	t1,a7,a3
2006154c:	01085893          	srli	a7,a6,0x10
20061550:	0315d6b3          	divu	a3,a1,a7
20061554:	01081613          	slli	a2,a6,0x10
20061558:	8241                	c.srli	a2,0x10
2006155a:	01035793          	srli	a5,t1,0x10
2006155e:	0315f733          	remu	a4,a1,a7
20061562:	8536                	c.mv	a0,a3
20061564:	02d605b3          	mul	a1,a2,a3
20061568:	0742                	c.slli	a4,0x10
2006156a:	8fd9                	c.or	a5,a4
2006156c:	00b7fc63          	bgeu	a5,a1,20061584 <__udivdi3+0x8c>
20061570:	97c2                	c.add	a5,a6
20061572:	fff68513          	addi	a0,a3,-1
20061576:	0107e763          	bltu	a5,a6,20061584 <__udivdi3+0x8c>
2006157a:	00b7f563          	bgeu	a5,a1,20061584 <__udivdi3+0x8c>
2006157e:	ffe68513          	addi	a0,a3,-2
20061582:	97c2                	c.add	a5,a6
20061584:	8f8d                	c.sub	a5,a1
20061586:	0317f733          	remu	a4,a5,a7
2006158a:	0342                	c.slli	t1,0x10
2006158c:	01035313          	srli	t1,t1,0x10
20061590:	0317d7b3          	divu	a5,a5,a7
20061594:	0742                	c.slli	a4,0x10
20061596:	00676333          	or	t1,a4,t1
2006159a:	02f606b3          	mul	a3,a2,a5
2006159e:	863e                	c.mv	a2,a5
200615a0:	00d37b63          	bgeu	t1,a3,200615b6 <__udivdi3+0xbe>
200615a4:	9342                	c.add	t1,a6
200615a6:	fff78613          	addi	a2,a5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
200615aa:	01036663          	bltu	t1,a6,200615b6 <__udivdi3+0xbe>
200615ae:	00d37463          	bgeu	t1,a3,200615b6 <__udivdi3+0xbe>
200615b2:	ffe78613          	addi	a2,a5,-2
200615b6:	0542                	c.slli	a0,0x10
200615b8:	8d51                	c.or	a0,a2
200615ba:	4581                	c.li	a1,0
200615bc:	a85d                	c.j	20061672 <__udivdi3+0x17a>
200615be:	01000537          	lui	a0,0x1000
200615c2:	4741                	c.li	a4,16
200615c4:	f4a66fe3          	bltu	a2,a0,20061522 <__udivdi3+0x2a>
200615c8:	4761                	c.li	a4,24
200615ca:	bfa1                	c.j	20061522 <__udivdi3+0x2a>
200615cc:	e601                	c.bnez	a2,200615d4 <__udivdi3+0xdc>
200615ce:	4705                	c.li	a4,1
200615d0:	02c75833          	divu	a6,a4,a2
200615d4:	6741                	c.lui	a4,0x10
200615d6:	08e87f63          	bgeu	a6,a4,20061674 <__udivdi3+0x17c>
200615da:	0ff00713          	addi	a4,zero,255
200615de:	01077363          	bgeu	a4,a6,200615e4 <__udivdi3+0xec>
200615e2:	4521                	c.li	a0,8
200615e4:	00a85733          	srl	a4,a6,a0
200615e8:	96ba                	c.add	a3,a4
200615ea:	0006c703          	lbu	a4,0(a3)
200615ee:	02000613          	addi	a2,zero,32
200615f2:	972a                	c.add	a4,a0
200615f4:	40e606b3          	sub	a3,a2,a4
200615f8:	08e61563          	bne	a2,a4,20061682 <__udivdi3+0x18a>
200615fc:	410787b3          	sub	a5,a5,a6
20061600:	4585                	c.li	a1,1
20061602:	01085893          	srli	a7,a6,0x10
20061606:	01081613          	slli	a2,a6,0x10
2006160a:	8241                	c.srli	a2,0x10
2006160c:	01035713          	srli	a4,t1,0x10
20061610:	0317f6b3          	remu	a3,a5,a7
20061614:	0317d7b3          	divu	a5,a5,a7
20061618:	06c2                	c.slli	a3,0x10
2006161a:	8f55                	c.or	a4,a3
2006161c:	02f60e33          	mul	t3,a2,a5
20061620:	853e                	c.mv	a0,a5
20061622:	01c77c63          	bgeu	a4,t3,2006163a <__udivdi3+0x142>
20061626:	9742                	c.add	a4,a6
20061628:	fff78513          	addi	a0,a5,-1
2006162c:	01076763          	bltu	a4,a6,2006163a <__udivdi3+0x142>
20061630:	01c77563          	bgeu	a4,t3,2006163a <__udivdi3+0x142>
20061634:	ffe78513          	addi	a0,a5,-2
20061638:	9742                	c.add	a4,a6
2006163a:	41c70733          	sub	a4,a4,t3
2006163e:	031777b3          	remu	a5,a4,a7
20061642:	0342                	c.slli	t1,0x10
20061644:	01035313          	srli	t1,t1,0x10
20061648:	03175733          	divu	a4,a4,a7
2006164c:	07c2                	c.slli	a5,0x10
2006164e:	0067e333          	or	t1,a5,t1
20061652:	02e606b3          	mul	a3,a2,a4
20061656:	863a                	c.mv	a2,a4
20061658:	00d37b63          	bgeu	t1,a3,2006166e <__udivdi3+0x176>
2006165c:	9342                	c.add	t1,a6
2006165e:	fff70613          	addi	a2,a4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061662:	01036663          	bltu	t1,a6,2006166e <__udivdi3+0x176>
20061666:	00d37463          	bgeu	t1,a3,2006166e <__udivdi3+0x176>
2006166a:	ffe70613          	addi	a2,a4,-2
2006166e:	0542                	c.slli	a0,0x10
20061670:	8d51                	c.or	a0,a2
20061672:	8082                	c.jr	ra
20061674:	01000737          	lui	a4,0x1000
20061678:	4541                	c.li	a0,16
2006167a:	f6e865e3          	bltu	a6,a4,200615e4 <__udivdi3+0xec>
2006167e:	4561                	c.li	a0,24
20061680:	b795                	c.j	200615e4 <__udivdi3+0xec>
20061682:	00d81833          	sll	a6,a6,a3
20061686:	00e7d533          	srl	a0,a5,a4
2006168a:	00d89333          	sll	t1,a7,a3
2006168e:	00d797b3          	sll	a5,a5,a3
20061692:	00e8d733          	srl	a4,a7,a4
20061696:	01085893          	srli	a7,a6,0x10
2006169a:	00f76633          	or	a2,a4,a5
2006169e:	03157733          	remu	a4,a0,a7
200616a2:	01081793          	slli	a5,a6,0x10
200616a6:	83c1                	c.srli	a5,0x10
200616a8:	01065593          	srli	a1,a2,0x10
200616ac:	03155533          	divu	a0,a0,a7
200616b0:	0742                	c.slli	a4,0x10
200616b2:	8f4d                	c.or	a4,a1
200616b4:	02a786b3          	mul	a3,a5,a0
200616b8:	85aa                	c.mv	a1,a0
200616ba:	00d77c63          	bgeu	a4,a3,200616d2 <__udivdi3+0x1da>
200616be:	9742                	c.add	a4,a6
200616c0:	fff50593          	addi	a1,a0,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
200616c4:	01076763          	bltu	a4,a6,200616d2 <__udivdi3+0x1da>
200616c8:	00d77563          	bgeu	a4,a3,200616d2 <__udivdi3+0x1da>
200616cc:	ffe50593          	addi	a1,a0,-2
200616d0:	9742                	c.add	a4,a6
200616d2:	40d706b3          	sub	a3,a4,a3
200616d6:	0316f733          	remu	a4,a3,a7
200616da:	0642                	c.slli	a2,0x10
200616dc:	8241                	c.srli	a2,0x10
200616de:	0316d6b3          	divu	a3,a3,a7
200616e2:	0742                	c.slli	a4,0x10
200616e4:	02d78533          	mul	a0,a5,a3
200616e8:	00c767b3          	or	a5,a4,a2
200616ec:	8736                	c.mv	a4,a3
200616ee:	00a7fc63          	bgeu	a5,a0,20061706 <__udivdi3+0x20e>
200616f2:	97c2                	c.add	a5,a6
200616f4:	fff68713          	addi	a4,a3,-1
200616f8:	0107e763          	bltu	a5,a6,20061706 <__udivdi3+0x20e>
200616fc:	00a7f563          	bgeu	a5,a0,20061706 <__udivdi3+0x20e>
20061700:	ffe68713          	addi	a4,a3,-2
20061704:	97c2                	c.add	a5,a6
20061706:	05c2                	c.slli	a1,0x10
20061708:	8f89                	c.sub	a5,a0
2006170a:	8dd9                	c.or	a1,a4
2006170c:	bddd                	c.j	20061602 <__udivdi3+0x10a>
2006170e:	14d5e263          	bltu	a1,a3,20061852 <__udivdi3+0x35a>
20061712:	6741                	c.lui	a4,0x10
20061714:	02e6ff63          	bgeu	a3,a4,20061752 <__udivdi3+0x25a>
20061718:	0ff00713          	addi	a4,zero,255
2006171c:	00d735b3          	sltu	a1,a4,a3
20061720:	058e                	c.slli	a1,0x3
20061722:	20065737          	lui	a4,0x20065
20061726:	00b6d533          	srl	a0,a3,a1
2006172a:	a5070713          	addi	a4,a4,-1456 # 20064a50 <__clz_tab>
2006172e:	972a                	c.add	a4,a0
20061730:	00074703          	lbu	a4,0(a4)
20061734:	02000513          	addi	a0,zero,32
20061738:	972e                	c.add	a4,a1
2006173a:	40e505b3          	sub	a1,a0,a4
2006173e:	02e51163          	bne	a0,a4,20061760 <__udivdi3+0x268>
20061742:	4505                	c.li	a0,1
20061744:	f2f6e7e3          	bltu	a3,a5,20061672 <__udivdi3+0x17a>
20061748:	00c8b533          	sltu	a0,a7,a2
2006174c:	00154513          	xori	a0,a0,1
20061750:	b70d                	c.j	20061672 <__udivdi3+0x17a>
20061752:	01000737          	lui	a4,0x1000
20061756:	45c1                	c.li	a1,16
20061758:	fce6e5e3          	bltu	a3,a4,20061722 <__udivdi3+0x22a>
2006175c:	45e1                	c.li	a1,24
2006175e:	b7d1                	c.j	20061722 <__udivdi3+0x22a>
20061760:	00e65833          	srl	a6,a2,a4
20061764:	00b696b3          	sll	a3,a3,a1
20061768:	00d86833          	or	a6,a6,a3
2006176c:	00e7de33          	srl	t3,a5,a4
20061770:	01085e93          	srli	t4,a6,0x10
20061774:	03de76b3          	remu	a3,t3,t4
20061778:	00b797b3          	sll	a5,a5,a1
2006177c:	00e8d733          	srl	a4,a7,a4
20061780:	00b61333          	sll	t1,a2,a1
20061784:	00f76633          	or	a2,a4,a5
20061788:	01081793          	slli	a5,a6,0x10
2006178c:	83c1                	c.srli	a5,0x10
2006178e:	01065713          	srli	a4,a2,0x10
20061792:	03de5e33          	divu	t3,t3,t4
20061796:	06c2                	c.slli	a3,0x10
20061798:	8f55                	c.or	a4,a3
2006179a:	03c78f33          	mul	t5,a5,t3
2006179e:	8572                	c.mv	a0,t3
200617a0:	01e77c63          	bgeu	a4,t5,200617b8 <__udivdi3+0x2c0>
200617a4:	9742                	c.add	a4,a6
200617a6:	fffe0513          	addi	a0,t3,-1
200617aa:	01076763          	bltu	a4,a6,200617b8 <__udivdi3+0x2c0>
200617ae:	01e77563          	bgeu	a4,t5,200617b8 <__udivdi3+0x2c0>
200617b2:	ffee0513          	addi	a0,t3,-2
200617b6:	9742                	c.add	a4,a6
200617b8:	41e70733          	sub	a4,a4,t5
200617bc:	03d776b3          	remu	a3,a4,t4
200617c0:	03d75733          	divu	a4,a4,t4
200617c4:	06c2                	c.slli	a3,0x10
200617c6:	02e78e33          	mul	t3,a5,a4
200617ca:	01061793          	slli	a5,a2,0x10
200617ce:	83c1                	c.srli	a5,0x10
200617d0:	8fd5                	c.or	a5,a3
200617d2:	863a                	c.mv	a2,a4
200617d4:	01c7fc63          	bgeu	a5,t3,200617ec <__udivdi3+0x2f4>
200617d8:	97c2                	c.add	a5,a6
200617da:	fff70613          	addi	a2,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
200617de:	0107e763          	bltu	a5,a6,200617ec <__udivdi3+0x2f4>
200617e2:	01c7f563          	bgeu	a5,t3,200617ec <__udivdi3+0x2f4>
200617e6:	ffe70613          	addi	a2,a4,-2
200617ea:	97c2                	c.add	a5,a6
200617ec:	0542                	c.slli	a0,0x10
200617ee:	6ec1                	c.lui	t4,0x10
200617f0:	8d51                	c.or	a0,a2
200617f2:	fffe8693          	addi	a3,t4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
200617f6:	00d57833          	and	a6,a0,a3
200617fa:	01055613          	srli	a2,a0,0x10
200617fe:	00d376b3          	and	a3,t1,a3
20061802:	01035313          	srli	t1,t1,0x10
20061806:	41c787b3          	sub	a5,a5,t3
2006180a:	02d80e33          	mul	t3,a6,a3
2006180e:	02d606b3          	mul	a3,a2,a3
20061812:	010e5713          	srli	a4,t3,0x10
20061816:	02680833          	mul	a6,a6,t1
2006181a:	9836                	c.add	a6,a3
2006181c:	9742                	c.add	a4,a6
2006181e:	02660633          	mul	a2,a2,t1
20061822:	00d77363          	bgeu	a4,a3,20061828 <__udivdi3+0x330>
20061826:	9676                	c.add	a2,t4
20061828:	01075693          	srli	a3,a4,0x10
2006182c:	9636                	c.add	a2,a3
2006182e:	02c7e063          	bltu	a5,a2,2006184e <__udivdi3+0x356>
20061832:	d8c794e3          	bne	a5,a2,200615ba <__udivdi3+0xc2>
20061836:	67c1                	c.lui	a5,0x10
20061838:	17fd                	c.addi	a5,-1
2006183a:	8f7d                	c.and	a4,a5
2006183c:	0742                	c.slli	a4,0x10
2006183e:	00fe7e33          	and	t3,t3,a5
20061842:	00b898b3          	sll	a7,a7,a1
20061846:	9772                	c.add	a4,t3
20061848:	4581                	c.li	a1,0
2006184a:	e2e8f4e3          	bgeu	a7,a4,20061672 <__udivdi3+0x17a>
2006184e:	157d                	c.addi	a0,-1
20061850:	b3ad                	c.j	200615ba <__udivdi3+0xc2>
20061852:	4581                	c.li	a1,0
20061854:	4501                	c.li	a0,0
20061856:	bd31                	c.j	20061672 <__udivdi3+0x17a>

20061858 <__umoddi3>:
20061858:	8eaa                	c.mv	t4,a0
2006185a:	832e                	c.mv	t1,a1
2006185c:	8832                	c.mv	a6,a2
2006185e:	87b6                	c.mv	a5,a3
20061860:	1c069263          	bne	a3,zero,20061a24 <__umoddi3+0x1cc>
20061864:	20065737          	lui	a4,0x20065
20061868:	a5070713          	addi	a4,a4,-1456 # 20064a50 <__clz_tab>
2006186c:	0ac5f563          	bgeu	a1,a2,20061916 <__umoddi3+0xbe>
20061870:	66c1                	c.lui	a3,0x10
20061872:	08d67b63          	bgeu	a2,a3,20061908 <__umoddi3+0xb0>
20061876:	0ff00693          	addi	a3,zero,255
2006187a:	00c6f363          	bgeu	a3,a2,20061880 <__umoddi3+0x28>
2006187e:	47a1                	c.li	a5,8
20061880:	00f656b3          	srl	a3,a2,a5
20061884:	9736                	c.add	a4,a3
20061886:	00074703          	lbu	a4,0(a4)
2006188a:	97ba                	c.add	a5,a4
2006188c:	02000713          	addi	a4,zero,32
20061890:	40f708b3          	sub	a7,a4,a5
20061894:	00f70b63          	beq	a4,a5,200618aa <__umoddi3+0x52>
20061898:	011315b3          	sll	a1,t1,a7
2006189c:	00fed7b3          	srl	a5,t4,a5
200618a0:	01161833          	sll	a6,a2,a7
200618a4:	8ddd                	c.or	a1,a5
200618a6:	011e9533          	sll	a0,t4,a7
200618aa:	01085693          	srli	a3,a6,0x10
200618ae:	02d5f7b3          	remu	a5,a1,a3
200618b2:	01081613          	slli	a2,a6,0x10
200618b6:	8241                	c.srli	a2,0x10
200618b8:	01055713          	srli	a4,a0,0x10
200618bc:	02d5d5b3          	divu	a1,a1,a3
200618c0:	07c2                	c.slli	a5,0x10
200618c2:	8f5d                	c.or	a4,a5
200618c4:	02b605b3          	mul	a1,a2,a1
200618c8:	00b77863          	bgeu	a4,a1,200618d8 <__umoddi3+0x80>
200618cc:	9742                	c.add	a4,a6
200618ce:	01076563          	bltu	a4,a6,200618d8 <__umoddi3+0x80>
200618d2:	00b77363          	bgeu	a4,a1,200618d8 <__umoddi3+0x80>
200618d6:	9742                	c.add	a4,a6
200618d8:	8f0d                	c.sub	a4,a1
200618da:	02d777b3          	remu	a5,a4,a3
200618de:	0542                	c.slli	a0,0x10
200618e0:	8141                	c.srli	a0,0x10
200618e2:	02d75733          	divu	a4,a4,a3
200618e6:	07c2                	c.slli	a5,0x10
200618e8:	8d5d                	c.or	a0,a5
200618ea:	02e60733          	mul	a4,a2,a4
200618ee:	00e57863          	bgeu	a0,a4,200618fe <__umoddi3+0xa6>
200618f2:	9542                	c.add	a0,a6
200618f4:	01056563          	bltu	a0,a6,200618fe <__umoddi3+0xa6>
200618f8:	00e57363          	bgeu	a0,a4,200618fe <__umoddi3+0xa6>
200618fc:	9542                	c.add	a0,a6
200618fe:	8d19                	c.sub	a0,a4
20061900:	01155533          	srl	a0,a0,a7
20061904:	4581                	c.li	a1,0
20061906:	8082                	c.jr	ra
20061908:	010006b7          	lui	a3,0x1000
2006190c:	47c1                	c.li	a5,16
2006190e:	f6d669e3          	bltu	a2,a3,20061880 <__umoddi3+0x28>
20061912:	47e1                	c.li	a5,24
20061914:	b7b5                	c.j	20061880 <__umoddi3+0x28>
20061916:	e601                	c.bnez	a2,2006191e <__umoddi3+0xc6>
20061918:	4685                	c.li	a3,1
2006191a:	02c6d833          	divu	a6,a3,a2
2006191e:	66c1                	c.lui	a3,0x10
20061920:	0ed87b63          	bgeu	a6,a3,20061a16 <__umoddi3+0x1be>
20061924:	0ff00693          	addi	a3,zero,255
20061928:	0106f363          	bgeu	a3,a6,2006192e <__umoddi3+0xd6>
2006192c:	47a1                	c.li	a5,8
2006192e:	00f856b3          	srl	a3,a6,a5
20061932:	9736                	c.add	a4,a3
20061934:	00074603          	lbu	a2,0(a4)
20061938:	410305b3          	sub	a1,t1,a6
2006193c:	963e                	c.add	a2,a5
2006193e:	02000793          	addi	a5,zero,32
20061942:	40c788b3          	sub	a7,a5,a2
20061946:	06c78a63          	beq	a5,a2,200619ba <__umoddi3+0x162>
2006194a:	01181833          	sll	a6,a6,a7
2006194e:	00c357b3          	srl	a5,t1,a2
20061952:	011315b3          	sll	a1,t1,a7
20061956:	01085313          	srli	t1,a6,0x10
2006195a:	00ced633          	srl	a2,t4,a2
2006195e:	8e4d                	c.or	a2,a1
20061960:	01081593          	slli	a1,a6,0x10
20061964:	81c1                	c.srli	a1,0x10
20061966:	011e9533          	sll	a0,t4,a7
2006196a:	0267d6b3          	divu	a3,a5,t1
2006196e:	0267f733          	remu	a4,a5,t1
20061972:	01065793          	srli	a5,a2,0x10
20061976:	02d586b3          	mul	a3,a1,a3
2006197a:	0742                	c.slli	a4,0x10
2006197c:	8f5d                	c.or	a4,a5
2006197e:	00d77863          	bgeu	a4,a3,2006198e <__umoddi3+0x136>
20061982:	9742                	c.add	a4,a6
20061984:	01076563          	bltu	a4,a6,2006198e <__umoddi3+0x136>
20061988:	00d77363          	bgeu	a4,a3,2006198e <__umoddi3+0x136>
2006198c:	9742                	c.add	a4,a6
2006198e:	40d706b3          	sub	a3,a4,a3
20061992:	0266f733          	remu	a4,a3,t1
20061996:	0642                	c.slli	a2,0x10
20061998:	8241                	c.srli	a2,0x10
2006199a:	0266d6b3          	divu	a3,a3,t1
2006199e:	0742                	c.slli	a4,0x10
200619a0:	02d586b3          	mul	a3,a1,a3
200619a4:	00c765b3          	or	a1,a4,a2
200619a8:	00d5f863          	bgeu	a1,a3,200619b8 <__umoddi3+0x160>
200619ac:	95c2                	c.add	a1,a6
200619ae:	0105e563          	bltu	a1,a6,200619b8 <__umoddi3+0x160>
200619b2:	00d5f363          	bgeu	a1,a3,200619b8 <__umoddi3+0x160>
200619b6:	95c2                	c.add	a1,a6
200619b8:	8d95                	c.sub	a1,a3
200619ba:	01085693          	srli	a3,a6,0x10
200619be:	02d5f733          	remu	a4,a1,a3
200619c2:	01081793          	slli	a5,a6,0x10
200619c6:	83c1                	c.srli	a5,0x10
200619c8:	01055613          	srli	a2,a0,0x10
200619cc:	02d5d5b3          	divu	a1,a1,a3
200619d0:	0742                	c.slli	a4,0x10
200619d2:	8f51                	c.or	a4,a2
200619d4:	02b785b3          	mul	a1,a5,a1
200619d8:	00b77863          	bgeu	a4,a1,200619e8 <__umoddi3+0x190>
200619dc:	9742                	c.add	a4,a6
200619de:	01076563          	bltu	a4,a6,200619e8 <__umoddi3+0x190>
200619e2:	00b77363          	bgeu	a4,a1,200619e8 <__umoddi3+0x190>
200619e6:	9742                	c.add	a4,a6
200619e8:	40b705b3          	sub	a1,a4,a1
200619ec:	02d5f733          	remu	a4,a1,a3
200619f0:	0542                	c.slli	a0,0x10
200619f2:	8141                	c.srli	a0,0x10
200619f4:	02d5d5b3          	divu	a1,a1,a3
200619f8:	02b785b3          	mul	a1,a5,a1
200619fc:	01071793          	slli	a5,a4,0x10
20061a00:	8d5d                	c.or	a0,a5
20061a02:	00b57863          	bgeu	a0,a1,20061a12 <__umoddi3+0x1ba>
20061a06:	9542                	c.add	a0,a6
20061a08:	01056563          	bltu	a0,a6,20061a12 <__umoddi3+0x1ba>
20061a0c:	00b57363          	bgeu	a0,a1,20061a12 <__umoddi3+0x1ba>
20061a10:	9542                	c.add	a0,a6
20061a12:	8d0d                	c.sub	a0,a1
20061a14:	b5f5                	c.j	20061900 <__umoddi3+0xa8>
20061a16:	010006b7          	lui	a3,0x1000
20061a1a:	47c1                	c.li	a5,16
20061a1c:	f0d869e3          	bltu	a6,a3,2006192e <__umoddi3+0xd6>
20061a20:	47e1                	c.li	a5,24
20061a22:	b731                	c.j	2006192e <__umoddi3+0xd6>
20061a24:	eed5e1e3          	bltu	a1,a3,20061906 <__umoddi3+0xae>
20061a28:	67c1                	c.lui	a5,0x10
20061a2a:	04f6f463          	bgeu	a3,a5,20061a72 <__umoddi3+0x21a>
20061a2e:	0ff00893          	addi	a7,zero,255
20061a32:	00d8b7b3          	sltu	a5,a7,a3
20061a36:	078e                	c.slli	a5,0x3
20061a38:	20065737          	lui	a4,0x20065
20061a3c:	00f6d833          	srl	a6,a3,a5
20061a40:	a5070713          	addi	a4,a4,-1456 # 20064a50 <__clz_tab>
20061a44:	9742                	c.add	a4,a6
20061a46:	00074883          	lbu	a7,0(a4)
20061a4a:	98be                	c.add	a7,a5
20061a4c:	02000793          	addi	a5,zero,32
20061a50:	41178833          	sub	a6,a5,a7
20061a54:	03179663          	bne	a5,a7,20061a80 <__umoddi3+0x228>
20061a58:	0066e463          	bltu	a3,t1,20061a60 <__umoddi3+0x208>
20061a5c:	eacee5e3          	bltu	t4,a2,20061906 <__umoddi3+0xae>
20061a60:	40ce8533          	sub	a0,t4,a2
20061a64:	40d305b3          	sub	a1,t1,a3
20061a68:	00aebeb3          	sltu	t4,t4,a0
20061a6c:	41d585b3          	sub	a1,a1,t4
20061a70:	bd59                	c.j	20061906 <__umoddi3+0xae>
20061a72:	01000737          	lui	a4,0x1000
20061a76:	47c1                	c.li	a5,16
20061a78:	fce6e0e3          	bltu	a3,a4,20061a38 <__umoddi3+0x1e0>
20061a7c:	47e1                	c.li	a5,24
20061a7e:	bf6d                	c.j	20061a38 <__umoddi3+0x1e0>
20061a80:	011657b3          	srl	a5,a2,a7
20061a84:	010696b3          	sll	a3,a3,a6
20061a88:	00d7ee33          	or	t3,a5,a3
20061a8c:	01135f33          	srl	t5,t1,a7
20061a90:	011ed7b3          	srl	a5,t4,a7
20061a94:	010e9533          	sll	a0,t4,a6
20061a98:	010e5e93          	srli	t4,t3,0x10
20061a9c:	010315b3          	sll	a1,t1,a6
20061aa0:	03df5333          	divu	t1,t5,t4
20061aa4:	8fcd                	c.or	a5,a1
20061aa6:	010e1593          	slli	a1,t3,0x10
20061aaa:	81c1                	c.srli	a1,0x10
20061aac:	0107d713          	srli	a4,a5,0x10
20061ab0:	01061633          	sll	a2,a2,a6
20061ab4:	03df76b3          	remu	a3,t5,t4
20061ab8:	02658f33          	mul	t5,a1,t1
20061abc:	06c2                	c.slli	a3,0x10
20061abe:	8f55                	c.or	a4,a3
20061ac0:	869a                	c.mv	a3,t1
20061ac2:	01e77c63          	bgeu	a4,t5,20061ada <__umoddi3+0x282>
20061ac6:	9772                	c.add	a4,t3
20061ac8:	fff30693          	addi	a3,t1,-1
20061acc:	01c76763          	bltu	a4,t3,20061ada <__umoddi3+0x282>
20061ad0:	01e77563          	bgeu	a4,t5,20061ada <__umoddi3+0x282>
20061ad4:	ffe30693          	addi	a3,t1,-2
20061ad8:	9772                	c.add	a4,t3
20061ada:	41e70733          	sub	a4,a4,t5
20061ade:	03d77333          	remu	t1,a4,t4
20061ae2:	07c2                	c.slli	a5,0x10
20061ae4:	83c1                	c.srli	a5,0x10
20061ae6:	03d75733          	divu	a4,a4,t4
20061aea:	0342                	c.slli	t1,0x10
20061aec:	00f36333          	or	t1,t1,a5
20061af0:	02e585b3          	mul	a1,a1,a4
20061af4:	87ba                	c.mv	a5,a4
20061af6:	00b37c63          	bgeu	t1,a1,20061b0e <__umoddi3+0x2b6>
20061afa:	9372                	c.add	t1,t3
20061afc:	fff70793          	addi	a5,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20061b00:	01c36763          	bltu	t1,t3,20061b0e <__umoddi3+0x2b6>
20061b04:	00b37563          	bgeu	t1,a1,20061b0e <__umoddi3+0x2b6>
20061b08:	ffe70793          	addi	a5,a4,-2
20061b0c:	9372                	c.add	t1,t3
20061b0e:	06c2                	c.slli	a3,0x10
20061b10:	6f41                	c.lui	t5,0x10
20061b12:	8edd                	c.or	a3,a5
20061b14:	40b305b3          	sub	a1,t1,a1
20061b18:	ffff0313          	addi	t1,t5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061b1c:	0066feb3          	and	t4,a3,t1
20061b20:	01065713          	srli	a4,a2,0x10
20061b24:	82c1                	c.srli	a3,0x10
20061b26:	00667333          	and	t1,a2,t1
20061b2a:	026e87b3          	mul	a5,t4,t1
20061b2e:	02668333          	mul	t1,a3,t1
20061b32:	02ee8eb3          	mul	t4,t4,a4
20061b36:	02e686b3          	mul	a3,a3,a4
20061b3a:	9e9a                	c.add	t4,t1
20061b3c:	0107d713          	srli	a4,a5,0x10
20061b40:	9776                	c.add	a4,t4
20061b42:	00677363          	bgeu	a4,t1,20061b48 <__umoddi3+0x2f0>
20061b46:	96fa                	c.add	a3,t5
20061b48:	01075313          	srli	t1,a4,0x10
20061b4c:	969a                	c.add	a3,t1
20061b4e:	6341                	c.lui	t1,0x10
20061b50:	137d                	c.addi	t1,-1
20061b52:	00677733          	and	a4,a4,t1
20061b56:	0742                	c.slli	a4,0x10
20061b58:	0067f7b3          	and	a5,a5,t1
20061b5c:	97ba                	c.add	a5,a4
20061b5e:	00d5e663          	bltu	a1,a3,20061b6a <__umoddi3+0x312>
20061b62:	00d59b63          	bne	a1,a3,20061b78 <__umoddi3+0x320>
20061b66:	00f57963          	bgeu	a0,a5,20061b78 <__umoddi3+0x320>
20061b6a:	40c78633          	sub	a2,a5,a2
20061b6e:	00c7b7b3          	sltu	a5,a5,a2
20061b72:	97f2                	c.add	a5,t3
20061b74:	8e9d                	c.sub	a3,a5
20061b76:	87b2                	c.mv	a5,a2
20061b78:	40f507b3          	sub	a5,a0,a5
20061b7c:	00f53533          	sltu	a0,a0,a5
20061b80:	8d95                	c.sub	a1,a3
20061b82:	8d89                	c.sub	a1,a0
20061b84:	011598b3          	sll	a7,a1,a7
20061b88:	0107d533          	srl	a0,a5,a6
20061b8c:	00a8e533          	or	a0,a7,a0
20061b90:	0105d5b3          	srl	a1,a1,a6
20061b94:	bb8d                	c.j	20061906 <__umoddi3+0xae>

20061b96 <atoi>:
20061b96:	4629                	c.li	a2,10
20061b98:	4581                	c.li	a1,0
20061b9a:	a1a5                	c.j	20062002 <strtol>

20061b9c <_atoi_r>:
20061b9c:	46a9                	c.li	a3,10
20061b9e:	4601                	c.li	a2,0
20061ba0:	a991                	c.j	20061ff4 <_strtol_r>

20061ba2 <__errno>:
20061ba2:	e941a503          	lw	a0,-364(gp) # 2006681c <_impure_ptr>
20061ba6:	8082                	c.jr	ra

20061ba8 <__libc_init_array>:
20061ba8:	1141                	c.addi	sp,-16
20061baa:	c422                	c.swsp	s0,8(sp)
20061bac:	c226                	c.swsp	s1,4(sp)
20061bae:	20065437          	lui	s0,0x20065
20061bb2:	200654b7          	lui	s1,0x20065
20061bb6:	da048793          	addi	a5,s1,-608 # 20064da0 <__frame_dummy_init_array_entry>
20061bba:	da040413          	addi	s0,s0,-608 # 20064da0 <__frame_dummy_init_array_entry>
20061bbe:	8c1d                	c.sub	s0,a5
20061bc0:	c04a                	c.swsp	s2,0(sp)
20061bc2:	c606                	c.swsp	ra,12(sp)
20061bc4:	8409                	c.srai	s0,0x2
20061bc6:	da048493          	addi	s1,s1,-608
20061bca:	4901                	c.li	s2,0
20061bcc:	02891763          	bne	s2,s0,20061bfa <__libc_init_array+0x52>
20061bd0:	200654b7          	lui	s1,0x20065
20061bd4:	20065437          	lui	s0,0x20065
20061bd8:	da048793          	addi	a5,s1,-608 # 20064da0 <__frame_dummy_init_array_entry>
20061bdc:	da440413          	addi	s0,s0,-604 # 20064da4 <__do_global_dtors_aux_fini_array_entry>
20061be0:	8c1d                	c.sub	s0,a5
20061be2:	8409                	c.srai	s0,0x2
20061be4:	da048493          	addi	s1,s1,-608
20061be8:	4901                	c.li	s2,0
20061bea:	00891d63          	bne	s2,s0,20061c04 <__libc_init_array+0x5c>
20061bee:	40b2                	c.lwsp	ra,12(sp)
20061bf0:	4422                	c.lwsp	s0,8(sp)
20061bf2:	4492                	c.lwsp	s1,4(sp)
20061bf4:	4902                	c.lwsp	s2,0(sp)
20061bf6:	0141                	c.addi	sp,16
20061bf8:	8082                	c.jr	ra
20061bfa:	409c                	c.lw	a5,0(s1)
20061bfc:	0905                	c.addi	s2,1
20061bfe:	0491                	c.addi	s1,4
20061c00:	9782                	c.jalr	a5
20061c02:	b7e9                	c.j	20061bcc <__libc_init_array+0x24>
20061c04:	409c                	c.lw	a5,0(s1)
20061c06:	0905                	c.addi	s2,1
20061c08:	0491                	c.addi	s1,4
20061c0a:	9782                	c.jalr	a5
20061c0c:	bff9                	c.j	20061bea <__libc_init_array+0x42>

20061c0e <cleanup_glue>:
20061c0e:	1141                	c.addi	sp,-16
20061c10:	c422                	c.swsp	s0,8(sp)
20061c12:	842e                	c.mv	s0,a1
20061c14:	418c                	c.lw	a1,0(a1)
20061c16:	c226                	c.swsp	s1,4(sp)
20061c18:	c606                	c.swsp	ra,12(sp)
20061c1a:	84aa                	c.mv	s1,a0
20061c1c:	c191                	c.beqz	a1,20061c20 <cleanup_glue+0x12>
20061c1e:	3fc5                	c.jal	20061c0e <cleanup_glue>
20061c20:	85a2                	c.mv	a1,s0
20061c22:	4422                	c.lwsp	s0,8(sp)
20061c24:	40b2                	c.lwsp	ra,12(sp)
20061c26:	8526                	c.mv	a0,s1
20061c28:	4492                	c.lwsp	s1,4(sp)
20061c2a:	0141                	c.addi	sp,16
20061c2c:	fadf806f          	jal	zero,2005abd8 <__wrap__free_r>

20061c30 <_reclaim_reent>:
20061c30:	e941a783          	lw	a5,-364(gp) # 2006681c <_impure_ptr>
20061c34:	0ea78263          	beq	a5,a0,20061d18 <_reclaim_reent+0xe8>
20061c38:	515c                	c.lw	a5,36(a0)
20061c3a:	1101                	c.addi	sp,-32
20061c3c:	cc22                	c.swsp	s0,24(sp)
20061c3e:	ce06                	c.swsp	ra,28(sp)
20061c40:	ca26                	c.swsp	s1,20(sp)
20061c42:	c84a                	c.swsp	s2,16(sp)
20061c44:	c64e                	c.swsp	s3,12(sp)
20061c46:	842a                	c.mv	s0,a0
20061c48:	cf81                	c.beqz	a5,20061c60 <_reclaim_reent+0x30>
20061c4a:	47dc                	c.lw	a5,12(a5)
20061c4c:	4481                	c.li	s1,0
20061c4e:	08000913          	addi	s2,zero,128
20061c52:	efc9                	c.bnez	a5,20061cec <_reclaim_reent+0xbc>
20061c54:	505c                	c.lw	a5,36(s0)
20061c56:	438c                	c.lw	a1,0(a5)
20061c58:	c581                	c.beqz	a1,20061c60 <_reclaim_reent+0x30>
20061c5a:	8522                	c.mv	a0,s0
20061c5c:	f7df80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061c60:	484c                	c.lw	a1,20(s0)
20061c62:	c581                	c.beqz	a1,20061c6a <_reclaim_reent+0x3a>
20061c64:	8522                	c.mv	a0,s0
20061c66:	f73f80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061c6a:	504c                	c.lw	a1,36(s0)
20061c6c:	c581                	c.beqz	a1,20061c74 <_reclaim_reent+0x44>
20061c6e:	8522                	c.mv	a0,s0
20061c70:	f69f80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061c74:	5c0c                	c.lw	a1,56(s0)
20061c76:	c581                	c.beqz	a1,20061c7e <_reclaim_reent+0x4e>
20061c78:	8522                	c.mv	a0,s0
20061c7a:	f5ff80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061c7e:	5c4c                	c.lw	a1,60(s0)
20061c80:	c581                	c.beqz	a1,20061c88 <_reclaim_reent+0x58>
20061c82:	8522                	c.mv	a0,s0
20061c84:	f55f80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061c88:	402c                	c.lw	a1,64(s0)
20061c8a:	c581                	c.beqz	a1,20061c92 <_reclaim_reent+0x62>
20061c8c:	8522                	c.mv	a0,s0
20061c8e:	f4bf80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061c92:	0ec42583          	lw	a1,236(s0)
20061c96:	c581                	c.beqz	a1,20061c9e <_reclaim_reent+0x6e>
20061c98:	8522                	c.mv	a0,s0
20061c9a:	f3ff80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061c9e:	0e842583          	lw	a1,232(s0)
20061ca2:	c581                	c.beqz	a1,20061caa <_reclaim_reent+0x7a>
20061ca4:	8522                	c.mv	a0,s0
20061ca6:	f33f80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061caa:	443c                	c.lw	a5,72(s0)
20061cac:	c799                	c.beqz	a5,20061cba <_reclaim_reent+0x8a>
20061cae:	0887a583          	lw	a1,136(a5) # 10088 <mbedtls_ecp_check_pub_priv+0x1aa>
20061cb2:	c581                	c.beqz	a1,20061cba <_reclaim_reent+0x8a>
20061cb4:	8522                	c.mv	a0,s0
20061cb6:	f23f80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061cba:	584c                	c.lw	a1,52(s0)
20061cbc:	c581                	c.beqz	a1,20061cc4 <_reclaim_reent+0x94>
20061cbe:	8522                	c.mv	a0,s0
20061cc0:	f19f80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061cc4:	4c1c                	c.lw	a5,24(s0)
20061cc6:	c3b1                	c.beqz	a5,20061d0a <_reclaim_reent+0xda>
20061cc8:	541c                	c.lw	a5,40(s0)
20061cca:	8522                	c.mv	a0,s0
20061ccc:	9782                	c.jalr	a5
20061cce:	0d842583          	lw	a1,216(s0)
20061cd2:	cd85                	c.beqz	a1,20061d0a <_reclaim_reent+0xda>
20061cd4:	8522                	c.mv	a0,s0
20061cd6:	4462                	c.lwsp	s0,24(sp)
20061cd8:	40f2                	c.lwsp	ra,28(sp)
20061cda:	44d2                	c.lwsp	s1,20(sp)
20061cdc:	4942                	c.lwsp	s2,16(sp)
20061cde:	49b2                	c.lwsp	s3,12(sp)
20061ce0:	6105                	c.addi16sp	sp,32
20061ce2:	b735                	c.j	20061c0e <cleanup_glue>
20061ce4:	95a6                	c.add	a1,s1
20061ce6:	418c                	c.lw	a1,0(a1)
20061ce8:	e991                	c.bnez	a1,20061cfc <_reclaim_reent+0xcc>
20061cea:	0491                	c.addi	s1,4
20061cec:	505c                	c.lw	a5,36(s0)
20061cee:	47cc                	c.lw	a1,12(a5)
20061cf0:	ff249ae3          	bne	s1,s2,20061ce4 <_reclaim_reent+0xb4>
20061cf4:	8522                	c.mv	a0,s0
20061cf6:	ee3f80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061cfa:	bfa9                	c.j	20061c54 <_reclaim_reent+0x24>
20061cfc:	0005a983          	lw	s3,0(a1)
20061d00:	8522                	c.mv	a0,s0
20061d02:	ed7f80ef          	jal	ra,2005abd8 <__wrap__free_r>
20061d06:	85ce                	c.mv	a1,s3
20061d08:	b7c5                	c.j	20061ce8 <_reclaim_reent+0xb8>
20061d0a:	40f2                	c.lwsp	ra,28(sp)
20061d0c:	4462                	c.lwsp	s0,24(sp)
20061d0e:	44d2                	c.lwsp	s1,20(sp)
20061d10:	4942                	c.lwsp	s2,16(sp)
20061d12:	49b2                	c.lwsp	s3,12(sp)
20061d14:	6105                	c.addi16sp	sp,32
20061d16:	8082                	c.jr	ra
20061d18:	8082                	c.jr	ra

20061d1a <strcpy>:
20061d1a:	00b56733          	or	a4,a0,a1
20061d1e:	8b0d                	c.andi	a4,3
20061d20:	87aa                	c.mv	a5,a0
20061d22:	e729                	c.bnez	a4,20061d6c <strcpy+0x52>
20061d24:	7f7f86b7          	lui	a3,0x7f7f8
20061d28:	f7f68693          	addi	a3,a3,-129 # 7f7f7f7f <__kr4_psram_text_start__+0x1f677f5f>
20061d2c:	587d                	c.li	a6,-1
20061d2e:	4190                	c.lw	a2,0(a1)
20061d30:	00d67733          	and	a4,a2,a3
20061d34:	9736                	c.add	a4,a3
20061d36:	8f51                	c.or	a4,a2
20061d38:	8f55                	c.or	a4,a3
20061d3a:	03070463          	beq	a4,a6,20061d62 <strcpy+0x48>
20061d3e:	0005c603          	lbu	a2,0(a1)
20061d42:	0015c683          	lbu	a3,1(a1)
20061d46:	0025c703          	lbu	a4,2(a1)
20061d4a:	00c78023          	sb	a2,0(a5)
20061d4e:	c615                	c.beqz	a2,20061d7a <strcpy+0x60>
20061d50:	00d780a3          	sb	a3,1(a5)
20061d54:	c29d                	c.beqz	a3,20061d7a <strcpy+0x60>
20061d56:	00e78123          	sb	a4,2(a5)
20061d5a:	c305                	c.beqz	a4,20061d7a <strcpy+0x60>
20061d5c:	000781a3          	sb	zero,3(a5)
20061d60:	8082                	c.jr	ra
20061d62:	0791                	c.addi	a5,4
20061d64:	0591                	c.addi	a1,4
20061d66:	fec7ae23          	sw	a2,-4(a5)
20061d6a:	b7d1                	c.j	20061d2e <strcpy+0x14>
20061d6c:	0005c703          	lbu	a4,0(a1)
20061d70:	0785                	c.addi	a5,1
20061d72:	0585                	c.addi	a1,1
20061d74:	fee78fa3          	sb	a4,-1(a5)
20061d78:	fb75                	c.bnez	a4,20061d6c <strcpy+0x52>
20061d7a:	8082                	c.jr	ra

20061d7c <strncmp>:
20061d7c:	c625                	c.beqz	a2,20061de4 <strncmp+0x68>
20061d7e:	00b567b3          	or	a5,a0,a1
20061d82:	8b8d                	c.andi	a5,3
20061d84:	e385                	c.bnez	a5,20061da4 <strncmp+0x28>
20061d86:	feff06b7          	lui	a3,0xfeff0
20061d8a:	80808837          	lui	a6,0x80808
20061d8e:	488d                	c.li	a7,3
20061d90:	eff68693          	addi	a3,a3,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061d94:	08080813          	addi	a6,a6,128 # 80808080 <__ctrace_end__+0xb6808080>
20061d98:	00c8f663          	bgeu	a7,a2,20061da4 <strncmp+0x28>
20061d9c:	411c                	c.lw	a5,0(a0)
20061d9e:	4198                	c.lw	a4,0(a1)
20061da0:	02e78163          	beq	a5,a4,20061dc2 <strncmp+0x46>
20061da4:	167d                	c.addi	a2,-1
20061da6:	4701                	c.li	a4,0
20061da8:	00e507b3          	add	a5,a0,a4
20061dac:	00e586b3          	add	a3,a1,a4
20061db0:	0007c783          	lbu	a5,0(a5)
20061db4:	0006c683          	lbu	a3,0(a3)
20061db8:	02d78263          	beq	a5,a3,20061ddc <strncmp+0x60>
20061dbc:	40d78533          	sub	a0,a5,a3
20061dc0:	8082                	c.jr	ra
20061dc2:	1671                	c.addi	a2,-4
20061dc4:	c205                	c.beqz	a2,20061de4 <strncmp+0x68>
20061dc6:	00d78733          	add	a4,a5,a3
20061dca:	fff7c793          	xori	a5,a5,-1
20061dce:	8ff9                	c.and	a5,a4
20061dd0:	0107f7b3          	and	a5,a5,a6
20061dd4:	eb81                	c.bnez	a5,20061de4 <strncmp+0x68>
20061dd6:	0511                	c.addi	a0,4
20061dd8:	0591                	c.addi	a1,4
20061dda:	bf7d                	c.j	20061d98 <strncmp+0x1c>
20061ddc:	00c70463          	beq	a4,a2,20061de4 <strncmp+0x68>
20061de0:	0705                	c.addi	a4,1
20061de2:	f3f9                	c.bnez	a5,20061da8 <strncmp+0x2c>
20061de4:	4501                	c.li	a0,0
20061de6:	8082                	c.jr	ra

20061de8 <strncpy>:
20061de8:	00a5e733          	or	a4,a1,a0
20061dec:	8b0d                	c.andi	a4,3
20061dee:	87aa                	c.mv	a5,a0
20061df0:	e701                	c.bnez	a4,20061df8 <strncpy+0x10>
20061df2:	470d                	c.li	a4,3
20061df4:	04c76163          	bltu	a4,a2,20061e36 <strncpy+0x4e>
20061df8:	ca09                	c.beqz	a2,20061e0a <strncpy+0x22>
20061dfa:	0005c703          	lbu	a4,0(a1)
20061dfe:	0585                	c.addi	a1,1
20061e00:	0785                	c.addi	a5,1
20061e02:	fee78fa3          	sb	a4,-1(a5)
20061e06:	167d                	c.addi	a2,-1
20061e08:	fb65                	c.bnez	a4,20061df8 <strncpy+0x10>
20061e0a:	963e                	c.add	a2,a5
20061e0c:	02f61f63          	bne	a2,a5,20061e4a <strncpy+0x62>
20061e10:	8082                	c.jr	ra
20061e12:	1671                	c.addi	a2,-4
20061e14:	0107a023          	sw	a6,0(a5)
20061e18:	0791                	c.addi	a5,4
20061e1a:	0591                	c.addi	a1,4
20061e1c:	fcce7ee3          	bgeu	t3,a2,20061df8 <strncpy+0x10>
20061e20:	0005a803          	lw	a6,0(a1)
20061e24:	01180733          	add	a4,a6,a7
20061e28:	fff84313          	xori	t1,a6,-1
20061e2c:	00677733          	and	a4,a4,t1
20061e30:	8f75                	c.and	a4,a3
20061e32:	d365                	c.beqz	a4,20061e12 <strncpy+0x2a>
20061e34:	b7d1                	c.j	20061df8 <strncpy+0x10>
20061e36:	feff08b7          	lui	a7,0xfeff0
20061e3a:	808086b7          	lui	a3,0x80808
20061e3e:	eff88893          	addi	a7,a7,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061e42:	08068693          	addi	a3,a3,128 # 80808080 <__ctrace_end__+0xb6808080>
20061e46:	4e0d                	c.li	t3,3
20061e48:	bfe1                	c.j	20061e20 <strncpy+0x38>
20061e4a:	0785                	c.addi	a5,1
20061e4c:	fe078fa3          	sb	zero,-1(a5)
20061e50:	bf75                	c.j	20061e0c <strncpy+0x24>

20061e52 <strsep>:
20061e52:	862a                	c.mv	a2,a0
20061e54:	4108                	c.lw	a0,0(a0)
20061e56:	4681                	c.li	a3,0
20061e58:	a009                	c.j	20061e5a <__strtok_r>

20061e5a <__strtok_r>:
20061e5a:	e119                	c.bnez	a0,20061e60 <__strtok_r+0x6>
20061e5c:	4208                	c.lw	a0,0(a2)
20061e5e:	c939                	c.beqz	a0,20061eb4 <__strtok_r+0x5a>
20061e60:	87aa                	c.mv	a5,a0
20061e62:	853e                	c.mv	a0,a5
20061e64:	0007c803          	lbu	a6,0(a5)
20061e68:	0785                	c.addi	a5,1
20061e6a:	872e                	c.mv	a4,a1
20061e6c:	00074883          	lbu	a7,0(a4)
20061e70:	0705                	c.addi	a4,1
20061e72:	00089863          	bne	a7,zero,20061e82 <__strtok_r+0x28>
20061e76:	02081063          	bne	a6,zero,20061e96 <__strtok_r+0x3c>
20061e7a:	00062023          	sw	zero,0(a2)
20061e7e:	4501                	c.li	a0,0
20061e80:	8082                	c.jr	ra
20061e82:	ff1815e3          	bne	a6,a7,20061e6c <__strtok_r+0x12>
20061e86:	fef1                	c.bnez	a3,20061e62 <__strtok_r+0x8>
20061e88:	c21c                	c.sw	a5,0(a2)
20061e8a:	00050023          	sb	zero,0(a0)
20061e8e:	8082                	c.jr	ra
20061e90:	4781                	c.li	a5,0
20061e92:	a839                	c.j	20061eb0 <__strtok_r+0x56>
20061e94:	e689                	c.bnez	a3,20061e9e <__strtok_r+0x44>
20061e96:	0007c803          	lbu	a6,0(a5)
20061e9a:	0785                	c.addi	a5,1
20061e9c:	872e                	c.mv	a4,a1
20061e9e:	00074683          	lbu	a3,0(a4)
20061ea2:	0705                	c.addi	a4,1
20061ea4:	fed818e3          	bne	a6,a3,20061e94 <__strtok_r+0x3a>
20061ea8:	fe0804e3          	beq	a6,zero,20061e90 <__strtok_r+0x36>
20061eac:	fe078fa3          	sb	zero,-1(a5)
20061eb0:	c21c                	c.sw	a5,0(a2)
20061eb2:	8082                	c.jr	ra
20061eb4:	8082                	c.jr	ra

20061eb6 <strtok_r>:
20061eb6:	4685                	c.li	a3,1
20061eb8:	b74d                	c.j	20061e5a <__strtok_r>

20061eba <_strtol_l.constprop.0>:
20061eba:	4785                	c.li	a5,1
20061ebc:	0af68863          	beq	a3,a5,20061f6c <_strtol_l.constprop.0+0xb2>
20061ec0:	02400793          	addi	a5,zero,36
20061ec4:	872e                	c.mv	a4,a1
20061ec6:	0ad7e363          	bltu	a5,a3,20061f6c <_strtol_l.constprop.0+0xb2>
20061eca:	20065837          	lui	a6,0x20065
20061ece:	8eaa                	c.mv	t4,a0
20061ed0:	b5180813          	addi	a6,a6,-1199 # 20064b51 <_ctype_+0x1>
20061ed4:	88ba                	c.mv	a7,a4
20061ed6:	00074783          	lbu	a5,0(a4)
20061eda:	0705                	c.addi	a4,1
20061edc:	01078533          	add	a0,a5,a6
20061ee0:	00054503          	lbu	a0,0(a0)
20061ee4:	8921                	c.andi	a0,8
20061ee6:	f57d                	c.bnez	a0,20061ed4 <_strtol_l.constprop.0+0x1a>
20061ee8:	02d00513          	addi	a0,zero,45
20061eec:	08a79a63          	bne	a5,a0,20061f80 <_strtol_l.constprop.0+0xc6>
20061ef0:	00074783          	lbu	a5,0(a4)
20061ef4:	4e05                	c.li	t3,1
20061ef6:	00288713          	addi	a4,a7,2
20061efa:	c6f5                	c.beqz	a3,20061fe6 <_strtol_l.constprop.0+0x12c>
20061efc:	4541                	c.li	a0,16
20061efe:	02a69263          	bne	a3,a0,20061f22 <_strtol_l.constprop.0+0x68>
20061f02:	03000513          	addi	a0,zero,48
20061f06:	00a79e63          	bne	a5,a0,20061f22 <_strtol_l.constprop.0+0x68>
20061f0a:	00074783          	lbu	a5,0(a4)
20061f0e:	05800513          	addi	a0,zero,88
20061f12:	0df7f793          	andi	a5,a5,223
20061f16:	0ca79363          	bne	a5,a0,20061fdc <_strtol_l.constprop.0+0x122>
20061f1a:	00174783          	lbu	a5,1(a4)
20061f1e:	46c1                	c.li	a3,16
20061f20:	0709                	c.addi	a4,2
20061f22:	800008b7          	lui	a7,0x80000
20061f26:	fff8c893          	xori	a7,a7,-1
20061f2a:	98f2                	c.add	a7,t3
20061f2c:	02d8f2b3          	remu	t0,a7,a3
20061f30:	4301                	c.li	t1,0
20061f32:	4501                	c.li	a0,0
20061f34:	43a5                	c.li	t2,9
20061f36:	4fe5                	c.li	t6,25
20061f38:	02d8df33          	divu	t5,a7,a3
20061f3c:	fd078813          	addi	a6,a5,-48
20061f40:	0503ea63          	bltu	t2,a6,20061f94 <_strtol_l.constprop.0+0xda>
20061f44:	87c2                	c.mv	a5,a6
20061f46:	06d7d763          	bge	a5,a3,20061fb4 <_strtol_l.constprop.0+0xfa>
20061f4a:	06034363          	blt	t1,zero,20061fb0 <_strtol_l.constprop.0+0xf6>
20061f4e:	537d                	c.li	t1,-1
20061f50:	00af6a63          	bltu	t5,a0,20061f64 <_strtol_l.constprop.0+0xaa>
20061f54:	00af1463          	bne	t5,a0,20061f5c <_strtol_l.constprop.0+0xa2>
20061f58:	00f2c663          	blt	t0,a5,20061f64 <_strtol_l.constprop.0+0xaa>
20061f5c:	4305                	c.li	t1,1
20061f5e:	02a68533          	mul	a0,a3,a0
20061f62:	953e                	c.add	a0,a5
20061f64:	0705                	c.addi	a4,1
20061f66:	fff74783          	lbu	a5,-1(a4)
20061f6a:	bfc9                	c.j	20061f3c <_strtol_l.constprop.0+0x82>
20061f6c:	1141                	c.addi	sp,-16
20061f6e:	c606                	c.swsp	ra,12(sp)
20061f70:	c33ff0ef          	jal	ra,20061ba2 <__errno>
20061f74:	40b2                	c.lwsp	ra,12(sp)
20061f76:	47d9                	c.li	a5,22
20061f78:	c11c                	c.sw	a5,0(a0)
20061f7a:	4501                	c.li	a0,0
20061f7c:	0141                	c.addi	sp,16
20061f7e:	8082                	c.jr	ra
20061f80:	02b00513          	addi	a0,zero,43
20061f84:	4e01                	c.li	t3,0
20061f86:	f6a79ae3          	bne	a5,a0,20061efa <_strtol_l.constprop.0+0x40>
20061f8a:	00074783          	lbu	a5,0(a4)
20061f8e:	00288713          	addi	a4,a7,2 # 80000002 <__ctrace_end__+0xb6000002>
20061f92:	b7a5                	c.j	20061efa <_strtol_l.constprop.0+0x40>
20061f94:	fbf78813          	addi	a6,a5,-65
20061f98:	010fe563          	bltu	t6,a6,20061fa2 <_strtol_l.constprop.0+0xe8>
20061f9c:	fc978793          	addi	a5,a5,-55
20061fa0:	b75d                	c.j	20061f46 <_strtol_l.constprop.0+0x8c>
20061fa2:	f9f78813          	addi	a6,a5,-97
20061fa6:	010fe763          	bltu	t6,a6,20061fb4 <_strtol_l.constprop.0+0xfa>
20061faa:	fa978793          	addi	a5,a5,-87
20061fae:	bf61                	c.j	20061f46 <_strtol_l.constprop.0+0x8c>
20061fb0:	537d                	c.li	t1,-1
20061fb2:	bf4d                	c.j	20061f64 <_strtol_l.constprop.0+0xaa>
20061fb4:	00035963          	bge	t1,zero,20061fc6 <_strtol_l.constprop.0+0x10c>
20061fb8:	02200793          	addi	a5,zero,34
20061fbc:	00fea023          	sw	a5,0(t4)
20061fc0:	8546                	c.mv	a0,a7
20061fc2:	ea09                	c.bnez	a2,20061fd4 <_strtol_l.constprop.0+0x11a>
20061fc4:	8082                	c.jr	ra
20061fc6:	000e0463          	beq	t3,zero,20061fce <_strtol_l.constprop.0+0x114>
20061fca:	40a00533          	sub	a0,zero,a0
20061fce:	c215                	c.beqz	a2,20061ff2 <_strtol_l.constprop.0+0x138>
20061fd0:	00030463          	beq	t1,zero,20061fd8 <_strtol_l.constprop.0+0x11e>
20061fd4:	fff70593          	addi	a1,a4,-1
20061fd8:	c20c                	c.sw	a1,0(a2)
20061fda:	8082                	c.jr	ra
20061fdc:	03000793          	addi	a5,zero,48
20061fe0:	f2a9                	c.bnez	a3,20061f22 <_strtol_l.constprop.0+0x68>
20061fe2:	46a1                	c.li	a3,8
20061fe4:	bf3d                	c.j	20061f22 <_strtol_l.constprop.0+0x68>
20061fe6:	03000513          	addi	a0,zero,48
20061fea:	f2a780e3          	beq	a5,a0,20061f0a <_strtol_l.constprop.0+0x50>
20061fee:	46a9                	c.li	a3,10
20061ff0:	bf0d                	c.j	20061f22 <_strtol_l.constprop.0+0x68>
20061ff2:	8082                	c.jr	ra

20061ff4 <_strtol_r>:
20061ff4:	b5d9                	c.j	20061eba <_strtol_l.constprop.0>

20061ff6 <strtol_l>:
20061ff6:	86b2                	c.mv	a3,a2
20061ff8:	862e                	c.mv	a2,a1
20061ffa:	85aa                	c.mv	a1,a0
20061ffc:	e941a503          	lw	a0,-364(gp) # 2006681c <_impure_ptr>
20062000:	bd6d                	c.j	20061eba <_strtol_l.constprop.0>

20062002 <strtol>:
20062002:	86b2                	c.mv	a3,a2
20062004:	862e                	c.mv	a2,a1
20062006:	85aa                	c.mv	a1,a0
20062008:	e941a503          	lw	a0,-364(gp) # 2006681c <_impure_ptr>
2006200c:	b57d                	c.j	20061eba <_strtol_l.constprop.0>
2006200e:	0000                	c.unimp
20062010:	6d6f682f          	0x6d6f682f
20062014:	2f65                	c.jal	200627cc <pmap_func+0x3a0>
20062016:	7561                	c.lui	a0,0xffff8
20062018:	6f74                	c.flw	fa3,92(a4)
2006201a:	6a5f 6e65 696b      	0x696b6e656a5f
20062020:	736e                	c.flwsp	ft6,248(sp)
20062022:	775f 616c 356e      	0x356e616c775f
20062028:	544f492f          	0x544f492f
2006202c:	415f 5354 722f      	0x722f5354415f
20062032:	7065                	c.lui	zero,0xffff9
20062034:	722f736f          	jal	t1,20159756 <__ap_sram_heap_start+0xd9756>
20062038:	6c65                	c.lui	s8,0x19
2006203a:	6165                	c.addi16sp	sp,112
2006203c:	6d2d6573          	csrrsi	a0,0x6d2,26
20062040:	7361                	c.lui	t1,0xffff8
20062042:	6574                	c.flw	fa3,76(a0)
20062044:	2f72                	c.fldsp	ft10,280(sp)
20062046:	72756f73          	csrrsi	t5,0x727,10
2006204a:	612f6563          	bltu	t5,s2,20062654 <pmap_func+0x228>
2006204e:	656d                	c.lui	a0,0x1b
20062050:	6162                	c.flwsp	ft2,24(sp)
20062052:	696c                	c.flw	fa1,84(a0)
20062054:	6574                	c.flw	fa3,76(a0)
20062056:	675f 6363 705f      	0x705f6363675f
2006205c:	6f72                	c.flwsp	ft10,28(sp)
2006205e:	656a                	c.flwsp	fa0,152(sp)
20062060:	702f7463          	bgeu	t5,sp,20062768 <pmap_func+0x33c>
20062064:	6f72                	c.flwsp	ft10,28(sp)
20062066:	656a                	c.flwsp	fa0,152(sp)
20062068:	6b5f7463          	bgeu	t5,s5,20062710 <pmap_func+0x2e4>
2006206c:	3472                	c.fldsp	fs0,312(sp)
2006206e:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
20062072:	2e2e2f6b          	0x2e2e2f6b
20062076:	2f2e2e2f          	0x2f2e2e2f
2006207a:	2e2e                	c.fldsp	ft8,200(sp)
2006207c:	6d6f632f          	0x6d6f632f
20062080:	6f70                	c.flw	fa2,92(a4)
20062082:	656e                	c.flwsp	fa0,216(sp)
20062084:	746e                	c.flwsp	fs0,248(sp)
20062086:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
2006208a:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
2006208e:	6162                	c.flwsp	ft2,24(sp)
20062090:	696c                	c.flw	fa1,84(a0)
20062092:	6574                	c.flw	fa3,76(a0)
20062094:	6c77662f          	0x6c77662f
20062098:	6269                	c.lui	tp,0x1a
2006209a:	6d61722f          	0x6d61722f
2006209e:	635f 6d6f 6f6d      	0x6f6d6d6f635f
200620a4:	2f6e                	c.fldsp	ft10,216(sp)
200620a6:	6d61                	c.lui	s10,0x18
200620a8:	6265                	c.lui	tp,0x19
200620aa:	5f61                	c.li	t5,-8
200620ac:	6c66                	c.flwsp	fs8,88(sp)
200620ae:	7361                	c.lui	t1,0xffff8
200620b0:	5f68                	c.lw	a0,124(a4)
200620b2:	6172                	c.flwsp	ft2,28(sp)
200620b4:	2e6d                	c.jal	2006246e <pmap_func+0x42>
200620b6:	68430063          	beq	t1,tp,20062736 <pmap_func+0x30a>
200620ba:	6e61                	c.lui	t3,0x18
200620bc:	656e                	c.flwsp	fa0,216(sp)
200620be:	206c                	c.fld	fa1,192(s0)
200620c0:	666e6f43          	fmadd.q	ft10,ft8,ft6,fa2,unknown
200620c4:	696c                	c.flw	fa1,84(a0)
200620c6:	66207463          	bgeu	zero,sp,2006272e <pmap_func+0x302>
200620ca:	4320726f          	jal	tp,200694fc <__bdram_heap_buffer_start__+0x9fc>
200620ce:	5550                	c.lw	a2,44(a0)
200620d0:	2520                	c.fld	fs0,72(a0)
200620d2:	756c                	c.flw	fa1,108(a0)
200620d4:	4320                	c.lw	s0,64(a4)
200620d6:	6168                	c.flw	fa0,68(a0)
200620d8:	6e6e                	c.flwsp	ft8,216(sp)
200620da:	6c65                	c.lui	s8,0x19
200620dc:	2520                	c.fld	fs0,72(a0)
200620de:	756c                	c.flw	fa1,108(a0)
200620e0:	2120                	c.fld	fs0,64(a0)
200620e2:	4920                	c.lw	s0,80(a0)
200620e4:	726f6e67          	0x726f6e67
200620e8:	2065                	c.jal	20062190 <strtol+0x18e>
200620ea:	6649                	c.lui	a2,0x12
200620ec:	4320                	c.lw	s0,64(a4)
200620ee:	5550                	c.lw	a2,44(a0)
200620f0:	4820                	c.lw	s0,80(s0)
200620f2:	7361                	c.lui	t1,0xffff8
200620f4:	5220                	c.lw	s0,96(a2)
200620f6:	7365                	c.lui	t1,0xffff9
200620f8:	7465                	c.lui	s0,0xffff9
200620fa:	0a0d                	c.addi	s4,3
200620fc:	0000                	c.unimp
200620fe:	0000                	c.unimp
20062100:	5049                	c.li	zero,-14
20062102:	49200043          	fmadd.s	ft0,ft0,fs2,fs1,rne
20062106:	4350                	c.lw	a2,4(a4)
20062108:	5220                	c.lw	s0,96(a2)
2006210a:	7165                	c.addi16sp	sp,-400
2006210c:	6575                	c.lui	a0,0x1d
2006210e:	54207473          	csrrci	s0,0x542,0
20062112:	6d69                	c.lui	s10,0x1a
20062114:	6f65                	c.lui	t5,0x19
20062116:	7475                	c.lui	s0,0xffffd
20062118:	0a0d                	c.addi	s4,3
2006211a:	0000                	c.unimp
2006211c:	4920                	c.lw	s0,80(a0)
2006211e:	4350                	c.lw	a2,4(a4)
20062120:	4720                	c.lw	s0,72(a4)
20062122:	7465                	c.lui	s0,0xffff9
20062124:	5320                	c.lw	s0,96(a4)
20062126:	6d65                	c.lui	s10,0x19
20062128:	7061                	c.lui	zero,0xffff8
2006212a:	6f68                	c.flw	fa0,92(a4)
2006212c:	6572                	c.flwsp	fa0,28(sp)
2006212e:	5420                	c.lw	s0,104(s0)
20062130:	6d69                	c.lui	s10,0x1a
20062132:	6f65                	c.lui	t5,0x19
20062134:	7475                	c.lui	s0,0xffffd
20062136:	0a0d                	c.addi	s4,3
20062138:	0000                	c.unimp
2006213a:	0000                	c.unimp
2006213c:	6d6f682f          	0x6d6f682f
20062140:	2f65                	c.jal	200628f8 <pmap_func+0x4cc>
20062142:	7561                	c.lui	a0,0xffff8
20062144:	6f74                	c.flw	fa3,92(a4)
20062146:	6a5f 6e65 696b      	0x696b6e656a5f
2006214c:	736e                	c.flwsp	ft6,248(sp)
2006214e:	775f 616c 356e      	0x356e616c775f
20062154:	544f492f          	0x544f492f
20062158:	415f 5354 722f      	0x722f5354415f
2006215e:	7065                	c.lui	zero,0xffff9
20062160:	722f736f          	jal	t1,20159882 <__ap_sram_heap_start+0xd9882>
20062164:	6c65                	c.lui	s8,0x19
20062166:	6165                	c.addi16sp	sp,112
20062168:	6d2d6573          	csrrsi	a0,0x6d2,26
2006216c:	7361                	c.lui	t1,0xffff8
2006216e:	6574                	c.flw	fa3,76(a0)
20062170:	2f72                	c.fldsp	ft10,280(sp)
20062172:	72756f73          	csrrsi	t5,0x727,10
20062176:	612f6563          	bltu	t5,s2,20062780 <pmap_func+0x354>
2006217a:	656d                	c.lui	a0,0x1b
2006217c:	6162                	c.flwsp	ft2,24(sp)
2006217e:	696c                	c.flw	fa1,84(a0)
20062180:	6574                	c.flw	fa3,76(a0)
20062182:	675f 6363 705f      	0x705f6363675f
20062188:	6f72                	c.flwsp	ft10,28(sp)
2006218a:	656a                	c.flwsp	fa0,152(sp)
2006218c:	702f7463          	bgeu	t5,sp,20062894 <pmap_func+0x468>
20062190:	6f72                	c.flwsp	ft10,28(sp)
20062192:	656a                	c.flwsp	fa0,152(sp)
20062194:	6b5f7463          	bgeu	t5,s5,2006283c <pmap_func+0x410>
20062198:	3472                	c.fldsp	fs0,312(sp)
2006219a:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
2006219e:	2e2e2f6b          	0x2e2e2f6b
200621a2:	2f2e2e2f          	0x2f2e2e2f
200621a6:	2e2e                	c.fldsp	ft8,200(sp)
200621a8:	6d6f632f          	0x6d6f632f
200621ac:	6f70                	c.flw	fa2,92(a4)
200621ae:	656e                	c.flwsp	fa0,216(sp)
200621b0:	746e                	c.flwsp	fs0,248(sp)
200621b2:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
200621b6:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
200621ba:	6162                	c.flwsp	ft2,24(sp)
200621bc:	696c                	c.flw	fa1,84(a0)
200621be:	6574                	c.flw	fa3,76(a0)
200621c0:	6c77662f          	0x6c77662f
200621c4:	6269                	c.lui	tp,0x1a
200621c6:	6d61722f          	0x6d61722f
200621ca:	635f 6d6f 6f6d      	0x6f6d6d6f635f
200621d0:	2f6e                	c.fldsp	ft10,216(sp)
200621d2:	6d61                	c.lui	s10,0x18
200621d4:	6265                	c.lui	tp,0x19
200621d6:	5f61                	c.li	t5,-8
200621d8:	7069                	c.lui	zero,0xffffa
200621da:	70615f63          	bge	sp,t1,200628f8 <pmap_func+0x4cc>
200621de:	2e69                	c.jal	20062578 <pmap_func+0x14c>
200621e0:	00000063          	beq	zero,zero,200621e0 <strtol+0x1de>
200621e4:	5049                	c.li	zero,-14
200621e6:	69445f43          	fmadd.s	ft10,fs0,fs4,fa3,unknown
200621ea:	3a72                	c.fldsp	fs4,312(sp)
200621ec:	6425                	c.lui	s0,0x9
200621ee:	202c                	c.fld	fa1,64(s0)
200621f0:	5049                	c.li	zero,-14
200621f2:	58545f43          	fmadd.s	ft10,fs0,ft5,fa1,unknown
200621f6:	445f 5441 2041      	0x20415441445f
200621fc:	7830                	c.flw	fa2,112(s0)
200621fe:	7825                	c.lui	a6,0xfffe9
20062200:	4e20                	c.lw	s0,88(a2)
20062202:	4320746f          	jal	s0,20069634 <__bdram_heap_buffer_start__+0xb34>
20062206:	656c                	c.flw	fa1,76(a0)
20062208:	6e61                	c.lui	t3,0x18
2006220a:	0d21                	c.addi	s10,8
2006220c:	000a                	c.slli	zero,0x2
	...

20062210 <CSWTCH.29>:
20062210:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20062220:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20062230:	0008 0000                                   ....

20062234 <CSWTCH.30>:
20062234:	0000 4102 0000 4102 0000 0000 0000 0000     ...A...A........
	...
20062274:	0080 4102 0080 4102 0000 0000 0000 0000     ...A...A........
	...
200622b4:	0100 4102 0100 4102 682f 6d6f 2f65 7561     ...A...A/home/au
200622c4:	6f74 6a5f 6e65 696b 736e 775f 616c 356e     to_jenkins_wlan5
200622d4:	492f 544f 415f 5354 722f 7065 736f 722f     /IOT_ATS/repos/r
200622e4:	6c65 6165 6573 6d2d 7361 6574 2f72 6f73     elease-master/so
200622f4:	7275 6563 612f 656d 6162 696c 6574 675f     urce/amebalite_g
20062304:	6363 705f 6f72 656a 7463 702f 6f72 656a     cc_project/proje
20062314:	7463 6b5f 3472 762f 6473 2f6b 2e2e 2e2f     ct_kr4/vsdk/../.
20062324:	2f2e 2e2e 632f 6d6f 6f70 656e 746e 732f     ./../component/s
20062334:	636f 612f 656d 6162 696c 6574 662f 6c77     oc/amebalite/fwl
20062344:	6269 722f 6d61 635f 6d6f 6f6d 2f6e 6d61     ib/ram_common/am
20062354:	6265 5f61 7069 5f63 6172 2e6d 0063 0000     eba_ipc_ram.c...
20062364:	5043 2055 6425 7420 6b61 2065 6573 616d     CPU %d take sema
20062374:	2520 2064 6f66 2072 7870 6469 2520 2064      %d for pxid %d 
20062384:	6166 6c69 203a 7063 6975 2064 6425 7020     fail: cpuid %d p
20062394:	6978 2064 6425 6820 7361 7420 6b61 6e65     xid %d has taken
200623a4:	7420 6568 7320 6d65 0a61 0000 5043 2055      the sema...CPU 
200623b4:	6425 6620 6572 2065 6573 616d 2520 2064     %d free sema %d 
200623c4:	6f66 2072 7870 6469 2520 2064 6166 6c69     for pxid %d fail
200623d4:	203a 7063 6975 2064 6425 202c 7870 6469     : cpuid %d, pxid
200623e4:	2520 2064 7369 7420 6b61 6e69 2067 6874      %d is taking th
200623f4:	2065 6573 616d 000a 544f 2050 6572 6461     e sema..OTP read
20062404:	6720 7465 6820 2077 6573 616d 6620 6961      get hw sema fai
20062414:	0a6c 0000 544f 5f50 6552 6461 2038 6146     l...OTP_Read8 Fa
20062424:	6c69 2520 2078 000a                         il %x ..

2006242c <pmap_func>:
2006242c:	0000 0101 0001 0101 0002 0102 0003 0102     ................
2006243c:	0004 0202 0005 0102 0006 0202 0007 0102     ................
2006244c:	0008 0101 0009 0101 000a 0101 000b 0101     ................
2006245c:	000c 0101 000d 0101 000e 0101 000f 0101     ................
2006246c:	0010 0101 0011 0101 0012 0000 0013 0202     ................
2006247c:	0014 0202 0015 0101 0016 0000 0017 0101     ................
2006248c:	0018 0101 0019 0101 001a 0101 001b 0101     ................
2006249c:	001c 0101 001d 0101 001e 0101 001f 0101     ................
200624ac:	0020 0101 0021 0101 0022 0101 0023 0101      ...!..."...#...
200624bc:	0024 0101 0025 0101 0026 0101 0027 0101     $...%...&...'...
200624cc:	0028 0101 0029 0101 002a 0101 002b 0100     (...)...*...+...
200624dc:	002c 0100 002d 0100 002e 0100 002f 0100     ,...-......./...
200624ec:	0030 0100 0031 0100 0032 0100 0033 0100     0...1...2...3...
200624fc:	ffff ffff 766e 6369 695f 7172 2520 756c     ....nvic_irq %lu
2006250c:	4620 6e75 4e20 746f 4120 7373 6769 216e      Fun Not Assign!
2006251c:	2121 2121 000a 0000 4c50 4349 0000 0000     !!!!....PLIC....
2006252c:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
2006253c:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
2006254c:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
2006255c:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
2006256c:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
2006257c:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
2006258c:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
2006259c:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
200625ac:	696c 6574 6d2f 7369 2f63 6d61 6265 5f61     lite/misc/ameba_
200625bc:	6d70 2e75 0063 0000 6e49 6176 696c 2064     pmu.c...Invalid 
200625cc:	7350 6172 436d 6b6c 000a 0000 5350 4152     PsramClk....PSRA
200625dc:	004d 0000 7370 6172 206d 7974 6570 6920     M...psram type i
200625ec:	2073 7830 7825 000a 5353 4549 524e 3009     s 0x%x..SSIENR.0
200625fc:	2578 3d78 7830 7825 000a 0000 5443 4c52     x%x=0x%x....CTRL
2006260c:	3052 3009 2578 3d78 7830 7825 000a 0000     R0.0x%x=0x%x....
2006261c:	4142 4455 0952 7830 7825 303d 2578 0a78     BAUDR.0x%x=0x%x.
2006262c:	0000 0000 4156 494c 5f44 4d43 0944 7830     ....VALID_CMD.0x
2006263c:	7825 303d 2578 0a78 0000 0000 5443 4c52     %x=0x%x.....CTRL
2006264c:	3252 3009 2578 3d78 7830 7825 000a 0000     R2.0x%x=0x%x....
2006265c:	4544 4956 4543 495f 464e 094f 7830 7825     DEVICE_INFO.0x%x
2006266c:	303d 2578 0a78 0000 5054 3052 3009 2578     =0x%x...TPR0.0x%
2006267c:	3d78 7830 7825 000a 4552 4441 465f 5341     x=0x%x..READ_FAS
2006268c:	5f54 4953 474e 454c 3009 2578 3d78 7830     T_SINGLE.0x%x=0x
2006269c:	7825 000a 5257 5449 5f45 4953 4e47 454c     %x..WRITE_SIGNLE
200626ac:	3009 2578 3d78 7830 7825 000a 5541 4f54     .0x%x=0x%x..AUTO
200626bc:	4c5f 4e45 5447 0948 7830 7825 303d 2578     _LENGTH.0x%x=0x%
200626cc:	0a78 0000 5355 5245 4c5f 4e45 5447 0948     x...USER_LENGTH.
200626dc:	7830 7825 303d 2578 0a78 0000 5541 4f54     0x%x=0x%x...AUTO
200626ec:	4c5f 4e45 5447 3248 3009 2578 3d78 7830     _LENGTH2.0x%x=0x
200626fc:	7825 000a 5253 3009 2578 3d78 7830 7825     %x..SR.0x%x=0x%x
2006270c:	000a 0000 5350 4850 5f59 4143 5f4c 4150     ....PSPHY_CAL_PA
2006271c:	0952 7830 7825 303d 2578 0a78 0000 0000     R.0x%x=0x%x.....
2006272c:	5350 4850 5f59 4850 5f59 5443 4c52 3009     PSPHY_PHY_CTRL.0
2006273c:	2578 3d78 7830 7825 000a 0000 5350 4850     x%x=0x%x....PSPH
2006274c:	5f59 4244 5f47 4553 094c 7830 7825 303d     Y_DBG_SEL.0x%x=0
2006275c:	2578 0a78 0000 0000 5249 5b30 5d31 303d     x%x.....IR0[1]=0
2006276c:	2578 0a78 0000 0000 5249 5b30 5d30 303d     x%x.....IR0[0]=0
2006277c:	2578 0a78 0000 0000 5243 5b30 5d31 303d     x%x.....CR0[1]=0
2006278c:	2578 0a78 0000 0000 5243 5b30 5d30 303d     x%x.....CR0[0]=0
2006279c:	2578 0a78 0000 0000 5243 5b31 5d31 303d     x%x.....CR1[1]=0
200627ac:	2578 0a78 0000 0000 5243 5b31 5d30 303d     x%x.....CR1[0]=0
200627bc:	2578 0a78 0000 0000 3d3d 703d 6168 6573     x%x.....===phase
200627cc:	2520 786c 3d20 3d3d 3d3d 000a 6b6f 2520      %lx =====..ok %
200627dc:	786c 2520 786c 2520 786c 2520 786c 2520     lx %lx %lx %lx %
200627ec:	786c 2520 786c 2520 786c 000a 6166 6c69     lx %lx %lx..fail
200627fc:	2520 786c 2520 786c 2520 786c 2520 786c      %lx %lx %lx %lx
2006280c:	2520 786c 2520 786c 2520 786c 000a 0000      %lx %lx %lx....
2006281c:	6143 4e6c 696d 206e 203d 7825 4320 6c61     CalNmin = %x Cal
2006282c:	6d4e 7861 3d20 2520 2078 6957 646e 776f     Nmax = %x Window
2006283c:	6953 657a 3d20 2520 2078 6870 7361 3a65     Size = %x phase:
2006284c:	2520 2078 000a 0000 5441 0000 003d 0000      %x ....AT..=...
2006285c:	002c 0000 7769 7270 7669 0020 0d0a 4d5b     ,...iwpriv ...[M
2006286c:	4d45 205d 6641 6574 2072 6f64 6320 646d     EM] After do cmd
2006287c:	202c 7661 6961 616c 6c62 2065 6568 7061     , available heap
2006288c:	2520 0a64 000d 0000 0a0d 230a 0a0d 0000      %d........#....
2006289c:	0000 0000 4f2b 4154 4c43 4145 3a52 0000     ....+OTACLEAR:..
200628ac:	0a0d 7325 4b4f 0a0d 0000 0000 4f2b 4154     ..%sOK......+OTA
200628bc:	4552 4f43 4556 3a52 0000 0000 0a0d 6120     RECOVER:...... a
200628cc:	6772 6c20 6e65 3d20 2520 2064 0a0d 0000     rg len = %d ....
200628dc:	0a0d 6120 6772 3d20 2520 2073 0a0d 0000     .. arg = %s ....
200628ec:	542b 5345 3a54 0000 522b 5453 003a 0000     +TEST:..+RST:...
200628fc:	6154 6b73 4c20 7369 3a74 0d20 250a 0d73     Task List: ..%s.
2006290c:	000a 0000 6548 7061 7453 7461 3a73 0d20     ....HeapStats: .
2006291c:	000a 0000 6f54 6174 206c 6548 7061 093a     ....Total Heap:.
2006292c:	7525 0a0d 0000 0000 6548 7061 4620 6572     %u......Heap Fre
2006293c:	2065 6f4e 3a77 2509 0d75 000a 6548 7061     e Now:.%u...Heap
2006294c:	5520 6573 2064 6f4e 3a77 2509 0d75 000a      Used Now:.%u...
2006295c:	6548 7061 5520 6573 2064 614d 3a78 2509     Heap Used Max:.%
2006296c:	0d75 000a 532b 4154 4554 003a 4d41 4245     u...+STATE:.AMEB
2006297c:	2d41 5452 534f 5320 4b44 5620 5245 4953     A-RTOS SDK VERSI
2006298c:	4e4f 203a 6425 252e 2e64 6425 000a 0000     ON: %d.%d.%d....
2006299c:	7325 0000 3056 2e32 2e31 0031 3376 352e     %s..V02.1.1.v3.5
200629ac:	0000 0000 3032 3432 302f 2f38 3132 312d     ....2024/08/21-1
200629bc:	3a30 3935 313a 0030 472b 524d 003a 0000     0:59:10.+GMR:...
200629cc:	0a0d 7325 7325 252c 2c73 7325 0a0d 0000     ..%s%s,%s,%s....
200629dc:	4c5b 474f 205d 5245 4f52 2052 7261 3a67     [LOG] ERROR arg:
200629ec:	0d20 000a 0023 0000 4c5b 474f 205d 6e49      ...#...[LOG] In
200629fc:	6176 696c 2064 6567 2074 6170 6172 656d     valid get parame
20062a0c:	6574 7372 0d2e 000a 253c 3e73 6c20 7665     ters....<%s> lev
20062a1c:	6c65 6920 2073 6425 0a0d 0000 4c5b 474f     el is %d....[LOG
20062a2c:	205d 6e49 6176 696c 2064 6573 2074 6170     ] Invalid set pa
20062a3c:	6172 656d 6574 7372 0d2e 000a 4c5b 474f     rameters....[LOG
20062a4c:	205d 6146 6c69 6465 7720 6568 206e 6573     ] Failed when se
20062a5c:	2e74 0a0d 0000 0000 4c5b 474f 205d 6e49     t.......[LOG] In
20062a6c:	6176 696c 2064 6170 6172 656d 6574 2072     valid parameter 
20062a7c:	756e 626d 7265 0d2e 000a 0000 4c5b 474f     number......[LOG
20062a8c:	205d 6e49 6176 696c 2064 6f6d 6564 2520     ] Invalid mode %
20062a9c:	2e64 0a0d 0000 0000 4c5b 474f 205d 6554     d.......[LOG] Te
20062aac:	7473 6120 776c 7961 2073 656c 6576 0d6c     st always level.
20062abc:	000a 0000 4c5b 474f 205d 6554 7473 6520     ....[LOG] Test e
20062acc:	7272 726f 6c20 7665 6c65 0a0d 0000 0000     rror level......
20062adc:	4c5b 474f 205d 6554 7473 7720 7261 696e     [LOG] Test warni
20062aec:	676e 6c20 7665 6c65 0a0d 0000 4c5b 474f     ng level....[LOG
20062afc:	205d 6554 7473 6920 666e 206f 656c 6576     ] Test info leve
20062b0c:	0d6c 000a 4c5b 474f 205d 6554 7473 4c20     l...[LOG] Test L
20062b1c:	474f 495f 4554 534d 0a0d 0000 4c2b 474f     OG_ITEMS....+LOG
20062b2c:	003a 0000 0a0d 7325 5245 4f52 3a52 6425     :.....%sERROR:%d
20062b3c:	0a0d 0000 5441 4c2b 474f 3c3d 6567 5f74     ....AT+LOG=<get_
20062b4c:	6573 3e74 3c2c 6f6d 7564 656c 5b3e 3c2c     set>,<module>[,<
20062b5c:	6f6c 5f67 656c 6576 3e6c 0d5d 000a 0000     log_level>].....
20062b6c:	3c09 6567 5f74 6573 3e74 093a 2d30 6567     .<get_set>:.0-ge
20062b7c:	2c74 3120 732d 7465 202c 2d32 7270 6e69     t, 1-set, 2-prin
20062b8c:	2074 6c61 2c6c 3320 632d 656c 7261 6120     t all, 3-clear a
20062b9c:	6c6c 0a0d 0000 0000 3c09 6f6d 7564 656c     ll.......<module
20062bac:	3a3e 2a09 652d 6361 2068 6f6d 7564 656c     >:.*-each module
20062bbc:	202c 746f 6568 7372 732d 6570 6963 6966     , others-specifi
20062bcc:	2063 6f6d 7564 656c 0a0d 0000 3c09 6f6c     c module.....<lo
20062bdc:	5f67 656c 6576 3e6c 093a 305b 352c 2c5d     g_level>:.[0,5],
20062bec:	6f20 6c6e 2079 7061 6c70 6369 6261 656c      only applicable
20062bfc:	6620 726f 7320 7465 6d20 646f 0d65 000a      for set mode...
20062c0c:	525b 4552 5d47 4520 7272 726f 7020 7261     [RREG] Error par
20062c1c:	6d61 7465 7265 0d73 000a 0000 522b 4552     ameters.....+RRE
20062c2c:	3a47 0000 575b 4552 5d47 4520 7272 726f     G:..[WREG] Error
20062c3c:	7020 7261 6d61 7465 7265 0d73 000a 0000      parameters.....
20062c4c:	572b 4552 3a47 0000 5043 2555 0000 0000     +WREG:..CPU%....
20062c5c:	7270 6f69 0000 0000 7473 7461 0065 0000     prio....state...
20062c6c:	6174 6b73 6e20 6d61 0065 0000 7325 2509     task name...%s.%
20062c7c:	0973 7325 2509 0a73 000d 0000 7325 2509     s.%s.%s.....%s.%
20062c8c:	0963 6425 2509 2e64 7325 0a0d 0000 0000     c.%d.%d.%s......
20062c9c:	7325 6d20 6c61 6f6c 2063 6166 6c69 6465     %s malloc failed
20062cac:	0a0d 0000 745b 706f 555d 6173 6567 203a     ....[top]Usage: 
20062cbc:	6f74 3d70 6f6d 6564 742c 6d69 2c65 6f63     top=mode,time,co
20062ccc:	6e75 0a74 200d 6f6d 6564 203a 2c30 7320     unt.. mode: 0, s
20062cdc:	6174 7472 6320 756f 746e 6320 7570 7520     tart count cpu u
20062cec:	6173 6567 6520 6576 7972 5b20 6974 656d     sage every [time
20062cfc:	205d 6573 6f63 646e 0d2e 200a 6f6d 6564     ] second... mode
20062d0c:	203a 2c31 7320 6f74 2070 6f6d 6564 3020     : 1, stop mode 0
20062d1c:	0d2e 200a 6f6d 6564 203a 3a32 7320 6174     ... mode: 2: sta
20062d2c:	7472 6320 756f 746e 6320 7570 7520 6173     rt count cpu usa
20062d3c:	6567 0d2e 200a 6f6d 6564 203a 3a33 7320     ge... mode: 3: s
20062d4c:	6f74 2070 6f6d 6564 3220 0d2e 200a 6974     top mode 2... ti
20062d5c:	656d 203a 5043 2055 7473 7461 7369 6974     me: CPU statisti
20062d6c:	7363 6920 746e 7265 6176 2e6c 6544 6166     cs interval.Defa
20062d7c:	6c75 2074 2e31 7520 696e 2874 2973 0d20     ult 1. unit(s) .
20062d8c:	200a 6f63 6e75 3a74 4320 5550 7320 6174     . count: CPU sta
20062d9c:	6974 7473 6369 2073 6f63 6e75 2c74 6420     tistics count, d
20062dac:	6665 7561 746c 7520 746e 6c69 7320 6f74     efault until sto
20062dbc:	2070 726f 3120 322c 332c 2e2e 002e 0000     p or 1,2,3......
20062dcc:	654d 6f6d 7972 6620 6961 756c 6572 6620     Memory failure f
20062ddc:	726f 7420 7361 5f6b 7473 7461 7375 0a0d     or task_status..
20062dec:	0000 0000 7563 7272 6e65 2074 6f6d 6564     ....current mode
20062dfc:	203a 6425 7420 6d69 3a65 2520 2864 2973     : %d time: %d(s)
20062e0c:	6320 756f 746e 203a 6425 2820 312d 6620      count: %d (-1 f
20062e1c:	726f 6920 666e 6e69 7469 2965 0a0d 0000     or infinite)....
20062e2c:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062e3c:	432b 5550 4f4c 4441 003a 0000 5441 432b     +CPULOAD:...AT+C
20062e4c:	5550 4f4c 4441 3c3d 6f6d 6564 5b3e 3c2c     PULOAD=<mode>[,<
20062e5c:	6974 656d 695f 746e 6176 3e6c 3c2c 6f63     time_intval>,<co
20062e6c:	6e75 6574 3e72 0d5d 000a 0000 3c09 6f6d     unter>]......<mo
20062e7c:	6564 3a3e 5b09 2c30 5d32 0a0d 0000 0000     de>:.[0,2]......
20062e8c:	3c09 6974 656d 695f 746e 6176 3e6c 093a     .<time_intval>:.
20062e9c:	6e49 7320 6365 0a0d 0000 0000 3c09 6f63     In sec.......<co
20062eac:	6e75 6574 3e72 3009 6d20 6165 736e 6920     unter>.0 means i
20062ebc:	666e 6e69 7469 0a0d 0000 0000 5441 7325     nfinit......AT%s
20062ecc:	0a0d 0000 6f43 6d6d 6e6f 4120 2054 6f43     ....Common AT Co
20062edc:	6d6d 6e61 3a64 0a0d 0000 0000 4c2b 5349     mmand:......+LIS
20062eec:	3a54 0000 4f2b 4154 4c43 4145 0052 0000     T:..+OTACLEAR...
20062efc:	4f2b 4154 4552 4f43 4556 0052 432b 5550     +OTARECOVER.+CPU
20062f0c:	4f4c 4441 0000 0000 542b 5345 0054 0000     LOAD....+TEST...
20062f1c:	4c2b 5349 0054 0000 522b 5453 0000 0000     +LIST...+RST....
20062f2c:	532b 4154 4554 0000 472b 524d 0000 0000     +STATE..+GMR....
20062f3c:	4c2b 474f 0000 0000 522b 4552 0047 0000     +LOG....+RREG...
20062f4c:	572b 4552 0047 0000                         +WREG...

20062f54 <__FUNCTION__.0>:
20062f54:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062f64:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20062f74:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20062f84:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20062f94:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20062fa4:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20062fb4:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20062fc4:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20062fd4:	6f70 656e 746e 6f2f 2f73 7266 6565 7472     ponent/os/freert
20062fe4:	736f 662f 6572 7265 6f74 5f73 6568 7061     os/freertos_heap
20062ff4:	5f35 6f63 666e 6769 632e 0000 255b 5d73     5_config.c..[%s]
20063004:	6c20 7665 6c65 3d20 2520 0a64 0000 0000      level = %d.....
20063014:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20063024:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20063034:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20063044:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20063054:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20063064:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20063074:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20063084:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
20063094:	696c 6574 732f 6c77 6269 6c2f 676f 632e     lite/swlib/log.c
200630a4:	0000 0000 002a 0000 6143 6863 2065 7261     ....*...Cache ar
200630b4:	6172 2079 7369 6620 6c75 2c6c 6120 646e     ray is full, and
200630c4:	7220 7065 616c 6563 6f20 646c 6520 746e      replace old ent
200630d4:	7972 000a 255b 3830 5d78 0020 0a0d 255b     ry..[%08x] ...[%
200630e4:	3830 5d78 0020 0000 3025 7838 0020 0000     08x] ...%08x ...
200630f4:	3025 7832 0020 0000 255b 2d73 6325 205d     %02x ...[%s-%c] 
20063104:	0000 0000 5749 4744 7220 6665 6572 6873     ....IWDG refresh
20063114:	6f20 216e 000a 0000 414d 4e49 0000 0000      on!....MAIN....
20063124:	4457 5f47 6954 656d 0072 0000 5749 4744     WDG_Timer...IWDG
20063134:	7220 6665 6572 6873 6520 7272 726f 000a      refresh error..
20063144:	524b 2034 534f 5320 4154 5452 0a20 0000     KR4 OS START ...
20063154:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
20063164:	202d 4554 5453 4320 4d4f 414d 444e 4d20     - TEST COMMAND M
20063174:	444f 2045 4548 504c 2520 2d64 2d2d 2d2d     ODE HELP %d-----
20063184:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 0a2d 0000     -------------...
20063194:	7325 000a 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     %s..------------
200631a4:	2d2d 2d2d 202d 4554 5453 4320 4d4f 414d     ----- TEST COMMA
200631b4:	444e 4d20 444f 2045 4e45 2044 2520 2d78     ND MODE END  %x-
200631c4:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
200631d4:	0a2d 0000 6552 6f62 746f 6e69 2067 2e2e     -...Rebooting ..
200631e4:	0a2e 000d 6175 7472 7562 6e72 0000 0000     ....uartburn....
200631f4:	255b 3830 5d78 2520 3830 2078 000a 0000     [%08x] %08x ....
20063204:	7257 6e6f 2067 7261 7567 656d 746e 6e20     Wrong argument n
20063214:	6d75 6562 2172 0a0d 0000 0000 5352 5049     umber!......RSIP
20063224:	6520 616e 6c62 6465 202c 6143 276e 2074      enabled, Can't 
20063234:	6552 6461 4620 616c 6873 0a2e 0000 0000     Read Flash......
20063244:	7257 6e6f 2067 6d63 2164 0a0d 0000 0000     Wrong cmd!......
20063254:	003f 0000 4809 4c45 2050 4028 293f 3a20     ?....HELP (@?) :
20063264:	0a20 0909 5020 6972 746e 6820 6c65 2070      ... Print help 
20063274:	656d 7373 6761 000a 5744 0000 4009 5744     messag..DW...@DW
20063284:	3c20 6441 7264 7365 2c73 4c20 6e65 7467      <Address, Lengt
20063294:	3e68 090a 2009 7544 706d 6d20 6d65 726f     h>... Dump memor
200632a4:	2079 6f77 6472 6f20 2072 6552 6461 4820     y word or Read H
200632b4:	2077 6f77 6472 7220 6765 7369 6574 3b72     w word register;
200632c4:	0a20 0909 4420 2057 613c 6464 3e72 0920      ... DW <addr> .
200632d4:	2009 7544 706d 6f20 6c6e 2079 6e6f 2065     . Dump only one 
200632e4:	6f77 6472 6120 2074 6874 2065 6173 656d     word at the same
200632f4:	7420 6d69 2c65 7520 696e 3a74 3420 7942      time, unit: 4By
20063304:	6574 0a73 0909 4420 2057 613c 6464 3e72     tes... DW <addr>
20063314:	3c20 656c 3e6e 0920 4420 6d75 2070 6874      <len> . Dump th
20063324:	2065 7073 6365 6669 6569 2064 656c 676e     e specified leng
20063334:	6874 6f20 2066 6874 2065 6f77 6472 202c     th of the word, 
20063344:	6e75 7469 203a 4234 7479 7365 090a 2009     unit: 4Bytes... 
20063354:	5744 3c20 6461 7264 203e 6c3c 6e65 203e     DW <addr> <len> 
20063364:	2062 4420 6d75 2070 6874 2065 7073 6365     b  Dump the spec
20063374:	6669 6569 2064 656c 676e 6874 6f20 2066     ified length of 
20063384:	6874 2065 7962 6574 202c 6e75 7469 203a     the byte, unit: 
20063394:	4231 7479 0a65 0000 5745 0000 4009 5745     1Byte...EW...@EW
200633a4:	3c20 6441 7264 7365 2c73 4820 7865 0a3e      <Address, Hex>.
200633b4:	0909 5720 6972 6574 6d20 6d65 726f 2079     .. Write memory 
200633c4:	7764 726f 2064 726f 5720 6972 6574 4820     dword or Write H
200633d4:	2077 7764 726f 2064 6572 6967 7473 7265     w dword register
200633e4:	0a20 0909 4320 6e61 7720 6972 6574 6f20      ... Can write o
200633f4:	6c6e 2079 6e6f 2065 7764 726f 2064 7461     nly one dword at
20063404:	7420 6568 7320 6d61 2065 6974 656d 0a20      the same time .
20063414:	0909 4520 3a78 4520 2057 6441 7264 7365     .. Ex: EW Addres
20063424:	2073 6156 756c 2065 000a 0000 4552 4f42     s Value ....REBO
20063434:	544f 0000 4009 4552 4f42 544f 0a20 0909     OT...@REBOOT ...
20063444:	7220 6265 6f6f 2074 090a 2009 6572 6f62      reboot ... rebo
20063454:	746f 7520 7261 6274 7275 206e 000a 0000     ot uartburn ....
20063464:	255b 5d73 2520 2073 3c3c 203c 6854 2065     [%s] %s <<< The 
20063474:	756d 6574 2078 6168 2073 6f6e 2074 6562     mutex has not be
20063484:	6e65 7220 6c65 6165 6573 2c64 6220 7475     en released, but
20063494:	7420 6568 6d20 7475 7865 6820 7361 6220      the mutex has b
200634a4:	6565 206e 6564 656c 6574 2e64 3e20 3e3e     een deleted. >>>
200634b4:	000a 0000                                   ....

200634b8 <__FUNCTION__.0>:
200634b8:	7472 736f 6d5f 7475 7865 645f 6c65 7465     rtos_mutex_delet
200634c8:	0065 0000 6f4e 7354 006b 0000 524b 0034     e...NoTsk...KR4.
200634d8:	614d 6c6c 636f 6620 6961 656c 2e64 4320     Malloc failed. C
200634e8:	726f 3a65 255b 5d73 202c 6154 6b73 5b3a     ore:[%s], Task:[
200634f8:	7325 2c5d 5b20 7266 6565 6820 6165 2070     %s], [free heap 
20063508:	6973 657a 203a 6425 0d5d 000a 0d0a 255b     size: %d].....[%
20063518:	5d73 5320 4154 4b43 4f20 4556 4652 4f4c     s] STACK OVERFLO
20063528:	2057 202d 6154 6b73 614e 656d 2528 2973     W - TaskName(%s)
20063538:	0d0a 0000                                   ....

2006353c <__FUNCTION__.5>:
2006353c:	4176 7070 696c 6163 6974 6e6f 7453 6361     vApplicationStac
2006354c:	4f6b 6576 6672 6f6c 4877 6f6f 006b 0000     kOverflowHook...
2006355c:	5441 3f53 0000 0000 0a0d 6e75 6e6b 776f     ATS?......unknow
2006356c:	206e 6f63 6d6d 6e61 2064 2527 2773 0000     n command '%s'..
2006357c:	4f4c 5547 5241 5f54 4154 4b53 0000 0000     LOGUART_TASK....
2006358c:	7243 6165 6574 4c20 676f 5520 5241 2054     Create Log UART 
2006359c:	6154 6b73 4520 7272 2121 000a 4853 4c45     Task Err!!..SHEL
200635ac:	004c 0000 255b 5d73 4920 474d 4449 203a     L...[%s] IMGID: 
200635bc:	6425 202c 7563 7272 6e65 2074 544f 2541     %d, current OTA%
200635cc:	2064 6441 7264 7365 3a73 3020 2578 3830     d Address: 0x%08
200635dc:	2c78 7420 7261 6567 2074 544f 2541 2064     x, target OTA%d 
200635ec:	6441 7264 7365 3a73 3020 2578 3830 0a78     Address: 0x%08x.
200635fc:	0000 0000 5953 0053 255b 5d73 4920 474d     ....SYS.[%s] IMG
2006360c:	4449 203a 6425 202c 7563 7272 6e65 2074     ID: %d, current 
2006361c:	6966 6d72 6177 6572 6920 2073 544f 2541     firmware is OTA%
2006362c:	2c64 7420 7261 6567 2074 6966 6d72 6177     d, target firmwa
2006363c:	6572 4f20 4154 6425 6920 2073 6e69 6176     re OTA%d is inva
2006364c:	696c 0a64 0000 0000 255b 5d73 6220 6361     lid.....[%s] bac
2006365c:	756b 2070 616d 6c6c 636f 6620 6961 646c     kup malloc faild
2006366c:	6465 000a                                   ed..

20063670 <__func__.0>:
20063670:	7973 5f73 6572 6f63 6576 5f72 746f 5f61     sys_recover_ota_
20063680:	6973 6e67 7461 7275 0065 0000               signature...

2006368c <__func__.1>:
2006368c:	7973 5f73 6c63 6165 5f72 746f 5f61 6973     sys_clear_ota_si
2006369c:	6e67 7461 7275 0065 4449 454c 0000 0000     gnature.IDLE....
200636ac:	2509 0963 6425 2509 0964 6425 0a0d 0000     .%c.%d.%d.%d....
200636bc:	6d54 2072 7653 0063 e8ae 2005 e8ae 2005     Tmr Svc.... ... 
200636cc:	e8ae 2005 e8ea 2005 e8f6 2005 e912 2005     ... ... ... ... 
200636dc:	e8ae 2005 e8ae 2005 e8ea 2005 e8f6 2005     ... ... ... ... 
200636ec:	7325 2528 2964 202c 7641 6961 616c 6c62     %s(%d), Availabl
200636fc:	2065 6568 7061 2520 0a64 0000 4c57 4e41     e heap %d...WLAN
2006370c:	0000 0000 6e69 7469 0000 0000 6c77 6e61     ....init....wlan
2006371c:	695f 696e 6974 6c61 7a69 2065 6166 6c69     _initialize fail
2006372c:	6465 000a                                   ed..

20063730 <__FUNCTION__.0>:
20063730:	695f 696e 5f74 6874 6572 6461 0000 0000     _init_thread....
20063740:	6f46 2072 6564 7562 2067 6170 6b63 6761     For debug packag
20063750:	2e65 5020 656c 7361 2065 6573 656c 7463     e. Please select
20063760:	7020 7273 6d61 7420 7079 2065 6361 6f63      psram type acco
20063770:	6472 6e69 2067 6f74 7920 756f 2072 6863     rding to your ch
20063780:	7069 000a 4843 5049 4e49 4f46 0000 0000     ip..CHIPINFO....
20063790:	3230 6620 726f 5720 2042 3233 624d 300a     02 for WB 32Mb.0
200637a0:	2034 6f66 2072 4257 3120 3832 624d 300a     4 for WB 128Mb.0
200637b0:	2033 6f66 2072 4257 3220 3635 624d 0a0a     3 for WB 256Mb..
200637c0:	0000 0000 682f 6d6f 2f65 7561 6f74 6a5f     ..../home/auto_j
200637d0:	6e65 696b 736e 775f 616c 356e 492f 544f     enkins_wlan5/IOT
200637e0:	415f 5354 722f 7065 736f 722f 6c65 6165     _ATS/repos/relea
200637f0:	6573 6d2d 7361 6574 2f72 6f73 7275 6563     se-master/source
20063800:	612f 656d 6162 696c 6574 675f 6363 705f     /amebalite_gcc_p
20063810:	6f72 656a 7463 702f 6f72 656a 7463 6b5f     roject/project_k
20063820:	3472 762f 6473 2f6b 2e2e 2e2f 2f2e 2e2e     r4/vsdk/../../..
20063830:	632f 6d6f 6f70 656e 746e 732f 636f 612f     /component/soc/a
20063840:	656d 6162 696c 6574 6c2f 6269 722f 6d61     mebalite/lib/ram
20063850:	635f 6d6f 6f6d 2f6e 6d61 6265 5f61 6863     _common/ameba_ch
20063860:	7069 6e69 6f66 6c5f 6269 632e 0000 0000     ipinfo_lib.c....
20063870:	7325 203a 0000 0000 5245 3a52 4720 7465     %s: ....ERR: Get
20063880:	5320 636f 4e20 6d61 2065 6146 6c69 000a      Soc Name Fail..
20063890:	3025 7832 0000 0000 4d4b 0034 7325 4c20     %02x....KM4.%s L
200638a0:	6269 5620 7265 6973 6e6f 0a3a 0000 0000     ib Version:.....
200638b0:	3032 3432 302f 2f38 3132 312d 3a30 3835     2024/08/21-10:58
200638c0:	353a 0035 4f43 504d 4c49 2045 4954 454d     :55.COMPILE TIME
200638d0:	203a 7325 000a 0000 6367 2063 6576 7372     : %s....gcc vers
200638e0:	6f69 206e 3031 332e 312e 3220 3230 3032     ion 10.3.1 20220
200638f0:	3335 2030 5228 6165 746c 6b65 5620 4453     530 (Realtek VSD
20063900:	2d4b 3031 332e 312e 4220 6975 646c 3420     K-10.3.1 Build 4
20063910:	3730 2b33 2029 0000 4f43 504d 4c49 5245     073+) ..COMPILER
20063920:	203a 7325 000a 0000 6325 0000 2d2d 2d2d     : %s....%c..----
20063930:	2d2d 2d2d 2d2d 2d2d 6154 6b73 4920 666e     --------Task Inf
20063940:	2d6f 2d2d 2d2d 2d2d 2d2d 2d2d 0d2d 000a     o------------...
20063950:	6146 6c75 2074 6e6f 7420 7361 206b 253c     Fault on task <%
20063960:	3e73 000a 6154 6b73 4920 3a44 2520 0d64     s>..Task ID: %d.
20063970:	000a 0000 6154 6b73 5420 4243 303a 2578     ....Task TCB:0x%
20063980:	3830 786c 0a0d 0000 7543 7272 6e65 2074     08lx....Current 
20063990:	7453 7461 3a65 2520 2064 2528 2973 0a0d     State: %d (%s)..
200639a0:	0000 0000 6142 6573 5020 6972 726f 7469     ....Base Priorit
200639b0:	3a79 2520 0d64 000a 7543 7272 6e65 2074     y: %d...Current 
200639c0:	7250 6f69 6972 7974 203a 6425 0a0d 0000     Priority: %d....
200639d0:	7552 206e 6954 656d 4320 756f 746e 7265     Run Time Counter
200639e0:	203a 6c25 0d75 000a 7453 6361 546b 706f     : %lu...StackTop
200639f0:	203a 7830 3025 6c38 2c78 5320 6174 6b63     : 0x%08lx, Stack
20063a00:	6142 6573 203a 7830 3025 6c38 2c78 5320     Base: 0x%08lx, S
20063a10:	6174 6b63 6e45 3a64 3020 2578 3830 786c     tackEnd: 0x%08lx
20063a20:	202c 7453 6361 536b 7a69 3d65 6c25 2875     , StackSize=%lu(
20063a30:	6f77 6472 0d29 000a 7453 6361 206b 6948     word)...Stack Hi
20063a40:	6867 5720 7461 7265 614d 6b72 203a 6425     gh WaterMark: %d
20063a50:	7728 726f 2964 0a0d 0000 0000 7552 6e6e     (word)......Runn
20063a60:	6e69 0067 6552 6461 2079 202f 6550 646e     ing.Ready / Pend
20063a70:	6e69 0067 6c42 636f 656b 0064 7553 7073     ing.Blocked.Susp
20063a80:	6e65 6564 0064 0000 6544 656c 6574 0064     ended...Deleted.
20063a90:	6e49 6176 696c 0064                         Invalid.

20063a98 <TaskStateString.0>:
20063a98:	3a5c 2006 3a64 2006 3a74 2006 3a7c 2006     \:. d:. t:. |:. 
20063aa8:	3a88 2006 3a90 2006 f220 2005 f134 2005     .:. .:.  .. 4.. 
20063ab8:	f134 2005 f134 2005 f134 2005 f134 2005     4.. 4.. 4.. 4.. 
20063ac8:	f134 2005 f134 2005 f134 2005 f134 2005     4.. 4.. 4.. 4.. 
20063ad8:	f16a 2005 f2dc 2005 f1aa 2005 f134 2005     j.. ... ... 4.. 
20063ae8:	f134 2005 f134 2005 f134 2005 f1aa 2005     4.. 4.. 4.. ... 
20063af8:	f134 2005 f134 2005 f330 2005 f134 2005     4.. 4.. 0.. 4.. 
20063b08:	f134 2005 f190 2005 f246 2005 f134 2005     4.. ... F.. 4.. 
20063b18:	f134 2005 f264 2005 f134 2005 f1ec 2005     4.. d.. 4.. ... 
20063b28:	f134 2005 f134 2005 f222 2005 f134 2005     4.. 4.. ".. 4.. 
20063b38:	f332 2005 3130 3332 3534 3736 3938 4241     2.. 0123456789AB
20063b48:	4443 4645 0000 0000 6f69 6420 6972 6576     CDEF....io drive
20063b58:	2072 6170 6172 656d 6574 7372 6520 7272     r parameters err
20063b68:	726f 2021 6966 656c 6e5f 6d61 3a65 2520     or! file_name: %
20063b78:	2c73 6c20 6e69 3a65 2520 0064 682f 6d6f     s, line: %d./hom
20063b88:	2f65 7561 6f74 6a5f 6e65 696b 736e 775f     e/auto_jenkins_w
20063b98:	616c 356e 492f 544f 415f 5354 722f 7065     lan5/IOT_ATS/rep
20063ba8:	736f 722f 6c65 6165 6573 6d2d 7361 6574     os/release-maste
20063bb8:	2f72 6f73 7275 6563 612f 656d 6162 696c     r/source/amebali
20063bc8:	6574 675f 6363 705f 6f72 656a 7463 702f     te_gcc_project/p
20063bd8:	6f72 656a 7463 6b5f 3472 762f 6473 2f6b     roject_kr4/vsdk/
20063be8:	2e2e 2e2f 2f2e 2e2e 632f 6d6f 6f70 656e     ../../../compone
20063bf8:	746e 732f 636f 612f 656d 6162 696c 6574     nt/soc/amebalite
20063c08:	6c2f 6269 722f 6d61 635f 6d6f 6f6d 2f6e     /lib/ram_common/
20063c18:	6d61 6265 5f61 6f72 5f6d 6170 6374 2e68     ameba_rom_patch.
20063c28:	0063 0000 7269 5b71 6425 205d 7270 6f69     c...irq[%d] prio
20063c38:	6972 7974 2520 2078 6873 6c61 206c 3d3c     rity %x shall <=
20063c48:	2520 0a64 0000 0000 5b1b 3b31 3333 5b6d      %d......[1;33m[
20063c58:	5245 4f52 5d52 5b20 7325 3a3a 6425 205d     ERROR] [%s::%d] 
20063c68:	7473 6361 5f6b 6973 657a 2528 2964 6920     stack_size(%d) i
20063c78:	2073 6f70 6973 6974 6576 3f20 0a3f 5b1b     s positive ??..[
20063c88:	6d30 0000 5b1b 3b31 3333 5b6d 5245 4f52     0m...[1;33m[ERRO
20063c98:	5d52 5b20 7325 3a3a 6425 205d 6172 6f5f     R] [%s::%d] ra_o
20063ca8:	6666 6573 2874 6425 2029 7369 6e20 6765     ffset(%d) is neg
20063cb8:	7461 7669 3f65 1b3f 305b 006d 5b1b 3b31     ative??.[0m..[1;
20063cc8:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063cd8:	6425 205d 7453 7261 2074 7473 6361 206b     %d] Start stack 
20063ce8:	6162 6b63 7274 6361 6e69 2067 6f66 2072     backtracing for 
20063cf8:	7073 2520 2c70 7020 2063 7025 1b0a 305b     sp %p, pc %p..[0
20063d08:	006d 0000 5b1b 3b31 3333 5b6d 5254 4341     m....[1;33m[TRAC
20063d18:	5d45 5b20 7325 3a3a 6425 205d 665b 6172     E] [%s::%d] [fra
20063d28:	656d 2320 6425 205d 7073 3e2d 2520 2c70     me #%d] sp-> %p,
20063d38:	7020 2d63 203e 7025 202c 7473 6361 5f6b      pc-> %p, stack_
20063d48:	6973 657a 3e2d 2520 2c64 7220 2d61 203e     size-> %d, ra-> 
20063d58:	7025 1b0a 305b 006d 5b1b 3b31 3333 5b6d     %p..[0m..[1;33m[
20063d68:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063d78:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20063d88:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20063d98:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20063da8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063db8:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20063dc8:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20063dd8:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20063de8:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20063df8:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20063e08:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063e18:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
20063e28:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
20063e38:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
20063e48:	1b0a 305b 006d 0000                         ..[0m...

20063e50 <__FUNCTION__.3>:
20063e50:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063e60:	6932 0000                                   2i..

20063e64 <__FUNCTION__.2>:
20063e64:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063e74:	6332 0000                                   2c..

20063e78 <__FUNCTION__.1>:
20063e78:	6f64 735f 6174 6b63 625f 6361 746b 6172     do_stack_backtra
20063e88:	6563 0000 6e55 6e6b 776f 206e 7865 6563     ce..Unknown exce
20063e98:	7470 6f69 006e 0000 5b1b 3b31 3333 5b6d     ption....[1;33m[
20063ea8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063eb8:	3d3d 3d3d 3d3d 3d3d 3d3d 4320 6172 6873     ========== Crash
20063ec8:	4420 6d75 2070 3d3d 3d3d 3d3d 3d3d 3d3d      Dump ==========
20063ed8:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063ee8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063ef8:	7845 6563 7470 6f69 206e 6163 6775 7468     Exception caught
20063f08:	6f20 206e 7025 7720 7469 2068 6572 7361      on %p with reas
20063f18:	6e6f 5b20 7830 7825 205d 3e2d 5b20 7325     on [0x%x] -> [%s
20063f28:	0a5d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     ]..[0m...[1;33m[
20063f38:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063f48:	3d3d 3d3d 3d3d 3d3d 3d3d 5220 6765 7369     ========== Regis
20063f58:	6574 2072 7544 706d 3d20 3d3d 3d3d 3d3d     ter Dump =======
20063f68:	3d3d 0a3d 5b1b 6d30 0000 0000 5b1b 3b31     ===..[0m.....[1;
20063f78:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063f88:	6425 205d 6d5b 6373 6172 6374 5d68 3020     %d] [mscratch] 0
20063f98:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063fa8:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063fb8:	6425 205d 6d5b 7065 5d63 2020 2020 3020     %d] [mepc]     0
20063fc8:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063fd8:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063fe8:	6425 205d 6d5b 6163 7375 5d65 2020 3020     %d] [mcause]   0
20063ff8:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20064008:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20064018:	6425 205d 6d5b 7674 6c61 205d 2020 3020     %d] [mtval]    0
20064028:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20064038:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20064048:	6425 205d 7845 6563 7470 6f69 206e 6163     %d] Exception ca
20064058:	7375 2065 6f63 6c75 2064 6562 0d3a 1b0a     use could be:...
20064068:	305b 006d 5b1b 3b31 3333 5b6d 5254 4341     [0m..[1;33m[TRAC
20064078:	5d45 5b20 7325 3a3a 6425 205d 2509 2c73     E] [%s::%d] .%s,
20064088:	2520 2073 203d 7830 7825 0a0d 5b1b 6d30      %s = 0x%x...[0m
20064098:	0000 0000 5b1b 3b31 3333 5b6d 5254 4341     .....[1;33m[TRAC
200640a8:	5d45 5b20 7325 3a3a 6425 205d 2509 0d73     E] [%s::%d] .%s.
200640b8:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
200640c8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200640d8:	785b 6425 2d20 203e 7325 205d 7830 3025     [x%d -> %s] 0x%0
200640e8:	7838 1b0a 305b 006d 5b1b 3b31 3333 5b6d     8x..[0m..[1;33m[
200640f8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20064108:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20064118:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20064128:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20064138:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20064148:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20064158:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20064168:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20064178:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20064188:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20064198:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
200641a8:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
200641b8:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
200641c8:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
200641d8:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
200641e8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200641f8:	3d3d 3d3d 3d3d 3d3d 3d3d 4520 646e 6f20     ========== End o
20064208:	2066 7243 7361 2068 7544 706d 3d20 3d3d     f Crash Dump ===
20064218:	3d3d 3d3d 3d3d 0a3d 5b1b 6d30 0000 0000     =======..[0m....
20064228:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20064238:	7325 3a3a 6425 205d 0a0d 0a0d 5b1b 6d30     %s::%d] .....[0m
20064248:	0000 0000 6f53 6572 412f 4f4d 7020 6761     ....Sore/AMO pag
20064258:	2065 6177 6b6c 6320 6568 6b63 6620 6961     e walk check fai
20064268:	006c 0000 7453 726f 2065 6461 7264 7365     l...Store addres
20064278:	0073 0000 6f4c 6461 7020 6761 2065 6177     s...Load page wa
20064288:	6b6c 6320 6568 6b63 6620 6961 006c 0000     lk check fail...
20064298:	6f4c 6461 6120 6464 6572 7373 0000 0000     Load address....
200642a8:	6e49 7473 7572 7463 6f69 206e 6170 6567     Instruction page
200642b8:	7720 6c61 206b 6863 6365 206b 6166 6c69      walk check fail
200642c8:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
200642d8:	6461 7264 7365 0073 7253 726f 2f65 4d41     address.Srore/AM
200642e8:	204f 4d50 2050 6863 6365 206b 6166 6c69     O PMP check fail
200642f8:	0000 0000 7453 726f 2f65 4d41 204f 6170     ....Store/AMO pa
20064308:	6567 7720 6c61 206b 7562 2073 7265 6f72     ge walk bus erro
20064318:	0072 0000 7453 726f 2065 7562 2073 7265     r...Store bus er
20064328:	6f72 0072 7453 726f 2f65 4d41 204f 6461     ror.Store/AMO ad
20064338:	7264 7365 2073 696d 6173 696c 6e67 6465     dress misaligned
20064348:	0000 0000 6f4c 6461 5020 504d 6320 6568     ....Load PMP che
20064358:	6b63 6620 6961 006c 6f4c 6461 5020 6761     ck fail.Load Pag
20064368:	2065 6177 6b6c 6220 7375 6520 7272 726f     e walk bus error
20064378:	0000 0000 6f4c 6461 6220 7375 6520 7272     ....Load bus err
20064388:	726f 0000 6f4c 6461 6120 6464 6572 7373     or..Load address
20064398:	6d20 7369 6c61 6769 656e 0064 6c49 656c      misaligned.Ille
200643a8:	6167 206c 6e69 7473 7572 7463 6f69 006e     gal instruction.
200643b8:	6e49 7473 7572 7463 6f69 206e 706f 6f63     Instruction opco
200643c8:	6564 0000 6e49 7473 7572 7463 6f69 206e     de..Instruction 
200643d8:	4d50 2050 6863 6365 206b 6166 6c69 0000     PMP check fail..
200643e8:	6e49 7473 7572 7463 6f69 206e 6150 6567     Instruction Page
200643f8:	7720 6c61 206b 7562 2073 7265 6f72 0072      walk bus error.
20064408:	6e49 7473 7572 7463 6f69 206e 7562 2073     Instruction bus 
20064418:	7265 6f72 0072 0000 7242 6e61 6863 7420     error...Branch t
20064428:	7261 6567 2074 696d 6173 696c 6e67 6465     arget misaligned
20064438:	0000 0000 6154 6772 7465 6120 6464 6572     ....Target addre
20064448:	7373 0000 6e49 7473 7572 7463 6f69 206e     ss..Instruction 
20064458:	6461 7264 7365 2073 696d 6173 696c 6e67     address misalign
20064468:	6465 0000 657a 6f72 0000 0000 6172 0000     ed..zero....ra..
20064478:	7073 0000 7067 0000 7074 0000 3074 0000     sp..gp..tp..t0..
20064488:	3174 0000 3274 0000 3073 662f 0070 0000     t1..t2..s0/fp...
20064498:	3173 0000 3061 0000 3161 0000 3261 0000     s1..a0..a1..a2..
200644a8:	3361 0000 3461 0000 3561 0000 3661 0000     a3..a4..a5..a6..
200644b8:	3761 0000 3273 0000 3373 0000 3473 0000     a7..s2..s3..s4..
200644c8:	3573 0000 3673 0000 3773 0000 3873 0000     s5..s6..s7..s8..
200644d8:	3973 0000 3173 0030 3173 0031 3374 0000     s9..s10.s11.t3..
200644e8:	3474 0000 3574 0000 3674 0000 6e49 7473     t4..t5..t6..Inst
200644f8:	7572 7463 6f69 206e 6361 6563 7373 6620     ruction access f
20064508:	7561 746c 0000 0000 7242 6165 706b 696f     ault....Breakpoi
20064518:	746e 0000 6f4c 6461 6120 6464 7365 2073     nt..Load addess 
20064528:	696d 6173 696c 6e67 6465 0000 6f4c 6461     misaligned..Load
20064538:	6120 6363 7365 2073 6166 6c75 0074 0000      access fault...
20064548:	7453 726f 2065 6461 7264 7365 2073 696d     Store address mi
20064558:	6173 696c 6e67 6465 0000 0000 7453 726f     saligned....Stor
20064568:	2065 6461 7264 7365 2073 6166 6c75 0074     e address fault.
20064578:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
20064588:	6620 6f72 206d 2055 6f6d 6564 0000 0000      from U mode....
20064598:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
200645a8:	6620 6f72 206d 2053 6f6d 6564 0000 0000      from S mode....
200645b8:	6552 6573 7672 6465 6520 6378 7065 6974     Reserved excepti
200645c8:	6e6f 0000 6e45 6976 6f72 6d6e 6e65 2074     on..Environment 
200645d8:	6163 6c6c 6620 6f72 206d 204d 6f6d 6564     call from M mode
200645e8:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
200645f8:	6170 6567 6620 7561 746c 0000 6f4c 6461     page fault..Load
20064608:	7020 6761 2065 6166 6c75 0074 7453 726f      page fault.Stor
20064618:	2065 6170 6567 6620 7561 746c 0000 0000     e page fault....

20064628 <__FUNCTION__.0>:
20064628:	7263 7361 5f68 7564 706d 0000               crash_dump..

20064634 <exception_cause_0_string>:
20064634:	4420 2006 443c 2006 444c 2006 42cc 2006      D. <D. LD. .B. 
	...

2006464c <exception_cause_12_string>:
2006464c:	42a8 2006 42cc 2006 0000 0000 0000 0000     .B. .B. ........

2006465c <exception_cause_13_string>:
2006465c:	427c 2006 4298 2006 0000 0000 0000 0000     |B. .B. ........

2006466c <exception_cause_15_string>:
2006466c:	424c 2006 426c 2006 0000 0000 0000 0000     LB. lB. ........

2006467c <exception_cause_1_string>:
2006467c:	43cc 2006 42cc 2006 43e8 2006 42cc 2006     .C. .B. .C. .B. 
2006468c:	4408 2006 42cc 2006 0000 0000 0000 0000     .D. .B. ........

2006469c <exception_cause_2_string>:
2006469c:	43a4 2006 43b8 2006 0000 0000 0000 0000     .C. .C. ........

200646ac <exception_cause_4_string>:
200646ac:	438c 2006 4298 2006 0000 0000 0000 0000     .C. .B. ........

200646bc <exception_cause_5_string>:
200646bc:	434c 2006 4298 2006 4360 2006 4298 2006     LC. .B. `C. .B. 
200646cc:	437c 2006 0000 0000 0000 0000 0000 0000     |C. ............

200646dc <exception_cause_6_string>:
200646dc:	432c 2006 426c 2006 0000 0000 0000 0000     ,C. lB. ........

200646ec <exception_cause_7_string>:
200646ec:	42e0 2006 426c 2006 42fc 2006 426c 2006     .B. lB. .B. lB. 
200646fc:	431c 2006 0000 0000 0000 0000 0000 0000     .C. ............

2006470c <exception_cause_string_tbl>:
2006470c:	4634 2006 467c 2006 469c 2006 0000 0000     4F. |F. .F. ....
2006471c:	46ac 2006 46bc 2006 46dc 2006 46ec 2006     .F. .F. .F. .F. 
	...
2006473c:	464c 2006 465c 2006 0000 0000 466c 2006     LF. \F. ....lF. 

2006474c <exception_code_string>:
2006474c:	444c 2006 44f4 2006 43a4 2006 4510 2006     LD. .D. .C. .E. 
2006475c:	451c 2006 4534 2006 4548 2006 4564 2006     .E. 4E. HE. dE. 
2006476c:	4578 2006 4598 2006 45b8 2006 45cc 2006     xE. .E. .E. .E. 
2006477c:	45ec 2006 4604 2006 0000 0000 4614 2006     .E. .F. .....F. 

2006478c <register_string>:
2006478c:	446c 2006 4474 2006 4478 2006 447c 2006     lD. tD. xD. |D. 
2006479c:	4480 2006 4484 2006 4488 2006 448c 2006     .D. .D. .D. .D. 
200647ac:	4490 2006 4498 2006 449c 2006 44a0 2006     .D. .D. .D. .D. 
200647bc:	44a4 2006 44a8 2006 44ac 2006 44b0 2006     .D. .D. .D. .D. 
200647cc:	44b4 2006 44b8 2006 44bc 2006 44c0 2006     .D. .D. .D. .D. 
200647dc:	44c4 2006 44c8 2006 44cc 2006 44d0 2006     .D. .D. .D. .D. 
200647ec:	44d4 2006 44d8 2006 44dc 2006 44e0 2006     .D. .D. .D. .D. 
200647fc:	44e4 2006 44e8 2006 44ec 2006 44f0 2006     .D. .D. .D. .D. 
2006480c:	4544 2056 5049 2043 5041 2049 736d 2067     DEV IPC API msg 
2006481c:	554e 4c4c 000a 0000 6f48 7473 4120 4950     NULL....Host API
2006482c:	5520 6b6e 6f6e 6e77 6520 7476 2528 2978      Unknown evt(%x)
2006483c:	000a 0000 04b4 2006 04c2 2006 057a 2006     ....... ... z.. 
2006484c:	051a 2006 0692 2006 06a6 2006 071c 2006     ... ... ... ... 
2006485c:	051a 2006 051a 2006 616c 7473 6920 696e     ... ... last ini
2006486c:	2063 7069 2063 6f6e 2074 6468 206c 000a     c ipc not hdl ..
2006487c:	7348 4d74 6773 6553 646e 7720 6961 2074     HstMsgSend wait 
2006488c:	6e69 6369 6920 6370 6420 6e6f 2065 7830     inic ipc done 0x
2006489c:	7825 202c 7830 7825 000a 0000 6e69 6369     %x, 0x%x....inic
200648ac:	615f 6970 685f 736f 5f74 6174 6b73 0000     _api_host_task..
200648bc:	7243 6165 6574 6120 6970 685f 736f 5f74     Create api_host_
200648cc:	6174 6b73 4520 7272 000a 0000 4e49 4349     task Err....INIC
200648dc:	0000 0000 6e69 6369 6d5f 5f70 6f63 6d6d     ....inic_mp_comm
200648ec:	6e61 2064 6166 6c69 000a 0000 7250 7669     and fail....Priv
200648fc:	7461 2065 654d 7373 6761 3a65 2520 0a73     ate Message: %s.
2006490c:	0000 0000 6f68 7473 735f 626b 785f 2078     ....host_skb_xx 
2006491c:	6f6e 2074 7266 6565 000a 0000 7325 3e3d     not free....%s=>
2006492c:	6b73 2062 616d 6c6c 636f 6620 6961 216c     skb malloc fail!
2006493c:	0d0a 0000 6f48 7473 5520 6b6e 6f6e 6e77     ....Host Unknown
2006494c:	6520 6576 746e 2528 786c 2129 0d0a 0000      event(%lx)!....

2006495c <__func__.1>:
2006495c:	6e69 6369 685f 736f 5f74 6e69 7469 735f     inic_host_init_s
2006496c:	626b 0000 6e69 6369 6d5f 6773 715f 745f     kb..inic_msg_q_t
2006497c:	7361 006b 7243 6165 6574 6d20 6773 715f     ask.Create msg_q
2006498c:	745f 7361 206b 7245 2172 000a 4f4e 6220     _task Err!..NO b
2006499c:	6675 6620 726f 6e20 7765 6e20 646f 7365     uf for new nodes
200649ac:	0a21 0000 6146 6c69 7420 206f 6966 646e     !...Fail to find
200649bc:	7420 6568 7420 6d69 7265 655f 746e 7972      the timer_entry
200649cc:	6920 206e 6974 656d 2072 6174 6c62 2e65      in timer table.
200649dc:	000a 0000 4954 454d 0052 0000 6146 6c69     ....TIMER...Fail
200649ec:	7420 206f 6e69 7469 7420 6d69 7265 0a2e      to init timer..
200649fc:	0000 0000 6f4d 5464 6d69 7265 203a 6f6e     ....ModTimer: no
20064a0c:	2074 6e69 7469 0a2e 0000 0000 6f4d 5464     t init......ModT
20064a1c:	6d69 7265 6620 6961 0a6c 0000 6143 636e     imer fail...Canc
20064a2c:	6c65 6954 656d 2072 6146 6c69 2528 2978     elTimer Fail(%x)
20064a3c:	000a 0000 6544 546c 6d69 7265 4620 6961     ....DelTimer Fai
20064a4c:	0a6c 0000                                   l...

20064a50 <__clz_tab>:
20064a50:	0100 0202 0303 0303 0404 0404 0404 0404     ................
20064a60:	0505 0505 0505 0505 0505 0505 0505 0505     ................
20064a70:	0606 0606 0606 0606 0606 0606 0606 0606     ................
20064a80:	0606 0606 0606 0606 0606 0606 0606 0606     ................
20064a90:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064aa0:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064ab0:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064ac0:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064ad0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064ae0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064af0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b00:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b10:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b20:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b30:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b40:	0808 0808 0808 0808 0808 0808 0808 0808     ................

20064b50 <_ctype_>:
20064b50:	2000 2020 2020 2020 2020 2828 2828 2028     .         ((((( 
20064b60:	2020 2020 2020 2020 2020 2020 2020 2020                     
20064b70:	8820 1010 1010 1010 1010 1010 1010 1010      ...............
20064b80:	0410 0404 0404 0404 0404 1004 1010 1010     ................
20064b90:	1010 4141 4141 4141 0101 0101 0101 0101     ..AAAAAA........
20064ba0:	0101 0101 0101 0101 0101 0101 1010 1010     ................
20064bb0:	1010 4242 4242 4242 0202 0202 0202 0202     ..BBBBBB........
20064bc0:	0202 0202 0202 0202 0202 0202 1010 1010     ................
20064bd0:	0020 0000 0000 0000 0000 0000 0000 0000      ...............
	...

20064c54 <shell_cmd_table>:
20064c54:	3254 2006 0000 0000 b3b6 2005 3258 2006     T2. ....... X2. 
20064c64:	327c 2006 0002 0000 b512 2005 3280 2006     |2. ....... .2. 
20064c74:	339c 2006 0002 0000 b46c 2005 33a0 2006     .3. ....l.. .3. 
20064c84:	3430 2006 0004 0000 b422 2005 3438 2006     04. ....".. 84. 

20064c94 <ipc_tickless_table>:
20064c94:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064ca4:	0000 0000 0010 0000 0000 0000               ............

20064cb0 <ipc_kr4cdsp_table>:
20064cb0:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064cc0:	0000 0000 0020 0000 0000 0000               .... .......

20064ccc <ipc_KM4WKR4_table>:
20064ccc:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064cdc:	0000 0000 0010 0000 0002 0000               ............

20064ce8 <ipc_shell_table>:
20064ce8:	0001 0000 cf3e 2005 0000 0000 8eac 2005     ....>.. ....... 
20064cf8:	0000 0000 0010 0000 0003 0000               ............

20064d04 <ipc_api_host_table>:
20064d04:	0000 0000 07f2 2006 0000 0000 8eac 2005     ....... ....... 
20064d14:	0000 0000 0010 0000 0007 0000               ............

20064d20 <ipc_host_event_table>:
20064d20:	0000 0000 0dae 2006 0000 0000 8eac 2005     ....... ....... 
20064d30:	0000 0000 0010 0000 0006 0000               ............

20064d3c <lib_pmc_git_rev>:
20064d3c:	6c20 6269 705f 636d 765f 7265 315f 3937      lib_pmc_ver_179
20064d4c:	6238 3166 6465 5f36 3032 3432 302f 2f38     8bf1ed6_2024/08/
20064d5c:	3132 312d 3a30 3935 303a 0032               21-10:59:02.

20064d68 <lib_wifi_com_sec_git_rev>:
20064d68:	6c20 6269 775f 6669 5f69 6f63 5f6d 6573      lib_wifi_com_se
20064d78:	5f63 6576 5f72 3731 3839 6662 6531 3664     c_ver_1798bf1ed6
20064d88:	325f 3230 2f34 3830 322f 2d31 3031 353a     _2024/08/21-10:5
20064d98:	3a39 3030                                    9:00.

20064d9d <__git_ver_table_end__>:
20064d9d:	0000                	c.unimp
	...

20064da0 <__frame_dummy_init_array_entry>:
20064da0:	1494 2006                                   ... 

20064da4 <__do_global_dtors_aux_fini_array_entry>:
20064da4:	145a 2006                                   Z.. 

20064da8 <__fini_array_end>:
	...

Disassembly of section .sram_image2.text.data:

20065dc0 <FLASH_ClockSwitch>:
20065dc0:	1101                	c.addi	sp,-32
20065dc2:	ca26                	c.swsp	s1,20(sp)
20065dc4:	c452                	c.swsp	s4,8(sp)
20065dc6:	ce06                	c.swsp	ra,28(sp)
20065dc8:	cc22                	c.swsp	s0,24(sp)
20065dca:	c84a                	c.swsp	s2,16(sp)
20065dcc:	c64e                	c.swsp	s3,12(sp)
20065dce:	c256                	c.swsp	s5,4(sp)
20065dd0:	84aa                	c.mv	s1,a0
20065dd2:	8a2e                	c.mv	s4,a1
20065dd4:	c1dd                	c.beqz	a1,20065e7a <FLASH_ClockSwitch+0xba>
20065dd6:	d01f20ef          	jal	ra,20058ad6 <Systick_State>
20065dda:	89aa                	c.mv	s3,a0
20065ddc:	d1bf20ef          	jal	ra,20058af6 <irq_disable_save>
20065de0:	892a                	c.mv	s2,a0
20065de2:	4501                	c.li	a0,0
20065de4:	cc7f20ef          	jal	ra,20058aaa <Systick_Cmd>
20065de8:	20001ab7          	lui	s5,0x20001
20065dec:	4789                	c.li	a5,2
20065dee:	864a8413          	addi	s0,s5,-1948 # 20000864 <flash_init_para>
20065df2:	08f49e63          	bne	s1,a5,20065e8e <FLASH_ClockSwitch+0xce>
20065df6:	4509                	c.li	a0,2
20065df8:	dffa0097          	auipc	ra,0xdffa0
20065dfc:	c46080e7          	jalr	ra,-954(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065e00:	4100c6b7          	lui	a3,0x4100c
20065e04:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065e08:	7775                	c.lui	a4,0xffffd
20065e0a:	177d                	c.addi	a4,-1
20065e0c:	8ff9                	c.and	a5,a4
20065e0e:	24f6ae23          	sw	a5,604(a3)
20065e12:	06344783          	lbu	a5,99(s0) # 9063 <shell_get_argv+0x18f>
20065e16:	c7ad                	c.beqz	a5,20065e80 <FLASH_ClockSwitch+0xc0>
20065e18:	4501                	c.li	a0,0
20065e1a:	dff9e097          	auipc	ra,0xdff9e
20065e1e:	b60080e7          	jalr	ra,-1184(ra) # 397a <FLASH_CalibrationNewCmd>
20065e22:	4505                	c.li	a0,1
20065e24:	dff9e097          	auipc	ra,0xdff9e
20065e28:	914080e7          	jalr	ra,-1772(ra) # 3738 <FLASH_CalibrationPLLSel>
20065e2c:	dff9e097          	auipc	ra,0xdff9e
20065e30:	97e080e7          	jalr	ra,-1666(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065e34:	4501                	c.li	a0,0
20065e36:	dff9e097          	auipc	ra,0xdff9e
20065e3a:	902080e7          	jalr	ra,-1790(ra) # 3738 <FLASH_CalibrationPLLSel>
20065e3e:	dff9e097          	auipc	ra,0xdff9e
20065e42:	96c080e7          	jalr	ra,-1684(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065e46:	04040ea3          	sb	zero,93(s0)
20065e4a:	00444583          	lbu	a1,4(s0)
20065e4e:	864a8513          	addi	a0,s5,-1948
20065e52:	dff9d097          	auipc	ra,0xdff9d
20065e56:	e72080e7          	jalr	ra,-398(ra) # 2cc4 <FLASH_SetSpiMode>
20065e5a:	0c0a0c63          	beq	s4,zero,20065f32 <FLASH_ClockSwitch+0x172>
20065e5e:	854e                	c.mv	a0,s3
20065e60:	c4bf20ef          	jal	ra,20058aaa <Systick_Cmd>
20065e64:	4462                	c.lwsp	s0,24(sp)
20065e66:	40f2                	c.lwsp	ra,28(sp)
20065e68:	44d2                	c.lwsp	s1,20(sp)
20065e6a:	49b2                	c.lwsp	s3,12(sp)
20065e6c:	4a22                	c.lwsp	s4,8(sp)
20065e6e:	4a92                	c.lwsp	s5,4(sp)
20065e70:	854a                	c.mv	a0,s2
20065e72:	4942                	c.lwsp	s2,16(sp)
20065e74:	6105                	c.addi16sp	sp,32
20065e76:	c89f206f          	jal	zero,20058afe <irq_enable_restore>
20065e7a:	4901                	c.li	s2,0
20065e7c:	4981                	c.li	s3,0
20065e7e:	b7ad                	c.j	20065de8 <FLASH_ClockSwitch+0x28>
20065e80:	4581                	c.li	a1,0
20065e82:	4501                	c.li	a0,0
20065e84:	dff9e097          	auipc	ra,0xdff9e
20065e88:	cfa080e7          	jalr	ra,-774(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
20065e8c:	bf59                	c.j	20065e22 <FLASH_ClockSwitch+0x62>
20065e8e:	ccb5                	c.beqz	s1,20065f0a <FLASH_ClockSwitch+0x14a>
20065e90:	4501                	c.li	a0,0
20065e92:	dff9e097          	auipc	ra,0xdff9e
20065e96:	8a6080e7          	jalr	ra,-1882(ra) # 3738 <FLASH_CalibrationPLLSel>
20065e9a:	dff9e097          	auipc	ra,0xdff9e
20065e9e:	8b2080e7          	jalr	ra,-1870(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065ea2:	4505                	c.li	a0,1
20065ea4:	dff9e097          	auipc	ra,0xdff9e
20065ea8:	894080e7          	jalr	ra,-1900(ra) # 3738 <FLASH_CalibrationPLLSel>
20065eac:	dff9e097          	auipc	ra,0xdff9e
20065eb0:	8a0080e7          	jalr	ra,-1888(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065eb4:	06344503          	lbu	a0,99(s0)
20065eb8:	891d                	c.andi	a0,7
20065eba:	dff9e097          	auipc	ra,0xdff9e
20065ebe:	970080e7          	jalr	ra,-1680(ra) # 382a <FLASH_CalibrationPLLPS_Shift>
20065ec2:	4100c737          	lui	a4,0x4100c
20065ec6:	25c72783          	lw	a5,604(a4) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065eca:	668d                	c.lui	a3,0x3
20065ecc:	8fd5                	c.or	a5,a3
20065ece:	24f72e23          	sw	a5,604(a4)
20065ed2:	4100c6b7          	lui	a3,0x4100c
20065ed6:	470d                	c.li	a4,3
20065ed8:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065edc:	8b8d                	c.andi	a5,3
20065ede:	fee79de3          	bne	a5,a4,20065ed8 <FLASH_ClockSwitch+0x118>
20065ee2:	06344783          	lbu	a5,99(s0)
20065ee6:	cf8d                	c.beqz	a5,20065f20 <FLASH_ClockSwitch+0x160>
20065ee8:	4505                	c.li	a0,1
20065eea:	dff9e097          	auipc	ra,0xdff9e
20065eee:	a90080e7          	jalr	ra,-1392(ra) # 397a <FLASH_CalibrationNewCmd>
20065ef2:	4505                	c.li	a0,1
20065ef4:	e091                	c.bnez	s1,20065ef8 <FLASH_ClockSwitch+0x138>
20065ef6:	4501                	c.li	a0,0
20065ef8:	dffa0097          	auipc	ra,0xdffa0
20065efc:	b46080e7          	jalr	ra,-1210(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065f00:	05c44783          	lbu	a5,92(s0)
20065f04:	04f40ea3          	sb	a5,93(s0)
20065f08:	b789                	c.j	20065e4a <FLASH_ClockSwitch+0x8a>
20065f0a:	4505                	c.li	a0,1
20065f0c:	dff9e097          	auipc	ra,0xdff9e
20065f10:	82c080e7          	jalr	ra,-2004(ra) # 3738 <FLASH_CalibrationPLLSel>
20065f14:	dff9e097          	auipc	ra,0xdff9e
20065f18:	838080e7          	jalr	ra,-1992(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065f1c:	4501                	c.li	a0,0
20065f1e:	b759                	c.j	20065ea4 <FLASH_ClockSwitch+0xe4>
20065f20:	05c44503          	lbu	a0,92(s0)
20065f24:	4585                	c.li	a1,1
20065f26:	1579                	c.addi	a0,-2
20065f28:	dff9e097          	auipc	ra,0xdff9e
20065f2c:	c56080e7          	jalr	ra,-938(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
20065f30:	b7c9                	c.j	20065ef2 <FLASH_ClockSwitch+0x132>
20065f32:	40f2                	c.lwsp	ra,28(sp)
20065f34:	4462                	c.lwsp	s0,24(sp)
20065f36:	44d2                	c.lwsp	s1,20(sp)
20065f38:	4942                	c.lwsp	s2,16(sp)
20065f3a:	49b2                	c.lwsp	s3,12(sp)
20065f3c:	4a22                	c.lwsp	s4,8(sp)
20065f3e:	4a92                	c.lwsp	s5,4(sp)
20065f40:	6105                	c.addi16sp	sp,32
20065f42:	8082                	c.jr	ra

20065f44 <FLASH_UserMode_Exit>:
20065f44:	4100a737          	lui	a4,0x4100a
20065f48:	431c                	c.lw	a5,0(a4)
20065f4a:	0786                	c.slli	a5,0x1
20065f4c:	8385                	c.srli	a5,0x1
20065f4e:	c31c                	c.sw	a5,0(a4)
20065f50:	431c                	c.lw	a5,0(a4)
20065f52:	fe07cfe3          	blt	a5,zero,20065f50 <FLASH_UserMode_Exit+0xc>
20065f56:	0ff0000f          	fence	iorw,iorw
20065f5a:	0001                	c.addi	zero,0
20065f5c:	0001                	c.addi	zero,0
20065f5e:	0001                	c.addi	zero,0
20065f60:	0001                	c.addi	zero,0
20065f62:	0001                	c.addi	zero,0
20065f64:	0001                	c.addi	zero,0
20065f66:	0001                	c.addi	zero,0
20065f68:	0001                	c.addi	zero,0
20065f6a:	0001                	c.addi	zero,0
20065f6c:	0001                	c.addi	zero,0
20065f6e:	0001                	c.addi	zero,0
20065f70:	8082                	c.jr	ra

20065f72 <FLASH_WaitBusy_InUserMode>:
20065f72:	7179                	c.addi16sp	sp,-48
20065f74:	d422                	c.swsp	s0,40(sp)
20065f76:	20001437          	lui	s0,0x20001
20065f7a:	d226                	c.swsp	s1,36(sp)
20065f7c:	d04a                	c.swsp	s2,32(sp)
20065f7e:	ce4e                	c.swsp	s3,28(sp)
20065f80:	cc52                	c.swsp	s4,24(sp)
20065f82:	ca56                	c.swsp	s5,20(sp)
20065f84:	d606                	c.swsp	ra,44(sp)
20065f86:	84aa                	c.mv	s1,a0
20065f88:	000107a3          	sb	zero,15(sp)
20065f8c:	4781                	c.li	a5,0
20065f8e:	4911                	c.li	s2,4
20065f90:	4985                	c.li	s3,1
20065f92:	4a0d                	c.li	s4,3
20065f94:	86440413          	addi	s0,s0,-1948 # 20000864 <flash_init_para>
20065f98:	4100aab7          	lui	s5,0x4100a
20065f9c:	ec91                	c.bnez	s1,20065fb8 <FLASH_WaitBusy_InUserMode+0x46>
20065f9e:	028aa783          	lw	a5,40(s5) # 4100a028 <__km4_bd_boot_download_addr__+0x10ff8028>
20065fa2:	8b85                	c.andi	a5,1
20065fa4:	ffe5                	c.bnez	a5,20065f9c <FLASH_WaitBusy_InUserMode+0x2a>
20065fa6:	50b2                	c.lwsp	ra,44(sp)
20065fa8:	5422                	c.lwsp	s0,40(sp)
20065faa:	5492                	c.lwsp	s1,36(sp)
20065fac:	5902                	c.lwsp	s2,32(sp)
20065fae:	49f2                	c.lwsp	s3,28(sp)
20065fb0:	4a62                	c.lwsp	s4,24(sp)
20065fb2:	4ad2                	c.lwsp	s5,20(sp)
20065fb4:	6145                	c.addi16sp	sp,48
20065fb6:	8082                	c.jr	ra
20065fb8:	01249563          	bne	s1,s2,20065fc2 <FLASH_WaitBusy_InUserMode+0x50>
20065fbc:	008aa783          	lw	a5,8(s5)
20065fc0:	b7cd                	c.j	20065fa2 <FLASH_WaitBusy_InUserMode+0x30>
20065fc2:	01349d63          	bne	s1,s3,20065fdc <FLASH_WaitBusy_InUserMode+0x6a>
20065fc6:	05244503          	lbu	a0,82(s0)
20065fca:	00f10613          	addi	a2,sp,15
20065fce:	4585                	c.li	a1,1
20065fd0:	288d                	c.jal	20066042 <FLASH_RxCmd_InUserMode>
20065fd2:	00f14783          	lbu	a5,15(sp)
20065fd6:	4818                	c.lw	a4,16(s0)
20065fd8:	8ff9                	c.and	a5,a4
20065fda:	b7e9                	c.j	20065fa4 <FLASH_WaitBusy_InUserMode+0x32>
20065fdc:	fd4494e3          	bne	s1,s4,20065fa4 <FLASH_WaitBusy_InUserMode+0x32>
20065fe0:	05244503          	lbu	a0,82(s0)
20065fe4:	00f10613          	addi	a2,sp,15
20065fe8:	4585                	c.li	a1,1
20065fea:	28a1                	c.jal	20066042 <FLASH_RxCmd_InUserMode>
20065fec:	00f14703          	lbu	a4,15(sp)
20065ff0:	4854                	c.lw	a3,20(s0)
20065ff2:	4785                	c.li	a5,1
20065ff4:	8ef9                	c.and	a3,a4
20065ff6:	d6dd                	c.beqz	a3,20065fa4 <FLASH_WaitBusy_InUserMode+0x32>
20065ff8:	481c                	c.lw	a5,16(s0)
20065ffa:	8ff9                	c.and	a5,a4
20065ffc:	00f037b3          	sltu	a5,zero,a5
20066000:	b755                	c.j	20065fa4 <FLASH_WaitBusy_InUserMode+0x32>

20066002 <FLASH_UserMode_Enter>:
20066002:	1141                	c.addi	sp,-16
20066004:	c606                	c.swsp	ra,12(sp)
20066006:	0ff0000f          	fence	iorw,iorw
2006600a:	0ff0000f          	fence	iorw,iorw
2006600e:	0001                	c.addi	zero,0
20066010:	0001                	c.addi	zero,0
20066012:	0001                	c.addi	zero,0
20066014:	0001                	c.addi	zero,0
20066016:	0001                	c.addi	zero,0
20066018:	0001                	c.addi	zero,0
2006601a:	0001                	c.addi	zero,0
2006601c:	0001                	c.addi	zero,0
2006601e:	0001                	c.addi	zero,0
20066020:	0001                	c.addi	zero,0
20066022:	0001                	c.addi	zero,0
20066024:	4501                	c.li	a0,0
20066026:	37b1                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
20066028:	4100a737          	lui	a4,0x4100a
2006602c:	431c                	c.lw	a5,0(a4)
2006602e:	800006b7          	lui	a3,0x80000
20066032:	8fd5                	c.or	a5,a3
20066034:	c31c                	c.sw	a5,0(a4)
20066036:	431c                	c.lw	a5,0(a4)
20066038:	fe07dfe3          	bge	a5,zero,20066036 <FLASH_UserMode_Enter+0x34>
2006603c:	40b2                	c.lwsp	ra,12(sp)
2006603e:	0141                	c.addi	sp,16
20066040:	8082                	c.jr	ra

20066042 <FLASH_RxCmd_InUserMode>:
20066042:	1101                	c.addi	sp,-32
20066044:	cc22                	c.swsp	s0,24(sp)
20066046:	20001437          	lui	s0,0x20001
2006604a:	ca26                	c.swsp	s1,20(sp)
2006604c:	c64e                	c.swsp	s3,12(sp)
2006604e:	84ae                	c.mv	s1,a1
20066050:	89aa                	c.mv	s3,a0
20066052:	4581                	c.li	a1,0
20066054:	86440513          	addi	a0,s0,-1948 # 20000864 <flash_init_para>
20066058:	c84a                	c.swsp	s2,16(sp)
2006605a:	ce06                	c.swsp	ra,28(sp)
2006605c:	8932                	c.mv	s2,a2
2006605e:	dff9d097          	auipc	ra,0xdff9d
20066062:	c66080e7          	jalr	ra,-922(ra) # 2cc4 <FLASH_SetSpiMode>
20066066:	4100a7b7          	lui	a5,0x4100a
2006606a:	10078613          	addi	a2,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
2006606e:	4e18                	c.lw	a4,24(a2)
20066070:	fff106b7          	lui	a3,0xfff10
20066074:	fff68593          	addi	a1,a3,-1 # fff0ffff <__ctrace_end__+0x35f0ffff>
20066078:	8f6d                	c.and	a4,a1
2006607a:	ce18                	c.sw	a4,24(a2)
2006607c:	00849713          	slli	a4,s1,0x8
20066080:	8321                	c.srli	a4,0x8
20066082:	c3d8                	c.sw	a4,4(a5)
20066084:	1207a823          	sw	zero,304(a5)
20066088:	4398                	c.lw	a4,0(a5)
2006608a:	cff68693          	addi	a3,a3,-769
2006608e:	8f75                	c.and	a4,a3
20066090:	30076713          	ori	a4,a4,768
20066094:	c398                	c.sw	a4,0(a5)
20066096:	07378023          	sb	s3,96(a5)
2006609a:	4705                	c.li	a4,1
2006609c:	c798                	c.sw	a4,8(a5)
2006609e:	4100a6b7          	lui	a3,0x4100a
200660a2:	4781                	c.li	a5,0
200660a4:	02979463          	bne	a5,s1,200660cc <FLASH_RxCmd_InUserMode+0x8a>
200660a8:	4511                	c.li	a0,4
200660aa:	35e1                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
200660ac:	86440793          	addi	a5,s0,-1948
200660b0:	86440513          	addi	a0,s0,-1948
200660b4:	4462                	c.lwsp	s0,24(sp)
200660b6:	40f2                	c.lwsp	ra,28(sp)
200660b8:	44d2                	c.lwsp	s1,20(sp)
200660ba:	4942                	c.lwsp	s2,16(sp)
200660bc:	49b2                	c.lwsp	s3,12(sp)
200660be:	0047c583          	lbu	a1,4(a5)
200660c2:	6105                	c.addi16sp	sp,32
200660c4:	dff9d317          	auipc	t1,0xdff9d
200660c8:	c0030067          	jalr	zero,-1024(t1) # 2cc4 <FLASH_SetSpiMode>
200660cc:	5698                	c.lw	a4,40(a3)
200660ce:	8b21                	c.andi	a4,8
200660d0:	db71                	c.beqz	a4,200660a4 <FLASH_RxCmd_InUserMode+0x62>
200660d2:	0606c603          	lbu	a2,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
200660d6:	00f90733          	add	a4,s2,a5
200660da:	0785                	c.addi	a5,1
200660dc:	00c70023          	sb	a2,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200660e0:	b7d1                	c.j	200660a4 <FLASH_RxCmd_InUserMode+0x62>

200660e2 <FLASH_RxCmd>:
200660e2:	1101                	c.addi	sp,-32
200660e4:	ce06                	c.swsp	ra,28(sp)
200660e6:	c62a                	c.swsp	a0,12(sp)
200660e8:	c42e                	c.swsp	a1,8(sp)
200660ea:	c232                	c.swsp	a2,4(sp)
200660ec:	3f19                	c.jal	20066002 <FLASH_UserMode_Enter>
200660ee:	4612                	c.lwsp	a2,4(sp)
200660f0:	45a2                	c.lwsp	a1,8(sp)
200660f2:	4532                	c.lwsp	a0,12(sp)
200660f4:	37b9                	c.jal	20066042 <FLASH_RxCmd_InUserMode>
200660f6:	40f2                	c.lwsp	ra,28(sp)
200660f8:	6105                	c.addi16sp	sp,32
200660fa:	b5a9                	c.j	20065f44 <FLASH_UserMode_Exit>

200660fc <FLASH_TxCmd_InUserMode>:
200660fc:	4100a7b7          	lui	a5,0x4100a
20066100:	1141                	c.addi	sp,-16
20066102:	c606                	c.swsp	ra,12(sp)
20066104:	c422                	c.swsp	s0,8(sp)
20066106:	c226                	c.swsp	s1,4(sp)
20066108:	c04a                	c.swsp	s2,0(sp)
2006610a:	10078713          	addi	a4,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
2006610e:	01872903          	lw	s2,24(a4)
20066112:	4384                	c.lw	s1,0(a5)
20066114:	4394                	c.lw	a3,0(a5)
20066116:	ffc10837          	lui	a6,0xffc10
2006611a:	cff80813          	addi	a6,a6,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
2006611e:	0106f6b3          	and	a3,a3,a6
20066122:	c394                	c.sw	a3,0(a5)
20066124:	4f14                	c.lw	a3,24(a4)
20066126:	fff10837          	lui	a6,0xfff10
2006612a:	187d                	c.addi	a6,-1
2006612c:	0106f6b3          	and	a3,a3,a6
20066130:	cf14                	c.sw	a3,24(a4)
20066132:	0007a223          	sw	zero,4(a5)
20066136:	db0c                	c.sw	a1,48(a4)
20066138:	06a78023          	sb	a0,96(a5)
2006613c:	4100a737          	lui	a4,0x4100a
20066140:	4781                	c.li	a5,0
20066142:	02b7e163          	bltu	a5,a1,20066164 <FLASH_TxCmd_InUserMode+0x68>
20066146:	4100a437          	lui	s0,0x4100a
2006614a:	4785                	c.li	a5,1
2006614c:	c41c                	c.sw	a5,8(s0)
2006614e:	4511                	c.li	a0,4
20066150:	350d                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
20066152:	11242c23          	sw	s2,280(s0) # 4100a118 <__km4_bd_boot_download_addr__+0x10ff8118>
20066156:	c004                	c.sw	s1,0(s0)
20066158:	40b2                	c.lwsp	ra,12(sp)
2006615a:	4422                	c.lwsp	s0,8(sp)
2006615c:	4492                	c.lwsp	s1,4(sp)
2006615e:	4902                	c.lwsp	s2,0(sp)
20066160:	0141                	c.addi	sp,16
20066162:	8082                	c.jr	ra
20066164:	00f606b3          	add	a3,a2,a5
20066168:	0006c683          	lbu	a3,0(a3)
2006616c:	0785                	c.addi	a5,1
2006616e:	06d70023          	sb	a3,96(a4) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20066172:	bfc1                	c.j	20066142 <FLASH_TxCmd_InUserMode+0x46>

20066174 <FLASH_WriteEn_InUserMode>:
20066174:	1141                	c.addi	sp,-16
20066176:	4505                	c.li	a0,1
20066178:	c606                	c.swsp	ra,12(sp)
2006617a:	3be5                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
2006617c:	200017b7          	lui	a5,0x20001
20066180:	8b47c503          	lbu	a0,-1868(a5) # 200008b4 <flash_init_para+0x50>
20066184:	4601                	c.li	a2,0
20066186:	4581                	c.li	a1,0
20066188:	3f95                	c.jal	200660fc <FLASH_TxCmd_InUserMode>
2006618a:	40b2                	c.lwsp	ra,12(sp)
2006618c:	450d                	c.li	a0,3
2006618e:	0141                	c.addi	sp,16
20066190:	b3cd                	c.j	20065f72 <FLASH_WaitBusy_InUserMode>

20066192 <FLASH_TxData>:
20066192:	1101                	c.addi	sp,-32
20066194:	0ff57793          	andi	a5,a0,255
20066198:	cc22                	c.swsp	s0,24(sp)
2006619a:	ca26                	c.swsp	s1,20(sp)
2006619c:	c84a                	c.swsp	s2,16(sp)
2006619e:	ce06                	c.swsp	ra,28(sp)
200661a0:	c64e                	c.swsp	s3,12(sp)
200661a2:	97ae                	c.add	a5,a1
200661a4:	10000713          	addi	a4,zero,256
200661a8:	842a                	c.mv	s0,a0
200661aa:	84ae                	c.mv	s1,a1
200661ac:	8932                	c.mv	s2,a2
200661ae:	00f77a63          	bgeu	a4,a5,200661c2 <FLASH_TxData+0x30>
200661b2:	20062537          	lui	a0,0x20062
200661b6:	23a00593          	addi	a1,zero,570
200661ba:	01050513          	addi	a0,a0,16 # 20062010 <strtol+0xe>
200661be:	eeef90ef          	jal	ra,2005f8ac <io_assert_failed>
200661c2:	3581                	c.jal	20066002 <FLASH_UserMode_Enter>
200661c4:	3f45                	c.jal	20066174 <FLASH_WriteEn_InUserMode>
200661c6:	4100a7b7          	lui	a5,0x4100a
200661ca:	0007a983          	lw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200661ce:	4398                	c.lw	a4,0(a5)
200661d0:	ffc106b7          	lui	a3,0xffc10
200661d4:	cff68693          	addi	a3,a3,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
200661d8:	8f75                	c.and	a4,a3
200661da:	200016b7          	lui	a3,0x20001
200661de:	c398                	c.sw	a4,0(a5)
200661e0:	86468693          	addi	a3,a3,-1948 # 20000864 <flash_init_para>
200661e4:	0646c703          	lbu	a4,100(a3)
200661e8:	1187a603          	lw	a2,280(a5)
200661ec:	000f05b7          	lui	a1,0xf0
200661f0:	0742                	c.slli	a4,0x10
200661f2:	8f6d                	c.and	a4,a1
200661f4:	fff105b7          	lui	a1,0xfff10
200661f8:	15fd                	c.addi	a1,-1
200661fa:	8e6d                	c.and	a2,a1
200661fc:	8f51                	c.or	a4,a2
200661fe:	10e7ac23          	sw	a4,280(a5)
20066202:	00849713          	slli	a4,s1,0x8
20066206:	0007a223          	sw	zero,4(a5)
2006620a:	8321                	c.srli	a4,0x8
2006620c:	12e7a823          	sw	a4,304(a5)
20066210:	4709                	c.li	a4,2
20066212:	06e78023          	sb	a4,96(a5)
20066216:	0646c683          	lbu	a3,100(a3)
2006621a:	4711                	c.li	a4,4
2006621c:	00e69663          	bne	a3,a4,20066228 <FLASH_TxData+0x96>
20066220:	01845713          	srli	a4,s0,0x18
20066224:	06e78023          	sb	a4,96(a5)
20066228:	01045713          	srli	a4,s0,0x10
2006622c:	4100a7b7          	lui	a5,0x4100a
20066230:	0ff77713          	andi	a4,a4,255
20066234:	06e78023          	sb	a4,96(a5) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20066238:	00845713          	srli	a4,s0,0x8
2006623c:	0ff77713          	andi	a4,a4,255
20066240:	06e78023          	sb	a4,96(a5)
20066244:	0ff47413          	andi	s0,s0,255
20066248:	06878023          	sb	s0,96(a5)
2006624c:	4100a6b7          	lui	a3,0x4100a
20066250:	4781                	c.li	a5,0
20066252:	00978563          	beq	a5,s1,2006625c <FLASH_TxData+0xca>
20066256:	5698                	c.lw	a4,40(a3)
20066258:	8b09                	c.andi	a4,2
2006625a:	eb05                	c.bnez	a4,2006628a <FLASH_TxData+0xf8>
2006625c:	4685                	c.li	a3,1
2006625e:	4100a737          	lui	a4,0x4100a
20066262:	c714                	c.sw	a3,8(a4)
20066264:	4100a6b7          	lui	a3,0x4100a
20066268:	0297e963          	bltu	a5,s1,2006629a <FLASH_TxData+0x108>
2006626c:	4511                	c.li	a0,4
2006626e:	3311                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
20066270:	4100a7b7          	lui	a5,0x4100a
20066274:	0137a023          	sw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20066278:	4505                	c.li	a0,1
2006627a:	39e5                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
2006627c:	4462                	c.lwsp	s0,24(sp)
2006627e:	40f2                	c.lwsp	ra,28(sp)
20066280:	44d2                	c.lwsp	s1,20(sp)
20066282:	4942                	c.lwsp	s2,16(sp)
20066284:	49b2                	c.lwsp	s3,12(sp)
20066286:	6105                	c.addi16sp	sp,32
20066288:	b975                	c.j	20065f44 <FLASH_UserMode_Exit>
2006628a:	00f90733          	add	a4,s2,a5
2006628e:	00074703          	lbu	a4,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20066292:	0785                	c.addi	a5,1
20066294:	06e68023          	sb	a4,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20066298:	bf6d                	c.j	20066252 <FLASH_TxData+0xc0>
2006629a:	5698                	c.lw	a4,40(a3)
2006629c:	8b09                	c.andi	a4,2
2006629e:	d769                	c.beqz	a4,20066268 <FLASH_TxData+0xd6>
200662a0:	00f90733          	add	a4,s2,a5
200662a4:	00074703          	lbu	a4,0(a4)
200662a8:	0785                	c.addi	a5,1
200662aa:	06e68023          	sb	a4,96(a3)
200662ae:	bf6d                	c.j	20066268 <FLASH_TxData+0xd6>

200662b0 <FLASH_SetStatus>:
200662b0:	1101                	c.addi	sp,-32
200662b2:	ce06                	c.swsp	ra,28(sp)
200662b4:	cc22                	c.swsp	s0,24(sp)
200662b6:	c62a                	c.swsp	a0,12(sp)
200662b8:	842e                	c.mv	s0,a1
200662ba:	c432                	c.swsp	a2,8(sp)
200662bc:	3399                	c.jal	20066002 <FLASH_UserMode_Enter>
200662be:	3d5d                	c.jal	20066174 <FLASH_WriteEn_InUserMode>
200662c0:	4622                	c.lwsp	a2,8(sp)
200662c2:	4532                	c.lwsp	a0,12(sp)
200662c4:	0ff47593          	andi	a1,s0,255
200662c8:	3d15                	c.jal	200660fc <FLASH_TxCmd_InUserMode>
200662ca:	4505                	c.li	a0,1
200662cc:	315d                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
200662ce:	4462                	c.lwsp	s0,24(sp)
200662d0:	40f2                	c.lwsp	ra,28(sp)
200662d2:	6105                	c.addi16sp	sp,32
200662d4:	b985                	c.j	20065f44 <FLASH_UserMode_Exit>

200662d6 <FLASH_SetStatusBits>:
200662d6:	7179                	c.addi16sp	sp,-48
200662d8:	d606                	c.swsp	ra,44(sp)
200662da:	d422                	c.swsp	s0,40(sp)
200662dc:	d226                	c.swsp	s1,36(sp)
200662de:	ce4e                	c.swsp	s3,28(sp)
200662e0:	842a                	c.mv	s0,a0
200662e2:	89ae                	c.mv	s3,a1
200662e4:	200014b7          	lui	s1,0x20001
200662e8:	d04a                	c.swsp	s2,32(sp)
200662ea:	3b21                	c.jal	20066002 <FLASH_UserMode_Enter>
200662ec:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
200662f0:	0527c503          	lbu	a0,82(a5)
200662f4:	0070                	c.addi4spn	a2,sp,12
200662f6:	4585                	c.li	a1,1
200662f8:	33a9                	c.jal	20066042 <FLASH_RxCmd_InUserMode>
200662fa:	4785                	c.li	a5,1
200662fc:	00c14703          	lbu	a4,12(sp)
20066300:	86448493          	addi	s1,s1,-1948
20066304:	04f99d63          	bne	s3,a5,2006635e <FLASH_SetStatusBits+0x88>
20066308:	00e467b3          	or	a5,s0,a4
2006630c:	0ff7f793          	andi	a5,a5,255
20066310:	00f10623          	sb	a5,12(sp)
20066314:	4c9c                	c.lw	a5,24(s1)
20066316:	4905                	c.li	s2,1
20066318:	c385                	c.beqz	a5,20066338 <FLASH_SetStatusBits+0x62>
2006631a:	0534c503          	lbu	a0,83(s1)
2006631e:	00d10613          	addi	a2,sp,13
20066322:	4585                	c.li	a1,1
20066324:	3b39                	c.jal	20066042 <FLASH_RxCmd_InUserMode>
20066326:	00d14503          	lbu	a0,13(sp)
2006632a:	8021                	c.srli	s0,0x8
2006632c:	03299d63          	bne	s3,s2,20066366 <FLASH_SetStatusBits+0x90>
20066330:	8c49                	c.or	s0,a0
20066332:	008106a3          	sb	s0,13(sp)
20066336:	4909                	c.li	s2,2
20066338:	0554c783          	lbu	a5,85(s1)
2006633c:	eb8d                	c.bnez	a5,2006636e <FLASH_SetStatusBits+0x98>
2006633e:	3d1d                	c.jal	20066174 <FLASH_WriteEn_InUserMode>
20066340:	0544c503          	lbu	a0,84(s1)
20066344:	0070                	c.addi4spn	a2,sp,12
20066346:	85ca                	c.mv	a1,s2
20066348:	3b55                	c.jal	200660fc <FLASH_TxCmd_InUserMode>
2006634a:	4505                	c.li	a0,1
2006634c:	311d                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
2006634e:	3edd                	c.jal	20065f44 <FLASH_UserMode_Exit>
20066350:	50b2                	c.lwsp	ra,44(sp)
20066352:	5422                	c.lwsp	s0,40(sp)
20066354:	5492                	c.lwsp	s1,36(sp)
20066356:	5902                	c.lwsp	s2,32(sp)
20066358:	49f2                	c.lwsp	s3,28(sp)
2006635a:	6145                	c.addi16sp	sp,48
2006635c:	8082                	c.jr	ra
2006635e:	fff44793          	xori	a5,s0,-1
20066362:	8ff9                	c.and	a5,a4
20066364:	b765                	c.j	2006630c <FLASH_SetStatusBits+0x36>
20066366:	fff44413          	xori	s0,s0,-1
2006636a:	8c69                	c.and	s0,a0
2006636c:	b7d9                	c.j	20066332 <FLASH_SetStatusBits+0x5c>
2006636e:	3519                	c.jal	20066174 <FLASH_WriteEn_InUserMode>
20066370:	0544c503          	lbu	a0,84(s1)
20066374:	0070                	c.addi4spn	a2,sp,12
20066376:	4585                	c.li	a1,1
20066378:	3351                	c.jal	200660fc <FLASH_TxCmd_InUserMode>
2006637a:	4505                	c.li	a0,1
2006637c:	3edd                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
2006637e:	3bdd                	c.jal	20066174 <FLASH_WriteEn_InUserMode>
20066380:	0554c503          	lbu	a0,85(s1)
20066384:	00d10613          	addi	a2,sp,13
20066388:	4585                	c.li	a1,1
2006638a:	bf7d                	c.j	20066348 <FLASH_SetStatusBits+0x72>

2006638c <FLASH_Erase>:
2006638c:	7179                	c.addi16sp	sp,-48
2006638e:	d422                	c.swsp	s0,40(sp)
20066390:	d04a                	c.swsp	s2,32(sp)
20066392:	d606                	c.swsp	ra,44(sp)
20066394:	d226                	c.swsp	s1,36(sp)
20066396:	4789                	c.li	a5,2
20066398:	892a                	c.mv	s2,a0
2006639a:	842e                	c.mv	s0,a1
2006639c:	00a7fa63          	bgeu	a5,a0,200663b0 <FLASH_Erase+0x24>
200663a0:	20062537          	lui	a0,0x20062
200663a4:	2d700593          	addi	a1,zero,727
200663a8:	01050513          	addi	a0,a0,16 # 20062010 <strtol+0xe>
200663ac:	d00f90ef          	jal	ra,2005f8ac <io_assert_failed>
200663b0:	200014b7          	lui	s1,0x20001
200663b4:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
200663b8:	0647c583          	lbu	a1,100(a5)
200663bc:	00845713          	srli	a4,s0,0x8
200663c0:	01045793          	srli	a5,s0,0x10
200663c4:	4611                	c.li	a2,4
200663c6:	0ff7f793          	andi	a5,a5,255
200663ca:	0ff77713          	andi	a4,a4,255
200663ce:	0ff47693          	andi	a3,s0,255
200663d2:	86448493          	addi	s1,s1,-1948
200663d6:	04c59463          	bne	a1,a2,2006641e <FLASH_Erase+0x92>
200663da:	01845713          	srli	a4,s0,0x18
200663de:	00f10ea3          	sb	a5,29(sp)
200663e2:	00841793          	slli	a5,s0,0x8
200663e6:	0442                	c.slli	s0,0x10
200663e8:	8041                	c.srli	s0,0x10
200663ea:	8021                	c.srli	s0,0x8
200663ec:	8c5d                	c.or	s0,a5
200663ee:	00e10e23          	sb	a4,28(sp)
200663f2:	00811f23          	sh	s0,30(sp)
200663f6:	c62e                	c.swsp	a1,12(sp)
200663f8:	3129                	c.jal	20066002 <FLASH_UserMode_Enter>
200663fa:	3bad                	c.jal	20066174 <FLASH_WriteEn_InUserMode>
200663fc:	45b2                	c.lwsp	a1,12(sp)
200663fe:	02091863          	bne	s2,zero,2006642e <FLASH_Erase+0xa2>
20066402:	0564c503          	lbu	a0,86(s1)
20066406:	4601                	c.li	a2,0
20066408:	4581                	c.li	a1,0
2006640a:	39cd                	c.jal	200660fc <FLASH_TxCmd_InUserMode>
2006640c:	4505                	c.li	a0,1
2006640e:	3695                	c.jal	20065f72 <FLASH_WaitBusy_InUserMode>
20066410:	3e15                	c.jal	20065f44 <FLASH_UserMode_Exit>
20066412:	50b2                	c.lwsp	ra,44(sp)
20066414:	5422                	c.lwsp	s0,40(sp)
20066416:	5492                	c.lwsp	s1,36(sp)
20066418:	5902                	c.lwsp	s2,32(sp)
2006641a:	6145                	c.addi16sp	sp,48
2006641c:	8082                	c.jr	ra
2006641e:	00f10e23          	sb	a5,28(sp)
20066422:	00e10ea3          	sb	a4,29(sp)
20066426:	00d10f23          	sb	a3,30(sp)
2006642a:	458d                	c.li	a1,3
2006642c:	b7e9                	c.j	200663f6 <FLASH_Erase+0x6a>
2006642e:	4785                	c.li	a5,1
20066430:	00f91663          	bne	s2,a5,2006643c <FLASH_Erase+0xb0>
20066434:	0574c503          	lbu	a0,87(s1)
20066438:	0870                	c.addi4spn	a2,sp,28
2006643a:	bfc1                	c.j	2006640a <FLASH_Erase+0x7e>
2006643c:	4789                	c.li	a5,2
2006643e:	fcf917e3          	bne	s2,a5,2006640c <FLASH_Erase+0x80>
20066442:	0584c503          	lbu	a0,88(s1)
20066446:	0870                	c.addi4spn	a2,sp,28
20066448:	b7c9                	c.j	2006640a <FLASH_Erase+0x7e>

2006644a <plic_get_pending>:
2006644a:	f1402773          	csrrs	a4,mhartid,zero
2006644e:	00251793          	slli	a5,a0,0x2
20066452:	80001537          	lui	a0,0x80001
20066456:	c0050513          	addi	a0,a0,-1024 # 80000c00 <__ctrace_end__+0xb6000c00>
2006645a:	97aa                	c.add	a5,a0
2006645c:	0722                	c.slli	a4,0x8
2006645e:	97ba                	c.add	a5,a4
20066460:	4388                	c.lw	a0,0(a5)
20066462:	8082                	c.jr	ra

20066464 <plic_interrupt_handler>:
20066464:	7179                	c.addi16sp	sp,-48
20066466:	d422                	c.swsp	s0,40(sp)
20066468:	d226                	c.swsp	s1,36(sp)
2006646a:	ce4e                	c.swsp	s3,28(sp)
2006646c:	d606                	c.swsp	ra,44(sp)
2006646e:	d04a                	c.swsp	s2,32(sp)
20066470:	0044                	c.addi4spn	s1,sp,4
20066472:	4401                	c.li	s0,0
20066474:	498d                	c.li	s3,3
20066476:	8522                	c.mv	a0,s0
20066478:	3fc9                	c.jal	2006644a <plic_get_pending>
2006647a:	892a                	c.mv	s2,a0
2006647c:	8522                	c.mv	a0,s0
2006647e:	dff9f097          	auipc	ra,0xdff9f
20066482:	2a4080e7          	jalr	ra,676(ra) # 5722 <plic_get_enable>
20066486:	c088                	c.sw	a0,0(s1)
20066488:	00a975b3          	and	a1,s2,a0
2006648c:	8522                	c.mv	a0,s0
2006648e:	0405                	c.addi	s0,1
20066490:	dff9f097          	auipc	ra,0xdff9f
20066494:	27a080e7          	jalr	ra,634(ra) # 570a <plic_set_enable>
20066498:	0491                	c.addi	s1,4
2006649a:	fd341ee3          	bne	s0,s3,20066476 <plic_interrupt_handler+0x12>
2006649e:	0ff0000f          	fence	iorw,iorw
200664a2:	dff9f097          	auipc	ra,0xdff9f
200664a6:	334080e7          	jalr	ra,820(ra) # 57d6 <plic_claim_irq>
200664aa:	4592                	c.lwsp	a1,4(sp)
200664ac:	ea018493          	addi	s1,gp,-352 # 20066828 <plic_irq_id>
200664b0:	c088                	c.sw	a0,0(s1)
200664b2:	4501                	c.li	a0,0
200664b4:	dff9f097          	auipc	ra,0xdff9f
200664b8:	256080e7          	jalr	ra,598(ra) # 570a <plic_set_enable>
200664bc:	45a2                	c.lwsp	a1,8(sp)
200664be:	4505                	c.li	a0,1
200664c0:	ea018413          	addi	s0,gp,-352 # 20066828 <plic_irq_id>
200664c4:	dff9f097          	auipc	ra,0xdff9f
200664c8:	246080e7          	jalr	ra,582(ra) # 570a <plic_set_enable>
200664cc:	45b2                	c.lwsp	a1,12(sp)
200664ce:	4509                	c.li	a0,2
200664d0:	dff9f097          	auipc	ra,0xdff9f
200664d4:	23a080e7          	jalr	ra,570(ra) # 570a <plic_set_enable>
200664d8:	4084                	c.lw	s1,0(s1)
200664da:	c4b1                	c.beqz	s1,20066526 <plic_interrupt_handler+0xc2>
200664dc:	02048493          	addi	s1,s1,32
200664e0:	8526                	c.mv	a0,s1
200664e2:	77e000e7          	jalr	ra,1918(zero) # 0 <Reset_Handler>
200664e6:	892a                	c.mv	s2,a0
200664e8:	c531                	c.beqz	a0,20066534 <plic_interrupt_handler+0xd0>
200664ea:	dff9f097          	auipc	ra,0xdff9f
200664ee:	328080e7          	jalr	ra,808(ra) # 5812 <plic_get_threshold>
200664f2:	89aa                	c.mv	s3,a0
200664f4:	4008                	c.lw	a0,0(s0)
200664f6:	dff9f097          	auipc	ra,0xdff9f
200664fa:	2b0080e7          	jalr	ra,688(ra) # 57a6 <plic_get_irq_priority>
200664fe:	dff9f097          	auipc	ra,0xdff9f
20066502:	2fc080e7          	jalr	ra,764(ra) # 57fa <plic_set_threshold>
20066506:	8526                	c.mv	a0,s1
20066508:	7b2000e7          	jalr	ra,1970(zero) # 0 <Reset_Handler>
2006650c:	9902                	c.jalr	s2
2006650e:	854e                	c.mv	a0,s3
20066510:	dff9f097          	auipc	ra,0xdff9f
20066514:	2ea080e7          	jalr	ra,746(ra) # 57fa <plic_set_threshold>
20066518:	4008                	c.lw	a0,0(s0)
2006651a:	dff9f097          	auipc	ra,0xdff9f
2006651e:	2ce080e7          	jalr	ra,718(ra) # 57e8 <plic_complete_irq>
20066522:	00042023          	sw	zero,0(s0)
20066526:	50b2                	c.lwsp	ra,44(sp)
20066528:	5422                	c.lwsp	s0,40(sp)
2006652a:	5492                	c.lwsp	s1,36(sp)
2006652c:	5902                	c.lwsp	s2,32(sp)
2006652e:	49f2                	c.lwsp	s3,28(sp)
20066530:	6145                	c.addi16sp	sp,48
20066532:	8082                	c.jr	ra
20066534:	200626b7          	lui	a3,0x20062
20066538:	200625b7          	lui	a1,0x20062
2006653c:	8726                	c.mv	a4,s1
2006653e:	50068693          	addi	a3,a3,1280 # 20062500 <pmap_func+0xd4>
20066542:	05700613          	addi	a2,zero,87
20066546:	52458593          	addi	a1,a1,1316 # 20062524 <pmap_func+0xf8>
2006654a:	450d                	c.li	a0,3
2006654c:	c43f40ef          	jal	ra,2005b18e <rtk_log_write>
20066550:	b7e1                	c.j	20066518 <plic_interrupt_handler+0xb4>

20066552 <plic_get_active_irq_id>:
20066552:	ea01a503          	lw	a0,-352(gp) # 20066828 <plic_irq_id>
20066556:	8082                	c.jr	ra

20066558 <__NVIC_GetActive_IRQ>:
20066558:	06f00793          	addi	a5,zero,111
2006655c:	00a7d463          	bge	a5,a0,20066564 <__NVIC_GetActive_IRQ+0xc>
20066560:	4501                	c.li	a0,0
20066562:	8082                	c.jr	ra
20066564:	34202773          	csrrs	a4,mcause,zero
20066568:	00171793          	slli	a5,a4,0x1
2006656c:	8385                	c.srli	a5,0x1
2006656e:	00055763          	bge	a0,zero,2006657c <__NVIC_GetActive_IRQ+0x24>
20066572:	0541                	c.addi	a0,16
20066574:	fe0746e3          	blt	a4,zero,20066560 <__NVIC_GetActive_IRQ+0x8>
20066578:	8d1d                	c.sub	a0,a5
2006657a:	a821                	c.j	20066592 <__NVIC_GetActive_IRQ+0x3a>
2006657c:	fe0752e3          	bge	a4,zero,20066560 <__NVIC_GetActive_IRQ+0x8>
20066580:	477d                	c.li	a4,31
20066582:	fea75be3          	bge	a4,a0,20066578 <__NVIC_GetActive_IRQ+0x20>
20066586:	ea01a783          	lw	a5,-352(gp) # 20066828 <plic_irq_id>
2006658a:	02078793          	addi	a5,a5,32
2006658e:	40a78533          	sub	a0,a5,a0
20066592:	00153513          	sltiu	a0,a0,1
20066596:	8082                	c.jr	ra

20066598 <__sram_image2_end__>:
	...

200665a0 <Flash_Layout>:
200665a0:	0000 0000 0000 0800 3fff 0801 0002 0000     .........?......
200665b0:	4000 0801 3fff 081f 0001 0000 0000 0820     .@...?........ .
200665c0:	3fff 0821 0003 0000 4000 0821 3fff 083f     .?!......@!..??.
200665d0:	0004 0000 0000 0840 ffff 086f 0005 0000     ......@...o.....
200665e0:	0000 0870 2fff 0870 0006 0000 3000 0870     ..p../p......0p.
200665f0:	2fff 0872 0008 0000 ffff ffff ffff ffff     ./r.............
20066600:	00ff 0000 ffff ffff ffff ffff               ............

2006660c <PSRAM_CALIB_PATTERN>:
2006660c:	3344 1122 a55a a55a 5aa5 5aa5 2211 4433     D3".Z.Z..Z.Z."3D
2006661c:	6996 9669 9669 6996                         .ii.i..i

20066624 <at_sys_items>:
20066624:	2ef0 2006 9f42 2005 0000 0000 0000 0000     ... B.. ........
20066634:	2efc 2006 9f62 2005 0000 0000 0000 0000     ... b.. ........
20066644:	2f08 2006 a796 2005 0000 0000 0000 0000     ./. ... ........
20066654:	2f14 2006 9f82 2005 0000 0000 0000 0000     ./. ... ........
20066664:	2f1c 2006 aa00 2005 0000 0000 0000 0000     ./. ... ........
20066674:	2f24 2006 9fdc 2005 0000 0000 0000 0000     $/. ... ........
20066684:	2f2c 2006 9ffc 2005 0000 0000 0000 0000     ,/. ... ........
20066694:	2f34 2006 a096 2005 0000 0000 0000 0000     4/. ... ........
200666a4:	2f3c 2006 a13a 2005 0000 0000 0000 0000     </. :.. ........
200666b4:	2f44 2006 a328 2005 0000 0000 0000 0000     D/. (.. ........
200666c4:	2f4c 2006 a3ae 2005 0000 0000 0000 0000     L/. ... ........

200666d4 <impure_data>:
200666d4:	0000 0000 89c4 2006 8a2c 2006 8a94 2006     ....... ,.. ... 
	...

200667e0 <__sdata_start__>:
200667e0:	0000                	c.unimp
200667e2:	42c8                	c.lw	a0,4(a3)

200667e4 <xISRStackTop>:
200667e4:	7f60 2006                                   `.. 

200667e8 <CSWTCH.248>:
200667e8:	5258 5342 0044 0000                         XRBSD...

200667f0 <SystemCoreClock>:
200667f0:	d180 08f0                                   ....

200667f4 <PXID_Idx>:
200667f4:	0101 0101                                   ....

200667f8 <system_can_yield>:
200667f8:	0001 0000                                   ....

200667fc <wakelock>:
200667fc:	0001 0000                                   ....

20066800 <log_init_table>:
20066800:	aa2a 2005                                   *.. 

20066804 <rtk_log_default_level>:
20066804:	0004 0000                                   ....

20066808 <mbedtls_calloc_func>:
20066808:	be5a 2005                                   Z.. 

2006680c <mbedtls_free_func>:
2006680c:	be5e 2005                                   ^.. 

20066810 <pullNextTime>:
20066810:	68d0 2006                                   .h. 

20066814 <uxTopUsedPriority>:
20066814:	000a 0000                                   ....

20066818 <crash_task_info>:
20066818:	ed5c 2005                                   \.. 

2006681c <_impure_ptr>:
2006681c:	66d4 2006                                   .f. 
