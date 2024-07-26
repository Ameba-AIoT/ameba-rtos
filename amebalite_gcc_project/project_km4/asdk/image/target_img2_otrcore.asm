
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
20058a08:	0140206f          	jal	zero,2005aa1c <BOOT_Image1>

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
20058a36:	af878793          	addi	a5,a5,-1288 # 20068af8 <__bss_end__>
20058a3a:	40c78633          	sub	a2,a5,a2
20058a3e:	4581                	c.li	a1,0
20058a40:	03818513          	addi	a0,gp,56 # 200669c0 <ipc_Semaphore>
20058a44:	dffb1097          	auipc	ra,0xdffb1
20058a48:	d46080e7          	jalr	ra,-698(ra) # 978a <_memset>
20058a4c:	1de060ef          	jal	ra,2005ec2a <ChipInfo_GetChipSram>
20058a50:	c919                	c.beqz	a0,20058a66 <app_start+0x5a>
20058a52:	000405b7          	lui	a1,0x40
20058a56:	20080537          	lui	a0,0x20080
20058a5a:	00058593          	addi	a1,a1,0 # 40000 <__ap_sram_heap_size>
20058a5e:	00050513          	addi	a0,a0,0 # 20080000 <__ap_sram_heap_start>
20058a62:	0f2020ef          	jal	ra,2005ab54 <os_heap_add>
20058a66:	473020ef          	jal	ra,2005b6d8 <rtos_mem_init>
20058a6a:	2005c7b7          	lui	a5,0x2005c
20058a6e:	e6878793          	addi	a5,a5,-408 # 2005be68 <freertos_risc_v_trap_handler>
20058a72:	9bf1                	c.andi	a5,-4
20058a74:	30579073          	csrrw	zero,mtvec,a5
20058a78:	2831                	c.jal	20058a94 <SystemCoreClockUpdate>
20058a7a:	dffae097          	auipc	ra,0xdffae
20058a7e:	24c080e7          	jalr	ra,588(ra) # 6cc6 <SYSTIMER_Init>
20058a82:	2db000ef          	jal	ra,2005955c <pinmap_init>
20058a86:	102090ef          	jal	ra,20061b88 <__libc_init_array>
20058a8a:	40b2                	c.lwsp	ra,12(sp)
20058a8c:	0141                	c.addi	sp,16
20058a8e:	0910206f          	jal	zero,2005b31e <main>

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
20058b8c:	ff050513          	addi	a0,a0,-16 # 20061ff0 <strtol+0xe>
20058b90:	505060ef          	jal	ra,2005f894 <io_assert_failed>
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
20058d84:	c74d8793          	addi	a5,s11,-908 # 20064c74 <ipc_tickless_table>
20058d88:	d1c48493          	addi	s1,s1,-740 # 20064d1c <lib_pmc_git_rev>
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
20058db0:	c74d8d93          	addi	s11,s11,-908
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
20058e56:	098b8693          	addi	a3,s7,152 # 20062098 <strtol+0xb6>
20058e5a:	04900613          	addi	a2,zero,73
20058e5e:	8319                	c.srli	a4,0x6
20058e60:	8b0d                	c.andi	a4,3
20058e62:	0e0c0593          	addi	a1,s8,224 # 200620e0 <strtol+0xfe>
20058e66:	4511                	c.li	a0,4
20058e68:	30e020ef          	jal	ra,2005b176 <rtk_log_write>
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
20058edc:	27b0206f          	jal	zero,2005b956 <rtos_sema_give>
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
20058f0c:	0e450513          	addi	a0,a0,228 # 200620e4 <strtol+0x102>
20058f10:	c606                	c.swsp	ra,12(sp)
20058f12:	47c060ef          	jal	ra,2005f38e <DiagPrintf_minimal>
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
20058f50:	17b020ef          	jal	ra,2005b8ca <rtos_sema_create_binary>
20058f54:	0ff97a13          	andi	s4,s2,255
20058f58:	85d2                	c.mv	a1,s4
20058f5a:	8526                	c.mv	a0,s1
20058f5c:	4605                	c.li	a2,1
20058f5e:	2401                	c.jal	2005915e <IPC_INTConfig>
20058f60:	013407b3          	add	a5,s0,s3
20058f64:	4388                	c.lw	a0,0(a5)
20058f66:	55fd                	c.li	a1,-1
20058f68:	1a3020ef          	jal	ra,2005b90a <rtos_sema_take>
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
20058f86:	0fc50513          	addi	a0,a0,252 # 200620fc <strtol+0x11a>
20058f8a:	404060ef          	jal	ra,2005f38e <DiagPrintf_minimal>
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
20058fd6:	11c50513          	addi	a0,a0,284 # 2006211c <strtol+0x13a>
20058fda:	0bb060ef          	jal	ra,2005f894 <io_assert_failed>
20058fde:	479d                	c.li	a5,7
20058fe0:	0157fa63          	bgeu	a5,s5,20058ff4 <ipc_send_message+0x4c>
20058fe4:	20062537          	lui	a0,0x20062
20058fe8:	0ad00593          	addi	a1,zero,173
20058fec:	11c50513          	addi	a0,a0,284 # 2006211c <strtol+0x13a>
20058ff0:	0a5060ef          	jal	ra,2005f894 <io_assert_failed>
20058ff4:	02100793          	addi	a5,zero,33
20058ff8:	0d37eb63          	bltu	a5,s3,200590ce <ipc_send_message+0x126>
20058ffc:	200627b7          	lui	a5,0x20062
20059000:	1f078793          	addi	a5,a5,496 # 200621f0 <CSWTCH.29>
20059004:	97ce                	c.add	a5,s3
20059006:	20062737          	lui	a4,0x20062
2005900a:	0007c583          	lbu	a1,0(a5)
2005900e:	21470713          	addi	a4,a4,532 # 20062214 <CSWTCH.30>
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
2005903a:	1c450513          	addi	a0,a0,452 # 200621c4 <strtol+0x1e2>
2005903e:	350060ef          	jal	ra,2005f38e <DiagPrintf_minimal>
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
20059246:	29c50513          	addi	a0,a0,668 # 2006229c <CSWTCH.30+0x88>
2005924a:	64a060ef          	jal	ra,2005f894 <io_assert_failed>
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
200592bc:	34450513          	addi	a0,a0,836 # 20062344 <CSWTCH.30+0x130>
200592c0:	0ce060ef          	jal	ra,2005f38e <DiagPrintf_minimal>
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
20059340:	39050513          	addi	a0,a0,912 # 20062390 <CSWTCH.30+0x17c>
20059344:	04a060ef          	jal	ra,2005f38e <DiagPrintf_minimal>
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
20059372:	29c50513          	addi	a0,a0,668 # 2006229c <CSWTCH.30+0x88>
20059376:	11b00593          	addi	a1,zero,283
2005937a:	c606                	c.swsp	ra,12(sp)
2005937c:	518060ef          	jal	ra,2005f894 <io_assert_failed>
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
200594fe:	3f850513          	addi	a0,a0,1016 # 200623f8 <CSWTCH.30+0x1e4>
20059502:	68d050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
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
20059526:	3dca0513          	addi	a0,s4,988 # 200623dc <CSWTCH.30+0x1c8>
2005952a:	665050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
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
20059584:	40c48493          	addi	s1,s1,1036 # 2006240c <pmap_func>
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
20059752:	4c8020ef          	jal	ra,2005bc1a <rtos_time_get_current_system_time_ms>
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
200597ac:	50c50513          	addi	a0,a0,1292 # 2006250c <pmap_func+0x100>
200597b0:	0e4060ef          	jal	ra,2005f894 <io_assert_failed>
200597b4:	00091b63          	bne	s2,zero,200597ca <pmu_register_sleep_callback+0x44>
200597b8:	e889                	c.bnez	s1,200597ca <pmu_register_sleep_callback+0x44>
200597ba:	20062537          	lui	a0,0x20062
200597be:	07400593          	addi	a1,zero,116
200597c2:	50c50513          	addi	a0,a0,1292 # 2006250c <pmap_func+0x100>
200597c6:	0ce060ef          	jal	ra,2005f894 <io_assert_failed>
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
20059800:	41a020ef          	jal	ra,2005bc1a <rtos_time_get_current_system_time_ms>
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
2005994c:	5a4c0793          	addi	a5,s8,1444 # 200625a4 <pmap_func+0x198>
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
2005997e:	5b858593          	addi	a1,a1,1464 # 200625b8 <pmap_func+0x1ac>
20059982:	4511                	c.li	a0,4
20059984:	7f2010ef          	jal	ra,2005b176 <rtk_log_write>
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
20059a98:	5b858593          	addi	a1,a1,1464 # 200625b8 <pmap_func+0x1ac>
20059a9c:	4511                	c.li	a0,4
20059a9e:	6d8010ef          	jal	ra,2005b176 <rtk_log_write>
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
20059ad8:	200626b7          	lui	a3,0x20062
20059adc:	200625b7          	lui	a1,0x20062
20059ae0:	88ca                	c.mv	a7,s2
20059ae2:	884e                	c.mv	a6,s3
20059ae4:	87d6                	c.mv	a5,s5
20059ae6:	8726                	c.mv	a4,s1
20059ae8:	7fc68693          	addi	a3,a3,2044 # 200627fc <pmap_func+0x3f0>
20059aec:	04900613          	addi	a2,zero,73
20059af0:	5b858593          	addi	a1,a1,1464 # 200625b8 <pmap_func+0x1ac>
20059af4:	4511                	c.li	a0,4
20059af6:	680010ef          	jal	ra,2005b176 <rtk_log_write>
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
20059bd4:	7d868693          	addi	a3,a3,2008 # 200627d8 <pmap_func+0x3cc>
20059bd8:	c03e                	c.swsp	a5,0(sp)
20059bda:	57e2                	c.lwsp	a5,56(sp)
20059bdc:	04900613          	addi	a2,zero,73
20059be0:	5b858593          	addi	a1,a1,1464 # 200625b8 <pmap_func+0x1ac>
20059be4:	4511                	c.li	a0,4
20059be6:	d67e                	c.swsp	t6,44(sp)
20059be8:	58e010ef          	jal	ra,2005b176 <rtk_log_write>
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
20059c66:	04000613          	addi	a2,zero,64
20059c6a:	43818413          	addi	s0,gp,1080 # 20066dc0 <wifi_user_config>
20059c6e:	4581                	c.li	a1,0
20059c70:	43818513          	addi	a0,gp,1080 # 20066dc0 <wifi_user_config>
20059c74:	c606                	c.swsp	ra,12(sp)
20059c76:	dffb0097          	auipc	ra,0xdffb0
20059c7a:	e58080e7          	jalr	ra,-424(ra) # 9ace <__wrap_memset>
20059c7e:	50800793          	addi	a5,zero,1288
20059c82:	00f41f23          	sh	a5,30(s0)
20059c86:	4799                	c.li	a5,6
20059c88:	d01c                	c.sw	a5,32(s0)
20059c8a:	47a1                	c.li	a5,8
20059c8c:	d05c                	c.sw	a5,36(s0)
20059c8e:	4785                	c.li	a5,1
20059c90:	00f405a3          	sb	a5,11(s0)
20059c94:	00f41623          	sh	a5,12(s0)
20059c98:	00f41423          	sh	a5,8(s0)
20059c9c:	0b0107b7          	lui	a5,0xb010
20059ca0:	10178793          	addi	a5,a5,257 # b010101 <__ap_sram_heap_size+0xafd0101>
20059ca4:	c81c                	c.sw	a5,16(s0)
20059ca6:	4789                	c.li	a5,2
20059ca8:	cc1c                	c.sw	a5,24(s0)
20059caa:	00020737          	lui	a4,0x20
20059cae:	10100793          	addi	a5,zero,257
20059cb2:	02f41723          	sh	a5,46(s0)
20059cb6:	02f41823          	sh	a5,48(s0)
20059cba:	20070713          	addi	a4,a4,512 # 20200 <__bdram_heap_buffer_size__+0x8d00>
20059cbe:	4791                	c.li	a5,4
20059cc0:	c018                	c.sw	a4,0(s0)
20059cc2:	02f40923          	sb	a5,50(s0)
20059cc6:	02f40a23          	sb	a5,52(s0)
20059cca:	050f0737          	lui	a4,0x50f0
20059cce:	6785                	c.lui	a5,0x1
20059cd0:	c058                	c.sw	a4,4(s0)
20059cd2:	38878793          	addi	a5,a5,904 # 1388 <CPU_ClkGet+0x872>
20059cd6:	4751                	c.li	a4,20
20059cd8:	40b2                	c.lwsp	ra,12(sp)
20059cda:	00040ea3          	sb	zero,29(s0)
20059cde:	02042423          	sw	zero,40(s0)
20059ce2:	00040a23          	sb	zero,20(s0)
20059ce6:	00041b23          	sh	zero,22(s0)
20059cea:	020406a3          	sb	zero,45(s0)
20059cee:	02e40ea3          	sb	a4,61(s0)
20059cf2:	dc1c                	c.sw	a5,56(s0)
20059cf4:	4422                	c.lwsp	s0,8(sp)
20059cf6:	0141                	c.addi	sp,16
20059cf8:	8082                	c.jr	ra

20059cfa <hash_index>:
20059cfa:	4781                	c.li	a5,0
20059cfc:	08300693          	addi	a3,zero,131
20059d00:	00054703          	lbu	a4,0(a0) # 150000 <__ap_sram_heap_size+0x110000>
20059d04:	e709                	c.bnez	a4,20059d0e <hash_index+0x14>
20059d06:	00179513          	slli	a0,a5,0x1
20059d0a:	8105                	c.srli	a0,0x1
20059d0c:	8082                	c.jr	ra
20059d0e:	02d787b3          	mul	a5,a5,a3
20059d12:	0505                	c.addi	a0,1
20059d14:	97ba                	c.add	a5,a4
20059d16:	b7ed                	c.j	20059d00 <hash_index+0x6>

20059d18 <log_add_new_command>:
20059d18:	1141                	c.addi	sp,-16
20059d1a:	c422                	c.swsp	s0,8(sp)
20059d1c:	c606                	c.swsp	ra,12(sp)
20059d1e:	842a                	c.mv	s0,a0
20059d20:	4108                	c.lw	a0,0(a0)
20059d22:	3fe1                	c.jal	20059cfa <hash_index>
20059d24:	02000793          	addi	a5,zero,32
20059d28:	02f56533          	rem	a0,a0,a5
20059d2c:	47818793          	addi	a5,gp,1144 # 20066e00 <log_hash>
20059d30:	00840713          	addi	a4,s0,8
20059d34:	050e                	c.slli	a0,0x3
20059d36:	97aa                	c.add	a5,a0
20059d38:	4394                	c.lw	a3,0(a5)
20059d3a:	c2d8                	c.sw	a4,4(a3)
20059d3c:	c414                	c.sw	a3,8(s0)
20059d3e:	c45c                	c.sw	a5,12(s0)
20059d40:	c398                	c.sw	a4,0(a5)
20059d42:	40b2                	c.lwsp	ra,12(sp)
20059d44:	4422                	c.lwsp	s0,8(sp)
20059d46:	0141                	c.addi	sp,16
20059d48:	8082                	c.jr	ra

20059d4a <atcmd_service_init>:
20059d4a:	47818793          	addi	a5,gp,1144 # 20066e00 <log_hash>
20059d4e:	10078713          	addi	a4,a5,256
20059d52:	c39c                	c.sw	a5,0(a5)
20059d54:	c3dc                	c.sw	a5,4(a5)
20059d56:	07a1                	c.addi	a5,8
20059d58:	fee79de3          	bne	a5,a4,20059d52 <atcmd_service_init+0x8>
20059d5c:	e781a783          	lw	a5,-392(gp) # 20066800 <log_init_table>
20059d60:	8782                	c.jr	a5

20059d62 <atcmd_service_add_table>:
20059d62:	1141                	c.addi	sp,-16
20059d64:	c422                	c.swsp	s0,8(sp)
20059d66:	c226                	c.swsp	s1,4(sp)
20059d68:	c04a                	c.swsp	s2,0(sp)
20059d6a:	c606                	c.swsp	ra,12(sp)
20059d6c:	892a                	c.mv	s2,a0
20059d6e:	84ae                	c.mv	s1,a1
20059d70:	4401                	c.li	s0,0
20059d72:	00944863          	blt	s0,s1,20059d82 <atcmd_service_add_table+0x20>
20059d76:	40b2                	c.lwsp	ra,12(sp)
20059d78:	4422                	c.lwsp	s0,8(sp)
20059d7a:	4492                	c.lwsp	s1,4(sp)
20059d7c:	4902                	c.lwsp	s2,0(sp)
20059d7e:	0141                	c.addi	sp,16
20059d80:	8082                	c.jr	ra
20059d82:	00441513          	slli	a0,s0,0x4
20059d86:	954a                	c.add	a0,s2
20059d88:	3f41                	c.jal	20059d18 <log_add_new_command>
20059d8a:	0405                	c.addi	s0,1
20059d8c:	b7dd                	c.j	20059d72 <atcmd_service_add_table+0x10>

20059d8e <atcmd_action>:
20059d8e:	1141                	c.addi	sp,-16
20059d90:	c422                	c.swsp	s0,8(sp)
20059d92:	c226                	c.swsp	s1,4(sp)
20059d94:	c04a                	c.swsp	s2,0(sp)
20059d96:	c606                	c.swsp	ra,12(sp)
20059d98:	892a                	c.mv	s2,a0
20059d9a:	02000413          	addi	s0,zero,32
20059d9e:	3fb1                	c.jal	20059cfa <hash_index>
20059da0:	02856433          	rem	s0,a0,s0
20059da4:	00341793          	slli	a5,s0,0x3
20059da8:	47818413          	addi	s0,gp,1144 # 20066e00 <log_hash>
20059dac:	943e                	c.add	s0,a5
20059dae:	4004                	c.lw	s1,0(s0)
20059db0:	00849463          	bne	s1,s0,20059db8 <atcmd_action+0x2a>
20059db4:	4501                	c.li	a0,0
20059db6:	a819                	c.j	20059dcc <atcmd_action+0x3e>
20059db8:	ff84a503          	lw	a0,-8(s1)
20059dbc:	85ca                	c.mv	a1,s2
20059dbe:	dffb0097          	auipc	ra,0xdffb0
20059dc2:	d1c080e7          	jalr	ra,-740(ra) # 9ada <__wrap_strcmp>
20059dc6:	e909                	c.bnez	a0,20059dd8 <atcmd_action+0x4a>
20059dc8:	ffc4a503          	lw	a0,-4(s1)
20059dcc:	40b2                	c.lwsp	ra,12(sp)
20059dce:	4422                	c.lwsp	s0,8(sp)
20059dd0:	4492                	c.lwsp	s1,4(sp)
20059dd2:	4902                	c.lwsp	s2,0(sp)
20059dd4:	0141                	c.addi	sp,16
20059dd6:	8082                	c.jr	ra
20059dd8:	4084                	c.lw	s1,0(s1)
20059dda:	bfd9                	c.j	20059db0 <atcmd_action+0x22>

20059ddc <atcmd_handler>:
20059ddc:	7139                	c.addi16sp	sp,-64
20059dde:	dc22                	c.swsp	s0,56(sp)
20059de0:	c42a                	c.swsp	a0,8(sp)
20059de2:	842a                	c.mv	s0,a0
20059de4:	4675                	c.li	a2,29
20059de6:	4581                	c.li	a1,0
20059de8:	0808                	c.addi4spn	a0,sp,16
20059dea:	de06                	c.swsp	ra,60(sp)
20059dec:	da26                	c.swsp	s1,52(sp)
20059dee:	c602                	c.swsp	zero,12(sp)
20059df0:	dffb0097          	auipc	ra,0xdffb0
20059df4:	cde080e7          	jalr	ra,-802(ra) # 9ace <__wrap_memset>
20059df8:	200635b7          	lui	a1,0x20063
20059dfc:	4609                	c.li	a2,2
20059dfe:	83458593          	addi	a1,a1,-1996 # 20062834 <pmap_func+0x428>
20059e02:	8522                	c.mv	a0,s0
20059e04:	759070ef          	jal	ra,20061d5c <strncmp>
20059e08:	c519                	c.beqz	a0,20059e16 <atcmd_handler+0x3a>
20059e0a:	4501                	c.li	a0,0
20059e0c:	50f2                	c.lwsp	ra,60(sp)
20059e0e:	5462                	c.lwsp	s0,56(sp)
20059e10:	54d2                	c.lwsp	s1,52(sp)
20059e12:	6121                	c.addi16sp	sp,64
20059e14:	8082                	c.jr	ra
20059e16:	200635b7          	lui	a1,0x20063
20059e1a:	83858593          	addi	a1,a1,-1992 # 20062838 <pmap_func+0x42c>
20059e1e:	0028                	c.addi4spn	a0,sp,8
20059e20:	012080ef          	jal	ra,20061e32 <strsep>
20059e24:	200635b7          	lui	a1,0x20063
20059e28:	842a                	c.mv	s0,a0
20059e2a:	87c58593          	addi	a1,a1,-1924 # 2006287c <pmap_func+0x470>
20059e2e:	0028                	c.addi4spn	a0,sp,8
20059e30:	002080ef          	jal	ra,20061e32 <strsep>
20059e34:	84aa                	c.mv	s1,a0
20059e36:	d871                	c.beqz	s0,20059e0a <atcmd_handler+0x2e>
20059e38:	8522                	c.mv	a0,s0
20059e3a:	dffb0097          	auipc	ra,0xdffb0
20059e3e:	c9e080e7          	jalr	ra,-866(ra) # 9ad8 <__wrap_strlen>
20059e42:	478d                	c.li	a5,3
20059e44:	fca7f3e3          	bgeu	a5,a0,20059e0a <atcmd_handler+0x2e>
20059e48:	85a2                	c.mv	a1,s0
20059e4a:	02000613          	addi	a2,zero,32
20059e4e:	0068                	c.addi4spn	a0,sp,12
20059e50:	779070ef          	jal	ra,20061dc8 <strncpy>
20059e54:	00e10513          	addi	a0,sp,14
20059e58:	3f1d                	c.jal	20059d8e <atcmd_action>
20059e5a:	842a                	c.mv	s0,a0
20059e5c:	4501                	c.li	a0,0
20059e5e:	d45d                	c.beqz	s0,20059e0c <atcmd_handler+0x30>
20059e60:	8526                	c.mv	a0,s1
20059e62:	9402                	c.jalr	s0
20059e64:	8522                	c.mv	a0,s0
20059e66:	b75d                	c.j	20059e0c <atcmd_handler+0x30>

20059e68 <parse_param>:
20059e68:	7179                	c.addi16sp	sp,-48
20059e6a:	d422                	c.swsp	s0,40(sp)
20059e6c:	d606                	c.swsp	ra,44(sp)
20059e6e:	d226                	c.swsp	s1,36(sp)
20059e70:	d04a                	c.swsp	s2,32(sp)
20059e72:	ce4e                	c.swsp	s3,28(sp)
20059e74:	4405                	c.li	s0,1
20059e76:	c505                	c.beqz	a0,20059e9e <parse_param+0x36>
20059e78:	84ae                	c.mv	s1,a1
20059e7a:	c62a                	c.swsp	a0,12(sp)
20059e7c:	4405                	c.li	s0,1
20059e7e:	200639b7          	lui	s3,0x20063
20059e82:	495d                	c.li	s2,23
20059e84:	83c98593          	addi	a1,s3,-1988 # 2006283c <pmap_func+0x430>
20059e88:	0068                	c.addi4spn	a0,sp,12
20059e8a:	7a9070ef          	jal	ra,20061e32 <strsep>
20059e8e:	00241793          	slli	a5,s0,0x2
20059e92:	97a6                	c.add	a5,s1
20059e94:	c388                	c.sw	a0,0(a5)
20059e96:	c501                	c.beqz	a0,20059e9e <parse_param+0x36>
20059e98:	0405                	c.addi	s0,1
20059e9a:	ff2415e3          	bne	s0,s2,20059e84 <parse_param+0x1c>
20059e9e:	50b2                	c.lwsp	ra,44(sp)
20059ea0:	8522                	c.mv	a0,s0
20059ea2:	5422                	c.lwsp	s0,40(sp)
20059ea4:	5492                	c.lwsp	s1,36(sp)
20059ea6:	5902                	c.lwsp	s2,32(sp)
20059ea8:	49f2                	c.lwsp	s3,28(sp)
20059eaa:	6145                	c.addi16sp	sp,48
20059eac:	8082                	c.jr	ra

20059eae <mp_command_handler>:
20059eae:	200635b7          	lui	a1,0x20063
20059eb2:	1141                	c.addi	sp,-16
20059eb4:	461d                	c.li	a2,7
20059eb6:	84058593          	addi	a1,a1,-1984 # 20062840 <pmap_func+0x434>
20059eba:	c422                	c.swsp	s0,8(sp)
20059ebc:	c606                	c.swsp	ra,12(sp)
20059ebe:	842a                	c.mv	s0,a0
20059ec0:	69d070ef          	jal	ra,20061d5c <strncmp>
20059ec4:	e115                	c.bnez	a0,20059ee8 <mp_command_handler+0x3a>
20059ec6:	041d                	c.addi	s0,7
20059ec8:	8522                	c.mv	a0,s0
20059eca:	dffb0097          	auipc	ra,0xdffb0
20059ece:	c0e080e7          	jalr	ra,-1010(ra) # 9ad8 <__wrap_strlen>
20059ed2:	00150593          	addi	a1,a0,1
20059ed6:	4605                	c.li	a2,1
20059ed8:	8522                	c.mv	a0,s0
20059eda:	379060ef          	jal	ra,20060a52 <inic_mp_command>
20059ede:	4505                	c.li	a0,1
20059ee0:	40b2                	c.lwsp	ra,12(sp)
20059ee2:	4422                	c.lwsp	s0,8(sp)
20059ee4:	0141                	c.addi	sp,16
20059ee6:	8082                	c.jr	ra
20059ee8:	4501                	c.li	a0,0
20059eea:	bfdd                	c.j	20059ee0 <mp_command_handler+0x32>

20059eec <atcmd_service>:
20059eec:	1141                	c.addi	sp,-16
20059eee:	c606                	c.swsp	ra,12(sp)
20059ef0:	35f5                	c.jal	20059ddc <atcmd_handler>
20059ef2:	c505                	c.beqz	a0,20059f1a <atcmd_service+0x2e>
20059ef4:	01b010ef          	jal	ra,2005b70e <rtos_mem_get_free_heap_size>
20059ef8:	85aa                	c.mv	a1,a0
20059efa:	20063537          	lui	a0,0x20063
20059efe:	84850513          	addi	a0,a0,-1976 # 20062848 <pmap_func+0x43c>
20059f02:	48c050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
20059f06:	20063537          	lui	a0,0x20063
20059f0a:	87450513          	addi	a0,a0,-1932 # 20062874 <pmap_func+0x468>
20059f0e:	480050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
20059f12:	4505                	c.li	a0,1
20059f14:	40b2                	c.lwsp	ra,12(sp)
20059f16:	0141                	c.addi	sp,16
20059f18:	8082                	c.jr	ra
20059f1a:	4501                	c.li	a0,0
20059f1c:	bfe5                	c.j	20059f14 <atcmd_service+0x28>

20059f1e <at_otaclear>:
20059f1e:	1141                	c.addi	sp,-16
20059f20:	c606                	c.swsp	ra,12(sp)
20059f22:	2c0030ef          	jal	ra,2005d1e2 <sys_clear_ota_signature>
20059f26:	40b2                	c.lwsp	ra,12(sp)
20059f28:	200635b7          	lui	a1,0x20063
20059f2c:	20063537          	lui	a0,0x20063
20059f30:	88058593          	addi	a1,a1,-1920 # 20062880 <pmap_func+0x474>
20059f34:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
20059f38:	0141                	c.addi	sp,16
20059f3a:	4540506f          	jal	zero,2005f38e <DiagPrintf_minimal>

20059f3e <at_otarecover>:
20059f3e:	1141                	c.addi	sp,-16
20059f40:	c606                	c.swsp	ra,12(sp)
20059f42:	3c2030ef          	jal	ra,2005d304 <sys_recover_ota_signature>
20059f46:	40b2                	c.lwsp	ra,12(sp)
20059f48:	200635b7          	lui	a1,0x20063
20059f4c:	20063537          	lui	a0,0x20063
20059f50:	89858593          	addi	a1,a1,-1896 # 20062898 <pmap_func+0x48c>
20059f54:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
20059f58:	0141                	c.addi	sp,16
20059f5a:	4340506f          	jal	zero,2005f38e <DiagPrintf_minimal>

20059f5e <at_test>:
20059f5e:	c521                	c.beqz	a0,20059fa6 <at_test+0x48>
20059f60:	1141                	c.addi	sp,-16
20059f62:	c606                	c.swsp	ra,12(sp)
20059f64:	c422                	c.swsp	s0,8(sp)
20059f66:	842a                	c.mv	s0,a0
20059f68:	dffb0097          	auipc	ra,0xdffb0
20059f6c:	b70080e7          	jalr	ra,-1168(ra) # 9ad8 <__wrap_strlen>
20059f70:	85aa                	c.mv	a1,a0
20059f72:	20063537          	lui	a0,0x20063
20059f76:	8a850513          	addi	a0,a0,-1880 # 200628a8 <pmap_func+0x49c>
20059f7a:	414050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
20059f7e:	20063537          	lui	a0,0x20063
20059f82:	85a2                	c.mv	a1,s0
20059f84:	8bc50513          	addi	a0,a0,-1860 # 200628bc <pmap_func+0x4b0>
20059f88:	406050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
20059f8c:	4422                	c.lwsp	s0,8(sp)
20059f8e:	200635b7          	lui	a1,0x20063
20059f92:	20063537          	lui	a0,0x20063
20059f96:	40b2                	c.lwsp	ra,12(sp)
20059f98:	8cc58593          	addi	a1,a1,-1844 # 200628cc <pmap_func+0x4c0>
20059f9c:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
20059fa0:	0141                	c.addi	sp,16
20059fa2:	3ec0506f          	jal	zero,2005f38e <DiagPrintf_minimal>
20059fa6:	200635b7          	lui	a1,0x20063
20059faa:	20063537          	lui	a0,0x20063
20059fae:	8cc58593          	addi	a1,a1,-1844 # 200628cc <pmap_func+0x4c0>
20059fb2:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
20059fb6:	b7f5                	c.j	20059fa2 <at_test+0x44>

20059fb8 <at_rst>:
20059fb8:	200635b7          	lui	a1,0x20063
20059fbc:	20063537          	lui	a0,0x20063
20059fc0:	1141                	c.addi	sp,-16
20059fc2:	8d458593          	addi	a1,a1,-1836 # 200628d4 <pmap_func+0x4c8>
20059fc6:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
20059fca:	c606                	c.swsp	ra,12(sp)
20059fcc:	3c2050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
20059fd0:	40b2                	c.lwsp	ra,12(sp)
20059fd2:	0141                	c.addi	sp,16
20059fd4:	46c0306f          	jal	zero,2005d440 <sys_reset>

20059fd8 <at_state>:
20059fd8:	bf010113          	addi	sp,sp,-1040
20059fdc:	850a                	c.mv	a0,sp
20059fde:	40112623          	sw	ra,1036(sp)
20059fe2:	40812423          	sw	s0,1032(sp)
20059fe6:	3a6040ef          	jal	ra,2005e38c <vTaskList>
20059fea:	20063537          	lui	a0,0x20063
20059fee:	858a                	c.mv	a1,sp
20059ff0:	8dc50513          	addi	a0,a0,-1828 # 200628dc <pmap_func+0x4d0>
20059ff4:	39a050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
20059ff8:	850a                	c.mv	a0,sp
20059ffa:	5cf000ef          	jal	ra,2005adc8 <vPortGetHeapStats>
20059ffe:	20063537          	lui	a0,0x20063
2005a002:	8f050513          	addi	a0,a0,-1808 # 200628f0 <pmap_func+0x4e4>
2005a006:	388050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a00a:	ec81a583          	lw	a1,-312(gp) # 20066850 <total_heap_size>
2005a00e:	20063537          	lui	a0,0x20063
2005a012:	90050513          	addi	a0,a0,-1792 # 20062900 <pmap_func+0x4f4>
2005a016:	378050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a01a:	4582                	c.lwsp	a1,0(sp)
2005a01c:	20063537          	lui	a0,0x20063
2005a020:	91450513          	addi	a0,a0,-1772 # 20062914 <pmap_func+0x508>
2005a024:	36a050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a028:	4782                	c.lwsp	a5,0(sp)
2005a02a:	ec81a583          	lw	a1,-312(gp) # 20066850 <total_heap_size>
2005a02e:	20063537          	lui	a0,0x20063
2005a032:	92850513          	addi	a0,a0,-1752 # 20062928 <pmap_func+0x51c>
2005a036:	8d9d                	c.sub	a1,a5
2005a038:	356050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a03c:	47c2                	c.lwsp	a5,16(sp)
2005a03e:	ec81a583          	lw	a1,-312(gp) # 20066850 <total_heap_size>
2005a042:	20063537          	lui	a0,0x20063
2005a046:	93c50513          	addi	a0,a0,-1732 # 2006293c <pmap_func+0x530>
2005a04a:	8d9d                	c.sub	a1,a5
2005a04c:	342050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a050:	200635b7          	lui	a1,0x20063
2005a054:	20063537          	lui	a0,0x20063
2005a058:	95058593          	addi	a1,a1,-1712 # 20062950 <pmap_func+0x544>
2005a05c:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
2005a060:	32e050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a064:	40c12083          	lw	ra,1036(sp)
2005a068:	40812403          	lw	s0,1032(sp)
2005a06c:	41010113          	addi	sp,sp,1040
2005a070:	8082                	c.jr	ra

2005a072 <at_gmr>:
2005a072:	715d                	c.addi16sp	sp,-80
2005a074:	40000513          	addi	a0,zero,1024
2005a078:	c686                	c.swsp	ra,76(sp)
2005a07a:	c4a2                	c.swsp	s0,72(sp)
2005a07c:	c2a6                	c.swsp	s1,68(sp)
2005a07e:	65e010ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005a082:	200636b7          	lui	a3,0x20063
2005a086:	200635b7          	lui	a1,0x20063
2005a08a:	4805                	c.li	a6,1
2005a08c:	4781                	c.li	a5,0
2005a08e:	4705                	c.li	a4,1
2005a090:	95868693          	addi	a3,a3,-1704 # 20062958 <pmap_func+0x54c>
2005a094:	842a                	c.mv	s0,a0
2005a096:	04900613          	addi	a2,zero,73
2005a09a:	ee458593          	addi	a1,a1,-284 # 20062ee4 <pmap_func+0xad8>
2005a09e:	4511                	c.li	a0,4
2005a0a0:	0d6010ef          	jal	ra,2005b176 <rtk_log_write>
2005a0a4:	8522                	c.mv	a0,s0
2005a0a6:	3ff00593          	addi	a1,zero,1023
2005a0aa:	1c9040ef          	jal	ra,2005ea72 <ChipInfo_GetSocName_ToBuf>
2005a0ae:	200634b7          	lui	s1,0x20063
2005a0b2:	85a2                	c.mv	a1,s0
2005a0b4:	97c48513          	addi	a0,s1,-1668 # 2006297c <pmap_func+0x570>
2005a0b8:	2d6050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a0bc:	8522                	c.mv	a0,s0
2005a0be:	3ff00593          	addi	a1,zero,1023
2005a0c2:	26d040ef          	jal	ra,2005eb2e <ChipInfo_GetLibVersion_ToBuf>
2005a0c6:	85a2                	c.mv	a1,s0
2005a0c8:	97c48513          	addi	a0,s1,-1668
2005a0cc:	2c2050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a0d0:	8522                	c.mv	a0,s0
2005a0d2:	634010ef          	jal	ra,2005b706 <rtos_mem_free>
2005a0d6:	200635b7          	lui	a1,0x20063
2005a0da:	02000613          	addi	a2,zero,32
2005a0de:	98058593          	addi	a1,a1,-1664 # 20062980 <pmap_func+0x574>
2005a0e2:	850a                	c.mv	a0,sp
2005a0e4:	4e5070ef          	jal	ra,20061dc8 <strncpy>
2005a0e8:	200635b7          	lui	a1,0x20063
2005a0ec:	02000613          	addi	a2,zero,32
2005a0f0:	98858593          	addi	a1,a1,-1656 # 20062988 <pmap_func+0x57c>
2005a0f4:	1008                	c.addi4spn	a0,sp,32
2005a0f6:	4d3070ef          	jal	ra,20061dc8 <strncpy>
2005a0fa:	20063737          	lui	a4,0x20063
2005a0fe:	200635b7          	lui	a1,0x20063
2005a102:	20063537          	lui	a0,0x20063
2005a106:	1014                	c.addi4spn	a3,sp,32
2005a108:	860a                	c.mv	a2,sp
2005a10a:	99070713          	addi	a4,a4,-1648 # 20062990 <pmap_func+0x584>
2005a10e:	9a458593          	addi	a1,a1,-1628 # 200629a4 <pmap_func+0x598>
2005a112:	9ac50513          	addi	a0,a0,-1620 # 200629ac <pmap_func+0x5a0>
2005a116:	278050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a11a:	40b6                	c.lwsp	ra,76(sp)
2005a11c:	4426                	c.lwsp	s0,72(sp)
2005a11e:	4496                	c.lwsp	s1,68(sp)
2005a120:	6161                	c.addi16sp	sp,80
2005a122:	8082                	c.jr	ra

2005a124 <at_log>:
2005a124:	7159                	c.addi16sp	sp,-112
2005a126:	d2a6                	c.swsp	s1,100(sp)
2005a128:	05c00613          	addi	a2,zero,92
2005a12c:	84aa                	c.mv	s1,a0
2005a12e:	4581                	c.li	a1,0
2005a130:	0048                	c.addi4spn	a0,sp,4
2005a132:	d4a2                	c.swsp	s0,104(sp)
2005a134:	d0ca                	c.swsp	s2,96(sp)
2005a136:	d686                	c.swsp	ra,108(sp)
2005a138:	20063437          	lui	s0,0x20063
2005a13c:	dffb0097          	auipc	ra,0xdffb0
2005a140:	992080e7          	jalr	ra,-1646(ra) # 9ace <__wrap_memset>
2005a144:	20063937          	lui	s2,0x20063
2005a148:	ec91                	c.bnez	s1,2005a164 <at_log+0x40>
2005a14a:	200636b7          	lui	a3,0x20063
2005a14e:	9bc68693          	addi	a3,a3,-1604 # 200629bc <pmap_func+0x5b0>
2005a152:	04100613          	addi	a2,zero,65
2005a156:	9d040593          	addi	a1,s0,-1584 # 200629d0 <pmap_func+0x5c4>
2005a15a:	4505                	c.li	a0,1
2005a15c:	01a010ef          	jal	ra,2005b176 <rtk_log_write>
2005a160:	4605                	c.li	a2,1
2005a162:	aa35                	c.j	2005a29e <at_log+0x17a>
2005a164:	8526                	c.mv	a0,s1
2005a166:	004c                	c.addi4spn	a1,sp,4
2005a168:	3301                	c.jal	20059e68 <parse_param>
2005a16a:	4785                	c.li	a5,1
2005a16c:	84aa                	c.mv	s1,a0
2005a16e:	1aa7d063          	bge	a5,a0,2005a30e <at_log+0x1ea>
2005a172:	4522                	c.lwsp	a0,8(sp)
2005a174:	18050d63          	beq	a0,zero,2005a30e <at_log+0x1ea>
2005a178:	1ff070ef          	jal	ra,20061b76 <atoi>
2005a17c:	0ff57713          	andi	a4,a0,255
2005a180:	4789                	c.li	a5,2
2005a182:	16f70563          	beq	a4,a5,2005a2ec <at_log+0x1c8>
2005a186:	00e7ef63          	bltu	a5,a4,2005a1a4 <at_log+0x80>
2005a18a:	e771                	c.bnez	a4,2005a256 <at_log+0x132>
2005a18c:	478d                	c.li	a5,3
2005a18e:	00f49663          	bne	s1,a5,2005a19a <at_log+0x76>
2005a192:	4532                	c.lwsp	a0,12(sp)
2005a194:	00054783          	lbu	a5,0(a0)
2005a198:	e78d                	c.bnez	a5,2005a1c2 <at_log+0x9e>
2005a19a:	200636b7          	lui	a3,0x20063
2005a19e:	9d468693          	addi	a3,a3,-1580 # 200629d4 <pmap_func+0x5c8>
2005a1a2:	bf45                	c.j	2005a152 <at_log+0x2e>
2005a1a4:	468d                	c.li	a3,3
2005a1a6:	14d70f63          	beq	a4,a3,2005a304 <at_log+0x1e0>
2005a1aa:	200636b7          	lui	a3,0x20063
2005a1ae:	a6868693          	addi	a3,a3,-1432 # 20062a68 <pmap_func+0x65c>
2005a1b2:	04100613          	addi	a2,zero,65
2005a1b6:	9d040593          	addi	a1,s0,-1584
2005a1ba:	4505                	c.li	a0,1
2005a1bc:	7bb000ef          	jal	ra,2005b176 <rtk_log_write>
2005a1c0:	b745                	c.j	2005a160 <at_log+0x3c>
2005a1c2:	579000ef          	jal	ra,2005af3a <rtk_log_level_get>
2005a1c6:	45b2                	c.lwsp	a1,12(sp)
2005a1c8:	862a                	c.mv	a2,a0
2005a1ca:	20063537          	lui	a0,0x20063
2005a1ce:	9f450513          	addi	a0,a0,-1548 # 200629f4 <pmap_func+0x5e8>
2005a1d2:	1bc050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a1d6:	200636b7          	lui	a3,0x20063
2005a1da:	a8468693          	addi	a3,a3,-1404 # 20062a84 <pmap_func+0x678>
2005a1de:	04100613          	addi	a2,zero,65
2005a1e2:	9d040593          	addi	a1,s0,-1584
2005a1e6:	4505                	c.li	a0,1
2005a1e8:	78f000ef          	jal	ra,2005b176 <rtk_log_write>
2005a1ec:	200636b7          	lui	a3,0x20063
2005a1f0:	aa068693          	addi	a3,a3,-1376 # 20062aa0 <pmap_func+0x694>
2005a1f4:	04500613          	addi	a2,zero,69
2005a1f8:	9d040593          	addi	a1,s0,-1584
2005a1fc:	4509                	c.li	a0,2
2005a1fe:	779000ef          	jal	ra,2005b176 <rtk_log_write>
2005a202:	200636b7          	lui	a3,0x20063
2005a206:	abc68693          	addi	a3,a3,-1348 # 20062abc <pmap_func+0x6b0>
2005a20a:	05700613          	addi	a2,zero,87
2005a20e:	9d040593          	addi	a1,s0,-1584
2005a212:	450d                	c.li	a0,3
2005a214:	763000ef          	jal	ra,2005b176 <rtk_log_write>
2005a218:	200636b7          	lui	a3,0x20063
2005a21c:	9d040593          	addi	a1,s0,-1584
2005a220:	ad868693          	addi	a3,a3,-1320 # 20062ad8 <pmap_func+0x6cc>
2005a224:	04900613          	addi	a2,zero,73
2005a228:	4511                	c.li	a0,4
2005a22a:	74d000ef          	jal	ra,2005b176 <rtk_log_write>
2005a22e:	20063537          	lui	a0,0x20063
2005a232:	af050513          	addi	a0,a0,-1296 # 20062af0 <pmap_func+0x6e4>
2005a236:	158050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a23a:	20063537          	lui	a0,0x20063
2005a23e:	b0890593          	addi	a1,s2,-1272 # 20062b08 <pmap_func+0x6fc>
2005a242:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
2005a246:	148050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a24a:	50b6                	c.lwsp	ra,108(sp)
2005a24c:	5426                	c.lwsp	s0,104(sp)
2005a24e:	5496                	c.lwsp	s1,100(sp)
2005a250:	5906                	c.lwsp	s2,96(sp)
2005a252:	6165                	c.addi16sp	sp,112
2005a254:	8082                	c.jr	ra
2005a256:	4791                	c.li	a5,4
2005a258:	00f49a63          	bne	s1,a5,2005a26c <at_log+0x148>
2005a25c:	47b2                	c.lwsp	a5,12(sp)
2005a25e:	0007c783          	lbu	a5,0(a5)
2005a262:	c789                	c.beqz	a5,2005a26c <at_log+0x148>
2005a264:	4542                	c.lwsp	a0,16(sp)
2005a266:	00054783          	lbu	a5,0(a0)
2005a26a:	e791                	c.bnez	a5,2005a276 <at_log+0x152>
2005a26c:	200636b7          	lui	a3,0x20063
2005a270:	a0868693          	addi	a3,a3,-1528 # 20062a08 <pmap_func+0x5fc>
2005a274:	bdf9                	c.j	2005a152 <at_log+0x2e>
2005a276:	101070ef          	jal	ra,20061b76 <atoi>
2005a27a:	0ff57593          	andi	a1,a0,255
2005a27e:	4532                	c.lwsp	a0,12(sp)
2005a280:	537000ef          	jal	ra,2005afb6 <rtk_log_level_set>
2005a284:	d929                	c.beqz	a0,2005a1d6 <at_log+0xb2>
2005a286:	200636b7          	lui	a3,0x20063
2005a28a:	04100613          	addi	a2,zero,65
2005a28e:	a2868693          	addi	a3,a3,-1496 # 20062a28 <pmap_func+0x61c>
2005a292:	9d040593          	addi	a1,s0,-1584
2005a296:	4505                	c.li	a0,1
2005a298:	6df000ef          	jal	ra,2005b176 <rtk_log_write>
2005a29c:	4609                	c.li	a2,2
2005a29e:	20063537          	lui	a0,0x20063
2005a2a2:	b0890593          	addi	a1,s2,-1272
2005a2a6:	b1050513          	addi	a0,a0,-1264 # 20062b10 <pmap_func+0x704>
2005a2aa:	0e4050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a2ae:	20063537          	lui	a0,0x20063
2005a2b2:	8b850513          	addi	a0,a0,-1864 # 200628b8 <pmap_func+0x4ac>
2005a2b6:	0d8050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a2ba:	20063537          	lui	a0,0x20063
2005a2be:	b2050513          	addi	a0,a0,-1248 # 20062b20 <pmap_func+0x714>
2005a2c2:	0cc050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a2c6:	20063537          	lui	a0,0x20063
2005a2ca:	b4c50513          	addi	a0,a0,-1204 # 20062b4c <pmap_func+0x740>
2005a2ce:	0c0050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a2d2:	20063537          	lui	a0,0x20063
2005a2d6:	b8450513          	addi	a0,a0,-1148 # 20062b84 <pmap_func+0x778>
2005a2da:	0b4050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a2de:	20063537          	lui	a0,0x20063
2005a2e2:	bb850513          	addi	a0,a0,-1096 # 20062bb8 <pmap_func+0x7ac>
2005a2e6:	0a8050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a2ea:	b785                	c.j	2005a24a <at_log+0x126>
2005a2ec:	00e48763          	beq	s1,a4,2005a2fa <at_log+0x1d6>
2005a2f0:	200636b7          	lui	a3,0x20063
2005a2f4:	a4468693          	addi	a3,a3,-1468 # 20062a44 <pmap_func+0x638>
2005a2f8:	bda9                	c.j	2005a152 <at_log+0x2e>
2005a2fa:	5a818513          	addi	a0,gp,1448 # 20066f30 <rtk_log_tag_array>
2005a2fe:	3c9000ef          	jal	ra,2005aec6 <rtk_log_array_print>
2005a302:	bdd1                	c.j	2005a1d6 <at_log+0xb2>
2005a304:	fef496e3          	bne	s1,a5,2005a2f0 <at_log+0x1cc>
2005a308:	413000ef          	jal	ra,2005af1a <rtk_log_array_clear>
2005a30c:	b5e9                	c.j	2005a1d6 <at_log+0xb2>
2005a30e:	4711                	c.li	a4,4
2005a310:	bd69                	c.j	2005a1aa <at_log+0x86>

2005a312 <at_rreg>:
2005a312:	7159                	c.addi16sp	sp,-112
2005a314:	d4a2                	c.swsp	s0,104(sp)
2005a316:	05c00613          	addi	a2,zero,92
2005a31a:	842a                	c.mv	s0,a0
2005a31c:	4581                	c.li	a1,0
2005a31e:	0048                	c.addi4spn	a0,sp,4
2005a320:	d2a6                	c.swsp	s1,100(sp)
2005a322:	d686                	c.swsp	ra,108(sp)
2005a324:	200634b7          	lui	s1,0x20063
2005a328:	dffaf097          	auipc	ra,0xdffaf
2005a32c:	7a6080e7          	jalr	ra,1958(ra) # 9ace <__wrap_memset>
2005a330:	ec05                	c.bnez	s0,2005a368 <at_rreg+0x56>
2005a332:	200636b7          	lui	a3,0x20063
2005a336:	200635b7          	lui	a1,0x20063
2005a33a:	05700613          	addi	a2,zero,87
2005a33e:	9d058593          	addi	a1,a1,-1584 # 200629d0 <pmap_func+0x5c4>
2005a342:	450d                	c.li	a0,3
2005a344:	bec68693          	addi	a3,a3,-1044 # 20062bec <pmap_func+0x7e0>
2005a348:	62f000ef          	jal	ra,2005b176 <rtk_log_write>
2005a34c:	20063537          	lui	a0,0x20063
2005a350:	4605                	c.li	a2,1
2005a352:	c0848593          	addi	a1,s1,-1016 # 20062c08 <pmap_func+0x7fc>
2005a356:	b1050513          	addi	a0,a0,-1264 # 20062b10 <pmap_func+0x704>
2005a35a:	034050ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a35e:	50b6                	c.lwsp	ra,108(sp)
2005a360:	5426                	c.lwsp	s0,104(sp)
2005a362:	5496                	c.lwsp	s1,100(sp)
2005a364:	6165                	c.addi16sp	sp,112
2005a366:	8082                	c.jr	ra
2005a368:	004c                	c.addi4spn	a1,sp,4
2005a36a:	8522                	c.mv	a0,s0
2005a36c:	afdff0ef          	jal	ra,20059e68 <parse_param>
2005a370:	ffe50713          	addi	a4,a0,-2
2005a374:	4789                	c.li	a5,2
2005a376:	fae7eee3          	bltu	a5,a4,2005a332 <at_rreg+0x20>
2005a37a:	157d                	c.addi	a0,-1
2005a37c:	0542                	c.slli	a0,0x10
2005a37e:	002c                	c.addi4spn	a1,sp,8
2005a380:	8141                	c.srli	a0,0x10
2005a382:	178010ef          	jal	ra,2005b4fa <cmd_dump_word>
2005a386:	20063537          	lui	a0,0x20063
2005a38a:	c0848593          	addi	a1,s1,-1016
2005a38e:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
2005a392:	7fd040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a396:	b7e1                	c.j	2005a35e <at_rreg+0x4c>

2005a398 <at_wreg>:
2005a398:	7159                	c.addi16sp	sp,-112
2005a39a:	d4a2                	c.swsp	s0,104(sp)
2005a39c:	05c00613          	addi	a2,zero,92
2005a3a0:	842a                	c.mv	s0,a0
2005a3a2:	4581                	c.li	a1,0
2005a3a4:	0048                	c.addi4spn	a0,sp,4
2005a3a6:	d2a6                	c.swsp	s1,100(sp)
2005a3a8:	d686                	c.swsp	ra,108(sp)
2005a3aa:	200634b7          	lui	s1,0x20063
2005a3ae:	dffaf097          	auipc	ra,0xdffaf
2005a3b2:	720080e7          	jalr	ra,1824(ra) # 9ace <__wrap_memset>
2005a3b6:	ec05                	c.bnez	s0,2005a3ee <at_wreg+0x56>
2005a3b8:	200636b7          	lui	a3,0x20063
2005a3bc:	200635b7          	lui	a1,0x20063
2005a3c0:	05700613          	addi	a2,zero,87
2005a3c4:	9d058593          	addi	a1,a1,-1584 # 200629d0 <pmap_func+0x5c4>
2005a3c8:	450d                	c.li	a0,3
2005a3ca:	c1068693          	addi	a3,a3,-1008 # 20062c10 <pmap_func+0x804>
2005a3ce:	5a9000ef          	jal	ra,2005b176 <rtk_log_write>
2005a3d2:	20063537          	lui	a0,0x20063
2005a3d6:	4605                	c.li	a2,1
2005a3d8:	c2c48593          	addi	a1,s1,-980 # 20062c2c <pmap_func+0x820>
2005a3dc:	b1050513          	addi	a0,a0,-1264 # 20062b10 <pmap_func+0x704>
2005a3e0:	7af040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a3e4:	50b6                	c.lwsp	ra,108(sp)
2005a3e6:	5426                	c.lwsp	s0,104(sp)
2005a3e8:	5496                	c.lwsp	s1,100(sp)
2005a3ea:	6165                	c.addi16sp	sp,112
2005a3ec:	8082                	c.jr	ra
2005a3ee:	004c                	c.addi4spn	a1,sp,4
2005a3f0:	8522                	c.mv	a0,s0
2005a3f2:	a77ff0ef          	jal	ra,20059e68 <parse_param>
2005a3f6:	478d                	c.li	a5,3
2005a3f8:	fcf510e3          	bne	a0,a5,2005a3b8 <at_wreg+0x20>
2005a3fc:	45a2                	c.lwsp	a1,8(sp)
2005a3fe:	4509                	c.li	a0,2
2005a400:	054010ef          	jal	ra,2005b454 <cmd_write_word>
2005a404:	20063537          	lui	a0,0x20063
2005a408:	c2c48593          	addi	a1,s1,-980
2005a40c:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
2005a410:	77f040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a414:	bfc1                	c.j	2005a3e4 <at_wreg+0x4c>

2005a416 <find_status>:
2005a416:	872a                	c.mv	a4,a0
2005a418:	4781                	c.li	a5,0
2005a41a:	00c7c463          	blt	a5,a2,2005a422 <find_status+0xc>
2005a41e:	4501                	c.li	a0,0
2005a420:	8082                	c.jr	ra
2005a422:	852e                	c.mv	a0,a1
2005a424:	4594                	c.lw	a3,8(a1)
2005a426:	02458593          	addi	a1,a1,36
2005a42a:	fee68be3          	beq	a3,a4,2005a420 <find_status+0xa>
2005a42e:	0785                	c.addi	a5,1
2005a430:	b7ed                	c.j	2005a41a <find_status+0x4>

2005a432 <find_free_delta>:
2005a432:	87aa                	c.mv	a5,a0
2005a434:	4701                	c.li	a4,0
2005a436:	00b74463          	blt	a4,a1,2005a43e <find_free_delta+0xc>
2005a43a:	4501                	c.li	a0,0
2005a43c:	8082                	c.jr	ra
2005a43e:	853e                	c.mv	a0,a5
2005a440:	5794                	c.lw	a3,40(a5)
2005a442:	03878793          	addi	a5,a5,56
2005a446:	dafd                	c.beqz	a3,2005a43c <find_free_delta+0xa>
2005a448:	0705                	c.addi	a4,1
2005a44a:	b7f5                	c.j	2005a436 <find_free_delta+0x4>

2005a44c <update_status>:
2005a44c:	7179                	c.addi16sp	sp,-48
2005a44e:	d226                	c.swsp	s1,36(sp)
2005a450:	ebc1a783          	lw	a5,-324(gp) # 20066844 <task_status>
2005a454:	6705                	c.lui	a4,0x1
2005a456:	d606                	c.swsp	ra,44(sp)
2005a458:	d422                	c.swsp	s0,40(sp)
2005a45a:	d04a                	c.swsp	s2,32(sp)
2005a45c:	ce4e                	c.swsp	s3,28(sp)
2005a45e:	cc52                	c.swsp	s4,24(sp)
2005a460:	ca56                	c.swsp	s5,20(sp)
2005a462:	c85a                	c.swsp	s6,16(sp)
2005a464:	c65e                	c.swsp	s7,12(sp)
2005a466:	c462                	c.swsp	s8,8(sp)
2005a468:	00e786b3          	add	a3,a5,a4
2005a46c:	2006a603          	lw	a2,512(a3)
2005a470:	ebc18493          	addi	s1,gp,-324 # 20066844 <task_status>
2005a474:	ea19                	c.bnez	a2,2005a48a <update_status+0x3e>
2005a476:	2046a603          	lw	a2,516(a3)
2005a47a:	ea01                	c.bnez	a2,2005a48a <update_status+0x3e>
2005a47c:	90070713          	addi	a4,a4,-1792 # 900 <BKUP_Set+0x5a>
2005a480:	973e                	c.add	a4,a5
2005a482:	20f6a023          	sw	a5,512(a3)
2005a486:	20e6a223          	sw	a4,516(a3)
2005a48a:	6405                	c.lui	s0,0x1
2005a48c:	97a2                	c.add	a5,s0
2005a48e:	20078793          	addi	a5,a5,512
2005a492:	47d8                	c.lw	a4,12(a5)
2005a494:	43d4                	c.lw	a3,4(a5)
2005a496:	21040913          	addi	s2,s0,528 # 1210 <CPU_ClkGet+0x6fa>
2005a49a:	c798                	c.sw	a4,8(a5)
2005a49c:	4398                	c.lw	a4,0(a5)
2005a49e:	c394                	c.sw	a3,0(a5)
2005a4a0:	4a01                	c.li	s4,0
2005a4a2:	c3d8                	c.sw	a4,4(a5)
2005a4a4:	24a030ef          	jal	ra,2005d6ee <uxTaskGetNumberOfTasks>
2005a4a8:	409c                	c.lw	a5,0(s1)
2005a4aa:	85aa                	c.mv	a1,a0
2005a4ac:	4601                	c.li	a2,0
2005a4ae:	97a2                	c.add	a5,s0
2005a4b0:	20a7a623          	sw	a0,524(a5)
2005a4b4:	2047a503          	lw	a0,516(a5)
2005a4b8:	595030ef          	jal	ra,2005e24c <uxTaskGetSystemState>
2005a4bc:	409c                	c.lw	a5,0(s1)
2005a4be:	e0040613          	addi	a2,s0,-512
2005a4c2:	4581                	c.li	a1,0
2005a4c4:	00878733          	add	a4,a5,s0
2005a4c8:	20a72623          	sw	a0,524(a4)
2005a4cc:	01278533          	add	a0,a5,s2
2005a4d0:	dffaf097          	auipc	ra,0xdffaf
2005a4d4:	5fe080e7          	jalr	ra,1534(ra) # 9ace <__wrap_memset>
2005a4d8:	0004aa83          	lw	s5,0(s1)
2005a4dc:	9456                	c.add	s0,s5
2005a4de:	20842783          	lw	a5,520(s0)
2005a4e2:	00f05963          	bge	zero,a5,2005a4f4 <update_status+0xa8>
2005a4e6:	02400c13          	addi	s8,zero,36
2005a4ea:	9aca                	c.add	s5,s2
2005a4ec:	20c42783          	lw	a5,524(s0)
2005a4f0:	00fa4e63          	blt	s4,a5,2005a50c <update_status+0xc0>
2005a4f4:	50b2                	c.lwsp	ra,44(sp)
2005a4f6:	5422                	c.lwsp	s0,40(sp)
2005a4f8:	5492                	c.lwsp	s1,36(sp)
2005a4fa:	5902                	c.lwsp	s2,32(sp)
2005a4fc:	49f2                	c.lwsp	s3,28(sp)
2005a4fe:	4a62                	c.lwsp	s4,24(sp)
2005a500:	4ad2                	c.lwsp	s5,20(sp)
2005a502:	4b42                	c.lwsp	s6,16(sp)
2005a504:	4bb2                	c.lwsp	s7,12(sp)
2005a506:	4c22                	c.lwsp	s8,8(sp)
2005a508:	6145                	c.addi16sp	sp,48
2005a50a:	8082                	c.jr	ra
2005a50c:	038a0933          	mul	s2,s4,s8
2005a510:	20442b83          	lw	s7,516(s0)
2005a514:	20842603          	lw	a2,520(s0)
2005a518:	20042583          	lw	a1,512(s0)
2005a51c:	9bca                	c.add	s7,s2
2005a51e:	008ba503          	lw	a0,8(s7)
2005a522:	3dd5                	c.jal	2005a416 <find_status>
2005a524:	04000593          	addi	a1,zero,64
2005a528:	8b2a                	c.mv	s6,a0
2005a52a:	8556                	c.mv	a0,s5
2005a52c:	3719                	c.jal	2005a432 <find_free_delta>
2005a52e:	000ba783          	lw	a5,0(s7)
2005a532:	84aa                	c.mv	s1,a0
2005a534:	467d                	c.li	a2,31
2005a536:	d51c                	c.sw	a5,40(a0)
2005a538:	20442783          	lw	a5,516(s0)
2005a53c:	97ca                	c.add	a5,s2
2005a53e:	43cc                	c.lw	a1,4(a5)
2005a540:	c14c                	c.sw	a1,4(a0)
2005a542:	20442783          	lw	a5,516(s0)
2005a546:	97ca                	c.add	a5,s2
2005a548:	479c                	c.lw	a5,8(a5)
2005a54a:	00052023          	sw	zero,0(a0)
2005a54e:	0521                	c.addi	a0,8
2005a550:	d15c                	c.sw	a5,36(a0)
2005a552:	20442783          	lw	a5,516(s0)
2005a556:	97ca                	c.add	a5,s2
2005a558:	00c7c783          	lbu	a5,12(a5)
2005a55c:	02f50623          	sb	a5,44(a0)
2005a560:	20442783          	lw	a5,516(s0)
2005a564:	97ca                	c.add	a5,s2
2005a566:	4b9c                	c.lw	a5,16(a5)
2005a568:	d51c                	c.sw	a5,40(a0)
2005a56a:	05f070ef          	jal	ra,20061dc8 <strncpy>
2005a56e:	c095                	c.beqz	s1,2005a592 <update_status+0x146>
2005a570:	20442783          	lw	a5,516(s0)
2005a574:	97ca                	c.add	a5,s2
2005a576:	4f9c                	c.lw	a5,24(a5)
2005a578:	000b0563          	beq	s6,zero,2005a582 <update_status+0x136>
2005a57c:	018b2703          	lw	a4,24(s6)
2005a580:	8f99                	c.sub	a5,a4
2005a582:	c09c                	c.sw	a5,0(s1)
2005a584:	20442783          	lw	a5,516(s0)
2005a588:	993e                	c.add	s2,a5
2005a58a:	00c94783          	lbu	a5,12(s2)
2005a58e:	02f48a23          	sb	a5,52(s1)
2005a592:	0a05                	c.addi	s4,1
2005a594:	bfa1                	c.j	2005a4ec <update_status+0xa0>

2005a596 <print_delta>:
2005a596:	715d                	c.addi16sp	sp,-80
2005a598:	736277b7          	lui	a5,0x73627
2005a59c:	c4a2                	c.swsp	s0,72(sp)
2005a59e:	25278793          	addi	a5,a5,594 # 73627252 <__kr4_psram_text_start__+0x134a7232>
2005a5a2:	842a                	c.mv	s0,a0
2005a5a4:	20063737          	lui	a4,0x20063
2005a5a8:	200636b7          	lui	a3,0x20063
2005a5ac:	20063637          	lui	a2,0x20063
2005a5b0:	200635b7          	lui	a1,0x20063
2005a5b4:	20063537          	lui	a0,0x20063
2005a5b8:	c43e                	c.swsp	a5,8(sp)
2005a5ba:	c3470713          	addi	a4,a4,-972 # 20062c34 <pmap_func+0x828>
2005a5be:	04400793          	addi	a5,zero,68
2005a5c2:	c3c68693          	addi	a3,a3,-964 # 20062c3c <pmap_func+0x830>
2005a5c6:	c4460613          	addi	a2,a2,-956 # 20062c44 <pmap_func+0x838>
2005a5ca:	c4c58593          	addi	a1,a1,-948 # 20062c4c <pmap_func+0x840>
2005a5ce:	c5850513          	addi	a0,a0,-936 # 20062c58 <pmap_func+0x84c>
2005a5d2:	c2a6                	c.swsp	s1,68(sp)
2005a5d4:	c0ca                	c.swsp	s2,64(sp)
2005a5d6:	de4e                	c.swsp	s3,60(sp)
2005a5d8:	dc52                	c.swsp	s4,56(sp)
2005a5da:	da56                	c.swsp	s5,52(sp)
2005a5dc:	d85a                	c.swsp	s6,48(sp)
2005a5de:	d65e                	c.swsp	s7,44(sp)
2005a5e0:	ee22                	c.fswsp	fs0,28(sp)
2005a5e2:	00f10623          	sb	a5,12(sp)
2005a5e6:	c686                	c.swsp	ra,76(sp)
2005a5e8:	00011223          	sh	zero,4(sp)
2005a5ec:	00010323          	sb	zero,6(sp)
2005a5f0:	59f040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a5f4:	e581a407          	flw	fs0,-424(gp) # 200667e0 <__sdata_start__>
2005a5f8:	6485                	c.lui	s1,0x1
2005a5fa:	4b81                	c.li	s7,0
2005a5fc:	03800993          	addi	s3,zero,56
2005a600:	06400b13          	addi	s6,zero,100
2005a604:	21848a13          	addi	s4,s1,536 # 1218 <CPU_ClkGet+0x702>
2005a608:	20063ab7          	lui	s5,0x20063
2005a60c:	460d                	c.li	a2,3
2005a60e:	4581                	c.li	a1,0
2005a610:	0048                	c.addi4spn	a0,sp,4
2005a612:	dffaf097          	auipc	ra,0xdffaf
2005a616:	4bc080e7          	jalr	ra,1212(ra) # 9ace <__wrap_memset>
2005a61a:	033b8633          	mul	a2,s7,s3
2005a61e:	ebc1a803          	lw	a6,-324(gp) # 20066844 <task_status>
2005a622:	d0047753          	fcvt.s.w	fa4,s0
2005a626:	4529                	c.li	a0,10
2005a628:	00c806b3          	add	a3,a6,a2
2005a62c:	96a6                	c.add	a3,s1
2005a62e:	2106a783          	lw	a5,528(a3)
2005a632:	d007f7d3          	fcvt.s.w	fa5,a5
2005a636:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a63a:	18e7f7d3          	fdiv.s	fa5,fa5,fa4
2005a63e:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a642:	c0079753          	fcvt.w.s	a4,fa5,rtz
2005a646:	036767b3          	rem	a5,a4,s6
2005a64a:	02a787b3          	mul	a5,a5,a0
2005a64e:	0367c5b3          	div	a1,a5,s6
2005a652:	0367e7b3          	rem	a5,a5,s6
2005a656:	03058593          	addi	a1,a1,48
2005a65a:	00b10223          	sb	a1,4(sp)
2005a65e:	02a787b3          	mul	a5,a5,a0
2005a662:	0367c7b3          	div	a5,a5,s6
2005a666:	03078793          	addi	a5,a5,48
2005a66a:	00f102a3          	sb	a5,5(sp)
2005a66e:	2386a783          	lw	a5,568(a3)
2005a672:	c78d                	c.beqz	a5,2005a69c <print_delta+0x106>
2005a674:	03674733          	div	a4,a4,s6
2005a678:	2446c503          	lbu	a0,580(a3)
2005a67c:	014605b3          	add	a1,a2,s4
2005a680:	2406a683          	lw	a3,576(a3)
2005a684:	01050793          	addi	a5,a0,16
2005a688:	00278533          	add	a0,a5,sp
2005a68c:	ff854603          	lbu	a2,-8(a0)
2005a690:	005c                	c.addi4spn	a5,sp,4
2005a692:	95c2                	c.add	a1,a6
2005a694:	c68a8513          	addi	a0,s5,-920 # 20062c68 <pmap_func+0x85c>
2005a698:	4f7040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a69c:	0b85                	c.addi	s7,1
2005a69e:	04000793          	addi	a5,zero,64
2005a6a2:	f6fb95e3          	bne	s7,a5,2005a60c <print_delta+0x76>
2005a6a6:	40b6                	c.lwsp	ra,76(sp)
2005a6a8:	4426                	c.lwsp	s0,72(sp)
2005a6aa:	4496                	c.lwsp	s1,68(sp)
2005a6ac:	4906                	c.lwsp	s2,64(sp)
2005a6ae:	59f2                	c.lwsp	s3,60(sp)
2005a6b0:	5a62                	c.lwsp	s4,56(sp)
2005a6b2:	5ad2                	c.lwsp	s5,52(sp)
2005a6b4:	5b42                	c.lwsp	s6,48(sp)
2005a6b6:	5bb2                	c.lwsp	s7,44(sp)
2005a6b8:	6472                	c.flwsp	fs0,28(sp)
2005a6ba:	6161                	c.addi16sp	sp,80
2005a6bc:	8082                	c.jr	ra

2005a6be <cpu_stat_thread>:
2005a6be:	7179                	c.addi16sp	sp,-48
2005a6c0:	c62a                	c.swsp	a0,12(sp)
2005a6c2:	4521                	c.li	a0,8
2005a6c4:	d226                	c.swsp	s1,36(sp)
2005a6c6:	d606                	c.swsp	ra,44(sp)
2005a6c8:	d422                	c.swsp	s0,40(sp)
2005a6ca:	d04a                	c.swsp	s2,32(sp)
2005a6cc:	ce4e                	c.swsp	s3,28(sp)
2005a6ce:	cc52                	c.swsp	s4,24(sp)
2005a6d0:	ca56                	c.swsp	s5,20(sp)
2005a6d2:	00a010ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005a6d6:	45b2                	c.lwsp	a1,12(sp)
2005a6d8:	ec018493          	addi	s1,gp,-320 # 20066848 <top_exit_sema>
2005a6dc:	e131                	c.bnez	a0,2005a720 <cpu_stat_thread+0x62>
2005a6de:	200635b7          	lui	a1,0x20063
2005a6e2:	20063537          	lui	a0,0x20063
2005a6e6:	f3458593          	addi	a1,a1,-204 # 20062f34 <__FUNCTION__.0>
2005a6ea:	c7c50513          	addi	a0,a0,-900 # 20062c7c <pmap_func+0x870>
2005a6ee:	4a1040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a6f2:	4088                	c.lw	a0,0(s1)
2005a6f4:	ebc18413          	addi	s0,gp,-324 # 20066844 <task_status>
2005a6f8:	1fc010ef          	jal	ra,2005b8f4 <rtos_sema_delete>
2005a6fc:	4008                	c.lw	a0,0(s0)
2005a6fe:	0004a023          	sw	zero,0(s1)
2005a702:	004010ef          	jal	ra,2005b706 <rtos_mem_free>
2005a706:	00042023          	sw	zero,0(s0)
2005a70a:	5422                	c.lwsp	s0,40(sp)
2005a70c:	50b2                	c.lwsp	ra,44(sp)
2005a70e:	5492                	c.lwsp	s1,36(sp)
2005a710:	5902                	c.lwsp	s2,32(sp)
2005a712:	49f2                	c.lwsp	s3,28(sp)
2005a714:	4a62                	c.lwsp	s4,24(sp)
2005a716:	4ad2                	c.lwsp	s5,20(sp)
2005a718:	4501                	c.li	a0,0
2005a71a:	6145                	c.addi16sp	sp,48
2005a71c:	4b20106f          	jal	zero,2005bbce <rtos_task_delete>
2005a720:	4621                	c.li	a2,8
2005a722:	842a                	c.mv	s0,a0
2005a724:	dffaf097          	auipc	ra,0xdffaf
2005a728:	3ac080e7          	jalr	ra,940(ra) # 9ad0 <__wrap_memcpy>
2005a72c:	410187b7          	lui	a5,0x41018
2005a730:	4bdc                	c.lw	a5,20(a5)
2005a732:	3e800a93          	addi	s5,zero,1000
2005a736:	eaf1a823          	sw	a5,-336(gp) # 20066838 <last_tick>
2005a73a:	5a7d                	c.li	s4,-1
2005a73c:	eb018913          	addi	s2,gp,-336 # 20066838 <last_tick>
2005a740:	410189b7          	lui	s3,0x41018
2005a744:	400c                	c.lw	a1,0(s0)
2005a746:	4088                	c.lw	a0,0(s1)
2005a748:	02ba85b3          	mul	a1,s5,a1
2005a74c:	1be010ef          	jal	ra,2005b90a <rtos_sema_take>
2005a750:	03451463          	bne	a0,s4,2005a778 <cpu_stat_thread+0xba>
2005a754:	39e5                	c.jal	2005a44c <update_status>
2005a756:	0149a503          	lw	a0,20(s3) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a75a:	00092783          	lw	a5,0(s2)
2005a75e:	0149a703          	lw	a4,20(s3)
2005a762:	8d1d                	c.sub	a0,a5
2005a764:	00e92023          	sw	a4,0(s2)
2005a768:	353d                	c.jal	2005a596 <print_delta>
2005a76a:	405c                	c.lw	a5,4(s0)
2005a76c:	00f05563          	bge	zero,a5,2005a776 <cpu_stat_thread+0xb8>
2005a770:	17fd                	c.addi	a5,-1
2005a772:	c05c                	c.sw	a5,4(s0)
2005a774:	bfc1                	c.j	2005a744 <cpu_stat_thread+0x86>
2005a776:	f7f9                	c.bnez	a5,2005a744 <cpu_stat_thread+0x86>
2005a778:	8522                	c.mv	a0,s0
2005a77a:	78d000ef          	jal	ra,2005b706 <rtos_mem_free>
2005a77e:	bf95                	c.j	2005a6f2 <cpu_stat_thread+0x34>

2005a780 <at_cpuload>:
2005a780:	7119                	c.addi16sp	sp,-128
2005a782:	dca2                	c.swsp	s0,120(sp)
2005a784:	05c00613          	addi	a2,zero,92
2005a788:	842a                	c.mv	s0,a0
2005a78a:	4581                	c.li	a1,0
2005a78c:	0048                	c.addi4spn	a0,sp,4
2005a78e:	de86                	c.swsp	ra,124(sp)
2005a790:	d8ca                	c.swsp	s2,112(sp)
2005a792:	d4d2                	c.swsp	s4,104(sp)
2005a794:	daa6                	c.swsp	s1,116(sp)
2005a796:	d6ce                	c.swsp	s3,108(sp)
2005a798:	d2d6                	c.swsp	s5,100(sp)
2005a79a:	dffaf097          	auipc	ra,0xdffaf
2005a79e:	334080e7          	jalr	ra,820(ra) # 9ace <__wrap_memset>
2005a7a2:	eb418793          	addi	a5,gp,-332 # 2006683c <para_in>
2005a7a6:	4705                	c.li	a4,1
2005a7a8:	c398                	c.sw	a4,0(a5)
2005a7aa:	004c                	c.addi4spn	a1,sp,4
2005a7ac:	577d                	c.li	a4,-1
2005a7ae:	8522                	c.mv	a0,s0
2005a7b0:	c3d8                	c.sw	a4,4(a5)
2005a7b2:	eb6ff0ef          	jal	ra,20059e68 <parse_param>
2005a7b6:	ffe50713          	addi	a4,a0,-2
2005a7ba:	4789                	c.li	a5,2
2005a7bc:	20063a37          	lui	s4,0x20063
2005a7c0:	06e7f463          	bgeu	a5,a4,2005a828 <at_cpuload+0xa8>
2005a7c4:	20063537          	lui	a0,0x20063
2005a7c8:	c9050513          	addi	a0,a0,-880 # 20062c90 <pmap_func+0x884>
2005a7cc:	3c3040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a7d0:	4405                	c.li	s0,1
2005a7d2:	20063537          	lui	a0,0x20063
2005a7d6:	8622                	c.mv	a2,s0
2005a7d8:	e1ca0593          	addi	a1,s4,-484 # 20062e1c <pmap_func+0xa10>
2005a7dc:	b1050513          	addi	a0,a0,-1264 # 20062b10 <pmap_func+0x704>
2005a7e0:	9875                	c.andi	s0,-3
2005a7e2:	3ad040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a7e6:	14040b63          	beq	s0,zero,2005a93c <at_cpuload+0x1bc>
2005a7ea:	20063537          	lui	a0,0x20063
2005a7ee:	8b850513          	addi	a0,a0,-1864 # 200628b8 <pmap_func+0x4ac>
2005a7f2:	39d040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a7f6:	20063537          	lui	a0,0x20063
2005a7fa:	e2850513          	addi	a0,a0,-472 # 20062e28 <pmap_func+0xa1c>
2005a7fe:	391040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a802:	20063537          	lui	a0,0x20063
2005a806:	e5850513          	addi	a0,a0,-424 # 20062e58 <pmap_func+0xa4c>
2005a80a:	385040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a80e:	20063537          	lui	a0,0x20063
2005a812:	e6c50513          	addi	a0,a0,-404 # 20062e6c <pmap_func+0xa60>
2005a816:	379040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a81a:	20063537          	lui	a0,0x20063
2005a81e:	e8850513          	addi	a0,a0,-376 # 20062e88 <pmap_func+0xa7c>
2005a822:	36d040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a826:	aa19                	c.j	2005a93c <at_cpuload+0x1bc>
2005a828:	84aa                	c.mv	s1,a0
2005a82a:	4522                	c.lwsp	a0,8(sp)
2005a82c:	eb418993          	addi	s3,gp,-332 # 2006683c <para_in>
2005a830:	4411                	c.li	s0,4
2005a832:	00054783          	lbu	a5,0(a0)
2005a836:	c789                	c.beqz	a5,2005a840 <at_cpuload+0xc0>
2005a838:	33e070ef          	jal	ra,20061b76 <atoi>
2005a83c:	0ff57413          	andi	s0,a0,255
2005a840:	4789                	c.li	a5,2
2005a842:	04f48463          	beq	s1,a5,2005a88a <at_cpuload+0x10a>
2005a846:	4532                	c.lwsp	a0,12(sp)
2005a848:	00054783          	lbu	a5,0(a0)
2005a84c:	cf81                	c.beqz	a5,2005a864 <at_cpuload+0xe4>
2005a84e:	328070ef          	jal	ra,20061b76 <atoi>
2005a852:	4785                	c.li	a5,1
2005a854:	00a05663          	bge	zero,a0,2005a860 <at_cpuload+0xe0>
2005a858:	4532                	c.lwsp	a0,12(sp)
2005a85a:	31c070ef          	jal	ra,20061b76 <atoi>
2005a85e:	87aa                	c.mv	a5,a0
2005a860:	00f9a023          	sw	a5,0(s3)
2005a864:	4791                	c.li	a5,4
2005a866:	02f49263          	bne	s1,a5,2005a88a <at_cpuload+0x10a>
2005a86a:	4542                	c.lwsp	a0,16(sp)
2005a86c:	00054783          	lbu	a5,0(a0)
2005a870:	cf89                	c.beqz	a5,2005a88a <at_cpuload+0x10a>
2005a872:	304070ef          	jal	ra,20061b76 <atoi>
2005a876:	57fd                	c.li	a5,-1
2005a878:	00a05763          	bge	zero,a0,2005a886 <at_cpuload+0x106>
2005a87c:	4542                	c.lwsp	a0,16(sp)
2005a87e:	2f8070ef          	jal	ra,20061b76 <atoi>
2005a882:	fff50793          	addi	a5,a0,-1
2005a886:	00f9a223          	sw	a5,4(s3)
2005a88a:	ebc18a93          	addi	s5,gp,-324 # 20066844 <task_status>
2005a88e:	000aa783          	lw	a5,0(s5)
2005a892:	ebc18493          	addi	s1,gp,-324 # 20066844 <task_status>
2005a896:	e79d                	c.bnez	a5,2005a8c4 <at_cpuload+0x144>
2005a898:	6509                	c.lui	a0,0x2
2005a89a:	0541                	c.addi	a0,16
2005a89c:	641000ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005a8a0:	00aaa023          	sw	a0,0(s5)
2005a8a4:	e105                	c.bnez	a0,2005a8c4 <at_cpuload+0x144>
2005a8a6:	200636b7          	lui	a3,0x20063
2005a8aa:	200635b7          	lui	a1,0x20063
2005a8ae:	dac68693          	addi	a3,a3,-596 # 20062dac <pmap_func+0x9a0>
2005a8b2:	04900613          	addi	a2,zero,73
2005a8b6:	9d058593          	addi	a1,a1,-1584 # 200629d0 <pmap_func+0x5c4>
2005a8ba:	4511                	c.li	a0,4
2005a8bc:	0bb000ef          	jal	ra,2005b176 <rtk_log_write>
2005a8c0:	4409                	c.li	s0,2
2005a8c2:	bf01                	c.j	2005a7d2 <at_cpuload+0x52>
2005a8c4:	0049a683          	lw	a3,4(s3)
2005a8c8:	0009a603          	lw	a2,0(s3)
2005a8cc:	20063537          	lui	a0,0x20063
2005a8d0:	85a2                	c.mv	a1,s0
2005a8d2:	dd050513          	addi	a0,a0,-560 # 20062dd0 <pmap_func+0x9c4>
2005a8d6:	2b9040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a8da:	4789                	c.li	a5,2
2005a8dc:	08f40363          	beq	s0,a5,2005a962 <at_cpuload+0x1e2>
2005a8e0:	0687e763          	bltu	a5,s0,2005a94e <at_cpuload+0x1ce>
2005a8e4:	ec018793          	addi	a5,gp,-320 # 20066848 <top_exit_sema>
2005a8e8:	e825                	c.bnez	s0,2005a958 <at_cpuload+0x1d8>
2005a8ea:	439c                	c.lw	a5,0(a5)
2005a8ec:	e3a1                	c.bnez	a5,2005a92c <at_cpuload+0x1ac>
2005a8ee:	4088                	c.lw	a0,0(s1)
2005a8f0:	6609                	c.lui	a2,0x2
2005a8f2:	0641                	c.addi	a2,16
2005a8f4:	4581                	c.li	a1,0
2005a8f6:	dffaf097          	auipc	ra,0xdffaf
2005a8fa:	1d8080e7          	jalr	ra,472(ra) # 9ace <__wrap_memset>
2005a8fe:	b4fff0ef          	jal	ra,2005a44c <update_status>
2005a902:	4605                	c.li	a2,1
2005a904:	4581                	c.li	a1,0
2005a906:	ec018513          	addi	a0,gp,-320 # 20066848 <top_exit_sema>
2005a90a:	75f000ef          	jal	ra,2005b868 <rtos_sema_create>
2005a90e:	2005a637          	lui	a2,0x2005a
2005a912:	200635b7          	lui	a1,0x20063
2005a916:	47a9                	c.li	a5,10
2005a918:	6711                	c.lui	a4,0x4
2005a91a:	eb418693          	addi	a3,gp,-332 # 2006683c <para_in>
2005a91e:	6be60613          	addi	a2,a2,1726 # 2005a6be <cpu_stat_thread>
2005a922:	e0c58593          	addi	a1,a1,-500 # 20062e0c <pmap_func+0xa00>
2005a926:	4501                	c.li	a0,0
2005a928:	282010ef          	jal	ra,2005bbaa <rtos_task_create>
2005a92c:	20063537          	lui	a0,0x20063
2005a930:	e1ca0593          	addi	a1,s4,-484
2005a934:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
2005a938:	257040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a93c:	50f6                	c.lwsp	ra,124(sp)
2005a93e:	5466                	c.lwsp	s0,120(sp)
2005a940:	54d6                	c.lwsp	s1,116(sp)
2005a942:	5946                	c.lwsp	s2,112(sp)
2005a944:	59b6                	c.lwsp	s3,108(sp)
2005a946:	5a26                	c.lwsp	s4,104(sp)
2005a948:	5a96                	c.lwsp	s5,100(sp)
2005a94a:	6109                	c.addi16sp	sp,128
2005a94c:	8082                	c.jr	ra
2005a94e:	478d                	c.li	a5,3
2005a950:	02f40c63          	beq	s0,a5,2005a988 <at_cpuload+0x208>
2005a954:	440d                	c.li	s0,3
2005a956:	bdb5                	c.j	2005a7d2 <at_cpuload+0x52>
2005a958:	4388                	c.lw	a0,0(a5)
2005a95a:	d969                	c.beqz	a0,2005a92c <at_cpuload+0x1ac>
2005a95c:	7fb000ef          	jal	ra,2005b956 <rtos_sema_give>
2005a960:	b7f1                	c.j	2005a92c <at_cpuload+0x1ac>
2005a962:	ec01a783          	lw	a5,-320(gp) # 20066848 <top_exit_sema>
2005a966:	f3f9                	c.bnez	a5,2005a92c <at_cpuload+0x1ac>
2005a968:	4088                	c.lw	a0,0(s1)
2005a96a:	6609                	c.lui	a2,0x2
2005a96c:	0641                	c.addi	a2,16
2005a96e:	4581                	c.li	a1,0
2005a970:	dffaf097          	auipc	ra,0xdffaf
2005a974:	15e080e7          	jalr	ra,350(ra) # 9ace <__wrap_memset>
2005a978:	410187b7          	lui	a5,0x41018
2005a97c:	4bd8                	c.lw	a4,20(a5)
2005a97e:	eae1a823          	sw	a4,-336(gp) # 20066838 <last_tick>
2005a982:	acbff0ef          	jal	ra,2005a44c <update_status>
2005a986:	b75d                	c.j	2005a92c <at_cpuload+0x1ac>
2005a988:	ec01a783          	lw	a5,-320(gp) # 20066848 <top_exit_sema>
2005a98c:	f3c5                	c.bnez	a5,2005a92c <at_cpuload+0x1ac>
2005a98e:	abfff0ef          	jal	ra,2005a44c <update_status>
2005a992:	41018937          	lui	s2,0x41018
2005a996:	eb018413          	addi	s0,gp,-336 # 20066838 <last_tick>
2005a99a:	401c                	c.lw	a5,0(s0)
2005a99c:	01492503          	lw	a0,20(s2) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a9a0:	8d1d                	c.sub	a0,a5
2005a9a2:	3ed5                	c.jal	2005a596 <print_delta>
2005a9a4:	01492783          	lw	a5,20(s2)
2005a9a8:	4088                	c.lw	a0,0(s1)
2005a9aa:	c01c                	c.sw	a5,0(s0)
2005a9ac:	55b000ef          	jal	ra,2005b706 <rtos_mem_free>
2005a9b0:	0004a023          	sw	zero,0(s1)
2005a9b4:	bfa5                	c.j	2005a92c <at_cpuload+0x1ac>

2005a9b6 <print_system_at>:
2005a9b6:	1141                	c.addi	sp,-16
2005a9b8:	c422                	c.swsp	s0,8(sp)
2005a9ba:	c9c18413          	addi	s0,gp,-868 # 20066624 <at_sys_items>
2005a9be:	c226                	c.swsp	s1,4(sp)
2005a9c0:	c04a                	c.swsp	s2,0(sp)
2005a9c2:	c606                	c.swsp	ra,12(sp)
2005a9c4:	0b040493          	addi	s1,s0,176
2005a9c8:	20063937          	lui	s2,0x20063
2005a9cc:	400c                	c.lw	a1,0(s0)
2005a9ce:	ea890513          	addi	a0,s2,-344 # 20062ea8 <pmap_func+0xa9c>
2005a9d2:	0441                	c.addi	s0,16
2005a9d4:	1bb040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a9d8:	fe941ae3          	bne	s0,s1,2005a9cc <print_system_at+0x16>
2005a9dc:	40b2                	c.lwsp	ra,12(sp)
2005a9de:	4422                	c.lwsp	s0,8(sp)
2005a9e0:	4492                	c.lwsp	s1,4(sp)
2005a9e2:	4902                	c.lwsp	s2,0(sp)
2005a9e4:	0141                	c.addi	sp,16
2005a9e6:	8082                	c.jr	ra

2005a9e8 <at_list>:
2005a9e8:	20063537          	lui	a0,0x20063
2005a9ec:	1141                	c.addi	sp,-16
2005a9ee:	eb050513          	addi	a0,a0,-336 # 20062eb0 <pmap_func+0xaa4>
2005a9f2:	c606                	c.swsp	ra,12(sp)
2005a9f4:	19b040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005a9f8:	3f7d                	c.jal	2005a9b6 <print_system_at>
2005a9fa:	40b2                	c.lwsp	ra,12(sp)
2005a9fc:	200635b7          	lui	a1,0x20063
2005aa00:	20063537          	lui	a0,0x20063
2005aa04:	ec858593          	addi	a1,a1,-312 # 20062ec8 <pmap_func+0xabc>
2005aa08:	88c50513          	addi	a0,a0,-1908 # 2006288c <pmap_func+0x480>
2005aa0c:	0141                	c.addi	sp,16
2005aa0e:	1810406f          	jal	zero,2005f38e <DiagPrintf_minimal>

2005aa12 <at_sys_init>:
2005aa12:	45ad                	c.li	a1,11
2005aa14:	c9c18513          	addi	a0,gp,-868 # 20066624 <at_sys_items>
2005aa18:	b4aff06f          	jal	zero,20059d62 <atcmd_service_add_table>

2005aa1c <BOOT_Image1>:
2005aa1c:	20000537          	lui	a0,0x20000
2005aa20:	200017b7          	lui	a5,0x20001
2005aa24:	cac78793          	addi	a5,a5,-852 # 20000cac <__rom_bss_end__>
2005aa28:	00050613          	addi	a2,a0,0 # 20000000 <UserIrqFunTable>
2005aa2c:	1141                	c.addi	sp,-16
2005aa2e:	40c78633          	sub	a2,a5,a2
2005aa32:	4581                	c.li	a1,0
2005aa34:	00050513          	addi	a0,a0,0
2005aa38:	c606                	c.swsp	ra,12(sp)
2005aa3a:	dffaf097          	auipc	ra,0xdffaf
2005aa3e:	d50080e7          	jalr	ra,-688(ra) # 978a <_memset>
2005aa42:	dffa6097          	auipc	ra,0xdffa6
2005aa46:	0d4080e7          	jalr	ra,212(ra) # b16 <CPU_ClkGet>
2005aa4a:	000f47b7          	lui	a5,0xf4
2005aa4e:	24078793          	addi	a5,a5,576 # f4240 <__ap_sram_heap_size+0xb4240>
2005aa52:	02f55533          	divu	a0,a0,a5
2005aa56:	200017b7          	lui	a5,0x20001
2005aa5a:	86a7a023          	sw	a0,-1952(a5) # 20000860 <RBSS_UDELAY_CLK>
2005aa5e:	dffa6097          	auipc	ra,0xdffa6
2005aa62:	ec6080e7          	jalr	ra,-314(ra) # 924 <BOOT_ROM_InitDebugFlg>
2005aa66:	200587b7          	lui	a5,0x20058
2005aa6a:	04078793          	addi	a5,a5,64 # 20058040 <Img2EntryFun0>
2005aa6e:	0017c703          	lbu	a4,1(a5)
2005aa72:	0007c683          	lbu	a3,0(a5)
2005aa76:	40b2                	c.lwsp	ra,12(sp)
2005aa78:	0722                	c.slli	a4,0x8
2005aa7a:	8ed9                	c.or	a3,a4
2005aa7c:	0027c703          	lbu	a4,2(a5)
2005aa80:	0037c783          	lbu	a5,3(a5)
2005aa84:	0742                	c.slli	a4,0x10
2005aa86:	8f55                	c.or	a4,a3
2005aa88:	07e2                	c.slli	a5,0x18
2005aa8a:	8fd9                	c.or	a5,a4
2005aa8c:	0141                	c.addi	sp,16
2005aa8e:	8782                	c.jr	a5

2005aa90 <flash_read_word>:
2005aa90:	1141                	c.addi	sp,-16
2005aa92:	852e                	c.mv	a0,a1
2005aa94:	4591                	c.li	a1,4
2005aa96:	c606                	c.swsp	ra,12(sp)
2005aa98:	8dafe0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005aa9c:	40b2                	c.lwsp	ra,12(sp)
2005aa9e:	4505                	c.li	a0,1
2005aaa0:	0141                	c.addi	sp,16
2005aaa2:	8082                	c.jr	ra

2005aaa4 <os_heap_init>:
2005aaa4:	1141                	c.addi	sp,-16
2005aaa6:	c606                	c.swsp	ra,12(sp)
2005aaa8:	c422                	c.swsp	s0,8(sp)
2005aaaa:	0800                	c.addi4spn	s0,sp,16
2005aaac:	00000793          	addi	a5,zero,0
2005aab0:	85be                	c.mv	a1,a5
2005aab2:	ca0007b7          	lui	a5,0xca000
2005aab6:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005aaba:	2869                	c.jal	2005ab54 <os_heap_add>
2005aabc:	67dd                	c.lui	a5,0x17
2005aabe:	50078793          	addi	a5,a5,1280 # 17500 <__bdram_heap_buffer_size__>
2005aac2:	85be                	c.mv	a1,a5
2005aac4:	200697b7          	lui	a5,0x20069
2005aac8:	b0078513          	addi	a0,a5,-1280 # 20068b00 <__bdram_heap_buffer_start__>
2005aacc:	2061                	c.jal	2005ab54 <os_heap_add>
2005aace:	00000793          	addi	a5,zero,0
2005aad2:	85be                	c.mv	a1,a5
2005aad4:	ca0007b7          	lui	a5,0xca000
2005aad8:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005aadc:	28a5                	c.jal	2005ab54 <os_heap_add>
2005aade:	00000793          	addi	a5,zero,0
2005aae2:	85be                	c.mv	a1,a5
2005aae4:	ca0007b7          	lui	a5,0xca000
2005aae8:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005aaec:	20a5                	c.jal	2005ab54 <os_heap_add>
2005aaee:	67a1                	c.lui	a5,0x8
2005aaf0:	00078793          	addi	a5,a5,0 # 8000 <__sram_bootloader_mp_size__>
2005aaf4:	85be                	c.mv	a1,a5
2005aaf6:	200127b7          	lui	a5,0x20012
2005aafa:	00078513          	addi	a0,a5,0 # 20012000 <__sram_bootloader_mp_start__>
2005aafe:	2899                	c.jal	2005ab54 <os_heap_add>
2005ab00:	ec41c783          	lbu	a5,-316(gp) # 2006684c <heap_index>
2005ab04:	c391                	c.beqz	a5,2005ab08 <os_heap_init+0x64>
2005ab06:	2809                	c.jal	2005ab18 <os_heap_sort>
2005ab08:	57818513          	addi	a0,gp,1400 # 20066f00 <xHeapRegions>
2005ab0c:	2481                	c.jal	2005ad4c <vPortDefineHeapRegions>
2005ab0e:	0001                	c.addi	zero,0
2005ab10:	40b2                	c.lwsp	ra,12(sp)
2005ab12:	4422                	c.lwsp	s0,8(sp)
2005ab14:	0141                	c.addi	sp,16
2005ab16:	8082                	c.jr	ra

2005ab18 <os_heap_sort>:
2005ab18:	4681                	c.li	a3,0
2005ab1a:	ec41c703          	lbu	a4,-316(gp) # 2006684c <heap_index>
2005ab1e:	177d                	c.addi	a4,-1
2005ab20:	02e6d963          	bge	a3,a4,2005ab52 <os_heap_sort+0x3a>
2005ab24:	57818793          	addi	a5,gp,1400 # 20066f00 <xHeapRegions>
2005ab28:	4601                	c.li	a2,0
2005ab2a:	8f15                	c.sub	a4,a3
2005ab2c:	a839                	c.j	2005ab4a <os_heap_sort+0x32>
2005ab2e:	438c                	c.lw	a1,0(a5)
2005ab30:	4788                	c.lw	a0,8(a5)
2005ab32:	0605                	c.addi	a2,1
2005ab34:	00b57a63          	bgeu	a0,a1,2005ab48 <os_heap_sort+0x30>
2005ab38:	0047a303          	lw	t1,4(a5)
2005ab3c:	c388                	c.sw	a0,0(a5)
2005ab3e:	47c8                	c.lw	a0,12(a5)
2005ab40:	c78c                	c.sw	a1,8(a5)
2005ab42:	0067a623          	sw	t1,12(a5)
2005ab46:	c3c8                	c.sw	a0,4(a5)
2005ab48:	07a1                	c.addi	a5,8
2005ab4a:	fee642e3          	blt	a2,a4,2005ab2e <os_heap_sort+0x16>
2005ab4e:	0685                	c.addi	a3,1
2005ab50:	b7e9                	c.j	2005ab1a <os_heap_sort+0x2>
2005ab52:	8082                	c.jr	ra

2005ab54 <os_heap_add>:
2005ab54:	1141                	c.addi	sp,-16
2005ab56:	c426                	c.swsp	s1,8(sp)
2005ab58:	ec41c703          	lbu	a4,-316(gp) # 2006684c <heap_index>
2005ab5c:	c24a                	c.swsp	s2,4(sp)
2005ab5e:	c04e                	c.swsp	s3,0(sp)
2005ab60:	c606                	c.swsp	ra,12(sp)
2005ab62:	4795                	c.li	a5,5
2005ab64:	89aa                	c.mv	s3,a0
2005ab66:	892e                	c.mv	s2,a1
2005ab68:	00e7fa63          	bgeu	a5,a4,2005ab7c <os_heap_add+0x28>
2005ab6c:	20063537          	lui	a0,0x20063
2005ab70:	02600593          	addi	a1,zero,38
2005ab74:	f4450513          	addi	a0,a0,-188 # 20062f44 <__FUNCTION__.0+0x10>
2005ab78:	51d040ef          	jal	ra,2005f894 <io_assert_failed>
2005ab7c:	ec41c703          	lbu	a4,-316(gp) # 2006684c <heap_index>
2005ab80:	57818793          	addi	a5,gp,1400 # 20066f00 <xHeapRegions>
2005ab84:	00371693          	slli	a3,a4,0x3
2005ab88:	97b6                	c.add	a5,a3
2005ab8a:	0137a023          	sw	s3,0(a5)
2005ab8e:	0127a223          	sw	s2,4(a5)
2005ab92:	4501                	c.li	a0,0
2005ab94:	00090c63          	beq	s2,zero,2005abac <os_heap_add+0x58>
2005ab98:	ec818793          	addi	a5,gp,-312 # 20066850 <total_heap_size>
2005ab9c:	438c                	c.lw	a1,0(a5)
2005ab9e:	0705                	c.addi	a4,1
2005aba0:	ece18223          	sb	a4,-316(gp) # 2006684c <heap_index>
2005aba4:	992e                	c.add	s2,a1
2005aba6:	0127a023          	sw	s2,0(a5)
2005abaa:	4505                	c.li	a0,1
2005abac:	40b2                	c.lwsp	ra,12(sp)
2005abae:	44a2                	c.lwsp	s1,8(sp)
2005abb0:	4912                	c.lwsp	s2,4(sp)
2005abb2:	4982                	c.lwsp	s3,0(sp)
2005abb4:	0141                	c.addi	sp,16
2005abb6:	8082                	c.jr	ra

2005abb8 <__wrap_malloc>:
2005abb8:	3250006f          	jal	zero,2005b6dc <rtos_mem_malloc>

2005abbc <__wrap_free>:
2005abbc:	34b0006f          	jal	zero,2005b706 <rtos_mem_free>

2005abc0 <__wrap__free_r>:
2005abc0:	852e                	c.mv	a0,a1
2005abc2:	3450006f          	jal	zero,2005b706 <rtos_mem_free>

2005abc6 <prvInsertBlockIntoFreeList>:
2005abc6:	ee418793          	addi	a5,gp,-284 # 2006686c <xStart>
2005abca:	873e                	c.mv	a4,a5
2005abcc:	439c                	c.lw	a5,0(a5)
2005abce:	fea7eee3          	bltu	a5,a0,2005abca <prvInsertBlockIntoFreeList+0x4>
2005abd2:	4350                	c.lw	a2,4(a4)
2005abd4:	00c706b3          	add	a3,a4,a2
2005abd8:	00d51663          	bne	a0,a3,2005abe4 <prvInsertBlockIntoFreeList+0x1e>
2005abdc:	4154                	c.lw	a3,4(a0)
2005abde:	853a                	c.mv	a0,a4
2005abe0:	96b2                	c.add	a3,a2
2005abe2:	c354                	c.sw	a3,4(a4)
2005abe4:	4154                	c.lw	a3,4(a0)
2005abe6:	00d50633          	add	a2,a0,a3
2005abea:	00c79b63          	bne	a5,a2,2005ac00 <prvInsertBlockIntoFreeList+0x3a>
2005abee:	ecc1a603          	lw	a2,-308(gp) # 20066854 <pxEnd>
2005abf2:	00c78763          	beq	a5,a2,2005ac00 <prvInsertBlockIntoFreeList+0x3a>
2005abf6:	43dc                	c.lw	a5,4(a5)
2005abf8:	97b6                	c.add	a5,a3
2005abfa:	c15c                	c.sw	a5,4(a0)
2005abfc:	431c                	c.lw	a5,0(a4)
2005abfe:	439c                	c.lw	a5,0(a5)
2005ac00:	c11c                	c.sw	a5,0(a0)
2005ac02:	00e50363          	beq	a0,a4,2005ac08 <prvInsertBlockIntoFreeList+0x42>
2005ac06:	c308                	c.sw	a0,0(a4)
2005ac08:	8082                	c.jr	ra

2005ac0a <pvPortMalloc>:
2005ac0a:	1101                	c.addi	sp,-32
2005ac0c:	cc22                	c.swsp	s0,24(sp)
2005ac0e:	c452                	c.swsp	s4,8(sp)
2005ac10:	842a                	c.mv	s0,a0
2005ac12:	ce06                	c.swsp	ra,28(sp)
2005ac14:	ca26                	c.swsp	s1,20(sp)
2005ac16:	c84a                	c.swsp	s2,16(sp)
2005ac18:	c64e                	c.swsp	s3,12(sp)
2005ac1a:	2c3020ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005ac1e:	ed01aa03          	lw	s4,-304(gp) # 20066858 <xBlockAllocatedBit>
2005ac22:	008a77b3          	and	a5,s4,s0
2005ac26:	c799                	c.beqz	a5,2005ac34 <pvPortMalloc+0x2a>
2005ac28:	362030ef          	jal	ra,2005df8a <xTaskResumeAll>
2005ac2c:	1b5010ef          	jal	ra,2005c5e0 <vApplicationMallocFailedHook>
2005ac30:	4901                	c.li	s2,0
2005ac32:	a879                	c.j	2005acd0 <pvPortMalloc+0xc6>
2005ac34:	fff40793          	addi	a5,s0,-1
2005ac38:	fde00713          	addi	a4,zero,-34
2005ac3c:	fef766e3          	bltu	a4,a5,2005ac28 <pvPortMalloc+0x1e>
2005ac40:	02040793          	addi	a5,s0,32
2005ac44:	887d                	c.andi	s0,31
2005ac46:	c801                	c.beqz	s0,2005ac56 <pvPortMalloc+0x4c>
2005ac48:	fe07f713          	andi	a4,a5,-32
2005ac4c:	02070713          	addi	a4,a4,32 # 4020 <GPIO_Direction+0x56>
2005ac50:	fce7fce3          	bgeu	a5,a4,2005ac28 <pvPortMalloc+0x1e>
2005ac54:	87ba                	c.mv	a5,a4
2005ac56:	ed41a483          	lw	s1,-300(gp) # 2006685c <xFreeBytesRemaining>
2005ac5a:	ed418993          	addi	s3,gp,-300 # 2006685c <xFreeBytesRemaining>
2005ac5e:	fcf4e5e3          	bltu	s1,a5,2005ac28 <pvPortMalloc+0x1e>
2005ac62:	ee41a403          	lw	s0,-284(gp) # 2006686c <xStart>
2005ac66:	ee418713          	addi	a4,gp,-284 # 2006686c <xStart>
2005ac6a:	4054                	c.lw	a3,4(s0)
2005ac6c:	00f6f463          	bgeu	a3,a5,2005ac74 <pvPortMalloc+0x6a>
2005ac70:	4014                	c.lw	a3,0(s0)
2005ac72:	eaa5                	c.bnez	a3,2005ace2 <pvPortMalloc+0xd8>
2005ac74:	ecc1a683          	lw	a3,-308(gp) # 20066854 <pxEnd>
2005ac78:	fa8688e3          	beq	a3,s0,2005ac28 <pvPortMalloc+0x1e>
2005ac7c:	4014                	c.lw	a3,0(s0)
2005ac7e:	00072903          	lw	s2,0(a4)
2005ac82:	c314                	c.sw	a3,0(a4)
2005ac84:	4054                	c.lw	a3,4(s0)
2005ac86:	04000713          	addi	a4,zero,64
2005ac8a:	02090913          	addi	s2,s2,32
2005ac8e:	8e9d                	c.sub	a3,a5
2005ac90:	00d77763          	bgeu	a4,a3,2005ac9e <pvPortMalloc+0x94>
2005ac94:	00f40533          	add	a0,s0,a5
2005ac98:	c154                	c.sw	a3,4(a0)
2005ac9a:	c05c                	c.sw	a5,4(s0)
2005ac9c:	372d                	c.jal	2005abc6 <prvInsertBlockIntoFreeList>
2005ac9e:	4058                	c.lw	a4,4(s0)
2005aca0:	ed818693          	addi	a3,gp,-296 # 20066860 <xMinimumEverFreeBytesRemaining>
2005aca4:	4290                	c.lw	a2,0(a3)
2005aca6:	40e487b3          	sub	a5,s1,a4
2005acaa:	00f9a023          	sw	a5,0(s3)
2005acae:	00c7f363          	bgeu	a5,a2,2005acb4 <pvPortMalloc+0xaa>
2005acb2:	c29c                	c.sw	a5,0(a3)
2005acb4:	00ea6733          	or	a4,s4,a4
2005acb8:	edc18793          	addi	a5,gp,-292 # 20066864 <xNumberOfSuccessfulAllocations>
2005acbc:	c058                	c.sw	a4,4(s0)
2005acbe:	4398                	c.lw	a4,0(a5)
2005acc0:	00042023          	sw	zero,0(s0)
2005acc4:	0705                	c.addi	a4,1
2005acc6:	c398                	c.sw	a4,0(a5)
2005acc8:	2c2030ef          	jal	ra,2005df8a <xTaskResumeAll>
2005accc:	f60900e3          	beq	s2,zero,2005ac2c <pvPortMalloc+0x22>
2005acd0:	40f2                	c.lwsp	ra,28(sp)
2005acd2:	4462                	c.lwsp	s0,24(sp)
2005acd4:	44d2                	c.lwsp	s1,20(sp)
2005acd6:	49b2                	c.lwsp	s3,12(sp)
2005acd8:	4a22                	c.lwsp	s4,8(sp)
2005acda:	854a                	c.mv	a0,s2
2005acdc:	4942                	c.lwsp	s2,16(sp)
2005acde:	6105                	c.addi16sp	sp,32
2005ace0:	8082                	c.jr	ra
2005ace2:	8722                	c.mv	a4,s0
2005ace4:	8436                	c.mv	s0,a3
2005ace6:	b751                	c.j	2005ac6a <pvPortMalloc+0x60>

2005ace8 <vPortFree>:
2005ace8:	cd31                	c.beqz	a0,2005ad44 <vPortFree+0x5c>
2005acea:	1141                	c.addi	sp,-16
2005acec:	c422                	c.swsp	s0,8(sp)
2005acee:	c606                	c.swsp	ra,12(sp)
2005acf0:	fe452703          	lw	a4,-28(a0)
2005acf4:	ed01a783          	lw	a5,-304(gp) # 20066858 <xBlockAllocatedBit>
2005acf8:	842a                	c.mv	s0,a0
2005acfa:	00f776b3          	and	a3,a4,a5
2005acfe:	ce9d                	c.beqz	a3,2005ad3c <vPortFree+0x54>
2005ad00:	fe052683          	lw	a3,-32(a0)
2005ad04:	ee85                	c.bnez	a3,2005ad3c <vPortFree+0x54>
2005ad06:	fff7c793          	xori	a5,a5,-1
2005ad0a:	8ff9                	c.and	a5,a4
2005ad0c:	fef52223          	sw	a5,-28(a0)
2005ad10:	1cd020ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005ad14:	ed418793          	addi	a5,gp,-300 # 2006685c <xFreeBytesRemaining>
2005ad18:	4394                	c.lw	a3,0(a5)
2005ad1a:	fe442703          	lw	a4,-28(s0)
2005ad1e:	fe040513          	addi	a0,s0,-32
2005ad22:	9736                	c.add	a4,a3
2005ad24:	c398                	c.sw	a4,0(a5)
2005ad26:	3545                	c.jal	2005abc6 <prvInsertBlockIntoFreeList>
2005ad28:	ee018793          	addi	a5,gp,-288 # 20066868 <xNumberOfSuccessfulFrees>
2005ad2c:	4398                	c.lw	a4,0(a5)
2005ad2e:	0705                	c.addi	a4,1
2005ad30:	c398                	c.sw	a4,0(a5)
2005ad32:	4422                	c.lwsp	s0,8(sp)
2005ad34:	40b2                	c.lwsp	ra,12(sp)
2005ad36:	0141                	c.addi	sp,16
2005ad38:	2520306f          	jal	zero,2005df8a <xTaskResumeAll>
2005ad3c:	40b2                	c.lwsp	ra,12(sp)
2005ad3e:	4422                	c.lwsp	s0,8(sp)
2005ad40:	0141                	c.addi	sp,16
2005ad42:	8082                	c.jr	ra
2005ad44:	8082                	c.jr	ra

2005ad46 <xPortGetFreeHeapSize>:
2005ad46:	ed41a503          	lw	a0,-300(gp) # 2006685c <xFreeBytesRemaining>
2005ad4a:	8082                	c.jr	ra

2005ad4c <vPortDefineHeapRegions>:
2005ad4c:	ecc1a583          	lw	a1,-308(gp) # 20066854 <pxEnd>
2005ad50:	4701                	c.li	a4,0
2005ad52:	4301                	c.li	t1,0
2005ad54:	4681                	c.li	a3,0
2005ad56:	ecc18613          	addi	a2,gp,-308 # 20066854 <pxEnd>
2005ad5a:	ee418813          	addi	a6,gp,-284 # 2006686c <xStart>
2005ad5e:	415c                	c.lw	a5,4(a0)
2005ad60:	ef81                	c.bnez	a5,2005ad78 <vPortDefineHeapRegions+0x2c>
2005ad62:	c311                	c.beqz	a4,2005ad66 <vPortDefineHeapRegions+0x1a>
2005ad64:	c20c                	c.sw	a1,0(a2)
2005ad66:	ecd1ac23          	sw	a3,-296(gp) # 20066860 <xMinimumEverFreeBytesRemaining>
2005ad6a:	ecd1aa23          	sw	a3,-300(gp) # 2006685c <xFreeBytesRemaining>
2005ad6e:	80000737          	lui	a4,0x80000
2005ad72:	ece1a823          	sw	a4,-304(gp) # 20066858 <xBlockAllocatedBit>
2005ad76:	8082                	c.jr	ra
2005ad78:	4118                	c.lw	a4,0(a0)
2005ad7a:	01f77893          	andi	a7,a4,31
2005ad7e:	00088a63          	beq	a7,zero,2005ad92 <vPortDefineHeapRegions+0x46>
2005ad82:	01f70893          	addi	a7,a4,31 # 8000001f <__ctrace_end__+0xb600001f>
2005ad86:	fe08f893          	andi	a7,a7,-32
2005ad8a:	97ba                	c.add	a5,a4
2005ad8c:	411787b3          	sub	a5,a5,a7
2005ad90:	8746                	c.mv	a4,a7
2005ad92:	00031663          	bne	t1,zero,2005ad9e <vPortDefineHeapRegions+0x52>
2005ad96:	00e82023          	sw	a4,0(a6)
2005ad9a:	00082223          	sw	zero,4(a6)
2005ad9e:	97ba                	c.add	a5,a4
2005ada0:	1781                	c.addi	a5,-32
2005ada2:	9b81                	c.andi	a5,-32
2005ada4:	0007a223          	sw	zero,4(a5)
2005ada8:	0007a023          	sw	zero,0(a5)
2005adac:	40e788b3          	sub	a7,a5,a4
2005adb0:	01172223          	sw	a7,4(a4)
2005adb4:	c31c                	c.sw	a5,0(a4)
2005adb6:	c191                	c.beqz	a1,2005adba <vPortDefineHeapRegions+0x6e>
2005adb8:	c198                	c.sw	a4,0(a1)
2005adba:	4358                	c.lw	a4,4(a4)
2005adbc:	0305                	c.addi	t1,1
2005adbe:	0521                	c.addi	a0,8
2005adc0:	96ba                	c.add	a3,a4
2005adc2:	85be                	c.mv	a1,a5
2005adc4:	4705                	c.li	a4,1
2005adc6:	bf61                	c.j	2005ad5e <vPortDefineHeapRegions+0x12>

2005adc8 <vPortGetHeapStats>:
2005adc8:	1101                	c.addi	sp,-32
2005adca:	cc22                	c.swsp	s0,24(sp)
2005adcc:	ce06                	c.swsp	ra,28(sp)
2005adce:	ca26                	c.swsp	s1,20(sp)
2005add0:	c84a                	c.swsp	s2,16(sp)
2005add2:	c64e                	c.swsp	s3,12(sp)
2005add4:	842a                	c.mv	s0,a0
2005add6:	107020ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005adda:	ee41a783          	lw	a5,-284(gp) # 2006686c <xStart>
2005adde:	cfb9                	c.beqz	a5,2005ae3c <vPortGetHeapStats+0x74>
2005ade0:	ecc1a683          	lw	a3,-308(gp) # 20066854 <pxEnd>
2005ade4:	597d                	c.li	s2,-1
2005ade6:	4981                	c.li	s3,0
2005ade8:	4481                	c.li	s1,0
2005adea:	43d8                	c.lw	a4,4(a5)
2005adec:	0485                	c.addi	s1,1
2005adee:	00e9f363          	bgeu	s3,a4,2005adf4 <vPortGetHeapStats+0x2c>
2005adf2:	89ba                	c.mv	s3,a4
2005adf4:	c701                	c.beqz	a4,2005adfc <vPortGetHeapStats+0x34>
2005adf6:	01277363          	bgeu	a4,s2,2005adfc <vPortGetHeapStats+0x34>
2005adfa:	893a                	c.mv	s2,a4
2005adfc:	439c                	c.lw	a5,0(a5)
2005adfe:	fef696e3          	bne	a3,a5,2005adea <vPortGetHeapStats+0x22>
2005ae02:	188030ef          	jal	ra,2005df8a <xTaskResumeAll>
2005ae06:	01342223          	sw	s3,4(s0)
2005ae0a:	01242423          	sw	s2,8(s0)
2005ae0e:	c444                	c.sw	s1,12(s0)
2005ae10:	5f3020ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005ae14:	ed41a783          	lw	a5,-300(gp) # 2006685c <xFreeBytesRemaining>
2005ae18:	40f2                	c.lwsp	ra,28(sp)
2005ae1a:	44d2                	c.lwsp	s1,20(sp)
2005ae1c:	c01c                	c.sw	a5,0(s0)
2005ae1e:	edc1a783          	lw	a5,-292(gp) # 20066864 <xNumberOfSuccessfulAllocations>
2005ae22:	4942                	c.lwsp	s2,16(sp)
2005ae24:	49b2                	c.lwsp	s3,12(sp)
2005ae26:	c85c                	c.sw	a5,20(s0)
2005ae28:	ee01a783          	lw	a5,-288(gp) # 20066868 <xNumberOfSuccessfulFrees>
2005ae2c:	cc1c                	c.sw	a5,24(s0)
2005ae2e:	ed81a783          	lw	a5,-296(gp) # 20066860 <xMinimumEverFreeBytesRemaining>
2005ae32:	c81c                	c.sw	a5,16(s0)
2005ae34:	4462                	c.lwsp	s0,24(sp)
2005ae36:	6105                	c.addi16sp	sp,32
2005ae38:	5e50206f          	jal	zero,2005dc1c <vTaskExitCritical>
2005ae3c:	597d                	c.li	s2,-1
2005ae3e:	4981                	c.li	s3,0
2005ae40:	4481                	c.li	s1,0
2005ae42:	b7c1                	c.j	2005ae02 <vPortGetHeapStats+0x3a>

2005ae44 <vListInitialise>:
2005ae44:	00850793          	addi	a5,a0,8
2005ae48:	577d                	c.li	a4,-1
2005ae4a:	c15c                	c.sw	a5,4(a0)
2005ae4c:	c518                	c.sw	a4,8(a0)
2005ae4e:	c55c                	c.sw	a5,12(a0)
2005ae50:	c91c                	c.sw	a5,16(a0)
2005ae52:	00052023          	sw	zero,0(a0)
2005ae56:	8082                	c.jr	ra

2005ae58 <vListInitialiseItem>:
2005ae58:	00052823          	sw	zero,16(a0)
2005ae5c:	8082                	c.jr	ra

2005ae5e <vListInsertEnd>:
2005ae5e:	415c                	c.lw	a5,4(a0)
2005ae60:	c1dc                	c.sw	a5,4(a1)
2005ae62:	4798                	c.lw	a4,8(a5)
2005ae64:	c598                	c.sw	a4,8(a1)
2005ae66:	c34c                	c.sw	a1,4(a4)
2005ae68:	c78c                	c.sw	a1,8(a5)
2005ae6a:	c988                	c.sw	a0,16(a1)
2005ae6c:	411c                	c.lw	a5,0(a0)
2005ae6e:	0785                	c.addi	a5,1
2005ae70:	c11c                	c.sw	a5,0(a0)
2005ae72:	8082                	c.jr	ra

2005ae74 <vListInsert>:
2005ae74:	4194                	c.lw	a3,0(a1)
2005ae76:	57fd                	c.li	a5,-1
2005ae78:	00850713          	addi	a4,a0,8
2005ae7c:	00f69d63          	bne	a3,a5,2005ae96 <vListInsert+0x22>
2005ae80:	491c                	c.lw	a5,16(a0)
2005ae82:	43d8                	c.lw	a4,4(a5)
2005ae84:	c1d8                	c.sw	a4,4(a1)
2005ae86:	c70c                	c.sw	a1,8(a4)
2005ae88:	c59c                	c.sw	a5,8(a1)
2005ae8a:	c3cc                	c.sw	a1,4(a5)
2005ae8c:	c988                	c.sw	a0,16(a1)
2005ae8e:	411c                	c.lw	a5,0(a0)
2005ae90:	0785                	c.addi	a5,1
2005ae92:	c11c                	c.sw	a5,0(a0)
2005ae94:	8082                	c.jr	ra
2005ae96:	87ba                	c.mv	a5,a4
2005ae98:	4358                	c.lw	a4,4(a4)
2005ae9a:	4310                	c.lw	a2,0(a4)
2005ae9c:	fec6fde3          	bgeu	a3,a2,2005ae96 <vListInsert+0x22>
2005aea0:	b7cd                	c.j	2005ae82 <vListInsert+0xe>

2005aea2 <uxListRemove>:
2005aea2:	4154                	c.lw	a3,4(a0)
2005aea4:	4518                	c.lw	a4,8(a0)
2005aea6:	491c                	c.lw	a5,16(a0)
2005aea8:	c698                	c.sw	a4,8(a3)
2005aeaa:	4154                	c.lw	a3,4(a0)
2005aeac:	c354                	c.sw	a3,4(a4)
2005aeae:	43d8                	c.lw	a4,4(a5)
2005aeb0:	00a71463          	bne	a4,a0,2005aeb8 <uxListRemove+0x16>
2005aeb4:	4518                	c.lw	a4,8(a0)
2005aeb6:	c3d8                	c.sw	a4,4(a5)
2005aeb8:	00052823          	sw	zero,16(a0)
2005aebc:	4398                	c.lw	a4,0(a5)
2005aebe:	177d                	c.addi	a4,-1
2005aec0:	c398                	c.sw	a4,0(a5)
2005aec2:	4388                	c.lw	a0,0(a5)
2005aec4:	8082                	c.jr	ra

2005aec6 <rtk_log_array_print>:
2005aec6:	1101                	c.addi	sp,-32
2005aec8:	cc22                	c.swsp	s0,24(sp)
2005aeca:	c84a                	c.swsp	s2,16(sp)
2005aecc:	ce06                	c.swsp	ra,28(sp)
2005aece:	ca26                	c.swsp	s1,20(sp)
2005aed0:	c64e                	c.swsp	s3,12(sp)
2005aed2:	eec18793          	addi	a5,gp,-276 # 20066874 <rtk_log_entry_count>
2005aed6:	4394                	c.lw	a3,0(a5)
2005aed8:	470d                	c.li	a4,3
2005aeda:	842a                	c.mv	s0,a0
2005aedc:	4911                	c.li	s2,4
2005aede:	00d76463          	bltu	a4,a3,2005aee6 <rtk_log_array_print+0x20>
2005aee2:	0007a903          	lw	s2,0(a5)
2005aee6:	557d                	c.li	a0,-1
2005aee8:	c801                	c.beqz	s0,2005aef8 <rtk_log_array_print+0x32>
2005aeea:	0405                	c.addi	s0,1
2005aeec:	4481                	c.li	s1,0
2005aeee:	200639b7          	lui	s3,0x20063
2005aef2:	01249a63          	bne	s1,s2,2005af06 <rtk_log_array_print+0x40>
2005aef6:	4501                	c.li	a0,0
2005aef8:	40f2                	c.lwsp	ra,28(sp)
2005aefa:	4462                	c.lwsp	s0,24(sp)
2005aefc:	44d2                	c.lwsp	s1,20(sp)
2005aefe:	4942                	c.lwsp	s2,16(sp)
2005af00:	49b2                	c.lwsp	s3,12(sp)
2005af02:	6105                	c.addi16sp	sp,32
2005af04:	8082                	c.jr	ra
2005af06:	fff44603          	lbu	a2,-1(s0)
2005af0a:	85a2                	c.mv	a1,s0
2005af0c:	fe098513          	addi	a0,s3,-32 # 20062fe0 <__FUNCTION__.0+0xac>
2005af10:	47e040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005af14:	0485                	c.addi	s1,1
2005af16:	042d                	c.addi	s0,11
2005af18:	bfe9                	c.j	2005aef2 <rtk_log_array_print+0x2c>

2005af1a <rtk_log_array_clear>:
2005af1a:	1141                	c.addi	sp,-16
2005af1c:	02c00613          	addi	a2,zero,44
2005af20:	4581                	c.li	a1,0
2005af22:	5a818513          	addi	a0,gp,1448 # 20066f30 <rtk_log_tag_array>
2005af26:	c606                	c.swsp	ra,12(sp)
2005af28:	dffaf097          	auipc	ra,0xdffaf
2005af2c:	862080e7          	jalr	ra,-1950(ra) # 978a <_memset>
2005af30:	40b2                	c.lwsp	ra,12(sp)
2005af32:	ee01a623          	sw	zero,-276(gp) # 20066874 <rtk_log_entry_count>
2005af36:	0141                	c.addi	sp,16
2005af38:	8082                	c.jr	ra

2005af3a <rtk_log_level_get>:
2005af3a:	1101                	c.addi	sp,-32
2005af3c:	c64e                	c.swsp	s3,12(sp)
2005af3e:	c452                	c.swsp	s4,8(sp)
2005af40:	ce06                	c.swsp	ra,28(sp)
2005af42:	cc22                	c.swsp	s0,24(sp)
2005af44:	ca26                	c.swsp	s1,20(sp)
2005af46:	c84a                	c.swsp	s2,16(sp)
2005af48:	eec18793          	addi	a5,gp,-276 # 20066874 <rtk_log_entry_count>
2005af4c:	4394                	c.lw	a3,0(a5)
2005af4e:	470d                	c.li	a4,3
2005af50:	89aa                	c.mv	s3,a0
2005af52:	4a11                	c.li	s4,4
2005af54:	00d76463          	bltu	a4,a3,2005af5c <rtk_log_level_get+0x22>
2005af58:	0007aa03          	lw	s4,0(a5)
2005af5c:	00099a63          	bne	s3,zero,2005af70 <rtk_log_level_get+0x36>
2005af60:	20063537          	lui	a0,0x20063
2005af64:	05000593          	addi	a1,zero,80
2005af68:	ff450513          	addi	a0,a0,-12 # 20062ff4 <__FUNCTION__.0+0xc0>
2005af6c:	129040ef          	jal	ra,2005f894 <io_assert_failed>
2005af70:	5a818913          	addi	s2,gp,1448 # 20066f30 <rtk_log_tag_array>
2005af74:	0905                	c.addi	s2,1
2005af76:	4401                	c.li	s0,0
2005af78:	5a818493          	addi	s1,gp,1448 # 20066f30 <rtk_log_tag_array>
2005af7c:	01441563          	bne	s0,s4,2005af86 <rtk_log_level_get+0x4c>
2005af80:	e7c1c503          	lbu	a0,-388(gp) # 20066804 <rtk_log_default_level>
2005af84:	a839                	c.j	2005afa2 <rtk_log_level_get+0x68>
2005af86:	854a                	c.mv	a0,s2
2005af88:	85ce                	c.mv	a1,s3
2005af8a:	dffaf097          	auipc	ra,0xdffaf
2005af8e:	8a2080e7          	jalr	ra,-1886(ra) # 982c <_strcmp>
2005af92:	092d                	c.addi	s2,11
2005af94:	ed19                	c.bnez	a0,2005afb2 <rtk_log_level_get+0x78>
2005af96:	47ad                	c.li	a5,11
2005af98:	02f40433          	mul	s0,s0,a5
2005af9c:	9426                	c.add	s0,s1
2005af9e:	00044503          	lbu	a0,0(s0)
2005afa2:	40f2                	c.lwsp	ra,28(sp)
2005afa4:	4462                	c.lwsp	s0,24(sp)
2005afa6:	44d2                	c.lwsp	s1,20(sp)
2005afa8:	4942                	c.lwsp	s2,16(sp)
2005afaa:	49b2                	c.lwsp	s3,12(sp)
2005afac:	4a22                	c.lwsp	s4,8(sp)
2005afae:	6105                	c.addi16sp	sp,32
2005afb0:	8082                	c.jr	ra
2005afb2:	0405                	c.addi	s0,1
2005afb4:	b7e1                	c.j	2005af7c <rtk_log_level_get+0x42>

2005afb6 <rtk_log_level_set>:
2005afb6:	1101                	c.addi	sp,-32
2005afb8:	cc22                	c.swsp	s0,24(sp)
2005afba:	eec18793          	addi	a5,gp,-276 # 20066874 <rtk_log_entry_count>
2005afbe:	c84a                	c.swsp	s2,16(sp)
2005afc0:	c64e                	c.swsp	s3,12(sp)
2005afc2:	c05a                	c.swsp	s6,0(sp)
2005afc4:	ce06                	c.swsp	ra,28(sp)
2005afc6:	ca26                	c.swsp	s1,20(sp)
2005afc8:	c452                	c.swsp	s4,8(sp)
2005afca:	c256                	c.swsp	s5,4(sp)
2005afcc:	4398                	c.lw	a4,0(a5)
2005afce:	478d                	c.li	a5,3
2005afd0:	892a                	c.mv	s2,a0
2005afd2:	89ae                	c.mv	s3,a1
2005afd4:	eec18413          	addi	s0,gp,-276 # 20066874 <rtk_log_entry_count>
2005afd8:	4b11                	c.li	s6,4
2005afda:	00e7e463          	bltu	a5,a4,2005afe2 <rtk_log_level_set+0x2c>
2005afde:	00042b03          	lw	s6,0(s0)
2005afe2:	557d                	c.li	a0,-1
2005afe4:	06090b63          	beq	s2,zero,2005b05a <rtk_log_level_set+0xa4>
2005afe8:	4795                	c.li	a5,5
2005afea:	0737e863          	bltu	a5,s3,2005b05a <rtk_log_level_set+0xa4>
2005afee:	200635b7          	lui	a1,0x20063
2005aff2:	08858593          	addi	a1,a1,136 # 20063088 <__FUNCTION__.0+0x154>
2005aff6:	854a                	c.mv	a0,s2
2005aff8:	dffaf097          	auipc	ra,0xdffaf
2005affc:	834080e7          	jalr	ra,-1996(ra) # 982c <_strcmp>
2005b000:	c939                	c.beqz	a0,2005b056 <rtk_log_level_set+0xa0>
2005b002:	5a818a93          	addi	s5,gp,1448 # 20066f30 <rtk_log_tag_array>
2005b006:	0a85                	c.addi	s5,1
2005b008:	4a01                	c.li	s4,0
2005b00a:	5a818493          	addi	s1,gp,1448 # 20066f30 <rtk_log_tag_array>
2005b00e:	076a1063          	bne	s4,s6,2005b06e <rtk_log_level_set+0xb8>
2005b012:	4018                	c.lw	a4,0(s0)
2005b014:	478d                	c.li	a5,3
2005b016:	00e7f863          	bgeu	a5,a4,2005b026 <rtk_log_level_set+0x70>
2005b01a:	20063537          	lui	a0,0x20063
2005b01e:	08c50513          	addi	a0,a0,140 # 2006308c <__FUNCTION__.0+0x158>
2005b022:	36c040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b026:	4018                	c.lw	a4,0(s0)
2005b028:	401c                	c.lw	a5,0(s0)
2005b02a:	452d                	c.li	a0,11
2005b02c:	8b0d                	c.andi	a4,3
2005b02e:	8b8d                	c.andi	a5,3
2005b030:	02a70733          	mul	a4,a4,a0
2005b034:	4625                	c.li	a2,9
2005b036:	85ca                	c.mv	a1,s2
2005b038:	02a787b3          	mul	a5,a5,a0
2005b03c:	9726                	c.add	a4,s1
2005b03e:	01370023          	sb	s3,0(a4)
2005b042:	0785                	c.addi	a5,1
2005b044:	00f48533          	add	a0,s1,a5
2005b048:	581060ef          	jal	ra,20061dc8 <strncpy>
2005b04c:	401c                	c.lw	a5,0(s0)
2005b04e:	4501                	c.li	a0,0
2005b050:	0785                	c.addi	a5,1
2005b052:	c01c                	c.sw	a5,0(s0)
2005b054:	a019                	c.j	2005b05a <rtk_log_level_set+0xa4>
2005b056:	e7318e23          	sb	s3,-388(gp) # 20066804 <rtk_log_default_level>
2005b05a:	40f2                	c.lwsp	ra,28(sp)
2005b05c:	4462                	c.lwsp	s0,24(sp)
2005b05e:	44d2                	c.lwsp	s1,20(sp)
2005b060:	4942                	c.lwsp	s2,16(sp)
2005b062:	49b2                	c.lwsp	s3,12(sp)
2005b064:	4a22                	c.lwsp	s4,8(sp)
2005b066:	4a92                	c.lwsp	s5,4(sp)
2005b068:	4b02                	c.lwsp	s6,0(sp)
2005b06a:	6105                	c.addi16sp	sp,32
2005b06c:	8082                	c.jr	ra
2005b06e:	8556                	c.mv	a0,s5
2005b070:	85ca                	c.mv	a1,s2
2005b072:	dffae097          	auipc	ra,0xdffae
2005b076:	7ba080e7          	jalr	ra,1978(ra) # 982c <_strcmp>
2005b07a:	0aad                	c.addi	s5,11
2005b07c:	e901                	c.bnez	a0,2005b08c <rtk_log_level_set+0xd6>
2005b07e:	47ad                	c.li	a5,11
2005b080:	02fa0a33          	mul	s4,s4,a5
2005b084:	94d2                	c.add	s1,s4
2005b086:	01348023          	sb	s3,0(s1)
2005b08a:	bfc1                	c.j	2005b05a <rtk_log_level_set+0xa4>
2005b08c:	0a05                	c.addi	s4,1
2005b08e:	b741                	c.j	2005b00e <rtk_log_level_set+0x58>

2005b090 <rtk_log_memory_dump_word>:
2005b090:	1101                	c.addi	sp,-32
2005b092:	cc22                	c.swsp	s0,24(sp)
2005b094:	ca26                	c.swsp	s1,20(sp)
2005b096:	c84a                	c.swsp	s2,16(sp)
2005b098:	c64e                	c.swsp	s3,12(sp)
2005b09a:	c452                	c.swsp	s4,8(sp)
2005b09c:	c256                	c.swsp	s5,4(sp)
2005b09e:	c05a                	c.swsp	s6,0(sp)
2005b0a0:	ce06                	c.swsp	ra,28(sp)
2005b0a2:	892a                	c.mv	s2,a0
2005b0a4:	89ae                	c.mv	s3,a1
2005b0a6:	84aa                	c.mv	s1,a0
2005b0a8:	4401                	c.li	s0,0
2005b0aa:	20063a37          	lui	s4,0x20063
2005b0ae:	20063ab7          	lui	s5,0x20063
2005b0b2:	20063b37          	lui	s6,0x20063
2005b0b6:	03341163          	bne	s0,s3,2005b0d8 <rtk_log_memory_dump_word+0x48>
2005b0ba:	4462                	c.lwsp	s0,24(sp)
2005b0bc:	40f2                	c.lwsp	ra,28(sp)
2005b0be:	44d2                	c.lwsp	s1,20(sp)
2005b0c0:	4942                	c.lwsp	s2,16(sp)
2005b0c2:	49b2                	c.lwsp	s3,12(sp)
2005b0c4:	4a22                	c.lwsp	s4,8(sp)
2005b0c6:	4a92                	c.lwsp	s5,4(sp)
2005b0c8:	4b02                	c.lwsp	s6,0(sp)
2005b0ca:	20063537          	lui	a0,0x20063
2005b0ce:	8fc50513          	addi	a0,a0,-1796 # 200628fc <pmap_func+0x4f0>
2005b0d2:	6105                	c.addi16sp	sp,32
2005b0d4:	2ba0406f          	jal	zero,2005f38e <DiagPrintf_minimal>
2005b0d8:	ec11                	c.bnez	s0,2005b0f4 <rtk_log_memory_dump_word+0x64>
2005b0da:	85ca                	c.mv	a1,s2
2005b0dc:	0b8a8513          	addi	a0,s5,184 # 200630b8 <__FUNCTION__.0+0x184>
2005b0e0:	2ae040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b0e4:	408c                	c.lw	a1,0(s1)
2005b0e6:	0ccb0513          	addi	a0,s6,204 # 200630cc <__FUNCTION__.0+0x198>
2005b0ea:	0405                	c.addi	s0,1
2005b0ec:	2a2040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b0f0:	0491                	c.addi	s1,4
2005b0f2:	b7d1                	c.j	2005b0b6 <rtk_log_memory_dump_word+0x26>
2005b0f4:	00747793          	andi	a5,s0,7
2005b0f8:	f7f5                	c.bnez	a5,2005b0e4 <rtk_log_memory_dump_word+0x54>
2005b0fa:	85a6                	c.mv	a1,s1
2005b0fc:	0c0a0513          	addi	a0,s4,192 # 200630c0 <__FUNCTION__.0+0x18c>
2005b100:	b7c5                	c.j	2005b0e0 <rtk_log_memory_dump_word+0x50>

2005b102 <rtk_log_memory_dump_byte>:
2005b102:	1101                	c.addi	sp,-32
2005b104:	cc22                	c.swsp	s0,24(sp)
2005b106:	ca26                	c.swsp	s1,20(sp)
2005b108:	c84a                	c.swsp	s2,16(sp)
2005b10a:	c452                	c.swsp	s4,8(sp)
2005b10c:	c256                	c.swsp	s5,4(sp)
2005b10e:	c05a                	c.swsp	s6,0(sp)
2005b110:	ce06                	c.swsp	ra,28(sp)
2005b112:	c64e                	c.swsp	s3,12(sp)
2005b114:	84aa                	c.mv	s1,a0
2005b116:	892e                	c.mv	s2,a1
2005b118:	4401                	c.li	s0,0
2005b11a:	20063a37          	lui	s4,0x20063
2005b11e:	20063ab7          	lui	s5,0x20063
2005b122:	20063b37          	lui	s6,0x20063
2005b126:	03241163          	bne	s0,s2,2005b148 <rtk_log_memory_dump_byte+0x46>
2005b12a:	4462                	c.lwsp	s0,24(sp)
2005b12c:	40f2                	c.lwsp	ra,28(sp)
2005b12e:	44d2                	c.lwsp	s1,20(sp)
2005b130:	4942                	c.lwsp	s2,16(sp)
2005b132:	49b2                	c.lwsp	s3,12(sp)
2005b134:	4a22                	c.lwsp	s4,8(sp)
2005b136:	4a92                	c.lwsp	s5,4(sp)
2005b138:	4b02                	c.lwsp	s6,0(sp)
2005b13a:	20063537          	lui	a0,0x20063
2005b13e:	8fc50513          	addi	a0,a0,-1796 # 200628fc <pmap_func+0x4f0>
2005b142:	6105                	c.addi16sp	sp,32
2005b144:	24a0406f          	jal	zero,2005f38e <DiagPrintf_minimal>
2005b148:	008489b3          	add	s3,s1,s0
2005b14c:	ec11                	c.bnez	s0,2005b168 <rtk_log_memory_dump_byte+0x66>
2005b14e:	85a6                	c.mv	a1,s1
2005b150:	0b8a8513          	addi	a0,s5,184 # 200630b8 <__FUNCTION__.0+0x184>
2005b154:	23a040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b158:	0009c583          	lbu	a1,0(s3)
2005b15c:	0d4b0513          	addi	a0,s6,212 # 200630d4 <__FUNCTION__.0+0x1a0>
2005b160:	0405                	c.addi	s0,1
2005b162:	22c040ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b166:	b7c1                	c.j	2005b126 <rtk_log_memory_dump_byte+0x24>
2005b168:	00747793          	andi	a5,s0,7
2005b16c:	f7f5                	c.bnez	a5,2005b158 <rtk_log_memory_dump_byte+0x56>
2005b16e:	85ce                	c.mv	a1,s3
2005b170:	0c0a0513          	addi	a0,s4,192 # 200630c0 <__FUNCTION__.0+0x18c>
2005b174:	b7c5                	c.j	2005b154 <rtk_log_memory_dump_byte+0x52>

2005b176 <rtk_log_write>:
2005b176:	7139                	c.addi16sp	sp,-64
2005b178:	d226                	c.swsp	s1,36(sp)
2005b17a:	84aa                	c.mv	s1,a0
2005b17c:	852e                	c.mv	a0,a1
2005b17e:	d422                	c.swsp	s0,40(sp)
2005b180:	d606                	c.swsp	ra,44(sp)
2005b182:	c632                	c.swsp	a2,12(sp)
2005b184:	8436                	c.mv	s0,a3
2005b186:	d83a                	c.swsp	a4,48(sp)
2005b188:	da3e                	c.swsp	a5,52(sp)
2005b18a:	dc42                	c.swsp	a6,56(sp)
2005b18c:	de46                	c.swsp	a7,60(sp)
2005b18e:	c42e                	c.swsp	a1,8(sp)
2005b190:	dabff0ef          	jal	ra,2005af3a <rtk_log_level_get>
2005b194:	02956563          	bltu	a0,s1,2005b1be <rtk_log_write+0x48>
2005b198:	45a2                	c.lwsp	a1,8(sp)
2005b19a:	02300793          	addi	a5,zero,35
2005b19e:	0005c703          	lbu	a4,0(a1)
2005b1a2:	00f70963          	beq	a4,a5,2005b1b4 <rtk_log_write+0x3e>
2005b1a6:	4632                	c.lwsp	a2,12(sp)
2005b1a8:	20063537          	lui	a0,0x20063
2005b1ac:	0dc50513          	addi	a0,a0,220 # 200630dc <__FUNCTION__.0+0x1a8>
2005b1b0:	1be040ef          	jal	ra,2005f36e <DiagPrintf>
2005b1b4:	180c                	c.addi4spn	a1,sp,48
2005b1b6:	8522                	c.mv	a0,s0
2005b1b8:	ce2e                	c.swsp	a1,28(sp)
2005b1ba:	6b7030ef          	jal	ra,2005f070 <DiagVprintf>
2005b1be:	50b2                	c.lwsp	ra,44(sp)
2005b1c0:	5422                	c.lwsp	s0,40(sp)
2005b1c2:	5492                	c.lwsp	s1,36(sp)
2005b1c4:	6121                	c.addi16sp	sp,64
2005b1c6:	8082                	c.jr	ra

2005b1c8 <app_mbedtls_free_func>:
2005b1c8:	ab3d                	c.j	2005b706 <rtos_mem_free>

2005b1ca <app_mbedtls_calloc_func>:
2005b1ca:	02b50633          	mul	a2,a0,a1
2005b1ce:	1101                	c.addi	sp,-32
2005b1d0:	cc22                	c.swsp	s0,24(sp)
2005b1d2:	ce06                	c.swsp	ra,28(sp)
2005b1d4:	8532                	c.mv	a0,a2
2005b1d6:	c632                	c.swsp	a2,12(sp)
2005b1d8:	2311                	c.jal	2005b6dc <rtos_mem_malloc>
2005b1da:	842a                	c.mv	s0,a0
2005b1dc:	c519                	c.beqz	a0,2005b1ea <app_mbedtls_calloc_func+0x20>
2005b1de:	4632                	c.lwsp	a2,12(sp)
2005b1e0:	4581                	c.li	a1,0
2005b1e2:	dffaf097          	auipc	ra,0xdffaf
2005b1e6:	8ec080e7          	jalr	ra,-1812(ra) # 9ace <__wrap_memset>
2005b1ea:	40f2                	c.lwsp	ra,28(sp)
2005b1ec:	8522                	c.mv	a0,s0
2005b1ee:	4462                	c.lwsp	s0,24(sp)
2005b1f0:	6105                	c.addi16sp	sp,32
2005b1f2:	8082                	c.jr	ra

2005b1f4 <app_IWDG_refresh>:
2005b1f4:	4100d537          	lui	a0,0x4100d
2005b1f8:	c0050513          	addi	a0,a0,-1024 # 4100cc00 <__km4_bd_boot_download_addr__+0x10ffac00>
2005b1fc:	dffad317          	auipc	t1,0xdffad
2005b200:	00630067          	jalr	zero,6(t1) # 8202 <WDG_Refresh>

2005b204 <app_init_debug>:
2005b204:	1141                	c.addi	sp,-16
2005b206:	c606                	c.swsp	ra,12(sp)
2005b208:	dffac097          	auipc	ra,0xdffac
2005b20c:	998080e7          	jalr	ra,-1640(ra) # 6ba0 <SYSCFG_OTP_DisBootLog>
2005b210:	00153513          	sltiu	a0,a0,1
2005b214:	200017b7          	lui	a5,0x20001
2005b218:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b21c:	0506                	c.slli	a0,0x1
2005b21e:	577d                	c.li	a4,-1
2005b220:	c398                	c.sw	a4,0(a5)
2005b222:	0007a223          	sw	zero,4(a5)
2005b226:	c788                	c.sw	a0,8(a5)
2005b228:	0007a623          	sw	zero,12(a5)
2005b22c:	40b2                	c.lwsp	ra,12(sp)
2005b22e:	0141                	c.addi	sp,16
2005b230:	8082                	c.jr	ra

2005b232 <app_mbedtls_rom_init>:
2005b232:	2005b5b7          	lui	a1,0x2005b
2005b236:	2005b537          	lui	a0,0x2005b
2005b23a:	1141                	c.addi	sp,-16
2005b23c:	1c858593          	addi	a1,a1,456 # 2005b1c8 <app_mbedtls_free_func>
2005b240:	1ca50513          	addi	a0,a0,458 # 2005b1ca <app_mbedtls_calloc_func>
2005b244:	c606                	c.swsp	ra,12(sp)
2005b246:	403000ef          	jal	ra,2005be48 <mbedtls_platform_set_calloc_free>
2005b24a:	40b2                	c.lwsp	ra,12(sp)
2005b24c:	ee01a823          	sw	zero,-272(gp) # 20066878 <use_hw_crypto_func>
2005b250:	0141                	c.addi	sp,16
2005b252:	8082                	c.jr	ra

2005b254 <app_pmu_init>:
2005b254:	1141                	c.addi	sp,-16
2005b256:	200017b7          	lui	a5,0x20001
2005b25a:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b25e:	c606                	c.swsp	ra,12(sp)
2005b260:	4798                	c.lw	a4,8(a5)
2005b262:	100006b7          	lui	a3,0x10000
2005b266:	4501                	c.li	a0,0
2005b268:	8f55                	c.or	a4,a3
2005b26a:	c798                	c.sw	a4,8(a5)
2005b26c:	dd6fe0ef          	jal	ra,20059842 <pmu_acquire_wakelock>
2005b270:	4100c7b7          	lui	a5,0x4100c
2005b274:	22c7a783          	lw	a5,556(a5) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
2005b278:	8b89                	c.andi	a5,2
2005b27a:	c38d                	c.beqz	a5,2005b29c <app_pmu_init+0x48>
2005b27c:	4509                	c.li	a0,2
2005b27e:	dc4fe0ef          	jal	ra,20059842 <pmu_acquire_wakelock>
2005b282:	9d3fe0ef          	jal	ra,20059c54 <SWR_CORE_Vol_Get>
2005b286:	4785                	c.li	a5,1
2005b288:	00f51a63          	bne	a0,a5,2005b29c <app_pmu_init+0x48>
2005b28c:	922fe0ef          	jal	ra,200593ae <dsp_status_on>
2005b290:	c511                	c.beqz	a0,2005b29c <app_pmu_init+0x48>
2005b292:	40b2                	c.lwsp	ra,12(sp)
2005b294:	4511                	c.li	a0,4
2005b296:	0141                	c.addi	sp,16
2005b298:	daafe06f          	jal	zero,20059842 <pmu_acquire_wakelock>
2005b29c:	40b2                	c.lwsp	ra,12(sp)
2005b29e:	0141                	c.addi	sp,16
2005b2a0:	8082                	c.jr	ra

2005b2a2 <app_IWDG_int>:
2005b2a2:	4100c7b7          	lui	a5,0x4100c
2005b2a6:	479c                	c.lw	a5,8(a5)
2005b2a8:	c0000737          	lui	a4,0xc0000
2005b2ac:	0709                	c.addi	a4,2
2005b2ae:	8ff9                	c.and	a5,a4
2005b2b0:	c7a5                	c.beqz	a5,2005b318 <app_IWDG_int+0x76>
2005b2b2:	1101                	c.addi	sp,-32
2005b2b4:	cc22                	c.swsp	s0,24(sp)
2005b2b6:	200636b7          	lui	a3,0x20063
2005b2ba:	20063437          	lui	s0,0x20063
2005b2be:	0e868693          	addi	a3,a3,232 # 200630e8 <__FUNCTION__.0+0x1b4>
2005b2c2:	04900613          	addi	a2,zero,73
2005b2c6:	0fc40593          	addi	a1,s0,252 # 200630fc <__FUNCTION__.0+0x1c8>
2005b2ca:	4511                	c.li	a0,4
2005b2cc:	ce06                	c.swsp	ra,28(sp)
2005b2ce:	3565                	c.jal	2005b176 <rtk_log_write>
2005b2d0:	2005b7b7          	lui	a5,0x2005b
2005b2d4:	200635b7          	lui	a1,0x20063
2005b2d8:	1f478793          	addi	a5,a5,500 # 2005b1f4 <app_IWDG_refresh>
2005b2dc:	4705                	c.li	a4,1
2005b2de:	1f400693          	addi	a3,zero,500
2005b2e2:	4601                	c.li	a2,0
2005b2e4:	10458593          	addi	a1,a1,260 # 20063104 <__FUNCTION__.0+0x1d0>
2005b2e8:	0068                	c.addi4spn	a0,sp,12
2005b2ea:	c602                	c.swsp	zero,12(sp)
2005b2ec:	14f000ef          	jal	ra,2005bc3a <rtos_timer_create>
2005b2f0:	4532                	c.lwsp	a0,12(sp)
2005b2f2:	ed19                	c.bnez	a0,2005b310 <app_IWDG_int+0x6e>
2005b2f4:	200636b7          	lui	a3,0x20063
2005b2f8:	11068693          	addi	a3,a3,272 # 20063110 <__FUNCTION__.0+0x1dc>
2005b2fc:	04500613          	addi	a2,zero,69
2005b300:	0fc40593          	addi	a1,s0,252
2005b304:	4509                	c.li	a0,2
2005b306:	3d85                	c.jal	2005b176 <rtk_log_write>
2005b308:	40f2                	c.lwsp	ra,28(sp)
2005b30a:	4462                	c.lwsp	s0,24(sp)
2005b30c:	6105                	c.addi16sp	sp,32
2005b30e:	8082                	c.jr	ra
2005b310:	4581                	c.li	a1,0
2005b312:	207000ef          	jal	ra,2005bd18 <rtos_timer_start>
2005b316:	bfcd                	c.j	2005b308 <app_IWDG_int+0x66>
2005b318:	8082                	c.jr	ra

2005b31a <app_pre_example>:
2005b31a:	8082                	c.jr	ra

2005b31c <app_example>:
2005b31c:	8082                	c.jr	ra

2005b31e <main>:
2005b31e:	20059537          	lui	a0,0x20059
2005b322:	1141                	c.addi	sp,-16
2005b324:	4691                	c.li	a3,4
2005b326:	41020637          	lui	a2,0x41020
2005b32a:	02b00593          	addi	a1,zero,43
2005b32e:	17e50513          	addi	a0,a0,382 # 2005917e <IPC_INTHandler>
2005b332:	c606                	c.swsp	ra,12(sp)
2005b334:	5a6040ef          	jal	ra,2005f8da <irq_register>
2005b338:	02b00513          	addi	a0,zero,43
2005b33c:	dffad097          	auipc	ra,0xdffad
2005b340:	c9c080e7          	jalr	ra,-868(ra) # 7fd8 <irq_enable>
2005b344:	35fd                	c.jal	2005b232 <app_mbedtls_rom_init>
2005b346:	4581                	c.li	a1,0
2005b348:	4501                	c.li	a0,0
2005b34a:	dffae097          	auipc	ra,0xdffae
2005b34e:	f0a080e7          	jalr	ra,-246(ra) # 9254 <shell_init_rom>
2005b352:	60b010ef          	jal	ra,2005d15c <shell_init_ram>
2005b356:	41020537          	lui	a0,0x41020
2005b35a:	a1dfd0ef          	jal	ra,20058d76 <ipc_table_init>
2005b35e:	2005c537          	lui	a0,0x2005c
2005b362:	be050513          	addi	a0,a0,-1056 # 2005bbe0 <rtos_time_delay_ms>
2005b366:	ffbfd0ef          	jal	ra,20059360 <IPC_SEMDelayStub>
2005b36a:	3f45                	c.jal	2005b31a <app_pre_example>
2005b36c:	6ae030ef          	jal	ra,2005ea1a <wlan_initialize>
2005b370:	3d51                	c.jal	2005b204 <app_init_debug>
2005b372:	35cd                	c.jal	2005b254 <app_pmu_init>
2005b374:	373d                	c.jal	2005b2a2 <app_IWDG_int>
2005b376:	375d                	c.jal	2005b31c <app_example>
2005b378:	200636b7          	lui	a3,0x20063
2005b37c:	200635b7          	lui	a1,0x20063
2005b380:	12468693          	addi	a3,a3,292 # 20063124 <__FUNCTION__.0+0x1f0>
2005b384:	04900613          	addi	a2,zero,73
2005b388:	0fc58593          	addi	a1,a1,252 # 200630fc <__FUNCTION__.0+0x1c8>
2005b38c:	4511                	c.li	a0,4
2005b38e:	de9ff0ef          	jal	ra,2005b176 <rtk_log_write>
2005b392:	7e2000ef          	jal	ra,2005bb74 <rtos_sched_start>
2005b396:	40b2                	c.lwsp	ra,12(sp)
2005b398:	4501                	c.li	a0,0
2005b39a:	0141                	c.addi	sp,16
2005b39c:	8082                	c.jr	ra

2005b39e <CmdRamHelp>:
2005b39e:	1101                	c.addi	sp,-32
2005b3a0:	cc22                	c.swsp	s0,24(sp)
2005b3a2:	ca26                	c.swsp	s1,20(sp)
2005b3a4:	20065437          	lui	s0,0x20065
2005b3a8:	200654b7          	lui	s1,0x20065
2005b3ac:	c3448793          	addi	a5,s1,-972 # 20064c34 <shell_cmd_table>
2005b3b0:	c7440413          	addi	s0,s0,-908 # 20064c74 <ipc_tickless_table>
2005b3b4:	8c1d                	c.sub	s0,a5
2005b3b6:	8011                	c.srli	s0,0x4
2005b3b8:	20063537          	lui	a0,0x20063
2005b3bc:	85a2                	c.mv	a1,s0
2005b3be:	13450513          	addi	a0,a0,308 # 20063134 <__FUNCTION__.0+0x200>
2005b3c2:	c84a                	c.swsp	s2,16(sp)
2005b3c4:	c64e                	c.swsp	s3,12(sp)
2005b3c6:	ce06                	c.swsp	ra,28(sp)
2005b3c8:	c3448493          	addi	s1,s1,-972
2005b3cc:	7c3030ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b3d0:	4901                	c.li	s2,0
2005b3d2:	200639b7          	lui	s3,0x20063
2005b3d6:	02891163          	bne	s2,s0,2005b3f8 <CmdRamHelp+0x5a>
2005b3da:	20063537          	lui	a0,0x20063
2005b3de:	85ca                	c.mv	a1,s2
2005b3e0:	17850513          	addi	a0,a0,376 # 20063178 <__FUNCTION__.0+0x244>
2005b3e4:	7ab030ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b3e8:	40f2                	c.lwsp	ra,28(sp)
2005b3ea:	4462                	c.lwsp	s0,24(sp)
2005b3ec:	44d2                	c.lwsp	s1,20(sp)
2005b3ee:	4942                	c.lwsp	s2,16(sp)
2005b3f0:	49b2                	c.lwsp	s3,12(sp)
2005b3f2:	4505                	c.li	a0,1
2005b3f4:	6105                	c.addi16sp	sp,32
2005b3f6:	8082                	c.jr	ra
2005b3f8:	44cc                	c.lw	a1,12(s1)
2005b3fa:	c589                	c.beqz	a1,2005b404 <CmdRamHelp+0x66>
2005b3fc:	17498513          	addi	a0,s3,372 # 20063174 <__FUNCTION__.0+0x240>
2005b400:	78f030ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b404:	0905                	c.addi	s2,1
2005b406:	04c1                	c.addi	s1,16
2005b408:	b7f9                	c.j	2005b3d6 <CmdRamHelp+0x38>

2005b40a <cmd_reboot>:
2005b40a:	1141                	c.addi	sp,-16
2005b40c:	c226                	c.swsp	s1,4(sp)
2005b40e:	84aa                	c.mv	s1,a0
2005b410:	20063537          	lui	a0,0x20063
2005b414:	1b850513          	addi	a0,a0,440 # 200631b8 <__FUNCTION__.0+0x284>
2005b418:	c422                	c.swsp	s0,8(sp)
2005b41a:	c606                	c.swsp	ra,12(sp)
2005b41c:	842e                	c.mv	s0,a1
2005b41e:	771030ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b422:	c08d                	c.beqz	s1,2005b444 <cmd_reboot+0x3a>
2005b424:	4008                	c.lw	a0,0(s0)
2005b426:	200635b7          	lui	a1,0x20063
2005b42a:	1c858593          	addi	a1,a1,456 # 200631c8 <__FUNCTION__.0+0x294>
2005b42e:	dffae097          	auipc	ra,0xdffae
2005b432:	3fe080e7          	jalr	ra,1022(ra) # 982c <_strcmp>
2005b436:	e519                	c.bnez	a0,2005b444 <cmd_reboot+0x3a>
2005b438:	20000593          	addi	a1,zero,512
2005b43c:	dffa5097          	auipc	ra,0xdffa5
2005b440:	46a080e7          	jalr	ra,1130(ra) # 8a6 <BKUP_Set>
2005b444:	fc4fe0ef          	jal	ra,20059c08 <System_Reset>
2005b448:	40b2                	c.lwsp	ra,12(sp)
2005b44a:	4422                	c.lwsp	s0,8(sp)
2005b44c:	4492                	c.lwsp	s1,4(sp)
2005b44e:	4505                	c.li	a0,1
2005b450:	0141                	c.addi	sp,16
2005b452:	8082                	c.jr	ra

2005b454 <cmd_write_word>:
2005b454:	1141                	c.addi	sp,-16
2005b456:	c606                	c.swsp	ra,12(sp)
2005b458:	c422                	c.swsp	s0,8(sp)
2005b45a:	c226                	c.swsp	s1,4(sp)
2005b45c:	c04a                	c.swsp	s2,0(sp)
2005b45e:	892e                	c.mv	s2,a1
2005b460:	00092503          	lw	a0,0(s2)
2005b464:	4641                	c.li	a2,16
2005b466:	4581                	c.li	a1,0
2005b468:	dffae097          	auipc	ra,0xdffae
2005b46c:	4ce080e7          	jalr	ra,1230(ra) # 9936 <_strtoul>
2005b470:	ffc57413          	andi	s0,a0,-4
2005b474:	84aa                	c.mv	s1,a0
2005b476:	00492503          	lw	a0,4(s2)
2005b47a:	4641                	c.li	a2,16
2005b47c:	4581                	c.li	a1,0
2005b47e:	dffae097          	auipc	ra,0xdffae
2005b482:	4b8080e7          	jalr	ra,1208(ra) # 9936 <_strtoul>
2005b486:	892a                	c.mv	s2,a0
2005b488:	862a                	c.mv	a2,a0
2005b48a:	20063537          	lui	a0,0x20063
2005b48e:	85a2                	c.mv	a1,s0
2005b490:	1d450513          	addi	a0,a0,468 # 200631d4 <__FUNCTION__.0+0x2a0>
2005b494:	6fb030ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b498:	01242023          	sw	s2,0(s0)
2005b49c:	7c1027f3          	csrrs	a5,0x7c1,zero
2005b4a0:	1007f793          	andi	a5,a5,256
2005b4a4:	c7a1                	c.beqz	a5,2005b4ec <cmd_write_word+0x98>
2005b4a6:	01c4f793          	andi	a5,s1,28
2005b4aa:	4711                	c.li	a4,4
2005b4ac:	c789                	c.beqz	a5,2005b4b6 <cmd_write_word+0x62>
2005b4ae:	00478713          	addi	a4,a5,4
2005b4b2:	fe04f413          	andi	s0,s1,-32
2005b4b6:	01f47793          	andi	a5,s0,31
2005b4ba:	97ba                	c.add	a5,a4
2005b4bc:	0ff0000f          	fence	iorw,iorw
2005b4c0:	97a2                	c.add	a5,s0
2005b4c2:	0484700b          	cache	dwb,(s0)
2005b4c6:	02040413          	addi	s0,s0,32
2005b4ca:	40878733          	sub	a4,a5,s0
2005b4ce:	fee04ae3          	blt	zero,a4,2005b4c2 <cmd_write_word+0x6e>
2005b4d2:	0ff0000f          	fence	iorw,iorw
2005b4d6:	0001                	c.addi	zero,0
2005b4d8:	0001                	c.addi	zero,0
2005b4da:	0001                	c.addi	zero,0
2005b4dc:	0001                	c.addi	zero,0
2005b4de:	0001                	c.addi	zero,0
2005b4e0:	0001                	c.addi	zero,0
2005b4e2:	0001                	c.addi	zero,0
2005b4e4:	0001                	c.addi	zero,0
2005b4e6:	0001                	c.addi	zero,0
2005b4e8:	0001                	c.addi	zero,0
2005b4ea:	0001                	c.addi	zero,0
2005b4ec:	40b2                	c.lwsp	ra,12(sp)
2005b4ee:	4422                	c.lwsp	s0,8(sp)
2005b4f0:	4492                	c.lwsp	s1,4(sp)
2005b4f2:	4902                	c.lwsp	s2,0(sp)
2005b4f4:	4501                	c.li	a0,0
2005b4f6:	0141                	c.addi	sp,16
2005b4f8:	8082                	c.jr	ra

2005b4fa <cmd_dump_word>:
2005b4fa:	1101                	c.addi	sp,-32
2005b4fc:	cc22                	c.swsp	s0,24(sp)
2005b4fe:	c84a                	c.swsp	s2,16(sp)
2005b500:	842a                	c.mv	s0,a0
2005b502:	ce06                	c.swsp	ra,28(sp)
2005b504:	ca26                	c.swsp	s1,20(sp)
2005b506:	c64e                	c.swsp	s3,12(sp)
2005b508:	892e                	c.mv	s2,a1
2005b50a:	dffab097          	auipc	ra,0xdffab
2005b50e:	644080e7          	jalr	ra,1604(ra) # 6b4e <SYSCFG_OTP_RSIPEn>
2005b512:	fff40793          	addi	a5,s0,-1
2005b516:	07c2                	c.slli	a5,0x10
2005b518:	83c1                	c.srli	a5,0x10
2005b51a:	4709                	c.li	a4,2
2005b51c:	00f77a63          	bgeu	a4,a5,2005b530 <cmd_dump_word+0x36>
2005b520:	20063537          	lui	a0,0x20063
2005b524:	1e450513          	addi	a0,a0,484 # 200631e4 <__FUNCTION__.0+0x2b0>
2005b528:	667030ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b52c:	4401                	c.li	s0,0
2005b52e:	a0f9                	c.j	2005b5fc <cmd_dump_word+0x102>
2005b530:	478d                	c.li	a5,3
2005b532:	89aa                	c.mv	s3,a0
2005b534:	06f41063          	bne	s0,a5,2005b594 <cmd_dump_word+0x9a>
2005b538:	00892783          	lw	a5,8(s2)
2005b53c:	0007c403          	lbu	s0,0(a5)
2005b540:	0df47413          	andi	s0,s0,223
2005b544:	fbe40413          	addi	s0,s0,-66
2005b548:	00143413          	sltiu	s0,s0,1
2005b54c:	00492503          	lw	a0,4(s2)
2005b550:	4629                	c.li	a2,10
2005b552:	4581                	c.li	a1,0
2005b554:	dffae097          	auipc	ra,0xdffae
2005b558:	3e2080e7          	jalr	ra,994(ra) # 9936 <_strtoul>
2005b55c:	84aa                	c.mv	s1,a0
2005b55e:	00092503          	lw	a0,0(s2)
2005b562:	4641                	c.li	a2,16
2005b564:	4581                	c.li	a1,0
2005b566:	dffae097          	auipc	ra,0xdffae
2005b56a:	3d0080e7          	jalr	ra,976(ra) # 9936 <_strtoul>
2005b56e:	87aa                	c.mv	a5,a0
2005b570:	f8000737          	lui	a4,0xf8000
2005b574:	9971                	c.andi	a0,-4
2005b576:	972a                	c.add	a4,a0
2005b578:	080006b7          	lui	a3,0x8000
2005b57c:	02d77463          	bgeu	a4,a3,2005b5a4 <cmd_dump_word+0xaa>
2005b580:	02098263          	beq	s3,zero,2005b5a4 <cmd_dump_word+0xaa>
2005b584:	20063537          	lui	a0,0x20063
2005b588:	20050513          	addi	a0,a0,512 # 20063200 <__FUNCTION__.0+0x2cc>
2005b58c:	603030ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b590:	4405                	c.li	s0,1
2005b592:	a0ad                	c.j	2005b5fc <cmd_dump_word+0x102>
2005b594:	4785                	c.li	a5,1
2005b596:	00f40463          	beq	s0,a5,2005b59e <cmd_dump_word+0xa4>
2005b59a:	4401                	c.li	s0,0
2005b59c:	bf45                	c.j	2005b54c <cmd_dump_word+0x52>
2005b59e:	4401                	c.li	s0,0
2005b5a0:	4485                	c.li	s1,1
2005b5a2:	bf75                	c.j	2005b55e <cmd_dump_word+0x64>
2005b5a4:	c43d                	c.beqz	s0,2005b612 <cmd_dump_word+0x118>
2005b5a6:	c4f9                	c.beqz	s1,2005b674 <cmd_dump_word+0x17a>
2005b5a8:	7c102773          	csrrs	a4,0x7c1,zero
2005b5ac:	10077713          	andi	a4,a4,256
2005b5b0:	c339                	c.beqz	a4,2005b5f6 <cmd_dump_word+0xfc>
2005b5b2:	01c7f713          	andi	a4,a5,28
2005b5b6:	cb39                	c.beqz	a4,2005b60c <cmd_dump_word+0x112>
2005b5b8:	9b81                	c.andi	a5,-32
2005b5ba:	9726                	c.add	a4,s1
2005b5bc:	02e05d63          	bge	zero,a4,2005b5f6 <cmd_dump_word+0xfc>
2005b5c0:	01f7f693          	andi	a3,a5,31
2005b5c4:	9736                	c.add	a4,a3
2005b5c6:	0ff0000f          	fence	iorw,iorw
2005b5ca:	973e                	c.add	a4,a5
2005b5cc:	02f7f00b          	cache	dwbinv,(a5)
2005b5d0:	02078793          	addi	a5,a5,32
2005b5d4:	40f706b3          	sub	a3,a4,a5
2005b5d8:	fed04ae3          	blt	zero,a3,2005b5cc <cmd_dump_word+0xd2>
2005b5dc:	0ff0000f          	fence	iorw,iorw
2005b5e0:	0001                	c.addi	zero,0
2005b5e2:	0001                	c.addi	zero,0
2005b5e4:	0001                	c.addi	zero,0
2005b5e6:	0001                	c.addi	zero,0
2005b5e8:	0001                	c.addi	zero,0
2005b5ea:	0001                	c.addi	zero,0
2005b5ec:	0001                	c.addi	zero,0
2005b5ee:	0001                	c.addi	zero,0
2005b5f0:	0001                	c.addi	zero,0
2005b5f2:	0001                	c.addi	zero,0
2005b5f4:	0001                	c.addi	zero,0
2005b5f6:	85a6                	c.mv	a1,s1
2005b5f8:	b0bff0ef          	jal	ra,2005b102 <rtk_log_memory_dump_byte>
2005b5fc:	40f2                	c.lwsp	ra,28(sp)
2005b5fe:	8522                	c.mv	a0,s0
2005b600:	4462                	c.lwsp	s0,24(sp)
2005b602:	44d2                	c.lwsp	s1,20(sp)
2005b604:	4942                	c.lwsp	s2,16(sp)
2005b606:	49b2                	c.lwsp	s3,12(sp)
2005b608:	6105                	c.addi16sp	sp,32
2005b60a:	8082                	c.jr	ra
2005b60c:	8726                	c.mv	a4,s1
2005b60e:	87aa                	c.mv	a5,a0
2005b610:	b775                	c.j	2005b5bc <cmd_dump_word+0xc2>
2005b612:	c0ad                	c.beqz	s1,2005b674 <cmd_dump_word+0x17a>
2005b614:	7c102773          	csrrs	a4,0x7c1,zero
2005b618:	10077713          	andi	a4,a4,256
2005b61c:	cb21                	c.beqz	a4,2005b66c <cmd_dump_word+0x172>
2005b61e:	01c7f693          	andi	a3,a5,28
2005b622:	00249613          	slli	a2,s1,0x2
2005b626:	872a                	c.mv	a4,a0
2005b628:	c681                	c.beqz	a3,2005b630 <cmd_dump_word+0x136>
2005b62a:	fe07f713          	andi	a4,a5,-32
2005b62e:	9636                	c.add	a2,a3
2005b630:	02c05e63          	bge	zero,a2,2005b66c <cmd_dump_word+0x172>
2005b634:	01f77693          	andi	a3,a4,31
2005b638:	96b2                	c.add	a3,a2
2005b63a:	0ff0000f          	fence	iorw,iorw
2005b63e:	87ba                	c.mv	a5,a4
2005b640:	9736                	c.add	a4,a3
2005b642:	02f7f00b          	cache	dwbinv,(a5)
2005b646:	02078793          	addi	a5,a5,32
2005b64a:	40f706b3          	sub	a3,a4,a5
2005b64e:	fed04ae3          	blt	zero,a3,2005b642 <cmd_dump_word+0x148>
2005b652:	0ff0000f          	fence	iorw,iorw
2005b656:	0001                	c.addi	zero,0
2005b658:	0001                	c.addi	zero,0
2005b65a:	0001                	c.addi	zero,0
2005b65c:	0001                	c.addi	zero,0
2005b65e:	0001                	c.addi	zero,0
2005b660:	0001                	c.addi	zero,0
2005b662:	0001                	c.addi	zero,0
2005b664:	0001                	c.addi	zero,0
2005b666:	0001                	c.addi	zero,0
2005b668:	0001                	c.addi	zero,0
2005b66a:	0001                	c.addi	zero,0
2005b66c:	85a6                	c.mv	a1,s1
2005b66e:	a23ff0ef          	jal	ra,2005b090 <rtk_log_memory_dump_word>
2005b672:	bf39                	c.j	2005b590 <cmd_dump_word+0x96>
2005b674:	20063537          	lui	a0,0x20063
2005b678:	22450513          	addi	a0,a0,548 # 20063224 <__FUNCTION__.0+0x2f0>
2005b67c:	b575                	c.j	2005b528 <cmd_dump_word+0x2e>

2005b67e <rtos_critical_is_in_interrupt>:
2005b67e:	1141                	c.addi	sp,-16
2005b680:	c606                	c.swsp	ra,12(sp)
2005b682:	6d10a0ef          	jal	ra,20066552 <plic_get_active_irq_id>
2005b686:	40b2                	c.lwsp	ra,12(sp)
2005b688:	00a03533          	sltu	a0,zero,a0
2005b68c:	0141                	c.addi	sp,16
2005b68e:	8082                	c.jr	ra

2005b690 <rtos_critical_enter>:
2005b690:	1141                	c.addi	sp,-16
2005b692:	c606                	c.swsp	ra,12(sp)
2005b694:	37ed                	c.jal	2005b67e <rtos_critical_is_in_interrupt>
2005b696:	c919                	c.beqz	a0,2005b6ac <rtos_critical_enter+0x1c>
2005b698:	ef418793          	addi	a5,gp,-268 # 2006687c <uxCriticalNestingCnt>
2005b69c:	0007d703          	lhu	a4,0(a5)
2005b6a0:	0705                	c.addi	a4,1
2005b6a2:	00e79023          	sh	a4,0(a5)
2005b6a6:	40b2                	c.lwsp	ra,12(sp)
2005b6a8:	0141                	c.addi	sp,16
2005b6aa:	8082                	c.jr	ra
2005b6ac:	40b2                	c.lwsp	ra,12(sp)
2005b6ae:	0141                	c.addi	sp,16
2005b6b0:	5520206f          	jal	zero,2005dc02 <vTaskEnterCritical>

2005b6b4 <rtos_critical_exit>:
2005b6b4:	1141                	c.addi	sp,-16
2005b6b6:	c606                	c.swsp	ra,12(sp)
2005b6b8:	37d9                	c.jal	2005b67e <rtos_critical_is_in_interrupt>
2005b6ba:	c919                	c.beqz	a0,2005b6d0 <rtos_critical_exit+0x1c>
2005b6bc:	ef418793          	addi	a5,gp,-268 # 2006687c <uxCriticalNestingCnt>
2005b6c0:	0007d703          	lhu	a4,0(a5)
2005b6c4:	177d                	c.addi	a4,-1
2005b6c6:	00e79023          	sh	a4,0(a5)
2005b6ca:	40b2                	c.lwsp	ra,12(sp)
2005b6cc:	0141                	c.addi	sp,16
2005b6ce:	8082                	c.jr	ra
2005b6d0:	40b2                	c.lwsp	ra,12(sp)
2005b6d2:	0141                	c.addi	sp,16
2005b6d4:	5480206f          	jal	zero,2005dc1c <vTaskExitCritical>

2005b6d8 <rtos_mem_init>:
2005b6d8:	bccff06f          	jal	zero,2005aaa4 <os_heap_init>

2005b6dc <rtos_mem_malloc>:
2005b6dc:	d2eff06f          	jal	zero,2005ac0a <pvPortMalloc>

2005b6e0 <rtos_mem_zmalloc>:
2005b6e0:	1101                	c.addi	sp,-32
2005b6e2:	cc22                	c.swsp	s0,24(sp)
2005b6e4:	ce06                	c.swsp	ra,28(sp)
2005b6e6:	c62a                	c.swsp	a0,12(sp)
2005b6e8:	d22ff0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005b6ec:	842a                	c.mv	s0,a0
2005b6ee:	c519                	c.beqz	a0,2005b6fc <rtos_mem_zmalloc+0x1c>
2005b6f0:	4632                	c.lwsp	a2,12(sp)
2005b6f2:	4581                	c.li	a1,0
2005b6f4:	dffae097          	auipc	ra,0xdffae
2005b6f8:	3da080e7          	jalr	ra,986(ra) # 9ace <__wrap_memset>
2005b6fc:	40f2                	c.lwsp	ra,28(sp)
2005b6fe:	8522                	c.mv	a0,s0
2005b700:	4462                	c.lwsp	s0,24(sp)
2005b702:	6105                	c.addi16sp	sp,32
2005b704:	8082                	c.jr	ra

2005b706 <rtos_mem_free>:
2005b706:	c119                	c.beqz	a0,2005b70c <rtos_mem_free+0x6>
2005b708:	de0ff06f          	jal	zero,2005ace8 <vPortFree>
2005b70c:	8082                	c.jr	ra

2005b70e <rtos_mem_get_free_heap_size>:
2005b70e:	e38ff06f          	jal	zero,2005ad46 <xPortGetFreeHeapSize>

2005b712 <rtos_mutex_create>:
2005b712:	c10d                	c.beqz	a0,2005b734 <rtos_mutex_create+0x22>
2005b714:	1141                	c.addi	sp,-16
2005b716:	c422                	c.swsp	s0,8(sp)
2005b718:	842a                	c.mv	s0,a0
2005b71a:	4505                	c.li	a0,1
2005b71c:	c606                	c.swsp	ra,12(sp)
2005b71e:	6d4010ef          	jal	ra,2005cdf2 <xQueueCreateMutex>
2005b722:	c008                	c.sw	a0,0(s0)
2005b724:	40b2                	c.lwsp	ra,12(sp)
2005b726:	4422                	c.lwsp	s0,8(sp)
2005b728:	00153513          	sltiu	a0,a0,1
2005b72c:	40a00533          	sub	a0,zero,a0
2005b730:	0141                	c.addi	sp,16
2005b732:	8082                	c.jr	ra
2005b734:	557d                	c.li	a0,-1
2005b736:	8082                	c.jr	ra

2005b738 <rtos_mutex_create_static>:
2005b738:	1141                	c.addi	sp,-16
2005b73a:	c422                	c.swsp	s0,8(sp)
2005b73c:	c606                	c.swsp	ra,12(sp)
2005b73e:	842a                	c.mv	s0,a0
2005b740:	2e4d                	c.jal	2005baf2 <__reserved_get_mutex_from_poll>
2005b742:	e511                	c.bnez	a0,2005b74e <rtos_mutex_create_static+0x16>
2005b744:	8522                	c.mv	a0,s0
2005b746:	4422                	c.lwsp	s0,8(sp)
2005b748:	40b2                	c.lwsp	ra,12(sp)
2005b74a:	0141                	c.addi	sp,16
2005b74c:	b7d9                	c.j	2005b712 <rtos_mutex_create>
2005b74e:	85aa                	c.mv	a1,a0
2005b750:	4505                	c.li	a0,1
2005b752:	72c010ef          	jal	ra,2005ce7e <xQueueCreateMutexStatic>
2005b756:	c008                	c.sw	a0,0(s0)
2005b758:	40b2                	c.lwsp	ra,12(sp)
2005b75a:	4422                	c.lwsp	s0,8(sp)
2005b75c:	00153513          	sltiu	a0,a0,1
2005b760:	40a00533          	sub	a0,zero,a0
2005b764:	0141                	c.addi	sp,16
2005b766:	8082                	c.jr	ra

2005b768 <rtos_mutex_delete>:
2005b768:	c139                	c.beqz	a0,2005b7ae <rtos_mutex_delete+0x46>
2005b76a:	1141                	c.addi	sp,-16
2005b76c:	c422                	c.swsp	s0,8(sp)
2005b76e:	c606                	c.swsp	ra,12(sp)
2005b770:	842a                	c.mv	s0,a0
2005b772:	12a010ef          	jal	ra,2005c89c <xQueueGetMutexHolder>
2005b776:	e909                	c.bnez	a0,2005b788 <rtos_mutex_delete+0x20>
2005b778:	8522                	c.mv	a0,s0
2005b77a:	786010ef          	jal	ra,2005cf00 <vQueueDelete>
2005b77e:	4501                	c.li	a0,0
2005b780:	40b2                	c.lwsp	ra,12(sp)
2005b782:	4422                	c.lwsp	s0,8(sp)
2005b784:	0141                	c.addi	sp,16
2005b786:	8082                	c.jr	ra
2005b788:	8522                	c.mv	a0,s0
2005b78a:	776010ef          	jal	ra,2005cf00 <vQueueDelete>
2005b78e:	4501                	c.li	a0,0
2005b790:	765010ef          	jal	ra,2005d6f4 <pcTaskGetName>
2005b794:	85aa                	c.mv	a1,a0
2005b796:	20063637          	lui	a2,0x20063
2005b79a:	20063537          	lui	a0,0x20063
2005b79e:	49860613          	addi	a2,a2,1176 # 20063498 <__FUNCTION__.0>
2005b7a2:	44450513          	addi	a0,a0,1092 # 20063444 <__FUNCTION__.0+0x510>
2005b7a6:	3e9030ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005b7aa:	557d                	c.li	a0,-1
2005b7ac:	bfd1                	c.j	2005b780 <rtos_mutex_delete+0x18>
2005b7ae:	557d                	c.li	a0,-1
2005b7b0:	8082                	c.jr	ra

2005b7b2 <rtos_mutex_delete_static>:
2005b7b2:	1141                	c.addi	sp,-16
2005b7b4:	c606                	c.swsp	ra,12(sp)
2005b7b6:	c422                	c.swsp	s0,8(sp)
2005b7b8:	c226                	c.swsp	s1,4(sp)
2005b7ba:	84aa                	c.mv	s1,a0
2005b7bc:	3775                	c.jal	2005b768 <rtos_mutex_delete>
2005b7be:	842a                	c.mv	s0,a0
2005b7c0:	8526                	c.mv	a0,s1
2005b7c2:	26a9                	c.jal	2005bb0c <__reserved_release_mutex_to_poll>
2005b7c4:	40b2                	c.lwsp	ra,12(sp)
2005b7c6:	8522                	c.mv	a0,s0
2005b7c8:	4422                	c.lwsp	s0,8(sp)
2005b7ca:	4492                	c.lwsp	s1,4(sp)
2005b7cc:	0141                	c.addi	sp,16
2005b7ce:	8082                	c.jr	ra

2005b7d0 <rtos_mutex_take>:
2005b7d0:	7179                	c.addi16sp	sp,-48
2005b7d2:	d422                	c.swsp	s0,40(sp)
2005b7d4:	c62e                	c.swsp	a1,12(sp)
2005b7d6:	d606                	c.swsp	ra,44(sp)
2005b7d8:	842a                	c.mv	s0,a0
2005b7da:	ce02                	c.swsp	zero,28(sp)
2005b7dc:	ea3ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005b7e0:	45b2                	c.lwsp	a1,12(sp)
2005b7e2:	c505                	c.beqz	a0,2005b80a <rtos_mutex_take+0x3a>
2005b7e4:	0870                	c.addi4spn	a2,sp,28
2005b7e6:	4581                	c.li	a1,0
2005b7e8:	8522                	c.mv	a0,s0
2005b7ea:	4a6010ef          	jal	ra,2005cc90 <xQueueReceiveFromISR>
2005b7ee:	872a                	c.mv	a4,a0
2005b7f0:	4785                	c.li	a5,1
2005b7f2:	557d                	c.li	a0,-1
2005b7f4:	00f71763          	bne	a4,a5,2005b802 <rtos_mutex_take+0x32>
2005b7f8:	4572                	c.lwsp	a0,28(sp)
2005b7fa:	c501                	c.beqz	a0,2005b802 <rtos_mutex_take+0x32>
2005b7fc:	03a020ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005b800:	4501                	c.li	a0,0
2005b802:	50b2                	c.lwsp	ra,44(sp)
2005b804:	5422                	c.lwsp	s0,40(sp)
2005b806:	6145                	c.addi16sp	sp,48
2005b808:	8082                	c.jr	ra
2005b80a:	8522                	c.mv	a0,s0
2005b80c:	366010ef          	jal	ra,2005cb72 <xQueueSemaphoreTake>
2005b810:	157d                	c.addi	a0,-1
2005b812:	00a03533          	sltu	a0,zero,a0
2005b816:	40a00533          	sub	a0,zero,a0
2005b81a:	b7e5                	c.j	2005b802 <rtos_mutex_take+0x32>

2005b81c <rtos_mutex_give>:
2005b81c:	1101                	c.addi	sp,-32
2005b81e:	cc22                	c.swsp	s0,24(sp)
2005b820:	ce06                	c.swsp	ra,28(sp)
2005b822:	842a                	c.mv	s0,a0
2005b824:	c602                	c.swsp	zero,12(sp)
2005b826:	e59ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005b82a:	c11d                	c.beqz	a0,2005b850 <rtos_mutex_give+0x34>
2005b82c:	006c                	c.addi4spn	a1,sp,12
2005b82e:	8522                	c.mv	a0,s0
2005b830:	1fe010ef          	jal	ra,2005ca2e <xQueueGiveFromISR>
2005b834:	872a                	c.mv	a4,a0
2005b836:	4785                	c.li	a5,1
2005b838:	557d                	c.li	a0,-1
2005b83a:	00f71763          	bne	a4,a5,2005b848 <rtos_mutex_give+0x2c>
2005b83e:	4532                	c.lwsp	a0,12(sp)
2005b840:	c501                	c.beqz	a0,2005b848 <rtos_mutex_give+0x2c>
2005b842:	7f5010ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005b846:	4501                	c.li	a0,0
2005b848:	40f2                	c.lwsp	ra,28(sp)
2005b84a:	4462                	c.lwsp	s0,24(sp)
2005b84c:	6105                	c.addi16sp	sp,32
2005b84e:	8082                	c.jr	ra
2005b850:	4681                	c.li	a3,0
2005b852:	4601                	c.li	a2,0
2005b854:	4581                	c.li	a1,0
2005b856:	8522                	c.mv	a0,s0
2005b858:	068010ef          	jal	ra,2005c8c0 <xQueueGenericSend>
2005b85c:	157d                	c.addi	a0,-1
2005b85e:	00a03533          	sltu	a0,zero,a0
2005b862:	40a00533          	sub	a0,zero,a0
2005b866:	b7cd                	c.j	2005b848 <rtos_mutex_give+0x2c>

2005b868 <rtos_sema_create>:
2005b868:	1141                	c.addi	sp,-16
2005b86a:	c422                	c.swsp	s0,8(sp)
2005b86c:	c606                	c.swsp	ra,12(sp)
2005b86e:	842a                	c.mv	s0,a0
2005b870:	557d                	c.li	a0,-1
2005b872:	c809                	c.beqz	s0,2005b884 <rtos_sema_create+0x1c>
2005b874:	8532                	c.mv	a0,a2
2005b876:	5b0010ef          	jal	ra,2005ce26 <xQueueCreateCountingSemaphore>
2005b87a:	c008                	c.sw	a0,0(s0)
2005b87c:	00153513          	sltiu	a0,a0,1
2005b880:	40a00533          	sub	a0,zero,a0
2005b884:	40b2                	c.lwsp	ra,12(sp)
2005b886:	4422                	c.lwsp	s0,8(sp)
2005b888:	0141                	c.addi	sp,16
2005b88a:	8082                	c.jr	ra

2005b88c <rtos_sema_create_static>:
2005b88c:	1101                	c.addi	sp,-32
2005b88e:	cc22                	c.swsp	s0,24(sp)
2005b890:	ca26                	c.swsp	s1,20(sp)
2005b892:	c62e                	c.swsp	a1,12(sp)
2005b894:	ce06                	c.swsp	ra,28(sp)
2005b896:	842a                	c.mv	s0,a0
2005b898:	84b2                	c.mv	s1,a2
2005b89a:	2471                	c.jal	2005bb26 <__reserved_get_sema_from_poll>
2005b89c:	45b2                	c.lwsp	a1,12(sp)
2005b89e:	e901                	c.bnez	a0,2005b8ae <rtos_sema_create_static+0x22>
2005b8a0:	8522                	c.mv	a0,s0
2005b8a2:	4462                	c.lwsp	s0,24(sp)
2005b8a4:	40f2                	c.lwsp	ra,28(sp)
2005b8a6:	8626                	c.mv	a2,s1
2005b8a8:	44d2                	c.lwsp	s1,20(sp)
2005b8aa:	6105                	c.addi16sp	sp,32
2005b8ac:	bf75                	c.j	2005b868 <rtos_sema_create>
2005b8ae:	862a                	c.mv	a2,a0
2005b8b0:	8526                	c.mv	a0,s1
2005b8b2:	604010ef          	jal	ra,2005ceb6 <xQueueCreateCountingSemaphoreStatic>
2005b8b6:	c008                	c.sw	a0,0(s0)
2005b8b8:	40f2                	c.lwsp	ra,28(sp)
2005b8ba:	4462                	c.lwsp	s0,24(sp)
2005b8bc:	00153513          	sltiu	a0,a0,1
2005b8c0:	44d2                	c.lwsp	s1,20(sp)
2005b8c2:	40a00533          	sub	a0,zero,a0
2005b8c6:	6105                	c.addi16sp	sp,32
2005b8c8:	8082                	c.jr	ra

2005b8ca <rtos_sema_create_binary>:
2005b8ca:	c11d                	c.beqz	a0,2005b8f0 <rtos_sema_create_binary+0x26>
2005b8cc:	1141                	c.addi	sp,-16
2005b8ce:	c422                	c.swsp	s0,8(sp)
2005b8d0:	460d                	c.li	a2,3
2005b8d2:	842a                	c.mv	s0,a0
2005b8d4:	4581                	c.li	a1,0
2005b8d6:	4505                	c.li	a0,1
2005b8d8:	c606                	c.swsp	ra,12(sp)
2005b8da:	4c0010ef          	jal	ra,2005cd9a <xQueueGenericCreate>
2005b8de:	c008                	c.sw	a0,0(s0)
2005b8e0:	40b2                	c.lwsp	ra,12(sp)
2005b8e2:	4422                	c.lwsp	s0,8(sp)
2005b8e4:	00153513          	sltiu	a0,a0,1
2005b8e8:	40a00533          	sub	a0,zero,a0
2005b8ec:	0141                	c.addi	sp,16
2005b8ee:	8082                	c.jr	ra
2005b8f0:	557d                	c.li	a0,-1
2005b8f2:	8082                	c.jr	ra

2005b8f4 <rtos_sema_delete>:
2005b8f4:	c909                	c.beqz	a0,2005b906 <rtos_sema_delete+0x12>
2005b8f6:	1141                	c.addi	sp,-16
2005b8f8:	c606                	c.swsp	ra,12(sp)
2005b8fa:	606010ef          	jal	ra,2005cf00 <vQueueDelete>
2005b8fe:	40b2                	c.lwsp	ra,12(sp)
2005b900:	4501                	c.li	a0,0
2005b902:	0141                	c.addi	sp,16
2005b904:	8082                	c.jr	ra
2005b906:	557d                	c.li	a0,-1
2005b908:	8082                	c.jr	ra

2005b90a <rtos_sema_take>:
2005b90a:	7179                	c.addi16sp	sp,-48
2005b90c:	d422                	c.swsp	s0,40(sp)
2005b90e:	c62e                	c.swsp	a1,12(sp)
2005b910:	d606                	c.swsp	ra,44(sp)
2005b912:	842a                	c.mv	s0,a0
2005b914:	ce02                	c.swsp	zero,28(sp)
2005b916:	d69ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005b91a:	45b2                	c.lwsp	a1,12(sp)
2005b91c:	c505                	c.beqz	a0,2005b944 <rtos_sema_take+0x3a>
2005b91e:	0870                	c.addi4spn	a2,sp,28
2005b920:	4581                	c.li	a1,0
2005b922:	8522                	c.mv	a0,s0
2005b924:	36c010ef          	jal	ra,2005cc90 <xQueueReceiveFromISR>
2005b928:	872a                	c.mv	a4,a0
2005b92a:	4785                	c.li	a5,1
2005b92c:	557d                	c.li	a0,-1
2005b92e:	00f71763          	bne	a4,a5,2005b93c <rtos_sema_take+0x32>
2005b932:	4572                	c.lwsp	a0,28(sp)
2005b934:	c501                	c.beqz	a0,2005b93c <rtos_sema_take+0x32>
2005b936:	701010ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005b93a:	4501                	c.li	a0,0
2005b93c:	50b2                	c.lwsp	ra,44(sp)
2005b93e:	5422                	c.lwsp	s0,40(sp)
2005b940:	6145                	c.addi16sp	sp,48
2005b942:	8082                	c.jr	ra
2005b944:	8522                	c.mv	a0,s0
2005b946:	22c010ef          	jal	ra,2005cb72 <xQueueSemaphoreTake>
2005b94a:	157d                	c.addi	a0,-1
2005b94c:	00a03533          	sltu	a0,zero,a0
2005b950:	40a00533          	sub	a0,zero,a0
2005b954:	b7e5                	c.j	2005b93c <rtos_sema_take+0x32>

2005b956 <rtos_sema_give>:
2005b956:	7179                	c.addi16sp	sp,-48
2005b958:	d422                	c.swsp	s0,40(sp)
2005b95a:	d606                	c.swsp	ra,44(sp)
2005b95c:	842a                	c.mv	s0,a0
2005b95e:	ce02                	c.swsp	zero,28(sp)
2005b960:	d1fff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005b964:	c505                	c.beqz	a0,2005b98c <rtos_sema_give+0x36>
2005b966:	086c                	c.addi4spn	a1,sp,28
2005b968:	8522                	c.mv	a0,s0
2005b96a:	0c4010ef          	jal	ra,2005ca2e <xQueueGiveFromISR>
2005b96e:	47f2                	c.lwsp	a5,28(sp)
2005b970:	c789                	c.beqz	a5,2005b97a <rtos_sema_give+0x24>
2005b972:	c62a                	c.swsp	a0,12(sp)
2005b974:	6c3010ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005b978:	4532                	c.lwsp	a0,12(sp)
2005b97a:	50b2                	c.lwsp	ra,44(sp)
2005b97c:	5422                	c.lwsp	s0,40(sp)
2005b97e:	157d                	c.addi	a0,-1
2005b980:	00a03533          	sltu	a0,zero,a0
2005b984:	40a00533          	sub	a0,zero,a0
2005b988:	6145                	c.addi16sp	sp,48
2005b98a:	8082                	c.jr	ra
2005b98c:	4681                	c.li	a3,0
2005b98e:	4601                	c.li	a2,0
2005b990:	4581                	c.li	a1,0
2005b992:	8522                	c.mv	a0,s0
2005b994:	72d000ef          	jal	ra,2005c8c0 <xQueueGenericSend>
2005b998:	b7cd                	c.j	2005b97a <rtos_sema_give+0x24>

2005b99a <__reserved_get_from_poll>:
2005b99a:	7179                	c.addi16sp	sp,-48
2005b99c:	d226                	c.swsp	s1,36(sp)
2005b99e:	d04a                	c.swsp	s2,32(sp)
2005b9a0:	ce4e                	c.swsp	s3,28(sp)
2005b9a2:	cc52                	c.swsp	s4,24(sp)
2005b9a4:	ca56                	c.swsp	s5,20(sp)
2005b9a6:	c85a                	c.swsp	s6,16(sp)
2005b9a8:	d606                	c.swsp	ra,44(sp)
2005b9aa:	d422                	c.swsp	s0,40(sp)
2005b9ac:	c65e                	c.swsp	s7,12(sp)
2005b9ae:	8a3e                	c.mv	s4,a5
2005b9b0:	421c                	c.lw	a5,0(a2)
2005b9b2:	84aa                	c.mv	s1,a0
2005b9b4:	8b2e                	c.mv	s6,a1
2005b9b6:	8ab2                	c.mv	s5,a2
2005b9b8:	8936                	c.mv	s2,a3
2005b9ba:	89ba                	c.mv	s3,a4
2005b9bc:	ef85                	c.bnez	a5,2005b9f4 <__reserved_get_from_poll+0x5a>
2005b9be:	4405                	c.li	s0,1
2005b9c0:	08851263          	bne	a0,s0,2005ba44 <__reserved_get_from_poll+0xaa>
2005b9c4:	f0418413          	addi	s0,gp,-252 # 2006688c <mutex_pool_init_flag>
2005b9c8:	401c                	c.lw	a5,0(s0)
2005b9ca:	02a78563          	beq	a5,a0,2005b9f4 <__reserved_get_from_poll+0x5a>
2005b9ce:	4601                	c.li	a2,0
2005b9d0:	4581                	c.li	a1,0
2005b9d2:	5d418513          	addi	a0,gp,1492 # 20066f5c <mutex_pool>
2005b9d6:	dffae097          	auipc	ra,0xdffae
2005b9da:	0f8080e7          	jalr	ra,248(ra) # 9ace <__wrap_memset>
2005b9de:	ee01ac23          	sw	zero,-264(gp) # 20066880 <mutex_buf_used_num>
2005b9e2:	f2818513          	addi	a0,gp,-216 # 200668b0 <wrapper_mutex_buf_list>
2005b9e6:	f001a023          	sw	zero,-256(gp) # 20066888 <mutex_max_buf_used_num>
2005b9ea:	c108                	c.sw	a0,0(a0)
2005b9ec:	c148                	c.sw	a0,4(a0)
2005b9ee:	ee01ae23          	sw	zero,-260(gp) # 20066884 <mutex_dynamic_num>
2005b9f2:	c004                	c.sw	s1,0(s0)
2005b9f4:	c9dff0ef          	jal	ra,2005b690 <rtos_critical_enter>
2005b9f8:	000b2403          	lw	s0,0(s6)
2005b9fc:	0a8b1b63          	bne	s6,s0,2005bab2 <__reserved_get_from_poll+0x118>
2005ba00:	cb5ff0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
2005ba04:	4b01                	c.li	s6,0
2005ba06:	000aa783          	lw	a5,0(s5)
2005ba0a:	c791                	c.beqz	a5,2005ba16 <__reserved_get_from_poll+0x7c>
2005ba0c:	00092783          	lw	a5,0(s2)
2005ba10:	0785                	c.addi	a5,1
2005ba12:	00f92023          	sw	a5,0(s2)
2005ba16:	00092703          	lw	a4,0(s2)
2005ba1a:	0009a783          	lw	a5,0(s3)
2005ba1e:	97ba                	c.add	a5,a4
2005ba20:	000a2703          	lw	a4,0(s4)
2005ba24:	00f77463          	bgeu	a4,a5,2005ba2c <__reserved_get_from_poll+0x92>
2005ba28:	00fa2023          	sw	a5,0(s4)
2005ba2c:	50b2                	c.lwsp	ra,44(sp)
2005ba2e:	5422                	c.lwsp	s0,40(sp)
2005ba30:	5492                	c.lwsp	s1,36(sp)
2005ba32:	5902                	c.lwsp	s2,32(sp)
2005ba34:	49f2                	c.lwsp	s3,28(sp)
2005ba36:	4a62                	c.lwsp	s4,24(sp)
2005ba38:	4ad2                	c.lwsp	s5,20(sp)
2005ba3a:	4bb2                	c.lwsp	s7,12(sp)
2005ba3c:	855a                	c.mv	a0,s6
2005ba3e:	4b42                	c.lwsp	s6,16(sp)
2005ba40:	6145                	c.addi16sp	sp,48
2005ba42:	8082                	c.jr	ra
2005ba44:	4789                	c.li	a5,2
2005ba46:	02f51d63          	bne	a0,a5,2005ba80 <__reserved_get_from_poll+0xe6>
2005ba4a:	f1418b93          	addi	s7,gp,-236 # 2006689c <sema_pool_init_flag>
2005ba4e:	000ba783          	lw	a5,0(s7)
2005ba52:	fa8781e3          	beq	a5,s0,2005b9f4 <__reserved_get_from_poll+0x5a>
2005ba56:	4601                	c.li	a2,0
2005ba58:	4581                	c.li	a1,0
2005ba5a:	5d418513          	addi	a0,gp,1492 # 20066f5c <mutex_pool>
2005ba5e:	dffae097          	auipc	ra,0xdffae
2005ba62:	070080e7          	jalr	ra,112(ra) # 9ace <__wrap_memset>
2005ba66:	f001a423          	sw	zero,-248(gp) # 20066890 <sema_buf_used_num>
2005ba6a:	f3018513          	addi	a0,gp,-208 # 200668b8 <wrapper_sema_buf_list>
2005ba6e:	f001a823          	sw	zero,-240(gp) # 20066898 <sema_max_buf_used_num>
2005ba72:	c108                	c.sw	a0,0(a0)
2005ba74:	c148                	c.sw	a0,4(a0)
2005ba76:	f001a623          	sw	zero,-244(gp) # 20066894 <sema_dynamic_num>
2005ba7a:	008ba023          	sw	s0,0(s7)
2005ba7e:	bf9d                	c.j	2005b9f4 <__reserved_get_from_poll+0x5a>
2005ba80:	f2418b93          	addi	s7,gp,-220 # 200668ac <timer_pool_init_flag>
2005ba84:	000ba783          	lw	a5,0(s7)
2005ba88:	f68786e3          	beq	a5,s0,2005b9f4 <__reserved_get_from_poll+0x5a>
2005ba8c:	4601                	c.li	a2,0
2005ba8e:	4581                	c.li	a1,0
2005ba90:	5d418513          	addi	a0,gp,1492 # 20066f5c <mutex_pool>
2005ba94:	dffae097          	auipc	ra,0xdffae
2005ba98:	03a080e7          	jalr	ra,58(ra) # 9ace <__wrap_memset>
2005ba9c:	f001ac23          	sw	zero,-232(gp) # 200668a0 <timer_buf_used_num>
2005baa0:	f3818513          	addi	a0,gp,-200 # 200668c0 <wrapper_timer_buf_list>
2005baa4:	f201a023          	sw	zero,-224(gp) # 200668a8 <timer_max_buf_used_num>
2005baa8:	c108                	c.sw	a0,0(a0)
2005baaa:	c148                	c.sw	a0,4(a0)
2005baac:	f001ae23          	sw	zero,-228(gp) # 200668a4 <timer_dynamic_num>
2005bab0:	b7e9                	c.j	2005ba7a <__reserved_get_from_poll+0xe0>
2005bab2:	405c                	c.lw	a5,4(s0)
2005bab4:	4018                	c.lw	a4,0(s0)
2005bab6:	00840b13          	addi	s6,s0,8
2005baba:	c35c                	c.sw	a5,4(a4)
2005babc:	c398                	c.sw	a4,0(a5)
2005babe:	c000                	c.sw	s0,0(s0)
2005bac0:	c040                	c.sw	s0,4(s0)
2005bac2:	0009a783          	lw	a5,0(s3)
2005bac6:	0785                	c.addi	a5,1
2005bac8:	00f9a023          	sw	a5,0(s3)
2005bacc:	be9ff0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
2005bad0:	57e1                	c.li	a5,-8
2005bad2:	f2f40ae3          	beq	s0,a5,2005ba06 <__reserved_get_from_poll+0x6c>
2005bad6:	478d                	c.li	a5,3
2005bad8:	05000613          	addi	a2,zero,80
2005badc:	00f49463          	bne	s1,a5,2005bae4 <__reserved_get_from_poll+0x14a>
2005bae0:	02c00613          	addi	a2,zero,44
2005bae4:	4581                	c.li	a1,0
2005bae6:	855a                	c.mv	a0,s6
2005bae8:	dffae097          	auipc	ra,0xdffae
2005baec:	fe6080e7          	jalr	ra,-26(ra) # 9ace <__wrap_memset>
2005baf0:	b71d                	c.j	2005ba16 <__reserved_get_from_poll+0x7c>

2005baf2 <__reserved_get_mutex_from_poll>:
2005baf2:	f0018793          	addi	a5,gp,-256 # 20066888 <mutex_max_buf_used_num>
2005baf6:	ef818713          	addi	a4,gp,-264 # 20066880 <mutex_buf_used_num>
2005bafa:	efc18693          	addi	a3,gp,-260 # 20066884 <mutex_dynamic_num>
2005bafe:	f0418613          	addi	a2,gp,-252 # 2006688c <mutex_pool_init_flag>
2005bb02:	f2818593          	addi	a1,gp,-216 # 200668b0 <wrapper_mutex_buf_list>
2005bb06:	4505                	c.li	a0,1
2005bb08:	e93ff06f          	jal	zero,2005b99a <__reserved_get_from_poll>

2005bb0c <__reserved_release_mutex_to_poll>:
2005bb0c:	1141                	c.addi	sp,-16
2005bb0e:	c606                	c.swsp	ra,12(sp)
2005bb10:	b81ff0ef          	jal	ra,2005b690 <rtos_critical_enter>
2005bb14:	efc18793          	addi	a5,gp,-260 # 20066884 <mutex_dynamic_num>
2005bb18:	4398                	c.lw	a4,0(a5)
2005bb1a:	177d                	c.addi	a4,-1
2005bb1c:	c398                	c.sw	a4,0(a5)
2005bb1e:	40b2                	c.lwsp	ra,12(sp)
2005bb20:	0141                	c.addi	sp,16
2005bb22:	b93ff06f          	jal	zero,2005b6b4 <rtos_critical_exit>

2005bb26 <__reserved_get_sema_from_poll>:
2005bb26:	f1018793          	addi	a5,gp,-240 # 20066898 <sema_max_buf_used_num>
2005bb2a:	f0818713          	addi	a4,gp,-248 # 20066890 <sema_buf_used_num>
2005bb2e:	f0c18693          	addi	a3,gp,-244 # 20066894 <sema_dynamic_num>
2005bb32:	f1418613          	addi	a2,gp,-236 # 2006689c <sema_pool_init_flag>
2005bb36:	f3018593          	addi	a1,gp,-208 # 200668b8 <wrapper_sema_buf_list>
2005bb3a:	4509                	c.li	a0,2
2005bb3c:	e5fff06f          	jal	zero,2005b99a <__reserved_get_from_poll>

2005bb40 <__reserved_get_timer_from_poll>:
2005bb40:	f2018793          	addi	a5,gp,-224 # 200668a8 <timer_max_buf_used_num>
2005bb44:	f1818713          	addi	a4,gp,-232 # 200668a0 <timer_buf_used_num>
2005bb48:	f1c18693          	addi	a3,gp,-228 # 200668a4 <timer_dynamic_num>
2005bb4c:	f2418613          	addi	a2,gp,-220 # 200668ac <timer_pool_init_flag>
2005bb50:	f3818593          	addi	a1,gp,-200 # 200668c0 <wrapper_timer_buf_list>
2005bb54:	450d                	c.li	a0,3
2005bb56:	e45ff06f          	jal	zero,2005b99a <__reserved_get_from_poll>

2005bb5a <__reserved_release_timer_to_poll>:
2005bb5a:	1141                	c.addi	sp,-16
2005bb5c:	c606                	c.swsp	ra,12(sp)
2005bb5e:	b33ff0ef          	jal	ra,2005b690 <rtos_critical_enter>
2005bb62:	f1c18793          	addi	a5,gp,-228 # 200668a4 <timer_dynamic_num>
2005bb66:	4398                	c.lw	a4,0(a5)
2005bb68:	177d                	c.addi	a4,-1
2005bb6a:	c398                	c.sw	a4,0(a5)
2005bb6c:	40b2                	c.lwsp	ra,12(sp)
2005bb6e:	0141                	c.addi	sp,16
2005bb70:	b45ff06f          	jal	zero,2005b6b4 <rtos_critical_exit>

2005bb74 <rtos_sched_start>:
2005bb74:	1141                	c.addi	sp,-16
2005bb76:	c606                	c.swsp	ra,12(sp)
2005bb78:	202020ef          	jal	ra,2005dd7a <vTaskStartScheduler>
2005bb7c:	40b2                	c.lwsp	ra,12(sp)
2005bb7e:	4501                	c.li	a0,0
2005bb80:	0141                	c.addi	sp,16
2005bb82:	8082                	c.jr	ra

2005bb84 <rtos_sched_get_state>:
2005bb84:	1141                	c.addi	sp,-16
2005bb86:	c606                	c.swsp	ra,12(sp)
2005bb88:	699010ef          	jal	ra,2005da20 <xTaskGetSchedulerState>
2005bb8c:	4785                	c.li	a5,1
2005bb8e:	00f50b63          	beq	a0,a5,2005bba4 <rtos_sched_get_state+0x20>
2005bb92:	68f010ef          	jal	ra,2005da20 <xTaskGetSchedulerState>
2005bb96:	c519                	c.beqz	a0,2005bba4 <rtos_sched_get_state+0x20>
2005bb98:	689010ef          	jal	ra,2005da20 <xTaskGetSchedulerState>
2005bb9c:	4789                	c.li	a5,2
2005bb9e:	00f50363          	beq	a0,a5,2005bba4 <rtos_sched_get_state+0x20>
2005bba2:	557d                	c.li	a0,-1
2005bba4:	40b2                	c.lwsp	ra,12(sp)
2005bba6:	0141                	c.addi	sp,16
2005bba8:	8082                	c.jr	ra

2005bbaa <rtos_task_create>:
2005bbaa:	882a                	c.mv	a6,a0
2005bbac:	8532                	c.mv	a0,a2
2005bbae:	863a                	c.mv	a2,a4
2005bbb0:	1141                	c.addi	sp,-16
2005bbb2:	873e                	c.mv	a4,a5
2005bbb4:	8209                	c.srli	a2,0x2
2005bbb6:	87c2                	c.mv	a5,a6
2005bbb8:	c606                	c.swsp	ra,12(sp)
2005bbba:	228020ef          	jal	ra,2005dde2 <xTaskCreate>
2005bbbe:	40b2                	c.lwsp	ra,12(sp)
2005bbc0:	157d                	c.addi	a0,-1
2005bbc2:	00a03533          	sltu	a0,zero,a0
2005bbc6:	40a00533          	sub	a0,zero,a0
2005bbca:	0141                	c.addi	sp,16
2005bbcc:	8082                	c.jr	ra

2005bbce <rtos_task_delete>:
2005bbce:	1141                	c.addi	sp,-16
2005bbd0:	c606                	c.swsp	ra,12(sp)
2005bbd2:	288020ef          	jal	ra,2005de5a <vTaskDelete>
2005bbd6:	40b2                	c.lwsp	ra,12(sp)
2005bbd8:	4501                	c.li	a0,0
2005bbda:	0141                	c.addi	sp,16
2005bbdc:	8082                	c.jr	ra

2005bbde <rtos_create_secure_context>:
2005bbde:	8082                	c.jr	ra

2005bbe0 <rtos_time_delay_ms>:
2005bbe0:	1141                	c.addi	sp,-16
2005bbe2:	c422                	c.swsp	s0,8(sp)
2005bbe4:	c606                	c.swsp	ra,12(sp)
2005bbe6:	842a                	c.mv	s0,a0
2005bbe8:	c0ffd0ef          	jal	ra,200597f6 <pmu_yield_os_check>
2005bbec:	cd19                	c.beqz	a0,2005bc0a <rtos_time_delay_ms+0x2a>
2005bbee:	633010ef          	jal	ra,2005da20 <xTaskGetSchedulerState>
2005bbf2:	4789                	c.li	a5,2
2005bbf4:	00f51b63          	bne	a0,a5,2005bc0a <rtos_time_delay_ms+0x2a>
2005bbf8:	a87ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005bbfc:	e519                	c.bnez	a0,2005bc0a <rtos_time_delay_ms+0x2a>
2005bbfe:	8522                	c.mv	a0,s0
2005bc00:	4422                	c.lwsp	s0,8(sp)
2005bc02:	40b2                	c.lwsp	ra,12(sp)
2005bc04:	0141                	c.addi	sp,16
2005bc06:	46e0206f          	jal	zero,2005e074 <vTaskDelay>
2005bc0a:	8522                	c.mv	a0,s0
2005bc0c:	4422                	c.lwsp	s0,8(sp)
2005bc0e:	40b2                	c.lwsp	ra,12(sp)
2005bc10:	0141                	c.addi	sp,16
2005bc12:	dffa6317          	auipc	t1,0xdffa6
2005bc16:	70830067          	jalr	zero,1800(t1) # 231a <DelayMs>

2005bc1a <rtos_time_get_current_system_time_ms>:
2005bc1a:	1141                	c.addi	sp,-16
2005bc1c:	c606                	c.swsp	ra,12(sp)
2005bc1e:	a61ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005bc22:	c509                	c.beqz	a0,2005bc2c <rtos_time_get_current_system_time_ms+0x12>
2005bc24:	40b2                	c.lwsp	ra,12(sp)
2005bc26:	0141                	c.addi	sp,16
2005bc28:	2c10106f          	jal	zero,2005d6e8 <xTaskGetTickCountFromISR>
2005bc2c:	bcbfd0ef          	jal	ra,200597f6 <pmu_yield_os_check>
2005bc30:	d975                	c.beqz	a0,2005bc24 <rtos_time_get_current_system_time_ms+0xa>
2005bc32:	40b2                	c.lwsp	ra,12(sp)
2005bc34:	0141                	c.addi	sp,16
2005bc36:	6e40206f          	jal	zero,2005e31a <xTaskGetTickCount>

2005bc3a <rtos_timer_create>:
2005bc3a:	1141                	c.addi	sp,-16
2005bc3c:	c422                	c.swsp	s0,8(sp)
2005bc3e:	c606                	c.swsp	ra,12(sp)
2005bc40:	842a                	c.mv	s0,a0
2005bc42:	587d                	c.li	a6,-1
2005bc44:	c105                	c.beqz	a0,2005bc64 <rtos_timer_create+0x2a>
2005bc46:	88b2                	c.mv	a7,a2
2005bc48:	587d                	c.li	a6,-1
2005bc4a:	863a                	c.mv	a2,a4
2005bc4c:	873e                	c.mv	a4,a5
2005bc4e:	cb99                	c.beqz	a5,2005bc64 <rtos_timer_create+0x2a>
2005bc50:	852e                	c.mv	a0,a1
2005bc52:	85b6                	c.mv	a1,a3
2005bc54:	86c6                	c.mv	a3,a7
2005bc56:	10b020ef          	jal	ra,2005e560 <xTimerCreate>
2005bc5a:	c008                	c.sw	a0,0(s0)
2005bc5c:	00153513          	sltiu	a0,a0,1
2005bc60:	40a00833          	sub	a6,zero,a0
2005bc64:	40b2                	c.lwsp	ra,12(sp)
2005bc66:	4422                	c.lwsp	s0,8(sp)
2005bc68:	8542                	c.mv	a0,a6
2005bc6a:	0141                	c.addi	sp,16
2005bc6c:	8082                	c.jr	ra

2005bc6e <rtos_timer_create_static>:
2005bc6e:	1101                	c.addi	sp,-32
2005bc70:	cc22                	c.swsp	s0,24(sp)
2005bc72:	ca26                	c.swsp	s1,20(sp)
2005bc74:	c84a                	c.swsp	s2,16(sp)
2005bc76:	84ae                	c.mv	s1,a1
2005bc78:	c632                	c.swsp	a2,12(sp)
2005bc7a:	893a                	c.mv	s2,a4
2005bc7c:	ce06                	c.swsp	ra,28(sp)
2005bc7e:	842a                	c.mv	s0,a0
2005bc80:	c436                	c.swsp	a3,8(sp)
2005bc82:	c23e                	c.swsp	a5,4(sp)
2005bc84:	ebdff0ef          	jal	ra,2005bb40 <__reserved_get_timer_from_poll>
2005bc88:	4712                	c.lwsp	a4,4(sp)
2005bc8a:	45a2                	c.lwsp	a1,8(sp)
2005bc8c:	4632                	c.lwsp	a2,12(sp)
2005bc8e:	ed09                	c.bnez	a0,2005bca8 <rtos_timer_create_static+0x3a>
2005bc90:	8522                	c.mv	a0,s0
2005bc92:	4462                	c.lwsp	s0,24(sp)
2005bc94:	40f2                	c.lwsp	ra,28(sp)
2005bc96:	87ba                	c.mv	a5,a4
2005bc98:	86ae                	c.mv	a3,a1
2005bc9a:	874a                	c.mv	a4,s2
2005bc9c:	85a6                	c.mv	a1,s1
2005bc9e:	4942                	c.lwsp	s2,16(sp)
2005bca0:	44d2                	c.lwsp	s1,20(sp)
2005bca2:	6105                	c.addi16sp	sp,32
2005bca4:	f97ff06f          	jal	zero,2005bc3a <rtos_timer_create>
2005bca8:	87aa                	c.mv	a5,a0
2005bcaa:	86b2                	c.mv	a3,a2
2005bcac:	8526                	c.mv	a0,s1
2005bcae:	864a                	c.mv	a2,s2
2005bcb0:	119020ef          	jal	ra,2005e5c8 <xTimerCreateStatic>
2005bcb4:	c008                	c.sw	a0,0(s0)
2005bcb6:	40f2                	c.lwsp	ra,28(sp)
2005bcb8:	4462                	c.lwsp	s0,24(sp)
2005bcba:	00153513          	sltiu	a0,a0,1
2005bcbe:	44d2                	c.lwsp	s1,20(sp)
2005bcc0:	4942                	c.lwsp	s2,16(sp)
2005bcc2:	40a00533          	sub	a0,zero,a0
2005bcc6:	6105                	c.addi16sp	sp,32
2005bcc8:	8082                	c.jr	ra

2005bcca <rtos_timer_delete>:
2005bcca:	c10d                	c.beqz	a0,2005bcec <rtos_timer_delete+0x22>
2005bccc:	1141                	c.addi	sp,-16
2005bcce:	872e                	c.mv	a4,a1
2005bcd0:	4681                	c.li	a3,0
2005bcd2:	4601                	c.li	a2,0
2005bcd4:	4595                	c.li	a1,5
2005bcd6:	c606                	c.swsp	ra,12(sp)
2005bcd8:	153020ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005bcdc:	40b2                	c.lwsp	ra,12(sp)
2005bcde:	157d                	c.addi	a0,-1
2005bce0:	00a03533          	sltu	a0,zero,a0
2005bce4:	40a00533          	sub	a0,zero,a0
2005bce8:	0141                	c.addi	sp,16
2005bcea:	8082                	c.jr	ra
2005bcec:	557d                	c.li	a0,-1
2005bcee:	8082                	c.jr	ra

2005bcf0 <rtos_timer_delete_static>:
2005bcf0:	1141                	c.addi	sp,-16
2005bcf2:	c422                	c.swsp	s0,8(sp)
2005bcf4:	c226                	c.swsp	s1,4(sp)
2005bcf6:	c606                	c.swsp	ra,12(sp)
2005bcf8:	842a                	c.mv	s0,a0
2005bcfa:	84ae                	c.mv	s1,a1
2005bcfc:	85a6                	c.mv	a1,s1
2005bcfe:	8522                	c.mv	a0,s0
2005bd00:	fcbff0ef          	jal	ra,2005bcca <rtos_timer_delete>
2005bd04:	fd65                	c.bnez	a0,2005bcfc <rtos_timer_delete_static+0xc>
2005bd06:	8522                	c.mv	a0,s0
2005bd08:	e53ff0ef          	jal	ra,2005bb5a <__reserved_release_timer_to_poll>
2005bd0c:	40b2                	c.lwsp	ra,12(sp)
2005bd0e:	4422                	c.lwsp	s0,8(sp)
2005bd10:	4492                	c.lwsp	s1,4(sp)
2005bd12:	4501                	c.li	a0,0
2005bd14:	0141                	c.addi	sp,16
2005bd16:	8082                	c.jr	ra

2005bd18 <rtos_timer_start>:
2005bd18:	e909                	c.bnez	a0,2005bd2a <rtos_timer_start+0x12>
2005bd1a:	557d                	c.li	a0,-1
2005bd1c:	8082                	c.jr	ra
2005bd1e:	557d                	c.li	a0,-1
2005bd20:	40f2                	c.lwsp	ra,28(sp)
2005bd22:	4462                	c.lwsp	s0,24(sp)
2005bd24:	44d2                	c.lwsp	s1,20(sp)
2005bd26:	6105                	c.addi16sp	sp,32
2005bd28:	8082                	c.jr	ra
2005bd2a:	1101                	c.addi	sp,-32
2005bd2c:	cc22                	c.swsp	s0,24(sp)
2005bd2e:	ca26                	c.swsp	s1,20(sp)
2005bd30:	ce06                	c.swsp	ra,28(sp)
2005bd32:	842a                	c.mv	s0,a0
2005bd34:	84ae                	c.mv	s1,a1
2005bd36:	949ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005bd3a:	c505                	c.beqz	a0,2005bd62 <rtos_timer_start+0x4a>
2005bd3c:	c602                	c.swsp	zero,12(sp)
2005bd3e:	1ab010ef          	jal	ra,2005d6e8 <xTaskGetTickCountFromISR>
2005bd42:	862a                	c.mv	a2,a0
2005bd44:	4701                	c.li	a4,0
2005bd46:	0074                	c.addi4spn	a3,sp,12
2005bd48:	4599                	c.li	a1,6
2005bd4a:	8522                	c.mv	a0,s0
2005bd4c:	0df020ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005bd50:	4785                	c.li	a5,1
2005bd52:	fcf516e3          	bne	a0,a5,2005bd1e <rtos_timer_start+0x6>
2005bd56:	4532                	c.lwsp	a0,12(sp)
2005bd58:	d561                	c.beqz	a0,2005bd20 <rtos_timer_start+0x8>
2005bd5a:	2dd010ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005bd5e:	4501                	c.li	a0,0
2005bd60:	b7c1                	c.j	2005bd20 <rtos_timer_start+0x8>
2005bd62:	5b8020ef          	jal	ra,2005e31a <xTaskGetTickCount>
2005bd66:	862a                	c.mv	a2,a0
2005bd68:	8726                	c.mv	a4,s1
2005bd6a:	4681                	c.li	a3,0
2005bd6c:	4585                	c.li	a1,1
2005bd6e:	8522                	c.mv	a0,s0
2005bd70:	0bb020ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005bd74:	157d                	c.addi	a0,-1
2005bd76:	00a03533          	sltu	a0,zero,a0
2005bd7a:	40a00533          	sub	a0,zero,a0
2005bd7e:	b74d                	c.j	2005bd20 <rtos_timer_start+0x8>

2005bd80 <rtos_timer_stop>:
2005bd80:	e901                	c.bnez	a0,2005bd90 <rtos_timer_stop+0x10>
2005bd82:	557d                	c.li	a0,-1
2005bd84:	8082                	c.jr	ra
2005bd86:	557d                	c.li	a0,-1
2005bd88:	50b2                	c.lwsp	ra,44(sp)
2005bd8a:	5422                	c.lwsp	s0,40(sp)
2005bd8c:	6145                	c.addi16sp	sp,48
2005bd8e:	8082                	c.jr	ra
2005bd90:	7179                	c.addi16sp	sp,-48
2005bd92:	d422                	c.swsp	s0,40(sp)
2005bd94:	d606                	c.swsp	ra,44(sp)
2005bd96:	842a                	c.mv	s0,a0
2005bd98:	c62e                	c.swsp	a1,12(sp)
2005bd9a:	8e5ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005bd9e:	4732                	c.lwsp	a4,12(sp)
2005bda0:	c115                	c.beqz	a0,2005bdc4 <rtos_timer_stop+0x44>
2005bda2:	4701                	c.li	a4,0
2005bda4:	0874                	c.addi4spn	a3,sp,28
2005bda6:	4601                	c.li	a2,0
2005bda8:	45a1                	c.li	a1,8
2005bdaa:	8522                	c.mv	a0,s0
2005bdac:	ce02                	c.swsp	zero,28(sp)
2005bdae:	07d020ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005bdb2:	4785                	c.li	a5,1
2005bdb4:	fcf519e3          	bne	a0,a5,2005bd86 <rtos_timer_stop+0x6>
2005bdb8:	4572                	c.lwsp	a0,28(sp)
2005bdba:	d579                	c.beqz	a0,2005bd88 <rtos_timer_stop+0x8>
2005bdbc:	27b010ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005bdc0:	4501                	c.li	a0,0
2005bdc2:	b7d9                	c.j	2005bd88 <rtos_timer_stop+0x8>
2005bdc4:	4681                	c.li	a3,0
2005bdc6:	4601                	c.li	a2,0
2005bdc8:	458d                	c.li	a1,3
2005bdca:	8522                	c.mv	a0,s0
2005bdcc:	05f020ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005bdd0:	157d                	c.addi	a0,-1
2005bdd2:	00a03533          	sltu	a0,zero,a0
2005bdd6:	40a00533          	sub	a0,zero,a0
2005bdda:	b77d                	c.j	2005bd88 <rtos_timer_stop+0x8>

2005bddc <rtos_timer_change_period>:
2005bddc:	e909                	c.bnez	a0,2005bdee <rtos_timer_change_period+0x12>
2005bdde:	557d                	c.li	a0,-1
2005bde0:	8082                	c.jr	ra
2005bde2:	557d                	c.li	a0,-1
2005bde4:	50b2                	c.lwsp	ra,44(sp)
2005bde6:	5422                	c.lwsp	s0,40(sp)
2005bde8:	5492                	c.lwsp	s1,36(sp)
2005bdea:	6145                	c.addi16sp	sp,48
2005bdec:	8082                	c.jr	ra
2005bdee:	7179                	c.addi16sp	sp,-48
2005bdf0:	d422                	c.swsp	s0,40(sp)
2005bdf2:	d226                	c.swsp	s1,36(sp)
2005bdf4:	d606                	c.swsp	ra,44(sp)
2005bdf6:	842a                	c.mv	s0,a0
2005bdf8:	84ae                	c.mv	s1,a1
2005bdfa:	c632                	c.swsp	a2,12(sp)
2005bdfc:	883ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005be00:	4732                	c.lwsp	a4,12(sp)
2005be02:	c115                	c.beqz	a0,2005be26 <rtos_timer_change_period+0x4a>
2005be04:	4701                	c.li	a4,0
2005be06:	0874                	c.addi4spn	a3,sp,28
2005be08:	8626                	c.mv	a2,s1
2005be0a:	45a5                	c.li	a1,9
2005be0c:	8522                	c.mv	a0,s0
2005be0e:	ce02                	c.swsp	zero,28(sp)
2005be10:	01b020ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005be14:	4785                	c.li	a5,1
2005be16:	fcf516e3          	bne	a0,a5,2005bde2 <rtos_timer_change_period+0x6>
2005be1a:	4572                	c.lwsp	a0,28(sp)
2005be1c:	d561                	c.beqz	a0,2005bde4 <rtos_timer_change_period+0x8>
2005be1e:	219010ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005be22:	4501                	c.li	a0,0
2005be24:	b7c1                	c.j	2005bde4 <rtos_timer_change_period+0x8>
2005be26:	4681                	c.li	a3,0
2005be28:	8626                	c.mv	a2,s1
2005be2a:	4591                	c.li	a1,4
2005be2c:	8522                	c.mv	a0,s0
2005be2e:	7fc020ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005be32:	157d                	c.addi	a0,-1
2005be34:	00a03533          	sltu	a0,zero,a0
2005be38:	40a00533          	sub	a0,zero,a0
2005be3c:	b765                	c.j	2005bde4 <rtos_timer_change_period+0x8>

2005be3e <rtos_timer_is_timer_active>:
2005be3e:	2cf0206f          	jal	zero,2005e90c <xTimerIsTimerActive>

2005be42 <platform_calloc_uninit>:
2005be42:	4501                	c.li	a0,0
2005be44:	8082                	c.jr	ra

2005be46 <platform_free_uninit>:
2005be46:	8082                	c.jr	ra

2005be48 <mbedtls_platform_set_calloc_free>:
2005be48:	e8a1a023          	sw	a0,-384(gp) # 20066808 <mbedtls_calloc_func>
2005be4c:	e8b1a223          	sw	a1,-380(gp) # 2006680c <mbedtls_free_func>
2005be50:	200017b7          	lui	a5,0x20001
2005be54:	c6c78793          	addi	a5,a5,-916 # 20000c6c <rom_ssl_ram_map>
2005be58:	20001737          	lui	a4,0x20001
2005be5c:	c388                	c.sw	a0,0(a5)
2005be5e:	c6f72423          	sw	a5,-920(a4) # 20000c68 <p_rom_ssl_ram_map>
2005be62:	c3cc                	c.sw	a1,4(a5)
2005be64:	4501                	c.li	a0,0
2005be66:	8082                	c.jr	ra

2005be68 <freertos_risc_v_trap_handler>:
2005be68:	ef810113          	addi	sp,sp,-264
2005be6c:	c206                	c.swsp	ra,4(sp)
2005be6e:	ca16                	c.swsp	t0,20(sp)
2005be70:	cc1a                	c.swsp	t1,24(sp)
2005be72:	ce1e                	c.swsp	t2,28(sp)
2005be74:	d022                	c.swsp	s0,32(sp)
2005be76:	d226                	c.swsp	s1,36(sp)
2005be78:	d42a                	c.swsp	a0,40(sp)
2005be7a:	d62e                	c.swsp	a1,44(sp)
2005be7c:	d832                	c.swsp	a2,48(sp)
2005be7e:	da36                	c.swsp	a3,52(sp)
2005be80:	dc3a                	c.swsp	a4,56(sp)
2005be82:	de3e                	c.swsp	a5,60(sp)
2005be84:	c0c2                	c.swsp	a6,64(sp)
2005be86:	c2c6                	c.swsp	a7,68(sp)
2005be88:	c4ca                	c.swsp	s2,72(sp)
2005be8a:	c6ce                	c.swsp	s3,76(sp)
2005be8c:	c8d2                	c.swsp	s4,80(sp)
2005be8e:	cad6                	c.swsp	s5,84(sp)
2005be90:	ccda                	c.swsp	s6,88(sp)
2005be92:	cede                	c.swsp	s7,92(sp)
2005be94:	d0e2                	c.swsp	s8,96(sp)
2005be96:	d2e6                	c.swsp	s9,100(sp)
2005be98:	d4ea                	c.swsp	s10,104(sp)
2005be9a:	d6ee                	c.swsp	s11,108(sp)
2005be9c:	d8f2                	c.swsp	t3,112(sp)
2005be9e:	daf6                	c.swsp	t4,116(sp)
2005bea0:	dcfa                	c.swsp	t5,120(sp)
2005bea2:	defe                	c.swsp	t6,124(sp)
2005bea4:	300022f3          	csrrs	t0,mstatus,zero
2005bea8:	6319                	c.lui	t1,0x6
2005beaa:	0062f2b3          	and	t0,t0,t1
2005beae:	6309                	c.lui	t1,0x2
2005beb0:	04628663          	beq	t0,t1,2005befc <freertos_risc_v_trap_handler+0x94>
2005beb4:	003022f3          	csrrs	t0,fcsr,zero
2005beb8:	e102                	c.fswsp	ft0,128(sp)
2005beba:	e306                	c.fswsp	ft1,132(sp)
2005bebc:	e50a                	c.fswsp	ft2,136(sp)
2005bebe:	e70e                	c.fswsp	ft3,140(sp)
2005bec0:	e912                	c.fswsp	ft4,144(sp)
2005bec2:	eb16                	c.fswsp	ft5,148(sp)
2005bec4:	ed1a                	c.fswsp	ft6,152(sp)
2005bec6:	ef1e                	c.fswsp	ft7,156(sp)
2005bec8:	f122                	c.fswsp	fs0,160(sp)
2005beca:	f326                	c.fswsp	fs1,164(sp)
2005becc:	f52a                	c.fswsp	fa0,168(sp)
2005bece:	f72e                	c.fswsp	fa1,172(sp)
2005bed0:	f932                	c.fswsp	fa2,176(sp)
2005bed2:	fb36                	c.fswsp	fa3,180(sp)
2005bed4:	fd3a                	c.fswsp	fa4,184(sp)
2005bed6:	ff3e                	c.fswsp	fa5,188(sp)
2005bed8:	e1c2                	c.fswsp	fa6,192(sp)
2005beda:	e3c6                	c.fswsp	fa7,196(sp)
2005bedc:	e5ca                	c.fswsp	fs2,200(sp)
2005bede:	e7ce                	c.fswsp	fs3,204(sp)
2005bee0:	e9d2                	c.fswsp	fs4,208(sp)
2005bee2:	ebd6                	c.fswsp	fs5,212(sp)
2005bee4:	edda                	c.fswsp	fs6,216(sp)
2005bee6:	efde                	c.fswsp	fs7,220(sp)
2005bee8:	f1e2                	c.fswsp	fs8,224(sp)
2005beea:	f3e6                	c.fswsp	fs9,228(sp)
2005beec:	f5ea                	c.fswsp	fs10,232(sp)
2005beee:	f7ee                	c.fswsp	fs11,236(sp)
2005bef0:	f9f2                	c.fswsp	ft8,240(sp)
2005bef2:	fbf6                	c.fswsp	ft9,244(sp)
2005bef4:	fdfa                	c.fswsp	ft10,248(sp)
2005bef6:	fffe                	c.fswsp	ft11,252(sp)
2005bef8:	10512023          	sw	t0,256(sp)
2005befc:	300022f3          	csrrs	t0,mstatus,zero
2005bf00:	10512223          	sw	t0,260(sp)
2005bf04:	34202573          	csrrs	a0,mcause,zero
2005bf08:	341025f3          	csrrs	a1,mepc,zero

2005bf0c <test_if_asynchronous>:
2005bf0c:	01f55613          	srli	a2,a0,0x1f
2005bf10:	e225                	c.bnez	a2,2005bf70 <handle_asynchronous>

2005bf12 <handle_synchronous>:
2005bf12:	0591                	c.addi	a1,4
2005bf14:	c02e                	c.swsp	a1,0(sp)

2005bf16 <test_if_environment_call>:
2005bf16:	42ad                	c.li	t0,11
2005bf18:	00a2cd63          	blt	t0,a0,2005bf32 <_crash_dump>
2005bf1c:	42a1                	c.li	t0,8
2005bf1e:	00554a63          	blt	a0,t0,2005bf32 <_crash_dump>
2005bf22:	a009                	c.j	2005bf24 <ecall_yield>

2005bf24 <ecall_yield>:
2005bf24:	f5c1a283          	lw	t0,-164(gp) # 200668e4 <pxCurrentTCB>
2005bf28:	0022a023          	sw	sp,0(t0)
2005bf2c:	e5c1a103          	lw	sp,-420(gp) # 200667e4 <xISRStackTop>
2005bf30:	a0a5                	c.j	2005bf98 <task_context_switch>

2005bf32 <_crash_dump>:
2005bf32:	34102573          	csrrs	a0,mepc,zero
2005bf36:	022c                	c.addi4spn	a1,sp,264
2005bf38:	860a                	c.mv	a2,sp
2005bf3a:	c002                	c.swsp	zero,0(sp)
2005bf3c:	c42e                	c.swsp	a1,8(sp)
2005bf3e:	c60e                	c.swsp	gp,12(sp)
2005bf40:	c812                	c.swsp	tp,16(sp)
2005bf42:	e5c1a303          	lw	t1,-420(gp) # 200667e4 <xISRStackTop>
2005bf46:	0000b397          	auipc	t2,0xb
2005bf4a:	01a38393          	addi	t2,t2,26 # 20066f60 <xISRStack>
2005bf4e:	08038393          	addi	t2,t2,128
2005bf52:	00615563          	bge	sp,t1,2005bf5c <not_isrstack>
2005bf56:	00714363          	blt	sp,t2,2005bf5c <not_isrstack>
2005bf5a:	a019                	c.j	2005bf60 <is_isrstack>

2005bf5c <not_isrstack>:
2005bf5c:	e5c1a103          	lw	sp,-420(gp) # 200667e4 <xISRStackTop>

2005bf60 <is_isrstack>:
2005bf60:	00004297          	auipc	t0,0x4
2005bf64:	0ae28293          	addi	t0,t0,174 # 2006000e <crash_dump>
2005bf68:	9282                	c.jalr	t0

2005bf6a <crash_dump_end>:
2005bf6a:	a001                	c.j	2005bf6a <crash_dump_end>

2005bf6c <unrecoverable_error>:
2005bf6c:	9002                	c.ebreak
2005bf6e:	bffd                	c.j	2005bf6c <unrecoverable_error>

2005bf70 <handle_asynchronous>:
2005bf70:	c02e                	c.swsp	a1,0(sp)
2005bf72:	f5c1a283          	lw	t0,-164(gp) # 200668e4 <pxCurrentTCB>
2005bf76:	0022a023          	sw	sp,0(t0)
2005bf7a:	e5c1a103          	lw	sp,-420(gp) # 200667e4 <xISRStackTop>

2005bf7e <nonvec_handle>:
2005bf7e:	4285                	c.li	t0,1
2005bf80:	02fe                	c.slli	t0,0x1f
2005bf82:	00728313          	addi	t1,t0,7
2005bf86:	00651763          	bne	a0,t1,2005bf94 <test_if_external_interrupt>
2005bf8a:	28d9                	c.jal	2005c060 <machine_timer_update>
2005bf8c:	774010ef          	jal	ra,2005d700 <xTaskIncrementTick>
2005bf90:	c909                	c.beqz	a0,2005bfa2 <processed_source>
2005bf92:	a019                	c.j	2005bf98 <task_context_switch>

2005bf94 <test_if_external_interrupt>:
2005bf94:	2211                	c.jal	2005c098 <vPortHandleInterrupt>
2005bf96:	a031                	c.j	2005bfa2 <processed_source>

2005bf98 <task_context_switch>:
2005bf98:	09f010ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005bf9c:	a019                	c.j	2005bfa2 <processed_source>

2005bf9e <as_yet_unhandled>:
2005bf9e:	9002                	c.ebreak
2005bfa0:	bffd                	c.j	2005bf9e <as_yet_unhandled>

2005bfa2 <processed_source>:
2005bfa2:	f5c1a303          	lw	t1,-164(gp) # 200668e4 <pxCurrentTCB>
2005bfa6:	00032103          	lw	sp,0(t1) # 2000 <CRYPTO_Init+0x86a>

2005bfaa <restore_regs>:
2005bfaa:	4282                	c.lwsp	t0,0(sp)
2005bfac:	34129073          	csrrw	zero,mepc,t0
2005bfb0:	10412283          	lw	t0,260(sp)
2005bfb4:	30029073          	csrrw	zero,mstatus,t0

2005bfb8 <_stack_pop>:
2005bfb8:	300022f3          	csrrs	t0,mstatus,zero
2005bfbc:	6319                	c.lui	t1,0x6
2005bfbe:	0062f2b3          	and	t0,t0,t1
2005bfc2:	6309                	c.lui	t1,0x2
2005bfc4:	04628c63          	beq	t0,t1,2005c01c <_stack_pop+0x64>
2005bfc8:	600a                	c.flwsp	ft0,128(sp)
2005bfca:	609a                	c.flwsp	ft1,132(sp)
2005bfcc:	612a                	c.flwsp	ft2,136(sp)
2005bfce:	61ba                	c.flwsp	ft3,140(sp)
2005bfd0:	624a                	c.flwsp	ft4,144(sp)
2005bfd2:	62da                	c.flwsp	ft5,148(sp)
2005bfd4:	636a                	c.flwsp	ft6,152(sp)
2005bfd6:	63fa                	c.flwsp	ft7,156(sp)
2005bfd8:	740a                	c.flwsp	fs0,160(sp)
2005bfda:	749a                	c.flwsp	fs1,164(sp)
2005bfdc:	752a                	c.flwsp	fa0,168(sp)
2005bfde:	75ba                	c.flwsp	fa1,172(sp)
2005bfe0:	764a                	c.flwsp	fa2,176(sp)
2005bfe2:	76da                	c.flwsp	fa3,180(sp)
2005bfe4:	776a                	c.flwsp	fa4,184(sp)
2005bfe6:	77fa                	c.flwsp	fa5,188(sp)
2005bfe8:	680e                	c.flwsp	fa6,192(sp)
2005bfea:	689e                	c.flwsp	fa7,196(sp)
2005bfec:	692e                	c.flwsp	fs2,200(sp)
2005bfee:	69be                	c.flwsp	fs3,204(sp)
2005bff0:	6a4e                	c.flwsp	fs4,208(sp)
2005bff2:	6ade                	c.flwsp	fs5,212(sp)
2005bff4:	6b6e                	c.flwsp	fs6,216(sp)
2005bff6:	6bfe                	c.flwsp	fs7,220(sp)
2005bff8:	7c0e                	c.flwsp	fs8,224(sp)
2005bffa:	7c9e                	c.flwsp	fs9,228(sp)
2005bffc:	7d2e                	c.flwsp	fs10,232(sp)
2005bffe:	7dbe                	c.flwsp	fs11,236(sp)
2005c000:	7e4e                	c.flwsp	ft8,240(sp)
2005c002:	7ede                	c.flwsp	ft9,244(sp)
2005c004:	7f6e                	c.flwsp	ft10,248(sp)
2005c006:	7ffe                	c.flwsp	ft11,252(sp)
2005c008:	10012283          	lw	t0,256(sp)
2005c00c:	00329073          	csrrw	zero,fcsr,t0
2005c010:	6319                	c.lui	t1,0x6
2005c012:	30033073          	csrrc	zero,mstatus,t1
2005c016:	6311                	c.lui	t1,0x4
2005c018:	30032073          	csrrs	zero,mstatus,t1
2005c01c:	4092                	c.lwsp	ra,4(sp)
2005c01e:	42d2                	c.lwsp	t0,20(sp)
2005c020:	4362                	c.lwsp	t1,24(sp)
2005c022:	43f2                	c.lwsp	t2,28(sp)
2005c024:	5402                	c.lwsp	s0,32(sp)
2005c026:	5492                	c.lwsp	s1,36(sp)
2005c028:	5522                	c.lwsp	a0,40(sp)
2005c02a:	55b2                	c.lwsp	a1,44(sp)
2005c02c:	5642                	c.lwsp	a2,48(sp)
2005c02e:	56d2                	c.lwsp	a3,52(sp)
2005c030:	5762                	c.lwsp	a4,56(sp)
2005c032:	57f2                	c.lwsp	a5,60(sp)
2005c034:	4806                	c.lwsp	a6,64(sp)
2005c036:	4896                	c.lwsp	a7,68(sp)
2005c038:	4926                	c.lwsp	s2,72(sp)
2005c03a:	49b6                	c.lwsp	s3,76(sp)
2005c03c:	4a46                	c.lwsp	s4,80(sp)
2005c03e:	4ad6                	c.lwsp	s5,84(sp)
2005c040:	4b66                	c.lwsp	s6,88(sp)
2005c042:	4bf6                	c.lwsp	s7,92(sp)
2005c044:	5c06                	c.lwsp	s8,96(sp)
2005c046:	5c96                	c.lwsp	s9,100(sp)
2005c048:	5d26                	c.lwsp	s10,104(sp)
2005c04a:	5db6                	c.lwsp	s11,108(sp)
2005c04c:	5e46                	c.lwsp	t3,112(sp)
2005c04e:	5ed6                	c.lwsp	t4,116(sp)
2005c050:	5f66                	c.lwsp	t5,120(sp)
2005c052:	5ff6                	c.lwsp	t6,124(sp)
2005c054:	10810113          	addi	sp,sp,264
2005c058:	30200073          	mret
2005c05c:	00000013          	addi	zero,zero,0

2005c060 <machine_timer_update>:
2005c060:	f401a283          	lw	t0,-192(gp) # 200668c8 <pullMachineTimerCompareRegister>
2005c064:	e881a303          	lw	t1,-376(gp) # 20066810 <pullNextTime>
2005c068:	5efd                	c.li	t4,-1
2005c06a:	00032383          	lw	t2,0(t1) # 4000 <GPIO_Direction+0x36>
2005c06e:	00432e03          	lw	t3,4(t1)
2005c072:	01d2a023          	sw	t4,0(t0)
2005c076:	01c2a223          	sw	t3,4(t0)
2005c07a:	0072a023          	sw	t2,0(t0)
2005c07e:	f501a283          	lw	t0,-176(gp) # 200668d8 <uxTimerIncrementsForOneTick>
2005c082:	00728eb3          	add	t4,t0,t2
2005c086:	007ebf33          	sltu	t5,t4,t2
2005c08a:	01ee0fb3          	add	t6,t3,t5
2005c08e:	01d32023          	sw	t4,0(t1)
2005c092:	01f32223          	sw	t6,4(t1)
2005c096:	8082                	c.jr	ra

2005c098 <vPortHandleInterrupt>:
2005c098:	42bd                	c.li	t0,15
2005c09a:	fff2c293          	xori	t0,t0,-1
2005c09e:	00517133          	and	sp,sp,t0
2005c0a2:	1141                	c.addi	sp,-16
2005c0a4:	c006                	c.swsp	ra,0(sp)
2005c0a6:	34202373          	csrrs	t1,mcause,zero
2005c0aa:	800003b7          	lui	t2,0x80000
2005c0ae:	fff3c393          	xori	t2,t2,-1
2005c0b2:	007373b3          	and	t2,t1,t2
2005c0b6:	00038533          	add	a0,t2,zero
2005c0ba:	0000a797          	auipc	a5,0xa
2005c0be:	3aa78793          	addi	a5,a5,938 # 20066464 <plic_interrupt_handler>
2005c0c2:	9782                	c.jalr	a5

2005c0c4 <vPortHandleInterrupt_Exit>:
2005c0c4:	4082                	c.lwsp	ra,0(sp)
2005c0c6:	0141                	c.addi	sp,16
2005c0c8:	8082                	c.jr	ra
2005c0ca:	0001                	c.addi	zero,0
2005c0cc:	00000013          	addi	zero,zero,0

2005c0d0 <xPortStartFirstTask>:
2005c0d0:	00000297          	auipc	t0,0x0
2005c0d4:	d9828293          	addi	t0,t0,-616 # 2005be68 <freertos_risc_v_trap_handler>
2005c0d8:	30529073          	csrrw	zero,mtvec,t0
2005c0dc:	f5c1a383          	lw	t2,-164(gp) # 200668e4 <pxCurrentTCB>
2005c0e0:	0003a103          	lw	sp,0(t2) # 80000000 <__ctrace_end__+0xb6000000>
2005c0e4:	300022f3          	csrrs	t0,mstatus,zero
2005c0e8:	6319                	c.lui	t1,0x6
2005c0ea:	0062f2b3          	and	t0,t0,t1
2005c0ee:	6309                	c.lui	t1,0x2
2005c0f0:	04628c63          	beq	t0,t1,2005c148 <xPortStartFirstTask+0x78>
2005c0f4:	600a                	c.flwsp	ft0,128(sp)
2005c0f6:	609a                	c.flwsp	ft1,132(sp)
2005c0f8:	612a                	c.flwsp	ft2,136(sp)
2005c0fa:	61ba                	c.flwsp	ft3,140(sp)
2005c0fc:	624a                	c.flwsp	ft4,144(sp)
2005c0fe:	62da                	c.flwsp	ft5,148(sp)
2005c100:	636a                	c.flwsp	ft6,152(sp)
2005c102:	63fa                	c.flwsp	ft7,156(sp)
2005c104:	740a                	c.flwsp	fs0,160(sp)
2005c106:	749a                	c.flwsp	fs1,164(sp)
2005c108:	752a                	c.flwsp	fa0,168(sp)
2005c10a:	75ba                	c.flwsp	fa1,172(sp)
2005c10c:	764a                	c.flwsp	fa2,176(sp)
2005c10e:	76da                	c.flwsp	fa3,180(sp)
2005c110:	776a                	c.flwsp	fa4,184(sp)
2005c112:	77fa                	c.flwsp	fa5,188(sp)
2005c114:	680e                	c.flwsp	fa6,192(sp)
2005c116:	689e                	c.flwsp	fa7,196(sp)
2005c118:	692e                	c.flwsp	fs2,200(sp)
2005c11a:	69be                	c.flwsp	fs3,204(sp)
2005c11c:	6a4e                	c.flwsp	fs4,208(sp)
2005c11e:	6ade                	c.flwsp	fs5,212(sp)
2005c120:	6b6e                	c.flwsp	fs6,216(sp)
2005c122:	6bfe                	c.flwsp	fs7,220(sp)
2005c124:	7c0e                	c.flwsp	fs8,224(sp)
2005c126:	7c9e                	c.flwsp	fs9,228(sp)
2005c128:	7d2e                	c.flwsp	fs10,232(sp)
2005c12a:	7dbe                	c.flwsp	fs11,236(sp)
2005c12c:	7e4e                	c.flwsp	ft8,240(sp)
2005c12e:	7ede                	c.flwsp	ft9,244(sp)
2005c130:	7f6e                	c.flwsp	ft10,248(sp)
2005c132:	7ffe                	c.flwsp	ft11,252(sp)
2005c134:	10012283          	lw	t0,256(sp)
2005c138:	00329073          	csrrw	zero,fcsr,t0
2005c13c:	6319                	c.lui	t1,0x6
2005c13e:	30033073          	csrrc	zero,mstatus,t1
2005c142:	6311                	c.lui	t1,0x4
2005c144:	30032073          	csrrs	zero,mstatus,t1
2005c148:	4092                	c.lwsp	ra,4(sp)
2005c14a:	42d2                	c.lwsp	t0,20(sp)
2005c14c:	4362                	c.lwsp	t1,24(sp)
2005c14e:	43f2                	c.lwsp	t2,28(sp)
2005c150:	5402                	c.lwsp	s0,32(sp)
2005c152:	5492                	c.lwsp	s1,36(sp)
2005c154:	5522                	c.lwsp	a0,40(sp)
2005c156:	55b2                	c.lwsp	a1,44(sp)
2005c158:	5642                	c.lwsp	a2,48(sp)
2005c15a:	56d2                	c.lwsp	a3,52(sp)
2005c15c:	5762                	c.lwsp	a4,56(sp)
2005c15e:	57f2                	c.lwsp	a5,60(sp)
2005c160:	4806                	c.lwsp	a6,64(sp)
2005c162:	4896                	c.lwsp	a7,68(sp)
2005c164:	4926                	c.lwsp	s2,72(sp)
2005c166:	49b6                	c.lwsp	s3,76(sp)
2005c168:	4a46                	c.lwsp	s4,80(sp)
2005c16a:	4ad6                	c.lwsp	s5,84(sp)
2005c16c:	4b66                	c.lwsp	s6,88(sp)
2005c16e:	4bf6                	c.lwsp	s7,92(sp)
2005c170:	5c06                	c.lwsp	s8,96(sp)
2005c172:	5c96                	c.lwsp	s9,100(sp)
2005c174:	5d26                	c.lwsp	s10,104(sp)
2005c176:	5db6                	c.lwsp	s11,108(sp)
2005c178:	5e46                	c.lwsp	t3,112(sp)
2005c17a:	5ed6                	c.lwsp	t4,116(sp)
2005c17c:	5f66                	c.lwsp	t5,120(sp)
2005c17e:	5ff6                	c.lwsp	t6,124(sp)
2005c180:	10412283          	lw	t0,260(sp)
2005c184:	0082e293          	ori	t0,t0,8
2005c188:	30029073          	csrrw	zero,mstatus,t0
2005c18c:	42d2                	c.lwsp	t0,20(sp)
2005c18e:	4082                	c.lwsp	ra,0(sp)
2005c190:	10810113          	addi	sp,sp,264
2005c194:	8082                	c.jr	ra
2005c196:	0001                	c.addi	zero,0

2005c198 <pxPortInitialiseStack>:
2005c198:	300022f3          	csrrs	t0,mstatus,zero
2005c19c:	ff72f293          	andi	t0,t0,-9
2005c1a0:	18800313          	addi	t1,zero,392
2005c1a4:	0312                	c.slli	t1,0x4
2005c1a6:	0062e2b3          	or	t0,t0,t1
2005c1aa:	1571                	c.addi	a0,-4
2005c1ac:	00552023          	sw	t0,0(a0)
2005c1b0:	f2450513          	addi	a0,a0,-220
2005c1b4:	c110                	c.sw	a2,0(a0)
2005c1b6:	fdc50513          	addi	a0,a0,-36
2005c1ba:	00052023          	sw	zero,0(a0)
2005c1be:	4281                	c.li	t0,0

2005c1c0 <chip_specific_stack_frame>:
2005c1c0:	00028763          	beq	t0,zero,2005c1ce <chip_specific_stack_frame+0xe>
2005c1c4:	1571                	c.addi	a0,-4
2005c1c6:	00052023          	sw	zero,0(a0)
2005c1ca:	12fd                	c.addi	t0,-1
2005c1cc:	bfd5                	c.j	2005c1c0 <chip_specific_stack_frame>
2005c1ce:	1571                	c.addi	a0,-4
2005c1d0:	c10c                	c.sw	a1,0(a0)
2005c1d2:	8082                	c.jr	ra

2005c1d4 <vPortBackupRegs>:
2005c1d4:	00152223          	sw	ra,4(a0)
2005c1d8:	00252423          	sw	sp,8(a0)
2005c1dc:	00352623          	sw	gp,12(a0)
2005c1e0:	00452823          	sw	tp,16(a0)
2005c1e4:	00552a23          	sw	t0,20(a0)
2005c1e8:	00652c23          	sw	t1,24(a0)
2005c1ec:	00752e23          	sw	t2,28(a0)
2005c1f0:	d100                	c.sw	s0,32(a0)
2005c1f2:	d144                	c.sw	s1,36(a0)
2005c1f4:	d508                	c.sw	a0,40(a0)
2005c1f6:	d54c                	c.sw	a1,44(a0)
2005c1f8:	d910                	c.sw	a2,48(a0)
2005c1fa:	d954                	c.sw	a3,52(a0)
2005c1fc:	dd18                	c.sw	a4,56(a0)
2005c1fe:	dd5c                	c.sw	a5,60(a0)
2005c200:	05052023          	sw	a6,64(a0)
2005c204:	05152223          	sw	a7,68(a0)
2005c208:	05252423          	sw	s2,72(a0)
2005c20c:	05352623          	sw	s3,76(a0)
2005c210:	05452823          	sw	s4,80(a0)
2005c214:	05552a23          	sw	s5,84(a0)
2005c218:	05652c23          	sw	s6,88(a0)
2005c21c:	05752e23          	sw	s7,92(a0)
2005c220:	07852023          	sw	s8,96(a0)
2005c224:	07952223          	sw	s9,100(a0)
2005c228:	07a52423          	sw	s10,104(a0)
2005c22c:	07b52623          	sw	s11,108(a0)
2005c230:	07c52823          	sw	t3,112(a0)
2005c234:	07d52a23          	sw	t4,116(a0)
2005c238:	07e52c23          	sw	t5,120(a0)
2005c23c:	07f52e23          	sw	t6,124(a0)
2005c240:	300022f3          	csrrs	t0,mstatus,zero
2005c244:	0055a023          	sw	t0,0(a1)
2005c248:	304022f3          	csrrs	t0,mie,zero
2005c24c:	0055a223          	sw	t0,4(a1)
2005c250:	305022f3          	csrrs	t0,mtvec,zero
2005c254:	0055a423          	sw	t0,8(a1)
2005c258:	0015a623          	sw	ra,12(a1)
2005c25c:	8082                	c.jr	ra

2005c25e <vPortBackupfloatRegs>:
2005c25e:	300022f3          	csrrs	t0,mstatus,zero
2005c262:	6319                	c.lui	t1,0x6
2005c264:	0062f2b3          	and	t0,t0,t1
2005c268:	06629e63          	bne	t0,t1,2005c2e4 <vPortBackupfloatRegs+0x86>
2005c26c:	00052027          	fsw	ft0,0(a0)
2005c270:	00152227          	fsw	ft1,4(a0)
2005c274:	00252427          	fsw	ft2,8(a0)
2005c278:	00352627          	fsw	ft3,12(a0)
2005c27c:	00452827          	fsw	ft4,16(a0)
2005c280:	00552a27          	fsw	ft5,20(a0)
2005c284:	00652c27          	fsw	ft6,24(a0)
2005c288:	00752e27          	fsw	ft7,28(a0)
2005c28c:	f100                	c.fsw	fs0,32(a0)
2005c28e:	f144                	c.fsw	fs1,36(a0)
2005c290:	f508                	c.fsw	fa0,40(a0)
2005c292:	f54c                	c.fsw	fa1,44(a0)
2005c294:	f910                	c.fsw	fa2,48(a0)
2005c296:	f954                	c.fsw	fa3,52(a0)
2005c298:	fd18                	c.fsw	fa4,56(a0)
2005c29a:	fd5c                	c.fsw	fa5,60(a0)
2005c29c:	05052027          	fsw	fa6,64(a0)
2005c2a0:	05152227          	fsw	fa7,68(a0)
2005c2a4:	05252427          	fsw	fs2,72(a0)
2005c2a8:	05352627          	fsw	fs3,76(a0)
2005c2ac:	05452827          	fsw	fs4,80(a0)
2005c2b0:	05552a27          	fsw	fs5,84(a0)
2005c2b4:	05652c27          	fsw	fs6,88(a0)
2005c2b8:	05752e27          	fsw	fs7,92(a0)
2005c2bc:	07852027          	fsw	fs8,96(a0)
2005c2c0:	07952227          	fsw	fs9,100(a0)
2005c2c4:	07a52427          	fsw	fs10,104(a0)
2005c2c8:	07b52627          	fsw	fs11,108(a0)
2005c2cc:	07c52827          	fsw	ft8,112(a0)
2005c2d0:	07d52a27          	fsw	ft9,116(a0)
2005c2d4:	07e52c27          	fsw	ft10,120(a0)
2005c2d8:	07f52e27          	fsw	ft11,124(a0)
2005c2dc:	003022f3          	csrrs	t0,fcsr,zero
2005c2e0:	0055a023          	sw	t0,0(a1)
2005c2e4:	8082                	c.jr	ra

2005c2e6 <vPortRestoreRegs>:
2005c2e6:	00852103          	lw	sp,8(a0)
2005c2ea:	00c52183          	lw	gp,12(a0)
2005c2ee:	01052203          	lw	tp,16(a0)
2005c2f2:	01452283          	lw	t0,20(a0)
2005c2f6:	01452283          	lw	t0,20(a0)
2005c2fa:	01852303          	lw	t1,24(a0)
2005c2fe:	01c52383          	lw	t2,28(a0)
2005c302:	5100                	c.lw	s0,32(a0)
2005c304:	5144                	c.lw	s1,36(a0)
2005c306:	5508                	c.lw	a0,40(a0)
2005c308:	554c                	c.lw	a1,44(a0)
2005c30a:	5910                	c.lw	a2,48(a0)
2005c30c:	5954                	c.lw	a3,52(a0)
2005c30e:	5d18                	c.lw	a4,56(a0)
2005c310:	5d5c                	c.lw	a5,60(a0)
2005c312:	04052803          	lw	a6,64(a0)
2005c316:	04452883          	lw	a7,68(a0)
2005c31a:	04852903          	lw	s2,72(a0)
2005c31e:	04c52983          	lw	s3,76(a0)
2005c322:	05052a03          	lw	s4,80(a0)
2005c326:	05452a83          	lw	s5,84(a0)
2005c32a:	05852b03          	lw	s6,88(a0)
2005c32e:	05c52b83          	lw	s7,92(a0)
2005c332:	06052c03          	lw	s8,96(a0)
2005c336:	06452c83          	lw	s9,100(a0)
2005c33a:	06852d03          	lw	s10,104(a0)
2005c33e:	06c52d83          	lw	s11,108(a0)
2005c342:	07052e03          	lw	t3,112(a0)
2005c346:	07452e83          	lw	t4,116(a0)
2005c34a:	07852f03          	lw	t5,120(a0)
2005c34e:	07c52f83          	lw	t6,124(a0)
2005c352:	0005a083          	lw	ra,0(a1)
2005c356:	30009073          	csrrw	zero,mstatus,ra
2005c35a:	0045a083          	lw	ra,4(a1)
2005c35e:	30409073          	csrrw	zero,mie,ra
2005c362:	0085a083          	lw	ra,8(a1)
2005c366:	30509073          	csrrw	zero,mtvec,ra
2005c36a:	00c5a083          	lw	ra,12(a1)
2005c36e:	8082                	c.jr	ra

2005c370 <vPortRestorefloatRegs>:
2005c370:	00062283          	lw	t0,0(a2)
2005c374:	6319                	c.lui	t1,0x6
2005c376:	0062f2b3          	and	t0,t0,t1
2005c37a:	06629e63          	bne	t0,t1,2005c3f6 <vPortRestorefloatRegs+0x86>
2005c37e:	00052007          	flw	ft0,0(a0)
2005c382:	00452087          	flw	ft1,4(a0)
2005c386:	00852107          	flw	ft2,8(a0)
2005c38a:	00c52187          	flw	ft3,12(a0)
2005c38e:	01052207          	flw	ft4,16(a0)
2005c392:	01452287          	flw	ft5,20(a0)
2005c396:	01852307          	flw	ft6,24(a0)
2005c39a:	01c52387          	flw	ft7,28(a0)
2005c39e:	7100                	c.flw	fs0,32(a0)
2005c3a0:	7144                	c.flw	fs1,36(a0)
2005c3a2:	7508                	c.flw	fa0,40(a0)
2005c3a4:	754c                	c.flw	fa1,44(a0)
2005c3a6:	7910                	c.flw	fa2,48(a0)
2005c3a8:	7954                	c.flw	fa3,52(a0)
2005c3aa:	7d18                	c.flw	fa4,56(a0)
2005c3ac:	7d5c                	c.flw	fa5,60(a0)
2005c3ae:	04052807          	flw	fa6,64(a0)
2005c3b2:	04452887          	flw	fa7,68(a0)
2005c3b6:	04852907          	flw	fs2,72(a0)
2005c3ba:	04c52987          	flw	fs3,76(a0)
2005c3be:	05052a07          	flw	fs4,80(a0)
2005c3c2:	05452a87          	flw	fs5,84(a0)
2005c3c6:	05852b07          	flw	fs6,88(a0)
2005c3ca:	05c52b87          	flw	fs7,92(a0)
2005c3ce:	06052c07          	flw	fs8,96(a0)
2005c3d2:	06452c87          	flw	fs9,100(a0)
2005c3d6:	06852d07          	flw	fs10,104(a0)
2005c3da:	06c52d87          	flw	fs11,108(a0)
2005c3de:	07052e07          	flw	ft8,112(a0)
2005c3e2:	07452e87          	flw	ft9,116(a0)
2005c3e6:	07852f07          	flw	ft10,120(a0)
2005c3ea:	07c52f87          	flw	ft11,124(a0)
2005c3ee:	0005a283          	lw	t0,0(a1)
2005c3f2:	00329073          	csrrw	zero,fcsr,t0
2005c3f6:	8082                	c.jr	ra

2005c3f8 <vPortBackupPmp>:
2005c3f8:	3b0022f3          	csrrs	t0,pmpaddr0,zero
2005c3fc:	00552023          	sw	t0,0(a0)
2005c400:	3b1022f3          	csrrs	t0,pmpaddr1,zero
2005c404:	00552223          	sw	t0,4(a0)
2005c408:	3b2022f3          	csrrs	t0,pmpaddr2,zero
2005c40c:	00552423          	sw	t0,8(a0)
2005c410:	3b3022f3          	csrrs	t0,pmpaddr3,zero
2005c414:	00552623          	sw	t0,12(a0)
2005c418:	3b4022f3          	csrrs	t0,pmpaddr4,zero
2005c41c:	00552823          	sw	t0,16(a0)
2005c420:	3b5022f3          	csrrs	t0,pmpaddr5,zero
2005c424:	00552a23          	sw	t0,20(a0)
2005c428:	3b6022f3          	csrrs	t0,pmpaddr6,zero
2005c42c:	00552c23          	sw	t0,24(a0)
2005c430:	3b7022f3          	csrrs	t0,pmpaddr7,zero
2005c434:	00552e23          	sw	t0,28(a0)
2005c438:	3b8022f3          	csrrs	t0,pmpaddr8,zero
2005c43c:	02552023          	sw	t0,32(a0)
2005c440:	3b9022f3          	csrrs	t0,pmpaddr9,zero
2005c444:	02552223          	sw	t0,36(a0)
2005c448:	3ba022f3          	csrrs	t0,pmpaddr10,zero
2005c44c:	02552423          	sw	t0,40(a0)
2005c450:	3bb022f3          	csrrs	t0,pmpaddr11,zero
2005c454:	02552623          	sw	t0,44(a0)
2005c458:	3bc022f3          	csrrs	t0,pmpaddr12,zero
2005c45c:	02552823          	sw	t0,48(a0)
2005c460:	3bd022f3          	csrrs	t0,pmpaddr13,zero
2005c464:	02552a23          	sw	t0,52(a0)
2005c468:	3be022f3          	csrrs	t0,pmpaddr14,zero
2005c46c:	02552c23          	sw	t0,56(a0)
2005c470:	3bf022f3          	csrrs	t0,pmpaddr15,zero
2005c474:	02552e23          	sw	t0,60(a0)
2005c478:	3a0022f3          	csrrs	t0,pmpcfg0,zero
2005c47c:	04552023          	sw	t0,64(a0)
2005c480:	3a2022f3          	csrrs	t0,pmpcfg2,zero
2005c484:	04552223          	sw	t0,68(a0)
2005c488:	3a1022f3          	csrrs	t0,pmpcfg1,zero
2005c48c:	04552423          	sw	t0,72(a0)
2005c490:	3a3022f3          	csrrs	t0,pmpcfg3,zero
2005c494:	04552623          	sw	t0,76(a0)
2005c498:	8082                	c.jr	ra

2005c49a <vPortRestorePmp>:
2005c49a:	00052283          	lw	t0,0(a0)
2005c49e:	3b029073          	csrrw	zero,pmpaddr0,t0
2005c4a2:	00452283          	lw	t0,4(a0)
2005c4a6:	3b129073          	csrrw	zero,pmpaddr1,t0
2005c4aa:	00852283          	lw	t0,8(a0)
2005c4ae:	3b229073          	csrrw	zero,pmpaddr2,t0
2005c4b2:	00c52283          	lw	t0,12(a0)
2005c4b6:	3b329073          	csrrw	zero,pmpaddr3,t0
2005c4ba:	01052283          	lw	t0,16(a0)
2005c4be:	3b429073          	csrrw	zero,pmpaddr4,t0
2005c4c2:	01452283          	lw	t0,20(a0)
2005c4c6:	3b529073          	csrrw	zero,pmpaddr5,t0
2005c4ca:	01852283          	lw	t0,24(a0)
2005c4ce:	3b629073          	csrrw	zero,pmpaddr6,t0
2005c4d2:	01c52283          	lw	t0,28(a0)
2005c4d6:	3b729073          	csrrw	zero,pmpaddr7,t0
2005c4da:	02052283          	lw	t0,32(a0)
2005c4de:	3b829073          	csrrw	zero,pmpaddr8,t0
2005c4e2:	02452283          	lw	t0,36(a0)
2005c4e6:	3b929073          	csrrw	zero,pmpaddr9,t0
2005c4ea:	02852283          	lw	t0,40(a0)
2005c4ee:	3ba29073          	csrrw	zero,pmpaddr10,t0
2005c4f2:	02c52283          	lw	t0,44(a0)
2005c4f6:	3bb29073          	csrrw	zero,pmpaddr11,t0
2005c4fa:	03052283          	lw	t0,48(a0)
2005c4fe:	3bc29073          	csrrw	zero,pmpaddr12,t0
2005c502:	03452283          	lw	t0,52(a0)
2005c506:	3bd29073          	csrrw	zero,pmpaddr13,t0
2005c50a:	03852283          	lw	t0,56(a0)
2005c50e:	3be29073          	csrrw	zero,pmpaddr14,t0
2005c512:	03c52283          	lw	t0,60(a0)
2005c516:	3bf29073          	csrrw	zero,pmpaddr15,t0
2005c51a:	04052283          	lw	t0,64(a0)
2005c51e:	3a029073          	csrrw	zero,pmpcfg0,t0
2005c522:	04452283          	lw	t0,68(a0)
2005c526:	3a229073          	csrrw	zero,pmpcfg2,t0
2005c52a:	04852283          	lw	t0,72(a0)
2005c52e:	3a129073          	csrrw	zero,pmpcfg1,t0
2005c532:	04c52283          	lw	t0,76(a0)
2005c536:	3a329073          	csrrw	zero,pmpcfg3,t0
2005c53a:	8082                	c.jr	ra
2005c53c:	0000                	c.unimp
2005c53e:	0000                	c.unimp
	...

2005c542 <vPortSetupTimerInterrupt>:
2005c542:	e681a783          	lw	a5,-408(gp) # 200667f0 <SystemCoreClock>
2005c546:	3e800713          	addi	a4,zero,1000
2005c54a:	02e7d7b3          	divu	a5,a5,a4
2005c54e:	1141                	c.addi	sp,-16
2005c550:	f4f1a823          	sw	a5,-176(gp) # 200668d8 <uxTimerIncrementsForOneTick>
2005c554:	f14027f3          	csrrs	a5,mhartid,zero
2005c558:	c63e                	c.swsp	a5,12(sp)
2005c55a:	4632                	c.lwsp	a2,12(sp)
2005c55c:	10000737          	lui	a4,0x10000
2005c560:	00170793          	addi	a5,a4,1 # 10000001 <__km4_boot_text_start__+0x7fffe1>
2005c564:	963e                	c.add	a2,a5
2005c566:	060e                	c.slli	a2,0x3
2005c568:	f4c1a023          	sw	a2,-192(gp) # 200668c8 <pullMachineTimerCompareRegister>
2005c56c:	47b2                	c.lwsp	a5,12(sp)
2005c56e:	f5018593          	addi	a1,gp,-176 # 200668d8 <uxTimerIncrementsForOneTick>
2005c572:	97ba                	c.add	a5,a4
2005c574:	078e                	c.slli	a5,0x3
2005c576:	f4f1a223          	sw	a5,-188(gp) # 200668cc <pullMachineTimerCounterRegister>
2005c57a:	800007b7          	lui	a5,0x80000
2005c57e:	43c8                	c.lw	a0,4(a5)
2005c580:	4394                	c.lw	a3,0(a5)
2005c582:	43d8                	c.lw	a4,4(a5)
2005c584:	fea71de3          	bne	a4,a0,2005c57e <vPortSetupTimerInterrupt+0x3c>
2005c588:	4188                	c.lw	a0,0(a1)
2005c58a:	9536                	c.add	a0,a3
2005c58c:	c208                	c.sw	a0,0(a2)
2005c58e:	419c                	c.lw	a5,0(a1)
2005c590:	00d536b3          	sltu	a3,a0,a3
2005c594:	96ba                	c.add	a3,a4
2005c596:	00a78733          	add	a4,a5,a0
2005c59a:	c254                	c.sw	a3,4(a2)
2005c59c:	00f737b3          	sltu	a5,a4,a5
2005c5a0:	f4818613          	addi	a2,gp,-184 # 200668d0 <ullNextTime>
2005c5a4:	96be                	c.add	a3,a5
2005c5a6:	c218                	c.sw	a4,0(a2)
2005c5a8:	c254                	c.sw	a3,4(a2)
2005c5aa:	0141                	c.addi	sp,16
2005c5ac:	8082                	c.jr	ra

2005c5ae <xPortStartScheduler>:
2005c5ae:	1141                	c.addi	sp,-16
2005c5b0:	c606                	c.swsp	ra,12(sp)
2005c5b2:	f91ff0ef          	jal	ra,2005c542 <vPortSetupTimerInterrupt>
2005c5b6:	6785                	c.lui	a5,0x1
2005c5b8:	88078793          	addi	a5,a5,-1920 # 880 <__NVIC_GetVectorArg+0xce>
2005c5bc:	3047a073          	csrrs	zero,mie,a5
2005c5c0:	b11ff0ef          	jal	ra,2005c0d0 <xPortStartFirstTask>
2005c5c4:	40b2                	c.lwsp	ra,12(sp)
2005c5c6:	4501                	c.li	a0,0
2005c5c8:	0141                	c.addi	sp,16
2005c5ca:	8082                	c.jr	ra

2005c5cc <vApplicationTickHook>:
2005c5cc:	8082                	c.jr	ra

2005c5ce <vApplicationIdleHook>:
2005c5ce:	1101                	c.addi	sp,-32
2005c5d0:	ce06                	c.swsp	ra,28(sp)
2005c5d2:	f74fe0ef          	jal	ra,2005ad46 <xPortGetFreeHeapSize>
2005c5d6:	40f2                	c.lwsp	ra,28(sp)
2005c5d8:	c62a                	c.swsp	a0,12(sp)
2005c5da:	47b2                	c.lwsp	a5,12(sp)
2005c5dc:	6105                	c.addi16sp	sp,32
2005c5de:	8082                	c.jr	ra

2005c5e0 <vApplicationMallocFailedHook>:
2005c5e0:	1101                	c.addi	sp,-32
2005c5e2:	ce06                	c.swsp	ra,28(sp)
2005c5e4:	43c010ef          	jal	ra,2005da20 <xTaskGetSchedulerState>
2005c5e8:	4785                	c.li	a5,1
2005c5ea:	02f50863          	beq	a0,a5,2005c61a <vApplicationMallocFailedHook+0x3a>
2005c5ee:	4501                	c.li	a0,0
2005c5f0:	104010ef          	jal	ra,2005d6f4 <pcTaskGetName>
2005c5f4:	862a                	c.mv	a2,a0
2005c5f6:	c632                	c.swsp	a2,12(sp)
2005c5f8:	f4efe0ef          	jal	ra,2005ad46 <xPortGetFreeHeapSize>
2005c5fc:	4632                	c.lwsp	a2,12(sp)
2005c5fe:	86aa                	c.mv	a3,a0
2005c600:	200635b7          	lui	a1,0x20063
2005c604:	20063537          	lui	a0,0x20063
2005c608:	4b458593          	addi	a1,a1,1204 # 200634b4 <__FUNCTION__.0+0x1c>
2005c60c:	4b850513          	addi	a0,a0,1208 # 200634b8 <__FUNCTION__.0+0x20>
2005c610:	57f020ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005c614:	30047073          	csrrci	zero,mstatus,8
2005c618:	a001                	c.j	2005c618 <vApplicationMallocFailedHook+0x38>
2005c61a:	20063637          	lui	a2,0x20063
2005c61e:	4ac60613          	addi	a2,a2,1196 # 200634ac <__FUNCTION__.0+0x14>
2005c622:	bfd1                	c.j	2005c5f6 <vApplicationMallocFailedHook+0x16>

2005c624 <vApplicationStackOverflowHook>:
2005c624:	862e                	c.mv	a2,a1
2005c626:	20063537          	lui	a0,0x20063
2005c62a:	200635b7          	lui	a1,0x20063
2005c62e:	1141                	c.addi	sp,-16
2005c630:	51c58593          	addi	a1,a1,1308 # 2006351c <__FUNCTION__.5>
2005c634:	4f450513          	addi	a0,a0,1268 # 200634f4 <__FUNCTION__.0+0x5c>
2005c638:	c606                	c.swsp	ra,12(sp)
2005c63a:	555020ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005c63e:	a001                	c.j	2005c63e <vApplicationStackOverflowHook+0x1a>

2005c640 <vApplicationGetIdleTaskMemory>:
2005c640:	200687b7          	lui	a5,0x20068
2005c644:	f6078793          	addi	a5,a5,-160 # 20067f60 <xIdleTaskTCB.4>
2005c648:	c11c                	c.sw	a5,0(a0)
2005c64a:	200657b7          	lui	a5,0x20065
2005c64e:	dc078793          	addi	a5,a5,-576 # 20064dc0 <uxIdleTaskStack.3>
2005c652:	c19c                	c.sw	a5,0(a1)
2005c654:	20000793          	addi	a5,zero,512
2005c658:	c21c                	c.sw	a5,0(a2)
2005c65a:	8082                	c.jr	ra

2005c65c <vApplicationGetTimerTaskMemory>:
2005c65c:	200687b7          	lui	a5,0x20068
2005c660:	0c478793          	addi	a5,a5,196 # 200680c4 <xTimerTaskTCB.2>
2005c664:	c11c                	c.sw	a5,0(a0)
2005c666:	200657b7          	lui	a5,0x20065
2005c66a:	5c078793          	addi	a5,a5,1472 # 200655c0 <uxTimerTaskStack.1>
2005c66e:	c19c                	c.sw	a5,0(a1)
2005c670:	20000793          	addi	a5,zero,512
2005c674:	c21c                	c.sw	a5,0(a2)
2005c676:	8082                	c.jr	ra

2005c678 <vPortSuppressTicksAndSleep>:
2005c678:	1141                	c.addi	sp,-16
2005c67a:	c422                	c.swsp	s0,8(sp)
2005c67c:	4501                	c.li	a0,0
2005c67e:	c606                	c.swsp	ra,12(sp)
2005c680:	e601a823          	sw	zero,-400(gp) # 200667f8 <system_can_yield>
2005c684:	c26fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c688:	300477f3          	csrrci	a5,mstatus,8
2005c68c:	35e010ef          	jal	ra,2005d9ea <eTaskConfirmSleepModeStatus>
2005c690:	c90d                	c.beqz	a0,2005c6c2 <vPortSuppressTicksAndSleep+0x4a>
2005c692:	96afd0ef          	jal	ra,200597fc <pmu_ready_to_sleep>
2005c696:	e515                	c.bnez	a0,2005c6c2 <vPortSuppressTicksAndSleep+0x4a>
2005c698:	4505                	c.li	a0,1
2005c69a:	c10fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c69e:	0ff0000f          	fence	iorw,iorw
2005c6a2:	10500073          	wfi
2005c6a6:	0001                	c.addi	zero,0
2005c6a8:	0ff0000f          	fence	iorw,iorw
2005c6ac:	0001                	c.addi	zero,0
2005c6ae:	0001                	c.addi	zero,0
2005c6b0:	0001                	c.addi	zero,0
2005c6b2:	0001                	c.addi	zero,0
2005c6b4:	0001                	c.addi	zero,0
2005c6b6:	0001                	c.addi	zero,0
2005c6b8:	0001                	c.addi	zero,0
2005c6ba:	0001                	c.addi	zero,0
2005c6bc:	0001                	c.addi	zero,0
2005c6be:	0001                	c.addi	zero,0
2005c6c0:	0001                	c.addi	zero,0
2005c6c2:	300467f3          	csrrsi	a5,mstatus,8
2005c6c6:	4505                	c.li	a0,1
2005c6c8:	be2fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c6cc:	4785                	c.li	a5,1
2005c6ce:	40b2                	c.lwsp	ra,12(sp)
2005c6d0:	e6f1a823          	sw	a5,-400(gp) # 200667f8 <system_can_yield>
2005c6d4:	4422                	c.lwsp	s0,8(sp)
2005c6d6:	0141                	c.addi	sp,16
2005c6d8:	8082                	c.jr	ra

2005c6da <prvIsQueueEmpty>:
2005c6da:	1141                	c.addi	sp,-16
2005c6dc:	c422                	c.swsp	s0,8(sp)
2005c6de:	842a                	c.mv	s0,a0
2005c6e0:	c606                	c.swsp	ra,12(sp)
2005c6e2:	520010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005c6e6:	5c00                	c.lw	s0,56(s0)
2005c6e8:	534010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c6ec:	40b2                	c.lwsp	ra,12(sp)
2005c6ee:	00143513          	sltiu	a0,s0,1
2005c6f2:	4422                	c.lwsp	s0,8(sp)
2005c6f4:	0141                	c.addi	sp,16
2005c6f6:	8082                	c.jr	ra

2005c6f8 <prvCopyDataToQueue>:
2005c6f8:	1141                	c.addi	sp,-16
2005c6fa:	c422                	c.swsp	s0,8(sp)
2005c6fc:	c226                	c.swsp	s1,4(sp)
2005c6fe:	c606                	c.swsp	ra,12(sp)
2005c700:	c04a                	c.swsp	s2,0(sp)
2005c702:	84b2                	c.mv	s1,a2
2005c704:	4130                	c.lw	a2,64(a0)
2005c706:	03852903          	lw	s2,56(a0)
2005c70a:	842a                	c.mv	s0,a0
2005c70c:	e605                	c.bnez	a2,2005c734 <prvCopyDataToQueue+0x3c>
2005c70e:	411c                	c.lw	a5,0(a0)
2005c710:	4481                	c.li	s1,0
2005c712:	e799                	c.bnez	a5,2005c720 <prvCopyDataToQueue+0x28>
2005c714:	4508                	c.lw	a0,8(a0)
2005c716:	3d2010ef          	jal	ra,2005dae8 <xTaskPriorityDisinherit>
2005c71a:	84aa                	c.mv	s1,a0
2005c71c:	00042423          	sw	zero,8(s0)
2005c720:	0905                	c.addi	s2,1
2005c722:	03242c23          	sw	s2,56(s0)
2005c726:	40b2                	c.lwsp	ra,12(sp)
2005c728:	4422                	c.lwsp	s0,8(sp)
2005c72a:	4902                	c.lwsp	s2,0(sp)
2005c72c:	8526                	c.mv	a0,s1
2005c72e:	4492                	c.lwsp	s1,4(sp)
2005c730:	0141                	c.addi	sp,16
2005c732:	8082                	c.jr	ra
2005c734:	e085                	c.bnez	s1,2005c754 <prvCopyDataToQueue+0x5c>
2005c736:	4148                	c.lw	a0,4(a0)
2005c738:	dffad097          	auipc	ra,0xdffad
2005c73c:	398080e7          	jalr	ra,920(ra) # 9ad0 <__wrap_memcpy>
2005c740:	405c                	c.lw	a5,4(s0)
2005c742:	4038                	c.lw	a4,64(s0)
2005c744:	97ba                	c.add	a5,a4
2005c746:	4418                	c.lw	a4,8(s0)
2005c748:	c05c                	c.sw	a5,4(s0)
2005c74a:	fce7ebe3          	bltu	a5,a4,2005c720 <prvCopyDataToQueue+0x28>
2005c74e:	401c                	c.lw	a5,0(s0)
2005c750:	c05c                	c.sw	a5,4(s0)
2005c752:	b7f9                	c.j	2005c720 <prvCopyDataToQueue+0x28>
2005c754:	4548                	c.lw	a0,12(a0)
2005c756:	dffad097          	auipc	ra,0xdffad
2005c75a:	37a080e7          	jalr	ra,890(ra) # 9ad0 <__wrap_memcpy>
2005c75e:	4038                	c.lw	a4,64(s0)
2005c760:	445c                	c.lw	a5,12(s0)
2005c762:	40e006b3          	sub	a3,zero,a4
2005c766:	8f99                	c.sub	a5,a4
2005c768:	4018                	c.lw	a4,0(s0)
2005c76a:	c45c                	c.sw	a5,12(s0)
2005c76c:	00e7f563          	bgeu	a5,a4,2005c776 <prvCopyDataToQueue+0x7e>
2005c770:	441c                	c.lw	a5,8(s0)
2005c772:	97b6                	c.add	a5,a3
2005c774:	c45c                	c.sw	a5,12(s0)
2005c776:	4789                	c.li	a5,2
2005c778:	00f49763          	bne	s1,a5,2005c786 <prvCopyDataToQueue+0x8e>
2005c77c:	4481                	c.li	s1,0
2005c77e:	fa0901e3          	beq	s2,zero,2005c720 <prvCopyDataToQueue+0x28>
2005c782:	197d                	c.addi	s2,-1
2005c784:	bf71                	c.j	2005c720 <prvCopyDataToQueue+0x28>
2005c786:	4481                	c.li	s1,0
2005c788:	bf61                	c.j	2005c720 <prvCopyDataToQueue+0x28>

2005c78a <prvCopyDataFromQueue>:
2005c78a:	87aa                	c.mv	a5,a0
2005c78c:	43b0                	c.lw	a2,64(a5)
2005c78e:	852e                	c.mv	a0,a1
2005c790:	ce11                	c.beqz	a2,2005c7ac <prvCopyDataFromQueue+0x22>
2005c792:	47d8                	c.lw	a4,12(a5)
2005c794:	4794                	c.lw	a3,8(a5)
2005c796:	9732                	c.add	a4,a2
2005c798:	c7d8                	c.sw	a4,12(a5)
2005c79a:	00d76463          	bltu	a4,a3,2005c7a2 <prvCopyDataFromQueue+0x18>
2005c79e:	4398                	c.lw	a4,0(a5)
2005c7a0:	c7d8                	c.sw	a4,12(a5)
2005c7a2:	47cc                	c.lw	a1,12(a5)
2005c7a4:	dffad317          	auipc	t1,0xdffad
2005c7a8:	32c30067          	jalr	zero,812(t1) # 9ad0 <__wrap_memcpy>
2005c7ac:	8082                	c.jr	ra

2005c7ae <prvUnlockQueue>:
2005c7ae:	1141                	c.addi	sp,-16
2005c7b0:	c422                	c.swsp	s0,8(sp)
2005c7b2:	842a                	c.mv	s0,a0
2005c7b4:	c226                	c.swsp	s1,4(sp)
2005c7b6:	c04a                	c.swsp	s2,0(sp)
2005c7b8:	c606                	c.swsp	ra,12(sp)
2005c7ba:	448010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005c7be:	04544483          	lbu	s1,69(s0)
2005c7c2:	02440913          	addi	s2,s0,36
2005c7c6:	04e2                	c.slli	s1,0x18
2005c7c8:	84e1                	c.srai	s1,0x18
2005c7ca:	02904b63          	blt	zero,s1,2005c800 <prvUnlockQueue+0x52>
2005c7ce:	57fd                	c.li	a5,-1
2005c7d0:	04f402a3          	sb	a5,69(s0)
2005c7d4:	448010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c7d8:	42a010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005c7dc:	04444483          	lbu	s1,68(s0)
2005c7e0:	01040913          	addi	s2,s0,16
2005c7e4:	04e2                	c.slli	s1,0x18
2005c7e6:	84e1                	c.srai	s1,0x18
2005c7e8:	02904863          	blt	zero,s1,2005c818 <prvUnlockQueue+0x6a>
2005c7ec:	57fd                	c.li	a5,-1
2005c7ee:	04f40223          	sb	a5,68(s0)
2005c7f2:	4422                	c.lwsp	s0,8(sp)
2005c7f4:	40b2                	c.lwsp	ra,12(sp)
2005c7f6:	4492                	c.lwsp	s1,4(sp)
2005c7f8:	4902                	c.lwsp	s2,0(sp)
2005c7fa:	0141                	c.addi	sp,16
2005c7fc:	4200106f          	jal	zero,2005dc1c <vTaskExitCritical>
2005c800:	505c                	c.lw	a5,36(s0)
2005c802:	d7f1                	c.beqz	a5,2005c7ce <prvUnlockQueue+0x20>
2005c804:	854a                	c.mv	a0,s2
2005c806:	14c010ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005c80a:	c119                	c.beqz	a0,2005c810 <prvUnlockQueue+0x62>
2005c80c:	1d6010ef          	jal	ra,2005d9e2 <vTaskMissedYield>
2005c810:	14fd                	c.addi	s1,-1
2005c812:	04e2                	c.slli	s1,0x18
2005c814:	84e1                	c.srai	s1,0x18
2005c816:	bf55                	c.j	2005c7ca <prvUnlockQueue+0x1c>
2005c818:	481c                	c.lw	a5,16(s0)
2005c81a:	dbe9                	c.beqz	a5,2005c7ec <prvUnlockQueue+0x3e>
2005c81c:	854a                	c.mv	a0,s2
2005c81e:	134010ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005c822:	c119                	c.beqz	a0,2005c828 <prvUnlockQueue+0x7a>
2005c824:	1be010ef          	jal	ra,2005d9e2 <vTaskMissedYield>
2005c828:	14fd                	c.addi	s1,-1
2005c82a:	04e2                	c.slli	s1,0x18
2005c82c:	84e1                	c.srai	s1,0x18
2005c82e:	bf6d                	c.j	2005c7e8 <prvUnlockQueue+0x3a>

2005c830 <xQueueGenericReset>:
2005c830:	1141                	c.addi	sp,-16
2005c832:	c422                	c.swsp	s0,8(sp)
2005c834:	c226                	c.swsp	s1,4(sp)
2005c836:	842a                	c.mv	s0,a0
2005c838:	c606                	c.swsp	ra,12(sp)
2005c83a:	84ae                	c.mv	s1,a1
2005c83c:	3c6010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005c840:	4034                	c.lw	a3,64(s0)
2005c842:	5c5c                	c.lw	a5,60(s0)
2005c844:	4018                	c.lw	a4,0(s0)
2005c846:	02042c23          	sw	zero,56(s0)
2005c84a:	02f687b3          	mul	a5,a3,a5
2005c84e:	c058                	c.sw	a4,4(s0)
2005c850:	00f70633          	add	a2,a4,a5
2005c854:	8f95                	c.sub	a5,a3
2005c856:	97ba                	c.add	a5,a4
2005c858:	c45c                	c.sw	a5,12(s0)
2005c85a:	57fd                	c.li	a5,-1
2005c85c:	04f40223          	sb	a5,68(s0)
2005c860:	c410                	c.sw	a2,8(s0)
2005c862:	04f402a3          	sb	a5,69(s0)
2005c866:	e095                	c.bnez	s1,2005c88a <xQueueGenericReset+0x5a>
2005c868:	481c                	c.lw	a5,16(s0)
2005c86a:	cb81                	c.beqz	a5,2005c87a <xQueueGenericReset+0x4a>
2005c86c:	01040513          	addi	a0,s0,16
2005c870:	0e2010ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005c874:	c119                	c.beqz	a0,2005c87a <xQueueGenericReset+0x4a>
2005c876:	00000073          	ecall
2005c87a:	3a2010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c87e:	40b2                	c.lwsp	ra,12(sp)
2005c880:	4422                	c.lwsp	s0,8(sp)
2005c882:	4492                	c.lwsp	s1,4(sp)
2005c884:	4505                	c.li	a0,1
2005c886:	0141                	c.addi	sp,16
2005c888:	8082                	c.jr	ra
2005c88a:	01040513          	addi	a0,s0,16
2005c88e:	db6fe0ef          	jal	ra,2005ae44 <vListInitialise>
2005c892:	02440513          	addi	a0,s0,36
2005c896:	daefe0ef          	jal	ra,2005ae44 <vListInitialise>
2005c89a:	b7c5                	c.j	2005c87a <xQueueGenericReset+0x4a>

2005c89c <xQueueGetMutexHolder>:
2005c89c:	1101                	c.addi	sp,-32
2005c89e:	cc22                	c.swsp	s0,24(sp)
2005c8a0:	ce06                	c.swsp	ra,28(sp)
2005c8a2:	842a                	c.mv	s0,a0
2005c8a4:	35e010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005c8a8:	401c                	c.lw	a5,0(s0)
2005c8aa:	4501                	c.li	a0,0
2005c8ac:	e391                	c.bnez	a5,2005c8b0 <xQueueGetMutexHolder+0x14>
2005c8ae:	4408                	c.lw	a0,8(s0)
2005c8b0:	c62a                	c.swsp	a0,12(sp)
2005c8b2:	36a010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c8b6:	40f2                	c.lwsp	ra,28(sp)
2005c8b8:	4462                	c.lwsp	s0,24(sp)
2005c8ba:	4532                	c.lwsp	a0,12(sp)
2005c8bc:	6105                	c.addi16sp	sp,32
2005c8be:	8082                	c.jr	ra

2005c8c0 <xQueueGenericSend>:
2005c8c0:	7139                	c.addi16sp	sp,-64
2005c8c2:	dc22                	c.swsp	s0,56(sp)
2005c8c4:	da26                	c.swsp	s1,52(sp)
2005c8c6:	d84a                	c.swsp	s2,48(sp)
2005c8c8:	d64e                	c.swsp	s3,44(sp)
2005c8ca:	d452                	c.swsp	s4,40(sp)
2005c8cc:	d256                	c.swsp	s5,36(sp)
2005c8ce:	d05a                	c.swsp	s6,32(sp)
2005c8d0:	de06                	c.swsp	ra,60(sp)
2005c8d2:	842a                	c.mv	s0,a0
2005c8d4:	892e                	c.mv	s2,a1
2005c8d6:	c632                	c.swsp	a2,12(sp)
2005c8d8:	84b6                	c.mv	s1,a3
2005c8da:	4a01                	c.li	s4,0
2005c8dc:	4a89                	c.li	s5,2
2005c8de:	59fd                	c.li	s3,-1
2005c8e0:	01050b13          	addi	s6,a0,16
2005c8e4:	31e010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005c8e8:	5c18                	c.lw	a4,56(s0)
2005c8ea:	5c5c                	c.lw	a5,60(s0)
2005c8ec:	00f76463          	bltu	a4,a5,2005c8f4 <xQueueGenericSend+0x34>
2005c8f0:	03549d63          	bne	s1,s5,2005c92a <xQueueGenericSend+0x6a>
2005c8f4:	8626                	c.mv	a2,s1
2005c8f6:	85ca                	c.mv	a1,s2
2005c8f8:	8522                	c.mv	a0,s0
2005c8fa:	dffff0ef          	jal	ra,2005c6f8 <prvCopyDataToQueue>
2005c8fe:	505c                	c.lw	a5,36(s0)
2005c900:	c789                	c.beqz	a5,2005c90a <xQueueGenericSend+0x4a>
2005c902:	02440513          	addi	a0,s0,36
2005c906:	04c010ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005c90a:	c119                	c.beqz	a0,2005c910 <xQueueGenericSend+0x50>
2005c90c:	00000073          	ecall
2005c910:	30c010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c914:	4505                	c.li	a0,1
2005c916:	50f2                	c.lwsp	ra,60(sp)
2005c918:	5462                	c.lwsp	s0,56(sp)
2005c91a:	54d2                	c.lwsp	s1,52(sp)
2005c91c:	5942                	c.lwsp	s2,48(sp)
2005c91e:	59b2                	c.lwsp	s3,44(sp)
2005c920:	5a22                	c.lwsp	s4,40(sp)
2005c922:	5a92                	c.lwsp	s5,36(sp)
2005c924:	5b02                	c.lwsp	s6,32(sp)
2005c926:	6121                	c.addi16sp	sp,64
2005c928:	8082                	c.jr	ra
2005c92a:	47b2                	c.lwsp	a5,12(sp)
2005c92c:	e789                	c.bnez	a5,2005c936 <xQueueGenericSend+0x76>
2005c92e:	2ee010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c932:	4501                	c.li	a0,0
2005c934:	b7cd                	c.j	2005c916 <xQueueGenericSend+0x56>
2005c936:	000a1563          	bne	s4,zero,2005c940 <xQueueGenericSend+0x80>
2005c93a:	0828                	c.addi4spn	a0,sp,24
2005c93c:	098010ef          	jal	ra,2005d9d4 <vTaskInternalSetTimeOutState>
2005c940:	2dc010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c944:	599000ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005c948:	2ba010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005c94c:	04444783          	lbu	a5,68(s0)
2005c950:	07e2                	c.slli	a5,0x18
2005c952:	87e1                	c.srai	a5,0x18
2005c954:	01379463          	bne	a5,s3,2005c95c <xQueueGenericSend+0x9c>
2005c958:	04040223          	sb	zero,68(s0)
2005c95c:	04544783          	lbu	a5,69(s0)
2005c960:	07e2                	c.slli	a5,0x18
2005c962:	87e1                	c.srai	a5,0x18
2005c964:	01379463          	bne	a5,s3,2005c96c <xQueueGenericSend+0xac>
2005c968:	040402a3          	sb	zero,69(s0)
2005c96c:	2b0010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c970:	006c                	c.addi4spn	a1,sp,12
2005c972:	0828                	c.addi4spn	a0,sp,24
2005c974:	1c1010ef          	jal	ra,2005e334 <xTaskCheckForTimeOut>
2005c978:	e121                	c.bnez	a0,2005c9b8 <xQueueGenericSend+0xf8>
2005c97a:	288010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005c97e:	5c18                	c.lw	a4,56(s0)
2005c980:	5c5c                	c.lw	a5,60(s0)
2005c982:	02f71063          	bne	a4,a5,2005c9a2 <xQueueGenericSend+0xe2>
2005c986:	296010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c98a:	45b2                	c.lwsp	a1,12(sp)
2005c98c:	855a                	c.mv	a0,s6
2005c98e:	77b000ef          	jal	ra,2005d908 <vTaskPlaceOnEventList>
2005c992:	8522                	c.mv	a0,s0
2005c994:	e1bff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005c998:	5f2010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005c99c:	c919                	c.beqz	a0,2005c9b2 <xQueueGenericSend+0xf2>
2005c99e:	4a05                	c.li	s4,1
2005c9a0:	b791                	c.j	2005c8e4 <xQueueGenericSend+0x24>
2005c9a2:	27a010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005c9a6:	8522                	c.mv	a0,s0
2005c9a8:	e07ff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005c9ac:	5de010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005c9b0:	b7fd                	c.j	2005c99e <xQueueGenericSend+0xde>
2005c9b2:	00000073          	ecall
2005c9b6:	b7e5                	c.j	2005c99e <xQueueGenericSend+0xde>
2005c9b8:	8522                	c.mv	a0,s0
2005c9ba:	df5ff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005c9be:	5cc010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005c9c2:	bf85                	c.j	2005c932 <xQueueGenericSend+0x72>

2005c9c4 <xQueueGenericSendFromISR>:
2005c9c4:	1141                	c.addi	sp,-16
2005c9c6:	c422                	c.swsp	s0,8(sp)
2005c9c8:	c04a                	c.swsp	s2,0(sp)
2005c9ca:	c606                	c.swsp	ra,12(sp)
2005c9cc:	c226                	c.swsp	s1,4(sp)
2005c9ce:	5d18                	c.lw	a4,56(a0)
2005c9d0:	5d5c                	c.lw	a5,60(a0)
2005c9d2:	8932                	c.mv	s2,a2
2005c9d4:	842a                	c.mv	s0,a0
2005c9d6:	8636                	c.mv	a2,a3
2005c9d8:	00f76663          	bltu	a4,a5,2005c9e4 <xQueueGenericSendFromISR+0x20>
2005c9dc:	4789                	c.li	a5,2
2005c9de:	4501                	c.li	a0,0
2005c9e0:	02f69063          	bne	a3,a5,2005ca00 <xQueueGenericSendFromISR+0x3c>
2005c9e4:	04544483          	lbu	s1,69(s0)
2005c9e8:	5c1c                	c.lw	a5,56(s0)
2005c9ea:	8522                	c.mv	a0,s0
2005c9ec:	04e2                	c.slli	s1,0x18
2005c9ee:	d0bff0ef          	jal	ra,2005c6f8 <prvCopyDataToQueue>
2005c9f2:	84e1                	c.srai	s1,0x18
2005c9f4:	57fd                	c.li	a5,-1
2005c9f6:	02f49663          	bne	s1,a5,2005ca22 <xQueueGenericSendFromISR+0x5e>
2005c9fa:	505c                	c.lw	a5,36(s0)
2005c9fc:	eb81                	c.bnez	a5,2005ca0c <xQueueGenericSendFromISR+0x48>
2005c9fe:	4505                	c.li	a0,1
2005ca00:	40b2                	c.lwsp	ra,12(sp)
2005ca02:	4422                	c.lwsp	s0,8(sp)
2005ca04:	4492                	c.lwsp	s1,4(sp)
2005ca06:	4902                	c.lwsp	s2,0(sp)
2005ca08:	0141                	c.addi	sp,16
2005ca0a:	8082                	c.jr	ra
2005ca0c:	02440513          	addi	a0,s0,36
2005ca10:	743000ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005ca14:	d56d                	c.beqz	a0,2005c9fe <xQueueGenericSendFromISR+0x3a>
2005ca16:	fe0904e3          	beq	s2,zero,2005c9fe <xQueueGenericSendFromISR+0x3a>
2005ca1a:	4785                	c.li	a5,1
2005ca1c:	00f92023          	sw	a5,0(s2)
2005ca20:	bff9                	c.j	2005c9fe <xQueueGenericSendFromISR+0x3a>
2005ca22:	0485                	c.addi	s1,1
2005ca24:	04e2                	c.slli	s1,0x18
2005ca26:	84e1                	c.srai	s1,0x18
2005ca28:	049402a3          	sb	s1,69(s0)
2005ca2c:	bfc9                	c.j	2005c9fe <xQueueGenericSendFromISR+0x3a>

2005ca2e <xQueueGiveFromISR>:
2005ca2e:	5d18                	c.lw	a4,56(a0)
2005ca30:	5d5c                	c.lw	a5,60(a0)
2005ca32:	04f77663          	bgeu	a4,a5,2005ca7e <xQueueGiveFromISR+0x50>
2005ca36:	04554783          	lbu	a5,69(a0)
2005ca3a:	0705                	c.addi	a4,1
2005ca3c:	dd18                	c.sw	a4,56(a0)
2005ca3e:	07e2                	c.slli	a5,0x18
2005ca40:	87e1                	c.srai	a5,0x18
2005ca42:	577d                	c.li	a4,-1
2005ca44:	02e79763          	bne	a5,a4,2005ca72 <xQueueGiveFromISR+0x44>
2005ca48:	515c                	c.lw	a5,36(a0)
2005ca4a:	e399                	c.bnez	a5,2005ca50 <xQueueGiveFromISR+0x22>
2005ca4c:	4505                	c.li	a0,1
2005ca4e:	8082                	c.jr	ra
2005ca50:	1141                	c.addi	sp,-16
2005ca52:	02450513          	addi	a0,a0,36
2005ca56:	c422                	c.swsp	s0,8(sp)
2005ca58:	c606                	c.swsp	ra,12(sp)
2005ca5a:	842e                	c.mv	s0,a1
2005ca5c:	6f7000ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005ca60:	c501                	c.beqz	a0,2005ca68 <xQueueGiveFromISR+0x3a>
2005ca62:	c019                	c.beqz	s0,2005ca68 <xQueueGiveFromISR+0x3a>
2005ca64:	4785                	c.li	a5,1
2005ca66:	c01c                	c.sw	a5,0(s0)
2005ca68:	40b2                	c.lwsp	ra,12(sp)
2005ca6a:	4422                	c.lwsp	s0,8(sp)
2005ca6c:	4505                	c.li	a0,1
2005ca6e:	0141                	c.addi	sp,16
2005ca70:	8082                	c.jr	ra
2005ca72:	0785                	c.addi	a5,1
2005ca74:	07e2                	c.slli	a5,0x18
2005ca76:	87e1                	c.srai	a5,0x18
2005ca78:	04f502a3          	sb	a5,69(a0)
2005ca7c:	bfc1                	c.j	2005ca4c <xQueueGiveFromISR+0x1e>
2005ca7e:	4501                	c.li	a0,0
2005ca80:	8082                	c.jr	ra

2005ca82 <xQueueReceive>:
2005ca82:	7139                	c.addi16sp	sp,-64
2005ca84:	dc22                	c.swsp	s0,56(sp)
2005ca86:	d84a                	c.swsp	s2,48(sp)
2005ca88:	d64e                	c.swsp	s3,44(sp)
2005ca8a:	d452                	c.swsp	s4,40(sp)
2005ca8c:	d256                	c.swsp	s5,36(sp)
2005ca8e:	de06                	c.swsp	ra,60(sp)
2005ca90:	da26                	c.swsp	s1,52(sp)
2005ca92:	842a                	c.mv	s0,a0
2005ca94:	892e                	c.mv	s2,a1
2005ca96:	c632                	c.swsp	a2,12(sp)
2005ca98:	4a01                	c.li	s4,0
2005ca9a:	59fd                	c.li	s3,-1
2005ca9c:	02450a93          	addi	s5,a0,36
2005caa0:	162010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005caa4:	5c04                	c.lw	s1,56(s0)
2005caa6:	cc85                	c.beqz	s1,2005cade <xQueueReceive+0x5c>
2005caa8:	85ca                	c.mv	a1,s2
2005caaa:	8522                	c.mv	a0,s0
2005caac:	14fd                	c.addi	s1,-1
2005caae:	cddff0ef          	jal	ra,2005c78a <prvCopyDataFromQueue>
2005cab2:	dc04                	c.sw	s1,56(s0)
2005cab4:	481c                	c.lw	a5,16(s0)
2005cab6:	cb81                	c.beqz	a5,2005cac6 <xQueueReceive+0x44>
2005cab8:	01040513          	addi	a0,s0,16
2005cabc:	697000ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005cac0:	c119                	c.beqz	a0,2005cac6 <xQueueReceive+0x44>
2005cac2:	00000073          	ecall
2005cac6:	156010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005caca:	4505                	c.li	a0,1
2005cacc:	50f2                	c.lwsp	ra,60(sp)
2005cace:	5462                	c.lwsp	s0,56(sp)
2005cad0:	54d2                	c.lwsp	s1,52(sp)
2005cad2:	5942                	c.lwsp	s2,48(sp)
2005cad4:	59b2                	c.lwsp	s3,44(sp)
2005cad6:	5a22                	c.lwsp	s4,40(sp)
2005cad8:	5a92                	c.lwsp	s5,36(sp)
2005cada:	6121                	c.addi16sp	sp,64
2005cadc:	8082                	c.jr	ra
2005cade:	47b2                	c.lwsp	a5,12(sp)
2005cae0:	e789                	c.bnez	a5,2005caea <xQueueReceive+0x68>
2005cae2:	13a010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005cae6:	4501                	c.li	a0,0
2005cae8:	b7d5                	c.j	2005cacc <xQueueReceive+0x4a>
2005caea:	000a1563          	bne	s4,zero,2005caf4 <xQueueReceive+0x72>
2005caee:	0828                	c.addi4spn	a0,sp,24
2005caf0:	6e5000ef          	jal	ra,2005d9d4 <vTaskInternalSetTimeOutState>
2005caf4:	128010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005caf8:	3e5000ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005cafc:	106010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005cb00:	04444783          	lbu	a5,68(s0)
2005cb04:	07e2                	c.slli	a5,0x18
2005cb06:	87e1                	c.srai	a5,0x18
2005cb08:	01379463          	bne	a5,s3,2005cb10 <xQueueReceive+0x8e>
2005cb0c:	04040223          	sb	zero,68(s0)
2005cb10:	04544783          	lbu	a5,69(s0)
2005cb14:	07e2                	c.slli	a5,0x18
2005cb16:	87e1                	c.srai	a5,0x18
2005cb18:	01379463          	bne	a5,s3,2005cb20 <xQueueReceive+0x9e>
2005cb1c:	040402a3          	sb	zero,69(s0)
2005cb20:	0fc010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005cb24:	006c                	c.addi4spn	a1,sp,12
2005cb26:	0828                	c.addi4spn	a0,sp,24
2005cb28:	00d010ef          	jal	ra,2005e334 <xTaskCheckForTimeOut>
2005cb2c:	e90d                	c.bnez	a0,2005cb5e <xQueueReceive+0xdc>
2005cb2e:	8522                	c.mv	a0,s0
2005cb30:	babff0ef          	jal	ra,2005c6da <prvIsQueueEmpty>
2005cb34:	cd19                	c.beqz	a0,2005cb52 <xQueueReceive+0xd0>
2005cb36:	45b2                	c.lwsp	a1,12(sp)
2005cb38:	8556                	c.mv	a0,s5
2005cb3a:	5cf000ef          	jal	ra,2005d908 <vTaskPlaceOnEventList>
2005cb3e:	8522                	c.mv	a0,s0
2005cb40:	c6fff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cb44:	446010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005cb48:	e119                	c.bnez	a0,2005cb4e <xQueueReceive+0xcc>
2005cb4a:	00000073          	ecall
2005cb4e:	4a05                	c.li	s4,1
2005cb50:	bf81                	c.j	2005caa0 <xQueueReceive+0x1e>
2005cb52:	8522                	c.mv	a0,s0
2005cb54:	c5bff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cb58:	432010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005cb5c:	bfcd                	c.j	2005cb4e <xQueueReceive+0xcc>
2005cb5e:	8522                	c.mv	a0,s0
2005cb60:	c4fff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cb64:	426010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005cb68:	8522                	c.mv	a0,s0
2005cb6a:	b71ff0ef          	jal	ra,2005c6da <prvIsQueueEmpty>
2005cb6e:	d165                	c.beqz	a0,2005cb4e <xQueueReceive+0xcc>
2005cb70:	bf9d                	c.j	2005cae6 <xQueueReceive+0x64>

2005cb72 <xQueueSemaphoreTake>:
2005cb72:	7139                	c.addi16sp	sp,-64
2005cb74:	dc22                	c.swsp	s0,56(sp)
2005cb76:	da26                	c.swsp	s1,52(sp)
2005cb78:	d84a                	c.swsp	s2,48(sp)
2005cb7a:	d64e                	c.swsp	s3,44(sp)
2005cb7c:	d452                	c.swsp	s4,40(sp)
2005cb7e:	de06                	c.swsp	ra,60(sp)
2005cb80:	842a                	c.mv	s0,a0
2005cb82:	c62e                	c.swsp	a1,12(sp)
2005cb84:	4481                	c.li	s1,0
2005cb86:	4981                	c.li	s3,0
2005cb88:	597d                	c.li	s2,-1
2005cb8a:	02450a13          	addi	s4,a0,36
2005cb8e:	074010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005cb92:	5c1c                	c.lw	a5,56(s0)
2005cb94:	cf8d                	c.beqz	a5,2005cbce <xQueueSemaphoreTake+0x5c>
2005cb96:	17fd                	c.addi	a5,-1
2005cb98:	dc1c                	c.sw	a5,56(s0)
2005cb9a:	401c                	c.lw	a5,0(s0)
2005cb9c:	e781                	c.bnez	a5,2005cba4 <xQueueSemaphoreTake+0x32>
2005cb9e:	0b9010ef          	jal	ra,2005e456 <pvTaskIncrementMutexHeldCount>
2005cba2:	c408                	c.sw	a0,8(s0)
2005cba4:	481c                	c.lw	a5,16(s0)
2005cba6:	cb81                	c.beqz	a5,2005cbb6 <xQueueSemaphoreTake+0x44>
2005cba8:	01040513          	addi	a0,s0,16
2005cbac:	5a7000ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005cbb0:	c119                	c.beqz	a0,2005cbb6 <xQueueSemaphoreTake+0x44>
2005cbb2:	00000073          	ecall
2005cbb6:	066010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005cbba:	4485                	c.li	s1,1
2005cbbc:	50f2                	c.lwsp	ra,60(sp)
2005cbbe:	5462                	c.lwsp	s0,56(sp)
2005cbc0:	5942                	c.lwsp	s2,48(sp)
2005cbc2:	59b2                	c.lwsp	s3,44(sp)
2005cbc4:	5a22                	c.lwsp	s4,40(sp)
2005cbc6:	8526                	c.mv	a0,s1
2005cbc8:	54d2                	c.lwsp	s1,52(sp)
2005cbca:	6121                	c.addi16sp	sp,64
2005cbcc:	8082                	c.jr	ra
2005cbce:	47b2                	c.lwsp	a5,12(sp)
2005cbd0:	e789                	c.bnez	a5,2005cbda <xQueueSemaphoreTake+0x68>
2005cbd2:	04a010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005cbd6:	4481                	c.li	s1,0
2005cbd8:	b7d5                	c.j	2005cbbc <xQueueSemaphoreTake+0x4a>
2005cbda:	00099563          	bne	s3,zero,2005cbe4 <xQueueSemaphoreTake+0x72>
2005cbde:	0828                	c.addi4spn	a0,sp,24
2005cbe0:	5f5000ef          	jal	ra,2005d9d4 <vTaskInternalSetTimeOutState>
2005cbe4:	038010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005cbe8:	2f5000ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005cbec:	016010ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005cbf0:	04444783          	lbu	a5,68(s0)
2005cbf4:	07e2                	c.slli	a5,0x18
2005cbf6:	87e1                	c.srai	a5,0x18
2005cbf8:	01279463          	bne	a5,s2,2005cc00 <xQueueSemaphoreTake+0x8e>
2005cbfc:	04040223          	sb	zero,68(s0)
2005cc00:	04544783          	lbu	a5,69(s0)
2005cc04:	07e2                	c.slli	a5,0x18
2005cc06:	87e1                	c.srai	a5,0x18
2005cc08:	01279463          	bne	a5,s2,2005cc10 <xQueueSemaphoreTake+0x9e>
2005cc0c:	040402a3          	sb	zero,69(s0)
2005cc10:	00c010ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005cc14:	006c                	c.addi4spn	a1,sp,12
2005cc16:	0828                	c.addi4spn	a0,sp,24
2005cc18:	71c010ef          	jal	ra,2005e334 <xTaskCheckForTimeOut>
2005cc1c:	e139                	c.bnez	a0,2005cc62 <xQueueSemaphoreTake+0xf0>
2005cc1e:	8522                	c.mv	a0,s0
2005cc20:	abbff0ef          	jal	ra,2005c6da <prvIsQueueEmpty>
2005cc24:	c90d                	c.beqz	a0,2005cc56 <xQueueSemaphoreTake+0xe4>
2005cc26:	401c                	c.lw	a5,0(s0)
2005cc28:	eb89                	c.bnez	a5,2005cc3a <xQueueSemaphoreTake+0xc8>
2005cc2a:	7d9000ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005cc2e:	4408                	c.lw	a0,8(s0)
2005cc30:	605000ef          	jal	ra,2005da34 <xTaskPriorityInherit>
2005cc34:	84aa                	c.mv	s1,a0
2005cc36:	7e7000ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005cc3a:	45b2                	c.lwsp	a1,12(sp)
2005cc3c:	8552                	c.mv	a0,s4
2005cc3e:	4cb000ef          	jal	ra,2005d908 <vTaskPlaceOnEventList>
2005cc42:	8522                	c.mv	a0,s0
2005cc44:	b6bff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cc48:	342010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005cc4c:	e119                	c.bnez	a0,2005cc52 <xQueueSemaphoreTake+0xe0>
2005cc4e:	00000073          	ecall
2005cc52:	4985                	c.li	s3,1
2005cc54:	bf2d                	c.j	2005cb8e <xQueueSemaphoreTake+0x1c>
2005cc56:	8522                	c.mv	a0,s0
2005cc58:	b57ff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cc5c:	32e010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005cc60:	bfcd                	c.j	2005cc52 <xQueueSemaphoreTake+0xe0>
2005cc62:	8522                	c.mv	a0,s0
2005cc64:	b4bff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cc68:	322010ef          	jal	ra,2005df8a <xTaskResumeAll>
2005cc6c:	8522                	c.mv	a0,s0
2005cc6e:	a6dff0ef          	jal	ra,2005c6da <prvIsQueueEmpty>
2005cc72:	d165                	c.beqz	a0,2005cc52 <xQueueSemaphoreTake+0xe0>
2005cc74:	f40484e3          	beq	s1,zero,2005cbbc <xQueueSemaphoreTake+0x4a>
2005cc78:	78b000ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005cc7c:	504c                	c.lw	a1,36(s0)
2005cc7e:	c589                	c.beqz	a1,2005cc88 <xQueueSemaphoreTake+0x116>
2005cc80:	581c                	c.lw	a5,48(s0)
2005cc82:	45ad                	c.li	a1,11
2005cc84:	439c                	c.lw	a5,0(a5)
2005cc86:	8d9d                	c.sub	a1,a5
2005cc88:	4408                	c.lw	a0,8(s0)
2005cc8a:	6db000ef          	jal	ra,2005db64 <vTaskPriorityDisinheritAfterTimeout>
2005cc8e:	b791                	c.j	2005cbd2 <xQueueSemaphoreTake+0x60>

2005cc90 <xQueueReceiveFromISR>:
2005cc90:	1101                	c.addi	sp,-32
2005cc92:	ce06                	c.swsp	ra,28(sp)
2005cc94:	cc22                	c.swsp	s0,24(sp)
2005cc96:	ca26                	c.swsp	s1,20(sp)
2005cc98:	c84a                	c.swsp	s2,16(sp)
2005cc9a:	c64e                	c.swsp	s3,12(sp)
2005cc9c:	03852983          	lw	s3,56(a0)
2005cca0:	04098b63          	beq	s3,zero,2005ccf6 <xQueueReceiveFromISR+0x66>
2005cca4:	04454483          	lbu	s1,68(a0)
2005cca8:	842a                	c.mv	s0,a0
2005ccaa:	8932                	c.mv	s2,a2
2005ccac:	04e2                	c.slli	s1,0x18
2005ccae:	addff0ef          	jal	ra,2005c78a <prvCopyDataFromQueue>
2005ccb2:	19fd                	c.addi	s3,-1
2005ccb4:	84e1                	c.srai	s1,0x18
2005ccb6:	03342c23          	sw	s3,56(s0)
2005ccba:	57fd                	c.li	a5,-1
2005ccbc:	02f49763          	bne	s1,a5,2005ccea <xQueueReceiveFromISR+0x5a>
2005ccc0:	481c                	c.lw	a5,16(s0)
2005ccc2:	eb89                	c.bnez	a5,2005ccd4 <xQueueReceiveFromISR+0x44>
2005ccc4:	4505                	c.li	a0,1
2005ccc6:	40f2                	c.lwsp	ra,28(sp)
2005ccc8:	4462                	c.lwsp	s0,24(sp)
2005ccca:	44d2                	c.lwsp	s1,20(sp)
2005cccc:	4942                	c.lwsp	s2,16(sp)
2005ccce:	49b2                	c.lwsp	s3,12(sp)
2005ccd0:	6105                	c.addi16sp	sp,32
2005ccd2:	8082                	c.jr	ra
2005ccd4:	01040513          	addi	a0,s0,16
2005ccd8:	47b000ef          	jal	ra,2005d952 <xTaskRemoveFromEventList>
2005ccdc:	d565                	c.beqz	a0,2005ccc4 <xQueueReceiveFromISR+0x34>
2005ccde:	fe0903e3          	beq	s2,zero,2005ccc4 <xQueueReceiveFromISR+0x34>
2005cce2:	4785                	c.li	a5,1
2005cce4:	00f92023          	sw	a5,0(s2)
2005cce8:	bff1                	c.j	2005ccc4 <xQueueReceiveFromISR+0x34>
2005ccea:	0485                	c.addi	s1,1
2005ccec:	04e2                	c.slli	s1,0x18
2005ccee:	84e1                	c.srai	s1,0x18
2005ccf0:	04940223          	sb	s1,68(s0)
2005ccf4:	bfc1                	c.j	2005ccc4 <xQueueReceiveFromISR+0x34>
2005ccf6:	4501                	c.li	a0,0
2005ccf8:	b7f9                	c.j	2005ccc6 <xQueueReceiveFromISR+0x36>

2005ccfa <vQueueWaitForMessageRestricted>:
2005ccfa:	1141                	c.addi	sp,-16
2005ccfc:	c422                	c.swsp	s0,8(sp)
2005ccfe:	c226                	c.swsp	s1,4(sp)
2005cd00:	c04a                	c.swsp	s2,0(sp)
2005cd02:	842a                	c.mv	s0,a0
2005cd04:	c606                	c.swsp	ra,12(sp)
2005cd06:	84ae                	c.mv	s1,a1
2005cd08:	8932                	c.mv	s2,a2
2005cd0a:	6f9000ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005cd0e:	04444783          	lbu	a5,68(s0)
2005cd12:	577d                	c.li	a4,-1
2005cd14:	07e2                	c.slli	a5,0x18
2005cd16:	87e1                	c.srai	a5,0x18
2005cd18:	00e79463          	bne	a5,a4,2005cd20 <vQueueWaitForMessageRestricted+0x26>
2005cd1c:	04040223          	sb	zero,68(s0)
2005cd20:	04544783          	lbu	a5,69(s0)
2005cd24:	577d                	c.li	a4,-1
2005cd26:	07e2                	c.slli	a5,0x18
2005cd28:	87e1                	c.srai	a5,0x18
2005cd2a:	00e79463          	bne	a5,a4,2005cd32 <vQueueWaitForMessageRestricted+0x38>
2005cd2e:	040402a3          	sb	zero,69(s0)
2005cd32:	6eb000ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005cd36:	5c1c                	c.lw	a5,56(s0)
2005cd38:	e799                	c.bnez	a5,2005cd46 <vQueueWaitForMessageRestricted+0x4c>
2005cd3a:	864a                	c.mv	a2,s2
2005cd3c:	85a6                	c.mv	a1,s1
2005cd3e:	02440513          	addi	a0,s0,36
2005cd42:	3e7000ef          	jal	ra,2005d928 <vTaskPlaceOnEventListRestricted>
2005cd46:	8522                	c.mv	a0,s0
2005cd48:	4422                	c.lwsp	s0,8(sp)
2005cd4a:	40b2                	c.lwsp	ra,12(sp)
2005cd4c:	4492                	c.lwsp	s1,4(sp)
2005cd4e:	4902                	c.lwsp	s2,0(sp)
2005cd50:	0141                	c.addi	sp,16
2005cd52:	a5dff06f          	jal	zero,2005c7ae <prvUnlockQueue>

2005cd56 <QueueCreateTrace>:
2005cd56:	f541a603          	lw	a2,-172(gp) # 200668dc <Inited.1>
2005cd5a:	f5418693          	addi	a3,gp,-172 # 200668dc <Inited.1>
2005cd5e:	200687b7          	lui	a5,0x20068
2005cd62:	22878713          	addi	a4,a5,552 # 20068228 <QueueTraceList>
2005cd66:	ee01                	c.bnez	a2,2005cd7e <QueueCreateTrace+0x28>
2005cd68:	22878793          	addi	a5,a5,552
2005cd6c:	04070613          	addi	a2,a4,64
2005cd70:	0007a023          	sw	zero,0(a5)
2005cd74:	0791                	c.addi	a5,4
2005cd76:	fec79de3          	bne	a5,a2,2005cd70 <QueueCreateTrace+0x1a>
2005cd7a:	4785                	c.li	a5,1
2005cd7c:	c29c                	c.sw	a5,0(a3)
2005cd7e:	86ba                	c.mv	a3,a4
2005cd80:	4781                	c.li	a5,0
2005cd82:	4641                	c.li	a2,16
2005cd84:	428c                	c.lw	a1,0(a3)
2005cd86:	e589                	c.bnez	a1,2005cd90 <QueueCreateTrace+0x3a>
2005cd88:	078a                	c.slli	a5,0x2
2005cd8a:	97ba                	c.add	a5,a4
2005cd8c:	c388                	c.sw	a0,0(a5)
2005cd8e:	8082                	c.jr	ra
2005cd90:	0785                	c.addi	a5,1
2005cd92:	0691                	c.addi	a3,4
2005cd94:	fec798e3          	bne	a5,a2,2005cd84 <QueueCreateTrace+0x2e>
2005cd98:	8082                	c.jr	ra

2005cd9a <xQueueGenericCreate>:
2005cd9a:	1101                	c.addi	sp,-32
2005cd9c:	c84a                	c.swsp	s2,16(sp)
2005cd9e:	892a                	c.mv	s2,a0
2005cda0:	02b50533          	mul	a0,a0,a1
2005cda4:	cc22                	c.swsp	s0,24(sp)
2005cda6:	ca26                	c.swsp	s1,20(sp)
2005cda8:	c64e                	c.swsp	s3,12(sp)
2005cdaa:	ce06                	c.swsp	ra,28(sp)
2005cdac:	84ae                	c.mv	s1,a1
2005cdae:	89b2                	c.mv	s3,a2
2005cdb0:	05050513          	addi	a0,a0,80
2005cdb4:	e57fd0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005cdb8:	842a                	c.mv	s0,a0
2005cdba:	c505                	c.beqz	a0,2005cde2 <xQueueGenericCreate+0x48>
2005cdbc:	04050323          	sb	zero,70(a0)
2005cdc0:	87aa                	c.mv	a5,a0
2005cdc2:	c099                	c.beqz	s1,2005cdc8 <xQueueGenericCreate+0x2e>
2005cdc4:	05050793          	addi	a5,a0,80
2005cdc8:	c01c                	c.sw	a5,0(s0)
2005cdca:	03242e23          	sw	s2,60(s0)
2005cdce:	c024                	c.sw	s1,64(s0)
2005cdd0:	4585                	c.li	a1,1
2005cdd2:	8522                	c.mv	a0,s0
2005cdd4:	a5dff0ef          	jal	ra,2005c830 <xQueueGenericReset>
2005cdd8:	8522                	c.mv	a0,s0
2005cdda:	05340623          	sb	s3,76(s0)
2005cdde:	f79ff0ef          	jal	ra,2005cd56 <QueueCreateTrace>
2005cde2:	40f2                	c.lwsp	ra,28(sp)
2005cde4:	8522                	c.mv	a0,s0
2005cde6:	4462                	c.lwsp	s0,24(sp)
2005cde8:	44d2                	c.lwsp	s1,20(sp)
2005cdea:	4942                	c.lwsp	s2,16(sp)
2005cdec:	49b2                	c.lwsp	s3,12(sp)
2005cdee:	6105                	c.addi16sp	sp,32
2005cdf0:	8082                	c.jr	ra

2005cdf2 <xQueueCreateMutex>:
2005cdf2:	1141                	c.addi	sp,-16
2005cdf4:	862a                	c.mv	a2,a0
2005cdf6:	4581                	c.li	a1,0
2005cdf8:	4505                	c.li	a0,1
2005cdfa:	c422                	c.swsp	s0,8(sp)
2005cdfc:	c606                	c.swsp	ra,12(sp)
2005cdfe:	f9dff0ef          	jal	ra,2005cd9a <xQueueGenericCreate>
2005ce02:	842a                	c.mv	s0,a0
2005ce04:	cd01                	c.beqz	a0,2005ce1c <xQueueCreateMutex+0x2a>
2005ce06:	00052423          	sw	zero,8(a0)
2005ce0a:	00052023          	sw	zero,0(a0)
2005ce0e:	00052623          	sw	zero,12(a0)
2005ce12:	4681                	c.li	a3,0
2005ce14:	4601                	c.li	a2,0
2005ce16:	4581                	c.li	a1,0
2005ce18:	aa9ff0ef          	jal	ra,2005c8c0 <xQueueGenericSend>
2005ce1c:	40b2                	c.lwsp	ra,12(sp)
2005ce1e:	8522                	c.mv	a0,s0
2005ce20:	4422                	c.lwsp	s0,8(sp)
2005ce22:	0141                	c.addi	sp,16
2005ce24:	8082                	c.jr	ra

2005ce26 <xQueueCreateCountingSemaphore>:
2005ce26:	1141                	c.addi	sp,-16
2005ce28:	c422                	c.swsp	s0,8(sp)
2005ce2a:	4609                	c.li	a2,2
2005ce2c:	842e                	c.mv	s0,a1
2005ce2e:	4581                	c.li	a1,0
2005ce30:	c606                	c.swsp	ra,12(sp)
2005ce32:	f69ff0ef          	jal	ra,2005cd9a <xQueueGenericCreate>
2005ce36:	c111                	c.beqz	a0,2005ce3a <xQueueCreateCountingSemaphore+0x14>
2005ce38:	dd00                	c.sw	s0,56(a0)
2005ce3a:	40b2                	c.lwsp	ra,12(sp)
2005ce3c:	4422                	c.lwsp	s0,8(sp)
2005ce3e:	0141                	c.addi	sp,16
2005ce40:	8082                	c.jr	ra

2005ce42 <xQueueGenericCreateStatic>:
2005ce42:	1141                	c.addi	sp,-16
2005ce44:	c422                	c.swsp	s0,8(sp)
2005ce46:	c606                	c.swsp	ra,12(sp)
2005ce48:	c226                	c.swsp	s1,4(sp)
2005ce4a:	8436                	c.mv	s0,a3
2005ce4c:	c29d                	c.beqz	a3,2005ce72 <xQueueGenericCreateStatic+0x30>
2005ce4e:	4785                	c.li	a5,1
2005ce50:	04f68323          	sb	a5,70(a3) # 8000046 <__ap_sram_heap_size+0x7fc0046>
2005ce54:	84ba                	c.mv	s1,a4
2005ce56:	e191                	c.bnez	a1,2005ce5a <xQueueGenericCreateStatic+0x18>
2005ce58:	8636                	c.mv	a2,a3
2005ce5a:	dc48                	c.sw	a0,60(s0)
2005ce5c:	c02c                	c.sw	a1,64(s0)
2005ce5e:	c010                	c.sw	a2,0(s0)
2005ce60:	4585                	c.li	a1,1
2005ce62:	8522                	c.mv	a0,s0
2005ce64:	9cdff0ef          	jal	ra,2005c830 <xQueueGenericReset>
2005ce68:	8522                	c.mv	a0,s0
2005ce6a:	04940623          	sb	s1,76(s0)
2005ce6e:	ee9ff0ef          	jal	ra,2005cd56 <QueueCreateTrace>
2005ce72:	40b2                	c.lwsp	ra,12(sp)
2005ce74:	8522                	c.mv	a0,s0
2005ce76:	4422                	c.lwsp	s0,8(sp)
2005ce78:	4492                	c.lwsp	s1,4(sp)
2005ce7a:	0141                	c.addi	sp,16
2005ce7c:	8082                	c.jr	ra

2005ce7e <xQueueCreateMutexStatic>:
2005ce7e:	1141                	c.addi	sp,-16
2005ce80:	872a                	c.mv	a4,a0
2005ce82:	86ae                	c.mv	a3,a1
2005ce84:	4601                	c.li	a2,0
2005ce86:	4581                	c.li	a1,0
2005ce88:	4505                	c.li	a0,1
2005ce8a:	c422                	c.swsp	s0,8(sp)
2005ce8c:	c606                	c.swsp	ra,12(sp)
2005ce8e:	fb5ff0ef          	jal	ra,2005ce42 <xQueueGenericCreateStatic>
2005ce92:	842a                	c.mv	s0,a0
2005ce94:	cd01                	c.beqz	a0,2005ceac <xQueueCreateMutexStatic+0x2e>
2005ce96:	00052423          	sw	zero,8(a0)
2005ce9a:	00052023          	sw	zero,0(a0)
2005ce9e:	00052623          	sw	zero,12(a0)
2005cea2:	4681                	c.li	a3,0
2005cea4:	4601                	c.li	a2,0
2005cea6:	4581                	c.li	a1,0
2005cea8:	a19ff0ef          	jal	ra,2005c8c0 <xQueueGenericSend>
2005ceac:	40b2                	c.lwsp	ra,12(sp)
2005ceae:	8522                	c.mv	a0,s0
2005ceb0:	4422                	c.lwsp	s0,8(sp)
2005ceb2:	0141                	c.addi	sp,16
2005ceb4:	8082                	c.jr	ra

2005ceb6 <xQueueCreateCountingSemaphoreStatic>:
2005ceb6:	1141                	c.addi	sp,-16
2005ceb8:	c422                	c.swsp	s0,8(sp)
2005ceba:	86b2                	c.mv	a3,a2
2005cebc:	842e                	c.mv	s0,a1
2005cebe:	4709                	c.li	a4,2
2005cec0:	4601                	c.li	a2,0
2005cec2:	4581                	c.li	a1,0
2005cec4:	c606                	c.swsp	ra,12(sp)
2005cec6:	f7dff0ef          	jal	ra,2005ce42 <xQueueGenericCreateStatic>
2005ceca:	c111                	c.beqz	a0,2005cece <xQueueCreateCountingSemaphoreStatic+0x18>
2005cecc:	dd00                	c.sw	s0,56(a0)
2005cece:	40b2                	c.lwsp	ra,12(sp)
2005ced0:	4422                	c.lwsp	s0,8(sp)
2005ced2:	0141                	c.addi	sp,16
2005ced4:	8082                	c.jr	ra

2005ced6 <QueueDeleteTrace>:
2005ced6:	20068737          	lui	a4,0x20068
2005ceda:	22870693          	addi	a3,a4,552 # 20068228 <QueueTraceList>
2005cede:	4781                	c.li	a5,0
2005cee0:	22870713          	addi	a4,a4,552
2005cee4:	4641                	c.li	a2,16
2005cee6:	428c                	c.lw	a1,0(a3)
2005cee8:	00a59763          	bne	a1,a0,2005cef6 <QueueDeleteTrace+0x20>
2005ceec:	078a                	c.slli	a5,0x2
2005ceee:	97ba                	c.add	a5,a4
2005cef0:	0007a023          	sw	zero,0(a5)
2005cef4:	8082                	c.jr	ra
2005cef6:	0785                	c.addi	a5,1
2005cef8:	0691                	c.addi	a3,4
2005cefa:	fec796e3          	bne	a5,a2,2005cee6 <QueueDeleteTrace+0x10>
2005cefe:	8082                	c.jr	ra

2005cf00 <vQueueDelete>:
2005cf00:	1141                	c.addi	sp,-16
2005cf02:	c422                	c.swsp	s0,8(sp)
2005cf04:	c606                	c.swsp	ra,12(sp)
2005cf06:	842a                	c.mv	s0,a0
2005cf08:	fcfff0ef          	jal	ra,2005ced6 <QueueDeleteTrace>
2005cf0c:	04644783          	lbu	a5,70(s0)
2005cf10:	e799                	c.bnez	a5,2005cf1e <vQueueDelete+0x1e>
2005cf12:	8522                	c.mv	a0,s0
2005cf14:	4422                	c.lwsp	s0,8(sp)
2005cf16:	40b2                	c.lwsp	ra,12(sp)
2005cf18:	0141                	c.addi	sp,16
2005cf1a:	dcffd06f          	jal	zero,2005ace8 <vPortFree>
2005cf1e:	40b2                	c.lwsp	ra,12(sp)
2005cf20:	4422                	c.lwsp	s0,8(sp)
2005cf22:	0141                	c.addi	sp,16
2005cf24:	8082                	c.jr	ra

2005cf26 <shell_loguratRx_ipc_int>:
2005cf26:	1141                	c.addi	sp,-16
2005cf28:	458d                	c.li	a1,3
2005cf2a:	4541                	c.li	a0,16
2005cf2c:	c422                	c.swsp	s0,8(sp)
2005cf2e:	c606                	c.swsp	ra,12(sp)
2005cf30:	20001437          	lui	s0,0x20001
2005cf34:	9bcfc0ef          	jal	ra,200590f0 <ipc_get_message>
2005cf38:	87aa                	c.mv	a5,a0
2005cf3a:	b4440713          	addi	a4,s0,-1212 # 20000b44 <shell_ctl>
2005cf3e:	4708                	c.lw	a0,8(a4)
2005cf40:	43cc                	c.lw	a1,4(a5)
2005cf42:	7c1027f3          	csrrs	a5,0x7c1,zero
2005cf46:	1007f793          	andi	a5,a5,256
2005cf4a:	b4440413          	addi	s0,s0,-1212
2005cf4e:	c3b9                	c.beqz	a5,2005cf94 <shell_loguratRx_ipc_int+0x6e>
2005cf50:	01f5f713          	andi	a4,a1,31
2005cf54:	c325                	c.beqz	a4,2005cfb4 <shell_loguratRx_ipc_int+0x8e>
2005cf56:	08070693          	addi	a3,a4,128
2005cf5a:	fe05f793          	andi	a5,a1,-32
2005cf5e:	01f7f713          	andi	a4,a5,31
2005cf62:	9736                	c.add	a4,a3
2005cf64:	0ff0000f          	fence	iorw,iorw
2005cf68:	973e                	c.add	a4,a5
2005cf6a:	00f7f00b          	cache	dinv,(a5)
2005cf6e:	02078793          	addi	a5,a5,32
2005cf72:	40f706b3          	sub	a3,a4,a5
2005cf76:	fed04ae3          	blt	zero,a3,2005cf6a <shell_loguratRx_ipc_int+0x44>
2005cf7a:	0ff0000f          	fence	iorw,iorw
2005cf7e:	0001                	c.addi	zero,0
2005cf80:	0001                	c.addi	zero,0
2005cf82:	0001                	c.addi	zero,0
2005cf84:	0001                	c.addi	zero,0
2005cf86:	0001                	c.addi	zero,0
2005cf88:	0001                	c.addi	zero,0
2005cf8a:	0001                	c.addi	zero,0
2005cf8c:	0001                	c.addi	zero,0
2005cf8e:	0001                	c.addi	zero,0
2005cf90:	0001                	c.addi	zero,0
2005cf92:	0001                	c.addi	zero,0
2005cf94:	08000613          	addi	a2,zero,128
2005cf98:	dffac097          	auipc	ra,0xdffac
2005cf9c:	67a080e7          	jalr	ra,1658(ra) # 9612 <_memcpy>
2005cfa0:	4785                	c.li	a5,1
2005cfa2:	00f40223          	sb	a5,4(s0)
2005cfa6:	501c                	c.lw	a5,32(s0)
2005cfa8:	cb91                	c.beqz	a5,2005cfbc <shell_loguratRx_ipc_int+0x96>
2005cfaa:	4c5c                	c.lw	a5,28(s0)
2005cfac:	4422                	c.lwsp	s0,8(sp)
2005cfae:	40b2                	c.lwsp	ra,12(sp)
2005cfb0:	0141                	c.addi	sp,16
2005cfb2:	8782                	c.jr	a5
2005cfb4:	87ae                	c.mv	a5,a1
2005cfb6:	08000693          	addi	a3,zero,128
2005cfba:	b755                	c.j	2005cf5e <shell_loguratRx_ipc_int+0x38>
2005cfbc:	40b2                	c.lwsp	ra,12(sp)
2005cfbe:	4422                	c.lwsp	s0,8(sp)
2005cfc0:	0141                	c.addi	sp,16
2005cfc2:	8082                	c.jr	ra

2005cfc4 <shell_give_sema>:
2005cfc4:	200017b7          	lui	a5,0x20001
2005cfc8:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005cfcc:	539c                	c.lw	a5,32(a5)
2005cfce:	c789                	c.beqz	a5,2005cfd8 <shell_give_sema+0x14>
2005cfd0:	f581a503          	lw	a0,-168(gp) # 200668e0 <shell_sema>
2005cfd4:	983fe06f          	jal	zero,2005b956 <rtos_sema_give>
2005cfd8:	8082                	c.jr	ra

2005cfda <shell_loguartRx_dispatch>:
2005cfda:	1141                	c.addi	sp,-16
2005cfdc:	c422                	c.swsp	s0,8(sp)
2005cfde:	20001437          	lui	s0,0x20001
2005cfe2:	b4440413          	addi	s0,s0,-1212 # 20000b44 <shell_ctl>
2005cfe6:	c04a                	c.swsp	s2,0(sp)
2005cfe8:	00842903          	lw	s2,8(s0)
2005cfec:	200635b7          	lui	a1,0x20063
2005cff0:	53c58593          	addi	a1,a1,1340 # 2006353c <__FUNCTION__.5+0x20>
2005cff4:	00190513          	addi	a0,s2,1
2005cff8:	c606                	c.swsp	ra,12(sp)
2005cffa:	c226                	c.swsp	s1,4(sp)
2005cffc:	dffad097          	auipc	ra,0xdffad
2005d000:	89c080e7          	jalr	ra,-1892(ra) # 9898 <_stricmp>
2005d004:	ed15                	c.bnez	a0,2005d040 <shell_loguartRx_dispatch+0x66>
2005d006:	40000513          	addi	a0,zero,1024
2005d00a:	ed2fe0ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005d00e:	3ff00593          	addi	a1,zero,1023
2005d012:	84aa                	c.mv	s1,a0
2005d014:	31b010ef          	jal	ra,2005eb2e <ChipInfo_GetLibVersion_ToBuf>
2005d018:	20063537          	lui	a0,0x20063
2005d01c:	85a6                	c.mv	a1,s1
2005d01e:	97c50513          	addi	a0,a0,-1668 # 2006297c <pmap_func+0x570>
2005d022:	36c020ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005d026:	8526                	c.mv	a0,s1
2005d028:	edefe0ef          	jal	ra,2005b706 <rtos_mem_free>
2005d02c:	4601                	c.li	a2,0
2005d02e:	08000593          	addi	a1,zero,128
2005d032:	854a                	c.mv	a0,s2
2005d034:	dffac097          	auipc	ra,0xdffac
2005d038:	e4e080e7          	jalr	ra,-434(ra) # 8e82 <shell_array_init>
2005d03c:	00040223          	sb	zero,4(s0)
2005d040:	40b2                	c.lwsp	ra,12(sp)
2005d042:	4422                	c.lwsp	s0,8(sp)
2005d044:	4492                	c.lwsp	s1,4(sp)
2005d046:	4902                	c.lwsp	s2,0(sp)
2005d048:	0141                	c.addi	sp,16
2005d04a:	8082                	c.jr	ra

2005d04c <shell_task_ram>:
2005d04c:	715d                	c.addi16sp	sp,-80
2005d04e:	c4a2                	c.swsp	s0,72(sp)
2005d050:	20001437          	lui	s0,0x20001
2005d054:	c0ca                	c.swsp	s2,64(sp)
2005d056:	da56                	c.swsp	s5,52(sp)
2005d058:	d65e                	c.swsp	s7,44(sp)
2005d05a:	d462                	c.swsp	s8,40(sp)
2005d05c:	d266                	c.swsp	s9,36(sp)
2005d05e:	b4440793          	addi	a5,s0,-1212 # 20000b44 <shell_ctl>
2005d062:	c686                	c.swsp	ra,76(sp)
2005d064:	c2a6                	c.swsp	s1,68(sp)
2005d066:	de4e                	c.swsp	s3,60(sp)
2005d068:	dc52                	c.swsp	s4,56(sp)
2005d06a:	d85a                	c.swsp	s6,48(sp)
2005d06c:	d06a                	c.swsp	s10,32(sp)
2005d06e:	ce6e                	c.swsp	s11,28(sp)
2005d070:	0087ab03          	lw	s6,8(a5)
2005d074:	4705                	c.li	a4,1
2005d076:	d398                	c.sw	a4,32(a5)
2005d078:	40000513          	addi	a0,zero,1024
2005d07c:	00e78323          	sb	a4,6(a5)
2005d080:	b4440413          	addi	s0,s0,-1212
2005d084:	b5bfe0ef          	jal	ra,2005bbde <rtos_create_secure_context>
2005d088:	20068ab7          	lui	s5,0x20068
2005d08c:	001b0913          	addi	s2,s6,1
2005d090:	20063c37          	lui	s8,0x20063
2005d094:	20063cb7          	lui	s9,0x20063
2005d098:	f581a503          	lw	a0,-168(gp) # 200668e0 <shell_sema>
2005d09c:	55fd                	c.li	a1,-1
2005d09e:	86dfe0ef          	jal	ra,2005b90a <rtos_sema_take>
2005d0a2:	f39ff0ef          	jal	ra,2005cfda <shell_loguartRx_dispatch>
2005d0a6:	00444783          	lbu	a5,4(s0)
2005d0aa:	0ff7f793          	andi	a5,a5,255
2005d0ae:	d7ed                	c.beqz	a5,2005d098 <shell_task_ram+0x4c>
2005d0b0:	4601                	c.li	a2,0
2005d0b2:	07f00593          	addi	a1,zero,127
2005d0b6:	268a8513          	addi	a0,s5,616 # 20068268 <atcmd_buf>
2005d0ba:	dffac097          	auipc	ra,0xdffac
2005d0be:	dc8080e7          	jalr	ra,-568(ra) # 8e82 <shell_array_init>
2005d0c2:	85ca                	c.mv	a1,s2
2005d0c4:	268a8513          	addi	a0,s5,616
2005d0c8:	433040ef          	jal	ra,20061cfa <strcpy>
2005d0cc:	268a8513          	addi	a0,s5,616
2005d0d0:	e1dfc0ef          	jal	ra,20059eec <atcmd_service>
2005d0d4:	ed39                	c.bnez	a0,2005d132 <shell_task_ram+0xe6>
2005d0d6:	854a                	c.mv	a0,s2
2005d0d8:	dd7fc0ef          	jal	ra,20059eae <mp_command_handler>
2005d0dc:	e939                	c.bnez	a0,2005d132 <shell_task_ram+0xe6>
2005d0de:	854a                	c.mv	a0,s2
2005d0e0:	dffac097          	auipc	ra,0xdffac
2005d0e4:	dba080e7          	jalr	ra,-582(ra) # 8e9a <shell_get_argc>
2005d0e8:	89aa                	c.mv	s3,a0
2005d0ea:	854a                	c.mv	a0,s2
2005d0ec:	dffac097          	auipc	ra,0xdffac
2005d0f0:	de8080e7          	jalr	ra,-536(ra) # 8ed4 <shell_get_argv>
2005d0f4:	411c                	c.lw	a5,0(a0)
2005d0f6:	4804                	c.lw	s1,16(s0)
2005d0f8:	01442d83          	lw	s11,20(s0)
2005d0fc:	8a2a                	c.mv	s4,a0
2005d0fe:	4d01                	c.li	s10,0
2005d100:	03ad8063          	beq	s11,s10,2005d120 <shell_task_ram+0xd4>
2005d104:	408c                	c.lw	a1,0(s1)
2005d106:	853e                	c.mv	a0,a5
2005d108:	c626                	c.swsp	s1,12(sp)
2005d10a:	c43e                	c.swsp	a5,8(sp)
2005d10c:	dffac097          	auipc	ra,0xdffac
2005d110:	78c080e7          	jalr	ra,1932(ra) # 9898 <_stricmp>
2005d114:	47a2                	c.lwsp	a5,8(sp)
2005d116:	4732                	c.lwsp	a4,12(sp)
2005d118:	04c1                	c.addi	s1,16
2005d11a:	e51d                	c.bnez	a0,2005d148 <shell_task_ram+0xfc>
2005d11c:	471c                	c.lw	a5,8(a4)
2005d11e:	e79d                	c.bnez	a5,2005d14c <shell_task_ram+0x100>
2005d120:	85ca                	c.mv	a1,s2
2005d122:	544c0513          	addi	a0,s8,1348 # 20063544 <__FUNCTION__.5+0x28>
2005d126:	268020ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005d12a:	874c8513          	addi	a0,s9,-1932 # 20062874 <pmap_func+0x468>
2005d12e:	260020ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005d132:	4601                	c.li	a2,0
2005d134:	08000593          	addi	a1,zero,128
2005d138:	855a                	c.mv	a0,s6
2005d13a:	dffac097          	auipc	ra,0xdffac
2005d13e:	d48080e7          	jalr	ra,-696(ra) # 8e82 <shell_array_init>
2005d142:	00040223          	sb	zero,4(s0)
2005d146:	bf89                	c.j	2005d098 <shell_task_ram+0x4c>
2005d148:	0d05                	c.addi	s10,1
2005d14a:	bf5d                	c.j	2005d100 <shell_task_ram+0xb4>
2005d14c:	fff98513          	addi	a0,s3,-1
2005d150:	0542                	c.slli	a0,0x10
2005d152:	004a0593          	addi	a1,s4,4
2005d156:	8141                	c.srli	a0,0x10
2005d158:	9782                	c.jalr	a5
2005d15a:	bfe1                	c.j	2005d132 <shell_task_ram+0xe6>

2005d15c <shell_init_ram>:
2005d15c:	1141                	c.addi	sp,-16
2005d15e:	c606                	c.swsp	ra,12(sp)
2005d160:	bebfc0ef          	jal	ra,20059d4a <atcmd_service_init>
2005d164:	20065737          	lui	a4,0x20065
2005d168:	c3470693          	addi	a3,a4,-972 # 20064c34 <shell_cmd_table>
2005d16c:	20065737          	lui	a4,0x20065
2005d170:	200017b7          	lui	a5,0x20001
2005d174:	c7470713          	addi	a4,a4,-908 # 20064c74 <ipc_tickless_table>
2005d178:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005d17c:	8f15                	c.sub	a4,a3
2005d17e:	cb94                	c.sw	a3,16(a5)
2005d180:	8311                	c.srli	a4,0x4
2005d182:	cbd8                	c.sw	a4,20(a5)
2005d184:	00078223          	sb	zero,4(a5)
2005d188:	4705                	c.li	a4,1
2005d18a:	00e782a3          	sb	a4,5(a5)
2005d18e:	2005d737          	lui	a4,0x2005d
2005d192:	fc470713          	addi	a4,a4,-60 # 2005cfc4 <shell_give_sema>
2005d196:	f5818513          	addi	a0,gp,-168 # 200668e0 <shell_sema>
2005d19a:	cfd8                	c.sw	a4,28(a5)
2005d19c:	f2efe0ef          	jal	ra,2005b8ca <rtos_sema_create_binary>
2005d1a0:	2005d637          	lui	a2,0x2005d
2005d1a4:	200635b7          	lui	a1,0x20063
2005d1a8:	4795                	c.li	a5,5
2005d1aa:	6705                	c.lui	a4,0x1
2005d1ac:	4681                	c.li	a3,0
2005d1ae:	04c60613          	addi	a2,a2,76 # 2005d04c <shell_task_ram>
2005d1b2:	55c58593          	addi	a1,a1,1372 # 2006355c <__FUNCTION__.5+0x40>
2005d1b6:	4501                	c.li	a0,0
2005d1b8:	9f3fe0ef          	jal	ra,2005bbaa <rtos_task_create>
2005d1bc:	c105                	c.beqz	a0,2005d1dc <shell_init_ram+0x80>
2005d1be:	40b2                	c.lwsp	ra,12(sp)
2005d1c0:	200636b7          	lui	a3,0x20063
2005d1c4:	200635b7          	lui	a1,0x20063
2005d1c8:	56c68693          	addi	a3,a3,1388 # 2006356c <__FUNCTION__.5+0x50>
2005d1cc:	04500613          	addi	a2,zero,69
2005d1d0:	58858593          	addi	a1,a1,1416 # 20063588 <__FUNCTION__.5+0x6c>
2005d1d4:	4509                	c.li	a0,2
2005d1d6:	0141                	c.addi	sp,16
2005d1d8:	f9ffd06f          	jal	zero,2005b176 <rtk_log_write>
2005d1dc:	40b2                	c.lwsp	ra,12(sp)
2005d1de:	0141                	c.addi	sp,16
2005d1e0:	8082                	c.jr	ra

2005d1e2 <sys_clear_ota_signature>:
2005d1e2:	7155                	c.addi16sp	sp,-208
2005d1e4:	4601                	c.li	a2,0
2005d1e6:	102c                	c.addi4spn	a1,sp,40
2005d1e8:	4509                	c.li	a0,2
2005d1ea:	c786                	c.swsp	ra,204(sp)
2005d1ec:	c3a6                	c.swsp	s1,196(sp)
2005d1ee:	c5a2                	c.swsp	s0,200(sp)
2005d1f0:	c1ca                	c.swsp	s2,192(sp)
2005d1f2:	df4e                	c.swsp	s3,188(sp)
2005d1f4:	dd52                	c.swsp	s4,184(sp)
2005d1f6:	c082                	c.swsp	zero,64(sp)
2005d1f8:	c282                	c.swsp	zero,68(sp)
2005d1fa:	90bfb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d1fe:	4601                	c.li	a2,0
2005d200:	106c                	c.addi4spn	a1,sp,44
2005d202:	450d                	c.li	a0,3
2005d204:	901fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d208:	4505                	c.li	a0,1
2005d20a:	9b8fc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d20e:	0ff57693          	andi	a3,a0,255
2005d212:	0016c493          	xori	s1,a3,1
2005d216:	f80007b7          	lui	a5,0xf8000
2005d21a:	e2e9                	c.bnez	a3,2005d2dc <sys_clear_ota_signature+0xfa>
2005d21c:	5722                	c.lwsp	a4,40(sp)
2005d21e:	0ff57813          	andi	a6,a0,255
2005d222:	97ba                	c.add	a5,a4
2005d224:	00281713          	slli	a4,a6,0x2
2005d228:	0a070613          	addi	a2,a4,160 # 10a0 <CPU_ClkGet+0x58a>
2005d22c:	080c                	c.addi4spn	a1,sp,16
2005d22e:	962e                	c.add	a2,a1
2005d230:	f8f62423          	sw	a5,-120(a2)
2005d234:	4785                	c.li	a5,1
2005d236:	f8000437          	lui	s0,0xf8000
2005d23a:	0af69363          	bne	a3,a5,2005d2e0 <sys_clear_ota_signature+0xfe>
2005d23e:	57a2                	c.lwsp	a5,40(sp)
2005d240:	943e                	c.add	s0,a5
2005d242:	00249793          	slli	a5,s1,0x2
2005d246:	0814                	c.addi4spn	a3,sp,16
2005d248:	0a078793          	addi	a5,a5,160 # f80000a0 <__ctrace_end__+0x2e0000a0>
2005d24c:	97b6                	c.add	a5,a3
2005d24e:	f887a423          	sw	s0,-120(a5)
2005d252:	0a070793          	addi	a5,a4,160
2005d256:	00d78733          	add	a4,a5,a3
2005d25a:	f8872a03          	lw	s4,-120(a4)
2005d25e:	0485                	c.addi	s1,1
2005d260:	200639b7          	lui	s3,0x20063
2005d264:	200636b7          	lui	a3,0x20063
2005d268:	20063937          	lui	s2,0x20063
2005d26c:	0805                	c.addi	a6,1
2005d26e:	4785                	c.li	a5,1
2005d270:	66c98713          	addi	a4,s3,1644 # 2006366c <__func__.1>
2005d274:	88d2                	c.mv	a7,s4
2005d276:	59068693          	addi	a3,a3,1424 # 20063590 <__FUNCTION__.5+0x74>
2005d27a:	04100613          	addi	a2,zero,65
2005d27e:	5e090593          	addi	a1,s2,1504 # 200635e0 <__FUNCTION__.5+0xc4>
2005d282:	4505                	c.li	a0,1
2005d284:	c222                	c.swsp	s0,4(sp)
2005d286:	c026                	c.swsp	s1,0(sp)
2005d288:	ce42                	c.swsp	a6,28(sp)
2005d28a:	eedfd0ef          	jal	ra,2005b176 <rtk_log_write>
2005d28e:	1810                	c.addi4spn	a2,sp,48
2005d290:	85a2                	c.mv	a1,s0
2005d292:	00a8                	c.addi4spn	a0,sp,72
2005d294:	ffcfd0ef          	jal	ra,2005aa90 <flash_read_word>
2005d298:	1850                	c.addi4spn	a2,sp,52
2005d29a:	00440593          	addi	a1,s0,4 # f8000004 <__ctrace_end__+0x2e000004>
2005d29e:	00a8                	c.addi4spn	a0,sp,72
2005d2a0:	ff0fd0ef          	jal	ra,2005aa90 <flash_read_word>
2005d2a4:	5742                	c.lwsp	a4,48(sp)
2005d2a6:	353937b7          	lui	a5,0x35393
2005d2aa:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d2ae:	4872                	c.lwsp	a6,28(sp)
2005d2b0:	02f71a63          	bne	a4,a5,2005d2e4 <sys_clear_ota_signature+0x102>
2005d2b4:	5752                	c.lwsp	a4,52(sp)
2005d2b6:	313137b7          	lui	a5,0x31313
2005d2ba:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d2be:	02f71363          	bne	a4,a5,2005d2e4 <sys_clear_ota_signature+0x102>
2005d2c2:	0090                	c.addi4spn	a2,sp,64
2005d2c4:	45a1                	c.li	a1,8
2005d2c6:	8552                	c.mv	a0,s4
2005d2c8:	8f1fb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d2cc:	40be                	c.lwsp	ra,204(sp)
2005d2ce:	442e                	c.lwsp	s0,200(sp)
2005d2d0:	449e                	c.lwsp	s1,196(sp)
2005d2d2:	490e                	c.lwsp	s2,192(sp)
2005d2d4:	59fa                	c.lwsp	s3,188(sp)
2005d2d6:	5a6a                	c.lwsp	s4,184(sp)
2005d2d8:	6169                	c.addi16sp	sp,208
2005d2da:	8082                	c.jr	ra
2005d2dc:	5732                	c.lwsp	a4,44(sp)
2005d2de:	b781                	c.j	2005d21e <sys_clear_ota_signature+0x3c>
2005d2e0:	57b2                	c.lwsp	a5,44(sp)
2005d2e2:	bfb9                	c.j	2005d240 <sys_clear_ota_signature+0x5e>
2005d2e4:	200636b7          	lui	a3,0x20063
2005d2e8:	88a6                	c.mv	a7,s1
2005d2ea:	4785                	c.li	a5,1
2005d2ec:	66c98713          	addi	a4,s3,1644
2005d2f0:	5e468693          	addi	a3,a3,1508 # 200635e4 <__FUNCTION__.5+0xc8>
2005d2f4:	04500613          	addi	a2,zero,69
2005d2f8:	5e090593          	addi	a1,s2,1504
2005d2fc:	4509                	c.li	a0,2
2005d2fe:	e79fd0ef          	jal	ra,2005b176 <rtk_log_write>
2005d302:	b7e9                	c.j	2005d2cc <sys_clear_ota_signature+0xea>

2005d304 <sys_recover_ota_signature>:
2005d304:	353937b7          	lui	a5,0x35393
2005d308:	715d                	c.addi16sp	sp,-80
2005d30a:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d30e:	d43e                	c.swsp	a5,40(sp)
2005d310:	313137b7          	lui	a5,0x31313
2005d314:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d318:	6505                	c.lui	a0,0x1
2005d31a:	c2a6                	c.swsp	s1,68(sp)
2005d31c:	de4e                	c.swsp	s3,60(sp)
2005d31e:	c686                	c.swsp	ra,76(sp)
2005d320:	c4a2                	c.swsp	s0,72(sp)
2005d322:	c0ca                	c.swsp	s2,64(sp)
2005d324:	d63e                	c.swsp	a5,44(sp)
2005d326:	893fd0ef          	jal	ra,2005abb8 <__wrap_malloc>
2005d32a:	200639b7          	lui	s3,0x20063
2005d32e:	200634b7          	lui	s1,0x20063
2005d332:	e50d                	c.bnez	a0,2005d35c <sys_recover_ota_signature+0x58>
2005d334:	200636b7          	lui	a3,0x20063
2005d338:	65098713          	addi	a4,s3,1616 # 20063650 <__func__.0>
2005d33c:	5e048593          	addi	a1,s1,1504 # 200635e0 <__FUNCTION__.5+0xc4>
2005d340:	63468693          	addi	a3,a3,1588 # 20063634 <__FUNCTION__.5+0x118>
2005d344:	04500613          	addi	a2,zero,69
2005d348:	4509                	c.li	a0,2
2005d34a:	e2dfd0ef          	jal	ra,2005b176 <rtk_log_write>
2005d34e:	40b6                	c.lwsp	ra,76(sp)
2005d350:	4426                	c.lwsp	s0,72(sp)
2005d352:	4496                	c.lwsp	s1,68(sp)
2005d354:	4906                	c.lwsp	s2,64(sp)
2005d356:	59f2                	c.lwsp	s3,60(sp)
2005d358:	6161                	c.addi16sp	sp,80
2005d35a:	8082                	c.jr	ra
2005d35c:	4601                	c.li	a2,0
2005d35e:	082c                	c.addi4spn	a1,sp,24
2005d360:	842a                	c.mv	s0,a0
2005d362:	4509                	c.li	a0,2
2005d364:	fa0fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d368:	086c                	c.addi4spn	a1,sp,28
2005d36a:	4601                	c.li	a2,0
2005d36c:	450d                	c.li	a0,3
2005d36e:	f96fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d372:	4505                	c.li	a0,1
2005d374:	84efc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d378:	0ff57593          	andi	a1,a0,255
2005d37c:	4785                	c.li	a5,1
2005d37e:	0015c693          	xori	a3,a1,1
2005d382:	0af59963          	bne	a1,a5,2005d434 <sys_recover_ota_signature+0x130>
2005d386:	4762                	c.lwsp	a4,24(sp)
2005d388:	f80007b7          	lui	a5,0xf8000
2005d38c:	00269613          	slli	a2,a3,0x2
2005d390:	97ba                	c.add	a5,a4
2005d392:	03060713          	addi	a4,a2,48
2005d396:	970a                	c.add	a4,sp
2005d398:	fef72823          	sw	a5,-16(a4)
2005d39c:	f80008b7          	lui	a7,0xf8000
2005d3a0:	edd1                	c.bnez	a1,2005d43c <sys_recover_ota_signature+0x138>
2005d3a2:	47e2                	c.lwsp	a5,24(sp)
2005d3a4:	0ff57813          	andi	a6,a0,255
2005d3a8:	98be                	c.add	a7,a5
2005d3aa:	00281793          	slli	a5,a6,0x2
2005d3ae:	03078793          	addi	a5,a5,48 # f8000030 <__ctrace_end__+0x2e000030>
2005d3b2:	978a                	c.add	a5,sp
2005d3b4:	ff17a823          	sw	a7,-16(a5)
2005d3b8:	03060793          	addi	a5,a2,48
2005d3bc:	00278633          	add	a2,a5,sp
2005d3c0:	ff062903          	lw	s2,-16(a2)
2005d3c4:	0685                	c.addi	a3,1
2005d3c6:	c036                	c.swsp	a3,0(sp)
2005d3c8:	200636b7          	lui	a3,0x20063
2005d3cc:	65098713          	addi	a4,s3,1616
2005d3d0:	0805                	c.addi	a6,1
2005d3d2:	4785                	c.li	a5,1
2005d3d4:	59068693          	addi	a3,a3,1424 # 20063590 <__FUNCTION__.5+0x74>
2005d3d8:	5e048593          	addi	a1,s1,1504
2005d3dc:	04100613          	addi	a2,zero,65
2005d3e0:	4505                	c.li	a0,1
2005d3e2:	c24a                	c.swsp	s2,4(sp)
2005d3e4:	d93fd0ef          	jal	ra,2005b176 <rtk_log_write>
2005d3e8:	8622                	c.mv	a2,s0
2005d3ea:	6585                	c.lui	a1,0x1
2005d3ec:	854a                	c.mv	a0,s2
2005d3ee:	f84fb0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005d3f2:	85ca                	c.mv	a1,s2
2005d3f4:	4509                	c.li	a0,2
2005d3f6:	8a9fb0ef          	jal	ra,20058c9e <FLASH_EraseXIP>
2005d3fa:	4621                	c.li	a2,8
2005d3fc:	102c                	c.addi4spn	a1,sp,40
2005d3fe:	8522                	c.mv	a0,s0
2005d400:	dffac097          	auipc	ra,0xdffac
2005d404:	212080e7          	jalr	ra,530(ra) # 9612 <_memcpy>
2005d408:	4481                	c.li	s1,0
2005d40a:	6985                	c.lui	s3,0x1
2005d40c:	01248533          	add	a0,s1,s2
2005d410:	8622                	c.mv	a2,s0
2005d412:	10000593          	addi	a1,zero,256
2005d416:	10048493          	addi	s1,s1,256
2005d41a:	f9efb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d41e:	ff3497e3          	bne	s1,s3,2005d40c <sys_recover_ota_signature+0x108>
2005d422:	8522                	c.mv	a0,s0
2005d424:	4426                	c.lwsp	s0,72(sp)
2005d426:	40b6                	c.lwsp	ra,76(sp)
2005d428:	4496                	c.lwsp	s1,68(sp)
2005d42a:	4906                	c.lwsp	s2,64(sp)
2005d42c:	59f2                	c.lwsp	s3,60(sp)
2005d42e:	6161                	c.addi16sp	sp,80
2005d430:	f8cfd06f          	jal	zero,2005abbc <__wrap_free>
2005d434:	4772                	c.lwsp	a4,28(sp)
2005d436:	f80007b7          	lui	a5,0xf8000
2005d43a:	bf89                	c.j	2005d38c <sys_recover_ota_signature+0x88>
2005d43c:	47f2                	c.lwsp	a5,28(sp)
2005d43e:	b79d                	c.j	2005d3a4 <sys_recover_ota_signature+0xa0>

2005d440 <sys_reset>:
2005d440:	fc8fc06f          	jal	zero,20059c08 <System_Reset>

2005d444 <prvAddCurrentTaskToDelayedList>:
2005d444:	1101                	c.addi	sp,-32
2005d446:	ca26                	c.swsp	s1,20(sp)
2005d448:	cc22                	c.swsp	s0,24(sp)
2005d44a:	c84a                	c.swsp	s2,16(sp)
2005d44c:	c64e                	c.swsp	s3,12(sp)
2005d44e:	ce06                	c.swsp	ra,28(sp)
2005d450:	f941a983          	lw	s3,-108(gp) # 2006691c <xTickCount>
2005d454:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005d458:	842a                	c.mv	s0,a0
2005d45a:	4388                	c.lw	a0,0(a5)
2005d45c:	892e                	c.mv	s2,a1
2005d45e:	0511                	c.addi	a0,4
2005d460:	a43fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005d464:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005d468:	ed09                	c.bnez	a0,2005d482 <prvAddCurrentTaskToDelayedList+0x3e>
2005d46a:	4398                	c.lw	a4,0(a5)
2005d46c:	f8018693          	addi	a3,gp,-128 # 20066908 <uxTopReadyPriority>
2005d470:	574c                	c.lw	a1,44(a4)
2005d472:	4290                	c.lw	a2,0(a3)
2005d474:	4705                	c.li	a4,1
2005d476:	00b71733          	sll	a4,a4,a1
2005d47a:	fff74713          	xori	a4,a4,-1
2005d47e:	8f71                	c.and	a4,a2
2005d480:	c298                	c.sw	a4,0(a3)
2005d482:	577d                	c.li	a4,-1
2005d484:	02e41263          	bne	s0,a4,2005d4a8 <prvAddCurrentTaskToDelayedList+0x64>
2005d488:	02090063          	beq	s2,zero,2005d4a8 <prvAddCurrentTaskToDelayedList+0x64>
2005d48c:	438c                	c.lw	a1,0(a5)
2005d48e:	4462                	c.lwsp	s0,24(sp)
2005d490:	40f2                	c.lwsp	ra,28(sp)
2005d492:	44d2                	c.lwsp	s1,20(sp)
2005d494:	4942                	c.lwsp	s2,16(sp)
2005d496:	49b2                	c.lwsp	s3,12(sp)
2005d498:	20068537          	lui	a0,0x20068
2005d49c:	0591                	c.addi	a1,4
2005d49e:	40050513          	addi	a0,a0,1024 # 20068400 <xSuspendedTaskList>
2005d4a2:	6105                	c.addi16sp	sp,32
2005d4a4:	9bbfd06f          	jal	zero,2005ae5e <vListInsertEnd>
2005d4a8:	4398                	c.lw	a4,0(a5)
2005d4aa:	944e                	c.add	s0,s3
2005d4ac:	c340                	c.sw	s0,4(a4)
2005d4ae:	01347e63          	bgeu	s0,s3,2005d4ca <prvAddCurrentTaskToDelayedList+0x86>
2005d4b2:	f641a503          	lw	a0,-156(gp) # 200668ec <pxOverflowDelayedTaskList>
2005d4b6:	4462                	c.lwsp	s0,24(sp)
2005d4b8:	438c                	c.lw	a1,0(a5)
2005d4ba:	40f2                	c.lwsp	ra,28(sp)
2005d4bc:	44d2                	c.lwsp	s1,20(sp)
2005d4be:	4942                	c.lwsp	s2,16(sp)
2005d4c0:	49b2                	c.lwsp	s3,12(sp)
2005d4c2:	0591                	c.addi	a1,4
2005d4c4:	6105                	c.addi16sp	sp,32
2005d4c6:	9affd06f          	jal	zero,2005ae74 <vListInsert>
2005d4ca:	f601a503          	lw	a0,-160(gp) # 200668e8 <pxDelayedTaskList>
2005d4ce:	438c                	c.lw	a1,0(a5)
2005d4d0:	0591                	c.addi	a1,4
2005d4d2:	9a3fd0ef          	jal	ra,2005ae74 <vListInsert>
2005d4d6:	f8418793          	addi	a5,gp,-124 # 2006690c <xNextTaskUnblockTime>
2005d4da:	4398                	c.lw	a4,0(a5)
2005d4dc:	00e47363          	bgeu	s0,a4,2005d4e2 <prvAddCurrentTaskToDelayedList+0x9e>
2005d4e0:	c380                	c.sw	s0,0(a5)
2005d4e2:	40f2                	c.lwsp	ra,28(sp)
2005d4e4:	4462                	c.lwsp	s0,24(sp)
2005d4e6:	44d2                	c.lwsp	s1,20(sp)
2005d4e8:	4942                	c.lwsp	s2,16(sp)
2005d4ea:	49b2                	c.lwsp	s3,12(sp)
2005d4ec:	6105                	c.addi16sp	sp,32
2005d4ee:	8082                	c.jr	ra

2005d4f0 <prvWriteNameToBuffer>:
2005d4f0:	1141                	c.addi	sp,-16
2005d4f2:	c606                	c.swsp	ra,12(sp)
2005d4f4:	c422                	c.swsp	s0,8(sp)
2005d4f6:	842a                	c.mv	s0,a0
2005d4f8:	003040ef          	jal	ra,20061cfa <strcpy>
2005d4fc:	8522                	c.mv	a0,s0
2005d4fe:	dffac097          	auipc	ra,0xdffac
2005d502:	5da080e7          	jalr	ra,1498(ra) # 9ad8 <__wrap_strlen>
2005d506:	87aa                	c.mv	a5,a0
2005d508:	4759                	c.li	a4,22
2005d50a:	02000693          	addi	a3,zero,32
2005d50e:	02f77063          	bgeu	a4,a5,2005d52e <prvWriteNameToBuffer+0x3e>
2005d512:	475d                	c.li	a4,23
2005d514:	4781                	c.li	a5,0
2005d516:	00a76463          	bltu	a4,a0,2005d51e <prvWriteNameToBuffer+0x2e>
2005d51a:	40a707b3          	sub	a5,a4,a0
2005d51e:	953e                	c.add	a0,a5
2005d520:	9522                	c.add	a0,s0
2005d522:	00050023          	sb	zero,0(a0)
2005d526:	40b2                	c.lwsp	ra,12(sp)
2005d528:	4422                	c.lwsp	s0,8(sp)
2005d52a:	0141                	c.addi	sp,16
2005d52c:	8082                	c.jr	ra
2005d52e:	00f40633          	add	a2,s0,a5
2005d532:	00d60023          	sb	a3,0(a2)
2005d536:	0785                	c.addi	a5,1
2005d538:	bfd9                	c.j	2005d50e <prvWriteNameToBuffer+0x1e>

2005d53a <prvGetExpectedIdleTime>:
2005d53a:	f801a703          	lw	a4,-128(gp) # 20066908 <uxTopReadyPriority>
2005d53e:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005d542:	57c8                	c.lw	a0,44(a5)
2005d544:	e105                	c.bnez	a0,2005d564 <prvGetExpectedIdleTime+0x2a>
2005d546:	200687b7          	lui	a5,0x20068
2005d54a:	2e87a683          	lw	a3,744(a5) # 200682e8 <pxReadyTasksLists>
2005d54e:	4785                	c.li	a5,1
2005d550:	00d7eb63          	bltu	a5,a3,2005d566 <prvGetExpectedIdleTime+0x2c>
2005d554:	00e7e963          	bltu	a5,a4,2005d566 <prvGetExpectedIdleTime+0x2c>
2005d558:	f841a503          	lw	a0,-124(gp) # 2006690c <xNextTaskUnblockTime>
2005d55c:	f941a783          	lw	a5,-108(gp) # 2006691c <xTickCount>
2005d560:	8d1d                	c.sub	a0,a5
2005d562:	8082                	c.jr	ra
2005d564:	4501                	c.li	a0,0
2005d566:	8082                	c.jr	ra

2005d568 <prvResetNextTaskUnblockTime>:
2005d568:	f6018713          	addi	a4,gp,-160 # 200668e8 <pxDelayedTaskList>
2005d56c:	431c                	c.lw	a5,0(a4)
2005d56e:	4394                	c.lw	a3,0(a5)
2005d570:	f8418793          	addi	a5,gp,-124 # 2006690c <xNextTaskUnblockTime>
2005d574:	e681                	c.bnez	a3,2005d57c <prvResetNextTaskUnblockTime+0x14>
2005d576:	577d                	c.li	a4,-1
2005d578:	c398                	c.sw	a4,0(a5)
2005d57a:	8082                	c.jr	ra
2005d57c:	4318                	c.lw	a4,0(a4)
2005d57e:	4758                	c.lw	a4,12(a4)
2005d580:	4318                	c.lw	a4,0(a4)
2005d582:	bfdd                	c.j	2005d578 <prvResetNextTaskUnblockTime+0x10>

2005d584 <prvDeleteTCB>:
2005d584:	1141                	c.addi	sp,-16
2005d586:	c422                	c.swsp	s0,8(sp)
2005d588:	842a                	c.mv	s0,a0
2005d58a:	06c50513          	addi	a0,a0,108
2005d58e:	c606                	c.swsp	ra,12(sp)
2005d590:	680040ef          	jal	ra,20061c10 <_reclaim_reent>
2005d594:	16144783          	lbu	a5,353(s0)
2005d598:	eb91                	c.bnez	a5,2005d5ac <prvDeleteTCB+0x28>
2005d59a:	5808                	c.lw	a0,48(s0)
2005d59c:	f4cfd0ef          	jal	ra,2005ace8 <vPortFree>
2005d5a0:	8522                	c.mv	a0,s0
2005d5a2:	4422                	c.lwsp	s0,8(sp)
2005d5a4:	40b2                	c.lwsp	ra,12(sp)
2005d5a6:	0141                	c.addi	sp,16
2005d5a8:	f40fd06f          	jal	zero,2005ace8 <vPortFree>
2005d5ac:	4705                	c.li	a4,1
2005d5ae:	fee789e3          	beq	a5,a4,2005d5a0 <prvDeleteTCB+0x1c>
2005d5b2:	40b2                	c.lwsp	ra,12(sp)
2005d5b4:	4422                	c.lwsp	s0,8(sp)
2005d5b6:	0141                	c.addi	sp,16
2005d5b8:	8082                	c.jr	ra

2005d5ba <prvInitialiseNewTask.constprop.0>:
2005d5ba:	1101                	c.addi	sp,-32
2005d5bc:	c452                	c.swsp	s4,8(sp)
2005d5be:	8a2a                	c.mv	s4,a0
2005d5c0:	03082503          	lw	a0,48(a6)
2005d5c4:	c64e                	c.swsp	s3,12(sp)
2005d5c6:	00261993          	slli	s3,a2,0x2
2005d5ca:	864e                	c.mv	a2,s3
2005d5cc:	ca26                	c.swsp	s1,20(sp)
2005d5ce:	84ae                	c.mv	s1,a1
2005d5d0:	0a500593          	addi	a1,zero,165
2005d5d4:	cc22                	c.swsp	s0,24(sp)
2005d5d6:	c84a                	c.swsp	s2,16(sp)
2005d5d8:	8442                	c.mv	s0,a6
2005d5da:	c256                	c.swsp	s5,4(sp)
2005d5dc:	c05a                	c.swsp	s6,0(sp)
2005d5de:	ce06                	c.swsp	ra,28(sp)
2005d5e0:	8ab6                	c.mv	s5,a3
2005d5e2:	893a                	c.mv	s2,a4
2005d5e4:	8b3e                	c.mv	s6,a5
2005d5e6:	dffac097          	auipc	ra,0xdffac
2005d5ea:	4e8080e7          	jalr	ra,1256(ra) # 9ace <__wrap_memset>
2005d5ee:	ffc98613          	addi	a2,s3,-4 # ffc <CPU_ClkGet+0x4e6>
2005d5f2:	03042983          	lw	s3,48(s0)
2005d5f6:	99b2                	c.add	s3,a2
2005d5f8:	fe09f993          	andi	s3,s3,-32
2005d5fc:	05342623          	sw	s3,76(s0)
2005d600:	c8f9                	c.beqz	s1,2005d6d6 <prvInitialiseNewTask.constprop.0+0x11c>
2005d602:	85a6                	c.mv	a1,s1
2005d604:	03440793          	addi	a5,s0,52
2005d608:	01848693          	addi	a3,s1,24
2005d60c:	0005c703          	lbu	a4,0(a1) # 1000 <CPU_ClkGet+0x4ea>
2005d610:	00e78023          	sb	a4,0(a5)
2005d614:	c709                	c.beqz	a4,2005d61e <prvInitialiseNewTask.constprop.0+0x64>
2005d616:	0585                	c.addi	a1,1
2005d618:	0785                	c.addi	a5,1
2005d61a:	fed599e3          	bne	a1,a3,2005d60c <prvInitialiseNewTask.constprop.0+0x52>
2005d61e:	040405a3          	sb	zero,75(s0)
2005d622:	47a9                	c.li	a5,10
2005d624:	0127f363          	bgeu	a5,s2,2005d62a <prvInitialiseNewTask.constprop.0+0x70>
2005d628:	4929                	c.li	s2,10
2005d62a:	00440513          	addi	a0,s0,4
2005d62e:	03242623          	sw	s2,44(s0)
2005d632:	05242e23          	sw	s2,92(s0)
2005d636:	06042023          	sw	zero,96(s0)
2005d63a:	81ffd0ef          	jal	ra,2005ae58 <vListInitialiseItem>
2005d63e:	01840513          	addi	a0,s0,24
2005d642:	817fd0ef          	jal	ra,2005ae58 <vListInitialiseItem>
2005d646:	472d                	c.li	a4,11
2005d648:	412704b3          	sub	s1,a4,s2
2005d64c:	4611                	c.li	a2,4
2005d64e:	4581                	c.li	a1,0
2005d650:	c800                	c.sw	s0,16(s0)
2005d652:	cc04                	c.sw	s1,24(s0)
2005d654:	d040                	c.sw	s0,36(s0)
2005d656:	04042823          	sw	zero,80(s0)
2005d65a:	06042223          	sw	zero,100(s0)
2005d65e:	06042423          	sw	zero,104(s0)
2005d662:	15c40513          	addi	a0,s0,348
2005d666:	dffac097          	auipc	ra,0xdffac
2005d66a:	468080e7          	jalr	ra,1128(ra) # 9ace <__wrap_memset>
2005d66e:	4605                	c.li	a2,1
2005d670:	4581                	c.li	a1,0
2005d672:	16040513          	addi	a0,s0,352
2005d676:	dffac097          	auipc	ra,0xdffac
2005d67a:	458080e7          	jalr	ra,1112(ra) # 9ace <__wrap_memset>
2005d67e:	0f000613          	addi	a2,zero,240
2005d682:	4581                	c.li	a1,0
2005d684:	06c40513          	addi	a0,s0,108
2005d688:	dffac097          	auipc	ra,0xdffac
2005d68c:	446080e7          	jalr	ra,1094(ra) # 9ace <__wrap_memset>
2005d690:	200697b7          	lui	a5,0x20069
2005d694:	9c078793          	addi	a5,a5,-1600 # 200689c0 <__sf>
2005d698:	d83c                	c.sw	a5,112(s0)
2005d69a:	200697b7          	lui	a5,0x20069
2005d69e:	a2878793          	addi	a5,a5,-1496 # 20068a28 <__sf+0x68>
2005d6a2:	d87c                	c.sw	a5,116(s0)
2005d6a4:	200697b7          	lui	a5,0x20069
2005d6a8:	a9078793          	addi	a5,a5,-1392 # 20068a90 <__sf+0xd0>
2005d6ac:	dc3c                	c.sw	a5,120(s0)
2005d6ae:	8656                	c.mv	a2,s5
2005d6b0:	85d2                	c.mv	a1,s4
2005d6b2:	854e                	c.mv	a0,s3
2005d6b4:	ae5fe0ef          	jal	ra,2005c198 <pxPortInitialiseStack>
2005d6b8:	c008                	c.sw	a0,0(s0)
2005d6ba:	000b0463          	beq	s6,zero,2005d6c2 <prvInitialiseNewTask.constprop.0+0x108>
2005d6be:	008b2023          	sw	s0,0(s6)
2005d6c2:	40f2                	c.lwsp	ra,28(sp)
2005d6c4:	4462                	c.lwsp	s0,24(sp)
2005d6c6:	44d2                	c.lwsp	s1,20(sp)
2005d6c8:	4942                	c.lwsp	s2,16(sp)
2005d6ca:	49b2                	c.lwsp	s3,12(sp)
2005d6cc:	4a22                	c.lwsp	s4,8(sp)
2005d6ce:	4a92                	c.lwsp	s5,4(sp)
2005d6d0:	4b02                	c.lwsp	s6,0(sp)
2005d6d2:	6105                	c.addi16sp	sp,32
2005d6d4:	8082                	c.jr	ra
2005d6d6:	02040a23          	sb	zero,52(s0)
2005d6da:	b7a1                	c.j	2005d622 <prvInitialiseNewTask.constprop.0+0x68>

2005d6dc <vTaskSuspendAll>:
2005d6dc:	f7818793          	addi	a5,gp,-136 # 20066900 <uxSchedulerSuspended>
2005d6e0:	4398                	c.lw	a4,0(a5)
2005d6e2:	0705                	c.addi	a4,1
2005d6e4:	c398                	c.sw	a4,0(a5)
2005d6e6:	8082                	c.jr	ra

2005d6e8 <xTaskGetTickCountFromISR>:
2005d6e8:	f941a503          	lw	a0,-108(gp) # 2006691c <xTickCount>
2005d6ec:	8082                	c.jr	ra

2005d6ee <uxTaskGetNumberOfTasks>:
2005d6ee:	f701a503          	lw	a0,-144(gp) # 200668f8 <uxCurrentNumberOfTasks>
2005d6f2:	8082                	c.jr	ra

2005d6f4 <pcTaskGetName>:
2005d6f4:	e119                	c.bnez	a0,2005d6fa <pcTaskGetName+0x6>
2005d6f6:	f5c1a503          	lw	a0,-164(gp) # 200668e4 <pxCurrentTCB>
2005d6fa:	03450513          	addi	a0,a0,52
2005d6fe:	8082                	c.jr	ra

2005d700 <xTaskIncrementTick>:
2005d700:	f781a783          	lw	a5,-136(gp) # 20066900 <uxSchedulerSuspended>
2005d704:	7179                	c.addi16sp	sp,-48
2005d706:	d606                	c.swsp	ra,44(sp)
2005d708:	d422                	c.swsp	s0,40(sp)
2005d70a:	d226                	c.swsp	s1,36(sp)
2005d70c:	d04a                	c.swsp	s2,32(sp)
2005d70e:	ce4e                	c.swsp	s3,28(sp)
2005d710:	cc52                	c.swsp	s4,24(sp)
2005d712:	ca56                	c.swsp	s5,20(sp)
2005d714:	c85a                	c.swsp	s6,16(sp)
2005d716:	c65e                	c.swsp	s7,12(sp)
2005d718:	c462                	c.swsp	s8,8(sp)
2005d71a:	c266                	c.swsp	s9,4(sp)
2005d71c:	c06a                	c.swsp	s10,0(sp)
2005d71e:	10079363          	bne	a5,zero,2005d824 <xTaskIncrementTick+0x124>
2005d722:	f9418793          	addi	a5,gp,-108 # 2006691c <xTickCount>
2005d726:	0007aa03          	lw	s4,0(a5)
2005d72a:	0a05                	c.addi	s4,1
2005d72c:	0147a023          	sw	s4,0(a5)
2005d730:	020a1163          	bne	s4,zero,2005d752 <xTaskIncrementTick+0x52>
2005d734:	f6018713          	addi	a4,gp,-160 # 200668e8 <pxDelayedTaskList>
2005d738:	f6418793          	addi	a5,gp,-156 # 200668ec <pxOverflowDelayedTaskList>
2005d73c:	4314                	c.lw	a3,0(a4)
2005d73e:	4390                	c.lw	a2,0(a5)
2005d740:	c310                	c.sw	a2,0(a4)
2005d742:	c394                	c.sw	a3,0(a5)
2005d744:	f8818793          	addi	a5,gp,-120 # 20066910 <xNumOfOverflows>
2005d748:	4398                	c.lw	a4,0(a5)
2005d74a:	0705                	c.addi	a4,1
2005d74c:	c398                	c.sw	a4,0(a5)
2005d74e:	e1bff0ef          	jal	ra,2005d568 <prvResetNextTaskUnblockTime>
2005d752:	f8418793          	addi	a5,gp,-124 # 2006690c <xNextTaskUnblockTime>
2005d756:	439c                	c.lw	a5,0(a5)
2005d758:	200684b7          	lui	s1,0x20068
2005d75c:	f8418993          	addi	s3,gp,-124 # 2006690c <xNextTaskUnblockTime>
2005d760:	2e848493          	addi	s1,s1,744 # 200682e8 <pxReadyTasksLists>
2005d764:	f5c18a93          	addi	s5,gp,-164 # 200668e4 <pxCurrentTCB>
2005d768:	4401                	c.li	s0,0
2005d76a:	04fa7663          	bgeu	s4,a5,2005d7b6 <xTaskIncrementTick+0xb6>
2005d76e:	000aa783          	lw	a5,0(s5)
2005d772:	4751                	c.li	a4,20
2005d774:	57dc                	c.lw	a5,44(a5)
2005d776:	02e787b3          	mul	a5,a5,a4
2005d77a:	94be                	c.add	s1,a5
2005d77c:	4098                	c.lw	a4,0(s1)
2005d77e:	4785                	c.li	a5,1
2005d780:	00e7f363          	bgeu	a5,a4,2005d786 <xTaskIncrementTick+0x86>
2005d784:	4405                	c.li	s0,1
2005d786:	f8c1a783          	lw	a5,-116(gp) # 20066914 <xPendedTicks>
2005d78a:	e399                	c.bnez	a5,2005d790 <xTaskIncrementTick+0x90>
2005d78c:	e41fe0ef          	jal	ra,2005c5cc <vApplicationTickHook>
2005d790:	f981a783          	lw	a5,-104(gp) # 20066920 <xYieldPending>
2005d794:	c391                	c.beqz	a5,2005d798 <xTaskIncrementTick+0x98>
2005d796:	4405                	c.li	s0,1
2005d798:	50b2                	c.lwsp	ra,44(sp)
2005d79a:	8522                	c.mv	a0,s0
2005d79c:	5422                	c.lwsp	s0,40(sp)
2005d79e:	5492                	c.lwsp	s1,36(sp)
2005d7a0:	5902                	c.lwsp	s2,32(sp)
2005d7a2:	49f2                	c.lwsp	s3,28(sp)
2005d7a4:	4a62                	c.lwsp	s4,24(sp)
2005d7a6:	4ad2                	c.lwsp	s5,20(sp)
2005d7a8:	4b42                	c.lwsp	s6,16(sp)
2005d7aa:	4bb2                	c.lwsp	s7,12(sp)
2005d7ac:	4c22                	c.lwsp	s8,8(sp)
2005d7ae:	4c92                	c.lwsp	s9,4(sp)
2005d7b0:	4d02                	c.lwsp	s10,0(sp)
2005d7b2:	6145                	c.addi16sp	sp,48
2005d7b4:	8082                	c.jr	ra
2005d7b6:	4c05                	c.li	s8,1
2005d7b8:	4cd1                	c.li	s9,20
2005d7ba:	f6018793          	addi	a5,gp,-160 # 200668e8 <pxDelayedTaskList>
2005d7be:	4398                	c.lw	a4,0(a5)
2005d7c0:	4318                	c.lw	a4,0(a4)
2005d7c2:	e709                	c.bnez	a4,2005d7cc <xTaskIncrementTick+0xcc>
2005d7c4:	57fd                	c.li	a5,-1
2005d7c6:	00f9a023          	sw	a5,0(s3)
2005d7ca:	b755                	c.j	2005d76e <xTaskIncrementTick+0x6e>
2005d7cc:	439c                	c.lw	a5,0(a5)
2005d7ce:	47dc                	c.lw	a5,12(a5)
2005d7d0:	00c7a903          	lw	s2,12(a5)
2005d7d4:	00492783          	lw	a5,4(s2)
2005d7d8:	fefa67e3          	bltu	s4,a5,2005d7c6 <xTaskIncrementTick+0xc6>
2005d7dc:	00490d13          	addi	s10,s2,4
2005d7e0:	856a                	c.mv	a0,s10
2005d7e2:	ec0fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005d7e6:	02892783          	lw	a5,40(s2)
2005d7ea:	c789                	c.beqz	a5,2005d7f4 <xTaskIncrementTick+0xf4>
2005d7ec:	01890513          	addi	a0,s2,24
2005d7f0:	eb2fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005d7f4:	02c92503          	lw	a0,44(s2)
2005d7f8:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005d7fc:	4314                	c.lw	a3,0(a4)
2005d7fe:	00ac17b3          	sll	a5,s8,a0
2005d802:	03950533          	mul	a0,a0,s9
2005d806:	8fd5                	c.or	a5,a3
2005d808:	85ea                	c.mv	a1,s10
2005d80a:	c31c                	c.sw	a5,0(a4)
2005d80c:	9526                	c.add	a0,s1
2005d80e:	e50fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005d812:	000aa783          	lw	a5,0(s5)
2005d816:	02c92703          	lw	a4,44(s2)
2005d81a:	57dc                	c.lw	a5,44(a5)
2005d81c:	f8f76fe3          	bltu	a4,a5,2005d7ba <xTaskIncrementTick+0xba>
2005d820:	4405                	c.li	s0,1
2005d822:	bf61                	c.j	2005d7ba <xTaskIncrementTick+0xba>
2005d824:	f8c18793          	addi	a5,gp,-116 # 20066914 <xPendedTicks>
2005d828:	4398                	c.lw	a4,0(a5)
2005d82a:	4401                	c.li	s0,0
2005d82c:	0705                	c.addi	a4,1
2005d82e:	c398                	c.sw	a4,0(a5)
2005d830:	d9dfe0ef          	jal	ra,2005c5cc <vApplicationTickHook>
2005d834:	b795                	c.j	2005d798 <xTaskIncrementTick+0x98>

2005d836 <vTaskSwitchContext>:
2005d836:	f781a703          	lw	a4,-136(gp) # 20066900 <uxSchedulerSuspended>
2005d83a:	f9818793          	addi	a5,gp,-104 # 20066920 <xYieldPending>
2005d83e:	c701                	c.beqz	a4,2005d846 <vTaskSwitchContext+0x10>
2005d840:	4705                	c.li	a4,1
2005d842:	c398                	c.sw	a4,0(a5)
2005d844:	8082                	c.jr	ra
2005d846:	1141                	c.addi	sp,-16
2005d848:	c422                	c.swsp	s0,8(sp)
2005d84a:	c606                	c.swsp	ra,12(sp)
2005d84c:	0007a023          	sw	zero,0(a5)
2005d850:	410187b7          	lui	a5,0x41018
2005d854:	4bd4                	c.lw	a3,20(a5)
2005d856:	f6c18713          	addi	a4,gp,-148 # 200668f4 <ulTotalRunTime>
2005d85a:	c314                	c.sw	a3,0(a4)
2005d85c:	f681a583          	lw	a1,-152(gp) # 200668f0 <ulTaskSwitchedInTime>
2005d860:	4318                	c.lw	a4,0(a4)
2005d862:	f6c18793          	addi	a5,gp,-148 # 200668f4 <ulTotalRunTime>
2005d866:	f6818693          	addi	a3,gp,-152 # 200668f0 <ulTaskSwitchedInTime>
2005d86a:	f5c18413          	addi	s0,gp,-164 # 200668e4 <pxCurrentTCB>
2005d86e:	00e5f863          	bgeu	a1,a4,2005d87e <vTaskSwitchContext+0x48>
2005d872:	4388                	c.lw	a0,0(a5)
2005d874:	4010                	c.lw	a2,0(s0)
2005d876:	5638                	c.lw	a4,104(a2)
2005d878:	8f0d                	c.sub	a4,a1
2005d87a:	972a                	c.add	a4,a0
2005d87c:	d638                	c.sw	a4,104(a2)
2005d87e:	439c                	c.lw	a5,0(a5)
2005d880:	a5a5a737          	lui	a4,0xa5a5a
2005d884:	5a570713          	addi	a4,a4,1445 # a5a5a5a5 <__ctrace_end__+0xdba5a5a5>
2005d888:	c29c                	c.sw	a5,0(a3)
2005d88a:	401c                	c.lw	a5,0(s0)
2005d88c:	5b9c                	c.lw	a5,48(a5)
2005d88e:	4390                	c.lw	a2,0(a5)
2005d890:	00e61b63          	bne	a2,a4,2005d8a6 <vTaskSwitchContext+0x70>
2005d894:	43d4                	c.lw	a3,4(a5)
2005d896:	00c69863          	bne	a3,a2,2005d8a6 <vTaskSwitchContext+0x70>
2005d89a:	4798                	c.lw	a4,8(a5)
2005d89c:	00d71563          	bne	a4,a3,2005d8a6 <vTaskSwitchContext+0x70>
2005d8a0:	47dc                	c.lw	a5,12(a5)
2005d8a2:	00e78863          	beq	a5,a4,2005d8b2 <vTaskSwitchContext+0x7c>
2005d8a6:	4008                	c.lw	a0,0(s0)
2005d8a8:	400c                	c.lw	a1,0(s0)
2005d8aa:	03458593          	addi	a1,a1,52
2005d8ae:	d77fe0ef          	jal	ra,2005c624 <vApplicationStackOverflowHook>
2005d8b2:	f801a503          	lw	a0,-128(gp) # 20066908 <uxTopReadyPriority>
2005d8b6:	3e7030ef          	jal	ra,2006149c <__clzsi2>
2005d8ba:	47fd                	c.li	a5,31
2005d8bc:	40a78533          	sub	a0,a5,a0
2005d8c0:	4751                	c.li	a4,20
2005d8c2:	02e50733          	mul	a4,a0,a4
2005d8c6:	200687b7          	lui	a5,0x20068
2005d8ca:	2e878693          	addi	a3,a5,744 # 200682e8 <pxReadyTasksLists>
2005d8ce:	2e878793          	addi	a5,a5,744
2005d8d2:	00e68633          	add	a2,a3,a4
2005d8d6:	424c                	c.lw	a1,4(a2)
2005d8d8:	0721                	c.addi	a4,8
2005d8da:	9736                	c.add	a4,a3
2005d8dc:	41cc                	c.lw	a1,4(a1)
2005d8de:	c24c                	c.sw	a1,4(a2)
2005d8e0:	00e59463          	bne	a1,a4,2005d8e8 <vTaskSwitchContext+0xb2>
2005d8e4:	41d8                	c.lw	a4,4(a1)
2005d8e6:	c258                	c.sw	a4,4(a2)
2005d8e8:	4751                	c.li	a4,20
2005d8ea:	02e50533          	mul	a0,a0,a4
2005d8ee:	40b2                	c.lwsp	ra,12(sp)
2005d8f0:	97aa                	c.add	a5,a0
2005d8f2:	43dc                	c.lw	a5,4(a5)
2005d8f4:	47dc                	c.lw	a5,12(a5)
2005d8f6:	c01c                	c.sw	a5,0(s0)
2005d8f8:	401c                	c.lw	a5,0(s0)
2005d8fa:	4422                	c.lwsp	s0,8(sp)
2005d8fc:	06c78793          	addi	a5,a5,108
2005d900:	e8f1aa23          	sw	a5,-364(gp) # 2006681c <_impure_ptr>
2005d904:	0141                	c.addi	sp,16
2005d906:	8082                	c.jr	ra

2005d908 <vTaskPlaceOnEventList>:
2005d908:	1141                	c.addi	sp,-16
2005d90a:	c422                	c.swsp	s0,8(sp)
2005d90c:	842e                	c.mv	s0,a1
2005d90e:	f5c1a583          	lw	a1,-164(gp) # 200668e4 <pxCurrentTCB>
2005d912:	c606                	c.swsp	ra,12(sp)
2005d914:	05e1                	c.addi	a1,24
2005d916:	d5efd0ef          	jal	ra,2005ae74 <vListInsert>
2005d91a:	8522                	c.mv	a0,s0
2005d91c:	4422                	c.lwsp	s0,8(sp)
2005d91e:	40b2                	c.lwsp	ra,12(sp)
2005d920:	4585                	c.li	a1,1
2005d922:	0141                	c.addi	sp,16
2005d924:	b21ff06f          	jal	zero,2005d444 <prvAddCurrentTaskToDelayedList>

2005d928 <vTaskPlaceOnEventListRestricted>:
2005d928:	1141                	c.addi	sp,-16
2005d92a:	c422                	c.swsp	s0,8(sp)
2005d92c:	842e                	c.mv	s0,a1
2005d92e:	f5c1a583          	lw	a1,-164(gp) # 200668e4 <pxCurrentTCB>
2005d932:	c226                	c.swsp	s1,4(sp)
2005d934:	84b2                	c.mv	s1,a2
2005d936:	05e1                	c.addi	a1,24
2005d938:	c606                	c.swsp	ra,12(sp)
2005d93a:	d24fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005d93e:	c091                	c.beqz	s1,2005d942 <vTaskPlaceOnEventListRestricted+0x1a>
2005d940:	547d                	c.li	s0,-1
2005d942:	8522                	c.mv	a0,s0
2005d944:	4422                	c.lwsp	s0,8(sp)
2005d946:	40b2                	c.lwsp	ra,12(sp)
2005d948:	85a6                	c.mv	a1,s1
2005d94a:	4492                	c.lwsp	s1,4(sp)
2005d94c:	0141                	c.addi	sp,16
2005d94e:	af7ff06f          	jal	zero,2005d444 <prvAddCurrentTaskToDelayedList>

2005d952 <xTaskRemoveFromEventList>:
2005d952:	1101                	c.addi	sp,-32
2005d954:	ce06                	c.swsp	ra,28(sp)
2005d956:	cc22                	c.swsp	s0,24(sp)
2005d958:	455c                	c.lw	a5,12(a0)
2005d95a:	47c0                	c.lw	s0,12(a5)
2005d95c:	01840593          	addi	a1,s0,24
2005d960:	852e                	c.mv	a0,a1
2005d962:	c62e                	c.swsp	a1,12(sp)
2005d964:	d3efd0ef          	jal	ra,2005aea2 <uxListRemove>
2005d968:	f781a783          	lw	a5,-136(gp) # 20066900 <uxSchedulerSuspended>
2005d96c:	45b2                	c.lwsp	a1,12(sp)
2005d96e:	efa1                	c.bnez	a5,2005d9c6 <xTaskRemoveFromEventList+0x74>
2005d970:	00440593          	addi	a1,s0,4
2005d974:	852e                	c.mv	a0,a1
2005d976:	c62e                	c.swsp	a1,12(sp)
2005d978:	d2afd0ef          	jal	ra,2005aea2 <uxListRemove>
2005d97c:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005d980:	5448                	c.lw	a0,44(s0)
2005d982:	4314                	c.lw	a3,0(a4)
2005d984:	4785                	c.li	a5,1
2005d986:	00a797b3          	sll	a5,a5,a0
2005d98a:	8fd5                	c.or	a5,a3
2005d98c:	c31c                	c.sw	a5,0(a4)
2005d98e:	47d1                	c.li	a5,20
2005d990:	02f50533          	mul	a0,a0,a5
2005d994:	45b2                	c.lwsp	a1,12(sp)
2005d996:	200687b7          	lui	a5,0x20068
2005d99a:	2e878793          	addi	a5,a5,744 # 200682e8 <pxReadyTasksLists>
2005d99e:	953e                	c.add	a0,a5
2005d9a0:	cbefd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005d9a4:	bc5ff0ef          	jal	ra,2005d568 <prvResetNextTaskUnblockTime>
2005d9a8:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005d9ac:	5458                	c.lw	a4,44(s0)
2005d9ae:	4501                	c.li	a0,0
2005d9b0:	57dc                	c.lw	a5,44(a5)
2005d9b2:	00e7f663          	bgeu	a5,a4,2005d9be <xTaskRemoveFromEventList+0x6c>
2005d9b6:	4705                	c.li	a4,1
2005d9b8:	f8e1ac23          	sw	a4,-104(gp) # 20066920 <xYieldPending>
2005d9bc:	4505                	c.li	a0,1
2005d9be:	40f2                	c.lwsp	ra,28(sp)
2005d9c0:	4462                	c.lwsp	s0,24(sp)
2005d9c2:	6105                	c.addi16sp	sp,32
2005d9c4:	8082                	c.jr	ra
2005d9c6:	20068537          	lui	a0,0x20068
2005d9ca:	3ec50513          	addi	a0,a0,1004 # 200683ec <xPendingReadyList>
2005d9ce:	c90fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005d9d2:	bfd9                	c.j	2005d9a8 <xTaskRemoveFromEventList+0x56>

2005d9d4 <vTaskInternalSetTimeOutState>:
2005d9d4:	f881a783          	lw	a5,-120(gp) # 20066910 <xNumOfOverflows>
2005d9d8:	c11c                	c.sw	a5,0(a0)
2005d9da:	f941a783          	lw	a5,-108(gp) # 2006691c <xTickCount>
2005d9de:	c15c                	c.sw	a5,4(a0)
2005d9e0:	8082                	c.jr	ra

2005d9e2 <vTaskMissedYield>:
2005d9e2:	4705                	c.li	a4,1
2005d9e4:	f8e1ac23          	sw	a4,-104(gp) # 20066920 <xYieldPending>
2005d9e8:	8082                	c.jr	ra

2005d9ea <eTaskConfirmSleepModeStatus>:
2005d9ea:	200687b7          	lui	a5,0x20068
2005d9ee:	3ec7a783          	lw	a5,1004(a5) # 200683ec <xPendingReadyList>
2005d9f2:	4501                	c.li	a0,0
2005d9f4:	e395                	c.bnez	a5,2005da18 <eTaskConfirmSleepModeStatus+0x2e>
2005d9f6:	f981a783          	lw	a5,-104(gp) # 20066920 <xYieldPending>
2005d9fa:	ef99                	c.bnez	a5,2005da18 <eTaskConfirmSleepModeStatus+0x2e>
2005d9fc:	f8c1a783          	lw	a5,-116(gp) # 20066914 <xPendedTicks>
2005da00:	ef81                	c.bnez	a5,2005da18 <eTaskConfirmSleepModeStatus+0x2e>
2005da02:	200687b7          	lui	a5,0x20068
2005da06:	4007a703          	lw	a4,1024(a5) # 20068400 <xSuspendedTaskList>
2005da0a:	f701a783          	lw	a5,-144(gp) # 200668f8 <uxCurrentNumberOfTasks>
2005da0e:	4505                	c.li	a0,1
2005da10:	17fd                	c.addi	a5,-1
2005da12:	00f71363          	bne	a4,a5,2005da18 <eTaskConfirmSleepModeStatus+0x2e>
2005da16:	4509                	c.li	a0,2
2005da18:	8082                	c.jr	ra

2005da1a <xTaskGetCurrentTaskHandle>:
2005da1a:	f5c1a503          	lw	a0,-164(gp) # 200668e4 <pxCurrentTCB>
2005da1e:	8082                	c.jr	ra

2005da20 <xTaskGetSchedulerState>:
2005da20:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005da24:	4505                	c.li	a0,1
2005da26:	c791                	c.beqz	a5,2005da32 <xTaskGetSchedulerState+0x12>
2005da28:	f781a503          	lw	a0,-136(gp) # 20066900 <uxSchedulerSuspended>
2005da2c:	00153513          	sltiu	a0,a0,1
2005da30:	0506                	c.slli	a0,0x1
2005da32:	8082                	c.jr	ra

2005da34 <xTaskPriorityInherit>:
2005da34:	c945                	c.beqz	a0,2005dae4 <xTaskPriorityInherit+0xb0>
2005da36:	1101                	c.addi	sp,-32
2005da38:	ca26                	c.swsp	s1,20(sp)
2005da3a:	cc22                	c.swsp	s0,24(sp)
2005da3c:	ce06                	c.swsp	ra,28(sp)
2005da3e:	c84a                	c.swsp	s2,16(sp)
2005da40:	f5c18713          	addi	a4,gp,-164 # 200668e4 <pxCurrentTCB>
2005da44:	4314                	c.lw	a3,0(a4)
2005da46:	555c                	c.lw	a5,44(a0)
2005da48:	842a                	c.mv	s0,a0
2005da4a:	56d4                	c.lw	a3,44(a3)
2005da4c:	f5c18493          	addi	s1,gp,-164 # 200668e4 <pxCurrentTCB>
2005da50:	08d7f463          	bgeu	a5,a3,2005dad8 <xTaskPriorityInherit+0xa4>
2005da54:	4d18                	c.lw	a4,24(a0)
2005da56:	00074763          	blt	a4,zero,2005da64 <xTaskPriorityInherit+0x30>
2005da5a:	4098                	c.lw	a4,0(s1)
2005da5c:	5754                	c.lw	a3,44(a4)
2005da5e:	472d                	c.li	a4,11
2005da60:	8f15                	c.sub	a4,a3
2005da62:	cd18                	c.sw	a4,24(a0)
2005da64:	46d1                	c.li	a3,20
2005da66:	02d787b3          	mul	a5,a5,a3
2005da6a:	20068537          	lui	a0,0x20068
2005da6e:	2e850713          	addi	a4,a0,744 # 200682e8 <pxReadyTasksLists>
2005da72:	2e850913          	addi	s2,a0,744
2005da76:	97ba                	c.add	a5,a4
2005da78:	4858                	c.lw	a4,20(s0)
2005da7a:	04f71b63          	bne	a4,a5,2005dad0 <xTaskPriorityInherit+0x9c>
2005da7e:	00440593          	addi	a1,s0,4
2005da82:	852e                	c.mv	a0,a1
2005da84:	c62e                	c.swsp	a1,12(sp)
2005da86:	c1cfd0ef          	jal	ra,2005aea2 <uxListRemove>
2005da8a:	45b2                	c.lwsp	a1,12(sp)
2005da8c:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005da90:	e911                	c.bnez	a0,2005daa4 <xTaskPriorityInherit+0x70>
2005da92:	5450                	c.lw	a2,44(s0)
2005da94:	4314                	c.lw	a3,0(a4)
2005da96:	4785                	c.li	a5,1
2005da98:	00c797b3          	sll	a5,a5,a2
2005da9c:	fff7c793          	xori	a5,a5,-1
2005daa0:	8ff5                	c.and	a5,a3
2005daa2:	c31c                	c.sw	a5,0(a4)
2005daa4:	409c                	c.lw	a5,0(s1)
2005daa6:	4314                	c.lw	a3,0(a4)
2005daa8:	57c8                	c.lw	a0,44(a5)
2005daaa:	4785                	c.li	a5,1
2005daac:	00a797b3          	sll	a5,a5,a0
2005dab0:	8fd5                	c.or	a5,a3
2005dab2:	c31c                	c.sw	a5,0(a4)
2005dab4:	47d1                	c.li	a5,20
2005dab6:	d448                	c.sw	a0,44(s0)
2005dab8:	02f50533          	mul	a0,a0,a5
2005dabc:	954a                	c.add	a0,s2
2005dabe:	ba0fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005dac2:	4505                	c.li	a0,1
2005dac4:	40f2                	c.lwsp	ra,28(sp)
2005dac6:	4462                	c.lwsp	s0,24(sp)
2005dac8:	44d2                	c.lwsp	s1,20(sp)
2005daca:	4942                	c.lwsp	s2,16(sp)
2005dacc:	6105                	c.addi16sp	sp,32
2005dace:	8082                	c.jr	ra
2005dad0:	409c                	c.lw	a5,0(s1)
2005dad2:	57dc                	c.lw	a5,44(a5)
2005dad4:	d45c                	c.sw	a5,44(s0)
2005dad6:	b7f5                	c.j	2005dac2 <xTaskPriorityInherit+0x8e>
2005dad8:	431c                	c.lw	a5,0(a4)
2005dada:	4d68                	c.lw	a0,92(a0)
2005dadc:	57dc                	c.lw	a5,44(a5)
2005dade:	00f53533          	sltu	a0,a0,a5
2005dae2:	b7cd                	c.j	2005dac4 <xTaskPriorityInherit+0x90>
2005dae4:	4501                	c.li	a0,0
2005dae6:	8082                	c.jr	ra

2005dae8 <xTaskPriorityDisinherit>:
2005dae8:	e901                	c.bnez	a0,2005daf8 <xTaskPriorityDisinherit+0x10>
2005daea:	4501                	c.li	a0,0
2005daec:	8082                	c.jr	ra
2005daee:	4501                	c.li	a0,0
2005daf0:	40f2                	c.lwsp	ra,28(sp)
2005daf2:	4462                	c.lwsp	s0,24(sp)
2005daf4:	6105                	c.addi16sp	sp,32
2005daf6:	8082                	c.jr	ra
2005daf8:	1101                	c.addi	sp,-32
2005dafa:	cc22                	c.swsp	s0,24(sp)
2005dafc:	ce06                	c.swsp	ra,28(sp)
2005dafe:	513c                	c.lw	a5,96(a0)
2005db00:	5554                	c.lw	a3,44(a0)
2005db02:	4d78                	c.lw	a4,92(a0)
2005db04:	17fd                	c.addi	a5,-1
2005db06:	d13c                	c.sw	a5,96(a0)
2005db08:	842a                	c.mv	s0,a0
2005db0a:	fee682e3          	beq	a3,a4,2005daee <xTaskPriorityDisinherit+0x6>
2005db0e:	f3e5                	c.bnez	a5,2005daee <xTaskPriorityDisinherit+0x6>
2005db10:	00450593          	addi	a1,a0,4
2005db14:	852e                	c.mv	a0,a1
2005db16:	c62e                	c.swsp	a1,12(sp)
2005db18:	b8afd0ef          	jal	ra,2005aea2 <uxListRemove>
2005db1c:	45b2                	c.lwsp	a1,12(sp)
2005db1e:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005db22:	e911                	c.bnez	a0,2005db36 <xTaskPriorityDisinherit+0x4e>
2005db24:	5450                	c.lw	a2,44(s0)
2005db26:	4314                	c.lw	a3,0(a4)
2005db28:	4785                	c.li	a5,1
2005db2a:	00c797b3          	sll	a5,a5,a2
2005db2e:	fff7c793          	xori	a5,a5,-1
2005db32:	8ff5                	c.and	a5,a3
2005db34:	c31c                	c.sw	a5,0(a4)
2005db36:	4c68                	c.lw	a0,92(s0)
2005db38:	47ad                	c.li	a5,11
2005db3a:	4314                	c.lw	a3,0(a4)
2005db3c:	8f89                	c.sub	a5,a0
2005db3e:	cc1c                	c.sw	a5,24(s0)
2005db40:	4785                	c.li	a5,1
2005db42:	00a797b3          	sll	a5,a5,a0
2005db46:	8fd5                	c.or	a5,a3
2005db48:	c31c                	c.sw	a5,0(a4)
2005db4a:	47d1                	c.li	a5,20
2005db4c:	d448                	c.sw	a0,44(s0)
2005db4e:	02f50533          	mul	a0,a0,a5
2005db52:	200687b7          	lui	a5,0x20068
2005db56:	2e878793          	addi	a5,a5,744 # 200682e8 <pxReadyTasksLists>
2005db5a:	953e                	c.add	a0,a5
2005db5c:	b02fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005db60:	4505                	c.li	a0,1
2005db62:	b779                	c.j	2005daf0 <xTaskPriorityDisinherit+0x8>

2005db64 <vTaskPriorityDisinheritAfterTimeout>:
2005db64:	cd51                	c.beqz	a0,2005dc00 <vTaskPriorityDisinheritAfterTimeout+0x9c>
2005db66:	1101                	c.addi	sp,-32
2005db68:	cc22                	c.swsp	s0,24(sp)
2005db6a:	ce06                	c.swsp	ra,28(sp)
2005db6c:	ca26                	c.swsp	s1,20(sp)
2005db6e:	4d7c                	c.lw	a5,92(a0)
2005db70:	842a                	c.mv	s0,a0
2005db72:	00b7f363          	bgeu	a5,a1,2005db78 <vTaskPriorityDisinheritAfterTimeout+0x14>
2005db76:	87ae                	c.mv	a5,a1
2005db78:	5458                	c.lw	a4,44(s0)
2005db7a:	06f70e63          	beq	a4,a5,2005dbf6 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005db7e:	5030                	c.lw	a2,96(s0)
2005db80:	4685                	c.li	a3,1
2005db82:	06d61a63          	bne	a2,a3,2005dbf6 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005db86:	4c14                	c.lw	a3,24(s0)
2005db88:	d45c                	c.sw	a5,44(s0)
2005db8a:	0006c663          	blt	a3,zero,2005db96 <vTaskPriorityDisinheritAfterTimeout+0x32>
2005db8e:	46ad                	c.li	a3,11
2005db90:	40f687b3          	sub	a5,a3,a5
2005db94:	cc1c                	c.sw	a5,24(s0)
2005db96:	46d1                	c.li	a3,20
2005db98:	02d70733          	mul	a4,a4,a3
2005db9c:	20068537          	lui	a0,0x20068
2005dba0:	2e850793          	addi	a5,a0,744 # 200682e8 <pxReadyTasksLists>
2005dba4:	2e850493          	addi	s1,a0,744
2005dba8:	973e                	c.add	a4,a5
2005dbaa:	485c                	c.lw	a5,20(s0)
2005dbac:	04e79563          	bne	a5,a4,2005dbf6 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005dbb0:	00440593          	addi	a1,s0,4
2005dbb4:	852e                	c.mv	a0,a1
2005dbb6:	c62e                	c.swsp	a1,12(sp)
2005dbb8:	aeafd0ef          	jal	ra,2005aea2 <uxListRemove>
2005dbbc:	5454                	c.lw	a3,44(s0)
2005dbbe:	45b2                	c.lwsp	a1,12(sp)
2005dbc0:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005dbc4:	e909                	c.bnez	a0,2005dbd6 <vTaskPriorityDisinheritAfterTimeout+0x72>
2005dbc6:	4310                	c.lw	a2,0(a4)
2005dbc8:	4785                	c.li	a5,1
2005dbca:	00d797b3          	sll	a5,a5,a3
2005dbce:	fff7c793          	xori	a5,a5,-1
2005dbd2:	8ff1                	c.and	a5,a2
2005dbd4:	c31c                	c.sw	a5,0(a4)
2005dbd6:	4551                	c.li	a0,20
2005dbd8:	02a68533          	mul	a0,a3,a0
2005dbdc:	4310                	c.lw	a2,0(a4)
2005dbde:	4785                	c.li	a5,1
2005dbe0:	00d797b3          	sll	a5,a5,a3
2005dbe4:	8fd1                	c.or	a5,a2
2005dbe6:	c31c                	c.sw	a5,0(a4)
2005dbe8:	4462                	c.lwsp	s0,24(sp)
2005dbea:	40f2                	c.lwsp	ra,28(sp)
2005dbec:	9526                	c.add	a0,s1
2005dbee:	44d2                	c.lwsp	s1,20(sp)
2005dbf0:	6105                	c.addi16sp	sp,32
2005dbf2:	a6cfd06f          	jal	zero,2005ae5e <vListInsertEnd>
2005dbf6:	40f2                	c.lwsp	ra,28(sp)
2005dbf8:	4462                	c.lwsp	s0,24(sp)
2005dbfa:	44d2                	c.lwsp	s1,20(sp)
2005dbfc:	6105                	c.addi16sp	sp,32
2005dbfe:	8082                	c.jr	ra
2005dc00:	8082                	c.jr	ra

2005dc02 <vTaskEnterCritical>:
2005dc02:	30047073          	csrrci	zero,mstatus,8
2005dc06:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dc0a:	cb81                	c.beqz	a5,2005dc1a <vTaskEnterCritical+0x18>
2005dc0c:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005dc10:	4394                	c.lw	a3,0(a5)
2005dc12:	439c                	c.lw	a5,0(a5)
2005dc14:	4ab8                	c.lw	a4,80(a3)
2005dc16:	0705                	c.addi	a4,1
2005dc18:	cab8                	c.sw	a4,80(a3)
2005dc1a:	8082                	c.jr	ra

2005dc1c <vTaskExitCritical>:
2005dc1c:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dc20:	cf99                	c.beqz	a5,2005dc3e <vTaskExitCritical+0x22>
2005dc22:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005dc26:	4398                	c.lw	a4,0(a5)
2005dc28:	4b38                	c.lw	a4,80(a4)
2005dc2a:	cb11                	c.beqz	a4,2005dc3e <vTaskExitCritical+0x22>
2005dc2c:	4394                	c.lw	a3,0(a5)
2005dc2e:	439c                	c.lw	a5,0(a5)
2005dc30:	4ab8                	c.lw	a4,80(a3)
2005dc32:	177d                	c.addi	a4,-1
2005dc34:	cab8                	c.sw	a4,80(a3)
2005dc36:	4bbc                	c.lw	a5,80(a5)
2005dc38:	e399                	c.bnez	a5,2005dc3e <vTaskExitCritical+0x22>
2005dc3a:	30046073          	csrrsi	zero,mstatus,8
2005dc3e:	8082                	c.jr	ra

2005dc40 <prvAddNewTaskToReadyList>:
2005dc40:	1101                	c.addi	sp,-32
2005dc42:	cc22                	c.swsp	s0,24(sp)
2005dc44:	ca26                	c.swsp	s1,20(sp)
2005dc46:	c84a                	c.swsp	s2,16(sp)
2005dc48:	c64e                	c.swsp	s3,12(sp)
2005dc4a:	ce06                	c.swsp	ra,28(sp)
2005dc4c:	c452                	c.swsp	s4,8(sp)
2005dc4e:	842a                	c.mv	s0,a0
2005dc50:	fb3ff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005dc54:	f7018793          	addi	a5,gp,-144 # 200668f8 <uxCurrentNumberOfTasks>
2005dc58:	4398                	c.lw	a4,0(a5)
2005dc5a:	20068937          	lui	s2,0x20068
2005dc5e:	0705                	c.addi	a4,1
2005dc60:	c398                	c.sw	a4,0(a5)
2005dc62:	f5c18713          	addi	a4,gp,-164 # 200668e4 <pxCurrentTCB>
2005dc66:	4318                	c.lw	a4,0(a4)
2005dc68:	f5c18493          	addi	s1,gp,-164 # 200668e4 <pxCurrentTCB>
2005dc6c:	2e890993          	addi	s3,s2,744 # 200682e8 <pxReadyTasksLists>
2005dc70:	e369                	c.bnez	a4,2005dd32 <prvAddNewTaskToReadyList+0xf2>
2005dc72:	c080                	c.sw	s0,0(s1)
2005dc74:	4398                	c.lw	a4,0(a5)
2005dc76:	4785                	c.li	a5,1
2005dc78:	06f71263          	bne	a4,a5,2005dcdc <prvAddNewTaskToReadyList+0x9c>
2005dc7c:	2e890913          	addi	s2,s2,744
2005dc80:	0dc98a13          	addi	s4,s3,220
2005dc84:	854a                	c.mv	a0,s2
2005dc86:	0951                	c.addi	s2,20
2005dc88:	9bcfd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dc8c:	ff2a1ce3          	bne	s4,s2,2005dc84 <prvAddNewTaskToReadyList+0x44>
2005dc90:	20068537          	lui	a0,0x20068
2005dc94:	3c450a13          	addi	s4,a0,964 # 200683c4 <xDelayedTaskList1>
2005dc98:	3c450513          	addi	a0,a0,964
2005dc9c:	9a8fd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dca0:	20068537          	lui	a0,0x20068
2005dca4:	3d850913          	addi	s2,a0,984 # 200683d8 <xDelayedTaskList2>
2005dca8:	3d850513          	addi	a0,a0,984
2005dcac:	998fd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dcb0:	20068537          	lui	a0,0x20068
2005dcb4:	3ec50513          	addi	a0,a0,1004 # 200683ec <xPendingReadyList>
2005dcb8:	98cfd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dcbc:	20068537          	lui	a0,0x20068
2005dcc0:	41450513          	addi	a0,a0,1044 # 20068414 <xTasksWaitingTermination>
2005dcc4:	980fd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dcc8:	20068537          	lui	a0,0x20068
2005dccc:	40050513          	addi	a0,a0,1024 # 20068400 <xSuspendedTaskList>
2005dcd0:	974fd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dcd4:	f741a023          	sw	s4,-160(gp) # 200668e8 <pxDelayedTaskList>
2005dcd8:	f721a223          	sw	s2,-156(gp) # 200668ec <pxOverflowDelayedTaskList>
2005dcdc:	f7c18713          	addi	a4,gp,-132 # 20066904 <uxTaskNumber>
2005dce0:	431c                	c.lw	a5,0(a4)
2005dce2:	5448                	c.lw	a0,44(s0)
2005dce4:	00440593          	addi	a1,s0,4
2005dce8:	0785                	c.addi	a5,1
2005dcea:	c31c                	c.sw	a5,0(a4)
2005dcec:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005dcf0:	4314                	c.lw	a3,0(a4)
2005dcf2:	c87c                	c.sw	a5,84(s0)
2005dcf4:	4785                	c.li	a5,1
2005dcf6:	00a797b3          	sll	a5,a5,a0
2005dcfa:	8fd5                	c.or	a5,a3
2005dcfc:	c31c                	c.sw	a5,0(a4)
2005dcfe:	47d1                	c.li	a5,20
2005dd00:	02f50533          	mul	a0,a0,a5
2005dd04:	954e                	c.add	a0,s3
2005dd06:	958fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005dd0a:	f13ff0ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005dd0e:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dd12:	cb81                	c.beqz	a5,2005dd22 <prvAddNewTaskToReadyList+0xe2>
2005dd14:	409c                	c.lw	a5,0(s1)
2005dd16:	57d8                	c.lw	a4,44(a5)
2005dd18:	545c                	c.lw	a5,44(s0)
2005dd1a:	00f77463          	bgeu	a4,a5,2005dd22 <prvAddNewTaskToReadyList+0xe2>
2005dd1e:	00000073          	ecall
2005dd22:	40f2                	c.lwsp	ra,28(sp)
2005dd24:	4462                	c.lwsp	s0,24(sp)
2005dd26:	44d2                	c.lwsp	s1,20(sp)
2005dd28:	4942                	c.lwsp	s2,16(sp)
2005dd2a:	49b2                	c.lwsp	s3,12(sp)
2005dd2c:	4a22                	c.lwsp	s4,8(sp)
2005dd2e:	6105                	c.addi16sp	sp,32
2005dd30:	8082                	c.jr	ra
2005dd32:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dd36:	f3dd                	c.bnez	a5,2005dcdc <prvAddNewTaskToReadyList+0x9c>
2005dd38:	409c                	c.lw	a5,0(s1)
2005dd3a:	57d8                	c.lw	a4,44(a5)
2005dd3c:	545c                	c.lw	a5,44(s0)
2005dd3e:	f8e7efe3          	bltu	a5,a4,2005dcdc <prvAddNewTaskToReadyList+0x9c>
2005dd42:	c080                	c.sw	s0,0(s1)
2005dd44:	bf61                	c.j	2005dcdc <prvAddNewTaskToReadyList+0x9c>

2005dd46 <xTaskCreateStatic>:
2005dd46:	1101                	c.addi	sp,-32
2005dd48:	cc22                	c.swsp	s0,24(sp)
2005dd4a:	ce06                	c.swsp	ra,28(sp)
2005dd4c:	8442                	c.mv	s0,a6
2005dd4e:	00080f63          	beq	a6,zero,2005dd6c <xTaskCreateStatic+0x26>
2005dd52:	c395                	c.beqz	a5,2005dd76 <xTaskCreateStatic+0x30>
2005dd54:	02f82823          	sw	a5,48(a6)
2005dd58:	4789                	c.li	a5,2
2005dd5a:	16f800a3          	sb	a5,353(a6)
2005dd5e:	007c                	c.addi4spn	a5,sp,12
2005dd60:	85bff0ef          	jal	ra,2005d5ba <prvInitialiseNewTask.constprop.0>
2005dd64:	8522                	c.mv	a0,s0
2005dd66:	edbff0ef          	jal	ra,2005dc40 <prvAddNewTaskToReadyList>
2005dd6a:	4432                	c.lwsp	s0,12(sp)
2005dd6c:	40f2                	c.lwsp	ra,28(sp)
2005dd6e:	8522                	c.mv	a0,s0
2005dd70:	4462                	c.lwsp	s0,24(sp)
2005dd72:	6105                	c.addi16sp	sp,32
2005dd74:	8082                	c.jr	ra
2005dd76:	4401                	c.li	s0,0
2005dd78:	bfd5                	c.j	2005dd6c <xTaskCreateStatic+0x26>

2005dd7a <vTaskStartScheduler>:
2005dd7a:	1101                	c.addi	sp,-32
2005dd7c:	0070                	c.addi4spn	a2,sp,12
2005dd7e:	002c                	c.addi4spn	a1,sp,8
2005dd80:	0048                	c.addi4spn	a0,sp,4
2005dd82:	ce06                	c.swsp	ra,28(sp)
2005dd84:	c202                	c.swsp	zero,4(sp)
2005dd86:	c402                	c.swsp	zero,8(sp)
2005dd88:	8b9fe0ef          	jal	ra,2005c640 <vApplicationGetIdleTaskMemory>
2005dd8c:	4812                	c.lwsp	a6,4(sp)
2005dd8e:	47a2                	c.lwsp	a5,8(sp)
2005dd90:	4632                	c.lwsp	a2,12(sp)
2005dd92:	200635b7          	lui	a1,0x20063
2005dd96:	2005e537          	lui	a0,0x2005e
2005dd9a:	4701                	c.li	a4,0
2005dd9c:	4681                	c.li	a3,0
2005dd9e:	68458593          	addi	a1,a1,1668 # 20063684 <__func__.1+0x18>
2005dda2:	09e50513          	addi	a0,a0,158 # 2005e09e <prvIdleTask>
2005dda6:	fa1ff0ef          	jal	ra,2005dd46 <xTaskCreateStatic>
2005ddaa:	c51d                	c.beqz	a0,2005ddd8 <vTaskStartScheduler+0x5e>
2005ddac:	76a000ef          	jal	ra,2005e516 <xTimerCreateTimerTask>
2005ddb0:	4785                	c.li	a5,1
2005ddb2:	02f51363          	bne	a0,a5,2005ddd8 <vTaskStartScheduler+0x5e>
2005ddb6:	30047073          	csrrci	zero,mstatus,8
2005ddba:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005ddbe:	06c78793          	addi	a5,a5,108
2005ddc2:	e8f1aa23          	sw	a5,-364(gp) # 2006681c <_impure_ptr>
2005ddc6:	577d                	c.li	a4,-1
2005ddc8:	f8e1a223          	sw	a4,-124(gp) # 2006690c <xNextTaskUnblockTime>
2005ddcc:	f8a1a823          	sw	a0,-112(gp) # 20066918 <xSchedulerRunning>
2005ddd0:	f801aa23          	sw	zero,-108(gp) # 2006691c <xTickCount>
2005ddd4:	fdafe0ef          	jal	ra,2005c5ae <xPortStartScheduler>
2005ddd8:	40f2                	c.lwsp	ra,28(sp)
2005ddda:	e8c1a783          	lw	a5,-372(gp) # 20066814 <uxTopUsedPriority>
2005ddde:	6105                	c.addi16sp	sp,32
2005dde0:	8082                	c.jr	ra

2005dde2 <xTaskCreate>:
2005dde2:	7179                	c.addi16sp	sp,-48
2005dde4:	ce4e                	c.swsp	s3,28(sp)
2005dde6:	89aa                	c.mv	s3,a0
2005dde8:	00261513          	slli	a0,a2,0x2
2005ddec:	d04a                	c.swsp	s2,32(sp)
2005ddee:	cc52                	c.swsp	s4,24(sp)
2005ddf0:	ca56                	c.swsp	s5,20(sp)
2005ddf2:	c85a                	c.swsp	s6,16(sp)
2005ddf4:	c65e                	c.swsp	s7,12(sp)
2005ddf6:	d606                	c.swsp	ra,44(sp)
2005ddf8:	d422                	c.swsp	s0,40(sp)
2005ddfa:	d226                	c.swsp	s1,36(sp)
2005ddfc:	8a2e                	c.mv	s4,a1
2005ddfe:	8932                	c.mv	s2,a2
2005de00:	8ab6                	c.mv	s5,a3
2005de02:	8b3a                	c.mv	s6,a4
2005de04:	8bbe                	c.mv	s7,a5
2005de06:	e05fc0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005de0a:	c531                	c.beqz	a0,2005de56 <xTaskCreate+0x74>
2005de0c:	84aa                	c.mv	s1,a0
2005de0e:	16400513          	addi	a0,zero,356
2005de12:	df9fc0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005de16:	842a                	c.mv	s0,a0
2005de18:	cd05                	c.beqz	a0,2005de50 <xTaskCreate+0x6e>
2005de1a:	882a                	c.mv	a6,a0
2005de1c:	d904                	c.sw	s1,48(a0)
2005de1e:	160500a3          	sb	zero,353(a0)
2005de22:	87de                	c.mv	a5,s7
2005de24:	875a                	c.mv	a4,s6
2005de26:	86d6                	c.mv	a3,s5
2005de28:	864a                	c.mv	a2,s2
2005de2a:	85d2                	c.mv	a1,s4
2005de2c:	854e                	c.mv	a0,s3
2005de2e:	f8cff0ef          	jal	ra,2005d5ba <prvInitialiseNewTask.constprop.0>
2005de32:	8522                	c.mv	a0,s0
2005de34:	e0dff0ef          	jal	ra,2005dc40 <prvAddNewTaskToReadyList>
2005de38:	4505                	c.li	a0,1
2005de3a:	50b2                	c.lwsp	ra,44(sp)
2005de3c:	5422                	c.lwsp	s0,40(sp)
2005de3e:	5492                	c.lwsp	s1,36(sp)
2005de40:	5902                	c.lwsp	s2,32(sp)
2005de42:	49f2                	c.lwsp	s3,28(sp)
2005de44:	4a62                	c.lwsp	s4,24(sp)
2005de46:	4ad2                	c.lwsp	s5,20(sp)
2005de48:	4b42                	c.lwsp	s6,16(sp)
2005de4a:	4bb2                	c.lwsp	s7,12(sp)
2005de4c:	6145                	c.addi16sp	sp,48
2005de4e:	8082                	c.jr	ra
2005de50:	8526                	c.mv	a0,s1
2005de52:	e97fc0ef          	jal	ra,2005ace8 <vPortFree>
2005de56:	557d                	c.li	a0,-1
2005de58:	b7cd                	c.j	2005de3a <xTaskCreate+0x58>

2005de5a <vTaskDelete>:
2005de5a:	1141                	c.addi	sp,-16
2005de5c:	c422                	c.swsp	s0,8(sp)
2005de5e:	c226                	c.swsp	s1,4(sp)
2005de60:	842a                	c.mv	s0,a0
2005de62:	c606                	c.swsp	ra,12(sp)
2005de64:	c04a                	c.swsp	s2,0(sp)
2005de66:	f5c18493          	addi	s1,gp,-164 # 200668e4 <pxCurrentTCB>
2005de6a:	d99ff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005de6e:	e011                	c.bnez	s0,2005de72 <vTaskDelete+0x18>
2005de70:	4080                	c.lw	s0,0(s1)
2005de72:	00440913          	addi	s2,s0,4
2005de76:	854a                	c.mv	a0,s2
2005de78:	82afd0ef          	jal	ra,2005aea2 <uxListRemove>
2005de7c:	e515                	c.bnez	a0,2005dea8 <vTaskDelete+0x4e>
2005de7e:	5450                	c.lw	a2,44(s0)
2005de80:	4751                	c.li	a4,20
2005de82:	02e606b3          	mul	a3,a2,a4
2005de86:	20068737          	lui	a4,0x20068
2005de8a:	2e870713          	addi	a4,a4,744 # 200682e8 <pxReadyTasksLists>
2005de8e:	9736                	c.add	a4,a3
2005de90:	431c                	c.lw	a5,0(a4)
2005de92:	eb99                	c.bnez	a5,2005dea8 <vTaskDelete+0x4e>
2005de94:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005de98:	4314                	c.lw	a3,0(a4)
2005de9a:	4785                	c.li	a5,1
2005de9c:	00c797b3          	sll	a5,a5,a2
2005dea0:	fff7c793          	xori	a5,a5,-1
2005dea4:	8ff5                	c.and	a5,a3
2005dea6:	c31c                	c.sw	a5,0(a4)
2005dea8:	541c                	c.lw	a5,40(s0)
2005deaa:	c789                	c.beqz	a5,2005deb4 <vTaskDelete+0x5a>
2005deac:	01840513          	addi	a0,s0,24
2005deb0:	ff3fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005deb4:	f7c18793          	addi	a5,gp,-132 # 20066904 <uxTaskNumber>
2005deb8:	4398                	c.lw	a4,0(a5)
2005deba:	0705                	c.addi	a4,1
2005debc:	c398                	c.sw	a4,0(a5)
2005debe:	409c                	c.lw	a5,0(s1)
2005dec0:	02879e63          	bne	a5,s0,2005defc <vTaskDelete+0xa2>
2005dec4:	20068537          	lui	a0,0x20068
2005dec8:	85ca                	c.mv	a1,s2
2005deca:	41450513          	addi	a0,a0,1044 # 20068414 <xTasksWaitingTermination>
2005dece:	f91fc0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005ded2:	f7418793          	addi	a5,gp,-140 # 200668fc <uxDeletedTasksWaitingCleanUp>
2005ded6:	4398                	c.lw	a4,0(a5)
2005ded8:	0705                	c.addi	a4,1
2005deda:	c398                	c.sw	a4,0(a5)
2005dedc:	d41ff0ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005dee0:	f901a783          	lw	a5,-112(gp) # 20066918 <xSchedulerRunning>
2005dee4:	c791                	c.beqz	a5,2005def0 <vTaskDelete+0x96>
2005dee6:	409c                	c.lw	a5,0(s1)
2005dee8:	00879463          	bne	a5,s0,2005def0 <vTaskDelete+0x96>
2005deec:	00000073          	ecall
2005def0:	40b2                	c.lwsp	ra,12(sp)
2005def2:	4422                	c.lwsp	s0,8(sp)
2005def4:	4492                	c.lwsp	s1,4(sp)
2005def6:	4902                	c.lwsp	s2,0(sp)
2005def8:	0141                	c.addi	sp,16
2005defa:	8082                	c.jr	ra
2005defc:	f7018793          	addi	a5,gp,-144 # 200668f8 <uxCurrentNumberOfTasks>
2005df00:	4398                	c.lw	a4,0(a5)
2005df02:	8522                	c.mv	a0,s0
2005df04:	177d                	c.addi	a4,-1
2005df06:	c398                	c.sw	a4,0(a5)
2005df08:	e7cff0ef          	jal	ra,2005d584 <prvDeleteTCB>
2005df0c:	e5cff0ef          	jal	ra,2005d568 <prvResetNextTaskUnblockTime>
2005df10:	b7f1                	c.j	2005dedc <vTaskDelete+0x82>

2005df12 <eTaskGetState>:
2005df12:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005df16:	06a78863          	beq	a5,a0,2005df86 <eTaskGetState+0x74>
2005df1a:	1101                	c.addi	sp,-32
2005df1c:	ce06                	c.swsp	ra,28(sp)
2005df1e:	cc22                	c.swsp	s0,24(sp)
2005df20:	ca26                	c.swsp	s1,20(sp)
2005df22:	842a                	c.mv	s0,a0
2005df24:	c84a                	c.swsp	s2,16(sp)
2005df26:	c64e                	c.swsp	s3,12(sp)
2005df28:	cdbff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005df2c:	f601a983          	lw	s3,-160(gp) # 200668e8 <pxDelayedTaskList>
2005df30:	4844                	c.lw	s1,20(s0)
2005df32:	f641a903          	lw	s2,-156(gp) # 200668ec <pxOverflowDelayedTaskList>
2005df36:	ce7ff0ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005df3a:	4509                	c.li	a0,2
2005df3c:	03348463          	beq	s1,s3,2005df64 <eTaskGetState+0x52>
2005df40:	03248263          	beq	s1,s2,2005df64 <eTaskGetState+0x52>
2005df44:	200687b7          	lui	a5,0x20068
2005df48:	40078793          	addi	a5,a5,1024 # 20068400 <xSuspendedTaskList>
2005df4c:	02f49363          	bne	s1,a5,2005df72 <eTaskGetState+0x60>
2005df50:	541c                	c.lw	a5,40(s0)
2005df52:	eb89                	c.bnez	a5,2005df64 <eTaskGetState+0x52>
2005df54:	16044783          	lbu	a5,352(s0)
2005df58:	4705                	c.li	a4,1
2005df5a:	0ff7f793          	andi	a5,a5,255
2005df5e:	00e78363          	beq	a5,a4,2005df64 <eTaskGetState+0x52>
2005df62:	450d                	c.li	a0,3
2005df64:	40f2                	c.lwsp	ra,28(sp)
2005df66:	4462                	c.lwsp	s0,24(sp)
2005df68:	44d2                	c.lwsp	s1,20(sp)
2005df6a:	4942                	c.lwsp	s2,16(sp)
2005df6c:	49b2                	c.lwsp	s3,12(sp)
2005df6e:	6105                	c.addi16sp	sp,32
2005df70:	8082                	c.jr	ra
2005df72:	200687b7          	lui	a5,0x20068
2005df76:	41478793          	addi	a5,a5,1044 # 20068414 <xTasksWaitingTermination>
2005df7a:	4511                	c.li	a0,4
2005df7c:	fef484e3          	beq	s1,a5,2005df64 <eTaskGetState+0x52>
2005df80:	d0f5                	c.beqz	s1,2005df64 <eTaskGetState+0x52>
2005df82:	4505                	c.li	a0,1
2005df84:	b7c5                	c.j	2005df64 <eTaskGetState+0x52>
2005df86:	4501                	c.li	a0,0
2005df88:	8082                	c.jr	ra

2005df8a <xTaskResumeAll>:
2005df8a:	7139                	c.addi16sp	sp,-64
2005df8c:	de06                	c.swsp	ra,60(sp)
2005df8e:	dc22                	c.swsp	s0,56(sp)
2005df90:	da26                	c.swsp	s1,52(sp)
2005df92:	d84a                	c.swsp	s2,48(sp)
2005df94:	d64e                	c.swsp	s3,44(sp)
2005df96:	d452                	c.swsp	s4,40(sp)
2005df98:	d256                	c.swsp	s5,36(sp)
2005df9a:	d05a                	c.swsp	s6,32(sp)
2005df9c:	ce5e                	c.swsp	s7,28(sp)
2005df9e:	c65ff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005dfa2:	f7818793          	addi	a5,gp,-136 # 20066900 <uxSchedulerSuspended>
2005dfa6:	4398                	c.lw	a4,0(a5)
2005dfa8:	177d                	c.addi	a4,-1
2005dfaa:	c398                	c.sw	a4,0(a5)
2005dfac:	439c                	c.lw	a5,0(a5)
2005dfae:	c38d                	c.beqz	a5,2005dfd0 <xTaskResumeAll+0x46>
2005dfb0:	4501                	c.li	a0,0
2005dfb2:	c62a                	c.swsp	a0,12(sp)
2005dfb4:	c69ff0ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005dfb8:	50f2                	c.lwsp	ra,60(sp)
2005dfba:	5462                	c.lwsp	s0,56(sp)
2005dfbc:	4532                	c.lwsp	a0,12(sp)
2005dfbe:	54d2                	c.lwsp	s1,52(sp)
2005dfc0:	5942                	c.lwsp	s2,48(sp)
2005dfc2:	59b2                	c.lwsp	s3,44(sp)
2005dfc4:	5a22                	c.lwsp	s4,40(sp)
2005dfc6:	5a92                	c.lwsp	s5,36(sp)
2005dfc8:	5b02                	c.lwsp	s6,32(sp)
2005dfca:	4bf2                	c.lwsp	s7,28(sp)
2005dfcc:	6121                	c.addi16sp	sp,64
2005dfce:	8082                	c.jr	ra
2005dfd0:	f701a783          	lw	a5,-144(gp) # 200668f8 <uxCurrentNumberOfTasks>
2005dfd4:	dff1                	c.beqz	a5,2005dfb0 <xTaskResumeAll+0x26>
2005dfd6:	200684b7          	lui	s1,0x20068
2005dfda:	20068937          	lui	s2,0x20068
2005dfde:	4401                	c.li	s0,0
2005dfe0:	3ec48493          	addi	s1,s1,1004 # 200683ec <xPendingReadyList>
2005dfe4:	4985                	c.li	s3,1
2005dfe6:	2e890913          	addi	s2,s2,744 # 200682e8 <pxReadyTasksLists>
2005dfea:	4ad1                	c.li	s5,20
2005dfec:	a0b1                	c.j	2005e038 <xTaskResumeAll+0xae>
2005dfee:	44dc                	c.lw	a5,12(s1)
2005dff0:	47c0                	c.lw	s0,12(a5)
2005dff2:	01840513          	addi	a0,s0,24
2005dff6:	eadfc0ef          	jal	ra,2005aea2 <uxListRemove>
2005dffa:	00440593          	addi	a1,s0,4
2005dffe:	852e                	c.mv	a0,a1
2005e000:	c62e                	c.swsp	a1,12(sp)
2005e002:	ea1fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e006:	5448                	c.lw	a0,44(s0)
2005e008:	f8018713          	addi	a4,gp,-128 # 20066908 <uxTopReadyPriority>
2005e00c:	4314                	c.lw	a3,0(a4)
2005e00e:	00a997b3          	sll	a5,s3,a0
2005e012:	03550533          	mul	a0,a0,s5
2005e016:	45b2                	c.lwsp	a1,12(sp)
2005e018:	8fd5                	c.or	a5,a3
2005e01a:	c31c                	c.sw	a5,0(a4)
2005e01c:	954a                	c.add	a0,s2
2005e01e:	e41fc0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005e022:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005e026:	439c                	c.lw	a5,0(a5)
2005e028:	5458                	c.lw	a4,44(s0)
2005e02a:	57dc                	c.lw	a5,44(a5)
2005e02c:	00f76663          	bltu	a4,a5,2005e038 <xTaskResumeAll+0xae>
2005e030:	f9818793          	addi	a5,gp,-104 # 20066920 <xYieldPending>
2005e034:	0137a023          	sw	s3,0(a5)
2005e038:	409c                	c.lw	a5,0(s1)
2005e03a:	fbd5                	c.bnez	a5,2005dfee <xTaskResumeAll+0x64>
2005e03c:	c019                	c.beqz	s0,2005e042 <xTaskResumeAll+0xb8>
2005e03e:	d2aff0ef          	jal	ra,2005d568 <prvResetNextTaskUnblockTime>
2005e042:	f8c18713          	addi	a4,gp,-116 # 20066914 <xPendedTicks>
2005e046:	4304                	c.lw	s1,0(a4)
2005e048:	f8c18413          	addi	s0,gp,-116 # 20066914 <xPendedTicks>
2005e04c:	cc89                	c.beqz	s1,2005e066 <xTaskResumeAll+0xdc>
2005e04e:	4985                	c.li	s3,1
2005e050:	eb0ff0ef          	jal	ra,2005d700 <xTaskIncrementTick>
2005e054:	c509                	c.beqz	a0,2005e05e <xTaskResumeAll+0xd4>
2005e056:	f9818793          	addi	a5,gp,-104 # 20066920 <xYieldPending>
2005e05a:	0137a023          	sw	s3,0(a5)
2005e05e:	14fd                	c.addi	s1,-1
2005e060:	f8e5                	c.bnez	s1,2005e050 <xTaskResumeAll+0xc6>
2005e062:	00042023          	sw	zero,0(s0)
2005e066:	f981a783          	lw	a5,-104(gp) # 20066920 <xYieldPending>
2005e06a:	d3b9                	c.beqz	a5,2005dfb0 <xTaskResumeAll+0x26>
2005e06c:	00000073          	ecall
2005e070:	4505                	c.li	a0,1
2005e072:	b781                	c.j	2005dfb2 <xTaskResumeAll+0x28>

2005e074 <vTaskDelay>:
2005e074:	e501                	c.bnez	a0,2005e07c <vTaskDelay+0x8>
2005e076:	00000073          	ecall
2005e07a:	8082                	c.jr	ra
2005e07c:	1101                	c.addi	sp,-32
2005e07e:	ce06                	c.swsp	ra,28(sp)
2005e080:	c62a                	c.swsp	a0,12(sp)
2005e082:	e5aff0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e086:	4532                	c.lwsp	a0,12(sp)
2005e088:	4581                	c.li	a1,0
2005e08a:	bbaff0ef          	jal	ra,2005d444 <prvAddCurrentTaskToDelayedList>
2005e08e:	efdff0ef          	jal	ra,2005df8a <xTaskResumeAll>
2005e092:	e119                	c.bnez	a0,2005e098 <vTaskDelay+0x24>
2005e094:	00000073          	ecall
2005e098:	40f2                	c.lwsp	ra,28(sp)
2005e09a:	6105                	c.addi16sp	sp,32
2005e09c:	8082                	c.jr	ra

2005e09e <prvIdleTask>:
2005e09e:	1101                	c.addi	sp,-32
2005e0a0:	ca26                	c.swsp	s1,20(sp)
2005e0a2:	200684b7          	lui	s1,0x20068
2005e0a6:	cc22                	c.swsp	s0,24(sp)
2005e0a8:	c452                	c.swsp	s4,8(sp)
2005e0aa:	c256                	c.swsp	s5,4(sp)
2005e0ac:	c05a                	c.swsp	s6,0(sp)
2005e0ae:	ce06                	c.swsp	ra,28(sp)
2005e0b0:	c84a                	c.swsp	s2,16(sp)
2005e0b2:	c64e                	c.swsp	s3,12(sp)
2005e0b4:	41448493          	addi	s1,s1,1044 # 20068414 <xTasksWaitingTermination>
2005e0b8:	20068ab7          	lui	s5,0x20068
2005e0bc:	4405                	c.li	s0,1
2005e0be:	f7418913          	addi	s2,gp,-140 # 200668fc <uxDeletedTasksWaitingCleanUp>
2005e0c2:	00092783          	lw	a5,0(s2)
2005e0c6:	eb8d                	c.bnez	a5,2005e0f8 <prvIdleTask+0x5a>
2005e0c8:	2e8a8793          	addi	a5,s5,744 # 200682e8 <pxReadyTasksLists>
2005e0cc:	439c                	c.lw	a5,0(a5)
2005e0ce:	00f47463          	bgeu	s0,a5,2005e0d6 <prvIdleTask+0x38>
2005e0d2:	00000073          	ecall
2005e0d6:	cf8fe0ef          	jal	ra,2005c5ce <vApplicationIdleHook>
2005e0da:	c60ff0ef          	jal	ra,2005d53a <prvGetExpectedIdleTime>
2005e0de:	fea470e3          	bgeu	s0,a0,2005e0be <prvIdleTask+0x20>
2005e0e2:	dfaff0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e0e6:	c54ff0ef          	jal	ra,2005d53a <prvGetExpectedIdleTime>
2005e0ea:	00a47463          	bgeu	s0,a0,2005e0f2 <prvIdleTask+0x54>
2005e0ee:	d8afe0ef          	jal	ra,2005c678 <vPortSuppressTicksAndSleep>
2005e0f2:	e99ff0ef          	jal	ra,2005df8a <xTaskResumeAll>
2005e0f6:	b7e1                	c.j	2005e0be <prvIdleTask+0x20>
2005e0f8:	b0bff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005e0fc:	44dc                	c.lw	a5,12(s1)
2005e0fe:	00c7a983          	lw	s3,12(a5)
2005e102:	00498513          	addi	a0,s3,4
2005e106:	d9dfc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e10a:	f7018713          	addi	a4,gp,-144 # 200668f8 <uxCurrentNumberOfTasks>
2005e10e:	431c                	c.lw	a5,0(a4)
2005e110:	17fd                	c.addi	a5,-1
2005e112:	c31c                	c.sw	a5,0(a4)
2005e114:	00092783          	lw	a5,0(s2)
2005e118:	17fd                	c.addi	a5,-1
2005e11a:	00f92023          	sw	a5,0(s2)
2005e11e:	affff0ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005e122:	854e                	c.mv	a0,s3
2005e124:	c60ff0ef          	jal	ra,2005d584 <prvDeleteTCB>
2005e128:	bf59                	c.j	2005e0be <prvIdleTask+0x20>

2005e12a <vTaskGetInfo>:
2005e12a:	1141                	c.addi	sp,-16
2005e12c:	c422                	c.swsp	s0,8(sp)
2005e12e:	c226                	c.swsp	s1,4(sp)
2005e130:	c04a                	c.swsp	s2,0(sp)
2005e132:	c606                	c.swsp	ra,12(sp)
2005e134:	842e                	c.mv	s0,a1
2005e136:	8932                	c.mv	s2,a2
2005e138:	84aa                	c.mv	s1,a0
2005e13a:	e119                	c.bnez	a0,2005e140 <vTaskGetInfo+0x16>
2005e13c:	f5c1a483          	lw	s1,-164(gp) # 200668e4 <pxCurrentTCB>
2005e140:	03448793          	addi	a5,s1,52
2005e144:	c004                	c.sw	s1,0(s0)
2005e146:	c05c                	c.sw	a5,4(s0)
2005e148:	54dc                	c.lw	a5,44(s1)
2005e14a:	c81c                	c.sw	a5,16(s0)
2005e14c:	589c                	c.lw	a5,48(s1)
2005e14e:	cc5c                	c.sw	a5,28(s0)
2005e150:	48fc                	c.lw	a5,84(s1)
2005e152:	c41c                	c.sw	a5,8(s0)
2005e154:	4cfc                	c.lw	a5,92(s1)
2005e156:	c85c                	c.sw	a5,20(s0)
2005e158:	54bc                	c.lw	a5,104(s1)
2005e15a:	cc1c                	c.sw	a5,24(s0)
2005e15c:	4795                	c.li	a5,5
2005e15e:	04f68c63          	beq	a3,a5,2005e1b6 <vTaskGetInfo+0x8c>
2005e162:	f5c1a783          	lw	a5,-164(gp) # 200668e4 <pxCurrentTCB>
2005e166:	02979963          	bne	a5,s1,2005e198 <vTaskGetInfo+0x6e>
2005e16a:	00040623          	sb	zero,12(s0)
2005e16e:	04090c63          	beq	s2,zero,2005e1c6 <vTaskGetInfo+0x9c>
2005e172:	5890                	c.lw	a2,48(s1)
2005e174:	4781                	c.li	a5,0
2005e176:	0a500713          	addi	a4,zero,165
2005e17a:	00f606b3          	add	a3,a2,a5
2005e17e:	0006c683          	lbu	a3,0(a3)
2005e182:	04e68063          	beq	a3,a4,2005e1c2 <vTaskGetInfo+0x98>
2005e186:	8389                	c.srli	a5,0x2
2005e188:	02f41023          	sh	a5,32(s0)
2005e18c:	40b2                	c.lwsp	ra,12(sp)
2005e18e:	4422                	c.lwsp	s0,8(sp)
2005e190:	4492                	c.lwsp	s1,4(sp)
2005e192:	4902                	c.lwsp	s2,0(sp)
2005e194:	0141                	c.addi	sp,16
2005e196:	8082                	c.jr	ra
2005e198:	00d40623          	sb	a3,12(s0)
2005e19c:	478d                	c.li	a5,3
2005e19e:	fcf698e3          	bne	a3,a5,2005e16e <vTaskGetInfo+0x44>
2005e1a2:	d3aff0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e1a6:	549c                	c.lw	a5,40(s1)
2005e1a8:	c781                	c.beqz	a5,2005e1b0 <vTaskGetInfo+0x86>
2005e1aa:	4789                	c.li	a5,2
2005e1ac:	00f40623          	sb	a5,12(s0)
2005e1b0:	ddbff0ef          	jal	ra,2005df8a <xTaskResumeAll>
2005e1b4:	bf6d                	c.j	2005e16e <vTaskGetInfo+0x44>
2005e1b6:	8526                	c.mv	a0,s1
2005e1b8:	d5bff0ef          	jal	ra,2005df12 <eTaskGetState>
2005e1bc:	00a40623          	sb	a0,12(s0)
2005e1c0:	b77d                	c.j	2005e16e <vTaskGetInfo+0x44>
2005e1c2:	0785                	c.addi	a5,1
2005e1c4:	bf5d                	c.j	2005e17a <vTaskGetInfo+0x50>
2005e1c6:	02041023          	sh	zero,32(s0)
2005e1ca:	b7c9                	c.j	2005e18c <vTaskGetInfo+0x62>

2005e1cc <prvListTasksWithinSingleList>:
2005e1cc:	7179                	c.addi16sp	sp,-48
2005e1ce:	d606                	c.swsp	ra,44(sp)
2005e1d0:	d422                	c.swsp	s0,40(sp)
2005e1d2:	d226                	c.swsp	s1,36(sp)
2005e1d4:	d04a                	c.swsp	s2,32(sp)
2005e1d6:	ce4e                	c.swsp	s3,28(sp)
2005e1d8:	cc52                	c.swsp	s4,24(sp)
2005e1da:	ca56                	c.swsp	s5,20(sp)
2005e1dc:	c85a                	c.swsp	s6,16(sp)
2005e1de:	c65e                	c.swsp	s7,12(sp)
2005e1e0:	4184                	c.lw	s1,0(a1)
2005e1e2:	c8a9                	c.beqz	s1,2005e234 <prvListTasksWithinSingleList+0x68>
2005e1e4:	41dc                	c.lw	a5,4(a1)
2005e1e6:	00858a13          	addi	s4,a1,8
2005e1ea:	892a                	c.mv	s2,a0
2005e1ec:	43dc                	c.lw	a5,4(a5)
2005e1ee:	842e                	c.mv	s0,a1
2005e1f0:	89b2                	c.mv	s3,a2
2005e1f2:	c1dc                	c.sw	a5,4(a1)
2005e1f4:	01479463          	bne	a5,s4,2005e1fc <prvListTasksWithinSingleList+0x30>
2005e1f8:	45dc                	c.lw	a5,12(a1)
2005e1fa:	c1dc                	c.sw	a5,4(a1)
2005e1fc:	405c                	c.lw	a5,4(s0)
2005e1fe:	4481                	c.li	s1,0
2005e200:	02400b93          	addi	s7,zero,36
2005e204:	00c7ab03          	lw	s6,12(a5)
2005e208:	405c                	c.lw	a5,4(s0)
2005e20a:	43dc                	c.lw	a5,4(a5)
2005e20c:	c05c                	c.sw	a5,4(s0)
2005e20e:	00fa1563          	bne	s4,a5,2005e218 <prvListTasksWithinSingleList+0x4c>
2005e212:	004a2783          	lw	a5,4(s4)
2005e216:	c05c                	c.sw	a5,4(s0)
2005e218:	037485b3          	mul	a1,s1,s7
2005e21c:	405c                	c.lw	a5,4(s0)
2005e21e:	86ce                	c.mv	a3,s3
2005e220:	4605                	c.li	a2,1
2005e222:	00c7aa83          	lw	s5,12(a5)
2005e226:	0485                	c.addi	s1,1
2005e228:	8556                	c.mv	a0,s5
2005e22a:	95ca                	c.add	a1,s2
2005e22c:	effff0ef          	jal	ra,2005e12a <vTaskGetInfo>
2005e230:	fd5b1ce3          	bne	s6,s5,2005e208 <prvListTasksWithinSingleList+0x3c>
2005e234:	50b2                	c.lwsp	ra,44(sp)
2005e236:	5422                	c.lwsp	s0,40(sp)
2005e238:	5902                	c.lwsp	s2,32(sp)
2005e23a:	49f2                	c.lwsp	s3,28(sp)
2005e23c:	4a62                	c.lwsp	s4,24(sp)
2005e23e:	4ad2                	c.lwsp	s5,20(sp)
2005e240:	4b42                	c.lwsp	s6,16(sp)
2005e242:	4bb2                	c.lwsp	s7,12(sp)
2005e244:	8526                	c.mv	a0,s1
2005e246:	5492                	c.lwsp	s1,36(sp)
2005e248:	6145                	c.addi16sp	sp,48
2005e24a:	8082                	c.jr	ra

2005e24c <uxTaskGetSystemState>:
2005e24c:	1101                	c.addi	sp,-32
2005e24e:	cc22                	c.swsp	s0,24(sp)
2005e250:	c84a                	c.swsp	s2,16(sp)
2005e252:	c64e                	c.swsp	s3,12(sp)
2005e254:	ce06                	c.swsp	ra,28(sp)
2005e256:	ca26                	c.swsp	s1,20(sp)
2005e258:	c452                	c.swsp	s4,8(sp)
2005e25a:	c256                	c.swsp	s5,4(sp)
2005e25c:	c05a                	c.swsp	s6,0(sp)
2005e25e:	892a                	c.mv	s2,a0
2005e260:	842e                	c.mv	s0,a1
2005e262:	89b2                	c.mv	s3,a2
2005e264:	c78ff0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e268:	f701a783          	lw	a5,-144(gp) # 200668f8 <uxCurrentNumberOfTasks>
2005e26c:	0af46563          	bltu	s0,a5,2005e316 <uxTaskGetSystemState+0xca>
2005e270:	20068ab7          	lui	s5,0x20068
2005e274:	4a2d                	c.li	s4,11
2005e276:	4401                	c.li	s0,0
2005e278:	4b51                	c.li	s6,20
2005e27a:	2e8a8a93          	addi	s5,s5,744 # 200682e8 <pxReadyTasksLists>
2005e27e:	02400493          	addi	s1,zero,36
2005e282:	1a7d                	c.addi	s4,-1
2005e284:	02940533          	mul	a0,s0,s1
2005e288:	4605                	c.li	a2,1
2005e28a:	036a05b3          	mul	a1,s4,s6
2005e28e:	954a                	c.add	a0,s2
2005e290:	95d6                	c.add	a1,s5
2005e292:	f3bff0ef          	jal	ra,2005e1cc <prvListTasksWithinSingleList>
2005e296:	942a                	c.add	s0,a0
2005e298:	fe0a15e3          	bne	s4,zero,2005e282 <uxTaskGetSystemState+0x36>
2005e29c:	02940533          	mul	a0,s0,s1
2005e2a0:	f601a583          	lw	a1,-160(gp) # 200668e8 <pxDelayedTaskList>
2005e2a4:	4609                	c.li	a2,2
2005e2a6:	954a                	c.add	a0,s2
2005e2a8:	f25ff0ef          	jal	ra,2005e1cc <prvListTasksWithinSingleList>
2005e2ac:	942a                	c.add	s0,a0
2005e2ae:	02940533          	mul	a0,s0,s1
2005e2b2:	f641a583          	lw	a1,-156(gp) # 200668ec <pxOverflowDelayedTaskList>
2005e2b6:	4609                	c.li	a2,2
2005e2b8:	954a                	c.add	a0,s2
2005e2ba:	f13ff0ef          	jal	ra,2005e1cc <prvListTasksWithinSingleList>
2005e2be:	942a                	c.add	s0,a0
2005e2c0:	02940533          	mul	a0,s0,s1
2005e2c4:	200685b7          	lui	a1,0x20068
2005e2c8:	4611                	c.li	a2,4
2005e2ca:	41458593          	addi	a1,a1,1044 # 20068414 <xTasksWaitingTermination>
2005e2ce:	954a                	c.add	a0,s2
2005e2d0:	efdff0ef          	jal	ra,2005e1cc <prvListTasksWithinSingleList>
2005e2d4:	942a                	c.add	s0,a0
2005e2d6:	029404b3          	mul	s1,s0,s1
2005e2da:	200685b7          	lui	a1,0x20068
2005e2de:	460d                	c.li	a2,3
2005e2e0:	40058593          	addi	a1,a1,1024 # 20068400 <xSuspendedTaskList>
2005e2e4:	00990533          	add	a0,s2,s1
2005e2e8:	ee5ff0ef          	jal	ra,2005e1cc <prvListTasksWithinSingleList>
2005e2ec:	942a                	c.add	s0,a0
2005e2ee:	00098763          	beq	s3,zero,2005e2fc <uxTaskGetSystemState+0xb0>
2005e2f2:	410187b7          	lui	a5,0x41018
2005e2f6:	4bdc                	c.lw	a5,20(a5)
2005e2f8:	00f9a023          	sw	a5,0(s3)
2005e2fc:	c8fff0ef          	jal	ra,2005df8a <xTaskResumeAll>
2005e300:	40f2                	c.lwsp	ra,28(sp)
2005e302:	8522                	c.mv	a0,s0
2005e304:	4462                	c.lwsp	s0,24(sp)
2005e306:	44d2                	c.lwsp	s1,20(sp)
2005e308:	4942                	c.lwsp	s2,16(sp)
2005e30a:	49b2                	c.lwsp	s3,12(sp)
2005e30c:	4a22                	c.lwsp	s4,8(sp)
2005e30e:	4a92                	c.lwsp	s5,4(sp)
2005e310:	4b02                	c.lwsp	s6,0(sp)
2005e312:	6105                	c.addi16sp	sp,32
2005e314:	8082                	c.jr	ra
2005e316:	4401                	c.li	s0,0
2005e318:	b7d5                	c.j	2005e2fc <uxTaskGetSystemState+0xb0>

2005e31a <xTaskGetTickCount>:
2005e31a:	1101                	c.addi	sp,-32
2005e31c:	ce06                	c.swsp	ra,28(sp)
2005e31e:	8e5ff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005e322:	f941a503          	lw	a0,-108(gp) # 2006691c <xTickCount>
2005e326:	c62a                	c.swsp	a0,12(sp)
2005e328:	8f5ff0ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005e32c:	40f2                	c.lwsp	ra,28(sp)
2005e32e:	4532                	c.lwsp	a0,12(sp)
2005e330:	6105                	c.addi16sp	sp,32
2005e332:	8082                	c.jr	ra

2005e334 <xTaskCheckForTimeOut>:
2005e334:	1101                	c.addi	sp,-32
2005e336:	cc22                	c.swsp	s0,24(sp)
2005e338:	ce06                	c.swsp	ra,28(sp)
2005e33a:	842e                	c.mv	s0,a1
2005e33c:	c62a                	c.swsp	a0,12(sp)
2005e33e:	8c5ff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005e342:	f941a683          	lw	a3,-108(gp) # 2006691c <xTickCount>
2005e346:	401c                	c.lw	a5,0(s0)
2005e348:	577d                	c.li	a4,-1
2005e34a:	02e78f63          	beq	a5,a4,2005e388 <xTaskCheckForTimeOut+0x54>
2005e34e:	4532                	c.lwsp	a0,12(sp)
2005e350:	f881a603          	lw	a2,-120(gp) # 20066910 <xNumOfOverflows>
2005e354:	410c                	c.lw	a1,0(a0)
2005e356:	4158                	c.lw	a4,4(a0)
2005e358:	00c58f63          	beq	a1,a2,2005e376 <xTaskCheckForTimeOut+0x42>
2005e35c:	00e6ed63          	bltu	a3,a4,2005e376 <xTaskCheckForTimeOut+0x42>
2005e360:	00042023          	sw	zero,0(s0)
2005e364:	4505                	c.li	a0,1
2005e366:	c62a                	c.swsp	a0,12(sp)
2005e368:	8b5ff0ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005e36c:	40f2                	c.lwsp	ra,28(sp)
2005e36e:	4462                	c.lwsp	s0,24(sp)
2005e370:	4532                	c.lwsp	a0,12(sp)
2005e372:	6105                	c.addi16sp	sp,32
2005e374:	8082                	c.jr	ra
2005e376:	40e68633          	sub	a2,a3,a4
2005e37a:	fef673e3          	bgeu	a2,a5,2005e360 <xTaskCheckForTimeOut+0x2c>
2005e37e:	8f95                	c.sub	a5,a3
2005e380:	97ba                	c.add	a5,a4
2005e382:	c01c                	c.sw	a5,0(s0)
2005e384:	e50ff0ef          	jal	ra,2005d9d4 <vTaskInternalSetTimeOutState>
2005e388:	4501                	c.li	a0,0
2005e38a:	bff1                	c.j	2005e366 <xTaskCheckForTimeOut+0x32>

2005e38c <vTaskList>:
2005e38c:	7139                	c.addi16sp	sp,-64
2005e38e:	dc22                	c.swsp	s0,56(sp)
2005e390:	de06                	c.swsp	ra,60(sp)
2005e392:	da26                	c.swsp	s1,52(sp)
2005e394:	d84a                	c.swsp	s2,48(sp)
2005e396:	d64e                	c.swsp	s3,44(sp)
2005e398:	d452                	c.swsp	s4,40(sp)
2005e39a:	d256                	c.swsp	s5,36(sp)
2005e39c:	d05a                	c.swsp	s6,32(sp)
2005e39e:	ce5e                	c.swsp	s7,28(sp)
2005e3a0:	f7018793          	addi	a5,gp,-144 # 200668f8 <uxCurrentNumberOfTasks>
2005e3a4:	438c                	c.lw	a1,0(a5)
2005e3a6:	842a                	c.mv	s0,a0
2005e3a8:	00050023          	sb	zero,0(a0)
2005e3ac:	4388                	c.lw	a0,0(a5)
2005e3ae:	02400793          	addi	a5,zero,36
2005e3b2:	c62e                	c.swsp	a1,12(sp)
2005e3b4:	02f50533          	mul	a0,a0,a5
2005e3b8:	853fc0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005e3bc:	c151                	c.beqz	a0,2005e440 <vTaskList+0xb4>
2005e3be:	45b2                	c.lwsp	a1,12(sp)
2005e3c0:	4601                	c.li	a2,0
2005e3c2:	892a                	c.mv	s2,a0
2005e3c4:	e89ff0ef          	jal	ra,2005e24c <uxTaskGetSystemState>
2005e3c8:	89aa                	c.mv	s3,a0
2005e3ca:	00490493          	addi	s1,s2,4
2005e3ce:	4a01                	c.li	s4,0
2005e3d0:	4b91                	c.li	s7,4
2005e3d2:	e6018a93          	addi	s5,gp,-416 # 200667e8 <CSWTCH.248>
2005e3d6:	20063b37          	lui	s6,0x20063
2005e3da:	013a1f63          	bne	s4,s3,2005e3f8 <vTaskList+0x6c>
2005e3de:	5462                	c.lwsp	s0,56(sp)
2005e3e0:	50f2                	c.lwsp	ra,60(sp)
2005e3e2:	54d2                	c.lwsp	s1,52(sp)
2005e3e4:	59b2                	c.lwsp	s3,44(sp)
2005e3e6:	5a22                	c.lwsp	s4,40(sp)
2005e3e8:	5a92                	c.lwsp	s5,36(sp)
2005e3ea:	5b02                	c.lwsp	s6,32(sp)
2005e3ec:	4bf2                	c.lwsp	s7,28(sp)
2005e3ee:	854a                	c.mv	a0,s2
2005e3f0:	5942                	c.lwsp	s2,48(sp)
2005e3f2:	6121                	c.addi16sp	sp,64
2005e3f4:	8f5fc06f          	jal	zero,2005ace8 <vPortFree>
2005e3f8:	0084c783          	lbu	a5,8(s1)
2005e3fc:	4681                	c.li	a3,0
2005e3fe:	00fbe563          	bltu	s7,a5,2005e408 <vTaskList+0x7c>
2005e402:	97d6                	c.add	a5,s5
2005e404:	0007c683          	lbu	a3,0(a5) # 41018000 <__km4_bd_boot_download_addr__+0x11006000>
2005e408:	408c                	c.lw	a1,0(s1)
2005e40a:	8522                	c.mv	a0,s0
2005e40c:	c636                	c.swsp	a3,12(sp)
2005e40e:	8e2ff0ef          	jal	ra,2005d4f0 <prvWriteNameToBuffer>
2005e412:	0044a803          	lw	a6,4(s1)
2005e416:	01c4d783          	lhu	a5,28(s1)
2005e41a:	44d8                	c.lw	a4,12(s1)
2005e41c:	46b2                	c.lwsp	a3,12(sp)
2005e41e:	68cb0613          	addi	a2,s6,1676 # 2006368c <__func__.1+0x20>
2005e422:	40000593          	addi	a1,zero,1024
2005e426:	842a                	c.mv	s0,a0
2005e428:	124010ef          	jal	ra,2005f54c <DiagSnPrintf>
2005e42c:	8522                	c.mv	a0,s0
2005e42e:	dffab097          	auipc	ra,0xdffab
2005e432:	6aa080e7          	jalr	ra,1706(ra) # 9ad8 <__wrap_strlen>
2005e436:	942a                	c.add	s0,a0
2005e438:	0a05                	c.addi	s4,1
2005e43a:	02448493          	addi	s1,s1,36
2005e43e:	bf71                	c.j	2005e3da <vTaskList+0x4e>
2005e440:	50f2                	c.lwsp	ra,60(sp)
2005e442:	5462                	c.lwsp	s0,56(sp)
2005e444:	54d2                	c.lwsp	s1,52(sp)
2005e446:	5942                	c.lwsp	s2,48(sp)
2005e448:	59b2                	c.lwsp	s3,44(sp)
2005e44a:	5a22                	c.lwsp	s4,40(sp)
2005e44c:	5a92                	c.lwsp	s5,36(sp)
2005e44e:	5b02                	c.lwsp	s6,32(sp)
2005e450:	4bf2                	c.lwsp	s7,28(sp)
2005e452:	6121                	c.addi16sp	sp,64
2005e454:	8082                	c.jr	ra

2005e456 <pvTaskIncrementMutexHeldCount>:
2005e456:	f5c18713          	addi	a4,gp,-164 # 200668e4 <pxCurrentTCB>
2005e45a:	4318                	c.lw	a4,0(a4)
2005e45c:	f5c18793          	addi	a5,gp,-164 # 200668e4 <pxCurrentTCB>
2005e460:	c709                	c.beqz	a4,2005e46a <pvTaskIncrementMutexHeldCount+0x14>
2005e462:	4394                	c.lw	a3,0(a5)
2005e464:	52b8                	c.lw	a4,96(a3)
2005e466:	0705                	c.addi	a4,1
2005e468:	d2b8                	c.sw	a4,96(a3)
2005e46a:	4388                	c.lw	a0,0(a5)
2005e46c:	8082                	c.jr	ra

2005e46e <prvCheckForValidListAndQueue>:
2005e46e:	1141                	c.addi	sp,-16
2005e470:	c422                	c.swsp	s0,8(sp)
2005e472:	c606                	c.swsp	ra,12(sp)
2005e474:	c226                	c.swsp	s1,4(sp)
2005e476:	c04a                	c.swsp	s2,0(sp)
2005e478:	fa818413          	addi	s0,gp,-88 # 20066930 <xTimerQueue>
2005e47c:	f86ff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005e480:	401c                	c.lw	a5,0(s0)
2005e482:	e3b9                	c.bnez	a5,2005e4c8 <prvCheckForValidListAndQueue+0x5a>
2005e484:	20068537          	lui	a0,0x20068
2005e488:	4a050913          	addi	s2,a0,1184 # 200684a0 <xActiveTimerList1>
2005e48c:	4a050513          	addi	a0,a0,1184
2005e490:	9b5fc0ef          	jal	ra,2005ae44 <vListInitialise>
2005e494:	20068537          	lui	a0,0x20068
2005e498:	4b450493          	addi	s1,a0,1204 # 200684b4 <xActiveTimerList2>
2005e49c:	4b450513          	addi	a0,a0,1204
2005e4a0:	9a5fc0ef          	jal	ra,2005ae44 <vListInitialise>
2005e4a4:	200686b7          	lui	a3,0x20068
2005e4a8:	20068637          	lui	a2,0x20068
2005e4ac:	f921ae23          	sw	s2,-100(gp) # 20066924 <pxCurrentTimerList>
2005e4b0:	4701                	c.li	a4,0
2005e4b2:	4c868693          	addi	a3,a3,1224 # 200684c8 <xStaticTimerQueue.0>
2005e4b6:	42860613          	addi	a2,a2,1064 # 20068428 <ucStaticTimerQueueStorage.1>
2005e4ba:	45b1                	c.li	a1,12
2005e4bc:	4529                	c.li	a0,10
2005e4be:	fa91a023          	sw	s1,-96(gp) # 20066928 <pxOverflowTimerList>
2005e4c2:	981fe0ef          	jal	ra,2005ce42 <xQueueGenericCreateStatic>
2005e4c6:	c008                	c.sw	a0,0(s0)
2005e4c8:	4422                	c.lwsp	s0,8(sp)
2005e4ca:	40b2                	c.lwsp	ra,12(sp)
2005e4cc:	4492                	c.lwsp	s1,4(sp)
2005e4ce:	4902                	c.lwsp	s2,0(sp)
2005e4d0:	0141                	c.addi	sp,16
2005e4d2:	f4aff06f          	jal	zero,2005dc1c <vTaskExitCritical>

2005e4d6 <prvInsertTimerInActiveList>:
2005e4d6:	1141                	c.addi	sp,-16
2005e4d8:	c606                	c.swsp	ra,12(sp)
2005e4da:	c14c                	c.sw	a1,4(a0)
2005e4dc:	c908                	c.sw	a0,16(a0)
2005e4de:	87aa                	c.mv	a5,a0
2005e4e0:	02b66163          	bltu	a2,a1,2005e502 <prvInsertTimerInActiveList+0x2c>
2005e4e4:	4d18                	c.lw	a4,24(a0)
2005e4e6:	8e15                	c.sub	a2,a3
2005e4e8:	4505                	c.li	a0,1
2005e4ea:	00e67963          	bgeu	a2,a4,2005e4fc <prvInsertTimerInActiveList+0x26>
2005e4ee:	00478593          	addi	a1,a5,4
2005e4f2:	fa01a503          	lw	a0,-96(gp) # 20066928 <pxOverflowTimerList>
2005e4f6:	97ffc0ef          	jal	ra,2005ae74 <vListInsert>
2005e4fa:	4501                	c.li	a0,0
2005e4fc:	40b2                	c.lwsp	ra,12(sp)
2005e4fe:	0141                	c.addi	sp,16
2005e500:	8082                	c.jr	ra
2005e502:	00d67563          	bgeu	a2,a3,2005e50c <prvInsertTimerInActiveList+0x36>
2005e506:	4505                	c.li	a0,1
2005e508:	fed5fae3          	bgeu	a1,a3,2005e4fc <prvInsertTimerInActiveList+0x26>
2005e50c:	00478593          	addi	a1,a5,4
2005e510:	f9c1a503          	lw	a0,-100(gp) # 20066924 <pxCurrentTimerList>
2005e514:	b7cd                	c.j	2005e4f6 <prvInsertTimerInActiveList+0x20>

2005e516 <xTimerCreateTimerTask>:
2005e516:	1101                	c.addi	sp,-32
2005e518:	ce06                	c.swsp	ra,28(sp)
2005e51a:	f55ff0ef          	jal	ra,2005e46e <prvCheckForValidListAndQueue>
2005e51e:	fa81a783          	lw	a5,-88(gp) # 20066930 <xTimerQueue>
2005e522:	4501                	c.li	a0,0
2005e524:	cb9d                	c.beqz	a5,2005e55a <xTimerCreateTimerTask+0x44>
2005e526:	0070                	c.addi4spn	a2,sp,12
2005e528:	002c                	c.addi4spn	a1,sp,8
2005e52a:	0048                	c.addi4spn	a0,sp,4
2005e52c:	c202                	c.swsp	zero,4(sp)
2005e52e:	c402                	c.swsp	zero,8(sp)
2005e530:	92cfe0ef          	jal	ra,2005c65c <vApplicationGetTimerTaskMemory>
2005e534:	47a2                	c.lwsp	a5,8(sp)
2005e536:	4812                	c.lwsp	a6,4(sp)
2005e538:	4632                	c.lwsp	a2,12(sp)
2005e53a:	200635b7          	lui	a1,0x20063
2005e53e:	2005e537          	lui	a0,0x2005e
2005e542:	4729                	c.li	a4,10
2005e544:	4681                	c.li	a3,0
2005e546:	69c58593          	addi	a1,a1,1692 # 2006369c <__func__.1+0x30>
2005e54a:	79c50513          	addi	a0,a0,1948 # 2005e79c <prvTimerTask>
2005e54e:	ff8ff0ef          	jal	ra,2005dd46 <xTaskCreateStatic>
2005e552:	faa1a623          	sw	a0,-84(gp) # 20066934 <xTimerTaskHandle>
2005e556:	00a03533          	sltu	a0,zero,a0
2005e55a:	40f2                	c.lwsp	ra,28(sp)
2005e55c:	6105                	c.addi16sp	sp,32
2005e55e:	8082                	c.jr	ra

2005e560 <xTimerCreate>:
2005e560:	1101                	c.addi	sp,-32
2005e562:	c256                	c.swsp	s5,4(sp)
2005e564:	8aaa                	c.mv	s5,a0
2005e566:	02c00513          	addi	a0,zero,44
2005e56a:	cc22                	c.swsp	s0,24(sp)
2005e56c:	ca26                	c.swsp	s1,20(sp)
2005e56e:	c84a                	c.swsp	s2,16(sp)
2005e570:	c64e                	c.swsp	s3,12(sp)
2005e572:	c452                	c.swsp	s4,8(sp)
2005e574:	ce06                	c.swsp	ra,28(sp)
2005e576:	8a2e                	c.mv	s4,a1
2005e578:	84b2                	c.mv	s1,a2
2005e57a:	89b6                	c.mv	s3,a3
2005e57c:	893a                	c.mv	s2,a4
2005e57e:	e8cfc0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005e582:	842a                	c.mv	s0,a0
2005e584:	c905                	c.beqz	a0,2005e5b4 <xTimerCreate+0x54>
2005e586:	02050423          	sb	zero,40(a0)
2005e58a:	ee5ff0ef          	jal	ra,2005e46e <prvCheckForValidListAndQueue>
2005e58e:	00440513          	addi	a0,s0,4
2005e592:	01542023          	sw	s5,0(s0)
2005e596:	01442c23          	sw	s4,24(s0)
2005e59a:	01342e23          	sw	s3,28(s0)
2005e59e:	03242023          	sw	s2,32(s0)
2005e5a2:	8b7fc0ef          	jal	ra,2005ae58 <vListInitialiseItem>
2005e5a6:	c499                	c.beqz	s1,2005e5b4 <xTimerCreate+0x54>
2005e5a8:	02844783          	lbu	a5,40(s0)
2005e5ac:	0047e793          	ori	a5,a5,4
2005e5b0:	02f40423          	sb	a5,40(s0)
2005e5b4:	40f2                	c.lwsp	ra,28(sp)
2005e5b6:	8522                	c.mv	a0,s0
2005e5b8:	4462                	c.lwsp	s0,24(sp)
2005e5ba:	44d2                	c.lwsp	s1,20(sp)
2005e5bc:	4942                	c.lwsp	s2,16(sp)
2005e5be:	49b2                	c.lwsp	s3,12(sp)
2005e5c0:	4a22                	c.lwsp	s4,8(sp)
2005e5c2:	4a92                	c.lwsp	s5,4(sp)
2005e5c4:	6105                	c.addi16sp	sp,32
2005e5c6:	8082                	c.jr	ra

2005e5c8 <xTimerCreateStatic>:
2005e5c8:	1101                	c.addi	sp,-32
2005e5ca:	cc22                	c.swsp	s0,24(sp)
2005e5cc:	ce06                	c.swsp	ra,28(sp)
2005e5ce:	ca26                	c.swsp	s1,20(sp)
2005e5d0:	c84a                	c.swsp	s2,16(sp)
2005e5d2:	c64e                	c.swsp	s3,12(sp)
2005e5d4:	c452                	c.swsp	s4,8(sp)
2005e5d6:	c256                	c.swsp	s5,4(sp)
2005e5d8:	843e                	c.mv	s0,a5
2005e5da:	cf95                	c.beqz	a5,2005e616 <xTimerCreateStatic+0x4e>
2005e5dc:	4789                	c.li	a5,2
2005e5de:	02f40423          	sb	a5,40(s0)
2005e5e2:	8aaa                	c.mv	s5,a0
2005e5e4:	8a2e                	c.mv	s4,a1
2005e5e6:	84b2                	c.mv	s1,a2
2005e5e8:	89b6                	c.mv	s3,a3
2005e5ea:	893a                	c.mv	s2,a4
2005e5ec:	e83ff0ef          	jal	ra,2005e46e <prvCheckForValidListAndQueue>
2005e5f0:	00440513          	addi	a0,s0,4
2005e5f4:	01542023          	sw	s5,0(s0)
2005e5f8:	01442c23          	sw	s4,24(s0)
2005e5fc:	01342e23          	sw	s3,28(s0)
2005e600:	03242023          	sw	s2,32(s0)
2005e604:	855fc0ef          	jal	ra,2005ae58 <vListInitialiseItem>
2005e608:	c499                	c.beqz	s1,2005e616 <xTimerCreateStatic+0x4e>
2005e60a:	02844783          	lbu	a5,40(s0)
2005e60e:	0047e793          	ori	a5,a5,4
2005e612:	02f40423          	sb	a5,40(s0)
2005e616:	40f2                	c.lwsp	ra,28(sp)
2005e618:	8522                	c.mv	a0,s0
2005e61a:	4462                	c.lwsp	s0,24(sp)
2005e61c:	44d2                	c.lwsp	s1,20(sp)
2005e61e:	4942                	c.lwsp	s2,16(sp)
2005e620:	49b2                	c.lwsp	s3,12(sp)
2005e622:	4a22                	c.lwsp	s4,8(sp)
2005e624:	4a92                	c.lwsp	s5,4(sp)
2005e626:	6105                	c.addi16sp	sp,32
2005e628:	8082                	c.jr	ra

2005e62a <xTimerGenericCommand>:
2005e62a:	7139                	c.addi16sp	sp,-64
2005e62c:	dc22                	c.swsp	s0,56(sp)
2005e62e:	da26                	c.swsp	s1,52(sp)
2005e630:	d84a                	c.swsp	s2,48(sp)
2005e632:	d452                	c.swsp	s4,40(sp)
2005e634:	d256                	c.swsp	s5,36(sp)
2005e636:	de06                	c.swsp	ra,60(sp)
2005e638:	d64e                	c.swsp	s3,44(sp)
2005e63a:	842a                	c.mv	s0,a0
2005e63c:	84ae                	c.mv	s1,a1
2005e63e:	8932                	c.mv	s2,a2
2005e640:	8a36                	c.mv	s4,a3
2005e642:	8aba                	c.mv	s5,a4
2005e644:	bd6ff0ef          	jal	ra,2005da1a <xTaskGetCurrentTaskHandle>
2005e648:	fac1a783          	lw	a5,-84(gp) # 20066934 <xTimerTaskHandle>
2005e64c:	06a79163          	bne	a5,a0,2005e6ae <xTimerGenericCommand+0x84>
2005e650:	ffd48793          	addi	a5,s1,-3
2005e654:	4705                	c.li	a4,1
2005e656:	04f76c63          	bltu	a4,a5,2005e6ae <xTimerGenericCommand+0x84>
2005e65a:	cc1ff0ef          	jal	ra,2005e31a <xTaskGetTickCount>
2005e65e:	485c                	c.lw	a5,20(s0)
2005e660:	86aa                	c.mv	a3,a0
2005e662:	c799                	c.beqz	a5,2005e670 <xTimerGenericCommand+0x46>
2005e664:	c62a                	c.swsp	a0,12(sp)
2005e666:	00440513          	addi	a0,s0,4
2005e66a:	839fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e66e:	46b2                	c.lwsp	a3,12(sp)
2005e670:	4711                	c.li	a4,4
2005e672:	02844783          	lbu	a5,40(s0)
2005e676:	00e48f63          	beq	s1,a4,2005e694 <xTimerGenericCommand+0x6a>
2005e67a:	9bf9                	c.andi	a5,-2
2005e67c:	02f40423          	sb	a5,40(s0)
2005e680:	4505                	c.li	a0,1
2005e682:	50f2                	c.lwsp	ra,60(sp)
2005e684:	5462                	c.lwsp	s0,56(sp)
2005e686:	54d2                	c.lwsp	s1,52(sp)
2005e688:	5942                	c.lwsp	s2,48(sp)
2005e68a:	59b2                	c.lwsp	s3,44(sp)
2005e68c:	5a22                	c.lwsp	s4,40(sp)
2005e68e:	5a92                	c.lwsp	s5,36(sp)
2005e690:	6121                	c.addi16sp	sp,64
2005e692:	8082                	c.jr	ra
2005e694:	0017e793          	ori	a5,a5,1
2005e698:	02f40423          	sb	a5,40(s0)
2005e69c:	01242c23          	sw	s2,24(s0)
2005e6a0:	8636                	c.mv	a2,a3
2005e6a2:	00d905b3          	add	a1,s2,a3
2005e6a6:	8522                	c.mv	a0,s0
2005e6a8:	e2fff0ef          	jal	ra,2005e4d6 <prvInsertTimerInActiveList>
2005e6ac:	bfd1                	c.j	2005e680 <xTimerGenericCommand+0x56>
2005e6ae:	fa818993          	addi	s3,gp,-88 # 20066930 <xTimerQueue>
2005e6b2:	0009a783          	lw	a5,0(s3)
2005e6b6:	4501                	c.li	a0,0
2005e6b8:	d7e9                	c.beqz	a5,2005e682 <xTimerGenericCommand+0x58>
2005e6ba:	ca26                	c.swsp	s1,20(sp)
2005e6bc:	cc4a                	c.swsp	s2,24(sp)
2005e6be:	ce22                	c.swsp	s0,28(sp)
2005e6c0:	4715                	c.li	a4,5
2005e6c2:	02974163          	blt	a4,s1,2005e6e4 <xTimerGenericCommand+0xba>
2005e6c6:	b5aff0ef          	jal	ra,2005da20 <xTaskGetSchedulerState>
2005e6ca:	872a                	c.mv	a4,a0
2005e6cc:	4789                	c.li	a5,2
2005e6ce:	0009a503          	lw	a0,0(s3)
2005e6d2:	4681                	c.li	a3,0
2005e6d4:	8656                	c.mv	a2,s5
2005e6d6:	00f70363          	beq	a4,a5,2005e6dc <xTimerGenericCommand+0xb2>
2005e6da:	4601                	c.li	a2,0
2005e6dc:	084c                	c.addi4spn	a1,sp,20
2005e6de:	9e2fe0ef          	jal	ra,2005c8c0 <xQueueGenericSend>
2005e6e2:	b745                	c.j	2005e682 <xTimerGenericCommand+0x58>
2005e6e4:	4681                	c.li	a3,0
2005e6e6:	8652                	c.mv	a2,s4
2005e6e8:	084c                	c.addi4spn	a1,sp,20
2005e6ea:	853e                	c.mv	a0,a5
2005e6ec:	ad8fe0ef          	jal	ra,2005c9c4 <xQueueGenericSendFromISR>
2005e6f0:	bf49                	c.j	2005e682 <xTimerGenericCommand+0x58>

2005e6f2 <prvSampleTimeNow>:
2005e6f2:	7179                	c.addi16sp	sp,-48
2005e6f4:	d226                	c.swsp	s1,36(sp)
2005e6f6:	d04a                	c.swsp	s2,32(sp)
2005e6f8:	ce4e                	c.swsp	s3,28(sp)
2005e6fa:	d606                	c.swsp	ra,44(sp)
2005e6fc:	d422                	c.swsp	s0,40(sp)
2005e6fe:	cc52                	c.swsp	s4,24(sp)
2005e700:	ca56                	c.swsp	s5,20(sp)
2005e702:	89aa                	c.mv	s3,a0
2005e704:	c17ff0ef          	jal	ra,2005e31a <xTaskGetTickCount>
2005e708:	fa41a783          	lw	a5,-92(gp) # 2006692c <xLastTime.2>
2005e70c:	84aa                	c.mv	s1,a0
2005e70e:	fa418913          	addi	s2,gp,-92 # 2006692c <xLastTime.2>
2005e712:	08f57263          	bgeu	a0,a5,2005e796 <prvSampleTimeNow+0xa4>
2005e716:	f9c18a13          	addi	s4,gp,-100 # 20066924 <pxCurrentTimerList>
2005e71a:	000a2703          	lw	a4,0(s4)
2005e71e:	431c                	c.lw	a5,0(a4)
2005e720:	e795                	c.bnez	a5,2005e74c <prvSampleTimeNow+0x5a>
2005e722:	fa018793          	addi	a5,gp,-96 # 20066928 <pxOverflowTimerList>
2005e726:	4394                	c.lw	a3,0(a5)
2005e728:	c398                	c.sw	a4,0(a5)
2005e72a:	4785                	c.li	a5,1
2005e72c:	00da2023          	sw	a3,0(s4)
2005e730:	00f9a023          	sw	a5,0(s3)
2005e734:	50b2                	c.lwsp	ra,44(sp)
2005e736:	5422                	c.lwsp	s0,40(sp)
2005e738:	00992023          	sw	s1,0(s2)
2005e73c:	49f2                	c.lwsp	s3,28(sp)
2005e73e:	5902                	c.lwsp	s2,32(sp)
2005e740:	4a62                	c.lwsp	s4,24(sp)
2005e742:	4ad2                	c.lwsp	s5,20(sp)
2005e744:	8526                	c.mv	a0,s1
2005e746:	5492                	c.lwsp	s1,36(sp)
2005e748:	6145                	c.addi16sp	sp,48
2005e74a:	8082                	c.jr	ra
2005e74c:	475c                	c.lw	a5,12(a4)
2005e74e:	47c0                	c.lw	s0,12(a5)
2005e750:	4390                	c.lw	a2,0(a5)
2005e752:	00440593          	addi	a1,s0,4
2005e756:	852e                	c.mv	a0,a1
2005e758:	c632                	c.swsp	a2,12(sp)
2005e75a:	c42e                	c.swsp	a1,8(sp)
2005e75c:	f46fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e760:	501c                	c.lw	a5,32(s0)
2005e762:	8522                	c.mv	a0,s0
2005e764:	9782                	c.jalr	a5
2005e766:	02844783          	lbu	a5,40(s0)
2005e76a:	45a2                	c.lwsp	a1,8(sp)
2005e76c:	4632                	c.lwsp	a2,12(sp)
2005e76e:	8b91                	c.andi	a5,4
2005e770:	d3dd                	c.beqz	a5,2005e716 <prvSampleTimeNow+0x24>
2005e772:	4c1c                	c.lw	a5,24(s0)
2005e774:	97b2                	c.add	a5,a2
2005e776:	00f67963          	bgeu	a2,a5,2005e788 <prvSampleTimeNow+0x96>
2005e77a:	000a2503          	lw	a0,0(s4)
2005e77e:	c05c                	c.sw	a5,4(s0)
2005e780:	c800                	c.sw	s0,16(s0)
2005e782:	ef2fc0ef          	jal	ra,2005ae74 <vListInsert>
2005e786:	bf41                	c.j	2005e716 <prvSampleTimeNow+0x24>
2005e788:	4701                	c.li	a4,0
2005e78a:	4681                	c.li	a3,0
2005e78c:	4581                	c.li	a1,0
2005e78e:	8522                	c.mv	a0,s0
2005e790:	e9bff0ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005e794:	b749                	c.j	2005e716 <prvSampleTimeNow+0x24>
2005e796:	0009a023          	sw	zero,0(s3)
2005e79a:	bf69                	c.j	2005e734 <prvSampleTimeNow+0x42>

2005e79c <prvTimerTask>:
2005e79c:	7139                	c.addi16sp	sp,-64
2005e79e:	d84a                	c.swsp	s2,48(sp)
2005e7a0:	20063937          	lui	s2,0x20063
2005e7a4:	d64e                	c.swsp	s3,44(sp)
2005e7a6:	d256                	c.swsp	s5,36(sp)
2005e7a8:	d05a                	c.swsp	s6,32(sp)
2005e7aa:	de06                	c.swsp	ra,60(sp)
2005e7ac:	dc22                	c.swsp	s0,56(sp)
2005e7ae:	da26                	c.swsp	s1,52(sp)
2005e7b0:	d452                	c.swsp	s4,40(sp)
2005e7b2:	ce5e                	c.swsp	s7,28(sp)
2005e7b4:	cc62                	c.swsp	s8,24(sp)
2005e7b6:	6a490913          	addi	s2,s2,1700 # 200636a4 <__func__.1+0x38>
2005e7ba:	f9c1a783          	lw	a5,-100(gp) # 20066924 <pxCurrentTimerList>
2005e7be:	f9c18c13          	addi	s8,gp,-100 # 20066924 <pxCurrentTimerList>
2005e7c2:	4405                	c.li	s0,1
2005e7c4:	4384                	c.lw	s1,0(a5)
2005e7c6:	c481                	c.beqz	s1,2005e7ce <prvTimerTask+0x32>
2005e7c8:	47dc                	c.lw	a5,12(a5)
2005e7ca:	4401                	c.li	s0,0
2005e7cc:	4384                	c.lw	s1,0(a5)
2005e7ce:	f0ffe0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e7d2:	0048                	c.addi4spn	a0,sp,4
2005e7d4:	f1fff0ef          	jal	ra,2005e6f2 <prvSampleTimeNow>
2005e7d8:	4792                	c.lwsp	a5,4(sp)
2005e7da:	8baa                	c.mv	s7,a0
2005e7dc:	fa818a13          	addi	s4,gp,-88 # 20066930 <xTimerQueue>
2005e7e0:	ebc5                	c.bnez	a5,2005e890 <prvTimerTask+0xf4>
2005e7e2:	e449                	c.bnez	s0,2005e86c <prvTimerTask+0xd0>
2005e7e4:	08956963          	bltu	a0,s1,2005e876 <prvTimerTask+0xda>
2005e7e8:	fa2ff0ef          	jal	ra,2005df8a <xTaskResumeAll>
2005e7ec:	000c2783          	lw	a5,0(s8)
2005e7f0:	47dc                	c.lw	a5,12(a5)
2005e7f2:	47c0                	c.lw	s0,12(a5)
2005e7f4:	00440513          	addi	a0,s0,4
2005e7f8:	eaafc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e7fc:	02844783          	lbu	a5,40(s0)
2005e800:	0047f713          	andi	a4,a5,4
2005e804:	c325                	c.beqz	a4,2005e864 <prvTimerTask+0xc8>
2005e806:	4c0c                	c.lw	a1,24(s0)
2005e808:	86a6                	c.mv	a3,s1
2005e80a:	865e                	c.mv	a2,s7
2005e80c:	95a6                	c.add	a1,s1
2005e80e:	8522                	c.mv	a0,s0
2005e810:	cc7ff0ef          	jal	ra,2005e4d6 <prvInsertTimerInActiveList>
2005e814:	c901                	c.beqz	a0,2005e824 <prvTimerTask+0x88>
2005e816:	4701                	c.li	a4,0
2005e818:	4681                	c.li	a3,0
2005e81a:	8626                	c.mv	a2,s1
2005e81c:	4581                	c.li	a1,0
2005e81e:	8522                	c.mv	a0,s0
2005e820:	e0bff0ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005e824:	501c                	c.lw	a5,32(s0)
2005e826:	8522                	c.mv	a0,s0
2005e828:	9782                	c.jalr	a5
2005e82a:	44a5                	c.li	s1,9
2005e82c:	000a2503          	lw	a0,0(s4)
2005e830:	4601                	c.li	a2,0
2005e832:	004c                	c.addi4spn	a1,sp,4
2005e834:	a4efe0ef          	jal	ra,2005ca82 <xQueueReceive>
2005e838:	d149                	c.beqz	a0,2005e7ba <prvTimerTask+0x1e>
2005e83a:	4792                	c.lwsp	a5,4(sp)
2005e83c:	fe07c8e3          	blt	a5,zero,2005e82c <prvTimerTask+0x90>
2005e840:	4432                	c.lwsp	s0,12(sp)
2005e842:	485c                	c.lw	a5,20(s0)
2005e844:	c789                	c.beqz	a5,2005e84e <prvTimerTask+0xb2>
2005e846:	00440513          	addi	a0,s0,4
2005e84a:	e58fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e84e:	850a                	c.mv	a0,sp
2005e850:	ea3ff0ef          	jal	ra,2005e6f2 <prvSampleTimeNow>
2005e854:	4792                	c.lwsp	a5,4(sp)
2005e856:	862a                	c.mv	a2,a0
2005e858:	fcf4eae3          	bltu	s1,a5,2005e82c <prvTimerTask+0x90>
2005e85c:	078a                	c.slli	a5,0x2
2005e85e:	97ca                	c.add	a5,s2
2005e860:	439c                	c.lw	a5,0(a5)
2005e862:	8782                	c.jr	a5
2005e864:	9bf9                	c.andi	a5,-2
2005e866:	02f40423          	sb	a5,40(s0)
2005e86a:	bf6d                	c.j	2005e824 <prvTimerTask+0x88>
2005e86c:	fa01a783          	lw	a5,-96(gp) # 20066928 <pxOverflowTimerList>
2005e870:	4380                	c.lw	s0,0(a5)
2005e872:	00143413          	sltiu	s0,s0,1
2005e876:	000a2503          	lw	a0,0(s4)
2005e87a:	8622                	c.mv	a2,s0
2005e87c:	417485b3          	sub	a1,s1,s7
2005e880:	c7afe0ef          	jal	ra,2005ccfa <vQueueWaitForMessageRestricted>
2005e884:	f06ff0ef          	jal	ra,2005df8a <xTaskResumeAll>
2005e888:	f14d                	c.bnez	a0,2005e82a <prvTimerTask+0x8e>
2005e88a:	00000073          	ecall
2005e88e:	bf71                	c.j	2005e82a <prvTimerTask+0x8e>
2005e890:	efaff0ef          	jal	ra,2005df8a <xTaskResumeAll>
2005e894:	bf59                	c.j	2005e82a <prvTimerTask+0x8e>
2005e896:	02844783          	lbu	a5,40(s0)
2005e89a:	4c0c                	c.lw	a1,24(s0)
2005e89c:	8522                	c.mv	a0,s0
2005e89e:	0017e793          	ori	a5,a5,1
2005e8a2:	02f40423          	sb	a5,40(s0)
2005e8a6:	46a2                	c.lwsp	a3,8(sp)
2005e8a8:	95b6                	c.add	a1,a3
2005e8aa:	c2dff0ef          	jal	ra,2005e4d6 <prvInsertTimerInActiveList>
2005e8ae:	dd3d                	c.beqz	a0,2005e82c <prvTimerTask+0x90>
2005e8b0:	501c                	c.lw	a5,32(s0)
2005e8b2:	8522                	c.mv	a0,s0
2005e8b4:	9782                	c.jalr	a5
2005e8b6:	02844783          	lbu	a5,40(s0)
2005e8ba:	8b91                	c.andi	a5,4
2005e8bc:	dba5                	c.beqz	a5,2005e82c <prvTimerTask+0x90>
2005e8be:	4c1c                	c.lw	a5,24(s0)
2005e8c0:	4622                	c.lwsp	a2,8(sp)
2005e8c2:	4701                	c.li	a4,0
2005e8c4:	4681                	c.li	a3,0
2005e8c6:	963e                	c.add	a2,a5
2005e8c8:	4581                	c.li	a1,0
2005e8ca:	8522                	c.mv	a0,s0
2005e8cc:	d5fff0ef          	jal	ra,2005e62a <xTimerGenericCommand>
2005e8d0:	bfb1                	c.j	2005e82c <prvTimerTask+0x90>
2005e8d2:	02844783          	lbu	a5,40(s0)
2005e8d6:	9bf9                	c.andi	a5,-2
2005e8d8:	02f40423          	sb	a5,40(s0)
2005e8dc:	bf81                	c.j	2005e82c <prvTimerTask+0x90>
2005e8de:	02844783          	lbu	a5,40(s0)
2005e8e2:	86aa                	c.mv	a3,a0
2005e8e4:	0017e793          	ori	a5,a5,1
2005e8e8:	02f40423          	sb	a5,40(s0)
2005e8ec:	45a2                	c.lwsp	a1,8(sp)
2005e8ee:	cc0c                	c.sw	a1,24(s0)
2005e8f0:	95aa                	c.add	a1,a0
2005e8f2:	8522                	c.mv	a0,s0
2005e8f4:	be3ff0ef          	jal	ra,2005e4d6 <prvInsertTimerInActiveList>
2005e8f8:	bf15                	c.j	2005e82c <prvTimerTask+0x90>
2005e8fa:	02844783          	lbu	a5,40(s0)
2005e8fe:	0027f713          	andi	a4,a5,2
2005e902:	fb71                	c.bnez	a4,2005e8d6 <prvTimerTask+0x13a>
2005e904:	8522                	c.mv	a0,s0
2005e906:	be2fc0ef          	jal	ra,2005ace8 <vPortFree>
2005e90a:	b70d                	c.j	2005e82c <prvTimerTask+0x90>

2005e90c <xTimerIsTimerActive>:
2005e90c:	1101                	c.addi	sp,-32
2005e90e:	ce06                	c.swsp	ra,28(sp)
2005e910:	cc22                	c.swsp	s0,24(sp)
2005e912:	842a                	c.mv	s0,a0
2005e914:	aeeff0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005e918:	02844503          	lbu	a0,40(s0)
2005e91c:	8905                	c.andi	a0,1
2005e91e:	c62a                	c.swsp	a0,12(sp)
2005e920:	afcff0ef          	jal	ra,2005dc1c <vTaskExitCritical>
2005e924:	40f2                	c.lwsp	ra,28(sp)
2005e926:	4462                	c.lwsp	s0,24(sp)
2005e928:	4532                	c.lwsp	a0,12(sp)
2005e92a:	6105                	c.addi16sp	sp,32
2005e92c:	8082                	c.jr	ra

2005e92e <wifi_fast_connect_enable>:
2005e92e:	fa01ac23          	sw	zero,-72(gp) # 20066940 <p_wifi_do_fast_connect>
2005e932:	fa01aa23          	sw	zero,-76(gp) # 2006693c <p_store_fast_connect_info>
2005e936:	8082                	c.jr	ra

2005e938 <rtw_indicate_event_handle>:
2005e938:	4761                	c.li	a4,24
2005e93a:	04a74e63          	blt	a4,a0,2005e996 <rtw_indicate_event_handle+0x5e>
2005e93e:	1101                	c.addi	sp,-32
2005e940:	20068737          	lui	a4,0x20068
2005e944:	ca26                	c.swsp	s1,20(sp)
2005e946:	c84a                	c.swsp	s2,16(sp)
2005e948:	00451493          	slli	s1,a0,0x4
2005e94c:	892e                	c.mv	s2,a1
2005e94e:	85b2                	c.mv	a1,a2
2005e950:	8636                	c.mv	a2,a3
2005e952:	51870693          	addi	a3,a4,1304 # 20068518 <event_callback_list>
2005e956:	cc22                	c.swsp	s0,24(sp)
2005e958:	ce06                	c.swsp	ra,28(sp)
2005e95a:	009687b3          	add	a5,a3,s1
2005e95e:	0007a803          	lw	a6,0(a5)
2005e962:	51870413          	addi	s0,a4,1304
2005e966:	00080963          	beq	a6,zero,2005e978 <rtw_indicate_event_handle+0x40>
2005e96a:	43d4                	c.lw	a3,4(a5)
2005e96c:	c632                	c.swsp	a2,12(sp)
2005e96e:	c42e                	c.swsp	a1,8(sp)
2005e970:	854a                	c.mv	a0,s2
2005e972:	9802                	c.jalr	a6
2005e974:	4632                	c.lwsp	a2,12(sp)
2005e976:	45a2                	c.lwsp	a1,8(sp)
2005e978:	009407b3          	add	a5,s0,s1
2005e97c:	4798                	c.lw	a4,8(a5)
2005e97e:	4501                	c.li	a0,0
2005e980:	c709                	c.beqz	a4,2005e98a <rtw_indicate_event_handle+0x52>
2005e982:	47d4                	c.lw	a3,12(a5)
2005e984:	854a                	c.mv	a0,s2
2005e986:	9702                	c.jalr	a4
2005e988:	4501                	c.li	a0,0
2005e98a:	40f2                	c.lwsp	ra,28(sp)
2005e98c:	4462                	c.lwsp	s0,24(sp)
2005e98e:	44d2                	c.lwsp	s1,20(sp)
2005e990:	4942                	c.lwsp	s2,16(sp)
2005e992:	6105                	c.addi16sp	sp,32
2005e994:	8082                	c.jr	ra
2005e996:	5579                	c.li	a0,-2
2005e998:	8082                	c.jr	ra

2005e99a <wifi_indication>:
2005e99a:	472d                	c.li	a4,11
2005e99c:	00e51863          	bne	a0,a4,2005e9ac <wifi_indication+0x12>
2005e9a0:	fbc1a703          	lw	a4,-68(gp) # 20066944 <p_wifi_joinstatus_internal_callback>
2005e9a4:	c711                	c.beqz	a4,2005e9b0 <wifi_indication+0x16>
2005e9a6:	0ff6f513          	andi	a0,a3,255
2005e9aa:	8702                	c.jr	a4
2005e9ac:	f8dff06f          	jal	zero,2005e938 <rtw_indicate_event_handle>
2005e9b0:	8082                	c.jr	ra

2005e9b2 <_init_thread>:
2005e9b2:	1141                	c.addi	sp,-16
2005e9b4:	c422                	c.swsp	s0,8(sp)
2005e9b6:	c606                	c.swsp	ra,12(sp)
2005e9b8:	4100c437          	lui	s0,0x4100c
2005e9bc:	08442783          	lw	a5,132(s0) # 4100c084 <__km4_bd_boot_download_addr__+0x10ffa084>
2005e9c0:	08440713          	addi	a4,s0,132
2005e9c4:	8b85                	c.andi	a5,1
2005e9c6:	c7b1                	c.beqz	a5,2005ea12 <_init_thread+0x60>
2005e9c8:	431c                	c.lw	a5,0(a4)
2005e9ca:	4505                	c.li	a0,1
2005e9cc:	9bf9                	c.andi	a5,-2
2005e9ce:	c31c                	c.sw	a5,0(a4)
2005e9d0:	17d010ef          	jal	ra,2006034c <wifi_on>
2005e9d4:	4505                	c.li	a0,1
2005e9d6:	209010ef          	jal	ra,200603de <wifi_config_autoreconnect>
2005e9da:	d35fc0ef          	jal	ra,2005b70e <rtos_mem_get_free_heap_size>
2005e9de:	20063737          	lui	a4,0x20063
2005e9e2:	200636b7          	lui	a3,0x20063
2005e9e6:	200635b7          	lui	a1,0x20063
2005e9ea:	882a                	c.mv	a6,a0
2005e9ec:	03500793          	addi	a5,zero,53
2005e9f0:	4511                	c.li	a0,4
2005e9f2:	71070713          	addi	a4,a4,1808 # 20063710 <__FUNCTION__.0>
2005e9f6:	6cc68693          	addi	a3,a3,1740 # 200636cc <__func__.1+0x60>
2005e9fa:	04900613          	addi	a2,zero,73
2005e9fe:	6e858593          	addi	a1,a1,1768 # 200636e8 <__func__.1+0x7c>
2005ea02:	f74fc0ef          	jal	ra,2005b176 <rtk_log_write>
2005ea06:	4422                	c.lwsp	s0,8(sp)
2005ea08:	40b2                	c.lwsp	ra,12(sp)
2005ea0a:	4501                	c.li	a0,0
2005ea0c:	0141                	c.addi	sp,16
2005ea0e:	9c0fd06f          	jal	zero,2005bbce <rtos_task_delete>
2005ea12:	4505                	c.li	a0,1
2005ea14:	9ccfd0ef          	jal	ra,2005bbe0 <rtos_time_delay_ms>
2005ea18:	b755                	c.j	2005e9bc <_init_thread+0xa>

2005ea1a <wlan_initialize>:
2005ea1a:	1141                	c.addi	sp,-16
2005ea1c:	c606                	c.swsp	ra,12(sp)
2005ea1e:	041010ef          	jal	ra,2006025e <wifi_set_rom2flash>
2005ea22:	1ca020ef          	jal	ra,20060bec <inic_host_init>
2005ea26:	4505                	c.li	a0,1
2005ea28:	f07ff0ef          	jal	ra,2005e92e <wifi_fast_connect_enable>
2005ea2c:	6705                	c.lui	a4,0x1
2005ea2e:	2005f637          	lui	a2,0x2005f
2005ea32:	200635b7          	lui	a1,0x20063
2005ea36:	4789                	c.li	a5,2
2005ea38:	40070713          	addi	a4,a4,1024 # 1400 <CPU_ClkGet+0x8ea>
2005ea3c:	4681                	c.li	a3,0
2005ea3e:	9b260613          	addi	a2,a2,-1614 # 2005e9b2 <_init_thread>
2005ea42:	6f058593          	addi	a1,a1,1776 # 200636f0 <__func__.1+0x84>
2005ea46:	4501                	c.li	a0,0
2005ea48:	962fd0ef          	jal	ra,2005bbaa <rtos_task_create>
2005ea4c:	c105                	c.beqz	a0,2005ea6c <wlan_initialize+0x52>
2005ea4e:	40b2                	c.lwsp	ra,12(sp)
2005ea50:	200636b7          	lui	a3,0x20063
2005ea54:	200635b7          	lui	a1,0x20063
2005ea58:	6f868693          	addi	a3,a3,1784 # 200636f8 <__func__.1+0x8c>
2005ea5c:	04500613          	addi	a2,zero,69
2005ea60:	6e858593          	addi	a1,a1,1768 # 200636e8 <__func__.1+0x7c>
2005ea64:	4509                	c.li	a0,2
2005ea66:	0141                	c.addi	sp,16
2005ea68:	f0efc06f          	jal	zero,2005b176 <rtk_log_write>
2005ea6c:	40b2                	c.lwsp	ra,12(sp)
2005ea6e:	0141                	c.addi	sp,16
2005ea70:	8082                	c.jr	ra

2005ea72 <ChipInfo_GetSocName_ToBuf>:
2005ea72:	7139                	c.addi16sp	sp,-64
2005ea74:	da26                	c.swsp	s1,52(sp)
2005ea76:	84ae                	c.mv	s1,a1
2005ea78:	200645b7          	lui	a1,0x20064
2005ea7c:	d84a                	c.swsp	s2,48(sp)
2005ea7e:	4625                	c.li	a2,9
2005ea80:	892a                	c.mv	s2,a0
2005ea82:	81f58593          	addi	a1,a1,-2017 # 2006381f <__FUNCTION__.0+0x10f>
2005ea86:	0068                	c.addi4spn	a0,sp,12
2005ea88:	dc22                	c.swsp	s0,56(sp)
2005ea8a:	de06                	c.swsp	ra,60(sp)
2005ea8c:	d64e                	c.swsp	s3,44(sp)
2005ea8e:	d452                	c.swsp	s4,40(sp)
2005ea90:	dffab097          	auipc	ra,0xdffab
2005ea94:	040080e7          	jalr	ra,64(ra) # 9ad0 <__wrap_memcpy>
2005ea98:	00010aa3          	sb	zero,21(sp)
2005ea9c:	4401                	c.li	s0,0
2005ea9e:	c899                	c.beqz	s1,2005eab4 <ChipInfo_GetSocName_ToBuf+0x42>
2005eaa0:	20064637          	lui	a2,0x20064
2005eaa4:	0074                	c.addi4spn	a3,sp,12
2005eaa6:	85060613          	addi	a2,a2,-1968 # 20063850 <__FUNCTION__.0+0x140>
2005eaaa:	85a6                	c.mv	a1,s1
2005eaac:	854a                	c.mv	a0,s2
2005eaae:	29f000ef          	jal	ra,2005f54c <DiagSnPrintf>
2005eab2:	842a                	c.mv	s0,a0
2005eab4:	7f000993          	addi	s3,zero,2032
2005eab8:	20064a37          	lui	s4,0x20064
2005eabc:	00b10593          	addi	a1,sp,11
2005eac0:	854e                	c.mv	a0,s3
2005eac2:	9dbfa0ef          	jal	ra,2005949c <OTP_Read8>
2005eac6:	e131                	c.bnez	a0,2005eb0a <ChipInfo_GetSocName_ToBuf+0x98>
2005eac8:	200646b7          	lui	a3,0x20064
2005eacc:	200635b7          	lui	a1,0x20063
2005ead0:	85868693          	addi	a3,a3,-1960 # 20063858 <__FUNCTION__.0+0x148>
2005ead4:	04500613          	addi	a2,zero,69
2005ead8:	76458593          	addi	a1,a1,1892 # 20063764 <__FUNCTION__.0+0x54>
2005eadc:	4509                	c.li	a0,2
2005eade:	e98fc0ef          	jal	ra,2005b176 <rtk_log_write>
2005eae2:	00947c63          	bgeu	s0,s1,2005eafa <ChipInfo_GetSocName_ToBuf+0x88>
2005eae6:	20063637          	lui	a2,0x20063
2005eaea:	8b860613          	addi	a2,a2,-1864 # 200628b8 <pmap_func+0x4ac>
2005eaee:	408485b3          	sub	a1,s1,s0
2005eaf2:	00890533          	add	a0,s2,s0
2005eaf6:	257000ef          	jal	ra,2005f54c <DiagSnPrintf>
2005eafa:	50f2                	c.lwsp	ra,60(sp)
2005eafc:	5462                	c.lwsp	s0,56(sp)
2005eafe:	54d2                	c.lwsp	s1,52(sp)
2005eb00:	5942                	c.lwsp	s2,48(sp)
2005eb02:	59b2                	c.lwsp	s3,44(sp)
2005eb04:	5a22                	c.lwsp	s4,40(sp)
2005eb06:	6121                	c.addi16sp	sp,64
2005eb08:	8082                	c.jr	ra
2005eb0a:	00947d63          	bgeu	s0,s1,2005eb24 <ChipInfo_GetSocName_ToBuf+0xb2>
2005eb0e:	00b14683          	lbu	a3,11(sp)
2005eb12:	408485b3          	sub	a1,s1,s0
2005eb16:	00890533          	add	a0,s2,s0
2005eb1a:	870a0613          	addi	a2,s4,-1936 # 20063870 <__FUNCTION__.0+0x160>
2005eb1e:	22f000ef          	jal	ra,2005f54c <DiagSnPrintf>
2005eb22:	942a                	c.add	s0,a0
2005eb24:	0985                	c.addi	s3,1
2005eb26:	80098793          	addi	a5,s3,-2048
2005eb2a:	fbc9                	c.bnez	a5,2005eabc <ChipInfo_GetSocName_ToBuf+0x4a>
2005eb2c:	bf5d                	c.j	2005eae2 <ChipInfo_GetSocName_ToBuf+0x70>

2005eb2e <ChipInfo_GetLibVersion_ToBuf>:
2005eb2e:	1101                	c.addi	sp,-32
2005eb30:	cc22                	c.swsp	s0,24(sp)
2005eb32:	ca26                	c.swsp	s1,20(sp)
2005eb34:	c64e                	c.swsp	s3,12(sp)
2005eb36:	ce06                	c.swsp	ra,28(sp)
2005eb38:	c84a                	c.swsp	s2,16(sp)
2005eb3a:	c452                	c.swsp	s4,8(sp)
2005eb3c:	c256                	c.swsp	s5,4(sp)
2005eb3e:	c05a                	c.swsp	s6,0(sp)
2005eb40:	4100c7b7          	lui	a5,0x4100c
2005eb44:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005eb48:	89aa                	c.mv	s3,a0
2005eb4a:	84ae                	c.mv	s1,a1
2005eb4c:	4401                	c.li	s0,0
2005eb4e:	c1b5                	c.beqz	a1,2005ebb2 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005eb50:	8399                	c.srli	a5,0x6
2005eb52:	8b8d                	c.andi	a5,3
2005eb54:	e7d9                	c.bnez	a5,2005ebe2 <ChipInfo_GetLibVersion_ToBuf+0xb4>
2005eb56:	200636b7          	lui	a3,0x20063
2005eb5a:	4b468693          	addi	a3,a3,1204 # 200634b4 <__FUNCTION__.0+0x1c>
2005eb5e:	20064637          	lui	a2,0x20064
2005eb62:	87c60613          	addi	a2,a2,-1924 # 2006387c <__FUNCTION__.0+0x16c>
2005eb66:	85a6                	c.mv	a1,s1
2005eb68:	854e                	c.mv	a0,s3
2005eb6a:	1e3000ef          	jal	ra,2005f54c <DiagSnPrintf>
2005eb6e:	842a                	c.mv	s0,a0
2005eb70:	04957163          	bgeu	a0,s1,2005ebb2 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005eb74:	200646b7          	lui	a3,0x20064
2005eb78:	20064637          	lui	a2,0x20064
2005eb7c:	40a485b3          	sub	a1,s1,a0
2005eb80:	89068693          	addi	a3,a3,-1904 # 20063890 <__FUNCTION__.0+0x180>
2005eb84:	8a460613          	addi	a2,a2,-1884 # 200638a4 <__FUNCTION__.0+0x194>
2005eb88:	954e                	c.add	a0,s3
2005eb8a:	1c3000ef          	jal	ra,2005f54c <DiagSnPrintf>
2005eb8e:	942a                	c.add	s0,a0
2005eb90:	02947163          	bgeu	s0,s1,2005ebb2 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005eb94:	200646b7          	lui	a3,0x20064
2005eb98:	20064637          	lui	a2,0x20064
2005eb9c:	408485b3          	sub	a1,s1,s0
2005eba0:	00898533          	add	a0,s3,s0
2005eba4:	8b868693          	addi	a3,a3,-1864 # 200638b8 <__FUNCTION__.0+0x1a8>
2005eba8:	8f860613          	addi	a2,a2,-1800 # 200638f8 <__FUNCTION__.0+0x1e8>
2005ebac:	1a1000ef          	jal	ra,2005f54c <DiagSnPrintf>
2005ebb0:	942a                	c.add	s0,a0
2005ebb2:	20065937          	lui	s2,0x20065
2005ebb6:	20065a37          	lui	s4,0x20065
2005ebba:	d1c90913          	addi	s2,s2,-740 # 20064d1c <lib_pmc_git_rev>
2005ebbe:	db1a0a13          	addi	s4,s4,-591 # 20064db1 <__git_ver_table_end__>
2005ebc2:	20064ab7          	lui	s5,0x20064
2005ebc6:	20063b37          	lui	s6,0x20063
2005ebca:	03496163          	bltu	s2,s4,2005ebec <ChipInfo_GetLibVersion_ToBuf+0xbe>
2005ebce:	40f2                	c.lwsp	ra,28(sp)
2005ebd0:	4462                	c.lwsp	s0,24(sp)
2005ebd2:	44d2                	c.lwsp	s1,20(sp)
2005ebd4:	4942                	c.lwsp	s2,16(sp)
2005ebd6:	49b2                	c.lwsp	s3,12(sp)
2005ebd8:	4a22                	c.lwsp	s4,8(sp)
2005ebda:	4a92                	c.lwsp	s5,4(sp)
2005ebdc:	4b02                	c.lwsp	s6,0(sp)
2005ebde:	6105                	c.addi16sp	sp,32
2005ebe0:	8082                	c.jr	ra
2005ebe2:	200646b7          	lui	a3,0x20064
2005ebe6:	87868693          	addi	a3,a3,-1928 # 20063878 <__FUNCTION__.0+0x168>
2005ebea:	bf95                	c.j	2005eb5e <ChipInfo_GetLibVersion_ToBuf+0x30>
2005ebec:	00094683          	lbu	a3,0(s2)
2005ebf0:	e28d                	c.bnez	a3,2005ec12 <ChipInfo_GetLibVersion_ToBuf+0xe4>
2005ebf2:	00194783          	lbu	a5,1(s2)
2005ebf6:	0905                	c.addi	s2,1
2005ebf8:	dfed                	c.beqz	a5,2005ebf2 <ChipInfo_GetLibVersion_ToBuf+0xc4>
2005ebfa:	fc9478e3          	bgeu	s0,s1,2005ebca <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ebfe:	8b8b0613          	addi	a2,s6,-1864 # 200628b8 <pmap_func+0x4ac>
2005ec02:	408485b3          	sub	a1,s1,s0
2005ec06:	00898533          	add	a0,s3,s0
2005ec0a:	143000ef          	jal	ra,2005f54c <DiagSnPrintf>
2005ec0e:	942a                	c.add	s0,a0
2005ec10:	bf6d                	c.j	2005ebca <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ec12:	fa947ce3          	bgeu	s0,s1,2005ebca <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ec16:	908a8613          	addi	a2,s5,-1784 # 20063908 <__FUNCTION__.0+0x1f8>
2005ec1a:	408485b3          	sub	a1,s1,s0
2005ec1e:	00898533          	add	a0,s3,s0
2005ec22:	0905                	c.addi	s2,1
2005ec24:	129000ef          	jal	ra,2005f54c <DiagSnPrintf>
2005ec28:	b7dd                	c.j	2005ec0e <ChipInfo_GetLibVersion_ToBuf+0xe0>

2005ec2a <ChipInfo_GetChipSram>:
2005ec2a:	1101                	c.addi	sp,-32
2005ec2c:	ce06                	c.swsp	ra,28(sp)
2005ec2e:	4100c7b7          	lui	a5,0x4100c
2005ec32:	2107a783          	lw	a5,528(a5) # 4100c210 <__km4_bd_boot_download_addr__+0x10ffa210>
2005ec36:	40400737          	lui	a4,0x40400
2005ec3a:	8ff9                	c.and	a5,a4
2005ec3c:	c39d                	c.beqz	a5,2005ec62 <ChipInfo_GetChipSram+0x38>
2005ec3e:	00f10593          	addi	a1,sp,15
2005ec42:	7fc00513          	addi	a0,zero,2044
2005ec46:	857fa0ef          	jal	ra,2005949c <OTP_Read8>
2005ec4a:	00f14783          	lbu	a5,15(sp)
2005ec4e:	470d                	c.li	a4,3
2005ec50:	8389                	c.srli	a5,0x2
2005ec52:	8b8d                	c.andi	a5,3
2005ec54:	00e79663          	bne	a5,a4,2005ec60 <ChipInfo_GetChipSram+0x36>
2005ec58:	4501                	c.li	a0,0
2005ec5a:	40f2                	c.lwsp	ra,28(sp)
2005ec5c:	6105                	c.addi16sp	sp,32
2005ec5e:	8082                	c.jr	ra
2005ec60:	dfe5                	c.beqz	a5,2005ec58 <ChipInfo_GetChipSram+0x2e>
2005ec62:	00f10593          	addi	a1,sp,15
2005ec66:	7fc00513          	addi	a0,zero,2044
2005ec6a:	833fa0ef          	jal	ra,2005949c <OTP_Read8>
2005ec6e:	00f14783          	lbu	a5,15(sp)
2005ec72:	470d                	c.li	a4,3
2005ec74:	8399                	c.srli	a5,0x6
2005ec76:	00e78363          	beq	a5,a4,2005ec7c <ChipInfo_GetChipSram+0x52>
2005ec7a:	fff9                	c.bnez	a5,2005ec58 <ChipInfo_GetChipSram+0x2e>
2005ec7c:	4505                	c.li	a0,1
2005ec7e:	bff1                	c.j	2005ec5a <ChipInfo_GetChipSram+0x30>

2005ec80 <vTaskStatus>:
2005ec80:	7139                	c.addi16sp	sp,-64
2005ec82:	de06                	c.swsp	ra,60(sp)
2005ec84:	dc22                	c.swsp	s0,56(sp)
2005ec86:	da26                	c.swsp	s1,52(sp)
2005ec88:	f7bfe0ef          	jal	ra,2005dc02 <vTaskEnterCritical>
2005ec8c:	d8ffe0ef          	jal	ra,2005da1a <xTaskGetCurrentTaskHandle>
2005ec90:	4695                	c.li	a3,5
2005ec92:	4605                	c.li	a2,1
2005ec94:	006c                	c.addi4spn	a1,sp,12
2005ec96:	842a                	c.mv	s0,a0
2005ec98:	200644b7          	lui	s1,0x20064
2005ec9c:	c8eff0ef          	jal	ra,2005e12a <vTaskGetInfo>
2005eca0:	90c48513          	addi	a0,s1,-1780 # 2006390c <__FUNCTION__.0+0x1fc>
2005eca4:	25e9                	c.jal	2005f36e <DiagPrintf>
2005eca6:	45c2                	c.lwsp	a1,16(sp)
2005eca8:	20064537          	lui	a0,0x20064
2005ecac:	93050513          	addi	a0,a0,-1744 # 20063930 <__FUNCTION__.0+0x220>
2005ecb0:	2d7d                	c.jal	2005f36e <DiagPrintf>
2005ecb2:	45d2                	c.lwsp	a1,20(sp)
2005ecb4:	20064537          	lui	a0,0x20064
2005ecb8:	94450513          	addi	a0,a0,-1724 # 20063944 <__FUNCTION__.0+0x234>
2005ecbc:	2d4d                	c.jal	2005f36e <DiagPrintf>
2005ecbe:	20064537          	lui	a0,0x20064
2005ecc2:	85a2                	c.mv	a1,s0
2005ecc4:	95450513          	addi	a0,a0,-1708 # 20063954 <__FUNCTION__.0+0x244>
2005ecc8:	255d                	c.jal	2005f36e <DiagPrintf>
2005ecca:	01814583          	lbu	a1,24(sp)
2005ecce:	200647b7          	lui	a5,0x20064
2005ecd2:	a7878793          	addi	a5,a5,-1416 # 20063a78 <TaskStateString.0>
2005ecd6:	00259713          	slli	a4,a1,0x2
2005ecda:	97ba                	c.add	a5,a4
2005ecdc:	4390                	c.lw	a2,0(a5)
2005ecde:	20064537          	lui	a0,0x20064
2005ece2:	96850513          	addi	a0,a0,-1688 # 20063968 <__FUNCTION__.0+0x258>
2005ece6:	2561                	c.jal	2005f36e <DiagPrintf>
2005ece8:	5582                	c.lwsp	a1,32(sp)
2005ecea:	20064537          	lui	a0,0x20064
2005ecee:	98450513          	addi	a0,a0,-1660 # 20063984 <__FUNCTION__.0+0x274>
2005ecf2:	2db5                	c.jal	2005f36e <DiagPrintf>
2005ecf4:	45f2                	c.lwsp	a1,28(sp)
2005ecf6:	20064537          	lui	a0,0x20064
2005ecfa:	99850513          	addi	a0,a0,-1640 # 20063998 <__FUNCTION__.0+0x288>
2005ecfe:	2d85                	c.jal	2005f36e <DiagPrintf>
2005ed00:	5592                	c.lwsp	a1,36(sp)
2005ed02:	20064537          	lui	a0,0x20064
2005ed06:	9b050513          	addi	a0,a0,-1616 # 200639b0 <__FUNCTION__.0+0x2a0>
2005ed0a:	2595                	c.jal	2005f36e <DiagPrintf>
2005ed0c:	4474                	c.lw	a3,76(s0)
2005ed0e:	5818                	c.lw	a4,48(s0)
2005ed10:	5622                	c.lwsp	a2,40(sp)
2005ed12:	400c                	c.lw	a1,0(s0)
2005ed14:	40e68733          	sub	a4,a3,a4
2005ed18:	8709                	c.srai	a4,0x2
2005ed1a:	20064537          	lui	a0,0x20064
2005ed1e:	0705                	c.addi	a4,1
2005ed20:	9c850513          	addi	a0,a0,-1592 # 200639c8 <__FUNCTION__.0+0x2b8>
2005ed24:	25a9                	c.jal	2005f36e <DiagPrintf>
2005ed26:	02c15583          	lhu	a1,44(sp)
2005ed2a:	20064537          	lui	a0,0x20064
2005ed2e:	a1850513          	addi	a0,a0,-1512 # 20063a18 <__FUNCTION__.0+0x308>
2005ed32:	2d35                	c.jal	2005f36e <DiagPrintf>
2005ed34:	90c48513          	addi	a0,s1,-1780
2005ed38:	2d1d                	c.jal	2005f36e <DiagPrintf>
2005ed3a:	50f2                	c.lwsp	ra,60(sp)
2005ed3c:	5462                	c.lwsp	s0,56(sp)
2005ed3e:	54d2                	c.lwsp	s1,52(sp)
2005ed40:	6121                	c.addi16sp	sp,64
2005ed42:	8082                	c.jr	ra

2005ed44 <vTaskCrashCallback>:
2005ed44:	1141                	c.addi	sp,-16
2005ed46:	c606                	c.swsp	ra,12(sp)
2005ed48:	e3dfc0ef          	jal	ra,2005bb84 <rtos_sched_get_state>
2005ed4c:	4785                	c.li	a5,1
2005ed4e:	00f50663          	beq	a0,a5,2005ed5a <vTaskCrashCallback+0x16>
2005ed52:	40b2                	c.lwsp	ra,12(sp)
2005ed54:	0141                	c.addi	sp,16
2005ed56:	f2bff06f          	jal	zero,2005ec80 <vTaskStatus>
2005ed5a:	40b2                	c.lwsp	ra,12(sp)
2005ed5c:	0141                	c.addi	sp,16
2005ed5e:	8082                	c.jr	ra

2005ed60 <print_unsigned_num>:
2005ed60:	715d                	c.addi16sp	sp,-80
2005ed62:	c4a2                	c.swsp	s0,72(sp)
2005ed64:	c2a6                	c.swsp	s1,68(sp)
2005ed66:	c0ca                	c.swsp	s2,64(sp)
2005ed68:	de4e                	c.swsp	s3,60(sp)
2005ed6a:	dc52                	c.swsp	s4,56(sp)
2005ed6c:	da56                	c.swsp	s5,52(sp)
2005ed6e:	d85a                	c.swsp	s6,48(sp)
2005ed70:	d65e                	c.swsp	s7,44(sp)
2005ed72:	d462                	c.swsp	s8,40(sp)
2005ed74:	d266                	c.swsp	s9,36(sp)
2005ed76:	d06a                	c.swsp	s10,32(sp)
2005ed78:	c686                	c.swsp	ra,76(sp)
2005ed7a:	8c2a                	c.mv	s8,a0
2005ed7c:	8cae                	c.mv	s9,a1
2005ed7e:	8d32                	c.mv	s10,a2
2005ed80:	8936                	c.mv	s2,a3
2005ed82:	8bba                	c.mv	s7,a4
2005ed84:	89be                	c.mv	s3,a5
2005ed86:	84c2                	c.mv	s1,a6
2005ed88:	4401                	c.li	s0,0
2005ed8a:	4a81                	c.li	s5,0
2005ed8c:	4a25                	c.li	s4,9
2005ed8e:	4b05                	c.li	s6,1
2005ed90:	866a                	c.mv	a2,s10
2005ed92:	4681                	c.li	a3,0
2005ed94:	8562                	c.mv	a0,s8
2005ed96:	85e6                	c.mv	a1,s9
2005ed98:	2a1020ef          	jal	ra,20061838 <__umoddi3>
2005ed9c:	0ff57893          	andi	a7,a0,255
2005eda0:	04aa6863          	bltu	s4,a0,2005edf0 <print_unsigned_num+0x90>
2005eda4:	03088893          	addi	a7,a7,48 # f8000030 <__ctrace_end__+0x2e000030>
2005eda8:	007c                	c.addi4spn	a5,sp,12
2005edaa:	97a2                	c.add	a5,s0
2005edac:	0ff8f893          	andi	a7,a7,255
2005edb0:	01178023          	sb	a7,0(a5)
2005edb4:	866a                	c.mv	a2,s10
2005edb6:	4681                	c.li	a3,0
2005edb8:	8562                	c.mv	a0,s8
2005edba:	85e6                	c.mv	a1,s9
2005edbc:	0405                	c.addi	s0,1
2005edbe:	71a020ef          	jal	ra,200614d8 <__udivdi3>
2005edc2:	039a9463          	bne	s5,s9,2005edea <print_unsigned_num+0x8a>
2005edc6:	03ac7263          	bgeu	s8,s10,2005edea <print_unsigned_num+0x8a>
2005edca:	c8d5                	c.beqz	s1,2005ee7e <print_unsigned_num+0x11e>
2005edcc:	03000793          	addi	a5,zero,48
2005edd0:	1bfd                	c.addi	s7,-1
2005edd2:	0af91163          	bne	s2,a5,2005ee74 <print_unsigned_num+0x114>
2005edd6:	02d00513          	addi	a0,zero,45
2005edda:	dffa5097          	auipc	ra,0xdffa5
2005edde:	76a080e7          	jalr	ra,1898(ra) # 4544 <LOGUART_PutChar>
2005ede2:	89a6                	c.mv	s3,s1
2005ede4:	09704b63          	blt	zero,s7,2005ee7a <print_unsigned_num+0x11a>
2005ede8:	a0b1                	c.j	2005ee34 <print_unsigned_num+0xd4>
2005edea:	8c2a                	c.mv	s8,a0
2005edec:	8cae                	c.mv	s9,a1
2005edee:	b74d                	c.j	2005ed90 <print_unsigned_num+0x30>
2005edf0:	01699563          	bne	s3,s6,2005edfa <print_unsigned_num+0x9a>
2005edf4:	03788893          	addi	a7,a7,55
2005edf8:	bf45                	c.j	2005eda8 <print_unsigned_num+0x48>
2005edfa:	05788893          	addi	a7,a7,87
2005edfe:	b76d                	c.j	2005eda8 <print_unsigned_num+0x48>
2005ee00:	854a                	c.mv	a0,s2
2005ee02:	dffa5097          	auipc	ra,0xdffa5
2005ee06:	742080e7          	jalr	ra,1858(ra) # 4544 <LOGUART_PutChar>
2005ee0a:	1a7d                	c.addi	s4,-1
2005ee0c:	ff444ae3          	blt	s0,s4,2005ee00 <print_unsigned_num+0xa0>
2005ee10:	4781                	c.li	a5,0
2005ee12:	008bc463          	blt	s7,s0,2005ee1a <print_unsigned_num+0xba>
2005ee16:	408b87b3          	sub	a5,s7,s0
2005ee1a:	97ce                	c.add	a5,s3
2005ee1c:	c0a9                	c.beqz	s1,2005ee5e <print_unsigned_num+0xfe>
2005ee1e:	84be                	c.mv	s1,a5
2005ee20:	0df97913          	andi	s2,s2,223
2005ee24:	00091863          	bne	s2,zero,2005ee34 <print_unsigned_num+0xd4>
2005ee28:	02d00513          	addi	a0,zero,45
2005ee2c:	dffa5097          	auipc	ra,0xdffa5
2005ee30:	718080e7          	jalr	ra,1816(ra) # 4544 <LOGUART_PutChar>
2005ee34:	8922                	c.mv	s2,s0
2005ee36:	59fd                	c.li	s3,-1
2005ee38:	197d                	c.addi	s2,-1
2005ee3a:	03391463          	bne	s2,s3,2005ee62 <print_unsigned_num+0x102>
2005ee3e:	00940533          	add	a0,s0,s1
2005ee42:	40b6                	c.lwsp	ra,76(sp)
2005ee44:	4426                	c.lwsp	s0,72(sp)
2005ee46:	4496                	c.lwsp	s1,68(sp)
2005ee48:	4906                	c.lwsp	s2,64(sp)
2005ee4a:	59f2                	c.lwsp	s3,60(sp)
2005ee4c:	5a62                	c.lwsp	s4,56(sp)
2005ee4e:	5ad2                	c.lwsp	s5,52(sp)
2005ee50:	5b42                	c.lwsp	s6,48(sp)
2005ee52:	5bb2                	c.lwsp	s7,44(sp)
2005ee54:	5c22                	c.lwsp	s8,40(sp)
2005ee56:	5c92                	c.lwsp	s9,36(sp)
2005ee58:	5d02                	c.lwsp	s10,32(sp)
2005ee5a:	6161                	c.addi16sp	sp,80
2005ee5c:	8082                	c.jr	ra
2005ee5e:	84be                	c.mv	s1,a5
2005ee60:	bfd1                	c.j	2005ee34 <print_unsigned_num+0xd4>
2005ee62:	007c                	c.addi4spn	a5,sp,12
2005ee64:	97ca                	c.add	a5,s2
2005ee66:	0007c503          	lbu	a0,0(a5)
2005ee6a:	dffa5097          	auipc	ra,0xdffa5
2005ee6e:	6da080e7          	jalr	ra,1754(ra) # 4544 <LOGUART_PutChar>
2005ee72:	b7d9                	c.j	2005ee38 <print_unsigned_num+0xd8>
2005ee74:	fb7056e3          	bge	zero,s7,2005ee20 <print_unsigned_num+0xc0>
2005ee78:	89a6                	c.mv	s3,s1
2005ee7a:	8a5e                	c.mv	s4,s7
2005ee7c:	bf41                	c.j	2005ee0c <print_unsigned_num+0xac>
2005ee7e:	4981                	c.li	s3,0
2005ee80:	b795                	c.j	2005ede4 <print_unsigned_num+0x84>

2005ee82 <pad_char_control>:
2005ee82:	1141                	c.addi	sp,-16
2005ee84:	c422                	c.swsp	s0,8(sp)
2005ee86:	c226                	c.swsp	s1,4(sp)
2005ee88:	c04a                	c.swsp	s2,0(sp)
2005ee8a:	c606                	c.swsp	ra,12(sp)
2005ee8c:	842a                	c.mv	s0,a0
2005ee8e:	892e                	c.mv	s2,a1
2005ee90:	84aa                	c.mv	s1,a0
2005ee92:	00904c63          	blt	zero,s1,2005eeaa <pad_char_control+0x28>
2005ee96:	8522                	c.mv	a0,s0
2005ee98:	00045363          	bge	s0,zero,2005ee9e <pad_char_control+0x1c>
2005ee9c:	4501                	c.li	a0,0
2005ee9e:	40b2                	c.lwsp	ra,12(sp)
2005eea0:	4422                	c.lwsp	s0,8(sp)
2005eea2:	4492                	c.lwsp	s1,4(sp)
2005eea4:	4902                	c.lwsp	s2,0(sp)
2005eea6:	0141                	c.addi	sp,16
2005eea8:	8082                	c.jr	ra
2005eeaa:	854a                	c.mv	a0,s2
2005eeac:	dffa5097          	auipc	ra,0xdffa5
2005eeb0:	698080e7          	jalr	ra,1688(ra) # 4544 <LOGUART_PutChar>
2005eeb4:	14fd                	c.addi	s1,-1
2005eeb6:	bff1                	c.j	2005ee92 <pad_char_control+0x10>

2005eeb8 <print_decimal_num>:
2005eeb8:	1101                	c.addi	sp,-32
2005eeba:	c64e                	c.swsp	s3,12(sp)
2005eebc:	200689b7          	lui	s3,0x20068
2005eec0:	cc22                	c.swsp	s0,24(sp)
2005eec2:	c84a                	c.swsp	s2,16(sp)
2005eec4:	c452                	c.swsp	s4,8(sp)
2005eec6:	c05a                	c.swsp	s6,0(sp)
2005eec8:	8936                	c.mv	s2,a3
2005eeca:	ce06                	c.swsp	ra,28(sp)
2005eecc:	ca26                	c.swsp	s1,20(sp)
2005eece:	c256                	c.swsp	s5,4(sp)
2005eed0:	882a                	c.mv	a6,a0
2005eed2:	8b32                	c.mv	s6,a2
2005eed4:	8a3a                	c.mv	s4,a4
2005eed6:	4401                	c.li	s0,0
2005eed8:	46a5                	c.li	a3,9
2005eeda:	6a898993          	addi	s3,s3,1704 # 200686a8 <num_buf_temp>
2005eede:	02b87733          	remu	a4,a6,a1
2005eee2:	0ff77793          	andi	a5,a4,255
2005eee6:	04e6ee63          	bltu	a3,a4,2005ef42 <print_decimal_num+0x8a>
2005eeea:	03078793          	addi	a5,a5,48
2005eeee:	0ff7f793          	andi	a5,a5,255
2005eef2:	00898733          	add	a4,s3,s0
2005eef6:	00f70023          	sb	a5,0(a4) # 40400000 <__km4_bd_boot_download_addr__+0x103ee000>
2005eefa:	00140493          	addi	s1,s0,1
2005eefe:	02b857b3          	divu	a5,a6,a1
2005ef02:	02b87d63          	bgeu	a6,a1,2005ef3c <print_decimal_num+0x84>
2005ef06:	8aca                	c.mv	s5,s2
2005ef08:	05204663          	blt	zero,s2,2005ef54 <print_decimal_num+0x9c>
2005ef0c:	4901                	c.li	s2,0
2005ef0e:	02d00513          	addi	a0,zero,45
2005ef12:	040a1e63          	bne	s4,zero,2005ef6e <print_decimal_num+0xb6>
2005ef16:	14fd                	c.addi	s1,-1
2005ef18:	0404d763          	bge	s1,zero,2005ef66 <print_decimal_num+0xae>
2005ef1c:	00045363          	bge	s0,zero,2005ef22 <print_decimal_num+0x6a>
2005ef20:	547d                	c.li	s0,-1
2005ef22:	0405                	c.addi	s0,1
2005ef24:	01240533          	add	a0,s0,s2
2005ef28:	40f2                	c.lwsp	ra,28(sp)
2005ef2a:	4462                	c.lwsp	s0,24(sp)
2005ef2c:	44d2                	c.lwsp	s1,20(sp)
2005ef2e:	4942                	c.lwsp	s2,16(sp)
2005ef30:	49b2                	c.lwsp	s3,12(sp)
2005ef32:	4a22                	c.lwsp	s4,8(sp)
2005ef34:	4a92                	c.lwsp	s5,4(sp)
2005ef36:	4b02                	c.lwsp	s6,0(sp)
2005ef38:	6105                	c.addi16sp	sp,32
2005ef3a:	8082                	c.jr	ra
2005ef3c:	8426                	c.mv	s0,s1
2005ef3e:	883e                	c.mv	a6,a5
2005ef40:	bf79                	c.j	2005eede <print_decimal_num+0x26>
2005ef42:	05778793          	addi	a5,a5,87
2005ef46:	b765                	c.j	2005eeee <print_decimal_num+0x36>
2005ef48:	855a                	c.mv	a0,s6
2005ef4a:	dffa5097          	auipc	ra,0xdffa5
2005ef4e:	5fa080e7          	jalr	ra,1530(ra) # 4544 <LOGUART_PutChar>
2005ef52:	1afd                	c.addi	s5,-1
2005ef54:	ff54cae3          	blt	s1,s5,2005ef48 <print_decimal_num+0x90>
2005ef58:	4781                	c.li	a5,0
2005ef5a:	00994463          	blt	s2,s1,2005ef62 <print_decimal_num+0xaa>
2005ef5e:	409907b3          	sub	a5,s2,s1
2005ef62:	893e                	c.mv	s2,a5
2005ef64:	b76d                	c.j	2005ef0e <print_decimal_num+0x56>
2005ef66:	009987b3          	add	a5,s3,s1
2005ef6a:	0007c503          	lbu	a0,0(a5)
2005ef6e:	dffa5097          	auipc	ra,0xdffa5
2005ef72:	5d6080e7          	jalr	ra,1494(ra) # 4544 <LOGUART_PutChar>
2005ef76:	b745                	c.j	2005ef16 <print_decimal_num+0x5e>

2005ef78 <pad_num_control>:
2005ef78:	04088163          	beq	a7,zero,2005efba <pad_num_control+0x42>
2005ef7c:	1141                	c.addi	sp,-16
2005ef7e:	c226                	c.swsp	s1,4(sp)
2005ef80:	84ba                	c.mv	s1,a4
2005ef82:	4701                	c.li	a4,0
2005ef84:	c422                	c.swsp	s0,8(sp)
2005ef86:	c04a                	c.swsp	s2,0(sp)
2005ef88:	c606                	c.swsp	ra,12(sp)
2005ef8a:	8936                	c.mv	s2,a3
2005ef8c:	dd5ff0ef          	jal	ra,2005ed60 <print_unsigned_num>
2005ef90:	842a                	c.mv	s0,a0
2005ef92:	00955d63          	bge	a0,s1,2005efac <pad_num_control+0x34>
2005ef96:	02000793          	addi	a5,zero,32
2005ef9a:	00f91963          	bne	s2,a5,2005efac <pad_num_control+0x34>
2005ef9e:	02000593          	addi	a1,zero,32
2005efa2:	40a48533          	sub	a0,s1,a0
2005efa6:	eddff0ef          	jal	ra,2005ee82 <pad_char_control>
2005efaa:	942a                	c.add	s0,a0
2005efac:	40b2                	c.lwsp	ra,12(sp)
2005efae:	8522                	c.mv	a0,s0
2005efb0:	4422                	c.lwsp	s0,8(sp)
2005efb2:	4492                	c.lwsp	s1,4(sp)
2005efb4:	4902                	c.lwsp	s2,0(sp)
2005efb6:	0141                	c.addi	sp,16
2005efb8:	8082                	c.jr	ra
2005efba:	da7ff06f          	jal	zero,2005ed60 <print_unsigned_num>

2005efbe <_rand>:
2005efbe:	1141                	c.addi	sp,-16
2005efc0:	450d                	c.li	a0,3
2005efc2:	c606                	c.swsp	ra,12(sp)
2005efc4:	f2afa0ef          	jal	ra,200596ee <PLL_State>
2005efc8:	c50d                	c.beqz	a0,2005eff2 <_rand+0x34>
2005efca:	65c1                	c.lui	a1,0x10
2005efcc:	410167b7          	lui	a5,0x41016
2005efd0:	453d                	c.li	a0,15
2005efd2:	4685                	c.li	a3,1
2005efd4:	00158713          	addi	a4,a1,1 # 10001 <mbedtls_ecp_check_pub_priv+0x123>
2005efd8:	47d0                	c.lw	a2,12(a5)
2005efda:	00a61763          	bne	a2,a0,2005efe8 <_rand+0x2a>
2005efde:	df94                	c.sw	a3,56(a5)
2005efe0:	177d                	c.addi	a4,-1
2005efe2:	fb7d                	c.bnez	a4,2005efd8 <_rand+0x1a>
2005efe4:	df94                	c.sw	a3,56(a5)
2005efe6:	b7fd                	c.j	2005efd4 <_rand+0x16>
2005efe8:	de65                	c.beqz	a2,2005efe0 <_rand+0x22>
2005efea:	4b88                	c.lw	a0,16(a5)
2005efec:	40b2                	c.lwsp	ra,12(sp)
2005efee:	0141                	c.addi	sp,16
2005eff0:	8082                	c.jr	ra
2005eff2:	fb018693          	addi	a3,gp,-80 # 20066938 <rand_first.1>
2005eff6:	429c                	c.lw	a5,0(a3)
2005eff8:	20068737          	lui	a4,0x20068
2005effc:	6b470713          	addi	a4,a4,1716 # 200686b4 <rand_seed.0>
2005f000:	eb91                	c.bnez	a5,2005f014 <_rand+0x56>
2005f002:	678d                	c.lui	a5,0x3
2005f004:	03978793          	addi	a5,a5,57 # 3039 <FLASH_SetSpiMode+0x375>
2005f008:	c31c                	c.sw	a5,0(a4)
2005f00a:	c35c                	c.sw	a5,4(a4)
2005f00c:	c71c                	c.sw	a5,8(a4)
2005f00e:	c75c                	c.sw	a5,12(a4)
2005f010:	4785                	c.li	a5,1
2005f012:	c29c                	c.sw	a5,0(a3)
2005f014:	4314                	c.lw	a3,0(a4)
2005f016:	fff80537          	lui	a0,0xfff80
2005f01a:	474c                	c.lw	a1,12(a4)
2005f01c:	00669793          	slli	a5,a3,0x6
2005f020:	8fb5                	c.xor	a5,a3
2005f022:	06ca                	c.slli	a3,0x12
2005f024:	8ee9                	c.and	a3,a0
2005f026:	83b5                	c.srli	a5,0xd
2005f028:	8fb5                	c.xor	a5,a3
2005f02a:	4354                	c.lw	a3,4(a4)
2005f02c:	fff00837          	lui	a6,0xfff00
2005f030:	c31c                	c.sw	a5,0(a4)
2005f032:	00269613          	slli	a2,a3,0x2
2005f036:	00c6c533          	xor	a0,a3,a2
2005f03a:	4714                	c.lw	a3,8(a4)
2005f03c:	9a01                	c.andi	a2,-32
2005f03e:	816d                	c.srli	a0,0x1b
2005f040:	8d31                	c.xor	a0,a2
2005f042:	00d69613          	slli	a2,a3,0xd
2005f046:	8e35                	c.xor	a2,a3
2005f048:	069e                	c.slli	a3,0x7
2005f04a:	8006f693          	andi	a3,a3,-2048
2005f04e:	8255                	c.srli	a2,0x15
2005f050:	8e35                	c.xor	a2,a3
2005f052:	00359693          	slli	a3,a1,0x3
2005f056:	8ead                	c.xor	a3,a1
2005f058:	05b6                	c.slli	a1,0xd
2005f05a:	c348                	c.sw	a0,4(a4)
2005f05c:	82b1                	c.srli	a3,0xc
2005f05e:	0105f5b3          	and	a1,a1,a6
2005f062:	8d3d                	c.xor	a0,a5
2005f064:	8ead                	c.xor	a3,a1
2005f066:	8d31                	c.xor	a0,a2
2005f068:	c710                	c.sw	a2,8(a4)
2005f06a:	c754                	c.sw	a3,12(a4)
2005f06c:	8d35                	c.xor	a0,a3
2005f06e:	bfbd                	c.j	2005efec <_rand+0x2e>

2005f070 <DiagVprintf>:
2005f070:	7139                	c.addi16sp	sp,-64
2005f072:	d452                	c.swsp	s4,40(sp)
2005f074:	20064a37          	lui	s4,0x20064
2005f078:	dc22                	c.swsp	s0,56(sp)
2005f07a:	da26                	c.swsp	s1,52(sp)
2005f07c:	d84a                	c.swsp	s2,48(sp)
2005f07e:	d64e                	c.swsp	s3,44(sp)
2005f080:	d256                	c.swsp	s5,36(sp)
2005f082:	d05a                	c.swsp	s6,32(sp)
2005f084:	de06                	c.swsp	ra,60(sp)
2005f086:	ce5e                	c.swsp	s7,28(sp)
2005f088:	cc62                	c.swsp	s8,24(sp)
2005f08a:	ca66                	c.swsp	s9,20(sp)
2005f08c:	892a                	c.mv	s2,a0
2005f08e:	842e                	c.mv	s0,a1
2005f090:	4981                	c.li	s3,0
2005f092:	4481                	c.li	s1,0
2005f094:	a90a0a13          	addi	s4,s4,-1392 # 20063a90 <TaskStateString.0+0x18>
2005f098:	4aa5                	c.li	s5,9
2005f09a:	4b29                	c.li	s6,10
2005f09c:	00094503          	lbu	a0,0(s2)
2005f0a0:	ed19                	c.bnez	a0,2005f0be <DiagVprintf+0x4e>
2005f0a2:	50f2                	c.lwsp	ra,60(sp)
2005f0a4:	5462                	c.lwsp	s0,56(sp)
2005f0a6:	5942                	c.lwsp	s2,48(sp)
2005f0a8:	59b2                	c.lwsp	s3,44(sp)
2005f0aa:	5a22                	c.lwsp	s4,40(sp)
2005f0ac:	5a92                	c.lwsp	s5,36(sp)
2005f0ae:	5b02                	c.lwsp	s6,32(sp)
2005f0b0:	4bf2                	c.lwsp	s7,28(sp)
2005f0b2:	4c62                	c.lwsp	s8,24(sp)
2005f0b4:	4cd2                	c.lwsp	s9,20(sp)
2005f0b6:	8526                	c.mv	a0,s1
2005f0b8:	54d2                	c.lwsp	s1,52(sp)
2005f0ba:	6121                	c.addi16sp	sp,64
2005f0bc:	8082                	c.jr	ra
2005f0be:	02500793          	addi	a5,zero,37
2005f0c2:	0905                	c.addi	s2,1
2005f0c4:	28f51f63          	bne	a0,a5,2005f362 <DiagVprintf+0x2f2>
2005f0c8:	4881                	c.li	a7,0
2005f0ca:	4701                	c.li	a4,0
2005f0cc:	4681                	c.li	a3,0
2005f0ce:	4601                	c.li	a2,0
2005f0d0:	07a00e13          	addi	t3,zero,122
2005f0d4:	05700e93          	addi	t4,zero,87
2005f0d8:	02200f13          	addi	t5,zero,34
2005f0dc:	03000593          	addi	a1,zero,48
2005f0e0:	04300813          	addi	a6,zero,67
2005f0e4:	04c00f93          	addi	t6,zero,76
2005f0e8:	03900293          	addi	t0,zero,57
2005f0ec:	02000393          	addi	t2,zero,32
2005f0f0:	02500313          	addi	t1,zero,37
2005f0f4:	02d00b93          	addi	s7,zero,45
2005f0f8:	00094783          	lbu	a5,0(s2)
2005f0fc:	02fe6063          	bltu	t3,a5,2005f11c <DiagVprintf+0xac>
2005f100:	02fee063          	bltu	t4,a5,2005f120 <DiagVprintf+0xb0>
2005f104:	22b78663          	beq	a5,a1,2005f330 <DiagVprintf+0x2c0>
2005f108:	02f5eb63          	bltu	a1,a5,2005f13e <DiagVprintf+0xce>
2005f10c:	20678963          	beq	a5,t1,2005f31e <DiagVprintf+0x2ae>
2005f110:	02f36263          	bltu	t1,a5,2005f134 <DiagVprintf+0xc4>
2005f114:	20078c63          	beq	a5,zero,2005f32c <DiagVprintf+0x2bc>
2005f118:	24778163          	beq	a5,t2,2005f35a <DiagVprintf+0x2ea>
2005f11c:	54fd                	c.li	s1,-1
2005f11e:	b751                	c.j	2005f0a2 <DiagVprintf+0x32>
2005f120:	fa878793          	addi	a5,a5,-88
2005f124:	0ff7f793          	andi	a5,a5,255
2005f128:	feff6ae3          	bltu	t5,a5,2005f11c <DiagVprintf+0xac>
2005f12c:	078a                	c.slli	a5,0x2
2005f12e:	97d2                	c.add	a5,s4
2005f130:	439c                	c.lw	a5,0(a5)
2005f132:	8782                	c.jr	a5
2005f134:	ff7794e3          	bne	a5,s7,2005f11c <DiagVprintf+0xac>
2005f138:	0905                	c.addi	s2,1
2005f13a:	0885                	c.addi	a7,1
2005f13c:	bf75                	c.j	2005f0f8 <DiagVprintf+0x88>
2005f13e:	19078363          	beq	a5,a6,2005f2c4 <DiagVprintf+0x254>
2005f142:	02f86563          	bltu	a6,a5,2005f16c <DiagVprintf+0xfc>
2005f146:	20f2fb63          	bgeu	t0,a5,2005f35c <DiagVprintf+0x2ec>
2005f14a:	04200593          	addi	a1,zero,66
2005f14e:	fcb797e3          	bne	a5,a1,2005f11c <DiagVprintf+0xac>
2005f152:	4785                	c.li	a5,1
2005f154:	0ac7d063          	bge	a5,a2,2005f1f4 <DiagVprintf+0x184>
2005f158:	041d                	c.addi	s0,7
2005f15a:	9861                	c.andi	s0,-8
2005f15c:	4008                	c.lw	a0,0(s0)
2005f15e:	404c                	c.lw	a1,4(s0)
2005f160:	00840b93          	addi	s7,s0,8
2005f164:	4801                	c.li	a6,0
2005f166:	87ce                	c.mv	a5,s3
2005f168:	4609                	c.li	a2,2
2005f16a:	a889                	c.j	2005f1bc <DiagVprintf+0x14c>
2005f16c:	1bf78663          	beq	a5,t6,2005f318 <DiagVprintf+0x2a8>
2005f170:	04f00593          	addi	a1,zero,79
2005f174:	fab794e3          	bne	a5,a1,2005f11c <DiagVprintf+0xac>
2005f178:	4785                	c.li	a5,1
2005f17a:	08c7d263          	bge	a5,a2,2005f1fe <DiagVprintf+0x18e>
2005f17e:	041d                	c.addi	s0,7
2005f180:	9861                	c.andi	s0,-8
2005f182:	4008                	c.lw	a0,0(s0)
2005f184:	404c                	c.lw	a1,4(s0)
2005f186:	00840b93          	addi	s7,s0,8
2005f18a:	4801                	c.li	a6,0
2005f18c:	87ce                	c.mv	a5,s3
2005f18e:	4621                	c.li	a2,8
2005f190:	a035                	c.j	2005f1bc <DiagVprintf+0x14c>
2005f192:	4785                	c.li	a5,1
2005f194:	02c7da63          	bge	a5,a2,2005f1c8 <DiagVprintf+0x158>
2005f198:	041d                	c.addi	s0,7
2005f19a:	9861                	c.andi	s0,-8
2005f19c:	4008                	c.lw	a0,0(s0)
2005f19e:	404c                	c.lw	a1,4(s0)
2005f1a0:	00840b93          	addi	s7,s0,8
2005f1a4:	0405d163          	bge	a1,zero,2005f1e6 <DiagVprintf+0x176>
2005f1a8:	00a037b3          	sltu	a5,zero,a0
2005f1ac:	40b005b3          	sub	a1,zero,a1
2005f1b0:	40a00533          	sub	a0,zero,a0
2005f1b4:	8d9d                	c.sub	a1,a5
2005f1b6:	4805                	c.li	a6,1
2005f1b8:	87ce                	c.mv	a5,s3
2005f1ba:	4629                	c.li	a2,10
2005f1bc:	dbdff0ef          	jal	ra,2005ef78 <pad_num_control>
2005f1c0:	94aa                	c.add	s1,a0
2005f1c2:	0905                	c.addi	s2,1
2005f1c4:	845e                	c.mv	s0,s7
2005f1c6:	bdd9                	c.j	2005f09c <DiagVprintf+0x2c>
2005f1c8:	4008                	c.lw	a0,0(s0)
2005f1ca:	00440b93          	addi	s7,s0,4
2005f1ce:	41f55593          	srai	a1,a0,0x1f
2005f1d2:	bfc9                	c.j	2005f1a4 <DiagVprintf+0x134>
2005f1d4:	4785                	c.li	a5,1
2005f1d6:	00c7da63          	bge	a5,a2,2005f1ea <DiagVprintf+0x17a>
2005f1da:	041d                	c.addi	s0,7
2005f1dc:	9861                	c.andi	s0,-8
2005f1de:	4008                	c.lw	a0,0(s0)
2005f1e0:	404c                	c.lw	a1,4(s0)
2005f1e2:	00840b93          	addi	s7,s0,8
2005f1e6:	4801                	c.li	a6,0
2005f1e8:	bfc1                	c.j	2005f1b8 <DiagVprintf+0x148>
2005f1ea:	4008                	c.lw	a0,0(s0)
2005f1ec:	4581                	c.li	a1,0
2005f1ee:	00440b93          	addi	s7,s0,4
2005f1f2:	bfd5                	c.j	2005f1e6 <DiagVprintf+0x176>
2005f1f4:	4008                	c.lw	a0,0(s0)
2005f1f6:	4581                	c.li	a1,0
2005f1f8:	00440b93          	addi	s7,s0,4
2005f1fc:	b7a5                	c.j	2005f164 <DiagVprintf+0xf4>
2005f1fe:	4008                	c.lw	a0,0(s0)
2005f200:	4581                	c.li	a1,0
2005f202:	00440b93          	addi	s7,s0,4
2005f206:	b751                	c.j	2005f18a <DiagVprintf+0x11a>
2005f208:	4985                	c.li	s3,1
2005f20a:	4785                	c.li	a5,1
2005f20c:	00c7dc63          	bge	a5,a2,2005f224 <DiagVprintf+0x1b4>
2005f210:	041d                	c.addi	s0,7
2005f212:	9861                	c.andi	s0,-8
2005f214:	4008                	c.lw	a0,0(s0)
2005f216:	404c                	c.lw	a1,4(s0)
2005f218:	00840b93          	addi	s7,s0,8
2005f21c:	4801                	c.li	a6,0
2005f21e:	87ce                	c.mv	a5,s3
2005f220:	4641                	c.li	a2,16
2005f222:	bf69                	c.j	2005f1bc <DiagVprintf+0x14c>
2005f224:	4008                	c.lw	a0,0(s0)
2005f226:	4581                	c.li	a1,0
2005f228:	00440b93          	addi	s7,s0,4
2005f22c:	bfc5                	c.j	2005f21c <DiagVprintf+0x1ac>
2005f22e:	4008                	c.lw	a0,0(s0)
2005f230:	4801                	c.li	a6,0
2005f232:	4785                	c.li	a5,1
2005f234:	4721                	c.li	a4,8
2005f236:	03000693          	addi	a3,zero,48
2005f23a:	4641                	c.li	a2,16
2005f23c:	4581                	c.li	a1,0
2005f23e:	d3bff0ef          	jal	ra,2005ef78 <pad_num_control>
2005f242:	00440b93          	addi	s7,s0,4
2005f246:	94aa                	c.add	s1,a0
2005f248:	4985                	c.li	s3,1
2005f24a:	bfa5                	c.j	2005f1c2 <DiagVprintf+0x152>
2005f24c:	00042c83          	lw	s9,0(s0)
2005f250:	00440b93          	addi	s7,s0,4
2005f254:	4401                	c.li	s0,0
2005f256:	008c87b3          	add	a5,s9,s0
2005f25a:	0007c783          	lbu	a5,0(a5)
2005f25e:	e7a9                	c.bnez	a5,2005f2a8 <DiagVprintf+0x238>
2005f260:	02089063          	bne	a7,zero,2005f280 <DiagVprintf+0x210>
2005f264:	40870533          	sub	a0,a4,s0
2005f268:	00a05c63          	bge	zero,a0,2005f280 <DiagVprintf+0x210>
2005f26c:	85b6                	c.mv	a1,a3
2005f26e:	c646                	c.swsp	a7,12(sp)
2005f270:	c43a                	c.swsp	a4,8(sp)
2005f272:	c236                	c.swsp	a3,4(sp)
2005f274:	c0fff0ef          	jal	ra,2005ee82 <pad_char_control>
2005f278:	48b2                	c.lwsp	a7,12(sp)
2005f27a:	4722                	c.lwsp	a4,8(sp)
2005f27c:	4692                	c.lwsp	a3,4(sp)
2005f27e:	94aa                	c.add	s1,a0
2005f280:	5c7d                	c.li	s8,-1
2005f282:	000c8863          	beq	s9,zero,2005f292 <DiagVprintf+0x222>
2005f286:	4c01                	c.li	s8,0
2005f288:	018c87b3          	add	a5,s9,s8
2005f28c:	0007c503          	lbu	a0,0(a5)
2005f290:	ed11                	c.bnez	a0,2005f2ac <DiagVprintf+0x23c>
2005f292:	94e2                	c.add	s1,s8
2005f294:	f20887e3          	beq	a7,zero,2005f1c2 <DiagVprintf+0x152>
2005f298:	40870533          	sub	a0,a4,s0
2005f29c:	85b6                	c.mv	a1,a3
2005f29e:	f2a052e3          	bge	zero,a0,2005f1c2 <DiagVprintf+0x152>
2005f2a2:	be1ff0ef          	jal	ra,2005ee82 <pad_char_control>
2005f2a6:	bf29                	c.j	2005f1c0 <DiagVprintf+0x150>
2005f2a8:	0405                	c.addi	s0,1
2005f2aa:	b775                	c.j	2005f256 <DiagVprintf+0x1e6>
2005f2ac:	c646                	c.swsp	a7,12(sp)
2005f2ae:	c43a                	c.swsp	a4,8(sp)
2005f2b0:	c236                	c.swsp	a3,4(sp)
2005f2b2:	dffa5097          	auipc	ra,0xdffa5
2005f2b6:	292080e7          	jalr	ra,658(ra) # 4544 <LOGUART_PutChar>
2005f2ba:	48b2                	c.lwsp	a7,12(sp)
2005f2bc:	4722                	c.lwsp	a4,8(sp)
2005f2be:	4692                	c.lwsp	a3,4(sp)
2005f2c0:	0c05                	c.addi	s8,1
2005f2c2:	b7d9                	c.j	2005f288 <DiagVprintf+0x218>
2005f2c4:	00440b93          	addi	s7,s0,4
2005f2c8:	00044403          	lbu	s0,0(s0)
2005f2cc:	02089163          	bne	a7,zero,2005f2ee <DiagVprintf+0x27e>
2005f2d0:	4785                	c.li	a5,1
2005f2d2:	00e7de63          	bge	a5,a4,2005f2ee <DiagVprintf+0x27e>
2005f2d6:	85b6                	c.mv	a1,a3
2005f2d8:	fff70513          	addi	a0,a4,-1
2005f2dc:	c646                	c.swsp	a7,12(sp)
2005f2de:	c436                	c.swsp	a3,8(sp)
2005f2e0:	c23a                	c.swsp	a4,4(sp)
2005f2e2:	ba1ff0ef          	jal	ra,2005ee82 <pad_char_control>
2005f2e6:	48b2                	c.lwsp	a7,12(sp)
2005f2e8:	46a2                	c.lwsp	a3,8(sp)
2005f2ea:	4712                	c.lwsp	a4,4(sp)
2005f2ec:	94aa                	c.add	s1,a0
2005f2ee:	8522                	c.mv	a0,s0
2005f2f0:	c646                	c.swsp	a7,12(sp)
2005f2f2:	c43a                	c.swsp	a4,8(sp)
2005f2f4:	c236                	c.swsp	a3,4(sp)
2005f2f6:	dffa5097          	auipc	ra,0xdffa5
2005f2fa:	24e080e7          	jalr	ra,590(ra) # 4544 <LOGUART_PutChar>
2005f2fe:	48b2                	c.lwsp	a7,12(sp)
2005f300:	0485                	c.addi	s1,1
2005f302:	ec0880e3          	beq	a7,zero,2005f1c2 <DiagVprintf+0x152>
2005f306:	4722                	c.lwsp	a4,8(sp)
2005f308:	4785                	c.li	a5,1
2005f30a:	eae7dce3          	bge	a5,a4,2005f1c2 <DiagVprintf+0x152>
2005f30e:	4692                	c.lwsp	a3,4(sp)
2005f310:	fff70513          	addi	a0,a4,-1
2005f314:	85b6                	c.mv	a1,a3
2005f316:	b771                	c.j	2005f2a2 <DiagVprintf+0x232>
2005f318:	0605                	c.addi	a2,1
2005f31a:	0905                	c.addi	s2,1
2005f31c:	bbf1                	c.j	2005f0f8 <DiagVprintf+0x88>
2005f31e:	02500513          	addi	a0,zero,37
2005f322:	dffa5097          	auipc	ra,0xdffa5
2005f326:	222080e7          	jalr	ra,546(ra) # 4544 <LOGUART_PutChar>
2005f32a:	0485                	c.addi	s1,1
2005f32c:	8ba2                	c.mv	s7,s0
2005f32e:	bd51                	c.j	2005f1c2 <DiagVprintf+0x152>
2005f330:	0905                	c.addi	s2,1
2005f332:	03000693          	addi	a3,zero,48
2005f336:	4701                	c.li	a4,0
2005f338:	00094783          	lbu	a5,0(s2)
2005f33c:	d60783e3          	beq	a5,zero,2005f0a2 <DiagVprintf+0x32>
2005f340:	fd078513          	addi	a0,a5,-48
2005f344:	0ff57513          	andi	a0,a0,255
2005f348:	daaae8e3          	bltu	s5,a0,2005f0f8 <DiagVprintf+0x88>
2005f34c:	03670733          	mul	a4,a4,s6
2005f350:	0905                	c.addi	s2,1
2005f352:	fd070713          	addi	a4,a4,-48
2005f356:	973e                	c.add	a4,a5
2005f358:	b7c5                	c.j	2005f338 <DiagVprintf+0x2c8>
2005f35a:	0905                	c.addi	s2,1
2005f35c:	02000693          	addi	a3,zero,32
2005f360:	bfd9                	c.j	2005f336 <DiagVprintf+0x2c6>
2005f362:	dffa5097          	auipc	ra,0xdffa5
2005f366:	1e2080e7          	jalr	ra,482(ra) # 4544 <LOGUART_PutChar>
2005f36a:	0485                	c.addi	s1,1
2005f36c:	bb05                	c.j	2005f09c <DiagVprintf+0x2c>

2005f36e <DiagPrintf>:
2005f36e:	7139                	c.addi16sp	sp,-64
2005f370:	d22e                	c.swsp	a1,36(sp)
2005f372:	104c                	c.addi4spn	a1,sp,36
2005f374:	ce06                	c.swsp	ra,28(sp)
2005f376:	d432                	c.swsp	a2,40(sp)
2005f378:	d636                	c.swsp	a3,44(sp)
2005f37a:	d83a                	c.swsp	a4,48(sp)
2005f37c:	da3e                	c.swsp	a5,52(sp)
2005f37e:	dc42                	c.swsp	a6,56(sp)
2005f380:	de46                	c.swsp	a7,60(sp)
2005f382:	c62e                	c.swsp	a1,12(sp)
2005f384:	cedff0ef          	jal	ra,2005f070 <DiagVprintf>
2005f388:	40f2                	c.lwsp	ra,28(sp)
2005f38a:	6121                	c.addi16sp	sp,64
2005f38c:	8082                	c.jr	ra

2005f38e <DiagPrintf_minimal>:
2005f38e:	711d                	c.addi16sp	sp,-96
2005f390:	da26                	c.swsp	s1,52(sp)
2005f392:	00c4                	c.addi4spn	s1,sp,68
2005f394:	dc22                	c.swsp	s0,56(sp)
2005f396:	d84a                	c.swsp	s2,48(sp)
2005f398:	d64e                	c.swsp	s3,44(sp)
2005f39a:	d452                	c.swsp	s4,40(sp)
2005f39c:	d256                	c.swsp	s5,36(sp)
2005f39e:	de06                	c.swsp	ra,60(sp)
2005f3a0:	d05a                	c.swsp	s6,32(sp)
2005f3a2:	ce5e                	c.swsp	s7,28(sp)
2005f3a4:	892a                	c.mv	s2,a0
2005f3a6:	c2ae                	c.swsp	a1,68(sp)
2005f3a8:	c4b2                	c.swsp	a2,72(sp)
2005f3aa:	c6b6                	c.swsp	a3,76(sp)
2005f3ac:	c8ba                	c.swsp	a4,80(sp)
2005f3ae:	cabe                	c.swsp	a5,84(sp)
2005f3b0:	ccc2                	c.swsp	a6,88(sp)
2005f3b2:	cec6                	c.swsp	a7,92(sp)
2005f3b4:	c626                	c.swsp	s1,12(sp)
2005f3b6:	4401                	c.li	s0,0
2005f3b8:	03000993          	addi	s3,zero,48
2005f3bc:	4a25                	c.li	s4,9
2005f3be:	4aa9                	c.li	s5,10
2005f3c0:	00094503          	lbu	a0,0(s2)
2005f3c4:	cd1d                	c.beqz	a0,2005f402 <DiagPrintf_minimal+0x74>
2005f3c6:	02500793          	addi	a5,zero,37
2005f3ca:	0905                	c.addi	s2,1
2005f3cc:	16f51a63          	bne	a0,a5,2005f540 <DiagPrintf_minimal+0x1b2>
2005f3d0:	4681                	c.li	a3,0
2005f3d2:	4601                	c.li	a2,0
2005f3d4:	06300893          	addi	a7,zero,99
2005f3d8:	03900593          	addi	a1,zero,57
2005f3dc:	02000313          	addi	t1,zero,32
2005f3e0:	00094783          	lbu	a5,0(s2)
2005f3e4:	05178163          	beq	a5,a7,2005f426 <DiagPrintf_minimal+0x98>
2005f3e8:	06f8e363          	bltu	a7,a5,2005f44e <DiagPrintf_minimal+0xc0>
2005f3ec:	13378063          	beq	a5,s3,2005f50c <DiagPrintf_minimal+0x17e>
2005f3f0:	02f9e563          	bltu	s3,a5,2005f41a <DiagPrintf_minimal+0x8c>
2005f3f4:	14678263          	beq	a5,t1,2005f538 <DiagPrintf_minimal+0x1aa>
2005f3f8:	02500713          	addi	a4,zero,37
2005f3fc:	10e78063          	beq	a5,a4,2005f4fc <DiagPrintf_minimal+0x16e>
2005f400:	547d                	c.li	s0,-1
2005f402:	50f2                	c.lwsp	ra,60(sp)
2005f404:	8522                	c.mv	a0,s0
2005f406:	5462                	c.lwsp	s0,56(sp)
2005f408:	54d2                	c.lwsp	s1,52(sp)
2005f40a:	5942                	c.lwsp	s2,48(sp)
2005f40c:	59b2                	c.lwsp	s3,44(sp)
2005f40e:	5a22                	c.lwsp	s4,40(sp)
2005f410:	5a92                	c.lwsp	s5,36(sp)
2005f412:	5b02                	c.lwsp	s6,32(sp)
2005f414:	4bf2                	c.lwsp	s7,28(sp)
2005f416:	6125                	c.addi16sp	sp,96
2005f418:	8082                	c.jr	ra
2005f41a:	12f5f063          	bgeu	a1,a5,2005f53a <DiagPrintf_minimal+0x1ac>
2005f41e:	04300713          	addi	a4,zero,67
2005f422:	fce79fe3          	bne	a5,a4,2005f400 <DiagPrintf_minimal+0x72>
2005f426:	4785                	c.li	a5,1
2005f428:	00448b13          	addi	s6,s1,4
2005f42c:	0004c483          	lbu	s1,0(s1)
2005f430:	00d7d863          	bge	a5,a3,2005f440 <DiagPrintf_minimal+0xb2>
2005f434:	85b2                	c.mv	a1,a2
2005f436:	fff68513          	addi	a0,a3,-1
2005f43a:	a49ff0ef          	jal	ra,2005ee82 <pad_char_control>
2005f43e:	942a                	c.add	s0,a0
2005f440:	8526                	c.mv	a0,s1
2005f442:	dffa5097          	auipc	ra,0xdffa5
2005f446:	102080e7          	jalr	ra,258(ra) # 4544 <LOGUART_PutChar>
2005f44a:	0405                	c.addi	s0,1
2005f44c:	a825                	c.j	2005f484 <DiagPrintf_minimal+0xf6>
2005f44e:	07300713          	addi	a4,zero,115
2005f452:	04e78e63          	beq	a5,a4,2005f4ae <DiagPrintf_minimal+0x120>
2005f456:	02f76963          	bltu	a4,a5,2005f488 <DiagPrintf_minimal+0xfa>
2005f45a:	06400713          	addi	a4,zero,100
2005f45e:	00e78663          	beq	a5,a4,2005f46a <DiagPrintf_minimal+0xdc>
2005f462:	06900713          	addi	a4,zero,105
2005f466:	f8e79de3          	bne	a5,a4,2005f400 <DiagPrintf_minimal+0x72>
2005f46a:	4088                	c.lw	a0,0(s1)
2005f46c:	00448b13          	addi	s6,s1,4
2005f470:	4701                	c.li	a4,0
2005f472:	00055563          	bge	a0,zero,2005f47c <DiagPrintf_minimal+0xee>
2005f476:	40a00533          	sub	a0,zero,a0
2005f47a:	4705                	c.li	a4,1
2005f47c:	45a9                	c.li	a1,10
2005f47e:	a3bff0ef          	jal	ra,2005eeb8 <print_decimal_num>
2005f482:	942a                	c.add	s0,a0
2005f484:	84da                	c.mv	s1,s6
2005f486:	a08d                	c.j	2005f4e8 <DiagPrintf_minimal+0x15a>
2005f488:	07500713          	addi	a4,zero,117
2005f48c:	00e78b63          	beq	a5,a4,2005f4a2 <DiagPrintf_minimal+0x114>
2005f490:	07800713          	addi	a4,zero,120
2005f494:	f6e796e3          	bne	a5,a4,2005f400 <DiagPrintf_minimal+0x72>
2005f498:	00448b13          	addi	s6,s1,4
2005f49c:	4701                	c.li	a4,0
2005f49e:	45c1                	c.li	a1,16
2005f4a0:	a029                	c.j	2005f4aa <DiagPrintf_minimal+0x11c>
2005f4a2:	00448b13          	addi	s6,s1,4
2005f4a6:	4701                	c.li	a4,0
2005f4a8:	45a9                	c.li	a1,10
2005f4aa:	4088                	c.lw	a0,0(s1)
2005f4ac:	bfc9                	c.j	2005f47e <DiagPrintf_minimal+0xf0>
2005f4ae:	0004ab03          	lw	s6,0(s1)
2005f4b2:	00448b93          	addi	s7,s1,4
2005f4b6:	4781                	c.li	a5,0
2005f4b8:	00fb0733          	add	a4,s6,a5
2005f4bc:	00074703          	lbu	a4,0(a4)
2005f4c0:	e715                	c.bnez	a4,2005f4ec <DiagPrintf_minimal+0x15e>
2005f4c2:	40f68533          	sub	a0,a3,a5
2005f4c6:	00a05663          	bge	zero,a0,2005f4d2 <DiagPrintf_minimal+0x144>
2005f4ca:	85b2                	c.mv	a1,a2
2005f4cc:	9b7ff0ef          	jal	ra,2005ee82 <pad_char_control>
2005f4d0:	942a                	c.add	s0,a0
2005f4d2:	54fd                	c.li	s1,-1
2005f4d4:	000b0863          	beq	s6,zero,2005f4e4 <DiagPrintf_minimal+0x156>
2005f4d8:	4481                	c.li	s1,0
2005f4da:	009b07b3          	add	a5,s6,s1
2005f4de:	0007c503          	lbu	a0,0(a5)
2005f4e2:	e519                	c.bnez	a0,2005f4f0 <DiagPrintf_minimal+0x162>
2005f4e4:	9426                	c.add	s0,s1
2005f4e6:	84de                	c.mv	s1,s7
2005f4e8:	0905                	c.addi	s2,1
2005f4ea:	bdd9                	c.j	2005f3c0 <DiagPrintf_minimal+0x32>
2005f4ec:	0785                	c.addi	a5,1
2005f4ee:	b7e9                	c.j	2005f4b8 <DiagPrintf_minimal+0x12a>
2005f4f0:	dffa5097          	auipc	ra,0xdffa5
2005f4f4:	054080e7          	jalr	ra,84(ra) # 4544 <LOGUART_PutChar>
2005f4f8:	0485                	c.addi	s1,1
2005f4fa:	b7c5                	c.j	2005f4da <DiagPrintf_minimal+0x14c>
2005f4fc:	02500513          	addi	a0,zero,37
2005f500:	dffa5097          	auipc	ra,0xdffa5
2005f504:	044080e7          	jalr	ra,68(ra) # 4544 <LOGUART_PutChar>
2005f508:	0405                	c.addi	s0,1
2005f50a:	bff9                	c.j	2005f4e8 <DiagPrintf_minimal+0x15a>
2005f50c:	0905                	c.addi	s2,1
2005f50e:	03000613          	addi	a2,zero,48
2005f512:	4681                	c.li	a3,0
2005f514:	00094703          	lbu	a4,0(s2)
2005f518:	ee0705e3          	beq	a4,zero,2005f402 <DiagPrintf_minimal+0x74>
2005f51c:	fd070813          	addi	a6,a4,-48
2005f520:	0ff87813          	andi	a6,a6,255
2005f524:	eb0a6ee3          	bltu	s4,a6,2005f3e0 <DiagPrintf_minimal+0x52>
2005f528:	03568533          	mul	a0,a3,s5
2005f52c:	0905                	c.addi	s2,1
2005f52e:	fd050513          	addi	a0,a0,-48 # fff7ffd0 <__ctrace_end__+0x35f7ffd0>
2005f532:	00a706b3          	add	a3,a4,a0
2005f536:	bff9                	c.j	2005f514 <DiagPrintf_minimal+0x186>
2005f538:	0905                	c.addi	s2,1
2005f53a:	02000613          	addi	a2,zero,32
2005f53e:	bfd1                	c.j	2005f512 <DiagPrintf_minimal+0x184>
2005f540:	dffa5097          	auipc	ra,0xdffa5
2005f544:	004080e7          	jalr	ra,4(ra) # 4544 <LOGUART_PutChar>
2005f548:	0405                	c.addi	s0,1
2005f54a:	bd9d                	c.j	2005f3c0 <DiagPrintf_minimal+0x32>

2005f54c <DiagSnPrintf>:
2005f54c:	7159                	c.addi16sp	sp,-112
2005f54e:	c6a2                	c.swsp	s0,76(sp)
2005f550:	c4a6                	c.swsp	s1,72(sp)
2005f552:	c2ca                	c.swsp	s2,68(sp)
2005f554:	c0ce                	c.swsp	s3,64(sp)
2005f556:	de52                	c.swsp	s4,60(sp)
2005f558:	dc56                	c.swsp	s5,56(sp)
2005f55a:	da5a                	c.swsp	s6,52(sp)
2005f55c:	d85e                	c.swsp	s7,48(sp)
2005f55e:	d662                	c.swsp	s8,44(sp)
2005f560:	d466                	c.swsp	s9,40(sp)
2005f562:	d26a                	c.swsp	s10,36(sp)
2005f564:	d06e                	c.swsp	s11,32(sp)
2005f566:	ceb6                	c.swsp	a3,92(sp)
2005f568:	d0ba                	c.swsp	a4,96(sp)
2005f56a:	d2be                	c.swsp	a5,100(sp)
2005f56c:	d4c2                	c.swsp	a6,104(sp)
2005f56e:	d6c6                	c.swsp	a7,108(sp)
2005f570:	28050463          	beq	a0,zero,2005f7f8 <DiagSnPrintf+0x2ac>
2005f574:	08fc                	c.addi4spn	a5,sp,92
2005f576:	c43e                	c.swsp	a5,8(sp)
2005f578:	537d                	c.li	t1,-1
2005f57a:	c199                	c.beqz	a1,2005f580 <DiagSnPrintf+0x34>
2005f57c:	00b50333          	add	t1,a0,a1
2005f580:	200643b7          	lui	t2,0x20064
2005f584:	6405                	c.lui	s0,0x1
2005f586:	882a                	c.mv	a6,a0
2005f588:	05000f13          	addi	t5,zero,80
2005f58c:	06400913          	addi	s2,zero,100
2005f590:	06300993          	addi	s3,zero,99
2005f594:	4fa9                	c.li	t6,10
2005f596:	03000293          	addi	t0,zero,48
2005f59a:	02d00a13          	addi	s4,zero,45
2005f59e:	b1c38393          	addi	t2,t2,-1252 # 20063b1c <TaskStateString.0+0xa4>
2005f5a2:	4aa5                	c.li	s5,9
2005f5a4:	0ef00b13          	addi	s6,zero,239
2005f5a8:	eff40b93          	addi	s7,s0,-257 # eff <CPU_ClkGet+0x3e9>
2005f5ac:	6c3d                	c.lui	s8,0xf
2005f5ae:	7cc1                	c.lui	s9,0xffff0
2005f5b0:	00064783          	lbu	a5,0(a2)
2005f5b4:	e399                	c.bnez	a5,2005f5ba <DiagSnPrintf+0x6e>
2005f5b6:	8742                	c.mv	a4,a6
2005f5b8:	a889                	c.j	2005f60a <DiagSnPrintf+0xbe>
2005f5ba:	02500713          	addi	a4,zero,37
2005f5be:	00e78b63          	beq	a5,a4,2005f5d4 <DiagSnPrintf+0x88>
2005f5c2:	00180713          	addi	a4,a6,1 # fff00001 <__ctrace_end__+0x35f00001>
2005f5c6:	00f80023          	sb	a5,0(a6)
2005f5ca:	04677063          	bgeu	a4,t1,2005f60a <DiagSnPrintf+0xbe>
2005f5ce:	0605                	c.addi	a2,1
2005f5d0:	883a                	c.mv	a6,a4
2005f5d2:	bff9                	c.j	2005f5b0 <DiagSnPrintf+0x64>
2005f5d4:	4722                	c.lwsp	a4,8(sp)
2005f5d6:	00164e03          	lbu	t3,1(a2)
2005f5da:	07300693          	addi	a3,zero,115
2005f5de:	431c                	c.lw	a5,0(a4)
2005f5e0:	00160593          	addi	a1,a2,1
2005f5e4:	0711                	c.addi	a4,4
2005f5e6:	04de1463          	bne	t3,a3,2005f62e <DiagSnPrintf+0xe2>
2005f5ea:	c43a                	c.swsp	a4,8(sp)
2005f5ec:	410787b3          	sub	a5,a5,a6
2005f5f0:	8742                	c.mv	a4,a6
2005f5f2:	00f706b3          	add	a3,a4,a5
2005f5f6:	0006c683          	lbu	a3,0(a3)
2005f5fa:	e299                	c.bnez	a3,2005f600 <DiagSnPrintf+0xb4>
2005f5fc:	862e                	c.mv	a2,a1
2005f5fe:	bfc1                	c.j	2005f5ce <DiagSnPrintf+0x82>
2005f600:	0705                	c.addi	a4,1
2005f602:	fed70fa3          	sb	a3,-1(a4)
2005f606:	fe6766e3          	bltu	a4,t1,2005f5f2 <DiagSnPrintf+0xa6>
2005f60a:	00070023          	sb	zero,0(a4)
2005f60e:	40a70533          	sub	a0,a4,a0
2005f612:	4436                	c.lwsp	s0,76(sp)
2005f614:	44a6                	c.lwsp	s1,72(sp)
2005f616:	4916                	c.lwsp	s2,68(sp)
2005f618:	4986                	c.lwsp	s3,64(sp)
2005f61a:	5a72                	c.lwsp	s4,60(sp)
2005f61c:	5ae2                	c.lwsp	s5,56(sp)
2005f61e:	5b52                	c.lwsp	s6,52(sp)
2005f620:	5bc2                	c.lwsp	s7,48(sp)
2005f622:	5c32                	c.lwsp	s8,44(sp)
2005f624:	5ca2                	c.lwsp	s9,40(sp)
2005f626:	5d12                	c.lwsp	s10,36(sp)
2005f628:	5d82                	c.lwsp	s11,32(sp)
2005f62a:	6165                	c.addi16sp	sp,112
2005f62c:	8082                	c.jr	ra
2005f62e:	c43a                	c.swsp	a4,8(sp)
2005f630:	0dfe7693          	andi	a3,t3,223
2005f634:	473d                	c.li	a4,15
2005f636:	0de69563          	bne	a3,t5,2005f700 <DiagSnPrintf+0x1b4>
2005f63a:	0cf75a63          	bge	a4,a5,2005f70e <DiagSnPrintf+0x1c2>
2005f63e:	46a1                	c.li	a3,8
2005f640:	03000893          	addi	a7,zero,48
2005f644:	ff078e93          	addi	t4,a5,-16
2005f648:	4711                	c.li	a4,4
2005f64a:	05db7463          	bgeu	s6,t4,2005f692 <DiagSnPrintf+0x146>
2005f64e:	f0078e93          	addi	t4,a5,-256
2005f652:	4721                	c.li	a4,8
2005f654:	03dbff63          	bgeu	s7,t4,2005f692 <DiagSnPrintf+0x146>
2005f658:	40878eb3          	sub	t4,a5,s0
2005f65c:	4731                	c.li	a4,12
2005f65e:	038eea63          	bltu	t4,s8,2005f692 <DiagSnPrintf+0x146>
2005f662:	01978eb3          	add	t4,a5,s9
2005f666:	000f04b7          	lui	s1,0xf0
2005f66a:	4741                	c.li	a4,16
2005f66c:	029ee363          	bltu	t4,s1,2005f692 <DiagSnPrintf+0x146>
2005f670:	fff00eb7          	lui	t4,0xfff00
2005f674:	9ebe                	c.add	t4,a5
2005f676:	00f004b7          	lui	s1,0xf00
2005f67a:	4751                	c.li	a4,20
2005f67c:	009eeb63          	bltu	t4,s1,2005f692 <DiagSnPrintf+0x146>
2005f680:	ff000eb7          	lui	t4,0xff000
2005f684:	9ebe                	c.add	t4,a5
2005f686:	0f0004b7          	lui	s1,0xf000
2005f68a:	4761                	c.li	a4,24
2005f68c:	009ee363          	bltu	t4,s1,2005f692 <DiagSnPrintf+0x146>
2005f690:	4771                	c.li	a4,28
2005f692:	005e1663          	bne	t3,t0,2005f69e <DiagSnPrintf+0x152>
2005f696:	00260593          	addi	a1,a2,2
2005f69a:	03000893          	addi	a7,zero,48
2005f69e:	0005c603          	lbu	a2,0(a1)
2005f6a2:	44a1                	c.li	s1,8
2005f6a4:	fcf60e13          	addi	t3,a2,-49
2005f6a8:	0ffe7e13          	andi	t3,t3,255
2005f6ac:	03c4e163          	bltu	s1,t3,2005f6ce <DiagSnPrintf+0x182>
2005f6b0:	4681                	c.li	a3,0
2005f6b2:	03f686b3          	mul	a3,a3,t6
2005f6b6:	0585                	c.addi	a1,1
2005f6b8:	96b2                	c.add	a3,a2
2005f6ba:	0005c603          	lbu	a2,0(a1)
2005f6be:	fd068693          	addi	a3,a3,-48
2005f6c2:	fd060e13          	addi	t3,a2,-48
2005f6c6:	0ffe7e13          	andi	t3,t3,255
2005f6ca:	ffcaf4e3          	bgeu	s5,t3,2005f6b2 <DiagSnPrintf+0x166>
2005f6ce:	0005c603          	lbu	a2,0(a1)
2005f6d2:	0d767e13          	andi	t3,a2,215
2005f6d6:	0bee1463          	bne	t3,t5,2005f77e <DiagSnPrintf+0x232>
2005f6da:	02067493          	andi	s1,a2,32
2005f6de:	0df67613          	andi	a2,a2,223
2005f6e2:	8ebe                	c.mv	t4,a5
2005f6e4:	00c10e13          	addi	t3,sp,12
2005f6e8:	01e61a63          	bne	a2,t5,2005f6fc <DiagSnPrintf+0x1b0>
2005f6ec:	0584e613          	ori	a2,s1,88
2005f6f0:	00510623          	sb	t0,12(sp)
2005f6f4:	00c106a3          	sb	a2,13(sp)
2005f6f8:	00e10e13          	addi	t3,sp,14
2005f6fc:	4601                	c.li	a2,0
2005f6fe:	a831                	c.j	2005f71a <DiagSnPrintf+0x1ce>
2005f700:	4681                	c.li	a3,0
2005f702:	02000893          	addi	a7,zero,32
2005f706:	f2f74fe3          	blt	a4,a5,2005f644 <DiagSnPrintf+0xf8>
2005f70a:	4701                	c.li	a4,0
2005f70c:	b759                	c.j	2005f692 <DiagSnPrintf+0x146>
2005f70e:	4701                	c.li	a4,0
2005f710:	46a1                	c.li	a3,8
2005f712:	b761                	c.j	2005f69a <DiagSnPrintf+0x14e>
2005f714:	0605                	c.addi	a2,1
2005f716:	004ede93          	srli	t4,t4,0x4
2005f71a:	fe0e9de3          	bne	t4,zero,2005f714 <DiagSnPrintf+0x1c8>
2005f71e:	8ef2                	c.mv	t4,t3
2005f720:	00de0db3          	add	s11,t3,a3
2005f724:	41dd8d33          	sub	s10,s11,t4
2005f728:	05a64763          	blt	a2,s10,2005f776 <DiagSnPrintf+0x22a>
2005f72c:	4881                	c.li	a7,0
2005f72e:	00c6c463          	blt	a3,a2,2005f736 <DiagSnPrintf+0x1ea>
2005f732:	40c688b3          	sub	a7,a3,a2
2005f736:	9e46                	c.add	t3,a7
2005f738:	88f2                	c.mv	a7,t3
2005f73a:	863a                	c.mv	a2,a4
2005f73c:	40c7d6b3          	sra	a3,a5,a2
2005f740:	8abd                	c.andi	a3,15
2005f742:	969e                	c.add	a3,t2
2005f744:	0006c683          	lbu	a3,0(a3)
2005f748:	1671                	c.addi	a2,-4
2005f74a:	0885                	c.addi	a7,1
2005f74c:	8ec5                	c.or	a3,s1
2005f74e:	fed88fa3          	sb	a3,-1(a7)
2005f752:	fe0655e3          	bge	a2,zero,2005f73c <DiagSnPrintf+0x1f0>
2005f756:	8309                	c.srli	a4,0x2
2005f758:	0705                	c.addi	a4,1
2005f75a:	9e3a                	c.add	t3,a4
2005f75c:	8742                	c.mv	a4,a6
2005f75e:	007c                	c.addi4spn	a5,sp,12
2005f760:	e9c7fee3          	bgeu	a5,t3,2005f5fc <DiagSnPrintf+0xb0>
2005f764:	0007c683          	lbu	a3,0(a5)
2005f768:	0705                	c.addi	a4,1
2005f76a:	fed70fa3          	sb	a3,-1(a4)
2005f76e:	e8677ee3          	bgeu	a4,t1,2005f60a <DiagSnPrintf+0xbe>
2005f772:	0785                	c.addi	a5,1
2005f774:	b7f5                	c.j	2005f760 <DiagSnPrintf+0x214>
2005f776:	011e8023          	sb	a7,0(t4) # ff000000 <__ctrace_end__+0x35000000>
2005f77a:	0e85                	c.addi	t4,1
2005f77c:	b765                	c.j	2005f724 <DiagSnPrintf+0x1d8>
2005f77e:	07261363          	bne	a2,s2,2005f7e4 <DiagSnPrintf+0x298>
2005f782:	0407de63          	bge	a5,zero,2005f7de <DiagSnPrintf+0x292>
2005f786:	01410623          	sb	s4,12(sp)
2005f78a:	40f007b3          	sub	a5,zero,a5
2005f78e:	4885                	c.li	a7,1
2005f790:	00d10693          	addi	a3,sp,13
2005f794:	8e36                	c.mv	t3,a3
2005f796:	03f7e633          	rem	a2,a5,t6
2005f79a:	0e05                	c.addi	t3,1
2005f79c:	03f7c7b3          	div	a5,a5,t6
2005f7a0:	03060613          	addi	a2,a2,48
2005f7a4:	fece0fa3          	sb	a2,-1(t3)
2005f7a8:	011e0633          	add	a2,t3,a7
2005f7ac:	8e15                	c.sub	a2,a3
2005f7ae:	f7e5                	c.bnez	a5,2005f796 <DiagSnPrintf+0x24a>
2005f7b0:	00c04563          	blt	zero,a2,2005f7ba <DiagSnPrintf+0x26e>
2005f7b4:	005e0023          	sb	t0,0(t3)
2005f7b8:	0e05                	c.addi	t3,1
2005f7ba:	1771                	c.addi	a4,-4
2005f7bc:	167d                	c.addi	a2,-1
2005f7be:	fe0759e3          	bge	a4,zero,2005f7b0 <DiagSnPrintf+0x264>
2005f7c2:	87f2                	c.mv	a5,t3
2005f7c4:	17fd                	c.addi	a5,-1
2005f7c6:	f8f6fbe3          	bgeu	a3,a5,2005f75c <DiagSnPrintf+0x210>
2005f7ca:	0006c603          	lbu	a2,0(a3)
2005f7ce:	0007c703          	lbu	a4,0(a5)
2005f7d2:	0685                	c.addi	a3,1
2005f7d4:	00c78023          	sb	a2,0(a5)
2005f7d8:	fee68fa3          	sb	a4,-1(a3)
2005f7dc:	b7e5                	c.j	2005f7c4 <DiagSnPrintf+0x278>
2005f7de:	4881                	c.li	a7,0
2005f7e0:	0074                	c.addi4spn	a3,sp,12
2005f7e2:	bf4d                	c.j	2005f794 <DiagSnPrintf+0x248>
2005f7e4:	01361763          	bne	a2,s3,2005f7f2 <DiagSnPrintf+0x2a6>
2005f7e8:	00f10623          	sb	a5,12(sp)
2005f7ec:	00d10e13          	addi	t3,sp,13
2005f7f0:	b7b5                	c.j	2005f75c <DiagSnPrintf+0x210>
2005f7f2:	00c10623          	sb	a2,12(sp)
2005f7f6:	bfdd                	c.j	2005f7ec <DiagSnPrintf+0x2a0>
2005f7f8:	4501                	c.li	a0,0
2005f7fa:	bd21                	c.j	2005f612 <DiagSnPrintf+0xc6>

2005f7fc <TRNG_get_random_bytes>:
2005f7fc:	7179                	c.addi16sp	sp,-48
2005f7fe:	d422                	c.swsp	s0,40(sp)
2005f800:	d226                	c.swsp	s1,36(sp)
2005f802:	d606                	c.swsp	ra,44(sp)
2005f804:	d04a                	c.swsp	s2,32(sp)
2005f806:	ce4e                	c.swsp	s3,28(sp)
2005f808:	4100c7b7          	lui	a5,0x4100c
2005f80c:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005f810:	84aa                	c.mv	s1,a0
2005f812:	842e                	c.mv	s0,a1
2005f814:	8399                	c.srli	a5,0x6
2005f816:	8b8d                	c.andi	a5,3
2005f818:	ef9d                	c.bnez	a5,2005f856 <TRNG_get_random_bytes+0x5a>
2005f81a:	00357793          	andi	a5,a0,3
2005f81e:	cf85                	c.beqz	a5,2005f856 <TRNG_get_random_bytes+0x5a>
2005f820:	852e                	c.mv	a0,a1
2005f822:	ebbfb0ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005f826:	85a2                	c.mv	a1,s0
2005f828:	892a                	c.mv	s2,a0
2005f82a:	dffa9097          	auipc	ra,0xdffa9
2005f82e:	540080e7          	jalr	ra,1344(ra) # 8d6a <RandBytes_Get>
2005f832:	8622                	c.mv	a2,s0
2005f834:	85ca                	c.mv	a1,s2
2005f836:	8526                	c.mv	a0,s1
2005f838:	dffaa097          	auipc	ra,0xdffaa
2005f83c:	298080e7          	jalr	ra,664(ra) # 9ad0 <__wrap_memcpy>
2005f840:	854a                	c.mv	a0,s2
2005f842:	ec5fb0ef          	jal	ra,2005b706 <rtos_mem_free>
2005f846:	50b2                	c.lwsp	ra,44(sp)
2005f848:	5422                	c.lwsp	s0,40(sp)
2005f84a:	5492                	c.lwsp	s1,36(sp)
2005f84c:	5902                	c.lwsp	s2,32(sp)
2005f84e:	49f2                	c.lwsp	s3,28(sp)
2005f850:	4501                	c.li	a0,0
2005f852:	6145                	c.addi16sp	sp,48
2005f854:	8082                	c.jr	ra
2005f856:	00245913          	srli	s2,s0,0x2
2005f85a:	4981                	c.li	s3,0
2005f85c:	0329c463          	blt	s3,s2,2005f884 <TRNG_get_random_bytes+0x88>
2005f860:	5671                	c.li	a2,-4
2005f862:	02c90633          	mul	a2,s2,a2
2005f866:	9432                	c.add	s0,a2
2005f868:	dc79                	c.beqz	s0,2005f846 <TRNG_get_random_bytes+0x4a>
2005f86a:	f54ff0ef          	jal	ra,2005efbe <_rand>
2005f86e:	c62a                	c.swsp	a0,12(sp)
2005f870:	00291513          	slli	a0,s2,0x2
2005f874:	8622                	c.mv	a2,s0
2005f876:	006c                	c.addi4spn	a1,sp,12
2005f878:	9526                	c.add	a0,s1
2005f87a:	dffaa097          	auipc	ra,0xdffaa
2005f87e:	d98080e7          	jalr	ra,-616(ra) # 9612 <_memcpy>
2005f882:	b7d1                	c.j	2005f846 <TRNG_get_random_bytes+0x4a>
2005f884:	f3aff0ef          	jal	ra,2005efbe <_rand>
2005f888:	00299793          	slli	a5,s3,0x2
2005f88c:	97a6                	c.add	a5,s1
2005f88e:	c388                	c.sw	a0,0(a5)
2005f890:	0985                	c.addi	s3,1
2005f892:	b7e9                	c.j	2005f85c <TRNG_get_random_bytes+0x60>

2005f894 <io_assert_failed>:
2005f894:	862e                	c.mv	a2,a1
2005f896:	85aa                	c.mv	a1,a0
2005f898:	20064537          	lui	a0,0x20064
2005f89c:	1141                	c.addi	sp,-16
2005f89e:	b3050513          	addi	a0,a0,-1232 # 20063b30 <TaskStateString.0+0xb8>
2005f8a2:	c606                	c.swsp	ra,12(sp)
2005f8a4:	aebff0ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005f8a8:	a001                	c.j	2005f8a8 <io_assert_failed+0x14>

2005f8aa <irq_set_priority>:
2005f8aa:	471d                	c.li	a4,7
2005f8ac:	02b77563          	bgeu	a4,a1,2005f8d6 <irq_set_priority+0x2c>
2005f8b0:	862e                	c.mv	a2,a1
2005f8b2:	85aa                	c.mv	a1,a0
2005f8b4:	20064537          	lui	a0,0x20064
2005f8b8:	1141                	c.addi	sp,-16
2005f8ba:	469d                	c.li	a3,7
2005f8bc:	c0c50513          	addi	a0,a0,-1012 # 20063c0c <TaskStateString.0+0x194>
2005f8c0:	c606                	c.swsp	ra,12(sp)
2005f8c2:	acdff0ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2005f8c6:	20064537          	lui	a0,0x20064
2005f8ca:	44800593          	addi	a1,zero,1096
2005f8ce:	b6450513          	addi	a0,a0,-1180 # 20063b64 <TaskStateString.0+0xec>
2005f8d2:	fc3ff0ef          	jal	ra,2005f894 <io_assert_failed>
2005f8d6:	73e00067          	jalr	zero,1854(zero) # 0 <Reset_Handler>

2005f8da <irq_register>:
2005f8da:	20000737          	lui	a4,0x20000
2005f8de:	1141                	c.addi	sp,-16
2005f8e0:	00259893          	slli	a7,a1,0x2
2005f8e4:	882a                	c.mv	a6,a0
2005f8e6:	00070713          	addi	a4,a4,0 # 20000000 <UserIrqFunTable>
2005f8ea:	200007b7          	lui	a5,0x20000
2005f8ee:	c606                	c.swsp	ra,12(sp)
2005f8f0:	9746                	c.add	a4,a7
2005f8f2:	00186813          	ori	a6,a6,1
2005f8f6:	1c078793          	addi	a5,a5,448 # 200001c0 <UserIrqDataTable>
2005f8fa:	01072023          	sw	a6,0(a4)
2005f8fe:	97c6                	c.add	a5,a7
2005f900:	852e                	c.mv	a0,a1
2005f902:	c390                	c.sw	a2,0(a5)
2005f904:	85b6                	c.mv	a1,a3
2005f906:	fa5ff0ef          	jal	ra,2005f8aa <irq_set_priority>
2005f90a:	40b2                	c.lwsp	ra,12(sp)
2005f90c:	4505                	c.li	a0,1
2005f90e:	0141                	c.addi	sp,16
2005f910:	8082                	c.jr	ra

2005f912 <unwind_frame_rv32i>:
2005f912:	715d                	c.addi16sp	sp,-80
2005f914:	c686                	c.swsp	ra,76(sp)
2005f916:	c4a2                	c.swsp	s0,72(sp)
2005f918:	0880                	c.addi4spn	s0,sp,80
2005f91a:	faa42e23          	sw	a0,-68(s0)
2005f91e:	fab42c23          	sw	a1,-72(s0)
2005f922:	fe042623          	sw	zero,-20(s0)
2005f926:	fe042423          	sw	zero,-24(s0)
2005f92a:	fbc42783          	lw	a5,-68(s0)
2005f92e:	43dc                	c.lw	a5,4(a5)
2005f930:	c789                	c.beqz	a5,2005f93a <unwind_frame_rv32i+0x28>
2005f932:	fbc42783          	lw	a5,-68(s0)
2005f936:	439c                	c.lw	a5,0(a5)
2005f938:	e399                	c.bnez	a5,2005f93e <unwind_frame_rv32i+0x2c>
2005f93a:	57fd                	c.li	a5,-1
2005f93c:	a44d                	c.j	2005fbde <unwind_frame_rv32i+0x2cc>
2005f93e:	fbc42783          	lw	a5,-68(s0)
2005f942:	43dc                	c.lw	a5,4(a5)
2005f944:	fef42223          	sw	a5,-28(s0)
2005f948:	a435                	c.j	2005fb74 <unwind_frame_rv32i+0x262>
2005f94a:	fe442783          	lw	a5,-28(s0)
2005f94e:	8b89                	c.andi	a5,2
2005f950:	c385                	c.beqz	a5,2005f970 <unwind_frame_rv32i+0x5e>
2005f952:	fe442783          	lw	a5,-28(s0)
2005f956:	0007d783          	lhu	a5,0(a5)
2005f95a:	873e                	c.mv	a4,a5
2005f95c:	fe442783          	lw	a5,-28(s0)
2005f960:	0789                	c.addi	a5,2
2005f962:	0007d783          	lhu	a5,0(a5)
2005f966:	07c2                	c.slli	a5,0x10
2005f968:	8fd9                	c.or	a5,a4
2005f96a:	fcf42c23          	sw	a5,-40(s0)
2005f96e:	a031                	c.j	2005f97a <unwind_frame_rv32i+0x68>
2005f970:	fe442783          	lw	a5,-28(s0)
2005f974:	439c                	c.lw	a5,0(a5)
2005f976:	fcf42c23          	sw	a5,-40(s0)
2005f97a:	fe442783          	lw	a5,-28(s0)
2005f97e:	0007d783          	lhu	a5,0(a5)
2005f982:	fcf41a23          	sh	a5,-44(s0)
2005f986:	fe442783          	lw	a5,-28(s0)
2005f98a:	0789                	c.addi	a5,2
2005f98c:	0007d783          	lhu	a5,0(a5)
2005f990:	fcf41823          	sh	a5,-48(s0)
2005f994:	fd845783          	lhu	a5,-40(s0)
2005f998:	07f7f793          	andi	a5,a5,127
2005f99c:	01079713          	slli	a4,a5,0x10
2005f9a0:	8341                	c.srli	a4,0x10
2005f9a2:	47cd                	c.li	a5,19
2005f9a4:	0af71c63          	bne	a4,a5,2005fa5c <unwind_frame_rv32i+0x14a>
2005f9a8:	fd845703          	lhu	a4,-40(s0)
2005f9ac:	679d                	c.lui	a5,0x7
2005f9ae:	8ff9                	c.and	a5,a4
2005f9b0:	07c2                	c.slli	a5,0x10
2005f9b2:	83c1                	c.srli	a5,0x10
2005f9b4:	e7c5                	c.bnez	a5,2005fa5c <unwind_frame_rv32i+0x14a>
2005f9b6:	fd842703          	lw	a4,-40(s0)
2005f9ba:	6785                	c.lui	a5,0x1
2005f9bc:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005f9c0:	8f7d                	c.and	a4,a5
2005f9c2:	10000793          	addi	a5,zero,256
2005f9c6:	18f71f63          	bne	a4,a5,2005fb64 <unwind_frame_rv32i+0x252>
2005f9ca:	fd842703          	lw	a4,-40(s0)
2005f9ce:	000f87b7          	lui	a5,0xf8
2005f9d2:	8f7d                	c.and	a4,a5
2005f9d4:	67c1                	c.lui	a5,0x10
2005f9d6:	18f71763          	bne	a4,a5,2005fb64 <unwind_frame_rv32i+0x252>
2005f9da:	fd842783          	lw	a5,-40(s0)
2005f9de:	83d1                	c.srli	a5,0x14
2005f9e0:	07c2                	c.slli	a5,0x10
2005f9e2:	83c1                	c.srli	a5,0x10
2005f9e4:	0792                	c.slli	a5,0x4
2005f9e6:	07c2                	c.slli	a5,0x10
2005f9e8:	87c1                	c.srai	a5,0x10
2005f9ea:	8791                	c.srai	a5,0x4
2005f9ec:	07c2                	c.slli	a5,0x10
2005f9ee:	87c1                	c.srai	a5,0x10
2005f9f0:	873e                	c.mv	a4,a5
2005f9f2:	6785                	c.lui	a5,0x1
2005f9f4:	17fd                	c.addi	a5,-1
2005f9f6:	00f776b3          	and	a3,a4,a5
2005f9fa:	fcc45703          	lhu	a4,-52(s0)
2005f9fe:	77fd                	c.lui	a5,0xfffff
2005fa00:	8ff9                	c.and	a5,a4
2005fa02:	873e                	c.mv	a4,a5
2005fa04:	87b6                	c.mv	a5,a3
2005fa06:	8fd9                	c.or	a5,a4
2005fa08:	fcf41623          	sh	a5,-52(s0)
2005fa0c:	fcc42783          	lw	a5,-52(s0)
2005fa10:	0792                	c.slli	a5,0x4
2005fa12:	07c2                	c.slli	a5,0x10
2005fa14:	87c1                	c.srai	a5,0x10
2005fa16:	8791                	c.srai	a5,0x4
2005fa18:	07c2                	c.slli	a5,0x10
2005fa1a:	87c1                	c.srai	a5,0x10
2005fa1c:	fef42023          	sw	a5,-32(s0)
2005fa20:	fe042783          	lw	a5,-32(s0)
2005fa24:	02f05363          	bge	zero,a5,2005fa4a <unwind_frame_rv32i+0x138>
2005fa28:	fe842783          	lw	a5,-24(s0)
2005fa2c:	86be                	c.mv	a3,a5
2005fa2e:	10d00613          	addi	a2,zero,269
2005fa32:	200647b7          	lui	a5,0x20064
2005fa36:	e3078593          	addi	a1,a5,-464 # 20063e30 <__FUNCTION__.3>
2005fa3a:	200647b7          	lui	a5,0x20064
2005fa3e:	c3078513          	addi	a0,a5,-976 # 20063c30 <TaskStateString.0+0x1b8>
2005fa42:	92dff0ef          	jal	ra,2005f36e <DiagPrintf>
2005fa46:	57fd                	c.li	a5,-1
2005fa48:	aa59                	c.j	2005fbde <unwind_frame_rv32i+0x2cc>
2005fa4a:	fe042783          	lw	a5,-32(s0)
2005fa4e:	0007d463          	bge	a5,zero,2005fa56 <unwind_frame_rv32i+0x144>
2005fa52:	40f007b3          	sub	a5,zero,a5
2005fa56:	fef42423          	sw	a5,-24(s0)
2005fa5a:	a229                	c.j	2005fb64 <unwind_frame_rv32i+0x252>
2005fa5c:	fd845783          	lhu	a5,-40(s0)
2005fa60:	07f7f793          	andi	a5,a5,127
2005fa64:	01079713          	slli	a4,a5,0x10
2005fa68:	8341                	c.srli	a4,0x10
2005fa6a:	02300793          	addi	a5,zero,35
2005fa6e:	0cf71463          	bne	a4,a5,2005fb36 <unwind_frame_rv32i+0x224>
2005fa72:	fd845703          	lhu	a4,-40(s0)
2005fa76:	679d                	c.lui	a5,0x7
2005fa78:	8ff9                	c.and	a5,a4
2005fa7a:	01079713          	slli	a4,a5,0x10
2005fa7e:	8341                	c.srli	a4,0x10
2005fa80:	6789                	c.lui	a5,0x2
2005fa82:	0af71a63          	bne	a4,a5,2005fb36 <unwind_frame_rv32i+0x224>
2005fa86:	fd842703          	lw	a4,-40(s0)
2005fa8a:	000f87b7          	lui	a5,0xf8
2005fa8e:	8f7d                	c.and	a4,a5
2005fa90:	67c1                	c.lui	a5,0x10
2005fa92:	0cf71b63          	bne	a4,a5,2005fb68 <unwind_frame_rv32i+0x256>
2005fa96:	fd842703          	lw	a4,-40(s0)
2005fa9a:	01f007b7          	lui	a5,0x1f00
2005fa9e:	8f7d                	c.and	a4,a5
2005faa0:	001007b7          	lui	a5,0x100
2005faa4:	0cf71263          	bne	a4,a5,2005fb68 <unwind_frame_rv32i+0x256>
2005faa8:	fd842783          	lw	a5,-40(s0)
2005faac:	83e5                	c.srli	a5,0x19
2005faae:	0ff7f793          	andi	a5,a5,255
2005fab2:	0796                	c.slli	a5,0x5
2005fab4:	01079713          	slli	a4,a5,0x10
2005fab8:	8741                	c.srai	a4,0x10
2005faba:	fd842783          	lw	a5,-40(s0)
2005fabe:	839d                	c.srli	a5,0x7
2005fac0:	8bfd                	c.andi	a5,31
2005fac2:	0ff7f793          	andi	a5,a5,255
2005fac6:	07c2                	c.slli	a5,0x10
2005fac8:	87c1                	c.srai	a5,0x10
2005faca:	8fd9                	c.or	a5,a4
2005facc:	07c2                	c.slli	a5,0x10
2005face:	87c1                	c.srai	a5,0x10
2005fad0:	0792                	c.slli	a5,0x4
2005fad2:	07c2                	c.slli	a5,0x10
2005fad4:	87c1                	c.srai	a5,0x10
2005fad6:	8791                	c.srai	a5,0x4
2005fad8:	07c2                	c.slli	a5,0x10
2005fada:	87c1                	c.srai	a5,0x10
2005fadc:	873e                	c.mv	a4,a5
2005fade:	6785                	c.lui	a5,0x1
2005fae0:	17fd                	c.addi	a5,-1
2005fae2:	00f776b3          	and	a3,a4,a5
2005fae6:	fc845703          	lhu	a4,-56(s0)
2005faea:	77fd                	c.lui	a5,0xfffff
2005faec:	8ff9                	c.and	a5,a4
2005faee:	873e                	c.mv	a4,a5
2005faf0:	87b6                	c.mv	a5,a3
2005faf2:	8fd9                	c.or	a5,a4
2005faf4:	fcf41423          	sh	a5,-56(s0)
2005faf8:	fc842783          	lw	a5,-56(s0)
2005fafc:	0792                	c.slli	a5,0x4
2005fafe:	07c2                	c.slli	a5,0x10
2005fb00:	87c1                	c.srai	a5,0x10
2005fb02:	8791                	c.srai	a5,0x4
2005fb04:	07c2                	c.slli	a5,0x10
2005fb06:	87c1                	c.srai	a5,0x10
2005fb08:	fef42623          	sw	a5,-20(s0)
2005fb0c:	fec42783          	lw	a5,-20(s0)
2005fb10:	0407dc63          	bge	a5,zero,2005fb68 <unwind_frame_rv32i+0x256>
2005fb14:	fec42783          	lw	a5,-20(s0)
2005fb18:	86be                	c.mv	a3,a5
2005fb1a:	11d00613          	addi	a2,zero,285
2005fb1e:	200647b7          	lui	a5,0x20064
2005fb22:	e3078593          	addi	a1,a5,-464 # 20063e30 <__FUNCTION__.3>
2005fb26:	200647b7          	lui	a5,0x20064
2005fb2a:	c6c78513          	addi	a0,a5,-916 # 20063c6c <TaskStateString.0+0x1f4>
2005fb2e:	841ff0ef          	jal	ra,2005f36e <DiagPrintf>
2005fb32:	57fd                	c.li	a5,-1
2005fb34:	a06d                	c.j	2005fbde <unwind_frame_rv32i+0x2cc>
2005fb36:	fd842703          	lw	a4,-40(s0)
2005fb3a:	67a1                	c.lui	a5,0x8
2005fb3c:	06778793          	addi	a5,a5,103 # 8067 <irq_unregister+0x41>
2005fb40:	06f70063          	beq	a4,a5,2005fba0 <unwind_frame_rv32i+0x28e>
2005fb44:	fd445703          	lhu	a4,-44(s0)
2005fb48:	67a1                	c.lui	a5,0x8
2005fb4a:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fb4e:	00f70963          	beq	a4,a5,2005fb60 <unwind_frame_rv32i+0x24e>
2005fb52:	fd045703          	lhu	a4,-48(s0)
2005fb56:	67a1                	c.lui	a5,0x8
2005fb58:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fb5c:	00f71763          	bne	a4,a5,2005fb6a <unwind_frame_rv32i+0x258>
2005fb60:	57fd                	c.li	a5,-1
2005fb62:	a8b5                	c.j	2005fbde <unwind_frame_rv32i+0x2cc>
2005fb64:	0001                	c.addi	zero,0
2005fb66:	a011                	c.j	2005fb6a <unwind_frame_rv32i+0x258>
2005fb68:	0001                	c.addi	zero,0
2005fb6a:	fe442783          	lw	a5,-28(s0)
2005fb6e:	17f1                	c.addi	a5,-4
2005fb70:	fef42223          	sw	a5,-28(s0)
2005fb74:	fb842783          	lw	a5,-72(s0)
2005fb78:	078a                	c.slli	a5,0x2
2005fb7a:	fe442703          	lw	a4,-28(s0)
2005fb7e:	973e                	c.add	a4,a5
2005fb80:	fbc42783          	lw	a5,-68(s0)
2005fb84:	43dc                	c.lw	a5,4(a5)
2005fb86:	00e7fe63          	bgeu	a5,a4,2005fba2 <unwind_frame_rv32i+0x290>
2005fb8a:	fec42783          	lw	a5,-20(s0)
2005fb8e:	c781                	c.beqz	a5,2005fb96 <unwind_frame_rv32i+0x284>
2005fb90:	fe842783          	lw	a5,-24(s0)
2005fb94:	e799                	c.bnez	a5,2005fba2 <unwind_frame_rv32i+0x290>
2005fb96:	fe442783          	lw	a5,-28(s0)
2005fb9a:	da0798e3          	bne	a5,zero,2005f94a <unwind_frame_rv32i+0x38>
2005fb9e:	a011                	c.j	2005fba2 <unwind_frame_rv32i+0x290>
2005fba0:	0001                	c.addi	zero,0
2005fba2:	fec42783          	lw	a5,-20(s0)
2005fba6:	cb91                	c.beqz	a5,2005fbba <unwind_frame_rv32i+0x2a8>
2005fba8:	fbc42783          	lw	a5,-68(s0)
2005fbac:	4398                	c.lw	a4,0(a5)
2005fbae:	fec42783          	lw	a5,-20(s0)
2005fbb2:	97ba                	c.add	a5,a4
2005fbb4:	439c                	c.lw	a5,0(a5)
2005fbb6:	873e                	c.mv	a4,a5
2005fbb8:	a011                	c.j	2005fbbc <unwind_frame_rv32i+0x2aa>
2005fbba:	4701                	c.li	a4,0
2005fbbc:	fbc42783          	lw	a5,-68(s0)
2005fbc0:	c798                	c.sw	a4,8(a5)
2005fbc2:	fe842703          	lw	a4,-24(s0)
2005fbc6:	fbc42783          	lw	a5,-68(s0)
2005fbca:	c7d8                	c.sw	a4,12(a5)
2005fbcc:	fec42783          	lw	a5,-20(s0)
2005fbd0:	c781                	c.beqz	a5,2005fbd8 <unwind_frame_rv32i+0x2c6>
2005fbd2:	fe842783          	lw	a5,-24(s0)
2005fbd6:	e399                	c.bnez	a5,2005fbdc <unwind_frame_rv32i+0x2ca>
2005fbd8:	57fd                	c.li	a5,-1
2005fbda:	a011                	c.j	2005fbde <unwind_frame_rv32i+0x2cc>
2005fbdc:	4781                	c.li	a5,0
2005fbde:	853e                	c.mv	a0,a5
2005fbe0:	40b6                	c.lwsp	ra,76(sp)
2005fbe2:	4426                	c.lwsp	s0,72(sp)
2005fbe4:	6161                	c.addi16sp	sp,80
2005fbe6:	8082                	c.jr	ra

2005fbe8 <unwind_frame_rv32c>:
2005fbe8:	715d                	c.addi16sp	sp,-80
2005fbea:	c686                	c.swsp	ra,76(sp)
2005fbec:	c4a2                	c.swsp	s0,72(sp)
2005fbee:	0880                	c.addi4spn	s0,sp,80
2005fbf0:	faa42e23          	sw	a0,-68(s0)
2005fbf4:	fab42c23          	sw	a1,-72(s0)
2005fbf8:	fe042623          	sw	zero,-20(s0)
2005fbfc:	fe042423          	sw	zero,-24(s0)
2005fc00:	fbc42783          	lw	a5,-68(s0)
2005fc04:	43dc                	c.lw	a5,4(a5)
2005fc06:	cb91                	c.beqz	a5,2005fc1a <unwind_frame_rv32c+0x32>
2005fc08:	fbc42783          	lw	a5,-68(s0)
2005fc0c:	439c                	c.lw	a5,0(a5)
2005fc0e:	c791                	c.beqz	a5,2005fc1a <unwind_frame_rv32c+0x32>
2005fc10:	fbc42783          	lw	a5,-68(s0)
2005fc14:	43dc                	c.lw	a5,4(a5)
2005fc16:	8b85                	c.andi	a5,1
2005fc18:	c399                	c.beqz	a5,2005fc1e <unwind_frame_rv32c+0x36>
2005fc1a:	57fd                	c.li	a5,-1
2005fc1c:	ae3d                	c.j	2005ff5a <unwind_frame_rv32c+0x372>
2005fc1e:	fbc42783          	lw	a5,-68(s0)
2005fc22:	43dc                	c.lw	a5,4(a5)
2005fc24:	fef42223          	sw	a5,-28(s0)
2005fc28:	a4e1                	c.j	2005fef0 <unwind_frame_rv32c+0x308>
2005fc2a:	fe442783          	lw	a5,-28(s0)
2005fc2e:	0007d783          	lhu	a5,0(a5)
2005fc32:	fcf41c23          	sh	a5,-40(s0)
2005fc36:	fd845703          	lhu	a4,-40(s0)
2005fc3a:	77f9                	c.lui	a5,0xffffe
2005fc3c:	8ff9                	c.and	a5,a4
2005fc3e:	07c2                	c.slli	a5,0x10
2005fc40:	83c1                	c.srli	a5,0x10
2005fc42:	e7e9                	c.bnez	a5,2005fd0c <unwind_frame_rv32c+0x124>
2005fc44:	fd845783          	lhu	a5,-40(s0)
2005fc48:	8b8d                	c.andi	a5,3
2005fc4a:	01079713          	slli	a4,a5,0x10
2005fc4e:	8341                	c.srli	a4,0x10
2005fc50:	4785                	c.li	a5,1
2005fc52:	0af71d63          	bne	a4,a5,2005fd0c <unwind_frame_rv32c+0x124>
2005fc56:	fd845703          	lhu	a4,-40(s0)
2005fc5a:	6785                	c.lui	a5,0x1
2005fc5c:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fc60:	8ff9                	c.and	a5,a4
2005fc62:	01079713          	slli	a4,a5,0x10
2005fc66:	8341                	c.srli	a4,0x10
2005fc68:	10000793          	addi	a5,zero,256
2005fc6c:	0af71063          	bne	a4,a5,2005fd0c <unwind_frame_rv32c+0x124>
2005fc70:	fd842783          	lw	a5,-40(s0)
2005fc74:	83b1                	c.srli	a5,0xc
2005fc76:	8b85                	c.andi	a5,1
2005fc78:	0ff7f793          	andi	a5,a5,255
2005fc7c:	0796                	c.slli	a5,0x5
2005fc7e:	01879713          	slli	a4,a5,0x18
2005fc82:	8761                	c.srai	a4,0x18
2005fc84:	fd842783          	lw	a5,-40(s0)
2005fc88:	8389                	c.srli	a5,0x2
2005fc8a:	8bfd                	c.andi	a5,31
2005fc8c:	0ff7f793          	andi	a5,a5,255
2005fc90:	07e2                	c.slli	a5,0x18
2005fc92:	87e1                	c.srai	a5,0x18
2005fc94:	8fd9                	c.or	a5,a4
2005fc96:	07e2                	c.slli	a5,0x18
2005fc98:	87e1                	c.srai	a5,0x18
2005fc9a:	078a                	c.slli	a5,0x2
2005fc9c:	07e2                	c.slli	a5,0x18
2005fc9e:	87e1                	c.srai	a5,0x18
2005fca0:	8789                	c.srai	a5,0x2
2005fca2:	07e2                	c.slli	a5,0x18
2005fca4:	87e1                	c.srai	a5,0x18
2005fca6:	03f7f693          	andi	a3,a5,63
2005fcaa:	fd444783          	lbu	a5,-44(s0)
2005fcae:	fc07f793          	andi	a5,a5,-64
2005fcb2:	873e                	c.mv	a4,a5
2005fcb4:	87b6                	c.mv	a5,a3
2005fcb6:	8fd9                	c.or	a5,a4
2005fcb8:	fcf40a23          	sb	a5,-44(s0)
2005fcbc:	fd442783          	lw	a5,-44(s0)
2005fcc0:	078a                	c.slli	a5,0x2
2005fcc2:	07e2                	c.slli	a5,0x18
2005fcc4:	87e1                	c.srai	a5,0x18
2005fcc6:	8789                	c.srai	a5,0x2
2005fcc8:	07e2                	c.slli	a5,0x18
2005fcca:	87e1                	c.srai	a5,0x18
2005fccc:	fef42023          	sw	a5,-32(s0)
2005fcd0:	fe042783          	lw	a5,-32(s0)
2005fcd4:	02f05363          	bge	zero,a5,2005fcfa <unwind_frame_rv32c+0x112>
2005fcd8:	fe842783          	lw	a5,-24(s0)
2005fcdc:	86be                	c.mv	a3,a5
2005fcde:	16900613          	addi	a2,zero,361
2005fce2:	200647b7          	lui	a5,0x20064
2005fce6:	e4478593          	addi	a1,a5,-444 # 20063e44 <__FUNCTION__.2>
2005fcea:	200647b7          	lui	a5,0x20064
2005fcee:	c3078513          	addi	a0,a5,-976 # 20063c30 <TaskStateString.0+0x1b8>
2005fcf2:	e7cff0ef          	jal	ra,2005f36e <DiagPrintf>
2005fcf6:	57fd                	c.li	a5,-1
2005fcf8:	a48d                	c.j	2005ff5a <unwind_frame_rv32c+0x372>
2005fcfa:	fe042783          	lw	a5,-32(s0)
2005fcfe:	0007d463          	bge	a5,zero,2005fd06 <unwind_frame_rv32c+0x11e>
2005fd02:	40f007b3          	sub	a5,zero,a5
2005fd06:	fef42423          	sw	a5,-24(s0)
2005fd0a:	aaf1                	c.j	2005fee6 <unwind_frame_rv32c+0x2fe>
2005fd0c:	fd845703          	lhu	a4,-40(s0)
2005fd10:	77f9                	c.lui	a5,0xffffe
2005fd12:	8ff9                	c.and	a5,a4
2005fd14:	01079713          	slli	a4,a5,0x10
2005fd18:	8341                	c.srli	a4,0x10
2005fd1a:	6799                	c.lui	a5,0x6
2005fd1c:	10f71e63          	bne	a4,a5,2005fe38 <unwind_frame_rv32c+0x250>
2005fd20:	fd845783          	lhu	a5,-40(s0)
2005fd24:	8b8d                	c.andi	a5,3
2005fd26:	01079713          	slli	a4,a5,0x10
2005fd2a:	8341                	c.srli	a4,0x10
2005fd2c:	4785                	c.li	a5,1
2005fd2e:	10f71563          	bne	a4,a5,2005fe38 <unwind_frame_rv32c+0x250>
2005fd32:	fd845703          	lhu	a4,-40(s0)
2005fd36:	6785                	c.lui	a5,0x1
2005fd38:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fd3c:	8ff9                	c.and	a5,a4
2005fd3e:	01079713          	slli	a4,a5,0x10
2005fd42:	8341                	c.srli	a4,0x10
2005fd44:	10000793          	addi	a5,zero,256
2005fd48:	0ef71863          	bne	a4,a5,2005fe38 <unwind_frame_rv32c+0x250>
2005fd4c:	fd842783          	lw	a5,-40(s0)
2005fd50:	83b1                	c.srli	a5,0xc
2005fd52:	8b85                	c.andi	a5,1
2005fd54:	0ff7f793          	andi	a5,a5,255
2005fd58:	07a6                	c.slli	a5,0x9
2005fd5a:	01079713          	slli	a4,a5,0x10
2005fd5e:	8741                	c.srai	a4,0x10
2005fd60:	fd842783          	lw	a5,-40(s0)
2005fd64:	8399                	c.srli	a5,0x6
2005fd66:	8b85                	c.andi	a5,1
2005fd68:	0ff7f793          	andi	a5,a5,255
2005fd6c:	0792                	c.slli	a5,0x4
2005fd6e:	07c2                	c.slli	a5,0x10
2005fd70:	87c1                	c.srai	a5,0x10
2005fd72:	8fd9                	c.or	a5,a4
2005fd74:	01079713          	slli	a4,a5,0x10
2005fd78:	8741                	c.srai	a4,0x10
2005fd7a:	fd842783          	lw	a5,-40(s0)
2005fd7e:	8395                	c.srli	a5,0x5
2005fd80:	8b85                	c.andi	a5,1
2005fd82:	0ff7f793          	andi	a5,a5,255
2005fd86:	079a                	c.slli	a5,0x6
2005fd88:	07c2                	c.slli	a5,0x10
2005fd8a:	87c1                	c.srai	a5,0x10
2005fd8c:	8fd9                	c.or	a5,a4
2005fd8e:	01079713          	slli	a4,a5,0x10
2005fd92:	8741                	c.srai	a4,0x10
2005fd94:	fd842783          	lw	a5,-40(s0)
2005fd98:	838d                	c.srli	a5,0x3
2005fd9a:	8b8d                	c.andi	a5,3
2005fd9c:	0ff7f793          	andi	a5,a5,255
2005fda0:	079e                	c.slli	a5,0x7
2005fda2:	07c2                	c.slli	a5,0x10
2005fda4:	87c1                	c.srai	a5,0x10
2005fda6:	8fd9                	c.or	a5,a4
2005fda8:	01079713          	slli	a4,a5,0x10
2005fdac:	8741                	c.srai	a4,0x10
2005fdae:	fd842783          	lw	a5,-40(s0)
2005fdb2:	8389                	c.srli	a5,0x2
2005fdb4:	8b85                	c.andi	a5,1
2005fdb6:	0ff7f793          	andi	a5,a5,255
2005fdba:	0796                	c.slli	a5,0x5
2005fdbc:	07c2                	c.slli	a5,0x10
2005fdbe:	87c1                	c.srai	a5,0x10
2005fdc0:	8fd9                	c.or	a5,a4
2005fdc2:	07c2                	c.slli	a5,0x10
2005fdc4:	87c1                	c.srai	a5,0x10
2005fdc6:	079a                	c.slli	a5,0x6
2005fdc8:	07c2                	c.slli	a5,0x10
2005fdca:	87c1                	c.srai	a5,0x10
2005fdcc:	8799                	c.srai	a5,0x6
2005fdce:	07c2                	c.slli	a5,0x10
2005fdd0:	87c1                	c.srai	a5,0x10
2005fdd2:	3ff7f693          	andi	a3,a5,1023
2005fdd6:	fd045783          	lhu	a5,-48(s0)
2005fdda:	c007f793          	andi	a5,a5,-1024
2005fdde:	873e                	c.mv	a4,a5
2005fde0:	87b6                	c.mv	a5,a3
2005fde2:	8fd9                	c.or	a5,a4
2005fde4:	fcf41823          	sh	a5,-48(s0)
2005fde8:	fd042783          	lw	a5,-48(s0)
2005fdec:	079a                	c.slli	a5,0x6
2005fdee:	07c2                	c.slli	a5,0x10
2005fdf0:	87c1                	c.srai	a5,0x10
2005fdf2:	8799                	c.srai	a5,0x6
2005fdf4:	07c2                	c.slli	a5,0x10
2005fdf6:	87c1                	c.srai	a5,0x10
2005fdf8:	fcf42e23          	sw	a5,-36(s0)
2005fdfc:	fdc42783          	lw	a5,-36(s0)
2005fe00:	02f05363          	bge	zero,a5,2005fe26 <unwind_frame_rv32c+0x23e>
2005fe04:	fe842783          	lw	a5,-24(s0)
2005fe08:	86be                	c.mv	a3,a5
2005fe0a:	18100613          	addi	a2,zero,385
2005fe0e:	200647b7          	lui	a5,0x20064
2005fe12:	e4478593          	addi	a1,a5,-444 # 20063e44 <__FUNCTION__.2>
2005fe16:	200647b7          	lui	a5,0x20064
2005fe1a:	c3078513          	addi	a0,a5,-976 # 20063c30 <TaskStateString.0+0x1b8>
2005fe1e:	d50ff0ef          	jal	ra,2005f36e <DiagPrintf>
2005fe22:	57fd                	c.li	a5,-1
2005fe24:	aa1d                	c.j	2005ff5a <unwind_frame_rv32c+0x372>
2005fe26:	fdc42783          	lw	a5,-36(s0)
2005fe2a:	0007d463          	bge	a5,zero,2005fe32 <unwind_frame_rv32c+0x24a>
2005fe2e:	40f007b3          	sub	a5,zero,a5
2005fe32:	fef42423          	sw	a5,-24(s0)
2005fe36:	a845                	c.j	2005fee6 <unwind_frame_rv32c+0x2fe>
2005fe38:	fd845703          	lhu	a4,-40(s0)
2005fe3c:	77f9                	c.lui	a5,0xffffe
2005fe3e:	8ff9                	c.and	a5,a4
2005fe40:	01079713          	slli	a4,a5,0x10
2005fe44:	8341                	c.srli	a4,0x10
2005fe46:	67b1                	c.lui	a5,0xc
2005fe48:	08f71663          	bne	a4,a5,2005fed4 <unwind_frame_rv32c+0x2ec>
2005fe4c:	fd845783          	lhu	a5,-40(s0)
2005fe50:	8b8d                	c.andi	a5,3
2005fe52:	01079713          	slli	a4,a5,0x10
2005fe56:	8341                	c.srli	a4,0x10
2005fe58:	4789                	c.li	a5,2
2005fe5a:	06f71d63          	bne	a4,a5,2005fed4 <unwind_frame_rv32c+0x2ec>
2005fe5e:	fd845783          	lhu	a5,-40(s0)
2005fe62:	07c7f793          	andi	a5,a5,124
2005fe66:	01079713          	slli	a4,a5,0x10
2005fe6a:	8341                	c.srli	a4,0x10
2005fe6c:	4791                	c.li	a5,4
2005fe6e:	06f71363          	bne	a4,a5,2005fed4 <unwind_frame_rv32c+0x2ec>
2005fe72:	fd842783          	lw	a5,-40(s0)
2005fe76:	839d                	c.srli	a5,0x7
2005fe78:	8b8d                	c.andi	a5,3
2005fe7a:	0ff7f793          	andi	a5,a5,255
2005fe7e:	079a                	c.slli	a5,0x6
2005fe80:	01879713          	slli	a4,a5,0x18
2005fe84:	8761                	c.srai	a4,0x18
2005fe86:	fd842783          	lw	a5,-40(s0)
2005fe8a:	83a5                	c.srli	a5,0x9
2005fe8c:	8bbd                	c.andi	a5,15
2005fe8e:	0ff7f793          	andi	a5,a5,255
2005fe92:	078a                	c.slli	a5,0x2
2005fe94:	07e2                	c.slli	a5,0x18
2005fe96:	87e1                	c.srai	a5,0x18
2005fe98:	8fd9                	c.or	a5,a4
2005fe9a:	07e2                	c.slli	a5,0x18
2005fe9c:	87e1                	c.srai	a5,0x18
2005fe9e:	fcf40623          	sb	a5,-52(s0)
2005fea2:	fcc40783          	lb	a5,-52(s0)
2005fea6:	fef42623          	sw	a5,-20(s0)
2005feaa:	fec42783          	lw	a5,-20(s0)
2005feae:	0207db63          	bge	a5,zero,2005fee4 <unwind_frame_rv32c+0x2fc>
2005feb2:	fec42783          	lw	a5,-20(s0)
2005feb6:	86be                	c.mv	a3,a5
2005feb8:	19100613          	addi	a2,zero,401
2005febc:	200647b7          	lui	a5,0x20064
2005fec0:	e4478593          	addi	a1,a5,-444 # 20063e44 <__FUNCTION__.2>
2005fec4:	200647b7          	lui	a5,0x20064
2005fec8:	c6c78513          	addi	a0,a5,-916 # 20063c6c <TaskStateString.0+0x1f4>
2005fecc:	ca2ff0ef          	jal	ra,2005f36e <DiagPrintf>
2005fed0:	57fd                	c.li	a5,-1
2005fed2:	a061                	c.j	2005ff5a <unwind_frame_rv32c+0x372>
2005fed4:	fd845703          	lhu	a4,-40(s0)
2005fed8:	67a1                	c.lui	a5,0x8
2005feda:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fede:	02f70f63          	beq	a4,a5,2005ff1c <unwind_frame_rv32c+0x334>
2005fee2:	a011                	c.j	2005fee6 <unwind_frame_rv32c+0x2fe>
2005fee4:	0001                	c.addi	zero,0
2005fee6:	fe442783          	lw	a5,-28(s0)
2005feea:	17f9                	c.addi	a5,-2
2005feec:	fef42223          	sw	a5,-28(s0)
2005fef0:	fb842783          	lw	a5,-72(s0)
2005fef4:	0786                	c.slli	a5,0x1
2005fef6:	fe442703          	lw	a4,-28(s0)
2005fefa:	973e                	c.add	a4,a5
2005fefc:	fbc42783          	lw	a5,-68(s0)
2005ff00:	43dc                	c.lw	a5,4(a5)
2005ff02:	00e7fe63          	bgeu	a5,a4,2005ff1e <unwind_frame_rv32c+0x336>
2005ff06:	fec42783          	lw	a5,-20(s0)
2005ff0a:	c781                	c.beqz	a5,2005ff12 <unwind_frame_rv32c+0x32a>
2005ff0c:	fe842783          	lw	a5,-24(s0)
2005ff10:	e799                	c.bnez	a5,2005ff1e <unwind_frame_rv32c+0x336>
2005ff12:	fe442783          	lw	a5,-28(s0)
2005ff16:	d0079ae3          	bne	a5,zero,2005fc2a <unwind_frame_rv32c+0x42>
2005ff1a:	a011                	c.j	2005ff1e <unwind_frame_rv32c+0x336>
2005ff1c:	0001                	c.addi	zero,0
2005ff1e:	fec42783          	lw	a5,-20(s0)
2005ff22:	cb91                	c.beqz	a5,2005ff36 <unwind_frame_rv32c+0x34e>
2005ff24:	fbc42783          	lw	a5,-68(s0)
2005ff28:	4398                	c.lw	a4,0(a5)
2005ff2a:	fec42783          	lw	a5,-20(s0)
2005ff2e:	97ba                	c.add	a5,a4
2005ff30:	439c                	c.lw	a5,0(a5)
2005ff32:	873e                	c.mv	a4,a5
2005ff34:	a011                	c.j	2005ff38 <unwind_frame_rv32c+0x350>
2005ff36:	4701                	c.li	a4,0
2005ff38:	fbc42783          	lw	a5,-68(s0)
2005ff3c:	c798                	c.sw	a4,8(a5)
2005ff3e:	fe842703          	lw	a4,-24(s0)
2005ff42:	fbc42783          	lw	a5,-68(s0)
2005ff46:	c7d8                	c.sw	a4,12(a5)
2005ff48:	fec42783          	lw	a5,-20(s0)
2005ff4c:	c781                	c.beqz	a5,2005ff54 <unwind_frame_rv32c+0x36c>
2005ff4e:	fe842783          	lw	a5,-24(s0)
2005ff52:	e399                	c.bnez	a5,2005ff58 <unwind_frame_rv32c+0x370>
2005ff54:	57fd                	c.li	a5,-1
2005ff56:	a011                	c.j	2005ff5a <unwind_frame_rv32c+0x372>
2005ff58:	4781                	c.li	a5,0
2005ff5a:	853e                	c.mv	a0,a5
2005ff5c:	40b6                	c.lwsp	ra,76(sp)
2005ff5e:	4426                	c.lwsp	s0,72(sp)
2005ff60:	6161                	c.addi16sp	sp,80
2005ff62:	8082                	c.jr	ra

2005ff64 <do_stack_backtrace>:
2005ff64:	7179                	c.addi16sp	sp,-48
2005ff66:	ce52                	c.swsp	s4,28(sp)
2005ff68:	d606                	c.swsp	ra,44(sp)
2005ff6a:	d426                	c.swsp	s1,40(sp)
2005ff6c:	d24a                	c.swsp	s2,36(sp)
2005ff6e:	d04e                	c.swsp	s3,32(sp)
2005ff70:	cc56                	c.swsp	s5,24(sp)
2005ff72:	c402                	c.swsp	zero,8(sp)
2005ff74:	c602                	c.swsp	zero,12(sp)
2005ff76:	c02e                	c.swsp	a1,0(sp)
2005ff78:	8a32                	c.mv	s4,a2
2005ff7a:	c525                	c.beqz	a0,2005ffe2 <do_stack_backtrace+0x7e>
2005ff7c:	c1bd                	c.beqz	a1,2005ffe2 <do_stack_backtrace+0x7e>
2005ff7e:	c22a                	c.swsp	a0,4(sp)
2005ff80:	4712                	c.lwsp	a4,4(sp)
2005ff82:	4682                	c.lwsp	a3,0(sp)
2005ff84:	200644b7          	lui	s1,0x20064
2005ff88:	200649b7          	lui	s3,0x20064
2005ff8c:	ca448513          	addi	a0,s1,-860 # 20063ca4 <TaskStateString.0+0x22c>
2005ff90:	1cc00613          	addi	a2,zero,460
2005ff94:	e5898593          	addi	a1,s3,-424 # 20063e58 <__FUNCTION__.1>
2005ff98:	ca448493          	addi	s1,s1,-860
2005ff9c:	bd2ff0ef          	jal	ra,2005f36e <DiagPrintf>
2005ffa0:	4901                	c.li	s2,0
2005ffa2:	04848493          	addi	s1,s1,72
2005ffa6:	05490b63          	beq	s2,s4,2005fffc <do_stack_backtrace+0x98>
2005ffaa:	40000593          	addi	a1,zero,1024
2005ffae:	850a                	c.mv	a0,sp
2005ffb0:	963ff0ef          	jal	ra,2005f912 <unwind_frame_rv32i>
2005ffb4:	ed15                	c.bnez	a0,2005fff0 <do_stack_backtrace+0x8c>
2005ffb6:	4792                	c.lwsp	a5,4(sp)
2005ffb8:	4702                	c.lwsp	a4,0(sp)
2005ffba:	48a2                	c.lwsp	a7,8(sp)
2005ffbc:	4832                	c.lwsp	a6,12(sp)
2005ffbe:	86ca                	c.mv	a3,s2
2005ffc0:	1d700613          	addi	a2,zero,471
2005ffc4:	e5898593          	addi	a1,s3,-424
2005ffc8:	8526                	c.mv	a0,s1
2005ffca:	ba4ff0ef          	jal	ra,2005f36e <DiagPrintf>
2005ffce:	47a2                	c.lwsp	a5,8(sp)
2005ffd0:	4732                	c.lwsp	a4,12(sp)
2005ffd2:	00190a93          	addi	s5,s2,1
2005ffd6:	c23e                	c.swsp	a5,4(sp)
2005ffd8:	4782                	c.lwsp	a5,0(sp)
2005ffda:	8956                	c.mv	s2,s5
2005ffdc:	97ba                	c.add	a5,a4
2005ffde:	c03e                	c.swsp	a5,0(sp)
2005ffe0:	b7d9                	c.j	2005ffa6 <do_stack_backtrace+0x42>
2005ffe2:	00010793          	addi	a5,sp,0
2005ffe6:	00000717          	auipc	a4,0x0
2005ffea:	c23a                	c.swsp	a4,4(sp)
2005ffec:	c03e                	c.swsp	a5,0(sp)
2005ffee:	bf49                	c.j	2005ff80 <do_stack_backtrace+0x1c>
2005fff0:	40000593          	addi	a1,zero,1024
2005fff4:	850a                	c.mv	a0,sp
2005fff6:	bf3ff0ef          	jal	ra,2005fbe8 <unwind_frame_rv32c>
2005fffa:	dd55                	c.beqz	a0,2005ffb6 <do_stack_backtrace+0x52>
2005fffc:	50b2                	c.lwsp	ra,44(sp)
2005fffe:	54a2                	c.lwsp	s1,40(sp)
20060000:	5982                	c.lwsp	s3,32(sp)
20060002:	4a72                	c.lwsp	s4,28(sp)
20060004:	4ae2                	c.lwsp	s5,24(sp)
20060006:	854a                	c.mv	a0,s2
20060008:	5912                	c.lwsp	s2,36(sp)
2006000a:	6145                	c.addi16sp	sp,48
2006000c:	8082                	c.jr	ra

2006000e <crash_dump>:
2006000e:	711d                	c.addi16sp	sp,-96
20060010:	c8ca                	c.swsp	s2,80(sp)
20060012:	c6ce                	c.swsp	s3,76(sp)
20060014:	c2d6                	c.swsp	s5,68(sp)
20060016:	ce86                	c.swsp	ra,92(sp)
20060018:	cca2                	c.swsp	s0,88(sp)
2006001a:	caa6                	c.swsp	s1,84(sp)
2006001c:	c4d2                	c.swsp	s4,72(sp)
2006001e:	c0da                	c.swsp	s6,64(sp)
20060020:	de5e                	c.swsp	s7,60(sp)
20060022:	dc62                	c.swsp	s8,56(sp)
20060024:	da66                	c.swsp	s9,52(sp)
20060026:	d86a                	c.swsp	s10,48(sp)
20060028:	d66e                	c.swsp	s11,44(sp)
2006002a:	89aa                	c.mv	s3,a0
2006002c:	892e                	c.mv	s2,a1
2006002e:	8ab2                	c.mv	s5,a2
20060030:	34002c73          	csrrs	s8,mscratch,zero
20060034:	34102cf3          	csrrs	s9,mepc,zero
20060038:	34202a73          	csrrs	s4,mcause,zero
2006003c:	34302bf3          	csrrs	s7,mtval,zero
20060040:	20064437          	lui	s0,0x20064
20060044:	e6c40513          	addi	a0,s0,-404 # 20063e6c <__FUNCTION__.1+0x14>
20060048:	200644b7          	lui	s1,0x20064
2006004c:	11200613          	addi	a2,zero,274
20060050:	60848593          	addi	a1,s1,1544 # 20064608 <__FUNCTION__.0>
20060054:	0551                	c.addi	a0,20
20060056:	b18ff0ef          	jal	ra,2005f36e <DiagPrintf>
2006005a:	e901a783          	lw	a5,-368(gp) # 20066818 <crash_task_info>
2006005e:	e6c40413          	addi	s0,s0,-404
20060062:	c399                	c.beqz	a5,20060068 <crash_dump+0x5a>
20060064:	8556                	c.mv	a0,s5
20060066:	9782                	c.jalr	a5
20060068:	473d                	c.li	a4,15
2006006a:	87a2                	c.mv	a5,s0
2006006c:	01476a63          	bltu	a4,s4,20060080 <crash_dump+0x72>
20060070:	20064737          	lui	a4,0x20064
20060074:	002a1793          	slli	a5,s4,0x2
20060078:	72c70713          	addi	a4,a4,1836 # 2006472c <exception_code_string>
2006007c:	97ba                	c.add	a5,a4
2006007e:	439c                	c.lw	a5,0(a5)
20060080:	8752                	c.mv	a4,s4
20060082:	86ce                	c.mv	a3,s3
20060084:	11700613          	addi	a2,zero,279
20060088:	60848593          	addi	a1,s1,1544
2006008c:	05440513          	addi	a0,s0,84
20060090:	adeff0ef          	jal	ra,2005f36e <DiagPrintf>
20060094:	47bd                	c.li	a5,15
20060096:	1547f463          	bgeu	a5,s4,200601de <crash_dump+0x1d0>
2006009a:	12b00613          	addi	a2,zero,299
2006009e:	60848593          	addi	a1,s1,1544
200600a2:	0a440513          	addi	a0,s0,164
200600a6:	ac8ff0ef          	jal	ra,2005f36e <DiagPrintf>
200600aa:	86e2                	c.mv	a3,s8
200600ac:	12c00613          	addi	a2,zero,300
200600b0:	60848593          	addi	a1,s1,1544
200600b4:	0e840513          	addi	a0,s0,232
200600b8:	ab6ff0ef          	jal	ra,2005f36e <DiagPrintf>
200600bc:	86e6                	c.mv	a3,s9
200600be:	12d00613          	addi	a2,zero,301
200600c2:	60848593          	addi	a1,s1,1544
200600c6:	11840513          	addi	a0,s0,280
200600ca:	aa4ff0ef          	jal	ra,2005f36e <DiagPrintf>
200600ce:	86d2                	c.mv	a3,s4
200600d0:	12e00613          	addi	a2,zero,302
200600d4:	60848593          	addi	a1,s1,1544
200600d8:	14840513          	addi	a0,s0,328
200600dc:	a92ff0ef          	jal	ra,2005f36e <DiagPrintf>
200600e0:	86de                	c.mv	a3,s7
200600e2:	12f00613          	addi	a2,zero,303
200600e6:	60848593          	addi	a1,s1,1544
200600ea:	17840513          	addi	a0,s0,376
200600ee:	20064b37          	lui	s6,0x20064
200600f2:	a7cff0ef          	jal	ra,2005f36e <DiagPrintf>
200600f6:	76cb0b13          	addi	s6,s6,1900 # 2006476c <register_string>
200600fa:	4a01                	c.li	s4,0
200600fc:	23440c13          	addi	s8,s0,564
20060100:	02000b93          	addi	s7,zero,32
20060104:	002a1793          	slli	a5,s4,0x2
20060108:	97d6                	c.add	a5,s5
2006010a:	000b2703          	lw	a4,0(s6)
2006010e:	439c                	c.lw	a5,0(a5)
20060110:	86d2                	c.mv	a3,s4
20060112:	13400613          	addi	a2,zero,308
20060116:	60848593          	addi	a1,s1,1544
2006011a:	8562                	c.mv	a0,s8
2006011c:	0a05                	c.addi	s4,1
2006011e:	a50ff0ef          	jal	ra,2005f36e <DiagPrintf>
20060122:	0b11                	c.addi	s6,4
20060124:	ff7a10e3          	bne	s4,s7,20060104 <crash_dump+0xf6>
20060128:	15a00613          	addi	a2,zero,346
2006012c:	60848593          	addi	a1,s1,1544
20060130:	26440513          	addi	a0,s0,612
20060134:	a3aff0ef          	jal	ra,2005f36e <DiagPrintf>
20060138:	4629                	c.li	a2,10
2006013a:	85ca                	c.mv	a1,s2
2006013c:	854e                	c.mv	a0,s3
2006013e:	e27ff0ef          	jal	ra,2005ff64 <do_stack_backtrace>
20060142:	e921                	c.bnez	a0,20060192 <crash_dump+0x184>
20060144:	40000593          	addi	a1,zero,1024
20060148:	850a                	c.mv	a0,sp
2006014a:	c24e                	c.swsp	s3,4(sp)
2006014c:	ca4e                	c.swsp	s3,20(sp)
2006014e:	c402                	c.swsp	zero,8(sp)
20060150:	c602                	c.swsp	zero,12(sp)
20060152:	c04a                	c.swsp	s2,0(sp)
20060154:	cc02                	c.swsp	zero,24(sp)
20060156:	ce02                	c.swsp	zero,28(sp)
20060158:	c84a                	c.swsp	s2,16(sp)
2006015a:	fb8ff0ef          	jal	ra,2005f912 <unwind_frame_rv32i>
2006015e:	40000593          	addi	a1,zero,1024
20060162:	0808                	c.addi4spn	a0,sp,16
20060164:	a85ff0ef          	jal	ra,2005fbe8 <unwind_frame_rv32c>
20060168:	46b2                	c.lwsp	a3,12(sp)
2006016a:	4772                	c.lwsp	a4,28(sp)
2006016c:	89b6                	c.mv	s3,a3
2006016e:	e291                	c.bnez	a3,20060172 <crash_dump+0x164>
20060170:	89ba                	c.mv	s3,a4
20060172:	16c00613          	addi	a2,zero,364
20060176:	60848593          	addi	a1,s1,1544
2006017a:	2a440513          	addi	a0,s0,676
2006017e:	9f0ff0ef          	jal	ra,2005f36e <DiagPrintf>
20060182:	004aa503          	lw	a0,4(s5)
20060186:	ffc9f593          	andi	a1,s3,-4
2006018a:	4629                	c.li	a2,10
2006018c:	95ca                	c.add	a1,s2
2006018e:	dd7ff0ef          	jal	ra,2005ff64 <do_stack_backtrace>
20060192:	60848593          	addi	a1,s1,1544
20060196:	17000613          	addi	a2,zero,368
2006019a:	30c40513          	addi	a0,s0,780
2006019e:	9d0ff0ef          	jal	ra,2005f36e <DiagPrintf>
200601a2:	60848593          	addi	a1,s1,1544
200601a6:	18300613          	addi	a2,zero,387
200601aa:	35440513          	addi	a0,s0,852
200601ae:	9c0ff0ef          	jal	ra,2005f36e <DiagPrintf>
200601b2:	39c40513          	addi	a0,s0,924
200601b6:	4466                	c.lwsp	s0,88(sp)
200601b8:	40f6                	c.lwsp	ra,92(sp)
200601ba:	4946                	c.lwsp	s2,80(sp)
200601bc:	49b6                	c.lwsp	s3,76(sp)
200601be:	4a26                	c.lwsp	s4,72(sp)
200601c0:	4a96                	c.lwsp	s5,68(sp)
200601c2:	4b06                	c.lwsp	s6,64(sp)
200601c4:	5bf2                	c.lwsp	s7,60(sp)
200601c6:	5c62                	c.lwsp	s8,56(sp)
200601c8:	5cd2                	c.lwsp	s9,52(sp)
200601ca:	5d42                	c.lwsp	s10,48(sp)
200601cc:	5db2                	c.lwsp	s11,44(sp)
200601ce:	60848593          	addi	a1,s1,1544
200601d2:	44d6                	c.lwsp	s1,84(sp)
200601d4:	18400613          	addi	a2,zero,388
200601d8:	6125                	c.addi16sp	sp,96
200601da:	994ff06f          	jal	zero,2005f36e <DiagPrintf>
200601de:	20064737          	lui	a4,0x20064
200601e2:	002a1793          	slli	a5,s4,0x2
200601e6:	6ec70713          	addi	a4,a4,1772 # 200646ec <exception_cause_string_tbl>
200601ea:	97ba                	c.add	a5,a4
200601ec:	0007ab03          	lw	s6,0(a5)
200601f0:	ea0b05e3          	beq	s6,zero,2006009a <crash_dump+0x8c>
200601f4:	11d00613          	addi	a2,zero,285
200601f8:	60848593          	addi	a1,s1,1544
200601fc:	1a840513          	addi	a0,s0,424
20060200:	96eff0ef          	jal	ra,2005f36e <DiagPrintf>
20060204:	21040d13          	addi	s10,s0,528
20060208:	1e040d93          	addi	s11,s0,480
2006020c:	000b2683          	lw	a3,0(s6)
20060210:	e80685e3          	beq	a3,zero,2006009a <crash_dump+0x8c>
20060214:	004b2703          	lw	a4,4(s6)
20060218:	cb19                	c.beqz	a4,2006022e <crash_dump+0x220>
2006021a:	87de                	c.mv	a5,s7
2006021c:	12000613          	addi	a2,zero,288
20060220:	60848593          	addi	a1,s1,1544
20060224:	856e                	c.mv	a0,s11
20060226:	948ff0ef          	jal	ra,2005f36e <DiagPrintf>
2006022a:	0b21                	c.addi	s6,8
2006022c:	b7c5                	c.j	2006020c <crash_dump+0x1fe>
2006022e:	12300613          	addi	a2,zero,291
20060232:	60848593          	addi	a1,s1,1544
20060236:	856a                	c.mv	a0,s10
20060238:	936ff0ef          	jal	ra,2005f36e <DiagPrintf>
2006023c:	b7fd                	c.j	2006022a <crash_dump+0x21c>

2006023e <_my_random>:
2006023e:	1141                	c.addi	sp,-16
20060240:	852e                	c.mv	a0,a1
20060242:	85b2                	c.mv	a1,a2
20060244:	c606                	c.swsp	ra,12(sp)
20060246:	db6ff0ef          	jal	ra,2005f7fc <TRNG_get_random_bytes>
2006024a:	40b2                	c.lwsp	ra,12(sp)
2006024c:	4501                	c.li	a0,0
2006024e:	0141                	c.addi	sp,16
20060250:	8082                	c.jr	ra

20060252 <_my_free>:
20060252:	cb4fb06f          	jal	zero,2005b706 <rtos_mem_free>

20060256 <_my_calloc>:
20060256:	02b50533          	mul	a0,a0,a1
2006025a:	c86fb06f          	jal	zero,2005b6e0 <rtos_mem_zmalloc>

2006025e <wifi_set_rom2flash>:
2006025e:	200697b7          	lui	a5,0x20069
20060262:	99878793          	addi	a5,a5,-1640 # 20068998 <wifi_rom_func_map>
20060266:	fef1a623          	sw	a5,-20(gp) # 20066974 <p_wifi_rom_func_map>
2006026a:	20060737          	lui	a4,0x20060
2006026e:	25670713          	addi	a4,a4,598 # 20060256 <_my_calloc>
20060272:	c398                	c.sw	a4,0(a5)
20060274:	20060737          	lui	a4,0x20060
20060278:	25270713          	addi	a4,a4,594 # 20060252 <_my_free>
2006027c:	c3d8                	c.sw	a4,4(a5)
2006027e:	20060737          	lui	a4,0x20060
20060282:	23e70713          	addi	a4,a4,574 # 2006023e <_my_random>
20060286:	c798                	c.sw	a4,8(a5)
20060288:	200697b7          	lui	a5,0x20069
2006028c:	82078793          	addi	a5,a5,-2016 # 20068820 <wifi_rom2flash>
20060290:	fef1a423          	sw	a5,-24(gp) # 20066970 <p_wifi_rom2flash>
20060294:	2005c737          	lui	a4,0x2005c
20060298:	81c70713          	addi	a4,a4,-2020 # 2005b81c <rtos_mutex_give>
2006029c:	c3d8                	c.sw	a4,4(a5)
2006029e:	2005b737          	lui	a4,0x2005b
200602a2:	7d070713          	addi	a4,a4,2000 # 2005b7d0 <rtos_mutex_take>
200602a6:	c398                	c.sw	a4,0(a5)
200602a8:	2005b737          	lui	a4,0x2005b
200602ac:	7b270713          	addi	a4,a4,1970 # 2005b7b2 <rtos_mutex_delete_static>
200602b0:	d798                	c.sw	a4,40(a5)
200602b2:	2005b737          	lui	a4,0x2005b
200602b6:	73870713          	addi	a4,a4,1848 # 2005b738 <rtos_mutex_create_static>
200602ba:	d7d8                	c.sw	a4,44(a5)
200602bc:	2005c737          	lui	a4,0x2005c
200602c0:	95670713          	addi	a4,a4,-1706 # 2005b956 <rtos_sema_give>
200602c4:	c7d8                	c.sw	a4,12(a5)
200602c6:	2005c737          	lui	a4,0x2005c
200602ca:	90a70713          	addi	a4,a4,-1782 # 2005b90a <rtos_sema_take>
200602ce:	cb98                	c.sw	a4,16(a5)
200602d0:	2005b737          	lui	a4,0x2005b
200602d4:	69070713          	addi	a4,a4,1680 # 2005b690 <rtos_critical_enter>
200602d8:	cbd8                	c.sw	a4,20(a5)
200602da:	2005b737          	lui	a4,0x2005b
200602de:	6b470713          	addi	a4,a4,1716 # 2005b6b4 <rtos_critical_exit>
200602e2:	cf98                	c.sw	a4,24(a5)
200602e4:	2005c737          	lui	a4,0x2005c
200602e8:	be070713          	addi	a4,a4,-1056 # 2005bbe0 <rtos_time_delay_ms>
200602ec:	d3d8                	c.sw	a4,36(a5)
200602ee:	2005c737          	lui	a4,0x2005c
200602f2:	c1a70713          	addi	a4,a4,-998 # 2005bc1a <rtos_time_get_current_system_time_ms>
200602f6:	c798                	c.sw	a4,8(a5)
200602f8:	20061737          	lui	a4,0x20061
200602fc:	37670713          	addi	a4,a4,886 # 20061376 <rtw_init_timer>
20060300:	d7b8                	c.sw	a4,104(a5)
20060302:	20061737          	lui	a4,0x20061
20060306:	38070713          	addi	a4,a4,896 # 20061380 <rtw_set_timer>
2006030a:	d7f8                	c.sw	a4,108(a5)
2006030c:	20061737          	lui	a4,0x20061
20060310:	38470713          	addi	a4,a4,900 # 20061384 <rtw_cancel_timer>
20060314:	dbb8                	c.sw	a4,112(a5)
20060316:	20061737          	lui	a4,0x20061
2006031a:	39470713          	addi	a4,a4,916 # 20061394 <rtw_del_timer>
2006031e:	dbf8                	c.sw	a4,116(a5)
20060320:	20059737          	lui	a4,0x20059
20060324:	71a70713          	addi	a4,a4,1818 # 2005971a <pmu_set_sysactive_time>
20060328:	dff8                	c.sw	a4,124(a5)
2006032a:	20061737          	lui	a4,0x20061
2006032e:	3cc70713          	addi	a4,a4,972 # 200613cc <rtw_wakelock_timeout>
20060332:	08e7a023          	sw	a4,128(a5)
20060336:	20061737          	lui	a4,0x20061
2006033a:	39870713          	addi	a4,a4,920 # 20061398 <rtw_acquire_wakelock>
2006033e:	db98                	c.sw	a4,48(a5)
20060340:	20061737          	lui	a4,0x20061
20060344:	3b270713          	addi	a4,a4,946 # 200613b2 <rtw_release_wakelock>
20060348:	dbd8                	c.sw	a4,52(a5)
2006034a:	8082                	c.jr	ra

2006034c <wifi_on>:
2006034c:	1101                	c.addi	sp,-32
2006034e:	ce06                	c.swsp	ra,28(sp)
20060350:	cc22                	c.swsp	s0,24(sp)
20060352:	ca26                	c.swsp	s1,20(sp)
20060354:	842a                	c.mv	s0,a0
20060356:	c84a                	c.swsp	s2,16(sp)
20060358:	90bf90ef          	jal	ra,20059c62 <wifi_set_user_config>
2006035c:	395000ef          	jal	ra,20060ef0 <inic_msg_q_init>
20060360:	43818793          	addi	a5,gp,1080 # 20066dc0 <wifi_user_config>
20060364:	7c102773          	csrrs	a4,0x7c1,zero
20060368:	10077713          	andi	a4,a4,256
2006036c:	c715                	c.beqz	a4,20060398 <wifi_on+0x4c>
2006036e:	0ff0000f          	fence	iorw,iorw
20060372:	04f7f00b          	cache	dwb,(a5)
20060376:	02078713          	addi	a4,a5,32
2006037a:	04e7700b          	cache	dwb,(a4)
2006037e:	0ff0000f          	fence	iorw,iorw
20060382:	0001                	c.addi	zero,0
20060384:	0001                	c.addi	zero,0
20060386:	0001                	c.addi	zero,0
20060388:	0001                	c.addi	zero,0
2006038a:	0001                	c.addi	zero,0
2006038c:	0001                	c.addi	zero,0
2006038e:	0001                	c.addi	zero,0
20060390:	0001                	c.addi	zero,0
20060392:	0001                	c.addi	zero,0
20060394:	0001                	c.addi	zero,0
20060396:	0001                	c.addi	zero,0
20060398:	4605                	c.li	a2,1
2006039a:	006c                	c.addi4spn	a1,sp,12
2006039c:	451d                	c.li	a0,7
2006039e:	c63e                	c.swsp	a5,12(sp)
200603a0:	292d                	c.jal	200607da <inic_api_host_message_send>
200603a2:	059000ef          	jal	ra,20060bfa <inic_host_init_skb>
200603a6:	4605                	c.li	a2,1
200603a8:	006c                	c.addi4spn	a1,sp,12
200603aa:	4525                	c.li	a0,9
200603ac:	c622                	c.swsp	s0,12(sp)
200603ae:	2135                	c.jal	200607da <inic_api_host_message_send>
200603b0:	fcc18793          	addi	a5,gp,-52 # 20066954 <wifi_boot.0>
200603b4:	4398                	c.lw	a4,0(a5)
200603b6:	84aa                	c.mv	s1,a0
200603b8:	ef01                	c.bnez	a4,200603d0 <wifi_on+0x84>
200603ba:	4905                	c.li	s2,1
200603bc:	0127a023          	sw	s2,0(a5)
200603c0:	5b9000ef          	jal	ra,20061178 <init_timer_wrapper>
200603c4:	fb81a783          	lw	a5,-72(gp) # 20066940 <p_wifi_do_fast_connect>
200603c8:	c781                	c.beqz	a5,200603d0 <wifi_on+0x84>
200603ca:	01241363          	bne	s0,s2,200603d0 <wifi_on+0x84>
200603ce:	9782                	c.jalr	a5
200603d0:	40f2                	c.lwsp	ra,28(sp)
200603d2:	4462                	c.lwsp	s0,24(sp)
200603d4:	4942                	c.lwsp	s2,16(sp)
200603d6:	8526                	c.mv	a0,s1
200603d8:	44d2                	c.lwsp	s1,20(sp)
200603da:	6105                	c.addi16sp	sp,32
200603dc:	8082                	c.jr	ra

200603de <wifi_config_autoreconnect>:
200603de:	557d                	c.li	a0,-1
200603e0:	8082                	c.jr	ra

200603e2 <_inic_ipc_ip_addr_update_in_wowlan>:
200603e2:	4505                	c.li	a0,1
200603e4:	8082                	c.jr	ra

200603e6 <inic_api_host_task_h>:
200603e6:	7179                	c.addi16sp	sp,-48
200603e8:	d226                	c.swsp	s1,36(sp)
200603ea:	200654b7          	lui	s1,0x20065
200603ee:	d04a                	c.swsp	s2,32(sp)
200603f0:	ce4e                	c.swsp	s3,28(sp)
200603f2:	cc52                	c.swsp	s4,24(sp)
200603f4:	ca56                	c.swsp	s5,20(sp)
200603f6:	c85a                	c.swsp	s6,16(sp)
200603f8:	c65e                	c.swsp	s7,12(sp)
200603fa:	d606                	c.swsp	ra,44(sp)
200603fc:	d422                	c.swsp	s0,40(sp)
200603fe:	20065a37          	lui	s4,0x20065
20060402:	82048493          	addi	s1,s1,-2016 # 20064820 <register_string+0xb4>
20060406:	fd81a503          	lw	a0,-40(gp) # 20066960 <g_host_inic_api_task_wake_sema>
2006040a:	55fd                	c.li	a1,-1
2006040c:	cfefb0ef          	jal	ra,2005b90a <rtos_sema_take>
20060410:	459d                	c.li	a1,7
20060412:	4541                	c.li	a0,16
20060414:	cddf80ef          	jal	ra,200590f0 <ipc_get_message>
20060418:	4140                	c.lw	s0,4(a0)
2006041a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006041e:	1007f793          	andi	a5,a5,256
20060422:	c3b9                	c.beqz	a5,20060468 <inic_api_host_task_h+0x82>
20060424:	01f47713          	andi	a4,s0,31
20060428:	cb21                	c.beqz	a4,20060478 <inic_api_host_task_h+0x92>
2006042a:	04070693          	addi	a3,a4,64
2006042e:	fe047793          	andi	a5,s0,-32
20060432:	01f7f713          	andi	a4,a5,31
20060436:	9736                	c.add	a4,a3
20060438:	0ff0000f          	fence	iorw,iorw
2006043c:	973e                	c.add	a4,a5
2006043e:	00f7f00b          	cache	dinv,(a5)
20060442:	02078793          	addi	a5,a5,32
20060446:	40f706b3          	sub	a3,a4,a5
2006044a:	fed04ae3          	blt	zero,a3,2006043e <inic_api_host_task_h+0x58>
2006044e:	0ff0000f          	fence	iorw,iorw
20060452:	0001                	c.addi	zero,0
20060454:	0001                	c.addi	zero,0
20060456:	0001                	c.addi	zero,0
20060458:	0001                	c.addi	zero,0
2006045a:	0001                	c.addi	zero,0
2006045c:	0001                	c.addi	zero,0
2006045e:	0001                	c.addi	zero,0
20060460:	0001                	c.addi	zero,0
20060462:	0001                	c.addi	zero,0
20060464:	0001                	c.addi	zero,0
20060466:	0001                	c.addi	zero,0
20060468:	ec01                	c.bnez	s0,20060480 <inic_api_host_task_h+0x9a>
2006046a:	20064537          	lui	a0,0x20064
2006046e:	7ec50513          	addi	a0,a0,2028 # 200647ec <register_string+0x80>
20060472:	f1dfe0ef          	jal	ra,2005f38e <DiagPrintf_minimal>
20060476:	bf41                	c.j	20060406 <inic_api_host_task_h+0x20>
20060478:	87a2                	c.mv	a5,s0
2006047a:	04000693          	addi	a3,zero,64
2006047e:	bf55                	c.j	20060432 <inic_api_host_task_h+0x4c>
20060480:	400c                	c.lw	a1,0(s0)
20060482:	4721                	c.li	a4,8
20060484:	fff58793          	addi	a5,a1,-1
20060488:	34f76063          	bltu	a4,a5,200607c8 <inic_api_host_task_h+0x3e2>
2006048c:	078a                	c.slli	a5,0x2
2006048e:	97a6                	c.add	a5,s1
20060490:	439c                	c.lw	a5,0(a5)
20060492:	8782                	c.jr	a5
20060494:	fc81a783          	lw	a5,-56(gp) # 20066950 <scan_user_callback_ptr>
20060498:	c3ad                	c.beqz	a5,200604fa <inic_api_host_task_h+0x114>
2006049a:	440c                	c.lw	a1,8(s0)
2006049c:	4048                	c.lw	a0,4(s0)
2006049e:	9782                	c.jalr	a5
200604a0:	a8a9                	c.j	200604fa <inic_api_host_task_h+0x114>
200604a2:	4048                	c.lw	a0,4(s0)
200604a4:	440c                	c.lw	a1,8(s0)
200604a6:	7c1027f3          	csrrs	a5,0x7c1,zero
200604aa:	1007f793          	andi	a5,a5,256
200604ae:	c3b9                	c.beqz	a5,200604f4 <inic_api_host_task_h+0x10e>
200604b0:	01f57713          	andi	a4,a0,31
200604b4:	cf59                	c.beqz	a4,20060552 <inic_api_host_task_h+0x16c>
200604b6:	03e70693          	addi	a3,a4,62
200604ba:	fe057793          	andi	a5,a0,-32
200604be:	01f7f713          	andi	a4,a5,31
200604c2:	9736                	c.add	a4,a3
200604c4:	0ff0000f          	fence	iorw,iorw
200604c8:	973e                	c.add	a4,a5
200604ca:	00f7f00b          	cache	dinv,(a5)
200604ce:	02078793          	addi	a5,a5,32
200604d2:	40f706b3          	sub	a3,a4,a5
200604d6:	fed04ae3          	blt	zero,a3,200604ca <inic_api_host_task_h+0xe4>
200604da:	0ff0000f          	fence	iorw,iorw
200604de:	0001                	c.addi	zero,0
200604e0:	0001                	c.addi	zero,0
200604e2:	0001                	c.addi	zero,0
200604e4:	0001                	c.addi	zero,0
200604e6:	0001                	c.addi	zero,0
200604e8:	0001                	c.addi	zero,0
200604ea:	0001                	c.addi	zero,0
200604ec:	0001                	c.addi	zero,0
200604ee:	0001                	c.addi	zero,0
200604f0:	0001                	c.addi	zero,0
200604f2:	0001                	c.addi	zero,0
200604f4:	fc41a783          	lw	a5,-60(gp) # 2006694c <scan_each_report_user_callback_ptr>
200604f8:	f3dd                	c.bnez	a5,2006049e <inic_api_host_task_h+0xb8>
200604fa:	00042023          	sw	zero,0(s0)
200604fe:	7c1027f3          	csrrs	a5,0x7c1,zero
20060502:	1007f793          	andi	a5,a5,256
20060506:	f00780e3          	beq	a5,zero,20060406 <inic_api_host_task_h+0x20>
2006050a:	01f47793          	andi	a5,s0,31
2006050e:	04000713          	addi	a4,zero,64
20060512:	c781                	c.beqz	a5,2006051a <inic_api_host_task_h+0x134>
20060514:	04078713          	addi	a4,a5,64
20060518:	9801                	c.andi	s0,-32
2006051a:	01f47793          	andi	a5,s0,31
2006051e:	97ba                	c.add	a5,a4
20060520:	0ff0000f          	fence	iorw,iorw
20060524:	97a2                	c.add	a5,s0
20060526:	0484700b          	cache	dwb,(s0)
2006052a:	02040413          	addi	s0,s0,32
2006052e:	40878733          	sub	a4,a5,s0
20060532:	fee04ae3          	blt	zero,a4,20060526 <inic_api_host_task_h+0x140>
20060536:	0ff0000f          	fence	iorw,iorw
2006053a:	0001                	c.addi	zero,0
2006053c:	0001                	c.addi	zero,0
2006053e:	0001                	c.addi	zero,0
20060540:	0001                	c.addi	zero,0
20060542:	0001                	c.addi	zero,0
20060544:	0001                	c.addi	zero,0
20060546:	0001                	c.addi	zero,0
20060548:	0001                	c.addi	zero,0
2006054a:	0001                	c.addi	zero,0
2006054c:	0001                	c.addi	zero,0
2006054e:	0001                	c.addi	zero,0
20060550:	bd5d                	c.j	20060406 <inic_api_host_task_h+0x20>
20060552:	87aa                	c.mv	a5,a0
20060554:	03e00693          	addi	a3,zero,62
20060558:	b79d                	c.j	200604be <inic_api_host_task_h+0xd8>
2006055a:	4418                	c.lw	a4,8(s0)
2006055c:	4448                	c.lw	a0,12(s0)
2006055e:	481c                	c.lw	a5,16(s0)
20060560:	4850                	c.lw	a2,20(s0)
20060562:	5014                	c.lw	a3,32(s0)
20060564:	7c1025f3          	csrrs	a1,0x7c1,zero
20060568:	1005f593          	andi	a1,a1,256
2006056c:	c58d                	c.beqz	a1,20060596 <inic_api_host_task_h+0x1b0>
2006056e:	00a775b3          	and	a1,a4,a0
20060572:	587d                	c.li	a6,-1
20060574:	0b059363          	bne	a1,a6,2006061a <inic_api_host_task_h+0x234>
20060578:	7c016773          	csrrsi	a4,0x7c0,2
2006057c:	0ff0000f          	fence	iorw,iorw
20060580:	0001                	c.addi	zero,0
20060582:	0001                	c.addi	zero,0
20060584:	0001                	c.addi	zero,0
20060586:	0001                	c.addi	zero,0
20060588:	0001                	c.addi	zero,0
2006058a:	0001                	c.addi	zero,0
2006058c:	0001                	c.addi	zero,0
2006058e:	0001                	c.addi	zero,0
20060590:	0001                	c.addi	zero,0
20060592:	0001                	c.addi	zero,0
20060594:	0001                	c.addi	zero,0
20060596:	7c102773          	csrrs	a4,0x7c1,zero
2006059a:	10077713          	andi	a4,a4,256
2006059e:	c70d                	c.beqz	a4,200605c8 <inic_api_host_task_h+0x1e2>
200605a0:	00c7f733          	and	a4,a5,a2
200605a4:	55fd                	c.li	a1,-1
200605a6:	0ab71063          	bne	a4,a1,20060646 <inic_api_host_task_h+0x260>
200605aa:	7c0167f3          	csrrsi	a5,0x7c0,2
200605ae:	0ff0000f          	fence	iorw,iorw
200605b2:	0001                	c.addi	zero,0
200605b4:	0001                	c.addi	zero,0
200605b6:	0001                	c.addi	zero,0
200605b8:	0001                	c.addi	zero,0
200605ba:	0001                	c.addi	zero,0
200605bc:	0001                	c.addi	zero,0
200605be:	0001                	c.addi	zero,0
200605c0:	0001                	c.addi	zero,0
200605c2:	0001                	c.addi	zero,0
200605c4:	0001                	c.addi	zero,0
200605c6:	0001                	c.addi	zero,0
200605c8:	7c1027f3          	csrrs	a5,0x7c1,zero
200605cc:	1007f793          	andi	a5,a5,256
200605d0:	d78d                	c.beqz	a5,200604fa <inic_api_host_task_h+0x114>
200605d2:	01f6f713          	andi	a4,a3,31
200605d6:	4799                	c.li	a5,6
200605d8:	c701                	c.beqz	a4,200605e0 <inic_api_host_task_h+0x1fa>
200605da:	00670793          	addi	a5,a4,6
200605de:	9a81                	c.andi	a3,-32
200605e0:	01f6f713          	andi	a4,a3,31
200605e4:	973e                	c.add	a4,a5
200605e6:	0ff0000f          	fence	iorw,iorw
200605ea:	87b6                	c.mv	a5,a3
200605ec:	9736                	c.add	a4,a3
200605ee:	00f7f00b          	cache	dinv,(a5)
200605f2:	02078793          	addi	a5,a5,32
200605f6:	40f706b3          	sub	a3,a4,a5
200605fa:	fed04ae3          	blt	zero,a3,200605ee <inic_api_host_task_h+0x208>
200605fe:	0ff0000f          	fence	iorw,iorw
20060602:	0001                	c.addi	zero,0
20060604:	0001                	c.addi	zero,0
20060606:	0001                	c.addi	zero,0
20060608:	0001                	c.addi	zero,0
2006060a:	0001                	c.addi	zero,0
2006060c:	0001                	c.addi	zero,0
2006060e:	0001                	c.addi	zero,0
20060610:	0001                	c.addi	zero,0
20060612:	0001                	c.addi	zero,0
20060614:	0001                	c.addi	zero,0
20060616:	0001                	c.addi	zero,0
20060618:	b5cd                	c.j	200604fa <inic_api_host_task_h+0x114>
2006061a:	01f77593          	andi	a1,a4,31
2006061e:	c199                	c.beqz	a1,20060624 <inic_api_host_task_h+0x23e>
20060620:	9b01                	c.andi	a4,-32
20060622:	952e                	c.add	a0,a1
20060624:	f6a059e3          	bge	zero,a0,20060596 <inic_api_host_task_h+0x1b0>
20060628:	01f77593          	andi	a1,a4,31
2006062c:	95aa                	c.add	a1,a0
2006062e:	0ff0000f          	fence	iorw,iorw
20060632:	95ba                	c.add	a1,a4
20060634:	00e7700b          	cache	dinv,(a4)
20060638:	02070713          	addi	a4,a4,32
2006063c:	40e58533          	sub	a0,a1,a4
20060640:	fea04ae3          	blt	zero,a0,20060634 <inic_api_host_task_h+0x24e>
20060644:	bf25                	c.j	2006057c <inic_api_host_task_h+0x196>
20060646:	01f7f713          	andi	a4,a5,31
2006064a:	c319                	c.beqz	a4,20060650 <inic_api_host_task_h+0x26a>
2006064c:	9b81                	c.andi	a5,-32
2006064e:	963a                	c.add	a2,a4
20060650:	f6c05ce3          	bge	zero,a2,200605c8 <inic_api_host_task_h+0x1e2>
20060654:	01f7f713          	andi	a4,a5,31
20060658:	9732                	c.add	a4,a2
2006065a:	0ff0000f          	fence	iorw,iorw
2006065e:	973e                	c.add	a4,a5
20060660:	00f7f00b          	cache	dinv,(a5)
20060664:	02078793          	addi	a5,a5,32
20060668:	40f70633          	sub	a2,a4,a5
2006066c:	fec04ae3          	blt	zero,a2,20060660 <inic_api_host_task_h+0x27a>
20060670:	bf3d                	c.j	200605ae <inic_api_host_task_h+0x1c8>
20060672:	fd01a783          	lw	a5,-48(gp) # 20066958 <p_ap_channel_switch_callback>
20060676:	e80782e3          	beq	a5,zero,200604fa <inic_api_host_task_h+0x114>
2006067a:	00840583          	lb	a1,8(s0)
2006067e:	00444503          	lbu	a0,4(s0)
20060682:	9782                	c.jalr	a5
20060684:	bd9d                	c.j	200604fa <inic_api_host_task_h+0x114>
20060686:	440c                	c.lw	a1,8(s0)
20060688:	4450                	c.lw	a2,12(s0)
2006068a:	00444503          	lbu	a0,4(s0)
2006068e:	4814                	c.lw	a3,16(s0)
20060690:	87ae                	c.mv	a5,a1
20060692:	8832                	c.mv	a6,a2
20060694:	7c102773          	csrrs	a4,0x7c1,zero
20060698:	10077713          	andi	a4,a4,256
2006069c:	c70d                	c.beqz	a4,200606c6 <inic_api_host_task_h+0x2e0>
2006069e:	00c5f733          	and	a4,a1,a2
200606a2:	58fd                	c.li	a7,-1
200606a4:	03171463          	bne	a4,a7,200606cc <inic_api_host_task_h+0x2e6>
200606a8:	7c0167f3          	csrrsi	a5,0x7c0,2
200606ac:	0ff0000f          	fence	iorw,iorw
200606b0:	0001                	c.addi	zero,0
200606b2:	0001                	c.addi	zero,0
200606b4:	0001                	c.addi	zero,0
200606b6:	0001                	c.addi	zero,0
200606b8:	0001                	c.addi	zero,0
200606ba:	0001                	c.addi	zero,0
200606bc:	0001                	c.addi	zero,0
200606be:	0001                	c.addi	zero,0
200606c0:	0001                	c.addi	zero,0
200606c2:	0001                	c.addi	zero,0
200606c4:	0001                	c.addi	zero,0
200606c6:	ad4fe0ef          	jal	ra,2005e99a <wifi_indication>
200606ca:	bd05                	c.j	200604fa <inic_api_host_task_h+0x114>
200606cc:	01f5f713          	andi	a4,a1,31
200606d0:	c709                	c.beqz	a4,200606da <inic_api_host_task_h+0x2f4>
200606d2:	fe05f793          	andi	a5,a1,-32
200606d6:	00e60833          	add	a6,a2,a4
200606da:	ff0056e3          	bge	zero,a6,200606c6 <inic_api_host_task_h+0x2e0>
200606de:	01f7f713          	andi	a4,a5,31
200606e2:	9742                	c.add	a4,a6
200606e4:	0ff0000f          	fence	iorw,iorw
200606e8:	973e                	c.add	a4,a5
200606ea:	00f7f00b          	cache	dinv,(a5)
200606ee:	02078793          	addi	a5,a5,32
200606f2:	40f70833          	sub	a6,a4,a5
200606f6:	ff004ae3          	blt	zero,a6,200606ea <inic_api_host_task_h+0x304>
200606fa:	bf4d                	c.j	200606ac <inic_api_host_task_h+0x2c6>
200606fc:	fc01a783          	lw	a5,-64(gp) # 20066948 <promisc_user_callback_ptr>
20060700:	4501                	c.li	a0,0
20060702:	cbc1                	c.beqz	a5,20060792 <inic_api_host_task_h+0x3ac>
20060704:	4058                	c.lw	a4,4(s0)
20060706:	7c1027f3          	csrrs	a5,0x7c1,zero
2006070a:	1007f793          	andi	a5,a5,256
2006070e:	c3b9                	c.beqz	a5,20060754 <inic_api_host_task_h+0x36e>
20060710:	01f77693          	andi	a3,a4,31
20060714:	c2c9                	c.beqz	a3,20060796 <inic_api_host_task_h+0x3b0>
20060716:	01068613          	addi	a2,a3,16
2006071a:	fe077793          	andi	a5,a4,-32
2006071e:	01f7f693          	andi	a3,a5,31
20060722:	96b2                	c.add	a3,a2
20060724:	0ff0000f          	fence	iorw,iorw
20060728:	96be                	c.add	a3,a5
2006072a:	00f7f00b          	cache	dinv,(a5)
2006072e:	02078793          	addi	a5,a5,32
20060732:	40f68633          	sub	a2,a3,a5
20060736:	fec04ae3          	blt	zero,a2,2006072a <inic_api_host_task_h+0x344>
2006073a:	0ff0000f          	fence	iorw,iorw
2006073e:	0001                	c.addi	zero,0
20060740:	0001                	c.addi	zero,0
20060742:	0001                	c.addi	zero,0
20060744:	0001                	c.addi	zero,0
20060746:	0001                	c.addi	zero,0
20060748:	0001                	c.addi	zero,0
2006074a:	0001                	c.addi	zero,0
2006074c:	0001                	c.addi	zero,0
2006074e:	0001                	c.addi	zero,0
20060750:	0001                	c.addi	zero,0
20060752:	0001                	c.addi	zero,0
20060754:	471c                	c.lw	a5,8(a4)
20060756:	4754                	c.lw	a3,12(a4)
20060758:	7c102773          	csrrs	a4,0x7c1,zero
2006075c:	10077713          	andi	a4,a4,256
20060760:	c70d                	c.beqz	a4,2006078a <inic_api_host_task_h+0x3a4>
20060762:	577d                	c.li	a4,-1
20060764:	02e79c63          	bne	a5,a4,2006079c <inic_api_host_task_h+0x3b6>
20060768:	02f69a63          	bne	a3,a5,2006079c <inic_api_host_task_h+0x3b6>
2006076c:	7c0167f3          	csrrsi	a5,0x7c0,2
20060770:	0ff0000f          	fence	iorw,iorw
20060774:	0001                	c.addi	zero,0
20060776:	0001                	c.addi	zero,0
20060778:	0001                	c.addi	zero,0
2006077a:	0001                	c.addi	zero,0
2006077c:	0001                	c.addi	zero,0
2006077e:	0001                	c.addi	zero,0
20060780:	0001                	c.addi	zero,0
20060782:	0001                	c.addi	zero,0
20060784:	0001                	c.addi	zero,0
20060786:	0001                	c.addi	zero,0
20060788:	0001                	c.addi	zero,0
2006078a:	fc01a783          	lw	a5,-64(gp) # 20066948 <promisc_user_callback_ptr>
2006078e:	4048                	c.lw	a0,4(s0)
20060790:	9782                	c.jalr	a5
20060792:	d048                	c.sw	a0,36(s0)
20060794:	b39d                	c.j	200604fa <inic_api_host_task_h+0x114>
20060796:	87ba                	c.mv	a5,a4
20060798:	4641                	c.li	a2,16
2006079a:	b751                	c.j	2006071e <inic_api_host_task_h+0x338>
2006079c:	01f7f713          	andi	a4,a5,31
200607a0:	c319                	c.beqz	a4,200607a6 <inic_api_host_task_h+0x3c0>
200607a2:	9b81                	c.andi	a5,-32
200607a4:	96ba                	c.add	a3,a4
200607a6:	fed052e3          	bge	zero,a3,2006078a <inic_api_host_task_h+0x3a4>
200607aa:	01f7f713          	andi	a4,a5,31
200607ae:	9736                	c.add	a4,a3
200607b0:	0ff0000f          	fence	iorw,iorw
200607b4:	973e                	c.add	a4,a5
200607b6:	00f7f00b          	cache	dinv,(a5)
200607ba:	02078793          	addi	a5,a5,32
200607be:	40f706b3          	sub	a3,a4,a5
200607c2:	fed04ae3          	blt	zero,a3,200607b6 <inic_api_host_task_h+0x3d0>
200607c6:	b76d                	c.j	20060770 <inic_api_host_task_h+0x38a>
200607c8:	804a0513          	addi	a0,s4,-2044 # 20064804 <register_string+0x98>
200607cc:	bc3fe0ef          	jal	ra,2005f38e <DiagPrintf_minimal>
200607d0:	b32d                	c.j	200604fa <inic_api_host_task_h+0x114>

200607d2 <inic_api_host_int_hdl>:
200607d2:	fd81a503          	lw	a0,-40(gp) # 20066960 <g_host_inic_api_task_wake_sema>
200607d6:	980fb06f          	jal	zero,2005b956 <rtos_sema_give>

200607da <inic_api_host_message_send>:
200607da:	7139                	c.addi16sp	sp,-64
200607dc:	d452                	c.swsp	s4,40(sp)
200607de:	d256                	c.swsp	s5,36(sp)
200607e0:	fca1ae23          	sw	a0,-36(gp) # 20066964 <latest_api_id>
200607e4:	d05a                	c.swsp	s6,32(sp)
200607e6:	8b2a                	c.mv	s6,a0
200607e8:	fd41a503          	lw	a0,-44(gp) # 2006695c <g_host_inic_api_message_send_sema>
200607ec:	cc62                	c.swsp	s8,24(sp)
200607ee:	20068c37          	lui	s8,0x20068
200607f2:	da26                	c.swsp	s1,52(sp)
200607f4:	d64e                	c.swsp	s3,44(sp)
200607f6:	ca66                	c.swsp	s9,20(sp)
200607f8:	89ae                	c.mv	s3,a1
200607fa:	740c0493          	addi	s1,s8,1856 # 20068740 <g_host_ipc_api_request_info>
200607fe:	55fd                	c.li	a1,-1
20060800:	6c8d                	c.lui	s9,0x3
20060802:	ce5e                	c.swsp	s7,28(sp)
20060804:	c86a                	c.swsp	s10,16(sp)
20060806:	c66e                	c.swsp	s11,12(sp)
20060808:	de06                	c.swsp	ra,60(sp)
2006080a:	dc22                	c.swsp	s0,56(sp)
2006080c:	d84a                	c.swsp	s2,48(sp)
2006080e:	8bb2                	c.mv	s7,a2
20060810:	fdc18a93          	addi	s5,gp,-36 # 20066964 <latest_api_id>
20060814:	8f6fb0ef          	jal	ra,2005b90a <rtos_sema_take>
20060818:	fd418a13          	addi	s4,gp,-44 # 2006695c <g_host_inic_api_message_send_sema>
2006081c:	02048d93          	addi	s11,s1,32
20060820:	ee0c8c93          	addi	s9,s9,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
20060824:	20065d37          	lui	s10,0x20065
20060828:	4401                	c.li	s0,0
2006082a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006082e:	1007f793          	andi	a5,a5,256
20060832:	740c0913          	addi	s2,s8,1856
20060836:	c785                	c.beqz	a5,2006085e <inic_api_host_message_send+0x84>
20060838:	0ff0000f          	fence	iorw,iorw
2006083c:	0094f00b          	cache	dinv,(s1)
20060840:	01bdf00b          	cache	dinv,(s11)
20060844:	0ff0000f          	fence	iorw,iorw
20060848:	0001                	c.addi	zero,0
2006084a:	0001                	c.addi	zero,0
2006084c:	0001                	c.addi	zero,0
2006084e:	0001                	c.addi	zero,0
20060850:	0001                	c.addi	zero,0
20060852:	0001                	c.addi	zero,0
20060854:	0001                	c.addi	zero,0
20060856:	0001                	c.addi	zero,0
20060858:	0001                	c.addi	zero,0
2006085a:	0001                	c.addi	zero,0
2006085c:	0001                	c.addi	zero,0
2006085e:	00092783          	lw	a5,0(s2)
20060862:	cf89                	c.beqz	a5,2006087c <inic_api_host_message_send+0xa2>
20060864:	4505                	c.li	a0,1
20060866:	0405                	c.addi	s0,1
20060868:	b78fb0ef          	jal	ra,2005bbe0 <rtos_time_delay_ms>
2006086c:	03946433          	rem	s0,s0,s9
20060870:	fc4d                	c.bnez	s0,2006082a <inic_api_host_message_send+0x50>
20060872:	844d0513          	addi	a0,s10,-1980 # 20064844 <register_string+0xd8>
20060876:	b19fe0ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2006087a:	b77d                	c.j	20060828 <inic_api_host_message_send+0x4e>
2006087c:	04000613          	addi	a2,zero,64
20060880:	4581                	c.li	a1,0
20060882:	740c0513          	addi	a0,s8,1856
20060886:	dffa9097          	auipc	ra,0xdffa9
2006088a:	248080e7          	jalr	ra,584(ra) # 9ace <__wrap_memset>
2006088e:	01692023          	sw	s6,0(s2)
20060892:	00098b63          	beq	s3,zero,200608a8 <inic_api_host_message_send+0xce>
20060896:	002b9613          	slli	a2,s7,0x2
2006089a:	85ce                	c.mv	a1,s3
2006089c:	00490513          	addi	a0,s2,4
200608a0:	dffa9097          	auipc	ra,0xdffa9
200608a4:	230080e7          	jalr	ra,560(ra) # 9ad0 <__wrap_memcpy>
200608a8:	7c1027f3          	csrrs	a5,0x7c1,zero
200608ac:	1007f793          	andi	a5,a5,256
200608b0:	c795                	c.beqz	a5,200608dc <inic_api_host_message_send+0x102>
200608b2:	0ff0000f          	fence	iorw,iorw
200608b6:	0494f00b          	cache	dwb,(s1)
200608ba:	02048793          	addi	a5,s1,32
200608be:	04f7f00b          	cache	dwb,(a5)
200608c2:	0ff0000f          	fence	iorw,iorw
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
200608da:	0001                	c.addi	zero,0
200608dc:	200689b7          	lui	s3,0x20068
200608e0:	4641                	c.li	a2,16
200608e2:	4581                	c.li	a1,0
200608e4:	70098513          	addi	a0,s3,1792 # 20068700 <g_host_ipc_api_msg>
200608e8:	dffa9097          	auipc	ra,0xdffa9
200608ec:	1e6080e7          	jalr	ra,486(ra) # 9ace <__wrap_memset>
200608f0:	70098b13          	addi	s6,s3,1792
200608f4:	04000793          	addi	a5,zero,64
200608f8:	009b2223          	sw	s1,4(s6)
200608fc:	000b2023          	sw	zero,0(s6)
20060900:	00fb2423          	sw	a5,8(s6)
20060904:	7c1027f3          	csrrs	a5,0x7c1,zero
20060908:	1007f793          	andi	a5,a5,256
2006090c:	c395                	c.beqz	a5,20060930 <inic_api_host_message_send+0x156>
2006090e:	0ff0000f          	fence	iorw,iorw
20060912:	056b700b          	cache	dwb,(s6)
20060916:	0ff0000f          	fence	iorw,iorw
2006091a:	0001                	c.addi	zero,0
2006091c:	0001                	c.addi	zero,0
2006091e:	0001                	c.addi	zero,0
20060920:	0001                	c.addi	zero,0
20060922:	0001                	c.addi	zero,0
20060924:	0001                	c.addi	zero,0
20060926:	0001                	c.addi	zero,0
20060928:	0001                	c.addi	zero,0
2006092a:	0001                	c.addi	zero,0
2006092c:	0001                	c.addi	zero,0
2006092e:	0001                	c.addi	zero,0
20060930:	70098613          	addi	a2,s3,1792
20060934:	459d                	c.li	a1,7
20060936:	4501                	c.li	a0,0
20060938:	698d                	c.lui	s3,0x3
2006093a:	e6ef80ef          	jal	ra,20058fa8 <ipc_send_message>
2006093e:	02048b93          	addi	s7,s1,32
20060942:	ee098993          	addi	s3,s3,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
20060946:	20065b37          	lui	s6,0x20065
2006094a:	00092783          	lw	a5,0(s2)
2006094e:	cba9                	c.beqz	a5,200609a0 <inic_api_host_message_send+0x1c6>
20060950:	4505                	c.li	a0,1
20060952:	a8efb0ef          	jal	ra,2005bbe0 <rtos_time_delay_ms>
20060956:	7c1027f3          	csrrs	a5,0x7c1,zero
2006095a:	1007f793          	andi	a5,a5,256
2006095e:	c785                	c.beqz	a5,20060986 <inic_api_host_message_send+0x1ac>
20060960:	0ff0000f          	fence	iorw,iorw
20060964:	0094f00b          	cache	dinv,(s1)
20060968:	017bf00b          	cache	dinv,(s7)
2006096c:	0ff0000f          	fence	iorw,iorw
20060970:	0001                	c.addi	zero,0
20060972:	0001                	c.addi	zero,0
20060974:	0001                	c.addi	zero,0
20060976:	0001                	c.addi	zero,0
20060978:	0001                	c.addi	zero,0
2006097a:	0001                	c.addi	zero,0
2006097c:	0001                	c.addi	zero,0
2006097e:	0001                	c.addi	zero,0
20060980:	0001                	c.addi	zero,0
20060982:	0001                	c.addi	zero,0
20060984:	0001                	c.addi	zero,0
20060986:	0405                	c.addi	s0,1
20060988:	03346433          	rem	s0,s0,s3
2006098c:	fc5d                	c.bnez	s0,2006094a <inic_api_host_message_send+0x170>
2006098e:	000aa603          	lw	a2,0(s5)
20060992:	00092583          	lw	a1,0(s2)
20060996:	85cb0513          	addi	a0,s6,-1956 # 2006485c <register_string+0xf0>
2006099a:	9f5fe0ef          	jal	ra,2005f38e <DiagPrintf_minimal>
2006099e:	b775                	c.j	2006094a <inic_api_host_message_send+0x170>
200609a0:	000a2503          	lw	a0,0(s4)
200609a4:	02892403          	lw	s0,40(s2)
200609a8:	faffa0ef          	jal	ra,2005b956 <rtos_sema_give>
200609ac:	50f2                	c.lwsp	ra,60(sp)
200609ae:	8522                	c.mv	a0,s0
200609b0:	5462                	c.lwsp	s0,56(sp)
200609b2:	54d2                	c.lwsp	s1,52(sp)
200609b4:	5942                	c.lwsp	s2,48(sp)
200609b6:	59b2                	c.lwsp	s3,44(sp)
200609b8:	5a22                	c.lwsp	s4,40(sp)
200609ba:	5a92                	c.lwsp	s5,36(sp)
200609bc:	5b02                	c.lwsp	s6,32(sp)
200609be:	4bf2                	c.lwsp	s7,28(sp)
200609c0:	4c62                	c.lwsp	s8,24(sp)
200609c2:	4cd2                	c.lwsp	s9,20(sp)
200609c4:	4d42                	c.lwsp	s10,16(sp)
200609c6:	4db2                	c.lwsp	s11,12(sp)
200609c8:	6121                	c.addi16sp	sp,64
200609ca:	8082                	c.jr	ra

200609cc <inic_api_init_host>:
200609cc:	1141                	c.addi	sp,-16
200609ce:	567d                	c.li	a2,-1
200609d0:	4581                	c.li	a1,0
200609d2:	fd818513          	addi	a0,gp,-40 # 20066960 <g_host_inic_api_task_wake_sema>
200609d6:	c606                	c.swsp	ra,12(sp)
200609d8:	c422                	c.swsp	s0,8(sp)
200609da:	eb3fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
200609de:	567d                	c.li	a2,-1
200609e0:	4581                	c.li	a1,0
200609e2:	fd418413          	addi	s0,gp,-44 # 2006695c <g_host_inic_api_message_send_sema>
200609e6:	fd418513          	addi	a0,gp,-44 # 2006695c <g_host_inic_api_message_send_sema>
200609ea:	ea3fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
200609ee:	4008                	c.lw	a0,0(s0)
200609f0:	f67fa0ef          	jal	ra,2005b956 <rtos_sema_give>
200609f4:	200605b7          	lui	a1,0x20060
200609f8:	4701                	c.li	a4,0
200609fa:	4681                	c.li	a3,0
200609fc:	4601                	c.li	a2,0
200609fe:	3e258593          	addi	a1,a1,994 # 200603e2 <_inic_ipc_ip_addr_update_in_wowlan>
20060a02:	4505                	c.li	a0,1
20060a04:	d83f80ef          	jal	ra,20059786 <pmu_register_sleep_callback>
20060a08:	6705                	c.lui	a4,0x1
20060a0a:	20060637          	lui	a2,0x20060
20060a0e:	200655b7          	lui	a1,0x20065
20060a12:	478d                	c.li	a5,3
20060a14:	ce070713          	addi	a4,a4,-800 # ce0 <CPU_ClkGet+0x1ca>
20060a18:	4681                	c.li	a3,0
20060a1a:	3e660613          	addi	a2,a2,998 # 200603e6 <inic_api_host_task_h>
20060a1e:	88858593          	addi	a1,a1,-1912 # 20064888 <register_string+0x11c>
20060a22:	4501                	c.li	a0,0
20060a24:	986fb0ef          	jal	ra,2005bbaa <rtos_task_create>
20060a28:	c10d                	c.beqz	a0,20060a4a <inic_api_init_host+0x7e>
20060a2a:	4422                	c.lwsp	s0,8(sp)
20060a2c:	40b2                	c.lwsp	ra,12(sp)
20060a2e:	200656b7          	lui	a3,0x20065
20060a32:	200655b7          	lui	a1,0x20065
20060a36:	89c68693          	addi	a3,a3,-1892 # 2006489c <register_string+0x130>
20060a3a:	04500613          	addi	a2,zero,69
20060a3e:	8b858593          	addi	a1,a1,-1864 # 200648b8 <register_string+0x14c>
20060a42:	4509                	c.li	a0,2
20060a44:	0141                	c.addi	sp,16
20060a46:	f30fa06f          	jal	zero,2005b176 <rtk_log_write>
20060a4a:	40b2                	c.lwsp	ra,12(sp)
20060a4c:	4422                	c.lwsp	s0,8(sp)
20060a4e:	0141                	c.addi	sp,16
20060a50:	8082                	c.jr	ra

20060a52 <inic_mp_command>:
20060a52:	7179                	c.addi16sp	sp,-48
20060a54:	ce4e                	c.swsp	s3,28(sp)
20060a56:	89aa                	c.mv	s3,a0
20060a58:	6505                	c.lui	a0,0x1
20060a5a:	d04a                	c.swsp	s2,32(sp)
20060a5c:	cc52                	c.swsp	s4,24(sp)
20060a5e:	d606                	c.swsp	ra,44(sp)
20060a60:	d422                	c.swsp	s0,40(sp)
20060a62:	d226                	c.swsp	s1,36(sp)
20060a64:	892e                	c.mv	s2,a1
20060a66:	8a32                	c.mv	s4,a2
20060a68:	c79fa0ef          	jal	ra,2005b6e0 <rtos_mem_zmalloc>
20060a6c:	e50d                	c.bnez	a0,20060a96 <inic_mp_command+0x44>
20060a6e:	5422                	c.lwsp	s0,40(sp)
20060a70:	50b2                	c.lwsp	ra,44(sp)
20060a72:	5492                	c.lwsp	s1,36(sp)
20060a74:	5902                	c.lwsp	s2,32(sp)
20060a76:	49f2                	c.lwsp	s3,28(sp)
20060a78:	4a62                	c.lwsp	s4,24(sp)
20060a7a:	200656b7          	lui	a3,0x20065
20060a7e:	200655b7          	lui	a1,0x20065
20060a82:	8c068693          	addi	a3,a3,-1856 # 200648c0 <register_string+0x154>
20060a86:	04500613          	addi	a2,zero,69
20060a8a:	8b858593          	addi	a1,a1,-1864 # 200648b8 <register_string+0x14c>
20060a8e:	4509                	c.li	a0,2
20060a90:	6145                	c.addi16sp	sp,48
20060a92:	ee4fa06f          	jal	zero,2005b176 <rtk_log_write>
20060a96:	842a                	c.mv	s0,a0
20060a98:	87ce                	c.mv	a5,s3
20060a9a:	c04e                	c.swsp	s3,0(sp)
20060a9c:	c24a                	c.swsp	s2,4(sp)
20060a9e:	c452                	c.swsp	s4,8(sp)
20060aa0:	84aa                	c.mv	s1,a0
20060aa2:	c62a                	c.swsp	a0,12(sp)
20060aa4:	7c102773          	csrrs	a4,0x7c1,zero
20060aa8:	10077713          	andi	a4,a4,256
20060aac:	c721                	c.beqz	a4,20060af4 <inic_mp_command+0xa2>
20060aae:	01f57693          	andi	a3,a0,31
20060ab2:	10068363          	beq	a3,zero,20060bb8 <inic_mp_command+0x166>
20060ab6:	6605                	c.lui	a2,0x1
20060ab8:	9636                	c.add	a2,a3
20060aba:	fe057713          	andi	a4,a0,-32
20060abe:	01f77693          	andi	a3,a4,31
20060ac2:	96b2                	c.add	a3,a2
20060ac4:	0ff0000f          	fence	iorw,iorw
20060ac8:	96ba                	c.add	a3,a4
20060aca:	04e7700b          	cache	dwb,(a4)
20060ace:	02070713          	addi	a4,a4,32
20060ad2:	40e68633          	sub	a2,a3,a4
20060ad6:	fec04ae3          	blt	zero,a2,20060aca <inic_mp_command+0x78>
20060ada:	0ff0000f          	fence	iorw,iorw
20060ade:	0001                	c.addi	zero,0
20060ae0:	0001                	c.addi	zero,0
20060ae2:	0001                	c.addi	zero,0
20060ae4:	0001                	c.addi	zero,0
20060ae6:	0001                	c.addi	zero,0
20060ae8:	0001                	c.addi	zero,0
20060aea:	0001                	c.addi	zero,0
20060aec:	0001                	c.addi	zero,0
20060aee:	0001                	c.addi	zero,0
20060af0:	0001                	c.addi	zero,0
20060af2:	0001                	c.addi	zero,0
20060af4:	7c102773          	csrrs	a4,0x7c1,zero
20060af8:	10077713          	andi	a4,a4,256
20060afc:	c70d                	c.beqz	a4,20060b26 <inic_mp_command+0xd4>
20060afe:	577d                	c.li	a4,-1
20060b00:	0ae99f63          	bne	s3,a4,20060bbe <inic_mp_command+0x16c>
20060b04:	0b391d63          	bne	s2,s3,20060bbe <inic_mp_command+0x16c>
20060b08:	7c0267f3          	csrrsi	a5,0x7c0,4
20060b0c:	0ff0000f          	fence	iorw,iorw
20060b10:	0001                	c.addi	zero,0
20060b12:	0001                	c.addi	zero,0
20060b14:	0001                	c.addi	zero,0
20060b16:	0001                	c.addi	zero,0
20060b18:	0001                	c.addi	zero,0
20060b1a:	0001                	c.addi	zero,0
20060b1c:	0001                	c.addi	zero,0
20060b1e:	0001                	c.addi	zero,0
20060b20:	0001                	c.addi	zero,0
20060b22:	0001                	c.addi	zero,0
20060b24:	0001                	c.addi	zero,0
20060b26:	650d                	c.lui	a0,0x3
20060b28:	4611                	c.li	a2,4
20060b2a:	858a                	c.mv	a1,sp
20060b2c:	050d                	c.addi	a0,3
20060b2e:	cadff0ef          	jal	ra,200607da <inic_api_host_message_send>
20060b32:	060a0863          	beq	s4,zero,20060ba2 <inic_mp_command+0x150>
20060b36:	7c1027f3          	csrrs	a5,0x7c1,zero
20060b3a:	1007f793          	andi	a5,a5,256
20060b3e:	c7a1                	c.beqz	a5,20060b86 <inic_mp_command+0x134>
20060b40:	01f47713          	andi	a4,s0,31
20060b44:	6785                	c.lui	a5,0x1
20060b46:	c701                	c.beqz	a4,20060b4e <inic_mp_command+0xfc>
20060b48:	97ba                	c.add	a5,a4
20060b4a:	fe047493          	andi	s1,s0,-32
20060b4e:	01f4f713          	andi	a4,s1,31
20060b52:	973e                	c.add	a4,a5
20060b54:	0ff0000f          	fence	iorw,iorw
20060b58:	87a6                	c.mv	a5,s1
20060b5a:	9726                	c.add	a4,s1
20060b5c:	00f7f00b          	cache	dinv,(a5)
20060b60:	02078793          	addi	a5,a5,32 # 1020 <CPU_ClkGet+0x50a>
20060b64:	40f706b3          	sub	a3,a4,a5
20060b68:	fed04ae3          	blt	zero,a3,20060b5c <inic_mp_command+0x10a>
20060b6c:	0ff0000f          	fence	iorw,iorw
20060b70:	0001                	c.addi	zero,0
20060b72:	0001                	c.addi	zero,0
20060b74:	0001                	c.addi	zero,0
20060b76:	0001                	c.addi	zero,0
20060b78:	0001                	c.addi	zero,0
20060b7a:	0001                	c.addi	zero,0
20060b7c:	0001                	c.addi	zero,0
20060b7e:	0001                	c.addi	zero,0
20060b80:	0001                	c.addi	zero,0
20060b82:	0001                	c.addi	zero,0
20060b84:	0001                	c.addi	zero,0
20060b86:	200656b7          	lui	a3,0x20065
20060b8a:	200655b7          	lui	a1,0x20065
20060b8e:	8722                	c.mv	a4,s0
20060b90:	8d868693          	addi	a3,a3,-1832 # 200648d8 <register_string+0x16c>
20060b94:	04100613          	addi	a2,zero,65
20060b98:	8b858593          	addi	a1,a1,-1864 # 200648b8 <register_string+0x14c>
20060b9c:	4505                	c.li	a0,1
20060b9e:	dd8fa0ef          	jal	ra,2005b176 <rtk_log_write>
20060ba2:	8522                	c.mv	a0,s0
20060ba4:	b63fa0ef          	jal	ra,2005b706 <rtos_mem_free>
20060ba8:	50b2                	c.lwsp	ra,44(sp)
20060baa:	5422                	c.lwsp	s0,40(sp)
20060bac:	5492                	c.lwsp	s1,36(sp)
20060bae:	5902                	c.lwsp	s2,32(sp)
20060bb0:	49f2                	c.lwsp	s3,28(sp)
20060bb2:	4a62                	c.lwsp	s4,24(sp)
20060bb4:	6145                	c.addi16sp	sp,48
20060bb6:	8082                	c.jr	ra
20060bb8:	872a                	c.mv	a4,a0
20060bba:	6605                	c.lui	a2,0x1
20060bbc:	b709                	c.j	20060abe <inic_mp_command+0x6c>
20060bbe:	01f9f713          	andi	a4,s3,31
20060bc2:	c701                	c.beqz	a4,20060bca <inic_mp_command+0x178>
20060bc4:	fe09f793          	andi	a5,s3,-32
20060bc8:	993a                	c.add	s2,a4
20060bca:	f5205ee3          	bge	zero,s2,20060b26 <inic_mp_command+0xd4>
20060bce:	01f7f593          	andi	a1,a5,31
20060bd2:	992e                	c.add	s2,a1
20060bd4:	0ff0000f          	fence	iorw,iorw
20060bd8:	993e                	c.add	s2,a5
20060bda:	04f7f00b          	cache	dwb,(a5)
20060bde:	02078793          	addi	a5,a5,32
20060be2:	40f90733          	sub	a4,s2,a5
20060be6:	fee04ae3          	blt	zero,a4,20060bda <inic_mp_command+0x188>
20060bea:	b70d                	c.j	20060b0c <inic_mp_command+0xba>

20060bec <inic_host_init>:
20060bec:	1141                	c.addi	sp,-16
20060bee:	c606                	c.swsp	ra,12(sp)
20060bf0:	2239                	c.jal	20060cfe <inic_host_init_priv>
20060bf2:	40b2                	c.lwsp	ra,12(sp)
20060bf4:	0141                	c.addi	sp,16
20060bf6:	dd7ff06f          	jal	zero,200609cc <inic_api_init_host>

20060bfa <inic_host_init_skb>:
20060bfa:	1141                	c.addi	sp,-16
20060bfc:	c422                	c.swsp	s0,8(sp)
20060bfe:	fe01a783          	lw	a5,-32(gp) # 20066968 <host_skb_buff>
20060c02:	c606                	c.swsp	ra,12(sp)
20060c04:	c226                	c.swsp	s1,4(sp)
20060c06:	c395                	c.beqz	a5,20060c2a <inic_host_init_skb+0x30>
20060c08:	4422                	c.lwsp	s0,8(sp)
20060c0a:	40b2                	c.lwsp	ra,12(sp)
20060c0c:	4492                	c.lwsp	s1,4(sp)
20060c0e:	200656b7          	lui	a3,0x20065
20060c12:	200655b7          	lui	a1,0x20065
20060c16:	8f068693          	addi	a3,a3,-1808 # 200648f0 <register_string+0x184>
20060c1a:	04500613          	addi	a2,zero,69
20060c1e:	8b858593          	addi	a1,a1,-1864 # 200648b8 <register_string+0x14c>
20060c22:	4509                	c.li	a0,2
20060c24:	0141                	c.addi	sp,16
20060c26:	d50fa06f          	jal	zero,2005b176 <rtk_log_write>
20060c2a:	66000793          	addi	a5,zero,1632
20060c2e:	fe418713          	addi	a4,gp,-28 # 2006696c <skb_buf_max_size>
20060c32:	c31c                	c.sw	a5,0(a4)
20060c34:	43818793          	addi	a5,gp,1080 # 20066dc0 <wifi_user_config>
20060c38:	579c                	c.lw	a5,40(a5)
20060c3a:	fe018413          	addi	s0,gp,-32 # 20066968 <host_skb_buff>
20060c3e:	43818493          	addi	s1,gp,1080 # 20066dc0 <wifi_user_config>
20060c42:	c781                	c.beqz	a5,20060c4a <inic_host_init_skb+0x50>
20060c44:	07fd                	c.addi	a5,31
20060c46:	9b81                	c.andi	a5,-32
20060c48:	c31c                	c.sw	a5,0(a4)
20060c4a:	50dc                	c.lw	a5,36(s1)
20060c4c:	6c000513          	addi	a0,zero,1728
20060c50:	02f50533          	mul	a0,a0,a5
20060c54:	a8dfa0ef          	jal	ra,2005b6e0 <rtos_mem_zmalloc>
20060c58:	c008                	c.sw	a0,0(s0)
20060c5a:	e115                	c.bnez	a0,20060c7e <inic_host_init_skb+0x84>
20060c5c:	20065737          	lui	a4,0x20065
20060c60:	200656b7          	lui	a3,0x20065
20060c64:	200655b7          	lui	a1,0x20065
20060c68:	93c70713          	addi	a4,a4,-1732 # 2006493c <__func__.1>
20060c6c:	90868693          	addi	a3,a3,-1784 # 20064908 <register_string+0x19c>
20060c70:	04500613          	addi	a2,zero,69
20060c74:	8b858593          	addi	a1,a1,-1864 # 200648b8 <register_string+0x14c>
20060c78:	4509                	c.li	a0,2
20060c7a:	cfcfa0ef          	jal	ra,2005b176 <rtk_log_write>
20060c7e:	4681                	c.li	a3,0
20060c80:	6c000613          	addi	a2,zero,1728
20060c84:	50d8                	c.lw	a4,36(s1)
20060c86:	401c                	c.lw	a5,0(s0)
20060c88:	06e6c263          	blt	a3,a4,20060cec <inic_host_init_skb+0xf2>
20060c8c:	7c1026f3          	csrrs	a3,0x7c1,zero
20060c90:	1006f693          	andi	a3,a3,256
20060c94:	c6b9                	c.beqz	a3,20060ce2 <inic_host_init_skb+0xe8>
20060c96:	6c000693          	addi	a3,zero,1728
20060c9a:	02d70733          	mul	a4,a4,a3
20060c9e:	01f7f693          	andi	a3,a5,31
20060ca2:	c299                	c.beqz	a3,20060ca8 <inic_host_init_skb+0xae>
20060ca4:	9b81                	c.andi	a5,-32
20060ca6:	9736                	c.add	a4,a3
20060ca8:	02e05d63          	bge	zero,a4,20060ce2 <inic_host_init_skb+0xe8>
20060cac:	01f7f693          	andi	a3,a5,31
20060cb0:	9736                	c.add	a4,a3
20060cb2:	0ff0000f          	fence	iorw,iorw
20060cb6:	973e                	c.add	a4,a5
20060cb8:	04f7f00b          	cache	dwb,(a5)
20060cbc:	02078793          	addi	a5,a5,32
20060cc0:	40f706b3          	sub	a3,a4,a5
20060cc4:	fed04ae3          	blt	zero,a3,20060cb8 <inic_host_init_skb+0xbe>
20060cc8:	0ff0000f          	fence	iorw,iorw
20060ccc:	0001                	c.addi	zero,0
20060cce:	0001                	c.addi	zero,0
20060cd0:	0001                	c.addi	zero,0
20060cd2:	0001                	c.addi	zero,0
20060cd4:	0001                	c.addi	zero,0
20060cd6:	0001                	c.addi	zero,0
20060cd8:	0001                	c.addi	zero,0
20060cda:	0001                	c.addi	zero,0
20060cdc:	0001                	c.addi	zero,0
20060cde:	0001                	c.addi	zero,0
20060ce0:	0001                	c.addi	zero,0
20060ce2:	40b2                	c.lwsp	ra,12(sp)
20060ce4:	4422                	c.lwsp	s0,8(sp)
20060ce6:	4492                	c.lwsp	s1,4(sp)
20060ce8:	0141                	c.addi	sp,16
20060cea:	8082                	c.jr	ra
20060cec:	02c68733          	mul	a4,a3,a2
20060cf0:	0685                	c.addi	a3,1
20060cf2:	97ba                	c.add	a5,a4
20060cf4:	c39c                	c.sw	a5,0(a5)
20060cf6:	401c                	c.lw	a5,0(s0)
20060cf8:	97ba                	c.add	a5,a4
20060cfa:	c3dc                	c.sw	a5,4(a5)
20060cfc:	b761                	c.j	20060c84 <inic_host_init_skb+0x8a>

20060cfe <inic_host_init_priv>:
20060cfe:	1141                	c.addi	sp,-16
20060d00:	c226                	c.swsp	s1,4(sp)
20060d02:	200684b7          	lui	s1,0x20068
20060d06:	02800613          	addi	a2,zero,40
20060d0a:	4581                	c.li	a1,0
20060d0c:	78048513          	addi	a0,s1,1920 # 20068780 <g_inic_host_priv>
20060d10:	c606                	c.swsp	ra,12(sp)
20060d12:	c422                	c.swsp	s0,8(sp)
20060d14:	dffa9097          	auipc	ra,0xdffa9
20060d18:	dba080e7          	jalr	ra,-582(ra) # 9ace <__wrap_memset>
20060d1c:	567d                	c.li	a2,-1
20060d1e:	4581                	c.li	a1,0
20060d20:	78048513          	addi	a0,s1,1920
20060d24:	78048413          	addi	s0,s1,1920
20060d28:	b65fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060d2c:	567d                	c.li	a2,-1
20060d2e:	4581                	c.li	a1,0
20060d30:	00440513          	addi	a0,s0,4
20060d34:	b59fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060d38:	567d                	c.li	a2,-1
20060d3a:	4581                	c.li	a1,0
20060d3c:	00840513          	addi	a0,s0,8
20060d40:	b4dfa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060d44:	4408                	c.lw	a0,8(s0)
20060d46:	c11fa0ef          	jal	ra,2005b956 <rtos_sema_give>
20060d4a:	00c40513          	addi	a0,s0,12
20060d4e:	2ea5                	c.jal	200610c6 <rtw_init_queue>
20060d50:	40b2                	c.lwsp	ra,12(sp)
20060d52:	00042c23          	sw	zero,24(s0)
20060d56:	00042e23          	sw	zero,28(s0)
20060d5a:	02042023          	sw	zero,32(s0)
20060d5e:	02042223          	sw	zero,36(s0)
20060d62:	4422                	c.lwsp	s0,8(sp)
20060d64:	4492                	c.lwsp	s1,4(sp)
20060d66:	0141                	c.addi	sp,16
20060d68:	8082                	c.jr	ra

20060d6a <inic_host_trx_event_hdl>:
20060d6a:	4791                	c.li	a5,4
20060d6c:	872a                	c.mv	a4,a0
20060d6e:	00f50f63          	beq	a0,a5,20060d8c <inic_host_trx_event_hdl+0x22>
20060d72:	200656b7          	lui	a3,0x20065
20060d76:	200655b7          	lui	a1,0x20065
20060d7a:	92068693          	addi	a3,a3,-1760 # 20064920 <register_string+0x1b4>
20060d7e:	04500613          	addi	a2,zero,69
20060d82:	8b858593          	addi	a1,a1,-1864 # 200648b8 <register_string+0x14c>
20060d86:	4509                	c.li	a0,2
20060d88:	beefa06f          	jal	zero,2005b176 <rtk_log_write>
20060d8c:	8082                	c.jr	ra

20060d8e <inic_host_trx_int_hdl>:
20060d8e:	1141                	c.addi	sp,-16
20060d90:	4599                	c.li	a1,6
20060d92:	4541                	c.li	a0,16
20060d94:	c422                	c.swsp	s0,8(sp)
20060d96:	c606                	c.swsp	ra,12(sp)
20060d98:	b58f80ef          	jal	ra,200590f0 <ipc_get_message>
20060d9c:	4140                	c.lw	s0,4(a0)
20060d9e:	7c1027f3          	csrrs	a5,0x7c1,zero
20060da2:	1007f793          	andi	a5,a5,256
20060da6:	c3b9                	c.beqz	a5,20060dec <inic_host_trx_int_hdl+0x5e>
20060da8:	01f47713          	andi	a4,s0,31
20060dac:	c755                	c.beqz	a4,20060e58 <inic_host_trx_int_hdl+0xca>
20060dae:	04070693          	addi	a3,a4,64
20060db2:	fe047793          	andi	a5,s0,-32
20060db6:	01f7f713          	andi	a4,a5,31
20060dba:	9736                	c.add	a4,a3
20060dbc:	0ff0000f          	fence	iorw,iorw
20060dc0:	973e                	c.add	a4,a5
20060dc2:	00f7f00b          	cache	dinv,(a5)
20060dc6:	02078793          	addi	a5,a5,32
20060dca:	40f706b3          	sub	a3,a4,a5
20060dce:	fed04ae3          	blt	zero,a3,20060dc2 <inic_host_trx_int_hdl+0x34>
20060dd2:	0ff0000f          	fence	iorw,iorw
20060dd6:	0001                	c.addi	zero,0
20060dd8:	0001                	c.addi	zero,0
20060dda:	0001                	c.addi	zero,0
20060ddc:	0001                	c.addi	zero,0
20060dde:	0001                	c.addi	zero,0
20060de0:	0001                	c.addi	zero,0
20060de2:	0001                	c.addi	zero,0
20060de4:	0001                	c.addi	zero,0
20060de6:	0001                	c.addi	zero,0
20060de8:	0001                	c.addi	zero,0
20060dea:	0001                	c.addi	zero,0
20060dec:	2455                	c.jal	20061090 <inic_msg_get_queue_status>
20060dee:	c92d                	c.beqz	a0,20060e60 <inic_host_trx_int_hdl+0xd2>
20060df0:	8522                	c.mv	a0,s0
20060df2:	2af1                	c.jal	20060fce <inic_msg_enqueue>
20060df4:	157d                	c.addi	a0,-1
20060df6:	00a03533          	sltu	a0,zero,a0
20060dfa:	c408                	c.sw	a0,8(s0)
20060dfc:	00042023          	sw	zero,0(s0)
20060e00:	7c1027f3          	csrrs	a5,0x7c1,zero
20060e04:	1007f793          	andi	a5,a5,256
20060e08:	c7a1                	c.beqz	a5,20060e50 <inic_host_trx_int_hdl+0xc2>
20060e0a:	01f47793          	andi	a5,s0,31
20060e0e:	04000713          	addi	a4,zero,64
20060e12:	c781                	c.beqz	a5,20060e1a <inic_host_trx_int_hdl+0x8c>
20060e14:	04078713          	addi	a4,a5,64
20060e18:	9801                	c.andi	s0,-32
20060e1a:	01f47793          	andi	a5,s0,31
20060e1e:	97ba                	c.add	a5,a4
20060e20:	0ff0000f          	fence	iorw,iorw
20060e24:	97a2                	c.add	a5,s0
20060e26:	0484700b          	cache	dwb,(s0)
20060e2a:	02040413          	addi	s0,s0,32
20060e2e:	40878733          	sub	a4,a5,s0
20060e32:	fee04ae3          	blt	zero,a4,20060e26 <inic_host_trx_int_hdl+0x98>
20060e36:	0ff0000f          	fence	iorw,iorw
20060e3a:	0001                	c.addi	zero,0
20060e3c:	0001                	c.addi	zero,0
20060e3e:	0001                	c.addi	zero,0
20060e40:	0001                	c.addi	zero,0
20060e42:	0001                	c.addi	zero,0
20060e44:	0001                	c.addi	zero,0
20060e46:	0001                	c.addi	zero,0
20060e48:	0001                	c.addi	zero,0
20060e4a:	0001                	c.addi	zero,0
20060e4c:	0001                	c.addi	zero,0
20060e4e:	0001                	c.addi	zero,0
20060e50:	40b2                	c.lwsp	ra,12(sp)
20060e52:	4422                	c.lwsp	s0,8(sp)
20060e54:	0141                	c.addi	sp,16
20060e56:	8082                	c.jr	ra
20060e58:	87a2                	c.mv	a5,s0
20060e5a:	04000693          	addi	a3,zero,64
20060e5e:	bfa1                	c.j	20060db6 <inic_host_trx_int_hdl+0x28>
20060e60:	00044503          	lbu	a0,0(s0)
20060e64:	00c44603          	lbu	a2,12(s0)
20060e68:	404c                	c.lw	a1,4(s0)
20060e6a:	f01ff0ef          	jal	ra,20060d6a <inic_host_trx_event_hdl>
20060e6e:	4501                	c.li	a0,0
20060e70:	b769                	c.j	20060dfa <inic_host_trx_int_hdl+0x6c>

20060e72 <inic_msg_q_task>:
20060e72:	1101                	c.addi	sp,-32
20060e74:	cc22                	c.swsp	s0,24(sp)
20060e76:	20069437          	lui	s0,0x20069
20060e7a:	c84a                	c.swsp	s2,16(sp)
20060e7c:	c64e                	c.swsp	s3,12(sp)
20060e7e:	80040913          	addi	s2,s0,-2048 # 20068800 <g_ipc_msg_q_priv>
20060e82:	ce06                	c.swsp	ra,28(sp)
20060e84:	ca26                	c.swsp	s1,20(sp)
20060e86:	80040413          	addi	s0,s0,-2048
20060e8a:	4985                	c.li	s3,1
20060e8c:	00c92503          	lw	a0,12(s2)
20060e90:	55fd                	c.li	a1,-1
20060e92:	a79fa0ef          	jal	ra,2005b90a <rtos_sema_take>
20060e96:	ffafa0ef          	jal	ra,2005b690 <rtos_critical_enter>
20060e9a:	8522                	c.mv	a0,s0
20060e9c:	2c25                	c.jal	200610d4 <rtw_queue_empty>
20060e9e:	03351063          	bne	a0,s3,20060ebe <inic_msg_q_task+0x4c>
20060ea2:	813fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20060ea6:	01444783          	lbu	a5,20(s0)
20060eaa:	f3ed                	c.bnez	a5,20060e8c <inic_msg_q_task+0x1a>
20060eac:	4462                	c.lwsp	s0,24(sp)
20060eae:	40f2                	c.lwsp	ra,28(sp)
20060eb0:	44d2                	c.lwsp	s1,20(sp)
20060eb2:	4942                	c.lwsp	s2,16(sp)
20060eb4:	49b2                	c.lwsp	s3,12(sp)
20060eb6:	4501                	c.li	a0,0
20060eb8:	6105                	c.addi16sp	sp,32
20060eba:	d15fa06f          	jal	zero,2005bbce <rtos_task_delete>
20060ebe:	4004                	c.lw	s1,0(s0)
20060ec0:	8526                	c.mv	a0,s1
20060ec2:	2add                	c.jal	200610b8 <rtw_list_delete>
20060ec4:	ff0fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20060ec8:	dcf9                	c.beqz	s1,20060ea6 <inic_msg_q_task+0x34>
20060eca:	44c8                	c.lw	a0,12(s1)
20060ecc:	448c                	c.lw	a1,8(s1)
20060ece:	00155613          	srli	a2,a0,0x1
20060ed2:	810d                	c.srli	a0,0x3
20060ed4:	8a0d                	c.andi	a2,3
20060ed6:	897d                	c.andi	a0,31
20060ed8:	e93ff0ef          	jal	ra,20060d6a <inic_host_trx_event_hdl>
20060edc:	fb4fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
20060ee0:	00c4c783          	lbu	a5,12(s1)
20060ee4:	9bf9                	c.andi	a5,-2
20060ee6:	00f48623          	sb	a5,12(s1)
20060eea:	fcafa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20060eee:	b765                	c.j	20060e96 <inic_msg_q_task+0x24>

20060ef0 <inic_msg_q_init>:
20060ef0:	1141                	c.addi	sp,-16
20060ef2:	c422                	c.swsp	s0,8(sp)
20060ef4:	20069437          	lui	s0,0x20069
20060ef8:	02000613          	addi	a2,zero,32
20060efc:	4581                	c.li	a1,0
20060efe:	80040513          	addi	a0,s0,-2048 # 20068800 <g_ipc_msg_q_priv>
20060f02:	c606                	c.swsp	ra,12(sp)
20060f04:	c226                	c.swsp	s1,4(sp)
20060f06:	dffa9097          	auipc	ra,0xdffa9
20060f0a:	bc8080e7          	jalr	ra,-1080(ra) # 9ace <__wrap_memset>
20060f0e:	20068537          	lui	a0,0x20068
20060f12:	04000613          	addi	a2,zero,64
20060f16:	4581                	c.li	a1,0
20060f18:	7c050513          	addi	a0,a0,1984 # 200687c0 <g_inic_ipc_ex_msg>
20060f1c:	dffa9097          	auipc	ra,0xdffa9
20060f20:	bb2080e7          	jalr	ra,-1102(ra) # 9ace <__wrap_memset>
20060f24:	80040513          	addi	a0,s0,-2048
20060f28:	80040493          	addi	s1,s0,-2048
20060f2c:	2a69                	c.jal	200610c6 <rtw_init_queue>
20060f2e:	567d                	c.li	a2,-1
20060f30:	4581                	c.li	a1,0
20060f32:	00c48513          	addi	a0,s1,12
20060f36:	957fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060f3a:	567d                	c.li	a2,-1
20060f3c:	4581                	c.li	a1,0
20060f3e:	01048513          	addi	a0,s1,16
20060f42:	94bfa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060f46:	4888                	c.lw	a0,16(s1)
20060f48:	80040413          	addi	s0,s0,-2048
20060f4c:	a0bfa0ef          	jal	ra,2005b956 <rtos_sema_give>
20060f50:	4581a503          	lw	a0,1112(gp) # 20066de0 <wifi_user_config+0x20>
20060f54:	00a48e23          	sb	a0,28(s1)
20060f58:	0ff57513          	andi	a0,a0,255
20060f5c:	0512                	c.slli	a0,0x4
20060f5e:	f82fa0ef          	jal	ra,2005b6e0 <rtos_mem_zmalloc>
20060f62:	cc88                	c.sw	a0,24(s1)
20060f64:	4781                	c.li	a5,0
20060f66:	01c44703          	lbu	a4,28(s0)
20060f6a:	04e7c763          	blt	a5,a4,20060fb8 <inic_msg_q_init+0xc8>
20060f6e:	20061637          	lui	a2,0x20061
20060f72:	200655b7          	lui	a1,0x20065
20060f76:	4799                	c.li	a5,6
20060f78:	41800713          	addi	a4,zero,1048
20060f7c:	4681                	c.li	a3,0
20060f7e:	e7260613          	addi	a2,a2,-398 # 20060e72 <inic_msg_q_task>
20060f82:	95058593          	addi	a1,a1,-1712 # 20064950 <__func__.1+0x14>
20060f86:	4501                	c.li	a0,0
20060f88:	c23fa0ef          	jal	ra,2005bbaa <rtos_task_create>
20060f8c:	cd11                	c.beqz	a0,20060fa8 <inic_msg_q_init+0xb8>
20060f8e:	200656b7          	lui	a3,0x20065
20060f92:	200655b7          	lui	a1,0x20065
20060f96:	96068693          	addi	a3,a3,-1696 # 20064960 <__func__.1+0x24>
20060f9a:	04500613          	addi	a2,zero,69
20060f9e:	8b858593          	addi	a1,a1,-1864 # 200648b8 <register_string+0x14c>
20060fa2:	4509                	c.li	a0,2
20060fa4:	9d2fa0ef          	jal	ra,2005b176 <rtk_log_write>
20060fa8:	4785                	c.li	a5,1
20060faa:	40b2                	c.lwsp	ra,12(sp)
20060fac:	00f40a23          	sb	a5,20(s0)
20060fb0:	4422                	c.lwsp	s0,8(sp)
20060fb2:	4492                	c.lwsp	s1,4(sp)
20060fb4:	0141                	c.addi	sp,16
20060fb6:	8082                	c.jr	ra
20060fb8:	4c18                	c.lw	a4,24(s0)
20060fba:	00479693          	slli	a3,a5,0x4
20060fbe:	0785                	c.addi	a5,1
20060fc0:	9736                	c.add	a4,a3
20060fc2:	00c74683          	lbu	a3,12(a4)
20060fc6:	9af9                	c.andi	a3,-2
20060fc8:	00d70623          	sb	a3,12(a4)
20060fcc:	bf69                	c.j	20060f66 <inic_msg_q_init+0x76>

20060fce <inic_msg_enqueue>:
20060fce:	1101                	c.addi	sp,-32
20060fd0:	ca26                	c.swsp	s1,20(sp)
20060fd2:	c84a                	c.swsp	s2,16(sp)
20060fd4:	c64e                	c.swsp	s3,12(sp)
20060fd6:	200694b7          	lui	s1,0x20069
20060fda:	ce06                	c.swsp	ra,28(sp)
20060fdc:	cc22                	c.swsp	s0,24(sp)
20060fde:	89aa                	c.mv	s3,a0
20060fe0:	eb0fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
20060fe4:	80048793          	addi	a5,s1,-2048 # 20068800 <g_ipc_msg_q_priv>
20060fe8:	01c7c683          	lbu	a3,28(a5)
20060fec:	4f90                	c.lw	a2,24(a5)
20060fee:	80048913          	addi	s2,s1,-2048
20060ff2:	4781                	c.li	a5,0
20060ff4:	00d7c463          	blt	a5,a3,20060ffc <inic_msg_enqueue+0x2e>
20060ff8:	4401                	c.li	s0,0
20060ffa:	a829                	c.j	20061014 <inic_msg_enqueue+0x46>
20060ffc:	00479413          	slli	s0,a5,0x4
20061000:	9432                	c.add	s0,a2
20061002:	00c44703          	lbu	a4,12(s0)
20061006:	00177593          	andi	a1,a4,1
2006100a:	e1b1                	c.bnez	a1,2006104e <inic_msg_enqueue+0x80>
2006100c:	00176713          	ori	a4,a4,1
20061010:	00e40623          	sb	a4,12(s0)
20061014:	ea0fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20061018:	ec0d                	c.bnez	s0,20061052 <inic_msg_enqueue+0x84>
2006101a:	200656b7          	lui	a3,0x20065
2006101e:	200655b7          	lui	a1,0x20065
20061022:	97868693          	addi	a3,a3,-1672 # 20064978 <__func__.1+0x3c>
20061026:	04500613          	addi	a2,zero,69
2006102a:	8b858593          	addi	a1,a1,-1864 # 200648b8 <register_string+0x14c>
2006102e:	4509                	c.li	a0,2
20061030:	946fa0ef          	jal	ra,2005b176 <rtk_log_write>
20061034:	547d                	c.li	s0,-1
20061036:	00c92503          	lw	a0,12(s2)
2006103a:	91dfa0ef          	jal	ra,2005b956 <rtos_sema_give>
2006103e:	40f2                	c.lwsp	ra,28(sp)
20061040:	8522                	c.mv	a0,s0
20061042:	4462                	c.lwsp	s0,24(sp)
20061044:	44d2                	c.lwsp	s1,20(sp)
20061046:	4942                	c.lwsp	s2,16(sp)
20061048:	49b2                	c.lwsp	s3,12(sp)
2006104a:	6105                	c.addi16sp	sp,32
2006104c:	8082                	c.jr	ra
2006104e:	0785                	c.addi	a5,1
20061050:	b755                	c.j	20060ff4 <inic_msg_enqueue+0x26>
20061052:	0009a783          	lw	a5,0(s3)
20061056:	00379713          	slli	a4,a5,0x3
2006105a:	00c44783          	lbu	a5,12(s0)
2006105e:	8b9d                	c.andi	a5,7
20061060:	8fd9                	c.or	a5,a4
20061062:	00f40623          	sb	a5,12(s0)
20061066:	0049a703          	lw	a4,4(s3)
2006106a:	9be5                	c.andi	a5,-7
2006106c:	c418                	c.sw	a4,8(s0)
2006106e:	00c9a703          	lw	a4,12(s3)
20061072:	8b0d                	c.andi	a4,3
20061074:	0706                	c.slli	a4,0x1
20061076:	8fd9                	c.or	a5,a4
20061078:	00f40623          	sb	a5,12(s0)
2006107c:	e14fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
20061080:	8522                	c.mv	a0,s0
20061082:	80048593          	addi	a1,s1,-2048
20061086:	201d                	c.jal	200610ac <rtw_list_insert_tail>
20061088:	4405                	c.li	s0,1
2006108a:	e2afa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
2006108e:	b765                	c.j	20061036 <inic_msg_enqueue+0x68>

20061090 <inic_msg_get_queue_status>:
20061090:	200697b7          	lui	a5,0x20069
20061094:	8147c503          	lbu	a0,-2028(a5) # 20068814 <g_ipc_msg_q_priv+0x14>
20061098:	8082                	c.jr	ra

2006109a <rtw_init_listhead>:
2006109a:	c108                	c.sw	a0,0(a0)
2006109c:	c148                	c.sw	a0,4(a0)
2006109e:	8082                	c.jr	ra

200610a0 <rtw_list_insert_head>:
200610a0:	419c                	c.lw	a5,0(a1)
200610a2:	c3c8                	c.sw	a0,4(a5)
200610a4:	c11c                	c.sw	a5,0(a0)
200610a6:	c14c                	c.sw	a1,4(a0)
200610a8:	c188                	c.sw	a0,0(a1)
200610aa:	8082                	c.jr	ra

200610ac <rtw_list_insert_tail>:
200610ac:	41dc                	c.lw	a5,4(a1)
200610ae:	c1c8                	c.sw	a0,4(a1)
200610b0:	c10c                	c.sw	a1,0(a0)
200610b2:	c15c                	c.sw	a5,4(a0)
200610b4:	c388                	c.sw	a0,0(a5)
200610b6:	8082                	c.jr	ra

200610b8 <rtw_list_delete>:
200610b8:	415c                	c.lw	a5,4(a0)
200610ba:	4118                	c.lw	a4,0(a0)
200610bc:	c35c                	c.sw	a5,4(a4)
200610be:	c398                	c.sw	a4,0(a5)
200610c0:	c108                	c.sw	a0,0(a0)
200610c2:	c148                	c.sw	a0,4(a0)
200610c4:	8082                	c.jr	ra

200610c6 <rtw_init_queue>:
200610c6:	c108                	c.sw	a0,0(a0)
200610c8:	fe81a783          	lw	a5,-24(gp) # 20066970 <p_wifi_rom2flash>
200610cc:	c148                	c.sw	a0,4(a0)
200610ce:	0521                	c.addi	a0,8
200610d0:	57dc                	c.lw	a5,44(a5)
200610d2:	8782                	c.jr	a5

200610d4 <rtw_queue_empty>:
200610d4:	411c                	c.lw	a5,0(a0)
200610d6:	40a78533          	sub	a0,a5,a0
200610da:	00153513          	sltiu	a0,a0,1
200610de:	8082                	c.jr	ra

200610e0 <rtw_end_of_queue_search>:
200610e0:	8d0d                	c.sub	a0,a1
200610e2:	00153513          	sltiu	a0,a0,1
200610e6:	8082                	c.jr	ra

200610e8 <timer_wrapper>:
200610e8:	1101                	c.addi	sp,-32
200610ea:	ca26                	c.swsp	s1,20(sp)
200610ec:	cc22                	c.swsp	s0,24(sp)
200610ee:	c84a                	c.swsp	s2,16(sp)
200610f0:	c64e                	c.swsp	s3,12(sp)
200610f2:	c452                	c.swsp	s4,8(sp)
200610f4:	ce06                	c.swsp	ra,28(sp)
200610f6:	89aa                	c.mv	s3,a0
200610f8:	d98fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
200610fc:	ff41a403          	lw	s0,-12(gp) # 2006697c <timer_table>
20061100:	4901                	c.li	s2,0
20061102:	ff418a13          	addi	s4,gp,-12 # 2006697c <timer_table>
20061106:	85a2                	c.mv	a1,s0
20061108:	ff418513          	addi	a0,gp,-12 # 2006697c <timer_table>
2006110c:	fd5ff0ef          	jal	ra,200610e0 <rtw_end_of_queue_search>
20061110:	c90d                	c.beqz	a0,20061142 <timer_wrapper+0x5a>
20061112:	da2fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20061116:	03441c63          	bne	s0,s4,2006114e <timer_wrapper+0x66>
2006111a:	4462                	c.lwsp	s0,24(sp)
2006111c:	40f2                	c.lwsp	ra,28(sp)
2006111e:	44d2                	c.lwsp	s1,20(sp)
20061120:	4942                	c.lwsp	s2,16(sp)
20061122:	49b2                	c.lwsp	s3,12(sp)
20061124:	4a22                	c.lwsp	s4,8(sp)
20061126:	200656b7          	lui	a3,0x20065
2006112a:	200655b7          	lui	a1,0x20065
2006112e:	99068693          	addi	a3,a3,-1648 # 20064990 <__func__.1+0x54>
20061132:	04500613          	addi	a2,zero,69
20061136:	9c058593          	addi	a1,a1,-1600 # 200649c0 <__func__.1+0x84>
2006113a:	4509                	c.li	a0,2
2006113c:	6105                	c.addi16sp	sp,32
2006113e:	838fa06f          	jal	zero,2005b176 <rtk_log_write>
20061142:	441c                	c.lw	a5,8(s0)
20061144:	8922                	c.mv	s2,s0
20061146:	fd3786e3          	beq	a5,s3,20061112 <timer_wrapper+0x2a>
2006114a:	4000                	c.lw	s0,0(s0)
2006114c:	bf6d                	c.j	20061106 <timer_wrapper+0x1e>
2006114e:	01092783          	lw	a5,16(s2)
20061152:	cb99                	c.beqz	a5,20061168 <timer_wrapper+0x80>
20061154:	4462                	c.lwsp	s0,24(sp)
20061156:	00c92503          	lw	a0,12(s2)
2006115a:	40f2                	c.lwsp	ra,28(sp)
2006115c:	44d2                	c.lwsp	s1,20(sp)
2006115e:	4942                	c.lwsp	s2,16(sp)
20061160:	49b2                	c.lwsp	s3,12(sp)
20061162:	4a22                	c.lwsp	s4,8(sp)
20061164:	6105                	c.addi16sp	sp,32
20061166:	8782                	c.jr	a5
20061168:	40f2                	c.lwsp	ra,28(sp)
2006116a:	4462                	c.lwsp	s0,24(sp)
2006116c:	44d2                	c.lwsp	s1,20(sp)
2006116e:	4942                	c.lwsp	s2,16(sp)
20061170:	49b2                	c.lwsp	s3,12(sp)
20061172:	4a22                	c.lwsp	s4,8(sp)
20061174:	6105                	c.addi16sp	sp,32
20061176:	8082                	c.jr	ra

20061178 <init_timer_wrapper>:
20061178:	1141                	c.addi	sp,-16
2006117a:	ff418513          	addi	a0,gp,-12 # 2006697c <timer_table>
2006117e:	c606                	c.swsp	ra,12(sp)
20061180:	f1bff0ef          	jal	ra,2006109a <rtw_init_listhead>
20061184:	40b2                	c.lwsp	ra,12(sp)
20061186:	fe01ae23          	sw	zero,-4(gp) # 20066984 <timer_used_num>
2006118a:	fe01a823          	sw	zero,-16(gp) # 20066978 <max_timer_used_num>
2006118e:	0141                	c.addi	sp,16
20061190:	8082                	c.jr	ra

20061192 <init_timer>:
20061192:	491c                	c.lw	a5,16(a0)
20061194:	cbc1                	c.beqz	a5,20061224 <init_timer+0x92>
20061196:	1141                	c.addi	sp,-16
20061198:	c422                	c.swsp	s0,8(sp)
2006119a:	c606                	c.swsp	ra,12(sp)
2006119c:	842a                	c.mv	s0,a0
2006119e:	4508                	c.lw	a0,8(a0)
200611a0:	e535                	c.bnez	a0,2006120c <init_timer+0x7a>
200611a2:	200617b7          	lui	a5,0x20061
200611a6:	0e878793          	addi	a5,a5,232 # 200610e8 <timer_wrapper>
200611aa:	4701                	c.li	a4,0
200611ac:	56fd                	c.li	a3,-1
200611ae:	4601                	c.li	a2,0
200611b0:	00840513          	addi	a0,s0,8
200611b4:	abbfa0ef          	jal	ra,2005bc6e <rtos_timer_create_static>
200611b8:	441c                	c.lw	a5,8(s0)
200611ba:	e38d                	c.bnez	a5,200611dc <init_timer+0x4a>
200611bc:	4422                	c.lwsp	s0,8(sp)
200611be:	40b2                	c.lwsp	ra,12(sp)
200611c0:	200656b7          	lui	a3,0x20065
200611c4:	200655b7          	lui	a1,0x20065
200611c8:	9c868693          	addi	a3,a3,-1592 # 200649c8 <__func__.1+0x8c>
200611cc:	04500613          	addi	a2,zero,69
200611d0:	9c058593          	addi	a1,a1,-1600 # 200649c0 <__func__.1+0x84>
200611d4:	4509                	c.li	a0,2
200611d6:	0141                	c.addi	sp,16
200611d8:	f9ff906f          	jal	zero,2005b176 <rtk_log_write>
200611dc:	cb4fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
200611e0:	ff418593          	addi	a1,gp,-12 # 2006697c <timer_table>
200611e4:	8522                	c.mv	a0,s0
200611e6:	ebbff0ef          	jal	ra,200610a0 <rtw_list_insert_head>
200611ea:	ccafa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
200611ee:	ffc18713          	addi	a4,gp,-4 # 20066984 <timer_used_num>
200611f2:	431c                	c.lw	a5,0(a4)
200611f4:	0785                	c.addi	a5,1
200611f6:	c31c                	c.sw	a5,0(a4)
200611f8:	ff018713          	addi	a4,gp,-16 # 20066978 <max_timer_used_num>
200611fc:	4314                	c.lw	a3,0(a4)
200611fe:	00f6d363          	bge	a3,a5,20061204 <init_timer+0x72>
20061202:	c31c                	c.sw	a5,0(a4)
20061204:	40b2                	c.lwsp	ra,12(sp)
20061206:	4422                	c.lwsp	s0,8(sp)
20061208:	0141                	c.addi	sp,16
2006120a:	8082                	c.jr	ra
2006120c:	c33fa0ef          	jal	ra,2005be3e <rtos_timer_is_timer_active>
20061210:	4785                	c.li	a5,1
20061212:	fef519e3          	bne	a0,a5,20061204 <init_timer+0x72>
20061216:	4408                	c.lw	a0,8(s0)
20061218:	4422                	c.lwsp	s0,8(sp)
2006121a:	40b2                	c.lwsp	ra,12(sp)
2006121c:	55fd                	c.li	a1,-1
2006121e:	0141                	c.addi	sp,16
20061220:	b61fa06f          	jal	zero,2005bd80 <rtos_timer_stop>
20061224:	8082                	c.jr	ra

20061226 <mod_timer>:
20061226:	1141                	c.addi	sp,-16
20061228:	c422                	c.swsp	s0,8(sp)
2006122a:	c226                	c.swsp	s1,4(sp)
2006122c:	c606                	c.swsp	ra,12(sp)
2006122e:	842a                	c.mv	s0,a0
20061230:	4508                	c.lw	a0,8(a0)
20061232:	84ae                	c.mv	s1,a1
20061234:	e915                	c.bnez	a0,20061268 <mod_timer+0x42>
20061236:	20065537          	lui	a0,0x20065
2006123a:	9e050513          	addi	a0,a0,-1568 # 200649e0 <__func__.1+0xa4>
2006123e:	950fe0ef          	jal	ra,2005f38e <DiagPrintf_minimal>
20061242:	4408                	c.lw	a0,8(s0)
20061244:	cd05                	c.beqz	a0,2006127c <mod_timer+0x56>
20061246:	567d                	c.li	a2,-1
20061248:	85a6                	c.mv	a1,s1
2006124a:	b93fa0ef          	jal	ra,2005bddc <rtos_timer_change_period>
2006124e:	57fd                	c.li	a5,-1
20061250:	02f51663          	bne	a0,a5,2006127c <mod_timer+0x56>
20061254:	4422                	c.lwsp	s0,8(sp)
20061256:	40b2                	c.lwsp	ra,12(sp)
20061258:	4492                	c.lwsp	s1,4(sp)
2006125a:	20065537          	lui	a0,0x20065
2006125e:	9f850513          	addi	a0,a0,-1544 # 200649f8 <__func__.1+0xbc>
20061262:	0141                	c.addi	sp,16
20061264:	92afe06f          	jal	zero,2005f38e <DiagPrintf_minimal>
20061268:	bd7fa0ef          	jal	ra,2005be3e <rtos_timer_is_timer_active>
2006126c:	4785                	c.li	a5,1
2006126e:	fcf51ae3          	bne	a0,a5,20061242 <mod_timer+0x1c>
20061272:	4408                	c.lw	a0,8(s0)
20061274:	55fd                	c.li	a1,-1
20061276:	b0bfa0ef          	jal	ra,2005bd80 <rtos_timer_stop>
2006127a:	b7e1                	c.j	20061242 <mod_timer+0x1c>
2006127c:	40b2                	c.lwsp	ra,12(sp)
2006127e:	4422                	c.lwsp	s0,8(sp)
20061280:	4492                	c.lwsp	s1,4(sp)
20061282:	0141                	c.addi	sp,16
20061284:	8082                	c.jr	ra

20061286 <cancel_timer_ex>:
20061286:	451c                	c.lw	a5,8(a0)
20061288:	c7ad                	c.beqz	a5,200612f2 <cancel_timer_ex+0x6c>
2006128a:	1101                	c.addi	sp,-32
2006128c:	c84a                	c.swsp	s2,16(sp)
2006128e:	cc22                	c.swsp	s0,24(sp)
20061290:	ca26                	c.swsp	s1,20(sp)
20061292:	c64e                	c.swsp	s3,12(sp)
20061294:	ce06                	c.swsp	ra,28(sp)
20061296:	84aa                	c.mv	s1,a0
20061298:	bf8fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
2006129c:	ff41a403          	lw	s0,-12(gp) # 2006697c <timer_table>
200612a0:	ff418993          	addi	s3,gp,-12 # 2006697c <timer_table>
200612a4:	85a2                	c.mv	a1,s0
200612a6:	ff418513          	addi	a0,gp,-12 # 2006697c <timer_table>
200612aa:	e37ff0ef          	jal	ra,200610e0 <rtw_end_of_queue_search>
200612ae:	e509                	c.bnez	a0,200612b8 <cancel_timer_ex+0x32>
200612b0:	4418                	c.lw	a4,8(s0)
200612b2:	449c                	c.lw	a5,8(s1)
200612b4:	02f71463          	bne	a4,a5,200612dc <cancel_timer_ex+0x56>
200612b8:	bfcfa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
200612bc:	4488                	c.lw	a0,8(s1)
200612be:	03341163          	bne	s0,s3,200612e0 <cancel_timer_ex+0x5a>
200612c2:	4462                	c.lwsp	s0,24(sp)
200612c4:	40f2                	c.lwsp	ra,28(sp)
200612c6:	44d2                	c.lwsp	s1,20(sp)
200612c8:	4942                	c.lwsp	s2,16(sp)
200612ca:	49b2                	c.lwsp	s3,12(sp)
200612cc:	85aa                	c.mv	a1,a0
200612ce:	20065537          	lui	a0,0x20065
200612d2:	a0850513          	addi	a0,a0,-1528 # 20064a08 <__func__.1+0xcc>
200612d6:	6105                	c.addi16sp	sp,32
200612d8:	8b6fe06f          	jal	zero,2005f38e <DiagPrintf_minimal>
200612dc:	4000                	c.lw	s0,0(s0)
200612de:	b7d9                	c.j	200612a4 <cancel_timer_ex+0x1e>
200612e0:	4462                	c.lwsp	s0,24(sp)
200612e2:	40f2                	c.lwsp	ra,28(sp)
200612e4:	44d2                	c.lwsp	s1,20(sp)
200612e6:	4942                	c.lwsp	s2,16(sp)
200612e8:	49b2                	c.lwsp	s3,12(sp)
200612ea:	55fd                	c.li	a1,-1
200612ec:	6105                	c.addi16sp	sp,32
200612ee:	a93fa06f          	jal	zero,2005bd80 <rtos_timer_stop>
200612f2:	8082                	c.jr	ra

200612f4 <del_timer_sync>:
200612f4:	451c                	c.lw	a5,8(a0)
200612f6:	cfbd                	c.beqz	a5,20061374 <del_timer_sync+0x80>
200612f8:	1101                	c.addi	sp,-32
200612fa:	c84a                	c.swsp	s2,16(sp)
200612fc:	cc22                	c.swsp	s0,24(sp)
200612fe:	ca26                	c.swsp	s1,20(sp)
20061300:	c64e                	c.swsp	s3,12(sp)
20061302:	ce06                	c.swsp	ra,28(sp)
20061304:	842a                	c.mv	s0,a0
20061306:	b8afa0ef          	jal	ra,2005b690 <rtos_critical_enter>
2006130a:	ff41a483          	lw	s1,-12(gp) # 2006697c <timer_table>
2006130e:	ff418993          	addi	s3,gp,-12 # 2006697c <timer_table>
20061312:	85a6                	c.mv	a1,s1
20061314:	ff418513          	addi	a0,gp,-12 # 2006697c <timer_table>
20061318:	dc9ff0ef          	jal	ra,200610e0 <rtw_end_of_queue_search>
2006131c:	e901                	c.bnez	a0,2006132c <del_timer_sync+0x38>
2006131e:	4498                	c.lw	a4,8(s1)
20061320:	441c                	c.lw	a5,8(s0)
20061322:	02f71563          	bne	a4,a5,2006134c <del_timer_sync+0x58>
20061326:	8526                	c.mv	a0,s1
20061328:	d91ff0ef          	jal	ra,200610b8 <rtw_list_delete>
2006132c:	b88fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20061330:	03349063          	bne	s1,s3,20061350 <del_timer_sync+0x5c>
20061334:	4462                	c.lwsp	s0,24(sp)
20061336:	40f2                	c.lwsp	ra,28(sp)
20061338:	44d2                	c.lwsp	s1,20(sp)
2006133a:	4942                	c.lwsp	s2,16(sp)
2006133c:	49b2                	c.lwsp	s3,12(sp)
2006133e:	20065537          	lui	a0,0x20065
20061342:	a2050513          	addi	a0,a0,-1504 # 20064a20 <__func__.1+0xe4>
20061346:	6105                	c.addi16sp	sp,32
20061348:	846fe06f          	jal	zero,2005f38e <DiagPrintf_minimal>
2006134c:	4084                	c.lw	s1,0(s1)
2006134e:	b7d1                	c.j	20061312 <del_timer_sync+0x1e>
20061350:	4408                	c.lw	a0,8(s0)
20061352:	55fd                	c.li	a1,-1
20061354:	99dfa0ef          	jal	ra,2005bcf0 <rtos_timer_delete_static>
20061358:	ffc18793          	addi	a5,gp,-4 # 20066984 <timer_used_num>
2006135c:	4398                	c.lw	a4,0(a5)
2006135e:	00042423          	sw	zero,8(s0)
20061362:	177d                	c.addi	a4,-1
20061364:	c398                	c.sw	a4,0(a5)
20061366:	40f2                	c.lwsp	ra,28(sp)
20061368:	4462                	c.lwsp	s0,24(sp)
2006136a:	44d2                	c.lwsp	s1,20(sp)
2006136c:	4942                	c.lwsp	s2,16(sp)
2006136e:	49b2                	c.lwsp	s3,12(sp)
20061370:	6105                	c.addi16sp	sp,32
20061372:	8082                	c.jr	ra
20061374:	8082                	c.jr	ra

20061376 <rtw_init_timer>:
20061376:	c910                	c.sw	a2,16(a0)
20061378:	c554                	c.sw	a3,12(a0)
2006137a:	85ba                	c.mv	a1,a4
2006137c:	e17ff06f          	jal	zero,20061192 <init_timer>

20061380 <rtw_set_timer>:
20061380:	ea7ff06f          	jal	zero,20061226 <mod_timer>

20061384 <rtw_cancel_timer>:
20061384:	1141                	c.addi	sp,-16
20061386:	c606                	c.swsp	ra,12(sp)
20061388:	effff0ef          	jal	ra,20061286 <cancel_timer_ex>
2006138c:	40b2                	c.lwsp	ra,12(sp)
2006138e:	4501                	c.li	a0,0
20061390:	0141                	c.addi	sp,16
20061392:	8082                	c.jr	ra

20061394 <rtw_del_timer>:
20061394:	f61ff06f          	jal	zero,200612f4 <del_timer_sync>

20061398 <rtw_acquire_wakelock>:
20061398:	1141                	c.addi	sp,-16
2006139a:	c606                	c.swsp	ra,12(sp)
2006139c:	c5af80ef          	jal	ra,200597f6 <pmu_yield_os_check>
200613a0:	c511                	c.beqz	a0,200613ac <rtw_acquire_wakelock+0x14>
200613a2:	40b2                	c.lwsp	ra,12(sp)
200613a4:	4505                	c.li	a0,1
200613a6:	0141                	c.addi	sp,16
200613a8:	c9af806f          	jal	zero,20059842 <pmu_acquire_wakelock>
200613ac:	40b2                	c.lwsp	ra,12(sp)
200613ae:	0141                	c.addi	sp,16
200613b0:	8082                	c.jr	ra

200613b2 <rtw_release_wakelock>:
200613b2:	1141                	c.addi	sp,-16
200613b4:	c606                	c.swsp	ra,12(sp)
200613b6:	c40f80ef          	jal	ra,200597f6 <pmu_yield_os_check>
200613ba:	c511                	c.beqz	a0,200613c6 <rtw_release_wakelock+0x14>
200613bc:	40b2                	c.lwsp	ra,12(sp)
200613be:	4505                	c.li	a0,1
200613c0:	0141                	c.addi	sp,16
200613c2:	ca6f806f          	jal	zero,20059868 <pmu_release_wakelock>
200613c6:	40b2                	c.lwsp	ra,12(sp)
200613c8:	0141                	c.addi	sp,16
200613ca:	8082                	c.jr	ra

200613cc <rtw_wakelock_timeout>:
200613cc:	1141                	c.addi	sp,-16
200613ce:	c422                	c.swsp	s0,8(sp)
200613d0:	c606                	c.swsp	ra,12(sp)
200613d2:	842a                	c.mv	s0,a0
200613d4:	c22f80ef          	jal	ra,200597f6 <pmu_yield_os_check>
200613d8:	c519                	c.beqz	a0,200613e6 <rtw_wakelock_timeout+0x1a>
200613da:	8522                	c.mv	a0,s0
200613dc:	4422                	c.lwsp	s0,8(sp)
200613de:	40b2                	c.lwsp	ra,12(sp)
200613e0:	0141                	c.addi	sp,16
200613e2:	b38f806f          	jal	zero,2005971a <pmu_set_sysactive_time>
200613e6:	40b2                	c.lwsp	ra,12(sp)
200613e8:	4422                	c.lwsp	s0,8(sp)
200613ea:	0141                	c.addi	sp,16
200613ec:	8082                	c.jr	ra

200613ee <deregister_tm_clones>:
200613ee:	20058537          	lui	a0,0x20058
200613f2:	200587b7          	lui	a5,0x20058
200613f6:	06050713          	addi	a4,a0,96 # 20058060 <BOOT_InitGP>
200613fa:	06078793          	addi	a5,a5,96 # 20058060 <BOOT_InitGP>
200613fe:	00e78863          	beq	a5,a4,2006140e <deregister_tm_clones+0x20>
20061402:	00000793          	addi	a5,zero,0
20061406:	c781                	c.beqz	a5,2006140e <deregister_tm_clones+0x20>
20061408:	06050513          	addi	a0,a0,96
2006140c:	8782                	c.jr	a5
2006140e:	8082                	c.jr	ra

20061410 <register_tm_clones>:
20061410:	200587b7          	lui	a5,0x20058
20061414:	20058537          	lui	a0,0x20058
20061418:	06078593          	addi	a1,a5,96 # 20058060 <BOOT_InitGP>
2006141c:	06050793          	addi	a5,a0,96 # 20058060 <BOOT_InitGP>
20061420:	8d9d                	c.sub	a1,a5
20061422:	8589                	c.srai	a1,0x2
20061424:	4789                	c.li	a5,2
20061426:	02f5c5b3          	div	a1,a1,a5
2006142a:	c599                	c.beqz	a1,20061438 <register_tm_clones+0x28>
2006142c:	00000793          	addi	a5,zero,0
20061430:	c781                	c.beqz	a5,20061438 <register_tm_clones+0x28>
20061432:	06050513          	addi	a0,a0,96
20061436:	8782                	c.jr	a5
20061438:	8082                	c.jr	ra

2006143a <__do_global_dtors_aux>:
2006143a:	200697b7          	lui	a5,0x20069
2006143e:	9a47c703          	lbu	a4,-1628(a5) # 200689a4 <completed.1>
20061442:	eb05                	c.bnez	a4,20061472 <__do_global_dtors_aux+0x38>
20061444:	1141                	c.addi	sp,-16
20061446:	c422                	c.swsp	s0,8(sp)
20061448:	c606                	c.swsp	ra,12(sp)
2006144a:	843e                	c.mv	s0,a5
2006144c:	374d                	c.jal	200613ee <deregister_tm_clones>
2006144e:	00000793          	addi	a5,zero,0
20061452:	cb89                	c.beqz	a5,20061464 <__do_global_dtors_aux+0x2a>
20061454:	20058537          	lui	a0,0x20058
20061458:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
2006145c:	00000097          	auipc	ra,0x0
20061460:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20061464:	4785                	c.li	a5,1
20061466:	40b2                	c.lwsp	ra,12(sp)
20061468:	9af40223          	sb	a5,-1628(s0)
2006146c:	4422                	c.lwsp	s0,8(sp)
2006146e:	0141                	c.addi	sp,16
20061470:	8082                	c.jr	ra
20061472:	8082                	c.jr	ra

20061474 <frame_dummy>:
20061474:	00000793          	addi	a5,zero,0
20061478:	c38d                	c.beqz	a5,2006149a <frame_dummy+0x26>
2006147a:	200695b7          	lui	a1,0x20069
2006147e:	20058537          	lui	a0,0x20058
20061482:	1141                	c.addi	sp,-16
20061484:	9a858593          	addi	a1,a1,-1624 # 200689a8 <object.0>
20061488:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
2006148c:	c606                	c.swsp	ra,12(sp)
2006148e:	00000097          	auipc	ra,0x0
20061492:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20061496:	40b2                	c.lwsp	ra,12(sp)
20061498:	0141                	c.addi	sp,16
2006149a:	bf9d                	c.j	20061410 <register_tm_clones>

2006149c <__clzsi2>:
2006149c:	67c1                	c.lui	a5,0x10
2006149e:	02f57663          	bgeu	a0,a5,200614ca <__clzsi2+0x2e>
200614a2:	0ff00793          	addi	a5,zero,255
200614a6:	00a7b7b3          	sltu	a5,a5,a0
200614aa:	078e                	c.slli	a5,0x3
200614ac:	20065737          	lui	a4,0x20065
200614b0:	02000693          	addi	a3,zero,32
200614b4:	8e9d                	c.sub	a3,a5
200614b6:	00f55533          	srl	a0,a0,a5
200614ba:	a3070793          	addi	a5,a4,-1488 # 20064a30 <__clz_tab>
200614be:	953e                	c.add	a0,a5
200614c0:	00054503          	lbu	a0,0(a0)
200614c4:	40a68533          	sub	a0,a3,a0
200614c8:	8082                	c.jr	ra
200614ca:	01000737          	lui	a4,0x1000
200614ce:	47c1                	c.li	a5,16
200614d0:	fce56ee3          	bltu	a0,a4,200614ac <__clzsi2+0x10>
200614d4:	47e1                	c.li	a5,24
200614d6:	bfd9                	c.j	200614ac <__clzsi2+0x10>

200614d8 <__udivdi3>:
200614d8:	88aa                	c.mv	a7,a0
200614da:	87ae                	c.mv	a5,a1
200614dc:	8832                	c.mv	a6,a2
200614de:	8536                	c.mv	a0,a3
200614e0:	8346                	c.mv	t1,a7
200614e2:	20069663          	bne	a3,zero,200616ee <__udivdi3+0x216>
200614e6:	200656b7          	lui	a3,0x20065
200614ea:	a3068693          	addi	a3,a3,-1488 # 20064a30 <__clz_tab>
200614ee:	0ac5ff63          	bgeu	a1,a2,200615ac <__udivdi3+0xd4>
200614f2:	6741                	c.lui	a4,0x10
200614f4:	0ae67563          	bgeu	a2,a4,2006159e <__udivdi3+0xc6>
200614f8:	0ff00713          	addi	a4,zero,255
200614fc:	00c73733          	sltu	a4,a4,a2
20061500:	070e                	c.slli	a4,0x3
20061502:	00e65533          	srl	a0,a2,a4
20061506:	96aa                	c.add	a3,a0
20061508:	0006c683          	lbu	a3,0(a3)
2006150c:	02000513          	addi	a0,zero,32
20061510:	9736                	c.add	a4,a3
20061512:	40e506b3          	sub	a3,a0,a4
20061516:	00e50b63          	beq	a0,a4,2006152c <__udivdi3+0x54>
2006151a:	00d795b3          	sll	a1,a5,a3
2006151e:	00e8d733          	srl	a4,a7,a4
20061522:	00d61833          	sll	a6,a2,a3
20061526:	8dd9                	c.or	a1,a4
20061528:	00d89333          	sll	t1,a7,a3
2006152c:	01085893          	srli	a7,a6,0x10
20061530:	0315d6b3          	divu	a3,a1,a7
20061534:	01081613          	slli	a2,a6,0x10
20061538:	8241                	c.srli	a2,0x10
2006153a:	01035793          	srli	a5,t1,0x10
2006153e:	0315f733          	remu	a4,a1,a7
20061542:	8536                	c.mv	a0,a3
20061544:	02d605b3          	mul	a1,a2,a3
20061548:	0742                	c.slli	a4,0x10
2006154a:	8fd9                	c.or	a5,a4
2006154c:	00b7fc63          	bgeu	a5,a1,20061564 <__udivdi3+0x8c>
20061550:	97c2                	c.add	a5,a6
20061552:	fff68513          	addi	a0,a3,-1
20061556:	0107e763          	bltu	a5,a6,20061564 <__udivdi3+0x8c>
2006155a:	00b7f563          	bgeu	a5,a1,20061564 <__udivdi3+0x8c>
2006155e:	ffe68513          	addi	a0,a3,-2
20061562:	97c2                	c.add	a5,a6
20061564:	8f8d                	c.sub	a5,a1
20061566:	0317f733          	remu	a4,a5,a7
2006156a:	0342                	c.slli	t1,0x10
2006156c:	01035313          	srli	t1,t1,0x10
20061570:	0317d7b3          	divu	a5,a5,a7
20061574:	0742                	c.slli	a4,0x10
20061576:	00676333          	or	t1,a4,t1
2006157a:	02f606b3          	mul	a3,a2,a5
2006157e:	863e                	c.mv	a2,a5
20061580:	00d37b63          	bgeu	t1,a3,20061596 <__udivdi3+0xbe>
20061584:	9342                	c.add	t1,a6
20061586:	fff78613          	addi	a2,a5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
2006158a:	01036663          	bltu	t1,a6,20061596 <__udivdi3+0xbe>
2006158e:	00d37463          	bgeu	t1,a3,20061596 <__udivdi3+0xbe>
20061592:	ffe78613          	addi	a2,a5,-2
20061596:	0542                	c.slli	a0,0x10
20061598:	8d51                	c.or	a0,a2
2006159a:	4581                	c.li	a1,0
2006159c:	a85d                	c.j	20061652 <__udivdi3+0x17a>
2006159e:	01000537          	lui	a0,0x1000
200615a2:	4741                	c.li	a4,16
200615a4:	f4a66fe3          	bltu	a2,a0,20061502 <__udivdi3+0x2a>
200615a8:	4761                	c.li	a4,24
200615aa:	bfa1                	c.j	20061502 <__udivdi3+0x2a>
200615ac:	e601                	c.bnez	a2,200615b4 <__udivdi3+0xdc>
200615ae:	4705                	c.li	a4,1
200615b0:	02c75833          	divu	a6,a4,a2
200615b4:	6741                	c.lui	a4,0x10
200615b6:	08e87f63          	bgeu	a6,a4,20061654 <__udivdi3+0x17c>
200615ba:	0ff00713          	addi	a4,zero,255
200615be:	01077363          	bgeu	a4,a6,200615c4 <__udivdi3+0xec>
200615c2:	4521                	c.li	a0,8
200615c4:	00a85733          	srl	a4,a6,a0
200615c8:	96ba                	c.add	a3,a4
200615ca:	0006c703          	lbu	a4,0(a3)
200615ce:	02000613          	addi	a2,zero,32
200615d2:	972a                	c.add	a4,a0
200615d4:	40e606b3          	sub	a3,a2,a4
200615d8:	08e61563          	bne	a2,a4,20061662 <__udivdi3+0x18a>
200615dc:	410787b3          	sub	a5,a5,a6
200615e0:	4585                	c.li	a1,1
200615e2:	01085893          	srli	a7,a6,0x10
200615e6:	01081613          	slli	a2,a6,0x10
200615ea:	8241                	c.srli	a2,0x10
200615ec:	01035713          	srli	a4,t1,0x10
200615f0:	0317f6b3          	remu	a3,a5,a7
200615f4:	0317d7b3          	divu	a5,a5,a7
200615f8:	06c2                	c.slli	a3,0x10
200615fa:	8f55                	c.or	a4,a3
200615fc:	02f60e33          	mul	t3,a2,a5
20061600:	853e                	c.mv	a0,a5
20061602:	01c77c63          	bgeu	a4,t3,2006161a <__udivdi3+0x142>
20061606:	9742                	c.add	a4,a6
20061608:	fff78513          	addi	a0,a5,-1
2006160c:	01076763          	bltu	a4,a6,2006161a <__udivdi3+0x142>
20061610:	01c77563          	bgeu	a4,t3,2006161a <__udivdi3+0x142>
20061614:	ffe78513          	addi	a0,a5,-2
20061618:	9742                	c.add	a4,a6
2006161a:	41c70733          	sub	a4,a4,t3
2006161e:	031777b3          	remu	a5,a4,a7
20061622:	0342                	c.slli	t1,0x10
20061624:	01035313          	srli	t1,t1,0x10
20061628:	03175733          	divu	a4,a4,a7
2006162c:	07c2                	c.slli	a5,0x10
2006162e:	0067e333          	or	t1,a5,t1
20061632:	02e606b3          	mul	a3,a2,a4
20061636:	863a                	c.mv	a2,a4
20061638:	00d37b63          	bgeu	t1,a3,2006164e <__udivdi3+0x176>
2006163c:	9342                	c.add	t1,a6
2006163e:	fff70613          	addi	a2,a4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061642:	01036663          	bltu	t1,a6,2006164e <__udivdi3+0x176>
20061646:	00d37463          	bgeu	t1,a3,2006164e <__udivdi3+0x176>
2006164a:	ffe70613          	addi	a2,a4,-2
2006164e:	0542                	c.slli	a0,0x10
20061650:	8d51                	c.or	a0,a2
20061652:	8082                	c.jr	ra
20061654:	01000737          	lui	a4,0x1000
20061658:	4541                	c.li	a0,16
2006165a:	f6e865e3          	bltu	a6,a4,200615c4 <__udivdi3+0xec>
2006165e:	4561                	c.li	a0,24
20061660:	b795                	c.j	200615c4 <__udivdi3+0xec>
20061662:	00d81833          	sll	a6,a6,a3
20061666:	00e7d533          	srl	a0,a5,a4
2006166a:	00d89333          	sll	t1,a7,a3
2006166e:	00d797b3          	sll	a5,a5,a3
20061672:	00e8d733          	srl	a4,a7,a4
20061676:	01085893          	srli	a7,a6,0x10
2006167a:	00f76633          	or	a2,a4,a5
2006167e:	03157733          	remu	a4,a0,a7
20061682:	01081793          	slli	a5,a6,0x10
20061686:	83c1                	c.srli	a5,0x10
20061688:	01065593          	srli	a1,a2,0x10
2006168c:	03155533          	divu	a0,a0,a7
20061690:	0742                	c.slli	a4,0x10
20061692:	8f4d                	c.or	a4,a1
20061694:	02a786b3          	mul	a3,a5,a0
20061698:	85aa                	c.mv	a1,a0
2006169a:	00d77c63          	bgeu	a4,a3,200616b2 <__udivdi3+0x1da>
2006169e:	9742                	c.add	a4,a6
200616a0:	fff50593          	addi	a1,a0,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
200616a4:	01076763          	bltu	a4,a6,200616b2 <__udivdi3+0x1da>
200616a8:	00d77563          	bgeu	a4,a3,200616b2 <__udivdi3+0x1da>
200616ac:	ffe50593          	addi	a1,a0,-2
200616b0:	9742                	c.add	a4,a6
200616b2:	40d706b3          	sub	a3,a4,a3
200616b6:	0316f733          	remu	a4,a3,a7
200616ba:	0642                	c.slli	a2,0x10
200616bc:	8241                	c.srli	a2,0x10
200616be:	0316d6b3          	divu	a3,a3,a7
200616c2:	0742                	c.slli	a4,0x10
200616c4:	02d78533          	mul	a0,a5,a3
200616c8:	00c767b3          	or	a5,a4,a2
200616cc:	8736                	c.mv	a4,a3
200616ce:	00a7fc63          	bgeu	a5,a0,200616e6 <__udivdi3+0x20e>
200616d2:	97c2                	c.add	a5,a6
200616d4:	fff68713          	addi	a4,a3,-1
200616d8:	0107e763          	bltu	a5,a6,200616e6 <__udivdi3+0x20e>
200616dc:	00a7f563          	bgeu	a5,a0,200616e6 <__udivdi3+0x20e>
200616e0:	ffe68713          	addi	a4,a3,-2
200616e4:	97c2                	c.add	a5,a6
200616e6:	05c2                	c.slli	a1,0x10
200616e8:	8f89                	c.sub	a5,a0
200616ea:	8dd9                	c.or	a1,a4
200616ec:	bddd                	c.j	200615e2 <__udivdi3+0x10a>
200616ee:	14d5e263          	bltu	a1,a3,20061832 <__udivdi3+0x35a>
200616f2:	6741                	c.lui	a4,0x10
200616f4:	02e6ff63          	bgeu	a3,a4,20061732 <__udivdi3+0x25a>
200616f8:	0ff00713          	addi	a4,zero,255
200616fc:	00d735b3          	sltu	a1,a4,a3
20061700:	058e                	c.slli	a1,0x3
20061702:	20065737          	lui	a4,0x20065
20061706:	00b6d533          	srl	a0,a3,a1
2006170a:	a3070713          	addi	a4,a4,-1488 # 20064a30 <__clz_tab>
2006170e:	972a                	c.add	a4,a0
20061710:	00074703          	lbu	a4,0(a4)
20061714:	02000513          	addi	a0,zero,32
20061718:	972e                	c.add	a4,a1
2006171a:	40e505b3          	sub	a1,a0,a4
2006171e:	02e51163          	bne	a0,a4,20061740 <__udivdi3+0x268>
20061722:	4505                	c.li	a0,1
20061724:	f2f6e7e3          	bltu	a3,a5,20061652 <__udivdi3+0x17a>
20061728:	00c8b533          	sltu	a0,a7,a2
2006172c:	00154513          	xori	a0,a0,1
20061730:	b70d                	c.j	20061652 <__udivdi3+0x17a>
20061732:	01000737          	lui	a4,0x1000
20061736:	45c1                	c.li	a1,16
20061738:	fce6e5e3          	bltu	a3,a4,20061702 <__udivdi3+0x22a>
2006173c:	45e1                	c.li	a1,24
2006173e:	b7d1                	c.j	20061702 <__udivdi3+0x22a>
20061740:	00e65833          	srl	a6,a2,a4
20061744:	00b696b3          	sll	a3,a3,a1
20061748:	00d86833          	or	a6,a6,a3
2006174c:	00e7de33          	srl	t3,a5,a4
20061750:	01085e93          	srli	t4,a6,0x10
20061754:	03de76b3          	remu	a3,t3,t4
20061758:	00b797b3          	sll	a5,a5,a1
2006175c:	00e8d733          	srl	a4,a7,a4
20061760:	00b61333          	sll	t1,a2,a1
20061764:	00f76633          	or	a2,a4,a5
20061768:	01081793          	slli	a5,a6,0x10
2006176c:	83c1                	c.srli	a5,0x10
2006176e:	01065713          	srli	a4,a2,0x10
20061772:	03de5e33          	divu	t3,t3,t4
20061776:	06c2                	c.slli	a3,0x10
20061778:	8f55                	c.or	a4,a3
2006177a:	03c78f33          	mul	t5,a5,t3
2006177e:	8572                	c.mv	a0,t3
20061780:	01e77c63          	bgeu	a4,t5,20061798 <__udivdi3+0x2c0>
20061784:	9742                	c.add	a4,a6
20061786:	fffe0513          	addi	a0,t3,-1
2006178a:	01076763          	bltu	a4,a6,20061798 <__udivdi3+0x2c0>
2006178e:	01e77563          	bgeu	a4,t5,20061798 <__udivdi3+0x2c0>
20061792:	ffee0513          	addi	a0,t3,-2
20061796:	9742                	c.add	a4,a6
20061798:	41e70733          	sub	a4,a4,t5
2006179c:	03d776b3          	remu	a3,a4,t4
200617a0:	03d75733          	divu	a4,a4,t4
200617a4:	06c2                	c.slli	a3,0x10
200617a6:	02e78e33          	mul	t3,a5,a4
200617aa:	01061793          	slli	a5,a2,0x10
200617ae:	83c1                	c.srli	a5,0x10
200617b0:	8fd5                	c.or	a5,a3
200617b2:	863a                	c.mv	a2,a4
200617b4:	01c7fc63          	bgeu	a5,t3,200617cc <__udivdi3+0x2f4>
200617b8:	97c2                	c.add	a5,a6
200617ba:	fff70613          	addi	a2,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
200617be:	0107e763          	bltu	a5,a6,200617cc <__udivdi3+0x2f4>
200617c2:	01c7f563          	bgeu	a5,t3,200617cc <__udivdi3+0x2f4>
200617c6:	ffe70613          	addi	a2,a4,-2
200617ca:	97c2                	c.add	a5,a6
200617cc:	0542                	c.slli	a0,0x10
200617ce:	6ec1                	c.lui	t4,0x10
200617d0:	8d51                	c.or	a0,a2
200617d2:	fffe8693          	addi	a3,t4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
200617d6:	00d57833          	and	a6,a0,a3
200617da:	01055613          	srli	a2,a0,0x10
200617de:	00d376b3          	and	a3,t1,a3
200617e2:	01035313          	srli	t1,t1,0x10
200617e6:	41c787b3          	sub	a5,a5,t3
200617ea:	02d80e33          	mul	t3,a6,a3
200617ee:	02d606b3          	mul	a3,a2,a3
200617f2:	010e5713          	srli	a4,t3,0x10
200617f6:	02680833          	mul	a6,a6,t1
200617fa:	9836                	c.add	a6,a3
200617fc:	9742                	c.add	a4,a6
200617fe:	02660633          	mul	a2,a2,t1
20061802:	00d77363          	bgeu	a4,a3,20061808 <__udivdi3+0x330>
20061806:	9676                	c.add	a2,t4
20061808:	01075693          	srli	a3,a4,0x10
2006180c:	9636                	c.add	a2,a3
2006180e:	02c7e063          	bltu	a5,a2,2006182e <__udivdi3+0x356>
20061812:	d8c794e3          	bne	a5,a2,2006159a <__udivdi3+0xc2>
20061816:	67c1                	c.lui	a5,0x10
20061818:	17fd                	c.addi	a5,-1
2006181a:	8f7d                	c.and	a4,a5
2006181c:	0742                	c.slli	a4,0x10
2006181e:	00fe7e33          	and	t3,t3,a5
20061822:	00b898b3          	sll	a7,a7,a1
20061826:	9772                	c.add	a4,t3
20061828:	4581                	c.li	a1,0
2006182a:	e2e8f4e3          	bgeu	a7,a4,20061652 <__udivdi3+0x17a>
2006182e:	157d                	c.addi	a0,-1
20061830:	b3ad                	c.j	2006159a <__udivdi3+0xc2>
20061832:	4581                	c.li	a1,0
20061834:	4501                	c.li	a0,0
20061836:	bd31                	c.j	20061652 <__udivdi3+0x17a>

20061838 <__umoddi3>:
20061838:	8eaa                	c.mv	t4,a0
2006183a:	832e                	c.mv	t1,a1
2006183c:	8832                	c.mv	a6,a2
2006183e:	87b6                	c.mv	a5,a3
20061840:	1c069263          	bne	a3,zero,20061a04 <__umoddi3+0x1cc>
20061844:	20065737          	lui	a4,0x20065
20061848:	a3070713          	addi	a4,a4,-1488 # 20064a30 <__clz_tab>
2006184c:	0ac5f563          	bgeu	a1,a2,200618f6 <__umoddi3+0xbe>
20061850:	66c1                	c.lui	a3,0x10
20061852:	08d67b63          	bgeu	a2,a3,200618e8 <__umoddi3+0xb0>
20061856:	0ff00693          	addi	a3,zero,255
2006185a:	00c6f363          	bgeu	a3,a2,20061860 <__umoddi3+0x28>
2006185e:	47a1                	c.li	a5,8
20061860:	00f656b3          	srl	a3,a2,a5
20061864:	9736                	c.add	a4,a3
20061866:	00074703          	lbu	a4,0(a4)
2006186a:	97ba                	c.add	a5,a4
2006186c:	02000713          	addi	a4,zero,32
20061870:	40f708b3          	sub	a7,a4,a5
20061874:	00f70b63          	beq	a4,a5,2006188a <__umoddi3+0x52>
20061878:	011315b3          	sll	a1,t1,a7
2006187c:	00fed7b3          	srl	a5,t4,a5
20061880:	01161833          	sll	a6,a2,a7
20061884:	8ddd                	c.or	a1,a5
20061886:	011e9533          	sll	a0,t4,a7
2006188a:	01085693          	srli	a3,a6,0x10
2006188e:	02d5f7b3          	remu	a5,a1,a3
20061892:	01081613          	slli	a2,a6,0x10
20061896:	8241                	c.srli	a2,0x10
20061898:	01055713          	srli	a4,a0,0x10
2006189c:	02d5d5b3          	divu	a1,a1,a3
200618a0:	07c2                	c.slli	a5,0x10
200618a2:	8f5d                	c.or	a4,a5
200618a4:	02b605b3          	mul	a1,a2,a1
200618a8:	00b77863          	bgeu	a4,a1,200618b8 <__umoddi3+0x80>
200618ac:	9742                	c.add	a4,a6
200618ae:	01076563          	bltu	a4,a6,200618b8 <__umoddi3+0x80>
200618b2:	00b77363          	bgeu	a4,a1,200618b8 <__umoddi3+0x80>
200618b6:	9742                	c.add	a4,a6
200618b8:	8f0d                	c.sub	a4,a1
200618ba:	02d777b3          	remu	a5,a4,a3
200618be:	0542                	c.slli	a0,0x10
200618c0:	8141                	c.srli	a0,0x10
200618c2:	02d75733          	divu	a4,a4,a3
200618c6:	07c2                	c.slli	a5,0x10
200618c8:	8d5d                	c.or	a0,a5
200618ca:	02e60733          	mul	a4,a2,a4
200618ce:	00e57863          	bgeu	a0,a4,200618de <__umoddi3+0xa6>
200618d2:	9542                	c.add	a0,a6
200618d4:	01056563          	bltu	a0,a6,200618de <__umoddi3+0xa6>
200618d8:	00e57363          	bgeu	a0,a4,200618de <__umoddi3+0xa6>
200618dc:	9542                	c.add	a0,a6
200618de:	8d19                	c.sub	a0,a4
200618e0:	01155533          	srl	a0,a0,a7
200618e4:	4581                	c.li	a1,0
200618e6:	8082                	c.jr	ra
200618e8:	010006b7          	lui	a3,0x1000
200618ec:	47c1                	c.li	a5,16
200618ee:	f6d669e3          	bltu	a2,a3,20061860 <__umoddi3+0x28>
200618f2:	47e1                	c.li	a5,24
200618f4:	b7b5                	c.j	20061860 <__umoddi3+0x28>
200618f6:	e601                	c.bnez	a2,200618fe <__umoddi3+0xc6>
200618f8:	4685                	c.li	a3,1
200618fa:	02c6d833          	divu	a6,a3,a2
200618fe:	66c1                	c.lui	a3,0x10
20061900:	0ed87b63          	bgeu	a6,a3,200619f6 <__umoddi3+0x1be>
20061904:	0ff00693          	addi	a3,zero,255
20061908:	0106f363          	bgeu	a3,a6,2006190e <__umoddi3+0xd6>
2006190c:	47a1                	c.li	a5,8
2006190e:	00f856b3          	srl	a3,a6,a5
20061912:	9736                	c.add	a4,a3
20061914:	00074603          	lbu	a2,0(a4)
20061918:	410305b3          	sub	a1,t1,a6
2006191c:	963e                	c.add	a2,a5
2006191e:	02000793          	addi	a5,zero,32
20061922:	40c788b3          	sub	a7,a5,a2
20061926:	06c78a63          	beq	a5,a2,2006199a <__umoddi3+0x162>
2006192a:	01181833          	sll	a6,a6,a7
2006192e:	00c357b3          	srl	a5,t1,a2
20061932:	011315b3          	sll	a1,t1,a7
20061936:	01085313          	srli	t1,a6,0x10
2006193a:	00ced633          	srl	a2,t4,a2
2006193e:	8e4d                	c.or	a2,a1
20061940:	01081593          	slli	a1,a6,0x10
20061944:	81c1                	c.srli	a1,0x10
20061946:	011e9533          	sll	a0,t4,a7
2006194a:	0267d6b3          	divu	a3,a5,t1
2006194e:	0267f733          	remu	a4,a5,t1
20061952:	01065793          	srli	a5,a2,0x10
20061956:	02d586b3          	mul	a3,a1,a3
2006195a:	0742                	c.slli	a4,0x10
2006195c:	8f5d                	c.or	a4,a5
2006195e:	00d77863          	bgeu	a4,a3,2006196e <__umoddi3+0x136>
20061962:	9742                	c.add	a4,a6
20061964:	01076563          	bltu	a4,a6,2006196e <__umoddi3+0x136>
20061968:	00d77363          	bgeu	a4,a3,2006196e <__umoddi3+0x136>
2006196c:	9742                	c.add	a4,a6
2006196e:	40d706b3          	sub	a3,a4,a3
20061972:	0266f733          	remu	a4,a3,t1
20061976:	0642                	c.slli	a2,0x10
20061978:	8241                	c.srli	a2,0x10
2006197a:	0266d6b3          	divu	a3,a3,t1
2006197e:	0742                	c.slli	a4,0x10
20061980:	02d586b3          	mul	a3,a1,a3
20061984:	00c765b3          	or	a1,a4,a2
20061988:	00d5f863          	bgeu	a1,a3,20061998 <__umoddi3+0x160>
2006198c:	95c2                	c.add	a1,a6
2006198e:	0105e563          	bltu	a1,a6,20061998 <__umoddi3+0x160>
20061992:	00d5f363          	bgeu	a1,a3,20061998 <__umoddi3+0x160>
20061996:	95c2                	c.add	a1,a6
20061998:	8d95                	c.sub	a1,a3
2006199a:	01085693          	srli	a3,a6,0x10
2006199e:	02d5f733          	remu	a4,a1,a3
200619a2:	01081793          	slli	a5,a6,0x10
200619a6:	83c1                	c.srli	a5,0x10
200619a8:	01055613          	srli	a2,a0,0x10
200619ac:	02d5d5b3          	divu	a1,a1,a3
200619b0:	0742                	c.slli	a4,0x10
200619b2:	8f51                	c.or	a4,a2
200619b4:	02b785b3          	mul	a1,a5,a1
200619b8:	00b77863          	bgeu	a4,a1,200619c8 <__umoddi3+0x190>
200619bc:	9742                	c.add	a4,a6
200619be:	01076563          	bltu	a4,a6,200619c8 <__umoddi3+0x190>
200619c2:	00b77363          	bgeu	a4,a1,200619c8 <__umoddi3+0x190>
200619c6:	9742                	c.add	a4,a6
200619c8:	40b705b3          	sub	a1,a4,a1
200619cc:	02d5f733          	remu	a4,a1,a3
200619d0:	0542                	c.slli	a0,0x10
200619d2:	8141                	c.srli	a0,0x10
200619d4:	02d5d5b3          	divu	a1,a1,a3
200619d8:	02b785b3          	mul	a1,a5,a1
200619dc:	01071793          	slli	a5,a4,0x10
200619e0:	8d5d                	c.or	a0,a5
200619e2:	00b57863          	bgeu	a0,a1,200619f2 <__umoddi3+0x1ba>
200619e6:	9542                	c.add	a0,a6
200619e8:	01056563          	bltu	a0,a6,200619f2 <__umoddi3+0x1ba>
200619ec:	00b57363          	bgeu	a0,a1,200619f2 <__umoddi3+0x1ba>
200619f0:	9542                	c.add	a0,a6
200619f2:	8d0d                	c.sub	a0,a1
200619f4:	b5f5                	c.j	200618e0 <__umoddi3+0xa8>
200619f6:	010006b7          	lui	a3,0x1000
200619fa:	47c1                	c.li	a5,16
200619fc:	f0d869e3          	bltu	a6,a3,2006190e <__umoddi3+0xd6>
20061a00:	47e1                	c.li	a5,24
20061a02:	b731                	c.j	2006190e <__umoddi3+0xd6>
20061a04:	eed5e1e3          	bltu	a1,a3,200618e6 <__umoddi3+0xae>
20061a08:	67c1                	c.lui	a5,0x10
20061a0a:	04f6f463          	bgeu	a3,a5,20061a52 <__umoddi3+0x21a>
20061a0e:	0ff00893          	addi	a7,zero,255
20061a12:	00d8b7b3          	sltu	a5,a7,a3
20061a16:	078e                	c.slli	a5,0x3
20061a18:	20065737          	lui	a4,0x20065
20061a1c:	00f6d833          	srl	a6,a3,a5
20061a20:	a3070713          	addi	a4,a4,-1488 # 20064a30 <__clz_tab>
20061a24:	9742                	c.add	a4,a6
20061a26:	00074883          	lbu	a7,0(a4)
20061a2a:	98be                	c.add	a7,a5
20061a2c:	02000793          	addi	a5,zero,32
20061a30:	41178833          	sub	a6,a5,a7
20061a34:	03179663          	bne	a5,a7,20061a60 <__umoddi3+0x228>
20061a38:	0066e463          	bltu	a3,t1,20061a40 <__umoddi3+0x208>
20061a3c:	eacee5e3          	bltu	t4,a2,200618e6 <__umoddi3+0xae>
20061a40:	40ce8533          	sub	a0,t4,a2
20061a44:	40d305b3          	sub	a1,t1,a3
20061a48:	00aebeb3          	sltu	t4,t4,a0
20061a4c:	41d585b3          	sub	a1,a1,t4
20061a50:	bd59                	c.j	200618e6 <__umoddi3+0xae>
20061a52:	01000737          	lui	a4,0x1000
20061a56:	47c1                	c.li	a5,16
20061a58:	fce6e0e3          	bltu	a3,a4,20061a18 <__umoddi3+0x1e0>
20061a5c:	47e1                	c.li	a5,24
20061a5e:	bf6d                	c.j	20061a18 <__umoddi3+0x1e0>
20061a60:	011657b3          	srl	a5,a2,a7
20061a64:	010696b3          	sll	a3,a3,a6
20061a68:	00d7ee33          	or	t3,a5,a3
20061a6c:	01135f33          	srl	t5,t1,a7
20061a70:	011ed7b3          	srl	a5,t4,a7
20061a74:	010e9533          	sll	a0,t4,a6
20061a78:	010e5e93          	srli	t4,t3,0x10
20061a7c:	010315b3          	sll	a1,t1,a6
20061a80:	03df5333          	divu	t1,t5,t4
20061a84:	8fcd                	c.or	a5,a1
20061a86:	010e1593          	slli	a1,t3,0x10
20061a8a:	81c1                	c.srli	a1,0x10
20061a8c:	0107d713          	srli	a4,a5,0x10
20061a90:	01061633          	sll	a2,a2,a6
20061a94:	03df76b3          	remu	a3,t5,t4
20061a98:	02658f33          	mul	t5,a1,t1
20061a9c:	06c2                	c.slli	a3,0x10
20061a9e:	8f55                	c.or	a4,a3
20061aa0:	869a                	c.mv	a3,t1
20061aa2:	01e77c63          	bgeu	a4,t5,20061aba <__umoddi3+0x282>
20061aa6:	9772                	c.add	a4,t3
20061aa8:	fff30693          	addi	a3,t1,-1
20061aac:	01c76763          	bltu	a4,t3,20061aba <__umoddi3+0x282>
20061ab0:	01e77563          	bgeu	a4,t5,20061aba <__umoddi3+0x282>
20061ab4:	ffe30693          	addi	a3,t1,-2
20061ab8:	9772                	c.add	a4,t3
20061aba:	41e70733          	sub	a4,a4,t5
20061abe:	03d77333          	remu	t1,a4,t4
20061ac2:	07c2                	c.slli	a5,0x10
20061ac4:	83c1                	c.srli	a5,0x10
20061ac6:	03d75733          	divu	a4,a4,t4
20061aca:	0342                	c.slli	t1,0x10
20061acc:	00f36333          	or	t1,t1,a5
20061ad0:	02e585b3          	mul	a1,a1,a4
20061ad4:	87ba                	c.mv	a5,a4
20061ad6:	00b37c63          	bgeu	t1,a1,20061aee <__umoddi3+0x2b6>
20061ada:	9372                	c.add	t1,t3
20061adc:	fff70793          	addi	a5,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20061ae0:	01c36763          	bltu	t1,t3,20061aee <__umoddi3+0x2b6>
20061ae4:	00b37563          	bgeu	t1,a1,20061aee <__umoddi3+0x2b6>
20061ae8:	ffe70793          	addi	a5,a4,-2
20061aec:	9372                	c.add	t1,t3
20061aee:	06c2                	c.slli	a3,0x10
20061af0:	6f41                	c.lui	t5,0x10
20061af2:	8edd                	c.or	a3,a5
20061af4:	40b305b3          	sub	a1,t1,a1
20061af8:	ffff0313          	addi	t1,t5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061afc:	0066feb3          	and	t4,a3,t1
20061b00:	01065713          	srli	a4,a2,0x10
20061b04:	82c1                	c.srli	a3,0x10
20061b06:	00667333          	and	t1,a2,t1
20061b0a:	026e87b3          	mul	a5,t4,t1
20061b0e:	02668333          	mul	t1,a3,t1
20061b12:	02ee8eb3          	mul	t4,t4,a4
20061b16:	02e686b3          	mul	a3,a3,a4
20061b1a:	9e9a                	c.add	t4,t1
20061b1c:	0107d713          	srli	a4,a5,0x10
20061b20:	9776                	c.add	a4,t4
20061b22:	00677363          	bgeu	a4,t1,20061b28 <__umoddi3+0x2f0>
20061b26:	96fa                	c.add	a3,t5
20061b28:	01075313          	srli	t1,a4,0x10
20061b2c:	969a                	c.add	a3,t1
20061b2e:	6341                	c.lui	t1,0x10
20061b30:	137d                	c.addi	t1,-1
20061b32:	00677733          	and	a4,a4,t1
20061b36:	0742                	c.slli	a4,0x10
20061b38:	0067f7b3          	and	a5,a5,t1
20061b3c:	97ba                	c.add	a5,a4
20061b3e:	00d5e663          	bltu	a1,a3,20061b4a <__umoddi3+0x312>
20061b42:	00d59b63          	bne	a1,a3,20061b58 <__umoddi3+0x320>
20061b46:	00f57963          	bgeu	a0,a5,20061b58 <__umoddi3+0x320>
20061b4a:	40c78633          	sub	a2,a5,a2
20061b4e:	00c7b7b3          	sltu	a5,a5,a2
20061b52:	97f2                	c.add	a5,t3
20061b54:	8e9d                	c.sub	a3,a5
20061b56:	87b2                	c.mv	a5,a2
20061b58:	40f507b3          	sub	a5,a0,a5
20061b5c:	00f53533          	sltu	a0,a0,a5
20061b60:	8d95                	c.sub	a1,a3
20061b62:	8d89                	c.sub	a1,a0
20061b64:	011598b3          	sll	a7,a1,a7
20061b68:	0107d533          	srl	a0,a5,a6
20061b6c:	00a8e533          	or	a0,a7,a0
20061b70:	0105d5b3          	srl	a1,a1,a6
20061b74:	bb8d                	c.j	200618e6 <__umoddi3+0xae>

20061b76 <atoi>:
20061b76:	4629                	c.li	a2,10
20061b78:	4581                	c.li	a1,0
20061b7a:	a1a5                	c.j	20061fe2 <strtol>

20061b7c <_atoi_r>:
20061b7c:	46a9                	c.li	a3,10
20061b7e:	4601                	c.li	a2,0
20061b80:	a991                	c.j	20061fd4 <_strtol_r>

20061b82 <__errno>:
20061b82:	e941a503          	lw	a0,-364(gp) # 2006681c <_impure_ptr>
20061b86:	8082                	c.jr	ra

20061b88 <__libc_init_array>:
20061b88:	1141                	c.addi	sp,-16
20061b8a:	c422                	c.swsp	s0,8(sp)
20061b8c:	c226                	c.swsp	s1,4(sp)
20061b8e:	20065437          	lui	s0,0x20065
20061b92:	200654b7          	lui	s1,0x20065
20061b96:	db448793          	addi	a5,s1,-588 # 20064db4 <__frame_dummy_init_array_entry>
20061b9a:	db440413          	addi	s0,s0,-588 # 20064db4 <__frame_dummy_init_array_entry>
20061b9e:	8c1d                	c.sub	s0,a5
20061ba0:	c04a                	c.swsp	s2,0(sp)
20061ba2:	c606                	c.swsp	ra,12(sp)
20061ba4:	8409                	c.srai	s0,0x2
20061ba6:	db448493          	addi	s1,s1,-588
20061baa:	4901                	c.li	s2,0
20061bac:	02891763          	bne	s2,s0,20061bda <__libc_init_array+0x52>
20061bb0:	200654b7          	lui	s1,0x20065
20061bb4:	20065437          	lui	s0,0x20065
20061bb8:	db448793          	addi	a5,s1,-588 # 20064db4 <__frame_dummy_init_array_entry>
20061bbc:	db840413          	addi	s0,s0,-584 # 20064db8 <__do_global_dtors_aux_fini_array_entry>
20061bc0:	8c1d                	c.sub	s0,a5
20061bc2:	8409                	c.srai	s0,0x2
20061bc4:	db448493          	addi	s1,s1,-588
20061bc8:	4901                	c.li	s2,0
20061bca:	00891d63          	bne	s2,s0,20061be4 <__libc_init_array+0x5c>
20061bce:	40b2                	c.lwsp	ra,12(sp)
20061bd0:	4422                	c.lwsp	s0,8(sp)
20061bd2:	4492                	c.lwsp	s1,4(sp)
20061bd4:	4902                	c.lwsp	s2,0(sp)
20061bd6:	0141                	c.addi	sp,16
20061bd8:	8082                	c.jr	ra
20061bda:	409c                	c.lw	a5,0(s1)
20061bdc:	0905                	c.addi	s2,1
20061bde:	0491                	c.addi	s1,4
20061be0:	9782                	c.jalr	a5
20061be2:	b7e9                	c.j	20061bac <__libc_init_array+0x24>
20061be4:	409c                	c.lw	a5,0(s1)
20061be6:	0905                	c.addi	s2,1
20061be8:	0491                	c.addi	s1,4
20061bea:	9782                	c.jalr	a5
20061bec:	bff9                	c.j	20061bca <__libc_init_array+0x42>

20061bee <cleanup_glue>:
20061bee:	1141                	c.addi	sp,-16
20061bf0:	c422                	c.swsp	s0,8(sp)
20061bf2:	842e                	c.mv	s0,a1
20061bf4:	418c                	c.lw	a1,0(a1)
20061bf6:	c226                	c.swsp	s1,4(sp)
20061bf8:	c606                	c.swsp	ra,12(sp)
20061bfa:	84aa                	c.mv	s1,a0
20061bfc:	c191                	c.beqz	a1,20061c00 <cleanup_glue+0x12>
20061bfe:	3fc5                	c.jal	20061bee <cleanup_glue>
20061c00:	85a2                	c.mv	a1,s0
20061c02:	4422                	c.lwsp	s0,8(sp)
20061c04:	40b2                	c.lwsp	ra,12(sp)
20061c06:	8526                	c.mv	a0,s1
20061c08:	4492                	c.lwsp	s1,4(sp)
20061c0a:	0141                	c.addi	sp,16
20061c0c:	fb5f806f          	jal	zero,2005abc0 <__wrap__free_r>

20061c10 <_reclaim_reent>:
20061c10:	e941a783          	lw	a5,-364(gp) # 2006681c <_impure_ptr>
20061c14:	0ea78263          	beq	a5,a0,20061cf8 <_reclaim_reent+0xe8>
20061c18:	515c                	c.lw	a5,36(a0)
20061c1a:	1101                	c.addi	sp,-32
20061c1c:	cc22                	c.swsp	s0,24(sp)
20061c1e:	ce06                	c.swsp	ra,28(sp)
20061c20:	ca26                	c.swsp	s1,20(sp)
20061c22:	c84a                	c.swsp	s2,16(sp)
20061c24:	c64e                	c.swsp	s3,12(sp)
20061c26:	842a                	c.mv	s0,a0
20061c28:	cf81                	c.beqz	a5,20061c40 <_reclaim_reent+0x30>
20061c2a:	47dc                	c.lw	a5,12(a5)
20061c2c:	4481                	c.li	s1,0
20061c2e:	08000913          	addi	s2,zero,128
20061c32:	efc9                	c.bnez	a5,20061ccc <_reclaim_reent+0xbc>
20061c34:	505c                	c.lw	a5,36(s0)
20061c36:	438c                	c.lw	a1,0(a5)
20061c38:	c581                	c.beqz	a1,20061c40 <_reclaim_reent+0x30>
20061c3a:	8522                	c.mv	a0,s0
20061c3c:	f85f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c40:	484c                	c.lw	a1,20(s0)
20061c42:	c581                	c.beqz	a1,20061c4a <_reclaim_reent+0x3a>
20061c44:	8522                	c.mv	a0,s0
20061c46:	f7bf80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c4a:	504c                	c.lw	a1,36(s0)
20061c4c:	c581                	c.beqz	a1,20061c54 <_reclaim_reent+0x44>
20061c4e:	8522                	c.mv	a0,s0
20061c50:	f71f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c54:	5c0c                	c.lw	a1,56(s0)
20061c56:	c581                	c.beqz	a1,20061c5e <_reclaim_reent+0x4e>
20061c58:	8522                	c.mv	a0,s0
20061c5a:	f67f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c5e:	5c4c                	c.lw	a1,60(s0)
20061c60:	c581                	c.beqz	a1,20061c68 <_reclaim_reent+0x58>
20061c62:	8522                	c.mv	a0,s0
20061c64:	f5df80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c68:	402c                	c.lw	a1,64(s0)
20061c6a:	c581                	c.beqz	a1,20061c72 <_reclaim_reent+0x62>
20061c6c:	8522                	c.mv	a0,s0
20061c6e:	f53f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c72:	0ec42583          	lw	a1,236(s0)
20061c76:	c581                	c.beqz	a1,20061c7e <_reclaim_reent+0x6e>
20061c78:	8522                	c.mv	a0,s0
20061c7a:	f47f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c7e:	0e842583          	lw	a1,232(s0)
20061c82:	c581                	c.beqz	a1,20061c8a <_reclaim_reent+0x7a>
20061c84:	8522                	c.mv	a0,s0
20061c86:	f3bf80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c8a:	443c                	c.lw	a5,72(s0)
20061c8c:	c799                	c.beqz	a5,20061c9a <_reclaim_reent+0x8a>
20061c8e:	0887a583          	lw	a1,136(a5) # 10088 <mbedtls_ecp_check_pub_priv+0x1aa>
20061c92:	c581                	c.beqz	a1,20061c9a <_reclaim_reent+0x8a>
20061c94:	8522                	c.mv	a0,s0
20061c96:	f2bf80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c9a:	584c                	c.lw	a1,52(s0)
20061c9c:	c581                	c.beqz	a1,20061ca4 <_reclaim_reent+0x94>
20061c9e:	8522                	c.mv	a0,s0
20061ca0:	f21f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061ca4:	4c1c                	c.lw	a5,24(s0)
20061ca6:	c3b1                	c.beqz	a5,20061cea <_reclaim_reent+0xda>
20061ca8:	541c                	c.lw	a5,40(s0)
20061caa:	8522                	c.mv	a0,s0
20061cac:	9782                	c.jalr	a5
20061cae:	0d842583          	lw	a1,216(s0)
20061cb2:	cd85                	c.beqz	a1,20061cea <_reclaim_reent+0xda>
20061cb4:	8522                	c.mv	a0,s0
20061cb6:	4462                	c.lwsp	s0,24(sp)
20061cb8:	40f2                	c.lwsp	ra,28(sp)
20061cba:	44d2                	c.lwsp	s1,20(sp)
20061cbc:	4942                	c.lwsp	s2,16(sp)
20061cbe:	49b2                	c.lwsp	s3,12(sp)
20061cc0:	6105                	c.addi16sp	sp,32
20061cc2:	b735                	c.j	20061bee <cleanup_glue>
20061cc4:	95a6                	c.add	a1,s1
20061cc6:	418c                	c.lw	a1,0(a1)
20061cc8:	e991                	c.bnez	a1,20061cdc <_reclaim_reent+0xcc>
20061cca:	0491                	c.addi	s1,4
20061ccc:	505c                	c.lw	a5,36(s0)
20061cce:	47cc                	c.lw	a1,12(a5)
20061cd0:	ff249ae3          	bne	s1,s2,20061cc4 <_reclaim_reent+0xb4>
20061cd4:	8522                	c.mv	a0,s0
20061cd6:	eebf80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061cda:	bfa9                	c.j	20061c34 <_reclaim_reent+0x24>
20061cdc:	0005a983          	lw	s3,0(a1)
20061ce0:	8522                	c.mv	a0,s0
20061ce2:	edff80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061ce6:	85ce                	c.mv	a1,s3
20061ce8:	b7c5                	c.j	20061cc8 <_reclaim_reent+0xb8>
20061cea:	40f2                	c.lwsp	ra,28(sp)
20061cec:	4462                	c.lwsp	s0,24(sp)
20061cee:	44d2                	c.lwsp	s1,20(sp)
20061cf0:	4942                	c.lwsp	s2,16(sp)
20061cf2:	49b2                	c.lwsp	s3,12(sp)
20061cf4:	6105                	c.addi16sp	sp,32
20061cf6:	8082                	c.jr	ra
20061cf8:	8082                	c.jr	ra

20061cfa <strcpy>:
20061cfa:	00b56733          	or	a4,a0,a1
20061cfe:	8b0d                	c.andi	a4,3
20061d00:	87aa                	c.mv	a5,a0
20061d02:	e729                	c.bnez	a4,20061d4c <strcpy+0x52>
20061d04:	7f7f86b7          	lui	a3,0x7f7f8
20061d08:	f7f68693          	addi	a3,a3,-129 # 7f7f7f7f <__kr4_psram_text_start__+0x1f677f5f>
20061d0c:	587d                	c.li	a6,-1
20061d0e:	4190                	c.lw	a2,0(a1)
20061d10:	00d67733          	and	a4,a2,a3
20061d14:	9736                	c.add	a4,a3
20061d16:	8f51                	c.or	a4,a2
20061d18:	8f55                	c.or	a4,a3
20061d1a:	03070463          	beq	a4,a6,20061d42 <strcpy+0x48>
20061d1e:	0005c603          	lbu	a2,0(a1)
20061d22:	0015c683          	lbu	a3,1(a1)
20061d26:	0025c703          	lbu	a4,2(a1)
20061d2a:	00c78023          	sb	a2,0(a5)
20061d2e:	c615                	c.beqz	a2,20061d5a <strcpy+0x60>
20061d30:	00d780a3          	sb	a3,1(a5)
20061d34:	c29d                	c.beqz	a3,20061d5a <strcpy+0x60>
20061d36:	00e78123          	sb	a4,2(a5)
20061d3a:	c305                	c.beqz	a4,20061d5a <strcpy+0x60>
20061d3c:	000781a3          	sb	zero,3(a5)
20061d40:	8082                	c.jr	ra
20061d42:	0791                	c.addi	a5,4
20061d44:	0591                	c.addi	a1,4
20061d46:	fec7ae23          	sw	a2,-4(a5)
20061d4a:	b7d1                	c.j	20061d0e <strcpy+0x14>
20061d4c:	0005c703          	lbu	a4,0(a1)
20061d50:	0785                	c.addi	a5,1
20061d52:	0585                	c.addi	a1,1
20061d54:	fee78fa3          	sb	a4,-1(a5)
20061d58:	fb75                	c.bnez	a4,20061d4c <strcpy+0x52>
20061d5a:	8082                	c.jr	ra

20061d5c <strncmp>:
20061d5c:	c625                	c.beqz	a2,20061dc4 <strncmp+0x68>
20061d5e:	00b567b3          	or	a5,a0,a1
20061d62:	8b8d                	c.andi	a5,3
20061d64:	e385                	c.bnez	a5,20061d84 <strncmp+0x28>
20061d66:	feff06b7          	lui	a3,0xfeff0
20061d6a:	80808837          	lui	a6,0x80808
20061d6e:	488d                	c.li	a7,3
20061d70:	eff68693          	addi	a3,a3,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061d74:	08080813          	addi	a6,a6,128 # 80808080 <__ctrace_end__+0xb6808080>
20061d78:	00c8f663          	bgeu	a7,a2,20061d84 <strncmp+0x28>
20061d7c:	411c                	c.lw	a5,0(a0)
20061d7e:	4198                	c.lw	a4,0(a1)
20061d80:	02e78163          	beq	a5,a4,20061da2 <strncmp+0x46>
20061d84:	167d                	c.addi	a2,-1
20061d86:	4701                	c.li	a4,0
20061d88:	00e507b3          	add	a5,a0,a4
20061d8c:	00e586b3          	add	a3,a1,a4
20061d90:	0007c783          	lbu	a5,0(a5)
20061d94:	0006c683          	lbu	a3,0(a3)
20061d98:	02d78263          	beq	a5,a3,20061dbc <strncmp+0x60>
20061d9c:	40d78533          	sub	a0,a5,a3
20061da0:	8082                	c.jr	ra
20061da2:	1671                	c.addi	a2,-4
20061da4:	c205                	c.beqz	a2,20061dc4 <strncmp+0x68>
20061da6:	00d78733          	add	a4,a5,a3
20061daa:	fff7c793          	xori	a5,a5,-1
20061dae:	8ff9                	c.and	a5,a4
20061db0:	0107f7b3          	and	a5,a5,a6
20061db4:	eb81                	c.bnez	a5,20061dc4 <strncmp+0x68>
20061db6:	0511                	c.addi	a0,4
20061db8:	0591                	c.addi	a1,4
20061dba:	bf7d                	c.j	20061d78 <strncmp+0x1c>
20061dbc:	00c70463          	beq	a4,a2,20061dc4 <strncmp+0x68>
20061dc0:	0705                	c.addi	a4,1
20061dc2:	f3f9                	c.bnez	a5,20061d88 <strncmp+0x2c>
20061dc4:	4501                	c.li	a0,0
20061dc6:	8082                	c.jr	ra

20061dc8 <strncpy>:
20061dc8:	00a5e733          	or	a4,a1,a0
20061dcc:	8b0d                	c.andi	a4,3
20061dce:	87aa                	c.mv	a5,a0
20061dd0:	e701                	c.bnez	a4,20061dd8 <strncpy+0x10>
20061dd2:	470d                	c.li	a4,3
20061dd4:	04c76163          	bltu	a4,a2,20061e16 <strncpy+0x4e>
20061dd8:	ca09                	c.beqz	a2,20061dea <strncpy+0x22>
20061dda:	0005c703          	lbu	a4,0(a1)
20061dde:	0585                	c.addi	a1,1
20061de0:	0785                	c.addi	a5,1
20061de2:	fee78fa3          	sb	a4,-1(a5)
20061de6:	167d                	c.addi	a2,-1
20061de8:	fb65                	c.bnez	a4,20061dd8 <strncpy+0x10>
20061dea:	963e                	c.add	a2,a5
20061dec:	02f61f63          	bne	a2,a5,20061e2a <strncpy+0x62>
20061df0:	8082                	c.jr	ra
20061df2:	1671                	c.addi	a2,-4
20061df4:	0107a023          	sw	a6,0(a5)
20061df8:	0791                	c.addi	a5,4
20061dfa:	0591                	c.addi	a1,4
20061dfc:	fcce7ee3          	bgeu	t3,a2,20061dd8 <strncpy+0x10>
20061e00:	0005a803          	lw	a6,0(a1)
20061e04:	01180733          	add	a4,a6,a7
20061e08:	fff84313          	xori	t1,a6,-1
20061e0c:	00677733          	and	a4,a4,t1
20061e10:	8f75                	c.and	a4,a3
20061e12:	d365                	c.beqz	a4,20061df2 <strncpy+0x2a>
20061e14:	b7d1                	c.j	20061dd8 <strncpy+0x10>
20061e16:	feff08b7          	lui	a7,0xfeff0
20061e1a:	808086b7          	lui	a3,0x80808
20061e1e:	eff88893          	addi	a7,a7,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061e22:	08068693          	addi	a3,a3,128 # 80808080 <__ctrace_end__+0xb6808080>
20061e26:	4e0d                	c.li	t3,3
20061e28:	bfe1                	c.j	20061e00 <strncpy+0x38>
20061e2a:	0785                	c.addi	a5,1
20061e2c:	fe078fa3          	sb	zero,-1(a5)
20061e30:	bf75                	c.j	20061dec <strncpy+0x24>

20061e32 <strsep>:
20061e32:	862a                	c.mv	a2,a0
20061e34:	4108                	c.lw	a0,0(a0)
20061e36:	4681                	c.li	a3,0
20061e38:	a009                	c.j	20061e3a <__strtok_r>

20061e3a <__strtok_r>:
20061e3a:	e119                	c.bnez	a0,20061e40 <__strtok_r+0x6>
20061e3c:	4208                	c.lw	a0,0(a2)
20061e3e:	c939                	c.beqz	a0,20061e94 <__strtok_r+0x5a>
20061e40:	87aa                	c.mv	a5,a0
20061e42:	853e                	c.mv	a0,a5
20061e44:	0007c803          	lbu	a6,0(a5)
20061e48:	0785                	c.addi	a5,1
20061e4a:	872e                	c.mv	a4,a1
20061e4c:	00074883          	lbu	a7,0(a4)
20061e50:	0705                	c.addi	a4,1
20061e52:	00089863          	bne	a7,zero,20061e62 <__strtok_r+0x28>
20061e56:	02081063          	bne	a6,zero,20061e76 <__strtok_r+0x3c>
20061e5a:	00062023          	sw	zero,0(a2)
20061e5e:	4501                	c.li	a0,0
20061e60:	8082                	c.jr	ra
20061e62:	ff1815e3          	bne	a6,a7,20061e4c <__strtok_r+0x12>
20061e66:	fef1                	c.bnez	a3,20061e42 <__strtok_r+0x8>
20061e68:	c21c                	c.sw	a5,0(a2)
20061e6a:	00050023          	sb	zero,0(a0)
20061e6e:	8082                	c.jr	ra
20061e70:	4781                	c.li	a5,0
20061e72:	a839                	c.j	20061e90 <__strtok_r+0x56>
20061e74:	e689                	c.bnez	a3,20061e7e <__strtok_r+0x44>
20061e76:	0007c803          	lbu	a6,0(a5)
20061e7a:	0785                	c.addi	a5,1
20061e7c:	872e                	c.mv	a4,a1
20061e7e:	00074683          	lbu	a3,0(a4)
20061e82:	0705                	c.addi	a4,1
20061e84:	fed818e3          	bne	a6,a3,20061e74 <__strtok_r+0x3a>
20061e88:	fe0804e3          	beq	a6,zero,20061e70 <__strtok_r+0x36>
20061e8c:	fe078fa3          	sb	zero,-1(a5)
20061e90:	c21c                	c.sw	a5,0(a2)
20061e92:	8082                	c.jr	ra
20061e94:	8082                	c.jr	ra

20061e96 <strtok_r>:
20061e96:	4685                	c.li	a3,1
20061e98:	b74d                	c.j	20061e3a <__strtok_r>

20061e9a <_strtol_l.constprop.0>:
20061e9a:	4785                	c.li	a5,1
20061e9c:	0af68863          	beq	a3,a5,20061f4c <_strtol_l.constprop.0+0xb2>
20061ea0:	02400793          	addi	a5,zero,36
20061ea4:	872e                	c.mv	a4,a1
20061ea6:	0ad7e363          	bltu	a5,a3,20061f4c <_strtol_l.constprop.0+0xb2>
20061eaa:	20065837          	lui	a6,0x20065
20061eae:	8eaa                	c.mv	t4,a0
20061eb0:	b3180813          	addi	a6,a6,-1231 # 20064b31 <_ctype_+0x1>
20061eb4:	88ba                	c.mv	a7,a4
20061eb6:	00074783          	lbu	a5,0(a4)
20061eba:	0705                	c.addi	a4,1
20061ebc:	01078533          	add	a0,a5,a6
20061ec0:	00054503          	lbu	a0,0(a0)
20061ec4:	8921                	c.andi	a0,8
20061ec6:	f57d                	c.bnez	a0,20061eb4 <_strtol_l.constprop.0+0x1a>
20061ec8:	02d00513          	addi	a0,zero,45
20061ecc:	08a79a63          	bne	a5,a0,20061f60 <_strtol_l.constprop.0+0xc6>
20061ed0:	00074783          	lbu	a5,0(a4)
20061ed4:	4e05                	c.li	t3,1
20061ed6:	00288713          	addi	a4,a7,2
20061eda:	c6f5                	c.beqz	a3,20061fc6 <_strtol_l.constprop.0+0x12c>
20061edc:	4541                	c.li	a0,16
20061ede:	02a69263          	bne	a3,a0,20061f02 <_strtol_l.constprop.0+0x68>
20061ee2:	03000513          	addi	a0,zero,48
20061ee6:	00a79e63          	bne	a5,a0,20061f02 <_strtol_l.constprop.0+0x68>
20061eea:	00074783          	lbu	a5,0(a4)
20061eee:	05800513          	addi	a0,zero,88
20061ef2:	0df7f793          	andi	a5,a5,223
20061ef6:	0ca79363          	bne	a5,a0,20061fbc <_strtol_l.constprop.0+0x122>
20061efa:	00174783          	lbu	a5,1(a4)
20061efe:	46c1                	c.li	a3,16
20061f00:	0709                	c.addi	a4,2
20061f02:	800008b7          	lui	a7,0x80000
20061f06:	fff8c893          	xori	a7,a7,-1
20061f0a:	98f2                	c.add	a7,t3
20061f0c:	02d8f2b3          	remu	t0,a7,a3
20061f10:	4301                	c.li	t1,0
20061f12:	4501                	c.li	a0,0
20061f14:	43a5                	c.li	t2,9
20061f16:	4fe5                	c.li	t6,25
20061f18:	02d8df33          	divu	t5,a7,a3
20061f1c:	fd078813          	addi	a6,a5,-48
20061f20:	0503ea63          	bltu	t2,a6,20061f74 <_strtol_l.constprop.0+0xda>
20061f24:	87c2                	c.mv	a5,a6
20061f26:	06d7d763          	bge	a5,a3,20061f94 <_strtol_l.constprop.0+0xfa>
20061f2a:	06034363          	blt	t1,zero,20061f90 <_strtol_l.constprop.0+0xf6>
20061f2e:	537d                	c.li	t1,-1
20061f30:	00af6a63          	bltu	t5,a0,20061f44 <_strtol_l.constprop.0+0xaa>
20061f34:	00af1463          	bne	t5,a0,20061f3c <_strtol_l.constprop.0+0xa2>
20061f38:	00f2c663          	blt	t0,a5,20061f44 <_strtol_l.constprop.0+0xaa>
20061f3c:	4305                	c.li	t1,1
20061f3e:	02a68533          	mul	a0,a3,a0
20061f42:	953e                	c.add	a0,a5
20061f44:	0705                	c.addi	a4,1
20061f46:	fff74783          	lbu	a5,-1(a4)
20061f4a:	bfc9                	c.j	20061f1c <_strtol_l.constprop.0+0x82>
20061f4c:	1141                	c.addi	sp,-16
20061f4e:	c606                	c.swsp	ra,12(sp)
20061f50:	c33ff0ef          	jal	ra,20061b82 <__errno>
20061f54:	40b2                	c.lwsp	ra,12(sp)
20061f56:	47d9                	c.li	a5,22
20061f58:	c11c                	c.sw	a5,0(a0)
20061f5a:	4501                	c.li	a0,0
20061f5c:	0141                	c.addi	sp,16
20061f5e:	8082                	c.jr	ra
20061f60:	02b00513          	addi	a0,zero,43
20061f64:	4e01                	c.li	t3,0
20061f66:	f6a79ae3          	bne	a5,a0,20061eda <_strtol_l.constprop.0+0x40>
20061f6a:	00074783          	lbu	a5,0(a4)
20061f6e:	00288713          	addi	a4,a7,2 # 80000002 <__ctrace_end__+0xb6000002>
20061f72:	b7a5                	c.j	20061eda <_strtol_l.constprop.0+0x40>
20061f74:	fbf78813          	addi	a6,a5,-65
20061f78:	010fe563          	bltu	t6,a6,20061f82 <_strtol_l.constprop.0+0xe8>
20061f7c:	fc978793          	addi	a5,a5,-55
20061f80:	b75d                	c.j	20061f26 <_strtol_l.constprop.0+0x8c>
20061f82:	f9f78813          	addi	a6,a5,-97
20061f86:	010fe763          	bltu	t6,a6,20061f94 <_strtol_l.constprop.0+0xfa>
20061f8a:	fa978793          	addi	a5,a5,-87
20061f8e:	bf61                	c.j	20061f26 <_strtol_l.constprop.0+0x8c>
20061f90:	537d                	c.li	t1,-1
20061f92:	bf4d                	c.j	20061f44 <_strtol_l.constprop.0+0xaa>
20061f94:	00035963          	bge	t1,zero,20061fa6 <_strtol_l.constprop.0+0x10c>
20061f98:	02200793          	addi	a5,zero,34
20061f9c:	00fea023          	sw	a5,0(t4)
20061fa0:	8546                	c.mv	a0,a7
20061fa2:	ea09                	c.bnez	a2,20061fb4 <_strtol_l.constprop.0+0x11a>
20061fa4:	8082                	c.jr	ra
20061fa6:	000e0463          	beq	t3,zero,20061fae <_strtol_l.constprop.0+0x114>
20061faa:	40a00533          	sub	a0,zero,a0
20061fae:	c215                	c.beqz	a2,20061fd2 <_strtol_l.constprop.0+0x138>
20061fb0:	00030463          	beq	t1,zero,20061fb8 <_strtol_l.constprop.0+0x11e>
20061fb4:	fff70593          	addi	a1,a4,-1
20061fb8:	c20c                	c.sw	a1,0(a2)
20061fba:	8082                	c.jr	ra
20061fbc:	03000793          	addi	a5,zero,48
20061fc0:	f2a9                	c.bnez	a3,20061f02 <_strtol_l.constprop.0+0x68>
20061fc2:	46a1                	c.li	a3,8
20061fc4:	bf3d                	c.j	20061f02 <_strtol_l.constprop.0+0x68>
20061fc6:	03000513          	addi	a0,zero,48
20061fca:	f2a780e3          	beq	a5,a0,20061eea <_strtol_l.constprop.0+0x50>
20061fce:	46a9                	c.li	a3,10
20061fd0:	bf0d                	c.j	20061f02 <_strtol_l.constprop.0+0x68>
20061fd2:	8082                	c.jr	ra

20061fd4 <_strtol_r>:
20061fd4:	b5d9                	c.j	20061e9a <_strtol_l.constprop.0>

20061fd6 <strtol_l>:
20061fd6:	86b2                	c.mv	a3,a2
20061fd8:	862e                	c.mv	a2,a1
20061fda:	85aa                	c.mv	a1,a0
20061fdc:	e941a503          	lw	a0,-364(gp) # 2006681c <_impure_ptr>
20061fe0:	bd6d                	c.j	20061e9a <_strtol_l.constprop.0>

20061fe2 <strtol>:
20061fe2:	86b2                	c.mv	a3,a2
20061fe4:	862e                	c.mv	a2,a1
20061fe6:	85aa                	c.mv	a1,a0
20061fe8:	e941a503          	lw	a0,-364(gp) # 2006681c <_impure_ptr>
20061fec:	b57d                	c.j	20061e9a <_strtol_l.constprop.0>
20061fee:	0000                	c.unimp
20061ff0:	6d6f682f          	0x6d6f682f
20061ff4:	2f65                	c.jal	200627ac <pmap_func+0x3a0>
20061ff6:	7561                	c.lui	a0,0xffff8
20061ff8:	6f74                	c.flw	fa3,92(a4)
20061ffa:	6a5f 6e65 696b      	0x696b6e656a5f
20062000:	736e                	c.flwsp	ft6,248(sp)
20062002:	775f 616c 356e      	0x356e616c775f
20062008:	544f492f          	0x544f492f
2006200c:	415f 5354 722f      	0x722f5354415f
20062012:	7065                	c.lui	zero,0xffff9
20062014:	722f736f          	jal	t1,20159736 <__ap_sram_heap_start+0xd9736>
20062018:	6c65                	c.lui	s8,0x19
2006201a:	6165                	c.addi16sp	sp,112
2006201c:	6d2d6573          	csrrsi	a0,0x6d2,26
20062020:	7361                	c.lui	t1,0xffff8
20062022:	6574                	c.flw	fa3,76(a0)
20062024:	2f72                	c.fldsp	ft10,280(sp)
20062026:	72756f73          	csrrsi	t5,0x727,10
2006202a:	612f6563          	bltu	t5,s2,20062634 <pmap_func+0x228>
2006202e:	656d                	c.lui	a0,0x1b
20062030:	6162                	c.flwsp	ft2,24(sp)
20062032:	696c                	c.flw	fa1,84(a0)
20062034:	6574                	c.flw	fa3,76(a0)
20062036:	675f 6363 705f      	0x705f6363675f
2006203c:	6f72                	c.flwsp	ft10,28(sp)
2006203e:	656a                	c.flwsp	fa0,152(sp)
20062040:	702f7463          	bgeu	t5,sp,20062748 <pmap_func+0x33c>
20062044:	6f72                	c.flwsp	ft10,28(sp)
20062046:	656a                	c.flwsp	fa0,152(sp)
20062048:	6b5f7463          	bgeu	t5,s5,200626f0 <pmap_func+0x2e4>
2006204c:	3472                	c.fldsp	fs0,312(sp)
2006204e:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
20062052:	2e2e2f6b          	0x2e2e2f6b
20062056:	2f2e2e2f          	0x2f2e2e2f
2006205a:	2e2e                	c.fldsp	ft8,200(sp)
2006205c:	6d6f632f          	0x6d6f632f
20062060:	6f70                	c.flw	fa2,92(a4)
20062062:	656e                	c.flwsp	fa0,216(sp)
20062064:	746e                	c.flwsp	fs0,248(sp)
20062066:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
2006206a:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
2006206e:	6162                	c.flwsp	ft2,24(sp)
20062070:	696c                	c.flw	fa1,84(a0)
20062072:	6574                	c.flw	fa3,76(a0)
20062074:	6c77662f          	0x6c77662f
20062078:	6269                	c.lui	tp,0x1a
2006207a:	6d61722f          	0x6d61722f
2006207e:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20062084:	2f6e                	c.fldsp	ft10,216(sp)
20062086:	6d61                	c.lui	s10,0x18
20062088:	6265                	c.lui	tp,0x19
2006208a:	5f61                	c.li	t5,-8
2006208c:	6c66                	c.flwsp	fs8,88(sp)
2006208e:	7361                	c.lui	t1,0xffff8
20062090:	5f68                	c.lw	a0,124(a4)
20062092:	6172                	c.flwsp	ft2,28(sp)
20062094:	2e6d                	c.jal	2006244e <pmap_func+0x42>
20062096:	68430063          	beq	t1,tp,20062716 <pmap_func+0x30a>
2006209a:	6e61                	c.lui	t3,0x18
2006209c:	656e                	c.flwsp	fa0,216(sp)
2006209e:	206c                	c.fld	fa1,192(s0)
200620a0:	666e6f43          	fmadd.q	ft10,ft8,ft6,fa2,unknown
200620a4:	696c                	c.flw	fa1,84(a0)
200620a6:	66207463          	bgeu	zero,sp,2006270e <pmap_func+0x302>
200620aa:	4320726f          	jal	tp,200694dc <__bdram_heap_buffer_start__+0x9dc>
200620ae:	5550                	c.lw	a2,44(a0)
200620b0:	2520                	c.fld	fs0,72(a0)
200620b2:	756c                	c.flw	fa1,108(a0)
200620b4:	4320                	c.lw	s0,64(a4)
200620b6:	6168                	c.flw	fa0,68(a0)
200620b8:	6e6e                	c.flwsp	ft8,216(sp)
200620ba:	6c65                	c.lui	s8,0x19
200620bc:	2520                	c.fld	fs0,72(a0)
200620be:	756c                	c.flw	fa1,108(a0)
200620c0:	2120                	c.fld	fs0,64(a0)
200620c2:	4920                	c.lw	s0,80(a0)
200620c4:	726f6e67          	0x726f6e67
200620c8:	2065                	c.jal	20062170 <strtol+0x18e>
200620ca:	6649                	c.lui	a2,0x12
200620cc:	4320                	c.lw	s0,64(a4)
200620ce:	5550                	c.lw	a2,44(a0)
200620d0:	4820                	c.lw	s0,80(s0)
200620d2:	7361                	c.lui	t1,0xffff8
200620d4:	5220                	c.lw	s0,96(a2)
200620d6:	7365                	c.lui	t1,0xffff9
200620d8:	7465                	c.lui	s0,0xffff9
200620da:	0a0d                	c.addi	s4,3
200620dc:	0000                	c.unimp
200620de:	0000                	c.unimp
200620e0:	5049                	c.li	zero,-14
200620e2:	49200043          	fmadd.s	ft0,ft0,fs2,fs1,rne
200620e6:	4350                	c.lw	a2,4(a4)
200620e8:	5220                	c.lw	s0,96(a2)
200620ea:	7165                	c.addi16sp	sp,-400
200620ec:	6575                	c.lui	a0,0x1d
200620ee:	54207473          	csrrci	s0,0x542,0
200620f2:	6d69                	c.lui	s10,0x1a
200620f4:	6f65                	c.lui	t5,0x19
200620f6:	7475                	c.lui	s0,0xffffd
200620f8:	0a0d                	c.addi	s4,3
200620fa:	0000                	c.unimp
200620fc:	4920                	c.lw	s0,80(a0)
200620fe:	4350                	c.lw	a2,4(a4)
20062100:	4720                	c.lw	s0,72(a4)
20062102:	7465                	c.lui	s0,0xffff9
20062104:	5320                	c.lw	s0,96(a4)
20062106:	6d65                	c.lui	s10,0x19
20062108:	7061                	c.lui	zero,0xffff8
2006210a:	6f68                	c.flw	fa0,92(a4)
2006210c:	6572                	c.flwsp	fa0,28(sp)
2006210e:	5420                	c.lw	s0,104(s0)
20062110:	6d69                	c.lui	s10,0x1a
20062112:	6f65                	c.lui	t5,0x19
20062114:	7475                	c.lui	s0,0xffffd
20062116:	0a0d                	c.addi	s4,3
20062118:	0000                	c.unimp
2006211a:	0000                	c.unimp
2006211c:	6d6f682f          	0x6d6f682f
20062120:	2f65                	c.jal	200628d8 <pmap_func+0x4cc>
20062122:	7561                	c.lui	a0,0xffff8
20062124:	6f74                	c.flw	fa3,92(a4)
20062126:	6a5f 6e65 696b      	0x696b6e656a5f
2006212c:	736e                	c.flwsp	ft6,248(sp)
2006212e:	775f 616c 356e      	0x356e616c775f
20062134:	544f492f          	0x544f492f
20062138:	415f 5354 722f      	0x722f5354415f
2006213e:	7065                	c.lui	zero,0xffff9
20062140:	722f736f          	jal	t1,20159862 <__ap_sram_heap_start+0xd9862>
20062144:	6c65                	c.lui	s8,0x19
20062146:	6165                	c.addi16sp	sp,112
20062148:	6d2d6573          	csrrsi	a0,0x6d2,26
2006214c:	7361                	c.lui	t1,0xffff8
2006214e:	6574                	c.flw	fa3,76(a0)
20062150:	2f72                	c.fldsp	ft10,280(sp)
20062152:	72756f73          	csrrsi	t5,0x727,10
20062156:	612f6563          	bltu	t5,s2,20062760 <pmap_func+0x354>
2006215a:	656d                	c.lui	a0,0x1b
2006215c:	6162                	c.flwsp	ft2,24(sp)
2006215e:	696c                	c.flw	fa1,84(a0)
20062160:	6574                	c.flw	fa3,76(a0)
20062162:	675f 6363 705f      	0x705f6363675f
20062168:	6f72                	c.flwsp	ft10,28(sp)
2006216a:	656a                	c.flwsp	fa0,152(sp)
2006216c:	702f7463          	bgeu	t5,sp,20062874 <pmap_func+0x468>
20062170:	6f72                	c.flwsp	ft10,28(sp)
20062172:	656a                	c.flwsp	fa0,152(sp)
20062174:	6b5f7463          	bgeu	t5,s5,2006281c <pmap_func+0x410>
20062178:	3472                	c.fldsp	fs0,312(sp)
2006217a:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
2006217e:	2e2e2f6b          	0x2e2e2f6b
20062182:	2f2e2e2f          	0x2f2e2e2f
20062186:	2e2e                	c.fldsp	ft8,200(sp)
20062188:	6d6f632f          	0x6d6f632f
2006218c:	6f70                	c.flw	fa2,92(a4)
2006218e:	656e                	c.flwsp	fa0,216(sp)
20062190:	746e                	c.flwsp	fs0,248(sp)
20062192:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
20062196:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
2006219a:	6162                	c.flwsp	ft2,24(sp)
2006219c:	696c                	c.flw	fa1,84(a0)
2006219e:	6574                	c.flw	fa3,76(a0)
200621a0:	6c77662f          	0x6c77662f
200621a4:	6269                	c.lui	tp,0x1a
200621a6:	6d61722f          	0x6d61722f
200621aa:	635f 6d6f 6f6d      	0x6f6d6d6f635f
200621b0:	2f6e                	c.fldsp	ft10,216(sp)
200621b2:	6d61                	c.lui	s10,0x18
200621b4:	6265                	c.lui	tp,0x19
200621b6:	5f61                	c.li	t5,-8
200621b8:	7069                	c.lui	zero,0xffffa
200621ba:	70615f63          	bge	sp,t1,200628d8 <pmap_func+0x4cc>
200621be:	2e69                	c.jal	20062558 <pmap_func+0x14c>
200621c0:	00000063          	beq	zero,zero,200621c0 <strtol+0x1de>
200621c4:	5049                	c.li	zero,-14
200621c6:	69445f43          	fmadd.s	ft10,fs0,fs4,fa3,unknown
200621ca:	3a72                	c.fldsp	fs4,312(sp)
200621cc:	6425                	c.lui	s0,0x9
200621ce:	202c                	c.fld	fa1,64(s0)
200621d0:	5049                	c.li	zero,-14
200621d2:	58545f43          	fmadd.s	ft10,fs0,ft5,fa1,unknown
200621d6:	445f 5441 2041      	0x20415441445f
200621dc:	7830                	c.flw	fa2,112(s0)
200621de:	7825                	c.lui	a6,0xfffe9
200621e0:	4e20                	c.lw	s0,88(a2)
200621e2:	4320746f          	jal	s0,20069614 <__bdram_heap_buffer_start__+0xb14>
200621e6:	656c                	c.flw	fa1,76(a0)
200621e8:	6e61                	c.lui	t3,0x18
200621ea:	0d21                	c.addi	s10,8
200621ec:	000a                	c.slli	zero,0x2
	...

200621f0 <CSWTCH.29>:
200621f0:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20062200:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20062210:	0008 0000                                   ....

20062214 <CSWTCH.30>:
20062214:	0000 4102 0000 4102 0000 0000 0000 0000     ...A...A........
	...
20062254:	0080 4102 0080 4102 0000 0000 0000 0000     ...A...A........
	...
20062294:	0100 4102 0100 4102 682f 6d6f 2f65 7561     ...A...A/home/au
200622a4:	6f74 6a5f 6e65 696b 736e 775f 616c 356e     to_jenkins_wlan5
200622b4:	492f 544f 415f 5354 722f 7065 736f 722f     /IOT_ATS/repos/r
200622c4:	6c65 6165 6573 6d2d 7361 6574 2f72 6f73     elease-master/so
200622d4:	7275 6563 612f 656d 6162 696c 6574 675f     urce/amebalite_g
200622e4:	6363 705f 6f72 656a 7463 702f 6f72 656a     cc_project/proje
200622f4:	7463 6b5f 3472 762f 6473 2f6b 2e2e 2e2f     ct_kr4/vsdk/../.
20062304:	2f2e 2e2e 632f 6d6f 6f70 656e 746e 732f     ./../component/s
20062314:	636f 612f 656d 6162 696c 6574 662f 6c77     oc/amebalite/fwl
20062324:	6269 722f 6d61 635f 6d6f 6f6d 2f6e 6d61     ib/ram_common/am
20062334:	6265 5f61 7069 5f63 6172 2e6d 0063 0000     eba_ipc_ram.c...
20062344:	5043 2055 6425 7420 6b61 2065 6573 616d     CPU %d take sema
20062354:	2520 2064 6f66 2072 7870 6469 2520 2064      %d for pxid %d 
20062364:	6166 6c69 203a 7063 6975 2064 6425 7020     fail: cpuid %d p
20062374:	6978 2064 6425 6820 7361 7420 6b61 6e65     xid %d has taken
20062384:	7420 6568 7320 6d65 0a61 0000 5043 2055      the sema...CPU 
20062394:	6425 6620 6572 2065 6573 616d 2520 2064     %d free sema %d 
200623a4:	6f66 2072 7870 6469 2520 2064 6166 6c69     for pxid %d fail
200623b4:	203a 7063 6975 2064 6425 202c 7870 6469     : cpuid %d, pxid
200623c4:	2520 2064 7369 7420 6b61 6e69 2067 6874      %d is taking th
200623d4:	2065 6573 616d 000a 544f 2050 6572 6461     e sema..OTP read
200623e4:	6720 7465 6820 2077 6573 616d 6620 6961      get hw sema fai
200623f4:	0a6c 0000 544f 5f50 6552 6461 2038 6146     l...OTP_Read8 Fa
20062404:	6c69 2520 2078 000a                         il %x ..

2006240c <pmap_func>:
2006240c:	0000 0101 0001 0101 0002 0102 0003 0102     ................
2006241c:	0004 0202 0005 0102 0006 0202 0007 0102     ................
2006242c:	0008 0101 0009 0101 000a 0101 000b 0101     ................
2006243c:	000c 0101 000d 0101 000e 0101 000f 0101     ................
2006244c:	0010 0101 0011 0101 0012 0000 0013 0202     ................
2006245c:	0014 0202 0015 0101 0016 0000 0017 0101     ................
2006246c:	0018 0101 0019 0101 001a 0101 001b 0101     ................
2006247c:	001c 0101 001d 0101 001e 0101 001f 0101     ................
2006248c:	0020 0101 0021 0101 0022 0101 0023 0101      ...!..."...#...
2006249c:	0024 0101 0025 0101 0026 0101 0027 0101     $...%...&...'...
200624ac:	0028 0101 0029 0101 002a 0101 002b 0100     (...)...*...+...
200624bc:	002c 0100 002d 0100 002e 0100 002f 0100     ,...-......./...
200624cc:	0030 0100 0031 0100 0032 0100 0033 0100     0...1...2...3...
200624dc:	ffff ffff 766e 6369 695f 7172 2520 756c     ....nvic_irq %lu
200624ec:	4620 6e75 4e20 746f 4120 7373 6769 216e      Fun Not Assign!
200624fc:	2121 2121 000a 0000 4c50 4349 0000 0000     !!!!....PLIC....
2006250c:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
2006251c:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
2006252c:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
2006253c:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
2006254c:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
2006255c:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
2006256c:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
2006257c:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
2006258c:	696c 6574 6d2f 7369 2f63 6d61 6265 5f61     lite/misc/ameba_
2006259c:	6d70 2e75 0063 0000 6e49 6176 696c 2064     pmu.c...Invalid 
200625ac:	7350 6172 436d 6b6c 000a 0000 5350 4152     PsramClk....PSRA
200625bc:	004d 0000 7370 6172 206d 7974 6570 6920     M...psram type i
200625cc:	2073 7830 7825 000a 5353 4549 524e 3009     s 0x%x..SSIENR.0
200625dc:	2578 3d78 7830 7825 000a 0000 5443 4c52     x%x=0x%x....CTRL
200625ec:	3052 3009 2578 3d78 7830 7825 000a 0000     R0.0x%x=0x%x....
200625fc:	4142 4455 0952 7830 7825 303d 2578 0a78     BAUDR.0x%x=0x%x.
2006260c:	0000 0000 4156 494c 5f44 4d43 0944 7830     ....VALID_CMD.0x
2006261c:	7825 303d 2578 0a78 0000 0000 5443 4c52     %x=0x%x.....CTRL
2006262c:	3252 3009 2578 3d78 7830 7825 000a 0000     R2.0x%x=0x%x....
2006263c:	4544 4956 4543 495f 464e 094f 7830 7825     DEVICE_INFO.0x%x
2006264c:	303d 2578 0a78 0000 5054 3052 3009 2578     =0x%x...TPR0.0x%
2006265c:	3d78 7830 7825 000a 4552 4441 465f 5341     x=0x%x..READ_FAS
2006266c:	5f54 4953 474e 454c 3009 2578 3d78 7830     T_SINGLE.0x%x=0x
2006267c:	7825 000a 5257 5449 5f45 4953 4e47 454c     %x..WRITE_SIGNLE
2006268c:	3009 2578 3d78 7830 7825 000a 5541 4f54     .0x%x=0x%x..AUTO
2006269c:	4c5f 4e45 5447 0948 7830 7825 303d 2578     _LENGTH.0x%x=0x%
200626ac:	0a78 0000 5355 5245 4c5f 4e45 5447 0948     x...USER_LENGTH.
200626bc:	7830 7825 303d 2578 0a78 0000 5541 4f54     0x%x=0x%x...AUTO
200626cc:	4c5f 4e45 5447 3248 3009 2578 3d78 7830     _LENGTH2.0x%x=0x
200626dc:	7825 000a 5253 3009 2578 3d78 7830 7825     %x..SR.0x%x=0x%x
200626ec:	000a 0000 5350 4850 5f59 4143 5f4c 4150     ....PSPHY_CAL_PA
200626fc:	0952 7830 7825 303d 2578 0a78 0000 0000     R.0x%x=0x%x.....
2006270c:	5350 4850 5f59 4850 5f59 5443 4c52 3009     PSPHY_PHY_CTRL.0
2006271c:	2578 3d78 7830 7825 000a 0000 5350 4850     x%x=0x%x....PSPH
2006272c:	5f59 4244 5f47 4553 094c 7830 7825 303d     Y_DBG_SEL.0x%x=0
2006273c:	2578 0a78 0000 0000 5249 5b30 5d31 303d     x%x.....IR0[1]=0
2006274c:	2578 0a78 0000 0000 5249 5b30 5d30 303d     x%x.....IR0[0]=0
2006275c:	2578 0a78 0000 0000 5243 5b30 5d31 303d     x%x.....CR0[1]=0
2006276c:	2578 0a78 0000 0000 5243 5b30 5d30 303d     x%x.....CR0[0]=0
2006277c:	2578 0a78 0000 0000 5243 5b31 5d31 303d     x%x.....CR1[1]=0
2006278c:	2578 0a78 0000 0000 5243 5b31 5d30 303d     x%x.....CR1[0]=0
2006279c:	2578 0a78 0000 0000 3d3d 703d 6168 6573     x%x.....===phase
200627ac:	2520 786c 3d20 3d3d 3d3d 000a 6b6f 2520      %lx =====..ok %
200627bc:	786c 2520 786c 2520 786c 2520 786c 2520     lx %lx %lx %lx %
200627cc:	786c 2520 786c 2520 786c 000a 6166 6c69     lx %lx %lx..fail
200627dc:	2520 786c 2520 786c 2520 786c 2520 786c      %lx %lx %lx %lx
200627ec:	2520 786c 2520 786c 2520 786c 000a 0000      %lx %lx %lx....
200627fc:	6143 4e6c 696d 206e 203d 7825 4320 6c61     CalNmin = %x Cal
2006280c:	6d4e 7861 3d20 2520 2078 6957 646e 776f     Nmax = %x Window
2006281c:	6953 657a 3d20 2520 2078 6870 7361 3a65     Size = %x phase:
2006282c:	2520 2078 000a 0000 5441 0000 003d 0000      %x ....AT..=...
2006283c:	002c 0000 7769 7270 7669 0020 0d0a 4d5b     ,...iwpriv ...[M
2006284c:	4d45 205d 6641 6574 2072 6f64 6320 646d     EM] After do cmd
2006285c:	202c 7661 6961 616c 6c62 2065 6568 7061     , available heap
2006286c:	2520 0a64 000d 0000 0a0d 230a 0a0d 0000      %d........#....
2006287c:	0000 0000 4f2b 4154 4c43 4145 3a52 0000     ....+OTACLEAR:..
2006288c:	0a0d 7325 4b4f 0a0d 0000 0000 4f2b 4154     ..%sOK......+OTA
2006289c:	4552 4f43 4556 3a52 0000 0000 0a0d 6120     RECOVER:...... a
200628ac:	6772 6c20 6e65 3d20 2520 2064 0a0d 0000     rg len = %d ....
200628bc:	0a0d 6120 6772 3d20 2520 2073 0a0d 0000     .. arg = %s ....
200628cc:	542b 5345 3a54 0000 522b 5453 003a 0000     +TEST:..+RST:...
200628dc:	6154 6b73 4c20 7369 3a74 0d20 250a 0d73     Task List: ..%s.
200628ec:	000a 0000 6548 7061 7453 7461 3a73 0d20     ....HeapStats: .
200628fc:	000a 0000 6f54 6174 206c 6548 7061 093a     ....Total Heap:.
2006290c:	7525 0a0d 0000 0000 6548 7061 4620 6572     %u......Heap Fre
2006291c:	2065 6f4e 3a77 2509 0d75 000a 6548 7061     e Now:.%u...Heap
2006292c:	5520 6573 2064 6f4e 3a77 2509 0d75 000a      Used Now:.%u...
2006293c:	6548 7061 5520 6573 2064 614d 3a78 2509     Heap Used Max:.%
2006294c:	0d75 000a 532b 4154 4554 003a 4d41 4245     u...+STATE:.AMEB
2006295c:	2d41 5452 534f 5320 4b44 5620 5245 4953     A-RTOS SDK VERSI
2006296c:	4e4f 203a 6425 252e 2e64 6425 000a 0000     ON: %d.%d.%d....
2006297c:	7325 0000 3056 2e32 2e31 0031 3376 352e     %s..V02.1.1.v3.5
2006298c:	0000 0000 3032 3432 302f 2f37 3532 312d     ....2024/07/25-1
2006299c:	3a36 3133 353a 0038 472b 524d 003a 0000     6:31:58.+GMR:...
200629ac:	0a0d 7325 7325 252c 2c73 7325 0a0d 0000     ..%s%s,%s,%s....
200629bc:	4c5b 474f 205d 5245 4f52 2052 7261 3a67     [LOG] ERROR arg:
200629cc:	0d20 000a 0023 0000 4c5b 474f 205d 6e49      ...#...[LOG] In
200629dc:	6176 696c 2064 6567 2074 6170 6172 656d     valid get parame
200629ec:	6574 7372 0d2e 000a 253c 3e73 6c20 7665     ters....<%s> lev
200629fc:	6c65 6920 2073 6425 0a0d 0000 4c5b 474f     el is %d....[LOG
20062a0c:	205d 6e49 6176 696c 2064 6573 2074 6170     ] Invalid set pa
20062a1c:	6172 656d 6574 7372 0d2e 000a 4c5b 474f     rameters....[LOG
20062a2c:	205d 6146 6c69 6465 7720 6568 206e 6573     ] Failed when se
20062a3c:	2e74 0a0d 0000 0000 4c5b 474f 205d 6e49     t.......[LOG] In
20062a4c:	6176 696c 2064 6170 6172 656d 6574 2072     valid parameter 
20062a5c:	756e 626d 7265 0d2e 000a 0000 4c5b 474f     number......[LOG
20062a6c:	205d 6e49 6176 696c 2064 6f6d 6564 2520     ] Invalid mode %
20062a7c:	2e64 0a0d 0000 0000 4c5b 474f 205d 6554     d.......[LOG] Te
20062a8c:	7473 6120 776c 7961 2073 656c 6576 0d6c     st always level.
20062a9c:	000a 0000 4c5b 474f 205d 6554 7473 6520     ....[LOG] Test e
20062aac:	7272 726f 6c20 7665 6c65 0a0d 0000 0000     rror level......
20062abc:	4c5b 474f 205d 6554 7473 7720 7261 696e     [LOG] Test warni
20062acc:	676e 6c20 7665 6c65 0a0d 0000 4c5b 474f     ng level....[LOG
20062adc:	205d 6554 7473 6920 666e 206f 656c 6576     ] Test info leve
20062aec:	0d6c 000a 4c5b 474f 205d 6554 7473 4c20     l...[LOG] Test L
20062afc:	474f 495f 4554 534d 0a0d 0000 4c2b 474f     OG_ITEMS....+LOG
20062b0c:	003a 0000 0a0d 7325 5245 4f52 3a52 6425     :.....%sERROR:%d
20062b1c:	0a0d 0000 5441 4c2b 474f 3c3d 6567 5f74     ....AT+LOG=<get_
20062b2c:	6573 3e74 3c2c 6f6d 7564 656c 5b3e 3c2c     set>,<module>[,<
20062b3c:	6f6c 5f67 656c 6576 3e6c 0d5d 000a 0000     log_level>].....
20062b4c:	3c09 6567 5f74 6573 3e74 093a 2d30 6567     .<get_set>:.0-ge
20062b5c:	2c74 3120 732d 7465 202c 2d32 7270 6e69     t, 1-set, 2-prin
20062b6c:	2074 6c61 2c6c 3320 632d 656c 7261 6120     t all, 3-clear a
20062b7c:	6c6c 0a0d 0000 0000 3c09 6f6d 7564 656c     ll.......<module
20062b8c:	3a3e 2a09 652d 6361 2068 6f6d 7564 656c     >:.*-each module
20062b9c:	202c 746f 6568 7372 732d 6570 6963 6966     , others-specifi
20062bac:	2063 6f6d 7564 656c 0a0d 0000 3c09 6f6c     c module.....<lo
20062bbc:	5f67 656c 6576 3e6c 093a 305b 352c 2c5d     g_level>:.[0,5],
20062bcc:	6f20 6c6e 2079 7061 6c70 6369 6261 656c      only applicable
20062bdc:	6620 726f 7320 7465 6d20 646f 0d65 000a      for set mode...
20062bec:	525b 4552 5d47 4520 7272 726f 7020 7261     [RREG] Error par
20062bfc:	6d61 7465 7265 0d73 000a 0000 522b 4552     ameters.....+RRE
20062c0c:	3a47 0000 575b 4552 5d47 4520 7272 726f     G:..[WREG] Error
20062c1c:	7020 7261 6d61 7465 7265 0d73 000a 0000      parameters.....
20062c2c:	572b 4552 3a47 0000 5043 2555 0000 0000     +WREG:..CPU%....
20062c3c:	7270 6f69 0000 0000 7473 7461 0065 0000     prio....state...
20062c4c:	6174 6b73 6e20 6d61 0065 0000 7325 2509     task name...%s.%
20062c5c:	0973 7325 2509 0a73 000d 0000 7325 2509     s.%s.%s.....%s.%
20062c6c:	0963 6425 2509 2e64 7325 0a0d 0000 0000     c.%d.%d.%s......
20062c7c:	7325 6d20 6c61 6f6c 2063 6166 6c69 6465     %s malloc failed
20062c8c:	0a0d 0000 745b 706f 555d 6173 6567 203a     ....[top]Usage: 
20062c9c:	6f74 3d70 6f6d 6564 742c 6d69 2c65 6f63     top=mode,time,co
20062cac:	6e75 0a74 200d 6f6d 6564 203a 2c30 7320     unt.. mode: 0, s
20062cbc:	6174 7472 6320 756f 746e 6320 7570 7520     tart count cpu u
20062ccc:	6173 6567 6520 6576 7972 5b20 6974 656d     sage every [time
20062cdc:	205d 6573 6f63 646e 0d2e 200a 6f6d 6564     ] second... mode
20062cec:	203a 2c31 7320 6f74 2070 6f6d 6564 3020     : 1, stop mode 0
20062cfc:	0d2e 200a 6f6d 6564 203a 3a32 7320 6174     ... mode: 2: sta
20062d0c:	7472 6320 756f 746e 6320 7570 7520 6173     rt count cpu usa
20062d1c:	6567 0d2e 200a 6f6d 6564 203a 3a33 7320     ge... mode: 3: s
20062d2c:	6f74 2070 6f6d 6564 3220 0d2e 200a 6974     top mode 2... ti
20062d3c:	656d 203a 5043 2055 7473 7461 7369 6974     me: CPU statisti
20062d4c:	7363 6920 746e 7265 6176 2e6c 6544 6166     cs interval.Defa
20062d5c:	6c75 2074 2e31 7520 696e 2874 2973 0d20     ult 1. unit(s) .
20062d6c:	200a 6f63 6e75 3a74 4320 5550 7320 6174     . count: CPU sta
20062d7c:	6974 7473 6369 2073 6f63 6e75 2c74 6420     tistics count, d
20062d8c:	6665 7561 746c 7520 746e 6c69 7320 6f74     efault until sto
20062d9c:	2070 726f 3120 322c 332c 2e2e 002e 0000     p or 1,2,3......
20062dac:	654d 6f6d 7972 6620 6961 756c 6572 6620     Memory failure f
20062dbc:	726f 7420 7361 5f6b 7473 7461 7375 0a0d     or task_status..
20062dcc:	0000 0000 7563 7272 6e65 2074 6f6d 6564     ....current mode
20062ddc:	203a 6425 7420 6d69 3a65 2520 2864 2973     : %d time: %d(s)
20062dec:	6320 756f 746e 203a 6425 2820 312d 6620      count: %d (-1 f
20062dfc:	726f 6920 666e 6e69 7469 2965 0a0d 0000     or infinite)....
20062e0c:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062e1c:	432b 5550 4f4c 4441 003a 0000 5441 432b     +CPULOAD:...AT+C
20062e2c:	5550 4f4c 4441 3c3d 6f6d 6564 5b3e 3c2c     PULOAD=<mode>[,<
20062e3c:	6974 656d 695f 746e 6176 3e6c 3c2c 6f63     time_intval>,<co
20062e4c:	6e75 6574 3e72 0d5d 000a 0000 3c09 6f6d     unter>]......<mo
20062e5c:	6564 3a3e 5b09 2c30 5d32 0a0d 0000 0000     de>:.[0,2]......
20062e6c:	3c09 6974 656d 695f 746e 6176 3e6c 093a     .<time_intval>:.
20062e7c:	6e49 7320 6365 0a0d 0000 0000 3c09 6f63     In sec.......<co
20062e8c:	6e75 6574 3e72 3009 6d20 6165 736e 6920     unter>.0 means i
20062e9c:	666e 6e69 7469 0a0d 0000 0000 5441 7325     nfinit......AT%s
20062eac:	0a0d 0000 6f43 6d6d 6e6f 4120 2054 6f43     ....Common AT Co
20062ebc:	6d6d 6e61 3a64 0a0d 0000 0000 4c2b 5349     mmand:......+LIS
20062ecc:	3a54 0000 4f2b 4154 4c43 4145 0052 0000     T:..+OTACLEAR...
20062edc:	4f2b 4154 4552 4f43 4556 0052 432b 5550     +OTARECOVER.+CPU
20062eec:	4f4c 4441 0000 0000 542b 5345 0054 0000     LOAD....+TEST...
20062efc:	4c2b 5349 0054 0000 522b 5453 0000 0000     +LIST...+RST....
20062f0c:	532b 4154 4554 0000 472b 524d 0000 0000     +STATE..+GMR....
20062f1c:	4c2b 474f 0000 0000 522b 4552 0047 0000     +LOG....+RREG...
20062f2c:	572b 4552 0047 0000                         +WREG...

20062f34 <__FUNCTION__.0>:
20062f34:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062f44:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20062f54:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20062f64:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20062f74:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20062f84:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20062f94:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20062fa4:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20062fb4:	6f70 656e 746e 6f2f 2f73 7266 6565 7472     ponent/os/freert
20062fc4:	736f 662f 6572 7265 6f74 5f73 6568 7061     os/freertos_heap
20062fd4:	5f35 6f63 666e 6769 632e 0000 255b 5d73     5_config.c..[%s]
20062fe4:	6c20 7665 6c65 3d20 2520 0a64 0000 0000      level = %d.....
20062ff4:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20063004:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20063014:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20063024:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20063034:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20063044:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20063054:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20063064:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
20063074:	696c 6574 732f 6c77 6269 6c2f 676f 632e     lite/swlib/log.c
20063084:	0000 0000 002a 0000 6143 6863 2065 7261     ....*...Cache ar
20063094:	6172 2079 7369 6620 6c75 2c6c 6120 646e     ray is full, and
200630a4:	7220 7065 616c 6563 6f20 646c 6520 746e      replace old ent
200630b4:	7972 000a 255b 3830 5d78 0020 0a0d 255b     ry..[%08x] ...[%
200630c4:	3830 5d78 0020 0000 3025 7838 0020 0000     08x] ...%08x ...
200630d4:	3025 7832 0020 0000 255b 2d73 6325 205d     %02x ...[%s-%c] 
200630e4:	0000 0000 5749 4744 7220 6665 6572 6873     ....IWDG refresh
200630f4:	6f20 216e 000a 0000 414d 4e49 0000 0000      on!....MAIN....
20063104:	4457 5f47 6954 656d 0072 0000 5749 4744     WDG_Timer...IWDG
20063114:	7220 6665 6572 6873 6520 7272 726f 000a      refresh error..
20063124:	524b 2034 534f 5320 4154 5452 0a20 0000     KR4 OS START ...
20063134:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
20063144:	202d 4554 5453 4320 4d4f 414d 444e 4d20     - TEST COMMAND M
20063154:	444f 2045 4548 504c 2520 2d64 2d2d 2d2d     ODE HELP %d-----
20063164:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 0a2d 0000     -------------...
20063174:	7325 000a 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     %s..------------
20063184:	2d2d 2d2d 202d 4554 5453 4320 4d4f 414d     ----- TEST COMMA
20063194:	444e 4d20 444f 2045 4e45 2044 2520 2d78     ND MODE END  %x-
200631a4:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
200631b4:	0a2d 0000 6552 6f62 746f 6e69 2067 2e2e     -...Rebooting ..
200631c4:	0a2e 000d 6175 7472 7562 6e72 0000 0000     ....uartburn....
200631d4:	255b 3830 5d78 2520 3830 2078 000a 0000     [%08x] %08x ....
200631e4:	7257 6e6f 2067 7261 7567 656d 746e 6e20     Wrong argument n
200631f4:	6d75 6562 2172 0a0d 0000 0000 5352 5049     umber!......RSIP
20063204:	6520 616e 6c62 6465 202c 6143 276e 2074      enabled, Can't 
20063214:	6552 6461 4620 616c 6873 0a2e 0000 0000     Read Flash......
20063224:	7257 6e6f 2067 6d63 2164 0a0d 0000 0000     Wrong cmd!......
20063234:	003f 0000 4809 4c45 2050 4028 293f 3a20     ?....HELP (@?) :
20063244:	0a20 0909 5020 6972 746e 6820 6c65 2070      ... Print help 
20063254:	656d 7373 6761 000a 5744 0000 4009 5744     messag..DW...@DW
20063264:	3c20 6441 7264 7365 2c73 4c20 6e65 7467      <Address, Lengt
20063274:	3e68 090a 2009 7544 706d 6d20 6d65 726f     h>... Dump memor
20063284:	2079 6f77 6472 6f20 2072 6552 6461 4820     y word or Read H
20063294:	2077 6f77 6472 7220 6765 7369 6574 3b72     w word register;
200632a4:	0a20 0909 4420 2057 613c 6464 3e72 0920      ... DW <addr> .
200632b4:	2009 7544 706d 6f20 6c6e 2079 6e6f 2065     . Dump only one 
200632c4:	6f77 6472 6120 2074 6874 2065 6173 656d     word at the same
200632d4:	7420 6d69 2c65 7520 696e 3a74 3420 7942      time, unit: 4By
200632e4:	6574 0a73 0909 4420 2057 613c 6464 3e72     tes... DW <addr>
200632f4:	3c20 656c 3e6e 0920 4420 6d75 2070 6874      <len> . Dump th
20063304:	2065 7073 6365 6669 6569 2064 656c 676e     e specified leng
20063314:	6874 6f20 2066 6874 2065 6f77 6472 202c     th of the word, 
20063324:	6e75 7469 203a 4234 7479 7365 090a 2009     unit: 4Bytes... 
20063334:	5744 3c20 6461 7264 203e 6c3c 6e65 203e     DW <addr> <len> 
20063344:	2062 4420 6d75 2070 6874 2065 7073 6365     b  Dump the spec
20063354:	6669 6569 2064 656c 676e 6874 6f20 2066     ified length of 
20063364:	6874 2065 7962 6574 202c 6e75 7469 203a     the byte, unit: 
20063374:	4231 7479 0a65 0000 5745 0000 4009 5745     1Byte...EW...@EW
20063384:	3c20 6441 7264 7365 2c73 4820 7865 0a3e      <Address, Hex>.
20063394:	0909 5720 6972 6574 6d20 6d65 726f 2079     .. Write memory 
200633a4:	7764 726f 2064 726f 5720 6972 6574 4820     dword or Write H
200633b4:	2077 7764 726f 2064 6572 6967 7473 7265     w dword register
200633c4:	0a20 0909 4320 6e61 7720 6972 6574 6f20      ... Can write o
200633d4:	6c6e 2079 6e6f 2065 7764 726f 2064 7461     nly one dword at
200633e4:	7420 6568 7320 6d61 2065 6974 656d 0a20      the same time .
200633f4:	0909 4520 3a78 4520 2057 6441 7264 7365     .. Ex: EW Addres
20063404:	2073 6156 756c 2065 000a 0000 4552 4f42     s Value ....REBO
20063414:	544f 0000 4009 4552 4f42 544f 0a20 0909     OT...@REBOOT ...
20063424:	7220 6265 6f6f 2074 090a 2009 6572 6f62      reboot ... rebo
20063434:	746f 7520 7261 6274 7275 206e 000a 0000     ot uartburn ....
20063444:	255b 5d73 2520 2073 3c3c 203c 6854 2065     [%s] %s <<< The 
20063454:	756d 6574 2078 6168 2073 6f6e 2074 6562     mutex has not be
20063464:	6e65 7220 6c65 6165 6573 2c64 6220 7475     en released, but
20063474:	7420 6568 6d20 7475 7865 6820 7361 6220      the mutex has b
20063484:	6565 206e 6564 656c 6574 2e64 3e20 3e3e     een deleted. >>>
20063494:	000a 0000                                   ....

20063498 <__FUNCTION__.0>:
20063498:	7472 736f 6d5f 7475 7865 645f 6c65 7465     rtos_mutex_delet
200634a8:	0065 0000 6f4e 7354 006b 0000 524b 0034     e...NoTsk...KR4.
200634b8:	614d 6c6c 636f 6620 6961 656c 2e64 4320     Malloc failed. C
200634c8:	726f 3a65 255b 5d73 202c 6154 6b73 5b3a     ore:[%s], Task:[
200634d8:	7325 2c5d 5b20 7266 6565 6820 6165 2070     %s], [free heap 
200634e8:	6973 657a 203a 6425 0d5d 000a 0d0a 255b     size: %d].....[%
200634f8:	5d73 5320 4154 4b43 4f20 4556 4652 4f4c     s] STACK OVERFLO
20063508:	2057 202d 6154 6b73 614e 656d 2528 2973     W - TaskName(%s)
20063518:	0d0a 0000                                   ....

2006351c <__FUNCTION__.5>:
2006351c:	4176 7070 696c 6163 6974 6e6f 7453 6361     vApplicationStac
2006352c:	4f6b 6576 6672 6f6c 4877 6f6f 006b 0000     kOverflowHook...
2006353c:	5441 3f53 0000 0000 0a0d 6e75 6e6b 776f     ATS?......unknow
2006354c:	206e 6f63 6d6d 6e61 2064 2527 2773 0000     n command '%s'..
2006355c:	4f4c 5547 5241 5f54 4154 4b53 0000 0000     LOGUART_TASK....
2006356c:	7243 6165 6574 4c20 676f 5520 5241 2054     Create Log UART 
2006357c:	6154 6b73 4520 7272 2121 000a 4853 4c45     Task Err!!..SHEL
2006358c:	004c 0000 255b 5d73 4920 474d 4449 203a     L...[%s] IMGID: 
2006359c:	6425 202c 7563 7272 6e65 2074 544f 2541     %d, current OTA%
200635ac:	2064 6441 7264 7365 3a73 3020 2578 3830     d Address: 0x%08
200635bc:	2c78 7420 7261 6567 2074 544f 2541 2064     x, target OTA%d 
200635cc:	6441 7264 7365 3a73 3020 2578 3830 0a78     Address: 0x%08x.
200635dc:	0000 0000 5953 0053 255b 5d73 4920 474d     ....SYS.[%s] IMG
200635ec:	4449 203a 6425 202c 7563 7272 6e65 2074     ID: %d, current 
200635fc:	6966 6d72 6177 6572 6920 2073 544f 2541     firmware is OTA%
2006360c:	2c64 7420 7261 6567 2074 6966 6d72 6177     d, target firmwa
2006361c:	6572 4f20 4154 6425 6920 2073 6e69 6176     re OTA%d is inva
2006362c:	696c 0a64 0000 0000 255b 5d73 6220 6361     lid.....[%s] bac
2006363c:	756b 2070 616d 6c6c 636f 6620 6961 646c     kup malloc faild
2006364c:	6465 000a                                   ed..

20063650 <__func__.0>:
20063650:	7973 5f73 6572 6f63 6576 5f72 746f 5f61     sys_recover_ota_
20063660:	6973 6e67 7461 7275 0065 0000               signature...

2006366c <__func__.1>:
2006366c:	7973 5f73 6c63 6165 5f72 746f 5f61 6973     sys_clear_ota_si
2006367c:	6e67 7461 7275 0065 4449 454c 0000 0000     gnature.IDLE....
2006368c:	2509 0963 6425 2509 0964 6425 0a0d 0000     .%c.%d.%d.%d....
2006369c:	6d54 2072 7653 0063 e896 2005 e896 2005     Tmr Svc.... ... 
200636ac:	e896 2005 e8d2 2005 e8de 2005 e8fa 2005     ... ... ... ... 
200636bc:	e896 2005 e896 2005 e8d2 2005 e8de 2005     ... ... ... ... 
200636cc:	7325 2528 2964 202c 7641 6961 616c 6c62     %s(%d), Availabl
200636dc:	2065 6568 7061 2520 0a64 0000 4c57 4e41     e heap %d...WLAN
200636ec:	0000 0000 6e69 7469 0000 0000 6c77 6e61     ....init....wlan
200636fc:	695f 696e 6974 6c61 7a69 2065 6166 6c69     _initialize fail
2006370c:	6465 000a                                   ed..

20063710 <__FUNCTION__.0>:
20063710:	695f 696e 5f74 6874 6572 6461 0000 0000     _init_thread....
20063720:	6f46 2072 6564 7562 2067 6170 6b63 6761     For debug packag
20063730:	2e65 5020 656c 7361 2065 6573 656c 7463     e. Please select
20063740:	7020 7273 6d61 7420 7079 2065 6361 6f63      psram type acco
20063750:	6472 6e69 2067 6f74 7920 756f 2072 6863     rding to your ch
20063760:	7069 000a 4843 5049 4e49 4f46 0000 0000     ip..CHIPINFO....
20063770:	3230 6620 726f 5720 2042 3233 624d 300a     02 for WB 32Mb.0
20063780:	2034 6f66 2072 4257 3120 3832 624d 300a     4 for WB 128Mb.0
20063790:	2033 6f66 2072 4257 3220 3635 624d 0a0a     3 for WB 256Mb..
200637a0:	0000 0000 682f 6d6f 2f65 7561 6f74 6a5f     ..../home/auto_j
200637b0:	6e65 696b 736e 775f 616c 356e 492f 544f     enkins_wlan5/IOT
200637c0:	415f 5354 722f 7065 736f 722f 6c65 6165     _ATS/repos/relea
200637d0:	6573 6d2d 7361 6574 2f72 6f73 7275 6563     se-master/source
200637e0:	612f 656d 6162 696c 6574 675f 6363 705f     /amebalite_gcc_p
200637f0:	6f72 656a 7463 702f 6f72 656a 7463 6b5f     roject/project_k
20063800:	3472 762f 6473 2f6b 2e2e 2e2f 2f2e 2e2e     r4/vsdk/../../..
20063810:	632f 6d6f 6f70 656e 746e 732f 636f 612f     /component/soc/a
20063820:	656d 6162 696c 6574 6c2f 6269 722f 6d61     mebalite/lib/ram
20063830:	635f 6d6f 6f6d 2f6e 6d61 6265 5f61 6863     _common/ameba_ch
20063840:	7069 6e69 6f66 6c5f 6269 632e 0000 0000     ipinfo_lib.c....
20063850:	7325 203a 0000 0000 5245 3a52 4720 7465     %s: ....ERR: Get
20063860:	5320 636f 4e20 6d61 2065 6146 6c69 000a      Soc Name Fail..
20063870:	3025 7832 0000 0000 4d4b 0034 7325 4c20     %02x....KM4.%s L
20063880:	6269 5620 7265 6973 6e6f 0a3a 0000 0000     ib Version:.....
20063890:	3032 3432 302f 2f37 3532 312d 3a36 3133     2024/07/25-16:31
200638a0:	343a 0033 4f43 504d 4c49 2045 4954 454d     :43.COMPILE TIME
200638b0:	203a 7325 000a 0000 6367 2063 6576 7372     : %s....gcc vers
200638c0:	6f69 206e 3031 332e 312e 3220 3230 3032     ion 10.3.1 20220
200638d0:	3335 2030 5228 6165 746c 6b65 5620 4453     530 (Realtek VSD
200638e0:	2d4b 3031 332e 312e 4220 6975 646c 3420     K-10.3.1 Build 4
200638f0:	3730 2b33 2029 0000 4f43 504d 4c49 5245     073+) ..COMPILER
20063900:	203a 7325 000a 0000 6325 0000 2d2d 2d2d     : %s....%c..----
20063910:	2d2d 2d2d 2d2d 2d2d 6154 6b73 4920 666e     --------Task Inf
20063920:	2d6f 2d2d 2d2d 2d2d 2d2d 2d2d 0d2d 000a     o------------...
20063930:	6146 6c75 2074 6e6f 7420 7361 206b 253c     Fault on task <%
20063940:	3e73 000a 6154 6b73 4920 3a44 2520 0d64     s>..Task ID: %d.
20063950:	000a 0000 6154 6b73 5420 4243 303a 2578     ....Task TCB:0x%
20063960:	3830 786c 0a0d 0000 7543 7272 6e65 2074     08lx....Current 
20063970:	7453 7461 3a65 2520 2064 2528 2973 0a0d     State: %d (%s)..
20063980:	0000 0000 6142 6573 5020 6972 726f 7469     ....Base Priorit
20063990:	3a79 2520 0d64 000a 7543 7272 6e65 2074     y: %d...Current 
200639a0:	7250 6f69 6972 7974 203a 6425 0a0d 0000     Priority: %d....
200639b0:	7552 206e 6954 656d 4320 756f 746e 7265     Run Time Counter
200639c0:	203a 6c25 0d75 000a 7453 6361 546b 706f     : %lu...StackTop
200639d0:	203a 7830 3025 6c38 2c78 5320 6174 6b63     : 0x%08lx, Stack
200639e0:	6142 6573 203a 7830 3025 6c38 2c78 5320     Base: 0x%08lx, S
200639f0:	6174 6b63 6e45 3a64 3020 2578 3830 786c     tackEnd: 0x%08lx
20063a00:	202c 7453 6361 536b 7a69 3d65 6c25 2875     , StackSize=%lu(
20063a10:	6f77 6472 0d29 000a 7453 6361 206b 6948     word)...Stack Hi
20063a20:	6867 5720 7461 7265 614d 6b72 203a 6425     gh WaterMark: %d
20063a30:	7728 726f 2964 0a0d 0000 0000 7552 6e6e     (word)......Runn
20063a40:	6e69 0067 6552 6461 2079 202f 6550 646e     ing.Ready / Pend
20063a50:	6e69 0067 6c42 636f 656b 0064 7553 7073     ing.Blocked.Susp
20063a60:	6e65 6564 0064 0000 6544 656c 6574 0064     ended...Deleted.
20063a70:	6e49 6176 696c 0064                         Invalid.

20063a78 <TaskStateString.0>:
20063a78:	3a3c 2006 3a44 2006 3a54 2006 3a5c 2006     <:. D:. T:. \:. 
20063a88:	3a68 2006 3a70 2006 f208 2005 f11c 2005     h:. p:. ... ... 
20063a98:	f11c 2005 f11c 2005 f11c 2005 f11c 2005     ... ... ... ... 
20063aa8:	f11c 2005 f11c 2005 f11c 2005 f11c 2005     ... ... ... ... 
20063ab8:	f152 2005 f2c4 2005 f192 2005 f11c 2005     R.. ... ... ... 
20063ac8:	f11c 2005 f11c 2005 f11c 2005 f192 2005     ... ... ... ... 
20063ad8:	f11c 2005 f11c 2005 f318 2005 f11c 2005     ... ... ... ... 
20063ae8:	f11c 2005 f178 2005 f22e 2005 f11c 2005     ... x.. ... ... 
20063af8:	f11c 2005 f24c 2005 f11c 2005 f1d4 2005     ... L.. ... ... 
20063b08:	f11c 2005 f11c 2005 f20a 2005 f11c 2005     ... ... ... ... 
20063b18:	f31a 2005 3130 3332 3534 3736 3938 4241     ... 0123456789AB
20063b28:	4443 4645 0000 0000 6f69 6420 6972 6576     CDEF....io drive
20063b38:	2072 6170 6172 656d 6574 7372 6520 7272     r parameters err
20063b48:	726f 2021 6966 656c 6e5f 6d61 3a65 2520     or! file_name: %
20063b58:	2c73 6c20 6e69 3a65 2520 0064 682f 6d6f     s, line: %d./hom
20063b68:	2f65 7561 6f74 6a5f 6e65 696b 736e 775f     e/auto_jenkins_w
20063b78:	616c 356e 492f 544f 415f 5354 722f 7065     lan5/IOT_ATS/rep
20063b88:	736f 722f 6c65 6165 6573 6d2d 7361 6574     os/release-maste
20063b98:	2f72 6f73 7275 6563 612f 656d 6162 696c     r/source/amebali
20063ba8:	6574 675f 6363 705f 6f72 656a 7463 702f     te_gcc_project/p
20063bb8:	6f72 656a 7463 6b5f 3472 762f 6473 2f6b     roject_kr4/vsdk/
20063bc8:	2e2e 2e2f 2f2e 2e2e 632f 6d6f 6f70 656e     ../../../compone
20063bd8:	746e 732f 636f 612f 656d 6162 696c 6574     nt/soc/amebalite
20063be8:	6c2f 6269 722f 6d61 635f 6d6f 6f6d 2f6e     /lib/ram_common/
20063bf8:	6d61 6265 5f61 6f72 5f6d 6170 6374 2e68     ameba_rom_patch.
20063c08:	0063 0000 7269 5b71 6425 205d 7270 6f69     c...irq[%d] prio
20063c18:	6972 7974 2520 2078 6873 6c61 206c 3d3c     rity %x shall <=
20063c28:	2520 0a64 0000 0000 5b1b 3b31 3333 5b6d      %d......[1;33m[
20063c38:	5245 4f52 5d52 5b20 7325 3a3a 6425 205d     ERROR] [%s::%d] 
20063c48:	7473 6361 5f6b 6973 657a 2528 2964 6920     stack_size(%d) i
20063c58:	2073 6f70 6973 6974 6576 3f20 0a3f 5b1b     s positive ??..[
20063c68:	6d30 0000 5b1b 3b31 3333 5b6d 5245 4f52     0m...[1;33m[ERRO
20063c78:	5d52 5b20 7325 3a3a 6425 205d 6172 6f5f     R] [%s::%d] ra_o
20063c88:	6666 6573 2874 6425 2029 7369 6e20 6765     ffset(%d) is neg
20063c98:	7461 7669 3f65 1b3f 305b 006d 5b1b 3b31     ative??.[0m..[1;
20063ca8:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063cb8:	6425 205d 7453 7261 2074 7473 6361 206b     %d] Start stack 
20063cc8:	6162 6b63 7274 6361 6e69 2067 6f66 2072     backtracing for 
20063cd8:	7073 2520 2c70 7020 2063 7025 1b0a 305b     sp %p, pc %p..[0
20063ce8:	006d 0000 5b1b 3b31 3333 5b6d 5254 4341     m....[1;33m[TRAC
20063cf8:	5d45 5b20 7325 3a3a 6425 205d 665b 6172     E] [%s::%d] [fra
20063d08:	656d 2320 6425 205d 7073 3e2d 2520 2c70     me #%d] sp-> %p,
20063d18:	7020 2d63 203e 7025 202c 7473 6361 5f6b      pc-> %p, stack_
20063d28:	6973 657a 3e2d 2520 2c64 7220 2d61 203e     size-> %d, ra-> 
20063d38:	7025 1b0a 305b 006d 5b1b 3b31 3333 5b6d     %p..[0m..[1;33m[
20063d48:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063d58:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20063d68:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20063d78:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20063d88:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063d98:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20063da8:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20063db8:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20063dc8:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20063dd8:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20063de8:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063df8:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
20063e08:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
20063e18:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
20063e28:	1b0a 305b 006d 0000                         ..[0m...

20063e30 <__FUNCTION__.3>:
20063e30:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063e40:	6932 0000                                   2i..

20063e44 <__FUNCTION__.2>:
20063e44:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063e54:	6332 0000                                   2c..

20063e58 <__FUNCTION__.1>:
20063e58:	6f64 735f 6174 6b63 625f 6361 746b 6172     do_stack_backtra
20063e68:	6563 0000 6e55 6e6b 776f 206e 7865 6563     ce..Unknown exce
20063e78:	7470 6f69 006e 0000 5b1b 3b31 3333 5b6d     ption....[1;33m[
20063e88:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063e98:	3d3d 3d3d 3d3d 3d3d 3d3d 4320 6172 6873     ========== Crash
20063ea8:	4420 6d75 2070 3d3d 3d3d 3d3d 3d3d 3d3d      Dump ==========
20063eb8:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063ec8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063ed8:	7845 6563 7470 6f69 206e 6163 6775 7468     Exception caught
20063ee8:	6f20 206e 7025 7720 7469 2068 6572 7361      on %p with reas
20063ef8:	6e6f 5b20 7830 7825 205d 3e2d 5b20 7325     on [0x%x] -> [%s
20063f08:	0a5d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     ]..[0m...[1;33m[
20063f18:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063f28:	3d3d 3d3d 3d3d 3d3d 3d3d 5220 6765 7369     ========== Regis
20063f38:	6574 2072 7544 706d 3d20 3d3d 3d3d 3d3d     ter Dump =======
20063f48:	3d3d 0a3d 5b1b 6d30 0000 0000 5b1b 3b31     ===..[0m.....[1;
20063f58:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063f68:	6425 205d 6d5b 6373 6172 6374 5d68 3020     %d] [mscratch] 0
20063f78:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063f88:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063f98:	6425 205d 6d5b 7065 5d63 2020 2020 3020     %d] [mepc]     0
20063fa8:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063fb8:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063fc8:	6425 205d 6d5b 6163 7375 5d65 2020 3020     %d] [mcause]   0
20063fd8:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063fe8:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063ff8:	6425 205d 6d5b 7674 6c61 205d 2020 3020     %d] [mtval]    0
20064008:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20064018:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20064028:	6425 205d 7845 6563 7470 6f69 206e 6163     %d] Exception ca
20064038:	7375 2065 6f63 6c75 2064 6562 0d3a 1b0a     use could be:...
20064048:	305b 006d 5b1b 3b31 3333 5b6d 5254 4341     [0m..[1;33m[TRAC
20064058:	5d45 5b20 7325 3a3a 6425 205d 2509 2c73     E] [%s::%d] .%s,
20064068:	2520 2073 203d 7830 7825 0a0d 5b1b 6d30      %s = 0x%x...[0m
20064078:	0000 0000 5b1b 3b31 3333 5b6d 5254 4341     .....[1;33m[TRAC
20064088:	5d45 5b20 7325 3a3a 6425 205d 2509 0d73     E] [%s::%d] .%s.
20064098:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
200640a8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200640b8:	785b 6425 2d20 203e 7325 205d 7830 3025     [x%d -> %s] 0x%0
200640c8:	7838 1b0a 305b 006d 5b1b 3b31 3333 5b6d     8x..[0m..[1;33m[
200640d8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200640e8:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
200640f8:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20064108:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20064118:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20064128:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20064138:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20064148:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20064158:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20064168:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20064178:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20064188:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
20064198:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
200641a8:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
200641b8:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
200641c8:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200641d8:	3d3d 3d3d 3d3d 3d3d 3d3d 4520 646e 6f20     ========== End o
200641e8:	2066 7243 7361 2068 7544 706d 3d20 3d3d     f Crash Dump ===
200641f8:	3d3d 3d3d 3d3d 0a3d 5b1b 6d30 0000 0000     =======..[0m....
20064208:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20064218:	7325 3a3a 6425 205d 0a0d 0a0d 5b1b 6d30     %s::%d] .....[0m
20064228:	0000 0000 6f53 6572 412f 4f4d 7020 6761     ....Sore/AMO pag
20064238:	2065 6177 6b6c 6320 6568 6b63 6620 6961     e walk check fai
20064248:	006c 0000 7453 726f 2065 6461 7264 7365     l...Store addres
20064258:	0073 0000 6f4c 6461 7020 6761 2065 6177     s...Load page wa
20064268:	6b6c 6320 6568 6b63 6620 6961 006c 0000     lk check fail...
20064278:	6f4c 6461 6120 6464 6572 7373 0000 0000     Load address....
20064288:	6e49 7473 7572 7463 6f69 206e 6170 6567     Instruction page
20064298:	7720 6c61 206b 6863 6365 206b 6166 6c69      walk check fail
200642a8:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
200642b8:	6461 7264 7365 0073 7253 726f 2f65 4d41     address.Srore/AM
200642c8:	204f 4d50 2050 6863 6365 206b 6166 6c69     O PMP check fail
200642d8:	0000 0000 7453 726f 2f65 4d41 204f 6170     ....Store/AMO pa
200642e8:	6567 7720 6c61 206b 7562 2073 7265 6f72     ge walk bus erro
200642f8:	0072 0000 7453 726f 2065 7562 2073 7265     r...Store bus er
20064308:	6f72 0072 7453 726f 2f65 4d41 204f 6461     ror.Store/AMO ad
20064318:	7264 7365 2073 696d 6173 696c 6e67 6465     dress misaligned
20064328:	0000 0000 6f4c 6461 5020 504d 6320 6568     ....Load PMP che
20064338:	6b63 6620 6961 006c 6f4c 6461 5020 6761     ck fail.Load Pag
20064348:	2065 6177 6b6c 6220 7375 6520 7272 726f     e walk bus error
20064358:	0000 0000 6f4c 6461 6220 7375 6520 7272     ....Load bus err
20064368:	726f 0000 6f4c 6461 6120 6464 6572 7373     or..Load address
20064378:	6d20 7369 6c61 6769 656e 0064 6c49 656c      misaligned.Ille
20064388:	6167 206c 6e69 7473 7572 7463 6f69 006e     gal instruction.
20064398:	6e49 7473 7572 7463 6f69 206e 706f 6f63     Instruction opco
200643a8:	6564 0000 6e49 7473 7572 7463 6f69 206e     de..Instruction 
200643b8:	4d50 2050 6863 6365 206b 6166 6c69 0000     PMP check fail..
200643c8:	6e49 7473 7572 7463 6f69 206e 6150 6567     Instruction Page
200643d8:	7720 6c61 206b 7562 2073 7265 6f72 0072      walk bus error.
200643e8:	6e49 7473 7572 7463 6f69 206e 7562 2073     Instruction bus 
200643f8:	7265 6f72 0072 0000 7242 6e61 6863 7420     error...Branch t
20064408:	7261 6567 2074 696d 6173 696c 6e67 6465     arget misaligned
20064418:	0000 0000 6154 6772 7465 6120 6464 6572     ....Target addre
20064428:	7373 0000 6e49 7473 7572 7463 6f69 206e     ss..Instruction 
20064438:	6461 7264 7365 2073 696d 6173 696c 6e67     address misalign
20064448:	6465 0000 657a 6f72 0000 0000 6172 0000     ed..zero....ra..
20064458:	7073 0000 7067 0000 7074 0000 3074 0000     sp..gp..tp..t0..
20064468:	3174 0000 3274 0000 3073 662f 0070 0000     t1..t2..s0/fp...
20064478:	3173 0000 3061 0000 3161 0000 3261 0000     s1..a0..a1..a2..
20064488:	3361 0000 3461 0000 3561 0000 3661 0000     a3..a4..a5..a6..
20064498:	3761 0000 3273 0000 3373 0000 3473 0000     a7..s2..s3..s4..
200644a8:	3573 0000 3673 0000 3773 0000 3873 0000     s5..s6..s7..s8..
200644b8:	3973 0000 3173 0030 3173 0031 3374 0000     s9..s10.s11.t3..
200644c8:	3474 0000 3574 0000 3674 0000 6e49 7473     t4..t5..t6..Inst
200644d8:	7572 7463 6f69 206e 6361 6563 7373 6620     ruction access f
200644e8:	7561 746c 0000 0000 7242 6165 706b 696f     ault....Breakpoi
200644f8:	746e 0000 6f4c 6461 6120 6464 7365 2073     nt..Load addess 
20064508:	696d 6173 696c 6e67 6465 0000 6f4c 6461     misaligned..Load
20064518:	6120 6363 7365 2073 6166 6c75 0074 0000      access fault...
20064528:	7453 726f 2065 6461 7264 7365 2073 696d     Store address mi
20064538:	6173 696c 6e67 6465 0000 0000 7453 726f     saligned....Stor
20064548:	2065 6461 7264 7365 2073 6166 6c75 0074     e address fault.
20064558:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
20064568:	6620 6f72 206d 2055 6f6d 6564 0000 0000      from U mode....
20064578:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
20064588:	6620 6f72 206d 2053 6f6d 6564 0000 0000      from S mode....
20064598:	6552 6573 7672 6465 6520 6378 7065 6974     Reserved excepti
200645a8:	6e6f 0000 6e45 6976 6f72 6d6e 6e65 2074     on..Environment 
200645b8:	6163 6c6c 6620 6f72 206d 204d 6f6d 6564     call from M mode
200645c8:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
200645d8:	6170 6567 6620 7561 746c 0000 6f4c 6461     page fault..Load
200645e8:	7020 6761 2065 6166 6c75 0074 7453 726f      page fault.Stor
200645f8:	2065 6170 6567 6620 7561 746c 0000 0000     e page fault....

20064608 <__FUNCTION__.0>:
20064608:	7263 7361 5f68 7564 706d 0000               crash_dump..

20064614 <exception_cause_0_string>:
20064614:	4400 2006 441c 2006 442c 2006 42ac 2006     .D. .D. ,D. .B. 
	...

2006462c <exception_cause_12_string>:
2006462c:	4288 2006 42ac 2006 0000 0000 0000 0000     .B. .B. ........

2006463c <exception_cause_13_string>:
2006463c:	425c 2006 4278 2006 0000 0000 0000 0000     \B. xB. ........

2006464c <exception_cause_15_string>:
2006464c:	422c 2006 424c 2006 0000 0000 0000 0000     ,B. LB. ........

2006465c <exception_cause_1_string>:
2006465c:	43ac 2006 42ac 2006 43c8 2006 42ac 2006     .C. .B. .C. .B. 
2006466c:	43e8 2006 42ac 2006 0000 0000 0000 0000     .C. .B. ........

2006467c <exception_cause_2_string>:
2006467c:	4384 2006 4398 2006 0000 0000 0000 0000     .C. .C. ........

2006468c <exception_cause_4_string>:
2006468c:	436c 2006 4278 2006 0000 0000 0000 0000     lC. xB. ........

2006469c <exception_cause_5_string>:
2006469c:	432c 2006 4278 2006 4340 2006 4278 2006     ,C. xB. @C. xB. 
200646ac:	435c 2006 0000 0000 0000 0000 0000 0000     \C. ............

200646bc <exception_cause_6_string>:
200646bc:	430c 2006 424c 2006 0000 0000 0000 0000     .C. LB. ........

200646cc <exception_cause_7_string>:
200646cc:	42c0 2006 424c 2006 42dc 2006 424c 2006     .B. LB. .B. LB. 
200646dc:	42fc 2006 0000 0000 0000 0000 0000 0000     .B. ............

200646ec <exception_cause_string_tbl>:
200646ec:	4614 2006 465c 2006 467c 2006 0000 0000     .F. \F. |F. ....
200646fc:	468c 2006 469c 2006 46bc 2006 46cc 2006     .F. .F. .F. .F. 
	...
2006471c:	462c 2006 463c 2006 0000 0000 464c 2006     ,F. <F. ....LF. 

2006472c <exception_code_string>:
2006472c:	442c 2006 44d4 2006 4384 2006 44f0 2006     ,D. .D. .C. .D. 
2006473c:	44fc 2006 4514 2006 4528 2006 4544 2006     .D. .E. (E. DE. 
2006474c:	4558 2006 4578 2006 4598 2006 45ac 2006     XE. xE. .E. .E. 
2006475c:	45cc 2006 45e4 2006 0000 0000 45f4 2006     .E. .E. .....E. 

2006476c <register_string>:
2006476c:	444c 2006 4454 2006 4458 2006 445c 2006     LD. TD. XD. \D. 
2006477c:	4460 2006 4464 2006 4468 2006 446c 2006     `D. dD. hD. lD. 
2006478c:	4470 2006 4478 2006 447c 2006 4480 2006     pD. xD. |D. .D. 
2006479c:	4484 2006 4488 2006 448c 2006 4490 2006     .D. .D. .D. .D. 
200647ac:	4494 2006 4498 2006 449c 2006 44a0 2006     .D. .D. .D. .D. 
200647bc:	44a4 2006 44a8 2006 44ac 2006 44b0 2006     .D. .D. .D. .D. 
200647cc:	44b4 2006 44b8 2006 44bc 2006 44c0 2006     .D. .D. .D. .D. 
200647dc:	44c4 2006 44c8 2006 44cc 2006 44d0 2006     .D. .D. .D. .D. 
200647ec:	4544 2056 5049 2043 5041 2049 736d 2067     DEV IPC API msg 
200647fc:	554e 4c4c 000a 0000 6f48 7473 4120 4950     NULL....Host API
2006480c:	5520 6b6e 6f6e 6e77 6520 7476 2528 2978      Unknown evt(%x)
2006481c:	000a 0000 0494 2006 04a2 2006 055a 2006     ....... ... Z.. 
2006482c:	04fa 2006 0672 2006 0686 2006 06fc 2006     ... r.. ... ... 
2006483c:	04fa 2006 04fa 2006 616c 7473 6920 696e     ... ... last ini
2006484c:	2063 7069 2063 6f6e 2074 6468 206c 000a     c ipc not hdl ..
2006485c:	7348 4d74 6773 6553 646e 7720 6961 2074     HstMsgSend wait 
2006486c:	6e69 6369 6920 6370 6420 6e6f 2065 7830     inic ipc done 0x
2006487c:	7825 202c 7830 7825 000a 0000 6e69 6369     %x, 0x%x....inic
2006488c:	615f 6970 685f 736f 5f74 6174 6b73 0000     _api_host_task..
2006489c:	7243 6165 6574 6120 6970 685f 736f 5f74     Create api_host_
200648ac:	6174 6b73 4520 7272 000a 0000 4e49 4349     task Err....INIC
200648bc:	0000 0000 6e69 6369 6d5f 5f70 6f63 6d6d     ....inic_mp_comm
200648cc:	6e61 2064 6166 6c69 000a 0000 7250 7669     and fail....Priv
200648dc:	7461 2065 654d 7373 6761 3a65 2520 0a73     ate Message: %s.
200648ec:	0000 0000 6f68 7473 735f 626b 785f 2078     ....host_skb_xx 
200648fc:	6f6e 2074 7266 6565 000a 0000 7325 3e3d     not free....%s=>
2006490c:	6b73 2062 616d 6c6c 636f 6620 6961 216c     skb malloc fail!
2006491c:	0d0a 0000 6f48 7473 5520 6b6e 6f6e 6e77     ....Host Unknown
2006492c:	6520 6576 746e 2528 786c 2129 0d0a 0000      event(%lx)!....

2006493c <__func__.1>:
2006493c:	6e69 6369 685f 736f 5f74 6e69 7469 735f     inic_host_init_s
2006494c:	626b 0000 6e69 6369 6d5f 6773 715f 745f     kb..inic_msg_q_t
2006495c:	7361 006b 7243 6165 6574 6d20 6773 715f     ask.Create msg_q
2006496c:	745f 7361 206b 7245 2172 000a 4f4e 6220     _task Err!..NO b
2006497c:	6675 6620 726f 6e20 7765 6e20 646f 7365     uf for new nodes
2006498c:	0a21 0000 6146 6c69 7420 206f 6966 646e     !...Fail to find
2006499c:	7420 6568 7420 6d69 7265 655f 746e 7972      the timer_entry
200649ac:	6920 206e 6974 656d 2072 6174 6c62 2e65      in timer table.
200649bc:	000a 0000 4954 454d 0052 0000 6146 6c69     ....TIMER...Fail
200649cc:	7420 206f 6e69 7469 7420 6d69 7265 0a2e      to init timer..
200649dc:	0000 0000 6f4d 5464 6d69 7265 203a 6f6e     ....ModTimer: no
200649ec:	2074 6e69 7469 0a2e 0000 0000 6f4d 5464     t init......ModT
200649fc:	6d69 7265 6620 6961 0a6c 0000 6143 636e     imer fail...Canc
20064a0c:	6c65 6954 656d 2072 6146 6c69 2528 2978     elTimer Fail(%x)
20064a1c:	000a 0000 6544 546c 6d69 7265 4620 6961     ....DelTimer Fai
20064a2c:	0a6c 0000                                   l...

20064a30 <__clz_tab>:
20064a30:	0100 0202 0303 0303 0404 0404 0404 0404     ................
20064a40:	0505 0505 0505 0505 0505 0505 0505 0505     ................
20064a50:	0606 0606 0606 0606 0606 0606 0606 0606     ................
20064a60:	0606 0606 0606 0606 0606 0606 0606 0606     ................
20064a70:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064a80:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064a90:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064aa0:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064ab0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064ac0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064ad0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064ae0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064af0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b00:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b10:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b20:	0808 0808 0808 0808 0808 0808 0808 0808     ................

20064b30 <_ctype_>:
20064b30:	2000 2020 2020 2020 2020 2828 2828 2028     .         ((((( 
20064b40:	2020 2020 2020 2020 2020 2020 2020 2020                     
20064b50:	8820 1010 1010 1010 1010 1010 1010 1010      ...............
20064b60:	0410 0404 0404 0404 0404 1004 1010 1010     ................
20064b70:	1010 4141 4141 4141 0101 0101 0101 0101     ..AAAAAA........
20064b80:	0101 0101 0101 0101 0101 0101 1010 1010     ................
20064b90:	1010 4242 4242 4242 0202 0202 0202 0202     ..BBBBBB........
20064ba0:	0202 0202 0202 0202 0202 0202 1010 1010     ................
20064bb0:	0020 0000 0000 0000 0000 0000 0000 0000      ...............
	...

20064c34 <shell_cmd_table>:
20064c34:	3234 2006 0000 0000 b39e 2005 3238 2006     42. ....... 82. 
20064c44:	325c 2006 0002 0000 b4fa 2005 3260 2006     \2. ....... `2. 
20064c54:	337c 2006 0002 0000 b454 2005 3380 2006     |3. ....T.. .3. 
20064c64:	3410 2006 0004 0000 b40a 2005 3418 2006     .4. ....... .4. 

20064c74 <ipc_tickless_table>:
20064c74:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064c84:	0000 0000 0010 0000 0000 0000               ............

20064c90 <ipc_kr4cdsp_table>:
20064c90:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064ca0:	0000 0000 0020 0000 0000 0000               .... .......

20064cac <ipc_KM4WKR4_table>:
20064cac:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064cbc:	0000 0000 0010 0000 0002 0000               ............

20064cc8 <ipc_shell_table>:
20064cc8:	0001 0000 cf26 2005 0000 0000 8eac 2005     ....&.. ....... 
20064cd8:	0000 0000 0010 0000 0003 0000               ............

20064ce4 <ipc_api_host_table>:
20064ce4:	0000 0000 07d2 2006 0000 0000 8eac 2005     ....... ....... 
20064cf4:	0000 0000 0010 0000 0007 0000               ............

20064d00 <ipc_host_event_table>:
20064d00:	0000 0000 0d8e 2006 0000 0000 8eac 2005     ....... ....... 
20064d10:	0000 0000 0010 0000 0006 0000               ............

20064d1c <lib_pmc_git_rev>:
20064d1c:	6c20 6269 705f 636d 765f 7265 645f 3339      lib_pmc_ver_d93
20064d2c:	6363 3136 3439 5f32 3032 3432 302f 2f37     cc61942_2024/07/
20064d3c:	3532 312d 3a36 3133 353a 0031               25-16:31:51.

20064d48 <lib_wifi_cast_git_rev>:
20064d48:	6c20 6269 775f 6669 5f69 6163 7473 765f      lib_wifi_cast_v
20064d58:	7265 645f 3339 6363 3136 3439 5f32 3032     er_d93cc61942_20
20064d68:	3432 302f 2f37 3532 312d 3a36 3133 353a     24/07/25-16:31:5
20064d78:	0031 0000                                   1...

20064d7c <lib_wifi_com_sec_git_rev>:
20064d7c:	6c20 6269 775f 6669 5f69 6f63 5f6d 6573      lib_wifi_com_se
20064d8c:	5f63 6576 5f72 3964 6333 3663 3931 3234     c_ver_d93cc61942
20064d9c:	325f 3230 2f34 3730 322f 2d35 3631 333a     _2024/07/25-16:3
20064dac:	3a31 3834                                    1:48.

20064db1 <__git_ver_table_end__>:
20064db1:	0000                	c.unimp
	...

20064db4 <__frame_dummy_init_array_entry>:
20064db4:	1474 2006                                   t.. 

20064db8 <__do_global_dtors_aux_fini_array_entry>:
20064db8:	143a 2006                                   :.. 

20064dbc <__fini_array_end>:
20064dbc:	0000                	c.unimp
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
200661ba:	ff050513          	addi	a0,a0,-16 # 20061ff0 <strtol+0xe>
200661be:	ed6f90ef          	jal	ra,2005f894 <io_assert_failed>
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
200663a8:	ff050513          	addi	a0,a0,-16 # 20061ff0 <strtol+0xe>
200663ac:	ce8f90ef          	jal	ra,2005f894 <io_assert_failed>
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
2006653e:	4e068693          	addi	a3,a3,1248 # 200624e0 <pmap_func+0xd4>
20066542:	05700613          	addi	a2,zero,87
20066546:	50458593          	addi	a1,a1,1284 # 20062504 <pmap_func+0xf8>
2006654a:	450d                	c.li	a0,3
2006654c:	c2bf40ef          	jal	ra,2005b176 <rtk_log_write>
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
20066624:	2ed0 2006 9f1e 2005 0000 0000 0000 0000     ... ... ........
20066634:	2edc 2006 9f3e 2005 0000 0000 0000 0000     ... >.. ........
20066644:	2ee8 2006 a780 2005 0000 0000 0000 0000     ... ... ........
20066654:	2ef4 2006 9f5e 2005 0000 0000 0000 0000     ... ^.. ........
20066664:	2efc 2006 a9e8 2005 0000 0000 0000 0000     ... ... ........
20066674:	2f04 2006 9fb8 2005 0000 0000 0000 0000     ./. ... ........
20066684:	2f0c 2006 9fd8 2005 0000 0000 0000 0000     ./. ... ........
20066694:	2f14 2006 a072 2005 0000 0000 0000 0000     ./. r.. ........
200666a4:	2f1c 2006 a124 2005 0000 0000 0000 0000     ./. $.. ........
200666b4:	2f24 2006 a312 2005 0000 0000 0000 0000     $/. ... ........
200666c4:	2f2c 2006 a398 2005 0000 0000 0000 0000     ,/. ... ........

200666d4 <impure_data>:
200666d4:	0000 0000 89c0 2006 8a28 2006 8a90 2006     ....... (.. ... 
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
20066800:	aa12 2005                                   ... 

20066804 <rtk_log_default_level>:
20066804:	0004 0000                                   ....

20066808 <mbedtls_calloc_func>:
20066808:	be42 2005                                   B.. 

2006680c <mbedtls_free_func>:
2006680c:	be46 2005                                   F.. 

20066810 <pullNextTime>:
20066810:	68d0 2006                                   .h. 

20066814 <uxTopUsedPriority>:
20066814:	000a 0000                                   ....

20066818 <crash_task_info>:
20066818:	ed44 2005                                   D.. 

2006681c <_impure_ptr>:
2006681c:	66d4 2006                                   .f. 
