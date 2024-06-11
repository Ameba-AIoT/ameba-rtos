
/home/auto_jenkins_wlan5/IOT_ATS/repos/release-master/source/amebalite_gcc_project/project_kr4/vsdk/image_mp/target_img2.axf:     file format elf32-littleriscv


Disassembly of section .xip_image2.text:

20058060 <BOOT_InitGP>:
20058060:	0000e197          	auipc	gp,0xe
20058064:	21c18193          	addi	gp,gp,540 # 2006627c <__global_pointer$>
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
2005833c:	4ea010ef          	jal	ra,20059826 <PSRAM_calibration>
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
20058a08:	53b0106f          	jal	zero,2005a742 <BOOT_Image1>

20058a0c <app_start>:
20058a0c:	1141                	c.addi	sp,-16
20058a0e:	c606                	c.swsp	ra,12(sp)
20058a10:	e50ff0ef          	jal	ra,20058060 <BOOT_InitGP>
20058a14:	00018793          	addi	a5,gp,0 # 2006627c <__global_pointer$>
20058a18:	ea418613          	addi	a2,gp,-348 # 20066120 <PrevIrqStatus>
20058a1c:	40c78633          	sub	a2,a5,a2
20058a20:	4581                	c.li	a1,0
20058a22:	ea418513          	addi	a0,gp,-348 # 20066120 <PrevIrqStatus>
20058a26:	dffb1097          	auipc	ra,0xdffb1
20058a2a:	d64080e7          	jalr	ra,-668(ra) # 978a <_memset>
20058a2e:	200687b7          	lui	a5,0x20068
20058a32:	00418613          	addi	a2,gp,4 # 20066280 <ipc_Semaphore>
20058a36:	22c78793          	addi	a5,a5,556 # 2006822c <__bss_end__>
20058a3a:	40c78633          	sub	a2,a5,a2
20058a3e:	4581                	c.li	a1,0
20058a40:	00418513          	addi	a0,gp,4 # 20066280 <ipc_Semaphore>
20058a44:	dffb1097          	auipc	ra,0xdffb1
20058a48:	d46080e7          	jalr	ra,-698(ra) # 978a <_memset>
20058a4c:	75d050ef          	jal	ra,2005e9a8 <ChipInfo_GetChipSram>
20058a50:	c919                	c.beqz	a0,20058a66 <app_start+0x5a>
20058a52:	000405b7          	lui	a1,0x40
20058a56:	20080537          	lui	a0,0x20080
20058a5a:	00058593          	addi	a1,a1,0 # 40000 <__ap_sram_heap_size>
20058a5e:	00050513          	addi	a0,a0,0 # 20080000 <__ap_sram_heap_start>
20058a62:	641010ef          	jal	ra,2005a8a2 <os_heap_add>
20058a66:	44b020ef          	jal	ra,2005b6b0 <rtos_mem_init>
20058a6a:	2005c7b7          	lui	a5,0x2005c
20058a6e:	ca078793          	addi	a5,a5,-864 # 2005bca0 <freertos_risc_v_trap_handler>
20058a72:	9bf1                	c.andi	a5,-4
20058a74:	30579073          	csrrw	zero,mtvec,a5
20058a78:	2831                	c.jal	20058a94 <SystemCoreClockUpdate>
20058a7a:	dffae097          	auipc	ra,0xdffae
20058a7e:	24c080e7          	jalr	ra,588(ra) # 6cc6 <SYSTIMER_Init>
20058a82:	2db000ef          	jal	ra,2005955c <pinmap_init>
20058a86:	353080ef          	jal	ra,200615d8 <__libc_init_array>
20058a8a:	40b2                	c.lwsp	ra,12(sp)
20058a8c:	0141                	c.addi	sp,16
20058a8e:	0690206f          	jal	zero,2005b2f6 <main>

20058a92 <_init>:
20058a92:	8082                	c.jr	ra

20058a94 <SystemCoreClockUpdate>:
20058a94:	1141                	c.addi	sp,-16
20058a96:	c606                	c.swsp	ra,12(sp)
20058a98:	dffa8097          	auipc	ra,0xdffa8
20058a9c:	07e080e7          	jalr	ra,126(ra) # b16 <CPU_ClkGet>
20058aa0:	40b2                	c.lwsp	ra,12(sp)
20058aa2:	e6a1aa23          	sw	a0,-396(gp) # 200660f0 <SystemCoreClock>
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
20058ae8:	36a0d0ef          	jal	ra,20065e52 <plic_get_active_irq_id>
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
20058b04:	c2418693          	addi	a3,gp,-988 # 20065ea0 <Flash_Layout>
20058b08:	4701                	c.li	a4,0
20058b0a:	c2418793          	addi	a5,gp,-988 # 20065ea0 <Flash_Layout>
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
20058b4a:	eaa1a223          	sw	a0,-348(gp) # 20066120 <PrevIrqStatus>
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
20058b6a:	ea41a503          	lw	a0,-348(gp) # 20066120 <PrevIrqStatus>
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
20058b8c:	9d450513          	addi	a0,a0,-1580 # 200619d4 <strtol+0xe>
20058b90:	25f060ef          	jal	ra,2005f5ee <io_assert_failed>
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
20058c00:	6930c0ef          	jal	ra,20065a92 <FLASH_TxData>
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
20058cb0:	7dd0c0ef          	jal	ra,20065c8c <FLASH_Erase>
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
20058d84:	580d8793          	addi	a5,s11,1408 # 20064580 <ipc_tickless_table>
20058d88:	62848493          	addi	s1,s1,1576 # 20064628 <lib_pmc_git_rev>
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
20058db0:	580d8d93          	addi	s11,s11,1408
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
20058e56:	a7cb8693          	addi	a3,s7,-1412 # 20061a7c <strtol+0xb6>
20058e5a:	04900613          	addi	a2,zero,73
20058e5e:	8319                	c.srli	a4,0x6
20058e60:	8b0d                	c.andi	a4,3
20058e62:	ac4c0593          	addi	a1,s8,-1340 # 20061ac4 <strtol+0xfe>
20058e66:	4511                	c.li	a0,4
20058e68:	050020ef          	jal	ra,2005aeb8 <rtk_log_write>
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
20058ecc:	00418613          	addi	a2,gp,4 # 20066280 <ipc_Semaphore>
20058ed0:	9432                	c.add	s0,a2
20058ed2:	4008                	c.lw	a0,0(s0)
20058ed4:	c511                	c.beqz	a0,20058ee0 <IPC_TXHandler+0x34>
20058ed6:	4422                	c.lwsp	s0,8(sp)
20058ed8:	40b2                	c.lwsp	ra,12(sp)
20058eda:	0141                	c.addi	sp,16
20058edc:	2530206f          	jal	zero,2005b92e <rtos_sema_give>
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
20058f0c:	ac850513          	addi	a0,a0,-1336 # 20061ac8 <strtol+0x102>
20058f10:	c606                	c.swsp	ra,12(sp)
20058f12:	1ec060ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
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
20058f38:	0c418713          	addi	a4,gp,196 # 20066340 <IPC_IrqHandler>
20058f3c:	974e                	c.add	a4,s3
20058f3e:	431c                	c.lw	a5,0(a4)
20058f40:	c79d                	c.beqz	a5,20058f6e <IPC_wait_idle+0x50>
20058f42:	00418513          	addi	a0,gp,4 # 20066280 <ipc_Semaphore>
20058f46:	954e                	c.add	a0,s3
20058f48:	4118                	c.lw	a4,0(a0)
20058f4a:	00418413          	addi	s0,gp,4 # 20066280 <ipc_Semaphore>
20058f4e:	e319                	c.bnez	a4,20058f54 <IPC_wait_idle+0x36>
20058f50:	153020ef          	jal	ra,2005b8a2 <rtos_sema_create_binary>
20058f54:	0ff97a13          	andi	s4,s2,255
20058f58:	85d2                	c.mv	a1,s4
20058f5a:	8526                	c.mv	a0,s1
20058f5c:	4605                	c.li	a2,1
20058f5e:	2401                	c.jal	2005915e <IPC_INTConfig>
20058f60:	013407b3          	add	a5,s0,s3
20058f64:	4388                	c.lw	a0,0(a5)
20058f66:	55fd                	c.li	a1,-1
20058f68:	17b020ef          	jal	ra,2005b8e2 <rtos_sema_take>
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
20058f86:	ae050513          	addi	a0,a0,-1312 # 20061ae0 <strtol+0x11a>
20058f8a:	174060ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
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
20058fd6:	b0050513          	addi	a0,a0,-1280 # 20061b00 <strtol+0x13a>
20058fda:	614060ef          	jal	ra,2005f5ee <io_assert_failed>
20058fde:	479d                	c.li	a5,7
20058fe0:	0157fa63          	bgeu	a5,s5,20058ff4 <ipc_send_message+0x4c>
20058fe4:	20062537          	lui	a0,0x20062
20058fe8:	0ad00593          	addi	a1,zero,173
20058fec:	b0050513          	addi	a0,a0,-1280 # 20061b00 <strtol+0x13a>
20058ff0:	5fe060ef          	jal	ra,2005f5ee <io_assert_failed>
20058ff4:	02100793          	addi	a5,zero,33
20058ff8:	0d37eb63          	bltu	a5,s3,200590ce <ipc_send_message+0x126>
20058ffc:	200627b7          	lui	a5,0x20062
20059000:	bd478793          	addi	a5,a5,-1068 # 20061bd4 <CSWTCH.29>
20059004:	97ce                	c.add	a5,s3
20059006:	20062737          	lui	a4,0x20062
2005900a:	0007c583          	lbu	a1,0(a5)
2005900e:	bf870713          	addi	a4,a4,-1032 # 20061bf8 <CSWTCH.30>
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
2005903a:	ba850513          	addi	a0,a0,-1112 # 20061ba8 <strtol+0x1e2>
2005903e:	0c0060ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
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
20059198:	04418913          	addi	s2,gp,68 # 200662c0 <IPC_IrqData>
2005919c:	0c418493          	addi	s1,gp,196 # 20066340 <IPC_IrqHandler>
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
200591ec:	0c418713          	addi	a4,gp,196 # 20066340 <IPC_IrqHandler>
200591f0:	883e                	c.mv	a6,a5
200591f2:	973e                	c.add	a4,a5
200591f4:	04418793          	addi	a5,gp,68 # 200662c0 <IPC_IrqData>
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
2005922a:	e7818793          	addi	a5,gp,-392 # 200660f4 <PXID_Idx>
2005922e:	97aa                	c.add	a5,a0
20059230:	0007c783          	lbu	a5,0(a5)
20059234:	892a                	c.mv	s2,a0
20059236:	842e                	c.mv	s0,a1
20059238:	e7818493          	addi	s1,gp,-392 # 200660f4 <PXID_Idx>
2005923c:	eb89                	c.bnez	a5,2005924e <IPC_SEMTake+0x3c>
2005923e:	20062537          	lui	a0,0x20062
20059242:	0bf00593          	addi	a1,zero,191
20059246:	c8050513          	addi	a0,a0,-896 # 20061c80 <CSWTCH.30+0x88>
2005924a:	3a4060ef          	jal	ra,2005f5ee <io_assert_failed>
2005924e:	00291793          	slli	a5,s2,0x2
20059252:	410209b7          	lui	s3,0x41020
20059256:	99be                	c.add	s3,a5
20059258:	0149a783          	lw	a5,20(s3) # 41020014 <__km4_bd_boot_download_addr__+0x1100e014>
2005925c:	83a1                	c.srli	a5,0x8
2005925e:	8b8d                	c.andi	a5,3
20059260:	cb89                	c.beqz	a5,20059272 <IPC_SEMTake+0x60>
20059262:	c42d                	c.beqz	s0,200592cc <IPC_SEMTake+0xba>
20059264:	ea81a783          	lw	a5,-344(gp) # 20066124 <ipc_delay>
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
200592bc:	d2850513          	addi	a0,a0,-728 # 20061d28 <CSWTCH.30+0x130>
200592c0:	63f050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
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
2005932a:	e7818693          	addi	a3,gp,-392 # 200660f4 <PXID_Idx>
2005932e:	c606                	c.swsp	ra,12(sp)
20059330:	96b2                	c.add	a3,a2
20059332:	0006c683          	lbu	a3,0(a3) # 41020000 <__km4_bd_boot_download_addr__+0x1100e000>
20059336:	20062537          	lui	a0,0x20062
2005933a:	0ff7f793          	andi	a5,a5,255
2005933e:	177d                	c.addi	a4,-1
20059340:	d7450513          	addi	a0,a0,-652 # 20061d74 <CSWTCH.30+0x17c>
20059344:	5bb050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
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
20059360:	eaa1a423          	sw	a0,-344(gp) # 20066124 <ipc_delay>
20059364:	8082                	c.jr	ra

20059366 <IPC_GetDevById>:
20059366:	4789                	c.li	a5,2
20059368:	02a7f163          	bgeu	a5,a0,2005938a <IPC_GetDevById+0x24>
2005936c:	20062537          	lui	a0,0x20062
20059370:	1141                	c.addi	sp,-16
20059372:	c8050513          	addi	a0,a0,-896 # 20061c80 <CSWTCH.30+0x88>
20059376:	11b00593          	addi	a1,zero,283
2005937a:	c606                	c.swsp	ra,12(sp)
2005937c:	272060ef          	jal	ra,2005f5ee <io_assert_failed>
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
200593e6:	14418793          	addi	a5,gp,324 # 200663c0 <IMG_ADDR>
200593ea:	97ce                	c.add	a5,s3
200593ec:	439c                	c.lw	a5,0(a5)
200593ee:	892a                	c.mv	s2,a0
200593f0:	14418493          	addi	s1,gp,324 # 200663c0 <IMG_ADDR>
200593f4:	e7a9                	c.bnez	a5,2005943e <ota_get_cur_index+0x7c>
200593f6:	4601                	c.li	a2,0
200593f8:	14418593          	addi	a1,gp,324 # 200663c0 <IMG_ADDR>
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
200594fe:	ddc50513          	addi	a0,a0,-548 # 20061ddc <CSWTCH.30+0x1e4>
20059502:	3fd050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
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
20059526:	dc0a0513          	addi	a0,s4,-576 # 20061dc0 <CSWTCH.30+0x1c8>
2005952a:	3d5050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
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
20059584:	df048493          	addi	s1,s1,-528 # 20061df0 <pmap_func>
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
20059740:	ef050513          	addi	a0,a0,-272 # 20061ef0 <pmap_func+0x100>
20059744:	6ab050ef          	jal	ra,2005f5ee <io_assert_failed>
20059748:	00091b63          	bne	s2,zero,2005975e <pmu_register_sleep_callback+0x44>
2005974c:	e889                	c.bnez	s1,2005975e <pmu_register_sleep_callback+0x44>
2005974e:	20062537          	lui	a0,0x20062
20059752:	07400593          	addi	a1,zero,116
20059756:	ef050513          	addi	a0,a0,-272 # 20061ef0 <pmap_func+0x100>
2005975a:	695050ef          	jal	ra,2005f5ee <io_assert_failed>
2005975e:	47d1                	c.li	a5,20
20059760:	02f40733          	mul	a4,s0,a5
20059764:	15c18793          	addi	a5,gp,348 # 200663d8 <gPsmDdHookInfo>
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
2005978a:	e7c1a503          	lw	a0,-388(gp) # 200660f8 <system_can_yield>
2005978e:	8082                	c.jr	ra

20059790 <pmu_ready_to_sleep>:
20059790:	1141                	c.addi	sp,-16
20059792:	c606                	c.swsp	ra,12(sp)
20059794:	42a020ef          	jal	ra,2005bbbe <rtos_time_get_current_system_time_ms>
20059798:	eb01a703          	lw	a4,-336(gp) # 2006612c <sleepwakelock_timeout>
2005979c:	800006b7          	lui	a3,0x80000
200597a0:	87aa                	c.mv	a5,a0
200597a2:	ffe6c693          	xori	a3,a3,-2
200597a6:	00a77e63          	bgeu	a4,a0,200597c2 <pmu_ready_to_sleep+0x32>
200597aa:	40e507b3          	sub	a5,a0,a4
200597ae:	4501                	c.li	a0,0
200597b0:	02f6e063          	bltu	a3,a5,200597d0 <pmu_ready_to_sleep+0x40>
200597b4:	3eed                	c.jal	200593ae <dsp_status_on>
200597b6:	e801a783          	lw	a5,-384(gp) # 200660fc <wakelock>
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
200597e2:	e8018713          	addi	a4,gp,-384 # 200660fc <wakelock>
200597e6:	4314                	c.lw	a3,0(a4)
200597e8:	4785                	c.li	a5,1
200597ea:	008797b3          	sll	a5,a5,s0
200597ee:	8fd5                	c.or	a5,a3
200597f0:	c31c                	c.sw	a5,0(a4)
200597f2:	4422                	c.lwsp	s0,8(sp)
200597f4:	40b2                	c.lwsp	ra,12(sp)
200597f6:	0141                	c.addi	sp,16
200597f8:	b06ff06f          	jal	zero,20058afe <irq_enable_restore>

200597fc <pmu_release_wakelock>:
200597fc:	1141                	c.addi	sp,-16
200597fe:	c422                	c.swsp	s0,8(sp)
20059800:	c606                	c.swsp	ra,12(sp)
20059802:	842a                	c.mv	s0,a0
20059804:	af2ff0ef          	jal	ra,20058af6 <irq_disable_save>
20059808:	e8018713          	addi	a4,gp,-384 # 200660fc <wakelock>
2005980c:	4314                	c.lw	a3,0(a4)
2005980e:	4785                	c.li	a5,1
20059810:	008797b3          	sll	a5,a5,s0
20059814:	fff7c793          	xori	a5,a5,-1
20059818:	8ff5                	c.and	a5,a3
2005981a:	c31c                	c.sw	a5,0(a4)
2005981c:	4422                	c.lwsp	s0,8(sp)
2005981e:	40b2                	c.lwsp	ra,12(sp)
20059820:	0141                	c.addi	sp,16
20059822:	adcff06f          	jal	zero,20058afe <irq_enable_restore>

20059826 <PSRAM_calibration>:
20059826:	7119                	c.addi16sp	sp,-128
20059828:	de86                	c.swsp	ra,124(sp)
2005982a:	dca6                	c.swsp	s1,120(sp)
2005982c:	daca                	c.swsp	s2,116(sp)
2005982e:	d8ce                	c.swsp	s3,112(sp)
20059830:	d6d2                	c.swsp	s4,108(sp)
20059832:	d4d6                	c.swsp	s5,104(sp)
20059834:	d2da                	c.swsp	s6,100(sp)
20059836:	d0de                	c.swsp	s7,96(sp)
20059838:	cee2                	c.swsp	s8,92(sp)
2005983a:	cce6                	c.swsp	s9,88(sp)
2005983c:	caea                	c.swsp	s10,84(sp)
2005983e:	c8ee                	c.swsp	s11,80(sp)
20059840:	cc2a                	c.swsp	a0,24(sp)
20059842:	410087b7          	lui	a5,0x41008
20059846:	43d8                	c.lw	a4,4(a5)
20059848:	100007b7          	lui	a5,0x10000
2005984c:	1781                	c.addi	a5,-32
2005984e:	8ff9                	c.and	a5,a4
20059850:	d43e                	c.swsp	a5,40(sp)
20059852:	dc02                	c.swsp	zero,56(sp)
20059854:	de02                	c.swsp	zero,60(sp)
20059856:	c082                	c.swsp	zero,64(sp)
20059858:	c282                	c.swsp	zero,68(sp)
2005985a:	c482                	c.swsp	zero,72(sp)
2005985c:	c682                	c.swsp	zero,76(sp)
2005985e:	7c1027f3          	csrrs	a5,0x7c1,zero
20059862:	1007f793          	andi	a5,a5,256
20059866:	c385                	c.beqz	a5,20059886 <PSRAM_calibration+0x60>
20059868:	7c0467f3          	csrrsi	a5,0x7c0,8
2005986c:	0ff0000f          	fence	iorw,iorw
20059870:	0001                	c.addi	zero,0
20059872:	0001                	c.addi	zero,0
20059874:	0001                	c.addi	zero,0
20059876:	0001                	c.addi	zero,0
20059878:	0001                	c.addi	zero,0
2005987a:	0001                	c.addi	zero,0
2005987c:	0001                	c.addi	zero,0
2005987e:	0001                	c.addi	zero,0
20059880:	0001                	c.addi	zero,0
20059882:	0001                	c.addi	zero,0
20059884:	0001                	c.addi	zero,0
20059886:	10000793          	addi	a5,zero,256
2005988a:	7c17b7f3          	csrrc	a5,0x7c1,a5
2005988e:	0ff0000f          	fence	iorw,iorw
20059892:	0001                	c.addi	zero,0
20059894:	0001                	c.addi	zero,0
20059896:	0001                	c.addi	zero,0
20059898:	0001                	c.addi	zero,0
2005989a:	0001                	c.addi	zero,0
2005989c:	0001                	c.addi	zero,0
2005989e:	0001                	c.addi	zero,0
200598a0:	0001                	c.addi	zero,0
200598a2:	0001                	c.addi	zero,0
200598a4:	0001                	c.addi	zero,0
200598a6:	0001                	c.addi	zero,0
200598a8:	7c0467f3          	csrrsi	a5,0x7c0,8
200598ac:	0ff0000f          	fence	iorw,iorw
200598b0:	0001                	c.addi	zero,0
200598b2:	0001                	c.addi	zero,0
200598b4:	0001                	c.addi	zero,0
200598b6:	0001                	c.addi	zero,0
200598b8:	0001                	c.addi	zero,0
200598ba:	0001                	c.addi	zero,0
200598bc:	0001                	c.addi	zero,0
200598be:	0001                	c.addi	zero,0
200598c0:	0001                	c.addi	zero,0
200598c2:	0001                	c.addi	zero,0
200598c4:	0001                	c.addi	zero,0
200598c6:	41008737          	lui	a4,0x41008
200598ca:	431c                	c.lw	a5,0(a4)
200598cc:	20062c37          	lui	s8,0x20062
200598d0:	60000d37          	lui	s10,0x60000
200598d4:	9bf9                	c.andi	a5,-2
200598d6:	c31c                	c.sw	a5,0(a4)
200598d8:	4791                	c.li	a5,4
200598da:	ca3e                	c.swsp	a5,20(sp)
200598dc:	000d0b13          	addi	s6,s10,0 # 60000000 <__km4_bd_psram_start__>
200598e0:	f88c0793          	addi	a5,s8,-120 # 20061f88 <pmap_func+0x198>
200598e4:	00050cb7          	lui	s9,0x50
200598e8:	d03e                	c.swsp	a5,32(sp)
200598ea:	019b07b3          	add	a5,s6,s9
200598ee:	597d                	c.li	s2,-1
200598f0:	4981                	c.li	s3,0
200598f2:	5afd                	c.li	s5,-1
200598f4:	54fd                	c.li	s1,-1
200598f6:	4a05                	c.li	s4,1
200598f8:	c9018b93          	addi	s7,gp,-880 # 20065f0c <PSRAM_CALIB_PATTERN>
200598fc:	ce3e                	c.swsp	a5,28(sp)
200598fe:	47e2                	c.lwsp	a5,24(sp)
20059900:	cf91                	c.beqz	a5,2005991c <PSRAM_calibration+0xf6>
20059902:	5782                	c.lwsp	a5,32(sp)
20059904:	200625b7          	lui	a1,0x20062
20059908:	8752                	c.mv	a4,s4
2005990a:	20078693          	addi	a3,a5,512 # 10000200 <__km4_boot_text_start__+0x8001e0>
2005990e:	04900613          	addi	a2,zero,73
20059912:	f9c58593          	addi	a1,a1,-100 # 20061f9c <pmap_func+0x1ac>
20059916:	4511                	c.li	a0,4
20059918:	5a0010ef          	jal	ra,2005aeb8 <rtk_log_write>
2005991c:	57a2                	c.lwsp	a5,40(sp)
2005991e:	01ca1d93          	slli	s11,s4,0x1c
20059922:	5ffd                	c.li	t6,-1
20059924:	00fde7b3          	or	a5,s11,a5
20059928:	d23e                	c.swsp	a5,36(sp)
2005992a:	4c01                	c.li	s8,0
2005992c:	5cfd                	c.li	s9,-1
2005992e:	4d81                	c.li	s11,0
20059930:	5792                	c.lwsp	a5,36(sp)
20059932:	d67e                	c.swsp	t6,44(sp)
20059934:	410086b7          	lui	a3,0x41008
20059938:	01b7e7b3          	or	a5,a5,s11
2005993c:	c2dc                	c.sw	a5,4(a3)
2005993e:	000ba783          	lw	a5,0(s7)
20059942:	000d4603          	lbu	a2,0(s10)
20059946:	4772                	c.lwsp	a4,28(sp)
20059948:	0ff7f693          	andi	a3,a5,255
2005994c:	00dd0023          	sb	a3,0(s10)
20059950:	0087d693          	srli	a3,a5,0x8
20059954:	0ff6f693          	andi	a3,a3,255
20059958:	001d4603          	lbu	a2,1(s10)
2005995c:	00dd00a3          	sb	a3,1(s10)
20059960:	0107d693          	srli	a3,a5,0x10
20059964:	0ff6f693          	andi	a3,a3,255
20059968:	002d4603          	lbu	a2,2(s10)
2005996c:	83e1                	c.srli	a5,0x18
2005996e:	00dd0123          	sb	a3,2(s10)
20059972:	003d4683          	lbu	a3,3(s10)
20059976:	00fd01a3          	sb	a5,3(s10)
2005997a:	004ba783          	lw	a5,4(s7)
2005997e:	00100837          	lui	a6,0x100
20059982:	985a                	c.add	a6,s6
20059984:	c31c                	c.sw	a5,0(a4)
20059986:	008ba783          	lw	a5,8(s7)
2005998a:	00150537          	lui	a0,0x150
2005998e:	955a                	c.add	a0,s6
20059990:	00f82023          	sw	a5,0(a6) # 100000 <__ap_sram_heap_size+0xc0000>
20059994:	00cba783          	lw	a5,12(s7)
20059998:	002005b7          	lui	a1,0x200
2005999c:	95da                	c.add	a1,s6
2005999e:	c11c                	c.sw	a5,0(a0)
200599a0:	010ba783          	lw	a5,16(s7)
200599a4:	00250637          	lui	a2,0x250
200599a8:	965a                	c.add	a2,s6
200599aa:	c19c                	c.sw	a5,0(a1)
200599ac:	014ba783          	lw	a5,20(s7)
200599b0:	c21c                	c.sw	a5,0(a2)
200599b2:	000d4683          	lbu	a3,0(s10)
200599b6:	001d4783          	lbu	a5,1(s10)
200599ba:	0ff6f693          	andi	a3,a3,255
200599be:	0ff7f793          	andi	a5,a5,255
200599c2:	07a2                	c.slli	a5,0x8
200599c4:	8edd                	c.or	a3,a5
200599c6:	002d4783          	lbu	a5,2(s10)
200599ca:	0ff7f793          	andi	a5,a5,255
200599ce:	07c2                	c.slli	a5,0x10
200599d0:	8fd5                	c.or	a5,a3
200599d2:	003d4683          	lbu	a3,3(s10)
200599d6:	06e2                	c.slli	a3,0x18
200599d8:	8fd5                	c.or	a5,a3
200599da:	dc3e                	c.swsp	a5,56(sp)
200599dc:	431c                	c.lw	a5,0(a4)
200599de:	de3e                	c.swsp	a5,60(sp)
200599e0:	00082783          	lw	a5,0(a6)
200599e4:	c0be                	c.swsp	a5,64(sp)
200599e6:	411c                	c.lw	a5,0(a0)
200599e8:	1828                	c.addi4spn	a0,sp,56
200599ea:	c2be                	c.swsp	a5,68(sp)
200599ec:	419c                	c.lw	a5,0(a1)
200599ee:	85de                	c.mv	a1,s7
200599f0:	c4be                	c.swsp	a5,72(sp)
200599f2:	421c                	c.lw	a5,0(a2)
200599f4:	4661                	c.li	a2,24
200599f6:	c6be                	c.swsp	a5,76(sp)
200599f8:	dffb0097          	auipc	ra,0xdffb0
200599fc:	bc8080e7          	jalr	ra,-1080(ra) # 95c0 <_memcmp>
20059a00:	5fb2                	c.lwsp	t6,44(sp)
20059a02:	47e2                	c.lwsp	a5,24(sp)
20059a04:	14051563          	bne	a0,zero,20059b4e <PSRAM_calibration+0x328>
20059a08:	c79d                	c.beqz	a5,20059a36 <PSRAM_calibration+0x210>
20059a0a:	47b6                	c.lwsp	a5,76(sp)
20059a0c:	5682                	c.lwsp	a3,32(sp)
20059a0e:	4886                	c.lwsp	a7,64(sp)
20059a10:	c43e                	c.swsp	a5,8(sp)
20059a12:	47a6                	c.lwsp	a5,72(sp)
20059a14:	5872                	c.lwsp	a6,60(sp)
20059a16:	200625b7          	lui	a1,0x20062
20059a1a:	c23e                	c.swsp	a5,4(sp)
20059a1c:	4796                	c.lwsp	a5,68(sp)
20059a1e:	876e                	c.mv	a4,s11
20059a20:	21468693          	addi	a3,a3,532 # 41008214 <__km4_bd_boot_download_addr__+0x10ff6214>
20059a24:	c03e                	c.swsp	a5,0(sp)
20059a26:	57e2                	c.lwsp	a5,56(sp)
20059a28:	04900613          	addi	a2,zero,73
20059a2c:	f9c58593          	addi	a1,a1,-100 # 20061f9c <pmap_func+0x1ac>
20059a30:	4511                	c.li	a0,4
20059a32:	486010ef          	jal	ra,2005aeb8 <rtk_log_write>
20059a36:	57fd                	c.li	a5,-1
20059a38:	00fc9363          	bne	s9,a5,20059a3e <PSRAM_calibration+0x218>
20059a3c:	8cee                	c.mv	s9,s11
20059a3e:	47fd                	c.li	a5,31
20059a40:	018c8fb3          	add	t6,s9,s8
20059a44:	0c05                	c.addi	s8,1
20059a46:	00fd9863          	bne	s11,a5,20059a56 <PSRAM_calibration+0x230>
20059a4a:	0189d663          	bge	s3,s8,20059a56 <PSRAM_calibration+0x230>
20059a4e:	8952                	c.mv	s2,s4
20059a50:	89e2                	c.mv	s3,s8
20059a52:	8afe                	c.mv	s5,t6
20059a54:	84e6                	c.mv	s1,s9
20059a56:	0d85                	c.addi	s11,1
20059a58:	02000793          	addi	a5,zero,32
20059a5c:	ecfd9ae3          	bne	s11,a5,20059930 <PSRAM_calibration+0x10a>
20059a60:	47d2                	c.lwsp	a5,20(sp)
20059a62:	0a06                	c.slli	s4,0x1
20059a64:	17fd                	c.addi	a5,-1
20059a66:	ca3e                	c.swsp	a5,20(sp)
20059a68:	e8079be3          	bne	a5,zero,200598fe <PSRAM_calibration+0xd8>
20059a6c:	200626b7          	lui	a3,0x20062
20059a70:	200625b7          	lui	a1,0x20062
20059a74:	88ca                	c.mv	a7,s2
20059a76:	884e                	c.mv	a6,s3
20059a78:	87d6                	c.mv	a5,s5
20059a7a:	8726                	c.mv	a4,s1
20059a7c:	1e068693          	addi	a3,a3,480 # 200621e0 <pmap_func+0x3f0>
20059a80:	04900613          	addi	a2,zero,73
20059a84:	f9c58593          	addi	a1,a1,-100 # 20061f9c <pmap_func+0x1ac>
20059a88:	4511                	c.li	a0,4
20059a8a:	42e010ef          	jal	ra,2005aeb8 <rtk_log_write>
20059a8e:	7c1027f3          	csrrs	a5,0x7c1,zero
20059a92:	1007f793          	andi	a5,a5,256
20059a96:	e7b1                	c.bnez	a5,20059ae2 <PSRAM_calibration+0x2bc>
20059a98:	7c1027f3          	csrrs	a5,0x7c1,zero
20059a9c:	1007f793          	andi	a5,a5,256
20059aa0:	e3a9                	c.bnez	a5,20059ae2 <PSRAM_calibration+0x2bc>
20059aa2:	7c0167f3          	csrrsi	a5,0x7c0,2
20059aa6:	0ff0000f          	fence	iorw,iorw
20059aaa:	0001                	c.addi	zero,0
20059aac:	0001                	c.addi	zero,0
20059aae:	0001                	c.addi	zero,0
20059ab0:	0001                	c.addi	zero,0
20059ab2:	0001                	c.addi	zero,0
20059ab4:	0001                	c.addi	zero,0
20059ab6:	0001                	c.addi	zero,0
20059ab8:	0001                	c.addi	zero,0
20059aba:	0001                	c.addi	zero,0
20059abc:	0001                	c.addi	zero,0
20059abe:	0001                	c.addi	zero,0
20059ac0:	10000793          	addi	a5,zero,256
20059ac4:	7c17a7f3          	csrrs	a5,0x7c1,a5
20059ac8:	0ff0000f          	fence	iorw,iorw
20059acc:	0001                	c.addi	zero,0
20059ace:	0001                	c.addi	zero,0
20059ad0:	0001                	c.addi	zero,0
20059ad2:	0001                	c.addi	zero,0
20059ad4:	0001                	c.addi	zero,0
20059ad6:	0001                	c.addi	zero,0
20059ad8:	0001                	c.addi	zero,0
20059ada:	0001                	c.addi	zero,0
20059adc:	0001                	c.addi	zero,0
20059ade:	0001                	c.addi	zero,0
20059ae0:	0001                	c.addi	zero,0
20059ae2:	47a1                	c.li	a5,8
20059ae4:	4501                	c.li	a0,0
20059ae6:	0537d663          	bge	a5,s3,20059b32 <PSRAM_calibration+0x30c>
20059aea:	409a8733          	sub	a4,s5,s1
20059aee:	4609                	c.li	a2,2
20059af0:	02c74733          	div	a4,a4,a2
20059af4:	94d6                	c.add	s1,s5
20059af6:	6685                	c.lui	a3,0x1
20059af8:	f0068693          	addi	a3,a3,-256 # f00 <CPU_ClkGet+0x3ea>
20059afc:	0972                	c.slli	s2,0x1c
20059afe:	4505                	c.li	a0,1
20059b00:	02c4c4b3          	div	s1,s1,a2
20059b04:	1779                	c.addi	a4,-2
20059b06:	00871793          	slli	a5,a4,0x8
20059b0a:	8ff5                	c.and	a5,a3
20059b0c:	0127e7b3          	or	a5,a5,s2
20059b10:	0742                	c.slli	a4,0x10
20059b12:	000f06b7          	lui	a3,0xf0
20059b16:	8f75                	c.and	a4,a3
20059b18:	88fd                	c.andi	s1,31
20059b1a:	8fc5                	c.or	a5,s1
20059b1c:	8fd9                	c.or	a5,a4
20059b1e:	02000737          	lui	a4,0x2000
20059b22:	8fd9                	c.or	a5,a4
20059b24:	41008737          	lui	a4,0x41008
20059b28:	c35c                	c.sw	a5,4(a4)
20059b2a:	431c                	c.lw	a5,0(a4)
20059b2c:	0017e793          	ori	a5,a5,1
20059b30:	c31c                	c.sw	a5,0(a4)
20059b32:	50f6                	c.lwsp	ra,124(sp)
20059b34:	54e6                	c.lwsp	s1,120(sp)
20059b36:	5956                	c.lwsp	s2,116(sp)
20059b38:	59c6                	c.lwsp	s3,112(sp)
20059b3a:	5a36                	c.lwsp	s4,108(sp)
20059b3c:	5aa6                	c.lwsp	s5,104(sp)
20059b3e:	5b16                	c.lwsp	s6,100(sp)
20059b40:	5b86                	c.lwsp	s7,96(sp)
20059b42:	4c76                	c.lwsp	s8,92(sp)
20059b44:	4ce6                	c.lwsp	s9,88(sp)
20059b46:	4d56                	c.lwsp	s10,84(sp)
20059b48:	4dc6                	c.lwsp	s11,80(sp)
20059b4a:	6109                	c.addi16sp	sp,128
20059b4c:	8082                	c.jr	ra
20059b4e:	cb95                	c.beqz	a5,20059b82 <PSRAM_calibration+0x35c>
20059b50:	47b6                	c.lwsp	a5,76(sp)
20059b52:	4886                	c.lwsp	a7,64(sp)
20059b54:	5872                	c.lwsp	a6,60(sp)
20059b56:	c43e                	c.swsp	a5,8(sp)
20059b58:	47a6                	c.lwsp	a5,72(sp)
20059b5a:	200626b7          	lui	a3,0x20062
20059b5e:	200625b7          	lui	a1,0x20062
20059b62:	c23e                	c.swsp	a5,4(sp)
20059b64:	4796                	c.lwsp	a5,68(sp)
20059b66:	876e                	c.mv	a4,s11
20059b68:	1bc68693          	addi	a3,a3,444 # 200621bc <pmap_func+0x3cc>
20059b6c:	c03e                	c.swsp	a5,0(sp)
20059b6e:	57e2                	c.lwsp	a5,56(sp)
20059b70:	04900613          	addi	a2,zero,73
20059b74:	f9c58593          	addi	a1,a1,-100 # 20061f9c <pmap_func+0x1ac>
20059b78:	4511                	c.li	a0,4
20059b7a:	d67e                	c.swsp	t6,44(sp)
20059b7c:	33c010ef          	jal	ra,2005aeb8 <rtk_log_write>
20059b80:	5fb2                	c.lwsp	t6,44(sp)
20059b82:	57fd                	c.li	a5,-1
20059b84:	ecfc89e3          	beq	s9,a5,20059a56 <PSRAM_calibration+0x230>
20059b88:	0189d663          	bge	s3,s8,20059b94 <PSRAM_calibration+0x36e>
20059b8c:	8952                	c.mv	s2,s4
20059b8e:	89e2                	c.mv	s3,s8
20059b90:	8afe                	c.mv	s5,t6
20059b92:	84e6                	c.mv	s1,s9
20059b94:	5ffd                	c.li	t6,-1
20059b96:	4c01                	c.li	s8,0
20059b98:	5cfd                	c.li	s9,-1
20059b9a:	bd75                	c.j	20059a56 <PSRAM_calibration+0x230>

20059b9c <System_Reset>:
20059b9c:	4100c7b7          	lui	a5,0x4100c
20059ba0:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
20059ba4:	4709                	c.li	a4,2
20059ba6:	8399                	c.srli	a5,0x6
20059ba8:	8b8d                	c.andi	a5,3
20059baa:	00e78963          	beq	a5,a4,20059bbc <System_Reset+0x20>
20059bae:	4705                	c.li	a4,1
20059bb0:	02e78963          	beq	a5,a4,20059be2 <System_Reset+0x46>
20059bb4:	40000737          	lui	a4,0x40000
20059bb8:	c781                	c.beqz	a5,20059bc0 <System_Reset+0x24>
20059bba:	8082                	c.jr	ra
20059bbc:	20000737          	lui	a4,0x20000
20059bc0:	969696b7          	lui	a3,0x96969
20059bc4:	4100c7b7          	lui	a5,0x4100c
20059bc8:	69668693          	addi	a3,a3,1686 # 96969696 <__ctrace_end__+0xcc969696>
20059bcc:	26d7a223          	sw	a3,612(a5) # 4100c264 <__km4_bd_boot_download_addr__+0x10ffa264>
20059bd0:	26e7a023          	sw	a4,608(a5)
20059bd4:	69697737          	lui	a4,0x69697
20059bd8:	96970713          	addi	a4,a4,-1687 # 69696969 <__kr4_psram_text_start__+0x9516949>
20059bdc:	26e7a223          	sw	a4,612(a5)
20059be0:	bfe9                	c.j	20059bba <System_Reset+0x1e>
20059be2:	80000737          	lui	a4,0x80000
20059be6:	bfe9                	c.j	20059bc0 <System_Reset+0x24>

20059be8 <SWR_CORE_Vol_Get>:
20059be8:	4100d7b7          	lui	a5,0x4100d
20059bec:	f3e7c503          	lbu	a0,-194(a5) # 4100cf3e <__km4_bd_boot_download_addr__+0x10ffaf3e>
20059bf0:	0ff57513          	andi	a0,a0,255
20059bf4:	8082                	c.jr	ra

20059bf6 <wifi_set_user_config>:
20059bf6:	1141                	c.addi	sp,-16
20059bf8:	c422                	c.swsp	s0,8(sp)
20059bfa:	03c00613          	addi	a2,zero,60
20059bfe:	40418413          	addi	s0,gp,1028 # 20066680 <wifi_user_config>
20059c02:	4581                	c.li	a1,0
20059c04:	40418513          	addi	a0,gp,1028 # 20066680 <wifi_user_config>
20059c08:	c606                	c.swsp	ra,12(sp)
20059c0a:	dffb0097          	auipc	ra,0xdffb0
20059c0e:	ec4080e7          	jalr	ra,-316(ra) # 9ace <__wrap_memset>
20059c12:	50800793          	addi	a5,zero,1288
20059c16:	00f41f23          	sh	a5,30(s0)
20059c1a:	4799                	c.li	a5,6
20059c1c:	d01c                	c.sw	a5,32(s0)
20059c1e:	47a1                	c.li	a5,8
20059c20:	d05c                	c.sw	a5,36(s0)
20059c22:	4785                	c.li	a5,1
20059c24:	00f405a3          	sb	a5,11(s0)
20059c28:	00f41623          	sh	a5,12(s0)
20059c2c:	00f41423          	sh	a5,8(s0)
20059c30:	0b0107b7          	lui	a5,0xb010
20059c34:	10178793          	addi	a5,a5,257 # b010101 <__ap_sram_heap_size+0xafd0101>
20059c38:	c81c                	c.sw	a5,16(s0)
20059c3a:	4789                	c.li	a5,2
20059c3c:	cc1c                	c.sw	a5,24(s0)
20059c3e:	00020737          	lui	a4,0x20
20059c42:	10100793          	addi	a5,zero,257
20059c46:	02f41523          	sh	a5,42(s0)
20059c4a:	02f41623          	sh	a5,44(s0)
20059c4e:	20070713          	addi	a4,a4,512 # 20200 <__bdram_heap_buffer_size__+0x8440>
20059c52:	4791                	c.li	a5,4
20059c54:	c018                	c.sw	a4,0(s0)
20059c56:	02f40723          	sb	a5,46(s0)
20059c5a:	02f40823          	sb	a5,48(s0)
20059c5e:	070f0737          	lui	a4,0x70f0
20059c62:	6785                	c.lui	a5,0x1
20059c64:	c058                	c.sw	a4,4(s0)
20059c66:	38878793          	addi	a5,a5,904 # 1388 <CPU_ClkGet+0x872>
20059c6a:	4751                	c.li	a4,20
20059c6c:	40b2                	c.lwsp	ra,12(sp)
20059c6e:	00040ea3          	sb	zero,29(s0)
20059c72:	00040a23          	sb	zero,20(s0)
20059c76:	00041b23          	sh	zero,22(s0)
20059c7a:	020404a3          	sb	zero,41(s0)
20059c7e:	02e40ca3          	sb	a4,57(s0)
20059c82:	d85c                	c.sw	a5,52(s0)
20059c84:	4422                	c.lwsp	s0,8(sp)
20059c86:	0141                	c.addi	sp,16
20059c88:	8082                	c.jr	ra

20059c8a <at_test>:
20059c8a:	200625b7          	lui	a1,0x20062
20059c8e:	20062537          	lui	a0,0x20062
20059c92:	21858593          	addi	a1,a1,536 # 20062218 <pmap_func+0x428>
20059c96:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
20059c9a:	4640506f          	jal	zero,2005f0fe <DiagPrintf_minimal>

20059c9e <at_otaclear>:
20059c9e:	1141                	c.addi	sp,-16
20059ca0:	c606                	c.swsp	ra,12(sp)
20059ca2:	342030ef          	jal	ra,2005cfe4 <sys_clear_ota_signature>
20059ca6:	40b2                	c.lwsp	ra,12(sp)
20059ca8:	200625b7          	lui	a1,0x20062
20059cac:	20062537          	lui	a0,0x20062
20059cb0:	22c58593          	addi	a1,a1,556 # 2006222c <pmap_func+0x43c>
20059cb4:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
20059cb8:	0141                	c.addi	sp,16
20059cba:	4440506f          	jal	zero,2005f0fe <DiagPrintf_minimal>

20059cbe <at_otarecover>:
20059cbe:	1141                	c.addi	sp,-16
20059cc0:	c606                	c.swsp	ra,12(sp)
20059cc2:	444030ef          	jal	ra,2005d106 <sys_recover_ota_signature>
20059cc6:	40b2                	c.lwsp	ra,12(sp)
20059cc8:	200625b7          	lui	a1,0x20062
20059ccc:	20062537          	lui	a0,0x20062
20059cd0:	23858593          	addi	a1,a1,568 # 20062238 <pmap_func+0x448>
20059cd4:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
20059cd8:	0141                	c.addi	sp,16
20059cda:	4240506f          	jal	zero,2005f0fe <DiagPrintf_minimal>

20059cde <at_rst>:
20059cde:	200625b7          	lui	a1,0x20062
20059ce2:	20062537          	lui	a0,0x20062
20059ce6:	1141                	c.addi	sp,-16
20059ce8:	24858593          	addi	a1,a1,584 # 20062248 <pmap_func+0x458>
20059cec:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
20059cf0:	c606                	c.swsp	ra,12(sp)
20059cf2:	40c050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059cf6:	40b2                	c.lwsp	ra,12(sp)
20059cf8:	0141                	c.addi	sp,16
20059cfa:	5480306f          	jal	zero,2005d242 <sys_reset>

20059cfe <at_state>:
20059cfe:	bf010113          	addi	sp,sp,-1040
20059d02:	850a                	c.mv	a0,sp
20059d04:	40112623          	sw	ra,1036(sp)
20059d08:	40812423          	sw	s0,1032(sp)
20059d0c:	482040ef          	jal	ra,2005e18e <vTaskList>
20059d10:	20062537          	lui	a0,0x20062
20059d14:	858a                	c.mv	a1,sp
20059d16:	25050513          	addi	a0,a0,592 # 20062250 <pmap_func+0x460>
20059d1a:	3e4050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059d1e:	850a                	c.mv	a0,sp
20059d20:	5eb000ef          	jal	ra,2005ab0a <vPortGetHeapStats>
20059d24:	20062537          	lui	a0,0x20062
20059d28:	26450513          	addi	a0,a0,612 # 20062264 <pmap_func+0x474>
20059d2c:	3d2050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059d30:	ec81a583          	lw	a1,-312(gp) # 20066144 <total_heap_size>
20059d34:	20062537          	lui	a0,0x20062
20059d38:	27450513          	addi	a0,a0,628 # 20062274 <pmap_func+0x484>
20059d3c:	3c2050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059d40:	4582                	c.lwsp	a1,0(sp)
20059d42:	20062537          	lui	a0,0x20062
20059d46:	28850513          	addi	a0,a0,648 # 20062288 <pmap_func+0x498>
20059d4a:	3b4050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059d4e:	4782                	c.lwsp	a5,0(sp)
20059d50:	ec81a583          	lw	a1,-312(gp) # 20066144 <total_heap_size>
20059d54:	20062537          	lui	a0,0x20062
20059d58:	29c50513          	addi	a0,a0,668 # 2006229c <pmap_func+0x4ac>
20059d5c:	8d9d                	c.sub	a1,a5
20059d5e:	3a0050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059d62:	47c2                	c.lwsp	a5,16(sp)
20059d64:	ec81a583          	lw	a1,-312(gp) # 20066144 <total_heap_size>
20059d68:	20062537          	lui	a0,0x20062
20059d6c:	2b050513          	addi	a0,a0,688 # 200622b0 <pmap_func+0x4c0>
20059d70:	8d9d                	c.sub	a1,a5
20059d72:	38c050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059d76:	200625b7          	lui	a1,0x20062
20059d7a:	20062537          	lui	a0,0x20062
20059d7e:	2c458593          	addi	a1,a1,708 # 200622c4 <pmap_func+0x4d4>
20059d82:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
20059d86:	378050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059d8a:	40c12083          	lw	ra,1036(sp)
20059d8e:	40812403          	lw	s0,1032(sp)
20059d92:	41010113          	addi	sp,sp,1040
20059d96:	8082                	c.jr	ra

20059d98 <at_gmr>:
20059d98:	715d                	c.addi16sp	sp,-80
20059d9a:	40000513          	addi	a0,zero,1024
20059d9e:	c686                	c.swsp	ra,76(sp)
20059da0:	c4a2                	c.swsp	s0,72(sp)
20059da2:	c2a6                	c.swsp	s1,68(sp)
20059da4:	111010ef          	jal	ra,2005b6b4 <rtos_mem_malloc>
20059da8:	3ff00593          	addi	a1,zero,1023
20059dac:	842a                	c.mv	s0,a0
20059dae:	200624b7          	lui	s1,0x20062
20059db2:	23f040ef          	jal	ra,2005e7f0 <ChipInfo_GetSocName_ToBuf>
20059db6:	85a2                	c.mv	a1,s0
20059db8:	2cc48513          	addi	a0,s1,716 # 200622cc <pmap_func+0x4dc>
20059dbc:	342050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059dc0:	8522                	c.mv	a0,s0
20059dc2:	3ff00593          	addi	a1,zero,1023
20059dc6:	2e7040ef          	jal	ra,2005e8ac <ChipInfo_GetLibVersion_ToBuf>
20059dca:	85a2                	c.mv	a1,s0
20059dcc:	2cc48513          	addi	a0,s1,716
20059dd0:	32e050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059dd4:	8522                	c.mv	a0,s0
20059dd6:	109010ef          	jal	ra,2005b6de <rtos_mem_free>
20059dda:	200625b7          	lui	a1,0x20062
20059dde:	02000613          	addi	a2,zero,32
20059de2:	2d058593          	addi	a1,a1,720 # 200622d0 <pmap_func+0x4e0>
20059de6:	850a                	c.mv	a0,sp
20059de8:	1c5070ef          	jal	ra,200617ac <strncpy>
20059dec:	200625b7          	lui	a1,0x20062
20059df0:	02000613          	addi	a2,zero,32
20059df4:	2d858593          	addi	a1,a1,728 # 200622d8 <pmap_func+0x4e8>
20059df8:	1008                	c.addi4spn	a0,sp,32
20059dfa:	1b3070ef          	jal	ra,200617ac <strncpy>
20059dfe:	20062737          	lui	a4,0x20062
20059e02:	200625b7          	lui	a1,0x20062
20059e06:	20062537          	lui	a0,0x20062
20059e0a:	1014                	c.addi4spn	a3,sp,32
20059e0c:	860a                	c.mv	a2,sp
20059e0e:	2e070713          	addi	a4,a4,736 # 200622e0 <pmap_func+0x4f0>
20059e12:	2f458593          	addi	a1,a1,756 # 200622f4 <pmap_func+0x504>
20059e16:	2fc50513          	addi	a0,a0,764 # 200622fc <pmap_func+0x50c>
20059e1a:	2e4050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059e1e:	40b6                	c.lwsp	ra,76(sp)
20059e20:	4426                	c.lwsp	s0,72(sp)
20059e22:	4496                	c.lwsp	s1,68(sp)
20059e24:	6161                	c.addi16sp	sp,80
20059e26:	8082                	c.jr	ra

20059e28 <at_log>:
20059e28:	7119                	c.addi16sp	sp,-128
20059e2a:	daa6                	c.swsp	s1,116(sp)
20059e2c:	05c00613          	addi	a2,zero,92
20059e30:	84aa                	c.mv	s1,a0
20059e32:	4581                	c.li	a1,0
20059e34:	0048                	c.addi4spn	a0,sp,4
20059e36:	dca2                	c.swsp	s0,120(sp)
20059e38:	d8ca                	c.swsp	s2,112(sp)
20059e3a:	de86                	c.swsp	ra,124(sp)
20059e3c:	d6ce                	c.swsp	s3,108(sp)
20059e3e:	20062437          	lui	s0,0x20062
20059e42:	dffb0097          	auipc	ra,0xdffb0
20059e46:	c8c080e7          	jalr	ra,-884(ra) # 9ace <__wrap_memset>
20059e4a:	20062937          	lui	s2,0x20062
20059e4e:	e4a5                	c.bnez	s1,20059eb6 <at_log+0x8e>
20059e50:	200626b7          	lui	a3,0x20062
20059e54:	30c68693          	addi	a3,a3,780 # 2006230c <pmap_func+0x51c>
20059e58:	04100613          	addi	a2,zero,65
20059e5c:	32040593          	addi	a1,s0,800 # 20062320 <pmap_func+0x530>
20059e60:	4505                	c.li	a0,1
20059e62:	056010ef          	jal	ra,2005aeb8 <rtk_log_write>
20059e66:	20062537          	lui	a0,0x20062
20059e6a:	4605                	c.li	a2,1
20059e6c:	45c90593          	addi	a1,s2,1116 # 2006245c <pmap_func+0x66c>
20059e70:	46450513          	addi	a0,a0,1124 # 20062464 <pmap_func+0x674>
20059e74:	28a050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059e78:	20063537          	lui	a0,0x20063
20059e7c:	a9450513          	addi	a0,a0,-1388 # 20062a94 <__FUNCTION__.0+0x20c>
20059e80:	27e050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059e84:	20062537          	lui	a0,0x20062
20059e88:	47450513          	addi	a0,a0,1140 # 20062474 <pmap_func+0x684>
20059e8c:	272050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059e90:	20062537          	lui	a0,0x20062
20059e94:	4a050513          	addi	a0,a0,1184 # 200624a0 <pmap_func+0x6b0>
20059e98:	266050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059e9c:	20062537          	lui	a0,0x20062
20059ea0:	4d850513          	addi	a0,a0,1240 # 200624d8 <pmap_func+0x6e8>
20059ea4:	25a050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059ea8:	20062537          	lui	a0,0x20062
20059eac:	50c50513          	addi	a0,a0,1292 # 2006250c <pmap_func+0x71c>
20059eb0:	24e050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059eb4:	a0c1                	c.j	20059f74 <at_log+0x14c>
20059eb6:	8526                	c.mv	a0,s1
20059eb8:	004c                	c.addi4spn	a1,sp,4
20059eba:	1e0010ef          	jal	ra,2005b09a <parse_param>
20059ebe:	4985                	c.li	s3,1
20059ec0:	84aa                	c.mv	s1,a0
20059ec2:	14a9d163          	bge	s3,a0,2005a004 <at_log+0x1dc>
20059ec6:	4522                	c.lwsp	a0,8(sp)
20059ec8:	12050e63          	beq	a0,zero,2005a004 <at_log+0x1dc>
20059ecc:	6fa070ef          	jal	ra,200615c6 <atoi>
20059ed0:	872a                	c.mv	a4,a0
20059ed2:	e945                	c.bnez	a0,20059f82 <at_log+0x15a>
20059ed4:	478d                	c.li	a5,3
20059ed6:	00f49663          	bne	s1,a5,20059ee2 <at_log+0xba>
20059eda:	4532                	c.lwsp	a0,12(sp)
20059edc:	00054783          	lbu	a5,0(a0)
20059ee0:	e791                	c.bnez	a5,20059eec <at_log+0xc4>
20059ee2:	200626b7          	lui	a3,0x20062
20059ee6:	32468693          	addi	a3,a3,804 # 20062324 <pmap_func+0x534>
20059eea:	b7bd                	c.j	20059e58 <at_log+0x30>
20059eec:	591000ef          	jal	ra,2005ac7c <rtk_log_level_get>
20059ef0:	45b2                	c.lwsp	a1,12(sp)
20059ef2:	862a                	c.mv	a2,a0
20059ef4:	20062537          	lui	a0,0x20062
20059ef8:	34450513          	addi	a0,a0,836 # 20062344 <pmap_func+0x554>
20059efc:	202050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059f00:	200626b7          	lui	a3,0x20062
20059f04:	3d868693          	addi	a3,a3,984 # 200623d8 <pmap_func+0x5e8>
20059f08:	04100613          	addi	a2,zero,65
20059f0c:	32040593          	addi	a1,s0,800
20059f10:	4505                	c.li	a0,1
20059f12:	7a7000ef          	jal	ra,2005aeb8 <rtk_log_write>
20059f16:	200626b7          	lui	a3,0x20062
20059f1a:	3f468693          	addi	a3,a3,1012 # 200623f4 <pmap_func+0x604>
20059f1e:	04500613          	addi	a2,zero,69
20059f22:	32040593          	addi	a1,s0,800
20059f26:	4509                	c.li	a0,2
20059f28:	791000ef          	jal	ra,2005aeb8 <rtk_log_write>
20059f2c:	200626b7          	lui	a3,0x20062
20059f30:	41068693          	addi	a3,a3,1040 # 20062410 <pmap_func+0x620>
20059f34:	05700613          	addi	a2,zero,87
20059f38:	32040593          	addi	a1,s0,800
20059f3c:	450d                	c.li	a0,3
20059f3e:	77b000ef          	jal	ra,2005aeb8 <rtk_log_write>
20059f42:	200626b7          	lui	a3,0x20062
20059f46:	32040593          	addi	a1,s0,800
20059f4a:	42c68693          	addi	a3,a3,1068 # 2006242c <pmap_func+0x63c>
20059f4e:	04900613          	addi	a2,zero,73
20059f52:	4511                	c.li	a0,4
20059f54:	765000ef          	jal	ra,2005aeb8 <rtk_log_write>
20059f58:	20062537          	lui	a0,0x20062
20059f5c:	44450513          	addi	a0,a0,1092 # 20062444 <pmap_func+0x654>
20059f60:	19e050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059f64:	20062537          	lui	a0,0x20062
20059f68:	45c90593          	addi	a1,s2,1116
20059f6c:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
20059f70:	18e050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20059f74:	50f6                	c.lwsp	ra,124(sp)
20059f76:	5466                	c.lwsp	s0,120(sp)
20059f78:	54d6                	c.lwsp	s1,116(sp)
20059f7a:	5946                	c.lwsp	s2,112(sp)
20059f7c:	59b6                	c.lwsp	s3,108(sp)
20059f7e:	6109                	c.addi16sp	sp,128
20059f80:	8082                	c.jr	ra
20059f82:	05351a63          	bne	a0,s3,20059fd6 <at_log+0x1ae>
20059f86:	4791                	c.li	a5,4
20059f88:	00f49a63          	bne	s1,a5,20059f9c <at_log+0x174>
20059f8c:	47b2                	c.lwsp	a5,12(sp)
20059f8e:	0007c783          	lbu	a5,0(a5)
20059f92:	c789                	c.beqz	a5,20059f9c <at_log+0x174>
20059f94:	4542                	c.lwsp	a0,16(sp)
20059f96:	00054783          	lbu	a5,0(a0)
20059f9a:	e791                	c.bnez	a5,20059fa6 <at_log+0x17e>
20059f9c:	200626b7          	lui	a3,0x20062
20059fa0:	35868693          	addi	a3,a3,856 # 20062358 <pmap_func+0x568>
20059fa4:	bd55                	c.j	20059e58 <at_log+0x30>
20059fa6:	620070ef          	jal	ra,200615c6 <atoi>
20059faa:	4795                	c.li	a5,5
20059fac:	872a                	c.mv	a4,a0
20059fae:	00a7fe63          	bgeu	a5,a0,20059fca <at_log+0x1a2>
20059fb2:	200626b7          	lui	a3,0x20062
20059fb6:	37868693          	addi	a3,a3,888 # 20062378 <pmap_func+0x588>
20059fba:	04100613          	addi	a2,zero,65
20059fbe:	32040593          	addi	a1,s0,800
20059fc2:	4505                	c.li	a0,1
20059fc4:	6f5000ef          	jal	ra,2005aeb8 <rtk_log_write>
20059fc8:	bd79                	c.j	20059e66 <at_log+0x3e>
20059fca:	0ff57593          	andi	a1,a0,255
20059fce:	4532                	c.lwsp	a0,12(sp)
20059fd0:	529000ef          	jal	ra,2005acf8 <rtk_log_level_set>
20059fd4:	b735                	c.j	20059f00 <at_log+0xd8>
20059fd6:	4789                	c.li	a5,2
20059fd8:	00f51e63          	bne	a0,a5,20059ff4 <at_log+0x1cc>
20059fdc:	00a48763          	beq	s1,a0,20059fea <at_log+0x1c2>
20059fe0:	200626b7          	lui	a3,0x20062
20059fe4:	39868693          	addi	a3,a3,920 # 20062398 <pmap_func+0x5a8>
20059fe8:	bd85                	c.j	20059e58 <at_log+0x30>
20059fea:	47018513          	addi	a0,gp,1136 # 200666ec <rtk_log_tag_array>
20059fee:	41b000ef          	jal	ra,2005ac08 <rtk_log_array_print>
20059ff2:	b739                	c.j	20059f00 <at_log+0xd8>
20059ff4:	468d                	c.li	a3,3
20059ff6:	00d51863          	bne	a0,a3,2005a006 <at_log+0x1de>
20059ffa:	fef493e3          	bne	s1,a5,20059fe0 <at_log+0x1b8>
20059ffe:	45f000ef          	jal	ra,2005ac5c <rtk_log_array_clear>
2005a002:	bdfd                	c.j	20059f00 <at_log+0xd8>
2005a004:	577d                	c.li	a4,-1
2005a006:	200626b7          	lui	a3,0x20062
2005a00a:	3bc68693          	addi	a3,a3,956 # 200623bc <pmap_func+0x5cc>
2005a00e:	b775                	c.j	20059fba <at_log+0x192>

2005a010 <at_rreg>:
2005a010:	7159                	c.addi16sp	sp,-112
2005a012:	d4a2                	c.swsp	s0,104(sp)
2005a014:	05c00613          	addi	a2,zero,92
2005a018:	842a                	c.mv	s0,a0
2005a01a:	4581                	c.li	a1,0
2005a01c:	0048                	c.addi4spn	a0,sp,4
2005a01e:	d2a6                	c.swsp	s1,100(sp)
2005a020:	d686                	c.swsp	ra,108(sp)
2005a022:	200624b7          	lui	s1,0x20062
2005a026:	dffb0097          	auipc	ra,0xdffb0
2005a02a:	aa8080e7          	jalr	ra,-1368(ra) # 9ace <__wrap_memset>
2005a02e:	ec05                	c.bnez	s0,2005a066 <at_rreg+0x56>
2005a030:	200626b7          	lui	a3,0x20062
2005a034:	200625b7          	lui	a1,0x20062
2005a038:	05700613          	addi	a2,zero,87
2005a03c:	32058593          	addi	a1,a1,800 # 20062320 <pmap_func+0x530>
2005a040:	450d                	c.li	a0,3
2005a042:	54068693          	addi	a3,a3,1344 # 20062540 <pmap_func+0x750>
2005a046:	673000ef          	jal	ra,2005aeb8 <rtk_log_write>
2005a04a:	20062537          	lui	a0,0x20062
2005a04e:	4605                	c.li	a2,1
2005a050:	55c48593          	addi	a1,s1,1372 # 2006255c <pmap_func+0x76c>
2005a054:	46450513          	addi	a0,a0,1124 # 20062464 <pmap_func+0x674>
2005a058:	0a6050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a05c:	50b6                	c.lwsp	ra,108(sp)
2005a05e:	5426                	c.lwsp	s0,104(sp)
2005a060:	5496                	c.lwsp	s1,100(sp)
2005a062:	6165                	c.addi16sp	sp,112
2005a064:	8082                	c.jr	ra
2005a066:	004c                	c.addi4spn	a1,sp,4
2005a068:	8522                	c.mv	a0,s0
2005a06a:	030010ef          	jal	ra,2005b09a <parse_param>
2005a06e:	ffe50713          	addi	a4,a0,-2
2005a072:	4789                	c.li	a5,2
2005a074:	fae7eee3          	bltu	a5,a4,2005a030 <at_rreg+0x20>
2005a078:	157d                	c.addi	a0,-1
2005a07a:	0542                	c.slli	a0,0x10
2005a07c:	002c                	c.addi4spn	a1,sp,8
2005a07e:	8141                	c.srli	a0,0x10
2005a080:	452010ef          	jal	ra,2005b4d2 <cmd_dump_word>
2005a084:	20062537          	lui	a0,0x20062
2005a088:	55c48593          	addi	a1,s1,1372
2005a08c:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
2005a090:	06e050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a094:	b7e1                	c.j	2005a05c <at_rreg+0x4c>

2005a096 <at_wreg>:
2005a096:	7159                	c.addi16sp	sp,-112
2005a098:	d4a2                	c.swsp	s0,104(sp)
2005a09a:	05c00613          	addi	a2,zero,92
2005a09e:	842a                	c.mv	s0,a0
2005a0a0:	4581                	c.li	a1,0
2005a0a2:	0048                	c.addi4spn	a0,sp,4
2005a0a4:	d2a6                	c.swsp	s1,100(sp)
2005a0a6:	d686                	c.swsp	ra,108(sp)
2005a0a8:	200624b7          	lui	s1,0x20062
2005a0ac:	dffb0097          	auipc	ra,0xdffb0
2005a0b0:	a22080e7          	jalr	ra,-1502(ra) # 9ace <__wrap_memset>
2005a0b4:	ec05                	c.bnez	s0,2005a0ec <at_wreg+0x56>
2005a0b6:	200626b7          	lui	a3,0x20062
2005a0ba:	200625b7          	lui	a1,0x20062
2005a0be:	05700613          	addi	a2,zero,87
2005a0c2:	32058593          	addi	a1,a1,800 # 20062320 <pmap_func+0x530>
2005a0c6:	450d                	c.li	a0,3
2005a0c8:	56468693          	addi	a3,a3,1380 # 20062564 <pmap_func+0x774>
2005a0cc:	5ed000ef          	jal	ra,2005aeb8 <rtk_log_write>
2005a0d0:	20062537          	lui	a0,0x20062
2005a0d4:	4605                	c.li	a2,1
2005a0d6:	58048593          	addi	a1,s1,1408 # 20062580 <pmap_func+0x790>
2005a0da:	46450513          	addi	a0,a0,1124 # 20062464 <pmap_func+0x674>
2005a0de:	020050ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a0e2:	50b6                	c.lwsp	ra,108(sp)
2005a0e4:	5426                	c.lwsp	s0,104(sp)
2005a0e6:	5496                	c.lwsp	s1,100(sp)
2005a0e8:	6165                	c.addi16sp	sp,112
2005a0ea:	8082                	c.jr	ra
2005a0ec:	004c                	c.addi4spn	a1,sp,4
2005a0ee:	8522                	c.mv	a0,s0
2005a0f0:	7ab000ef          	jal	ra,2005b09a <parse_param>
2005a0f4:	478d                	c.li	a5,3
2005a0f6:	fcf510e3          	bne	a0,a5,2005a0b6 <at_wreg+0x20>
2005a0fa:	45a2                	c.lwsp	a1,8(sp)
2005a0fc:	4509                	c.li	a0,2
2005a0fe:	32e010ef          	jal	ra,2005b42c <cmd_write_word>
2005a102:	20062537          	lui	a0,0x20062
2005a106:	58048593          	addi	a1,s1,1408
2005a10a:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
2005a10e:	7f1040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a112:	bfc1                	c.j	2005a0e2 <at_wreg+0x4c>

2005a114 <find_status>:
2005a114:	872a                	c.mv	a4,a0
2005a116:	4781                	c.li	a5,0
2005a118:	00c7c463          	blt	a5,a2,2005a120 <find_status+0xc>
2005a11c:	4501                	c.li	a0,0
2005a11e:	8082                	c.jr	ra
2005a120:	852e                	c.mv	a0,a1
2005a122:	4594                	c.lw	a3,8(a1)
2005a124:	02458593          	addi	a1,a1,36
2005a128:	fee68be3          	beq	a3,a4,2005a11e <find_status+0xa>
2005a12c:	0785                	c.addi	a5,1
2005a12e:	b7ed                	c.j	2005a118 <find_status+0x4>

2005a130 <find_free_delta>:
2005a130:	87aa                	c.mv	a5,a0
2005a132:	4701                	c.li	a4,0
2005a134:	00b74463          	blt	a4,a1,2005a13c <find_free_delta+0xc>
2005a138:	4501                	c.li	a0,0
2005a13a:	8082                	c.jr	ra
2005a13c:	853e                	c.mv	a0,a5
2005a13e:	5794                	c.lw	a3,40(a5)
2005a140:	03878793          	addi	a5,a5,56
2005a144:	dafd                	c.beqz	a3,2005a13a <find_free_delta+0xa>
2005a146:	0705                	c.addi	a4,1
2005a148:	b7f5                	c.j	2005a134 <find_free_delta+0x4>

2005a14a <update_status>:
2005a14a:	7179                	c.addi16sp	sp,-48
2005a14c:	d226                	c.swsp	s1,36(sp)
2005a14e:	ec01a783          	lw	a5,-320(gp) # 2006613c <task_status>
2005a152:	6705                	c.lui	a4,0x1
2005a154:	d606                	c.swsp	ra,44(sp)
2005a156:	d422                	c.swsp	s0,40(sp)
2005a158:	d04a                	c.swsp	s2,32(sp)
2005a15a:	ce4e                	c.swsp	s3,28(sp)
2005a15c:	cc52                	c.swsp	s4,24(sp)
2005a15e:	ca56                	c.swsp	s5,20(sp)
2005a160:	c85a                	c.swsp	s6,16(sp)
2005a162:	c65e                	c.swsp	s7,12(sp)
2005a164:	c462                	c.swsp	s8,8(sp)
2005a166:	00e786b3          	add	a3,a5,a4
2005a16a:	2006a603          	lw	a2,512(a3)
2005a16e:	ec018493          	addi	s1,gp,-320 # 2006613c <task_status>
2005a172:	ea19                	c.bnez	a2,2005a188 <update_status+0x3e>
2005a174:	2046a603          	lw	a2,516(a3)
2005a178:	ea01                	c.bnez	a2,2005a188 <update_status+0x3e>
2005a17a:	90070713          	addi	a4,a4,-1792 # 900 <BKUP_Set+0x5a>
2005a17e:	973e                	c.add	a4,a5
2005a180:	20f6a023          	sw	a5,512(a3)
2005a184:	20e6a223          	sw	a4,516(a3)
2005a188:	6405                	c.lui	s0,0x1
2005a18a:	97a2                	c.add	a5,s0
2005a18c:	20078793          	addi	a5,a5,512
2005a190:	47d8                	c.lw	a4,12(a5)
2005a192:	43d4                	c.lw	a3,4(a5)
2005a194:	21040913          	addi	s2,s0,528 # 1210 <CPU_ClkGet+0x6fa>
2005a198:	c798                	c.sw	a4,8(a5)
2005a19a:	4398                	c.lw	a4,0(a5)
2005a19c:	c394                	c.sw	a3,0(a5)
2005a19e:	4a01                	c.li	s4,0
2005a1a0:	c3d8                	c.sw	a4,4(a5)
2005a1a2:	34e030ef          	jal	ra,2005d4f0 <uxTaskGetNumberOfTasks>
2005a1a6:	409c                	c.lw	a5,0(s1)
2005a1a8:	85aa                	c.mv	a1,a0
2005a1aa:	4601                	c.li	a2,0
2005a1ac:	97a2                	c.add	a5,s0
2005a1ae:	20a7a623          	sw	a0,524(a5)
2005a1b2:	2047a503          	lw	a0,516(a5)
2005a1b6:	699030ef          	jal	ra,2005e04e <uxTaskGetSystemState>
2005a1ba:	409c                	c.lw	a5,0(s1)
2005a1bc:	e0040613          	addi	a2,s0,-512
2005a1c0:	4581                	c.li	a1,0
2005a1c2:	00878733          	add	a4,a5,s0
2005a1c6:	20a72623          	sw	a0,524(a4)
2005a1ca:	01278533          	add	a0,a5,s2
2005a1ce:	dffb0097          	auipc	ra,0xdffb0
2005a1d2:	900080e7          	jalr	ra,-1792(ra) # 9ace <__wrap_memset>
2005a1d6:	0004aa83          	lw	s5,0(s1)
2005a1da:	9456                	c.add	s0,s5
2005a1dc:	20842783          	lw	a5,520(s0)
2005a1e0:	00f05963          	bge	zero,a5,2005a1f2 <update_status+0xa8>
2005a1e4:	02400c13          	addi	s8,zero,36
2005a1e8:	9aca                	c.add	s5,s2
2005a1ea:	20c42783          	lw	a5,524(s0)
2005a1ee:	00fa4e63          	blt	s4,a5,2005a20a <update_status+0xc0>
2005a1f2:	50b2                	c.lwsp	ra,44(sp)
2005a1f4:	5422                	c.lwsp	s0,40(sp)
2005a1f6:	5492                	c.lwsp	s1,36(sp)
2005a1f8:	5902                	c.lwsp	s2,32(sp)
2005a1fa:	49f2                	c.lwsp	s3,28(sp)
2005a1fc:	4a62                	c.lwsp	s4,24(sp)
2005a1fe:	4ad2                	c.lwsp	s5,20(sp)
2005a200:	4b42                	c.lwsp	s6,16(sp)
2005a202:	4bb2                	c.lwsp	s7,12(sp)
2005a204:	4c22                	c.lwsp	s8,8(sp)
2005a206:	6145                	c.addi16sp	sp,48
2005a208:	8082                	c.jr	ra
2005a20a:	038a0933          	mul	s2,s4,s8
2005a20e:	20442b83          	lw	s7,516(s0)
2005a212:	20842603          	lw	a2,520(s0)
2005a216:	20042583          	lw	a1,512(s0)
2005a21a:	9bca                	c.add	s7,s2
2005a21c:	008ba503          	lw	a0,8(s7)
2005a220:	3dd5                	c.jal	2005a114 <find_status>
2005a222:	04000593          	addi	a1,zero,64
2005a226:	8b2a                	c.mv	s6,a0
2005a228:	8556                	c.mv	a0,s5
2005a22a:	3719                	c.jal	2005a130 <find_free_delta>
2005a22c:	000ba783          	lw	a5,0(s7)
2005a230:	84aa                	c.mv	s1,a0
2005a232:	467d                	c.li	a2,31
2005a234:	d51c                	c.sw	a5,40(a0)
2005a236:	20442783          	lw	a5,516(s0)
2005a23a:	97ca                	c.add	a5,s2
2005a23c:	43cc                	c.lw	a1,4(a5)
2005a23e:	c14c                	c.sw	a1,4(a0)
2005a240:	20442783          	lw	a5,516(s0)
2005a244:	97ca                	c.add	a5,s2
2005a246:	479c                	c.lw	a5,8(a5)
2005a248:	00052023          	sw	zero,0(a0)
2005a24c:	0521                	c.addi	a0,8
2005a24e:	d15c                	c.sw	a5,36(a0)
2005a250:	20442783          	lw	a5,516(s0)
2005a254:	97ca                	c.add	a5,s2
2005a256:	00c7c783          	lbu	a5,12(a5)
2005a25a:	02f50623          	sb	a5,44(a0)
2005a25e:	20442783          	lw	a5,516(s0)
2005a262:	97ca                	c.add	a5,s2
2005a264:	4b9c                	c.lw	a5,16(a5)
2005a266:	d51c                	c.sw	a5,40(a0)
2005a268:	544070ef          	jal	ra,200617ac <strncpy>
2005a26c:	c095                	c.beqz	s1,2005a290 <update_status+0x146>
2005a26e:	20442783          	lw	a5,516(s0)
2005a272:	97ca                	c.add	a5,s2
2005a274:	4f9c                	c.lw	a5,24(a5)
2005a276:	000b0563          	beq	s6,zero,2005a280 <update_status+0x136>
2005a27a:	018b2703          	lw	a4,24(s6)
2005a27e:	8f99                	c.sub	a5,a4
2005a280:	c09c                	c.sw	a5,0(s1)
2005a282:	20442783          	lw	a5,516(s0)
2005a286:	993e                	c.add	s2,a5
2005a288:	00c94783          	lbu	a5,12(s2)
2005a28c:	02f48a23          	sb	a5,52(s1)
2005a290:	0a05                	c.addi	s4,1
2005a292:	bfa1                	c.j	2005a1ea <update_status+0xa0>

2005a294 <print_delta>:
2005a294:	715d                	c.addi16sp	sp,-80
2005a296:	736277b7          	lui	a5,0x73627
2005a29a:	c4a2                	c.swsp	s0,72(sp)
2005a29c:	25278793          	addi	a5,a5,594 # 73627252 <__kr4_psram_text_start__+0x134a7232>
2005a2a0:	842a                	c.mv	s0,a0
2005a2a2:	20062737          	lui	a4,0x20062
2005a2a6:	200626b7          	lui	a3,0x20062
2005a2aa:	20062637          	lui	a2,0x20062
2005a2ae:	200625b7          	lui	a1,0x20062
2005a2b2:	20062537          	lui	a0,0x20062
2005a2b6:	c43e                	c.swsp	a5,8(sp)
2005a2b8:	58870713          	addi	a4,a4,1416 # 20062588 <pmap_func+0x798>
2005a2bc:	04400793          	addi	a5,zero,68
2005a2c0:	59068693          	addi	a3,a3,1424 # 20062590 <pmap_func+0x7a0>
2005a2c4:	59860613          	addi	a2,a2,1432 # 20062598 <pmap_func+0x7a8>
2005a2c8:	5a058593          	addi	a1,a1,1440 # 200625a0 <pmap_func+0x7b0>
2005a2cc:	5ac50513          	addi	a0,a0,1452 # 200625ac <pmap_func+0x7bc>
2005a2d0:	c2a6                	c.swsp	s1,68(sp)
2005a2d2:	c0ca                	c.swsp	s2,64(sp)
2005a2d4:	de4e                	c.swsp	s3,60(sp)
2005a2d6:	dc52                	c.swsp	s4,56(sp)
2005a2d8:	da56                	c.swsp	s5,52(sp)
2005a2da:	d85a                	c.swsp	s6,48(sp)
2005a2dc:	d65e                	c.swsp	s7,44(sp)
2005a2de:	ee22                	c.fswsp	fs0,28(sp)
2005a2e0:	00f10623          	sb	a5,12(sp)
2005a2e4:	c686                	c.swsp	ra,76(sp)
2005a2e6:	00011223          	sh	zero,4(sp)
2005a2ea:	00010323          	sb	zero,6(sp)
2005a2ee:	611040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a2f2:	e641a407          	flw	fs0,-412(gp) # 200660e0 <__sdata_start__>
2005a2f6:	6485                	c.lui	s1,0x1
2005a2f8:	4b81                	c.li	s7,0
2005a2fa:	03800993          	addi	s3,zero,56
2005a2fe:	06400b13          	addi	s6,zero,100
2005a302:	21848a13          	addi	s4,s1,536 # 1218 <CPU_ClkGet+0x702>
2005a306:	20062ab7          	lui	s5,0x20062
2005a30a:	460d                	c.li	a2,3
2005a30c:	4581                	c.li	a1,0
2005a30e:	0048                	c.addi4spn	a0,sp,4
2005a310:	dffaf097          	auipc	ra,0xdffaf
2005a314:	7be080e7          	jalr	ra,1982(ra) # 9ace <__wrap_memset>
2005a318:	033b8633          	mul	a2,s7,s3
2005a31c:	ec01a803          	lw	a6,-320(gp) # 2006613c <task_status>
2005a320:	d0047753          	fcvt.s.w	fa4,s0
2005a324:	4529                	c.li	a0,10
2005a326:	00c806b3          	add	a3,a6,a2
2005a32a:	96a6                	c.add	a3,s1
2005a32c:	2106a783          	lw	a5,528(a3)
2005a330:	d007f7d3          	fcvt.s.w	fa5,a5
2005a334:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a338:	18e7f7d3          	fdiv.s	fa5,fa5,fa4
2005a33c:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a340:	c0079753          	fcvt.w.s	a4,fa5,rtz
2005a344:	036767b3          	rem	a5,a4,s6
2005a348:	02a787b3          	mul	a5,a5,a0
2005a34c:	0367c5b3          	div	a1,a5,s6
2005a350:	0367e7b3          	rem	a5,a5,s6
2005a354:	03058593          	addi	a1,a1,48
2005a358:	00b10223          	sb	a1,4(sp)
2005a35c:	02a787b3          	mul	a5,a5,a0
2005a360:	0367c7b3          	div	a5,a5,s6
2005a364:	03078793          	addi	a5,a5,48
2005a368:	00f102a3          	sb	a5,5(sp)
2005a36c:	2386a783          	lw	a5,568(a3)
2005a370:	c78d                	c.beqz	a5,2005a39a <print_delta+0x106>
2005a372:	03674733          	div	a4,a4,s6
2005a376:	2446c503          	lbu	a0,580(a3)
2005a37a:	014605b3          	add	a1,a2,s4
2005a37e:	2406a683          	lw	a3,576(a3)
2005a382:	01050793          	addi	a5,a0,16
2005a386:	00278533          	add	a0,a5,sp
2005a38a:	ff854603          	lbu	a2,-8(a0)
2005a38e:	005c                	c.addi4spn	a5,sp,4
2005a390:	95c2                	c.add	a1,a6
2005a392:	5bca8513          	addi	a0,s5,1468 # 200625bc <pmap_func+0x7cc>
2005a396:	569040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a39a:	0b85                	c.addi	s7,1
2005a39c:	04000793          	addi	a5,zero,64
2005a3a0:	f6fb95e3          	bne	s7,a5,2005a30a <print_delta+0x76>
2005a3a4:	40b6                	c.lwsp	ra,76(sp)
2005a3a6:	4426                	c.lwsp	s0,72(sp)
2005a3a8:	4496                	c.lwsp	s1,68(sp)
2005a3aa:	4906                	c.lwsp	s2,64(sp)
2005a3ac:	59f2                	c.lwsp	s3,60(sp)
2005a3ae:	5a62                	c.lwsp	s4,56(sp)
2005a3b0:	5ad2                	c.lwsp	s5,52(sp)
2005a3b2:	5b42                	c.lwsp	s6,48(sp)
2005a3b4:	5bb2                	c.lwsp	s7,44(sp)
2005a3b6:	6472                	c.flwsp	fs0,28(sp)
2005a3b8:	6161                	c.addi16sp	sp,80
2005a3ba:	8082                	c.jr	ra

2005a3bc <cpu_stat_thread>:
2005a3bc:	7179                	c.addi16sp	sp,-48
2005a3be:	c62a                	c.swsp	a0,12(sp)
2005a3c0:	4521                	c.li	a0,8
2005a3c2:	d226                	c.swsp	s1,36(sp)
2005a3c4:	d606                	c.swsp	ra,44(sp)
2005a3c6:	d422                	c.swsp	s0,40(sp)
2005a3c8:	d04a                	c.swsp	s2,32(sp)
2005a3ca:	ce4e                	c.swsp	s3,28(sp)
2005a3cc:	cc52                	c.swsp	s4,24(sp)
2005a3ce:	ca56                	c.swsp	s5,20(sp)
2005a3d0:	2e4010ef          	jal	ra,2005b6b4 <rtos_mem_malloc>
2005a3d4:	45b2                	c.lwsp	a1,12(sp)
2005a3d6:	ec418493          	addi	s1,gp,-316 # 20066140 <top_exit_sema>
2005a3da:	e131                	c.bnez	a0,2005a41e <cpu_stat_thread+0x62>
2005a3dc:	200635b7          	lui	a1,0x20063
2005a3e0:	20062537          	lui	a0,0x20062
2005a3e4:	88858593          	addi	a1,a1,-1912 # 20062888 <__FUNCTION__.0>
2005a3e8:	5d050513          	addi	a0,a0,1488 # 200625d0 <pmap_func+0x7e0>
2005a3ec:	513040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a3f0:	4088                	c.lw	a0,0(s1)
2005a3f2:	ec018413          	addi	s0,gp,-320 # 2006613c <task_status>
2005a3f6:	4d6010ef          	jal	ra,2005b8cc <rtos_sema_delete>
2005a3fa:	4008                	c.lw	a0,0(s0)
2005a3fc:	0004a023          	sw	zero,0(s1)
2005a400:	2de010ef          	jal	ra,2005b6de <rtos_mem_free>
2005a404:	00042023          	sw	zero,0(s0)
2005a408:	5422                	c.lwsp	s0,40(sp)
2005a40a:	50b2                	c.lwsp	ra,44(sp)
2005a40c:	5492                	c.lwsp	s1,36(sp)
2005a40e:	5902                	c.lwsp	s2,32(sp)
2005a410:	49f2                	c.lwsp	s3,28(sp)
2005a412:	4a62                	c.lwsp	s4,24(sp)
2005a414:	4ad2                	c.lwsp	s5,20(sp)
2005a416:	4501                	c.li	a0,0
2005a418:	6145                	c.addi16sp	sp,48
2005a41a:	7580106f          	jal	zero,2005bb72 <rtos_task_delete>
2005a41e:	4621                	c.li	a2,8
2005a420:	842a                	c.mv	s0,a0
2005a422:	dffaf097          	auipc	ra,0xdffaf
2005a426:	6ae080e7          	jalr	ra,1710(ra) # 9ad0 <__wrap_memcpy>
2005a42a:	410187b7          	lui	a5,0x41018
2005a42e:	4bdc                	c.lw	a5,20(a5)
2005a430:	3e800a93          	addi	s5,zero,1000
2005a434:	eaf1aa23          	sw	a5,-332(gp) # 20066130 <last_tick>
2005a438:	5a7d                	c.li	s4,-1
2005a43a:	eb418913          	addi	s2,gp,-332 # 20066130 <last_tick>
2005a43e:	410189b7          	lui	s3,0x41018
2005a442:	400c                	c.lw	a1,0(s0)
2005a444:	4088                	c.lw	a0,0(s1)
2005a446:	02ba85b3          	mul	a1,s5,a1
2005a44a:	498010ef          	jal	ra,2005b8e2 <rtos_sema_take>
2005a44e:	03451463          	bne	a0,s4,2005a476 <cpu_stat_thread+0xba>
2005a452:	39e5                	c.jal	2005a14a <update_status>
2005a454:	0149a503          	lw	a0,20(s3) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a458:	00092783          	lw	a5,0(s2)
2005a45c:	0149a703          	lw	a4,20(s3)
2005a460:	8d1d                	c.sub	a0,a5
2005a462:	00e92023          	sw	a4,0(s2)
2005a466:	353d                	c.jal	2005a294 <print_delta>
2005a468:	405c                	c.lw	a5,4(s0)
2005a46a:	00f05563          	bge	zero,a5,2005a474 <cpu_stat_thread+0xb8>
2005a46e:	17fd                	c.addi	a5,-1
2005a470:	c05c                	c.sw	a5,4(s0)
2005a472:	bfc1                	c.j	2005a442 <cpu_stat_thread+0x86>
2005a474:	f7f9                	c.bnez	a5,2005a442 <cpu_stat_thread+0x86>
2005a476:	8522                	c.mv	a0,s0
2005a478:	266010ef          	jal	ra,2005b6de <rtos_mem_free>
2005a47c:	bf95                	c.j	2005a3f0 <cpu_stat_thread+0x34>

2005a47e <at_cpuload>:
2005a47e:	7119                	c.addi16sp	sp,-128
2005a480:	dca2                	c.swsp	s0,120(sp)
2005a482:	05c00613          	addi	a2,zero,92
2005a486:	842a                	c.mv	s0,a0
2005a488:	4581                	c.li	a1,0
2005a48a:	0048                	c.addi4spn	a0,sp,4
2005a48c:	de86                	c.swsp	ra,124(sp)
2005a48e:	d8ca                	c.swsp	s2,112(sp)
2005a490:	d4d2                	c.swsp	s4,104(sp)
2005a492:	daa6                	c.swsp	s1,116(sp)
2005a494:	d6ce                	c.swsp	s3,108(sp)
2005a496:	d2d6                	c.swsp	s5,100(sp)
2005a498:	dffaf097          	auipc	ra,0xdffaf
2005a49c:	636080e7          	jalr	ra,1590(ra) # 9ace <__wrap_memset>
2005a4a0:	eb818793          	addi	a5,gp,-328 # 20066134 <para_in>
2005a4a4:	4705                	c.li	a4,1
2005a4a6:	c398                	c.sw	a4,0(a5)
2005a4a8:	004c                	c.addi4spn	a1,sp,4
2005a4aa:	577d                	c.li	a4,-1
2005a4ac:	8522                	c.mv	a0,s0
2005a4ae:	c3d8                	c.sw	a4,4(a5)
2005a4b0:	3eb000ef          	jal	ra,2005b09a <parse_param>
2005a4b4:	ffe50693          	addi	a3,a0,-2
2005a4b8:	4709                	c.li	a4,2
2005a4ba:	20062a37          	lui	s4,0x20062
2005a4be:	06d77463          	bgeu	a4,a3,2005a526 <at_cpuload+0xa8>
2005a4c2:	20062537          	lui	a0,0x20062
2005a4c6:	5e450513          	addi	a0,a0,1508 # 200625e4 <pmap_func+0x7f4>
2005a4ca:	435040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a4ce:	4405                	c.li	s0,1
2005a4d0:	20062537          	lui	a0,0x20062
2005a4d4:	8622                	c.mv	a2,s0
2005a4d6:	770a0593          	addi	a1,s4,1904 # 20062770 <pmap_func+0x980>
2005a4da:	46450513          	addi	a0,a0,1124 # 20062464 <pmap_func+0x674>
2005a4de:	9875                	c.andi	s0,-3
2005a4e0:	41f040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a4e4:	1a040a63          	beq	s0,zero,2005a698 <at_cpuload+0x21a>
2005a4e8:	20063537          	lui	a0,0x20063
2005a4ec:	a9450513          	addi	a0,a0,-1388 # 20062a94 <__FUNCTION__.0+0x20c>
2005a4f0:	40f040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a4f4:	20062537          	lui	a0,0x20062
2005a4f8:	77c50513          	addi	a0,a0,1916 # 2006277c <pmap_func+0x98c>
2005a4fc:	403040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a500:	20062537          	lui	a0,0x20062
2005a504:	7ac50513          	addi	a0,a0,1964 # 200627ac <pmap_func+0x9bc>
2005a508:	3f7040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a50c:	20062537          	lui	a0,0x20062
2005a510:	7c050513          	addi	a0,a0,1984 # 200627c0 <pmap_func+0x9d0>
2005a514:	3eb040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a518:	20062537          	lui	a0,0x20062
2005a51c:	7dc50513          	addi	a0,a0,2012 # 200627dc <pmap_func+0x9ec>
2005a520:	3df040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a524:	aa95                	c.j	2005a698 <at_cpuload+0x21a>
2005a526:	87aa                	c.mv	a5,a0
2005a528:	468d                	c.li	a3,3
2005a52a:	4522                	c.lwsp	a0,8(sp)
2005a52c:	eb818993          	addi	s3,gp,-328 # 20066134 <para_in>
2005a530:	06d79163          	bne	a5,a3,2005a592 <at_cpuload+0x114>
2005a534:	092070ef          	jal	ra,200615c6 <atoi>
2005a538:	842a                	c.mv	s0,a0
2005a53a:	4532                	c.lwsp	a0,12(sp)
2005a53c:	00054783          	lbu	a5,0(a0)
2005a540:	cf81                	c.beqz	a5,2005a558 <at_cpuload+0xda>
2005a542:	084070ef          	jal	ra,200615c6 <atoi>
2005a546:	4785                	c.li	a5,1
2005a548:	00a05663          	bge	zero,a0,2005a554 <at_cpuload+0xd6>
2005a54c:	4532                	c.lwsp	a0,12(sp)
2005a54e:	078070ef          	jal	ra,200615c6 <atoi>
2005a552:	87aa                	c.mv	a5,a0
2005a554:	00f9a023          	sw	a5,0(s3)
2005a558:	ec018a93          	addi	s5,gp,-320 # 2006613c <task_status>
2005a55c:	000aa783          	lw	a5,0(s5)
2005a560:	ec018493          	addi	s1,gp,-320 # 2006613c <task_status>
2005a564:	e3c1                	c.bnez	a5,2005a5e4 <at_cpuload+0x166>
2005a566:	6509                	c.lui	a0,0x2
2005a568:	0541                	c.addi	a0,16
2005a56a:	14a010ef          	jal	ra,2005b6b4 <rtos_mem_malloc>
2005a56e:	00aaa023          	sw	a0,0(s5)
2005a572:	e92d                	c.bnez	a0,2005a5e4 <at_cpuload+0x166>
2005a574:	200626b7          	lui	a3,0x20062
2005a578:	200625b7          	lui	a1,0x20062
2005a57c:	70068693          	addi	a3,a3,1792 # 20062700 <pmap_func+0x910>
2005a580:	04900613          	addi	a2,zero,73
2005a584:	32058593          	addi	a1,a1,800 # 20062320 <pmap_func+0x530>
2005a588:	4511                	c.li	a0,4
2005a58a:	12f000ef          	jal	ra,2005aeb8 <rtk_log_write>
2005a58e:	4409                	c.li	s0,2
2005a590:	b781                	c.j	2005a4d0 <at_cpuload+0x52>
2005a592:	00e79663          	bne	a5,a4,2005a59e <at_cpuload+0x120>
2005a596:	030070ef          	jal	ra,200615c6 <atoi>
2005a59a:	842a                	c.mv	s0,a0
2005a59c:	bf75                	c.j	2005a558 <at_cpuload+0xda>
2005a59e:	028070ef          	jal	ra,200615c6 <atoi>
2005a5a2:	842a                	c.mv	s0,a0
2005a5a4:	4532                	c.lwsp	a0,12(sp)
2005a5a6:	00054783          	lbu	a5,0(a0) # 2000 <CRYPTO_Init+0x86a>
2005a5aa:	cf81                	c.beqz	a5,2005a5c2 <at_cpuload+0x144>
2005a5ac:	01a070ef          	jal	ra,200615c6 <atoi>
2005a5b0:	4785                	c.li	a5,1
2005a5b2:	00a05663          	bge	zero,a0,2005a5be <at_cpuload+0x140>
2005a5b6:	4532                	c.lwsp	a0,12(sp)
2005a5b8:	00e070ef          	jal	ra,200615c6 <atoi>
2005a5bc:	87aa                	c.mv	a5,a0
2005a5be:	00f9a023          	sw	a5,0(s3)
2005a5c2:	4542                	c.lwsp	a0,16(sp)
2005a5c4:	00054783          	lbu	a5,0(a0)
2005a5c8:	dbc1                	c.beqz	a5,2005a558 <at_cpuload+0xda>
2005a5ca:	7fd060ef          	jal	ra,200615c6 <atoi>
2005a5ce:	57fd                	c.li	a5,-1
2005a5d0:	00a05763          	bge	zero,a0,2005a5de <at_cpuload+0x160>
2005a5d4:	4542                	c.lwsp	a0,16(sp)
2005a5d6:	7f1060ef          	jal	ra,200615c6 <atoi>
2005a5da:	fff50793          	addi	a5,a0,-1
2005a5de:	00f9a223          	sw	a5,4(s3)
2005a5e2:	bf9d                	c.j	2005a558 <at_cpuload+0xda>
2005a5e4:	0049a683          	lw	a3,4(s3)
2005a5e8:	0009a603          	lw	a2,0(s3)
2005a5ec:	20062537          	lui	a0,0x20062
2005a5f0:	85a2                	c.mv	a1,s0
2005a5f2:	72450513          	addi	a0,a0,1828 # 20062724 <pmap_func+0x934>
2005a5f6:	309040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a5fa:	4789                	c.li	a5,2
2005a5fc:	0af40d63          	beq	s0,a5,2005a6b6 <at_cpuload+0x238>
2005a600:	0087c863          	blt	a5,s0,2005a610 <at_cpuload+0x192>
2005a604:	c021                	c.beqz	s0,2005a644 <at_cpuload+0x1c6>
2005a606:	4785                	c.li	a5,1
2005a608:	0af40163          	beq	s0,a5,2005a6aa <at_cpuload+0x22c>
2005a60c:	440d                	c.li	s0,3
2005a60e:	b5c9                	c.j	2005a4d0 <at_cpuload+0x52>
2005a610:	478d                	c.li	a5,3
2005a612:	fef41de3          	bne	s0,a5,2005a60c <at_cpuload+0x18e>
2005a616:	ec41a783          	lw	a5,-316(gp) # 20066140 <top_exit_sema>
2005a61a:	e7bd                	c.bnez	a5,2005a688 <at_cpuload+0x20a>
2005a61c:	b2fff0ef          	jal	ra,2005a14a <update_status>
2005a620:	41018937          	lui	s2,0x41018
2005a624:	eb418413          	addi	s0,gp,-332 # 20066130 <last_tick>
2005a628:	401c                	c.lw	a5,0(s0)
2005a62a:	01492503          	lw	a0,20(s2) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a62e:	8d1d                	c.sub	a0,a5
2005a630:	3195                	c.jal	2005a294 <print_delta>
2005a632:	01492783          	lw	a5,20(s2)
2005a636:	4088                	c.lw	a0,0(s1)
2005a638:	c01c                	c.sw	a5,0(s0)
2005a63a:	0a4010ef          	jal	ra,2005b6de <rtos_mem_free>
2005a63e:	0004a023          	sw	zero,0(s1)
2005a642:	a099                	c.j	2005a688 <at_cpuload+0x20a>
2005a644:	ec41a783          	lw	a5,-316(gp) # 20066140 <top_exit_sema>
2005a648:	e3a1                	c.bnez	a5,2005a688 <at_cpuload+0x20a>
2005a64a:	4088                	c.lw	a0,0(s1)
2005a64c:	6609                	c.lui	a2,0x2
2005a64e:	0641                	c.addi	a2,16
2005a650:	4581                	c.li	a1,0
2005a652:	dffaf097          	auipc	ra,0xdffaf
2005a656:	47c080e7          	jalr	ra,1148(ra) # 9ace <__wrap_memset>
2005a65a:	af1ff0ef          	jal	ra,2005a14a <update_status>
2005a65e:	4605                	c.li	a2,1
2005a660:	4581                	c.li	a1,0
2005a662:	ec418513          	addi	a0,gp,-316 # 20066140 <top_exit_sema>
2005a666:	1da010ef          	jal	ra,2005b840 <rtos_sema_create>
2005a66a:	2005a637          	lui	a2,0x2005a
2005a66e:	200625b7          	lui	a1,0x20062
2005a672:	47a9                	c.li	a5,10
2005a674:	6711                	c.lui	a4,0x4
2005a676:	eb818693          	addi	a3,gp,-328 # 20066134 <para_in>
2005a67a:	3bc60613          	addi	a2,a2,956 # 2005a3bc <cpu_stat_thread>
2005a67e:	76058593          	addi	a1,a1,1888 # 20062760 <pmap_func+0x970>
2005a682:	4501                	c.li	a0,0
2005a684:	4ca010ef          	jal	ra,2005bb4e <rtos_task_create>
2005a688:	20062537          	lui	a0,0x20062
2005a68c:	770a0593          	addi	a1,s4,1904
2005a690:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
2005a694:	26b040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a698:	50f6                	c.lwsp	ra,124(sp)
2005a69a:	5466                	c.lwsp	s0,120(sp)
2005a69c:	54d6                	c.lwsp	s1,116(sp)
2005a69e:	5946                	c.lwsp	s2,112(sp)
2005a6a0:	59b6                	c.lwsp	s3,108(sp)
2005a6a2:	5a26                	c.lwsp	s4,104(sp)
2005a6a4:	5a96                	c.lwsp	s5,100(sp)
2005a6a6:	6109                	c.addi16sp	sp,128
2005a6a8:	8082                	c.jr	ra
2005a6aa:	ec41a503          	lw	a0,-316(gp) # 20066140 <top_exit_sema>
2005a6ae:	dd69                	c.beqz	a0,2005a688 <at_cpuload+0x20a>
2005a6b0:	27e010ef          	jal	ra,2005b92e <rtos_sema_give>
2005a6b4:	bfd1                	c.j	2005a688 <at_cpuload+0x20a>
2005a6b6:	ec41a783          	lw	a5,-316(gp) # 20066140 <top_exit_sema>
2005a6ba:	f7f9                	c.bnez	a5,2005a688 <at_cpuload+0x20a>
2005a6bc:	4088                	c.lw	a0,0(s1)
2005a6be:	6609                	c.lui	a2,0x2
2005a6c0:	0641                	c.addi	a2,16
2005a6c2:	4581                	c.li	a1,0
2005a6c4:	dffaf097          	auipc	ra,0xdffaf
2005a6c8:	40a080e7          	jalr	ra,1034(ra) # 9ace <__wrap_memset>
2005a6cc:	410187b7          	lui	a5,0x41018
2005a6d0:	4bd8                	c.lw	a4,20(a5)
2005a6d2:	eae1aa23          	sw	a4,-332(gp) # 20066130 <last_tick>
2005a6d6:	a75ff0ef          	jal	ra,2005a14a <update_status>
2005a6da:	b77d                	c.j	2005a688 <at_cpuload+0x20a>

2005a6dc <print_system_at>:
2005a6dc:	1141                	c.addi	sp,-16
2005a6de:	c422                	c.swsp	s0,8(sp)
2005a6e0:	ca818413          	addi	s0,gp,-856 # 20065f24 <at_sys_items>
2005a6e4:	c226                	c.swsp	s1,4(sp)
2005a6e6:	c04a                	c.swsp	s2,0(sp)
2005a6e8:	c606                	c.swsp	ra,12(sp)
2005a6ea:	0b040493          	addi	s1,s0,176
2005a6ee:	20062937          	lui	s2,0x20062
2005a6f2:	400c                	c.lw	a1,0(s0)
2005a6f4:	7fc90513          	addi	a0,s2,2044 # 200627fc <pmap_func+0xa0c>
2005a6f8:	0441                	c.addi	s0,16
2005a6fa:	205040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a6fe:	fe941ae3          	bne	s0,s1,2005a6f2 <print_system_at+0x16>
2005a702:	40b2                	c.lwsp	ra,12(sp)
2005a704:	4422                	c.lwsp	s0,8(sp)
2005a706:	4492                	c.lwsp	s1,4(sp)
2005a708:	4902                	c.lwsp	s2,0(sp)
2005a70a:	0141                	c.addi	sp,16
2005a70c:	8082                	c.jr	ra

2005a70e <at_list>:
2005a70e:	20063537          	lui	a0,0x20063
2005a712:	1141                	c.addi	sp,-16
2005a714:	80450513          	addi	a0,a0,-2044 # 20062804 <pmap_func+0xa14>
2005a718:	c606                	c.swsp	ra,12(sp)
2005a71a:	1e5040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005a71e:	3f7d                	c.jal	2005a6dc <print_system_at>
2005a720:	40b2                	c.lwsp	ra,12(sp)
2005a722:	200635b7          	lui	a1,0x20063
2005a726:	20062537          	lui	a0,0x20062
2005a72a:	81c58593          	addi	a1,a1,-2020 # 2006281c <pmap_func+0xa2c>
2005a72e:	22050513          	addi	a0,a0,544 # 20062220 <pmap_func+0x430>
2005a732:	0141                	c.addi	sp,16
2005a734:	1cb0406f          	jal	zero,2005f0fe <DiagPrintf_minimal>

2005a738 <at_sys_init>:
2005a738:	45ad                	c.li	a1,11
2005a73a:	ca818513          	addi	a0,gp,-856 # 20065f24 <at_sys_items>
2005a73e:	0330006f          	jal	zero,2005af70 <log_service_add_table>

2005a742 <BOOT_Image1>:
2005a742:	20000537          	lui	a0,0x20000
2005a746:	200017b7          	lui	a5,0x20001
2005a74a:	cac78793          	addi	a5,a5,-852 # 20000cac <__rom_bss_end__>
2005a74e:	00050613          	addi	a2,a0,0 # 20000000 <UserIrqFunTable>
2005a752:	1141                	c.addi	sp,-16
2005a754:	40c78633          	sub	a2,a5,a2
2005a758:	4581                	c.li	a1,0
2005a75a:	00050513          	addi	a0,a0,0
2005a75e:	c606                	c.swsp	ra,12(sp)
2005a760:	dffaf097          	auipc	ra,0xdffaf
2005a764:	02a080e7          	jalr	ra,42(ra) # 978a <_memset>
2005a768:	dffa6097          	auipc	ra,0xdffa6
2005a76c:	3ae080e7          	jalr	ra,942(ra) # b16 <CPU_ClkGet>
2005a770:	000f47b7          	lui	a5,0xf4
2005a774:	24078793          	addi	a5,a5,576 # f4240 <__ap_sram_heap_size+0xb4240>
2005a778:	02f55533          	divu	a0,a0,a5
2005a77c:	200017b7          	lui	a5,0x20001
2005a780:	86a7a023          	sw	a0,-1952(a5) # 20000860 <RBSS_UDELAY_CLK>
2005a784:	dffa6097          	auipc	ra,0xdffa6
2005a788:	1a0080e7          	jalr	ra,416(ra) # 924 <BOOT_ROM_InitDebugFlg>
2005a78c:	200587b7          	lui	a5,0x20058
2005a790:	04078793          	addi	a5,a5,64 # 20058040 <Img2EntryFun0>
2005a794:	0017c703          	lbu	a4,1(a5)
2005a798:	0007c683          	lbu	a3,0(a5)
2005a79c:	40b2                	c.lwsp	ra,12(sp)
2005a79e:	0722                	c.slli	a4,0x8
2005a7a0:	8ed9                	c.or	a3,a4
2005a7a2:	0027c703          	lbu	a4,2(a5)
2005a7a6:	0037c783          	lbu	a5,3(a5)
2005a7aa:	0742                	c.slli	a4,0x10
2005a7ac:	8f55                	c.or	a4,a3
2005a7ae:	07e2                	c.slli	a5,0x18
2005a7b0:	8fd9                	c.or	a5,a4
2005a7b2:	0141                	c.addi	sp,16
2005a7b4:	8782                	c.jr	a5

2005a7b6 <flash_read_word>:
2005a7b6:	1141                	c.addi	sp,-16
2005a7b8:	852e                	c.mv	a0,a1
2005a7ba:	4591                	c.li	a1,4
2005a7bc:	c606                	c.swsp	ra,12(sp)
2005a7be:	bb4fe0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005a7c2:	40b2                	c.lwsp	ra,12(sp)
2005a7c4:	4505                	c.li	a0,1
2005a7c6:	0141                	c.addi	sp,16
2005a7c8:	8082                	c.jr	ra

2005a7ca <os_heap_init>:
2005a7ca:	1141                	c.addi	sp,-16
2005a7cc:	c606                	c.swsp	ra,12(sp)
2005a7ce:	c422                	c.swsp	s0,8(sp)
2005a7d0:	0800                	c.addi4spn	s0,sp,16
2005a7d2:	00000793          	addi	a5,zero,0
2005a7d6:	85be                	c.mv	a1,a5
2005a7d8:	ca0007b7          	lui	a5,0xca000
2005a7dc:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a7e0:	20c9                	c.jal	2005a8a2 <os_heap_add>
2005a7e2:	ec81a703          	lw	a4,-312(gp) # 20066144 <total_heap_size>
2005a7e6:	00000793          	addi	a5,zero,0
2005a7ea:	973e                	c.add	a4,a5
2005a7ec:	ece1a423          	sw	a4,-312(gp) # 20066144 <total_heap_size>
2005a7f0:	67e1                	c.lui	a5,0x18
2005a7f2:	dc078793          	addi	a5,a5,-576 # 17dc0 <__bdram_heap_buffer_size__>
2005a7f6:	85be                	c.mv	a1,a5
2005a7f8:	200687b7          	lui	a5,0x20068
2005a7fc:	24078513          	addi	a0,a5,576 # 20068240 <__bdram_heap_buffer_start__>
2005a800:	204d                	c.jal	2005a8a2 <os_heap_add>
2005a802:	ec81a703          	lw	a4,-312(gp) # 20066144 <total_heap_size>
2005a806:	67e1                	c.lui	a5,0x18
2005a808:	dc078793          	addi	a5,a5,-576 # 17dc0 <__bdram_heap_buffer_size__>
2005a80c:	973e                	c.add	a4,a5
2005a80e:	ece1a423          	sw	a4,-312(gp) # 20066144 <total_heap_size>
2005a812:	00000793          	addi	a5,zero,0
2005a816:	85be                	c.mv	a1,a5
2005a818:	ca0007b7          	lui	a5,0xca000
2005a81c:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a820:	2049                	c.jal	2005a8a2 <os_heap_add>
2005a822:	ec81a703          	lw	a4,-312(gp) # 20066144 <total_heap_size>
2005a826:	00000793          	addi	a5,zero,0
2005a82a:	973e                	c.add	a4,a5
2005a82c:	ece1a423          	sw	a4,-312(gp) # 20066144 <total_heap_size>
2005a830:	00000793          	addi	a5,zero,0
2005a834:	85be                	c.mv	a1,a5
2005a836:	ca0007b7          	lui	a5,0xca000
2005a83a:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005a83e:	2095                	c.jal	2005a8a2 <os_heap_add>
2005a840:	ec81a703          	lw	a4,-312(gp) # 20066144 <total_heap_size>
2005a844:	00000793          	addi	a5,zero,0
2005a848:	973e                	c.add	a4,a5
2005a84a:	ece1a423          	sw	a4,-312(gp) # 20066144 <total_heap_size>
2005a84e:	ecc1c783          	lbu	a5,-308(gp) # 20066148 <heap_index>
2005a852:	c391                	c.beqz	a5,2005a856 <os_heap_init+0x8c>
2005a854:	2809                	c.jal	2005a866 <os_heap_sort>
2005a856:	44018513          	addi	a0,gp,1088 # 200666bc <xHeapRegions>
2005a85a:	2c15                	c.jal	2005aa8e <vPortDefineHeapRegions>
2005a85c:	0001                	c.addi	zero,0
2005a85e:	40b2                	c.lwsp	ra,12(sp)
2005a860:	4422                	c.lwsp	s0,8(sp)
2005a862:	0141                	c.addi	sp,16
2005a864:	8082                	c.jr	ra

2005a866 <os_heap_sort>:
2005a866:	4681                	c.li	a3,0
2005a868:	ecc1c703          	lbu	a4,-308(gp) # 20066148 <heap_index>
2005a86c:	177d                	c.addi	a4,-1
2005a86e:	02e6d963          	bge	a3,a4,2005a8a0 <os_heap_sort+0x3a>
2005a872:	44018793          	addi	a5,gp,1088 # 200666bc <xHeapRegions>
2005a876:	4601                	c.li	a2,0
2005a878:	8f15                	c.sub	a4,a3
2005a87a:	a839                	c.j	2005a898 <os_heap_sort+0x32>
2005a87c:	438c                	c.lw	a1,0(a5)
2005a87e:	4788                	c.lw	a0,8(a5)
2005a880:	0605                	c.addi	a2,1
2005a882:	00b57a63          	bgeu	a0,a1,2005a896 <os_heap_sort+0x30>
2005a886:	0047a303          	lw	t1,4(a5)
2005a88a:	c388                	c.sw	a0,0(a5)
2005a88c:	47c8                	c.lw	a0,12(a5)
2005a88e:	c78c                	c.sw	a1,8(a5)
2005a890:	0067a623          	sw	t1,12(a5)
2005a894:	c3c8                	c.sw	a0,4(a5)
2005a896:	07a1                	c.addi	a5,8
2005a898:	fee642e3          	blt	a2,a4,2005a87c <os_heap_sort+0x16>
2005a89c:	0685                	c.addi	a3,1
2005a89e:	b7e9                	c.j	2005a868 <os_heap_sort+0x2>
2005a8a0:	8082                	c.jr	ra

2005a8a2 <os_heap_add>:
2005a8a2:	1141                	c.addi	sp,-16
2005a8a4:	c426                	c.swsp	s1,8(sp)
2005a8a6:	ecc1c703          	lbu	a4,-308(gp) # 20066148 <heap_index>
2005a8aa:	c24a                	c.swsp	s2,4(sp)
2005a8ac:	c04e                	c.swsp	s3,0(sp)
2005a8ae:	c606                	c.swsp	ra,12(sp)
2005a8b0:	4795                	c.li	a5,5
2005a8b2:	89aa                	c.mv	s3,a0
2005a8b4:	892e                	c.mv	s2,a1
2005a8b6:	00e7fa63          	bgeu	a5,a4,2005a8ca <os_heap_add+0x28>
2005a8ba:	20063537          	lui	a0,0x20063
2005a8be:	02600593          	addi	a1,zero,38
2005a8c2:	89850513          	addi	a0,a0,-1896 # 20062898 <__FUNCTION__.0+0x10>
2005a8c6:	529040ef          	jal	ra,2005f5ee <io_assert_failed>
2005a8ca:	ecc1c703          	lbu	a4,-308(gp) # 20066148 <heap_index>
2005a8ce:	44018793          	addi	a5,gp,1088 # 200666bc <xHeapRegions>
2005a8d2:	00371693          	slli	a3,a4,0x3
2005a8d6:	97b6                	c.add	a5,a3
2005a8d8:	0137a023          	sw	s3,0(a5)
2005a8dc:	0127a223          	sw	s2,4(a5)
2005a8e0:	4501                	c.li	a0,0
2005a8e2:	00090663          	beq	s2,zero,2005a8ee <os_heap_add+0x4c>
2005a8e6:	0705                	c.addi	a4,1
2005a8e8:	ece18623          	sb	a4,-308(gp) # 20066148 <heap_index>
2005a8ec:	4505                	c.li	a0,1
2005a8ee:	40b2                	c.lwsp	ra,12(sp)
2005a8f0:	44a2                	c.lwsp	s1,8(sp)
2005a8f2:	4912                	c.lwsp	s2,4(sp)
2005a8f4:	4982                	c.lwsp	s3,0(sp)
2005a8f6:	0141                	c.addi	sp,16
2005a8f8:	8082                	c.jr	ra

2005a8fa <__wrap_malloc>:
2005a8fa:	5bb0006f          	jal	zero,2005b6b4 <rtos_mem_malloc>

2005a8fe <__wrap_free>:
2005a8fe:	5e10006f          	jal	zero,2005b6de <rtos_mem_free>

2005a902 <__wrap__free_r>:
2005a902:	852e                	c.mv	a0,a1
2005a904:	5db0006f          	jal	zero,2005b6de <rtos_mem_free>

2005a908 <prvInsertBlockIntoFreeList>:
2005a908:	ee818793          	addi	a5,gp,-280 # 20066164 <xStart>
2005a90c:	873e                	c.mv	a4,a5
2005a90e:	439c                	c.lw	a5,0(a5)
2005a910:	fea7eee3          	bltu	a5,a0,2005a90c <prvInsertBlockIntoFreeList+0x4>
2005a914:	4350                	c.lw	a2,4(a4)
2005a916:	00c706b3          	add	a3,a4,a2
2005a91a:	00d51663          	bne	a0,a3,2005a926 <prvInsertBlockIntoFreeList+0x1e>
2005a91e:	4154                	c.lw	a3,4(a0)
2005a920:	853a                	c.mv	a0,a4
2005a922:	96b2                	c.add	a3,a2
2005a924:	c354                	c.sw	a3,4(a4)
2005a926:	4154                	c.lw	a3,4(a0)
2005a928:	00d50633          	add	a2,a0,a3
2005a92c:	00c79b63          	bne	a5,a2,2005a942 <prvInsertBlockIntoFreeList+0x3a>
2005a930:	ed01a603          	lw	a2,-304(gp) # 2006614c <pxEnd>
2005a934:	00c78763          	beq	a5,a2,2005a942 <prvInsertBlockIntoFreeList+0x3a>
2005a938:	43dc                	c.lw	a5,4(a5)
2005a93a:	97b6                	c.add	a5,a3
2005a93c:	c15c                	c.sw	a5,4(a0)
2005a93e:	431c                	c.lw	a5,0(a4)
2005a940:	439c                	c.lw	a5,0(a5)
2005a942:	c11c                	c.sw	a5,0(a0)
2005a944:	00e50363          	beq	a0,a4,2005a94a <prvInsertBlockIntoFreeList+0x42>
2005a948:	c308                	c.sw	a0,0(a4)
2005a94a:	8082                	c.jr	ra

2005a94c <pvPortMalloc>:
2005a94c:	1101                	c.addi	sp,-32
2005a94e:	cc22                	c.swsp	s0,24(sp)
2005a950:	c452                	c.swsp	s4,8(sp)
2005a952:	842a                	c.mv	s0,a0
2005a954:	ce06                	c.swsp	ra,28(sp)
2005a956:	ca26                	c.swsp	s1,20(sp)
2005a958:	c84a                	c.swsp	s2,16(sp)
2005a95a:	c64e                	c.swsp	s3,12(sp)
2005a95c:	383020ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005a960:	ed41aa03          	lw	s4,-300(gp) # 20066150 <xBlockAllocatedBit>
2005a964:	008a77b3          	and	a5,s4,s0
2005a968:	c799                	c.beqz	a5,2005a976 <pvPortMalloc+0x2a>
2005a96a:	422030ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005a96e:	2ab010ef          	jal	ra,2005c418 <vApplicationMallocFailedHook>
2005a972:	4901                	c.li	s2,0
2005a974:	a879                	c.j	2005aa12 <pvPortMalloc+0xc6>
2005a976:	fff40793          	addi	a5,s0,-1
2005a97a:	fde00713          	addi	a4,zero,-34
2005a97e:	fef766e3          	bltu	a4,a5,2005a96a <pvPortMalloc+0x1e>
2005a982:	02040793          	addi	a5,s0,32
2005a986:	887d                	c.andi	s0,31
2005a988:	c801                	c.beqz	s0,2005a998 <pvPortMalloc+0x4c>
2005a98a:	fe07f713          	andi	a4,a5,-32
2005a98e:	02070713          	addi	a4,a4,32 # 4020 <GPIO_Direction+0x56>
2005a992:	fce7fce3          	bgeu	a5,a4,2005a96a <pvPortMalloc+0x1e>
2005a996:	87ba                	c.mv	a5,a4
2005a998:	ed81a483          	lw	s1,-296(gp) # 20066154 <xFreeBytesRemaining>
2005a99c:	ed818993          	addi	s3,gp,-296 # 20066154 <xFreeBytesRemaining>
2005a9a0:	fcf4e5e3          	bltu	s1,a5,2005a96a <pvPortMalloc+0x1e>
2005a9a4:	ee81a403          	lw	s0,-280(gp) # 20066164 <xStart>
2005a9a8:	ee818713          	addi	a4,gp,-280 # 20066164 <xStart>
2005a9ac:	4054                	c.lw	a3,4(s0)
2005a9ae:	00f6f463          	bgeu	a3,a5,2005a9b6 <pvPortMalloc+0x6a>
2005a9b2:	4014                	c.lw	a3,0(s0)
2005a9b4:	eaa5                	c.bnez	a3,2005aa24 <pvPortMalloc+0xd8>
2005a9b6:	ed01a683          	lw	a3,-304(gp) # 2006614c <pxEnd>
2005a9ba:	fa8688e3          	beq	a3,s0,2005a96a <pvPortMalloc+0x1e>
2005a9be:	4014                	c.lw	a3,0(s0)
2005a9c0:	00072903          	lw	s2,0(a4)
2005a9c4:	c314                	c.sw	a3,0(a4)
2005a9c6:	4054                	c.lw	a3,4(s0)
2005a9c8:	04000713          	addi	a4,zero,64
2005a9cc:	02090913          	addi	s2,s2,32
2005a9d0:	8e9d                	c.sub	a3,a5
2005a9d2:	00d77763          	bgeu	a4,a3,2005a9e0 <pvPortMalloc+0x94>
2005a9d6:	00f40533          	add	a0,s0,a5
2005a9da:	c154                	c.sw	a3,4(a0)
2005a9dc:	c05c                	c.sw	a5,4(s0)
2005a9de:	372d                	c.jal	2005a908 <prvInsertBlockIntoFreeList>
2005a9e0:	4058                	c.lw	a4,4(s0)
2005a9e2:	edc18693          	addi	a3,gp,-292 # 20066158 <xMinimumEverFreeBytesRemaining>
2005a9e6:	4290                	c.lw	a2,0(a3)
2005a9e8:	40e487b3          	sub	a5,s1,a4
2005a9ec:	00f9a023          	sw	a5,0(s3)
2005a9f0:	00c7f363          	bgeu	a5,a2,2005a9f6 <pvPortMalloc+0xaa>
2005a9f4:	c29c                	c.sw	a5,0(a3)
2005a9f6:	00ea6733          	or	a4,s4,a4
2005a9fa:	ee018793          	addi	a5,gp,-288 # 2006615c <xNumberOfSuccessfulAllocations>
2005a9fe:	c058                	c.sw	a4,4(s0)
2005aa00:	4398                	c.lw	a4,0(a5)
2005aa02:	00042023          	sw	zero,0(s0)
2005aa06:	0705                	c.addi	a4,1
2005aa08:	c398                	c.sw	a4,0(a5)
2005aa0a:	382030ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005aa0e:	f60900e3          	beq	s2,zero,2005a96e <pvPortMalloc+0x22>
2005aa12:	40f2                	c.lwsp	ra,28(sp)
2005aa14:	4462                	c.lwsp	s0,24(sp)
2005aa16:	44d2                	c.lwsp	s1,20(sp)
2005aa18:	49b2                	c.lwsp	s3,12(sp)
2005aa1a:	4a22                	c.lwsp	s4,8(sp)
2005aa1c:	854a                	c.mv	a0,s2
2005aa1e:	4942                	c.lwsp	s2,16(sp)
2005aa20:	6105                	c.addi16sp	sp,32
2005aa22:	8082                	c.jr	ra
2005aa24:	8722                	c.mv	a4,s0
2005aa26:	8436                	c.mv	s0,a3
2005aa28:	b751                	c.j	2005a9ac <pvPortMalloc+0x60>

2005aa2a <vPortFree>:
2005aa2a:	cd31                	c.beqz	a0,2005aa86 <vPortFree+0x5c>
2005aa2c:	1141                	c.addi	sp,-16
2005aa2e:	c422                	c.swsp	s0,8(sp)
2005aa30:	c606                	c.swsp	ra,12(sp)
2005aa32:	fe452703          	lw	a4,-28(a0)
2005aa36:	ed41a783          	lw	a5,-300(gp) # 20066150 <xBlockAllocatedBit>
2005aa3a:	842a                	c.mv	s0,a0
2005aa3c:	00f776b3          	and	a3,a4,a5
2005aa40:	ce9d                	c.beqz	a3,2005aa7e <vPortFree+0x54>
2005aa42:	fe052683          	lw	a3,-32(a0)
2005aa46:	ee85                	c.bnez	a3,2005aa7e <vPortFree+0x54>
2005aa48:	fff7c793          	xori	a5,a5,-1
2005aa4c:	8ff9                	c.and	a5,a4
2005aa4e:	fef52223          	sw	a5,-28(a0)
2005aa52:	28d020ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005aa56:	ed818793          	addi	a5,gp,-296 # 20066154 <xFreeBytesRemaining>
2005aa5a:	4394                	c.lw	a3,0(a5)
2005aa5c:	fe442703          	lw	a4,-28(s0)
2005aa60:	fe040513          	addi	a0,s0,-32
2005aa64:	9736                	c.add	a4,a3
2005aa66:	c398                	c.sw	a4,0(a5)
2005aa68:	3545                	c.jal	2005a908 <prvInsertBlockIntoFreeList>
2005aa6a:	ee418793          	addi	a5,gp,-284 # 20066160 <xNumberOfSuccessfulFrees>
2005aa6e:	4398                	c.lw	a4,0(a5)
2005aa70:	0705                	c.addi	a4,1
2005aa72:	c398                	c.sw	a4,0(a5)
2005aa74:	4422                	c.lwsp	s0,8(sp)
2005aa76:	40b2                	c.lwsp	ra,12(sp)
2005aa78:	0141                	c.addi	sp,16
2005aa7a:	3120306f          	jal	zero,2005dd8c <xTaskResumeAll>
2005aa7e:	40b2                	c.lwsp	ra,12(sp)
2005aa80:	4422                	c.lwsp	s0,8(sp)
2005aa82:	0141                	c.addi	sp,16
2005aa84:	8082                	c.jr	ra
2005aa86:	8082                	c.jr	ra

2005aa88 <xPortGetFreeHeapSize>:
2005aa88:	ed81a503          	lw	a0,-296(gp) # 20066154 <xFreeBytesRemaining>
2005aa8c:	8082                	c.jr	ra

2005aa8e <vPortDefineHeapRegions>:
2005aa8e:	ed01a583          	lw	a1,-304(gp) # 2006614c <pxEnd>
2005aa92:	4701                	c.li	a4,0
2005aa94:	4301                	c.li	t1,0
2005aa96:	4681                	c.li	a3,0
2005aa98:	ed018613          	addi	a2,gp,-304 # 2006614c <pxEnd>
2005aa9c:	ee818813          	addi	a6,gp,-280 # 20066164 <xStart>
2005aaa0:	415c                	c.lw	a5,4(a0)
2005aaa2:	ef81                	c.bnez	a5,2005aaba <vPortDefineHeapRegions+0x2c>
2005aaa4:	c311                	c.beqz	a4,2005aaa8 <vPortDefineHeapRegions+0x1a>
2005aaa6:	c20c                	c.sw	a1,0(a2)
2005aaa8:	ecd1ae23          	sw	a3,-292(gp) # 20066158 <xMinimumEverFreeBytesRemaining>
2005aaac:	ecd1ac23          	sw	a3,-296(gp) # 20066154 <xFreeBytesRemaining>
2005aab0:	80000737          	lui	a4,0x80000
2005aab4:	ece1aa23          	sw	a4,-300(gp) # 20066150 <xBlockAllocatedBit>
2005aab8:	8082                	c.jr	ra
2005aaba:	4118                	c.lw	a4,0(a0)
2005aabc:	01f77893          	andi	a7,a4,31
2005aac0:	00088a63          	beq	a7,zero,2005aad4 <vPortDefineHeapRegions+0x46>
2005aac4:	01f70893          	addi	a7,a4,31 # 8000001f <__ctrace_end__+0xb600001f>
2005aac8:	fe08f893          	andi	a7,a7,-32
2005aacc:	97ba                	c.add	a5,a4
2005aace:	411787b3          	sub	a5,a5,a7
2005aad2:	8746                	c.mv	a4,a7
2005aad4:	00031663          	bne	t1,zero,2005aae0 <vPortDefineHeapRegions+0x52>
2005aad8:	00e82023          	sw	a4,0(a6)
2005aadc:	00082223          	sw	zero,4(a6)
2005aae0:	97ba                	c.add	a5,a4
2005aae2:	1781                	c.addi	a5,-32
2005aae4:	9b81                	c.andi	a5,-32
2005aae6:	0007a223          	sw	zero,4(a5)
2005aaea:	0007a023          	sw	zero,0(a5)
2005aaee:	40e788b3          	sub	a7,a5,a4
2005aaf2:	01172223          	sw	a7,4(a4)
2005aaf6:	c31c                	c.sw	a5,0(a4)
2005aaf8:	c191                	c.beqz	a1,2005aafc <vPortDefineHeapRegions+0x6e>
2005aafa:	c198                	c.sw	a4,0(a1)
2005aafc:	4358                	c.lw	a4,4(a4)
2005aafe:	0305                	c.addi	t1,1
2005ab00:	0521                	c.addi	a0,8
2005ab02:	96ba                	c.add	a3,a4
2005ab04:	85be                	c.mv	a1,a5
2005ab06:	4705                	c.li	a4,1
2005ab08:	bf61                	c.j	2005aaa0 <vPortDefineHeapRegions+0x12>

2005ab0a <vPortGetHeapStats>:
2005ab0a:	1101                	c.addi	sp,-32
2005ab0c:	cc22                	c.swsp	s0,24(sp)
2005ab0e:	ce06                	c.swsp	ra,28(sp)
2005ab10:	ca26                	c.swsp	s1,20(sp)
2005ab12:	c84a                	c.swsp	s2,16(sp)
2005ab14:	c64e                	c.swsp	s3,12(sp)
2005ab16:	842a                	c.mv	s0,a0
2005ab18:	1c7020ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005ab1c:	ee81a783          	lw	a5,-280(gp) # 20066164 <xStart>
2005ab20:	cfb9                	c.beqz	a5,2005ab7e <vPortGetHeapStats+0x74>
2005ab22:	ed01a683          	lw	a3,-304(gp) # 2006614c <pxEnd>
2005ab26:	597d                	c.li	s2,-1
2005ab28:	4981                	c.li	s3,0
2005ab2a:	4481                	c.li	s1,0
2005ab2c:	43d8                	c.lw	a4,4(a5)
2005ab2e:	0485                	c.addi	s1,1
2005ab30:	00e9f363          	bgeu	s3,a4,2005ab36 <vPortGetHeapStats+0x2c>
2005ab34:	89ba                	c.mv	s3,a4
2005ab36:	c701                	c.beqz	a4,2005ab3e <vPortGetHeapStats+0x34>
2005ab38:	01277363          	bgeu	a4,s2,2005ab3e <vPortGetHeapStats+0x34>
2005ab3c:	893a                	c.mv	s2,a4
2005ab3e:	439c                	c.lw	a5,0(a5)
2005ab40:	fef696e3          	bne	a3,a5,2005ab2c <vPortGetHeapStats+0x22>
2005ab44:	248030ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005ab48:	01342223          	sw	s3,4(s0)
2005ab4c:	01242423          	sw	s2,8(s0)
2005ab50:	c444                	c.sw	s1,12(s0)
2005ab52:	6b3020ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005ab56:	ed81a783          	lw	a5,-296(gp) # 20066154 <xFreeBytesRemaining>
2005ab5a:	40f2                	c.lwsp	ra,28(sp)
2005ab5c:	44d2                	c.lwsp	s1,20(sp)
2005ab5e:	c01c                	c.sw	a5,0(s0)
2005ab60:	ee01a783          	lw	a5,-288(gp) # 2006615c <xNumberOfSuccessfulAllocations>
2005ab64:	4942                	c.lwsp	s2,16(sp)
2005ab66:	49b2                	c.lwsp	s3,12(sp)
2005ab68:	c85c                	c.sw	a5,20(s0)
2005ab6a:	ee41a783          	lw	a5,-284(gp) # 20066160 <xNumberOfSuccessfulFrees>
2005ab6e:	cc1c                	c.sw	a5,24(s0)
2005ab70:	edc1a783          	lw	a5,-292(gp) # 20066158 <xMinimumEverFreeBytesRemaining>
2005ab74:	c81c                	c.sw	a5,16(s0)
2005ab76:	4462                	c.lwsp	s0,24(sp)
2005ab78:	6105                	c.addi16sp	sp,32
2005ab7a:	6a50206f          	jal	zero,2005da1e <vTaskExitCritical>
2005ab7e:	597d                	c.li	s2,-1
2005ab80:	4981                	c.li	s3,0
2005ab82:	4481                	c.li	s1,0
2005ab84:	b7c1                	c.j	2005ab44 <vPortGetHeapStats+0x3a>

2005ab86 <vListInitialise>:
2005ab86:	00850793          	addi	a5,a0,8
2005ab8a:	577d                	c.li	a4,-1
2005ab8c:	c15c                	c.sw	a5,4(a0)
2005ab8e:	c518                	c.sw	a4,8(a0)
2005ab90:	c55c                	c.sw	a5,12(a0)
2005ab92:	c91c                	c.sw	a5,16(a0)
2005ab94:	00052023          	sw	zero,0(a0)
2005ab98:	8082                	c.jr	ra

2005ab9a <vListInitialiseItem>:
2005ab9a:	00052823          	sw	zero,16(a0)
2005ab9e:	8082                	c.jr	ra

2005aba0 <vListInsertEnd>:
2005aba0:	415c                	c.lw	a5,4(a0)
2005aba2:	c1dc                	c.sw	a5,4(a1)
2005aba4:	4798                	c.lw	a4,8(a5)
2005aba6:	c598                	c.sw	a4,8(a1)
2005aba8:	c34c                	c.sw	a1,4(a4)
2005abaa:	c78c                	c.sw	a1,8(a5)
2005abac:	c988                	c.sw	a0,16(a1)
2005abae:	411c                	c.lw	a5,0(a0)
2005abb0:	0785                	c.addi	a5,1
2005abb2:	c11c                	c.sw	a5,0(a0)
2005abb4:	8082                	c.jr	ra

2005abb6 <vListInsert>:
2005abb6:	4194                	c.lw	a3,0(a1)
2005abb8:	57fd                	c.li	a5,-1
2005abba:	00850713          	addi	a4,a0,8
2005abbe:	00f69d63          	bne	a3,a5,2005abd8 <vListInsert+0x22>
2005abc2:	491c                	c.lw	a5,16(a0)
2005abc4:	43d8                	c.lw	a4,4(a5)
2005abc6:	c1d8                	c.sw	a4,4(a1)
2005abc8:	c70c                	c.sw	a1,8(a4)
2005abca:	c59c                	c.sw	a5,8(a1)
2005abcc:	c3cc                	c.sw	a1,4(a5)
2005abce:	c988                	c.sw	a0,16(a1)
2005abd0:	411c                	c.lw	a5,0(a0)
2005abd2:	0785                	c.addi	a5,1
2005abd4:	c11c                	c.sw	a5,0(a0)
2005abd6:	8082                	c.jr	ra
2005abd8:	87ba                	c.mv	a5,a4
2005abda:	4358                	c.lw	a4,4(a4)
2005abdc:	4310                	c.lw	a2,0(a4)
2005abde:	fec6fde3          	bgeu	a3,a2,2005abd8 <vListInsert+0x22>
2005abe2:	b7cd                	c.j	2005abc4 <vListInsert+0xe>

2005abe4 <uxListRemove>:
2005abe4:	4154                	c.lw	a3,4(a0)
2005abe6:	4518                	c.lw	a4,8(a0)
2005abe8:	491c                	c.lw	a5,16(a0)
2005abea:	c698                	c.sw	a4,8(a3)
2005abec:	4154                	c.lw	a3,4(a0)
2005abee:	c354                	c.sw	a3,4(a4)
2005abf0:	43d8                	c.lw	a4,4(a5)
2005abf2:	00a71463          	bne	a4,a0,2005abfa <uxListRemove+0x16>
2005abf6:	4518                	c.lw	a4,8(a0)
2005abf8:	c3d8                	c.sw	a4,4(a5)
2005abfa:	00052823          	sw	zero,16(a0)
2005abfe:	4398                	c.lw	a4,0(a5)
2005ac00:	177d                	c.addi	a4,-1
2005ac02:	c398                	c.sw	a4,0(a5)
2005ac04:	4388                	c.lw	a0,0(a5)
2005ac06:	8082                	c.jr	ra

2005ac08 <rtk_log_array_print>:
2005ac08:	1101                	c.addi	sp,-32
2005ac0a:	cc22                	c.swsp	s0,24(sp)
2005ac0c:	c84a                	c.swsp	s2,16(sp)
2005ac0e:	ce06                	c.swsp	ra,28(sp)
2005ac10:	ca26                	c.swsp	s1,20(sp)
2005ac12:	c64e                	c.swsp	s3,12(sp)
2005ac14:	ef018793          	addi	a5,gp,-272 # 2006616c <rtk_log_entry_count>
2005ac18:	4394                	c.lw	a3,0(a5)
2005ac1a:	470d                	c.li	a4,3
2005ac1c:	842a                	c.mv	s0,a0
2005ac1e:	4911                	c.li	s2,4
2005ac20:	00d76463          	bltu	a4,a3,2005ac28 <rtk_log_array_print+0x20>
2005ac24:	0007a903          	lw	s2,0(a5)
2005ac28:	557d                	c.li	a0,-1
2005ac2a:	c801                	c.beqz	s0,2005ac3a <rtk_log_array_print+0x32>
2005ac2c:	0405                	c.addi	s0,1
2005ac2e:	4481                	c.li	s1,0
2005ac30:	200639b7          	lui	s3,0x20063
2005ac34:	01249a63          	bne	s1,s2,2005ac48 <rtk_log_array_print+0x40>
2005ac38:	4501                	c.li	a0,0
2005ac3a:	40f2                	c.lwsp	ra,28(sp)
2005ac3c:	4462                	c.lwsp	s0,24(sp)
2005ac3e:	44d2                	c.lwsp	s1,20(sp)
2005ac40:	4942                	c.lwsp	s2,16(sp)
2005ac42:	49b2                	c.lwsp	s3,12(sp)
2005ac44:	6105                	c.addi16sp	sp,32
2005ac46:	8082                	c.jr	ra
2005ac48:	fff44603          	lbu	a2,-1(s0)
2005ac4c:	85a2                	c.mv	a1,s0
2005ac4e:	93498513          	addi	a0,s3,-1740 # 20062934 <__FUNCTION__.0+0xac>
2005ac52:	4ac040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005ac56:	0485                	c.addi	s1,1
2005ac58:	042d                	c.addi	s0,11
2005ac5a:	bfe9                	c.j	2005ac34 <rtk_log_array_print+0x2c>

2005ac5c <rtk_log_array_clear>:
2005ac5c:	1141                	c.addi	sp,-16
2005ac5e:	02c00613          	addi	a2,zero,44
2005ac62:	4581                	c.li	a1,0
2005ac64:	47018513          	addi	a0,gp,1136 # 200666ec <rtk_log_tag_array>
2005ac68:	c606                	c.swsp	ra,12(sp)
2005ac6a:	dffaf097          	auipc	ra,0xdffaf
2005ac6e:	b20080e7          	jalr	ra,-1248(ra) # 978a <_memset>
2005ac72:	40b2                	c.lwsp	ra,12(sp)
2005ac74:	ee01a823          	sw	zero,-272(gp) # 2006616c <rtk_log_entry_count>
2005ac78:	0141                	c.addi	sp,16
2005ac7a:	8082                	c.jr	ra

2005ac7c <rtk_log_level_get>:
2005ac7c:	1101                	c.addi	sp,-32
2005ac7e:	c64e                	c.swsp	s3,12(sp)
2005ac80:	c452                	c.swsp	s4,8(sp)
2005ac82:	ce06                	c.swsp	ra,28(sp)
2005ac84:	cc22                	c.swsp	s0,24(sp)
2005ac86:	ca26                	c.swsp	s1,20(sp)
2005ac88:	c84a                	c.swsp	s2,16(sp)
2005ac8a:	ef018793          	addi	a5,gp,-272 # 2006616c <rtk_log_entry_count>
2005ac8e:	4394                	c.lw	a3,0(a5)
2005ac90:	470d                	c.li	a4,3
2005ac92:	89aa                	c.mv	s3,a0
2005ac94:	4a11                	c.li	s4,4
2005ac96:	00d76463          	bltu	a4,a3,2005ac9e <rtk_log_level_get+0x22>
2005ac9a:	0007aa03          	lw	s4,0(a5)
2005ac9e:	00099a63          	bne	s3,zero,2005acb2 <rtk_log_level_get+0x36>
2005aca2:	20063537          	lui	a0,0x20063
2005aca6:	05000593          	addi	a1,zero,80
2005acaa:	94850513          	addi	a0,a0,-1720 # 20062948 <__FUNCTION__.0+0xc0>
2005acae:	141040ef          	jal	ra,2005f5ee <io_assert_failed>
2005acb2:	47018913          	addi	s2,gp,1136 # 200666ec <rtk_log_tag_array>
2005acb6:	0905                	c.addi	s2,1
2005acb8:	4401                	c.li	s0,0
2005acba:	47018493          	addi	s1,gp,1136 # 200666ec <rtk_log_tag_array>
2005acbe:	01441563          	bne	s0,s4,2005acc8 <rtk_log_level_get+0x4c>
2005acc2:	e841c503          	lbu	a0,-380(gp) # 20066100 <rtk_log_default_level>
2005acc6:	a839                	c.j	2005ace4 <rtk_log_level_get+0x68>
2005acc8:	854a                	c.mv	a0,s2
2005acca:	85ce                	c.mv	a1,s3
2005accc:	dffaf097          	auipc	ra,0xdffaf
2005acd0:	b60080e7          	jalr	ra,-1184(ra) # 982c <_strcmp>
2005acd4:	092d                	c.addi	s2,11
2005acd6:	ed19                	c.bnez	a0,2005acf4 <rtk_log_level_get+0x78>
2005acd8:	47ad                	c.li	a5,11
2005acda:	02f40433          	mul	s0,s0,a5
2005acde:	9426                	c.add	s0,s1
2005ace0:	00044503          	lbu	a0,0(s0)
2005ace4:	40f2                	c.lwsp	ra,28(sp)
2005ace6:	4462                	c.lwsp	s0,24(sp)
2005ace8:	44d2                	c.lwsp	s1,20(sp)
2005acea:	4942                	c.lwsp	s2,16(sp)
2005acec:	49b2                	c.lwsp	s3,12(sp)
2005acee:	4a22                	c.lwsp	s4,8(sp)
2005acf0:	6105                	c.addi16sp	sp,32
2005acf2:	8082                	c.jr	ra
2005acf4:	0405                	c.addi	s0,1
2005acf6:	b7e1                	c.j	2005acbe <rtk_log_level_get+0x42>

2005acf8 <rtk_log_level_set>:
2005acf8:	1101                	c.addi	sp,-32
2005acfa:	cc22                	c.swsp	s0,24(sp)
2005acfc:	ef018793          	addi	a5,gp,-272 # 2006616c <rtk_log_entry_count>
2005ad00:	c84a                	c.swsp	s2,16(sp)
2005ad02:	c64e                	c.swsp	s3,12(sp)
2005ad04:	c05a                	c.swsp	s6,0(sp)
2005ad06:	ce06                	c.swsp	ra,28(sp)
2005ad08:	ca26                	c.swsp	s1,20(sp)
2005ad0a:	c452                	c.swsp	s4,8(sp)
2005ad0c:	c256                	c.swsp	s5,4(sp)
2005ad0e:	4398                	c.lw	a4,0(a5)
2005ad10:	478d                	c.li	a5,3
2005ad12:	892a                	c.mv	s2,a0
2005ad14:	89ae                	c.mv	s3,a1
2005ad16:	ef018413          	addi	s0,gp,-272 # 2006616c <rtk_log_entry_count>
2005ad1a:	4b11                	c.li	s6,4
2005ad1c:	00e7e463          	bltu	a5,a4,2005ad24 <rtk_log_level_set+0x2c>
2005ad20:	00042b03          	lw	s6,0(s0)
2005ad24:	557d                	c.li	a0,-1
2005ad26:	06090b63          	beq	s2,zero,2005ad9c <rtk_log_level_set+0xa4>
2005ad2a:	4795                	c.li	a5,5
2005ad2c:	0737e863          	bltu	a5,s3,2005ad9c <rtk_log_level_set+0xa4>
2005ad30:	200635b7          	lui	a1,0x20063
2005ad34:	9dc58593          	addi	a1,a1,-1572 # 200629dc <__FUNCTION__.0+0x154>
2005ad38:	854a                	c.mv	a0,s2
2005ad3a:	dffaf097          	auipc	ra,0xdffaf
2005ad3e:	af2080e7          	jalr	ra,-1294(ra) # 982c <_strcmp>
2005ad42:	c939                	c.beqz	a0,2005ad98 <rtk_log_level_set+0xa0>
2005ad44:	47018a93          	addi	s5,gp,1136 # 200666ec <rtk_log_tag_array>
2005ad48:	0a85                	c.addi	s5,1
2005ad4a:	4a01                	c.li	s4,0
2005ad4c:	47018493          	addi	s1,gp,1136 # 200666ec <rtk_log_tag_array>
2005ad50:	076a1063          	bne	s4,s6,2005adb0 <rtk_log_level_set+0xb8>
2005ad54:	4018                	c.lw	a4,0(s0)
2005ad56:	478d                	c.li	a5,3
2005ad58:	00e7f863          	bgeu	a5,a4,2005ad68 <rtk_log_level_set+0x70>
2005ad5c:	20063537          	lui	a0,0x20063
2005ad60:	9e050513          	addi	a0,a0,-1568 # 200629e0 <__FUNCTION__.0+0x158>
2005ad64:	39a040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005ad68:	4018                	c.lw	a4,0(s0)
2005ad6a:	401c                	c.lw	a5,0(s0)
2005ad6c:	452d                	c.li	a0,11
2005ad6e:	8b0d                	c.andi	a4,3
2005ad70:	8b8d                	c.andi	a5,3
2005ad72:	02a70733          	mul	a4,a4,a0
2005ad76:	4625                	c.li	a2,9
2005ad78:	85ca                	c.mv	a1,s2
2005ad7a:	02a787b3          	mul	a5,a5,a0
2005ad7e:	9726                	c.add	a4,s1
2005ad80:	01370023          	sb	s3,0(a4)
2005ad84:	0785                	c.addi	a5,1
2005ad86:	00f48533          	add	a0,s1,a5
2005ad8a:	223060ef          	jal	ra,200617ac <strncpy>
2005ad8e:	401c                	c.lw	a5,0(s0)
2005ad90:	4501                	c.li	a0,0
2005ad92:	0785                	c.addi	a5,1
2005ad94:	c01c                	c.sw	a5,0(s0)
2005ad96:	a019                	c.j	2005ad9c <rtk_log_level_set+0xa4>
2005ad98:	e9318223          	sb	s3,-380(gp) # 20066100 <rtk_log_default_level>
2005ad9c:	40f2                	c.lwsp	ra,28(sp)
2005ad9e:	4462                	c.lwsp	s0,24(sp)
2005ada0:	44d2                	c.lwsp	s1,20(sp)
2005ada2:	4942                	c.lwsp	s2,16(sp)
2005ada4:	49b2                	c.lwsp	s3,12(sp)
2005ada6:	4a22                	c.lwsp	s4,8(sp)
2005ada8:	4a92                	c.lwsp	s5,4(sp)
2005adaa:	4b02                	c.lwsp	s6,0(sp)
2005adac:	6105                	c.addi16sp	sp,32
2005adae:	8082                	c.jr	ra
2005adb0:	8556                	c.mv	a0,s5
2005adb2:	85ca                	c.mv	a1,s2
2005adb4:	dffaf097          	auipc	ra,0xdffaf
2005adb8:	a78080e7          	jalr	ra,-1416(ra) # 982c <_strcmp>
2005adbc:	0aad                	c.addi	s5,11
2005adbe:	e901                	c.bnez	a0,2005adce <rtk_log_level_set+0xd6>
2005adc0:	47ad                	c.li	a5,11
2005adc2:	02fa0a33          	mul	s4,s4,a5
2005adc6:	94d2                	c.add	s1,s4
2005adc8:	01348023          	sb	s3,0(s1)
2005adcc:	bfc1                	c.j	2005ad9c <rtk_log_level_set+0xa4>
2005adce:	0a05                	c.addi	s4,1
2005add0:	b741                	c.j	2005ad50 <rtk_log_level_set+0x58>

2005add2 <rtk_log_memory_dump_word>:
2005add2:	1101                	c.addi	sp,-32
2005add4:	cc22                	c.swsp	s0,24(sp)
2005add6:	ca26                	c.swsp	s1,20(sp)
2005add8:	c84a                	c.swsp	s2,16(sp)
2005adda:	c64e                	c.swsp	s3,12(sp)
2005addc:	c452                	c.swsp	s4,8(sp)
2005adde:	c256                	c.swsp	s5,4(sp)
2005ade0:	c05a                	c.swsp	s6,0(sp)
2005ade2:	ce06                	c.swsp	ra,28(sp)
2005ade4:	892a                	c.mv	s2,a0
2005ade6:	89ae                	c.mv	s3,a1
2005ade8:	84aa                	c.mv	s1,a0
2005adea:	4401                	c.li	s0,0
2005adec:	20063a37          	lui	s4,0x20063
2005adf0:	20063ab7          	lui	s5,0x20063
2005adf4:	20063b37          	lui	s6,0x20063
2005adf8:	03341163          	bne	s0,s3,2005ae1a <rtk_log_memory_dump_word+0x48>
2005adfc:	4462                	c.lwsp	s0,24(sp)
2005adfe:	40f2                	c.lwsp	ra,28(sp)
2005ae00:	44d2                	c.lwsp	s1,20(sp)
2005ae02:	4942                	c.lwsp	s2,16(sp)
2005ae04:	49b2                	c.lwsp	s3,12(sp)
2005ae06:	4a22                	c.lwsp	s4,8(sp)
2005ae08:	4a92                	c.lwsp	s5,4(sp)
2005ae0a:	4b02                	c.lwsp	s6,0(sp)
2005ae0c:	20062537          	lui	a0,0x20062
2005ae10:	27050513          	addi	a0,a0,624 # 20062270 <pmap_func+0x480>
2005ae14:	6105                	c.addi16sp	sp,32
2005ae16:	2e80406f          	jal	zero,2005f0fe <DiagPrintf_minimal>
2005ae1a:	ec11                	c.bnez	s0,2005ae36 <rtk_log_memory_dump_word+0x64>
2005ae1c:	85ca                	c.mv	a1,s2
2005ae1e:	a0ca8513          	addi	a0,s5,-1524 # 20062a0c <__FUNCTION__.0+0x184>
2005ae22:	2dc040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005ae26:	408c                	c.lw	a1,0(s1)
2005ae28:	a20b0513          	addi	a0,s6,-1504 # 20062a20 <__FUNCTION__.0+0x198>
2005ae2c:	0405                	c.addi	s0,1
2005ae2e:	2d0040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005ae32:	0491                	c.addi	s1,4
2005ae34:	b7d1                	c.j	2005adf8 <rtk_log_memory_dump_word+0x26>
2005ae36:	00747793          	andi	a5,s0,7
2005ae3a:	f7f5                	c.bnez	a5,2005ae26 <rtk_log_memory_dump_word+0x54>
2005ae3c:	85a6                	c.mv	a1,s1
2005ae3e:	a14a0513          	addi	a0,s4,-1516 # 20062a14 <__FUNCTION__.0+0x18c>
2005ae42:	b7c5                	c.j	2005ae22 <rtk_log_memory_dump_word+0x50>

2005ae44 <rtk_log_memory_dump_byte>:
2005ae44:	1101                	c.addi	sp,-32
2005ae46:	cc22                	c.swsp	s0,24(sp)
2005ae48:	ca26                	c.swsp	s1,20(sp)
2005ae4a:	c84a                	c.swsp	s2,16(sp)
2005ae4c:	c452                	c.swsp	s4,8(sp)
2005ae4e:	c256                	c.swsp	s5,4(sp)
2005ae50:	c05a                	c.swsp	s6,0(sp)
2005ae52:	ce06                	c.swsp	ra,28(sp)
2005ae54:	c64e                	c.swsp	s3,12(sp)
2005ae56:	84aa                	c.mv	s1,a0
2005ae58:	892e                	c.mv	s2,a1
2005ae5a:	4401                	c.li	s0,0
2005ae5c:	20063a37          	lui	s4,0x20063
2005ae60:	20063ab7          	lui	s5,0x20063
2005ae64:	20063b37          	lui	s6,0x20063
2005ae68:	03241163          	bne	s0,s2,2005ae8a <rtk_log_memory_dump_byte+0x46>
2005ae6c:	4462                	c.lwsp	s0,24(sp)
2005ae6e:	40f2                	c.lwsp	ra,28(sp)
2005ae70:	44d2                	c.lwsp	s1,20(sp)
2005ae72:	4942                	c.lwsp	s2,16(sp)
2005ae74:	49b2                	c.lwsp	s3,12(sp)
2005ae76:	4a22                	c.lwsp	s4,8(sp)
2005ae78:	4a92                	c.lwsp	s5,4(sp)
2005ae7a:	4b02                	c.lwsp	s6,0(sp)
2005ae7c:	20062537          	lui	a0,0x20062
2005ae80:	27050513          	addi	a0,a0,624 # 20062270 <pmap_func+0x480>
2005ae84:	6105                	c.addi16sp	sp,32
2005ae86:	2780406f          	jal	zero,2005f0fe <DiagPrintf_minimal>
2005ae8a:	008489b3          	add	s3,s1,s0
2005ae8e:	ec11                	c.bnez	s0,2005aeaa <rtk_log_memory_dump_byte+0x66>
2005ae90:	85a6                	c.mv	a1,s1
2005ae92:	a0ca8513          	addi	a0,s5,-1524 # 20062a0c <__FUNCTION__.0+0x184>
2005ae96:	268040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005ae9a:	0009c583          	lbu	a1,0(s3)
2005ae9e:	a28b0513          	addi	a0,s6,-1496 # 20062a28 <__FUNCTION__.0+0x1a0>
2005aea2:	0405                	c.addi	s0,1
2005aea4:	25a040ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005aea8:	b7c1                	c.j	2005ae68 <rtk_log_memory_dump_byte+0x24>
2005aeaa:	00747793          	andi	a5,s0,7
2005aeae:	f7f5                	c.bnez	a5,2005ae9a <rtk_log_memory_dump_byte+0x56>
2005aeb0:	85ce                	c.mv	a1,s3
2005aeb2:	a14a0513          	addi	a0,s4,-1516 # 20062a14 <__FUNCTION__.0+0x18c>
2005aeb6:	b7c5                	c.j	2005ae96 <rtk_log_memory_dump_byte+0x52>

2005aeb8 <rtk_log_write>:
2005aeb8:	7139                	c.addi16sp	sp,-64
2005aeba:	d226                	c.swsp	s1,36(sp)
2005aebc:	84aa                	c.mv	s1,a0
2005aebe:	852e                	c.mv	a0,a1
2005aec0:	d422                	c.swsp	s0,40(sp)
2005aec2:	d606                	c.swsp	ra,44(sp)
2005aec4:	c632                	c.swsp	a2,12(sp)
2005aec6:	8436                	c.mv	s0,a3
2005aec8:	d83a                	c.swsp	a4,48(sp)
2005aeca:	da3e                	c.swsp	a5,52(sp)
2005aecc:	dc42                	c.swsp	a6,56(sp)
2005aece:	de46                	c.swsp	a7,60(sp)
2005aed0:	c42e                	c.swsp	a1,8(sp)
2005aed2:	336d                	c.jal	2005ac7c <rtk_log_level_get>
2005aed4:	02956563          	bltu	a0,s1,2005aefe <rtk_log_write+0x46>
2005aed8:	45a2                	c.lwsp	a1,8(sp)
2005aeda:	02300793          	addi	a5,zero,35
2005aede:	0005c703          	lbu	a4,0(a1)
2005aee2:	00f70963          	beq	a4,a5,2005aef4 <rtk_log_write+0x3c>
2005aee6:	4632                	c.lwsp	a2,12(sp)
2005aee8:	20063537          	lui	a0,0x20063
2005aeec:	a3050513          	addi	a0,a0,-1488 # 20062a30 <__FUNCTION__.0+0x1a8>
2005aef0:	1ee040ef          	jal	ra,2005f0de <DiagPrintf>
2005aef4:	180c                	c.addi4spn	a1,sp,48
2005aef6:	8522                	c.mv	a0,s0
2005aef8:	ce2e                	c.swsp	a1,28(sp)
2005aefa:	6e7030ef          	jal	ra,2005ede0 <DiagVprintf>
2005aefe:	50b2                	c.lwsp	ra,44(sp)
2005af00:	5422                	c.lwsp	s0,40(sp)
2005af02:	5492                	c.lwsp	s1,36(sp)
2005af04:	6121                	c.addi16sp	sp,64
2005af06:	8082                	c.jr	ra

2005af08 <hash_index>:
2005af08:	4781                	c.li	a5,0
2005af0a:	08300693          	addi	a3,zero,131
2005af0e:	00054703          	lbu	a4,0(a0)
2005af12:	e709                	c.bnez	a4,2005af1c <hash_index+0x14>
2005af14:	00179513          	slli	a0,a5,0x1
2005af18:	8105                	c.srli	a0,0x1
2005af1a:	8082                	c.jr	ra
2005af1c:	02d787b3          	mul	a5,a5,a3
2005af20:	0505                	c.addi	a0,1
2005af22:	97ba                	c.add	a5,a4
2005af24:	b7ed                	c.j	2005af0e <hash_index+0x6>

2005af26 <log_add_new_command>:
2005af26:	1141                	c.addi	sp,-16
2005af28:	c422                	c.swsp	s0,8(sp)
2005af2a:	c606                	c.swsp	ra,12(sp)
2005af2c:	842a                	c.mv	s0,a0
2005af2e:	4108                	c.lw	a0,0(a0)
2005af30:	3fe1                	c.jal	2005af08 <hash_index>
2005af32:	02000793          	addi	a5,zero,32
2005af36:	02f56533          	rem	a0,a0,a5
2005af3a:	49c18793          	addi	a5,gp,1180 # 20066718 <log_hash>
2005af3e:	00840713          	addi	a4,s0,8
2005af42:	050e                	c.slli	a0,0x3
2005af44:	97aa                	c.add	a5,a0
2005af46:	4394                	c.lw	a3,0(a5)
2005af48:	c2d8                	c.sw	a4,4(a3)
2005af4a:	c414                	c.sw	a3,8(s0)
2005af4c:	c45c                	c.sw	a5,12(s0)
2005af4e:	c398                	c.sw	a4,0(a5)
2005af50:	40b2                	c.lwsp	ra,12(sp)
2005af52:	4422                	c.lwsp	s0,8(sp)
2005af54:	0141                	c.addi	sp,16
2005af56:	8082                	c.jr	ra

2005af58 <log_service_init>:
2005af58:	49c18793          	addi	a5,gp,1180 # 20066718 <log_hash>
2005af5c:	10078713          	addi	a4,a5,256
2005af60:	c39c                	c.sw	a5,0(a5)
2005af62:	c3dc                	c.sw	a5,4(a5)
2005af64:	07a1                	c.addi	a5,8
2005af66:	fee79de3          	bne	a5,a4,2005af60 <log_service_init+0x8>
2005af6a:	e881a783          	lw	a5,-376(gp) # 20066104 <log_init_table>
2005af6e:	8782                	c.jr	a5

2005af70 <log_service_add_table>:
2005af70:	1141                	c.addi	sp,-16
2005af72:	c422                	c.swsp	s0,8(sp)
2005af74:	c226                	c.swsp	s1,4(sp)
2005af76:	c04a                	c.swsp	s2,0(sp)
2005af78:	c606                	c.swsp	ra,12(sp)
2005af7a:	892a                	c.mv	s2,a0
2005af7c:	84ae                	c.mv	s1,a1
2005af7e:	4401                	c.li	s0,0
2005af80:	00944863          	blt	s0,s1,2005af90 <log_service_add_table+0x20>
2005af84:	40b2                	c.lwsp	ra,12(sp)
2005af86:	4422                	c.lwsp	s0,8(sp)
2005af88:	4492                	c.lwsp	s1,4(sp)
2005af8a:	4902                	c.lwsp	s2,0(sp)
2005af8c:	0141                	c.addi	sp,16
2005af8e:	8082                	c.jr	ra
2005af90:	00441513          	slli	a0,s0,0x4
2005af94:	954a                	c.add	a0,s2
2005af96:	3f41                	c.jal	2005af26 <log_add_new_command>
2005af98:	0405                	c.addi	s0,1
2005af9a:	b7dd                	c.j	2005af80 <log_service_add_table+0x10>

2005af9c <log_action>:
2005af9c:	1141                	c.addi	sp,-16
2005af9e:	c422                	c.swsp	s0,8(sp)
2005afa0:	c226                	c.swsp	s1,4(sp)
2005afa2:	c04a                	c.swsp	s2,0(sp)
2005afa4:	c606                	c.swsp	ra,12(sp)
2005afa6:	892a                	c.mv	s2,a0
2005afa8:	02000413          	addi	s0,zero,32
2005afac:	3fb1                	c.jal	2005af08 <hash_index>
2005afae:	02856433          	rem	s0,a0,s0
2005afb2:	00341793          	slli	a5,s0,0x3
2005afb6:	49c18413          	addi	s0,gp,1180 # 20066718 <log_hash>
2005afba:	943e                	c.add	s0,a5
2005afbc:	4004                	c.lw	s1,0(s0)
2005afbe:	00849463          	bne	s1,s0,2005afc6 <log_action+0x2a>
2005afc2:	4501                	c.li	a0,0
2005afc4:	a819                	c.j	2005afda <log_action+0x3e>
2005afc6:	ff84a503          	lw	a0,-8(s1)
2005afca:	85ca                	c.mv	a1,s2
2005afcc:	dffaf097          	auipc	ra,0xdffaf
2005afd0:	b0e080e7          	jalr	ra,-1266(ra) # 9ada <__wrap_strcmp>
2005afd4:	e909                	c.bnez	a0,2005afe6 <log_action+0x4a>
2005afd6:	ffc4a503          	lw	a0,-4(s1)
2005afda:	40b2                	c.lwsp	ra,12(sp)
2005afdc:	4422                	c.lwsp	s0,8(sp)
2005afde:	4492                	c.lwsp	s1,4(sp)
2005afe0:	4902                	c.lwsp	s2,0(sp)
2005afe2:	0141                	c.addi	sp,16
2005afe4:	8082                	c.jr	ra
2005afe6:	4084                	c.lw	s1,0(s1)
2005afe8:	bfd9                	c.j	2005afbe <log_action+0x22>

2005afea <log_handler>:
2005afea:	7135                	c.addi16sp	sp,-160
2005afec:	cd22                	c.swsp	s0,152(sp)
2005afee:	1060                	c.addi4spn	s0,sp,44
2005aff0:	cb26                	c.swsp	s1,148(sp)
2005aff2:	06400613          	addi	a2,zero,100
2005aff6:	84aa                	c.mv	s1,a0
2005aff8:	4581                	c.li	a1,0
2005affa:	8522                	c.mv	a0,s0
2005affc:	cf06                	c.swsp	ra,156(sp)
2005affe:	dffaf097          	auipc	ra,0xdffaf
2005b002:	ad0080e7          	jalr	ra,-1328(ra) # 9ace <__wrap_memset>
2005b006:	4675                	c.li	a2,29
2005b008:	4581                	c.li	a1,0
2005b00a:	0068                	c.addi4spn	a0,sp,12
2005b00c:	c222                	c.swsp	s0,4(sp)
2005b00e:	c402                	c.swsp	zero,8(sp)
2005b010:	dffaf097          	auipc	ra,0xdffaf
2005b014:	abe080e7          	jalr	ra,-1346(ra) # 9ace <__wrap_memset>
2005b018:	06300613          	addi	a2,zero,99
2005b01c:	85a6                	c.mv	a1,s1
2005b01e:	8522                	c.mv	a0,s0
2005b020:	78c060ef          	jal	ra,200617ac <strncpy>
2005b024:	200635b7          	lui	a1,0x20063
2005b028:	a3c58593          	addi	a1,a1,-1476 # 20062a3c <__FUNCTION__.0+0x1b4>
2005b02c:	0048                	c.addi4spn	a0,sp,4
2005b02e:	7e8060ef          	jal	ra,20061816 <strsep>
2005b032:	200635b7          	lui	a1,0x20063
2005b036:	842a                	c.mv	s0,a0
2005b038:	a9858593          	addi	a1,a1,-1384 # 20062a98 <__FUNCTION__.0+0x210>
2005b03c:	0048                	c.addi4spn	a0,sp,4
2005b03e:	7d8060ef          	jal	ra,20061816 <strsep>
2005b042:	e419                	c.bnez	s0,2005b050 <log_handler+0x66>
2005b044:	4501                	c.li	a0,0
2005b046:	40fa                	c.lwsp	ra,156(sp)
2005b048:	446a                	c.lwsp	s0,152(sp)
2005b04a:	44da                	c.lwsp	s1,148(sp)
2005b04c:	610d                	c.addi16sp	sp,160
2005b04e:	8082                	c.jr	ra
2005b050:	84aa                	c.mv	s1,a0
2005b052:	8522                	c.mv	a0,s0
2005b054:	dffaf097          	auipc	ra,0xdffaf
2005b058:	a84080e7          	jalr	ra,-1404(ra) # 9ad8 <__wrap_strlen>
2005b05c:	478d                	c.li	a5,3
2005b05e:	fea7f3e3          	bgeu	a5,a0,2005b044 <log_handler+0x5a>
2005b062:	02000613          	addi	a2,zero,32
2005b066:	85a2                	c.mv	a1,s0
2005b068:	0028                	c.addi4spn	a0,sp,8
2005b06a:	742060ef          	jal	ra,200617ac <strncpy>
2005b06e:	00814703          	lbu	a4,8(sp)
2005b072:	04100793          	addi	a5,zero,65
2005b076:	fcf717e3          	bne	a4,a5,2005b044 <log_handler+0x5a>
2005b07a:	00914703          	lbu	a4,9(sp)
2005b07e:	05400793          	addi	a5,zero,84
2005b082:	fcf711e3          	bne	a4,a5,2005b044 <log_handler+0x5a>
2005b086:	00a10513          	addi	a0,sp,10
2005b08a:	3f09                	c.jal	2005af9c <log_action>
2005b08c:	842a                	c.mv	s0,a0
2005b08e:	4501                	c.li	a0,0
2005b090:	d85d                	c.beqz	s0,2005b046 <log_handler+0x5c>
2005b092:	8526                	c.mv	a0,s1
2005b094:	9402                	c.jalr	s0
2005b096:	8522                	c.mv	a0,s0
2005b098:	b77d                	c.j	2005b046 <log_handler+0x5c>

2005b09a <parse_param>:
2005b09a:	7179                	c.addi16sp	sp,-48
2005b09c:	d422                	c.swsp	s0,40(sp)
2005b09e:	d606                	c.swsp	ra,44(sp)
2005b0a0:	d226                	c.swsp	s1,36(sp)
2005b0a2:	d04a                	c.swsp	s2,32(sp)
2005b0a4:	ce4e                	c.swsp	s3,28(sp)
2005b0a6:	4405                	c.li	s0,1
2005b0a8:	c505                	c.beqz	a0,2005b0d0 <parse_param+0x36>
2005b0aa:	84ae                	c.mv	s1,a1
2005b0ac:	c62a                	c.swsp	a0,12(sp)
2005b0ae:	4405                	c.li	s0,1
2005b0b0:	200639b7          	lui	s3,0x20063
2005b0b4:	495d                	c.li	s2,23
2005b0b6:	a4098593          	addi	a1,s3,-1472 # 20062a40 <__FUNCTION__.0+0x1b8>
2005b0ba:	0068                	c.addi4spn	a0,sp,12
2005b0bc:	75a060ef          	jal	ra,20061816 <strsep>
2005b0c0:	00241793          	slli	a5,s0,0x2
2005b0c4:	97a6                	c.add	a5,s1
2005b0c6:	c388                	c.sw	a0,0(a5)
2005b0c8:	c501                	c.beqz	a0,2005b0d0 <parse_param+0x36>
2005b0ca:	0405                	c.addi	s0,1
2005b0cc:	ff2415e3          	bne	s0,s2,2005b0b6 <parse_param+0x1c>
2005b0d0:	50b2                	c.lwsp	ra,44(sp)
2005b0d2:	8522                	c.mv	a0,s0
2005b0d4:	5422                	c.lwsp	s0,40(sp)
2005b0d6:	5492                	c.lwsp	s1,36(sp)
2005b0d8:	5902                	c.lwsp	s2,32(sp)
2005b0da:	49f2                	c.lwsp	s3,28(sp)
2005b0dc:	6145                	c.addi16sp	sp,48
2005b0de:	8082                	c.jr	ra

2005b0e0 <mp_commnad_handler>:
2005b0e0:	7135                	c.addi16sp	sp,-160
2005b0e2:	cd22                	c.swsp	s0,152(sp)
2005b0e4:	0800                	c.addi4spn	s0,sp,16
2005b0e6:	cb26                	c.swsp	s1,148(sp)
2005b0e8:	08000613          	addi	a2,zero,128
2005b0ec:	84aa                	c.mv	s1,a0
2005b0ee:	4581                	c.li	a1,0
2005b0f0:	8522                	c.mv	a0,s0
2005b0f2:	cf06                	c.swsp	ra,156(sp)
2005b0f4:	c622                	c.swsp	s0,12(sp)
2005b0f6:	dffaf097          	auipc	ra,0xdffaf
2005b0fa:	9d8080e7          	jalr	ra,-1576(ra) # 9ace <__wrap_memset>
2005b0fe:	85a6                	c.mv	a1,s1
2005b100:	07f00613          	addi	a2,zero,127
2005b104:	8522                	c.mv	a0,s0
2005b106:	6a6060ef          	jal	ra,200617ac <strncpy>
2005b10a:	200635b7          	lui	a1,0x20063
2005b10e:	a1c58593          	addi	a1,a1,-1508 # 20062a1c <__FUNCTION__.0+0x194>
2005b112:	0068                	c.addi4spn	a0,sp,12
2005b114:	702060ef          	jal	ra,20061816 <strsep>
2005b118:	cd15                	c.beqz	a0,2005b154 <mp_commnad_handler+0x74>
2005b11a:	200635b7          	lui	a1,0x20063
2005b11e:	8522                	c.mv	a0,s0
2005b120:	a4458593          	addi	a1,a1,-1468 # 20062a44 <__FUNCTION__.0+0x1bc>
2005b124:	dffaf097          	auipc	ra,0xdffaf
2005b128:	9b6080e7          	jalr	ra,-1610(ra) # 9ada <__wrap_strcmp>
2005b12c:	842a                	c.mv	s0,a0
2005b12e:	e11d                	c.bnez	a0,2005b154 <mp_commnad_handler+0x74>
2005b130:	200635b7          	lui	a1,0x20063
2005b134:	13c58593          	addi	a1,a1,316 # 2006313c <__FUNCTION__.0+0x90>
2005b138:	0068                	c.addi4spn	a0,sp,12
2005b13a:	6dc060ef          	jal	ra,20061816 <strsep>
2005b13e:	4605                	c.li	a2,1
2005b140:	08000593          	addi	a1,zero,128
2005b144:	5ae050ef          	jal	ra,200606f2 <inic_mp_command>
2005b148:	40fa                	c.lwsp	ra,156(sp)
2005b14a:	8522                	c.mv	a0,s0
2005b14c:	446a                	c.lwsp	s0,152(sp)
2005b14e:	44da                	c.lwsp	s1,148(sp)
2005b150:	610d                	c.addi16sp	sp,160
2005b152:	8082                	c.jr	ra
2005b154:	547d                	c.li	s0,-1
2005b156:	bfcd                	c.j	2005b148 <mp_commnad_handler+0x68>

2005b158 <log_service>:
2005b158:	1141                	c.addi	sp,-16
2005b15a:	c422                	c.swsp	s0,8(sp)
2005b15c:	c606                	c.swsp	ra,12(sp)
2005b15e:	842a                	c.mv	s0,a0
2005b160:	3569                	c.jal	2005afea <log_handler>
2005b162:	ed01                	c.bnez	a0,2005b17a <log_service+0x22>
2005b164:	8522                	c.mv	a0,s0
2005b166:	3fad                	c.jal	2005b0e0 <mp_commnad_handler>
2005b168:	00055963          	bge	a0,zero,2005b17a <log_service+0x22>
2005b16c:	20063537          	lui	a0,0x20063
2005b170:	85a2                	c.mv	a1,s0
2005b172:	a4c50513          	addi	a0,a0,-1460 # 20062a4c <__FUNCTION__.0+0x1c4>
2005b176:	789030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b17a:	23b5                	c.jal	2005b6e6 <rtos_mem_get_free_heap_size>
2005b17c:	85aa                	c.mv	a1,a0
2005b17e:	20063537          	lui	a0,0x20063
2005b182:	a6450513          	addi	a0,a0,-1436 # 20062a64 <__FUNCTION__.0+0x1dc>
2005b186:	779030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b18a:	4422                	c.lwsp	s0,8(sp)
2005b18c:	40b2                	c.lwsp	ra,12(sp)
2005b18e:	20063537          	lui	a0,0x20063
2005b192:	a9050513          	addi	a0,a0,-1392 # 20062a90 <__FUNCTION__.0+0x208>
2005b196:	0141                	c.addi	sp,16
2005b198:	7670306f          	jal	zero,2005f0fe <DiagPrintf_minimal>

2005b19c <app_mbedtls_free_func>:
2005b19c:	a389                	c.j	2005b6de <rtos_mem_free>

2005b19e <app_mbedtls_calloc_func>:
2005b19e:	02b50633          	mul	a2,a0,a1
2005b1a2:	1101                	c.addi	sp,-32
2005b1a4:	cc22                	c.swsp	s0,24(sp)
2005b1a6:	ce06                	c.swsp	ra,28(sp)
2005b1a8:	8532                	c.mv	a0,a2
2005b1aa:	c632                	c.swsp	a2,12(sp)
2005b1ac:	2321                	c.jal	2005b6b4 <rtos_mem_malloc>
2005b1ae:	842a                	c.mv	s0,a0
2005b1b0:	c519                	c.beqz	a0,2005b1be <app_mbedtls_calloc_func+0x20>
2005b1b2:	4632                	c.lwsp	a2,12(sp)
2005b1b4:	4581                	c.li	a1,0
2005b1b6:	dffaf097          	auipc	ra,0xdffaf
2005b1ba:	918080e7          	jalr	ra,-1768(ra) # 9ace <__wrap_memset>
2005b1be:	40f2                	c.lwsp	ra,28(sp)
2005b1c0:	8522                	c.mv	a0,s0
2005b1c2:	4462                	c.lwsp	s0,24(sp)
2005b1c4:	6105                	c.addi16sp	sp,32
2005b1c6:	8082                	c.jr	ra

2005b1c8 <app_IWDG_refresh>:
2005b1c8:	4100d537          	lui	a0,0x4100d
2005b1cc:	c0050513          	addi	a0,a0,-1024 # 4100cc00 <__km4_bd_boot_download_addr__+0x10ffac00>
2005b1d0:	dffad317          	auipc	t1,0xdffad
2005b1d4:	03230067          	jalr	zero,50(t1) # 8202 <WDG_Refresh>

2005b1d8 <app_init_debug>:
2005b1d8:	1141                	c.addi	sp,-16
2005b1da:	c606                	c.swsp	ra,12(sp)
2005b1dc:	dffac097          	auipc	ra,0xdffac
2005b1e0:	9c4080e7          	jalr	ra,-1596(ra) # 6ba0 <SYSCFG_OTP_DisBootLog>
2005b1e4:	00153513          	sltiu	a0,a0,1
2005b1e8:	200017b7          	lui	a5,0x20001
2005b1ec:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b1f0:	0506                	c.slli	a0,0x1
2005b1f2:	577d                	c.li	a4,-1
2005b1f4:	c398                	c.sw	a4,0(a5)
2005b1f6:	0007a223          	sw	zero,4(a5)
2005b1fa:	c788                	c.sw	a0,8(a5)
2005b1fc:	0007a623          	sw	zero,12(a5)
2005b200:	40b2                	c.lwsp	ra,12(sp)
2005b202:	0141                	c.addi	sp,16
2005b204:	8082                	c.jr	ra

2005b206 <app_mbedtls_rom_init>:
2005b206:	2005b5b7          	lui	a1,0x2005b
2005b20a:	2005b537          	lui	a0,0x2005b
2005b20e:	1141                	c.addi	sp,-16
2005b210:	19c58593          	addi	a1,a1,412 # 2005b19c <app_mbedtls_free_func>
2005b214:	19e50513          	addi	a0,a0,414 # 2005b19e <app_mbedtls_calloc_func>
2005b218:	c606                	c.swsp	ra,12(sp)
2005b21a:	267000ef          	jal	ra,2005bc80 <mbedtls_platform_set_calloc_free>
2005b21e:	40b2                	c.lwsp	ra,12(sp)
2005b220:	ee01aa23          	sw	zero,-268(gp) # 20066170 <use_hw_crypto_func>
2005b224:	0141                	c.addi	sp,16
2005b226:	8082                	c.jr	ra

2005b228 <app_pmu_init>:
2005b228:	1141                	c.addi	sp,-16
2005b22a:	200017b7          	lui	a5,0x20001
2005b22e:	ad878793          	addi	a5,a5,-1320 # 20000ad8 <ConfigDebug>
2005b232:	c606                	c.swsp	ra,12(sp)
2005b234:	4798                	c.lw	a4,8(a5)
2005b236:	100006b7          	lui	a3,0x10000
2005b23a:	4501                	c.li	a0,0
2005b23c:	8f55                	c.or	a4,a3
2005b23e:	c798                	c.sw	a4,8(a5)
2005b240:	d96fe0ef          	jal	ra,200597d6 <pmu_acquire_wakelock>
2005b244:	4100c7b7          	lui	a5,0x4100c
2005b248:	22c7a783          	lw	a5,556(a5) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
2005b24c:	8b89                	c.andi	a5,2
2005b24e:	c38d                	c.beqz	a5,2005b270 <app_pmu_init+0x48>
2005b250:	4509                	c.li	a0,2
2005b252:	d84fe0ef          	jal	ra,200597d6 <pmu_acquire_wakelock>
2005b256:	993fe0ef          	jal	ra,20059be8 <SWR_CORE_Vol_Get>
2005b25a:	4785                	c.li	a5,1
2005b25c:	00f51a63          	bne	a0,a5,2005b270 <app_pmu_init+0x48>
2005b260:	94efe0ef          	jal	ra,200593ae <dsp_status_on>
2005b264:	c511                	c.beqz	a0,2005b270 <app_pmu_init+0x48>
2005b266:	40b2                	c.lwsp	ra,12(sp)
2005b268:	4511                	c.li	a0,4
2005b26a:	0141                	c.addi	sp,16
2005b26c:	d6afe06f          	jal	zero,200597d6 <pmu_acquire_wakelock>
2005b270:	40b2                	c.lwsp	ra,12(sp)
2005b272:	0141                	c.addi	sp,16
2005b274:	8082                	c.jr	ra

2005b276 <app_IWDG_int>:
2005b276:	4100c7b7          	lui	a5,0x4100c
2005b27a:	479c                	c.lw	a5,8(a5)
2005b27c:	c0000737          	lui	a4,0xc0000
2005b280:	0709                	c.addi	a4,2
2005b282:	8ff9                	c.and	a5,a4
2005b284:	c7b5                	c.beqz	a5,2005b2f0 <app_IWDG_int+0x7a>
2005b286:	1101                	c.addi	sp,-32
2005b288:	cc22                	c.swsp	s0,24(sp)
2005b28a:	200636b7          	lui	a3,0x20063
2005b28e:	20063437          	lui	s0,0x20063
2005b292:	a9c68693          	addi	a3,a3,-1380 # 20062a9c <__FUNCTION__.0+0x214>
2005b296:	04900613          	addi	a2,zero,73
2005b29a:	ab040593          	addi	a1,s0,-1360 # 20062ab0 <__FUNCTION__.0+0x228>
2005b29e:	4511                	c.li	a0,4
2005b2a0:	ce06                	c.swsp	ra,28(sp)
2005b2a2:	c17ff0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005b2a6:	2005b7b7          	lui	a5,0x2005b
2005b2aa:	200635b7          	lui	a1,0x20063
2005b2ae:	1c878793          	addi	a5,a5,456 # 2005b1c8 <app_IWDG_refresh>
2005b2b2:	4705                	c.li	a4,1
2005b2b4:	1f400693          	addi	a3,zero,500
2005b2b8:	4601                	c.li	a2,0
2005b2ba:	ab858593          	addi	a1,a1,-1352 # 20062ab8 <__FUNCTION__.0+0x230>
2005b2be:	0068                	c.addi4spn	a0,sp,12
2005b2c0:	c602                	c.swsp	zero,12(sp)
2005b2c2:	11d000ef          	jal	ra,2005bbde <rtos_timer_create>
2005b2c6:	4532                	c.lwsp	a0,12(sp)
2005b2c8:	e105                	c.bnez	a0,2005b2e8 <app_IWDG_int+0x72>
2005b2ca:	200636b7          	lui	a3,0x20063
2005b2ce:	ac468693          	addi	a3,a3,-1340 # 20062ac4 <__FUNCTION__.0+0x23c>
2005b2d2:	04500613          	addi	a2,zero,69
2005b2d6:	ab040593          	addi	a1,s0,-1360
2005b2da:	4509                	c.li	a0,2
2005b2dc:	bddff0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005b2e0:	40f2                	c.lwsp	ra,28(sp)
2005b2e2:	4462                	c.lwsp	s0,24(sp)
2005b2e4:	6105                	c.addi16sp	sp,32
2005b2e6:	8082                	c.jr	ra
2005b2e8:	4581                	c.li	a1,0
2005b2ea:	129000ef          	jal	ra,2005bc12 <rtos_timer_start>
2005b2ee:	bfcd                	c.j	2005b2e0 <app_IWDG_int+0x6a>
2005b2f0:	8082                	c.jr	ra

2005b2f2 <app_pre_example>:
2005b2f2:	8082                	c.jr	ra

2005b2f4 <app_example>:
2005b2f4:	8082                	c.jr	ra

2005b2f6 <main>:
2005b2f6:	20059537          	lui	a0,0x20059
2005b2fa:	1141                	c.addi	sp,-16
2005b2fc:	4691                	c.li	a3,4
2005b2fe:	41020637          	lui	a2,0x41020
2005b302:	02b00593          	addi	a1,zero,43
2005b306:	17e50513          	addi	a0,a0,382 # 2005917e <IPC_INTHandler>
2005b30a:	c606                	c.swsp	ra,12(sp)
2005b30c:	328040ef          	jal	ra,2005f634 <irq_register>
2005b310:	02b00513          	addi	a0,zero,43
2005b314:	dffad097          	auipc	ra,0xdffad
2005b318:	cc4080e7          	jalr	ra,-828(ra) # 7fd8 <irq_enable>
2005b31c:	35ed                	c.jal	2005b206 <app_mbedtls_rom_init>
2005b31e:	4581                	c.li	a1,0
2005b320:	4501                	c.li	a0,0
2005b322:	dffae097          	auipc	ra,0xdffae
2005b326:	f32080e7          	jalr	ra,-206(ra) # 9254 <shell_init_rom>
2005b32a:	435010ef          	jal	ra,2005cf5e <shell_init_ram>
2005b32e:	41020537          	lui	a0,0x41020
2005b332:	a45fd0ef          	jal	ra,20058d76 <ipc_table_init>
2005b336:	2005c537          	lui	a0,0x2005c
2005b33a:	b8450513          	addi	a0,a0,-1148 # 2005bb84 <rtos_time_delay_ms>
2005b33e:	822fe0ef          	jal	ra,20059360 <IPC_SEMDelayStub>
2005b342:	3f45                	c.jal	2005b2f2 <app_pre_example>
2005b344:	454030ef          	jal	ra,2005e798 <wlan_initialize>
2005b348:	3d41                	c.jal	2005b1d8 <app_init_debug>
2005b34a:	3df9                	c.jal	2005b228 <app_pmu_init>
2005b34c:	372d                	c.jal	2005b276 <app_IWDG_int>
2005b34e:	375d                	c.jal	2005b2f4 <app_example>
2005b350:	200636b7          	lui	a3,0x20063
2005b354:	200635b7          	lui	a1,0x20063
2005b358:	ad868693          	addi	a3,a3,-1320 # 20062ad8 <__FUNCTION__.0+0x250>
2005b35c:	04900613          	addi	a2,zero,73
2005b360:	ab058593          	addi	a1,a1,-1360 # 20062ab0 <__FUNCTION__.0+0x228>
2005b364:	4511                	c.li	a0,4
2005b366:	b53ff0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005b36a:	7ae000ef          	jal	ra,2005bb18 <rtos_sched_start>
2005b36e:	40b2                	c.lwsp	ra,12(sp)
2005b370:	4501                	c.li	a0,0
2005b372:	0141                	c.addi	sp,16
2005b374:	8082                	c.jr	ra

2005b376 <CmdRamHelp>:
2005b376:	1101                	c.addi	sp,-32
2005b378:	cc22                	c.swsp	s0,24(sp)
2005b37a:	ca26                	c.swsp	s1,20(sp)
2005b37c:	20064437          	lui	s0,0x20064
2005b380:	200644b7          	lui	s1,0x20064
2005b384:	54048793          	addi	a5,s1,1344 # 20064540 <shell_cmd_table>
2005b388:	58040413          	addi	s0,s0,1408 # 20064580 <ipc_tickless_table>
2005b38c:	8c1d                	c.sub	s0,a5
2005b38e:	8011                	c.srli	s0,0x4
2005b390:	20063537          	lui	a0,0x20063
2005b394:	85a2                	c.mv	a1,s0
2005b396:	ae850513          	addi	a0,a0,-1304 # 20062ae8 <__FUNCTION__.0+0x260>
2005b39a:	c84a                	c.swsp	s2,16(sp)
2005b39c:	c64e                	c.swsp	s3,12(sp)
2005b39e:	ce06                	c.swsp	ra,28(sp)
2005b3a0:	54048493          	addi	s1,s1,1344
2005b3a4:	55b030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b3a8:	4901                	c.li	s2,0
2005b3aa:	200639b7          	lui	s3,0x20063
2005b3ae:	02891163          	bne	s2,s0,2005b3d0 <CmdRamHelp+0x5a>
2005b3b2:	20063537          	lui	a0,0x20063
2005b3b6:	85ca                	c.mv	a1,s2
2005b3b8:	b2c50513          	addi	a0,a0,-1236 # 20062b2c <__FUNCTION__.0+0x2a4>
2005b3bc:	543030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b3c0:	40f2                	c.lwsp	ra,28(sp)
2005b3c2:	4462                	c.lwsp	s0,24(sp)
2005b3c4:	44d2                	c.lwsp	s1,20(sp)
2005b3c6:	4942                	c.lwsp	s2,16(sp)
2005b3c8:	49b2                	c.lwsp	s3,12(sp)
2005b3ca:	4505                	c.li	a0,1
2005b3cc:	6105                	c.addi16sp	sp,32
2005b3ce:	8082                	c.jr	ra
2005b3d0:	44cc                	c.lw	a1,12(s1)
2005b3d2:	c589                	c.beqz	a1,2005b3dc <CmdRamHelp+0x66>
2005b3d4:	b2898513          	addi	a0,s3,-1240 # 20062b28 <__FUNCTION__.0+0x2a0>
2005b3d8:	527030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b3dc:	0905                	c.addi	s2,1
2005b3de:	04c1                	c.addi	s1,16
2005b3e0:	b7f9                	c.j	2005b3ae <CmdRamHelp+0x38>

2005b3e2 <cmd_reboot>:
2005b3e2:	1141                	c.addi	sp,-16
2005b3e4:	c226                	c.swsp	s1,4(sp)
2005b3e6:	84aa                	c.mv	s1,a0
2005b3e8:	20063537          	lui	a0,0x20063
2005b3ec:	b6c50513          	addi	a0,a0,-1172 # 20062b6c <__FUNCTION__.0+0x2e4>
2005b3f0:	c422                	c.swsp	s0,8(sp)
2005b3f2:	c606                	c.swsp	ra,12(sp)
2005b3f4:	842e                	c.mv	s0,a1
2005b3f6:	509030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b3fa:	c08d                	c.beqz	s1,2005b41c <cmd_reboot+0x3a>
2005b3fc:	4008                	c.lw	a0,0(s0)
2005b3fe:	200635b7          	lui	a1,0x20063
2005b402:	b7c58593          	addi	a1,a1,-1156 # 20062b7c <__FUNCTION__.0+0x2f4>
2005b406:	dffae097          	auipc	ra,0xdffae
2005b40a:	426080e7          	jalr	ra,1062(ra) # 982c <_strcmp>
2005b40e:	e519                	c.bnez	a0,2005b41c <cmd_reboot+0x3a>
2005b410:	20000593          	addi	a1,zero,512
2005b414:	dffa5097          	auipc	ra,0xdffa5
2005b418:	492080e7          	jalr	ra,1170(ra) # 8a6 <BKUP_Set>
2005b41c:	f80fe0ef          	jal	ra,20059b9c <System_Reset>
2005b420:	40b2                	c.lwsp	ra,12(sp)
2005b422:	4422                	c.lwsp	s0,8(sp)
2005b424:	4492                	c.lwsp	s1,4(sp)
2005b426:	4505                	c.li	a0,1
2005b428:	0141                	c.addi	sp,16
2005b42a:	8082                	c.jr	ra

2005b42c <cmd_write_word>:
2005b42c:	1141                	c.addi	sp,-16
2005b42e:	c606                	c.swsp	ra,12(sp)
2005b430:	c422                	c.swsp	s0,8(sp)
2005b432:	c226                	c.swsp	s1,4(sp)
2005b434:	c04a                	c.swsp	s2,0(sp)
2005b436:	892e                	c.mv	s2,a1
2005b438:	00092503          	lw	a0,0(s2)
2005b43c:	4641                	c.li	a2,16
2005b43e:	4581                	c.li	a1,0
2005b440:	dffae097          	auipc	ra,0xdffae
2005b444:	4f6080e7          	jalr	ra,1270(ra) # 9936 <_strtoul>
2005b448:	ffc57413          	andi	s0,a0,-4
2005b44c:	84aa                	c.mv	s1,a0
2005b44e:	00492503          	lw	a0,4(s2)
2005b452:	4641                	c.li	a2,16
2005b454:	4581                	c.li	a1,0
2005b456:	dffae097          	auipc	ra,0xdffae
2005b45a:	4e0080e7          	jalr	ra,1248(ra) # 9936 <_strtoul>
2005b45e:	892a                	c.mv	s2,a0
2005b460:	862a                	c.mv	a2,a0
2005b462:	20063537          	lui	a0,0x20063
2005b466:	85a2                	c.mv	a1,s0
2005b468:	b8850513          	addi	a0,a0,-1144 # 20062b88 <__FUNCTION__.0+0x300>
2005b46c:	493030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b470:	01242023          	sw	s2,0(s0)
2005b474:	7c1027f3          	csrrs	a5,0x7c1,zero
2005b478:	1007f793          	andi	a5,a5,256
2005b47c:	c7a1                	c.beqz	a5,2005b4c4 <cmd_write_word+0x98>
2005b47e:	01c4f793          	andi	a5,s1,28
2005b482:	4711                	c.li	a4,4
2005b484:	c789                	c.beqz	a5,2005b48e <cmd_write_word+0x62>
2005b486:	00478713          	addi	a4,a5,4
2005b48a:	fe04f413          	andi	s0,s1,-32
2005b48e:	01f47793          	andi	a5,s0,31
2005b492:	97ba                	c.add	a5,a4
2005b494:	0ff0000f          	fence	iorw,iorw
2005b498:	97a2                	c.add	a5,s0
2005b49a:	0484700b          	cache	dwb,(s0)
2005b49e:	02040413          	addi	s0,s0,32
2005b4a2:	40878733          	sub	a4,a5,s0
2005b4a6:	fee04ae3          	blt	zero,a4,2005b49a <cmd_write_word+0x6e>
2005b4aa:	0ff0000f          	fence	iorw,iorw
2005b4ae:	0001                	c.addi	zero,0
2005b4b0:	0001                	c.addi	zero,0
2005b4b2:	0001                	c.addi	zero,0
2005b4b4:	0001                	c.addi	zero,0
2005b4b6:	0001                	c.addi	zero,0
2005b4b8:	0001                	c.addi	zero,0
2005b4ba:	0001                	c.addi	zero,0
2005b4bc:	0001                	c.addi	zero,0
2005b4be:	0001                	c.addi	zero,0
2005b4c0:	0001                	c.addi	zero,0
2005b4c2:	0001                	c.addi	zero,0
2005b4c4:	40b2                	c.lwsp	ra,12(sp)
2005b4c6:	4422                	c.lwsp	s0,8(sp)
2005b4c8:	4492                	c.lwsp	s1,4(sp)
2005b4ca:	4902                	c.lwsp	s2,0(sp)
2005b4cc:	4501                	c.li	a0,0
2005b4ce:	0141                	c.addi	sp,16
2005b4d0:	8082                	c.jr	ra

2005b4d2 <cmd_dump_word>:
2005b4d2:	1101                	c.addi	sp,-32
2005b4d4:	cc22                	c.swsp	s0,24(sp)
2005b4d6:	c84a                	c.swsp	s2,16(sp)
2005b4d8:	842a                	c.mv	s0,a0
2005b4da:	ce06                	c.swsp	ra,28(sp)
2005b4dc:	ca26                	c.swsp	s1,20(sp)
2005b4de:	c64e                	c.swsp	s3,12(sp)
2005b4e0:	892e                	c.mv	s2,a1
2005b4e2:	dffab097          	auipc	ra,0xdffab
2005b4e6:	66c080e7          	jalr	ra,1644(ra) # 6b4e <SYSCFG_OTP_RSIPEn>
2005b4ea:	fff40793          	addi	a5,s0,-1
2005b4ee:	07c2                	c.slli	a5,0x10
2005b4f0:	83c1                	c.srli	a5,0x10
2005b4f2:	4709                	c.li	a4,2
2005b4f4:	00f77a63          	bgeu	a4,a5,2005b508 <cmd_dump_word+0x36>
2005b4f8:	20063537          	lui	a0,0x20063
2005b4fc:	b9850513          	addi	a0,a0,-1128 # 20062b98 <__FUNCTION__.0+0x310>
2005b500:	3ff030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b504:	4401                	c.li	s0,0
2005b506:	a0f9                	c.j	2005b5d4 <cmd_dump_word+0x102>
2005b508:	478d                	c.li	a5,3
2005b50a:	89aa                	c.mv	s3,a0
2005b50c:	06f41063          	bne	s0,a5,2005b56c <cmd_dump_word+0x9a>
2005b510:	00892783          	lw	a5,8(s2)
2005b514:	0007c403          	lbu	s0,0(a5)
2005b518:	0df47413          	andi	s0,s0,223
2005b51c:	fbe40413          	addi	s0,s0,-66
2005b520:	00143413          	sltiu	s0,s0,1
2005b524:	00492503          	lw	a0,4(s2)
2005b528:	4629                	c.li	a2,10
2005b52a:	4581                	c.li	a1,0
2005b52c:	dffae097          	auipc	ra,0xdffae
2005b530:	40a080e7          	jalr	ra,1034(ra) # 9936 <_strtoul>
2005b534:	84aa                	c.mv	s1,a0
2005b536:	00092503          	lw	a0,0(s2)
2005b53a:	4641                	c.li	a2,16
2005b53c:	4581                	c.li	a1,0
2005b53e:	dffae097          	auipc	ra,0xdffae
2005b542:	3f8080e7          	jalr	ra,1016(ra) # 9936 <_strtoul>
2005b546:	87aa                	c.mv	a5,a0
2005b548:	f8000737          	lui	a4,0xf8000
2005b54c:	9971                	c.andi	a0,-4
2005b54e:	972a                	c.add	a4,a0
2005b550:	080006b7          	lui	a3,0x8000
2005b554:	02d77463          	bgeu	a4,a3,2005b57c <cmd_dump_word+0xaa>
2005b558:	02098263          	beq	s3,zero,2005b57c <cmd_dump_word+0xaa>
2005b55c:	20063537          	lui	a0,0x20063
2005b560:	bb450513          	addi	a0,a0,-1100 # 20062bb4 <__FUNCTION__.0+0x32c>
2005b564:	39b030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b568:	4405                	c.li	s0,1
2005b56a:	a0ad                	c.j	2005b5d4 <cmd_dump_word+0x102>
2005b56c:	4785                	c.li	a5,1
2005b56e:	00f40463          	beq	s0,a5,2005b576 <cmd_dump_word+0xa4>
2005b572:	4401                	c.li	s0,0
2005b574:	bf45                	c.j	2005b524 <cmd_dump_word+0x52>
2005b576:	4401                	c.li	s0,0
2005b578:	4485                	c.li	s1,1
2005b57a:	bf75                	c.j	2005b536 <cmd_dump_word+0x64>
2005b57c:	c43d                	c.beqz	s0,2005b5ea <cmd_dump_word+0x118>
2005b57e:	c4f9                	c.beqz	s1,2005b64c <cmd_dump_word+0x17a>
2005b580:	7c102773          	csrrs	a4,0x7c1,zero
2005b584:	10077713          	andi	a4,a4,256
2005b588:	c339                	c.beqz	a4,2005b5ce <cmd_dump_word+0xfc>
2005b58a:	01c7f713          	andi	a4,a5,28
2005b58e:	cb39                	c.beqz	a4,2005b5e4 <cmd_dump_word+0x112>
2005b590:	9b81                	c.andi	a5,-32
2005b592:	9726                	c.add	a4,s1
2005b594:	02e05d63          	bge	zero,a4,2005b5ce <cmd_dump_word+0xfc>
2005b598:	01f7f693          	andi	a3,a5,31
2005b59c:	9736                	c.add	a4,a3
2005b59e:	0ff0000f          	fence	iorw,iorw
2005b5a2:	973e                	c.add	a4,a5
2005b5a4:	02f7f00b          	cache	dwbinv,(a5)
2005b5a8:	02078793          	addi	a5,a5,32
2005b5ac:	40f706b3          	sub	a3,a4,a5
2005b5b0:	fed04ae3          	blt	zero,a3,2005b5a4 <cmd_dump_word+0xd2>
2005b5b4:	0ff0000f          	fence	iorw,iorw
2005b5b8:	0001                	c.addi	zero,0
2005b5ba:	0001                	c.addi	zero,0
2005b5bc:	0001                	c.addi	zero,0
2005b5be:	0001                	c.addi	zero,0
2005b5c0:	0001                	c.addi	zero,0
2005b5c2:	0001                	c.addi	zero,0
2005b5c4:	0001                	c.addi	zero,0
2005b5c6:	0001                	c.addi	zero,0
2005b5c8:	0001                	c.addi	zero,0
2005b5ca:	0001                	c.addi	zero,0
2005b5cc:	0001                	c.addi	zero,0
2005b5ce:	85a6                	c.mv	a1,s1
2005b5d0:	875ff0ef          	jal	ra,2005ae44 <rtk_log_memory_dump_byte>
2005b5d4:	40f2                	c.lwsp	ra,28(sp)
2005b5d6:	8522                	c.mv	a0,s0
2005b5d8:	4462                	c.lwsp	s0,24(sp)
2005b5da:	44d2                	c.lwsp	s1,20(sp)
2005b5dc:	4942                	c.lwsp	s2,16(sp)
2005b5de:	49b2                	c.lwsp	s3,12(sp)
2005b5e0:	6105                	c.addi16sp	sp,32
2005b5e2:	8082                	c.jr	ra
2005b5e4:	8726                	c.mv	a4,s1
2005b5e6:	87aa                	c.mv	a5,a0
2005b5e8:	b775                	c.j	2005b594 <cmd_dump_word+0xc2>
2005b5ea:	c0ad                	c.beqz	s1,2005b64c <cmd_dump_word+0x17a>
2005b5ec:	7c102773          	csrrs	a4,0x7c1,zero
2005b5f0:	10077713          	andi	a4,a4,256
2005b5f4:	cb21                	c.beqz	a4,2005b644 <cmd_dump_word+0x172>
2005b5f6:	01c7f693          	andi	a3,a5,28
2005b5fa:	00249613          	slli	a2,s1,0x2
2005b5fe:	872a                	c.mv	a4,a0
2005b600:	c681                	c.beqz	a3,2005b608 <cmd_dump_word+0x136>
2005b602:	fe07f713          	andi	a4,a5,-32
2005b606:	9636                	c.add	a2,a3
2005b608:	02c05e63          	bge	zero,a2,2005b644 <cmd_dump_word+0x172>
2005b60c:	01f77693          	andi	a3,a4,31
2005b610:	96b2                	c.add	a3,a2
2005b612:	0ff0000f          	fence	iorw,iorw
2005b616:	87ba                	c.mv	a5,a4
2005b618:	9736                	c.add	a4,a3
2005b61a:	02f7f00b          	cache	dwbinv,(a5)
2005b61e:	02078793          	addi	a5,a5,32
2005b622:	40f706b3          	sub	a3,a4,a5
2005b626:	fed04ae3          	blt	zero,a3,2005b61a <cmd_dump_word+0x148>
2005b62a:	0ff0000f          	fence	iorw,iorw
2005b62e:	0001                	c.addi	zero,0
2005b630:	0001                	c.addi	zero,0
2005b632:	0001                	c.addi	zero,0
2005b634:	0001                	c.addi	zero,0
2005b636:	0001                	c.addi	zero,0
2005b638:	0001                	c.addi	zero,0
2005b63a:	0001                	c.addi	zero,0
2005b63c:	0001                	c.addi	zero,0
2005b63e:	0001                	c.addi	zero,0
2005b640:	0001                	c.addi	zero,0
2005b642:	0001                	c.addi	zero,0
2005b644:	85a6                	c.mv	a1,s1
2005b646:	f8cff0ef          	jal	ra,2005add2 <rtk_log_memory_dump_word>
2005b64a:	bf39                	c.j	2005b568 <cmd_dump_word+0x96>
2005b64c:	20063537          	lui	a0,0x20063
2005b650:	bd850513          	addi	a0,a0,-1064 # 20062bd8 <__FUNCTION__.0+0x350>
2005b654:	b575                	c.j	2005b500 <cmd_dump_word+0x2e>

2005b656 <rtos_critical_is_in_interrupt>:
2005b656:	1141                	c.addi	sp,-16
2005b658:	c606                	c.swsp	ra,12(sp)
2005b65a:	7f80a0ef          	jal	ra,20065e52 <plic_get_active_irq_id>
2005b65e:	40b2                	c.lwsp	ra,12(sp)
2005b660:	00a03533          	sltu	a0,zero,a0
2005b664:	0141                	c.addi	sp,16
2005b666:	8082                	c.jr	ra

2005b668 <rtos_critical_enter>:
2005b668:	1141                	c.addi	sp,-16
2005b66a:	c606                	c.swsp	ra,12(sp)
2005b66c:	37ed                	c.jal	2005b656 <rtos_critical_is_in_interrupt>
2005b66e:	c919                	c.beqz	a0,2005b684 <rtos_critical_enter+0x1c>
2005b670:	ef818793          	addi	a5,gp,-264 # 20066174 <uxCriticalNestingCnt>
2005b674:	0007d703          	lhu	a4,0(a5)
2005b678:	0705                	c.addi	a4,1
2005b67a:	00e79023          	sh	a4,0(a5)
2005b67e:	40b2                	c.lwsp	ra,12(sp)
2005b680:	0141                	c.addi	sp,16
2005b682:	8082                	c.jr	ra
2005b684:	40b2                	c.lwsp	ra,12(sp)
2005b686:	0141                	c.addi	sp,16
2005b688:	37c0206f          	jal	zero,2005da04 <vTaskEnterCritical>

2005b68c <rtos_critical_exit>:
2005b68c:	1141                	c.addi	sp,-16
2005b68e:	c606                	c.swsp	ra,12(sp)
2005b690:	37d9                	c.jal	2005b656 <rtos_critical_is_in_interrupt>
2005b692:	c919                	c.beqz	a0,2005b6a8 <rtos_critical_exit+0x1c>
2005b694:	ef818793          	addi	a5,gp,-264 # 20066174 <uxCriticalNestingCnt>
2005b698:	0007d703          	lhu	a4,0(a5)
2005b69c:	177d                	c.addi	a4,-1
2005b69e:	00e79023          	sh	a4,0(a5)
2005b6a2:	40b2                	c.lwsp	ra,12(sp)
2005b6a4:	0141                	c.addi	sp,16
2005b6a6:	8082                	c.jr	ra
2005b6a8:	40b2                	c.lwsp	ra,12(sp)
2005b6aa:	0141                	c.addi	sp,16
2005b6ac:	3720206f          	jal	zero,2005da1e <vTaskExitCritical>

2005b6b0 <rtos_mem_init>:
2005b6b0:	91aff06f          	jal	zero,2005a7ca <os_heap_init>

2005b6b4 <rtos_mem_malloc>:
2005b6b4:	a98ff06f          	jal	zero,2005a94c <pvPortMalloc>

2005b6b8 <rtos_mem_zmalloc>:
2005b6b8:	1101                	c.addi	sp,-32
2005b6ba:	cc22                	c.swsp	s0,24(sp)
2005b6bc:	ce06                	c.swsp	ra,28(sp)
2005b6be:	c62a                	c.swsp	a0,12(sp)
2005b6c0:	a8cff0ef          	jal	ra,2005a94c <pvPortMalloc>
2005b6c4:	842a                	c.mv	s0,a0
2005b6c6:	c519                	c.beqz	a0,2005b6d4 <rtos_mem_zmalloc+0x1c>
2005b6c8:	4632                	c.lwsp	a2,12(sp)
2005b6ca:	4581                	c.li	a1,0
2005b6cc:	dffae097          	auipc	ra,0xdffae
2005b6d0:	402080e7          	jalr	ra,1026(ra) # 9ace <__wrap_memset>
2005b6d4:	40f2                	c.lwsp	ra,28(sp)
2005b6d6:	8522                	c.mv	a0,s0
2005b6d8:	4462                	c.lwsp	s0,24(sp)
2005b6da:	6105                	c.addi16sp	sp,32
2005b6dc:	8082                	c.jr	ra

2005b6de <rtos_mem_free>:
2005b6de:	c119                	c.beqz	a0,2005b6e4 <rtos_mem_free+0x6>
2005b6e0:	b4aff06f          	jal	zero,2005aa2a <vPortFree>
2005b6e4:	8082                	c.jr	ra

2005b6e6 <rtos_mem_get_free_heap_size>:
2005b6e6:	ba2ff06f          	jal	zero,2005aa88 <xPortGetFreeHeapSize>

2005b6ea <rtos_mutex_create>:
2005b6ea:	c10d                	c.beqz	a0,2005b70c <rtos_mutex_create+0x22>
2005b6ec:	1141                	c.addi	sp,-16
2005b6ee:	c422                	c.swsp	s0,8(sp)
2005b6f0:	842a                	c.mv	s0,a0
2005b6f2:	4505                	c.li	a0,1
2005b6f4:	c606                	c.swsp	ra,12(sp)
2005b6f6:	534010ef          	jal	ra,2005cc2a <xQueueCreateMutex>
2005b6fa:	c008                	c.sw	a0,0(s0)
2005b6fc:	40b2                	c.lwsp	ra,12(sp)
2005b6fe:	4422                	c.lwsp	s0,8(sp)
2005b700:	00153513          	sltiu	a0,a0,1
2005b704:	40a00533          	sub	a0,zero,a0
2005b708:	0141                	c.addi	sp,16
2005b70a:	8082                	c.jr	ra
2005b70c:	557d                	c.li	a0,-1
2005b70e:	8082                	c.jr	ra

2005b710 <rtos_mutex_create_static>:
2005b710:	1141                	c.addi	sp,-16
2005b712:	c422                	c.swsp	s0,8(sp)
2005b714:	c606                	c.swsp	ra,12(sp)
2005b716:	842a                	c.mv	s0,a0
2005b718:	2e4d                	c.jal	2005baca <__reserved_get_mutex_from_poll>
2005b71a:	e511                	c.bnez	a0,2005b726 <rtos_mutex_create_static+0x16>
2005b71c:	8522                	c.mv	a0,s0
2005b71e:	4422                	c.lwsp	s0,8(sp)
2005b720:	40b2                	c.lwsp	ra,12(sp)
2005b722:	0141                	c.addi	sp,16
2005b724:	b7d9                	c.j	2005b6ea <rtos_mutex_create>
2005b726:	85aa                	c.mv	a1,a0
2005b728:	4505                	c.li	a0,1
2005b72a:	58c010ef          	jal	ra,2005ccb6 <xQueueCreateMutexStatic>
2005b72e:	c008                	c.sw	a0,0(s0)
2005b730:	40b2                	c.lwsp	ra,12(sp)
2005b732:	4422                	c.lwsp	s0,8(sp)
2005b734:	00153513          	sltiu	a0,a0,1
2005b738:	40a00533          	sub	a0,zero,a0
2005b73c:	0141                	c.addi	sp,16
2005b73e:	8082                	c.jr	ra

2005b740 <rtos_mutex_delete>:
2005b740:	c139                	c.beqz	a0,2005b786 <rtos_mutex_delete+0x46>
2005b742:	1141                	c.addi	sp,-16
2005b744:	c422                	c.swsp	s0,8(sp)
2005b746:	c606                	c.swsp	ra,12(sp)
2005b748:	842a                	c.mv	s0,a0
2005b74a:	78b000ef          	jal	ra,2005c6d4 <xQueueGetMutexHolder>
2005b74e:	e909                	c.bnez	a0,2005b760 <rtos_mutex_delete+0x20>
2005b750:	8522                	c.mv	a0,s0
2005b752:	5e6010ef          	jal	ra,2005cd38 <vQueueDelete>
2005b756:	4501                	c.li	a0,0
2005b758:	40b2                	c.lwsp	ra,12(sp)
2005b75a:	4422                	c.lwsp	s0,8(sp)
2005b75c:	0141                	c.addi	sp,16
2005b75e:	8082                	c.jr	ra
2005b760:	8522                	c.mv	a0,s0
2005b762:	5d6010ef          	jal	ra,2005cd38 <vQueueDelete>
2005b766:	4501                	c.li	a0,0
2005b768:	58f010ef          	jal	ra,2005d4f6 <pcTaskGetName>
2005b76c:	85aa                	c.mv	a1,a0
2005b76e:	20063637          	lui	a2,0x20063
2005b772:	20063537          	lui	a0,0x20063
2005b776:	e4c60613          	addi	a2,a2,-436 # 20062e4c <__FUNCTION__.0>
2005b77a:	df850513          	addi	a0,a0,-520 # 20062df8 <__FUNCTION__.0+0x570>
2005b77e:	181030ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005b782:	557d                	c.li	a0,-1
2005b784:	bfd1                	c.j	2005b758 <rtos_mutex_delete+0x18>
2005b786:	557d                	c.li	a0,-1
2005b788:	8082                	c.jr	ra

2005b78a <rtos_mutex_delete_static>:
2005b78a:	1141                	c.addi	sp,-16
2005b78c:	c606                	c.swsp	ra,12(sp)
2005b78e:	c422                	c.swsp	s0,8(sp)
2005b790:	c226                	c.swsp	s1,4(sp)
2005b792:	84aa                	c.mv	s1,a0
2005b794:	3775                	c.jal	2005b740 <rtos_mutex_delete>
2005b796:	842a                	c.mv	s0,a0
2005b798:	8526                	c.mv	a0,s1
2005b79a:	26a9                	c.jal	2005bae4 <__reserved_release_mutex_to_poll>
2005b79c:	40b2                	c.lwsp	ra,12(sp)
2005b79e:	8522                	c.mv	a0,s0
2005b7a0:	4422                	c.lwsp	s0,8(sp)
2005b7a2:	4492                	c.lwsp	s1,4(sp)
2005b7a4:	0141                	c.addi	sp,16
2005b7a6:	8082                	c.jr	ra

2005b7a8 <rtos_mutex_take>:
2005b7a8:	7179                	c.addi16sp	sp,-48
2005b7aa:	d422                	c.swsp	s0,40(sp)
2005b7ac:	c62e                	c.swsp	a1,12(sp)
2005b7ae:	d606                	c.swsp	ra,44(sp)
2005b7b0:	842a                	c.mv	s0,a0
2005b7b2:	ce02                	c.swsp	zero,28(sp)
2005b7b4:	ea3ff0ef          	jal	ra,2005b656 <rtos_critical_is_in_interrupt>
2005b7b8:	45b2                	c.lwsp	a1,12(sp)
2005b7ba:	c505                	c.beqz	a0,2005b7e2 <rtos_mutex_take+0x3a>
2005b7bc:	0870                	c.addi4spn	a2,sp,28
2005b7be:	4581                	c.li	a1,0
2005b7c0:	8522                	c.mv	a0,s0
2005b7c2:	306010ef          	jal	ra,2005cac8 <xQueueReceiveFromISR>
2005b7c6:	872a                	c.mv	a4,a0
2005b7c8:	4785                	c.li	a5,1
2005b7ca:	557d                	c.li	a0,-1
2005b7cc:	00f71763          	bne	a4,a5,2005b7da <rtos_mutex_take+0x32>
2005b7d0:	4572                	c.lwsp	a0,28(sp)
2005b7d2:	c501                	c.beqz	a0,2005b7da <rtos_mutex_take+0x32>
2005b7d4:	665010ef          	jal	ra,2005d638 <vTaskSwitchContext>
2005b7d8:	4501                	c.li	a0,0
2005b7da:	50b2                	c.lwsp	ra,44(sp)
2005b7dc:	5422                	c.lwsp	s0,40(sp)
2005b7de:	6145                	c.addi16sp	sp,48
2005b7e0:	8082                	c.jr	ra
2005b7e2:	8522                	c.mv	a0,s0
2005b7e4:	1c6010ef          	jal	ra,2005c9aa <xQueueSemaphoreTake>
2005b7e8:	157d                	c.addi	a0,-1
2005b7ea:	00a03533          	sltu	a0,zero,a0
2005b7ee:	40a00533          	sub	a0,zero,a0
2005b7f2:	b7e5                	c.j	2005b7da <rtos_mutex_take+0x32>

2005b7f4 <rtos_mutex_give>:
2005b7f4:	1101                	c.addi	sp,-32
2005b7f6:	cc22                	c.swsp	s0,24(sp)
2005b7f8:	ce06                	c.swsp	ra,28(sp)
2005b7fa:	842a                	c.mv	s0,a0
2005b7fc:	c602                	c.swsp	zero,12(sp)
2005b7fe:	e59ff0ef          	jal	ra,2005b656 <rtos_critical_is_in_interrupt>
2005b802:	c11d                	c.beqz	a0,2005b828 <rtos_mutex_give+0x34>
2005b804:	006c                	c.addi4spn	a1,sp,12
2005b806:	8522                	c.mv	a0,s0
2005b808:	05e010ef          	jal	ra,2005c866 <xQueueGiveFromISR>
2005b80c:	872a                	c.mv	a4,a0
2005b80e:	4785                	c.li	a5,1
2005b810:	557d                	c.li	a0,-1
2005b812:	00f71763          	bne	a4,a5,2005b820 <rtos_mutex_give+0x2c>
2005b816:	4532                	c.lwsp	a0,12(sp)
2005b818:	c501                	c.beqz	a0,2005b820 <rtos_mutex_give+0x2c>
2005b81a:	61f010ef          	jal	ra,2005d638 <vTaskSwitchContext>
2005b81e:	4501                	c.li	a0,0
2005b820:	40f2                	c.lwsp	ra,28(sp)
2005b822:	4462                	c.lwsp	s0,24(sp)
2005b824:	6105                	c.addi16sp	sp,32
2005b826:	8082                	c.jr	ra
2005b828:	4681                	c.li	a3,0
2005b82a:	4601                	c.li	a2,0
2005b82c:	4581                	c.li	a1,0
2005b82e:	8522                	c.mv	a0,s0
2005b830:	6c9000ef          	jal	ra,2005c6f8 <xQueueGenericSend>
2005b834:	157d                	c.addi	a0,-1
2005b836:	00a03533          	sltu	a0,zero,a0
2005b83a:	40a00533          	sub	a0,zero,a0
2005b83e:	b7cd                	c.j	2005b820 <rtos_mutex_give+0x2c>

2005b840 <rtos_sema_create>:
2005b840:	1141                	c.addi	sp,-16
2005b842:	c422                	c.swsp	s0,8(sp)
2005b844:	c606                	c.swsp	ra,12(sp)
2005b846:	842a                	c.mv	s0,a0
2005b848:	557d                	c.li	a0,-1
2005b84a:	c809                	c.beqz	s0,2005b85c <rtos_sema_create+0x1c>
2005b84c:	8532                	c.mv	a0,a2
2005b84e:	410010ef          	jal	ra,2005cc5e <xQueueCreateCountingSemaphore>
2005b852:	c008                	c.sw	a0,0(s0)
2005b854:	00153513          	sltiu	a0,a0,1
2005b858:	40a00533          	sub	a0,zero,a0
2005b85c:	40b2                	c.lwsp	ra,12(sp)
2005b85e:	4422                	c.lwsp	s0,8(sp)
2005b860:	0141                	c.addi	sp,16
2005b862:	8082                	c.jr	ra

2005b864 <rtos_sema_create_static>:
2005b864:	1101                	c.addi	sp,-32
2005b866:	cc22                	c.swsp	s0,24(sp)
2005b868:	ca26                	c.swsp	s1,20(sp)
2005b86a:	c62e                	c.swsp	a1,12(sp)
2005b86c:	ce06                	c.swsp	ra,28(sp)
2005b86e:	842a                	c.mv	s0,a0
2005b870:	84b2                	c.mv	s1,a2
2005b872:	2471                	c.jal	2005bafe <__reserved_get_sema_from_poll>
2005b874:	45b2                	c.lwsp	a1,12(sp)
2005b876:	e901                	c.bnez	a0,2005b886 <rtos_sema_create_static+0x22>
2005b878:	8522                	c.mv	a0,s0
2005b87a:	4462                	c.lwsp	s0,24(sp)
2005b87c:	40f2                	c.lwsp	ra,28(sp)
2005b87e:	8626                	c.mv	a2,s1
2005b880:	44d2                	c.lwsp	s1,20(sp)
2005b882:	6105                	c.addi16sp	sp,32
2005b884:	bf75                	c.j	2005b840 <rtos_sema_create>
2005b886:	862a                	c.mv	a2,a0
2005b888:	8526                	c.mv	a0,s1
2005b88a:	464010ef          	jal	ra,2005ccee <xQueueCreateCountingSemaphoreStatic>
2005b88e:	c008                	c.sw	a0,0(s0)
2005b890:	40f2                	c.lwsp	ra,28(sp)
2005b892:	4462                	c.lwsp	s0,24(sp)
2005b894:	00153513          	sltiu	a0,a0,1
2005b898:	44d2                	c.lwsp	s1,20(sp)
2005b89a:	40a00533          	sub	a0,zero,a0
2005b89e:	6105                	c.addi16sp	sp,32
2005b8a0:	8082                	c.jr	ra

2005b8a2 <rtos_sema_create_binary>:
2005b8a2:	c11d                	c.beqz	a0,2005b8c8 <rtos_sema_create_binary+0x26>
2005b8a4:	1141                	c.addi	sp,-16
2005b8a6:	c422                	c.swsp	s0,8(sp)
2005b8a8:	460d                	c.li	a2,3
2005b8aa:	842a                	c.mv	s0,a0
2005b8ac:	4581                	c.li	a1,0
2005b8ae:	4505                	c.li	a0,1
2005b8b0:	c606                	c.swsp	ra,12(sp)
2005b8b2:	320010ef          	jal	ra,2005cbd2 <xQueueGenericCreate>
2005b8b6:	c008                	c.sw	a0,0(s0)
2005b8b8:	40b2                	c.lwsp	ra,12(sp)
2005b8ba:	4422                	c.lwsp	s0,8(sp)
2005b8bc:	00153513          	sltiu	a0,a0,1
2005b8c0:	40a00533          	sub	a0,zero,a0
2005b8c4:	0141                	c.addi	sp,16
2005b8c6:	8082                	c.jr	ra
2005b8c8:	557d                	c.li	a0,-1
2005b8ca:	8082                	c.jr	ra

2005b8cc <rtos_sema_delete>:
2005b8cc:	c909                	c.beqz	a0,2005b8de <rtos_sema_delete+0x12>
2005b8ce:	1141                	c.addi	sp,-16
2005b8d0:	c606                	c.swsp	ra,12(sp)
2005b8d2:	466010ef          	jal	ra,2005cd38 <vQueueDelete>
2005b8d6:	40b2                	c.lwsp	ra,12(sp)
2005b8d8:	4501                	c.li	a0,0
2005b8da:	0141                	c.addi	sp,16
2005b8dc:	8082                	c.jr	ra
2005b8de:	557d                	c.li	a0,-1
2005b8e0:	8082                	c.jr	ra

2005b8e2 <rtos_sema_take>:
2005b8e2:	7179                	c.addi16sp	sp,-48
2005b8e4:	d422                	c.swsp	s0,40(sp)
2005b8e6:	c62e                	c.swsp	a1,12(sp)
2005b8e8:	d606                	c.swsp	ra,44(sp)
2005b8ea:	842a                	c.mv	s0,a0
2005b8ec:	ce02                	c.swsp	zero,28(sp)
2005b8ee:	d69ff0ef          	jal	ra,2005b656 <rtos_critical_is_in_interrupt>
2005b8f2:	45b2                	c.lwsp	a1,12(sp)
2005b8f4:	c505                	c.beqz	a0,2005b91c <rtos_sema_take+0x3a>
2005b8f6:	0870                	c.addi4spn	a2,sp,28
2005b8f8:	4581                	c.li	a1,0
2005b8fa:	8522                	c.mv	a0,s0
2005b8fc:	1cc010ef          	jal	ra,2005cac8 <xQueueReceiveFromISR>
2005b900:	872a                	c.mv	a4,a0
2005b902:	4785                	c.li	a5,1
2005b904:	557d                	c.li	a0,-1
2005b906:	00f71763          	bne	a4,a5,2005b914 <rtos_sema_take+0x32>
2005b90a:	4572                	c.lwsp	a0,28(sp)
2005b90c:	c501                	c.beqz	a0,2005b914 <rtos_sema_take+0x32>
2005b90e:	52b010ef          	jal	ra,2005d638 <vTaskSwitchContext>
2005b912:	4501                	c.li	a0,0
2005b914:	50b2                	c.lwsp	ra,44(sp)
2005b916:	5422                	c.lwsp	s0,40(sp)
2005b918:	6145                	c.addi16sp	sp,48
2005b91a:	8082                	c.jr	ra
2005b91c:	8522                	c.mv	a0,s0
2005b91e:	08c010ef          	jal	ra,2005c9aa <xQueueSemaphoreTake>
2005b922:	157d                	c.addi	a0,-1
2005b924:	00a03533          	sltu	a0,zero,a0
2005b928:	40a00533          	sub	a0,zero,a0
2005b92c:	b7e5                	c.j	2005b914 <rtos_sema_take+0x32>

2005b92e <rtos_sema_give>:
2005b92e:	7179                	c.addi16sp	sp,-48
2005b930:	d422                	c.swsp	s0,40(sp)
2005b932:	d606                	c.swsp	ra,44(sp)
2005b934:	842a                	c.mv	s0,a0
2005b936:	ce02                	c.swsp	zero,28(sp)
2005b938:	d1fff0ef          	jal	ra,2005b656 <rtos_critical_is_in_interrupt>
2005b93c:	c505                	c.beqz	a0,2005b964 <rtos_sema_give+0x36>
2005b93e:	086c                	c.addi4spn	a1,sp,28
2005b940:	8522                	c.mv	a0,s0
2005b942:	725000ef          	jal	ra,2005c866 <xQueueGiveFromISR>
2005b946:	47f2                	c.lwsp	a5,28(sp)
2005b948:	c789                	c.beqz	a5,2005b952 <rtos_sema_give+0x24>
2005b94a:	c62a                	c.swsp	a0,12(sp)
2005b94c:	4ed010ef          	jal	ra,2005d638 <vTaskSwitchContext>
2005b950:	4532                	c.lwsp	a0,12(sp)
2005b952:	50b2                	c.lwsp	ra,44(sp)
2005b954:	5422                	c.lwsp	s0,40(sp)
2005b956:	157d                	c.addi	a0,-1
2005b958:	00a03533          	sltu	a0,zero,a0
2005b95c:	40a00533          	sub	a0,zero,a0
2005b960:	6145                	c.addi16sp	sp,48
2005b962:	8082                	c.jr	ra
2005b964:	4681                	c.li	a3,0
2005b966:	4601                	c.li	a2,0
2005b968:	4581                	c.li	a1,0
2005b96a:	8522                	c.mv	a0,s0
2005b96c:	58d000ef          	jal	ra,2005c6f8 <xQueueGenericSend>
2005b970:	b7cd                	c.j	2005b952 <rtos_sema_give+0x24>

2005b972 <__reserved_get_from_poll>:
2005b972:	7179                	c.addi16sp	sp,-48
2005b974:	d226                	c.swsp	s1,36(sp)
2005b976:	d04a                	c.swsp	s2,32(sp)
2005b978:	ce4e                	c.swsp	s3,28(sp)
2005b97a:	cc52                	c.swsp	s4,24(sp)
2005b97c:	ca56                	c.swsp	s5,20(sp)
2005b97e:	c85a                	c.swsp	s6,16(sp)
2005b980:	d606                	c.swsp	ra,44(sp)
2005b982:	d422                	c.swsp	s0,40(sp)
2005b984:	c65e                	c.swsp	s7,12(sp)
2005b986:	8a3e                	c.mv	s4,a5
2005b988:	421c                	c.lw	a5,0(a2)
2005b98a:	84aa                	c.mv	s1,a0
2005b98c:	8b2e                	c.mv	s6,a1
2005b98e:	8ab2                	c.mv	s5,a2
2005b990:	8936                	c.mv	s2,a3
2005b992:	89ba                	c.mv	s3,a4
2005b994:	ef85                	c.bnez	a5,2005b9cc <__reserved_get_from_poll+0x5a>
2005b996:	4405                	c.li	s0,1
2005b998:	08851263          	bne	a0,s0,2005ba1c <__reserved_get_from_poll+0xaa>
2005b99c:	f0818413          	addi	s0,gp,-248 # 20066184 <mutex_pool_init_flag>
2005b9a0:	401c                	c.lw	a5,0(s0)
2005b9a2:	02a78563          	beq	a5,a0,2005b9cc <__reserved_get_from_poll+0x5a>
2005b9a6:	4601                	c.li	a2,0
2005b9a8:	4581                	c.li	a1,0
2005b9aa:	59c18513          	addi	a0,gp,1436 # 20066818 <mutex_pool>
2005b9ae:	dffae097          	auipc	ra,0xdffae
2005b9b2:	120080e7          	jalr	ra,288(ra) # 9ace <__wrap_memset>
2005b9b6:	ee01ae23          	sw	zero,-260(gp) # 20066178 <mutex_buf_used_num>
2005b9ba:	f2c18513          	addi	a0,gp,-212 # 200661a8 <wrapper_mutex_buf_list>
2005b9be:	f001a223          	sw	zero,-252(gp) # 20066180 <mutex_max_buf_used_num>
2005b9c2:	c108                	c.sw	a0,0(a0)
2005b9c4:	c148                	c.sw	a0,4(a0)
2005b9c6:	f001a023          	sw	zero,-256(gp) # 2006617c <mutex_dynamic_num>
2005b9ca:	c004                	c.sw	s1,0(s0)
2005b9cc:	c9dff0ef          	jal	ra,2005b668 <rtos_critical_enter>
2005b9d0:	000b2403          	lw	s0,0(s6)
2005b9d4:	0a8b1b63          	bne	s6,s0,2005ba8a <__reserved_get_from_poll+0x118>
2005b9d8:	cb5ff0ef          	jal	ra,2005b68c <rtos_critical_exit>
2005b9dc:	4b01                	c.li	s6,0
2005b9de:	000aa783          	lw	a5,0(s5)
2005b9e2:	c791                	c.beqz	a5,2005b9ee <__reserved_get_from_poll+0x7c>
2005b9e4:	00092783          	lw	a5,0(s2)
2005b9e8:	0785                	c.addi	a5,1
2005b9ea:	00f92023          	sw	a5,0(s2)
2005b9ee:	00092703          	lw	a4,0(s2)
2005b9f2:	0009a783          	lw	a5,0(s3)
2005b9f6:	97ba                	c.add	a5,a4
2005b9f8:	000a2703          	lw	a4,0(s4)
2005b9fc:	00f77463          	bgeu	a4,a5,2005ba04 <__reserved_get_from_poll+0x92>
2005ba00:	00fa2023          	sw	a5,0(s4)
2005ba04:	50b2                	c.lwsp	ra,44(sp)
2005ba06:	5422                	c.lwsp	s0,40(sp)
2005ba08:	5492                	c.lwsp	s1,36(sp)
2005ba0a:	5902                	c.lwsp	s2,32(sp)
2005ba0c:	49f2                	c.lwsp	s3,28(sp)
2005ba0e:	4a62                	c.lwsp	s4,24(sp)
2005ba10:	4ad2                	c.lwsp	s5,20(sp)
2005ba12:	4bb2                	c.lwsp	s7,12(sp)
2005ba14:	855a                	c.mv	a0,s6
2005ba16:	4b42                	c.lwsp	s6,16(sp)
2005ba18:	6145                	c.addi16sp	sp,48
2005ba1a:	8082                	c.jr	ra
2005ba1c:	4789                	c.li	a5,2
2005ba1e:	02f51d63          	bne	a0,a5,2005ba58 <__reserved_get_from_poll+0xe6>
2005ba22:	f1818b93          	addi	s7,gp,-232 # 20066194 <sema_pool_init_flag>
2005ba26:	000ba783          	lw	a5,0(s7)
2005ba2a:	fa8781e3          	beq	a5,s0,2005b9cc <__reserved_get_from_poll+0x5a>
2005ba2e:	4601                	c.li	a2,0
2005ba30:	4581                	c.li	a1,0
2005ba32:	59c18513          	addi	a0,gp,1436 # 20066818 <mutex_pool>
2005ba36:	dffae097          	auipc	ra,0xdffae
2005ba3a:	098080e7          	jalr	ra,152(ra) # 9ace <__wrap_memset>
2005ba3e:	f001a623          	sw	zero,-244(gp) # 20066188 <sema_buf_used_num>
2005ba42:	f3418513          	addi	a0,gp,-204 # 200661b0 <wrapper_sema_buf_list>
2005ba46:	f001aa23          	sw	zero,-236(gp) # 20066190 <sema_max_buf_used_num>
2005ba4a:	c108                	c.sw	a0,0(a0)
2005ba4c:	c148                	c.sw	a0,4(a0)
2005ba4e:	f001a823          	sw	zero,-240(gp) # 2006618c <sema_dynamic_num>
2005ba52:	008ba023          	sw	s0,0(s7)
2005ba56:	bf9d                	c.j	2005b9cc <__reserved_get_from_poll+0x5a>
2005ba58:	f2818b93          	addi	s7,gp,-216 # 200661a4 <timer_pool_init_flag>
2005ba5c:	000ba783          	lw	a5,0(s7)
2005ba60:	f68786e3          	beq	a5,s0,2005b9cc <__reserved_get_from_poll+0x5a>
2005ba64:	4601                	c.li	a2,0
2005ba66:	4581                	c.li	a1,0
2005ba68:	59c18513          	addi	a0,gp,1436 # 20066818 <mutex_pool>
2005ba6c:	dffae097          	auipc	ra,0xdffae
2005ba70:	062080e7          	jalr	ra,98(ra) # 9ace <__wrap_memset>
2005ba74:	f001ae23          	sw	zero,-228(gp) # 20066198 <timer_buf_used_num>
2005ba78:	f3c18513          	addi	a0,gp,-196 # 200661b8 <wrapper_timer_buf_list>
2005ba7c:	f201a223          	sw	zero,-220(gp) # 200661a0 <timer_max_buf_used_num>
2005ba80:	c108                	c.sw	a0,0(a0)
2005ba82:	c148                	c.sw	a0,4(a0)
2005ba84:	f201a023          	sw	zero,-224(gp) # 2006619c <timer_dynamic_num>
2005ba88:	b7e9                	c.j	2005ba52 <__reserved_get_from_poll+0xe0>
2005ba8a:	405c                	c.lw	a5,4(s0)
2005ba8c:	4018                	c.lw	a4,0(s0)
2005ba8e:	00840b13          	addi	s6,s0,8
2005ba92:	c35c                	c.sw	a5,4(a4)
2005ba94:	c398                	c.sw	a4,0(a5)
2005ba96:	c000                	c.sw	s0,0(s0)
2005ba98:	c040                	c.sw	s0,4(s0)
2005ba9a:	0009a783          	lw	a5,0(s3)
2005ba9e:	0785                	c.addi	a5,1
2005baa0:	00f9a023          	sw	a5,0(s3)
2005baa4:	be9ff0ef          	jal	ra,2005b68c <rtos_critical_exit>
2005baa8:	57e1                	c.li	a5,-8
2005baaa:	f2f40ae3          	beq	s0,a5,2005b9de <__reserved_get_from_poll+0x6c>
2005baae:	478d                	c.li	a5,3
2005bab0:	05000613          	addi	a2,zero,80
2005bab4:	00f49463          	bne	s1,a5,2005babc <__reserved_get_from_poll+0x14a>
2005bab8:	02c00613          	addi	a2,zero,44
2005babc:	4581                	c.li	a1,0
2005babe:	855a                	c.mv	a0,s6
2005bac0:	dffae097          	auipc	ra,0xdffae
2005bac4:	00e080e7          	jalr	ra,14(ra) # 9ace <__wrap_memset>
2005bac8:	b71d                	c.j	2005b9ee <__reserved_get_from_poll+0x7c>

2005baca <__reserved_get_mutex_from_poll>:
2005baca:	f0418793          	addi	a5,gp,-252 # 20066180 <mutex_max_buf_used_num>
2005bace:	efc18713          	addi	a4,gp,-260 # 20066178 <mutex_buf_used_num>
2005bad2:	f0018693          	addi	a3,gp,-256 # 2006617c <mutex_dynamic_num>
2005bad6:	f0818613          	addi	a2,gp,-248 # 20066184 <mutex_pool_init_flag>
2005bada:	f2c18593          	addi	a1,gp,-212 # 200661a8 <wrapper_mutex_buf_list>
2005bade:	4505                	c.li	a0,1
2005bae0:	e93ff06f          	jal	zero,2005b972 <__reserved_get_from_poll>

2005bae4 <__reserved_release_mutex_to_poll>:
2005bae4:	1141                	c.addi	sp,-16
2005bae6:	c606                	c.swsp	ra,12(sp)
2005bae8:	b81ff0ef          	jal	ra,2005b668 <rtos_critical_enter>
2005baec:	f0018793          	addi	a5,gp,-256 # 2006617c <mutex_dynamic_num>
2005baf0:	4398                	c.lw	a4,0(a5)
2005baf2:	177d                	c.addi	a4,-1
2005baf4:	c398                	c.sw	a4,0(a5)
2005baf6:	40b2                	c.lwsp	ra,12(sp)
2005baf8:	0141                	c.addi	sp,16
2005bafa:	b93ff06f          	jal	zero,2005b68c <rtos_critical_exit>

2005bafe <__reserved_get_sema_from_poll>:
2005bafe:	f1418793          	addi	a5,gp,-236 # 20066190 <sema_max_buf_used_num>
2005bb02:	f0c18713          	addi	a4,gp,-244 # 20066188 <sema_buf_used_num>
2005bb06:	f1018693          	addi	a3,gp,-240 # 2006618c <sema_dynamic_num>
2005bb0a:	f1818613          	addi	a2,gp,-232 # 20066194 <sema_pool_init_flag>
2005bb0e:	f3418593          	addi	a1,gp,-204 # 200661b0 <wrapper_sema_buf_list>
2005bb12:	4509                	c.li	a0,2
2005bb14:	e5fff06f          	jal	zero,2005b972 <__reserved_get_from_poll>

2005bb18 <rtos_sched_start>:
2005bb18:	1141                	c.addi	sp,-16
2005bb1a:	c606                	c.swsp	ra,12(sp)
2005bb1c:	060020ef          	jal	ra,2005db7c <vTaskStartScheduler>
2005bb20:	40b2                	c.lwsp	ra,12(sp)
2005bb22:	4501                	c.li	a0,0
2005bb24:	0141                	c.addi	sp,16
2005bb26:	8082                	c.jr	ra

2005bb28 <rtos_sched_get_state>:
2005bb28:	1141                	c.addi	sp,-16
2005bb2a:	c606                	c.swsp	ra,12(sp)
2005bb2c:	4f7010ef          	jal	ra,2005d822 <xTaskGetSchedulerState>
2005bb30:	4785                	c.li	a5,1
2005bb32:	00f50b63          	beq	a0,a5,2005bb48 <rtos_sched_get_state+0x20>
2005bb36:	4ed010ef          	jal	ra,2005d822 <xTaskGetSchedulerState>
2005bb3a:	c519                	c.beqz	a0,2005bb48 <rtos_sched_get_state+0x20>
2005bb3c:	4e7010ef          	jal	ra,2005d822 <xTaskGetSchedulerState>
2005bb40:	4789                	c.li	a5,2
2005bb42:	00f50363          	beq	a0,a5,2005bb48 <rtos_sched_get_state+0x20>
2005bb46:	557d                	c.li	a0,-1
2005bb48:	40b2                	c.lwsp	ra,12(sp)
2005bb4a:	0141                	c.addi	sp,16
2005bb4c:	8082                	c.jr	ra

2005bb4e <rtos_task_create>:
2005bb4e:	882a                	c.mv	a6,a0
2005bb50:	8532                	c.mv	a0,a2
2005bb52:	863a                	c.mv	a2,a4
2005bb54:	1141                	c.addi	sp,-16
2005bb56:	873e                	c.mv	a4,a5
2005bb58:	8209                	c.srli	a2,0x2
2005bb5a:	87c2                	c.mv	a5,a6
2005bb5c:	c606                	c.swsp	ra,12(sp)
2005bb5e:	086020ef          	jal	ra,2005dbe4 <xTaskCreate>
2005bb62:	40b2                	c.lwsp	ra,12(sp)
2005bb64:	157d                	c.addi	a0,-1
2005bb66:	00a03533          	sltu	a0,zero,a0
2005bb6a:	40a00533          	sub	a0,zero,a0
2005bb6e:	0141                	c.addi	sp,16
2005bb70:	8082                	c.jr	ra

2005bb72 <rtos_task_delete>:
2005bb72:	1141                	c.addi	sp,-16
2005bb74:	c606                	c.swsp	ra,12(sp)
2005bb76:	0e6020ef          	jal	ra,2005dc5c <vTaskDelete>
2005bb7a:	40b2                	c.lwsp	ra,12(sp)
2005bb7c:	4501                	c.li	a0,0
2005bb7e:	0141                	c.addi	sp,16
2005bb80:	8082                	c.jr	ra

2005bb82 <rtos_create_secure_context>:
2005bb82:	8082                	c.jr	ra

2005bb84 <rtos_time_delay_ms>:
2005bb84:	1141                	c.addi	sp,-16
2005bb86:	c422                	c.swsp	s0,8(sp)
2005bb88:	c606                	c.swsp	ra,12(sp)
2005bb8a:	842a                	c.mv	s0,a0
2005bb8c:	bfffd0ef          	jal	ra,2005978a <pmu_yield_os_check>
2005bb90:	cd19                	c.beqz	a0,2005bbae <rtos_time_delay_ms+0x2a>
2005bb92:	491010ef          	jal	ra,2005d822 <xTaskGetSchedulerState>
2005bb96:	4789                	c.li	a5,2
2005bb98:	00f51b63          	bne	a0,a5,2005bbae <rtos_time_delay_ms+0x2a>
2005bb9c:	abbff0ef          	jal	ra,2005b656 <rtos_critical_is_in_interrupt>
2005bba0:	e519                	c.bnez	a0,2005bbae <rtos_time_delay_ms+0x2a>
2005bba2:	8522                	c.mv	a0,s0
2005bba4:	4422                	c.lwsp	s0,8(sp)
2005bba6:	40b2                	c.lwsp	ra,12(sp)
2005bba8:	0141                	c.addi	sp,16
2005bbaa:	2cc0206f          	jal	zero,2005de76 <vTaskDelay>
2005bbae:	8522                	c.mv	a0,s0
2005bbb0:	4422                	c.lwsp	s0,8(sp)
2005bbb2:	40b2                	c.lwsp	ra,12(sp)
2005bbb4:	0141                	c.addi	sp,16
2005bbb6:	dffa6317          	auipc	t1,0xdffa6
2005bbba:	76430067          	jalr	zero,1892(t1) # 231a <DelayMs>

2005bbbe <rtos_time_get_current_system_time_ms>:
2005bbbe:	1141                	c.addi	sp,-16
2005bbc0:	c606                	c.swsp	ra,12(sp)
2005bbc2:	a95ff0ef          	jal	ra,2005b656 <rtos_critical_is_in_interrupt>
2005bbc6:	c509                	c.beqz	a0,2005bbd0 <rtos_time_get_current_system_time_ms+0x12>
2005bbc8:	40b2                	c.lwsp	ra,12(sp)
2005bbca:	0141                	c.addi	sp,16
2005bbcc:	11f0106f          	jal	zero,2005d4ea <xTaskGetTickCountFromISR>
2005bbd0:	bbbfd0ef          	jal	ra,2005978a <pmu_yield_os_check>
2005bbd4:	d975                	c.beqz	a0,2005bbc8 <rtos_time_get_current_system_time_ms+0xa>
2005bbd6:	40b2                	c.lwsp	ra,12(sp)
2005bbd8:	0141                	c.addi	sp,16
2005bbda:	5420206f          	jal	zero,2005e11c <xTaskGetTickCount>

2005bbde <rtos_timer_create>:
2005bbde:	1141                	c.addi	sp,-16
2005bbe0:	c422                	c.swsp	s0,8(sp)
2005bbe2:	c606                	c.swsp	ra,12(sp)
2005bbe4:	842a                	c.mv	s0,a0
2005bbe6:	587d                	c.li	a6,-1
2005bbe8:	c105                	c.beqz	a0,2005bc08 <rtos_timer_create+0x2a>
2005bbea:	88b2                	c.mv	a7,a2
2005bbec:	587d                	c.li	a6,-1
2005bbee:	863a                	c.mv	a2,a4
2005bbf0:	873e                	c.mv	a4,a5
2005bbf2:	cb99                	c.beqz	a5,2005bc08 <rtos_timer_create+0x2a>
2005bbf4:	852e                	c.mv	a0,a1
2005bbf6:	85b6                	c.mv	a1,a3
2005bbf8:	86c6                	c.mv	a3,a7
2005bbfa:	768020ef          	jal	ra,2005e362 <xTimerCreate>
2005bbfe:	c008                	c.sw	a0,0(s0)
2005bc00:	00153513          	sltiu	a0,a0,1
2005bc04:	40a00833          	sub	a6,zero,a0
2005bc08:	40b2                	c.lwsp	ra,12(sp)
2005bc0a:	4422                	c.lwsp	s0,8(sp)
2005bc0c:	8542                	c.mv	a0,a6
2005bc0e:	0141                	c.addi	sp,16
2005bc10:	8082                	c.jr	ra

2005bc12 <rtos_timer_start>:
2005bc12:	e909                	c.bnez	a0,2005bc24 <rtos_timer_start+0x12>
2005bc14:	557d                	c.li	a0,-1
2005bc16:	8082                	c.jr	ra
2005bc18:	557d                	c.li	a0,-1
2005bc1a:	40f2                	c.lwsp	ra,28(sp)
2005bc1c:	4462                	c.lwsp	s0,24(sp)
2005bc1e:	44d2                	c.lwsp	s1,20(sp)
2005bc20:	6105                	c.addi16sp	sp,32
2005bc22:	8082                	c.jr	ra
2005bc24:	1101                	c.addi	sp,-32
2005bc26:	cc22                	c.swsp	s0,24(sp)
2005bc28:	ca26                	c.swsp	s1,20(sp)
2005bc2a:	ce06                	c.swsp	ra,28(sp)
2005bc2c:	842a                	c.mv	s0,a0
2005bc2e:	84ae                	c.mv	s1,a1
2005bc30:	a27ff0ef          	jal	ra,2005b656 <rtos_critical_is_in_interrupt>
2005bc34:	c505                	c.beqz	a0,2005bc5c <rtos_timer_start+0x4a>
2005bc36:	c602                	c.swsp	zero,12(sp)
2005bc38:	0b3010ef          	jal	ra,2005d4ea <xTaskGetTickCountFromISR>
2005bc3c:	862a                	c.mv	a2,a0
2005bc3e:	4701                	c.li	a4,0
2005bc40:	0074                	c.addi4spn	a3,sp,12
2005bc42:	4599                	c.li	a1,6
2005bc44:	8522                	c.mv	a0,s0
2005bc46:	784020ef          	jal	ra,2005e3ca <xTimerGenericCommand>
2005bc4a:	4785                	c.li	a5,1
2005bc4c:	fcf516e3          	bne	a0,a5,2005bc18 <rtos_timer_start+0x6>
2005bc50:	4532                	c.lwsp	a0,12(sp)
2005bc52:	d561                	c.beqz	a0,2005bc1a <rtos_timer_start+0x8>
2005bc54:	1e5010ef          	jal	ra,2005d638 <vTaskSwitchContext>
2005bc58:	4501                	c.li	a0,0
2005bc5a:	b7c1                	c.j	2005bc1a <rtos_timer_start+0x8>
2005bc5c:	4c0020ef          	jal	ra,2005e11c <xTaskGetTickCount>
2005bc60:	862a                	c.mv	a2,a0
2005bc62:	8726                	c.mv	a4,s1
2005bc64:	4681                	c.li	a3,0
2005bc66:	4585                	c.li	a1,1
2005bc68:	8522                	c.mv	a0,s0
2005bc6a:	760020ef          	jal	ra,2005e3ca <xTimerGenericCommand>
2005bc6e:	157d                	c.addi	a0,-1
2005bc70:	00a03533          	sltu	a0,zero,a0
2005bc74:	40a00533          	sub	a0,zero,a0
2005bc78:	b74d                	c.j	2005bc1a <rtos_timer_start+0x8>

2005bc7a <platform_calloc_uninit>:
2005bc7a:	4501                	c.li	a0,0
2005bc7c:	8082                	c.jr	ra

2005bc7e <platform_free_uninit>:
2005bc7e:	8082                	c.jr	ra

2005bc80 <mbedtls_platform_set_calloc_free>:
2005bc80:	e8a1a623          	sw	a0,-372(gp) # 20066108 <mbedtls_calloc_func>
2005bc84:	e8b1a823          	sw	a1,-368(gp) # 2006610c <mbedtls_free_func>
2005bc88:	200017b7          	lui	a5,0x20001
2005bc8c:	c6c78793          	addi	a5,a5,-916 # 20000c6c <rom_ssl_ram_map>
2005bc90:	20001737          	lui	a4,0x20001
2005bc94:	c388                	c.sw	a0,0(a5)
2005bc96:	c6f72423          	sw	a5,-920(a4) # 20000c68 <p_rom_ssl_ram_map>
2005bc9a:	c3cc                	c.sw	a1,4(a5)
2005bc9c:	4501                	c.li	a0,0
2005bc9e:	8082                	c.jr	ra

2005bca0 <freertos_risc_v_trap_handler>:
2005bca0:	ef810113          	addi	sp,sp,-264
2005bca4:	c206                	c.swsp	ra,4(sp)
2005bca6:	ca16                	c.swsp	t0,20(sp)
2005bca8:	cc1a                	c.swsp	t1,24(sp)
2005bcaa:	ce1e                	c.swsp	t2,28(sp)
2005bcac:	d022                	c.swsp	s0,32(sp)
2005bcae:	d226                	c.swsp	s1,36(sp)
2005bcb0:	d42a                	c.swsp	a0,40(sp)
2005bcb2:	d62e                	c.swsp	a1,44(sp)
2005bcb4:	d832                	c.swsp	a2,48(sp)
2005bcb6:	da36                	c.swsp	a3,52(sp)
2005bcb8:	dc3a                	c.swsp	a4,56(sp)
2005bcba:	de3e                	c.swsp	a5,60(sp)
2005bcbc:	c0c2                	c.swsp	a6,64(sp)
2005bcbe:	c2c6                	c.swsp	a7,68(sp)
2005bcc0:	c4ca                	c.swsp	s2,72(sp)
2005bcc2:	c6ce                	c.swsp	s3,76(sp)
2005bcc4:	c8d2                	c.swsp	s4,80(sp)
2005bcc6:	cad6                	c.swsp	s5,84(sp)
2005bcc8:	ccda                	c.swsp	s6,88(sp)
2005bcca:	cede                	c.swsp	s7,92(sp)
2005bccc:	d0e2                	c.swsp	s8,96(sp)
2005bcce:	d2e6                	c.swsp	s9,100(sp)
2005bcd0:	d4ea                	c.swsp	s10,104(sp)
2005bcd2:	d6ee                	c.swsp	s11,108(sp)
2005bcd4:	d8f2                	c.swsp	t3,112(sp)
2005bcd6:	daf6                	c.swsp	t4,116(sp)
2005bcd8:	dcfa                	c.swsp	t5,120(sp)
2005bcda:	defe                	c.swsp	t6,124(sp)
2005bcdc:	300022f3          	csrrs	t0,mstatus,zero
2005bce0:	6319                	c.lui	t1,0x6
2005bce2:	0062f2b3          	and	t0,t0,t1
2005bce6:	6309                	c.lui	t1,0x2
2005bce8:	04628663          	beq	t0,t1,2005bd34 <freertos_risc_v_trap_handler+0x94>
2005bcec:	003022f3          	csrrs	t0,fcsr,zero
2005bcf0:	e102                	c.fswsp	ft0,128(sp)
2005bcf2:	e306                	c.fswsp	ft1,132(sp)
2005bcf4:	e50a                	c.fswsp	ft2,136(sp)
2005bcf6:	e70e                	c.fswsp	ft3,140(sp)
2005bcf8:	e912                	c.fswsp	ft4,144(sp)
2005bcfa:	eb16                	c.fswsp	ft5,148(sp)
2005bcfc:	ed1a                	c.fswsp	ft6,152(sp)
2005bcfe:	ef1e                	c.fswsp	ft7,156(sp)
2005bd00:	f122                	c.fswsp	fs0,160(sp)
2005bd02:	f326                	c.fswsp	fs1,164(sp)
2005bd04:	f52a                	c.fswsp	fa0,168(sp)
2005bd06:	f72e                	c.fswsp	fa1,172(sp)
2005bd08:	f932                	c.fswsp	fa2,176(sp)
2005bd0a:	fb36                	c.fswsp	fa3,180(sp)
2005bd0c:	fd3a                	c.fswsp	fa4,184(sp)
2005bd0e:	ff3e                	c.fswsp	fa5,188(sp)
2005bd10:	e1c2                	c.fswsp	fa6,192(sp)
2005bd12:	e3c6                	c.fswsp	fa7,196(sp)
2005bd14:	e5ca                	c.fswsp	fs2,200(sp)
2005bd16:	e7ce                	c.fswsp	fs3,204(sp)
2005bd18:	e9d2                	c.fswsp	fs4,208(sp)
2005bd1a:	ebd6                	c.fswsp	fs5,212(sp)
2005bd1c:	edda                	c.fswsp	fs6,216(sp)
2005bd1e:	efde                	c.fswsp	fs7,220(sp)
2005bd20:	f1e2                	c.fswsp	fs8,224(sp)
2005bd22:	f3e6                	c.fswsp	fs9,228(sp)
2005bd24:	f5ea                	c.fswsp	fs10,232(sp)
2005bd26:	f7ee                	c.fswsp	fs11,236(sp)
2005bd28:	f9f2                	c.fswsp	ft8,240(sp)
2005bd2a:	fbf6                	c.fswsp	ft9,244(sp)
2005bd2c:	fdfa                	c.fswsp	ft10,248(sp)
2005bd2e:	fffe                	c.fswsp	ft11,252(sp)
2005bd30:	10512023          	sw	t0,256(sp)
2005bd34:	300022f3          	csrrs	t0,mstatus,zero
2005bd38:	10512223          	sw	t0,260(sp)
2005bd3c:	34202573          	csrrs	a0,mcause,zero
2005bd40:	341025f3          	csrrs	a1,mepc,zero

2005bd44 <test_if_asynchronous>:
2005bd44:	01f55613          	srli	a2,a0,0x1f
2005bd48:	e225                	c.bnez	a2,2005bda8 <handle_asynchronous>

2005bd4a <handle_synchronous>:
2005bd4a:	0591                	c.addi	a1,4
2005bd4c:	c02e                	c.swsp	a1,0(sp)

2005bd4e <test_if_environment_call>:
2005bd4e:	42ad                	c.li	t0,11
2005bd50:	00a2cd63          	blt	t0,a0,2005bd6a <_crash_dump>
2005bd54:	42a1                	c.li	t0,8
2005bd56:	00554a63          	blt	a0,t0,2005bd6a <_crash_dump>
2005bd5a:	a009                	c.j	2005bd5c <ecall_yield>

2005bd5c <ecall_yield>:
2005bd5c:	f601a283          	lw	t0,-160(gp) # 200661dc <pxCurrentTCB>
2005bd60:	0022a023          	sw	sp,0(t0)
2005bd64:	e681a103          	lw	sp,-408(gp) # 200660e4 <xISRStackTop>
2005bd68:	a0a5                	c.j	2005bdd0 <task_context_switch>

2005bd6a <_crash_dump>:
2005bd6a:	34102573          	csrrs	a0,mepc,zero
2005bd6e:	022c                	c.addi4spn	a1,sp,264
2005bd70:	860a                	c.mv	a2,sp
2005bd72:	c002                	c.swsp	zero,0(sp)
2005bd74:	c42e                	c.swsp	a1,8(sp)
2005bd76:	c60e                	c.swsp	gp,12(sp)
2005bd78:	c812                	c.swsp	tp,16(sp)
2005bd7a:	e681a303          	lw	t1,-408(gp) # 200660e4 <xISRStackTop>
2005bd7e:	0000b397          	auipc	t2,0xb
2005bd82:	aa238393          	addi	t2,t2,-1374 # 20066820 <xISRStack>
2005bd86:	08038393          	addi	t2,t2,128
2005bd8a:	00615563          	bge	sp,t1,2005bd94 <not_isrstack>
2005bd8e:	00714363          	blt	sp,t2,2005bd94 <not_isrstack>
2005bd92:	a019                	c.j	2005bd98 <is_isrstack>

2005bd94 <not_isrstack>:
2005bd94:	e681a103          	lw	sp,-408(gp) # 200660e4 <xISRStackTop>

2005bd98 <is_isrstack>:
2005bd98:	00004297          	auipc	t0,0x4
2005bd9c:	fd028293          	addi	t0,t0,-48 # 2005fd68 <crash_dump>
2005bda0:	9282                	c.jalr	t0

2005bda2 <crash_dump_end>:
2005bda2:	a001                	c.j	2005bda2 <crash_dump_end>

2005bda4 <unrecoverable_error>:
2005bda4:	9002                	c.ebreak
2005bda6:	bffd                	c.j	2005bda4 <unrecoverable_error>

2005bda8 <handle_asynchronous>:
2005bda8:	c02e                	c.swsp	a1,0(sp)
2005bdaa:	f601a283          	lw	t0,-160(gp) # 200661dc <pxCurrentTCB>
2005bdae:	0022a023          	sw	sp,0(t0)
2005bdb2:	e681a103          	lw	sp,-408(gp) # 200660e4 <xISRStackTop>

2005bdb6 <nonvec_handle>:
2005bdb6:	4285                	c.li	t0,1
2005bdb8:	02fe                	c.slli	t0,0x1f
2005bdba:	00728313          	addi	t1,t0,7
2005bdbe:	00651763          	bne	a0,t1,2005bdcc <test_if_external_interrupt>
2005bdc2:	28d9                	c.jal	2005be98 <machine_timer_update>
2005bdc4:	73e010ef          	jal	ra,2005d502 <xTaskIncrementTick>
2005bdc8:	c909                	c.beqz	a0,2005bdda <processed_source>
2005bdca:	a019                	c.j	2005bdd0 <task_context_switch>

2005bdcc <test_if_external_interrupt>:
2005bdcc:	2211                	c.jal	2005bed0 <vPortHandleInterrupt>
2005bdce:	a031                	c.j	2005bdda <processed_source>

2005bdd0 <task_context_switch>:
2005bdd0:	069010ef          	jal	ra,2005d638 <vTaskSwitchContext>
2005bdd4:	a019                	c.j	2005bdda <processed_source>

2005bdd6 <as_yet_unhandled>:
2005bdd6:	9002                	c.ebreak
2005bdd8:	bffd                	c.j	2005bdd6 <as_yet_unhandled>

2005bdda <processed_source>:
2005bdda:	f601a303          	lw	t1,-160(gp) # 200661dc <pxCurrentTCB>
2005bdde:	00032103          	lw	sp,0(t1) # 2000 <CRYPTO_Init+0x86a>

2005bde2 <restore_regs>:
2005bde2:	4282                	c.lwsp	t0,0(sp)
2005bde4:	34129073          	csrrw	zero,mepc,t0
2005bde8:	10412283          	lw	t0,260(sp)
2005bdec:	30029073          	csrrw	zero,mstatus,t0

2005bdf0 <_stack_pop>:
2005bdf0:	300022f3          	csrrs	t0,mstatus,zero
2005bdf4:	6319                	c.lui	t1,0x6
2005bdf6:	0062f2b3          	and	t0,t0,t1
2005bdfa:	6309                	c.lui	t1,0x2
2005bdfc:	04628c63          	beq	t0,t1,2005be54 <_stack_pop+0x64>
2005be00:	600a                	c.flwsp	ft0,128(sp)
2005be02:	609a                	c.flwsp	ft1,132(sp)
2005be04:	612a                	c.flwsp	ft2,136(sp)
2005be06:	61ba                	c.flwsp	ft3,140(sp)
2005be08:	624a                	c.flwsp	ft4,144(sp)
2005be0a:	62da                	c.flwsp	ft5,148(sp)
2005be0c:	636a                	c.flwsp	ft6,152(sp)
2005be0e:	63fa                	c.flwsp	ft7,156(sp)
2005be10:	740a                	c.flwsp	fs0,160(sp)
2005be12:	749a                	c.flwsp	fs1,164(sp)
2005be14:	752a                	c.flwsp	fa0,168(sp)
2005be16:	75ba                	c.flwsp	fa1,172(sp)
2005be18:	764a                	c.flwsp	fa2,176(sp)
2005be1a:	76da                	c.flwsp	fa3,180(sp)
2005be1c:	776a                	c.flwsp	fa4,184(sp)
2005be1e:	77fa                	c.flwsp	fa5,188(sp)
2005be20:	680e                	c.flwsp	fa6,192(sp)
2005be22:	689e                	c.flwsp	fa7,196(sp)
2005be24:	692e                	c.flwsp	fs2,200(sp)
2005be26:	69be                	c.flwsp	fs3,204(sp)
2005be28:	6a4e                	c.flwsp	fs4,208(sp)
2005be2a:	6ade                	c.flwsp	fs5,212(sp)
2005be2c:	6b6e                	c.flwsp	fs6,216(sp)
2005be2e:	6bfe                	c.flwsp	fs7,220(sp)
2005be30:	7c0e                	c.flwsp	fs8,224(sp)
2005be32:	7c9e                	c.flwsp	fs9,228(sp)
2005be34:	7d2e                	c.flwsp	fs10,232(sp)
2005be36:	7dbe                	c.flwsp	fs11,236(sp)
2005be38:	7e4e                	c.flwsp	ft8,240(sp)
2005be3a:	7ede                	c.flwsp	ft9,244(sp)
2005be3c:	7f6e                	c.flwsp	ft10,248(sp)
2005be3e:	7ffe                	c.flwsp	ft11,252(sp)
2005be40:	10012283          	lw	t0,256(sp)
2005be44:	00329073          	csrrw	zero,fcsr,t0
2005be48:	6319                	c.lui	t1,0x6
2005be4a:	30033073          	csrrc	zero,mstatus,t1
2005be4e:	6311                	c.lui	t1,0x4
2005be50:	30032073          	csrrs	zero,mstatus,t1
2005be54:	4092                	c.lwsp	ra,4(sp)
2005be56:	42d2                	c.lwsp	t0,20(sp)
2005be58:	4362                	c.lwsp	t1,24(sp)
2005be5a:	43f2                	c.lwsp	t2,28(sp)
2005be5c:	5402                	c.lwsp	s0,32(sp)
2005be5e:	5492                	c.lwsp	s1,36(sp)
2005be60:	5522                	c.lwsp	a0,40(sp)
2005be62:	55b2                	c.lwsp	a1,44(sp)
2005be64:	5642                	c.lwsp	a2,48(sp)
2005be66:	56d2                	c.lwsp	a3,52(sp)
2005be68:	5762                	c.lwsp	a4,56(sp)
2005be6a:	57f2                	c.lwsp	a5,60(sp)
2005be6c:	4806                	c.lwsp	a6,64(sp)
2005be6e:	4896                	c.lwsp	a7,68(sp)
2005be70:	4926                	c.lwsp	s2,72(sp)
2005be72:	49b6                	c.lwsp	s3,76(sp)
2005be74:	4a46                	c.lwsp	s4,80(sp)
2005be76:	4ad6                	c.lwsp	s5,84(sp)
2005be78:	4b66                	c.lwsp	s6,88(sp)
2005be7a:	4bf6                	c.lwsp	s7,92(sp)
2005be7c:	5c06                	c.lwsp	s8,96(sp)
2005be7e:	5c96                	c.lwsp	s9,100(sp)
2005be80:	5d26                	c.lwsp	s10,104(sp)
2005be82:	5db6                	c.lwsp	s11,108(sp)
2005be84:	5e46                	c.lwsp	t3,112(sp)
2005be86:	5ed6                	c.lwsp	t4,116(sp)
2005be88:	5f66                	c.lwsp	t5,120(sp)
2005be8a:	5ff6                	c.lwsp	t6,124(sp)
2005be8c:	10810113          	addi	sp,sp,264
2005be90:	30200073          	mret
2005be94:	00000013          	addi	zero,zero,0

2005be98 <machine_timer_update>:
2005be98:	f441a283          	lw	t0,-188(gp) # 200661c0 <pullMachineTimerCompareRegister>
2005be9c:	e941a303          	lw	t1,-364(gp) # 20066110 <pullNextTime>
2005bea0:	5efd                	c.li	t4,-1
2005bea2:	00032383          	lw	t2,0(t1) # 4000 <GPIO_Direction+0x36>
2005bea6:	00432e03          	lw	t3,4(t1)
2005beaa:	01d2a023          	sw	t4,0(t0)
2005beae:	01c2a223          	sw	t3,4(t0)
2005beb2:	0072a023          	sw	t2,0(t0)
2005beb6:	f541a283          	lw	t0,-172(gp) # 200661d0 <uxTimerIncrementsForOneTick>
2005beba:	00728eb3          	add	t4,t0,t2
2005bebe:	007ebf33          	sltu	t5,t4,t2
2005bec2:	01ee0fb3          	add	t6,t3,t5
2005bec6:	01d32023          	sw	t4,0(t1)
2005beca:	01f32223          	sw	t6,4(t1)
2005bece:	8082                	c.jr	ra

2005bed0 <vPortHandleInterrupt>:
2005bed0:	42bd                	c.li	t0,15
2005bed2:	fff2c293          	xori	t0,t0,-1
2005bed6:	00517133          	and	sp,sp,t0
2005beda:	1141                	c.addi	sp,-16
2005bedc:	c006                	c.swsp	ra,0(sp)
2005bede:	34202373          	csrrs	t1,mcause,zero
2005bee2:	800003b7          	lui	t2,0x80000
2005bee6:	fff3c393          	xori	t2,t2,-1
2005beea:	007373b3          	and	t2,t1,t2
2005beee:	00038533          	add	a0,t2,zero
2005bef2:	0000a797          	auipc	a5,0xa
2005bef6:	e7278793          	addi	a5,a5,-398 # 20065d64 <plic_interrupt_handler>
2005befa:	9782                	c.jalr	a5

2005befc <vPortHandleInterrupt_Exit>:
2005befc:	4082                	c.lwsp	ra,0(sp)
2005befe:	0141                	c.addi	sp,16
2005bf00:	8082                	c.jr	ra
2005bf02:	0001                	c.addi	zero,0
2005bf04:	00000013          	addi	zero,zero,0

2005bf08 <xPortStartFirstTask>:
2005bf08:	00000297          	auipc	t0,0x0
2005bf0c:	d9828293          	addi	t0,t0,-616 # 2005bca0 <freertos_risc_v_trap_handler>
2005bf10:	30529073          	csrrw	zero,mtvec,t0
2005bf14:	f601a383          	lw	t2,-160(gp) # 200661dc <pxCurrentTCB>
2005bf18:	0003a103          	lw	sp,0(t2) # 80000000 <__ctrace_end__+0xb6000000>
2005bf1c:	300022f3          	csrrs	t0,mstatus,zero
2005bf20:	6319                	c.lui	t1,0x6
2005bf22:	0062f2b3          	and	t0,t0,t1
2005bf26:	6309                	c.lui	t1,0x2
2005bf28:	04628c63          	beq	t0,t1,2005bf80 <xPortStartFirstTask+0x78>
2005bf2c:	600a                	c.flwsp	ft0,128(sp)
2005bf2e:	609a                	c.flwsp	ft1,132(sp)
2005bf30:	612a                	c.flwsp	ft2,136(sp)
2005bf32:	61ba                	c.flwsp	ft3,140(sp)
2005bf34:	624a                	c.flwsp	ft4,144(sp)
2005bf36:	62da                	c.flwsp	ft5,148(sp)
2005bf38:	636a                	c.flwsp	ft6,152(sp)
2005bf3a:	63fa                	c.flwsp	ft7,156(sp)
2005bf3c:	740a                	c.flwsp	fs0,160(sp)
2005bf3e:	749a                	c.flwsp	fs1,164(sp)
2005bf40:	752a                	c.flwsp	fa0,168(sp)
2005bf42:	75ba                	c.flwsp	fa1,172(sp)
2005bf44:	764a                	c.flwsp	fa2,176(sp)
2005bf46:	76da                	c.flwsp	fa3,180(sp)
2005bf48:	776a                	c.flwsp	fa4,184(sp)
2005bf4a:	77fa                	c.flwsp	fa5,188(sp)
2005bf4c:	680e                	c.flwsp	fa6,192(sp)
2005bf4e:	689e                	c.flwsp	fa7,196(sp)
2005bf50:	692e                	c.flwsp	fs2,200(sp)
2005bf52:	69be                	c.flwsp	fs3,204(sp)
2005bf54:	6a4e                	c.flwsp	fs4,208(sp)
2005bf56:	6ade                	c.flwsp	fs5,212(sp)
2005bf58:	6b6e                	c.flwsp	fs6,216(sp)
2005bf5a:	6bfe                	c.flwsp	fs7,220(sp)
2005bf5c:	7c0e                	c.flwsp	fs8,224(sp)
2005bf5e:	7c9e                	c.flwsp	fs9,228(sp)
2005bf60:	7d2e                	c.flwsp	fs10,232(sp)
2005bf62:	7dbe                	c.flwsp	fs11,236(sp)
2005bf64:	7e4e                	c.flwsp	ft8,240(sp)
2005bf66:	7ede                	c.flwsp	ft9,244(sp)
2005bf68:	7f6e                	c.flwsp	ft10,248(sp)
2005bf6a:	7ffe                	c.flwsp	ft11,252(sp)
2005bf6c:	10012283          	lw	t0,256(sp)
2005bf70:	00329073          	csrrw	zero,fcsr,t0
2005bf74:	6319                	c.lui	t1,0x6
2005bf76:	30033073          	csrrc	zero,mstatus,t1
2005bf7a:	6311                	c.lui	t1,0x4
2005bf7c:	30032073          	csrrs	zero,mstatus,t1
2005bf80:	4092                	c.lwsp	ra,4(sp)
2005bf82:	42d2                	c.lwsp	t0,20(sp)
2005bf84:	4362                	c.lwsp	t1,24(sp)
2005bf86:	43f2                	c.lwsp	t2,28(sp)
2005bf88:	5402                	c.lwsp	s0,32(sp)
2005bf8a:	5492                	c.lwsp	s1,36(sp)
2005bf8c:	5522                	c.lwsp	a0,40(sp)
2005bf8e:	55b2                	c.lwsp	a1,44(sp)
2005bf90:	5642                	c.lwsp	a2,48(sp)
2005bf92:	56d2                	c.lwsp	a3,52(sp)
2005bf94:	5762                	c.lwsp	a4,56(sp)
2005bf96:	57f2                	c.lwsp	a5,60(sp)
2005bf98:	4806                	c.lwsp	a6,64(sp)
2005bf9a:	4896                	c.lwsp	a7,68(sp)
2005bf9c:	4926                	c.lwsp	s2,72(sp)
2005bf9e:	49b6                	c.lwsp	s3,76(sp)
2005bfa0:	4a46                	c.lwsp	s4,80(sp)
2005bfa2:	4ad6                	c.lwsp	s5,84(sp)
2005bfa4:	4b66                	c.lwsp	s6,88(sp)
2005bfa6:	4bf6                	c.lwsp	s7,92(sp)
2005bfa8:	5c06                	c.lwsp	s8,96(sp)
2005bfaa:	5c96                	c.lwsp	s9,100(sp)
2005bfac:	5d26                	c.lwsp	s10,104(sp)
2005bfae:	5db6                	c.lwsp	s11,108(sp)
2005bfb0:	5e46                	c.lwsp	t3,112(sp)
2005bfb2:	5ed6                	c.lwsp	t4,116(sp)
2005bfb4:	5f66                	c.lwsp	t5,120(sp)
2005bfb6:	5ff6                	c.lwsp	t6,124(sp)
2005bfb8:	10412283          	lw	t0,260(sp)
2005bfbc:	0082e293          	ori	t0,t0,8
2005bfc0:	30029073          	csrrw	zero,mstatus,t0
2005bfc4:	42d2                	c.lwsp	t0,20(sp)
2005bfc6:	4082                	c.lwsp	ra,0(sp)
2005bfc8:	10810113          	addi	sp,sp,264
2005bfcc:	8082                	c.jr	ra
2005bfce:	0001                	c.addi	zero,0

2005bfd0 <pxPortInitialiseStack>:
2005bfd0:	300022f3          	csrrs	t0,mstatus,zero
2005bfd4:	ff72f293          	andi	t0,t0,-9
2005bfd8:	18800313          	addi	t1,zero,392
2005bfdc:	0312                	c.slli	t1,0x4
2005bfde:	0062e2b3          	or	t0,t0,t1
2005bfe2:	1571                	c.addi	a0,-4
2005bfe4:	00552023          	sw	t0,0(a0)
2005bfe8:	f2450513          	addi	a0,a0,-220
2005bfec:	c110                	c.sw	a2,0(a0)
2005bfee:	fdc50513          	addi	a0,a0,-36
2005bff2:	00052023          	sw	zero,0(a0)
2005bff6:	4281                	c.li	t0,0

2005bff8 <chip_specific_stack_frame>:
2005bff8:	00028763          	beq	t0,zero,2005c006 <chip_specific_stack_frame+0xe>
2005bffc:	1571                	c.addi	a0,-4
2005bffe:	00052023          	sw	zero,0(a0)
2005c002:	12fd                	c.addi	t0,-1
2005c004:	bfd5                	c.j	2005bff8 <chip_specific_stack_frame>
2005c006:	1571                	c.addi	a0,-4
2005c008:	c10c                	c.sw	a1,0(a0)
2005c00a:	8082                	c.jr	ra

2005c00c <vPortBackupRegs>:
2005c00c:	00152223          	sw	ra,4(a0)
2005c010:	00252423          	sw	sp,8(a0)
2005c014:	00352623          	sw	gp,12(a0)
2005c018:	00452823          	sw	tp,16(a0)
2005c01c:	00552a23          	sw	t0,20(a0)
2005c020:	00652c23          	sw	t1,24(a0)
2005c024:	00752e23          	sw	t2,28(a0)
2005c028:	d100                	c.sw	s0,32(a0)
2005c02a:	d144                	c.sw	s1,36(a0)
2005c02c:	d508                	c.sw	a0,40(a0)
2005c02e:	d54c                	c.sw	a1,44(a0)
2005c030:	d910                	c.sw	a2,48(a0)
2005c032:	d954                	c.sw	a3,52(a0)
2005c034:	dd18                	c.sw	a4,56(a0)
2005c036:	dd5c                	c.sw	a5,60(a0)
2005c038:	05052023          	sw	a6,64(a0)
2005c03c:	05152223          	sw	a7,68(a0)
2005c040:	05252423          	sw	s2,72(a0)
2005c044:	05352623          	sw	s3,76(a0)
2005c048:	05452823          	sw	s4,80(a0)
2005c04c:	05552a23          	sw	s5,84(a0)
2005c050:	05652c23          	sw	s6,88(a0)
2005c054:	05752e23          	sw	s7,92(a0)
2005c058:	07852023          	sw	s8,96(a0)
2005c05c:	07952223          	sw	s9,100(a0)
2005c060:	07a52423          	sw	s10,104(a0)
2005c064:	07b52623          	sw	s11,108(a0)
2005c068:	07c52823          	sw	t3,112(a0)
2005c06c:	07d52a23          	sw	t4,116(a0)
2005c070:	07e52c23          	sw	t5,120(a0)
2005c074:	07f52e23          	sw	t6,124(a0)
2005c078:	300022f3          	csrrs	t0,mstatus,zero
2005c07c:	0055a023          	sw	t0,0(a1)
2005c080:	304022f3          	csrrs	t0,mie,zero
2005c084:	0055a223          	sw	t0,4(a1)
2005c088:	305022f3          	csrrs	t0,mtvec,zero
2005c08c:	0055a423          	sw	t0,8(a1)
2005c090:	0015a623          	sw	ra,12(a1)
2005c094:	8082                	c.jr	ra

2005c096 <vPortBackupfloatRegs>:
2005c096:	300022f3          	csrrs	t0,mstatus,zero
2005c09a:	6319                	c.lui	t1,0x6
2005c09c:	0062f2b3          	and	t0,t0,t1
2005c0a0:	06629e63          	bne	t0,t1,2005c11c <vPortBackupfloatRegs+0x86>
2005c0a4:	00052027          	fsw	ft0,0(a0)
2005c0a8:	00152227          	fsw	ft1,4(a0)
2005c0ac:	00252427          	fsw	ft2,8(a0)
2005c0b0:	00352627          	fsw	ft3,12(a0)
2005c0b4:	00452827          	fsw	ft4,16(a0)
2005c0b8:	00552a27          	fsw	ft5,20(a0)
2005c0bc:	00652c27          	fsw	ft6,24(a0)
2005c0c0:	00752e27          	fsw	ft7,28(a0)
2005c0c4:	f100                	c.fsw	fs0,32(a0)
2005c0c6:	f144                	c.fsw	fs1,36(a0)
2005c0c8:	f508                	c.fsw	fa0,40(a0)
2005c0ca:	f54c                	c.fsw	fa1,44(a0)
2005c0cc:	f910                	c.fsw	fa2,48(a0)
2005c0ce:	f954                	c.fsw	fa3,52(a0)
2005c0d0:	fd18                	c.fsw	fa4,56(a0)
2005c0d2:	fd5c                	c.fsw	fa5,60(a0)
2005c0d4:	05052027          	fsw	fa6,64(a0)
2005c0d8:	05152227          	fsw	fa7,68(a0)
2005c0dc:	05252427          	fsw	fs2,72(a0)
2005c0e0:	05352627          	fsw	fs3,76(a0)
2005c0e4:	05452827          	fsw	fs4,80(a0)
2005c0e8:	05552a27          	fsw	fs5,84(a0)
2005c0ec:	05652c27          	fsw	fs6,88(a0)
2005c0f0:	05752e27          	fsw	fs7,92(a0)
2005c0f4:	07852027          	fsw	fs8,96(a0)
2005c0f8:	07952227          	fsw	fs9,100(a0)
2005c0fc:	07a52427          	fsw	fs10,104(a0)
2005c100:	07b52627          	fsw	fs11,108(a0)
2005c104:	07c52827          	fsw	ft8,112(a0)
2005c108:	07d52a27          	fsw	ft9,116(a0)
2005c10c:	07e52c27          	fsw	ft10,120(a0)
2005c110:	07f52e27          	fsw	ft11,124(a0)
2005c114:	003022f3          	csrrs	t0,fcsr,zero
2005c118:	0055a023          	sw	t0,0(a1)
2005c11c:	8082                	c.jr	ra

2005c11e <vPortRestoreRegs>:
2005c11e:	00852103          	lw	sp,8(a0)
2005c122:	00c52183          	lw	gp,12(a0)
2005c126:	01052203          	lw	tp,16(a0)
2005c12a:	01452283          	lw	t0,20(a0)
2005c12e:	01452283          	lw	t0,20(a0)
2005c132:	01852303          	lw	t1,24(a0)
2005c136:	01c52383          	lw	t2,28(a0)
2005c13a:	5100                	c.lw	s0,32(a0)
2005c13c:	5144                	c.lw	s1,36(a0)
2005c13e:	5508                	c.lw	a0,40(a0)
2005c140:	554c                	c.lw	a1,44(a0)
2005c142:	5910                	c.lw	a2,48(a0)
2005c144:	5954                	c.lw	a3,52(a0)
2005c146:	5d18                	c.lw	a4,56(a0)
2005c148:	5d5c                	c.lw	a5,60(a0)
2005c14a:	04052803          	lw	a6,64(a0)
2005c14e:	04452883          	lw	a7,68(a0)
2005c152:	04852903          	lw	s2,72(a0)
2005c156:	04c52983          	lw	s3,76(a0)
2005c15a:	05052a03          	lw	s4,80(a0)
2005c15e:	05452a83          	lw	s5,84(a0)
2005c162:	05852b03          	lw	s6,88(a0)
2005c166:	05c52b83          	lw	s7,92(a0)
2005c16a:	06052c03          	lw	s8,96(a0)
2005c16e:	06452c83          	lw	s9,100(a0)
2005c172:	06852d03          	lw	s10,104(a0)
2005c176:	06c52d83          	lw	s11,108(a0)
2005c17a:	07052e03          	lw	t3,112(a0)
2005c17e:	07452e83          	lw	t4,116(a0)
2005c182:	07852f03          	lw	t5,120(a0)
2005c186:	07c52f83          	lw	t6,124(a0)
2005c18a:	0005a083          	lw	ra,0(a1)
2005c18e:	30009073          	csrrw	zero,mstatus,ra
2005c192:	0045a083          	lw	ra,4(a1)
2005c196:	30409073          	csrrw	zero,mie,ra
2005c19a:	0085a083          	lw	ra,8(a1)
2005c19e:	30509073          	csrrw	zero,mtvec,ra
2005c1a2:	00c5a083          	lw	ra,12(a1)
2005c1a6:	8082                	c.jr	ra

2005c1a8 <vPortRestorefloatRegs>:
2005c1a8:	00062283          	lw	t0,0(a2)
2005c1ac:	6319                	c.lui	t1,0x6
2005c1ae:	0062f2b3          	and	t0,t0,t1
2005c1b2:	06629e63          	bne	t0,t1,2005c22e <vPortRestorefloatRegs+0x86>
2005c1b6:	00052007          	flw	ft0,0(a0)
2005c1ba:	00452087          	flw	ft1,4(a0)
2005c1be:	00852107          	flw	ft2,8(a0)
2005c1c2:	00c52187          	flw	ft3,12(a0)
2005c1c6:	01052207          	flw	ft4,16(a0)
2005c1ca:	01452287          	flw	ft5,20(a0)
2005c1ce:	01852307          	flw	ft6,24(a0)
2005c1d2:	01c52387          	flw	ft7,28(a0)
2005c1d6:	7100                	c.flw	fs0,32(a0)
2005c1d8:	7144                	c.flw	fs1,36(a0)
2005c1da:	7508                	c.flw	fa0,40(a0)
2005c1dc:	754c                	c.flw	fa1,44(a0)
2005c1de:	7910                	c.flw	fa2,48(a0)
2005c1e0:	7954                	c.flw	fa3,52(a0)
2005c1e2:	7d18                	c.flw	fa4,56(a0)
2005c1e4:	7d5c                	c.flw	fa5,60(a0)
2005c1e6:	04052807          	flw	fa6,64(a0)
2005c1ea:	04452887          	flw	fa7,68(a0)
2005c1ee:	04852907          	flw	fs2,72(a0)
2005c1f2:	04c52987          	flw	fs3,76(a0)
2005c1f6:	05052a07          	flw	fs4,80(a0)
2005c1fa:	05452a87          	flw	fs5,84(a0)
2005c1fe:	05852b07          	flw	fs6,88(a0)
2005c202:	05c52b87          	flw	fs7,92(a0)
2005c206:	06052c07          	flw	fs8,96(a0)
2005c20a:	06452c87          	flw	fs9,100(a0)
2005c20e:	06852d07          	flw	fs10,104(a0)
2005c212:	06c52d87          	flw	fs11,108(a0)
2005c216:	07052e07          	flw	ft8,112(a0)
2005c21a:	07452e87          	flw	ft9,116(a0)
2005c21e:	07852f07          	flw	ft10,120(a0)
2005c222:	07c52f87          	flw	ft11,124(a0)
2005c226:	0005a283          	lw	t0,0(a1)
2005c22a:	00329073          	csrrw	zero,fcsr,t0
2005c22e:	8082                	c.jr	ra

2005c230 <vPortBackupPmp>:
2005c230:	3b0022f3          	csrrs	t0,pmpaddr0,zero
2005c234:	00552023          	sw	t0,0(a0)
2005c238:	3b1022f3          	csrrs	t0,pmpaddr1,zero
2005c23c:	00552223          	sw	t0,4(a0)
2005c240:	3b2022f3          	csrrs	t0,pmpaddr2,zero
2005c244:	00552423          	sw	t0,8(a0)
2005c248:	3b3022f3          	csrrs	t0,pmpaddr3,zero
2005c24c:	00552623          	sw	t0,12(a0)
2005c250:	3b4022f3          	csrrs	t0,pmpaddr4,zero
2005c254:	00552823          	sw	t0,16(a0)
2005c258:	3b5022f3          	csrrs	t0,pmpaddr5,zero
2005c25c:	00552a23          	sw	t0,20(a0)
2005c260:	3b6022f3          	csrrs	t0,pmpaddr6,zero
2005c264:	00552c23          	sw	t0,24(a0)
2005c268:	3b7022f3          	csrrs	t0,pmpaddr7,zero
2005c26c:	00552e23          	sw	t0,28(a0)
2005c270:	3b8022f3          	csrrs	t0,pmpaddr8,zero
2005c274:	02552023          	sw	t0,32(a0)
2005c278:	3b9022f3          	csrrs	t0,pmpaddr9,zero
2005c27c:	02552223          	sw	t0,36(a0)
2005c280:	3ba022f3          	csrrs	t0,pmpaddr10,zero
2005c284:	02552423          	sw	t0,40(a0)
2005c288:	3bb022f3          	csrrs	t0,pmpaddr11,zero
2005c28c:	02552623          	sw	t0,44(a0)
2005c290:	3bc022f3          	csrrs	t0,pmpaddr12,zero
2005c294:	02552823          	sw	t0,48(a0)
2005c298:	3bd022f3          	csrrs	t0,pmpaddr13,zero
2005c29c:	02552a23          	sw	t0,52(a0)
2005c2a0:	3be022f3          	csrrs	t0,pmpaddr14,zero
2005c2a4:	02552c23          	sw	t0,56(a0)
2005c2a8:	3bf022f3          	csrrs	t0,pmpaddr15,zero
2005c2ac:	02552e23          	sw	t0,60(a0)
2005c2b0:	3a0022f3          	csrrs	t0,pmpcfg0,zero
2005c2b4:	04552023          	sw	t0,64(a0)
2005c2b8:	3a2022f3          	csrrs	t0,pmpcfg2,zero
2005c2bc:	04552223          	sw	t0,68(a0)
2005c2c0:	3a1022f3          	csrrs	t0,pmpcfg1,zero
2005c2c4:	04552423          	sw	t0,72(a0)
2005c2c8:	3a3022f3          	csrrs	t0,pmpcfg3,zero
2005c2cc:	04552623          	sw	t0,76(a0)
2005c2d0:	8082                	c.jr	ra

2005c2d2 <vPortRestorePmp>:
2005c2d2:	00052283          	lw	t0,0(a0)
2005c2d6:	3b029073          	csrrw	zero,pmpaddr0,t0
2005c2da:	00452283          	lw	t0,4(a0)
2005c2de:	3b129073          	csrrw	zero,pmpaddr1,t0
2005c2e2:	00852283          	lw	t0,8(a0)
2005c2e6:	3b229073          	csrrw	zero,pmpaddr2,t0
2005c2ea:	00c52283          	lw	t0,12(a0)
2005c2ee:	3b329073          	csrrw	zero,pmpaddr3,t0
2005c2f2:	01052283          	lw	t0,16(a0)
2005c2f6:	3b429073          	csrrw	zero,pmpaddr4,t0
2005c2fa:	01452283          	lw	t0,20(a0)
2005c2fe:	3b529073          	csrrw	zero,pmpaddr5,t0
2005c302:	01852283          	lw	t0,24(a0)
2005c306:	3b629073          	csrrw	zero,pmpaddr6,t0
2005c30a:	01c52283          	lw	t0,28(a0)
2005c30e:	3b729073          	csrrw	zero,pmpaddr7,t0
2005c312:	02052283          	lw	t0,32(a0)
2005c316:	3b829073          	csrrw	zero,pmpaddr8,t0
2005c31a:	02452283          	lw	t0,36(a0)
2005c31e:	3b929073          	csrrw	zero,pmpaddr9,t0
2005c322:	02852283          	lw	t0,40(a0)
2005c326:	3ba29073          	csrrw	zero,pmpaddr10,t0
2005c32a:	02c52283          	lw	t0,44(a0)
2005c32e:	3bb29073          	csrrw	zero,pmpaddr11,t0
2005c332:	03052283          	lw	t0,48(a0)
2005c336:	3bc29073          	csrrw	zero,pmpaddr12,t0
2005c33a:	03452283          	lw	t0,52(a0)
2005c33e:	3bd29073          	csrrw	zero,pmpaddr13,t0
2005c342:	03852283          	lw	t0,56(a0)
2005c346:	3be29073          	csrrw	zero,pmpaddr14,t0
2005c34a:	03c52283          	lw	t0,60(a0)
2005c34e:	3bf29073          	csrrw	zero,pmpaddr15,t0
2005c352:	04052283          	lw	t0,64(a0)
2005c356:	3a029073          	csrrw	zero,pmpcfg0,t0
2005c35a:	04452283          	lw	t0,68(a0)
2005c35e:	3a229073          	csrrw	zero,pmpcfg2,t0
2005c362:	04852283          	lw	t0,72(a0)
2005c366:	3a129073          	csrrw	zero,pmpcfg1,t0
2005c36a:	04c52283          	lw	t0,76(a0)
2005c36e:	3a329073          	csrrw	zero,pmpcfg3,t0
2005c372:	8082                	c.jr	ra
2005c374:	0000                	c.unimp
2005c376:	0000                	c.unimp
	...

2005c37a <vPortSetupTimerInterrupt>:
2005c37a:	e741a783          	lw	a5,-396(gp) # 200660f0 <SystemCoreClock>
2005c37e:	3e800713          	addi	a4,zero,1000
2005c382:	02e7d7b3          	divu	a5,a5,a4
2005c386:	1141                	c.addi	sp,-16
2005c388:	f4f1aa23          	sw	a5,-172(gp) # 200661d0 <uxTimerIncrementsForOneTick>
2005c38c:	f14027f3          	csrrs	a5,mhartid,zero
2005c390:	c63e                	c.swsp	a5,12(sp)
2005c392:	4632                	c.lwsp	a2,12(sp)
2005c394:	10000737          	lui	a4,0x10000
2005c398:	00170793          	addi	a5,a4,1 # 10000001 <__km4_boot_text_start__+0x7fffe1>
2005c39c:	963e                	c.add	a2,a5
2005c39e:	060e                	c.slli	a2,0x3
2005c3a0:	f4c1a223          	sw	a2,-188(gp) # 200661c0 <pullMachineTimerCompareRegister>
2005c3a4:	47b2                	c.lwsp	a5,12(sp)
2005c3a6:	f5418593          	addi	a1,gp,-172 # 200661d0 <uxTimerIncrementsForOneTick>
2005c3aa:	97ba                	c.add	a5,a4
2005c3ac:	078e                	c.slli	a5,0x3
2005c3ae:	f4f1a423          	sw	a5,-184(gp) # 200661c4 <pullMachineTimerCounterRegister>
2005c3b2:	800007b7          	lui	a5,0x80000
2005c3b6:	43c8                	c.lw	a0,4(a5)
2005c3b8:	4394                	c.lw	a3,0(a5)
2005c3ba:	43d8                	c.lw	a4,4(a5)
2005c3bc:	fea71de3          	bne	a4,a0,2005c3b6 <vPortSetupTimerInterrupt+0x3c>
2005c3c0:	4188                	c.lw	a0,0(a1)
2005c3c2:	9536                	c.add	a0,a3
2005c3c4:	c208                	c.sw	a0,0(a2)
2005c3c6:	419c                	c.lw	a5,0(a1)
2005c3c8:	00d536b3          	sltu	a3,a0,a3
2005c3cc:	96ba                	c.add	a3,a4
2005c3ce:	00a78733          	add	a4,a5,a0
2005c3d2:	c254                	c.sw	a3,4(a2)
2005c3d4:	00f737b3          	sltu	a5,a4,a5
2005c3d8:	f4c18613          	addi	a2,gp,-180 # 200661c8 <ullNextTime>
2005c3dc:	96be                	c.add	a3,a5
2005c3de:	c218                	c.sw	a4,0(a2)
2005c3e0:	c254                	c.sw	a3,4(a2)
2005c3e2:	0141                	c.addi	sp,16
2005c3e4:	8082                	c.jr	ra

2005c3e6 <xPortStartScheduler>:
2005c3e6:	1141                	c.addi	sp,-16
2005c3e8:	c606                	c.swsp	ra,12(sp)
2005c3ea:	f91ff0ef          	jal	ra,2005c37a <vPortSetupTimerInterrupt>
2005c3ee:	6785                	c.lui	a5,0x1
2005c3f0:	88078793          	addi	a5,a5,-1920 # 880 <__NVIC_GetVectorArg+0xce>
2005c3f4:	3047a073          	csrrs	zero,mie,a5
2005c3f8:	b11ff0ef          	jal	ra,2005bf08 <xPortStartFirstTask>
2005c3fc:	40b2                	c.lwsp	ra,12(sp)
2005c3fe:	4501                	c.li	a0,0
2005c400:	0141                	c.addi	sp,16
2005c402:	8082                	c.jr	ra

2005c404 <vApplicationTickHook>:
2005c404:	8082                	c.jr	ra

2005c406 <vApplicationIdleHook>:
2005c406:	1101                	c.addi	sp,-32
2005c408:	ce06                	c.swsp	ra,28(sp)
2005c40a:	e7efe0ef          	jal	ra,2005aa88 <xPortGetFreeHeapSize>
2005c40e:	40f2                	c.lwsp	ra,28(sp)
2005c410:	c62a                	c.swsp	a0,12(sp)
2005c412:	47b2                	c.lwsp	a5,12(sp)
2005c414:	6105                	c.addi16sp	sp,32
2005c416:	8082                	c.jr	ra

2005c418 <vApplicationMallocFailedHook>:
2005c418:	1101                	c.addi	sp,-32
2005c41a:	ce06                	c.swsp	ra,28(sp)
2005c41c:	406010ef          	jal	ra,2005d822 <xTaskGetSchedulerState>
2005c420:	4785                	c.li	a5,1
2005c422:	02f50863          	beq	a0,a5,2005c452 <vApplicationMallocFailedHook+0x3a>
2005c426:	4501                	c.li	a0,0
2005c428:	0ce010ef          	jal	ra,2005d4f6 <pcTaskGetName>
2005c42c:	862a                	c.mv	a2,a0
2005c42e:	c632                	c.swsp	a2,12(sp)
2005c430:	e58fe0ef          	jal	ra,2005aa88 <xPortGetFreeHeapSize>
2005c434:	4632                	c.lwsp	a2,12(sp)
2005c436:	86aa                	c.mv	a3,a0
2005c438:	200635b7          	lui	a1,0x20063
2005c43c:	20063537          	lui	a0,0x20063
2005c440:	e6858593          	addi	a1,a1,-408 # 20062e68 <__FUNCTION__.0+0x1c>
2005c444:	e6c50513          	addi	a0,a0,-404 # 20062e6c <__FUNCTION__.0+0x20>
2005c448:	4b7020ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005c44c:	30047073          	csrrci	zero,mstatus,8
2005c450:	a001                	c.j	2005c450 <vApplicationMallocFailedHook+0x38>
2005c452:	20063637          	lui	a2,0x20063
2005c456:	e6060613          	addi	a2,a2,-416 # 20062e60 <__FUNCTION__.0+0x14>
2005c45a:	bfd1                	c.j	2005c42e <vApplicationMallocFailedHook+0x16>

2005c45c <vApplicationStackOverflowHook>:
2005c45c:	862e                	c.mv	a2,a1
2005c45e:	20063537          	lui	a0,0x20063
2005c462:	200635b7          	lui	a1,0x20063
2005c466:	1141                	c.addi	sp,-16
2005c468:	ed058593          	addi	a1,a1,-304 # 20062ed0 <__FUNCTION__.5>
2005c46c:	ea850513          	addi	a0,a0,-344 # 20062ea8 <__FUNCTION__.0+0x5c>
2005c470:	c606                	c.swsp	ra,12(sp)
2005c472:	48d020ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005c476:	a001                	c.j	2005c476 <vApplicationStackOverflowHook+0x1a>

2005c478 <vApplicationGetIdleTaskMemory>:
2005c478:	200687b7          	lui	a5,0x20068
2005c47c:	82078793          	addi	a5,a5,-2016 # 20067820 <xIdleTaskTCB.4>
2005c480:	c11c                	c.sw	a5,0(a0)
2005c482:	200647b7          	lui	a5,0x20064
2005c486:	6c078793          	addi	a5,a5,1728 # 200646c0 <__fini_array_end>
2005c48a:	c19c                	c.sw	a5,0(a1)
2005c48c:	20000793          	addi	a5,zero,512
2005c490:	c21c                	c.sw	a5,0(a2)
2005c492:	8082                	c.jr	ra

2005c494 <vApplicationGetTimerTaskMemory>:
2005c494:	200687b7          	lui	a5,0x20068
2005c498:	98478793          	addi	a5,a5,-1660 # 20067984 <xTimerTaskTCB.2>
2005c49c:	c11c                	c.sw	a5,0(a0)
2005c49e:	200657b7          	lui	a5,0x20065
2005c4a2:	ec078793          	addi	a5,a5,-320 # 20064ec0 <uxTimerTaskStack.1>
2005c4a6:	c19c                	c.sw	a5,0(a1)
2005c4a8:	20000793          	addi	a5,zero,512
2005c4ac:	c21c                	c.sw	a5,0(a2)
2005c4ae:	8082                	c.jr	ra

2005c4b0 <vPortSuppressTicksAndSleep>:
2005c4b0:	1141                	c.addi	sp,-16
2005c4b2:	c422                	c.swsp	s0,8(sp)
2005c4b4:	4501                	c.li	a0,0
2005c4b6:	c606                	c.swsp	ra,12(sp)
2005c4b8:	e601ae23          	sw	zero,-388(gp) # 200660f8 <system_can_yield>
2005c4bc:	deefc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c4c0:	300477f3          	csrrci	a5,mstatus,8
2005c4c4:	328010ef          	jal	ra,2005d7ec <eTaskConfirmSleepModeStatus>
2005c4c8:	c90d                	c.beqz	a0,2005c4fa <vPortSuppressTicksAndSleep+0x4a>
2005c4ca:	ac6fd0ef          	jal	ra,20059790 <pmu_ready_to_sleep>
2005c4ce:	e515                	c.bnez	a0,2005c4fa <vPortSuppressTicksAndSleep+0x4a>
2005c4d0:	4505                	c.li	a0,1
2005c4d2:	dd8fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c4d6:	0ff0000f          	fence	iorw,iorw
2005c4da:	10500073          	wfi
2005c4de:	0001                	c.addi	zero,0
2005c4e0:	0ff0000f          	fence	iorw,iorw
2005c4e4:	0001                	c.addi	zero,0
2005c4e6:	0001                	c.addi	zero,0
2005c4e8:	0001                	c.addi	zero,0
2005c4ea:	0001                	c.addi	zero,0
2005c4ec:	0001                	c.addi	zero,0
2005c4ee:	0001                	c.addi	zero,0
2005c4f0:	0001                	c.addi	zero,0
2005c4f2:	0001                	c.addi	zero,0
2005c4f4:	0001                	c.addi	zero,0
2005c4f6:	0001                	c.addi	zero,0
2005c4f8:	0001                	c.addi	zero,0
2005c4fa:	300467f3          	csrrsi	a5,mstatus,8
2005c4fe:	4505                	c.li	a0,1
2005c500:	daafc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c504:	4785                	c.li	a5,1
2005c506:	40b2                	c.lwsp	ra,12(sp)
2005c508:	e6f1ae23          	sw	a5,-388(gp) # 200660f8 <system_can_yield>
2005c50c:	4422                	c.lwsp	s0,8(sp)
2005c50e:	0141                	c.addi	sp,16
2005c510:	8082                	c.jr	ra

2005c512 <prvIsQueueEmpty>:
2005c512:	1141                	c.addi	sp,-16
2005c514:	c422                	c.swsp	s0,8(sp)
2005c516:	842a                	c.mv	s0,a0
2005c518:	c606                	c.swsp	ra,12(sp)
2005c51a:	4ea010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c51e:	5c00                	c.lw	s0,56(s0)
2005c520:	4fe010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c524:	40b2                	c.lwsp	ra,12(sp)
2005c526:	00143513          	sltiu	a0,s0,1
2005c52a:	4422                	c.lwsp	s0,8(sp)
2005c52c:	0141                	c.addi	sp,16
2005c52e:	8082                	c.jr	ra

2005c530 <prvCopyDataToQueue>:
2005c530:	1141                	c.addi	sp,-16
2005c532:	c422                	c.swsp	s0,8(sp)
2005c534:	c226                	c.swsp	s1,4(sp)
2005c536:	c606                	c.swsp	ra,12(sp)
2005c538:	c04a                	c.swsp	s2,0(sp)
2005c53a:	84b2                	c.mv	s1,a2
2005c53c:	4130                	c.lw	a2,64(a0)
2005c53e:	03852903          	lw	s2,56(a0)
2005c542:	842a                	c.mv	s0,a0
2005c544:	e605                	c.bnez	a2,2005c56c <prvCopyDataToQueue+0x3c>
2005c546:	411c                	c.lw	a5,0(a0)
2005c548:	4481                	c.li	s1,0
2005c54a:	e799                	c.bnez	a5,2005c558 <prvCopyDataToQueue+0x28>
2005c54c:	4508                	c.lw	a0,8(a0)
2005c54e:	39c010ef          	jal	ra,2005d8ea <xTaskPriorityDisinherit>
2005c552:	84aa                	c.mv	s1,a0
2005c554:	00042423          	sw	zero,8(s0)
2005c558:	0905                	c.addi	s2,1
2005c55a:	03242c23          	sw	s2,56(s0)
2005c55e:	40b2                	c.lwsp	ra,12(sp)
2005c560:	4422                	c.lwsp	s0,8(sp)
2005c562:	4902                	c.lwsp	s2,0(sp)
2005c564:	8526                	c.mv	a0,s1
2005c566:	4492                	c.lwsp	s1,4(sp)
2005c568:	0141                	c.addi	sp,16
2005c56a:	8082                	c.jr	ra
2005c56c:	e085                	c.bnez	s1,2005c58c <prvCopyDataToQueue+0x5c>
2005c56e:	4148                	c.lw	a0,4(a0)
2005c570:	dffad097          	auipc	ra,0xdffad
2005c574:	560080e7          	jalr	ra,1376(ra) # 9ad0 <__wrap_memcpy>
2005c578:	405c                	c.lw	a5,4(s0)
2005c57a:	4038                	c.lw	a4,64(s0)
2005c57c:	97ba                	c.add	a5,a4
2005c57e:	4418                	c.lw	a4,8(s0)
2005c580:	c05c                	c.sw	a5,4(s0)
2005c582:	fce7ebe3          	bltu	a5,a4,2005c558 <prvCopyDataToQueue+0x28>
2005c586:	401c                	c.lw	a5,0(s0)
2005c588:	c05c                	c.sw	a5,4(s0)
2005c58a:	b7f9                	c.j	2005c558 <prvCopyDataToQueue+0x28>
2005c58c:	4548                	c.lw	a0,12(a0)
2005c58e:	dffad097          	auipc	ra,0xdffad
2005c592:	542080e7          	jalr	ra,1346(ra) # 9ad0 <__wrap_memcpy>
2005c596:	4038                	c.lw	a4,64(s0)
2005c598:	445c                	c.lw	a5,12(s0)
2005c59a:	40e006b3          	sub	a3,zero,a4
2005c59e:	8f99                	c.sub	a5,a4
2005c5a0:	4018                	c.lw	a4,0(s0)
2005c5a2:	c45c                	c.sw	a5,12(s0)
2005c5a4:	00e7f563          	bgeu	a5,a4,2005c5ae <prvCopyDataToQueue+0x7e>
2005c5a8:	441c                	c.lw	a5,8(s0)
2005c5aa:	97b6                	c.add	a5,a3
2005c5ac:	c45c                	c.sw	a5,12(s0)
2005c5ae:	4789                	c.li	a5,2
2005c5b0:	00f49763          	bne	s1,a5,2005c5be <prvCopyDataToQueue+0x8e>
2005c5b4:	4481                	c.li	s1,0
2005c5b6:	fa0901e3          	beq	s2,zero,2005c558 <prvCopyDataToQueue+0x28>
2005c5ba:	197d                	c.addi	s2,-1
2005c5bc:	bf71                	c.j	2005c558 <prvCopyDataToQueue+0x28>
2005c5be:	4481                	c.li	s1,0
2005c5c0:	bf61                	c.j	2005c558 <prvCopyDataToQueue+0x28>

2005c5c2 <prvCopyDataFromQueue>:
2005c5c2:	87aa                	c.mv	a5,a0
2005c5c4:	43b0                	c.lw	a2,64(a5)
2005c5c6:	852e                	c.mv	a0,a1
2005c5c8:	ce11                	c.beqz	a2,2005c5e4 <prvCopyDataFromQueue+0x22>
2005c5ca:	47d8                	c.lw	a4,12(a5)
2005c5cc:	4794                	c.lw	a3,8(a5)
2005c5ce:	9732                	c.add	a4,a2
2005c5d0:	c7d8                	c.sw	a4,12(a5)
2005c5d2:	00d76463          	bltu	a4,a3,2005c5da <prvCopyDataFromQueue+0x18>
2005c5d6:	4398                	c.lw	a4,0(a5)
2005c5d8:	c7d8                	c.sw	a4,12(a5)
2005c5da:	47cc                	c.lw	a1,12(a5)
2005c5dc:	dffad317          	auipc	t1,0xdffad
2005c5e0:	4f430067          	jalr	zero,1268(t1) # 9ad0 <__wrap_memcpy>
2005c5e4:	8082                	c.jr	ra

2005c5e6 <prvUnlockQueue>:
2005c5e6:	1141                	c.addi	sp,-16
2005c5e8:	c422                	c.swsp	s0,8(sp)
2005c5ea:	842a                	c.mv	s0,a0
2005c5ec:	c226                	c.swsp	s1,4(sp)
2005c5ee:	c04a                	c.swsp	s2,0(sp)
2005c5f0:	c606                	c.swsp	ra,12(sp)
2005c5f2:	412010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c5f6:	04544483          	lbu	s1,69(s0)
2005c5fa:	02440913          	addi	s2,s0,36
2005c5fe:	04e2                	c.slli	s1,0x18
2005c600:	84e1                	c.srai	s1,0x18
2005c602:	02904b63          	blt	zero,s1,2005c638 <prvUnlockQueue+0x52>
2005c606:	57fd                	c.li	a5,-1
2005c608:	04f402a3          	sb	a5,69(s0)
2005c60c:	412010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c610:	3f4010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c614:	04444483          	lbu	s1,68(s0)
2005c618:	01040913          	addi	s2,s0,16
2005c61c:	04e2                	c.slli	s1,0x18
2005c61e:	84e1                	c.srai	s1,0x18
2005c620:	02904863          	blt	zero,s1,2005c650 <prvUnlockQueue+0x6a>
2005c624:	57fd                	c.li	a5,-1
2005c626:	04f40223          	sb	a5,68(s0)
2005c62a:	4422                	c.lwsp	s0,8(sp)
2005c62c:	40b2                	c.lwsp	ra,12(sp)
2005c62e:	4492                	c.lwsp	s1,4(sp)
2005c630:	4902                	c.lwsp	s2,0(sp)
2005c632:	0141                	c.addi	sp,16
2005c634:	3ea0106f          	jal	zero,2005da1e <vTaskExitCritical>
2005c638:	505c                	c.lw	a5,36(s0)
2005c63a:	d7f1                	c.beqz	a5,2005c606 <prvUnlockQueue+0x20>
2005c63c:	854a                	c.mv	a0,s2
2005c63e:	116010ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005c642:	c119                	c.beqz	a0,2005c648 <prvUnlockQueue+0x62>
2005c644:	1a0010ef          	jal	ra,2005d7e4 <vTaskMissedYield>
2005c648:	14fd                	c.addi	s1,-1
2005c64a:	04e2                	c.slli	s1,0x18
2005c64c:	84e1                	c.srai	s1,0x18
2005c64e:	bf55                	c.j	2005c602 <prvUnlockQueue+0x1c>
2005c650:	481c                	c.lw	a5,16(s0)
2005c652:	dbe9                	c.beqz	a5,2005c624 <prvUnlockQueue+0x3e>
2005c654:	854a                	c.mv	a0,s2
2005c656:	0fe010ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005c65a:	c119                	c.beqz	a0,2005c660 <prvUnlockQueue+0x7a>
2005c65c:	188010ef          	jal	ra,2005d7e4 <vTaskMissedYield>
2005c660:	14fd                	c.addi	s1,-1
2005c662:	04e2                	c.slli	s1,0x18
2005c664:	84e1                	c.srai	s1,0x18
2005c666:	bf6d                	c.j	2005c620 <prvUnlockQueue+0x3a>

2005c668 <xQueueGenericReset>:
2005c668:	1141                	c.addi	sp,-16
2005c66a:	c422                	c.swsp	s0,8(sp)
2005c66c:	c226                	c.swsp	s1,4(sp)
2005c66e:	842a                	c.mv	s0,a0
2005c670:	c606                	c.swsp	ra,12(sp)
2005c672:	84ae                	c.mv	s1,a1
2005c674:	390010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c678:	4034                	c.lw	a3,64(s0)
2005c67a:	5c5c                	c.lw	a5,60(s0)
2005c67c:	4018                	c.lw	a4,0(s0)
2005c67e:	02042c23          	sw	zero,56(s0)
2005c682:	02f687b3          	mul	a5,a3,a5
2005c686:	c058                	c.sw	a4,4(s0)
2005c688:	00f70633          	add	a2,a4,a5
2005c68c:	8f95                	c.sub	a5,a3
2005c68e:	97ba                	c.add	a5,a4
2005c690:	c45c                	c.sw	a5,12(s0)
2005c692:	57fd                	c.li	a5,-1
2005c694:	04f40223          	sb	a5,68(s0)
2005c698:	c410                	c.sw	a2,8(s0)
2005c69a:	04f402a3          	sb	a5,69(s0)
2005c69e:	e095                	c.bnez	s1,2005c6c2 <xQueueGenericReset+0x5a>
2005c6a0:	481c                	c.lw	a5,16(s0)
2005c6a2:	cb81                	c.beqz	a5,2005c6b2 <xQueueGenericReset+0x4a>
2005c6a4:	01040513          	addi	a0,s0,16
2005c6a8:	0ac010ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005c6ac:	c119                	c.beqz	a0,2005c6b2 <xQueueGenericReset+0x4a>
2005c6ae:	00000073          	ecall
2005c6b2:	36c010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c6b6:	40b2                	c.lwsp	ra,12(sp)
2005c6b8:	4422                	c.lwsp	s0,8(sp)
2005c6ba:	4492                	c.lwsp	s1,4(sp)
2005c6bc:	4505                	c.li	a0,1
2005c6be:	0141                	c.addi	sp,16
2005c6c0:	8082                	c.jr	ra
2005c6c2:	01040513          	addi	a0,s0,16
2005c6c6:	cc0fe0ef          	jal	ra,2005ab86 <vListInitialise>
2005c6ca:	02440513          	addi	a0,s0,36
2005c6ce:	cb8fe0ef          	jal	ra,2005ab86 <vListInitialise>
2005c6d2:	b7c5                	c.j	2005c6b2 <xQueueGenericReset+0x4a>

2005c6d4 <xQueueGetMutexHolder>:
2005c6d4:	1101                	c.addi	sp,-32
2005c6d6:	cc22                	c.swsp	s0,24(sp)
2005c6d8:	ce06                	c.swsp	ra,28(sp)
2005c6da:	842a                	c.mv	s0,a0
2005c6dc:	328010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c6e0:	401c                	c.lw	a5,0(s0)
2005c6e2:	4501                	c.li	a0,0
2005c6e4:	e391                	c.bnez	a5,2005c6e8 <xQueueGetMutexHolder+0x14>
2005c6e6:	4408                	c.lw	a0,8(s0)
2005c6e8:	c62a                	c.swsp	a0,12(sp)
2005c6ea:	334010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c6ee:	40f2                	c.lwsp	ra,28(sp)
2005c6f0:	4462                	c.lwsp	s0,24(sp)
2005c6f2:	4532                	c.lwsp	a0,12(sp)
2005c6f4:	6105                	c.addi16sp	sp,32
2005c6f6:	8082                	c.jr	ra

2005c6f8 <xQueueGenericSend>:
2005c6f8:	7139                	c.addi16sp	sp,-64
2005c6fa:	dc22                	c.swsp	s0,56(sp)
2005c6fc:	da26                	c.swsp	s1,52(sp)
2005c6fe:	d84a                	c.swsp	s2,48(sp)
2005c700:	d64e                	c.swsp	s3,44(sp)
2005c702:	d452                	c.swsp	s4,40(sp)
2005c704:	d256                	c.swsp	s5,36(sp)
2005c706:	d05a                	c.swsp	s6,32(sp)
2005c708:	de06                	c.swsp	ra,60(sp)
2005c70a:	842a                	c.mv	s0,a0
2005c70c:	892e                	c.mv	s2,a1
2005c70e:	c632                	c.swsp	a2,12(sp)
2005c710:	84b6                	c.mv	s1,a3
2005c712:	4a01                	c.li	s4,0
2005c714:	4a89                	c.li	s5,2
2005c716:	59fd                	c.li	s3,-1
2005c718:	01050b13          	addi	s6,a0,16
2005c71c:	2e8010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c720:	5c18                	c.lw	a4,56(s0)
2005c722:	5c5c                	c.lw	a5,60(s0)
2005c724:	00f76463          	bltu	a4,a5,2005c72c <xQueueGenericSend+0x34>
2005c728:	03549d63          	bne	s1,s5,2005c762 <xQueueGenericSend+0x6a>
2005c72c:	8626                	c.mv	a2,s1
2005c72e:	85ca                	c.mv	a1,s2
2005c730:	8522                	c.mv	a0,s0
2005c732:	dffff0ef          	jal	ra,2005c530 <prvCopyDataToQueue>
2005c736:	505c                	c.lw	a5,36(s0)
2005c738:	c789                	c.beqz	a5,2005c742 <xQueueGenericSend+0x4a>
2005c73a:	02440513          	addi	a0,s0,36
2005c73e:	016010ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005c742:	c119                	c.beqz	a0,2005c748 <xQueueGenericSend+0x50>
2005c744:	00000073          	ecall
2005c748:	2d6010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c74c:	4505                	c.li	a0,1
2005c74e:	50f2                	c.lwsp	ra,60(sp)
2005c750:	5462                	c.lwsp	s0,56(sp)
2005c752:	54d2                	c.lwsp	s1,52(sp)
2005c754:	5942                	c.lwsp	s2,48(sp)
2005c756:	59b2                	c.lwsp	s3,44(sp)
2005c758:	5a22                	c.lwsp	s4,40(sp)
2005c75a:	5a92                	c.lwsp	s5,36(sp)
2005c75c:	5b02                	c.lwsp	s6,32(sp)
2005c75e:	6121                	c.addi16sp	sp,64
2005c760:	8082                	c.jr	ra
2005c762:	47b2                	c.lwsp	a5,12(sp)
2005c764:	e789                	c.bnez	a5,2005c76e <xQueueGenericSend+0x76>
2005c766:	2b8010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c76a:	4501                	c.li	a0,0
2005c76c:	b7cd                	c.j	2005c74e <xQueueGenericSend+0x56>
2005c76e:	000a1563          	bne	s4,zero,2005c778 <xQueueGenericSend+0x80>
2005c772:	0828                	c.addi4spn	a0,sp,24
2005c774:	062010ef          	jal	ra,2005d7d6 <vTaskInternalSetTimeOutState>
2005c778:	2a6010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c77c:	563000ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005c780:	284010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c784:	04444783          	lbu	a5,68(s0)
2005c788:	07e2                	c.slli	a5,0x18
2005c78a:	87e1                	c.srai	a5,0x18
2005c78c:	01379463          	bne	a5,s3,2005c794 <xQueueGenericSend+0x9c>
2005c790:	04040223          	sb	zero,68(s0)
2005c794:	04544783          	lbu	a5,69(s0)
2005c798:	07e2                	c.slli	a5,0x18
2005c79a:	87e1                	c.srai	a5,0x18
2005c79c:	01379463          	bne	a5,s3,2005c7a4 <xQueueGenericSend+0xac>
2005c7a0:	040402a3          	sb	zero,69(s0)
2005c7a4:	27a010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c7a8:	006c                	c.addi4spn	a1,sp,12
2005c7aa:	0828                	c.addi4spn	a0,sp,24
2005c7ac:	18b010ef          	jal	ra,2005e136 <xTaskCheckForTimeOut>
2005c7b0:	e121                	c.bnez	a0,2005c7f0 <xQueueGenericSend+0xf8>
2005c7b2:	252010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c7b6:	5c18                	c.lw	a4,56(s0)
2005c7b8:	5c5c                	c.lw	a5,60(s0)
2005c7ba:	02f71063          	bne	a4,a5,2005c7da <xQueueGenericSend+0xe2>
2005c7be:	260010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c7c2:	45b2                	c.lwsp	a1,12(sp)
2005c7c4:	855a                	c.mv	a0,s6
2005c7c6:	745000ef          	jal	ra,2005d70a <vTaskPlaceOnEventList>
2005c7ca:	8522                	c.mv	a0,s0
2005c7cc:	e1bff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005c7d0:	5bc010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005c7d4:	c919                	c.beqz	a0,2005c7ea <xQueueGenericSend+0xf2>
2005c7d6:	4a05                	c.li	s4,1
2005c7d8:	b791                	c.j	2005c71c <xQueueGenericSend+0x24>
2005c7da:	244010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c7de:	8522                	c.mv	a0,s0
2005c7e0:	e07ff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005c7e4:	5a8010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005c7e8:	b7fd                	c.j	2005c7d6 <xQueueGenericSend+0xde>
2005c7ea:	00000073          	ecall
2005c7ee:	b7e5                	c.j	2005c7d6 <xQueueGenericSend+0xde>
2005c7f0:	8522                	c.mv	a0,s0
2005c7f2:	df5ff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005c7f6:	596010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005c7fa:	bf85                	c.j	2005c76a <xQueueGenericSend+0x72>

2005c7fc <xQueueGenericSendFromISR>:
2005c7fc:	1141                	c.addi	sp,-16
2005c7fe:	c422                	c.swsp	s0,8(sp)
2005c800:	c04a                	c.swsp	s2,0(sp)
2005c802:	c606                	c.swsp	ra,12(sp)
2005c804:	c226                	c.swsp	s1,4(sp)
2005c806:	5d18                	c.lw	a4,56(a0)
2005c808:	5d5c                	c.lw	a5,60(a0)
2005c80a:	8932                	c.mv	s2,a2
2005c80c:	842a                	c.mv	s0,a0
2005c80e:	8636                	c.mv	a2,a3
2005c810:	00f76663          	bltu	a4,a5,2005c81c <xQueueGenericSendFromISR+0x20>
2005c814:	4789                	c.li	a5,2
2005c816:	4501                	c.li	a0,0
2005c818:	02f69063          	bne	a3,a5,2005c838 <xQueueGenericSendFromISR+0x3c>
2005c81c:	04544483          	lbu	s1,69(s0)
2005c820:	5c1c                	c.lw	a5,56(s0)
2005c822:	8522                	c.mv	a0,s0
2005c824:	04e2                	c.slli	s1,0x18
2005c826:	d0bff0ef          	jal	ra,2005c530 <prvCopyDataToQueue>
2005c82a:	84e1                	c.srai	s1,0x18
2005c82c:	57fd                	c.li	a5,-1
2005c82e:	02f49663          	bne	s1,a5,2005c85a <xQueueGenericSendFromISR+0x5e>
2005c832:	505c                	c.lw	a5,36(s0)
2005c834:	eb81                	c.bnez	a5,2005c844 <xQueueGenericSendFromISR+0x48>
2005c836:	4505                	c.li	a0,1
2005c838:	40b2                	c.lwsp	ra,12(sp)
2005c83a:	4422                	c.lwsp	s0,8(sp)
2005c83c:	4492                	c.lwsp	s1,4(sp)
2005c83e:	4902                	c.lwsp	s2,0(sp)
2005c840:	0141                	c.addi	sp,16
2005c842:	8082                	c.jr	ra
2005c844:	02440513          	addi	a0,s0,36
2005c848:	70d000ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005c84c:	d56d                	c.beqz	a0,2005c836 <xQueueGenericSendFromISR+0x3a>
2005c84e:	fe0904e3          	beq	s2,zero,2005c836 <xQueueGenericSendFromISR+0x3a>
2005c852:	4785                	c.li	a5,1
2005c854:	00f92023          	sw	a5,0(s2)
2005c858:	bff9                	c.j	2005c836 <xQueueGenericSendFromISR+0x3a>
2005c85a:	0485                	c.addi	s1,1
2005c85c:	04e2                	c.slli	s1,0x18
2005c85e:	84e1                	c.srai	s1,0x18
2005c860:	049402a3          	sb	s1,69(s0)
2005c864:	bfc9                	c.j	2005c836 <xQueueGenericSendFromISR+0x3a>

2005c866 <xQueueGiveFromISR>:
2005c866:	5d18                	c.lw	a4,56(a0)
2005c868:	5d5c                	c.lw	a5,60(a0)
2005c86a:	04f77663          	bgeu	a4,a5,2005c8b6 <xQueueGiveFromISR+0x50>
2005c86e:	04554783          	lbu	a5,69(a0)
2005c872:	0705                	c.addi	a4,1
2005c874:	dd18                	c.sw	a4,56(a0)
2005c876:	07e2                	c.slli	a5,0x18
2005c878:	87e1                	c.srai	a5,0x18
2005c87a:	577d                	c.li	a4,-1
2005c87c:	02e79763          	bne	a5,a4,2005c8aa <xQueueGiveFromISR+0x44>
2005c880:	515c                	c.lw	a5,36(a0)
2005c882:	e399                	c.bnez	a5,2005c888 <xQueueGiveFromISR+0x22>
2005c884:	4505                	c.li	a0,1
2005c886:	8082                	c.jr	ra
2005c888:	1141                	c.addi	sp,-16
2005c88a:	02450513          	addi	a0,a0,36
2005c88e:	c422                	c.swsp	s0,8(sp)
2005c890:	c606                	c.swsp	ra,12(sp)
2005c892:	842e                	c.mv	s0,a1
2005c894:	6c1000ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005c898:	c501                	c.beqz	a0,2005c8a0 <xQueueGiveFromISR+0x3a>
2005c89a:	c019                	c.beqz	s0,2005c8a0 <xQueueGiveFromISR+0x3a>
2005c89c:	4785                	c.li	a5,1
2005c89e:	c01c                	c.sw	a5,0(s0)
2005c8a0:	40b2                	c.lwsp	ra,12(sp)
2005c8a2:	4422                	c.lwsp	s0,8(sp)
2005c8a4:	4505                	c.li	a0,1
2005c8a6:	0141                	c.addi	sp,16
2005c8a8:	8082                	c.jr	ra
2005c8aa:	0785                	c.addi	a5,1
2005c8ac:	07e2                	c.slli	a5,0x18
2005c8ae:	87e1                	c.srai	a5,0x18
2005c8b0:	04f502a3          	sb	a5,69(a0)
2005c8b4:	bfc1                	c.j	2005c884 <xQueueGiveFromISR+0x1e>
2005c8b6:	4501                	c.li	a0,0
2005c8b8:	8082                	c.jr	ra

2005c8ba <xQueueReceive>:
2005c8ba:	7139                	c.addi16sp	sp,-64
2005c8bc:	dc22                	c.swsp	s0,56(sp)
2005c8be:	d84a                	c.swsp	s2,48(sp)
2005c8c0:	d64e                	c.swsp	s3,44(sp)
2005c8c2:	d452                	c.swsp	s4,40(sp)
2005c8c4:	d256                	c.swsp	s5,36(sp)
2005c8c6:	de06                	c.swsp	ra,60(sp)
2005c8c8:	da26                	c.swsp	s1,52(sp)
2005c8ca:	842a                	c.mv	s0,a0
2005c8cc:	892e                	c.mv	s2,a1
2005c8ce:	c632                	c.swsp	a2,12(sp)
2005c8d0:	4a01                	c.li	s4,0
2005c8d2:	59fd                	c.li	s3,-1
2005c8d4:	02450a93          	addi	s5,a0,36
2005c8d8:	12c010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c8dc:	5c04                	c.lw	s1,56(s0)
2005c8de:	cc85                	c.beqz	s1,2005c916 <xQueueReceive+0x5c>
2005c8e0:	85ca                	c.mv	a1,s2
2005c8e2:	8522                	c.mv	a0,s0
2005c8e4:	14fd                	c.addi	s1,-1
2005c8e6:	cddff0ef          	jal	ra,2005c5c2 <prvCopyDataFromQueue>
2005c8ea:	dc04                	c.sw	s1,56(s0)
2005c8ec:	481c                	c.lw	a5,16(s0)
2005c8ee:	cb81                	c.beqz	a5,2005c8fe <xQueueReceive+0x44>
2005c8f0:	01040513          	addi	a0,s0,16
2005c8f4:	661000ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005c8f8:	c119                	c.beqz	a0,2005c8fe <xQueueReceive+0x44>
2005c8fa:	00000073          	ecall
2005c8fe:	120010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c902:	4505                	c.li	a0,1
2005c904:	50f2                	c.lwsp	ra,60(sp)
2005c906:	5462                	c.lwsp	s0,56(sp)
2005c908:	54d2                	c.lwsp	s1,52(sp)
2005c90a:	5942                	c.lwsp	s2,48(sp)
2005c90c:	59b2                	c.lwsp	s3,44(sp)
2005c90e:	5a22                	c.lwsp	s4,40(sp)
2005c910:	5a92                	c.lwsp	s5,36(sp)
2005c912:	6121                	c.addi16sp	sp,64
2005c914:	8082                	c.jr	ra
2005c916:	47b2                	c.lwsp	a5,12(sp)
2005c918:	e789                	c.bnez	a5,2005c922 <xQueueReceive+0x68>
2005c91a:	104010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c91e:	4501                	c.li	a0,0
2005c920:	b7d5                	c.j	2005c904 <xQueueReceive+0x4a>
2005c922:	000a1563          	bne	s4,zero,2005c92c <xQueueReceive+0x72>
2005c926:	0828                	c.addi4spn	a0,sp,24
2005c928:	6af000ef          	jal	ra,2005d7d6 <vTaskInternalSetTimeOutState>
2005c92c:	0f2010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c930:	3af000ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005c934:	0d0010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c938:	04444783          	lbu	a5,68(s0)
2005c93c:	07e2                	c.slli	a5,0x18
2005c93e:	87e1                	c.srai	a5,0x18
2005c940:	01379463          	bne	a5,s3,2005c948 <xQueueReceive+0x8e>
2005c944:	04040223          	sb	zero,68(s0)
2005c948:	04544783          	lbu	a5,69(s0)
2005c94c:	07e2                	c.slli	a5,0x18
2005c94e:	87e1                	c.srai	a5,0x18
2005c950:	01379463          	bne	a5,s3,2005c958 <xQueueReceive+0x9e>
2005c954:	040402a3          	sb	zero,69(s0)
2005c958:	0c6010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c95c:	006c                	c.addi4spn	a1,sp,12
2005c95e:	0828                	c.addi4spn	a0,sp,24
2005c960:	7d6010ef          	jal	ra,2005e136 <xTaskCheckForTimeOut>
2005c964:	e90d                	c.bnez	a0,2005c996 <xQueueReceive+0xdc>
2005c966:	8522                	c.mv	a0,s0
2005c968:	babff0ef          	jal	ra,2005c512 <prvIsQueueEmpty>
2005c96c:	cd19                	c.beqz	a0,2005c98a <xQueueReceive+0xd0>
2005c96e:	45b2                	c.lwsp	a1,12(sp)
2005c970:	8556                	c.mv	a0,s5
2005c972:	599000ef          	jal	ra,2005d70a <vTaskPlaceOnEventList>
2005c976:	8522                	c.mv	a0,s0
2005c978:	c6fff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005c97c:	410010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005c980:	e119                	c.bnez	a0,2005c986 <xQueueReceive+0xcc>
2005c982:	00000073          	ecall
2005c986:	4a05                	c.li	s4,1
2005c988:	bf81                	c.j	2005c8d8 <xQueueReceive+0x1e>
2005c98a:	8522                	c.mv	a0,s0
2005c98c:	c5bff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005c990:	3fc010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005c994:	bfcd                	c.j	2005c986 <xQueueReceive+0xcc>
2005c996:	8522                	c.mv	a0,s0
2005c998:	c4fff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005c99c:	3f0010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005c9a0:	8522                	c.mv	a0,s0
2005c9a2:	b71ff0ef          	jal	ra,2005c512 <prvIsQueueEmpty>
2005c9a6:	d165                	c.beqz	a0,2005c986 <xQueueReceive+0xcc>
2005c9a8:	bf9d                	c.j	2005c91e <xQueueReceive+0x64>

2005c9aa <xQueueSemaphoreTake>:
2005c9aa:	7139                	c.addi16sp	sp,-64
2005c9ac:	dc22                	c.swsp	s0,56(sp)
2005c9ae:	da26                	c.swsp	s1,52(sp)
2005c9b0:	d84a                	c.swsp	s2,48(sp)
2005c9b2:	d64e                	c.swsp	s3,44(sp)
2005c9b4:	d452                	c.swsp	s4,40(sp)
2005c9b6:	de06                	c.swsp	ra,60(sp)
2005c9b8:	842a                	c.mv	s0,a0
2005c9ba:	c62e                	c.swsp	a1,12(sp)
2005c9bc:	4481                	c.li	s1,0
2005c9be:	4981                	c.li	s3,0
2005c9c0:	597d                	c.li	s2,-1
2005c9c2:	02450a13          	addi	s4,a0,36
2005c9c6:	03e010ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005c9ca:	5c1c                	c.lw	a5,56(s0)
2005c9cc:	cf8d                	c.beqz	a5,2005ca06 <xQueueSemaphoreTake+0x5c>
2005c9ce:	17fd                	c.addi	a5,-1
2005c9d0:	dc1c                	c.sw	a5,56(s0)
2005c9d2:	401c                	c.lw	a5,0(s0)
2005c9d4:	e781                	c.bnez	a5,2005c9dc <xQueueSemaphoreTake+0x32>
2005c9d6:	083010ef          	jal	ra,2005e258 <pvTaskIncrementMutexHeldCount>
2005c9da:	c408                	c.sw	a0,8(s0)
2005c9dc:	481c                	c.lw	a5,16(s0)
2005c9de:	cb81                	c.beqz	a5,2005c9ee <xQueueSemaphoreTake+0x44>
2005c9e0:	01040513          	addi	a0,s0,16
2005c9e4:	571000ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005c9e8:	c119                	c.beqz	a0,2005c9ee <xQueueSemaphoreTake+0x44>
2005c9ea:	00000073          	ecall
2005c9ee:	030010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005c9f2:	4485                	c.li	s1,1
2005c9f4:	50f2                	c.lwsp	ra,60(sp)
2005c9f6:	5462                	c.lwsp	s0,56(sp)
2005c9f8:	5942                	c.lwsp	s2,48(sp)
2005c9fa:	59b2                	c.lwsp	s3,44(sp)
2005c9fc:	5a22                	c.lwsp	s4,40(sp)
2005c9fe:	8526                	c.mv	a0,s1
2005ca00:	54d2                	c.lwsp	s1,52(sp)
2005ca02:	6121                	c.addi16sp	sp,64
2005ca04:	8082                	c.jr	ra
2005ca06:	47b2                	c.lwsp	a5,12(sp)
2005ca08:	e789                	c.bnez	a5,2005ca12 <xQueueSemaphoreTake+0x68>
2005ca0a:	014010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005ca0e:	4481                	c.li	s1,0
2005ca10:	b7d5                	c.j	2005c9f4 <xQueueSemaphoreTake+0x4a>
2005ca12:	00099563          	bne	s3,zero,2005ca1c <xQueueSemaphoreTake+0x72>
2005ca16:	0828                	c.addi4spn	a0,sp,24
2005ca18:	5bf000ef          	jal	ra,2005d7d6 <vTaskInternalSetTimeOutState>
2005ca1c:	002010ef          	jal	ra,2005da1e <vTaskExitCritical>
2005ca20:	2bf000ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005ca24:	7e1000ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005ca28:	04444783          	lbu	a5,68(s0)
2005ca2c:	07e2                	c.slli	a5,0x18
2005ca2e:	87e1                	c.srai	a5,0x18
2005ca30:	01279463          	bne	a5,s2,2005ca38 <xQueueSemaphoreTake+0x8e>
2005ca34:	04040223          	sb	zero,68(s0)
2005ca38:	04544783          	lbu	a5,69(s0)
2005ca3c:	07e2                	c.slli	a5,0x18
2005ca3e:	87e1                	c.srai	a5,0x18
2005ca40:	01279463          	bne	a5,s2,2005ca48 <xQueueSemaphoreTake+0x9e>
2005ca44:	040402a3          	sb	zero,69(s0)
2005ca48:	7d7000ef          	jal	ra,2005da1e <vTaskExitCritical>
2005ca4c:	006c                	c.addi4spn	a1,sp,12
2005ca4e:	0828                	c.addi4spn	a0,sp,24
2005ca50:	6e6010ef          	jal	ra,2005e136 <xTaskCheckForTimeOut>
2005ca54:	e139                	c.bnez	a0,2005ca9a <xQueueSemaphoreTake+0xf0>
2005ca56:	8522                	c.mv	a0,s0
2005ca58:	abbff0ef          	jal	ra,2005c512 <prvIsQueueEmpty>
2005ca5c:	c90d                	c.beqz	a0,2005ca8e <xQueueSemaphoreTake+0xe4>
2005ca5e:	401c                	c.lw	a5,0(s0)
2005ca60:	eb89                	c.bnez	a5,2005ca72 <xQueueSemaphoreTake+0xc8>
2005ca62:	7a3000ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005ca66:	4408                	c.lw	a0,8(s0)
2005ca68:	5cf000ef          	jal	ra,2005d836 <xTaskPriorityInherit>
2005ca6c:	84aa                	c.mv	s1,a0
2005ca6e:	7b1000ef          	jal	ra,2005da1e <vTaskExitCritical>
2005ca72:	45b2                	c.lwsp	a1,12(sp)
2005ca74:	8552                	c.mv	a0,s4
2005ca76:	495000ef          	jal	ra,2005d70a <vTaskPlaceOnEventList>
2005ca7a:	8522                	c.mv	a0,s0
2005ca7c:	b6bff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005ca80:	30c010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005ca84:	e119                	c.bnez	a0,2005ca8a <xQueueSemaphoreTake+0xe0>
2005ca86:	00000073          	ecall
2005ca8a:	4985                	c.li	s3,1
2005ca8c:	bf2d                	c.j	2005c9c6 <xQueueSemaphoreTake+0x1c>
2005ca8e:	8522                	c.mv	a0,s0
2005ca90:	b57ff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005ca94:	2f8010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005ca98:	bfcd                	c.j	2005ca8a <xQueueSemaphoreTake+0xe0>
2005ca9a:	8522                	c.mv	a0,s0
2005ca9c:	b4bff0ef          	jal	ra,2005c5e6 <prvUnlockQueue>
2005caa0:	2ec010ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005caa4:	8522                	c.mv	a0,s0
2005caa6:	a6dff0ef          	jal	ra,2005c512 <prvIsQueueEmpty>
2005caaa:	d165                	c.beqz	a0,2005ca8a <xQueueSemaphoreTake+0xe0>
2005caac:	f40484e3          	beq	s1,zero,2005c9f4 <xQueueSemaphoreTake+0x4a>
2005cab0:	755000ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005cab4:	504c                	c.lw	a1,36(s0)
2005cab6:	c589                	c.beqz	a1,2005cac0 <xQueueSemaphoreTake+0x116>
2005cab8:	581c                	c.lw	a5,48(s0)
2005caba:	45ad                	c.li	a1,11
2005cabc:	439c                	c.lw	a5,0(a5)
2005cabe:	8d9d                	c.sub	a1,a5
2005cac0:	4408                	c.lw	a0,8(s0)
2005cac2:	6a5000ef          	jal	ra,2005d966 <vTaskPriorityDisinheritAfterTimeout>
2005cac6:	b791                	c.j	2005ca0a <xQueueSemaphoreTake+0x60>

2005cac8 <xQueueReceiveFromISR>:
2005cac8:	1101                	c.addi	sp,-32
2005caca:	ce06                	c.swsp	ra,28(sp)
2005cacc:	cc22                	c.swsp	s0,24(sp)
2005cace:	ca26                	c.swsp	s1,20(sp)
2005cad0:	c84a                	c.swsp	s2,16(sp)
2005cad2:	c64e                	c.swsp	s3,12(sp)
2005cad4:	03852983          	lw	s3,56(a0)
2005cad8:	04098b63          	beq	s3,zero,2005cb2e <xQueueReceiveFromISR+0x66>
2005cadc:	04454483          	lbu	s1,68(a0)
2005cae0:	842a                	c.mv	s0,a0
2005cae2:	8932                	c.mv	s2,a2
2005cae4:	04e2                	c.slli	s1,0x18
2005cae6:	addff0ef          	jal	ra,2005c5c2 <prvCopyDataFromQueue>
2005caea:	19fd                	c.addi	s3,-1
2005caec:	84e1                	c.srai	s1,0x18
2005caee:	03342c23          	sw	s3,56(s0)
2005caf2:	57fd                	c.li	a5,-1
2005caf4:	02f49763          	bne	s1,a5,2005cb22 <xQueueReceiveFromISR+0x5a>
2005caf8:	481c                	c.lw	a5,16(s0)
2005cafa:	eb89                	c.bnez	a5,2005cb0c <xQueueReceiveFromISR+0x44>
2005cafc:	4505                	c.li	a0,1
2005cafe:	40f2                	c.lwsp	ra,28(sp)
2005cb00:	4462                	c.lwsp	s0,24(sp)
2005cb02:	44d2                	c.lwsp	s1,20(sp)
2005cb04:	4942                	c.lwsp	s2,16(sp)
2005cb06:	49b2                	c.lwsp	s3,12(sp)
2005cb08:	6105                	c.addi16sp	sp,32
2005cb0a:	8082                	c.jr	ra
2005cb0c:	01040513          	addi	a0,s0,16
2005cb10:	445000ef          	jal	ra,2005d754 <xTaskRemoveFromEventList>
2005cb14:	d565                	c.beqz	a0,2005cafc <xQueueReceiveFromISR+0x34>
2005cb16:	fe0903e3          	beq	s2,zero,2005cafc <xQueueReceiveFromISR+0x34>
2005cb1a:	4785                	c.li	a5,1
2005cb1c:	00f92023          	sw	a5,0(s2)
2005cb20:	bff1                	c.j	2005cafc <xQueueReceiveFromISR+0x34>
2005cb22:	0485                	c.addi	s1,1
2005cb24:	04e2                	c.slli	s1,0x18
2005cb26:	84e1                	c.srai	s1,0x18
2005cb28:	04940223          	sb	s1,68(s0)
2005cb2c:	bfc1                	c.j	2005cafc <xQueueReceiveFromISR+0x34>
2005cb2e:	4501                	c.li	a0,0
2005cb30:	b7f9                	c.j	2005cafe <xQueueReceiveFromISR+0x36>

2005cb32 <vQueueWaitForMessageRestricted>:
2005cb32:	1141                	c.addi	sp,-16
2005cb34:	c422                	c.swsp	s0,8(sp)
2005cb36:	c226                	c.swsp	s1,4(sp)
2005cb38:	c04a                	c.swsp	s2,0(sp)
2005cb3a:	842a                	c.mv	s0,a0
2005cb3c:	c606                	c.swsp	ra,12(sp)
2005cb3e:	84ae                	c.mv	s1,a1
2005cb40:	8932                	c.mv	s2,a2
2005cb42:	6c3000ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005cb46:	04444783          	lbu	a5,68(s0)
2005cb4a:	577d                	c.li	a4,-1
2005cb4c:	07e2                	c.slli	a5,0x18
2005cb4e:	87e1                	c.srai	a5,0x18
2005cb50:	00e79463          	bne	a5,a4,2005cb58 <vQueueWaitForMessageRestricted+0x26>
2005cb54:	04040223          	sb	zero,68(s0)
2005cb58:	04544783          	lbu	a5,69(s0)
2005cb5c:	577d                	c.li	a4,-1
2005cb5e:	07e2                	c.slli	a5,0x18
2005cb60:	87e1                	c.srai	a5,0x18
2005cb62:	00e79463          	bne	a5,a4,2005cb6a <vQueueWaitForMessageRestricted+0x38>
2005cb66:	040402a3          	sb	zero,69(s0)
2005cb6a:	6b5000ef          	jal	ra,2005da1e <vTaskExitCritical>
2005cb6e:	5c1c                	c.lw	a5,56(s0)
2005cb70:	e799                	c.bnez	a5,2005cb7e <vQueueWaitForMessageRestricted+0x4c>
2005cb72:	864a                	c.mv	a2,s2
2005cb74:	85a6                	c.mv	a1,s1
2005cb76:	02440513          	addi	a0,s0,36
2005cb7a:	3b1000ef          	jal	ra,2005d72a <vTaskPlaceOnEventListRestricted>
2005cb7e:	8522                	c.mv	a0,s0
2005cb80:	4422                	c.lwsp	s0,8(sp)
2005cb82:	40b2                	c.lwsp	ra,12(sp)
2005cb84:	4492                	c.lwsp	s1,4(sp)
2005cb86:	4902                	c.lwsp	s2,0(sp)
2005cb88:	0141                	c.addi	sp,16
2005cb8a:	a5dff06f          	jal	zero,2005c5e6 <prvUnlockQueue>

2005cb8e <QueueCreateTrace>:
2005cb8e:	f581a603          	lw	a2,-168(gp) # 200661d4 <Inited.1>
2005cb92:	f5818693          	addi	a3,gp,-168 # 200661d4 <Inited.1>
2005cb96:	200687b7          	lui	a5,0x20068
2005cb9a:	ae878713          	addi	a4,a5,-1304 # 20067ae8 <QueueTraceList>
2005cb9e:	ee01                	c.bnez	a2,2005cbb6 <QueueCreateTrace+0x28>
2005cba0:	ae878793          	addi	a5,a5,-1304
2005cba4:	04070613          	addi	a2,a4,64
2005cba8:	0007a023          	sw	zero,0(a5)
2005cbac:	0791                	c.addi	a5,4
2005cbae:	fec79de3          	bne	a5,a2,2005cba8 <QueueCreateTrace+0x1a>
2005cbb2:	4785                	c.li	a5,1
2005cbb4:	c29c                	c.sw	a5,0(a3)
2005cbb6:	86ba                	c.mv	a3,a4
2005cbb8:	4781                	c.li	a5,0
2005cbba:	4641                	c.li	a2,16
2005cbbc:	428c                	c.lw	a1,0(a3)
2005cbbe:	e589                	c.bnez	a1,2005cbc8 <QueueCreateTrace+0x3a>
2005cbc0:	078a                	c.slli	a5,0x2
2005cbc2:	97ba                	c.add	a5,a4
2005cbc4:	c388                	c.sw	a0,0(a5)
2005cbc6:	8082                	c.jr	ra
2005cbc8:	0785                	c.addi	a5,1
2005cbca:	0691                	c.addi	a3,4
2005cbcc:	fec798e3          	bne	a5,a2,2005cbbc <QueueCreateTrace+0x2e>
2005cbd0:	8082                	c.jr	ra

2005cbd2 <xQueueGenericCreate>:
2005cbd2:	1101                	c.addi	sp,-32
2005cbd4:	c84a                	c.swsp	s2,16(sp)
2005cbd6:	892a                	c.mv	s2,a0
2005cbd8:	02b50533          	mul	a0,a0,a1
2005cbdc:	cc22                	c.swsp	s0,24(sp)
2005cbde:	ca26                	c.swsp	s1,20(sp)
2005cbe0:	c64e                	c.swsp	s3,12(sp)
2005cbe2:	ce06                	c.swsp	ra,28(sp)
2005cbe4:	84ae                	c.mv	s1,a1
2005cbe6:	89b2                	c.mv	s3,a2
2005cbe8:	05050513          	addi	a0,a0,80
2005cbec:	d61fd0ef          	jal	ra,2005a94c <pvPortMalloc>
2005cbf0:	842a                	c.mv	s0,a0
2005cbf2:	c505                	c.beqz	a0,2005cc1a <xQueueGenericCreate+0x48>
2005cbf4:	04050323          	sb	zero,70(a0)
2005cbf8:	87aa                	c.mv	a5,a0
2005cbfa:	c099                	c.beqz	s1,2005cc00 <xQueueGenericCreate+0x2e>
2005cbfc:	05050793          	addi	a5,a0,80
2005cc00:	c01c                	c.sw	a5,0(s0)
2005cc02:	03242e23          	sw	s2,60(s0)
2005cc06:	c024                	c.sw	s1,64(s0)
2005cc08:	4585                	c.li	a1,1
2005cc0a:	8522                	c.mv	a0,s0
2005cc0c:	a5dff0ef          	jal	ra,2005c668 <xQueueGenericReset>
2005cc10:	8522                	c.mv	a0,s0
2005cc12:	05340623          	sb	s3,76(s0)
2005cc16:	f79ff0ef          	jal	ra,2005cb8e <QueueCreateTrace>
2005cc1a:	40f2                	c.lwsp	ra,28(sp)
2005cc1c:	8522                	c.mv	a0,s0
2005cc1e:	4462                	c.lwsp	s0,24(sp)
2005cc20:	44d2                	c.lwsp	s1,20(sp)
2005cc22:	4942                	c.lwsp	s2,16(sp)
2005cc24:	49b2                	c.lwsp	s3,12(sp)
2005cc26:	6105                	c.addi16sp	sp,32
2005cc28:	8082                	c.jr	ra

2005cc2a <xQueueCreateMutex>:
2005cc2a:	1141                	c.addi	sp,-16
2005cc2c:	862a                	c.mv	a2,a0
2005cc2e:	4581                	c.li	a1,0
2005cc30:	4505                	c.li	a0,1
2005cc32:	c422                	c.swsp	s0,8(sp)
2005cc34:	c606                	c.swsp	ra,12(sp)
2005cc36:	f9dff0ef          	jal	ra,2005cbd2 <xQueueGenericCreate>
2005cc3a:	842a                	c.mv	s0,a0
2005cc3c:	cd01                	c.beqz	a0,2005cc54 <xQueueCreateMutex+0x2a>
2005cc3e:	00052423          	sw	zero,8(a0)
2005cc42:	00052023          	sw	zero,0(a0)
2005cc46:	00052623          	sw	zero,12(a0)
2005cc4a:	4681                	c.li	a3,0
2005cc4c:	4601                	c.li	a2,0
2005cc4e:	4581                	c.li	a1,0
2005cc50:	aa9ff0ef          	jal	ra,2005c6f8 <xQueueGenericSend>
2005cc54:	40b2                	c.lwsp	ra,12(sp)
2005cc56:	8522                	c.mv	a0,s0
2005cc58:	4422                	c.lwsp	s0,8(sp)
2005cc5a:	0141                	c.addi	sp,16
2005cc5c:	8082                	c.jr	ra

2005cc5e <xQueueCreateCountingSemaphore>:
2005cc5e:	1141                	c.addi	sp,-16
2005cc60:	c422                	c.swsp	s0,8(sp)
2005cc62:	4609                	c.li	a2,2
2005cc64:	842e                	c.mv	s0,a1
2005cc66:	4581                	c.li	a1,0
2005cc68:	c606                	c.swsp	ra,12(sp)
2005cc6a:	f69ff0ef          	jal	ra,2005cbd2 <xQueueGenericCreate>
2005cc6e:	c111                	c.beqz	a0,2005cc72 <xQueueCreateCountingSemaphore+0x14>
2005cc70:	dd00                	c.sw	s0,56(a0)
2005cc72:	40b2                	c.lwsp	ra,12(sp)
2005cc74:	4422                	c.lwsp	s0,8(sp)
2005cc76:	0141                	c.addi	sp,16
2005cc78:	8082                	c.jr	ra

2005cc7a <xQueueGenericCreateStatic>:
2005cc7a:	1141                	c.addi	sp,-16
2005cc7c:	c422                	c.swsp	s0,8(sp)
2005cc7e:	c606                	c.swsp	ra,12(sp)
2005cc80:	c226                	c.swsp	s1,4(sp)
2005cc82:	8436                	c.mv	s0,a3
2005cc84:	c29d                	c.beqz	a3,2005ccaa <xQueueGenericCreateStatic+0x30>
2005cc86:	4785                	c.li	a5,1
2005cc88:	04f68323          	sb	a5,70(a3) # 8000046 <__ap_sram_heap_size+0x7fc0046>
2005cc8c:	84ba                	c.mv	s1,a4
2005cc8e:	e191                	c.bnez	a1,2005cc92 <xQueueGenericCreateStatic+0x18>
2005cc90:	8636                	c.mv	a2,a3
2005cc92:	dc48                	c.sw	a0,60(s0)
2005cc94:	c02c                	c.sw	a1,64(s0)
2005cc96:	c010                	c.sw	a2,0(s0)
2005cc98:	4585                	c.li	a1,1
2005cc9a:	8522                	c.mv	a0,s0
2005cc9c:	9cdff0ef          	jal	ra,2005c668 <xQueueGenericReset>
2005cca0:	8522                	c.mv	a0,s0
2005cca2:	04940623          	sb	s1,76(s0)
2005cca6:	ee9ff0ef          	jal	ra,2005cb8e <QueueCreateTrace>
2005ccaa:	40b2                	c.lwsp	ra,12(sp)
2005ccac:	8522                	c.mv	a0,s0
2005ccae:	4422                	c.lwsp	s0,8(sp)
2005ccb0:	4492                	c.lwsp	s1,4(sp)
2005ccb2:	0141                	c.addi	sp,16
2005ccb4:	8082                	c.jr	ra

2005ccb6 <xQueueCreateMutexStatic>:
2005ccb6:	1141                	c.addi	sp,-16
2005ccb8:	872a                	c.mv	a4,a0
2005ccba:	86ae                	c.mv	a3,a1
2005ccbc:	4601                	c.li	a2,0
2005ccbe:	4581                	c.li	a1,0
2005ccc0:	4505                	c.li	a0,1
2005ccc2:	c422                	c.swsp	s0,8(sp)
2005ccc4:	c606                	c.swsp	ra,12(sp)
2005ccc6:	fb5ff0ef          	jal	ra,2005cc7a <xQueueGenericCreateStatic>
2005ccca:	842a                	c.mv	s0,a0
2005cccc:	cd01                	c.beqz	a0,2005cce4 <xQueueCreateMutexStatic+0x2e>
2005ccce:	00052423          	sw	zero,8(a0)
2005ccd2:	00052023          	sw	zero,0(a0)
2005ccd6:	00052623          	sw	zero,12(a0)
2005ccda:	4681                	c.li	a3,0
2005ccdc:	4601                	c.li	a2,0
2005ccde:	4581                	c.li	a1,0
2005cce0:	a19ff0ef          	jal	ra,2005c6f8 <xQueueGenericSend>
2005cce4:	40b2                	c.lwsp	ra,12(sp)
2005cce6:	8522                	c.mv	a0,s0
2005cce8:	4422                	c.lwsp	s0,8(sp)
2005ccea:	0141                	c.addi	sp,16
2005ccec:	8082                	c.jr	ra

2005ccee <xQueueCreateCountingSemaphoreStatic>:
2005ccee:	1141                	c.addi	sp,-16
2005ccf0:	c422                	c.swsp	s0,8(sp)
2005ccf2:	86b2                	c.mv	a3,a2
2005ccf4:	842e                	c.mv	s0,a1
2005ccf6:	4709                	c.li	a4,2
2005ccf8:	4601                	c.li	a2,0
2005ccfa:	4581                	c.li	a1,0
2005ccfc:	c606                	c.swsp	ra,12(sp)
2005ccfe:	f7dff0ef          	jal	ra,2005cc7a <xQueueGenericCreateStatic>
2005cd02:	c111                	c.beqz	a0,2005cd06 <xQueueCreateCountingSemaphoreStatic+0x18>
2005cd04:	dd00                	c.sw	s0,56(a0)
2005cd06:	40b2                	c.lwsp	ra,12(sp)
2005cd08:	4422                	c.lwsp	s0,8(sp)
2005cd0a:	0141                	c.addi	sp,16
2005cd0c:	8082                	c.jr	ra

2005cd0e <QueueDeleteTrace>:
2005cd0e:	20068737          	lui	a4,0x20068
2005cd12:	ae870693          	addi	a3,a4,-1304 # 20067ae8 <QueueTraceList>
2005cd16:	4781                	c.li	a5,0
2005cd18:	ae870713          	addi	a4,a4,-1304
2005cd1c:	4641                	c.li	a2,16
2005cd1e:	428c                	c.lw	a1,0(a3)
2005cd20:	00a59763          	bne	a1,a0,2005cd2e <QueueDeleteTrace+0x20>
2005cd24:	078a                	c.slli	a5,0x2
2005cd26:	97ba                	c.add	a5,a4
2005cd28:	0007a023          	sw	zero,0(a5)
2005cd2c:	8082                	c.jr	ra
2005cd2e:	0785                	c.addi	a5,1
2005cd30:	0691                	c.addi	a3,4
2005cd32:	fec796e3          	bne	a5,a2,2005cd1e <QueueDeleteTrace+0x10>
2005cd36:	8082                	c.jr	ra

2005cd38 <vQueueDelete>:
2005cd38:	1141                	c.addi	sp,-16
2005cd3a:	c422                	c.swsp	s0,8(sp)
2005cd3c:	c606                	c.swsp	ra,12(sp)
2005cd3e:	842a                	c.mv	s0,a0
2005cd40:	fcfff0ef          	jal	ra,2005cd0e <QueueDeleteTrace>
2005cd44:	04644783          	lbu	a5,70(s0)
2005cd48:	e799                	c.bnez	a5,2005cd56 <vQueueDelete+0x1e>
2005cd4a:	8522                	c.mv	a0,s0
2005cd4c:	4422                	c.lwsp	s0,8(sp)
2005cd4e:	40b2                	c.lwsp	ra,12(sp)
2005cd50:	0141                	c.addi	sp,16
2005cd52:	cd9fd06f          	jal	zero,2005aa2a <vPortFree>
2005cd56:	40b2                	c.lwsp	ra,12(sp)
2005cd58:	4422                	c.lwsp	s0,8(sp)
2005cd5a:	0141                	c.addi	sp,16
2005cd5c:	8082                	c.jr	ra

2005cd5e <shell_loguratRx_ipc_int>:
2005cd5e:	1141                	c.addi	sp,-16
2005cd60:	458d                	c.li	a1,3
2005cd62:	4541                	c.li	a0,16
2005cd64:	c422                	c.swsp	s0,8(sp)
2005cd66:	c606                	c.swsp	ra,12(sp)
2005cd68:	20001437          	lui	s0,0x20001
2005cd6c:	b84fc0ef          	jal	ra,200590f0 <ipc_get_message>
2005cd70:	87aa                	c.mv	a5,a0
2005cd72:	b4440713          	addi	a4,s0,-1212 # 20000b44 <shell_ctl>
2005cd76:	4708                	c.lw	a0,8(a4)
2005cd78:	43cc                	c.lw	a1,4(a5)
2005cd7a:	7c1027f3          	csrrs	a5,0x7c1,zero
2005cd7e:	1007f793          	andi	a5,a5,256
2005cd82:	b4440413          	addi	s0,s0,-1212
2005cd86:	c3b9                	c.beqz	a5,2005cdcc <shell_loguratRx_ipc_int+0x6e>
2005cd88:	01f5f713          	andi	a4,a1,31
2005cd8c:	c325                	c.beqz	a4,2005cdec <shell_loguratRx_ipc_int+0x8e>
2005cd8e:	08070693          	addi	a3,a4,128
2005cd92:	fe05f793          	andi	a5,a1,-32
2005cd96:	01f7f713          	andi	a4,a5,31
2005cd9a:	9736                	c.add	a4,a3
2005cd9c:	0ff0000f          	fence	iorw,iorw
2005cda0:	973e                	c.add	a4,a5
2005cda2:	00f7f00b          	cache	dinv,(a5)
2005cda6:	02078793          	addi	a5,a5,32
2005cdaa:	40f706b3          	sub	a3,a4,a5
2005cdae:	fed04ae3          	blt	zero,a3,2005cda2 <shell_loguratRx_ipc_int+0x44>
2005cdb2:	0ff0000f          	fence	iorw,iorw
2005cdb6:	0001                	c.addi	zero,0
2005cdb8:	0001                	c.addi	zero,0
2005cdba:	0001                	c.addi	zero,0
2005cdbc:	0001                	c.addi	zero,0
2005cdbe:	0001                	c.addi	zero,0
2005cdc0:	0001                	c.addi	zero,0
2005cdc2:	0001                	c.addi	zero,0
2005cdc4:	0001                	c.addi	zero,0
2005cdc6:	0001                	c.addi	zero,0
2005cdc8:	0001                	c.addi	zero,0
2005cdca:	0001                	c.addi	zero,0
2005cdcc:	08000613          	addi	a2,zero,128
2005cdd0:	dffad097          	auipc	ra,0xdffad
2005cdd4:	842080e7          	jalr	ra,-1982(ra) # 9612 <_memcpy>
2005cdd8:	4785                	c.li	a5,1
2005cdda:	00f40223          	sb	a5,4(s0)
2005cdde:	501c                	c.lw	a5,32(s0)
2005cde0:	cb91                	c.beqz	a5,2005cdf4 <shell_loguratRx_ipc_int+0x96>
2005cde2:	4c5c                	c.lw	a5,28(s0)
2005cde4:	4422                	c.lwsp	s0,8(sp)
2005cde6:	40b2                	c.lwsp	ra,12(sp)
2005cde8:	0141                	c.addi	sp,16
2005cdea:	8782                	c.jr	a5
2005cdec:	87ae                	c.mv	a5,a1
2005cdee:	08000693          	addi	a3,zero,128
2005cdf2:	b755                	c.j	2005cd96 <shell_loguratRx_ipc_int+0x38>
2005cdf4:	40b2                	c.lwsp	ra,12(sp)
2005cdf6:	4422                	c.lwsp	s0,8(sp)
2005cdf8:	0141                	c.addi	sp,16
2005cdfa:	8082                	c.jr	ra

2005cdfc <shell_give_sema>:
2005cdfc:	200017b7          	lui	a5,0x20001
2005ce00:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005ce04:	539c                	c.lw	a5,32(a5)
2005ce06:	c789                	c.beqz	a5,2005ce10 <shell_give_sema+0x14>
2005ce08:	f5c1a503          	lw	a0,-164(gp) # 200661d8 <shell_sema>
2005ce0c:	b23fe06f          	jal	zero,2005b92e <rtos_sema_give>
2005ce10:	8082                	c.jr	ra

2005ce12 <shell_loguartRx_dispatch>:
2005ce12:	1141                	c.addi	sp,-16
2005ce14:	c422                	c.swsp	s0,8(sp)
2005ce16:	20001437          	lui	s0,0x20001
2005ce1a:	b4440413          	addi	s0,s0,-1212 # 20000b44 <shell_ctl>
2005ce1e:	c04a                	c.swsp	s2,0(sp)
2005ce20:	00842903          	lw	s2,8(s0)
2005ce24:	200635b7          	lui	a1,0x20063
2005ce28:	ef058593          	addi	a1,a1,-272 # 20062ef0 <__FUNCTION__.5+0x20>
2005ce2c:	00190513          	addi	a0,s2,1
2005ce30:	c606                	c.swsp	ra,12(sp)
2005ce32:	c226                	c.swsp	s1,4(sp)
2005ce34:	dffad097          	auipc	ra,0xdffad
2005ce38:	a64080e7          	jalr	ra,-1436(ra) # 9898 <_stricmp>
2005ce3c:	ed15                	c.bnez	a0,2005ce78 <shell_loguartRx_dispatch+0x66>
2005ce3e:	40000513          	addi	a0,zero,1024
2005ce42:	873fe0ef          	jal	ra,2005b6b4 <rtos_mem_malloc>
2005ce46:	3ff00593          	addi	a1,zero,1023
2005ce4a:	84aa                	c.mv	s1,a0
2005ce4c:	261010ef          	jal	ra,2005e8ac <ChipInfo_GetLibVersion_ToBuf>
2005ce50:	20062537          	lui	a0,0x20062
2005ce54:	85a6                	c.mv	a1,s1
2005ce56:	2cc50513          	addi	a0,a0,716 # 200622cc <pmap_func+0x4dc>
2005ce5a:	2a4020ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005ce5e:	8526                	c.mv	a0,s1
2005ce60:	87ffe0ef          	jal	ra,2005b6de <rtos_mem_free>
2005ce64:	4601                	c.li	a2,0
2005ce66:	08000593          	addi	a1,zero,128
2005ce6a:	854a                	c.mv	a0,s2
2005ce6c:	dffac097          	auipc	ra,0xdffac
2005ce70:	016080e7          	jalr	ra,22(ra) # 8e82 <shell_array_init>
2005ce74:	00040223          	sb	zero,4(s0)
2005ce78:	40b2                	c.lwsp	ra,12(sp)
2005ce7a:	4422                	c.lwsp	s0,8(sp)
2005ce7c:	4492                	c.lwsp	s1,4(sp)
2005ce7e:	4902                	c.lwsp	s2,0(sp)
2005ce80:	0141                	c.addi	sp,16
2005ce82:	8082                	c.jr	ra

2005ce84 <shell_task_ram>:
2005ce84:	715d                	c.addi16sp	sp,-80
2005ce86:	c4a2                	c.swsp	s0,72(sp)
2005ce88:	20001437          	lui	s0,0x20001
2005ce8c:	dc52                	c.swsp	s4,56(sp)
2005ce8e:	d85a                	c.swsp	s6,48(sp)
2005ce90:	d65e                	c.swsp	s7,44(sp)
2005ce92:	d462                	c.swsp	s8,40(sp)
2005ce94:	b4440793          	addi	a5,s0,-1212 # 20000b44 <shell_ctl>
2005ce98:	c686                	c.swsp	ra,76(sp)
2005ce9a:	c2a6                	c.swsp	s1,68(sp)
2005ce9c:	c0ca                	c.swsp	s2,64(sp)
2005ce9e:	de4e                	c.swsp	s3,60(sp)
2005cea0:	da56                	c.swsp	s5,52(sp)
2005cea2:	d266                	c.swsp	s9,36(sp)
2005cea4:	d06a                	c.swsp	s10,32(sp)
2005cea6:	ce6e                	c.swsp	s11,28(sp)
2005cea8:	0087aa83          	lw	s5,8(a5)
2005ceac:	4705                	c.li	a4,1
2005ceae:	d398                	c.sw	a4,32(a5)
2005ceb0:	40000513          	addi	a0,zero,1024
2005ceb4:	20068b37          	lui	s6,0x20068
2005ceb8:	00e78323          	sb	a4,6(a5)
2005cebc:	b4440413          	addi	s0,s0,-1212
2005cec0:	cc3fe0ef          	jal	ra,2005bb82 <rtos_create_secure_context>
2005cec4:	001a8a13          	addi	s4,s5,1
2005cec8:	8c5a                	c.mv	s8,s6
2005ceca:	f5c1a503          	lw	a0,-164(gp) # 200661d8 <shell_sema>
2005cece:	55fd                	c.li	a1,-1
2005ced0:	a13fe0ef          	jal	ra,2005b8e2 <rtos_sema_take>
2005ced4:	f3fff0ef          	jal	ra,2005ce12 <shell_loguartRx_dispatch>
2005ced8:	00444783          	lbu	a5,4(s0)
2005cedc:	0ff7f793          	andi	a5,a5,255
2005cee0:	d7ed                	c.beqz	a5,2005ceca <shell_task_ram+0x46>
2005cee2:	85d2                	c.mv	a1,s4
2005cee4:	b28b0513          	addi	a0,s6,-1240 # 20067b28 <log_buf>
2005cee8:	063040ef          	jal	ra,2006174a <strcpy>
2005ceec:	8552                	c.mv	a0,s4
2005ceee:	dffac097          	auipc	ra,0xdffac
2005cef2:	fac080e7          	jalr	ra,-84(ra) # 8e9a <shell_get_argc>
2005cef6:	892a                	c.mv	s2,a0
2005cef8:	8552                	c.mv	a0,s4
2005cefa:	dffac097          	auipc	ra,0xdffac
2005cefe:	fda080e7          	jalr	ra,-38(ra) # 8ed4 <shell_get_argv>
2005cf02:	00052d83          	lw	s11,0(a0)
2005cf06:	4804                	c.lw	s1,16(s0)
2005cf08:	01442d03          	lw	s10,20(s0)
2005cf0c:	89aa                	c.mv	s3,a0
2005cf0e:	4c81                	c.li	s9,0
2005cf10:	019d0e63          	beq	s10,s9,2005cf2c <shell_task_ram+0xa8>
2005cf14:	408c                	c.lw	a1,0(s1)
2005cf16:	856e                	c.mv	a0,s11
2005cf18:	c626                	c.swsp	s1,12(sp)
2005cf1a:	dffad097          	auipc	ra,0xdffad
2005cf1e:	97e080e7          	jalr	ra,-1666(ra) # 9898 <_stricmp>
2005cf22:	47b2                	c.lwsp	a5,12(sp)
2005cf24:	04c1                	c.addi	s1,16
2005cf26:	e115                	c.bnez	a0,2005cf4a <shell_task_ram+0xc6>
2005cf28:	479c                	c.lw	a5,8(a5)
2005cf2a:	e395                	c.bnez	a5,2005cf4e <shell_task_ram+0xca>
2005cf2c:	b28c0513          	addi	a0,s8,-1240
2005cf30:	a28fe0ef          	jal	ra,2005b158 <log_service>
2005cf34:	4601                	c.li	a2,0
2005cf36:	08000593          	addi	a1,zero,128
2005cf3a:	8556                	c.mv	a0,s5
2005cf3c:	dffac097          	auipc	ra,0xdffac
2005cf40:	f46080e7          	jalr	ra,-186(ra) # 8e82 <shell_array_init>
2005cf44:	00040223          	sb	zero,4(s0)
2005cf48:	b749                	c.j	2005ceca <shell_task_ram+0x46>
2005cf4a:	0c85                	c.addi	s9,1
2005cf4c:	b7d1                	c.j	2005cf10 <shell_task_ram+0x8c>
2005cf4e:	fff90513          	addi	a0,s2,-1
2005cf52:	0542                	c.slli	a0,0x10
2005cf54:	00498593          	addi	a1,s3,4
2005cf58:	8141                	c.srli	a0,0x10
2005cf5a:	9782                	c.jalr	a5
2005cf5c:	bfe1                	c.j	2005cf34 <shell_task_ram+0xb0>

2005cf5e <shell_init_ram>:
2005cf5e:	1141                	c.addi	sp,-16
2005cf60:	c606                	c.swsp	ra,12(sp)
2005cf62:	ff7fd0ef          	jal	ra,2005af58 <log_service_init>
2005cf66:	20064737          	lui	a4,0x20064
2005cf6a:	54070693          	addi	a3,a4,1344 # 20064540 <shell_cmd_table>
2005cf6e:	20064737          	lui	a4,0x20064
2005cf72:	200017b7          	lui	a5,0x20001
2005cf76:	58070713          	addi	a4,a4,1408 # 20064580 <ipc_tickless_table>
2005cf7a:	b4478793          	addi	a5,a5,-1212 # 20000b44 <shell_ctl>
2005cf7e:	8f15                	c.sub	a4,a3
2005cf80:	cb94                	c.sw	a3,16(a5)
2005cf82:	8311                	c.srli	a4,0x4
2005cf84:	cbd8                	c.sw	a4,20(a5)
2005cf86:	00078223          	sb	zero,4(a5)
2005cf8a:	4705                	c.li	a4,1
2005cf8c:	00e782a3          	sb	a4,5(a5)
2005cf90:	2005d737          	lui	a4,0x2005d
2005cf94:	dfc70713          	addi	a4,a4,-516 # 2005cdfc <shell_give_sema>
2005cf98:	f5c18513          	addi	a0,gp,-164 # 200661d8 <shell_sema>
2005cf9c:	cfd8                	c.sw	a4,28(a5)
2005cf9e:	905fe0ef          	jal	ra,2005b8a2 <rtos_sema_create_binary>
2005cfa2:	2005d637          	lui	a2,0x2005d
2005cfa6:	200635b7          	lui	a1,0x20063
2005cfaa:	4795                	c.li	a5,5
2005cfac:	6705                	c.lui	a4,0x1
2005cfae:	4681                	c.li	a3,0
2005cfb0:	e8460613          	addi	a2,a2,-380 # 2005ce84 <shell_task_ram>
2005cfb4:	ef858593          	addi	a1,a1,-264 # 20062ef8 <__FUNCTION__.5+0x28>
2005cfb8:	4501                	c.li	a0,0
2005cfba:	b95fe0ef          	jal	ra,2005bb4e <rtos_task_create>
2005cfbe:	c105                	c.beqz	a0,2005cfde <shell_init_ram+0x80>
2005cfc0:	40b2                	c.lwsp	ra,12(sp)
2005cfc2:	200636b7          	lui	a3,0x20063
2005cfc6:	200635b7          	lui	a1,0x20063
2005cfca:	f0868693          	addi	a3,a3,-248 # 20062f08 <__FUNCTION__.5+0x38>
2005cfce:	04500613          	addi	a2,zero,69
2005cfd2:	f2458593          	addi	a1,a1,-220 # 20062f24 <__FUNCTION__.5+0x54>
2005cfd6:	4509                	c.li	a0,2
2005cfd8:	0141                	c.addi	sp,16
2005cfda:	edffd06f          	jal	zero,2005aeb8 <rtk_log_write>
2005cfde:	40b2                	c.lwsp	ra,12(sp)
2005cfe0:	0141                	c.addi	sp,16
2005cfe2:	8082                	c.jr	ra

2005cfe4 <sys_clear_ota_signature>:
2005cfe4:	7155                	c.addi16sp	sp,-208
2005cfe6:	4601                	c.li	a2,0
2005cfe8:	102c                	c.addi4spn	a1,sp,40
2005cfea:	4509                	c.li	a0,2
2005cfec:	c786                	c.swsp	ra,204(sp)
2005cfee:	c3a6                	c.swsp	s1,196(sp)
2005cff0:	c5a2                	c.swsp	s0,200(sp)
2005cff2:	c1ca                	c.swsp	s2,192(sp)
2005cff4:	df4e                	c.swsp	s3,188(sp)
2005cff6:	dd52                	c.swsp	s4,184(sp)
2005cff8:	c082                	c.swsp	zero,64(sp)
2005cffa:	c282                	c.swsp	zero,68(sp)
2005cffc:	b09fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d000:	4601                	c.li	a2,0
2005d002:	106c                	c.addi4spn	a1,sp,44
2005d004:	450d                	c.li	a0,3
2005d006:	afffb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d00a:	4505                	c.li	a0,1
2005d00c:	bb6fc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d010:	0ff57693          	andi	a3,a0,255
2005d014:	0016c493          	xori	s1,a3,1
2005d018:	f80007b7          	lui	a5,0xf8000
2005d01c:	e2e9                	c.bnez	a3,2005d0de <sys_clear_ota_signature+0xfa>
2005d01e:	5722                	c.lwsp	a4,40(sp)
2005d020:	0ff57813          	andi	a6,a0,255
2005d024:	97ba                	c.add	a5,a4
2005d026:	00281713          	slli	a4,a6,0x2
2005d02a:	0a070613          	addi	a2,a4,160 # 10a0 <CPU_ClkGet+0x58a>
2005d02e:	080c                	c.addi4spn	a1,sp,16
2005d030:	962e                	c.add	a2,a1
2005d032:	f8f62423          	sw	a5,-120(a2)
2005d036:	4785                	c.li	a5,1
2005d038:	f8000437          	lui	s0,0xf8000
2005d03c:	0af69363          	bne	a3,a5,2005d0e2 <sys_clear_ota_signature+0xfe>
2005d040:	57a2                	c.lwsp	a5,40(sp)
2005d042:	943e                	c.add	s0,a5
2005d044:	00249793          	slli	a5,s1,0x2
2005d048:	0814                	c.addi4spn	a3,sp,16
2005d04a:	0a078793          	addi	a5,a5,160 # f80000a0 <__ctrace_end__+0x2e0000a0>
2005d04e:	97b6                	c.add	a5,a3
2005d050:	f887a423          	sw	s0,-120(a5)
2005d054:	0a070793          	addi	a5,a4,160
2005d058:	00d78733          	add	a4,a5,a3
2005d05c:	f8872a03          	lw	s4,-120(a4)
2005d060:	0485                	c.addi	s1,1
2005d062:	200639b7          	lui	s3,0x20063
2005d066:	200636b7          	lui	a3,0x20063
2005d06a:	20063937          	lui	s2,0x20063
2005d06e:	0805                	c.addi	a6,1
2005d070:	4785                	c.li	a5,1
2005d072:	00898713          	addi	a4,s3,8 # 20063008 <__func__.1>
2005d076:	88d2                	c.mv	a7,s4
2005d078:	f2c68693          	addi	a3,a3,-212 # 20062f2c <__FUNCTION__.5+0x5c>
2005d07c:	04100613          	addi	a2,zero,65
2005d080:	f7c90593          	addi	a1,s2,-132 # 20062f7c <__FUNCTION__.5+0xac>
2005d084:	4505                	c.li	a0,1
2005d086:	c222                	c.swsp	s0,4(sp)
2005d088:	c026                	c.swsp	s1,0(sp)
2005d08a:	ce42                	c.swsp	a6,28(sp)
2005d08c:	e2dfd0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005d090:	1810                	c.addi4spn	a2,sp,48
2005d092:	85a2                	c.mv	a1,s0
2005d094:	00a8                	c.addi4spn	a0,sp,72
2005d096:	f20fd0ef          	jal	ra,2005a7b6 <flash_read_word>
2005d09a:	1850                	c.addi4spn	a2,sp,52
2005d09c:	00440593          	addi	a1,s0,4 # f8000004 <__ctrace_end__+0x2e000004>
2005d0a0:	00a8                	c.addi4spn	a0,sp,72
2005d0a2:	f14fd0ef          	jal	ra,2005a7b6 <flash_read_word>
2005d0a6:	5742                	c.lwsp	a4,48(sp)
2005d0a8:	353937b7          	lui	a5,0x35393
2005d0ac:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d0b0:	4872                	c.lwsp	a6,28(sp)
2005d0b2:	02f71a63          	bne	a4,a5,2005d0e6 <sys_clear_ota_signature+0x102>
2005d0b6:	5752                	c.lwsp	a4,52(sp)
2005d0b8:	313137b7          	lui	a5,0x31313
2005d0bc:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d0c0:	02f71363          	bne	a4,a5,2005d0e6 <sys_clear_ota_signature+0x102>
2005d0c4:	0090                	c.addi4spn	a2,sp,64
2005d0c6:	45a1                	c.li	a1,8
2005d0c8:	8552                	c.mv	a0,s4
2005d0ca:	aeffb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d0ce:	40be                	c.lwsp	ra,204(sp)
2005d0d0:	442e                	c.lwsp	s0,200(sp)
2005d0d2:	449e                	c.lwsp	s1,196(sp)
2005d0d4:	490e                	c.lwsp	s2,192(sp)
2005d0d6:	59fa                	c.lwsp	s3,188(sp)
2005d0d8:	5a6a                	c.lwsp	s4,184(sp)
2005d0da:	6169                	c.addi16sp	sp,208
2005d0dc:	8082                	c.jr	ra
2005d0de:	5732                	c.lwsp	a4,44(sp)
2005d0e0:	b781                	c.j	2005d020 <sys_clear_ota_signature+0x3c>
2005d0e2:	57b2                	c.lwsp	a5,44(sp)
2005d0e4:	bfb9                	c.j	2005d042 <sys_clear_ota_signature+0x5e>
2005d0e6:	200636b7          	lui	a3,0x20063
2005d0ea:	88a6                	c.mv	a7,s1
2005d0ec:	4785                	c.li	a5,1
2005d0ee:	00898713          	addi	a4,s3,8
2005d0f2:	f8068693          	addi	a3,a3,-128 # 20062f80 <__FUNCTION__.5+0xb0>
2005d0f6:	04500613          	addi	a2,zero,69
2005d0fa:	f7c90593          	addi	a1,s2,-132
2005d0fe:	4509                	c.li	a0,2
2005d100:	db9fd0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005d104:	b7e9                	c.j	2005d0ce <sys_clear_ota_signature+0xea>

2005d106 <sys_recover_ota_signature>:
2005d106:	353937b7          	lui	a5,0x35393
2005d10a:	715d                	c.addi16sp	sp,-80
2005d10c:	13878793          	addi	a5,a5,312 # 35393138 <__km4_bd_boot_download_addr__+0x5381138>
2005d110:	d43e                	c.swsp	a5,40(sp)
2005d112:	313137b7          	lui	a5,0x31313
2005d116:	73878793          	addi	a5,a5,1848 # 31313738 <__km4_bd_boot_download_addr__+0x1301738>
2005d11a:	6505                	c.lui	a0,0x1
2005d11c:	c2a6                	c.swsp	s1,68(sp)
2005d11e:	de4e                	c.swsp	s3,60(sp)
2005d120:	c686                	c.swsp	ra,76(sp)
2005d122:	c4a2                	c.swsp	s0,72(sp)
2005d124:	c0ca                	c.swsp	s2,64(sp)
2005d126:	d63e                	c.swsp	a5,44(sp)
2005d128:	fd2fd0ef          	jal	ra,2005a8fa <__wrap_malloc>
2005d12c:	200639b7          	lui	s3,0x20063
2005d130:	200634b7          	lui	s1,0x20063
2005d134:	e50d                	c.bnez	a0,2005d15e <sys_recover_ota_signature+0x58>
2005d136:	200636b7          	lui	a3,0x20063
2005d13a:	fec98713          	addi	a4,s3,-20 # 20062fec <__func__.0>
2005d13e:	f7c48593          	addi	a1,s1,-132 # 20062f7c <__FUNCTION__.5+0xac>
2005d142:	fd068693          	addi	a3,a3,-48 # 20062fd0 <__FUNCTION__.5+0x100>
2005d146:	04500613          	addi	a2,zero,69
2005d14a:	4509                	c.li	a0,2
2005d14c:	d6dfd0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005d150:	40b6                	c.lwsp	ra,76(sp)
2005d152:	4426                	c.lwsp	s0,72(sp)
2005d154:	4496                	c.lwsp	s1,68(sp)
2005d156:	4906                	c.lwsp	s2,64(sp)
2005d158:	59f2                	c.lwsp	s3,60(sp)
2005d15a:	6161                	c.addi16sp	sp,80
2005d15c:	8082                	c.jr	ra
2005d15e:	4601                	c.li	a2,0
2005d160:	082c                	c.addi4spn	a1,sp,24
2005d162:	842a                	c.mv	s0,a0
2005d164:	4509                	c.li	a0,2
2005d166:	99ffb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d16a:	086c                	c.addi4spn	a1,sp,28
2005d16c:	4601                	c.li	a2,0
2005d16e:	450d                	c.li	a0,3
2005d170:	995fb0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005d174:	4505                	c.li	a0,1
2005d176:	a4cfc0ef          	jal	ra,200593c2 <ota_get_cur_index>
2005d17a:	0ff57593          	andi	a1,a0,255
2005d17e:	4785                	c.li	a5,1
2005d180:	0015c693          	xori	a3,a1,1
2005d184:	0af59963          	bne	a1,a5,2005d236 <sys_recover_ota_signature+0x130>
2005d188:	4762                	c.lwsp	a4,24(sp)
2005d18a:	f80007b7          	lui	a5,0xf8000
2005d18e:	00269613          	slli	a2,a3,0x2
2005d192:	97ba                	c.add	a5,a4
2005d194:	03060713          	addi	a4,a2,48
2005d198:	970a                	c.add	a4,sp
2005d19a:	fef72823          	sw	a5,-16(a4)
2005d19e:	f80008b7          	lui	a7,0xf8000
2005d1a2:	edd1                	c.bnez	a1,2005d23e <sys_recover_ota_signature+0x138>
2005d1a4:	47e2                	c.lwsp	a5,24(sp)
2005d1a6:	0ff57813          	andi	a6,a0,255
2005d1aa:	98be                	c.add	a7,a5
2005d1ac:	00281793          	slli	a5,a6,0x2
2005d1b0:	03078793          	addi	a5,a5,48 # f8000030 <__ctrace_end__+0x2e000030>
2005d1b4:	978a                	c.add	a5,sp
2005d1b6:	ff17a823          	sw	a7,-16(a5)
2005d1ba:	03060793          	addi	a5,a2,48
2005d1be:	00278633          	add	a2,a5,sp
2005d1c2:	ff062903          	lw	s2,-16(a2)
2005d1c6:	0685                	c.addi	a3,1
2005d1c8:	c036                	c.swsp	a3,0(sp)
2005d1ca:	200636b7          	lui	a3,0x20063
2005d1ce:	fec98713          	addi	a4,s3,-20
2005d1d2:	0805                	c.addi	a6,1
2005d1d4:	4785                	c.li	a5,1
2005d1d6:	f2c68693          	addi	a3,a3,-212 # 20062f2c <__FUNCTION__.5+0x5c>
2005d1da:	f7c48593          	addi	a1,s1,-132
2005d1de:	04100613          	addi	a2,zero,65
2005d1e2:	4505                	c.li	a0,1
2005d1e4:	c24a                	c.swsp	s2,4(sp)
2005d1e6:	cd3fd0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005d1ea:	8622                	c.mv	a2,s0
2005d1ec:	6585                	c.lui	a1,0x1
2005d1ee:	854a                	c.mv	a0,s2
2005d1f0:	983fb0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005d1f4:	85ca                	c.mv	a1,s2
2005d1f6:	4509                	c.li	a0,2
2005d1f8:	aa7fb0ef          	jal	ra,20058c9e <FLASH_EraseXIP>
2005d1fc:	4621                	c.li	a2,8
2005d1fe:	102c                	c.addi4spn	a1,sp,40
2005d200:	8522                	c.mv	a0,s0
2005d202:	dffac097          	auipc	ra,0xdffac
2005d206:	410080e7          	jalr	ra,1040(ra) # 9612 <_memcpy>
2005d20a:	4481                	c.li	s1,0
2005d20c:	6985                	c.lui	s3,0x1
2005d20e:	01248533          	add	a0,s1,s2
2005d212:	8622                	c.mv	a2,s0
2005d214:	10000593          	addi	a1,zero,256
2005d218:	10048493          	addi	s1,s1,256
2005d21c:	99dfb0ef          	jal	ra,20058bb8 <FLASH_WriteStream>
2005d220:	ff3497e3          	bne	s1,s3,2005d20e <sys_recover_ota_signature+0x108>
2005d224:	8522                	c.mv	a0,s0
2005d226:	4426                	c.lwsp	s0,72(sp)
2005d228:	40b6                	c.lwsp	ra,76(sp)
2005d22a:	4496                	c.lwsp	s1,68(sp)
2005d22c:	4906                	c.lwsp	s2,64(sp)
2005d22e:	59f2                	c.lwsp	s3,60(sp)
2005d230:	6161                	c.addi16sp	sp,80
2005d232:	eccfd06f          	jal	zero,2005a8fe <__wrap_free>
2005d236:	4772                	c.lwsp	a4,28(sp)
2005d238:	f80007b7          	lui	a5,0xf8000
2005d23c:	bf89                	c.j	2005d18e <sys_recover_ota_signature+0x88>
2005d23e:	47f2                	c.lwsp	a5,28(sp)
2005d240:	b79d                	c.j	2005d1a6 <sys_recover_ota_signature+0xa0>

2005d242 <sys_reset>:
2005d242:	95bfc06f          	jal	zero,20059b9c <System_Reset>

2005d246 <prvAddCurrentTaskToDelayedList>:
2005d246:	1101                	c.addi	sp,-32
2005d248:	ca26                	c.swsp	s1,20(sp)
2005d24a:	cc22                	c.swsp	s0,24(sp)
2005d24c:	c84a                	c.swsp	s2,16(sp)
2005d24e:	c64e                	c.swsp	s3,12(sp)
2005d250:	ce06                	c.swsp	ra,28(sp)
2005d252:	f981a983          	lw	s3,-104(gp) # 20066214 <xTickCount>
2005d256:	f6018793          	addi	a5,gp,-160 # 200661dc <pxCurrentTCB>
2005d25a:	842a                	c.mv	s0,a0
2005d25c:	4388                	c.lw	a0,0(a5)
2005d25e:	892e                	c.mv	s2,a1
2005d260:	0511                	c.addi	a0,4
2005d262:	983fd0ef          	jal	ra,2005abe4 <uxListRemove>
2005d266:	f6018793          	addi	a5,gp,-160 # 200661dc <pxCurrentTCB>
2005d26a:	ed09                	c.bnez	a0,2005d284 <prvAddCurrentTaskToDelayedList+0x3e>
2005d26c:	4398                	c.lw	a4,0(a5)
2005d26e:	f8418693          	addi	a3,gp,-124 # 20066200 <uxTopReadyPriority>
2005d272:	574c                	c.lw	a1,44(a4)
2005d274:	4290                	c.lw	a2,0(a3)
2005d276:	4705                	c.li	a4,1
2005d278:	00b71733          	sll	a4,a4,a1
2005d27c:	fff74713          	xori	a4,a4,-1
2005d280:	8f71                	c.and	a4,a2
2005d282:	c298                	c.sw	a4,0(a3)
2005d284:	577d                	c.li	a4,-1
2005d286:	02e41263          	bne	s0,a4,2005d2aa <prvAddCurrentTaskToDelayedList+0x64>
2005d28a:	02090063          	beq	s2,zero,2005d2aa <prvAddCurrentTaskToDelayedList+0x64>
2005d28e:	438c                	c.lw	a1,0(a5)
2005d290:	4462                	c.lwsp	s0,24(sp)
2005d292:	40f2                	c.lwsp	ra,28(sp)
2005d294:	44d2                	c.lwsp	s1,20(sp)
2005d296:	4942                	c.lwsp	s2,16(sp)
2005d298:	49b2                	c.lwsp	s3,12(sp)
2005d29a:	20068537          	lui	a0,0x20068
2005d29e:	0591                	c.addi	a1,4
2005d2a0:	cc050513          	addi	a0,a0,-832 # 20067cc0 <xSuspendedTaskList>
2005d2a4:	6105                	c.addi16sp	sp,32
2005d2a6:	8fbfd06f          	jal	zero,2005aba0 <vListInsertEnd>
2005d2aa:	4398                	c.lw	a4,0(a5)
2005d2ac:	944e                	c.add	s0,s3
2005d2ae:	c340                	c.sw	s0,4(a4)
2005d2b0:	01347e63          	bgeu	s0,s3,2005d2cc <prvAddCurrentTaskToDelayedList+0x86>
2005d2b4:	f681a503          	lw	a0,-152(gp) # 200661e4 <pxOverflowDelayedTaskList>
2005d2b8:	4462                	c.lwsp	s0,24(sp)
2005d2ba:	438c                	c.lw	a1,0(a5)
2005d2bc:	40f2                	c.lwsp	ra,28(sp)
2005d2be:	44d2                	c.lwsp	s1,20(sp)
2005d2c0:	4942                	c.lwsp	s2,16(sp)
2005d2c2:	49b2                	c.lwsp	s3,12(sp)
2005d2c4:	0591                	c.addi	a1,4
2005d2c6:	6105                	c.addi16sp	sp,32
2005d2c8:	8effd06f          	jal	zero,2005abb6 <vListInsert>
2005d2cc:	f641a503          	lw	a0,-156(gp) # 200661e0 <pxDelayedTaskList>
2005d2d0:	438c                	c.lw	a1,0(a5)
2005d2d2:	0591                	c.addi	a1,4
2005d2d4:	8e3fd0ef          	jal	ra,2005abb6 <vListInsert>
2005d2d8:	f8818793          	addi	a5,gp,-120 # 20066204 <xNextTaskUnblockTime>
2005d2dc:	4398                	c.lw	a4,0(a5)
2005d2de:	00e47363          	bgeu	s0,a4,2005d2e4 <prvAddCurrentTaskToDelayedList+0x9e>
2005d2e2:	c380                	c.sw	s0,0(a5)
2005d2e4:	40f2                	c.lwsp	ra,28(sp)
2005d2e6:	4462                	c.lwsp	s0,24(sp)
2005d2e8:	44d2                	c.lwsp	s1,20(sp)
2005d2ea:	4942                	c.lwsp	s2,16(sp)
2005d2ec:	49b2                	c.lwsp	s3,12(sp)
2005d2ee:	6105                	c.addi16sp	sp,32
2005d2f0:	8082                	c.jr	ra

2005d2f2 <prvWriteNameToBuffer>:
2005d2f2:	1141                	c.addi	sp,-16
2005d2f4:	c606                	c.swsp	ra,12(sp)
2005d2f6:	c422                	c.swsp	s0,8(sp)
2005d2f8:	842a                	c.mv	s0,a0
2005d2fa:	450040ef          	jal	ra,2006174a <strcpy>
2005d2fe:	8522                	c.mv	a0,s0
2005d300:	dffac097          	auipc	ra,0xdffac
2005d304:	7d8080e7          	jalr	ra,2008(ra) # 9ad8 <__wrap_strlen>
2005d308:	87aa                	c.mv	a5,a0
2005d30a:	4759                	c.li	a4,22
2005d30c:	02000693          	addi	a3,zero,32
2005d310:	02f77063          	bgeu	a4,a5,2005d330 <prvWriteNameToBuffer+0x3e>
2005d314:	475d                	c.li	a4,23
2005d316:	4781                	c.li	a5,0
2005d318:	00a76463          	bltu	a4,a0,2005d320 <prvWriteNameToBuffer+0x2e>
2005d31c:	40a707b3          	sub	a5,a4,a0
2005d320:	953e                	c.add	a0,a5
2005d322:	9522                	c.add	a0,s0
2005d324:	00050023          	sb	zero,0(a0)
2005d328:	40b2                	c.lwsp	ra,12(sp)
2005d32a:	4422                	c.lwsp	s0,8(sp)
2005d32c:	0141                	c.addi	sp,16
2005d32e:	8082                	c.jr	ra
2005d330:	00f40633          	add	a2,s0,a5
2005d334:	00d60023          	sb	a3,0(a2)
2005d338:	0785                	c.addi	a5,1
2005d33a:	bfd9                	c.j	2005d310 <prvWriteNameToBuffer+0x1e>

2005d33c <prvGetExpectedIdleTime>:
2005d33c:	f841a703          	lw	a4,-124(gp) # 20066200 <uxTopReadyPriority>
2005d340:	f601a783          	lw	a5,-160(gp) # 200661dc <pxCurrentTCB>
2005d344:	57c8                	c.lw	a0,44(a5)
2005d346:	e105                	c.bnez	a0,2005d366 <prvGetExpectedIdleTime+0x2a>
2005d348:	200687b7          	lui	a5,0x20068
2005d34c:	ba87a683          	lw	a3,-1112(a5) # 20067ba8 <pxReadyTasksLists>
2005d350:	4785                	c.li	a5,1
2005d352:	00d7eb63          	bltu	a5,a3,2005d368 <prvGetExpectedIdleTime+0x2c>
2005d356:	00e7e963          	bltu	a5,a4,2005d368 <prvGetExpectedIdleTime+0x2c>
2005d35a:	f881a503          	lw	a0,-120(gp) # 20066204 <xNextTaskUnblockTime>
2005d35e:	f981a783          	lw	a5,-104(gp) # 20066214 <xTickCount>
2005d362:	8d1d                	c.sub	a0,a5
2005d364:	8082                	c.jr	ra
2005d366:	4501                	c.li	a0,0
2005d368:	8082                	c.jr	ra

2005d36a <prvResetNextTaskUnblockTime>:
2005d36a:	f6418713          	addi	a4,gp,-156 # 200661e0 <pxDelayedTaskList>
2005d36e:	431c                	c.lw	a5,0(a4)
2005d370:	4394                	c.lw	a3,0(a5)
2005d372:	f8818793          	addi	a5,gp,-120 # 20066204 <xNextTaskUnblockTime>
2005d376:	e681                	c.bnez	a3,2005d37e <prvResetNextTaskUnblockTime+0x14>
2005d378:	577d                	c.li	a4,-1
2005d37a:	c398                	c.sw	a4,0(a5)
2005d37c:	8082                	c.jr	ra
2005d37e:	4318                	c.lw	a4,0(a4)
2005d380:	4758                	c.lw	a4,12(a4)
2005d382:	4318                	c.lw	a4,0(a4)
2005d384:	bfdd                	c.j	2005d37a <prvResetNextTaskUnblockTime+0x10>

2005d386 <prvDeleteTCB>:
2005d386:	1141                	c.addi	sp,-16
2005d388:	c422                	c.swsp	s0,8(sp)
2005d38a:	842a                	c.mv	s0,a0
2005d38c:	06c50513          	addi	a0,a0,108
2005d390:	c606                	c.swsp	ra,12(sp)
2005d392:	2ce040ef          	jal	ra,20061660 <_reclaim_reent>
2005d396:	16144783          	lbu	a5,353(s0)
2005d39a:	eb91                	c.bnez	a5,2005d3ae <prvDeleteTCB+0x28>
2005d39c:	5808                	c.lw	a0,48(s0)
2005d39e:	e8cfd0ef          	jal	ra,2005aa2a <vPortFree>
2005d3a2:	8522                	c.mv	a0,s0
2005d3a4:	4422                	c.lwsp	s0,8(sp)
2005d3a6:	40b2                	c.lwsp	ra,12(sp)
2005d3a8:	0141                	c.addi	sp,16
2005d3aa:	e80fd06f          	jal	zero,2005aa2a <vPortFree>
2005d3ae:	4705                	c.li	a4,1
2005d3b0:	fee789e3          	beq	a5,a4,2005d3a2 <prvDeleteTCB+0x1c>
2005d3b4:	40b2                	c.lwsp	ra,12(sp)
2005d3b6:	4422                	c.lwsp	s0,8(sp)
2005d3b8:	0141                	c.addi	sp,16
2005d3ba:	8082                	c.jr	ra

2005d3bc <prvInitialiseNewTask.constprop.0>:
2005d3bc:	1101                	c.addi	sp,-32
2005d3be:	c452                	c.swsp	s4,8(sp)
2005d3c0:	8a2a                	c.mv	s4,a0
2005d3c2:	03082503          	lw	a0,48(a6)
2005d3c6:	c64e                	c.swsp	s3,12(sp)
2005d3c8:	00261993          	slli	s3,a2,0x2
2005d3cc:	864e                	c.mv	a2,s3
2005d3ce:	ca26                	c.swsp	s1,20(sp)
2005d3d0:	84ae                	c.mv	s1,a1
2005d3d2:	0a500593          	addi	a1,zero,165
2005d3d6:	cc22                	c.swsp	s0,24(sp)
2005d3d8:	c84a                	c.swsp	s2,16(sp)
2005d3da:	8442                	c.mv	s0,a6
2005d3dc:	c256                	c.swsp	s5,4(sp)
2005d3de:	c05a                	c.swsp	s6,0(sp)
2005d3e0:	ce06                	c.swsp	ra,28(sp)
2005d3e2:	8ab6                	c.mv	s5,a3
2005d3e4:	893a                	c.mv	s2,a4
2005d3e6:	8b3e                	c.mv	s6,a5
2005d3e8:	dffac097          	auipc	ra,0xdffac
2005d3ec:	6e6080e7          	jalr	ra,1766(ra) # 9ace <__wrap_memset>
2005d3f0:	ffc98613          	addi	a2,s3,-4 # ffc <CPU_ClkGet+0x4e6>
2005d3f4:	03042983          	lw	s3,48(s0)
2005d3f8:	99b2                	c.add	s3,a2
2005d3fa:	fe09f993          	andi	s3,s3,-32
2005d3fe:	05342623          	sw	s3,76(s0)
2005d402:	c8f9                	c.beqz	s1,2005d4d8 <prvInitialiseNewTask.constprop.0+0x11c>
2005d404:	85a6                	c.mv	a1,s1
2005d406:	03440793          	addi	a5,s0,52
2005d40a:	01848693          	addi	a3,s1,24
2005d40e:	0005c703          	lbu	a4,0(a1) # 1000 <CPU_ClkGet+0x4ea>
2005d412:	00e78023          	sb	a4,0(a5)
2005d416:	c709                	c.beqz	a4,2005d420 <prvInitialiseNewTask.constprop.0+0x64>
2005d418:	0585                	c.addi	a1,1
2005d41a:	0785                	c.addi	a5,1
2005d41c:	fed599e3          	bne	a1,a3,2005d40e <prvInitialiseNewTask.constprop.0+0x52>
2005d420:	040405a3          	sb	zero,75(s0)
2005d424:	47a9                	c.li	a5,10
2005d426:	0127f363          	bgeu	a5,s2,2005d42c <prvInitialiseNewTask.constprop.0+0x70>
2005d42a:	4929                	c.li	s2,10
2005d42c:	00440513          	addi	a0,s0,4
2005d430:	03242623          	sw	s2,44(s0)
2005d434:	05242e23          	sw	s2,92(s0)
2005d438:	06042023          	sw	zero,96(s0)
2005d43c:	f5efd0ef          	jal	ra,2005ab9a <vListInitialiseItem>
2005d440:	01840513          	addi	a0,s0,24
2005d444:	f56fd0ef          	jal	ra,2005ab9a <vListInitialiseItem>
2005d448:	472d                	c.li	a4,11
2005d44a:	412704b3          	sub	s1,a4,s2
2005d44e:	4611                	c.li	a2,4
2005d450:	4581                	c.li	a1,0
2005d452:	c800                	c.sw	s0,16(s0)
2005d454:	cc04                	c.sw	s1,24(s0)
2005d456:	d040                	c.sw	s0,36(s0)
2005d458:	04042823          	sw	zero,80(s0)
2005d45c:	06042223          	sw	zero,100(s0)
2005d460:	06042423          	sw	zero,104(s0)
2005d464:	15c40513          	addi	a0,s0,348
2005d468:	dffac097          	auipc	ra,0xdffac
2005d46c:	666080e7          	jalr	ra,1638(ra) # 9ace <__wrap_memset>
2005d470:	4605                	c.li	a2,1
2005d472:	4581                	c.li	a1,0
2005d474:	16040513          	addi	a0,s0,352
2005d478:	dffac097          	auipc	ra,0xdffac
2005d47c:	656080e7          	jalr	ra,1622(ra) # 9ace <__wrap_memset>
2005d480:	0f000613          	addi	a2,zero,240
2005d484:	4581                	c.li	a1,0
2005d486:	06c40513          	addi	a0,s0,108
2005d48a:	dffac097          	auipc	ra,0xdffac
2005d48e:	644080e7          	jalr	ra,1604(ra) # 9ace <__wrap_memset>
2005d492:	200687b7          	lui	a5,0x20068
2005d496:	0f478793          	addi	a5,a5,244 # 200680f4 <__sf>
2005d49a:	d83c                	c.sw	a5,112(s0)
2005d49c:	200687b7          	lui	a5,0x20068
2005d4a0:	15c78793          	addi	a5,a5,348 # 2006815c <__sf+0x68>
2005d4a4:	d87c                	c.sw	a5,116(s0)
2005d4a6:	200687b7          	lui	a5,0x20068
2005d4aa:	1c478793          	addi	a5,a5,452 # 200681c4 <__sf+0xd0>
2005d4ae:	dc3c                	c.sw	a5,120(s0)
2005d4b0:	8656                	c.mv	a2,s5
2005d4b2:	85d2                	c.mv	a1,s4
2005d4b4:	854e                	c.mv	a0,s3
2005d4b6:	b1bfe0ef          	jal	ra,2005bfd0 <pxPortInitialiseStack>
2005d4ba:	c008                	c.sw	a0,0(s0)
2005d4bc:	000b0463          	beq	s6,zero,2005d4c4 <prvInitialiseNewTask.constprop.0+0x108>
2005d4c0:	008b2023          	sw	s0,0(s6)
2005d4c4:	40f2                	c.lwsp	ra,28(sp)
2005d4c6:	4462                	c.lwsp	s0,24(sp)
2005d4c8:	44d2                	c.lwsp	s1,20(sp)
2005d4ca:	4942                	c.lwsp	s2,16(sp)
2005d4cc:	49b2                	c.lwsp	s3,12(sp)
2005d4ce:	4a22                	c.lwsp	s4,8(sp)
2005d4d0:	4a92                	c.lwsp	s5,4(sp)
2005d4d2:	4b02                	c.lwsp	s6,0(sp)
2005d4d4:	6105                	c.addi16sp	sp,32
2005d4d6:	8082                	c.jr	ra
2005d4d8:	02040a23          	sb	zero,52(s0)
2005d4dc:	b7a1                	c.j	2005d424 <prvInitialiseNewTask.constprop.0+0x68>

2005d4de <vTaskSuspendAll>:
2005d4de:	f7c18793          	addi	a5,gp,-132 # 200661f8 <uxSchedulerSuspended>
2005d4e2:	4398                	c.lw	a4,0(a5)
2005d4e4:	0705                	c.addi	a4,1
2005d4e6:	c398                	c.sw	a4,0(a5)
2005d4e8:	8082                	c.jr	ra

2005d4ea <xTaskGetTickCountFromISR>:
2005d4ea:	f981a503          	lw	a0,-104(gp) # 20066214 <xTickCount>
2005d4ee:	8082                	c.jr	ra

2005d4f0 <uxTaskGetNumberOfTasks>:
2005d4f0:	f741a503          	lw	a0,-140(gp) # 200661f0 <uxCurrentNumberOfTasks>
2005d4f4:	8082                	c.jr	ra

2005d4f6 <pcTaskGetName>:
2005d4f6:	e119                	c.bnez	a0,2005d4fc <pcTaskGetName+0x6>
2005d4f8:	f601a503          	lw	a0,-160(gp) # 200661dc <pxCurrentTCB>
2005d4fc:	03450513          	addi	a0,a0,52
2005d500:	8082                	c.jr	ra

2005d502 <xTaskIncrementTick>:
2005d502:	f7c1a783          	lw	a5,-132(gp) # 200661f8 <uxSchedulerSuspended>
2005d506:	7179                	c.addi16sp	sp,-48
2005d508:	d606                	c.swsp	ra,44(sp)
2005d50a:	d422                	c.swsp	s0,40(sp)
2005d50c:	d226                	c.swsp	s1,36(sp)
2005d50e:	d04a                	c.swsp	s2,32(sp)
2005d510:	ce4e                	c.swsp	s3,28(sp)
2005d512:	cc52                	c.swsp	s4,24(sp)
2005d514:	ca56                	c.swsp	s5,20(sp)
2005d516:	c85a                	c.swsp	s6,16(sp)
2005d518:	c65e                	c.swsp	s7,12(sp)
2005d51a:	c462                	c.swsp	s8,8(sp)
2005d51c:	c266                	c.swsp	s9,4(sp)
2005d51e:	c06a                	c.swsp	s10,0(sp)
2005d520:	10079363          	bne	a5,zero,2005d626 <xTaskIncrementTick+0x124>
2005d524:	f9818793          	addi	a5,gp,-104 # 20066214 <xTickCount>
2005d528:	0007aa03          	lw	s4,0(a5)
2005d52c:	0a05                	c.addi	s4,1
2005d52e:	0147a023          	sw	s4,0(a5)
2005d532:	020a1163          	bne	s4,zero,2005d554 <xTaskIncrementTick+0x52>
2005d536:	f6418713          	addi	a4,gp,-156 # 200661e0 <pxDelayedTaskList>
2005d53a:	f6818793          	addi	a5,gp,-152 # 200661e4 <pxOverflowDelayedTaskList>
2005d53e:	4314                	c.lw	a3,0(a4)
2005d540:	4390                	c.lw	a2,0(a5)
2005d542:	c310                	c.sw	a2,0(a4)
2005d544:	c394                	c.sw	a3,0(a5)
2005d546:	f8c18793          	addi	a5,gp,-116 # 20066208 <xNumOfOverflows>
2005d54a:	4398                	c.lw	a4,0(a5)
2005d54c:	0705                	c.addi	a4,1
2005d54e:	c398                	c.sw	a4,0(a5)
2005d550:	e1bff0ef          	jal	ra,2005d36a <prvResetNextTaskUnblockTime>
2005d554:	f8818793          	addi	a5,gp,-120 # 20066204 <xNextTaskUnblockTime>
2005d558:	439c                	c.lw	a5,0(a5)
2005d55a:	200684b7          	lui	s1,0x20068
2005d55e:	f8818993          	addi	s3,gp,-120 # 20066204 <xNextTaskUnblockTime>
2005d562:	ba848493          	addi	s1,s1,-1112 # 20067ba8 <pxReadyTasksLists>
2005d566:	f6018a93          	addi	s5,gp,-160 # 200661dc <pxCurrentTCB>
2005d56a:	4401                	c.li	s0,0
2005d56c:	04fa7663          	bgeu	s4,a5,2005d5b8 <xTaskIncrementTick+0xb6>
2005d570:	000aa783          	lw	a5,0(s5)
2005d574:	4751                	c.li	a4,20
2005d576:	57dc                	c.lw	a5,44(a5)
2005d578:	02e787b3          	mul	a5,a5,a4
2005d57c:	94be                	c.add	s1,a5
2005d57e:	4098                	c.lw	a4,0(s1)
2005d580:	4785                	c.li	a5,1
2005d582:	00e7f363          	bgeu	a5,a4,2005d588 <xTaskIncrementTick+0x86>
2005d586:	4405                	c.li	s0,1
2005d588:	f901a783          	lw	a5,-112(gp) # 2006620c <xPendedTicks>
2005d58c:	e399                	c.bnez	a5,2005d592 <xTaskIncrementTick+0x90>
2005d58e:	e77fe0ef          	jal	ra,2005c404 <vApplicationTickHook>
2005d592:	f9c1a783          	lw	a5,-100(gp) # 20066218 <xYieldPending>
2005d596:	c391                	c.beqz	a5,2005d59a <xTaskIncrementTick+0x98>
2005d598:	4405                	c.li	s0,1
2005d59a:	50b2                	c.lwsp	ra,44(sp)
2005d59c:	8522                	c.mv	a0,s0
2005d59e:	5422                	c.lwsp	s0,40(sp)
2005d5a0:	5492                	c.lwsp	s1,36(sp)
2005d5a2:	5902                	c.lwsp	s2,32(sp)
2005d5a4:	49f2                	c.lwsp	s3,28(sp)
2005d5a6:	4a62                	c.lwsp	s4,24(sp)
2005d5a8:	4ad2                	c.lwsp	s5,20(sp)
2005d5aa:	4b42                	c.lwsp	s6,16(sp)
2005d5ac:	4bb2                	c.lwsp	s7,12(sp)
2005d5ae:	4c22                	c.lwsp	s8,8(sp)
2005d5b0:	4c92                	c.lwsp	s9,4(sp)
2005d5b2:	4d02                	c.lwsp	s10,0(sp)
2005d5b4:	6145                	c.addi16sp	sp,48
2005d5b6:	8082                	c.jr	ra
2005d5b8:	4c05                	c.li	s8,1
2005d5ba:	4cd1                	c.li	s9,20
2005d5bc:	f6418793          	addi	a5,gp,-156 # 200661e0 <pxDelayedTaskList>
2005d5c0:	4398                	c.lw	a4,0(a5)
2005d5c2:	4318                	c.lw	a4,0(a4)
2005d5c4:	e709                	c.bnez	a4,2005d5ce <xTaskIncrementTick+0xcc>
2005d5c6:	57fd                	c.li	a5,-1
2005d5c8:	00f9a023          	sw	a5,0(s3)
2005d5cc:	b755                	c.j	2005d570 <xTaskIncrementTick+0x6e>
2005d5ce:	439c                	c.lw	a5,0(a5)
2005d5d0:	47dc                	c.lw	a5,12(a5)
2005d5d2:	00c7a903          	lw	s2,12(a5)
2005d5d6:	00492783          	lw	a5,4(s2)
2005d5da:	fefa67e3          	bltu	s4,a5,2005d5c8 <xTaskIncrementTick+0xc6>
2005d5de:	00490d13          	addi	s10,s2,4
2005d5e2:	856a                	c.mv	a0,s10
2005d5e4:	e00fd0ef          	jal	ra,2005abe4 <uxListRemove>
2005d5e8:	02892783          	lw	a5,40(s2)
2005d5ec:	c789                	c.beqz	a5,2005d5f6 <xTaskIncrementTick+0xf4>
2005d5ee:	01890513          	addi	a0,s2,24
2005d5f2:	df2fd0ef          	jal	ra,2005abe4 <uxListRemove>
2005d5f6:	02c92503          	lw	a0,44(s2)
2005d5fa:	f8418713          	addi	a4,gp,-124 # 20066200 <uxTopReadyPriority>
2005d5fe:	4314                	c.lw	a3,0(a4)
2005d600:	00ac17b3          	sll	a5,s8,a0
2005d604:	03950533          	mul	a0,a0,s9
2005d608:	8fd5                	c.or	a5,a3
2005d60a:	85ea                	c.mv	a1,s10
2005d60c:	c31c                	c.sw	a5,0(a4)
2005d60e:	9526                	c.add	a0,s1
2005d610:	d90fd0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005d614:	000aa783          	lw	a5,0(s5)
2005d618:	02c92703          	lw	a4,44(s2)
2005d61c:	57dc                	c.lw	a5,44(a5)
2005d61e:	f8f76fe3          	bltu	a4,a5,2005d5bc <xTaskIncrementTick+0xba>
2005d622:	4405                	c.li	s0,1
2005d624:	bf61                	c.j	2005d5bc <xTaskIncrementTick+0xba>
2005d626:	f9018793          	addi	a5,gp,-112 # 2006620c <xPendedTicks>
2005d62a:	4398                	c.lw	a4,0(a5)
2005d62c:	4401                	c.li	s0,0
2005d62e:	0705                	c.addi	a4,1
2005d630:	c398                	c.sw	a4,0(a5)
2005d632:	dd3fe0ef          	jal	ra,2005c404 <vApplicationTickHook>
2005d636:	b795                	c.j	2005d59a <xTaskIncrementTick+0x98>

2005d638 <vTaskSwitchContext>:
2005d638:	f7c1a703          	lw	a4,-132(gp) # 200661f8 <uxSchedulerSuspended>
2005d63c:	f9c18793          	addi	a5,gp,-100 # 20066218 <xYieldPending>
2005d640:	c701                	c.beqz	a4,2005d648 <vTaskSwitchContext+0x10>
2005d642:	4705                	c.li	a4,1
2005d644:	c398                	c.sw	a4,0(a5)
2005d646:	8082                	c.jr	ra
2005d648:	1141                	c.addi	sp,-16
2005d64a:	c422                	c.swsp	s0,8(sp)
2005d64c:	c606                	c.swsp	ra,12(sp)
2005d64e:	0007a023          	sw	zero,0(a5)
2005d652:	410187b7          	lui	a5,0x41018
2005d656:	4bd4                	c.lw	a3,20(a5)
2005d658:	f7018713          	addi	a4,gp,-144 # 200661ec <ulTotalRunTime>
2005d65c:	c314                	c.sw	a3,0(a4)
2005d65e:	f6c1a583          	lw	a1,-148(gp) # 200661e8 <ulTaskSwitchedInTime>
2005d662:	4318                	c.lw	a4,0(a4)
2005d664:	f7018793          	addi	a5,gp,-144 # 200661ec <ulTotalRunTime>
2005d668:	f6c18693          	addi	a3,gp,-148 # 200661e8 <ulTaskSwitchedInTime>
2005d66c:	f6018413          	addi	s0,gp,-160 # 200661dc <pxCurrentTCB>
2005d670:	00e5f863          	bgeu	a1,a4,2005d680 <vTaskSwitchContext+0x48>
2005d674:	4388                	c.lw	a0,0(a5)
2005d676:	4010                	c.lw	a2,0(s0)
2005d678:	5638                	c.lw	a4,104(a2)
2005d67a:	8f0d                	c.sub	a4,a1
2005d67c:	972a                	c.add	a4,a0
2005d67e:	d638                	c.sw	a4,104(a2)
2005d680:	439c                	c.lw	a5,0(a5)
2005d682:	a5a5a737          	lui	a4,0xa5a5a
2005d686:	5a570713          	addi	a4,a4,1445 # a5a5a5a5 <__ctrace_end__+0xdba5a5a5>
2005d68a:	c29c                	c.sw	a5,0(a3)
2005d68c:	401c                	c.lw	a5,0(s0)
2005d68e:	5b9c                	c.lw	a5,48(a5)
2005d690:	4390                	c.lw	a2,0(a5)
2005d692:	00e61b63          	bne	a2,a4,2005d6a8 <vTaskSwitchContext+0x70>
2005d696:	43d4                	c.lw	a3,4(a5)
2005d698:	00c69863          	bne	a3,a2,2005d6a8 <vTaskSwitchContext+0x70>
2005d69c:	4798                	c.lw	a4,8(a5)
2005d69e:	00d71563          	bne	a4,a3,2005d6a8 <vTaskSwitchContext+0x70>
2005d6a2:	47dc                	c.lw	a5,12(a5)
2005d6a4:	00e78863          	beq	a5,a4,2005d6b4 <vTaskSwitchContext+0x7c>
2005d6a8:	4008                	c.lw	a0,0(s0)
2005d6aa:	400c                	c.lw	a1,0(s0)
2005d6ac:	03458593          	addi	a1,a1,52
2005d6b0:	dadfe0ef          	jal	ra,2005c45c <vApplicationStackOverflowHook>
2005d6b4:	f841a503          	lw	a0,-124(gp) # 20066200 <uxTopReadyPriority>
2005d6b8:	035030ef          	jal	ra,20060eec <__clzsi2>
2005d6bc:	47fd                	c.li	a5,31
2005d6be:	40a78533          	sub	a0,a5,a0
2005d6c2:	4751                	c.li	a4,20
2005d6c4:	02e50733          	mul	a4,a0,a4
2005d6c8:	200687b7          	lui	a5,0x20068
2005d6cc:	ba878693          	addi	a3,a5,-1112 # 20067ba8 <pxReadyTasksLists>
2005d6d0:	ba878793          	addi	a5,a5,-1112
2005d6d4:	00e68633          	add	a2,a3,a4
2005d6d8:	424c                	c.lw	a1,4(a2)
2005d6da:	0721                	c.addi	a4,8
2005d6dc:	9736                	c.add	a4,a3
2005d6de:	41cc                	c.lw	a1,4(a1)
2005d6e0:	c24c                	c.sw	a1,4(a2)
2005d6e2:	00e59463          	bne	a1,a4,2005d6ea <vTaskSwitchContext+0xb2>
2005d6e6:	41d8                	c.lw	a4,4(a1)
2005d6e8:	c258                	c.sw	a4,4(a2)
2005d6ea:	4751                	c.li	a4,20
2005d6ec:	02e50533          	mul	a0,a0,a4
2005d6f0:	40b2                	c.lwsp	ra,12(sp)
2005d6f2:	97aa                	c.add	a5,a0
2005d6f4:	43dc                	c.lw	a5,4(a5)
2005d6f6:	47dc                	c.lw	a5,12(a5)
2005d6f8:	c01c                	c.sw	a5,0(s0)
2005d6fa:	401c                	c.lw	a5,0(s0)
2005d6fc:	4422                	c.lwsp	s0,8(sp)
2005d6fe:	06c78793          	addi	a5,a5,108
2005d702:	eaf1a023          	sw	a5,-352(gp) # 2006611c <_impure_ptr>
2005d706:	0141                	c.addi	sp,16
2005d708:	8082                	c.jr	ra

2005d70a <vTaskPlaceOnEventList>:
2005d70a:	1141                	c.addi	sp,-16
2005d70c:	c422                	c.swsp	s0,8(sp)
2005d70e:	842e                	c.mv	s0,a1
2005d710:	f601a583          	lw	a1,-160(gp) # 200661dc <pxCurrentTCB>
2005d714:	c606                	c.swsp	ra,12(sp)
2005d716:	05e1                	c.addi	a1,24
2005d718:	c9efd0ef          	jal	ra,2005abb6 <vListInsert>
2005d71c:	8522                	c.mv	a0,s0
2005d71e:	4422                	c.lwsp	s0,8(sp)
2005d720:	40b2                	c.lwsp	ra,12(sp)
2005d722:	4585                	c.li	a1,1
2005d724:	0141                	c.addi	sp,16
2005d726:	b21ff06f          	jal	zero,2005d246 <prvAddCurrentTaskToDelayedList>

2005d72a <vTaskPlaceOnEventListRestricted>:
2005d72a:	1141                	c.addi	sp,-16
2005d72c:	c422                	c.swsp	s0,8(sp)
2005d72e:	842e                	c.mv	s0,a1
2005d730:	f601a583          	lw	a1,-160(gp) # 200661dc <pxCurrentTCB>
2005d734:	c226                	c.swsp	s1,4(sp)
2005d736:	84b2                	c.mv	s1,a2
2005d738:	05e1                	c.addi	a1,24
2005d73a:	c606                	c.swsp	ra,12(sp)
2005d73c:	c64fd0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005d740:	c091                	c.beqz	s1,2005d744 <vTaskPlaceOnEventListRestricted+0x1a>
2005d742:	547d                	c.li	s0,-1
2005d744:	8522                	c.mv	a0,s0
2005d746:	4422                	c.lwsp	s0,8(sp)
2005d748:	40b2                	c.lwsp	ra,12(sp)
2005d74a:	85a6                	c.mv	a1,s1
2005d74c:	4492                	c.lwsp	s1,4(sp)
2005d74e:	0141                	c.addi	sp,16
2005d750:	af7ff06f          	jal	zero,2005d246 <prvAddCurrentTaskToDelayedList>

2005d754 <xTaskRemoveFromEventList>:
2005d754:	1101                	c.addi	sp,-32
2005d756:	ce06                	c.swsp	ra,28(sp)
2005d758:	cc22                	c.swsp	s0,24(sp)
2005d75a:	455c                	c.lw	a5,12(a0)
2005d75c:	47c0                	c.lw	s0,12(a5)
2005d75e:	01840593          	addi	a1,s0,24
2005d762:	852e                	c.mv	a0,a1
2005d764:	c62e                	c.swsp	a1,12(sp)
2005d766:	c7efd0ef          	jal	ra,2005abe4 <uxListRemove>
2005d76a:	f7c1a783          	lw	a5,-132(gp) # 200661f8 <uxSchedulerSuspended>
2005d76e:	45b2                	c.lwsp	a1,12(sp)
2005d770:	efa1                	c.bnez	a5,2005d7c8 <xTaskRemoveFromEventList+0x74>
2005d772:	00440593          	addi	a1,s0,4
2005d776:	852e                	c.mv	a0,a1
2005d778:	c62e                	c.swsp	a1,12(sp)
2005d77a:	c6afd0ef          	jal	ra,2005abe4 <uxListRemove>
2005d77e:	f8418713          	addi	a4,gp,-124 # 20066200 <uxTopReadyPriority>
2005d782:	5448                	c.lw	a0,44(s0)
2005d784:	4314                	c.lw	a3,0(a4)
2005d786:	4785                	c.li	a5,1
2005d788:	00a797b3          	sll	a5,a5,a0
2005d78c:	8fd5                	c.or	a5,a3
2005d78e:	c31c                	c.sw	a5,0(a4)
2005d790:	47d1                	c.li	a5,20
2005d792:	02f50533          	mul	a0,a0,a5
2005d796:	45b2                	c.lwsp	a1,12(sp)
2005d798:	200687b7          	lui	a5,0x20068
2005d79c:	ba878793          	addi	a5,a5,-1112 # 20067ba8 <pxReadyTasksLists>
2005d7a0:	953e                	c.add	a0,a5
2005d7a2:	bfefd0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005d7a6:	bc5ff0ef          	jal	ra,2005d36a <prvResetNextTaskUnblockTime>
2005d7aa:	f601a783          	lw	a5,-160(gp) # 200661dc <pxCurrentTCB>
2005d7ae:	5458                	c.lw	a4,44(s0)
2005d7b0:	4501                	c.li	a0,0
2005d7b2:	57dc                	c.lw	a5,44(a5)
2005d7b4:	00e7f663          	bgeu	a5,a4,2005d7c0 <xTaskRemoveFromEventList+0x6c>
2005d7b8:	4705                	c.li	a4,1
2005d7ba:	f8e1ae23          	sw	a4,-100(gp) # 20066218 <xYieldPending>
2005d7be:	4505                	c.li	a0,1
2005d7c0:	40f2                	c.lwsp	ra,28(sp)
2005d7c2:	4462                	c.lwsp	s0,24(sp)
2005d7c4:	6105                	c.addi16sp	sp,32
2005d7c6:	8082                	c.jr	ra
2005d7c8:	20068537          	lui	a0,0x20068
2005d7cc:	cac50513          	addi	a0,a0,-852 # 20067cac <xPendingReadyList>
2005d7d0:	bd0fd0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005d7d4:	bfd9                	c.j	2005d7aa <xTaskRemoveFromEventList+0x56>

2005d7d6 <vTaskInternalSetTimeOutState>:
2005d7d6:	f8c1a783          	lw	a5,-116(gp) # 20066208 <xNumOfOverflows>
2005d7da:	c11c                	c.sw	a5,0(a0)
2005d7dc:	f981a783          	lw	a5,-104(gp) # 20066214 <xTickCount>
2005d7e0:	c15c                	c.sw	a5,4(a0)
2005d7e2:	8082                	c.jr	ra

2005d7e4 <vTaskMissedYield>:
2005d7e4:	4705                	c.li	a4,1
2005d7e6:	f8e1ae23          	sw	a4,-100(gp) # 20066218 <xYieldPending>
2005d7ea:	8082                	c.jr	ra

2005d7ec <eTaskConfirmSleepModeStatus>:
2005d7ec:	200687b7          	lui	a5,0x20068
2005d7f0:	cac7a783          	lw	a5,-852(a5) # 20067cac <xPendingReadyList>
2005d7f4:	4501                	c.li	a0,0
2005d7f6:	e395                	c.bnez	a5,2005d81a <eTaskConfirmSleepModeStatus+0x2e>
2005d7f8:	f9c1a783          	lw	a5,-100(gp) # 20066218 <xYieldPending>
2005d7fc:	ef99                	c.bnez	a5,2005d81a <eTaskConfirmSleepModeStatus+0x2e>
2005d7fe:	f901a783          	lw	a5,-112(gp) # 2006620c <xPendedTicks>
2005d802:	ef81                	c.bnez	a5,2005d81a <eTaskConfirmSleepModeStatus+0x2e>
2005d804:	200687b7          	lui	a5,0x20068
2005d808:	cc07a703          	lw	a4,-832(a5) # 20067cc0 <xSuspendedTaskList>
2005d80c:	f741a783          	lw	a5,-140(gp) # 200661f0 <uxCurrentNumberOfTasks>
2005d810:	4505                	c.li	a0,1
2005d812:	17fd                	c.addi	a5,-1
2005d814:	00f71363          	bne	a4,a5,2005d81a <eTaskConfirmSleepModeStatus+0x2e>
2005d818:	4509                	c.li	a0,2
2005d81a:	8082                	c.jr	ra

2005d81c <xTaskGetCurrentTaskHandle>:
2005d81c:	f601a503          	lw	a0,-160(gp) # 200661dc <pxCurrentTCB>
2005d820:	8082                	c.jr	ra

2005d822 <xTaskGetSchedulerState>:
2005d822:	f941a783          	lw	a5,-108(gp) # 20066210 <xSchedulerRunning>
2005d826:	4505                	c.li	a0,1
2005d828:	c791                	c.beqz	a5,2005d834 <xTaskGetSchedulerState+0x12>
2005d82a:	f7c1a503          	lw	a0,-132(gp) # 200661f8 <uxSchedulerSuspended>
2005d82e:	00153513          	sltiu	a0,a0,1
2005d832:	0506                	c.slli	a0,0x1
2005d834:	8082                	c.jr	ra

2005d836 <xTaskPriorityInherit>:
2005d836:	c945                	c.beqz	a0,2005d8e6 <xTaskPriorityInherit+0xb0>
2005d838:	1101                	c.addi	sp,-32
2005d83a:	ca26                	c.swsp	s1,20(sp)
2005d83c:	cc22                	c.swsp	s0,24(sp)
2005d83e:	ce06                	c.swsp	ra,28(sp)
2005d840:	c84a                	c.swsp	s2,16(sp)
2005d842:	f6018713          	addi	a4,gp,-160 # 200661dc <pxCurrentTCB>
2005d846:	4314                	c.lw	a3,0(a4)
2005d848:	555c                	c.lw	a5,44(a0)
2005d84a:	842a                	c.mv	s0,a0
2005d84c:	56d4                	c.lw	a3,44(a3)
2005d84e:	f6018493          	addi	s1,gp,-160 # 200661dc <pxCurrentTCB>
2005d852:	08d7f463          	bgeu	a5,a3,2005d8da <xTaskPriorityInherit+0xa4>
2005d856:	4d18                	c.lw	a4,24(a0)
2005d858:	00074763          	blt	a4,zero,2005d866 <xTaskPriorityInherit+0x30>
2005d85c:	4098                	c.lw	a4,0(s1)
2005d85e:	5754                	c.lw	a3,44(a4)
2005d860:	472d                	c.li	a4,11
2005d862:	8f15                	c.sub	a4,a3
2005d864:	cd18                	c.sw	a4,24(a0)
2005d866:	46d1                	c.li	a3,20
2005d868:	02d787b3          	mul	a5,a5,a3
2005d86c:	20068537          	lui	a0,0x20068
2005d870:	ba850713          	addi	a4,a0,-1112 # 20067ba8 <pxReadyTasksLists>
2005d874:	ba850913          	addi	s2,a0,-1112
2005d878:	97ba                	c.add	a5,a4
2005d87a:	4858                	c.lw	a4,20(s0)
2005d87c:	04f71b63          	bne	a4,a5,2005d8d2 <xTaskPriorityInherit+0x9c>
2005d880:	00440593          	addi	a1,s0,4
2005d884:	852e                	c.mv	a0,a1
2005d886:	c62e                	c.swsp	a1,12(sp)
2005d888:	b5cfd0ef          	jal	ra,2005abe4 <uxListRemove>
2005d88c:	45b2                	c.lwsp	a1,12(sp)
2005d88e:	f8418713          	addi	a4,gp,-124 # 20066200 <uxTopReadyPriority>
2005d892:	e911                	c.bnez	a0,2005d8a6 <xTaskPriorityInherit+0x70>
2005d894:	5450                	c.lw	a2,44(s0)
2005d896:	4314                	c.lw	a3,0(a4)
2005d898:	4785                	c.li	a5,1
2005d89a:	00c797b3          	sll	a5,a5,a2
2005d89e:	fff7c793          	xori	a5,a5,-1
2005d8a2:	8ff5                	c.and	a5,a3
2005d8a4:	c31c                	c.sw	a5,0(a4)
2005d8a6:	409c                	c.lw	a5,0(s1)
2005d8a8:	4314                	c.lw	a3,0(a4)
2005d8aa:	57c8                	c.lw	a0,44(a5)
2005d8ac:	4785                	c.li	a5,1
2005d8ae:	00a797b3          	sll	a5,a5,a0
2005d8b2:	8fd5                	c.or	a5,a3
2005d8b4:	c31c                	c.sw	a5,0(a4)
2005d8b6:	47d1                	c.li	a5,20
2005d8b8:	d448                	c.sw	a0,44(s0)
2005d8ba:	02f50533          	mul	a0,a0,a5
2005d8be:	954a                	c.add	a0,s2
2005d8c0:	ae0fd0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005d8c4:	4505                	c.li	a0,1
2005d8c6:	40f2                	c.lwsp	ra,28(sp)
2005d8c8:	4462                	c.lwsp	s0,24(sp)
2005d8ca:	44d2                	c.lwsp	s1,20(sp)
2005d8cc:	4942                	c.lwsp	s2,16(sp)
2005d8ce:	6105                	c.addi16sp	sp,32
2005d8d0:	8082                	c.jr	ra
2005d8d2:	409c                	c.lw	a5,0(s1)
2005d8d4:	57dc                	c.lw	a5,44(a5)
2005d8d6:	d45c                	c.sw	a5,44(s0)
2005d8d8:	b7f5                	c.j	2005d8c4 <xTaskPriorityInherit+0x8e>
2005d8da:	431c                	c.lw	a5,0(a4)
2005d8dc:	4d68                	c.lw	a0,92(a0)
2005d8de:	57dc                	c.lw	a5,44(a5)
2005d8e0:	00f53533          	sltu	a0,a0,a5
2005d8e4:	b7cd                	c.j	2005d8c6 <xTaskPriorityInherit+0x90>
2005d8e6:	4501                	c.li	a0,0
2005d8e8:	8082                	c.jr	ra

2005d8ea <xTaskPriorityDisinherit>:
2005d8ea:	e901                	c.bnez	a0,2005d8fa <xTaskPriorityDisinherit+0x10>
2005d8ec:	4501                	c.li	a0,0
2005d8ee:	8082                	c.jr	ra
2005d8f0:	4501                	c.li	a0,0
2005d8f2:	40f2                	c.lwsp	ra,28(sp)
2005d8f4:	4462                	c.lwsp	s0,24(sp)
2005d8f6:	6105                	c.addi16sp	sp,32
2005d8f8:	8082                	c.jr	ra
2005d8fa:	1101                	c.addi	sp,-32
2005d8fc:	cc22                	c.swsp	s0,24(sp)
2005d8fe:	ce06                	c.swsp	ra,28(sp)
2005d900:	513c                	c.lw	a5,96(a0)
2005d902:	5554                	c.lw	a3,44(a0)
2005d904:	4d78                	c.lw	a4,92(a0)
2005d906:	17fd                	c.addi	a5,-1
2005d908:	d13c                	c.sw	a5,96(a0)
2005d90a:	842a                	c.mv	s0,a0
2005d90c:	fee682e3          	beq	a3,a4,2005d8f0 <xTaskPriorityDisinherit+0x6>
2005d910:	f3e5                	c.bnez	a5,2005d8f0 <xTaskPriorityDisinherit+0x6>
2005d912:	00450593          	addi	a1,a0,4
2005d916:	852e                	c.mv	a0,a1
2005d918:	c62e                	c.swsp	a1,12(sp)
2005d91a:	acafd0ef          	jal	ra,2005abe4 <uxListRemove>
2005d91e:	45b2                	c.lwsp	a1,12(sp)
2005d920:	f8418713          	addi	a4,gp,-124 # 20066200 <uxTopReadyPriority>
2005d924:	e911                	c.bnez	a0,2005d938 <xTaskPriorityDisinherit+0x4e>
2005d926:	5450                	c.lw	a2,44(s0)
2005d928:	4314                	c.lw	a3,0(a4)
2005d92a:	4785                	c.li	a5,1
2005d92c:	00c797b3          	sll	a5,a5,a2
2005d930:	fff7c793          	xori	a5,a5,-1
2005d934:	8ff5                	c.and	a5,a3
2005d936:	c31c                	c.sw	a5,0(a4)
2005d938:	4c68                	c.lw	a0,92(s0)
2005d93a:	47ad                	c.li	a5,11
2005d93c:	4314                	c.lw	a3,0(a4)
2005d93e:	8f89                	c.sub	a5,a0
2005d940:	cc1c                	c.sw	a5,24(s0)
2005d942:	4785                	c.li	a5,1
2005d944:	00a797b3          	sll	a5,a5,a0
2005d948:	8fd5                	c.or	a5,a3
2005d94a:	c31c                	c.sw	a5,0(a4)
2005d94c:	47d1                	c.li	a5,20
2005d94e:	d448                	c.sw	a0,44(s0)
2005d950:	02f50533          	mul	a0,a0,a5
2005d954:	200687b7          	lui	a5,0x20068
2005d958:	ba878793          	addi	a5,a5,-1112 # 20067ba8 <pxReadyTasksLists>
2005d95c:	953e                	c.add	a0,a5
2005d95e:	a42fd0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005d962:	4505                	c.li	a0,1
2005d964:	b779                	c.j	2005d8f2 <xTaskPriorityDisinherit+0x8>

2005d966 <vTaskPriorityDisinheritAfterTimeout>:
2005d966:	cd51                	c.beqz	a0,2005da02 <vTaskPriorityDisinheritAfterTimeout+0x9c>
2005d968:	1101                	c.addi	sp,-32
2005d96a:	cc22                	c.swsp	s0,24(sp)
2005d96c:	ce06                	c.swsp	ra,28(sp)
2005d96e:	ca26                	c.swsp	s1,20(sp)
2005d970:	4d7c                	c.lw	a5,92(a0)
2005d972:	842a                	c.mv	s0,a0
2005d974:	00b7f363          	bgeu	a5,a1,2005d97a <vTaskPriorityDisinheritAfterTimeout+0x14>
2005d978:	87ae                	c.mv	a5,a1
2005d97a:	5458                	c.lw	a4,44(s0)
2005d97c:	06f70e63          	beq	a4,a5,2005d9f8 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005d980:	5030                	c.lw	a2,96(s0)
2005d982:	4685                	c.li	a3,1
2005d984:	06d61a63          	bne	a2,a3,2005d9f8 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005d988:	4c14                	c.lw	a3,24(s0)
2005d98a:	d45c                	c.sw	a5,44(s0)
2005d98c:	0006c663          	blt	a3,zero,2005d998 <vTaskPriorityDisinheritAfterTimeout+0x32>
2005d990:	46ad                	c.li	a3,11
2005d992:	40f687b3          	sub	a5,a3,a5
2005d996:	cc1c                	c.sw	a5,24(s0)
2005d998:	46d1                	c.li	a3,20
2005d99a:	02d70733          	mul	a4,a4,a3
2005d99e:	20068537          	lui	a0,0x20068
2005d9a2:	ba850793          	addi	a5,a0,-1112 # 20067ba8 <pxReadyTasksLists>
2005d9a6:	ba850493          	addi	s1,a0,-1112
2005d9aa:	973e                	c.add	a4,a5
2005d9ac:	485c                	c.lw	a5,20(s0)
2005d9ae:	04e79563          	bne	a5,a4,2005d9f8 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005d9b2:	00440593          	addi	a1,s0,4
2005d9b6:	852e                	c.mv	a0,a1
2005d9b8:	c62e                	c.swsp	a1,12(sp)
2005d9ba:	a2afd0ef          	jal	ra,2005abe4 <uxListRemove>
2005d9be:	5454                	c.lw	a3,44(s0)
2005d9c0:	45b2                	c.lwsp	a1,12(sp)
2005d9c2:	f8418713          	addi	a4,gp,-124 # 20066200 <uxTopReadyPriority>
2005d9c6:	e909                	c.bnez	a0,2005d9d8 <vTaskPriorityDisinheritAfterTimeout+0x72>
2005d9c8:	4310                	c.lw	a2,0(a4)
2005d9ca:	4785                	c.li	a5,1
2005d9cc:	00d797b3          	sll	a5,a5,a3
2005d9d0:	fff7c793          	xori	a5,a5,-1
2005d9d4:	8ff1                	c.and	a5,a2
2005d9d6:	c31c                	c.sw	a5,0(a4)
2005d9d8:	4551                	c.li	a0,20
2005d9da:	02a68533          	mul	a0,a3,a0
2005d9de:	4310                	c.lw	a2,0(a4)
2005d9e0:	4785                	c.li	a5,1
2005d9e2:	00d797b3          	sll	a5,a5,a3
2005d9e6:	8fd1                	c.or	a5,a2
2005d9e8:	c31c                	c.sw	a5,0(a4)
2005d9ea:	4462                	c.lwsp	s0,24(sp)
2005d9ec:	40f2                	c.lwsp	ra,28(sp)
2005d9ee:	9526                	c.add	a0,s1
2005d9f0:	44d2                	c.lwsp	s1,20(sp)
2005d9f2:	6105                	c.addi16sp	sp,32
2005d9f4:	9acfd06f          	jal	zero,2005aba0 <vListInsertEnd>
2005d9f8:	40f2                	c.lwsp	ra,28(sp)
2005d9fa:	4462                	c.lwsp	s0,24(sp)
2005d9fc:	44d2                	c.lwsp	s1,20(sp)
2005d9fe:	6105                	c.addi16sp	sp,32
2005da00:	8082                	c.jr	ra
2005da02:	8082                	c.jr	ra

2005da04 <vTaskEnterCritical>:
2005da04:	30047073          	csrrci	zero,mstatus,8
2005da08:	f941a783          	lw	a5,-108(gp) # 20066210 <xSchedulerRunning>
2005da0c:	cb81                	c.beqz	a5,2005da1c <vTaskEnterCritical+0x18>
2005da0e:	f6018793          	addi	a5,gp,-160 # 200661dc <pxCurrentTCB>
2005da12:	4394                	c.lw	a3,0(a5)
2005da14:	439c                	c.lw	a5,0(a5)
2005da16:	4ab8                	c.lw	a4,80(a3)
2005da18:	0705                	c.addi	a4,1
2005da1a:	cab8                	c.sw	a4,80(a3)
2005da1c:	8082                	c.jr	ra

2005da1e <vTaskExitCritical>:
2005da1e:	f941a783          	lw	a5,-108(gp) # 20066210 <xSchedulerRunning>
2005da22:	cf99                	c.beqz	a5,2005da40 <vTaskExitCritical+0x22>
2005da24:	f6018793          	addi	a5,gp,-160 # 200661dc <pxCurrentTCB>
2005da28:	4398                	c.lw	a4,0(a5)
2005da2a:	4b38                	c.lw	a4,80(a4)
2005da2c:	cb11                	c.beqz	a4,2005da40 <vTaskExitCritical+0x22>
2005da2e:	4394                	c.lw	a3,0(a5)
2005da30:	439c                	c.lw	a5,0(a5)
2005da32:	4ab8                	c.lw	a4,80(a3)
2005da34:	177d                	c.addi	a4,-1
2005da36:	cab8                	c.sw	a4,80(a3)
2005da38:	4bbc                	c.lw	a5,80(a5)
2005da3a:	e399                	c.bnez	a5,2005da40 <vTaskExitCritical+0x22>
2005da3c:	30046073          	csrrsi	zero,mstatus,8
2005da40:	8082                	c.jr	ra

2005da42 <prvAddNewTaskToReadyList>:
2005da42:	1101                	c.addi	sp,-32
2005da44:	cc22                	c.swsp	s0,24(sp)
2005da46:	ca26                	c.swsp	s1,20(sp)
2005da48:	c84a                	c.swsp	s2,16(sp)
2005da4a:	c64e                	c.swsp	s3,12(sp)
2005da4c:	ce06                	c.swsp	ra,28(sp)
2005da4e:	c452                	c.swsp	s4,8(sp)
2005da50:	842a                	c.mv	s0,a0
2005da52:	fb3ff0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005da56:	f7418793          	addi	a5,gp,-140 # 200661f0 <uxCurrentNumberOfTasks>
2005da5a:	4398                	c.lw	a4,0(a5)
2005da5c:	20068937          	lui	s2,0x20068
2005da60:	0705                	c.addi	a4,1
2005da62:	c398                	c.sw	a4,0(a5)
2005da64:	f6018713          	addi	a4,gp,-160 # 200661dc <pxCurrentTCB>
2005da68:	4318                	c.lw	a4,0(a4)
2005da6a:	f6018493          	addi	s1,gp,-160 # 200661dc <pxCurrentTCB>
2005da6e:	ba890993          	addi	s3,s2,-1112 # 20067ba8 <pxReadyTasksLists>
2005da72:	e369                	c.bnez	a4,2005db34 <prvAddNewTaskToReadyList+0xf2>
2005da74:	c080                	c.sw	s0,0(s1)
2005da76:	4398                	c.lw	a4,0(a5)
2005da78:	4785                	c.li	a5,1
2005da7a:	06f71263          	bne	a4,a5,2005dade <prvAddNewTaskToReadyList+0x9c>
2005da7e:	ba890913          	addi	s2,s2,-1112
2005da82:	0dc98a13          	addi	s4,s3,220
2005da86:	854a                	c.mv	a0,s2
2005da88:	0951                	c.addi	s2,20
2005da8a:	8fcfd0ef          	jal	ra,2005ab86 <vListInitialise>
2005da8e:	ff2a1ce3          	bne	s4,s2,2005da86 <prvAddNewTaskToReadyList+0x44>
2005da92:	20068537          	lui	a0,0x20068
2005da96:	c8450a13          	addi	s4,a0,-892 # 20067c84 <xDelayedTaskList1>
2005da9a:	c8450513          	addi	a0,a0,-892
2005da9e:	8e8fd0ef          	jal	ra,2005ab86 <vListInitialise>
2005daa2:	20068537          	lui	a0,0x20068
2005daa6:	c9850913          	addi	s2,a0,-872 # 20067c98 <xDelayedTaskList2>
2005daaa:	c9850513          	addi	a0,a0,-872
2005daae:	8d8fd0ef          	jal	ra,2005ab86 <vListInitialise>
2005dab2:	20068537          	lui	a0,0x20068
2005dab6:	cac50513          	addi	a0,a0,-852 # 20067cac <xPendingReadyList>
2005daba:	8ccfd0ef          	jal	ra,2005ab86 <vListInitialise>
2005dabe:	20068537          	lui	a0,0x20068
2005dac2:	cd450513          	addi	a0,a0,-812 # 20067cd4 <xTasksWaitingTermination>
2005dac6:	8c0fd0ef          	jal	ra,2005ab86 <vListInitialise>
2005daca:	20068537          	lui	a0,0x20068
2005dace:	cc050513          	addi	a0,a0,-832 # 20067cc0 <xSuspendedTaskList>
2005dad2:	8b4fd0ef          	jal	ra,2005ab86 <vListInitialise>
2005dad6:	f741a223          	sw	s4,-156(gp) # 200661e0 <pxDelayedTaskList>
2005dada:	f721a423          	sw	s2,-152(gp) # 200661e4 <pxOverflowDelayedTaskList>
2005dade:	f8018713          	addi	a4,gp,-128 # 200661fc <uxTaskNumber>
2005dae2:	431c                	c.lw	a5,0(a4)
2005dae4:	5448                	c.lw	a0,44(s0)
2005dae6:	00440593          	addi	a1,s0,4
2005daea:	0785                	c.addi	a5,1
2005daec:	c31c                	c.sw	a5,0(a4)
2005daee:	f8418713          	addi	a4,gp,-124 # 20066200 <uxTopReadyPriority>
2005daf2:	4314                	c.lw	a3,0(a4)
2005daf4:	c87c                	c.sw	a5,84(s0)
2005daf6:	4785                	c.li	a5,1
2005daf8:	00a797b3          	sll	a5,a5,a0
2005dafc:	8fd5                	c.or	a5,a3
2005dafe:	c31c                	c.sw	a5,0(a4)
2005db00:	47d1                	c.li	a5,20
2005db02:	02f50533          	mul	a0,a0,a5
2005db06:	954e                	c.add	a0,s3
2005db08:	898fd0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005db0c:	f13ff0ef          	jal	ra,2005da1e <vTaskExitCritical>
2005db10:	f941a783          	lw	a5,-108(gp) # 20066210 <xSchedulerRunning>
2005db14:	cb81                	c.beqz	a5,2005db24 <prvAddNewTaskToReadyList+0xe2>
2005db16:	409c                	c.lw	a5,0(s1)
2005db18:	57d8                	c.lw	a4,44(a5)
2005db1a:	545c                	c.lw	a5,44(s0)
2005db1c:	00f77463          	bgeu	a4,a5,2005db24 <prvAddNewTaskToReadyList+0xe2>
2005db20:	00000073          	ecall
2005db24:	40f2                	c.lwsp	ra,28(sp)
2005db26:	4462                	c.lwsp	s0,24(sp)
2005db28:	44d2                	c.lwsp	s1,20(sp)
2005db2a:	4942                	c.lwsp	s2,16(sp)
2005db2c:	49b2                	c.lwsp	s3,12(sp)
2005db2e:	4a22                	c.lwsp	s4,8(sp)
2005db30:	6105                	c.addi16sp	sp,32
2005db32:	8082                	c.jr	ra
2005db34:	f941a783          	lw	a5,-108(gp) # 20066210 <xSchedulerRunning>
2005db38:	f3dd                	c.bnez	a5,2005dade <prvAddNewTaskToReadyList+0x9c>
2005db3a:	409c                	c.lw	a5,0(s1)
2005db3c:	57d8                	c.lw	a4,44(a5)
2005db3e:	545c                	c.lw	a5,44(s0)
2005db40:	f8e7efe3          	bltu	a5,a4,2005dade <prvAddNewTaskToReadyList+0x9c>
2005db44:	c080                	c.sw	s0,0(s1)
2005db46:	bf61                	c.j	2005dade <prvAddNewTaskToReadyList+0x9c>

2005db48 <xTaskCreateStatic>:
2005db48:	1101                	c.addi	sp,-32
2005db4a:	cc22                	c.swsp	s0,24(sp)
2005db4c:	ce06                	c.swsp	ra,28(sp)
2005db4e:	8442                	c.mv	s0,a6
2005db50:	00080f63          	beq	a6,zero,2005db6e <xTaskCreateStatic+0x26>
2005db54:	c395                	c.beqz	a5,2005db78 <xTaskCreateStatic+0x30>
2005db56:	02f82823          	sw	a5,48(a6)
2005db5a:	4789                	c.li	a5,2
2005db5c:	16f800a3          	sb	a5,353(a6)
2005db60:	007c                	c.addi4spn	a5,sp,12
2005db62:	85bff0ef          	jal	ra,2005d3bc <prvInitialiseNewTask.constprop.0>
2005db66:	8522                	c.mv	a0,s0
2005db68:	edbff0ef          	jal	ra,2005da42 <prvAddNewTaskToReadyList>
2005db6c:	4432                	c.lwsp	s0,12(sp)
2005db6e:	40f2                	c.lwsp	ra,28(sp)
2005db70:	8522                	c.mv	a0,s0
2005db72:	4462                	c.lwsp	s0,24(sp)
2005db74:	6105                	c.addi16sp	sp,32
2005db76:	8082                	c.jr	ra
2005db78:	4401                	c.li	s0,0
2005db7a:	bfd5                	c.j	2005db6e <xTaskCreateStatic+0x26>

2005db7c <vTaskStartScheduler>:
2005db7c:	1101                	c.addi	sp,-32
2005db7e:	0070                	c.addi4spn	a2,sp,12
2005db80:	002c                	c.addi4spn	a1,sp,8
2005db82:	0048                	c.addi4spn	a0,sp,4
2005db84:	ce06                	c.swsp	ra,28(sp)
2005db86:	c202                	c.swsp	zero,4(sp)
2005db88:	c402                	c.swsp	zero,8(sp)
2005db8a:	8effe0ef          	jal	ra,2005c478 <vApplicationGetIdleTaskMemory>
2005db8e:	4812                	c.lwsp	a6,4(sp)
2005db90:	47a2                	c.lwsp	a5,8(sp)
2005db92:	4632                	c.lwsp	a2,12(sp)
2005db94:	200635b7          	lui	a1,0x20063
2005db98:	2005e537          	lui	a0,0x2005e
2005db9c:	4701                	c.li	a4,0
2005db9e:	4681                	c.li	a3,0
2005dba0:	02058593          	addi	a1,a1,32 # 20063020 <__func__.1+0x18>
2005dba4:	ea050513          	addi	a0,a0,-352 # 2005dea0 <prvIdleTask>
2005dba8:	fa1ff0ef          	jal	ra,2005db48 <xTaskCreateStatic>
2005dbac:	c51d                	c.beqz	a0,2005dbda <vTaskStartScheduler+0x5e>
2005dbae:	76a000ef          	jal	ra,2005e318 <xTimerCreateTimerTask>
2005dbb2:	4785                	c.li	a5,1
2005dbb4:	02f51363          	bne	a0,a5,2005dbda <vTaskStartScheduler+0x5e>
2005dbb8:	30047073          	csrrci	zero,mstatus,8
2005dbbc:	f601a783          	lw	a5,-160(gp) # 200661dc <pxCurrentTCB>
2005dbc0:	06c78793          	addi	a5,a5,108
2005dbc4:	eaf1a023          	sw	a5,-352(gp) # 2006611c <_impure_ptr>
2005dbc8:	577d                	c.li	a4,-1
2005dbca:	f8e1a423          	sw	a4,-120(gp) # 20066204 <xNextTaskUnblockTime>
2005dbce:	f8a1aa23          	sw	a0,-108(gp) # 20066210 <xSchedulerRunning>
2005dbd2:	f801ac23          	sw	zero,-104(gp) # 20066214 <xTickCount>
2005dbd6:	811fe0ef          	jal	ra,2005c3e6 <xPortStartScheduler>
2005dbda:	40f2                	c.lwsp	ra,28(sp)
2005dbdc:	e981a783          	lw	a5,-360(gp) # 20066114 <uxTopUsedPriority>
2005dbe0:	6105                	c.addi16sp	sp,32
2005dbe2:	8082                	c.jr	ra

2005dbe4 <xTaskCreate>:
2005dbe4:	7179                	c.addi16sp	sp,-48
2005dbe6:	ce4e                	c.swsp	s3,28(sp)
2005dbe8:	89aa                	c.mv	s3,a0
2005dbea:	00261513          	slli	a0,a2,0x2
2005dbee:	d04a                	c.swsp	s2,32(sp)
2005dbf0:	cc52                	c.swsp	s4,24(sp)
2005dbf2:	ca56                	c.swsp	s5,20(sp)
2005dbf4:	c85a                	c.swsp	s6,16(sp)
2005dbf6:	c65e                	c.swsp	s7,12(sp)
2005dbf8:	d606                	c.swsp	ra,44(sp)
2005dbfa:	d422                	c.swsp	s0,40(sp)
2005dbfc:	d226                	c.swsp	s1,36(sp)
2005dbfe:	8a2e                	c.mv	s4,a1
2005dc00:	8932                	c.mv	s2,a2
2005dc02:	8ab6                	c.mv	s5,a3
2005dc04:	8b3a                	c.mv	s6,a4
2005dc06:	8bbe                	c.mv	s7,a5
2005dc08:	d45fc0ef          	jal	ra,2005a94c <pvPortMalloc>
2005dc0c:	c531                	c.beqz	a0,2005dc58 <xTaskCreate+0x74>
2005dc0e:	84aa                	c.mv	s1,a0
2005dc10:	16400513          	addi	a0,zero,356
2005dc14:	d39fc0ef          	jal	ra,2005a94c <pvPortMalloc>
2005dc18:	842a                	c.mv	s0,a0
2005dc1a:	cd05                	c.beqz	a0,2005dc52 <xTaskCreate+0x6e>
2005dc1c:	882a                	c.mv	a6,a0
2005dc1e:	d904                	c.sw	s1,48(a0)
2005dc20:	160500a3          	sb	zero,353(a0)
2005dc24:	87de                	c.mv	a5,s7
2005dc26:	875a                	c.mv	a4,s6
2005dc28:	86d6                	c.mv	a3,s5
2005dc2a:	864a                	c.mv	a2,s2
2005dc2c:	85d2                	c.mv	a1,s4
2005dc2e:	854e                	c.mv	a0,s3
2005dc30:	f8cff0ef          	jal	ra,2005d3bc <prvInitialiseNewTask.constprop.0>
2005dc34:	8522                	c.mv	a0,s0
2005dc36:	e0dff0ef          	jal	ra,2005da42 <prvAddNewTaskToReadyList>
2005dc3a:	4505                	c.li	a0,1
2005dc3c:	50b2                	c.lwsp	ra,44(sp)
2005dc3e:	5422                	c.lwsp	s0,40(sp)
2005dc40:	5492                	c.lwsp	s1,36(sp)
2005dc42:	5902                	c.lwsp	s2,32(sp)
2005dc44:	49f2                	c.lwsp	s3,28(sp)
2005dc46:	4a62                	c.lwsp	s4,24(sp)
2005dc48:	4ad2                	c.lwsp	s5,20(sp)
2005dc4a:	4b42                	c.lwsp	s6,16(sp)
2005dc4c:	4bb2                	c.lwsp	s7,12(sp)
2005dc4e:	6145                	c.addi16sp	sp,48
2005dc50:	8082                	c.jr	ra
2005dc52:	8526                	c.mv	a0,s1
2005dc54:	dd7fc0ef          	jal	ra,2005aa2a <vPortFree>
2005dc58:	557d                	c.li	a0,-1
2005dc5a:	b7cd                	c.j	2005dc3c <xTaskCreate+0x58>

2005dc5c <vTaskDelete>:
2005dc5c:	1141                	c.addi	sp,-16
2005dc5e:	c422                	c.swsp	s0,8(sp)
2005dc60:	c226                	c.swsp	s1,4(sp)
2005dc62:	842a                	c.mv	s0,a0
2005dc64:	c606                	c.swsp	ra,12(sp)
2005dc66:	c04a                	c.swsp	s2,0(sp)
2005dc68:	f6018493          	addi	s1,gp,-160 # 200661dc <pxCurrentTCB>
2005dc6c:	d99ff0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005dc70:	e011                	c.bnez	s0,2005dc74 <vTaskDelete+0x18>
2005dc72:	4080                	c.lw	s0,0(s1)
2005dc74:	00440913          	addi	s2,s0,4
2005dc78:	854a                	c.mv	a0,s2
2005dc7a:	f6bfc0ef          	jal	ra,2005abe4 <uxListRemove>
2005dc7e:	e515                	c.bnez	a0,2005dcaa <vTaskDelete+0x4e>
2005dc80:	5450                	c.lw	a2,44(s0)
2005dc82:	4751                	c.li	a4,20
2005dc84:	02e606b3          	mul	a3,a2,a4
2005dc88:	20068737          	lui	a4,0x20068
2005dc8c:	ba870713          	addi	a4,a4,-1112 # 20067ba8 <pxReadyTasksLists>
2005dc90:	9736                	c.add	a4,a3
2005dc92:	431c                	c.lw	a5,0(a4)
2005dc94:	eb99                	c.bnez	a5,2005dcaa <vTaskDelete+0x4e>
2005dc96:	f8418713          	addi	a4,gp,-124 # 20066200 <uxTopReadyPriority>
2005dc9a:	4314                	c.lw	a3,0(a4)
2005dc9c:	4785                	c.li	a5,1
2005dc9e:	00c797b3          	sll	a5,a5,a2
2005dca2:	fff7c793          	xori	a5,a5,-1
2005dca6:	8ff5                	c.and	a5,a3
2005dca8:	c31c                	c.sw	a5,0(a4)
2005dcaa:	541c                	c.lw	a5,40(s0)
2005dcac:	c789                	c.beqz	a5,2005dcb6 <vTaskDelete+0x5a>
2005dcae:	01840513          	addi	a0,s0,24
2005dcb2:	f33fc0ef          	jal	ra,2005abe4 <uxListRemove>
2005dcb6:	f8018793          	addi	a5,gp,-128 # 200661fc <uxTaskNumber>
2005dcba:	4398                	c.lw	a4,0(a5)
2005dcbc:	0705                	c.addi	a4,1
2005dcbe:	c398                	c.sw	a4,0(a5)
2005dcc0:	409c                	c.lw	a5,0(s1)
2005dcc2:	02879e63          	bne	a5,s0,2005dcfe <vTaskDelete+0xa2>
2005dcc6:	20068537          	lui	a0,0x20068
2005dcca:	85ca                	c.mv	a1,s2
2005dccc:	cd450513          	addi	a0,a0,-812 # 20067cd4 <xTasksWaitingTermination>
2005dcd0:	ed1fc0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005dcd4:	f7818793          	addi	a5,gp,-136 # 200661f4 <uxDeletedTasksWaitingCleanUp>
2005dcd8:	4398                	c.lw	a4,0(a5)
2005dcda:	0705                	c.addi	a4,1
2005dcdc:	c398                	c.sw	a4,0(a5)
2005dcde:	d41ff0ef          	jal	ra,2005da1e <vTaskExitCritical>
2005dce2:	f941a783          	lw	a5,-108(gp) # 20066210 <xSchedulerRunning>
2005dce6:	c791                	c.beqz	a5,2005dcf2 <vTaskDelete+0x96>
2005dce8:	409c                	c.lw	a5,0(s1)
2005dcea:	00879463          	bne	a5,s0,2005dcf2 <vTaskDelete+0x96>
2005dcee:	00000073          	ecall
2005dcf2:	40b2                	c.lwsp	ra,12(sp)
2005dcf4:	4422                	c.lwsp	s0,8(sp)
2005dcf6:	4492                	c.lwsp	s1,4(sp)
2005dcf8:	4902                	c.lwsp	s2,0(sp)
2005dcfa:	0141                	c.addi	sp,16
2005dcfc:	8082                	c.jr	ra
2005dcfe:	f7418793          	addi	a5,gp,-140 # 200661f0 <uxCurrentNumberOfTasks>
2005dd02:	4398                	c.lw	a4,0(a5)
2005dd04:	8522                	c.mv	a0,s0
2005dd06:	177d                	c.addi	a4,-1
2005dd08:	c398                	c.sw	a4,0(a5)
2005dd0a:	e7cff0ef          	jal	ra,2005d386 <prvDeleteTCB>
2005dd0e:	e5cff0ef          	jal	ra,2005d36a <prvResetNextTaskUnblockTime>
2005dd12:	b7f1                	c.j	2005dcde <vTaskDelete+0x82>

2005dd14 <eTaskGetState>:
2005dd14:	f601a783          	lw	a5,-160(gp) # 200661dc <pxCurrentTCB>
2005dd18:	06a78863          	beq	a5,a0,2005dd88 <eTaskGetState+0x74>
2005dd1c:	1101                	c.addi	sp,-32
2005dd1e:	ce06                	c.swsp	ra,28(sp)
2005dd20:	cc22                	c.swsp	s0,24(sp)
2005dd22:	ca26                	c.swsp	s1,20(sp)
2005dd24:	842a                	c.mv	s0,a0
2005dd26:	c84a                	c.swsp	s2,16(sp)
2005dd28:	c64e                	c.swsp	s3,12(sp)
2005dd2a:	cdbff0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005dd2e:	f641a983          	lw	s3,-156(gp) # 200661e0 <pxDelayedTaskList>
2005dd32:	4844                	c.lw	s1,20(s0)
2005dd34:	f681a903          	lw	s2,-152(gp) # 200661e4 <pxOverflowDelayedTaskList>
2005dd38:	ce7ff0ef          	jal	ra,2005da1e <vTaskExitCritical>
2005dd3c:	4509                	c.li	a0,2
2005dd3e:	03348463          	beq	s1,s3,2005dd66 <eTaskGetState+0x52>
2005dd42:	03248263          	beq	s1,s2,2005dd66 <eTaskGetState+0x52>
2005dd46:	200687b7          	lui	a5,0x20068
2005dd4a:	cc078793          	addi	a5,a5,-832 # 20067cc0 <xSuspendedTaskList>
2005dd4e:	02f49363          	bne	s1,a5,2005dd74 <eTaskGetState+0x60>
2005dd52:	541c                	c.lw	a5,40(s0)
2005dd54:	eb89                	c.bnez	a5,2005dd66 <eTaskGetState+0x52>
2005dd56:	16044783          	lbu	a5,352(s0)
2005dd5a:	4705                	c.li	a4,1
2005dd5c:	0ff7f793          	andi	a5,a5,255
2005dd60:	00e78363          	beq	a5,a4,2005dd66 <eTaskGetState+0x52>
2005dd64:	450d                	c.li	a0,3
2005dd66:	40f2                	c.lwsp	ra,28(sp)
2005dd68:	4462                	c.lwsp	s0,24(sp)
2005dd6a:	44d2                	c.lwsp	s1,20(sp)
2005dd6c:	4942                	c.lwsp	s2,16(sp)
2005dd6e:	49b2                	c.lwsp	s3,12(sp)
2005dd70:	6105                	c.addi16sp	sp,32
2005dd72:	8082                	c.jr	ra
2005dd74:	200687b7          	lui	a5,0x20068
2005dd78:	cd478793          	addi	a5,a5,-812 # 20067cd4 <xTasksWaitingTermination>
2005dd7c:	4511                	c.li	a0,4
2005dd7e:	fef484e3          	beq	s1,a5,2005dd66 <eTaskGetState+0x52>
2005dd82:	d0f5                	c.beqz	s1,2005dd66 <eTaskGetState+0x52>
2005dd84:	4505                	c.li	a0,1
2005dd86:	b7c5                	c.j	2005dd66 <eTaskGetState+0x52>
2005dd88:	4501                	c.li	a0,0
2005dd8a:	8082                	c.jr	ra

2005dd8c <xTaskResumeAll>:
2005dd8c:	7139                	c.addi16sp	sp,-64
2005dd8e:	de06                	c.swsp	ra,60(sp)
2005dd90:	dc22                	c.swsp	s0,56(sp)
2005dd92:	da26                	c.swsp	s1,52(sp)
2005dd94:	d84a                	c.swsp	s2,48(sp)
2005dd96:	d64e                	c.swsp	s3,44(sp)
2005dd98:	d452                	c.swsp	s4,40(sp)
2005dd9a:	d256                	c.swsp	s5,36(sp)
2005dd9c:	d05a                	c.swsp	s6,32(sp)
2005dd9e:	ce5e                	c.swsp	s7,28(sp)
2005dda0:	c65ff0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005dda4:	f7c18793          	addi	a5,gp,-132 # 200661f8 <uxSchedulerSuspended>
2005dda8:	4398                	c.lw	a4,0(a5)
2005ddaa:	177d                	c.addi	a4,-1
2005ddac:	c398                	c.sw	a4,0(a5)
2005ddae:	439c                	c.lw	a5,0(a5)
2005ddb0:	c38d                	c.beqz	a5,2005ddd2 <xTaskResumeAll+0x46>
2005ddb2:	4501                	c.li	a0,0
2005ddb4:	c62a                	c.swsp	a0,12(sp)
2005ddb6:	c69ff0ef          	jal	ra,2005da1e <vTaskExitCritical>
2005ddba:	50f2                	c.lwsp	ra,60(sp)
2005ddbc:	5462                	c.lwsp	s0,56(sp)
2005ddbe:	4532                	c.lwsp	a0,12(sp)
2005ddc0:	54d2                	c.lwsp	s1,52(sp)
2005ddc2:	5942                	c.lwsp	s2,48(sp)
2005ddc4:	59b2                	c.lwsp	s3,44(sp)
2005ddc6:	5a22                	c.lwsp	s4,40(sp)
2005ddc8:	5a92                	c.lwsp	s5,36(sp)
2005ddca:	5b02                	c.lwsp	s6,32(sp)
2005ddcc:	4bf2                	c.lwsp	s7,28(sp)
2005ddce:	6121                	c.addi16sp	sp,64
2005ddd0:	8082                	c.jr	ra
2005ddd2:	f741a783          	lw	a5,-140(gp) # 200661f0 <uxCurrentNumberOfTasks>
2005ddd6:	dff1                	c.beqz	a5,2005ddb2 <xTaskResumeAll+0x26>
2005ddd8:	200684b7          	lui	s1,0x20068
2005dddc:	20068937          	lui	s2,0x20068
2005dde0:	4401                	c.li	s0,0
2005dde2:	cac48493          	addi	s1,s1,-852 # 20067cac <xPendingReadyList>
2005dde6:	4985                	c.li	s3,1
2005dde8:	ba890913          	addi	s2,s2,-1112 # 20067ba8 <pxReadyTasksLists>
2005ddec:	4ad1                	c.li	s5,20
2005ddee:	a0b1                	c.j	2005de3a <xTaskResumeAll+0xae>
2005ddf0:	44dc                	c.lw	a5,12(s1)
2005ddf2:	47c0                	c.lw	s0,12(a5)
2005ddf4:	01840513          	addi	a0,s0,24
2005ddf8:	dedfc0ef          	jal	ra,2005abe4 <uxListRemove>
2005ddfc:	00440593          	addi	a1,s0,4
2005de00:	852e                	c.mv	a0,a1
2005de02:	c62e                	c.swsp	a1,12(sp)
2005de04:	de1fc0ef          	jal	ra,2005abe4 <uxListRemove>
2005de08:	5448                	c.lw	a0,44(s0)
2005de0a:	f8418713          	addi	a4,gp,-124 # 20066200 <uxTopReadyPriority>
2005de0e:	4314                	c.lw	a3,0(a4)
2005de10:	00a997b3          	sll	a5,s3,a0
2005de14:	03550533          	mul	a0,a0,s5
2005de18:	45b2                	c.lwsp	a1,12(sp)
2005de1a:	8fd5                	c.or	a5,a3
2005de1c:	c31c                	c.sw	a5,0(a4)
2005de1e:	954a                	c.add	a0,s2
2005de20:	d81fc0ef          	jal	ra,2005aba0 <vListInsertEnd>
2005de24:	f6018793          	addi	a5,gp,-160 # 200661dc <pxCurrentTCB>
2005de28:	439c                	c.lw	a5,0(a5)
2005de2a:	5458                	c.lw	a4,44(s0)
2005de2c:	57dc                	c.lw	a5,44(a5)
2005de2e:	00f76663          	bltu	a4,a5,2005de3a <xTaskResumeAll+0xae>
2005de32:	f9c18793          	addi	a5,gp,-100 # 20066218 <xYieldPending>
2005de36:	0137a023          	sw	s3,0(a5)
2005de3a:	409c                	c.lw	a5,0(s1)
2005de3c:	fbd5                	c.bnez	a5,2005ddf0 <xTaskResumeAll+0x64>
2005de3e:	c019                	c.beqz	s0,2005de44 <xTaskResumeAll+0xb8>
2005de40:	d2aff0ef          	jal	ra,2005d36a <prvResetNextTaskUnblockTime>
2005de44:	f9018713          	addi	a4,gp,-112 # 2006620c <xPendedTicks>
2005de48:	4304                	c.lw	s1,0(a4)
2005de4a:	f9018413          	addi	s0,gp,-112 # 2006620c <xPendedTicks>
2005de4e:	cc89                	c.beqz	s1,2005de68 <xTaskResumeAll+0xdc>
2005de50:	4985                	c.li	s3,1
2005de52:	eb0ff0ef          	jal	ra,2005d502 <xTaskIncrementTick>
2005de56:	c509                	c.beqz	a0,2005de60 <xTaskResumeAll+0xd4>
2005de58:	f9c18793          	addi	a5,gp,-100 # 20066218 <xYieldPending>
2005de5c:	0137a023          	sw	s3,0(a5)
2005de60:	14fd                	c.addi	s1,-1
2005de62:	f8e5                	c.bnez	s1,2005de52 <xTaskResumeAll+0xc6>
2005de64:	00042023          	sw	zero,0(s0)
2005de68:	f9c1a783          	lw	a5,-100(gp) # 20066218 <xYieldPending>
2005de6c:	d3b9                	c.beqz	a5,2005ddb2 <xTaskResumeAll+0x26>
2005de6e:	00000073          	ecall
2005de72:	4505                	c.li	a0,1
2005de74:	b781                	c.j	2005ddb4 <xTaskResumeAll+0x28>

2005de76 <vTaskDelay>:
2005de76:	e501                	c.bnez	a0,2005de7e <vTaskDelay+0x8>
2005de78:	00000073          	ecall
2005de7c:	8082                	c.jr	ra
2005de7e:	1101                	c.addi	sp,-32
2005de80:	ce06                	c.swsp	ra,28(sp)
2005de82:	c62a                	c.swsp	a0,12(sp)
2005de84:	e5aff0ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005de88:	4532                	c.lwsp	a0,12(sp)
2005de8a:	4581                	c.li	a1,0
2005de8c:	bbaff0ef          	jal	ra,2005d246 <prvAddCurrentTaskToDelayedList>
2005de90:	efdff0ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005de94:	e119                	c.bnez	a0,2005de9a <vTaskDelay+0x24>
2005de96:	00000073          	ecall
2005de9a:	40f2                	c.lwsp	ra,28(sp)
2005de9c:	6105                	c.addi16sp	sp,32
2005de9e:	8082                	c.jr	ra

2005dea0 <prvIdleTask>:
2005dea0:	1101                	c.addi	sp,-32
2005dea2:	ca26                	c.swsp	s1,20(sp)
2005dea4:	200684b7          	lui	s1,0x20068
2005dea8:	cc22                	c.swsp	s0,24(sp)
2005deaa:	c452                	c.swsp	s4,8(sp)
2005deac:	c256                	c.swsp	s5,4(sp)
2005deae:	c05a                	c.swsp	s6,0(sp)
2005deb0:	ce06                	c.swsp	ra,28(sp)
2005deb2:	c84a                	c.swsp	s2,16(sp)
2005deb4:	c64e                	c.swsp	s3,12(sp)
2005deb6:	cd448493          	addi	s1,s1,-812 # 20067cd4 <xTasksWaitingTermination>
2005deba:	20068ab7          	lui	s5,0x20068
2005debe:	4405                	c.li	s0,1
2005dec0:	f7818913          	addi	s2,gp,-136 # 200661f4 <uxDeletedTasksWaitingCleanUp>
2005dec4:	00092783          	lw	a5,0(s2)
2005dec8:	eb8d                	c.bnez	a5,2005defa <prvIdleTask+0x5a>
2005deca:	ba8a8793          	addi	a5,s5,-1112 # 20067ba8 <pxReadyTasksLists>
2005dece:	439c                	c.lw	a5,0(a5)
2005ded0:	00f47463          	bgeu	s0,a5,2005ded8 <prvIdleTask+0x38>
2005ded4:	00000073          	ecall
2005ded8:	d2efe0ef          	jal	ra,2005c406 <vApplicationIdleHook>
2005dedc:	c60ff0ef          	jal	ra,2005d33c <prvGetExpectedIdleTime>
2005dee0:	fea470e3          	bgeu	s0,a0,2005dec0 <prvIdleTask+0x20>
2005dee4:	dfaff0ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005dee8:	c54ff0ef          	jal	ra,2005d33c <prvGetExpectedIdleTime>
2005deec:	00a47463          	bgeu	s0,a0,2005def4 <prvIdleTask+0x54>
2005def0:	dc0fe0ef          	jal	ra,2005c4b0 <vPortSuppressTicksAndSleep>
2005def4:	e99ff0ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005def8:	b7e1                	c.j	2005dec0 <prvIdleTask+0x20>
2005defa:	b0bff0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005defe:	44dc                	c.lw	a5,12(s1)
2005df00:	00c7a983          	lw	s3,12(a5)
2005df04:	00498513          	addi	a0,s3,4
2005df08:	cddfc0ef          	jal	ra,2005abe4 <uxListRemove>
2005df0c:	f7418713          	addi	a4,gp,-140 # 200661f0 <uxCurrentNumberOfTasks>
2005df10:	431c                	c.lw	a5,0(a4)
2005df12:	17fd                	c.addi	a5,-1
2005df14:	c31c                	c.sw	a5,0(a4)
2005df16:	00092783          	lw	a5,0(s2)
2005df1a:	17fd                	c.addi	a5,-1
2005df1c:	00f92023          	sw	a5,0(s2)
2005df20:	affff0ef          	jal	ra,2005da1e <vTaskExitCritical>
2005df24:	854e                	c.mv	a0,s3
2005df26:	c60ff0ef          	jal	ra,2005d386 <prvDeleteTCB>
2005df2a:	bf59                	c.j	2005dec0 <prvIdleTask+0x20>

2005df2c <vTaskGetInfo>:
2005df2c:	1141                	c.addi	sp,-16
2005df2e:	c422                	c.swsp	s0,8(sp)
2005df30:	c226                	c.swsp	s1,4(sp)
2005df32:	c04a                	c.swsp	s2,0(sp)
2005df34:	c606                	c.swsp	ra,12(sp)
2005df36:	842e                	c.mv	s0,a1
2005df38:	8932                	c.mv	s2,a2
2005df3a:	84aa                	c.mv	s1,a0
2005df3c:	e119                	c.bnez	a0,2005df42 <vTaskGetInfo+0x16>
2005df3e:	f601a483          	lw	s1,-160(gp) # 200661dc <pxCurrentTCB>
2005df42:	03448793          	addi	a5,s1,52
2005df46:	c004                	c.sw	s1,0(s0)
2005df48:	c05c                	c.sw	a5,4(s0)
2005df4a:	54dc                	c.lw	a5,44(s1)
2005df4c:	c81c                	c.sw	a5,16(s0)
2005df4e:	589c                	c.lw	a5,48(s1)
2005df50:	cc5c                	c.sw	a5,28(s0)
2005df52:	48fc                	c.lw	a5,84(s1)
2005df54:	c41c                	c.sw	a5,8(s0)
2005df56:	4cfc                	c.lw	a5,92(s1)
2005df58:	c85c                	c.sw	a5,20(s0)
2005df5a:	54bc                	c.lw	a5,104(s1)
2005df5c:	cc1c                	c.sw	a5,24(s0)
2005df5e:	4795                	c.li	a5,5
2005df60:	04f68c63          	beq	a3,a5,2005dfb8 <vTaskGetInfo+0x8c>
2005df64:	f601a783          	lw	a5,-160(gp) # 200661dc <pxCurrentTCB>
2005df68:	02979963          	bne	a5,s1,2005df9a <vTaskGetInfo+0x6e>
2005df6c:	00040623          	sb	zero,12(s0)
2005df70:	04090c63          	beq	s2,zero,2005dfc8 <vTaskGetInfo+0x9c>
2005df74:	5890                	c.lw	a2,48(s1)
2005df76:	4781                	c.li	a5,0
2005df78:	0a500713          	addi	a4,zero,165
2005df7c:	00f606b3          	add	a3,a2,a5
2005df80:	0006c683          	lbu	a3,0(a3)
2005df84:	04e68063          	beq	a3,a4,2005dfc4 <vTaskGetInfo+0x98>
2005df88:	8389                	c.srli	a5,0x2
2005df8a:	02f41023          	sh	a5,32(s0)
2005df8e:	40b2                	c.lwsp	ra,12(sp)
2005df90:	4422                	c.lwsp	s0,8(sp)
2005df92:	4492                	c.lwsp	s1,4(sp)
2005df94:	4902                	c.lwsp	s2,0(sp)
2005df96:	0141                	c.addi	sp,16
2005df98:	8082                	c.jr	ra
2005df9a:	00d40623          	sb	a3,12(s0)
2005df9e:	478d                	c.li	a5,3
2005dfa0:	fcf698e3          	bne	a3,a5,2005df70 <vTaskGetInfo+0x44>
2005dfa4:	d3aff0ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005dfa8:	549c                	c.lw	a5,40(s1)
2005dfaa:	c781                	c.beqz	a5,2005dfb2 <vTaskGetInfo+0x86>
2005dfac:	4789                	c.li	a5,2
2005dfae:	00f40623          	sb	a5,12(s0)
2005dfb2:	ddbff0ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005dfb6:	bf6d                	c.j	2005df70 <vTaskGetInfo+0x44>
2005dfb8:	8526                	c.mv	a0,s1
2005dfba:	d5bff0ef          	jal	ra,2005dd14 <eTaskGetState>
2005dfbe:	00a40623          	sb	a0,12(s0)
2005dfc2:	b77d                	c.j	2005df70 <vTaskGetInfo+0x44>
2005dfc4:	0785                	c.addi	a5,1
2005dfc6:	bf5d                	c.j	2005df7c <vTaskGetInfo+0x50>
2005dfc8:	02041023          	sh	zero,32(s0)
2005dfcc:	b7c9                	c.j	2005df8e <vTaskGetInfo+0x62>

2005dfce <prvListTasksWithinSingleList>:
2005dfce:	7179                	c.addi16sp	sp,-48
2005dfd0:	d606                	c.swsp	ra,44(sp)
2005dfd2:	d422                	c.swsp	s0,40(sp)
2005dfd4:	d226                	c.swsp	s1,36(sp)
2005dfd6:	d04a                	c.swsp	s2,32(sp)
2005dfd8:	ce4e                	c.swsp	s3,28(sp)
2005dfda:	cc52                	c.swsp	s4,24(sp)
2005dfdc:	ca56                	c.swsp	s5,20(sp)
2005dfde:	c85a                	c.swsp	s6,16(sp)
2005dfe0:	c65e                	c.swsp	s7,12(sp)
2005dfe2:	4184                	c.lw	s1,0(a1)
2005dfe4:	c8a9                	c.beqz	s1,2005e036 <prvListTasksWithinSingleList+0x68>
2005dfe6:	41dc                	c.lw	a5,4(a1)
2005dfe8:	00858a13          	addi	s4,a1,8
2005dfec:	892a                	c.mv	s2,a0
2005dfee:	43dc                	c.lw	a5,4(a5)
2005dff0:	842e                	c.mv	s0,a1
2005dff2:	89b2                	c.mv	s3,a2
2005dff4:	c1dc                	c.sw	a5,4(a1)
2005dff6:	01479463          	bne	a5,s4,2005dffe <prvListTasksWithinSingleList+0x30>
2005dffa:	45dc                	c.lw	a5,12(a1)
2005dffc:	c1dc                	c.sw	a5,4(a1)
2005dffe:	405c                	c.lw	a5,4(s0)
2005e000:	4481                	c.li	s1,0
2005e002:	02400b93          	addi	s7,zero,36
2005e006:	00c7ab03          	lw	s6,12(a5)
2005e00a:	405c                	c.lw	a5,4(s0)
2005e00c:	43dc                	c.lw	a5,4(a5)
2005e00e:	c05c                	c.sw	a5,4(s0)
2005e010:	00fa1563          	bne	s4,a5,2005e01a <prvListTasksWithinSingleList+0x4c>
2005e014:	004a2783          	lw	a5,4(s4)
2005e018:	c05c                	c.sw	a5,4(s0)
2005e01a:	037485b3          	mul	a1,s1,s7
2005e01e:	405c                	c.lw	a5,4(s0)
2005e020:	86ce                	c.mv	a3,s3
2005e022:	4605                	c.li	a2,1
2005e024:	00c7aa83          	lw	s5,12(a5)
2005e028:	0485                	c.addi	s1,1
2005e02a:	8556                	c.mv	a0,s5
2005e02c:	95ca                	c.add	a1,s2
2005e02e:	effff0ef          	jal	ra,2005df2c <vTaskGetInfo>
2005e032:	fd5b1ce3          	bne	s6,s5,2005e00a <prvListTasksWithinSingleList+0x3c>
2005e036:	50b2                	c.lwsp	ra,44(sp)
2005e038:	5422                	c.lwsp	s0,40(sp)
2005e03a:	5902                	c.lwsp	s2,32(sp)
2005e03c:	49f2                	c.lwsp	s3,28(sp)
2005e03e:	4a62                	c.lwsp	s4,24(sp)
2005e040:	4ad2                	c.lwsp	s5,20(sp)
2005e042:	4b42                	c.lwsp	s6,16(sp)
2005e044:	4bb2                	c.lwsp	s7,12(sp)
2005e046:	8526                	c.mv	a0,s1
2005e048:	5492                	c.lwsp	s1,36(sp)
2005e04a:	6145                	c.addi16sp	sp,48
2005e04c:	8082                	c.jr	ra

2005e04e <uxTaskGetSystemState>:
2005e04e:	1101                	c.addi	sp,-32
2005e050:	cc22                	c.swsp	s0,24(sp)
2005e052:	c84a                	c.swsp	s2,16(sp)
2005e054:	c64e                	c.swsp	s3,12(sp)
2005e056:	ce06                	c.swsp	ra,28(sp)
2005e058:	ca26                	c.swsp	s1,20(sp)
2005e05a:	c452                	c.swsp	s4,8(sp)
2005e05c:	c256                	c.swsp	s5,4(sp)
2005e05e:	c05a                	c.swsp	s6,0(sp)
2005e060:	892a                	c.mv	s2,a0
2005e062:	842e                	c.mv	s0,a1
2005e064:	89b2                	c.mv	s3,a2
2005e066:	c78ff0ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005e06a:	f741a783          	lw	a5,-140(gp) # 200661f0 <uxCurrentNumberOfTasks>
2005e06e:	0af46563          	bltu	s0,a5,2005e118 <uxTaskGetSystemState+0xca>
2005e072:	20068ab7          	lui	s5,0x20068
2005e076:	4a2d                	c.li	s4,11
2005e078:	4401                	c.li	s0,0
2005e07a:	4b51                	c.li	s6,20
2005e07c:	ba8a8a93          	addi	s5,s5,-1112 # 20067ba8 <pxReadyTasksLists>
2005e080:	02400493          	addi	s1,zero,36
2005e084:	1a7d                	c.addi	s4,-1
2005e086:	02940533          	mul	a0,s0,s1
2005e08a:	4605                	c.li	a2,1
2005e08c:	036a05b3          	mul	a1,s4,s6
2005e090:	954a                	c.add	a0,s2
2005e092:	95d6                	c.add	a1,s5
2005e094:	f3bff0ef          	jal	ra,2005dfce <prvListTasksWithinSingleList>
2005e098:	942a                	c.add	s0,a0
2005e09a:	fe0a15e3          	bne	s4,zero,2005e084 <uxTaskGetSystemState+0x36>
2005e09e:	02940533          	mul	a0,s0,s1
2005e0a2:	f641a583          	lw	a1,-156(gp) # 200661e0 <pxDelayedTaskList>
2005e0a6:	4609                	c.li	a2,2
2005e0a8:	954a                	c.add	a0,s2
2005e0aa:	f25ff0ef          	jal	ra,2005dfce <prvListTasksWithinSingleList>
2005e0ae:	942a                	c.add	s0,a0
2005e0b0:	02940533          	mul	a0,s0,s1
2005e0b4:	f681a583          	lw	a1,-152(gp) # 200661e4 <pxOverflowDelayedTaskList>
2005e0b8:	4609                	c.li	a2,2
2005e0ba:	954a                	c.add	a0,s2
2005e0bc:	f13ff0ef          	jal	ra,2005dfce <prvListTasksWithinSingleList>
2005e0c0:	942a                	c.add	s0,a0
2005e0c2:	02940533          	mul	a0,s0,s1
2005e0c6:	200685b7          	lui	a1,0x20068
2005e0ca:	4611                	c.li	a2,4
2005e0cc:	cd458593          	addi	a1,a1,-812 # 20067cd4 <xTasksWaitingTermination>
2005e0d0:	954a                	c.add	a0,s2
2005e0d2:	efdff0ef          	jal	ra,2005dfce <prvListTasksWithinSingleList>
2005e0d6:	942a                	c.add	s0,a0
2005e0d8:	029404b3          	mul	s1,s0,s1
2005e0dc:	200685b7          	lui	a1,0x20068
2005e0e0:	460d                	c.li	a2,3
2005e0e2:	cc058593          	addi	a1,a1,-832 # 20067cc0 <xSuspendedTaskList>
2005e0e6:	00990533          	add	a0,s2,s1
2005e0ea:	ee5ff0ef          	jal	ra,2005dfce <prvListTasksWithinSingleList>
2005e0ee:	942a                	c.add	s0,a0
2005e0f0:	00098763          	beq	s3,zero,2005e0fe <uxTaskGetSystemState+0xb0>
2005e0f4:	410187b7          	lui	a5,0x41018
2005e0f8:	4bdc                	c.lw	a5,20(a5)
2005e0fa:	00f9a023          	sw	a5,0(s3)
2005e0fe:	c8fff0ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005e102:	40f2                	c.lwsp	ra,28(sp)
2005e104:	8522                	c.mv	a0,s0
2005e106:	4462                	c.lwsp	s0,24(sp)
2005e108:	44d2                	c.lwsp	s1,20(sp)
2005e10a:	4942                	c.lwsp	s2,16(sp)
2005e10c:	49b2                	c.lwsp	s3,12(sp)
2005e10e:	4a22                	c.lwsp	s4,8(sp)
2005e110:	4a92                	c.lwsp	s5,4(sp)
2005e112:	4b02                	c.lwsp	s6,0(sp)
2005e114:	6105                	c.addi16sp	sp,32
2005e116:	8082                	c.jr	ra
2005e118:	4401                	c.li	s0,0
2005e11a:	b7d5                	c.j	2005e0fe <uxTaskGetSystemState+0xb0>

2005e11c <xTaskGetTickCount>:
2005e11c:	1101                	c.addi	sp,-32
2005e11e:	ce06                	c.swsp	ra,28(sp)
2005e120:	8e5ff0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005e124:	f981a503          	lw	a0,-104(gp) # 20066214 <xTickCount>
2005e128:	c62a                	c.swsp	a0,12(sp)
2005e12a:	8f5ff0ef          	jal	ra,2005da1e <vTaskExitCritical>
2005e12e:	40f2                	c.lwsp	ra,28(sp)
2005e130:	4532                	c.lwsp	a0,12(sp)
2005e132:	6105                	c.addi16sp	sp,32
2005e134:	8082                	c.jr	ra

2005e136 <xTaskCheckForTimeOut>:
2005e136:	1101                	c.addi	sp,-32
2005e138:	cc22                	c.swsp	s0,24(sp)
2005e13a:	ce06                	c.swsp	ra,28(sp)
2005e13c:	842e                	c.mv	s0,a1
2005e13e:	c62a                	c.swsp	a0,12(sp)
2005e140:	8c5ff0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005e144:	f981a683          	lw	a3,-104(gp) # 20066214 <xTickCount>
2005e148:	401c                	c.lw	a5,0(s0)
2005e14a:	577d                	c.li	a4,-1
2005e14c:	02e78f63          	beq	a5,a4,2005e18a <xTaskCheckForTimeOut+0x54>
2005e150:	4532                	c.lwsp	a0,12(sp)
2005e152:	f8c1a603          	lw	a2,-116(gp) # 20066208 <xNumOfOverflows>
2005e156:	410c                	c.lw	a1,0(a0)
2005e158:	4158                	c.lw	a4,4(a0)
2005e15a:	00c58f63          	beq	a1,a2,2005e178 <xTaskCheckForTimeOut+0x42>
2005e15e:	00e6ed63          	bltu	a3,a4,2005e178 <xTaskCheckForTimeOut+0x42>
2005e162:	00042023          	sw	zero,0(s0)
2005e166:	4505                	c.li	a0,1
2005e168:	c62a                	c.swsp	a0,12(sp)
2005e16a:	8b5ff0ef          	jal	ra,2005da1e <vTaskExitCritical>
2005e16e:	40f2                	c.lwsp	ra,28(sp)
2005e170:	4462                	c.lwsp	s0,24(sp)
2005e172:	4532                	c.lwsp	a0,12(sp)
2005e174:	6105                	c.addi16sp	sp,32
2005e176:	8082                	c.jr	ra
2005e178:	40e68633          	sub	a2,a3,a4
2005e17c:	fef673e3          	bgeu	a2,a5,2005e162 <xTaskCheckForTimeOut+0x2c>
2005e180:	8f95                	c.sub	a5,a3
2005e182:	97ba                	c.add	a5,a4
2005e184:	c01c                	c.sw	a5,0(s0)
2005e186:	e50ff0ef          	jal	ra,2005d7d6 <vTaskInternalSetTimeOutState>
2005e18a:	4501                	c.li	a0,0
2005e18c:	bff1                	c.j	2005e168 <xTaskCheckForTimeOut+0x32>

2005e18e <vTaskList>:
2005e18e:	7139                	c.addi16sp	sp,-64
2005e190:	dc22                	c.swsp	s0,56(sp)
2005e192:	de06                	c.swsp	ra,60(sp)
2005e194:	da26                	c.swsp	s1,52(sp)
2005e196:	d84a                	c.swsp	s2,48(sp)
2005e198:	d64e                	c.swsp	s3,44(sp)
2005e19a:	d452                	c.swsp	s4,40(sp)
2005e19c:	d256                	c.swsp	s5,36(sp)
2005e19e:	d05a                	c.swsp	s6,32(sp)
2005e1a0:	ce5e                	c.swsp	s7,28(sp)
2005e1a2:	f7418793          	addi	a5,gp,-140 # 200661f0 <uxCurrentNumberOfTasks>
2005e1a6:	438c                	c.lw	a1,0(a5)
2005e1a8:	842a                	c.mv	s0,a0
2005e1aa:	00050023          	sb	zero,0(a0)
2005e1ae:	4388                	c.lw	a0,0(a5)
2005e1b0:	02400793          	addi	a5,zero,36
2005e1b4:	c62e                	c.swsp	a1,12(sp)
2005e1b6:	02f50533          	mul	a0,a0,a5
2005e1ba:	f92fc0ef          	jal	ra,2005a94c <pvPortMalloc>
2005e1be:	c151                	c.beqz	a0,2005e242 <vTaskList+0xb4>
2005e1c0:	45b2                	c.lwsp	a1,12(sp)
2005e1c2:	4601                	c.li	a2,0
2005e1c4:	892a                	c.mv	s2,a0
2005e1c6:	e89ff0ef          	jal	ra,2005e04e <uxTaskGetSystemState>
2005e1ca:	89aa                	c.mv	s3,a0
2005e1cc:	00490493          	addi	s1,s2,4
2005e1d0:	4a01                	c.li	s4,0
2005e1d2:	4b91                	c.li	s7,4
2005e1d4:	e6c18a93          	addi	s5,gp,-404 # 200660e8 <CSWTCH.248>
2005e1d8:	20063b37          	lui	s6,0x20063
2005e1dc:	013a1f63          	bne	s4,s3,2005e1fa <vTaskList+0x6c>
2005e1e0:	5462                	c.lwsp	s0,56(sp)
2005e1e2:	50f2                	c.lwsp	ra,60(sp)
2005e1e4:	54d2                	c.lwsp	s1,52(sp)
2005e1e6:	59b2                	c.lwsp	s3,44(sp)
2005e1e8:	5a22                	c.lwsp	s4,40(sp)
2005e1ea:	5a92                	c.lwsp	s5,36(sp)
2005e1ec:	5b02                	c.lwsp	s6,32(sp)
2005e1ee:	4bf2                	c.lwsp	s7,28(sp)
2005e1f0:	854a                	c.mv	a0,s2
2005e1f2:	5942                	c.lwsp	s2,48(sp)
2005e1f4:	6121                	c.addi16sp	sp,64
2005e1f6:	835fc06f          	jal	zero,2005aa2a <vPortFree>
2005e1fa:	0084c783          	lbu	a5,8(s1)
2005e1fe:	4681                	c.li	a3,0
2005e200:	00fbe563          	bltu	s7,a5,2005e20a <vTaskList+0x7c>
2005e204:	97d6                	c.add	a5,s5
2005e206:	0007c683          	lbu	a3,0(a5) # 41018000 <__km4_bd_boot_download_addr__+0x11006000>
2005e20a:	408c                	c.lw	a1,0(s1)
2005e20c:	8522                	c.mv	a0,s0
2005e20e:	c636                	c.swsp	a3,12(sp)
2005e210:	8e2ff0ef          	jal	ra,2005d2f2 <prvWriteNameToBuffer>
2005e214:	0044a803          	lw	a6,4(s1)
2005e218:	01c4d783          	lhu	a5,28(s1)
2005e21c:	44d8                	c.lw	a4,12(s1)
2005e21e:	46b2                	c.lwsp	a3,12(sp)
2005e220:	028b0613          	addi	a2,s6,40 # 20063028 <__func__.1+0x20>
2005e224:	40000593          	addi	a1,zero,1024
2005e228:	842a                	c.mv	s0,a0
2005e22a:	07c010ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e22e:	8522                	c.mv	a0,s0
2005e230:	dffac097          	auipc	ra,0xdffac
2005e234:	8a8080e7          	jalr	ra,-1880(ra) # 9ad8 <__wrap_strlen>
2005e238:	942a                	c.add	s0,a0
2005e23a:	0a05                	c.addi	s4,1
2005e23c:	02448493          	addi	s1,s1,36
2005e240:	bf71                	c.j	2005e1dc <vTaskList+0x4e>
2005e242:	50f2                	c.lwsp	ra,60(sp)
2005e244:	5462                	c.lwsp	s0,56(sp)
2005e246:	54d2                	c.lwsp	s1,52(sp)
2005e248:	5942                	c.lwsp	s2,48(sp)
2005e24a:	59b2                	c.lwsp	s3,44(sp)
2005e24c:	5a22                	c.lwsp	s4,40(sp)
2005e24e:	5a92                	c.lwsp	s5,36(sp)
2005e250:	5b02                	c.lwsp	s6,32(sp)
2005e252:	4bf2                	c.lwsp	s7,28(sp)
2005e254:	6121                	c.addi16sp	sp,64
2005e256:	8082                	c.jr	ra

2005e258 <pvTaskIncrementMutexHeldCount>:
2005e258:	f6018713          	addi	a4,gp,-160 # 200661dc <pxCurrentTCB>
2005e25c:	4318                	c.lw	a4,0(a4)
2005e25e:	f6018793          	addi	a5,gp,-160 # 200661dc <pxCurrentTCB>
2005e262:	c709                	c.beqz	a4,2005e26c <pvTaskIncrementMutexHeldCount+0x14>
2005e264:	4394                	c.lw	a3,0(a5)
2005e266:	52b8                	c.lw	a4,96(a3)
2005e268:	0705                	c.addi	a4,1
2005e26a:	d2b8                	c.sw	a4,96(a3)
2005e26c:	4388                	c.lw	a0,0(a5)
2005e26e:	8082                	c.jr	ra

2005e270 <prvCheckForValidListAndQueue>:
2005e270:	1141                	c.addi	sp,-16
2005e272:	c422                	c.swsp	s0,8(sp)
2005e274:	c606                	c.swsp	ra,12(sp)
2005e276:	c226                	c.swsp	s1,4(sp)
2005e278:	c04a                	c.swsp	s2,0(sp)
2005e27a:	fac18413          	addi	s0,gp,-84 # 20066228 <xTimerQueue>
2005e27e:	f86ff0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005e282:	401c                	c.lw	a5,0(s0)
2005e284:	e3b9                	c.bnez	a5,2005e2ca <prvCheckForValidListAndQueue+0x5a>
2005e286:	20068537          	lui	a0,0x20068
2005e28a:	d6050913          	addi	s2,a0,-672 # 20067d60 <xActiveTimerList1>
2005e28e:	d6050513          	addi	a0,a0,-672
2005e292:	8f5fc0ef          	jal	ra,2005ab86 <vListInitialise>
2005e296:	20068537          	lui	a0,0x20068
2005e29a:	d7450493          	addi	s1,a0,-652 # 20067d74 <xActiveTimerList2>
2005e29e:	d7450513          	addi	a0,a0,-652
2005e2a2:	8e5fc0ef          	jal	ra,2005ab86 <vListInitialise>
2005e2a6:	200686b7          	lui	a3,0x20068
2005e2aa:	20068637          	lui	a2,0x20068
2005e2ae:	fb21a023          	sw	s2,-96(gp) # 2006621c <pxCurrentTimerList>
2005e2b2:	4701                	c.li	a4,0
2005e2b4:	d8868693          	addi	a3,a3,-632 # 20067d88 <xStaticTimerQueue.0>
2005e2b8:	ce860613          	addi	a2,a2,-792 # 20067ce8 <ucStaticTimerQueueStorage.1>
2005e2bc:	45b1                	c.li	a1,12
2005e2be:	4529                	c.li	a0,10
2005e2c0:	fa91a223          	sw	s1,-92(gp) # 20066220 <pxOverflowTimerList>
2005e2c4:	9b7fe0ef          	jal	ra,2005cc7a <xQueueGenericCreateStatic>
2005e2c8:	c008                	c.sw	a0,0(s0)
2005e2ca:	4422                	c.lwsp	s0,8(sp)
2005e2cc:	40b2                	c.lwsp	ra,12(sp)
2005e2ce:	4492                	c.lwsp	s1,4(sp)
2005e2d0:	4902                	c.lwsp	s2,0(sp)
2005e2d2:	0141                	c.addi	sp,16
2005e2d4:	f4aff06f          	jal	zero,2005da1e <vTaskExitCritical>

2005e2d8 <prvInsertTimerInActiveList>:
2005e2d8:	1141                	c.addi	sp,-16
2005e2da:	c606                	c.swsp	ra,12(sp)
2005e2dc:	c14c                	c.sw	a1,4(a0)
2005e2de:	c908                	c.sw	a0,16(a0)
2005e2e0:	87aa                	c.mv	a5,a0
2005e2e2:	02b66163          	bltu	a2,a1,2005e304 <prvInsertTimerInActiveList+0x2c>
2005e2e6:	4d18                	c.lw	a4,24(a0)
2005e2e8:	8e15                	c.sub	a2,a3
2005e2ea:	4505                	c.li	a0,1
2005e2ec:	00e67963          	bgeu	a2,a4,2005e2fe <prvInsertTimerInActiveList+0x26>
2005e2f0:	00478593          	addi	a1,a5,4
2005e2f4:	fa41a503          	lw	a0,-92(gp) # 20066220 <pxOverflowTimerList>
2005e2f8:	8bffc0ef          	jal	ra,2005abb6 <vListInsert>
2005e2fc:	4501                	c.li	a0,0
2005e2fe:	40b2                	c.lwsp	ra,12(sp)
2005e300:	0141                	c.addi	sp,16
2005e302:	8082                	c.jr	ra
2005e304:	00d67563          	bgeu	a2,a3,2005e30e <prvInsertTimerInActiveList+0x36>
2005e308:	4505                	c.li	a0,1
2005e30a:	fed5fae3          	bgeu	a1,a3,2005e2fe <prvInsertTimerInActiveList+0x26>
2005e30e:	00478593          	addi	a1,a5,4
2005e312:	fa01a503          	lw	a0,-96(gp) # 2006621c <pxCurrentTimerList>
2005e316:	b7cd                	c.j	2005e2f8 <prvInsertTimerInActiveList+0x20>

2005e318 <xTimerCreateTimerTask>:
2005e318:	1101                	c.addi	sp,-32
2005e31a:	ce06                	c.swsp	ra,28(sp)
2005e31c:	f55ff0ef          	jal	ra,2005e270 <prvCheckForValidListAndQueue>
2005e320:	fac1a783          	lw	a5,-84(gp) # 20066228 <xTimerQueue>
2005e324:	4501                	c.li	a0,0
2005e326:	cb9d                	c.beqz	a5,2005e35c <xTimerCreateTimerTask+0x44>
2005e328:	0070                	c.addi4spn	a2,sp,12
2005e32a:	002c                	c.addi4spn	a1,sp,8
2005e32c:	0048                	c.addi4spn	a0,sp,4
2005e32e:	c202                	c.swsp	zero,4(sp)
2005e330:	c402                	c.swsp	zero,8(sp)
2005e332:	962fe0ef          	jal	ra,2005c494 <vApplicationGetTimerTaskMemory>
2005e336:	47a2                	c.lwsp	a5,8(sp)
2005e338:	4812                	c.lwsp	a6,4(sp)
2005e33a:	4632                	c.lwsp	a2,12(sp)
2005e33c:	200635b7          	lui	a1,0x20063
2005e340:	2005e537          	lui	a0,0x2005e
2005e344:	4729                	c.li	a4,10
2005e346:	4681                	c.li	a3,0
2005e348:	03858593          	addi	a1,a1,56 # 20063038 <__func__.1+0x30>
2005e34c:	53c50513          	addi	a0,a0,1340 # 2005e53c <prvTimerTask>
2005e350:	ff8ff0ef          	jal	ra,2005db48 <xTaskCreateStatic>
2005e354:	faa1a823          	sw	a0,-80(gp) # 2006622c <xTimerTaskHandle>
2005e358:	00a03533          	sltu	a0,zero,a0
2005e35c:	40f2                	c.lwsp	ra,28(sp)
2005e35e:	6105                	c.addi16sp	sp,32
2005e360:	8082                	c.jr	ra

2005e362 <xTimerCreate>:
2005e362:	1101                	c.addi	sp,-32
2005e364:	c256                	c.swsp	s5,4(sp)
2005e366:	8aaa                	c.mv	s5,a0
2005e368:	02c00513          	addi	a0,zero,44
2005e36c:	cc22                	c.swsp	s0,24(sp)
2005e36e:	ca26                	c.swsp	s1,20(sp)
2005e370:	c84a                	c.swsp	s2,16(sp)
2005e372:	c64e                	c.swsp	s3,12(sp)
2005e374:	c452                	c.swsp	s4,8(sp)
2005e376:	ce06                	c.swsp	ra,28(sp)
2005e378:	8a2e                	c.mv	s4,a1
2005e37a:	84b2                	c.mv	s1,a2
2005e37c:	89b6                	c.mv	s3,a3
2005e37e:	893a                	c.mv	s2,a4
2005e380:	dccfc0ef          	jal	ra,2005a94c <pvPortMalloc>
2005e384:	842a                	c.mv	s0,a0
2005e386:	c905                	c.beqz	a0,2005e3b6 <xTimerCreate+0x54>
2005e388:	02050423          	sb	zero,40(a0)
2005e38c:	ee5ff0ef          	jal	ra,2005e270 <prvCheckForValidListAndQueue>
2005e390:	00440513          	addi	a0,s0,4
2005e394:	01542023          	sw	s5,0(s0)
2005e398:	01442c23          	sw	s4,24(s0)
2005e39c:	01342e23          	sw	s3,28(s0)
2005e3a0:	03242023          	sw	s2,32(s0)
2005e3a4:	ff6fc0ef          	jal	ra,2005ab9a <vListInitialiseItem>
2005e3a8:	c499                	c.beqz	s1,2005e3b6 <xTimerCreate+0x54>
2005e3aa:	02844783          	lbu	a5,40(s0)
2005e3ae:	0047e793          	ori	a5,a5,4
2005e3b2:	02f40423          	sb	a5,40(s0)
2005e3b6:	40f2                	c.lwsp	ra,28(sp)
2005e3b8:	8522                	c.mv	a0,s0
2005e3ba:	4462                	c.lwsp	s0,24(sp)
2005e3bc:	44d2                	c.lwsp	s1,20(sp)
2005e3be:	4942                	c.lwsp	s2,16(sp)
2005e3c0:	49b2                	c.lwsp	s3,12(sp)
2005e3c2:	4a22                	c.lwsp	s4,8(sp)
2005e3c4:	4a92                	c.lwsp	s5,4(sp)
2005e3c6:	6105                	c.addi16sp	sp,32
2005e3c8:	8082                	c.jr	ra

2005e3ca <xTimerGenericCommand>:
2005e3ca:	7139                	c.addi16sp	sp,-64
2005e3cc:	dc22                	c.swsp	s0,56(sp)
2005e3ce:	da26                	c.swsp	s1,52(sp)
2005e3d0:	d84a                	c.swsp	s2,48(sp)
2005e3d2:	d452                	c.swsp	s4,40(sp)
2005e3d4:	d256                	c.swsp	s5,36(sp)
2005e3d6:	de06                	c.swsp	ra,60(sp)
2005e3d8:	d64e                	c.swsp	s3,44(sp)
2005e3da:	842a                	c.mv	s0,a0
2005e3dc:	84ae                	c.mv	s1,a1
2005e3de:	8932                	c.mv	s2,a2
2005e3e0:	8a36                	c.mv	s4,a3
2005e3e2:	8aba                	c.mv	s5,a4
2005e3e4:	c38ff0ef          	jal	ra,2005d81c <xTaskGetCurrentTaskHandle>
2005e3e8:	fb01a783          	lw	a5,-80(gp) # 2006622c <xTimerTaskHandle>
2005e3ec:	06a79163          	bne	a5,a0,2005e44e <xTimerGenericCommand+0x84>
2005e3f0:	ffd48793          	addi	a5,s1,-3
2005e3f4:	4705                	c.li	a4,1
2005e3f6:	04f76c63          	bltu	a4,a5,2005e44e <xTimerGenericCommand+0x84>
2005e3fa:	d23ff0ef          	jal	ra,2005e11c <xTaskGetTickCount>
2005e3fe:	485c                	c.lw	a5,20(s0)
2005e400:	86aa                	c.mv	a3,a0
2005e402:	c799                	c.beqz	a5,2005e410 <xTimerGenericCommand+0x46>
2005e404:	c62a                	c.swsp	a0,12(sp)
2005e406:	00440513          	addi	a0,s0,4
2005e40a:	fdafc0ef          	jal	ra,2005abe4 <uxListRemove>
2005e40e:	46b2                	c.lwsp	a3,12(sp)
2005e410:	4711                	c.li	a4,4
2005e412:	02844783          	lbu	a5,40(s0)
2005e416:	00e48f63          	beq	s1,a4,2005e434 <xTimerGenericCommand+0x6a>
2005e41a:	9bf9                	c.andi	a5,-2
2005e41c:	02f40423          	sb	a5,40(s0)
2005e420:	4505                	c.li	a0,1
2005e422:	50f2                	c.lwsp	ra,60(sp)
2005e424:	5462                	c.lwsp	s0,56(sp)
2005e426:	54d2                	c.lwsp	s1,52(sp)
2005e428:	5942                	c.lwsp	s2,48(sp)
2005e42a:	59b2                	c.lwsp	s3,44(sp)
2005e42c:	5a22                	c.lwsp	s4,40(sp)
2005e42e:	5a92                	c.lwsp	s5,36(sp)
2005e430:	6121                	c.addi16sp	sp,64
2005e432:	8082                	c.jr	ra
2005e434:	0017e793          	ori	a5,a5,1
2005e438:	02f40423          	sb	a5,40(s0)
2005e43c:	01242c23          	sw	s2,24(s0)
2005e440:	8636                	c.mv	a2,a3
2005e442:	00d905b3          	add	a1,s2,a3
2005e446:	8522                	c.mv	a0,s0
2005e448:	e91ff0ef          	jal	ra,2005e2d8 <prvInsertTimerInActiveList>
2005e44c:	bfd1                	c.j	2005e420 <xTimerGenericCommand+0x56>
2005e44e:	fac18993          	addi	s3,gp,-84 # 20066228 <xTimerQueue>
2005e452:	0009a783          	lw	a5,0(s3)
2005e456:	4501                	c.li	a0,0
2005e458:	d7e9                	c.beqz	a5,2005e422 <xTimerGenericCommand+0x58>
2005e45a:	ca26                	c.swsp	s1,20(sp)
2005e45c:	cc4a                	c.swsp	s2,24(sp)
2005e45e:	ce22                	c.swsp	s0,28(sp)
2005e460:	4715                	c.li	a4,5
2005e462:	02974163          	blt	a4,s1,2005e484 <xTimerGenericCommand+0xba>
2005e466:	bbcff0ef          	jal	ra,2005d822 <xTaskGetSchedulerState>
2005e46a:	872a                	c.mv	a4,a0
2005e46c:	4789                	c.li	a5,2
2005e46e:	0009a503          	lw	a0,0(s3)
2005e472:	4681                	c.li	a3,0
2005e474:	8656                	c.mv	a2,s5
2005e476:	00f70363          	beq	a4,a5,2005e47c <xTimerGenericCommand+0xb2>
2005e47a:	4601                	c.li	a2,0
2005e47c:	084c                	c.addi4spn	a1,sp,20
2005e47e:	a7afe0ef          	jal	ra,2005c6f8 <xQueueGenericSend>
2005e482:	b745                	c.j	2005e422 <xTimerGenericCommand+0x58>
2005e484:	4681                	c.li	a3,0
2005e486:	8652                	c.mv	a2,s4
2005e488:	084c                	c.addi4spn	a1,sp,20
2005e48a:	853e                	c.mv	a0,a5
2005e48c:	b70fe0ef          	jal	ra,2005c7fc <xQueueGenericSendFromISR>
2005e490:	bf49                	c.j	2005e422 <xTimerGenericCommand+0x58>

2005e492 <prvSampleTimeNow>:
2005e492:	7179                	c.addi16sp	sp,-48
2005e494:	d226                	c.swsp	s1,36(sp)
2005e496:	d04a                	c.swsp	s2,32(sp)
2005e498:	ce4e                	c.swsp	s3,28(sp)
2005e49a:	d606                	c.swsp	ra,44(sp)
2005e49c:	d422                	c.swsp	s0,40(sp)
2005e49e:	cc52                	c.swsp	s4,24(sp)
2005e4a0:	ca56                	c.swsp	s5,20(sp)
2005e4a2:	89aa                	c.mv	s3,a0
2005e4a4:	c79ff0ef          	jal	ra,2005e11c <xTaskGetTickCount>
2005e4a8:	fa81a783          	lw	a5,-88(gp) # 20066224 <xLastTime.2>
2005e4ac:	84aa                	c.mv	s1,a0
2005e4ae:	fa818913          	addi	s2,gp,-88 # 20066224 <xLastTime.2>
2005e4b2:	08f57263          	bgeu	a0,a5,2005e536 <prvSampleTimeNow+0xa4>
2005e4b6:	fa018a13          	addi	s4,gp,-96 # 2006621c <pxCurrentTimerList>
2005e4ba:	000a2703          	lw	a4,0(s4)
2005e4be:	431c                	c.lw	a5,0(a4)
2005e4c0:	e795                	c.bnez	a5,2005e4ec <prvSampleTimeNow+0x5a>
2005e4c2:	fa418793          	addi	a5,gp,-92 # 20066220 <pxOverflowTimerList>
2005e4c6:	4394                	c.lw	a3,0(a5)
2005e4c8:	c398                	c.sw	a4,0(a5)
2005e4ca:	4785                	c.li	a5,1
2005e4cc:	00da2023          	sw	a3,0(s4)
2005e4d0:	00f9a023          	sw	a5,0(s3)
2005e4d4:	50b2                	c.lwsp	ra,44(sp)
2005e4d6:	5422                	c.lwsp	s0,40(sp)
2005e4d8:	00992023          	sw	s1,0(s2)
2005e4dc:	49f2                	c.lwsp	s3,28(sp)
2005e4de:	5902                	c.lwsp	s2,32(sp)
2005e4e0:	4a62                	c.lwsp	s4,24(sp)
2005e4e2:	4ad2                	c.lwsp	s5,20(sp)
2005e4e4:	8526                	c.mv	a0,s1
2005e4e6:	5492                	c.lwsp	s1,36(sp)
2005e4e8:	6145                	c.addi16sp	sp,48
2005e4ea:	8082                	c.jr	ra
2005e4ec:	475c                	c.lw	a5,12(a4)
2005e4ee:	47c0                	c.lw	s0,12(a5)
2005e4f0:	4390                	c.lw	a2,0(a5)
2005e4f2:	00440593          	addi	a1,s0,4
2005e4f6:	852e                	c.mv	a0,a1
2005e4f8:	c632                	c.swsp	a2,12(sp)
2005e4fa:	c42e                	c.swsp	a1,8(sp)
2005e4fc:	ee8fc0ef          	jal	ra,2005abe4 <uxListRemove>
2005e500:	501c                	c.lw	a5,32(s0)
2005e502:	8522                	c.mv	a0,s0
2005e504:	9782                	c.jalr	a5
2005e506:	02844783          	lbu	a5,40(s0)
2005e50a:	45a2                	c.lwsp	a1,8(sp)
2005e50c:	4632                	c.lwsp	a2,12(sp)
2005e50e:	8b91                	c.andi	a5,4
2005e510:	d3dd                	c.beqz	a5,2005e4b6 <prvSampleTimeNow+0x24>
2005e512:	4c1c                	c.lw	a5,24(s0)
2005e514:	97b2                	c.add	a5,a2
2005e516:	00f67963          	bgeu	a2,a5,2005e528 <prvSampleTimeNow+0x96>
2005e51a:	000a2503          	lw	a0,0(s4)
2005e51e:	c05c                	c.sw	a5,4(s0)
2005e520:	c800                	c.sw	s0,16(s0)
2005e522:	e94fc0ef          	jal	ra,2005abb6 <vListInsert>
2005e526:	bf41                	c.j	2005e4b6 <prvSampleTimeNow+0x24>
2005e528:	4701                	c.li	a4,0
2005e52a:	4681                	c.li	a3,0
2005e52c:	4581                	c.li	a1,0
2005e52e:	8522                	c.mv	a0,s0
2005e530:	e9bff0ef          	jal	ra,2005e3ca <xTimerGenericCommand>
2005e534:	b749                	c.j	2005e4b6 <prvSampleTimeNow+0x24>
2005e536:	0009a023          	sw	zero,0(s3)
2005e53a:	bf69                	c.j	2005e4d4 <prvSampleTimeNow+0x42>

2005e53c <prvTimerTask>:
2005e53c:	7139                	c.addi16sp	sp,-64
2005e53e:	d84a                	c.swsp	s2,48(sp)
2005e540:	20063937          	lui	s2,0x20063
2005e544:	d64e                	c.swsp	s3,44(sp)
2005e546:	d256                	c.swsp	s5,36(sp)
2005e548:	d05a                	c.swsp	s6,32(sp)
2005e54a:	de06                	c.swsp	ra,60(sp)
2005e54c:	dc22                	c.swsp	s0,56(sp)
2005e54e:	da26                	c.swsp	s1,52(sp)
2005e550:	d452                	c.swsp	s4,40(sp)
2005e552:	ce5e                	c.swsp	s7,28(sp)
2005e554:	cc62                	c.swsp	s8,24(sp)
2005e556:	04090913          	addi	s2,s2,64 # 20063040 <__func__.1+0x38>
2005e55a:	fa01a783          	lw	a5,-96(gp) # 2006621c <pxCurrentTimerList>
2005e55e:	fa018c13          	addi	s8,gp,-96 # 2006621c <pxCurrentTimerList>
2005e562:	4405                	c.li	s0,1
2005e564:	4384                	c.lw	s1,0(a5)
2005e566:	c481                	c.beqz	s1,2005e56e <prvTimerTask+0x32>
2005e568:	47dc                	c.lw	a5,12(a5)
2005e56a:	4401                	c.li	s0,0
2005e56c:	4384                	c.lw	s1,0(a5)
2005e56e:	f71fe0ef          	jal	ra,2005d4de <vTaskSuspendAll>
2005e572:	0048                	c.addi4spn	a0,sp,4
2005e574:	f1fff0ef          	jal	ra,2005e492 <prvSampleTimeNow>
2005e578:	4792                	c.lwsp	a5,4(sp)
2005e57a:	8baa                	c.mv	s7,a0
2005e57c:	fac18a13          	addi	s4,gp,-84 # 20066228 <xTimerQueue>
2005e580:	ebc5                	c.bnez	a5,2005e630 <prvTimerTask+0xf4>
2005e582:	e449                	c.bnez	s0,2005e60c <prvTimerTask+0xd0>
2005e584:	08956963          	bltu	a0,s1,2005e616 <prvTimerTask+0xda>
2005e588:	805ff0ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005e58c:	000c2783          	lw	a5,0(s8)
2005e590:	47dc                	c.lw	a5,12(a5)
2005e592:	47c0                	c.lw	s0,12(a5)
2005e594:	00440513          	addi	a0,s0,4
2005e598:	e4cfc0ef          	jal	ra,2005abe4 <uxListRemove>
2005e59c:	02844783          	lbu	a5,40(s0)
2005e5a0:	0047f713          	andi	a4,a5,4
2005e5a4:	c325                	c.beqz	a4,2005e604 <prvTimerTask+0xc8>
2005e5a6:	4c0c                	c.lw	a1,24(s0)
2005e5a8:	86a6                	c.mv	a3,s1
2005e5aa:	865e                	c.mv	a2,s7
2005e5ac:	95a6                	c.add	a1,s1
2005e5ae:	8522                	c.mv	a0,s0
2005e5b0:	d29ff0ef          	jal	ra,2005e2d8 <prvInsertTimerInActiveList>
2005e5b4:	c901                	c.beqz	a0,2005e5c4 <prvTimerTask+0x88>
2005e5b6:	4701                	c.li	a4,0
2005e5b8:	4681                	c.li	a3,0
2005e5ba:	8626                	c.mv	a2,s1
2005e5bc:	4581                	c.li	a1,0
2005e5be:	8522                	c.mv	a0,s0
2005e5c0:	e0bff0ef          	jal	ra,2005e3ca <xTimerGenericCommand>
2005e5c4:	501c                	c.lw	a5,32(s0)
2005e5c6:	8522                	c.mv	a0,s0
2005e5c8:	9782                	c.jalr	a5
2005e5ca:	44a5                	c.li	s1,9
2005e5cc:	000a2503          	lw	a0,0(s4)
2005e5d0:	4601                	c.li	a2,0
2005e5d2:	004c                	c.addi4spn	a1,sp,4
2005e5d4:	ae6fe0ef          	jal	ra,2005c8ba <xQueueReceive>
2005e5d8:	d149                	c.beqz	a0,2005e55a <prvTimerTask+0x1e>
2005e5da:	4792                	c.lwsp	a5,4(sp)
2005e5dc:	fe07c8e3          	blt	a5,zero,2005e5cc <prvTimerTask+0x90>
2005e5e0:	4432                	c.lwsp	s0,12(sp)
2005e5e2:	485c                	c.lw	a5,20(s0)
2005e5e4:	c789                	c.beqz	a5,2005e5ee <prvTimerTask+0xb2>
2005e5e6:	00440513          	addi	a0,s0,4
2005e5ea:	dfafc0ef          	jal	ra,2005abe4 <uxListRemove>
2005e5ee:	850a                	c.mv	a0,sp
2005e5f0:	ea3ff0ef          	jal	ra,2005e492 <prvSampleTimeNow>
2005e5f4:	4792                	c.lwsp	a5,4(sp)
2005e5f6:	862a                	c.mv	a2,a0
2005e5f8:	fcf4eae3          	bltu	s1,a5,2005e5cc <prvTimerTask+0x90>
2005e5fc:	078a                	c.slli	a5,0x2
2005e5fe:	97ca                	c.add	a5,s2
2005e600:	439c                	c.lw	a5,0(a5)
2005e602:	8782                	c.jr	a5
2005e604:	9bf9                	c.andi	a5,-2
2005e606:	02f40423          	sb	a5,40(s0)
2005e60a:	bf6d                	c.j	2005e5c4 <prvTimerTask+0x88>
2005e60c:	fa41a783          	lw	a5,-92(gp) # 20066220 <pxOverflowTimerList>
2005e610:	4380                	c.lw	s0,0(a5)
2005e612:	00143413          	sltiu	s0,s0,1
2005e616:	000a2503          	lw	a0,0(s4)
2005e61a:	8622                	c.mv	a2,s0
2005e61c:	417485b3          	sub	a1,s1,s7
2005e620:	d12fe0ef          	jal	ra,2005cb32 <vQueueWaitForMessageRestricted>
2005e624:	f68ff0ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005e628:	f14d                	c.bnez	a0,2005e5ca <prvTimerTask+0x8e>
2005e62a:	00000073          	ecall
2005e62e:	bf71                	c.j	2005e5ca <prvTimerTask+0x8e>
2005e630:	f5cff0ef          	jal	ra,2005dd8c <xTaskResumeAll>
2005e634:	bf59                	c.j	2005e5ca <prvTimerTask+0x8e>
2005e636:	02844783          	lbu	a5,40(s0)
2005e63a:	4c0c                	c.lw	a1,24(s0)
2005e63c:	8522                	c.mv	a0,s0
2005e63e:	0017e793          	ori	a5,a5,1
2005e642:	02f40423          	sb	a5,40(s0)
2005e646:	46a2                	c.lwsp	a3,8(sp)
2005e648:	95b6                	c.add	a1,a3
2005e64a:	c8fff0ef          	jal	ra,2005e2d8 <prvInsertTimerInActiveList>
2005e64e:	dd3d                	c.beqz	a0,2005e5cc <prvTimerTask+0x90>
2005e650:	501c                	c.lw	a5,32(s0)
2005e652:	8522                	c.mv	a0,s0
2005e654:	9782                	c.jalr	a5
2005e656:	02844783          	lbu	a5,40(s0)
2005e65a:	8b91                	c.andi	a5,4
2005e65c:	dba5                	c.beqz	a5,2005e5cc <prvTimerTask+0x90>
2005e65e:	4c1c                	c.lw	a5,24(s0)
2005e660:	4622                	c.lwsp	a2,8(sp)
2005e662:	4701                	c.li	a4,0
2005e664:	4681                	c.li	a3,0
2005e666:	963e                	c.add	a2,a5
2005e668:	4581                	c.li	a1,0
2005e66a:	8522                	c.mv	a0,s0
2005e66c:	d5fff0ef          	jal	ra,2005e3ca <xTimerGenericCommand>
2005e670:	bfb1                	c.j	2005e5cc <prvTimerTask+0x90>
2005e672:	02844783          	lbu	a5,40(s0)
2005e676:	9bf9                	c.andi	a5,-2
2005e678:	02f40423          	sb	a5,40(s0)
2005e67c:	bf81                	c.j	2005e5cc <prvTimerTask+0x90>
2005e67e:	02844783          	lbu	a5,40(s0)
2005e682:	86aa                	c.mv	a3,a0
2005e684:	0017e793          	ori	a5,a5,1
2005e688:	02f40423          	sb	a5,40(s0)
2005e68c:	45a2                	c.lwsp	a1,8(sp)
2005e68e:	cc0c                	c.sw	a1,24(s0)
2005e690:	95aa                	c.add	a1,a0
2005e692:	8522                	c.mv	a0,s0
2005e694:	c45ff0ef          	jal	ra,2005e2d8 <prvInsertTimerInActiveList>
2005e698:	bf15                	c.j	2005e5cc <prvTimerTask+0x90>
2005e69a:	02844783          	lbu	a5,40(s0)
2005e69e:	0027f713          	andi	a4,a5,2
2005e6a2:	fb71                	c.bnez	a4,2005e676 <prvTimerTask+0x13a>
2005e6a4:	8522                	c.mv	a0,s0
2005e6a6:	b84fc0ef          	jal	ra,2005aa2a <vPortFree>
2005e6aa:	b70d                	c.j	2005e5cc <prvTimerTask+0x90>

2005e6ac <wifi_fast_connect_enable>:
2005e6ac:	fa01ae23          	sw	zero,-68(gp) # 20066238 <p_wifi_do_fast_connect>
2005e6b0:	fa01ac23          	sw	zero,-72(gp) # 20066234 <p_store_fast_connect_info>
2005e6b4:	8082                	c.jr	ra

2005e6b6 <rtw_indicate_event_handle>:
2005e6b6:	4759                	c.li	a4,22
2005e6b8:	04a74e63          	blt	a4,a0,2005e714 <rtw_indicate_event_handle+0x5e>
2005e6bc:	1101                	c.addi	sp,-32
2005e6be:	20068737          	lui	a4,0x20068
2005e6c2:	ca26                	c.swsp	s1,20(sp)
2005e6c4:	c84a                	c.swsp	s2,16(sp)
2005e6c6:	00451493          	slli	s1,a0,0x4
2005e6ca:	892e                	c.mv	s2,a1
2005e6cc:	85b2                	c.mv	a1,a2
2005e6ce:	8636                	c.mv	a2,a3
2005e6d0:	dd870693          	addi	a3,a4,-552 # 20067dd8 <event_callback_list>
2005e6d4:	cc22                	c.swsp	s0,24(sp)
2005e6d6:	ce06                	c.swsp	ra,28(sp)
2005e6d8:	009687b3          	add	a5,a3,s1
2005e6dc:	0007a803          	lw	a6,0(a5)
2005e6e0:	dd870413          	addi	s0,a4,-552
2005e6e4:	00080963          	beq	a6,zero,2005e6f6 <rtw_indicate_event_handle+0x40>
2005e6e8:	43d4                	c.lw	a3,4(a5)
2005e6ea:	c632                	c.swsp	a2,12(sp)
2005e6ec:	c42e                	c.swsp	a1,8(sp)
2005e6ee:	854a                	c.mv	a0,s2
2005e6f0:	9802                	c.jalr	a6
2005e6f2:	4632                	c.lwsp	a2,12(sp)
2005e6f4:	45a2                	c.lwsp	a1,8(sp)
2005e6f6:	009407b3          	add	a5,s0,s1
2005e6fa:	4798                	c.lw	a4,8(a5)
2005e6fc:	4501                	c.li	a0,0
2005e6fe:	c709                	c.beqz	a4,2005e708 <rtw_indicate_event_handle+0x52>
2005e700:	47d4                	c.lw	a3,12(a5)
2005e702:	854a                	c.mv	a0,s2
2005e704:	9702                	c.jalr	a4
2005e706:	4501                	c.li	a0,0
2005e708:	40f2                	c.lwsp	ra,28(sp)
2005e70a:	4462                	c.lwsp	s0,24(sp)
2005e70c:	44d2                	c.lwsp	s1,20(sp)
2005e70e:	4942                	c.lwsp	s2,16(sp)
2005e710:	6105                	c.addi16sp	sp,32
2005e712:	8082                	c.jr	ra
2005e714:	5579                	c.li	a0,-2
2005e716:	8082                	c.jr	ra

2005e718 <wifi_indication>:
2005e718:	472d                	c.li	a4,11
2005e71a:	00e51863          	bne	a0,a4,2005e72a <wifi_indication+0x12>
2005e71e:	fc01a703          	lw	a4,-64(gp) # 2006623c <p_wifi_joinstatus_internal_callback>
2005e722:	c711                	c.beqz	a4,2005e72e <wifi_indication+0x16>
2005e724:	0ff6f513          	andi	a0,a3,255
2005e728:	8702                	c.jr	a4
2005e72a:	f8dff06f          	jal	zero,2005e6b6 <rtw_indicate_event_handle>
2005e72e:	8082                	c.jr	ra

2005e730 <_init_thread>:
2005e730:	1141                	c.addi	sp,-16
2005e732:	c422                	c.swsp	s0,8(sp)
2005e734:	c606                	c.swsp	ra,12(sp)
2005e736:	4100c437          	lui	s0,0x4100c
2005e73a:	08442783          	lw	a5,132(s0) # 4100c084 <__km4_bd_boot_download_addr__+0x10ffa084>
2005e73e:	08440713          	addi	a4,s0,132
2005e742:	8b85                	c.andi	a5,1
2005e744:	c7b1                	c.beqz	a5,2005e790 <_init_thread+0x60>
2005e746:	431c                	c.lw	a5,0(a4)
2005e748:	4505                	c.li	a0,1
2005e74a:	9bf9                	c.andi	a5,-2
2005e74c:	c31c                	c.sw	a5,0(a4)
2005e74e:	0fd010ef          	jal	ra,2006004a <wifi_on>
2005e752:	4505                	c.li	a0,1
2005e754:	189010ef          	jal	ra,200600dc <wifi_config_autoreconnect>
2005e758:	f8ffc0ef          	jal	ra,2005b6e6 <rtos_mem_get_free_heap_size>
2005e75c:	20063737          	lui	a4,0x20063
2005e760:	200636b7          	lui	a3,0x20063
2005e764:	200635b7          	lui	a1,0x20063
2005e768:	882a                	c.mv	a6,a0
2005e76a:	03500793          	addi	a5,zero,53
2005e76e:	4511                	c.li	a0,4
2005e770:	0ac70713          	addi	a4,a4,172 # 200630ac <__FUNCTION__.0>
2005e774:	06868693          	addi	a3,a3,104 # 20063068 <__func__.1+0x60>
2005e778:	04900613          	addi	a2,zero,73
2005e77c:	08458593          	addi	a1,a1,132 # 20063084 <__func__.1+0x7c>
2005e780:	f38fc0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005e784:	4422                	c.lwsp	s0,8(sp)
2005e786:	40b2                	c.lwsp	ra,12(sp)
2005e788:	4501                	c.li	a0,0
2005e78a:	0141                	c.addi	sp,16
2005e78c:	be6fd06f          	jal	zero,2005bb72 <rtos_task_delete>
2005e790:	4505                	c.li	a0,1
2005e792:	bf2fd0ef          	jal	ra,2005bb84 <rtos_time_delay_ms>
2005e796:	b755                	c.j	2005e73a <_init_thread+0xa>

2005e798 <wlan_initialize>:
2005e798:	1141                	c.addi	sp,-16
2005e79a:	c606                	c.swsp	ra,12(sp)
2005e79c:	01d010ef          	jal	ra,2005ffb8 <wifi_set_rom2flash>
2005e7a0:	0ec020ef          	jal	ra,2006088c <inic_host_init>
2005e7a4:	4505                	c.li	a0,1
2005e7a6:	f07ff0ef          	jal	ra,2005e6ac <wifi_fast_connect_enable>
2005e7aa:	6705                	c.lui	a4,0x1
2005e7ac:	2005e637          	lui	a2,0x2005e
2005e7b0:	200635b7          	lui	a1,0x20063
2005e7b4:	4789                	c.li	a5,2
2005e7b6:	40070713          	addi	a4,a4,1024 # 1400 <CPU_ClkGet+0x8ea>
2005e7ba:	4681                	c.li	a3,0
2005e7bc:	73060613          	addi	a2,a2,1840 # 2005e730 <_init_thread>
2005e7c0:	08c58593          	addi	a1,a1,140 # 2006308c <__func__.1+0x84>
2005e7c4:	4501                	c.li	a0,0
2005e7c6:	b88fd0ef          	jal	ra,2005bb4e <rtos_task_create>
2005e7ca:	c105                	c.beqz	a0,2005e7ea <wlan_initialize+0x52>
2005e7cc:	40b2                	c.lwsp	ra,12(sp)
2005e7ce:	200636b7          	lui	a3,0x20063
2005e7d2:	200635b7          	lui	a1,0x20063
2005e7d6:	09468693          	addi	a3,a3,148 # 20063094 <__func__.1+0x8c>
2005e7da:	04500613          	addi	a2,zero,69
2005e7de:	08458593          	addi	a1,a1,132 # 20063084 <__func__.1+0x7c>
2005e7e2:	4509                	c.li	a0,2
2005e7e4:	0141                	c.addi	sp,16
2005e7e6:	ed2fc06f          	jal	zero,2005aeb8 <rtk_log_write>
2005e7ea:	40b2                	c.lwsp	ra,12(sp)
2005e7ec:	0141                	c.addi	sp,16
2005e7ee:	8082                	c.jr	ra

2005e7f0 <ChipInfo_GetSocName_ToBuf>:
2005e7f0:	7139                	c.addi16sp	sp,-64
2005e7f2:	da26                	c.swsp	s1,52(sp)
2005e7f4:	84ae                	c.mv	s1,a1
2005e7f6:	200635b7          	lui	a1,0x20063
2005e7fa:	d84a                	c.swsp	s2,48(sp)
2005e7fc:	4625                	c.li	a2,9
2005e7fe:	892a                	c.mv	s2,a0
2005e800:	1bb58593          	addi	a1,a1,443 # 200631bb <__FUNCTION__.0+0x10f>
2005e804:	0068                	c.addi4spn	a0,sp,12
2005e806:	dc22                	c.swsp	s0,56(sp)
2005e808:	de06                	c.swsp	ra,60(sp)
2005e80a:	d64e                	c.swsp	s3,44(sp)
2005e80c:	d452                	c.swsp	s4,40(sp)
2005e80e:	dffab097          	auipc	ra,0xdffab
2005e812:	2c2080e7          	jalr	ra,706(ra) # 9ad0 <__wrap_memcpy>
2005e816:	00010aa3          	sb	zero,21(sp)
2005e81a:	4401                	c.li	s0,0
2005e81c:	c899                	c.beqz	s1,2005e832 <ChipInfo_GetSocName_ToBuf+0x42>
2005e81e:	20063637          	lui	a2,0x20063
2005e822:	0074                	c.addi4spn	a3,sp,12
2005e824:	1ec60613          	addi	a2,a2,492 # 200631ec <__FUNCTION__.0+0x140>
2005e828:	85a6                	c.mv	a1,s1
2005e82a:	854a                	c.mv	a0,s2
2005e82c:	27b000ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e830:	842a                	c.mv	s0,a0
2005e832:	7f000993          	addi	s3,zero,2032
2005e836:	20063a37          	lui	s4,0x20063
2005e83a:	00b10593          	addi	a1,sp,11
2005e83e:	854e                	c.mv	a0,s3
2005e840:	c5dfa0ef          	jal	ra,2005949c <OTP_Read8>
2005e844:	e131                	c.bnez	a0,2005e888 <ChipInfo_GetSocName_ToBuf+0x98>
2005e846:	200636b7          	lui	a3,0x20063
2005e84a:	200635b7          	lui	a1,0x20063
2005e84e:	1f468693          	addi	a3,a3,500 # 200631f4 <__FUNCTION__.0+0x148>
2005e852:	04500613          	addi	a2,zero,69
2005e856:	10058593          	addi	a1,a1,256 # 20063100 <__FUNCTION__.0+0x54>
2005e85a:	4509                	c.li	a0,2
2005e85c:	e5cfc0ef          	jal	ra,2005aeb8 <rtk_log_write>
2005e860:	00947c63          	bgeu	s0,s1,2005e878 <ChipInfo_GetSocName_ToBuf+0x88>
2005e864:	20063637          	lui	a2,0x20063
2005e868:	a9460613          	addi	a2,a2,-1388 # 20062a94 <__FUNCTION__.0+0x20c>
2005e86c:	408485b3          	sub	a1,s1,s0
2005e870:	00890533          	add	a0,s2,s0
2005e874:	233000ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e878:	50f2                	c.lwsp	ra,60(sp)
2005e87a:	5462                	c.lwsp	s0,56(sp)
2005e87c:	54d2                	c.lwsp	s1,52(sp)
2005e87e:	5942                	c.lwsp	s2,48(sp)
2005e880:	59b2                	c.lwsp	s3,44(sp)
2005e882:	5a22                	c.lwsp	s4,40(sp)
2005e884:	6121                	c.addi16sp	sp,64
2005e886:	8082                	c.jr	ra
2005e888:	00947d63          	bgeu	s0,s1,2005e8a2 <ChipInfo_GetSocName_ToBuf+0xb2>
2005e88c:	00b14683          	lbu	a3,11(sp)
2005e890:	408485b3          	sub	a1,s1,s0
2005e894:	00890533          	add	a0,s2,s0
2005e898:	20ca0613          	addi	a2,s4,524 # 2006320c <__FUNCTION__.0+0x160>
2005e89c:	20b000ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e8a0:	942a                	c.add	s0,a0
2005e8a2:	0985                	c.addi	s3,1
2005e8a4:	80098793          	addi	a5,s3,-2048
2005e8a8:	fbc9                	c.bnez	a5,2005e83a <ChipInfo_GetSocName_ToBuf+0x4a>
2005e8aa:	bf5d                	c.j	2005e860 <ChipInfo_GetSocName_ToBuf+0x70>

2005e8ac <ChipInfo_GetLibVersion_ToBuf>:
2005e8ac:	1101                	c.addi	sp,-32
2005e8ae:	cc22                	c.swsp	s0,24(sp)
2005e8b0:	ca26                	c.swsp	s1,20(sp)
2005e8b2:	c64e                	c.swsp	s3,12(sp)
2005e8b4:	ce06                	c.swsp	ra,28(sp)
2005e8b6:	c84a                	c.swsp	s2,16(sp)
2005e8b8:	c452                	c.swsp	s4,8(sp)
2005e8ba:	c256                	c.swsp	s5,4(sp)
2005e8bc:	c05a                	c.swsp	s6,0(sp)
2005e8be:	4100c7b7          	lui	a5,0x4100c
2005e8c2:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005e8c6:	89aa                	c.mv	s3,a0
2005e8c8:	84ae                	c.mv	s1,a1
2005e8ca:	4401                	c.li	s0,0
2005e8cc:	c1b5                	c.beqz	a1,2005e930 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005e8ce:	8399                	c.srli	a5,0x6
2005e8d0:	8b8d                	c.andi	a5,3
2005e8d2:	e7d9                	c.bnez	a5,2005e960 <ChipInfo_GetLibVersion_ToBuf+0xb4>
2005e8d4:	200636b7          	lui	a3,0x20063
2005e8d8:	e6868693          	addi	a3,a3,-408 # 20062e68 <__FUNCTION__.0+0x1c>
2005e8dc:	20063637          	lui	a2,0x20063
2005e8e0:	21860613          	addi	a2,a2,536 # 20063218 <__FUNCTION__.0+0x16c>
2005e8e4:	85a6                	c.mv	a1,s1
2005e8e6:	854e                	c.mv	a0,s3
2005e8e8:	1bf000ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e8ec:	842a                	c.mv	s0,a0
2005e8ee:	04957163          	bgeu	a0,s1,2005e930 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005e8f2:	200636b7          	lui	a3,0x20063
2005e8f6:	20063637          	lui	a2,0x20063
2005e8fa:	40a485b3          	sub	a1,s1,a0
2005e8fe:	22c68693          	addi	a3,a3,556 # 2006322c <__FUNCTION__.0+0x180>
2005e902:	24060613          	addi	a2,a2,576 # 20063240 <__FUNCTION__.0+0x194>
2005e906:	954e                	c.add	a0,s3
2005e908:	19f000ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e90c:	942a                	c.add	s0,a0
2005e90e:	02947163          	bgeu	s0,s1,2005e930 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005e912:	200636b7          	lui	a3,0x20063
2005e916:	20063637          	lui	a2,0x20063
2005e91a:	408485b3          	sub	a1,s1,s0
2005e91e:	00898533          	add	a0,s3,s0
2005e922:	25468693          	addi	a3,a3,596 # 20063254 <__FUNCTION__.0+0x1a8>
2005e926:	29460613          	addi	a2,a2,660 # 20063294 <__FUNCTION__.0+0x1e8>
2005e92a:	17d000ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e92e:	942a                	c.add	s0,a0
2005e930:	20064937          	lui	s2,0x20064
2005e934:	20064a37          	lui	s4,0x20064
2005e938:	62890913          	addi	s2,s2,1576 # 20064628 <lib_pmc_git_rev>
2005e93c:	6b8a0a13          	addi	s4,s4,1720 # 200646b8 <__frame_dummy_init_array_entry>
2005e940:	20063ab7          	lui	s5,0x20063
2005e944:	20063b37          	lui	s6,0x20063
2005e948:	03496163          	bltu	s2,s4,2005e96a <ChipInfo_GetLibVersion_ToBuf+0xbe>
2005e94c:	40f2                	c.lwsp	ra,28(sp)
2005e94e:	4462                	c.lwsp	s0,24(sp)
2005e950:	44d2                	c.lwsp	s1,20(sp)
2005e952:	4942                	c.lwsp	s2,16(sp)
2005e954:	49b2                	c.lwsp	s3,12(sp)
2005e956:	4a22                	c.lwsp	s4,8(sp)
2005e958:	4a92                	c.lwsp	s5,4(sp)
2005e95a:	4b02                	c.lwsp	s6,0(sp)
2005e95c:	6105                	c.addi16sp	sp,32
2005e95e:	8082                	c.jr	ra
2005e960:	200636b7          	lui	a3,0x20063
2005e964:	21468693          	addi	a3,a3,532 # 20063214 <__FUNCTION__.0+0x168>
2005e968:	bf95                	c.j	2005e8dc <ChipInfo_GetLibVersion_ToBuf+0x30>
2005e96a:	00094683          	lbu	a3,0(s2)
2005e96e:	e28d                	c.bnez	a3,2005e990 <ChipInfo_GetLibVersion_ToBuf+0xe4>
2005e970:	00194783          	lbu	a5,1(s2)
2005e974:	0905                	c.addi	s2,1
2005e976:	dfed                	c.beqz	a5,2005e970 <ChipInfo_GetLibVersion_ToBuf+0xc4>
2005e978:	fc9478e3          	bgeu	s0,s1,2005e948 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005e97c:	a94b0613          	addi	a2,s6,-1388 # 20062a94 <__FUNCTION__.0+0x20c>
2005e980:	408485b3          	sub	a1,s1,s0
2005e984:	00898533          	add	a0,s3,s0
2005e988:	11f000ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e98c:	942a                	c.add	s0,a0
2005e98e:	bf6d                	c.j	2005e948 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005e990:	fa947ce3          	bgeu	s0,s1,2005e948 <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005e994:	2a4a8613          	addi	a2,s5,676 # 200632a4 <__FUNCTION__.0+0x1f8>
2005e998:	408485b3          	sub	a1,s1,s0
2005e99c:	00898533          	add	a0,s3,s0
2005e9a0:	0905                	c.addi	s2,1
2005e9a2:	105000ef          	jal	ra,2005f2a6 <DiagSnPrintf>
2005e9a6:	b7dd                	c.j	2005e98c <ChipInfo_GetLibVersion_ToBuf+0xe0>

2005e9a8 <ChipInfo_GetChipSram>:
2005e9a8:	1101                	c.addi	sp,-32
2005e9aa:	ce06                	c.swsp	ra,28(sp)
2005e9ac:	4100c7b7          	lui	a5,0x4100c
2005e9b0:	2107a783          	lw	a5,528(a5) # 4100c210 <__km4_bd_boot_download_addr__+0x10ffa210>
2005e9b4:	40400737          	lui	a4,0x40400
2005e9b8:	8ff9                	c.and	a5,a4
2005e9ba:	c39d                	c.beqz	a5,2005e9e0 <ChipInfo_GetChipSram+0x38>
2005e9bc:	00f10593          	addi	a1,sp,15
2005e9c0:	7fc00513          	addi	a0,zero,2044
2005e9c4:	ad9fa0ef          	jal	ra,2005949c <OTP_Read8>
2005e9c8:	00f14783          	lbu	a5,15(sp)
2005e9cc:	470d                	c.li	a4,3
2005e9ce:	8389                	c.srli	a5,0x2
2005e9d0:	8b8d                	c.andi	a5,3
2005e9d2:	00e79663          	bne	a5,a4,2005e9de <ChipInfo_GetChipSram+0x36>
2005e9d6:	4501                	c.li	a0,0
2005e9d8:	40f2                	c.lwsp	ra,28(sp)
2005e9da:	6105                	c.addi16sp	sp,32
2005e9dc:	8082                	c.jr	ra
2005e9de:	dfe5                	c.beqz	a5,2005e9d6 <ChipInfo_GetChipSram+0x2e>
2005e9e0:	00f10593          	addi	a1,sp,15
2005e9e4:	7fc00513          	addi	a0,zero,2044
2005e9e8:	ab5fa0ef          	jal	ra,2005949c <OTP_Read8>
2005e9ec:	00f14783          	lbu	a5,15(sp)
2005e9f0:	470d                	c.li	a4,3
2005e9f2:	8399                	c.srli	a5,0x6
2005e9f4:	00e78363          	beq	a5,a4,2005e9fa <ChipInfo_GetChipSram+0x52>
2005e9f8:	fff9                	c.bnez	a5,2005e9d6 <ChipInfo_GetChipSram+0x2e>
2005e9fa:	4505                	c.li	a0,1
2005e9fc:	bff1                	c.j	2005e9d8 <ChipInfo_GetChipSram+0x30>

2005e9fe <vTaskStatus>:
2005e9fe:	7139                	c.addi16sp	sp,-64
2005ea00:	de06                	c.swsp	ra,60(sp)
2005ea02:	dc22                	c.swsp	s0,56(sp)
2005ea04:	da26                	c.swsp	s1,52(sp)
2005ea06:	ffffe0ef          	jal	ra,2005da04 <vTaskEnterCritical>
2005ea0a:	e13fe0ef          	jal	ra,2005d81c <xTaskGetCurrentTaskHandle>
2005ea0e:	4695                	c.li	a3,5
2005ea10:	4605                	c.li	a2,1
2005ea12:	006c                	c.addi4spn	a1,sp,12
2005ea14:	842a                	c.mv	s0,a0
2005ea16:	200634b7          	lui	s1,0x20063
2005ea1a:	d12ff0ef          	jal	ra,2005df2c <vTaskGetInfo>
2005ea1e:	2a848513          	addi	a0,s1,680 # 200632a8 <__FUNCTION__.0+0x1fc>
2005ea22:	2d75                	c.jal	2005f0de <DiagPrintf>
2005ea24:	45c2                	c.lwsp	a1,16(sp)
2005ea26:	20063537          	lui	a0,0x20063
2005ea2a:	2cc50513          	addi	a0,a0,716 # 200632cc <__FUNCTION__.0+0x220>
2005ea2e:	2d45                	c.jal	2005f0de <DiagPrintf>
2005ea30:	45d2                	c.lwsp	a1,20(sp)
2005ea32:	20063537          	lui	a0,0x20063
2005ea36:	2e050513          	addi	a0,a0,736 # 200632e0 <__FUNCTION__.0+0x234>
2005ea3a:	2555                	c.jal	2005f0de <DiagPrintf>
2005ea3c:	20063537          	lui	a0,0x20063
2005ea40:	85a2                	c.mv	a1,s0
2005ea42:	2f050513          	addi	a0,a0,752 # 200632f0 <__FUNCTION__.0+0x244>
2005ea46:	2d61                	c.jal	2005f0de <DiagPrintf>
2005ea48:	01814583          	lbu	a1,24(sp)
2005ea4c:	200637b7          	lui	a5,0x20063
2005ea50:	41478793          	addi	a5,a5,1044 # 20063414 <TaskStateString.0>
2005ea54:	00259713          	slli	a4,a1,0x2
2005ea58:	97ba                	c.add	a5,a4
2005ea5a:	4390                	c.lw	a2,0(a5)
2005ea5c:	20063537          	lui	a0,0x20063
2005ea60:	30450513          	addi	a0,a0,772 # 20063304 <__FUNCTION__.0+0x258>
2005ea64:	2dad                	c.jal	2005f0de <DiagPrintf>
2005ea66:	5582                	c.lwsp	a1,32(sp)
2005ea68:	20063537          	lui	a0,0x20063
2005ea6c:	32050513          	addi	a0,a0,800 # 20063320 <__FUNCTION__.0+0x274>
2005ea70:	25bd                	c.jal	2005f0de <DiagPrintf>
2005ea72:	45f2                	c.lwsp	a1,28(sp)
2005ea74:	20063537          	lui	a0,0x20063
2005ea78:	33450513          	addi	a0,a0,820 # 20063334 <__FUNCTION__.0+0x288>
2005ea7c:	258d                	c.jal	2005f0de <DiagPrintf>
2005ea7e:	5592                	c.lwsp	a1,36(sp)
2005ea80:	20063537          	lui	a0,0x20063
2005ea84:	34c50513          	addi	a0,a0,844 # 2006334c <__FUNCTION__.0+0x2a0>
2005ea88:	2d99                	c.jal	2005f0de <DiagPrintf>
2005ea8a:	4474                	c.lw	a3,76(s0)
2005ea8c:	5818                	c.lw	a4,48(s0)
2005ea8e:	5622                	c.lwsp	a2,40(sp)
2005ea90:	400c                	c.lw	a1,0(s0)
2005ea92:	40e68733          	sub	a4,a3,a4
2005ea96:	8709                	c.srai	a4,0x2
2005ea98:	20063537          	lui	a0,0x20063
2005ea9c:	0705                	c.addi	a4,1
2005ea9e:	36450513          	addi	a0,a0,868 # 20063364 <__FUNCTION__.0+0x2b8>
2005eaa2:	2d35                	c.jal	2005f0de <DiagPrintf>
2005eaa4:	02c15583          	lhu	a1,44(sp)
2005eaa8:	20063537          	lui	a0,0x20063
2005eaac:	3b450513          	addi	a0,a0,948 # 200633b4 <__FUNCTION__.0+0x308>
2005eab0:	253d                	c.jal	2005f0de <DiagPrintf>
2005eab2:	2a848513          	addi	a0,s1,680
2005eab6:	2525                	c.jal	2005f0de <DiagPrintf>
2005eab8:	50f2                	c.lwsp	ra,60(sp)
2005eaba:	5462                	c.lwsp	s0,56(sp)
2005eabc:	54d2                	c.lwsp	s1,52(sp)
2005eabe:	6121                	c.addi16sp	sp,64
2005eac0:	8082                	c.jr	ra

2005eac2 <vTaskCrashCallback>:
2005eac2:	1141                	c.addi	sp,-16
2005eac4:	c606                	c.swsp	ra,12(sp)
2005eac6:	862fd0ef          	jal	ra,2005bb28 <rtos_sched_get_state>
2005eaca:	4785                	c.li	a5,1
2005eacc:	00f50663          	beq	a0,a5,2005ead8 <vTaskCrashCallback+0x16>
2005ead0:	40b2                	c.lwsp	ra,12(sp)
2005ead2:	0141                	c.addi	sp,16
2005ead4:	f2bff06f          	jal	zero,2005e9fe <vTaskStatus>
2005ead8:	40b2                	c.lwsp	ra,12(sp)
2005eada:	0141                	c.addi	sp,16
2005eadc:	8082                	c.jr	ra

2005eade <print_unsigned_num>:
2005eade:	715d                	c.addi16sp	sp,-80
2005eae0:	c4a2                	c.swsp	s0,72(sp)
2005eae2:	c2a6                	c.swsp	s1,68(sp)
2005eae4:	c0ca                	c.swsp	s2,64(sp)
2005eae6:	de4e                	c.swsp	s3,60(sp)
2005eae8:	dc52                	c.swsp	s4,56(sp)
2005eaea:	da56                	c.swsp	s5,52(sp)
2005eaec:	d85a                	c.swsp	s6,48(sp)
2005eaee:	d65e                	c.swsp	s7,44(sp)
2005eaf0:	d462                	c.swsp	s8,40(sp)
2005eaf2:	d266                	c.swsp	s9,36(sp)
2005eaf4:	d06a                	c.swsp	s10,32(sp)
2005eaf6:	c686                	c.swsp	ra,76(sp)
2005eaf8:	8c2a                	c.mv	s8,a0
2005eafa:	8cae                	c.mv	s9,a1
2005eafc:	8d32                	c.mv	s10,a2
2005eafe:	8936                	c.mv	s2,a3
2005eb00:	8bba                	c.mv	s7,a4
2005eb02:	89be                	c.mv	s3,a5
2005eb04:	84c2                	c.mv	s1,a6
2005eb06:	4401                	c.li	s0,0
2005eb08:	4a81                	c.li	s5,0
2005eb0a:	4a25                	c.li	s4,9
2005eb0c:	4b05                	c.li	s6,1
2005eb0e:	866a                	c.mv	a2,s10
2005eb10:	4681                	c.li	a3,0
2005eb12:	8562                	c.mv	a0,s8
2005eb14:	85e6                	c.mv	a1,s9
2005eb16:	772020ef          	jal	ra,20061288 <__umoddi3>
2005eb1a:	0ff57893          	andi	a7,a0,255
2005eb1e:	04aa6863          	bltu	s4,a0,2005eb6e <print_unsigned_num+0x90>
2005eb22:	03088893          	addi	a7,a7,48 # f8000030 <__ctrace_end__+0x2e000030>
2005eb26:	007c                	c.addi4spn	a5,sp,12
2005eb28:	97a2                	c.add	a5,s0
2005eb2a:	0ff8f893          	andi	a7,a7,255
2005eb2e:	01178023          	sb	a7,0(a5)
2005eb32:	866a                	c.mv	a2,s10
2005eb34:	4681                	c.li	a3,0
2005eb36:	8562                	c.mv	a0,s8
2005eb38:	85e6                	c.mv	a1,s9
2005eb3a:	0405                	c.addi	s0,1
2005eb3c:	3ec020ef          	jal	ra,20060f28 <__udivdi3>
2005eb40:	039a9463          	bne	s5,s9,2005eb68 <print_unsigned_num+0x8a>
2005eb44:	03ac7263          	bgeu	s8,s10,2005eb68 <print_unsigned_num+0x8a>
2005eb48:	c8d5                	c.beqz	s1,2005ebfc <print_unsigned_num+0x11e>
2005eb4a:	03000793          	addi	a5,zero,48
2005eb4e:	1bfd                	c.addi	s7,-1
2005eb50:	0af91163          	bne	s2,a5,2005ebf2 <print_unsigned_num+0x114>
2005eb54:	02d00513          	addi	a0,zero,45
2005eb58:	dffa6097          	auipc	ra,0xdffa6
2005eb5c:	9ec080e7          	jalr	ra,-1556(ra) # 4544 <LOGUART_PutChar>
2005eb60:	89a6                	c.mv	s3,s1
2005eb62:	09704b63          	blt	zero,s7,2005ebf8 <print_unsigned_num+0x11a>
2005eb66:	a0b1                	c.j	2005ebb2 <print_unsigned_num+0xd4>
2005eb68:	8c2a                	c.mv	s8,a0
2005eb6a:	8cae                	c.mv	s9,a1
2005eb6c:	b74d                	c.j	2005eb0e <print_unsigned_num+0x30>
2005eb6e:	01699563          	bne	s3,s6,2005eb78 <print_unsigned_num+0x9a>
2005eb72:	03788893          	addi	a7,a7,55
2005eb76:	bf45                	c.j	2005eb26 <print_unsigned_num+0x48>
2005eb78:	05788893          	addi	a7,a7,87
2005eb7c:	b76d                	c.j	2005eb26 <print_unsigned_num+0x48>
2005eb7e:	854a                	c.mv	a0,s2
2005eb80:	dffa6097          	auipc	ra,0xdffa6
2005eb84:	9c4080e7          	jalr	ra,-1596(ra) # 4544 <LOGUART_PutChar>
2005eb88:	1a7d                	c.addi	s4,-1
2005eb8a:	ff444ae3          	blt	s0,s4,2005eb7e <print_unsigned_num+0xa0>
2005eb8e:	4781                	c.li	a5,0
2005eb90:	008bc463          	blt	s7,s0,2005eb98 <print_unsigned_num+0xba>
2005eb94:	408b87b3          	sub	a5,s7,s0
2005eb98:	97ce                	c.add	a5,s3
2005eb9a:	c0a9                	c.beqz	s1,2005ebdc <print_unsigned_num+0xfe>
2005eb9c:	84be                	c.mv	s1,a5
2005eb9e:	0df97913          	andi	s2,s2,223
2005eba2:	00091863          	bne	s2,zero,2005ebb2 <print_unsigned_num+0xd4>
2005eba6:	02d00513          	addi	a0,zero,45
2005ebaa:	dffa6097          	auipc	ra,0xdffa6
2005ebae:	99a080e7          	jalr	ra,-1638(ra) # 4544 <LOGUART_PutChar>
2005ebb2:	8922                	c.mv	s2,s0
2005ebb4:	59fd                	c.li	s3,-1
2005ebb6:	197d                	c.addi	s2,-1
2005ebb8:	03391463          	bne	s2,s3,2005ebe0 <print_unsigned_num+0x102>
2005ebbc:	00940533          	add	a0,s0,s1
2005ebc0:	40b6                	c.lwsp	ra,76(sp)
2005ebc2:	4426                	c.lwsp	s0,72(sp)
2005ebc4:	4496                	c.lwsp	s1,68(sp)
2005ebc6:	4906                	c.lwsp	s2,64(sp)
2005ebc8:	59f2                	c.lwsp	s3,60(sp)
2005ebca:	5a62                	c.lwsp	s4,56(sp)
2005ebcc:	5ad2                	c.lwsp	s5,52(sp)
2005ebce:	5b42                	c.lwsp	s6,48(sp)
2005ebd0:	5bb2                	c.lwsp	s7,44(sp)
2005ebd2:	5c22                	c.lwsp	s8,40(sp)
2005ebd4:	5c92                	c.lwsp	s9,36(sp)
2005ebd6:	5d02                	c.lwsp	s10,32(sp)
2005ebd8:	6161                	c.addi16sp	sp,80
2005ebda:	8082                	c.jr	ra
2005ebdc:	84be                	c.mv	s1,a5
2005ebde:	bfd1                	c.j	2005ebb2 <print_unsigned_num+0xd4>
2005ebe0:	007c                	c.addi4spn	a5,sp,12
2005ebe2:	97ca                	c.add	a5,s2
2005ebe4:	0007c503          	lbu	a0,0(a5)
2005ebe8:	dffa6097          	auipc	ra,0xdffa6
2005ebec:	95c080e7          	jalr	ra,-1700(ra) # 4544 <LOGUART_PutChar>
2005ebf0:	b7d9                	c.j	2005ebb6 <print_unsigned_num+0xd8>
2005ebf2:	fb7056e3          	bge	zero,s7,2005eb9e <print_unsigned_num+0xc0>
2005ebf6:	89a6                	c.mv	s3,s1
2005ebf8:	8a5e                	c.mv	s4,s7
2005ebfa:	bf41                	c.j	2005eb8a <print_unsigned_num+0xac>
2005ebfc:	4981                	c.li	s3,0
2005ebfe:	b795                	c.j	2005eb62 <print_unsigned_num+0x84>

2005ec00 <pad_char_control>:
2005ec00:	1141                	c.addi	sp,-16
2005ec02:	c422                	c.swsp	s0,8(sp)
2005ec04:	c226                	c.swsp	s1,4(sp)
2005ec06:	c04a                	c.swsp	s2,0(sp)
2005ec08:	c606                	c.swsp	ra,12(sp)
2005ec0a:	842a                	c.mv	s0,a0
2005ec0c:	892e                	c.mv	s2,a1
2005ec0e:	84aa                	c.mv	s1,a0
2005ec10:	00904c63          	blt	zero,s1,2005ec28 <pad_char_control+0x28>
2005ec14:	8522                	c.mv	a0,s0
2005ec16:	00045363          	bge	s0,zero,2005ec1c <pad_char_control+0x1c>
2005ec1a:	4501                	c.li	a0,0
2005ec1c:	40b2                	c.lwsp	ra,12(sp)
2005ec1e:	4422                	c.lwsp	s0,8(sp)
2005ec20:	4492                	c.lwsp	s1,4(sp)
2005ec22:	4902                	c.lwsp	s2,0(sp)
2005ec24:	0141                	c.addi	sp,16
2005ec26:	8082                	c.jr	ra
2005ec28:	854a                	c.mv	a0,s2
2005ec2a:	dffa6097          	auipc	ra,0xdffa6
2005ec2e:	91a080e7          	jalr	ra,-1766(ra) # 4544 <LOGUART_PutChar>
2005ec32:	14fd                	c.addi	s1,-1
2005ec34:	bff1                	c.j	2005ec10 <pad_char_control+0x10>

2005ec36 <print_decimal_num>:
2005ec36:	1101                	c.addi	sp,-32
2005ec38:	c64e                	c.swsp	s3,12(sp)
2005ec3a:	200689b7          	lui	s3,0x20068
2005ec3e:	cc22                	c.swsp	s0,24(sp)
2005ec40:	c84a                	c.swsp	s2,16(sp)
2005ec42:	c256                	c.swsp	s5,4(sp)
2005ec44:	ce06                	c.swsp	ra,28(sp)
2005ec46:	8ab2                	c.mv	s5,a2
2005ec48:	ca26                	c.swsp	s1,20(sp)
2005ec4a:	c452                	c.swsp	s4,8(sp)
2005ec4c:	872a                	c.mv	a4,a0
2005ec4e:	8936                	c.mv	s2,a3
2005ec50:	4401                	c.li	s0,0
2005ec52:	4625                	c.li	a2,9
2005ec54:	f4898993          	addi	s3,s3,-184 # 20067f48 <num_buf_temp>
2005ec58:	02b776b3          	remu	a3,a4,a1
2005ec5c:	0ff6f793          	andi	a5,a3,255
2005ec60:	04d66963          	bltu	a2,a3,2005ecb2 <print_decimal_num+0x7c>
2005ec64:	03078793          	addi	a5,a5,48
2005ec68:	0ff7f793          	andi	a5,a5,255
2005ec6c:	008986b3          	add	a3,s3,s0
2005ec70:	00f68023          	sb	a5,0(a3)
2005ec74:	00140493          	addi	s1,s0,1
2005ec78:	02b757b3          	divu	a5,a4,a1
2005ec7c:	02b77863          	bgeu	a4,a1,2005ecac <print_decimal_num+0x76>
2005ec80:	8a4a                	c.mv	s4,s2
2005ec82:	05204163          	blt	zero,s2,2005ecc4 <print_decimal_num+0x8e>
2005ec86:	4901                	c.li	s2,0
2005ec88:	14fd                	c.addi	s1,-1
2005ec8a:	0404d663          	bge	s1,zero,2005ecd6 <print_decimal_num+0xa0>
2005ec8e:	00045363          	bge	s0,zero,2005ec94 <print_decimal_num+0x5e>
2005ec92:	547d                	c.li	s0,-1
2005ec94:	0405                	c.addi	s0,1
2005ec96:	01240533          	add	a0,s0,s2
2005ec9a:	40f2                	c.lwsp	ra,28(sp)
2005ec9c:	4462                	c.lwsp	s0,24(sp)
2005ec9e:	44d2                	c.lwsp	s1,20(sp)
2005eca0:	4942                	c.lwsp	s2,16(sp)
2005eca2:	49b2                	c.lwsp	s3,12(sp)
2005eca4:	4a22                	c.lwsp	s4,8(sp)
2005eca6:	4a92                	c.lwsp	s5,4(sp)
2005eca8:	6105                	c.addi16sp	sp,32
2005ecaa:	8082                	c.jr	ra
2005ecac:	8426                	c.mv	s0,s1
2005ecae:	873e                	c.mv	a4,a5
2005ecb0:	b765                	c.j	2005ec58 <print_decimal_num+0x22>
2005ecb2:	05778793          	addi	a5,a5,87
2005ecb6:	bf4d                	c.j	2005ec68 <print_decimal_num+0x32>
2005ecb8:	8556                	c.mv	a0,s5
2005ecba:	dffa6097          	auipc	ra,0xdffa6
2005ecbe:	88a080e7          	jalr	ra,-1910(ra) # 4544 <LOGUART_PutChar>
2005ecc2:	1a7d                	c.addi	s4,-1
2005ecc4:	ff44cae3          	blt	s1,s4,2005ecb8 <print_decimal_num+0x82>
2005ecc8:	4781                	c.li	a5,0
2005ecca:	00994463          	blt	s2,s1,2005ecd2 <print_decimal_num+0x9c>
2005ecce:	409907b3          	sub	a5,s2,s1
2005ecd2:	893e                	c.mv	s2,a5
2005ecd4:	bf55                	c.j	2005ec88 <print_decimal_num+0x52>
2005ecd6:	009987b3          	add	a5,s3,s1
2005ecda:	0007c503          	lbu	a0,0(a5)
2005ecde:	dffa6097          	auipc	ra,0xdffa6
2005ece2:	866080e7          	jalr	ra,-1946(ra) # 4544 <LOGUART_PutChar>
2005ece6:	b74d                	c.j	2005ec88 <print_decimal_num+0x52>

2005ece8 <pad_num_control>:
2005ece8:	04088163          	beq	a7,zero,2005ed2a <pad_num_control+0x42>
2005ecec:	1141                	c.addi	sp,-16
2005ecee:	c226                	c.swsp	s1,4(sp)
2005ecf0:	84ba                	c.mv	s1,a4
2005ecf2:	4701                	c.li	a4,0
2005ecf4:	c422                	c.swsp	s0,8(sp)
2005ecf6:	c04a                	c.swsp	s2,0(sp)
2005ecf8:	c606                	c.swsp	ra,12(sp)
2005ecfa:	8936                	c.mv	s2,a3
2005ecfc:	de3ff0ef          	jal	ra,2005eade <print_unsigned_num>
2005ed00:	842a                	c.mv	s0,a0
2005ed02:	00955d63          	bge	a0,s1,2005ed1c <pad_num_control+0x34>
2005ed06:	02000793          	addi	a5,zero,32
2005ed0a:	00f91963          	bne	s2,a5,2005ed1c <pad_num_control+0x34>
2005ed0e:	02000593          	addi	a1,zero,32
2005ed12:	40a48533          	sub	a0,s1,a0
2005ed16:	eebff0ef          	jal	ra,2005ec00 <pad_char_control>
2005ed1a:	942a                	c.add	s0,a0
2005ed1c:	40b2                	c.lwsp	ra,12(sp)
2005ed1e:	8522                	c.mv	a0,s0
2005ed20:	4422                	c.lwsp	s0,8(sp)
2005ed22:	4492                	c.lwsp	s1,4(sp)
2005ed24:	4902                	c.lwsp	s2,0(sp)
2005ed26:	0141                	c.addi	sp,16
2005ed28:	8082                	c.jr	ra
2005ed2a:	db5ff06f          	jal	zero,2005eade <print_unsigned_num>

2005ed2e <_rand>:
2005ed2e:	1141                	c.addi	sp,-16
2005ed30:	450d                	c.li	a0,3
2005ed32:	c606                	c.swsp	ra,12(sp)
2005ed34:	9bbfa0ef          	jal	ra,200596ee <PLL_State>
2005ed38:	c50d                	c.beqz	a0,2005ed62 <_rand+0x34>
2005ed3a:	65c1                	c.lui	a1,0x10
2005ed3c:	410167b7          	lui	a5,0x41016
2005ed40:	453d                	c.li	a0,15
2005ed42:	4685                	c.li	a3,1
2005ed44:	00158713          	addi	a4,a1,1 # 10001 <mbedtls_ecp_check_pub_priv+0x123>
2005ed48:	47d0                	c.lw	a2,12(a5)
2005ed4a:	00a61763          	bne	a2,a0,2005ed58 <_rand+0x2a>
2005ed4e:	df94                	c.sw	a3,56(a5)
2005ed50:	177d                	c.addi	a4,-1
2005ed52:	fb7d                	c.bnez	a4,2005ed48 <_rand+0x1a>
2005ed54:	df94                	c.sw	a3,56(a5)
2005ed56:	b7fd                	c.j	2005ed44 <_rand+0x16>
2005ed58:	de65                	c.beqz	a2,2005ed50 <_rand+0x22>
2005ed5a:	4b88                	c.lw	a0,16(a5)
2005ed5c:	40b2                	c.lwsp	ra,12(sp)
2005ed5e:	0141                	c.addi	sp,16
2005ed60:	8082                	c.jr	ra
2005ed62:	fb418693          	addi	a3,gp,-76 # 20066230 <rand_first.1>
2005ed66:	429c                	c.lw	a5,0(a3)
2005ed68:	20068737          	lui	a4,0x20068
2005ed6c:	f5470713          	addi	a4,a4,-172 # 20067f54 <rand_seed.0>
2005ed70:	eb91                	c.bnez	a5,2005ed84 <_rand+0x56>
2005ed72:	678d                	c.lui	a5,0x3
2005ed74:	03978793          	addi	a5,a5,57 # 3039 <FLASH_SetSpiMode+0x375>
2005ed78:	c31c                	c.sw	a5,0(a4)
2005ed7a:	c35c                	c.sw	a5,4(a4)
2005ed7c:	c71c                	c.sw	a5,8(a4)
2005ed7e:	c75c                	c.sw	a5,12(a4)
2005ed80:	4785                	c.li	a5,1
2005ed82:	c29c                	c.sw	a5,0(a3)
2005ed84:	4314                	c.lw	a3,0(a4)
2005ed86:	fff80537          	lui	a0,0xfff80
2005ed8a:	474c                	c.lw	a1,12(a4)
2005ed8c:	00669793          	slli	a5,a3,0x6
2005ed90:	8fb5                	c.xor	a5,a3
2005ed92:	06ca                	c.slli	a3,0x12
2005ed94:	8ee9                	c.and	a3,a0
2005ed96:	83b5                	c.srli	a5,0xd
2005ed98:	8fb5                	c.xor	a5,a3
2005ed9a:	4354                	c.lw	a3,4(a4)
2005ed9c:	fff00837          	lui	a6,0xfff00
2005eda0:	c31c                	c.sw	a5,0(a4)
2005eda2:	00269613          	slli	a2,a3,0x2
2005eda6:	00c6c533          	xor	a0,a3,a2
2005edaa:	4714                	c.lw	a3,8(a4)
2005edac:	9a01                	c.andi	a2,-32
2005edae:	816d                	c.srli	a0,0x1b
2005edb0:	8d31                	c.xor	a0,a2
2005edb2:	00d69613          	slli	a2,a3,0xd
2005edb6:	8e35                	c.xor	a2,a3
2005edb8:	069e                	c.slli	a3,0x7
2005edba:	8006f693          	andi	a3,a3,-2048
2005edbe:	8255                	c.srli	a2,0x15
2005edc0:	8e35                	c.xor	a2,a3
2005edc2:	00359693          	slli	a3,a1,0x3
2005edc6:	8ead                	c.xor	a3,a1
2005edc8:	05b6                	c.slli	a1,0xd
2005edca:	c348                	c.sw	a0,4(a4)
2005edcc:	82b1                	c.srli	a3,0xc
2005edce:	0105f5b3          	and	a1,a1,a6
2005edd2:	8d3d                	c.xor	a0,a5
2005edd4:	8ead                	c.xor	a3,a1
2005edd6:	8d31                	c.xor	a0,a2
2005edd8:	c710                	c.sw	a2,8(a4)
2005edda:	c754                	c.sw	a3,12(a4)
2005eddc:	8d35                	c.xor	a0,a3
2005edde:	bfbd                	c.j	2005ed5c <_rand+0x2e>

2005ede0 <DiagVprintf>:
2005ede0:	7139                	c.addi16sp	sp,-64
2005ede2:	d452                	c.swsp	s4,40(sp)
2005ede4:	20063a37          	lui	s4,0x20063
2005ede8:	dc22                	c.swsp	s0,56(sp)
2005edea:	da26                	c.swsp	s1,52(sp)
2005edec:	d84a                	c.swsp	s2,48(sp)
2005edee:	d64e                	c.swsp	s3,44(sp)
2005edf0:	d256                	c.swsp	s5,36(sp)
2005edf2:	d05a                	c.swsp	s6,32(sp)
2005edf4:	de06                	c.swsp	ra,60(sp)
2005edf6:	ce5e                	c.swsp	s7,28(sp)
2005edf8:	cc62                	c.swsp	s8,24(sp)
2005edfa:	ca66                	c.swsp	s9,20(sp)
2005edfc:	892a                	c.mv	s2,a0
2005edfe:	842e                	c.mv	s0,a1
2005ee00:	4981                	c.li	s3,0
2005ee02:	4481                	c.li	s1,0
2005ee04:	42ca0a13          	addi	s4,s4,1068 # 2006342c <TaskStateString.0+0x18>
2005ee08:	4aa5                	c.li	s5,9
2005ee0a:	4b29                	c.li	s6,10
2005ee0c:	00094503          	lbu	a0,0(s2)
2005ee10:	ed19                	c.bnez	a0,2005ee2e <DiagVprintf+0x4e>
2005ee12:	50f2                	c.lwsp	ra,60(sp)
2005ee14:	5462                	c.lwsp	s0,56(sp)
2005ee16:	5942                	c.lwsp	s2,48(sp)
2005ee18:	59b2                	c.lwsp	s3,44(sp)
2005ee1a:	5a22                	c.lwsp	s4,40(sp)
2005ee1c:	5a92                	c.lwsp	s5,36(sp)
2005ee1e:	5b02                	c.lwsp	s6,32(sp)
2005ee20:	4bf2                	c.lwsp	s7,28(sp)
2005ee22:	4c62                	c.lwsp	s8,24(sp)
2005ee24:	4cd2                	c.lwsp	s9,20(sp)
2005ee26:	8526                	c.mv	a0,s1
2005ee28:	54d2                	c.lwsp	s1,52(sp)
2005ee2a:	6121                	c.addi16sp	sp,64
2005ee2c:	8082                	c.jr	ra
2005ee2e:	02500793          	addi	a5,zero,37
2005ee32:	0905                	c.addi	s2,1
2005ee34:	28f51f63          	bne	a0,a5,2005f0d2 <DiagVprintf+0x2f2>
2005ee38:	4881                	c.li	a7,0
2005ee3a:	4701                	c.li	a4,0
2005ee3c:	4681                	c.li	a3,0
2005ee3e:	4601                	c.li	a2,0
2005ee40:	07a00e13          	addi	t3,zero,122
2005ee44:	05700e93          	addi	t4,zero,87
2005ee48:	02200f13          	addi	t5,zero,34
2005ee4c:	03000593          	addi	a1,zero,48
2005ee50:	04300813          	addi	a6,zero,67
2005ee54:	04c00f93          	addi	t6,zero,76
2005ee58:	03900293          	addi	t0,zero,57
2005ee5c:	02000393          	addi	t2,zero,32
2005ee60:	02500313          	addi	t1,zero,37
2005ee64:	02d00b93          	addi	s7,zero,45
2005ee68:	00094783          	lbu	a5,0(s2)
2005ee6c:	02fe6063          	bltu	t3,a5,2005ee8c <DiagVprintf+0xac>
2005ee70:	02fee063          	bltu	t4,a5,2005ee90 <DiagVprintf+0xb0>
2005ee74:	22b78663          	beq	a5,a1,2005f0a0 <DiagVprintf+0x2c0>
2005ee78:	02f5eb63          	bltu	a1,a5,2005eeae <DiagVprintf+0xce>
2005ee7c:	20678963          	beq	a5,t1,2005f08e <DiagVprintf+0x2ae>
2005ee80:	02f36263          	bltu	t1,a5,2005eea4 <DiagVprintf+0xc4>
2005ee84:	20078c63          	beq	a5,zero,2005f09c <DiagVprintf+0x2bc>
2005ee88:	24778163          	beq	a5,t2,2005f0ca <DiagVprintf+0x2ea>
2005ee8c:	54fd                	c.li	s1,-1
2005ee8e:	b751                	c.j	2005ee12 <DiagVprintf+0x32>
2005ee90:	fa878793          	addi	a5,a5,-88
2005ee94:	0ff7f793          	andi	a5,a5,255
2005ee98:	feff6ae3          	bltu	t5,a5,2005ee8c <DiagVprintf+0xac>
2005ee9c:	078a                	c.slli	a5,0x2
2005ee9e:	97d2                	c.add	a5,s4
2005eea0:	439c                	c.lw	a5,0(a5)
2005eea2:	8782                	c.jr	a5
2005eea4:	ff7794e3          	bne	a5,s7,2005ee8c <DiagVprintf+0xac>
2005eea8:	0905                	c.addi	s2,1
2005eeaa:	0885                	c.addi	a7,1
2005eeac:	bf75                	c.j	2005ee68 <DiagVprintf+0x88>
2005eeae:	19078363          	beq	a5,a6,2005f034 <DiagVprintf+0x254>
2005eeb2:	02f86563          	bltu	a6,a5,2005eedc <DiagVprintf+0xfc>
2005eeb6:	20f2fb63          	bgeu	t0,a5,2005f0cc <DiagVprintf+0x2ec>
2005eeba:	04200593          	addi	a1,zero,66
2005eebe:	fcb797e3          	bne	a5,a1,2005ee8c <DiagVprintf+0xac>
2005eec2:	4785                	c.li	a5,1
2005eec4:	0ac7d063          	bge	a5,a2,2005ef64 <DiagVprintf+0x184>
2005eec8:	041d                	c.addi	s0,7
2005eeca:	9861                	c.andi	s0,-8
2005eecc:	4008                	c.lw	a0,0(s0)
2005eece:	404c                	c.lw	a1,4(s0)
2005eed0:	00840b93          	addi	s7,s0,8
2005eed4:	4801                	c.li	a6,0
2005eed6:	87ce                	c.mv	a5,s3
2005eed8:	4609                	c.li	a2,2
2005eeda:	a889                	c.j	2005ef2c <DiagVprintf+0x14c>
2005eedc:	1bf78663          	beq	a5,t6,2005f088 <DiagVprintf+0x2a8>
2005eee0:	04f00593          	addi	a1,zero,79
2005eee4:	fab794e3          	bne	a5,a1,2005ee8c <DiagVprintf+0xac>
2005eee8:	4785                	c.li	a5,1
2005eeea:	08c7d263          	bge	a5,a2,2005ef6e <DiagVprintf+0x18e>
2005eeee:	041d                	c.addi	s0,7
2005eef0:	9861                	c.andi	s0,-8
2005eef2:	4008                	c.lw	a0,0(s0)
2005eef4:	404c                	c.lw	a1,4(s0)
2005eef6:	00840b93          	addi	s7,s0,8
2005eefa:	4801                	c.li	a6,0
2005eefc:	87ce                	c.mv	a5,s3
2005eefe:	4621                	c.li	a2,8
2005ef00:	a035                	c.j	2005ef2c <DiagVprintf+0x14c>
2005ef02:	4785                	c.li	a5,1
2005ef04:	02c7da63          	bge	a5,a2,2005ef38 <DiagVprintf+0x158>
2005ef08:	041d                	c.addi	s0,7
2005ef0a:	9861                	c.andi	s0,-8
2005ef0c:	4008                	c.lw	a0,0(s0)
2005ef0e:	404c                	c.lw	a1,4(s0)
2005ef10:	00840b93          	addi	s7,s0,8
2005ef14:	0405d163          	bge	a1,zero,2005ef56 <DiagVprintf+0x176>
2005ef18:	00a037b3          	sltu	a5,zero,a0
2005ef1c:	40b005b3          	sub	a1,zero,a1
2005ef20:	40a00533          	sub	a0,zero,a0
2005ef24:	8d9d                	c.sub	a1,a5
2005ef26:	4805                	c.li	a6,1
2005ef28:	87ce                	c.mv	a5,s3
2005ef2a:	4629                	c.li	a2,10
2005ef2c:	dbdff0ef          	jal	ra,2005ece8 <pad_num_control>
2005ef30:	94aa                	c.add	s1,a0
2005ef32:	0905                	c.addi	s2,1
2005ef34:	845e                	c.mv	s0,s7
2005ef36:	bdd9                	c.j	2005ee0c <DiagVprintf+0x2c>
2005ef38:	4008                	c.lw	a0,0(s0)
2005ef3a:	00440b93          	addi	s7,s0,4
2005ef3e:	41f55593          	srai	a1,a0,0x1f
2005ef42:	bfc9                	c.j	2005ef14 <DiagVprintf+0x134>
2005ef44:	4785                	c.li	a5,1
2005ef46:	00c7da63          	bge	a5,a2,2005ef5a <DiagVprintf+0x17a>
2005ef4a:	041d                	c.addi	s0,7
2005ef4c:	9861                	c.andi	s0,-8
2005ef4e:	4008                	c.lw	a0,0(s0)
2005ef50:	404c                	c.lw	a1,4(s0)
2005ef52:	00840b93          	addi	s7,s0,8
2005ef56:	4801                	c.li	a6,0
2005ef58:	bfc1                	c.j	2005ef28 <DiagVprintf+0x148>
2005ef5a:	4008                	c.lw	a0,0(s0)
2005ef5c:	4581                	c.li	a1,0
2005ef5e:	00440b93          	addi	s7,s0,4
2005ef62:	bfd5                	c.j	2005ef56 <DiagVprintf+0x176>
2005ef64:	4008                	c.lw	a0,0(s0)
2005ef66:	4581                	c.li	a1,0
2005ef68:	00440b93          	addi	s7,s0,4
2005ef6c:	b7a5                	c.j	2005eed4 <DiagVprintf+0xf4>
2005ef6e:	4008                	c.lw	a0,0(s0)
2005ef70:	4581                	c.li	a1,0
2005ef72:	00440b93          	addi	s7,s0,4
2005ef76:	b751                	c.j	2005eefa <DiagVprintf+0x11a>
2005ef78:	4985                	c.li	s3,1
2005ef7a:	4785                	c.li	a5,1
2005ef7c:	00c7dc63          	bge	a5,a2,2005ef94 <DiagVprintf+0x1b4>
2005ef80:	041d                	c.addi	s0,7
2005ef82:	9861                	c.andi	s0,-8
2005ef84:	4008                	c.lw	a0,0(s0)
2005ef86:	404c                	c.lw	a1,4(s0)
2005ef88:	00840b93          	addi	s7,s0,8
2005ef8c:	4801                	c.li	a6,0
2005ef8e:	87ce                	c.mv	a5,s3
2005ef90:	4641                	c.li	a2,16
2005ef92:	bf69                	c.j	2005ef2c <DiagVprintf+0x14c>
2005ef94:	4008                	c.lw	a0,0(s0)
2005ef96:	4581                	c.li	a1,0
2005ef98:	00440b93          	addi	s7,s0,4
2005ef9c:	bfc5                	c.j	2005ef8c <DiagVprintf+0x1ac>
2005ef9e:	4008                	c.lw	a0,0(s0)
2005efa0:	4801                	c.li	a6,0
2005efa2:	4785                	c.li	a5,1
2005efa4:	4721                	c.li	a4,8
2005efa6:	03000693          	addi	a3,zero,48
2005efaa:	4641                	c.li	a2,16
2005efac:	4581                	c.li	a1,0
2005efae:	d3bff0ef          	jal	ra,2005ece8 <pad_num_control>
2005efb2:	00440b93          	addi	s7,s0,4
2005efb6:	94aa                	c.add	s1,a0
2005efb8:	4985                	c.li	s3,1
2005efba:	bfa5                	c.j	2005ef32 <DiagVprintf+0x152>
2005efbc:	00042c83          	lw	s9,0(s0)
2005efc0:	00440b93          	addi	s7,s0,4
2005efc4:	4401                	c.li	s0,0
2005efc6:	008c87b3          	add	a5,s9,s0
2005efca:	0007c783          	lbu	a5,0(a5)
2005efce:	e7a9                	c.bnez	a5,2005f018 <DiagVprintf+0x238>
2005efd0:	02089063          	bne	a7,zero,2005eff0 <DiagVprintf+0x210>
2005efd4:	40870533          	sub	a0,a4,s0
2005efd8:	00a05c63          	bge	zero,a0,2005eff0 <DiagVprintf+0x210>
2005efdc:	85b6                	c.mv	a1,a3
2005efde:	c646                	c.swsp	a7,12(sp)
2005efe0:	c43a                	c.swsp	a4,8(sp)
2005efe2:	c236                	c.swsp	a3,4(sp)
2005efe4:	c1dff0ef          	jal	ra,2005ec00 <pad_char_control>
2005efe8:	48b2                	c.lwsp	a7,12(sp)
2005efea:	4722                	c.lwsp	a4,8(sp)
2005efec:	4692                	c.lwsp	a3,4(sp)
2005efee:	94aa                	c.add	s1,a0
2005eff0:	5c7d                	c.li	s8,-1
2005eff2:	000c8863          	beq	s9,zero,2005f002 <DiagVprintf+0x222>
2005eff6:	4c01                	c.li	s8,0
2005eff8:	018c87b3          	add	a5,s9,s8
2005effc:	0007c503          	lbu	a0,0(a5)
2005f000:	ed11                	c.bnez	a0,2005f01c <DiagVprintf+0x23c>
2005f002:	94e2                	c.add	s1,s8
2005f004:	f20887e3          	beq	a7,zero,2005ef32 <DiagVprintf+0x152>
2005f008:	40870533          	sub	a0,a4,s0
2005f00c:	85b6                	c.mv	a1,a3
2005f00e:	f2a052e3          	bge	zero,a0,2005ef32 <DiagVprintf+0x152>
2005f012:	befff0ef          	jal	ra,2005ec00 <pad_char_control>
2005f016:	bf29                	c.j	2005ef30 <DiagVprintf+0x150>
2005f018:	0405                	c.addi	s0,1
2005f01a:	b775                	c.j	2005efc6 <DiagVprintf+0x1e6>
2005f01c:	c646                	c.swsp	a7,12(sp)
2005f01e:	c43a                	c.swsp	a4,8(sp)
2005f020:	c236                	c.swsp	a3,4(sp)
2005f022:	dffa5097          	auipc	ra,0xdffa5
2005f026:	522080e7          	jalr	ra,1314(ra) # 4544 <LOGUART_PutChar>
2005f02a:	48b2                	c.lwsp	a7,12(sp)
2005f02c:	4722                	c.lwsp	a4,8(sp)
2005f02e:	4692                	c.lwsp	a3,4(sp)
2005f030:	0c05                	c.addi	s8,1
2005f032:	b7d9                	c.j	2005eff8 <DiagVprintf+0x218>
2005f034:	00440b93          	addi	s7,s0,4
2005f038:	00044403          	lbu	s0,0(s0)
2005f03c:	02089163          	bne	a7,zero,2005f05e <DiagVprintf+0x27e>
2005f040:	4785                	c.li	a5,1
2005f042:	00e7de63          	bge	a5,a4,2005f05e <DiagVprintf+0x27e>
2005f046:	85b6                	c.mv	a1,a3
2005f048:	fff70513          	addi	a0,a4,-1
2005f04c:	c646                	c.swsp	a7,12(sp)
2005f04e:	c436                	c.swsp	a3,8(sp)
2005f050:	c23a                	c.swsp	a4,4(sp)
2005f052:	bafff0ef          	jal	ra,2005ec00 <pad_char_control>
2005f056:	48b2                	c.lwsp	a7,12(sp)
2005f058:	46a2                	c.lwsp	a3,8(sp)
2005f05a:	4712                	c.lwsp	a4,4(sp)
2005f05c:	94aa                	c.add	s1,a0
2005f05e:	8522                	c.mv	a0,s0
2005f060:	c646                	c.swsp	a7,12(sp)
2005f062:	c43a                	c.swsp	a4,8(sp)
2005f064:	c236                	c.swsp	a3,4(sp)
2005f066:	dffa5097          	auipc	ra,0xdffa5
2005f06a:	4de080e7          	jalr	ra,1246(ra) # 4544 <LOGUART_PutChar>
2005f06e:	48b2                	c.lwsp	a7,12(sp)
2005f070:	0485                	c.addi	s1,1
2005f072:	ec0880e3          	beq	a7,zero,2005ef32 <DiagVprintf+0x152>
2005f076:	4722                	c.lwsp	a4,8(sp)
2005f078:	4785                	c.li	a5,1
2005f07a:	eae7dce3          	bge	a5,a4,2005ef32 <DiagVprintf+0x152>
2005f07e:	4692                	c.lwsp	a3,4(sp)
2005f080:	fff70513          	addi	a0,a4,-1
2005f084:	85b6                	c.mv	a1,a3
2005f086:	b771                	c.j	2005f012 <DiagVprintf+0x232>
2005f088:	0605                	c.addi	a2,1
2005f08a:	0905                	c.addi	s2,1
2005f08c:	bbf1                	c.j	2005ee68 <DiagVprintf+0x88>
2005f08e:	02500513          	addi	a0,zero,37
2005f092:	dffa5097          	auipc	ra,0xdffa5
2005f096:	4b2080e7          	jalr	ra,1202(ra) # 4544 <LOGUART_PutChar>
2005f09a:	0485                	c.addi	s1,1
2005f09c:	8ba2                	c.mv	s7,s0
2005f09e:	bd51                	c.j	2005ef32 <DiagVprintf+0x152>
2005f0a0:	0905                	c.addi	s2,1
2005f0a2:	03000693          	addi	a3,zero,48
2005f0a6:	4701                	c.li	a4,0
2005f0a8:	00094783          	lbu	a5,0(s2)
2005f0ac:	d60783e3          	beq	a5,zero,2005ee12 <DiagVprintf+0x32>
2005f0b0:	fd078513          	addi	a0,a5,-48
2005f0b4:	0ff57513          	andi	a0,a0,255
2005f0b8:	daaae8e3          	bltu	s5,a0,2005ee68 <DiagVprintf+0x88>
2005f0bc:	03670733          	mul	a4,a4,s6
2005f0c0:	0905                	c.addi	s2,1
2005f0c2:	fd070713          	addi	a4,a4,-48
2005f0c6:	973e                	c.add	a4,a5
2005f0c8:	b7c5                	c.j	2005f0a8 <DiagVprintf+0x2c8>
2005f0ca:	0905                	c.addi	s2,1
2005f0cc:	02000693          	addi	a3,zero,32
2005f0d0:	bfd9                	c.j	2005f0a6 <DiagVprintf+0x2c6>
2005f0d2:	dffa5097          	auipc	ra,0xdffa5
2005f0d6:	472080e7          	jalr	ra,1138(ra) # 4544 <LOGUART_PutChar>
2005f0da:	0485                	c.addi	s1,1
2005f0dc:	bb05                	c.j	2005ee0c <DiagVprintf+0x2c>

2005f0de <DiagPrintf>:
2005f0de:	7139                	c.addi16sp	sp,-64
2005f0e0:	d22e                	c.swsp	a1,36(sp)
2005f0e2:	104c                	c.addi4spn	a1,sp,36
2005f0e4:	ce06                	c.swsp	ra,28(sp)
2005f0e6:	d432                	c.swsp	a2,40(sp)
2005f0e8:	d636                	c.swsp	a3,44(sp)
2005f0ea:	d83a                	c.swsp	a4,48(sp)
2005f0ec:	da3e                	c.swsp	a5,52(sp)
2005f0ee:	dc42                	c.swsp	a6,56(sp)
2005f0f0:	de46                	c.swsp	a7,60(sp)
2005f0f2:	c62e                	c.swsp	a1,12(sp)
2005f0f4:	cedff0ef          	jal	ra,2005ede0 <DiagVprintf>
2005f0f8:	40f2                	c.lwsp	ra,28(sp)
2005f0fa:	6121                	c.addi16sp	sp,64
2005f0fc:	8082                	c.jr	ra

2005f0fe <DiagPrintf_minimal>:
2005f0fe:	711d                	c.addi16sp	sp,-96
2005f100:	d84a                	c.swsp	s2,48(sp)
2005f102:	04410913          	addi	s2,sp,68
2005f106:	dc22                	c.swsp	s0,56(sp)
2005f108:	da26                	c.swsp	s1,52(sp)
2005f10a:	d64e                	c.swsp	s3,44(sp)
2005f10c:	d452                	c.swsp	s4,40(sp)
2005f10e:	d256                	c.swsp	s5,36(sp)
2005f110:	de06                	c.swsp	ra,60(sp)
2005f112:	d05a                	c.swsp	s6,32(sp)
2005f114:	ce5e                	c.swsp	s7,28(sp)
2005f116:	84aa                	c.mv	s1,a0
2005f118:	c2ae                	c.swsp	a1,68(sp)
2005f11a:	c4b2                	c.swsp	a2,72(sp)
2005f11c:	c6b6                	c.swsp	a3,76(sp)
2005f11e:	c8ba                	c.swsp	a4,80(sp)
2005f120:	cabe                	c.swsp	a5,84(sp)
2005f122:	ccc2                	c.swsp	a6,88(sp)
2005f124:	cec6                	c.swsp	a7,92(sp)
2005f126:	c64a                	c.swsp	s2,12(sp)
2005f128:	4401                	c.li	s0,0
2005f12a:	03000993          	addi	s3,zero,48
2005f12e:	4a25                	c.li	s4,9
2005f130:	4aa9                	c.li	s5,10
2005f132:	0004c503          	lbu	a0,0(s1)
2005f136:	ed09                	c.bnez	a0,2005f150 <DiagPrintf_minimal+0x52>
2005f138:	50f2                	c.lwsp	ra,60(sp)
2005f13a:	8522                	c.mv	a0,s0
2005f13c:	5462                	c.lwsp	s0,56(sp)
2005f13e:	54d2                	c.lwsp	s1,52(sp)
2005f140:	5942                	c.lwsp	s2,48(sp)
2005f142:	59b2                	c.lwsp	s3,44(sp)
2005f144:	5a22                	c.lwsp	s4,40(sp)
2005f146:	5a92                	c.lwsp	s5,36(sp)
2005f148:	5b02                	c.lwsp	s6,32(sp)
2005f14a:	4bf2                	c.lwsp	s7,28(sp)
2005f14c:	6125                	c.addi16sp	sp,96
2005f14e:	8082                	c.jr	ra
2005f150:	02500793          	addi	a5,zero,37
2005f154:	0485                	c.addi	s1,1
2005f156:	14f51263          	bne	a0,a5,2005f29a <DiagPrintf_minimal+0x19c>
2005f15a:	4681                	c.li	a3,0
2005f15c:	4601                	c.li	a2,0
2005f15e:	04300513          	addi	a0,zero,67
2005f162:	45a1                	c.li	a1,8
2005f164:	02000813          	addi	a6,zero,32
2005f168:	0004c783          	lbu	a5,0(s1)
2005f16c:	0ca78463          	beq	a5,a0,2005f234 <DiagPrintf_minimal+0x136>
2005f170:	02f56763          	bltu	a0,a5,2005f19e <DiagPrintf_minimal+0xa0>
2005f174:	0f378c63          	beq	a5,s3,2005f26c <DiagPrintf_minimal+0x16e>
2005f178:	00f9ea63          	bltu	s3,a5,2005f18c <DiagPrintf_minimal+0x8e>
2005f17c:	11078d63          	beq	a5,a6,2005f296 <DiagPrintf_minimal+0x198>
2005f180:	02500713          	addi	a4,zero,37
2005f184:	0ce78c63          	beq	a5,a4,2005f25c <DiagPrintf_minimal+0x15e>
2005f188:	547d                	c.li	s0,-1
2005f18a:	b77d                	c.j	2005f138 <DiagPrintf_minimal+0x3a>
2005f18c:	fcf78793          	addi	a5,a5,-49
2005f190:	0ff7f793          	andi	a5,a5,255
2005f194:	fef5eae3          	bltu	a1,a5,2005f188 <DiagPrintf_minimal+0x8a>
2005f198:	02000613          	addi	a2,zero,32
2005f19c:	a8d9                	c.j	2005f272 <DiagPrintf_minimal+0x174>
2005f19e:	07300713          	addi	a4,zero,115
2005f1a2:	04e78263          	beq	a5,a4,2005f1e6 <DiagPrintf_minimal+0xe8>
2005f1a6:	02f76463          	bltu	a4,a5,2005f1ce <DiagPrintf_minimal+0xd0>
2005f1aa:	06300713          	addi	a4,zero,99
2005f1ae:	08e78363          	beq	a5,a4,2005f234 <DiagPrintf_minimal+0x136>
2005f1b2:	06400713          	addi	a4,zero,100
2005f1b6:	fce799e3          	bne	a5,a4,2005f188 <DiagPrintf_minimal+0x8a>
2005f1ba:	00490b13          	addi	s6,s2,4
2005f1be:	45a9                	c.li	a1,10
2005f1c0:	00092503          	lw	a0,0(s2)
2005f1c4:	a73ff0ef          	jal	ra,2005ec36 <print_decimal_num>
2005f1c8:	942a                	c.add	s0,a0
2005f1ca:	895a                	c.mv	s2,s6
2005f1cc:	a891                	c.j	2005f220 <DiagPrintf_minimal+0x122>
2005f1ce:	07500713          	addi	a4,zero,117
2005f1d2:	fee784e3          	beq	a5,a4,2005f1ba <DiagPrintf_minimal+0xbc>
2005f1d6:	07800713          	addi	a4,zero,120
2005f1da:	fae797e3          	bne	a5,a4,2005f188 <DiagPrintf_minimal+0x8a>
2005f1de:	00490b13          	addi	s6,s2,4
2005f1e2:	45c1                	c.li	a1,16
2005f1e4:	bff1                	c.j	2005f1c0 <DiagPrintf_minimal+0xc2>
2005f1e6:	00092b03          	lw	s6,0(s2)
2005f1ea:	00490b93          	addi	s7,s2,4
2005f1ee:	4501                	c.li	a0,0
2005f1f0:	00ab07b3          	add	a5,s6,a0
2005f1f4:	0007c783          	lbu	a5,0(a5)
2005f1f8:	e795                	c.bnez	a5,2005f224 <DiagPrintf_minimal+0x126>
2005f1fa:	40a68533          	sub	a0,a3,a0
2005f1fe:	00a05663          	bge	zero,a0,2005f20a <DiagPrintf_minimal+0x10c>
2005f202:	85b2                	c.mv	a1,a2
2005f204:	9fdff0ef          	jal	ra,2005ec00 <pad_char_control>
2005f208:	942a                	c.add	s0,a0
2005f20a:	597d                	c.li	s2,-1
2005f20c:	000b0863          	beq	s6,zero,2005f21c <DiagPrintf_minimal+0x11e>
2005f210:	4901                	c.li	s2,0
2005f212:	012b07b3          	add	a5,s6,s2
2005f216:	0007c503          	lbu	a0,0(a5)
2005f21a:	e519                	c.bnez	a0,2005f228 <DiagPrintf_minimal+0x12a>
2005f21c:	944a                	c.add	s0,s2
2005f21e:	895e                	c.mv	s2,s7
2005f220:	0485                	c.addi	s1,1
2005f222:	bf01                	c.j	2005f132 <DiagPrintf_minimal+0x34>
2005f224:	0505                	c.addi	a0,1
2005f226:	b7e9                	c.j	2005f1f0 <DiagPrintf_minimal+0xf2>
2005f228:	dffa5097          	auipc	ra,0xdffa5
2005f22c:	31c080e7          	jalr	ra,796(ra) # 4544 <LOGUART_PutChar>
2005f230:	0905                	c.addi	s2,1
2005f232:	b7c5                	c.j	2005f212 <DiagPrintf_minimal+0x114>
2005f234:	4785                	c.li	a5,1
2005f236:	00490b13          	addi	s6,s2,4
2005f23a:	00094903          	lbu	s2,0(s2)
2005f23e:	00d7d863          	bge	a5,a3,2005f24e <DiagPrintf_minimal+0x150>
2005f242:	85b2                	c.mv	a1,a2
2005f244:	fff68513          	addi	a0,a3,-1
2005f248:	9b9ff0ef          	jal	ra,2005ec00 <pad_char_control>
2005f24c:	942a                	c.add	s0,a0
2005f24e:	854a                	c.mv	a0,s2
2005f250:	dffa5097          	auipc	ra,0xdffa5
2005f254:	2f4080e7          	jalr	ra,756(ra) # 4544 <LOGUART_PutChar>
2005f258:	0405                	c.addi	s0,1
2005f25a:	bf85                	c.j	2005f1ca <DiagPrintf_minimal+0xcc>
2005f25c:	02500513          	addi	a0,zero,37
2005f260:	dffa5097          	auipc	ra,0xdffa5
2005f264:	2e4080e7          	jalr	ra,740(ra) # 4544 <LOGUART_PutChar>
2005f268:	0405                	c.addi	s0,1
2005f26a:	bf5d                	c.j	2005f220 <DiagPrintf_minimal+0x122>
2005f26c:	0485                	c.addi	s1,1
2005f26e:	03000613          	addi	a2,zero,48
2005f272:	4681                	c.li	a3,0
2005f274:	0004c783          	lbu	a5,0(s1)
2005f278:	ec0780e3          	beq	a5,zero,2005f138 <DiagPrintf_minimal+0x3a>
2005f27c:	fd078713          	addi	a4,a5,-48
2005f280:	0ff77713          	andi	a4,a4,255
2005f284:	eeea62e3          	bltu	s4,a4,2005f168 <DiagPrintf_minimal+0x6a>
2005f288:	035686b3          	mul	a3,a3,s5
2005f28c:	0485                	c.addi	s1,1
2005f28e:	fd068693          	addi	a3,a3,-48
2005f292:	96be                	c.add	a3,a5
2005f294:	b7c5                	c.j	2005f274 <DiagPrintf_minimal+0x176>
2005f296:	0485                	c.addi	s1,1
2005f298:	b701                	c.j	2005f198 <DiagPrintf_minimal+0x9a>
2005f29a:	dffa5097          	auipc	ra,0xdffa5
2005f29e:	2aa080e7          	jalr	ra,682(ra) # 4544 <LOGUART_PutChar>
2005f2a2:	0405                	c.addi	s0,1
2005f2a4:	b579                	c.j	2005f132 <DiagPrintf_minimal+0x34>

2005f2a6 <DiagSnPrintf>:
2005f2a6:	7159                	c.addi16sp	sp,-112
2005f2a8:	c6a2                	c.swsp	s0,76(sp)
2005f2aa:	c4a6                	c.swsp	s1,72(sp)
2005f2ac:	c2ca                	c.swsp	s2,68(sp)
2005f2ae:	c0ce                	c.swsp	s3,64(sp)
2005f2b0:	de52                	c.swsp	s4,60(sp)
2005f2b2:	dc56                	c.swsp	s5,56(sp)
2005f2b4:	da5a                	c.swsp	s6,52(sp)
2005f2b6:	d85e                	c.swsp	s7,48(sp)
2005f2b8:	d662                	c.swsp	s8,44(sp)
2005f2ba:	d466                	c.swsp	s9,40(sp)
2005f2bc:	d26a                	c.swsp	s10,36(sp)
2005f2be:	d06e                	c.swsp	s11,32(sp)
2005f2c0:	ceb6                	c.swsp	a3,92(sp)
2005f2c2:	d0ba                	c.swsp	a4,96(sp)
2005f2c4:	d2be                	c.swsp	a5,100(sp)
2005f2c6:	d4c2                	c.swsp	a6,104(sp)
2005f2c8:	d6c6                	c.swsp	a7,108(sp)
2005f2ca:	28050463          	beq	a0,zero,2005f552 <DiagSnPrintf+0x2ac>
2005f2ce:	08fc                	c.addi4spn	a5,sp,92
2005f2d0:	c43e                	c.swsp	a5,8(sp)
2005f2d2:	537d                	c.li	t1,-1
2005f2d4:	c199                	c.beqz	a1,2005f2da <DiagSnPrintf+0x34>
2005f2d6:	00b50333          	add	t1,a0,a1
2005f2da:	200633b7          	lui	t2,0x20063
2005f2de:	6405                	c.lui	s0,0x1
2005f2e0:	882a                	c.mv	a6,a0
2005f2e2:	05000f13          	addi	t5,zero,80
2005f2e6:	06400913          	addi	s2,zero,100
2005f2ea:	06300993          	addi	s3,zero,99
2005f2ee:	4fa9                	c.li	t6,10
2005f2f0:	03000293          	addi	t0,zero,48
2005f2f4:	02d00a13          	addi	s4,zero,45
2005f2f8:	4b838393          	addi	t2,t2,1208 # 200634b8 <TaskStateString.0+0xa4>
2005f2fc:	4aa5                	c.li	s5,9
2005f2fe:	0ef00b13          	addi	s6,zero,239
2005f302:	eff40b93          	addi	s7,s0,-257 # eff <CPU_ClkGet+0x3e9>
2005f306:	6c3d                	c.lui	s8,0xf
2005f308:	7cc1                	c.lui	s9,0xffff0
2005f30a:	00064783          	lbu	a5,0(a2)
2005f30e:	e399                	c.bnez	a5,2005f314 <DiagSnPrintf+0x6e>
2005f310:	8742                	c.mv	a4,a6
2005f312:	a889                	c.j	2005f364 <DiagSnPrintf+0xbe>
2005f314:	02500713          	addi	a4,zero,37
2005f318:	00e78b63          	beq	a5,a4,2005f32e <DiagSnPrintf+0x88>
2005f31c:	00180713          	addi	a4,a6,1 # fff00001 <__ctrace_end__+0x35f00001>
2005f320:	00f80023          	sb	a5,0(a6)
2005f324:	04677063          	bgeu	a4,t1,2005f364 <DiagSnPrintf+0xbe>
2005f328:	0605                	c.addi	a2,1
2005f32a:	883a                	c.mv	a6,a4
2005f32c:	bff9                	c.j	2005f30a <DiagSnPrintf+0x64>
2005f32e:	4722                	c.lwsp	a4,8(sp)
2005f330:	00164e03          	lbu	t3,1(a2)
2005f334:	07300693          	addi	a3,zero,115
2005f338:	431c                	c.lw	a5,0(a4)
2005f33a:	00160593          	addi	a1,a2,1
2005f33e:	0711                	c.addi	a4,4
2005f340:	04de1463          	bne	t3,a3,2005f388 <DiagSnPrintf+0xe2>
2005f344:	c43a                	c.swsp	a4,8(sp)
2005f346:	410787b3          	sub	a5,a5,a6
2005f34a:	8742                	c.mv	a4,a6
2005f34c:	00f706b3          	add	a3,a4,a5
2005f350:	0006c683          	lbu	a3,0(a3)
2005f354:	e299                	c.bnez	a3,2005f35a <DiagSnPrintf+0xb4>
2005f356:	862e                	c.mv	a2,a1
2005f358:	bfc1                	c.j	2005f328 <DiagSnPrintf+0x82>
2005f35a:	0705                	c.addi	a4,1
2005f35c:	fed70fa3          	sb	a3,-1(a4)
2005f360:	fe6766e3          	bltu	a4,t1,2005f34c <DiagSnPrintf+0xa6>
2005f364:	00070023          	sb	zero,0(a4)
2005f368:	40a70533          	sub	a0,a4,a0
2005f36c:	4436                	c.lwsp	s0,76(sp)
2005f36e:	44a6                	c.lwsp	s1,72(sp)
2005f370:	4916                	c.lwsp	s2,68(sp)
2005f372:	4986                	c.lwsp	s3,64(sp)
2005f374:	5a72                	c.lwsp	s4,60(sp)
2005f376:	5ae2                	c.lwsp	s5,56(sp)
2005f378:	5b52                	c.lwsp	s6,52(sp)
2005f37a:	5bc2                	c.lwsp	s7,48(sp)
2005f37c:	5c32                	c.lwsp	s8,44(sp)
2005f37e:	5ca2                	c.lwsp	s9,40(sp)
2005f380:	5d12                	c.lwsp	s10,36(sp)
2005f382:	5d82                	c.lwsp	s11,32(sp)
2005f384:	6165                	c.addi16sp	sp,112
2005f386:	8082                	c.jr	ra
2005f388:	c43a                	c.swsp	a4,8(sp)
2005f38a:	0dfe7693          	andi	a3,t3,223
2005f38e:	473d                	c.li	a4,15
2005f390:	0de69563          	bne	a3,t5,2005f45a <DiagSnPrintf+0x1b4>
2005f394:	0cf75a63          	bge	a4,a5,2005f468 <DiagSnPrintf+0x1c2>
2005f398:	46a1                	c.li	a3,8
2005f39a:	03000893          	addi	a7,zero,48
2005f39e:	ff078e93          	addi	t4,a5,-16
2005f3a2:	4711                	c.li	a4,4
2005f3a4:	05db7463          	bgeu	s6,t4,2005f3ec <DiagSnPrintf+0x146>
2005f3a8:	f0078e93          	addi	t4,a5,-256
2005f3ac:	4721                	c.li	a4,8
2005f3ae:	03dbff63          	bgeu	s7,t4,2005f3ec <DiagSnPrintf+0x146>
2005f3b2:	40878eb3          	sub	t4,a5,s0
2005f3b6:	4731                	c.li	a4,12
2005f3b8:	038eea63          	bltu	t4,s8,2005f3ec <DiagSnPrintf+0x146>
2005f3bc:	01978eb3          	add	t4,a5,s9
2005f3c0:	000f04b7          	lui	s1,0xf0
2005f3c4:	4741                	c.li	a4,16
2005f3c6:	029ee363          	bltu	t4,s1,2005f3ec <DiagSnPrintf+0x146>
2005f3ca:	fff00eb7          	lui	t4,0xfff00
2005f3ce:	9ebe                	c.add	t4,a5
2005f3d0:	00f004b7          	lui	s1,0xf00
2005f3d4:	4751                	c.li	a4,20
2005f3d6:	009eeb63          	bltu	t4,s1,2005f3ec <DiagSnPrintf+0x146>
2005f3da:	ff000eb7          	lui	t4,0xff000
2005f3de:	9ebe                	c.add	t4,a5
2005f3e0:	0f0004b7          	lui	s1,0xf000
2005f3e4:	4761                	c.li	a4,24
2005f3e6:	009ee363          	bltu	t4,s1,2005f3ec <DiagSnPrintf+0x146>
2005f3ea:	4771                	c.li	a4,28
2005f3ec:	005e1663          	bne	t3,t0,2005f3f8 <DiagSnPrintf+0x152>
2005f3f0:	00260593          	addi	a1,a2,2
2005f3f4:	03000893          	addi	a7,zero,48
2005f3f8:	0005c603          	lbu	a2,0(a1)
2005f3fc:	44a1                	c.li	s1,8
2005f3fe:	fcf60e13          	addi	t3,a2,-49
2005f402:	0ffe7e13          	andi	t3,t3,255
2005f406:	03c4e163          	bltu	s1,t3,2005f428 <DiagSnPrintf+0x182>
2005f40a:	4681                	c.li	a3,0
2005f40c:	03f686b3          	mul	a3,a3,t6
2005f410:	0585                	c.addi	a1,1
2005f412:	96b2                	c.add	a3,a2
2005f414:	0005c603          	lbu	a2,0(a1)
2005f418:	fd068693          	addi	a3,a3,-48
2005f41c:	fd060e13          	addi	t3,a2,-48
2005f420:	0ffe7e13          	andi	t3,t3,255
2005f424:	ffcaf4e3          	bgeu	s5,t3,2005f40c <DiagSnPrintf+0x166>
2005f428:	0005c603          	lbu	a2,0(a1)
2005f42c:	0d767e13          	andi	t3,a2,215
2005f430:	0bee1463          	bne	t3,t5,2005f4d8 <DiagSnPrintf+0x232>
2005f434:	02067493          	andi	s1,a2,32
2005f438:	0df67613          	andi	a2,a2,223
2005f43c:	8ebe                	c.mv	t4,a5
2005f43e:	00c10e13          	addi	t3,sp,12
2005f442:	01e61a63          	bne	a2,t5,2005f456 <DiagSnPrintf+0x1b0>
2005f446:	0584e613          	ori	a2,s1,88
2005f44a:	00510623          	sb	t0,12(sp)
2005f44e:	00c106a3          	sb	a2,13(sp)
2005f452:	00e10e13          	addi	t3,sp,14
2005f456:	4601                	c.li	a2,0
2005f458:	a831                	c.j	2005f474 <DiagSnPrintf+0x1ce>
2005f45a:	4681                	c.li	a3,0
2005f45c:	02000893          	addi	a7,zero,32
2005f460:	f2f74fe3          	blt	a4,a5,2005f39e <DiagSnPrintf+0xf8>
2005f464:	4701                	c.li	a4,0
2005f466:	b759                	c.j	2005f3ec <DiagSnPrintf+0x146>
2005f468:	4701                	c.li	a4,0
2005f46a:	46a1                	c.li	a3,8
2005f46c:	b761                	c.j	2005f3f4 <DiagSnPrintf+0x14e>
2005f46e:	0605                	c.addi	a2,1
2005f470:	004ede93          	srli	t4,t4,0x4
2005f474:	fe0e9de3          	bne	t4,zero,2005f46e <DiagSnPrintf+0x1c8>
2005f478:	8ef2                	c.mv	t4,t3
2005f47a:	00de0db3          	add	s11,t3,a3
2005f47e:	41dd8d33          	sub	s10,s11,t4
2005f482:	05a64763          	blt	a2,s10,2005f4d0 <DiagSnPrintf+0x22a>
2005f486:	4881                	c.li	a7,0
2005f488:	00c6c463          	blt	a3,a2,2005f490 <DiagSnPrintf+0x1ea>
2005f48c:	40c688b3          	sub	a7,a3,a2
2005f490:	9e46                	c.add	t3,a7
2005f492:	88f2                	c.mv	a7,t3
2005f494:	863a                	c.mv	a2,a4
2005f496:	40c7d6b3          	sra	a3,a5,a2
2005f49a:	8abd                	c.andi	a3,15
2005f49c:	969e                	c.add	a3,t2
2005f49e:	0006c683          	lbu	a3,0(a3)
2005f4a2:	1671                	c.addi	a2,-4
2005f4a4:	0885                	c.addi	a7,1
2005f4a6:	8ec5                	c.or	a3,s1
2005f4a8:	fed88fa3          	sb	a3,-1(a7)
2005f4ac:	fe0655e3          	bge	a2,zero,2005f496 <DiagSnPrintf+0x1f0>
2005f4b0:	8309                	c.srli	a4,0x2
2005f4b2:	0705                	c.addi	a4,1
2005f4b4:	9e3a                	c.add	t3,a4
2005f4b6:	8742                	c.mv	a4,a6
2005f4b8:	007c                	c.addi4spn	a5,sp,12
2005f4ba:	e9c7fee3          	bgeu	a5,t3,2005f356 <DiagSnPrintf+0xb0>
2005f4be:	0007c683          	lbu	a3,0(a5)
2005f4c2:	0705                	c.addi	a4,1
2005f4c4:	fed70fa3          	sb	a3,-1(a4)
2005f4c8:	e8677ee3          	bgeu	a4,t1,2005f364 <DiagSnPrintf+0xbe>
2005f4cc:	0785                	c.addi	a5,1
2005f4ce:	b7f5                	c.j	2005f4ba <DiagSnPrintf+0x214>
2005f4d0:	011e8023          	sb	a7,0(t4) # ff000000 <__ctrace_end__+0x35000000>
2005f4d4:	0e85                	c.addi	t4,1
2005f4d6:	b765                	c.j	2005f47e <DiagSnPrintf+0x1d8>
2005f4d8:	07261363          	bne	a2,s2,2005f53e <DiagSnPrintf+0x298>
2005f4dc:	0407de63          	bge	a5,zero,2005f538 <DiagSnPrintf+0x292>
2005f4e0:	01410623          	sb	s4,12(sp)
2005f4e4:	40f007b3          	sub	a5,zero,a5
2005f4e8:	4885                	c.li	a7,1
2005f4ea:	00d10693          	addi	a3,sp,13
2005f4ee:	8e36                	c.mv	t3,a3
2005f4f0:	03f7e633          	rem	a2,a5,t6
2005f4f4:	0e05                	c.addi	t3,1
2005f4f6:	03f7c7b3          	div	a5,a5,t6
2005f4fa:	03060613          	addi	a2,a2,48
2005f4fe:	fece0fa3          	sb	a2,-1(t3)
2005f502:	011e0633          	add	a2,t3,a7
2005f506:	8e15                	c.sub	a2,a3
2005f508:	f7e5                	c.bnez	a5,2005f4f0 <DiagSnPrintf+0x24a>
2005f50a:	00c04563          	blt	zero,a2,2005f514 <DiagSnPrintf+0x26e>
2005f50e:	005e0023          	sb	t0,0(t3)
2005f512:	0e05                	c.addi	t3,1
2005f514:	1771                	c.addi	a4,-4
2005f516:	167d                	c.addi	a2,-1
2005f518:	fe0759e3          	bge	a4,zero,2005f50a <DiagSnPrintf+0x264>
2005f51c:	87f2                	c.mv	a5,t3
2005f51e:	17fd                	c.addi	a5,-1
2005f520:	f8f6fbe3          	bgeu	a3,a5,2005f4b6 <DiagSnPrintf+0x210>
2005f524:	0006c603          	lbu	a2,0(a3)
2005f528:	0007c703          	lbu	a4,0(a5)
2005f52c:	0685                	c.addi	a3,1
2005f52e:	00c78023          	sb	a2,0(a5)
2005f532:	fee68fa3          	sb	a4,-1(a3)
2005f536:	b7e5                	c.j	2005f51e <DiagSnPrintf+0x278>
2005f538:	4881                	c.li	a7,0
2005f53a:	0074                	c.addi4spn	a3,sp,12
2005f53c:	bf4d                	c.j	2005f4ee <DiagSnPrintf+0x248>
2005f53e:	01361763          	bne	a2,s3,2005f54c <DiagSnPrintf+0x2a6>
2005f542:	00f10623          	sb	a5,12(sp)
2005f546:	00d10e13          	addi	t3,sp,13
2005f54a:	b7b5                	c.j	2005f4b6 <DiagSnPrintf+0x210>
2005f54c:	00c10623          	sb	a2,12(sp)
2005f550:	bfdd                	c.j	2005f546 <DiagSnPrintf+0x2a0>
2005f552:	4501                	c.li	a0,0
2005f554:	bd21                	c.j	2005f36c <DiagSnPrintf+0xc6>

2005f556 <TRNG_get_random_bytes>:
2005f556:	7179                	c.addi16sp	sp,-48
2005f558:	d422                	c.swsp	s0,40(sp)
2005f55a:	d226                	c.swsp	s1,36(sp)
2005f55c:	d606                	c.swsp	ra,44(sp)
2005f55e:	d04a                	c.swsp	s2,32(sp)
2005f560:	ce4e                	c.swsp	s3,28(sp)
2005f562:	4100c7b7          	lui	a5,0x4100c
2005f566:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005f56a:	84aa                	c.mv	s1,a0
2005f56c:	842e                	c.mv	s0,a1
2005f56e:	8399                	c.srli	a5,0x6
2005f570:	8b8d                	c.andi	a5,3
2005f572:	ef9d                	c.bnez	a5,2005f5b0 <TRNG_get_random_bytes+0x5a>
2005f574:	00357793          	andi	a5,a0,3
2005f578:	cf85                	c.beqz	a5,2005f5b0 <TRNG_get_random_bytes+0x5a>
2005f57a:	852e                	c.mv	a0,a1
2005f57c:	938fc0ef          	jal	ra,2005b6b4 <rtos_mem_malloc>
2005f580:	85a2                	c.mv	a1,s0
2005f582:	892a                	c.mv	s2,a0
2005f584:	dffa9097          	auipc	ra,0xdffa9
2005f588:	7e6080e7          	jalr	ra,2022(ra) # 8d6a <RandBytes_Get>
2005f58c:	8622                	c.mv	a2,s0
2005f58e:	85ca                	c.mv	a1,s2
2005f590:	8526                	c.mv	a0,s1
2005f592:	dffaa097          	auipc	ra,0xdffaa
2005f596:	53e080e7          	jalr	ra,1342(ra) # 9ad0 <__wrap_memcpy>
2005f59a:	854a                	c.mv	a0,s2
2005f59c:	942fc0ef          	jal	ra,2005b6de <rtos_mem_free>
2005f5a0:	50b2                	c.lwsp	ra,44(sp)
2005f5a2:	5422                	c.lwsp	s0,40(sp)
2005f5a4:	5492                	c.lwsp	s1,36(sp)
2005f5a6:	5902                	c.lwsp	s2,32(sp)
2005f5a8:	49f2                	c.lwsp	s3,28(sp)
2005f5aa:	4501                	c.li	a0,0
2005f5ac:	6145                	c.addi16sp	sp,48
2005f5ae:	8082                	c.jr	ra
2005f5b0:	00245913          	srli	s2,s0,0x2
2005f5b4:	4981                	c.li	s3,0
2005f5b6:	0329c463          	blt	s3,s2,2005f5de <TRNG_get_random_bytes+0x88>
2005f5ba:	5671                	c.li	a2,-4
2005f5bc:	02c90633          	mul	a2,s2,a2
2005f5c0:	9432                	c.add	s0,a2
2005f5c2:	dc79                	c.beqz	s0,2005f5a0 <TRNG_get_random_bytes+0x4a>
2005f5c4:	f6aff0ef          	jal	ra,2005ed2e <_rand>
2005f5c8:	c62a                	c.swsp	a0,12(sp)
2005f5ca:	00291513          	slli	a0,s2,0x2
2005f5ce:	8622                	c.mv	a2,s0
2005f5d0:	006c                	c.addi4spn	a1,sp,12
2005f5d2:	9526                	c.add	a0,s1
2005f5d4:	dffaa097          	auipc	ra,0xdffaa
2005f5d8:	03e080e7          	jalr	ra,62(ra) # 9612 <_memcpy>
2005f5dc:	b7d1                	c.j	2005f5a0 <TRNG_get_random_bytes+0x4a>
2005f5de:	f50ff0ef          	jal	ra,2005ed2e <_rand>
2005f5e2:	00299793          	slli	a5,s3,0x2
2005f5e6:	97a6                	c.add	a5,s1
2005f5e8:	c388                	c.sw	a0,0(a5)
2005f5ea:	0985                	c.addi	s3,1
2005f5ec:	b7e9                	c.j	2005f5b6 <TRNG_get_random_bytes+0x60>

2005f5ee <io_assert_failed>:
2005f5ee:	862e                	c.mv	a2,a1
2005f5f0:	85aa                	c.mv	a1,a0
2005f5f2:	20063537          	lui	a0,0x20063
2005f5f6:	1141                	c.addi	sp,-16
2005f5f8:	4cc50513          	addi	a0,a0,1228 # 200634cc <TaskStateString.0+0xb8>
2005f5fc:	c606                	c.swsp	ra,12(sp)
2005f5fe:	b01ff0ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005f602:	a001                	c.j	2005f602 <io_assert_failed+0x14>

2005f604 <irq_set_priority>:
2005f604:	471d                	c.li	a4,7
2005f606:	02b77563          	bgeu	a4,a1,2005f630 <irq_set_priority+0x2c>
2005f60a:	862e                	c.mv	a2,a1
2005f60c:	85aa                	c.mv	a1,a0
2005f60e:	20063537          	lui	a0,0x20063
2005f612:	1141                	c.addi	sp,-16
2005f614:	469d                	c.li	a3,7
2005f616:	5a850513          	addi	a0,a0,1448 # 200635a8 <TaskStateString.0+0x194>
2005f61a:	c606                	c.swsp	ra,12(sp)
2005f61c:	ae3ff0ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2005f620:	20063537          	lui	a0,0x20063
2005f624:	44800593          	addi	a1,zero,1096
2005f628:	50050513          	addi	a0,a0,1280 # 20063500 <TaskStateString.0+0xec>
2005f62c:	fc3ff0ef          	jal	ra,2005f5ee <io_assert_failed>
2005f630:	73e00067          	jalr	zero,1854(zero) # 0 <Reset_Handler>

2005f634 <irq_register>:
2005f634:	20000737          	lui	a4,0x20000
2005f638:	1141                	c.addi	sp,-16
2005f63a:	00259893          	slli	a7,a1,0x2
2005f63e:	882a                	c.mv	a6,a0
2005f640:	00070713          	addi	a4,a4,0 # 20000000 <UserIrqFunTable>
2005f644:	200007b7          	lui	a5,0x20000
2005f648:	c606                	c.swsp	ra,12(sp)
2005f64a:	9746                	c.add	a4,a7
2005f64c:	00186813          	ori	a6,a6,1
2005f650:	1c078793          	addi	a5,a5,448 # 200001c0 <UserIrqDataTable>
2005f654:	01072023          	sw	a6,0(a4)
2005f658:	97c6                	c.add	a5,a7
2005f65a:	852e                	c.mv	a0,a1
2005f65c:	c390                	c.sw	a2,0(a5)
2005f65e:	85b6                	c.mv	a1,a3
2005f660:	fa5ff0ef          	jal	ra,2005f604 <irq_set_priority>
2005f664:	40b2                	c.lwsp	ra,12(sp)
2005f666:	4505                	c.li	a0,1
2005f668:	0141                	c.addi	sp,16
2005f66a:	8082                	c.jr	ra

2005f66c <unwind_frame_rv32i>:
2005f66c:	715d                	c.addi16sp	sp,-80
2005f66e:	c686                	c.swsp	ra,76(sp)
2005f670:	c4a2                	c.swsp	s0,72(sp)
2005f672:	0880                	c.addi4spn	s0,sp,80
2005f674:	faa42e23          	sw	a0,-68(s0)
2005f678:	fab42c23          	sw	a1,-72(s0)
2005f67c:	fe042623          	sw	zero,-20(s0)
2005f680:	fe042423          	sw	zero,-24(s0)
2005f684:	fbc42783          	lw	a5,-68(s0)
2005f688:	43dc                	c.lw	a5,4(a5)
2005f68a:	c789                	c.beqz	a5,2005f694 <unwind_frame_rv32i+0x28>
2005f68c:	fbc42783          	lw	a5,-68(s0)
2005f690:	439c                	c.lw	a5,0(a5)
2005f692:	e399                	c.bnez	a5,2005f698 <unwind_frame_rv32i+0x2c>
2005f694:	57fd                	c.li	a5,-1
2005f696:	a44d                	c.j	2005f938 <unwind_frame_rv32i+0x2cc>
2005f698:	fbc42783          	lw	a5,-68(s0)
2005f69c:	43dc                	c.lw	a5,4(a5)
2005f69e:	fef42223          	sw	a5,-28(s0)
2005f6a2:	a435                	c.j	2005f8ce <unwind_frame_rv32i+0x262>
2005f6a4:	fe442783          	lw	a5,-28(s0)
2005f6a8:	8b89                	c.andi	a5,2
2005f6aa:	c385                	c.beqz	a5,2005f6ca <unwind_frame_rv32i+0x5e>
2005f6ac:	fe442783          	lw	a5,-28(s0)
2005f6b0:	0007d783          	lhu	a5,0(a5)
2005f6b4:	873e                	c.mv	a4,a5
2005f6b6:	fe442783          	lw	a5,-28(s0)
2005f6ba:	0789                	c.addi	a5,2
2005f6bc:	0007d783          	lhu	a5,0(a5)
2005f6c0:	07c2                	c.slli	a5,0x10
2005f6c2:	8fd9                	c.or	a5,a4
2005f6c4:	fcf42c23          	sw	a5,-40(s0)
2005f6c8:	a031                	c.j	2005f6d4 <unwind_frame_rv32i+0x68>
2005f6ca:	fe442783          	lw	a5,-28(s0)
2005f6ce:	439c                	c.lw	a5,0(a5)
2005f6d0:	fcf42c23          	sw	a5,-40(s0)
2005f6d4:	fe442783          	lw	a5,-28(s0)
2005f6d8:	0007d783          	lhu	a5,0(a5)
2005f6dc:	fcf41a23          	sh	a5,-44(s0)
2005f6e0:	fe442783          	lw	a5,-28(s0)
2005f6e4:	0789                	c.addi	a5,2
2005f6e6:	0007d783          	lhu	a5,0(a5)
2005f6ea:	fcf41823          	sh	a5,-48(s0)
2005f6ee:	fd845783          	lhu	a5,-40(s0)
2005f6f2:	07f7f793          	andi	a5,a5,127
2005f6f6:	01079713          	slli	a4,a5,0x10
2005f6fa:	8341                	c.srli	a4,0x10
2005f6fc:	47cd                	c.li	a5,19
2005f6fe:	0af71c63          	bne	a4,a5,2005f7b6 <unwind_frame_rv32i+0x14a>
2005f702:	fd845703          	lhu	a4,-40(s0)
2005f706:	679d                	c.lui	a5,0x7
2005f708:	8ff9                	c.and	a5,a4
2005f70a:	07c2                	c.slli	a5,0x10
2005f70c:	83c1                	c.srli	a5,0x10
2005f70e:	e7c5                	c.bnez	a5,2005f7b6 <unwind_frame_rv32i+0x14a>
2005f710:	fd842703          	lw	a4,-40(s0)
2005f714:	6785                	c.lui	a5,0x1
2005f716:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005f71a:	8f7d                	c.and	a4,a5
2005f71c:	10000793          	addi	a5,zero,256
2005f720:	18f71f63          	bne	a4,a5,2005f8be <unwind_frame_rv32i+0x252>
2005f724:	fd842703          	lw	a4,-40(s0)
2005f728:	000f87b7          	lui	a5,0xf8
2005f72c:	8f7d                	c.and	a4,a5
2005f72e:	67c1                	c.lui	a5,0x10
2005f730:	18f71763          	bne	a4,a5,2005f8be <unwind_frame_rv32i+0x252>
2005f734:	fd842783          	lw	a5,-40(s0)
2005f738:	83d1                	c.srli	a5,0x14
2005f73a:	07c2                	c.slli	a5,0x10
2005f73c:	83c1                	c.srli	a5,0x10
2005f73e:	0792                	c.slli	a5,0x4
2005f740:	07c2                	c.slli	a5,0x10
2005f742:	87c1                	c.srai	a5,0x10
2005f744:	8791                	c.srai	a5,0x4
2005f746:	07c2                	c.slli	a5,0x10
2005f748:	87c1                	c.srai	a5,0x10
2005f74a:	873e                	c.mv	a4,a5
2005f74c:	6785                	c.lui	a5,0x1
2005f74e:	17fd                	c.addi	a5,-1
2005f750:	00f776b3          	and	a3,a4,a5
2005f754:	fcc45703          	lhu	a4,-52(s0)
2005f758:	77fd                	c.lui	a5,0xfffff
2005f75a:	8ff9                	c.and	a5,a4
2005f75c:	873e                	c.mv	a4,a5
2005f75e:	87b6                	c.mv	a5,a3
2005f760:	8fd9                	c.or	a5,a4
2005f762:	fcf41623          	sh	a5,-52(s0)
2005f766:	fcc42783          	lw	a5,-52(s0)
2005f76a:	0792                	c.slli	a5,0x4
2005f76c:	07c2                	c.slli	a5,0x10
2005f76e:	87c1                	c.srai	a5,0x10
2005f770:	8791                	c.srai	a5,0x4
2005f772:	07c2                	c.slli	a5,0x10
2005f774:	87c1                	c.srai	a5,0x10
2005f776:	fef42023          	sw	a5,-32(s0)
2005f77a:	fe042783          	lw	a5,-32(s0)
2005f77e:	02f05363          	bge	zero,a5,2005f7a4 <unwind_frame_rv32i+0x138>
2005f782:	fe842783          	lw	a5,-24(s0)
2005f786:	86be                	c.mv	a3,a5
2005f788:	10d00613          	addi	a2,zero,269
2005f78c:	200637b7          	lui	a5,0x20063
2005f790:	7cc78593          	addi	a1,a5,1996 # 200637cc <__FUNCTION__.3>
2005f794:	200637b7          	lui	a5,0x20063
2005f798:	5cc78513          	addi	a0,a5,1484 # 200635cc <TaskStateString.0+0x1b8>
2005f79c:	943ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005f7a0:	57fd                	c.li	a5,-1
2005f7a2:	aa59                	c.j	2005f938 <unwind_frame_rv32i+0x2cc>
2005f7a4:	fe042783          	lw	a5,-32(s0)
2005f7a8:	0007d463          	bge	a5,zero,2005f7b0 <unwind_frame_rv32i+0x144>
2005f7ac:	40f007b3          	sub	a5,zero,a5
2005f7b0:	fef42423          	sw	a5,-24(s0)
2005f7b4:	a229                	c.j	2005f8be <unwind_frame_rv32i+0x252>
2005f7b6:	fd845783          	lhu	a5,-40(s0)
2005f7ba:	07f7f793          	andi	a5,a5,127
2005f7be:	01079713          	slli	a4,a5,0x10
2005f7c2:	8341                	c.srli	a4,0x10
2005f7c4:	02300793          	addi	a5,zero,35
2005f7c8:	0cf71463          	bne	a4,a5,2005f890 <unwind_frame_rv32i+0x224>
2005f7cc:	fd845703          	lhu	a4,-40(s0)
2005f7d0:	679d                	c.lui	a5,0x7
2005f7d2:	8ff9                	c.and	a5,a4
2005f7d4:	01079713          	slli	a4,a5,0x10
2005f7d8:	8341                	c.srli	a4,0x10
2005f7da:	6789                	c.lui	a5,0x2
2005f7dc:	0af71a63          	bne	a4,a5,2005f890 <unwind_frame_rv32i+0x224>
2005f7e0:	fd842703          	lw	a4,-40(s0)
2005f7e4:	000f87b7          	lui	a5,0xf8
2005f7e8:	8f7d                	c.and	a4,a5
2005f7ea:	67c1                	c.lui	a5,0x10
2005f7ec:	0cf71b63          	bne	a4,a5,2005f8c2 <unwind_frame_rv32i+0x256>
2005f7f0:	fd842703          	lw	a4,-40(s0)
2005f7f4:	01f007b7          	lui	a5,0x1f00
2005f7f8:	8f7d                	c.and	a4,a5
2005f7fa:	001007b7          	lui	a5,0x100
2005f7fe:	0cf71263          	bne	a4,a5,2005f8c2 <unwind_frame_rv32i+0x256>
2005f802:	fd842783          	lw	a5,-40(s0)
2005f806:	83e5                	c.srli	a5,0x19
2005f808:	0ff7f793          	andi	a5,a5,255
2005f80c:	0796                	c.slli	a5,0x5
2005f80e:	01079713          	slli	a4,a5,0x10
2005f812:	8741                	c.srai	a4,0x10
2005f814:	fd842783          	lw	a5,-40(s0)
2005f818:	839d                	c.srli	a5,0x7
2005f81a:	8bfd                	c.andi	a5,31
2005f81c:	0ff7f793          	andi	a5,a5,255
2005f820:	07c2                	c.slli	a5,0x10
2005f822:	87c1                	c.srai	a5,0x10
2005f824:	8fd9                	c.or	a5,a4
2005f826:	07c2                	c.slli	a5,0x10
2005f828:	87c1                	c.srai	a5,0x10
2005f82a:	0792                	c.slli	a5,0x4
2005f82c:	07c2                	c.slli	a5,0x10
2005f82e:	87c1                	c.srai	a5,0x10
2005f830:	8791                	c.srai	a5,0x4
2005f832:	07c2                	c.slli	a5,0x10
2005f834:	87c1                	c.srai	a5,0x10
2005f836:	873e                	c.mv	a4,a5
2005f838:	6785                	c.lui	a5,0x1
2005f83a:	17fd                	c.addi	a5,-1
2005f83c:	00f776b3          	and	a3,a4,a5
2005f840:	fc845703          	lhu	a4,-56(s0)
2005f844:	77fd                	c.lui	a5,0xfffff
2005f846:	8ff9                	c.and	a5,a4
2005f848:	873e                	c.mv	a4,a5
2005f84a:	87b6                	c.mv	a5,a3
2005f84c:	8fd9                	c.or	a5,a4
2005f84e:	fcf41423          	sh	a5,-56(s0)
2005f852:	fc842783          	lw	a5,-56(s0)
2005f856:	0792                	c.slli	a5,0x4
2005f858:	07c2                	c.slli	a5,0x10
2005f85a:	87c1                	c.srai	a5,0x10
2005f85c:	8791                	c.srai	a5,0x4
2005f85e:	07c2                	c.slli	a5,0x10
2005f860:	87c1                	c.srai	a5,0x10
2005f862:	fef42623          	sw	a5,-20(s0)
2005f866:	fec42783          	lw	a5,-20(s0)
2005f86a:	0407dc63          	bge	a5,zero,2005f8c2 <unwind_frame_rv32i+0x256>
2005f86e:	fec42783          	lw	a5,-20(s0)
2005f872:	86be                	c.mv	a3,a5
2005f874:	11d00613          	addi	a2,zero,285
2005f878:	200637b7          	lui	a5,0x20063
2005f87c:	7cc78593          	addi	a1,a5,1996 # 200637cc <__FUNCTION__.3>
2005f880:	200637b7          	lui	a5,0x20063
2005f884:	60878513          	addi	a0,a5,1544 # 20063608 <TaskStateString.0+0x1f4>
2005f888:	857ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005f88c:	57fd                	c.li	a5,-1
2005f88e:	a06d                	c.j	2005f938 <unwind_frame_rv32i+0x2cc>
2005f890:	fd842703          	lw	a4,-40(s0)
2005f894:	67a1                	c.lui	a5,0x8
2005f896:	06778793          	addi	a5,a5,103 # 8067 <irq_unregister+0x41>
2005f89a:	06f70063          	beq	a4,a5,2005f8fa <unwind_frame_rv32i+0x28e>
2005f89e:	fd445703          	lhu	a4,-44(s0)
2005f8a2:	67a1                	c.lui	a5,0x8
2005f8a4:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005f8a8:	00f70963          	beq	a4,a5,2005f8ba <unwind_frame_rv32i+0x24e>
2005f8ac:	fd045703          	lhu	a4,-48(s0)
2005f8b0:	67a1                	c.lui	a5,0x8
2005f8b2:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005f8b6:	00f71763          	bne	a4,a5,2005f8c4 <unwind_frame_rv32i+0x258>
2005f8ba:	57fd                	c.li	a5,-1
2005f8bc:	a8b5                	c.j	2005f938 <unwind_frame_rv32i+0x2cc>
2005f8be:	0001                	c.addi	zero,0
2005f8c0:	a011                	c.j	2005f8c4 <unwind_frame_rv32i+0x258>
2005f8c2:	0001                	c.addi	zero,0
2005f8c4:	fe442783          	lw	a5,-28(s0)
2005f8c8:	17f1                	c.addi	a5,-4
2005f8ca:	fef42223          	sw	a5,-28(s0)
2005f8ce:	fb842783          	lw	a5,-72(s0)
2005f8d2:	078a                	c.slli	a5,0x2
2005f8d4:	fe442703          	lw	a4,-28(s0)
2005f8d8:	973e                	c.add	a4,a5
2005f8da:	fbc42783          	lw	a5,-68(s0)
2005f8de:	43dc                	c.lw	a5,4(a5)
2005f8e0:	00e7fe63          	bgeu	a5,a4,2005f8fc <unwind_frame_rv32i+0x290>
2005f8e4:	fec42783          	lw	a5,-20(s0)
2005f8e8:	c781                	c.beqz	a5,2005f8f0 <unwind_frame_rv32i+0x284>
2005f8ea:	fe842783          	lw	a5,-24(s0)
2005f8ee:	e799                	c.bnez	a5,2005f8fc <unwind_frame_rv32i+0x290>
2005f8f0:	fe442783          	lw	a5,-28(s0)
2005f8f4:	da0798e3          	bne	a5,zero,2005f6a4 <unwind_frame_rv32i+0x38>
2005f8f8:	a011                	c.j	2005f8fc <unwind_frame_rv32i+0x290>
2005f8fa:	0001                	c.addi	zero,0
2005f8fc:	fec42783          	lw	a5,-20(s0)
2005f900:	cb91                	c.beqz	a5,2005f914 <unwind_frame_rv32i+0x2a8>
2005f902:	fbc42783          	lw	a5,-68(s0)
2005f906:	4398                	c.lw	a4,0(a5)
2005f908:	fec42783          	lw	a5,-20(s0)
2005f90c:	97ba                	c.add	a5,a4
2005f90e:	439c                	c.lw	a5,0(a5)
2005f910:	873e                	c.mv	a4,a5
2005f912:	a011                	c.j	2005f916 <unwind_frame_rv32i+0x2aa>
2005f914:	4701                	c.li	a4,0
2005f916:	fbc42783          	lw	a5,-68(s0)
2005f91a:	c798                	c.sw	a4,8(a5)
2005f91c:	fe842703          	lw	a4,-24(s0)
2005f920:	fbc42783          	lw	a5,-68(s0)
2005f924:	c7d8                	c.sw	a4,12(a5)
2005f926:	fec42783          	lw	a5,-20(s0)
2005f92a:	c781                	c.beqz	a5,2005f932 <unwind_frame_rv32i+0x2c6>
2005f92c:	fe842783          	lw	a5,-24(s0)
2005f930:	e399                	c.bnez	a5,2005f936 <unwind_frame_rv32i+0x2ca>
2005f932:	57fd                	c.li	a5,-1
2005f934:	a011                	c.j	2005f938 <unwind_frame_rv32i+0x2cc>
2005f936:	4781                	c.li	a5,0
2005f938:	853e                	c.mv	a0,a5
2005f93a:	40b6                	c.lwsp	ra,76(sp)
2005f93c:	4426                	c.lwsp	s0,72(sp)
2005f93e:	6161                	c.addi16sp	sp,80
2005f940:	8082                	c.jr	ra

2005f942 <unwind_frame_rv32c>:
2005f942:	715d                	c.addi16sp	sp,-80
2005f944:	c686                	c.swsp	ra,76(sp)
2005f946:	c4a2                	c.swsp	s0,72(sp)
2005f948:	0880                	c.addi4spn	s0,sp,80
2005f94a:	faa42e23          	sw	a0,-68(s0)
2005f94e:	fab42c23          	sw	a1,-72(s0)
2005f952:	fe042623          	sw	zero,-20(s0)
2005f956:	fe042423          	sw	zero,-24(s0)
2005f95a:	fbc42783          	lw	a5,-68(s0)
2005f95e:	43dc                	c.lw	a5,4(a5)
2005f960:	cb91                	c.beqz	a5,2005f974 <unwind_frame_rv32c+0x32>
2005f962:	fbc42783          	lw	a5,-68(s0)
2005f966:	439c                	c.lw	a5,0(a5)
2005f968:	c791                	c.beqz	a5,2005f974 <unwind_frame_rv32c+0x32>
2005f96a:	fbc42783          	lw	a5,-68(s0)
2005f96e:	43dc                	c.lw	a5,4(a5)
2005f970:	8b85                	c.andi	a5,1
2005f972:	c399                	c.beqz	a5,2005f978 <unwind_frame_rv32c+0x36>
2005f974:	57fd                	c.li	a5,-1
2005f976:	ae3d                	c.j	2005fcb4 <unwind_frame_rv32c+0x372>
2005f978:	fbc42783          	lw	a5,-68(s0)
2005f97c:	43dc                	c.lw	a5,4(a5)
2005f97e:	fef42223          	sw	a5,-28(s0)
2005f982:	a4e1                	c.j	2005fc4a <unwind_frame_rv32c+0x308>
2005f984:	fe442783          	lw	a5,-28(s0)
2005f988:	0007d783          	lhu	a5,0(a5)
2005f98c:	fcf41c23          	sh	a5,-40(s0)
2005f990:	fd845703          	lhu	a4,-40(s0)
2005f994:	77f9                	c.lui	a5,0xffffe
2005f996:	8ff9                	c.and	a5,a4
2005f998:	07c2                	c.slli	a5,0x10
2005f99a:	83c1                	c.srli	a5,0x10
2005f99c:	e7e9                	c.bnez	a5,2005fa66 <unwind_frame_rv32c+0x124>
2005f99e:	fd845783          	lhu	a5,-40(s0)
2005f9a2:	8b8d                	c.andi	a5,3
2005f9a4:	01079713          	slli	a4,a5,0x10
2005f9a8:	8341                	c.srli	a4,0x10
2005f9aa:	4785                	c.li	a5,1
2005f9ac:	0af71d63          	bne	a4,a5,2005fa66 <unwind_frame_rv32c+0x124>
2005f9b0:	fd845703          	lhu	a4,-40(s0)
2005f9b4:	6785                	c.lui	a5,0x1
2005f9b6:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005f9ba:	8ff9                	c.and	a5,a4
2005f9bc:	01079713          	slli	a4,a5,0x10
2005f9c0:	8341                	c.srli	a4,0x10
2005f9c2:	10000793          	addi	a5,zero,256
2005f9c6:	0af71063          	bne	a4,a5,2005fa66 <unwind_frame_rv32c+0x124>
2005f9ca:	fd842783          	lw	a5,-40(s0)
2005f9ce:	83b1                	c.srli	a5,0xc
2005f9d0:	8b85                	c.andi	a5,1
2005f9d2:	0ff7f793          	andi	a5,a5,255
2005f9d6:	0796                	c.slli	a5,0x5
2005f9d8:	01879713          	slli	a4,a5,0x18
2005f9dc:	8761                	c.srai	a4,0x18
2005f9de:	fd842783          	lw	a5,-40(s0)
2005f9e2:	8389                	c.srli	a5,0x2
2005f9e4:	8bfd                	c.andi	a5,31
2005f9e6:	0ff7f793          	andi	a5,a5,255
2005f9ea:	07e2                	c.slli	a5,0x18
2005f9ec:	87e1                	c.srai	a5,0x18
2005f9ee:	8fd9                	c.or	a5,a4
2005f9f0:	07e2                	c.slli	a5,0x18
2005f9f2:	87e1                	c.srai	a5,0x18
2005f9f4:	078a                	c.slli	a5,0x2
2005f9f6:	07e2                	c.slli	a5,0x18
2005f9f8:	87e1                	c.srai	a5,0x18
2005f9fa:	8789                	c.srai	a5,0x2
2005f9fc:	07e2                	c.slli	a5,0x18
2005f9fe:	87e1                	c.srai	a5,0x18
2005fa00:	03f7f693          	andi	a3,a5,63
2005fa04:	fd444783          	lbu	a5,-44(s0)
2005fa08:	fc07f793          	andi	a5,a5,-64
2005fa0c:	873e                	c.mv	a4,a5
2005fa0e:	87b6                	c.mv	a5,a3
2005fa10:	8fd9                	c.or	a5,a4
2005fa12:	fcf40a23          	sb	a5,-44(s0)
2005fa16:	fd442783          	lw	a5,-44(s0)
2005fa1a:	078a                	c.slli	a5,0x2
2005fa1c:	07e2                	c.slli	a5,0x18
2005fa1e:	87e1                	c.srai	a5,0x18
2005fa20:	8789                	c.srai	a5,0x2
2005fa22:	07e2                	c.slli	a5,0x18
2005fa24:	87e1                	c.srai	a5,0x18
2005fa26:	fef42023          	sw	a5,-32(s0)
2005fa2a:	fe042783          	lw	a5,-32(s0)
2005fa2e:	02f05363          	bge	zero,a5,2005fa54 <unwind_frame_rv32c+0x112>
2005fa32:	fe842783          	lw	a5,-24(s0)
2005fa36:	86be                	c.mv	a3,a5
2005fa38:	16900613          	addi	a2,zero,361
2005fa3c:	200637b7          	lui	a5,0x20063
2005fa40:	7e078593          	addi	a1,a5,2016 # 200637e0 <__FUNCTION__.2>
2005fa44:	200637b7          	lui	a5,0x20063
2005fa48:	5cc78513          	addi	a0,a5,1484 # 200635cc <TaskStateString.0+0x1b8>
2005fa4c:	e92ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fa50:	57fd                	c.li	a5,-1
2005fa52:	a48d                	c.j	2005fcb4 <unwind_frame_rv32c+0x372>
2005fa54:	fe042783          	lw	a5,-32(s0)
2005fa58:	0007d463          	bge	a5,zero,2005fa60 <unwind_frame_rv32c+0x11e>
2005fa5c:	40f007b3          	sub	a5,zero,a5
2005fa60:	fef42423          	sw	a5,-24(s0)
2005fa64:	aaf1                	c.j	2005fc40 <unwind_frame_rv32c+0x2fe>
2005fa66:	fd845703          	lhu	a4,-40(s0)
2005fa6a:	77f9                	c.lui	a5,0xffffe
2005fa6c:	8ff9                	c.and	a5,a4
2005fa6e:	01079713          	slli	a4,a5,0x10
2005fa72:	8341                	c.srli	a4,0x10
2005fa74:	6799                	c.lui	a5,0x6
2005fa76:	10f71e63          	bne	a4,a5,2005fb92 <unwind_frame_rv32c+0x250>
2005fa7a:	fd845783          	lhu	a5,-40(s0)
2005fa7e:	8b8d                	c.andi	a5,3
2005fa80:	01079713          	slli	a4,a5,0x10
2005fa84:	8341                	c.srli	a4,0x10
2005fa86:	4785                	c.li	a5,1
2005fa88:	10f71563          	bne	a4,a5,2005fb92 <unwind_frame_rv32c+0x250>
2005fa8c:	fd845703          	lhu	a4,-40(s0)
2005fa90:	6785                	c.lui	a5,0x1
2005fa92:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fa96:	8ff9                	c.and	a5,a4
2005fa98:	01079713          	slli	a4,a5,0x10
2005fa9c:	8341                	c.srli	a4,0x10
2005fa9e:	10000793          	addi	a5,zero,256
2005faa2:	0ef71863          	bne	a4,a5,2005fb92 <unwind_frame_rv32c+0x250>
2005faa6:	fd842783          	lw	a5,-40(s0)
2005faaa:	83b1                	c.srli	a5,0xc
2005faac:	8b85                	c.andi	a5,1
2005faae:	0ff7f793          	andi	a5,a5,255
2005fab2:	07a6                	c.slli	a5,0x9
2005fab4:	01079713          	slli	a4,a5,0x10
2005fab8:	8741                	c.srai	a4,0x10
2005faba:	fd842783          	lw	a5,-40(s0)
2005fabe:	8399                	c.srli	a5,0x6
2005fac0:	8b85                	c.andi	a5,1
2005fac2:	0ff7f793          	andi	a5,a5,255
2005fac6:	0792                	c.slli	a5,0x4
2005fac8:	07c2                	c.slli	a5,0x10
2005faca:	87c1                	c.srai	a5,0x10
2005facc:	8fd9                	c.or	a5,a4
2005face:	01079713          	slli	a4,a5,0x10
2005fad2:	8741                	c.srai	a4,0x10
2005fad4:	fd842783          	lw	a5,-40(s0)
2005fad8:	8395                	c.srli	a5,0x5
2005fada:	8b85                	c.andi	a5,1
2005fadc:	0ff7f793          	andi	a5,a5,255
2005fae0:	079a                	c.slli	a5,0x6
2005fae2:	07c2                	c.slli	a5,0x10
2005fae4:	87c1                	c.srai	a5,0x10
2005fae6:	8fd9                	c.or	a5,a4
2005fae8:	01079713          	slli	a4,a5,0x10
2005faec:	8741                	c.srai	a4,0x10
2005faee:	fd842783          	lw	a5,-40(s0)
2005faf2:	838d                	c.srli	a5,0x3
2005faf4:	8b8d                	c.andi	a5,3
2005faf6:	0ff7f793          	andi	a5,a5,255
2005fafa:	079e                	c.slli	a5,0x7
2005fafc:	07c2                	c.slli	a5,0x10
2005fafe:	87c1                	c.srai	a5,0x10
2005fb00:	8fd9                	c.or	a5,a4
2005fb02:	01079713          	slli	a4,a5,0x10
2005fb06:	8741                	c.srai	a4,0x10
2005fb08:	fd842783          	lw	a5,-40(s0)
2005fb0c:	8389                	c.srli	a5,0x2
2005fb0e:	8b85                	c.andi	a5,1
2005fb10:	0ff7f793          	andi	a5,a5,255
2005fb14:	0796                	c.slli	a5,0x5
2005fb16:	07c2                	c.slli	a5,0x10
2005fb18:	87c1                	c.srai	a5,0x10
2005fb1a:	8fd9                	c.or	a5,a4
2005fb1c:	07c2                	c.slli	a5,0x10
2005fb1e:	87c1                	c.srai	a5,0x10
2005fb20:	079a                	c.slli	a5,0x6
2005fb22:	07c2                	c.slli	a5,0x10
2005fb24:	87c1                	c.srai	a5,0x10
2005fb26:	8799                	c.srai	a5,0x6
2005fb28:	07c2                	c.slli	a5,0x10
2005fb2a:	87c1                	c.srai	a5,0x10
2005fb2c:	3ff7f693          	andi	a3,a5,1023
2005fb30:	fd045783          	lhu	a5,-48(s0)
2005fb34:	c007f793          	andi	a5,a5,-1024
2005fb38:	873e                	c.mv	a4,a5
2005fb3a:	87b6                	c.mv	a5,a3
2005fb3c:	8fd9                	c.or	a5,a4
2005fb3e:	fcf41823          	sh	a5,-48(s0)
2005fb42:	fd042783          	lw	a5,-48(s0)
2005fb46:	079a                	c.slli	a5,0x6
2005fb48:	07c2                	c.slli	a5,0x10
2005fb4a:	87c1                	c.srai	a5,0x10
2005fb4c:	8799                	c.srai	a5,0x6
2005fb4e:	07c2                	c.slli	a5,0x10
2005fb50:	87c1                	c.srai	a5,0x10
2005fb52:	fcf42e23          	sw	a5,-36(s0)
2005fb56:	fdc42783          	lw	a5,-36(s0)
2005fb5a:	02f05363          	bge	zero,a5,2005fb80 <unwind_frame_rv32c+0x23e>
2005fb5e:	fe842783          	lw	a5,-24(s0)
2005fb62:	86be                	c.mv	a3,a5
2005fb64:	18100613          	addi	a2,zero,385
2005fb68:	200637b7          	lui	a5,0x20063
2005fb6c:	7e078593          	addi	a1,a5,2016 # 200637e0 <__FUNCTION__.2>
2005fb70:	200637b7          	lui	a5,0x20063
2005fb74:	5cc78513          	addi	a0,a5,1484 # 200635cc <TaskStateString.0+0x1b8>
2005fb78:	d66ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fb7c:	57fd                	c.li	a5,-1
2005fb7e:	aa1d                	c.j	2005fcb4 <unwind_frame_rv32c+0x372>
2005fb80:	fdc42783          	lw	a5,-36(s0)
2005fb84:	0007d463          	bge	a5,zero,2005fb8c <unwind_frame_rv32c+0x24a>
2005fb88:	40f007b3          	sub	a5,zero,a5
2005fb8c:	fef42423          	sw	a5,-24(s0)
2005fb90:	a845                	c.j	2005fc40 <unwind_frame_rv32c+0x2fe>
2005fb92:	fd845703          	lhu	a4,-40(s0)
2005fb96:	77f9                	c.lui	a5,0xffffe
2005fb98:	8ff9                	c.and	a5,a4
2005fb9a:	01079713          	slli	a4,a5,0x10
2005fb9e:	8341                	c.srli	a4,0x10
2005fba0:	67b1                	c.lui	a5,0xc
2005fba2:	08f71663          	bne	a4,a5,2005fc2e <unwind_frame_rv32c+0x2ec>
2005fba6:	fd845783          	lhu	a5,-40(s0)
2005fbaa:	8b8d                	c.andi	a5,3
2005fbac:	01079713          	slli	a4,a5,0x10
2005fbb0:	8341                	c.srli	a4,0x10
2005fbb2:	4789                	c.li	a5,2
2005fbb4:	06f71d63          	bne	a4,a5,2005fc2e <unwind_frame_rv32c+0x2ec>
2005fbb8:	fd845783          	lhu	a5,-40(s0)
2005fbbc:	07c7f793          	andi	a5,a5,124
2005fbc0:	01079713          	slli	a4,a5,0x10
2005fbc4:	8341                	c.srli	a4,0x10
2005fbc6:	4791                	c.li	a5,4
2005fbc8:	06f71363          	bne	a4,a5,2005fc2e <unwind_frame_rv32c+0x2ec>
2005fbcc:	fd842783          	lw	a5,-40(s0)
2005fbd0:	839d                	c.srli	a5,0x7
2005fbd2:	8b8d                	c.andi	a5,3
2005fbd4:	0ff7f793          	andi	a5,a5,255
2005fbd8:	079a                	c.slli	a5,0x6
2005fbda:	01879713          	slli	a4,a5,0x18
2005fbde:	8761                	c.srai	a4,0x18
2005fbe0:	fd842783          	lw	a5,-40(s0)
2005fbe4:	83a5                	c.srli	a5,0x9
2005fbe6:	8bbd                	c.andi	a5,15
2005fbe8:	0ff7f793          	andi	a5,a5,255
2005fbec:	078a                	c.slli	a5,0x2
2005fbee:	07e2                	c.slli	a5,0x18
2005fbf0:	87e1                	c.srai	a5,0x18
2005fbf2:	8fd9                	c.or	a5,a4
2005fbf4:	07e2                	c.slli	a5,0x18
2005fbf6:	87e1                	c.srai	a5,0x18
2005fbf8:	fcf40623          	sb	a5,-52(s0)
2005fbfc:	fcc40783          	lb	a5,-52(s0)
2005fc00:	fef42623          	sw	a5,-20(s0)
2005fc04:	fec42783          	lw	a5,-20(s0)
2005fc08:	0207db63          	bge	a5,zero,2005fc3e <unwind_frame_rv32c+0x2fc>
2005fc0c:	fec42783          	lw	a5,-20(s0)
2005fc10:	86be                	c.mv	a3,a5
2005fc12:	19100613          	addi	a2,zero,401
2005fc16:	200637b7          	lui	a5,0x20063
2005fc1a:	7e078593          	addi	a1,a5,2016 # 200637e0 <__FUNCTION__.2>
2005fc1e:	200637b7          	lui	a5,0x20063
2005fc22:	60878513          	addi	a0,a5,1544 # 20063608 <TaskStateString.0+0x1f4>
2005fc26:	cb8ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fc2a:	57fd                	c.li	a5,-1
2005fc2c:	a061                	c.j	2005fcb4 <unwind_frame_rv32c+0x372>
2005fc2e:	fd845703          	lhu	a4,-40(s0)
2005fc32:	67a1                	c.lui	a5,0x8
2005fc34:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fc38:	02f70f63          	beq	a4,a5,2005fc76 <unwind_frame_rv32c+0x334>
2005fc3c:	a011                	c.j	2005fc40 <unwind_frame_rv32c+0x2fe>
2005fc3e:	0001                	c.addi	zero,0
2005fc40:	fe442783          	lw	a5,-28(s0)
2005fc44:	17f9                	c.addi	a5,-2
2005fc46:	fef42223          	sw	a5,-28(s0)
2005fc4a:	fb842783          	lw	a5,-72(s0)
2005fc4e:	0786                	c.slli	a5,0x1
2005fc50:	fe442703          	lw	a4,-28(s0)
2005fc54:	973e                	c.add	a4,a5
2005fc56:	fbc42783          	lw	a5,-68(s0)
2005fc5a:	43dc                	c.lw	a5,4(a5)
2005fc5c:	00e7fe63          	bgeu	a5,a4,2005fc78 <unwind_frame_rv32c+0x336>
2005fc60:	fec42783          	lw	a5,-20(s0)
2005fc64:	c781                	c.beqz	a5,2005fc6c <unwind_frame_rv32c+0x32a>
2005fc66:	fe842783          	lw	a5,-24(s0)
2005fc6a:	e799                	c.bnez	a5,2005fc78 <unwind_frame_rv32c+0x336>
2005fc6c:	fe442783          	lw	a5,-28(s0)
2005fc70:	d0079ae3          	bne	a5,zero,2005f984 <unwind_frame_rv32c+0x42>
2005fc74:	a011                	c.j	2005fc78 <unwind_frame_rv32c+0x336>
2005fc76:	0001                	c.addi	zero,0
2005fc78:	fec42783          	lw	a5,-20(s0)
2005fc7c:	cb91                	c.beqz	a5,2005fc90 <unwind_frame_rv32c+0x34e>
2005fc7e:	fbc42783          	lw	a5,-68(s0)
2005fc82:	4398                	c.lw	a4,0(a5)
2005fc84:	fec42783          	lw	a5,-20(s0)
2005fc88:	97ba                	c.add	a5,a4
2005fc8a:	439c                	c.lw	a5,0(a5)
2005fc8c:	873e                	c.mv	a4,a5
2005fc8e:	a011                	c.j	2005fc92 <unwind_frame_rv32c+0x350>
2005fc90:	4701                	c.li	a4,0
2005fc92:	fbc42783          	lw	a5,-68(s0)
2005fc96:	c798                	c.sw	a4,8(a5)
2005fc98:	fe842703          	lw	a4,-24(s0)
2005fc9c:	fbc42783          	lw	a5,-68(s0)
2005fca0:	c7d8                	c.sw	a4,12(a5)
2005fca2:	fec42783          	lw	a5,-20(s0)
2005fca6:	c781                	c.beqz	a5,2005fcae <unwind_frame_rv32c+0x36c>
2005fca8:	fe842783          	lw	a5,-24(s0)
2005fcac:	e399                	c.bnez	a5,2005fcb2 <unwind_frame_rv32c+0x370>
2005fcae:	57fd                	c.li	a5,-1
2005fcb0:	a011                	c.j	2005fcb4 <unwind_frame_rv32c+0x372>
2005fcb2:	4781                	c.li	a5,0
2005fcb4:	853e                	c.mv	a0,a5
2005fcb6:	40b6                	c.lwsp	ra,76(sp)
2005fcb8:	4426                	c.lwsp	s0,72(sp)
2005fcba:	6161                	c.addi16sp	sp,80
2005fcbc:	8082                	c.jr	ra

2005fcbe <do_stack_backtrace>:
2005fcbe:	7179                	c.addi16sp	sp,-48
2005fcc0:	ce52                	c.swsp	s4,28(sp)
2005fcc2:	d606                	c.swsp	ra,44(sp)
2005fcc4:	d426                	c.swsp	s1,40(sp)
2005fcc6:	d24a                	c.swsp	s2,36(sp)
2005fcc8:	d04e                	c.swsp	s3,32(sp)
2005fcca:	cc56                	c.swsp	s5,24(sp)
2005fccc:	c402                	c.swsp	zero,8(sp)
2005fcce:	c602                	c.swsp	zero,12(sp)
2005fcd0:	c02e                	c.swsp	a1,0(sp)
2005fcd2:	8a32                	c.mv	s4,a2
2005fcd4:	c525                	c.beqz	a0,2005fd3c <do_stack_backtrace+0x7e>
2005fcd6:	c1bd                	c.beqz	a1,2005fd3c <do_stack_backtrace+0x7e>
2005fcd8:	c22a                	c.swsp	a0,4(sp)
2005fcda:	4712                	c.lwsp	a4,4(sp)
2005fcdc:	4682                	c.lwsp	a3,0(sp)
2005fcde:	200634b7          	lui	s1,0x20063
2005fce2:	200639b7          	lui	s3,0x20063
2005fce6:	64048513          	addi	a0,s1,1600 # 20063640 <TaskStateString.0+0x22c>
2005fcea:	1cc00613          	addi	a2,zero,460
2005fcee:	7f498593          	addi	a1,s3,2036 # 200637f4 <__FUNCTION__.1>
2005fcf2:	64048493          	addi	s1,s1,1600
2005fcf6:	be8ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fcfa:	4901                	c.li	s2,0
2005fcfc:	04848493          	addi	s1,s1,72
2005fd00:	05490b63          	beq	s2,s4,2005fd56 <do_stack_backtrace+0x98>
2005fd04:	40000593          	addi	a1,zero,1024
2005fd08:	850a                	c.mv	a0,sp
2005fd0a:	963ff0ef          	jal	ra,2005f66c <unwind_frame_rv32i>
2005fd0e:	ed15                	c.bnez	a0,2005fd4a <do_stack_backtrace+0x8c>
2005fd10:	4792                	c.lwsp	a5,4(sp)
2005fd12:	4702                	c.lwsp	a4,0(sp)
2005fd14:	48a2                	c.lwsp	a7,8(sp)
2005fd16:	4832                	c.lwsp	a6,12(sp)
2005fd18:	86ca                	c.mv	a3,s2
2005fd1a:	1d700613          	addi	a2,zero,471
2005fd1e:	7f498593          	addi	a1,s3,2036
2005fd22:	8526                	c.mv	a0,s1
2005fd24:	bbaff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fd28:	47a2                	c.lwsp	a5,8(sp)
2005fd2a:	4732                	c.lwsp	a4,12(sp)
2005fd2c:	00190a93          	addi	s5,s2,1
2005fd30:	c23e                	c.swsp	a5,4(sp)
2005fd32:	4782                	c.lwsp	a5,0(sp)
2005fd34:	8956                	c.mv	s2,s5
2005fd36:	97ba                	c.add	a5,a4
2005fd38:	c03e                	c.swsp	a5,0(sp)
2005fd3a:	b7d9                	c.j	2005fd00 <do_stack_backtrace+0x42>
2005fd3c:	00010793          	addi	a5,sp,0
2005fd40:	00000717          	auipc	a4,0x0
2005fd44:	c23a                	c.swsp	a4,4(sp)
2005fd46:	c03e                	c.swsp	a5,0(sp)
2005fd48:	bf49                	c.j	2005fcda <do_stack_backtrace+0x1c>
2005fd4a:	40000593          	addi	a1,zero,1024
2005fd4e:	850a                	c.mv	a0,sp
2005fd50:	bf3ff0ef          	jal	ra,2005f942 <unwind_frame_rv32c>
2005fd54:	dd55                	c.beqz	a0,2005fd10 <do_stack_backtrace+0x52>
2005fd56:	50b2                	c.lwsp	ra,44(sp)
2005fd58:	54a2                	c.lwsp	s1,40(sp)
2005fd5a:	5982                	c.lwsp	s3,32(sp)
2005fd5c:	4a72                	c.lwsp	s4,28(sp)
2005fd5e:	4ae2                	c.lwsp	s5,24(sp)
2005fd60:	854a                	c.mv	a0,s2
2005fd62:	5912                	c.lwsp	s2,36(sp)
2005fd64:	6145                	c.addi16sp	sp,48
2005fd66:	8082                	c.jr	ra

2005fd68 <crash_dump>:
2005fd68:	711d                	c.addi16sp	sp,-96
2005fd6a:	c8ca                	c.swsp	s2,80(sp)
2005fd6c:	c6ce                	c.swsp	s3,76(sp)
2005fd6e:	c2d6                	c.swsp	s5,68(sp)
2005fd70:	ce86                	c.swsp	ra,92(sp)
2005fd72:	cca2                	c.swsp	s0,88(sp)
2005fd74:	caa6                	c.swsp	s1,84(sp)
2005fd76:	c4d2                	c.swsp	s4,72(sp)
2005fd78:	c0da                	c.swsp	s6,64(sp)
2005fd7a:	de5e                	c.swsp	s7,60(sp)
2005fd7c:	dc62                	c.swsp	s8,56(sp)
2005fd7e:	da66                	c.swsp	s9,52(sp)
2005fd80:	d86a                	c.swsp	s10,48(sp)
2005fd82:	d66e                	c.swsp	s11,44(sp)
2005fd84:	89aa                	c.mv	s3,a0
2005fd86:	892e                	c.mv	s2,a1
2005fd88:	8ab2                	c.mv	s5,a2
2005fd8a:	34002c73          	csrrs	s8,mscratch,zero
2005fd8e:	34102cf3          	csrrs	s9,mepc,zero
2005fd92:	34202a73          	csrrs	s4,mcause,zero
2005fd96:	34302bf3          	csrrs	s7,mtval,zero
2005fd9a:	20064437          	lui	s0,0x20064
2005fd9e:	80840513          	addi	a0,s0,-2040 # 20063808 <__FUNCTION__.1+0x14>
2005fda2:	200644b7          	lui	s1,0x20064
2005fda6:	11200613          	addi	a2,zero,274
2005fdaa:	fa448593          	addi	a1,s1,-92 # 20063fa4 <__FUNCTION__.0>
2005fdae:	0551                	c.addi	a0,20
2005fdb0:	b2eff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fdb4:	e9c1a783          	lw	a5,-356(gp) # 20066118 <crash_task_info>
2005fdb8:	80840413          	addi	s0,s0,-2040
2005fdbc:	c399                	c.beqz	a5,2005fdc2 <crash_dump+0x5a>
2005fdbe:	8556                	c.mv	a0,s5
2005fdc0:	9782                	c.jalr	a5
2005fdc2:	473d                	c.li	a4,15
2005fdc4:	87a2                	c.mv	a5,s0
2005fdc6:	01476a63          	bltu	a4,s4,2005fdda <crash_dump+0x72>
2005fdca:	20064737          	lui	a4,0x20064
2005fdce:	002a1793          	slli	a5,s4,0x2
2005fdd2:	0c870713          	addi	a4,a4,200 # 200640c8 <exception_code_string>
2005fdd6:	97ba                	c.add	a5,a4
2005fdd8:	439c                	c.lw	a5,0(a5)
2005fdda:	8752                	c.mv	a4,s4
2005fddc:	86ce                	c.mv	a3,s3
2005fdde:	11700613          	addi	a2,zero,279
2005fde2:	fa448593          	addi	a1,s1,-92
2005fde6:	05440513          	addi	a0,s0,84
2005fdea:	af4ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fdee:	47bd                	c.li	a5,15
2005fdf0:	1547f463          	bgeu	a5,s4,2005ff38 <crash_dump+0x1d0>
2005fdf4:	12b00613          	addi	a2,zero,299
2005fdf8:	fa448593          	addi	a1,s1,-92
2005fdfc:	0a440513          	addi	a0,s0,164
2005fe00:	adeff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fe04:	86e2                	c.mv	a3,s8
2005fe06:	12c00613          	addi	a2,zero,300
2005fe0a:	fa448593          	addi	a1,s1,-92
2005fe0e:	0e840513          	addi	a0,s0,232
2005fe12:	accff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fe16:	86e6                	c.mv	a3,s9
2005fe18:	12d00613          	addi	a2,zero,301
2005fe1c:	fa448593          	addi	a1,s1,-92
2005fe20:	11840513          	addi	a0,s0,280
2005fe24:	abaff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fe28:	86d2                	c.mv	a3,s4
2005fe2a:	12e00613          	addi	a2,zero,302
2005fe2e:	fa448593          	addi	a1,s1,-92
2005fe32:	14840513          	addi	a0,s0,328
2005fe36:	aa8ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fe3a:	86de                	c.mv	a3,s7
2005fe3c:	12f00613          	addi	a2,zero,303
2005fe40:	fa448593          	addi	a1,s1,-92
2005fe44:	17840513          	addi	a0,s0,376
2005fe48:	20064b37          	lui	s6,0x20064
2005fe4c:	a92ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fe50:	108b0b13          	addi	s6,s6,264 # 20064108 <register_string>
2005fe54:	4a01                	c.li	s4,0
2005fe56:	23440c13          	addi	s8,s0,564
2005fe5a:	02000b93          	addi	s7,zero,32
2005fe5e:	002a1793          	slli	a5,s4,0x2
2005fe62:	97d6                	c.add	a5,s5
2005fe64:	000b2703          	lw	a4,0(s6)
2005fe68:	439c                	c.lw	a5,0(a5)
2005fe6a:	86d2                	c.mv	a3,s4
2005fe6c:	13400613          	addi	a2,zero,308
2005fe70:	fa448593          	addi	a1,s1,-92
2005fe74:	8562                	c.mv	a0,s8
2005fe76:	0a05                	c.addi	s4,1
2005fe78:	a66ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fe7c:	0b11                	c.addi	s6,4
2005fe7e:	ff7a10e3          	bne	s4,s7,2005fe5e <crash_dump+0xf6>
2005fe82:	15a00613          	addi	a2,zero,346
2005fe86:	fa448593          	addi	a1,s1,-92
2005fe8a:	26440513          	addi	a0,s0,612
2005fe8e:	a50ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fe92:	4629                	c.li	a2,10
2005fe94:	85ca                	c.mv	a1,s2
2005fe96:	854e                	c.mv	a0,s3
2005fe98:	e27ff0ef          	jal	ra,2005fcbe <do_stack_backtrace>
2005fe9c:	e921                	c.bnez	a0,2005feec <crash_dump+0x184>
2005fe9e:	40000593          	addi	a1,zero,1024
2005fea2:	850a                	c.mv	a0,sp
2005fea4:	c24e                	c.swsp	s3,4(sp)
2005fea6:	ca4e                	c.swsp	s3,20(sp)
2005fea8:	c402                	c.swsp	zero,8(sp)
2005feaa:	c602                	c.swsp	zero,12(sp)
2005feac:	c04a                	c.swsp	s2,0(sp)
2005feae:	cc02                	c.swsp	zero,24(sp)
2005feb0:	ce02                	c.swsp	zero,28(sp)
2005feb2:	c84a                	c.swsp	s2,16(sp)
2005feb4:	fb8ff0ef          	jal	ra,2005f66c <unwind_frame_rv32i>
2005feb8:	40000593          	addi	a1,zero,1024
2005febc:	0808                	c.addi4spn	a0,sp,16
2005febe:	a85ff0ef          	jal	ra,2005f942 <unwind_frame_rv32c>
2005fec2:	46b2                	c.lwsp	a3,12(sp)
2005fec4:	4772                	c.lwsp	a4,28(sp)
2005fec6:	89b6                	c.mv	s3,a3
2005fec8:	e291                	c.bnez	a3,2005fecc <crash_dump+0x164>
2005feca:	89ba                	c.mv	s3,a4
2005fecc:	16c00613          	addi	a2,zero,364
2005fed0:	fa448593          	addi	a1,s1,-92
2005fed4:	2a440513          	addi	a0,s0,676
2005fed8:	a06ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fedc:	004aa503          	lw	a0,4(s5)
2005fee0:	ffc9f593          	andi	a1,s3,-4
2005fee4:	4629                	c.li	a2,10
2005fee6:	95ca                	c.add	a1,s2
2005fee8:	dd7ff0ef          	jal	ra,2005fcbe <do_stack_backtrace>
2005feec:	fa448593          	addi	a1,s1,-92
2005fef0:	17000613          	addi	a2,zero,368
2005fef4:	30c40513          	addi	a0,s0,780
2005fef8:	9e6ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005fefc:	fa448593          	addi	a1,s1,-92
2005ff00:	18300613          	addi	a2,zero,387
2005ff04:	35440513          	addi	a0,s0,852
2005ff08:	9d6ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005ff0c:	39c40513          	addi	a0,s0,924
2005ff10:	4466                	c.lwsp	s0,88(sp)
2005ff12:	40f6                	c.lwsp	ra,92(sp)
2005ff14:	4946                	c.lwsp	s2,80(sp)
2005ff16:	49b6                	c.lwsp	s3,76(sp)
2005ff18:	4a26                	c.lwsp	s4,72(sp)
2005ff1a:	4a96                	c.lwsp	s5,68(sp)
2005ff1c:	4b06                	c.lwsp	s6,64(sp)
2005ff1e:	5bf2                	c.lwsp	s7,60(sp)
2005ff20:	5c62                	c.lwsp	s8,56(sp)
2005ff22:	5cd2                	c.lwsp	s9,52(sp)
2005ff24:	5d42                	c.lwsp	s10,48(sp)
2005ff26:	5db2                	c.lwsp	s11,44(sp)
2005ff28:	fa448593          	addi	a1,s1,-92
2005ff2c:	44d6                	c.lwsp	s1,84(sp)
2005ff2e:	18400613          	addi	a2,zero,388
2005ff32:	6125                	c.addi16sp	sp,96
2005ff34:	9aaff06f          	jal	zero,2005f0de <DiagPrintf>
2005ff38:	20064737          	lui	a4,0x20064
2005ff3c:	002a1793          	slli	a5,s4,0x2
2005ff40:	08870713          	addi	a4,a4,136 # 20064088 <exception_cause_string_tbl>
2005ff44:	97ba                	c.add	a5,a4
2005ff46:	0007ab03          	lw	s6,0(a5)
2005ff4a:	ea0b05e3          	beq	s6,zero,2005fdf4 <crash_dump+0x8c>
2005ff4e:	11d00613          	addi	a2,zero,285
2005ff52:	fa448593          	addi	a1,s1,-92
2005ff56:	1a840513          	addi	a0,s0,424
2005ff5a:	984ff0ef          	jal	ra,2005f0de <DiagPrintf>
2005ff5e:	21040d13          	addi	s10,s0,528
2005ff62:	1e040d93          	addi	s11,s0,480
2005ff66:	000b2683          	lw	a3,0(s6)
2005ff6a:	e80685e3          	beq	a3,zero,2005fdf4 <crash_dump+0x8c>
2005ff6e:	004b2703          	lw	a4,4(s6)
2005ff72:	cb19                	c.beqz	a4,2005ff88 <crash_dump+0x220>
2005ff74:	87de                	c.mv	a5,s7
2005ff76:	12000613          	addi	a2,zero,288
2005ff7a:	fa448593          	addi	a1,s1,-92
2005ff7e:	856e                	c.mv	a0,s11
2005ff80:	95eff0ef          	jal	ra,2005f0de <DiagPrintf>
2005ff84:	0b21                	c.addi	s6,8
2005ff86:	b7c5                	c.j	2005ff66 <crash_dump+0x1fe>
2005ff88:	12300613          	addi	a2,zero,291
2005ff8c:	fa448593          	addi	a1,s1,-92
2005ff90:	856a                	c.mv	a0,s10
2005ff92:	94cff0ef          	jal	ra,2005f0de <DiagPrintf>
2005ff96:	b7fd                	c.j	2005ff84 <crash_dump+0x21c>

2005ff98 <_my_random>:
2005ff98:	1141                	c.addi	sp,-16
2005ff9a:	852e                	c.mv	a0,a1
2005ff9c:	85b2                	c.mv	a1,a2
2005ff9e:	c606                	c.swsp	ra,12(sp)
2005ffa0:	db6ff0ef          	jal	ra,2005f556 <TRNG_get_random_bytes>
2005ffa4:	40b2                	c.lwsp	ra,12(sp)
2005ffa6:	4501                	c.li	a0,0
2005ffa8:	0141                	c.addi	sp,16
2005ffaa:	8082                	c.jr	ra

2005ffac <_my_free>:
2005ffac:	f32fb06f          	jal	zero,2005b6de <rtos_mem_free>

2005ffb0 <_my_calloc>:
2005ffb0:	02b50533          	mul	a0,a0,a1
2005ffb4:	f04fb06f          	jal	zero,2005b6b8 <rtos_mem_zmalloc>

2005ffb8 <wifi_set_rom2flash>:
2005ffb8:	200687b7          	lui	a5,0x20068
2005ffbc:	0cc78793          	addi	a5,a5,204 # 200680cc <wifi_rom_func_map>
2005ffc0:	fef1a623          	sw	a5,-20(gp) # 20066268 <p_wifi_rom_func_map>
2005ffc4:	20060737          	lui	a4,0x20060
2005ffc8:	fb070713          	addi	a4,a4,-80 # 2005ffb0 <_my_calloc>
2005ffcc:	c398                	c.sw	a4,0(a5)
2005ffce:	20060737          	lui	a4,0x20060
2005ffd2:	fac70713          	addi	a4,a4,-84 # 2005ffac <_my_free>
2005ffd6:	c3d8                	c.sw	a4,4(a5)
2005ffd8:	20060737          	lui	a4,0x20060
2005ffdc:	f9870713          	addi	a4,a4,-104 # 2005ff98 <_my_random>
2005ffe0:	c798                	c.sw	a4,8(a5)
2005ffe2:	200687b7          	lui	a5,0x20068
2005ffe6:	0a078793          	addi	a5,a5,160 # 200680a0 <wifi_rom2flash>
2005ffea:	fef1a423          	sw	a5,-24(gp) # 20066264 <p_wifi_rom2flash>
2005ffee:	2005b737          	lui	a4,0x2005b
2005fff2:	7f470713          	addi	a4,a4,2036 # 2005b7f4 <rtos_mutex_give>
2005fff6:	c3d8                	c.sw	a4,4(a5)
2005fff8:	2005b737          	lui	a4,0x2005b
2005fffc:	7a870713          	addi	a4,a4,1960 # 2005b7a8 <rtos_mutex_take>
20060000:	c398                	c.sw	a4,0(a5)
20060002:	2005b737          	lui	a4,0x2005b
20060006:	78a70713          	addi	a4,a4,1930 # 2005b78a <rtos_mutex_delete_static>
2006000a:	cfd8                	c.sw	a4,28(a5)
2006000c:	2005b737          	lui	a4,0x2005b
20060010:	71070713          	addi	a4,a4,1808 # 2005b710 <rtos_mutex_create_static>
20060014:	d398                	c.sw	a4,32(a5)
20060016:	2005b737          	lui	a4,0x2005b
2006001a:	66870713          	addi	a4,a4,1640 # 2005b668 <rtos_critical_enter>
2006001e:	c798                	c.sw	a4,8(a5)
20060020:	2005b737          	lui	a4,0x2005b
20060024:	68c70713          	addi	a4,a4,1676 # 2005b68c <rtos_critical_exit>
20060028:	c7d8                	c.sw	a4,12(a5)
2006002a:	2005c737          	lui	a4,0x2005c
2006002e:	b8470713          	addi	a4,a4,-1148 # 2005bb84 <rtos_time_delay_ms>
20060032:	cf98                	c.sw	a4,24(a5)
20060034:	20061737          	lui	a4,0x20061
20060038:	e0a70713          	addi	a4,a4,-502 # 20060e0a <rtw_acquire_wakelock>
2006003c:	d3d8                	c.sw	a4,36(a5)
2006003e:	20061737          	lui	a4,0x20061
20060042:	e2470713          	addi	a4,a4,-476 # 20060e24 <rtw_release_wakelock>
20060046:	d798                	c.sw	a4,40(a5)
20060048:	8082                	c.jr	ra

2006004a <wifi_on>:
2006004a:	1101                	c.addi	sp,-32
2006004c:	ce06                	c.swsp	ra,28(sp)
2006004e:	cc22                	c.swsp	s0,24(sp)
20060050:	ca26                	c.swsp	s1,20(sp)
20060052:	842a                	c.mv	s0,a0
20060054:	c84a                	c.swsp	s2,16(sp)
20060056:	ba1f90ef          	jal	ra,20059bf6 <wifi_set_user_config>
2006005a:	3b3000ef          	jal	ra,20060c0c <inic_msg_q_init>
2006005e:	40418793          	addi	a5,gp,1028 # 20066680 <wifi_user_config>
20060062:	7c102773          	csrrs	a4,0x7c1,zero
20060066:	10077713          	andi	a4,a4,256
2006006a:	c715                	c.beqz	a4,20060096 <wifi_on+0x4c>
2006006c:	0ff0000f          	fence	iorw,iorw
20060070:	04f7f00b          	cache	dwb,(a5)
20060074:	02078713          	addi	a4,a5,32
20060078:	04e7700b          	cache	dwb,(a4)
2006007c:	0ff0000f          	fence	iorw,iorw
20060080:	0001                	c.addi	zero,0
20060082:	0001                	c.addi	zero,0
20060084:	0001                	c.addi	zero,0
20060086:	0001                	c.addi	zero,0
20060088:	0001                	c.addi	zero,0
2006008a:	0001                	c.addi	zero,0
2006008c:	0001                	c.addi	zero,0
2006008e:	0001                	c.addi	zero,0
20060090:	0001                	c.addi	zero,0
20060092:	0001                	c.addi	zero,0
20060094:	0001                	c.addi	zero,0
20060096:	4605                	c.li	a2,1
20060098:	006c                	c.addi4spn	a1,sp,12
2006009a:	451d                	c.li	a0,7
2006009c:	c63e                	c.swsp	a5,12(sp)
2006009e:	26e5                	c.jal	20060486 <inic_api_host_message_send>
200600a0:	7fa000ef          	jal	ra,2006089a <inic_host_init_skb>
200600a4:	4605                	c.li	a2,1
200600a6:	006c                	c.addi4spn	a1,sp,12
200600a8:	4525                	c.li	a0,9
200600aa:	c622                	c.swsp	s0,12(sp)
200600ac:	2ee9                	c.jal	20060486 <inic_api_host_message_send>
200600ae:	fd018793          	addi	a5,gp,-48 # 2006624c <wifi_boot.0>
200600b2:	4398                	c.lw	a4,0(a5)
200600b4:	84aa                	c.mv	s1,a0
200600b6:	ef01                	c.bnez	a4,200600ce <wifi_on+0x84>
200600b8:	4905                	c.li	s2,1
200600ba:	0127a023          	sw	s2,0(a5)
200600be:	533000ef          	jal	ra,20060df0 <init_timer_wrapper>
200600c2:	fbc1a783          	lw	a5,-68(gp) # 20066238 <p_wifi_do_fast_connect>
200600c6:	c781                	c.beqz	a5,200600ce <wifi_on+0x84>
200600c8:	01241363          	bne	s0,s2,200600ce <wifi_on+0x84>
200600cc:	9782                	c.jalr	a5
200600ce:	40f2                	c.lwsp	ra,28(sp)
200600d0:	4462                	c.lwsp	s0,24(sp)
200600d2:	4942                	c.lwsp	s2,16(sp)
200600d4:	8526                	c.mv	a0,s1
200600d6:	44d2                	c.lwsp	s1,20(sp)
200600d8:	6105                	c.addi16sp	sp,32
200600da:	8082                	c.jr	ra

200600dc <wifi_config_autoreconnect>:
200600dc:	557d                	c.li	a0,-1
200600de:	8082                	c.jr	ra

200600e0 <_inic_ipc_ip_addr_update_in_wowlan>:
200600e0:	4505                	c.li	a0,1
200600e2:	8082                	c.jr	ra

200600e4 <inic_api_host_task_h>:
200600e4:	7179                	c.addi16sp	sp,-48
200600e6:	d226                	c.swsp	s1,36(sp)
200600e8:	200644b7          	lui	s1,0x20064
200600ec:	d04a                	c.swsp	s2,32(sp)
200600ee:	ce4e                	c.swsp	s3,28(sp)
200600f0:	cc52                	c.swsp	s4,24(sp)
200600f2:	ca56                	c.swsp	s5,20(sp)
200600f4:	c85a                	c.swsp	s6,16(sp)
200600f6:	c65e                	c.swsp	s7,12(sp)
200600f8:	d606                	c.swsp	ra,44(sp)
200600fa:	d422                	c.swsp	s0,40(sp)
200600fc:	20064a37          	lui	s4,0x20064
20060100:	1bc48493          	addi	s1,s1,444 # 200641bc <register_string+0xb4>
20060104:	fdc1a503          	lw	a0,-36(gp) # 20066258 <g_host_inic_api_task_wake_sema>
20060108:	55fd                	c.li	a1,-1
2006010a:	fd8fb0ef          	jal	ra,2005b8e2 <rtos_sema_take>
2006010e:	459d                	c.li	a1,7
20060110:	4541                	c.li	a0,16
20060112:	fdff80ef          	jal	ra,200590f0 <ipc_get_message>
20060116:	4140                	c.lw	s0,4(a0)
20060118:	7c1027f3          	csrrs	a5,0x7c1,zero
2006011c:	1007f793          	andi	a5,a5,256
20060120:	c3b9                	c.beqz	a5,20060166 <inic_api_host_task_h+0x82>
20060122:	01f47713          	andi	a4,s0,31
20060126:	cb21                	c.beqz	a4,20060176 <inic_api_host_task_h+0x92>
20060128:	04070693          	addi	a3,a4,64
2006012c:	fe047793          	andi	a5,s0,-32
20060130:	01f7f713          	andi	a4,a5,31
20060134:	9736                	c.add	a4,a3
20060136:	0ff0000f          	fence	iorw,iorw
2006013a:	973e                	c.add	a4,a5
2006013c:	00f7f00b          	cache	dinv,(a5)
20060140:	02078793          	addi	a5,a5,32
20060144:	40f706b3          	sub	a3,a4,a5
20060148:	fed04ae3          	blt	zero,a3,2006013c <inic_api_host_task_h+0x58>
2006014c:	0ff0000f          	fence	iorw,iorw
20060150:	0001                	c.addi	zero,0
20060152:	0001                	c.addi	zero,0
20060154:	0001                	c.addi	zero,0
20060156:	0001                	c.addi	zero,0
20060158:	0001                	c.addi	zero,0
2006015a:	0001                	c.addi	zero,0
2006015c:	0001                	c.addi	zero,0
2006015e:	0001                	c.addi	zero,0
20060160:	0001                	c.addi	zero,0
20060162:	0001                	c.addi	zero,0
20060164:	0001                	c.addi	zero,0
20060166:	ec01                	c.bnez	s0,2006017e <inic_api_host_task_h+0x9a>
20060168:	20064537          	lui	a0,0x20064
2006016c:	18850513          	addi	a0,a0,392 # 20064188 <register_string+0x80>
20060170:	f8ffe0ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20060174:	bf41                	c.j	20060104 <inic_api_host_task_h+0x20>
20060176:	87a2                	c.mv	a5,s0
20060178:	04000693          	addi	a3,zero,64
2006017c:	bf55                	c.j	20060130 <inic_api_host_task_h+0x4c>
2006017e:	400c                	c.lw	a1,0(s0)
20060180:	4721                	c.li	a4,8
20060182:	fff58793          	addi	a5,a1,-1
20060186:	2ef76763          	bltu	a4,a5,20060474 <inic_api_host_task_h+0x390>
2006018a:	078a                	c.slli	a5,0x2
2006018c:	97a6                	c.add	a5,s1
2006018e:	439c                	c.lw	a5,0(a5)
20060190:	8782                	c.jr	a5
20060192:	fcc1a783          	lw	a5,-52(gp) # 20066248 <scan_user_callback_ptr>
20060196:	c3ad                	c.beqz	a5,200601f8 <inic_api_host_task_h+0x114>
20060198:	440c                	c.lw	a1,8(s0)
2006019a:	4048                	c.lw	a0,4(s0)
2006019c:	9782                	c.jalr	a5
2006019e:	a8a9                	c.j	200601f8 <inic_api_host_task_h+0x114>
200601a0:	4048                	c.lw	a0,4(s0)
200601a2:	440c                	c.lw	a1,8(s0)
200601a4:	7c1027f3          	csrrs	a5,0x7c1,zero
200601a8:	1007f793          	andi	a5,a5,256
200601ac:	c3b9                	c.beqz	a5,200601f2 <inic_api_host_task_h+0x10e>
200601ae:	01f57713          	andi	a4,a0,31
200601b2:	cf59                	c.beqz	a4,20060250 <inic_api_host_task_h+0x16c>
200601b4:	03570693          	addi	a3,a4,53
200601b8:	fe057793          	andi	a5,a0,-32
200601bc:	01f7f713          	andi	a4,a5,31
200601c0:	9736                	c.add	a4,a3
200601c2:	0ff0000f          	fence	iorw,iorw
200601c6:	973e                	c.add	a4,a5
200601c8:	00f7f00b          	cache	dinv,(a5)
200601cc:	02078793          	addi	a5,a5,32
200601d0:	40f706b3          	sub	a3,a4,a5
200601d4:	fed04ae3          	blt	zero,a3,200601c8 <inic_api_host_task_h+0xe4>
200601d8:	0ff0000f          	fence	iorw,iorw
200601dc:	0001                	c.addi	zero,0
200601de:	0001                	c.addi	zero,0
200601e0:	0001                	c.addi	zero,0
200601e2:	0001                	c.addi	zero,0
200601e4:	0001                	c.addi	zero,0
200601e6:	0001                	c.addi	zero,0
200601e8:	0001                	c.addi	zero,0
200601ea:	0001                	c.addi	zero,0
200601ec:	0001                	c.addi	zero,0
200601ee:	0001                	c.addi	zero,0
200601f0:	0001                	c.addi	zero,0
200601f2:	fc81a783          	lw	a5,-56(gp) # 20066244 <scan_each_report_user_callback_ptr>
200601f6:	f3dd                	c.bnez	a5,2006019c <inic_api_host_task_h+0xb8>
200601f8:	00042023          	sw	zero,0(s0)
200601fc:	7c1027f3          	csrrs	a5,0x7c1,zero
20060200:	1007f793          	andi	a5,a5,256
20060204:	f00780e3          	beq	a5,zero,20060104 <inic_api_host_task_h+0x20>
20060208:	01f47793          	andi	a5,s0,31
2006020c:	04000713          	addi	a4,zero,64
20060210:	c781                	c.beqz	a5,20060218 <inic_api_host_task_h+0x134>
20060212:	04078713          	addi	a4,a5,64
20060216:	9801                	c.andi	s0,-32
20060218:	01f47793          	andi	a5,s0,31
2006021c:	97ba                	c.add	a5,a4
2006021e:	0ff0000f          	fence	iorw,iorw
20060222:	97a2                	c.add	a5,s0
20060224:	0484700b          	cache	dwb,(s0)
20060228:	02040413          	addi	s0,s0,32
2006022c:	40878733          	sub	a4,a5,s0
20060230:	fee04ae3          	blt	zero,a4,20060224 <inic_api_host_task_h+0x140>
20060234:	0ff0000f          	fence	iorw,iorw
20060238:	0001                	c.addi	zero,0
2006023a:	0001                	c.addi	zero,0
2006023c:	0001                	c.addi	zero,0
2006023e:	0001                	c.addi	zero,0
20060240:	0001                	c.addi	zero,0
20060242:	0001                	c.addi	zero,0
20060244:	0001                	c.addi	zero,0
20060246:	0001                	c.addi	zero,0
20060248:	0001                	c.addi	zero,0
2006024a:	0001                	c.addi	zero,0
2006024c:	0001                	c.addi	zero,0
2006024e:	bd5d                	c.j	20060104 <inic_api_host_task_h+0x20>
20060250:	87aa                	c.mv	a5,a0
20060252:	03500693          	addi	a3,zero,53
20060256:	b79d                	c.j	200601bc <inic_api_host_task_h+0xd8>
20060258:	4418                	c.lw	a4,8(s0)
2006025a:	444c                	c.lw	a1,12(s0)
2006025c:	481c                	c.lw	a5,16(s0)
2006025e:	4854                	c.lw	a3,20(s0)
20060260:	7c102673          	csrrs	a2,0x7c1,zero
20060264:	10067613          	andi	a2,a2,256
20060268:	c60d                	c.beqz	a2,20060292 <inic_api_host_task_h+0x1ae>
2006026a:	00b77633          	and	a2,a4,a1
2006026e:	557d                	c.li	a0,-1
20060270:	04a61b63          	bne	a2,a0,200602c6 <inic_api_host_task_h+0x1e2>
20060274:	7c016773          	csrrsi	a4,0x7c0,2
20060278:	0ff0000f          	fence	iorw,iorw
2006027c:	0001                	c.addi	zero,0
2006027e:	0001                	c.addi	zero,0
20060280:	0001                	c.addi	zero,0
20060282:	0001                	c.addi	zero,0
20060284:	0001                	c.addi	zero,0
20060286:	0001                	c.addi	zero,0
20060288:	0001                	c.addi	zero,0
2006028a:	0001                	c.addi	zero,0
2006028c:	0001                	c.addi	zero,0
2006028e:	0001                	c.addi	zero,0
20060290:	0001                	c.addi	zero,0
20060292:	7c102773          	csrrs	a4,0x7c1,zero
20060296:	10077713          	andi	a4,a4,256
2006029a:	df39                	c.beqz	a4,200601f8 <inic_api_host_task_h+0x114>
2006029c:	00d7f733          	and	a4,a5,a3
200602a0:	567d                	c.li	a2,-1
200602a2:	04c71863          	bne	a4,a2,200602f2 <inic_api_host_task_h+0x20e>
200602a6:	7c0167f3          	csrrsi	a5,0x7c0,2
200602aa:	0ff0000f          	fence	iorw,iorw
200602ae:	0001                	c.addi	zero,0
200602b0:	0001                	c.addi	zero,0
200602b2:	0001                	c.addi	zero,0
200602b4:	0001                	c.addi	zero,0
200602b6:	0001                	c.addi	zero,0
200602b8:	0001                	c.addi	zero,0
200602ba:	0001                	c.addi	zero,0
200602bc:	0001                	c.addi	zero,0
200602be:	0001                	c.addi	zero,0
200602c0:	0001                	c.addi	zero,0
200602c2:	0001                	c.addi	zero,0
200602c4:	bf15                	c.j	200601f8 <inic_api_host_task_h+0x114>
200602c6:	01f77613          	andi	a2,a4,31
200602ca:	c219                	c.beqz	a2,200602d0 <inic_api_host_task_h+0x1ec>
200602cc:	9b01                	c.andi	a4,-32
200602ce:	95b2                	c.add	a1,a2
200602d0:	fcb051e3          	bge	zero,a1,20060292 <inic_api_host_task_h+0x1ae>
200602d4:	01f77613          	andi	a2,a4,31
200602d8:	962e                	c.add	a2,a1
200602da:	0ff0000f          	fence	iorw,iorw
200602de:	963a                	c.add	a2,a4
200602e0:	00e7700b          	cache	dinv,(a4)
200602e4:	02070713          	addi	a4,a4,32
200602e8:	40e605b3          	sub	a1,a2,a4
200602ec:	feb04ae3          	blt	zero,a1,200602e0 <inic_api_host_task_h+0x1fc>
200602f0:	b761                	c.j	20060278 <inic_api_host_task_h+0x194>
200602f2:	01f7f713          	andi	a4,a5,31
200602f6:	c319                	c.beqz	a4,200602fc <inic_api_host_task_h+0x218>
200602f8:	9b81                	c.andi	a5,-32
200602fa:	96ba                	c.add	a3,a4
200602fc:	eed05ee3          	bge	zero,a3,200601f8 <inic_api_host_task_h+0x114>
20060300:	01f7f713          	andi	a4,a5,31
20060304:	9736                	c.add	a4,a3
20060306:	0ff0000f          	fence	iorw,iorw
2006030a:	973e                	c.add	a4,a5
2006030c:	00f7f00b          	cache	dinv,(a5)
20060310:	02078793          	addi	a5,a5,32
20060314:	40f706b3          	sub	a3,a4,a5
20060318:	fed04ae3          	blt	zero,a3,2006030c <inic_api_host_task_h+0x228>
2006031c:	b779                	c.j	200602aa <inic_api_host_task_h+0x1c6>
2006031e:	fd41a783          	lw	a5,-44(gp) # 20066250 <p_ap_channel_switch_callback>
20060322:	ec078be3          	beq	a5,zero,200601f8 <inic_api_host_task_h+0x114>
20060326:	00840583          	lb	a1,8(s0)
2006032a:	00444503          	lbu	a0,4(s0)
2006032e:	9782                	c.jalr	a5
20060330:	b5e1                	c.j	200601f8 <inic_api_host_task_h+0x114>
20060332:	440c                	c.lw	a1,8(s0)
20060334:	4450                	c.lw	a2,12(s0)
20060336:	00444503          	lbu	a0,4(s0)
2006033a:	4814                	c.lw	a3,16(s0)
2006033c:	87ae                	c.mv	a5,a1
2006033e:	8832                	c.mv	a6,a2
20060340:	7c102773          	csrrs	a4,0x7c1,zero
20060344:	10077713          	andi	a4,a4,256
20060348:	c70d                	c.beqz	a4,20060372 <inic_api_host_task_h+0x28e>
2006034a:	00c5f733          	and	a4,a1,a2
2006034e:	58fd                	c.li	a7,-1
20060350:	03171463          	bne	a4,a7,20060378 <inic_api_host_task_h+0x294>
20060354:	7c0167f3          	csrrsi	a5,0x7c0,2
20060358:	0ff0000f          	fence	iorw,iorw
2006035c:	0001                	c.addi	zero,0
2006035e:	0001                	c.addi	zero,0
20060360:	0001                	c.addi	zero,0
20060362:	0001                	c.addi	zero,0
20060364:	0001                	c.addi	zero,0
20060366:	0001                	c.addi	zero,0
20060368:	0001                	c.addi	zero,0
2006036a:	0001                	c.addi	zero,0
2006036c:	0001                	c.addi	zero,0
2006036e:	0001                	c.addi	zero,0
20060370:	0001                	c.addi	zero,0
20060372:	ba6fe0ef          	jal	ra,2005e718 <wifi_indication>
20060376:	b549                	c.j	200601f8 <inic_api_host_task_h+0x114>
20060378:	01f5f713          	andi	a4,a1,31
2006037c:	c709                	c.beqz	a4,20060386 <inic_api_host_task_h+0x2a2>
2006037e:	fe05f793          	andi	a5,a1,-32
20060382:	00e60833          	add	a6,a2,a4
20060386:	ff0056e3          	bge	zero,a6,20060372 <inic_api_host_task_h+0x28e>
2006038a:	01f7f713          	andi	a4,a5,31
2006038e:	9742                	c.add	a4,a6
20060390:	0ff0000f          	fence	iorw,iorw
20060394:	973e                	c.add	a4,a5
20060396:	00f7f00b          	cache	dinv,(a5)
2006039a:	02078793          	addi	a5,a5,32
2006039e:	40f70833          	sub	a6,a4,a5
200603a2:	ff004ae3          	blt	zero,a6,20060396 <inic_api_host_task_h+0x2b2>
200603a6:	bf4d                	c.j	20060358 <inic_api_host_task_h+0x274>
200603a8:	fc41a783          	lw	a5,-60(gp) # 20066240 <promisc_user_callback_ptr>
200603ac:	4501                	c.li	a0,0
200603ae:	cbc1                	c.beqz	a5,2006043e <inic_api_host_task_h+0x35a>
200603b0:	4058                	c.lw	a4,4(s0)
200603b2:	7c1027f3          	csrrs	a5,0x7c1,zero
200603b6:	1007f793          	andi	a5,a5,256
200603ba:	c3b9                	c.beqz	a5,20060400 <inic_api_host_task_h+0x31c>
200603bc:	01f77693          	andi	a3,a4,31
200603c0:	c2c9                	c.beqz	a3,20060442 <inic_api_host_task_h+0x35e>
200603c2:	01068613          	addi	a2,a3,16
200603c6:	fe077793          	andi	a5,a4,-32
200603ca:	01f7f693          	andi	a3,a5,31
200603ce:	96b2                	c.add	a3,a2
200603d0:	0ff0000f          	fence	iorw,iorw
200603d4:	96be                	c.add	a3,a5
200603d6:	00f7f00b          	cache	dinv,(a5)
200603da:	02078793          	addi	a5,a5,32
200603de:	40f68633          	sub	a2,a3,a5
200603e2:	fec04ae3          	blt	zero,a2,200603d6 <inic_api_host_task_h+0x2f2>
200603e6:	0ff0000f          	fence	iorw,iorw
200603ea:	0001                	c.addi	zero,0
200603ec:	0001                	c.addi	zero,0
200603ee:	0001                	c.addi	zero,0
200603f0:	0001                	c.addi	zero,0
200603f2:	0001                	c.addi	zero,0
200603f4:	0001                	c.addi	zero,0
200603f6:	0001                	c.addi	zero,0
200603f8:	0001                	c.addi	zero,0
200603fa:	0001                	c.addi	zero,0
200603fc:	0001                	c.addi	zero,0
200603fe:	0001                	c.addi	zero,0
20060400:	471c                	c.lw	a5,8(a4)
20060402:	4754                	c.lw	a3,12(a4)
20060404:	7c102773          	csrrs	a4,0x7c1,zero
20060408:	10077713          	andi	a4,a4,256
2006040c:	c70d                	c.beqz	a4,20060436 <inic_api_host_task_h+0x352>
2006040e:	577d                	c.li	a4,-1
20060410:	02e79c63          	bne	a5,a4,20060448 <inic_api_host_task_h+0x364>
20060414:	02f69a63          	bne	a3,a5,20060448 <inic_api_host_task_h+0x364>
20060418:	7c0167f3          	csrrsi	a5,0x7c0,2
2006041c:	0ff0000f          	fence	iorw,iorw
20060420:	0001                	c.addi	zero,0
20060422:	0001                	c.addi	zero,0
20060424:	0001                	c.addi	zero,0
20060426:	0001                	c.addi	zero,0
20060428:	0001                	c.addi	zero,0
2006042a:	0001                	c.addi	zero,0
2006042c:	0001                	c.addi	zero,0
2006042e:	0001                	c.addi	zero,0
20060430:	0001                	c.addi	zero,0
20060432:	0001                	c.addi	zero,0
20060434:	0001                	c.addi	zero,0
20060436:	fc41a783          	lw	a5,-60(gp) # 20066240 <promisc_user_callback_ptr>
2006043a:	4048                	c.lw	a0,4(s0)
2006043c:	9782                	c.jalr	a5
2006043e:	d008                	c.sw	a0,32(s0)
20060440:	bb65                	c.j	200601f8 <inic_api_host_task_h+0x114>
20060442:	87ba                	c.mv	a5,a4
20060444:	4641                	c.li	a2,16
20060446:	b751                	c.j	200603ca <inic_api_host_task_h+0x2e6>
20060448:	01f7f713          	andi	a4,a5,31
2006044c:	c319                	c.beqz	a4,20060452 <inic_api_host_task_h+0x36e>
2006044e:	9b81                	c.andi	a5,-32
20060450:	96ba                	c.add	a3,a4
20060452:	fed052e3          	bge	zero,a3,20060436 <inic_api_host_task_h+0x352>
20060456:	01f7f713          	andi	a4,a5,31
2006045a:	9736                	c.add	a4,a3
2006045c:	0ff0000f          	fence	iorw,iorw
20060460:	973e                	c.add	a4,a5
20060462:	00f7f00b          	cache	dinv,(a5)
20060466:	02078793          	addi	a5,a5,32
2006046a:	40f706b3          	sub	a3,a4,a5
2006046e:	fed04ae3          	blt	zero,a3,20060462 <inic_api_host_task_h+0x37e>
20060472:	b76d                	c.j	2006041c <inic_api_host_task_h+0x338>
20060474:	1a0a0513          	addi	a0,s4,416 # 200641a0 <register_string+0x98>
20060478:	c87fe0ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2006047c:	bbb5                	c.j	200601f8 <inic_api_host_task_h+0x114>

2006047e <inic_api_host_int_hdl>:
2006047e:	fdc1a503          	lw	a0,-36(gp) # 20066258 <g_host_inic_api_task_wake_sema>
20060482:	cacfb06f          	jal	zero,2005b92e <rtos_sema_give>

20060486 <inic_api_host_message_send>:
20060486:	7179                	c.addi16sp	sp,-48
20060488:	cc52                	c.swsp	s4,24(sp)
2006048a:	c85a                	c.swsp	s6,16(sp)
2006048c:	8b2a                	c.mv	s6,a0
2006048e:	fd81a503          	lw	a0,-40(gp) # 20066254 <g_host_inic_api_message_send_sema>
20060492:	c65e                	c.swsp	s7,12(sp)
20060494:	20068bb7          	lui	s7,0x20068
20060498:	d226                	c.swsp	s1,36(sp)
2006049a:	ce4e                	c.swsp	s3,28(sp)
2006049c:	c462                	c.swsp	s8,8(sp)
2006049e:	89ae                	c.mv	s3,a1
200604a0:	fc0b8493          	addi	s1,s7,-64 # 20067fc0 <g_host_ipc_api_request_info>
200604a4:	55fd                	c.li	a1,-1
200604a6:	6c0d                	c.lui	s8,0x3
200604a8:	ca56                	c.swsp	s5,20(sp)
200604aa:	c266                	c.swsp	s9,4(sp)
200604ac:	c06a                	c.swsp	s10,0(sp)
200604ae:	d606                	c.swsp	ra,44(sp)
200604b0:	d422                	c.swsp	s0,40(sp)
200604b2:	d04a                	c.swsp	s2,32(sp)
200604b4:	8ab2                	c.mv	s5,a2
200604b6:	fd818a13          	addi	s4,gp,-40 # 20066254 <g_host_inic_api_message_send_sema>
200604ba:	c28fb0ef          	jal	ra,2005b8e2 <rtos_sema_take>
200604be:	02048d13          	addi	s10,s1,32
200604c2:	ee0c0c13          	addi	s8,s8,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
200604c6:	20064cb7          	lui	s9,0x20064
200604ca:	4401                	c.li	s0,0
200604cc:	7c1027f3          	csrrs	a5,0x7c1,zero
200604d0:	1007f793          	andi	a5,a5,256
200604d4:	fc0b8913          	addi	s2,s7,-64
200604d8:	c785                	c.beqz	a5,20060500 <inic_api_host_message_send+0x7a>
200604da:	0ff0000f          	fence	iorw,iorw
200604de:	0094f00b          	cache	dinv,(s1)
200604e2:	01ad700b          	cache	dinv,(s10)
200604e6:	0ff0000f          	fence	iorw,iorw
200604ea:	0001                	c.addi	zero,0
200604ec:	0001                	c.addi	zero,0
200604ee:	0001                	c.addi	zero,0
200604f0:	0001                	c.addi	zero,0
200604f2:	0001                	c.addi	zero,0
200604f4:	0001                	c.addi	zero,0
200604f6:	0001                	c.addi	zero,0
200604f8:	0001                	c.addi	zero,0
200604fa:	0001                	c.addi	zero,0
200604fc:	0001                	c.addi	zero,0
200604fe:	0001                	c.addi	zero,0
20060500:	00092783          	lw	a5,0(s2)
20060504:	cf89                	c.beqz	a5,2006051e <inic_api_host_message_send+0x98>
20060506:	4505                	c.li	a0,1
20060508:	0405                	c.addi	s0,1
2006050a:	e7afb0ef          	jal	ra,2005bb84 <rtos_time_delay_ms>
2006050e:	03846433          	rem	s0,s0,s8
20060512:	fc4d                	c.bnez	s0,200604cc <inic_api_host_message_send+0x46>
20060514:	1e0c8513          	addi	a0,s9,480 # 200641e0 <register_string+0xd8>
20060518:	be7fe0ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
2006051c:	b77d                	c.j	200604ca <inic_api_host_message_send+0x44>
2006051e:	04000613          	addi	a2,zero,64
20060522:	4581                	c.li	a1,0
20060524:	fc0b8513          	addi	a0,s7,-64
20060528:	dffa9097          	auipc	ra,0xdffa9
2006052c:	5a6080e7          	jalr	ra,1446(ra) # 9ace <__wrap_memset>
20060530:	01692023          	sw	s6,0(s2)
20060534:	00098b63          	beq	s3,zero,2006054a <inic_api_host_message_send+0xc4>
20060538:	002a9613          	slli	a2,s5,0x2
2006053c:	85ce                	c.mv	a1,s3
2006053e:	00490513          	addi	a0,s2,4
20060542:	dffa9097          	auipc	ra,0xdffa9
20060546:	58e080e7          	jalr	ra,1422(ra) # 9ad0 <__wrap_memcpy>
2006054a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006054e:	1007f793          	andi	a5,a5,256
20060552:	c795                	c.beqz	a5,2006057e <inic_api_host_message_send+0xf8>
20060554:	0ff0000f          	fence	iorw,iorw
20060558:	0494f00b          	cache	dwb,(s1)
2006055c:	02048793          	addi	a5,s1,32
20060560:	04f7f00b          	cache	dwb,(a5)
20060564:	0ff0000f          	fence	iorw,iorw
20060568:	0001                	c.addi	zero,0
2006056a:	0001                	c.addi	zero,0
2006056c:	0001                	c.addi	zero,0
2006056e:	0001                	c.addi	zero,0
20060570:	0001                	c.addi	zero,0
20060572:	0001                	c.addi	zero,0
20060574:	0001                	c.addi	zero,0
20060576:	0001                	c.addi	zero,0
20060578:	0001                	c.addi	zero,0
2006057a:	0001                	c.addi	zero,0
2006057c:	0001                	c.addi	zero,0
2006057e:	200689b7          	lui	s3,0x20068
20060582:	4641                	c.li	a2,16
20060584:	4581                	c.li	a1,0
20060586:	f8098513          	addi	a0,s3,-128 # 20067f80 <g_host_ipc_api_msg>
2006058a:	dffa9097          	auipc	ra,0xdffa9
2006058e:	544080e7          	jalr	ra,1348(ra) # 9ace <__wrap_memset>
20060592:	f8098a93          	addi	s5,s3,-128
20060596:	04000793          	addi	a5,zero,64
2006059a:	009aa223          	sw	s1,4(s5)
2006059e:	000aa023          	sw	zero,0(s5)
200605a2:	00faa423          	sw	a5,8(s5)
200605a6:	7c1027f3          	csrrs	a5,0x7c1,zero
200605aa:	1007f793          	andi	a5,a5,256
200605ae:	c395                	c.beqz	a5,200605d2 <inic_api_host_message_send+0x14c>
200605b0:	0ff0000f          	fence	iorw,iorw
200605b4:	055af00b          	cache	dwb,(s5)
200605b8:	0ff0000f          	fence	iorw,iorw
200605bc:	0001                	c.addi	zero,0
200605be:	0001                	c.addi	zero,0
200605c0:	0001                	c.addi	zero,0
200605c2:	0001                	c.addi	zero,0
200605c4:	0001                	c.addi	zero,0
200605c6:	0001                	c.addi	zero,0
200605c8:	0001                	c.addi	zero,0
200605ca:	0001                	c.addi	zero,0
200605cc:	0001                	c.addi	zero,0
200605ce:	0001                	c.addi	zero,0
200605d0:	0001                	c.addi	zero,0
200605d2:	f8098613          	addi	a2,s3,-128
200605d6:	459d                	c.li	a1,7
200605d8:	4501                	c.li	a0,0
200605da:	698d                	c.lui	s3,0x3
200605dc:	9cdf80ef          	jal	ra,20058fa8 <ipc_send_message>
200605e0:	02048b93          	addi	s7,s1,32
200605e4:	ee098993          	addi	s3,s3,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
200605e8:	20064b37          	lui	s6,0x20064
200605ec:	20064ab7          	lui	s5,0x20064
200605f0:	00092783          	lw	a5,0(s2)
200605f4:	c7b9                	c.beqz	a5,20060642 <inic_api_host_message_send+0x1bc>
200605f6:	4505                	c.li	a0,1
200605f8:	d8cfb0ef          	jal	ra,2005bb84 <rtos_time_delay_ms>
200605fc:	7c1027f3          	csrrs	a5,0x7c1,zero
20060600:	1007f793          	andi	a5,a5,256
20060604:	c785                	c.beqz	a5,2006062c <inic_api_host_message_send+0x1a6>
20060606:	0ff0000f          	fence	iorw,iorw
2006060a:	0094f00b          	cache	dinv,(s1)
2006060e:	017bf00b          	cache	dinv,(s7)
20060612:	0ff0000f          	fence	iorw,iorw
20060616:	0001                	c.addi	zero,0
20060618:	0001                	c.addi	zero,0
2006061a:	0001                	c.addi	zero,0
2006061c:	0001                	c.addi	zero,0
2006061e:	0001                	c.addi	zero,0
20060620:	0001                	c.addi	zero,0
20060622:	0001                	c.addi	zero,0
20060624:	0001                	c.addi	zero,0
20060626:	0001                	c.addi	zero,0
20060628:	0001                	c.addi	zero,0
2006062a:	0001                	c.addi	zero,0
2006062c:	0405                	c.addi	s0,1
2006062e:	03346433          	rem	s0,s0,s3
20060632:	fc5d                	c.bnez	s0,200605f0 <inic_api_host_message_send+0x16a>
20060634:	280b0593          	addi	a1,s6,640 # 20064280 <__func__.0>
20060638:	1f8a8513          	addi	a0,s5,504 # 200641f8 <register_string+0xf0>
2006063c:	ac3fe0ef          	jal	ra,2005f0fe <DiagPrintf_minimal>
20060640:	bf45                	c.j	200605f0 <inic_api_host_message_send+0x16a>
20060642:	000a2503          	lw	a0,0(s4)
20060646:	02892403          	lw	s0,40(s2)
2006064a:	ae4fb0ef          	jal	ra,2005b92e <rtos_sema_give>
2006064e:	50b2                	c.lwsp	ra,44(sp)
20060650:	8522                	c.mv	a0,s0
20060652:	5422                	c.lwsp	s0,40(sp)
20060654:	5492                	c.lwsp	s1,36(sp)
20060656:	5902                	c.lwsp	s2,32(sp)
20060658:	49f2                	c.lwsp	s3,28(sp)
2006065a:	4a62                	c.lwsp	s4,24(sp)
2006065c:	4ad2                	c.lwsp	s5,20(sp)
2006065e:	4b42                	c.lwsp	s6,16(sp)
20060660:	4bb2                	c.lwsp	s7,12(sp)
20060662:	4c22                	c.lwsp	s8,8(sp)
20060664:	4c92                	c.lwsp	s9,4(sp)
20060666:	4d02                	c.lwsp	s10,0(sp)
20060668:	6145                	c.addi16sp	sp,48
2006066a:	8082                	c.jr	ra

2006066c <inic_api_init_host>:
2006066c:	1141                	c.addi	sp,-16
2006066e:	567d                	c.li	a2,-1
20060670:	4581                	c.li	a1,0
20060672:	fdc18513          	addi	a0,gp,-36 # 20066258 <g_host_inic_api_task_wake_sema>
20060676:	c606                	c.swsp	ra,12(sp)
20060678:	c422                	c.swsp	s0,8(sp)
2006067a:	9eafb0ef          	jal	ra,2005b864 <rtos_sema_create_static>
2006067e:	567d                	c.li	a2,-1
20060680:	4581                	c.li	a1,0
20060682:	fd818413          	addi	s0,gp,-40 # 20066254 <g_host_inic_api_message_send_sema>
20060686:	fd818513          	addi	a0,gp,-40 # 20066254 <g_host_inic_api_message_send_sema>
2006068a:	9dafb0ef          	jal	ra,2005b864 <rtos_sema_create_static>
2006068e:	4008                	c.lw	a0,0(s0)
20060690:	a9efb0ef          	jal	ra,2005b92e <rtos_sema_give>
20060694:	200605b7          	lui	a1,0x20060
20060698:	4701                	c.li	a4,0
2006069a:	4681                	c.li	a3,0
2006069c:	4601                	c.li	a2,0
2006069e:	0e058593          	addi	a1,a1,224 # 200600e0 <_inic_ipc_ip_addr_update_in_wowlan>
200606a2:	4505                	c.li	a0,1
200606a4:	876f90ef          	jal	ra,2005971a <pmu_register_sleep_callback>
200606a8:	6705                	c.lui	a4,0x1
200606aa:	20060637          	lui	a2,0x20060
200606ae:	200645b7          	lui	a1,0x20064
200606b2:	478d                	c.li	a5,3
200606b4:	ce070713          	addi	a4,a4,-800 # ce0 <CPU_ClkGet+0x1ca>
200606b8:	4681                	c.li	a3,0
200606ba:	0e460613          	addi	a2,a2,228 # 200600e4 <inic_api_host_task_h>
200606be:	21858593          	addi	a1,a1,536 # 20064218 <register_string+0x110>
200606c2:	4501                	c.li	a0,0
200606c4:	c8afb0ef          	jal	ra,2005bb4e <rtos_task_create>
200606c8:	c10d                	c.beqz	a0,200606ea <inic_api_init_host+0x7e>
200606ca:	4422                	c.lwsp	s0,8(sp)
200606cc:	40b2                	c.lwsp	ra,12(sp)
200606ce:	200646b7          	lui	a3,0x20064
200606d2:	200645b7          	lui	a1,0x20064
200606d6:	22c68693          	addi	a3,a3,556 # 2006422c <register_string+0x124>
200606da:	04500613          	addi	a2,zero,69
200606de:	24858593          	addi	a1,a1,584 # 20064248 <register_string+0x140>
200606e2:	4509                	c.li	a0,2
200606e4:	0141                	c.addi	sp,16
200606e6:	fd2fa06f          	jal	zero,2005aeb8 <rtk_log_write>
200606ea:	40b2                	c.lwsp	ra,12(sp)
200606ec:	4422                	c.lwsp	s0,8(sp)
200606ee:	0141                	c.addi	sp,16
200606f0:	8082                	c.jr	ra

200606f2 <inic_mp_command>:
200606f2:	7179                	c.addi16sp	sp,-48
200606f4:	ce4e                	c.swsp	s3,28(sp)
200606f6:	89aa                	c.mv	s3,a0
200606f8:	6505                	c.lui	a0,0x1
200606fa:	d04a                	c.swsp	s2,32(sp)
200606fc:	cc52                	c.swsp	s4,24(sp)
200606fe:	d606                	c.swsp	ra,44(sp)
20060700:	d422                	c.swsp	s0,40(sp)
20060702:	d226                	c.swsp	s1,36(sp)
20060704:	892e                	c.mv	s2,a1
20060706:	8a32                	c.mv	s4,a2
20060708:	fb1fa0ef          	jal	ra,2005b6b8 <rtos_mem_zmalloc>
2006070c:	e50d                	c.bnez	a0,20060736 <inic_mp_command+0x44>
2006070e:	5422                	c.lwsp	s0,40(sp)
20060710:	50b2                	c.lwsp	ra,44(sp)
20060712:	5492                	c.lwsp	s1,36(sp)
20060714:	5902                	c.lwsp	s2,32(sp)
20060716:	49f2                	c.lwsp	s3,28(sp)
20060718:	4a62                	c.lwsp	s4,24(sp)
2006071a:	200646b7          	lui	a3,0x20064
2006071e:	200645b7          	lui	a1,0x20064
20060722:	25068693          	addi	a3,a3,592 # 20064250 <register_string+0x148>
20060726:	04500613          	addi	a2,zero,69
2006072a:	24858593          	addi	a1,a1,584 # 20064248 <register_string+0x140>
2006072e:	4509                	c.li	a0,2
20060730:	6145                	c.addi16sp	sp,48
20060732:	f86fa06f          	jal	zero,2005aeb8 <rtk_log_write>
20060736:	842a                	c.mv	s0,a0
20060738:	87ce                	c.mv	a5,s3
2006073a:	c04e                	c.swsp	s3,0(sp)
2006073c:	c24a                	c.swsp	s2,4(sp)
2006073e:	c452                	c.swsp	s4,8(sp)
20060740:	84aa                	c.mv	s1,a0
20060742:	c62a                	c.swsp	a0,12(sp)
20060744:	7c102773          	csrrs	a4,0x7c1,zero
20060748:	10077713          	andi	a4,a4,256
2006074c:	c721                	c.beqz	a4,20060794 <inic_mp_command+0xa2>
2006074e:	01f57693          	andi	a3,a0,31
20060752:	10068363          	beq	a3,zero,20060858 <inic_mp_command+0x166>
20060756:	6605                	c.lui	a2,0x1
20060758:	9636                	c.add	a2,a3
2006075a:	fe057713          	andi	a4,a0,-32
2006075e:	01f77693          	andi	a3,a4,31
20060762:	96b2                	c.add	a3,a2
20060764:	0ff0000f          	fence	iorw,iorw
20060768:	96ba                	c.add	a3,a4
2006076a:	04e7700b          	cache	dwb,(a4)
2006076e:	02070713          	addi	a4,a4,32
20060772:	40e68633          	sub	a2,a3,a4
20060776:	fec04ae3          	blt	zero,a2,2006076a <inic_mp_command+0x78>
2006077a:	0ff0000f          	fence	iorw,iorw
2006077e:	0001                	c.addi	zero,0
20060780:	0001                	c.addi	zero,0
20060782:	0001                	c.addi	zero,0
20060784:	0001                	c.addi	zero,0
20060786:	0001                	c.addi	zero,0
20060788:	0001                	c.addi	zero,0
2006078a:	0001                	c.addi	zero,0
2006078c:	0001                	c.addi	zero,0
2006078e:	0001                	c.addi	zero,0
20060790:	0001                	c.addi	zero,0
20060792:	0001                	c.addi	zero,0
20060794:	7c102773          	csrrs	a4,0x7c1,zero
20060798:	10077713          	andi	a4,a4,256
2006079c:	c70d                	c.beqz	a4,200607c6 <inic_mp_command+0xd4>
2006079e:	577d                	c.li	a4,-1
200607a0:	0ae99f63          	bne	s3,a4,2006085e <inic_mp_command+0x16c>
200607a4:	0b391d63          	bne	s2,s3,2006085e <inic_mp_command+0x16c>
200607a8:	7c0267f3          	csrrsi	a5,0x7c0,4
200607ac:	0ff0000f          	fence	iorw,iorw
200607b0:	0001                	c.addi	zero,0
200607b2:	0001                	c.addi	zero,0
200607b4:	0001                	c.addi	zero,0
200607b6:	0001                	c.addi	zero,0
200607b8:	0001                	c.addi	zero,0
200607ba:	0001                	c.addi	zero,0
200607bc:	0001                	c.addi	zero,0
200607be:	0001                	c.addi	zero,0
200607c0:	0001                	c.addi	zero,0
200607c2:	0001                	c.addi	zero,0
200607c4:	0001                	c.addi	zero,0
200607c6:	4611                	c.li	a2,4
200607c8:	858a                	c.mv	a1,sp
200607ca:	04f00513          	addi	a0,zero,79
200607ce:	cb9ff0ef          	jal	ra,20060486 <inic_api_host_message_send>
200607d2:	060a0863          	beq	s4,zero,20060842 <inic_mp_command+0x150>
200607d6:	7c1027f3          	csrrs	a5,0x7c1,zero
200607da:	1007f793          	andi	a5,a5,256
200607de:	c7a1                	c.beqz	a5,20060826 <inic_mp_command+0x134>
200607e0:	01f47713          	andi	a4,s0,31
200607e4:	6785                	c.lui	a5,0x1
200607e6:	c701                	c.beqz	a4,200607ee <inic_mp_command+0xfc>
200607e8:	97ba                	c.add	a5,a4
200607ea:	fe047493          	andi	s1,s0,-32
200607ee:	01f4f713          	andi	a4,s1,31
200607f2:	973e                	c.add	a4,a5
200607f4:	0ff0000f          	fence	iorw,iorw
200607f8:	87a6                	c.mv	a5,s1
200607fa:	9726                	c.add	a4,s1
200607fc:	00f7f00b          	cache	dinv,(a5)
20060800:	02078793          	addi	a5,a5,32 # 1020 <CPU_ClkGet+0x50a>
20060804:	40f706b3          	sub	a3,a4,a5
20060808:	fed04ae3          	blt	zero,a3,200607fc <inic_mp_command+0x10a>
2006080c:	0ff0000f          	fence	iorw,iorw
20060810:	0001                	c.addi	zero,0
20060812:	0001                	c.addi	zero,0
20060814:	0001                	c.addi	zero,0
20060816:	0001                	c.addi	zero,0
20060818:	0001                	c.addi	zero,0
2006081a:	0001                	c.addi	zero,0
2006081c:	0001                	c.addi	zero,0
2006081e:	0001                	c.addi	zero,0
20060820:	0001                	c.addi	zero,0
20060822:	0001                	c.addi	zero,0
20060824:	0001                	c.addi	zero,0
20060826:	200646b7          	lui	a3,0x20064
2006082a:	200645b7          	lui	a1,0x20064
2006082e:	8722                	c.mv	a4,s0
20060830:	26868693          	addi	a3,a3,616 # 20064268 <register_string+0x160>
20060834:	04100613          	addi	a2,zero,65
20060838:	24858593          	addi	a1,a1,584 # 20064248 <register_string+0x140>
2006083c:	4505                	c.li	a0,1
2006083e:	e7afa0ef          	jal	ra,2005aeb8 <rtk_log_write>
20060842:	8522                	c.mv	a0,s0
20060844:	e9bfa0ef          	jal	ra,2005b6de <rtos_mem_free>
20060848:	50b2                	c.lwsp	ra,44(sp)
2006084a:	5422                	c.lwsp	s0,40(sp)
2006084c:	5492                	c.lwsp	s1,36(sp)
2006084e:	5902                	c.lwsp	s2,32(sp)
20060850:	49f2                	c.lwsp	s3,28(sp)
20060852:	4a62                	c.lwsp	s4,24(sp)
20060854:	6145                	c.addi16sp	sp,48
20060856:	8082                	c.jr	ra
20060858:	872a                	c.mv	a4,a0
2006085a:	6605                	c.lui	a2,0x1
2006085c:	b709                	c.j	2006075e <inic_mp_command+0x6c>
2006085e:	01f9f713          	andi	a4,s3,31
20060862:	c701                	c.beqz	a4,2006086a <inic_mp_command+0x178>
20060864:	fe09f793          	andi	a5,s3,-32
20060868:	993a                	c.add	s2,a4
2006086a:	f5205ee3          	bge	zero,s2,200607c6 <inic_mp_command+0xd4>
2006086e:	01f7f593          	andi	a1,a5,31
20060872:	992e                	c.add	s2,a1
20060874:	0ff0000f          	fence	iorw,iorw
20060878:	993e                	c.add	s2,a5
2006087a:	04f7f00b          	cache	dwb,(a5)
2006087e:	02078793          	addi	a5,a5,32
20060882:	40f90733          	sub	a4,s2,a5
20060886:	fee04ae3          	blt	zero,a4,2006087a <inic_mp_command+0x188>
2006088a:	b70d                	c.j	200607ac <inic_mp_command+0xba>

2006088c <inic_host_init>:
2006088c:	1141                	c.addi	sp,-16
2006088e:	c606                	c.swsp	ra,12(sp)
20060890:	2269                	c.jal	20060a1a <inic_host_init_priv>
20060892:	40b2                	c.lwsp	ra,12(sp)
20060894:	0141                	c.addi	sp,16
20060896:	dd7ff06f          	jal	zero,2006066c <inic_api_init_host>

2006089a <inic_host_init_skb>:
2006089a:	1101                	c.addi	sp,-32
2006089c:	ca26                	c.swsp	s1,20(sp)
2006089e:	fe41a783          	lw	a5,-28(gp) # 20066260 <host_skb_info>
200608a2:	ce06                	c.swsp	ra,28(sp)
200608a4:	cc22                	c.swsp	s0,24(sp)
200608a6:	c84a                	c.swsp	s2,16(sp)
200608a8:	c64e                	c.swsp	s3,12(sp)
200608aa:	e791                	c.bnez	a5,200608b6 <inic_host_init_skb+0x1c>
200608ac:	fe01a783          	lw	a5,-32(gp) # 2006625c <host_skb_data>
200608b0:	fe018413          	addi	s0,gp,-32 # 2006625c <host_skb_data>
200608b4:	c785                	c.beqz	a5,200608dc <inic_host_init_skb+0x42>
200608b6:	4462                	c.lwsp	s0,24(sp)
200608b8:	40f2                	c.lwsp	ra,28(sp)
200608ba:	44d2                	c.lwsp	s1,20(sp)
200608bc:	4942                	c.lwsp	s2,16(sp)
200608be:	49b2                	c.lwsp	s3,12(sp)
200608c0:	200646b7          	lui	a3,0x20064
200608c4:	200645b7          	lui	a1,0x20064
200608c8:	29c68693          	addi	a3,a3,668 # 2006429c <__func__.0+0x1c>
200608cc:	04500613          	addi	a2,zero,69
200608d0:	24858593          	addi	a1,a1,584 # 20064248 <register_string+0x140>
200608d4:	4509                	c.li	a0,2
200608d6:	6105                	c.addi16sp	sp,32
200608d8:	de0fa06f          	jal	zero,2005aeb8 <rtk_log_write>
200608dc:	40418993          	addi	s3,gp,1028 # 20066680 <wifi_user_config>
200608e0:	0249a503          	lw	a0,36(s3)
200608e4:	fe418493          	addi	s1,gp,-28 # 20066260 <host_skb_info>
200608e8:	40418913          	addi	s2,gp,1028 # 20066680 <wifi_user_config>
200608ec:	051a                	c.slli	a0,0x6
200608ee:	dcbfa0ef          	jal	ra,2005b6b8 <rtos_mem_zmalloc>
200608f2:	0249a783          	lw	a5,36(s3)
200608f6:	c088                	c.sw	a0,0(s1)
200608f8:	6a000513          	addi	a0,zero,1696
200608fc:	02f50533          	mul	a0,a0,a5
20060900:	db9fa0ef          	jal	ra,2005b6b8 <rtos_mem_zmalloc>
20060904:	409c                	c.lw	a5,0(s1)
20060906:	c008                	c.sw	a0,0(s0)
20060908:	c391                	c.beqz	a5,2006090c <inic_host_init_skb+0x72>
2006090a:	e115                	c.bnez	a0,2006092e <inic_host_init_skb+0x94>
2006090c:	20064737          	lui	a4,0x20064
20060910:	200646b7          	lui	a3,0x20064
20060914:	200645b7          	lui	a1,0x20064
20060918:	2e870713          	addi	a4,a4,744 # 200642e8 <__func__.1>
2006091c:	2b468693          	addi	a3,a3,692 # 200642b4 <__func__.0+0x34>
20060920:	04500613          	addi	a2,zero,69
20060924:	24858593          	addi	a1,a1,584 # 20064248 <register_string+0x140>
20060928:	4509                	c.li	a0,2
2006092a:	d8efa0ef          	jal	ra,2005aeb8 <rtk_log_write>
2006092e:	4681                	c.li	a3,0
20060930:	6a000593          	addi	a1,zero,1696
20060934:	02492703          	lw	a4,36(s2)
20060938:	409c                	c.lw	a5,0(s1)
2006093a:	0ae6cf63          	blt	a3,a4,200609f8 <inic_host_init_skb+0x15e>
2006093e:	7c1026f3          	csrrs	a3,0x7c1,zero
20060942:	1006f693          	andi	a3,a3,256
20060946:	c6a1                	c.beqz	a3,2006098e <inic_host_init_skb+0xf4>
20060948:	01f7f693          	andi	a3,a5,31
2006094c:	071a                	c.slli	a4,0x6
2006094e:	c299                	c.beqz	a3,20060954 <inic_host_init_skb+0xba>
20060950:	9b81                	c.andi	a5,-32
20060952:	9736                	c.add	a4,a3
20060954:	02e05d63          	bge	zero,a4,2006098e <inic_host_init_skb+0xf4>
20060958:	01f7f693          	andi	a3,a5,31
2006095c:	9736                	c.add	a4,a3
2006095e:	0ff0000f          	fence	iorw,iorw
20060962:	973e                	c.add	a4,a5
20060964:	04f7f00b          	cache	dwb,(a5)
20060968:	02078793          	addi	a5,a5,32
2006096c:	40f706b3          	sub	a3,a4,a5
20060970:	fed04ae3          	blt	zero,a3,20060964 <inic_host_init_skb+0xca>
20060974:	0ff0000f          	fence	iorw,iorw
20060978:	0001                	c.addi	zero,0
2006097a:	0001                	c.addi	zero,0
2006097c:	0001                	c.addi	zero,0
2006097e:	0001                	c.addi	zero,0
20060980:	0001                	c.addi	zero,0
20060982:	0001                	c.addi	zero,0
20060984:	0001                	c.addi	zero,0
20060986:	0001                	c.addi	zero,0
20060988:	0001                	c.addi	zero,0
2006098a:	0001                	c.addi	zero,0
2006098c:	0001                	c.addi	zero,0
2006098e:	401c                	c.lw	a5,0(s0)
20060990:	02492703          	lw	a4,36(s2)
20060994:	7c1026f3          	csrrs	a3,0x7c1,zero
20060998:	1006f693          	andi	a3,a3,256
2006099c:	c6b9                	c.beqz	a3,200609ea <inic_host_init_skb+0x150>
2006099e:	6a000693          	addi	a3,zero,1696
200609a2:	02d70733          	mul	a4,a4,a3
200609a6:	01f7f693          	andi	a3,a5,31
200609aa:	c299                	c.beqz	a3,200609b0 <inic_host_init_skb+0x116>
200609ac:	9b81                	c.andi	a5,-32
200609ae:	9736                	c.add	a4,a3
200609b0:	02e05d63          	bge	zero,a4,200609ea <inic_host_init_skb+0x150>
200609b4:	01f7f693          	andi	a3,a5,31
200609b8:	9736                	c.add	a4,a3
200609ba:	0ff0000f          	fence	iorw,iorw
200609be:	973e                	c.add	a4,a5
200609c0:	04f7f00b          	cache	dwb,(a5)
200609c4:	02078793          	addi	a5,a5,32
200609c8:	40f706b3          	sub	a3,a4,a5
200609cc:	fed04ae3          	blt	zero,a3,200609c0 <inic_host_init_skb+0x126>
200609d0:	0ff0000f          	fence	iorw,iorw
200609d4:	0001                	c.addi	zero,0
200609d6:	0001                	c.addi	zero,0
200609d8:	0001                	c.addi	zero,0
200609da:	0001                	c.addi	zero,0
200609dc:	0001                	c.addi	zero,0
200609de:	0001                	c.addi	zero,0
200609e0:	0001                	c.addi	zero,0
200609e2:	0001                	c.addi	zero,0
200609e4:	0001                	c.addi	zero,0
200609e6:	0001                	c.addi	zero,0
200609e8:	0001                	c.addi	zero,0
200609ea:	40f2                	c.lwsp	ra,28(sp)
200609ec:	4462                	c.lwsp	s0,24(sp)
200609ee:	44d2                	c.lwsp	s1,20(sp)
200609f0:	4942                	c.lwsp	s2,16(sp)
200609f2:	49b2                	c.lwsp	s3,12(sp)
200609f4:	6105                	c.addi16sp	sp,32
200609f6:	8082                	c.jr	ra
200609f8:	02b68633          	mul	a2,a3,a1
200609fc:	00669713          	slli	a4,a3,0x6
20060a00:	97ba                	c.add	a5,a4
20060a02:	c39c                	c.sw	a5,0(a5)
20060a04:	409c                	c.lw	a5,0(s1)
20060a06:	0685                	c.addi	a3,1
20060a08:	97ba                	c.add	a5,a4
20060a0a:	4018                	c.lw	a4,0(s0)
20060a0c:	c3dc                	c.sw	a5,4(a5)
20060a0e:	9732                	c.add	a4,a2
20060a10:	c318                	c.sw	a4,0(a4)
20060a12:	401c                	c.lw	a5,0(s0)
20060a14:	97b2                	c.add	a5,a2
20060a16:	c3dc                	c.sw	a5,4(a5)
20060a18:	bf31                	c.j	20060934 <inic_host_init_skb+0x9a>

20060a1a <inic_host_init_priv>:
20060a1a:	1141                	c.addi	sp,-16
20060a1c:	c226                	c.swsp	s1,4(sp)
20060a1e:	200684b7          	lui	s1,0x20068
20060a22:	02800613          	addi	a2,zero,40
20060a26:	4581                	c.li	a1,0
20060a28:	00048513          	addi	a0,s1,0 # 20068000 <g_inic_host_priv>
20060a2c:	c606                	c.swsp	ra,12(sp)
20060a2e:	c422                	c.swsp	s0,8(sp)
20060a30:	dffa9097          	auipc	ra,0xdffa9
20060a34:	09e080e7          	jalr	ra,158(ra) # 9ace <__wrap_memset>
20060a38:	567d                	c.li	a2,-1
20060a3a:	4581                	c.li	a1,0
20060a3c:	00048513          	addi	a0,s1,0
20060a40:	00048413          	addi	s0,s1,0
20060a44:	e21fa0ef          	jal	ra,2005b864 <rtos_sema_create_static>
20060a48:	567d                	c.li	a2,-1
20060a4a:	4581                	c.li	a1,0
20060a4c:	00440513          	addi	a0,s0,4
20060a50:	e15fa0ef          	jal	ra,2005b864 <rtos_sema_create_static>
20060a54:	567d                	c.li	a2,-1
20060a56:	4581                	c.li	a1,0
20060a58:	00840513          	addi	a0,s0,8
20060a5c:	e09fa0ef          	jal	ra,2005b864 <rtos_sema_create_static>
20060a60:	4408                	c.lw	a0,8(s0)
20060a62:	ecdfa0ef          	jal	ra,2005b92e <rtos_sema_give>
20060a66:	00c40513          	addi	a0,s0,12
20060a6a:	26b5                	c.jal	20060dd6 <rtw_init_queue>
20060a6c:	40b2                	c.lwsp	ra,12(sp)
20060a6e:	00042c23          	sw	zero,24(s0)
20060a72:	00042e23          	sw	zero,28(s0)
20060a76:	02042023          	sw	zero,32(s0)
20060a7a:	02042223          	sw	zero,36(s0)
20060a7e:	4422                	c.lwsp	s0,8(sp)
20060a80:	4492                	c.lwsp	s1,4(sp)
20060a82:	0141                	c.addi	sp,16
20060a84:	8082                	c.jr	ra

20060a86 <inic_host_trx_event_hdl>:
20060a86:	4791                	c.li	a5,4
20060a88:	872a                	c.mv	a4,a0
20060a8a:	00f50f63          	beq	a0,a5,20060aa8 <inic_host_trx_event_hdl+0x22>
20060a8e:	200646b7          	lui	a3,0x20064
20060a92:	200645b7          	lui	a1,0x20064
20060a96:	2cc68693          	addi	a3,a3,716 # 200642cc <__func__.0+0x4c>
20060a9a:	04500613          	addi	a2,zero,69
20060a9e:	24858593          	addi	a1,a1,584 # 20064248 <register_string+0x140>
20060aa2:	4509                	c.li	a0,2
20060aa4:	c14fa06f          	jal	zero,2005aeb8 <rtk_log_write>
20060aa8:	8082                	c.jr	ra

20060aaa <inic_host_trx_int_hdl>:
20060aaa:	1141                	c.addi	sp,-16
20060aac:	4599                	c.li	a1,6
20060aae:	4541                	c.li	a0,16
20060ab0:	c422                	c.swsp	s0,8(sp)
20060ab2:	c606                	c.swsp	ra,12(sp)
20060ab4:	e3cf80ef          	jal	ra,200590f0 <ipc_get_message>
20060ab8:	4140                	c.lw	s0,4(a0)
20060aba:	7c1027f3          	csrrs	a5,0x7c1,zero
20060abe:	1007f793          	andi	a5,a5,256
20060ac2:	c3b9                	c.beqz	a5,20060b08 <inic_host_trx_int_hdl+0x5e>
20060ac4:	01f47713          	andi	a4,s0,31
20060ac8:	c755                	c.beqz	a4,20060b74 <inic_host_trx_int_hdl+0xca>
20060aca:	04070693          	addi	a3,a4,64
20060ace:	fe047793          	andi	a5,s0,-32
20060ad2:	01f7f713          	andi	a4,a5,31
20060ad6:	9736                	c.add	a4,a3
20060ad8:	0ff0000f          	fence	iorw,iorw
20060adc:	973e                	c.add	a4,a5
20060ade:	00f7f00b          	cache	dinv,(a5)
20060ae2:	02078793          	addi	a5,a5,32
20060ae6:	40f706b3          	sub	a3,a4,a5
20060aea:	fed04ae3          	blt	zero,a3,20060ade <inic_host_trx_int_hdl+0x34>
20060aee:	0ff0000f          	fence	iorw,iorw
20060af2:	0001                	c.addi	zero,0
20060af4:	0001                	c.addi	zero,0
20060af6:	0001                	c.addi	zero,0
20060af8:	0001                	c.addi	zero,0
20060afa:	0001                	c.addi	zero,0
20060afc:	0001                	c.addi	zero,0
20060afe:	0001                	c.addi	zero,0
20060b00:	0001                	c.addi	zero,0
20060b02:	0001                	c.addi	zero,0
20060b04:	0001                	c.addi	zero,0
20060b06:	0001                	c.addi	zero,0
20060b08:	2455                	c.jal	20060dac <inic_msg_get_queue_status>
20060b0a:	c92d                	c.beqz	a0,20060b7c <inic_host_trx_int_hdl+0xd2>
20060b0c:	8522                	c.mv	a0,s0
20060b0e:	2af1                	c.jal	20060cea <inic_msg_enqueue>
20060b10:	157d                	c.addi	a0,-1
20060b12:	00a03533          	sltu	a0,zero,a0
20060b16:	c408                	c.sw	a0,8(s0)
20060b18:	00042023          	sw	zero,0(s0)
20060b1c:	7c1027f3          	csrrs	a5,0x7c1,zero
20060b20:	1007f793          	andi	a5,a5,256
20060b24:	c7a1                	c.beqz	a5,20060b6c <inic_host_trx_int_hdl+0xc2>
20060b26:	01f47793          	andi	a5,s0,31
20060b2a:	04000713          	addi	a4,zero,64
20060b2e:	c781                	c.beqz	a5,20060b36 <inic_host_trx_int_hdl+0x8c>
20060b30:	04078713          	addi	a4,a5,64
20060b34:	9801                	c.andi	s0,-32
20060b36:	01f47793          	andi	a5,s0,31
20060b3a:	97ba                	c.add	a5,a4
20060b3c:	0ff0000f          	fence	iorw,iorw
20060b40:	97a2                	c.add	a5,s0
20060b42:	0484700b          	cache	dwb,(s0)
20060b46:	02040413          	addi	s0,s0,32
20060b4a:	40878733          	sub	a4,a5,s0
20060b4e:	fee04ae3          	blt	zero,a4,20060b42 <inic_host_trx_int_hdl+0x98>
20060b52:	0ff0000f          	fence	iorw,iorw
20060b56:	0001                	c.addi	zero,0
20060b58:	0001                	c.addi	zero,0
20060b5a:	0001                	c.addi	zero,0
20060b5c:	0001                	c.addi	zero,0
20060b5e:	0001                	c.addi	zero,0
20060b60:	0001                	c.addi	zero,0
20060b62:	0001                	c.addi	zero,0
20060b64:	0001                	c.addi	zero,0
20060b66:	0001                	c.addi	zero,0
20060b68:	0001                	c.addi	zero,0
20060b6a:	0001                	c.addi	zero,0
20060b6c:	40b2                	c.lwsp	ra,12(sp)
20060b6e:	4422                	c.lwsp	s0,8(sp)
20060b70:	0141                	c.addi	sp,16
20060b72:	8082                	c.jr	ra
20060b74:	87a2                	c.mv	a5,s0
20060b76:	04000693          	addi	a3,zero,64
20060b7a:	bfa1                	c.j	20060ad2 <inic_host_trx_int_hdl+0x28>
20060b7c:	00044503          	lbu	a0,0(s0)
20060b80:	00c44603          	lbu	a2,12(s0)
20060b84:	404c                	c.lw	a1,4(s0)
20060b86:	f01ff0ef          	jal	ra,20060a86 <inic_host_trx_event_hdl>
20060b8a:	4501                	c.li	a0,0
20060b8c:	b769                	c.j	20060b16 <inic_host_trx_int_hdl+0x6c>

20060b8e <inic_msg_q_task>:
20060b8e:	1101                	c.addi	sp,-32
20060b90:	cc22                	c.swsp	s0,24(sp)
20060b92:	20068437          	lui	s0,0x20068
20060b96:	c84a                	c.swsp	s2,16(sp)
20060b98:	c64e                	c.swsp	s3,12(sp)
20060b9a:	08040913          	addi	s2,s0,128 # 20068080 <g_ipc_msg_q_priv>
20060b9e:	ce06                	c.swsp	ra,28(sp)
20060ba0:	ca26                	c.swsp	s1,20(sp)
20060ba2:	08040413          	addi	s0,s0,128
20060ba6:	4985                	c.li	s3,1
20060ba8:	00c92503          	lw	a0,12(s2)
20060bac:	55fd                	c.li	a1,-1
20060bae:	d35fa0ef          	jal	ra,2005b8e2 <rtos_sema_take>
20060bb2:	ab7fa0ef          	jal	ra,2005b668 <rtos_critical_enter>
20060bb6:	8522                	c.mv	a0,s0
20060bb8:	2435                	c.jal	20060de4 <rtw_queue_empty>
20060bba:	03351063          	bne	a0,s3,20060bda <inic_msg_q_task+0x4c>
20060bbe:	acffa0ef          	jal	ra,2005b68c <rtos_critical_exit>
20060bc2:	01444783          	lbu	a5,20(s0)
20060bc6:	f3ed                	c.bnez	a5,20060ba8 <inic_msg_q_task+0x1a>
20060bc8:	4462                	c.lwsp	s0,24(sp)
20060bca:	40f2                	c.lwsp	ra,28(sp)
20060bcc:	44d2                	c.lwsp	s1,20(sp)
20060bce:	4942                	c.lwsp	s2,16(sp)
20060bd0:	49b2                	c.lwsp	s3,12(sp)
20060bd2:	4501                	c.li	a0,0
20060bd4:	6105                	c.addi16sp	sp,32
20060bd6:	f9dfa06f          	jal	zero,2005bb72 <rtos_task_delete>
20060bda:	4004                	c.lw	s1,0(s0)
20060bdc:	8526                	c.mv	a0,s1
20060bde:	22ed                	c.jal	20060dc8 <rtw_list_delete>
20060be0:	aadfa0ef          	jal	ra,2005b68c <rtos_critical_exit>
20060be4:	dcf9                	c.beqz	s1,20060bc2 <inic_msg_q_task+0x34>
20060be6:	44c8                	c.lw	a0,12(s1)
20060be8:	448c                	c.lw	a1,8(s1)
20060bea:	00155613          	srli	a2,a0,0x1
20060bee:	810d                	c.srli	a0,0x3
20060bf0:	8a0d                	c.andi	a2,3
20060bf2:	897d                	c.andi	a0,31
20060bf4:	e93ff0ef          	jal	ra,20060a86 <inic_host_trx_event_hdl>
20060bf8:	a71fa0ef          	jal	ra,2005b668 <rtos_critical_enter>
20060bfc:	00c4c783          	lbu	a5,12(s1)
20060c00:	9bf9                	c.andi	a5,-2
20060c02:	00f48623          	sb	a5,12(s1)
20060c06:	a87fa0ef          	jal	ra,2005b68c <rtos_critical_exit>
20060c0a:	b765                	c.j	20060bb2 <inic_msg_q_task+0x24>

20060c0c <inic_msg_q_init>:
20060c0c:	1141                	c.addi	sp,-16
20060c0e:	c422                	c.swsp	s0,8(sp)
20060c10:	20068437          	lui	s0,0x20068
20060c14:	02000613          	addi	a2,zero,32
20060c18:	4581                	c.li	a1,0
20060c1a:	08040513          	addi	a0,s0,128 # 20068080 <g_ipc_msg_q_priv>
20060c1e:	c606                	c.swsp	ra,12(sp)
20060c20:	c226                	c.swsp	s1,4(sp)
20060c22:	dffa9097          	auipc	ra,0xdffa9
20060c26:	eac080e7          	jalr	ra,-340(ra) # 9ace <__wrap_memset>
20060c2a:	20068537          	lui	a0,0x20068
20060c2e:	04000613          	addi	a2,zero,64
20060c32:	4581                	c.li	a1,0
20060c34:	04050513          	addi	a0,a0,64 # 20068040 <g_inic_ipc_ex_msg>
20060c38:	dffa9097          	auipc	ra,0xdffa9
20060c3c:	e96080e7          	jalr	ra,-362(ra) # 9ace <__wrap_memset>
20060c40:	08040513          	addi	a0,s0,128
20060c44:	08040493          	addi	s1,s0,128
20060c48:	2279                	c.jal	20060dd6 <rtw_init_queue>
20060c4a:	567d                	c.li	a2,-1
20060c4c:	4581                	c.li	a1,0
20060c4e:	00c48513          	addi	a0,s1,12
20060c52:	c13fa0ef          	jal	ra,2005b864 <rtos_sema_create_static>
20060c56:	567d                	c.li	a2,-1
20060c58:	4581                	c.li	a1,0
20060c5a:	01048513          	addi	a0,s1,16
20060c5e:	c07fa0ef          	jal	ra,2005b864 <rtos_sema_create_static>
20060c62:	4888                	c.lw	a0,16(s1)
20060c64:	08040413          	addi	s0,s0,128
20060c68:	cc7fa0ef          	jal	ra,2005b92e <rtos_sema_give>
20060c6c:	4241a503          	lw	a0,1060(gp) # 200666a0 <wifi_user_config+0x20>
20060c70:	00a48e23          	sb	a0,28(s1)
20060c74:	0ff57513          	andi	a0,a0,255
20060c78:	0512                	c.slli	a0,0x4
20060c7a:	a3ffa0ef          	jal	ra,2005b6b8 <rtos_mem_zmalloc>
20060c7e:	cc88                	c.sw	a0,24(s1)
20060c80:	4781                	c.li	a5,0
20060c82:	01c44703          	lbu	a4,28(s0)
20060c86:	04e7c763          	blt	a5,a4,20060cd4 <inic_msg_q_init+0xc8>
20060c8a:	20061637          	lui	a2,0x20061
20060c8e:	200645b7          	lui	a1,0x20064
20060c92:	4799                	c.li	a5,6
20060c94:	31000713          	addi	a4,zero,784
20060c98:	4681                	c.li	a3,0
20060c9a:	b8e60613          	addi	a2,a2,-1138 # 20060b8e <inic_msg_q_task>
20060c9e:	2fc58593          	addi	a1,a1,764 # 200642fc <__func__.1+0x14>
20060ca2:	4501                	c.li	a0,0
20060ca4:	eabfa0ef          	jal	ra,2005bb4e <rtos_task_create>
20060ca8:	cd11                	c.beqz	a0,20060cc4 <inic_msg_q_init+0xb8>
20060caa:	200646b7          	lui	a3,0x20064
20060cae:	200645b7          	lui	a1,0x20064
20060cb2:	30c68693          	addi	a3,a3,780 # 2006430c <__func__.1+0x24>
20060cb6:	04500613          	addi	a2,zero,69
20060cba:	24858593          	addi	a1,a1,584 # 20064248 <register_string+0x140>
20060cbe:	4509                	c.li	a0,2
20060cc0:	9f8fa0ef          	jal	ra,2005aeb8 <rtk_log_write>
20060cc4:	4785                	c.li	a5,1
20060cc6:	40b2                	c.lwsp	ra,12(sp)
20060cc8:	00f40a23          	sb	a5,20(s0)
20060ccc:	4422                	c.lwsp	s0,8(sp)
20060cce:	4492                	c.lwsp	s1,4(sp)
20060cd0:	0141                	c.addi	sp,16
20060cd2:	8082                	c.jr	ra
20060cd4:	4c18                	c.lw	a4,24(s0)
20060cd6:	00479693          	slli	a3,a5,0x4
20060cda:	0785                	c.addi	a5,1
20060cdc:	9736                	c.add	a4,a3
20060cde:	00c74683          	lbu	a3,12(a4)
20060ce2:	9af9                	c.andi	a3,-2
20060ce4:	00d70623          	sb	a3,12(a4)
20060ce8:	bf69                	c.j	20060c82 <inic_msg_q_init+0x76>

20060cea <inic_msg_enqueue>:
20060cea:	1101                	c.addi	sp,-32
20060cec:	ca26                	c.swsp	s1,20(sp)
20060cee:	c84a                	c.swsp	s2,16(sp)
20060cf0:	c64e                	c.swsp	s3,12(sp)
20060cf2:	200684b7          	lui	s1,0x20068
20060cf6:	ce06                	c.swsp	ra,28(sp)
20060cf8:	cc22                	c.swsp	s0,24(sp)
20060cfa:	89aa                	c.mv	s3,a0
20060cfc:	96dfa0ef          	jal	ra,2005b668 <rtos_critical_enter>
20060d00:	08048793          	addi	a5,s1,128 # 20068080 <g_ipc_msg_q_priv>
20060d04:	01c7c683          	lbu	a3,28(a5)
20060d08:	4f90                	c.lw	a2,24(a5)
20060d0a:	08048913          	addi	s2,s1,128
20060d0e:	4781                	c.li	a5,0
20060d10:	00d7c463          	blt	a5,a3,20060d18 <inic_msg_enqueue+0x2e>
20060d14:	4401                	c.li	s0,0
20060d16:	a829                	c.j	20060d30 <inic_msg_enqueue+0x46>
20060d18:	00479413          	slli	s0,a5,0x4
20060d1c:	9432                	c.add	s0,a2
20060d1e:	00c44703          	lbu	a4,12(s0)
20060d22:	00177593          	andi	a1,a4,1
20060d26:	e1b1                	c.bnez	a1,20060d6a <inic_msg_enqueue+0x80>
20060d28:	00176713          	ori	a4,a4,1
20060d2c:	00e40623          	sb	a4,12(s0)
20060d30:	95dfa0ef          	jal	ra,2005b68c <rtos_critical_exit>
20060d34:	ec0d                	c.bnez	s0,20060d6e <inic_msg_enqueue+0x84>
20060d36:	200646b7          	lui	a3,0x20064
20060d3a:	200645b7          	lui	a1,0x20064
20060d3e:	32468693          	addi	a3,a3,804 # 20064324 <__func__.1+0x3c>
20060d42:	04500613          	addi	a2,zero,69
20060d46:	24858593          	addi	a1,a1,584 # 20064248 <register_string+0x140>
20060d4a:	4509                	c.li	a0,2
20060d4c:	96cfa0ef          	jal	ra,2005aeb8 <rtk_log_write>
20060d50:	547d                	c.li	s0,-1
20060d52:	00c92503          	lw	a0,12(s2)
20060d56:	bd9fa0ef          	jal	ra,2005b92e <rtos_sema_give>
20060d5a:	40f2                	c.lwsp	ra,28(sp)
20060d5c:	8522                	c.mv	a0,s0
20060d5e:	4462                	c.lwsp	s0,24(sp)
20060d60:	44d2                	c.lwsp	s1,20(sp)
20060d62:	4942                	c.lwsp	s2,16(sp)
20060d64:	49b2                	c.lwsp	s3,12(sp)
20060d66:	6105                	c.addi16sp	sp,32
20060d68:	8082                	c.jr	ra
20060d6a:	0785                	c.addi	a5,1
20060d6c:	b755                	c.j	20060d10 <inic_msg_enqueue+0x26>
20060d6e:	0009a783          	lw	a5,0(s3)
20060d72:	00379713          	slli	a4,a5,0x3
20060d76:	00c44783          	lbu	a5,12(s0)
20060d7a:	8b9d                	c.andi	a5,7
20060d7c:	8fd9                	c.or	a5,a4
20060d7e:	00f40623          	sb	a5,12(s0)
20060d82:	0049a703          	lw	a4,4(s3)
20060d86:	9be5                	c.andi	a5,-7
20060d88:	c418                	c.sw	a4,8(s0)
20060d8a:	00c9a703          	lw	a4,12(s3)
20060d8e:	8b0d                	c.andi	a4,3
20060d90:	0706                	c.slli	a4,0x1
20060d92:	8fd9                	c.or	a5,a4
20060d94:	00f40623          	sb	a5,12(s0)
20060d98:	8d1fa0ef          	jal	ra,2005b668 <rtos_critical_enter>
20060d9c:	8522                	c.mv	a0,s0
20060d9e:	08048593          	addi	a1,s1,128
20060da2:	2829                	c.jal	20060dbc <rtw_list_insert_tail>
20060da4:	4405                	c.li	s0,1
20060da6:	8e7fa0ef          	jal	ra,2005b68c <rtos_critical_exit>
20060daa:	b765                	c.j	20060d52 <inic_msg_enqueue+0x68>

20060dac <inic_msg_get_queue_status>:
20060dac:	200687b7          	lui	a5,0x20068
20060db0:	0947c503          	lbu	a0,148(a5) # 20068094 <g_ipc_msg_q_priv+0x14>
20060db4:	8082                	c.jr	ra

20060db6 <rtw_init_listhead>:
20060db6:	c108                	c.sw	a0,0(a0)
20060db8:	c148                	c.sw	a0,4(a0)
20060dba:	8082                	c.jr	ra

20060dbc <rtw_list_insert_tail>:
20060dbc:	41dc                	c.lw	a5,4(a1)
20060dbe:	c1c8                	c.sw	a0,4(a1)
20060dc0:	c10c                	c.sw	a1,0(a0)
20060dc2:	c15c                	c.sw	a5,4(a0)
20060dc4:	c388                	c.sw	a0,0(a5)
20060dc6:	8082                	c.jr	ra

20060dc8 <rtw_list_delete>:
20060dc8:	415c                	c.lw	a5,4(a0)
20060dca:	4118                	c.lw	a4,0(a0)
20060dcc:	c35c                	c.sw	a5,4(a4)
20060dce:	c398                	c.sw	a4,0(a5)
20060dd0:	c108                	c.sw	a0,0(a0)
20060dd2:	c148                	c.sw	a0,4(a0)
20060dd4:	8082                	c.jr	ra

20060dd6 <rtw_init_queue>:
20060dd6:	c108                	c.sw	a0,0(a0)
20060dd8:	fe81a783          	lw	a5,-24(gp) # 20066264 <p_wifi_rom2flash>
20060ddc:	c148                	c.sw	a0,4(a0)
20060dde:	0521                	c.addi	a0,8
20060de0:	539c                	c.lw	a5,32(a5)
20060de2:	8782                	c.jr	a5

20060de4 <rtw_queue_empty>:
20060de4:	411c                	c.lw	a5,0(a0)
20060de6:	40a78533          	sub	a0,a5,a0
20060dea:	00153513          	sltiu	a0,a0,1
20060dee:	8082                	c.jr	ra

20060df0 <init_timer_wrapper>:
20060df0:	1141                	c.addi	sp,-16
20060df2:	ff418513          	addi	a0,gp,-12 # 20066270 <timer_table>
20060df6:	c606                	c.swsp	ra,12(sp)
20060df8:	fbfff0ef          	jal	ra,20060db6 <rtw_init_listhead>
20060dfc:	40b2                	c.lwsp	ra,12(sp)
20060dfe:	fe01ae23          	sw	zero,-4(gp) # 20066278 <timer_used_num>
20060e02:	fe01a823          	sw	zero,-16(gp) # 2006626c <max_timer_used_num>
20060e06:	0141                	c.addi	sp,16
20060e08:	8082                	c.jr	ra

20060e0a <rtw_acquire_wakelock>:
20060e0a:	1141                	c.addi	sp,-16
20060e0c:	c606                	c.swsp	ra,12(sp)
20060e0e:	97df80ef          	jal	ra,2005978a <pmu_yield_os_check>
20060e12:	c511                	c.beqz	a0,20060e1e <rtw_acquire_wakelock+0x14>
20060e14:	40b2                	c.lwsp	ra,12(sp)
20060e16:	4505                	c.li	a0,1
20060e18:	0141                	c.addi	sp,16
20060e1a:	9bdf806f          	jal	zero,200597d6 <pmu_acquire_wakelock>
20060e1e:	40b2                	c.lwsp	ra,12(sp)
20060e20:	0141                	c.addi	sp,16
20060e22:	8082                	c.jr	ra

20060e24 <rtw_release_wakelock>:
20060e24:	1141                	c.addi	sp,-16
20060e26:	c606                	c.swsp	ra,12(sp)
20060e28:	963f80ef          	jal	ra,2005978a <pmu_yield_os_check>
20060e2c:	c511                	c.beqz	a0,20060e38 <rtw_release_wakelock+0x14>
20060e2e:	40b2                	c.lwsp	ra,12(sp)
20060e30:	4505                	c.li	a0,1
20060e32:	0141                	c.addi	sp,16
20060e34:	9c9f806f          	jal	zero,200597fc <pmu_release_wakelock>
20060e38:	40b2                	c.lwsp	ra,12(sp)
20060e3a:	0141                	c.addi	sp,16
20060e3c:	8082                	c.jr	ra

20060e3e <deregister_tm_clones>:
20060e3e:	20058537          	lui	a0,0x20058
20060e42:	200587b7          	lui	a5,0x20058
20060e46:	06050713          	addi	a4,a0,96 # 20058060 <BOOT_InitGP>
20060e4a:	06078793          	addi	a5,a5,96 # 20058060 <BOOT_InitGP>
20060e4e:	00e78863          	beq	a5,a4,20060e5e <deregister_tm_clones+0x20>
20060e52:	00000793          	addi	a5,zero,0
20060e56:	c781                	c.beqz	a5,20060e5e <deregister_tm_clones+0x20>
20060e58:	06050513          	addi	a0,a0,96
20060e5c:	8782                	c.jr	a5
20060e5e:	8082                	c.jr	ra

20060e60 <register_tm_clones>:
20060e60:	200587b7          	lui	a5,0x20058
20060e64:	20058537          	lui	a0,0x20058
20060e68:	06078593          	addi	a1,a5,96 # 20058060 <BOOT_InitGP>
20060e6c:	06050793          	addi	a5,a0,96 # 20058060 <BOOT_InitGP>
20060e70:	8d9d                	c.sub	a1,a5
20060e72:	8589                	c.srai	a1,0x2
20060e74:	4789                	c.li	a5,2
20060e76:	02f5c5b3          	div	a1,a1,a5
20060e7a:	c599                	c.beqz	a1,20060e88 <register_tm_clones+0x28>
20060e7c:	00000793          	addi	a5,zero,0
20060e80:	c781                	c.beqz	a5,20060e88 <register_tm_clones+0x28>
20060e82:	06050513          	addi	a0,a0,96
20060e86:	8782                	c.jr	a5
20060e88:	8082                	c.jr	ra

20060e8a <__do_global_dtors_aux>:
20060e8a:	200687b7          	lui	a5,0x20068
20060e8e:	0d87c703          	lbu	a4,216(a5) # 200680d8 <completed.1>
20060e92:	eb05                	c.bnez	a4,20060ec2 <__do_global_dtors_aux+0x38>
20060e94:	1141                	c.addi	sp,-16
20060e96:	c422                	c.swsp	s0,8(sp)
20060e98:	c606                	c.swsp	ra,12(sp)
20060e9a:	843e                	c.mv	s0,a5
20060e9c:	374d                	c.jal	20060e3e <deregister_tm_clones>
20060e9e:	00000793          	addi	a5,zero,0
20060ea2:	cb89                	c.beqz	a5,20060eb4 <__do_global_dtors_aux+0x2a>
20060ea4:	20058537          	lui	a0,0x20058
20060ea8:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
20060eac:	00000097          	auipc	ra,0x0
20060eb0:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20060eb4:	4785                	c.li	a5,1
20060eb6:	40b2                	c.lwsp	ra,12(sp)
20060eb8:	0cf40c23          	sb	a5,216(s0)
20060ebc:	4422                	c.lwsp	s0,8(sp)
20060ebe:	0141                	c.addi	sp,16
20060ec0:	8082                	c.jr	ra
20060ec2:	8082                	c.jr	ra

20060ec4 <frame_dummy>:
20060ec4:	00000793          	addi	a5,zero,0
20060ec8:	c38d                	c.beqz	a5,20060eea <frame_dummy+0x26>
20060eca:	200685b7          	lui	a1,0x20068
20060ece:	20058537          	lui	a0,0x20058
20060ed2:	1141                	c.addi	sp,-16
20060ed4:	0dc58593          	addi	a1,a1,220 # 200680dc <object.0>
20060ed8:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
20060edc:	c606                	c.swsp	ra,12(sp)
20060ede:	00000097          	auipc	ra,0x0
20060ee2:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20060ee6:	40b2                	c.lwsp	ra,12(sp)
20060ee8:	0141                	c.addi	sp,16
20060eea:	bf9d                	c.j	20060e60 <register_tm_clones>

20060eec <__clzsi2>:
20060eec:	67c1                	c.lui	a5,0x10
20060eee:	02f57663          	bgeu	a0,a5,20060f1a <__clzsi2+0x2e>
20060ef2:	0ff00793          	addi	a5,zero,255
20060ef6:	00a7b7b3          	sltu	a5,a5,a0
20060efa:	078e                	c.slli	a5,0x3
20060efc:	20064737          	lui	a4,0x20064
20060f00:	02000693          	addi	a3,zero,32
20060f04:	8e9d                	c.sub	a3,a5
20060f06:	00f55533          	srl	a0,a0,a5
20060f0a:	33c70793          	addi	a5,a4,828 # 2006433c <__clz_tab>
20060f0e:	953e                	c.add	a0,a5
20060f10:	00054503          	lbu	a0,0(a0)
20060f14:	40a68533          	sub	a0,a3,a0
20060f18:	8082                	c.jr	ra
20060f1a:	01000737          	lui	a4,0x1000
20060f1e:	47c1                	c.li	a5,16
20060f20:	fce56ee3          	bltu	a0,a4,20060efc <__clzsi2+0x10>
20060f24:	47e1                	c.li	a5,24
20060f26:	bfd9                	c.j	20060efc <__clzsi2+0x10>

20060f28 <__udivdi3>:
20060f28:	88aa                	c.mv	a7,a0
20060f2a:	87ae                	c.mv	a5,a1
20060f2c:	8832                	c.mv	a6,a2
20060f2e:	8536                	c.mv	a0,a3
20060f30:	8346                	c.mv	t1,a7
20060f32:	20069663          	bne	a3,zero,2006113e <__udivdi3+0x216>
20060f36:	200646b7          	lui	a3,0x20064
20060f3a:	33c68693          	addi	a3,a3,828 # 2006433c <__clz_tab>
20060f3e:	0ac5ff63          	bgeu	a1,a2,20060ffc <__udivdi3+0xd4>
20060f42:	6741                	c.lui	a4,0x10
20060f44:	0ae67563          	bgeu	a2,a4,20060fee <__udivdi3+0xc6>
20060f48:	0ff00713          	addi	a4,zero,255
20060f4c:	00c73733          	sltu	a4,a4,a2
20060f50:	070e                	c.slli	a4,0x3
20060f52:	00e65533          	srl	a0,a2,a4
20060f56:	96aa                	c.add	a3,a0
20060f58:	0006c683          	lbu	a3,0(a3)
20060f5c:	02000513          	addi	a0,zero,32
20060f60:	9736                	c.add	a4,a3
20060f62:	40e506b3          	sub	a3,a0,a4
20060f66:	00e50b63          	beq	a0,a4,20060f7c <__udivdi3+0x54>
20060f6a:	00d795b3          	sll	a1,a5,a3
20060f6e:	00e8d733          	srl	a4,a7,a4
20060f72:	00d61833          	sll	a6,a2,a3
20060f76:	8dd9                	c.or	a1,a4
20060f78:	00d89333          	sll	t1,a7,a3
20060f7c:	01085893          	srli	a7,a6,0x10
20060f80:	0315d6b3          	divu	a3,a1,a7
20060f84:	01081613          	slli	a2,a6,0x10
20060f88:	8241                	c.srli	a2,0x10
20060f8a:	01035793          	srli	a5,t1,0x10
20060f8e:	0315f733          	remu	a4,a1,a7
20060f92:	8536                	c.mv	a0,a3
20060f94:	02d605b3          	mul	a1,a2,a3
20060f98:	0742                	c.slli	a4,0x10
20060f9a:	8fd9                	c.or	a5,a4
20060f9c:	00b7fc63          	bgeu	a5,a1,20060fb4 <__udivdi3+0x8c>
20060fa0:	97c2                	c.add	a5,a6
20060fa2:	fff68513          	addi	a0,a3,-1
20060fa6:	0107e763          	bltu	a5,a6,20060fb4 <__udivdi3+0x8c>
20060faa:	00b7f563          	bgeu	a5,a1,20060fb4 <__udivdi3+0x8c>
20060fae:	ffe68513          	addi	a0,a3,-2
20060fb2:	97c2                	c.add	a5,a6
20060fb4:	8f8d                	c.sub	a5,a1
20060fb6:	0317f733          	remu	a4,a5,a7
20060fba:	0342                	c.slli	t1,0x10
20060fbc:	01035313          	srli	t1,t1,0x10
20060fc0:	0317d7b3          	divu	a5,a5,a7
20060fc4:	0742                	c.slli	a4,0x10
20060fc6:	00676333          	or	t1,a4,t1
20060fca:	02f606b3          	mul	a3,a2,a5
20060fce:	863e                	c.mv	a2,a5
20060fd0:	00d37b63          	bgeu	t1,a3,20060fe6 <__udivdi3+0xbe>
20060fd4:	9342                	c.add	t1,a6
20060fd6:	fff78613          	addi	a2,a5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20060fda:	01036663          	bltu	t1,a6,20060fe6 <__udivdi3+0xbe>
20060fde:	00d37463          	bgeu	t1,a3,20060fe6 <__udivdi3+0xbe>
20060fe2:	ffe78613          	addi	a2,a5,-2
20060fe6:	0542                	c.slli	a0,0x10
20060fe8:	8d51                	c.or	a0,a2
20060fea:	4581                	c.li	a1,0
20060fec:	a85d                	c.j	200610a2 <__udivdi3+0x17a>
20060fee:	01000537          	lui	a0,0x1000
20060ff2:	4741                	c.li	a4,16
20060ff4:	f4a66fe3          	bltu	a2,a0,20060f52 <__udivdi3+0x2a>
20060ff8:	4761                	c.li	a4,24
20060ffa:	bfa1                	c.j	20060f52 <__udivdi3+0x2a>
20060ffc:	e601                	c.bnez	a2,20061004 <__udivdi3+0xdc>
20060ffe:	4705                	c.li	a4,1
20061000:	02c75833          	divu	a6,a4,a2
20061004:	6741                	c.lui	a4,0x10
20061006:	08e87f63          	bgeu	a6,a4,200610a4 <__udivdi3+0x17c>
2006100a:	0ff00713          	addi	a4,zero,255
2006100e:	01077363          	bgeu	a4,a6,20061014 <__udivdi3+0xec>
20061012:	4521                	c.li	a0,8
20061014:	00a85733          	srl	a4,a6,a0
20061018:	96ba                	c.add	a3,a4
2006101a:	0006c703          	lbu	a4,0(a3)
2006101e:	02000613          	addi	a2,zero,32
20061022:	972a                	c.add	a4,a0
20061024:	40e606b3          	sub	a3,a2,a4
20061028:	08e61563          	bne	a2,a4,200610b2 <__udivdi3+0x18a>
2006102c:	410787b3          	sub	a5,a5,a6
20061030:	4585                	c.li	a1,1
20061032:	01085893          	srli	a7,a6,0x10
20061036:	01081613          	slli	a2,a6,0x10
2006103a:	8241                	c.srli	a2,0x10
2006103c:	01035713          	srli	a4,t1,0x10
20061040:	0317f6b3          	remu	a3,a5,a7
20061044:	0317d7b3          	divu	a5,a5,a7
20061048:	06c2                	c.slli	a3,0x10
2006104a:	8f55                	c.or	a4,a3
2006104c:	02f60e33          	mul	t3,a2,a5
20061050:	853e                	c.mv	a0,a5
20061052:	01c77c63          	bgeu	a4,t3,2006106a <__udivdi3+0x142>
20061056:	9742                	c.add	a4,a6
20061058:	fff78513          	addi	a0,a5,-1
2006105c:	01076763          	bltu	a4,a6,2006106a <__udivdi3+0x142>
20061060:	01c77563          	bgeu	a4,t3,2006106a <__udivdi3+0x142>
20061064:	ffe78513          	addi	a0,a5,-2
20061068:	9742                	c.add	a4,a6
2006106a:	41c70733          	sub	a4,a4,t3
2006106e:	031777b3          	remu	a5,a4,a7
20061072:	0342                	c.slli	t1,0x10
20061074:	01035313          	srli	t1,t1,0x10
20061078:	03175733          	divu	a4,a4,a7
2006107c:	07c2                	c.slli	a5,0x10
2006107e:	0067e333          	or	t1,a5,t1
20061082:	02e606b3          	mul	a3,a2,a4
20061086:	863a                	c.mv	a2,a4
20061088:	00d37b63          	bgeu	t1,a3,2006109e <__udivdi3+0x176>
2006108c:	9342                	c.add	t1,a6
2006108e:	fff70613          	addi	a2,a4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061092:	01036663          	bltu	t1,a6,2006109e <__udivdi3+0x176>
20061096:	00d37463          	bgeu	t1,a3,2006109e <__udivdi3+0x176>
2006109a:	ffe70613          	addi	a2,a4,-2
2006109e:	0542                	c.slli	a0,0x10
200610a0:	8d51                	c.or	a0,a2
200610a2:	8082                	c.jr	ra
200610a4:	01000737          	lui	a4,0x1000
200610a8:	4541                	c.li	a0,16
200610aa:	f6e865e3          	bltu	a6,a4,20061014 <__udivdi3+0xec>
200610ae:	4561                	c.li	a0,24
200610b0:	b795                	c.j	20061014 <__udivdi3+0xec>
200610b2:	00d81833          	sll	a6,a6,a3
200610b6:	00e7d533          	srl	a0,a5,a4
200610ba:	00d89333          	sll	t1,a7,a3
200610be:	00d797b3          	sll	a5,a5,a3
200610c2:	00e8d733          	srl	a4,a7,a4
200610c6:	01085893          	srli	a7,a6,0x10
200610ca:	00f76633          	or	a2,a4,a5
200610ce:	03157733          	remu	a4,a0,a7
200610d2:	01081793          	slli	a5,a6,0x10
200610d6:	83c1                	c.srli	a5,0x10
200610d8:	01065593          	srli	a1,a2,0x10
200610dc:	03155533          	divu	a0,a0,a7
200610e0:	0742                	c.slli	a4,0x10
200610e2:	8f4d                	c.or	a4,a1
200610e4:	02a786b3          	mul	a3,a5,a0
200610e8:	85aa                	c.mv	a1,a0
200610ea:	00d77c63          	bgeu	a4,a3,20061102 <__udivdi3+0x1da>
200610ee:	9742                	c.add	a4,a6
200610f0:	fff50593          	addi	a1,a0,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
200610f4:	01076763          	bltu	a4,a6,20061102 <__udivdi3+0x1da>
200610f8:	00d77563          	bgeu	a4,a3,20061102 <__udivdi3+0x1da>
200610fc:	ffe50593          	addi	a1,a0,-2
20061100:	9742                	c.add	a4,a6
20061102:	40d706b3          	sub	a3,a4,a3
20061106:	0316f733          	remu	a4,a3,a7
2006110a:	0642                	c.slli	a2,0x10
2006110c:	8241                	c.srli	a2,0x10
2006110e:	0316d6b3          	divu	a3,a3,a7
20061112:	0742                	c.slli	a4,0x10
20061114:	02d78533          	mul	a0,a5,a3
20061118:	00c767b3          	or	a5,a4,a2
2006111c:	8736                	c.mv	a4,a3
2006111e:	00a7fc63          	bgeu	a5,a0,20061136 <__udivdi3+0x20e>
20061122:	97c2                	c.add	a5,a6
20061124:	fff68713          	addi	a4,a3,-1
20061128:	0107e763          	bltu	a5,a6,20061136 <__udivdi3+0x20e>
2006112c:	00a7f563          	bgeu	a5,a0,20061136 <__udivdi3+0x20e>
20061130:	ffe68713          	addi	a4,a3,-2
20061134:	97c2                	c.add	a5,a6
20061136:	05c2                	c.slli	a1,0x10
20061138:	8f89                	c.sub	a5,a0
2006113a:	8dd9                	c.or	a1,a4
2006113c:	bddd                	c.j	20061032 <__udivdi3+0x10a>
2006113e:	14d5e263          	bltu	a1,a3,20061282 <__udivdi3+0x35a>
20061142:	6741                	c.lui	a4,0x10
20061144:	02e6ff63          	bgeu	a3,a4,20061182 <__udivdi3+0x25a>
20061148:	0ff00713          	addi	a4,zero,255
2006114c:	00d735b3          	sltu	a1,a4,a3
20061150:	058e                	c.slli	a1,0x3
20061152:	20064737          	lui	a4,0x20064
20061156:	00b6d533          	srl	a0,a3,a1
2006115a:	33c70713          	addi	a4,a4,828 # 2006433c <__clz_tab>
2006115e:	972a                	c.add	a4,a0
20061160:	00074703          	lbu	a4,0(a4)
20061164:	02000513          	addi	a0,zero,32
20061168:	972e                	c.add	a4,a1
2006116a:	40e505b3          	sub	a1,a0,a4
2006116e:	02e51163          	bne	a0,a4,20061190 <__udivdi3+0x268>
20061172:	4505                	c.li	a0,1
20061174:	f2f6e7e3          	bltu	a3,a5,200610a2 <__udivdi3+0x17a>
20061178:	00c8b533          	sltu	a0,a7,a2
2006117c:	00154513          	xori	a0,a0,1
20061180:	b70d                	c.j	200610a2 <__udivdi3+0x17a>
20061182:	01000737          	lui	a4,0x1000
20061186:	45c1                	c.li	a1,16
20061188:	fce6e5e3          	bltu	a3,a4,20061152 <__udivdi3+0x22a>
2006118c:	45e1                	c.li	a1,24
2006118e:	b7d1                	c.j	20061152 <__udivdi3+0x22a>
20061190:	00e65833          	srl	a6,a2,a4
20061194:	00b696b3          	sll	a3,a3,a1
20061198:	00d86833          	or	a6,a6,a3
2006119c:	00e7de33          	srl	t3,a5,a4
200611a0:	01085e93          	srli	t4,a6,0x10
200611a4:	03de76b3          	remu	a3,t3,t4
200611a8:	00b797b3          	sll	a5,a5,a1
200611ac:	00e8d733          	srl	a4,a7,a4
200611b0:	00b61333          	sll	t1,a2,a1
200611b4:	00f76633          	or	a2,a4,a5
200611b8:	01081793          	slli	a5,a6,0x10
200611bc:	83c1                	c.srli	a5,0x10
200611be:	01065713          	srli	a4,a2,0x10
200611c2:	03de5e33          	divu	t3,t3,t4
200611c6:	06c2                	c.slli	a3,0x10
200611c8:	8f55                	c.or	a4,a3
200611ca:	03c78f33          	mul	t5,a5,t3
200611ce:	8572                	c.mv	a0,t3
200611d0:	01e77c63          	bgeu	a4,t5,200611e8 <__udivdi3+0x2c0>
200611d4:	9742                	c.add	a4,a6
200611d6:	fffe0513          	addi	a0,t3,-1
200611da:	01076763          	bltu	a4,a6,200611e8 <__udivdi3+0x2c0>
200611de:	01e77563          	bgeu	a4,t5,200611e8 <__udivdi3+0x2c0>
200611e2:	ffee0513          	addi	a0,t3,-2
200611e6:	9742                	c.add	a4,a6
200611e8:	41e70733          	sub	a4,a4,t5
200611ec:	03d776b3          	remu	a3,a4,t4
200611f0:	03d75733          	divu	a4,a4,t4
200611f4:	06c2                	c.slli	a3,0x10
200611f6:	02e78e33          	mul	t3,a5,a4
200611fa:	01061793          	slli	a5,a2,0x10
200611fe:	83c1                	c.srli	a5,0x10
20061200:	8fd5                	c.or	a5,a3
20061202:	863a                	c.mv	a2,a4
20061204:	01c7fc63          	bgeu	a5,t3,2006121c <__udivdi3+0x2f4>
20061208:	97c2                	c.add	a5,a6
2006120a:	fff70613          	addi	a2,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
2006120e:	0107e763          	bltu	a5,a6,2006121c <__udivdi3+0x2f4>
20061212:	01c7f563          	bgeu	a5,t3,2006121c <__udivdi3+0x2f4>
20061216:	ffe70613          	addi	a2,a4,-2
2006121a:	97c2                	c.add	a5,a6
2006121c:	0542                	c.slli	a0,0x10
2006121e:	6ec1                	c.lui	t4,0x10
20061220:	8d51                	c.or	a0,a2
20061222:	fffe8693          	addi	a3,t4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061226:	00d57833          	and	a6,a0,a3
2006122a:	01055613          	srli	a2,a0,0x10
2006122e:	00d376b3          	and	a3,t1,a3
20061232:	01035313          	srli	t1,t1,0x10
20061236:	41c787b3          	sub	a5,a5,t3
2006123a:	02d80e33          	mul	t3,a6,a3
2006123e:	02d606b3          	mul	a3,a2,a3
20061242:	010e5713          	srli	a4,t3,0x10
20061246:	02680833          	mul	a6,a6,t1
2006124a:	9836                	c.add	a6,a3
2006124c:	9742                	c.add	a4,a6
2006124e:	02660633          	mul	a2,a2,t1
20061252:	00d77363          	bgeu	a4,a3,20061258 <__udivdi3+0x330>
20061256:	9676                	c.add	a2,t4
20061258:	01075693          	srli	a3,a4,0x10
2006125c:	9636                	c.add	a2,a3
2006125e:	02c7e063          	bltu	a5,a2,2006127e <__udivdi3+0x356>
20061262:	d8c794e3          	bne	a5,a2,20060fea <__udivdi3+0xc2>
20061266:	67c1                	c.lui	a5,0x10
20061268:	17fd                	c.addi	a5,-1
2006126a:	8f7d                	c.and	a4,a5
2006126c:	0742                	c.slli	a4,0x10
2006126e:	00fe7e33          	and	t3,t3,a5
20061272:	00b898b3          	sll	a7,a7,a1
20061276:	9772                	c.add	a4,t3
20061278:	4581                	c.li	a1,0
2006127a:	e2e8f4e3          	bgeu	a7,a4,200610a2 <__udivdi3+0x17a>
2006127e:	157d                	c.addi	a0,-1
20061280:	b3ad                	c.j	20060fea <__udivdi3+0xc2>
20061282:	4581                	c.li	a1,0
20061284:	4501                	c.li	a0,0
20061286:	bd31                	c.j	200610a2 <__udivdi3+0x17a>

20061288 <__umoddi3>:
20061288:	8eaa                	c.mv	t4,a0
2006128a:	832e                	c.mv	t1,a1
2006128c:	8832                	c.mv	a6,a2
2006128e:	87b6                	c.mv	a5,a3
20061290:	1c069263          	bne	a3,zero,20061454 <__umoddi3+0x1cc>
20061294:	20064737          	lui	a4,0x20064
20061298:	33c70713          	addi	a4,a4,828 # 2006433c <__clz_tab>
2006129c:	0ac5f563          	bgeu	a1,a2,20061346 <__umoddi3+0xbe>
200612a0:	66c1                	c.lui	a3,0x10
200612a2:	08d67b63          	bgeu	a2,a3,20061338 <__umoddi3+0xb0>
200612a6:	0ff00693          	addi	a3,zero,255
200612aa:	00c6f363          	bgeu	a3,a2,200612b0 <__umoddi3+0x28>
200612ae:	47a1                	c.li	a5,8
200612b0:	00f656b3          	srl	a3,a2,a5
200612b4:	9736                	c.add	a4,a3
200612b6:	00074703          	lbu	a4,0(a4)
200612ba:	97ba                	c.add	a5,a4
200612bc:	02000713          	addi	a4,zero,32
200612c0:	40f708b3          	sub	a7,a4,a5
200612c4:	00f70b63          	beq	a4,a5,200612da <__umoddi3+0x52>
200612c8:	011315b3          	sll	a1,t1,a7
200612cc:	00fed7b3          	srl	a5,t4,a5
200612d0:	01161833          	sll	a6,a2,a7
200612d4:	8ddd                	c.or	a1,a5
200612d6:	011e9533          	sll	a0,t4,a7
200612da:	01085693          	srli	a3,a6,0x10
200612de:	02d5f7b3          	remu	a5,a1,a3
200612e2:	01081613          	slli	a2,a6,0x10
200612e6:	8241                	c.srli	a2,0x10
200612e8:	01055713          	srli	a4,a0,0x10
200612ec:	02d5d5b3          	divu	a1,a1,a3
200612f0:	07c2                	c.slli	a5,0x10
200612f2:	8f5d                	c.or	a4,a5
200612f4:	02b605b3          	mul	a1,a2,a1
200612f8:	00b77863          	bgeu	a4,a1,20061308 <__umoddi3+0x80>
200612fc:	9742                	c.add	a4,a6
200612fe:	01076563          	bltu	a4,a6,20061308 <__umoddi3+0x80>
20061302:	00b77363          	bgeu	a4,a1,20061308 <__umoddi3+0x80>
20061306:	9742                	c.add	a4,a6
20061308:	8f0d                	c.sub	a4,a1
2006130a:	02d777b3          	remu	a5,a4,a3
2006130e:	0542                	c.slli	a0,0x10
20061310:	8141                	c.srli	a0,0x10
20061312:	02d75733          	divu	a4,a4,a3
20061316:	07c2                	c.slli	a5,0x10
20061318:	8d5d                	c.or	a0,a5
2006131a:	02e60733          	mul	a4,a2,a4
2006131e:	00e57863          	bgeu	a0,a4,2006132e <__umoddi3+0xa6>
20061322:	9542                	c.add	a0,a6
20061324:	01056563          	bltu	a0,a6,2006132e <__umoddi3+0xa6>
20061328:	00e57363          	bgeu	a0,a4,2006132e <__umoddi3+0xa6>
2006132c:	9542                	c.add	a0,a6
2006132e:	8d19                	c.sub	a0,a4
20061330:	01155533          	srl	a0,a0,a7
20061334:	4581                	c.li	a1,0
20061336:	8082                	c.jr	ra
20061338:	010006b7          	lui	a3,0x1000
2006133c:	47c1                	c.li	a5,16
2006133e:	f6d669e3          	bltu	a2,a3,200612b0 <__umoddi3+0x28>
20061342:	47e1                	c.li	a5,24
20061344:	b7b5                	c.j	200612b0 <__umoddi3+0x28>
20061346:	e601                	c.bnez	a2,2006134e <__umoddi3+0xc6>
20061348:	4685                	c.li	a3,1
2006134a:	02c6d833          	divu	a6,a3,a2
2006134e:	66c1                	c.lui	a3,0x10
20061350:	0ed87b63          	bgeu	a6,a3,20061446 <__umoddi3+0x1be>
20061354:	0ff00693          	addi	a3,zero,255
20061358:	0106f363          	bgeu	a3,a6,2006135e <__umoddi3+0xd6>
2006135c:	47a1                	c.li	a5,8
2006135e:	00f856b3          	srl	a3,a6,a5
20061362:	9736                	c.add	a4,a3
20061364:	00074603          	lbu	a2,0(a4)
20061368:	410305b3          	sub	a1,t1,a6
2006136c:	963e                	c.add	a2,a5
2006136e:	02000793          	addi	a5,zero,32
20061372:	40c788b3          	sub	a7,a5,a2
20061376:	06c78a63          	beq	a5,a2,200613ea <__umoddi3+0x162>
2006137a:	01181833          	sll	a6,a6,a7
2006137e:	00c357b3          	srl	a5,t1,a2
20061382:	011315b3          	sll	a1,t1,a7
20061386:	01085313          	srli	t1,a6,0x10
2006138a:	00ced633          	srl	a2,t4,a2
2006138e:	8e4d                	c.or	a2,a1
20061390:	01081593          	slli	a1,a6,0x10
20061394:	81c1                	c.srli	a1,0x10
20061396:	011e9533          	sll	a0,t4,a7
2006139a:	0267d6b3          	divu	a3,a5,t1
2006139e:	0267f733          	remu	a4,a5,t1
200613a2:	01065793          	srli	a5,a2,0x10
200613a6:	02d586b3          	mul	a3,a1,a3
200613aa:	0742                	c.slli	a4,0x10
200613ac:	8f5d                	c.or	a4,a5
200613ae:	00d77863          	bgeu	a4,a3,200613be <__umoddi3+0x136>
200613b2:	9742                	c.add	a4,a6
200613b4:	01076563          	bltu	a4,a6,200613be <__umoddi3+0x136>
200613b8:	00d77363          	bgeu	a4,a3,200613be <__umoddi3+0x136>
200613bc:	9742                	c.add	a4,a6
200613be:	40d706b3          	sub	a3,a4,a3
200613c2:	0266f733          	remu	a4,a3,t1
200613c6:	0642                	c.slli	a2,0x10
200613c8:	8241                	c.srli	a2,0x10
200613ca:	0266d6b3          	divu	a3,a3,t1
200613ce:	0742                	c.slli	a4,0x10
200613d0:	02d586b3          	mul	a3,a1,a3
200613d4:	00c765b3          	or	a1,a4,a2
200613d8:	00d5f863          	bgeu	a1,a3,200613e8 <__umoddi3+0x160>
200613dc:	95c2                	c.add	a1,a6
200613de:	0105e563          	bltu	a1,a6,200613e8 <__umoddi3+0x160>
200613e2:	00d5f363          	bgeu	a1,a3,200613e8 <__umoddi3+0x160>
200613e6:	95c2                	c.add	a1,a6
200613e8:	8d95                	c.sub	a1,a3
200613ea:	01085693          	srli	a3,a6,0x10
200613ee:	02d5f733          	remu	a4,a1,a3
200613f2:	01081793          	slli	a5,a6,0x10
200613f6:	83c1                	c.srli	a5,0x10
200613f8:	01055613          	srli	a2,a0,0x10
200613fc:	02d5d5b3          	divu	a1,a1,a3
20061400:	0742                	c.slli	a4,0x10
20061402:	8f51                	c.or	a4,a2
20061404:	02b785b3          	mul	a1,a5,a1
20061408:	00b77863          	bgeu	a4,a1,20061418 <__umoddi3+0x190>
2006140c:	9742                	c.add	a4,a6
2006140e:	01076563          	bltu	a4,a6,20061418 <__umoddi3+0x190>
20061412:	00b77363          	bgeu	a4,a1,20061418 <__umoddi3+0x190>
20061416:	9742                	c.add	a4,a6
20061418:	40b705b3          	sub	a1,a4,a1
2006141c:	02d5f733          	remu	a4,a1,a3
20061420:	0542                	c.slli	a0,0x10
20061422:	8141                	c.srli	a0,0x10
20061424:	02d5d5b3          	divu	a1,a1,a3
20061428:	02b785b3          	mul	a1,a5,a1
2006142c:	01071793          	slli	a5,a4,0x10
20061430:	8d5d                	c.or	a0,a5
20061432:	00b57863          	bgeu	a0,a1,20061442 <__umoddi3+0x1ba>
20061436:	9542                	c.add	a0,a6
20061438:	01056563          	bltu	a0,a6,20061442 <__umoddi3+0x1ba>
2006143c:	00b57363          	bgeu	a0,a1,20061442 <__umoddi3+0x1ba>
20061440:	9542                	c.add	a0,a6
20061442:	8d0d                	c.sub	a0,a1
20061444:	b5f5                	c.j	20061330 <__umoddi3+0xa8>
20061446:	010006b7          	lui	a3,0x1000
2006144a:	47c1                	c.li	a5,16
2006144c:	f0d869e3          	bltu	a6,a3,2006135e <__umoddi3+0xd6>
20061450:	47e1                	c.li	a5,24
20061452:	b731                	c.j	2006135e <__umoddi3+0xd6>
20061454:	eed5e1e3          	bltu	a1,a3,20061336 <__umoddi3+0xae>
20061458:	67c1                	c.lui	a5,0x10
2006145a:	04f6f463          	bgeu	a3,a5,200614a2 <__umoddi3+0x21a>
2006145e:	0ff00893          	addi	a7,zero,255
20061462:	00d8b7b3          	sltu	a5,a7,a3
20061466:	078e                	c.slli	a5,0x3
20061468:	20064737          	lui	a4,0x20064
2006146c:	00f6d833          	srl	a6,a3,a5
20061470:	33c70713          	addi	a4,a4,828 # 2006433c <__clz_tab>
20061474:	9742                	c.add	a4,a6
20061476:	00074883          	lbu	a7,0(a4)
2006147a:	98be                	c.add	a7,a5
2006147c:	02000793          	addi	a5,zero,32
20061480:	41178833          	sub	a6,a5,a7
20061484:	03179663          	bne	a5,a7,200614b0 <__umoddi3+0x228>
20061488:	0066e463          	bltu	a3,t1,20061490 <__umoddi3+0x208>
2006148c:	eacee5e3          	bltu	t4,a2,20061336 <__umoddi3+0xae>
20061490:	40ce8533          	sub	a0,t4,a2
20061494:	40d305b3          	sub	a1,t1,a3
20061498:	00aebeb3          	sltu	t4,t4,a0
2006149c:	41d585b3          	sub	a1,a1,t4
200614a0:	bd59                	c.j	20061336 <__umoddi3+0xae>
200614a2:	01000737          	lui	a4,0x1000
200614a6:	47c1                	c.li	a5,16
200614a8:	fce6e0e3          	bltu	a3,a4,20061468 <__umoddi3+0x1e0>
200614ac:	47e1                	c.li	a5,24
200614ae:	bf6d                	c.j	20061468 <__umoddi3+0x1e0>
200614b0:	011657b3          	srl	a5,a2,a7
200614b4:	010696b3          	sll	a3,a3,a6
200614b8:	00d7ee33          	or	t3,a5,a3
200614bc:	01135f33          	srl	t5,t1,a7
200614c0:	011ed7b3          	srl	a5,t4,a7
200614c4:	010e9533          	sll	a0,t4,a6
200614c8:	010e5e93          	srli	t4,t3,0x10
200614cc:	010315b3          	sll	a1,t1,a6
200614d0:	03df5333          	divu	t1,t5,t4
200614d4:	8fcd                	c.or	a5,a1
200614d6:	010e1593          	slli	a1,t3,0x10
200614da:	81c1                	c.srli	a1,0x10
200614dc:	0107d713          	srli	a4,a5,0x10
200614e0:	01061633          	sll	a2,a2,a6
200614e4:	03df76b3          	remu	a3,t5,t4
200614e8:	02658f33          	mul	t5,a1,t1
200614ec:	06c2                	c.slli	a3,0x10
200614ee:	8f55                	c.or	a4,a3
200614f0:	869a                	c.mv	a3,t1
200614f2:	01e77c63          	bgeu	a4,t5,2006150a <__umoddi3+0x282>
200614f6:	9772                	c.add	a4,t3
200614f8:	fff30693          	addi	a3,t1,-1
200614fc:	01c76763          	bltu	a4,t3,2006150a <__umoddi3+0x282>
20061500:	01e77563          	bgeu	a4,t5,2006150a <__umoddi3+0x282>
20061504:	ffe30693          	addi	a3,t1,-2
20061508:	9772                	c.add	a4,t3
2006150a:	41e70733          	sub	a4,a4,t5
2006150e:	03d77333          	remu	t1,a4,t4
20061512:	07c2                	c.slli	a5,0x10
20061514:	83c1                	c.srli	a5,0x10
20061516:	03d75733          	divu	a4,a4,t4
2006151a:	0342                	c.slli	t1,0x10
2006151c:	00f36333          	or	t1,t1,a5
20061520:	02e585b3          	mul	a1,a1,a4
20061524:	87ba                	c.mv	a5,a4
20061526:	00b37c63          	bgeu	t1,a1,2006153e <__umoddi3+0x2b6>
2006152a:	9372                	c.add	t1,t3
2006152c:	fff70793          	addi	a5,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20061530:	01c36763          	bltu	t1,t3,2006153e <__umoddi3+0x2b6>
20061534:	00b37563          	bgeu	t1,a1,2006153e <__umoddi3+0x2b6>
20061538:	ffe70793          	addi	a5,a4,-2
2006153c:	9372                	c.add	t1,t3
2006153e:	06c2                	c.slli	a3,0x10
20061540:	6f41                	c.lui	t5,0x10
20061542:	8edd                	c.or	a3,a5
20061544:	40b305b3          	sub	a1,t1,a1
20061548:	ffff0313          	addi	t1,t5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
2006154c:	0066feb3          	and	t4,a3,t1
20061550:	01065713          	srli	a4,a2,0x10
20061554:	82c1                	c.srli	a3,0x10
20061556:	00667333          	and	t1,a2,t1
2006155a:	026e87b3          	mul	a5,t4,t1
2006155e:	02668333          	mul	t1,a3,t1
20061562:	02ee8eb3          	mul	t4,t4,a4
20061566:	02e686b3          	mul	a3,a3,a4
2006156a:	9e9a                	c.add	t4,t1
2006156c:	0107d713          	srli	a4,a5,0x10
20061570:	9776                	c.add	a4,t4
20061572:	00677363          	bgeu	a4,t1,20061578 <__umoddi3+0x2f0>
20061576:	96fa                	c.add	a3,t5
20061578:	01075313          	srli	t1,a4,0x10
2006157c:	969a                	c.add	a3,t1
2006157e:	6341                	c.lui	t1,0x10
20061580:	137d                	c.addi	t1,-1
20061582:	00677733          	and	a4,a4,t1
20061586:	0742                	c.slli	a4,0x10
20061588:	0067f7b3          	and	a5,a5,t1
2006158c:	97ba                	c.add	a5,a4
2006158e:	00d5e663          	bltu	a1,a3,2006159a <__umoddi3+0x312>
20061592:	00d59b63          	bne	a1,a3,200615a8 <__umoddi3+0x320>
20061596:	00f57963          	bgeu	a0,a5,200615a8 <__umoddi3+0x320>
2006159a:	40c78633          	sub	a2,a5,a2
2006159e:	00c7b7b3          	sltu	a5,a5,a2
200615a2:	97f2                	c.add	a5,t3
200615a4:	8e9d                	c.sub	a3,a5
200615a6:	87b2                	c.mv	a5,a2
200615a8:	40f507b3          	sub	a5,a0,a5
200615ac:	00f53533          	sltu	a0,a0,a5
200615b0:	8d95                	c.sub	a1,a3
200615b2:	8d89                	c.sub	a1,a0
200615b4:	011598b3          	sll	a7,a1,a7
200615b8:	0107d533          	srl	a0,a5,a6
200615bc:	00a8e533          	or	a0,a7,a0
200615c0:	0105d5b3          	srl	a1,a1,a6
200615c4:	bb8d                	c.j	20061336 <__umoddi3+0xae>

200615c6 <atoi>:
200615c6:	4629                	c.li	a2,10
200615c8:	4581                	c.li	a1,0
200615ca:	aef5                	c.j	200619c6 <strtol>

200615cc <_atoi_r>:
200615cc:	46a9                	c.li	a3,10
200615ce:	4601                	c.li	a2,0
200615d0:	a6e5                	c.j	200619b8 <_strtol_r>

200615d2 <__errno>:
200615d2:	ea01a503          	lw	a0,-352(gp) # 2006611c <_impure_ptr>
200615d6:	8082                	c.jr	ra

200615d8 <__libc_init_array>:
200615d8:	1141                	c.addi	sp,-16
200615da:	c422                	c.swsp	s0,8(sp)
200615dc:	c226                	c.swsp	s1,4(sp)
200615de:	20064437          	lui	s0,0x20064
200615e2:	200644b7          	lui	s1,0x20064
200615e6:	6b848793          	addi	a5,s1,1720 # 200646b8 <__frame_dummy_init_array_entry>
200615ea:	6b840413          	addi	s0,s0,1720 # 200646b8 <__frame_dummy_init_array_entry>
200615ee:	8c1d                	c.sub	s0,a5
200615f0:	c04a                	c.swsp	s2,0(sp)
200615f2:	c606                	c.swsp	ra,12(sp)
200615f4:	8409                	c.srai	s0,0x2
200615f6:	6b848493          	addi	s1,s1,1720
200615fa:	4901                	c.li	s2,0
200615fc:	02891763          	bne	s2,s0,2006162a <__libc_init_array+0x52>
20061600:	200644b7          	lui	s1,0x20064
20061604:	20064437          	lui	s0,0x20064
20061608:	6b848793          	addi	a5,s1,1720 # 200646b8 <__frame_dummy_init_array_entry>
2006160c:	6bc40413          	addi	s0,s0,1724 # 200646bc <__do_global_dtors_aux_fini_array_entry>
20061610:	8c1d                	c.sub	s0,a5
20061612:	8409                	c.srai	s0,0x2
20061614:	6b848493          	addi	s1,s1,1720
20061618:	4901                	c.li	s2,0
2006161a:	00891d63          	bne	s2,s0,20061634 <__libc_init_array+0x5c>
2006161e:	40b2                	c.lwsp	ra,12(sp)
20061620:	4422                	c.lwsp	s0,8(sp)
20061622:	4492                	c.lwsp	s1,4(sp)
20061624:	4902                	c.lwsp	s2,0(sp)
20061626:	0141                	c.addi	sp,16
20061628:	8082                	c.jr	ra
2006162a:	409c                	c.lw	a5,0(s1)
2006162c:	0905                	c.addi	s2,1
2006162e:	0491                	c.addi	s1,4
20061630:	9782                	c.jalr	a5
20061632:	b7e9                	c.j	200615fc <__libc_init_array+0x24>
20061634:	409c                	c.lw	a5,0(s1)
20061636:	0905                	c.addi	s2,1
20061638:	0491                	c.addi	s1,4
2006163a:	9782                	c.jalr	a5
2006163c:	bff9                	c.j	2006161a <__libc_init_array+0x42>

2006163e <cleanup_glue>:
2006163e:	1141                	c.addi	sp,-16
20061640:	c422                	c.swsp	s0,8(sp)
20061642:	842e                	c.mv	s0,a1
20061644:	418c                	c.lw	a1,0(a1)
20061646:	c226                	c.swsp	s1,4(sp)
20061648:	c606                	c.swsp	ra,12(sp)
2006164a:	84aa                	c.mv	s1,a0
2006164c:	c191                	c.beqz	a1,20061650 <cleanup_glue+0x12>
2006164e:	3fc5                	c.jal	2006163e <cleanup_glue>
20061650:	85a2                	c.mv	a1,s0
20061652:	4422                	c.lwsp	s0,8(sp)
20061654:	40b2                	c.lwsp	ra,12(sp)
20061656:	8526                	c.mv	a0,s1
20061658:	4492                	c.lwsp	s1,4(sp)
2006165a:	0141                	c.addi	sp,16
2006165c:	aa6f906f          	jal	zero,2005a902 <__wrap__free_r>

20061660 <_reclaim_reent>:
20061660:	ea01a783          	lw	a5,-352(gp) # 2006611c <_impure_ptr>
20061664:	0ea78263          	beq	a5,a0,20061748 <_reclaim_reent+0xe8>
20061668:	515c                	c.lw	a5,36(a0)
2006166a:	1101                	c.addi	sp,-32
2006166c:	cc22                	c.swsp	s0,24(sp)
2006166e:	ce06                	c.swsp	ra,28(sp)
20061670:	ca26                	c.swsp	s1,20(sp)
20061672:	c84a                	c.swsp	s2,16(sp)
20061674:	c64e                	c.swsp	s3,12(sp)
20061676:	842a                	c.mv	s0,a0
20061678:	cf81                	c.beqz	a5,20061690 <_reclaim_reent+0x30>
2006167a:	47dc                	c.lw	a5,12(a5)
2006167c:	4481                	c.li	s1,0
2006167e:	08000913          	addi	s2,zero,128
20061682:	efc9                	c.bnez	a5,2006171c <_reclaim_reent+0xbc>
20061684:	505c                	c.lw	a5,36(s0)
20061686:	438c                	c.lw	a1,0(a5)
20061688:	c581                	c.beqz	a1,20061690 <_reclaim_reent+0x30>
2006168a:	8522                	c.mv	a0,s0
2006168c:	a76f90ef          	jal	ra,2005a902 <__wrap__free_r>
20061690:	484c                	c.lw	a1,20(s0)
20061692:	c581                	c.beqz	a1,2006169a <_reclaim_reent+0x3a>
20061694:	8522                	c.mv	a0,s0
20061696:	a6cf90ef          	jal	ra,2005a902 <__wrap__free_r>
2006169a:	504c                	c.lw	a1,36(s0)
2006169c:	c581                	c.beqz	a1,200616a4 <_reclaim_reent+0x44>
2006169e:	8522                	c.mv	a0,s0
200616a0:	a62f90ef          	jal	ra,2005a902 <__wrap__free_r>
200616a4:	5c0c                	c.lw	a1,56(s0)
200616a6:	c581                	c.beqz	a1,200616ae <_reclaim_reent+0x4e>
200616a8:	8522                	c.mv	a0,s0
200616aa:	a58f90ef          	jal	ra,2005a902 <__wrap__free_r>
200616ae:	5c4c                	c.lw	a1,60(s0)
200616b0:	c581                	c.beqz	a1,200616b8 <_reclaim_reent+0x58>
200616b2:	8522                	c.mv	a0,s0
200616b4:	a4ef90ef          	jal	ra,2005a902 <__wrap__free_r>
200616b8:	402c                	c.lw	a1,64(s0)
200616ba:	c581                	c.beqz	a1,200616c2 <_reclaim_reent+0x62>
200616bc:	8522                	c.mv	a0,s0
200616be:	a44f90ef          	jal	ra,2005a902 <__wrap__free_r>
200616c2:	0ec42583          	lw	a1,236(s0)
200616c6:	c581                	c.beqz	a1,200616ce <_reclaim_reent+0x6e>
200616c8:	8522                	c.mv	a0,s0
200616ca:	a38f90ef          	jal	ra,2005a902 <__wrap__free_r>
200616ce:	0e842583          	lw	a1,232(s0)
200616d2:	c581                	c.beqz	a1,200616da <_reclaim_reent+0x7a>
200616d4:	8522                	c.mv	a0,s0
200616d6:	a2cf90ef          	jal	ra,2005a902 <__wrap__free_r>
200616da:	443c                	c.lw	a5,72(s0)
200616dc:	c799                	c.beqz	a5,200616ea <_reclaim_reent+0x8a>
200616de:	0887a583          	lw	a1,136(a5) # 10088 <mbedtls_ecp_check_pub_priv+0x1aa>
200616e2:	c581                	c.beqz	a1,200616ea <_reclaim_reent+0x8a>
200616e4:	8522                	c.mv	a0,s0
200616e6:	a1cf90ef          	jal	ra,2005a902 <__wrap__free_r>
200616ea:	584c                	c.lw	a1,52(s0)
200616ec:	c581                	c.beqz	a1,200616f4 <_reclaim_reent+0x94>
200616ee:	8522                	c.mv	a0,s0
200616f0:	a12f90ef          	jal	ra,2005a902 <__wrap__free_r>
200616f4:	4c1c                	c.lw	a5,24(s0)
200616f6:	c3b1                	c.beqz	a5,2006173a <_reclaim_reent+0xda>
200616f8:	541c                	c.lw	a5,40(s0)
200616fa:	8522                	c.mv	a0,s0
200616fc:	9782                	c.jalr	a5
200616fe:	0d842583          	lw	a1,216(s0)
20061702:	cd85                	c.beqz	a1,2006173a <_reclaim_reent+0xda>
20061704:	8522                	c.mv	a0,s0
20061706:	4462                	c.lwsp	s0,24(sp)
20061708:	40f2                	c.lwsp	ra,28(sp)
2006170a:	44d2                	c.lwsp	s1,20(sp)
2006170c:	4942                	c.lwsp	s2,16(sp)
2006170e:	49b2                	c.lwsp	s3,12(sp)
20061710:	6105                	c.addi16sp	sp,32
20061712:	b735                	c.j	2006163e <cleanup_glue>
20061714:	95a6                	c.add	a1,s1
20061716:	418c                	c.lw	a1,0(a1)
20061718:	e991                	c.bnez	a1,2006172c <_reclaim_reent+0xcc>
2006171a:	0491                	c.addi	s1,4
2006171c:	505c                	c.lw	a5,36(s0)
2006171e:	47cc                	c.lw	a1,12(a5)
20061720:	ff249ae3          	bne	s1,s2,20061714 <_reclaim_reent+0xb4>
20061724:	8522                	c.mv	a0,s0
20061726:	9dcf90ef          	jal	ra,2005a902 <__wrap__free_r>
2006172a:	bfa9                	c.j	20061684 <_reclaim_reent+0x24>
2006172c:	0005a983          	lw	s3,0(a1)
20061730:	8522                	c.mv	a0,s0
20061732:	9d0f90ef          	jal	ra,2005a902 <__wrap__free_r>
20061736:	85ce                	c.mv	a1,s3
20061738:	b7c5                	c.j	20061718 <_reclaim_reent+0xb8>
2006173a:	40f2                	c.lwsp	ra,28(sp)
2006173c:	4462                	c.lwsp	s0,24(sp)
2006173e:	44d2                	c.lwsp	s1,20(sp)
20061740:	4942                	c.lwsp	s2,16(sp)
20061742:	49b2                	c.lwsp	s3,12(sp)
20061744:	6105                	c.addi16sp	sp,32
20061746:	8082                	c.jr	ra
20061748:	8082                	c.jr	ra

2006174a <strcpy>:
2006174a:	00b56733          	or	a4,a0,a1
2006174e:	8b0d                	c.andi	a4,3
20061750:	87aa                	c.mv	a5,a0
20061752:	e729                	c.bnez	a4,2006179c <strcpy+0x52>
20061754:	7f7f86b7          	lui	a3,0x7f7f8
20061758:	f7f68693          	addi	a3,a3,-129 # 7f7f7f7f <__kr4_psram_text_start__+0x1f677f5f>
2006175c:	587d                	c.li	a6,-1
2006175e:	4190                	c.lw	a2,0(a1)
20061760:	00d67733          	and	a4,a2,a3
20061764:	9736                	c.add	a4,a3
20061766:	8f51                	c.or	a4,a2
20061768:	8f55                	c.or	a4,a3
2006176a:	03070463          	beq	a4,a6,20061792 <strcpy+0x48>
2006176e:	0005c603          	lbu	a2,0(a1)
20061772:	0015c683          	lbu	a3,1(a1)
20061776:	0025c703          	lbu	a4,2(a1)
2006177a:	00c78023          	sb	a2,0(a5)
2006177e:	c615                	c.beqz	a2,200617aa <strcpy+0x60>
20061780:	00d780a3          	sb	a3,1(a5)
20061784:	c29d                	c.beqz	a3,200617aa <strcpy+0x60>
20061786:	00e78123          	sb	a4,2(a5)
2006178a:	c305                	c.beqz	a4,200617aa <strcpy+0x60>
2006178c:	000781a3          	sb	zero,3(a5)
20061790:	8082                	c.jr	ra
20061792:	0791                	c.addi	a5,4
20061794:	0591                	c.addi	a1,4
20061796:	fec7ae23          	sw	a2,-4(a5)
2006179a:	b7d1                	c.j	2006175e <strcpy+0x14>
2006179c:	0005c703          	lbu	a4,0(a1)
200617a0:	0785                	c.addi	a5,1
200617a2:	0585                	c.addi	a1,1
200617a4:	fee78fa3          	sb	a4,-1(a5)
200617a8:	fb75                	c.bnez	a4,2006179c <strcpy+0x52>
200617aa:	8082                	c.jr	ra

200617ac <strncpy>:
200617ac:	00a5e733          	or	a4,a1,a0
200617b0:	8b0d                	c.andi	a4,3
200617b2:	87aa                	c.mv	a5,a0
200617b4:	e701                	c.bnez	a4,200617bc <strncpy+0x10>
200617b6:	470d                	c.li	a4,3
200617b8:	04c76163          	bltu	a4,a2,200617fa <strncpy+0x4e>
200617bc:	ca09                	c.beqz	a2,200617ce <strncpy+0x22>
200617be:	0005c703          	lbu	a4,0(a1)
200617c2:	0585                	c.addi	a1,1
200617c4:	0785                	c.addi	a5,1
200617c6:	fee78fa3          	sb	a4,-1(a5)
200617ca:	167d                	c.addi	a2,-1
200617cc:	fb65                	c.bnez	a4,200617bc <strncpy+0x10>
200617ce:	963e                	c.add	a2,a5
200617d0:	02f61f63          	bne	a2,a5,2006180e <strncpy+0x62>
200617d4:	8082                	c.jr	ra
200617d6:	1671                	c.addi	a2,-4
200617d8:	0107a023          	sw	a6,0(a5)
200617dc:	0791                	c.addi	a5,4
200617de:	0591                	c.addi	a1,4
200617e0:	fcce7ee3          	bgeu	t3,a2,200617bc <strncpy+0x10>
200617e4:	0005a803          	lw	a6,0(a1)
200617e8:	01180733          	add	a4,a6,a7
200617ec:	fff84313          	xori	t1,a6,-1
200617f0:	00677733          	and	a4,a4,t1
200617f4:	8f75                	c.and	a4,a3
200617f6:	d365                	c.beqz	a4,200617d6 <strncpy+0x2a>
200617f8:	b7d1                	c.j	200617bc <strncpy+0x10>
200617fa:	feff08b7          	lui	a7,0xfeff0
200617fe:	808086b7          	lui	a3,0x80808
20061802:	eff88893          	addi	a7,a7,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061806:	08068693          	addi	a3,a3,128 # 80808080 <__ctrace_end__+0xb6808080>
2006180a:	4e0d                	c.li	t3,3
2006180c:	bfe1                	c.j	200617e4 <strncpy+0x38>
2006180e:	0785                	c.addi	a5,1
20061810:	fe078fa3          	sb	zero,-1(a5)
20061814:	bf75                	c.j	200617d0 <strncpy+0x24>

20061816 <strsep>:
20061816:	862a                	c.mv	a2,a0
20061818:	4108                	c.lw	a0,0(a0)
2006181a:	4681                	c.li	a3,0
2006181c:	a009                	c.j	2006181e <__strtok_r>

2006181e <__strtok_r>:
2006181e:	e119                	c.bnez	a0,20061824 <__strtok_r+0x6>
20061820:	4208                	c.lw	a0,0(a2)
20061822:	c939                	c.beqz	a0,20061878 <__strtok_r+0x5a>
20061824:	87aa                	c.mv	a5,a0
20061826:	853e                	c.mv	a0,a5
20061828:	0007c803          	lbu	a6,0(a5)
2006182c:	0785                	c.addi	a5,1
2006182e:	872e                	c.mv	a4,a1
20061830:	00074883          	lbu	a7,0(a4)
20061834:	0705                	c.addi	a4,1
20061836:	00089863          	bne	a7,zero,20061846 <__strtok_r+0x28>
2006183a:	02081063          	bne	a6,zero,2006185a <__strtok_r+0x3c>
2006183e:	00062023          	sw	zero,0(a2)
20061842:	4501                	c.li	a0,0
20061844:	8082                	c.jr	ra
20061846:	ff1815e3          	bne	a6,a7,20061830 <__strtok_r+0x12>
2006184a:	fef1                	c.bnez	a3,20061826 <__strtok_r+0x8>
2006184c:	c21c                	c.sw	a5,0(a2)
2006184e:	00050023          	sb	zero,0(a0)
20061852:	8082                	c.jr	ra
20061854:	4781                	c.li	a5,0
20061856:	a839                	c.j	20061874 <__strtok_r+0x56>
20061858:	e689                	c.bnez	a3,20061862 <__strtok_r+0x44>
2006185a:	0007c803          	lbu	a6,0(a5)
2006185e:	0785                	c.addi	a5,1
20061860:	872e                	c.mv	a4,a1
20061862:	00074683          	lbu	a3,0(a4)
20061866:	0705                	c.addi	a4,1
20061868:	fed818e3          	bne	a6,a3,20061858 <__strtok_r+0x3a>
2006186c:	fe0804e3          	beq	a6,zero,20061854 <__strtok_r+0x36>
20061870:	fe078fa3          	sb	zero,-1(a5)
20061874:	c21c                	c.sw	a5,0(a2)
20061876:	8082                	c.jr	ra
20061878:	8082                	c.jr	ra

2006187a <strtok_r>:
2006187a:	4685                	c.li	a3,1
2006187c:	b74d                	c.j	2006181e <__strtok_r>

2006187e <_strtol_l.constprop.0>:
2006187e:	4785                	c.li	a5,1
20061880:	0af68863          	beq	a3,a5,20061930 <_strtol_l.constprop.0+0xb2>
20061884:	02400793          	addi	a5,zero,36
20061888:	872e                	c.mv	a4,a1
2006188a:	0ad7e363          	bltu	a5,a3,20061930 <_strtol_l.constprop.0+0xb2>
2006188e:	20064837          	lui	a6,0x20064
20061892:	8eaa                	c.mv	t4,a0
20061894:	43d80813          	addi	a6,a6,1085 # 2006443d <_ctype_+0x1>
20061898:	88ba                	c.mv	a7,a4
2006189a:	00074783          	lbu	a5,0(a4)
2006189e:	0705                	c.addi	a4,1
200618a0:	01078533          	add	a0,a5,a6
200618a4:	00054503          	lbu	a0,0(a0)
200618a8:	8921                	c.andi	a0,8
200618aa:	f57d                	c.bnez	a0,20061898 <_strtol_l.constprop.0+0x1a>
200618ac:	02d00513          	addi	a0,zero,45
200618b0:	08a79a63          	bne	a5,a0,20061944 <_strtol_l.constprop.0+0xc6>
200618b4:	00074783          	lbu	a5,0(a4)
200618b8:	4e05                	c.li	t3,1
200618ba:	00288713          	addi	a4,a7,2
200618be:	c6f5                	c.beqz	a3,200619aa <_strtol_l.constprop.0+0x12c>
200618c0:	4541                	c.li	a0,16
200618c2:	02a69263          	bne	a3,a0,200618e6 <_strtol_l.constprop.0+0x68>
200618c6:	03000513          	addi	a0,zero,48
200618ca:	00a79e63          	bne	a5,a0,200618e6 <_strtol_l.constprop.0+0x68>
200618ce:	00074783          	lbu	a5,0(a4)
200618d2:	05800513          	addi	a0,zero,88
200618d6:	0df7f793          	andi	a5,a5,223
200618da:	0ca79363          	bne	a5,a0,200619a0 <_strtol_l.constprop.0+0x122>
200618de:	00174783          	lbu	a5,1(a4)
200618e2:	46c1                	c.li	a3,16
200618e4:	0709                	c.addi	a4,2
200618e6:	800008b7          	lui	a7,0x80000
200618ea:	fff8c893          	xori	a7,a7,-1
200618ee:	98f2                	c.add	a7,t3
200618f0:	02d8f2b3          	remu	t0,a7,a3
200618f4:	4301                	c.li	t1,0
200618f6:	4501                	c.li	a0,0
200618f8:	43a5                	c.li	t2,9
200618fa:	4fe5                	c.li	t6,25
200618fc:	02d8df33          	divu	t5,a7,a3
20061900:	fd078813          	addi	a6,a5,-48
20061904:	0503ea63          	bltu	t2,a6,20061958 <_strtol_l.constprop.0+0xda>
20061908:	87c2                	c.mv	a5,a6
2006190a:	06d7d763          	bge	a5,a3,20061978 <_strtol_l.constprop.0+0xfa>
2006190e:	06034363          	blt	t1,zero,20061974 <_strtol_l.constprop.0+0xf6>
20061912:	537d                	c.li	t1,-1
20061914:	00af6a63          	bltu	t5,a0,20061928 <_strtol_l.constprop.0+0xaa>
20061918:	00af1463          	bne	t5,a0,20061920 <_strtol_l.constprop.0+0xa2>
2006191c:	00f2c663          	blt	t0,a5,20061928 <_strtol_l.constprop.0+0xaa>
20061920:	4305                	c.li	t1,1
20061922:	02a68533          	mul	a0,a3,a0
20061926:	953e                	c.add	a0,a5
20061928:	0705                	c.addi	a4,1
2006192a:	fff74783          	lbu	a5,-1(a4)
2006192e:	bfc9                	c.j	20061900 <_strtol_l.constprop.0+0x82>
20061930:	1141                	c.addi	sp,-16
20061932:	c606                	c.swsp	ra,12(sp)
20061934:	c9fff0ef          	jal	ra,200615d2 <__errno>
20061938:	40b2                	c.lwsp	ra,12(sp)
2006193a:	47d9                	c.li	a5,22
2006193c:	c11c                	c.sw	a5,0(a0)
2006193e:	4501                	c.li	a0,0
20061940:	0141                	c.addi	sp,16
20061942:	8082                	c.jr	ra
20061944:	02b00513          	addi	a0,zero,43
20061948:	4e01                	c.li	t3,0
2006194a:	f6a79ae3          	bne	a5,a0,200618be <_strtol_l.constprop.0+0x40>
2006194e:	00074783          	lbu	a5,0(a4)
20061952:	00288713          	addi	a4,a7,2 # 80000002 <__ctrace_end__+0xb6000002>
20061956:	b7a5                	c.j	200618be <_strtol_l.constprop.0+0x40>
20061958:	fbf78813          	addi	a6,a5,-65
2006195c:	010fe563          	bltu	t6,a6,20061966 <_strtol_l.constprop.0+0xe8>
20061960:	fc978793          	addi	a5,a5,-55
20061964:	b75d                	c.j	2006190a <_strtol_l.constprop.0+0x8c>
20061966:	f9f78813          	addi	a6,a5,-97
2006196a:	010fe763          	bltu	t6,a6,20061978 <_strtol_l.constprop.0+0xfa>
2006196e:	fa978793          	addi	a5,a5,-87
20061972:	bf61                	c.j	2006190a <_strtol_l.constprop.0+0x8c>
20061974:	537d                	c.li	t1,-1
20061976:	bf4d                	c.j	20061928 <_strtol_l.constprop.0+0xaa>
20061978:	00035963          	bge	t1,zero,2006198a <_strtol_l.constprop.0+0x10c>
2006197c:	02200793          	addi	a5,zero,34
20061980:	00fea023          	sw	a5,0(t4)
20061984:	8546                	c.mv	a0,a7
20061986:	ea09                	c.bnez	a2,20061998 <_strtol_l.constprop.0+0x11a>
20061988:	8082                	c.jr	ra
2006198a:	000e0463          	beq	t3,zero,20061992 <_strtol_l.constprop.0+0x114>
2006198e:	40a00533          	sub	a0,zero,a0
20061992:	c215                	c.beqz	a2,200619b6 <_strtol_l.constprop.0+0x138>
20061994:	00030463          	beq	t1,zero,2006199c <_strtol_l.constprop.0+0x11e>
20061998:	fff70593          	addi	a1,a4,-1
2006199c:	c20c                	c.sw	a1,0(a2)
2006199e:	8082                	c.jr	ra
200619a0:	03000793          	addi	a5,zero,48
200619a4:	f2a9                	c.bnez	a3,200618e6 <_strtol_l.constprop.0+0x68>
200619a6:	46a1                	c.li	a3,8
200619a8:	bf3d                	c.j	200618e6 <_strtol_l.constprop.0+0x68>
200619aa:	03000513          	addi	a0,zero,48
200619ae:	f2a780e3          	beq	a5,a0,200618ce <_strtol_l.constprop.0+0x50>
200619b2:	46a9                	c.li	a3,10
200619b4:	bf0d                	c.j	200618e6 <_strtol_l.constprop.0+0x68>
200619b6:	8082                	c.jr	ra

200619b8 <_strtol_r>:
200619b8:	b5d9                	c.j	2006187e <_strtol_l.constprop.0>

200619ba <strtol_l>:
200619ba:	86b2                	c.mv	a3,a2
200619bc:	862e                	c.mv	a2,a1
200619be:	85aa                	c.mv	a1,a0
200619c0:	ea01a503          	lw	a0,-352(gp) # 2006611c <_impure_ptr>
200619c4:	bd6d                	c.j	2006187e <_strtol_l.constprop.0>

200619c6 <strtol>:
200619c6:	86b2                	c.mv	a3,a2
200619c8:	862e                	c.mv	a2,a1
200619ca:	85aa                	c.mv	a1,a0
200619cc:	ea01a503          	lw	a0,-352(gp) # 2006611c <_impure_ptr>
200619d0:	b57d                	c.j	2006187e <_strtol_l.constprop.0>
200619d2:	0000                	c.unimp
200619d4:	6d6f682f          	0x6d6f682f
200619d8:	2f65                	c.jal	20062190 <pmap_func+0x3a0>
200619da:	7561                	c.lui	a0,0xffff8
200619dc:	6f74                	c.flw	fa3,92(a4)
200619de:	6a5f 6e65 696b      	0x696b6e656a5f
200619e4:	736e                	c.flwsp	ft6,248(sp)
200619e6:	775f 616c 356e      	0x356e616c775f
200619ec:	544f492f          	0x544f492f
200619f0:	415f 5354 722f      	0x722f5354415f
200619f6:	7065                	c.lui	zero,0xffff9
200619f8:	722f736f          	jal	t1,2015911a <__ap_sram_heap_start+0xd911a>
200619fc:	6c65                	c.lui	s8,0x19
200619fe:	6165                	c.addi16sp	sp,112
20061a00:	6d2d6573          	csrrsi	a0,0x6d2,26
20061a04:	7361                	c.lui	t1,0xffff8
20061a06:	6574                	c.flw	fa3,76(a0)
20061a08:	2f72                	c.fldsp	ft10,280(sp)
20061a0a:	72756f73          	csrrsi	t5,0x727,10
20061a0e:	612f6563          	bltu	t5,s2,20062018 <pmap_func+0x228>
20061a12:	656d                	c.lui	a0,0x1b
20061a14:	6162                	c.flwsp	ft2,24(sp)
20061a16:	696c                	c.flw	fa1,84(a0)
20061a18:	6574                	c.flw	fa3,76(a0)
20061a1a:	675f 6363 705f      	0x705f6363675f
20061a20:	6f72                	c.flwsp	ft10,28(sp)
20061a22:	656a                	c.flwsp	fa0,152(sp)
20061a24:	702f7463          	bgeu	t5,sp,2006212c <pmap_func+0x33c>
20061a28:	6f72                	c.flwsp	ft10,28(sp)
20061a2a:	656a                	c.flwsp	fa0,152(sp)
20061a2c:	6b5f7463          	bgeu	t5,s5,200620d4 <pmap_func+0x2e4>
20061a30:	3472                	c.fldsp	fs0,312(sp)
20061a32:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
20061a36:	2e2e2f6b          	0x2e2e2f6b
20061a3a:	2f2e2e2f          	0x2f2e2e2f
20061a3e:	2e2e                	c.fldsp	ft8,200(sp)
20061a40:	6d6f632f          	0x6d6f632f
20061a44:	6f70                	c.flw	fa2,92(a4)
20061a46:	656e                	c.flwsp	fa0,216(sp)
20061a48:	746e                	c.flwsp	fs0,248(sp)
20061a4a:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
20061a4e:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
20061a52:	6162                	c.flwsp	ft2,24(sp)
20061a54:	696c                	c.flw	fa1,84(a0)
20061a56:	6574                	c.flw	fa3,76(a0)
20061a58:	6c77662f          	0x6c77662f
20061a5c:	6269                	c.lui	tp,0x1a
20061a5e:	6d61722f          	0x6d61722f
20061a62:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20061a68:	2f6e                	c.fldsp	ft10,216(sp)
20061a6a:	6d61                	c.lui	s10,0x18
20061a6c:	6265                	c.lui	tp,0x19
20061a6e:	5f61                	c.li	t5,-8
20061a70:	6c66                	c.flwsp	fs8,88(sp)
20061a72:	7361                	c.lui	t1,0xffff8
20061a74:	5f68                	c.lw	a0,124(a4)
20061a76:	6172                	c.flwsp	ft2,28(sp)
20061a78:	2e6d                	c.jal	20061e32 <pmap_func+0x42>
20061a7a:	68430063          	beq	t1,tp,200620fa <pmap_func+0x30a>
20061a7e:	6e61                	c.lui	t3,0x18
20061a80:	656e                	c.flwsp	fa0,216(sp)
20061a82:	206c                	c.fld	fa1,192(s0)
20061a84:	666e6f43          	fmadd.q	ft10,ft8,ft6,fa2,unknown
20061a88:	696c                	c.flw	fa1,84(a0)
20061a8a:	66207463          	bgeu	zero,sp,200620f2 <pmap_func+0x302>
20061a8e:	4320726f          	jal	tp,20068ec0 <__bdram_heap_buffer_start__+0xc80>
20061a92:	5550                	c.lw	a2,44(a0)
20061a94:	2520                	c.fld	fs0,72(a0)
20061a96:	756c                	c.flw	fa1,108(a0)
20061a98:	4320                	c.lw	s0,64(a4)
20061a9a:	6168                	c.flw	fa0,68(a0)
20061a9c:	6e6e                	c.flwsp	ft8,216(sp)
20061a9e:	6c65                	c.lui	s8,0x19
20061aa0:	2520                	c.fld	fs0,72(a0)
20061aa2:	756c                	c.flw	fa1,108(a0)
20061aa4:	2120                	c.fld	fs0,64(a0)
20061aa6:	4920                	c.lw	s0,80(a0)
20061aa8:	726f6e67          	0x726f6e67
20061aac:	2065                	c.jal	20061b54 <strtol+0x18e>
20061aae:	6649                	c.lui	a2,0x12
20061ab0:	4320                	c.lw	s0,64(a4)
20061ab2:	5550                	c.lw	a2,44(a0)
20061ab4:	4820                	c.lw	s0,80(s0)
20061ab6:	7361                	c.lui	t1,0xffff8
20061ab8:	5220                	c.lw	s0,96(a2)
20061aba:	7365                	c.lui	t1,0xffff9
20061abc:	7465                	c.lui	s0,0xffff9
20061abe:	0a0d                	c.addi	s4,3
20061ac0:	0000                	c.unimp
20061ac2:	0000                	c.unimp
20061ac4:	5049                	c.li	zero,-14
20061ac6:	49200043          	fmadd.s	ft0,ft0,fs2,fs1,rne
20061aca:	4350                	c.lw	a2,4(a4)
20061acc:	5220                	c.lw	s0,96(a2)
20061ace:	7165                	c.addi16sp	sp,-400
20061ad0:	6575                	c.lui	a0,0x1d
20061ad2:	54207473          	csrrci	s0,0x542,0
20061ad6:	6d69                	c.lui	s10,0x1a
20061ad8:	6f65                	c.lui	t5,0x19
20061ada:	7475                	c.lui	s0,0xffffd
20061adc:	0a0d                	c.addi	s4,3
20061ade:	0000                	c.unimp
20061ae0:	4920                	c.lw	s0,80(a0)
20061ae2:	4350                	c.lw	a2,4(a4)
20061ae4:	4720                	c.lw	s0,72(a4)
20061ae6:	7465                	c.lui	s0,0xffff9
20061ae8:	5320                	c.lw	s0,96(a4)
20061aea:	6d65                	c.lui	s10,0x19
20061aec:	7061                	c.lui	zero,0xffff8
20061aee:	6f68                	c.flw	fa0,92(a4)
20061af0:	6572                	c.flwsp	fa0,28(sp)
20061af2:	5420                	c.lw	s0,104(s0)
20061af4:	6d69                	c.lui	s10,0x1a
20061af6:	6f65                	c.lui	t5,0x19
20061af8:	7475                	c.lui	s0,0xffffd
20061afa:	0a0d                	c.addi	s4,3
20061afc:	0000                	c.unimp
20061afe:	0000                	c.unimp
20061b00:	6d6f682f          	0x6d6f682f
20061b04:	2f65                	c.jal	200622bc <pmap_func+0x4cc>
20061b06:	7561                	c.lui	a0,0xffff8
20061b08:	6f74                	c.flw	fa3,92(a4)
20061b0a:	6a5f 6e65 696b      	0x696b6e656a5f
20061b10:	736e                	c.flwsp	ft6,248(sp)
20061b12:	775f 616c 356e      	0x356e616c775f
20061b18:	544f492f          	0x544f492f
20061b1c:	415f 5354 722f      	0x722f5354415f
20061b22:	7065                	c.lui	zero,0xffff9
20061b24:	722f736f          	jal	t1,20159246 <__ap_sram_heap_start+0xd9246>
20061b28:	6c65                	c.lui	s8,0x19
20061b2a:	6165                	c.addi16sp	sp,112
20061b2c:	6d2d6573          	csrrsi	a0,0x6d2,26
20061b30:	7361                	c.lui	t1,0xffff8
20061b32:	6574                	c.flw	fa3,76(a0)
20061b34:	2f72                	c.fldsp	ft10,280(sp)
20061b36:	72756f73          	csrrsi	t5,0x727,10
20061b3a:	612f6563          	bltu	t5,s2,20062144 <pmap_func+0x354>
20061b3e:	656d                	c.lui	a0,0x1b
20061b40:	6162                	c.flwsp	ft2,24(sp)
20061b42:	696c                	c.flw	fa1,84(a0)
20061b44:	6574                	c.flw	fa3,76(a0)
20061b46:	675f 6363 705f      	0x705f6363675f
20061b4c:	6f72                	c.flwsp	ft10,28(sp)
20061b4e:	656a                	c.flwsp	fa0,152(sp)
20061b50:	702f7463          	bgeu	t5,sp,20062258 <pmap_func+0x468>
20061b54:	6f72                	c.flwsp	ft10,28(sp)
20061b56:	656a                	c.flwsp	fa0,152(sp)
20061b58:	6b5f7463          	bgeu	t5,s5,20062200 <pmap_func+0x410>
20061b5c:	3472                	c.fldsp	fs0,312(sp)
20061b5e:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
20061b62:	2e2e2f6b          	0x2e2e2f6b
20061b66:	2f2e2e2f          	0x2f2e2e2f
20061b6a:	2e2e                	c.fldsp	ft8,200(sp)
20061b6c:	6d6f632f          	0x6d6f632f
20061b70:	6f70                	c.flw	fa2,92(a4)
20061b72:	656e                	c.flwsp	fa0,216(sp)
20061b74:	746e                	c.flwsp	fs0,248(sp)
20061b76:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
20061b7a:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
20061b7e:	6162                	c.flwsp	ft2,24(sp)
20061b80:	696c                	c.flw	fa1,84(a0)
20061b82:	6574                	c.flw	fa3,76(a0)
20061b84:	6c77662f          	0x6c77662f
20061b88:	6269                	c.lui	tp,0x1a
20061b8a:	6d61722f          	0x6d61722f
20061b8e:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20061b94:	2f6e                	c.fldsp	ft10,216(sp)
20061b96:	6d61                	c.lui	s10,0x18
20061b98:	6265                	c.lui	tp,0x19
20061b9a:	5f61                	c.li	t5,-8
20061b9c:	7069                	c.lui	zero,0xffffa
20061b9e:	70615f63          	bge	sp,t1,200622bc <pmap_func+0x4cc>
20061ba2:	2e69                	c.jal	20061f3c <pmap_func+0x14c>
20061ba4:	00000063          	beq	zero,zero,20061ba4 <strtol+0x1de>
20061ba8:	5049                	c.li	zero,-14
20061baa:	69445f43          	fmadd.s	ft10,fs0,fs4,fa3,unknown
20061bae:	3a72                	c.fldsp	fs4,312(sp)
20061bb0:	6425                	c.lui	s0,0x9
20061bb2:	202c                	c.fld	fa1,64(s0)
20061bb4:	5049                	c.li	zero,-14
20061bb6:	58545f43          	fmadd.s	ft10,fs0,ft5,fa1,unknown
20061bba:	445f 5441 2041      	0x20415441445f
20061bc0:	7830                	c.flw	fa2,112(s0)
20061bc2:	7825                	c.lui	a6,0xfffe9
20061bc4:	4e20                	c.lw	s0,88(a2)
20061bc6:	4320746f          	jal	s0,20068ff8 <__bdram_heap_buffer_start__+0xdb8>
20061bca:	656c                	c.flw	fa1,76(a0)
20061bcc:	6e61                	c.lui	t3,0x18
20061bce:	0d21                	c.addi	s10,8
20061bd0:	000a                	c.slli	zero,0x2
	...

20061bd4 <CSWTCH.29>:
20061bd4:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20061be4:	0008 0000 0000 0000 0000 0000 0000 0000     ................
20061bf4:	0008 0000                                   ....

20061bf8 <CSWTCH.30>:
20061bf8:	0000 4102 0000 4102 0000 0000 0000 0000     ...A...A........
	...
20061c38:	0080 4102 0080 4102 0000 0000 0000 0000     ...A...A........
	...
20061c78:	0100 4102 0100 4102 682f 6d6f 2f65 7561     ...A...A/home/au
20061c88:	6f74 6a5f 6e65 696b 736e 775f 616c 356e     to_jenkins_wlan5
20061c98:	492f 544f 415f 5354 722f 7065 736f 722f     /IOT_ATS/repos/r
20061ca8:	6c65 6165 6573 6d2d 7361 6574 2f72 6f73     elease-master/so
20061cb8:	7275 6563 612f 656d 6162 696c 6574 675f     urce/amebalite_g
20061cc8:	6363 705f 6f72 656a 7463 702f 6f72 656a     cc_project/proje
20061cd8:	7463 6b5f 3472 762f 6473 2f6b 2e2e 2e2f     ct_kr4/vsdk/../.
20061ce8:	2f2e 2e2e 632f 6d6f 6f70 656e 746e 732f     ./../component/s
20061cf8:	636f 612f 656d 6162 696c 6574 662f 6c77     oc/amebalite/fwl
20061d08:	6269 722f 6d61 635f 6d6f 6f6d 2f6e 6d61     ib/ram_common/am
20061d18:	6265 5f61 7069 5f63 6172 2e6d 0063 0000     eba_ipc_ram.c...
20061d28:	5043 2055 6425 7420 6b61 2065 6573 616d     CPU %d take sema
20061d38:	2520 2064 6f66 2072 7870 6469 2520 2064      %d for pxid %d 
20061d48:	6166 6c69 203a 7063 6975 2064 6425 7020     fail: cpuid %d p
20061d58:	6978 2064 6425 6820 7361 7420 6b61 6e65     xid %d has taken
20061d68:	7420 6568 7320 6d65 0a61 0000 5043 2055      the sema...CPU 
20061d78:	6425 6620 6572 2065 6573 616d 2520 2064     %d free sema %d 
20061d88:	6f66 2072 7870 6469 2520 2064 6166 6c69     for pxid %d fail
20061d98:	203a 7063 6975 2064 6425 202c 7870 6469     : cpuid %d, pxid
20061da8:	2520 2064 7369 7420 6b61 6e69 2067 6874      %d is taking th
20061db8:	2065 6573 616d 000a 544f 2050 6572 6461     e sema..OTP read
20061dc8:	6720 7465 6820 2077 6573 616d 6620 6961      get hw sema fai
20061dd8:	0a6c 0000 544f 5f50 6552 6461 2038 6146     l...OTP_Read8 Fa
20061de8:	6c69 2520 2078 000a                         il %x ..

20061df0 <pmap_func>:
20061df0:	0000 0101 0001 0101 0002 0102 0003 0102     ................
20061e00:	0004 0202 0005 0102 0006 0202 0007 0102     ................
20061e10:	0008 0101 0009 0101 000a 0101 000b 0101     ................
20061e20:	000c 0101 000d 0101 000e 0101 000f 0101     ................
20061e30:	0010 0101 0011 0101 0012 0000 0013 0202     ................
20061e40:	0014 0202 0015 0101 0016 0000 0017 0101     ................
20061e50:	0018 0101 0019 0101 001a 0101 001b 0101     ................
20061e60:	001c 0101 001d 0101 001e 0101 001f 0101     ................
20061e70:	0020 0101 0021 0101 0022 0101 0023 0101      ...!..."...#...
20061e80:	0024 0101 0025 0101 0026 0101 0027 0101     $...%...&...'...
20061e90:	0028 0101 0029 0101 002a 0101 002b 0100     (...)...*...+...
20061ea0:	002c 0100 002d 0100 002e 0100 002f 0100     ,...-......./...
20061eb0:	0030 0100 0031 0100 0032 0100 0033 0100     0...1...2...3...
20061ec0:	ffff ffff 766e 6369 695f 7172 2520 756c     ....nvic_irq %lu
20061ed0:	4620 6e75 4e20 746f 4120 7373 6769 216e      Fun Not Assign!
20061ee0:	2121 2121 000a 0000 4c50 4349 0000 0000     !!!!....PLIC....
20061ef0:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20061f00:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20061f10:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20061f20:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20061f30:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20061f40:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20061f50:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20061f60:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
20061f70:	696c 6574 6d2f 7369 2f63 6d61 6265 5f61     lite/misc/ameba_
20061f80:	6d70 2e75 0063 0000 6e49 6176 696c 2064     pmu.c...Invalid 
20061f90:	7350 6172 436d 6b6c 000a 0000 5350 4152     PsramClk....PSRA
20061fa0:	004d 0000 7370 6172 206d 7974 6570 6920     M...psram type i
20061fb0:	2073 7830 7825 000a 5353 4549 524e 3009     s 0x%x..SSIENR.0
20061fc0:	2578 3d78 7830 7825 000a 0000 5443 4c52     x%x=0x%x....CTRL
20061fd0:	3052 3009 2578 3d78 7830 7825 000a 0000     R0.0x%x=0x%x....
20061fe0:	4142 4455 0952 7830 7825 303d 2578 0a78     BAUDR.0x%x=0x%x.
20061ff0:	0000 0000 4156 494c 5f44 4d43 0944 7830     ....VALID_CMD.0x
20062000:	7825 303d 2578 0a78 0000 0000 5443 4c52     %x=0x%x.....CTRL
20062010:	3252 3009 2578 3d78 7830 7825 000a 0000     R2.0x%x=0x%x....
20062020:	4544 4956 4543 495f 464e 094f 7830 7825     DEVICE_INFO.0x%x
20062030:	303d 2578 0a78 0000 5054 3052 3009 2578     =0x%x...TPR0.0x%
20062040:	3d78 7830 7825 000a 4552 4441 465f 5341     x=0x%x..READ_FAS
20062050:	5f54 4953 474e 454c 3009 2578 3d78 7830     T_SINGLE.0x%x=0x
20062060:	7825 000a 5257 5449 5f45 4953 4e47 454c     %x..WRITE_SIGNLE
20062070:	3009 2578 3d78 7830 7825 000a 5541 4f54     .0x%x=0x%x..AUTO
20062080:	4c5f 4e45 5447 0948 7830 7825 303d 2578     _LENGTH.0x%x=0x%
20062090:	0a78 0000 5355 5245 4c5f 4e45 5447 0948     x...USER_LENGTH.
200620a0:	7830 7825 303d 2578 0a78 0000 5541 4f54     0x%x=0x%x...AUTO
200620b0:	4c5f 4e45 5447 3248 3009 2578 3d78 7830     _LENGTH2.0x%x=0x
200620c0:	7825 000a 5253 3009 2578 3d78 7830 7825     %x..SR.0x%x=0x%x
200620d0:	000a 0000 5350 4850 5f59 4143 5f4c 4150     ....PSPHY_CAL_PA
200620e0:	0952 7830 7825 303d 2578 0a78 0000 0000     R.0x%x=0x%x.....
200620f0:	5350 4850 5f59 4850 5f59 5443 4c52 3009     PSPHY_PHY_CTRL.0
20062100:	2578 3d78 7830 7825 000a 0000 5350 4850     x%x=0x%x....PSPH
20062110:	5f59 4244 5f47 4553 094c 7830 7825 303d     Y_DBG_SEL.0x%x=0
20062120:	2578 0a78 0000 0000 5249 5b30 5d31 303d     x%x.....IR0[1]=0
20062130:	2578 0a78 0000 0000 5249 5b30 5d30 303d     x%x.....IR0[0]=0
20062140:	2578 0a78 0000 0000 5243 5b30 5d31 303d     x%x.....CR0[1]=0
20062150:	2578 0a78 0000 0000 5243 5b30 5d30 303d     x%x.....CR0[0]=0
20062160:	2578 0a78 0000 0000 5243 5b31 5d31 303d     x%x.....CR1[1]=0
20062170:	2578 0a78 0000 0000 5243 5b31 5d30 303d     x%x.....CR1[0]=0
20062180:	2578 0a78 0000 0000 3d3d 703d 6168 6573     x%x.....===phase
20062190:	2520 786c 3d20 3d3d 3d3d 000a 6b6f 2520      %lx =====..ok %
200621a0:	786c 2520 786c 2520 786c 2520 786c 2520     lx %lx %lx %lx %
200621b0:	786c 2520 786c 2520 786c 000a 6166 6c69     lx %lx %lx..fail
200621c0:	2520 786c 2520 786c 2520 786c 2520 786c      %lx %lx %lx %lx
200621d0:	2520 786c 2520 786c 2520 786c 000a 0000      %lx %lx %lx....
200621e0:	6143 4e6c 696d 206e 203d 7825 4320 6c61     CalNmin = %x Cal
200621f0:	6d4e 7861 3d20 2520 2078 6957 646e 776f     Nmax = %x Window
20062200:	6953 657a 3d20 2520 2078 6870 7361 3a65     Size = %x phase:
20062210:	2520 2078 000a 0000 542b 5345 3a54 0000      %x ....+TEST:..
20062220:	0a0d 7325 4b4f 0a0d 0000 0000 4f2b 4154     ..%sOK......+OTA
20062230:	4c43 4145 3a52 0000 4f2b 4154 4552 4f43     CLEAR:..+OTARECO
20062240:	4556 3a52 0000 0000 522b 5453 003a 0000     VER:....+RST:...
20062250:	6154 6b73 4c20 7369 3a74 0d20 250a 0d73     Task List: ..%s.
20062260:	000a 0000 6548 7061 7453 7461 3a73 0d20     ....HeapStats: .
20062270:	000a 0000 6f54 6174 206c 6548 7061 093a     ....Total Heap:.
20062280:	7525 0a0d 0000 0000 6548 7061 4620 6572     %u......Heap Fre
20062290:	2065 6f4e 3a77 2509 0d75 000a 6548 7061     e Now:.%u...Heap
200622a0:	5520 6573 2064 6f4e 3a77 2509 0d75 000a      Used Now:.%u...
200622b0:	6548 7061 5520 6573 2064 614d 3a78 2509     Heap Used Max:.%
200622c0:	0d75 000a 532b 4154 4554 003a 7325 0000     u...+STATE:.%s..
200622d0:	3056 2e32 2e31 0031 3376 352e 0000 0000     V02.1.1.v3.5....
200622e0:	3032 3432 302f 2f36 3131 312d 3a34 3933     2024/06/11-14:39
200622f0:	323a 0036 472b 524d 003a 0000 0a0d 7325     :26.+GMR:.....%s
20062300:	7325 252c 2c73 7325 0a0d 0000 4c5b 474f     %s,%s,%s....[LOG
20062310:	205d 5245 4f52 2052 7261 3a67 0d20 000a     ] ERROR arg: ...
20062320:	0023 0000 4c5b 474f 205d 6e49 6176 696c     #...[LOG] Invali
20062330:	2064 6567 2074 6170 6172 656d 6574 7372     d get parameters
20062340:	0d2e 000a 253c 3e73 6c20 7665 6c65 6920     ....<%s> level i
20062350:	2073 6425 0a0d 0000 4c5b 474f 205d 6e49     s %d....[LOG] In
20062360:	6176 696c 2064 6573 2074 6170 6172 656d     valid set parame
20062370:	6574 7372 0d2e 000a 4c5b 474f 205d 6e49     ters....[LOG] In
20062380:	6176 696c 2064 6f6c 2067 656c 6576 206c     valid log level 
20062390:	6425 0d2e 000a 0000 4c5b 474f 205d 6e49     %d......[LOG] In
200623a0:	6176 696c 2064 6170 6172 656d 6574 2072     valid parameter 
200623b0:	756e 626d 7265 0d2e 000a 0000 4c5b 474f     number......[LOG
200623c0:	205d 6e49 6176 696c 2064 6567 5f74 6573     ] Invalid get_se
200623d0:	2074 6425 0d2e 000a 4c5b 474f 205d 6554     t %d....[LOG] Te
200623e0:	7473 6120 776c 7961 2073 656c 6576 0d6c     st always level.
200623f0:	000a 0000 4c5b 474f 205d 6554 7473 6520     ....[LOG] Test e
20062400:	7272 726f 6c20 7665 6c65 0a0d 0000 0000     rror level......
20062410:	4c5b 474f 205d 6554 7473 7720 7261 696e     [LOG] Test warni
20062420:	676e 6c20 7665 6c65 0a0d 0000 4c5b 474f     ng level....[LOG
20062430:	205d 6554 7473 6920 666e 206f 656c 6576     ] Test info leve
20062440:	0d6c 000a 4c5b 474f 205d 6554 7473 4c20     l...[LOG] Test L
20062450:	474f 495f 4554 534d 0a0d 0000 4c2b 474f     OG_ITEMS....+LOG
20062460:	003a 0000 0a0d 7325 5245 4f52 3a52 6425     :.....%sERROR:%d
20062470:	0a0d 0000 5441 4c2b 474f 3c3d 6567 5f74     ....AT+LOG=<get_
20062480:	6573 3e74 3c2c 6f6d 7564 656c 5b3e 3c2c     set>,<module>[,<
20062490:	6f6c 5f67 656c 6576 3e6c 0d5d 000a 0000     log_level>].....
200624a0:	3c09 6567 5f74 6573 3e74 093a 2d30 6567     .<get_set>:.0-ge
200624b0:	2c74 3120 732d 7465 202c 2d32 7270 6e69     t, 1-set, 2-prin
200624c0:	2074 6c61 2c6c 3320 632d 656c 7261 6120     t all, 3-clear a
200624d0:	6c6c 0a0d 0000 0000 3c09 6f6d 7564 656c     ll.......<module
200624e0:	3a3e 2a09 652d 6361 2068 6f6d 7564 656c     >:.*-each module
200624f0:	202c 746f 6568 7372 732d 6570 6963 6966     , others-specifi
20062500:	2063 6f6d 7564 656c 0a0d 0000 3c09 6f6c     c module.....<lo
20062510:	5f67 656c 6576 3e6c 093a 305b 352c 2c5d     g_level>:.[0,5],
20062520:	6f20 6c6e 2079 7061 6c70 6369 6261 656c      only applicable
20062530:	6620 726f 7320 7465 6d20 646f 0d65 000a      for set mode...
20062540:	525b 4552 5d47 4520 7272 726f 7020 7261     [RREG] Error par
20062550:	6d61 7465 7265 0d73 000a 0000 522b 4552     ameters.....+RRE
20062560:	3a47 0000 575b 4552 5d47 4520 7272 726f     G:..[WREG] Error
20062570:	7020 7261 6d61 7465 7265 0d73 000a 0000      parameters.....
20062580:	572b 4552 3a47 0000 5043 2555 0000 0000     +WREG:..CPU%....
20062590:	7270 6f69 0000 0000 7473 7461 0065 0000     prio....state...
200625a0:	6174 6b73 6e20 6d61 0065 0000 7325 2509     task name...%s.%
200625b0:	0973 7325 2509 0a73 000d 0000 7325 2509     s.%s.%s.....%s.%
200625c0:	0963 6425 2509 2e64 7325 0a0d 0000 0000     c.%d.%d.%s......
200625d0:	7325 6d20 6c61 6f6c 2063 6166 6c69 6465     %s malloc failed
200625e0:	0a0d 0000 745b 706f 555d 6173 6567 203a     ....[top]Usage: 
200625f0:	6f74 3d70 6f6d 6564 742c 6d69 2c65 6f63     top=mode,time,co
20062600:	6e75 0a74 200d 6f6d 6564 203a 2c30 7320     unt.. mode: 0, s
20062610:	6174 7472 6320 756f 746e 6320 7570 7520     tart count cpu u
20062620:	6173 6567 6520 6576 7972 5b20 6974 656d     sage every [time
20062630:	205d 6573 6f63 646e 0d2e 200a 6f6d 6564     ] second... mode
20062640:	203a 2c31 7320 6f74 2070 6f6d 6564 3020     : 1, stop mode 0
20062650:	0d2e 200a 6f6d 6564 203a 3a32 7320 6174     ... mode: 2: sta
20062660:	7472 6320 756f 746e 6320 7570 7520 6173     rt count cpu usa
20062670:	6567 0d2e 200a 6f6d 6564 203a 3a33 7320     ge... mode: 3: s
20062680:	6f74 2070 6f6d 6564 3220 0d2e 200a 6974     top mode 2... ti
20062690:	656d 203a 5043 2055 7473 7461 7369 6974     me: CPU statisti
200626a0:	7363 6920 746e 7265 6176 2e6c 6544 6166     cs interval.Defa
200626b0:	6c75 2074 2e31 7520 696e 2874 2973 0d20     ult 1. unit(s) .
200626c0:	200a 6f63 6e75 3a74 4320 5550 7320 6174     . count: CPU sta
200626d0:	6974 7473 6369 2073 6f63 6e75 2c74 6420     tistics count, d
200626e0:	6665 7561 746c 7520 746e 6c69 7320 6f74     efault until sto
200626f0:	2070 726f 3120 322c 332c 2e2e 002e 0000     p or 1,2,3......
20062700:	654d 6f6d 7972 6620 6961 756c 6572 6620     Memory failure f
20062710:	726f 7420 7361 5f6b 7473 7461 7375 0a0d     or task_status..
20062720:	0000 0000 7563 7272 6e65 2074 6f6d 6564     ....current mode
20062730:	203a 6425 7420 6d69 3a65 2520 2864 2973     : %d time: %d(s)
20062740:	6320 756f 746e 203a 6425 2820 312d 6620      count: %d (-1 f
20062750:	726f 6920 666e 6e69 7469 2965 0a0d 0000     or infinite)....
20062760:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062770:	432b 5550 4f4c 4441 003a 0000 5441 432b     +CPULOAD:...AT+C
20062780:	5550 4f4c 4441 3c3d 6f6d 6564 5b3e 3c2c     PULOAD=<mode>[,<
20062790:	6974 656d 695f 746e 6176 3e6c 3c2c 6f63     time_intval>,<co
200627a0:	6e75 6574 3e72 0d5d 000a 0000 3c09 6f6d     unter>]......<mo
200627b0:	6564 3a3e 5b09 2c30 5d32 0a0d 0000 0000     de>:.[0,2]......
200627c0:	3c09 6974 656d 695f 746e 6176 3e6c 093a     .<time_intval>:.
200627d0:	6e49 7320 6365 0a0d 0000 0000 3c09 6f63     In sec.......<co
200627e0:	6e75 6574 3e72 3009 6d20 6165 736e 6920     unter>.0 means i
200627f0:	666e 6e69 7469 0a0d 0000 0000 5441 7325     nfinit......AT%s
20062800:	0a0d 0000 6f43 6d6d 6e6f 4120 2054 6f43     ....Common AT Co
20062810:	6d6d 6e61 3a64 0a0d 0000 0000 4c2b 5349     mmand:......+LIS
20062820:	3a54 0000 4f2b 4154 4c43 4145 0052 0000     T:..+OTACLEAR...
20062830:	4f2b 4154 4552 4f43 4556 0052 432b 5550     +OTARECOVER.+CPU
20062840:	4f4c 4441 0000 0000 542b 5345 0054 0000     LOAD....+TEST...
20062850:	4c2b 5349 0054 0000 522b 5453 0000 0000     +LIST...+RST....
20062860:	532b 4154 4554 0000 472b 524d 0000 0000     +STATE..+GMR....
20062870:	4c2b 474f 0000 0000 522b 4552 0047 0000     +LOG....+RREG...
20062880:	572b 4552 0047 0000                         +WREG...

20062888 <__FUNCTION__.0>:
20062888:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062898:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
200628a8:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
200628b8:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
200628c8:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
200628d8:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
200628e8:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
200628f8:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20062908:	6f70 656e 746e 6f2f 2f73 7266 6565 7472     ponent/os/freert
20062918:	736f 662f 6572 7265 6f74 5f73 6568 7061     os/freertos_heap
20062928:	5f35 6f63 666e 6769 632e 0000 255b 5d73     5_config.c..[%s]
20062938:	6c20 7665 6c65 3d20 2520 0a64 0000 0000      level = %d.....
20062948:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20062958:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20062968:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20062978:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20062988:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20062998:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
200629a8:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
200629b8:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
200629c8:	696c 6574 732f 6c77 6269 6c2f 676f 632e     lite/swlib/log.c
200629d8:	0000 0000 002a 0000 6143 6863 2065 7261     ....*...Cache ar
200629e8:	6172 2079 7369 6620 6c75 2c6c 6120 646e     ray is full, and
200629f8:	7220 7065 616c 6563 6f20 646c 6520 746e      replace old ent
20062a08:	7972 000a 255b 3830 5d78 0020 0a0d 255b     ry..[%08x] ...[%
20062a18:	3830 5d78 0020 0000 3025 7838 0020 0000     08x] ...%08x ...
20062a28:	3025 7832 0020 0000 255b 2d73 6325 205d     %02x ...[%s-%c] 
20062a38:	0000 0000 003d 0000 002c 0000 7769 7270     ....=...,...iwpr
20062a48:	7669 0000 0a0d 6e75 6e6b 776f 206e 6f63     iv....unknown co
20062a58:	6d6d 6e61 2064 2527 2773 0000 0d0a 4d5b     mmand '%s'....[M
20062a68:	4d45 205d 6641 6574 2072 6f64 6320 646d     EM] After do cmd
20062a78:	202c 7661 6961 616c 6c62 2065 6568 7061     , available heap
20062a88:	2520 0a64 000d 0000 0a0d 230a 0a0d 0000      %d........#....
20062a98:	0000 0000 5749 4744 7220 6665 6572 6873     ....IWDG refresh
20062aa8:	6f20 216e 000a 0000 414d 4e49 0000 0000      on!....MAIN....
20062ab8:	4457 5f47 6954 656d 0072 0000 5749 4744     WDG_Timer...IWDG
20062ac8:	7220 6665 6572 6873 6520 7272 726f 000a      refresh error..
20062ad8:	524b 2034 534f 5320 4154 5452 0a20 0000     KR4 OS START ...
20062ae8:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
20062af8:	202d 4554 5453 4320 4d4f 414d 444e 4d20     - TEST COMMAND M
20062b08:	444f 2045 4548 504c 2520 2d64 2d2d 2d2d     ODE HELP %d-----
20062b18:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 0a2d 0000     -------------...
20062b28:	7325 000a 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     %s..------------
20062b38:	2d2d 2d2d 202d 4554 5453 4320 4d4f 414d     ----- TEST COMMA
20062b48:	444e 4d20 444f 2045 4e45 2044 2520 2d78     ND MODE END  %x-
20062b58:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
20062b68:	0a2d 0000 6552 6f62 746f 6e69 2067 2e2e     -...Rebooting ..
20062b78:	0a2e 000d 6175 7472 7562 6e72 0000 0000     ....uartburn....
20062b88:	255b 3830 5d78 2520 3830 2078 000a 0000     [%08x] %08x ....
20062b98:	7257 6e6f 2067 7261 7567 656d 746e 6e20     Wrong argument n
20062ba8:	6d75 6562 2172 0a0d 0000 0000 5352 5049     umber!......RSIP
20062bb8:	6520 616e 6c62 6465 202c 6143 276e 2074      enabled, Can't 
20062bc8:	6552 6461 4620 616c 6873 0a2e 0000 0000     Read Flash......
20062bd8:	7257 6e6f 2067 6d63 2164 0a0d 0000 0000     Wrong cmd!......
20062be8:	003f 0000 4809 4c45 2050 4028 293f 3a20     ?....HELP (@?) :
20062bf8:	0a20 0909 5020 6972 746e 6820 6c65 2070      ... Print help 
20062c08:	656d 7373 6761 000a 5744 0000 4009 5744     messag..DW...@DW
20062c18:	3c20 6441 7264 7365 2c73 4c20 6e65 7467      <Address, Lengt
20062c28:	3e68 090a 2009 7544 706d 6d20 6d65 726f     h>... Dump memor
20062c38:	2079 6f77 6472 6f20 2072 6552 6461 4820     y word or Read H
20062c48:	2077 6f77 6472 7220 6765 7369 6574 3b72     w word register;
20062c58:	0a20 0909 4420 2057 613c 6464 3e72 0920      ... DW <addr> .
20062c68:	2009 7544 706d 6f20 6c6e 2079 6e6f 2065     . Dump only one 
20062c78:	6f77 6472 6120 2074 6874 2065 6173 656d     word at the same
20062c88:	7420 6d69 2c65 7520 696e 3a74 3420 7942      time, unit: 4By
20062c98:	6574 0a73 0909 4420 2057 613c 6464 3e72     tes... DW <addr>
20062ca8:	3c20 656c 3e6e 0920 4420 6d75 2070 6874      <len> . Dump th
20062cb8:	2065 7073 6365 6669 6569 2064 656c 676e     e specified leng
20062cc8:	6874 6f20 2066 6874 2065 6f77 6472 202c     th of the word, 
20062cd8:	6e75 7469 203a 4234 7479 7365 090a 2009     unit: 4Bytes... 
20062ce8:	5744 3c20 6461 7264 203e 6c3c 6e65 203e     DW <addr> <len> 
20062cf8:	2062 4420 6d75 2070 6874 2065 7073 6365     b  Dump the spec
20062d08:	6669 6569 2064 656c 676e 6874 6f20 2066     ified length of 
20062d18:	6874 2065 7962 6574 202c 6e75 7469 203a     the byte, unit: 
20062d28:	4231 7479 0a65 0000 5745 0000 4009 5745     1Byte...EW...@EW
20062d38:	3c20 6441 7264 7365 2c73 4820 7865 0a3e      <Address, Hex>.
20062d48:	0909 5720 6972 6574 6d20 6d65 726f 2079     .. Write memory 
20062d58:	7764 726f 2064 726f 5720 6972 6574 4820     dword or Write H
20062d68:	2077 7764 726f 2064 6572 6967 7473 7265     w dword register
20062d78:	0a20 0909 4320 6e61 7720 6972 6574 6f20      ... Can write o
20062d88:	6c6e 2079 6e6f 2065 7764 726f 2064 7461     nly one dword at
20062d98:	7420 6568 7320 6d61 2065 6974 656d 0a20      the same time .
20062da8:	0909 4520 3a78 4520 2057 6441 7264 7365     .. Ex: EW Addres
20062db8:	2073 6156 756c 2065 000a 0000 4552 4f42     s Value ....REBO
20062dc8:	544f 0000 4009 4552 4f42 544f 0a20 0909     OT...@REBOOT ...
20062dd8:	7220 6265 6f6f 2074 090a 2009 6572 6f62      reboot ... rebo
20062de8:	746f 7520 7261 6274 7275 206e 000a 0000     ot uartburn ....
20062df8:	255b 5d73 2520 2073 3c3c 203c 6854 2065     [%s] %s <<< The 
20062e08:	756d 6574 2078 6168 2073 6f6e 2074 6562     mutex has not be
20062e18:	6e65 7220 6c65 6165 6573 2c64 6220 7475     en released, but
20062e28:	7420 6568 6d20 7475 7865 6820 7361 6220      the mutex has b
20062e38:	6565 206e 6564 656c 6574 2e64 3e20 3e3e     een deleted. >>>
20062e48:	000a 0000                                   ....

20062e4c <__FUNCTION__.0>:
20062e4c:	7472 736f 6d5f 7475 7865 645f 6c65 7465     rtos_mutex_delet
20062e5c:	0065 0000 6f4e 7354 006b 0000 524b 0034     e...NoTsk...KR4.
20062e6c:	614d 6c6c 636f 6620 6961 656c 2e64 4320     Malloc failed. C
20062e7c:	726f 3a65 255b 5d73 202c 6154 6b73 5b3a     ore:[%s], Task:[
20062e8c:	7325 2c5d 5b20 7266 6565 6820 6165 2070     %s], [free heap 
20062e9c:	6973 657a 203a 6425 0d5d 000a 0d0a 255b     size: %d].....[%
20062eac:	5d73 5320 4154 4b43 4f20 4556 4652 4f4c     s] STACK OVERFLO
20062ebc:	2057 202d 6154 6b73 614e 656d 2528 2973     W - TaskName(%s)
20062ecc:	0d0a 0000                                   ....

20062ed0 <__FUNCTION__.5>:
20062ed0:	4176 7070 696c 6163 6974 6e6f 7453 6361     vApplicationStac
20062ee0:	4f6b 6576 6672 6f6c 4877 6f6f 006b 0000     kOverflowHook...
20062ef0:	5441 3f53 0000 0000 4f4c 5547 5241 5f54     ATS?....LOGUART_
20062f00:	4154 4b53 0000 0000 7243 6165 6574 4c20     TASK....Create L
20062f10:	676f 5520 5241 2054 6154 6b73 4520 7272     og UART Task Err
20062f20:	2121 000a 4853 4c45 004c 0000 255b 5d73     !!..SHELL...[%s]
20062f30:	4920 474d 4449 203a 6425 202c 7563 7272      IMGID: %d, curr
20062f40:	6e65 2074 544f 2541 2064 6441 7264 7365     ent OTA%d Addres
20062f50:	3a73 3020 2578 3830 2c78 7420 7261 6567     s: 0x%08x, targe
20062f60:	2074 544f 2541 2064 6441 7264 7365 3a73     t OTA%d Address:
20062f70:	3020 2578 3830 0a78 0000 0000 5953 0053      0x%08x.....SYS.
20062f80:	255b 5d73 4920 474d 4449 203a 6425 202c     [%s] IMGID: %d, 
20062f90:	7563 7272 6e65 2074 6966 6d72 6177 6572     current firmware
20062fa0:	6920 2073 544f 2541 2c64 7420 7261 6567      is OTA%d, targe
20062fb0:	2074 6966 6d72 6177 6572 4f20 4154 6425     t firmware OTA%d
20062fc0:	6920 2073 6e69 6176 696c 0a64 0000 0000      is invalid.....
20062fd0:	255b 5d73 6220 6361 756b 2070 616d 6c6c     [%s] backup mall
20062fe0:	636f 6620 6961 646c 6465 000a               oc failded..

20062fec <__func__.0>:
20062fec:	7973 5f73 6572 6f63 6576 5f72 746f 5f61     sys_recover_ota_
20062ffc:	6973 6e67 7461 7275 0065 0000               signature...

20063008 <__func__.1>:
20063008:	7973 5f73 6c63 6165 5f72 746f 5f61 6973     sys_clear_ota_si
20063018:	6e67 7461 7275 0065 4449 454c 0000 0000     gnature.IDLE....
20063028:	2509 0963 6425 2509 0964 6425 0a0d 0000     .%c.%d.%d.%d....
20063038:	6d54 2072 7653 0063 e636 2005 e636 2005     Tmr Svc.6.. 6.. 
20063048:	e636 2005 e672 2005 e67e 2005 e69a 2005     6.. r.. ~.. ... 
20063058:	e636 2005 e636 2005 e672 2005 e67e 2005     6.. 6.. r.. ~.. 
20063068:	7325 2528 2964 202c 7641 6961 616c 6c62     %s(%d), Availabl
20063078:	2065 6568 7061 2520 0a64 0000 4c57 4e41     e heap %d...WLAN
20063088:	0000 0000 6e69 7469 0000 0000 6c77 6e61     ....init....wlan
20063098:	695f 696e 6974 6c61 7a69 2065 6166 6c69     _initialize fail
200630a8:	6465 000a                                   ed..

200630ac <__FUNCTION__.0>:
200630ac:	695f 696e 5f74 6874 6572 6461 0000 0000     _init_thread....
200630bc:	6f46 2072 6564 7562 2067 6170 6b63 6761     For debug packag
200630cc:	2e65 5020 656c 7361 2065 6573 656c 7463     e. Please select
200630dc:	7020 7273 6d61 7420 7079 2065 6361 6f63      psram type acco
200630ec:	6472 6e69 2067 6f74 7920 756f 2072 6863     rding to your ch
200630fc:	7069 000a 4843 5049 4e49 4f46 0000 0000     ip..CHIPINFO....
2006310c:	3230 6620 726f 5720 2042 3233 624d 300a     02 for WB 32Mb.0
2006311c:	2034 6f66 2072 4257 3120 3832 624d 300a     4 for WB 128Mb.0
2006312c:	2033 6f66 2072 4257 3220 3635 624d 0a0a     3 for WB 256Mb..
2006313c:	0000 0000 682f 6d6f 2f65 7561 6f74 6a5f     ..../home/auto_j
2006314c:	6e65 696b 736e 775f 616c 356e 492f 544f     enkins_wlan5/IOT
2006315c:	415f 5354 722f 7065 736f 722f 6c65 6165     _ATS/repos/relea
2006316c:	6573 6d2d 7361 6574 2f72 6f73 7275 6563     se-master/source
2006317c:	612f 656d 6162 696c 6574 675f 6363 705f     /amebalite_gcc_p
2006318c:	6f72 656a 7463 702f 6f72 656a 7463 6b5f     roject/project_k
2006319c:	3472 762f 6473 2f6b 2e2e 2e2f 2f2e 2e2e     r4/vsdk/../../..
200631ac:	632f 6d6f 6f70 656e 746e 732f 636f 612f     /component/soc/a
200631bc:	656d 6162 696c 6574 6c2f 6269 722f 6d61     mebalite/lib/ram
200631cc:	635f 6d6f 6f6d 2f6e 6d61 6265 5f61 6863     _common/ameba_ch
200631dc:	7069 6e69 6f66 6c5f 6269 632e 0000 0000     ipinfo_lib.c....
200631ec:	7325 203a 0000 0000 5245 3a52 4720 7465     %s: ....ERR: Get
200631fc:	5320 636f 4e20 6d61 2065 6146 6c69 000a      Soc Name Fail..
2006320c:	3025 7832 0000 0000 4d4b 0034 7325 4c20     %02x....KM4.%s L
2006321c:	6269 5620 7265 6973 6e6f 0a3a 0000 0000     ib Version:.....
2006322c:	3032 3432 302f 2f36 3131 312d 3a34 3933     2024/06/11-14:39
2006323c:	303a 0037 4f43 504d 4c49 2045 4954 454d     :07.COMPILE TIME
2006324c:	203a 7325 000a 0000 6367 2063 6576 7372     : %s....gcc vers
2006325c:	6f69 206e 3031 332e 312e 3220 3230 3032     ion 10.3.1 20220
2006326c:	3335 2030 5228 6165 746c 6b65 5620 4453     530 (Realtek VSD
2006327c:	2d4b 3031 332e 312e 4220 6975 646c 3420     K-10.3.1 Build 4
2006328c:	3730 2b33 2029 0000 4f43 504d 4c49 5245     073+) ..COMPILER
2006329c:	203a 7325 000a 0000 6325 0000 2d2d 2d2d     : %s....%c..----
200632ac:	2d2d 2d2d 2d2d 2d2d 6154 6b73 4920 666e     --------Task Inf
200632bc:	2d6f 2d2d 2d2d 2d2d 2d2d 2d2d 0d2d 000a     o------------...
200632cc:	6146 6c75 2074 6e6f 7420 7361 206b 253c     Fault on task <%
200632dc:	3e73 000a 6154 6b73 4920 3a44 2520 0d64     s>..Task ID: %d.
200632ec:	000a 0000 6154 6b73 5420 4243 303a 2578     ....Task TCB:0x%
200632fc:	3830 786c 0a0d 0000 7543 7272 6e65 2074     08lx....Current 
2006330c:	7453 7461 3a65 2520 2064 2528 2973 0a0d     State: %d (%s)..
2006331c:	0000 0000 6142 6573 5020 6972 726f 7469     ....Base Priorit
2006332c:	3a79 2520 0d64 000a 7543 7272 6e65 2074     y: %d...Current 
2006333c:	7250 6f69 6972 7974 203a 6425 0a0d 0000     Priority: %d....
2006334c:	7552 206e 6954 656d 4320 756f 746e 7265     Run Time Counter
2006335c:	203a 6c25 0d75 000a 7453 6361 546b 706f     : %lu...StackTop
2006336c:	203a 7830 3025 6c38 2c78 5320 6174 6b63     : 0x%08lx, Stack
2006337c:	6142 6573 203a 7830 3025 6c38 2c78 5320     Base: 0x%08lx, S
2006338c:	6174 6b63 6e45 3a64 3020 2578 3830 786c     tackEnd: 0x%08lx
2006339c:	202c 7453 6361 536b 7a69 3d65 6c25 2875     , StackSize=%lu(
200633ac:	6f77 6472 0d29 000a 7453 6361 206b 6948     word)...Stack Hi
200633bc:	6867 5720 7461 7265 614d 6b72 203a 6425     gh WaterMark: %d
200633cc:	7728 726f 2964 0a0d 0000 0000 7552 6e6e     (word)......Runn
200633dc:	6e69 0067 6552 6461 2079 202f 6550 646e     ing.Ready / Pend
200633ec:	6e69 0067 6c42 636f 656b 0064 7553 7073     ing.Blocked.Susp
200633fc:	6e65 6564 0064 0000 6544 656c 6574 0064     ended...Deleted.
2006340c:	6e49 6176 696c 0064                         Invalid.

20063414 <TaskStateString.0>:
20063414:	33d8 2006 33e0 2006 33f0 2006 33f8 2006     .3. .3. .3. .3. 
20063424:	3404 2006 340c 2006 ef78 2005 ee8c 2005     .4. .4. x.. ... 
20063434:	ee8c 2005 ee8c 2005 ee8c 2005 ee8c 2005     ... ... ... ... 
20063444:	ee8c 2005 ee8c 2005 ee8c 2005 ee8c 2005     ... ... ... ... 
20063454:	eec2 2005 f034 2005 ef02 2005 ee8c 2005     ... 4.. ... ... 
20063464:	ee8c 2005 ee8c 2005 ee8c 2005 ef02 2005     ... ... ... ... 
20063474:	ee8c 2005 ee8c 2005 f088 2005 ee8c 2005     ... ... ... ... 
20063484:	ee8c 2005 eee8 2005 ef9e 2005 ee8c 2005     ... ... ... ... 
20063494:	ee8c 2005 efbc 2005 ee8c 2005 ef44 2005     ... ... ... D.. 
200634a4:	ee8c 2005 ee8c 2005 ef7a 2005 ee8c 2005     ... ... z.. ... 
200634b4:	f08a 2005 3130 3332 3534 3736 3938 4241     ... 0123456789AB
200634c4:	4443 4645 0000 0000 6f69 6420 6972 6576     CDEF....io drive
200634d4:	2072 6170 6172 656d 6574 7372 6520 7272     r parameters err
200634e4:	726f 2021 6966 656c 6e5f 6d61 3a65 2520     or! file_name: %
200634f4:	2c73 6c20 6e69 3a65 2520 0064 682f 6d6f     s, line: %d./hom
20063504:	2f65 7561 6f74 6a5f 6e65 696b 736e 775f     e/auto_jenkins_w
20063514:	616c 356e 492f 544f 415f 5354 722f 7065     lan5/IOT_ATS/rep
20063524:	736f 722f 6c65 6165 6573 6d2d 7361 6574     os/release-maste
20063534:	2f72 6f73 7275 6563 612f 656d 6162 696c     r/source/amebali
20063544:	6574 675f 6363 705f 6f72 656a 7463 702f     te_gcc_project/p
20063554:	6f72 656a 7463 6b5f 3472 762f 6473 2f6b     roject_kr4/vsdk/
20063564:	2e2e 2e2f 2f2e 2e2e 632f 6d6f 6f70 656e     ../../../compone
20063574:	746e 732f 636f 612f 656d 6162 696c 6574     nt/soc/amebalite
20063584:	6c2f 6269 722f 6d61 635f 6d6f 6f6d 2f6e     /lib/ram_common/
20063594:	6d61 6265 5f61 6f72 5f6d 6170 6374 2e68     ameba_rom_patch.
200635a4:	0063 0000 7269 5b71 6425 205d 7270 6f69     c...irq[%d] prio
200635b4:	6972 7974 2520 2078 6873 6c61 206c 3d3c     rity %x shall <=
200635c4:	2520 0a64 0000 0000 5b1b 3b31 3333 5b6d      %d......[1;33m[
200635d4:	5245 4f52 5d52 5b20 7325 3a3a 6425 205d     ERROR] [%s::%d] 
200635e4:	7473 6361 5f6b 6973 657a 2528 2964 6920     stack_size(%d) i
200635f4:	2073 6f70 6973 6974 6576 3f20 0a3f 5b1b     s positive ??..[
20063604:	6d30 0000 5b1b 3b31 3333 5b6d 5245 4f52     0m...[1;33m[ERRO
20063614:	5d52 5b20 7325 3a3a 6425 205d 6172 6f5f     R] [%s::%d] ra_o
20063624:	6666 6573 2874 6425 2029 7369 6e20 6765     ffset(%d) is neg
20063634:	7461 7669 3f65 1b3f 305b 006d 5b1b 3b31     ative??.[0m..[1;
20063644:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063654:	6425 205d 7453 7261 2074 7473 6361 206b     %d] Start stack 
20063664:	6162 6b63 7274 6361 6e69 2067 6f66 2072     backtracing for 
20063674:	7073 2520 2c70 7020 2063 7025 1b0a 305b     sp %p, pc %p..[0
20063684:	006d 0000 5b1b 3b31 3333 5b6d 5254 4341     m....[1;33m[TRAC
20063694:	5d45 5b20 7325 3a3a 6425 205d 665b 6172     E] [%s::%d] [fra
200636a4:	656d 2320 6425 205d 7073 3e2d 2520 2c70     me #%d] sp-> %p,
200636b4:	7020 2d63 203e 7025 202c 7473 6361 5f6b      pc-> %p, stack_
200636c4:	6973 657a 3e2d 2520 2c64 7220 2d61 203e     size-> %d, ra-> 
200636d4:	7025 1b0a 305b 006d 5b1b 3b31 3333 5b6d     %p..[0m..[1;33m[
200636e4:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200636f4:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20063704:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20063714:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20063724:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063734:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20063744:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20063754:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20063764:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20063774:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20063784:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063794:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
200637a4:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
200637b4:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
200637c4:	1b0a 305b 006d 0000                         ..[0m...

200637cc <__FUNCTION__.3>:
200637cc:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
200637dc:	6932 0000                                   2i..

200637e0 <__FUNCTION__.2>:
200637e0:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
200637f0:	6332 0000                                   2c..

200637f4 <__FUNCTION__.1>:
200637f4:	6f64 735f 6174 6b63 625f 6361 746b 6172     do_stack_backtra
20063804:	6563 0000 6e55 6e6b 776f 206e 7865 6563     ce..Unknown exce
20063814:	7470 6f69 006e 0000 5b1b 3b31 3333 5b6d     ption....[1;33m[
20063824:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063834:	3d3d 3d3d 3d3d 3d3d 3d3d 4320 6172 6873     ========== Crash
20063844:	4420 6d75 2070 3d3d 3d3d 3d3d 3d3d 3d3d      Dump ==========
20063854:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063864:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063874:	7845 6563 7470 6f69 206e 6163 6775 7468     Exception caught
20063884:	6f20 206e 7025 7720 7469 2068 6572 7361      on %p with reas
20063894:	6e6f 5b20 7830 7825 205d 3e2d 5b20 7325     on [0x%x] -> [%s
200638a4:	0a5d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     ]..[0m...[1;33m[
200638b4:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200638c4:	3d3d 3d3d 3d3d 3d3d 3d3d 5220 6765 7369     ========== Regis
200638d4:	6574 2072 7544 706d 3d20 3d3d 3d3d 3d3d     ter Dump =======
200638e4:	3d3d 0a3d 5b1b 6d30 0000 0000 5b1b 3b31     ===..[0m.....[1;
200638f4:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063904:	6425 205d 6d5b 6373 6172 6374 5d68 3020     %d] [mscratch] 0
20063914:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063924:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063934:	6425 205d 6d5b 7065 5d63 2020 2020 3020     %d] [mepc]     0
20063944:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063954:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063964:	6425 205d 6d5b 6163 7375 5d65 2020 3020     %d] [mcause]   0
20063974:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063984:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063994:	6425 205d 6d5b 7674 6c61 205d 2020 3020     %d] [mtval]    0
200639a4:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
200639b4:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
200639c4:	6425 205d 7845 6563 7470 6f69 206e 6163     %d] Exception ca
200639d4:	7375 2065 6f63 6c75 2064 6562 0d3a 1b0a     use could be:...
200639e4:	305b 006d 5b1b 3b31 3333 5b6d 5254 4341     [0m..[1;33m[TRAC
200639f4:	5d45 5b20 7325 3a3a 6425 205d 2509 2c73     E] [%s::%d] .%s,
20063a04:	2520 2073 203d 7830 7825 0a0d 5b1b 6d30      %s = 0x%x...[0m
20063a14:	0000 0000 5b1b 3b31 3333 5b6d 5254 4341     .....[1;33m[TRAC
20063a24:	5d45 5b20 7325 3a3a 6425 205d 2509 0d73     E] [%s::%d] .%s.
20063a34:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063a44:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063a54:	785b 6425 2d20 203e 7325 205d 7830 3025     [x%d -> %s] 0x%0
20063a64:	7838 1b0a 305b 006d 5b1b 3b31 3333 5b6d     8x..[0m..[1;33m[
20063a74:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063a84:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20063a94:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20063aa4:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20063ab4:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063ac4:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20063ad4:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20063ae4:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20063af4:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20063b04:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20063b14:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063b24:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
20063b34:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
20063b44:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
20063b54:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063b64:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063b74:	3d3d 3d3d 3d3d 3d3d 3d3d 4520 646e 6f20     ========== End o
20063b84:	2066 7243 7361 2068 7544 706d 3d20 3d3d     f Crash Dump ===
20063b94:	3d3d 3d3d 3d3d 0a3d 5b1b 6d30 0000 0000     =======..[0m....
20063ba4:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063bb4:	7325 3a3a 6425 205d 0a0d 0a0d 5b1b 6d30     %s::%d] .....[0m
20063bc4:	0000 0000 6f53 6572 412f 4f4d 7020 6761     ....Sore/AMO pag
20063bd4:	2065 6177 6b6c 6320 6568 6b63 6620 6961     e walk check fai
20063be4:	006c 0000 7453 726f 2065 6461 7264 7365     l...Store addres
20063bf4:	0073 0000 6f4c 6461 7020 6761 2065 6177     s...Load page wa
20063c04:	6b6c 6320 6568 6b63 6620 6961 006c 0000     lk check fail...
20063c14:	6f4c 6461 6120 6464 6572 7373 0000 0000     Load address....
20063c24:	6e49 7473 7572 7463 6f69 206e 6170 6567     Instruction page
20063c34:	7720 6c61 206b 6863 6365 206b 6166 6c69      walk check fail
20063c44:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
20063c54:	6461 7264 7365 0073 7253 726f 2f65 4d41     address.Srore/AM
20063c64:	204f 4d50 2050 6863 6365 206b 6166 6c69     O PMP check fail
20063c74:	0000 0000 7453 726f 2f65 4d41 204f 6170     ....Store/AMO pa
20063c84:	6567 7720 6c61 206b 7562 2073 7265 6f72     ge walk bus erro
20063c94:	0072 0000 7453 726f 2065 7562 2073 7265     r...Store bus er
20063ca4:	6f72 0072 7453 726f 2f65 4d41 204f 6461     ror.Store/AMO ad
20063cb4:	7264 7365 2073 696d 6173 696c 6e67 6465     dress misaligned
20063cc4:	0000 0000 6f4c 6461 5020 504d 6320 6568     ....Load PMP che
20063cd4:	6b63 6620 6961 006c 6f4c 6461 5020 6761     ck fail.Load Pag
20063ce4:	2065 6177 6b6c 6220 7375 6520 7272 726f     e walk bus error
20063cf4:	0000 0000 6f4c 6461 6220 7375 6520 7272     ....Load bus err
20063d04:	726f 0000 6f4c 6461 6120 6464 6572 7373     or..Load address
20063d14:	6d20 7369 6c61 6769 656e 0064 6c49 656c      misaligned.Ille
20063d24:	6167 206c 6e69 7473 7572 7463 6f69 006e     gal instruction.
20063d34:	6e49 7473 7572 7463 6f69 206e 706f 6f63     Instruction opco
20063d44:	6564 0000 6e49 7473 7572 7463 6f69 206e     de..Instruction 
20063d54:	4d50 2050 6863 6365 206b 6166 6c69 0000     PMP check fail..
20063d64:	6e49 7473 7572 7463 6f69 206e 6150 6567     Instruction Page
20063d74:	7720 6c61 206b 7562 2073 7265 6f72 0072      walk bus error.
20063d84:	6e49 7473 7572 7463 6f69 206e 7562 2073     Instruction bus 
20063d94:	7265 6f72 0072 0000 7242 6e61 6863 7420     error...Branch t
20063da4:	7261 6567 2074 696d 6173 696c 6e67 6465     arget misaligned
20063db4:	0000 0000 6154 6772 7465 6120 6464 6572     ....Target addre
20063dc4:	7373 0000 6e49 7473 7572 7463 6f69 206e     ss..Instruction 
20063dd4:	6461 7264 7365 2073 696d 6173 696c 6e67     address misalign
20063de4:	6465 0000 657a 6f72 0000 0000 6172 0000     ed..zero....ra..
20063df4:	7073 0000 7067 0000 7074 0000 3074 0000     sp..gp..tp..t0..
20063e04:	3174 0000 3274 0000 3073 662f 0070 0000     t1..t2..s0/fp...
20063e14:	3173 0000 3061 0000 3161 0000 3261 0000     s1..a0..a1..a2..
20063e24:	3361 0000 3461 0000 3561 0000 3661 0000     a3..a4..a5..a6..
20063e34:	3761 0000 3273 0000 3373 0000 3473 0000     a7..s2..s3..s4..
20063e44:	3573 0000 3673 0000 3773 0000 3873 0000     s5..s6..s7..s8..
20063e54:	3973 0000 3173 0030 3173 0031 3374 0000     s9..s10.s11.t3..
20063e64:	3474 0000 3574 0000 3674 0000 6e49 7473     t4..t5..t6..Inst
20063e74:	7572 7463 6f69 206e 6361 6563 7373 6620     ruction access f
20063e84:	7561 746c 0000 0000 7242 6165 706b 696f     ault....Breakpoi
20063e94:	746e 0000 6f4c 6461 6120 6464 7365 2073     nt..Load addess 
20063ea4:	696d 6173 696c 6e67 6465 0000 6f4c 6461     misaligned..Load
20063eb4:	6120 6363 7365 2073 6166 6c75 0074 0000      access fault...
20063ec4:	7453 726f 2065 6461 7264 7365 2073 696d     Store address mi
20063ed4:	6173 696c 6e67 6465 0000 0000 7453 726f     saligned....Stor
20063ee4:	2065 6461 7264 7365 2073 6166 6c75 0074     e address fault.
20063ef4:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
20063f04:	6620 6f72 206d 2055 6f6d 6564 0000 0000      from U mode....
20063f14:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
20063f24:	6620 6f72 206d 2053 6f6d 6564 0000 0000      from S mode....
20063f34:	6552 6573 7672 6465 6520 6378 7065 6974     Reserved excepti
20063f44:	6e6f 0000 6e45 6976 6f72 6d6e 6e65 2074     on..Environment 
20063f54:	6163 6c6c 6620 6f72 206d 204d 6f6d 6564     call from M mode
20063f64:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
20063f74:	6170 6567 6620 7561 746c 0000 6f4c 6461     page fault..Load
20063f84:	7020 6761 2065 6166 6c75 0074 7453 726f      page fault.Stor
20063f94:	2065 6170 6567 6620 7561 746c 0000 0000     e page fault....

20063fa4 <__FUNCTION__.0>:
20063fa4:	7263 7361 5f68 7564 706d 0000               crash_dump..

20063fb0 <exception_cause_0_string>:
20063fb0:	3d9c 2006 3db8 2006 3dc8 2006 3c48 2006     .=. .=. .=. H<. 
	...

20063fc8 <exception_cause_12_string>:
20063fc8:	3c24 2006 3c48 2006 0000 0000 0000 0000     $<. H<. ........

20063fd8 <exception_cause_13_string>:
20063fd8:	3bf8 2006 3c14 2006 0000 0000 0000 0000     .;. .<. ........

20063fe8 <exception_cause_15_string>:
20063fe8:	3bc8 2006 3be8 2006 0000 0000 0000 0000     .;. .;. ........

20063ff8 <exception_cause_1_string>:
20063ff8:	3d48 2006 3c48 2006 3d64 2006 3c48 2006     H=. H<. d=. H<. 
20064008:	3d84 2006 3c48 2006 0000 0000 0000 0000     .=. H<. ........

20064018 <exception_cause_2_string>:
20064018:	3d20 2006 3d34 2006 0000 0000 0000 0000      =. 4=. ........

20064028 <exception_cause_4_string>:
20064028:	3d08 2006 3c14 2006 0000 0000 0000 0000     .=. .<. ........

20064038 <exception_cause_5_string>:
20064038:	3cc8 2006 3c14 2006 3cdc 2006 3c14 2006     .<. .<. .<. .<. 
20064048:	3cf8 2006 0000 0000 0000 0000 0000 0000     .<. ............

20064058 <exception_cause_6_string>:
20064058:	3ca8 2006 3be8 2006 0000 0000 0000 0000     .<. .;. ........

20064068 <exception_cause_7_string>:
20064068:	3c5c 2006 3be8 2006 3c78 2006 3be8 2006     \<. .;. x<. .;. 
20064078:	3c98 2006 0000 0000 0000 0000 0000 0000     .<. ............

20064088 <exception_cause_string_tbl>:
20064088:	3fb0 2006 3ff8 2006 4018 2006 0000 0000     .?. .?. .@. ....
20064098:	4028 2006 4038 2006 4058 2006 4068 2006     (@. 8@. X@. h@. 
	...
200640b8:	3fc8 2006 3fd8 2006 0000 0000 3fe8 2006     .?. .?. .....?. 

200640c8 <exception_code_string>:
200640c8:	3dc8 2006 3e70 2006 3d20 2006 3e8c 2006     .=. p>.  =. .>. 
200640d8:	3e98 2006 3eb0 2006 3ec4 2006 3ee0 2006     .>. .>. .>. .>. 
200640e8:	3ef4 2006 3f14 2006 3f34 2006 3f48 2006     .>. .?. 4?. H?. 
200640f8:	3f68 2006 3f80 2006 0000 0000 3f90 2006     h?. .?. .....?. 

20064108 <register_string>:
20064108:	3de8 2006 3df0 2006 3df4 2006 3df8 2006     .=. .=. .=. .=. 
20064118:	3dfc 2006 3e00 2006 3e04 2006 3e08 2006     .=. .>. .>. .>. 
20064128:	3e0c 2006 3e14 2006 3e18 2006 3e1c 2006     .>. .>. .>. .>. 
20064138:	3e20 2006 3e24 2006 3e28 2006 3e2c 2006      >. $>. (>. ,>. 
20064148:	3e30 2006 3e34 2006 3e38 2006 3e3c 2006     0>. 4>. 8>. <>. 
20064158:	3e40 2006 3e44 2006 3e48 2006 3e4c 2006     @>. D>. H>. L>. 
20064168:	3e50 2006 3e54 2006 3e58 2006 3e5c 2006     P>. T>. X>. \>. 
20064178:	3e60 2006 3e64 2006 3e68 2006 3e6c 2006     `>. d>. h>. l>. 
20064188:	4544 2056 5049 2043 5041 2049 736d 2067     DEV IPC API msg 
20064198:	554e 4c4c 000a 0000 6f48 7473 4120 4950     NULL....Host API
200641a8:	5520 6b6e 6f6e 6e77 6520 7476 2528 2978      Unknown evt(%x)
200641b8:	000a 0000 0192 2006 01a0 2006 0258 2006     ....... ... X.. 
200641c8:	01f8 2006 031e 2006 0332 2006 03a8 2006     ... ... 2.. ... 
200641d8:	01f8 2006 01f8 2006 616c 7473 6920 696e     ... ... last ini
200641e8:	2063 7069 2063 6f6e 2074 6468 206c 000a     c ipc not hdl ..
200641f8:	7348 4d74 6773 6553 646e 7720 6961 2074     HstMsgSend wait 
20064208:	6e69 6369 6920 6370 6420 6e6f 0a65 0000     inic ipc done...
20064218:	6e69 6369 615f 6970 685f 736f 5f74 6174     inic_api_host_ta
20064228:	6b73 0000 7243 6165 6574 6120 6970 685f     sk..Create api_h
20064238:	736f 5f74 6174 6b73 4520 7272 000a 0000     ost_task Err....
20064248:	4e49 4349 0000 0000 6e69 6369 6d5f 5f70     INIC....inic_mp_
20064258:	6f63 6d6d 6e61 2064 6166 6c69 000a 0000     command fail....
20064268:	7250 7669 7461 2065 654d 7373 6761 3a65     Private Message:
20064278:	2520 0a73 0000 0000                          %s.....

20064280 <__func__.0>:
20064280:	6e69 6369 615f 6970 685f 736f 5f74 656d     inic_api_host_me
20064290:	7373 6761 5f65 6573 646e 0000 6f68 7473     ssage_send..host
200642a0:	735f 626b 785f 2078 6f6e 2074 7266 6565     _skb_xx not free
200642b0:	000a 0000 7325 3e3d 6b73 2062 616d 6c6c     ....%s=>skb mall
200642c0:	636f 6620 6961 216c 0d0a 0000 6f48 7473     oc fail!....Host
200642d0:	5520 6b6e 6f6e 6e77 6520 6576 746e 2528      Unknown event(%
200642e0:	786c 2129 0d0a 0000                         lx)!....

200642e8 <__func__.1>:
200642e8:	6e69 6369 685f 736f 5f74 6e69 7469 735f     inic_host_init_s
200642f8:	626b 0000 6e69 6369 6d5f 6773 715f 745f     kb..inic_msg_q_t
20064308:	7361 006b 7243 6165 6574 6d20 6773 715f     ask.Create msg_q
20064318:	745f 7361 206b 7245 2172 000a 4f4e 6220     _task Err!..NO b
20064328:	6675 6620 726f 6e20 7765 6e20 646f 7365     uf for new nodes
20064338:	0a21 0000                                   !...

2006433c <__clz_tab>:
2006433c:	0100 0202 0303 0303 0404 0404 0404 0404     ................
2006434c:	0505 0505 0505 0505 0505 0505 0505 0505     ................
2006435c:	0606 0606 0606 0606 0606 0606 0606 0606     ................
2006436c:	0606 0606 0606 0606 0606 0606 0606 0606     ................
2006437c:	0707 0707 0707 0707 0707 0707 0707 0707     ................
2006438c:	0707 0707 0707 0707 0707 0707 0707 0707     ................
2006439c:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200643ac:	0707 0707 0707 0707 0707 0707 0707 0707     ................
200643bc:	0808 0808 0808 0808 0808 0808 0808 0808     ................
200643cc:	0808 0808 0808 0808 0808 0808 0808 0808     ................
200643dc:	0808 0808 0808 0808 0808 0808 0808 0808     ................
200643ec:	0808 0808 0808 0808 0808 0808 0808 0808     ................
200643fc:	0808 0808 0808 0808 0808 0808 0808 0808     ................
2006440c:	0808 0808 0808 0808 0808 0808 0808 0808     ................
2006441c:	0808 0808 0808 0808 0808 0808 0808 0808     ................
2006442c:	0808 0808 0808 0808 0808 0808 0808 0808     ................

2006443c <_ctype_>:
2006443c:	2000 2020 2020 2020 2020 2828 2828 2028     .         ((((( 
2006444c:	2020 2020 2020 2020 2020 2020 2020 2020                     
2006445c:	8820 1010 1010 1010 1010 1010 1010 1010      ...............
2006446c:	0410 0404 0404 0404 0404 1004 1010 1010     ................
2006447c:	1010 4141 4141 4141 0101 0101 0101 0101     ..AAAAAA........
2006448c:	0101 0101 0101 0101 0101 0101 1010 1010     ................
2006449c:	1010 4242 4242 4242 0202 0202 0202 0202     ..BBBBBB........
200644ac:	0202 0202 0202 0202 0202 0202 1010 1010     ................
200644bc:	0020 0000 0000 0000 0000 0000 0000 0000      ...............
	...

20064540 <shell_cmd_table>:
20064540:	2be8 2006 0000 0000 b376 2005 2bec 2006     .+. ....v.. .+. 
20064550:	2c10 2006 0002 0000 b4d2 2005 2c14 2006     .,. ....... .,. 
20064560:	2d30 2006 0002 0000 b42c 2005 2d34 2006     0-. ....,.. 4-. 
20064570:	2dc4 2006 0004 0000 b3e2 2005 2dcc 2006     .-. ....... .-. 

20064580 <ipc_tickless_table>:
20064580:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064590:	0000 0000 0010 0000 0000 0000               ............

2006459c <ipc_kr4cdsp_table>:
2006459c:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
200645ac:	0000 0000 0020 0000 0000 0000               .... .......

200645b8 <ipc_KM4WKR4_table>:
200645b8:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
200645c8:	0000 0000 0010 0000 0002 0000               ............

200645d4 <ipc_shell_table>:
200645d4:	0001 0000 cd5e 2005 0000 0000 8eac 2005     ....^.. ....... 
200645e4:	0000 0000 0010 0000 0003 0000               ............

200645f0 <ipc_api_host_table>:
200645f0:	0000 0000 047e 2006 0000 0000 8eac 2005     ....~.. ....... 
20064600:	0000 0000 0010 0000 0007 0000               ............

2006460c <ipc_host_event_table>:
2006460c:	0000 0000 0aaa 2006 0000 0000 8eac 2005     ....... ....... 
2006461c:	0000 0000 0010 0000 0006 0000               ............

20064628 <lib_pmc_git_rev>:
20064628:	6c20 6269 705f 636d 765f 7265 335f 3561      lib_pmc_ver_3a5
20064638:	3132 3930 3964 5f64 3032 3432 302f 2f36     2109d9d_2024/06/
20064648:	3131 312d 3a34 3933 313a 0034               11-14:39:14.

20064654 <lib_wifi_com_sec_git_rev>:
20064654:	6c20 6269 775f 6669 5f69 6f63 5f6d 6573      lib_wifi_com_se
20064664:	5f63 6576 5f72 6133 3235 3031 6439 6439     c_ver_3a52109d9d
20064674:	325f 3230 2f34 3630 312f 2d31 3431 333a     _2024/06/11-14:3
20064684:	3a39 3231 0000 0000                         9:12....

2006468c <lib_wps_git_rev>:
2006468c:	6c20 6269 775f 7370 765f 7265 335f 3561      lib_wps_ver_3a5
2006469c:	3132 3930 3964 5f64 3032 3432 302f 2f36     2109d9d_2024/06/
200646ac:	3131 312d 3a34 3933 313a 0035               11-14:39:15.

200646b8 <__frame_dummy_init_array_entry>:
200646b8:	0ec4 2006                                   ... 

200646bc <__do_global_dtors_aux_fini_array_entry>:
200646bc:	0e8a 2006                                   ... 

Disassembly of section .sram_image2.text.data:

200656c0 <FLASH_ClockSwitch>:
200656c0:	1101                	c.addi	sp,-32
200656c2:	ca26                	c.swsp	s1,20(sp)
200656c4:	c452                	c.swsp	s4,8(sp)
200656c6:	ce06                	c.swsp	ra,28(sp)
200656c8:	cc22                	c.swsp	s0,24(sp)
200656ca:	c84a                	c.swsp	s2,16(sp)
200656cc:	c64e                	c.swsp	s3,12(sp)
200656ce:	c256                	c.swsp	s5,4(sp)
200656d0:	84aa                	c.mv	s1,a0
200656d2:	8a2e                	c.mv	s4,a1
200656d4:	c1dd                	c.beqz	a1,2006577a <FLASH_ClockSwitch+0xba>
200656d6:	c00f30ef          	jal	ra,20058ad6 <Systick_State>
200656da:	89aa                	c.mv	s3,a0
200656dc:	c1af30ef          	jal	ra,20058af6 <irq_disable_save>
200656e0:	892a                	c.mv	s2,a0
200656e2:	4501                	c.li	a0,0
200656e4:	bc6f30ef          	jal	ra,20058aaa <Systick_Cmd>
200656e8:	20001ab7          	lui	s5,0x20001
200656ec:	4789                	c.li	a5,2
200656ee:	864a8413          	addi	s0,s5,-1948 # 20000864 <flash_init_para>
200656f2:	08f49e63          	bne	s1,a5,2006578e <FLASH_ClockSwitch+0xce>
200656f6:	4509                	c.li	a0,2
200656f8:	dffa0097          	auipc	ra,0xdffa0
200656fc:	346080e7          	jalr	ra,838(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065700:	4100c6b7          	lui	a3,0x4100c
20065704:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065708:	7775                	c.lui	a4,0xffffd
2006570a:	177d                	c.addi	a4,-1
2006570c:	8ff9                	c.and	a5,a4
2006570e:	24f6ae23          	sw	a5,604(a3)
20065712:	06344783          	lbu	a5,99(s0) # 9063 <shell_get_argv+0x18f>
20065716:	c7ad                	c.beqz	a5,20065780 <FLASH_ClockSwitch+0xc0>
20065718:	4501                	c.li	a0,0
2006571a:	dff9e097          	auipc	ra,0xdff9e
2006571e:	260080e7          	jalr	ra,608(ra) # 397a <FLASH_CalibrationNewCmd>
20065722:	4505                	c.li	a0,1
20065724:	dff9e097          	auipc	ra,0xdff9e
20065728:	014080e7          	jalr	ra,20(ra) # 3738 <FLASH_CalibrationPLLSel>
2006572c:	dff9e097          	auipc	ra,0xdff9e
20065730:	07e080e7          	jalr	ra,126(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065734:	4501                	c.li	a0,0
20065736:	dff9e097          	auipc	ra,0xdff9e
2006573a:	002080e7          	jalr	ra,2(ra) # 3738 <FLASH_CalibrationPLLSel>
2006573e:	dff9e097          	auipc	ra,0xdff9e
20065742:	06c080e7          	jalr	ra,108(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065746:	04040ea3          	sb	zero,93(s0)
2006574a:	00444583          	lbu	a1,4(s0)
2006574e:	864a8513          	addi	a0,s5,-1948
20065752:	dff9d097          	auipc	ra,0xdff9d
20065756:	572080e7          	jalr	ra,1394(ra) # 2cc4 <FLASH_SetSpiMode>
2006575a:	0c0a0c63          	beq	s4,zero,20065832 <FLASH_ClockSwitch+0x172>
2006575e:	854e                	c.mv	a0,s3
20065760:	b4af30ef          	jal	ra,20058aaa <Systick_Cmd>
20065764:	4462                	c.lwsp	s0,24(sp)
20065766:	40f2                	c.lwsp	ra,28(sp)
20065768:	44d2                	c.lwsp	s1,20(sp)
2006576a:	49b2                	c.lwsp	s3,12(sp)
2006576c:	4a22                	c.lwsp	s4,8(sp)
2006576e:	4a92                	c.lwsp	s5,4(sp)
20065770:	854a                	c.mv	a0,s2
20065772:	4942                	c.lwsp	s2,16(sp)
20065774:	6105                	c.addi16sp	sp,32
20065776:	b88f306f          	jal	zero,20058afe <irq_enable_restore>
2006577a:	4901                	c.li	s2,0
2006577c:	4981                	c.li	s3,0
2006577e:	b7ad                	c.j	200656e8 <FLASH_ClockSwitch+0x28>
20065780:	4581                	c.li	a1,0
20065782:	4501                	c.li	a0,0
20065784:	dff9e097          	auipc	ra,0xdff9e
20065788:	3fa080e7          	jalr	ra,1018(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
2006578c:	bf59                	c.j	20065722 <FLASH_ClockSwitch+0x62>
2006578e:	ccb5                	c.beqz	s1,2006580a <FLASH_ClockSwitch+0x14a>
20065790:	4501                	c.li	a0,0
20065792:	dff9e097          	auipc	ra,0xdff9e
20065796:	fa6080e7          	jalr	ra,-90(ra) # 3738 <FLASH_CalibrationPLLSel>
2006579a:	dff9e097          	auipc	ra,0xdff9e
2006579e:	fb2080e7          	jalr	ra,-78(ra) # 374c <FLASH_Calibration_PSPLL_Open>
200657a2:	4505                	c.li	a0,1
200657a4:	dff9e097          	auipc	ra,0xdff9e
200657a8:	f94080e7          	jalr	ra,-108(ra) # 3738 <FLASH_CalibrationPLLSel>
200657ac:	dff9e097          	auipc	ra,0xdff9e
200657b0:	fa0080e7          	jalr	ra,-96(ra) # 374c <FLASH_Calibration_PSPLL_Open>
200657b4:	06344503          	lbu	a0,99(s0)
200657b8:	891d                	c.andi	a0,7
200657ba:	dff9e097          	auipc	ra,0xdff9e
200657be:	070080e7          	jalr	ra,112(ra) # 382a <FLASH_CalibrationPLLPS_Shift>
200657c2:	4100c737          	lui	a4,0x4100c
200657c6:	25c72783          	lw	a5,604(a4) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
200657ca:	668d                	c.lui	a3,0x3
200657cc:	8fd5                	c.or	a5,a3
200657ce:	24f72e23          	sw	a5,604(a4)
200657d2:	4100c6b7          	lui	a3,0x4100c
200657d6:	470d                	c.li	a4,3
200657d8:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
200657dc:	8b8d                	c.andi	a5,3
200657de:	fee79de3          	bne	a5,a4,200657d8 <FLASH_ClockSwitch+0x118>
200657e2:	06344783          	lbu	a5,99(s0)
200657e6:	cf8d                	c.beqz	a5,20065820 <FLASH_ClockSwitch+0x160>
200657e8:	4505                	c.li	a0,1
200657ea:	dff9e097          	auipc	ra,0xdff9e
200657ee:	190080e7          	jalr	ra,400(ra) # 397a <FLASH_CalibrationNewCmd>
200657f2:	4505                	c.li	a0,1
200657f4:	e091                	c.bnez	s1,200657f8 <FLASH_ClockSwitch+0x138>
200657f6:	4501                	c.li	a0,0
200657f8:	dffa0097          	auipc	ra,0xdffa0
200657fc:	246080e7          	jalr	ra,582(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065800:	05c44783          	lbu	a5,92(s0)
20065804:	04f40ea3          	sb	a5,93(s0)
20065808:	b789                	c.j	2006574a <FLASH_ClockSwitch+0x8a>
2006580a:	4505                	c.li	a0,1
2006580c:	dff9e097          	auipc	ra,0xdff9e
20065810:	f2c080e7          	jalr	ra,-212(ra) # 3738 <FLASH_CalibrationPLLSel>
20065814:	dff9e097          	auipc	ra,0xdff9e
20065818:	f38080e7          	jalr	ra,-200(ra) # 374c <FLASH_Calibration_PSPLL_Open>
2006581c:	4501                	c.li	a0,0
2006581e:	b759                	c.j	200657a4 <FLASH_ClockSwitch+0xe4>
20065820:	05c44503          	lbu	a0,92(s0)
20065824:	4585                	c.li	a1,1
20065826:	1579                	c.addi	a0,-2
20065828:	dff9e097          	auipc	ra,0xdff9e
2006582c:	356080e7          	jalr	ra,854(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
20065830:	b7c9                	c.j	200657f2 <FLASH_ClockSwitch+0x132>
20065832:	40f2                	c.lwsp	ra,28(sp)
20065834:	4462                	c.lwsp	s0,24(sp)
20065836:	44d2                	c.lwsp	s1,20(sp)
20065838:	4942                	c.lwsp	s2,16(sp)
2006583a:	49b2                	c.lwsp	s3,12(sp)
2006583c:	4a22                	c.lwsp	s4,8(sp)
2006583e:	4a92                	c.lwsp	s5,4(sp)
20065840:	6105                	c.addi16sp	sp,32
20065842:	8082                	c.jr	ra

20065844 <FLASH_UserMode_Exit>:
20065844:	4100a737          	lui	a4,0x4100a
20065848:	431c                	c.lw	a5,0(a4)
2006584a:	0786                	c.slli	a5,0x1
2006584c:	8385                	c.srli	a5,0x1
2006584e:	c31c                	c.sw	a5,0(a4)
20065850:	431c                	c.lw	a5,0(a4)
20065852:	fe07cfe3          	blt	a5,zero,20065850 <FLASH_UserMode_Exit+0xc>
20065856:	0ff0000f          	fence	iorw,iorw
2006585a:	0001                	c.addi	zero,0
2006585c:	0001                	c.addi	zero,0
2006585e:	0001                	c.addi	zero,0
20065860:	0001                	c.addi	zero,0
20065862:	0001                	c.addi	zero,0
20065864:	0001                	c.addi	zero,0
20065866:	0001                	c.addi	zero,0
20065868:	0001                	c.addi	zero,0
2006586a:	0001                	c.addi	zero,0
2006586c:	0001                	c.addi	zero,0
2006586e:	0001                	c.addi	zero,0
20065870:	8082                	c.jr	ra

20065872 <FLASH_WaitBusy_InUserMode>:
20065872:	7179                	c.addi16sp	sp,-48
20065874:	d422                	c.swsp	s0,40(sp)
20065876:	20001437          	lui	s0,0x20001
2006587a:	d226                	c.swsp	s1,36(sp)
2006587c:	d04a                	c.swsp	s2,32(sp)
2006587e:	ce4e                	c.swsp	s3,28(sp)
20065880:	cc52                	c.swsp	s4,24(sp)
20065882:	ca56                	c.swsp	s5,20(sp)
20065884:	d606                	c.swsp	ra,44(sp)
20065886:	84aa                	c.mv	s1,a0
20065888:	000107a3          	sb	zero,15(sp)
2006588c:	4781                	c.li	a5,0
2006588e:	4911                	c.li	s2,4
20065890:	4985                	c.li	s3,1
20065892:	4a0d                	c.li	s4,3
20065894:	86440413          	addi	s0,s0,-1948 # 20000864 <flash_init_para>
20065898:	4100aab7          	lui	s5,0x4100a
2006589c:	ec91                	c.bnez	s1,200658b8 <FLASH_WaitBusy_InUserMode+0x46>
2006589e:	028aa783          	lw	a5,40(s5) # 4100a028 <__km4_bd_boot_download_addr__+0x10ff8028>
200658a2:	8b85                	c.andi	a5,1
200658a4:	ffe5                	c.bnez	a5,2006589c <FLASH_WaitBusy_InUserMode+0x2a>
200658a6:	50b2                	c.lwsp	ra,44(sp)
200658a8:	5422                	c.lwsp	s0,40(sp)
200658aa:	5492                	c.lwsp	s1,36(sp)
200658ac:	5902                	c.lwsp	s2,32(sp)
200658ae:	49f2                	c.lwsp	s3,28(sp)
200658b0:	4a62                	c.lwsp	s4,24(sp)
200658b2:	4ad2                	c.lwsp	s5,20(sp)
200658b4:	6145                	c.addi16sp	sp,48
200658b6:	8082                	c.jr	ra
200658b8:	01249563          	bne	s1,s2,200658c2 <FLASH_WaitBusy_InUserMode+0x50>
200658bc:	008aa783          	lw	a5,8(s5)
200658c0:	b7cd                	c.j	200658a2 <FLASH_WaitBusy_InUserMode+0x30>
200658c2:	01349d63          	bne	s1,s3,200658dc <FLASH_WaitBusy_InUserMode+0x6a>
200658c6:	05244503          	lbu	a0,82(s0)
200658ca:	00f10613          	addi	a2,sp,15
200658ce:	4585                	c.li	a1,1
200658d0:	288d                	c.jal	20065942 <FLASH_RxCmd_InUserMode>
200658d2:	00f14783          	lbu	a5,15(sp)
200658d6:	4818                	c.lw	a4,16(s0)
200658d8:	8ff9                	c.and	a5,a4
200658da:	b7e9                	c.j	200658a4 <FLASH_WaitBusy_InUserMode+0x32>
200658dc:	fd4494e3          	bne	s1,s4,200658a4 <FLASH_WaitBusy_InUserMode+0x32>
200658e0:	05244503          	lbu	a0,82(s0)
200658e4:	00f10613          	addi	a2,sp,15
200658e8:	4585                	c.li	a1,1
200658ea:	28a1                	c.jal	20065942 <FLASH_RxCmd_InUserMode>
200658ec:	00f14703          	lbu	a4,15(sp)
200658f0:	4854                	c.lw	a3,20(s0)
200658f2:	4785                	c.li	a5,1
200658f4:	8ef9                	c.and	a3,a4
200658f6:	d6dd                	c.beqz	a3,200658a4 <FLASH_WaitBusy_InUserMode+0x32>
200658f8:	481c                	c.lw	a5,16(s0)
200658fa:	8ff9                	c.and	a5,a4
200658fc:	00f037b3          	sltu	a5,zero,a5
20065900:	b755                	c.j	200658a4 <FLASH_WaitBusy_InUserMode+0x32>

20065902 <FLASH_UserMode_Enter>:
20065902:	1141                	c.addi	sp,-16
20065904:	c606                	c.swsp	ra,12(sp)
20065906:	0ff0000f          	fence	iorw,iorw
2006590a:	0ff0000f          	fence	iorw,iorw
2006590e:	0001                	c.addi	zero,0
20065910:	0001                	c.addi	zero,0
20065912:	0001                	c.addi	zero,0
20065914:	0001                	c.addi	zero,0
20065916:	0001                	c.addi	zero,0
20065918:	0001                	c.addi	zero,0
2006591a:	0001                	c.addi	zero,0
2006591c:	0001                	c.addi	zero,0
2006591e:	0001                	c.addi	zero,0
20065920:	0001                	c.addi	zero,0
20065922:	0001                	c.addi	zero,0
20065924:	4501                	c.li	a0,0
20065926:	37b1                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065928:	4100a737          	lui	a4,0x4100a
2006592c:	431c                	c.lw	a5,0(a4)
2006592e:	800006b7          	lui	a3,0x80000
20065932:	8fd5                	c.or	a5,a3
20065934:	c31c                	c.sw	a5,0(a4)
20065936:	431c                	c.lw	a5,0(a4)
20065938:	fe07dfe3          	bge	a5,zero,20065936 <FLASH_UserMode_Enter+0x34>
2006593c:	40b2                	c.lwsp	ra,12(sp)
2006593e:	0141                	c.addi	sp,16
20065940:	8082                	c.jr	ra

20065942 <FLASH_RxCmd_InUserMode>:
20065942:	1101                	c.addi	sp,-32
20065944:	cc22                	c.swsp	s0,24(sp)
20065946:	20001437          	lui	s0,0x20001
2006594a:	ca26                	c.swsp	s1,20(sp)
2006594c:	c64e                	c.swsp	s3,12(sp)
2006594e:	84ae                	c.mv	s1,a1
20065950:	89aa                	c.mv	s3,a0
20065952:	4581                	c.li	a1,0
20065954:	86440513          	addi	a0,s0,-1948 # 20000864 <flash_init_para>
20065958:	c84a                	c.swsp	s2,16(sp)
2006595a:	ce06                	c.swsp	ra,28(sp)
2006595c:	8932                	c.mv	s2,a2
2006595e:	dff9d097          	auipc	ra,0xdff9d
20065962:	366080e7          	jalr	ra,870(ra) # 2cc4 <FLASH_SetSpiMode>
20065966:	4100a7b7          	lui	a5,0x4100a
2006596a:	10078613          	addi	a2,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
2006596e:	4e18                	c.lw	a4,24(a2)
20065970:	fff106b7          	lui	a3,0xfff10
20065974:	fff68593          	addi	a1,a3,-1 # fff0ffff <__ctrace_end__+0x35f0ffff>
20065978:	8f6d                	c.and	a4,a1
2006597a:	ce18                	c.sw	a4,24(a2)
2006597c:	00849713          	slli	a4,s1,0x8
20065980:	8321                	c.srli	a4,0x8
20065982:	c3d8                	c.sw	a4,4(a5)
20065984:	1207a823          	sw	zero,304(a5)
20065988:	4398                	c.lw	a4,0(a5)
2006598a:	cff68693          	addi	a3,a3,-769
2006598e:	8f75                	c.and	a4,a3
20065990:	30076713          	ori	a4,a4,768
20065994:	c398                	c.sw	a4,0(a5)
20065996:	07378023          	sb	s3,96(a5)
2006599a:	4705                	c.li	a4,1
2006599c:	c798                	c.sw	a4,8(a5)
2006599e:	4100a6b7          	lui	a3,0x4100a
200659a2:	4781                	c.li	a5,0
200659a4:	02979463          	bne	a5,s1,200659cc <FLASH_RxCmd_InUserMode+0x8a>
200659a8:	4511                	c.li	a0,4
200659aa:	35e1                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
200659ac:	86440793          	addi	a5,s0,-1948
200659b0:	86440513          	addi	a0,s0,-1948
200659b4:	4462                	c.lwsp	s0,24(sp)
200659b6:	40f2                	c.lwsp	ra,28(sp)
200659b8:	44d2                	c.lwsp	s1,20(sp)
200659ba:	4942                	c.lwsp	s2,16(sp)
200659bc:	49b2                	c.lwsp	s3,12(sp)
200659be:	0047c583          	lbu	a1,4(a5)
200659c2:	6105                	c.addi16sp	sp,32
200659c4:	dff9d317          	auipc	t1,0xdff9d
200659c8:	30030067          	jalr	zero,768(t1) # 2cc4 <FLASH_SetSpiMode>
200659cc:	5698                	c.lw	a4,40(a3)
200659ce:	8b21                	c.andi	a4,8
200659d0:	db71                	c.beqz	a4,200659a4 <FLASH_RxCmd_InUserMode+0x62>
200659d2:	0606c603          	lbu	a2,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
200659d6:	00f90733          	add	a4,s2,a5
200659da:	0785                	c.addi	a5,1
200659dc:	00c70023          	sb	a2,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200659e0:	b7d1                	c.j	200659a4 <FLASH_RxCmd_InUserMode+0x62>

200659e2 <FLASH_RxCmd>:
200659e2:	1101                	c.addi	sp,-32
200659e4:	ce06                	c.swsp	ra,28(sp)
200659e6:	c62a                	c.swsp	a0,12(sp)
200659e8:	c42e                	c.swsp	a1,8(sp)
200659ea:	c232                	c.swsp	a2,4(sp)
200659ec:	3f19                	c.jal	20065902 <FLASH_UserMode_Enter>
200659ee:	4612                	c.lwsp	a2,4(sp)
200659f0:	45a2                	c.lwsp	a1,8(sp)
200659f2:	4532                	c.lwsp	a0,12(sp)
200659f4:	37b9                	c.jal	20065942 <FLASH_RxCmd_InUserMode>
200659f6:	40f2                	c.lwsp	ra,28(sp)
200659f8:	6105                	c.addi16sp	sp,32
200659fa:	b5a9                	c.j	20065844 <FLASH_UserMode_Exit>

200659fc <FLASH_TxCmd_InUserMode>:
200659fc:	4100a7b7          	lui	a5,0x4100a
20065a00:	1141                	c.addi	sp,-16
20065a02:	c606                	c.swsp	ra,12(sp)
20065a04:	c422                	c.swsp	s0,8(sp)
20065a06:	c226                	c.swsp	s1,4(sp)
20065a08:	c04a                	c.swsp	s2,0(sp)
20065a0a:	10078713          	addi	a4,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
20065a0e:	01872903          	lw	s2,24(a4)
20065a12:	4384                	c.lw	s1,0(a5)
20065a14:	4394                	c.lw	a3,0(a5)
20065a16:	ffc10837          	lui	a6,0xffc10
20065a1a:	cff80813          	addi	a6,a6,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
20065a1e:	0106f6b3          	and	a3,a3,a6
20065a22:	c394                	c.sw	a3,0(a5)
20065a24:	4f14                	c.lw	a3,24(a4)
20065a26:	fff10837          	lui	a6,0xfff10
20065a2a:	187d                	c.addi	a6,-1
20065a2c:	0106f6b3          	and	a3,a3,a6
20065a30:	cf14                	c.sw	a3,24(a4)
20065a32:	0007a223          	sw	zero,4(a5)
20065a36:	db0c                	c.sw	a1,48(a4)
20065a38:	06a78023          	sb	a0,96(a5)
20065a3c:	4100a737          	lui	a4,0x4100a
20065a40:	4781                	c.li	a5,0
20065a42:	02b7e163          	bltu	a5,a1,20065a64 <FLASH_TxCmd_InUserMode+0x68>
20065a46:	4100a437          	lui	s0,0x4100a
20065a4a:	4785                	c.li	a5,1
20065a4c:	c41c                	c.sw	a5,8(s0)
20065a4e:	4511                	c.li	a0,4
20065a50:	350d                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065a52:	11242c23          	sw	s2,280(s0) # 4100a118 <__km4_bd_boot_download_addr__+0x10ff8118>
20065a56:	c004                	c.sw	s1,0(s0)
20065a58:	40b2                	c.lwsp	ra,12(sp)
20065a5a:	4422                	c.lwsp	s0,8(sp)
20065a5c:	4492                	c.lwsp	s1,4(sp)
20065a5e:	4902                	c.lwsp	s2,0(sp)
20065a60:	0141                	c.addi	sp,16
20065a62:	8082                	c.jr	ra
20065a64:	00f606b3          	add	a3,a2,a5
20065a68:	0006c683          	lbu	a3,0(a3)
20065a6c:	0785                	c.addi	a5,1
20065a6e:	06d70023          	sb	a3,96(a4) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20065a72:	bfc1                	c.j	20065a42 <FLASH_TxCmd_InUserMode+0x46>

20065a74 <FLASH_WriteEn_InUserMode>:
20065a74:	1141                	c.addi	sp,-16
20065a76:	4505                	c.li	a0,1
20065a78:	c606                	c.swsp	ra,12(sp)
20065a7a:	3be5                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065a7c:	200017b7          	lui	a5,0x20001
20065a80:	8b47c503          	lbu	a0,-1868(a5) # 200008b4 <flash_init_para+0x50>
20065a84:	4601                	c.li	a2,0
20065a86:	4581                	c.li	a1,0
20065a88:	3f95                	c.jal	200659fc <FLASH_TxCmd_InUserMode>
20065a8a:	40b2                	c.lwsp	ra,12(sp)
20065a8c:	450d                	c.li	a0,3
20065a8e:	0141                	c.addi	sp,16
20065a90:	b3cd                	c.j	20065872 <FLASH_WaitBusy_InUserMode>

20065a92 <FLASH_TxData>:
20065a92:	1101                	c.addi	sp,-32
20065a94:	0ff57793          	andi	a5,a0,255
20065a98:	cc22                	c.swsp	s0,24(sp)
20065a9a:	ca26                	c.swsp	s1,20(sp)
20065a9c:	c84a                	c.swsp	s2,16(sp)
20065a9e:	ce06                	c.swsp	ra,28(sp)
20065aa0:	c64e                	c.swsp	s3,12(sp)
20065aa2:	97ae                	c.add	a5,a1
20065aa4:	10000713          	addi	a4,zero,256
20065aa8:	842a                	c.mv	s0,a0
20065aaa:	84ae                	c.mv	s1,a1
20065aac:	8932                	c.mv	s2,a2
20065aae:	00f77a63          	bgeu	a4,a5,20065ac2 <FLASH_TxData+0x30>
20065ab2:	20062537          	lui	a0,0x20062
20065ab6:	23a00593          	addi	a1,zero,570
20065aba:	9d450513          	addi	a0,a0,-1580 # 200619d4 <strtol+0xe>
20065abe:	b31f90ef          	jal	ra,2005f5ee <io_assert_failed>
20065ac2:	3581                	c.jal	20065902 <FLASH_UserMode_Enter>
20065ac4:	3f45                	c.jal	20065a74 <FLASH_WriteEn_InUserMode>
20065ac6:	4100a7b7          	lui	a5,0x4100a
20065aca:	0007a983          	lw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20065ace:	4398                	c.lw	a4,0(a5)
20065ad0:	ffc106b7          	lui	a3,0xffc10
20065ad4:	cff68693          	addi	a3,a3,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
20065ad8:	8f75                	c.and	a4,a3
20065ada:	200016b7          	lui	a3,0x20001
20065ade:	c398                	c.sw	a4,0(a5)
20065ae0:	86468693          	addi	a3,a3,-1948 # 20000864 <flash_init_para>
20065ae4:	0646c703          	lbu	a4,100(a3)
20065ae8:	1187a603          	lw	a2,280(a5)
20065aec:	000f05b7          	lui	a1,0xf0
20065af0:	0742                	c.slli	a4,0x10
20065af2:	8f6d                	c.and	a4,a1
20065af4:	fff105b7          	lui	a1,0xfff10
20065af8:	15fd                	c.addi	a1,-1
20065afa:	8e6d                	c.and	a2,a1
20065afc:	8f51                	c.or	a4,a2
20065afe:	10e7ac23          	sw	a4,280(a5)
20065b02:	00849713          	slli	a4,s1,0x8
20065b06:	0007a223          	sw	zero,4(a5)
20065b0a:	8321                	c.srli	a4,0x8
20065b0c:	12e7a823          	sw	a4,304(a5)
20065b10:	4709                	c.li	a4,2
20065b12:	06e78023          	sb	a4,96(a5)
20065b16:	0646c683          	lbu	a3,100(a3)
20065b1a:	4711                	c.li	a4,4
20065b1c:	00e69663          	bne	a3,a4,20065b28 <FLASH_TxData+0x96>
20065b20:	01845713          	srli	a4,s0,0x18
20065b24:	06e78023          	sb	a4,96(a5)
20065b28:	01045713          	srli	a4,s0,0x10
20065b2c:	4100a7b7          	lui	a5,0x4100a
20065b30:	0ff77713          	andi	a4,a4,255
20065b34:	06e78023          	sb	a4,96(a5) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20065b38:	00845713          	srli	a4,s0,0x8
20065b3c:	0ff77713          	andi	a4,a4,255
20065b40:	06e78023          	sb	a4,96(a5)
20065b44:	0ff47413          	andi	s0,s0,255
20065b48:	06878023          	sb	s0,96(a5)
20065b4c:	4100a6b7          	lui	a3,0x4100a
20065b50:	4781                	c.li	a5,0
20065b52:	00978563          	beq	a5,s1,20065b5c <FLASH_TxData+0xca>
20065b56:	5698                	c.lw	a4,40(a3)
20065b58:	8b09                	c.andi	a4,2
20065b5a:	eb05                	c.bnez	a4,20065b8a <FLASH_TxData+0xf8>
20065b5c:	4685                	c.li	a3,1
20065b5e:	4100a737          	lui	a4,0x4100a
20065b62:	c714                	c.sw	a3,8(a4)
20065b64:	4100a6b7          	lui	a3,0x4100a
20065b68:	0297e963          	bltu	a5,s1,20065b9a <FLASH_TxData+0x108>
20065b6c:	4511                	c.li	a0,4
20065b6e:	3311                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065b70:	4100a7b7          	lui	a5,0x4100a
20065b74:	0137a023          	sw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20065b78:	4505                	c.li	a0,1
20065b7a:	39e5                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065b7c:	4462                	c.lwsp	s0,24(sp)
20065b7e:	40f2                	c.lwsp	ra,28(sp)
20065b80:	44d2                	c.lwsp	s1,20(sp)
20065b82:	4942                	c.lwsp	s2,16(sp)
20065b84:	49b2                	c.lwsp	s3,12(sp)
20065b86:	6105                	c.addi16sp	sp,32
20065b88:	b975                	c.j	20065844 <FLASH_UserMode_Exit>
20065b8a:	00f90733          	add	a4,s2,a5
20065b8e:	00074703          	lbu	a4,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20065b92:	0785                	c.addi	a5,1
20065b94:	06e68023          	sb	a4,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20065b98:	bf6d                	c.j	20065b52 <FLASH_TxData+0xc0>
20065b9a:	5698                	c.lw	a4,40(a3)
20065b9c:	8b09                	c.andi	a4,2
20065b9e:	d769                	c.beqz	a4,20065b68 <FLASH_TxData+0xd6>
20065ba0:	00f90733          	add	a4,s2,a5
20065ba4:	00074703          	lbu	a4,0(a4)
20065ba8:	0785                	c.addi	a5,1
20065baa:	06e68023          	sb	a4,96(a3)
20065bae:	bf6d                	c.j	20065b68 <FLASH_TxData+0xd6>

20065bb0 <FLASH_SetStatus>:
20065bb0:	1101                	c.addi	sp,-32
20065bb2:	ce06                	c.swsp	ra,28(sp)
20065bb4:	cc22                	c.swsp	s0,24(sp)
20065bb6:	c62a                	c.swsp	a0,12(sp)
20065bb8:	842e                	c.mv	s0,a1
20065bba:	c432                	c.swsp	a2,8(sp)
20065bbc:	3399                	c.jal	20065902 <FLASH_UserMode_Enter>
20065bbe:	3d5d                	c.jal	20065a74 <FLASH_WriteEn_InUserMode>
20065bc0:	4622                	c.lwsp	a2,8(sp)
20065bc2:	4532                	c.lwsp	a0,12(sp)
20065bc4:	0ff47593          	andi	a1,s0,255
20065bc8:	3d15                	c.jal	200659fc <FLASH_TxCmd_InUserMode>
20065bca:	4505                	c.li	a0,1
20065bcc:	315d                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065bce:	4462                	c.lwsp	s0,24(sp)
20065bd0:	40f2                	c.lwsp	ra,28(sp)
20065bd2:	6105                	c.addi16sp	sp,32
20065bd4:	b985                	c.j	20065844 <FLASH_UserMode_Exit>

20065bd6 <FLASH_SetStatusBits>:
20065bd6:	7179                	c.addi16sp	sp,-48
20065bd8:	d606                	c.swsp	ra,44(sp)
20065bda:	d422                	c.swsp	s0,40(sp)
20065bdc:	d226                	c.swsp	s1,36(sp)
20065bde:	ce4e                	c.swsp	s3,28(sp)
20065be0:	842a                	c.mv	s0,a0
20065be2:	89ae                	c.mv	s3,a1
20065be4:	200014b7          	lui	s1,0x20001
20065be8:	d04a                	c.swsp	s2,32(sp)
20065bea:	3b21                	c.jal	20065902 <FLASH_UserMode_Enter>
20065bec:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
20065bf0:	0527c503          	lbu	a0,82(a5)
20065bf4:	0070                	c.addi4spn	a2,sp,12
20065bf6:	4585                	c.li	a1,1
20065bf8:	33a9                	c.jal	20065942 <FLASH_RxCmd_InUserMode>
20065bfa:	4785                	c.li	a5,1
20065bfc:	00c14703          	lbu	a4,12(sp)
20065c00:	86448493          	addi	s1,s1,-1948
20065c04:	04f99d63          	bne	s3,a5,20065c5e <FLASH_SetStatusBits+0x88>
20065c08:	00e467b3          	or	a5,s0,a4
20065c0c:	0ff7f793          	andi	a5,a5,255
20065c10:	00f10623          	sb	a5,12(sp)
20065c14:	4c9c                	c.lw	a5,24(s1)
20065c16:	4905                	c.li	s2,1
20065c18:	c385                	c.beqz	a5,20065c38 <FLASH_SetStatusBits+0x62>
20065c1a:	0534c503          	lbu	a0,83(s1)
20065c1e:	00d10613          	addi	a2,sp,13
20065c22:	4585                	c.li	a1,1
20065c24:	3b39                	c.jal	20065942 <FLASH_RxCmd_InUserMode>
20065c26:	00d14503          	lbu	a0,13(sp)
20065c2a:	8021                	c.srli	s0,0x8
20065c2c:	03299d63          	bne	s3,s2,20065c66 <FLASH_SetStatusBits+0x90>
20065c30:	8c49                	c.or	s0,a0
20065c32:	008106a3          	sb	s0,13(sp)
20065c36:	4909                	c.li	s2,2
20065c38:	0554c783          	lbu	a5,85(s1)
20065c3c:	eb8d                	c.bnez	a5,20065c6e <FLASH_SetStatusBits+0x98>
20065c3e:	3d1d                	c.jal	20065a74 <FLASH_WriteEn_InUserMode>
20065c40:	0544c503          	lbu	a0,84(s1)
20065c44:	0070                	c.addi4spn	a2,sp,12
20065c46:	85ca                	c.mv	a1,s2
20065c48:	3b55                	c.jal	200659fc <FLASH_TxCmd_InUserMode>
20065c4a:	4505                	c.li	a0,1
20065c4c:	311d                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065c4e:	3edd                	c.jal	20065844 <FLASH_UserMode_Exit>
20065c50:	50b2                	c.lwsp	ra,44(sp)
20065c52:	5422                	c.lwsp	s0,40(sp)
20065c54:	5492                	c.lwsp	s1,36(sp)
20065c56:	5902                	c.lwsp	s2,32(sp)
20065c58:	49f2                	c.lwsp	s3,28(sp)
20065c5a:	6145                	c.addi16sp	sp,48
20065c5c:	8082                	c.jr	ra
20065c5e:	fff44793          	xori	a5,s0,-1
20065c62:	8ff9                	c.and	a5,a4
20065c64:	b765                	c.j	20065c0c <FLASH_SetStatusBits+0x36>
20065c66:	fff44413          	xori	s0,s0,-1
20065c6a:	8c69                	c.and	s0,a0
20065c6c:	b7d9                	c.j	20065c32 <FLASH_SetStatusBits+0x5c>
20065c6e:	3519                	c.jal	20065a74 <FLASH_WriteEn_InUserMode>
20065c70:	0544c503          	lbu	a0,84(s1)
20065c74:	0070                	c.addi4spn	a2,sp,12
20065c76:	4585                	c.li	a1,1
20065c78:	3351                	c.jal	200659fc <FLASH_TxCmd_InUserMode>
20065c7a:	4505                	c.li	a0,1
20065c7c:	3edd                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065c7e:	3bdd                	c.jal	20065a74 <FLASH_WriteEn_InUserMode>
20065c80:	0554c503          	lbu	a0,85(s1)
20065c84:	00d10613          	addi	a2,sp,13
20065c88:	4585                	c.li	a1,1
20065c8a:	bf7d                	c.j	20065c48 <FLASH_SetStatusBits+0x72>

20065c8c <FLASH_Erase>:
20065c8c:	7179                	c.addi16sp	sp,-48
20065c8e:	d422                	c.swsp	s0,40(sp)
20065c90:	d04a                	c.swsp	s2,32(sp)
20065c92:	d606                	c.swsp	ra,44(sp)
20065c94:	d226                	c.swsp	s1,36(sp)
20065c96:	4789                	c.li	a5,2
20065c98:	892a                	c.mv	s2,a0
20065c9a:	842e                	c.mv	s0,a1
20065c9c:	00a7fa63          	bgeu	a5,a0,20065cb0 <FLASH_Erase+0x24>
20065ca0:	20062537          	lui	a0,0x20062
20065ca4:	2d700593          	addi	a1,zero,727
20065ca8:	9d450513          	addi	a0,a0,-1580 # 200619d4 <strtol+0xe>
20065cac:	943f90ef          	jal	ra,2005f5ee <io_assert_failed>
20065cb0:	200014b7          	lui	s1,0x20001
20065cb4:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
20065cb8:	0647c583          	lbu	a1,100(a5)
20065cbc:	00845713          	srli	a4,s0,0x8
20065cc0:	01045793          	srli	a5,s0,0x10
20065cc4:	4611                	c.li	a2,4
20065cc6:	0ff7f793          	andi	a5,a5,255
20065cca:	0ff77713          	andi	a4,a4,255
20065cce:	0ff47693          	andi	a3,s0,255
20065cd2:	86448493          	addi	s1,s1,-1948
20065cd6:	04c59463          	bne	a1,a2,20065d1e <FLASH_Erase+0x92>
20065cda:	01845713          	srli	a4,s0,0x18
20065cde:	00f10ea3          	sb	a5,29(sp)
20065ce2:	00841793          	slli	a5,s0,0x8
20065ce6:	0442                	c.slli	s0,0x10
20065ce8:	8041                	c.srli	s0,0x10
20065cea:	8021                	c.srli	s0,0x8
20065cec:	8c5d                	c.or	s0,a5
20065cee:	00e10e23          	sb	a4,28(sp)
20065cf2:	00811f23          	sh	s0,30(sp)
20065cf6:	c62e                	c.swsp	a1,12(sp)
20065cf8:	3129                	c.jal	20065902 <FLASH_UserMode_Enter>
20065cfa:	3bad                	c.jal	20065a74 <FLASH_WriteEn_InUserMode>
20065cfc:	45b2                	c.lwsp	a1,12(sp)
20065cfe:	02091863          	bne	s2,zero,20065d2e <FLASH_Erase+0xa2>
20065d02:	0564c503          	lbu	a0,86(s1)
20065d06:	4601                	c.li	a2,0
20065d08:	4581                	c.li	a1,0
20065d0a:	39cd                	c.jal	200659fc <FLASH_TxCmd_InUserMode>
20065d0c:	4505                	c.li	a0,1
20065d0e:	3695                	c.jal	20065872 <FLASH_WaitBusy_InUserMode>
20065d10:	3e15                	c.jal	20065844 <FLASH_UserMode_Exit>
20065d12:	50b2                	c.lwsp	ra,44(sp)
20065d14:	5422                	c.lwsp	s0,40(sp)
20065d16:	5492                	c.lwsp	s1,36(sp)
20065d18:	5902                	c.lwsp	s2,32(sp)
20065d1a:	6145                	c.addi16sp	sp,48
20065d1c:	8082                	c.jr	ra
20065d1e:	00f10e23          	sb	a5,28(sp)
20065d22:	00e10ea3          	sb	a4,29(sp)
20065d26:	00d10f23          	sb	a3,30(sp)
20065d2a:	458d                	c.li	a1,3
20065d2c:	b7e9                	c.j	20065cf6 <FLASH_Erase+0x6a>
20065d2e:	4785                	c.li	a5,1
20065d30:	00f91663          	bne	s2,a5,20065d3c <FLASH_Erase+0xb0>
20065d34:	0574c503          	lbu	a0,87(s1)
20065d38:	0870                	c.addi4spn	a2,sp,28
20065d3a:	bfc1                	c.j	20065d0a <FLASH_Erase+0x7e>
20065d3c:	4789                	c.li	a5,2
20065d3e:	fcf917e3          	bne	s2,a5,20065d0c <FLASH_Erase+0x80>
20065d42:	0584c503          	lbu	a0,88(s1)
20065d46:	0870                	c.addi4spn	a2,sp,28
20065d48:	b7c9                	c.j	20065d0a <FLASH_Erase+0x7e>

20065d4a <plic_get_pending>:
20065d4a:	f1402773          	csrrs	a4,mhartid,zero
20065d4e:	00251793          	slli	a5,a0,0x2
20065d52:	80001537          	lui	a0,0x80001
20065d56:	c0050513          	addi	a0,a0,-1024 # 80000c00 <__ctrace_end__+0xb6000c00>
20065d5a:	97aa                	c.add	a5,a0
20065d5c:	0722                	c.slli	a4,0x8
20065d5e:	97ba                	c.add	a5,a4
20065d60:	4388                	c.lw	a0,0(a5)
20065d62:	8082                	c.jr	ra

20065d64 <plic_interrupt_handler>:
20065d64:	7179                	c.addi16sp	sp,-48
20065d66:	d422                	c.swsp	s0,40(sp)
20065d68:	d226                	c.swsp	s1,36(sp)
20065d6a:	ce4e                	c.swsp	s3,28(sp)
20065d6c:	d606                	c.swsp	ra,44(sp)
20065d6e:	d04a                	c.swsp	s2,32(sp)
20065d70:	0044                	c.addi4spn	s1,sp,4
20065d72:	4401                	c.li	s0,0
20065d74:	498d                	c.li	s3,3
20065d76:	8522                	c.mv	a0,s0
20065d78:	3fc9                	c.jal	20065d4a <plic_get_pending>
20065d7a:	892a                	c.mv	s2,a0
20065d7c:	8522                	c.mv	a0,s0
20065d7e:	dffa0097          	auipc	ra,0xdffa0
20065d82:	9a4080e7          	jalr	ra,-1628(ra) # 5722 <plic_get_enable>
20065d86:	c088                	c.sw	a0,0(s1)
20065d88:	00a975b3          	and	a1,s2,a0
20065d8c:	8522                	c.mv	a0,s0
20065d8e:	0405                	c.addi	s0,1
20065d90:	dffa0097          	auipc	ra,0xdffa0
20065d94:	97a080e7          	jalr	ra,-1670(ra) # 570a <plic_set_enable>
20065d98:	0491                	c.addi	s1,4
20065d9a:	fd341ee3          	bne	s0,s3,20065d76 <plic_interrupt_handler+0x12>
20065d9e:	0ff0000f          	fence	iorw,iorw
20065da2:	dffa0097          	auipc	ra,0xdffa0
20065da6:	a34080e7          	jalr	ra,-1484(ra) # 57d6 <plic_claim_irq>
20065daa:	4592                	c.lwsp	a1,4(sp)
20065dac:	eac18493          	addi	s1,gp,-340 # 20066128 <plic_irq_id>
20065db0:	c088                	c.sw	a0,0(s1)
20065db2:	4501                	c.li	a0,0
20065db4:	dffa0097          	auipc	ra,0xdffa0
20065db8:	956080e7          	jalr	ra,-1706(ra) # 570a <plic_set_enable>
20065dbc:	45a2                	c.lwsp	a1,8(sp)
20065dbe:	4505                	c.li	a0,1
20065dc0:	eac18413          	addi	s0,gp,-340 # 20066128 <plic_irq_id>
20065dc4:	dffa0097          	auipc	ra,0xdffa0
20065dc8:	946080e7          	jalr	ra,-1722(ra) # 570a <plic_set_enable>
20065dcc:	45b2                	c.lwsp	a1,12(sp)
20065dce:	4509                	c.li	a0,2
20065dd0:	dffa0097          	auipc	ra,0xdffa0
20065dd4:	93a080e7          	jalr	ra,-1734(ra) # 570a <plic_set_enable>
20065dd8:	4084                	c.lw	s1,0(s1)
20065dda:	c4b1                	c.beqz	s1,20065e26 <plic_interrupt_handler+0xc2>
20065ddc:	02048493          	addi	s1,s1,32
20065de0:	8526                	c.mv	a0,s1
20065de2:	77e000e7          	jalr	ra,1918(zero) # 0 <Reset_Handler>
20065de6:	892a                	c.mv	s2,a0
20065de8:	c531                	c.beqz	a0,20065e34 <plic_interrupt_handler+0xd0>
20065dea:	dffa0097          	auipc	ra,0xdffa0
20065dee:	a28080e7          	jalr	ra,-1496(ra) # 5812 <plic_get_threshold>
20065df2:	89aa                	c.mv	s3,a0
20065df4:	4008                	c.lw	a0,0(s0)
20065df6:	dffa0097          	auipc	ra,0xdffa0
20065dfa:	9b0080e7          	jalr	ra,-1616(ra) # 57a6 <plic_get_irq_priority>
20065dfe:	dffa0097          	auipc	ra,0xdffa0
20065e02:	9fc080e7          	jalr	ra,-1540(ra) # 57fa <plic_set_threshold>
20065e06:	8526                	c.mv	a0,s1
20065e08:	7b2000e7          	jalr	ra,1970(zero) # 0 <Reset_Handler>
20065e0c:	9902                	c.jalr	s2
20065e0e:	854e                	c.mv	a0,s3
20065e10:	dffa0097          	auipc	ra,0xdffa0
20065e14:	9ea080e7          	jalr	ra,-1558(ra) # 57fa <plic_set_threshold>
20065e18:	4008                	c.lw	a0,0(s0)
20065e1a:	dffa0097          	auipc	ra,0xdffa0
20065e1e:	9ce080e7          	jalr	ra,-1586(ra) # 57e8 <plic_complete_irq>
20065e22:	00042023          	sw	zero,0(s0)
20065e26:	50b2                	c.lwsp	ra,44(sp)
20065e28:	5422                	c.lwsp	s0,40(sp)
20065e2a:	5492                	c.lwsp	s1,36(sp)
20065e2c:	5902                	c.lwsp	s2,32(sp)
20065e2e:	49f2                	c.lwsp	s3,28(sp)
20065e30:	6145                	c.addi16sp	sp,48
20065e32:	8082                	c.jr	ra
20065e34:	200626b7          	lui	a3,0x20062
20065e38:	200625b7          	lui	a1,0x20062
20065e3c:	8726                	c.mv	a4,s1
20065e3e:	ec468693          	addi	a3,a3,-316 # 20061ec4 <pmap_func+0xd4>
20065e42:	05700613          	addi	a2,zero,87
20065e46:	ee858593          	addi	a1,a1,-280 # 20061ee8 <pmap_func+0xf8>
20065e4a:	450d                	c.li	a0,3
20065e4c:	86cf50ef          	jal	ra,2005aeb8 <rtk_log_write>
20065e50:	b7e1                	c.j	20065e18 <plic_interrupt_handler+0xb4>

20065e52 <plic_get_active_irq_id>:
20065e52:	eac1a503          	lw	a0,-340(gp) # 20066128 <plic_irq_id>
20065e56:	8082                	c.jr	ra

20065e58 <__NVIC_GetActive_IRQ>:
20065e58:	06f00793          	addi	a5,zero,111
20065e5c:	00a7d463          	bge	a5,a0,20065e64 <__NVIC_GetActive_IRQ+0xc>
20065e60:	4501                	c.li	a0,0
20065e62:	8082                	c.jr	ra
20065e64:	34202773          	csrrs	a4,mcause,zero
20065e68:	00171793          	slli	a5,a4,0x1
20065e6c:	8385                	c.srli	a5,0x1
20065e6e:	00055763          	bge	a0,zero,20065e7c <__NVIC_GetActive_IRQ+0x24>
20065e72:	0541                	c.addi	a0,16
20065e74:	fe0746e3          	blt	a4,zero,20065e60 <__NVIC_GetActive_IRQ+0x8>
20065e78:	8d1d                	c.sub	a0,a5
20065e7a:	a821                	c.j	20065e92 <__NVIC_GetActive_IRQ+0x3a>
20065e7c:	fe0752e3          	bge	a4,zero,20065e60 <__NVIC_GetActive_IRQ+0x8>
20065e80:	477d                	c.li	a4,31
20065e82:	fea75be3          	bge	a4,a0,20065e78 <__NVIC_GetActive_IRQ+0x20>
20065e86:	eac1a783          	lw	a5,-340(gp) # 20066128 <plic_irq_id>
20065e8a:	02078793          	addi	a5,a5,32
20065e8e:	40a78533          	sub	a0,a5,a0
20065e92:	00153513          	sltiu	a0,a0,1
20065e96:	8082                	c.jr	ra

20065e98 <__sram_image2_end__>:
	...

20065ea0 <Flash_Layout>:
20065ea0:	0000 0000 0000 0800 3fff 0801 0002 0000     .........?......
20065eb0:	4000 0801 3fff 081f 0001 0000 0000 0820     .@...?........ .
20065ec0:	3fff 0821 0003 0000 4000 0821 3fff 083f     .?!......@!..??.
20065ed0:	0004 0000 0000 0840 ffff 086f 0005 0000     ......@...o.....
20065ee0:	0000 0870 2fff 0870 0006 0000 3000 0870     ..p../p......0p.
20065ef0:	2fff 0872 0008 0000 ffff ffff ffff ffff     ./r.............
20065f00:	00ff 0000 ffff ffff ffff ffff               ............

20065f0c <PSRAM_CALIB_PATTERN>:
20065f0c:	3344 1122 a55a a55a 5aa5 5aa5 2211 4433     D3".Z.Z..Z.Z."3D
20065f1c:	6996 9669 9669 6996                         .ii.i..i

20065f24 <at_sys_items>:
20065f24:	2824 2006 9c9e 2005 0000 0000 0000 0000     $(. ... ........
20065f34:	2830 2006 9cbe 2005 0000 0000 0000 0000     0(. ... ........
20065f44:	283c 2006 a47e 2005 0000 0000 0000 0000     <(. ~.. ........
20065f54:	2848 2006 9c8a 2005 0000 0000 0000 0000     H(. ... ........
20065f64:	2850 2006 a70e 2005 0000 0000 0000 0000     P(. ... ........
20065f74:	2858 2006 9cde 2005 0000 0000 0000 0000     X(. ... ........
20065f84:	2860 2006 9cfe 2005 0000 0000 0000 0000     `(. ... ........
20065f94:	2868 2006 9d98 2005 0000 0000 0000 0000     h(. ... ........
20065fa4:	2870 2006 9e28 2005 0000 0000 0000 0000     p(. (.. ........
20065fb4:	2878 2006 a010 2005 0000 0000 0000 0000     x(. ... ........
20065fc4:	2880 2006 a096 2005 0000 0000 0000 0000     .(. ... ........

20065fd4 <impure_data>:
20065fd4:	0000 0000 80f4 2006 815c 2006 81c4 2006     ....... \.. ... 
	...

200660e0 <__sdata_start__>:
200660e0:	0000                	c.unimp
200660e2:	42c8                	c.lw	a0,4(a3)

200660e4 <xISRStackTop>:
200660e4:	7820 2006                                    x. 

200660e8 <CSWTCH.248>:
200660e8:	5258 5342 0044 0000                         XRBSD...

200660f0 <SystemCoreClock>:
200660f0:	d180 08f0                                   ....

200660f4 <PXID_Idx>:
200660f4:	0101 0101                                   ....

200660f8 <system_can_yield>:
200660f8:	0001 0000                                   ....

200660fc <wakelock>:
200660fc:	0001 0000                                   ....

20066100 <rtk_log_default_level>:
20066100:	0004 0000                                   ....

20066104 <log_init_table>:
20066104:	a738 2005                                   8.. 

20066108 <mbedtls_calloc_func>:
20066108:	bc7a 2005                                   z.. 

2006610c <mbedtls_free_func>:
2006610c:	bc7e 2005                                   ~.. 

20066110 <pullNextTime>:
20066110:	61c8 2006                                   .a. 

20066114 <uxTopUsedPriority>:
20066114:	000a 0000                                   ....

20066118 <crash_task_info>:
20066118:	eac2 2005                                   ... 

2006611c <_impure_ptr>:
2006611c:	5fd4 2006                                   ._. 
