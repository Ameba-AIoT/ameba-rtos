
/home/auto_jenkins_wlan5/IOT_ATS/repos/release-master/source/amebalite_gcc_project/project_kr4/vsdk/image_mp/target_img2.axf:     file format elf32-littleriscv


Disassembly of section .xip_image2.text:

20058060 <BOOT_InitGP>:
20058060:	0000f197          	auipc	gp,0xf
20058064:	84418193          	addi	gp,gp,-1980 # 200668a4 <__global_pointer$>
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
20058a08:	58b0106f          	jal	zero,2005a792 <BOOT_Image1>

20058a0c <app_start>:
20058a0c:	1141                	c.addi	sp,-16
20058a0e:	c606                	c.swsp	ra,12(sp)
20058a10:	e50ff0ef          	jal	ra,20058060 <BOOT_InitGP>
20058a14:	00018793          	addi	a5,gp,0 # 200668a4 <__global_pointer$>
20058a18:	e9c18613          	addi	a2,gp,-356 # 20066740 <PrevIrqStatus>
20058a1c:	40c78633          	sub	a2,a5,a2
20058a20:	4581                	c.li	a1,0
20058a22:	e9c18513          	addi	a0,gp,-356 # 20066740 <PrevIrqStatus>
20058a26:	dffb1097          	auipc	ra,0xdffb1
20058a2a:	d64080e7          	jalr	ra,-668(ra) # 978a <_memset>
20058a2e:	200697b7          	lui	a5,0x20069
20058a32:	01c18613          	addi	a2,gp,28 # 200668c0 <ipc_Semaphore>
20058a36:	98478793          	addi	a5,a5,-1660 # 20068984 <__bss_end__>
20058a3a:	40c78633          	sub	a2,a5,a2
20058a3e:	4581                	c.li	a1,0
20058a40:	01c18513          	addi	a0,gp,28 # 200668c0 <ipc_Semaphore>
20058a44:	dffb1097          	auipc	ra,0xdffb1
20058a48:	d46080e7          	jalr	ra,-698(ra) # 978a <_memset>
20058a4c:	170060ef          	jal	ra,2005ebbc <ChipInfo_GetChipSram>
20058a50:	c919                	c.beqz	a0,20058a66 <app_start+0x5a>
20058a52:	000405b7          	lui	a1,0x40
20058a56:	20080537          	lui	a0,0x20080
20058a5a:	00058593          	addi	a1,a1,0 # 40000 <__ap_sram_heap_size>
20058a5e:	00050513          	addi	a0,a0,0 # 20080000 <__ap_sram_heap_start>
20058a62:	669010ef          	jal	ra,2005a8ca <os_heap_add>
20058a66:	43b020ef          	jal	ra,2005b6a0 <rtos_mem_init>
20058a6a:	2005c7b7          	lui	a5,0x2005c
20058a6e:	e3078793          	addi	a5,a5,-464 # 2005be30 <freertos_risc_v_trap_handler>
20058a72:	9bf1                	c.andi	a5,-4
20058a74:	30579073          	csrrw	zero,mtvec,a5
20058a78:	2831                	c.jal	20058a94 <SystemCoreClockUpdate>
20058a7a:	dffae097          	auipc	ra,0xdffae
20058a7e:	24c080e7          	jalr	ra,588(ra) # 6cc6 <SYSTIMER_Init>
20058a82:	2db000ef          	jal	ra,2005955c <pinmap_init>
20058a86:	088090ef          	jal	ra,20061b0e <__libc_init_array>
20058a8a:	40b2                	c.lwsp	ra,12(sp)
20058a8c:	0141                	c.addi	sp,16
20058a8e:	0590206f          	jal	zero,2005b2e6 <main>

20058a92 <_init>:
20058a92:	8082                	c.jr	ra

20058a94 <SystemCoreClockUpdate>:
20058a94:	1141                	c.addi	sp,-16
20058a96:	c606                	c.swsp	ra,12(sp)
20058a98:	dffa8097          	auipc	ra,0xdffa8
20058a9c:	07e080e7          	jalr	ra,126(ra) # b16 <CPU_ClkGet>
20058aa0:	40b2                	c.lwsp	ra,12(sp)
20058aa2:	e6a1a623          	sw	a0,-404(gp) # 20066710 <SystemCoreClock>
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
20058ae8:	18b0d0ef          	jal	ra,20066472 <plic_get_active_irq_id>
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
20058b04:	c1c18693          	addi	a3,gp,-996 # 200664c0 <Flash_Layout>
20058b08:	4701                	c.li	a4,0
20058b0a:	c1c18793          	addi	a5,gp,-996 # 200664c0 <Flash_Layout>
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
20058b4a:	e8a1ae23          	sw	a0,-356(gp) # 20066740 <PrevIrqStatus>
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
20058b6a:	e9c1a503          	lw	a0,-356(gp) # 20066740 <PrevIrqStatus>
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
20058b8c:	f7450513          	addi	a0,a0,-140 # 20061f74 <strtol+0xc>
20058b90:	497060ef          	jal	ra,2005f826 <io_assert_failed>
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
20058c00:	4b20d0ef          	jal	ra,200660b2 <FLASH_TxData>
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
20058cb0:	5fc0d0ef          	jal	ra,200662ac <FLASH_Erase>
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
20058d84:	bbcd8793          	addi	a5,s11,-1092 # 20064bbc <ipc_tickless_table>
20058d88:	c6448493          	addi	s1,s1,-924 # 20064c64 <lib_pmc_git_rev>
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
20058db0:	bbcd8d93          	addi	s11,s11,-1092
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
20058e56:	01cb8693          	addi	a3,s7,28 # 2006201c <strtol+0xb4>
20058e5a:	04900613          	addi	a2,zero,73
20058e5e:	8319                	c.srli	a4,0x6
20058e60:	8b0d                	c.andi	a4,3
20058e62:	064c0593          	addi	a1,s8,100 # 20062064 <strtol+0xfc>
20058e66:	4511                	c.li	a0,4
20058e68:	084020ef          	jal	ra,2005aeec <rtk_log_write>
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
20058ecc:	01c18613          	addi	a2,gp,28 # 200668c0 <ipc_Semaphore>
20058ed0:	9432                	c.add	s0,a2
20058ed2:	4008                	c.lw	a0,0(s0)
20058ed4:	c511                	c.beqz	a0,20058ee0 <IPC_TXHandler+0x34>
20058ed6:	4422                	c.lwsp	s0,8(sp)
20058ed8:	40b2                	c.lwsp	ra,12(sp)
20058eda:	0141                	c.addi	sp,16
20058edc:	2430206f          	jal	zero,2005b91e <rtos_sema_give>
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
20058f0c:	06850513          	addi	a0,a0,104 # 20062068 <strtol+0x100>
20058f10:	c606                	c.swsp	ra,12(sp)
20058f12:	40e060ef          	jal	ra,2005f320 <DiagPrintf_minimal>
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
20058f38:	0dc18713          	addi	a4,gp,220 # 20066980 <IPC_IrqHandler>
20058f3c:	974e                	c.add	a4,s3
20058f3e:	431c                	c.lw	a5,0(a4)
20058f40:	c79d                	c.beqz	a5,20058f6e <IPC_wait_idle+0x50>
20058f42:	01c18513          	addi	a0,gp,28 # 200668c0 <ipc_Semaphore>
20058f46:	954e                	c.add	a0,s3
20058f48:	4118                	c.lw	a4,0(a0)
20058f4a:	01c18413          	addi	s0,gp,28 # 200668c0 <ipc_Semaphore>
20058f4e:	e319                	c.bnez	a4,20058f54 <IPC_wait_idle+0x36>
20058f50:	143020ef          	jal	ra,2005b892 <rtos_sema_create_binary>
20058f54:	0ff97a13          	andi	s4,s2,255
20058f58:	85d2                	c.mv	a1,s4
20058f5a:	8526                	c.mv	a0,s1
20058f5c:	4605                	c.li	a2,1
20058f5e:	2401                	c.jal	2005915e <IPC_INTConfig>
20058f60:	013407b3          	add	a5,s0,s3
20058f64:	4388                	c.lw	a0,0(a5)
20058f66:	55fd                	c.li	a1,-1
20058f68:	16b020ef          	jal	ra,2005b8d2 <rtos_sema_take>
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
20058f86:	08050513          	addi	a0,a0,128 # 20062080 <strtol+0x118>
20058f8a:	396060ef          	jal	ra,2005f320 <DiagPrintf_minimal>
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
20058fd6:	0a050513          	addi	a0,a0,160 # 200620a0 <strtol+0x138>
20058fda:	04d060ef          	jal	ra,2005f826 <io_assert_failed>
20058fde:	479d                	c.li	a5,7
20058fe0:	0157fa63          	bgeu	a5,s5,20058ff4 <ipc_send_message+0x4c>
20058fe4:	20062537          	lui	a0,0x20062
20058fe8:	0ad00593          	addi	a1,zero,173
20058fec:	0a050513          	addi	a0,a0,160 # 200620a0 <strtol+0x138>
20058ff0:	037060ef          	jal	ra,2005f826 <io_assert_failed>
20058ff4:	02100793          	addi	a5,zero,33
20058ff8:	0d37eb63          	bltu	a5,s3,200590ce <ipc_send_message+0x126>
20058ffc:	200627b7          	lui	a5,0x20062
20059000:	17478793          	addi	a5,a5,372 # 20062174 <CSWTCH.29>
20059004:	97ce                	c.add	a5,s3
20059006:	20062737          	lui	a4,0x20062
2005900a:	0007c583          	lbu	a1,0(a5)
2005900e:	19870713          	addi	a4,a4,408 # 20062198 <CSWTCH.30>
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
2005903a:	14850513          	addi	a0,a0,328 # 20062148 <strtol+0x1e0>
2005903e:	2e2060ef          	jal	ra,2005f320 <DiagPrintf_minimal>
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
20059198:	05c18913          	addi	s2,gp,92 # 20066900 <IPC_IrqData>
2005919c:	0dc18493          	addi	s1,gp,220 # 20066980 <IPC_IrqHandler>
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
200591ec:	0dc18713          	addi	a4,gp,220 # 20066980 <IPC_IrqHandler>
200591f0:	883e                	c.mv	a6,a5
200591f2:	973e                	c.add	a4,a5
200591f4:	05c18793          	addi	a5,gp,92 # 20066900 <IPC_IrqData>
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
2005922a:	e7018793          	addi	a5,gp,-400 # 20066714 <PXID_Idx>
2005922e:	97aa                	c.add	a5,a0
20059230:	0007c783          	lbu	a5,0(a5)
20059234:	892a                	c.mv	s2,a0
20059236:	842e                	c.mv	s0,a1
20059238:	e7018493          	addi	s1,gp,-400 # 20066714 <PXID_Idx>
2005923c:	eb89                	c.bnez	a5,2005924e <IPC_SEMTake+0x3c>
2005923e:	20062537          	lui	a0,0x20062
20059242:	0bf00593          	addi	a1,zero,191
20059246:	22050513          	addi	a0,a0,544 # 20062220 <CSWTCH.30+0x88>
2005924a:	5dc060ef          	jal	ra,2005f826 <io_assert_failed>
2005924e:	00291793          	slli	a5,s2,0x2
20059252:	410209b7          	lui	s3,0x41020
20059256:	99be                	c.add	s3,a5
20059258:	0149a783          	lw	a5,20(s3) # 41020014 <__km4_bd_boot_download_addr__+0x1100e014>
2005925c:	83a1                	c.srli	a5,0x8
2005925e:	8b8d                	c.andi	a5,3
20059260:	cb89                	c.beqz	a5,20059272 <IPC_SEMTake+0x60>
20059262:	c42d                	c.beqz	s0,200592cc <IPC_SEMTake+0xba>
20059264:	ea01a783          	lw	a5,-352(gp) # 20066744 <ipc_delay>
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
200592bc:	2c850513          	addi	a0,a0,712 # 200622c8 <CSWTCH.30+0x130>
200592c0:	060060ef          	jal	ra,2005f320 <DiagPrintf_minimal>
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
2005932a:	e7018693          	addi	a3,gp,-400 # 20066714 <PXID_Idx>
2005932e:	c606                	c.swsp	ra,12(sp)
20059330:	96b2                	c.add	a3,a2
20059332:	0006c683          	lbu	a3,0(a3) # 41020000 <__km4_bd_boot_download_addr__+0x1100e000>
20059336:	20062537          	lui	a0,0x20062
2005933a:	0ff7f793          	andi	a5,a5,255
2005933e:	177d                	c.addi	a4,-1
20059340:	31450513          	addi	a0,a0,788 # 20062314 <CSWTCH.30+0x17c>
20059344:	7dd050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
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
20059360:	eaa1a023          	sw	a0,-352(gp) # 20066744 <ipc_delay>
20059364:	8082                	c.jr	ra

20059366 <IPC_GetDevById>:
20059366:	4789                	c.li	a5,2
20059368:	02a7f163          	bgeu	a5,a0,2005938a <IPC_GetDevById+0x24>
2005936c:	20062537          	lui	a0,0x20062
20059370:	1141                	c.addi	sp,-16
20059372:	22050513          	addi	a0,a0,544 # 20062220 <CSWTCH.30+0x88>
20059376:	11b00593          	addi	a1,zero,283
2005937a:	c606                	c.swsp	ra,12(sp)
2005937c:	4aa060ef          	jal	ra,2005f826 <io_assert_failed>
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
200593e6:	15c18793          	addi	a5,gp,348 # 20066a00 <IMG_ADDR>
200593ea:	97ce                	c.add	a5,s3
200593ec:	439c                	c.lw	a5,0(a5)
200593ee:	892a                	c.mv	s2,a0
200593f0:	15c18493          	addi	s1,gp,348 # 20066a00 <IMG_ADDR>
200593f4:	e7a9                	c.bnez	a5,2005943e <ota_get_cur_index+0x7c>
200593f6:	4601                	c.li	a2,0
200593f8:	15c18593          	addi	a1,gp,348 # 20066a00 <IMG_ADDR>
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
200594fe:	37c50513          	addi	a0,a0,892 # 2006237c <CSWTCH.30+0x1e4>
20059502:	61f050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
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
20059526:	360a0513          	addi	a0,s4,864 # 20062360 <CSWTCH.30+0x1c8>
2005952a:	5f7050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
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
20059584:	39048493          	addi	s1,s1,912 # 20062390 <pmap_func>
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
2005971e:	eac1a483          	lw	s1,-340(gp) # 20066750 <sysactive_timeout_flag>
20059722:	c422                	c.swsp	s0,8(sp)
20059724:	c606                	c.swsp	ra,12(sp)
20059726:	eb018693          	addi	a3,gp,-336 # 20066754 <sysactive_timeout_temp>
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
20059744:	eb018793          	addi	a5,gp,-336 # 20066754 <sysactive_timeout_temp>
20059748:	00e57363          	bgeu	a0,a4,2005974e <pmu_set_sysactive_time+0x34>
2005974c:	843a                	c.mv	s0,a4
2005974e:	0007a023          	sw	zero,0(a5)
20059752:	490020ef          	jal	ra,2005bbe2 <rtos_time_get_current_system_time_ms>
20059756:	ea81a783          	lw	a5,-344(gp) # 2006674c <sleepwakelock_timeout>
2005975a:	800006b7          	lui	a3,0x80000
2005975e:	942a                	c.add	s0,a0
20059760:	ea818713          	addi	a4,gp,-344 # 2006674c <sleepwakelock_timeout>
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
200597ac:	49050513          	addi	a0,a0,1168 # 20062490 <pmap_func+0x100>
200597b0:	076060ef          	jal	ra,2005f826 <io_assert_failed>
200597b4:	00091b63          	bne	s2,zero,200597ca <pmu_register_sleep_callback+0x44>
200597b8:	e889                	c.bnez	s1,200597ca <pmu_register_sleep_callback+0x44>
200597ba:	20062537          	lui	a0,0x20062
200597be:	07400593          	addi	a1,zero,116
200597c2:	49050513          	addi	a0,a0,1168 # 20062490 <pmap_func+0x100>
200597c6:	060060ef          	jal	ra,2005f826 <io_assert_failed>
200597ca:	47d1                	c.li	a5,20
200597cc:	02f40733          	mul	a4,s0,a5
200597d0:	17418793          	addi	a5,gp,372 # 20066a18 <gPsmDdHookInfo>
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
200597f6:	e741a503          	lw	a0,-396(gp) # 20066718 <system_can_yield>
200597fa:	8082                	c.jr	ra

200597fc <pmu_ready_to_sleep>:
200597fc:	1141                	c.addi	sp,-16
200597fe:	c606                	c.swsp	ra,12(sp)
20059800:	3e2020ef          	jal	ra,2005bbe2 <rtos_time_get_current_system_time_ms>
20059804:	ea81a703          	lw	a4,-344(gp) # 2006674c <sleepwakelock_timeout>
20059808:	800006b7          	lui	a3,0x80000
2005980c:	87aa                	c.mv	a5,a0
2005980e:	ffe6c693          	xori	a3,a3,-2
20059812:	00a77e63          	bgeu	a4,a0,2005982e <pmu_ready_to_sleep+0x32>
20059816:	40e507b3          	sub	a5,a0,a4
2005981a:	4501                	c.li	a0,0
2005981c:	02f6e063          	bltu	a3,a5,2005983c <pmu_ready_to_sleep+0x40>
20059820:	3679                	c.jal	200593ae <dsp_status_on>
20059822:	e781a783          	lw	a5,-392(gp) # 2006671c <wakelock>
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
2005984e:	e7818713          	addi	a4,gp,-392 # 2006671c <wakelock>
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
20059874:	e7818713          	addi	a4,gp,-392 # 2006671c <wakelock>
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
2005994c:	528c0793          	addi	a5,s8,1320 # 20062528 <pmap_func+0x198>
20059950:	00050cb7          	lui	s9,0x50
20059954:	d03e                	c.swsp	a5,32(sp)
20059956:	019b07b3          	add	a5,s6,s9
2005995a:	597d                	c.li	s2,-1
2005995c:	4981                	c.li	s3,0
2005995e:	5afd                	c.li	s5,-1
20059960:	54fd                	c.li	s1,-1
20059962:	4a05                	c.li	s4,1
20059964:	c8818b93          	addi	s7,gp,-888 # 2006652c <PSRAM_CALIB_PATTERN>
20059968:	ce3e                	c.swsp	a5,28(sp)
2005996a:	47e2                	c.lwsp	a5,24(sp)
2005996c:	cf91                	c.beqz	a5,20059988 <PSRAM_calibration+0xf6>
2005996e:	5782                	c.lwsp	a5,32(sp)
20059970:	200625b7          	lui	a1,0x20062
20059974:	8752                	c.mv	a4,s4
20059976:	20078693          	addi	a3,a5,512 # 10000200 <__km4_boot_text_start__+0x8001e0>
2005997a:	04900613          	addi	a2,zero,73
2005997e:	53c58593          	addi	a1,a1,1340 # 2006253c <pmap_func+0x1ac>
20059982:	4511                	c.li	a0,4
20059984:	568010ef          	jal	ra,2005aeec <rtk_log_write>
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
20059a98:	53c58593          	addi	a1,a1,1340 # 2006253c <pmap_func+0x1ac>
20059a9c:	4511                	c.li	a0,4
20059a9e:	44e010ef          	jal	ra,2005aeec <rtk_log_write>
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
20059ae8:	78068693          	addi	a3,a3,1920 # 20062780 <pmap_func+0x3f0>
20059aec:	04900613          	addi	a2,zero,73
20059af0:	53c58593          	addi	a1,a1,1340 # 2006253c <pmap_func+0x1ac>
20059af4:	4511                	c.li	a0,4
20059af6:	3f6010ef          	jal	ra,2005aeec <rtk_log_write>
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
20059bd4:	75c68693          	addi	a3,a3,1884 # 2006275c <pmap_func+0x3cc>
20059bd8:	c03e                	c.swsp	a5,0(sp)
20059bda:	57e2                	c.lwsp	a5,56(sp)
20059bdc:	04900613          	addi	a2,zero,73
20059be0:	53c58593          	addi	a1,a1,1340 # 2006253c <pmap_func+0x1ac>
20059be4:	4511                	c.li	a0,4
20059be6:	d67e                	c.swsp	t6,44(sp)
20059be8:	304010ef          	jal	ra,2005aeec <rtk_log_write>
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
20059c6a:	41c18413          	addi	s0,gp,1052 # 20066cc0 <wifi_user_config>
20059c6e:	4581                	c.li	a1,0
20059c70:	41c18513          	addi	a0,gp,1052 # 20066cc0 <wifi_user_config>
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
20059cba:	20070713          	addi	a4,a4,512 # 20200 <__bdram_heap_buffer_size__+0x8ba0>
20059cbe:	4791                	c.li	a5,4
20059cc0:	c018                	c.sw	a4,0(s0)
20059cc2:	02f40923          	sb	a5,50(s0)
20059cc6:	02f40a23          	sb	a5,52(s0)
20059cca:	070f0737          	lui	a4,0x70f0
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

20059cfa <at_test>:
20059cfa:	200625b7          	lui	a1,0x20062
20059cfe:	20062537          	lui	a0,0x20062
20059d02:	7b858593          	addi	a1,a1,1976 # 200627b8 <pmap_func+0x428>
20059d06:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
20059d0a:	6160506f          	jal	zero,2005f320 <DiagPrintf_minimal>

20059d0e <at_otaclear>:
20059d0e:	1141                	c.addi	sp,-16
20059d10:	c606                	c.swsp	ra,12(sp)
20059d12:	462030ef          	jal	ra,2005d174 <sys_clear_ota_signature>
20059d16:	40b2                	c.lwsp	ra,12(sp)
20059d18:	200625b7          	lui	a1,0x20062
20059d1c:	20062537          	lui	a0,0x20062
20059d20:	7cc58593          	addi	a1,a1,1996 # 200627cc <pmap_func+0x43c>
20059d24:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
20059d28:	0141                	c.addi	sp,16
20059d2a:	5f60506f          	jal	zero,2005f320 <DiagPrintf_minimal>

20059d2e <at_otarecover>:
20059d2e:	1141                	c.addi	sp,-16
20059d30:	c606                	c.swsp	ra,12(sp)
20059d32:	564030ef          	jal	ra,2005d296 <sys_recover_ota_signature>
20059d36:	40b2                	c.lwsp	ra,12(sp)
20059d38:	200625b7          	lui	a1,0x20062
20059d3c:	20062537          	lui	a0,0x20062
20059d40:	7d858593          	addi	a1,a1,2008 # 200627d8 <pmap_func+0x448>
20059d44:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
20059d48:	0141                	c.addi	sp,16
20059d4a:	5d60506f          	jal	zero,2005f320 <DiagPrintf_minimal>

20059d4e <at_rst>:
20059d4e:	200625b7          	lui	a1,0x20062
20059d52:	20062537          	lui	a0,0x20062
20059d56:	1141                	c.addi	sp,-16
20059d58:	7e858593          	addi	a1,a1,2024 # 200627e8 <pmap_func+0x458>
20059d5c:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
20059d60:	c606                	c.swsp	ra,12(sp)
20059d62:	5be050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059d66:	40b2                	c.lwsp	ra,12(sp)
20059d68:	0141                	c.addi	sp,16
20059d6a:	6680306f          	jal	zero,2005d3d2 <sys_reset>

20059d6e <at_state>:
20059d6e:	bf010113          	addi	sp,sp,-1040
20059d72:	850a                	c.mv	a0,sp
20059d74:	40112623          	sw	ra,1036(sp)
20059d78:	40812423          	sw	s0,1032(sp)
20059d7c:	5a2040ef          	jal	ra,2005e31e <vTaskList>
20059d80:	20062537          	lui	a0,0x20062
20059d84:	858a                	c.mv	a1,sp
20059d86:	7f050513          	addi	a0,a0,2032 # 200627f0 <pmap_func+0x460>
20059d8a:	596050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059d8e:	850a                	c.mv	a0,sp
20059d90:	5af000ef          	jal	ra,2005ab3e <vPortGetHeapStats>
20059d94:	20063537          	lui	a0,0x20063
20059d98:	80450513          	addi	a0,a0,-2044 # 20062804 <pmap_func+0x474>
20059d9c:	584050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059da0:	ecc1a583          	lw	a1,-308(gp) # 20066770 <total_heap_size>
20059da4:	20063537          	lui	a0,0x20063
20059da8:	81450513          	addi	a0,a0,-2028 # 20062814 <pmap_func+0x484>
20059dac:	574050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059db0:	4582                	c.lwsp	a1,0(sp)
20059db2:	20063537          	lui	a0,0x20063
20059db6:	82850513          	addi	a0,a0,-2008 # 20062828 <pmap_func+0x498>
20059dba:	566050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059dbe:	4782                	c.lwsp	a5,0(sp)
20059dc0:	ecc1a583          	lw	a1,-308(gp) # 20066770 <total_heap_size>
20059dc4:	20063537          	lui	a0,0x20063
20059dc8:	83c50513          	addi	a0,a0,-1988 # 2006283c <pmap_func+0x4ac>
20059dcc:	8d9d                	c.sub	a1,a5
20059dce:	552050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059dd2:	47c2                	c.lwsp	a5,16(sp)
20059dd4:	ecc1a583          	lw	a1,-308(gp) # 20066770 <total_heap_size>
20059dd8:	20063537          	lui	a0,0x20063
20059ddc:	85050513          	addi	a0,a0,-1968 # 20062850 <pmap_func+0x4c0>
20059de0:	8d9d                	c.sub	a1,a5
20059de2:	53e050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059de6:	200635b7          	lui	a1,0x20063
20059dea:	20062537          	lui	a0,0x20062
20059dee:	86458593          	addi	a1,a1,-1948 # 20062864 <pmap_func+0x4d4>
20059df2:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
20059df6:	52a050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059dfa:	40c12083          	lw	ra,1036(sp)
20059dfe:	40812403          	lw	s0,1032(sp)
20059e02:	41010113          	addi	sp,sp,1040
20059e06:	8082                	c.jr	ra

20059e08 <at_gmr>:
20059e08:	715d                	c.addi16sp	sp,-80
20059e0a:	40000513          	addi	a0,zero,1024
20059e0e:	c686                	c.swsp	ra,76(sp)
20059e10:	c4a2                	c.swsp	s0,72(sp)
20059e12:	c2a6                	c.swsp	s1,68(sp)
20059e14:	091010ef          	jal	ra,2005b6a4 <rtos_mem_malloc>
20059e18:	3ff00593          	addi	a1,zero,1023
20059e1c:	842a                	c.mv	s0,a0
20059e1e:	200634b7          	lui	s1,0x20063
20059e22:	3e3040ef          	jal	ra,2005ea04 <ChipInfo_GetSocName_ToBuf>
20059e26:	85a2                	c.mv	a1,s0
20059e28:	86c48513          	addi	a0,s1,-1940 # 2006286c <pmap_func+0x4dc>
20059e2c:	4f4050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059e30:	8522                	c.mv	a0,s0
20059e32:	3ff00593          	addi	a1,zero,1023
20059e36:	48b040ef          	jal	ra,2005eac0 <ChipInfo_GetLibVersion_ToBuf>
20059e3a:	85a2                	c.mv	a1,s0
20059e3c:	86c48513          	addi	a0,s1,-1940
20059e40:	4e0050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059e44:	8522                	c.mv	a0,s0
20059e46:	089010ef          	jal	ra,2005b6ce <rtos_mem_free>
20059e4a:	200635b7          	lui	a1,0x20063
20059e4e:	02000613          	addi	a2,zero,32
20059e52:	87058593          	addi	a1,a1,-1936 # 20062870 <pmap_func+0x4e0>
20059e56:	850a                	c.mv	a0,sp
20059e58:	6f7070ef          	jal	ra,20061d4e <strncpy>
20059e5c:	200635b7          	lui	a1,0x20063
20059e60:	02000613          	addi	a2,zero,32
20059e64:	87858593          	addi	a1,a1,-1928 # 20062878 <pmap_func+0x4e8>
20059e68:	1008                	c.addi4spn	a0,sp,32
20059e6a:	6e5070ef          	jal	ra,20061d4e <strncpy>
20059e6e:	20063737          	lui	a4,0x20063
20059e72:	200635b7          	lui	a1,0x20063
20059e76:	20063537          	lui	a0,0x20063
20059e7a:	1014                	c.addi4spn	a3,sp,32
20059e7c:	860a                	c.mv	a2,sp
20059e7e:	88070713          	addi	a4,a4,-1920 # 20062880 <pmap_func+0x4f0>
20059e82:	89458593          	addi	a1,a1,-1900 # 20062894 <pmap_func+0x504>
20059e86:	89c50513          	addi	a0,a0,-1892 # 2006289c <pmap_func+0x50c>
20059e8a:	496050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059e8e:	40b6                	c.lwsp	ra,76(sp)
20059e90:	4426                	c.lwsp	s0,72(sp)
20059e92:	4496                	c.lwsp	s1,68(sp)
20059e94:	6161                	c.addi16sp	sp,80
20059e96:	8082                	c.jr	ra

20059e98 <at_log>:
20059e98:	7159                	c.addi16sp	sp,-112
20059e9a:	d2a6                	c.swsp	s1,100(sp)
20059e9c:	05c00613          	addi	a2,zero,92
20059ea0:	84aa                	c.mv	s1,a0
20059ea2:	4581                	c.li	a1,0
20059ea4:	0048                	c.addi4spn	a0,sp,4
20059ea6:	d4a2                	c.swsp	s0,104(sp)
20059ea8:	d0ca                	c.swsp	s2,96(sp)
20059eaa:	d686                	c.swsp	ra,108(sp)
20059eac:	20063437          	lui	s0,0x20063
20059eb0:	dffb0097          	auipc	ra,0xdffb0
20059eb4:	c1e080e7          	jalr	ra,-994(ra) # 9ace <__wrap_memset>
20059eb8:	20063937          	lui	s2,0x20063
20059ebc:	ec91                	c.bnez	s1,20059ed8 <at_log+0x40>
20059ebe:	200636b7          	lui	a3,0x20063
20059ec2:	8ac68693          	addi	a3,a3,-1876 # 200628ac <pmap_func+0x51c>
20059ec6:	04100613          	addi	a2,zero,65
20059eca:	8c040593          	addi	a1,s0,-1856 # 200628c0 <pmap_func+0x530>
20059ece:	4505                	c.li	a0,1
20059ed0:	01c010ef          	jal	ra,2005aeec <rtk_log_write>
20059ed4:	4605                	c.li	a2,1
20059ed6:	aa3d                	c.j	2005a014 <at_log+0x17c>
20059ed8:	8526                	c.mv	a0,s1
20059eda:	004c                	c.addi4spn	a1,sp,4
20059edc:	1e6010ef          	jal	ra,2005b0c2 <parse_param>
20059ee0:	4785                	c.li	a5,1
20059ee2:	84aa                	c.mv	s1,a0
20059ee4:	1aa7d063          	bge	a5,a0,2005a084 <at_log+0x1ec>
20059ee8:	4522                	c.lwsp	a0,8(sp)
20059eea:	18050d63          	beq	a0,zero,2005a084 <at_log+0x1ec>
20059eee:	40f070ef          	jal	ra,20061afc <atoi>
20059ef2:	0ff57713          	andi	a4,a0,255
20059ef6:	4789                	c.li	a5,2
20059ef8:	16f70563          	beq	a4,a5,2005a062 <at_log+0x1ca>
20059efc:	00e7ef63          	bltu	a5,a4,20059f1a <at_log+0x82>
20059f00:	e771                	c.bnez	a4,20059fcc <at_log+0x134>
20059f02:	478d                	c.li	a5,3
20059f04:	00f49663          	bne	s1,a5,20059f10 <at_log+0x78>
20059f08:	4532                	c.lwsp	a0,12(sp)
20059f0a:	00054783          	lbu	a5,0(a0)
20059f0e:	e78d                	c.bnez	a5,20059f38 <at_log+0xa0>
20059f10:	200636b7          	lui	a3,0x20063
20059f14:	8c468693          	addi	a3,a3,-1852 # 200628c4 <pmap_func+0x534>
20059f18:	b77d                	c.j	20059ec6 <at_log+0x2e>
20059f1a:	468d                	c.li	a3,3
20059f1c:	14d70f63          	beq	a4,a3,2005a07a <at_log+0x1e2>
20059f20:	200636b7          	lui	a3,0x20063
20059f24:	95868693          	addi	a3,a3,-1704 # 20062958 <pmap_func+0x5c8>
20059f28:	04100613          	addi	a2,zero,65
20059f2c:	8c040593          	addi	a1,s0,-1856
20059f30:	4505                	c.li	a0,1
20059f32:	7bb000ef          	jal	ra,2005aeec <rtk_log_write>
20059f36:	bf79                	c.j	20059ed4 <at_log+0x3c>
20059f38:	579000ef          	jal	ra,2005acb0 <rtk_log_level_get>
20059f3c:	45b2                	c.lwsp	a1,12(sp)
20059f3e:	862a                	c.mv	a2,a0
20059f40:	20063537          	lui	a0,0x20063
20059f44:	8e450513          	addi	a0,a0,-1820 # 200628e4 <pmap_func+0x554>
20059f48:	3d8050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059f4c:	200636b7          	lui	a3,0x20063
20059f50:	97468693          	addi	a3,a3,-1676 # 20062974 <pmap_func+0x5e4>
20059f54:	04100613          	addi	a2,zero,65
20059f58:	8c040593          	addi	a1,s0,-1856
20059f5c:	4505                	c.li	a0,1
20059f5e:	78f000ef          	jal	ra,2005aeec <rtk_log_write>
20059f62:	200636b7          	lui	a3,0x20063
20059f66:	99068693          	addi	a3,a3,-1648 # 20062990 <pmap_func+0x600>
20059f6a:	04500613          	addi	a2,zero,69
20059f6e:	8c040593          	addi	a1,s0,-1856
20059f72:	4509                	c.li	a0,2
20059f74:	779000ef          	jal	ra,2005aeec <rtk_log_write>
20059f78:	200636b7          	lui	a3,0x20063
20059f7c:	9ac68693          	addi	a3,a3,-1620 # 200629ac <pmap_func+0x61c>
20059f80:	05700613          	addi	a2,zero,87
20059f84:	8c040593          	addi	a1,s0,-1856
20059f88:	450d                	c.li	a0,3
20059f8a:	763000ef          	jal	ra,2005aeec <rtk_log_write>
20059f8e:	200636b7          	lui	a3,0x20063
20059f92:	8c040593          	addi	a1,s0,-1856
20059f96:	9c868693          	addi	a3,a3,-1592 # 200629c8 <pmap_func+0x638>
20059f9a:	04900613          	addi	a2,zero,73
20059f9e:	4511                	c.li	a0,4
20059fa0:	74d000ef          	jal	ra,2005aeec <rtk_log_write>
20059fa4:	20063537          	lui	a0,0x20063
20059fa8:	9e050513          	addi	a0,a0,-1568 # 200629e0 <pmap_func+0x650>
20059fac:	374050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059fb0:	20062537          	lui	a0,0x20062
20059fb4:	9f890593          	addi	a1,s2,-1544 # 200629f8 <pmap_func+0x668>
20059fb8:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
20059fbc:	364050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20059fc0:	50b6                	c.lwsp	ra,108(sp)
20059fc2:	5426                	c.lwsp	s0,104(sp)
20059fc4:	5496                	c.lwsp	s1,100(sp)
20059fc6:	5906                	c.lwsp	s2,96(sp)
20059fc8:	6165                	c.addi16sp	sp,112
20059fca:	8082                	c.jr	ra
20059fcc:	4791                	c.li	a5,4
20059fce:	00f49a63          	bne	s1,a5,20059fe2 <at_log+0x14a>
20059fd2:	47b2                	c.lwsp	a5,12(sp)
20059fd4:	0007c783          	lbu	a5,0(a5)
20059fd8:	c789                	c.beqz	a5,20059fe2 <at_log+0x14a>
20059fda:	4542                	c.lwsp	a0,16(sp)
20059fdc:	00054783          	lbu	a5,0(a0)
20059fe0:	e791                	c.bnez	a5,20059fec <at_log+0x154>
20059fe2:	200636b7          	lui	a3,0x20063
20059fe6:	8f868693          	addi	a3,a3,-1800 # 200628f8 <pmap_func+0x568>
20059fea:	bdf1                	c.j	20059ec6 <at_log+0x2e>
20059fec:	311070ef          	jal	ra,20061afc <atoi>
20059ff0:	0ff57593          	andi	a1,a0,255
20059ff4:	4532                	c.lwsp	a0,12(sp)
20059ff6:	537000ef          	jal	ra,2005ad2c <rtk_log_level_set>
20059ffa:	d929                	c.beqz	a0,20059f4c <at_log+0xb4>
20059ffc:	200636b7          	lui	a3,0x20063
2005a000:	04100613          	addi	a2,zero,65
2005a004:	91868693          	addi	a3,a3,-1768 # 20062918 <pmap_func+0x588>
2005a008:	8c040593          	addi	a1,s0,-1856
2005a00c:	4505                	c.li	a0,1
2005a00e:	6df000ef          	jal	ra,2005aeec <rtk_log_write>
2005a012:	4609                	c.li	a2,2
2005a014:	20063537          	lui	a0,0x20063
2005a018:	9f890593          	addi	a1,s2,-1544
2005a01c:	a0050513          	addi	a0,a0,-1536 # 20062a00 <pmap_func+0x670>
2005a020:	300050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a024:	20063537          	lui	a0,0x20063
2005a028:	03050513          	addi	a0,a0,48 # 20063030 <__FUNCTION__.0+0x20c>
2005a02c:	2f4050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a030:	20063537          	lui	a0,0x20063
2005a034:	a1050513          	addi	a0,a0,-1520 # 20062a10 <pmap_func+0x680>
2005a038:	2e8050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a03c:	20063537          	lui	a0,0x20063
2005a040:	a3c50513          	addi	a0,a0,-1476 # 20062a3c <pmap_func+0x6ac>
2005a044:	2dc050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a048:	20063537          	lui	a0,0x20063
2005a04c:	a7450513          	addi	a0,a0,-1420 # 20062a74 <pmap_func+0x6e4>
2005a050:	2d0050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a054:	20063537          	lui	a0,0x20063
2005a058:	aa850513          	addi	a0,a0,-1368 # 20062aa8 <pmap_func+0x718>
2005a05c:	2c4050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a060:	b785                	c.j	20059fc0 <at_log+0x128>
2005a062:	00e48763          	beq	s1,a4,2005a070 <at_log+0x1d8>
2005a066:	200636b7          	lui	a3,0x20063
2005a06a:	93468693          	addi	a3,a3,-1740 # 20062934 <pmap_func+0x5a4>
2005a06e:	bda1                	c.j	20059ec6 <at_log+0x2e>
2005a070:	48c18513          	addi	a0,gp,1164 # 20066d30 <rtk_log_tag_array>
2005a074:	3c9000ef          	jal	ra,2005ac3c <rtk_log_array_print>
2005a078:	bdd1                	c.j	20059f4c <at_log+0xb4>
2005a07a:	fef496e3          	bne	s1,a5,2005a066 <at_log+0x1ce>
2005a07e:	413000ef          	jal	ra,2005ac90 <rtk_log_array_clear>
2005a082:	b5e9                	c.j	20059f4c <at_log+0xb4>
2005a084:	4711                	c.li	a4,4
2005a086:	bd69                	c.j	20059f20 <at_log+0x88>

2005a088 <at_rreg>:
2005a088:	7159                	c.addi16sp	sp,-112
2005a08a:	d4a2                	c.swsp	s0,104(sp)
2005a08c:	05c00613          	addi	a2,zero,92
2005a090:	842a                	c.mv	s0,a0
2005a092:	4581                	c.li	a1,0
2005a094:	0048                	c.addi4spn	a0,sp,4
2005a096:	d2a6                	c.swsp	s1,100(sp)
2005a098:	d686                	c.swsp	ra,108(sp)
2005a09a:	200634b7          	lui	s1,0x20063
2005a09e:	dffb0097          	auipc	ra,0xdffb0
2005a0a2:	a30080e7          	jalr	ra,-1488(ra) # 9ace <__wrap_memset>
2005a0a6:	ec05                	c.bnez	s0,2005a0de <at_rreg+0x56>
2005a0a8:	200636b7          	lui	a3,0x20063
2005a0ac:	200635b7          	lui	a1,0x20063
2005a0b0:	05700613          	addi	a2,zero,87
2005a0b4:	8c058593          	addi	a1,a1,-1856 # 200628c0 <pmap_func+0x530>
2005a0b8:	450d                	c.li	a0,3
2005a0ba:	adc68693          	addi	a3,a3,-1316 # 20062adc <pmap_func+0x74c>
2005a0be:	62f000ef          	jal	ra,2005aeec <rtk_log_write>
2005a0c2:	20063537          	lui	a0,0x20063
2005a0c6:	4605                	c.li	a2,1
2005a0c8:	af848593          	addi	a1,s1,-1288 # 20062af8 <pmap_func+0x768>
2005a0cc:	a0050513          	addi	a0,a0,-1536 # 20062a00 <pmap_func+0x670>
2005a0d0:	250050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a0d4:	50b6                	c.lwsp	ra,108(sp)
2005a0d6:	5426                	c.lwsp	s0,104(sp)
2005a0d8:	5496                	c.lwsp	s1,100(sp)
2005a0da:	6165                	c.addi16sp	sp,112
2005a0dc:	8082                	c.jr	ra
2005a0de:	004c                	c.addi4spn	a1,sp,4
2005a0e0:	8522                	c.mv	a0,s0
2005a0e2:	7e1000ef          	jal	ra,2005b0c2 <parse_param>
2005a0e6:	ffe50713          	addi	a4,a0,-2
2005a0ea:	4789                	c.li	a5,2
2005a0ec:	fae7eee3          	bltu	a5,a4,2005a0a8 <at_rreg+0x20>
2005a0f0:	157d                	c.addi	a0,-1
2005a0f2:	0542                	c.slli	a0,0x10
2005a0f4:	002c                	c.addi4spn	a1,sp,8
2005a0f6:	8141                	c.srli	a0,0x10
2005a0f8:	3ca010ef          	jal	ra,2005b4c2 <cmd_dump_word>
2005a0fc:	20062537          	lui	a0,0x20062
2005a100:	af848593          	addi	a1,s1,-1288
2005a104:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
2005a108:	218050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a10c:	b7e1                	c.j	2005a0d4 <at_rreg+0x4c>

2005a10e <at_wreg>:
2005a10e:	7159                	c.addi16sp	sp,-112
2005a110:	d4a2                	c.swsp	s0,104(sp)
2005a112:	05c00613          	addi	a2,zero,92
2005a116:	842a                	c.mv	s0,a0
2005a118:	4581                	c.li	a1,0
2005a11a:	0048                	c.addi4spn	a0,sp,4
2005a11c:	d2a6                	c.swsp	s1,100(sp)
2005a11e:	d686                	c.swsp	ra,108(sp)
2005a120:	200634b7          	lui	s1,0x20063
2005a124:	dffb0097          	auipc	ra,0xdffb0
2005a128:	9aa080e7          	jalr	ra,-1622(ra) # 9ace <__wrap_memset>
2005a12c:	ec05                	c.bnez	s0,2005a164 <at_wreg+0x56>
2005a12e:	200636b7          	lui	a3,0x20063
2005a132:	200635b7          	lui	a1,0x20063
2005a136:	05700613          	addi	a2,zero,87
2005a13a:	8c058593          	addi	a1,a1,-1856 # 200628c0 <pmap_func+0x530>
2005a13e:	450d                	c.li	a0,3
2005a140:	b0068693          	addi	a3,a3,-1280 # 20062b00 <pmap_func+0x770>
2005a144:	5a9000ef          	jal	ra,2005aeec <rtk_log_write>
2005a148:	20063537          	lui	a0,0x20063
2005a14c:	4605                	c.li	a2,1
2005a14e:	b1c48593          	addi	a1,s1,-1252 # 20062b1c <pmap_func+0x78c>
2005a152:	a0050513          	addi	a0,a0,-1536 # 20062a00 <pmap_func+0x670>
2005a156:	1ca050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a15a:	50b6                	c.lwsp	ra,108(sp)
2005a15c:	5426                	c.lwsp	s0,104(sp)
2005a15e:	5496                	c.lwsp	s1,100(sp)
2005a160:	6165                	c.addi16sp	sp,112
2005a162:	8082                	c.jr	ra
2005a164:	004c                	c.addi4spn	a1,sp,4
2005a166:	8522                	c.mv	a0,s0
2005a168:	75b000ef          	jal	ra,2005b0c2 <parse_param>
2005a16c:	478d                	c.li	a5,3
2005a16e:	fcf510e3          	bne	a0,a5,2005a12e <at_wreg+0x20>
2005a172:	45a2                	c.lwsp	a1,8(sp)
2005a174:	4509                	c.li	a0,2
2005a176:	2a6010ef          	jal	ra,2005b41c <cmd_write_word>
2005a17a:	20062537          	lui	a0,0x20062
2005a17e:	b1c48593          	addi	a1,s1,-1252
2005a182:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
2005a186:	19a050ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a18a:	bfc1                	c.j	2005a15a <at_wreg+0x4c>

2005a18c <find_status>:
2005a18c:	872a                	c.mv	a4,a0
2005a18e:	4781                	c.li	a5,0
2005a190:	00c7c463          	blt	a5,a2,2005a198 <find_status+0xc>
2005a194:	4501                	c.li	a0,0
2005a196:	8082                	c.jr	ra
2005a198:	852e                	c.mv	a0,a1
2005a19a:	4594                	c.lw	a3,8(a1)
2005a19c:	02458593          	addi	a1,a1,36
2005a1a0:	fee68be3          	beq	a3,a4,2005a196 <find_status+0xa>
2005a1a4:	0785                	c.addi	a5,1
2005a1a6:	b7ed                	c.j	2005a190 <find_status+0x4>

2005a1a8 <find_free_delta>:
2005a1a8:	87aa                	c.mv	a5,a0
2005a1aa:	4701                	c.li	a4,0
2005a1ac:	00b74463          	blt	a4,a1,2005a1b4 <find_free_delta+0xc>
2005a1b0:	4501                	c.li	a0,0
2005a1b2:	8082                	c.jr	ra
2005a1b4:	853e                	c.mv	a0,a5
2005a1b6:	5794                	c.lw	a3,40(a5)
2005a1b8:	03878793          	addi	a5,a5,56
2005a1bc:	dafd                	c.beqz	a3,2005a1b2 <find_free_delta+0xa>
2005a1be:	0705                	c.addi	a4,1
2005a1c0:	b7f5                	c.j	2005a1ac <find_free_delta+0x4>

2005a1c2 <update_status>:
2005a1c2:	7179                	c.addi16sp	sp,-48
2005a1c4:	d226                	c.swsp	s1,36(sp)
2005a1c6:	ec01a783          	lw	a5,-320(gp) # 20066764 <task_status>
2005a1ca:	6705                	c.lui	a4,0x1
2005a1cc:	d606                	c.swsp	ra,44(sp)
2005a1ce:	d422                	c.swsp	s0,40(sp)
2005a1d0:	d04a                	c.swsp	s2,32(sp)
2005a1d2:	ce4e                	c.swsp	s3,28(sp)
2005a1d4:	cc52                	c.swsp	s4,24(sp)
2005a1d6:	ca56                	c.swsp	s5,20(sp)
2005a1d8:	c85a                	c.swsp	s6,16(sp)
2005a1da:	c65e                	c.swsp	s7,12(sp)
2005a1dc:	c462                	c.swsp	s8,8(sp)
2005a1de:	00e786b3          	add	a3,a5,a4
2005a1e2:	2006a603          	lw	a2,512(a3)
2005a1e6:	ec018493          	addi	s1,gp,-320 # 20066764 <task_status>
2005a1ea:	ea19                	c.bnez	a2,2005a200 <update_status+0x3e>
2005a1ec:	2046a603          	lw	a2,516(a3)
2005a1f0:	ea01                	c.bnez	a2,2005a200 <update_status+0x3e>
2005a1f2:	90070713          	addi	a4,a4,-1792 # 900 <BKUP_Set+0x5a>
2005a1f6:	973e                	c.add	a4,a5
2005a1f8:	20f6a023          	sw	a5,512(a3)
2005a1fc:	20e6a223          	sw	a4,516(a3)
2005a200:	6405                	c.lui	s0,0x1
2005a202:	97a2                	c.add	a5,s0
2005a204:	20078793          	addi	a5,a5,512
2005a208:	47d8                	c.lw	a4,12(a5)
2005a20a:	43d4                	c.lw	a3,4(a5)
2005a20c:	21040913          	addi	s2,s0,528 # 1210 <CPU_ClkGet+0x6fa>
2005a210:	c798                	c.sw	a4,8(a5)
2005a212:	4398                	c.lw	a4,0(a5)
2005a214:	c394                	c.sw	a3,0(a5)
2005a216:	4a01                	c.li	s4,0
2005a218:	c3d8                	c.sw	a4,4(a5)
2005a21a:	466030ef          	jal	ra,2005d680 <uxTaskGetNumberOfTasks>
2005a21e:	409c                	c.lw	a5,0(s1)
2005a220:	85aa                	c.mv	a1,a0
2005a222:	4601                	c.li	a2,0
2005a224:	97a2                	c.add	a5,s0
2005a226:	20a7a623          	sw	a0,524(a5)
2005a22a:	2047a503          	lw	a0,516(a5)
2005a22e:	7b1030ef          	jal	ra,2005e1de <uxTaskGetSystemState>
2005a232:	409c                	c.lw	a5,0(s1)
2005a234:	e0040613          	addi	a2,s0,-512
2005a238:	4581                	c.li	a1,0
2005a23a:	00878733          	add	a4,a5,s0
2005a23e:	20a72623          	sw	a0,524(a4)
2005a242:	01278533          	add	a0,a5,s2
2005a246:	dffb0097          	auipc	ra,0xdffb0
2005a24a:	888080e7          	jalr	ra,-1912(ra) # 9ace <__wrap_memset>
2005a24e:	0004aa83          	lw	s5,0(s1)
2005a252:	9456                	c.add	s0,s5
2005a254:	20842783          	lw	a5,520(s0)
2005a258:	00f05963          	bge	zero,a5,2005a26a <update_status+0xa8>
2005a25c:	02400c13          	addi	s8,zero,36
2005a260:	9aca                	c.add	s5,s2
2005a262:	20c42783          	lw	a5,524(s0)
2005a266:	00fa4e63          	blt	s4,a5,2005a282 <update_status+0xc0>
2005a26a:	50b2                	c.lwsp	ra,44(sp)
2005a26c:	5422                	c.lwsp	s0,40(sp)
2005a26e:	5492                	c.lwsp	s1,36(sp)
2005a270:	5902                	c.lwsp	s2,32(sp)
2005a272:	49f2                	c.lwsp	s3,28(sp)
2005a274:	4a62                	c.lwsp	s4,24(sp)
2005a276:	4ad2                	c.lwsp	s5,20(sp)
2005a278:	4b42                	c.lwsp	s6,16(sp)
2005a27a:	4bb2                	c.lwsp	s7,12(sp)
2005a27c:	4c22                	c.lwsp	s8,8(sp)
2005a27e:	6145                	c.addi16sp	sp,48
2005a280:	8082                	c.jr	ra
2005a282:	038a0933          	mul	s2,s4,s8
2005a286:	20442b83          	lw	s7,516(s0)
2005a28a:	20842603          	lw	a2,520(s0)
2005a28e:	20042583          	lw	a1,512(s0)
2005a292:	9bca                	c.add	s7,s2
2005a294:	008ba503          	lw	a0,8(s7)
2005a298:	3dd5                	c.jal	2005a18c <find_status>
2005a29a:	04000593          	addi	a1,zero,64
2005a29e:	8b2a                	c.mv	s6,a0
2005a2a0:	8556                	c.mv	a0,s5
2005a2a2:	3719                	c.jal	2005a1a8 <find_free_delta>
2005a2a4:	000ba783          	lw	a5,0(s7)
2005a2a8:	84aa                	c.mv	s1,a0
2005a2aa:	467d                	c.li	a2,31
2005a2ac:	d51c                	c.sw	a5,40(a0)
2005a2ae:	20442783          	lw	a5,516(s0)
2005a2b2:	97ca                	c.add	a5,s2
2005a2b4:	43cc                	c.lw	a1,4(a5)
2005a2b6:	c14c                	c.sw	a1,4(a0)
2005a2b8:	20442783          	lw	a5,516(s0)
2005a2bc:	97ca                	c.add	a5,s2
2005a2be:	479c                	c.lw	a5,8(a5)
2005a2c0:	00052023          	sw	zero,0(a0)
2005a2c4:	0521                	c.addi	a0,8
2005a2c6:	d15c                	c.sw	a5,36(a0)
2005a2c8:	20442783          	lw	a5,516(s0)
2005a2cc:	97ca                	c.add	a5,s2
2005a2ce:	00c7c783          	lbu	a5,12(a5)
2005a2d2:	02f50623          	sb	a5,44(a0)
2005a2d6:	20442783          	lw	a5,516(s0)
2005a2da:	97ca                	c.add	a5,s2
2005a2dc:	4b9c                	c.lw	a5,16(a5)
2005a2de:	d51c                	c.sw	a5,40(a0)
2005a2e0:	26f070ef          	jal	ra,20061d4e <strncpy>
2005a2e4:	c095                	c.beqz	s1,2005a308 <update_status+0x146>
2005a2e6:	20442783          	lw	a5,516(s0)
2005a2ea:	97ca                	c.add	a5,s2
2005a2ec:	4f9c                	c.lw	a5,24(a5)
2005a2ee:	000b0563          	beq	s6,zero,2005a2f8 <update_status+0x136>
2005a2f2:	018b2703          	lw	a4,24(s6)
2005a2f6:	8f99                	c.sub	a5,a4
2005a2f8:	c09c                	c.sw	a5,0(s1)
2005a2fa:	20442783          	lw	a5,516(s0)
2005a2fe:	993e                	c.add	s2,a5
2005a300:	00c94783          	lbu	a5,12(s2)
2005a304:	02f48a23          	sb	a5,52(s1)
2005a308:	0a05                	c.addi	s4,1
2005a30a:	bfa1                	c.j	2005a262 <update_status+0xa0>

2005a30c <print_delta>:
2005a30c:	715d                	c.addi16sp	sp,-80
2005a30e:	736277b7          	lui	a5,0x73627
2005a312:	c4a2                	c.swsp	s0,72(sp)
2005a314:	25278793          	addi	a5,a5,594 # 73627252 <__kr4_psram_text_start__+0x134a7232>
2005a318:	842a                	c.mv	s0,a0
2005a31a:	20063737          	lui	a4,0x20063
2005a31e:	200636b7          	lui	a3,0x20063
2005a322:	20063637          	lui	a2,0x20063
2005a326:	200635b7          	lui	a1,0x20063
2005a32a:	20063537          	lui	a0,0x20063
2005a32e:	c43e                	c.swsp	a5,8(sp)
2005a330:	b2470713          	addi	a4,a4,-1244 # 20062b24 <pmap_func+0x794>
2005a334:	04400793          	addi	a5,zero,68
2005a338:	b2c68693          	addi	a3,a3,-1236 # 20062b2c <pmap_func+0x79c>
2005a33c:	b3460613          	addi	a2,a2,-1228 # 20062b34 <pmap_func+0x7a4>
2005a340:	b3c58593          	addi	a1,a1,-1220 # 20062b3c <pmap_func+0x7ac>
2005a344:	b4850513          	addi	a0,a0,-1208 # 20062b48 <pmap_func+0x7b8>
2005a348:	c2a6                	c.swsp	s1,68(sp)
2005a34a:	c0ca                	c.swsp	s2,64(sp)
2005a34c:	de4e                	c.swsp	s3,60(sp)
2005a34e:	dc52                	c.swsp	s4,56(sp)
2005a350:	da56                	c.swsp	s5,52(sp)
2005a352:	d85a                	c.swsp	s6,48(sp)
2005a354:	d65e                	c.swsp	s7,44(sp)
2005a356:	ee22                	c.fswsp	fs0,28(sp)
2005a358:	00f10623          	sb	a5,12(sp)
2005a35c:	c686                	c.swsp	ra,76(sp)
2005a35e:	00011223          	sh	zero,4(sp)
2005a362:	00010323          	sb	zero,6(sp)
2005a366:	7bb040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a36a:	e5c1a407          	flw	fs0,-420(gp) # 20066700 <__sdata_start__>
2005a36e:	6485                	c.lui	s1,0x1
2005a370:	4b81                	c.li	s7,0
2005a372:	03800993          	addi	s3,zero,56
2005a376:	06400b13          	addi	s6,zero,100
2005a37a:	21848a13          	addi	s4,s1,536 # 1218 <CPU_ClkGet+0x702>
2005a37e:	20063ab7          	lui	s5,0x20063
2005a382:	460d                	c.li	a2,3
2005a384:	4581                	c.li	a1,0
2005a386:	0048                	c.addi4spn	a0,sp,4
2005a388:	dffaf097          	auipc	ra,0xdffaf
2005a38c:	746080e7          	jalr	ra,1862(ra) # 9ace <__wrap_memset>
2005a390:	033b8633          	mul	a2,s7,s3
2005a394:	ec01a803          	lw	a6,-320(gp) # 20066764 <task_status>
2005a398:	d0047753          	fcvt.s.w	fa4,s0
2005a39c:	4529                	c.li	a0,10
2005a39e:	00c806b3          	add	a3,a6,a2
2005a3a2:	96a6                	c.add	a3,s1
2005a3a4:	2106a783          	lw	a5,528(a3)
2005a3a8:	d007f7d3          	fcvt.s.w	fa5,a5
2005a3ac:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a3b0:	18e7f7d3          	fdiv.s	fa5,fa5,fa4
2005a3b4:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a3b8:	c0079753          	fcvt.w.s	a4,fa5,rtz
2005a3bc:	036767b3          	rem	a5,a4,s6
2005a3c0:	02a787b3          	mul	a5,a5,a0
2005a3c4:	0367c5b3          	div	a1,a5,s6
2005a3c8:	0367e7b3          	rem	a5,a5,s6
2005a3cc:	03058593          	addi	a1,a1,48
2005a3d0:	00b10223          	sb	a1,4(sp)
2005a3d4:	02a787b3          	mul	a5,a5,a0
2005a3d8:	0367c7b3          	div	a5,a5,s6
2005a3dc:	03078793          	addi	a5,a5,48
2005a3e0:	00f102a3          	sb	a5,5(sp)
2005a3e4:	2386a783          	lw	a5,568(a3)
2005a3e8:	c78d                	c.beqz	a5,2005a412 <print_delta+0x106>
2005a3ea:	03674733          	div	a4,a4,s6
2005a3ee:	2446c503          	lbu	a0,580(a3)
2005a3f2:	014605b3          	add	a1,a2,s4
2005a3f6:	2406a683          	lw	a3,576(a3)
2005a3fa:	01050793          	addi	a5,a0,16
2005a3fe:	00278533          	add	a0,a5,sp
2005a402:	ff854603          	lbu	a2,-8(a0)
2005a406:	005c                	c.addi4spn	a5,sp,4
2005a408:	95c2                	c.add	a1,a6
2005a40a:	b58a8513          	addi	a0,s5,-1192 # 20062b58 <pmap_func+0x7c8>
2005a40e:	713040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a412:	0b85                	c.addi	s7,1
2005a414:	04000793          	addi	a5,zero,64
2005a418:	f6fb95e3          	bne	s7,a5,2005a382 <print_delta+0x76>
2005a41c:	40b6                	c.lwsp	ra,76(sp)
2005a41e:	4426                	c.lwsp	s0,72(sp)
2005a420:	4496                	c.lwsp	s1,68(sp)
2005a422:	4906                	c.lwsp	s2,64(sp)
2005a424:	59f2                	c.lwsp	s3,60(sp)
2005a426:	5a62                	c.lwsp	s4,56(sp)
2005a428:	5ad2                	c.lwsp	s5,52(sp)
2005a42a:	5b42                	c.lwsp	s6,48(sp)
2005a42c:	5bb2                	c.lwsp	s7,44(sp)
2005a42e:	6472                	c.flwsp	fs0,28(sp)
2005a430:	6161                	c.addi16sp	sp,80
2005a432:	8082                	c.jr	ra

2005a434 <cpu_stat_thread>:
2005a434:	7179                	c.addi16sp	sp,-48
2005a436:	c62a                	c.swsp	a0,12(sp)
2005a438:	4521                	c.li	a0,8
2005a43a:	d226                	c.swsp	s1,36(sp)
2005a43c:	d606                	c.swsp	ra,44(sp)
2005a43e:	d422                	c.swsp	s0,40(sp)
2005a440:	d04a                	c.swsp	s2,32(sp)
2005a442:	ce4e                	c.swsp	s3,28(sp)
2005a444:	cc52                	c.swsp	s4,24(sp)
2005a446:	ca56                	c.swsp	s5,20(sp)
2005a448:	25c010ef          	jal	ra,2005b6a4 <rtos_mem_malloc>
2005a44c:	45b2                	c.lwsp	a1,12(sp)
2005a44e:	ec418493          	addi	s1,gp,-316 # 20066768 <top_exit_sema>
2005a452:	e131                	c.bnez	a0,2005a496 <cpu_stat_thread+0x62>
2005a454:	200635b7          	lui	a1,0x20063
2005a458:	20063537          	lui	a0,0x20063
2005a45c:	e2458593          	addi	a1,a1,-476 # 20062e24 <__FUNCTION__.0>
2005a460:	b6c50513          	addi	a0,a0,-1172 # 20062b6c <pmap_func+0x7dc>
2005a464:	6bd040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a468:	4088                	c.lw	a0,0(s1)
2005a46a:	ec018413          	addi	s0,gp,-320 # 20066764 <task_status>
2005a46e:	44e010ef          	jal	ra,2005b8bc <rtos_sema_delete>
2005a472:	4008                	c.lw	a0,0(s0)
2005a474:	0004a023          	sw	zero,0(s1)
2005a478:	256010ef          	jal	ra,2005b6ce <rtos_mem_free>
2005a47c:	00042023          	sw	zero,0(s0)
2005a480:	5422                	c.lwsp	s0,40(sp)
2005a482:	50b2                	c.lwsp	ra,44(sp)
2005a484:	5492                	c.lwsp	s1,36(sp)
2005a486:	5902                	c.lwsp	s2,32(sp)
2005a488:	49f2                	c.lwsp	s3,28(sp)
2005a48a:	4a62                	c.lwsp	s4,24(sp)
2005a48c:	4ad2                	c.lwsp	s5,20(sp)
2005a48e:	4501                	c.li	a0,0
2005a490:	6145                	c.addi16sp	sp,48
2005a492:	7040106f          	jal	zero,2005bb96 <rtos_task_delete>
2005a496:	4621                	c.li	a2,8
2005a498:	842a                	c.mv	s0,a0
2005a49a:	dffaf097          	auipc	ra,0xdffaf
2005a49e:	636080e7          	jalr	ra,1590(ra) # 9ad0 <__wrap_memcpy>
2005a4a2:	410187b7          	lui	a5,0x41018
2005a4a6:	4bdc                	c.lw	a5,20(a5)
2005a4a8:	3e800a93          	addi	s5,zero,1000
2005a4ac:	eaf1aa23          	sw	a5,-332(gp) # 20066758 <last_tick>
2005a4b0:	5a7d                	c.li	s4,-1
2005a4b2:	eb418913          	addi	s2,gp,-332 # 20066758 <last_tick>
2005a4b6:	410189b7          	lui	s3,0x41018
2005a4ba:	400c                	c.lw	a1,0(s0)
2005a4bc:	4088                	c.lw	a0,0(s1)
2005a4be:	02ba85b3          	mul	a1,s5,a1
2005a4c2:	410010ef          	jal	ra,2005b8d2 <rtos_sema_take>
2005a4c6:	03451463          	bne	a0,s4,2005a4ee <cpu_stat_thread+0xba>
2005a4ca:	39e5                	c.jal	2005a1c2 <update_status>
2005a4cc:	0149a503          	lw	a0,20(s3) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a4d0:	00092783          	lw	a5,0(s2)
2005a4d4:	0149a703          	lw	a4,20(s3)
2005a4d8:	8d1d                	c.sub	a0,a5
2005a4da:	00e92023          	sw	a4,0(s2)
2005a4de:	353d                	c.jal	2005a30c <print_delta>
2005a4e0:	405c                	c.lw	a5,4(s0)
2005a4e2:	00f05563          	bge	zero,a5,2005a4ec <cpu_stat_thread+0xb8>
2005a4e6:	17fd                	c.addi	a5,-1
2005a4e8:	c05c                	c.sw	a5,4(s0)
2005a4ea:	bfc1                	c.j	2005a4ba <cpu_stat_thread+0x86>
2005a4ec:	f7f9                	c.bnez	a5,2005a4ba <cpu_stat_thread+0x86>
2005a4ee:	8522                	c.mv	a0,s0
2005a4f0:	1de010ef          	jal	ra,2005b6ce <rtos_mem_free>
2005a4f4:	bf95                	c.j	2005a468 <cpu_stat_thread+0x34>

2005a4f6 <at_cpuload>:
2005a4f6:	7119                	c.addi16sp	sp,-128
2005a4f8:	dca2                	c.swsp	s0,120(sp)
2005a4fa:	05c00613          	addi	a2,zero,92
2005a4fe:	842a                	c.mv	s0,a0
2005a500:	4581                	c.li	a1,0
2005a502:	0048                	c.addi4spn	a0,sp,4
2005a504:	de86                	c.swsp	ra,124(sp)
2005a506:	d8ca                	c.swsp	s2,112(sp)
2005a508:	d4d2                	c.swsp	s4,104(sp)
2005a50a:	daa6                	c.swsp	s1,116(sp)
2005a50c:	d6ce                	c.swsp	s3,108(sp)
2005a50e:	d2d6                	c.swsp	s5,100(sp)
2005a510:	dffaf097          	auipc	ra,0xdffaf
2005a514:	5be080e7          	jalr	ra,1470(ra) # 9ace <__wrap_memset>
2005a518:	eb818793          	addi	a5,gp,-328 # 2006675c <para_in>
2005a51c:	4705                	c.li	a4,1
2005a51e:	c398                	c.sw	a4,0(a5)
2005a520:	004c                	c.addi4spn	a1,sp,4
2005a522:	577d                	c.li	a4,-1
2005a524:	8522                	c.mv	a0,s0
2005a526:	c3d8                	c.sw	a4,4(a5)
2005a528:	39b000ef          	jal	ra,2005b0c2 <parse_param>
2005a52c:	ffe50713          	addi	a4,a0,-2
2005a530:	4789                	c.li	a5,2
2005a532:	20063a37          	lui	s4,0x20063
2005a536:	06e7f463          	bgeu	a5,a4,2005a59e <at_cpuload+0xa8>
2005a53a:	20063537          	lui	a0,0x20063
2005a53e:	b8050513          	addi	a0,a0,-1152 # 20062b80 <pmap_func+0x7f0>
2005a542:	5df040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a546:	4405                	c.li	s0,1
2005a548:	20063537          	lui	a0,0x20063
2005a54c:	8622                	c.mv	a2,s0
2005a54e:	d0ca0593          	addi	a1,s4,-756 # 20062d0c <pmap_func+0x97c>
2005a552:	a0050513          	addi	a0,a0,-1536 # 20062a00 <pmap_func+0x670>
2005a556:	9875                	c.andi	s0,-3
2005a558:	5c9040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a55c:	14040b63          	beq	s0,zero,2005a6b2 <at_cpuload+0x1bc>
2005a560:	20063537          	lui	a0,0x20063
2005a564:	03050513          	addi	a0,a0,48 # 20063030 <__FUNCTION__.0+0x20c>
2005a568:	5b9040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a56c:	20063537          	lui	a0,0x20063
2005a570:	d1850513          	addi	a0,a0,-744 # 20062d18 <pmap_func+0x988>
2005a574:	5ad040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a578:	20063537          	lui	a0,0x20063
2005a57c:	d4850513          	addi	a0,a0,-696 # 20062d48 <pmap_func+0x9b8>
2005a580:	5a1040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a584:	20063537          	lui	a0,0x20063
2005a588:	d5c50513          	addi	a0,a0,-676 # 20062d5c <pmap_func+0x9cc>
2005a58c:	595040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a590:	20063537          	lui	a0,0x20063
2005a594:	d7850513          	addi	a0,a0,-648 # 20062d78 <pmap_func+0x9e8>
2005a598:	589040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a59c:	aa19                	c.j	2005a6b2 <at_cpuload+0x1bc>
2005a59e:	84aa                	c.mv	s1,a0
2005a5a0:	4522                	c.lwsp	a0,8(sp)
2005a5a2:	eb818993          	addi	s3,gp,-328 # 2006675c <para_in>
2005a5a6:	4411                	c.li	s0,4
2005a5a8:	00054783          	lbu	a5,0(a0)
2005a5ac:	c789                	c.beqz	a5,2005a5b6 <at_cpuload+0xc0>
2005a5ae:	54e070ef          	jal	ra,20061afc <atoi>
2005a5b2:	0ff57413          	andi	s0,a0,255
2005a5b6:	4789                	c.li	a5,2
2005a5b8:	04f48463          	beq	s1,a5,2005a600 <at_cpuload+0x10a>
2005a5bc:	4532                	c.lwsp	a0,12(sp)
2005a5be:	00054783          	lbu	a5,0(a0)
2005a5c2:	cf81                	c.beqz	a5,2005a5da <at_cpuload+0xe4>
2005a5c4:	538070ef          	jal	ra,20061afc <atoi>
2005a5c8:	4785                	c.li	a5,1
2005a5ca:	00a05663          	bge	zero,a0,2005a5d6 <at_cpuload+0xe0>
2005a5ce:	4532                	c.lwsp	a0,12(sp)
2005a5d0:	52c070ef          	jal	ra,20061afc <atoi>
2005a5d4:	87aa                	c.mv	a5,a0
2005a5d6:	00f9a023          	sw	a5,0(s3)
2005a5da:	4791                	c.li	a5,4
2005a5dc:	02f49263          	bne	s1,a5,2005a600 <at_cpuload+0x10a>
2005a5e0:	4542                	c.lwsp	a0,16(sp)
2005a5e2:	00054783          	lbu	a5,0(a0)
2005a5e6:	cf89                	c.beqz	a5,2005a600 <at_cpuload+0x10a>
2005a5e8:	514070ef          	jal	ra,20061afc <atoi>
2005a5ec:	57fd                	c.li	a5,-1
2005a5ee:	00a05763          	bge	zero,a0,2005a5fc <at_cpuload+0x106>
2005a5f2:	4542                	c.lwsp	a0,16(sp)
2005a5f4:	508070ef          	jal	ra,20061afc <atoi>
2005a5f8:	fff50793          	addi	a5,a0,-1
2005a5fc:	00f9a223          	sw	a5,4(s3)
2005a600:	ec018a93          	addi	s5,gp,-320 # 20066764 <task_status>
2005a604:	000aa783          	lw	a5,0(s5)
2005a608:	ec018493          	addi	s1,gp,-320 # 20066764 <task_status>
2005a60c:	e79d                	c.bnez	a5,2005a63a <at_cpuload+0x144>
2005a60e:	6509                	c.lui	a0,0x2
2005a610:	0541                	c.addi	a0,16
2005a612:	092010ef          	jal	ra,2005b6a4 <rtos_mem_malloc>
2005a616:	00aaa023          	sw	a0,0(s5)
2005a61a:	e105                	c.bnez	a0,2005a63a <at_cpuload+0x144>
2005a61c:	200636b7          	lui	a3,0x20063
2005a620:	200635b7          	lui	a1,0x20063
2005a624:	c9c68693          	addi	a3,a3,-868 # 20062c9c <pmap_func+0x90c>
2005a628:	04900613          	addi	a2,zero,73
2005a62c:	8c058593          	addi	a1,a1,-1856 # 200628c0 <pmap_func+0x530>
2005a630:	4511                	c.li	a0,4
2005a632:	0bb000ef          	jal	ra,2005aeec <rtk_log_write>
2005a636:	4409                	c.li	s0,2
2005a638:	bf01                	c.j	2005a548 <at_cpuload+0x52>
2005a63a:	0049a683          	lw	a3,4(s3)
2005a63e:	0009a603          	lw	a2,0(s3)
2005a642:	20063537          	lui	a0,0x20063
2005a646:	85a2                	c.mv	a1,s0
2005a648:	cc050513          	addi	a0,a0,-832 # 20062cc0 <pmap_func+0x930>
2005a64c:	4d5040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a650:	4789                	c.li	a5,2
2005a652:	08f40363          	beq	s0,a5,2005a6d8 <at_cpuload+0x1e2>
2005a656:	0687e763          	bltu	a5,s0,2005a6c4 <at_cpuload+0x1ce>
2005a65a:	ec418793          	addi	a5,gp,-316 # 20066768 <top_exit_sema>
2005a65e:	e825                	c.bnez	s0,2005a6ce <at_cpuload+0x1d8>
2005a660:	439c                	c.lw	a5,0(a5)
2005a662:	e3a1                	c.bnez	a5,2005a6a2 <at_cpuload+0x1ac>
2005a664:	4088                	c.lw	a0,0(s1)
2005a666:	6609                	c.lui	a2,0x2
2005a668:	0641                	c.addi	a2,16
2005a66a:	4581                	c.li	a1,0
2005a66c:	dffaf097          	auipc	ra,0xdffaf
2005a670:	462080e7          	jalr	ra,1122(ra) # 9ace <__wrap_memset>
2005a674:	b4fff0ef          	jal	ra,2005a1c2 <update_status>
2005a678:	4605                	c.li	a2,1
2005a67a:	4581                	c.li	a1,0
2005a67c:	ec418513          	addi	a0,gp,-316 # 20066768 <top_exit_sema>
2005a680:	1b0010ef          	jal	ra,2005b830 <rtos_sema_create>
2005a684:	2005a637          	lui	a2,0x2005a
2005a688:	200635b7          	lui	a1,0x20063
2005a68c:	47a9                	c.li	a5,10
2005a68e:	6711                	c.lui	a4,0x4
2005a690:	eb818693          	addi	a3,gp,-328 # 2006675c <para_in>
2005a694:	43460613          	addi	a2,a2,1076 # 2005a434 <cpu_stat_thread>
2005a698:	cfc58593          	addi	a1,a1,-772 # 20062cfc <pmap_func+0x96c>
2005a69c:	4501                	c.li	a0,0
2005a69e:	4d4010ef          	jal	ra,2005bb72 <rtos_task_create>
2005a6a2:	20062537          	lui	a0,0x20062
2005a6a6:	d0ca0593          	addi	a1,s4,-756
2005a6aa:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
2005a6ae:	473040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a6b2:	50f6                	c.lwsp	ra,124(sp)
2005a6b4:	5466                	c.lwsp	s0,120(sp)
2005a6b6:	54d6                	c.lwsp	s1,116(sp)
2005a6b8:	5946                	c.lwsp	s2,112(sp)
2005a6ba:	59b6                	c.lwsp	s3,108(sp)
2005a6bc:	5a26                	c.lwsp	s4,104(sp)
2005a6be:	5a96                	c.lwsp	s5,100(sp)
2005a6c0:	6109                	c.addi16sp	sp,128
2005a6c2:	8082                	c.jr	ra
2005a6c4:	478d                	c.li	a5,3
2005a6c6:	02f40c63          	beq	s0,a5,2005a6fe <at_cpuload+0x208>
2005a6ca:	440d                	c.li	s0,3
2005a6cc:	bdb5                	c.j	2005a548 <at_cpuload+0x52>
2005a6ce:	4388                	c.lw	a0,0(a5)
2005a6d0:	d969                	c.beqz	a0,2005a6a2 <at_cpuload+0x1ac>
2005a6d2:	24c010ef          	jal	ra,2005b91e <rtos_sema_give>
2005a6d6:	b7f1                	c.j	2005a6a2 <at_cpuload+0x1ac>
2005a6d8:	ec41a783          	lw	a5,-316(gp) # 20066768 <top_exit_sema>
2005a6dc:	f3f9                	c.bnez	a5,2005a6a2 <at_cpuload+0x1ac>
2005a6de:	4088                	c.lw	a0,0(s1)
2005a6e0:	6609                	c.lui	a2,0x2
2005a6e2:	0641                	c.addi	a2,16
2005a6e4:	4581                	c.li	a1,0
2005a6e6:	dffaf097          	auipc	ra,0xdffaf
2005a6ea:	3e8080e7          	jalr	ra,1000(ra) # 9ace <__wrap_memset>
2005a6ee:	410187b7          	lui	a5,0x41018
2005a6f2:	4bd8                	c.lw	a4,20(a5)
2005a6f4:	eae1aa23          	sw	a4,-332(gp) # 20066758 <last_tick>
2005a6f8:	acbff0ef          	jal	ra,2005a1c2 <update_status>
2005a6fc:	b75d                	c.j	2005a6a2 <at_cpuload+0x1ac>
2005a6fe:	ec41a783          	lw	a5,-316(gp) # 20066768 <top_exit_sema>
2005a702:	f3c5                	c.bnez	a5,2005a6a2 <at_cpuload+0x1ac>
2005a704:	abfff0ef          	jal	ra,2005a1c2 <update_status>
2005a708:	41018937          	lui	s2,0x41018
2005a70c:	eb418413          	addi	s0,gp,-332 # 20066758 <last_tick>
2005a710:	401c                	c.lw	a5,0(s0)
2005a712:	01492503          	lw	a0,20(s2) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a716:	8d1d                	c.sub	a0,a5
2005a718:	3ed5                	c.jal	2005a30c <print_delta>
2005a71a:	01492783          	lw	a5,20(s2)
2005a71e:	4088                	c.lw	a0,0(s1)
2005a720:	c01c                	c.sw	a5,0(s0)
2005a722:	7ad000ef          	jal	ra,2005b6ce <rtos_mem_free>
2005a726:	0004a023          	sw	zero,0(s1)
2005a72a:	bfa5                	c.j	2005a6a2 <at_cpuload+0x1ac>

2005a72c <print_system_at>:
2005a72c:	1141                	c.addi	sp,-16
2005a72e:	c422                	c.swsp	s0,8(sp)
2005a730:	ca018413          	addi	s0,gp,-864 # 20066544 <at_sys_items>
2005a734:	c226                	c.swsp	s1,4(sp)
2005a736:	c04a                	c.swsp	s2,0(sp)
2005a738:	c606                	c.swsp	ra,12(sp)
2005a73a:	0b040493          	addi	s1,s0,176
2005a73e:	20063937          	lui	s2,0x20063
2005a742:	400c                	c.lw	a1,0(s0)
2005a744:	d9890513          	addi	a0,s2,-616 # 20062d98 <pmap_func+0xa08>
2005a748:	0441                	c.addi	s0,16
2005a74a:	3d7040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a74e:	fe941ae3          	bne	s0,s1,2005a742 <print_system_at+0x16>
2005a752:	40b2                	c.lwsp	ra,12(sp)
2005a754:	4422                	c.lwsp	s0,8(sp)
2005a756:	4492                	c.lwsp	s1,4(sp)
2005a758:	4902                	c.lwsp	s2,0(sp)
2005a75a:	0141                	c.addi	sp,16
2005a75c:	8082                	c.jr	ra

2005a75e <at_list>:
2005a75e:	20063537          	lui	a0,0x20063
2005a762:	1141                	c.addi	sp,-16
2005a764:	da050513          	addi	a0,a0,-608 # 20062da0 <pmap_func+0xa10>
2005a768:	c606                	c.swsp	ra,12(sp)
2005a76a:	3b7040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005a76e:	3f7d                	c.jal	2005a72c <print_system_at>
2005a770:	40b2                	c.lwsp	ra,12(sp)
2005a772:	200635b7          	lui	a1,0x20063
2005a776:	20062537          	lui	a0,0x20062
2005a77a:	db858593          	addi	a1,a1,-584 # 20062db8 <pmap_func+0xa28>
2005a77e:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x430>
2005a782:	0141                	c.addi	sp,16
2005a784:	39d0406f          	jal	zero,2005f320 <DiagPrintf_minimal>

2005a788 <at_sys_init>:
2005a788:	45ad                	c.li	a1,11
2005a78a:	ca018513          	addi	a0,gp,-864 # 20066544 <at_sys_items>
2005a78e:	0170006f          	jal	zero,2005afa4 <log_service_add_table>

2005a792 <BOOT_Image1>:
2005a792:	20000537          	lui	a0,0x20000
2005a796:	200017b7          	lui	a5,0x20001
2005a79a:	cac78793          	addi	a5,a5,-852 # 20000cac <__rom_bss_end__>
2005a79e:	00050613          	addi	a2,a0,0 # 20000000 <UserIrqFunTable>
2005a7a2:	1141                	c.addi	sp,-16
2005a7a4:	40c78633          	sub	a2,a5,a2
2005a7a8:	4581                	c.li	a1,0
2005a7aa:	00050513          	addi	a0,a0,0
2005a7ae:	c606                	c.swsp	ra,12(sp)
2005a7b0:	dffaf097          	auipc	ra,0xdffaf
2005a7b4:	fda080e7          	jalr	ra,-38(ra) # 978a <_memset>
2005a7b8:	dffa6097          	auipc	ra,0xdffa6
2005a7bc:	35e080e7          	jalr	ra,862(ra) # b16 <CPU_ClkGet>
2005a7c0:	000f47b7          	lui	a5,0xf4
2005a7c4:	24078793          	addi	a5,a5,576 # f4240 <__ap_sram_heap_size+0xb4240>
2005a7c8:	02f55533          	divu	a0,a0,a5
2005a7cc:	200017b7          	lui	a5,0x20001
2005a7d0:	86a7a023          	sw	a0,-1952(a5) # 20000860 <RBSS_UDELAY_CLK>
2005a7d4:	dffa6097          	auipc	ra,0xdffa6
2005a7d8:	150080e7          	jalr	ra,336(ra) # 924 <BOOT_ROM_InitDebugFlg>
2005a7dc:	200587b7          	lui	a5,0x20058
2005a7e0:	04078793          	addi	a5,a5,64 # 20058040 <Img2EntryFun0>
2005a7e4:	0017c703          	lbu	a4,1(a5)
2005a7e8:	0007c683          	lbu	a3,0(a5)
2005a7ec:	40b2                	c.lwsp	ra,12(sp)
2005a7ee:	0722                	c.slli	a4,0x8
2005a7f0:	8ed9                	c.or	a3,a4
2005a7f2:	0027c703          	lbu	a4,2(a5)
2005a7f6:	0037c783          	lbu	a5,3(a5)
2005a7fa:	0742                	c.slli	a4,0x10
2005a7fc:	8f55                	c.or	a4,a3
2005a7fe:	07e2                	c.slli	a5,0x18
2005a800:	8fd9                	c.or	a5,a4
2005a802:	0141                	c.addi	sp,16
2005a804:	8782                	c.jr	a5

2005a806 <flash_read_word>:
2005a806:	1141                	c.addi	sp,-16
2005a808:	852e                	c.mv	a0,a1
2005a80a:	4591                	c.li	a1,4
2005a80c:	c606                	c.swsp	ra,12(sp)
2005a80e:	b64fe0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005a812:	40b2                	c.lwsp	ra,12(sp)
2005a814:	4505                	c.li	a0,1
2005a816:	0141                	c.addi	sp,16
2005a818:	8082                	c.jr	ra

2005a81a <os_heap_init>:
2005a81a:	1141                	c.addi	sp,-16
2005a81c:	c606                	c.swsp	ra,12(sp)
2005a81e:	c422                	c.swsp	s0,8(sp)
2005a820:	0800                	c.addi4spn	s0,sp,16
2005a822:	00000793          	addi	a5,zero,0
2005a826:	85be                	c.mv	a1,a5
2005a828:	ca0007b7          	lui	a5,0xca000
2005a82c:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a830:	2869                	c.jal	2005a8ca <os_heap_add>
2005a832:	67dd                	c.lui	a5,0x17
2005a834:	66078793          	addi	a5,a5,1632 # 17660 <__bdram_heap_buffer_size__>
2005a838:	85be                	c.mv	a1,a5
2005a83a:	200697b7          	lui	a5,0x20069
2005a83e:	9a078513          	addi	a0,a5,-1632 # 200689a0 <__bdram_heap_buffer_start__>
2005a842:	2061                	c.jal	2005a8ca <os_heap_add>
2005a844:	00000793          	addi	a5,zero,0
2005a848:	85be                	c.mv	a1,a5
2005a84a:	ca0007b7          	lui	a5,0xca000
2005a84e:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a852:	28a5                	c.jal	2005a8ca <os_heap_add>
2005a854:	00000793          	addi	a5,zero,0
2005a858:	85be                	c.mv	a1,a5
2005a85a:	ca0007b7          	lui	a5,0xca000
2005a85e:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a862:	20a5                	c.jal	2005a8ca <os_heap_add>
2005a864:	67a1                	c.lui	a5,0x8
2005a866:	00078793          	addi	a5,a5,0 # 8000 <__sram_bootloader_mp_size__>
2005a86a:	85be                	c.mv	a1,a5
2005a86c:	200127b7          	lui	a5,0x20012
2005a870:	00078513          	addi	a0,a5,0 # 20012000 <__sram_bootloader_mp_start__>
2005a874:	2899                	c.jal	2005a8ca <os_heap_add>
2005a876:	ec81c783          	lbu	a5,-312(gp) # 2006676c <heap_index>
2005a87a:	c391                	c.beqz	a5,2005a87e <os_heap_init+0x64>
2005a87c:	2809                	c.jal	2005a88e <os_heap_sort>
2005a87e:	45c18513          	addi	a0,gp,1116 # 20066d00 <xHeapRegions>
2005a882:	2481                	c.jal	2005aac2 <vPortDefineHeapRegions>
2005a884:	0001                	c.addi	zero,0
2005a886:	40b2                	c.lwsp	ra,12(sp)
2005a888:	4422                	c.lwsp	s0,8(sp)
2005a88a:	0141                	c.addi	sp,16
2005a88c:	8082                	c.jr	ra

2005a88e <os_heap_sort>:
2005a88e:	4681                	c.li	a3,0
2005a890:	ec81c703          	lbu	a4,-312(gp) # 2006676c <heap_index>
2005a894:	177d                	c.addi	a4,-1
2005a896:	02e6d963          	bge	a3,a4,2005a8c8 <os_heap_sort+0x3a>
2005a89a:	45c18793          	addi	a5,gp,1116 # 20066d00 <xHeapRegions>
2005a89e:	4601                	c.li	a2,0
2005a8a0:	8f15                	c.sub	a4,a3
2005a8a2:	a839                	c.j	2005a8c0 <os_heap_sort+0x32>
2005a8a4:	438c                	c.lw	a1,0(a5)
2005a8a6:	4788                	c.lw	a0,8(a5)
2005a8a8:	0605                	c.addi	a2,1
2005a8aa:	00b57a63          	bgeu	a0,a1,2005a8be <os_heap_sort+0x30>
2005a8ae:	0047a303          	lw	t1,4(a5)
2005a8b2:	c388                	c.sw	a0,0(a5)
2005a8b4:	47c8                	c.lw	a0,12(a5)
2005a8b6:	c78c                	c.sw	a1,8(a5)
2005a8b8:	0067a623          	sw	t1,12(a5)
2005a8bc:	c3c8                	c.sw	a0,4(a5)
2005a8be:	07a1                	c.addi	a5,8
2005a8c0:	fee642e3          	blt	a2,a4,2005a8a4 <os_heap_sort+0x16>
2005a8c4:	0685                	c.addi	a3,1
2005a8c6:	b7e9                	c.j	2005a890 <os_heap_sort+0x2>
2005a8c8:	8082                	c.jr	ra

2005a8ca <os_heap_add>:
2005a8ca:	1141                	c.addi	sp,-16
2005a8cc:	c426                	c.swsp	s1,8(sp)
2005a8ce:	ec81c703          	lbu	a4,-312(gp) # 2006676c <heap_index>
2005a8d2:	c24a                	c.swsp	s2,4(sp)
2005a8d4:	c04e                	c.swsp	s3,0(sp)
2005a8d6:	c606                	c.swsp	ra,12(sp)
2005a8d8:	4795                	c.li	a5,5
2005a8da:	89aa                	c.mv	s3,a0
2005a8dc:	892e                	c.mv	s2,a1
2005a8de:	00e7fa63          	bgeu	a5,a4,2005a8f2 <os_heap_add+0x28>
2005a8e2:	20063537          	lui	a0,0x20063
2005a8e6:	02600593          	addi	a1,zero,38
2005a8ea:	e3450513          	addi	a0,a0,-460 # 20062e34 <__FUNCTION__.0+0x10>
2005a8ee:	739040ef          	jal	ra,2005f826 <io_assert_failed>
2005a8f2:	ec81c703          	lbu	a4,-312(gp) # 2006676c <heap_index>
2005a8f6:	45c18793          	addi	a5,gp,1116 # 20066d00 <xHeapRegions>
2005a8fa:	00371693          	slli	a3,a4,0x3
2005a8fe:	97b6                	c.add	a5,a3
2005a900:	0137a023          	sw	s3,0(a5)
2005a904:	0127a223          	sw	s2,4(a5)
2005a908:	4501                	c.li	a0,0
2005a90a:	00090c63          	beq	s2,zero,2005a922 <os_heap_add+0x58>
2005a90e:	ecc18793          	addi	a5,gp,-308 # 20066770 <total_heap_size>
2005a912:	438c                	c.lw	a1,0(a5)
2005a914:	0705                	c.addi	a4,1
2005a916:	ece18423          	sb	a4,-312(gp) # 2006676c <heap_index>
2005a91a:	992e                	c.add	s2,a1
2005a91c:	0127a023          	sw	s2,0(a5)
2005a920:	4505                	c.li	a0,1
2005a922:	40b2                	c.lwsp	ra,12(sp)
2005a924:	44a2                	c.lwsp	s1,8(sp)
2005a926:	4912                	c.lwsp	s2,4(sp)
2005a928:	4982                	c.lwsp	s3,0(sp)
2005a92a:	0141                	c.addi	sp,16
2005a92c:	8082                	c.jr	ra

2005a92e <__wrap_malloc>:
2005a92e:	5770006f          	jal	zero,2005b6a4 <rtos_mem_malloc>

2005a932 <__wrap_free>:
2005a932:	59d0006f          	jal	zero,2005b6ce <rtos_mem_free>

2005a936 <__wrap__free_r>:
2005a936:	852e                	c.mv	a0,a1
2005a938:	5970006f          	jal	zero,2005b6ce <rtos_mem_free>

2005a93c <prvInsertBlockIntoFreeList>:
2005a93c:	ee818793          	addi	a5,gp,-280 # 2006678c <xStart>
2005a940:	873e                	c.mv	a4,a5
2005a942:	439c                	c.lw	a5,0(a5)
2005a944:	fea7eee3          	bltu	a5,a0,2005a940 <prvInsertBlockIntoFreeList+0x4>
2005a948:	4350                	c.lw	a2,4(a4)
2005a94a:	00c706b3          	add	a3,a4,a2
2005a94e:	00d51663          	bne	a0,a3,2005a95a <prvInsertBlockIntoFreeList+0x1e>
2005a952:	4154                	c.lw	a3,4(a0)
2005a954:	853a                	c.mv	a0,a4
2005a956:	96b2                	c.add	a3,a2
2005a958:	c354                	c.sw	a3,4(a4)
2005a95a:	4154                	c.lw	a3,4(a0)
2005a95c:	00d50633          	add	a2,a0,a3
2005a960:	00c79b63          	bne	a5,a2,2005a976 <prvInsertBlockIntoFreeList+0x3a>
2005a964:	ed01a603          	lw	a2,-304(gp) # 20066774 <pxEnd>
2005a968:	00c78763          	beq	a5,a2,2005a976 <prvInsertBlockIntoFreeList+0x3a>
2005a96c:	43dc                	c.lw	a5,4(a5)
2005a96e:	97b6                	c.add	a5,a3
2005a970:	c15c                	c.sw	a5,4(a0)
2005a972:	431c                	c.lw	a5,0(a4)
2005a974:	439c                	c.lw	a5,0(a5)
2005a976:	c11c                	c.sw	a5,0(a0)
2005a978:	00e50363          	beq	a0,a4,2005a97e <prvInsertBlockIntoFreeList+0x42>
2005a97c:	c308                	c.sw	a0,0(a4)
2005a97e:	8082                	c.jr	ra

2005a980 <pvPortMalloc>:
2005a980:	1101                	c.addi	sp,-32
2005a982:	cc22                	c.swsp	s0,24(sp)
2005a984:	c452                	c.swsp	s4,8(sp)
2005a986:	842a                	c.mv	s0,a0
2005a988:	ce06                	c.swsp	ra,28(sp)
2005a98a:	ca26                	c.swsp	s1,20(sp)
2005a98c:	c84a                	c.swsp	s2,16(sp)
2005a98e:	c64e                	c.swsp	s3,12(sp)
2005a990:	4df020ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005a994:	ed41aa03          	lw	s4,-300(gp) # 20066778 <xBlockAllocatedBit>
2005a998:	008a77b3          	and	a5,s4,s0
2005a99c:	c799                	c.beqz	a5,2005a9aa <pvPortMalloc+0x2a>
2005a99e:	57e030ef          	jal	ra,2005df1c <xTaskResumeAll>
2005a9a2:	407010ef          	jal	ra,2005c5a8 <vApplicationMallocFailedHook>
2005a9a6:	4901                	c.li	s2,0
2005a9a8:	a879                	c.j	2005aa46 <pvPortMalloc+0xc6>
2005a9aa:	fff40793          	addi	a5,s0,-1
2005a9ae:	fde00713          	addi	a4,zero,-34
2005a9b2:	fef766e3          	bltu	a4,a5,2005a99e <pvPortMalloc+0x1e>
2005a9b6:	02040793          	addi	a5,s0,32
2005a9ba:	887d                	c.andi	s0,31
2005a9bc:	c801                	c.beqz	s0,2005a9cc <pvPortMalloc+0x4c>
2005a9be:	fe07f713          	andi	a4,a5,-32
2005a9c2:	02070713          	addi	a4,a4,32 # 4020 <GPIO_Direction+0x56>
2005a9c6:	fce7fce3          	bgeu	a5,a4,2005a99e <pvPortMalloc+0x1e>
2005a9ca:	87ba                	c.mv	a5,a4
2005a9cc:	ed81a483          	lw	s1,-296(gp) # 2006677c <xFreeBytesRemaining>
2005a9d0:	ed818993          	addi	s3,gp,-296 # 2006677c <xFreeBytesRemaining>
2005a9d4:	fcf4e5e3          	bltu	s1,a5,2005a99e <pvPortMalloc+0x1e>
2005a9d8:	ee81a403          	lw	s0,-280(gp) # 2006678c <xStart>
2005a9dc:	ee818713          	addi	a4,gp,-280 # 2006678c <xStart>
2005a9e0:	4054                	c.lw	a3,4(s0)
2005a9e2:	00f6f463          	bgeu	a3,a5,2005a9ea <pvPortMalloc+0x6a>
2005a9e6:	4014                	c.lw	a3,0(s0)
2005a9e8:	eaa5                	c.bnez	a3,2005aa58 <pvPortMalloc+0xd8>
2005a9ea:	ed01a683          	lw	a3,-304(gp) # 20066774 <pxEnd>
2005a9ee:	fa8688e3          	beq	a3,s0,2005a99e <pvPortMalloc+0x1e>
2005a9f2:	4014                	c.lw	a3,0(s0)
2005a9f4:	00072903          	lw	s2,0(a4)
2005a9f8:	c314                	c.sw	a3,0(a4)
2005a9fa:	4054                	c.lw	a3,4(s0)
2005a9fc:	04000713          	addi	a4,zero,64
2005aa00:	02090913          	addi	s2,s2,32
2005aa04:	8e9d                	c.sub	a3,a5
2005aa06:	00d77763          	bgeu	a4,a3,2005aa14 <pvPortMalloc+0x94>
2005aa0a:	00f40533          	add	a0,s0,a5
2005aa0e:	c154                	c.sw	a3,4(a0)
2005aa10:	c05c                	c.sw	a5,4(s0)
2005aa12:	372d                	c.jal	2005a93c <prvInsertBlockIntoFreeList>
2005aa14:	4058                	c.lw	a4,4(s0)
2005aa16:	edc18693          	addi	a3,gp,-292 # 20066780 <xMinimumEverFreeBytesRemaining>
2005aa1a:	4290                	c.lw	a2,0(a3)
2005aa1c:	40e487b3          	sub	a5,s1,a4
2005aa20:	00f9a023          	sw	a5,0(s3)
2005aa24:	00c7f363          	bgeu	a5,a2,2005aa2a <pvPortMalloc+0xaa>
2005aa28:	c29c                	c.sw	a5,0(a3)
2005aa2a:	00ea6733          	or	a4,s4,a4
2005aa2e:	ee018793          	addi	a5,gp,-288 # 20066784 <xNumberOfSuccessfulAllocations>
2005aa32:	c058                	c.sw	a4,4(s0)
2005aa34:	4398                	c.lw	a4,0(a5)
2005aa36:	00042023          	sw	zero,0(s0)
2005aa3a:	0705                	c.addi	a4,1
2005aa3c:	c398                	c.sw	a4,0(a5)
2005aa3e:	4de030ef          	jal	ra,2005df1c <xTaskResumeAll>
2005aa42:	f60900e3          	beq	s2,zero,2005a9a2 <pvPortMalloc+0x22>
2005aa46:	40f2                	c.lwsp	ra,28(sp)
2005aa48:	4462                	c.lwsp	s0,24(sp)
2005aa4a:	44d2                	c.lwsp	s1,20(sp)
2005aa4c:	49b2                	c.lwsp	s3,12(sp)
2005aa4e:	4a22                	c.lwsp	s4,8(sp)
2005aa50:	854a                	c.mv	a0,s2
2005aa52:	4942                	c.lwsp	s2,16(sp)
2005aa54:	6105                	c.addi16sp	sp,32
2005aa56:	8082                	c.jr	ra
2005aa58:	8722                	c.mv	a4,s0
2005aa5a:	8436                	c.mv	s0,a3
2005aa5c:	b751                	c.j	2005a9e0 <pvPortMalloc+0x60>

2005aa5e <vPortFree>:
2005aa5e:	cd31                	c.beqz	a0,2005aaba <vPortFree+0x5c>
2005aa60:	1141                	c.addi	sp,-16
2005aa62:	c422                	c.swsp	s0,8(sp)
2005aa64:	c606                	c.swsp	ra,12(sp)
2005aa66:	fe452703          	lw	a4,-28(a0)
2005aa6a:	ed41a783          	lw	a5,-300(gp) # 20066778 <xBlockAllocatedBit>
2005aa6e:	842a                	c.mv	s0,a0
2005aa70:	00f776b3          	and	a3,a4,a5
2005aa74:	ce9d                	c.beqz	a3,2005aab2 <vPortFree+0x54>
2005aa76:	fe052683          	lw	a3,-32(a0)
2005aa7a:	ee85                	c.bnez	a3,2005aab2 <vPortFree+0x54>
2005aa7c:	fff7c793          	xori	a5,a5,-1
2005aa80:	8ff9                	c.and	a5,a4
2005aa82:	fef52223          	sw	a5,-28(a0)
2005aa86:	3e9020ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005aa8a:	ed818793          	addi	a5,gp,-296 # 2006677c <xFreeBytesRemaining>
2005aa8e:	4394                	c.lw	a3,0(a5)
2005aa90:	fe442703          	lw	a4,-28(s0)
2005aa94:	fe040513          	addi	a0,s0,-32
2005aa98:	9736                	c.add	a4,a3
2005aa9a:	c398                	c.sw	a4,0(a5)
2005aa9c:	3545                	c.jal	2005a93c <prvInsertBlockIntoFreeList>
2005aa9e:	ee418793          	addi	a5,gp,-284 # 20066788 <xNumberOfSuccessfulFrees>
2005aaa2:	4398                	c.lw	a4,0(a5)
2005aaa4:	0705                	c.addi	a4,1
2005aaa6:	c398                	c.sw	a4,0(a5)
2005aaa8:	4422                	c.lwsp	s0,8(sp)
2005aaaa:	40b2                	c.lwsp	ra,12(sp)
2005aaac:	0141                	c.addi	sp,16
2005aaae:	46e0306f          	jal	zero,2005df1c <xTaskResumeAll>
2005aab2:	40b2                	c.lwsp	ra,12(sp)
2005aab4:	4422                	c.lwsp	s0,8(sp)
2005aab6:	0141                	c.addi	sp,16
2005aab8:	8082                	c.jr	ra
2005aaba:	8082                	c.jr	ra

2005aabc <xPortGetFreeHeapSize>:
2005aabc:	ed81a503          	lw	a0,-296(gp) # 2006677c <xFreeBytesRemaining>
2005aac0:	8082                	c.jr	ra

2005aac2 <vPortDefineHeapRegions>:
2005aac2:	ed01a583          	lw	a1,-304(gp) # 20066774 <pxEnd>
2005aac6:	4701                	c.li	a4,0
2005aac8:	4301                	c.li	t1,0
2005aaca:	4681                	c.li	a3,0
2005aacc:	ed018613          	addi	a2,gp,-304 # 20066774 <pxEnd>
2005aad0:	ee818813          	addi	a6,gp,-280 # 2006678c <xStart>
2005aad4:	415c                	c.lw	a5,4(a0)
2005aad6:	ef81                	c.bnez	a5,2005aaee <vPortDefineHeapRegions+0x2c>
2005aad8:	c311                	c.beqz	a4,2005aadc <vPortDefineHeapRegions+0x1a>
2005aada:	c20c                	c.sw	a1,0(a2)
2005aadc:	ecd1ae23          	sw	a3,-292(gp) # 20066780 <xMinimumEverFreeBytesRemaining>
2005aae0:	ecd1ac23          	sw	a3,-296(gp) # 2006677c <xFreeBytesRemaining>
2005aae4:	80000737          	lui	a4,0x80000
2005aae8:	ece1aa23          	sw	a4,-300(gp) # 20066778 <xBlockAllocatedBit>
2005aaec:	8082                	c.jr	ra
2005aaee:	4118                	c.lw	a4,0(a0)
2005aaf0:	01f77893          	andi	a7,a4,31
2005aaf4:	00088a63          	beq	a7,zero,2005ab08 <vPortDefineHeapRegions+0x46>
2005aaf8:	01f70893          	addi	a7,a4,31 # 8000001f <__ctrace_end__+0xb600001f>
2005aafc:	fe08f893          	andi	a7,a7,-32
2005ab00:	97ba                	c.add	a5,a4
2005ab02:	411787b3          	sub	a5,a5,a7
2005ab06:	8746                	c.mv	a4,a7
2005ab08:	00031663          	bne	t1,zero,2005ab14 <vPortDefineHeapRegions+0x52>
2005ab0c:	00e82023          	sw	a4,0(a6)
2005ab10:	00082223          	sw	zero,4(a6)
2005ab14:	97ba                	c.add	a5,a4
2005ab16:	1781                	c.addi	a5,-32
2005ab18:	9b81                	c.andi	a5,-32
2005ab1a:	0007a223          	sw	zero,4(a5)
2005ab1e:	0007a023          	sw	zero,0(a5)
2005ab22:	40e788b3          	sub	a7,a5,a4
2005ab26:	01172223          	sw	a7,4(a4)
2005ab2a:	c31c                	c.sw	a5,0(a4)
2005ab2c:	c191                	c.beqz	a1,2005ab30 <vPortDefineHeapRegions+0x6e>
2005ab2e:	c198                	c.sw	a4,0(a1)
2005ab30:	4358                	c.lw	a4,4(a4)
2005ab32:	0305                	c.addi	t1,1
2005ab34:	0521                	c.addi	a0,8
2005ab36:	96ba                	c.add	a3,a4
2005ab38:	85be                	c.mv	a1,a5
2005ab3a:	4705                	c.li	a4,1
2005ab3c:	bf61                	c.j	2005aad4 <vPortDefineHeapRegions+0x12>

2005ab3e <vPortGetHeapStats>:
2005ab3e:	1101                	c.addi	sp,-32
2005ab40:	cc22                	c.swsp	s0,24(sp)
2005ab42:	ce06                	c.swsp	ra,28(sp)
2005ab44:	ca26                	c.swsp	s1,20(sp)
2005ab46:	c84a                	c.swsp	s2,16(sp)
2005ab48:	c64e                	c.swsp	s3,12(sp)
2005ab4a:	842a                	c.mv	s0,a0
2005ab4c:	323020ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005ab50:	ee81a783          	lw	a5,-280(gp) # 2006678c <xStart>
2005ab54:	cfb9                	c.beqz	a5,2005abb2 <vPortGetHeapStats+0x74>
2005ab56:	ed01a683          	lw	a3,-304(gp) # 20066774 <pxEnd>
2005ab5a:	597d                	c.li	s2,-1
2005ab5c:	4981                	c.li	s3,0
2005ab5e:	4481                	c.li	s1,0
2005ab60:	43d8                	c.lw	a4,4(a5)
2005ab62:	0485                	c.addi	s1,1
2005ab64:	00e9f363          	bgeu	s3,a4,2005ab6a <vPortGetHeapStats+0x2c>
2005ab68:	89ba                	c.mv	s3,a4
2005ab6a:	c701                	c.beqz	a4,2005ab72 <vPortGetHeapStats+0x34>
2005ab6c:	01277363          	bgeu	a4,s2,2005ab72 <vPortGetHeapStats+0x34>
2005ab70:	893a                	c.mv	s2,a4
2005ab72:	439c                	c.lw	a5,0(a5)
2005ab74:	fef696e3          	bne	a3,a5,2005ab60 <vPortGetHeapStats+0x22>
2005ab78:	3a4030ef          	jal	ra,2005df1c <xTaskResumeAll>
2005ab7c:	01342223          	sw	s3,4(s0)
2005ab80:	01242423          	sw	s2,8(s0)
2005ab84:	c444                	c.sw	s1,12(s0)
2005ab86:	00e030ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005ab8a:	ed81a783          	lw	a5,-296(gp) # 2006677c <xFreeBytesRemaining>
2005ab8e:	40f2                	c.lwsp	ra,28(sp)
2005ab90:	44d2                	c.lwsp	s1,20(sp)
2005ab92:	c01c                	c.sw	a5,0(s0)
2005ab94:	ee01a783          	lw	a5,-288(gp) # 20066784 <xNumberOfSuccessfulAllocations>
2005ab98:	4942                	c.lwsp	s2,16(sp)
2005ab9a:	49b2                	c.lwsp	s3,12(sp)
2005ab9c:	c85c                	c.sw	a5,20(s0)
2005ab9e:	ee41a783          	lw	a5,-284(gp) # 20066788 <xNumberOfSuccessfulFrees>
2005aba2:	cc1c                	c.sw	a5,24(s0)
2005aba4:	edc1a783          	lw	a5,-292(gp) # 20066780 <xMinimumEverFreeBytesRemaining>
2005aba8:	c81c                	c.sw	a5,16(s0)
2005abaa:	4462                	c.lwsp	s0,24(sp)
2005abac:	6105                	c.addi16sp	sp,32
2005abae:	0000306f          	jal	zero,2005dbae <vTaskExitCritical>
2005abb2:	597d                	c.li	s2,-1
2005abb4:	4981                	c.li	s3,0
2005abb6:	4481                	c.li	s1,0
2005abb8:	b7c1                	c.j	2005ab78 <vPortGetHeapStats+0x3a>

2005abba <vListInitialise>:
2005abba:	00850793          	addi	a5,a0,8
2005abbe:	577d                	c.li	a4,-1
2005abc0:	c15c                	c.sw	a5,4(a0)
2005abc2:	c518                	c.sw	a4,8(a0)
2005abc4:	c55c                	c.sw	a5,12(a0)
2005abc6:	c91c                	c.sw	a5,16(a0)
2005abc8:	00052023          	sw	zero,0(a0)
2005abcc:	8082                	c.jr	ra

2005abce <vListInitialiseItem>:
2005abce:	00052823          	sw	zero,16(a0)
2005abd2:	8082                	c.jr	ra

2005abd4 <vListInsertEnd>:
2005abd4:	415c                	c.lw	a5,4(a0)
2005abd6:	c1dc                	c.sw	a5,4(a1)
2005abd8:	4798                	c.lw	a4,8(a5)
2005abda:	c598                	c.sw	a4,8(a1)
2005abdc:	c34c                	c.sw	a1,4(a4)
2005abde:	c78c                	c.sw	a1,8(a5)
2005abe0:	c988                	c.sw	a0,16(a1)
2005abe2:	411c                	c.lw	a5,0(a0)
2005abe4:	0785                	c.addi	a5,1
2005abe6:	c11c                	c.sw	a5,0(a0)
2005abe8:	8082                	c.jr	ra

2005abea <vListInsert>:
2005abea:	4194                	c.lw	a3,0(a1)
2005abec:	57fd                	c.li	a5,-1
2005abee:	00850713          	addi	a4,a0,8
2005abf2:	00f69d63          	bne	a3,a5,2005ac0c <vListInsert+0x22>
2005abf6:	491c                	c.lw	a5,16(a0)
2005abf8:	43d8                	c.lw	a4,4(a5)
2005abfa:	c1d8                	c.sw	a4,4(a1)
2005abfc:	c70c                	c.sw	a1,8(a4)
2005abfe:	c59c                	c.sw	a5,8(a1)
2005ac00:	c3cc                	c.sw	a1,4(a5)
2005ac02:	c988                	c.sw	a0,16(a1)
2005ac04:	411c                	c.lw	a5,0(a0)
2005ac06:	0785                	c.addi	a5,1
2005ac08:	c11c                	c.sw	a5,0(a0)
2005ac0a:	8082                	c.jr	ra
2005ac0c:	87ba                	c.mv	a5,a4
2005ac0e:	4358                	c.lw	a4,4(a4)
2005ac10:	4310                	c.lw	a2,0(a4)
2005ac12:	fec6fde3          	bgeu	a3,a2,2005ac0c <vListInsert+0x22>
2005ac16:	b7cd                	c.j	2005abf8 <vListInsert+0xe>

2005ac18 <uxListRemove>:
2005ac18:	4154                	c.lw	a3,4(a0)
2005ac1a:	4518                	c.lw	a4,8(a0)
2005ac1c:	491c                	c.lw	a5,16(a0)
2005ac1e:	c698                	c.sw	a4,8(a3)
2005ac20:	4154                	c.lw	a3,4(a0)
2005ac22:	c354                	c.sw	a3,4(a4)
2005ac24:	43d8                	c.lw	a4,4(a5)
2005ac26:	00a71463          	bne	a4,a0,2005ac2e <uxListRemove+0x16>
2005ac2a:	4518                	c.lw	a4,8(a0)
2005ac2c:	c3d8                	c.sw	a4,4(a5)
2005ac2e:	00052823          	sw	zero,16(a0)
2005ac32:	4398                	c.lw	a4,0(a5)
2005ac34:	177d                	c.addi	a4,-1
2005ac36:	c398                	c.sw	a4,0(a5)
2005ac38:	4388                	c.lw	a0,0(a5)
2005ac3a:	8082                	c.jr	ra

2005ac3c <rtk_log_array_print>:
2005ac3c:	1101                	c.addi	sp,-32
2005ac3e:	cc22                	c.swsp	s0,24(sp)
2005ac40:	c84a                	c.swsp	s2,16(sp)
2005ac42:	ce06                	c.swsp	ra,28(sp)
2005ac44:	ca26                	c.swsp	s1,20(sp)
2005ac46:	c64e                	c.swsp	s3,12(sp)
2005ac48:	ef018793          	addi	a5,gp,-272 # 20066794 <rtk_log_entry_count>
2005ac4c:	4394                	c.lw	a3,0(a5)
2005ac4e:	470d                	c.li	a4,3
2005ac50:	842a                	c.mv	s0,a0
2005ac52:	4911                	c.li	s2,4
2005ac54:	00d76463          	bltu	a4,a3,2005ac5c <rtk_log_array_print+0x20>
2005ac58:	0007a903          	lw	s2,0(a5)
2005ac5c:	557d                	c.li	a0,-1
2005ac5e:	c801                	c.beqz	s0,2005ac6e <rtk_log_array_print+0x32>
2005ac60:	0405                	c.addi	s0,1
2005ac62:	4481                	c.li	s1,0
2005ac64:	200639b7          	lui	s3,0x20063
2005ac68:	01249a63          	bne	s1,s2,2005ac7c <rtk_log_array_print+0x40>
2005ac6c:	4501                	c.li	a0,0
2005ac6e:	40f2                	c.lwsp	ra,28(sp)
2005ac70:	4462                	c.lwsp	s0,24(sp)
2005ac72:	44d2                	c.lwsp	s1,20(sp)
2005ac74:	4942                	c.lwsp	s2,16(sp)
2005ac76:	49b2                	c.lwsp	s3,12(sp)
2005ac78:	6105                	c.addi16sp	sp,32
2005ac7a:	8082                	c.jr	ra
2005ac7c:	fff44603          	lbu	a2,-1(s0)
2005ac80:	85a2                	c.mv	a1,s0
2005ac82:	ed098513          	addi	a0,s3,-304 # 20062ed0 <__FUNCTION__.0+0xac>
2005ac86:	69a040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005ac8a:	0485                	c.addi	s1,1
2005ac8c:	042d                	c.addi	s0,11
2005ac8e:	bfe9                	c.j	2005ac68 <rtk_log_array_print+0x2c>

2005ac90 <rtk_log_array_clear>:
2005ac90:	1141                	c.addi	sp,-16
2005ac92:	02c00613          	addi	a2,zero,44
2005ac96:	4581                	c.li	a1,0
2005ac98:	48c18513          	addi	a0,gp,1164 # 20066d30 <rtk_log_tag_array>
2005ac9c:	c606                	c.swsp	ra,12(sp)
2005ac9e:	dffaf097          	auipc	ra,0xdffaf
2005aca2:	aec080e7          	jalr	ra,-1300(ra) # 978a <_memset>
2005aca6:	40b2                	c.lwsp	ra,12(sp)
2005aca8:	ee01a823          	sw	zero,-272(gp) # 20066794 <rtk_log_entry_count>
2005acac:	0141                	c.addi	sp,16
2005acae:	8082                	c.jr	ra

2005acb0 <rtk_log_level_get>:
2005acb0:	1101                	c.addi	sp,-32
2005acb2:	c64e                	c.swsp	s3,12(sp)
2005acb4:	c452                	c.swsp	s4,8(sp)
2005acb6:	ce06                	c.swsp	ra,28(sp)
2005acb8:	cc22                	c.swsp	s0,24(sp)
2005acba:	ca26                	c.swsp	s1,20(sp)
2005acbc:	c84a                	c.swsp	s2,16(sp)
2005acbe:	ef018793          	addi	a5,gp,-272 # 20066794 <rtk_log_entry_count>
2005acc2:	4394                	c.lw	a3,0(a5)
2005acc4:	470d                	c.li	a4,3
2005acc6:	89aa                	c.mv	s3,a0
2005acc8:	4a11                	c.li	s4,4
2005acca:	00d76463          	bltu	a4,a3,2005acd2 <rtk_log_level_get+0x22>
2005acce:	0007aa03          	lw	s4,0(a5)
2005acd2:	00099a63          	bne	s3,zero,2005ace6 <rtk_log_level_get+0x36>
2005acd6:	20063537          	lui	a0,0x20063
2005acda:	05000593          	addi	a1,zero,80
2005acde:	ee450513          	addi	a0,a0,-284 # 20062ee4 <__FUNCTION__.0+0xc0>
2005ace2:	345040ef          	jal	ra,2005f826 <io_assert_failed>
2005ace6:	48c18913          	addi	s2,gp,1164 # 20066d30 <rtk_log_tag_array>
2005acea:	0905                	c.addi	s2,1
2005acec:	4401                	c.li	s0,0
2005acee:	48c18493          	addi	s1,gp,1164 # 20066d30 <rtk_log_tag_array>
2005acf2:	01441563          	bne	s0,s4,2005acfc <rtk_log_level_get+0x4c>
2005acf6:	e7c1c503          	lbu	a0,-388(gp) # 20066720 <rtk_log_default_level>
2005acfa:	a839                	c.j	2005ad18 <rtk_log_level_get+0x68>
2005acfc:	854a                	c.mv	a0,s2
2005acfe:	85ce                	c.mv	a1,s3
2005ad00:	dffaf097          	auipc	ra,0xdffaf
2005ad04:	b2c080e7          	jalr	ra,-1236(ra) # 982c <_strcmp>
2005ad08:	092d                	c.addi	s2,11
2005ad0a:	ed19                	c.bnez	a0,2005ad28 <rtk_log_level_get+0x78>
2005ad0c:	47ad                	c.li	a5,11
2005ad0e:	02f40433          	mul	s0,s0,a5
2005ad12:	9426                	c.add	s0,s1
2005ad14:	00044503          	lbu	a0,0(s0)
2005ad18:	40f2                	c.lwsp	ra,28(sp)
2005ad1a:	4462                	c.lwsp	s0,24(sp)
2005ad1c:	44d2                	c.lwsp	s1,20(sp)
2005ad1e:	4942                	c.lwsp	s2,16(sp)
2005ad20:	49b2                	c.lwsp	s3,12(sp)
2005ad22:	4a22                	c.lwsp	s4,8(sp)
2005ad24:	6105                	c.addi16sp	sp,32
2005ad26:	8082                	c.jr	ra
2005ad28:	0405                	c.addi	s0,1
2005ad2a:	b7e1                	c.j	2005acf2 <rtk_log_level_get+0x42>

2005ad2c <rtk_log_level_set>:
2005ad2c:	1101                	c.addi	sp,-32
2005ad2e:	cc22                	c.swsp	s0,24(sp)
2005ad30:	ef018793          	addi	a5,gp,-272 # 20066794 <rtk_log_entry_count>
2005ad34:	c84a                	c.swsp	s2,16(sp)
2005ad36:	c64e                	c.swsp	s3,12(sp)
2005ad38:	c05a                	c.swsp	s6,0(sp)
2005ad3a:	ce06                	c.swsp	ra,28(sp)
2005ad3c:	ca26                	c.swsp	s1,20(sp)
2005ad3e:	c452                	c.swsp	s4,8(sp)
2005ad40:	c256                	c.swsp	s5,4(sp)
2005ad42:	4398                	c.lw	a4,0(a5)
2005ad44:	478d                	c.li	a5,3
2005ad46:	892a                	c.mv	s2,a0
2005ad48:	89ae                	c.mv	s3,a1
2005ad4a:	ef018413          	addi	s0,gp,-272 # 20066794 <rtk_log_entry_count>
2005ad4e:	4b11                	c.li	s6,4
2005ad50:	00e7e463          	bltu	a5,a4,2005ad58 <rtk_log_level_set+0x2c>
2005ad54:	00042b03          	lw	s6,0(s0)
2005ad58:	557d                	c.li	a0,-1
2005ad5a:	06090b63          	beq	s2,zero,2005add0 <rtk_log_level_set+0xa4>
2005ad5e:	4795                	c.li	a5,5
2005ad60:	0737e863          	bltu	a5,s3,2005add0 <rtk_log_level_set+0xa4>
2005ad64:	200635b7          	lui	a1,0x20063
2005ad68:	f7858593          	addi	a1,a1,-136 # 20062f78 <__FUNCTION__.0+0x154>
2005ad6c:	854a                	c.mv	a0,s2
2005ad6e:	dffaf097          	auipc	ra,0xdffaf
2005ad72:	abe080e7          	jalr	ra,-1346(ra) # 982c <_strcmp>
2005ad76:	c939                	c.beqz	a0,2005adcc <rtk_log_level_set+0xa0>
2005ad78:	48c18a93          	addi	s5,gp,1164 # 20066d30 <rtk_log_tag_array>
2005ad7c:	0a85                	c.addi	s5,1
2005ad7e:	4a01                	c.li	s4,0
2005ad80:	48c18493          	addi	s1,gp,1164 # 20066d30 <rtk_log_tag_array>
2005ad84:	076a1063          	bne	s4,s6,2005ade4 <rtk_log_level_set+0xb8>
2005ad88:	4018                	c.lw	a4,0(s0)
2005ad8a:	478d                	c.li	a5,3
2005ad8c:	00e7f863          	bgeu	a5,a4,2005ad9c <rtk_log_level_set+0x70>
2005ad90:	20063537          	lui	a0,0x20063
2005ad94:	f7c50513          	addi	a0,a0,-132 # 20062f7c <__FUNCTION__.0+0x158>
2005ad98:	588040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005ad9c:	4018                	c.lw	a4,0(s0)
2005ad9e:	401c                	c.lw	a5,0(s0)
2005ada0:	452d                	c.li	a0,11
2005ada2:	8b0d                	c.andi	a4,3
2005ada4:	8b8d                	c.andi	a5,3
2005ada6:	02a70733          	mul	a4,a4,a0
2005adaa:	4625                	c.li	a2,9
2005adac:	85ca                	c.mv	a1,s2
2005adae:	02a787b3          	mul	a5,a5,a0
2005adb2:	9726                	c.add	a4,s1
2005adb4:	01370023          	sb	s3,0(a4)
2005adb8:	0785                	c.addi	a5,1
2005adba:	00f48533          	add	a0,s1,a5
2005adbe:	791060ef          	jal	ra,20061d4e <strncpy>
2005adc2:	401c                	c.lw	a5,0(s0)
2005adc4:	4501                	c.li	a0,0
2005adc6:	0785                	c.addi	a5,1
2005adc8:	c01c                	c.sw	a5,0(s0)
2005adca:	a019                	c.j	2005add0 <rtk_log_level_set+0xa4>
2005adcc:	e7318e23          	sb	s3,-388(gp) # 20066720 <rtk_log_default_level>
2005add0:	40f2                	c.lwsp	ra,28(sp)
2005add2:	4462                	c.lwsp	s0,24(sp)
2005add4:	44d2                	c.lwsp	s1,20(sp)
2005add6:	4942                	c.lwsp	s2,16(sp)
2005add8:	49b2                	c.lwsp	s3,12(sp)
2005adda:	4a22                	c.lwsp	s4,8(sp)
2005addc:	4a92                	c.lwsp	s5,4(sp)
2005adde:	4b02                	c.lwsp	s6,0(sp)
2005ade0:	6105                	c.addi16sp	sp,32
2005ade2:	8082                	c.jr	ra
2005ade4:	8556                	c.mv	a0,s5
2005ade6:	85ca                	c.mv	a1,s2
2005ade8:	dffaf097          	auipc	ra,0xdffaf
2005adec:	a44080e7          	jalr	ra,-1468(ra) # 982c <_strcmp>
2005adf0:	0aad                	c.addi	s5,11
2005adf2:	e901                	c.bnez	a0,2005ae02 <rtk_log_level_set+0xd6>
2005adf4:	47ad                	c.li	a5,11
2005adf6:	02fa0a33          	mul	s4,s4,a5
2005adfa:	94d2                	c.add	s1,s4
2005adfc:	01348023          	sb	s3,0(s1)
2005ae00:	bfc1                	c.j	2005add0 <rtk_log_level_set+0xa4>
2005ae02:	0a05                	c.addi	s4,1
2005ae04:	b741                	c.j	2005ad84 <rtk_log_level_set+0x58>

2005ae06 <rtk_log_memory_dump_word>:
2005ae06:	1101                	c.addi	sp,-32
2005ae08:	cc22                	c.swsp	s0,24(sp)
2005ae0a:	ca26                	c.swsp	s1,20(sp)
2005ae0c:	c84a                	c.swsp	s2,16(sp)
2005ae0e:	c64e                	c.swsp	s3,12(sp)
2005ae10:	c452                	c.swsp	s4,8(sp)
2005ae12:	c256                	c.swsp	s5,4(sp)
2005ae14:	c05a                	c.swsp	s6,0(sp)
2005ae16:	ce06                	c.swsp	ra,28(sp)
2005ae18:	892a                	c.mv	s2,a0
2005ae1a:	89ae                	c.mv	s3,a1
2005ae1c:	84aa                	c.mv	s1,a0
2005ae1e:	4401                	c.li	s0,0
2005ae20:	20063a37          	lui	s4,0x20063
2005ae24:	20063ab7          	lui	s5,0x20063
2005ae28:	20063b37          	lui	s6,0x20063
2005ae2c:	03341163          	bne	s0,s3,2005ae4e <rtk_log_memory_dump_word+0x48>
2005ae30:	4462                	c.lwsp	s0,24(sp)
2005ae32:	40f2                	c.lwsp	ra,28(sp)
2005ae34:	44d2                	c.lwsp	s1,20(sp)
2005ae36:	4942                	c.lwsp	s2,16(sp)
2005ae38:	49b2                	c.lwsp	s3,12(sp)
2005ae3a:	4a22                	c.lwsp	s4,8(sp)
2005ae3c:	4a92                	c.lwsp	s5,4(sp)
2005ae3e:	4b02                	c.lwsp	s6,0(sp)
2005ae40:	20063537          	lui	a0,0x20063
2005ae44:	81050513          	addi	a0,a0,-2032 # 20062810 <pmap_func+0x480>
2005ae48:	6105                	c.addi16sp	sp,32
2005ae4a:	4d60406f          	jal	zero,2005f320 <DiagPrintf_minimal>
2005ae4e:	ec11                	c.bnez	s0,2005ae6a <rtk_log_memory_dump_word+0x64>
2005ae50:	85ca                	c.mv	a1,s2
2005ae52:	fa8a8513          	addi	a0,s5,-88 # 20062fa8 <__FUNCTION__.0+0x184>
2005ae56:	4ca040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005ae5a:	408c                	c.lw	a1,0(s1)
2005ae5c:	fbcb0513          	addi	a0,s6,-68 # 20062fbc <__FUNCTION__.0+0x198>
2005ae60:	0405                	c.addi	s0,1
2005ae62:	4be040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005ae66:	0491                	c.addi	s1,4
2005ae68:	b7d1                	c.j	2005ae2c <rtk_log_memory_dump_word+0x26>
2005ae6a:	00747793          	andi	a5,s0,7
2005ae6e:	f7f5                	c.bnez	a5,2005ae5a <rtk_log_memory_dump_word+0x54>
2005ae70:	85a6                	c.mv	a1,s1
2005ae72:	fb0a0513          	addi	a0,s4,-80 # 20062fb0 <__FUNCTION__.0+0x18c>
2005ae76:	b7c5                	c.j	2005ae56 <rtk_log_memory_dump_word+0x50>

2005ae78 <rtk_log_memory_dump_byte>:
2005ae78:	1101                	c.addi	sp,-32
2005ae7a:	cc22                	c.swsp	s0,24(sp)
2005ae7c:	ca26                	c.swsp	s1,20(sp)
2005ae7e:	c84a                	c.swsp	s2,16(sp)
2005ae80:	c452                	c.swsp	s4,8(sp)
2005ae82:	c256                	c.swsp	s5,4(sp)
2005ae84:	c05a                	c.swsp	s6,0(sp)
2005ae86:	ce06                	c.swsp	ra,28(sp)
2005ae88:	c64e                	c.swsp	s3,12(sp)
2005ae8a:	84aa                	c.mv	s1,a0
2005ae8c:	892e                	c.mv	s2,a1
2005ae8e:	4401                	c.li	s0,0
2005ae90:	20063a37          	lui	s4,0x20063
2005ae94:	20063ab7          	lui	s5,0x20063
2005ae98:	20063b37          	lui	s6,0x20063
2005ae9c:	03241163          	bne	s0,s2,2005aebe <rtk_log_memory_dump_byte+0x46>
2005aea0:	4462                	c.lwsp	s0,24(sp)
2005aea2:	40f2                	c.lwsp	ra,28(sp)
2005aea4:	44d2                	c.lwsp	s1,20(sp)
2005aea6:	4942                	c.lwsp	s2,16(sp)
2005aea8:	49b2                	c.lwsp	s3,12(sp)
2005aeaa:	4a22                	c.lwsp	s4,8(sp)
2005aeac:	4a92                	c.lwsp	s5,4(sp)
2005aeae:	4b02                	c.lwsp	s6,0(sp)
2005aeb0:	20063537          	lui	a0,0x20063
2005aeb4:	81050513          	addi	a0,a0,-2032 # 20062810 <pmap_func+0x480>
2005aeb8:	6105                	c.addi16sp	sp,32
2005aeba:	4660406f          	jal	zero,2005f320 <DiagPrintf_minimal>
2005aebe:	008489b3          	add	s3,s1,s0
2005aec2:	ec11                	c.bnez	s0,2005aede <rtk_log_memory_dump_byte+0x66>
2005aec4:	85a6                	c.mv	a1,s1
2005aec6:	fa8a8513          	addi	a0,s5,-88 # 20062fa8 <__FUNCTION__.0+0x184>
2005aeca:	456040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005aece:	0009c583          	lbu	a1,0(s3)
2005aed2:	fc4b0513          	addi	a0,s6,-60 # 20062fc4 <__FUNCTION__.0+0x1a0>
2005aed6:	0405                	c.addi	s0,1
2005aed8:	448040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005aedc:	b7c1                	c.j	2005ae9c <rtk_log_memory_dump_byte+0x24>
2005aede:	00747793          	andi	a5,s0,7
2005aee2:	f7f5                	c.bnez	a5,2005aece <rtk_log_memory_dump_byte+0x56>
2005aee4:	85ce                	c.mv	a1,s3
2005aee6:	fb0a0513          	addi	a0,s4,-80 # 20062fb0 <__FUNCTION__.0+0x18c>
2005aeea:	b7c5                	c.j	2005aeca <rtk_log_memory_dump_byte+0x52>

2005aeec <rtk_log_write>:
2005aeec:	7139                	c.addi16sp	sp,-64
2005aeee:	d226                	c.swsp	s1,36(sp)
2005aef0:	84aa                	c.mv	s1,a0
2005aef2:	852e                	c.mv	a0,a1
2005aef4:	d422                	c.swsp	s0,40(sp)
2005aef6:	d606                	c.swsp	ra,44(sp)
2005aef8:	c632                	c.swsp	a2,12(sp)
2005aefa:	8436                	c.mv	s0,a3
2005aefc:	d83a                	c.swsp	a4,48(sp)
2005aefe:	da3e                	c.swsp	a5,52(sp)
2005af00:	dc42                	c.swsp	a6,56(sp)
2005af02:	de46                	c.swsp	a7,60(sp)
2005af04:	c42e                	c.swsp	a1,8(sp)
2005af06:	336d                	c.jal	2005acb0 <rtk_log_level_get>
2005af08:	02956563          	bltu	a0,s1,2005af32 <rtk_log_write+0x46>
2005af0c:	45a2                	c.lwsp	a1,8(sp)
2005af0e:	02300793          	addi	a5,zero,35
2005af12:	0005c703          	lbu	a4,0(a1)
2005af16:	00f70963          	beq	a4,a5,2005af28 <rtk_log_write+0x3c>
2005af1a:	4632                	c.lwsp	a2,12(sp)
2005af1c:	20063537          	lui	a0,0x20063
2005af20:	fcc50513          	addi	a0,a0,-52 # 20062fcc <__FUNCTION__.0+0x1a8>
2005af24:	3dc040ef          	jal	ra,2005f300 <DiagPrintf>
2005af28:	180c                	c.addi4spn	a1,sp,48
2005af2a:	8522                	c.mv	a0,s0
2005af2c:	ce2e                	c.swsp	a1,28(sp)
2005af2e:	0d4040ef          	jal	ra,2005f002 <DiagVprintf>
2005af32:	50b2                	c.lwsp	ra,44(sp)
2005af34:	5422                	c.lwsp	s0,40(sp)
2005af36:	5492                	c.lwsp	s1,36(sp)
2005af38:	6121                	c.addi16sp	sp,64
2005af3a:	8082                	c.jr	ra

2005af3c <hash_index>:
2005af3c:	4781                	c.li	a5,0
2005af3e:	08300693          	addi	a3,zero,131
2005af42:	00054703          	lbu	a4,0(a0)
2005af46:	e709                	c.bnez	a4,2005af50 <hash_index+0x14>
2005af48:	00179513          	slli	a0,a5,0x1
2005af4c:	8105                	c.srli	a0,0x1
2005af4e:	8082                	c.jr	ra
2005af50:	02d787b3          	mul	a5,a5,a3
2005af54:	0505                	c.addi	a0,1
2005af56:	97ba                	c.add	a5,a4
2005af58:	b7ed                	c.j	2005af42 <hash_index+0x6>

2005af5a <log_add_new_command>:
2005af5a:	1141                	c.addi	sp,-16
2005af5c:	c422                	c.swsp	s0,8(sp)
2005af5e:	c606                	c.swsp	ra,12(sp)
2005af60:	842a                	c.mv	s0,a0
2005af62:	4108                	c.lw	a0,0(a0)
2005af64:	3fe1                	c.jal	2005af3c <hash_index>
2005af66:	02000793          	addi	a5,zero,32
2005af6a:	02f56533          	rem	a0,a0,a5
2005af6e:	4b818793          	addi	a5,gp,1208 # 20066d5c <log_hash>
2005af72:	00840713          	addi	a4,s0,8
2005af76:	050e                	c.slli	a0,0x3
2005af78:	97aa                	c.add	a5,a0
2005af7a:	4394                	c.lw	a3,0(a5)
2005af7c:	c2d8                	c.sw	a4,4(a3)
2005af7e:	c414                	c.sw	a3,8(s0)
2005af80:	c45c                	c.sw	a5,12(s0)
2005af82:	c398                	c.sw	a4,0(a5)
2005af84:	40b2                	c.lwsp	ra,12(sp)
2005af86:	4422                	c.lwsp	s0,8(sp)
2005af88:	0141                	c.addi	sp,16
2005af8a:	8082                	c.jr	ra

2005af8c <log_service_init>:
2005af8c:	4b818793          	addi	a5,gp,1208 # 20066d5c <log_hash>
2005af90:	10078713          	addi	a4,a5,256
2005af94:	c39c                	c.sw	a5,0(a5)
2005af96:	c3dc                	c.sw	a5,4(a5)
2005af98:	07a1                	c.addi	a5,8
2005af9a:	fee79de3          	bne	a5,a4,2005af94 <log_service_init+0x8>
2005af9e:	e801a783          	lw	a5,-384(gp) # 20066724 <log_init_table>
2005afa2:	8782                	c.jr	a5

2005afa4 <log_service_add_table>:
2005afa4:	1141                	c.addi	sp,-16
2005afa6:	c422                	c.swsp	s0,8(sp)
2005afa8:	c226                	c.swsp	s1,4(sp)
2005afaa:	c04a                	c.swsp	s2,0(sp)
2005afac:	c606                	c.swsp	ra,12(sp)
2005afae:	892a                	c.mv	s2,a0
2005afb0:	84ae                	c.mv	s1,a1
2005afb2:	4401                	c.li	s0,0
2005afb4:	00944863          	blt	s0,s1,2005afc4 <log_service_add_table+0x20>
2005afb8:	40b2                	c.lwsp	ra,12(sp)
2005afba:	4422                	c.lwsp	s0,8(sp)
2005afbc:	4492                	c.lwsp	s1,4(sp)
2005afbe:	4902                	c.lwsp	s2,0(sp)
2005afc0:	0141                	c.addi	sp,16
2005afc2:	8082                	c.jr	ra
2005afc4:	00441513          	slli	a0,s0,0x4
2005afc8:	954a                	c.add	a0,s2
2005afca:	3f41                	c.jal	2005af5a <log_add_new_command>
2005afcc:	0405                	c.addi	s0,1
2005afce:	b7dd                	c.j	2005afb4 <log_service_add_table+0x10>

2005afd0 <log_action>:
2005afd0:	1141                	c.addi	sp,-16
2005afd2:	c422                	c.swsp	s0,8(sp)
2005afd4:	c226                	c.swsp	s1,4(sp)
2005afd6:	c04a                	c.swsp	s2,0(sp)
2005afd8:	c606                	c.swsp	ra,12(sp)
2005afda:	892a                	c.mv	s2,a0
2005afdc:	02000413          	addi	s0,zero,32
2005afe0:	3fb1                	c.jal	2005af3c <hash_index>
2005afe2:	02856433          	rem	s0,a0,s0
2005afe6:	00341793          	slli	a5,s0,0x3
2005afea:	4b818413          	addi	s0,gp,1208 # 20066d5c <log_hash>
2005afee:	943e                	c.add	s0,a5
2005aff0:	4004                	c.lw	s1,0(s0)
2005aff2:	00849463          	bne	s1,s0,2005affa <log_action+0x2a>
2005aff6:	4501                	c.li	a0,0
2005aff8:	a819                	c.j	2005b00e <log_action+0x3e>
2005affa:	ff84a503          	lw	a0,-8(s1)
2005affe:	85ca                	c.mv	a1,s2
2005b000:	dffaf097          	auipc	ra,0xdffaf
2005b004:	ada080e7          	jalr	ra,-1318(ra) # 9ada <__wrap_strcmp>
2005b008:	e909                	c.bnez	a0,2005b01a <log_action+0x4a>
2005b00a:	ffc4a503          	lw	a0,-4(s1)
2005b00e:	40b2                	c.lwsp	ra,12(sp)
2005b010:	4422                	c.lwsp	s0,8(sp)
2005b012:	4492                	c.lwsp	s1,4(sp)
2005b014:	4902                	c.lwsp	s2,0(sp)
2005b016:	0141                	c.addi	sp,16
2005b018:	8082                	c.jr	ra
2005b01a:	4084                	c.lw	s1,0(s1)
2005b01c:	bfd9                	c.j	2005aff2 <log_action+0x22>

2005b01e <log_handler>:
2005b01e:	7139                	c.addi16sp	sp,-64
2005b020:	dc22                	c.swsp	s0,56(sp)
2005b022:	c42a                	c.swsp	a0,8(sp)
2005b024:	842a                	c.mv	s0,a0
2005b026:	4675                	c.li	a2,29
2005b028:	4581                	c.li	a1,0
2005b02a:	0808                	c.addi4spn	a0,sp,16
2005b02c:	de06                	c.swsp	ra,60(sp)
2005b02e:	da26                	c.swsp	s1,52(sp)
2005b030:	c602                	c.swsp	zero,12(sp)
2005b032:	dffaf097          	auipc	ra,0xdffaf
2005b036:	a9c080e7          	jalr	ra,-1380(ra) # 9ace <__wrap_memset>
2005b03a:	200635b7          	lui	a1,0x20063
2005b03e:	461d                	c.li	a2,7
2005b040:	fd858593          	addi	a1,a1,-40 # 20062fd8 <__FUNCTION__.0+0x1b4>
2005b044:	8522                	c.mv	a0,s0
2005b046:	49d060ef          	jal	ra,20061ce2 <strncmp>
2005b04a:	e519                	c.bnez	a0,2005b058 <log_handler+0x3a>
2005b04c:	4501                	c.li	a0,0
2005b04e:	50f2                	c.lwsp	ra,60(sp)
2005b050:	5462                	c.lwsp	s0,56(sp)
2005b052:	54d2                	c.lwsp	s1,52(sp)
2005b054:	6121                	c.addi16sp	sp,64
2005b056:	8082                	c.jr	ra
2005b058:	200635b7          	lui	a1,0x20063
2005b05c:	fe058593          	addi	a1,a1,-32 # 20062fe0 <__FUNCTION__.0+0x1bc>
2005b060:	0028                	c.addi4spn	a0,sp,8
2005b062:	557060ef          	jal	ra,20061db8 <strsep>
2005b066:	200635b7          	lui	a1,0x20063
2005b06a:	842a                	c.mv	s0,a0
2005b06c:	03458593          	addi	a1,a1,52 # 20063034 <__FUNCTION__.0+0x210>
2005b070:	0028                	c.addi4spn	a0,sp,8
2005b072:	547060ef          	jal	ra,20061db8 <strsep>
2005b076:	84aa                	c.mv	s1,a0
2005b078:	d871                	c.beqz	s0,2005b04c <log_handler+0x2e>
2005b07a:	8522                	c.mv	a0,s0
2005b07c:	dffaf097          	auipc	ra,0xdffaf
2005b080:	a5c080e7          	jalr	ra,-1444(ra) # 9ad8 <__wrap_strlen>
2005b084:	478d                	c.li	a5,3
2005b086:	fca7f3e3          	bgeu	a5,a0,2005b04c <log_handler+0x2e>
2005b08a:	02000613          	addi	a2,zero,32
2005b08e:	85a2                	c.mv	a1,s0
2005b090:	0068                	c.addi4spn	a0,sp,12
2005b092:	4bd060ef          	jal	ra,20061d4e <strncpy>
2005b096:	00c14703          	lbu	a4,12(sp)
2005b09a:	04100793          	addi	a5,zero,65
2005b09e:	faf717e3          	bne	a4,a5,2005b04c <log_handler+0x2e>
2005b0a2:	00d14703          	lbu	a4,13(sp)
2005b0a6:	05400793          	addi	a5,zero,84
2005b0aa:	faf711e3          	bne	a4,a5,2005b04c <log_handler+0x2e>
2005b0ae:	00e10513          	addi	a0,sp,14
2005b0b2:	3f39                	c.jal	2005afd0 <log_action>
2005b0b4:	842a                	c.mv	s0,a0
2005b0b6:	4501                	c.li	a0,0
2005b0b8:	d859                	c.beqz	s0,2005b04e <log_handler+0x30>
2005b0ba:	8526                	c.mv	a0,s1
2005b0bc:	9402                	c.jalr	s0
2005b0be:	8522                	c.mv	a0,s0
2005b0c0:	b779                	c.j	2005b04e <log_handler+0x30>

2005b0c2 <parse_param>:
2005b0c2:	7179                	c.addi16sp	sp,-48
2005b0c4:	d422                	c.swsp	s0,40(sp)
2005b0c6:	d606                	c.swsp	ra,44(sp)
2005b0c8:	d226                	c.swsp	s1,36(sp)
2005b0ca:	d04a                	c.swsp	s2,32(sp)
2005b0cc:	ce4e                	c.swsp	s3,28(sp)
2005b0ce:	4405                	c.li	s0,1
2005b0d0:	c505                	c.beqz	a0,2005b0f8 <parse_param+0x36>
2005b0d2:	84ae                	c.mv	s1,a1
2005b0d4:	c62a                	c.swsp	a0,12(sp)
2005b0d6:	4405                	c.li	s0,1
2005b0d8:	200639b7          	lui	s3,0x20063
2005b0dc:	495d                	c.li	s2,23
2005b0de:	fe498593          	addi	a1,s3,-28 # 20062fe4 <__FUNCTION__.0+0x1c0>
2005b0e2:	0068                	c.addi4spn	a0,sp,12
2005b0e4:	4d5060ef          	jal	ra,20061db8 <strsep>
2005b0e8:	00241793          	slli	a5,s0,0x2
2005b0ec:	97a6                	c.add	a5,s1
2005b0ee:	c388                	c.sw	a0,0(a5)
2005b0f0:	c501                	c.beqz	a0,2005b0f8 <parse_param+0x36>
2005b0f2:	0405                	c.addi	s0,1
2005b0f4:	ff2415e3          	bne	s0,s2,2005b0de <parse_param+0x1c>
2005b0f8:	50b2                	c.lwsp	ra,44(sp)
2005b0fa:	8522                	c.mv	a0,s0
2005b0fc:	5422                	c.lwsp	s0,40(sp)
2005b0fe:	5492                	c.lwsp	s1,36(sp)
2005b100:	5902                	c.lwsp	s2,32(sp)
2005b102:	49f2                	c.lwsp	s3,28(sp)
2005b104:	6145                	c.addi16sp	sp,48
2005b106:	8082                	c.jr	ra

2005b108 <mp_command_handler>:
2005b108:	200635b7          	lui	a1,0x20063
2005b10c:	1141                	c.addi	sp,-16
2005b10e:	461d                	c.li	a2,7
2005b110:	fd858593          	addi	a1,a1,-40 # 20062fd8 <__FUNCTION__.0+0x1b4>
2005b114:	c422                	c.swsp	s0,8(sp)
2005b116:	c226                	c.swsp	s1,4(sp)
2005b118:	c606                	c.swsp	ra,12(sp)
2005b11a:	842a                	c.mv	s0,a0
2005b11c:	3c7060ef          	jal	ra,20061ce2 <strncmp>
2005b120:	54fd                	c.li	s1,-1
2005b122:	ed09                	c.bnez	a0,2005b13c <mp_command_handler+0x34>
2005b124:	041d                	c.addi	s0,7
2005b126:	84aa                	c.mv	s1,a0
2005b128:	8522                	c.mv	a0,s0
2005b12a:	dffaf097          	auipc	ra,0xdffaf
2005b12e:	9ae080e7          	jalr	ra,-1618(ra) # 9ad8 <__wrap_strlen>
2005b132:	85aa                	c.mv	a1,a0
2005b134:	4605                	c.li	a2,1
2005b136:	8522                	c.mv	a0,s0
2005b138:	0a1050ef          	jal	ra,200609d8 <inic_mp_command>
2005b13c:	40b2                	c.lwsp	ra,12(sp)
2005b13e:	4422                	c.lwsp	s0,8(sp)
2005b140:	8526                	c.mv	a0,s1
2005b142:	4492                	c.lwsp	s1,4(sp)
2005b144:	0141                	c.addi	sp,16
2005b146:	8082                	c.jr	ra

2005b148 <log_service>:
2005b148:	1141                	c.addi	sp,-16
2005b14a:	c422                	c.swsp	s0,8(sp)
2005b14c:	c606                	c.swsp	ra,12(sp)
2005b14e:	842a                	c.mv	s0,a0
2005b150:	35f9                	c.jal	2005b01e <log_handler>
2005b152:	ed01                	c.bnez	a0,2005b16a <log_service+0x22>
2005b154:	8522                	c.mv	a0,s0
2005b156:	3f4d                	c.jal	2005b108 <mp_command_handler>
2005b158:	00055963          	bge	a0,zero,2005b16a <log_service+0x22>
2005b15c:	20063537          	lui	a0,0x20063
2005b160:	85a2                	c.mv	a1,s0
2005b162:	fe850513          	addi	a0,a0,-24 # 20062fe8 <__FUNCTION__.0+0x1c4>
2005b166:	1ba040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b16a:	23b5                	c.jal	2005b6d6 <rtos_mem_get_free_heap_size>
2005b16c:	85aa                	c.mv	a1,a0
2005b16e:	20063537          	lui	a0,0x20063
2005b172:	00050513          	addi	a0,a0,0 # 20063000 <__FUNCTION__.0+0x1dc>
2005b176:	1aa040ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b17a:	4422                	c.lwsp	s0,8(sp)
2005b17c:	40b2                	c.lwsp	ra,12(sp)
2005b17e:	20063537          	lui	a0,0x20063
2005b182:	02c50513          	addi	a0,a0,44 # 2006302c <__FUNCTION__.0+0x208>
2005b186:	0141                	c.addi	sp,16
2005b188:	1980406f          	jal	zero,2005f320 <DiagPrintf_minimal>

2005b18c <app_mbedtls_free_func>:
2005b18c:	a389                	c.j	2005b6ce <rtos_mem_free>

2005b18e <app_mbedtls_calloc_func>:
2005b18e:	02b50633          	mul	a2,a0,a1
2005b192:	1101                	c.addi	sp,-32
2005b194:	cc22                	c.swsp	s0,24(sp)
2005b196:	ce06                	c.swsp	ra,28(sp)
2005b198:	8532                	c.mv	a0,a2
2005b19a:	c632                	c.swsp	a2,12(sp)
2005b19c:	2321                	c.jal	2005b6a4 <rtos_mem_malloc>
2005b19e:	842a                	c.mv	s0,a0
2005b1a0:	c519                	c.beqz	a0,2005b1ae <app_mbedtls_calloc_func+0x20>
2005b1a2:	4632                	c.lwsp	a2,12(sp)
2005b1a4:	4581                	c.li	a1,0
2005b1a6:	dffaf097          	auipc	ra,0xdffaf
2005b1aa:	928080e7          	jalr	ra,-1752(ra) # 9ace <__wrap_memset>
2005b1ae:	40f2                	c.lwsp	ra,28(sp)
2005b1b0:	8522                	c.mv	a0,s0
2005b1b2:	4462                	c.lwsp	s0,24(sp)
2005b1b4:	6105                	c.addi16sp	sp,32
2005b1b6:	8082                	c.jr	ra

2005b1b8 <app_IWDG_refresh>:
2005b1b8:	4100d537          	lui	a0,0x4100d
2005b1bc:	c0050513          	addi	a0,a0,-1024 # 4100cc00 <__km4_bd_boot_download_addr__+0x10ffac00>
2005b1c0:	dffad317          	auipc	t1,0xdffad
2005b1c4:	04230067          	jalr	zero,66(t1) # 8202 <WDG_Refresh>

2005b1c8 <app_init_debug>:
2005b1c8:	1141                	c.addi	sp,-16
2005b1ca:	c606                	c.swsp	ra,12(sp)
2005b1cc:	dffac097          	auipc	ra,0xdffac
2005b1d0:	9d4080e7          	jalr	ra,-1580(ra) # 6ba0 <SYSCFG_OTP_DisBootLog>
2005b1d4:	00153513          	sltiu	a0,a0,1
2005b1d8:	200017b7          	lui	a5,0x20001
2005b1dc:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b1e0:	0506                	c.slli	a0,0x1
2005b1e2:	577d                	c.li	a4,-1
2005b1e4:	c398                	c.sw	a4,0(a5)
2005b1e6:	0007a223          	sw	zero,4(a5)
2005b1ea:	c788                	c.sw	a0,8(a5)
2005b1ec:	0007a623          	sw	zero,12(a5)
2005b1f0:	40b2                	c.lwsp	ra,12(sp)
2005b1f2:	0141                	c.addi	sp,16
2005b1f4:	8082                	c.jr	ra

2005b1f6 <app_mbedtls_rom_init>:
2005b1f6:	2005b5b7          	lui	a1,0x2005b
2005b1fa:	2005b537          	lui	a0,0x2005b
2005b1fe:	1141                	c.addi	sp,-16
2005b200:	18c58593          	addi	a1,a1,396 # 2005b18c <app_mbedtls_free_func>
2005b204:	18e50513          	addi	a0,a0,398 # 2005b18e <app_mbedtls_calloc_func>
2005b208:	c606                	c.swsp	ra,12(sp)
2005b20a:	407000ef          	jal	ra,2005be10 <mbedtls_platform_set_calloc_free>
2005b20e:	40b2                	c.lwsp	ra,12(sp)
2005b210:	ee01aa23          	sw	zero,-268(gp) # 20066798 <use_hw_crypto_func>
2005b214:	0141                	c.addi	sp,16
2005b216:	8082                	c.jr	ra

2005b218 <app_pmu_init>:
2005b218:	1141                	c.addi	sp,-16
2005b21a:	200017b7          	lui	a5,0x20001
2005b21e:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b222:	c606                	c.swsp	ra,12(sp)
2005b224:	4798                	c.lw	a4,8(a5)
2005b226:	100006b7          	lui	a3,0x10000
2005b22a:	4501                	c.li	a0,0
2005b22c:	8f55                	c.or	a4,a3
2005b22e:	c798                	c.sw	a4,8(a5)
2005b230:	e12fe0ef          	jal	ra,20059842 <pmu_acquire_wakelock>
2005b234:	4100c7b7          	lui	a5,0x4100c
2005b238:	22c7a783          	lw	a5,556(a5) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
2005b23c:	8b89                	c.andi	a5,2
2005b23e:	c38d                	c.beqz	a5,2005b260 <app_pmu_init+0x48>
2005b240:	4509                	c.li	a0,2
2005b242:	e00fe0ef          	jal	ra,20059842 <pmu_acquire_wakelock>
2005b246:	a0ffe0ef          	jal	ra,20059c54 <SWR_CORE_Vol_Get>
2005b24a:	4785                	c.li	a5,1
2005b24c:	00f51a63          	bne	a0,a5,2005b260 <app_pmu_init+0x48>
2005b250:	95efe0ef          	jal	ra,200593ae <dsp_status_on>
2005b254:	c511                	c.beqz	a0,2005b260 <app_pmu_init+0x48>
2005b256:	40b2                	c.lwsp	ra,12(sp)
2005b258:	4511                	c.li	a0,4
2005b25a:	0141                	c.addi	sp,16
2005b25c:	de6fe06f          	jal	zero,20059842 <pmu_acquire_wakelock>
2005b260:	40b2                	c.lwsp	ra,12(sp)
2005b262:	0141                	c.addi	sp,16
2005b264:	8082                	c.jr	ra

2005b266 <app_IWDG_int>:
2005b266:	4100c7b7          	lui	a5,0x4100c
2005b26a:	479c                	c.lw	a5,8(a5)
2005b26c:	c0000737          	lui	a4,0xc0000
2005b270:	0709                	c.addi	a4,2
2005b272:	8ff9                	c.and	a5,a4
2005b274:	c7b5                	c.beqz	a5,2005b2e0 <app_IWDG_int+0x7a>
2005b276:	1101                	c.addi	sp,-32
2005b278:	cc22                	c.swsp	s0,24(sp)
2005b27a:	200636b7          	lui	a3,0x20063
2005b27e:	20063437          	lui	s0,0x20063
2005b282:	03868693          	addi	a3,a3,56 # 20063038 <__FUNCTION__.0+0x214>
2005b286:	04900613          	addi	a2,zero,73
2005b28a:	04c40593          	addi	a1,s0,76 # 2006304c <__FUNCTION__.0+0x228>
2005b28e:	4511                	c.li	a0,4
2005b290:	ce06                	c.swsp	ra,28(sp)
2005b292:	c5bff0ef          	jal	ra,2005aeec <rtk_log_write>
2005b296:	2005b7b7          	lui	a5,0x2005b
2005b29a:	200635b7          	lui	a1,0x20063
2005b29e:	1b878793          	addi	a5,a5,440 # 2005b1b8 <app_IWDG_refresh>
2005b2a2:	4705                	c.li	a4,1
2005b2a4:	1f400693          	addi	a3,zero,500
2005b2a8:	4601                	c.li	a2,0
2005b2aa:	05458593          	addi	a1,a1,84 # 20063054 <__FUNCTION__.0+0x230>
2005b2ae:	0068                	c.addi4spn	a0,sp,12
2005b2b0:	c602                	c.swsp	zero,12(sp)
2005b2b2:	151000ef          	jal	ra,2005bc02 <rtos_timer_create>
2005b2b6:	4532                	c.lwsp	a0,12(sp)
2005b2b8:	e105                	c.bnez	a0,2005b2d8 <app_IWDG_int+0x72>
2005b2ba:	200636b7          	lui	a3,0x20063
2005b2be:	06068693          	addi	a3,a3,96 # 20063060 <__FUNCTION__.0+0x23c>
2005b2c2:	04500613          	addi	a2,zero,69
2005b2c6:	04c40593          	addi	a1,s0,76
2005b2ca:	4509                	c.li	a0,2
2005b2cc:	c21ff0ef          	jal	ra,2005aeec <rtk_log_write>
2005b2d0:	40f2                	c.lwsp	ra,28(sp)
2005b2d2:	4462                	c.lwsp	s0,24(sp)
2005b2d4:	6105                	c.addi16sp	sp,32
2005b2d6:	8082                	c.jr	ra
2005b2d8:	4581                	c.li	a1,0
2005b2da:	207000ef          	jal	ra,2005bce0 <rtos_timer_start>
2005b2de:	bfcd                	c.j	2005b2d0 <app_IWDG_int+0x6a>
2005b2e0:	8082                	c.jr	ra

2005b2e2 <app_pre_example>:
2005b2e2:	8082                	c.jr	ra

2005b2e4 <app_example>:
2005b2e4:	8082                	c.jr	ra

2005b2e6 <main>:
2005b2e6:	20059537          	lui	a0,0x20059
2005b2ea:	1141                	c.addi	sp,-16
2005b2ec:	4691                	c.li	a3,4
2005b2ee:	41020637          	lui	a2,0x41020
2005b2f2:	02b00593          	addi	a1,zero,43
2005b2f6:	17e50513          	addi	a0,a0,382 # 2005917e <IPC_INTHandler>
2005b2fa:	c606                	c.swsp	ra,12(sp)
2005b2fc:	570040ef          	jal	ra,2005f86c <irq_register>
2005b300:	02b00513          	addi	a0,zero,43
2005b304:	dffad097          	auipc	ra,0xdffad
2005b308:	cd4080e7          	jalr	ra,-812(ra) # 7fd8 <irq_enable>
2005b30c:	35ed                	c.jal	2005b1f6 <app_mbedtls_rom_init>
2005b30e:	4581                	c.li	a1,0
2005b310:	4501                	c.li	a0,0
2005b312:	dffae097          	auipc	ra,0xdffae
2005b316:	f42080e7          	jalr	ra,-190(ra) # 9254 <shell_init_rom>
2005b31a:	5d5010ef          	jal	ra,2005d0ee <shell_init_ram>
2005b31e:	41020537          	lui	a0,0x41020
2005b322:	a55fd0ef          	jal	ra,20058d76 <ipc_table_init>
2005b326:	2005c537          	lui	a0,0x2005c
2005b32a:	ba850513          	addi	a0,a0,-1112 # 2005bba8 <rtos_time_delay_ms>
2005b32e:	832fe0ef          	jal	ra,20059360 <IPC_SEMDelayStub>
2005b332:	3f45                	c.jal	2005b2e2 <app_pre_example>
2005b334:	678030ef          	jal	ra,2005e9ac <wlan_initialize>
2005b338:	3d41                	c.jal	2005b1c8 <app_init_debug>
2005b33a:	3df9                	c.jal	2005b218 <app_pmu_init>
2005b33c:	372d                	c.jal	2005b266 <app_IWDG_int>
2005b33e:	375d                	c.jal	2005b2e4 <app_example>
2005b340:	200636b7          	lui	a3,0x20063
2005b344:	200635b7          	lui	a1,0x20063
2005b348:	07468693          	addi	a3,a3,116 # 20063074 <__FUNCTION__.0+0x250>
2005b34c:	04900613          	addi	a2,zero,73
2005b350:	04c58593          	addi	a1,a1,76 # 2006304c <__FUNCTION__.0+0x228>
2005b354:	4511                	c.li	a0,4
2005b356:	b97ff0ef          	jal	ra,2005aeec <rtk_log_write>
2005b35a:	7e2000ef          	jal	ra,2005bb3c <rtos_sched_start>
2005b35e:	40b2                	c.lwsp	ra,12(sp)
2005b360:	4501                	c.li	a0,0
2005b362:	0141                	c.addi	sp,16
2005b364:	8082                	c.jr	ra

2005b366 <CmdRamHelp>:
2005b366:	1101                	c.addi	sp,-32
2005b368:	cc22                	c.swsp	s0,24(sp)
2005b36a:	ca26                	c.swsp	s1,20(sp)
2005b36c:	20065437          	lui	s0,0x20065
2005b370:	200654b7          	lui	s1,0x20065
2005b374:	b7c48793          	addi	a5,s1,-1156 # 20064b7c <shell_cmd_table>
2005b378:	bbc40413          	addi	s0,s0,-1092 # 20064bbc <ipc_tickless_table>
2005b37c:	8c1d                	c.sub	s0,a5
2005b37e:	8011                	c.srli	s0,0x4
2005b380:	20063537          	lui	a0,0x20063
2005b384:	85a2                	c.mv	a1,s0
2005b386:	08450513          	addi	a0,a0,132 # 20063084 <__FUNCTION__.0+0x260>
2005b38a:	c84a                	c.swsp	s2,16(sp)
2005b38c:	c64e                	c.swsp	s3,12(sp)
2005b38e:	ce06                	c.swsp	ra,28(sp)
2005b390:	b7c48493          	addi	s1,s1,-1156
2005b394:	78d030ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b398:	4901                	c.li	s2,0
2005b39a:	200639b7          	lui	s3,0x20063
2005b39e:	02891163          	bne	s2,s0,2005b3c0 <CmdRamHelp+0x5a>
2005b3a2:	20063537          	lui	a0,0x20063
2005b3a6:	85ca                	c.mv	a1,s2
2005b3a8:	0c850513          	addi	a0,a0,200 # 200630c8 <__FUNCTION__.0+0x2a4>
2005b3ac:	775030ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b3b0:	40f2                	c.lwsp	ra,28(sp)
2005b3b2:	4462                	c.lwsp	s0,24(sp)
2005b3b4:	44d2                	c.lwsp	s1,20(sp)
2005b3b6:	4942                	c.lwsp	s2,16(sp)
2005b3b8:	49b2                	c.lwsp	s3,12(sp)
2005b3ba:	4505                	c.li	a0,1
2005b3bc:	6105                	c.addi16sp	sp,32
2005b3be:	8082                	c.jr	ra
2005b3c0:	44cc                	c.lw	a1,12(s1)
2005b3c2:	c589                	c.beqz	a1,2005b3cc <CmdRamHelp+0x66>
2005b3c4:	0c498513          	addi	a0,s3,196 # 200630c4 <__FUNCTION__.0+0x2a0>
2005b3c8:	759030ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b3cc:	0905                	c.addi	s2,1
2005b3ce:	04c1                	c.addi	s1,16
2005b3d0:	b7f9                	c.j	2005b39e <CmdRamHelp+0x38>

2005b3d2 <cmd_reboot>:
2005b3d2:	1141                	c.addi	sp,-16
2005b3d4:	c226                	c.swsp	s1,4(sp)
2005b3d6:	84aa                	c.mv	s1,a0
2005b3d8:	20063537          	lui	a0,0x20063
2005b3dc:	10850513          	addi	a0,a0,264 # 20063108 <__FUNCTION__.0+0x2e4>
2005b3e0:	c422                	c.swsp	s0,8(sp)
2005b3e2:	c606                	c.swsp	ra,12(sp)
2005b3e4:	842e                	c.mv	s0,a1
2005b3e6:	73b030ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b3ea:	c08d                	c.beqz	s1,2005b40c <cmd_reboot+0x3a>
2005b3ec:	4008                	c.lw	a0,0(s0)
2005b3ee:	200635b7          	lui	a1,0x20063
2005b3f2:	11858593          	addi	a1,a1,280 # 20063118 <__FUNCTION__.0+0x2f4>
2005b3f6:	dffae097          	auipc	ra,0xdffae
2005b3fa:	436080e7          	jalr	ra,1078(ra) # 982c <_strcmp>
2005b3fe:	e519                	c.bnez	a0,2005b40c <cmd_reboot+0x3a>
2005b400:	20000593          	addi	a1,zero,512
2005b404:	dffa5097          	auipc	ra,0xdffa5
2005b408:	4a2080e7          	jalr	ra,1186(ra) # 8a6 <BKUP_Set>
2005b40c:	ffcfe0ef          	jal	ra,20059c08 <System_Reset>
2005b410:	40b2                	c.lwsp	ra,12(sp)
2005b412:	4422                	c.lwsp	s0,8(sp)
2005b414:	4492                	c.lwsp	s1,4(sp)
2005b416:	4505                	c.li	a0,1
2005b418:	0141                	c.addi	sp,16
2005b41a:	8082                	c.jr	ra

2005b41c <cmd_write_word>:
2005b41c:	1141                	c.addi	sp,-16
2005b41e:	c606                	c.swsp	ra,12(sp)
2005b420:	c422                	c.swsp	s0,8(sp)
2005b422:	c226                	c.swsp	s1,4(sp)
2005b424:	c04a                	c.swsp	s2,0(sp)
2005b426:	892e                	c.mv	s2,a1
2005b428:	00092503          	lw	a0,0(s2)
2005b42c:	4641                	c.li	a2,16
2005b42e:	4581                	c.li	a1,0
2005b430:	dffae097          	auipc	ra,0xdffae
2005b434:	506080e7          	jalr	ra,1286(ra) # 9936 <_strtoul>
2005b438:	ffc57413          	andi	s0,a0,-4
2005b43c:	84aa                	c.mv	s1,a0
2005b43e:	00492503          	lw	a0,4(s2)
2005b442:	4641                	c.li	a2,16
2005b444:	4581                	c.li	a1,0
2005b446:	dffae097          	auipc	ra,0xdffae
2005b44a:	4f0080e7          	jalr	ra,1264(ra) # 9936 <_strtoul>
2005b44e:	892a                	c.mv	s2,a0
2005b450:	862a                	c.mv	a2,a0
2005b452:	20063537          	lui	a0,0x20063
2005b456:	85a2                	c.mv	a1,s0
2005b458:	12450513          	addi	a0,a0,292 # 20063124 <__FUNCTION__.0+0x300>
2005b45c:	6c5030ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b460:	01242023          	sw	s2,0(s0)
2005b464:	7c1027f3          	csrrs	a5,0x7c1,zero
2005b468:	1007f793          	andi	a5,a5,256
2005b46c:	c7a1                	c.beqz	a5,2005b4b4 <cmd_write_word+0x98>
2005b46e:	01c4f793          	andi	a5,s1,28
2005b472:	4711                	c.li	a4,4
2005b474:	c789                	c.beqz	a5,2005b47e <cmd_write_word+0x62>
2005b476:	00478713          	addi	a4,a5,4
2005b47a:	fe04f413          	andi	s0,s1,-32
2005b47e:	01f47793          	andi	a5,s0,31
2005b482:	97ba                	c.add	a5,a4
2005b484:	0ff0000f          	fence	iorw,iorw
2005b488:	97a2                	c.add	a5,s0
2005b48a:	0484700b          	cache	dwb,(s0)
2005b48e:	02040413          	addi	s0,s0,32
2005b492:	40878733          	sub	a4,a5,s0
2005b496:	fee04ae3          	blt	zero,a4,2005b48a <cmd_write_word+0x6e>
2005b49a:	0ff0000f          	fence	iorw,iorw
2005b49e:	0001                	c.addi	zero,0
2005b4a0:	0001                	c.addi	zero,0
2005b4a2:	0001                	c.addi	zero,0
2005b4a4:	0001                	c.addi	zero,0
2005b4a6:	0001                	c.addi	zero,0
2005b4a8:	0001                	c.addi	zero,0
2005b4aa:	0001                	c.addi	zero,0
2005b4ac:	0001                	c.addi	zero,0
2005b4ae:	0001                	c.addi	zero,0
2005b4b0:	0001                	c.addi	zero,0
2005b4b2:	0001                	c.addi	zero,0
2005b4b4:	40b2                	c.lwsp	ra,12(sp)
2005b4b6:	4422                	c.lwsp	s0,8(sp)
2005b4b8:	4492                	c.lwsp	s1,4(sp)
2005b4ba:	4902                	c.lwsp	s2,0(sp)
2005b4bc:	4501                	c.li	a0,0
2005b4be:	0141                	c.addi	sp,16
2005b4c0:	8082                	c.jr	ra

2005b4c2 <cmd_dump_word>:
2005b4c2:	1101                	c.addi	sp,-32
2005b4c4:	cc22                	c.swsp	s0,24(sp)
2005b4c6:	c84a                	c.swsp	s2,16(sp)
2005b4c8:	842a                	c.mv	s0,a0
2005b4ca:	ce06                	c.swsp	ra,28(sp)
2005b4cc:	ca26                	c.swsp	s1,20(sp)
2005b4ce:	c64e                	c.swsp	s3,12(sp)
2005b4d0:	892e                	c.mv	s2,a1
2005b4d2:	dffab097          	auipc	ra,0xdffab
2005b4d6:	67c080e7          	jalr	ra,1660(ra) # 6b4e <SYSCFG_OTP_RSIPEn>
2005b4da:	fff40793          	addi	a5,s0,-1
2005b4de:	07c2                	c.slli	a5,0x10
2005b4e0:	83c1                	c.srli	a5,0x10
2005b4e2:	4709                	c.li	a4,2
2005b4e4:	00f77a63          	bgeu	a4,a5,2005b4f8 <cmd_dump_word+0x36>
2005b4e8:	20063537          	lui	a0,0x20063
2005b4ec:	13450513          	addi	a0,a0,308 # 20063134 <__FUNCTION__.0+0x310>
2005b4f0:	631030ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b4f4:	4401                	c.li	s0,0
2005b4f6:	a0f9                	c.j	2005b5c4 <cmd_dump_word+0x102>
2005b4f8:	478d                	c.li	a5,3
2005b4fa:	89aa                	c.mv	s3,a0
2005b4fc:	06f41063          	bne	s0,a5,2005b55c <cmd_dump_word+0x9a>
2005b500:	00892783          	lw	a5,8(s2)
2005b504:	0007c403          	lbu	s0,0(a5)
2005b508:	0df47413          	andi	s0,s0,223
2005b50c:	fbe40413          	addi	s0,s0,-66
2005b510:	00143413          	sltiu	s0,s0,1
2005b514:	00492503          	lw	a0,4(s2)
2005b518:	4629                	c.li	a2,10
2005b51a:	4581                	c.li	a1,0
2005b51c:	dffae097          	auipc	ra,0xdffae
2005b520:	41a080e7          	jalr	ra,1050(ra) # 9936 <_strtoul>
2005b524:	84aa                	c.mv	s1,a0
2005b526:	00092503          	lw	a0,0(s2)
2005b52a:	4641                	c.li	a2,16
2005b52c:	4581                	c.li	a1,0
2005b52e:	dffae097          	auipc	ra,0xdffae
2005b532:	408080e7          	jalr	ra,1032(ra) # 9936 <_strtoul>
2005b536:	87aa                	c.mv	a5,a0
2005b538:	f8000737          	lui	a4,0xf8000
2005b53c:	9971                	c.andi	a0,-4
2005b53e:	972a                	c.add	a4,a0
2005b540:	080006b7          	lui	a3,0x8000
2005b544:	02d77463          	bgeu	a4,a3,2005b56c <cmd_dump_word+0xaa>
2005b548:	02098263          	beq	s3,zero,2005b56c <cmd_dump_word+0xaa>
2005b54c:	20063537          	lui	a0,0x20063
2005b550:	15050513          	addi	a0,a0,336 # 20063150 <__FUNCTION__.0+0x32c>
2005b554:	5cd030ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b558:	4405                	c.li	s0,1
2005b55a:	a0ad                	c.j	2005b5c4 <cmd_dump_word+0x102>
2005b55c:	4785                	c.li	a5,1
2005b55e:	00f40463          	beq	s0,a5,2005b566 <cmd_dump_word+0xa4>
2005b562:	4401                	c.li	s0,0
2005b564:	bf45                	c.j	2005b514 <cmd_dump_word+0x52>
2005b566:	4401                	c.li	s0,0
2005b568:	4485                	c.li	s1,1
2005b56a:	bf75                	c.j	2005b526 <cmd_dump_word+0x64>
2005b56c:	c43d                	c.beqz	s0,2005b5da <cmd_dump_word+0x118>
2005b56e:	c4f9                	c.beqz	s1,2005b63c <cmd_dump_word+0x17a>
2005b570:	7c102773          	csrrs	a4,0x7c1,zero
2005b574:	10077713          	andi	a4,a4,256
2005b578:	c339                	c.beqz	a4,2005b5be <cmd_dump_word+0xfc>
2005b57a:	01c7f713          	andi	a4,a5,28
2005b57e:	cb39                	c.beqz	a4,2005b5d4 <cmd_dump_word+0x112>
2005b580:	9b81                	c.andi	a5,-32
2005b582:	9726                	c.add	a4,s1
2005b584:	02e05d63          	bge	zero,a4,2005b5be <cmd_dump_word+0xfc>
2005b588:	01f7f693          	andi	a3,a5,31
2005b58c:	9736                	c.add	a4,a3
2005b58e:	0ff0000f          	fence	iorw,iorw
2005b592:	973e                	c.add	a4,a5
2005b594:	02f7f00b          	cache	dwbinv,(a5)
2005b598:	02078793          	addi	a5,a5,32
2005b59c:	40f706b3          	sub	a3,a4,a5
2005b5a0:	fed04ae3          	blt	zero,a3,2005b594 <cmd_dump_word+0xd2>
2005b5a4:	0ff0000f          	fence	iorw,iorw
2005b5a8:	0001                	c.addi	zero,0
2005b5aa:	0001                	c.addi	zero,0
2005b5ac:	0001                	c.addi	zero,0
2005b5ae:	0001                	c.addi	zero,0
2005b5b0:	0001                	c.addi	zero,0
2005b5b2:	0001                	c.addi	zero,0
2005b5b4:	0001                	c.addi	zero,0
2005b5b6:	0001                	c.addi	zero,0
2005b5b8:	0001                	c.addi	zero,0
2005b5ba:	0001                	c.addi	zero,0
2005b5bc:	0001                	c.addi	zero,0
2005b5be:	85a6                	c.mv	a1,s1
2005b5c0:	8b9ff0ef          	jal	ra,2005ae78 <rtk_log_memory_dump_byte>
2005b5c4:	40f2                	c.lwsp	ra,28(sp)
2005b5c6:	8522                	c.mv	a0,s0
2005b5c8:	4462                	c.lwsp	s0,24(sp)
2005b5ca:	44d2                	c.lwsp	s1,20(sp)
2005b5cc:	4942                	c.lwsp	s2,16(sp)
2005b5ce:	49b2                	c.lwsp	s3,12(sp)
2005b5d0:	6105                	c.addi16sp	sp,32
2005b5d2:	8082                	c.jr	ra
2005b5d4:	8726                	c.mv	a4,s1
2005b5d6:	87aa                	c.mv	a5,a0
2005b5d8:	b775                	c.j	2005b584 <cmd_dump_word+0xc2>
2005b5da:	c0ad                	c.beqz	s1,2005b63c <cmd_dump_word+0x17a>
2005b5dc:	7c102773          	csrrs	a4,0x7c1,zero
2005b5e0:	10077713          	andi	a4,a4,256
2005b5e4:	cb21                	c.beqz	a4,2005b634 <cmd_dump_word+0x172>
2005b5e6:	01c7f693          	andi	a3,a5,28
2005b5ea:	00249613          	slli	a2,s1,0x2
2005b5ee:	872a                	c.mv	a4,a0
2005b5f0:	c681                	c.beqz	a3,2005b5f8 <cmd_dump_word+0x136>
2005b5f2:	fe07f713          	andi	a4,a5,-32
2005b5f6:	9636                	c.add	a2,a3
2005b5f8:	02c05e63          	bge	zero,a2,2005b634 <cmd_dump_word+0x172>
2005b5fc:	01f77693          	andi	a3,a4,31
2005b600:	96b2                	c.add	a3,a2
2005b602:	0ff0000f          	fence	iorw,iorw
2005b606:	87ba                	c.mv	a5,a4
2005b608:	9736                	c.add	a4,a3
2005b60a:	02f7f00b          	cache	dwbinv,(a5)
2005b60e:	02078793          	addi	a5,a5,32
2005b612:	40f706b3          	sub	a3,a4,a5
2005b616:	fed04ae3          	blt	zero,a3,2005b60a <cmd_dump_word+0x148>
2005b61a:	0ff0000f          	fence	iorw,iorw
2005b61e:	0001                	c.addi	zero,0
2005b620:	0001                	c.addi	zero,0
2005b622:	0001                	c.addi	zero,0
2005b624:	0001                	c.addi	zero,0
2005b626:	0001                	c.addi	zero,0
2005b628:	0001                	c.addi	zero,0
2005b62a:	0001                	c.addi	zero,0
2005b62c:	0001                	c.addi	zero,0
2005b62e:	0001                	c.addi	zero,0
2005b630:	0001                	c.addi	zero,0
2005b632:	0001                	c.addi	zero,0
2005b634:	85a6                	c.mv	a1,s1
2005b636:	fd0ff0ef          	jal	ra,2005ae06 <rtk_log_memory_dump_word>
2005b63a:	bf39                	c.j	2005b558 <cmd_dump_word+0x96>
2005b63c:	20063537          	lui	a0,0x20063
2005b640:	17450513          	addi	a0,a0,372 # 20063174 <__FUNCTION__.0+0x350>
2005b644:	b575                	c.j	2005b4f0 <cmd_dump_word+0x2e>

2005b646 <rtos_critical_is_in_interrupt>:
2005b646:	1141                	c.addi	sp,-16
2005b648:	c606                	c.swsp	ra,12(sp)
2005b64a:	6290a0ef          	jal	ra,20066472 <plic_get_active_irq_id>
2005b64e:	40b2                	c.lwsp	ra,12(sp)
2005b650:	00a03533          	sltu	a0,zero,a0
2005b654:	0141                	c.addi	sp,16
2005b656:	8082                	c.jr	ra

2005b658 <rtos_critical_enter>:
2005b658:	1141                	c.addi	sp,-16
2005b65a:	c606                	c.swsp	ra,12(sp)
2005b65c:	37ed                	c.jal	2005b646 <rtos_critical_is_in_interrupt>
2005b65e:	c919                	c.beqz	a0,2005b674 <rtos_critical_enter+0x1c>
2005b660:	ef818793          	addi	a5,gp,-264 # 2006679c <uxCriticalNestingCnt>
2005b664:	0007d703          	lhu	a4,0(a5)
2005b668:	0705                	c.addi	a4,1
2005b66a:	00e79023          	sh	a4,0(a5)
2005b66e:	40b2                	c.lwsp	ra,12(sp)
2005b670:	0141                	c.addi	sp,16
2005b672:	8082                	c.jr	ra
2005b674:	40b2                	c.lwsp	ra,12(sp)
2005b676:	0141                	c.addi	sp,16
2005b678:	51c0206f          	jal	zero,2005db94 <vTaskEnterCritical>

2005b67c <rtos_critical_exit>:
2005b67c:	1141                	c.addi	sp,-16
2005b67e:	c606                	c.swsp	ra,12(sp)
2005b680:	37d9                	c.jal	2005b646 <rtos_critical_is_in_interrupt>
2005b682:	c919                	c.beqz	a0,2005b698 <rtos_critical_exit+0x1c>
2005b684:	ef818793          	addi	a5,gp,-264 # 2006679c <uxCriticalNestingCnt>
2005b688:	0007d703          	lhu	a4,0(a5)
2005b68c:	177d                	c.addi	a4,-1
2005b68e:	00e79023          	sh	a4,0(a5)
2005b692:	40b2                	c.lwsp	ra,12(sp)
2005b694:	0141                	c.addi	sp,16
2005b696:	8082                	c.jr	ra
2005b698:	40b2                	c.lwsp	ra,12(sp)
2005b69a:	0141                	c.addi	sp,16
2005b69c:	5120206f          	jal	zero,2005dbae <vTaskExitCritical>

2005b6a0 <rtos_mem_init>:
2005b6a0:	97aff06f          	jal	zero,2005a81a <os_heap_init>

2005b6a4 <rtos_mem_malloc>:
2005b6a4:	adcff06f          	jal	zero,2005a980 <pvPortMalloc>

2005b6a8 <rtos_mem_zmalloc>:
2005b6a8:	1101                	c.addi	sp,-32
2005b6aa:	cc22                	c.swsp	s0,24(sp)
2005b6ac:	ce06                	c.swsp	ra,28(sp)
2005b6ae:	c62a                	c.swsp	a0,12(sp)
2005b6b0:	ad0ff0ef          	jal	ra,2005a980 <pvPortMalloc>
2005b6b4:	842a                	c.mv	s0,a0
2005b6b6:	c519                	c.beqz	a0,2005b6c4 <rtos_mem_zmalloc+0x1c>
2005b6b8:	4632                	c.lwsp	a2,12(sp)
2005b6ba:	4581                	c.li	a1,0
2005b6bc:	dffae097          	auipc	ra,0xdffae
2005b6c0:	412080e7          	jalr	ra,1042(ra) # 9ace <__wrap_memset>
2005b6c4:	40f2                	c.lwsp	ra,28(sp)
2005b6c6:	8522                	c.mv	a0,s0
2005b6c8:	4462                	c.lwsp	s0,24(sp)
2005b6ca:	6105                	c.addi16sp	sp,32
2005b6cc:	8082                	c.jr	ra

2005b6ce <rtos_mem_free>:
2005b6ce:	c119                	c.beqz	a0,2005b6d4 <rtos_mem_free+0x6>
2005b6d0:	b8eff06f          	jal	zero,2005aa5e <vPortFree>
2005b6d4:	8082                	c.jr	ra

2005b6d6 <rtos_mem_get_free_heap_size>:
2005b6d6:	be6ff06f          	jal	zero,2005aabc <xPortGetFreeHeapSize>

2005b6da <rtos_mutex_create>:
2005b6da:	c10d                	c.beqz	a0,2005b6fc <rtos_mutex_create+0x22>
2005b6dc:	1141                	c.addi	sp,-16
2005b6de:	c422                	c.swsp	s0,8(sp)
2005b6e0:	842a                	c.mv	s0,a0
2005b6e2:	4505                	c.li	a0,1
2005b6e4:	c606                	c.swsp	ra,12(sp)
2005b6e6:	6d4010ef          	jal	ra,2005cdba <xQueueCreateMutex>
2005b6ea:	c008                	c.sw	a0,0(s0)
2005b6ec:	40b2                	c.lwsp	ra,12(sp)
2005b6ee:	4422                	c.lwsp	s0,8(sp)
2005b6f0:	00153513          	sltiu	a0,a0,1
2005b6f4:	40a00533          	sub	a0,zero,a0
2005b6f8:	0141                	c.addi	sp,16
2005b6fa:	8082                	c.jr	ra
2005b6fc:	557d                	c.li	a0,-1
2005b6fe:	8082                	c.jr	ra

2005b700 <rtos_mutex_create_static>:
2005b700:	1141                	c.addi	sp,-16
2005b702:	c422                	c.swsp	s0,8(sp)
2005b704:	c606                	c.swsp	ra,12(sp)
2005b706:	842a                	c.mv	s0,a0
2005b708:	2e4d                	c.jal	2005baba <__reserved_get_mutex_from_poll>
2005b70a:	e511                	c.bnez	a0,2005b716 <rtos_mutex_create_static+0x16>
2005b70c:	8522                	c.mv	a0,s0
2005b70e:	4422                	c.lwsp	s0,8(sp)
2005b710:	40b2                	c.lwsp	ra,12(sp)
2005b712:	0141                	c.addi	sp,16
2005b714:	b7d9                	c.j	2005b6da <rtos_mutex_create>
2005b716:	85aa                	c.mv	a1,a0
2005b718:	4505                	c.li	a0,1
2005b71a:	72c010ef          	jal	ra,2005ce46 <xQueueCreateMutexStatic>
2005b71e:	c008                	c.sw	a0,0(s0)
2005b720:	40b2                	c.lwsp	ra,12(sp)
2005b722:	4422                	c.lwsp	s0,8(sp)
2005b724:	00153513          	sltiu	a0,a0,1
2005b728:	40a00533          	sub	a0,zero,a0
2005b72c:	0141                	c.addi	sp,16
2005b72e:	8082                	c.jr	ra

2005b730 <rtos_mutex_delete>:
2005b730:	c139                	c.beqz	a0,2005b776 <rtos_mutex_delete+0x46>
2005b732:	1141                	c.addi	sp,-16
2005b734:	c422                	c.swsp	s0,8(sp)
2005b736:	c606                	c.swsp	ra,12(sp)
2005b738:	842a                	c.mv	s0,a0
2005b73a:	12a010ef          	jal	ra,2005c864 <xQueueGetMutexHolder>
2005b73e:	e909                	c.bnez	a0,2005b750 <rtos_mutex_delete+0x20>
2005b740:	8522                	c.mv	a0,s0
2005b742:	786010ef          	jal	ra,2005cec8 <vQueueDelete>
2005b746:	4501                	c.li	a0,0
2005b748:	40b2                	c.lwsp	ra,12(sp)
2005b74a:	4422                	c.lwsp	s0,8(sp)
2005b74c:	0141                	c.addi	sp,16
2005b74e:	8082                	c.jr	ra
2005b750:	8522                	c.mv	a0,s0
2005b752:	776010ef          	jal	ra,2005cec8 <vQueueDelete>
2005b756:	4501                	c.li	a0,0
2005b758:	72f010ef          	jal	ra,2005d686 <pcTaskGetName>
2005b75c:	85aa                	c.mv	a1,a0
2005b75e:	20063637          	lui	a2,0x20063
2005b762:	20063537          	lui	a0,0x20063
2005b766:	3e860613          	addi	a2,a2,1000 # 200633e8 <__FUNCTION__.0>
2005b76a:	39450513          	addi	a0,a0,916 # 20063394 <__FUNCTION__.0+0x570>
2005b76e:	3b3030ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005b772:	557d                	c.li	a0,-1
2005b774:	bfd1                	c.j	2005b748 <rtos_mutex_delete+0x18>
2005b776:	557d                	c.li	a0,-1
2005b778:	8082                	c.jr	ra

2005b77a <rtos_mutex_delete_static>:
2005b77a:	1141                	c.addi	sp,-16
2005b77c:	c606                	c.swsp	ra,12(sp)
2005b77e:	c422                	c.swsp	s0,8(sp)
2005b780:	c226                	c.swsp	s1,4(sp)
2005b782:	84aa                	c.mv	s1,a0
2005b784:	3775                	c.jal	2005b730 <rtos_mutex_delete>
2005b786:	842a                	c.mv	s0,a0
2005b788:	8526                	c.mv	a0,s1
2005b78a:	26a9                	c.jal	2005bad4 <__reserved_release_mutex_to_poll>
2005b78c:	40b2                	c.lwsp	ra,12(sp)
2005b78e:	8522                	c.mv	a0,s0
2005b790:	4422                	c.lwsp	s0,8(sp)
2005b792:	4492                	c.lwsp	s1,4(sp)
2005b794:	0141                	c.addi	sp,16
2005b796:	8082                	c.jr	ra

2005b798 <rtos_mutex_take>:
2005b798:	7179                	c.addi16sp	sp,-48
2005b79a:	d422                	c.swsp	s0,40(sp)
2005b79c:	c62e                	c.swsp	a1,12(sp)
2005b79e:	d606                	c.swsp	ra,44(sp)
2005b7a0:	842a                	c.mv	s0,a0
2005b7a2:	ce02                	c.swsp	zero,28(sp)
2005b7a4:	ea3ff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005b7a8:	45b2                	c.lwsp	a1,12(sp)
2005b7aa:	c505                	c.beqz	a0,2005b7d2 <rtos_mutex_take+0x3a>
2005b7ac:	0870                	c.addi4spn	a2,sp,28
2005b7ae:	4581                	c.li	a1,0
2005b7b0:	8522                	c.mv	a0,s0
2005b7b2:	4a6010ef          	jal	ra,2005cc58 <xQueueReceiveFromISR>
2005b7b6:	872a                	c.mv	a4,a0
2005b7b8:	4785                	c.li	a5,1
2005b7ba:	557d                	c.li	a0,-1
2005b7bc:	00f71763          	bne	a4,a5,2005b7ca <rtos_mutex_take+0x32>
2005b7c0:	4572                	c.lwsp	a0,28(sp)
2005b7c2:	c501                	c.beqz	a0,2005b7ca <rtos_mutex_take+0x32>
2005b7c4:	004020ef          	jal	ra,2005d7c8 <vTaskSwitchContext>
2005b7c8:	4501                	c.li	a0,0
2005b7ca:	50b2                	c.lwsp	ra,44(sp)
2005b7cc:	5422                	c.lwsp	s0,40(sp)
2005b7ce:	6145                	c.addi16sp	sp,48
2005b7d0:	8082                	c.jr	ra
2005b7d2:	8522                	c.mv	a0,s0
2005b7d4:	366010ef          	jal	ra,2005cb3a <xQueueSemaphoreTake>
2005b7d8:	157d                	c.addi	a0,-1
2005b7da:	00a03533          	sltu	a0,zero,a0
2005b7de:	40a00533          	sub	a0,zero,a0
2005b7e2:	b7e5                	c.j	2005b7ca <rtos_mutex_take+0x32>

2005b7e4 <rtos_mutex_give>:
2005b7e4:	1101                	c.addi	sp,-32
2005b7e6:	cc22                	c.swsp	s0,24(sp)
2005b7e8:	ce06                	c.swsp	ra,28(sp)
2005b7ea:	842a                	c.mv	s0,a0
2005b7ec:	c602                	c.swsp	zero,12(sp)
2005b7ee:	e59ff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005b7f2:	c11d                	c.beqz	a0,2005b818 <rtos_mutex_give+0x34>
2005b7f4:	006c                	c.addi4spn	a1,sp,12
2005b7f6:	8522                	c.mv	a0,s0
2005b7f8:	1fe010ef          	jal	ra,2005c9f6 <xQueueGiveFromISR>
2005b7fc:	872a                	c.mv	a4,a0
2005b7fe:	4785                	c.li	a5,1
2005b800:	557d                	c.li	a0,-1
2005b802:	00f71763          	bne	a4,a5,2005b810 <rtos_mutex_give+0x2c>
2005b806:	4532                	c.lwsp	a0,12(sp)
2005b808:	c501                	c.beqz	a0,2005b810 <rtos_mutex_give+0x2c>
2005b80a:	7bf010ef          	jal	ra,2005d7c8 <vTaskSwitchContext>
2005b80e:	4501                	c.li	a0,0
2005b810:	40f2                	c.lwsp	ra,28(sp)
2005b812:	4462                	c.lwsp	s0,24(sp)
2005b814:	6105                	c.addi16sp	sp,32
2005b816:	8082                	c.jr	ra
2005b818:	4681                	c.li	a3,0
2005b81a:	4601                	c.li	a2,0
2005b81c:	4581                	c.li	a1,0
2005b81e:	8522                	c.mv	a0,s0
2005b820:	068010ef          	jal	ra,2005c888 <xQueueGenericSend>
2005b824:	157d                	c.addi	a0,-1
2005b826:	00a03533          	sltu	a0,zero,a0
2005b82a:	40a00533          	sub	a0,zero,a0
2005b82e:	b7cd                	c.j	2005b810 <rtos_mutex_give+0x2c>

2005b830 <rtos_sema_create>:
2005b830:	1141                	c.addi	sp,-16
2005b832:	c422                	c.swsp	s0,8(sp)
2005b834:	c606                	c.swsp	ra,12(sp)
2005b836:	842a                	c.mv	s0,a0
2005b838:	557d                	c.li	a0,-1
2005b83a:	c809                	c.beqz	s0,2005b84c <rtos_sema_create+0x1c>
2005b83c:	8532                	c.mv	a0,a2
2005b83e:	5b0010ef          	jal	ra,2005cdee <xQueueCreateCountingSemaphore>
2005b842:	c008                	c.sw	a0,0(s0)
2005b844:	00153513          	sltiu	a0,a0,1
2005b848:	40a00533          	sub	a0,zero,a0
2005b84c:	40b2                	c.lwsp	ra,12(sp)
2005b84e:	4422                	c.lwsp	s0,8(sp)
2005b850:	0141                	c.addi	sp,16
2005b852:	8082                	c.jr	ra

2005b854 <rtos_sema_create_static>:
2005b854:	1101                	c.addi	sp,-32
2005b856:	cc22                	c.swsp	s0,24(sp)
2005b858:	ca26                	c.swsp	s1,20(sp)
2005b85a:	c62e                	c.swsp	a1,12(sp)
2005b85c:	ce06                	c.swsp	ra,28(sp)
2005b85e:	842a                	c.mv	s0,a0
2005b860:	84b2                	c.mv	s1,a2
2005b862:	2471                	c.jal	2005baee <__reserved_get_sema_from_poll>
2005b864:	45b2                	c.lwsp	a1,12(sp)
2005b866:	e901                	c.bnez	a0,2005b876 <rtos_sema_create_static+0x22>
2005b868:	8522                	c.mv	a0,s0
2005b86a:	4462                	c.lwsp	s0,24(sp)
2005b86c:	40f2                	c.lwsp	ra,28(sp)
2005b86e:	8626                	c.mv	a2,s1
2005b870:	44d2                	c.lwsp	s1,20(sp)
2005b872:	6105                	c.addi16sp	sp,32
2005b874:	bf75                	c.j	2005b830 <rtos_sema_create>
2005b876:	862a                	c.mv	a2,a0
2005b878:	8526                	c.mv	a0,s1
2005b87a:	604010ef          	jal	ra,2005ce7e <xQueueCreateCountingSemaphoreStatic>
2005b87e:	c008                	c.sw	a0,0(s0)
2005b880:	40f2                	c.lwsp	ra,28(sp)
2005b882:	4462                	c.lwsp	s0,24(sp)
2005b884:	00153513          	sltiu	a0,a0,1
2005b888:	44d2                	c.lwsp	s1,20(sp)
2005b88a:	40a00533          	sub	a0,zero,a0
2005b88e:	6105                	c.addi16sp	sp,32
2005b890:	8082                	c.jr	ra

2005b892 <rtos_sema_create_binary>:
2005b892:	c11d                	c.beqz	a0,2005b8b8 <rtos_sema_create_binary+0x26>
2005b894:	1141                	c.addi	sp,-16
2005b896:	c422                	c.swsp	s0,8(sp)
2005b898:	460d                	c.li	a2,3
2005b89a:	842a                	c.mv	s0,a0
2005b89c:	4581                	c.li	a1,0
2005b89e:	4505                	c.li	a0,1
2005b8a0:	c606                	c.swsp	ra,12(sp)
2005b8a2:	4c0010ef          	jal	ra,2005cd62 <xQueueGenericCreate>
2005b8a6:	c008                	c.sw	a0,0(s0)
2005b8a8:	40b2                	c.lwsp	ra,12(sp)
2005b8aa:	4422                	c.lwsp	s0,8(sp)
2005b8ac:	00153513          	sltiu	a0,a0,1
2005b8b0:	40a00533          	sub	a0,zero,a0
2005b8b4:	0141                	c.addi	sp,16
2005b8b6:	8082                	c.jr	ra
2005b8b8:	557d                	c.li	a0,-1
2005b8ba:	8082                	c.jr	ra

2005b8bc <rtos_sema_delete>:
2005b8bc:	c909                	c.beqz	a0,2005b8ce <rtos_sema_delete+0x12>
2005b8be:	1141                	c.addi	sp,-16
2005b8c0:	c606                	c.swsp	ra,12(sp)
2005b8c2:	606010ef          	jal	ra,2005cec8 <vQueueDelete>
2005b8c6:	40b2                	c.lwsp	ra,12(sp)
2005b8c8:	4501                	c.li	a0,0
2005b8ca:	0141                	c.addi	sp,16
2005b8cc:	8082                	c.jr	ra
2005b8ce:	557d                	c.li	a0,-1
2005b8d0:	8082                	c.jr	ra

2005b8d2 <rtos_sema_take>:
2005b8d2:	7179                	c.addi16sp	sp,-48
2005b8d4:	d422                	c.swsp	s0,40(sp)
2005b8d6:	c62e                	c.swsp	a1,12(sp)
2005b8d8:	d606                	c.swsp	ra,44(sp)
2005b8da:	842a                	c.mv	s0,a0
2005b8dc:	ce02                	c.swsp	zero,28(sp)
2005b8de:	d69ff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005b8e2:	45b2                	c.lwsp	a1,12(sp)
2005b8e4:	c505                	c.beqz	a0,2005b90c <rtos_sema_take+0x3a>
2005b8e6:	0870                	c.addi4spn	a2,sp,28
2005b8e8:	4581                	c.li	a1,0
2005b8ea:	8522                	c.mv	a0,s0
2005b8ec:	36c010ef          	jal	ra,2005cc58 <xQueueReceiveFromISR>
2005b8f0:	872a                	c.mv	a4,a0
2005b8f2:	4785                	c.li	a5,1
2005b8f4:	557d                	c.li	a0,-1
2005b8f6:	00f71763          	bne	a4,a5,2005b904 <rtos_sema_take+0x32>
2005b8fa:	4572                	c.lwsp	a0,28(sp)
2005b8fc:	c501                	c.beqz	a0,2005b904 <rtos_sema_take+0x32>
2005b8fe:	6cb010ef          	jal	ra,2005d7c8 <vTaskSwitchContext>
2005b902:	4501                	c.li	a0,0
2005b904:	50b2                	c.lwsp	ra,44(sp)
2005b906:	5422                	c.lwsp	s0,40(sp)
2005b908:	6145                	c.addi16sp	sp,48
2005b90a:	8082                	c.jr	ra
2005b90c:	8522                	c.mv	a0,s0
2005b90e:	22c010ef          	jal	ra,2005cb3a <xQueueSemaphoreTake>
2005b912:	157d                	c.addi	a0,-1
2005b914:	00a03533          	sltu	a0,zero,a0
2005b918:	40a00533          	sub	a0,zero,a0
2005b91c:	b7e5                	c.j	2005b904 <rtos_sema_take+0x32>

2005b91e <rtos_sema_give>:
2005b91e:	7179                	c.addi16sp	sp,-48
2005b920:	d422                	c.swsp	s0,40(sp)
2005b922:	d606                	c.swsp	ra,44(sp)
2005b924:	842a                	c.mv	s0,a0
2005b926:	ce02                	c.swsp	zero,28(sp)
2005b928:	d1fff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005b92c:	c505                	c.beqz	a0,2005b954 <rtos_sema_give+0x36>
2005b92e:	086c                	c.addi4spn	a1,sp,28
2005b930:	8522                	c.mv	a0,s0
2005b932:	0c4010ef          	jal	ra,2005c9f6 <xQueueGiveFromISR>
2005b936:	47f2                	c.lwsp	a5,28(sp)
2005b938:	c789                	c.beqz	a5,2005b942 <rtos_sema_give+0x24>
2005b93a:	c62a                	c.swsp	a0,12(sp)
2005b93c:	68d010ef          	jal	ra,2005d7c8 <vTaskSwitchContext>
2005b940:	4532                	c.lwsp	a0,12(sp)
2005b942:	50b2                	c.lwsp	ra,44(sp)
2005b944:	5422                	c.lwsp	s0,40(sp)
2005b946:	157d                	c.addi	a0,-1
2005b948:	00a03533          	sltu	a0,zero,a0
2005b94c:	40a00533          	sub	a0,zero,a0
2005b950:	6145                	c.addi16sp	sp,48
2005b952:	8082                	c.jr	ra
2005b954:	4681                	c.li	a3,0
2005b956:	4601                	c.li	a2,0
2005b958:	4581                	c.li	a1,0
2005b95a:	8522                	c.mv	a0,s0
2005b95c:	72d000ef          	jal	ra,2005c888 <xQueueGenericSend>
2005b960:	b7cd                	c.j	2005b942 <rtos_sema_give+0x24>

2005b962 <__reserved_get_from_poll>:
2005b962:	7179                	c.addi16sp	sp,-48
2005b964:	d226                	c.swsp	s1,36(sp)
2005b966:	d04a                	c.swsp	s2,32(sp)
2005b968:	ce4e                	c.swsp	s3,28(sp)
2005b96a:	cc52                	c.swsp	s4,24(sp)
2005b96c:	ca56                	c.swsp	s5,20(sp)
2005b96e:	c85a                	c.swsp	s6,16(sp)
2005b970:	d606                	c.swsp	ra,44(sp)
2005b972:	d422                	c.swsp	s0,40(sp)
2005b974:	c65e                	c.swsp	s7,12(sp)
2005b976:	8a3e                	c.mv	s4,a5
2005b978:	421c                	c.lw	a5,0(a2)
2005b97a:	84aa                	c.mv	s1,a0
2005b97c:	8b2e                	c.mv	s6,a1
2005b97e:	8ab2                	c.mv	s5,a2
2005b980:	8936                	c.mv	s2,a3
2005b982:	89ba                	c.mv	s3,a4
2005b984:	ef85                	c.bnez	a5,2005b9bc <__reserved_get_from_poll+0x5a>
2005b986:	4405                	c.li	s0,1
2005b988:	08851263          	bne	a0,s0,2005ba0c <__reserved_get_from_poll+0xaa>
2005b98c:	f0818413          	addi	s0,gp,-248 # 200667ac <mutex_pool_init_flag>
2005b990:	401c                	c.lw	a5,0(s0)
2005b992:	02a78563          	beq	a5,a0,2005b9bc <__reserved_get_from_poll+0x5a>
2005b996:	4601                	c.li	a2,0
2005b998:	4581                	c.li	a1,0
2005b99a:	5b818513          	addi	a0,gp,1464 # 20066e5c <mutex_pool>
2005b99e:	dffae097          	auipc	ra,0xdffae
2005b9a2:	130080e7          	jalr	ra,304(ra) # 9ace <__wrap_memset>
2005b9a6:	ee01ae23          	sw	zero,-260(gp) # 200667a0 <mutex_buf_used_num>
2005b9aa:	f2c18513          	addi	a0,gp,-212 # 200667d0 <wrapper_mutex_buf_list>
2005b9ae:	f001a223          	sw	zero,-252(gp) # 200667a8 <mutex_max_buf_used_num>
2005b9b2:	c108                	c.sw	a0,0(a0)
2005b9b4:	c148                	c.sw	a0,4(a0)
2005b9b6:	f001a023          	sw	zero,-256(gp) # 200667a4 <mutex_dynamic_num>
2005b9ba:	c004                	c.sw	s1,0(s0)
2005b9bc:	c9dff0ef          	jal	ra,2005b658 <rtos_critical_enter>
2005b9c0:	000b2403          	lw	s0,0(s6)
2005b9c4:	0a8b1b63          	bne	s6,s0,2005ba7a <__reserved_get_from_poll+0x118>
2005b9c8:	cb5ff0ef          	jal	ra,2005b67c <rtos_critical_exit>
2005b9cc:	4b01                	c.li	s6,0
2005b9ce:	000aa783          	lw	a5,0(s5)
2005b9d2:	c791                	c.beqz	a5,2005b9de <__reserved_get_from_poll+0x7c>
2005b9d4:	00092783          	lw	a5,0(s2)
2005b9d8:	0785                	c.addi	a5,1
2005b9da:	00f92023          	sw	a5,0(s2)
2005b9de:	00092703          	lw	a4,0(s2)
2005b9e2:	0009a783          	lw	a5,0(s3)
2005b9e6:	97ba                	c.add	a5,a4
2005b9e8:	000a2703          	lw	a4,0(s4)
2005b9ec:	00f77463          	bgeu	a4,a5,2005b9f4 <__reserved_get_from_poll+0x92>
2005b9f0:	00fa2023          	sw	a5,0(s4)
2005b9f4:	50b2                	c.lwsp	ra,44(sp)
2005b9f6:	5422                	c.lwsp	s0,40(sp)
2005b9f8:	5492                	c.lwsp	s1,36(sp)
2005b9fa:	5902                	c.lwsp	s2,32(sp)
2005b9fc:	49f2                	c.lwsp	s3,28(sp)
2005b9fe:	4a62                	c.lwsp	s4,24(sp)
2005ba00:	4ad2                	c.lwsp	s5,20(sp)
2005ba02:	4bb2                	c.lwsp	s7,12(sp)
2005ba04:	855a                	c.mv	a0,s6
2005ba06:	4b42                	c.lwsp	s6,16(sp)
2005ba08:	6145                	c.addi16sp	sp,48
2005ba0a:	8082                	c.jr	ra
2005ba0c:	4789                	c.li	a5,2
2005ba0e:	02f51d63          	bne	a0,a5,2005ba48 <__reserved_get_from_poll+0xe6>
2005ba12:	f1818b93          	addi	s7,gp,-232 # 200667bc <sema_pool_init_flag>
2005ba16:	000ba783          	lw	a5,0(s7)
2005ba1a:	fa8781e3          	beq	a5,s0,2005b9bc <__reserved_get_from_poll+0x5a>
2005ba1e:	4601                	c.li	a2,0
2005ba20:	4581                	c.li	a1,0
2005ba22:	5b818513          	addi	a0,gp,1464 # 20066e5c <mutex_pool>
2005ba26:	dffae097          	auipc	ra,0xdffae
2005ba2a:	0a8080e7          	jalr	ra,168(ra) # 9ace <__wrap_memset>
2005ba2e:	f001a623          	sw	zero,-244(gp) # 200667b0 <sema_buf_used_num>
2005ba32:	f3418513          	addi	a0,gp,-204 # 200667d8 <wrapper_sema_buf_list>
2005ba36:	f001aa23          	sw	zero,-236(gp) # 200667b8 <sema_max_buf_used_num>
2005ba3a:	c108                	c.sw	a0,0(a0)
2005ba3c:	c148                	c.sw	a0,4(a0)
2005ba3e:	f001a823          	sw	zero,-240(gp) # 200667b4 <sema_dynamic_num>
2005ba42:	008ba023          	sw	s0,0(s7)
2005ba46:	bf9d                	c.j	2005b9bc <__reserved_get_from_poll+0x5a>
2005ba48:	f2818b93          	addi	s7,gp,-216 # 200667cc <timer_pool_init_flag>
2005ba4c:	000ba783          	lw	a5,0(s7)
2005ba50:	f68786e3          	beq	a5,s0,2005b9bc <__reserved_get_from_poll+0x5a>
2005ba54:	4601                	c.li	a2,0
2005ba56:	4581                	c.li	a1,0
2005ba58:	5b818513          	addi	a0,gp,1464 # 20066e5c <mutex_pool>
2005ba5c:	dffae097          	auipc	ra,0xdffae
2005ba60:	072080e7          	jalr	ra,114(ra) # 9ace <__wrap_memset>
2005ba64:	f001ae23          	sw	zero,-228(gp) # 200667c0 <timer_buf_used_num>
2005ba68:	f3c18513          	addi	a0,gp,-196 # 200667e0 <wrapper_timer_buf_list>
2005ba6c:	f201a223          	sw	zero,-220(gp) # 200667c8 <timer_max_buf_used_num>
2005ba70:	c108                	c.sw	a0,0(a0)
2005ba72:	c148                	c.sw	a0,4(a0)
2005ba74:	f201a023          	sw	zero,-224(gp) # 200667c4 <timer_dynamic_num>
2005ba78:	b7e9                	c.j	2005ba42 <__reserved_get_from_poll+0xe0>
2005ba7a:	405c                	c.lw	a5,4(s0)
2005ba7c:	4018                	c.lw	a4,0(s0)
2005ba7e:	00840b13          	addi	s6,s0,8
2005ba82:	c35c                	c.sw	a5,4(a4)
2005ba84:	c398                	c.sw	a4,0(a5)
2005ba86:	c000                	c.sw	s0,0(s0)
2005ba88:	c040                	c.sw	s0,4(s0)
2005ba8a:	0009a783          	lw	a5,0(s3)
2005ba8e:	0785                	c.addi	a5,1
2005ba90:	00f9a023          	sw	a5,0(s3)
2005ba94:	be9ff0ef          	jal	ra,2005b67c <rtos_critical_exit>
2005ba98:	57e1                	c.li	a5,-8
2005ba9a:	f2f40ae3          	beq	s0,a5,2005b9ce <__reserved_get_from_poll+0x6c>
2005ba9e:	478d                	c.li	a5,3
2005baa0:	05000613          	addi	a2,zero,80
2005baa4:	00f49463          	bne	s1,a5,2005baac <__reserved_get_from_poll+0x14a>
2005baa8:	02c00613          	addi	a2,zero,44
2005baac:	4581                	c.li	a1,0
2005baae:	855a                	c.mv	a0,s6
2005bab0:	dffae097          	auipc	ra,0xdffae
2005bab4:	01e080e7          	jalr	ra,30(ra) # 9ace <__wrap_memset>
2005bab8:	b71d                	c.j	2005b9de <__reserved_get_from_poll+0x7c>

2005baba <__reserved_get_mutex_from_poll>:
2005baba:	f0418793          	addi	a5,gp,-252 # 200667a8 <mutex_max_buf_used_num>
2005babe:	efc18713          	addi	a4,gp,-260 # 200667a0 <mutex_buf_used_num>
2005bac2:	f0018693          	addi	a3,gp,-256 # 200667a4 <mutex_dynamic_num>
2005bac6:	f0818613          	addi	a2,gp,-248 # 200667ac <mutex_pool_init_flag>
2005baca:	f2c18593          	addi	a1,gp,-212 # 200667d0 <wrapper_mutex_buf_list>
2005bace:	4505                	c.li	a0,1
2005bad0:	e93ff06f          	jal	zero,2005b962 <__reserved_get_from_poll>

2005bad4 <__reserved_release_mutex_to_poll>:
2005bad4:	1141                	c.addi	sp,-16
2005bad6:	c606                	c.swsp	ra,12(sp)
2005bad8:	b81ff0ef          	jal	ra,2005b658 <rtos_critical_enter>
2005badc:	f0018793          	addi	a5,gp,-256 # 200667a4 <mutex_dynamic_num>
2005bae0:	4398                	c.lw	a4,0(a5)
2005bae2:	177d                	c.addi	a4,-1
2005bae4:	c398                	c.sw	a4,0(a5)
2005bae6:	40b2                	c.lwsp	ra,12(sp)
2005bae8:	0141                	c.addi	sp,16
2005baea:	b93ff06f          	jal	zero,2005b67c <rtos_critical_exit>

2005baee <__reserved_get_sema_from_poll>:
2005baee:	f1418793          	addi	a5,gp,-236 # 200667b8 <sema_max_buf_used_num>
2005baf2:	f0c18713          	addi	a4,gp,-244 # 200667b0 <sema_buf_used_num>
2005baf6:	f1018693          	addi	a3,gp,-240 # 200667b4 <sema_dynamic_num>
2005bafa:	f1818613          	addi	a2,gp,-232 # 200667bc <sema_pool_init_flag>
2005bafe:	f3418593          	addi	a1,gp,-204 # 200667d8 <wrapper_sema_buf_list>
2005bb02:	4509                	c.li	a0,2
2005bb04:	e5fff06f          	jal	zero,2005b962 <__reserved_get_from_poll>

2005bb08 <__reserved_get_timer_from_poll>:
2005bb08:	f2418793          	addi	a5,gp,-220 # 200667c8 <timer_max_buf_used_num>
2005bb0c:	f1c18713          	addi	a4,gp,-228 # 200667c0 <timer_buf_used_num>
2005bb10:	f2018693          	addi	a3,gp,-224 # 200667c4 <timer_dynamic_num>
2005bb14:	f2818613          	addi	a2,gp,-216 # 200667cc <timer_pool_init_flag>
2005bb18:	f3c18593          	addi	a1,gp,-196 # 200667e0 <wrapper_timer_buf_list>
2005bb1c:	450d                	c.li	a0,3
2005bb1e:	e45ff06f          	jal	zero,2005b962 <__reserved_get_from_poll>

2005bb22 <__reserved_release_timer_to_poll>:
2005bb22:	1141                	c.addi	sp,-16
2005bb24:	c606                	c.swsp	ra,12(sp)
2005bb26:	b33ff0ef          	jal	ra,2005b658 <rtos_critical_enter>
2005bb2a:	f2018793          	addi	a5,gp,-224 # 200667c4 <timer_dynamic_num>
2005bb2e:	4398                	c.lw	a4,0(a5)
2005bb30:	177d                	c.addi	a4,-1
2005bb32:	c398                	c.sw	a4,0(a5)
2005bb34:	40b2                	c.lwsp	ra,12(sp)
2005bb36:	0141                	c.addi	sp,16
2005bb38:	b45ff06f          	jal	zero,2005b67c <rtos_critical_exit>

2005bb3c <rtos_sched_start>:
2005bb3c:	1141                	c.addi	sp,-16
2005bb3e:	c606                	c.swsp	ra,12(sp)
2005bb40:	1cc020ef          	jal	ra,2005dd0c <vTaskStartScheduler>
2005bb44:	40b2                	c.lwsp	ra,12(sp)
2005bb46:	4501                	c.li	a0,0
2005bb48:	0141                	c.addi	sp,16
2005bb4a:	8082                	c.jr	ra

2005bb4c <rtos_sched_get_state>:
2005bb4c:	1141                	c.addi	sp,-16
2005bb4e:	c606                	c.swsp	ra,12(sp)
2005bb50:	663010ef          	jal	ra,2005d9b2 <xTaskGetSchedulerState>
2005bb54:	4785                	c.li	a5,1
2005bb56:	00f50b63          	beq	a0,a5,2005bb6c <rtos_sched_get_state+0x20>
2005bb5a:	659010ef          	jal	ra,2005d9b2 <xTaskGetSchedulerState>
2005bb5e:	c519                	c.beqz	a0,2005bb6c <rtos_sched_get_state+0x20>
2005bb60:	653010ef          	jal	ra,2005d9b2 <xTaskGetSchedulerState>
2005bb64:	4789                	c.li	a5,2
2005bb66:	00f50363          	beq	a0,a5,2005bb6c <rtos_sched_get_state+0x20>
2005bb6a:	557d                	c.li	a0,-1
2005bb6c:	40b2                	c.lwsp	ra,12(sp)
2005bb6e:	0141                	c.addi	sp,16
2005bb70:	8082                	c.jr	ra

2005bb72 <rtos_task_create>:
2005bb72:	882a                	c.mv	a6,a0
2005bb74:	8532                	c.mv	a0,a2
2005bb76:	863a                	c.mv	a2,a4
2005bb78:	1141                	c.addi	sp,-16
2005bb7a:	873e                	c.mv	a4,a5
2005bb7c:	8209                	c.srli	a2,0x2
2005bb7e:	87c2                	c.mv	a5,a6
2005bb80:	c606                	c.swsp	ra,12(sp)
2005bb82:	1f2020ef          	jal	ra,2005dd74 <xTaskCreate>
2005bb86:	40b2                	c.lwsp	ra,12(sp)
2005bb88:	157d                	c.addi	a0,-1
2005bb8a:	00a03533          	sltu	a0,zero,a0
2005bb8e:	40a00533          	sub	a0,zero,a0
2005bb92:	0141                	c.addi	sp,16
2005bb94:	8082                	c.jr	ra

2005bb96 <rtos_task_delete>:
2005bb96:	1141                	c.addi	sp,-16
2005bb98:	c606                	c.swsp	ra,12(sp)
2005bb9a:	252020ef          	jal	ra,2005ddec <vTaskDelete>
2005bb9e:	40b2                	c.lwsp	ra,12(sp)
2005bba0:	4501                	c.li	a0,0
2005bba2:	0141                	c.addi	sp,16
2005bba4:	8082                	c.jr	ra

2005bba6 <rtos_create_secure_context>:
2005bba6:	8082                	c.jr	ra

2005bba8 <rtos_time_delay_ms>:
2005bba8:	1141                	c.addi	sp,-16
2005bbaa:	c422                	c.swsp	s0,8(sp)
2005bbac:	c606                	c.swsp	ra,12(sp)
2005bbae:	842a                	c.mv	s0,a0
2005bbb0:	c47fd0ef          	jal	ra,200597f6 <pmu_yield_os_check>
2005bbb4:	cd19                	c.beqz	a0,2005bbd2 <rtos_time_delay_ms+0x2a>
2005bbb6:	5fd010ef          	jal	ra,2005d9b2 <xTaskGetSchedulerState>
2005bbba:	4789                	c.li	a5,2
2005bbbc:	00f51b63          	bne	a0,a5,2005bbd2 <rtos_time_delay_ms+0x2a>
2005bbc0:	a87ff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005bbc4:	e519                	c.bnez	a0,2005bbd2 <rtos_time_delay_ms+0x2a>
2005bbc6:	8522                	c.mv	a0,s0
2005bbc8:	4422                	c.lwsp	s0,8(sp)
2005bbca:	40b2                	c.lwsp	ra,12(sp)
2005bbcc:	0141                	c.addi	sp,16
2005bbce:	4380206f          	jal	zero,2005e006 <vTaskDelay>
2005bbd2:	8522                	c.mv	a0,s0
2005bbd4:	4422                	c.lwsp	s0,8(sp)
2005bbd6:	40b2                	c.lwsp	ra,12(sp)
2005bbd8:	0141                	c.addi	sp,16
2005bbda:	dffa6317          	auipc	t1,0xdffa6
2005bbde:	74030067          	jalr	zero,1856(t1) # 231a <DelayMs>

2005bbe2 <rtos_time_get_current_system_time_ms>:
2005bbe2:	1141                	c.addi	sp,-16
2005bbe4:	c606                	c.swsp	ra,12(sp)
2005bbe6:	a61ff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005bbea:	c509                	c.beqz	a0,2005bbf4 <rtos_time_get_current_system_time_ms+0x12>
2005bbec:	40b2                	c.lwsp	ra,12(sp)
2005bbee:	0141                	c.addi	sp,16
2005bbf0:	28b0106f          	jal	zero,2005d67a <xTaskGetTickCountFromISR>
2005bbf4:	c03fd0ef          	jal	ra,200597f6 <pmu_yield_os_check>
2005bbf8:	d975                	c.beqz	a0,2005bbec <rtos_time_get_current_system_time_ms+0xa>
2005bbfa:	40b2                	c.lwsp	ra,12(sp)
2005bbfc:	0141                	c.addi	sp,16
2005bbfe:	6ae0206f          	jal	zero,2005e2ac <xTaskGetTickCount>

2005bc02 <rtos_timer_create>:
2005bc02:	1141                	c.addi	sp,-16
2005bc04:	c422                	c.swsp	s0,8(sp)
2005bc06:	c606                	c.swsp	ra,12(sp)
2005bc08:	842a                	c.mv	s0,a0
2005bc0a:	587d                	c.li	a6,-1
2005bc0c:	c105                	c.beqz	a0,2005bc2c <rtos_timer_create+0x2a>
2005bc0e:	88b2                	c.mv	a7,a2
2005bc10:	587d                	c.li	a6,-1
2005bc12:	863a                	c.mv	a2,a4
2005bc14:	873e                	c.mv	a4,a5
2005bc16:	cb99                	c.beqz	a5,2005bc2c <rtos_timer_create+0x2a>
2005bc18:	852e                	c.mv	a0,a1
2005bc1a:	85b6                	c.mv	a1,a3
2005bc1c:	86c6                	c.mv	a3,a7
2005bc1e:	0d5020ef          	jal	ra,2005e4f2 <xTimerCreate>
2005bc22:	c008                	c.sw	a0,0(s0)
2005bc24:	00153513          	sltiu	a0,a0,1
2005bc28:	40a00833          	sub	a6,zero,a0
2005bc2c:	40b2                	c.lwsp	ra,12(sp)
2005bc2e:	4422                	c.lwsp	s0,8(sp)
2005bc30:	8542                	c.mv	a0,a6
2005bc32:	0141                	c.addi	sp,16
2005bc34:	8082                	c.jr	ra

2005bc36 <rtos_timer_create_static>:
2005bc36:	1101                	c.addi	sp,-32
2005bc38:	cc22                	c.swsp	s0,24(sp)
2005bc3a:	ca26                	c.swsp	s1,20(sp)
2005bc3c:	c84a                	c.swsp	s2,16(sp)
2005bc3e:	84ae                	c.mv	s1,a1
2005bc40:	c632                	c.swsp	a2,12(sp)
2005bc42:	893a                	c.mv	s2,a4
2005bc44:	ce06                	c.swsp	ra,28(sp)
2005bc46:	842a                	c.mv	s0,a0
2005bc48:	c436                	c.swsp	a3,8(sp)
2005bc4a:	c23e                	c.swsp	a5,4(sp)
2005bc4c:	ebdff0ef          	jal	ra,2005bb08 <__reserved_get_timer_from_poll>
2005bc50:	4712                	c.lwsp	a4,4(sp)
2005bc52:	45a2                	c.lwsp	a1,8(sp)
2005bc54:	4632                	c.lwsp	a2,12(sp)
2005bc56:	ed09                	c.bnez	a0,2005bc70 <rtos_timer_create_static+0x3a>
2005bc58:	8522                	c.mv	a0,s0
2005bc5a:	4462                	c.lwsp	s0,24(sp)
2005bc5c:	40f2                	c.lwsp	ra,28(sp)
2005bc5e:	87ba                	c.mv	a5,a4
2005bc60:	86ae                	c.mv	a3,a1
2005bc62:	874a                	c.mv	a4,s2
2005bc64:	85a6                	c.mv	a1,s1
2005bc66:	4942                	c.lwsp	s2,16(sp)
2005bc68:	44d2                	c.lwsp	s1,20(sp)
2005bc6a:	6105                	c.addi16sp	sp,32
2005bc6c:	f97ff06f          	jal	zero,2005bc02 <rtos_timer_create>
2005bc70:	87aa                	c.mv	a5,a0
2005bc72:	86b2                	c.mv	a3,a2
2005bc74:	8526                	c.mv	a0,s1
2005bc76:	864a                	c.mv	a2,s2
2005bc78:	0e3020ef          	jal	ra,2005e55a <xTimerCreateStatic>
2005bc7c:	c008                	c.sw	a0,0(s0)
2005bc7e:	40f2                	c.lwsp	ra,28(sp)
2005bc80:	4462                	c.lwsp	s0,24(sp)
2005bc82:	00153513          	sltiu	a0,a0,1
2005bc86:	44d2                	c.lwsp	s1,20(sp)
2005bc88:	4942                	c.lwsp	s2,16(sp)
2005bc8a:	40a00533          	sub	a0,zero,a0
2005bc8e:	6105                	c.addi16sp	sp,32
2005bc90:	8082                	c.jr	ra

2005bc92 <rtos_timer_delete>:
2005bc92:	c10d                	c.beqz	a0,2005bcb4 <rtos_timer_delete+0x22>
2005bc94:	1141                	c.addi	sp,-16
2005bc96:	872e                	c.mv	a4,a1
2005bc98:	4681                	c.li	a3,0
2005bc9a:	4601                	c.li	a2,0
2005bc9c:	4595                	c.li	a1,5
2005bc9e:	c606                	c.swsp	ra,12(sp)
2005bca0:	11d020ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005bca4:	40b2                	c.lwsp	ra,12(sp)
2005bca6:	157d                	c.addi	a0,-1
2005bca8:	00a03533          	sltu	a0,zero,a0
2005bcac:	40a00533          	sub	a0,zero,a0
2005bcb0:	0141                	c.addi	sp,16
2005bcb2:	8082                	c.jr	ra
2005bcb4:	557d                	c.li	a0,-1
2005bcb6:	8082                	c.jr	ra

2005bcb8 <rtos_timer_delete_static>:
2005bcb8:	1141                	c.addi	sp,-16
2005bcba:	c422                	c.swsp	s0,8(sp)
2005bcbc:	c226                	c.swsp	s1,4(sp)
2005bcbe:	c606                	c.swsp	ra,12(sp)
2005bcc0:	842a                	c.mv	s0,a0
2005bcc2:	84ae                	c.mv	s1,a1
2005bcc4:	85a6                	c.mv	a1,s1
2005bcc6:	8522                	c.mv	a0,s0
2005bcc8:	fcbff0ef          	jal	ra,2005bc92 <rtos_timer_delete>
2005bccc:	fd65                	c.bnez	a0,2005bcc4 <rtos_timer_delete_static+0xc>
2005bcce:	8522                	c.mv	a0,s0
2005bcd0:	e53ff0ef          	jal	ra,2005bb22 <__reserved_release_timer_to_poll>
2005bcd4:	40b2                	c.lwsp	ra,12(sp)
2005bcd6:	4422                	c.lwsp	s0,8(sp)
2005bcd8:	4492                	c.lwsp	s1,4(sp)
2005bcda:	4501                	c.li	a0,0
2005bcdc:	0141                	c.addi	sp,16
2005bcde:	8082                	c.jr	ra

2005bce0 <rtos_timer_start>:
2005bce0:	e909                	c.bnez	a0,2005bcf2 <rtos_timer_start+0x12>
2005bce2:	557d                	c.li	a0,-1
2005bce4:	8082                	c.jr	ra
2005bce6:	557d                	c.li	a0,-1
2005bce8:	40f2                	c.lwsp	ra,28(sp)
2005bcea:	4462                	c.lwsp	s0,24(sp)
2005bcec:	44d2                	c.lwsp	s1,20(sp)
2005bcee:	6105                	c.addi16sp	sp,32
2005bcf0:	8082                	c.jr	ra
2005bcf2:	1101                	c.addi	sp,-32
2005bcf4:	cc22                	c.swsp	s0,24(sp)
2005bcf6:	ca26                	c.swsp	s1,20(sp)
2005bcf8:	ce06                	c.swsp	ra,28(sp)
2005bcfa:	842a                	c.mv	s0,a0
2005bcfc:	84ae                	c.mv	s1,a1
2005bcfe:	949ff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005bd02:	c505                	c.beqz	a0,2005bd2a <rtos_timer_start+0x4a>
2005bd04:	c602                	c.swsp	zero,12(sp)
2005bd06:	175010ef          	jal	ra,2005d67a <xTaskGetTickCountFromISR>
2005bd0a:	862a                	c.mv	a2,a0
2005bd0c:	4701                	c.li	a4,0
2005bd0e:	0074                	c.addi4spn	a3,sp,12
2005bd10:	4599                	c.li	a1,6
2005bd12:	8522                	c.mv	a0,s0
2005bd14:	0a9020ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005bd18:	4785                	c.li	a5,1
2005bd1a:	fcf516e3          	bne	a0,a5,2005bce6 <rtos_timer_start+0x6>
2005bd1e:	4532                	c.lwsp	a0,12(sp)
2005bd20:	d561                	c.beqz	a0,2005bce8 <rtos_timer_start+0x8>
2005bd22:	2a7010ef          	jal	ra,2005d7c8 <vTaskSwitchContext>
2005bd26:	4501                	c.li	a0,0
2005bd28:	b7c1                	c.j	2005bce8 <rtos_timer_start+0x8>
2005bd2a:	582020ef          	jal	ra,2005e2ac <xTaskGetTickCount>
2005bd2e:	862a                	c.mv	a2,a0
2005bd30:	8726                	c.mv	a4,s1
2005bd32:	4681                	c.li	a3,0
2005bd34:	4585                	c.li	a1,1
2005bd36:	8522                	c.mv	a0,s0
2005bd38:	085020ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005bd3c:	157d                	c.addi	a0,-1
2005bd3e:	00a03533          	sltu	a0,zero,a0
2005bd42:	40a00533          	sub	a0,zero,a0
2005bd46:	b74d                	c.j	2005bce8 <rtos_timer_start+0x8>

2005bd48 <rtos_timer_stop>:
2005bd48:	e901                	c.bnez	a0,2005bd58 <rtos_timer_stop+0x10>
2005bd4a:	557d                	c.li	a0,-1
2005bd4c:	8082                	c.jr	ra
2005bd4e:	557d                	c.li	a0,-1
2005bd50:	50b2                	c.lwsp	ra,44(sp)
2005bd52:	5422                	c.lwsp	s0,40(sp)
2005bd54:	6145                	c.addi16sp	sp,48
2005bd56:	8082                	c.jr	ra
2005bd58:	7179                	c.addi16sp	sp,-48
2005bd5a:	d422                	c.swsp	s0,40(sp)
2005bd5c:	d606                	c.swsp	ra,44(sp)
2005bd5e:	842a                	c.mv	s0,a0
2005bd60:	c62e                	c.swsp	a1,12(sp)
2005bd62:	8e5ff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005bd66:	4732                	c.lwsp	a4,12(sp)
2005bd68:	c115                	c.beqz	a0,2005bd8c <rtos_timer_stop+0x44>
2005bd6a:	4701                	c.li	a4,0
2005bd6c:	0874                	c.addi4spn	a3,sp,28
2005bd6e:	4601                	c.li	a2,0
2005bd70:	45a1                	c.li	a1,8
2005bd72:	8522                	c.mv	a0,s0
2005bd74:	ce02                	c.swsp	zero,28(sp)
2005bd76:	047020ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005bd7a:	4785                	c.li	a5,1
2005bd7c:	fcf519e3          	bne	a0,a5,2005bd4e <rtos_timer_stop+0x6>
2005bd80:	4572                	c.lwsp	a0,28(sp)
2005bd82:	d579                	c.beqz	a0,2005bd50 <rtos_timer_stop+0x8>
2005bd84:	245010ef          	jal	ra,2005d7c8 <vTaskSwitchContext>
2005bd88:	4501                	c.li	a0,0
2005bd8a:	b7d9                	c.j	2005bd50 <rtos_timer_stop+0x8>
2005bd8c:	4681                	c.li	a3,0
2005bd8e:	4601                	c.li	a2,0
2005bd90:	458d                	c.li	a1,3
2005bd92:	8522                	c.mv	a0,s0
2005bd94:	029020ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005bd98:	157d                	c.addi	a0,-1
2005bd9a:	00a03533          	sltu	a0,zero,a0
2005bd9e:	40a00533          	sub	a0,zero,a0
2005bda2:	b77d                	c.j	2005bd50 <rtos_timer_stop+0x8>

2005bda4 <rtos_timer_change_period>:
2005bda4:	e909                	c.bnez	a0,2005bdb6 <rtos_timer_change_period+0x12>
2005bda6:	557d                	c.li	a0,-1
2005bda8:	8082                	c.jr	ra
2005bdaa:	557d                	c.li	a0,-1
2005bdac:	50b2                	c.lwsp	ra,44(sp)
2005bdae:	5422                	c.lwsp	s0,40(sp)
2005bdb0:	5492                	c.lwsp	s1,36(sp)
2005bdb2:	6145                	c.addi16sp	sp,48
2005bdb4:	8082                	c.jr	ra
2005bdb6:	7179                	c.addi16sp	sp,-48
2005bdb8:	d422                	c.swsp	s0,40(sp)
2005bdba:	d226                	c.swsp	s1,36(sp)
2005bdbc:	d606                	c.swsp	ra,44(sp)
2005bdbe:	842a                	c.mv	s0,a0
2005bdc0:	84ae                	c.mv	s1,a1
2005bdc2:	c632                	c.swsp	a2,12(sp)
2005bdc4:	883ff0ef          	jal	ra,2005b646 <rtos_critical_is_in_interrupt>
2005bdc8:	4732                	c.lwsp	a4,12(sp)
2005bdca:	c115                	c.beqz	a0,2005bdee <rtos_timer_change_period+0x4a>
2005bdcc:	4701                	c.li	a4,0
2005bdce:	0874                	c.addi4spn	a3,sp,28
2005bdd0:	8626                	c.mv	a2,s1
2005bdd2:	45a5                	c.li	a1,9
2005bdd4:	8522                	c.mv	a0,s0
2005bdd6:	ce02                	c.swsp	zero,28(sp)
2005bdd8:	7e4020ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005bddc:	4785                	c.li	a5,1
2005bdde:	fcf516e3          	bne	a0,a5,2005bdaa <rtos_timer_change_period+0x6>
2005bde2:	4572                	c.lwsp	a0,28(sp)
2005bde4:	d561                	c.beqz	a0,2005bdac <rtos_timer_change_period+0x8>
2005bde6:	1e3010ef          	jal	ra,2005d7c8 <vTaskSwitchContext>
2005bdea:	4501                	c.li	a0,0
2005bdec:	b7c1                	c.j	2005bdac <rtos_timer_change_period+0x8>
2005bdee:	4681                	c.li	a3,0
2005bdf0:	8626                	c.mv	a2,s1
2005bdf2:	4591                	c.li	a1,4
2005bdf4:	8522                	c.mv	a0,s0
2005bdf6:	7c6020ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005bdfa:	157d                	c.addi	a0,-1
2005bdfc:	00a03533          	sltu	a0,zero,a0
2005be00:	40a00533          	sub	a0,zero,a0
2005be04:	b765                	c.j	2005bdac <rtos_timer_change_period+0x8>

2005be06 <rtos_timer_is_timer_active>:
2005be06:	2990206f          	jal	zero,2005e89e <xTimerIsTimerActive>

2005be0a <platform_calloc_uninit>:
2005be0a:	4501                	c.li	a0,0
2005be0c:	8082                	c.jr	ra

2005be0e <platform_free_uninit>:
2005be0e:	8082                	c.jr	ra

2005be10 <mbedtls_platform_set_calloc_free>:
2005be10:	e8a1a223          	sw	a0,-380(gp) # 20066728 <mbedtls_calloc_func>
2005be14:	e8b1a423          	sw	a1,-376(gp) # 2006672c <mbedtls_free_func>
2005be18:	200017b7          	lui	a5,0x20001
2005be1c:	c6c78793          	addi	a5,a5,-916 # 20000c6c <rom_ssl_ram_map>
2005be20:	20001737          	lui	a4,0x20001
2005be24:	c388                	c.sw	a0,0(a5)
2005be26:	c6f72423          	sw	a5,-920(a4) # 20000c68 <p_rom_ssl_ram_map>
2005be2a:	c3cc                	c.sw	a1,4(a5)
2005be2c:	4501                	c.li	a0,0
2005be2e:	8082                	c.jr	ra

2005be30 <freertos_risc_v_trap_handler>:
2005be30:	ef810113          	addi	sp,sp,-264
2005be34:	c206                	c.swsp	ra,4(sp)
2005be36:	ca16                	c.swsp	t0,20(sp)
2005be38:	cc1a                	c.swsp	t1,24(sp)
2005be3a:	ce1e                	c.swsp	t2,28(sp)
2005be3c:	d022                	c.swsp	s0,32(sp)
2005be3e:	d226                	c.swsp	s1,36(sp)
2005be40:	d42a                	c.swsp	a0,40(sp)
2005be42:	d62e                	c.swsp	a1,44(sp)
2005be44:	d832                	c.swsp	a2,48(sp)
2005be46:	da36                	c.swsp	a3,52(sp)
2005be48:	dc3a                	c.swsp	a4,56(sp)
2005be4a:	de3e                	c.swsp	a5,60(sp)
2005be4c:	c0c2                	c.swsp	a6,64(sp)
2005be4e:	c2c6                	c.swsp	a7,68(sp)
2005be50:	c4ca                	c.swsp	s2,72(sp)
2005be52:	c6ce                	c.swsp	s3,76(sp)
2005be54:	c8d2                	c.swsp	s4,80(sp)
2005be56:	cad6                	c.swsp	s5,84(sp)
2005be58:	ccda                	c.swsp	s6,88(sp)
2005be5a:	cede                	c.swsp	s7,92(sp)
2005be5c:	d0e2                	c.swsp	s8,96(sp)
2005be5e:	d2e6                	c.swsp	s9,100(sp)
2005be60:	d4ea                	c.swsp	s10,104(sp)
2005be62:	d6ee                	c.swsp	s11,108(sp)
2005be64:	d8f2                	c.swsp	t3,112(sp)
2005be66:	daf6                	c.swsp	t4,116(sp)
2005be68:	dcfa                	c.swsp	t5,120(sp)
2005be6a:	defe                	c.swsp	t6,124(sp)
2005be6c:	300022f3          	csrrs	t0,mstatus,zero
2005be70:	6319                	c.lui	t1,0x6
2005be72:	0062f2b3          	and	t0,t0,t1
2005be76:	6309                	c.lui	t1,0x2
2005be78:	04628663          	beq	t0,t1,2005bec4 <freertos_risc_v_trap_handler+0x94>
2005be7c:	003022f3          	csrrs	t0,fcsr,zero
2005be80:	e102                	c.fswsp	ft0,128(sp)
2005be82:	e306                	c.fswsp	ft1,132(sp)
2005be84:	e50a                	c.fswsp	ft2,136(sp)
2005be86:	e70e                	c.fswsp	ft3,140(sp)
2005be88:	e912                	c.fswsp	ft4,144(sp)
2005be8a:	eb16                	c.fswsp	ft5,148(sp)
2005be8c:	ed1a                	c.fswsp	ft6,152(sp)
2005be8e:	ef1e                	c.fswsp	ft7,156(sp)
2005be90:	f122                	c.fswsp	fs0,160(sp)
2005be92:	f326                	c.fswsp	fs1,164(sp)
2005be94:	f52a                	c.fswsp	fa0,168(sp)
2005be96:	f72e                	c.fswsp	fa1,172(sp)
2005be98:	f932                	c.fswsp	fa2,176(sp)
2005be9a:	fb36                	c.fswsp	fa3,180(sp)
2005be9c:	fd3a                	c.fswsp	fa4,184(sp)
2005be9e:	ff3e                	c.fswsp	fa5,188(sp)
2005bea0:	e1c2                	c.fswsp	fa6,192(sp)
2005bea2:	e3c6                	c.fswsp	fa7,196(sp)
2005bea4:	e5ca                	c.fswsp	fs2,200(sp)
2005bea6:	e7ce                	c.fswsp	fs3,204(sp)
2005bea8:	e9d2                	c.fswsp	fs4,208(sp)
2005beaa:	ebd6                	c.fswsp	fs5,212(sp)
2005beac:	edda                	c.fswsp	fs6,216(sp)
2005beae:	efde                	c.fswsp	fs7,220(sp)
2005beb0:	f1e2                	c.fswsp	fs8,224(sp)
2005beb2:	f3e6                	c.fswsp	fs9,228(sp)
2005beb4:	f5ea                	c.fswsp	fs10,232(sp)
2005beb6:	f7ee                	c.fswsp	fs11,236(sp)
2005beb8:	f9f2                	c.fswsp	ft8,240(sp)
2005beba:	fbf6                	c.fswsp	ft9,244(sp)
2005bebc:	fdfa                	c.fswsp	ft10,248(sp)
2005bebe:	fffe                	c.fswsp	ft11,252(sp)
2005bec0:	10512023          	sw	t0,256(sp)
2005bec4:	300022f3          	csrrs	t0,mstatus,zero
2005bec8:	10512223          	sw	t0,260(sp)
2005becc:	34202573          	csrrs	a0,mcause,zero
2005bed0:	341025f3          	csrrs	a1,mepc,zero

2005bed4 <test_if_asynchronous>:
2005bed4:	01f55613          	srli	a2,a0,0x1f
2005bed8:	e225                	c.bnez	a2,2005bf38 <handle_asynchronous>

2005beda <handle_synchronous>:
2005beda:	0591                	c.addi	a1,4
2005bedc:	c02e                	c.swsp	a1,0(sp)

2005bede <test_if_environment_call>:
2005bede:	42ad                	c.li	t0,11
2005bee0:	00a2cd63          	blt	t0,a0,2005befa <_crash_dump>
2005bee4:	42a1                	c.li	t0,8
2005bee6:	00554a63          	blt	a0,t0,2005befa <_crash_dump>
2005beea:	a009                	c.j	2005beec <ecall_yield>

2005beec <ecall_yield>:
2005beec:	f601a283          	lw	t0,-160(gp) # 20066804 <pxCurrentTCB>
2005bef0:	0022a023          	sw	sp,0(t0)
2005bef4:	e601a103          	lw	sp,-416(gp) # 20066704 <xISRStackTop>
2005bef8:	a0a5                	c.j	2005bf60 <task_context_switch>

2005befa <_crash_dump>:
2005befa:	34102573          	csrrs	a0,mepc,zero
2005befe:	022c                	c.addi4spn	a1,sp,264
2005bf00:	860a                	c.mv	a2,sp
2005bf02:	c002                	c.swsp	zero,0(sp)
2005bf04:	c42e                	c.swsp	a1,8(sp)
2005bf06:	c60e                	c.swsp	gp,12(sp)
2005bf08:	c812                	c.swsp	tp,16(sp)
2005bf0a:	e601a303          	lw	t1,-416(gp) # 20066704 <xISRStackTop>
2005bf0e:	0000b397          	auipc	t2,0xb
2005bf12:	f5238393          	addi	t2,t2,-174 # 20066e60 <xISRStack>
2005bf16:	08038393          	addi	t2,t2,128
2005bf1a:	00615563          	bge	sp,t1,2005bf24 <not_isrstack>
2005bf1e:	00714363          	blt	sp,t2,2005bf24 <not_isrstack>
2005bf22:	a019                	c.j	2005bf28 <is_isrstack>

2005bf24 <not_isrstack>:
2005bf24:	e601a103          	lw	sp,-416(gp) # 20066704 <xISRStackTop>

2005bf28 <is_isrstack>:
2005bf28:	00004297          	auipc	t0,0x4
2005bf2c:	07828293          	addi	t0,t0,120 # 2005ffa0 <crash_dump>
2005bf30:	9282                	c.jalr	t0

2005bf32 <crash_dump_end>:
2005bf32:	a001                	c.j	2005bf32 <crash_dump_end>

2005bf34 <unrecoverable_error>:
2005bf34:	9002                	c.ebreak
2005bf36:	bffd                	c.j	2005bf34 <unrecoverable_error>

2005bf38 <handle_asynchronous>:
2005bf38:	c02e                	c.swsp	a1,0(sp)
2005bf3a:	f601a283          	lw	t0,-160(gp) # 20066804 <pxCurrentTCB>
2005bf3e:	0022a023          	sw	sp,0(t0)
2005bf42:	e601a103          	lw	sp,-416(gp) # 20066704 <xISRStackTop>

2005bf46 <nonvec_handle>:
2005bf46:	4285                	c.li	t0,1
2005bf48:	02fe                	c.slli	t0,0x1f
2005bf4a:	00728313          	addi	t1,t0,7
2005bf4e:	00651763          	bne	a0,t1,2005bf5c <test_if_external_interrupt>
2005bf52:	28d9                	c.jal	2005c028 <machine_timer_update>
2005bf54:	73e010ef          	jal	ra,2005d692 <xTaskIncrementTick>
2005bf58:	c909                	c.beqz	a0,2005bf6a <processed_source>
2005bf5a:	a019                	c.j	2005bf60 <task_context_switch>

2005bf5c <test_if_external_interrupt>:
2005bf5c:	2211                	c.jal	2005c060 <vPortHandleInterrupt>
2005bf5e:	a031                	c.j	2005bf6a <processed_source>

2005bf60 <task_context_switch>:
2005bf60:	069010ef          	jal	ra,2005d7c8 <vTaskSwitchContext>
2005bf64:	a019                	c.j	2005bf6a <processed_source>

2005bf66 <as_yet_unhandled>:
2005bf66:	9002                	c.ebreak
2005bf68:	bffd                	c.j	2005bf66 <as_yet_unhandled>

2005bf6a <processed_source>:
2005bf6a:	f601a303          	lw	t1,-160(gp) # 20066804 <pxCurrentTCB>
2005bf6e:	00032103          	lw	sp,0(t1) # 2000 <CRYPTO_Init+0x86a>

2005bf72 <restore_regs>:
2005bf72:	4282                	c.lwsp	t0,0(sp)
2005bf74:	34129073          	csrrw	zero,mepc,t0
2005bf78:	10412283          	lw	t0,260(sp)
2005bf7c:	30029073          	csrrw	zero,mstatus,t0

2005bf80 <_stack_pop>:
2005bf80:	300022f3          	csrrs	t0,mstatus,zero
2005bf84:	6319                	c.lui	t1,0x6
2005bf86:	0062f2b3          	and	t0,t0,t1
2005bf8a:	6309                	c.lui	t1,0x2
2005bf8c:	04628c63          	beq	t0,t1,2005bfe4 <_stack_pop+0x64>
2005bf90:	600a                	c.flwsp	ft0,128(sp)
2005bf92:	609a                	c.flwsp	ft1,132(sp)
2005bf94:	612a                	c.flwsp	ft2,136(sp)
2005bf96:	61ba                	c.flwsp	ft3,140(sp)
2005bf98:	624a                	c.flwsp	ft4,144(sp)
2005bf9a:	62da                	c.flwsp	ft5,148(sp)
2005bf9c:	636a                	c.flwsp	ft6,152(sp)
2005bf9e:	63fa                	c.flwsp	ft7,156(sp)
2005bfa0:	740a                	c.flwsp	fs0,160(sp)
2005bfa2:	749a                	c.flwsp	fs1,164(sp)
2005bfa4:	752a                	c.flwsp	fa0,168(sp)
2005bfa6:	75ba                	c.flwsp	fa1,172(sp)
2005bfa8:	764a                	c.flwsp	fa2,176(sp)
2005bfaa:	76da                	c.flwsp	fa3,180(sp)
2005bfac:	776a                	c.flwsp	fa4,184(sp)
2005bfae:	77fa                	c.flwsp	fa5,188(sp)
2005bfb0:	680e                	c.flwsp	fa6,192(sp)
2005bfb2:	689e                	c.flwsp	fa7,196(sp)
2005bfb4:	692e                	c.flwsp	fs2,200(sp)
2005bfb6:	69be                	c.flwsp	fs3,204(sp)
2005bfb8:	6a4e                	c.flwsp	fs4,208(sp)
2005bfba:	6ade                	c.flwsp	fs5,212(sp)
2005bfbc:	6b6e                	c.flwsp	fs6,216(sp)
2005bfbe:	6bfe                	c.flwsp	fs7,220(sp)
2005bfc0:	7c0e                	c.flwsp	fs8,224(sp)
2005bfc2:	7c9e                	c.flwsp	fs9,228(sp)
2005bfc4:	7d2e                	c.flwsp	fs10,232(sp)
2005bfc6:	7dbe                	c.flwsp	fs11,236(sp)
2005bfc8:	7e4e                	c.flwsp	ft8,240(sp)
2005bfca:	7ede                	c.flwsp	ft9,244(sp)
2005bfcc:	7f6e                	c.flwsp	ft10,248(sp)
2005bfce:	7ffe                	c.flwsp	ft11,252(sp)
2005bfd0:	10012283          	lw	t0,256(sp)
2005bfd4:	00329073          	csrrw	zero,fcsr,t0
2005bfd8:	6319                	c.lui	t1,0x6
2005bfda:	30033073          	csrrc	zero,mstatus,t1
2005bfde:	6311                	c.lui	t1,0x4
2005bfe0:	30032073          	csrrs	zero,mstatus,t1
2005bfe4:	4092                	c.lwsp	ra,4(sp)
2005bfe6:	42d2                	c.lwsp	t0,20(sp)
2005bfe8:	4362                	c.lwsp	t1,24(sp)
2005bfea:	43f2                	c.lwsp	t2,28(sp)
2005bfec:	5402                	c.lwsp	s0,32(sp)
2005bfee:	5492                	c.lwsp	s1,36(sp)
2005bff0:	5522                	c.lwsp	a0,40(sp)
2005bff2:	55b2                	c.lwsp	a1,44(sp)
2005bff4:	5642                	c.lwsp	a2,48(sp)
2005bff6:	56d2                	c.lwsp	a3,52(sp)
2005bff8:	5762                	c.lwsp	a4,56(sp)
2005bffa:	57f2                	c.lwsp	a5,60(sp)
2005bffc:	4806                	c.lwsp	a6,64(sp)
2005bffe:	4896                	c.lwsp	a7,68(sp)
2005c000:	4926                	c.lwsp	s2,72(sp)
2005c002:	49b6                	c.lwsp	s3,76(sp)
2005c004:	4a46                	c.lwsp	s4,80(sp)
2005c006:	4ad6                	c.lwsp	s5,84(sp)
2005c008:	4b66                	c.lwsp	s6,88(sp)
2005c00a:	4bf6                	c.lwsp	s7,92(sp)
2005c00c:	5c06                	c.lwsp	s8,96(sp)
2005c00e:	5c96                	c.lwsp	s9,100(sp)
2005c010:	5d26                	c.lwsp	s10,104(sp)
2005c012:	5db6                	c.lwsp	s11,108(sp)
2005c014:	5e46                	c.lwsp	t3,112(sp)
2005c016:	5ed6                	c.lwsp	t4,116(sp)
2005c018:	5f66                	c.lwsp	t5,120(sp)
2005c01a:	5ff6                	c.lwsp	t6,124(sp)
2005c01c:	10810113          	addi	sp,sp,264
2005c020:	30200073          	mret
2005c024:	00000013          	addi	zero,zero,0

2005c028 <machine_timer_update>:
2005c028:	f441a283          	lw	t0,-188(gp) # 200667e8 <pullMachineTimerCompareRegister>
2005c02c:	e8c1a303          	lw	t1,-372(gp) # 20066730 <pullNextTime>
2005c030:	5efd                	c.li	t4,-1
2005c032:	00032383          	lw	t2,0(t1) # 4000 <GPIO_Direction+0x36>
2005c036:	00432e03          	lw	t3,4(t1)
2005c03a:	01d2a023          	sw	t4,0(t0)
2005c03e:	01c2a223          	sw	t3,4(t0)
2005c042:	0072a023          	sw	t2,0(t0)
2005c046:	f541a283          	lw	t0,-172(gp) # 200667f8 <uxTimerIncrementsForOneTick>
2005c04a:	00728eb3          	add	t4,t0,t2
2005c04e:	007ebf33          	sltu	t5,t4,t2
2005c052:	01ee0fb3          	add	t6,t3,t5
2005c056:	01d32023          	sw	t4,0(t1)
2005c05a:	01f32223          	sw	t6,4(t1)
2005c05e:	8082                	c.jr	ra

2005c060 <vPortHandleInterrupt>:
2005c060:	42bd                	c.li	t0,15
2005c062:	fff2c293          	xori	t0,t0,-1
2005c066:	00517133          	and	sp,sp,t0
2005c06a:	1141                	c.addi	sp,-16
2005c06c:	c006                	c.swsp	ra,0(sp)
2005c06e:	34202373          	csrrs	t1,mcause,zero
2005c072:	800003b7          	lui	t2,0x80000
2005c076:	fff3c393          	xori	t2,t2,-1
2005c07a:	007373b3          	and	t2,t1,t2
2005c07e:	00038533          	add	a0,t2,zero
2005c082:	0000a797          	auipc	a5,0xa
2005c086:	30278793          	addi	a5,a5,770 # 20066384 <plic_interrupt_handler>
2005c08a:	9782                	c.jalr	a5

2005c08c <vPortHandleInterrupt_Exit>:
2005c08c:	4082                	c.lwsp	ra,0(sp)
2005c08e:	0141                	c.addi	sp,16
2005c090:	8082                	c.jr	ra
2005c092:	0001                	c.addi	zero,0
2005c094:	00000013          	addi	zero,zero,0

2005c098 <xPortStartFirstTask>:
2005c098:	00000297          	auipc	t0,0x0
2005c09c:	d9828293          	addi	t0,t0,-616 # 2005be30 <freertos_risc_v_trap_handler>
2005c0a0:	30529073          	csrrw	zero,mtvec,t0
2005c0a4:	f601a383          	lw	t2,-160(gp) # 20066804 <pxCurrentTCB>
2005c0a8:	0003a103          	lw	sp,0(t2) # 80000000 <__ctrace_end__+0xb6000000>
2005c0ac:	300022f3          	csrrs	t0,mstatus,zero
2005c0b0:	6319                	c.lui	t1,0x6
2005c0b2:	0062f2b3          	and	t0,t0,t1
2005c0b6:	6309                	c.lui	t1,0x2
2005c0b8:	04628c63          	beq	t0,t1,2005c110 <xPortStartFirstTask+0x78>
2005c0bc:	600a                	c.flwsp	ft0,128(sp)
2005c0be:	609a                	c.flwsp	ft1,132(sp)
2005c0c0:	612a                	c.flwsp	ft2,136(sp)
2005c0c2:	61ba                	c.flwsp	ft3,140(sp)
2005c0c4:	624a                	c.flwsp	ft4,144(sp)
2005c0c6:	62da                	c.flwsp	ft5,148(sp)
2005c0c8:	636a                	c.flwsp	ft6,152(sp)
2005c0ca:	63fa                	c.flwsp	ft7,156(sp)
2005c0cc:	740a                	c.flwsp	fs0,160(sp)
2005c0ce:	749a                	c.flwsp	fs1,164(sp)
2005c0d0:	752a                	c.flwsp	fa0,168(sp)
2005c0d2:	75ba                	c.flwsp	fa1,172(sp)
2005c0d4:	764a                	c.flwsp	fa2,176(sp)
2005c0d6:	76da                	c.flwsp	fa3,180(sp)
2005c0d8:	776a                	c.flwsp	fa4,184(sp)
2005c0da:	77fa                	c.flwsp	fa5,188(sp)
2005c0dc:	680e                	c.flwsp	fa6,192(sp)
2005c0de:	689e                	c.flwsp	fa7,196(sp)
2005c0e0:	692e                	c.flwsp	fs2,200(sp)
2005c0e2:	69be                	c.flwsp	fs3,204(sp)
2005c0e4:	6a4e                	c.flwsp	fs4,208(sp)
2005c0e6:	6ade                	c.flwsp	fs5,212(sp)
2005c0e8:	6b6e                	c.flwsp	fs6,216(sp)
2005c0ea:	6bfe                	c.flwsp	fs7,220(sp)
2005c0ec:	7c0e                	c.flwsp	fs8,224(sp)
2005c0ee:	7c9e                	c.flwsp	fs9,228(sp)
2005c0f0:	7d2e                	c.flwsp	fs10,232(sp)
2005c0f2:	7dbe                	c.flwsp	fs11,236(sp)
2005c0f4:	7e4e                	c.flwsp	ft8,240(sp)
2005c0f6:	7ede                	c.flwsp	ft9,244(sp)
2005c0f8:	7f6e                	c.flwsp	ft10,248(sp)
2005c0fa:	7ffe                	c.flwsp	ft11,252(sp)
2005c0fc:	10012283          	lw	t0,256(sp)
2005c100:	00329073          	csrrw	zero,fcsr,t0
2005c104:	6319                	c.lui	t1,0x6
2005c106:	30033073          	csrrc	zero,mstatus,t1
2005c10a:	6311                	c.lui	t1,0x4
2005c10c:	30032073          	csrrs	zero,mstatus,t1
2005c110:	4092                	c.lwsp	ra,4(sp)
2005c112:	42d2                	c.lwsp	t0,20(sp)
2005c114:	4362                	c.lwsp	t1,24(sp)
2005c116:	43f2                	c.lwsp	t2,28(sp)
2005c118:	5402                	c.lwsp	s0,32(sp)
2005c11a:	5492                	c.lwsp	s1,36(sp)
2005c11c:	5522                	c.lwsp	a0,40(sp)
2005c11e:	55b2                	c.lwsp	a1,44(sp)
2005c120:	5642                	c.lwsp	a2,48(sp)
2005c122:	56d2                	c.lwsp	a3,52(sp)
2005c124:	5762                	c.lwsp	a4,56(sp)
2005c126:	57f2                	c.lwsp	a5,60(sp)
2005c128:	4806                	c.lwsp	a6,64(sp)
2005c12a:	4896                	c.lwsp	a7,68(sp)
2005c12c:	4926                	c.lwsp	s2,72(sp)
2005c12e:	49b6                	c.lwsp	s3,76(sp)
2005c130:	4a46                	c.lwsp	s4,80(sp)
2005c132:	4ad6                	c.lwsp	s5,84(sp)
2005c134:	4b66                	c.lwsp	s6,88(sp)
2005c136:	4bf6                	c.lwsp	s7,92(sp)
2005c138:	5c06                	c.lwsp	s8,96(sp)
2005c13a:	5c96                	c.lwsp	s9,100(sp)
2005c13c:	5d26                	c.lwsp	s10,104(sp)
2005c13e:	5db6                	c.lwsp	s11,108(sp)
2005c140:	5e46                	c.lwsp	t3,112(sp)
2005c142:	5ed6                	c.lwsp	t4,116(sp)
2005c144:	5f66                	c.lwsp	t5,120(sp)
2005c146:	5ff6                	c.lwsp	t6,124(sp)
2005c148:	10412283          	lw	t0,260(sp)
2005c14c:	0082e293          	ori	t0,t0,8
2005c150:	30029073          	csrrw	zero,mstatus,t0
2005c154:	42d2                	c.lwsp	t0,20(sp)
2005c156:	4082                	c.lwsp	ra,0(sp)
2005c158:	10810113          	addi	sp,sp,264
2005c15c:	8082                	c.jr	ra
2005c15e:	0001                	c.addi	zero,0

2005c160 <pxPortInitialiseStack>:
2005c160:	300022f3          	csrrs	t0,mstatus,zero
2005c164:	ff72f293          	andi	t0,t0,-9
2005c168:	18800313          	addi	t1,zero,392
2005c16c:	0312                	c.slli	t1,0x4
2005c16e:	0062e2b3          	or	t0,t0,t1
2005c172:	1571                	c.addi	a0,-4
2005c174:	00552023          	sw	t0,0(a0)
2005c178:	f2450513          	addi	a0,a0,-220
2005c17c:	c110                	c.sw	a2,0(a0)
2005c17e:	fdc50513          	addi	a0,a0,-36
2005c182:	00052023          	sw	zero,0(a0)
2005c186:	4281                	c.li	t0,0

2005c188 <chip_specific_stack_frame>:
2005c188:	00028763          	beq	t0,zero,2005c196 <chip_specific_stack_frame+0xe>
2005c18c:	1571                	c.addi	a0,-4
2005c18e:	00052023          	sw	zero,0(a0)
2005c192:	12fd                	c.addi	t0,-1
2005c194:	bfd5                	c.j	2005c188 <chip_specific_stack_frame>
2005c196:	1571                	c.addi	a0,-4
2005c198:	c10c                	c.sw	a1,0(a0)
2005c19a:	8082                	c.jr	ra

2005c19c <vPortBackupRegs>:
2005c19c:	00152223          	sw	ra,4(a0)
2005c1a0:	00252423          	sw	sp,8(a0)
2005c1a4:	00352623          	sw	gp,12(a0)
2005c1a8:	00452823          	sw	tp,16(a0)
2005c1ac:	00552a23          	sw	t0,20(a0)
2005c1b0:	00652c23          	sw	t1,24(a0)
2005c1b4:	00752e23          	sw	t2,28(a0)
2005c1b8:	d100                	c.sw	s0,32(a0)
2005c1ba:	d144                	c.sw	s1,36(a0)
2005c1bc:	d508                	c.sw	a0,40(a0)
2005c1be:	d54c                	c.sw	a1,44(a0)
2005c1c0:	d910                	c.sw	a2,48(a0)
2005c1c2:	d954                	c.sw	a3,52(a0)
2005c1c4:	dd18                	c.sw	a4,56(a0)
2005c1c6:	dd5c                	c.sw	a5,60(a0)
2005c1c8:	05052023          	sw	a6,64(a0)
2005c1cc:	05152223          	sw	a7,68(a0)
2005c1d0:	05252423          	sw	s2,72(a0)
2005c1d4:	05352623          	sw	s3,76(a0)
2005c1d8:	05452823          	sw	s4,80(a0)
2005c1dc:	05552a23          	sw	s5,84(a0)
2005c1e0:	05652c23          	sw	s6,88(a0)
2005c1e4:	05752e23          	sw	s7,92(a0)
2005c1e8:	07852023          	sw	s8,96(a0)
2005c1ec:	07952223          	sw	s9,100(a0)
2005c1f0:	07a52423          	sw	s10,104(a0)
2005c1f4:	07b52623          	sw	s11,108(a0)
2005c1f8:	07c52823          	sw	t3,112(a0)
2005c1fc:	07d52a23          	sw	t4,116(a0)
2005c200:	07e52c23          	sw	t5,120(a0)
2005c204:	07f52e23          	sw	t6,124(a0)
2005c208:	300022f3          	csrrs	t0,mstatus,zero
2005c20c:	0055a023          	sw	t0,0(a1)
2005c210:	304022f3          	csrrs	t0,mie,zero
2005c214:	0055a223          	sw	t0,4(a1)
2005c218:	305022f3          	csrrs	t0,mtvec,zero
2005c21c:	0055a423          	sw	t0,8(a1)
2005c220:	0015a623          	sw	ra,12(a1)
2005c224:	8082                	c.jr	ra

2005c226 <vPortBackupfloatRegs>:
2005c226:	300022f3          	csrrs	t0,mstatus,zero
2005c22a:	6319                	c.lui	t1,0x6
2005c22c:	0062f2b3          	and	t0,t0,t1
2005c230:	06629e63          	bne	t0,t1,2005c2ac <vPortBackupfloatRegs+0x86>
2005c234:	00052027          	fsw	ft0,0(a0)
2005c238:	00152227          	fsw	ft1,4(a0)
2005c23c:	00252427          	fsw	ft2,8(a0)
2005c240:	00352627          	fsw	ft3,12(a0)
2005c244:	00452827          	fsw	ft4,16(a0)
2005c248:	00552a27          	fsw	ft5,20(a0)
2005c24c:	00652c27          	fsw	ft6,24(a0)
2005c250:	00752e27          	fsw	ft7,28(a0)
2005c254:	f100                	c.fsw	fs0,32(a0)
2005c256:	f144                	c.fsw	fs1,36(a0)
2005c258:	f508                	c.fsw	fa0,40(a0)
2005c25a:	f54c                	c.fsw	fa1,44(a0)
2005c25c:	f910                	c.fsw	fa2,48(a0)
2005c25e:	f954                	c.fsw	fa3,52(a0)
2005c260:	fd18                	c.fsw	fa4,56(a0)
2005c262:	fd5c                	c.fsw	fa5,60(a0)
2005c264:	05052027          	fsw	fa6,64(a0)
2005c268:	05152227          	fsw	fa7,68(a0)
2005c26c:	05252427          	fsw	fs2,72(a0)
2005c270:	05352627          	fsw	fs3,76(a0)
2005c274:	05452827          	fsw	fs4,80(a0)
2005c278:	05552a27          	fsw	fs5,84(a0)
2005c27c:	05652c27          	fsw	fs6,88(a0)
2005c280:	05752e27          	fsw	fs7,92(a0)
2005c284:	07852027          	fsw	fs8,96(a0)
2005c288:	07952227          	fsw	fs9,100(a0)
2005c28c:	07a52427          	fsw	fs10,104(a0)
2005c290:	07b52627          	fsw	fs11,108(a0)
2005c294:	07c52827          	fsw	ft8,112(a0)
2005c298:	07d52a27          	fsw	ft9,116(a0)
2005c29c:	07e52c27          	fsw	ft10,120(a0)
2005c2a0:	07f52e27          	fsw	ft11,124(a0)
2005c2a4:	003022f3          	csrrs	t0,fcsr,zero
2005c2a8:	0055a023          	sw	t0,0(a1)
2005c2ac:	8082                	c.jr	ra

2005c2ae <vPortRestoreRegs>:
2005c2ae:	00852103          	lw	sp,8(a0)
2005c2b2:	00c52183          	lw	gp,12(a0)
2005c2b6:	01052203          	lw	tp,16(a0)
2005c2ba:	01452283          	lw	t0,20(a0)
2005c2be:	01452283          	lw	t0,20(a0)
2005c2c2:	01852303          	lw	t1,24(a0)
2005c2c6:	01c52383          	lw	t2,28(a0)
2005c2ca:	5100                	c.lw	s0,32(a0)
2005c2cc:	5144                	c.lw	s1,36(a0)
2005c2ce:	5508                	c.lw	a0,40(a0)
2005c2d0:	554c                	c.lw	a1,44(a0)
2005c2d2:	5910                	c.lw	a2,48(a0)
2005c2d4:	5954                	c.lw	a3,52(a0)
2005c2d6:	5d18                	c.lw	a4,56(a0)
2005c2d8:	5d5c                	c.lw	a5,60(a0)
2005c2da:	04052803          	lw	a6,64(a0)
2005c2de:	04452883          	lw	a7,68(a0)
2005c2e2:	04852903          	lw	s2,72(a0)
2005c2e6:	04c52983          	lw	s3,76(a0)
2005c2ea:	05052a03          	lw	s4,80(a0)
2005c2ee:	05452a83          	lw	s5,84(a0)
2005c2f2:	05852b03          	lw	s6,88(a0)
2005c2f6:	05c52b83          	lw	s7,92(a0)
2005c2fa:	06052c03          	lw	s8,96(a0)
2005c2fe:	06452c83          	lw	s9,100(a0)
2005c302:	06852d03          	lw	s10,104(a0)
2005c306:	06c52d83          	lw	s11,108(a0)
2005c30a:	07052e03          	lw	t3,112(a0)
2005c30e:	07452e83          	lw	t4,116(a0)
2005c312:	07852f03          	lw	t5,120(a0)
2005c316:	07c52f83          	lw	t6,124(a0)
2005c31a:	0005a083          	lw	ra,0(a1)
2005c31e:	30009073          	csrrw	zero,mstatus,ra
2005c322:	0045a083          	lw	ra,4(a1)
2005c326:	30409073          	csrrw	zero,mie,ra
2005c32a:	0085a083          	lw	ra,8(a1)
2005c32e:	30509073          	csrrw	zero,mtvec,ra
2005c332:	00c5a083          	lw	ra,12(a1)
2005c336:	8082                	c.jr	ra

2005c338 <vPortRestorefloatRegs>:
2005c338:	00062283          	lw	t0,0(a2)
2005c33c:	6319                	c.lui	t1,0x6
2005c33e:	0062f2b3          	and	t0,t0,t1
2005c342:	06629e63          	bne	t0,t1,2005c3be <vPortRestorefloatRegs+0x86>
2005c346:	00052007          	flw	ft0,0(a0)
2005c34a:	00452087          	flw	ft1,4(a0)
2005c34e:	00852107          	flw	ft2,8(a0)
2005c352:	00c52187          	flw	ft3,12(a0)
2005c356:	01052207          	flw	ft4,16(a0)
2005c35a:	01452287          	flw	ft5,20(a0)
2005c35e:	01852307          	flw	ft6,24(a0)
2005c362:	01c52387          	flw	ft7,28(a0)
2005c366:	7100                	c.flw	fs0,32(a0)
2005c368:	7144                	c.flw	fs1,36(a0)
2005c36a:	7508                	c.flw	fa0,40(a0)
2005c36c:	754c                	c.flw	fa1,44(a0)
2005c36e:	7910                	c.flw	fa2,48(a0)
2005c370:	7954                	c.flw	fa3,52(a0)
2005c372:	7d18                	c.flw	fa4,56(a0)
2005c374:	7d5c                	c.flw	fa5,60(a0)
2005c376:	04052807          	flw	fa6,64(a0)
2005c37a:	04452887          	flw	fa7,68(a0)
2005c37e:	04852907          	flw	fs2,72(a0)
2005c382:	04c52987          	flw	fs3,76(a0)
2005c386:	05052a07          	flw	fs4,80(a0)
2005c38a:	05452a87          	flw	fs5,84(a0)
2005c38e:	05852b07          	flw	fs6,88(a0)
2005c392:	05c52b87          	flw	fs7,92(a0)
2005c396:	06052c07          	flw	fs8,96(a0)
2005c39a:	06452c87          	flw	fs9,100(a0)
2005c39e:	06852d07          	flw	fs10,104(a0)
2005c3a2:	06c52d87          	flw	fs11,108(a0)
2005c3a6:	07052e07          	flw	ft8,112(a0)
2005c3aa:	07452e87          	flw	ft9,116(a0)
2005c3ae:	07852f07          	flw	ft10,120(a0)
2005c3b2:	07c52f87          	flw	ft11,124(a0)
2005c3b6:	0005a283          	lw	t0,0(a1)
2005c3ba:	00329073          	csrrw	zero,fcsr,t0
2005c3be:	8082                	c.jr	ra

2005c3c0 <vPortBackupPmp>:
2005c3c0:	3b0022f3          	csrrs	t0,pmpaddr0,zero
2005c3c4:	00552023          	sw	t0,0(a0)
2005c3c8:	3b1022f3          	csrrs	t0,pmpaddr1,zero
2005c3cc:	00552223          	sw	t0,4(a0)
2005c3d0:	3b2022f3          	csrrs	t0,pmpaddr2,zero
2005c3d4:	00552423          	sw	t0,8(a0)
2005c3d8:	3b3022f3          	csrrs	t0,pmpaddr3,zero
2005c3dc:	00552623          	sw	t0,12(a0)
2005c3e0:	3b4022f3          	csrrs	t0,pmpaddr4,zero
2005c3e4:	00552823          	sw	t0,16(a0)
2005c3e8:	3b5022f3          	csrrs	t0,pmpaddr5,zero
2005c3ec:	00552a23          	sw	t0,20(a0)
2005c3f0:	3b6022f3          	csrrs	t0,pmpaddr6,zero
2005c3f4:	00552c23          	sw	t0,24(a0)
2005c3f8:	3b7022f3          	csrrs	t0,pmpaddr7,zero
2005c3fc:	00552e23          	sw	t0,28(a0)
2005c400:	3b8022f3          	csrrs	t0,pmpaddr8,zero
2005c404:	02552023          	sw	t0,32(a0)
2005c408:	3b9022f3          	csrrs	t0,pmpaddr9,zero
2005c40c:	02552223          	sw	t0,36(a0)
2005c410:	3ba022f3          	csrrs	t0,pmpaddr10,zero
2005c414:	02552423          	sw	t0,40(a0)
2005c418:	3bb022f3          	csrrs	t0,pmpaddr11,zero
2005c41c:	02552623          	sw	t0,44(a0)
2005c420:	3bc022f3          	csrrs	t0,pmpaddr12,zero
2005c424:	02552823          	sw	t0,48(a0)
2005c428:	3bd022f3          	csrrs	t0,pmpaddr13,zero
2005c42c:	02552a23          	sw	t0,52(a0)
2005c430:	3be022f3          	csrrs	t0,pmpaddr14,zero
2005c434:	02552c23          	sw	t0,56(a0)
2005c438:	3bf022f3          	csrrs	t0,pmpaddr15,zero
2005c43c:	02552e23          	sw	t0,60(a0)
2005c440:	3a0022f3          	csrrs	t0,pmpcfg0,zero
2005c444:	04552023          	sw	t0,64(a0)
2005c448:	3a2022f3          	csrrs	t0,pmpcfg2,zero
2005c44c:	04552223          	sw	t0,68(a0)
2005c450:	3a1022f3          	csrrs	t0,pmpcfg1,zero
2005c454:	04552423          	sw	t0,72(a0)
2005c458:	3a3022f3          	csrrs	t0,pmpcfg3,zero
2005c45c:	04552623          	sw	t0,76(a0)
2005c460:	8082                	c.jr	ra

2005c462 <vPortRestorePmp>:
2005c462:	00052283          	lw	t0,0(a0)
2005c466:	3b029073          	csrrw	zero,pmpaddr0,t0
2005c46a:	00452283          	lw	t0,4(a0)
2005c46e:	3b129073          	csrrw	zero,pmpaddr1,t0
2005c472:	00852283          	lw	t0,8(a0)
2005c476:	3b229073          	csrrw	zero,pmpaddr2,t0
2005c47a:	00c52283          	lw	t0,12(a0)
2005c47e:	3b329073          	csrrw	zero,pmpaddr3,t0
2005c482:	01052283          	lw	t0,16(a0)
2005c486:	3b429073          	csrrw	zero,pmpaddr4,t0
2005c48a:	01452283          	lw	t0,20(a0)
2005c48e:	3b529073          	csrrw	zero,pmpaddr5,t0
2005c492:	01852283          	lw	t0,24(a0)
2005c496:	3b629073          	csrrw	zero,pmpaddr6,t0
2005c49a:	01c52283          	lw	t0,28(a0)
2005c49e:	3b729073          	csrrw	zero,pmpaddr7,t0
2005c4a2:	02052283          	lw	t0,32(a0)
2005c4a6:	3b829073          	csrrw	zero,pmpaddr8,t0
2005c4aa:	02452283          	lw	t0,36(a0)
2005c4ae:	3b929073          	csrrw	zero,pmpaddr9,t0
2005c4b2:	02852283          	lw	t0,40(a0)
2005c4b6:	3ba29073          	csrrw	zero,pmpaddr10,t0
2005c4ba:	02c52283          	lw	t0,44(a0)
2005c4be:	3bb29073          	csrrw	zero,pmpaddr11,t0
2005c4c2:	03052283          	lw	t0,48(a0)
2005c4c6:	3bc29073          	csrrw	zero,pmpaddr12,t0
2005c4ca:	03452283          	lw	t0,52(a0)
2005c4ce:	3bd29073          	csrrw	zero,pmpaddr13,t0
2005c4d2:	03852283          	lw	t0,56(a0)
2005c4d6:	3be29073          	csrrw	zero,pmpaddr14,t0
2005c4da:	03c52283          	lw	t0,60(a0)
2005c4de:	3bf29073          	csrrw	zero,pmpaddr15,t0
2005c4e2:	04052283          	lw	t0,64(a0)
2005c4e6:	3a029073          	csrrw	zero,pmpcfg0,t0
2005c4ea:	04452283          	lw	t0,68(a0)
2005c4ee:	3a229073          	csrrw	zero,pmpcfg2,t0
2005c4f2:	04852283          	lw	t0,72(a0)
2005c4f6:	3a129073          	csrrw	zero,pmpcfg1,t0
2005c4fa:	04c52283          	lw	t0,76(a0)
2005c4fe:	3a329073          	csrrw	zero,pmpcfg3,t0
2005c502:	8082                	c.jr	ra
2005c504:	0000                	c.unimp
2005c506:	0000                	c.unimp
	...

2005c50a <vPortSetupTimerInterrupt>:
2005c50a:	e6c1a783          	lw	a5,-404(gp) # 20066710 <SystemCoreClock>
2005c50e:	3e800713          	addi	a4,zero,1000
2005c512:	02e7d7b3          	divu	a5,a5,a4
2005c516:	1141                	c.addi	sp,-16
2005c518:	f4f1aa23          	sw	a5,-172(gp) # 200667f8 <uxTimerIncrementsForOneTick>
2005c51c:	f14027f3          	csrrs	a5,mhartid,zero
2005c520:	c63e                	c.swsp	a5,12(sp)
2005c522:	4632                	c.lwsp	a2,12(sp)
2005c524:	10000737          	lui	a4,0x10000
2005c528:	00170793          	addi	a5,a4,1 # 10000001 <__km4_boot_text_start__+0x7fffe1>
2005c52c:	963e                	c.add	a2,a5
2005c52e:	060e                	c.slli	a2,0x3
2005c530:	f4c1a223          	sw	a2,-188(gp) # 200667e8 <pullMachineTimerCompareRegister>
2005c534:	47b2                	c.lwsp	a5,12(sp)
2005c536:	f5418593          	addi	a1,gp,-172 # 200667f8 <uxTimerIncrementsForOneTick>
2005c53a:	97ba                	c.add	a5,a4
2005c53c:	078e                	c.slli	a5,0x3
2005c53e:	f4f1a423          	sw	a5,-184(gp) # 200667ec <pullMachineTimerCounterRegister>
2005c542:	800007b7          	lui	a5,0x80000
2005c546:	43c8                	c.lw	a0,4(a5)
2005c548:	4394                	c.lw	a3,0(a5)
2005c54a:	43d8                	c.lw	a4,4(a5)
2005c54c:	fea71de3          	bne	a4,a0,2005c546 <vPortSetupTimerInterrupt+0x3c>
2005c550:	4188                	c.lw	a0,0(a1)
2005c552:	9536                	c.add	a0,a3
2005c554:	c208                	c.sw	a0,0(a2)
2005c556:	419c                	c.lw	a5,0(a1)
2005c558:	00d536b3          	sltu	a3,a0,a3
2005c55c:	96ba                	c.add	a3,a4
2005c55e:	00a78733          	add	a4,a5,a0
2005c562:	c254                	c.sw	a3,4(a2)
2005c564:	00f737b3          	sltu	a5,a4,a5
2005c568:	f4c18613          	addi	a2,gp,-180 # 200667f0 <ullNextTime>
2005c56c:	96be                	c.add	a3,a5
2005c56e:	c218                	c.sw	a4,0(a2)
2005c570:	c254                	c.sw	a3,4(a2)
2005c572:	0141                	c.addi	sp,16
2005c574:	8082                	c.jr	ra

2005c576 <xPortStartScheduler>:
2005c576:	1141                	c.addi	sp,-16
2005c578:	c606                	c.swsp	ra,12(sp)
2005c57a:	f91ff0ef          	jal	ra,2005c50a <vPortSetupTimerInterrupt>
2005c57e:	6785                	c.lui	a5,0x1
2005c580:	88078793          	addi	a5,a5,-1920 # 880 <__NVIC_GetVectorArg+0xce>
2005c584:	3047a073          	csrrs	zero,mie,a5
2005c588:	b11ff0ef          	jal	ra,2005c098 <xPortStartFirstTask>
2005c58c:	40b2                	c.lwsp	ra,12(sp)
2005c58e:	4501                	c.li	a0,0
2005c590:	0141                	c.addi	sp,16
2005c592:	8082                	c.jr	ra

2005c594 <vApplicationTickHook>:
2005c594:	8082                	c.jr	ra

2005c596 <vApplicationIdleHook>:
2005c596:	1101                	c.addi	sp,-32
2005c598:	ce06                	c.swsp	ra,28(sp)
2005c59a:	d22fe0ef          	jal	ra,2005aabc <xPortGetFreeHeapSize>
2005c59e:	40f2                	c.lwsp	ra,28(sp)
2005c5a0:	c62a                	c.swsp	a0,12(sp)
2005c5a2:	47b2                	c.lwsp	a5,12(sp)
2005c5a4:	6105                	c.addi16sp	sp,32
2005c5a6:	8082                	c.jr	ra

2005c5a8 <vApplicationMallocFailedHook>:
2005c5a8:	1101                	c.addi	sp,-32
2005c5aa:	ce06                	c.swsp	ra,28(sp)
2005c5ac:	406010ef          	jal	ra,2005d9b2 <xTaskGetSchedulerState>
2005c5b0:	4785                	c.li	a5,1
2005c5b2:	02f50863          	beq	a0,a5,2005c5e2 <vApplicationMallocFailedHook+0x3a>
2005c5b6:	4501                	c.li	a0,0
2005c5b8:	0ce010ef          	jal	ra,2005d686 <pcTaskGetName>
2005c5bc:	862a                	c.mv	a2,a0
2005c5be:	c632                	c.swsp	a2,12(sp)
2005c5c0:	cfcfe0ef          	jal	ra,2005aabc <xPortGetFreeHeapSize>
2005c5c4:	4632                	c.lwsp	a2,12(sp)
2005c5c6:	86aa                	c.mv	a3,a0
2005c5c8:	200635b7          	lui	a1,0x20063
2005c5cc:	20063537          	lui	a0,0x20063
2005c5d0:	40458593          	addi	a1,a1,1028 # 20063404 <__FUNCTION__.0+0x1c>
2005c5d4:	40850513          	addi	a0,a0,1032 # 20063408 <__FUNCTION__.0+0x20>
2005c5d8:	549020ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005c5dc:	30047073          	csrrci	zero,mstatus,8
2005c5e0:	a001                	c.j	2005c5e0 <vApplicationMallocFailedHook+0x38>
2005c5e2:	20063637          	lui	a2,0x20063
2005c5e6:	3fc60613          	addi	a2,a2,1020 # 200633fc <__FUNCTION__.0+0x14>
2005c5ea:	bfd1                	c.j	2005c5be <vApplicationMallocFailedHook+0x16>

2005c5ec <vApplicationStackOverflowHook>:
2005c5ec:	862e                	c.mv	a2,a1
2005c5ee:	20063537          	lui	a0,0x20063
2005c5f2:	200635b7          	lui	a1,0x20063
2005c5f6:	1141                	c.addi	sp,-16
2005c5f8:	46c58593          	addi	a1,a1,1132 # 2006346c <__FUNCTION__.5>
2005c5fc:	44450513          	addi	a0,a0,1092 # 20063444 <__FUNCTION__.0+0x5c>
2005c600:	c606                	c.swsp	ra,12(sp)
2005c602:	51f020ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005c606:	a001                	c.j	2005c606 <vApplicationStackOverflowHook+0x1a>

2005c608 <vApplicationGetIdleTaskMemory>:
2005c608:	200687b7          	lui	a5,0x20068
2005c60c:	e6078793          	addi	a5,a5,-416 # 20067e60 <xIdleTaskTCB.4>
2005c610:	c11c                	c.sw	a5,0(a0)
2005c612:	200657b7          	lui	a5,0x20065
2005c616:	ce078793          	addi	a5,a5,-800 # 20064ce0 <uxIdleTaskStack.3>
2005c61a:	c19c                	c.sw	a5,0(a1)
2005c61c:	20000793          	addi	a5,zero,512
2005c620:	c21c                	c.sw	a5,0(a2)
2005c622:	8082                	c.jr	ra

2005c624 <vApplicationGetTimerTaskMemory>:
2005c624:	200687b7          	lui	a5,0x20068
2005c628:	fc478793          	addi	a5,a5,-60 # 20067fc4 <xTimerTaskTCB.2>
2005c62c:	c11c                	c.sw	a5,0(a0)
2005c62e:	200657b7          	lui	a5,0x20065
2005c632:	4e078793          	addi	a5,a5,1248 # 200654e0 <uxTimerTaskStack.1>
2005c636:	c19c                	c.sw	a5,0(a1)
2005c638:	20000793          	addi	a5,zero,512
2005c63c:	c21c                	c.sw	a5,0(a2)
2005c63e:	8082                	c.jr	ra

2005c640 <vPortSuppressTicksAndSleep>:
2005c640:	1141                	c.addi	sp,-16
2005c642:	c422                	c.swsp	s0,8(sp)
2005c644:	4501                	c.li	a0,0
2005c646:	c606                	c.swsp	ra,12(sp)
2005c648:	e601aa23          	sw	zero,-396(gp) # 20066718 <system_can_yield>
2005c64c:	c5efc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c650:	300477f3          	csrrci	a5,mstatus,8
2005c654:	328010ef          	jal	ra,2005d97c <eTaskConfirmSleepModeStatus>
2005c658:	c90d                	c.beqz	a0,2005c68a <vPortSuppressTicksAndSleep+0x4a>
2005c65a:	9a2fd0ef          	jal	ra,200597fc <pmu_ready_to_sleep>
2005c65e:	e515                	c.bnez	a0,2005c68a <vPortSuppressTicksAndSleep+0x4a>
2005c660:	4505                	c.li	a0,1
2005c662:	c48fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c666:	0ff0000f          	fence	iorw,iorw
2005c66a:	10500073          	wfi
2005c66e:	0001                	c.addi	zero,0
2005c670:	0ff0000f          	fence	iorw,iorw
2005c674:	0001                	c.addi	zero,0
2005c676:	0001                	c.addi	zero,0
2005c678:	0001                	c.addi	zero,0
2005c67a:	0001                	c.addi	zero,0
2005c67c:	0001                	c.addi	zero,0
2005c67e:	0001                	c.addi	zero,0
2005c680:	0001                	c.addi	zero,0
2005c682:	0001                	c.addi	zero,0
2005c684:	0001                	c.addi	zero,0
2005c686:	0001                	c.addi	zero,0
2005c688:	0001                	c.addi	zero,0
2005c68a:	300467f3          	csrrsi	a5,mstatus,8
2005c68e:	4505                	c.li	a0,1
2005c690:	c1afc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c694:	4785                	c.li	a5,1
2005c696:	40b2                	c.lwsp	ra,12(sp)
2005c698:	e6f1aa23          	sw	a5,-396(gp) # 20066718 <system_can_yield>
2005c69c:	4422                	c.lwsp	s0,8(sp)
2005c69e:	0141                	c.addi	sp,16
2005c6a0:	8082                	c.jr	ra

2005c6a2 <prvIsQueueEmpty>:
2005c6a2:	1141                	c.addi	sp,-16
2005c6a4:	c422                	c.swsp	s0,8(sp)
2005c6a6:	842a                	c.mv	s0,a0
2005c6a8:	c606                	c.swsp	ra,12(sp)
2005c6aa:	4ea010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005c6ae:	5c00                	c.lw	s0,56(s0)
2005c6b0:	4fe010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c6b4:	40b2                	c.lwsp	ra,12(sp)
2005c6b6:	00143513          	sltiu	a0,s0,1
2005c6ba:	4422                	c.lwsp	s0,8(sp)
2005c6bc:	0141                	c.addi	sp,16
2005c6be:	8082                	c.jr	ra

2005c6c0 <prvCopyDataToQueue>:
2005c6c0:	1141                	c.addi	sp,-16
2005c6c2:	c422                	c.swsp	s0,8(sp)
2005c6c4:	c226                	c.swsp	s1,4(sp)
2005c6c6:	c606                	c.swsp	ra,12(sp)
2005c6c8:	c04a                	c.swsp	s2,0(sp)
2005c6ca:	84b2                	c.mv	s1,a2
2005c6cc:	4130                	c.lw	a2,64(a0)
2005c6ce:	03852903          	lw	s2,56(a0)
2005c6d2:	842a                	c.mv	s0,a0
2005c6d4:	e605                	c.bnez	a2,2005c6fc <prvCopyDataToQueue+0x3c>
2005c6d6:	411c                	c.lw	a5,0(a0)
2005c6d8:	4481                	c.li	s1,0
2005c6da:	e799                	c.bnez	a5,2005c6e8 <prvCopyDataToQueue+0x28>
2005c6dc:	4508                	c.lw	a0,8(a0)
2005c6de:	39c010ef          	jal	ra,2005da7a <xTaskPriorityDisinherit>
2005c6e2:	84aa                	c.mv	s1,a0
2005c6e4:	00042423          	sw	zero,8(s0)
2005c6e8:	0905                	c.addi	s2,1
2005c6ea:	03242c23          	sw	s2,56(s0)
2005c6ee:	40b2                	c.lwsp	ra,12(sp)
2005c6f0:	4422                	c.lwsp	s0,8(sp)
2005c6f2:	4902                	c.lwsp	s2,0(sp)
2005c6f4:	8526                	c.mv	a0,s1
2005c6f6:	4492                	c.lwsp	s1,4(sp)
2005c6f8:	0141                	c.addi	sp,16
2005c6fa:	8082                	c.jr	ra
2005c6fc:	e085                	c.bnez	s1,2005c71c <prvCopyDataToQueue+0x5c>
2005c6fe:	4148                	c.lw	a0,4(a0)
2005c700:	dffad097          	auipc	ra,0xdffad
2005c704:	3d0080e7          	jalr	ra,976(ra) # 9ad0 <__wrap_memcpy>
2005c708:	405c                	c.lw	a5,4(s0)
2005c70a:	4038                	c.lw	a4,64(s0)
2005c70c:	97ba                	c.add	a5,a4
2005c70e:	4418                	c.lw	a4,8(s0)
2005c710:	c05c                	c.sw	a5,4(s0)
2005c712:	fce7ebe3          	bltu	a5,a4,2005c6e8 <prvCopyDataToQueue+0x28>
2005c716:	401c                	c.lw	a5,0(s0)
2005c718:	c05c                	c.sw	a5,4(s0)
2005c71a:	b7f9                	c.j	2005c6e8 <prvCopyDataToQueue+0x28>
2005c71c:	4548                	c.lw	a0,12(a0)
2005c71e:	dffad097          	auipc	ra,0xdffad
2005c722:	3b2080e7          	jalr	ra,946(ra) # 9ad0 <__wrap_memcpy>
2005c726:	4038                	c.lw	a4,64(s0)
2005c728:	445c                	c.lw	a5,12(s0)
2005c72a:	40e006b3          	sub	a3,zero,a4
2005c72e:	8f99                	c.sub	a5,a4
2005c730:	4018                	c.lw	a4,0(s0)
2005c732:	c45c                	c.sw	a5,12(s0)
2005c734:	00e7f563          	bgeu	a5,a4,2005c73e <prvCopyDataToQueue+0x7e>
2005c738:	441c                	c.lw	a5,8(s0)
2005c73a:	97b6                	c.add	a5,a3
2005c73c:	c45c                	c.sw	a5,12(s0)
2005c73e:	4789                	c.li	a5,2
2005c740:	00f49763          	bne	s1,a5,2005c74e <prvCopyDataToQueue+0x8e>
2005c744:	4481                	c.li	s1,0
2005c746:	fa0901e3          	beq	s2,zero,2005c6e8 <prvCopyDataToQueue+0x28>
2005c74a:	197d                	c.addi	s2,-1
2005c74c:	bf71                	c.j	2005c6e8 <prvCopyDataToQueue+0x28>
2005c74e:	4481                	c.li	s1,0
2005c750:	bf61                	c.j	2005c6e8 <prvCopyDataToQueue+0x28>

2005c752 <prvCopyDataFromQueue>:
2005c752:	87aa                	c.mv	a5,a0
2005c754:	43b0                	c.lw	a2,64(a5)
2005c756:	852e                	c.mv	a0,a1
2005c758:	ce11                	c.beqz	a2,2005c774 <prvCopyDataFromQueue+0x22>
2005c75a:	47d8                	c.lw	a4,12(a5)
2005c75c:	4794                	c.lw	a3,8(a5)
2005c75e:	9732                	c.add	a4,a2
2005c760:	c7d8                	c.sw	a4,12(a5)
2005c762:	00d76463          	bltu	a4,a3,2005c76a <prvCopyDataFromQueue+0x18>
2005c766:	4398                	c.lw	a4,0(a5)
2005c768:	c7d8                	c.sw	a4,12(a5)
2005c76a:	47cc                	c.lw	a1,12(a5)
2005c76c:	dffad317          	auipc	t1,0xdffad
2005c770:	36430067          	jalr	zero,868(t1) # 9ad0 <__wrap_memcpy>
2005c774:	8082                	c.jr	ra

2005c776 <prvUnlockQueue>:
2005c776:	1141                	c.addi	sp,-16
2005c778:	c422                	c.swsp	s0,8(sp)
2005c77a:	842a                	c.mv	s0,a0
2005c77c:	c226                	c.swsp	s1,4(sp)
2005c77e:	c04a                	c.swsp	s2,0(sp)
2005c780:	c606                	c.swsp	ra,12(sp)
2005c782:	412010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005c786:	04544483          	lbu	s1,69(s0)
2005c78a:	02440913          	addi	s2,s0,36
2005c78e:	04e2                	c.slli	s1,0x18
2005c790:	84e1                	c.srai	s1,0x18
2005c792:	02904b63          	blt	zero,s1,2005c7c8 <prvUnlockQueue+0x52>
2005c796:	57fd                	c.li	a5,-1
2005c798:	04f402a3          	sb	a5,69(s0)
2005c79c:	412010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c7a0:	3f4010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005c7a4:	04444483          	lbu	s1,68(s0)
2005c7a8:	01040913          	addi	s2,s0,16
2005c7ac:	04e2                	c.slli	s1,0x18
2005c7ae:	84e1                	c.srai	s1,0x18
2005c7b0:	02904863          	blt	zero,s1,2005c7e0 <prvUnlockQueue+0x6a>
2005c7b4:	57fd                	c.li	a5,-1
2005c7b6:	04f40223          	sb	a5,68(s0)
2005c7ba:	4422                	c.lwsp	s0,8(sp)
2005c7bc:	40b2                	c.lwsp	ra,12(sp)
2005c7be:	4492                	c.lwsp	s1,4(sp)
2005c7c0:	4902                	c.lwsp	s2,0(sp)
2005c7c2:	0141                	c.addi	sp,16
2005c7c4:	3ea0106f          	jal	zero,2005dbae <vTaskExitCritical>
2005c7c8:	505c                	c.lw	a5,36(s0)
2005c7ca:	d7f1                	c.beqz	a5,2005c796 <prvUnlockQueue+0x20>
2005c7cc:	854a                	c.mv	a0,s2
2005c7ce:	116010ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005c7d2:	c119                	c.beqz	a0,2005c7d8 <prvUnlockQueue+0x62>
2005c7d4:	1a0010ef          	jal	ra,2005d974 <vTaskMissedYield>
2005c7d8:	14fd                	c.addi	s1,-1
2005c7da:	04e2                	c.slli	s1,0x18
2005c7dc:	84e1                	c.srai	s1,0x18
2005c7de:	bf55                	c.j	2005c792 <prvUnlockQueue+0x1c>
2005c7e0:	481c                	c.lw	a5,16(s0)
2005c7e2:	dbe9                	c.beqz	a5,2005c7b4 <prvUnlockQueue+0x3e>
2005c7e4:	854a                	c.mv	a0,s2
2005c7e6:	0fe010ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005c7ea:	c119                	c.beqz	a0,2005c7f0 <prvUnlockQueue+0x7a>
2005c7ec:	188010ef          	jal	ra,2005d974 <vTaskMissedYield>
2005c7f0:	14fd                	c.addi	s1,-1
2005c7f2:	04e2                	c.slli	s1,0x18
2005c7f4:	84e1                	c.srai	s1,0x18
2005c7f6:	bf6d                	c.j	2005c7b0 <prvUnlockQueue+0x3a>

2005c7f8 <xQueueGenericReset>:
2005c7f8:	1141                	c.addi	sp,-16
2005c7fa:	c422                	c.swsp	s0,8(sp)
2005c7fc:	c226                	c.swsp	s1,4(sp)
2005c7fe:	842a                	c.mv	s0,a0
2005c800:	c606                	c.swsp	ra,12(sp)
2005c802:	84ae                	c.mv	s1,a1
2005c804:	390010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005c808:	4034                	c.lw	a3,64(s0)
2005c80a:	5c5c                	c.lw	a5,60(s0)
2005c80c:	4018                	c.lw	a4,0(s0)
2005c80e:	02042c23          	sw	zero,56(s0)
2005c812:	02f687b3          	mul	a5,a3,a5
2005c816:	c058                	c.sw	a4,4(s0)
2005c818:	00f70633          	add	a2,a4,a5
2005c81c:	8f95                	c.sub	a5,a3
2005c81e:	97ba                	c.add	a5,a4
2005c820:	c45c                	c.sw	a5,12(s0)
2005c822:	57fd                	c.li	a5,-1
2005c824:	04f40223          	sb	a5,68(s0)
2005c828:	c410                	c.sw	a2,8(s0)
2005c82a:	04f402a3          	sb	a5,69(s0)
2005c82e:	e095                	c.bnez	s1,2005c852 <xQueueGenericReset+0x5a>
2005c830:	481c                	c.lw	a5,16(s0)
2005c832:	cb81                	c.beqz	a5,2005c842 <xQueueGenericReset+0x4a>
2005c834:	01040513          	addi	a0,s0,16
2005c838:	0ac010ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005c83c:	c119                	c.beqz	a0,2005c842 <xQueueGenericReset+0x4a>
2005c83e:	00000073          	ecall
2005c842:	36c010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c846:	40b2                	c.lwsp	ra,12(sp)
2005c848:	4422                	c.lwsp	s0,8(sp)
2005c84a:	4492                	c.lwsp	s1,4(sp)
2005c84c:	4505                	c.li	a0,1
2005c84e:	0141                	c.addi	sp,16
2005c850:	8082                	c.jr	ra
2005c852:	01040513          	addi	a0,s0,16
2005c856:	b64fe0ef          	jal	ra,2005abba <vListInitialise>
2005c85a:	02440513          	addi	a0,s0,36
2005c85e:	b5cfe0ef          	jal	ra,2005abba <vListInitialise>
2005c862:	b7c5                	c.j	2005c842 <xQueueGenericReset+0x4a>

2005c864 <xQueueGetMutexHolder>:
2005c864:	1101                	c.addi	sp,-32
2005c866:	cc22                	c.swsp	s0,24(sp)
2005c868:	ce06                	c.swsp	ra,28(sp)
2005c86a:	842a                	c.mv	s0,a0
2005c86c:	328010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005c870:	401c                	c.lw	a5,0(s0)
2005c872:	4501                	c.li	a0,0
2005c874:	e391                	c.bnez	a5,2005c878 <xQueueGetMutexHolder+0x14>
2005c876:	4408                	c.lw	a0,8(s0)
2005c878:	c62a                	c.swsp	a0,12(sp)
2005c87a:	334010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c87e:	40f2                	c.lwsp	ra,28(sp)
2005c880:	4462                	c.lwsp	s0,24(sp)
2005c882:	4532                	c.lwsp	a0,12(sp)
2005c884:	6105                	c.addi16sp	sp,32
2005c886:	8082                	c.jr	ra

2005c888 <xQueueGenericSend>:
2005c888:	7139                	c.addi16sp	sp,-64
2005c88a:	dc22                	c.swsp	s0,56(sp)
2005c88c:	da26                	c.swsp	s1,52(sp)
2005c88e:	d84a                	c.swsp	s2,48(sp)
2005c890:	d64e                	c.swsp	s3,44(sp)
2005c892:	d452                	c.swsp	s4,40(sp)
2005c894:	d256                	c.swsp	s5,36(sp)
2005c896:	d05a                	c.swsp	s6,32(sp)
2005c898:	de06                	c.swsp	ra,60(sp)
2005c89a:	842a                	c.mv	s0,a0
2005c89c:	892e                	c.mv	s2,a1
2005c89e:	c632                	c.swsp	a2,12(sp)
2005c8a0:	84b6                	c.mv	s1,a3
2005c8a2:	4a01                	c.li	s4,0
2005c8a4:	4a89                	c.li	s5,2
2005c8a6:	59fd                	c.li	s3,-1
2005c8a8:	01050b13          	addi	s6,a0,16
2005c8ac:	2e8010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005c8b0:	5c18                	c.lw	a4,56(s0)
2005c8b2:	5c5c                	c.lw	a5,60(s0)
2005c8b4:	00f76463          	bltu	a4,a5,2005c8bc <xQueueGenericSend+0x34>
2005c8b8:	03549d63          	bne	s1,s5,2005c8f2 <xQueueGenericSend+0x6a>
2005c8bc:	8626                	c.mv	a2,s1
2005c8be:	85ca                	c.mv	a1,s2
2005c8c0:	8522                	c.mv	a0,s0
2005c8c2:	dffff0ef          	jal	ra,2005c6c0 <prvCopyDataToQueue>
2005c8c6:	505c                	c.lw	a5,36(s0)
2005c8c8:	c789                	c.beqz	a5,2005c8d2 <xQueueGenericSend+0x4a>
2005c8ca:	02440513          	addi	a0,s0,36
2005c8ce:	016010ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005c8d2:	c119                	c.beqz	a0,2005c8d8 <xQueueGenericSend+0x50>
2005c8d4:	00000073          	ecall
2005c8d8:	2d6010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c8dc:	4505                	c.li	a0,1
2005c8de:	50f2                	c.lwsp	ra,60(sp)
2005c8e0:	5462                	c.lwsp	s0,56(sp)
2005c8e2:	54d2                	c.lwsp	s1,52(sp)
2005c8e4:	5942                	c.lwsp	s2,48(sp)
2005c8e6:	59b2                	c.lwsp	s3,44(sp)
2005c8e8:	5a22                	c.lwsp	s4,40(sp)
2005c8ea:	5a92                	c.lwsp	s5,36(sp)
2005c8ec:	5b02                	c.lwsp	s6,32(sp)
2005c8ee:	6121                	c.addi16sp	sp,64
2005c8f0:	8082                	c.jr	ra
2005c8f2:	47b2                	c.lwsp	a5,12(sp)
2005c8f4:	e789                	c.bnez	a5,2005c8fe <xQueueGenericSend+0x76>
2005c8f6:	2b8010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c8fa:	4501                	c.li	a0,0
2005c8fc:	b7cd                	c.j	2005c8de <xQueueGenericSend+0x56>
2005c8fe:	000a1563          	bne	s4,zero,2005c908 <xQueueGenericSend+0x80>
2005c902:	0828                	c.addi4spn	a0,sp,24
2005c904:	062010ef          	jal	ra,2005d966 <vTaskInternalSetTimeOutState>
2005c908:	2a6010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c90c:	563000ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005c910:	284010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005c914:	04444783          	lbu	a5,68(s0)
2005c918:	07e2                	c.slli	a5,0x18
2005c91a:	87e1                	c.srai	a5,0x18
2005c91c:	01379463          	bne	a5,s3,2005c924 <xQueueGenericSend+0x9c>
2005c920:	04040223          	sb	zero,68(s0)
2005c924:	04544783          	lbu	a5,69(s0)
2005c928:	07e2                	c.slli	a5,0x18
2005c92a:	87e1                	c.srai	a5,0x18
2005c92c:	01379463          	bne	a5,s3,2005c934 <xQueueGenericSend+0xac>
2005c930:	040402a3          	sb	zero,69(s0)
2005c934:	27a010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c938:	006c                	c.addi4spn	a1,sp,12
2005c93a:	0828                	c.addi4spn	a0,sp,24
2005c93c:	18b010ef          	jal	ra,2005e2c6 <xTaskCheckForTimeOut>
2005c940:	e121                	c.bnez	a0,2005c980 <xQueueGenericSend+0xf8>
2005c942:	252010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005c946:	5c18                	c.lw	a4,56(s0)
2005c948:	5c5c                	c.lw	a5,60(s0)
2005c94a:	02f71063          	bne	a4,a5,2005c96a <xQueueGenericSend+0xe2>
2005c94e:	260010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c952:	45b2                	c.lwsp	a1,12(sp)
2005c954:	855a                	c.mv	a0,s6
2005c956:	745000ef          	jal	ra,2005d89a <vTaskPlaceOnEventList>
2005c95a:	8522                	c.mv	a0,s0
2005c95c:	e1bff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005c960:	5bc010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005c964:	c919                	c.beqz	a0,2005c97a <xQueueGenericSend+0xf2>
2005c966:	4a05                	c.li	s4,1
2005c968:	b791                	c.j	2005c8ac <xQueueGenericSend+0x24>
2005c96a:	244010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005c96e:	8522                	c.mv	a0,s0
2005c970:	e07ff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005c974:	5a8010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005c978:	b7fd                	c.j	2005c966 <xQueueGenericSend+0xde>
2005c97a:	00000073          	ecall
2005c97e:	b7e5                	c.j	2005c966 <xQueueGenericSend+0xde>
2005c980:	8522                	c.mv	a0,s0
2005c982:	df5ff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005c986:	596010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005c98a:	bf85                	c.j	2005c8fa <xQueueGenericSend+0x72>

2005c98c <xQueueGenericSendFromISR>:
2005c98c:	1141                	c.addi	sp,-16
2005c98e:	c422                	c.swsp	s0,8(sp)
2005c990:	c04a                	c.swsp	s2,0(sp)
2005c992:	c606                	c.swsp	ra,12(sp)
2005c994:	c226                	c.swsp	s1,4(sp)
2005c996:	5d18                	c.lw	a4,56(a0)
2005c998:	5d5c                	c.lw	a5,60(a0)
2005c99a:	8932                	c.mv	s2,a2
2005c99c:	842a                	c.mv	s0,a0
2005c99e:	8636                	c.mv	a2,a3
2005c9a0:	00f76663          	bltu	a4,a5,2005c9ac <xQueueGenericSendFromISR+0x20>
2005c9a4:	4789                	c.li	a5,2
2005c9a6:	4501                	c.li	a0,0
2005c9a8:	02f69063          	bne	a3,a5,2005c9c8 <xQueueGenericSendFromISR+0x3c>
2005c9ac:	04544483          	lbu	s1,69(s0)
2005c9b0:	5c1c                	c.lw	a5,56(s0)
2005c9b2:	8522                	c.mv	a0,s0
2005c9b4:	04e2                	c.slli	s1,0x18
2005c9b6:	d0bff0ef          	jal	ra,2005c6c0 <prvCopyDataToQueue>
2005c9ba:	84e1                	c.srai	s1,0x18
2005c9bc:	57fd                	c.li	a5,-1
2005c9be:	02f49663          	bne	s1,a5,2005c9ea <xQueueGenericSendFromISR+0x5e>
2005c9c2:	505c                	c.lw	a5,36(s0)
2005c9c4:	eb81                	c.bnez	a5,2005c9d4 <xQueueGenericSendFromISR+0x48>
2005c9c6:	4505                	c.li	a0,1
2005c9c8:	40b2                	c.lwsp	ra,12(sp)
2005c9ca:	4422                	c.lwsp	s0,8(sp)
2005c9cc:	4492                	c.lwsp	s1,4(sp)
2005c9ce:	4902                	c.lwsp	s2,0(sp)
2005c9d0:	0141                	c.addi	sp,16
2005c9d2:	8082                	c.jr	ra
2005c9d4:	02440513          	addi	a0,s0,36
2005c9d8:	70d000ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005c9dc:	d56d                	c.beqz	a0,2005c9c6 <xQueueGenericSendFromISR+0x3a>
2005c9de:	fe0904e3          	beq	s2,zero,2005c9c6 <xQueueGenericSendFromISR+0x3a>
2005c9e2:	4785                	c.li	a5,1
2005c9e4:	00f92023          	sw	a5,0(s2)
2005c9e8:	bff9                	c.j	2005c9c6 <xQueueGenericSendFromISR+0x3a>
2005c9ea:	0485                	c.addi	s1,1
2005c9ec:	04e2                	c.slli	s1,0x18
2005c9ee:	84e1                	c.srai	s1,0x18
2005c9f0:	049402a3          	sb	s1,69(s0)
2005c9f4:	bfc9                	c.j	2005c9c6 <xQueueGenericSendFromISR+0x3a>

2005c9f6 <xQueueGiveFromISR>:
2005c9f6:	5d18                	c.lw	a4,56(a0)
2005c9f8:	5d5c                	c.lw	a5,60(a0)
2005c9fa:	04f77663          	bgeu	a4,a5,2005ca46 <xQueueGiveFromISR+0x50>
2005c9fe:	04554783          	lbu	a5,69(a0)
2005ca02:	0705                	c.addi	a4,1
2005ca04:	dd18                	c.sw	a4,56(a0)
2005ca06:	07e2                	c.slli	a5,0x18
2005ca08:	87e1                	c.srai	a5,0x18
2005ca0a:	577d                	c.li	a4,-1
2005ca0c:	02e79763          	bne	a5,a4,2005ca3a <xQueueGiveFromISR+0x44>
2005ca10:	515c                	c.lw	a5,36(a0)
2005ca12:	e399                	c.bnez	a5,2005ca18 <xQueueGiveFromISR+0x22>
2005ca14:	4505                	c.li	a0,1
2005ca16:	8082                	c.jr	ra
2005ca18:	1141                	c.addi	sp,-16
2005ca1a:	02450513          	addi	a0,a0,36
2005ca1e:	c422                	c.swsp	s0,8(sp)
2005ca20:	c606                	c.swsp	ra,12(sp)
2005ca22:	842e                	c.mv	s0,a1
2005ca24:	6c1000ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005ca28:	c501                	c.beqz	a0,2005ca30 <xQueueGiveFromISR+0x3a>
2005ca2a:	c019                	c.beqz	s0,2005ca30 <xQueueGiveFromISR+0x3a>
2005ca2c:	4785                	c.li	a5,1
2005ca2e:	c01c                	c.sw	a5,0(s0)
2005ca30:	40b2                	c.lwsp	ra,12(sp)
2005ca32:	4422                	c.lwsp	s0,8(sp)
2005ca34:	4505                	c.li	a0,1
2005ca36:	0141                	c.addi	sp,16
2005ca38:	8082                	c.jr	ra
2005ca3a:	0785                	c.addi	a5,1
2005ca3c:	07e2                	c.slli	a5,0x18
2005ca3e:	87e1                	c.srai	a5,0x18
2005ca40:	04f502a3          	sb	a5,69(a0)
2005ca44:	bfc1                	c.j	2005ca14 <xQueueGiveFromISR+0x1e>
2005ca46:	4501                	c.li	a0,0
2005ca48:	8082                	c.jr	ra

2005ca4a <xQueueReceive>:
2005ca4a:	7139                	c.addi16sp	sp,-64
2005ca4c:	dc22                	c.swsp	s0,56(sp)
2005ca4e:	d84a                	c.swsp	s2,48(sp)
2005ca50:	d64e                	c.swsp	s3,44(sp)
2005ca52:	d452                	c.swsp	s4,40(sp)
2005ca54:	d256                	c.swsp	s5,36(sp)
2005ca56:	de06                	c.swsp	ra,60(sp)
2005ca58:	da26                	c.swsp	s1,52(sp)
2005ca5a:	842a                	c.mv	s0,a0
2005ca5c:	892e                	c.mv	s2,a1
2005ca5e:	c632                	c.swsp	a2,12(sp)
2005ca60:	4a01                	c.li	s4,0
2005ca62:	59fd                	c.li	s3,-1
2005ca64:	02450a93          	addi	s5,a0,36
2005ca68:	12c010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005ca6c:	5c04                	c.lw	s1,56(s0)
2005ca6e:	cc85                	c.beqz	s1,2005caa6 <xQueueReceive+0x5c>
2005ca70:	85ca                	c.mv	a1,s2
2005ca72:	8522                	c.mv	a0,s0
2005ca74:	14fd                	c.addi	s1,-1
2005ca76:	cddff0ef          	jal	ra,2005c752 <prvCopyDataFromQueue>
2005ca7a:	dc04                	c.sw	s1,56(s0)
2005ca7c:	481c                	c.lw	a5,16(s0)
2005ca7e:	cb81                	c.beqz	a5,2005ca8e <xQueueReceive+0x44>
2005ca80:	01040513          	addi	a0,s0,16
2005ca84:	661000ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005ca88:	c119                	c.beqz	a0,2005ca8e <xQueueReceive+0x44>
2005ca8a:	00000073          	ecall
2005ca8e:	120010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005ca92:	4505                	c.li	a0,1
2005ca94:	50f2                	c.lwsp	ra,60(sp)
2005ca96:	5462                	c.lwsp	s0,56(sp)
2005ca98:	54d2                	c.lwsp	s1,52(sp)
2005ca9a:	5942                	c.lwsp	s2,48(sp)
2005ca9c:	59b2                	c.lwsp	s3,44(sp)
2005ca9e:	5a22                	c.lwsp	s4,40(sp)
2005caa0:	5a92                	c.lwsp	s5,36(sp)
2005caa2:	6121                	c.addi16sp	sp,64
2005caa4:	8082                	c.jr	ra
2005caa6:	47b2                	c.lwsp	a5,12(sp)
2005caa8:	e789                	c.bnez	a5,2005cab2 <xQueueReceive+0x68>
2005caaa:	104010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005caae:	4501                	c.li	a0,0
2005cab0:	b7d5                	c.j	2005ca94 <xQueueReceive+0x4a>
2005cab2:	000a1563          	bne	s4,zero,2005cabc <xQueueReceive+0x72>
2005cab6:	0828                	c.addi4spn	a0,sp,24
2005cab8:	6af000ef          	jal	ra,2005d966 <vTaskInternalSetTimeOutState>
2005cabc:	0f2010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005cac0:	3af000ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005cac4:	0d0010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005cac8:	04444783          	lbu	a5,68(s0)
2005cacc:	07e2                	c.slli	a5,0x18
2005cace:	87e1                	c.srai	a5,0x18
2005cad0:	01379463          	bne	a5,s3,2005cad8 <xQueueReceive+0x8e>
2005cad4:	04040223          	sb	zero,68(s0)
2005cad8:	04544783          	lbu	a5,69(s0)
2005cadc:	07e2                	c.slli	a5,0x18
2005cade:	87e1                	c.srai	a5,0x18
2005cae0:	01379463          	bne	a5,s3,2005cae8 <xQueueReceive+0x9e>
2005cae4:	040402a3          	sb	zero,69(s0)
2005cae8:	0c6010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005caec:	006c                	c.addi4spn	a1,sp,12
2005caee:	0828                	c.addi4spn	a0,sp,24
2005caf0:	7d6010ef          	jal	ra,2005e2c6 <xTaskCheckForTimeOut>
2005caf4:	e90d                	c.bnez	a0,2005cb26 <xQueueReceive+0xdc>
2005caf6:	8522                	c.mv	a0,s0
2005caf8:	babff0ef          	jal	ra,2005c6a2 <prvIsQueueEmpty>
2005cafc:	cd19                	c.beqz	a0,2005cb1a <xQueueReceive+0xd0>
2005cafe:	45b2                	c.lwsp	a1,12(sp)
2005cb00:	8556                	c.mv	a0,s5
2005cb02:	599000ef          	jal	ra,2005d89a <vTaskPlaceOnEventList>
2005cb06:	8522                	c.mv	a0,s0
2005cb08:	c6fff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005cb0c:	410010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005cb10:	e119                	c.bnez	a0,2005cb16 <xQueueReceive+0xcc>
2005cb12:	00000073          	ecall
2005cb16:	4a05                	c.li	s4,1
2005cb18:	bf81                	c.j	2005ca68 <xQueueReceive+0x1e>
2005cb1a:	8522                	c.mv	a0,s0
2005cb1c:	c5bff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005cb20:	3fc010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005cb24:	bfcd                	c.j	2005cb16 <xQueueReceive+0xcc>
2005cb26:	8522                	c.mv	a0,s0
2005cb28:	c4fff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005cb2c:	3f0010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005cb30:	8522                	c.mv	a0,s0
2005cb32:	b71ff0ef          	jal	ra,2005c6a2 <prvIsQueueEmpty>
2005cb36:	d165                	c.beqz	a0,2005cb16 <xQueueReceive+0xcc>
2005cb38:	bf9d                	c.j	2005caae <xQueueReceive+0x64>

2005cb3a <xQueueSemaphoreTake>:
2005cb3a:	7139                	c.addi16sp	sp,-64
2005cb3c:	dc22                	c.swsp	s0,56(sp)
2005cb3e:	da26                	c.swsp	s1,52(sp)
2005cb40:	d84a                	c.swsp	s2,48(sp)
2005cb42:	d64e                	c.swsp	s3,44(sp)
2005cb44:	d452                	c.swsp	s4,40(sp)
2005cb46:	de06                	c.swsp	ra,60(sp)
2005cb48:	842a                	c.mv	s0,a0
2005cb4a:	c62e                	c.swsp	a1,12(sp)
2005cb4c:	4481                	c.li	s1,0
2005cb4e:	4981                	c.li	s3,0
2005cb50:	597d                	c.li	s2,-1
2005cb52:	02450a13          	addi	s4,a0,36
2005cb56:	03e010ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005cb5a:	5c1c                	c.lw	a5,56(s0)
2005cb5c:	cf8d                	c.beqz	a5,2005cb96 <xQueueSemaphoreTake+0x5c>
2005cb5e:	17fd                	c.addi	a5,-1
2005cb60:	dc1c                	c.sw	a5,56(s0)
2005cb62:	401c                	c.lw	a5,0(s0)
2005cb64:	e781                	c.bnez	a5,2005cb6c <xQueueSemaphoreTake+0x32>
2005cb66:	083010ef          	jal	ra,2005e3e8 <pvTaskIncrementMutexHeldCount>
2005cb6a:	c408                	c.sw	a0,8(s0)
2005cb6c:	481c                	c.lw	a5,16(s0)
2005cb6e:	cb81                	c.beqz	a5,2005cb7e <xQueueSemaphoreTake+0x44>
2005cb70:	01040513          	addi	a0,s0,16
2005cb74:	571000ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005cb78:	c119                	c.beqz	a0,2005cb7e <xQueueSemaphoreTake+0x44>
2005cb7a:	00000073          	ecall
2005cb7e:	030010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005cb82:	4485                	c.li	s1,1
2005cb84:	50f2                	c.lwsp	ra,60(sp)
2005cb86:	5462                	c.lwsp	s0,56(sp)
2005cb88:	5942                	c.lwsp	s2,48(sp)
2005cb8a:	59b2                	c.lwsp	s3,44(sp)
2005cb8c:	5a22                	c.lwsp	s4,40(sp)
2005cb8e:	8526                	c.mv	a0,s1
2005cb90:	54d2                	c.lwsp	s1,52(sp)
2005cb92:	6121                	c.addi16sp	sp,64
2005cb94:	8082                	c.jr	ra
2005cb96:	47b2                	c.lwsp	a5,12(sp)
2005cb98:	e789                	c.bnez	a5,2005cba2 <xQueueSemaphoreTake+0x68>
2005cb9a:	014010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005cb9e:	4481                	c.li	s1,0
2005cba0:	b7d5                	c.j	2005cb84 <xQueueSemaphoreTake+0x4a>
2005cba2:	00099563          	bne	s3,zero,2005cbac <xQueueSemaphoreTake+0x72>
2005cba6:	0828                	c.addi4spn	a0,sp,24
2005cba8:	5bf000ef          	jal	ra,2005d966 <vTaskInternalSetTimeOutState>
2005cbac:	002010ef          	jal	ra,2005dbae <vTaskExitCritical>
2005cbb0:	2bf000ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005cbb4:	7e1000ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005cbb8:	04444783          	lbu	a5,68(s0)
2005cbbc:	07e2                	c.slli	a5,0x18
2005cbbe:	87e1                	c.srai	a5,0x18
2005cbc0:	01279463          	bne	a5,s2,2005cbc8 <xQueueSemaphoreTake+0x8e>
2005cbc4:	04040223          	sb	zero,68(s0)
2005cbc8:	04544783          	lbu	a5,69(s0)
2005cbcc:	07e2                	c.slli	a5,0x18
2005cbce:	87e1                	c.srai	a5,0x18
2005cbd0:	01279463          	bne	a5,s2,2005cbd8 <xQueueSemaphoreTake+0x9e>
2005cbd4:	040402a3          	sb	zero,69(s0)
2005cbd8:	7d7000ef          	jal	ra,2005dbae <vTaskExitCritical>
2005cbdc:	006c                	c.addi4spn	a1,sp,12
2005cbde:	0828                	c.addi4spn	a0,sp,24
2005cbe0:	6e6010ef          	jal	ra,2005e2c6 <xTaskCheckForTimeOut>
2005cbe4:	e139                	c.bnez	a0,2005cc2a <xQueueSemaphoreTake+0xf0>
2005cbe6:	8522                	c.mv	a0,s0
2005cbe8:	abbff0ef          	jal	ra,2005c6a2 <prvIsQueueEmpty>
2005cbec:	c90d                	c.beqz	a0,2005cc1e <xQueueSemaphoreTake+0xe4>
2005cbee:	401c                	c.lw	a5,0(s0)
2005cbf0:	eb89                	c.bnez	a5,2005cc02 <xQueueSemaphoreTake+0xc8>
2005cbf2:	7a3000ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005cbf6:	4408                	c.lw	a0,8(s0)
2005cbf8:	5cf000ef          	jal	ra,2005d9c6 <xTaskPriorityInherit>
2005cbfc:	84aa                	c.mv	s1,a0
2005cbfe:	7b1000ef          	jal	ra,2005dbae <vTaskExitCritical>
2005cc02:	45b2                	c.lwsp	a1,12(sp)
2005cc04:	8552                	c.mv	a0,s4
2005cc06:	495000ef          	jal	ra,2005d89a <vTaskPlaceOnEventList>
2005cc0a:	8522                	c.mv	a0,s0
2005cc0c:	b6bff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005cc10:	30c010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005cc14:	e119                	c.bnez	a0,2005cc1a <xQueueSemaphoreTake+0xe0>
2005cc16:	00000073          	ecall
2005cc1a:	4985                	c.li	s3,1
2005cc1c:	bf2d                	c.j	2005cb56 <xQueueSemaphoreTake+0x1c>
2005cc1e:	8522                	c.mv	a0,s0
2005cc20:	b57ff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005cc24:	2f8010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005cc28:	bfcd                	c.j	2005cc1a <xQueueSemaphoreTake+0xe0>
2005cc2a:	8522                	c.mv	a0,s0
2005cc2c:	b4bff0ef          	jal	ra,2005c776 <prvUnlockQueue>
2005cc30:	2ec010ef          	jal	ra,2005df1c <xTaskResumeAll>
2005cc34:	8522                	c.mv	a0,s0
2005cc36:	a6dff0ef          	jal	ra,2005c6a2 <prvIsQueueEmpty>
2005cc3a:	d165                	c.beqz	a0,2005cc1a <xQueueSemaphoreTake+0xe0>
2005cc3c:	f40484e3          	beq	s1,zero,2005cb84 <xQueueSemaphoreTake+0x4a>
2005cc40:	755000ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005cc44:	504c                	c.lw	a1,36(s0)
2005cc46:	c589                	c.beqz	a1,2005cc50 <xQueueSemaphoreTake+0x116>
2005cc48:	581c                	c.lw	a5,48(s0)
2005cc4a:	45ad                	c.li	a1,11
2005cc4c:	439c                	c.lw	a5,0(a5)
2005cc4e:	8d9d                	c.sub	a1,a5
2005cc50:	4408                	c.lw	a0,8(s0)
2005cc52:	6a5000ef          	jal	ra,2005daf6 <vTaskPriorityDisinheritAfterTimeout>
2005cc56:	b791                	c.j	2005cb9a <xQueueSemaphoreTake+0x60>

2005cc58 <xQueueReceiveFromISR>:
2005cc58:	1101                	c.addi	sp,-32
2005cc5a:	ce06                	c.swsp	ra,28(sp)
2005cc5c:	cc22                	c.swsp	s0,24(sp)
2005cc5e:	ca26                	c.swsp	s1,20(sp)
2005cc60:	c84a                	c.swsp	s2,16(sp)
2005cc62:	c64e                	c.swsp	s3,12(sp)
2005cc64:	03852983          	lw	s3,56(a0)
2005cc68:	04098b63          	beq	s3,zero,2005ccbe <xQueueReceiveFromISR+0x66>
2005cc6c:	04454483          	lbu	s1,68(a0)
2005cc70:	842a                	c.mv	s0,a0
2005cc72:	8932                	c.mv	s2,a2
2005cc74:	04e2                	c.slli	s1,0x18
2005cc76:	addff0ef          	jal	ra,2005c752 <prvCopyDataFromQueue>
2005cc7a:	19fd                	c.addi	s3,-1
2005cc7c:	84e1                	c.srai	s1,0x18
2005cc7e:	03342c23          	sw	s3,56(s0)
2005cc82:	57fd                	c.li	a5,-1
2005cc84:	02f49763          	bne	s1,a5,2005ccb2 <xQueueReceiveFromISR+0x5a>
2005cc88:	481c                	c.lw	a5,16(s0)
2005cc8a:	eb89                	c.bnez	a5,2005cc9c <xQueueReceiveFromISR+0x44>
2005cc8c:	4505                	c.li	a0,1
2005cc8e:	40f2                	c.lwsp	ra,28(sp)
2005cc90:	4462                	c.lwsp	s0,24(sp)
2005cc92:	44d2                	c.lwsp	s1,20(sp)
2005cc94:	4942                	c.lwsp	s2,16(sp)
2005cc96:	49b2                	c.lwsp	s3,12(sp)
2005cc98:	6105                	c.addi16sp	sp,32
2005cc9a:	8082                	c.jr	ra
2005cc9c:	01040513          	addi	a0,s0,16
2005cca0:	445000ef          	jal	ra,2005d8e4 <xTaskRemoveFromEventList>
2005cca4:	d565                	c.beqz	a0,2005cc8c <xQueueReceiveFromISR+0x34>
2005cca6:	fe0903e3          	beq	s2,zero,2005cc8c <xQueueReceiveFromISR+0x34>
2005ccaa:	4785                	c.li	a5,1
2005ccac:	00f92023          	sw	a5,0(s2)
2005ccb0:	bff1                	c.j	2005cc8c <xQueueReceiveFromISR+0x34>
2005ccb2:	0485                	c.addi	s1,1
2005ccb4:	04e2                	c.slli	s1,0x18
2005ccb6:	84e1                	c.srai	s1,0x18
2005ccb8:	04940223          	sb	s1,68(s0)
2005ccbc:	bfc1                	c.j	2005cc8c <xQueueReceiveFromISR+0x34>
2005ccbe:	4501                	c.li	a0,0
2005ccc0:	b7f9                	c.j	2005cc8e <xQueueReceiveFromISR+0x36>

2005ccc2 <vQueueWaitForMessageRestricted>:
2005ccc2:	1141                	c.addi	sp,-16
2005ccc4:	c422                	c.swsp	s0,8(sp)
2005ccc6:	c226                	c.swsp	s1,4(sp)
2005ccc8:	c04a                	c.swsp	s2,0(sp)
2005ccca:	842a                	c.mv	s0,a0
2005cccc:	c606                	c.swsp	ra,12(sp)
2005ccce:	84ae                	c.mv	s1,a1
2005ccd0:	8932                	c.mv	s2,a2
2005ccd2:	6c3000ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005ccd6:	04444783          	lbu	a5,68(s0)
2005ccda:	577d                	c.li	a4,-1
2005ccdc:	07e2                	c.slli	a5,0x18
2005ccde:	87e1                	c.srai	a5,0x18
2005cce0:	00e79463          	bne	a5,a4,2005cce8 <vQueueWaitForMessageRestricted+0x26>
2005cce4:	04040223          	sb	zero,68(s0)
2005cce8:	04544783          	lbu	a5,69(s0)
2005ccec:	577d                	c.li	a4,-1
2005ccee:	07e2                	c.slli	a5,0x18
2005ccf0:	87e1                	c.srai	a5,0x18
2005ccf2:	00e79463          	bne	a5,a4,2005ccfa <vQueueWaitForMessageRestricted+0x38>
2005ccf6:	040402a3          	sb	zero,69(s0)
2005ccfa:	6b5000ef          	jal	ra,2005dbae <vTaskExitCritical>
2005ccfe:	5c1c                	c.lw	a5,56(s0)
2005cd00:	e799                	c.bnez	a5,2005cd0e <vQueueWaitForMessageRestricted+0x4c>
2005cd02:	864a                	c.mv	a2,s2
2005cd04:	85a6                	c.mv	a1,s1
2005cd06:	02440513          	addi	a0,s0,36
2005cd0a:	3b1000ef          	jal	ra,2005d8ba <vTaskPlaceOnEventListRestricted>
2005cd0e:	8522                	c.mv	a0,s0
2005cd10:	4422                	c.lwsp	s0,8(sp)
2005cd12:	40b2                	c.lwsp	ra,12(sp)
2005cd14:	4492                	c.lwsp	s1,4(sp)
2005cd16:	4902                	c.lwsp	s2,0(sp)
2005cd18:	0141                	c.addi	sp,16
2005cd1a:	a5dff06f          	jal	zero,2005c776 <prvUnlockQueue>

2005cd1e <QueueCreateTrace>:
2005cd1e:	f581a603          	lw	a2,-168(gp) # 200667fc <Inited.1>
2005cd22:	f5818693          	addi	a3,gp,-168 # 200667fc <Inited.1>
2005cd26:	200687b7          	lui	a5,0x20068
2005cd2a:	12878713          	addi	a4,a5,296 # 20068128 <QueueTraceList>
2005cd2e:	ee01                	c.bnez	a2,2005cd46 <QueueCreateTrace+0x28>
2005cd30:	12878793          	addi	a5,a5,296
2005cd34:	04070613          	addi	a2,a4,64
2005cd38:	0007a023          	sw	zero,0(a5)
2005cd3c:	0791                	c.addi	a5,4
2005cd3e:	fec79de3          	bne	a5,a2,2005cd38 <QueueCreateTrace+0x1a>
2005cd42:	4785                	c.li	a5,1
2005cd44:	c29c                	c.sw	a5,0(a3)
2005cd46:	86ba                	c.mv	a3,a4
2005cd48:	4781                	c.li	a5,0
2005cd4a:	4641                	c.li	a2,16
2005cd4c:	428c                	c.lw	a1,0(a3)
2005cd4e:	e589                	c.bnez	a1,2005cd58 <QueueCreateTrace+0x3a>
2005cd50:	078a                	c.slli	a5,0x2
2005cd52:	97ba                	c.add	a5,a4
2005cd54:	c388                	c.sw	a0,0(a5)
2005cd56:	8082                	c.jr	ra
2005cd58:	0785                	c.addi	a5,1
2005cd5a:	0691                	c.addi	a3,4
2005cd5c:	fec798e3          	bne	a5,a2,2005cd4c <QueueCreateTrace+0x2e>
2005cd60:	8082                	c.jr	ra

2005cd62 <xQueueGenericCreate>:
2005cd62:	1101                	c.addi	sp,-32
2005cd64:	c84a                	c.swsp	s2,16(sp)
2005cd66:	892a                	c.mv	s2,a0
2005cd68:	02b50533          	mul	a0,a0,a1
2005cd6c:	cc22                	c.swsp	s0,24(sp)
2005cd6e:	ca26                	c.swsp	s1,20(sp)
2005cd70:	c64e                	c.swsp	s3,12(sp)
2005cd72:	ce06                	c.swsp	ra,28(sp)
2005cd74:	84ae                	c.mv	s1,a1
2005cd76:	89b2                	c.mv	s3,a2
2005cd78:	05050513          	addi	a0,a0,80
2005cd7c:	c05fd0ef          	jal	ra,2005a980 <pvPortMalloc>
2005cd80:	842a                	c.mv	s0,a0
2005cd82:	c505                	c.beqz	a0,2005cdaa <xQueueGenericCreate+0x48>
2005cd84:	04050323          	sb	zero,70(a0)
2005cd88:	87aa                	c.mv	a5,a0
2005cd8a:	c099                	c.beqz	s1,2005cd90 <xQueueGenericCreate+0x2e>
2005cd8c:	05050793          	addi	a5,a0,80
2005cd90:	c01c                	c.sw	a5,0(s0)
2005cd92:	03242e23          	sw	s2,60(s0)
2005cd96:	c024                	c.sw	s1,64(s0)
2005cd98:	4585                	c.li	a1,1
2005cd9a:	8522                	c.mv	a0,s0
2005cd9c:	a5dff0ef          	jal	ra,2005c7f8 <xQueueGenericReset>
2005cda0:	8522                	c.mv	a0,s0
2005cda2:	05340623          	sb	s3,76(s0)
2005cda6:	f79ff0ef          	jal	ra,2005cd1e <QueueCreateTrace>
2005cdaa:	40f2                	c.lwsp	ra,28(sp)
2005cdac:	8522                	c.mv	a0,s0
2005cdae:	4462                	c.lwsp	s0,24(sp)
2005cdb0:	44d2                	c.lwsp	s1,20(sp)
2005cdb2:	4942                	c.lwsp	s2,16(sp)
2005cdb4:	49b2                	c.lwsp	s3,12(sp)
2005cdb6:	6105                	c.addi16sp	sp,32
2005cdb8:	8082                	c.jr	ra

2005cdba <xQueueCreateMutex>:
2005cdba:	1141                	c.addi	sp,-16
2005cdbc:	862a                	c.mv	a2,a0
2005cdbe:	4581                	c.li	a1,0
2005cdc0:	4505                	c.li	a0,1
2005cdc2:	c422                	c.swsp	s0,8(sp)
2005cdc4:	c606                	c.swsp	ra,12(sp)
2005cdc6:	f9dff0ef          	jal	ra,2005cd62 <xQueueGenericCreate>
2005cdca:	842a                	c.mv	s0,a0
2005cdcc:	cd01                	c.beqz	a0,2005cde4 <xQueueCreateMutex+0x2a>
2005cdce:	00052423          	sw	zero,8(a0)
2005cdd2:	00052023          	sw	zero,0(a0)
2005cdd6:	00052623          	sw	zero,12(a0)
2005cdda:	4681                	c.li	a3,0
2005cddc:	4601                	c.li	a2,0
2005cdde:	4581                	c.li	a1,0
2005cde0:	aa9ff0ef          	jal	ra,2005c888 <xQueueGenericSend>
2005cde4:	40b2                	c.lwsp	ra,12(sp)
2005cde6:	8522                	c.mv	a0,s0
2005cde8:	4422                	c.lwsp	s0,8(sp)
2005cdea:	0141                	c.addi	sp,16
2005cdec:	8082                	c.jr	ra

2005cdee <xQueueCreateCountingSemaphore>:
2005cdee:	1141                	c.addi	sp,-16
2005cdf0:	c422                	c.swsp	s0,8(sp)
2005cdf2:	4609                	c.li	a2,2
2005cdf4:	842e                	c.mv	s0,a1
2005cdf6:	4581                	c.li	a1,0
2005cdf8:	c606                	c.swsp	ra,12(sp)
2005cdfa:	f69ff0ef          	jal	ra,2005cd62 <xQueueGenericCreate>
2005cdfe:	c111                	c.beqz	a0,2005ce02 <xQueueCreateCountingSemaphore+0x14>
2005ce00:	dd00                	c.sw	s0,56(a0)
2005ce02:	40b2                	c.lwsp	ra,12(sp)
2005ce04:	4422                	c.lwsp	s0,8(sp)
2005ce06:	0141                	c.addi	sp,16
2005ce08:	8082                	c.jr	ra

2005ce0a <xQueueGenericCreateStatic>:
2005ce0a:	1141                	c.addi	sp,-16
2005ce0c:	c422                	c.swsp	s0,8(sp)
2005ce0e:	c606                	c.swsp	ra,12(sp)
2005ce10:	c226                	c.swsp	s1,4(sp)
2005ce12:	8436                	c.mv	s0,a3
2005ce14:	c29d                	c.beqz	a3,2005ce3a <xQueueGenericCreateStatic+0x30>
2005ce16:	4785                	c.li	a5,1
2005ce18:	04f68323          	sb	a5,70(a3) # 8000046 <__ap_sram_heap_size+0x7fc0046>
2005ce1c:	84ba                	c.mv	s1,a4
2005ce1e:	e191                	c.bnez	a1,2005ce22 <xQueueGenericCreateStatic+0x18>
2005ce20:	8636                	c.mv	a2,a3
2005ce22:	dc48                	c.sw	a0,60(s0)
2005ce24:	c02c                	c.sw	a1,64(s0)
2005ce26:	c010                	c.sw	a2,0(s0)
2005ce28:	4585                	c.li	a1,1
2005ce2a:	8522                	c.mv	a0,s0
2005ce2c:	9cdff0ef          	jal	ra,2005c7f8 <xQueueGenericReset>
2005ce30:	8522                	c.mv	a0,s0
2005ce32:	04940623          	sb	s1,76(s0)
2005ce36:	ee9ff0ef          	jal	ra,2005cd1e <QueueCreateTrace>
2005ce3a:	40b2                	c.lwsp	ra,12(sp)
2005ce3c:	8522                	c.mv	a0,s0
2005ce3e:	4422                	c.lwsp	s0,8(sp)
2005ce40:	4492                	c.lwsp	s1,4(sp)
2005ce42:	0141                	c.addi	sp,16
2005ce44:	8082                	c.jr	ra

2005ce46 <xQueueCreateMutexStatic>:
2005ce46:	1141                	c.addi	sp,-16
2005ce48:	872a                	c.mv	a4,a0
2005ce4a:	86ae                	c.mv	a3,a1
2005ce4c:	4601                	c.li	a2,0
2005ce4e:	4581                	c.li	a1,0
2005ce50:	4505                	c.li	a0,1
2005ce52:	c422                	c.swsp	s0,8(sp)
2005ce54:	c606                	c.swsp	ra,12(sp)
2005ce56:	fb5ff0ef          	jal	ra,2005ce0a <xQueueGenericCreateStatic>
2005ce5a:	842a                	c.mv	s0,a0
2005ce5c:	cd01                	c.beqz	a0,2005ce74 <xQueueCreateMutexStatic+0x2e>
2005ce5e:	00052423          	sw	zero,8(a0)
2005ce62:	00052023          	sw	zero,0(a0)
2005ce66:	00052623          	sw	zero,12(a0)
2005ce6a:	4681                	c.li	a3,0
2005ce6c:	4601                	c.li	a2,0
2005ce6e:	4581                	c.li	a1,0
2005ce70:	a19ff0ef          	jal	ra,2005c888 <xQueueGenericSend>
2005ce74:	40b2                	c.lwsp	ra,12(sp)
2005ce76:	8522                	c.mv	a0,s0
2005ce78:	4422                	c.lwsp	s0,8(sp)
2005ce7a:	0141                	c.addi	sp,16
2005ce7c:	8082                	c.jr	ra

2005ce7e <xQueueCreateCountingSemaphoreStatic>:
2005ce7e:	1141                	c.addi	sp,-16
2005ce80:	c422                	c.swsp	s0,8(sp)
2005ce82:	86b2                	c.mv	a3,a2
2005ce84:	842e                	c.mv	s0,a1
2005ce86:	4709                	c.li	a4,2
2005ce88:	4601                	c.li	a2,0
2005ce8a:	4581                	c.li	a1,0
2005ce8c:	c606                	c.swsp	ra,12(sp)
2005ce8e:	f7dff0ef          	jal	ra,2005ce0a <xQueueGenericCreateStatic>
2005ce92:	c111                	c.beqz	a0,2005ce96 <xQueueCreateCountingSemaphoreStatic+0x18>
2005ce94:	dd00                	c.sw	s0,56(a0)
2005ce96:	40b2                	c.lwsp	ra,12(sp)
2005ce98:	4422                	c.lwsp	s0,8(sp)
2005ce9a:	0141                	c.addi	sp,16
2005ce9c:	8082                	c.jr	ra

2005ce9e <QueueDeleteTrace>:
2005ce9e:	20068737          	lui	a4,0x20068
2005cea2:	12870693          	addi	a3,a4,296 # 20068128 <QueueTraceList>
2005cea6:	4781                	c.li	a5,0
2005cea8:	12870713          	addi	a4,a4,296
2005ceac:	4641                	c.li	a2,16
2005ceae:	428c                	c.lw	a1,0(a3)
2005ceb0:	00a59763          	bne	a1,a0,2005cebe <QueueDeleteTrace+0x20>
2005ceb4:	078a                	c.slli	a5,0x2
2005ceb6:	97ba                	c.add	a5,a4
2005ceb8:	0007a023          	sw	zero,0(a5)
2005cebc:	8082                	c.jr	ra
2005cebe:	0785                	c.addi	a5,1
2005cec0:	0691                	c.addi	a3,4
2005cec2:	fec796e3          	bne	a5,a2,2005ceae <QueueDeleteTrace+0x10>
2005cec6:	8082                	c.jr	ra

2005cec8 <vQueueDelete>:
2005cec8:	1141                	c.addi	sp,-16
2005ceca:	c422                	c.swsp	s0,8(sp)
2005cecc:	c606                	c.swsp	ra,12(sp)
2005cece:	842a                	c.mv	s0,a0
2005ced0:	fcfff0ef          	jal	ra,2005ce9e <QueueDeleteTrace>
2005ced4:	04644783          	lbu	a5,70(s0)
2005ced8:	e799                	c.bnez	a5,2005cee6 <vQueueDelete+0x1e>
2005ceda:	8522                	c.mv	a0,s0
2005cedc:	4422                	c.lwsp	s0,8(sp)
2005cede:	40b2                	c.lwsp	ra,12(sp)
2005cee0:	0141                	c.addi	sp,16
2005cee2:	b7dfd06f          	jal	zero,2005aa5e <vPortFree>
2005cee6:	40b2                	c.lwsp	ra,12(sp)
2005cee8:	4422                	c.lwsp	s0,8(sp)
2005ceea:	0141                	c.addi	sp,16
2005ceec:	8082                	c.jr	ra

2005ceee <shell_loguratRx_ipc_int>:
2005ceee:	1141                	c.addi	sp,-16
2005cef0:	458d                	c.li	a1,3
2005cef2:	4541                	c.li	a0,16
2005cef4:	c422                	c.swsp	s0,8(sp)
2005cef6:	c606                	c.swsp	ra,12(sp)
2005cef8:	20001437          	lui	s0,0x20001
2005cefc:	9f4fc0ef          	jal	ra,200590f0 <ipc_get_message>
2005cf00:	87aa                	c.mv	a5,a0
2005cf02:	b4440713          	addi	a4,s0,-1212 # 20000b44 <shell_ctl>
2005cf06:	4708                	c.lw	a0,8(a4)
2005cf08:	43cc                	c.lw	a1,4(a5)
2005cf0a:	7c1027f3          	csrrs	a5,0x7c1,zero
2005cf0e:	1007f793          	andi	a5,a5,256
2005cf12:	b4440413          	addi	s0,s0,-1212
2005cf16:	c3b9                	c.beqz	a5,2005cf5c <shell_loguratRx_ipc_int+0x6e>
2005cf18:	01f5f713          	andi	a4,a1,31
2005cf1c:	c325                	c.beqz	a4,2005cf7c <shell_loguratRx_ipc_int+0x8e>
2005cf1e:	08070693          	addi	a3,a4,128
2005cf22:	fe05f793          	andi	a5,a1,-32
2005cf26:	01f7f713          	andi	a4,a5,31
2005cf2a:	9736                	c.add	a4,a3
2005cf2c:	0ff0000f          	fence	iorw,iorw
2005cf30:	973e                	c.add	a4,a5
2005cf32:	00f7f00b          	cache	dinv,(a5)
2005cf36:	02078793          	addi	a5,a5,32
2005cf3a:	40f706b3          	sub	a3,a4,a5
2005cf3e:	fed04ae3          	blt	zero,a3,2005cf32 <shell_loguratRx_ipc_int+0x44>
2005cf42:	0ff0000f          	fence	iorw,iorw
2005cf46:	0001                	c.addi	zero,0
2005cf48:	0001                	c.addi	zero,0
2005cf4a:	0001                	c.addi	zero,0
2005cf4c:	0001                	c.addi	zero,0
2005cf4e:	0001                	c.addi	zero,0
2005cf50:	0001                	c.addi	zero,0
2005cf52:	0001                	c.addi	zero,0
2005cf54:	0001                	c.addi	zero,0
2005cf56:	0001                	c.addi	zero,0
2005cf58:	0001                	c.addi	zero,0
2005cf5a:	0001                	c.addi	zero,0
2005cf5c:	08000613          	addi	a2,zero,128
2005cf60:	dffac097          	auipc	ra,0xdffac
2005cf64:	6b2080e7          	jalr	ra,1714(ra) # 9612 <_memcpy>
2005cf68:	4785                	c.li	a5,1
2005cf6a:	00f40223          	sb	a5,4(s0)
2005cf6e:	501c                	c.lw	a5,32(s0)
2005cf70:	cb91                	c.beqz	a5,2005cf84 <shell_loguratRx_ipc_int+0x96>
2005cf72:	4c5c                	c.lw	a5,28(s0)
2005cf74:	4422                	c.lwsp	s0,8(sp)
2005cf76:	40b2                	c.lwsp	ra,12(sp)
2005cf78:	0141                	c.addi	sp,16
2005cf7a:	8782                	c.jr	a5
2005cf7c:	87ae                	c.mv	a5,a1
2005cf7e:	08000693          	addi	a3,zero,128
2005cf82:	b755                	c.j	2005cf26 <shell_loguratRx_ipc_int+0x38>
2005cf84:	40b2                	c.lwsp	ra,12(sp)
2005cf86:	4422                	c.lwsp	s0,8(sp)
2005cf88:	0141                	c.addi	sp,16
2005cf8a:	8082                	c.jr	ra

2005cf8c <shell_give_sema>:
2005cf8c:	200017b7          	lui	a5,0x20001
2005cf90:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005cf94:	539c                	c.lw	a5,32(a5)
2005cf96:	c789                	c.beqz	a5,2005cfa0 <shell_give_sema+0x14>
2005cf98:	f5c1a503          	lw	a0,-164(gp) # 20066800 <shell_sema>
2005cf9c:	983fe06f          	jal	zero,2005b91e <rtos_sema_give>
2005cfa0:	8082                	c.jr	ra

2005cfa2 <shell_loguartRx_dispatch>:
2005cfa2:	1141                	c.addi	sp,-16
2005cfa4:	c422                	c.swsp	s0,8(sp)
2005cfa6:	20001437          	lui	s0,0x20001
2005cfaa:	b4440413          	addi	s0,s0,-1212 # 20000b44 <shell_ctl>
2005cfae:	c04a                	c.swsp	s2,0(sp)
2005cfb0:	00842903          	lw	s2,8(s0)
2005cfb4:	200635b7          	lui	a1,0x20063
2005cfb8:	48c58593          	addi	a1,a1,1164 # 2006348c <__FUNCTION__.5+0x20>
2005cfbc:	00190513          	addi	a0,s2,1
2005cfc0:	c606                	c.swsp	ra,12(sp)
2005cfc2:	c226                	c.swsp	s1,4(sp)
2005cfc4:	dffad097          	auipc	ra,0xdffad
2005cfc8:	8d4080e7          	jalr	ra,-1836(ra) # 9898 <_stricmp>
2005cfcc:	ed15                	c.bnez	a0,2005d008 <shell_loguartRx_dispatch+0x66>
2005cfce:	40000513          	addi	a0,zero,1024
2005cfd2:	ed2fe0ef          	jal	ra,2005b6a4 <rtos_mem_malloc>
2005cfd6:	3ff00593          	addi	a1,zero,1023
2005cfda:	84aa                	c.mv	s1,a0
2005cfdc:	2e5010ef          	jal	ra,2005eac0 <ChipInfo_GetLibVersion_ToBuf>
2005cfe0:	20063537          	lui	a0,0x20063
2005cfe4:	85a6                	c.mv	a1,s1
2005cfe6:	86c50513          	addi	a0,a0,-1940 # 2006286c <pmap_func+0x4dc>
2005cfea:	336020ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005cfee:	8526                	c.mv	a0,s1
2005cff0:	edefe0ef          	jal	ra,2005b6ce <rtos_mem_free>
2005cff4:	4601                	c.li	a2,0
2005cff6:	08000593          	addi	a1,zero,128
2005cffa:	854a                	c.mv	a0,s2
2005cffc:	dffac097          	auipc	ra,0xdffac
2005d000:	e86080e7          	jalr	ra,-378(ra) # 8e82 <shell_array_init>
2005d004:	00040223          	sb	zero,4(s0)
2005d008:	40b2                	c.lwsp	ra,12(sp)
2005d00a:	4422                	c.lwsp	s0,8(sp)
2005d00c:	4492                	c.lwsp	s1,4(sp)
2005d00e:	4902                	c.lwsp	s2,0(sp)
2005d010:	0141                	c.addi	sp,16
2005d012:	8082                	c.jr	ra

2005d014 <shell_task_ram>:
2005d014:	715d                	c.addi16sp	sp,-80
2005d016:	c4a2                	c.swsp	s0,72(sp)
2005d018:	20001437          	lui	s0,0x20001
2005d01c:	dc52                	c.swsp	s4,56(sp)
2005d01e:	d85a                	c.swsp	s6,48(sp)
2005d020:	d65e                	c.swsp	s7,44(sp)
2005d022:	d462                	c.swsp	s8,40(sp)
2005d024:	b4440793          	addi	a5,s0,-1212 # 20000b44 <shell_ctl>
2005d028:	c686                	c.swsp	ra,76(sp)
2005d02a:	c2a6                	c.swsp	s1,68(sp)
2005d02c:	c0ca                	c.swsp	s2,64(sp)
2005d02e:	de4e                	c.swsp	s3,60(sp)
2005d030:	da56                	c.swsp	s5,52(sp)
2005d032:	d266                	c.swsp	s9,36(sp)
2005d034:	d06a                	c.swsp	s10,32(sp)
2005d036:	ce6e                	c.swsp	s11,28(sp)
2005d038:	0087aa83          	lw	s5,8(a5)
2005d03c:	4705                	c.li	a4,1
2005d03e:	d398                	c.sw	a4,32(a5)
2005d040:	40000513          	addi	a0,zero,1024
2005d044:	20068b37          	lui	s6,0x20068
2005d048:	00e78323          	sb	a4,6(a5)
2005d04c:	b4440413          	addi	s0,s0,-1212
2005d050:	b57fe0ef          	jal	ra,2005bba6 <rtos_create_secure_context>
2005d054:	001a8a13          	addi	s4,s5,1
2005d058:	8c5a                	c.mv	s8,s6
2005d05a:	f5c1a503          	lw	a0,-164(gp) # 20066800 <shell_sema>
2005d05e:	55fd                	c.li	a1,-1
2005d060:	873fe0ef          	jal	ra,2005b8d2 <rtos_sema_take>
2005d064:	f3fff0ef          	jal	ra,2005cfa2 <shell_loguartRx_dispatch>
2005d068:	00444783          	lbu	a5,4(s0)
2005d06c:	0ff7f793          	andi	a5,a5,255
2005d070:	d7ed                	c.beqz	a5,2005d05a <shell_task_ram+0x46>
2005d072:	85d2                	c.mv	a1,s4
2005d074:	168b0513          	addi	a0,s6,360 # 20068168 <log_buf>
2005d078:	409040ef          	jal	ra,20061c80 <strcpy>
2005d07c:	8552                	c.mv	a0,s4
2005d07e:	dffac097          	auipc	ra,0xdffac
2005d082:	e1c080e7          	jalr	ra,-484(ra) # 8e9a <shell_get_argc>
2005d086:	892a                	c.mv	s2,a0
2005d088:	8552                	c.mv	a0,s4
2005d08a:	dffac097          	auipc	ra,0xdffac
2005d08e:	e4a080e7          	jalr	ra,-438(ra) # 8ed4 <shell_get_argv>
2005d092:	00052d83          	lw	s11,0(a0)
2005d096:	4804                	c.lw	s1,16(s0)
2005d098:	01442d03          	lw	s10,20(s0)
2005d09c:	89aa                	c.mv	s3,a0
2005d09e:	4c81                	c.li	s9,0
2005d0a0:	019d0e63          	beq	s10,s9,2005d0bc <shell_task_ram+0xa8>
2005d0a4:	408c                	c.lw	a1,0(s1)
2005d0a6:	856e                	c.mv	a0,s11
2005d0a8:	c626                	c.swsp	s1,12(sp)
2005d0aa:	dffac097          	auipc	ra,0xdffac
2005d0ae:	7ee080e7          	jalr	ra,2030(ra) # 9898 <_stricmp>
2005d0b2:	47b2                	c.lwsp	a5,12(sp)
2005d0b4:	04c1                	c.addi	s1,16
2005d0b6:	e115                	c.bnez	a0,2005d0da <shell_task_ram+0xc6>
2005d0b8:	479c                	c.lw	a5,8(a5)
2005d0ba:	e395                	c.bnez	a5,2005d0de <shell_task_ram+0xca>
2005d0bc:	168c0513          	addi	a0,s8,360
2005d0c0:	888fe0ef          	jal	ra,2005b148 <log_service>
2005d0c4:	4601                	c.li	a2,0
2005d0c6:	08000593          	addi	a1,zero,128
2005d0ca:	8556                	c.mv	a0,s5
2005d0cc:	dffac097          	auipc	ra,0xdffac
2005d0d0:	db6080e7          	jalr	ra,-586(ra) # 8e82 <shell_array_init>
2005d0d4:	00040223          	sb	zero,4(s0)
2005d0d8:	b749                	c.j	2005d05a <shell_task_ram+0x46>
2005d0da:	0c85                	c.addi	s9,1
2005d0dc:	b7d1                	c.j	2005d0a0 <shell_task_ram+0x8c>
2005d0de:	fff90513          	addi	a0,s2,-1
2005d0e2:	0542                	c.slli	a0,0x10
2005d0e4:	00498593          	addi	a1,s3,4
2005d0e8:	8141                	c.srli	a0,0x10
2005d0ea:	9782                	c.jalr	a5
2005d0ec:	bfe1                	c.j	2005d0c4 <shell_task_ram+0xb0>

2005d0ee <shell_init_ram>:
2005d0ee:	1141                	c.addi	sp,-16
2005d0f0:	c606                	c.swsp	ra,12(sp)
2005d0f2:	e9bfd0ef          	jal	ra,2005af8c <log_service_init>
2005d0f6:	20065737          	lui	a4,0x20065
2005d0fa:	b7c70693          	addi	a3,a4,-1156 # 20064b7c <shell_cmd_table>
2005d0fe:	20065737          	lui	a4,0x20065
2005d102:	200017b7          	lui	a5,0x20001
2005d106:	bbc70713          	addi	a4,a4,-1092 # 20064bbc <ipc_tickless_table>
2005d10a:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005d10e:	8f15                	c.sub	a4,a3
2005d110:	cb94                	c.sw	a3,16(a5)
2005d112:	8311                	c.srli	a4,0x4
2005d114:	cbd8                	c.sw	a4,20(a5)
2005d116:	00078223          	sb	zero,4(a5)
2005d11a:	4705                	c.li	a4,1
2005d11c:	00e782a3          	sb	a4,5(a5)
2005d120:	2005d737          	lui	a4,0x2005d
2005d124:	f8c70713          	addi	a4,a4,-116 # 2005cf8c <shell_give_sema>
2005d128:	f5c18513          	addi	a0,gp,-164 # 20066800 <shell_sema>
2005d12c:	cfd8                	c.sw	a4,28(a5)
2005d12e:	f64fe0ef          	jal	ra,2005b892 <rtos_sema_create_binary>
2005d132:	2005d637          	lui	a2,0x2005d
2005d136:	200635b7          	lui	a1,0x20063
2005d13a:	4795                	c.li	a5,5
2005d13c:	6705                	c.lui	a4,0x1
2005d13e:	4681                	c.li	a3,0
2005d140:	01460613          	addi	a2,a2,20 # 2005d014 <shell_task_ram>
2005d144:	49458593          	addi	a1,a1,1172 # 20063494 <__FUNCTION__.5+0x28>
2005d148:	4501                	c.li	a0,0
2005d14a:	a29fe0ef          	jal	ra,2005bb72 <rtos_task_create>
2005d14e:	c105                	c.beqz	a0,2005d16e <shell_init_ram+0x80>
2005d150:	40b2                	c.lwsp	ra,12(sp)
2005d152:	200636b7          	lui	a3,0x20063
2005d156:	200635b7          	lui	a1,0x20063
2005d15a:	4a468693          	addi	a3,a3,1188 # 200634a4 <__FUNCTION__.5+0x38>
2005d15e:	04500613          	addi	a2,zero,69
2005d162:	4c058593          	addi	a1,a1,1216 # 200634c0 <__FUNCTION__.5+0x54>
2005d166:	4509                	c.li	a0,2
2005d168:	0141                	c.addi	sp,16
2005d16a:	d83fd06f          	jal	zero,2005aeec <rtk_log_write>
2005d16e:	40b2                	c.lwsp	ra,12(sp)
2005d170:	0141                	c.addi	sp,16
2005d172:	8082                	c.jr	ra

2005d174 <sys_clear_ota_signature>:
2005d174:	7155                	c.addi16sp	sp,-208
2005d176:	4601                	c.li	a2,0
2005d178:	102c                	c.addi4spn	a1,sp,40
2005d17a:	4509                	c.li	a0,2
2005d17c:	c786                	c.swsp	ra,204(sp)
2005d17e:	c3a6                	c.swsp	s1,196(sp)
2005d180:	c5a2                	c.swsp	s0,200(sp)
2005d182:	c1ca                	c.swsp	s2,192(sp)
2005d184:	df4e                	c.swsp	s3,188(sp)
2005d186:	dd52                	c.swsp	s4,184(sp)
2005d188:	c082                	c.swsp	zero,64(sp)
2005d18a:	c282                	c.swsp	zero,68(sp)
2005d18c:	979fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d190:	4601                	c.li	a2,0
2005d192:	106c                	c.addi4spn	a1,sp,44
2005d194:	450d                	c.li	a0,3
2005d196:	96ffb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d19a:	4505                	c.li	a0,1
2005d19c:	a26fc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d1a0:	0ff57693          	andi	a3,a0,255
2005d1a4:	0016c493          	xori	s1,a3,1
2005d1a8:	f80007b7          	lui	a5,0xf8000
2005d1ac:	e2e9                	c.bnez	a3,2005d26e <sys_clear_ota_signature+0xfa>
2005d1ae:	5722                	c.lwsp	a4,40(sp)
2005d1b0:	0ff57813          	andi	a6,a0,255
2005d1b4:	97ba                	c.add	a5,a4
2005d1b6:	00281713          	slli	a4,a6,0x2
2005d1ba:	0a070613          	addi	a2,a4,160 # 10a0 <CPU_ClkGet+0x58a>
2005d1be:	080c                	c.addi4spn	a1,sp,16
2005d1c0:	962e                	c.add	a2,a1
2005d1c2:	f8f62423          	sw	a5,-120(a2)
2005d1c6:	4785                	c.li	a5,1
2005d1c8:	f8000437          	lui	s0,0xf8000
2005d1cc:	0af69363          	bne	a3,a5,2005d272 <sys_clear_ota_signature+0xfe>
2005d1d0:	57a2                	c.lwsp	a5,40(sp)
2005d1d2:	943e                	c.add	s0,a5
2005d1d4:	00249793          	slli	a5,s1,0x2
2005d1d8:	0814                	c.addi4spn	a3,sp,16
2005d1da:	0a078793          	addi	a5,a5,160 # f80000a0 <__ctrace_end__+0x2e0000a0>
2005d1de:	97b6                	c.add	a5,a3
2005d1e0:	f887a423          	sw	s0,-120(a5)
2005d1e4:	0a070793          	addi	a5,a4,160
2005d1e8:	00d78733          	add	a4,a5,a3
2005d1ec:	f8872a03          	lw	s4,-120(a4)
2005d1f0:	0485                	c.addi	s1,1
2005d1f2:	200639b7          	lui	s3,0x20063
2005d1f6:	200636b7          	lui	a3,0x20063
2005d1fa:	20063937          	lui	s2,0x20063
2005d1fe:	0805                	c.addi	a6,1
2005d200:	4785                	c.li	a5,1
2005d202:	5a498713          	addi	a4,s3,1444 # 200635a4 <__func__.1>
2005d206:	88d2                	c.mv	a7,s4
2005d208:	4c868693          	addi	a3,a3,1224 # 200634c8 <__FUNCTION__.5+0x5c>
2005d20c:	04100613          	addi	a2,zero,65
2005d210:	51890593          	addi	a1,s2,1304 # 20063518 <__FUNCTION__.5+0xac>
2005d214:	4505                	c.li	a0,1
2005d216:	c222                	c.swsp	s0,4(sp)
2005d218:	c026                	c.swsp	s1,0(sp)
2005d21a:	ce42                	c.swsp	a6,28(sp)
2005d21c:	cd1fd0ef          	jal	ra,2005aeec <rtk_log_write>
2005d220:	1810                	c.addi4spn	a2,sp,48
2005d222:	85a2                	c.mv	a1,s0
2005d224:	00a8                	c.addi4spn	a0,sp,72
2005d226:	de0fd0ef          	jal	ra,2005a806 <flash_read_word>
2005d22a:	1850                	c.addi4spn	a2,sp,52
2005d22c:	00440593          	addi	a1,s0,4 # f8000004 <__ctrace_end__+0x2e000004>
2005d230:	00a8                	c.addi4spn	a0,sp,72
2005d232:	dd4fd0ef          	jal	ra,2005a806 <flash_read_word>
2005d236:	5742                	c.lwsp	a4,48(sp)
2005d238:	353937b7          	lui	a5,0x35393
2005d23c:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d240:	4872                	c.lwsp	a6,28(sp)
2005d242:	02f71a63          	bne	a4,a5,2005d276 <sys_clear_ota_signature+0x102>
2005d246:	5752                	c.lwsp	a4,52(sp)
2005d248:	313137b7          	lui	a5,0x31313
2005d24c:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d250:	02f71363          	bne	a4,a5,2005d276 <sys_clear_ota_signature+0x102>
2005d254:	0090                	c.addi4spn	a2,sp,64
2005d256:	45a1                	c.li	a1,8
2005d258:	8552                	c.mv	a0,s4
2005d25a:	95ffb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d25e:	40be                	c.lwsp	ra,204(sp)
2005d260:	442e                	c.lwsp	s0,200(sp)
2005d262:	449e                	c.lwsp	s1,196(sp)
2005d264:	490e                	c.lwsp	s2,192(sp)
2005d266:	59fa                	c.lwsp	s3,188(sp)
2005d268:	5a6a                	c.lwsp	s4,184(sp)
2005d26a:	6169                	c.addi16sp	sp,208
2005d26c:	8082                	c.jr	ra
2005d26e:	5732                	c.lwsp	a4,44(sp)
2005d270:	b781                	c.j	2005d1b0 <sys_clear_ota_signature+0x3c>
2005d272:	57b2                	c.lwsp	a5,44(sp)
2005d274:	bfb9                	c.j	2005d1d2 <sys_clear_ota_signature+0x5e>
2005d276:	200636b7          	lui	a3,0x20063
2005d27a:	88a6                	c.mv	a7,s1
2005d27c:	4785                	c.li	a5,1
2005d27e:	5a498713          	addi	a4,s3,1444
2005d282:	51c68693          	addi	a3,a3,1308 # 2006351c <__FUNCTION__.5+0xb0>
2005d286:	04500613          	addi	a2,zero,69
2005d28a:	51890593          	addi	a1,s2,1304
2005d28e:	4509                	c.li	a0,2
2005d290:	c5dfd0ef          	jal	ra,2005aeec <rtk_log_write>
2005d294:	b7e9                	c.j	2005d25e <sys_clear_ota_signature+0xea>

2005d296 <sys_recover_ota_signature>:
2005d296:	353937b7          	lui	a5,0x35393
2005d29a:	715d                	c.addi16sp	sp,-80
2005d29c:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d2a0:	d43e                	c.swsp	a5,40(sp)
2005d2a2:	313137b7          	lui	a5,0x31313
2005d2a6:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d2aa:	6505                	c.lui	a0,0x1
2005d2ac:	c2a6                	c.swsp	s1,68(sp)
2005d2ae:	de4e                	c.swsp	s3,60(sp)
2005d2b0:	c686                	c.swsp	ra,76(sp)
2005d2b2:	c4a2                	c.swsp	s0,72(sp)
2005d2b4:	c0ca                	c.swsp	s2,64(sp)
2005d2b6:	d63e                	c.swsp	a5,44(sp)
2005d2b8:	e76fd0ef          	jal	ra,2005a92e <__wrap_malloc>
2005d2bc:	200639b7          	lui	s3,0x20063
2005d2c0:	200634b7          	lui	s1,0x20063
2005d2c4:	e50d                	c.bnez	a0,2005d2ee <sys_recover_ota_signature+0x58>
2005d2c6:	200636b7          	lui	a3,0x20063
2005d2ca:	58898713          	addi	a4,s3,1416 # 20063588 <__func__.0>
2005d2ce:	51848593          	addi	a1,s1,1304 # 20063518 <__FUNCTION__.5+0xac>
2005d2d2:	56c68693          	addi	a3,a3,1388 # 2006356c <__FUNCTION__.5+0x100>
2005d2d6:	04500613          	addi	a2,zero,69
2005d2da:	4509                	c.li	a0,2
2005d2dc:	c11fd0ef          	jal	ra,2005aeec <rtk_log_write>
2005d2e0:	40b6                	c.lwsp	ra,76(sp)
2005d2e2:	4426                	c.lwsp	s0,72(sp)
2005d2e4:	4496                	c.lwsp	s1,68(sp)
2005d2e6:	4906                	c.lwsp	s2,64(sp)
2005d2e8:	59f2                	c.lwsp	s3,60(sp)
2005d2ea:	6161                	c.addi16sp	sp,80
2005d2ec:	8082                	c.jr	ra
2005d2ee:	4601                	c.li	a2,0
2005d2f0:	082c                	c.addi4spn	a1,sp,24
2005d2f2:	842a                	c.mv	s0,a0
2005d2f4:	4509                	c.li	a0,2
2005d2f6:	80ffb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d2fa:	086c                	c.addi4spn	a1,sp,28
2005d2fc:	4601                	c.li	a2,0
2005d2fe:	450d                	c.li	a0,3
2005d300:	805fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d304:	4505                	c.li	a0,1
2005d306:	8bcfc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d30a:	0ff57593          	andi	a1,a0,255
2005d30e:	4785                	c.li	a5,1
2005d310:	0015c693          	xori	a3,a1,1
2005d314:	0af59963          	bne	a1,a5,2005d3c6 <sys_recover_ota_signature+0x130>
2005d318:	4762                	c.lwsp	a4,24(sp)
2005d31a:	f80007b7          	lui	a5,0xf8000
2005d31e:	00269613          	slli	a2,a3,0x2
2005d322:	97ba                	c.add	a5,a4
2005d324:	03060713          	addi	a4,a2,48
2005d328:	970a                	c.add	a4,sp
2005d32a:	fef72823          	sw	a5,-16(a4)
2005d32e:	f80008b7          	lui	a7,0xf8000
2005d332:	edd1                	c.bnez	a1,2005d3ce <sys_recover_ota_signature+0x138>
2005d334:	47e2                	c.lwsp	a5,24(sp)
2005d336:	0ff57813          	andi	a6,a0,255
2005d33a:	98be                	c.add	a7,a5
2005d33c:	00281793          	slli	a5,a6,0x2
2005d340:	03078793          	addi	a5,a5,48 # f8000030 <__ctrace_end__+0x2e000030>
2005d344:	978a                	c.add	a5,sp
2005d346:	ff17a823          	sw	a7,-16(a5)
2005d34a:	03060793          	addi	a5,a2,48
2005d34e:	00278633          	add	a2,a5,sp
2005d352:	ff062903          	lw	s2,-16(a2)
2005d356:	0685                	c.addi	a3,1
2005d358:	c036                	c.swsp	a3,0(sp)
2005d35a:	200636b7          	lui	a3,0x20063
2005d35e:	58898713          	addi	a4,s3,1416
2005d362:	0805                	c.addi	a6,1
2005d364:	4785                	c.li	a5,1
2005d366:	4c868693          	addi	a3,a3,1224 # 200634c8 <__FUNCTION__.5+0x5c>
2005d36a:	51848593          	addi	a1,s1,1304
2005d36e:	04100613          	addi	a2,zero,65
2005d372:	4505                	c.li	a0,1
2005d374:	c24a                	c.swsp	s2,4(sp)
2005d376:	b77fd0ef          	jal	ra,2005aeec <rtk_log_write>
2005d37a:	8622                	c.mv	a2,s0
2005d37c:	6585                	c.lui	a1,0x1
2005d37e:	854a                	c.mv	a0,s2
2005d380:	ff2fb0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005d384:	85ca                	c.mv	a1,s2
2005d386:	4509                	c.li	a0,2
2005d388:	917fb0ef          	jal	ra,20058c9e <FLASH_EraseXIP>
2005d38c:	4621                	c.li	a2,8
2005d38e:	102c                	c.addi4spn	a1,sp,40
2005d390:	8522                	c.mv	a0,s0
2005d392:	dffac097          	auipc	ra,0xdffac
2005d396:	280080e7          	jalr	ra,640(ra) # 9612 <_memcpy>
2005d39a:	4481                	c.li	s1,0
2005d39c:	6985                	c.lui	s3,0x1
2005d39e:	01248533          	add	a0,s1,s2
2005d3a2:	8622                	c.mv	a2,s0
2005d3a4:	10000593          	addi	a1,zero,256
2005d3a8:	10048493          	addi	s1,s1,256
2005d3ac:	80dfb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d3b0:	ff3497e3          	bne	s1,s3,2005d39e <sys_recover_ota_signature+0x108>
2005d3b4:	8522                	c.mv	a0,s0
2005d3b6:	4426                	c.lwsp	s0,72(sp)
2005d3b8:	40b6                	c.lwsp	ra,76(sp)
2005d3ba:	4496                	c.lwsp	s1,68(sp)
2005d3bc:	4906                	c.lwsp	s2,64(sp)
2005d3be:	59f2                	c.lwsp	s3,60(sp)
2005d3c0:	6161                	c.addi16sp	sp,80
2005d3c2:	d70fd06f          	jal	zero,2005a932 <__wrap_free>
2005d3c6:	4772                	c.lwsp	a4,28(sp)
2005d3c8:	f80007b7          	lui	a5,0xf8000
2005d3cc:	bf89                	c.j	2005d31e <sys_recover_ota_signature+0x88>
2005d3ce:	47f2                	c.lwsp	a5,28(sp)
2005d3d0:	b79d                	c.j	2005d336 <sys_recover_ota_signature+0xa0>

2005d3d2 <sys_reset>:
2005d3d2:	837fc06f          	jal	zero,20059c08 <System_Reset>

2005d3d6 <prvAddCurrentTaskToDelayedList>:
2005d3d6:	1101                	c.addi	sp,-32
2005d3d8:	ca26                	c.swsp	s1,20(sp)
2005d3da:	cc22                	c.swsp	s0,24(sp)
2005d3dc:	c84a                	c.swsp	s2,16(sp)
2005d3de:	c64e                	c.swsp	s3,12(sp)
2005d3e0:	ce06                	c.swsp	ra,28(sp)
2005d3e2:	f981a983          	lw	s3,-104(gp) # 2006683c <xTickCount>
2005d3e6:	f6018793          	addi	a5,gp,-160 # 20066804 <pxCurrentTCB>
2005d3ea:	842a                	c.mv	s0,a0
2005d3ec:	4388                	c.lw	a0,0(a5)
2005d3ee:	892e                	c.mv	s2,a1
2005d3f0:	0511                	c.addi	a0,4
2005d3f2:	827fd0ef          	jal	ra,2005ac18 <uxListRemove>
2005d3f6:	f6018793          	addi	a5,gp,-160 # 20066804 <pxCurrentTCB>
2005d3fa:	ed09                	c.bnez	a0,2005d414 <prvAddCurrentTaskToDelayedList+0x3e>
2005d3fc:	4398                	c.lw	a4,0(a5)
2005d3fe:	f8418693          	addi	a3,gp,-124 # 20066828 <uxTopReadyPriority>
2005d402:	574c                	c.lw	a1,44(a4)
2005d404:	4290                	c.lw	a2,0(a3)
2005d406:	4705                	c.li	a4,1
2005d408:	00b71733          	sll	a4,a4,a1
2005d40c:	fff74713          	xori	a4,a4,-1
2005d410:	8f71                	c.and	a4,a2
2005d412:	c298                	c.sw	a4,0(a3)
2005d414:	577d                	c.li	a4,-1
2005d416:	02e41263          	bne	s0,a4,2005d43a <prvAddCurrentTaskToDelayedList+0x64>
2005d41a:	02090063          	beq	s2,zero,2005d43a <prvAddCurrentTaskToDelayedList+0x64>
2005d41e:	438c                	c.lw	a1,0(a5)
2005d420:	4462                	c.lwsp	s0,24(sp)
2005d422:	40f2                	c.lwsp	ra,28(sp)
2005d424:	44d2                	c.lwsp	s1,20(sp)
2005d426:	4942                	c.lwsp	s2,16(sp)
2005d428:	49b2                	c.lwsp	s3,12(sp)
2005d42a:	20068537          	lui	a0,0x20068
2005d42e:	0591                	c.addi	a1,4
2005d430:	30050513          	addi	a0,a0,768 # 20068300 <xSuspendedTaskList>
2005d434:	6105                	c.addi16sp	sp,32
2005d436:	f9efd06f          	jal	zero,2005abd4 <vListInsertEnd>
2005d43a:	4398                	c.lw	a4,0(a5)
2005d43c:	944e                	c.add	s0,s3
2005d43e:	c340                	c.sw	s0,4(a4)
2005d440:	01347e63          	bgeu	s0,s3,2005d45c <prvAddCurrentTaskToDelayedList+0x86>
2005d444:	f681a503          	lw	a0,-152(gp) # 2006680c <pxOverflowDelayedTaskList>
2005d448:	4462                	c.lwsp	s0,24(sp)
2005d44a:	438c                	c.lw	a1,0(a5)
2005d44c:	40f2                	c.lwsp	ra,28(sp)
2005d44e:	44d2                	c.lwsp	s1,20(sp)
2005d450:	4942                	c.lwsp	s2,16(sp)
2005d452:	49b2                	c.lwsp	s3,12(sp)
2005d454:	0591                	c.addi	a1,4
2005d456:	6105                	c.addi16sp	sp,32
2005d458:	f92fd06f          	jal	zero,2005abea <vListInsert>
2005d45c:	f641a503          	lw	a0,-156(gp) # 20066808 <pxDelayedTaskList>
2005d460:	438c                	c.lw	a1,0(a5)
2005d462:	0591                	c.addi	a1,4
2005d464:	f86fd0ef          	jal	ra,2005abea <vListInsert>
2005d468:	f8818793          	addi	a5,gp,-120 # 2006682c <xNextTaskUnblockTime>
2005d46c:	4398                	c.lw	a4,0(a5)
2005d46e:	00e47363          	bgeu	s0,a4,2005d474 <prvAddCurrentTaskToDelayedList+0x9e>
2005d472:	c380                	c.sw	s0,0(a5)
2005d474:	40f2                	c.lwsp	ra,28(sp)
2005d476:	4462                	c.lwsp	s0,24(sp)
2005d478:	44d2                	c.lwsp	s1,20(sp)
2005d47a:	4942                	c.lwsp	s2,16(sp)
2005d47c:	49b2                	c.lwsp	s3,12(sp)
2005d47e:	6105                	c.addi16sp	sp,32
2005d480:	8082                	c.jr	ra

2005d482 <prvWriteNameToBuffer>:
2005d482:	1141                	c.addi	sp,-16
2005d484:	c606                	c.swsp	ra,12(sp)
2005d486:	c422                	c.swsp	s0,8(sp)
2005d488:	842a                	c.mv	s0,a0
2005d48a:	7f6040ef          	jal	ra,20061c80 <strcpy>
2005d48e:	8522                	c.mv	a0,s0
2005d490:	dffac097          	auipc	ra,0xdffac
2005d494:	648080e7          	jalr	ra,1608(ra) # 9ad8 <__wrap_strlen>
2005d498:	87aa                	c.mv	a5,a0
2005d49a:	4759                	c.li	a4,22
2005d49c:	02000693          	addi	a3,zero,32
2005d4a0:	02f77063          	bgeu	a4,a5,2005d4c0 <prvWriteNameToBuffer+0x3e>
2005d4a4:	475d                	c.li	a4,23
2005d4a6:	4781                	c.li	a5,0
2005d4a8:	00a76463          	bltu	a4,a0,2005d4b0 <prvWriteNameToBuffer+0x2e>
2005d4ac:	40a707b3          	sub	a5,a4,a0
2005d4b0:	953e                	c.add	a0,a5
2005d4b2:	9522                	c.add	a0,s0
2005d4b4:	00050023          	sb	zero,0(a0)
2005d4b8:	40b2                	c.lwsp	ra,12(sp)
2005d4ba:	4422                	c.lwsp	s0,8(sp)
2005d4bc:	0141                	c.addi	sp,16
2005d4be:	8082                	c.jr	ra
2005d4c0:	00f40633          	add	a2,s0,a5
2005d4c4:	00d60023          	sb	a3,0(a2)
2005d4c8:	0785                	c.addi	a5,1
2005d4ca:	bfd9                	c.j	2005d4a0 <prvWriteNameToBuffer+0x1e>

2005d4cc <prvGetExpectedIdleTime>:
2005d4cc:	f841a703          	lw	a4,-124(gp) # 20066828 <uxTopReadyPriority>
2005d4d0:	f601a783          	lw	a5,-160(gp) # 20066804 <pxCurrentTCB>
2005d4d4:	57c8                	c.lw	a0,44(a5)
2005d4d6:	e105                	c.bnez	a0,2005d4f6 <prvGetExpectedIdleTime+0x2a>
2005d4d8:	200687b7          	lui	a5,0x20068
2005d4dc:	1e87a683          	lw	a3,488(a5) # 200681e8 <pxReadyTasksLists>
2005d4e0:	4785                	c.li	a5,1
2005d4e2:	00d7eb63          	bltu	a5,a3,2005d4f8 <prvGetExpectedIdleTime+0x2c>
2005d4e6:	00e7e963          	bltu	a5,a4,2005d4f8 <prvGetExpectedIdleTime+0x2c>
2005d4ea:	f881a503          	lw	a0,-120(gp) # 2006682c <xNextTaskUnblockTime>
2005d4ee:	f981a783          	lw	a5,-104(gp) # 2006683c <xTickCount>
2005d4f2:	8d1d                	c.sub	a0,a5
2005d4f4:	8082                	c.jr	ra
2005d4f6:	4501                	c.li	a0,0
2005d4f8:	8082                	c.jr	ra

2005d4fa <prvResetNextTaskUnblockTime>:
2005d4fa:	f6418713          	addi	a4,gp,-156 # 20066808 <pxDelayedTaskList>
2005d4fe:	431c                	c.lw	a5,0(a4)
2005d500:	4394                	c.lw	a3,0(a5)
2005d502:	f8818793          	addi	a5,gp,-120 # 2006682c <xNextTaskUnblockTime>
2005d506:	e681                	c.bnez	a3,2005d50e <prvResetNextTaskUnblockTime+0x14>
2005d508:	577d                	c.li	a4,-1
2005d50a:	c398                	c.sw	a4,0(a5)
2005d50c:	8082                	c.jr	ra
2005d50e:	4318                	c.lw	a4,0(a4)
2005d510:	4758                	c.lw	a4,12(a4)
2005d512:	4318                	c.lw	a4,0(a4)
2005d514:	bfdd                	c.j	2005d50a <prvResetNextTaskUnblockTime+0x10>

2005d516 <prvDeleteTCB>:
2005d516:	1141                	c.addi	sp,-16
2005d518:	c422                	c.swsp	s0,8(sp)
2005d51a:	842a                	c.mv	s0,a0
2005d51c:	06c50513          	addi	a0,a0,108
2005d520:	c606                	c.swsp	ra,12(sp)
2005d522:	674040ef          	jal	ra,20061b96 <_reclaim_reent>
2005d526:	16144783          	lbu	a5,353(s0)
2005d52a:	eb91                	c.bnez	a5,2005d53e <prvDeleteTCB+0x28>
2005d52c:	5808                	c.lw	a0,48(s0)
2005d52e:	d30fd0ef          	jal	ra,2005aa5e <vPortFree>
2005d532:	8522                	c.mv	a0,s0
2005d534:	4422                	c.lwsp	s0,8(sp)
2005d536:	40b2                	c.lwsp	ra,12(sp)
2005d538:	0141                	c.addi	sp,16
2005d53a:	d24fd06f          	jal	zero,2005aa5e <vPortFree>
2005d53e:	4705                	c.li	a4,1
2005d540:	fee789e3          	beq	a5,a4,2005d532 <prvDeleteTCB+0x1c>
2005d544:	40b2                	c.lwsp	ra,12(sp)
2005d546:	4422                	c.lwsp	s0,8(sp)
2005d548:	0141                	c.addi	sp,16
2005d54a:	8082                	c.jr	ra

2005d54c <prvInitialiseNewTask.constprop.0>:
2005d54c:	1101                	c.addi	sp,-32
2005d54e:	c452                	c.swsp	s4,8(sp)
2005d550:	8a2a                	c.mv	s4,a0
2005d552:	03082503          	lw	a0,48(a6)
2005d556:	c64e                	c.swsp	s3,12(sp)
2005d558:	00261993          	slli	s3,a2,0x2
2005d55c:	864e                	c.mv	a2,s3
2005d55e:	ca26                	c.swsp	s1,20(sp)
2005d560:	84ae                	c.mv	s1,a1
2005d562:	0a500593          	addi	a1,zero,165
2005d566:	cc22                	c.swsp	s0,24(sp)
2005d568:	c84a                	c.swsp	s2,16(sp)
2005d56a:	8442                	c.mv	s0,a6
2005d56c:	c256                	c.swsp	s5,4(sp)
2005d56e:	c05a                	c.swsp	s6,0(sp)
2005d570:	ce06                	c.swsp	ra,28(sp)
2005d572:	8ab6                	c.mv	s5,a3
2005d574:	893a                	c.mv	s2,a4
2005d576:	8b3e                	c.mv	s6,a5
2005d578:	dffac097          	auipc	ra,0xdffac
2005d57c:	556080e7          	jalr	ra,1366(ra) # 9ace <__wrap_memset>
2005d580:	ffc98613          	addi	a2,s3,-4 # ffc <CPU_ClkGet+0x4e6>
2005d584:	03042983          	lw	s3,48(s0)
2005d588:	99b2                	c.add	s3,a2
2005d58a:	fe09f993          	andi	s3,s3,-32
2005d58e:	05342623          	sw	s3,76(s0)
2005d592:	c8f9                	c.beqz	s1,2005d668 <prvInitialiseNewTask.constprop.0+0x11c>
2005d594:	85a6                	c.mv	a1,s1
2005d596:	03440793          	addi	a5,s0,52
2005d59a:	01848693          	addi	a3,s1,24
2005d59e:	0005c703          	lbu	a4,0(a1) # 1000 <CPU_ClkGet+0x4ea>
2005d5a2:	00e78023          	sb	a4,0(a5)
2005d5a6:	c709                	c.beqz	a4,2005d5b0 <prvInitialiseNewTask.constprop.0+0x64>
2005d5a8:	0585                	c.addi	a1,1
2005d5aa:	0785                	c.addi	a5,1
2005d5ac:	fed599e3          	bne	a1,a3,2005d59e <prvInitialiseNewTask.constprop.0+0x52>
2005d5b0:	040405a3          	sb	zero,75(s0)
2005d5b4:	47a9                	c.li	a5,10
2005d5b6:	0127f363          	bgeu	a5,s2,2005d5bc <prvInitialiseNewTask.constprop.0+0x70>
2005d5ba:	4929                	c.li	s2,10
2005d5bc:	00440513          	addi	a0,s0,4
2005d5c0:	03242623          	sw	s2,44(s0)
2005d5c4:	05242e23          	sw	s2,92(s0)
2005d5c8:	06042023          	sw	zero,96(s0)
2005d5cc:	e02fd0ef          	jal	ra,2005abce <vListInitialiseItem>
2005d5d0:	01840513          	addi	a0,s0,24
2005d5d4:	dfafd0ef          	jal	ra,2005abce <vListInitialiseItem>
2005d5d8:	472d                	c.li	a4,11
2005d5da:	412704b3          	sub	s1,a4,s2
2005d5de:	4611                	c.li	a2,4
2005d5e0:	4581                	c.li	a1,0
2005d5e2:	c800                	c.sw	s0,16(s0)
2005d5e4:	cc04                	c.sw	s1,24(s0)
2005d5e6:	d040                	c.sw	s0,36(s0)
2005d5e8:	04042823          	sw	zero,80(s0)
2005d5ec:	06042223          	sw	zero,100(s0)
2005d5f0:	06042423          	sw	zero,104(s0)
2005d5f4:	15c40513          	addi	a0,s0,348
2005d5f8:	dffac097          	auipc	ra,0xdffac
2005d5fc:	4d6080e7          	jalr	ra,1238(ra) # 9ace <__wrap_memset>
2005d600:	4605                	c.li	a2,1
2005d602:	4581                	c.li	a1,0
2005d604:	16040513          	addi	a0,s0,352
2005d608:	dffac097          	auipc	ra,0xdffac
2005d60c:	4c6080e7          	jalr	ra,1222(ra) # 9ace <__wrap_memset>
2005d610:	0f000613          	addi	a2,zero,240
2005d614:	4581                	c.li	a1,0
2005d616:	06c40513          	addi	a0,s0,108
2005d61a:	dffac097          	auipc	ra,0xdffac
2005d61e:	4b4080e7          	jalr	ra,1204(ra) # 9ace <__wrap_memset>
2005d622:	200697b7          	lui	a5,0x20069
2005d626:	84c78793          	addi	a5,a5,-1972 # 2006884c <__sf>
2005d62a:	d83c                	c.sw	a5,112(s0)
2005d62c:	200697b7          	lui	a5,0x20069
2005d630:	8b478793          	addi	a5,a5,-1868 # 200688b4 <__sf+0x68>
2005d634:	d87c                	c.sw	a5,116(s0)
2005d636:	200697b7          	lui	a5,0x20069
2005d63a:	91c78793          	addi	a5,a5,-1764 # 2006891c <__sf+0xd0>
2005d63e:	dc3c                	c.sw	a5,120(s0)
2005d640:	8656                	c.mv	a2,s5
2005d642:	85d2                	c.mv	a1,s4
2005d644:	854e                	c.mv	a0,s3
2005d646:	b1bfe0ef          	jal	ra,2005c160 <pxPortInitialiseStack>
2005d64a:	c008                	c.sw	a0,0(s0)
2005d64c:	000b0463          	beq	s6,zero,2005d654 <prvInitialiseNewTask.constprop.0+0x108>
2005d650:	008b2023          	sw	s0,0(s6)
2005d654:	40f2                	c.lwsp	ra,28(sp)
2005d656:	4462                	c.lwsp	s0,24(sp)
2005d658:	44d2                	c.lwsp	s1,20(sp)
2005d65a:	4942                	c.lwsp	s2,16(sp)
2005d65c:	49b2                	c.lwsp	s3,12(sp)
2005d65e:	4a22                	c.lwsp	s4,8(sp)
2005d660:	4a92                	c.lwsp	s5,4(sp)
2005d662:	4b02                	c.lwsp	s6,0(sp)
2005d664:	6105                	c.addi16sp	sp,32
2005d666:	8082                	c.jr	ra
2005d668:	02040a23          	sb	zero,52(s0)
2005d66c:	b7a1                	c.j	2005d5b4 <prvInitialiseNewTask.constprop.0+0x68>

2005d66e <vTaskSuspendAll>:
2005d66e:	f7c18793          	addi	a5,gp,-132 # 20066820 <uxSchedulerSuspended>
2005d672:	4398                	c.lw	a4,0(a5)
2005d674:	0705                	c.addi	a4,1
2005d676:	c398                	c.sw	a4,0(a5)
2005d678:	8082                	c.jr	ra

2005d67a <xTaskGetTickCountFromISR>:
2005d67a:	f981a503          	lw	a0,-104(gp) # 2006683c <xTickCount>
2005d67e:	8082                	c.jr	ra

2005d680 <uxTaskGetNumberOfTasks>:
2005d680:	f741a503          	lw	a0,-140(gp) # 20066818 <uxCurrentNumberOfTasks>
2005d684:	8082                	c.jr	ra

2005d686 <pcTaskGetName>:
2005d686:	e119                	c.bnez	a0,2005d68c <pcTaskGetName+0x6>
2005d688:	f601a503          	lw	a0,-160(gp) # 20066804 <pxCurrentTCB>
2005d68c:	03450513          	addi	a0,a0,52
2005d690:	8082                	c.jr	ra

2005d692 <xTaskIncrementTick>:
2005d692:	f7c1a783          	lw	a5,-132(gp) # 20066820 <uxSchedulerSuspended>
2005d696:	7179                	c.addi16sp	sp,-48
2005d698:	d606                	c.swsp	ra,44(sp)
2005d69a:	d422                	c.swsp	s0,40(sp)
2005d69c:	d226                	c.swsp	s1,36(sp)
2005d69e:	d04a                	c.swsp	s2,32(sp)
2005d6a0:	ce4e                	c.swsp	s3,28(sp)
2005d6a2:	cc52                	c.swsp	s4,24(sp)
2005d6a4:	ca56                	c.swsp	s5,20(sp)
2005d6a6:	c85a                	c.swsp	s6,16(sp)
2005d6a8:	c65e                	c.swsp	s7,12(sp)
2005d6aa:	c462                	c.swsp	s8,8(sp)
2005d6ac:	c266                	c.swsp	s9,4(sp)
2005d6ae:	c06a                	c.swsp	s10,0(sp)
2005d6b0:	10079363          	bne	a5,zero,2005d7b6 <xTaskIncrementTick+0x124>
2005d6b4:	f9818793          	addi	a5,gp,-104 # 2006683c <xTickCount>
2005d6b8:	0007aa03          	lw	s4,0(a5)
2005d6bc:	0a05                	c.addi	s4,1
2005d6be:	0147a023          	sw	s4,0(a5)
2005d6c2:	020a1163          	bne	s4,zero,2005d6e4 <xTaskIncrementTick+0x52>
2005d6c6:	f6418713          	addi	a4,gp,-156 # 20066808 <pxDelayedTaskList>
2005d6ca:	f6818793          	addi	a5,gp,-152 # 2006680c <pxOverflowDelayedTaskList>
2005d6ce:	4314                	c.lw	a3,0(a4)
2005d6d0:	4390                	c.lw	a2,0(a5)
2005d6d2:	c310                	c.sw	a2,0(a4)
2005d6d4:	c394                	c.sw	a3,0(a5)
2005d6d6:	f8c18793          	addi	a5,gp,-116 # 20066830 <xNumOfOverflows>
2005d6da:	4398                	c.lw	a4,0(a5)
2005d6dc:	0705                	c.addi	a4,1
2005d6de:	c398                	c.sw	a4,0(a5)
2005d6e0:	e1bff0ef          	jal	ra,2005d4fa <prvResetNextTaskUnblockTime>
2005d6e4:	f8818793          	addi	a5,gp,-120 # 2006682c <xNextTaskUnblockTime>
2005d6e8:	439c                	c.lw	a5,0(a5)
2005d6ea:	200684b7          	lui	s1,0x20068
2005d6ee:	f8818993          	addi	s3,gp,-120 # 2006682c <xNextTaskUnblockTime>
2005d6f2:	1e848493          	addi	s1,s1,488 # 200681e8 <pxReadyTasksLists>
2005d6f6:	f6018a93          	addi	s5,gp,-160 # 20066804 <pxCurrentTCB>
2005d6fa:	4401                	c.li	s0,0
2005d6fc:	04fa7663          	bgeu	s4,a5,2005d748 <xTaskIncrementTick+0xb6>
2005d700:	000aa783          	lw	a5,0(s5)
2005d704:	4751                	c.li	a4,20
2005d706:	57dc                	c.lw	a5,44(a5)
2005d708:	02e787b3          	mul	a5,a5,a4
2005d70c:	94be                	c.add	s1,a5
2005d70e:	4098                	c.lw	a4,0(s1)
2005d710:	4785                	c.li	a5,1
2005d712:	00e7f363          	bgeu	a5,a4,2005d718 <xTaskIncrementTick+0x86>
2005d716:	4405                	c.li	s0,1
2005d718:	f901a783          	lw	a5,-112(gp) # 20066834 <xPendedTicks>
2005d71c:	e399                	c.bnez	a5,2005d722 <xTaskIncrementTick+0x90>
2005d71e:	e77fe0ef          	jal	ra,2005c594 <vApplicationTickHook>
2005d722:	f9c1a783          	lw	a5,-100(gp) # 20066840 <xYieldPending>
2005d726:	c391                	c.beqz	a5,2005d72a <xTaskIncrementTick+0x98>
2005d728:	4405                	c.li	s0,1
2005d72a:	50b2                	c.lwsp	ra,44(sp)
2005d72c:	8522                	c.mv	a0,s0
2005d72e:	5422                	c.lwsp	s0,40(sp)
2005d730:	5492                	c.lwsp	s1,36(sp)
2005d732:	5902                	c.lwsp	s2,32(sp)
2005d734:	49f2                	c.lwsp	s3,28(sp)
2005d736:	4a62                	c.lwsp	s4,24(sp)
2005d738:	4ad2                	c.lwsp	s5,20(sp)
2005d73a:	4b42                	c.lwsp	s6,16(sp)
2005d73c:	4bb2                	c.lwsp	s7,12(sp)
2005d73e:	4c22                	c.lwsp	s8,8(sp)
2005d740:	4c92                	c.lwsp	s9,4(sp)
2005d742:	4d02                	c.lwsp	s10,0(sp)
2005d744:	6145                	c.addi16sp	sp,48
2005d746:	8082                	c.jr	ra
2005d748:	4c05                	c.li	s8,1
2005d74a:	4cd1                	c.li	s9,20
2005d74c:	f6418793          	addi	a5,gp,-156 # 20066808 <pxDelayedTaskList>
2005d750:	4398                	c.lw	a4,0(a5)
2005d752:	4318                	c.lw	a4,0(a4)
2005d754:	e709                	c.bnez	a4,2005d75e <xTaskIncrementTick+0xcc>
2005d756:	57fd                	c.li	a5,-1
2005d758:	00f9a023          	sw	a5,0(s3)
2005d75c:	b755                	c.j	2005d700 <xTaskIncrementTick+0x6e>
2005d75e:	439c                	c.lw	a5,0(a5)
2005d760:	47dc                	c.lw	a5,12(a5)
2005d762:	00c7a903          	lw	s2,12(a5)
2005d766:	00492783          	lw	a5,4(s2)
2005d76a:	fefa67e3          	bltu	s4,a5,2005d758 <xTaskIncrementTick+0xc6>
2005d76e:	00490d13          	addi	s10,s2,4
2005d772:	856a                	c.mv	a0,s10
2005d774:	ca4fd0ef          	jal	ra,2005ac18 <uxListRemove>
2005d778:	02892783          	lw	a5,40(s2)
2005d77c:	c789                	c.beqz	a5,2005d786 <xTaskIncrementTick+0xf4>
2005d77e:	01890513          	addi	a0,s2,24
2005d782:	c96fd0ef          	jal	ra,2005ac18 <uxListRemove>
2005d786:	02c92503          	lw	a0,44(s2)
2005d78a:	f8418713          	addi	a4,gp,-124 # 20066828 <uxTopReadyPriority>
2005d78e:	4314                	c.lw	a3,0(a4)
2005d790:	00ac17b3          	sll	a5,s8,a0
2005d794:	03950533          	mul	a0,a0,s9
2005d798:	8fd5                	c.or	a5,a3
2005d79a:	85ea                	c.mv	a1,s10
2005d79c:	c31c                	c.sw	a5,0(a4)
2005d79e:	9526                	c.add	a0,s1
2005d7a0:	c34fd0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005d7a4:	000aa783          	lw	a5,0(s5)
2005d7a8:	02c92703          	lw	a4,44(s2)
2005d7ac:	57dc                	c.lw	a5,44(a5)
2005d7ae:	f8f76fe3          	bltu	a4,a5,2005d74c <xTaskIncrementTick+0xba>
2005d7b2:	4405                	c.li	s0,1
2005d7b4:	bf61                	c.j	2005d74c <xTaskIncrementTick+0xba>
2005d7b6:	f9018793          	addi	a5,gp,-112 # 20066834 <xPendedTicks>
2005d7ba:	4398                	c.lw	a4,0(a5)
2005d7bc:	4401                	c.li	s0,0
2005d7be:	0705                	c.addi	a4,1
2005d7c0:	c398                	c.sw	a4,0(a5)
2005d7c2:	dd3fe0ef          	jal	ra,2005c594 <vApplicationTickHook>
2005d7c6:	b795                	c.j	2005d72a <xTaskIncrementTick+0x98>

2005d7c8 <vTaskSwitchContext>:
2005d7c8:	f7c1a703          	lw	a4,-132(gp) # 20066820 <uxSchedulerSuspended>
2005d7cc:	f9c18793          	addi	a5,gp,-100 # 20066840 <xYieldPending>
2005d7d0:	c701                	c.beqz	a4,2005d7d8 <vTaskSwitchContext+0x10>
2005d7d2:	4705                	c.li	a4,1
2005d7d4:	c398                	c.sw	a4,0(a5)
2005d7d6:	8082                	c.jr	ra
2005d7d8:	1141                	c.addi	sp,-16
2005d7da:	c422                	c.swsp	s0,8(sp)
2005d7dc:	c606                	c.swsp	ra,12(sp)
2005d7de:	0007a023          	sw	zero,0(a5)
2005d7e2:	410187b7          	lui	a5,0x41018
2005d7e6:	4bd4                	c.lw	a3,20(a5)
2005d7e8:	f7018713          	addi	a4,gp,-144 # 20066814 <ulTotalRunTime>
2005d7ec:	c314                	c.sw	a3,0(a4)
2005d7ee:	f6c1a583          	lw	a1,-148(gp) # 20066810 <ulTaskSwitchedInTime>
2005d7f2:	4318                	c.lw	a4,0(a4)
2005d7f4:	f7018793          	addi	a5,gp,-144 # 20066814 <ulTotalRunTime>
2005d7f8:	f6c18693          	addi	a3,gp,-148 # 20066810 <ulTaskSwitchedInTime>
2005d7fc:	f6018413          	addi	s0,gp,-160 # 20066804 <pxCurrentTCB>
2005d800:	00e5f863          	bgeu	a1,a4,2005d810 <vTaskSwitchContext+0x48>
2005d804:	4388                	c.lw	a0,0(a5)
2005d806:	4010                	c.lw	a2,0(s0)
2005d808:	5638                	c.lw	a4,104(a2)
2005d80a:	8f0d                	c.sub	a4,a1
2005d80c:	972a                	c.add	a4,a0
2005d80e:	d638                	c.sw	a4,104(a2)
2005d810:	439c                	c.lw	a5,0(a5)
2005d812:	a5a5a737          	lui	a4,0xa5a5a
2005d816:	5a570713          	addi	a4,a4,1445 # a5a5a5a5 <__ctrace_end__+0xdba5a5a5>
2005d81a:	c29c                	c.sw	a5,0(a3)
2005d81c:	401c                	c.lw	a5,0(s0)
2005d81e:	5b9c                	c.lw	a5,48(a5)
2005d820:	4390                	c.lw	a2,0(a5)
2005d822:	00e61b63          	bne	a2,a4,2005d838 <vTaskSwitchContext+0x70>
2005d826:	43d4                	c.lw	a3,4(a5)
2005d828:	00c69863          	bne	a3,a2,2005d838 <vTaskSwitchContext+0x70>
2005d82c:	4798                	c.lw	a4,8(a5)
2005d82e:	00d71563          	bne	a4,a3,2005d838 <vTaskSwitchContext+0x70>
2005d832:	47dc                	c.lw	a5,12(a5)
2005d834:	00e78863          	beq	a5,a4,2005d844 <vTaskSwitchContext+0x7c>
2005d838:	4008                	c.lw	a0,0(s0)
2005d83a:	400c                	c.lw	a1,0(s0)
2005d83c:	03458593          	addi	a1,a1,52
2005d840:	dadfe0ef          	jal	ra,2005c5ec <vApplicationStackOverflowHook>
2005d844:	f841a503          	lw	a0,-124(gp) # 20066828 <uxTopReadyPriority>
2005d848:	3db030ef          	jal	ra,20061422 <__clzsi2>
2005d84c:	47fd                	c.li	a5,31
2005d84e:	40a78533          	sub	a0,a5,a0
2005d852:	4751                	c.li	a4,20
2005d854:	02e50733          	mul	a4,a0,a4
2005d858:	200687b7          	lui	a5,0x20068
2005d85c:	1e878693          	addi	a3,a5,488 # 200681e8 <pxReadyTasksLists>
2005d860:	1e878793          	addi	a5,a5,488
2005d864:	00e68633          	add	a2,a3,a4
2005d868:	424c                	c.lw	a1,4(a2)
2005d86a:	0721                	c.addi	a4,8
2005d86c:	9736                	c.add	a4,a3
2005d86e:	41cc                	c.lw	a1,4(a1)
2005d870:	c24c                	c.sw	a1,4(a2)
2005d872:	00e59463          	bne	a1,a4,2005d87a <vTaskSwitchContext+0xb2>
2005d876:	41d8                	c.lw	a4,4(a1)
2005d878:	c258                	c.sw	a4,4(a2)
2005d87a:	4751                	c.li	a4,20
2005d87c:	02e50533          	mul	a0,a0,a4
2005d880:	40b2                	c.lwsp	ra,12(sp)
2005d882:	97aa                	c.add	a5,a0
2005d884:	43dc                	c.lw	a5,4(a5)
2005d886:	47dc                	c.lw	a5,12(a5)
2005d888:	c01c                	c.sw	a5,0(s0)
2005d88a:	401c                	c.lw	a5,0(s0)
2005d88c:	4422                	c.lwsp	s0,8(sp)
2005d88e:	06c78793          	addi	a5,a5,108
2005d892:	e8f1ac23          	sw	a5,-360(gp) # 2006673c <_impure_ptr>
2005d896:	0141                	c.addi	sp,16
2005d898:	8082                	c.jr	ra

2005d89a <vTaskPlaceOnEventList>:
2005d89a:	1141                	c.addi	sp,-16
2005d89c:	c422                	c.swsp	s0,8(sp)
2005d89e:	842e                	c.mv	s0,a1
2005d8a0:	f601a583          	lw	a1,-160(gp) # 20066804 <pxCurrentTCB>
2005d8a4:	c606                	c.swsp	ra,12(sp)
2005d8a6:	05e1                	c.addi	a1,24
2005d8a8:	b42fd0ef          	jal	ra,2005abea <vListInsert>
2005d8ac:	8522                	c.mv	a0,s0
2005d8ae:	4422                	c.lwsp	s0,8(sp)
2005d8b0:	40b2                	c.lwsp	ra,12(sp)
2005d8b2:	4585                	c.li	a1,1
2005d8b4:	0141                	c.addi	sp,16
2005d8b6:	b21ff06f          	jal	zero,2005d3d6 <prvAddCurrentTaskToDelayedList>

2005d8ba <vTaskPlaceOnEventListRestricted>:
2005d8ba:	1141                	c.addi	sp,-16
2005d8bc:	c422                	c.swsp	s0,8(sp)
2005d8be:	842e                	c.mv	s0,a1
2005d8c0:	f601a583          	lw	a1,-160(gp) # 20066804 <pxCurrentTCB>
2005d8c4:	c226                	c.swsp	s1,4(sp)
2005d8c6:	84b2                	c.mv	s1,a2
2005d8c8:	05e1                	c.addi	a1,24
2005d8ca:	c606                	c.swsp	ra,12(sp)
2005d8cc:	b08fd0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005d8d0:	c091                	c.beqz	s1,2005d8d4 <vTaskPlaceOnEventListRestricted+0x1a>
2005d8d2:	547d                	c.li	s0,-1
2005d8d4:	8522                	c.mv	a0,s0
2005d8d6:	4422                	c.lwsp	s0,8(sp)
2005d8d8:	40b2                	c.lwsp	ra,12(sp)
2005d8da:	85a6                	c.mv	a1,s1
2005d8dc:	4492                	c.lwsp	s1,4(sp)
2005d8de:	0141                	c.addi	sp,16
2005d8e0:	af7ff06f          	jal	zero,2005d3d6 <prvAddCurrentTaskToDelayedList>

2005d8e4 <xTaskRemoveFromEventList>:
2005d8e4:	1101                	c.addi	sp,-32
2005d8e6:	ce06                	c.swsp	ra,28(sp)
2005d8e8:	cc22                	c.swsp	s0,24(sp)
2005d8ea:	455c                	c.lw	a5,12(a0)
2005d8ec:	47c0                	c.lw	s0,12(a5)
2005d8ee:	01840593          	addi	a1,s0,24
2005d8f2:	852e                	c.mv	a0,a1
2005d8f4:	c62e                	c.swsp	a1,12(sp)
2005d8f6:	b22fd0ef          	jal	ra,2005ac18 <uxListRemove>
2005d8fa:	f7c1a783          	lw	a5,-132(gp) # 20066820 <uxSchedulerSuspended>
2005d8fe:	45b2                	c.lwsp	a1,12(sp)
2005d900:	efa1                	c.bnez	a5,2005d958 <xTaskRemoveFromEventList+0x74>
2005d902:	00440593          	addi	a1,s0,4
2005d906:	852e                	c.mv	a0,a1
2005d908:	c62e                	c.swsp	a1,12(sp)
2005d90a:	b0efd0ef          	jal	ra,2005ac18 <uxListRemove>
2005d90e:	f8418713          	addi	a4,gp,-124 # 20066828 <uxTopReadyPriority>
2005d912:	5448                	c.lw	a0,44(s0)
2005d914:	4314                	c.lw	a3,0(a4)
2005d916:	4785                	c.li	a5,1
2005d918:	00a797b3          	sll	a5,a5,a0
2005d91c:	8fd5                	c.or	a5,a3
2005d91e:	c31c                	c.sw	a5,0(a4)
2005d920:	47d1                	c.li	a5,20
2005d922:	02f50533          	mul	a0,a0,a5
2005d926:	45b2                	c.lwsp	a1,12(sp)
2005d928:	200687b7          	lui	a5,0x20068
2005d92c:	1e878793          	addi	a5,a5,488 # 200681e8 <pxReadyTasksLists>
2005d930:	953e                	c.add	a0,a5
2005d932:	aa2fd0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005d936:	bc5ff0ef          	jal	ra,2005d4fa <prvResetNextTaskUnblockTime>
2005d93a:	f601a783          	lw	a5,-160(gp) # 20066804 <pxCurrentTCB>
2005d93e:	5458                	c.lw	a4,44(s0)
2005d940:	4501                	c.li	a0,0
2005d942:	57dc                	c.lw	a5,44(a5)
2005d944:	00e7f663          	bgeu	a5,a4,2005d950 <xTaskRemoveFromEventList+0x6c>
2005d948:	4705                	c.li	a4,1
2005d94a:	f8e1ae23          	sw	a4,-100(gp) # 20066840 <xYieldPending>
2005d94e:	4505                	c.li	a0,1
2005d950:	40f2                	c.lwsp	ra,28(sp)
2005d952:	4462                	c.lwsp	s0,24(sp)
2005d954:	6105                	c.addi16sp	sp,32
2005d956:	8082                	c.jr	ra
2005d958:	20068537          	lui	a0,0x20068
2005d95c:	2ec50513          	addi	a0,a0,748 # 200682ec <xPendingReadyList>
2005d960:	a74fd0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005d964:	bfd9                	c.j	2005d93a <xTaskRemoveFromEventList+0x56>

2005d966 <vTaskInternalSetTimeOutState>:
2005d966:	f8c1a783          	lw	a5,-116(gp) # 20066830 <xNumOfOverflows>
2005d96a:	c11c                	c.sw	a5,0(a0)
2005d96c:	f981a783          	lw	a5,-104(gp) # 2006683c <xTickCount>
2005d970:	c15c                	c.sw	a5,4(a0)
2005d972:	8082                	c.jr	ra

2005d974 <vTaskMissedYield>:
2005d974:	4705                	c.li	a4,1
2005d976:	f8e1ae23          	sw	a4,-100(gp) # 20066840 <xYieldPending>
2005d97a:	8082                	c.jr	ra

2005d97c <eTaskConfirmSleepModeStatus>:
2005d97c:	200687b7          	lui	a5,0x20068
2005d980:	2ec7a783          	lw	a5,748(a5) # 200682ec <xPendingReadyList>
2005d984:	4501                	c.li	a0,0
2005d986:	e395                	c.bnez	a5,2005d9aa <eTaskConfirmSleepModeStatus+0x2e>
2005d988:	f9c1a783          	lw	a5,-100(gp) # 20066840 <xYieldPending>
2005d98c:	ef99                	c.bnez	a5,2005d9aa <eTaskConfirmSleepModeStatus+0x2e>
2005d98e:	f901a783          	lw	a5,-112(gp) # 20066834 <xPendedTicks>
2005d992:	ef81                	c.bnez	a5,2005d9aa <eTaskConfirmSleepModeStatus+0x2e>
2005d994:	200687b7          	lui	a5,0x20068
2005d998:	3007a703          	lw	a4,768(a5) # 20068300 <xSuspendedTaskList>
2005d99c:	f741a783          	lw	a5,-140(gp) # 20066818 <uxCurrentNumberOfTasks>
2005d9a0:	4505                	c.li	a0,1
2005d9a2:	17fd                	c.addi	a5,-1
2005d9a4:	00f71363          	bne	a4,a5,2005d9aa <eTaskConfirmSleepModeStatus+0x2e>
2005d9a8:	4509                	c.li	a0,2
2005d9aa:	8082                	c.jr	ra

2005d9ac <xTaskGetCurrentTaskHandle>:
2005d9ac:	f601a503          	lw	a0,-160(gp) # 20066804 <pxCurrentTCB>
2005d9b0:	8082                	c.jr	ra

2005d9b2 <xTaskGetSchedulerState>:
2005d9b2:	f941a783          	lw	a5,-108(gp) # 20066838 <xSchedulerRunning>
2005d9b6:	4505                	c.li	a0,1
2005d9b8:	c791                	c.beqz	a5,2005d9c4 <xTaskGetSchedulerState+0x12>
2005d9ba:	f7c1a503          	lw	a0,-132(gp) # 20066820 <uxSchedulerSuspended>
2005d9be:	00153513          	sltiu	a0,a0,1
2005d9c2:	0506                	c.slli	a0,0x1
2005d9c4:	8082                	c.jr	ra

2005d9c6 <xTaskPriorityInherit>:
2005d9c6:	c945                	c.beqz	a0,2005da76 <xTaskPriorityInherit+0xb0>
2005d9c8:	1101                	c.addi	sp,-32
2005d9ca:	ca26                	c.swsp	s1,20(sp)
2005d9cc:	cc22                	c.swsp	s0,24(sp)
2005d9ce:	ce06                	c.swsp	ra,28(sp)
2005d9d0:	c84a                	c.swsp	s2,16(sp)
2005d9d2:	f6018713          	addi	a4,gp,-160 # 20066804 <pxCurrentTCB>
2005d9d6:	4314                	c.lw	a3,0(a4)
2005d9d8:	555c                	c.lw	a5,44(a0)
2005d9da:	842a                	c.mv	s0,a0
2005d9dc:	56d4                	c.lw	a3,44(a3)
2005d9de:	f6018493          	addi	s1,gp,-160 # 20066804 <pxCurrentTCB>
2005d9e2:	08d7f463          	bgeu	a5,a3,2005da6a <xTaskPriorityInherit+0xa4>
2005d9e6:	4d18                	c.lw	a4,24(a0)
2005d9e8:	00074763          	blt	a4,zero,2005d9f6 <xTaskPriorityInherit+0x30>
2005d9ec:	4098                	c.lw	a4,0(s1)
2005d9ee:	5754                	c.lw	a3,44(a4)
2005d9f0:	472d                	c.li	a4,11
2005d9f2:	8f15                	c.sub	a4,a3
2005d9f4:	cd18                	c.sw	a4,24(a0)
2005d9f6:	46d1                	c.li	a3,20
2005d9f8:	02d787b3          	mul	a5,a5,a3
2005d9fc:	20068537          	lui	a0,0x20068
2005da00:	1e850713          	addi	a4,a0,488 # 200681e8 <pxReadyTasksLists>
2005da04:	1e850913          	addi	s2,a0,488
2005da08:	97ba                	c.add	a5,a4
2005da0a:	4858                	c.lw	a4,20(s0)
2005da0c:	04f71b63          	bne	a4,a5,2005da62 <xTaskPriorityInherit+0x9c>
2005da10:	00440593          	addi	a1,s0,4
2005da14:	852e                	c.mv	a0,a1
2005da16:	c62e                	c.swsp	a1,12(sp)
2005da18:	a00fd0ef          	jal	ra,2005ac18 <uxListRemove>
2005da1c:	45b2                	c.lwsp	a1,12(sp)
2005da1e:	f8418713          	addi	a4,gp,-124 # 20066828 <uxTopReadyPriority>
2005da22:	e911                	c.bnez	a0,2005da36 <xTaskPriorityInherit+0x70>
2005da24:	5450                	c.lw	a2,44(s0)
2005da26:	4314                	c.lw	a3,0(a4)
2005da28:	4785                	c.li	a5,1
2005da2a:	00c797b3          	sll	a5,a5,a2
2005da2e:	fff7c793          	xori	a5,a5,-1
2005da32:	8ff5                	c.and	a5,a3
2005da34:	c31c                	c.sw	a5,0(a4)
2005da36:	409c                	c.lw	a5,0(s1)
2005da38:	4314                	c.lw	a3,0(a4)
2005da3a:	57c8                	c.lw	a0,44(a5)
2005da3c:	4785                	c.li	a5,1
2005da3e:	00a797b3          	sll	a5,a5,a0
2005da42:	8fd5                	c.or	a5,a3
2005da44:	c31c                	c.sw	a5,0(a4)
2005da46:	47d1                	c.li	a5,20
2005da48:	d448                	c.sw	a0,44(s0)
2005da4a:	02f50533          	mul	a0,a0,a5
2005da4e:	954a                	c.add	a0,s2
2005da50:	984fd0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005da54:	4505                	c.li	a0,1
2005da56:	40f2                	c.lwsp	ra,28(sp)
2005da58:	4462                	c.lwsp	s0,24(sp)
2005da5a:	44d2                	c.lwsp	s1,20(sp)
2005da5c:	4942                	c.lwsp	s2,16(sp)
2005da5e:	6105                	c.addi16sp	sp,32
2005da60:	8082                	c.jr	ra
2005da62:	409c                	c.lw	a5,0(s1)
2005da64:	57dc                	c.lw	a5,44(a5)
2005da66:	d45c                	c.sw	a5,44(s0)
2005da68:	b7f5                	c.j	2005da54 <xTaskPriorityInherit+0x8e>
2005da6a:	431c                	c.lw	a5,0(a4)
2005da6c:	4d68                	c.lw	a0,92(a0)
2005da6e:	57dc                	c.lw	a5,44(a5)
2005da70:	00f53533          	sltu	a0,a0,a5
2005da74:	b7cd                	c.j	2005da56 <xTaskPriorityInherit+0x90>
2005da76:	4501                	c.li	a0,0
2005da78:	8082                	c.jr	ra

2005da7a <xTaskPriorityDisinherit>:
2005da7a:	e901                	c.bnez	a0,2005da8a <xTaskPriorityDisinherit+0x10>
2005da7c:	4501                	c.li	a0,0
2005da7e:	8082                	c.jr	ra
2005da80:	4501                	c.li	a0,0
2005da82:	40f2                	c.lwsp	ra,28(sp)
2005da84:	4462                	c.lwsp	s0,24(sp)
2005da86:	6105                	c.addi16sp	sp,32
2005da88:	8082                	c.jr	ra
2005da8a:	1101                	c.addi	sp,-32
2005da8c:	cc22                	c.swsp	s0,24(sp)
2005da8e:	ce06                	c.swsp	ra,28(sp)
2005da90:	513c                	c.lw	a5,96(a0)
2005da92:	5554                	c.lw	a3,44(a0)
2005da94:	4d78                	c.lw	a4,92(a0)
2005da96:	17fd                	c.addi	a5,-1
2005da98:	d13c                	c.sw	a5,96(a0)
2005da9a:	842a                	c.mv	s0,a0
2005da9c:	fee682e3          	beq	a3,a4,2005da80 <xTaskPriorityDisinherit+0x6>
2005daa0:	f3e5                	c.bnez	a5,2005da80 <xTaskPriorityDisinherit+0x6>
2005daa2:	00450593          	addi	a1,a0,4
2005daa6:	852e                	c.mv	a0,a1
2005daa8:	c62e                	c.swsp	a1,12(sp)
2005daaa:	96efd0ef          	jal	ra,2005ac18 <uxListRemove>
2005daae:	45b2                	c.lwsp	a1,12(sp)
2005dab0:	f8418713          	addi	a4,gp,-124 # 20066828 <uxTopReadyPriority>
2005dab4:	e911                	c.bnez	a0,2005dac8 <xTaskPriorityDisinherit+0x4e>
2005dab6:	5450                	c.lw	a2,44(s0)
2005dab8:	4314                	c.lw	a3,0(a4)
2005daba:	4785                	c.li	a5,1
2005dabc:	00c797b3          	sll	a5,a5,a2
2005dac0:	fff7c793          	xori	a5,a5,-1
2005dac4:	8ff5                	c.and	a5,a3
2005dac6:	c31c                	c.sw	a5,0(a4)
2005dac8:	4c68                	c.lw	a0,92(s0)
2005daca:	47ad                	c.li	a5,11
2005dacc:	4314                	c.lw	a3,0(a4)
2005dace:	8f89                	c.sub	a5,a0
2005dad0:	cc1c                	c.sw	a5,24(s0)
2005dad2:	4785                	c.li	a5,1
2005dad4:	00a797b3          	sll	a5,a5,a0
2005dad8:	8fd5                	c.or	a5,a3
2005dada:	c31c                	c.sw	a5,0(a4)
2005dadc:	47d1                	c.li	a5,20
2005dade:	d448                	c.sw	a0,44(s0)
2005dae0:	02f50533          	mul	a0,a0,a5
2005dae4:	200687b7          	lui	a5,0x20068
2005dae8:	1e878793          	addi	a5,a5,488 # 200681e8 <pxReadyTasksLists>
2005daec:	953e                	c.add	a0,a5
2005daee:	8e6fd0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005daf2:	4505                	c.li	a0,1
2005daf4:	b779                	c.j	2005da82 <xTaskPriorityDisinherit+0x8>

2005daf6 <vTaskPriorityDisinheritAfterTimeout>:
2005daf6:	cd51                	c.beqz	a0,2005db92 <vTaskPriorityDisinheritAfterTimeout+0x9c>
2005daf8:	1101                	c.addi	sp,-32
2005dafa:	cc22                	c.swsp	s0,24(sp)
2005dafc:	ce06                	c.swsp	ra,28(sp)
2005dafe:	ca26                	c.swsp	s1,20(sp)
2005db00:	4d7c                	c.lw	a5,92(a0)
2005db02:	842a                	c.mv	s0,a0
2005db04:	00b7f363          	bgeu	a5,a1,2005db0a <vTaskPriorityDisinheritAfterTimeout+0x14>
2005db08:	87ae                	c.mv	a5,a1
2005db0a:	5458                	c.lw	a4,44(s0)
2005db0c:	06f70e63          	beq	a4,a5,2005db88 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005db10:	5030                	c.lw	a2,96(s0)
2005db12:	4685                	c.li	a3,1
2005db14:	06d61a63          	bne	a2,a3,2005db88 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005db18:	4c14                	c.lw	a3,24(s0)
2005db1a:	d45c                	c.sw	a5,44(s0)
2005db1c:	0006c663          	blt	a3,zero,2005db28 <vTaskPriorityDisinheritAfterTimeout+0x32>
2005db20:	46ad                	c.li	a3,11
2005db22:	40f687b3          	sub	a5,a3,a5
2005db26:	cc1c                	c.sw	a5,24(s0)
2005db28:	46d1                	c.li	a3,20
2005db2a:	02d70733          	mul	a4,a4,a3
2005db2e:	20068537          	lui	a0,0x20068
2005db32:	1e850793          	addi	a5,a0,488 # 200681e8 <pxReadyTasksLists>
2005db36:	1e850493          	addi	s1,a0,488
2005db3a:	973e                	c.add	a4,a5
2005db3c:	485c                	c.lw	a5,20(s0)
2005db3e:	04e79563          	bne	a5,a4,2005db88 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005db42:	00440593          	addi	a1,s0,4
2005db46:	852e                	c.mv	a0,a1
2005db48:	c62e                	c.swsp	a1,12(sp)
2005db4a:	8cefd0ef          	jal	ra,2005ac18 <uxListRemove>
2005db4e:	5454                	c.lw	a3,44(s0)
2005db50:	45b2                	c.lwsp	a1,12(sp)
2005db52:	f8418713          	addi	a4,gp,-124 # 20066828 <uxTopReadyPriority>
2005db56:	e909                	c.bnez	a0,2005db68 <vTaskPriorityDisinheritAfterTimeout+0x72>
2005db58:	4310                	c.lw	a2,0(a4)
2005db5a:	4785                	c.li	a5,1
2005db5c:	00d797b3          	sll	a5,a5,a3
2005db60:	fff7c793          	xori	a5,a5,-1
2005db64:	8ff1                	c.and	a5,a2
2005db66:	c31c                	c.sw	a5,0(a4)
2005db68:	4551                	c.li	a0,20
2005db6a:	02a68533          	mul	a0,a3,a0
2005db6e:	4310                	c.lw	a2,0(a4)
2005db70:	4785                	c.li	a5,1
2005db72:	00d797b3          	sll	a5,a5,a3
2005db76:	8fd1                	c.or	a5,a2
2005db78:	c31c                	c.sw	a5,0(a4)
2005db7a:	4462                	c.lwsp	s0,24(sp)
2005db7c:	40f2                	c.lwsp	ra,28(sp)
2005db7e:	9526                	c.add	a0,s1
2005db80:	44d2                	c.lwsp	s1,20(sp)
2005db82:	6105                	c.addi16sp	sp,32
2005db84:	850fd06f          	jal	zero,2005abd4 <vListInsertEnd>
2005db88:	40f2                	c.lwsp	ra,28(sp)
2005db8a:	4462                	c.lwsp	s0,24(sp)
2005db8c:	44d2                	c.lwsp	s1,20(sp)
2005db8e:	6105                	c.addi16sp	sp,32
2005db90:	8082                	c.jr	ra
2005db92:	8082                	c.jr	ra

2005db94 <vTaskEnterCritical>:
2005db94:	30047073          	csrrci	zero,mstatus,8
2005db98:	f941a783          	lw	a5,-108(gp) # 20066838 <xSchedulerRunning>
2005db9c:	cb81                	c.beqz	a5,2005dbac <vTaskEnterCritical+0x18>
2005db9e:	f6018793          	addi	a5,gp,-160 # 20066804 <pxCurrentTCB>
2005dba2:	4394                	c.lw	a3,0(a5)
2005dba4:	439c                	c.lw	a5,0(a5)
2005dba6:	4ab8                	c.lw	a4,80(a3)
2005dba8:	0705                	c.addi	a4,1
2005dbaa:	cab8                	c.sw	a4,80(a3)
2005dbac:	8082                	c.jr	ra

2005dbae <vTaskExitCritical>:
2005dbae:	f941a783          	lw	a5,-108(gp) # 20066838 <xSchedulerRunning>
2005dbb2:	cf99                	c.beqz	a5,2005dbd0 <vTaskExitCritical+0x22>
2005dbb4:	f6018793          	addi	a5,gp,-160 # 20066804 <pxCurrentTCB>
2005dbb8:	4398                	c.lw	a4,0(a5)
2005dbba:	4b38                	c.lw	a4,80(a4)
2005dbbc:	cb11                	c.beqz	a4,2005dbd0 <vTaskExitCritical+0x22>
2005dbbe:	4394                	c.lw	a3,0(a5)
2005dbc0:	439c                	c.lw	a5,0(a5)
2005dbc2:	4ab8                	c.lw	a4,80(a3)
2005dbc4:	177d                	c.addi	a4,-1
2005dbc6:	cab8                	c.sw	a4,80(a3)
2005dbc8:	4bbc                	c.lw	a5,80(a5)
2005dbca:	e399                	c.bnez	a5,2005dbd0 <vTaskExitCritical+0x22>
2005dbcc:	30046073          	csrrsi	zero,mstatus,8
2005dbd0:	8082                	c.jr	ra

2005dbd2 <prvAddNewTaskToReadyList>:
2005dbd2:	1101                	c.addi	sp,-32
2005dbd4:	cc22                	c.swsp	s0,24(sp)
2005dbd6:	ca26                	c.swsp	s1,20(sp)
2005dbd8:	c84a                	c.swsp	s2,16(sp)
2005dbda:	c64e                	c.swsp	s3,12(sp)
2005dbdc:	ce06                	c.swsp	ra,28(sp)
2005dbde:	c452                	c.swsp	s4,8(sp)
2005dbe0:	842a                	c.mv	s0,a0
2005dbe2:	fb3ff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005dbe6:	f7418793          	addi	a5,gp,-140 # 20066818 <uxCurrentNumberOfTasks>
2005dbea:	4398                	c.lw	a4,0(a5)
2005dbec:	20068937          	lui	s2,0x20068
2005dbf0:	0705                	c.addi	a4,1
2005dbf2:	c398                	c.sw	a4,0(a5)
2005dbf4:	f6018713          	addi	a4,gp,-160 # 20066804 <pxCurrentTCB>
2005dbf8:	4318                	c.lw	a4,0(a4)
2005dbfa:	f6018493          	addi	s1,gp,-160 # 20066804 <pxCurrentTCB>
2005dbfe:	1e890993          	addi	s3,s2,488 # 200681e8 <pxReadyTasksLists>
2005dc02:	e369                	c.bnez	a4,2005dcc4 <prvAddNewTaskToReadyList+0xf2>
2005dc04:	c080                	c.sw	s0,0(s1)
2005dc06:	4398                	c.lw	a4,0(a5)
2005dc08:	4785                	c.li	a5,1
2005dc0a:	06f71263          	bne	a4,a5,2005dc6e <prvAddNewTaskToReadyList+0x9c>
2005dc0e:	1e890913          	addi	s2,s2,488
2005dc12:	0dc98a13          	addi	s4,s3,220
2005dc16:	854a                	c.mv	a0,s2
2005dc18:	0951                	c.addi	s2,20
2005dc1a:	fa1fc0ef          	jal	ra,2005abba <vListInitialise>
2005dc1e:	ff2a1ce3          	bne	s4,s2,2005dc16 <prvAddNewTaskToReadyList+0x44>
2005dc22:	20068537          	lui	a0,0x20068
2005dc26:	2c450a13          	addi	s4,a0,708 # 200682c4 <xDelayedTaskList1>
2005dc2a:	2c450513          	addi	a0,a0,708
2005dc2e:	f8dfc0ef          	jal	ra,2005abba <vListInitialise>
2005dc32:	20068537          	lui	a0,0x20068
2005dc36:	2d850913          	addi	s2,a0,728 # 200682d8 <xDelayedTaskList2>
2005dc3a:	2d850513          	addi	a0,a0,728
2005dc3e:	f7dfc0ef          	jal	ra,2005abba <vListInitialise>
2005dc42:	20068537          	lui	a0,0x20068
2005dc46:	2ec50513          	addi	a0,a0,748 # 200682ec <xPendingReadyList>
2005dc4a:	f71fc0ef          	jal	ra,2005abba <vListInitialise>
2005dc4e:	20068537          	lui	a0,0x20068
2005dc52:	31450513          	addi	a0,a0,788 # 20068314 <xTasksWaitingTermination>
2005dc56:	f65fc0ef          	jal	ra,2005abba <vListInitialise>
2005dc5a:	20068537          	lui	a0,0x20068
2005dc5e:	30050513          	addi	a0,a0,768 # 20068300 <xSuspendedTaskList>
2005dc62:	f59fc0ef          	jal	ra,2005abba <vListInitialise>
2005dc66:	f741a223          	sw	s4,-156(gp) # 20066808 <pxDelayedTaskList>
2005dc6a:	f721a423          	sw	s2,-152(gp) # 2006680c <pxOverflowDelayedTaskList>
2005dc6e:	f8018713          	addi	a4,gp,-128 # 20066824 <uxTaskNumber>
2005dc72:	431c                	c.lw	a5,0(a4)
2005dc74:	5448                	c.lw	a0,44(s0)
2005dc76:	00440593          	addi	a1,s0,4
2005dc7a:	0785                	c.addi	a5,1
2005dc7c:	c31c                	c.sw	a5,0(a4)
2005dc7e:	f8418713          	addi	a4,gp,-124 # 20066828 <uxTopReadyPriority>
2005dc82:	4314                	c.lw	a3,0(a4)
2005dc84:	c87c                	c.sw	a5,84(s0)
2005dc86:	4785                	c.li	a5,1
2005dc88:	00a797b3          	sll	a5,a5,a0
2005dc8c:	8fd5                	c.or	a5,a3
2005dc8e:	c31c                	c.sw	a5,0(a4)
2005dc90:	47d1                	c.li	a5,20
2005dc92:	02f50533          	mul	a0,a0,a5
2005dc96:	954e                	c.add	a0,s3
2005dc98:	f3dfc0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005dc9c:	f13ff0ef          	jal	ra,2005dbae <vTaskExitCritical>
2005dca0:	f941a783          	lw	a5,-108(gp) # 20066838 <xSchedulerRunning>
2005dca4:	cb81                	c.beqz	a5,2005dcb4 <prvAddNewTaskToReadyList+0xe2>
2005dca6:	409c                	c.lw	a5,0(s1)
2005dca8:	57d8                	c.lw	a4,44(a5)
2005dcaa:	545c                	c.lw	a5,44(s0)
2005dcac:	00f77463          	bgeu	a4,a5,2005dcb4 <prvAddNewTaskToReadyList+0xe2>
2005dcb0:	00000073          	ecall
2005dcb4:	40f2                	c.lwsp	ra,28(sp)
2005dcb6:	4462                	c.lwsp	s0,24(sp)
2005dcb8:	44d2                	c.lwsp	s1,20(sp)
2005dcba:	4942                	c.lwsp	s2,16(sp)
2005dcbc:	49b2                	c.lwsp	s3,12(sp)
2005dcbe:	4a22                	c.lwsp	s4,8(sp)
2005dcc0:	6105                	c.addi16sp	sp,32
2005dcc2:	8082                	c.jr	ra
2005dcc4:	f941a783          	lw	a5,-108(gp) # 20066838 <xSchedulerRunning>
2005dcc8:	f3dd                	c.bnez	a5,2005dc6e <prvAddNewTaskToReadyList+0x9c>
2005dcca:	409c                	c.lw	a5,0(s1)
2005dccc:	57d8                	c.lw	a4,44(a5)
2005dcce:	545c                	c.lw	a5,44(s0)
2005dcd0:	f8e7efe3          	bltu	a5,a4,2005dc6e <prvAddNewTaskToReadyList+0x9c>
2005dcd4:	c080                	c.sw	s0,0(s1)
2005dcd6:	bf61                	c.j	2005dc6e <prvAddNewTaskToReadyList+0x9c>

2005dcd8 <xTaskCreateStatic>:
2005dcd8:	1101                	c.addi	sp,-32
2005dcda:	cc22                	c.swsp	s0,24(sp)
2005dcdc:	ce06                	c.swsp	ra,28(sp)
2005dcde:	8442                	c.mv	s0,a6
2005dce0:	00080f63          	beq	a6,zero,2005dcfe <xTaskCreateStatic+0x26>
2005dce4:	c395                	c.beqz	a5,2005dd08 <xTaskCreateStatic+0x30>
2005dce6:	02f82823          	sw	a5,48(a6)
2005dcea:	4789                	c.li	a5,2
2005dcec:	16f800a3          	sb	a5,353(a6)
2005dcf0:	007c                	c.addi4spn	a5,sp,12
2005dcf2:	85bff0ef          	jal	ra,2005d54c <prvInitialiseNewTask.constprop.0>
2005dcf6:	8522                	c.mv	a0,s0
2005dcf8:	edbff0ef          	jal	ra,2005dbd2 <prvAddNewTaskToReadyList>
2005dcfc:	4432                	c.lwsp	s0,12(sp)
2005dcfe:	40f2                	c.lwsp	ra,28(sp)
2005dd00:	8522                	c.mv	a0,s0
2005dd02:	4462                	c.lwsp	s0,24(sp)
2005dd04:	6105                	c.addi16sp	sp,32
2005dd06:	8082                	c.jr	ra
2005dd08:	4401                	c.li	s0,0
2005dd0a:	bfd5                	c.j	2005dcfe <xTaskCreateStatic+0x26>

2005dd0c <vTaskStartScheduler>:
2005dd0c:	1101                	c.addi	sp,-32
2005dd0e:	0070                	c.addi4spn	a2,sp,12
2005dd10:	002c                	c.addi4spn	a1,sp,8
2005dd12:	0048                	c.addi4spn	a0,sp,4
2005dd14:	ce06                	c.swsp	ra,28(sp)
2005dd16:	c202                	c.swsp	zero,4(sp)
2005dd18:	c402                	c.swsp	zero,8(sp)
2005dd1a:	8effe0ef          	jal	ra,2005c608 <vApplicationGetIdleTaskMemory>
2005dd1e:	4812                	c.lwsp	a6,4(sp)
2005dd20:	47a2                	c.lwsp	a5,8(sp)
2005dd22:	4632                	c.lwsp	a2,12(sp)
2005dd24:	200635b7          	lui	a1,0x20063
2005dd28:	2005e537          	lui	a0,0x2005e
2005dd2c:	4701                	c.li	a4,0
2005dd2e:	4681                	c.li	a3,0
2005dd30:	5bc58593          	addi	a1,a1,1468 # 200635bc <__func__.1+0x18>
2005dd34:	03050513          	addi	a0,a0,48 # 2005e030 <prvIdleTask>
2005dd38:	fa1ff0ef          	jal	ra,2005dcd8 <xTaskCreateStatic>
2005dd3c:	c51d                	c.beqz	a0,2005dd6a <vTaskStartScheduler+0x5e>
2005dd3e:	76a000ef          	jal	ra,2005e4a8 <xTimerCreateTimerTask>
2005dd42:	4785                	c.li	a5,1
2005dd44:	02f51363          	bne	a0,a5,2005dd6a <vTaskStartScheduler+0x5e>
2005dd48:	30047073          	csrrci	zero,mstatus,8
2005dd4c:	f601a783          	lw	a5,-160(gp) # 20066804 <pxCurrentTCB>
2005dd50:	06c78793          	addi	a5,a5,108
2005dd54:	e8f1ac23          	sw	a5,-360(gp) # 2006673c <_impure_ptr>
2005dd58:	577d                	c.li	a4,-1
2005dd5a:	f8e1a423          	sw	a4,-120(gp) # 2006682c <xNextTaskUnblockTime>
2005dd5e:	f8a1aa23          	sw	a0,-108(gp) # 20066838 <xSchedulerRunning>
2005dd62:	f801ac23          	sw	zero,-104(gp) # 2006683c <xTickCount>
2005dd66:	811fe0ef          	jal	ra,2005c576 <xPortStartScheduler>
2005dd6a:	40f2                	c.lwsp	ra,28(sp)
2005dd6c:	e901a783          	lw	a5,-368(gp) # 20066734 <uxTopUsedPriority>
2005dd70:	6105                	c.addi16sp	sp,32
2005dd72:	8082                	c.jr	ra

2005dd74 <xTaskCreate>:
2005dd74:	7179                	c.addi16sp	sp,-48
2005dd76:	ce4e                	c.swsp	s3,28(sp)
2005dd78:	89aa                	c.mv	s3,a0
2005dd7a:	00261513          	slli	a0,a2,0x2
2005dd7e:	d04a                	c.swsp	s2,32(sp)
2005dd80:	cc52                	c.swsp	s4,24(sp)
2005dd82:	ca56                	c.swsp	s5,20(sp)
2005dd84:	c85a                	c.swsp	s6,16(sp)
2005dd86:	c65e                	c.swsp	s7,12(sp)
2005dd88:	d606                	c.swsp	ra,44(sp)
2005dd8a:	d422                	c.swsp	s0,40(sp)
2005dd8c:	d226                	c.swsp	s1,36(sp)
2005dd8e:	8a2e                	c.mv	s4,a1
2005dd90:	8932                	c.mv	s2,a2
2005dd92:	8ab6                	c.mv	s5,a3
2005dd94:	8b3a                	c.mv	s6,a4
2005dd96:	8bbe                	c.mv	s7,a5
2005dd98:	be9fc0ef          	jal	ra,2005a980 <pvPortMalloc>
2005dd9c:	c531                	c.beqz	a0,2005dde8 <xTaskCreate+0x74>
2005dd9e:	84aa                	c.mv	s1,a0
2005dda0:	16400513          	addi	a0,zero,356
2005dda4:	bddfc0ef          	jal	ra,2005a980 <pvPortMalloc>
2005dda8:	842a                	c.mv	s0,a0
2005ddaa:	cd05                	c.beqz	a0,2005dde2 <xTaskCreate+0x6e>
2005ddac:	882a                	c.mv	a6,a0
2005ddae:	d904                	c.sw	s1,48(a0)
2005ddb0:	160500a3          	sb	zero,353(a0)
2005ddb4:	87de                	c.mv	a5,s7
2005ddb6:	875a                	c.mv	a4,s6
2005ddb8:	86d6                	c.mv	a3,s5
2005ddba:	864a                	c.mv	a2,s2
2005ddbc:	85d2                	c.mv	a1,s4
2005ddbe:	854e                	c.mv	a0,s3
2005ddc0:	f8cff0ef          	jal	ra,2005d54c <prvInitialiseNewTask.constprop.0>
2005ddc4:	8522                	c.mv	a0,s0
2005ddc6:	e0dff0ef          	jal	ra,2005dbd2 <prvAddNewTaskToReadyList>
2005ddca:	4505                	c.li	a0,1
2005ddcc:	50b2                	c.lwsp	ra,44(sp)
2005ddce:	5422                	c.lwsp	s0,40(sp)
2005ddd0:	5492                	c.lwsp	s1,36(sp)
2005ddd2:	5902                	c.lwsp	s2,32(sp)
2005ddd4:	49f2                	c.lwsp	s3,28(sp)
2005ddd6:	4a62                	c.lwsp	s4,24(sp)
2005ddd8:	4ad2                	c.lwsp	s5,20(sp)
2005ddda:	4b42                	c.lwsp	s6,16(sp)
2005dddc:	4bb2                	c.lwsp	s7,12(sp)
2005ddde:	6145                	c.addi16sp	sp,48
2005dde0:	8082                	c.jr	ra
2005dde2:	8526                	c.mv	a0,s1
2005dde4:	c7bfc0ef          	jal	ra,2005aa5e <vPortFree>
2005dde8:	557d                	c.li	a0,-1
2005ddea:	b7cd                	c.j	2005ddcc <xTaskCreate+0x58>

2005ddec <vTaskDelete>:
2005ddec:	1141                	c.addi	sp,-16
2005ddee:	c422                	c.swsp	s0,8(sp)
2005ddf0:	c226                	c.swsp	s1,4(sp)
2005ddf2:	842a                	c.mv	s0,a0
2005ddf4:	c606                	c.swsp	ra,12(sp)
2005ddf6:	c04a                	c.swsp	s2,0(sp)
2005ddf8:	f6018493          	addi	s1,gp,-160 # 20066804 <pxCurrentTCB>
2005ddfc:	d99ff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005de00:	e011                	c.bnez	s0,2005de04 <vTaskDelete+0x18>
2005de02:	4080                	c.lw	s0,0(s1)
2005de04:	00440913          	addi	s2,s0,4
2005de08:	854a                	c.mv	a0,s2
2005de0a:	e0ffc0ef          	jal	ra,2005ac18 <uxListRemove>
2005de0e:	e515                	c.bnez	a0,2005de3a <vTaskDelete+0x4e>
2005de10:	5450                	c.lw	a2,44(s0)
2005de12:	4751                	c.li	a4,20
2005de14:	02e606b3          	mul	a3,a2,a4
2005de18:	20068737          	lui	a4,0x20068
2005de1c:	1e870713          	addi	a4,a4,488 # 200681e8 <pxReadyTasksLists>
2005de20:	9736                	c.add	a4,a3
2005de22:	431c                	c.lw	a5,0(a4)
2005de24:	eb99                	c.bnez	a5,2005de3a <vTaskDelete+0x4e>
2005de26:	f8418713          	addi	a4,gp,-124 # 20066828 <uxTopReadyPriority>
2005de2a:	4314                	c.lw	a3,0(a4)
2005de2c:	4785                	c.li	a5,1
2005de2e:	00c797b3          	sll	a5,a5,a2
2005de32:	fff7c793          	xori	a5,a5,-1
2005de36:	8ff5                	c.and	a5,a3
2005de38:	c31c                	c.sw	a5,0(a4)
2005de3a:	541c                	c.lw	a5,40(s0)
2005de3c:	c789                	c.beqz	a5,2005de46 <vTaskDelete+0x5a>
2005de3e:	01840513          	addi	a0,s0,24
2005de42:	dd7fc0ef          	jal	ra,2005ac18 <uxListRemove>
2005de46:	f8018793          	addi	a5,gp,-128 # 20066824 <uxTaskNumber>
2005de4a:	4398                	c.lw	a4,0(a5)
2005de4c:	0705                	c.addi	a4,1
2005de4e:	c398                	c.sw	a4,0(a5)
2005de50:	409c                	c.lw	a5,0(s1)
2005de52:	02879e63          	bne	a5,s0,2005de8e <vTaskDelete+0xa2>
2005de56:	20068537          	lui	a0,0x20068
2005de5a:	85ca                	c.mv	a1,s2
2005de5c:	31450513          	addi	a0,a0,788 # 20068314 <xTasksWaitingTermination>
2005de60:	d75fc0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005de64:	f7818793          	addi	a5,gp,-136 # 2006681c <uxDeletedTasksWaitingCleanUp>
2005de68:	4398                	c.lw	a4,0(a5)
2005de6a:	0705                	c.addi	a4,1
2005de6c:	c398                	c.sw	a4,0(a5)
2005de6e:	d41ff0ef          	jal	ra,2005dbae <vTaskExitCritical>
2005de72:	f941a783          	lw	a5,-108(gp) # 20066838 <xSchedulerRunning>
2005de76:	c791                	c.beqz	a5,2005de82 <vTaskDelete+0x96>
2005de78:	409c                	c.lw	a5,0(s1)
2005de7a:	00879463          	bne	a5,s0,2005de82 <vTaskDelete+0x96>
2005de7e:	00000073          	ecall
2005de82:	40b2                	c.lwsp	ra,12(sp)
2005de84:	4422                	c.lwsp	s0,8(sp)
2005de86:	4492                	c.lwsp	s1,4(sp)
2005de88:	4902                	c.lwsp	s2,0(sp)
2005de8a:	0141                	c.addi	sp,16
2005de8c:	8082                	c.jr	ra
2005de8e:	f7418793          	addi	a5,gp,-140 # 20066818 <uxCurrentNumberOfTasks>
2005de92:	4398                	c.lw	a4,0(a5)
2005de94:	8522                	c.mv	a0,s0
2005de96:	177d                	c.addi	a4,-1
2005de98:	c398                	c.sw	a4,0(a5)
2005de9a:	e7cff0ef          	jal	ra,2005d516 <prvDeleteTCB>
2005de9e:	e5cff0ef          	jal	ra,2005d4fa <prvResetNextTaskUnblockTime>
2005dea2:	b7f1                	c.j	2005de6e <vTaskDelete+0x82>

2005dea4 <eTaskGetState>:
2005dea4:	f601a783          	lw	a5,-160(gp) # 20066804 <pxCurrentTCB>
2005dea8:	06a78863          	beq	a5,a0,2005df18 <eTaskGetState+0x74>
2005deac:	1101                	c.addi	sp,-32
2005deae:	ce06                	c.swsp	ra,28(sp)
2005deb0:	cc22                	c.swsp	s0,24(sp)
2005deb2:	ca26                	c.swsp	s1,20(sp)
2005deb4:	842a                	c.mv	s0,a0
2005deb6:	c84a                	c.swsp	s2,16(sp)
2005deb8:	c64e                	c.swsp	s3,12(sp)
2005deba:	cdbff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005debe:	f641a983          	lw	s3,-156(gp) # 20066808 <pxDelayedTaskList>
2005dec2:	4844                	c.lw	s1,20(s0)
2005dec4:	f681a903          	lw	s2,-152(gp) # 2006680c <pxOverflowDelayedTaskList>
2005dec8:	ce7ff0ef          	jal	ra,2005dbae <vTaskExitCritical>
2005decc:	4509                	c.li	a0,2
2005dece:	03348463          	beq	s1,s3,2005def6 <eTaskGetState+0x52>
2005ded2:	03248263          	beq	s1,s2,2005def6 <eTaskGetState+0x52>
2005ded6:	200687b7          	lui	a5,0x20068
2005deda:	30078793          	addi	a5,a5,768 # 20068300 <xSuspendedTaskList>
2005dede:	02f49363          	bne	s1,a5,2005df04 <eTaskGetState+0x60>
2005dee2:	541c                	c.lw	a5,40(s0)
2005dee4:	eb89                	c.bnez	a5,2005def6 <eTaskGetState+0x52>
2005dee6:	16044783          	lbu	a5,352(s0)
2005deea:	4705                	c.li	a4,1
2005deec:	0ff7f793          	andi	a5,a5,255
2005def0:	00e78363          	beq	a5,a4,2005def6 <eTaskGetState+0x52>
2005def4:	450d                	c.li	a0,3
2005def6:	40f2                	c.lwsp	ra,28(sp)
2005def8:	4462                	c.lwsp	s0,24(sp)
2005defa:	44d2                	c.lwsp	s1,20(sp)
2005defc:	4942                	c.lwsp	s2,16(sp)
2005defe:	49b2                	c.lwsp	s3,12(sp)
2005df00:	6105                	c.addi16sp	sp,32
2005df02:	8082                	c.jr	ra
2005df04:	200687b7          	lui	a5,0x20068
2005df08:	31478793          	addi	a5,a5,788 # 20068314 <xTasksWaitingTermination>
2005df0c:	4511                	c.li	a0,4
2005df0e:	fef484e3          	beq	s1,a5,2005def6 <eTaskGetState+0x52>
2005df12:	d0f5                	c.beqz	s1,2005def6 <eTaskGetState+0x52>
2005df14:	4505                	c.li	a0,1
2005df16:	b7c5                	c.j	2005def6 <eTaskGetState+0x52>
2005df18:	4501                	c.li	a0,0
2005df1a:	8082                	c.jr	ra

2005df1c <xTaskResumeAll>:
2005df1c:	7139                	c.addi16sp	sp,-64
2005df1e:	de06                	c.swsp	ra,60(sp)
2005df20:	dc22                	c.swsp	s0,56(sp)
2005df22:	da26                	c.swsp	s1,52(sp)
2005df24:	d84a                	c.swsp	s2,48(sp)
2005df26:	d64e                	c.swsp	s3,44(sp)
2005df28:	d452                	c.swsp	s4,40(sp)
2005df2a:	d256                	c.swsp	s5,36(sp)
2005df2c:	d05a                	c.swsp	s6,32(sp)
2005df2e:	ce5e                	c.swsp	s7,28(sp)
2005df30:	c65ff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005df34:	f7c18793          	addi	a5,gp,-132 # 20066820 <uxSchedulerSuspended>
2005df38:	4398                	c.lw	a4,0(a5)
2005df3a:	177d                	c.addi	a4,-1
2005df3c:	c398                	c.sw	a4,0(a5)
2005df3e:	439c                	c.lw	a5,0(a5)
2005df40:	c38d                	c.beqz	a5,2005df62 <xTaskResumeAll+0x46>
2005df42:	4501                	c.li	a0,0
2005df44:	c62a                	c.swsp	a0,12(sp)
2005df46:	c69ff0ef          	jal	ra,2005dbae <vTaskExitCritical>
2005df4a:	50f2                	c.lwsp	ra,60(sp)
2005df4c:	5462                	c.lwsp	s0,56(sp)
2005df4e:	4532                	c.lwsp	a0,12(sp)
2005df50:	54d2                	c.lwsp	s1,52(sp)
2005df52:	5942                	c.lwsp	s2,48(sp)
2005df54:	59b2                	c.lwsp	s3,44(sp)
2005df56:	5a22                	c.lwsp	s4,40(sp)
2005df58:	5a92                	c.lwsp	s5,36(sp)
2005df5a:	5b02                	c.lwsp	s6,32(sp)
2005df5c:	4bf2                	c.lwsp	s7,28(sp)
2005df5e:	6121                	c.addi16sp	sp,64
2005df60:	8082                	c.jr	ra
2005df62:	f741a783          	lw	a5,-140(gp) # 20066818 <uxCurrentNumberOfTasks>
2005df66:	dff1                	c.beqz	a5,2005df42 <xTaskResumeAll+0x26>
2005df68:	200684b7          	lui	s1,0x20068
2005df6c:	20068937          	lui	s2,0x20068
2005df70:	4401                	c.li	s0,0
2005df72:	2ec48493          	addi	s1,s1,748 # 200682ec <xPendingReadyList>
2005df76:	4985                	c.li	s3,1
2005df78:	1e890913          	addi	s2,s2,488 # 200681e8 <pxReadyTasksLists>
2005df7c:	4ad1                	c.li	s5,20
2005df7e:	a0b1                	c.j	2005dfca <xTaskResumeAll+0xae>
2005df80:	44dc                	c.lw	a5,12(s1)
2005df82:	47c0                	c.lw	s0,12(a5)
2005df84:	01840513          	addi	a0,s0,24
2005df88:	c91fc0ef          	jal	ra,2005ac18 <uxListRemove>
2005df8c:	00440593          	addi	a1,s0,4
2005df90:	852e                	c.mv	a0,a1
2005df92:	c62e                	c.swsp	a1,12(sp)
2005df94:	c85fc0ef          	jal	ra,2005ac18 <uxListRemove>
2005df98:	5448                	c.lw	a0,44(s0)
2005df9a:	f8418713          	addi	a4,gp,-124 # 20066828 <uxTopReadyPriority>
2005df9e:	4314                	c.lw	a3,0(a4)
2005dfa0:	00a997b3          	sll	a5,s3,a0
2005dfa4:	03550533          	mul	a0,a0,s5
2005dfa8:	45b2                	c.lwsp	a1,12(sp)
2005dfaa:	8fd5                	c.or	a5,a3
2005dfac:	c31c                	c.sw	a5,0(a4)
2005dfae:	954a                	c.add	a0,s2
2005dfb0:	c25fc0ef          	jal	ra,2005abd4 <vListInsertEnd>
2005dfb4:	f6018793          	addi	a5,gp,-160 # 20066804 <pxCurrentTCB>
2005dfb8:	439c                	c.lw	a5,0(a5)
2005dfba:	5458                	c.lw	a4,44(s0)
2005dfbc:	57dc                	c.lw	a5,44(a5)
2005dfbe:	00f76663          	bltu	a4,a5,2005dfca <xTaskResumeAll+0xae>
2005dfc2:	f9c18793          	addi	a5,gp,-100 # 20066840 <xYieldPending>
2005dfc6:	0137a023          	sw	s3,0(a5)
2005dfca:	409c                	c.lw	a5,0(s1)
2005dfcc:	fbd5                	c.bnez	a5,2005df80 <xTaskResumeAll+0x64>
2005dfce:	c019                	c.beqz	s0,2005dfd4 <xTaskResumeAll+0xb8>
2005dfd0:	d2aff0ef          	jal	ra,2005d4fa <prvResetNextTaskUnblockTime>
2005dfd4:	f9018713          	addi	a4,gp,-112 # 20066834 <xPendedTicks>
2005dfd8:	4304                	c.lw	s1,0(a4)
2005dfda:	f9018413          	addi	s0,gp,-112 # 20066834 <xPendedTicks>
2005dfde:	cc89                	c.beqz	s1,2005dff8 <xTaskResumeAll+0xdc>
2005dfe0:	4985                	c.li	s3,1
2005dfe2:	eb0ff0ef          	jal	ra,2005d692 <xTaskIncrementTick>
2005dfe6:	c509                	c.beqz	a0,2005dff0 <xTaskResumeAll+0xd4>
2005dfe8:	f9c18793          	addi	a5,gp,-100 # 20066840 <xYieldPending>
2005dfec:	0137a023          	sw	s3,0(a5)
2005dff0:	14fd                	c.addi	s1,-1
2005dff2:	f8e5                	c.bnez	s1,2005dfe2 <xTaskResumeAll+0xc6>
2005dff4:	00042023          	sw	zero,0(s0)
2005dff8:	f9c1a783          	lw	a5,-100(gp) # 20066840 <xYieldPending>
2005dffc:	d3b9                	c.beqz	a5,2005df42 <xTaskResumeAll+0x26>
2005dffe:	00000073          	ecall
2005e002:	4505                	c.li	a0,1
2005e004:	b781                	c.j	2005df44 <xTaskResumeAll+0x28>

2005e006 <vTaskDelay>:
2005e006:	e501                	c.bnez	a0,2005e00e <vTaskDelay+0x8>
2005e008:	00000073          	ecall
2005e00c:	8082                	c.jr	ra
2005e00e:	1101                	c.addi	sp,-32
2005e010:	ce06                	c.swsp	ra,28(sp)
2005e012:	c62a                	c.swsp	a0,12(sp)
2005e014:	e5aff0ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005e018:	4532                	c.lwsp	a0,12(sp)
2005e01a:	4581                	c.li	a1,0
2005e01c:	bbaff0ef          	jal	ra,2005d3d6 <prvAddCurrentTaskToDelayedList>
2005e020:	efdff0ef          	jal	ra,2005df1c <xTaskResumeAll>
2005e024:	e119                	c.bnez	a0,2005e02a <vTaskDelay+0x24>
2005e026:	00000073          	ecall
2005e02a:	40f2                	c.lwsp	ra,28(sp)
2005e02c:	6105                	c.addi16sp	sp,32
2005e02e:	8082                	c.jr	ra

2005e030 <prvIdleTask>:
2005e030:	1101                	c.addi	sp,-32
2005e032:	ca26                	c.swsp	s1,20(sp)
2005e034:	200684b7          	lui	s1,0x20068
2005e038:	cc22                	c.swsp	s0,24(sp)
2005e03a:	c452                	c.swsp	s4,8(sp)
2005e03c:	c256                	c.swsp	s5,4(sp)
2005e03e:	c05a                	c.swsp	s6,0(sp)
2005e040:	ce06                	c.swsp	ra,28(sp)
2005e042:	c84a                	c.swsp	s2,16(sp)
2005e044:	c64e                	c.swsp	s3,12(sp)
2005e046:	31448493          	addi	s1,s1,788 # 20068314 <xTasksWaitingTermination>
2005e04a:	20068ab7          	lui	s5,0x20068
2005e04e:	4405                	c.li	s0,1
2005e050:	f7818913          	addi	s2,gp,-136 # 2006681c <uxDeletedTasksWaitingCleanUp>
2005e054:	00092783          	lw	a5,0(s2)
2005e058:	eb8d                	c.bnez	a5,2005e08a <prvIdleTask+0x5a>
2005e05a:	1e8a8793          	addi	a5,s5,488 # 200681e8 <pxReadyTasksLists>
2005e05e:	439c                	c.lw	a5,0(a5)
2005e060:	00f47463          	bgeu	s0,a5,2005e068 <prvIdleTask+0x38>
2005e064:	00000073          	ecall
2005e068:	d2efe0ef          	jal	ra,2005c596 <vApplicationIdleHook>
2005e06c:	c60ff0ef          	jal	ra,2005d4cc <prvGetExpectedIdleTime>
2005e070:	fea470e3          	bgeu	s0,a0,2005e050 <prvIdleTask+0x20>
2005e074:	dfaff0ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005e078:	c54ff0ef          	jal	ra,2005d4cc <prvGetExpectedIdleTime>
2005e07c:	00a47463          	bgeu	s0,a0,2005e084 <prvIdleTask+0x54>
2005e080:	dc0fe0ef          	jal	ra,2005c640 <vPortSuppressTicksAndSleep>
2005e084:	e99ff0ef          	jal	ra,2005df1c <xTaskResumeAll>
2005e088:	b7e1                	c.j	2005e050 <prvIdleTask+0x20>
2005e08a:	b0bff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005e08e:	44dc                	c.lw	a5,12(s1)
2005e090:	00c7a983          	lw	s3,12(a5)
2005e094:	00498513          	addi	a0,s3,4
2005e098:	b81fc0ef          	jal	ra,2005ac18 <uxListRemove>
2005e09c:	f7418713          	addi	a4,gp,-140 # 20066818 <uxCurrentNumberOfTasks>
2005e0a0:	431c                	c.lw	a5,0(a4)
2005e0a2:	17fd                	c.addi	a5,-1
2005e0a4:	c31c                	c.sw	a5,0(a4)
2005e0a6:	00092783          	lw	a5,0(s2)
2005e0aa:	17fd                	c.addi	a5,-1
2005e0ac:	00f92023          	sw	a5,0(s2)
2005e0b0:	affff0ef          	jal	ra,2005dbae <vTaskExitCritical>
2005e0b4:	854e                	c.mv	a0,s3
2005e0b6:	c60ff0ef          	jal	ra,2005d516 <prvDeleteTCB>
2005e0ba:	bf59                	c.j	2005e050 <prvIdleTask+0x20>

2005e0bc <vTaskGetInfo>:
2005e0bc:	1141                	c.addi	sp,-16
2005e0be:	c422                	c.swsp	s0,8(sp)
2005e0c0:	c226                	c.swsp	s1,4(sp)
2005e0c2:	c04a                	c.swsp	s2,0(sp)
2005e0c4:	c606                	c.swsp	ra,12(sp)
2005e0c6:	842e                	c.mv	s0,a1
2005e0c8:	8932                	c.mv	s2,a2
2005e0ca:	84aa                	c.mv	s1,a0
2005e0cc:	e119                	c.bnez	a0,2005e0d2 <vTaskGetInfo+0x16>
2005e0ce:	f601a483          	lw	s1,-160(gp) # 20066804 <pxCurrentTCB>
2005e0d2:	03448793          	addi	a5,s1,52
2005e0d6:	c004                	c.sw	s1,0(s0)
2005e0d8:	c05c                	c.sw	a5,4(s0)
2005e0da:	54dc                	c.lw	a5,44(s1)
2005e0dc:	c81c                	c.sw	a5,16(s0)
2005e0de:	589c                	c.lw	a5,48(s1)
2005e0e0:	cc5c                	c.sw	a5,28(s0)
2005e0e2:	48fc                	c.lw	a5,84(s1)
2005e0e4:	c41c                	c.sw	a5,8(s0)
2005e0e6:	4cfc                	c.lw	a5,92(s1)
2005e0e8:	c85c                	c.sw	a5,20(s0)
2005e0ea:	54bc                	c.lw	a5,104(s1)
2005e0ec:	cc1c                	c.sw	a5,24(s0)
2005e0ee:	4795                	c.li	a5,5
2005e0f0:	04f68c63          	beq	a3,a5,2005e148 <vTaskGetInfo+0x8c>
2005e0f4:	f601a783          	lw	a5,-160(gp) # 20066804 <pxCurrentTCB>
2005e0f8:	02979963          	bne	a5,s1,2005e12a <vTaskGetInfo+0x6e>
2005e0fc:	00040623          	sb	zero,12(s0)
2005e100:	04090c63          	beq	s2,zero,2005e158 <vTaskGetInfo+0x9c>
2005e104:	5890                	c.lw	a2,48(s1)
2005e106:	4781                	c.li	a5,0
2005e108:	0a500713          	addi	a4,zero,165
2005e10c:	00f606b3          	add	a3,a2,a5
2005e110:	0006c683          	lbu	a3,0(a3)
2005e114:	04e68063          	beq	a3,a4,2005e154 <vTaskGetInfo+0x98>
2005e118:	8389                	c.srli	a5,0x2
2005e11a:	02f41023          	sh	a5,32(s0)
2005e11e:	40b2                	c.lwsp	ra,12(sp)
2005e120:	4422                	c.lwsp	s0,8(sp)
2005e122:	4492                	c.lwsp	s1,4(sp)
2005e124:	4902                	c.lwsp	s2,0(sp)
2005e126:	0141                	c.addi	sp,16
2005e128:	8082                	c.jr	ra
2005e12a:	00d40623          	sb	a3,12(s0)
2005e12e:	478d                	c.li	a5,3
2005e130:	fcf698e3          	bne	a3,a5,2005e100 <vTaskGetInfo+0x44>
2005e134:	d3aff0ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005e138:	549c                	c.lw	a5,40(s1)
2005e13a:	c781                	c.beqz	a5,2005e142 <vTaskGetInfo+0x86>
2005e13c:	4789                	c.li	a5,2
2005e13e:	00f40623          	sb	a5,12(s0)
2005e142:	ddbff0ef          	jal	ra,2005df1c <xTaskResumeAll>
2005e146:	bf6d                	c.j	2005e100 <vTaskGetInfo+0x44>
2005e148:	8526                	c.mv	a0,s1
2005e14a:	d5bff0ef          	jal	ra,2005dea4 <eTaskGetState>
2005e14e:	00a40623          	sb	a0,12(s0)
2005e152:	b77d                	c.j	2005e100 <vTaskGetInfo+0x44>
2005e154:	0785                	c.addi	a5,1
2005e156:	bf5d                	c.j	2005e10c <vTaskGetInfo+0x50>
2005e158:	02041023          	sh	zero,32(s0)
2005e15c:	b7c9                	c.j	2005e11e <vTaskGetInfo+0x62>

2005e15e <prvListTasksWithinSingleList>:
2005e15e:	7179                	c.addi16sp	sp,-48
2005e160:	d606                	c.swsp	ra,44(sp)
2005e162:	d422                	c.swsp	s0,40(sp)
2005e164:	d226                	c.swsp	s1,36(sp)
2005e166:	d04a                	c.swsp	s2,32(sp)
2005e168:	ce4e                	c.swsp	s3,28(sp)
2005e16a:	cc52                	c.swsp	s4,24(sp)
2005e16c:	ca56                	c.swsp	s5,20(sp)
2005e16e:	c85a                	c.swsp	s6,16(sp)
2005e170:	c65e                	c.swsp	s7,12(sp)
2005e172:	4184                	c.lw	s1,0(a1)
2005e174:	c8a9                	c.beqz	s1,2005e1c6 <prvListTasksWithinSingleList+0x68>
2005e176:	41dc                	c.lw	a5,4(a1)
2005e178:	00858a13          	addi	s4,a1,8
2005e17c:	892a                	c.mv	s2,a0
2005e17e:	43dc                	c.lw	a5,4(a5)
2005e180:	842e                	c.mv	s0,a1
2005e182:	89b2                	c.mv	s3,a2
2005e184:	c1dc                	c.sw	a5,4(a1)
2005e186:	01479463          	bne	a5,s4,2005e18e <prvListTasksWithinSingleList+0x30>
2005e18a:	45dc                	c.lw	a5,12(a1)
2005e18c:	c1dc                	c.sw	a5,4(a1)
2005e18e:	405c                	c.lw	a5,4(s0)
2005e190:	4481                	c.li	s1,0
2005e192:	02400b93          	addi	s7,zero,36
2005e196:	00c7ab03          	lw	s6,12(a5)
2005e19a:	405c                	c.lw	a5,4(s0)
2005e19c:	43dc                	c.lw	a5,4(a5)
2005e19e:	c05c                	c.sw	a5,4(s0)
2005e1a0:	00fa1563          	bne	s4,a5,2005e1aa <prvListTasksWithinSingleList+0x4c>
2005e1a4:	004a2783          	lw	a5,4(s4)
2005e1a8:	c05c                	c.sw	a5,4(s0)
2005e1aa:	037485b3          	mul	a1,s1,s7
2005e1ae:	405c                	c.lw	a5,4(s0)
2005e1b0:	86ce                	c.mv	a3,s3
2005e1b2:	4605                	c.li	a2,1
2005e1b4:	00c7aa83          	lw	s5,12(a5)
2005e1b8:	0485                	c.addi	s1,1
2005e1ba:	8556                	c.mv	a0,s5
2005e1bc:	95ca                	c.add	a1,s2
2005e1be:	effff0ef          	jal	ra,2005e0bc <vTaskGetInfo>
2005e1c2:	fd5b1ce3          	bne	s6,s5,2005e19a <prvListTasksWithinSingleList+0x3c>
2005e1c6:	50b2                	c.lwsp	ra,44(sp)
2005e1c8:	5422                	c.lwsp	s0,40(sp)
2005e1ca:	5902                	c.lwsp	s2,32(sp)
2005e1cc:	49f2                	c.lwsp	s3,28(sp)
2005e1ce:	4a62                	c.lwsp	s4,24(sp)
2005e1d0:	4ad2                	c.lwsp	s5,20(sp)
2005e1d2:	4b42                	c.lwsp	s6,16(sp)
2005e1d4:	4bb2                	c.lwsp	s7,12(sp)
2005e1d6:	8526                	c.mv	a0,s1
2005e1d8:	5492                	c.lwsp	s1,36(sp)
2005e1da:	6145                	c.addi16sp	sp,48
2005e1dc:	8082                	c.jr	ra

2005e1de <uxTaskGetSystemState>:
2005e1de:	1101                	c.addi	sp,-32
2005e1e0:	cc22                	c.swsp	s0,24(sp)
2005e1e2:	c84a                	c.swsp	s2,16(sp)
2005e1e4:	c64e                	c.swsp	s3,12(sp)
2005e1e6:	ce06                	c.swsp	ra,28(sp)
2005e1e8:	ca26                	c.swsp	s1,20(sp)
2005e1ea:	c452                	c.swsp	s4,8(sp)
2005e1ec:	c256                	c.swsp	s5,4(sp)
2005e1ee:	c05a                	c.swsp	s6,0(sp)
2005e1f0:	892a                	c.mv	s2,a0
2005e1f2:	842e                	c.mv	s0,a1
2005e1f4:	89b2                	c.mv	s3,a2
2005e1f6:	c78ff0ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005e1fa:	f741a783          	lw	a5,-140(gp) # 20066818 <uxCurrentNumberOfTasks>
2005e1fe:	0af46563          	bltu	s0,a5,2005e2a8 <uxTaskGetSystemState+0xca>
2005e202:	20068ab7          	lui	s5,0x20068
2005e206:	4a2d                	c.li	s4,11
2005e208:	4401                	c.li	s0,0
2005e20a:	4b51                	c.li	s6,20
2005e20c:	1e8a8a93          	addi	s5,s5,488 # 200681e8 <pxReadyTasksLists>
2005e210:	02400493          	addi	s1,zero,36
2005e214:	1a7d                	c.addi	s4,-1
2005e216:	02940533          	mul	a0,s0,s1
2005e21a:	4605                	c.li	a2,1
2005e21c:	036a05b3          	mul	a1,s4,s6
2005e220:	954a                	c.add	a0,s2
2005e222:	95d6                	c.add	a1,s5
2005e224:	f3bff0ef          	jal	ra,2005e15e <prvListTasksWithinSingleList>
2005e228:	942a                	c.add	s0,a0
2005e22a:	fe0a15e3          	bne	s4,zero,2005e214 <uxTaskGetSystemState+0x36>
2005e22e:	02940533          	mul	a0,s0,s1
2005e232:	f641a583          	lw	a1,-156(gp) # 20066808 <pxDelayedTaskList>
2005e236:	4609                	c.li	a2,2
2005e238:	954a                	c.add	a0,s2
2005e23a:	f25ff0ef          	jal	ra,2005e15e <prvListTasksWithinSingleList>
2005e23e:	942a                	c.add	s0,a0
2005e240:	02940533          	mul	a0,s0,s1
2005e244:	f681a583          	lw	a1,-152(gp) # 2006680c <pxOverflowDelayedTaskList>
2005e248:	4609                	c.li	a2,2
2005e24a:	954a                	c.add	a0,s2
2005e24c:	f13ff0ef          	jal	ra,2005e15e <prvListTasksWithinSingleList>
2005e250:	942a                	c.add	s0,a0
2005e252:	02940533          	mul	a0,s0,s1
2005e256:	200685b7          	lui	a1,0x20068
2005e25a:	4611                	c.li	a2,4
2005e25c:	31458593          	addi	a1,a1,788 # 20068314 <xTasksWaitingTermination>
2005e260:	954a                	c.add	a0,s2
2005e262:	efdff0ef          	jal	ra,2005e15e <prvListTasksWithinSingleList>
2005e266:	942a                	c.add	s0,a0
2005e268:	029404b3          	mul	s1,s0,s1
2005e26c:	200685b7          	lui	a1,0x20068
2005e270:	460d                	c.li	a2,3
2005e272:	30058593          	addi	a1,a1,768 # 20068300 <xSuspendedTaskList>
2005e276:	00990533          	add	a0,s2,s1
2005e27a:	ee5ff0ef          	jal	ra,2005e15e <prvListTasksWithinSingleList>
2005e27e:	942a                	c.add	s0,a0
2005e280:	00098763          	beq	s3,zero,2005e28e <uxTaskGetSystemState+0xb0>
2005e284:	410187b7          	lui	a5,0x41018
2005e288:	4bdc                	c.lw	a5,20(a5)
2005e28a:	00f9a023          	sw	a5,0(s3)
2005e28e:	c8fff0ef          	jal	ra,2005df1c <xTaskResumeAll>
2005e292:	40f2                	c.lwsp	ra,28(sp)
2005e294:	8522                	c.mv	a0,s0
2005e296:	4462                	c.lwsp	s0,24(sp)
2005e298:	44d2                	c.lwsp	s1,20(sp)
2005e29a:	4942                	c.lwsp	s2,16(sp)
2005e29c:	49b2                	c.lwsp	s3,12(sp)
2005e29e:	4a22                	c.lwsp	s4,8(sp)
2005e2a0:	4a92                	c.lwsp	s5,4(sp)
2005e2a2:	4b02                	c.lwsp	s6,0(sp)
2005e2a4:	6105                	c.addi16sp	sp,32
2005e2a6:	8082                	c.jr	ra
2005e2a8:	4401                	c.li	s0,0
2005e2aa:	b7d5                	c.j	2005e28e <uxTaskGetSystemState+0xb0>

2005e2ac <xTaskGetTickCount>:
2005e2ac:	1101                	c.addi	sp,-32
2005e2ae:	ce06                	c.swsp	ra,28(sp)
2005e2b0:	8e5ff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005e2b4:	f981a503          	lw	a0,-104(gp) # 2006683c <xTickCount>
2005e2b8:	c62a                	c.swsp	a0,12(sp)
2005e2ba:	8f5ff0ef          	jal	ra,2005dbae <vTaskExitCritical>
2005e2be:	40f2                	c.lwsp	ra,28(sp)
2005e2c0:	4532                	c.lwsp	a0,12(sp)
2005e2c2:	6105                	c.addi16sp	sp,32
2005e2c4:	8082                	c.jr	ra

2005e2c6 <xTaskCheckForTimeOut>:
2005e2c6:	1101                	c.addi	sp,-32
2005e2c8:	cc22                	c.swsp	s0,24(sp)
2005e2ca:	ce06                	c.swsp	ra,28(sp)
2005e2cc:	842e                	c.mv	s0,a1
2005e2ce:	c62a                	c.swsp	a0,12(sp)
2005e2d0:	8c5ff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005e2d4:	f981a683          	lw	a3,-104(gp) # 2006683c <xTickCount>
2005e2d8:	401c                	c.lw	a5,0(s0)
2005e2da:	577d                	c.li	a4,-1
2005e2dc:	02e78f63          	beq	a5,a4,2005e31a <xTaskCheckForTimeOut+0x54>
2005e2e0:	4532                	c.lwsp	a0,12(sp)
2005e2e2:	f8c1a603          	lw	a2,-116(gp) # 20066830 <xNumOfOverflows>
2005e2e6:	410c                	c.lw	a1,0(a0)
2005e2e8:	4158                	c.lw	a4,4(a0)
2005e2ea:	00c58f63          	beq	a1,a2,2005e308 <xTaskCheckForTimeOut+0x42>
2005e2ee:	00e6ed63          	bltu	a3,a4,2005e308 <xTaskCheckForTimeOut+0x42>
2005e2f2:	00042023          	sw	zero,0(s0)
2005e2f6:	4505                	c.li	a0,1
2005e2f8:	c62a                	c.swsp	a0,12(sp)
2005e2fa:	8b5ff0ef          	jal	ra,2005dbae <vTaskExitCritical>
2005e2fe:	40f2                	c.lwsp	ra,28(sp)
2005e300:	4462                	c.lwsp	s0,24(sp)
2005e302:	4532                	c.lwsp	a0,12(sp)
2005e304:	6105                	c.addi16sp	sp,32
2005e306:	8082                	c.jr	ra
2005e308:	40e68633          	sub	a2,a3,a4
2005e30c:	fef673e3          	bgeu	a2,a5,2005e2f2 <xTaskCheckForTimeOut+0x2c>
2005e310:	8f95                	c.sub	a5,a3
2005e312:	97ba                	c.add	a5,a4
2005e314:	c01c                	c.sw	a5,0(s0)
2005e316:	e50ff0ef          	jal	ra,2005d966 <vTaskInternalSetTimeOutState>
2005e31a:	4501                	c.li	a0,0
2005e31c:	bff1                	c.j	2005e2f8 <xTaskCheckForTimeOut+0x32>

2005e31e <vTaskList>:
2005e31e:	7139                	c.addi16sp	sp,-64
2005e320:	dc22                	c.swsp	s0,56(sp)
2005e322:	de06                	c.swsp	ra,60(sp)
2005e324:	da26                	c.swsp	s1,52(sp)
2005e326:	d84a                	c.swsp	s2,48(sp)
2005e328:	d64e                	c.swsp	s3,44(sp)
2005e32a:	d452                	c.swsp	s4,40(sp)
2005e32c:	d256                	c.swsp	s5,36(sp)
2005e32e:	d05a                	c.swsp	s6,32(sp)
2005e330:	ce5e                	c.swsp	s7,28(sp)
2005e332:	f7418793          	addi	a5,gp,-140 # 20066818 <uxCurrentNumberOfTasks>
2005e336:	438c                	c.lw	a1,0(a5)
2005e338:	842a                	c.mv	s0,a0
2005e33a:	00050023          	sb	zero,0(a0)
2005e33e:	4388                	c.lw	a0,0(a5)
2005e340:	02400793          	addi	a5,zero,36
2005e344:	c62e                	c.swsp	a1,12(sp)
2005e346:	02f50533          	mul	a0,a0,a5
2005e34a:	e36fc0ef          	jal	ra,2005a980 <pvPortMalloc>
2005e34e:	c151                	c.beqz	a0,2005e3d2 <vTaskList+0xb4>
2005e350:	45b2                	c.lwsp	a1,12(sp)
2005e352:	4601                	c.li	a2,0
2005e354:	892a                	c.mv	s2,a0
2005e356:	e89ff0ef          	jal	ra,2005e1de <uxTaskGetSystemState>
2005e35a:	89aa                	c.mv	s3,a0
2005e35c:	00490493          	addi	s1,s2,4
2005e360:	4a01                	c.li	s4,0
2005e362:	4b91                	c.li	s7,4
2005e364:	e6418a93          	addi	s5,gp,-412 # 20066708 <CSWTCH.248>
2005e368:	20063b37          	lui	s6,0x20063
2005e36c:	013a1f63          	bne	s4,s3,2005e38a <vTaskList+0x6c>
2005e370:	5462                	c.lwsp	s0,56(sp)
2005e372:	50f2                	c.lwsp	ra,60(sp)
2005e374:	54d2                	c.lwsp	s1,52(sp)
2005e376:	59b2                	c.lwsp	s3,44(sp)
2005e378:	5a22                	c.lwsp	s4,40(sp)
2005e37a:	5a92                	c.lwsp	s5,36(sp)
2005e37c:	5b02                	c.lwsp	s6,32(sp)
2005e37e:	4bf2                	c.lwsp	s7,28(sp)
2005e380:	854a                	c.mv	a0,s2
2005e382:	5942                	c.lwsp	s2,48(sp)
2005e384:	6121                	c.addi16sp	sp,64
2005e386:	ed8fc06f          	jal	zero,2005aa5e <vPortFree>
2005e38a:	0084c783          	lbu	a5,8(s1)
2005e38e:	4681                	c.li	a3,0
2005e390:	00fbe563          	bltu	s7,a5,2005e39a <vTaskList+0x7c>
2005e394:	97d6                	c.add	a5,s5
2005e396:	0007c683          	lbu	a3,0(a5) # 41018000 <__km4_bd_boot_download_addr__+0x11006000>
2005e39a:	408c                	c.lw	a1,0(s1)
2005e39c:	8522                	c.mv	a0,s0
2005e39e:	c636                	c.swsp	a3,12(sp)
2005e3a0:	8e2ff0ef          	jal	ra,2005d482 <prvWriteNameToBuffer>
2005e3a4:	0044a803          	lw	a6,4(s1)
2005e3a8:	01c4d783          	lhu	a5,28(s1)
2005e3ac:	44d8                	c.lw	a4,12(s1)
2005e3ae:	46b2                	c.lwsp	a3,12(sp)
2005e3b0:	5c4b0613          	addi	a2,s6,1476 # 200635c4 <__func__.1+0x20>
2005e3b4:	40000593          	addi	a1,zero,1024
2005e3b8:	842a                	c.mv	s0,a0
2005e3ba:	124010ef          	jal	ra,2005f4de <DiagSnPrintf>
2005e3be:	8522                	c.mv	a0,s0
2005e3c0:	dffab097          	auipc	ra,0xdffab
2005e3c4:	718080e7          	jalr	ra,1816(ra) # 9ad8 <__wrap_strlen>
2005e3c8:	942a                	c.add	s0,a0
2005e3ca:	0a05                	c.addi	s4,1
2005e3cc:	02448493          	addi	s1,s1,36
2005e3d0:	bf71                	c.j	2005e36c <vTaskList+0x4e>
2005e3d2:	50f2                	c.lwsp	ra,60(sp)
2005e3d4:	5462                	c.lwsp	s0,56(sp)
2005e3d6:	54d2                	c.lwsp	s1,52(sp)
2005e3d8:	5942                	c.lwsp	s2,48(sp)
2005e3da:	59b2                	c.lwsp	s3,44(sp)
2005e3dc:	5a22                	c.lwsp	s4,40(sp)
2005e3de:	5a92                	c.lwsp	s5,36(sp)
2005e3e0:	5b02                	c.lwsp	s6,32(sp)
2005e3e2:	4bf2                	c.lwsp	s7,28(sp)
2005e3e4:	6121                	c.addi16sp	sp,64
2005e3e6:	8082                	c.jr	ra

2005e3e8 <pvTaskIncrementMutexHeldCount>:
2005e3e8:	f6018713          	addi	a4,gp,-160 # 20066804 <pxCurrentTCB>
2005e3ec:	4318                	c.lw	a4,0(a4)
2005e3ee:	f6018793          	addi	a5,gp,-160 # 20066804 <pxCurrentTCB>
2005e3f2:	c709                	c.beqz	a4,2005e3fc <pvTaskIncrementMutexHeldCount+0x14>
2005e3f4:	4394                	c.lw	a3,0(a5)
2005e3f6:	52b8                	c.lw	a4,96(a3)
2005e3f8:	0705                	c.addi	a4,1
2005e3fa:	d2b8                	c.sw	a4,96(a3)
2005e3fc:	4388                	c.lw	a0,0(a5)
2005e3fe:	8082                	c.jr	ra

2005e400 <prvCheckForValidListAndQueue>:
2005e400:	1141                	c.addi	sp,-16
2005e402:	c422                	c.swsp	s0,8(sp)
2005e404:	c606                	c.swsp	ra,12(sp)
2005e406:	c226                	c.swsp	s1,4(sp)
2005e408:	c04a                	c.swsp	s2,0(sp)
2005e40a:	fac18413          	addi	s0,gp,-84 # 20066850 <xTimerQueue>
2005e40e:	f86ff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005e412:	401c                	c.lw	a5,0(s0)
2005e414:	e3b9                	c.bnez	a5,2005e45a <prvCheckForValidListAndQueue+0x5a>
2005e416:	20068537          	lui	a0,0x20068
2005e41a:	3a050913          	addi	s2,a0,928 # 200683a0 <xActiveTimerList1>
2005e41e:	3a050513          	addi	a0,a0,928
2005e422:	f98fc0ef          	jal	ra,2005abba <vListInitialise>
2005e426:	20068537          	lui	a0,0x20068
2005e42a:	3b450493          	addi	s1,a0,948 # 200683b4 <xActiveTimerList2>
2005e42e:	3b450513          	addi	a0,a0,948
2005e432:	f88fc0ef          	jal	ra,2005abba <vListInitialise>
2005e436:	200686b7          	lui	a3,0x20068
2005e43a:	20068637          	lui	a2,0x20068
2005e43e:	fb21a023          	sw	s2,-96(gp) # 20066844 <pxCurrentTimerList>
2005e442:	4701                	c.li	a4,0
2005e444:	3c868693          	addi	a3,a3,968 # 200683c8 <xStaticTimerQueue.0>
2005e448:	32860613          	addi	a2,a2,808 # 20068328 <ucStaticTimerQueueStorage.1>
2005e44c:	45b1                	c.li	a1,12
2005e44e:	4529                	c.li	a0,10
2005e450:	fa91a223          	sw	s1,-92(gp) # 20066848 <pxOverflowTimerList>
2005e454:	9b7fe0ef          	jal	ra,2005ce0a <xQueueGenericCreateStatic>
2005e458:	c008                	c.sw	a0,0(s0)
2005e45a:	4422                	c.lwsp	s0,8(sp)
2005e45c:	40b2                	c.lwsp	ra,12(sp)
2005e45e:	4492                	c.lwsp	s1,4(sp)
2005e460:	4902                	c.lwsp	s2,0(sp)
2005e462:	0141                	c.addi	sp,16
2005e464:	f4aff06f          	jal	zero,2005dbae <vTaskExitCritical>

2005e468 <prvInsertTimerInActiveList>:
2005e468:	1141                	c.addi	sp,-16
2005e46a:	c606                	c.swsp	ra,12(sp)
2005e46c:	c14c                	c.sw	a1,4(a0)
2005e46e:	c908                	c.sw	a0,16(a0)
2005e470:	87aa                	c.mv	a5,a0
2005e472:	02b66163          	bltu	a2,a1,2005e494 <prvInsertTimerInActiveList+0x2c>
2005e476:	4d18                	c.lw	a4,24(a0)
2005e478:	8e15                	c.sub	a2,a3
2005e47a:	4505                	c.li	a0,1
2005e47c:	00e67963          	bgeu	a2,a4,2005e48e <prvInsertTimerInActiveList+0x26>
2005e480:	00478593          	addi	a1,a5,4
2005e484:	fa41a503          	lw	a0,-92(gp) # 20066848 <pxOverflowTimerList>
2005e488:	f62fc0ef          	jal	ra,2005abea <vListInsert>
2005e48c:	4501                	c.li	a0,0
2005e48e:	40b2                	c.lwsp	ra,12(sp)
2005e490:	0141                	c.addi	sp,16
2005e492:	8082                	c.jr	ra
2005e494:	00d67563          	bgeu	a2,a3,2005e49e <prvInsertTimerInActiveList+0x36>
2005e498:	4505                	c.li	a0,1
2005e49a:	fed5fae3          	bgeu	a1,a3,2005e48e <prvInsertTimerInActiveList+0x26>
2005e49e:	00478593          	addi	a1,a5,4
2005e4a2:	fa01a503          	lw	a0,-96(gp) # 20066844 <pxCurrentTimerList>
2005e4a6:	b7cd                	c.j	2005e488 <prvInsertTimerInActiveList+0x20>

2005e4a8 <xTimerCreateTimerTask>:
2005e4a8:	1101                	c.addi	sp,-32
2005e4aa:	ce06                	c.swsp	ra,28(sp)
2005e4ac:	f55ff0ef          	jal	ra,2005e400 <prvCheckForValidListAndQueue>
2005e4b0:	fac1a783          	lw	a5,-84(gp) # 20066850 <xTimerQueue>
2005e4b4:	4501                	c.li	a0,0
2005e4b6:	cb9d                	c.beqz	a5,2005e4ec <xTimerCreateTimerTask+0x44>
2005e4b8:	0070                	c.addi4spn	a2,sp,12
2005e4ba:	002c                	c.addi4spn	a1,sp,8
2005e4bc:	0048                	c.addi4spn	a0,sp,4
2005e4be:	c202                	c.swsp	zero,4(sp)
2005e4c0:	c402                	c.swsp	zero,8(sp)
2005e4c2:	962fe0ef          	jal	ra,2005c624 <vApplicationGetTimerTaskMemory>
2005e4c6:	47a2                	c.lwsp	a5,8(sp)
2005e4c8:	4812                	c.lwsp	a6,4(sp)
2005e4ca:	4632                	c.lwsp	a2,12(sp)
2005e4cc:	200635b7          	lui	a1,0x20063
2005e4d0:	2005e537          	lui	a0,0x2005e
2005e4d4:	4729                	c.li	a4,10
2005e4d6:	4681                	c.li	a3,0
2005e4d8:	5d458593          	addi	a1,a1,1492 # 200635d4 <__func__.1+0x30>
2005e4dc:	72e50513          	addi	a0,a0,1838 # 2005e72e <prvTimerTask>
2005e4e0:	ff8ff0ef          	jal	ra,2005dcd8 <xTaskCreateStatic>
2005e4e4:	faa1a823          	sw	a0,-80(gp) # 20066854 <xTimerTaskHandle>
2005e4e8:	00a03533          	sltu	a0,zero,a0
2005e4ec:	40f2                	c.lwsp	ra,28(sp)
2005e4ee:	6105                	c.addi16sp	sp,32
2005e4f0:	8082                	c.jr	ra

2005e4f2 <xTimerCreate>:
2005e4f2:	1101                	c.addi	sp,-32
2005e4f4:	c256                	c.swsp	s5,4(sp)
2005e4f6:	8aaa                	c.mv	s5,a0
2005e4f8:	02c00513          	addi	a0,zero,44
2005e4fc:	cc22                	c.swsp	s0,24(sp)
2005e4fe:	ca26                	c.swsp	s1,20(sp)
2005e500:	c84a                	c.swsp	s2,16(sp)
2005e502:	c64e                	c.swsp	s3,12(sp)
2005e504:	c452                	c.swsp	s4,8(sp)
2005e506:	ce06                	c.swsp	ra,28(sp)
2005e508:	8a2e                	c.mv	s4,a1
2005e50a:	84b2                	c.mv	s1,a2
2005e50c:	89b6                	c.mv	s3,a3
2005e50e:	893a                	c.mv	s2,a4
2005e510:	c70fc0ef          	jal	ra,2005a980 <pvPortMalloc>
2005e514:	842a                	c.mv	s0,a0
2005e516:	c905                	c.beqz	a0,2005e546 <xTimerCreate+0x54>
2005e518:	02050423          	sb	zero,40(a0)
2005e51c:	ee5ff0ef          	jal	ra,2005e400 <prvCheckForValidListAndQueue>
2005e520:	00440513          	addi	a0,s0,4
2005e524:	01542023          	sw	s5,0(s0)
2005e528:	01442c23          	sw	s4,24(s0)
2005e52c:	01342e23          	sw	s3,28(s0)
2005e530:	03242023          	sw	s2,32(s0)
2005e534:	e9afc0ef          	jal	ra,2005abce <vListInitialiseItem>
2005e538:	c499                	c.beqz	s1,2005e546 <xTimerCreate+0x54>
2005e53a:	02844783          	lbu	a5,40(s0)
2005e53e:	0047e793          	ori	a5,a5,4
2005e542:	02f40423          	sb	a5,40(s0)
2005e546:	40f2                	c.lwsp	ra,28(sp)
2005e548:	8522                	c.mv	a0,s0
2005e54a:	4462                	c.lwsp	s0,24(sp)
2005e54c:	44d2                	c.lwsp	s1,20(sp)
2005e54e:	4942                	c.lwsp	s2,16(sp)
2005e550:	49b2                	c.lwsp	s3,12(sp)
2005e552:	4a22                	c.lwsp	s4,8(sp)
2005e554:	4a92                	c.lwsp	s5,4(sp)
2005e556:	6105                	c.addi16sp	sp,32
2005e558:	8082                	c.jr	ra

2005e55a <xTimerCreateStatic>:
2005e55a:	1101                	c.addi	sp,-32
2005e55c:	cc22                	c.swsp	s0,24(sp)
2005e55e:	ce06                	c.swsp	ra,28(sp)
2005e560:	ca26                	c.swsp	s1,20(sp)
2005e562:	c84a                	c.swsp	s2,16(sp)
2005e564:	c64e                	c.swsp	s3,12(sp)
2005e566:	c452                	c.swsp	s4,8(sp)
2005e568:	c256                	c.swsp	s5,4(sp)
2005e56a:	843e                	c.mv	s0,a5
2005e56c:	cf95                	c.beqz	a5,2005e5a8 <xTimerCreateStatic+0x4e>
2005e56e:	4789                	c.li	a5,2
2005e570:	02f40423          	sb	a5,40(s0)
2005e574:	8aaa                	c.mv	s5,a0
2005e576:	8a2e                	c.mv	s4,a1
2005e578:	84b2                	c.mv	s1,a2
2005e57a:	89b6                	c.mv	s3,a3
2005e57c:	893a                	c.mv	s2,a4
2005e57e:	e83ff0ef          	jal	ra,2005e400 <prvCheckForValidListAndQueue>
2005e582:	00440513          	addi	a0,s0,4
2005e586:	01542023          	sw	s5,0(s0)
2005e58a:	01442c23          	sw	s4,24(s0)
2005e58e:	01342e23          	sw	s3,28(s0)
2005e592:	03242023          	sw	s2,32(s0)
2005e596:	e38fc0ef          	jal	ra,2005abce <vListInitialiseItem>
2005e59a:	c499                	c.beqz	s1,2005e5a8 <xTimerCreateStatic+0x4e>
2005e59c:	02844783          	lbu	a5,40(s0)
2005e5a0:	0047e793          	ori	a5,a5,4
2005e5a4:	02f40423          	sb	a5,40(s0)
2005e5a8:	40f2                	c.lwsp	ra,28(sp)
2005e5aa:	8522                	c.mv	a0,s0
2005e5ac:	4462                	c.lwsp	s0,24(sp)
2005e5ae:	44d2                	c.lwsp	s1,20(sp)
2005e5b0:	4942                	c.lwsp	s2,16(sp)
2005e5b2:	49b2                	c.lwsp	s3,12(sp)
2005e5b4:	4a22                	c.lwsp	s4,8(sp)
2005e5b6:	4a92                	c.lwsp	s5,4(sp)
2005e5b8:	6105                	c.addi16sp	sp,32
2005e5ba:	8082                	c.jr	ra

2005e5bc <xTimerGenericCommand>:
2005e5bc:	7139                	c.addi16sp	sp,-64
2005e5be:	dc22                	c.swsp	s0,56(sp)
2005e5c0:	da26                	c.swsp	s1,52(sp)
2005e5c2:	d84a                	c.swsp	s2,48(sp)
2005e5c4:	d452                	c.swsp	s4,40(sp)
2005e5c6:	d256                	c.swsp	s5,36(sp)
2005e5c8:	de06                	c.swsp	ra,60(sp)
2005e5ca:	d64e                	c.swsp	s3,44(sp)
2005e5cc:	842a                	c.mv	s0,a0
2005e5ce:	84ae                	c.mv	s1,a1
2005e5d0:	8932                	c.mv	s2,a2
2005e5d2:	8a36                	c.mv	s4,a3
2005e5d4:	8aba                	c.mv	s5,a4
2005e5d6:	bd6ff0ef          	jal	ra,2005d9ac <xTaskGetCurrentTaskHandle>
2005e5da:	fb01a783          	lw	a5,-80(gp) # 20066854 <xTimerTaskHandle>
2005e5de:	06a79163          	bne	a5,a0,2005e640 <xTimerGenericCommand+0x84>
2005e5e2:	ffd48793          	addi	a5,s1,-3
2005e5e6:	4705                	c.li	a4,1
2005e5e8:	04f76c63          	bltu	a4,a5,2005e640 <xTimerGenericCommand+0x84>
2005e5ec:	cc1ff0ef          	jal	ra,2005e2ac <xTaskGetTickCount>
2005e5f0:	485c                	c.lw	a5,20(s0)
2005e5f2:	86aa                	c.mv	a3,a0
2005e5f4:	c799                	c.beqz	a5,2005e602 <xTimerGenericCommand+0x46>
2005e5f6:	c62a                	c.swsp	a0,12(sp)
2005e5f8:	00440513          	addi	a0,s0,4
2005e5fc:	e1cfc0ef          	jal	ra,2005ac18 <uxListRemove>
2005e600:	46b2                	c.lwsp	a3,12(sp)
2005e602:	4711                	c.li	a4,4
2005e604:	02844783          	lbu	a5,40(s0)
2005e608:	00e48f63          	beq	s1,a4,2005e626 <xTimerGenericCommand+0x6a>
2005e60c:	9bf9                	c.andi	a5,-2
2005e60e:	02f40423          	sb	a5,40(s0)
2005e612:	4505                	c.li	a0,1
2005e614:	50f2                	c.lwsp	ra,60(sp)
2005e616:	5462                	c.lwsp	s0,56(sp)
2005e618:	54d2                	c.lwsp	s1,52(sp)
2005e61a:	5942                	c.lwsp	s2,48(sp)
2005e61c:	59b2                	c.lwsp	s3,44(sp)
2005e61e:	5a22                	c.lwsp	s4,40(sp)
2005e620:	5a92                	c.lwsp	s5,36(sp)
2005e622:	6121                	c.addi16sp	sp,64
2005e624:	8082                	c.jr	ra
2005e626:	0017e793          	ori	a5,a5,1
2005e62a:	02f40423          	sb	a5,40(s0)
2005e62e:	01242c23          	sw	s2,24(s0)
2005e632:	8636                	c.mv	a2,a3
2005e634:	00d905b3          	add	a1,s2,a3
2005e638:	8522                	c.mv	a0,s0
2005e63a:	e2fff0ef          	jal	ra,2005e468 <prvInsertTimerInActiveList>
2005e63e:	bfd1                	c.j	2005e612 <xTimerGenericCommand+0x56>
2005e640:	fac18993          	addi	s3,gp,-84 # 20066850 <xTimerQueue>
2005e644:	0009a783          	lw	a5,0(s3)
2005e648:	4501                	c.li	a0,0
2005e64a:	d7e9                	c.beqz	a5,2005e614 <xTimerGenericCommand+0x58>
2005e64c:	ca26                	c.swsp	s1,20(sp)
2005e64e:	cc4a                	c.swsp	s2,24(sp)
2005e650:	ce22                	c.swsp	s0,28(sp)
2005e652:	4715                	c.li	a4,5
2005e654:	02974163          	blt	a4,s1,2005e676 <xTimerGenericCommand+0xba>
2005e658:	b5aff0ef          	jal	ra,2005d9b2 <xTaskGetSchedulerState>
2005e65c:	872a                	c.mv	a4,a0
2005e65e:	4789                	c.li	a5,2
2005e660:	0009a503          	lw	a0,0(s3)
2005e664:	4681                	c.li	a3,0
2005e666:	8656                	c.mv	a2,s5
2005e668:	00f70363          	beq	a4,a5,2005e66e <xTimerGenericCommand+0xb2>
2005e66c:	4601                	c.li	a2,0
2005e66e:	084c                	c.addi4spn	a1,sp,20
2005e670:	a18fe0ef          	jal	ra,2005c888 <xQueueGenericSend>
2005e674:	b745                	c.j	2005e614 <xTimerGenericCommand+0x58>
2005e676:	4681                	c.li	a3,0
2005e678:	8652                	c.mv	a2,s4
2005e67a:	084c                	c.addi4spn	a1,sp,20
2005e67c:	853e                	c.mv	a0,a5
2005e67e:	b0efe0ef          	jal	ra,2005c98c <xQueueGenericSendFromISR>
2005e682:	bf49                	c.j	2005e614 <xTimerGenericCommand+0x58>

2005e684 <prvSampleTimeNow>:
2005e684:	7179                	c.addi16sp	sp,-48
2005e686:	d226                	c.swsp	s1,36(sp)
2005e688:	d04a                	c.swsp	s2,32(sp)
2005e68a:	ce4e                	c.swsp	s3,28(sp)
2005e68c:	d606                	c.swsp	ra,44(sp)
2005e68e:	d422                	c.swsp	s0,40(sp)
2005e690:	cc52                	c.swsp	s4,24(sp)
2005e692:	ca56                	c.swsp	s5,20(sp)
2005e694:	89aa                	c.mv	s3,a0
2005e696:	c17ff0ef          	jal	ra,2005e2ac <xTaskGetTickCount>
2005e69a:	fa81a783          	lw	a5,-88(gp) # 2006684c <xLastTime.2>
2005e69e:	84aa                	c.mv	s1,a0
2005e6a0:	fa818913          	addi	s2,gp,-88 # 2006684c <xLastTime.2>
2005e6a4:	08f57263          	bgeu	a0,a5,2005e728 <prvSampleTimeNow+0xa4>
2005e6a8:	fa018a13          	addi	s4,gp,-96 # 20066844 <pxCurrentTimerList>
2005e6ac:	000a2703          	lw	a4,0(s4)
2005e6b0:	431c                	c.lw	a5,0(a4)
2005e6b2:	e795                	c.bnez	a5,2005e6de <prvSampleTimeNow+0x5a>
2005e6b4:	fa418793          	addi	a5,gp,-92 # 20066848 <pxOverflowTimerList>
2005e6b8:	4394                	c.lw	a3,0(a5)
2005e6ba:	c398                	c.sw	a4,0(a5)
2005e6bc:	4785                	c.li	a5,1
2005e6be:	00da2023          	sw	a3,0(s4)
2005e6c2:	00f9a023          	sw	a5,0(s3)
2005e6c6:	50b2                	c.lwsp	ra,44(sp)
2005e6c8:	5422                	c.lwsp	s0,40(sp)
2005e6ca:	00992023          	sw	s1,0(s2)
2005e6ce:	49f2                	c.lwsp	s3,28(sp)
2005e6d0:	5902                	c.lwsp	s2,32(sp)
2005e6d2:	4a62                	c.lwsp	s4,24(sp)
2005e6d4:	4ad2                	c.lwsp	s5,20(sp)
2005e6d6:	8526                	c.mv	a0,s1
2005e6d8:	5492                	c.lwsp	s1,36(sp)
2005e6da:	6145                	c.addi16sp	sp,48
2005e6dc:	8082                	c.jr	ra
2005e6de:	475c                	c.lw	a5,12(a4)
2005e6e0:	47c0                	c.lw	s0,12(a5)
2005e6e2:	4390                	c.lw	a2,0(a5)
2005e6e4:	00440593          	addi	a1,s0,4
2005e6e8:	852e                	c.mv	a0,a1
2005e6ea:	c632                	c.swsp	a2,12(sp)
2005e6ec:	c42e                	c.swsp	a1,8(sp)
2005e6ee:	d2afc0ef          	jal	ra,2005ac18 <uxListRemove>
2005e6f2:	501c                	c.lw	a5,32(s0)
2005e6f4:	8522                	c.mv	a0,s0
2005e6f6:	9782                	c.jalr	a5
2005e6f8:	02844783          	lbu	a5,40(s0)
2005e6fc:	45a2                	c.lwsp	a1,8(sp)
2005e6fe:	4632                	c.lwsp	a2,12(sp)
2005e700:	8b91                	c.andi	a5,4
2005e702:	d3dd                	c.beqz	a5,2005e6a8 <prvSampleTimeNow+0x24>
2005e704:	4c1c                	c.lw	a5,24(s0)
2005e706:	97b2                	c.add	a5,a2
2005e708:	00f67963          	bgeu	a2,a5,2005e71a <prvSampleTimeNow+0x96>
2005e70c:	000a2503          	lw	a0,0(s4)
2005e710:	c05c                	c.sw	a5,4(s0)
2005e712:	c800                	c.sw	s0,16(s0)
2005e714:	cd6fc0ef          	jal	ra,2005abea <vListInsert>
2005e718:	bf41                	c.j	2005e6a8 <prvSampleTimeNow+0x24>
2005e71a:	4701                	c.li	a4,0
2005e71c:	4681                	c.li	a3,0
2005e71e:	4581                	c.li	a1,0
2005e720:	8522                	c.mv	a0,s0
2005e722:	e9bff0ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005e726:	b749                	c.j	2005e6a8 <prvSampleTimeNow+0x24>
2005e728:	0009a023          	sw	zero,0(s3)
2005e72c:	bf69                	c.j	2005e6c6 <prvSampleTimeNow+0x42>

2005e72e <prvTimerTask>:
2005e72e:	7139                	c.addi16sp	sp,-64
2005e730:	d84a                	c.swsp	s2,48(sp)
2005e732:	20063937          	lui	s2,0x20063
2005e736:	d64e                	c.swsp	s3,44(sp)
2005e738:	d256                	c.swsp	s5,36(sp)
2005e73a:	d05a                	c.swsp	s6,32(sp)
2005e73c:	de06                	c.swsp	ra,60(sp)
2005e73e:	dc22                	c.swsp	s0,56(sp)
2005e740:	da26                	c.swsp	s1,52(sp)
2005e742:	d452                	c.swsp	s4,40(sp)
2005e744:	ce5e                	c.swsp	s7,28(sp)
2005e746:	cc62                	c.swsp	s8,24(sp)
2005e748:	5dc90913          	addi	s2,s2,1500 # 200635dc <__func__.1+0x38>
2005e74c:	fa01a783          	lw	a5,-96(gp) # 20066844 <pxCurrentTimerList>
2005e750:	fa018c13          	addi	s8,gp,-96 # 20066844 <pxCurrentTimerList>
2005e754:	4405                	c.li	s0,1
2005e756:	4384                	c.lw	s1,0(a5)
2005e758:	c481                	c.beqz	s1,2005e760 <prvTimerTask+0x32>
2005e75a:	47dc                	c.lw	a5,12(a5)
2005e75c:	4401                	c.li	s0,0
2005e75e:	4384                	c.lw	s1,0(a5)
2005e760:	f0ffe0ef          	jal	ra,2005d66e <vTaskSuspendAll>
2005e764:	0048                	c.addi4spn	a0,sp,4
2005e766:	f1fff0ef          	jal	ra,2005e684 <prvSampleTimeNow>
2005e76a:	4792                	c.lwsp	a5,4(sp)
2005e76c:	8baa                	c.mv	s7,a0
2005e76e:	fac18a13          	addi	s4,gp,-84 # 20066850 <xTimerQueue>
2005e772:	ebc5                	c.bnez	a5,2005e822 <prvTimerTask+0xf4>
2005e774:	e449                	c.bnez	s0,2005e7fe <prvTimerTask+0xd0>
2005e776:	08956963          	bltu	a0,s1,2005e808 <prvTimerTask+0xda>
2005e77a:	fa2ff0ef          	jal	ra,2005df1c <xTaskResumeAll>
2005e77e:	000c2783          	lw	a5,0(s8)
2005e782:	47dc                	c.lw	a5,12(a5)
2005e784:	47c0                	c.lw	s0,12(a5)
2005e786:	00440513          	addi	a0,s0,4
2005e78a:	c8efc0ef          	jal	ra,2005ac18 <uxListRemove>
2005e78e:	02844783          	lbu	a5,40(s0)
2005e792:	0047f713          	andi	a4,a5,4
2005e796:	c325                	c.beqz	a4,2005e7f6 <prvTimerTask+0xc8>
2005e798:	4c0c                	c.lw	a1,24(s0)
2005e79a:	86a6                	c.mv	a3,s1
2005e79c:	865e                	c.mv	a2,s7
2005e79e:	95a6                	c.add	a1,s1
2005e7a0:	8522                	c.mv	a0,s0
2005e7a2:	cc7ff0ef          	jal	ra,2005e468 <prvInsertTimerInActiveList>
2005e7a6:	c901                	c.beqz	a0,2005e7b6 <prvTimerTask+0x88>
2005e7a8:	4701                	c.li	a4,0
2005e7aa:	4681                	c.li	a3,0
2005e7ac:	8626                	c.mv	a2,s1
2005e7ae:	4581                	c.li	a1,0
2005e7b0:	8522                	c.mv	a0,s0
2005e7b2:	e0bff0ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005e7b6:	501c                	c.lw	a5,32(s0)
2005e7b8:	8522                	c.mv	a0,s0
2005e7ba:	9782                	c.jalr	a5
2005e7bc:	44a5                	c.li	s1,9
2005e7be:	000a2503          	lw	a0,0(s4)
2005e7c2:	4601                	c.li	a2,0
2005e7c4:	004c                	c.addi4spn	a1,sp,4
2005e7c6:	a84fe0ef          	jal	ra,2005ca4a <xQueueReceive>
2005e7ca:	d149                	c.beqz	a0,2005e74c <prvTimerTask+0x1e>
2005e7cc:	4792                	c.lwsp	a5,4(sp)
2005e7ce:	fe07c8e3          	blt	a5,zero,2005e7be <prvTimerTask+0x90>
2005e7d2:	4432                	c.lwsp	s0,12(sp)
2005e7d4:	485c                	c.lw	a5,20(s0)
2005e7d6:	c789                	c.beqz	a5,2005e7e0 <prvTimerTask+0xb2>
2005e7d8:	00440513          	addi	a0,s0,4
2005e7dc:	c3cfc0ef          	jal	ra,2005ac18 <uxListRemove>
2005e7e0:	850a                	c.mv	a0,sp
2005e7e2:	ea3ff0ef          	jal	ra,2005e684 <prvSampleTimeNow>
2005e7e6:	4792                	c.lwsp	a5,4(sp)
2005e7e8:	862a                	c.mv	a2,a0
2005e7ea:	fcf4eae3          	bltu	s1,a5,2005e7be <prvTimerTask+0x90>
2005e7ee:	078a                	c.slli	a5,0x2
2005e7f0:	97ca                	c.add	a5,s2
2005e7f2:	439c                	c.lw	a5,0(a5)
2005e7f4:	8782                	c.jr	a5
2005e7f6:	9bf9                	c.andi	a5,-2
2005e7f8:	02f40423          	sb	a5,40(s0)
2005e7fc:	bf6d                	c.j	2005e7b6 <prvTimerTask+0x88>
2005e7fe:	fa41a783          	lw	a5,-92(gp) # 20066848 <pxOverflowTimerList>
2005e802:	4380                	c.lw	s0,0(a5)
2005e804:	00143413          	sltiu	s0,s0,1
2005e808:	000a2503          	lw	a0,0(s4)
2005e80c:	8622                	c.mv	a2,s0
2005e80e:	417485b3          	sub	a1,s1,s7
2005e812:	cb0fe0ef          	jal	ra,2005ccc2 <vQueueWaitForMessageRestricted>
2005e816:	f06ff0ef          	jal	ra,2005df1c <xTaskResumeAll>
2005e81a:	f14d                	c.bnez	a0,2005e7bc <prvTimerTask+0x8e>
2005e81c:	00000073          	ecall
2005e820:	bf71                	c.j	2005e7bc <prvTimerTask+0x8e>
2005e822:	efaff0ef          	jal	ra,2005df1c <xTaskResumeAll>
2005e826:	bf59                	c.j	2005e7bc <prvTimerTask+0x8e>
2005e828:	02844783          	lbu	a5,40(s0)
2005e82c:	4c0c                	c.lw	a1,24(s0)
2005e82e:	8522                	c.mv	a0,s0
2005e830:	0017e793          	ori	a5,a5,1
2005e834:	02f40423          	sb	a5,40(s0)
2005e838:	46a2                	c.lwsp	a3,8(sp)
2005e83a:	95b6                	c.add	a1,a3
2005e83c:	c2dff0ef          	jal	ra,2005e468 <prvInsertTimerInActiveList>
2005e840:	dd3d                	c.beqz	a0,2005e7be <prvTimerTask+0x90>
2005e842:	501c                	c.lw	a5,32(s0)
2005e844:	8522                	c.mv	a0,s0
2005e846:	9782                	c.jalr	a5
2005e848:	02844783          	lbu	a5,40(s0)
2005e84c:	8b91                	c.andi	a5,4
2005e84e:	dba5                	c.beqz	a5,2005e7be <prvTimerTask+0x90>
2005e850:	4c1c                	c.lw	a5,24(s0)
2005e852:	4622                	c.lwsp	a2,8(sp)
2005e854:	4701                	c.li	a4,0
2005e856:	4681                	c.li	a3,0
2005e858:	963e                	c.add	a2,a5
2005e85a:	4581                	c.li	a1,0
2005e85c:	8522                	c.mv	a0,s0
2005e85e:	d5fff0ef          	jal	ra,2005e5bc <xTimerGenericCommand>
2005e862:	bfb1                	c.j	2005e7be <prvTimerTask+0x90>
2005e864:	02844783          	lbu	a5,40(s0)
2005e868:	9bf9                	c.andi	a5,-2
2005e86a:	02f40423          	sb	a5,40(s0)
2005e86e:	bf81                	c.j	2005e7be <prvTimerTask+0x90>
2005e870:	02844783          	lbu	a5,40(s0)
2005e874:	86aa                	c.mv	a3,a0
2005e876:	0017e793          	ori	a5,a5,1
2005e87a:	02f40423          	sb	a5,40(s0)
2005e87e:	45a2                	c.lwsp	a1,8(sp)
2005e880:	cc0c                	c.sw	a1,24(s0)
2005e882:	95aa                	c.add	a1,a0
2005e884:	8522                	c.mv	a0,s0
2005e886:	be3ff0ef          	jal	ra,2005e468 <prvInsertTimerInActiveList>
2005e88a:	bf15                	c.j	2005e7be <prvTimerTask+0x90>
2005e88c:	02844783          	lbu	a5,40(s0)
2005e890:	0027f713          	andi	a4,a5,2
2005e894:	fb71                	c.bnez	a4,2005e868 <prvTimerTask+0x13a>
2005e896:	8522                	c.mv	a0,s0
2005e898:	9c6fc0ef          	jal	ra,2005aa5e <vPortFree>
2005e89c:	b70d                	c.j	2005e7be <prvTimerTask+0x90>

2005e89e <xTimerIsTimerActive>:
2005e89e:	1101                	c.addi	sp,-32
2005e8a0:	ce06                	c.swsp	ra,28(sp)
2005e8a2:	cc22                	c.swsp	s0,24(sp)
2005e8a4:	842a                	c.mv	s0,a0
2005e8a6:	aeeff0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005e8aa:	02844503          	lbu	a0,40(s0)
2005e8ae:	8905                	c.andi	a0,1
2005e8b0:	c62a                	c.swsp	a0,12(sp)
2005e8b2:	afcff0ef          	jal	ra,2005dbae <vTaskExitCritical>
2005e8b6:	40f2                	c.lwsp	ra,28(sp)
2005e8b8:	4462                	c.lwsp	s0,24(sp)
2005e8ba:	4532                	c.lwsp	a0,12(sp)
2005e8bc:	6105                	c.addi16sp	sp,32
2005e8be:	8082                	c.jr	ra

2005e8c0 <wifi_fast_connect_enable>:
2005e8c0:	fa01ae23          	sw	zero,-68(gp) # 20066860 <p_wifi_do_fast_connect>
2005e8c4:	fa01ac23          	sw	zero,-72(gp) # 2006685c <p_store_fast_connect_info>
2005e8c8:	8082                	c.jr	ra

2005e8ca <rtw_indicate_event_handle>:
2005e8ca:	4759                	c.li	a4,22
2005e8cc:	04a74e63          	blt	a4,a0,2005e928 <rtw_indicate_event_handle+0x5e>
2005e8d0:	1101                	c.addi	sp,-32
2005e8d2:	20068737          	lui	a4,0x20068
2005e8d6:	ca26                	c.swsp	s1,20(sp)
2005e8d8:	c84a                	c.swsp	s2,16(sp)
2005e8da:	00451493          	slli	s1,a0,0x4
2005e8de:	892e                	c.mv	s2,a1
2005e8e0:	85b2                	c.mv	a1,a2
2005e8e2:	8636                	c.mv	a2,a3
2005e8e4:	41870693          	addi	a3,a4,1048 # 20068418 <event_callback_list>
2005e8e8:	cc22                	c.swsp	s0,24(sp)
2005e8ea:	ce06                	c.swsp	ra,28(sp)
2005e8ec:	009687b3          	add	a5,a3,s1
2005e8f0:	0007a803          	lw	a6,0(a5)
2005e8f4:	41870413          	addi	s0,a4,1048
2005e8f8:	00080963          	beq	a6,zero,2005e90a <rtw_indicate_event_handle+0x40>
2005e8fc:	43d4                	c.lw	a3,4(a5)
2005e8fe:	c632                	c.swsp	a2,12(sp)
2005e900:	c42e                	c.swsp	a1,8(sp)
2005e902:	854a                	c.mv	a0,s2
2005e904:	9802                	c.jalr	a6
2005e906:	4632                	c.lwsp	a2,12(sp)
2005e908:	45a2                	c.lwsp	a1,8(sp)
2005e90a:	009407b3          	add	a5,s0,s1
2005e90e:	4798                	c.lw	a4,8(a5)
2005e910:	4501                	c.li	a0,0
2005e912:	c709                	c.beqz	a4,2005e91c <rtw_indicate_event_handle+0x52>
2005e914:	47d4                	c.lw	a3,12(a5)
2005e916:	854a                	c.mv	a0,s2
2005e918:	9702                	c.jalr	a4
2005e91a:	4501                	c.li	a0,0
2005e91c:	40f2                	c.lwsp	ra,28(sp)
2005e91e:	4462                	c.lwsp	s0,24(sp)
2005e920:	44d2                	c.lwsp	s1,20(sp)
2005e922:	4942                	c.lwsp	s2,16(sp)
2005e924:	6105                	c.addi16sp	sp,32
2005e926:	8082                	c.jr	ra
2005e928:	5579                	c.li	a0,-2
2005e92a:	8082                	c.jr	ra

2005e92c <wifi_indication>:
2005e92c:	472d                	c.li	a4,11
2005e92e:	00e51863          	bne	a0,a4,2005e93e <wifi_indication+0x12>
2005e932:	fc01a703          	lw	a4,-64(gp) # 20066864 <p_wifi_joinstatus_internal_callback>
2005e936:	c711                	c.beqz	a4,2005e942 <wifi_indication+0x16>
2005e938:	0ff6f513          	andi	a0,a3,255
2005e93c:	8702                	c.jr	a4
2005e93e:	f8dff06f          	jal	zero,2005e8ca <rtw_indicate_event_handle>
2005e942:	8082                	c.jr	ra

2005e944 <_init_thread>:
2005e944:	1141                	c.addi	sp,-16
2005e946:	c422                	c.swsp	s0,8(sp)
2005e948:	c606                	c.swsp	ra,12(sp)
2005e94a:	4100c437          	lui	s0,0x4100c
2005e94e:	08442783          	lw	a5,132(s0) # 4100c084 <__km4_bd_boot_download_addr__+0x10ffa084>
2005e952:	08440713          	addi	a4,s0,132
2005e956:	8b85                	c.andi	a5,1
2005e958:	c7b1                	c.beqz	a5,2005e9a4 <_init_thread+0x60>
2005e95a:	431c                	c.lw	a5,0(a4)
2005e95c:	4505                	c.li	a0,1
2005e95e:	9bf9                	c.andi	a5,-2
2005e960:	c31c                	c.sw	a5,0(a4)
2005e962:	17d010ef          	jal	ra,200602de <wifi_on>
2005e966:	4505                	c.li	a0,1
2005e968:	209010ef          	jal	ra,20060370 <wifi_config_autoreconnect>
2005e96c:	d6bfc0ef          	jal	ra,2005b6d6 <rtos_mem_get_free_heap_size>
2005e970:	20063737          	lui	a4,0x20063
2005e974:	200636b7          	lui	a3,0x20063
2005e978:	200635b7          	lui	a1,0x20063
2005e97c:	882a                	c.mv	a6,a0
2005e97e:	03500793          	addi	a5,zero,53
2005e982:	4511                	c.li	a0,4
2005e984:	64870713          	addi	a4,a4,1608 # 20063648 <__FUNCTION__.0>
2005e988:	60468693          	addi	a3,a3,1540 # 20063604 <__func__.1+0x60>
2005e98c:	04900613          	addi	a2,zero,73
2005e990:	62058593          	addi	a1,a1,1568 # 20063620 <__func__.1+0x7c>
2005e994:	d58fc0ef          	jal	ra,2005aeec <rtk_log_write>
2005e998:	4422                	c.lwsp	s0,8(sp)
2005e99a:	40b2                	c.lwsp	ra,12(sp)
2005e99c:	4501                	c.li	a0,0
2005e99e:	0141                	c.addi	sp,16
2005e9a0:	9f6fd06f          	jal	zero,2005bb96 <rtos_task_delete>
2005e9a4:	4505                	c.li	a0,1
2005e9a6:	a02fd0ef          	jal	ra,2005bba8 <rtos_time_delay_ms>
2005e9aa:	b755                	c.j	2005e94e <_init_thread+0xa>

2005e9ac <wlan_initialize>:
2005e9ac:	1141                	c.addi	sp,-16
2005e9ae:	c606                	c.swsp	ra,12(sp)
2005e9b0:	041010ef          	jal	ra,200601f0 <wifi_set_rom2flash>
2005e9b4:	1be020ef          	jal	ra,20060b72 <inic_host_init>
2005e9b8:	4505                	c.li	a0,1
2005e9ba:	f07ff0ef          	jal	ra,2005e8c0 <wifi_fast_connect_enable>
2005e9be:	6705                	c.lui	a4,0x1
2005e9c0:	2005f637          	lui	a2,0x2005f
2005e9c4:	200635b7          	lui	a1,0x20063
2005e9c8:	4789                	c.li	a5,2
2005e9ca:	40070713          	addi	a4,a4,1024 # 1400 <CPU_ClkGet+0x8ea>
2005e9ce:	4681                	c.li	a3,0
2005e9d0:	94460613          	addi	a2,a2,-1724 # 2005e944 <_init_thread>
2005e9d4:	62858593          	addi	a1,a1,1576 # 20063628 <__func__.1+0x84>
2005e9d8:	4501                	c.li	a0,0
2005e9da:	998fd0ef          	jal	ra,2005bb72 <rtos_task_create>
2005e9de:	c105                	c.beqz	a0,2005e9fe <wlan_initialize+0x52>
2005e9e0:	40b2                	c.lwsp	ra,12(sp)
2005e9e2:	200636b7          	lui	a3,0x20063
2005e9e6:	200635b7          	lui	a1,0x20063
2005e9ea:	63068693          	addi	a3,a3,1584 # 20063630 <__func__.1+0x8c>
2005e9ee:	04500613          	addi	a2,zero,69
2005e9f2:	62058593          	addi	a1,a1,1568 # 20063620 <__func__.1+0x7c>
2005e9f6:	4509                	c.li	a0,2
2005e9f8:	0141                	c.addi	sp,16
2005e9fa:	cf2fc06f          	jal	zero,2005aeec <rtk_log_write>
2005e9fe:	40b2                	c.lwsp	ra,12(sp)
2005ea00:	0141                	c.addi	sp,16
2005ea02:	8082                	c.jr	ra

2005ea04 <ChipInfo_GetSocName_ToBuf>:
2005ea04:	7139                	c.addi16sp	sp,-64
2005ea06:	da26                	c.swsp	s1,52(sp)
2005ea08:	84ae                	c.mv	s1,a1
2005ea0a:	200635b7          	lui	a1,0x20063
2005ea0e:	d84a                	c.swsp	s2,48(sp)
2005ea10:	4625                	c.li	a2,9
2005ea12:	892a                	c.mv	s2,a0
2005ea14:	75758593          	addi	a1,a1,1879 # 20063757 <__FUNCTION__.0+0x10f>
2005ea18:	0068                	c.addi4spn	a0,sp,12
2005ea1a:	dc22                	c.swsp	s0,56(sp)
2005ea1c:	de06                	c.swsp	ra,60(sp)
2005ea1e:	d64e                	c.swsp	s3,44(sp)
2005ea20:	d452                	c.swsp	s4,40(sp)
2005ea22:	dffab097          	auipc	ra,0xdffab
2005ea26:	0ae080e7          	jalr	ra,174(ra) # 9ad0 <__wrap_memcpy>
2005ea2a:	00010aa3          	sb	zero,21(sp)
2005ea2e:	4401                	c.li	s0,0
2005ea30:	c899                	c.beqz	s1,2005ea46 <ChipInfo_GetSocName_ToBuf+0x42>
2005ea32:	20063637          	lui	a2,0x20063
2005ea36:	0074                	c.addi4spn	a3,sp,12
2005ea38:	78860613          	addi	a2,a2,1928 # 20063788 <__FUNCTION__.0+0x140>
2005ea3c:	85a6                	c.mv	a1,s1
2005ea3e:	854a                	c.mv	a0,s2
2005ea40:	29f000ef          	jal	ra,2005f4de <DiagSnPrintf>
2005ea44:	842a                	c.mv	s0,a0
2005ea46:	7f000993          	addi	s3,zero,2032
2005ea4a:	20063a37          	lui	s4,0x20063
2005ea4e:	00b10593          	addi	a1,sp,11
2005ea52:	854e                	c.mv	a0,s3
2005ea54:	a49fa0ef          	jal	ra,2005949c <OTP_Read8>
2005ea58:	e131                	c.bnez	a0,2005ea9c <ChipInfo_GetSocName_ToBuf+0x98>
2005ea5a:	200636b7          	lui	a3,0x20063
2005ea5e:	200635b7          	lui	a1,0x20063
2005ea62:	79068693          	addi	a3,a3,1936 # 20063790 <__FUNCTION__.0+0x148>
2005ea66:	04500613          	addi	a2,zero,69
2005ea6a:	69c58593          	addi	a1,a1,1692 # 2006369c <__FUNCTION__.0+0x54>
2005ea6e:	4509                	c.li	a0,2
2005ea70:	c7cfc0ef          	jal	ra,2005aeec <rtk_log_write>
2005ea74:	00947c63          	bgeu	s0,s1,2005ea8c <ChipInfo_GetSocName_ToBuf+0x88>
2005ea78:	20063637          	lui	a2,0x20063
2005ea7c:	03060613          	addi	a2,a2,48 # 20063030 <__FUNCTION__.0+0x20c>
2005ea80:	408485b3          	sub	a1,s1,s0
2005ea84:	00890533          	add	a0,s2,s0
2005ea88:	257000ef          	jal	ra,2005f4de <DiagSnPrintf>
2005ea8c:	50f2                	c.lwsp	ra,60(sp)
2005ea8e:	5462                	c.lwsp	s0,56(sp)
2005ea90:	54d2                	c.lwsp	s1,52(sp)
2005ea92:	5942                	c.lwsp	s2,48(sp)
2005ea94:	59b2                	c.lwsp	s3,44(sp)
2005ea96:	5a22                	c.lwsp	s4,40(sp)
2005ea98:	6121                	c.addi16sp	sp,64
2005ea9a:	8082                	c.jr	ra
2005ea9c:	00947d63          	bgeu	s0,s1,2005eab6 <ChipInfo_GetSocName_ToBuf+0xb2>
2005eaa0:	00b14683          	lbu	a3,11(sp)
2005eaa4:	408485b3          	sub	a1,s1,s0
2005eaa8:	00890533          	add	a0,s2,s0
2005eaac:	7a8a0613          	addi	a2,s4,1960 # 200637a8 <__FUNCTION__.0+0x160>
2005eab0:	22f000ef          	jal	ra,2005f4de <DiagSnPrintf>
2005eab4:	942a                	c.add	s0,a0
2005eab6:	0985                	c.addi	s3,1
2005eab8:	80098793          	addi	a5,s3,-2048
2005eabc:	fbc9                	c.bnez	a5,2005ea4e <ChipInfo_GetSocName_ToBuf+0x4a>
2005eabe:	bf5d                	c.j	2005ea74 <ChipInfo_GetSocName_ToBuf+0x70>

2005eac0 <ChipInfo_GetLibVersion_ToBuf>:
2005eac0:	1101                	c.addi	sp,-32
2005eac2:	cc22                	c.swsp	s0,24(sp)
2005eac4:	ca26                	c.swsp	s1,20(sp)
2005eac6:	c64e                	c.swsp	s3,12(sp)
2005eac8:	ce06                	c.swsp	ra,28(sp)
2005eaca:	c84a                	c.swsp	s2,16(sp)
2005eacc:	c452                	c.swsp	s4,8(sp)
2005eace:	c256                	c.swsp	s5,4(sp)
2005ead0:	c05a                	c.swsp	s6,0(sp)
2005ead2:	4100c7b7          	lui	a5,0x4100c
2005ead6:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005eada:	89aa                	c.mv	s3,a0
2005eadc:	84ae                	c.mv	s1,a1
2005eade:	4401                	c.li	s0,0
2005eae0:	c1b5                	c.beqz	a1,2005eb44 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005eae2:	8399                	c.srli	a5,0x6
2005eae4:	8b8d                	c.andi	a5,3
2005eae6:	e7d9                	c.bnez	a5,2005eb74 <ChipInfo_GetLibVersion_ToBuf+0xb4>
2005eae8:	200636b7          	lui	a3,0x20063
2005eaec:	40468693          	addi	a3,a3,1028 # 20063404 <__FUNCTION__.0+0x1c>
2005eaf0:	20063637          	lui	a2,0x20063
2005eaf4:	7b460613          	addi	a2,a2,1972 # 200637b4 <__FUNCTION__.0+0x16c>
2005eaf8:	85a6                	c.mv	a1,s1
2005eafa:	854e                	c.mv	a0,s3
2005eafc:	1e3000ef          	jal	ra,2005f4de <DiagSnPrintf>
2005eb00:	842a                	c.mv	s0,a0
2005eb02:	04957163          	bgeu	a0,s1,2005eb44 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005eb06:	200636b7          	lui	a3,0x20063
2005eb0a:	20063637          	lui	a2,0x20063
2005eb0e:	40a485b3          	sub	a1,s1,a0
2005eb12:	7c868693          	addi	a3,a3,1992 # 200637c8 <__FUNCTION__.0+0x180>
2005eb16:	7dc60613          	addi	a2,a2,2012 # 200637dc <__FUNCTION__.0+0x194>
2005eb1a:	954e                	c.add	a0,s3
2005eb1c:	1c3000ef          	jal	ra,2005f4de <DiagSnPrintf>
2005eb20:	942a                	c.add	s0,a0
2005eb22:	02947163          	bgeu	s0,s1,2005eb44 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005eb26:	200636b7          	lui	a3,0x20063
2005eb2a:	20064637          	lui	a2,0x20064
2005eb2e:	408485b3          	sub	a1,s1,s0
2005eb32:	00898533          	add	a0,s3,s0
2005eb36:	7f068693          	addi	a3,a3,2032 # 200637f0 <__FUNCTION__.0+0x1a8>
2005eb3a:	83060613          	addi	a2,a2,-2000 # 20063830 <__FUNCTION__.0+0x1e8>
2005eb3e:	1a1000ef          	jal	ra,2005f4de <DiagSnPrintf>
2005eb42:	942a                	c.add	s0,a0
2005eb44:	20065937          	lui	s2,0x20065
2005eb48:	20065a37          	lui	s4,0x20065
2005eb4c:	c6490913          	addi	s2,s2,-924 # 20064c64 <lib_pmc_git_rev>
2005eb50:	cc5a0a13          	addi	s4,s4,-827 # 20064cc5 <__git_ver_table_end__>
2005eb54:	20064ab7          	lui	s5,0x20064
2005eb58:	20063b37          	lui	s6,0x20063
2005eb5c:	03496163          	bltu	s2,s4,2005eb7e <ChipInfo_GetLibVersion_ToBuf+0xbe>
2005eb60:	40f2                	c.lwsp	ra,28(sp)
2005eb62:	4462                	c.lwsp	s0,24(sp)
2005eb64:	44d2                	c.lwsp	s1,20(sp)
2005eb66:	4942                	c.lwsp	s2,16(sp)
2005eb68:	49b2                	c.lwsp	s3,12(sp)
2005eb6a:	4a22                	c.lwsp	s4,8(sp)
2005eb6c:	4a92                	c.lwsp	s5,4(sp)
2005eb6e:	4b02                	c.lwsp	s6,0(sp)
2005eb70:	6105                	c.addi16sp	sp,32
2005eb72:	8082                	c.jr	ra
2005eb74:	200636b7          	lui	a3,0x20063
2005eb78:	7b068693          	addi	a3,a3,1968 # 200637b0 <__FUNCTION__.0+0x168>
2005eb7c:	bf95                	c.j	2005eaf0 <ChipInfo_GetLibVersion_ToBuf+0x30>
2005eb7e:	00094683          	lbu	a3,0(s2)
2005eb82:	e28d                	c.bnez	a3,2005eba4 <ChipInfo_GetLibVersion_ToBuf+0xe4>
2005eb84:	00194783          	lbu	a5,1(s2)
2005eb88:	0905                	c.addi	s2,1
2005eb8a:	dfed                	c.beqz	a5,2005eb84 <ChipInfo_GetLibVersion_ToBuf+0xc4>
2005eb8c:	fc9478e3          	bgeu	s0,s1,2005eb5c <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005eb90:	030b0613          	addi	a2,s6,48 # 20063030 <__FUNCTION__.0+0x20c>
2005eb94:	408485b3          	sub	a1,s1,s0
2005eb98:	00898533          	add	a0,s3,s0
2005eb9c:	143000ef          	jal	ra,2005f4de <DiagSnPrintf>
2005eba0:	942a                	c.add	s0,a0
2005eba2:	bf6d                	c.j	2005eb5c <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005eba4:	fa947ce3          	bgeu	s0,s1,2005eb5c <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005eba8:	840a8613          	addi	a2,s5,-1984 # 20063840 <__FUNCTION__.0+0x1f8>
2005ebac:	408485b3          	sub	a1,s1,s0
2005ebb0:	00898533          	add	a0,s3,s0
2005ebb4:	0905                	c.addi	s2,1
2005ebb6:	129000ef          	jal	ra,2005f4de <DiagSnPrintf>
2005ebba:	b7dd                	c.j	2005eba0 <ChipInfo_GetLibVersion_ToBuf+0xe0>

2005ebbc <ChipInfo_GetChipSram>:
2005ebbc:	1101                	c.addi	sp,-32
2005ebbe:	ce06                	c.swsp	ra,28(sp)
2005ebc0:	4100c7b7          	lui	a5,0x4100c
2005ebc4:	2107a783          	lw	a5,528(a5) # 4100c210 <__km4_bd_boot_download_addr__+0x10ffa210>
2005ebc8:	40400737          	lui	a4,0x40400
2005ebcc:	8ff9                	c.and	a5,a4
2005ebce:	c39d                	c.beqz	a5,2005ebf4 <ChipInfo_GetChipSram+0x38>
2005ebd0:	00f10593          	addi	a1,sp,15
2005ebd4:	7fc00513          	addi	a0,zero,2044
2005ebd8:	8c5fa0ef          	jal	ra,2005949c <OTP_Read8>
2005ebdc:	00f14783          	lbu	a5,15(sp)
2005ebe0:	470d                	c.li	a4,3
2005ebe2:	8389                	c.srli	a5,0x2
2005ebe4:	8b8d                	c.andi	a5,3
2005ebe6:	00e79663          	bne	a5,a4,2005ebf2 <ChipInfo_GetChipSram+0x36>
2005ebea:	4501                	c.li	a0,0
2005ebec:	40f2                	c.lwsp	ra,28(sp)
2005ebee:	6105                	c.addi16sp	sp,32
2005ebf0:	8082                	c.jr	ra
2005ebf2:	dfe5                	c.beqz	a5,2005ebea <ChipInfo_GetChipSram+0x2e>
2005ebf4:	00f10593          	addi	a1,sp,15
2005ebf8:	7fc00513          	addi	a0,zero,2044
2005ebfc:	8a1fa0ef          	jal	ra,2005949c <OTP_Read8>
2005ec00:	00f14783          	lbu	a5,15(sp)
2005ec04:	470d                	c.li	a4,3
2005ec06:	8399                	c.srli	a5,0x6
2005ec08:	00e78363          	beq	a5,a4,2005ec0e <ChipInfo_GetChipSram+0x52>
2005ec0c:	fff9                	c.bnez	a5,2005ebea <ChipInfo_GetChipSram+0x2e>
2005ec0e:	4505                	c.li	a0,1
2005ec10:	bff1                	c.j	2005ebec <ChipInfo_GetChipSram+0x30>

2005ec12 <vTaskStatus>:
2005ec12:	7139                	c.addi16sp	sp,-64
2005ec14:	de06                	c.swsp	ra,60(sp)
2005ec16:	dc22                	c.swsp	s0,56(sp)
2005ec18:	da26                	c.swsp	s1,52(sp)
2005ec1a:	f7bfe0ef          	jal	ra,2005db94 <vTaskEnterCritical>
2005ec1e:	d8ffe0ef          	jal	ra,2005d9ac <xTaskGetCurrentTaskHandle>
2005ec22:	4695                	c.li	a3,5
2005ec24:	4605                	c.li	a2,1
2005ec26:	006c                	c.addi4spn	a1,sp,12
2005ec28:	842a                	c.mv	s0,a0
2005ec2a:	200644b7          	lui	s1,0x20064
2005ec2e:	c8eff0ef          	jal	ra,2005e0bc <vTaskGetInfo>
2005ec32:	84448513          	addi	a0,s1,-1980 # 20063844 <__FUNCTION__.0+0x1fc>
2005ec36:	25e9                	c.jal	2005f300 <DiagPrintf>
2005ec38:	45c2                	c.lwsp	a1,16(sp)
2005ec3a:	20064537          	lui	a0,0x20064
2005ec3e:	86850513          	addi	a0,a0,-1944 # 20063868 <__FUNCTION__.0+0x220>
2005ec42:	2d7d                	c.jal	2005f300 <DiagPrintf>
2005ec44:	45d2                	c.lwsp	a1,20(sp)
2005ec46:	20064537          	lui	a0,0x20064
2005ec4a:	87c50513          	addi	a0,a0,-1924 # 2006387c <__FUNCTION__.0+0x234>
2005ec4e:	2d4d                	c.jal	2005f300 <DiagPrintf>
2005ec50:	20064537          	lui	a0,0x20064
2005ec54:	85a2                	c.mv	a1,s0
2005ec56:	88c50513          	addi	a0,a0,-1908 # 2006388c <__FUNCTION__.0+0x244>
2005ec5a:	255d                	c.jal	2005f300 <DiagPrintf>
2005ec5c:	01814583          	lbu	a1,24(sp)
2005ec60:	200647b7          	lui	a5,0x20064
2005ec64:	9b078793          	addi	a5,a5,-1616 # 200639b0 <TaskStateString.0>
2005ec68:	00259713          	slli	a4,a1,0x2
2005ec6c:	97ba                	c.add	a5,a4
2005ec6e:	4390                	c.lw	a2,0(a5)
2005ec70:	20064537          	lui	a0,0x20064
2005ec74:	8a050513          	addi	a0,a0,-1888 # 200638a0 <__FUNCTION__.0+0x258>
2005ec78:	2561                	c.jal	2005f300 <DiagPrintf>
2005ec7a:	5582                	c.lwsp	a1,32(sp)
2005ec7c:	20064537          	lui	a0,0x20064
2005ec80:	8bc50513          	addi	a0,a0,-1860 # 200638bc <__FUNCTION__.0+0x274>
2005ec84:	2db5                	c.jal	2005f300 <DiagPrintf>
2005ec86:	45f2                	c.lwsp	a1,28(sp)
2005ec88:	20064537          	lui	a0,0x20064
2005ec8c:	8d050513          	addi	a0,a0,-1840 # 200638d0 <__FUNCTION__.0+0x288>
2005ec90:	2d85                	c.jal	2005f300 <DiagPrintf>
2005ec92:	5592                	c.lwsp	a1,36(sp)
2005ec94:	20064537          	lui	a0,0x20064
2005ec98:	8e850513          	addi	a0,a0,-1816 # 200638e8 <__FUNCTION__.0+0x2a0>
2005ec9c:	2595                	c.jal	2005f300 <DiagPrintf>
2005ec9e:	4474                	c.lw	a3,76(s0)
2005eca0:	5818                	c.lw	a4,48(s0)
2005eca2:	5622                	c.lwsp	a2,40(sp)
2005eca4:	400c                	c.lw	a1,0(s0)
2005eca6:	40e68733          	sub	a4,a3,a4
2005ecaa:	8709                	c.srai	a4,0x2
2005ecac:	20064537          	lui	a0,0x20064
2005ecb0:	0705                	c.addi	a4,1
2005ecb2:	90050513          	addi	a0,a0,-1792 # 20063900 <__FUNCTION__.0+0x2b8>
2005ecb6:	25a9                	c.jal	2005f300 <DiagPrintf>
2005ecb8:	02c15583          	lhu	a1,44(sp)
2005ecbc:	20064537          	lui	a0,0x20064
2005ecc0:	95050513          	addi	a0,a0,-1712 # 20063950 <__FUNCTION__.0+0x308>
2005ecc4:	2d35                	c.jal	2005f300 <DiagPrintf>
2005ecc6:	84448513          	addi	a0,s1,-1980
2005ecca:	2d1d                	c.jal	2005f300 <DiagPrintf>
2005eccc:	50f2                	c.lwsp	ra,60(sp)
2005ecce:	5462                	c.lwsp	s0,56(sp)
2005ecd0:	54d2                	c.lwsp	s1,52(sp)
2005ecd2:	6121                	c.addi16sp	sp,64
2005ecd4:	8082                	c.jr	ra

2005ecd6 <vTaskCrashCallback>:
2005ecd6:	1141                	c.addi	sp,-16
2005ecd8:	c606                	c.swsp	ra,12(sp)
2005ecda:	e73fc0ef          	jal	ra,2005bb4c <rtos_sched_get_state>
2005ecde:	4785                	c.li	a5,1
2005ece0:	00f50663          	beq	a0,a5,2005ecec <vTaskCrashCallback+0x16>
2005ece4:	40b2                	c.lwsp	ra,12(sp)
2005ece6:	0141                	c.addi	sp,16
2005ece8:	f2bff06f          	jal	zero,2005ec12 <vTaskStatus>
2005ecec:	40b2                	c.lwsp	ra,12(sp)
2005ecee:	0141                	c.addi	sp,16
2005ecf0:	8082                	c.jr	ra

2005ecf2 <print_unsigned_num>:
2005ecf2:	715d                	c.addi16sp	sp,-80
2005ecf4:	c4a2                	c.swsp	s0,72(sp)
2005ecf6:	c2a6                	c.swsp	s1,68(sp)
2005ecf8:	c0ca                	c.swsp	s2,64(sp)
2005ecfa:	de4e                	c.swsp	s3,60(sp)
2005ecfc:	dc52                	c.swsp	s4,56(sp)
2005ecfe:	da56                	c.swsp	s5,52(sp)
2005ed00:	d85a                	c.swsp	s6,48(sp)
2005ed02:	d65e                	c.swsp	s7,44(sp)
2005ed04:	d462                	c.swsp	s8,40(sp)
2005ed06:	d266                	c.swsp	s9,36(sp)
2005ed08:	d06a                	c.swsp	s10,32(sp)
2005ed0a:	c686                	c.swsp	ra,76(sp)
2005ed0c:	8c2a                	c.mv	s8,a0
2005ed0e:	8cae                	c.mv	s9,a1
2005ed10:	8d32                	c.mv	s10,a2
2005ed12:	8936                	c.mv	s2,a3
2005ed14:	8bba                	c.mv	s7,a4
2005ed16:	89be                	c.mv	s3,a5
2005ed18:	84c2                	c.mv	s1,a6
2005ed1a:	4401                	c.li	s0,0
2005ed1c:	4a81                	c.li	s5,0
2005ed1e:	4a25                	c.li	s4,9
2005ed20:	4b05                	c.li	s6,1
2005ed22:	866a                	c.mv	a2,s10
2005ed24:	4681                	c.li	a3,0
2005ed26:	8562                	c.mv	a0,s8
2005ed28:	85e6                	c.mv	a1,s9
2005ed2a:	295020ef          	jal	ra,200617be <__umoddi3>
2005ed2e:	0ff57893          	andi	a7,a0,255
2005ed32:	04aa6863          	bltu	s4,a0,2005ed82 <print_unsigned_num+0x90>
2005ed36:	03088893          	addi	a7,a7,48 # f8000030 <__ctrace_end__+0x2e000030>
2005ed3a:	007c                	c.addi4spn	a5,sp,12
2005ed3c:	97a2                	c.add	a5,s0
2005ed3e:	0ff8f893          	andi	a7,a7,255
2005ed42:	01178023          	sb	a7,0(a5)
2005ed46:	866a                	c.mv	a2,s10
2005ed48:	4681                	c.li	a3,0
2005ed4a:	8562                	c.mv	a0,s8
2005ed4c:	85e6                	c.mv	a1,s9
2005ed4e:	0405                	c.addi	s0,1
2005ed50:	70e020ef          	jal	ra,2006145e <__udivdi3>
2005ed54:	039a9463          	bne	s5,s9,2005ed7c <print_unsigned_num+0x8a>
2005ed58:	03ac7263          	bgeu	s8,s10,2005ed7c <print_unsigned_num+0x8a>
2005ed5c:	c8d5                	c.beqz	s1,2005ee10 <print_unsigned_num+0x11e>
2005ed5e:	03000793          	addi	a5,zero,48
2005ed62:	1bfd                	c.addi	s7,-1
2005ed64:	0af91163          	bne	s2,a5,2005ee06 <print_unsigned_num+0x114>
2005ed68:	02d00513          	addi	a0,zero,45
2005ed6c:	dffa5097          	auipc	ra,0xdffa5
2005ed70:	7d8080e7          	jalr	ra,2008(ra) # 4544 <LOGUART_PutChar>
2005ed74:	89a6                	c.mv	s3,s1
2005ed76:	09704b63          	blt	zero,s7,2005ee0c <print_unsigned_num+0x11a>
2005ed7a:	a0b1                	c.j	2005edc6 <print_unsigned_num+0xd4>
2005ed7c:	8c2a                	c.mv	s8,a0
2005ed7e:	8cae                	c.mv	s9,a1
2005ed80:	b74d                	c.j	2005ed22 <print_unsigned_num+0x30>
2005ed82:	01699563          	bne	s3,s6,2005ed8c <print_unsigned_num+0x9a>
2005ed86:	03788893          	addi	a7,a7,55
2005ed8a:	bf45                	c.j	2005ed3a <print_unsigned_num+0x48>
2005ed8c:	05788893          	addi	a7,a7,87
2005ed90:	b76d                	c.j	2005ed3a <print_unsigned_num+0x48>
2005ed92:	854a                	c.mv	a0,s2
2005ed94:	dffa5097          	auipc	ra,0xdffa5
2005ed98:	7b0080e7          	jalr	ra,1968(ra) # 4544 <LOGUART_PutChar>
2005ed9c:	1a7d                	c.addi	s4,-1
2005ed9e:	ff444ae3          	blt	s0,s4,2005ed92 <print_unsigned_num+0xa0>
2005eda2:	4781                	c.li	a5,0
2005eda4:	008bc463          	blt	s7,s0,2005edac <print_unsigned_num+0xba>
2005eda8:	408b87b3          	sub	a5,s7,s0
2005edac:	97ce                	c.add	a5,s3
2005edae:	c0a9                	c.beqz	s1,2005edf0 <print_unsigned_num+0xfe>
2005edb0:	84be                	c.mv	s1,a5
2005edb2:	0df97913          	andi	s2,s2,223
2005edb6:	00091863          	bne	s2,zero,2005edc6 <print_unsigned_num+0xd4>
2005edba:	02d00513          	addi	a0,zero,45
2005edbe:	dffa5097          	auipc	ra,0xdffa5
2005edc2:	786080e7          	jalr	ra,1926(ra) # 4544 <LOGUART_PutChar>
2005edc6:	8922                	c.mv	s2,s0
2005edc8:	59fd                	c.li	s3,-1
2005edca:	197d                	c.addi	s2,-1
2005edcc:	03391463          	bne	s2,s3,2005edf4 <print_unsigned_num+0x102>
2005edd0:	00940533          	add	a0,s0,s1
2005edd4:	40b6                	c.lwsp	ra,76(sp)
2005edd6:	4426                	c.lwsp	s0,72(sp)
2005edd8:	4496                	c.lwsp	s1,68(sp)
2005edda:	4906                	c.lwsp	s2,64(sp)
2005eddc:	59f2                	c.lwsp	s3,60(sp)
2005edde:	5a62                	c.lwsp	s4,56(sp)
2005ede0:	5ad2                	c.lwsp	s5,52(sp)
2005ede2:	5b42                	c.lwsp	s6,48(sp)
2005ede4:	5bb2                	c.lwsp	s7,44(sp)
2005ede6:	5c22                	c.lwsp	s8,40(sp)
2005ede8:	5c92                	c.lwsp	s9,36(sp)
2005edea:	5d02                	c.lwsp	s10,32(sp)
2005edec:	6161                	c.addi16sp	sp,80
2005edee:	8082                	c.jr	ra
2005edf0:	84be                	c.mv	s1,a5
2005edf2:	bfd1                	c.j	2005edc6 <print_unsigned_num+0xd4>
2005edf4:	007c                	c.addi4spn	a5,sp,12
2005edf6:	97ca                	c.add	a5,s2
2005edf8:	0007c503          	lbu	a0,0(a5)
2005edfc:	dffa5097          	auipc	ra,0xdffa5
2005ee00:	748080e7          	jalr	ra,1864(ra) # 4544 <LOGUART_PutChar>
2005ee04:	b7d9                	c.j	2005edca <print_unsigned_num+0xd8>
2005ee06:	fb7056e3          	bge	zero,s7,2005edb2 <print_unsigned_num+0xc0>
2005ee0a:	89a6                	c.mv	s3,s1
2005ee0c:	8a5e                	c.mv	s4,s7
2005ee0e:	bf41                	c.j	2005ed9e <print_unsigned_num+0xac>
2005ee10:	4981                	c.li	s3,0
2005ee12:	b795                	c.j	2005ed76 <print_unsigned_num+0x84>

2005ee14 <pad_char_control>:
2005ee14:	1141                	c.addi	sp,-16
2005ee16:	c422                	c.swsp	s0,8(sp)
2005ee18:	c226                	c.swsp	s1,4(sp)
2005ee1a:	c04a                	c.swsp	s2,0(sp)
2005ee1c:	c606                	c.swsp	ra,12(sp)
2005ee1e:	842a                	c.mv	s0,a0
2005ee20:	892e                	c.mv	s2,a1
2005ee22:	84aa                	c.mv	s1,a0
2005ee24:	00904c63          	blt	zero,s1,2005ee3c <pad_char_control+0x28>
2005ee28:	8522                	c.mv	a0,s0
2005ee2a:	00045363          	bge	s0,zero,2005ee30 <pad_char_control+0x1c>
2005ee2e:	4501                	c.li	a0,0
2005ee30:	40b2                	c.lwsp	ra,12(sp)
2005ee32:	4422                	c.lwsp	s0,8(sp)
2005ee34:	4492                	c.lwsp	s1,4(sp)
2005ee36:	4902                	c.lwsp	s2,0(sp)
2005ee38:	0141                	c.addi	sp,16
2005ee3a:	8082                	c.jr	ra
2005ee3c:	854a                	c.mv	a0,s2
2005ee3e:	dffa5097          	auipc	ra,0xdffa5
2005ee42:	706080e7          	jalr	ra,1798(ra) # 4544 <LOGUART_PutChar>
2005ee46:	14fd                	c.addi	s1,-1
2005ee48:	bff1                	c.j	2005ee24 <pad_char_control+0x10>

2005ee4a <print_decimal_num>:
2005ee4a:	1101                	c.addi	sp,-32
2005ee4c:	c64e                	c.swsp	s3,12(sp)
2005ee4e:	200689b7          	lui	s3,0x20068
2005ee52:	cc22                	c.swsp	s0,24(sp)
2005ee54:	c84a                	c.swsp	s2,16(sp)
2005ee56:	c452                	c.swsp	s4,8(sp)
2005ee58:	c05a                	c.swsp	s6,0(sp)
2005ee5a:	8936                	c.mv	s2,a3
2005ee5c:	ce06                	c.swsp	ra,28(sp)
2005ee5e:	ca26                	c.swsp	s1,20(sp)
2005ee60:	c256                	c.swsp	s5,4(sp)
2005ee62:	882a                	c.mv	a6,a0
2005ee64:	8b32                	c.mv	s6,a2
2005ee66:	8a3a                	c.mv	s4,a4
2005ee68:	4401                	c.li	s0,0
2005ee6a:	46a5                	c.li	a3,9
2005ee6c:	58898993          	addi	s3,s3,1416 # 20068588 <num_buf_temp>
2005ee70:	02b87733          	remu	a4,a6,a1
2005ee74:	0ff77793          	andi	a5,a4,255
2005ee78:	04e6ee63          	bltu	a3,a4,2005eed4 <print_decimal_num+0x8a>
2005ee7c:	03078793          	addi	a5,a5,48
2005ee80:	0ff7f793          	andi	a5,a5,255
2005ee84:	00898733          	add	a4,s3,s0
2005ee88:	00f70023          	sb	a5,0(a4) # 40400000 <__km4_bd_boot_download_addr__+0x103ee000>
2005ee8c:	00140493          	addi	s1,s0,1
2005ee90:	02b857b3          	divu	a5,a6,a1
2005ee94:	02b87d63          	bgeu	a6,a1,2005eece <print_decimal_num+0x84>
2005ee98:	8aca                	c.mv	s5,s2
2005ee9a:	05204663          	blt	zero,s2,2005eee6 <print_decimal_num+0x9c>
2005ee9e:	4901                	c.li	s2,0
2005eea0:	02d00513          	addi	a0,zero,45
2005eea4:	040a1e63          	bne	s4,zero,2005ef00 <print_decimal_num+0xb6>
2005eea8:	14fd                	c.addi	s1,-1
2005eeaa:	0404d763          	bge	s1,zero,2005eef8 <print_decimal_num+0xae>
2005eeae:	00045363          	bge	s0,zero,2005eeb4 <print_decimal_num+0x6a>
2005eeb2:	547d                	c.li	s0,-1
2005eeb4:	0405                	c.addi	s0,1
2005eeb6:	01240533          	add	a0,s0,s2
2005eeba:	40f2                	c.lwsp	ra,28(sp)
2005eebc:	4462                	c.lwsp	s0,24(sp)
2005eebe:	44d2                	c.lwsp	s1,20(sp)
2005eec0:	4942                	c.lwsp	s2,16(sp)
2005eec2:	49b2                	c.lwsp	s3,12(sp)
2005eec4:	4a22                	c.lwsp	s4,8(sp)
2005eec6:	4a92                	c.lwsp	s5,4(sp)
2005eec8:	4b02                	c.lwsp	s6,0(sp)
2005eeca:	6105                	c.addi16sp	sp,32
2005eecc:	8082                	c.jr	ra
2005eece:	8426                	c.mv	s0,s1
2005eed0:	883e                	c.mv	a6,a5
2005eed2:	bf79                	c.j	2005ee70 <print_decimal_num+0x26>
2005eed4:	05778793          	addi	a5,a5,87
2005eed8:	b765                	c.j	2005ee80 <print_decimal_num+0x36>
2005eeda:	855a                	c.mv	a0,s6
2005eedc:	dffa5097          	auipc	ra,0xdffa5
2005eee0:	668080e7          	jalr	ra,1640(ra) # 4544 <LOGUART_PutChar>
2005eee4:	1afd                	c.addi	s5,-1
2005eee6:	ff54cae3          	blt	s1,s5,2005eeda <print_decimal_num+0x90>
2005eeea:	4781                	c.li	a5,0
2005eeec:	00994463          	blt	s2,s1,2005eef4 <print_decimal_num+0xaa>
2005eef0:	409907b3          	sub	a5,s2,s1
2005eef4:	893e                	c.mv	s2,a5
2005eef6:	b76d                	c.j	2005eea0 <print_decimal_num+0x56>
2005eef8:	009987b3          	add	a5,s3,s1
2005eefc:	0007c503          	lbu	a0,0(a5)
2005ef00:	dffa5097          	auipc	ra,0xdffa5
2005ef04:	644080e7          	jalr	ra,1604(ra) # 4544 <LOGUART_PutChar>
2005ef08:	b745                	c.j	2005eea8 <print_decimal_num+0x5e>

2005ef0a <pad_num_control>:
2005ef0a:	04088163          	beq	a7,zero,2005ef4c <pad_num_control+0x42>
2005ef0e:	1141                	c.addi	sp,-16
2005ef10:	c226                	c.swsp	s1,4(sp)
2005ef12:	84ba                	c.mv	s1,a4
2005ef14:	4701                	c.li	a4,0
2005ef16:	c422                	c.swsp	s0,8(sp)
2005ef18:	c04a                	c.swsp	s2,0(sp)
2005ef1a:	c606                	c.swsp	ra,12(sp)
2005ef1c:	8936                	c.mv	s2,a3
2005ef1e:	dd5ff0ef          	jal	ra,2005ecf2 <print_unsigned_num>
2005ef22:	842a                	c.mv	s0,a0
2005ef24:	00955d63          	bge	a0,s1,2005ef3e <pad_num_control+0x34>
2005ef28:	02000793          	addi	a5,zero,32
2005ef2c:	00f91963          	bne	s2,a5,2005ef3e <pad_num_control+0x34>
2005ef30:	02000593          	addi	a1,zero,32
2005ef34:	40a48533          	sub	a0,s1,a0
2005ef38:	eddff0ef          	jal	ra,2005ee14 <pad_char_control>
2005ef3c:	942a                	c.add	s0,a0
2005ef3e:	40b2                	c.lwsp	ra,12(sp)
2005ef40:	8522                	c.mv	a0,s0
2005ef42:	4422                	c.lwsp	s0,8(sp)
2005ef44:	4492                	c.lwsp	s1,4(sp)
2005ef46:	4902                	c.lwsp	s2,0(sp)
2005ef48:	0141                	c.addi	sp,16
2005ef4a:	8082                	c.jr	ra
2005ef4c:	da7ff06f          	jal	zero,2005ecf2 <print_unsigned_num>

2005ef50 <_rand>:
2005ef50:	1141                	c.addi	sp,-16
2005ef52:	450d                	c.li	a0,3
2005ef54:	c606                	c.swsp	ra,12(sp)
2005ef56:	f98fa0ef          	jal	ra,200596ee <PLL_State>
2005ef5a:	c50d                	c.beqz	a0,2005ef84 <_rand+0x34>
2005ef5c:	65c1                	c.lui	a1,0x10
2005ef5e:	410167b7          	lui	a5,0x41016
2005ef62:	453d                	c.li	a0,15
2005ef64:	4685                	c.li	a3,1
2005ef66:	00158713          	addi	a4,a1,1 # 10001 <mbedtls_ecp_check_pub_priv+0x123>
2005ef6a:	47d0                	c.lw	a2,12(a5)
2005ef6c:	00a61763          	bne	a2,a0,2005ef7a <_rand+0x2a>
2005ef70:	df94                	c.sw	a3,56(a5)
2005ef72:	177d                	c.addi	a4,-1
2005ef74:	fb7d                	c.bnez	a4,2005ef6a <_rand+0x1a>
2005ef76:	df94                	c.sw	a3,56(a5)
2005ef78:	b7fd                	c.j	2005ef66 <_rand+0x16>
2005ef7a:	de65                	c.beqz	a2,2005ef72 <_rand+0x22>
2005ef7c:	4b88                	c.lw	a0,16(a5)
2005ef7e:	40b2                	c.lwsp	ra,12(sp)
2005ef80:	0141                	c.addi	sp,16
2005ef82:	8082                	c.jr	ra
2005ef84:	fb418693          	addi	a3,gp,-76 # 20066858 <rand_first.1>
2005ef88:	429c                	c.lw	a5,0(a3)
2005ef8a:	20068737          	lui	a4,0x20068
2005ef8e:	59470713          	addi	a4,a4,1428 # 20068594 <rand_seed.0>
2005ef92:	eb91                	c.bnez	a5,2005efa6 <_rand+0x56>
2005ef94:	678d                	c.lui	a5,0x3
2005ef96:	03978793          	addi	a5,a5,57 # 3039 <FLASH_SetSpiMode+0x375>
2005ef9a:	c31c                	c.sw	a5,0(a4)
2005ef9c:	c35c                	c.sw	a5,4(a4)
2005ef9e:	c71c                	c.sw	a5,8(a4)
2005efa0:	c75c                	c.sw	a5,12(a4)
2005efa2:	4785                	c.li	a5,1
2005efa4:	c29c                	c.sw	a5,0(a3)
2005efa6:	4314                	c.lw	a3,0(a4)
2005efa8:	fff80537          	lui	a0,0xfff80
2005efac:	474c                	c.lw	a1,12(a4)
2005efae:	00669793          	slli	a5,a3,0x6
2005efb2:	8fb5                	c.xor	a5,a3
2005efb4:	06ca                	c.slli	a3,0x12
2005efb6:	8ee9                	c.and	a3,a0
2005efb8:	83b5                	c.srli	a5,0xd
2005efba:	8fb5                	c.xor	a5,a3
2005efbc:	4354                	c.lw	a3,4(a4)
2005efbe:	fff00837          	lui	a6,0xfff00
2005efc2:	c31c                	c.sw	a5,0(a4)
2005efc4:	00269613          	slli	a2,a3,0x2
2005efc8:	00c6c533          	xor	a0,a3,a2
2005efcc:	4714                	c.lw	a3,8(a4)
2005efce:	9a01                	c.andi	a2,-32
2005efd0:	816d                	c.srli	a0,0x1b
2005efd2:	8d31                	c.xor	a0,a2
2005efd4:	00d69613          	slli	a2,a3,0xd
2005efd8:	8e35                	c.xor	a2,a3
2005efda:	069e                	c.slli	a3,0x7
2005efdc:	8006f693          	andi	a3,a3,-2048
2005efe0:	8255                	c.srli	a2,0x15
2005efe2:	8e35                	c.xor	a2,a3
2005efe4:	00359693          	slli	a3,a1,0x3
2005efe8:	8ead                	c.xor	a3,a1
2005efea:	05b6                	c.slli	a1,0xd
2005efec:	c348                	c.sw	a0,4(a4)
2005efee:	82b1                	c.srli	a3,0xc
2005eff0:	0105f5b3          	and	a1,a1,a6
2005eff4:	8d3d                	c.xor	a0,a5
2005eff6:	8ead                	c.xor	a3,a1
2005eff8:	8d31                	c.xor	a0,a2
2005effa:	c710                	c.sw	a2,8(a4)
2005effc:	c754                	c.sw	a3,12(a4)
2005effe:	8d35                	c.xor	a0,a3
2005f000:	bfbd                	c.j	2005ef7e <_rand+0x2e>

2005f002 <DiagVprintf>:
2005f002:	7139                	c.addi16sp	sp,-64
2005f004:	d452                	c.swsp	s4,40(sp)
2005f006:	20064a37          	lui	s4,0x20064
2005f00a:	dc22                	c.swsp	s0,56(sp)
2005f00c:	da26                	c.swsp	s1,52(sp)
2005f00e:	d84a                	c.swsp	s2,48(sp)
2005f010:	d64e                	c.swsp	s3,44(sp)
2005f012:	d256                	c.swsp	s5,36(sp)
2005f014:	d05a                	c.swsp	s6,32(sp)
2005f016:	de06                	c.swsp	ra,60(sp)
2005f018:	ce5e                	c.swsp	s7,28(sp)
2005f01a:	cc62                	c.swsp	s8,24(sp)
2005f01c:	ca66                	c.swsp	s9,20(sp)
2005f01e:	892a                	c.mv	s2,a0
2005f020:	842e                	c.mv	s0,a1
2005f022:	4981                	c.li	s3,0
2005f024:	4481                	c.li	s1,0
2005f026:	9c8a0a13          	addi	s4,s4,-1592 # 200639c8 <TaskStateString.0+0x18>
2005f02a:	4aa5                	c.li	s5,9
2005f02c:	4b29                	c.li	s6,10
2005f02e:	00094503          	lbu	a0,0(s2)
2005f032:	ed19                	c.bnez	a0,2005f050 <DiagVprintf+0x4e>
2005f034:	50f2                	c.lwsp	ra,60(sp)
2005f036:	5462                	c.lwsp	s0,56(sp)
2005f038:	5942                	c.lwsp	s2,48(sp)
2005f03a:	59b2                	c.lwsp	s3,44(sp)
2005f03c:	5a22                	c.lwsp	s4,40(sp)
2005f03e:	5a92                	c.lwsp	s5,36(sp)
2005f040:	5b02                	c.lwsp	s6,32(sp)
2005f042:	4bf2                	c.lwsp	s7,28(sp)
2005f044:	4c62                	c.lwsp	s8,24(sp)
2005f046:	4cd2                	c.lwsp	s9,20(sp)
2005f048:	8526                	c.mv	a0,s1
2005f04a:	54d2                	c.lwsp	s1,52(sp)
2005f04c:	6121                	c.addi16sp	sp,64
2005f04e:	8082                	c.jr	ra
2005f050:	02500793          	addi	a5,zero,37
2005f054:	0905                	c.addi	s2,1
2005f056:	28f51f63          	bne	a0,a5,2005f2f4 <DiagVprintf+0x2f2>
2005f05a:	4881                	c.li	a7,0
2005f05c:	4701                	c.li	a4,0
2005f05e:	4681                	c.li	a3,0
2005f060:	4601                	c.li	a2,0
2005f062:	07a00e13          	addi	t3,zero,122
2005f066:	05700e93          	addi	t4,zero,87
2005f06a:	02200f13          	addi	t5,zero,34
2005f06e:	03000593          	addi	a1,zero,48
2005f072:	04300813          	addi	a6,zero,67
2005f076:	04c00f93          	addi	t6,zero,76
2005f07a:	03900293          	addi	t0,zero,57
2005f07e:	02000393          	addi	t2,zero,32
2005f082:	02500313          	addi	t1,zero,37
2005f086:	02d00b93          	addi	s7,zero,45
2005f08a:	00094783          	lbu	a5,0(s2)
2005f08e:	02fe6063          	bltu	t3,a5,2005f0ae <DiagVprintf+0xac>
2005f092:	02fee063          	bltu	t4,a5,2005f0b2 <DiagVprintf+0xb0>
2005f096:	22b78663          	beq	a5,a1,2005f2c2 <DiagVprintf+0x2c0>
2005f09a:	02f5eb63          	bltu	a1,a5,2005f0d0 <DiagVprintf+0xce>
2005f09e:	20678963          	beq	a5,t1,2005f2b0 <DiagVprintf+0x2ae>
2005f0a2:	02f36263          	bltu	t1,a5,2005f0c6 <DiagVprintf+0xc4>
2005f0a6:	20078c63          	beq	a5,zero,2005f2be <DiagVprintf+0x2bc>
2005f0aa:	24778163          	beq	a5,t2,2005f2ec <DiagVprintf+0x2ea>
2005f0ae:	54fd                	c.li	s1,-1
2005f0b0:	b751                	c.j	2005f034 <DiagVprintf+0x32>
2005f0b2:	fa878793          	addi	a5,a5,-88
2005f0b6:	0ff7f793          	andi	a5,a5,255
2005f0ba:	feff6ae3          	bltu	t5,a5,2005f0ae <DiagVprintf+0xac>
2005f0be:	078a                	c.slli	a5,0x2
2005f0c0:	97d2                	c.add	a5,s4
2005f0c2:	439c                	c.lw	a5,0(a5)
2005f0c4:	8782                	c.jr	a5
2005f0c6:	ff7794e3          	bne	a5,s7,2005f0ae <DiagVprintf+0xac>
2005f0ca:	0905                	c.addi	s2,1
2005f0cc:	0885                	c.addi	a7,1
2005f0ce:	bf75                	c.j	2005f08a <DiagVprintf+0x88>
2005f0d0:	19078363          	beq	a5,a6,2005f256 <DiagVprintf+0x254>
2005f0d4:	02f86563          	bltu	a6,a5,2005f0fe <DiagVprintf+0xfc>
2005f0d8:	20f2fb63          	bgeu	t0,a5,2005f2ee <DiagVprintf+0x2ec>
2005f0dc:	04200593          	addi	a1,zero,66
2005f0e0:	fcb797e3          	bne	a5,a1,2005f0ae <DiagVprintf+0xac>
2005f0e4:	4785                	c.li	a5,1
2005f0e6:	0ac7d063          	bge	a5,a2,2005f186 <DiagVprintf+0x184>
2005f0ea:	041d                	c.addi	s0,7
2005f0ec:	9861                	c.andi	s0,-8
2005f0ee:	4008                	c.lw	a0,0(s0)
2005f0f0:	404c                	c.lw	a1,4(s0)
2005f0f2:	00840b93          	addi	s7,s0,8
2005f0f6:	4801                	c.li	a6,0
2005f0f8:	87ce                	c.mv	a5,s3
2005f0fa:	4609                	c.li	a2,2
2005f0fc:	a889                	c.j	2005f14e <DiagVprintf+0x14c>
2005f0fe:	1bf78663          	beq	a5,t6,2005f2aa <DiagVprintf+0x2a8>
2005f102:	04f00593          	addi	a1,zero,79
2005f106:	fab794e3          	bne	a5,a1,2005f0ae <DiagVprintf+0xac>
2005f10a:	4785                	c.li	a5,1
2005f10c:	08c7d263          	bge	a5,a2,2005f190 <DiagVprintf+0x18e>
2005f110:	041d                	c.addi	s0,7
2005f112:	9861                	c.andi	s0,-8
2005f114:	4008                	c.lw	a0,0(s0)
2005f116:	404c                	c.lw	a1,4(s0)
2005f118:	00840b93          	addi	s7,s0,8
2005f11c:	4801                	c.li	a6,0
2005f11e:	87ce                	c.mv	a5,s3
2005f120:	4621                	c.li	a2,8
2005f122:	a035                	c.j	2005f14e <DiagVprintf+0x14c>
2005f124:	4785                	c.li	a5,1
2005f126:	02c7da63          	bge	a5,a2,2005f15a <DiagVprintf+0x158>
2005f12a:	041d                	c.addi	s0,7
2005f12c:	9861                	c.andi	s0,-8
2005f12e:	4008                	c.lw	a0,0(s0)
2005f130:	404c                	c.lw	a1,4(s0)
2005f132:	00840b93          	addi	s7,s0,8
2005f136:	0405d163          	bge	a1,zero,2005f178 <DiagVprintf+0x176>
2005f13a:	00a037b3          	sltu	a5,zero,a0
2005f13e:	40b005b3          	sub	a1,zero,a1
2005f142:	40a00533          	sub	a0,zero,a0
2005f146:	8d9d                	c.sub	a1,a5
2005f148:	4805                	c.li	a6,1
2005f14a:	87ce                	c.mv	a5,s3
2005f14c:	4629                	c.li	a2,10
2005f14e:	dbdff0ef          	jal	ra,2005ef0a <pad_num_control>
2005f152:	94aa                	c.add	s1,a0
2005f154:	0905                	c.addi	s2,1
2005f156:	845e                	c.mv	s0,s7
2005f158:	bdd9                	c.j	2005f02e <DiagVprintf+0x2c>
2005f15a:	4008                	c.lw	a0,0(s0)
2005f15c:	00440b93          	addi	s7,s0,4
2005f160:	41f55593          	srai	a1,a0,0x1f
2005f164:	bfc9                	c.j	2005f136 <DiagVprintf+0x134>
2005f166:	4785                	c.li	a5,1
2005f168:	00c7da63          	bge	a5,a2,2005f17c <DiagVprintf+0x17a>
2005f16c:	041d                	c.addi	s0,7
2005f16e:	9861                	c.andi	s0,-8
2005f170:	4008                	c.lw	a0,0(s0)
2005f172:	404c                	c.lw	a1,4(s0)
2005f174:	00840b93          	addi	s7,s0,8
2005f178:	4801                	c.li	a6,0
2005f17a:	bfc1                	c.j	2005f14a <DiagVprintf+0x148>
2005f17c:	4008                	c.lw	a0,0(s0)
2005f17e:	4581                	c.li	a1,0
2005f180:	00440b93          	addi	s7,s0,4
2005f184:	bfd5                	c.j	2005f178 <DiagVprintf+0x176>
2005f186:	4008                	c.lw	a0,0(s0)
2005f188:	4581                	c.li	a1,0
2005f18a:	00440b93          	addi	s7,s0,4
2005f18e:	b7a5                	c.j	2005f0f6 <DiagVprintf+0xf4>
2005f190:	4008                	c.lw	a0,0(s0)
2005f192:	4581                	c.li	a1,0
2005f194:	00440b93          	addi	s7,s0,4
2005f198:	b751                	c.j	2005f11c <DiagVprintf+0x11a>
2005f19a:	4985                	c.li	s3,1
2005f19c:	4785                	c.li	a5,1
2005f19e:	00c7dc63          	bge	a5,a2,2005f1b6 <DiagVprintf+0x1b4>
2005f1a2:	041d                	c.addi	s0,7
2005f1a4:	9861                	c.andi	s0,-8
2005f1a6:	4008                	c.lw	a0,0(s0)
2005f1a8:	404c                	c.lw	a1,4(s0)
2005f1aa:	00840b93          	addi	s7,s0,8
2005f1ae:	4801                	c.li	a6,0
2005f1b0:	87ce                	c.mv	a5,s3
2005f1b2:	4641                	c.li	a2,16
2005f1b4:	bf69                	c.j	2005f14e <DiagVprintf+0x14c>
2005f1b6:	4008                	c.lw	a0,0(s0)
2005f1b8:	4581                	c.li	a1,0
2005f1ba:	00440b93          	addi	s7,s0,4
2005f1be:	bfc5                	c.j	2005f1ae <DiagVprintf+0x1ac>
2005f1c0:	4008                	c.lw	a0,0(s0)
2005f1c2:	4801                	c.li	a6,0
2005f1c4:	4785                	c.li	a5,1
2005f1c6:	4721                	c.li	a4,8
2005f1c8:	03000693          	addi	a3,zero,48
2005f1cc:	4641                	c.li	a2,16
2005f1ce:	4581                	c.li	a1,0
2005f1d0:	d3bff0ef          	jal	ra,2005ef0a <pad_num_control>
2005f1d4:	00440b93          	addi	s7,s0,4
2005f1d8:	94aa                	c.add	s1,a0
2005f1da:	4985                	c.li	s3,1
2005f1dc:	bfa5                	c.j	2005f154 <DiagVprintf+0x152>
2005f1de:	00042c83          	lw	s9,0(s0)
2005f1e2:	00440b93          	addi	s7,s0,4
2005f1e6:	4401                	c.li	s0,0
2005f1e8:	008c87b3          	add	a5,s9,s0
2005f1ec:	0007c783          	lbu	a5,0(a5)
2005f1f0:	e7a9                	c.bnez	a5,2005f23a <DiagVprintf+0x238>
2005f1f2:	02089063          	bne	a7,zero,2005f212 <DiagVprintf+0x210>
2005f1f6:	40870533          	sub	a0,a4,s0
2005f1fa:	00a05c63          	bge	zero,a0,2005f212 <DiagVprintf+0x210>
2005f1fe:	85b6                	c.mv	a1,a3
2005f200:	c646                	c.swsp	a7,12(sp)
2005f202:	c43a                	c.swsp	a4,8(sp)
2005f204:	c236                	c.swsp	a3,4(sp)
2005f206:	c0fff0ef          	jal	ra,2005ee14 <pad_char_control>
2005f20a:	48b2                	c.lwsp	a7,12(sp)
2005f20c:	4722                	c.lwsp	a4,8(sp)
2005f20e:	4692                	c.lwsp	a3,4(sp)
2005f210:	94aa                	c.add	s1,a0
2005f212:	5c7d                	c.li	s8,-1
2005f214:	000c8863          	beq	s9,zero,2005f224 <DiagVprintf+0x222>
2005f218:	4c01                	c.li	s8,0
2005f21a:	018c87b3          	add	a5,s9,s8
2005f21e:	0007c503          	lbu	a0,0(a5)
2005f222:	ed11                	c.bnez	a0,2005f23e <DiagVprintf+0x23c>
2005f224:	94e2                	c.add	s1,s8
2005f226:	f20887e3          	beq	a7,zero,2005f154 <DiagVprintf+0x152>
2005f22a:	40870533          	sub	a0,a4,s0
2005f22e:	85b6                	c.mv	a1,a3
2005f230:	f2a052e3          	bge	zero,a0,2005f154 <DiagVprintf+0x152>
2005f234:	be1ff0ef          	jal	ra,2005ee14 <pad_char_control>
2005f238:	bf29                	c.j	2005f152 <DiagVprintf+0x150>
2005f23a:	0405                	c.addi	s0,1
2005f23c:	b775                	c.j	2005f1e8 <DiagVprintf+0x1e6>
2005f23e:	c646                	c.swsp	a7,12(sp)
2005f240:	c43a                	c.swsp	a4,8(sp)
2005f242:	c236                	c.swsp	a3,4(sp)
2005f244:	dffa5097          	auipc	ra,0xdffa5
2005f248:	300080e7          	jalr	ra,768(ra) # 4544 <LOGUART_PutChar>
2005f24c:	48b2                	c.lwsp	a7,12(sp)
2005f24e:	4722                	c.lwsp	a4,8(sp)
2005f250:	4692                	c.lwsp	a3,4(sp)
2005f252:	0c05                	c.addi	s8,1
2005f254:	b7d9                	c.j	2005f21a <DiagVprintf+0x218>
2005f256:	00440b93          	addi	s7,s0,4
2005f25a:	00044403          	lbu	s0,0(s0)
2005f25e:	02089163          	bne	a7,zero,2005f280 <DiagVprintf+0x27e>
2005f262:	4785                	c.li	a5,1
2005f264:	00e7de63          	bge	a5,a4,2005f280 <DiagVprintf+0x27e>
2005f268:	85b6                	c.mv	a1,a3
2005f26a:	fff70513          	addi	a0,a4,-1
2005f26e:	c646                	c.swsp	a7,12(sp)
2005f270:	c436                	c.swsp	a3,8(sp)
2005f272:	c23a                	c.swsp	a4,4(sp)
2005f274:	ba1ff0ef          	jal	ra,2005ee14 <pad_char_control>
2005f278:	48b2                	c.lwsp	a7,12(sp)
2005f27a:	46a2                	c.lwsp	a3,8(sp)
2005f27c:	4712                	c.lwsp	a4,4(sp)
2005f27e:	94aa                	c.add	s1,a0
2005f280:	8522                	c.mv	a0,s0
2005f282:	c646                	c.swsp	a7,12(sp)
2005f284:	c43a                	c.swsp	a4,8(sp)
2005f286:	c236                	c.swsp	a3,4(sp)
2005f288:	dffa5097          	auipc	ra,0xdffa5
2005f28c:	2bc080e7          	jalr	ra,700(ra) # 4544 <LOGUART_PutChar>
2005f290:	48b2                	c.lwsp	a7,12(sp)
2005f292:	0485                	c.addi	s1,1
2005f294:	ec0880e3          	beq	a7,zero,2005f154 <DiagVprintf+0x152>
2005f298:	4722                	c.lwsp	a4,8(sp)
2005f29a:	4785                	c.li	a5,1
2005f29c:	eae7dce3          	bge	a5,a4,2005f154 <DiagVprintf+0x152>
2005f2a0:	4692                	c.lwsp	a3,4(sp)
2005f2a2:	fff70513          	addi	a0,a4,-1
2005f2a6:	85b6                	c.mv	a1,a3
2005f2a8:	b771                	c.j	2005f234 <DiagVprintf+0x232>
2005f2aa:	0605                	c.addi	a2,1
2005f2ac:	0905                	c.addi	s2,1
2005f2ae:	bbf1                	c.j	2005f08a <DiagVprintf+0x88>
2005f2b0:	02500513          	addi	a0,zero,37
2005f2b4:	dffa5097          	auipc	ra,0xdffa5
2005f2b8:	290080e7          	jalr	ra,656(ra) # 4544 <LOGUART_PutChar>
2005f2bc:	0485                	c.addi	s1,1
2005f2be:	8ba2                	c.mv	s7,s0
2005f2c0:	bd51                	c.j	2005f154 <DiagVprintf+0x152>
2005f2c2:	0905                	c.addi	s2,1
2005f2c4:	03000693          	addi	a3,zero,48
2005f2c8:	4701                	c.li	a4,0
2005f2ca:	00094783          	lbu	a5,0(s2)
2005f2ce:	d60783e3          	beq	a5,zero,2005f034 <DiagVprintf+0x32>
2005f2d2:	fd078513          	addi	a0,a5,-48
2005f2d6:	0ff57513          	andi	a0,a0,255
2005f2da:	daaae8e3          	bltu	s5,a0,2005f08a <DiagVprintf+0x88>
2005f2de:	03670733          	mul	a4,a4,s6
2005f2e2:	0905                	c.addi	s2,1
2005f2e4:	fd070713          	addi	a4,a4,-48
2005f2e8:	973e                	c.add	a4,a5
2005f2ea:	b7c5                	c.j	2005f2ca <DiagVprintf+0x2c8>
2005f2ec:	0905                	c.addi	s2,1
2005f2ee:	02000693          	addi	a3,zero,32
2005f2f2:	bfd9                	c.j	2005f2c8 <DiagVprintf+0x2c6>
2005f2f4:	dffa5097          	auipc	ra,0xdffa5
2005f2f8:	250080e7          	jalr	ra,592(ra) # 4544 <LOGUART_PutChar>
2005f2fc:	0485                	c.addi	s1,1
2005f2fe:	bb05                	c.j	2005f02e <DiagVprintf+0x2c>

2005f300 <DiagPrintf>:
2005f300:	7139                	c.addi16sp	sp,-64
2005f302:	d22e                	c.swsp	a1,36(sp)
2005f304:	104c                	c.addi4spn	a1,sp,36
2005f306:	ce06                	c.swsp	ra,28(sp)
2005f308:	d432                	c.swsp	a2,40(sp)
2005f30a:	d636                	c.swsp	a3,44(sp)
2005f30c:	d83a                	c.swsp	a4,48(sp)
2005f30e:	da3e                	c.swsp	a5,52(sp)
2005f310:	dc42                	c.swsp	a6,56(sp)
2005f312:	de46                	c.swsp	a7,60(sp)
2005f314:	c62e                	c.swsp	a1,12(sp)
2005f316:	cedff0ef          	jal	ra,2005f002 <DiagVprintf>
2005f31a:	40f2                	c.lwsp	ra,28(sp)
2005f31c:	6121                	c.addi16sp	sp,64
2005f31e:	8082                	c.jr	ra

2005f320 <DiagPrintf_minimal>:
2005f320:	711d                	c.addi16sp	sp,-96
2005f322:	da26                	c.swsp	s1,52(sp)
2005f324:	00c4                	c.addi4spn	s1,sp,68
2005f326:	dc22                	c.swsp	s0,56(sp)
2005f328:	d84a                	c.swsp	s2,48(sp)
2005f32a:	d64e                	c.swsp	s3,44(sp)
2005f32c:	d452                	c.swsp	s4,40(sp)
2005f32e:	d256                	c.swsp	s5,36(sp)
2005f330:	de06                	c.swsp	ra,60(sp)
2005f332:	d05a                	c.swsp	s6,32(sp)
2005f334:	ce5e                	c.swsp	s7,28(sp)
2005f336:	892a                	c.mv	s2,a0
2005f338:	c2ae                	c.swsp	a1,68(sp)
2005f33a:	c4b2                	c.swsp	a2,72(sp)
2005f33c:	c6b6                	c.swsp	a3,76(sp)
2005f33e:	c8ba                	c.swsp	a4,80(sp)
2005f340:	cabe                	c.swsp	a5,84(sp)
2005f342:	ccc2                	c.swsp	a6,88(sp)
2005f344:	cec6                	c.swsp	a7,92(sp)
2005f346:	c626                	c.swsp	s1,12(sp)
2005f348:	4401                	c.li	s0,0
2005f34a:	03000993          	addi	s3,zero,48
2005f34e:	4a25                	c.li	s4,9
2005f350:	4aa9                	c.li	s5,10
2005f352:	00094503          	lbu	a0,0(s2)
2005f356:	cd1d                	c.beqz	a0,2005f394 <DiagPrintf_minimal+0x74>
2005f358:	02500793          	addi	a5,zero,37
2005f35c:	0905                	c.addi	s2,1
2005f35e:	16f51a63          	bne	a0,a5,2005f4d2 <DiagPrintf_minimal+0x1b2>
2005f362:	4681                	c.li	a3,0
2005f364:	4601                	c.li	a2,0
2005f366:	06300893          	addi	a7,zero,99
2005f36a:	03900593          	addi	a1,zero,57
2005f36e:	02000313          	addi	t1,zero,32
2005f372:	00094783          	lbu	a5,0(s2)
2005f376:	05178163          	beq	a5,a7,2005f3b8 <DiagPrintf_minimal+0x98>
2005f37a:	06f8e363          	bltu	a7,a5,2005f3e0 <DiagPrintf_minimal+0xc0>
2005f37e:	13378063          	beq	a5,s3,2005f49e <DiagPrintf_minimal+0x17e>
2005f382:	02f9e563          	bltu	s3,a5,2005f3ac <DiagPrintf_minimal+0x8c>
2005f386:	14678263          	beq	a5,t1,2005f4ca <DiagPrintf_minimal+0x1aa>
2005f38a:	02500713          	addi	a4,zero,37
2005f38e:	10e78063          	beq	a5,a4,2005f48e <DiagPrintf_minimal+0x16e>
2005f392:	547d                	c.li	s0,-1
2005f394:	50f2                	c.lwsp	ra,60(sp)
2005f396:	8522                	c.mv	a0,s0
2005f398:	5462                	c.lwsp	s0,56(sp)
2005f39a:	54d2                	c.lwsp	s1,52(sp)
2005f39c:	5942                	c.lwsp	s2,48(sp)
2005f39e:	59b2                	c.lwsp	s3,44(sp)
2005f3a0:	5a22                	c.lwsp	s4,40(sp)
2005f3a2:	5a92                	c.lwsp	s5,36(sp)
2005f3a4:	5b02                	c.lwsp	s6,32(sp)
2005f3a6:	4bf2                	c.lwsp	s7,28(sp)
2005f3a8:	6125                	c.addi16sp	sp,96
2005f3aa:	8082                	c.jr	ra
2005f3ac:	12f5f063          	bgeu	a1,a5,2005f4cc <DiagPrintf_minimal+0x1ac>
2005f3b0:	04300713          	addi	a4,zero,67
2005f3b4:	fce79fe3          	bne	a5,a4,2005f392 <DiagPrintf_minimal+0x72>
2005f3b8:	4785                	c.li	a5,1
2005f3ba:	00448b13          	addi	s6,s1,4
2005f3be:	0004c483          	lbu	s1,0(s1)
2005f3c2:	00d7d863          	bge	a5,a3,2005f3d2 <DiagPrintf_minimal+0xb2>
2005f3c6:	85b2                	c.mv	a1,a2
2005f3c8:	fff68513          	addi	a0,a3,-1
2005f3cc:	a49ff0ef          	jal	ra,2005ee14 <pad_char_control>
2005f3d0:	942a                	c.add	s0,a0
2005f3d2:	8526                	c.mv	a0,s1
2005f3d4:	dffa5097          	auipc	ra,0xdffa5
2005f3d8:	170080e7          	jalr	ra,368(ra) # 4544 <LOGUART_PutChar>
2005f3dc:	0405                	c.addi	s0,1
2005f3de:	a825                	c.j	2005f416 <DiagPrintf_minimal+0xf6>
2005f3e0:	07300713          	addi	a4,zero,115
2005f3e4:	04e78e63          	beq	a5,a4,2005f440 <DiagPrintf_minimal+0x120>
2005f3e8:	02f76963          	bltu	a4,a5,2005f41a <DiagPrintf_minimal+0xfa>
2005f3ec:	06400713          	addi	a4,zero,100
2005f3f0:	00e78663          	beq	a5,a4,2005f3fc <DiagPrintf_minimal+0xdc>
2005f3f4:	06900713          	addi	a4,zero,105
2005f3f8:	f8e79de3          	bne	a5,a4,2005f392 <DiagPrintf_minimal+0x72>
2005f3fc:	4088                	c.lw	a0,0(s1)
2005f3fe:	00448b13          	addi	s6,s1,4
2005f402:	4701                	c.li	a4,0
2005f404:	00055563          	bge	a0,zero,2005f40e <DiagPrintf_minimal+0xee>
2005f408:	40a00533          	sub	a0,zero,a0
2005f40c:	4705                	c.li	a4,1
2005f40e:	45a9                	c.li	a1,10
2005f410:	a3bff0ef          	jal	ra,2005ee4a <print_decimal_num>
2005f414:	942a                	c.add	s0,a0
2005f416:	84da                	c.mv	s1,s6
2005f418:	a08d                	c.j	2005f47a <DiagPrintf_minimal+0x15a>
2005f41a:	07500713          	addi	a4,zero,117
2005f41e:	00e78b63          	beq	a5,a4,2005f434 <DiagPrintf_minimal+0x114>
2005f422:	07800713          	addi	a4,zero,120
2005f426:	f6e796e3          	bne	a5,a4,2005f392 <DiagPrintf_minimal+0x72>
2005f42a:	00448b13          	addi	s6,s1,4
2005f42e:	4701                	c.li	a4,0
2005f430:	45c1                	c.li	a1,16
2005f432:	a029                	c.j	2005f43c <DiagPrintf_minimal+0x11c>
2005f434:	00448b13          	addi	s6,s1,4
2005f438:	4701                	c.li	a4,0
2005f43a:	45a9                	c.li	a1,10
2005f43c:	4088                	c.lw	a0,0(s1)
2005f43e:	bfc9                	c.j	2005f410 <DiagPrintf_minimal+0xf0>
2005f440:	0004ab03          	lw	s6,0(s1)
2005f444:	00448b93          	addi	s7,s1,4
2005f448:	4781                	c.li	a5,0
2005f44a:	00fb0733          	add	a4,s6,a5
2005f44e:	00074703          	lbu	a4,0(a4)
2005f452:	e715                	c.bnez	a4,2005f47e <DiagPrintf_minimal+0x15e>
2005f454:	40f68533          	sub	a0,a3,a5
2005f458:	00a05663          	bge	zero,a0,2005f464 <DiagPrintf_minimal+0x144>
2005f45c:	85b2                	c.mv	a1,a2
2005f45e:	9b7ff0ef          	jal	ra,2005ee14 <pad_char_control>
2005f462:	942a                	c.add	s0,a0
2005f464:	54fd                	c.li	s1,-1
2005f466:	000b0863          	beq	s6,zero,2005f476 <DiagPrintf_minimal+0x156>
2005f46a:	4481                	c.li	s1,0
2005f46c:	009b07b3          	add	a5,s6,s1
2005f470:	0007c503          	lbu	a0,0(a5)
2005f474:	e519                	c.bnez	a0,2005f482 <DiagPrintf_minimal+0x162>
2005f476:	9426                	c.add	s0,s1
2005f478:	84de                	c.mv	s1,s7
2005f47a:	0905                	c.addi	s2,1
2005f47c:	bdd9                	c.j	2005f352 <DiagPrintf_minimal+0x32>
2005f47e:	0785                	c.addi	a5,1
2005f480:	b7e9                	c.j	2005f44a <DiagPrintf_minimal+0x12a>
2005f482:	dffa5097          	auipc	ra,0xdffa5
2005f486:	0c2080e7          	jalr	ra,194(ra) # 4544 <LOGUART_PutChar>
2005f48a:	0485                	c.addi	s1,1
2005f48c:	b7c5                	c.j	2005f46c <DiagPrintf_minimal+0x14c>
2005f48e:	02500513          	addi	a0,zero,37
2005f492:	dffa5097          	auipc	ra,0xdffa5
2005f496:	0b2080e7          	jalr	ra,178(ra) # 4544 <LOGUART_PutChar>
2005f49a:	0405                	c.addi	s0,1
2005f49c:	bff9                	c.j	2005f47a <DiagPrintf_minimal+0x15a>
2005f49e:	0905                	c.addi	s2,1
2005f4a0:	03000613          	addi	a2,zero,48
2005f4a4:	4681                	c.li	a3,0
2005f4a6:	00094703          	lbu	a4,0(s2)
2005f4aa:	ee0705e3          	beq	a4,zero,2005f394 <DiagPrintf_minimal+0x74>
2005f4ae:	fd070813          	addi	a6,a4,-48
2005f4b2:	0ff87813          	andi	a6,a6,255
2005f4b6:	eb0a6ee3          	bltu	s4,a6,2005f372 <DiagPrintf_minimal+0x52>
2005f4ba:	03568533          	mul	a0,a3,s5
2005f4be:	0905                	c.addi	s2,1
2005f4c0:	fd050513          	addi	a0,a0,-48 # fff7ffd0 <__ctrace_end__+0x35f7ffd0>
2005f4c4:	00a706b3          	add	a3,a4,a0
2005f4c8:	bff9                	c.j	2005f4a6 <DiagPrintf_minimal+0x186>
2005f4ca:	0905                	c.addi	s2,1
2005f4cc:	02000613          	addi	a2,zero,32
2005f4d0:	bfd1                	c.j	2005f4a4 <DiagPrintf_minimal+0x184>
2005f4d2:	dffa5097          	auipc	ra,0xdffa5
2005f4d6:	072080e7          	jalr	ra,114(ra) # 4544 <LOGUART_PutChar>
2005f4da:	0405                	c.addi	s0,1
2005f4dc:	bd9d                	c.j	2005f352 <DiagPrintf_minimal+0x32>

2005f4de <DiagSnPrintf>:
2005f4de:	7159                	c.addi16sp	sp,-112
2005f4e0:	c6a2                	c.swsp	s0,76(sp)
2005f4e2:	c4a6                	c.swsp	s1,72(sp)
2005f4e4:	c2ca                	c.swsp	s2,68(sp)
2005f4e6:	c0ce                	c.swsp	s3,64(sp)
2005f4e8:	de52                	c.swsp	s4,60(sp)
2005f4ea:	dc56                	c.swsp	s5,56(sp)
2005f4ec:	da5a                	c.swsp	s6,52(sp)
2005f4ee:	d85e                	c.swsp	s7,48(sp)
2005f4f0:	d662                	c.swsp	s8,44(sp)
2005f4f2:	d466                	c.swsp	s9,40(sp)
2005f4f4:	d26a                	c.swsp	s10,36(sp)
2005f4f6:	d06e                	c.swsp	s11,32(sp)
2005f4f8:	ceb6                	c.swsp	a3,92(sp)
2005f4fa:	d0ba                	c.swsp	a4,96(sp)
2005f4fc:	d2be                	c.swsp	a5,100(sp)
2005f4fe:	d4c2                	c.swsp	a6,104(sp)
2005f500:	d6c6                	c.swsp	a7,108(sp)
2005f502:	28050463          	beq	a0,zero,2005f78a <DiagSnPrintf+0x2ac>
2005f506:	08fc                	c.addi4spn	a5,sp,92
2005f508:	c43e                	c.swsp	a5,8(sp)
2005f50a:	537d                	c.li	t1,-1
2005f50c:	c199                	c.beqz	a1,2005f512 <DiagSnPrintf+0x34>
2005f50e:	00b50333          	add	t1,a0,a1
2005f512:	200643b7          	lui	t2,0x20064
2005f516:	6405                	c.lui	s0,0x1
2005f518:	882a                	c.mv	a6,a0
2005f51a:	05000f13          	addi	t5,zero,80
2005f51e:	06400913          	addi	s2,zero,100
2005f522:	06300993          	addi	s3,zero,99
2005f526:	4fa9                	c.li	t6,10
2005f528:	03000293          	addi	t0,zero,48
2005f52c:	02d00a13          	addi	s4,zero,45
2005f530:	a5438393          	addi	t2,t2,-1452 # 20063a54 <TaskStateString.0+0xa4>
2005f534:	4aa5                	c.li	s5,9
2005f536:	0ef00b13          	addi	s6,zero,239
2005f53a:	eff40b93          	addi	s7,s0,-257 # eff <CPU_ClkGet+0x3e9>
2005f53e:	6c3d                	c.lui	s8,0xf
2005f540:	7cc1                	c.lui	s9,0xffff0
2005f542:	00064783          	lbu	a5,0(a2)
2005f546:	e399                	c.bnez	a5,2005f54c <DiagSnPrintf+0x6e>
2005f548:	8742                	c.mv	a4,a6
2005f54a:	a889                	c.j	2005f59c <DiagSnPrintf+0xbe>
2005f54c:	02500713          	addi	a4,zero,37
2005f550:	00e78b63          	beq	a5,a4,2005f566 <DiagSnPrintf+0x88>
2005f554:	00180713          	addi	a4,a6,1 # fff00001 <__ctrace_end__+0x35f00001>
2005f558:	00f80023          	sb	a5,0(a6)
2005f55c:	04677063          	bgeu	a4,t1,2005f59c <DiagSnPrintf+0xbe>
2005f560:	0605                	c.addi	a2,1
2005f562:	883a                	c.mv	a6,a4
2005f564:	bff9                	c.j	2005f542 <DiagSnPrintf+0x64>
2005f566:	4722                	c.lwsp	a4,8(sp)
2005f568:	00164e03          	lbu	t3,1(a2)
2005f56c:	07300693          	addi	a3,zero,115
2005f570:	431c                	c.lw	a5,0(a4)
2005f572:	00160593          	addi	a1,a2,1
2005f576:	0711                	c.addi	a4,4
2005f578:	04de1463          	bne	t3,a3,2005f5c0 <DiagSnPrintf+0xe2>
2005f57c:	c43a                	c.swsp	a4,8(sp)
2005f57e:	410787b3          	sub	a5,a5,a6
2005f582:	8742                	c.mv	a4,a6
2005f584:	00f706b3          	add	a3,a4,a5
2005f588:	0006c683          	lbu	a3,0(a3)
2005f58c:	e299                	c.bnez	a3,2005f592 <DiagSnPrintf+0xb4>
2005f58e:	862e                	c.mv	a2,a1
2005f590:	bfc1                	c.j	2005f560 <DiagSnPrintf+0x82>
2005f592:	0705                	c.addi	a4,1
2005f594:	fed70fa3          	sb	a3,-1(a4)
2005f598:	fe6766e3          	bltu	a4,t1,2005f584 <DiagSnPrintf+0xa6>
2005f59c:	00070023          	sb	zero,0(a4)
2005f5a0:	40a70533          	sub	a0,a4,a0
2005f5a4:	4436                	c.lwsp	s0,76(sp)
2005f5a6:	44a6                	c.lwsp	s1,72(sp)
2005f5a8:	4916                	c.lwsp	s2,68(sp)
2005f5aa:	4986                	c.lwsp	s3,64(sp)
2005f5ac:	5a72                	c.lwsp	s4,60(sp)
2005f5ae:	5ae2                	c.lwsp	s5,56(sp)
2005f5b0:	5b52                	c.lwsp	s6,52(sp)
2005f5b2:	5bc2                	c.lwsp	s7,48(sp)
2005f5b4:	5c32                	c.lwsp	s8,44(sp)
2005f5b6:	5ca2                	c.lwsp	s9,40(sp)
2005f5b8:	5d12                	c.lwsp	s10,36(sp)
2005f5ba:	5d82                	c.lwsp	s11,32(sp)
2005f5bc:	6165                	c.addi16sp	sp,112
2005f5be:	8082                	c.jr	ra
2005f5c0:	c43a                	c.swsp	a4,8(sp)
2005f5c2:	0dfe7693          	andi	a3,t3,223
2005f5c6:	473d                	c.li	a4,15
2005f5c8:	0de69563          	bne	a3,t5,2005f692 <DiagSnPrintf+0x1b4>
2005f5cc:	0cf75a63          	bge	a4,a5,2005f6a0 <DiagSnPrintf+0x1c2>
2005f5d0:	46a1                	c.li	a3,8
2005f5d2:	03000893          	addi	a7,zero,48
2005f5d6:	ff078e93          	addi	t4,a5,-16
2005f5da:	4711                	c.li	a4,4
2005f5dc:	05db7463          	bgeu	s6,t4,2005f624 <DiagSnPrintf+0x146>
2005f5e0:	f0078e93          	addi	t4,a5,-256
2005f5e4:	4721                	c.li	a4,8
2005f5e6:	03dbff63          	bgeu	s7,t4,2005f624 <DiagSnPrintf+0x146>
2005f5ea:	40878eb3          	sub	t4,a5,s0
2005f5ee:	4731                	c.li	a4,12
2005f5f0:	038eea63          	bltu	t4,s8,2005f624 <DiagSnPrintf+0x146>
2005f5f4:	01978eb3          	add	t4,a5,s9
2005f5f8:	000f04b7          	lui	s1,0xf0
2005f5fc:	4741                	c.li	a4,16
2005f5fe:	029ee363          	bltu	t4,s1,2005f624 <DiagSnPrintf+0x146>
2005f602:	fff00eb7          	lui	t4,0xfff00
2005f606:	9ebe                	c.add	t4,a5
2005f608:	00f004b7          	lui	s1,0xf00
2005f60c:	4751                	c.li	a4,20
2005f60e:	009eeb63          	bltu	t4,s1,2005f624 <DiagSnPrintf+0x146>
2005f612:	ff000eb7          	lui	t4,0xff000
2005f616:	9ebe                	c.add	t4,a5
2005f618:	0f0004b7          	lui	s1,0xf000
2005f61c:	4761                	c.li	a4,24
2005f61e:	009ee363          	bltu	t4,s1,2005f624 <DiagSnPrintf+0x146>
2005f622:	4771                	c.li	a4,28
2005f624:	005e1663          	bne	t3,t0,2005f630 <DiagSnPrintf+0x152>
2005f628:	00260593          	addi	a1,a2,2
2005f62c:	03000893          	addi	a7,zero,48
2005f630:	0005c603          	lbu	a2,0(a1)
2005f634:	44a1                	c.li	s1,8
2005f636:	fcf60e13          	addi	t3,a2,-49
2005f63a:	0ffe7e13          	andi	t3,t3,255
2005f63e:	03c4e163          	bltu	s1,t3,2005f660 <DiagSnPrintf+0x182>
2005f642:	4681                	c.li	a3,0
2005f644:	03f686b3          	mul	a3,a3,t6
2005f648:	0585                	c.addi	a1,1
2005f64a:	96b2                	c.add	a3,a2
2005f64c:	0005c603          	lbu	a2,0(a1)
2005f650:	fd068693          	addi	a3,a3,-48
2005f654:	fd060e13          	addi	t3,a2,-48
2005f658:	0ffe7e13          	andi	t3,t3,255
2005f65c:	ffcaf4e3          	bgeu	s5,t3,2005f644 <DiagSnPrintf+0x166>
2005f660:	0005c603          	lbu	a2,0(a1)
2005f664:	0d767e13          	andi	t3,a2,215
2005f668:	0bee1463          	bne	t3,t5,2005f710 <DiagSnPrintf+0x232>
2005f66c:	02067493          	andi	s1,a2,32
2005f670:	0df67613          	andi	a2,a2,223
2005f674:	8ebe                	c.mv	t4,a5
2005f676:	00c10e13          	addi	t3,sp,12
2005f67a:	01e61a63          	bne	a2,t5,2005f68e <DiagSnPrintf+0x1b0>
2005f67e:	0584e613          	ori	a2,s1,88
2005f682:	00510623          	sb	t0,12(sp)
2005f686:	00c106a3          	sb	a2,13(sp)
2005f68a:	00e10e13          	addi	t3,sp,14
2005f68e:	4601                	c.li	a2,0
2005f690:	a831                	c.j	2005f6ac <DiagSnPrintf+0x1ce>
2005f692:	4681                	c.li	a3,0
2005f694:	02000893          	addi	a7,zero,32
2005f698:	f2f74fe3          	blt	a4,a5,2005f5d6 <DiagSnPrintf+0xf8>
2005f69c:	4701                	c.li	a4,0
2005f69e:	b759                	c.j	2005f624 <DiagSnPrintf+0x146>
2005f6a0:	4701                	c.li	a4,0
2005f6a2:	46a1                	c.li	a3,8
2005f6a4:	b761                	c.j	2005f62c <DiagSnPrintf+0x14e>
2005f6a6:	0605                	c.addi	a2,1
2005f6a8:	004ede93          	srli	t4,t4,0x4
2005f6ac:	fe0e9de3          	bne	t4,zero,2005f6a6 <DiagSnPrintf+0x1c8>
2005f6b0:	8ef2                	c.mv	t4,t3
2005f6b2:	00de0db3          	add	s11,t3,a3
2005f6b6:	41dd8d33          	sub	s10,s11,t4
2005f6ba:	05a64763          	blt	a2,s10,2005f708 <DiagSnPrintf+0x22a>
2005f6be:	4881                	c.li	a7,0
2005f6c0:	00c6c463          	blt	a3,a2,2005f6c8 <DiagSnPrintf+0x1ea>
2005f6c4:	40c688b3          	sub	a7,a3,a2
2005f6c8:	9e46                	c.add	t3,a7
2005f6ca:	88f2                	c.mv	a7,t3
2005f6cc:	863a                	c.mv	a2,a4
2005f6ce:	40c7d6b3          	sra	a3,a5,a2
2005f6d2:	8abd                	c.andi	a3,15
2005f6d4:	969e                	c.add	a3,t2
2005f6d6:	0006c683          	lbu	a3,0(a3)
2005f6da:	1671                	c.addi	a2,-4
2005f6dc:	0885                	c.addi	a7,1
2005f6de:	8ec5                	c.or	a3,s1
2005f6e0:	fed88fa3          	sb	a3,-1(a7)
2005f6e4:	fe0655e3          	bge	a2,zero,2005f6ce <DiagSnPrintf+0x1f0>
2005f6e8:	8309                	c.srli	a4,0x2
2005f6ea:	0705                	c.addi	a4,1
2005f6ec:	9e3a                	c.add	t3,a4
2005f6ee:	8742                	c.mv	a4,a6
2005f6f0:	007c                	c.addi4spn	a5,sp,12
2005f6f2:	e9c7fee3          	bgeu	a5,t3,2005f58e <DiagSnPrintf+0xb0>
2005f6f6:	0007c683          	lbu	a3,0(a5)
2005f6fa:	0705                	c.addi	a4,1
2005f6fc:	fed70fa3          	sb	a3,-1(a4)
2005f700:	e8677ee3          	bgeu	a4,t1,2005f59c <DiagSnPrintf+0xbe>
2005f704:	0785                	c.addi	a5,1
2005f706:	b7f5                	c.j	2005f6f2 <DiagSnPrintf+0x214>
2005f708:	011e8023          	sb	a7,0(t4) # ff000000 <__ctrace_end__+0x35000000>
2005f70c:	0e85                	c.addi	t4,1
2005f70e:	b765                	c.j	2005f6b6 <DiagSnPrintf+0x1d8>
2005f710:	07261363          	bne	a2,s2,2005f776 <DiagSnPrintf+0x298>
2005f714:	0407de63          	bge	a5,zero,2005f770 <DiagSnPrintf+0x292>
2005f718:	01410623          	sb	s4,12(sp)
2005f71c:	40f007b3          	sub	a5,zero,a5
2005f720:	4885                	c.li	a7,1
2005f722:	00d10693          	addi	a3,sp,13
2005f726:	8e36                	c.mv	t3,a3
2005f728:	03f7e633          	rem	a2,a5,t6
2005f72c:	0e05                	c.addi	t3,1
2005f72e:	03f7c7b3          	div	a5,a5,t6
2005f732:	03060613          	addi	a2,a2,48
2005f736:	fece0fa3          	sb	a2,-1(t3)
2005f73a:	011e0633          	add	a2,t3,a7
2005f73e:	8e15                	c.sub	a2,a3
2005f740:	f7e5                	c.bnez	a5,2005f728 <DiagSnPrintf+0x24a>
2005f742:	00c04563          	blt	zero,a2,2005f74c <DiagSnPrintf+0x26e>
2005f746:	005e0023          	sb	t0,0(t3)
2005f74a:	0e05                	c.addi	t3,1
2005f74c:	1771                	c.addi	a4,-4
2005f74e:	167d                	c.addi	a2,-1
2005f750:	fe0759e3          	bge	a4,zero,2005f742 <DiagSnPrintf+0x264>
2005f754:	87f2                	c.mv	a5,t3
2005f756:	17fd                	c.addi	a5,-1
2005f758:	f8f6fbe3          	bgeu	a3,a5,2005f6ee <DiagSnPrintf+0x210>
2005f75c:	0006c603          	lbu	a2,0(a3)
2005f760:	0007c703          	lbu	a4,0(a5)
2005f764:	0685                	c.addi	a3,1
2005f766:	00c78023          	sb	a2,0(a5)
2005f76a:	fee68fa3          	sb	a4,-1(a3)
2005f76e:	b7e5                	c.j	2005f756 <DiagSnPrintf+0x278>
2005f770:	4881                	c.li	a7,0
2005f772:	0074                	c.addi4spn	a3,sp,12
2005f774:	bf4d                	c.j	2005f726 <DiagSnPrintf+0x248>
2005f776:	01361763          	bne	a2,s3,2005f784 <DiagSnPrintf+0x2a6>
2005f77a:	00f10623          	sb	a5,12(sp)
2005f77e:	00d10e13          	addi	t3,sp,13
2005f782:	b7b5                	c.j	2005f6ee <DiagSnPrintf+0x210>
2005f784:	00c10623          	sb	a2,12(sp)
2005f788:	bfdd                	c.j	2005f77e <DiagSnPrintf+0x2a0>
2005f78a:	4501                	c.li	a0,0
2005f78c:	bd21                	c.j	2005f5a4 <DiagSnPrintf+0xc6>

2005f78e <TRNG_get_random_bytes>:
2005f78e:	7179                	c.addi16sp	sp,-48
2005f790:	d422                	c.swsp	s0,40(sp)
2005f792:	d226                	c.swsp	s1,36(sp)
2005f794:	d606                	c.swsp	ra,44(sp)
2005f796:	d04a                	c.swsp	s2,32(sp)
2005f798:	ce4e                	c.swsp	s3,28(sp)
2005f79a:	4100c7b7          	lui	a5,0x4100c
2005f79e:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005f7a2:	84aa                	c.mv	s1,a0
2005f7a4:	842e                	c.mv	s0,a1
2005f7a6:	8399                	c.srli	a5,0x6
2005f7a8:	8b8d                	c.andi	a5,3
2005f7aa:	ef9d                	c.bnez	a5,2005f7e8 <TRNG_get_random_bytes+0x5a>
2005f7ac:	00357793          	andi	a5,a0,3
2005f7b0:	cf85                	c.beqz	a5,2005f7e8 <TRNG_get_random_bytes+0x5a>
2005f7b2:	852e                	c.mv	a0,a1
2005f7b4:	ef1fb0ef          	jal	ra,2005b6a4 <rtos_mem_malloc>
2005f7b8:	85a2                	c.mv	a1,s0
2005f7ba:	892a                	c.mv	s2,a0
2005f7bc:	dffa9097          	auipc	ra,0xdffa9
2005f7c0:	5ae080e7          	jalr	ra,1454(ra) # 8d6a <RandBytes_Get>
2005f7c4:	8622                	c.mv	a2,s0
2005f7c6:	85ca                	c.mv	a1,s2
2005f7c8:	8526                	c.mv	a0,s1
2005f7ca:	dffaa097          	auipc	ra,0xdffaa
2005f7ce:	306080e7          	jalr	ra,774(ra) # 9ad0 <__wrap_memcpy>
2005f7d2:	854a                	c.mv	a0,s2
2005f7d4:	efbfb0ef          	jal	ra,2005b6ce <rtos_mem_free>
2005f7d8:	50b2                	c.lwsp	ra,44(sp)
2005f7da:	5422                	c.lwsp	s0,40(sp)
2005f7dc:	5492                	c.lwsp	s1,36(sp)
2005f7de:	5902                	c.lwsp	s2,32(sp)
2005f7e0:	49f2                	c.lwsp	s3,28(sp)
2005f7e2:	4501                	c.li	a0,0
2005f7e4:	6145                	c.addi16sp	sp,48
2005f7e6:	8082                	c.jr	ra
2005f7e8:	00245913          	srli	s2,s0,0x2
2005f7ec:	4981                	c.li	s3,0
2005f7ee:	0329c463          	blt	s3,s2,2005f816 <TRNG_get_random_bytes+0x88>
2005f7f2:	5671                	c.li	a2,-4
2005f7f4:	02c90633          	mul	a2,s2,a2
2005f7f8:	9432                	c.add	s0,a2
2005f7fa:	dc79                	c.beqz	s0,2005f7d8 <TRNG_get_random_bytes+0x4a>
2005f7fc:	f54ff0ef          	jal	ra,2005ef50 <_rand>
2005f800:	c62a                	c.swsp	a0,12(sp)
2005f802:	00291513          	slli	a0,s2,0x2
2005f806:	8622                	c.mv	a2,s0
2005f808:	006c                	c.addi4spn	a1,sp,12
2005f80a:	9526                	c.add	a0,s1
2005f80c:	dffaa097          	auipc	ra,0xdffaa
2005f810:	e06080e7          	jalr	ra,-506(ra) # 9612 <_memcpy>
2005f814:	b7d1                	c.j	2005f7d8 <TRNG_get_random_bytes+0x4a>
2005f816:	f3aff0ef          	jal	ra,2005ef50 <_rand>
2005f81a:	00299793          	slli	a5,s3,0x2
2005f81e:	97a6                	c.add	a5,s1
2005f820:	c388                	c.sw	a0,0(a5)
2005f822:	0985                	c.addi	s3,1
2005f824:	b7e9                	c.j	2005f7ee <TRNG_get_random_bytes+0x60>

2005f826 <io_assert_failed>:
2005f826:	862e                	c.mv	a2,a1
2005f828:	85aa                	c.mv	a1,a0
2005f82a:	20064537          	lui	a0,0x20064
2005f82e:	1141                	c.addi	sp,-16
2005f830:	a6850513          	addi	a0,a0,-1432 # 20063a68 <TaskStateString.0+0xb8>
2005f834:	c606                	c.swsp	ra,12(sp)
2005f836:	aebff0ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005f83a:	a001                	c.j	2005f83a <io_assert_failed+0x14>

2005f83c <irq_set_priority>:
2005f83c:	471d                	c.li	a4,7
2005f83e:	02b77563          	bgeu	a4,a1,2005f868 <irq_set_priority+0x2c>
2005f842:	862e                	c.mv	a2,a1
2005f844:	85aa                	c.mv	a1,a0
2005f846:	20064537          	lui	a0,0x20064
2005f84a:	1141                	c.addi	sp,-16
2005f84c:	469d                	c.li	a3,7
2005f84e:	b4450513          	addi	a0,a0,-1212 # 20063b44 <TaskStateString.0+0x194>
2005f852:	c606                	c.swsp	ra,12(sp)
2005f854:	acdff0ef          	jal	ra,2005f320 <DiagPrintf_minimal>
2005f858:	20064537          	lui	a0,0x20064
2005f85c:	44800593          	addi	a1,zero,1096
2005f860:	a9c50513          	addi	a0,a0,-1380 # 20063a9c <TaskStateString.0+0xec>
2005f864:	fc3ff0ef          	jal	ra,2005f826 <io_assert_failed>
2005f868:	73e00067          	jalr	zero,1854(zero) # 0 <Reset_Handler>

2005f86c <irq_register>:
2005f86c:	20000737          	lui	a4,0x20000
2005f870:	1141                	c.addi	sp,-16
2005f872:	00259893          	slli	a7,a1,0x2
2005f876:	882a                	c.mv	a6,a0
2005f878:	00070713          	addi	a4,a4,0 # 20000000 <UserIrqFunTable>
2005f87c:	200007b7          	lui	a5,0x20000
2005f880:	c606                	c.swsp	ra,12(sp)
2005f882:	9746                	c.add	a4,a7
2005f884:	00186813          	ori	a6,a6,1
2005f888:	1c078793          	addi	a5,a5,448 # 200001c0 <UserIrqDataTable>
2005f88c:	01072023          	sw	a6,0(a4)
2005f890:	97c6                	c.add	a5,a7
2005f892:	852e                	c.mv	a0,a1
2005f894:	c390                	c.sw	a2,0(a5)
2005f896:	85b6                	c.mv	a1,a3
2005f898:	fa5ff0ef          	jal	ra,2005f83c <irq_set_priority>
2005f89c:	40b2                	c.lwsp	ra,12(sp)
2005f89e:	4505                	c.li	a0,1
2005f8a0:	0141                	c.addi	sp,16
2005f8a2:	8082                	c.jr	ra

2005f8a4 <unwind_frame_rv32i>:
2005f8a4:	715d                	c.addi16sp	sp,-80
2005f8a6:	c686                	c.swsp	ra,76(sp)
2005f8a8:	c4a2                	c.swsp	s0,72(sp)
2005f8aa:	0880                	c.addi4spn	s0,sp,80
2005f8ac:	faa42e23          	sw	a0,-68(s0)
2005f8b0:	fab42c23          	sw	a1,-72(s0)
2005f8b4:	fe042623          	sw	zero,-20(s0)
2005f8b8:	fe042423          	sw	zero,-24(s0)
2005f8bc:	fbc42783          	lw	a5,-68(s0)
2005f8c0:	43dc                	c.lw	a5,4(a5)
2005f8c2:	c789                	c.beqz	a5,2005f8cc <unwind_frame_rv32i+0x28>
2005f8c4:	fbc42783          	lw	a5,-68(s0)
2005f8c8:	439c                	c.lw	a5,0(a5)
2005f8ca:	e399                	c.bnez	a5,2005f8d0 <unwind_frame_rv32i+0x2c>
2005f8cc:	57fd                	c.li	a5,-1
2005f8ce:	a44d                	c.j	2005fb70 <unwind_frame_rv32i+0x2cc>
2005f8d0:	fbc42783          	lw	a5,-68(s0)
2005f8d4:	43dc                	c.lw	a5,4(a5)
2005f8d6:	fef42223          	sw	a5,-28(s0)
2005f8da:	a435                	c.j	2005fb06 <unwind_frame_rv32i+0x262>
2005f8dc:	fe442783          	lw	a5,-28(s0)
2005f8e0:	8b89                	c.andi	a5,2
2005f8e2:	c385                	c.beqz	a5,2005f902 <unwind_frame_rv32i+0x5e>
2005f8e4:	fe442783          	lw	a5,-28(s0)
2005f8e8:	0007d783          	lhu	a5,0(a5)
2005f8ec:	873e                	c.mv	a4,a5
2005f8ee:	fe442783          	lw	a5,-28(s0)
2005f8f2:	0789                	c.addi	a5,2
2005f8f4:	0007d783          	lhu	a5,0(a5)
2005f8f8:	07c2                	c.slli	a5,0x10
2005f8fa:	8fd9                	c.or	a5,a4
2005f8fc:	fcf42c23          	sw	a5,-40(s0)
2005f900:	a031                	c.j	2005f90c <unwind_frame_rv32i+0x68>
2005f902:	fe442783          	lw	a5,-28(s0)
2005f906:	439c                	c.lw	a5,0(a5)
2005f908:	fcf42c23          	sw	a5,-40(s0)
2005f90c:	fe442783          	lw	a5,-28(s0)
2005f910:	0007d783          	lhu	a5,0(a5)
2005f914:	fcf41a23          	sh	a5,-44(s0)
2005f918:	fe442783          	lw	a5,-28(s0)
2005f91c:	0789                	c.addi	a5,2
2005f91e:	0007d783          	lhu	a5,0(a5)
2005f922:	fcf41823          	sh	a5,-48(s0)
2005f926:	fd845783          	lhu	a5,-40(s0)
2005f92a:	07f7f793          	andi	a5,a5,127
2005f92e:	01079713          	slli	a4,a5,0x10
2005f932:	8341                	c.srli	a4,0x10
2005f934:	47cd                	c.li	a5,19
2005f936:	0af71c63          	bne	a4,a5,2005f9ee <unwind_frame_rv32i+0x14a>
2005f93a:	fd845703          	lhu	a4,-40(s0)
2005f93e:	679d                	c.lui	a5,0x7
2005f940:	8ff9                	c.and	a5,a4
2005f942:	07c2                	c.slli	a5,0x10
2005f944:	83c1                	c.srli	a5,0x10
2005f946:	e7c5                	c.bnez	a5,2005f9ee <unwind_frame_rv32i+0x14a>
2005f948:	fd842703          	lw	a4,-40(s0)
2005f94c:	6785                	c.lui	a5,0x1
2005f94e:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005f952:	8f7d                	c.and	a4,a5
2005f954:	10000793          	addi	a5,zero,256
2005f958:	18f71f63          	bne	a4,a5,2005faf6 <unwind_frame_rv32i+0x252>
2005f95c:	fd842703          	lw	a4,-40(s0)
2005f960:	000f87b7          	lui	a5,0xf8
2005f964:	8f7d                	c.and	a4,a5
2005f966:	67c1                	c.lui	a5,0x10
2005f968:	18f71763          	bne	a4,a5,2005faf6 <unwind_frame_rv32i+0x252>
2005f96c:	fd842783          	lw	a5,-40(s0)
2005f970:	83d1                	c.srli	a5,0x14
2005f972:	07c2                	c.slli	a5,0x10
2005f974:	83c1                	c.srli	a5,0x10
2005f976:	0792                	c.slli	a5,0x4
2005f978:	07c2                	c.slli	a5,0x10
2005f97a:	87c1                	c.srai	a5,0x10
2005f97c:	8791                	c.srai	a5,0x4
2005f97e:	07c2                	c.slli	a5,0x10
2005f980:	87c1                	c.srai	a5,0x10
2005f982:	873e                	c.mv	a4,a5
2005f984:	6785                	c.lui	a5,0x1
2005f986:	17fd                	c.addi	a5,-1
2005f988:	00f776b3          	and	a3,a4,a5
2005f98c:	fcc45703          	lhu	a4,-52(s0)
2005f990:	77fd                	c.lui	a5,0xfffff
2005f992:	8ff9                	c.and	a5,a4
2005f994:	873e                	c.mv	a4,a5
2005f996:	87b6                	c.mv	a5,a3
2005f998:	8fd9                	c.or	a5,a4
2005f99a:	fcf41623          	sh	a5,-52(s0)
2005f99e:	fcc42783          	lw	a5,-52(s0)
2005f9a2:	0792                	c.slli	a5,0x4
2005f9a4:	07c2                	c.slli	a5,0x10
2005f9a6:	87c1                	c.srai	a5,0x10
2005f9a8:	8791                	c.srai	a5,0x4
2005f9aa:	07c2                	c.slli	a5,0x10
2005f9ac:	87c1                	c.srai	a5,0x10
2005f9ae:	fef42023          	sw	a5,-32(s0)
2005f9b2:	fe042783          	lw	a5,-32(s0)
2005f9b6:	02f05363          	bge	zero,a5,2005f9dc <unwind_frame_rv32i+0x138>
2005f9ba:	fe842783          	lw	a5,-24(s0)
2005f9be:	86be                	c.mv	a3,a5
2005f9c0:	10d00613          	addi	a2,zero,269
2005f9c4:	200647b7          	lui	a5,0x20064
2005f9c8:	d6878593          	addi	a1,a5,-664 # 20063d68 <__FUNCTION__.3>
2005f9cc:	200647b7          	lui	a5,0x20064
2005f9d0:	b6878513          	addi	a0,a5,-1176 # 20063b68 <TaskStateString.0+0x1b8>
2005f9d4:	92dff0ef          	jal	ra,2005f300 <DiagPrintf>
2005f9d8:	57fd                	c.li	a5,-1
2005f9da:	aa59                	c.j	2005fb70 <unwind_frame_rv32i+0x2cc>
2005f9dc:	fe042783          	lw	a5,-32(s0)
2005f9e0:	0007d463          	bge	a5,zero,2005f9e8 <unwind_frame_rv32i+0x144>
2005f9e4:	40f007b3          	sub	a5,zero,a5
2005f9e8:	fef42423          	sw	a5,-24(s0)
2005f9ec:	a229                	c.j	2005faf6 <unwind_frame_rv32i+0x252>
2005f9ee:	fd845783          	lhu	a5,-40(s0)
2005f9f2:	07f7f793          	andi	a5,a5,127
2005f9f6:	01079713          	slli	a4,a5,0x10
2005f9fa:	8341                	c.srli	a4,0x10
2005f9fc:	02300793          	addi	a5,zero,35
2005fa00:	0cf71463          	bne	a4,a5,2005fac8 <unwind_frame_rv32i+0x224>
2005fa04:	fd845703          	lhu	a4,-40(s0)
2005fa08:	679d                	c.lui	a5,0x7
2005fa0a:	8ff9                	c.and	a5,a4
2005fa0c:	01079713          	slli	a4,a5,0x10
2005fa10:	8341                	c.srli	a4,0x10
2005fa12:	6789                	c.lui	a5,0x2
2005fa14:	0af71a63          	bne	a4,a5,2005fac8 <unwind_frame_rv32i+0x224>
2005fa18:	fd842703          	lw	a4,-40(s0)
2005fa1c:	000f87b7          	lui	a5,0xf8
2005fa20:	8f7d                	c.and	a4,a5
2005fa22:	67c1                	c.lui	a5,0x10
2005fa24:	0cf71b63          	bne	a4,a5,2005fafa <unwind_frame_rv32i+0x256>
2005fa28:	fd842703          	lw	a4,-40(s0)
2005fa2c:	01f007b7          	lui	a5,0x1f00
2005fa30:	8f7d                	c.and	a4,a5
2005fa32:	001007b7          	lui	a5,0x100
2005fa36:	0cf71263          	bne	a4,a5,2005fafa <unwind_frame_rv32i+0x256>
2005fa3a:	fd842783          	lw	a5,-40(s0)
2005fa3e:	83e5                	c.srli	a5,0x19
2005fa40:	0ff7f793          	andi	a5,a5,255
2005fa44:	0796                	c.slli	a5,0x5
2005fa46:	01079713          	slli	a4,a5,0x10
2005fa4a:	8741                	c.srai	a4,0x10
2005fa4c:	fd842783          	lw	a5,-40(s0)
2005fa50:	839d                	c.srli	a5,0x7
2005fa52:	8bfd                	c.andi	a5,31
2005fa54:	0ff7f793          	andi	a5,a5,255
2005fa58:	07c2                	c.slli	a5,0x10
2005fa5a:	87c1                	c.srai	a5,0x10
2005fa5c:	8fd9                	c.or	a5,a4
2005fa5e:	07c2                	c.slli	a5,0x10
2005fa60:	87c1                	c.srai	a5,0x10
2005fa62:	0792                	c.slli	a5,0x4
2005fa64:	07c2                	c.slli	a5,0x10
2005fa66:	87c1                	c.srai	a5,0x10
2005fa68:	8791                	c.srai	a5,0x4
2005fa6a:	07c2                	c.slli	a5,0x10
2005fa6c:	87c1                	c.srai	a5,0x10
2005fa6e:	873e                	c.mv	a4,a5
2005fa70:	6785                	c.lui	a5,0x1
2005fa72:	17fd                	c.addi	a5,-1
2005fa74:	00f776b3          	and	a3,a4,a5
2005fa78:	fc845703          	lhu	a4,-56(s0)
2005fa7c:	77fd                	c.lui	a5,0xfffff
2005fa7e:	8ff9                	c.and	a5,a4
2005fa80:	873e                	c.mv	a4,a5
2005fa82:	87b6                	c.mv	a5,a3
2005fa84:	8fd9                	c.or	a5,a4
2005fa86:	fcf41423          	sh	a5,-56(s0)
2005fa8a:	fc842783          	lw	a5,-56(s0)
2005fa8e:	0792                	c.slli	a5,0x4
2005fa90:	07c2                	c.slli	a5,0x10
2005fa92:	87c1                	c.srai	a5,0x10
2005fa94:	8791                	c.srai	a5,0x4
2005fa96:	07c2                	c.slli	a5,0x10
2005fa98:	87c1                	c.srai	a5,0x10
2005fa9a:	fef42623          	sw	a5,-20(s0)
2005fa9e:	fec42783          	lw	a5,-20(s0)
2005faa2:	0407dc63          	bge	a5,zero,2005fafa <unwind_frame_rv32i+0x256>
2005faa6:	fec42783          	lw	a5,-20(s0)
2005faaa:	86be                	c.mv	a3,a5
2005faac:	11d00613          	addi	a2,zero,285
2005fab0:	200647b7          	lui	a5,0x20064
2005fab4:	d6878593          	addi	a1,a5,-664 # 20063d68 <__FUNCTION__.3>
2005fab8:	200647b7          	lui	a5,0x20064
2005fabc:	ba478513          	addi	a0,a5,-1116 # 20063ba4 <TaskStateString.0+0x1f4>
2005fac0:	841ff0ef          	jal	ra,2005f300 <DiagPrintf>
2005fac4:	57fd                	c.li	a5,-1
2005fac6:	a06d                	c.j	2005fb70 <unwind_frame_rv32i+0x2cc>
2005fac8:	fd842703          	lw	a4,-40(s0)
2005facc:	67a1                	c.lui	a5,0x8
2005face:	06778793          	addi	a5,a5,103 # 8067 <irq_unregister+0x41>
2005fad2:	06f70063          	beq	a4,a5,2005fb32 <unwind_frame_rv32i+0x28e>
2005fad6:	fd445703          	lhu	a4,-44(s0)
2005fada:	67a1                	c.lui	a5,0x8
2005fadc:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fae0:	00f70963          	beq	a4,a5,2005faf2 <unwind_frame_rv32i+0x24e>
2005fae4:	fd045703          	lhu	a4,-48(s0)
2005fae8:	67a1                	c.lui	a5,0x8
2005faea:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005faee:	00f71763          	bne	a4,a5,2005fafc <unwind_frame_rv32i+0x258>
2005faf2:	57fd                	c.li	a5,-1
2005faf4:	a8b5                	c.j	2005fb70 <unwind_frame_rv32i+0x2cc>
2005faf6:	0001                	c.addi	zero,0
2005faf8:	a011                	c.j	2005fafc <unwind_frame_rv32i+0x258>
2005fafa:	0001                	c.addi	zero,0
2005fafc:	fe442783          	lw	a5,-28(s0)
2005fb00:	17f1                	c.addi	a5,-4
2005fb02:	fef42223          	sw	a5,-28(s0)
2005fb06:	fb842783          	lw	a5,-72(s0)
2005fb0a:	078a                	c.slli	a5,0x2
2005fb0c:	fe442703          	lw	a4,-28(s0)
2005fb10:	973e                	c.add	a4,a5
2005fb12:	fbc42783          	lw	a5,-68(s0)
2005fb16:	43dc                	c.lw	a5,4(a5)
2005fb18:	00e7fe63          	bgeu	a5,a4,2005fb34 <unwind_frame_rv32i+0x290>
2005fb1c:	fec42783          	lw	a5,-20(s0)
2005fb20:	c781                	c.beqz	a5,2005fb28 <unwind_frame_rv32i+0x284>
2005fb22:	fe842783          	lw	a5,-24(s0)
2005fb26:	e799                	c.bnez	a5,2005fb34 <unwind_frame_rv32i+0x290>
2005fb28:	fe442783          	lw	a5,-28(s0)
2005fb2c:	da0798e3          	bne	a5,zero,2005f8dc <unwind_frame_rv32i+0x38>
2005fb30:	a011                	c.j	2005fb34 <unwind_frame_rv32i+0x290>
2005fb32:	0001                	c.addi	zero,0
2005fb34:	fec42783          	lw	a5,-20(s0)
2005fb38:	cb91                	c.beqz	a5,2005fb4c <unwind_frame_rv32i+0x2a8>
2005fb3a:	fbc42783          	lw	a5,-68(s0)
2005fb3e:	4398                	c.lw	a4,0(a5)
2005fb40:	fec42783          	lw	a5,-20(s0)
2005fb44:	97ba                	c.add	a5,a4
2005fb46:	439c                	c.lw	a5,0(a5)
2005fb48:	873e                	c.mv	a4,a5
2005fb4a:	a011                	c.j	2005fb4e <unwind_frame_rv32i+0x2aa>
2005fb4c:	4701                	c.li	a4,0
2005fb4e:	fbc42783          	lw	a5,-68(s0)
2005fb52:	c798                	c.sw	a4,8(a5)
2005fb54:	fe842703          	lw	a4,-24(s0)
2005fb58:	fbc42783          	lw	a5,-68(s0)
2005fb5c:	c7d8                	c.sw	a4,12(a5)
2005fb5e:	fec42783          	lw	a5,-20(s0)
2005fb62:	c781                	c.beqz	a5,2005fb6a <unwind_frame_rv32i+0x2c6>
2005fb64:	fe842783          	lw	a5,-24(s0)
2005fb68:	e399                	c.bnez	a5,2005fb6e <unwind_frame_rv32i+0x2ca>
2005fb6a:	57fd                	c.li	a5,-1
2005fb6c:	a011                	c.j	2005fb70 <unwind_frame_rv32i+0x2cc>
2005fb6e:	4781                	c.li	a5,0
2005fb70:	853e                	c.mv	a0,a5
2005fb72:	40b6                	c.lwsp	ra,76(sp)
2005fb74:	4426                	c.lwsp	s0,72(sp)
2005fb76:	6161                	c.addi16sp	sp,80
2005fb78:	8082                	c.jr	ra

2005fb7a <unwind_frame_rv32c>:
2005fb7a:	715d                	c.addi16sp	sp,-80
2005fb7c:	c686                	c.swsp	ra,76(sp)
2005fb7e:	c4a2                	c.swsp	s0,72(sp)
2005fb80:	0880                	c.addi4spn	s0,sp,80
2005fb82:	faa42e23          	sw	a0,-68(s0)
2005fb86:	fab42c23          	sw	a1,-72(s0)
2005fb8a:	fe042623          	sw	zero,-20(s0)
2005fb8e:	fe042423          	sw	zero,-24(s0)
2005fb92:	fbc42783          	lw	a5,-68(s0)
2005fb96:	43dc                	c.lw	a5,4(a5)
2005fb98:	cb91                	c.beqz	a5,2005fbac <unwind_frame_rv32c+0x32>
2005fb9a:	fbc42783          	lw	a5,-68(s0)
2005fb9e:	439c                	c.lw	a5,0(a5)
2005fba0:	c791                	c.beqz	a5,2005fbac <unwind_frame_rv32c+0x32>
2005fba2:	fbc42783          	lw	a5,-68(s0)
2005fba6:	43dc                	c.lw	a5,4(a5)
2005fba8:	8b85                	c.andi	a5,1
2005fbaa:	c399                	c.beqz	a5,2005fbb0 <unwind_frame_rv32c+0x36>
2005fbac:	57fd                	c.li	a5,-1
2005fbae:	ae3d                	c.j	2005feec <unwind_frame_rv32c+0x372>
2005fbb0:	fbc42783          	lw	a5,-68(s0)
2005fbb4:	43dc                	c.lw	a5,4(a5)
2005fbb6:	fef42223          	sw	a5,-28(s0)
2005fbba:	a4e1                	c.j	2005fe82 <unwind_frame_rv32c+0x308>
2005fbbc:	fe442783          	lw	a5,-28(s0)
2005fbc0:	0007d783          	lhu	a5,0(a5)
2005fbc4:	fcf41c23          	sh	a5,-40(s0)
2005fbc8:	fd845703          	lhu	a4,-40(s0)
2005fbcc:	77f9                	c.lui	a5,0xffffe
2005fbce:	8ff9                	c.and	a5,a4
2005fbd0:	07c2                	c.slli	a5,0x10
2005fbd2:	83c1                	c.srli	a5,0x10
2005fbd4:	e7e9                	c.bnez	a5,2005fc9e <unwind_frame_rv32c+0x124>
2005fbd6:	fd845783          	lhu	a5,-40(s0)
2005fbda:	8b8d                	c.andi	a5,3
2005fbdc:	01079713          	slli	a4,a5,0x10
2005fbe0:	8341                	c.srli	a4,0x10
2005fbe2:	4785                	c.li	a5,1
2005fbe4:	0af71d63          	bne	a4,a5,2005fc9e <unwind_frame_rv32c+0x124>
2005fbe8:	fd845703          	lhu	a4,-40(s0)
2005fbec:	6785                	c.lui	a5,0x1
2005fbee:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fbf2:	8ff9                	c.and	a5,a4
2005fbf4:	01079713          	slli	a4,a5,0x10
2005fbf8:	8341                	c.srli	a4,0x10
2005fbfa:	10000793          	addi	a5,zero,256
2005fbfe:	0af71063          	bne	a4,a5,2005fc9e <unwind_frame_rv32c+0x124>
2005fc02:	fd842783          	lw	a5,-40(s0)
2005fc06:	83b1                	c.srli	a5,0xc
2005fc08:	8b85                	c.andi	a5,1
2005fc0a:	0ff7f793          	andi	a5,a5,255
2005fc0e:	0796                	c.slli	a5,0x5
2005fc10:	01879713          	slli	a4,a5,0x18
2005fc14:	8761                	c.srai	a4,0x18
2005fc16:	fd842783          	lw	a5,-40(s0)
2005fc1a:	8389                	c.srli	a5,0x2
2005fc1c:	8bfd                	c.andi	a5,31
2005fc1e:	0ff7f793          	andi	a5,a5,255
2005fc22:	07e2                	c.slli	a5,0x18
2005fc24:	87e1                	c.srai	a5,0x18
2005fc26:	8fd9                	c.or	a5,a4
2005fc28:	07e2                	c.slli	a5,0x18
2005fc2a:	87e1                	c.srai	a5,0x18
2005fc2c:	078a                	c.slli	a5,0x2
2005fc2e:	07e2                	c.slli	a5,0x18
2005fc30:	87e1                	c.srai	a5,0x18
2005fc32:	8789                	c.srai	a5,0x2
2005fc34:	07e2                	c.slli	a5,0x18
2005fc36:	87e1                	c.srai	a5,0x18
2005fc38:	03f7f693          	andi	a3,a5,63
2005fc3c:	fd444783          	lbu	a5,-44(s0)
2005fc40:	fc07f793          	andi	a5,a5,-64
2005fc44:	873e                	c.mv	a4,a5
2005fc46:	87b6                	c.mv	a5,a3
2005fc48:	8fd9                	c.or	a5,a4
2005fc4a:	fcf40a23          	sb	a5,-44(s0)
2005fc4e:	fd442783          	lw	a5,-44(s0)
2005fc52:	078a                	c.slli	a5,0x2
2005fc54:	07e2                	c.slli	a5,0x18
2005fc56:	87e1                	c.srai	a5,0x18
2005fc58:	8789                	c.srai	a5,0x2
2005fc5a:	07e2                	c.slli	a5,0x18
2005fc5c:	87e1                	c.srai	a5,0x18
2005fc5e:	fef42023          	sw	a5,-32(s0)
2005fc62:	fe042783          	lw	a5,-32(s0)
2005fc66:	02f05363          	bge	zero,a5,2005fc8c <unwind_frame_rv32c+0x112>
2005fc6a:	fe842783          	lw	a5,-24(s0)
2005fc6e:	86be                	c.mv	a3,a5
2005fc70:	16900613          	addi	a2,zero,361
2005fc74:	200647b7          	lui	a5,0x20064
2005fc78:	d7c78593          	addi	a1,a5,-644 # 20063d7c <__FUNCTION__.2>
2005fc7c:	200647b7          	lui	a5,0x20064
2005fc80:	b6878513          	addi	a0,a5,-1176 # 20063b68 <TaskStateString.0+0x1b8>
2005fc84:	e7cff0ef          	jal	ra,2005f300 <DiagPrintf>
2005fc88:	57fd                	c.li	a5,-1
2005fc8a:	a48d                	c.j	2005feec <unwind_frame_rv32c+0x372>
2005fc8c:	fe042783          	lw	a5,-32(s0)
2005fc90:	0007d463          	bge	a5,zero,2005fc98 <unwind_frame_rv32c+0x11e>
2005fc94:	40f007b3          	sub	a5,zero,a5
2005fc98:	fef42423          	sw	a5,-24(s0)
2005fc9c:	aaf1                	c.j	2005fe78 <unwind_frame_rv32c+0x2fe>
2005fc9e:	fd845703          	lhu	a4,-40(s0)
2005fca2:	77f9                	c.lui	a5,0xffffe
2005fca4:	8ff9                	c.and	a5,a4
2005fca6:	01079713          	slli	a4,a5,0x10
2005fcaa:	8341                	c.srli	a4,0x10
2005fcac:	6799                	c.lui	a5,0x6
2005fcae:	10f71e63          	bne	a4,a5,2005fdca <unwind_frame_rv32c+0x250>
2005fcb2:	fd845783          	lhu	a5,-40(s0)
2005fcb6:	8b8d                	c.andi	a5,3
2005fcb8:	01079713          	slli	a4,a5,0x10
2005fcbc:	8341                	c.srli	a4,0x10
2005fcbe:	4785                	c.li	a5,1
2005fcc0:	10f71563          	bne	a4,a5,2005fdca <unwind_frame_rv32c+0x250>
2005fcc4:	fd845703          	lhu	a4,-40(s0)
2005fcc8:	6785                	c.lui	a5,0x1
2005fcca:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fcce:	8ff9                	c.and	a5,a4
2005fcd0:	01079713          	slli	a4,a5,0x10
2005fcd4:	8341                	c.srli	a4,0x10
2005fcd6:	10000793          	addi	a5,zero,256
2005fcda:	0ef71863          	bne	a4,a5,2005fdca <unwind_frame_rv32c+0x250>
2005fcde:	fd842783          	lw	a5,-40(s0)
2005fce2:	83b1                	c.srli	a5,0xc
2005fce4:	8b85                	c.andi	a5,1
2005fce6:	0ff7f793          	andi	a5,a5,255
2005fcea:	07a6                	c.slli	a5,0x9
2005fcec:	01079713          	slli	a4,a5,0x10
2005fcf0:	8741                	c.srai	a4,0x10
2005fcf2:	fd842783          	lw	a5,-40(s0)
2005fcf6:	8399                	c.srli	a5,0x6
2005fcf8:	8b85                	c.andi	a5,1
2005fcfa:	0ff7f793          	andi	a5,a5,255
2005fcfe:	0792                	c.slli	a5,0x4
2005fd00:	07c2                	c.slli	a5,0x10
2005fd02:	87c1                	c.srai	a5,0x10
2005fd04:	8fd9                	c.or	a5,a4
2005fd06:	01079713          	slli	a4,a5,0x10
2005fd0a:	8741                	c.srai	a4,0x10
2005fd0c:	fd842783          	lw	a5,-40(s0)
2005fd10:	8395                	c.srli	a5,0x5
2005fd12:	8b85                	c.andi	a5,1
2005fd14:	0ff7f793          	andi	a5,a5,255
2005fd18:	079a                	c.slli	a5,0x6
2005fd1a:	07c2                	c.slli	a5,0x10
2005fd1c:	87c1                	c.srai	a5,0x10
2005fd1e:	8fd9                	c.or	a5,a4
2005fd20:	01079713          	slli	a4,a5,0x10
2005fd24:	8741                	c.srai	a4,0x10
2005fd26:	fd842783          	lw	a5,-40(s0)
2005fd2a:	838d                	c.srli	a5,0x3
2005fd2c:	8b8d                	c.andi	a5,3
2005fd2e:	0ff7f793          	andi	a5,a5,255
2005fd32:	079e                	c.slli	a5,0x7
2005fd34:	07c2                	c.slli	a5,0x10
2005fd36:	87c1                	c.srai	a5,0x10
2005fd38:	8fd9                	c.or	a5,a4
2005fd3a:	01079713          	slli	a4,a5,0x10
2005fd3e:	8741                	c.srai	a4,0x10
2005fd40:	fd842783          	lw	a5,-40(s0)
2005fd44:	8389                	c.srli	a5,0x2
2005fd46:	8b85                	c.andi	a5,1
2005fd48:	0ff7f793          	andi	a5,a5,255
2005fd4c:	0796                	c.slli	a5,0x5
2005fd4e:	07c2                	c.slli	a5,0x10
2005fd50:	87c1                	c.srai	a5,0x10
2005fd52:	8fd9                	c.or	a5,a4
2005fd54:	07c2                	c.slli	a5,0x10
2005fd56:	87c1                	c.srai	a5,0x10
2005fd58:	079a                	c.slli	a5,0x6
2005fd5a:	07c2                	c.slli	a5,0x10
2005fd5c:	87c1                	c.srai	a5,0x10
2005fd5e:	8799                	c.srai	a5,0x6
2005fd60:	07c2                	c.slli	a5,0x10
2005fd62:	87c1                	c.srai	a5,0x10
2005fd64:	3ff7f693          	andi	a3,a5,1023
2005fd68:	fd045783          	lhu	a5,-48(s0)
2005fd6c:	c007f793          	andi	a5,a5,-1024
2005fd70:	873e                	c.mv	a4,a5
2005fd72:	87b6                	c.mv	a5,a3
2005fd74:	8fd9                	c.or	a5,a4
2005fd76:	fcf41823          	sh	a5,-48(s0)
2005fd7a:	fd042783          	lw	a5,-48(s0)
2005fd7e:	079a                	c.slli	a5,0x6
2005fd80:	07c2                	c.slli	a5,0x10
2005fd82:	87c1                	c.srai	a5,0x10
2005fd84:	8799                	c.srai	a5,0x6
2005fd86:	07c2                	c.slli	a5,0x10
2005fd88:	87c1                	c.srai	a5,0x10
2005fd8a:	fcf42e23          	sw	a5,-36(s0)
2005fd8e:	fdc42783          	lw	a5,-36(s0)
2005fd92:	02f05363          	bge	zero,a5,2005fdb8 <unwind_frame_rv32c+0x23e>
2005fd96:	fe842783          	lw	a5,-24(s0)
2005fd9a:	86be                	c.mv	a3,a5
2005fd9c:	18100613          	addi	a2,zero,385
2005fda0:	200647b7          	lui	a5,0x20064
2005fda4:	d7c78593          	addi	a1,a5,-644 # 20063d7c <__FUNCTION__.2>
2005fda8:	200647b7          	lui	a5,0x20064
2005fdac:	b6878513          	addi	a0,a5,-1176 # 20063b68 <TaskStateString.0+0x1b8>
2005fdb0:	d50ff0ef          	jal	ra,2005f300 <DiagPrintf>
2005fdb4:	57fd                	c.li	a5,-1
2005fdb6:	aa1d                	c.j	2005feec <unwind_frame_rv32c+0x372>
2005fdb8:	fdc42783          	lw	a5,-36(s0)
2005fdbc:	0007d463          	bge	a5,zero,2005fdc4 <unwind_frame_rv32c+0x24a>
2005fdc0:	40f007b3          	sub	a5,zero,a5
2005fdc4:	fef42423          	sw	a5,-24(s0)
2005fdc8:	a845                	c.j	2005fe78 <unwind_frame_rv32c+0x2fe>
2005fdca:	fd845703          	lhu	a4,-40(s0)
2005fdce:	77f9                	c.lui	a5,0xffffe
2005fdd0:	8ff9                	c.and	a5,a4
2005fdd2:	01079713          	slli	a4,a5,0x10
2005fdd6:	8341                	c.srli	a4,0x10
2005fdd8:	67b1                	c.lui	a5,0xc
2005fdda:	08f71663          	bne	a4,a5,2005fe66 <unwind_frame_rv32c+0x2ec>
2005fdde:	fd845783          	lhu	a5,-40(s0)
2005fde2:	8b8d                	c.andi	a5,3
2005fde4:	01079713          	slli	a4,a5,0x10
2005fde8:	8341                	c.srli	a4,0x10
2005fdea:	4789                	c.li	a5,2
2005fdec:	06f71d63          	bne	a4,a5,2005fe66 <unwind_frame_rv32c+0x2ec>
2005fdf0:	fd845783          	lhu	a5,-40(s0)
2005fdf4:	07c7f793          	andi	a5,a5,124
2005fdf8:	01079713          	slli	a4,a5,0x10
2005fdfc:	8341                	c.srli	a4,0x10
2005fdfe:	4791                	c.li	a5,4
2005fe00:	06f71363          	bne	a4,a5,2005fe66 <unwind_frame_rv32c+0x2ec>
2005fe04:	fd842783          	lw	a5,-40(s0)
2005fe08:	839d                	c.srli	a5,0x7
2005fe0a:	8b8d                	c.andi	a5,3
2005fe0c:	0ff7f793          	andi	a5,a5,255
2005fe10:	079a                	c.slli	a5,0x6
2005fe12:	01879713          	slli	a4,a5,0x18
2005fe16:	8761                	c.srai	a4,0x18
2005fe18:	fd842783          	lw	a5,-40(s0)
2005fe1c:	83a5                	c.srli	a5,0x9
2005fe1e:	8bbd                	c.andi	a5,15
2005fe20:	0ff7f793          	andi	a5,a5,255
2005fe24:	078a                	c.slli	a5,0x2
2005fe26:	07e2                	c.slli	a5,0x18
2005fe28:	87e1                	c.srai	a5,0x18
2005fe2a:	8fd9                	c.or	a5,a4
2005fe2c:	07e2                	c.slli	a5,0x18
2005fe2e:	87e1                	c.srai	a5,0x18
2005fe30:	fcf40623          	sb	a5,-52(s0)
2005fe34:	fcc40783          	lb	a5,-52(s0)
2005fe38:	fef42623          	sw	a5,-20(s0)
2005fe3c:	fec42783          	lw	a5,-20(s0)
2005fe40:	0207db63          	bge	a5,zero,2005fe76 <unwind_frame_rv32c+0x2fc>
2005fe44:	fec42783          	lw	a5,-20(s0)
2005fe48:	86be                	c.mv	a3,a5
2005fe4a:	19100613          	addi	a2,zero,401
2005fe4e:	200647b7          	lui	a5,0x20064
2005fe52:	d7c78593          	addi	a1,a5,-644 # 20063d7c <__FUNCTION__.2>
2005fe56:	200647b7          	lui	a5,0x20064
2005fe5a:	ba478513          	addi	a0,a5,-1116 # 20063ba4 <TaskStateString.0+0x1f4>
2005fe5e:	ca2ff0ef          	jal	ra,2005f300 <DiagPrintf>
2005fe62:	57fd                	c.li	a5,-1
2005fe64:	a061                	c.j	2005feec <unwind_frame_rv32c+0x372>
2005fe66:	fd845703          	lhu	a4,-40(s0)
2005fe6a:	67a1                	c.lui	a5,0x8
2005fe6c:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fe70:	02f70f63          	beq	a4,a5,2005feae <unwind_frame_rv32c+0x334>
2005fe74:	a011                	c.j	2005fe78 <unwind_frame_rv32c+0x2fe>
2005fe76:	0001                	c.addi	zero,0
2005fe78:	fe442783          	lw	a5,-28(s0)
2005fe7c:	17f9                	c.addi	a5,-2
2005fe7e:	fef42223          	sw	a5,-28(s0)
2005fe82:	fb842783          	lw	a5,-72(s0)
2005fe86:	0786                	c.slli	a5,0x1
2005fe88:	fe442703          	lw	a4,-28(s0)
2005fe8c:	973e                	c.add	a4,a5
2005fe8e:	fbc42783          	lw	a5,-68(s0)
2005fe92:	43dc                	c.lw	a5,4(a5)
2005fe94:	00e7fe63          	bgeu	a5,a4,2005feb0 <unwind_frame_rv32c+0x336>
2005fe98:	fec42783          	lw	a5,-20(s0)
2005fe9c:	c781                	c.beqz	a5,2005fea4 <unwind_frame_rv32c+0x32a>
2005fe9e:	fe842783          	lw	a5,-24(s0)
2005fea2:	e799                	c.bnez	a5,2005feb0 <unwind_frame_rv32c+0x336>
2005fea4:	fe442783          	lw	a5,-28(s0)
2005fea8:	d0079ae3          	bne	a5,zero,2005fbbc <unwind_frame_rv32c+0x42>
2005feac:	a011                	c.j	2005feb0 <unwind_frame_rv32c+0x336>
2005feae:	0001                	c.addi	zero,0
2005feb0:	fec42783          	lw	a5,-20(s0)
2005feb4:	cb91                	c.beqz	a5,2005fec8 <unwind_frame_rv32c+0x34e>
2005feb6:	fbc42783          	lw	a5,-68(s0)
2005feba:	4398                	c.lw	a4,0(a5)
2005febc:	fec42783          	lw	a5,-20(s0)
2005fec0:	97ba                	c.add	a5,a4
2005fec2:	439c                	c.lw	a5,0(a5)
2005fec4:	873e                	c.mv	a4,a5
2005fec6:	a011                	c.j	2005feca <unwind_frame_rv32c+0x350>
2005fec8:	4701                	c.li	a4,0
2005feca:	fbc42783          	lw	a5,-68(s0)
2005fece:	c798                	c.sw	a4,8(a5)
2005fed0:	fe842703          	lw	a4,-24(s0)
2005fed4:	fbc42783          	lw	a5,-68(s0)
2005fed8:	c7d8                	c.sw	a4,12(a5)
2005feda:	fec42783          	lw	a5,-20(s0)
2005fede:	c781                	c.beqz	a5,2005fee6 <unwind_frame_rv32c+0x36c>
2005fee0:	fe842783          	lw	a5,-24(s0)
2005fee4:	e399                	c.bnez	a5,2005feea <unwind_frame_rv32c+0x370>
2005fee6:	57fd                	c.li	a5,-1
2005fee8:	a011                	c.j	2005feec <unwind_frame_rv32c+0x372>
2005feea:	4781                	c.li	a5,0
2005feec:	853e                	c.mv	a0,a5
2005feee:	40b6                	c.lwsp	ra,76(sp)
2005fef0:	4426                	c.lwsp	s0,72(sp)
2005fef2:	6161                	c.addi16sp	sp,80
2005fef4:	8082                	c.jr	ra

2005fef6 <do_stack_backtrace>:
2005fef6:	7179                	c.addi16sp	sp,-48
2005fef8:	ce52                	c.swsp	s4,28(sp)
2005fefa:	d606                	c.swsp	ra,44(sp)
2005fefc:	d426                	c.swsp	s1,40(sp)
2005fefe:	d24a                	c.swsp	s2,36(sp)
2005ff00:	d04e                	c.swsp	s3,32(sp)
2005ff02:	cc56                	c.swsp	s5,24(sp)
2005ff04:	c402                	c.swsp	zero,8(sp)
2005ff06:	c602                	c.swsp	zero,12(sp)
2005ff08:	c02e                	c.swsp	a1,0(sp)
2005ff0a:	8a32                	c.mv	s4,a2
2005ff0c:	c525                	c.beqz	a0,2005ff74 <do_stack_backtrace+0x7e>
2005ff0e:	c1bd                	c.beqz	a1,2005ff74 <do_stack_backtrace+0x7e>
2005ff10:	c22a                	c.swsp	a0,4(sp)
2005ff12:	4712                	c.lwsp	a4,4(sp)
2005ff14:	4682                	c.lwsp	a3,0(sp)
2005ff16:	200644b7          	lui	s1,0x20064
2005ff1a:	200649b7          	lui	s3,0x20064
2005ff1e:	bdc48513          	addi	a0,s1,-1060 # 20063bdc <TaskStateString.0+0x22c>
2005ff22:	1cc00613          	addi	a2,zero,460
2005ff26:	d9098593          	addi	a1,s3,-624 # 20063d90 <__FUNCTION__.1>
2005ff2a:	bdc48493          	addi	s1,s1,-1060
2005ff2e:	bd2ff0ef          	jal	ra,2005f300 <DiagPrintf>
2005ff32:	4901                	c.li	s2,0
2005ff34:	04848493          	addi	s1,s1,72
2005ff38:	05490b63          	beq	s2,s4,2005ff8e <do_stack_backtrace+0x98>
2005ff3c:	40000593          	addi	a1,zero,1024
2005ff40:	850a                	c.mv	a0,sp
2005ff42:	963ff0ef          	jal	ra,2005f8a4 <unwind_frame_rv32i>
2005ff46:	ed15                	c.bnez	a0,2005ff82 <do_stack_backtrace+0x8c>
2005ff48:	4792                	c.lwsp	a5,4(sp)
2005ff4a:	4702                	c.lwsp	a4,0(sp)
2005ff4c:	48a2                	c.lwsp	a7,8(sp)
2005ff4e:	4832                	c.lwsp	a6,12(sp)
2005ff50:	86ca                	c.mv	a3,s2
2005ff52:	1d700613          	addi	a2,zero,471
2005ff56:	d9098593          	addi	a1,s3,-624
2005ff5a:	8526                	c.mv	a0,s1
2005ff5c:	ba4ff0ef          	jal	ra,2005f300 <DiagPrintf>
2005ff60:	47a2                	c.lwsp	a5,8(sp)
2005ff62:	4732                	c.lwsp	a4,12(sp)
2005ff64:	00190a93          	addi	s5,s2,1
2005ff68:	c23e                	c.swsp	a5,4(sp)
2005ff6a:	4782                	c.lwsp	a5,0(sp)
2005ff6c:	8956                	c.mv	s2,s5
2005ff6e:	97ba                	c.add	a5,a4
2005ff70:	c03e                	c.swsp	a5,0(sp)
2005ff72:	b7d9                	c.j	2005ff38 <do_stack_backtrace+0x42>
2005ff74:	00010793          	addi	a5,sp,0
2005ff78:	00000717          	auipc	a4,0x0
2005ff7c:	c23a                	c.swsp	a4,4(sp)
2005ff7e:	c03e                	c.swsp	a5,0(sp)
2005ff80:	bf49                	c.j	2005ff12 <do_stack_backtrace+0x1c>
2005ff82:	40000593          	addi	a1,zero,1024
2005ff86:	850a                	c.mv	a0,sp
2005ff88:	bf3ff0ef          	jal	ra,2005fb7a <unwind_frame_rv32c>
2005ff8c:	dd55                	c.beqz	a0,2005ff48 <do_stack_backtrace+0x52>
2005ff8e:	50b2                	c.lwsp	ra,44(sp)
2005ff90:	54a2                	c.lwsp	s1,40(sp)
2005ff92:	5982                	c.lwsp	s3,32(sp)
2005ff94:	4a72                	c.lwsp	s4,28(sp)
2005ff96:	4ae2                	c.lwsp	s5,24(sp)
2005ff98:	854a                	c.mv	a0,s2
2005ff9a:	5912                	c.lwsp	s2,36(sp)
2005ff9c:	6145                	c.addi16sp	sp,48
2005ff9e:	8082                	c.jr	ra

2005ffa0 <crash_dump>:
2005ffa0:	711d                	c.addi16sp	sp,-96
2005ffa2:	c8ca                	c.swsp	s2,80(sp)
2005ffa4:	c6ce                	c.swsp	s3,76(sp)
2005ffa6:	c2d6                	c.swsp	s5,68(sp)
2005ffa8:	ce86                	c.swsp	ra,92(sp)
2005ffaa:	cca2                	c.swsp	s0,88(sp)
2005ffac:	caa6                	c.swsp	s1,84(sp)
2005ffae:	c4d2                	c.swsp	s4,72(sp)
2005ffb0:	c0da                	c.swsp	s6,64(sp)
2005ffb2:	de5e                	c.swsp	s7,60(sp)
2005ffb4:	dc62                	c.swsp	s8,56(sp)
2005ffb6:	da66                	c.swsp	s9,52(sp)
2005ffb8:	d86a                	c.swsp	s10,48(sp)
2005ffba:	d66e                	c.swsp	s11,44(sp)
2005ffbc:	89aa                	c.mv	s3,a0
2005ffbe:	892e                	c.mv	s2,a1
2005ffc0:	8ab2                	c.mv	s5,a2
2005ffc2:	34002c73          	csrrs	s8,mscratch,zero
2005ffc6:	34102cf3          	csrrs	s9,mepc,zero
2005ffca:	34202a73          	csrrs	s4,mcause,zero
2005ffce:	34302bf3          	csrrs	s7,mtval,zero
2005ffd2:	20064437          	lui	s0,0x20064
2005ffd6:	da440513          	addi	a0,s0,-604 # 20063da4 <__FUNCTION__.1+0x14>
2005ffda:	200644b7          	lui	s1,0x20064
2005ffde:	11200613          	addi	a2,zero,274
2005ffe2:	54048593          	addi	a1,s1,1344 # 20064540 <__FUNCTION__.0>
2005ffe6:	0551                	c.addi	a0,20
2005ffe8:	b18ff0ef          	jal	ra,2005f300 <DiagPrintf>
2005ffec:	e941a783          	lw	a5,-364(gp) # 20066738 <crash_task_info>
2005fff0:	da440413          	addi	s0,s0,-604
2005fff4:	c399                	c.beqz	a5,2005fffa <crash_dump+0x5a>
2005fff6:	8556                	c.mv	a0,s5
2005fff8:	9782                	c.jalr	a5
2005fffa:	473d                	c.li	a4,15
2005fffc:	87a2                	c.mv	a5,s0
2005fffe:	01476a63          	bltu	a4,s4,20060012 <crash_dump+0x72>
20060002:	20064737          	lui	a4,0x20064
20060006:	002a1793          	slli	a5,s4,0x2
2006000a:	66470713          	addi	a4,a4,1636 # 20064664 <exception_code_string>
2006000e:	97ba                	c.add	a5,a4
20060010:	439c                	c.lw	a5,0(a5)
20060012:	8752                	c.mv	a4,s4
20060014:	86ce                	c.mv	a3,s3
20060016:	11700613          	addi	a2,zero,279
2006001a:	54048593          	addi	a1,s1,1344
2006001e:	05440513          	addi	a0,s0,84
20060022:	adeff0ef          	jal	ra,2005f300 <DiagPrintf>
20060026:	47bd                	c.li	a5,15
20060028:	1547f463          	bgeu	a5,s4,20060170 <crash_dump+0x1d0>
2006002c:	12b00613          	addi	a2,zero,299
20060030:	54048593          	addi	a1,s1,1344
20060034:	0a440513          	addi	a0,s0,164
20060038:	ac8ff0ef          	jal	ra,2005f300 <DiagPrintf>
2006003c:	86e2                	c.mv	a3,s8
2006003e:	12c00613          	addi	a2,zero,300
20060042:	54048593          	addi	a1,s1,1344
20060046:	0e840513          	addi	a0,s0,232
2006004a:	ab6ff0ef          	jal	ra,2005f300 <DiagPrintf>
2006004e:	86e6                	c.mv	a3,s9
20060050:	12d00613          	addi	a2,zero,301
20060054:	54048593          	addi	a1,s1,1344
20060058:	11840513          	addi	a0,s0,280
2006005c:	aa4ff0ef          	jal	ra,2005f300 <DiagPrintf>
20060060:	86d2                	c.mv	a3,s4
20060062:	12e00613          	addi	a2,zero,302
20060066:	54048593          	addi	a1,s1,1344
2006006a:	14840513          	addi	a0,s0,328
2006006e:	a92ff0ef          	jal	ra,2005f300 <DiagPrintf>
20060072:	86de                	c.mv	a3,s7
20060074:	12f00613          	addi	a2,zero,303
20060078:	54048593          	addi	a1,s1,1344
2006007c:	17840513          	addi	a0,s0,376
20060080:	20064b37          	lui	s6,0x20064
20060084:	a7cff0ef          	jal	ra,2005f300 <DiagPrintf>
20060088:	6a4b0b13          	addi	s6,s6,1700 # 200646a4 <register_string>
2006008c:	4a01                	c.li	s4,0
2006008e:	23440c13          	addi	s8,s0,564
20060092:	02000b93          	addi	s7,zero,32
20060096:	002a1793          	slli	a5,s4,0x2
2006009a:	97d6                	c.add	a5,s5
2006009c:	000b2703          	lw	a4,0(s6)
200600a0:	439c                	c.lw	a5,0(a5)
200600a2:	86d2                	c.mv	a3,s4
200600a4:	13400613          	addi	a2,zero,308
200600a8:	54048593          	addi	a1,s1,1344
200600ac:	8562                	c.mv	a0,s8
200600ae:	0a05                	c.addi	s4,1
200600b0:	a50ff0ef          	jal	ra,2005f300 <DiagPrintf>
200600b4:	0b11                	c.addi	s6,4
200600b6:	ff7a10e3          	bne	s4,s7,20060096 <crash_dump+0xf6>
200600ba:	15a00613          	addi	a2,zero,346
200600be:	54048593          	addi	a1,s1,1344
200600c2:	26440513          	addi	a0,s0,612
200600c6:	a3aff0ef          	jal	ra,2005f300 <DiagPrintf>
200600ca:	4629                	c.li	a2,10
200600cc:	85ca                	c.mv	a1,s2
200600ce:	854e                	c.mv	a0,s3
200600d0:	e27ff0ef          	jal	ra,2005fef6 <do_stack_backtrace>
200600d4:	e921                	c.bnez	a0,20060124 <crash_dump+0x184>
200600d6:	40000593          	addi	a1,zero,1024
200600da:	850a                	c.mv	a0,sp
200600dc:	c24e                	c.swsp	s3,4(sp)
200600de:	ca4e                	c.swsp	s3,20(sp)
200600e0:	c402                	c.swsp	zero,8(sp)
200600e2:	c602                	c.swsp	zero,12(sp)
200600e4:	c04a                	c.swsp	s2,0(sp)
200600e6:	cc02                	c.swsp	zero,24(sp)
200600e8:	ce02                	c.swsp	zero,28(sp)
200600ea:	c84a                	c.swsp	s2,16(sp)
200600ec:	fb8ff0ef          	jal	ra,2005f8a4 <unwind_frame_rv32i>
200600f0:	40000593          	addi	a1,zero,1024
200600f4:	0808                	c.addi4spn	a0,sp,16
200600f6:	a85ff0ef          	jal	ra,2005fb7a <unwind_frame_rv32c>
200600fa:	46b2                	c.lwsp	a3,12(sp)
200600fc:	4772                	c.lwsp	a4,28(sp)
200600fe:	89b6                	c.mv	s3,a3
20060100:	e291                	c.bnez	a3,20060104 <crash_dump+0x164>
20060102:	89ba                	c.mv	s3,a4
20060104:	16c00613          	addi	a2,zero,364
20060108:	54048593          	addi	a1,s1,1344
2006010c:	2a440513          	addi	a0,s0,676
20060110:	9f0ff0ef          	jal	ra,2005f300 <DiagPrintf>
20060114:	004aa503          	lw	a0,4(s5)
20060118:	ffc9f593          	andi	a1,s3,-4
2006011c:	4629                	c.li	a2,10
2006011e:	95ca                	c.add	a1,s2
20060120:	dd7ff0ef          	jal	ra,2005fef6 <do_stack_backtrace>
20060124:	54048593          	addi	a1,s1,1344
20060128:	17000613          	addi	a2,zero,368
2006012c:	30c40513          	addi	a0,s0,780
20060130:	9d0ff0ef          	jal	ra,2005f300 <DiagPrintf>
20060134:	54048593          	addi	a1,s1,1344
20060138:	18300613          	addi	a2,zero,387
2006013c:	35440513          	addi	a0,s0,852
20060140:	9c0ff0ef          	jal	ra,2005f300 <DiagPrintf>
20060144:	39c40513          	addi	a0,s0,924
20060148:	4466                	c.lwsp	s0,88(sp)
2006014a:	40f6                	c.lwsp	ra,92(sp)
2006014c:	4946                	c.lwsp	s2,80(sp)
2006014e:	49b6                	c.lwsp	s3,76(sp)
20060150:	4a26                	c.lwsp	s4,72(sp)
20060152:	4a96                	c.lwsp	s5,68(sp)
20060154:	4b06                	c.lwsp	s6,64(sp)
20060156:	5bf2                	c.lwsp	s7,60(sp)
20060158:	5c62                	c.lwsp	s8,56(sp)
2006015a:	5cd2                	c.lwsp	s9,52(sp)
2006015c:	5d42                	c.lwsp	s10,48(sp)
2006015e:	5db2                	c.lwsp	s11,44(sp)
20060160:	54048593          	addi	a1,s1,1344
20060164:	44d6                	c.lwsp	s1,84(sp)
20060166:	18400613          	addi	a2,zero,388
2006016a:	6125                	c.addi16sp	sp,96
2006016c:	994ff06f          	jal	zero,2005f300 <DiagPrintf>
20060170:	20064737          	lui	a4,0x20064
20060174:	002a1793          	slli	a5,s4,0x2
20060178:	62470713          	addi	a4,a4,1572 # 20064624 <exception_cause_string_tbl>
2006017c:	97ba                	c.add	a5,a4
2006017e:	0007ab03          	lw	s6,0(a5)
20060182:	ea0b05e3          	beq	s6,zero,2006002c <crash_dump+0x8c>
20060186:	11d00613          	addi	a2,zero,285
2006018a:	54048593          	addi	a1,s1,1344
2006018e:	1a840513          	addi	a0,s0,424
20060192:	96eff0ef          	jal	ra,2005f300 <DiagPrintf>
20060196:	21040d13          	addi	s10,s0,528
2006019a:	1e040d93          	addi	s11,s0,480
2006019e:	000b2683          	lw	a3,0(s6)
200601a2:	e80685e3          	beq	a3,zero,2006002c <crash_dump+0x8c>
200601a6:	004b2703          	lw	a4,4(s6)
200601aa:	cb19                	c.beqz	a4,200601c0 <crash_dump+0x220>
200601ac:	87de                	c.mv	a5,s7
200601ae:	12000613          	addi	a2,zero,288
200601b2:	54048593          	addi	a1,s1,1344
200601b6:	856e                	c.mv	a0,s11
200601b8:	948ff0ef          	jal	ra,2005f300 <DiagPrintf>
200601bc:	0b21                	c.addi	s6,8
200601be:	b7c5                	c.j	2006019e <crash_dump+0x1fe>
200601c0:	12300613          	addi	a2,zero,291
200601c4:	54048593          	addi	a1,s1,1344
200601c8:	856a                	c.mv	a0,s10
200601ca:	936ff0ef          	jal	ra,2005f300 <DiagPrintf>
200601ce:	b7fd                	c.j	200601bc <crash_dump+0x21c>

200601d0 <_my_random>:
200601d0:	1141                	c.addi	sp,-16
200601d2:	852e                	c.mv	a0,a1
200601d4:	85b2                	c.mv	a1,a2
200601d6:	c606                	c.swsp	ra,12(sp)
200601d8:	db6ff0ef          	jal	ra,2005f78e <TRNG_get_random_bytes>
200601dc:	40b2                	c.lwsp	ra,12(sp)
200601de:	4501                	c.li	a0,0
200601e0:	0141                	c.addi	sp,16
200601e2:	8082                	c.jr	ra

200601e4 <_my_free>:
200601e4:	ceafb06f          	jal	zero,2005b6ce <rtos_mem_free>

200601e8 <_my_calloc>:
200601e8:	02b50533          	mul	a0,a0,a1
200601ec:	cbcfb06f          	jal	zero,2005b6a8 <rtos_mem_zmalloc>

200601f0 <wifi_set_rom2flash>:
200601f0:	200697b7          	lui	a5,0x20069
200601f4:	82478793          	addi	a5,a5,-2012 # 20068824 <wifi_rom_func_map>
200601f8:	fef1a623          	sw	a5,-20(gp) # 20066890 <p_wifi_rom_func_map>
200601fc:	20060737          	lui	a4,0x20060
20060200:	1e870713          	addi	a4,a4,488 # 200601e8 <_my_calloc>
20060204:	c398                	c.sw	a4,0(a5)
20060206:	20060737          	lui	a4,0x20060
2006020a:	1e470713          	addi	a4,a4,484 # 200601e4 <_my_free>
2006020e:	c3d8                	c.sw	a4,4(a5)
20060210:	20060737          	lui	a4,0x20060
20060214:	1d070713          	addi	a4,a4,464 # 200601d0 <_my_random>
20060218:	c798                	c.sw	a4,8(a5)
2006021a:	200687b7          	lui	a5,0x20068
2006021e:	6e078793          	addi	a5,a5,1760 # 200686e0 <wifi_rom2flash>
20060222:	fef1a423          	sw	a5,-24(gp) # 2006688c <p_wifi_rom2flash>
20060226:	2005b737          	lui	a4,0x2005b
2006022a:	7e470713          	addi	a4,a4,2020 # 2005b7e4 <rtos_mutex_give>
2006022e:	c3d8                	c.sw	a4,4(a5)
20060230:	2005b737          	lui	a4,0x2005b
20060234:	79870713          	addi	a4,a4,1944 # 2005b798 <rtos_mutex_take>
20060238:	c398                	c.sw	a4,0(a5)
2006023a:	2005b737          	lui	a4,0x2005b
2006023e:	77a70713          	addi	a4,a4,1914 # 2005b77a <rtos_mutex_delete_static>
20060242:	d798                	c.sw	a4,40(a5)
20060244:	2005b737          	lui	a4,0x2005b
20060248:	70070713          	addi	a4,a4,1792 # 2005b700 <rtos_mutex_create_static>
2006024c:	d7d8                	c.sw	a4,44(a5)
2006024e:	2005c737          	lui	a4,0x2005c
20060252:	91e70713          	addi	a4,a4,-1762 # 2005b91e <rtos_sema_give>
20060256:	c7d8                	c.sw	a4,12(a5)
20060258:	2005c737          	lui	a4,0x2005c
2006025c:	8d270713          	addi	a4,a4,-1838 # 2005b8d2 <rtos_sema_take>
20060260:	cb98                	c.sw	a4,16(a5)
20060262:	2005b737          	lui	a4,0x2005b
20060266:	65870713          	addi	a4,a4,1624 # 2005b658 <rtos_critical_enter>
2006026a:	cbd8                	c.sw	a4,20(a5)
2006026c:	2005b737          	lui	a4,0x2005b
20060270:	67c70713          	addi	a4,a4,1660 # 2005b67c <rtos_critical_exit>
20060274:	cf98                	c.sw	a4,24(a5)
20060276:	2005c737          	lui	a4,0x2005c
2006027a:	ba870713          	addi	a4,a4,-1112 # 2005bba8 <rtos_time_delay_ms>
2006027e:	d3d8                	c.sw	a4,36(a5)
20060280:	2005c737          	lui	a4,0x2005c
20060284:	be270713          	addi	a4,a4,-1054 # 2005bbe2 <rtos_time_get_current_system_time_ms>
20060288:	c798                	c.sw	a4,8(a5)
2006028a:	20061737          	lui	a4,0x20061
2006028e:	2fc70713          	addi	a4,a4,764 # 200612fc <rtw_init_timer>
20060292:	d7b8                	c.sw	a4,104(a5)
20060294:	20061737          	lui	a4,0x20061
20060298:	30670713          	addi	a4,a4,774 # 20061306 <rtw_set_timer>
2006029c:	d7f8                	c.sw	a4,108(a5)
2006029e:	20061737          	lui	a4,0x20061
200602a2:	30a70713          	addi	a4,a4,778 # 2006130a <rtw_cancel_timer>
200602a6:	dbb8                	c.sw	a4,112(a5)
200602a8:	20061737          	lui	a4,0x20061
200602ac:	31a70713          	addi	a4,a4,794 # 2006131a <rtw_del_timer>
200602b0:	dbf8                	c.sw	a4,116(a5)
200602b2:	20059737          	lui	a4,0x20059
200602b6:	71a70713          	addi	a4,a4,1818 # 2005971a <pmu_set_sysactive_time>
200602ba:	dff8                	c.sw	a4,124(a5)
200602bc:	20061737          	lui	a4,0x20061
200602c0:	35270713          	addi	a4,a4,850 # 20061352 <rtw_wakelock_timeout>
200602c4:	08e7a023          	sw	a4,128(a5)
200602c8:	20061737          	lui	a4,0x20061
200602cc:	31e70713          	addi	a4,a4,798 # 2006131e <rtw_acquire_wakelock>
200602d0:	db98                	c.sw	a4,48(a5)
200602d2:	20061737          	lui	a4,0x20061
200602d6:	33870713          	addi	a4,a4,824 # 20061338 <rtw_release_wakelock>
200602da:	dbd8                	c.sw	a4,52(a5)
200602dc:	8082                	c.jr	ra

200602de <wifi_on>:
200602de:	1101                	c.addi	sp,-32
200602e0:	ce06                	c.swsp	ra,28(sp)
200602e2:	cc22                	c.swsp	s0,24(sp)
200602e4:	ca26                	c.swsp	s1,20(sp)
200602e6:	842a                	c.mv	s0,a0
200602e8:	c84a                	c.swsp	s2,16(sp)
200602ea:	979f90ef          	jal	ra,20059c62 <wifi_set_user_config>
200602ee:	389000ef          	jal	ra,20060e76 <inic_msg_q_init>
200602f2:	41c18793          	addi	a5,gp,1052 # 20066cc0 <wifi_user_config>
200602f6:	7c102773          	csrrs	a4,0x7c1,zero
200602fa:	10077713          	andi	a4,a4,256
200602fe:	c715                	c.beqz	a4,2006032a <wifi_on+0x4c>
20060300:	0ff0000f          	fence	iorw,iorw
20060304:	04f7f00b          	cache	dwb,(a5)
20060308:	02078713          	addi	a4,a5,32
2006030c:	04e7700b          	cache	dwb,(a4)
20060310:	0ff0000f          	fence	iorw,iorw
20060314:	0001                	c.addi	zero,0
20060316:	0001                	c.addi	zero,0
20060318:	0001                	c.addi	zero,0
2006031a:	0001                	c.addi	zero,0
2006031c:	0001                	c.addi	zero,0
2006031e:	0001                	c.addi	zero,0
20060320:	0001                	c.addi	zero,0
20060322:	0001                	c.addi	zero,0
20060324:	0001                	c.addi	zero,0
20060326:	0001                	c.addi	zero,0
20060328:	0001                	c.addi	zero,0
2006032a:	4605                	c.li	a2,1
2006032c:	006c                	c.addi4spn	a1,sp,12
2006032e:	451d                	c.li	a0,7
20060330:	c63e                	c.swsp	a5,12(sp)
20060332:	292d                	c.jal	2006076c <inic_api_host_message_send>
20060334:	04d000ef          	jal	ra,20060b80 <inic_host_init_skb>
20060338:	4605                	c.li	a2,1
2006033a:	006c                	c.addi4spn	a1,sp,12
2006033c:	4525                	c.li	a0,9
2006033e:	c622                	c.swsp	s0,12(sp)
20060340:	2135                	c.jal	2006076c <inic_api_host_message_send>
20060342:	fd018793          	addi	a5,gp,-48 # 20066874 <wifi_boot.0>
20060346:	4398                	c.lw	a4,0(a5)
20060348:	84aa                	c.mv	s1,a0
2006034a:	ef01                	c.bnez	a4,20060362 <wifi_on+0x84>
2006034c:	4905                	c.li	s2,1
2006034e:	0127a023          	sw	s2,0(a5)
20060352:	5ad000ef          	jal	ra,200610fe <init_timer_wrapper>
20060356:	fbc1a783          	lw	a5,-68(gp) # 20066860 <p_wifi_do_fast_connect>
2006035a:	c781                	c.beqz	a5,20060362 <wifi_on+0x84>
2006035c:	01241363          	bne	s0,s2,20060362 <wifi_on+0x84>
20060360:	9782                	c.jalr	a5
20060362:	40f2                	c.lwsp	ra,28(sp)
20060364:	4462                	c.lwsp	s0,24(sp)
20060366:	4942                	c.lwsp	s2,16(sp)
20060368:	8526                	c.mv	a0,s1
2006036a:	44d2                	c.lwsp	s1,20(sp)
2006036c:	6105                	c.addi16sp	sp,32
2006036e:	8082                	c.jr	ra

20060370 <wifi_config_autoreconnect>:
20060370:	557d                	c.li	a0,-1
20060372:	8082                	c.jr	ra

20060374 <_inic_ipc_ip_addr_update_in_wowlan>:
20060374:	4505                	c.li	a0,1
20060376:	8082                	c.jr	ra

20060378 <inic_api_host_task_h>:
20060378:	7179                	c.addi16sp	sp,-48
2006037a:	d226                	c.swsp	s1,36(sp)
2006037c:	200644b7          	lui	s1,0x20064
20060380:	d04a                	c.swsp	s2,32(sp)
20060382:	ce4e                	c.swsp	s3,28(sp)
20060384:	cc52                	c.swsp	s4,24(sp)
20060386:	ca56                	c.swsp	s5,20(sp)
20060388:	c85a                	c.swsp	s6,16(sp)
2006038a:	c65e                	c.swsp	s7,12(sp)
2006038c:	d606                	c.swsp	ra,44(sp)
2006038e:	d422                	c.swsp	s0,40(sp)
20060390:	20064a37          	lui	s4,0x20064
20060394:	75848493          	addi	s1,s1,1880 # 20064758 <register_string+0xb4>
20060398:	fdc1a503          	lw	a0,-36(gp) # 20066880 <g_host_inic_api_task_wake_sema>
2006039c:	55fd                	c.li	a1,-1
2006039e:	d34fb0ef          	jal	ra,2005b8d2 <rtos_sema_take>
200603a2:	459d                	c.li	a1,7
200603a4:	4541                	c.li	a0,16
200603a6:	d4bf80ef          	jal	ra,200590f0 <ipc_get_message>
200603aa:	4140                	c.lw	s0,4(a0)
200603ac:	7c1027f3          	csrrs	a5,0x7c1,zero
200603b0:	1007f793          	andi	a5,a5,256
200603b4:	c3b9                	c.beqz	a5,200603fa <inic_api_host_task_h+0x82>
200603b6:	01f47713          	andi	a4,s0,31
200603ba:	cb21                	c.beqz	a4,2006040a <inic_api_host_task_h+0x92>
200603bc:	04070693          	addi	a3,a4,64
200603c0:	fe047793          	andi	a5,s0,-32
200603c4:	01f7f713          	andi	a4,a5,31
200603c8:	9736                	c.add	a4,a3
200603ca:	0ff0000f          	fence	iorw,iorw
200603ce:	973e                	c.add	a4,a5
200603d0:	00f7f00b          	cache	dinv,(a5)
200603d4:	02078793          	addi	a5,a5,32
200603d8:	40f706b3          	sub	a3,a4,a5
200603dc:	fed04ae3          	blt	zero,a3,200603d0 <inic_api_host_task_h+0x58>
200603e0:	0ff0000f          	fence	iorw,iorw
200603e4:	0001                	c.addi	zero,0
200603e6:	0001                	c.addi	zero,0
200603e8:	0001                	c.addi	zero,0
200603ea:	0001                	c.addi	zero,0
200603ec:	0001                	c.addi	zero,0
200603ee:	0001                	c.addi	zero,0
200603f0:	0001                	c.addi	zero,0
200603f2:	0001                	c.addi	zero,0
200603f4:	0001                	c.addi	zero,0
200603f6:	0001                	c.addi	zero,0
200603f8:	0001                	c.addi	zero,0
200603fa:	ec01                	c.bnez	s0,20060412 <inic_api_host_task_h+0x9a>
200603fc:	20064537          	lui	a0,0x20064
20060400:	72450513          	addi	a0,a0,1828 # 20064724 <register_string+0x80>
20060404:	f1dfe0ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20060408:	bf41                	c.j	20060398 <inic_api_host_task_h+0x20>
2006040a:	87a2                	c.mv	a5,s0
2006040c:	04000693          	addi	a3,zero,64
20060410:	bf55                	c.j	200603c4 <inic_api_host_task_h+0x4c>
20060412:	400c                	c.lw	a1,0(s0)
20060414:	4721                	c.li	a4,8
20060416:	fff58793          	addi	a5,a1,-1
2006041a:	34f76063          	bltu	a4,a5,2006075a <inic_api_host_task_h+0x3e2>
2006041e:	078a                	c.slli	a5,0x2
20060420:	97a6                	c.add	a5,s1
20060422:	439c                	c.lw	a5,0(a5)
20060424:	8782                	c.jr	a5
20060426:	fcc1a783          	lw	a5,-52(gp) # 20066870 <scan_user_callback_ptr>
2006042a:	c3ad                	c.beqz	a5,2006048c <inic_api_host_task_h+0x114>
2006042c:	440c                	c.lw	a1,8(s0)
2006042e:	4048                	c.lw	a0,4(s0)
20060430:	9782                	c.jalr	a5
20060432:	a8a9                	c.j	2006048c <inic_api_host_task_h+0x114>
20060434:	4048                	c.lw	a0,4(s0)
20060436:	440c                	c.lw	a1,8(s0)
20060438:	7c1027f3          	csrrs	a5,0x7c1,zero
2006043c:	1007f793          	andi	a5,a5,256
20060440:	c3b9                	c.beqz	a5,20060486 <inic_api_host_task_h+0x10e>
20060442:	01f57713          	andi	a4,a0,31
20060446:	cf59                	c.beqz	a4,200604e4 <inic_api_host_task_h+0x16c>
20060448:	03e70693          	addi	a3,a4,62
2006044c:	fe057793          	andi	a5,a0,-32
20060450:	01f7f713          	andi	a4,a5,31
20060454:	9736                	c.add	a4,a3
20060456:	0ff0000f          	fence	iorw,iorw
2006045a:	973e                	c.add	a4,a5
2006045c:	00f7f00b          	cache	dinv,(a5)
20060460:	02078793          	addi	a5,a5,32
20060464:	40f706b3          	sub	a3,a4,a5
20060468:	fed04ae3          	blt	zero,a3,2006045c <inic_api_host_task_h+0xe4>
2006046c:	0ff0000f          	fence	iorw,iorw
20060470:	0001                	c.addi	zero,0
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
20060486:	fc81a783          	lw	a5,-56(gp) # 2006686c <scan_each_report_user_callback_ptr>
2006048a:	f3dd                	c.bnez	a5,20060430 <inic_api_host_task_h+0xb8>
2006048c:	00042023          	sw	zero,0(s0)
20060490:	7c1027f3          	csrrs	a5,0x7c1,zero
20060494:	1007f793          	andi	a5,a5,256
20060498:	f00780e3          	beq	a5,zero,20060398 <inic_api_host_task_h+0x20>
2006049c:	01f47793          	andi	a5,s0,31
200604a0:	04000713          	addi	a4,zero,64
200604a4:	c781                	c.beqz	a5,200604ac <inic_api_host_task_h+0x134>
200604a6:	04078713          	addi	a4,a5,64
200604aa:	9801                	c.andi	s0,-32
200604ac:	01f47793          	andi	a5,s0,31
200604b0:	97ba                	c.add	a5,a4
200604b2:	0ff0000f          	fence	iorw,iorw
200604b6:	97a2                	c.add	a5,s0
200604b8:	0484700b          	cache	dwb,(s0)
200604bc:	02040413          	addi	s0,s0,32
200604c0:	40878733          	sub	a4,a5,s0
200604c4:	fee04ae3          	blt	zero,a4,200604b8 <inic_api_host_task_h+0x140>
200604c8:	0ff0000f          	fence	iorw,iorw
200604cc:	0001                	c.addi	zero,0
200604ce:	0001                	c.addi	zero,0
200604d0:	0001                	c.addi	zero,0
200604d2:	0001                	c.addi	zero,0
200604d4:	0001                	c.addi	zero,0
200604d6:	0001                	c.addi	zero,0
200604d8:	0001                	c.addi	zero,0
200604da:	0001                	c.addi	zero,0
200604dc:	0001                	c.addi	zero,0
200604de:	0001                	c.addi	zero,0
200604e0:	0001                	c.addi	zero,0
200604e2:	bd5d                	c.j	20060398 <inic_api_host_task_h+0x20>
200604e4:	87aa                	c.mv	a5,a0
200604e6:	03e00693          	addi	a3,zero,62
200604ea:	b79d                	c.j	20060450 <inic_api_host_task_h+0xd8>
200604ec:	4418                	c.lw	a4,8(s0)
200604ee:	4448                	c.lw	a0,12(s0)
200604f0:	481c                	c.lw	a5,16(s0)
200604f2:	4850                	c.lw	a2,20(s0)
200604f4:	5014                	c.lw	a3,32(s0)
200604f6:	7c1025f3          	csrrs	a1,0x7c1,zero
200604fa:	1005f593          	andi	a1,a1,256
200604fe:	c58d                	c.beqz	a1,20060528 <inic_api_host_task_h+0x1b0>
20060500:	00a775b3          	and	a1,a4,a0
20060504:	587d                	c.li	a6,-1
20060506:	0b059363          	bne	a1,a6,200605ac <inic_api_host_task_h+0x234>
2006050a:	7c016773          	csrrsi	a4,0x7c0,2
2006050e:	0ff0000f          	fence	iorw,iorw
20060512:	0001                	c.addi	zero,0
20060514:	0001                	c.addi	zero,0
20060516:	0001                	c.addi	zero,0
20060518:	0001                	c.addi	zero,0
2006051a:	0001                	c.addi	zero,0
2006051c:	0001                	c.addi	zero,0
2006051e:	0001                	c.addi	zero,0
20060520:	0001                	c.addi	zero,0
20060522:	0001                	c.addi	zero,0
20060524:	0001                	c.addi	zero,0
20060526:	0001                	c.addi	zero,0
20060528:	7c102773          	csrrs	a4,0x7c1,zero
2006052c:	10077713          	andi	a4,a4,256
20060530:	c70d                	c.beqz	a4,2006055a <inic_api_host_task_h+0x1e2>
20060532:	00c7f733          	and	a4,a5,a2
20060536:	55fd                	c.li	a1,-1
20060538:	0ab71063          	bne	a4,a1,200605d8 <inic_api_host_task_h+0x260>
2006053c:	7c0167f3          	csrrsi	a5,0x7c0,2
20060540:	0ff0000f          	fence	iorw,iorw
20060544:	0001                	c.addi	zero,0
20060546:	0001                	c.addi	zero,0
20060548:	0001                	c.addi	zero,0
2006054a:	0001                	c.addi	zero,0
2006054c:	0001                	c.addi	zero,0
2006054e:	0001                	c.addi	zero,0
20060550:	0001                	c.addi	zero,0
20060552:	0001                	c.addi	zero,0
20060554:	0001                	c.addi	zero,0
20060556:	0001                	c.addi	zero,0
20060558:	0001                	c.addi	zero,0
2006055a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006055e:	1007f793          	andi	a5,a5,256
20060562:	d78d                	c.beqz	a5,2006048c <inic_api_host_task_h+0x114>
20060564:	01f6f713          	andi	a4,a3,31
20060568:	4799                	c.li	a5,6
2006056a:	c701                	c.beqz	a4,20060572 <inic_api_host_task_h+0x1fa>
2006056c:	00670793          	addi	a5,a4,6
20060570:	9a81                	c.andi	a3,-32
20060572:	01f6f713          	andi	a4,a3,31
20060576:	973e                	c.add	a4,a5
20060578:	0ff0000f          	fence	iorw,iorw
2006057c:	87b6                	c.mv	a5,a3
2006057e:	9736                	c.add	a4,a3
20060580:	00f7f00b          	cache	dinv,(a5)
20060584:	02078793          	addi	a5,a5,32
20060588:	40f706b3          	sub	a3,a4,a5
2006058c:	fed04ae3          	blt	zero,a3,20060580 <inic_api_host_task_h+0x208>
20060590:	0ff0000f          	fence	iorw,iorw
20060594:	0001                	c.addi	zero,0
20060596:	0001                	c.addi	zero,0
20060598:	0001                	c.addi	zero,0
2006059a:	0001                	c.addi	zero,0
2006059c:	0001                	c.addi	zero,0
2006059e:	0001                	c.addi	zero,0
200605a0:	0001                	c.addi	zero,0
200605a2:	0001                	c.addi	zero,0
200605a4:	0001                	c.addi	zero,0
200605a6:	0001                	c.addi	zero,0
200605a8:	0001                	c.addi	zero,0
200605aa:	b5cd                	c.j	2006048c <inic_api_host_task_h+0x114>
200605ac:	01f77593          	andi	a1,a4,31
200605b0:	c199                	c.beqz	a1,200605b6 <inic_api_host_task_h+0x23e>
200605b2:	9b01                	c.andi	a4,-32
200605b4:	952e                	c.add	a0,a1
200605b6:	f6a059e3          	bge	zero,a0,20060528 <inic_api_host_task_h+0x1b0>
200605ba:	01f77593          	andi	a1,a4,31
200605be:	95aa                	c.add	a1,a0
200605c0:	0ff0000f          	fence	iorw,iorw
200605c4:	95ba                	c.add	a1,a4
200605c6:	00e7700b          	cache	dinv,(a4)
200605ca:	02070713          	addi	a4,a4,32
200605ce:	40e58533          	sub	a0,a1,a4
200605d2:	fea04ae3          	blt	zero,a0,200605c6 <inic_api_host_task_h+0x24e>
200605d6:	bf25                	c.j	2006050e <inic_api_host_task_h+0x196>
200605d8:	01f7f713          	andi	a4,a5,31
200605dc:	c319                	c.beqz	a4,200605e2 <inic_api_host_task_h+0x26a>
200605de:	9b81                	c.andi	a5,-32
200605e0:	963a                	c.add	a2,a4
200605e2:	f6c05ce3          	bge	zero,a2,2006055a <inic_api_host_task_h+0x1e2>
200605e6:	01f7f713          	andi	a4,a5,31
200605ea:	9732                	c.add	a4,a2
200605ec:	0ff0000f          	fence	iorw,iorw
200605f0:	973e                	c.add	a4,a5
200605f2:	00f7f00b          	cache	dinv,(a5)
200605f6:	02078793          	addi	a5,a5,32
200605fa:	40f70633          	sub	a2,a4,a5
200605fe:	fec04ae3          	blt	zero,a2,200605f2 <inic_api_host_task_h+0x27a>
20060602:	bf3d                	c.j	20060540 <inic_api_host_task_h+0x1c8>
20060604:	fd41a783          	lw	a5,-44(gp) # 20066878 <p_ap_channel_switch_callback>
20060608:	e80782e3          	beq	a5,zero,2006048c <inic_api_host_task_h+0x114>
2006060c:	00840583          	lb	a1,8(s0)
20060610:	00444503          	lbu	a0,4(s0)
20060614:	9782                	c.jalr	a5
20060616:	bd9d                	c.j	2006048c <inic_api_host_task_h+0x114>
20060618:	440c                	c.lw	a1,8(s0)
2006061a:	4450                	c.lw	a2,12(s0)
2006061c:	00444503          	lbu	a0,4(s0)
20060620:	4814                	c.lw	a3,16(s0)
20060622:	87ae                	c.mv	a5,a1
20060624:	8832                	c.mv	a6,a2
20060626:	7c102773          	csrrs	a4,0x7c1,zero
2006062a:	10077713          	andi	a4,a4,256
2006062e:	c70d                	c.beqz	a4,20060658 <inic_api_host_task_h+0x2e0>
20060630:	00c5f733          	and	a4,a1,a2
20060634:	58fd                	c.li	a7,-1
20060636:	03171463          	bne	a4,a7,2006065e <inic_api_host_task_h+0x2e6>
2006063a:	7c0167f3          	csrrsi	a5,0x7c0,2
2006063e:	0ff0000f          	fence	iorw,iorw
20060642:	0001                	c.addi	zero,0
20060644:	0001                	c.addi	zero,0
20060646:	0001                	c.addi	zero,0
20060648:	0001                	c.addi	zero,0
2006064a:	0001                	c.addi	zero,0
2006064c:	0001                	c.addi	zero,0
2006064e:	0001                	c.addi	zero,0
20060650:	0001                	c.addi	zero,0
20060652:	0001                	c.addi	zero,0
20060654:	0001                	c.addi	zero,0
20060656:	0001                	c.addi	zero,0
20060658:	ad4fe0ef          	jal	ra,2005e92c <wifi_indication>
2006065c:	bd05                	c.j	2006048c <inic_api_host_task_h+0x114>
2006065e:	01f5f713          	andi	a4,a1,31
20060662:	c709                	c.beqz	a4,2006066c <inic_api_host_task_h+0x2f4>
20060664:	fe05f793          	andi	a5,a1,-32
20060668:	00e60833          	add	a6,a2,a4
2006066c:	ff0056e3          	bge	zero,a6,20060658 <inic_api_host_task_h+0x2e0>
20060670:	01f7f713          	andi	a4,a5,31
20060674:	9742                	c.add	a4,a6
20060676:	0ff0000f          	fence	iorw,iorw
2006067a:	973e                	c.add	a4,a5
2006067c:	00f7f00b          	cache	dinv,(a5)
20060680:	02078793          	addi	a5,a5,32
20060684:	40f70833          	sub	a6,a4,a5
20060688:	ff004ae3          	blt	zero,a6,2006067c <inic_api_host_task_h+0x304>
2006068c:	bf4d                	c.j	2006063e <inic_api_host_task_h+0x2c6>
2006068e:	fc41a783          	lw	a5,-60(gp) # 20066868 <promisc_user_callback_ptr>
20060692:	4501                	c.li	a0,0
20060694:	cbc1                	c.beqz	a5,20060724 <inic_api_host_task_h+0x3ac>
20060696:	4058                	c.lw	a4,4(s0)
20060698:	7c1027f3          	csrrs	a5,0x7c1,zero
2006069c:	1007f793          	andi	a5,a5,256
200606a0:	c3b9                	c.beqz	a5,200606e6 <inic_api_host_task_h+0x36e>
200606a2:	01f77693          	andi	a3,a4,31
200606a6:	c2c9                	c.beqz	a3,20060728 <inic_api_host_task_h+0x3b0>
200606a8:	01068613          	addi	a2,a3,16
200606ac:	fe077793          	andi	a5,a4,-32
200606b0:	01f7f693          	andi	a3,a5,31
200606b4:	96b2                	c.add	a3,a2
200606b6:	0ff0000f          	fence	iorw,iorw
200606ba:	96be                	c.add	a3,a5
200606bc:	00f7f00b          	cache	dinv,(a5)
200606c0:	02078793          	addi	a5,a5,32
200606c4:	40f68633          	sub	a2,a3,a5
200606c8:	fec04ae3          	blt	zero,a2,200606bc <inic_api_host_task_h+0x344>
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
200606e6:	471c                	c.lw	a5,8(a4)
200606e8:	4754                	c.lw	a3,12(a4)
200606ea:	7c102773          	csrrs	a4,0x7c1,zero
200606ee:	10077713          	andi	a4,a4,256
200606f2:	c70d                	c.beqz	a4,2006071c <inic_api_host_task_h+0x3a4>
200606f4:	577d                	c.li	a4,-1
200606f6:	02e79c63          	bne	a5,a4,2006072e <inic_api_host_task_h+0x3b6>
200606fa:	02f69a63          	bne	a3,a5,2006072e <inic_api_host_task_h+0x3b6>
200606fe:	7c0167f3          	csrrsi	a5,0x7c0,2
20060702:	0ff0000f          	fence	iorw,iorw
20060706:	0001                	c.addi	zero,0
20060708:	0001                	c.addi	zero,0
2006070a:	0001                	c.addi	zero,0
2006070c:	0001                	c.addi	zero,0
2006070e:	0001                	c.addi	zero,0
20060710:	0001                	c.addi	zero,0
20060712:	0001                	c.addi	zero,0
20060714:	0001                	c.addi	zero,0
20060716:	0001                	c.addi	zero,0
20060718:	0001                	c.addi	zero,0
2006071a:	0001                	c.addi	zero,0
2006071c:	fc41a783          	lw	a5,-60(gp) # 20066868 <promisc_user_callback_ptr>
20060720:	4048                	c.lw	a0,4(s0)
20060722:	9782                	c.jalr	a5
20060724:	d048                	c.sw	a0,36(s0)
20060726:	b39d                	c.j	2006048c <inic_api_host_task_h+0x114>
20060728:	87ba                	c.mv	a5,a4
2006072a:	4641                	c.li	a2,16
2006072c:	b751                	c.j	200606b0 <inic_api_host_task_h+0x338>
2006072e:	01f7f713          	andi	a4,a5,31
20060732:	c319                	c.beqz	a4,20060738 <inic_api_host_task_h+0x3c0>
20060734:	9b81                	c.andi	a5,-32
20060736:	96ba                	c.add	a3,a4
20060738:	fed052e3          	bge	zero,a3,2006071c <inic_api_host_task_h+0x3a4>
2006073c:	01f7f713          	andi	a4,a5,31
20060740:	9736                	c.add	a4,a3
20060742:	0ff0000f          	fence	iorw,iorw
20060746:	973e                	c.add	a4,a5
20060748:	00f7f00b          	cache	dinv,(a5)
2006074c:	02078793          	addi	a5,a5,32
20060750:	40f706b3          	sub	a3,a4,a5
20060754:	fed04ae3          	blt	zero,a3,20060748 <inic_api_host_task_h+0x3d0>
20060758:	b76d                	c.j	20060702 <inic_api_host_task_h+0x38a>
2006075a:	73ca0513          	addi	a0,s4,1852 # 2006473c <register_string+0x98>
2006075e:	bc3fe0ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20060762:	b32d                	c.j	2006048c <inic_api_host_task_h+0x114>

20060764 <inic_api_host_int_hdl>:
20060764:	fdc1a503          	lw	a0,-36(gp) # 20066880 <g_host_inic_api_task_wake_sema>
20060768:	9b6fb06f          	jal	zero,2005b91e <rtos_sema_give>

2006076c <inic_api_host_message_send>:
2006076c:	7179                	c.addi16sp	sp,-48
2006076e:	cc52                	c.swsp	s4,24(sp)
20060770:	c85a                	c.swsp	s6,16(sp)
20060772:	8b2a                	c.mv	s6,a0
20060774:	fd81a503          	lw	a0,-40(gp) # 2006687c <g_host_inic_api_message_send_sema>
20060778:	c65e                	c.swsp	s7,12(sp)
2006077a:	20068bb7          	lui	s7,0x20068
2006077e:	d226                	c.swsp	s1,36(sp)
20060780:	ce4e                	c.swsp	s3,28(sp)
20060782:	c462                	c.swsp	s8,8(sp)
20060784:	89ae                	c.mv	s3,a1
20060786:	600b8493          	addi	s1,s7,1536 # 20068600 <g_host_ipc_api_request_info>
2006078a:	55fd                	c.li	a1,-1
2006078c:	6c0d                	c.lui	s8,0x3
2006078e:	ca56                	c.swsp	s5,20(sp)
20060790:	c266                	c.swsp	s9,4(sp)
20060792:	c06a                	c.swsp	s10,0(sp)
20060794:	d606                	c.swsp	ra,44(sp)
20060796:	d422                	c.swsp	s0,40(sp)
20060798:	d04a                	c.swsp	s2,32(sp)
2006079a:	8ab2                	c.mv	s5,a2
2006079c:	fd818a13          	addi	s4,gp,-40 # 2006687c <g_host_inic_api_message_send_sema>
200607a0:	932fb0ef          	jal	ra,2005b8d2 <rtos_sema_take>
200607a4:	02048d13          	addi	s10,s1,32
200607a8:	ee0c0c13          	addi	s8,s8,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
200607ac:	20064cb7          	lui	s9,0x20064
200607b0:	4401                	c.li	s0,0
200607b2:	7c1027f3          	csrrs	a5,0x7c1,zero
200607b6:	1007f793          	andi	a5,a5,256
200607ba:	600b8913          	addi	s2,s7,1536
200607be:	c785                	c.beqz	a5,200607e6 <inic_api_host_message_send+0x7a>
200607c0:	0ff0000f          	fence	iorw,iorw
200607c4:	0094f00b          	cache	dinv,(s1)
200607c8:	01ad700b          	cache	dinv,(s10)
200607cc:	0ff0000f          	fence	iorw,iorw
200607d0:	0001                	c.addi	zero,0
200607d2:	0001                	c.addi	zero,0
200607d4:	0001                	c.addi	zero,0
200607d6:	0001                	c.addi	zero,0
200607d8:	0001                	c.addi	zero,0
200607da:	0001                	c.addi	zero,0
200607dc:	0001                	c.addi	zero,0
200607de:	0001                	c.addi	zero,0
200607e0:	0001                	c.addi	zero,0
200607e2:	0001                	c.addi	zero,0
200607e4:	0001                	c.addi	zero,0
200607e6:	00092783          	lw	a5,0(s2)
200607ea:	cf89                	c.beqz	a5,20060804 <inic_api_host_message_send+0x98>
200607ec:	4505                	c.li	a0,1
200607ee:	0405                	c.addi	s0,1
200607f0:	bb8fb0ef          	jal	ra,2005bba8 <rtos_time_delay_ms>
200607f4:	03846433          	rem	s0,s0,s8
200607f8:	fc4d                	c.bnez	s0,200607b2 <inic_api_host_message_send+0x46>
200607fa:	77cc8513          	addi	a0,s9,1916 # 2006477c <register_string+0xd8>
200607fe:	b23fe0ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20060802:	b77d                	c.j	200607b0 <inic_api_host_message_send+0x44>
20060804:	04000613          	addi	a2,zero,64
20060808:	4581                	c.li	a1,0
2006080a:	600b8513          	addi	a0,s7,1536
2006080e:	dffa9097          	auipc	ra,0xdffa9
20060812:	2c0080e7          	jalr	ra,704(ra) # 9ace <__wrap_memset>
20060816:	01692023          	sw	s6,0(s2)
2006081a:	00098b63          	beq	s3,zero,20060830 <inic_api_host_message_send+0xc4>
2006081e:	002a9613          	slli	a2,s5,0x2
20060822:	85ce                	c.mv	a1,s3
20060824:	00490513          	addi	a0,s2,4
20060828:	dffa9097          	auipc	ra,0xdffa9
2006082c:	2a8080e7          	jalr	ra,680(ra) # 9ad0 <__wrap_memcpy>
20060830:	7c1027f3          	csrrs	a5,0x7c1,zero
20060834:	1007f793          	andi	a5,a5,256
20060838:	c795                	c.beqz	a5,20060864 <inic_api_host_message_send+0xf8>
2006083a:	0ff0000f          	fence	iorw,iorw
2006083e:	0494f00b          	cache	dwb,(s1)
20060842:	02048793          	addi	a5,s1,32
20060846:	04f7f00b          	cache	dwb,(a5)
2006084a:	0ff0000f          	fence	iorw,iorw
2006084e:	0001                	c.addi	zero,0
20060850:	0001                	c.addi	zero,0
20060852:	0001                	c.addi	zero,0
20060854:	0001                	c.addi	zero,0
20060856:	0001                	c.addi	zero,0
20060858:	0001                	c.addi	zero,0
2006085a:	0001                	c.addi	zero,0
2006085c:	0001                	c.addi	zero,0
2006085e:	0001                	c.addi	zero,0
20060860:	0001                	c.addi	zero,0
20060862:	0001                	c.addi	zero,0
20060864:	200689b7          	lui	s3,0x20068
20060868:	4641                	c.li	a2,16
2006086a:	4581                	c.li	a1,0
2006086c:	5c098513          	addi	a0,s3,1472 # 200685c0 <g_host_ipc_api_msg>
20060870:	dffa9097          	auipc	ra,0xdffa9
20060874:	25e080e7          	jalr	ra,606(ra) # 9ace <__wrap_memset>
20060878:	5c098a93          	addi	s5,s3,1472
2006087c:	04000793          	addi	a5,zero,64
20060880:	009aa223          	sw	s1,4(s5)
20060884:	000aa023          	sw	zero,0(s5)
20060888:	00faa423          	sw	a5,8(s5)
2006088c:	7c1027f3          	csrrs	a5,0x7c1,zero
20060890:	1007f793          	andi	a5,a5,256
20060894:	c395                	c.beqz	a5,200608b8 <inic_api_host_message_send+0x14c>
20060896:	0ff0000f          	fence	iorw,iorw
2006089a:	055af00b          	cache	dwb,(s5)
2006089e:	0ff0000f          	fence	iorw,iorw
200608a2:	0001                	c.addi	zero,0
200608a4:	0001                	c.addi	zero,0
200608a6:	0001                	c.addi	zero,0
200608a8:	0001                	c.addi	zero,0
200608aa:	0001                	c.addi	zero,0
200608ac:	0001                	c.addi	zero,0
200608ae:	0001                	c.addi	zero,0
200608b0:	0001                	c.addi	zero,0
200608b2:	0001                	c.addi	zero,0
200608b4:	0001                	c.addi	zero,0
200608b6:	0001                	c.addi	zero,0
200608b8:	5c098613          	addi	a2,s3,1472
200608bc:	459d                	c.li	a1,7
200608be:	4501                	c.li	a0,0
200608c0:	698d                	c.lui	s3,0x3
200608c2:	ee6f80ef          	jal	ra,20058fa8 <ipc_send_message>
200608c6:	02048b93          	addi	s7,s1,32
200608ca:	ee098993          	addi	s3,s3,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
200608ce:	20065b37          	lui	s6,0x20065
200608d2:	20064ab7          	lui	s5,0x20064
200608d6:	00092783          	lw	a5,0(s2)
200608da:	c7b9                	c.beqz	a5,20060928 <inic_api_host_message_send+0x1bc>
200608dc:	4505                	c.li	a0,1
200608de:	acafb0ef          	jal	ra,2005bba8 <rtos_time_delay_ms>
200608e2:	7c1027f3          	csrrs	a5,0x7c1,zero
200608e6:	1007f793          	andi	a5,a5,256
200608ea:	c785                	c.beqz	a5,20060912 <inic_api_host_message_send+0x1a6>
200608ec:	0ff0000f          	fence	iorw,iorw
200608f0:	0094f00b          	cache	dinv,(s1)
200608f4:	017bf00b          	cache	dinv,(s7)
200608f8:	0ff0000f          	fence	iorw,iorw
200608fc:	0001                	c.addi	zero,0
200608fe:	0001                	c.addi	zero,0
20060900:	0001                	c.addi	zero,0
20060902:	0001                	c.addi	zero,0
20060904:	0001                	c.addi	zero,0
20060906:	0001                	c.addi	zero,0
20060908:	0001                	c.addi	zero,0
2006090a:	0001                	c.addi	zero,0
2006090c:	0001                	c.addi	zero,0
2006090e:	0001                	c.addi	zero,0
20060910:	0001                	c.addi	zero,0
20060912:	0405                	c.addi	s0,1
20060914:	03346433          	rem	s0,s0,s3
20060918:	fc5d                	c.bnez	s0,200608d6 <inic_api_host_message_send+0x16a>
2006091a:	81cb0593          	addi	a1,s6,-2020 # 2006481c <__func__.0>
2006091e:	794a8513          	addi	a0,s5,1940 # 20064794 <register_string+0xf0>
20060922:	9fffe0ef          	jal	ra,2005f320 <DiagPrintf_minimal>
20060926:	bf45                	c.j	200608d6 <inic_api_host_message_send+0x16a>
20060928:	000a2503          	lw	a0,0(s4)
2006092c:	02892403          	lw	s0,40(s2)
20060930:	feffa0ef          	jal	ra,2005b91e <rtos_sema_give>
20060934:	50b2                	c.lwsp	ra,44(sp)
20060936:	8522                	c.mv	a0,s0
20060938:	5422                	c.lwsp	s0,40(sp)
2006093a:	5492                	c.lwsp	s1,36(sp)
2006093c:	5902                	c.lwsp	s2,32(sp)
2006093e:	49f2                	c.lwsp	s3,28(sp)
20060940:	4a62                	c.lwsp	s4,24(sp)
20060942:	4ad2                	c.lwsp	s5,20(sp)
20060944:	4b42                	c.lwsp	s6,16(sp)
20060946:	4bb2                	c.lwsp	s7,12(sp)
20060948:	4c22                	c.lwsp	s8,8(sp)
2006094a:	4c92                	c.lwsp	s9,4(sp)
2006094c:	4d02                	c.lwsp	s10,0(sp)
2006094e:	6145                	c.addi16sp	sp,48
20060950:	8082                	c.jr	ra

20060952 <inic_api_init_host>:
20060952:	1141                	c.addi	sp,-16
20060954:	567d                	c.li	a2,-1
20060956:	4581                	c.li	a1,0
20060958:	fdc18513          	addi	a0,gp,-36 # 20066880 <g_host_inic_api_task_wake_sema>
2006095c:	c606                	c.swsp	ra,12(sp)
2006095e:	c422                	c.swsp	s0,8(sp)
20060960:	ef5fa0ef          	jal	ra,2005b854 <rtos_sema_create_static>
20060964:	567d                	c.li	a2,-1
20060966:	4581                	c.li	a1,0
20060968:	fd818413          	addi	s0,gp,-40 # 2006687c <g_host_inic_api_message_send_sema>
2006096c:	fd818513          	addi	a0,gp,-40 # 2006687c <g_host_inic_api_message_send_sema>
20060970:	ee5fa0ef          	jal	ra,2005b854 <rtos_sema_create_static>
20060974:	4008                	c.lw	a0,0(s0)
20060976:	fa9fa0ef          	jal	ra,2005b91e <rtos_sema_give>
2006097a:	200605b7          	lui	a1,0x20060
2006097e:	4701                	c.li	a4,0
20060980:	4681                	c.li	a3,0
20060982:	4601                	c.li	a2,0
20060984:	37458593          	addi	a1,a1,884 # 20060374 <_inic_ipc_ip_addr_update_in_wowlan>
20060988:	4505                	c.li	a0,1
2006098a:	dfdf80ef          	jal	ra,20059786 <pmu_register_sleep_callback>
2006098e:	6705                	c.lui	a4,0x1
20060990:	20060637          	lui	a2,0x20060
20060994:	200645b7          	lui	a1,0x20064
20060998:	478d                	c.li	a5,3
2006099a:	ce070713          	addi	a4,a4,-800 # ce0 <CPU_ClkGet+0x1ca>
2006099e:	4681                	c.li	a3,0
200609a0:	37860613          	addi	a2,a2,888 # 20060378 <inic_api_host_task_h>
200609a4:	7b458593          	addi	a1,a1,1972 # 200647b4 <register_string+0x110>
200609a8:	4501                	c.li	a0,0
200609aa:	9c8fb0ef          	jal	ra,2005bb72 <rtos_task_create>
200609ae:	c10d                	c.beqz	a0,200609d0 <inic_api_init_host+0x7e>
200609b0:	4422                	c.lwsp	s0,8(sp)
200609b2:	40b2                	c.lwsp	ra,12(sp)
200609b4:	200646b7          	lui	a3,0x20064
200609b8:	200645b7          	lui	a1,0x20064
200609bc:	7c868693          	addi	a3,a3,1992 # 200647c8 <register_string+0x124>
200609c0:	04500613          	addi	a2,zero,69
200609c4:	7e458593          	addi	a1,a1,2020 # 200647e4 <register_string+0x140>
200609c8:	4509                	c.li	a0,2
200609ca:	0141                	c.addi	sp,16
200609cc:	d20fa06f          	jal	zero,2005aeec <rtk_log_write>
200609d0:	40b2                	c.lwsp	ra,12(sp)
200609d2:	4422                	c.lwsp	s0,8(sp)
200609d4:	0141                	c.addi	sp,16
200609d6:	8082                	c.jr	ra

200609d8 <inic_mp_command>:
200609d8:	7179                	c.addi16sp	sp,-48
200609da:	ce4e                	c.swsp	s3,28(sp)
200609dc:	89aa                	c.mv	s3,a0
200609de:	6505                	c.lui	a0,0x1
200609e0:	d04a                	c.swsp	s2,32(sp)
200609e2:	cc52                	c.swsp	s4,24(sp)
200609e4:	d606                	c.swsp	ra,44(sp)
200609e6:	d422                	c.swsp	s0,40(sp)
200609e8:	d226                	c.swsp	s1,36(sp)
200609ea:	892e                	c.mv	s2,a1
200609ec:	8a32                	c.mv	s4,a2
200609ee:	cbbfa0ef          	jal	ra,2005b6a8 <rtos_mem_zmalloc>
200609f2:	e50d                	c.bnez	a0,20060a1c <inic_mp_command+0x44>
200609f4:	5422                	c.lwsp	s0,40(sp)
200609f6:	50b2                	c.lwsp	ra,44(sp)
200609f8:	5492                	c.lwsp	s1,36(sp)
200609fa:	5902                	c.lwsp	s2,32(sp)
200609fc:	49f2                	c.lwsp	s3,28(sp)
200609fe:	4a62                	c.lwsp	s4,24(sp)
20060a00:	200646b7          	lui	a3,0x20064
20060a04:	200645b7          	lui	a1,0x20064
20060a08:	7ec68693          	addi	a3,a3,2028 # 200647ec <register_string+0x148>
20060a0c:	04500613          	addi	a2,zero,69
20060a10:	7e458593          	addi	a1,a1,2020 # 200647e4 <register_string+0x140>
20060a14:	4509                	c.li	a0,2
20060a16:	6145                	c.addi16sp	sp,48
20060a18:	cd4fa06f          	jal	zero,2005aeec <rtk_log_write>
20060a1c:	842a                	c.mv	s0,a0
20060a1e:	87ce                	c.mv	a5,s3
20060a20:	c04e                	c.swsp	s3,0(sp)
20060a22:	c24a                	c.swsp	s2,4(sp)
20060a24:	c452                	c.swsp	s4,8(sp)
20060a26:	84aa                	c.mv	s1,a0
20060a28:	c62a                	c.swsp	a0,12(sp)
20060a2a:	7c102773          	csrrs	a4,0x7c1,zero
20060a2e:	10077713          	andi	a4,a4,256
20060a32:	c721                	c.beqz	a4,20060a7a <inic_mp_command+0xa2>
20060a34:	01f57693          	andi	a3,a0,31
20060a38:	10068363          	beq	a3,zero,20060b3e <inic_mp_command+0x166>
20060a3c:	6605                	c.lui	a2,0x1
20060a3e:	9636                	c.add	a2,a3
20060a40:	fe057713          	andi	a4,a0,-32
20060a44:	01f77693          	andi	a3,a4,31
20060a48:	96b2                	c.add	a3,a2
20060a4a:	0ff0000f          	fence	iorw,iorw
20060a4e:	96ba                	c.add	a3,a4
20060a50:	04e7700b          	cache	dwb,(a4)
20060a54:	02070713          	addi	a4,a4,32
20060a58:	40e68633          	sub	a2,a3,a4
20060a5c:	fec04ae3          	blt	zero,a2,20060a50 <inic_mp_command+0x78>
20060a60:	0ff0000f          	fence	iorw,iorw
20060a64:	0001                	c.addi	zero,0
20060a66:	0001                	c.addi	zero,0
20060a68:	0001                	c.addi	zero,0
20060a6a:	0001                	c.addi	zero,0
20060a6c:	0001                	c.addi	zero,0
20060a6e:	0001                	c.addi	zero,0
20060a70:	0001                	c.addi	zero,0
20060a72:	0001                	c.addi	zero,0
20060a74:	0001                	c.addi	zero,0
20060a76:	0001                	c.addi	zero,0
20060a78:	0001                	c.addi	zero,0
20060a7a:	7c102773          	csrrs	a4,0x7c1,zero
20060a7e:	10077713          	andi	a4,a4,256
20060a82:	c70d                	c.beqz	a4,20060aac <inic_mp_command+0xd4>
20060a84:	577d                	c.li	a4,-1
20060a86:	0ae99f63          	bne	s3,a4,20060b44 <inic_mp_command+0x16c>
20060a8a:	0b391d63          	bne	s2,s3,20060b44 <inic_mp_command+0x16c>
20060a8e:	7c0267f3          	csrrsi	a5,0x7c0,4
20060a92:	0ff0000f          	fence	iorw,iorw
20060a96:	0001                	c.addi	zero,0
20060a98:	0001                	c.addi	zero,0
20060a9a:	0001                	c.addi	zero,0
20060a9c:	0001                	c.addi	zero,0
20060a9e:	0001                	c.addi	zero,0
20060aa0:	0001                	c.addi	zero,0
20060aa2:	0001                	c.addi	zero,0
20060aa4:	0001                	c.addi	zero,0
20060aa6:	0001                	c.addi	zero,0
20060aa8:	0001                	c.addi	zero,0
20060aaa:	0001                	c.addi	zero,0
20060aac:	650d                	c.lui	a0,0x3
20060aae:	4611                	c.li	a2,4
20060ab0:	858a                	c.mv	a1,sp
20060ab2:	050d                	c.addi	a0,3
20060ab4:	cb9ff0ef          	jal	ra,2006076c <inic_api_host_message_send>
20060ab8:	060a0863          	beq	s4,zero,20060b28 <inic_mp_command+0x150>
20060abc:	7c1027f3          	csrrs	a5,0x7c1,zero
20060ac0:	1007f793          	andi	a5,a5,256
20060ac4:	c7a1                	c.beqz	a5,20060b0c <inic_mp_command+0x134>
20060ac6:	01f47713          	andi	a4,s0,31
20060aca:	6785                	c.lui	a5,0x1
20060acc:	c701                	c.beqz	a4,20060ad4 <inic_mp_command+0xfc>
20060ace:	97ba                	c.add	a5,a4
20060ad0:	fe047493          	andi	s1,s0,-32
20060ad4:	01f4f713          	andi	a4,s1,31
20060ad8:	973e                	c.add	a4,a5
20060ada:	0ff0000f          	fence	iorw,iorw
20060ade:	87a6                	c.mv	a5,s1
20060ae0:	9726                	c.add	a4,s1
20060ae2:	00f7f00b          	cache	dinv,(a5)
20060ae6:	02078793          	addi	a5,a5,32 # 1020 <CPU_ClkGet+0x50a>
20060aea:	40f706b3          	sub	a3,a4,a5
20060aee:	fed04ae3          	blt	zero,a3,20060ae2 <inic_mp_command+0x10a>
20060af2:	0ff0000f          	fence	iorw,iorw
20060af6:	0001                	c.addi	zero,0
20060af8:	0001                	c.addi	zero,0
20060afa:	0001                	c.addi	zero,0
20060afc:	0001                	c.addi	zero,0
20060afe:	0001                	c.addi	zero,0
20060b00:	0001                	c.addi	zero,0
20060b02:	0001                	c.addi	zero,0
20060b04:	0001                	c.addi	zero,0
20060b06:	0001                	c.addi	zero,0
20060b08:	0001                	c.addi	zero,0
20060b0a:	0001                	c.addi	zero,0
20060b0c:	200656b7          	lui	a3,0x20065
20060b10:	200645b7          	lui	a1,0x20064
20060b14:	8722                	c.mv	a4,s0
20060b16:	80468693          	addi	a3,a3,-2044 # 20064804 <register_string+0x160>
20060b1a:	04100613          	addi	a2,zero,65
20060b1e:	7e458593          	addi	a1,a1,2020 # 200647e4 <register_string+0x140>
20060b22:	4505                	c.li	a0,1
20060b24:	bc8fa0ef          	jal	ra,2005aeec <rtk_log_write>
20060b28:	8522                	c.mv	a0,s0
20060b2a:	ba5fa0ef          	jal	ra,2005b6ce <rtos_mem_free>
20060b2e:	50b2                	c.lwsp	ra,44(sp)
20060b30:	5422                	c.lwsp	s0,40(sp)
20060b32:	5492                	c.lwsp	s1,36(sp)
20060b34:	5902                	c.lwsp	s2,32(sp)
20060b36:	49f2                	c.lwsp	s3,28(sp)
20060b38:	4a62                	c.lwsp	s4,24(sp)
20060b3a:	6145                	c.addi16sp	sp,48
20060b3c:	8082                	c.jr	ra
20060b3e:	872a                	c.mv	a4,a0
20060b40:	6605                	c.lui	a2,0x1
20060b42:	b709                	c.j	20060a44 <inic_mp_command+0x6c>
20060b44:	01f9f713          	andi	a4,s3,31
20060b48:	c701                	c.beqz	a4,20060b50 <inic_mp_command+0x178>
20060b4a:	fe09f793          	andi	a5,s3,-32
20060b4e:	993a                	c.add	s2,a4
20060b50:	f5205ee3          	bge	zero,s2,20060aac <inic_mp_command+0xd4>
20060b54:	01f7f593          	andi	a1,a5,31
20060b58:	992e                	c.add	s2,a1
20060b5a:	0ff0000f          	fence	iorw,iorw
20060b5e:	993e                	c.add	s2,a5
20060b60:	04f7f00b          	cache	dwb,(a5)
20060b64:	02078793          	addi	a5,a5,32
20060b68:	40f90733          	sub	a4,s2,a5
20060b6c:	fee04ae3          	blt	zero,a4,20060b60 <inic_mp_command+0x188>
20060b70:	b70d                	c.j	20060a92 <inic_mp_command+0xba>

20060b72 <inic_host_init>:
20060b72:	1141                	c.addi	sp,-16
20060b74:	c606                	c.swsp	ra,12(sp)
20060b76:	2239                	c.jal	20060c84 <inic_host_init_priv>
20060b78:	40b2                	c.lwsp	ra,12(sp)
20060b7a:	0141                	c.addi	sp,16
20060b7c:	dd7ff06f          	jal	zero,20060952 <inic_api_init_host>

20060b80 <inic_host_init_skb>:
20060b80:	1141                	c.addi	sp,-16
20060b82:	c422                	c.swsp	s0,8(sp)
20060b84:	fe01a783          	lw	a5,-32(gp) # 20066884 <host_skb_buff>
20060b88:	c606                	c.swsp	ra,12(sp)
20060b8a:	c226                	c.swsp	s1,4(sp)
20060b8c:	c395                	c.beqz	a5,20060bb0 <inic_host_init_skb+0x30>
20060b8e:	4422                	c.lwsp	s0,8(sp)
20060b90:	40b2                	c.lwsp	ra,12(sp)
20060b92:	4492                	c.lwsp	s1,4(sp)
20060b94:	200656b7          	lui	a3,0x20065
20060b98:	200645b7          	lui	a1,0x20064
20060b9c:	83868693          	addi	a3,a3,-1992 # 20064838 <__func__.0+0x1c>
20060ba0:	04500613          	addi	a2,zero,69
20060ba4:	7e458593          	addi	a1,a1,2020 # 200647e4 <register_string+0x140>
20060ba8:	4509                	c.li	a0,2
20060baa:	0141                	c.addi	sp,16
20060bac:	b40fa06f          	jal	zero,2005aeec <rtk_log_write>
20060bb0:	66000793          	addi	a5,zero,1632
20060bb4:	fe418713          	addi	a4,gp,-28 # 20066888 <skb_buf_max_size>
20060bb8:	c31c                	c.sw	a5,0(a4)
20060bba:	41c18793          	addi	a5,gp,1052 # 20066cc0 <wifi_user_config>
20060bbe:	579c                	c.lw	a5,40(a5)
20060bc0:	fe018413          	addi	s0,gp,-32 # 20066884 <host_skb_buff>
20060bc4:	41c18493          	addi	s1,gp,1052 # 20066cc0 <wifi_user_config>
20060bc8:	c781                	c.beqz	a5,20060bd0 <inic_host_init_skb+0x50>
20060bca:	07fd                	c.addi	a5,31
20060bcc:	9b81                	c.andi	a5,-32
20060bce:	c31c                	c.sw	a5,0(a4)
20060bd0:	50dc                	c.lw	a5,36(s1)
20060bd2:	6c000513          	addi	a0,zero,1728
20060bd6:	02f50533          	mul	a0,a0,a5
20060bda:	acffa0ef          	jal	ra,2005b6a8 <rtos_mem_zmalloc>
20060bde:	c008                	c.sw	a0,0(s0)
20060be0:	e115                	c.bnez	a0,20060c04 <inic_host_init_skb+0x84>
20060be2:	20065737          	lui	a4,0x20065
20060be6:	200656b7          	lui	a3,0x20065
20060bea:	200645b7          	lui	a1,0x20064
20060bee:	88470713          	addi	a4,a4,-1916 # 20064884 <__func__.1>
20060bf2:	85068693          	addi	a3,a3,-1968 # 20064850 <__func__.0+0x34>
20060bf6:	04500613          	addi	a2,zero,69
20060bfa:	7e458593          	addi	a1,a1,2020 # 200647e4 <register_string+0x140>
20060bfe:	4509                	c.li	a0,2
20060c00:	aecfa0ef          	jal	ra,2005aeec <rtk_log_write>
20060c04:	4681                	c.li	a3,0
20060c06:	6c000613          	addi	a2,zero,1728
20060c0a:	50d8                	c.lw	a4,36(s1)
20060c0c:	401c                	c.lw	a5,0(s0)
20060c0e:	06e6c263          	blt	a3,a4,20060c72 <inic_host_init_skb+0xf2>
20060c12:	7c1026f3          	csrrs	a3,0x7c1,zero
20060c16:	1006f693          	andi	a3,a3,256
20060c1a:	c6b9                	c.beqz	a3,20060c68 <inic_host_init_skb+0xe8>
20060c1c:	6c000693          	addi	a3,zero,1728
20060c20:	02d70733          	mul	a4,a4,a3
20060c24:	01f7f693          	andi	a3,a5,31
20060c28:	c299                	c.beqz	a3,20060c2e <inic_host_init_skb+0xae>
20060c2a:	9b81                	c.andi	a5,-32
20060c2c:	9736                	c.add	a4,a3
20060c2e:	02e05d63          	bge	zero,a4,20060c68 <inic_host_init_skb+0xe8>
20060c32:	01f7f693          	andi	a3,a5,31
20060c36:	9736                	c.add	a4,a3
20060c38:	0ff0000f          	fence	iorw,iorw
20060c3c:	973e                	c.add	a4,a5
20060c3e:	04f7f00b          	cache	dwb,(a5)
20060c42:	02078793          	addi	a5,a5,32
20060c46:	40f706b3          	sub	a3,a4,a5
20060c4a:	fed04ae3          	blt	zero,a3,20060c3e <inic_host_init_skb+0xbe>
20060c4e:	0ff0000f          	fence	iorw,iorw
20060c52:	0001                	c.addi	zero,0
20060c54:	0001                	c.addi	zero,0
20060c56:	0001                	c.addi	zero,0
20060c58:	0001                	c.addi	zero,0
20060c5a:	0001                	c.addi	zero,0
20060c5c:	0001                	c.addi	zero,0
20060c5e:	0001                	c.addi	zero,0
20060c60:	0001                	c.addi	zero,0
20060c62:	0001                	c.addi	zero,0
20060c64:	0001                	c.addi	zero,0
20060c66:	0001                	c.addi	zero,0
20060c68:	40b2                	c.lwsp	ra,12(sp)
20060c6a:	4422                	c.lwsp	s0,8(sp)
20060c6c:	4492                	c.lwsp	s1,4(sp)
20060c6e:	0141                	c.addi	sp,16
20060c70:	8082                	c.jr	ra
20060c72:	02c68733          	mul	a4,a3,a2
20060c76:	0685                	c.addi	a3,1
20060c78:	97ba                	c.add	a5,a4
20060c7a:	c39c                	c.sw	a5,0(a5)
20060c7c:	401c                	c.lw	a5,0(s0)
20060c7e:	97ba                	c.add	a5,a4
20060c80:	c3dc                	c.sw	a5,4(a5)
20060c82:	b761                	c.j	20060c0a <inic_host_init_skb+0x8a>

20060c84 <inic_host_init_priv>:
20060c84:	1141                	c.addi	sp,-16
20060c86:	c226                	c.swsp	s1,4(sp)
20060c88:	200684b7          	lui	s1,0x20068
20060c8c:	02800613          	addi	a2,zero,40
20060c90:	4581                	c.li	a1,0
20060c92:	64048513          	addi	a0,s1,1600 # 20068640 <g_inic_host_priv>
20060c96:	c606                	c.swsp	ra,12(sp)
20060c98:	c422                	c.swsp	s0,8(sp)
20060c9a:	dffa9097          	auipc	ra,0xdffa9
20060c9e:	e34080e7          	jalr	ra,-460(ra) # 9ace <__wrap_memset>
20060ca2:	567d                	c.li	a2,-1
20060ca4:	4581                	c.li	a1,0
20060ca6:	64048513          	addi	a0,s1,1600
20060caa:	64048413          	addi	s0,s1,1600
20060cae:	ba7fa0ef          	jal	ra,2005b854 <rtos_sema_create_static>
20060cb2:	567d                	c.li	a2,-1
20060cb4:	4581                	c.li	a1,0
20060cb6:	00440513          	addi	a0,s0,4
20060cba:	b9bfa0ef          	jal	ra,2005b854 <rtos_sema_create_static>
20060cbe:	567d                	c.li	a2,-1
20060cc0:	4581                	c.li	a1,0
20060cc2:	00840513          	addi	a0,s0,8
20060cc6:	b8ffa0ef          	jal	ra,2005b854 <rtos_sema_create_static>
20060cca:	4408                	c.lw	a0,8(s0)
20060ccc:	c53fa0ef          	jal	ra,2005b91e <rtos_sema_give>
20060cd0:	00c40513          	addi	a0,s0,12
20060cd4:	2ea5                	c.jal	2006104c <rtw_init_queue>
20060cd6:	40b2                	c.lwsp	ra,12(sp)
20060cd8:	00042c23          	sw	zero,24(s0)
20060cdc:	00042e23          	sw	zero,28(s0)
20060ce0:	02042023          	sw	zero,32(s0)
20060ce4:	02042223          	sw	zero,36(s0)
20060ce8:	4422                	c.lwsp	s0,8(sp)
20060cea:	4492                	c.lwsp	s1,4(sp)
20060cec:	0141                	c.addi	sp,16
20060cee:	8082                	c.jr	ra

20060cf0 <inic_host_trx_event_hdl>:
20060cf0:	4791                	c.li	a5,4
20060cf2:	872a                	c.mv	a4,a0
20060cf4:	00f50f63          	beq	a0,a5,20060d12 <inic_host_trx_event_hdl+0x22>
20060cf8:	200656b7          	lui	a3,0x20065
20060cfc:	200645b7          	lui	a1,0x20064
20060d00:	86868693          	addi	a3,a3,-1944 # 20064868 <__func__.0+0x4c>
20060d04:	04500613          	addi	a2,zero,69
20060d08:	7e458593          	addi	a1,a1,2020 # 200647e4 <register_string+0x140>
20060d0c:	4509                	c.li	a0,2
20060d0e:	9defa06f          	jal	zero,2005aeec <rtk_log_write>
20060d12:	8082                	c.jr	ra

20060d14 <inic_host_trx_int_hdl>:
20060d14:	1141                	c.addi	sp,-16
20060d16:	4599                	c.li	a1,6
20060d18:	4541                	c.li	a0,16
20060d1a:	c422                	c.swsp	s0,8(sp)
20060d1c:	c606                	c.swsp	ra,12(sp)
20060d1e:	bd2f80ef          	jal	ra,200590f0 <ipc_get_message>
20060d22:	4140                	c.lw	s0,4(a0)
20060d24:	7c1027f3          	csrrs	a5,0x7c1,zero
20060d28:	1007f793          	andi	a5,a5,256
20060d2c:	c3b9                	c.beqz	a5,20060d72 <inic_host_trx_int_hdl+0x5e>
20060d2e:	01f47713          	andi	a4,s0,31
20060d32:	c755                	c.beqz	a4,20060dde <inic_host_trx_int_hdl+0xca>
20060d34:	04070693          	addi	a3,a4,64
20060d38:	fe047793          	andi	a5,s0,-32
20060d3c:	01f7f713          	andi	a4,a5,31
20060d40:	9736                	c.add	a4,a3
20060d42:	0ff0000f          	fence	iorw,iorw
20060d46:	973e                	c.add	a4,a5
20060d48:	00f7f00b          	cache	dinv,(a5)
20060d4c:	02078793          	addi	a5,a5,32
20060d50:	40f706b3          	sub	a3,a4,a5
20060d54:	fed04ae3          	blt	zero,a3,20060d48 <inic_host_trx_int_hdl+0x34>
20060d58:	0ff0000f          	fence	iorw,iorw
20060d5c:	0001                	c.addi	zero,0
20060d5e:	0001                	c.addi	zero,0
20060d60:	0001                	c.addi	zero,0
20060d62:	0001                	c.addi	zero,0
20060d64:	0001                	c.addi	zero,0
20060d66:	0001                	c.addi	zero,0
20060d68:	0001                	c.addi	zero,0
20060d6a:	0001                	c.addi	zero,0
20060d6c:	0001                	c.addi	zero,0
20060d6e:	0001                	c.addi	zero,0
20060d70:	0001                	c.addi	zero,0
20060d72:	2455                	c.jal	20061016 <inic_msg_get_queue_status>
20060d74:	c92d                	c.beqz	a0,20060de6 <inic_host_trx_int_hdl+0xd2>
20060d76:	8522                	c.mv	a0,s0
20060d78:	2af1                	c.jal	20060f54 <inic_msg_enqueue>
20060d7a:	157d                	c.addi	a0,-1
20060d7c:	00a03533          	sltu	a0,zero,a0
20060d80:	c408                	c.sw	a0,8(s0)
20060d82:	00042023          	sw	zero,0(s0)
20060d86:	7c1027f3          	csrrs	a5,0x7c1,zero
20060d8a:	1007f793          	andi	a5,a5,256
20060d8e:	c7a1                	c.beqz	a5,20060dd6 <inic_host_trx_int_hdl+0xc2>
20060d90:	01f47793          	andi	a5,s0,31
20060d94:	04000713          	addi	a4,zero,64
20060d98:	c781                	c.beqz	a5,20060da0 <inic_host_trx_int_hdl+0x8c>
20060d9a:	04078713          	addi	a4,a5,64
20060d9e:	9801                	c.andi	s0,-32
20060da0:	01f47793          	andi	a5,s0,31
20060da4:	97ba                	c.add	a5,a4
20060da6:	0ff0000f          	fence	iorw,iorw
20060daa:	97a2                	c.add	a5,s0
20060dac:	0484700b          	cache	dwb,(s0)
20060db0:	02040413          	addi	s0,s0,32
20060db4:	40878733          	sub	a4,a5,s0
20060db8:	fee04ae3          	blt	zero,a4,20060dac <inic_host_trx_int_hdl+0x98>
20060dbc:	0ff0000f          	fence	iorw,iorw
20060dc0:	0001                	c.addi	zero,0
20060dc2:	0001                	c.addi	zero,0
20060dc4:	0001                	c.addi	zero,0
20060dc6:	0001                	c.addi	zero,0
20060dc8:	0001                	c.addi	zero,0
20060dca:	0001                	c.addi	zero,0
20060dcc:	0001                	c.addi	zero,0
20060dce:	0001                	c.addi	zero,0
20060dd0:	0001                	c.addi	zero,0
20060dd2:	0001                	c.addi	zero,0
20060dd4:	0001                	c.addi	zero,0
20060dd6:	40b2                	c.lwsp	ra,12(sp)
20060dd8:	4422                	c.lwsp	s0,8(sp)
20060dda:	0141                	c.addi	sp,16
20060ddc:	8082                	c.jr	ra
20060dde:	87a2                	c.mv	a5,s0
20060de0:	04000693          	addi	a3,zero,64
20060de4:	bfa1                	c.j	20060d3c <inic_host_trx_int_hdl+0x28>
20060de6:	00044503          	lbu	a0,0(s0)
20060dea:	00c44603          	lbu	a2,12(s0)
20060dee:	404c                	c.lw	a1,4(s0)
20060df0:	f01ff0ef          	jal	ra,20060cf0 <inic_host_trx_event_hdl>
20060df4:	4501                	c.li	a0,0
20060df6:	b769                	c.j	20060d80 <inic_host_trx_int_hdl+0x6c>

20060df8 <inic_msg_q_task>:
20060df8:	1101                	c.addi	sp,-32
20060dfa:	cc22                	c.swsp	s0,24(sp)
20060dfc:	20068437          	lui	s0,0x20068
20060e00:	c84a                	c.swsp	s2,16(sp)
20060e02:	c64e                	c.swsp	s3,12(sp)
20060e04:	6c040913          	addi	s2,s0,1728 # 200686c0 <g_ipc_msg_q_priv>
20060e08:	ce06                	c.swsp	ra,28(sp)
20060e0a:	ca26                	c.swsp	s1,20(sp)
20060e0c:	6c040413          	addi	s0,s0,1728
20060e10:	4985                	c.li	s3,1
20060e12:	00c92503          	lw	a0,12(s2)
20060e16:	55fd                	c.li	a1,-1
20060e18:	abbfa0ef          	jal	ra,2005b8d2 <rtos_sema_take>
20060e1c:	83dfa0ef          	jal	ra,2005b658 <rtos_critical_enter>
20060e20:	8522                	c.mv	a0,s0
20060e22:	2c25                	c.jal	2006105a <rtw_queue_empty>
20060e24:	03351063          	bne	a0,s3,20060e44 <inic_msg_q_task+0x4c>
20060e28:	855fa0ef          	jal	ra,2005b67c <rtos_critical_exit>
20060e2c:	01444783          	lbu	a5,20(s0)
20060e30:	f3ed                	c.bnez	a5,20060e12 <inic_msg_q_task+0x1a>
20060e32:	4462                	c.lwsp	s0,24(sp)
20060e34:	40f2                	c.lwsp	ra,28(sp)
20060e36:	44d2                	c.lwsp	s1,20(sp)
20060e38:	4942                	c.lwsp	s2,16(sp)
20060e3a:	49b2                	c.lwsp	s3,12(sp)
20060e3c:	4501                	c.li	a0,0
20060e3e:	6105                	c.addi16sp	sp,32
20060e40:	d57fa06f          	jal	zero,2005bb96 <rtos_task_delete>
20060e44:	4004                	c.lw	s1,0(s0)
20060e46:	8526                	c.mv	a0,s1
20060e48:	2add                	c.jal	2006103e <rtw_list_delete>
20060e4a:	833fa0ef          	jal	ra,2005b67c <rtos_critical_exit>
20060e4e:	dcf9                	c.beqz	s1,20060e2c <inic_msg_q_task+0x34>
20060e50:	44c8                	c.lw	a0,12(s1)
20060e52:	448c                	c.lw	a1,8(s1)
20060e54:	00155613          	srli	a2,a0,0x1
20060e58:	810d                	c.srli	a0,0x3
20060e5a:	8a0d                	c.andi	a2,3
20060e5c:	897d                	c.andi	a0,31
20060e5e:	e93ff0ef          	jal	ra,20060cf0 <inic_host_trx_event_hdl>
20060e62:	ff6fa0ef          	jal	ra,2005b658 <rtos_critical_enter>
20060e66:	00c4c783          	lbu	a5,12(s1)
20060e6a:	9bf9                	c.andi	a5,-2
20060e6c:	00f48623          	sb	a5,12(s1)
20060e70:	80dfa0ef          	jal	ra,2005b67c <rtos_critical_exit>
20060e74:	b765                	c.j	20060e1c <inic_msg_q_task+0x24>

20060e76 <inic_msg_q_init>:
20060e76:	1141                	c.addi	sp,-16
20060e78:	c422                	c.swsp	s0,8(sp)
20060e7a:	20068437          	lui	s0,0x20068
20060e7e:	02000613          	addi	a2,zero,32
20060e82:	4581                	c.li	a1,0
20060e84:	6c040513          	addi	a0,s0,1728 # 200686c0 <g_ipc_msg_q_priv>
20060e88:	c606                	c.swsp	ra,12(sp)
20060e8a:	c226                	c.swsp	s1,4(sp)
20060e8c:	dffa9097          	auipc	ra,0xdffa9
20060e90:	c42080e7          	jalr	ra,-958(ra) # 9ace <__wrap_memset>
20060e94:	20068537          	lui	a0,0x20068
20060e98:	04000613          	addi	a2,zero,64
20060e9c:	4581                	c.li	a1,0
20060e9e:	68050513          	addi	a0,a0,1664 # 20068680 <g_inic_ipc_ex_msg>
20060ea2:	dffa9097          	auipc	ra,0xdffa9
20060ea6:	c2c080e7          	jalr	ra,-980(ra) # 9ace <__wrap_memset>
20060eaa:	6c040513          	addi	a0,s0,1728
20060eae:	6c040493          	addi	s1,s0,1728
20060eb2:	2a69                	c.jal	2006104c <rtw_init_queue>
20060eb4:	567d                	c.li	a2,-1
20060eb6:	4581                	c.li	a1,0
20060eb8:	00c48513          	addi	a0,s1,12
20060ebc:	999fa0ef          	jal	ra,2005b854 <rtos_sema_create_static>
20060ec0:	567d                	c.li	a2,-1
20060ec2:	4581                	c.li	a1,0
20060ec4:	01048513          	addi	a0,s1,16
20060ec8:	98dfa0ef          	jal	ra,2005b854 <rtos_sema_create_static>
20060ecc:	4888                	c.lw	a0,16(s1)
20060ece:	6c040413          	addi	s0,s0,1728
20060ed2:	a4dfa0ef          	jal	ra,2005b91e <rtos_sema_give>
20060ed6:	43c1a503          	lw	a0,1084(gp) # 20066ce0 <wifi_user_config+0x20>
20060eda:	00a48e23          	sb	a0,28(s1)
20060ede:	0ff57513          	andi	a0,a0,255
20060ee2:	0512                	c.slli	a0,0x4
20060ee4:	fc4fa0ef          	jal	ra,2005b6a8 <rtos_mem_zmalloc>
20060ee8:	cc88                	c.sw	a0,24(s1)
20060eea:	4781                	c.li	a5,0
20060eec:	01c44703          	lbu	a4,28(s0)
20060ef0:	04e7c763          	blt	a5,a4,20060f3e <inic_msg_q_init+0xc8>
20060ef4:	20061637          	lui	a2,0x20061
20060ef8:	200655b7          	lui	a1,0x20065
20060efc:	4799                	c.li	a5,6
20060efe:	41800713          	addi	a4,zero,1048
20060f02:	4681                	c.li	a3,0
20060f04:	df860613          	addi	a2,a2,-520 # 20060df8 <inic_msg_q_task>
20060f08:	89858593          	addi	a1,a1,-1896 # 20064898 <__func__.1+0x14>
20060f0c:	4501                	c.li	a0,0
20060f0e:	c65fa0ef          	jal	ra,2005bb72 <rtos_task_create>
20060f12:	cd11                	c.beqz	a0,20060f2e <inic_msg_q_init+0xb8>
20060f14:	200656b7          	lui	a3,0x20065
20060f18:	200645b7          	lui	a1,0x20064
20060f1c:	8a868693          	addi	a3,a3,-1880 # 200648a8 <__func__.1+0x24>
20060f20:	04500613          	addi	a2,zero,69
20060f24:	7e458593          	addi	a1,a1,2020 # 200647e4 <register_string+0x140>
20060f28:	4509                	c.li	a0,2
20060f2a:	fc3f90ef          	jal	ra,2005aeec <rtk_log_write>
20060f2e:	4785                	c.li	a5,1
20060f30:	40b2                	c.lwsp	ra,12(sp)
20060f32:	00f40a23          	sb	a5,20(s0)
20060f36:	4422                	c.lwsp	s0,8(sp)
20060f38:	4492                	c.lwsp	s1,4(sp)
20060f3a:	0141                	c.addi	sp,16
20060f3c:	8082                	c.jr	ra
20060f3e:	4c18                	c.lw	a4,24(s0)
20060f40:	00479693          	slli	a3,a5,0x4
20060f44:	0785                	c.addi	a5,1
20060f46:	9736                	c.add	a4,a3
20060f48:	00c74683          	lbu	a3,12(a4)
20060f4c:	9af9                	c.andi	a3,-2
20060f4e:	00d70623          	sb	a3,12(a4)
20060f52:	bf69                	c.j	20060eec <inic_msg_q_init+0x76>

20060f54 <inic_msg_enqueue>:
20060f54:	1101                	c.addi	sp,-32
20060f56:	ca26                	c.swsp	s1,20(sp)
20060f58:	c84a                	c.swsp	s2,16(sp)
20060f5a:	c64e                	c.swsp	s3,12(sp)
20060f5c:	200684b7          	lui	s1,0x20068
20060f60:	ce06                	c.swsp	ra,28(sp)
20060f62:	cc22                	c.swsp	s0,24(sp)
20060f64:	89aa                	c.mv	s3,a0
20060f66:	ef2fa0ef          	jal	ra,2005b658 <rtos_critical_enter>
20060f6a:	6c048793          	addi	a5,s1,1728 # 200686c0 <g_ipc_msg_q_priv>
20060f6e:	01c7c683          	lbu	a3,28(a5)
20060f72:	4f90                	c.lw	a2,24(a5)
20060f74:	6c048913          	addi	s2,s1,1728
20060f78:	4781                	c.li	a5,0
20060f7a:	00d7c463          	blt	a5,a3,20060f82 <inic_msg_enqueue+0x2e>
20060f7e:	4401                	c.li	s0,0
20060f80:	a829                	c.j	20060f9a <inic_msg_enqueue+0x46>
20060f82:	00479413          	slli	s0,a5,0x4
20060f86:	9432                	c.add	s0,a2
20060f88:	00c44703          	lbu	a4,12(s0)
20060f8c:	00177593          	andi	a1,a4,1
20060f90:	e1b1                	c.bnez	a1,20060fd4 <inic_msg_enqueue+0x80>
20060f92:	00176713          	ori	a4,a4,1
20060f96:	00e40623          	sb	a4,12(s0)
20060f9a:	ee2fa0ef          	jal	ra,2005b67c <rtos_critical_exit>
20060f9e:	ec0d                	c.bnez	s0,20060fd8 <inic_msg_enqueue+0x84>
20060fa0:	200656b7          	lui	a3,0x20065
20060fa4:	200645b7          	lui	a1,0x20064
20060fa8:	8c068693          	addi	a3,a3,-1856 # 200648c0 <__func__.1+0x3c>
20060fac:	04500613          	addi	a2,zero,69
20060fb0:	7e458593          	addi	a1,a1,2020 # 200647e4 <register_string+0x140>
20060fb4:	4509                	c.li	a0,2
20060fb6:	f37f90ef          	jal	ra,2005aeec <rtk_log_write>
20060fba:	547d                	c.li	s0,-1
20060fbc:	00c92503          	lw	a0,12(s2)
20060fc0:	95ffa0ef          	jal	ra,2005b91e <rtos_sema_give>
20060fc4:	40f2                	c.lwsp	ra,28(sp)
20060fc6:	8522                	c.mv	a0,s0
20060fc8:	4462                	c.lwsp	s0,24(sp)
20060fca:	44d2                	c.lwsp	s1,20(sp)
20060fcc:	4942                	c.lwsp	s2,16(sp)
20060fce:	49b2                	c.lwsp	s3,12(sp)
20060fd0:	6105                	c.addi16sp	sp,32
20060fd2:	8082                	c.jr	ra
20060fd4:	0785                	c.addi	a5,1
20060fd6:	b755                	c.j	20060f7a <inic_msg_enqueue+0x26>
20060fd8:	0009a783          	lw	a5,0(s3)
20060fdc:	00379713          	slli	a4,a5,0x3
20060fe0:	00c44783          	lbu	a5,12(s0)
20060fe4:	8b9d                	c.andi	a5,7
20060fe6:	8fd9                	c.or	a5,a4
20060fe8:	00f40623          	sb	a5,12(s0)
20060fec:	0049a703          	lw	a4,4(s3)
20060ff0:	9be5                	c.andi	a5,-7
20060ff2:	c418                	c.sw	a4,8(s0)
20060ff4:	00c9a703          	lw	a4,12(s3)
20060ff8:	8b0d                	c.andi	a4,3
20060ffa:	0706                	c.slli	a4,0x1
20060ffc:	8fd9                	c.or	a5,a4
20060ffe:	00f40623          	sb	a5,12(s0)
20061002:	e56fa0ef          	jal	ra,2005b658 <rtos_critical_enter>
20061006:	8522                	c.mv	a0,s0
20061008:	6c048593          	addi	a1,s1,1728
2006100c:	201d                	c.jal	20061032 <rtw_list_insert_tail>
2006100e:	4405                	c.li	s0,1
20061010:	e6cfa0ef          	jal	ra,2005b67c <rtos_critical_exit>
20061014:	b765                	c.j	20060fbc <inic_msg_enqueue+0x68>

20061016 <inic_msg_get_queue_status>:
20061016:	200687b7          	lui	a5,0x20068
2006101a:	6d47c503          	lbu	a0,1748(a5) # 200686d4 <g_ipc_msg_q_priv+0x14>
2006101e:	8082                	c.jr	ra

20061020 <rtw_init_listhead>:
20061020:	c108                	c.sw	a0,0(a0)
20061022:	c148                	c.sw	a0,4(a0)
20061024:	8082                	c.jr	ra

20061026 <rtw_list_insert_head>:
20061026:	419c                	c.lw	a5,0(a1)
20061028:	c3c8                	c.sw	a0,4(a5)
2006102a:	c11c                	c.sw	a5,0(a0)
2006102c:	c14c                	c.sw	a1,4(a0)
2006102e:	c188                	c.sw	a0,0(a1)
20061030:	8082                	c.jr	ra

20061032 <rtw_list_insert_tail>:
20061032:	41dc                	c.lw	a5,4(a1)
20061034:	c1c8                	c.sw	a0,4(a1)
20061036:	c10c                	c.sw	a1,0(a0)
20061038:	c15c                	c.sw	a5,4(a0)
2006103a:	c388                	c.sw	a0,0(a5)
2006103c:	8082                	c.jr	ra

2006103e <rtw_list_delete>:
2006103e:	415c                	c.lw	a5,4(a0)
20061040:	4118                	c.lw	a4,0(a0)
20061042:	c35c                	c.sw	a5,4(a4)
20061044:	c398                	c.sw	a4,0(a5)
20061046:	c108                	c.sw	a0,0(a0)
20061048:	c148                	c.sw	a0,4(a0)
2006104a:	8082                	c.jr	ra

2006104c <rtw_init_queue>:
2006104c:	c108                	c.sw	a0,0(a0)
2006104e:	fe81a783          	lw	a5,-24(gp) # 2006688c <p_wifi_rom2flash>
20061052:	c148                	c.sw	a0,4(a0)
20061054:	0521                	c.addi	a0,8
20061056:	57dc                	c.lw	a5,44(a5)
20061058:	8782                	c.jr	a5

2006105a <rtw_queue_empty>:
2006105a:	411c                	c.lw	a5,0(a0)
2006105c:	40a78533          	sub	a0,a5,a0
20061060:	00153513          	sltiu	a0,a0,1
20061064:	8082                	c.jr	ra

20061066 <rtw_end_of_queue_search>:
20061066:	8d0d                	c.sub	a0,a1
20061068:	00153513          	sltiu	a0,a0,1
2006106c:	8082                	c.jr	ra

2006106e <timer_wrapper>:
2006106e:	1101                	c.addi	sp,-32
20061070:	ca26                	c.swsp	s1,20(sp)
20061072:	cc22                	c.swsp	s0,24(sp)
20061074:	c84a                	c.swsp	s2,16(sp)
20061076:	c64e                	c.swsp	s3,12(sp)
20061078:	c452                	c.swsp	s4,8(sp)
2006107a:	ce06                	c.swsp	ra,28(sp)
2006107c:	89aa                	c.mv	s3,a0
2006107e:	ddafa0ef          	jal	ra,2005b658 <rtos_critical_enter>
20061082:	ff41a403          	lw	s0,-12(gp) # 20066898 <timer_table>
20061086:	4901                	c.li	s2,0
20061088:	ff418a13          	addi	s4,gp,-12 # 20066898 <timer_table>
2006108c:	85a2                	c.mv	a1,s0
2006108e:	ff418513          	addi	a0,gp,-12 # 20066898 <timer_table>
20061092:	fd5ff0ef          	jal	ra,20061066 <rtw_end_of_queue_search>
20061096:	c90d                	c.beqz	a0,200610c8 <timer_wrapper+0x5a>
20061098:	de4fa0ef          	jal	ra,2005b67c <rtos_critical_exit>
2006109c:	03441c63          	bne	s0,s4,200610d4 <timer_wrapper+0x66>
200610a0:	4462                	c.lwsp	s0,24(sp)
200610a2:	40f2                	c.lwsp	ra,28(sp)
200610a4:	44d2                	c.lwsp	s1,20(sp)
200610a6:	4942                	c.lwsp	s2,16(sp)
200610a8:	49b2                	c.lwsp	s3,12(sp)
200610aa:	4a22                	c.lwsp	s4,8(sp)
200610ac:	200656b7          	lui	a3,0x20065
200610b0:	200655b7          	lui	a1,0x20065
200610b4:	8d868693          	addi	a3,a3,-1832 # 200648d8 <__func__.1+0x54>
200610b8:	04500613          	addi	a2,zero,69
200610bc:	90858593          	addi	a1,a1,-1784 # 20064908 <__func__.1+0x84>
200610c0:	4509                	c.li	a0,2
200610c2:	6105                	c.addi16sp	sp,32
200610c4:	e29f906f          	jal	zero,2005aeec <rtk_log_write>
200610c8:	441c                	c.lw	a5,8(s0)
200610ca:	8922                	c.mv	s2,s0
200610cc:	fd3786e3          	beq	a5,s3,20061098 <timer_wrapper+0x2a>
200610d0:	4000                	c.lw	s0,0(s0)
200610d2:	bf6d                	c.j	2006108c <timer_wrapper+0x1e>
200610d4:	01092783          	lw	a5,16(s2)
200610d8:	cb99                	c.beqz	a5,200610ee <timer_wrapper+0x80>
200610da:	4462                	c.lwsp	s0,24(sp)
200610dc:	00c92503          	lw	a0,12(s2)
200610e0:	40f2                	c.lwsp	ra,28(sp)
200610e2:	44d2                	c.lwsp	s1,20(sp)
200610e4:	4942                	c.lwsp	s2,16(sp)
200610e6:	49b2                	c.lwsp	s3,12(sp)
200610e8:	4a22                	c.lwsp	s4,8(sp)
200610ea:	6105                	c.addi16sp	sp,32
200610ec:	8782                	c.jr	a5
200610ee:	40f2                	c.lwsp	ra,28(sp)
200610f0:	4462                	c.lwsp	s0,24(sp)
200610f2:	44d2                	c.lwsp	s1,20(sp)
200610f4:	4942                	c.lwsp	s2,16(sp)
200610f6:	49b2                	c.lwsp	s3,12(sp)
200610f8:	4a22                	c.lwsp	s4,8(sp)
200610fa:	6105                	c.addi16sp	sp,32
200610fc:	8082                	c.jr	ra

200610fe <init_timer_wrapper>:
200610fe:	1141                	c.addi	sp,-16
20061100:	ff418513          	addi	a0,gp,-12 # 20066898 <timer_table>
20061104:	c606                	c.swsp	ra,12(sp)
20061106:	f1bff0ef          	jal	ra,20061020 <rtw_init_listhead>
2006110a:	40b2                	c.lwsp	ra,12(sp)
2006110c:	fe01ae23          	sw	zero,-4(gp) # 200668a0 <timer_used_num>
20061110:	fe01a823          	sw	zero,-16(gp) # 20066894 <max_timer_used_num>
20061114:	0141                	c.addi	sp,16
20061116:	8082                	c.jr	ra

20061118 <init_timer>:
20061118:	491c                	c.lw	a5,16(a0)
2006111a:	cbc1                	c.beqz	a5,200611aa <init_timer+0x92>
2006111c:	1141                	c.addi	sp,-16
2006111e:	c422                	c.swsp	s0,8(sp)
20061120:	c606                	c.swsp	ra,12(sp)
20061122:	842a                	c.mv	s0,a0
20061124:	4508                	c.lw	a0,8(a0)
20061126:	e535                	c.bnez	a0,20061192 <init_timer+0x7a>
20061128:	200617b7          	lui	a5,0x20061
2006112c:	06e78793          	addi	a5,a5,110 # 2006106e <timer_wrapper>
20061130:	4701                	c.li	a4,0
20061132:	56fd                	c.li	a3,-1
20061134:	4601                	c.li	a2,0
20061136:	00840513          	addi	a0,s0,8
2006113a:	afdfa0ef          	jal	ra,2005bc36 <rtos_timer_create_static>
2006113e:	441c                	c.lw	a5,8(s0)
20061140:	e38d                	c.bnez	a5,20061162 <init_timer+0x4a>
20061142:	4422                	c.lwsp	s0,8(sp)
20061144:	40b2                	c.lwsp	ra,12(sp)
20061146:	200656b7          	lui	a3,0x20065
2006114a:	200655b7          	lui	a1,0x20065
2006114e:	91068693          	addi	a3,a3,-1776 # 20064910 <__func__.1+0x8c>
20061152:	04500613          	addi	a2,zero,69
20061156:	90858593          	addi	a1,a1,-1784 # 20064908 <__func__.1+0x84>
2006115a:	4509                	c.li	a0,2
2006115c:	0141                	c.addi	sp,16
2006115e:	d8ff906f          	jal	zero,2005aeec <rtk_log_write>
20061162:	cf6fa0ef          	jal	ra,2005b658 <rtos_critical_enter>
20061166:	ff418593          	addi	a1,gp,-12 # 20066898 <timer_table>
2006116a:	8522                	c.mv	a0,s0
2006116c:	ebbff0ef          	jal	ra,20061026 <rtw_list_insert_head>
20061170:	d0cfa0ef          	jal	ra,2005b67c <rtos_critical_exit>
20061174:	ffc18713          	addi	a4,gp,-4 # 200668a0 <timer_used_num>
20061178:	431c                	c.lw	a5,0(a4)
2006117a:	0785                	c.addi	a5,1
2006117c:	c31c                	c.sw	a5,0(a4)
2006117e:	ff018713          	addi	a4,gp,-16 # 20066894 <max_timer_used_num>
20061182:	4314                	c.lw	a3,0(a4)
20061184:	00f6d363          	bge	a3,a5,2006118a <init_timer+0x72>
20061188:	c31c                	c.sw	a5,0(a4)
2006118a:	40b2                	c.lwsp	ra,12(sp)
2006118c:	4422                	c.lwsp	s0,8(sp)
2006118e:	0141                	c.addi	sp,16
20061190:	8082                	c.jr	ra
20061192:	c75fa0ef          	jal	ra,2005be06 <rtos_timer_is_timer_active>
20061196:	4785                	c.li	a5,1
20061198:	fef519e3          	bne	a0,a5,2006118a <init_timer+0x72>
2006119c:	4408                	c.lw	a0,8(s0)
2006119e:	4422                	c.lwsp	s0,8(sp)
200611a0:	40b2                	c.lwsp	ra,12(sp)
200611a2:	55fd                	c.li	a1,-1
200611a4:	0141                	c.addi	sp,16
200611a6:	ba3fa06f          	jal	zero,2005bd48 <rtos_timer_stop>
200611aa:	8082                	c.jr	ra

200611ac <mod_timer>:
200611ac:	1141                	c.addi	sp,-16
200611ae:	c422                	c.swsp	s0,8(sp)
200611b0:	c226                	c.swsp	s1,4(sp)
200611b2:	c606                	c.swsp	ra,12(sp)
200611b4:	842a                	c.mv	s0,a0
200611b6:	4508                	c.lw	a0,8(a0)
200611b8:	84ae                	c.mv	s1,a1
200611ba:	e915                	c.bnez	a0,200611ee <mod_timer+0x42>
200611bc:	20065537          	lui	a0,0x20065
200611c0:	92850513          	addi	a0,a0,-1752 # 20064928 <__func__.1+0xa4>
200611c4:	95cfe0ef          	jal	ra,2005f320 <DiagPrintf_minimal>
200611c8:	4408                	c.lw	a0,8(s0)
200611ca:	cd05                	c.beqz	a0,20061202 <mod_timer+0x56>
200611cc:	567d                	c.li	a2,-1
200611ce:	85a6                	c.mv	a1,s1
200611d0:	bd5fa0ef          	jal	ra,2005bda4 <rtos_timer_change_period>
200611d4:	57fd                	c.li	a5,-1
200611d6:	02f51663          	bne	a0,a5,20061202 <mod_timer+0x56>
200611da:	4422                	c.lwsp	s0,8(sp)
200611dc:	40b2                	c.lwsp	ra,12(sp)
200611de:	4492                	c.lwsp	s1,4(sp)
200611e0:	20065537          	lui	a0,0x20065
200611e4:	94050513          	addi	a0,a0,-1728 # 20064940 <__func__.1+0xbc>
200611e8:	0141                	c.addi	sp,16
200611ea:	936fe06f          	jal	zero,2005f320 <DiagPrintf_minimal>
200611ee:	c19fa0ef          	jal	ra,2005be06 <rtos_timer_is_timer_active>
200611f2:	4785                	c.li	a5,1
200611f4:	fcf51ae3          	bne	a0,a5,200611c8 <mod_timer+0x1c>
200611f8:	4408                	c.lw	a0,8(s0)
200611fa:	55fd                	c.li	a1,-1
200611fc:	b4dfa0ef          	jal	ra,2005bd48 <rtos_timer_stop>
20061200:	b7e1                	c.j	200611c8 <mod_timer+0x1c>
20061202:	40b2                	c.lwsp	ra,12(sp)
20061204:	4422                	c.lwsp	s0,8(sp)
20061206:	4492                	c.lwsp	s1,4(sp)
20061208:	0141                	c.addi	sp,16
2006120a:	8082                	c.jr	ra

2006120c <cancel_timer_ex>:
2006120c:	451c                	c.lw	a5,8(a0)
2006120e:	c7ad                	c.beqz	a5,20061278 <cancel_timer_ex+0x6c>
20061210:	1101                	c.addi	sp,-32
20061212:	c84a                	c.swsp	s2,16(sp)
20061214:	cc22                	c.swsp	s0,24(sp)
20061216:	ca26                	c.swsp	s1,20(sp)
20061218:	c64e                	c.swsp	s3,12(sp)
2006121a:	ce06                	c.swsp	ra,28(sp)
2006121c:	84aa                	c.mv	s1,a0
2006121e:	c3afa0ef          	jal	ra,2005b658 <rtos_critical_enter>
20061222:	ff41a403          	lw	s0,-12(gp) # 20066898 <timer_table>
20061226:	ff418993          	addi	s3,gp,-12 # 20066898 <timer_table>
2006122a:	85a2                	c.mv	a1,s0
2006122c:	ff418513          	addi	a0,gp,-12 # 20066898 <timer_table>
20061230:	e37ff0ef          	jal	ra,20061066 <rtw_end_of_queue_search>
20061234:	e509                	c.bnez	a0,2006123e <cancel_timer_ex+0x32>
20061236:	4418                	c.lw	a4,8(s0)
20061238:	449c                	c.lw	a5,8(s1)
2006123a:	02f71463          	bne	a4,a5,20061262 <cancel_timer_ex+0x56>
2006123e:	c3efa0ef          	jal	ra,2005b67c <rtos_critical_exit>
20061242:	4488                	c.lw	a0,8(s1)
20061244:	03341163          	bne	s0,s3,20061266 <cancel_timer_ex+0x5a>
20061248:	4462                	c.lwsp	s0,24(sp)
2006124a:	40f2                	c.lwsp	ra,28(sp)
2006124c:	44d2                	c.lwsp	s1,20(sp)
2006124e:	4942                	c.lwsp	s2,16(sp)
20061250:	49b2                	c.lwsp	s3,12(sp)
20061252:	85aa                	c.mv	a1,a0
20061254:	20065537          	lui	a0,0x20065
20061258:	95050513          	addi	a0,a0,-1712 # 20064950 <__func__.1+0xcc>
2006125c:	6105                	c.addi16sp	sp,32
2006125e:	8c2fe06f          	jal	zero,2005f320 <DiagPrintf_minimal>
20061262:	4000                	c.lw	s0,0(s0)
20061264:	b7d9                	c.j	2006122a <cancel_timer_ex+0x1e>
20061266:	4462                	c.lwsp	s0,24(sp)
20061268:	40f2                	c.lwsp	ra,28(sp)
2006126a:	44d2                	c.lwsp	s1,20(sp)
2006126c:	4942                	c.lwsp	s2,16(sp)
2006126e:	49b2                	c.lwsp	s3,12(sp)
20061270:	55fd                	c.li	a1,-1
20061272:	6105                	c.addi16sp	sp,32
20061274:	ad5fa06f          	jal	zero,2005bd48 <rtos_timer_stop>
20061278:	8082                	c.jr	ra

2006127a <del_timer_sync>:
2006127a:	451c                	c.lw	a5,8(a0)
2006127c:	cfbd                	c.beqz	a5,200612fa <del_timer_sync+0x80>
2006127e:	1101                	c.addi	sp,-32
20061280:	c84a                	c.swsp	s2,16(sp)
20061282:	cc22                	c.swsp	s0,24(sp)
20061284:	ca26                	c.swsp	s1,20(sp)
20061286:	c64e                	c.swsp	s3,12(sp)
20061288:	ce06                	c.swsp	ra,28(sp)
2006128a:	842a                	c.mv	s0,a0
2006128c:	bccfa0ef          	jal	ra,2005b658 <rtos_critical_enter>
20061290:	ff41a483          	lw	s1,-12(gp) # 20066898 <timer_table>
20061294:	ff418993          	addi	s3,gp,-12 # 20066898 <timer_table>
20061298:	85a6                	c.mv	a1,s1
2006129a:	ff418513          	addi	a0,gp,-12 # 20066898 <timer_table>
2006129e:	dc9ff0ef          	jal	ra,20061066 <rtw_end_of_queue_search>
200612a2:	e901                	c.bnez	a0,200612b2 <del_timer_sync+0x38>
200612a4:	4498                	c.lw	a4,8(s1)
200612a6:	441c                	c.lw	a5,8(s0)
200612a8:	02f71563          	bne	a4,a5,200612d2 <del_timer_sync+0x58>
200612ac:	8526                	c.mv	a0,s1
200612ae:	d91ff0ef          	jal	ra,2006103e <rtw_list_delete>
200612b2:	bcafa0ef          	jal	ra,2005b67c <rtos_critical_exit>
200612b6:	03349063          	bne	s1,s3,200612d6 <del_timer_sync+0x5c>
200612ba:	4462                	c.lwsp	s0,24(sp)
200612bc:	40f2                	c.lwsp	ra,28(sp)
200612be:	44d2                	c.lwsp	s1,20(sp)
200612c0:	4942                	c.lwsp	s2,16(sp)
200612c2:	49b2                	c.lwsp	s3,12(sp)
200612c4:	20065537          	lui	a0,0x20065
200612c8:	96850513          	addi	a0,a0,-1688 # 20064968 <__func__.1+0xe4>
200612cc:	6105                	c.addi16sp	sp,32
200612ce:	852fe06f          	jal	zero,2005f320 <DiagPrintf_minimal>
200612d2:	4084                	c.lw	s1,0(s1)
200612d4:	b7d1                	c.j	20061298 <del_timer_sync+0x1e>
200612d6:	4408                	c.lw	a0,8(s0)
200612d8:	55fd                	c.li	a1,-1
200612da:	9dffa0ef          	jal	ra,2005bcb8 <rtos_timer_delete_static>
200612de:	ffc18793          	addi	a5,gp,-4 # 200668a0 <timer_used_num>
200612e2:	4398                	c.lw	a4,0(a5)
200612e4:	00042423          	sw	zero,8(s0)
200612e8:	177d                	c.addi	a4,-1
200612ea:	c398                	c.sw	a4,0(a5)
200612ec:	40f2                	c.lwsp	ra,28(sp)
200612ee:	4462                	c.lwsp	s0,24(sp)
200612f0:	44d2                	c.lwsp	s1,20(sp)
200612f2:	4942                	c.lwsp	s2,16(sp)
200612f4:	49b2                	c.lwsp	s3,12(sp)
200612f6:	6105                	c.addi16sp	sp,32
200612f8:	8082                	c.jr	ra
200612fa:	8082                	c.jr	ra

200612fc <rtw_init_timer>:
200612fc:	c910                	c.sw	a2,16(a0)
200612fe:	c554                	c.sw	a3,12(a0)
20061300:	85ba                	c.mv	a1,a4
20061302:	e17ff06f          	jal	zero,20061118 <init_timer>

20061306 <rtw_set_timer>:
20061306:	ea7ff06f          	jal	zero,200611ac <mod_timer>

2006130a <rtw_cancel_timer>:
2006130a:	1141                	c.addi	sp,-16
2006130c:	c606                	c.swsp	ra,12(sp)
2006130e:	effff0ef          	jal	ra,2006120c <cancel_timer_ex>
20061312:	40b2                	c.lwsp	ra,12(sp)
20061314:	4501                	c.li	a0,0
20061316:	0141                	c.addi	sp,16
20061318:	8082                	c.jr	ra

2006131a <rtw_del_timer>:
2006131a:	f61ff06f          	jal	zero,2006127a <del_timer_sync>

2006131e <rtw_acquire_wakelock>:
2006131e:	1141                	c.addi	sp,-16
20061320:	c606                	c.swsp	ra,12(sp)
20061322:	cd4f80ef          	jal	ra,200597f6 <pmu_yield_os_check>
20061326:	c511                	c.beqz	a0,20061332 <rtw_acquire_wakelock+0x14>
20061328:	40b2                	c.lwsp	ra,12(sp)
2006132a:	4505                	c.li	a0,1
2006132c:	0141                	c.addi	sp,16
2006132e:	d14f806f          	jal	zero,20059842 <pmu_acquire_wakelock>
20061332:	40b2                	c.lwsp	ra,12(sp)
20061334:	0141                	c.addi	sp,16
20061336:	8082                	c.jr	ra

20061338 <rtw_release_wakelock>:
20061338:	1141                	c.addi	sp,-16
2006133a:	c606                	c.swsp	ra,12(sp)
2006133c:	cbaf80ef          	jal	ra,200597f6 <pmu_yield_os_check>
20061340:	c511                	c.beqz	a0,2006134c <rtw_release_wakelock+0x14>
20061342:	40b2                	c.lwsp	ra,12(sp)
20061344:	4505                	c.li	a0,1
20061346:	0141                	c.addi	sp,16
20061348:	d20f806f          	jal	zero,20059868 <pmu_release_wakelock>
2006134c:	40b2                	c.lwsp	ra,12(sp)
2006134e:	0141                	c.addi	sp,16
20061350:	8082                	c.jr	ra

20061352 <rtw_wakelock_timeout>:
20061352:	1141                	c.addi	sp,-16
20061354:	c422                	c.swsp	s0,8(sp)
20061356:	c606                	c.swsp	ra,12(sp)
20061358:	842a                	c.mv	s0,a0
2006135a:	c9cf80ef          	jal	ra,200597f6 <pmu_yield_os_check>
2006135e:	c519                	c.beqz	a0,2006136c <rtw_wakelock_timeout+0x1a>
20061360:	8522                	c.mv	a0,s0
20061362:	4422                	c.lwsp	s0,8(sp)
20061364:	40b2                	c.lwsp	ra,12(sp)
20061366:	0141                	c.addi	sp,16
20061368:	bb2f806f          	jal	zero,2005971a <pmu_set_sysactive_time>
2006136c:	40b2                	c.lwsp	ra,12(sp)
2006136e:	4422                	c.lwsp	s0,8(sp)
20061370:	0141                	c.addi	sp,16
20061372:	8082                	c.jr	ra

20061374 <deregister_tm_clones>:
20061374:	20058537          	lui	a0,0x20058
20061378:	200587b7          	lui	a5,0x20058
2006137c:	06050713          	addi	a4,a0,96 # 20058060 <BOOT_InitGP>
20061380:	06078793          	addi	a5,a5,96 # 20058060 <BOOT_InitGP>
20061384:	00e78863          	beq	a5,a4,20061394 <deregister_tm_clones+0x20>
20061388:	00000793          	addi	a5,zero,0
2006138c:	c781                	c.beqz	a5,20061394 <deregister_tm_clones+0x20>
2006138e:	06050513          	addi	a0,a0,96
20061392:	8782                	c.jr	a5
20061394:	8082                	c.jr	ra

20061396 <register_tm_clones>:
20061396:	200587b7          	lui	a5,0x20058
2006139a:	20058537          	lui	a0,0x20058
2006139e:	06078593          	addi	a1,a5,96 # 20058060 <BOOT_InitGP>
200613a2:	06050793          	addi	a5,a0,96 # 20058060 <BOOT_InitGP>
200613a6:	8d9d                	c.sub	a1,a5
200613a8:	8589                	c.srai	a1,0x2
200613aa:	4789                	c.li	a5,2
200613ac:	02f5c5b3          	div	a1,a1,a5
200613b0:	c599                	c.beqz	a1,200613be <register_tm_clones+0x28>
200613b2:	00000793          	addi	a5,zero,0
200613b6:	c781                	c.beqz	a5,200613be <register_tm_clones+0x28>
200613b8:	06050513          	addi	a0,a0,96
200613bc:	8782                	c.jr	a5
200613be:	8082                	c.jr	ra

200613c0 <__do_global_dtors_aux>:
200613c0:	200697b7          	lui	a5,0x20069
200613c4:	8307c703          	lbu	a4,-2000(a5) # 20068830 <completed.1>
200613c8:	eb05                	c.bnez	a4,200613f8 <__do_global_dtors_aux+0x38>
200613ca:	1141                	c.addi	sp,-16
200613cc:	c422                	c.swsp	s0,8(sp)
200613ce:	c606                	c.swsp	ra,12(sp)
200613d0:	843e                	c.mv	s0,a5
200613d2:	374d                	c.jal	20061374 <deregister_tm_clones>
200613d4:	00000793          	addi	a5,zero,0
200613d8:	cb89                	c.beqz	a5,200613ea <__do_global_dtors_aux+0x2a>
200613da:	20058537          	lui	a0,0x20058
200613de:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
200613e2:	00000097          	auipc	ra,0x0
200613e6:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
200613ea:	4785                	c.li	a5,1
200613ec:	40b2                	c.lwsp	ra,12(sp)
200613ee:	82f40823          	sb	a5,-2000(s0)
200613f2:	4422                	c.lwsp	s0,8(sp)
200613f4:	0141                	c.addi	sp,16
200613f6:	8082                	c.jr	ra
200613f8:	8082                	c.jr	ra

200613fa <frame_dummy>:
200613fa:	00000793          	addi	a5,zero,0
200613fe:	c38d                	c.beqz	a5,20061420 <frame_dummy+0x26>
20061400:	200695b7          	lui	a1,0x20069
20061404:	20058537          	lui	a0,0x20058
20061408:	1141                	c.addi	sp,-16
2006140a:	83458593          	addi	a1,a1,-1996 # 20068834 <object.0>
2006140e:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
20061412:	c606                	c.swsp	ra,12(sp)
20061414:	00000097          	auipc	ra,0x0
20061418:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
2006141c:	40b2                	c.lwsp	ra,12(sp)
2006141e:	0141                	c.addi	sp,16
20061420:	bf9d                	c.j	20061396 <register_tm_clones>

20061422 <__clzsi2>:
20061422:	67c1                	c.lui	a5,0x10
20061424:	02f57663          	bgeu	a0,a5,20061450 <__clzsi2+0x2e>
20061428:	0ff00793          	addi	a5,zero,255
2006142c:	00a7b7b3          	sltu	a5,a5,a0
20061430:	078e                	c.slli	a5,0x3
20061432:	20065737          	lui	a4,0x20065
20061436:	02000693          	addi	a3,zero,32
2006143a:	8e9d                	c.sub	a3,a5
2006143c:	00f55533          	srl	a0,a0,a5
20061440:	97870793          	addi	a5,a4,-1672 # 20064978 <__clz_tab>
20061444:	953e                	c.add	a0,a5
20061446:	00054503          	lbu	a0,0(a0)
2006144a:	40a68533          	sub	a0,a3,a0
2006144e:	8082                	c.jr	ra
20061450:	01000737          	lui	a4,0x1000
20061454:	47c1                	c.li	a5,16
20061456:	fce56ee3          	bltu	a0,a4,20061432 <__clzsi2+0x10>
2006145a:	47e1                	c.li	a5,24
2006145c:	bfd9                	c.j	20061432 <__clzsi2+0x10>

2006145e <__udivdi3>:
2006145e:	88aa                	c.mv	a7,a0
20061460:	87ae                	c.mv	a5,a1
20061462:	8832                	c.mv	a6,a2
20061464:	8536                	c.mv	a0,a3
20061466:	8346                	c.mv	t1,a7
20061468:	20069663          	bne	a3,zero,20061674 <__udivdi3+0x216>
2006146c:	200656b7          	lui	a3,0x20065
20061470:	97868693          	addi	a3,a3,-1672 # 20064978 <__clz_tab>
20061474:	0ac5ff63          	bgeu	a1,a2,20061532 <__udivdi3+0xd4>
20061478:	6741                	c.lui	a4,0x10
2006147a:	0ae67563          	bgeu	a2,a4,20061524 <__udivdi3+0xc6>
2006147e:	0ff00713          	addi	a4,zero,255
20061482:	00c73733          	sltu	a4,a4,a2
20061486:	070e                	c.slli	a4,0x3
20061488:	00e65533          	srl	a0,a2,a4
2006148c:	96aa                	c.add	a3,a0
2006148e:	0006c683          	lbu	a3,0(a3)
20061492:	02000513          	addi	a0,zero,32
20061496:	9736                	c.add	a4,a3
20061498:	40e506b3          	sub	a3,a0,a4
2006149c:	00e50b63          	beq	a0,a4,200614b2 <__udivdi3+0x54>
200614a0:	00d795b3          	sll	a1,a5,a3
200614a4:	00e8d733          	srl	a4,a7,a4
200614a8:	00d61833          	sll	a6,a2,a3
200614ac:	8dd9                	c.or	a1,a4
200614ae:	00d89333          	sll	t1,a7,a3
200614b2:	01085893          	srli	a7,a6,0x10
200614b6:	0315d6b3          	divu	a3,a1,a7
200614ba:	01081613          	slli	a2,a6,0x10
200614be:	8241                	c.srli	a2,0x10
200614c0:	01035793          	srli	a5,t1,0x10
200614c4:	0315f733          	remu	a4,a1,a7
200614c8:	8536                	c.mv	a0,a3
200614ca:	02d605b3          	mul	a1,a2,a3
200614ce:	0742                	c.slli	a4,0x10
200614d0:	8fd9                	c.or	a5,a4
200614d2:	00b7fc63          	bgeu	a5,a1,200614ea <__udivdi3+0x8c>
200614d6:	97c2                	c.add	a5,a6
200614d8:	fff68513          	addi	a0,a3,-1
200614dc:	0107e763          	bltu	a5,a6,200614ea <__udivdi3+0x8c>
200614e0:	00b7f563          	bgeu	a5,a1,200614ea <__udivdi3+0x8c>
200614e4:	ffe68513          	addi	a0,a3,-2
200614e8:	97c2                	c.add	a5,a6
200614ea:	8f8d                	c.sub	a5,a1
200614ec:	0317f733          	remu	a4,a5,a7
200614f0:	0342                	c.slli	t1,0x10
200614f2:	01035313          	srli	t1,t1,0x10
200614f6:	0317d7b3          	divu	a5,a5,a7
200614fa:	0742                	c.slli	a4,0x10
200614fc:	00676333          	or	t1,a4,t1
20061500:	02f606b3          	mul	a3,a2,a5
20061504:	863e                	c.mv	a2,a5
20061506:	00d37b63          	bgeu	t1,a3,2006151c <__udivdi3+0xbe>
2006150a:	9342                	c.add	t1,a6
2006150c:	fff78613          	addi	a2,a5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061510:	01036663          	bltu	t1,a6,2006151c <__udivdi3+0xbe>
20061514:	00d37463          	bgeu	t1,a3,2006151c <__udivdi3+0xbe>
20061518:	ffe78613          	addi	a2,a5,-2
2006151c:	0542                	c.slli	a0,0x10
2006151e:	8d51                	c.or	a0,a2
20061520:	4581                	c.li	a1,0
20061522:	a85d                	c.j	200615d8 <__udivdi3+0x17a>
20061524:	01000537          	lui	a0,0x1000
20061528:	4741                	c.li	a4,16
2006152a:	f4a66fe3          	bltu	a2,a0,20061488 <__udivdi3+0x2a>
2006152e:	4761                	c.li	a4,24
20061530:	bfa1                	c.j	20061488 <__udivdi3+0x2a>
20061532:	e601                	c.bnez	a2,2006153a <__udivdi3+0xdc>
20061534:	4705                	c.li	a4,1
20061536:	02c75833          	divu	a6,a4,a2
2006153a:	6741                	c.lui	a4,0x10
2006153c:	08e87f63          	bgeu	a6,a4,200615da <__udivdi3+0x17c>
20061540:	0ff00713          	addi	a4,zero,255
20061544:	01077363          	bgeu	a4,a6,2006154a <__udivdi3+0xec>
20061548:	4521                	c.li	a0,8
2006154a:	00a85733          	srl	a4,a6,a0
2006154e:	96ba                	c.add	a3,a4
20061550:	0006c703          	lbu	a4,0(a3)
20061554:	02000613          	addi	a2,zero,32
20061558:	972a                	c.add	a4,a0
2006155a:	40e606b3          	sub	a3,a2,a4
2006155e:	08e61563          	bne	a2,a4,200615e8 <__udivdi3+0x18a>
20061562:	410787b3          	sub	a5,a5,a6
20061566:	4585                	c.li	a1,1
20061568:	01085893          	srli	a7,a6,0x10
2006156c:	01081613          	slli	a2,a6,0x10
20061570:	8241                	c.srli	a2,0x10
20061572:	01035713          	srli	a4,t1,0x10
20061576:	0317f6b3          	remu	a3,a5,a7
2006157a:	0317d7b3          	divu	a5,a5,a7
2006157e:	06c2                	c.slli	a3,0x10
20061580:	8f55                	c.or	a4,a3
20061582:	02f60e33          	mul	t3,a2,a5
20061586:	853e                	c.mv	a0,a5
20061588:	01c77c63          	bgeu	a4,t3,200615a0 <__udivdi3+0x142>
2006158c:	9742                	c.add	a4,a6
2006158e:	fff78513          	addi	a0,a5,-1
20061592:	01076763          	bltu	a4,a6,200615a0 <__udivdi3+0x142>
20061596:	01c77563          	bgeu	a4,t3,200615a0 <__udivdi3+0x142>
2006159a:	ffe78513          	addi	a0,a5,-2
2006159e:	9742                	c.add	a4,a6
200615a0:	41c70733          	sub	a4,a4,t3
200615a4:	031777b3          	remu	a5,a4,a7
200615a8:	0342                	c.slli	t1,0x10
200615aa:	01035313          	srli	t1,t1,0x10
200615ae:	03175733          	divu	a4,a4,a7
200615b2:	07c2                	c.slli	a5,0x10
200615b4:	0067e333          	or	t1,a5,t1
200615b8:	02e606b3          	mul	a3,a2,a4
200615bc:	863a                	c.mv	a2,a4
200615be:	00d37b63          	bgeu	t1,a3,200615d4 <__udivdi3+0x176>
200615c2:	9342                	c.add	t1,a6
200615c4:	fff70613          	addi	a2,a4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
200615c8:	01036663          	bltu	t1,a6,200615d4 <__udivdi3+0x176>
200615cc:	00d37463          	bgeu	t1,a3,200615d4 <__udivdi3+0x176>
200615d0:	ffe70613          	addi	a2,a4,-2
200615d4:	0542                	c.slli	a0,0x10
200615d6:	8d51                	c.or	a0,a2
200615d8:	8082                	c.jr	ra
200615da:	01000737          	lui	a4,0x1000
200615de:	4541                	c.li	a0,16
200615e0:	f6e865e3          	bltu	a6,a4,2006154a <__udivdi3+0xec>
200615e4:	4561                	c.li	a0,24
200615e6:	b795                	c.j	2006154a <__udivdi3+0xec>
200615e8:	00d81833          	sll	a6,a6,a3
200615ec:	00e7d533          	srl	a0,a5,a4
200615f0:	00d89333          	sll	t1,a7,a3
200615f4:	00d797b3          	sll	a5,a5,a3
200615f8:	00e8d733          	srl	a4,a7,a4
200615fc:	01085893          	srli	a7,a6,0x10
20061600:	00f76633          	or	a2,a4,a5
20061604:	03157733          	remu	a4,a0,a7
20061608:	01081793          	slli	a5,a6,0x10
2006160c:	83c1                	c.srli	a5,0x10
2006160e:	01065593          	srli	a1,a2,0x10
20061612:	03155533          	divu	a0,a0,a7
20061616:	0742                	c.slli	a4,0x10
20061618:	8f4d                	c.or	a4,a1
2006161a:	02a786b3          	mul	a3,a5,a0
2006161e:	85aa                	c.mv	a1,a0
20061620:	00d77c63          	bgeu	a4,a3,20061638 <__udivdi3+0x1da>
20061624:	9742                	c.add	a4,a6
20061626:	fff50593          	addi	a1,a0,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
2006162a:	01076763          	bltu	a4,a6,20061638 <__udivdi3+0x1da>
2006162e:	00d77563          	bgeu	a4,a3,20061638 <__udivdi3+0x1da>
20061632:	ffe50593          	addi	a1,a0,-2
20061636:	9742                	c.add	a4,a6
20061638:	40d706b3          	sub	a3,a4,a3
2006163c:	0316f733          	remu	a4,a3,a7
20061640:	0642                	c.slli	a2,0x10
20061642:	8241                	c.srli	a2,0x10
20061644:	0316d6b3          	divu	a3,a3,a7
20061648:	0742                	c.slli	a4,0x10
2006164a:	02d78533          	mul	a0,a5,a3
2006164e:	00c767b3          	or	a5,a4,a2
20061652:	8736                	c.mv	a4,a3
20061654:	00a7fc63          	bgeu	a5,a0,2006166c <__udivdi3+0x20e>
20061658:	97c2                	c.add	a5,a6
2006165a:	fff68713          	addi	a4,a3,-1
2006165e:	0107e763          	bltu	a5,a6,2006166c <__udivdi3+0x20e>
20061662:	00a7f563          	bgeu	a5,a0,2006166c <__udivdi3+0x20e>
20061666:	ffe68713          	addi	a4,a3,-2
2006166a:	97c2                	c.add	a5,a6
2006166c:	05c2                	c.slli	a1,0x10
2006166e:	8f89                	c.sub	a5,a0
20061670:	8dd9                	c.or	a1,a4
20061672:	bddd                	c.j	20061568 <__udivdi3+0x10a>
20061674:	14d5e263          	bltu	a1,a3,200617b8 <__udivdi3+0x35a>
20061678:	6741                	c.lui	a4,0x10
2006167a:	02e6ff63          	bgeu	a3,a4,200616b8 <__udivdi3+0x25a>
2006167e:	0ff00713          	addi	a4,zero,255
20061682:	00d735b3          	sltu	a1,a4,a3
20061686:	058e                	c.slli	a1,0x3
20061688:	20065737          	lui	a4,0x20065
2006168c:	00b6d533          	srl	a0,a3,a1
20061690:	97870713          	addi	a4,a4,-1672 # 20064978 <__clz_tab>
20061694:	972a                	c.add	a4,a0
20061696:	00074703          	lbu	a4,0(a4)
2006169a:	02000513          	addi	a0,zero,32
2006169e:	972e                	c.add	a4,a1
200616a0:	40e505b3          	sub	a1,a0,a4
200616a4:	02e51163          	bne	a0,a4,200616c6 <__udivdi3+0x268>
200616a8:	4505                	c.li	a0,1
200616aa:	f2f6e7e3          	bltu	a3,a5,200615d8 <__udivdi3+0x17a>
200616ae:	00c8b533          	sltu	a0,a7,a2
200616b2:	00154513          	xori	a0,a0,1
200616b6:	b70d                	c.j	200615d8 <__udivdi3+0x17a>
200616b8:	01000737          	lui	a4,0x1000
200616bc:	45c1                	c.li	a1,16
200616be:	fce6e5e3          	bltu	a3,a4,20061688 <__udivdi3+0x22a>
200616c2:	45e1                	c.li	a1,24
200616c4:	b7d1                	c.j	20061688 <__udivdi3+0x22a>
200616c6:	00e65833          	srl	a6,a2,a4
200616ca:	00b696b3          	sll	a3,a3,a1
200616ce:	00d86833          	or	a6,a6,a3
200616d2:	00e7de33          	srl	t3,a5,a4
200616d6:	01085e93          	srli	t4,a6,0x10
200616da:	03de76b3          	remu	a3,t3,t4
200616de:	00b797b3          	sll	a5,a5,a1
200616e2:	00e8d733          	srl	a4,a7,a4
200616e6:	00b61333          	sll	t1,a2,a1
200616ea:	00f76633          	or	a2,a4,a5
200616ee:	01081793          	slli	a5,a6,0x10
200616f2:	83c1                	c.srli	a5,0x10
200616f4:	01065713          	srli	a4,a2,0x10
200616f8:	03de5e33          	divu	t3,t3,t4
200616fc:	06c2                	c.slli	a3,0x10
200616fe:	8f55                	c.or	a4,a3
20061700:	03c78f33          	mul	t5,a5,t3
20061704:	8572                	c.mv	a0,t3
20061706:	01e77c63          	bgeu	a4,t5,2006171e <__udivdi3+0x2c0>
2006170a:	9742                	c.add	a4,a6
2006170c:	fffe0513          	addi	a0,t3,-1
20061710:	01076763          	bltu	a4,a6,2006171e <__udivdi3+0x2c0>
20061714:	01e77563          	bgeu	a4,t5,2006171e <__udivdi3+0x2c0>
20061718:	ffee0513          	addi	a0,t3,-2
2006171c:	9742                	c.add	a4,a6
2006171e:	41e70733          	sub	a4,a4,t5
20061722:	03d776b3          	remu	a3,a4,t4
20061726:	03d75733          	divu	a4,a4,t4
2006172a:	06c2                	c.slli	a3,0x10
2006172c:	02e78e33          	mul	t3,a5,a4
20061730:	01061793          	slli	a5,a2,0x10
20061734:	83c1                	c.srli	a5,0x10
20061736:	8fd5                	c.or	a5,a3
20061738:	863a                	c.mv	a2,a4
2006173a:	01c7fc63          	bgeu	a5,t3,20061752 <__udivdi3+0x2f4>
2006173e:	97c2                	c.add	a5,a6
20061740:	fff70613          	addi	a2,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20061744:	0107e763          	bltu	a5,a6,20061752 <__udivdi3+0x2f4>
20061748:	01c7f563          	bgeu	a5,t3,20061752 <__udivdi3+0x2f4>
2006174c:	ffe70613          	addi	a2,a4,-2
20061750:	97c2                	c.add	a5,a6
20061752:	0542                	c.slli	a0,0x10
20061754:	6ec1                	c.lui	t4,0x10
20061756:	8d51                	c.or	a0,a2
20061758:	fffe8693          	addi	a3,t4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
2006175c:	00d57833          	and	a6,a0,a3
20061760:	01055613          	srli	a2,a0,0x10
20061764:	00d376b3          	and	a3,t1,a3
20061768:	01035313          	srli	t1,t1,0x10
2006176c:	41c787b3          	sub	a5,a5,t3
20061770:	02d80e33          	mul	t3,a6,a3
20061774:	02d606b3          	mul	a3,a2,a3
20061778:	010e5713          	srli	a4,t3,0x10
2006177c:	02680833          	mul	a6,a6,t1
20061780:	9836                	c.add	a6,a3
20061782:	9742                	c.add	a4,a6
20061784:	02660633          	mul	a2,a2,t1
20061788:	00d77363          	bgeu	a4,a3,2006178e <__udivdi3+0x330>
2006178c:	9676                	c.add	a2,t4
2006178e:	01075693          	srli	a3,a4,0x10
20061792:	9636                	c.add	a2,a3
20061794:	02c7e063          	bltu	a5,a2,200617b4 <__udivdi3+0x356>
20061798:	d8c794e3          	bne	a5,a2,20061520 <__udivdi3+0xc2>
2006179c:	67c1                	c.lui	a5,0x10
2006179e:	17fd                	c.addi	a5,-1
200617a0:	8f7d                	c.and	a4,a5
200617a2:	0742                	c.slli	a4,0x10
200617a4:	00fe7e33          	and	t3,t3,a5
200617a8:	00b898b3          	sll	a7,a7,a1
200617ac:	9772                	c.add	a4,t3
200617ae:	4581                	c.li	a1,0
200617b0:	e2e8f4e3          	bgeu	a7,a4,200615d8 <__udivdi3+0x17a>
200617b4:	157d                	c.addi	a0,-1
200617b6:	b3ad                	c.j	20061520 <__udivdi3+0xc2>
200617b8:	4581                	c.li	a1,0
200617ba:	4501                	c.li	a0,0
200617bc:	bd31                	c.j	200615d8 <__udivdi3+0x17a>

200617be <__umoddi3>:
200617be:	8eaa                	c.mv	t4,a0
200617c0:	832e                	c.mv	t1,a1
200617c2:	8832                	c.mv	a6,a2
200617c4:	87b6                	c.mv	a5,a3
200617c6:	1c069263          	bne	a3,zero,2006198a <__umoddi3+0x1cc>
200617ca:	20065737          	lui	a4,0x20065
200617ce:	97870713          	addi	a4,a4,-1672 # 20064978 <__clz_tab>
200617d2:	0ac5f563          	bgeu	a1,a2,2006187c <__umoddi3+0xbe>
200617d6:	66c1                	c.lui	a3,0x10
200617d8:	08d67b63          	bgeu	a2,a3,2006186e <__umoddi3+0xb0>
200617dc:	0ff00693          	addi	a3,zero,255
200617e0:	00c6f363          	bgeu	a3,a2,200617e6 <__umoddi3+0x28>
200617e4:	47a1                	c.li	a5,8
200617e6:	00f656b3          	srl	a3,a2,a5
200617ea:	9736                	c.add	a4,a3
200617ec:	00074703          	lbu	a4,0(a4)
200617f0:	97ba                	c.add	a5,a4
200617f2:	02000713          	addi	a4,zero,32
200617f6:	40f708b3          	sub	a7,a4,a5
200617fa:	00f70b63          	beq	a4,a5,20061810 <__umoddi3+0x52>
200617fe:	011315b3          	sll	a1,t1,a7
20061802:	00fed7b3          	srl	a5,t4,a5
20061806:	01161833          	sll	a6,a2,a7
2006180a:	8ddd                	c.or	a1,a5
2006180c:	011e9533          	sll	a0,t4,a7
20061810:	01085693          	srli	a3,a6,0x10
20061814:	02d5f7b3          	remu	a5,a1,a3
20061818:	01081613          	slli	a2,a6,0x10
2006181c:	8241                	c.srli	a2,0x10
2006181e:	01055713          	srli	a4,a0,0x10
20061822:	02d5d5b3          	divu	a1,a1,a3
20061826:	07c2                	c.slli	a5,0x10
20061828:	8f5d                	c.or	a4,a5
2006182a:	02b605b3          	mul	a1,a2,a1
2006182e:	00b77863          	bgeu	a4,a1,2006183e <__umoddi3+0x80>
20061832:	9742                	c.add	a4,a6
20061834:	01076563          	bltu	a4,a6,2006183e <__umoddi3+0x80>
20061838:	00b77363          	bgeu	a4,a1,2006183e <__umoddi3+0x80>
2006183c:	9742                	c.add	a4,a6
2006183e:	8f0d                	c.sub	a4,a1
20061840:	02d777b3          	remu	a5,a4,a3
20061844:	0542                	c.slli	a0,0x10
20061846:	8141                	c.srli	a0,0x10
20061848:	02d75733          	divu	a4,a4,a3
2006184c:	07c2                	c.slli	a5,0x10
2006184e:	8d5d                	c.or	a0,a5
20061850:	02e60733          	mul	a4,a2,a4
20061854:	00e57863          	bgeu	a0,a4,20061864 <__umoddi3+0xa6>
20061858:	9542                	c.add	a0,a6
2006185a:	01056563          	bltu	a0,a6,20061864 <__umoddi3+0xa6>
2006185e:	00e57363          	bgeu	a0,a4,20061864 <__umoddi3+0xa6>
20061862:	9542                	c.add	a0,a6
20061864:	8d19                	c.sub	a0,a4
20061866:	01155533          	srl	a0,a0,a7
2006186a:	4581                	c.li	a1,0
2006186c:	8082                	c.jr	ra
2006186e:	010006b7          	lui	a3,0x1000
20061872:	47c1                	c.li	a5,16
20061874:	f6d669e3          	bltu	a2,a3,200617e6 <__umoddi3+0x28>
20061878:	47e1                	c.li	a5,24
2006187a:	b7b5                	c.j	200617e6 <__umoddi3+0x28>
2006187c:	e601                	c.bnez	a2,20061884 <__umoddi3+0xc6>
2006187e:	4685                	c.li	a3,1
20061880:	02c6d833          	divu	a6,a3,a2
20061884:	66c1                	c.lui	a3,0x10
20061886:	0ed87b63          	bgeu	a6,a3,2006197c <__umoddi3+0x1be>
2006188a:	0ff00693          	addi	a3,zero,255
2006188e:	0106f363          	bgeu	a3,a6,20061894 <__umoddi3+0xd6>
20061892:	47a1                	c.li	a5,8
20061894:	00f856b3          	srl	a3,a6,a5
20061898:	9736                	c.add	a4,a3
2006189a:	00074603          	lbu	a2,0(a4)
2006189e:	410305b3          	sub	a1,t1,a6
200618a2:	963e                	c.add	a2,a5
200618a4:	02000793          	addi	a5,zero,32
200618a8:	40c788b3          	sub	a7,a5,a2
200618ac:	06c78a63          	beq	a5,a2,20061920 <__umoddi3+0x162>
200618b0:	01181833          	sll	a6,a6,a7
200618b4:	00c357b3          	srl	a5,t1,a2
200618b8:	011315b3          	sll	a1,t1,a7
200618bc:	01085313          	srli	t1,a6,0x10
200618c0:	00ced633          	srl	a2,t4,a2
200618c4:	8e4d                	c.or	a2,a1
200618c6:	01081593          	slli	a1,a6,0x10
200618ca:	81c1                	c.srli	a1,0x10
200618cc:	011e9533          	sll	a0,t4,a7
200618d0:	0267d6b3          	divu	a3,a5,t1
200618d4:	0267f733          	remu	a4,a5,t1
200618d8:	01065793          	srli	a5,a2,0x10
200618dc:	02d586b3          	mul	a3,a1,a3
200618e0:	0742                	c.slli	a4,0x10
200618e2:	8f5d                	c.or	a4,a5
200618e4:	00d77863          	bgeu	a4,a3,200618f4 <__umoddi3+0x136>
200618e8:	9742                	c.add	a4,a6
200618ea:	01076563          	bltu	a4,a6,200618f4 <__umoddi3+0x136>
200618ee:	00d77363          	bgeu	a4,a3,200618f4 <__umoddi3+0x136>
200618f2:	9742                	c.add	a4,a6
200618f4:	40d706b3          	sub	a3,a4,a3
200618f8:	0266f733          	remu	a4,a3,t1
200618fc:	0642                	c.slli	a2,0x10
200618fe:	8241                	c.srli	a2,0x10
20061900:	0266d6b3          	divu	a3,a3,t1
20061904:	0742                	c.slli	a4,0x10
20061906:	02d586b3          	mul	a3,a1,a3
2006190a:	00c765b3          	or	a1,a4,a2
2006190e:	00d5f863          	bgeu	a1,a3,2006191e <__umoddi3+0x160>
20061912:	95c2                	c.add	a1,a6
20061914:	0105e563          	bltu	a1,a6,2006191e <__umoddi3+0x160>
20061918:	00d5f363          	bgeu	a1,a3,2006191e <__umoddi3+0x160>
2006191c:	95c2                	c.add	a1,a6
2006191e:	8d95                	c.sub	a1,a3
20061920:	01085693          	srli	a3,a6,0x10
20061924:	02d5f733          	remu	a4,a1,a3
20061928:	01081793          	slli	a5,a6,0x10
2006192c:	83c1                	c.srli	a5,0x10
2006192e:	01055613          	srli	a2,a0,0x10
20061932:	02d5d5b3          	divu	a1,a1,a3
20061936:	0742                	c.slli	a4,0x10
20061938:	8f51                	c.or	a4,a2
2006193a:	02b785b3          	mul	a1,a5,a1
2006193e:	00b77863          	bgeu	a4,a1,2006194e <__umoddi3+0x190>
20061942:	9742                	c.add	a4,a6
20061944:	01076563          	bltu	a4,a6,2006194e <__umoddi3+0x190>
20061948:	00b77363          	bgeu	a4,a1,2006194e <__umoddi3+0x190>
2006194c:	9742                	c.add	a4,a6
2006194e:	40b705b3          	sub	a1,a4,a1
20061952:	02d5f733          	remu	a4,a1,a3
20061956:	0542                	c.slli	a0,0x10
20061958:	8141                	c.srli	a0,0x10
2006195a:	02d5d5b3          	divu	a1,a1,a3
2006195e:	02b785b3          	mul	a1,a5,a1
20061962:	01071793          	slli	a5,a4,0x10
20061966:	8d5d                	c.or	a0,a5
20061968:	00b57863          	bgeu	a0,a1,20061978 <__umoddi3+0x1ba>
2006196c:	9542                	c.add	a0,a6
2006196e:	01056563          	bltu	a0,a6,20061978 <__umoddi3+0x1ba>
20061972:	00b57363          	bgeu	a0,a1,20061978 <__umoddi3+0x1ba>
20061976:	9542                	c.add	a0,a6
20061978:	8d0d                	c.sub	a0,a1
2006197a:	b5f5                	c.j	20061866 <__umoddi3+0xa8>
2006197c:	010006b7          	lui	a3,0x1000
20061980:	47c1                	c.li	a5,16
20061982:	f0d869e3          	bltu	a6,a3,20061894 <__umoddi3+0xd6>
20061986:	47e1                	c.li	a5,24
20061988:	b731                	c.j	20061894 <__umoddi3+0xd6>
2006198a:	eed5e1e3          	bltu	a1,a3,2006186c <__umoddi3+0xae>
2006198e:	67c1                	c.lui	a5,0x10
20061990:	04f6f463          	bgeu	a3,a5,200619d8 <__umoddi3+0x21a>
20061994:	0ff00893          	addi	a7,zero,255
20061998:	00d8b7b3          	sltu	a5,a7,a3
2006199c:	078e                	c.slli	a5,0x3
2006199e:	20065737          	lui	a4,0x20065
200619a2:	00f6d833          	srl	a6,a3,a5
200619a6:	97870713          	addi	a4,a4,-1672 # 20064978 <__clz_tab>
200619aa:	9742                	c.add	a4,a6
200619ac:	00074883          	lbu	a7,0(a4)
200619b0:	98be                	c.add	a7,a5
200619b2:	02000793          	addi	a5,zero,32
200619b6:	41178833          	sub	a6,a5,a7
200619ba:	03179663          	bne	a5,a7,200619e6 <__umoddi3+0x228>
200619be:	0066e463          	bltu	a3,t1,200619c6 <__umoddi3+0x208>
200619c2:	eacee5e3          	bltu	t4,a2,2006186c <__umoddi3+0xae>
200619c6:	40ce8533          	sub	a0,t4,a2
200619ca:	40d305b3          	sub	a1,t1,a3
200619ce:	00aebeb3          	sltu	t4,t4,a0
200619d2:	41d585b3          	sub	a1,a1,t4
200619d6:	bd59                	c.j	2006186c <__umoddi3+0xae>
200619d8:	01000737          	lui	a4,0x1000
200619dc:	47c1                	c.li	a5,16
200619de:	fce6e0e3          	bltu	a3,a4,2006199e <__umoddi3+0x1e0>
200619e2:	47e1                	c.li	a5,24
200619e4:	bf6d                	c.j	2006199e <__umoddi3+0x1e0>
200619e6:	011657b3          	srl	a5,a2,a7
200619ea:	010696b3          	sll	a3,a3,a6
200619ee:	00d7ee33          	or	t3,a5,a3
200619f2:	01135f33          	srl	t5,t1,a7
200619f6:	011ed7b3          	srl	a5,t4,a7
200619fa:	010e9533          	sll	a0,t4,a6
200619fe:	010e5e93          	srli	t4,t3,0x10
20061a02:	010315b3          	sll	a1,t1,a6
20061a06:	03df5333          	divu	t1,t5,t4
20061a0a:	8fcd                	c.or	a5,a1
20061a0c:	010e1593          	slli	a1,t3,0x10
20061a10:	81c1                	c.srli	a1,0x10
20061a12:	0107d713          	srli	a4,a5,0x10
20061a16:	01061633          	sll	a2,a2,a6
20061a1a:	03df76b3          	remu	a3,t5,t4
20061a1e:	02658f33          	mul	t5,a1,t1
20061a22:	06c2                	c.slli	a3,0x10
20061a24:	8f55                	c.or	a4,a3
20061a26:	869a                	c.mv	a3,t1
20061a28:	01e77c63          	bgeu	a4,t5,20061a40 <__umoddi3+0x282>
20061a2c:	9772                	c.add	a4,t3
20061a2e:	fff30693          	addi	a3,t1,-1
20061a32:	01c76763          	bltu	a4,t3,20061a40 <__umoddi3+0x282>
20061a36:	01e77563          	bgeu	a4,t5,20061a40 <__umoddi3+0x282>
20061a3a:	ffe30693          	addi	a3,t1,-2
20061a3e:	9772                	c.add	a4,t3
20061a40:	41e70733          	sub	a4,a4,t5
20061a44:	03d77333          	remu	t1,a4,t4
20061a48:	07c2                	c.slli	a5,0x10
20061a4a:	83c1                	c.srli	a5,0x10
20061a4c:	03d75733          	divu	a4,a4,t4
20061a50:	0342                	c.slli	t1,0x10
20061a52:	00f36333          	or	t1,t1,a5
20061a56:	02e585b3          	mul	a1,a1,a4
20061a5a:	87ba                	c.mv	a5,a4
20061a5c:	00b37c63          	bgeu	t1,a1,20061a74 <__umoddi3+0x2b6>
20061a60:	9372                	c.add	t1,t3
20061a62:	fff70793          	addi	a5,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20061a66:	01c36763          	bltu	t1,t3,20061a74 <__umoddi3+0x2b6>
20061a6a:	00b37563          	bgeu	t1,a1,20061a74 <__umoddi3+0x2b6>
20061a6e:	ffe70793          	addi	a5,a4,-2
20061a72:	9372                	c.add	t1,t3
20061a74:	06c2                	c.slli	a3,0x10
20061a76:	6f41                	c.lui	t5,0x10
20061a78:	8edd                	c.or	a3,a5
20061a7a:	40b305b3          	sub	a1,t1,a1
20061a7e:	ffff0313          	addi	t1,t5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061a82:	0066feb3          	and	t4,a3,t1
20061a86:	01065713          	srli	a4,a2,0x10
20061a8a:	82c1                	c.srli	a3,0x10
20061a8c:	00667333          	and	t1,a2,t1
20061a90:	026e87b3          	mul	a5,t4,t1
20061a94:	02668333          	mul	t1,a3,t1
20061a98:	02ee8eb3          	mul	t4,t4,a4
20061a9c:	02e686b3          	mul	a3,a3,a4
20061aa0:	9e9a                	c.add	t4,t1
20061aa2:	0107d713          	srli	a4,a5,0x10
20061aa6:	9776                	c.add	a4,t4
20061aa8:	00677363          	bgeu	a4,t1,20061aae <__umoddi3+0x2f0>
20061aac:	96fa                	c.add	a3,t5
20061aae:	01075313          	srli	t1,a4,0x10
20061ab2:	969a                	c.add	a3,t1
20061ab4:	6341                	c.lui	t1,0x10
20061ab6:	137d                	c.addi	t1,-1
20061ab8:	00677733          	and	a4,a4,t1
20061abc:	0742                	c.slli	a4,0x10
20061abe:	0067f7b3          	and	a5,a5,t1
20061ac2:	97ba                	c.add	a5,a4
20061ac4:	00d5e663          	bltu	a1,a3,20061ad0 <__umoddi3+0x312>
20061ac8:	00d59b63          	bne	a1,a3,20061ade <__umoddi3+0x320>
20061acc:	00f57963          	bgeu	a0,a5,20061ade <__umoddi3+0x320>
20061ad0:	40c78633          	sub	a2,a5,a2
20061ad4:	00c7b7b3          	sltu	a5,a5,a2
20061ad8:	97f2                	c.add	a5,t3
20061ada:	8e9d                	c.sub	a3,a5
20061adc:	87b2                	c.mv	a5,a2
20061ade:	40f507b3          	sub	a5,a0,a5
20061ae2:	00f53533          	sltu	a0,a0,a5
20061ae6:	8d95                	c.sub	a1,a3
20061ae8:	8d89                	c.sub	a1,a0
20061aea:	011598b3          	sll	a7,a1,a7
20061aee:	0107d533          	srl	a0,a5,a6
20061af2:	00a8e533          	or	a0,a7,a0
20061af6:	0105d5b3          	srl	a1,a1,a6
20061afa:	bb8d                	c.j	2006186c <__umoddi3+0xae>

20061afc <atoi>:
20061afc:	4629                	c.li	a2,10
20061afe:	4581                	c.li	a1,0
20061b00:	a1a5                	c.j	20061f68 <strtol>

20061b02 <_atoi_r>:
20061b02:	46a9                	c.li	a3,10
20061b04:	4601                	c.li	a2,0
20061b06:	a991                	c.j	20061f5a <_strtol_r>

20061b08 <__errno>:
20061b08:	e981a503          	lw	a0,-360(gp) # 2006673c <_impure_ptr>
20061b0c:	8082                	c.jr	ra

20061b0e <__libc_init_array>:
20061b0e:	1141                	c.addi	sp,-16
20061b10:	c422                	c.swsp	s0,8(sp)
20061b12:	c226                	c.swsp	s1,4(sp)
20061b14:	20065437          	lui	s0,0x20065
20061b18:	200654b7          	lui	s1,0x20065
20061b1c:	cc848793          	addi	a5,s1,-824 # 20064cc8 <__frame_dummy_init_array_entry>
20061b20:	cc840413          	addi	s0,s0,-824 # 20064cc8 <__frame_dummy_init_array_entry>
20061b24:	8c1d                	c.sub	s0,a5
20061b26:	c04a                	c.swsp	s2,0(sp)
20061b28:	c606                	c.swsp	ra,12(sp)
20061b2a:	8409                	c.srai	s0,0x2
20061b2c:	cc848493          	addi	s1,s1,-824
20061b30:	4901                	c.li	s2,0
20061b32:	02891763          	bne	s2,s0,20061b60 <__libc_init_array+0x52>
20061b36:	200654b7          	lui	s1,0x20065
20061b3a:	20065437          	lui	s0,0x20065
20061b3e:	cc848793          	addi	a5,s1,-824 # 20064cc8 <__frame_dummy_init_array_entry>
20061b42:	ccc40413          	addi	s0,s0,-820 # 20064ccc <__do_global_dtors_aux_fini_array_entry>
20061b46:	8c1d                	c.sub	s0,a5
20061b48:	8409                	c.srai	s0,0x2
20061b4a:	cc848493          	addi	s1,s1,-824
20061b4e:	4901                	c.li	s2,0
20061b50:	00891d63          	bne	s2,s0,20061b6a <__libc_init_array+0x5c>
20061b54:	40b2                	c.lwsp	ra,12(sp)
20061b56:	4422                	c.lwsp	s0,8(sp)
20061b58:	4492                	c.lwsp	s1,4(sp)
20061b5a:	4902                	c.lwsp	s2,0(sp)
20061b5c:	0141                	c.addi	sp,16
20061b5e:	8082                	c.jr	ra
20061b60:	409c                	c.lw	a5,0(s1)
20061b62:	0905                	c.addi	s2,1
20061b64:	0491                	c.addi	s1,4
20061b66:	9782                	c.jalr	a5
20061b68:	b7e9                	c.j	20061b32 <__libc_init_array+0x24>
20061b6a:	409c                	c.lw	a5,0(s1)
20061b6c:	0905                	c.addi	s2,1
20061b6e:	0491                	c.addi	s1,4
20061b70:	9782                	c.jalr	a5
20061b72:	bff9                	c.j	20061b50 <__libc_init_array+0x42>

20061b74 <cleanup_glue>:
20061b74:	1141                	c.addi	sp,-16
20061b76:	c422                	c.swsp	s0,8(sp)
20061b78:	842e                	c.mv	s0,a1
20061b7a:	418c                	c.lw	a1,0(a1)
20061b7c:	c226                	c.swsp	s1,4(sp)
20061b7e:	c606                	c.swsp	ra,12(sp)
20061b80:	84aa                	c.mv	s1,a0
20061b82:	c191                	c.beqz	a1,20061b86 <cleanup_glue+0x12>
20061b84:	3fc5                	c.jal	20061b74 <cleanup_glue>
20061b86:	85a2                	c.mv	a1,s0
20061b88:	4422                	c.lwsp	s0,8(sp)
20061b8a:	40b2                	c.lwsp	ra,12(sp)
20061b8c:	8526                	c.mv	a0,s1
20061b8e:	4492                	c.lwsp	s1,4(sp)
20061b90:	0141                	c.addi	sp,16
20061b92:	da5f806f          	jal	zero,2005a936 <__wrap__free_r>

20061b96 <_reclaim_reent>:
20061b96:	e981a783          	lw	a5,-360(gp) # 2006673c <_impure_ptr>
20061b9a:	0ea78263          	beq	a5,a0,20061c7e <_reclaim_reent+0xe8>
20061b9e:	515c                	c.lw	a5,36(a0)
20061ba0:	1101                	c.addi	sp,-32
20061ba2:	cc22                	c.swsp	s0,24(sp)
20061ba4:	ce06                	c.swsp	ra,28(sp)
20061ba6:	ca26                	c.swsp	s1,20(sp)
20061ba8:	c84a                	c.swsp	s2,16(sp)
20061baa:	c64e                	c.swsp	s3,12(sp)
20061bac:	842a                	c.mv	s0,a0
20061bae:	cf81                	c.beqz	a5,20061bc6 <_reclaim_reent+0x30>
20061bb0:	47dc                	c.lw	a5,12(a5)
20061bb2:	4481                	c.li	s1,0
20061bb4:	08000913          	addi	s2,zero,128
20061bb8:	efc9                	c.bnez	a5,20061c52 <_reclaim_reent+0xbc>
20061bba:	505c                	c.lw	a5,36(s0)
20061bbc:	438c                	c.lw	a1,0(a5)
20061bbe:	c581                	c.beqz	a1,20061bc6 <_reclaim_reent+0x30>
20061bc0:	8522                	c.mv	a0,s0
20061bc2:	d75f80ef          	jal	ra,2005a936 <__wrap__free_r>
20061bc6:	484c                	c.lw	a1,20(s0)
20061bc8:	c581                	c.beqz	a1,20061bd0 <_reclaim_reent+0x3a>
20061bca:	8522                	c.mv	a0,s0
20061bcc:	d6bf80ef          	jal	ra,2005a936 <__wrap__free_r>
20061bd0:	504c                	c.lw	a1,36(s0)
20061bd2:	c581                	c.beqz	a1,20061bda <_reclaim_reent+0x44>
20061bd4:	8522                	c.mv	a0,s0
20061bd6:	d61f80ef          	jal	ra,2005a936 <__wrap__free_r>
20061bda:	5c0c                	c.lw	a1,56(s0)
20061bdc:	c581                	c.beqz	a1,20061be4 <_reclaim_reent+0x4e>
20061bde:	8522                	c.mv	a0,s0
20061be0:	d57f80ef          	jal	ra,2005a936 <__wrap__free_r>
20061be4:	5c4c                	c.lw	a1,60(s0)
20061be6:	c581                	c.beqz	a1,20061bee <_reclaim_reent+0x58>
20061be8:	8522                	c.mv	a0,s0
20061bea:	d4df80ef          	jal	ra,2005a936 <__wrap__free_r>
20061bee:	402c                	c.lw	a1,64(s0)
20061bf0:	c581                	c.beqz	a1,20061bf8 <_reclaim_reent+0x62>
20061bf2:	8522                	c.mv	a0,s0
20061bf4:	d43f80ef          	jal	ra,2005a936 <__wrap__free_r>
20061bf8:	0ec42583          	lw	a1,236(s0)
20061bfc:	c581                	c.beqz	a1,20061c04 <_reclaim_reent+0x6e>
20061bfe:	8522                	c.mv	a0,s0
20061c00:	d37f80ef          	jal	ra,2005a936 <__wrap__free_r>
20061c04:	0e842583          	lw	a1,232(s0)
20061c08:	c581                	c.beqz	a1,20061c10 <_reclaim_reent+0x7a>
20061c0a:	8522                	c.mv	a0,s0
20061c0c:	d2bf80ef          	jal	ra,2005a936 <__wrap__free_r>
20061c10:	443c                	c.lw	a5,72(s0)
20061c12:	c799                	c.beqz	a5,20061c20 <_reclaim_reent+0x8a>
20061c14:	0887a583          	lw	a1,136(a5) # 10088 <mbedtls_ecp_check_pub_priv+0x1aa>
20061c18:	c581                	c.beqz	a1,20061c20 <_reclaim_reent+0x8a>
20061c1a:	8522                	c.mv	a0,s0
20061c1c:	d1bf80ef          	jal	ra,2005a936 <__wrap__free_r>
20061c20:	584c                	c.lw	a1,52(s0)
20061c22:	c581                	c.beqz	a1,20061c2a <_reclaim_reent+0x94>
20061c24:	8522                	c.mv	a0,s0
20061c26:	d11f80ef          	jal	ra,2005a936 <__wrap__free_r>
20061c2a:	4c1c                	c.lw	a5,24(s0)
20061c2c:	c3b1                	c.beqz	a5,20061c70 <_reclaim_reent+0xda>
20061c2e:	541c                	c.lw	a5,40(s0)
20061c30:	8522                	c.mv	a0,s0
20061c32:	9782                	c.jalr	a5
20061c34:	0d842583          	lw	a1,216(s0)
20061c38:	cd85                	c.beqz	a1,20061c70 <_reclaim_reent+0xda>
20061c3a:	8522                	c.mv	a0,s0
20061c3c:	4462                	c.lwsp	s0,24(sp)
20061c3e:	40f2                	c.lwsp	ra,28(sp)
20061c40:	44d2                	c.lwsp	s1,20(sp)
20061c42:	4942                	c.lwsp	s2,16(sp)
20061c44:	49b2                	c.lwsp	s3,12(sp)
20061c46:	6105                	c.addi16sp	sp,32
20061c48:	b735                	c.j	20061b74 <cleanup_glue>
20061c4a:	95a6                	c.add	a1,s1
20061c4c:	418c                	c.lw	a1,0(a1)
20061c4e:	e991                	c.bnez	a1,20061c62 <_reclaim_reent+0xcc>
20061c50:	0491                	c.addi	s1,4
20061c52:	505c                	c.lw	a5,36(s0)
20061c54:	47cc                	c.lw	a1,12(a5)
20061c56:	ff249ae3          	bne	s1,s2,20061c4a <_reclaim_reent+0xb4>
20061c5a:	8522                	c.mv	a0,s0
20061c5c:	cdbf80ef          	jal	ra,2005a936 <__wrap__free_r>
20061c60:	bfa9                	c.j	20061bba <_reclaim_reent+0x24>
20061c62:	0005a983          	lw	s3,0(a1)
20061c66:	8522                	c.mv	a0,s0
20061c68:	ccff80ef          	jal	ra,2005a936 <__wrap__free_r>
20061c6c:	85ce                	c.mv	a1,s3
20061c6e:	b7c5                	c.j	20061c4e <_reclaim_reent+0xb8>
20061c70:	40f2                	c.lwsp	ra,28(sp)
20061c72:	4462                	c.lwsp	s0,24(sp)
20061c74:	44d2                	c.lwsp	s1,20(sp)
20061c76:	4942                	c.lwsp	s2,16(sp)
20061c78:	49b2                	c.lwsp	s3,12(sp)
20061c7a:	6105                	c.addi16sp	sp,32
20061c7c:	8082                	c.jr	ra
20061c7e:	8082                	c.jr	ra

20061c80 <strcpy>:
20061c80:	00b56733          	or	a4,a0,a1
20061c84:	8b0d                	c.andi	a4,3
20061c86:	87aa                	c.mv	a5,a0
20061c88:	e729                	c.bnez	a4,20061cd2 <strcpy+0x52>
20061c8a:	7f7f86b7          	lui	a3,0x7f7f8
20061c8e:	f7f68693          	addi	a3,a3,-129 # 7f7f7f7f <__kr4_psram_text_start__+0x1f677f5f>
20061c92:	587d                	c.li	a6,-1
20061c94:	4190                	c.lw	a2,0(a1)
20061c96:	00d67733          	and	a4,a2,a3
20061c9a:	9736                	c.add	a4,a3
20061c9c:	8f51                	c.or	a4,a2
20061c9e:	8f55                	c.or	a4,a3
20061ca0:	03070463          	beq	a4,a6,20061cc8 <strcpy+0x48>
20061ca4:	0005c603          	lbu	a2,0(a1)
20061ca8:	0015c683          	lbu	a3,1(a1)
20061cac:	0025c703          	lbu	a4,2(a1)
20061cb0:	00c78023          	sb	a2,0(a5)
20061cb4:	c615                	c.beqz	a2,20061ce0 <strcpy+0x60>
20061cb6:	00d780a3          	sb	a3,1(a5)
20061cba:	c29d                	c.beqz	a3,20061ce0 <strcpy+0x60>
20061cbc:	00e78123          	sb	a4,2(a5)
20061cc0:	c305                	c.beqz	a4,20061ce0 <strcpy+0x60>
20061cc2:	000781a3          	sb	zero,3(a5)
20061cc6:	8082                	c.jr	ra
20061cc8:	0791                	c.addi	a5,4
20061cca:	0591                	c.addi	a1,4
20061ccc:	fec7ae23          	sw	a2,-4(a5)
20061cd0:	b7d1                	c.j	20061c94 <strcpy+0x14>
20061cd2:	0005c703          	lbu	a4,0(a1)
20061cd6:	0785                	c.addi	a5,1
20061cd8:	0585                	c.addi	a1,1
20061cda:	fee78fa3          	sb	a4,-1(a5)
20061cde:	fb75                	c.bnez	a4,20061cd2 <strcpy+0x52>
20061ce0:	8082                	c.jr	ra

20061ce2 <strncmp>:
20061ce2:	c625                	c.beqz	a2,20061d4a <strncmp+0x68>
20061ce4:	00b567b3          	or	a5,a0,a1
20061ce8:	8b8d                	c.andi	a5,3
20061cea:	e385                	c.bnez	a5,20061d0a <strncmp+0x28>
20061cec:	feff06b7          	lui	a3,0xfeff0
20061cf0:	80808837          	lui	a6,0x80808
20061cf4:	488d                	c.li	a7,3
20061cf6:	eff68693          	addi	a3,a3,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061cfa:	08080813          	addi	a6,a6,128 # 80808080 <__ctrace_end__+0xb6808080>
20061cfe:	00c8f663          	bgeu	a7,a2,20061d0a <strncmp+0x28>
20061d02:	411c                	c.lw	a5,0(a0)
20061d04:	4198                	c.lw	a4,0(a1)
20061d06:	02e78163          	beq	a5,a4,20061d28 <strncmp+0x46>
20061d0a:	167d                	c.addi	a2,-1
20061d0c:	4701                	c.li	a4,0
20061d0e:	00e507b3          	add	a5,a0,a4
20061d12:	00e586b3          	add	a3,a1,a4
20061d16:	0007c783          	lbu	a5,0(a5)
20061d1a:	0006c683          	lbu	a3,0(a3)
20061d1e:	02d78263          	beq	a5,a3,20061d42 <strncmp+0x60>
20061d22:	40d78533          	sub	a0,a5,a3
20061d26:	8082                	c.jr	ra
20061d28:	1671                	c.addi	a2,-4
20061d2a:	c205                	c.beqz	a2,20061d4a <strncmp+0x68>
20061d2c:	00d78733          	add	a4,a5,a3
20061d30:	fff7c793          	xori	a5,a5,-1
20061d34:	8ff9                	c.and	a5,a4
20061d36:	0107f7b3          	and	a5,a5,a6
20061d3a:	eb81                	c.bnez	a5,20061d4a <strncmp+0x68>
20061d3c:	0511                	c.addi	a0,4
20061d3e:	0591                	c.addi	a1,4
20061d40:	bf7d                	c.j	20061cfe <strncmp+0x1c>
20061d42:	00c70463          	beq	a4,a2,20061d4a <strncmp+0x68>
20061d46:	0705                	c.addi	a4,1
20061d48:	f3f9                	c.bnez	a5,20061d0e <strncmp+0x2c>
20061d4a:	4501                	c.li	a0,0
20061d4c:	8082                	c.jr	ra

20061d4e <strncpy>:
20061d4e:	00a5e733          	or	a4,a1,a0
20061d52:	8b0d                	c.andi	a4,3
20061d54:	87aa                	c.mv	a5,a0
20061d56:	e701                	c.bnez	a4,20061d5e <strncpy+0x10>
20061d58:	470d                	c.li	a4,3
20061d5a:	04c76163          	bltu	a4,a2,20061d9c <strncpy+0x4e>
20061d5e:	ca09                	c.beqz	a2,20061d70 <strncpy+0x22>
20061d60:	0005c703          	lbu	a4,0(a1)
20061d64:	0585                	c.addi	a1,1
20061d66:	0785                	c.addi	a5,1
20061d68:	fee78fa3          	sb	a4,-1(a5)
20061d6c:	167d                	c.addi	a2,-1
20061d6e:	fb65                	c.bnez	a4,20061d5e <strncpy+0x10>
20061d70:	963e                	c.add	a2,a5
20061d72:	02f61f63          	bne	a2,a5,20061db0 <strncpy+0x62>
20061d76:	8082                	c.jr	ra
20061d78:	1671                	c.addi	a2,-4
20061d7a:	0107a023          	sw	a6,0(a5)
20061d7e:	0791                	c.addi	a5,4
20061d80:	0591                	c.addi	a1,4
20061d82:	fcce7ee3          	bgeu	t3,a2,20061d5e <strncpy+0x10>
20061d86:	0005a803          	lw	a6,0(a1)
20061d8a:	01180733          	add	a4,a6,a7
20061d8e:	fff84313          	xori	t1,a6,-1
20061d92:	00677733          	and	a4,a4,t1
20061d96:	8f75                	c.and	a4,a3
20061d98:	d365                	c.beqz	a4,20061d78 <strncpy+0x2a>
20061d9a:	b7d1                	c.j	20061d5e <strncpy+0x10>
20061d9c:	feff08b7          	lui	a7,0xfeff0
20061da0:	808086b7          	lui	a3,0x80808
20061da4:	eff88893          	addi	a7,a7,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061da8:	08068693          	addi	a3,a3,128 # 80808080 <__ctrace_end__+0xb6808080>
20061dac:	4e0d                	c.li	t3,3
20061dae:	bfe1                	c.j	20061d86 <strncpy+0x38>
20061db0:	0785                	c.addi	a5,1
20061db2:	fe078fa3          	sb	zero,-1(a5)
20061db6:	bf75                	c.j	20061d72 <strncpy+0x24>

20061db8 <strsep>:
20061db8:	862a                	c.mv	a2,a0
20061dba:	4108                	c.lw	a0,0(a0)
20061dbc:	4681                	c.li	a3,0
20061dbe:	a009                	c.j	20061dc0 <__strtok_r>

20061dc0 <__strtok_r>:
20061dc0:	e119                	c.bnez	a0,20061dc6 <__strtok_r+0x6>
20061dc2:	4208                	c.lw	a0,0(a2)
20061dc4:	c939                	c.beqz	a0,20061e1a <__strtok_r+0x5a>
20061dc6:	87aa                	c.mv	a5,a0
20061dc8:	853e                	c.mv	a0,a5
20061dca:	0007c803          	lbu	a6,0(a5)
20061dce:	0785                	c.addi	a5,1
20061dd0:	872e                	c.mv	a4,a1
20061dd2:	00074883          	lbu	a7,0(a4)
20061dd6:	0705                	c.addi	a4,1
20061dd8:	00089863          	bne	a7,zero,20061de8 <__strtok_r+0x28>
20061ddc:	02081063          	bne	a6,zero,20061dfc <__strtok_r+0x3c>
20061de0:	00062023          	sw	zero,0(a2)
20061de4:	4501                	c.li	a0,0
20061de6:	8082                	c.jr	ra
20061de8:	ff1815e3          	bne	a6,a7,20061dd2 <__strtok_r+0x12>
20061dec:	fef1                	c.bnez	a3,20061dc8 <__strtok_r+0x8>
20061dee:	c21c                	c.sw	a5,0(a2)
20061df0:	00050023          	sb	zero,0(a0)
20061df4:	8082                	c.jr	ra
20061df6:	4781                	c.li	a5,0
20061df8:	a839                	c.j	20061e16 <__strtok_r+0x56>
20061dfa:	e689                	c.bnez	a3,20061e04 <__strtok_r+0x44>
20061dfc:	0007c803          	lbu	a6,0(a5)
20061e00:	0785                	c.addi	a5,1
20061e02:	872e                	c.mv	a4,a1
20061e04:	00074683          	lbu	a3,0(a4)
20061e08:	0705                	c.addi	a4,1
20061e0a:	fed818e3          	bne	a6,a3,20061dfa <__strtok_r+0x3a>
20061e0e:	fe0804e3          	beq	a6,zero,20061df6 <__strtok_r+0x36>
20061e12:	fe078fa3          	sb	zero,-1(a5)
20061e16:	c21c                	c.sw	a5,0(a2)
20061e18:	8082                	c.jr	ra
20061e1a:	8082                	c.jr	ra

20061e1c <strtok_r>:
20061e1c:	4685                	c.li	a3,1
20061e1e:	b74d                	c.j	20061dc0 <__strtok_r>

20061e20 <_strtol_l.constprop.0>:
20061e20:	4785                	c.li	a5,1
20061e22:	0af68863          	beq	a3,a5,20061ed2 <_strtol_l.constprop.0+0xb2>
20061e26:	02400793          	addi	a5,zero,36
20061e2a:	872e                	c.mv	a4,a1
20061e2c:	0ad7e363          	bltu	a5,a3,20061ed2 <_strtol_l.constprop.0+0xb2>
20061e30:	20065837          	lui	a6,0x20065
20061e34:	8eaa                	c.mv	t4,a0
20061e36:	a7980813          	addi	a6,a6,-1415 # 20064a79 <_ctype_+0x1>
20061e3a:	88ba                	c.mv	a7,a4
20061e3c:	00074783          	lbu	a5,0(a4)
20061e40:	0705                	c.addi	a4,1
20061e42:	01078533          	add	a0,a5,a6
20061e46:	00054503          	lbu	a0,0(a0)
20061e4a:	8921                	c.andi	a0,8
20061e4c:	f57d                	c.bnez	a0,20061e3a <_strtol_l.constprop.0+0x1a>
20061e4e:	02d00513          	addi	a0,zero,45
20061e52:	08a79a63          	bne	a5,a0,20061ee6 <_strtol_l.constprop.0+0xc6>
20061e56:	00074783          	lbu	a5,0(a4)
20061e5a:	4e05                	c.li	t3,1
20061e5c:	00288713          	addi	a4,a7,2
20061e60:	c6f5                	c.beqz	a3,20061f4c <_strtol_l.constprop.0+0x12c>
20061e62:	4541                	c.li	a0,16
20061e64:	02a69263          	bne	a3,a0,20061e88 <_strtol_l.constprop.0+0x68>
20061e68:	03000513          	addi	a0,zero,48
20061e6c:	00a79e63          	bne	a5,a0,20061e88 <_strtol_l.constprop.0+0x68>
20061e70:	00074783          	lbu	a5,0(a4)
20061e74:	05800513          	addi	a0,zero,88
20061e78:	0df7f793          	andi	a5,a5,223
20061e7c:	0ca79363          	bne	a5,a0,20061f42 <_strtol_l.constprop.0+0x122>
20061e80:	00174783          	lbu	a5,1(a4)
20061e84:	46c1                	c.li	a3,16
20061e86:	0709                	c.addi	a4,2
20061e88:	800008b7          	lui	a7,0x80000
20061e8c:	fff8c893          	xori	a7,a7,-1
20061e90:	98f2                	c.add	a7,t3
20061e92:	02d8f2b3          	remu	t0,a7,a3
20061e96:	4301                	c.li	t1,0
20061e98:	4501                	c.li	a0,0
20061e9a:	43a5                	c.li	t2,9
20061e9c:	4fe5                	c.li	t6,25
20061e9e:	02d8df33          	divu	t5,a7,a3
20061ea2:	fd078813          	addi	a6,a5,-48
20061ea6:	0503ea63          	bltu	t2,a6,20061efa <_strtol_l.constprop.0+0xda>
20061eaa:	87c2                	c.mv	a5,a6
20061eac:	06d7d763          	bge	a5,a3,20061f1a <_strtol_l.constprop.0+0xfa>
20061eb0:	06034363          	blt	t1,zero,20061f16 <_strtol_l.constprop.0+0xf6>
20061eb4:	537d                	c.li	t1,-1
20061eb6:	00af6a63          	bltu	t5,a0,20061eca <_strtol_l.constprop.0+0xaa>
20061eba:	00af1463          	bne	t5,a0,20061ec2 <_strtol_l.constprop.0+0xa2>
20061ebe:	00f2c663          	blt	t0,a5,20061eca <_strtol_l.constprop.0+0xaa>
20061ec2:	4305                	c.li	t1,1
20061ec4:	02a68533          	mul	a0,a3,a0
20061ec8:	953e                	c.add	a0,a5
20061eca:	0705                	c.addi	a4,1
20061ecc:	fff74783          	lbu	a5,-1(a4)
20061ed0:	bfc9                	c.j	20061ea2 <_strtol_l.constprop.0+0x82>
20061ed2:	1141                	c.addi	sp,-16
20061ed4:	c606                	c.swsp	ra,12(sp)
20061ed6:	c33ff0ef          	jal	ra,20061b08 <__errno>
20061eda:	40b2                	c.lwsp	ra,12(sp)
20061edc:	47d9                	c.li	a5,22
20061ede:	c11c                	c.sw	a5,0(a0)
20061ee0:	4501                	c.li	a0,0
20061ee2:	0141                	c.addi	sp,16
20061ee4:	8082                	c.jr	ra
20061ee6:	02b00513          	addi	a0,zero,43
20061eea:	4e01                	c.li	t3,0
20061eec:	f6a79ae3          	bne	a5,a0,20061e60 <_strtol_l.constprop.0+0x40>
20061ef0:	00074783          	lbu	a5,0(a4)
20061ef4:	00288713          	addi	a4,a7,2 # 80000002 <__ctrace_end__+0xb6000002>
20061ef8:	b7a5                	c.j	20061e60 <_strtol_l.constprop.0+0x40>
20061efa:	fbf78813          	addi	a6,a5,-65
20061efe:	010fe563          	bltu	t6,a6,20061f08 <_strtol_l.constprop.0+0xe8>
20061f02:	fc978793          	addi	a5,a5,-55
20061f06:	b75d                	c.j	20061eac <_strtol_l.constprop.0+0x8c>
20061f08:	f9f78813          	addi	a6,a5,-97
20061f0c:	010fe763          	bltu	t6,a6,20061f1a <_strtol_l.constprop.0+0xfa>
20061f10:	fa978793          	addi	a5,a5,-87
20061f14:	bf61                	c.j	20061eac <_strtol_l.constprop.0+0x8c>
20061f16:	537d                	c.li	t1,-1
20061f18:	bf4d                	c.j	20061eca <_strtol_l.constprop.0+0xaa>
20061f1a:	00035963          	bge	t1,zero,20061f2c <_strtol_l.constprop.0+0x10c>
20061f1e:	02200793          	addi	a5,zero,34
20061f22:	00fea023          	sw	a5,0(t4)
20061f26:	8546                	c.mv	a0,a7
20061f28:	ea09                	c.bnez	a2,20061f3a <_strtol_l.constprop.0+0x11a>
20061f2a:	8082                	c.jr	ra
20061f2c:	000e0463          	beq	t3,zero,20061f34 <_strtol_l.constprop.0+0x114>
20061f30:	40a00533          	sub	a0,zero,a0
20061f34:	c215                	c.beqz	a2,20061f58 <_strtol_l.constprop.0+0x138>
20061f36:	00030463          	beq	t1,zero,20061f3e <_strtol_l.constprop.0+0x11e>
20061f3a:	fff70593          	addi	a1,a4,-1
20061f3e:	c20c                	c.sw	a1,0(a2)
20061f40:	8082                	c.jr	ra
20061f42:	03000793          	addi	a5,zero,48
20061f46:	f2a9                	c.bnez	a3,20061e88 <_strtol_l.constprop.0+0x68>
20061f48:	46a1                	c.li	a3,8
20061f4a:	bf3d                	c.j	20061e88 <_strtol_l.constprop.0+0x68>
20061f4c:	03000513          	addi	a0,zero,48
20061f50:	f2a780e3          	beq	a5,a0,20061e70 <_strtol_l.constprop.0+0x50>
20061f54:	46a9                	c.li	a3,10
20061f56:	bf0d                	c.j	20061e88 <_strtol_l.constprop.0+0x68>
20061f58:	8082                	c.jr	ra

20061f5a <_strtol_r>:
20061f5a:	b5d9                	c.j	20061e20 <_strtol_l.constprop.0>

20061f5c <strtol_l>:
20061f5c:	86b2                	c.mv	a3,a2
20061f5e:	862e                	c.mv	a2,a1
20061f60:	85aa                	c.mv	a1,a0
20061f62:	e981a503          	lw	a0,-360(gp) # 2006673c <_impure_ptr>
20061f66:	bd6d                	c.j	20061e20 <_strtol_l.constprop.0>

20061f68 <strtol>:
20061f68:	86b2                	c.mv	a3,a2
20061f6a:	862e                	c.mv	a2,a1
20061f6c:	85aa                	c.mv	a1,a0
20061f6e:	e981a503          	lw	a0,-360(gp) # 2006673c <_impure_ptr>
20061f72:	b57d                	c.j	20061e20 <_strtol_l.constprop.0>
20061f74:	6d6f682f          	0x6d6f682f
20061f78:	2f65                	c.jal	20062730 <pmap_func+0x3a0>
20061f7a:	7561                	c.lui	a0,0xffff8
20061f7c:	6f74                	c.flw	fa3,92(a4)
20061f7e:	6a5f 6e65 696b      	0x696b6e656a5f
20061f84:	736e                	c.flwsp	ft6,248(sp)
20061f86:	775f 616c 356e      	0x356e616c775f
20061f8c:	544f492f          	0x544f492f
20061f90:	415f 5354 722f      	0x722f5354415f
20061f96:	7065                	c.lui	zero,0xffff9
20061f98:	722f736f          	jal	t1,201596ba <__ap_sram_heap_start+0xd96ba>
20061f9c:	6c65                	c.lui	s8,0x19
20061f9e:	6165                	c.addi16sp	sp,112
20061fa0:	6d2d6573          	csrrsi	a0,0x6d2,26
20061fa4:	7361                	c.lui	t1,0xffff8
20061fa6:	6574                	c.flw	fa3,76(a0)
20061fa8:	2f72                	c.fldsp	ft10,280(sp)
20061faa:	72756f73          	csrrsi	t5,0x727,10
20061fae:	612f6563          	bltu	t5,s2,200625b8 <pmap_func+0x228>
20061fb2:	656d                	c.lui	a0,0x1b
20061fb4:	6162                	c.flwsp	ft2,24(sp)
20061fb6:	696c                	c.flw	fa1,84(a0)
20061fb8:	6574                	c.flw	fa3,76(a0)
20061fba:	675f 6363 705f      	0x705f6363675f
20061fc0:	6f72                	c.flwsp	ft10,28(sp)
20061fc2:	656a                	c.flwsp	fa0,152(sp)
20061fc4:	702f7463          	bgeu	t5,sp,200626cc <pmap_func+0x33c>
20061fc8:	6f72                	c.flwsp	ft10,28(sp)
20061fca:	656a                	c.flwsp	fa0,152(sp)
20061fcc:	6b5f7463          	bgeu	t5,s5,20062674 <pmap_func+0x2e4>
20061fd0:	3472                	c.fldsp	fs0,312(sp)
20061fd2:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
20061fd6:	2e2e2f6b          	0x2e2e2f6b
20061fda:	2f2e2e2f          	0x2f2e2e2f
20061fde:	2e2e                	c.fldsp	ft8,200(sp)
20061fe0:	6d6f632f          	0x6d6f632f
20061fe4:	6f70                	c.flw	fa2,92(a4)
20061fe6:	656e                	c.flwsp	fa0,216(sp)
20061fe8:	746e                	c.flwsp	fs0,248(sp)
20061fea:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
20061fee:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
20061ff2:	6162                	c.flwsp	ft2,24(sp)
20061ff4:	696c                	c.flw	fa1,84(a0)
20061ff6:	6574                	c.flw	fa3,76(a0)
20061ff8:	6c77662f          	0x6c77662f
20061ffc:	6269                	c.lui	tp,0x1a
20061ffe:	6d61722f          	0x6d61722f
20062002:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20062008:	2f6e                	c.fldsp	ft10,216(sp)
2006200a:	6d61                	c.lui	s10,0x18
2006200c:	6265                	c.lui	tp,0x19
2006200e:	5f61                	c.li	t5,-8
20062010:	6c66                	c.flwsp	fs8,88(sp)
20062012:	7361                	c.lui	t1,0xffff8
20062014:	5f68                	c.lw	a0,124(a4)
20062016:	6172                	c.flwsp	ft2,28(sp)
20062018:	2e6d                	c.jal	200623d2 <pmap_func+0x42>
2006201a:	68430063          	beq	t1,tp,2006269a <pmap_func+0x30a>
2006201e:	6e61                	c.lui	t3,0x18
20062020:	656e                	c.flwsp	fa0,216(sp)
20062022:	206c                	c.fld	fa1,192(s0)
20062024:	666e6f43          	fmadd.q	ft10,ft8,ft6,fa2,unknown
20062028:	696c                	c.flw	fa1,84(a0)
2006202a:	66207463          	bgeu	zero,sp,20062692 <pmap_func+0x302>
2006202e:	4320726f          	jal	tp,20069460 <__bdram_heap_buffer_start__+0xac0>
20062032:	5550                	c.lw	a2,44(a0)
20062034:	2520                	c.fld	fs0,72(a0)
20062036:	756c                	c.flw	fa1,108(a0)
20062038:	4320                	c.lw	s0,64(a4)
2006203a:	6168                	c.flw	fa0,68(a0)
2006203c:	6e6e                	c.flwsp	ft8,216(sp)
2006203e:	6c65                	c.lui	s8,0x19
20062040:	2520                	c.fld	fs0,72(a0)
20062042:	756c                	c.flw	fa1,108(a0)
20062044:	2120                	c.fld	fs0,64(a0)
20062046:	4920                	c.lw	s0,80(a0)
20062048:	726f6e67          	0x726f6e67
2006204c:	2065                	c.jal	200620f4 <strtol+0x18c>
2006204e:	6649                	c.lui	a2,0x12
20062050:	4320                	c.lw	s0,64(a4)
20062052:	5550                	c.lw	a2,44(a0)
20062054:	4820                	c.lw	s0,80(s0)
20062056:	7361                	c.lui	t1,0xffff8
20062058:	5220                	c.lw	s0,96(a2)
2006205a:	7365                	c.lui	t1,0xffff9
2006205c:	7465                	c.lui	s0,0xffff9
2006205e:	0a0d                	c.addi	s4,3
20062060:	0000                	c.unimp
20062062:	0000                	c.unimp
20062064:	5049                	c.li	zero,-14
20062066:	49200043          	fmadd.s	ft0,ft0,fs2,fs1,rne
2006206a:	4350                	c.lw	a2,4(a4)
2006206c:	5220                	c.lw	s0,96(a2)
2006206e:	7165                	c.addi16sp	sp,-400
20062070:	6575                	c.lui	a0,0x1d
20062072:	54207473          	csrrci	s0,0x542,0
20062076:	6d69                	c.lui	s10,0x1a
20062078:	6f65                	c.lui	t5,0x19
2006207a:	7475                	c.lui	s0,0xffffd
2006207c:	0a0d                	c.addi	s4,3
2006207e:	0000                	c.unimp
20062080:	4920                	c.lw	s0,80(a0)
20062082:	4350                	c.lw	a2,4(a4)
20062084:	4720                	c.lw	s0,72(a4)
20062086:	7465                	c.lui	s0,0xffff9
20062088:	5320                	c.lw	s0,96(a4)
2006208a:	6d65                	c.lui	s10,0x19
2006208c:	7061                	c.lui	zero,0xffff8
2006208e:	6f68                	c.flw	fa0,92(a4)
20062090:	6572                	c.flwsp	fa0,28(sp)
20062092:	5420                	c.lw	s0,104(s0)
20062094:	6d69                	c.lui	s10,0x1a
20062096:	6f65                	c.lui	t5,0x19
20062098:	7475                	c.lui	s0,0xffffd
2006209a:	0a0d                	c.addi	s4,3
2006209c:	0000                	c.unimp
2006209e:	0000                	c.unimp
200620a0:	6d6f682f          	0x6d6f682f
200620a4:	2f65                	c.jal	2006285c <pmap_func+0x4cc>
200620a6:	7561                	c.lui	a0,0xffff8
200620a8:	6f74                	c.flw	fa3,92(a4)
200620aa:	6a5f 6e65 696b      	0x696b6e656a5f
200620b0:	736e                	c.flwsp	ft6,248(sp)
200620b2:	775f 616c 356e      	0x356e616c775f
200620b8:	544f492f          	0x544f492f
200620bc:	415f 5354 722f      	0x722f5354415f
200620c2:	7065                	c.lui	zero,0xffff9
200620c4:	722f736f          	jal	t1,201597e6 <__ap_sram_heap_start+0xd97e6>
200620c8:	6c65                	c.lui	s8,0x19
200620ca:	6165                	c.addi16sp	sp,112
200620cc:	6d2d6573          	csrrsi	a0,0x6d2,26
200620d0:	7361                	c.lui	t1,0xffff8
200620d2:	6574                	c.flw	fa3,76(a0)
200620d4:	2f72                	c.fldsp	ft10,280(sp)
200620d6:	72756f73          	csrrsi	t5,0x727,10
200620da:	612f6563          	bltu	t5,s2,200626e4 <pmap_func+0x354>
200620de:	656d                	c.lui	a0,0x1b
200620e0:	6162                	c.flwsp	ft2,24(sp)
200620e2:	696c                	c.flw	fa1,84(a0)
200620e4:	6574                	c.flw	fa3,76(a0)
200620e6:	675f 6363 705f      	0x705f6363675f
200620ec:	6f72                	c.flwsp	ft10,28(sp)
200620ee:	656a                	c.flwsp	fa0,152(sp)
200620f0:	702f7463          	bgeu	t5,sp,200627f8 <pmap_func+0x468>
200620f4:	6f72                	c.flwsp	ft10,28(sp)
200620f6:	656a                	c.flwsp	fa0,152(sp)
200620f8:	6b5f7463          	bgeu	t5,s5,200627a0 <pmap_func+0x410>
200620fc:	3472                	c.fldsp	fs0,312(sp)
200620fe:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
20062102:	2e2e2f6b          	0x2e2e2f6b
20062106:	2f2e2e2f          	0x2f2e2e2f
2006210a:	2e2e                	c.fldsp	ft8,200(sp)
2006210c:	6d6f632f          	0x6d6f632f
20062110:	6f70                	c.flw	fa2,92(a4)
20062112:	656e                	c.flwsp	fa0,216(sp)
20062114:	746e                	c.flwsp	fs0,248(sp)
20062116:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
2006211a:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
2006211e:	6162                	c.flwsp	ft2,24(sp)
20062120:	696c                	c.flw	fa1,84(a0)
20062122:	6574                	c.flw	fa3,76(a0)
20062124:	6c77662f          	0x6c77662f
20062128:	6269                	c.lui	tp,0x1a
2006212a:	6d61722f          	0x6d61722f
2006212e:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20062134:	2f6e                	c.fldsp	ft10,216(sp)
20062136:	6d61                	c.lui	s10,0x18
20062138:	6265                	c.lui	tp,0x19
2006213a:	5f61                	c.li	t5,-8
2006213c:	7069                	c.lui	zero,0xffffa
2006213e:	70615f63          	bge	sp,t1,2006285c <pmap_func+0x4cc>
20062142:	2e69                	c.jal	200624dc <pmap_func+0x14c>
20062144:	00000063          	beq	zero,zero,20062144 <strtol+0x1dc>
20062148:	5049                	c.li	zero,-14
2006214a:	69445f43          	fmadd.s	ft10,fs0,fs4,fa3,unknown
2006214e:	3a72                	c.fldsp	fs4,312(sp)
20062150:	6425                	c.lui	s0,0x9
20062152:	202c                	c.fld	fa1,64(s0)
20062154:	5049                	c.li	zero,-14
20062156:	58545f43          	fmadd.s	ft10,fs0,ft5,fa1,unknown
2006215a:	445f 5441 2041      	0x20415441445f
20062160:	7830                	c.flw	fa2,112(s0)
20062162:	7825                	c.lui	a6,0xfffe9
20062164:	4e20                	c.lw	s0,88(a2)
20062166:	4320746f          	jal	s0,20069598 <__bdram_heap_buffer_start__+0xbf8>
2006216a:	656c                	c.flw	fa1,76(a0)
2006216c:	6e61                	c.lui	t3,0x18
2006216e:	0d21                	c.addi	s10,8
20062170:	000a                	c.slli	zero,0x2
	...

20062174 <CSWTCH.29>:
20062174:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20062184:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20062194:	0008 0000                                   ....

20062198 <CSWTCH.30>:
20062198:	0000 4102 0000 4102 0000 0000 0000 0000     ...A...A........
	...
200621d8:	0080 4102 0080 4102 0000 0000 0000 0000     ...A...A........
	...
20062218:	0100 4102 0100 4102 682f 6d6f 2f65 7561     ...A...A/home/au
20062228:	6f74 6a5f 6e65 696b 736e 775f 616c 356e     to_jenkins_wlan5
20062238:	492f 544f 415f 5354 722f 7065 736f 722f     /IOT_ATS/repos/r
20062248:	6c65 6165 6573 6d2d 7361 6574 2f72 6f73     elease-master/so
20062258:	7275 6563 612f 656d 6162 696c 6574 675f     urce/amebalite_g
20062268:	6363 705f 6f72 656a 7463 702f 6f72 656a     cc_project/proje
20062278:	7463 6b5f 3472 762f 6473 2f6b 2e2e 2e2f     ct_kr4/vsdk/../.
20062288:	2f2e 2e2e 632f 6d6f 6f70 656e 746e 732f     ./../component/s
20062298:	636f 612f 656d 6162 696c 6574 662f 6c77     oc/amebalite/fwl
200622a8:	6269 722f 6d61 635f 6d6f 6f6d 2f6e 6d61     ib/ram_common/am
200622b8:	6265 5f61 7069 5f63 6172 2e6d 0063 0000     eba_ipc_ram.c...
200622c8:	5043 2055 6425 7420 6b61 2065 6573 616d     CPU %d take sema
200622d8:	2520 2064 6f66 2072 7870 6469 2520 2064      %d for pxid %d 
200622e8:	6166 6c69 203a 7063 6975 2064 6425 7020     fail: cpuid %d p
200622f8:	6978 2064 6425 6820 7361 7420 6b61 6e65     xid %d has taken
20062308:	7420 6568 7320 6d65 0a61 0000 5043 2055      the sema...CPU 
20062318:	6425 6620 6572 2065 6573 616d 2520 2064     %d free sema %d 
20062328:	6f66 2072 7870 6469 2520 2064 6166 6c69     for pxid %d fail
20062338:	203a 7063 6975 2064 6425 202c 7870 6469     : cpuid %d, pxid
20062348:	2520 2064 7369 7420 6b61 6e69 2067 6874      %d is taking th
20062358:	2065 6573 616d 000a 544f 2050 6572 6461     e sema..OTP read
20062368:	6720 7465 6820 2077 6573 616d 6620 6961      get hw sema fai
20062378:	0a6c 0000 544f 5f50 6552 6461 2038 6146     l...OTP_Read8 Fa
20062388:	6c69 2520 2078 000a                         il %x ..

20062390 <pmap_func>:
20062390:	0000 0101 0001 0101 0002 0102 0003 0102     ................
200623a0:	0004 0202 0005 0102 0006 0202 0007 0102     ................
200623b0:	0008 0101 0009 0101 000a 0101 000b 0101     ................
200623c0:	000c 0101 000d 0101 000e 0101 000f 0101     ................
200623d0:	0010 0101 0011 0101 0012 0000 0013 0202     ................
200623e0:	0014 0202 0015 0101 0016 0000 0017 0101     ................
200623f0:	0018 0101 0019 0101 001a 0101 001b 0101     ................
20062400:	001c 0101 001d 0101 001e 0101 001f 0101     ................
20062410:	0020 0101 0021 0101 0022 0101 0023 0101      ...!..."...#...
20062420:	0024 0101 0025 0101 0026 0101 0027 0101     $...%...&...'...
20062430:	0028 0101 0029 0101 002a 0101 002b 0100     (...)...*...+...
20062440:	002c 0100 002d 0100 002e 0100 002f 0100     ,...-......./...
20062450:	0030 0100 0031 0100 0032 0100 0033 0100     0...1...2...3...
20062460:	ffff ffff 766e 6369 695f 7172 2520 756c     ....nvic_irq %lu
20062470:	4620 6e75 4e20 746f 4120 7373 6769 216e      Fun Not Assign!
20062480:	2121 2121 000a 0000 4c50 4349 0000 0000     !!!!....PLIC....
20062490:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
200624a0:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
200624b0:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
200624c0:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
200624d0:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
200624e0:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
200624f0:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20062500:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
20062510:	696c 6574 6d2f 7369 2f63 6d61 6265 5f61     lite/misc/ameba_
20062520:	6d70 2e75 0063 0000 6e49 6176 696c 2064     pmu.c...Invalid 
20062530:	7350 6172 436d 6b6c 000a 0000 5350 4152     PsramClk....PSRA
20062540:	004d 0000 7370 6172 206d 7974 6570 6920     M...psram type i
20062550:	2073 7830 7825 000a 5353 4549 524e 3009     s 0x%x..SSIENR.0
20062560:	2578 3d78 7830 7825 000a 0000 5443 4c52     x%x=0x%x....CTRL
20062570:	3052 3009 2578 3d78 7830 7825 000a 0000     R0.0x%x=0x%x....
20062580:	4142 4455 0952 7830 7825 303d 2578 0a78     BAUDR.0x%x=0x%x.
20062590:	0000 0000 4156 494c 5f44 4d43 0944 7830     ....VALID_CMD.0x
200625a0:	7825 303d 2578 0a78 0000 0000 5443 4c52     %x=0x%x.....CTRL
200625b0:	3252 3009 2578 3d78 7830 7825 000a 0000     R2.0x%x=0x%x....
200625c0:	4544 4956 4543 495f 464e 094f 7830 7825     DEVICE_INFO.0x%x
200625d0:	303d 2578 0a78 0000 5054 3052 3009 2578     =0x%x...TPR0.0x%
200625e0:	3d78 7830 7825 000a 4552 4441 465f 5341     x=0x%x..READ_FAS
200625f0:	5f54 4953 474e 454c 3009 2578 3d78 7830     T_SINGLE.0x%x=0x
20062600:	7825 000a 5257 5449 5f45 4953 4e47 454c     %x..WRITE_SIGNLE
20062610:	3009 2578 3d78 7830 7825 000a 5541 4f54     .0x%x=0x%x..AUTO
20062620:	4c5f 4e45 5447 0948 7830 7825 303d 2578     _LENGTH.0x%x=0x%
20062630:	0a78 0000 5355 5245 4c5f 4e45 5447 0948     x...USER_LENGTH.
20062640:	7830 7825 303d 2578 0a78 0000 5541 4f54     0x%x=0x%x...AUTO
20062650:	4c5f 4e45 5447 3248 3009 2578 3d78 7830     _LENGTH2.0x%x=0x
20062660:	7825 000a 5253 3009 2578 3d78 7830 7825     %x..SR.0x%x=0x%x
20062670:	000a 0000 5350 4850 5f59 4143 5f4c 4150     ....PSPHY_CAL_PA
20062680:	0952 7830 7825 303d 2578 0a78 0000 0000     R.0x%x=0x%x.....
20062690:	5350 4850 5f59 4850 5f59 5443 4c52 3009     PSPHY_PHY_CTRL.0
200626a0:	2578 3d78 7830 7825 000a 0000 5350 4850     x%x=0x%x....PSPH
200626b0:	5f59 4244 5f47 4553 094c 7830 7825 303d     Y_DBG_SEL.0x%x=0
200626c0:	2578 0a78 0000 0000 5249 5b30 5d31 303d     x%x.....IR0[1]=0
200626d0:	2578 0a78 0000 0000 5249 5b30 5d30 303d     x%x.....IR0[0]=0
200626e0:	2578 0a78 0000 0000 5243 5b30 5d31 303d     x%x.....CR0[1]=0
200626f0:	2578 0a78 0000 0000 5243 5b30 5d30 303d     x%x.....CR0[0]=0
20062700:	2578 0a78 0000 0000 5243 5b31 5d31 303d     x%x.....CR1[1]=0
20062710:	2578 0a78 0000 0000 5243 5b31 5d30 303d     x%x.....CR1[0]=0
20062720:	2578 0a78 0000 0000 3d3d 703d 6168 6573     x%x.....===phase
20062730:	2520 786c 3d20 3d3d 3d3d 000a 6b6f 2520      %lx =====..ok %
20062740:	786c 2520 786c 2520 786c 2520 786c 2520     lx %lx %lx %lx %
20062750:	786c 2520 786c 2520 786c 000a 6166 6c69     lx %lx %lx..fail
20062760:	2520 786c 2520 786c 2520 786c 2520 786c      %lx %lx %lx %lx
20062770:	2520 786c 2520 786c 2520 786c 000a 0000      %lx %lx %lx....
20062780:	6143 4e6c 696d 206e 203d 7825 4320 6c61     CalNmin = %x Cal
20062790:	6d4e 7861 3d20 2520 2078 6957 646e 776f     Nmax = %x Window
200627a0:	6953 657a 3d20 2520 2078 6870 7361 3a65     Size = %x phase:
200627b0:	2520 2078 000a 0000 542b 5345 3a54 0000      %x ....+TEST:..
200627c0:	0a0d 7325 4b4f 0a0d 0000 0000 4f2b 4154     ..%sOK......+OTA
200627d0:	4c43 4145 3a52 0000 4f2b 4154 4552 4f43     CLEAR:..+OTARECO
200627e0:	4556 3a52 0000 0000 522b 5453 003a 0000     VER:....+RST:...
200627f0:	6154 6b73 4c20 7369 3a74 0d20 250a 0d73     Task List: ..%s.
20062800:	000a 0000 6548 7061 7453 7461 3a73 0d20     ....HeapStats: .
20062810:	000a 0000 6f54 6174 206c 6548 7061 093a     ....Total Heap:.
20062820:	7525 0a0d 0000 0000 6548 7061 4620 6572     %u......Heap Fre
20062830:	2065 6f4e 3a77 2509 0d75 000a 6548 7061     e Now:.%u...Heap
20062840:	5520 6573 2064 6f4e 3a77 2509 0d75 000a      Used Now:.%u...
20062850:	6548 7061 5520 6573 2064 614d 3a78 2509     Heap Used Max:.%
20062860:	0d75 000a 532b 4154 4554 003a 7325 0000     u...+STATE:.%s..
20062870:	3056 2e32 2e31 0031 3376 352e 0000 0000     V02.1.1.v3.5....
20062880:	3032 3432 302f 2f37 3330 312d 3a39 3632     2024/07/03-19:26
20062890:	323a 0031 472b 524d 003a 0000 0a0d 7325     :21.+GMR:.....%s
200628a0:	7325 252c 2c73 7325 0a0d 0000 4c5b 474f     %s,%s,%s....[LOG
200628b0:	205d 5245 4f52 2052 7261 3a67 0d20 000a     ] ERROR arg: ...
200628c0:	0023 0000 4c5b 474f 205d 6e49 6176 696c     #...[LOG] Invali
200628d0:	2064 6567 2074 6170 6172 656d 6574 7372     d get parameters
200628e0:	0d2e 000a 253c 3e73 6c20 7665 6c65 6920     ....<%s> level i
200628f0:	2073 6425 0a0d 0000 4c5b 474f 205d 6e49     s %d....[LOG] In
20062900:	6176 696c 2064 6573 2074 6170 6172 656d     valid set parame
20062910:	6574 7372 0d2e 000a 4c5b 474f 205d 6146     ters....[LOG] Fa
20062920:	6c69 6465 7720 6568 206e 6573 2e74 0a0d     iled when set...
20062930:	0000 0000 4c5b 474f 205d 6e49 6176 696c     ....[LOG] Invali
20062940:	2064 6170 6172 656d 6574 2072 756e 626d     d parameter numb
20062950:	7265 0d2e 000a 0000 4c5b 474f 205d 6e49     er......[LOG] In
20062960:	6176 696c 2064 6f6d 6564 2520 2e64 0a0d     valid mode %d...
20062970:	0000 0000 4c5b 474f 205d 6554 7473 6120     ....[LOG] Test a
20062980:	776c 7961 2073 656c 6576 0d6c 000a 0000     lways level.....
20062990:	4c5b 474f 205d 6554 7473 6520 7272 726f     [LOG] Test error
200629a0:	6c20 7665 6c65 0a0d 0000 0000 4c5b 474f      level......[LOG
200629b0:	205d 6554 7473 7720 7261 696e 676e 6c20     ] Test warning l
200629c0:	7665 6c65 0a0d 0000 4c5b 474f 205d 6554     evel....[LOG] Te
200629d0:	7473 6920 666e 206f 656c 6576 0d6c 000a     st info level...
200629e0:	4c5b 474f 205d 6554 7473 4c20 474f 495f     [LOG] Test LOG_I
200629f0:	4554 534d 0a0d 0000 4c2b 474f 003a 0000     TEMS....+LOG:...
20062a00:	0a0d 7325 5245 4f52 3a52 6425 0a0d 0000     ..%sERROR:%d....
20062a10:	5441 4c2b 474f 3c3d 6567 5f74 6573 3e74     AT+LOG=<get_set>
20062a20:	3c2c 6f6d 7564 656c 5b3e 3c2c 6f6c 5f67     ,<module>[,<log_
20062a30:	656c 6576 3e6c 0d5d 000a 0000 3c09 6567     level>]......<ge
20062a40:	5f74 6573 3e74 093a 2d30 6567 2c74 3120     t_set>:.0-get, 1
20062a50:	732d 7465 202c 2d32 7270 6e69 2074 6c61     -set, 2-print al
20062a60:	2c6c 3320 632d 656c 7261 6120 6c6c 0a0d     l, 3-clear all..
20062a70:	0000 0000 3c09 6f6d 7564 656c 3a3e 2a09     .....<module>:.*
20062a80:	652d 6361 2068 6f6d 7564 656c 202c 746f     -each module, ot
20062a90:	6568 7372 732d 6570 6963 6966 2063 6f6d     hers-specific mo
20062aa0:	7564 656c 0a0d 0000 3c09 6f6c 5f67 656c     dule.....<log_le
20062ab0:	6576 3e6c 093a 305b 352c 2c5d 6f20 6c6e     vel>:.[0,5], onl
20062ac0:	2079 7061 6c70 6369 6261 656c 6620 726f     y applicable for
20062ad0:	7320 7465 6d20 646f 0d65 000a 525b 4552      set mode...[RRE
20062ae0:	5d47 4520 7272 726f 7020 7261 6d61 7465     G] Error paramet
20062af0:	7265 0d73 000a 0000 522b 4552 3a47 0000     ers.....+RREG:..
20062b00:	575b 4552 5d47 4520 7272 726f 7020 7261     [WREG] Error par
20062b10:	6d61 7465 7265 0d73 000a 0000 572b 4552     ameters.....+WRE
20062b20:	3a47 0000 5043 2555 0000 0000 7270 6f69     G:..CPU%....prio
20062b30:	0000 0000 7473 7461 0065 0000 6174 6b73     ....state...task
20062b40:	6e20 6d61 0065 0000 7325 2509 0973 7325      name...%s.%s.%s
20062b50:	2509 0a73 000d 0000 7325 2509 0963 6425     .%s.....%s.%c.%d
20062b60:	2509 2e64 7325 0a0d 0000 0000 7325 6d20     .%d.%s......%s m
20062b70:	6c61 6f6c 2063 6166 6c69 6465 0a0d 0000     alloc failed....
20062b80:	745b 706f 555d 6173 6567 203a 6f74 3d70     [top]Usage: top=
20062b90:	6f6d 6564 742c 6d69 2c65 6f63 6e75 0a74     mode,time,count.
20062ba0:	200d 6f6d 6564 203a 2c30 7320 6174 7472     . mode: 0, start
20062bb0:	6320 756f 746e 6320 7570 7520 6173 6567      count cpu usage
20062bc0:	6520 6576 7972 5b20 6974 656d 205d 6573      every [time] se
20062bd0:	6f63 646e 0d2e 200a 6f6d 6564 203a 2c31     cond... mode: 1,
20062be0:	7320 6f74 2070 6f6d 6564 3020 0d2e 200a      stop mode 0... 
20062bf0:	6f6d 6564 203a 3a32 7320 6174 7472 6320     mode: 2: start c
20062c00:	756f 746e 6320 7570 7520 6173 6567 0d2e     ount cpu usage..
20062c10:	200a 6f6d 6564 203a 3a33 7320 6f74 2070     . mode: 3: stop 
20062c20:	6f6d 6564 3220 0d2e 200a 6974 656d 203a     mode 2... time: 
20062c30:	5043 2055 7473 7461 7369 6974 7363 6920     CPU statistics i
20062c40:	746e 7265 6176 2e6c 6544 6166 6c75 2074     nterval.Default 
20062c50:	2e31 7520 696e 2874 2973 0d20 200a 6f63     1. unit(s) .. co
20062c60:	6e75 3a74 4320 5550 7320 6174 6974 7473     unt: CPU statist
20062c70:	6369 2073 6f63 6e75 2c74 6420 6665 7561     ics count, defau
20062c80:	746c 7520 746e 6c69 7320 6f74 2070 726f     lt until stop or
20062c90:	3120 322c 332c 2e2e 002e 0000 654d 6f6d      1,2,3......Memo
20062ca0:	7972 6620 6961 756c 6572 6620 726f 7420     ry failure for t
20062cb0:	7361 5f6b 7473 7461 7375 0a0d 0000 0000     ask_status......
20062cc0:	7563 7272 6e65 2074 6f6d 6564 203a 6425     current mode: %d
20062cd0:	7420 6d69 3a65 2520 2864 2973 6320 756f      time: %d(s) cou
20062ce0:	746e 203a 6425 2820 312d 6620 726f 6920     nt: %d (-1 for i
20062cf0:	666e 6e69 7469 2965 0a0d 0000 7063 5f75     nfinite)....cpu_
20062d00:	7473 7461 745f 7268 6165 0064 432b 5550     stat_thread.+CPU
20062d10:	4f4c 4441 003a 0000 5441 432b 5550 4f4c     LOAD:...AT+CPULO
20062d20:	4441 3c3d 6f6d 6564 5b3e 3c2c 6974 656d     AD=<mode>[,<time
20062d30:	695f 746e 6176 3e6c 3c2c 6f63 6e75 6574     _intval>,<counte
20062d40:	3e72 0d5d 000a 0000 3c09 6f6d 6564 3a3e     r>]......<mode>:
20062d50:	5b09 2c30 5d32 0a0d 0000 0000 3c09 6974     .[0,2].......<ti
20062d60:	656d 695f 746e 6176 3e6c 093a 6e49 7320     me_intval>:.In s
20062d70:	6365 0a0d 0000 0000 3c09 6f63 6e75 6574     ec.......<counte
20062d80:	3e72 3009 6d20 6165 736e 6920 666e 6e69     r>.0 means infin
20062d90:	7469 0a0d 0000 0000 5441 7325 0a0d 0000     it......AT%s....
20062da0:	6f43 6d6d 6e6f 4120 2054 6f43 6d6d 6e61     Common AT Comman
20062db0:	3a64 0a0d 0000 0000 4c2b 5349 3a54 0000     d:......+LIST:..
20062dc0:	4f2b 4154 4c43 4145 0052 0000 4f2b 4154     +OTACLEAR...+OTA
20062dd0:	4552 4f43 4556 0052 432b 5550 4f4c 4441     RECOVER.+CPULOAD
20062de0:	0000 0000 542b 5345 0054 0000 4c2b 5349     ....+TEST...+LIS
20062df0:	0054 0000 522b 5453 0000 0000 532b 4154     T...+RST....+STA
20062e00:	4554 0000 472b 524d 0000 0000 4c2b 474f     TE..+GMR....+LOG
20062e10:	0000 0000 522b 4552 0047 0000 572b 4552     ....+RREG...+WRE
20062e20:	0047 0000                                   G...

20062e24 <__FUNCTION__.0>:
20062e24:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062e34:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20062e44:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20062e54:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20062e64:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20062e74:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20062e84:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20062e94:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20062ea4:	6f70 656e 746e 6f2f 2f73 7266 6565 7472     ponent/os/freert
20062eb4:	736f 662f 6572 7265 6f74 5f73 6568 7061     os/freertos_heap
20062ec4:	5f35 6f63 666e 6769 632e 0000 255b 5d73     5_config.c..[%s]
20062ed4:	6c20 7665 6c65 3d20 2520 0a64 0000 0000      level = %d.....
20062ee4:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20062ef4:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20062f04:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20062f14:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20062f24:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20062f34:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20062f44:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20062f54:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
20062f64:	696c 6574 732f 6c77 6269 6c2f 676f 632e     lite/swlib/log.c
20062f74:	0000 0000 002a 0000 6143 6863 2065 7261     ....*...Cache ar
20062f84:	6172 2079 7369 6620 6c75 2c6c 6120 646e     ray is full, and
20062f94:	7220 7065 616c 6563 6f20 646c 6520 746e      replace old ent
20062fa4:	7972 000a 255b 3830 5d78 0020 0a0d 255b     ry..[%08x] ...[%
20062fb4:	3830 5d78 0020 0000 3025 7838 0020 0000     08x] ...%08x ...
20062fc4:	3025 7832 0020 0000 255b 2d73 6325 205d     %02x ...[%s-%c] 
20062fd4:	0000 0000 7769 7270 7669 0020 003d 0000     ....iwpriv .=...
20062fe4:	002c 0000 0a0d 6e75 6e6b 776f 206e 6f63     ,.....unknown co
20062ff4:	6d6d 6e61 2064 2527 2773 0000 0d0a 4d5b     mmand '%s'....[M
20063004:	4d45 205d 6641 6574 2072 6f64 6320 646d     EM] After do cmd
20063014:	202c 7661 6961 616c 6c62 2065 6568 7061     , available heap
20063024:	2520 0a64 000d 0000 0a0d 230a 0a0d 0000      %d........#....
20063034:	0000 0000 5749 4744 7220 6665 6572 6873     ....IWDG refresh
20063044:	6f20 216e 000a 0000 414d 4e49 0000 0000      on!....MAIN....
20063054:	4457 5f47 6954 656d 0072 0000 5749 4744     WDG_Timer...IWDG
20063064:	7220 6665 6572 6873 6520 7272 726f 000a      refresh error..
20063074:	524b 2034 534f 5320 4154 5452 0a20 0000     KR4 OS START ...
20063084:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
20063094:	202d 4554 5453 4320 4d4f 414d 444e 4d20     - TEST COMMAND M
200630a4:	444f 2045 4548 504c 2520 2d64 2d2d 2d2d     ODE HELP %d-----
200630b4:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 0a2d 0000     -------------...
200630c4:	7325 000a 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     %s..------------
200630d4:	2d2d 2d2d 202d 4554 5453 4320 4d4f 414d     ----- TEST COMMA
200630e4:	444e 4d20 444f 2045 4e45 2044 2520 2d78     ND MODE END  %x-
200630f4:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
20063104:	0a2d 0000 6552 6f62 746f 6e69 2067 2e2e     -...Rebooting ..
20063114:	0a2e 000d 6175 7472 7562 6e72 0000 0000     ....uartburn....
20063124:	255b 3830 5d78 2520 3830 2078 000a 0000     [%08x] %08x ....
20063134:	7257 6e6f 2067 7261 7567 656d 746e 6e20     Wrong argument n
20063144:	6d75 6562 2172 0a0d 0000 0000 5352 5049     umber!......RSIP
20063154:	6520 616e 6c62 6465 202c 6143 276e 2074      enabled, Can't 
20063164:	6552 6461 4620 616c 6873 0a2e 0000 0000     Read Flash......
20063174:	7257 6e6f 2067 6d63 2164 0a0d 0000 0000     Wrong cmd!......
20063184:	003f 0000 4809 4c45 2050 4028 293f 3a20     ?....HELP (@?) :
20063194:	0a20 0909 5020 6972 746e 6820 6c65 2070      ... Print help 
200631a4:	656d 7373 6761 000a 5744 0000 4009 5744     messag..DW...@DW
200631b4:	3c20 6441 7264 7365 2c73 4c20 6e65 7467      <Address, Lengt
200631c4:	3e68 090a 2009 7544 706d 6d20 6d65 726f     h>... Dump memor
200631d4:	2079 6f77 6472 6f20 2072 6552 6461 4820     y word or Read H
200631e4:	2077 6f77 6472 7220 6765 7369 6574 3b72     w word register;
200631f4:	0a20 0909 4420 2057 613c 6464 3e72 0920      ... DW <addr> .
20063204:	2009 7544 706d 6f20 6c6e 2079 6e6f 2065     . Dump only one 
20063214:	6f77 6472 6120 2074 6874 2065 6173 656d     word at the same
20063224:	7420 6d69 2c65 7520 696e 3a74 3420 7942      time, unit: 4By
20063234:	6574 0a73 0909 4420 2057 613c 6464 3e72     tes... DW <addr>
20063244:	3c20 656c 3e6e 0920 4420 6d75 2070 6874      <len> . Dump th
20063254:	2065 7073 6365 6669 6569 2064 656c 676e     e specified leng
20063264:	6874 6f20 2066 6874 2065 6f77 6472 202c     th of the word, 
20063274:	6e75 7469 203a 4234 7479 7365 090a 2009     unit: 4Bytes... 
20063284:	5744 3c20 6461 7264 203e 6c3c 6e65 203e     DW <addr> <len> 
20063294:	2062 4420 6d75 2070 6874 2065 7073 6365     b  Dump the spec
200632a4:	6669 6569 2064 656c 676e 6874 6f20 2066     ified length of 
200632b4:	6874 2065 7962 6574 202c 6e75 7469 203a     the byte, unit: 
200632c4:	4231 7479 0a65 0000 5745 0000 4009 5745     1Byte...EW...@EW
200632d4:	3c20 6441 7264 7365 2c73 4820 7865 0a3e      <Address, Hex>.
200632e4:	0909 5720 6972 6574 6d20 6d65 726f 2079     .. Write memory 
200632f4:	7764 726f 2064 726f 5720 6972 6574 4820     dword or Write H
20063304:	2077 7764 726f 2064 6572 6967 7473 7265     w dword register
20063314:	0a20 0909 4320 6e61 7720 6972 6574 6f20      ... Can write o
20063324:	6c6e 2079 6e6f 2065 7764 726f 2064 7461     nly one dword at
20063334:	7420 6568 7320 6d61 2065 6974 656d 0a20      the same time .
20063344:	0909 4520 3a78 4520 2057 6441 7264 7365     .. Ex: EW Addres
20063354:	2073 6156 756c 2065 000a 0000 4552 4f42     s Value ....REBO
20063364:	544f 0000 4009 4552 4f42 544f 0a20 0909     OT...@REBOOT ...
20063374:	7220 6265 6f6f 2074 090a 2009 6572 6f62      reboot ... rebo
20063384:	746f 7520 7261 6274 7275 206e 000a 0000     ot uartburn ....
20063394:	255b 5d73 2520 2073 3c3c 203c 6854 2065     [%s] %s <<< The 
200633a4:	756d 6574 2078 6168 2073 6f6e 2074 6562     mutex has not be
200633b4:	6e65 7220 6c65 6165 6573 2c64 6220 7475     en released, but
200633c4:	7420 6568 6d20 7475 7865 6820 7361 6220      the mutex has b
200633d4:	6565 206e 6564 656c 6574 2e64 3e20 3e3e     een deleted. >>>
200633e4:	000a 0000                                   ....

200633e8 <__FUNCTION__.0>:
200633e8:	7472 736f 6d5f 7475 7865 645f 6c65 7465     rtos_mutex_delet
200633f8:	0065 0000 6f4e 7354 006b 0000 524b 0034     e...NoTsk...KR4.
20063408:	614d 6c6c 636f 6620 6961 656c 2e64 4320     Malloc failed. C
20063418:	726f 3a65 255b 5d73 202c 6154 6b73 5b3a     ore:[%s], Task:[
20063428:	7325 2c5d 5b20 7266 6565 6820 6165 2070     %s], [free heap 
20063438:	6973 657a 203a 6425 0d5d 000a 0d0a 255b     size: %d].....[%
20063448:	5d73 5320 4154 4b43 4f20 4556 4652 4f4c     s] STACK OVERFLO
20063458:	2057 202d 6154 6b73 614e 656d 2528 2973     W - TaskName(%s)
20063468:	0d0a 0000                                   ....

2006346c <__FUNCTION__.5>:
2006346c:	4176 7070 696c 6163 6974 6e6f 7453 6361     vApplicationStac
2006347c:	4f6b 6576 6672 6f6c 4877 6f6f 006b 0000     kOverflowHook...
2006348c:	5441 3f53 0000 0000 4f4c 5547 5241 5f54     ATS?....LOGUART_
2006349c:	4154 4b53 0000 0000 7243 6165 6574 4c20     TASK....Create L
200634ac:	676f 5520 5241 2054 6154 6b73 4520 7272     og UART Task Err
200634bc:	2121 000a 4853 4c45 004c 0000 255b 5d73     !!..SHELL...[%s]
200634cc:	4920 474d 4449 203a 6425 202c 7563 7272      IMGID: %d, curr
200634dc:	6e65 2074 544f 2541 2064 6441 7264 7365     ent OTA%d Addres
200634ec:	3a73 3020 2578 3830 2c78 7420 7261 6567     s: 0x%08x, targe
200634fc:	2074 544f 2541 2064 6441 7264 7365 3a73     t OTA%d Address:
2006350c:	3020 2578 3830 0a78 0000 0000 5953 0053      0x%08x.....SYS.
2006351c:	255b 5d73 4920 474d 4449 203a 6425 202c     [%s] IMGID: %d, 
2006352c:	7563 7272 6e65 2074 6966 6d72 6177 6572     current firmware
2006353c:	6920 2073 544f 2541 2c64 7420 7261 6567      is OTA%d, targe
2006354c:	2074 6966 6d72 6177 6572 4f20 4154 6425     t firmware OTA%d
2006355c:	6920 2073 6e69 6176 696c 0a64 0000 0000      is invalid.....
2006356c:	255b 5d73 6220 6361 756b 2070 616d 6c6c     [%s] backup mall
2006357c:	636f 6620 6961 646c 6465 000a               oc failded..

20063588 <__func__.0>:
20063588:	7973 5f73 6572 6f63 6576 5f72 746f 5f61     sys_recover_ota_
20063598:	6973 6e67 7461 7275 0065 0000               signature...

200635a4 <__func__.1>:
200635a4:	7973 5f73 6c63 6165 5f72 746f 5f61 6973     sys_clear_ota_si
200635b4:	6e67 7461 7275 0065 4449 454c 0000 0000     gnature.IDLE....
200635c4:	2509 0963 6425 2509 0964 6425 0a0d 0000     .%c.%d.%d.%d....
200635d4:	6d54 2072 7653 0063 e828 2005 e828 2005     Tmr Svc.(.. (.. 
200635e4:	e828 2005 e864 2005 e870 2005 e88c 2005     (.. d.. p.. ... 
200635f4:	e828 2005 e828 2005 e864 2005 e870 2005     (.. (.. d.. p.. 
20063604:	7325 2528 2964 202c 7641 6961 616c 6c62     %s(%d), Availabl
20063614:	2065 6568 7061 2520 0a64 0000 4c57 4e41     e heap %d...WLAN
20063624:	0000 0000 6e69 7469 0000 0000 6c77 6e61     ....init....wlan
20063634:	695f 696e 6974 6c61 7a69 2065 6166 6c69     _initialize fail
20063644:	6465 000a                                   ed..

20063648 <__FUNCTION__.0>:
20063648:	695f 696e 5f74 6874 6572 6461 0000 0000     _init_thread....
20063658:	6f46 2072 6564 7562 2067 6170 6b63 6761     For debug packag
20063668:	2e65 5020 656c 7361 2065 6573 656c 7463     e. Please select
20063678:	7020 7273 6d61 7420 7079 2065 6361 6f63      psram type acco
20063688:	6472 6e69 2067 6f74 7920 756f 2072 6863     rding to your ch
20063698:	7069 000a 4843 5049 4e49 4f46 0000 0000     ip..CHIPINFO....
200636a8:	3230 6620 726f 5720 2042 3233 624d 300a     02 for WB 32Mb.0
200636b8:	2034 6f66 2072 4257 3120 3832 624d 300a     4 for WB 128Mb.0
200636c8:	2033 6f66 2072 4257 3220 3635 624d 0a0a     3 for WB 256Mb..
200636d8:	0000 0000 682f 6d6f 2f65 7561 6f74 6a5f     ..../home/auto_j
200636e8:	6e65 696b 736e 775f 616c 356e 492f 544f     enkins_wlan5/IOT
200636f8:	415f 5354 722f 7065 736f 722f 6c65 6165     _ATS/repos/relea
20063708:	6573 6d2d 7361 6574 2f72 6f73 7275 6563     se-master/source
20063718:	612f 656d 6162 696c 6574 675f 6363 705f     /amebalite_gcc_p
20063728:	6f72 656a 7463 702f 6f72 656a 7463 6b5f     roject/project_k
20063738:	3472 762f 6473 2f6b 2e2e 2e2f 2f2e 2e2e     r4/vsdk/../../..
20063748:	632f 6d6f 6f70 656e 746e 732f 636f 612f     /component/soc/a
20063758:	656d 6162 696c 6574 6c2f 6269 722f 6d61     mebalite/lib/ram
20063768:	635f 6d6f 6f6d 2f6e 6d61 6265 5f61 6863     _common/ameba_ch
20063778:	7069 6e69 6f66 6c5f 6269 632e 0000 0000     ipinfo_lib.c....
20063788:	7325 203a 0000 0000 5245 3a52 4720 7465     %s: ....ERR: Get
20063798:	5320 636f 4e20 6d61 2065 6146 6c69 000a      Soc Name Fail..
200637a8:	3025 7832 0000 0000 4d4b 0034 7325 4c20     %02x....KM4.%s L
200637b8:	6269 5620 7265 6973 6e6f 0a3a 0000 0000     ib Version:.....
200637c8:	3032 3432 302f 2f37 3330 312d 3a39 3632     2024/07/03-19:26
200637d8:	303a 0037 4f43 504d 4c49 2045 4954 454d     :07.COMPILE TIME
200637e8:	203a 7325 000a 0000 6367 2063 6576 7372     : %s....gcc vers
200637f8:	6f69 206e 3031 332e 312e 3220 3230 3032     ion 10.3.1 20220
20063808:	3335 2030 5228 6165 746c 6b65 5620 4453     530 (Realtek VSD
20063818:	2d4b 3031 332e 312e 4220 6975 646c 3420     K-10.3.1 Build 4
20063828:	3730 2b33 2029 0000 4f43 504d 4c49 5245     073+) ..COMPILER
20063838:	203a 7325 000a 0000 6325 0000 2d2d 2d2d     : %s....%c..----
20063848:	2d2d 2d2d 2d2d 2d2d 6154 6b73 4920 666e     --------Task Inf
20063858:	2d6f 2d2d 2d2d 2d2d 2d2d 2d2d 0d2d 000a     o------------...
20063868:	6146 6c75 2074 6e6f 7420 7361 206b 253c     Fault on task <%
20063878:	3e73 000a 6154 6b73 4920 3a44 2520 0d64     s>..Task ID: %d.
20063888:	000a 0000 6154 6b73 5420 4243 303a 2578     ....Task TCB:0x%
20063898:	3830 786c 0a0d 0000 7543 7272 6e65 2074     08lx....Current 
200638a8:	7453 7461 3a65 2520 2064 2528 2973 0a0d     State: %d (%s)..
200638b8:	0000 0000 6142 6573 5020 6972 726f 7469     ....Base Priorit
200638c8:	3a79 2520 0d64 000a 7543 7272 6e65 2074     y: %d...Current 
200638d8:	7250 6f69 6972 7974 203a 6425 0a0d 0000     Priority: %d....
200638e8:	7552 206e 6954 656d 4320 756f 746e 7265     Run Time Counter
200638f8:	203a 6c25 0d75 000a 7453 6361 546b 706f     : %lu...StackTop
20063908:	203a 7830 3025 6c38 2c78 5320 6174 6b63     : 0x%08lx, Stack
20063918:	6142 6573 203a 7830 3025 6c38 2c78 5320     Base: 0x%08lx, S
20063928:	6174 6b63 6e45 3a64 3020 2578 3830 786c     tackEnd: 0x%08lx
20063938:	202c 7453 6361 536b 7a69 3d65 6c25 2875     , StackSize=%lu(
20063948:	6f77 6472 0d29 000a 7453 6361 206b 6948     word)...Stack Hi
20063958:	6867 5720 7461 7265 614d 6b72 203a 6425     gh WaterMark: %d
20063968:	7728 726f 2964 0a0d 0000 0000 7552 6e6e     (word)......Runn
20063978:	6e69 0067 6552 6461 2079 202f 6550 646e     ing.Ready / Pend
20063988:	6e69 0067 6c42 636f 656b 0064 7553 7073     ing.Blocked.Susp
20063998:	6e65 6564 0064 0000 6544 656c 6574 0064     ended...Deleted.
200639a8:	6e49 6176 696c 0064                         Invalid.

200639b0 <TaskStateString.0>:
200639b0:	3974 2006 397c 2006 398c 2006 3994 2006     t9. |9. .9. .9. 
200639c0:	39a0 2006 39a8 2006 f19a 2005 f0ae 2005     .9. .9. ... ... 
200639d0:	f0ae 2005 f0ae 2005 f0ae 2005 f0ae 2005     ... ... ... ... 
200639e0:	f0ae 2005 f0ae 2005 f0ae 2005 f0ae 2005     ... ... ... ... 
200639f0:	f0e4 2005 f256 2005 f124 2005 f0ae 2005     ... V.. $.. ... 
20063a00:	f0ae 2005 f0ae 2005 f0ae 2005 f124 2005     ... ... ... $.. 
20063a10:	f0ae 2005 f0ae 2005 f2aa 2005 f0ae 2005     ... ... ... ... 
20063a20:	f0ae 2005 f10a 2005 f1c0 2005 f0ae 2005     ... ... ... ... 
20063a30:	f0ae 2005 f1de 2005 f0ae 2005 f166 2005     ... ... ... f.. 
20063a40:	f0ae 2005 f0ae 2005 f19c 2005 f0ae 2005     ... ... ... ... 
20063a50:	f2ac 2005 3130 3332 3534 3736 3938 4241     ... 0123456789AB
20063a60:	4443 4645 0000 0000 6f69 6420 6972 6576     CDEF....io drive
20063a70:	2072 6170 6172 656d 6574 7372 6520 7272     r parameters err
20063a80:	726f 2021 6966 656c 6e5f 6d61 3a65 2520     or! file_name: %
20063a90:	2c73 6c20 6e69 3a65 2520 0064 682f 6d6f     s, line: %d./hom
20063aa0:	2f65 7561 6f74 6a5f 6e65 696b 736e 775f     e/auto_jenkins_w
20063ab0:	616c 356e 492f 544f 415f 5354 722f 7065     lan5/IOT_ATS/rep
20063ac0:	736f 722f 6c65 6165 6573 6d2d 7361 6574     os/release-maste
20063ad0:	2f72 6f73 7275 6563 612f 656d 6162 696c     r/source/amebali
20063ae0:	6574 675f 6363 705f 6f72 656a 7463 702f     te_gcc_project/p
20063af0:	6f72 656a 7463 6b5f 3472 762f 6473 2f6b     roject_kr4/vsdk/
20063b00:	2e2e 2e2f 2f2e 2e2e 632f 6d6f 6f70 656e     ../../../compone
20063b10:	746e 732f 636f 612f 656d 6162 696c 6574     nt/soc/amebalite
20063b20:	6c2f 6269 722f 6d61 635f 6d6f 6f6d 2f6e     /lib/ram_common/
20063b30:	6d61 6265 5f61 6f72 5f6d 6170 6374 2e68     ameba_rom_patch.
20063b40:	0063 0000 7269 5b71 6425 205d 7270 6f69     c...irq[%d] prio
20063b50:	6972 7974 2520 2078 6873 6c61 206c 3d3c     rity %x shall <=
20063b60:	2520 0a64 0000 0000 5b1b 3b31 3333 5b6d      %d......[1;33m[
20063b70:	5245 4f52 5d52 5b20 7325 3a3a 6425 205d     ERROR] [%s::%d] 
20063b80:	7473 6361 5f6b 6973 657a 2528 2964 6920     stack_size(%d) i
20063b90:	2073 6f70 6973 6974 6576 3f20 0a3f 5b1b     s positive ??..[
20063ba0:	6d30 0000 5b1b 3b31 3333 5b6d 5245 4f52     0m...[1;33m[ERRO
20063bb0:	5d52 5b20 7325 3a3a 6425 205d 6172 6f5f     R] [%s::%d] ra_o
20063bc0:	6666 6573 2874 6425 2029 7369 6e20 6765     ffset(%d) is neg
20063bd0:	7461 7669 3f65 1b3f 305b 006d 5b1b 3b31     ative??.[0m..[1;
20063be0:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063bf0:	6425 205d 7453 7261 2074 7473 6361 206b     %d] Start stack 
20063c00:	6162 6b63 7274 6361 6e69 2067 6f66 2072     backtracing for 
20063c10:	7073 2520 2c70 7020 2063 7025 1b0a 305b     sp %p, pc %p..[0
20063c20:	006d 0000 5b1b 3b31 3333 5b6d 5254 4341     m....[1;33m[TRAC
20063c30:	5d45 5b20 7325 3a3a 6425 205d 665b 6172     E] [%s::%d] [fra
20063c40:	656d 2320 6425 205d 7073 3e2d 2520 2c70     me #%d] sp-> %p,
20063c50:	7020 2d63 203e 7025 202c 7473 6361 5f6b      pc-> %p, stack_
20063c60:	6973 657a 3e2d 2520 2c64 7220 2d61 203e     size-> %d, ra-> 
20063c70:	7025 1b0a 305b 006d 5b1b 3b31 3333 5b6d     %p..[0m..[1;33m[
20063c80:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063c90:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20063ca0:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20063cb0:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20063cc0:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063cd0:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20063ce0:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20063cf0:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20063d00:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20063d10:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20063d20:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063d30:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
20063d40:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
20063d50:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
20063d60:	1b0a 305b 006d 0000                         ..[0m...

20063d68 <__FUNCTION__.3>:
20063d68:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063d78:	6932 0000                                   2i..

20063d7c <__FUNCTION__.2>:
20063d7c:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063d8c:	6332 0000                                   2c..

20063d90 <__FUNCTION__.1>:
20063d90:	6f64 735f 6174 6b63 625f 6361 746b 6172     do_stack_backtra
20063da0:	6563 0000 6e55 6e6b 776f 206e 7865 6563     ce..Unknown exce
20063db0:	7470 6f69 006e 0000 5b1b 3b31 3333 5b6d     ption....[1;33m[
20063dc0:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063dd0:	3d3d 3d3d 3d3d 3d3d 3d3d 4320 6172 6873     ========== Crash
20063de0:	4420 6d75 2070 3d3d 3d3d 3d3d 3d3d 3d3d      Dump ==========
20063df0:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063e00:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063e10:	7845 6563 7470 6f69 206e 6163 6775 7468     Exception caught
20063e20:	6f20 206e 7025 7720 7469 2068 6572 7361      on %p with reas
20063e30:	6e6f 5b20 7830 7825 205d 3e2d 5b20 7325     on [0x%x] -> [%s
20063e40:	0a5d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     ]..[0m...[1;33m[
20063e50:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063e60:	3d3d 3d3d 3d3d 3d3d 3d3d 5220 6765 7369     ========== Regis
20063e70:	6574 2072 7544 706d 3d20 3d3d 3d3d 3d3d     ter Dump =======
20063e80:	3d3d 0a3d 5b1b 6d30 0000 0000 5b1b 3b31     ===..[0m.....[1;
20063e90:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063ea0:	6425 205d 6d5b 6373 6172 6374 5d68 3020     %d] [mscratch] 0
20063eb0:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063ec0:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063ed0:	6425 205d 6d5b 7065 5d63 2020 2020 3020     %d] [mepc]     0
20063ee0:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063ef0:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063f00:	6425 205d 6d5b 6163 7375 5d65 2020 3020     %d] [mcause]   0
20063f10:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063f20:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063f30:	6425 205d 6d5b 7674 6c61 205d 2020 3020     %d] [mtval]    0
20063f40:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063f50:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063f60:	6425 205d 7845 6563 7470 6f69 206e 6163     %d] Exception ca
20063f70:	7375 2065 6f63 6c75 2064 6562 0d3a 1b0a     use could be:...
20063f80:	305b 006d 5b1b 3b31 3333 5b6d 5254 4341     [0m..[1;33m[TRAC
20063f90:	5d45 5b20 7325 3a3a 6425 205d 2509 2c73     E] [%s::%d] .%s,
20063fa0:	2520 2073 203d 7830 7825 0a0d 5b1b 6d30      %s = 0x%x...[0m
20063fb0:	0000 0000 5b1b 3b31 3333 5b6d 5254 4341     .....[1;33m[TRAC
20063fc0:	5d45 5b20 7325 3a3a 6425 205d 2509 0d73     E] [%s::%d] .%s.
20063fd0:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063fe0:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063ff0:	785b 6425 2d20 203e 7325 205d 7830 3025     [x%d -> %s] 0x%0
20064000:	7838 1b0a 305b 006d 5b1b 3b31 3333 5b6d     8x..[0m..[1;33m[
20064010:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20064020:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20064030:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20064040:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20064050:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20064060:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20064070:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20064080:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20064090:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
200640a0:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
200640b0:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
200640c0:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
200640d0:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
200640e0:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
200640f0:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20064100:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20064110:	3d3d 3d3d 3d3d 3d3d 3d3d 4520 646e 6f20     ========== End o
20064120:	2066 7243 7361 2068 7544 706d 3d20 3d3d     f Crash Dump ===
20064130:	3d3d 3d3d 3d3d 0a3d 5b1b 6d30 0000 0000     =======..[0m....
20064140:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20064150:	7325 3a3a 6425 205d 0a0d 0a0d 5b1b 6d30     %s::%d] .....[0m
20064160:	0000 0000 6f53 6572 412f 4f4d 7020 6761     ....Sore/AMO pag
20064170:	2065 6177 6b6c 6320 6568 6b63 6620 6961     e walk check fai
20064180:	006c 0000 7453 726f 2065 6461 7264 7365     l...Store addres
20064190:	0073 0000 6f4c 6461 7020 6761 2065 6177     s...Load page wa
200641a0:	6b6c 6320 6568 6b63 6620 6961 006c 0000     lk check fail...
200641b0:	6f4c 6461 6120 6464 6572 7373 0000 0000     Load address....
200641c0:	6e49 7473 7572 7463 6f69 206e 6170 6567     Instruction page
200641d0:	7720 6c61 206b 6863 6365 206b 6166 6c69      walk check fail
200641e0:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
200641f0:	6461 7264 7365 0073 7253 726f 2f65 4d41     address.Srore/AM
20064200:	204f 4d50 2050 6863 6365 206b 6166 6c69     O PMP check fail
20064210:	0000 0000 7453 726f 2f65 4d41 204f 6170     ....Store/AMO pa
20064220:	6567 7720 6c61 206b 7562 2073 7265 6f72     ge walk bus erro
20064230:	0072 0000 7453 726f 2065 7562 2073 7265     r...Store bus er
20064240:	6f72 0072 7453 726f 2f65 4d41 204f 6461     ror.Store/AMO ad
20064250:	7264 7365 2073 696d 6173 696c 6e67 6465     dress misaligned
20064260:	0000 0000 6f4c 6461 5020 504d 6320 6568     ....Load PMP che
20064270:	6b63 6620 6961 006c 6f4c 6461 5020 6761     ck fail.Load Pag
20064280:	2065 6177 6b6c 6220 7375 6520 7272 726f     e walk bus error
20064290:	0000 0000 6f4c 6461 6220 7375 6520 7272     ....Load bus err
200642a0:	726f 0000 6f4c 6461 6120 6464 6572 7373     or..Load address
200642b0:	6d20 7369 6c61 6769 656e 0064 6c49 656c      misaligned.Ille
200642c0:	6167 206c 6e69 7473 7572 7463 6f69 006e     gal instruction.
200642d0:	6e49 7473 7572 7463 6f69 206e 706f 6f63     Instruction opco
200642e0:	6564 0000 6e49 7473 7572 7463 6f69 206e     de..Instruction 
200642f0:	4d50 2050 6863 6365 206b 6166 6c69 0000     PMP check fail..
20064300:	6e49 7473 7572 7463 6f69 206e 6150 6567     Instruction Page
20064310:	7720 6c61 206b 7562 2073 7265 6f72 0072      walk bus error.
20064320:	6e49 7473 7572 7463 6f69 206e 7562 2073     Instruction bus 
20064330:	7265 6f72 0072 0000 7242 6e61 6863 7420     error...Branch t
20064340:	7261 6567 2074 696d 6173 696c 6e67 6465     arget misaligned
20064350:	0000 0000 6154 6772 7465 6120 6464 6572     ....Target addre
20064360:	7373 0000 6e49 7473 7572 7463 6f69 206e     ss..Instruction 
20064370:	6461 7264 7365 2073 696d 6173 696c 6e67     address misalign
20064380:	6465 0000 657a 6f72 0000 0000 6172 0000     ed..zero....ra..
20064390:	7073 0000 7067 0000 7074 0000 3074 0000     sp..gp..tp..t0..
200643a0:	3174 0000 3274 0000 3073 662f 0070 0000     t1..t2..s0/fp...
200643b0:	3173 0000 3061 0000 3161 0000 3261 0000     s1..a0..a1..a2..
200643c0:	3361 0000 3461 0000 3561 0000 3661 0000     a3..a4..a5..a6..
200643d0:	3761 0000 3273 0000 3373 0000 3473 0000     a7..s2..s3..s4..
200643e0:	3573 0000 3673 0000 3773 0000 3873 0000     s5..s6..s7..s8..
200643f0:	3973 0000 3173 0030 3173 0031 3374 0000     s9..s10.s11.t3..
20064400:	3474 0000 3574 0000 3674 0000 6e49 7473     t4..t5..t6..Inst
20064410:	7572 7463 6f69 206e 6361 6563 7373 6620     ruction access f
20064420:	7561 746c 0000 0000 7242 6165 706b 696f     ault....Breakpoi
20064430:	746e 0000 6f4c 6461 6120 6464 7365 2073     nt..Load addess 
20064440:	696d 6173 696c 6e67 6465 0000 6f4c 6461     misaligned..Load
20064450:	6120 6363 7365 2073 6166 6c75 0074 0000      access fault...
20064460:	7453 726f 2065 6461 7264 7365 2073 696d     Store address mi
20064470:	6173 696c 6e67 6465 0000 0000 7453 726f     saligned....Stor
20064480:	2065 6461 7264 7365 2073 6166 6c75 0074     e address fault.
20064490:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
200644a0:	6620 6f72 206d 2055 6f6d 6564 0000 0000      from U mode....
200644b0:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
200644c0:	6620 6f72 206d 2053 6f6d 6564 0000 0000      from S mode....
200644d0:	6552 6573 7672 6465 6520 6378 7065 6974     Reserved excepti
200644e0:	6e6f 0000 6e45 6976 6f72 6d6e 6e65 2074     on..Environment 
200644f0:	6163 6c6c 6620 6f72 206d 204d 6f6d 6564     call from M mode
20064500:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
20064510:	6170 6567 6620 7561 746c 0000 6f4c 6461     page fault..Load
20064520:	7020 6761 2065 6166 6c75 0074 7453 726f      page fault.Stor
20064530:	2065 6170 6567 6620 7561 746c 0000 0000     e page fault....

20064540 <__FUNCTION__.0>:
20064540:	7263 7361 5f68 7564 706d 0000               crash_dump..

2006454c <exception_cause_0_string>:
2006454c:	4338 2006 4354 2006 4364 2006 41e4 2006     8C. TC. dC. .A. 
	...

20064564 <exception_cause_12_string>:
20064564:	41c0 2006 41e4 2006 0000 0000 0000 0000     .A. .A. ........

20064574 <exception_cause_13_string>:
20064574:	4194 2006 41b0 2006 0000 0000 0000 0000     .A. .A. ........

20064584 <exception_cause_15_string>:
20064584:	4164 2006 4184 2006 0000 0000 0000 0000     dA. .A. ........

20064594 <exception_cause_1_string>:
20064594:	42e4 2006 41e4 2006 4300 2006 41e4 2006     .B. .A. .C. .A. 
200645a4:	4320 2006 41e4 2006 0000 0000 0000 0000      C. .A. ........

200645b4 <exception_cause_2_string>:
200645b4:	42bc 2006 42d0 2006 0000 0000 0000 0000     .B. .B. ........

200645c4 <exception_cause_4_string>:
200645c4:	42a4 2006 41b0 2006 0000 0000 0000 0000     .B. .A. ........

200645d4 <exception_cause_5_string>:
200645d4:	4264 2006 41b0 2006 4278 2006 41b0 2006     dB. .A. xB. .A. 
200645e4:	4294 2006 0000 0000 0000 0000 0000 0000     .B. ............

200645f4 <exception_cause_6_string>:
200645f4:	4244 2006 4184 2006 0000 0000 0000 0000     DB. .A. ........

20064604 <exception_cause_7_string>:
20064604:	41f8 2006 4184 2006 4214 2006 4184 2006     .A. .A. .B. .A. 
20064614:	4234 2006 0000 0000 0000 0000 0000 0000     4B. ............

20064624 <exception_cause_string_tbl>:
20064624:	454c 2006 4594 2006 45b4 2006 0000 0000     LE. .E. .E. ....
20064634:	45c4 2006 45d4 2006 45f4 2006 4604 2006     .E. .E. .E. .F. 
	...
20064654:	4564 2006 4574 2006 0000 0000 4584 2006     dE. tE. .....E. 

20064664 <exception_code_string>:
20064664:	4364 2006 440c 2006 42bc 2006 4428 2006     dC. .D. .B. (D. 
20064674:	4434 2006 444c 2006 4460 2006 447c 2006     4D. LD. `D. |D. 
20064684:	4490 2006 44b0 2006 44d0 2006 44e4 2006     .D. .D. .D. .D. 
20064694:	4504 2006 451c 2006 0000 0000 452c 2006     .E. .E. ....,E. 

200646a4 <register_string>:
200646a4:	4384 2006 438c 2006 4390 2006 4394 2006     .C. .C. .C. .C. 
200646b4:	4398 2006 439c 2006 43a0 2006 43a4 2006     .C. .C. .C. .C. 
200646c4:	43a8 2006 43b0 2006 43b4 2006 43b8 2006     .C. .C. .C. .C. 
200646d4:	43bc 2006 43c0 2006 43c4 2006 43c8 2006     .C. .C. .C. .C. 
200646e4:	43cc 2006 43d0 2006 43d4 2006 43d8 2006     .C. .C. .C. .C. 
200646f4:	43dc 2006 43e0 2006 43e4 2006 43e8 2006     .C. .C. .C. .C. 
20064704:	43ec 2006 43f0 2006 43f4 2006 43f8 2006     .C. .C. .C. .C. 
20064714:	43fc 2006 4400 2006 4404 2006 4408 2006     .C. .D. .D. .D. 
20064724:	4544 2056 5049 2043 5041 2049 736d 2067     DEV IPC API msg 
20064734:	554e 4c4c 000a 0000 6f48 7473 4120 4950     NULL....Host API
20064744:	5520 6b6e 6f6e 6e77 6520 7476 2528 2978      Unknown evt(%x)
20064754:	000a 0000 0426 2006 0434 2006 04ec 2006     ....&.. 4.. ... 
20064764:	048c 2006 0604 2006 0618 2006 068e 2006     ... ... ... ... 
20064774:	048c 2006 048c 2006 616c 7473 6920 696e     ... ... last ini
20064784:	2063 7069 2063 6f6e 2074 6468 206c 000a     c ipc not hdl ..
20064794:	7348 4d74 6773 6553 646e 7720 6961 2074     HstMsgSend wait 
200647a4:	6e69 6369 6920 6370 6420 6e6f 0a65 0000     inic ipc done...
200647b4:	6e69 6369 615f 6970 685f 736f 5f74 6174     inic_api_host_ta
200647c4:	6b73 0000 7243 6165 6574 6120 6970 685f     sk..Create api_h
200647d4:	736f 5f74 6174 6b73 4520 7272 000a 0000     ost_task Err....
200647e4:	4e49 4349 0000 0000 6e69 6369 6d5f 5f70     INIC....inic_mp_
200647f4:	6f63 6d6d 6e61 2064 6166 6c69 000a 0000     command fail....
20064804:	7250 7669 7461 2065 654d 7373 6761 3a65     Private Message:
20064814:	2520 0a73 0000 0000                          %s.....

2006481c <__func__.0>:
2006481c:	6e69 6369 615f 6970 685f 736f 5f74 656d     inic_api_host_me
2006482c:	7373 6761 5f65 6573 646e 0000 6f68 7473     ssage_send..host
2006483c:	735f 626b 785f 2078 6f6e 2074 7266 6565     _skb_xx not free
2006484c:	000a 0000 7325 3e3d 6b73 2062 616d 6c6c     ....%s=>skb mall
2006485c:	636f 6620 6961 216c 0d0a 0000 6f48 7473     oc fail!....Host
2006486c:	5520 6b6e 6f6e 6e77 6520 6576 746e 2528      Unknown event(%
2006487c:	786c 2129 0d0a 0000                         lx)!....

20064884 <__func__.1>:
20064884:	6e69 6369 685f 736f 5f74 6e69 7469 735f     inic_host_init_s
20064894:	626b 0000 6e69 6369 6d5f 6773 715f 745f     kb..inic_msg_q_t
200648a4:	7361 006b 7243 6165 6574 6d20 6773 715f     ask.Create msg_q
200648b4:	745f 7361 206b 7245 2172 000a 4f4e 6220     _task Err!..NO b
200648c4:	6675 6620 726f 6e20 7765 6e20 646f 7365     uf for new nodes
200648d4:	0a21 0000 6146 6c69 7420 206f 6966 646e     !...Fail to find
200648e4:	7420 6568 7420 6d69 7265 655f 746e 7972      the timer_entry
200648f4:	6920 206e 6974 656d 2072 6174 6c62 2e65      in timer table.
20064904:	000a 0000 4954 454d 0052 0000 6146 6c69     ....TIMER...Fail
20064914:	7420 206f 6e69 7469 7420 6d69 7265 0a2e      to init timer..
20064924:	0000 0000 6f4d 5464 6d69 7265 203a 6f6e     ....ModTimer: no
20064934:	2074 6e69 7469 0a2e 0000 0000 6f4d 5464     t init......ModT
20064944:	6d69 7265 6620 6961 0a6c 0000 6143 636e     imer fail...Canc
20064954:	6c65 6954 656d 2072 6146 6c69 2528 2978     elTimer Fail(%x)
20064964:	000a 0000 6544 546c 6d69 7265 4620 6961     ....DelTimer Fai
20064974:	0a6c 0000                                   l...

20064978 <__clz_tab>:
20064978:	0100 0202 0303 0303 0404 0404 0404 0404     ................
20064988:	0505 0505 0505 0505 0505 0505 0505 0505     ................
20064998:	0606 0606 0606 0606 0606 0606 0606 0606     ................
200649a8:	0606 0606 0606 0606 0606 0606 0606 0606     ................
200649b8:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200649c8:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200649d8:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200649e8:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200649f8:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064a08:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064a18:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064a28:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064a38:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064a48:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064a58:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064a68:	0808 0808 0808 0808 0808 0808 0808 0808     ................

20064a78 <_ctype_>:
20064a78:	2000 2020 2020 2020 2020 2828 2828 2028     .         ((((( 
20064a88:	2020 2020 2020 2020 2020 2020 2020 2020                     
20064a98:	8820 1010 1010 1010 1010 1010 1010 1010      ...............
20064aa8:	0410 0404 0404 0404 0404 1004 1010 1010     ................
20064ab8:	1010 4141 4141 4141 0101 0101 0101 0101     ..AAAAAA........
20064ac8:	0101 0101 0101 0101 0101 0101 1010 1010     ................
20064ad8:	1010 4242 4242 4242 0202 0202 0202 0202     ..BBBBBB........
20064ae8:	0202 0202 0202 0202 0202 0202 1010 1010     ................
20064af8:	0020 0000 0000 0000 0000 0000 0000 0000      ...............
	...

20064b7c <shell_cmd_table>:
20064b7c:	3184 2006 0000 0000 b366 2005 3188 2006     .1. ....f.. .1. 
20064b8c:	31ac 2006 0002 0000 b4c2 2005 31b0 2006     .1. ....... .1. 
20064b9c:	32cc 2006 0002 0000 b41c 2005 32d0 2006     .2. ....... .2. 
20064bac:	3360 2006 0004 0000 b3d2 2005 3368 2006     `3. ....... h3. 

20064bbc <ipc_tickless_table>:
20064bbc:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064bcc:	0000 0000 0010 0000 0000 0000               ............

20064bd8 <ipc_kr4cdsp_table>:
20064bd8:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064be8:	0000 0000 0020 0000 0000 0000               .... .......

20064bf4 <ipc_KM4WKR4_table>:
20064bf4:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064c04:	0000 0000 0010 0000 0002 0000               ............

20064c10 <ipc_shell_table>:
20064c10:	0001 0000 ceee 2005 0000 0000 8eac 2005     ....... ....... 
20064c20:	0000 0000 0010 0000 0003 0000               ............

20064c2c <ipc_api_host_table>:
20064c2c:	0000 0000 0764 2006 0000 0000 8eac 2005     ....d.. ....... 
20064c3c:	0000 0000 0010 0000 0007 0000               ............

20064c48 <ipc_host_event_table>:
20064c48:	0000 0000 0d14 2006 0000 0000 8eac 2005     ....... ....... 
20064c58:	0000 0000 0010 0000 0006 0000               ............

20064c64 <lib_pmc_git_rev>:
20064c64:	6c20 6269 705f 636d 765f 7265 655f 3765      lib_pmc_ver_ee7
20064c74:	3438 3935 3836 5f30 3032 3432 302f 2f37     8459680_2024/07/
20064c84:	3330 312d 3a39 3632 313a 0033               03-19:26:13.

20064c90 <lib_wifi_com_sec_git_rev>:
20064c90:	6c20 6269 775f 6669 5f69 6f63 5f6d 6573      lib_wifi_com_se
20064ca0:	5f63 6576 5f72 6565 3837 3534 3639 3038     c_ver_ee78459680
20064cb0:	325f 3230 2f34 3730 302f 2d33 3931 323a     _2024/07/03-19:2
20064cc0:	3a36 3131                                    6:11.

20064cc5 <__git_ver_table_end__>:
20064cc5:	0000                	c.unimp
	...

20064cc8 <__frame_dummy_init_array_entry>:
20064cc8:	13fa 2006                                   ... 

20064ccc <__do_global_dtors_aux_fini_array_entry>:
20064ccc:	13c0 2006                                   ... 

20064cd0 <__fini_array_end>:
	...

Disassembly of section .sram_image2.text.data:

20065ce0 <FLASH_ClockSwitch>:
20065ce0:	1101                	c.addi	sp,-32
20065ce2:	ca26                	c.swsp	s1,20(sp)
20065ce4:	c452                	c.swsp	s4,8(sp)
20065ce6:	ce06                	c.swsp	ra,28(sp)
20065ce8:	cc22                	c.swsp	s0,24(sp)
20065cea:	c84a                	c.swsp	s2,16(sp)
20065cec:	c64e                	c.swsp	s3,12(sp)
20065cee:	c256                	c.swsp	s5,4(sp)
20065cf0:	84aa                	c.mv	s1,a0
20065cf2:	8a2e                	c.mv	s4,a1
20065cf4:	c1dd                	c.beqz	a1,20065d9a <FLASH_ClockSwitch+0xba>
20065cf6:	de1f20ef          	jal	ra,20058ad6 <Systick_State>
20065cfa:	89aa                	c.mv	s3,a0
20065cfc:	dfbf20ef          	jal	ra,20058af6 <irq_disable_save>
20065d00:	892a                	c.mv	s2,a0
20065d02:	4501                	c.li	a0,0
20065d04:	da7f20ef          	jal	ra,20058aaa <Systick_Cmd>
20065d08:	20001ab7          	lui	s5,0x20001
20065d0c:	4789                	c.li	a5,2
20065d0e:	864a8413          	addi	s0,s5,-1948 # 20000864 <flash_init_para>
20065d12:	08f49e63          	bne	s1,a5,20065dae <FLASH_ClockSwitch+0xce>
20065d16:	4509                	c.li	a0,2
20065d18:	dffa0097          	auipc	ra,0xdffa0
20065d1c:	d26080e7          	jalr	ra,-730(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065d20:	4100c6b7          	lui	a3,0x4100c
20065d24:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065d28:	7775                	c.lui	a4,0xffffd
20065d2a:	177d                	c.addi	a4,-1
20065d2c:	8ff9                	c.and	a5,a4
20065d2e:	24f6ae23          	sw	a5,604(a3)
20065d32:	06344783          	lbu	a5,99(s0) # 9063 <shell_get_argv+0x18f>
20065d36:	c7ad                	c.beqz	a5,20065da0 <FLASH_ClockSwitch+0xc0>
20065d38:	4501                	c.li	a0,0
20065d3a:	dff9e097          	auipc	ra,0xdff9e
20065d3e:	c40080e7          	jalr	ra,-960(ra) # 397a <FLASH_CalibrationNewCmd>
20065d42:	4505                	c.li	a0,1
20065d44:	dff9e097          	auipc	ra,0xdff9e
20065d48:	9f4080e7          	jalr	ra,-1548(ra) # 3738 <FLASH_CalibrationPLLSel>
20065d4c:	dff9e097          	auipc	ra,0xdff9e
20065d50:	a5e080e7          	jalr	ra,-1442(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065d54:	4501                	c.li	a0,0
20065d56:	dff9e097          	auipc	ra,0xdff9e
20065d5a:	9e2080e7          	jalr	ra,-1566(ra) # 3738 <FLASH_CalibrationPLLSel>
20065d5e:	dff9e097          	auipc	ra,0xdff9e
20065d62:	a4c080e7          	jalr	ra,-1460(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065d66:	04040ea3          	sb	zero,93(s0)
20065d6a:	00444583          	lbu	a1,4(s0)
20065d6e:	864a8513          	addi	a0,s5,-1948
20065d72:	dff9d097          	auipc	ra,0xdff9d
20065d76:	f52080e7          	jalr	ra,-174(ra) # 2cc4 <FLASH_SetSpiMode>
20065d7a:	0c0a0c63          	beq	s4,zero,20065e52 <FLASH_ClockSwitch+0x172>
20065d7e:	854e                	c.mv	a0,s3
20065d80:	d2bf20ef          	jal	ra,20058aaa <Systick_Cmd>
20065d84:	4462                	c.lwsp	s0,24(sp)
20065d86:	40f2                	c.lwsp	ra,28(sp)
20065d88:	44d2                	c.lwsp	s1,20(sp)
20065d8a:	49b2                	c.lwsp	s3,12(sp)
20065d8c:	4a22                	c.lwsp	s4,8(sp)
20065d8e:	4a92                	c.lwsp	s5,4(sp)
20065d90:	854a                	c.mv	a0,s2
20065d92:	4942                	c.lwsp	s2,16(sp)
20065d94:	6105                	c.addi16sp	sp,32
20065d96:	d69f206f          	jal	zero,20058afe <irq_enable_restore>
20065d9a:	4901                	c.li	s2,0
20065d9c:	4981                	c.li	s3,0
20065d9e:	b7ad                	c.j	20065d08 <FLASH_ClockSwitch+0x28>
20065da0:	4581                	c.li	a1,0
20065da2:	4501                	c.li	a0,0
20065da4:	dff9e097          	auipc	ra,0xdff9e
20065da8:	dda080e7          	jalr	ra,-550(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
20065dac:	bf59                	c.j	20065d42 <FLASH_ClockSwitch+0x62>
20065dae:	ccb5                	c.beqz	s1,20065e2a <FLASH_ClockSwitch+0x14a>
20065db0:	4501                	c.li	a0,0
20065db2:	dff9e097          	auipc	ra,0xdff9e
20065db6:	986080e7          	jalr	ra,-1658(ra) # 3738 <FLASH_CalibrationPLLSel>
20065dba:	dff9e097          	auipc	ra,0xdff9e
20065dbe:	992080e7          	jalr	ra,-1646(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065dc2:	4505                	c.li	a0,1
20065dc4:	dff9e097          	auipc	ra,0xdff9e
20065dc8:	974080e7          	jalr	ra,-1676(ra) # 3738 <FLASH_CalibrationPLLSel>
20065dcc:	dff9e097          	auipc	ra,0xdff9e
20065dd0:	980080e7          	jalr	ra,-1664(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065dd4:	06344503          	lbu	a0,99(s0)
20065dd8:	891d                	c.andi	a0,7
20065dda:	dff9e097          	auipc	ra,0xdff9e
20065dde:	a50080e7          	jalr	ra,-1456(ra) # 382a <FLASH_CalibrationPLLPS_Shift>
20065de2:	4100c737          	lui	a4,0x4100c
20065de6:	25c72783          	lw	a5,604(a4) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065dea:	668d                	c.lui	a3,0x3
20065dec:	8fd5                	c.or	a5,a3
20065dee:	24f72e23          	sw	a5,604(a4)
20065df2:	4100c6b7          	lui	a3,0x4100c
20065df6:	470d                	c.li	a4,3
20065df8:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065dfc:	8b8d                	c.andi	a5,3
20065dfe:	fee79de3          	bne	a5,a4,20065df8 <FLASH_ClockSwitch+0x118>
20065e02:	06344783          	lbu	a5,99(s0)
20065e06:	cf8d                	c.beqz	a5,20065e40 <FLASH_ClockSwitch+0x160>
20065e08:	4505                	c.li	a0,1
20065e0a:	dff9e097          	auipc	ra,0xdff9e
20065e0e:	b70080e7          	jalr	ra,-1168(ra) # 397a <FLASH_CalibrationNewCmd>
20065e12:	4505                	c.li	a0,1
20065e14:	e091                	c.bnez	s1,20065e18 <FLASH_ClockSwitch+0x138>
20065e16:	4501                	c.li	a0,0
20065e18:	dffa0097          	auipc	ra,0xdffa0
20065e1c:	c26080e7          	jalr	ra,-986(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065e20:	05c44783          	lbu	a5,92(s0)
20065e24:	04f40ea3          	sb	a5,93(s0)
20065e28:	b789                	c.j	20065d6a <FLASH_ClockSwitch+0x8a>
20065e2a:	4505                	c.li	a0,1
20065e2c:	dff9e097          	auipc	ra,0xdff9e
20065e30:	90c080e7          	jalr	ra,-1780(ra) # 3738 <FLASH_CalibrationPLLSel>
20065e34:	dff9e097          	auipc	ra,0xdff9e
20065e38:	918080e7          	jalr	ra,-1768(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065e3c:	4501                	c.li	a0,0
20065e3e:	b759                	c.j	20065dc4 <FLASH_ClockSwitch+0xe4>
20065e40:	05c44503          	lbu	a0,92(s0)
20065e44:	4585                	c.li	a1,1
20065e46:	1579                	c.addi	a0,-2
20065e48:	dff9e097          	auipc	ra,0xdff9e
20065e4c:	d36080e7          	jalr	ra,-714(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
20065e50:	b7c9                	c.j	20065e12 <FLASH_ClockSwitch+0x132>
20065e52:	40f2                	c.lwsp	ra,28(sp)
20065e54:	4462                	c.lwsp	s0,24(sp)
20065e56:	44d2                	c.lwsp	s1,20(sp)
20065e58:	4942                	c.lwsp	s2,16(sp)
20065e5a:	49b2                	c.lwsp	s3,12(sp)
20065e5c:	4a22                	c.lwsp	s4,8(sp)
20065e5e:	4a92                	c.lwsp	s5,4(sp)
20065e60:	6105                	c.addi16sp	sp,32
20065e62:	8082                	c.jr	ra

20065e64 <FLASH_UserMode_Exit>:
20065e64:	4100a737          	lui	a4,0x4100a
20065e68:	431c                	c.lw	a5,0(a4)
20065e6a:	0786                	c.slli	a5,0x1
20065e6c:	8385                	c.srli	a5,0x1
20065e6e:	c31c                	c.sw	a5,0(a4)
20065e70:	431c                	c.lw	a5,0(a4)
20065e72:	fe07cfe3          	blt	a5,zero,20065e70 <FLASH_UserMode_Exit+0xc>
20065e76:	0ff0000f          	fence	iorw,iorw
20065e7a:	0001                	c.addi	zero,0
20065e7c:	0001                	c.addi	zero,0
20065e7e:	0001                	c.addi	zero,0
20065e80:	0001                	c.addi	zero,0
20065e82:	0001                	c.addi	zero,0
20065e84:	0001                	c.addi	zero,0
20065e86:	0001                	c.addi	zero,0
20065e88:	0001                	c.addi	zero,0
20065e8a:	0001                	c.addi	zero,0
20065e8c:	0001                	c.addi	zero,0
20065e8e:	0001                	c.addi	zero,0
20065e90:	8082                	c.jr	ra

20065e92 <FLASH_WaitBusy_InUserMode>:
20065e92:	7179                	c.addi16sp	sp,-48
20065e94:	d422                	c.swsp	s0,40(sp)
20065e96:	20001437          	lui	s0,0x20001
20065e9a:	d226                	c.swsp	s1,36(sp)
20065e9c:	d04a                	c.swsp	s2,32(sp)
20065e9e:	ce4e                	c.swsp	s3,28(sp)
20065ea0:	cc52                	c.swsp	s4,24(sp)
20065ea2:	ca56                	c.swsp	s5,20(sp)
20065ea4:	d606                	c.swsp	ra,44(sp)
20065ea6:	84aa                	c.mv	s1,a0
20065ea8:	000107a3          	sb	zero,15(sp)
20065eac:	4781                	c.li	a5,0
20065eae:	4911                	c.li	s2,4
20065eb0:	4985                	c.li	s3,1
20065eb2:	4a0d                	c.li	s4,3
20065eb4:	86440413          	addi	s0,s0,-1948 # 20000864 <flash_init_para>
20065eb8:	4100aab7          	lui	s5,0x4100a
20065ebc:	ec91                	c.bnez	s1,20065ed8 <FLASH_WaitBusy_InUserMode+0x46>
20065ebe:	028aa783          	lw	a5,40(s5) # 4100a028 <__km4_bd_boot_download_addr__+0x10ff8028>
20065ec2:	8b85                	c.andi	a5,1
20065ec4:	ffe5                	c.bnez	a5,20065ebc <FLASH_WaitBusy_InUserMode+0x2a>
20065ec6:	50b2                	c.lwsp	ra,44(sp)
20065ec8:	5422                	c.lwsp	s0,40(sp)
20065eca:	5492                	c.lwsp	s1,36(sp)
20065ecc:	5902                	c.lwsp	s2,32(sp)
20065ece:	49f2                	c.lwsp	s3,28(sp)
20065ed0:	4a62                	c.lwsp	s4,24(sp)
20065ed2:	4ad2                	c.lwsp	s5,20(sp)
20065ed4:	6145                	c.addi16sp	sp,48
20065ed6:	8082                	c.jr	ra
20065ed8:	01249563          	bne	s1,s2,20065ee2 <FLASH_WaitBusy_InUserMode+0x50>
20065edc:	008aa783          	lw	a5,8(s5)
20065ee0:	b7cd                	c.j	20065ec2 <FLASH_WaitBusy_InUserMode+0x30>
20065ee2:	01349d63          	bne	s1,s3,20065efc <FLASH_WaitBusy_InUserMode+0x6a>
20065ee6:	05244503          	lbu	a0,82(s0)
20065eea:	00f10613          	addi	a2,sp,15
20065eee:	4585                	c.li	a1,1
20065ef0:	288d                	c.jal	20065f62 <FLASH_RxCmd_InUserMode>
20065ef2:	00f14783          	lbu	a5,15(sp)
20065ef6:	4818                	c.lw	a4,16(s0)
20065ef8:	8ff9                	c.and	a5,a4
20065efa:	b7e9                	c.j	20065ec4 <FLASH_WaitBusy_InUserMode+0x32>
20065efc:	fd4494e3          	bne	s1,s4,20065ec4 <FLASH_WaitBusy_InUserMode+0x32>
20065f00:	05244503          	lbu	a0,82(s0)
20065f04:	00f10613          	addi	a2,sp,15
20065f08:	4585                	c.li	a1,1
20065f0a:	28a1                	c.jal	20065f62 <FLASH_RxCmd_InUserMode>
20065f0c:	00f14703          	lbu	a4,15(sp)
20065f10:	4854                	c.lw	a3,20(s0)
20065f12:	4785                	c.li	a5,1
20065f14:	8ef9                	c.and	a3,a4
20065f16:	d6dd                	c.beqz	a3,20065ec4 <FLASH_WaitBusy_InUserMode+0x32>
20065f18:	481c                	c.lw	a5,16(s0)
20065f1a:	8ff9                	c.and	a5,a4
20065f1c:	00f037b3          	sltu	a5,zero,a5
20065f20:	b755                	c.j	20065ec4 <FLASH_WaitBusy_InUserMode+0x32>

20065f22 <FLASH_UserMode_Enter>:
20065f22:	1141                	c.addi	sp,-16
20065f24:	c606                	c.swsp	ra,12(sp)
20065f26:	0ff0000f          	fence	iorw,iorw
20065f2a:	0ff0000f          	fence	iorw,iorw
20065f2e:	0001                	c.addi	zero,0
20065f30:	0001                	c.addi	zero,0
20065f32:	0001                	c.addi	zero,0
20065f34:	0001                	c.addi	zero,0
20065f36:	0001                	c.addi	zero,0
20065f38:	0001                	c.addi	zero,0
20065f3a:	0001                	c.addi	zero,0
20065f3c:	0001                	c.addi	zero,0
20065f3e:	0001                	c.addi	zero,0
20065f40:	0001                	c.addi	zero,0
20065f42:	0001                	c.addi	zero,0
20065f44:	4501                	c.li	a0,0
20065f46:	37b1                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
20065f48:	4100a737          	lui	a4,0x4100a
20065f4c:	431c                	c.lw	a5,0(a4)
20065f4e:	800006b7          	lui	a3,0x80000
20065f52:	8fd5                	c.or	a5,a3
20065f54:	c31c                	c.sw	a5,0(a4)
20065f56:	431c                	c.lw	a5,0(a4)
20065f58:	fe07dfe3          	bge	a5,zero,20065f56 <FLASH_UserMode_Enter+0x34>
20065f5c:	40b2                	c.lwsp	ra,12(sp)
20065f5e:	0141                	c.addi	sp,16
20065f60:	8082                	c.jr	ra

20065f62 <FLASH_RxCmd_InUserMode>:
20065f62:	1101                	c.addi	sp,-32
20065f64:	cc22                	c.swsp	s0,24(sp)
20065f66:	20001437          	lui	s0,0x20001
20065f6a:	ca26                	c.swsp	s1,20(sp)
20065f6c:	c64e                	c.swsp	s3,12(sp)
20065f6e:	84ae                	c.mv	s1,a1
20065f70:	89aa                	c.mv	s3,a0
20065f72:	4581                	c.li	a1,0
20065f74:	86440513          	addi	a0,s0,-1948 # 20000864 <flash_init_para>
20065f78:	c84a                	c.swsp	s2,16(sp)
20065f7a:	ce06                	c.swsp	ra,28(sp)
20065f7c:	8932                	c.mv	s2,a2
20065f7e:	dff9d097          	auipc	ra,0xdff9d
20065f82:	d46080e7          	jalr	ra,-698(ra) # 2cc4 <FLASH_SetSpiMode>
20065f86:	4100a7b7          	lui	a5,0x4100a
20065f8a:	10078613          	addi	a2,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
20065f8e:	4e18                	c.lw	a4,24(a2)
20065f90:	fff106b7          	lui	a3,0xfff10
20065f94:	fff68593          	addi	a1,a3,-1 # fff0ffff <__ctrace_end__+0x35f0ffff>
20065f98:	8f6d                	c.and	a4,a1
20065f9a:	ce18                	c.sw	a4,24(a2)
20065f9c:	00849713          	slli	a4,s1,0x8
20065fa0:	8321                	c.srli	a4,0x8
20065fa2:	c3d8                	c.sw	a4,4(a5)
20065fa4:	1207a823          	sw	zero,304(a5)
20065fa8:	4398                	c.lw	a4,0(a5)
20065faa:	cff68693          	addi	a3,a3,-769
20065fae:	8f75                	c.and	a4,a3
20065fb0:	30076713          	ori	a4,a4,768
20065fb4:	c398                	c.sw	a4,0(a5)
20065fb6:	07378023          	sb	s3,96(a5)
20065fba:	4705                	c.li	a4,1
20065fbc:	c798                	c.sw	a4,8(a5)
20065fbe:	4100a6b7          	lui	a3,0x4100a
20065fc2:	4781                	c.li	a5,0
20065fc4:	02979463          	bne	a5,s1,20065fec <FLASH_RxCmd_InUserMode+0x8a>
20065fc8:	4511                	c.li	a0,4
20065fca:	35e1                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
20065fcc:	86440793          	addi	a5,s0,-1948
20065fd0:	86440513          	addi	a0,s0,-1948
20065fd4:	4462                	c.lwsp	s0,24(sp)
20065fd6:	40f2                	c.lwsp	ra,28(sp)
20065fd8:	44d2                	c.lwsp	s1,20(sp)
20065fda:	4942                	c.lwsp	s2,16(sp)
20065fdc:	49b2                	c.lwsp	s3,12(sp)
20065fde:	0047c583          	lbu	a1,4(a5)
20065fe2:	6105                	c.addi16sp	sp,32
20065fe4:	dff9d317          	auipc	t1,0xdff9d
20065fe8:	ce030067          	jalr	zero,-800(t1) # 2cc4 <FLASH_SetSpiMode>
20065fec:	5698                	c.lw	a4,40(a3)
20065fee:	8b21                	c.andi	a4,8
20065ff0:	db71                	c.beqz	a4,20065fc4 <FLASH_RxCmd_InUserMode+0x62>
20065ff2:	0606c603          	lbu	a2,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20065ff6:	00f90733          	add	a4,s2,a5
20065ffa:	0785                	c.addi	a5,1
20065ffc:	00c70023          	sb	a2,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20066000:	b7d1                	c.j	20065fc4 <FLASH_RxCmd_InUserMode+0x62>

20066002 <FLASH_RxCmd>:
20066002:	1101                	c.addi	sp,-32
20066004:	ce06                	c.swsp	ra,28(sp)
20066006:	c62a                	c.swsp	a0,12(sp)
20066008:	c42e                	c.swsp	a1,8(sp)
2006600a:	c232                	c.swsp	a2,4(sp)
2006600c:	3f19                	c.jal	20065f22 <FLASH_UserMode_Enter>
2006600e:	4612                	c.lwsp	a2,4(sp)
20066010:	45a2                	c.lwsp	a1,8(sp)
20066012:	4532                	c.lwsp	a0,12(sp)
20066014:	37b9                	c.jal	20065f62 <FLASH_RxCmd_InUserMode>
20066016:	40f2                	c.lwsp	ra,28(sp)
20066018:	6105                	c.addi16sp	sp,32
2006601a:	b5a9                	c.j	20065e64 <FLASH_UserMode_Exit>

2006601c <FLASH_TxCmd_InUserMode>:
2006601c:	4100a7b7          	lui	a5,0x4100a
20066020:	1141                	c.addi	sp,-16
20066022:	c606                	c.swsp	ra,12(sp)
20066024:	c422                	c.swsp	s0,8(sp)
20066026:	c226                	c.swsp	s1,4(sp)
20066028:	c04a                	c.swsp	s2,0(sp)
2006602a:	10078713          	addi	a4,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
2006602e:	01872903          	lw	s2,24(a4)
20066032:	4384                	c.lw	s1,0(a5)
20066034:	4394                	c.lw	a3,0(a5)
20066036:	ffc10837          	lui	a6,0xffc10
2006603a:	cff80813          	addi	a6,a6,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
2006603e:	0106f6b3          	and	a3,a3,a6
20066042:	c394                	c.sw	a3,0(a5)
20066044:	4f14                	c.lw	a3,24(a4)
20066046:	fff10837          	lui	a6,0xfff10
2006604a:	187d                	c.addi	a6,-1
2006604c:	0106f6b3          	and	a3,a3,a6
20066050:	cf14                	c.sw	a3,24(a4)
20066052:	0007a223          	sw	zero,4(a5)
20066056:	db0c                	c.sw	a1,48(a4)
20066058:	06a78023          	sb	a0,96(a5)
2006605c:	4100a737          	lui	a4,0x4100a
20066060:	4781                	c.li	a5,0
20066062:	02b7e163          	bltu	a5,a1,20066084 <FLASH_TxCmd_InUserMode+0x68>
20066066:	4100a437          	lui	s0,0x4100a
2006606a:	4785                	c.li	a5,1
2006606c:	c41c                	c.sw	a5,8(s0)
2006606e:	4511                	c.li	a0,4
20066070:	350d                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
20066072:	11242c23          	sw	s2,280(s0) # 4100a118 <__km4_bd_boot_download_addr__+0x10ff8118>
20066076:	c004                	c.sw	s1,0(s0)
20066078:	40b2                	c.lwsp	ra,12(sp)
2006607a:	4422                	c.lwsp	s0,8(sp)
2006607c:	4492                	c.lwsp	s1,4(sp)
2006607e:	4902                	c.lwsp	s2,0(sp)
20066080:	0141                	c.addi	sp,16
20066082:	8082                	c.jr	ra
20066084:	00f606b3          	add	a3,a2,a5
20066088:	0006c683          	lbu	a3,0(a3)
2006608c:	0785                	c.addi	a5,1
2006608e:	06d70023          	sb	a3,96(a4) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20066092:	bfc1                	c.j	20066062 <FLASH_TxCmd_InUserMode+0x46>

20066094 <FLASH_WriteEn_InUserMode>:
20066094:	1141                	c.addi	sp,-16
20066096:	4505                	c.li	a0,1
20066098:	c606                	c.swsp	ra,12(sp)
2006609a:	3be5                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
2006609c:	200017b7          	lui	a5,0x20001
200660a0:	8b47c503          	lbu	a0,-1868(a5) # 200008b4 <flash_init_para+0x50>
200660a4:	4601                	c.li	a2,0
200660a6:	4581                	c.li	a1,0
200660a8:	3f95                	c.jal	2006601c <FLASH_TxCmd_InUserMode>
200660aa:	40b2                	c.lwsp	ra,12(sp)
200660ac:	450d                	c.li	a0,3
200660ae:	0141                	c.addi	sp,16
200660b0:	b3cd                	c.j	20065e92 <FLASH_WaitBusy_InUserMode>

200660b2 <FLASH_TxData>:
200660b2:	1101                	c.addi	sp,-32
200660b4:	0ff57793          	andi	a5,a0,255
200660b8:	cc22                	c.swsp	s0,24(sp)
200660ba:	ca26                	c.swsp	s1,20(sp)
200660bc:	c84a                	c.swsp	s2,16(sp)
200660be:	ce06                	c.swsp	ra,28(sp)
200660c0:	c64e                	c.swsp	s3,12(sp)
200660c2:	97ae                	c.add	a5,a1
200660c4:	10000713          	addi	a4,zero,256
200660c8:	842a                	c.mv	s0,a0
200660ca:	84ae                	c.mv	s1,a1
200660cc:	8932                	c.mv	s2,a2
200660ce:	00f77a63          	bgeu	a4,a5,200660e2 <FLASH_TxData+0x30>
200660d2:	20062537          	lui	a0,0x20062
200660d6:	23a00593          	addi	a1,zero,570
200660da:	f7450513          	addi	a0,a0,-140 # 20061f74 <strtol+0xc>
200660de:	f48f90ef          	jal	ra,2005f826 <io_assert_failed>
200660e2:	3581                	c.jal	20065f22 <FLASH_UserMode_Enter>
200660e4:	3f45                	c.jal	20066094 <FLASH_WriteEn_InUserMode>
200660e6:	4100a7b7          	lui	a5,0x4100a
200660ea:	0007a983          	lw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200660ee:	4398                	c.lw	a4,0(a5)
200660f0:	ffc106b7          	lui	a3,0xffc10
200660f4:	cff68693          	addi	a3,a3,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
200660f8:	8f75                	c.and	a4,a3
200660fa:	200016b7          	lui	a3,0x20001
200660fe:	c398                	c.sw	a4,0(a5)
20066100:	86468693          	addi	a3,a3,-1948 # 20000864 <flash_init_para>
20066104:	0646c703          	lbu	a4,100(a3)
20066108:	1187a603          	lw	a2,280(a5)
2006610c:	000f05b7          	lui	a1,0xf0
20066110:	0742                	c.slli	a4,0x10
20066112:	8f6d                	c.and	a4,a1
20066114:	fff105b7          	lui	a1,0xfff10
20066118:	15fd                	c.addi	a1,-1
2006611a:	8e6d                	c.and	a2,a1
2006611c:	8f51                	c.or	a4,a2
2006611e:	10e7ac23          	sw	a4,280(a5)
20066122:	00849713          	slli	a4,s1,0x8
20066126:	0007a223          	sw	zero,4(a5)
2006612a:	8321                	c.srli	a4,0x8
2006612c:	12e7a823          	sw	a4,304(a5)
20066130:	4709                	c.li	a4,2
20066132:	06e78023          	sb	a4,96(a5)
20066136:	0646c683          	lbu	a3,100(a3)
2006613a:	4711                	c.li	a4,4
2006613c:	00e69663          	bne	a3,a4,20066148 <FLASH_TxData+0x96>
20066140:	01845713          	srli	a4,s0,0x18
20066144:	06e78023          	sb	a4,96(a5)
20066148:	01045713          	srli	a4,s0,0x10
2006614c:	4100a7b7          	lui	a5,0x4100a
20066150:	0ff77713          	andi	a4,a4,255
20066154:	06e78023          	sb	a4,96(a5) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20066158:	00845713          	srli	a4,s0,0x8
2006615c:	0ff77713          	andi	a4,a4,255
20066160:	06e78023          	sb	a4,96(a5)
20066164:	0ff47413          	andi	s0,s0,255
20066168:	06878023          	sb	s0,96(a5)
2006616c:	4100a6b7          	lui	a3,0x4100a
20066170:	4781                	c.li	a5,0
20066172:	00978563          	beq	a5,s1,2006617c <FLASH_TxData+0xca>
20066176:	5698                	c.lw	a4,40(a3)
20066178:	8b09                	c.andi	a4,2
2006617a:	eb05                	c.bnez	a4,200661aa <FLASH_TxData+0xf8>
2006617c:	4685                	c.li	a3,1
2006617e:	4100a737          	lui	a4,0x4100a
20066182:	c714                	c.sw	a3,8(a4)
20066184:	4100a6b7          	lui	a3,0x4100a
20066188:	0297e963          	bltu	a5,s1,200661ba <FLASH_TxData+0x108>
2006618c:	4511                	c.li	a0,4
2006618e:	3311                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
20066190:	4100a7b7          	lui	a5,0x4100a
20066194:	0137a023          	sw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20066198:	4505                	c.li	a0,1
2006619a:	39e5                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
2006619c:	4462                	c.lwsp	s0,24(sp)
2006619e:	40f2                	c.lwsp	ra,28(sp)
200661a0:	44d2                	c.lwsp	s1,20(sp)
200661a2:	4942                	c.lwsp	s2,16(sp)
200661a4:	49b2                	c.lwsp	s3,12(sp)
200661a6:	6105                	c.addi16sp	sp,32
200661a8:	b975                	c.j	20065e64 <FLASH_UserMode_Exit>
200661aa:	00f90733          	add	a4,s2,a5
200661ae:	00074703          	lbu	a4,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200661b2:	0785                	c.addi	a5,1
200661b4:	06e68023          	sb	a4,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
200661b8:	bf6d                	c.j	20066172 <FLASH_TxData+0xc0>
200661ba:	5698                	c.lw	a4,40(a3)
200661bc:	8b09                	c.andi	a4,2
200661be:	d769                	c.beqz	a4,20066188 <FLASH_TxData+0xd6>
200661c0:	00f90733          	add	a4,s2,a5
200661c4:	00074703          	lbu	a4,0(a4)
200661c8:	0785                	c.addi	a5,1
200661ca:	06e68023          	sb	a4,96(a3)
200661ce:	bf6d                	c.j	20066188 <FLASH_TxData+0xd6>

200661d0 <FLASH_SetStatus>:
200661d0:	1101                	c.addi	sp,-32
200661d2:	ce06                	c.swsp	ra,28(sp)
200661d4:	cc22                	c.swsp	s0,24(sp)
200661d6:	c62a                	c.swsp	a0,12(sp)
200661d8:	842e                	c.mv	s0,a1
200661da:	c432                	c.swsp	a2,8(sp)
200661dc:	3399                	c.jal	20065f22 <FLASH_UserMode_Enter>
200661de:	3d5d                	c.jal	20066094 <FLASH_WriteEn_InUserMode>
200661e0:	4622                	c.lwsp	a2,8(sp)
200661e2:	4532                	c.lwsp	a0,12(sp)
200661e4:	0ff47593          	andi	a1,s0,255
200661e8:	3d15                	c.jal	2006601c <FLASH_TxCmd_InUserMode>
200661ea:	4505                	c.li	a0,1
200661ec:	315d                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
200661ee:	4462                	c.lwsp	s0,24(sp)
200661f0:	40f2                	c.lwsp	ra,28(sp)
200661f2:	6105                	c.addi16sp	sp,32
200661f4:	b985                	c.j	20065e64 <FLASH_UserMode_Exit>

200661f6 <FLASH_SetStatusBits>:
200661f6:	7179                	c.addi16sp	sp,-48
200661f8:	d606                	c.swsp	ra,44(sp)
200661fa:	d422                	c.swsp	s0,40(sp)
200661fc:	d226                	c.swsp	s1,36(sp)
200661fe:	ce4e                	c.swsp	s3,28(sp)
20066200:	842a                	c.mv	s0,a0
20066202:	89ae                	c.mv	s3,a1
20066204:	200014b7          	lui	s1,0x20001
20066208:	d04a                	c.swsp	s2,32(sp)
2006620a:	3b21                	c.jal	20065f22 <FLASH_UserMode_Enter>
2006620c:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
20066210:	0527c503          	lbu	a0,82(a5)
20066214:	0070                	c.addi4spn	a2,sp,12
20066216:	4585                	c.li	a1,1
20066218:	33a9                	c.jal	20065f62 <FLASH_RxCmd_InUserMode>
2006621a:	4785                	c.li	a5,1
2006621c:	00c14703          	lbu	a4,12(sp)
20066220:	86448493          	addi	s1,s1,-1948
20066224:	04f99d63          	bne	s3,a5,2006627e <FLASH_SetStatusBits+0x88>
20066228:	00e467b3          	or	a5,s0,a4
2006622c:	0ff7f793          	andi	a5,a5,255
20066230:	00f10623          	sb	a5,12(sp)
20066234:	4c9c                	c.lw	a5,24(s1)
20066236:	4905                	c.li	s2,1
20066238:	c385                	c.beqz	a5,20066258 <FLASH_SetStatusBits+0x62>
2006623a:	0534c503          	lbu	a0,83(s1)
2006623e:	00d10613          	addi	a2,sp,13
20066242:	4585                	c.li	a1,1
20066244:	3b39                	c.jal	20065f62 <FLASH_RxCmd_InUserMode>
20066246:	00d14503          	lbu	a0,13(sp)
2006624a:	8021                	c.srli	s0,0x8
2006624c:	03299d63          	bne	s3,s2,20066286 <FLASH_SetStatusBits+0x90>
20066250:	8c49                	c.or	s0,a0
20066252:	008106a3          	sb	s0,13(sp)
20066256:	4909                	c.li	s2,2
20066258:	0554c783          	lbu	a5,85(s1)
2006625c:	eb8d                	c.bnez	a5,2006628e <FLASH_SetStatusBits+0x98>
2006625e:	3d1d                	c.jal	20066094 <FLASH_WriteEn_InUserMode>
20066260:	0544c503          	lbu	a0,84(s1)
20066264:	0070                	c.addi4spn	a2,sp,12
20066266:	85ca                	c.mv	a1,s2
20066268:	3b55                	c.jal	2006601c <FLASH_TxCmd_InUserMode>
2006626a:	4505                	c.li	a0,1
2006626c:	311d                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
2006626e:	3edd                	c.jal	20065e64 <FLASH_UserMode_Exit>
20066270:	50b2                	c.lwsp	ra,44(sp)
20066272:	5422                	c.lwsp	s0,40(sp)
20066274:	5492                	c.lwsp	s1,36(sp)
20066276:	5902                	c.lwsp	s2,32(sp)
20066278:	49f2                	c.lwsp	s3,28(sp)
2006627a:	6145                	c.addi16sp	sp,48
2006627c:	8082                	c.jr	ra
2006627e:	fff44793          	xori	a5,s0,-1
20066282:	8ff9                	c.and	a5,a4
20066284:	b765                	c.j	2006622c <FLASH_SetStatusBits+0x36>
20066286:	fff44413          	xori	s0,s0,-1
2006628a:	8c69                	c.and	s0,a0
2006628c:	b7d9                	c.j	20066252 <FLASH_SetStatusBits+0x5c>
2006628e:	3519                	c.jal	20066094 <FLASH_WriteEn_InUserMode>
20066290:	0544c503          	lbu	a0,84(s1)
20066294:	0070                	c.addi4spn	a2,sp,12
20066296:	4585                	c.li	a1,1
20066298:	3351                	c.jal	2006601c <FLASH_TxCmd_InUserMode>
2006629a:	4505                	c.li	a0,1
2006629c:	3edd                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
2006629e:	3bdd                	c.jal	20066094 <FLASH_WriteEn_InUserMode>
200662a0:	0554c503          	lbu	a0,85(s1)
200662a4:	00d10613          	addi	a2,sp,13
200662a8:	4585                	c.li	a1,1
200662aa:	bf7d                	c.j	20066268 <FLASH_SetStatusBits+0x72>

200662ac <FLASH_Erase>:
200662ac:	7179                	c.addi16sp	sp,-48
200662ae:	d422                	c.swsp	s0,40(sp)
200662b0:	d04a                	c.swsp	s2,32(sp)
200662b2:	d606                	c.swsp	ra,44(sp)
200662b4:	d226                	c.swsp	s1,36(sp)
200662b6:	4789                	c.li	a5,2
200662b8:	892a                	c.mv	s2,a0
200662ba:	842e                	c.mv	s0,a1
200662bc:	00a7fa63          	bgeu	a5,a0,200662d0 <FLASH_Erase+0x24>
200662c0:	20062537          	lui	a0,0x20062
200662c4:	2d700593          	addi	a1,zero,727
200662c8:	f7450513          	addi	a0,a0,-140 # 20061f74 <strtol+0xc>
200662cc:	d5af90ef          	jal	ra,2005f826 <io_assert_failed>
200662d0:	200014b7          	lui	s1,0x20001
200662d4:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
200662d8:	0647c583          	lbu	a1,100(a5)
200662dc:	00845713          	srli	a4,s0,0x8
200662e0:	01045793          	srli	a5,s0,0x10
200662e4:	4611                	c.li	a2,4
200662e6:	0ff7f793          	andi	a5,a5,255
200662ea:	0ff77713          	andi	a4,a4,255
200662ee:	0ff47693          	andi	a3,s0,255
200662f2:	86448493          	addi	s1,s1,-1948
200662f6:	04c59463          	bne	a1,a2,2006633e <FLASH_Erase+0x92>
200662fa:	01845713          	srli	a4,s0,0x18
200662fe:	00f10ea3          	sb	a5,29(sp)
20066302:	00841793          	slli	a5,s0,0x8
20066306:	0442                	c.slli	s0,0x10
20066308:	8041                	c.srli	s0,0x10
2006630a:	8021                	c.srli	s0,0x8
2006630c:	8c5d                	c.or	s0,a5
2006630e:	00e10e23          	sb	a4,28(sp)
20066312:	00811f23          	sh	s0,30(sp)
20066316:	c62e                	c.swsp	a1,12(sp)
20066318:	3129                	c.jal	20065f22 <FLASH_UserMode_Enter>
2006631a:	3bad                	c.jal	20066094 <FLASH_WriteEn_InUserMode>
2006631c:	45b2                	c.lwsp	a1,12(sp)
2006631e:	02091863          	bne	s2,zero,2006634e <FLASH_Erase+0xa2>
20066322:	0564c503          	lbu	a0,86(s1)
20066326:	4601                	c.li	a2,0
20066328:	4581                	c.li	a1,0
2006632a:	39cd                	c.jal	2006601c <FLASH_TxCmd_InUserMode>
2006632c:	4505                	c.li	a0,1
2006632e:	3695                	c.jal	20065e92 <FLASH_WaitBusy_InUserMode>
20066330:	3e15                	c.jal	20065e64 <FLASH_UserMode_Exit>
20066332:	50b2                	c.lwsp	ra,44(sp)
20066334:	5422                	c.lwsp	s0,40(sp)
20066336:	5492                	c.lwsp	s1,36(sp)
20066338:	5902                	c.lwsp	s2,32(sp)
2006633a:	6145                	c.addi16sp	sp,48
2006633c:	8082                	c.jr	ra
2006633e:	00f10e23          	sb	a5,28(sp)
20066342:	00e10ea3          	sb	a4,29(sp)
20066346:	00d10f23          	sb	a3,30(sp)
2006634a:	458d                	c.li	a1,3
2006634c:	b7e9                	c.j	20066316 <FLASH_Erase+0x6a>
2006634e:	4785                	c.li	a5,1
20066350:	00f91663          	bne	s2,a5,2006635c <FLASH_Erase+0xb0>
20066354:	0574c503          	lbu	a0,87(s1)
20066358:	0870                	c.addi4spn	a2,sp,28
2006635a:	bfc1                	c.j	2006632a <FLASH_Erase+0x7e>
2006635c:	4789                	c.li	a5,2
2006635e:	fcf917e3          	bne	s2,a5,2006632c <FLASH_Erase+0x80>
20066362:	0584c503          	lbu	a0,88(s1)
20066366:	0870                	c.addi4spn	a2,sp,28
20066368:	b7c9                	c.j	2006632a <FLASH_Erase+0x7e>

2006636a <plic_get_pending>:
2006636a:	f1402773          	csrrs	a4,mhartid,zero
2006636e:	00251793          	slli	a5,a0,0x2
20066372:	80001537          	lui	a0,0x80001
20066376:	c0050513          	addi	a0,a0,-1024 # 80000c00 <__ctrace_end__+0xb6000c00>
2006637a:	97aa                	c.add	a5,a0
2006637c:	0722                	c.slli	a4,0x8
2006637e:	97ba                	c.add	a5,a4
20066380:	4388                	c.lw	a0,0(a5)
20066382:	8082                	c.jr	ra

20066384 <plic_interrupt_handler>:
20066384:	7179                	c.addi16sp	sp,-48
20066386:	d422                	c.swsp	s0,40(sp)
20066388:	d226                	c.swsp	s1,36(sp)
2006638a:	ce4e                	c.swsp	s3,28(sp)
2006638c:	d606                	c.swsp	ra,44(sp)
2006638e:	d04a                	c.swsp	s2,32(sp)
20066390:	0044                	c.addi4spn	s1,sp,4
20066392:	4401                	c.li	s0,0
20066394:	498d                	c.li	s3,3
20066396:	8522                	c.mv	a0,s0
20066398:	3fc9                	c.jal	2006636a <plic_get_pending>
2006639a:	892a                	c.mv	s2,a0
2006639c:	8522                	c.mv	a0,s0
2006639e:	dff9f097          	auipc	ra,0xdff9f
200663a2:	384080e7          	jalr	ra,900(ra) # 5722 <plic_get_enable>
200663a6:	c088                	c.sw	a0,0(s1)
200663a8:	00a975b3          	and	a1,s2,a0
200663ac:	8522                	c.mv	a0,s0
200663ae:	0405                	c.addi	s0,1
200663b0:	dff9f097          	auipc	ra,0xdff9f
200663b4:	35a080e7          	jalr	ra,858(ra) # 570a <plic_set_enable>
200663b8:	0491                	c.addi	s1,4
200663ba:	fd341ee3          	bne	s0,s3,20066396 <plic_interrupt_handler+0x12>
200663be:	0ff0000f          	fence	iorw,iorw
200663c2:	dff9f097          	auipc	ra,0xdff9f
200663c6:	414080e7          	jalr	ra,1044(ra) # 57d6 <plic_claim_irq>
200663ca:	4592                	c.lwsp	a1,4(sp)
200663cc:	ea418493          	addi	s1,gp,-348 # 20066748 <plic_irq_id>
200663d0:	c088                	c.sw	a0,0(s1)
200663d2:	4501                	c.li	a0,0
200663d4:	dff9f097          	auipc	ra,0xdff9f
200663d8:	336080e7          	jalr	ra,822(ra) # 570a <plic_set_enable>
200663dc:	45a2                	c.lwsp	a1,8(sp)
200663de:	4505                	c.li	a0,1
200663e0:	ea418413          	addi	s0,gp,-348 # 20066748 <plic_irq_id>
200663e4:	dff9f097          	auipc	ra,0xdff9f
200663e8:	326080e7          	jalr	ra,806(ra) # 570a <plic_set_enable>
200663ec:	45b2                	c.lwsp	a1,12(sp)
200663ee:	4509                	c.li	a0,2
200663f0:	dff9f097          	auipc	ra,0xdff9f
200663f4:	31a080e7          	jalr	ra,794(ra) # 570a <plic_set_enable>
200663f8:	4084                	c.lw	s1,0(s1)
200663fa:	c4b1                	c.beqz	s1,20066446 <plic_interrupt_handler+0xc2>
200663fc:	02048493          	addi	s1,s1,32
20066400:	8526                	c.mv	a0,s1
20066402:	77e000e7          	jalr	ra,1918(zero) # 0 <Reset_Handler>
20066406:	892a                	c.mv	s2,a0
20066408:	c531                	c.beqz	a0,20066454 <plic_interrupt_handler+0xd0>
2006640a:	dff9f097          	auipc	ra,0xdff9f
2006640e:	408080e7          	jalr	ra,1032(ra) # 5812 <plic_get_threshold>
20066412:	89aa                	c.mv	s3,a0
20066414:	4008                	c.lw	a0,0(s0)
20066416:	dff9f097          	auipc	ra,0xdff9f
2006641a:	390080e7          	jalr	ra,912(ra) # 57a6 <plic_get_irq_priority>
2006641e:	dff9f097          	auipc	ra,0xdff9f
20066422:	3dc080e7          	jalr	ra,988(ra) # 57fa <plic_set_threshold>
20066426:	8526                	c.mv	a0,s1
20066428:	7b2000e7          	jalr	ra,1970(zero) # 0 <Reset_Handler>
2006642c:	9902                	c.jalr	s2
2006642e:	854e                	c.mv	a0,s3
20066430:	dff9f097          	auipc	ra,0xdff9f
20066434:	3ca080e7          	jalr	ra,970(ra) # 57fa <plic_set_threshold>
20066438:	4008                	c.lw	a0,0(s0)
2006643a:	dff9f097          	auipc	ra,0xdff9f
2006643e:	3ae080e7          	jalr	ra,942(ra) # 57e8 <plic_complete_irq>
20066442:	00042023          	sw	zero,0(s0)
20066446:	50b2                	c.lwsp	ra,44(sp)
20066448:	5422                	c.lwsp	s0,40(sp)
2006644a:	5492                	c.lwsp	s1,36(sp)
2006644c:	5902                	c.lwsp	s2,32(sp)
2006644e:	49f2                	c.lwsp	s3,28(sp)
20066450:	6145                	c.addi16sp	sp,48
20066452:	8082                	c.jr	ra
20066454:	200626b7          	lui	a3,0x20062
20066458:	200625b7          	lui	a1,0x20062
2006645c:	8726                	c.mv	a4,s1
2006645e:	46468693          	addi	a3,a3,1124 # 20062464 <pmap_func+0xd4>
20066462:	05700613          	addi	a2,zero,87
20066466:	48858593          	addi	a1,a1,1160 # 20062488 <pmap_func+0xf8>
2006646a:	450d                	c.li	a0,3
2006646c:	a81f40ef          	jal	ra,2005aeec <rtk_log_write>
20066470:	b7e1                	c.j	20066438 <plic_interrupt_handler+0xb4>

20066472 <plic_get_active_irq_id>:
20066472:	ea41a503          	lw	a0,-348(gp) # 20066748 <plic_irq_id>
20066476:	8082                	c.jr	ra

20066478 <__NVIC_GetActive_IRQ>:
20066478:	06f00793          	addi	a5,zero,111
2006647c:	00a7d463          	bge	a5,a0,20066484 <__NVIC_GetActive_IRQ+0xc>
20066480:	4501                	c.li	a0,0
20066482:	8082                	c.jr	ra
20066484:	34202773          	csrrs	a4,mcause,zero
20066488:	00171793          	slli	a5,a4,0x1
2006648c:	8385                	c.srli	a5,0x1
2006648e:	00055763          	bge	a0,zero,2006649c <__NVIC_GetActive_IRQ+0x24>
20066492:	0541                	c.addi	a0,16
20066494:	fe0746e3          	blt	a4,zero,20066480 <__NVIC_GetActive_IRQ+0x8>
20066498:	8d1d                	c.sub	a0,a5
2006649a:	a821                	c.j	200664b2 <__NVIC_GetActive_IRQ+0x3a>
2006649c:	fe0752e3          	bge	a4,zero,20066480 <__NVIC_GetActive_IRQ+0x8>
200664a0:	477d                	c.li	a4,31
200664a2:	fea75be3          	bge	a4,a0,20066498 <__NVIC_GetActive_IRQ+0x20>
200664a6:	ea41a783          	lw	a5,-348(gp) # 20066748 <plic_irq_id>
200664aa:	02078793          	addi	a5,a5,32
200664ae:	40a78533          	sub	a0,a5,a0
200664b2:	00153513          	sltiu	a0,a0,1
200664b6:	8082                	c.jr	ra

200664b8 <__sram_image2_end__>:
	...

200664c0 <Flash_Layout>:
200664c0:	0000 0000 0000 0800 3fff 0801 0002 0000     .........?......
200664d0:	4000 0801 3fff 081f 0001 0000 0000 0820     .@...?........ .
200664e0:	3fff 0821 0003 0000 4000 0821 3fff 083f     .?!......@!..??.
200664f0:	0004 0000 0000 0840 ffff 086f 0005 0000     ......@...o.....
20066500:	0000 0870 2fff 0870 0006 0000 3000 0870     ..p../p......0p.
20066510:	2fff 0872 0008 0000 ffff ffff ffff ffff     ./r.............
20066520:	00ff 0000 ffff ffff ffff ffff               ............

2006652c <PSRAM_CALIB_PATTERN>:
2006652c:	3344 1122 a55a a55a 5aa5 5aa5 2211 4433     D3".Z.Z..Z.Z."3D
2006653c:	6996 9669 9669 6996                         .ii.i..i

20066544 <at_sys_items>:
20066544:	2dc0 2006 9d0e 2005 0000 0000 0000 0000     .-. ... ........
20066554:	2dcc 2006 9d2e 2005 0000 0000 0000 0000     .-. ... ........
20066564:	2dd8 2006 a4f6 2005 0000 0000 0000 0000     .-. ... ........
20066574:	2de4 2006 9cfa 2005 0000 0000 0000 0000     .-. ... ........
20066584:	2dec 2006 a75e 2005 0000 0000 0000 0000     .-. ^.. ........
20066594:	2df4 2006 9d4e 2005 0000 0000 0000 0000     .-. N.. ........
200665a4:	2dfc 2006 9d6e 2005 0000 0000 0000 0000     .-. n.. ........
200665b4:	2e04 2006 9e08 2005 0000 0000 0000 0000     ... ... ........
200665c4:	2e0c 2006 9e98 2005 0000 0000 0000 0000     ... ... ........
200665d4:	2e14 2006 a088 2005 0000 0000 0000 0000     ... ... ........
200665e4:	2e1c 2006 a10e 2005 0000 0000 0000 0000     ... ... ........

200665f4 <impure_data>:
200665f4:	0000 0000 884c 2006 88b4 2006 891c 2006     ....L.. ... ... 
	...

20066700 <__sdata_start__>:
20066700:	0000                	c.unimp
20066702:	42c8                	c.lw	a0,4(a3)

20066704 <xISRStackTop>:
20066704:	7e60 2006                                   `~. 

20066708 <CSWTCH.248>:
20066708:	5258 5342 0044 0000                         XRBSD...

20066710 <SystemCoreClock>:
20066710:	d180 08f0                                   ....

20066714 <PXID_Idx>:
20066714:	0101 0101                                   ....

20066718 <system_can_yield>:
20066718:	0001 0000                                   ....

2006671c <wakelock>:
2006671c:	0001 0000                                   ....

20066720 <rtk_log_default_level>:
20066720:	0004 0000                                   ....

20066724 <log_init_table>:
20066724:	a788 2005                                   ... 

20066728 <mbedtls_calloc_func>:
20066728:	be0a 2005                                   ... 

2006672c <mbedtls_free_func>:
2006672c:	be0e 2005                                   ... 

20066730 <pullNextTime>:
20066730:	67f0 2006                                   .g. 

20066734 <uxTopUsedPriority>:
20066734:	000a 0000                                   ....

20066738 <crash_task_info>:
20066738:	ecd6 2005                                   ... 

2006673c <_impure_ptr>:
2006673c:	65f4 2006                                   .e. 
