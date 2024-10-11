
/home/auto_jenkins_wlan5/IOT_ATS/repos/release-master/source/amebalite_gcc_project/project_kr4/vsdk/image_mp/target_img2.axf:     file format elf32-littleriscv


Disassembly of section .xip_image2.text:

20058060 <BOOT_InitGP>:
20058060:	0000f197          	auipc	gp,0xf
20058064:	95018193          	addi	gp,gp,-1712 # 200669b0 <__global_pointer$>
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
2005833c:	510010ef          	jal	ra,2005984c <PSRAM_calibration>
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
20058a08:	0340206f          	jal	zero,2005aa3c <BOOT_Image1>

20058a0c <app_start>:
20058a0c:	1141                	c.addi	sp,-16
20058a0e:	c606                	c.swsp	ra,12(sp)
20058a10:	e50ff0ef          	jal	ra,20058060 <BOOT_InitGP>
20058a14:	00018793          	addi	a5,gp,0 # 200669b0 <__global_pointer$>
20058a18:	e9018613          	addi	a2,gp,-368 # 20066840 <PrevIrqStatus>
20058a1c:	40c78633          	sub	a2,a5,a2
20058a20:	4581                	c.li	a1,0
20058a22:	e9018513          	addi	a0,gp,-368 # 20066840 <PrevIrqStatus>
20058a26:	dffb1097          	auipc	ra,0xdffb1
20058a2a:	d64080e7          	jalr	ra,-668(ra) # 978a <_memset>
20058a2e:	200697b7          	lui	a5,0x20069
20058a32:	01018613          	addi	a2,gp,16 # 200669c0 <ipc_Semaphore>
20058a36:	9b478793          	addi	a5,a5,-1612 # 200689b4 <__bss_end__>
20058a3a:	40c78633          	sub	a2,a5,a2
20058a3e:	4581                	c.li	a1,0
20058a40:	01018513          	addi	a0,gp,16 # 200669c0 <ipc_Semaphore>
20058a44:	dffb1097          	auipc	ra,0xdffb1
20058a48:	d46080e7          	jalr	ra,-698(ra) # 978a <_memset>
20058a4c:	252060ef          	jal	ra,2005ec9e <ChipInfo_GetChipSram>
20058a50:	c919                	c.beqz	a0,20058a66 <app_start+0x5a>
20058a52:	000405b7          	lui	a1,0x40
20058a56:	20080537          	lui	a0,0x20080
20058a5a:	00058593          	addi	a1,a1,0 # 40000 <__ap_sram_heap_size>
20058a5e:	00050513          	addi	a0,a0,0 # 20080000 <__ap_sram_heap_start>
20058a62:	112020ef          	jal	ra,2005ab74 <os_heap_add>
20058a66:	473020ef          	jal	ra,2005b6d8 <rtos_mem_init>
20058a6a:	2005c7b7          	lui	a5,0x2005c
20058a6e:	e6878793          	addi	a5,a5,-408 # 2005be68 <freertos_risc_v_trap_handler>
20058a72:	9bf1                	c.andi	a5,-4
20058a74:	30579073          	csrrw	zero,mtvec,a5
20058a78:	2831                	c.jal	20058a94 <SystemCoreClockUpdate>
20058a7a:	dffae097          	auipc	ra,0xdffae
20058a7e:	24c080e7          	jalr	ra,588(ra) # 6cc6 <SYSTIMER_Init>
20058a82:	2c5000ef          	jal	ra,20059546 <pinmap_init>
20058a86:	0c4090ef          	jal	ra,20061b4a <__libc_init_array>
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
20058aa2:	e6a1a023          	sw	a0,-416(gp) # 20066810 <SystemCoreClock>
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
20058ae8:	27d0d0ef          	jal	ra,20066564 <plic_get_active_irq_id>
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
20058b04:	c1018693          	addi	a3,gp,-1008 # 200665c0 <Flash_Layout>
20058b08:	4701                	c.li	a4,0
20058b0a:	c1018793          	addi	a5,gp,-1008 # 200665c0 <Flash_Layout>
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
20058b4a:	e8a1a823          	sw	a0,-368(gp) # 20066840 <PrevIrqStatus>
20058b4e:	55fd                	c.li	a1,-1
20058b50:	4505                	c.li	a0,1
20058b52:	2d55                	c.jal	20059206 <IPC_SEMTake>
20058b54:	dd6d                	c.beqz	a0,20058b4e <FLASH_Write_Lock+0xa>
20058b56:	40b2                	c.lwsp	ra,12(sp)
20058b58:	0141                	c.addi	sp,16
20058b5a:	8082                	c.jr	ra

20058b5c <FLASH_Write_Unlock>:
20058b5c:	1141                	c.addi	sp,-16
20058b5e:	c606                	c.swsp	ra,12(sp)
20058b60:	4505                	c.li	a0,1
20058b62:	792000ef          	jal	ra,200592f4 <IPC_SEMFree>
20058b66:	dd6d                	c.beqz	a0,20058b60 <FLASH_Write_Unlock+0x4>
20058b68:	40b2                	c.lwsp	ra,12(sp)
20058b6a:	e901a503          	lw	a0,-368(gp) # 20066840 <PrevIrqStatus>
20058b6e:	0141                	c.addi	sp,16
20058b70:	b779                	c.j	20058afe <irq_enable_restore>

20058b72 <FLASH_ReadStream>:
20058b72:	1141                	c.addi	sp,-16
20058b74:	c606                	c.swsp	ra,12(sp)
20058b76:	c422                	c.swsp	s0,8(sp)
20058b78:	c226                	c.swsp	s1,4(sp)
20058b7a:	c04a                	c.swsp	s2,0(sp)
20058b7c:	ea09                	c.bnez	a2,20058b8e <FLASH_ReadStream+0x1c>
20058b7e:	20062537          	lui	a0,0x20062
20058b82:	0c200593          	addi	a1,zero,194
20058b86:	fb050513          	addi	a0,a0,-80 # 20061fb0 <strtol+0xc>
20058b8a:	57f060ef          	jal	ra,2005f908 <io_assert_failed>
20058b8e:	84aa                	c.mv	s1,a0
20058b90:	892e                	c.mv	s2,a1
20058b92:	8432                	c.mv	s0,a2
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
20058c00:	5b20d0ef          	jal	ra,200661b2 <FLASH_TxData>
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
20058cb0:	6fc0d0ef          	jal	ra,200663ac <FLASH_Erase>
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
20058d84:	cc4d8793          	addi	a5,s11,-828 # 20064cc4 <ipc_tickless_table>
20058d88:	d6c48493          	addi	s1,s1,-660 # 20064d6c <lib_pmc_git_rev>
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
20058db0:	cc4d8d93          	addi	s11,s11,-828
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
20058e44:	262d                	c.jal	2005916e <IPC_IERGet>
20058e46:	47a2                	c.lwsp	a5,8(sp)
20058e48:	4732                	c.lwsp	a4,12(sp)
20058e4a:	00f996b3          	sll	a3,s3,a5
20058e4e:	8ee9                	c.and	a3,a0
20058e50:	ca8d                	c.beqz	a3,20058e82 <ipc_table_init+0x10c>
20058e52:	228b2703          	lw	a4,552(s6) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
20058e56:	058b8693          	addi	a3,s7,88 # 20062058 <strtol+0xb4>
20058e5a:	04900613          	addi	a2,zero,73
20058e5e:	8319                	c.srli	a4,0x6
20058e60:	8b0d                	c.andi	a4,3
20058e62:	0a0c0593          	addi	a1,s8,160 # 200620a0 <strtol+0xfc>
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
20058e94:	2699                	c.jal	200591da <IPC_INTUserHandler>
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
20058ec0:	2969                	c.jal	2005935a <IPC_GetDevById>
20058ec2:	4601                	c.li	a2,0
20058ec4:	0ff47593          	andi	a1,s0,255
20058ec8:	2469                	c.jal	20059152 <IPC_INTConfig>
20058eca:	040a                	c.slli	s0,0x2
20058ecc:	01018613          	addi	a2,gp,16 # 200669c0 <ipc_Semaphore>
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
20058f0c:	0a450513          	addi	a0,a0,164 # 200620a4 <strtol+0x100>
20058f10:	c606                	c.swsp	ra,12(sp)
20058f12:	4f0060ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
20058f38:	0d018713          	addi	a4,gp,208 # 20066a80 <IPC_IrqHandler>
20058f3c:	974e                	c.add	a4,s3
20058f3e:	431c                	c.lw	a5,0(a4)
20058f40:	c79d                	c.beqz	a5,20058f6e <IPC_wait_idle+0x50>
20058f42:	01018513          	addi	a0,gp,16 # 200669c0 <ipc_Semaphore>
20058f46:	954e                	c.add	a0,s3
20058f48:	4118                	c.lw	a4,0(a0)
20058f4a:	01018413          	addi	s0,gp,16 # 200669c0 <ipc_Semaphore>
20058f4e:	e319                	c.bnez	a4,20058f54 <IPC_wait_idle+0x36>
20058f50:	17b020ef          	jal	ra,2005b8ca <rtos_sema_create_binary>
20058f54:	0ff97a13          	andi	s4,s2,255
20058f58:	85d2                	c.mv	a1,s4
20058f5a:	8526                	c.mv	a0,s1
20058f5c:	4605                	c.li	a2,1
20058f5e:	2ad5                	c.jal	20059152 <IPC_INTConfig>
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
20058f86:	0bc50513          	addi	a0,a0,188 # 200620bc <strtol+0x118>
20058f8a:	478060ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20058f8e:	8526                	c.mv	a0,s1
20058f90:	85d2                	c.mv	a1,s4
20058f92:	4601                	c.li	a2,0
20058f94:	2a7d                	c.jal	20059152 <IPC_INTConfig>
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
20058fc2:	cb81                	c.beqz	a5,20058fd2 <ipc_send_message+0x2a>
20058fc4:	fe050793          	addi	a5,a0,-32
20058fc8:	4705                	c.li	a4,1
20058fca:	0ac00593          	addi	a1,zero,172
20058fce:	00f76763          	bltu	a4,a5,20058fdc <ipc_send_message+0x34>
20058fd2:	479d                	c.li	a5,7
20058fd4:	0157fa63          	bgeu	a5,s5,20058fe8 <ipc_send_message+0x40>
20058fd8:	0ad00593          	addi	a1,zero,173
20058fdc:	20062537          	lui	a0,0x20062
20058fe0:	0dc50513          	addi	a0,a0,220 # 200620dc <strtol+0x138>
20058fe4:	125060ef          	jal	ra,2005f908 <io_assert_failed>
20058fe8:	02100793          	addi	a5,zero,33
20058fec:	0d37eb63          	bltu	a5,s3,200590c2 <ipc_send_message+0x11a>
20058ff0:	200627b7          	lui	a5,0x20062
20058ff4:	1b078793          	addi	a5,a5,432 # 200621b0 <CSWTCH.29>
20058ff8:	97ce                	c.add	a5,s3
20058ffa:	20062737          	lui	a4,0x20062
20058ffe:	0007c583          	lbu	a1,0(a5)
20059002:	1d470713          	addi	a4,a4,468 # 200621d4 <CSWTCH.30>
20059006:	00299793          	slli	a5,s3,0x2
2005900a:	97ba                	c.add	a5,a4
2005900c:	4384                	c.lw	s1,0(a5)
2005900e:	409c                	c.lw	a5,0(s1)
20059010:	95d6                	c.add	a1,s5
20059012:	4405                	c.li	s0,1
20059014:	00b41433          	sll	s0,s0,a1
20059018:	8fe1                	c.and	a5,s0
2005901a:	e7dd                	c.bnez	a5,200590c8 <ipc_send_message+0x120>
2005901c:	3ce9                	c.jal	20058af6 <irq_disable_save>
2005901e:	409c                	c.lw	a5,0(s1)
20059020:	892a                	c.mv	s2,a0
20059022:	8fe1                	c.and	a5,s0
20059024:	cb89                	c.beqz	a5,20059036 <ipc_send_message+0x8e>
20059026:	20062537          	lui	a0,0x20062
2005902a:	8622                	c.mv	a2,s0
2005902c:	85ce                	c.mv	a1,s3
2005902e:	18450513          	addi	a0,a0,388 # 20062184 <strtol+0x1e0>
20059032:	3d0060ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059036:	080a0163          	beq	s4,zero,200590b8 <ipc_send_message+0x110>
2005903a:	0039d793          	srli	a5,s3,0x3
2005903e:	8bf9                	c.andi	a5,30
20059040:	00f9f993          	andi	s3,s3,15
20059044:	97ce                	c.add	a5,s3
20059046:	078e                	c.slli	a5,0x3
20059048:	97d6                	c.add	a5,s5
2005904a:	20005737          	lui	a4,0x20005
2005904e:	0792                	c.slli	a5,0x4
20059050:	90070713          	addi	a4,a4,-1792 # 20004900 <__kr4_ipc_memory_start__>
20059054:	00e789b3          	add	s3,a5,a4
20059058:	4641                	c.li	a2,16
2005905a:	85d2                	c.mv	a1,s4
2005905c:	854e                	c.mv	a0,s3
2005905e:	dffb0097          	auipc	ra,0xdffb0
20059062:	5b4080e7          	jalr	ra,1460(ra) # 9612 <_memcpy>
20059066:	7c102773          	csrrs	a4,0x7c1,zero
2005906a:	10077713          	andi	a4,a4,256
2005906e:	c729                	c.beqz	a4,200590b8 <ipc_send_message+0x110>
20059070:	01f9f713          	andi	a4,s3,31
20059074:	87ce                	c.mv	a5,s3
20059076:	46c1                	c.li	a3,16
20059078:	c709                	c.beqz	a4,20059082 <ipc_send_message+0xda>
2005907a:	01070693          	addi	a3,a4,16
2005907e:	fe09f793          	andi	a5,s3,-32
20059082:	01f7f713          	andi	a4,a5,31
20059086:	9736                	c.add	a4,a3
20059088:	0ff0000f          	fence	iorw,iorw
2005908c:	973e                	c.add	a4,a5
2005908e:	04f7f00b          	cache	dwb,(a5)
20059092:	02078793          	addi	a5,a5,32
20059096:	40f706b3          	sub	a3,a4,a5
2005909a:	fed04ae3          	blt	zero,a3,2005908e <ipc_send_message+0xe6>
2005909e:	0ff0000f          	fence	iorw,iorw
200590a2:	0001                	c.addi	zero,0
200590a4:	0001                	c.addi	zero,0
200590a6:	0001                	c.addi	zero,0
200590a8:	0001                	c.addi	zero,0
200590aa:	0001                	c.addi	zero,0
200590ac:	0001                	c.addi	zero,0
200590ae:	0001                	c.addi	zero,0
200590b0:	0001                	c.addi	zero,0
200590b2:	0001                	c.addi	zero,0
200590b4:	0001                	c.addi	zero,0
200590b6:	0001                	c.addi	zero,0
200590b8:	c080                	c.sw	s0,0(s1)
200590ba:	854a                	c.mv	a0,s2
200590bc:	3489                	c.jal	20058afe <irq_enable_restore>
200590be:	4781                	c.li	a5,0
200590c0:	a801                	c.j	200590d0 <ipc_send_message+0x128>
200590c2:	4481                	c.li	s1,0
200590c4:	4581                	c.li	a1,0
200590c6:	b7a1                	c.j	2005900e <ipc_send_message+0x66>
200590c8:	8526                	c.mv	a0,s1
200590ca:	3d91                	c.jal	20058f1e <IPC_wait_idle>
200590cc:	4785                	c.li	a5,1
200590ce:	d539                	c.beqz	a0,2005901c <ipc_send_message+0x74>
200590d0:	40f2                	c.lwsp	ra,28(sp)
200590d2:	4462                	c.lwsp	s0,24(sp)
200590d4:	44d2                	c.lwsp	s1,20(sp)
200590d6:	4942                	c.lwsp	s2,16(sp)
200590d8:	49b2                	c.lwsp	s3,12(sp)
200590da:	4a22                	c.lwsp	s4,8(sp)
200590dc:	4a92                	c.lwsp	s5,4(sp)
200590de:	853e                	c.mv	a0,a5
200590e0:	6105                	c.addi16sp	sp,32
200590e2:	8082                	c.jr	ra

200590e4 <ipc_get_message>:
200590e4:	00355793          	srli	a5,a0,0x3
200590e8:	8bf9                	c.andi	a5,30
200590ea:	893d                	c.andi	a0,15
200590ec:	953e                	c.add	a0,a5
200590ee:	050e                	c.slli	a0,0x3
200590f0:	952e                	c.add	a0,a1
200590f2:	200055b7          	lui	a1,0x20005
200590f6:	90058593          	addi	a1,a1,-1792 # 20004900 <__kr4_ipc_memory_start__>
200590fa:	0512                	c.slli	a0,0x4
200590fc:	952e                	c.add	a0,a1
200590fe:	7c1027f3          	csrrs	a5,0x7c1,zero
20059102:	1007f793          	andi	a5,a5,256
20059106:	c7a9                	c.beqz	a5,20059150 <ipc_get_message+0x6c>
20059108:	01f57713          	andi	a4,a0,31
2005910c:	87aa                	c.mv	a5,a0
2005910e:	46c1                	c.li	a3,16
20059110:	c709                	c.beqz	a4,2005911a <ipc_get_message+0x36>
20059112:	01070693          	addi	a3,a4,16
20059116:	fe057793          	andi	a5,a0,-32
2005911a:	01f7f713          	andi	a4,a5,31
2005911e:	9736                	c.add	a4,a3
20059120:	0ff0000f          	fence	iorw,iorw
20059124:	973e                	c.add	a4,a5
20059126:	00f7f00b          	cache	dinv,(a5)
2005912a:	02078793          	addi	a5,a5,32
2005912e:	40f706b3          	sub	a3,a4,a5
20059132:	fed04ae3          	blt	zero,a3,20059126 <ipc_get_message+0x42>
20059136:	0ff0000f          	fence	iorw,iorw
2005913a:	0001                	c.addi	zero,0
2005913c:	0001                	c.addi	zero,0
2005913e:	0001                	c.addi	zero,0
20059140:	0001                	c.addi	zero,0
20059142:	0001                	c.addi	zero,0
20059144:	0001                	c.addi	zero,0
20059146:	0001                	c.addi	zero,0
20059148:	0001                	c.addi	zero,0
2005914a:	0001                	c.addi	zero,0
2005914c:	0001                	c.addi	zero,0
2005914e:	0001                	c.addi	zero,0
20059150:	8082                	c.jr	ra

20059152 <IPC_INTConfig>:
20059152:	4785                	c.li	a5,1
20059154:	00b795b3          	sll	a1,a5,a1
20059158:	00f61663          	bne	a2,a5,20059164 <IPC_INTConfig+0x12>
2005915c:	455c                	c.lw	a5,12(a0)
2005915e:	8ddd                	c.or	a1,a5
20059160:	c54c                	c.sw	a1,12(a0)
20059162:	8082                	c.jr	ra
20059164:	455c                	c.lw	a5,12(a0)
20059166:	fff5c593          	xori	a1,a1,-1
2005916a:	8dfd                	c.and	a1,a5
2005916c:	bfd5                	c.j	20059160 <IPC_INTConfig+0xe>

2005916e <IPC_IERGet>:
2005916e:	4548                	c.lw	a0,12(a0)
20059170:	8082                	c.jr	ra

20059172 <IPC_INTHandler>:
20059172:	7179                	c.addi16sp	sp,-48
20059174:	d422                	c.swsp	s0,40(sp)
20059176:	d226                	c.swsp	s1,36(sp)
20059178:	d04a                	c.swsp	s2,32(sp)
2005917a:	ce4e                	c.swsp	s3,28(sp)
2005917c:	c85a                	c.swsp	s6,16(sp)
2005917e:	c65e                	c.swsp	s7,12(sp)
20059180:	d606                	c.swsp	ra,44(sp)
20059182:	cc52                	c.swsp	s4,24(sp)
20059184:	ca56                	c.swsp	s5,20(sp)
20059186:	00852a83          	lw	s5,8(a0)
2005918a:	89aa                	c.mv	s3,a0
2005918c:	05018913          	addi	s2,gp,80 # 20066a00 <IPC_IrqData>
20059190:	0d018493          	addi	s1,gp,208 # 20066a80 <IPC_IrqHandler>
20059194:	4401                	c.li	s0,0
20059196:	4b85                	c.li	s7,1
20059198:	02000b13          	addi	s6,zero,32
2005919c:	008b9a33          	sll	s4,s7,s0
200591a0:	015a77b3          	and	a5,s4,s5
200591a4:	cb91                	c.beqz	a5,200591b8 <IPC_INTHandler+0x46>
200591a6:	409c                	c.lw	a5,0(s1)
200591a8:	c791                	c.beqz	a5,200591b4 <IPC_INTHandler+0x42>
200591aa:	00092503          	lw	a0,0(s2)
200591ae:	8622                	c.mv	a2,s0
200591b0:	85d6                	c.mv	a1,s5
200591b2:	9782                	c.jalr	a5
200591b4:	0149a423          	sw	s4,8(s3)
200591b8:	0405                	c.addi	s0,1
200591ba:	0911                	c.addi	s2,4
200591bc:	0491                	c.addi	s1,4
200591be:	fd641fe3          	bne	s0,s6,2005919c <IPC_INTHandler+0x2a>
200591c2:	50b2                	c.lwsp	ra,44(sp)
200591c4:	5422                	c.lwsp	s0,40(sp)
200591c6:	5492                	c.lwsp	s1,36(sp)
200591c8:	5902                	c.lwsp	s2,32(sp)
200591ca:	49f2                	c.lwsp	s3,28(sp)
200591cc:	4a62                	c.lwsp	s4,24(sp)
200591ce:	4ad2                	c.lwsp	s5,20(sp)
200591d0:	4b42                	c.lwsp	s6,16(sp)
200591d2:	4bb2                	c.lwsp	s7,12(sp)
200591d4:	4501                	c.li	a0,0
200591d6:	6145                	c.addi16sp	sp,48
200591d8:	8082                	c.jr	ra

200591da <IPC_INTUserHandler>:
200591da:	c60d                	c.beqz	a2,20059204 <IPC_INTUserHandler+0x2a>
200591dc:	00259793          	slli	a5,a1,0x2
200591e0:	0d018713          	addi	a4,gp,208 # 20066a80 <IPC_IrqHandler>
200591e4:	883e                	c.mv	a6,a5
200591e6:	973e                	c.add	a4,a5
200591e8:	05018793          	addi	a5,gp,80 # 20066a00 <IPC_IrqData>
200591ec:	97c2                	c.add	a5,a6
200591ee:	c394                	c.sw	a3,0(a5)
200591f0:	c310                	c.sw	a2,0(a4)
200591f2:	47bd                	c.li	a5,15
200591f4:	00b7f863          	bgeu	a5,a1,20059204 <IPC_INTUserHandler+0x2a>
200591f8:	4558                	c.lw	a4,12(a0)
200591fa:	4785                	c.li	a5,1
200591fc:	00b795b3          	sll	a1,a5,a1
20059200:	8dd9                	c.or	a1,a4
20059202:	c54c                	c.sw	a1,12(a0)
20059204:	8082                	c.jr	ra

20059206 <IPC_SEMTake>:
20059206:	7179                	c.addi16sp	sp,-48
20059208:	d226                	c.swsp	s1,36(sp)
2005920a:	d606                	c.swsp	ra,44(sp)
2005920c:	d422                	c.swsp	s0,40(sp)
2005920e:	d04a                	c.swsp	s2,32(sp)
20059210:	ce4e                	c.swsp	s3,28(sp)
20059212:	cc52                	c.swsp	s4,24(sp)
20059214:	ca56                	c.swsp	s5,20(sp)
20059216:	4100c7b7          	lui	a5,0x4100c
2005921a:	2287aa03          	lw	s4,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005921e:	e6418793          	addi	a5,gp,-412 # 20066814 <PXID_Idx>
20059222:	97aa                	c.add	a5,a0
20059224:	0007c783          	lbu	a5,0(a5)
20059228:	c79d                	c.beqz	a5,20059256 <IPC_SEMTake+0x50>
2005922a:	00251793          	slli	a5,a0,0x2
2005922e:	410209b7          	lui	s3,0x41020
20059232:	892a                	c.mv	s2,a0
20059234:	842e                	c.mv	s0,a1
20059236:	e6418493          	addi	s1,gp,-412 # 20066814 <PXID_Idx>
2005923a:	99be                	c.add	s3,a5
2005923c:	0149a783          	lw	a5,20(s3) # 41020014 <__km4_bd_boot_download_addr__+0x1100e014>
20059240:	83a1                	c.srli	a5,0x8
20059242:	8b8d                	c.andi	a5,3
20059244:	c38d                	c.beqz	a5,20059266 <IPC_SEMTake+0x60>
20059246:	cc2d                	c.beqz	s0,200592c0 <IPC_SEMTake+0xba>
20059248:	e941a783          	lw	a5,-364(gp) # 20066844 <ipc_delay>
2005924c:	c399                	c.beqz	a5,20059252 <IPC_SEMTake+0x4c>
2005924e:	4505                	c.li	a0,1
20059250:	9782                	c.jalr	a5
20059252:	147d                	c.addi	s0,-1
20059254:	b7e5                	c.j	2005923c <IPC_SEMTake+0x36>
20059256:	20062537          	lui	a0,0x20062
2005925a:	0bf00593          	addi	a1,zero,191
2005925e:	25c50513          	addi	a0,a0,604 # 2006225c <CSWTCH.30+0x88>
20059262:	6a6060ef          	jal	ra,2005f908 <io_assert_failed>
20059266:	006a5593          	srli	a1,s4,0x6
2005926a:	898d                	c.andi	a1,3
2005926c:	c62e                	c.swsp	a1,12(sp)
2005926e:	889ff0ef          	jal	ra,20058af6 <irq_disable_save>
20059272:	01248733          	add	a4,s1,s2
20059276:	0149a783          	lw	a5,20(s3)
2005927a:	00074683          	lbu	a3,0(a4)
2005927e:	45b2                	c.lwsp	a1,12(sp)
20059280:	842a                	c.mv	s0,a0
20059282:	8fd5                	c.or	a5,a3
20059284:	4007e793          	ori	a5,a5,1024
20059288:	00f9aa23          	sw	a5,20(s3)
2005928c:	0149a703          	lw	a4,20(s3)
20059290:	0149a783          	lw	a5,20(s3)
20059294:	00158613          	addi	a2,a1,1
20059298:	8321                	c.srli	a4,0x8
2005929a:	8b0d                	c.andi	a4,3
2005929c:	0ff7f793          	andi	a5,a5,255
200592a0:	00e61463          	bne	a2,a4,200592a8 <IPC_SEMTake+0xa2>
200592a4:	02f68863          	beq	a3,a5,200592d4 <IPC_SEMTake+0xce>
200592a8:	20062537          	lui	a0,0x20062
200592ac:	177d                	c.addi	a4,-1
200592ae:	864a                	c.mv	a2,s2
200592b0:	30450513          	addi	a0,a0,772 # 20062304 <CSWTCH.30+0x130>
200592b4:	14e060ef          	jal	ra,2005f402 <DiagPrintf_minimal>
200592b8:	8522                	c.mv	a0,s0
200592ba:	845ff0ef          	jal	ra,20058afe <irq_enable_restore>
200592be:	4401                	c.li	s0,0
200592c0:	50b2                	c.lwsp	ra,44(sp)
200592c2:	8522                	c.mv	a0,s0
200592c4:	5422                	c.lwsp	s0,40(sp)
200592c6:	5492                	c.lwsp	s1,36(sp)
200592c8:	5902                	c.lwsp	s2,32(sp)
200592ca:	49f2                	c.lwsp	s3,28(sp)
200592cc:	4a62                	c.lwsp	s4,24(sp)
200592ce:	4ad2                	c.lwsp	s5,20(sp)
200592d0:	6145                	c.addi16sp	sp,48
200592d2:	8082                	c.jr	ra
200592d4:	0ff00713          	addi	a4,zero,255
200592d8:	4785                	c.li	a5,1
200592da:	00e68563          	beq	a3,a4,200592e4 <IPC_SEMTake+0xde>
200592de:	0685                	c.addi	a3,1
200592e0:	0ff6f793          	andi	a5,a3,255
200592e4:	94ca                	c.add	s1,s2
200592e6:	8522                	c.mv	a0,s0
200592e8:	00f48023          	sb	a5,0(s1)
200592ec:	813ff0ef          	jal	ra,20058afe <irq_enable_restore>
200592f0:	4405                	c.li	s0,1
200592f2:	b7f9                	c.j	200592c0 <IPC_SEMTake+0xba>

200592f4 <IPC_SEMFree>:
200592f4:	4100c7b7          	lui	a5,0x4100c
200592f8:	2287a583          	lw	a1,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
200592fc:	410206b7          	lui	a3,0x41020
20059300:	00251793          	slli	a5,a0,0x2
20059304:	96be                	c.add	a3,a5
20059306:	4ad8                	c.lw	a4,20(a3)
20059308:	8199                	c.srli	a1,0x6
2005930a:	898d                	c.andi	a1,3
2005930c:	8321                	c.srli	a4,0x8
2005930e:	4adc                	c.lw	a5,20(a3)
20059310:	862a                	c.mv	a2,a0
20059312:	8b0d                	c.andi	a4,3
20059314:	00158513          	addi	a0,a1,1
20059318:	02e50663          	beq	a0,a4,20059344 <IPC_SEMFree+0x50>
2005931c:	1141                	c.addi	sp,-16
2005931e:	e6418693          	addi	a3,gp,-412 # 20066814 <PXID_Idx>
20059322:	c606                	c.swsp	ra,12(sp)
20059324:	96b2                	c.add	a3,a2
20059326:	0006c683          	lbu	a3,0(a3) # 41020000 <__km4_bd_boot_download_addr__+0x1100e000>
2005932a:	20062537          	lui	a0,0x20062
2005932e:	0ff7f793          	andi	a5,a5,255
20059332:	177d                	c.addi	a4,-1
20059334:	35050513          	addi	a0,a0,848 # 20062350 <CSWTCH.30+0x17c>
20059338:	0ca060ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005933c:	40b2                	c.lwsp	ra,12(sp)
2005933e:	4501                	c.li	a0,0
20059340:	0141                	c.addi	sp,16
20059342:	8082                	c.jr	ra
20059344:	4adc                	c.lw	a5,20(a3)
20059346:	6705                	c.lui	a4,0x1
20059348:	80070713          	addi	a4,a4,-2048 # 800 <__NVIC_GetVectorArg+0x4e>
2005934c:	8fd9                	c.or	a5,a4
2005934e:	cadc                	c.sw	a5,20(a3)
20059350:	4505                	c.li	a0,1
20059352:	8082                	c.jr	ra

20059354 <IPC_SEMDelayStub>:
20059354:	e8a1aa23          	sw	a0,-364(gp) # 20066844 <ipc_delay>
20059358:	8082                	c.jr	ra

2005935a <IPC_GetDevById>:
2005935a:	4789                	c.li	a5,2
2005935c:	00a7fc63          	bgeu	a5,a0,20059374 <IPC_GetDevById+0x1a>
20059360:	20062537          	lui	a0,0x20062
20059364:	1141                	c.addi	sp,-16
20059366:	11b00593          	addi	a1,zero,283
2005936a:	25c50513          	addi	a0,a0,604 # 2006225c <CSWTCH.30+0x88>
2005936e:	c606                	c.swsp	ra,12(sp)
20059370:	598060ef          	jal	ra,2005f908 <io_assert_failed>
20059374:	4705                	c.li	a4,1
20059376:	00e50763          	beq	a0,a4,20059384 <IPC_GetDevById+0x2a>
2005937a:	00f50a63          	beq	a0,a5,2005938e <IPC_GetDevById+0x34>
2005937e:	41020537          	lui	a0,0x41020
20059382:	8082                	c.jr	ra
20059384:	41020537          	lui	a0,0x41020
20059388:	08050513          	addi	a0,a0,128 # 41020080 <__km4_bd_boot_download_addr__+0x1100e080>
2005938c:	8082                	c.jr	ra
2005938e:	41020537          	lui	a0,0x41020
20059392:	10050513          	addi	a0,a0,256 # 41020100 <__km4_bd_boot_download_addr__+0x1100e100>
20059396:	8082                	c.jr	ra

20059398 <dsp_status_on>:
20059398:	4100c7b7          	lui	a5,0x4100c
2005939c:	2107a503          	lw	a0,528(a5) # 4100c210 <__km4_bd_boot_download_addr__+0x10ffa210>
200593a0:	404007b7          	lui	a5,0x40400
200593a4:	8d7d                	c.and	a0,a5
200593a6:	00a03533          	sltu	a0,zero,a0
200593aa:	8082                	c.jr	ra

200593ac <ota_get_cur_index>:
200593ac:	47b1                	c.li	a5,12
200593ae:	02f50733          	mul	a4,a0,a5
200593b2:	4100d7b7          	lui	a5,0x4100d
200593b6:	1101                	c.addi	sp,-32
200593b8:	a0078793          	addi	a5,a5,-1536 # 4100ca00 <__km4_bd_boot_download_addr__+0x10ffaa00>
200593bc:	ca26                	c.swsp	s1,20(sp)
200593be:	c84a                	c.swsp	s2,16(sp)
200593c0:	c64e                	c.swsp	s3,12(sp)
200593c2:	ce06                	c.swsp	ra,28(sp)
200593c4:	cc22                	c.swsp	s0,24(sp)
200593c6:	97ba                	c.add	a5,a4
200593c8:	1087a403          	lw	s0,264(a5)
200593cc:	00351993          	slli	s3,a0,0x3
200593d0:	15018793          	addi	a5,gp,336 # 20066b00 <IMG_ADDR>
200593d4:	97ce                	c.add	a5,s3
200593d6:	439c                	c.lw	a5,0(a5)
200593d8:	892a                	c.mv	s2,a0
200593da:	15018493          	addi	s1,gp,336 # 20066b00 <IMG_ADDR>
200593de:	e7a9                	c.bnez	a5,20059428 <ota_get_cur_index+0x7c>
200593e0:	4601                	c.li	a2,0
200593e2:	15018593          	addi	a1,gp,336 # 20066b00 <IMG_ADDR>
200593e6:	4501                	c.li	a0,0
200593e8:	f1cff0ef          	jal	ra,20058b04 <flash_get_layout_info>
200593ec:	4601                	c.li	a2,0
200593ee:	00448593          	addi	a1,s1,4
200593f2:	4505                	c.li	a0,1
200593f4:	f10ff0ef          	jal	ra,20058b04 <flash_get_layout_info>
200593f8:	4601                	c.li	a2,0
200593fa:	00848593          	addi	a1,s1,8
200593fe:	4509                	c.li	a0,2
20059400:	f04ff0ef          	jal	ra,20058b04 <flash_get_layout_info>
20059404:	4601                	c.li	a2,0
20059406:	00c48593          	addi	a1,s1,12
2005940a:	450d                	c.li	a0,3
2005940c:	ef8ff0ef          	jal	ra,20058b04 <flash_get_layout_info>
20059410:	4601                	c.li	a2,0
20059412:	01048593          	addi	a1,s1,16
20059416:	4511                	c.li	a0,4
20059418:	eecff0ef          	jal	ra,20058b04 <flash_get_layout_info>
2005941c:	4601                	c.li	a2,0
2005941e:	01448593          	addi	a1,s1,20
20059422:	4511                	c.li	a0,4
20059424:	ee0ff0ef          	jal	ra,20058b04 <flash_get_layout_info>
20059428:	00147513          	andi	a0,s0,1
2005942c:	cd01                	c.beqz	a0,20059444 <ota_get_cur_index+0x98>
2005942e:	8025                	c.srli	s0,0x9
20059430:	4785                	c.li	a5,1
20059432:	0416                	c.slli	s0,0x5
20059434:	00f91f63          	bne	s2,a5,20059452 <ota_get_cur_index+0xa6>
20059438:	7579                	c.lui	a0,0xffffe
2005943a:	942a                	c.add	s0,a0
2005943c:	4488                	c.lw	a0,8(s1)
2005943e:	8d01                	c.sub	a0,s0
20059440:	00a03533          	sltu	a0,zero,a0
20059444:	40f2                	c.lwsp	ra,28(sp)
20059446:	4462                	c.lwsp	s0,24(sp)
20059448:	44d2                	c.lwsp	s1,20(sp)
2005944a:	4942                	c.lwsp	s2,16(sp)
2005944c:	49b2                	c.lwsp	s3,12(sp)
2005944e:	6105                	c.addi16sp	sp,32
20059450:	8082                	c.jr	ra
20059452:	757d                	c.lui	a0,0xfffff
20059454:	94ce                	c.add	s1,s3
20059456:	942a                	c.add	s0,a0
20059458:	4088                	c.lw	a0,0(s1)
2005945a:	b7d5                	c.j	2005943e <ota_get_cur_index+0x92>

2005945c <OTPPowerSwitch.constprop.0>:
2005945c:	4100c7b7          	lui	a5,0x4100c
20059460:	4398                	c.lw	a4,0(a5)
20059462:	8b09                	c.andi	a4,2
20059464:	e709                	c.bnez	a4,2005946e <OTPPowerSwitch.constprop.0+0x12>
20059466:	4398                	c.lw	a4,0(a5)
20059468:	00276713          	ori	a4,a4,2
2005946c:	c398                	c.sw	a4,0(a5)
2005946e:	41000737          	lui	a4,0x41000
20059472:	4b5c                	c.lw	a5,20(a4)
20059474:	c511                	c.beqz	a0,20059480 <OTPPowerSwitch.constprop.0+0x24>
20059476:	690006b7          	lui	a3,0x69000
2005947a:	8fd5                	c.or	a5,a3
2005947c:	cb5c                	c.sw	a5,20(a4)
2005947e:	8082                	c.jr	ra
20059480:	07a2                	c.slli	a5,0x8
20059482:	83a1                	c.srli	a5,0x8
20059484:	bfe5                	c.j	2005947c <OTPPowerSwitch.constprop.0+0x20>

20059486 <OTP_Read8>:
20059486:	1101                	c.addi	sp,-32
20059488:	c84a                	c.swsp	s2,16(sp)
2005948a:	ce06                	c.swsp	ra,28(sp)
2005948c:	cc22                	c.swsp	s0,24(sp)
2005948e:	ca26                	c.swsp	s1,20(sp)
20059490:	c64e                	c.swsp	s3,12(sp)
20059492:	c452                	c.swsp	s4,8(sp)
20059494:	c256                	c.swsp	s5,4(sp)
20059496:	7ff00793          	addi	a5,zero,2047
2005949a:	892e                	c.mv	s2,a1
2005949c:	04a7ec63          	bltu	a5,a0,200594f4 <OTP_Read8+0x6e>
200594a0:	84aa                	c.mv	s1,a0
200594a2:	4985                	c.li	s3,1
200594a4:	20062a37          	lui	s4,0x20062
200594a8:	3e800593          	addi	a1,zero,1000
200594ac:	4509                	c.li	a0,2
200594ae:	3ba1                	c.jal	20059206 <IPC_SEMTake>
200594b0:	842a                	c.mv	s0,a0
200594b2:	05351f63          	bne	a0,s3,20059510 <OTP_Read8+0x8a>
200594b6:	4501                	c.li	a0,0
200594b8:	3755                	c.jal	2005945c <OTPPowerSwitch.constprop.0>
200594ba:	00849793          	slli	a5,s1,0x8
200594be:	41000737          	lui	a4,0x41000
200594c2:	6995                	c.lui	s3,0x5
200594c4:	c71c                	c.sw	a5,8(a4)
200594c6:	4a01                	c.li	s4,0
200594c8:	41000ab7          	lui	s5,0x41000
200594cc:	e2098993          	addi	s3,s3,-480 # 4e20 <LOGUART_ClearRxFifo+0x80a>
200594d0:	008aa783          	lw	a5,8(s5) # 41000008 <__km4_bd_boot_download_addr__+0x10fee008>
200594d4:	0407ca63          	blt	a5,zero,20059528 <OTP_Read8+0xa2>
200594d8:	053a1163          	bne	s4,s3,2005951a <OTP_Read8+0x94>
200594dc:	57fd                	c.li	a5,-1
200594de:	20062537          	lui	a0,0x20062
200594e2:	00f90023          	sb	a5,0(s2)
200594e6:	85a6                	c.mv	a1,s1
200594e8:	3b850513          	addi	a0,a0,952 # 200623b8 <CSWTCH.30+0x1e4>
200594ec:	717050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
200594f0:	4401                	c.li	s0,0
200594f2:	a089                	c.j	20059534 <OTP_Read8+0xae>
200594f4:	57fd                	c.li	a5,-1
200594f6:	00f58023          	sb	a5,0(a1)
200594fa:	4401                	c.li	s0,0
200594fc:	40f2                	c.lwsp	ra,28(sp)
200594fe:	8522                	c.mv	a0,s0
20059500:	4462                	c.lwsp	s0,24(sp)
20059502:	44d2                	c.lwsp	s1,20(sp)
20059504:	4942                	c.lwsp	s2,16(sp)
20059506:	49b2                	c.lwsp	s3,12(sp)
20059508:	4a22                	c.lwsp	s4,8(sp)
2005950a:	4a92                	c.lwsp	s5,4(sp)
2005950c:	6105                	c.addi16sp	sp,32
2005950e:	8082                	c.jr	ra
20059510:	39ca0513          	addi	a0,s4,924 # 2006239c <CSWTCH.30+0x1c8>
20059514:	6ef050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059518:	bf41                	c.j	200594a8 <OTP_Read8+0x22>
2005951a:	4515                	c.li	a0,5
2005951c:	dffa9097          	auipc	ra,0xdffa9
20059520:	d9e080e7          	jalr	ra,-610(ra) # 22ba <DelayUs>
20059524:	0a05                	c.addi	s4,1
20059526:	b76d                	c.j	200594d0 <OTP_Read8+0x4a>
20059528:	fb3a0ae3          	beq	s4,s3,200594dc <OTP_Read8+0x56>
2005952c:	008aa783          	lw	a5,8(s5)
20059530:	00f90023          	sb	a5,0(s2)
20059534:	41000737          	lui	a4,0x41000
20059538:	4b5c                	c.lw	a5,20(a4)
2005953a:	4509                	c.li	a0,2
2005953c:	07a2                	c.slli	a5,0x8
2005953e:	83a1                	c.srli	a5,0x8
20059540:	cb5c                	c.sw	a5,20(a4)
20059542:	3b4d                	c.jal	200592f4 <IPC_SEMFree>
20059544:	bf65                	c.j	200594fc <OTP_Read8+0x76>

20059546 <pinmap_init>:
20059546:	715d                	c.addi16sp	sp,-80
20059548:	4100d7b7          	lui	a5,0x4100d
2005954c:	c4a2                	c.swsp	s0,72(sp)
2005954e:	c2a6                	c.swsp	s1,68(sp)
20059550:	c0ca                	c.swsp	s2,64(sp)
20059552:	de4e                	c.swsp	s3,60(sp)
20059554:	dc52                	c.swsp	s4,56(sp)
20059556:	c686                	c.swsp	ra,76(sp)
20059558:	da56                	c.swsp	s5,52(sp)
2005955a:	d85a                	c.swsp	s6,48(sp)
2005955c:	d65e                	c.swsp	s7,44(sp)
2005955e:	90078793          	addi	a5,a5,-1792 # 4100c900 <__km4_bd_boot_download_addr__+0x10ffa900>
20059562:	4398                	c.lw	a4,0(a5)
20059564:	200624b7          	lui	s1,0x20062
20059568:	6941                	c.lui	s2,0x10
2005956a:	c03a                	c.swsp	a4,0(sp)
2005956c:	4798                	c.lw	a4,8(a5)
2005956e:	3cc48493          	addi	s1,s1,972 # 200623cc <pmap_func>
20059572:	197d                	c.addi	s2,-1
20059574:	c43a                	c.swsp	a4,8(sp)
20059576:	43d8                	c.lw	a4,4(a5)
20059578:	4a0d                	c.li	s4,3
2005957a:	4405                	c.li	s0,1
2005957c:	c23a                	c.swsp	a4,4(sp)
2005957e:	47d8                	c.lw	a4,12(a5)
20059580:	4989                	c.li	s3,2
20059582:	c63a                	c.swsp	a4,12(sp)
20059584:	4b98                	c.lw	a4,16(a5)
20059586:	c83a                	c.swsp	a4,16(sp)
20059588:	4f98                	c.lw	a4,24(a5)
2005958a:	cc3a                	c.swsp	a4,24(sp)
2005958c:	4bd8                	c.lw	a4,20(a5)
2005958e:	ca3a                	c.swsp	a4,20(sp)
20059590:	4fdc                	c.lw	a5,28(a5)
20059592:	ce3e                	c.swsp	a5,28(sp)
20059594:	0004da83          	lhu	s5,0(s1)
20059598:	112a8163          	beq	s5,s2,2005969a <pinmap_init+0x154>
2005959c:	0024cb03          	lbu	s6,2(s1)
200595a0:	0ffafb93          	andi	s7,s5,255
200595a4:	014b1863          	bne	s6,s4,200595b4 <pinmap_init+0x6e>
200595a8:	4581                	c.li	a1,0
200595aa:	855e                	c.mv	a0,s7
200595ac:	dffac097          	auipc	ra,0xdffac
200595b0:	f6a080e7          	jalr	ra,-150(ra) # 5516 <PAD_CMD>
200595b4:	005ad793          	srli	a5,s5,0x5
200595b8:	8b8d                	c.andi	a5,3
200595ba:	01fbf513          	andi	a0,s7,31
200595be:	048b1963          	bne	s6,s0,20059610 <pinmap_init+0xca>
200595c2:	00279713          	slli	a4,a5,0x2
200595c6:	02070713          	addi	a4,a4,32 # 41000020 <__km4_bd_boot_download_addr__+0x10fee020>
200595ca:	970a                	c.add	a4,sp
200595cc:	f8070713          	addi	a4,a4,-128
200595d0:	5330                	c.lw	a2,96(a4)
200595d2:	00a416b3          	sll	a3,s0,a0
200595d6:	fff6c593          	xori	a1,a3,-1
200595da:	8e6d                	c.and	a2,a1
200595dc:	d330                	c.sw	a2,96(a4)
200595de:	5730                	c.lw	a2,104(a4)
200595e0:	8ed1                	c.or	a3,a2
200595e2:	d734                	c.sw	a3,104(a4)
200595e4:	0034c703          	lbu	a4,3(s1)
200595e8:	06871863          	bne	a4,s0,20059658 <pinmap_init+0x112>
200595ec:	078a                	c.slli	a5,0x2
200595ee:	02078793          	addi	a5,a5,32
200595f2:	978a                	c.add	a5,sp
200595f4:	f8078793          	addi	a5,a5,-128
200595f8:	5bb8                	c.lw	a4,112(a5)
200595fa:	00a41533          	sll	a0,s0,a0
200595fe:	fff54693          	xori	a3,a0,-1
20059602:	8f75                	c.and	a4,a3
20059604:	dbb8                	c.sw	a4,112(a5)
20059606:	5fb8                	c.lw	a4,120(a5)
20059608:	8d59                	c.or	a0,a4
2005960a:	dfa8                	c.sw	a0,120(a5)
2005960c:	0491                	c.addi	s1,4
2005960e:	b759                	c.j	20059594 <pinmap_init+0x4e>
20059610:	033b1363          	bne	s6,s3,20059636 <pinmap_init+0xf0>
20059614:	00279713          	slli	a4,a5,0x2
20059618:	02070713          	addi	a4,a4,32
2005961c:	970a                	c.add	a4,sp
2005961e:	f8070713          	addi	a4,a4,-128
20059622:	5330                	c.lw	a2,96(a4)
20059624:	00a416b3          	sll	a3,s0,a0
20059628:	8e55                	c.or	a2,a3
2005962a:	d330                	c.sw	a2,96(a4)
2005962c:	fff6c693          	xori	a3,a3,-1
20059630:	5730                	c.lw	a2,104(a4)
20059632:	8ef1                	c.and	a3,a2
20059634:	b77d                	c.j	200595e2 <pinmap_init+0x9c>
20059636:	fa0b17e3          	bne	s6,zero,200595e4 <pinmap_init+0x9e>
2005963a:	00279713          	slli	a4,a5,0x2
2005963e:	02070713          	addi	a4,a4,32
20059642:	970a                	c.add	a4,sp
20059644:	f8070713          	addi	a4,a4,-128
20059648:	5330                	c.lw	a2,96(a4)
2005964a:	00a416b3          	sll	a3,s0,a0
2005964e:	fff6c693          	xori	a3,a3,-1
20059652:	8e75                	c.and	a2,a3
20059654:	d330                	c.sw	a2,96(a4)
20059656:	bfe9                	c.j	20059630 <pinmap_init+0xea>
20059658:	03371263          	bne	a4,s3,2005967c <pinmap_init+0x136>
2005965c:	078a                	c.slli	a5,0x2
2005965e:	02078793          	addi	a5,a5,32
20059662:	978a                	c.add	a5,sp
20059664:	f8078793          	addi	a5,a5,-128
20059668:	5bb8                	c.lw	a4,112(a5)
2005966a:	00a41533          	sll	a0,s0,a0
2005966e:	8f49                	c.or	a4,a0
20059670:	dbb8                	c.sw	a4,112(a5)
20059672:	fff54513          	xori	a0,a0,-1
20059676:	5fb8                	c.lw	a4,120(a5)
20059678:	8d79                	c.and	a0,a4
2005967a:	bf41                	c.j	2005960a <pinmap_init+0xc4>
2005967c:	fb41                	c.bnez	a4,2005960c <pinmap_init+0xc6>
2005967e:	078a                	c.slli	a5,0x2
20059680:	02078793          	addi	a5,a5,32
20059684:	978a                	c.add	a5,sp
20059686:	f8078793          	addi	a5,a5,-128
2005968a:	5bb8                	c.lw	a4,112(a5)
2005968c:	00a41533          	sll	a0,s0,a0
20059690:	fff54513          	xori	a0,a0,-1
20059694:	8f69                	c.and	a4,a0
20059696:	dbb8                	c.sw	a4,112(a5)
20059698:	bff9                	c.j	20059676 <pinmap_init+0x130>
2005969a:	4702                	c.lwsp	a4,0(sp)
2005969c:	4100d7b7          	lui	a5,0x4100d
200596a0:	90078793          	addi	a5,a5,-1792 # 4100c900 <__km4_bd_boot_download_addr__+0x10ffa900>
200596a4:	c398                	c.sw	a4,0(a5)
200596a6:	4722                	c.lwsp	a4,8(sp)
200596a8:	c798                	c.sw	a4,8(a5)
200596aa:	4742                	c.lwsp	a4,16(sp)
200596ac:	cb98                	c.sw	a4,16(a5)
200596ae:	4762                	c.lwsp	a4,24(sp)
200596b0:	cf98                	c.sw	a4,24(a5)
200596b2:	4712                	c.lwsp	a4,4(sp)
200596b4:	c3d8                	c.sw	a4,4(a5)
200596b6:	4732                	c.lwsp	a4,12(sp)
200596b8:	c7d8                	c.sw	a4,12(a5)
200596ba:	4752                	c.lwsp	a4,20(sp)
200596bc:	cbd8                	c.sw	a4,20(a5)
200596be:	4772                	c.lwsp	a4,28(sp)
200596c0:	cfd8                	c.sw	a4,28(a5)
200596c2:	40b6                	c.lwsp	ra,76(sp)
200596c4:	4426                	c.lwsp	s0,72(sp)
200596c6:	4496                	c.lwsp	s1,68(sp)
200596c8:	4906                	c.lwsp	s2,64(sp)
200596ca:	59f2                	c.lwsp	s3,60(sp)
200596cc:	5a62                	c.lwsp	s4,56(sp)
200596ce:	5ad2                	c.lwsp	s5,52(sp)
200596d0:	5b42                	c.lwsp	s6,48(sp)
200596d2:	5bb2                	c.lwsp	s7,44(sp)
200596d4:	6161                	c.addi16sp	sp,80
200596d6:	8082                	c.jr	ra

200596d8 <PLL_State>:
200596d8:	4709                	c.li	a4,2
200596da:	87aa                	c.mv	a5,a0
200596dc:	00e51963          	bne	a0,a4,200596ee <PLL_State+0x16>
200596e0:	4100c7b7          	lui	a5,0x4100c
200596e4:	44078793          	addi	a5,a5,1088 # 4100c440 <__km4_bd_boot_download_addr__+0x10ffa440>
200596e8:	5b88                	c.lw	a0,48(a5)
200596ea:	8905                	c.andi	a0,1
200596ec:	8082                	c.jr	ra
200596ee:	470d                	c.li	a4,3
200596f0:	4501                	c.li	a0,0
200596f2:	00e79863          	bne	a5,a4,20059702 <PLL_State+0x2a>
200596f6:	4100c7b7          	lui	a5,0x4100c
200596fa:	44078793          	addi	a5,a5,1088 # 4100c440 <__km4_bd_boot_download_addr__+0x10ffa440>
200596fe:	5b88                	c.lw	a0,48(a5)
20059700:	8909                	c.andi	a0,2
20059702:	8082                	c.jr	ra

20059704 <pmu_set_sysactive_time>:
20059704:	1141                	c.addi	sp,-16
20059706:	c226                	c.swsp	s1,4(sp)
20059708:	ea01a483          	lw	s1,-352(gp) # 20066850 <sysactive_timeout_flag>
2005970c:	c422                	c.swsp	s0,8(sp)
2005970e:	c606                	c.swsp	ra,12(sp)
20059710:	ea418693          	addi	a3,gp,-348 # 20066854 <sysactive_timeout_temp>
20059714:	4298                	c.lw	a4,0(a3)
20059716:	842a                	c.mv	s0,a0
20059718:	c899                	c.beqz	s1,2005972e <pmu_set_sysactive_time+0x2a>
2005971a:	4485                	c.li	s1,1
2005971c:	00a77363          	bgeu	a4,a0,20059722 <pmu_set_sysactive_time+0x1e>
20059720:	c288                	c.sw	a0,0(a3)
20059722:	40b2                	c.lwsp	ra,12(sp)
20059724:	4422                	c.lwsp	s0,8(sp)
20059726:	8526                	c.mv	a0,s1
20059728:	4492                	c.lwsp	s1,4(sp)
2005972a:	0141                	c.addi	sp,16
2005972c:	8082                	c.jr	ra
2005972e:	ea418793          	addi	a5,gp,-348 # 20066854 <sysactive_timeout_temp>
20059732:	00e57363          	bgeu	a0,a4,20059738 <pmu_set_sysactive_time+0x34>
20059736:	843a                	c.mv	s0,a4
20059738:	0007a023          	sw	zero,0(a5)
2005973c:	4de020ef          	jal	ra,2005bc1a <rtos_time_get_current_system_time_ms>
20059740:	e9c1a783          	lw	a5,-356(gp) # 2006684c <sleepwakelock_timeout>
20059744:	800006b7          	lui	a3,0x80000
20059748:	942a                	c.add	s0,a0
2005974a:	e9c18713          	addi	a4,gp,-356 # 2006684c <sleepwakelock_timeout>
2005974e:	ffe6c693          	xori	a3,a3,-2
20059752:	0087f863          	bgeu	a5,s0,20059762 <pmu_set_sysactive_time+0x5e>
20059756:	40f407b3          	sub	a5,s0,a5
2005975a:	fcf6e4e3          	bltu	a3,a5,20059722 <pmu_set_sysactive_time+0x1e>
2005975e:	c300                	c.sw	s0,0(a4)
20059760:	b7c9                	c.j	20059722 <pmu_set_sysactive_time+0x1e>
20059762:	40878633          	sub	a2,a5,s0
20059766:	fec6ece3          	bltu	a3,a2,2005975e <pmu_set_sysactive_time+0x5a>
2005976a:	fa879ce3          	bne	a5,s0,20059722 <pmu_set_sysactive_time+0x1e>
2005976e:	bfc5                	c.j	2005975e <pmu_set_sysactive_time+0x5a>

20059770 <pmu_register_sleep_callback>:
20059770:	1141                	c.addi	sp,-16
20059772:	c606                	c.swsp	ra,12(sp)
20059774:	47f9                	c.li	a5,30
20059776:	00a7fa63          	bgeu	a5,a0,2005978a <pmu_register_sleep_callback+0x1a>
2005977a:	07300593          	addi	a1,zero,115
2005977e:	20062537          	lui	a0,0x20062
20059782:	4cc50513          	addi	a0,a0,1228 # 200624cc <pmap_func+0x100>
20059786:	182060ef          	jal	ra,2005f908 <io_assert_failed>
2005978a:	e589                	c.bnez	a1,20059794 <pmu_register_sleep_callback+0x24>
2005978c:	e681                	c.bnez	a3,20059794 <pmu_register_sleep_callback+0x24>
2005978e:	07400593          	addi	a1,zero,116
20059792:	b7f5                	c.j	2005977e <pmu_register_sleep_callback+0xe>
20059794:	47d1                	c.li	a5,20
20059796:	02f50833          	mul	a6,a0,a5
2005979a:	16818793          	addi	a5,gp,360 # 20066b18 <gPsmDdHookInfo>
2005979e:	97c2                	c.add	a5,a6
200597a0:	c388                	c.sw	a0,0(a5)
200597a2:	c3cc                	c.sw	a1,4(a5)
200597a4:	c790                	c.sw	a2,8(a5)
200597a6:	c7d4                	c.sw	a3,12(a5)
200597a8:	cb98                	c.sw	a4,16(a5)
200597aa:	40b2                	c.lwsp	ra,12(sp)
200597ac:	0141                	c.addi	sp,16
200597ae:	8082                	c.jr	ra

200597b0 <pmu_yield_os_check>:
200597b0:	e681a503          	lw	a0,-408(gp) # 20066818 <system_can_yield>
200597b4:	8082                	c.jr	ra

200597b6 <pmu_ready_to_sleep>:
200597b6:	1141                	c.addi	sp,-16
200597b8:	c606                	c.swsp	ra,12(sp)
200597ba:	460020ef          	jal	ra,2005bc1a <rtos_time_get_current_system_time_ms>
200597be:	e9c1a703          	lw	a4,-356(gp) # 2006684c <sleepwakelock_timeout>
200597c2:	800006b7          	lui	a3,0x80000
200597c6:	87aa                	c.mv	a5,a0
200597c8:	ffe6c693          	xori	a3,a3,-2
200597cc:	00a77e63          	bgeu	a4,a0,200597e8 <pmu_ready_to_sleep+0x32>
200597d0:	40e507b3          	sub	a5,a0,a4
200597d4:	4501                	c.li	a0,0
200597d6:	02f6e063          	bltu	a3,a5,200597f6 <pmu_ready_to_sleep+0x40>
200597da:	3e7d                	c.jal	20059398 <dsp_status_on>
200597dc:	e6c1a783          	lw	a5,-404(gp) # 2006681c <wakelock>
200597e0:	8d5d                	c.or	a0,a5
200597e2:	00153513          	sltiu	a0,a0,1
200597e6:	a801                	c.j	200597f6 <pmu_ready_to_sleep+0x40>
200597e8:	40a70633          	sub	a2,a4,a0
200597ec:	fec6e7e3          	bltu	a3,a2,200597da <pmu_ready_to_sleep+0x24>
200597f0:	4501                	c.li	a0,0
200597f2:	fee7f4e3          	bgeu	a5,a4,200597da <pmu_ready_to_sleep+0x24>
200597f6:	40b2                	c.lwsp	ra,12(sp)
200597f8:	0141                	c.addi	sp,16
200597fa:	8082                	c.jr	ra

200597fc <pmu_acquire_wakelock>:
200597fc:	1141                	c.addi	sp,-16
200597fe:	c422                	c.swsp	s0,8(sp)
20059800:	c606                	c.swsp	ra,12(sp)
20059802:	842a                	c.mv	s0,a0
20059804:	af2ff0ef          	jal	ra,20058af6 <irq_disable_save>
20059808:	e6c18713          	addi	a4,gp,-404 # 2006681c <wakelock>
2005980c:	4314                	c.lw	a3,0(a4)
2005980e:	4785                	c.li	a5,1
20059810:	008797b3          	sll	a5,a5,s0
20059814:	8fd5                	c.or	a5,a3
20059816:	c31c                	c.sw	a5,0(a4)
20059818:	4422                	c.lwsp	s0,8(sp)
2005981a:	40b2                	c.lwsp	ra,12(sp)
2005981c:	0141                	c.addi	sp,16
2005981e:	ae0ff06f          	jal	zero,20058afe <irq_enable_restore>

20059822 <pmu_release_wakelock>:
20059822:	1141                	c.addi	sp,-16
20059824:	c422                	c.swsp	s0,8(sp)
20059826:	c606                	c.swsp	ra,12(sp)
20059828:	842a                	c.mv	s0,a0
2005982a:	accff0ef          	jal	ra,20058af6 <irq_disable_save>
2005982e:	e6c18713          	addi	a4,gp,-404 # 2006681c <wakelock>
20059832:	4314                	c.lw	a3,0(a4)
20059834:	4785                	c.li	a5,1
20059836:	008797b3          	sll	a5,a5,s0
2005983a:	fff7c793          	xori	a5,a5,-1
2005983e:	8ff5                	c.and	a5,a3
20059840:	c31c                	c.sw	a5,0(a4)
20059842:	4422                	c.lwsp	s0,8(sp)
20059844:	40b2                	c.lwsp	ra,12(sp)
20059846:	0141                	c.addi	sp,16
20059848:	ab6ff06f          	jal	zero,20058afe <irq_enable_restore>

2005984c <PSRAM_calibration>:
2005984c:	7119                	c.addi16sp	sp,-128
2005984e:	de86                	c.swsp	ra,124(sp)
20059850:	dca6                	c.swsp	s1,120(sp)
20059852:	daca                	c.swsp	s2,116(sp)
20059854:	d8ce                	c.swsp	s3,112(sp)
20059856:	d6d2                	c.swsp	s4,108(sp)
20059858:	d4d6                	c.swsp	s5,104(sp)
2005985a:	d2da                	c.swsp	s6,100(sp)
2005985c:	d0de                	c.swsp	s7,96(sp)
2005985e:	cee2                	c.swsp	s8,92(sp)
20059860:	cce6                	c.swsp	s9,88(sp)
20059862:	caea                	c.swsp	s10,84(sp)
20059864:	c8ee                	c.swsp	s11,80(sp)
20059866:	cc2a                	c.swsp	a0,24(sp)
20059868:	410087b7          	lui	a5,0x41008
2005986c:	43d8                	c.lw	a4,4(a5)
2005986e:	100007b7          	lui	a5,0x10000
20059872:	1781                	c.addi	a5,-32
20059874:	8ff9                	c.and	a5,a4
20059876:	d43e                	c.swsp	a5,40(sp)
20059878:	dc02                	c.swsp	zero,56(sp)
2005987a:	de02                	c.swsp	zero,60(sp)
2005987c:	c082                	c.swsp	zero,64(sp)
2005987e:	c282                	c.swsp	zero,68(sp)
20059880:	c482                	c.swsp	zero,72(sp)
20059882:	c682                	c.swsp	zero,76(sp)
20059884:	7c1027f3          	csrrs	a5,0x7c1,zero
20059888:	1007f793          	andi	a5,a5,256
2005988c:	c385                	c.beqz	a5,200598ac <PSRAM_calibration+0x60>
2005988e:	7c0467f3          	csrrsi	a5,0x7c0,8
20059892:	0ff0000f          	fence	iorw,iorw
20059896:	0001                	c.addi	zero,0
20059898:	0001                	c.addi	zero,0
2005989a:	0001                	c.addi	zero,0
2005989c:	0001                	c.addi	zero,0
2005989e:	0001                	c.addi	zero,0
200598a0:	0001                	c.addi	zero,0
200598a2:	0001                	c.addi	zero,0
200598a4:	0001                	c.addi	zero,0
200598a6:	0001                	c.addi	zero,0
200598a8:	0001                	c.addi	zero,0
200598aa:	0001                	c.addi	zero,0
200598ac:	10000793          	addi	a5,zero,256
200598b0:	7c17b7f3          	csrrc	a5,0x7c1,a5
200598b4:	0ff0000f          	fence	iorw,iorw
200598b8:	0001                	c.addi	zero,0
200598ba:	0001                	c.addi	zero,0
200598bc:	0001                	c.addi	zero,0
200598be:	0001                	c.addi	zero,0
200598c0:	0001                	c.addi	zero,0
200598c2:	0001                	c.addi	zero,0
200598c4:	0001                	c.addi	zero,0
200598c6:	0001                	c.addi	zero,0
200598c8:	0001                	c.addi	zero,0
200598ca:	0001                	c.addi	zero,0
200598cc:	0001                	c.addi	zero,0
200598ce:	7c0467f3          	csrrsi	a5,0x7c0,8
200598d2:	0ff0000f          	fence	iorw,iorw
200598d6:	0001                	c.addi	zero,0
200598d8:	0001                	c.addi	zero,0
200598da:	0001                	c.addi	zero,0
200598dc:	0001                	c.addi	zero,0
200598de:	0001                	c.addi	zero,0
200598e0:	0001                	c.addi	zero,0
200598e2:	0001                	c.addi	zero,0
200598e4:	0001                	c.addi	zero,0
200598e6:	0001                	c.addi	zero,0
200598e8:	0001                	c.addi	zero,0
200598ea:	0001                	c.addi	zero,0
200598ec:	41008737          	lui	a4,0x41008
200598f0:	431c                	c.lw	a5,0(a4)
200598f2:	20062c37          	lui	s8,0x20062
200598f6:	60000d37          	lui	s10,0x60000
200598fa:	9bf9                	c.andi	a5,-2
200598fc:	c31c                	c.sw	a5,0(a4)
200598fe:	4791                	c.li	a5,4
20059900:	ca3e                	c.swsp	a5,20(sp)
20059902:	000d0b13          	addi	s6,s10,0 # 60000000 <__km4_bd_psram_start__>
20059906:	564c0793          	addi	a5,s8,1380 # 20062564 <pmap_func+0x198>
2005990a:	00050cb7          	lui	s9,0x50
2005990e:	d03e                	c.swsp	a5,32(sp)
20059910:	019b07b3          	add	a5,s6,s9
20059914:	597d                	c.li	s2,-1
20059916:	4981                	c.li	s3,0
20059918:	5afd                	c.li	s5,-1
2005991a:	54fd                	c.li	s1,-1
2005991c:	4a05                	c.li	s4,1
2005991e:	c8818b93          	addi	s7,gp,-888 # 20066638 <PSRAM_CALIB_PATTERN>
20059922:	ce3e                	c.swsp	a5,28(sp)
20059924:	47e2                	c.lwsp	a5,24(sp)
20059926:	cf91                	c.beqz	a5,20059942 <PSRAM_calibration+0xf6>
20059928:	5782                	c.lwsp	a5,32(sp)
2005992a:	200625b7          	lui	a1,0x20062
2005992e:	8752                	c.mv	a4,s4
20059930:	20078693          	addi	a3,a5,512 # 10000200 <__km4_boot_text_start__+0x8001e0>
20059934:	04900613          	addi	a2,zero,73
20059938:	57858593          	addi	a1,a1,1400 # 20062578 <pmap_func+0x1ac>
2005993c:	4511                	c.li	a0,4
2005993e:	039010ef          	jal	ra,2005b176 <rtk_log_write>
20059942:	57a2                	c.lwsp	a5,40(sp)
20059944:	01ca1d93          	slli	s11,s4,0x1c
20059948:	5ffd                	c.li	t6,-1
2005994a:	00fde7b3          	or	a5,s11,a5
2005994e:	d23e                	c.swsp	a5,36(sp)
20059950:	4c01                	c.li	s8,0
20059952:	5cfd                	c.li	s9,-1
20059954:	4d81                	c.li	s11,0
20059956:	5792                	c.lwsp	a5,36(sp)
20059958:	d67e                	c.swsp	t6,44(sp)
2005995a:	410086b7          	lui	a3,0x41008
2005995e:	01b7e7b3          	or	a5,a5,s11
20059962:	c2dc                	c.sw	a5,4(a3)
20059964:	000ba783          	lw	a5,0(s7)
20059968:	000d4603          	lbu	a2,0(s10)
2005996c:	4772                	c.lwsp	a4,28(sp)
2005996e:	0ff7f693          	andi	a3,a5,255
20059972:	00dd0023          	sb	a3,0(s10)
20059976:	0087d693          	srli	a3,a5,0x8
2005997a:	0ff6f693          	andi	a3,a3,255
2005997e:	001d4603          	lbu	a2,1(s10)
20059982:	00dd00a3          	sb	a3,1(s10)
20059986:	0107d693          	srli	a3,a5,0x10
2005998a:	0ff6f693          	andi	a3,a3,255
2005998e:	002d4603          	lbu	a2,2(s10)
20059992:	83e1                	c.srli	a5,0x18
20059994:	00dd0123          	sb	a3,2(s10)
20059998:	003d4683          	lbu	a3,3(s10)
2005999c:	00fd01a3          	sb	a5,3(s10)
200599a0:	004ba783          	lw	a5,4(s7)
200599a4:	00100837          	lui	a6,0x100
200599a8:	985a                	c.add	a6,s6
200599aa:	c31c                	c.sw	a5,0(a4)
200599ac:	008ba783          	lw	a5,8(s7)
200599b0:	00150537          	lui	a0,0x150
200599b4:	955a                	c.add	a0,s6
200599b6:	00f82023          	sw	a5,0(a6) # 100000 <__ap_sram_heap_size+0xc0000>
200599ba:	00cba783          	lw	a5,12(s7)
200599be:	002005b7          	lui	a1,0x200
200599c2:	95da                	c.add	a1,s6
200599c4:	c11c                	c.sw	a5,0(a0)
200599c6:	010ba783          	lw	a5,16(s7)
200599ca:	00250637          	lui	a2,0x250
200599ce:	965a                	c.add	a2,s6
200599d0:	c19c                	c.sw	a5,0(a1)
200599d2:	014ba783          	lw	a5,20(s7)
200599d6:	c21c                	c.sw	a5,0(a2)
200599d8:	000d4683          	lbu	a3,0(s10)
200599dc:	001d4783          	lbu	a5,1(s10)
200599e0:	0ff6f693          	andi	a3,a3,255
200599e4:	0ff7f793          	andi	a5,a5,255
200599e8:	07a2                	c.slli	a5,0x8
200599ea:	8edd                	c.or	a3,a5
200599ec:	002d4783          	lbu	a5,2(s10)
200599f0:	0ff7f793          	andi	a5,a5,255
200599f4:	07c2                	c.slli	a5,0x10
200599f6:	8fd5                	c.or	a5,a3
200599f8:	003d4683          	lbu	a3,3(s10)
200599fc:	06e2                	c.slli	a3,0x18
200599fe:	8fd5                	c.or	a5,a3
20059a00:	dc3e                	c.swsp	a5,56(sp)
20059a02:	431c                	c.lw	a5,0(a4)
20059a04:	de3e                	c.swsp	a5,60(sp)
20059a06:	00082783          	lw	a5,0(a6)
20059a0a:	c0be                	c.swsp	a5,64(sp)
20059a0c:	411c                	c.lw	a5,0(a0)
20059a0e:	1828                	c.addi4spn	a0,sp,56
20059a10:	c2be                	c.swsp	a5,68(sp)
20059a12:	419c                	c.lw	a5,0(a1)
20059a14:	85de                	c.mv	a1,s7
20059a16:	c4be                	c.swsp	a5,72(sp)
20059a18:	421c                	c.lw	a5,0(a2)
20059a1a:	4661                	c.li	a2,24
20059a1c:	c6be                	c.swsp	a5,76(sp)
20059a1e:	dffb0097          	auipc	ra,0xdffb0
20059a22:	ba2080e7          	jalr	ra,-1118(ra) # 95c0 <_memcmp>
20059a26:	5fb2                	c.lwsp	t6,44(sp)
20059a28:	47e2                	c.lwsp	a5,24(sp)
20059a2a:	14051563          	bne	a0,zero,20059b74 <PSRAM_calibration+0x328>
20059a2e:	c79d                	c.beqz	a5,20059a5c <PSRAM_calibration+0x210>
20059a30:	47b6                	c.lwsp	a5,76(sp)
20059a32:	5682                	c.lwsp	a3,32(sp)
20059a34:	4886                	c.lwsp	a7,64(sp)
20059a36:	c43e                	c.swsp	a5,8(sp)
20059a38:	47a6                	c.lwsp	a5,72(sp)
20059a3a:	5872                	c.lwsp	a6,60(sp)
20059a3c:	200625b7          	lui	a1,0x20062
20059a40:	c23e                	c.swsp	a5,4(sp)
20059a42:	4796                	c.lwsp	a5,68(sp)
20059a44:	876e                	c.mv	a4,s11
20059a46:	21468693          	addi	a3,a3,532 # 41008214 <__km4_bd_boot_download_addr__+0x10ff6214>
20059a4a:	c03e                	c.swsp	a5,0(sp)
20059a4c:	57e2                	c.lwsp	a5,56(sp)
20059a4e:	04900613          	addi	a2,zero,73
20059a52:	57858593          	addi	a1,a1,1400 # 20062578 <pmap_func+0x1ac>
20059a56:	4511                	c.li	a0,4
20059a58:	71e010ef          	jal	ra,2005b176 <rtk_log_write>
20059a5c:	57fd                	c.li	a5,-1
20059a5e:	00fc9363          	bne	s9,a5,20059a64 <PSRAM_calibration+0x218>
20059a62:	8cee                	c.mv	s9,s11
20059a64:	47fd                	c.li	a5,31
20059a66:	018c8fb3          	add	t6,s9,s8
20059a6a:	0c05                	c.addi	s8,1
20059a6c:	00fd9863          	bne	s11,a5,20059a7c <PSRAM_calibration+0x230>
20059a70:	0189d663          	bge	s3,s8,20059a7c <PSRAM_calibration+0x230>
20059a74:	8952                	c.mv	s2,s4
20059a76:	89e2                	c.mv	s3,s8
20059a78:	8afe                	c.mv	s5,t6
20059a7a:	84e6                	c.mv	s1,s9
20059a7c:	0d85                	c.addi	s11,1
20059a7e:	02000793          	addi	a5,zero,32
20059a82:	ecfd9ae3          	bne	s11,a5,20059956 <PSRAM_calibration+0x10a>
20059a86:	47d2                	c.lwsp	a5,20(sp)
20059a88:	0a06                	c.slli	s4,0x1
20059a8a:	17fd                	c.addi	a5,-1
20059a8c:	ca3e                	c.swsp	a5,20(sp)
20059a8e:	e8079be3          	bne	a5,zero,20059924 <PSRAM_calibration+0xd8>
20059a92:	200626b7          	lui	a3,0x20062
20059a96:	200625b7          	lui	a1,0x20062
20059a9a:	88ca                	c.mv	a7,s2
20059a9c:	884e                	c.mv	a6,s3
20059a9e:	87d6                	c.mv	a5,s5
20059aa0:	8726                	c.mv	a4,s1
20059aa2:	7bc68693          	addi	a3,a3,1980 # 200627bc <pmap_func+0x3f0>
20059aa6:	04900613          	addi	a2,zero,73
20059aaa:	57858593          	addi	a1,a1,1400 # 20062578 <pmap_func+0x1ac>
20059aae:	4511                	c.li	a0,4
20059ab0:	6c6010ef          	jal	ra,2005b176 <rtk_log_write>
20059ab4:	7c1027f3          	csrrs	a5,0x7c1,zero
20059ab8:	1007f793          	andi	a5,a5,256
20059abc:	e7b1                	c.bnez	a5,20059b08 <PSRAM_calibration+0x2bc>
20059abe:	7c1027f3          	csrrs	a5,0x7c1,zero
20059ac2:	1007f793          	andi	a5,a5,256
20059ac6:	e3a9                	c.bnez	a5,20059b08 <PSRAM_calibration+0x2bc>
20059ac8:	7c0167f3          	csrrsi	a5,0x7c0,2
20059acc:	0ff0000f          	fence	iorw,iorw
20059ad0:	0001                	c.addi	zero,0
20059ad2:	0001                	c.addi	zero,0
20059ad4:	0001                	c.addi	zero,0
20059ad6:	0001                	c.addi	zero,0
20059ad8:	0001                	c.addi	zero,0
20059ada:	0001                	c.addi	zero,0
20059adc:	0001                	c.addi	zero,0
20059ade:	0001                	c.addi	zero,0
20059ae0:	0001                	c.addi	zero,0
20059ae2:	0001                	c.addi	zero,0
20059ae4:	0001                	c.addi	zero,0
20059ae6:	10000793          	addi	a5,zero,256
20059aea:	7c17a7f3          	csrrs	a5,0x7c1,a5
20059aee:	0ff0000f          	fence	iorw,iorw
20059af2:	0001                	c.addi	zero,0
20059af4:	0001                	c.addi	zero,0
20059af6:	0001                	c.addi	zero,0
20059af8:	0001                	c.addi	zero,0
20059afa:	0001                	c.addi	zero,0
20059afc:	0001                	c.addi	zero,0
20059afe:	0001                	c.addi	zero,0
20059b00:	0001                	c.addi	zero,0
20059b02:	0001                	c.addi	zero,0
20059b04:	0001                	c.addi	zero,0
20059b06:	0001                	c.addi	zero,0
20059b08:	47a1                	c.li	a5,8
20059b0a:	4501                	c.li	a0,0
20059b0c:	0537d663          	bge	a5,s3,20059b58 <PSRAM_calibration+0x30c>
20059b10:	409a8733          	sub	a4,s5,s1
20059b14:	4609                	c.li	a2,2
20059b16:	02c74733          	div	a4,a4,a2
20059b1a:	94d6                	c.add	s1,s5
20059b1c:	6685                	c.lui	a3,0x1
20059b1e:	f0068693          	addi	a3,a3,-256 # f00 <CPU_ClkGet+0x3ea>
20059b22:	0972                	c.slli	s2,0x1c
20059b24:	4505                	c.li	a0,1
20059b26:	02c4c4b3          	div	s1,s1,a2
20059b2a:	1779                	c.addi	a4,-2
20059b2c:	00871793          	slli	a5,a4,0x8
20059b30:	8ff5                	c.and	a5,a3
20059b32:	0127e7b3          	or	a5,a5,s2
20059b36:	0742                	c.slli	a4,0x10
20059b38:	000f06b7          	lui	a3,0xf0
20059b3c:	8f75                	c.and	a4,a3
20059b3e:	88fd                	c.andi	s1,31
20059b40:	8fc5                	c.or	a5,s1
20059b42:	8fd9                	c.or	a5,a4
20059b44:	02000737          	lui	a4,0x2000
20059b48:	8fd9                	c.or	a5,a4
20059b4a:	41008737          	lui	a4,0x41008
20059b4e:	c35c                	c.sw	a5,4(a4)
20059b50:	431c                	c.lw	a5,0(a4)
20059b52:	0017e793          	ori	a5,a5,1
20059b56:	c31c                	c.sw	a5,0(a4)
20059b58:	50f6                	c.lwsp	ra,124(sp)
20059b5a:	54e6                	c.lwsp	s1,120(sp)
20059b5c:	5956                	c.lwsp	s2,116(sp)
20059b5e:	59c6                	c.lwsp	s3,112(sp)
20059b60:	5a36                	c.lwsp	s4,108(sp)
20059b62:	5aa6                	c.lwsp	s5,104(sp)
20059b64:	5b16                	c.lwsp	s6,100(sp)
20059b66:	5b86                	c.lwsp	s7,96(sp)
20059b68:	4c76                	c.lwsp	s8,92(sp)
20059b6a:	4ce6                	c.lwsp	s9,88(sp)
20059b6c:	4d56                	c.lwsp	s10,84(sp)
20059b6e:	4dc6                	c.lwsp	s11,80(sp)
20059b70:	6109                	c.addi16sp	sp,128
20059b72:	8082                	c.jr	ra
20059b74:	cb95                	c.beqz	a5,20059ba8 <PSRAM_calibration+0x35c>
20059b76:	47b6                	c.lwsp	a5,76(sp)
20059b78:	4886                	c.lwsp	a7,64(sp)
20059b7a:	5872                	c.lwsp	a6,60(sp)
20059b7c:	c43e                	c.swsp	a5,8(sp)
20059b7e:	47a6                	c.lwsp	a5,72(sp)
20059b80:	200626b7          	lui	a3,0x20062
20059b84:	200625b7          	lui	a1,0x20062
20059b88:	c23e                	c.swsp	a5,4(sp)
20059b8a:	4796                	c.lwsp	a5,68(sp)
20059b8c:	876e                	c.mv	a4,s11
20059b8e:	79868693          	addi	a3,a3,1944 # 20062798 <pmap_func+0x3cc>
20059b92:	c03e                	c.swsp	a5,0(sp)
20059b94:	57e2                	c.lwsp	a5,56(sp)
20059b96:	04900613          	addi	a2,zero,73
20059b9a:	57858593          	addi	a1,a1,1400 # 20062578 <pmap_func+0x1ac>
20059b9e:	4511                	c.li	a0,4
20059ba0:	d67e                	c.swsp	t6,44(sp)
20059ba2:	5d4010ef          	jal	ra,2005b176 <rtk_log_write>
20059ba6:	5fb2                	c.lwsp	t6,44(sp)
20059ba8:	57fd                	c.li	a5,-1
20059baa:	ecfc89e3          	beq	s9,a5,20059a7c <PSRAM_calibration+0x230>
20059bae:	0189d663          	bge	s3,s8,20059bba <PSRAM_calibration+0x36e>
20059bb2:	8952                	c.mv	s2,s4
20059bb4:	89e2                	c.mv	s3,s8
20059bb6:	8afe                	c.mv	s5,t6
20059bb8:	84e6                	c.mv	s1,s9
20059bba:	5ffd                	c.li	t6,-1
20059bbc:	4c01                	c.li	s8,0
20059bbe:	5cfd                	c.li	s9,-1
20059bc0:	bd75                	c.j	20059a7c <PSRAM_calibration+0x230>

20059bc2 <System_Reset>:
20059bc2:	4100c7b7          	lui	a5,0x4100c
20059bc6:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
20059bca:	4709                	c.li	a4,2
20059bcc:	8399                	c.srli	a5,0x6
20059bce:	8b8d                	c.andi	a5,3
20059bd0:	00e78963          	beq	a5,a4,20059be2 <System_Reset+0x20>
20059bd4:	4705                	c.li	a4,1
20059bd6:	02e78963          	beq	a5,a4,20059c08 <System_Reset+0x46>
20059bda:	40000737          	lui	a4,0x40000
20059bde:	c781                	c.beqz	a5,20059be6 <System_Reset+0x24>
20059be0:	8082                	c.jr	ra
20059be2:	20000737          	lui	a4,0x20000
20059be6:	969696b7          	lui	a3,0x96969
20059bea:	4100c7b7          	lui	a5,0x4100c
20059bee:	69668693          	addi	a3,a3,1686 # 96969696 <__ctrace_end__+0xcc969696>
20059bf2:	26d7a223          	sw	a3,612(a5) # 4100c264 <__km4_bd_boot_download_addr__+0x10ffa264>
20059bf6:	26e7a023          	sw	a4,608(a5)
20059bfa:	69697737          	lui	a4,0x69697
20059bfe:	96970713          	addi	a4,a4,-1687 # 69696969 <__kr4_psram_text_start__+0x9516949>
20059c02:	26e7a223          	sw	a4,612(a5)
20059c06:	bfe9                	c.j	20059be0 <System_Reset+0x1e>
20059c08:	80000737          	lui	a4,0x80000
20059c0c:	bfe9                	c.j	20059be6 <System_Reset+0x24>

20059c0e <SWR_CORE_Vol_Get>:
20059c0e:	4100d7b7          	lui	a5,0x4100d
20059c12:	f3e7c503          	lbu	a0,-194(a5) # 4100cf3e <__km4_bd_boot_download_addr__+0x10ffaf3e>
20059c16:	0ff57513          	andi	a0,a0,255
20059c1a:	8082                	c.jr	ra

20059c1c <wifi_set_user_config>:
20059c1c:	1141                	c.addi	sp,-16
20059c1e:	c422                	c.swsp	s0,8(sp)
20059c20:	04400613          	addi	a2,zero,68
20059c24:	41018413          	addi	s0,gp,1040 # 20066dc0 <wifi_user_config>
20059c28:	4581                	c.li	a1,0
20059c2a:	41018513          	addi	a0,gp,1040 # 20066dc0 <wifi_user_config>
20059c2e:	c606                	c.swsp	ra,12(sp)
20059c30:	dffb0097          	auipc	ra,0xdffb0
20059c34:	b5a080e7          	jalr	ra,-1190(ra) # 978a <_memset>
20059c38:	4799                	c.li	a5,6
20059c3a:	d05c                	c.sw	a5,36(s0)
20059c3c:	47a1                	c.li	a5,8
20059c3e:	d41c                	c.sw	a5,40(s0)
20059c40:	50a00793          	addi	a5,zero,1290
20059c44:	00f41f23          	sh	a5,30(s0)
20059c48:	47a5                	c.li	a5,9
20059c4a:	02f40023          	sb	a5,32(s0)
20059c4e:	4785                	c.li	a5,1
20059c50:	00f405a3          	sb	a5,11(s0)
20059c54:	00f41623          	sh	a5,12(s0)
20059c58:	00f41423          	sh	a5,8(s0)
20059c5c:	0b0107b7          	lui	a5,0xb010
20059c60:	10178793          	addi	a5,a5,257 # b010101 <__ap_sram_heap_size+0xafd0101>
20059c64:	c81c                	c.sw	a5,16(s0)
20059c66:	4789                	c.li	a5,2
20059c68:	cc1c                	c.sw	a5,24(s0)
20059c6a:	00020737          	lui	a4,0x20
20059c6e:	10100793          	addi	a5,zero,257
20059c72:	02f41923          	sh	a5,50(s0)
20059c76:	02f41a23          	sh	a5,52(s0)
20059c7a:	20070713          	addi	a4,a4,512 # 20200 <__bdram_heap_buffer_size__+0x8bc0>
20059c7e:	4791                	c.li	a5,4
20059c80:	c018                	c.sw	a4,0(s0)
20059c82:	02f40b23          	sb	a5,54(s0)
20059c86:	02f40c23          	sb	a5,56(s0)
20059c8a:	050f0737          	lui	a4,0x50f0
20059c8e:	6785                	c.lui	a5,0x1
20059c90:	c058                	c.sw	a4,4(s0)
20059c92:	38878793          	addi	a5,a5,904 # 1388 <CPU_ClkGet+0x872>
20059c96:	4751                	c.li	a4,20
20059c98:	40b2                	c.lwsp	ra,12(sp)
20059c9a:	02042623          	sw	zero,44(s0)
20059c9e:	00040ea3          	sb	zero,29(s0)
20059ca2:	00040a23          	sb	zero,20(s0)
20059ca6:	00041b23          	sh	zero,22(s0)
20059caa:	020408a3          	sb	zero,49(s0)
20059cae:	04e400a3          	sb	a4,65(s0)
20059cb2:	dc5c                	c.sw	a5,60(s0)
20059cb4:	4422                	c.lwsp	s0,8(sp)
20059cb6:	0141                	c.addi	sp,16
20059cb8:	8082                	c.jr	ra

20059cba <hash_index>:
20059cba:	4781                	c.li	a5,0
20059cbc:	08300693          	addi	a3,zero,131
20059cc0:	00054703          	lbu	a4,0(a0) # 150000 <__ap_sram_heap_size+0x110000>
20059cc4:	e709                	c.bnez	a4,20059cce <hash_index+0x14>
20059cc6:	00179513          	slli	a0,a5,0x1
20059cca:	8105                	c.srli	a0,0x1
20059ccc:	8082                	c.jr	ra
20059cce:	02d787b3          	mul	a5,a5,a3
20059cd2:	0505                	c.addi	a0,1
20059cd4:	97ba                	c.add	a5,a4
20059cd6:	b7ed                	c.j	20059cc0 <hash_index+0x6>

20059cd8 <log_add_new_command>:
20059cd8:	1141                	c.addi	sp,-16
20059cda:	c422                	c.swsp	s0,8(sp)
20059cdc:	c606                	c.swsp	ra,12(sp)
20059cde:	842a                	c.mv	s0,a0
20059ce0:	4108                	c.lw	a0,0(a0)
20059ce2:	3fe1                	c.jal	20059cba <hash_index>
20059ce4:	02000793          	addi	a5,zero,32
20059ce8:	02f56533          	rem	a0,a0,a5
20059cec:	45418793          	addi	a5,gp,1108 # 20066e04 <log_hash>
20059cf0:	00840713          	addi	a4,s0,8
20059cf4:	050e                	c.slli	a0,0x3
20059cf6:	97aa                	c.add	a5,a0
20059cf8:	4394                	c.lw	a3,0(a5)
20059cfa:	c2d8                	c.sw	a4,4(a3)
20059cfc:	c414                	c.sw	a3,8(s0)
20059cfe:	c45c                	c.sw	a5,12(s0)
20059d00:	c398                	c.sw	a4,0(a5)
20059d02:	40b2                	c.lwsp	ra,12(sp)
20059d04:	4422                	c.lwsp	s0,8(sp)
20059d06:	0141                	c.addi	sp,16
20059d08:	8082                	c.jr	ra

20059d0a <atcmd_service_init>:
20059d0a:	45418793          	addi	a5,gp,1108 # 20066e04 <log_hash>
20059d0e:	10078713          	addi	a4,a5,256
20059d12:	c39c                	c.sw	a5,0(a5)
20059d14:	c3dc                	c.sw	a5,4(a5)
20059d16:	07a1                	c.addi	a5,8
20059d18:	fee79de3          	bne	a5,a4,20059d12 <atcmd_service_init+0x8>
20059d1c:	e701a783          	lw	a5,-400(gp) # 20066820 <log_init_table>
20059d20:	8782                	c.jr	a5

20059d22 <atcmd_service_add_table>:
20059d22:	1141                	c.addi	sp,-16
20059d24:	c422                	c.swsp	s0,8(sp)
20059d26:	c226                	c.swsp	s1,4(sp)
20059d28:	c04a                	c.swsp	s2,0(sp)
20059d2a:	c606                	c.swsp	ra,12(sp)
20059d2c:	892a                	c.mv	s2,a0
20059d2e:	84ae                	c.mv	s1,a1
20059d30:	4401                	c.li	s0,0
20059d32:	00944863          	blt	s0,s1,20059d42 <atcmd_service_add_table+0x20>
20059d36:	40b2                	c.lwsp	ra,12(sp)
20059d38:	4422                	c.lwsp	s0,8(sp)
20059d3a:	4492                	c.lwsp	s1,4(sp)
20059d3c:	4902                	c.lwsp	s2,0(sp)
20059d3e:	0141                	c.addi	sp,16
20059d40:	8082                	c.jr	ra
20059d42:	00441513          	slli	a0,s0,0x4
20059d46:	954a                	c.add	a0,s2
20059d48:	3f41                	c.jal	20059cd8 <log_add_new_command>
20059d4a:	0405                	c.addi	s0,1
20059d4c:	b7dd                	c.j	20059d32 <atcmd_service_add_table+0x10>

20059d4e <atcmd_action>:
20059d4e:	1141                	c.addi	sp,-16
20059d50:	c422                	c.swsp	s0,8(sp)
20059d52:	c226                	c.swsp	s1,4(sp)
20059d54:	c04a                	c.swsp	s2,0(sp)
20059d56:	c606                	c.swsp	ra,12(sp)
20059d58:	892a                	c.mv	s2,a0
20059d5a:	02000413          	addi	s0,zero,32
20059d5e:	3fb1                	c.jal	20059cba <hash_index>
20059d60:	02856433          	rem	s0,a0,s0
20059d64:	00341793          	slli	a5,s0,0x3
20059d68:	45418413          	addi	s0,gp,1108 # 20066e04 <log_hash>
20059d6c:	943e                	c.add	s0,a5
20059d6e:	4004                	c.lw	s1,0(s0)
20059d70:	00849463          	bne	s1,s0,20059d78 <atcmd_action+0x2a>
20059d74:	4501                	c.li	a0,0
20059d76:	a819                	c.j	20059d8c <atcmd_action+0x3e>
20059d78:	ff84a503          	lw	a0,-8(s1)
20059d7c:	85ca                	c.mv	a1,s2
20059d7e:	dffb0097          	auipc	ra,0xdffb0
20059d82:	d5c080e7          	jalr	ra,-676(ra) # 9ada <__wrap_strcmp>
20059d86:	e909                	c.bnez	a0,20059d98 <atcmd_action+0x4a>
20059d88:	ffc4a503          	lw	a0,-4(s1)
20059d8c:	40b2                	c.lwsp	ra,12(sp)
20059d8e:	4422                	c.lwsp	s0,8(sp)
20059d90:	4492                	c.lwsp	s1,4(sp)
20059d92:	4902                	c.lwsp	s2,0(sp)
20059d94:	0141                	c.addi	sp,16
20059d96:	8082                	c.jr	ra
20059d98:	4084                	c.lw	s1,0(s1)
20059d9a:	bfd9                	c.j	20059d70 <atcmd_action+0x22>

20059d9c <atcmd_handler>:
20059d9c:	7139                	c.addi16sp	sp,-64
20059d9e:	dc22                	c.swsp	s0,56(sp)
20059da0:	c42a                	c.swsp	a0,8(sp)
20059da2:	842a                	c.mv	s0,a0
20059da4:	4675                	c.li	a2,29
20059da6:	4581                	c.li	a1,0
20059da8:	0808                	c.addi4spn	a0,sp,16
20059daa:	de06                	c.swsp	ra,60(sp)
20059dac:	da26                	c.swsp	s1,52(sp)
20059dae:	c602                	c.swsp	zero,12(sp)
20059db0:	dffb0097          	auipc	ra,0xdffb0
20059db4:	d1e080e7          	jalr	ra,-738(ra) # 9ace <__wrap_memset>
20059db8:	200625b7          	lui	a1,0x20062
20059dbc:	4609                	c.li	a2,2
20059dbe:	7f458593          	addi	a1,a1,2036 # 200627f4 <pmap_func+0x428>
20059dc2:	8522                	c.mv	a0,s0
20059dc4:	75b070ef          	jal	ra,20061d1e <strncmp>
20059dc8:	c519                	c.beqz	a0,20059dd6 <atcmd_handler+0x3a>
20059dca:	4501                	c.li	a0,0
20059dcc:	50f2                	c.lwsp	ra,60(sp)
20059dce:	5462                	c.lwsp	s0,56(sp)
20059dd0:	54d2                	c.lwsp	s1,52(sp)
20059dd2:	6121                	c.addi16sp	sp,64
20059dd4:	8082                	c.jr	ra
20059dd6:	200625b7          	lui	a1,0x20062
20059dda:	7f858593          	addi	a1,a1,2040 # 200627f8 <pmap_func+0x42c>
20059dde:	0028                	c.addi4spn	a0,sp,8
20059de0:	014080ef          	jal	ra,20061df4 <strsep>
20059de4:	200635b7          	lui	a1,0x20063
20059de8:	842a                	c.mv	s0,a0
20059dea:	83c58593          	addi	a1,a1,-1988 # 2006283c <pmap_func+0x470>
20059dee:	0028                	c.addi4spn	a0,sp,8
20059df0:	004080ef          	jal	ra,20061df4 <strsep>
20059df4:	84aa                	c.mv	s1,a0
20059df6:	d871                	c.beqz	s0,20059dca <atcmd_handler+0x2e>
20059df8:	8522                	c.mv	a0,s0
20059dfa:	dffb0097          	auipc	ra,0xdffb0
20059dfe:	cde080e7          	jalr	ra,-802(ra) # 9ad8 <__wrap_strlen>
20059e02:	478d                	c.li	a5,3
20059e04:	fca7f3e3          	bgeu	a5,a0,20059dca <atcmd_handler+0x2e>
20059e08:	85a2                	c.mv	a1,s0
20059e0a:	02000613          	addi	a2,zero,32
20059e0e:	0068                	c.addi4spn	a0,sp,12
20059e10:	77b070ef          	jal	ra,20061d8a <strncpy>
20059e14:	00e10513          	addi	a0,sp,14
20059e18:	3f1d                	c.jal	20059d4e <atcmd_action>
20059e1a:	842a                	c.mv	s0,a0
20059e1c:	4501                	c.li	a0,0
20059e1e:	d45d                	c.beqz	s0,20059dcc <atcmd_handler+0x30>
20059e20:	8526                	c.mv	a0,s1
20059e22:	9402                	c.jalr	s0
20059e24:	8522                	c.mv	a0,s0
20059e26:	b75d                	c.j	20059dcc <atcmd_handler+0x30>

20059e28 <parse_param>:
20059e28:	7179                	c.addi16sp	sp,-48
20059e2a:	d422                	c.swsp	s0,40(sp)
20059e2c:	d606                	c.swsp	ra,44(sp)
20059e2e:	d226                	c.swsp	s1,36(sp)
20059e30:	d04a                	c.swsp	s2,32(sp)
20059e32:	ce4e                	c.swsp	s3,28(sp)
20059e34:	4405                	c.li	s0,1
20059e36:	c505                	c.beqz	a0,20059e5e <parse_param+0x36>
20059e38:	84ae                	c.mv	s1,a1
20059e3a:	c62a                	c.swsp	a0,12(sp)
20059e3c:	4405                	c.li	s0,1
20059e3e:	200629b7          	lui	s3,0x20062
20059e42:	495d                	c.li	s2,23
20059e44:	7fc98593          	addi	a1,s3,2044 # 200627fc <pmap_func+0x430>
20059e48:	0068                	c.addi4spn	a0,sp,12
20059e4a:	7ab070ef          	jal	ra,20061df4 <strsep>
20059e4e:	00241793          	slli	a5,s0,0x2
20059e52:	97a6                	c.add	a5,s1
20059e54:	c388                	c.sw	a0,0(a5)
20059e56:	c501                	c.beqz	a0,20059e5e <parse_param+0x36>
20059e58:	0405                	c.addi	s0,1
20059e5a:	ff2415e3          	bne	s0,s2,20059e44 <parse_param+0x1c>
20059e5e:	50b2                	c.lwsp	ra,44(sp)
20059e60:	8522                	c.mv	a0,s0
20059e62:	5422                	c.lwsp	s0,40(sp)
20059e64:	5492                	c.lwsp	s1,36(sp)
20059e66:	5902                	c.lwsp	s2,32(sp)
20059e68:	49f2                	c.lwsp	s3,28(sp)
20059e6a:	6145                	c.addi16sp	sp,48
20059e6c:	8082                	c.jr	ra

20059e6e <mp_command_handler>:
20059e6e:	200635b7          	lui	a1,0x20063
20059e72:	1141                	c.addi	sp,-16
20059e74:	461d                	c.li	a2,7
20059e76:	80058593          	addi	a1,a1,-2048 # 20062800 <pmap_func+0x434>
20059e7a:	c422                	c.swsp	s0,8(sp)
20059e7c:	c606                	c.swsp	ra,12(sp)
20059e7e:	842a                	c.mv	s0,a0
20059e80:	69f070ef          	jal	ra,20061d1e <strncmp>
20059e84:	e129                	c.bnez	a0,20059ec6 <mp_command_handler+0x58>
20059e86:	041d                	c.addi	s0,7
20059e88:	8522                	c.mv	a0,s0
20059e8a:	dffb0097          	auipc	ra,0xdffb0
20059e8e:	c4e080e7          	jalr	ra,-946(ra) # 9ad8 <__wrap_strlen>
20059e92:	4605                	c.li	a2,1
20059e94:	00150593          	addi	a1,a0,1
20059e98:	8522                	c.mv	a0,s0
20059e9a:	37b060ef          	jal	ra,20060a14 <inic_mp_command>
20059e9e:	071010ef          	jal	ra,2005b70e <rtos_mem_get_free_heap_size>
20059ea2:	85aa                	c.mv	a1,a0
20059ea4:	20063537          	lui	a0,0x20063
20059ea8:	80850513          	addi	a0,a0,-2040 # 20062808 <pmap_func+0x43c>
20059eac:	556050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059eb0:	20063537          	lui	a0,0x20063
20059eb4:	83450513          	addi	a0,a0,-1996 # 20062834 <pmap_func+0x468>
20059eb8:	54a050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059ebc:	4505                	c.li	a0,1
20059ebe:	40b2                	c.lwsp	ra,12(sp)
20059ec0:	4422                	c.lwsp	s0,8(sp)
20059ec2:	0141                	c.addi	sp,16
20059ec4:	8082                	c.jr	ra
20059ec6:	4501                	c.li	a0,0
20059ec8:	bfdd                	c.j	20059ebe <mp_command_handler+0x50>

20059eca <atcmd_service>:
20059eca:	1141                	c.addi	sp,-16
20059ecc:	c606                	c.swsp	ra,12(sp)
20059ece:	35f9                	c.jal	20059d9c <atcmd_handler>
20059ed0:	c505                	c.beqz	a0,20059ef8 <atcmd_service+0x2e>
20059ed2:	03d010ef          	jal	ra,2005b70e <rtos_mem_get_free_heap_size>
20059ed6:	85aa                	c.mv	a1,a0
20059ed8:	20063537          	lui	a0,0x20063
20059edc:	80850513          	addi	a0,a0,-2040 # 20062808 <pmap_func+0x43c>
20059ee0:	522050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059ee4:	20063537          	lui	a0,0x20063
20059ee8:	83450513          	addi	a0,a0,-1996 # 20062834 <pmap_func+0x468>
20059eec:	516050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059ef0:	4505                	c.li	a0,1
20059ef2:	40b2                	c.lwsp	ra,12(sp)
20059ef4:	0141                	c.addi	sp,16
20059ef6:	8082                	c.jr	ra
20059ef8:	4501                	c.li	a0,0
20059efa:	bfe5                	c.j	20059ef2 <atcmd_service+0x28>

20059efc <at_otaclear>:
20059efc:	1141                	c.addi	sp,-16
20059efe:	c606                	c.swsp	ra,12(sp)
20059f00:	2e2030ef          	jal	ra,2005d1e2 <sys_clear_ota_signature>
20059f04:	40b2                	c.lwsp	ra,12(sp)
20059f06:	200635b7          	lui	a1,0x20063
20059f0a:	20063537          	lui	a0,0x20063
20059f0e:	84058593          	addi	a1,a1,-1984 # 20062840 <pmap_func+0x474>
20059f12:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
20059f16:	0141                	c.addi	sp,16
20059f18:	4ea0506f          	jal	zero,2005f402 <DiagPrintf_minimal>

20059f1c <at_otarecover>:
20059f1c:	1141                	c.addi	sp,-16
20059f1e:	c606                	c.swsp	ra,12(sp)
20059f20:	3e4030ef          	jal	ra,2005d304 <sys_recover_ota_signature>
20059f24:	40b2                	c.lwsp	ra,12(sp)
20059f26:	200635b7          	lui	a1,0x20063
20059f2a:	20063537          	lui	a0,0x20063
20059f2e:	85858593          	addi	a1,a1,-1960 # 20062858 <pmap_func+0x48c>
20059f32:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
20059f36:	0141                	c.addi	sp,16
20059f38:	4ca0506f          	jal	zero,2005f402 <DiagPrintf_minimal>

20059f3c <at_test>:
20059f3c:	c521                	c.beqz	a0,20059f84 <at_test+0x48>
20059f3e:	1141                	c.addi	sp,-16
20059f40:	c606                	c.swsp	ra,12(sp)
20059f42:	c422                	c.swsp	s0,8(sp)
20059f44:	842a                	c.mv	s0,a0
20059f46:	dffb0097          	auipc	ra,0xdffb0
20059f4a:	b92080e7          	jalr	ra,-1134(ra) # 9ad8 <__wrap_strlen>
20059f4e:	85aa                	c.mv	a1,a0
20059f50:	20063537          	lui	a0,0x20063
20059f54:	86850513          	addi	a0,a0,-1944 # 20062868 <pmap_func+0x49c>
20059f58:	4aa050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059f5c:	20063537          	lui	a0,0x20063
20059f60:	85a2                	c.mv	a1,s0
20059f62:	87c50513          	addi	a0,a0,-1924 # 2006287c <pmap_func+0x4b0>
20059f66:	49c050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059f6a:	4422                	c.lwsp	s0,8(sp)
20059f6c:	200635b7          	lui	a1,0x20063
20059f70:	20063537          	lui	a0,0x20063
20059f74:	40b2                	c.lwsp	ra,12(sp)
20059f76:	88c58593          	addi	a1,a1,-1908 # 2006288c <pmap_func+0x4c0>
20059f7a:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
20059f7e:	0141                	c.addi	sp,16
20059f80:	4820506f          	jal	zero,2005f402 <DiagPrintf_minimal>
20059f84:	200635b7          	lui	a1,0x20063
20059f88:	20063537          	lui	a0,0x20063
20059f8c:	88c58593          	addi	a1,a1,-1908 # 2006288c <pmap_func+0x4c0>
20059f90:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
20059f94:	b7f5                	c.j	20059f80 <at_test+0x44>

20059f96 <at_rst>:
20059f96:	200635b7          	lui	a1,0x20063
20059f9a:	20063537          	lui	a0,0x20063
20059f9e:	1141                	c.addi	sp,-16
20059fa0:	89458593          	addi	a1,a1,-1900 # 20062894 <pmap_func+0x4c8>
20059fa4:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
20059fa8:	c606                	c.swsp	ra,12(sp)
20059faa:	458050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059fae:	40b2                	c.lwsp	ra,12(sp)
20059fb0:	0141                	c.addi	sp,16
20059fb2:	48e0306f          	jal	zero,2005d440 <sys_reset>

20059fb6 <at_state>:
20059fb6:	7179                	c.addi16sp	sp,-48
20059fb8:	d606                	c.swsp	ra,44(sp)
20059fba:	d422                	c.swsp	s0,40(sp)
20059fbc:	732030ef          	jal	ra,2005d6ee <uxTaskGetNumberOfTasks>
20059fc0:	02d00793          	addi	a5,zero,45
20059fc4:	02f50533          	mul	a0,a0,a5
20059fc8:	714010ef          	jal	ra,2005b6dc <rtos_mem_malloc>
20059fcc:	842a                	c.mv	s0,a0
20059fce:	3d8040ef          	jal	ra,2005e3a6 <vTaskList>
20059fd2:	20063537          	lui	a0,0x20063
20059fd6:	85a2                	c.mv	a1,s0
20059fd8:	89c50513          	addi	a0,a0,-1892 # 2006289c <pmap_func+0x4d0>
20059fdc:	426050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059fe0:	8522                	c.mv	a0,s0
20059fe2:	724010ef          	jal	ra,2005b706 <rtos_mem_free>
20059fe6:	0048                	c.addi4spn	a0,sp,4
20059fe8:	5e1000ef          	jal	ra,2005adc8 <vPortGetHeapStats>
20059fec:	20063537          	lui	a0,0x20063
20059ff0:	8b050513          	addi	a0,a0,-1872 # 200628b0 <pmap_func+0x4e4>
20059ff4:	40e050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20059ff8:	ec01a583          	lw	a1,-320(gp) # 20066870 <total_heap_size>
20059ffc:	20063537          	lui	a0,0x20063
2005a000:	8c050513          	addi	a0,a0,-1856 # 200628c0 <pmap_func+0x4f4>
2005a004:	3fe050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a008:	4592                	c.lwsp	a1,4(sp)
2005a00a:	20063537          	lui	a0,0x20063
2005a00e:	8d450513          	addi	a0,a0,-1836 # 200628d4 <pmap_func+0x508>
2005a012:	3f0050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a016:	4792                	c.lwsp	a5,4(sp)
2005a018:	ec01a583          	lw	a1,-320(gp) # 20066870 <total_heap_size>
2005a01c:	20063537          	lui	a0,0x20063
2005a020:	8e850513          	addi	a0,a0,-1816 # 200628e8 <pmap_func+0x51c>
2005a024:	8d9d                	c.sub	a1,a5
2005a026:	3dc050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a02a:	47d2                	c.lwsp	a5,20(sp)
2005a02c:	ec01a583          	lw	a1,-320(gp) # 20066870 <total_heap_size>
2005a030:	20063537          	lui	a0,0x20063
2005a034:	8fc50513          	addi	a0,a0,-1796 # 200628fc <pmap_func+0x530>
2005a038:	8d9d                	c.sub	a1,a5
2005a03a:	3c8050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a03e:	200635b7          	lui	a1,0x20063
2005a042:	20063537          	lui	a0,0x20063
2005a046:	91058593          	addi	a1,a1,-1776 # 20062910 <pmap_func+0x544>
2005a04a:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
2005a04e:	3b4050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a052:	50b2                	c.lwsp	ra,44(sp)
2005a054:	5422                	c.lwsp	s0,40(sp)
2005a056:	6145                	c.addi16sp	sp,48
2005a058:	8082                	c.jr	ra

2005a05a <at_gmr>:
2005a05a:	715d                	c.addi16sp	sp,-80
2005a05c:	40000513          	addi	a0,zero,1024
2005a060:	c686                	c.swsp	ra,76(sp)
2005a062:	c4a2                	c.swsp	s0,72(sp)
2005a064:	c2a6                	c.swsp	s1,68(sp)
2005a066:	676010ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005a06a:	842a                	c.mv	s0,a0
2005a06c:	20063537          	lui	a0,0x20063
2005a070:	4685                	c.li	a3,1
2005a072:	4601                	c.li	a2,0
2005a074:	4585                	c.li	a1,1
2005a076:	91850513          	addi	a0,a0,-1768 # 20062918 <pmap_func+0x54c>
2005a07a:	388050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a07e:	8522                	c.mv	a0,s0
2005a080:	3ff00593          	addi	a1,zero,1023
2005a084:	263040ef          	jal	ra,2005eae6 <ChipInfo_GetSocName_ToBuf>
2005a088:	200634b7          	lui	s1,0x20063
2005a08c:	85a2                	c.mv	a1,s0
2005a08e:	93c48513          	addi	a0,s1,-1732 # 2006293c <pmap_func+0x570>
2005a092:	370050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a096:	8522                	c.mv	a0,s0
2005a098:	3ff00593          	addi	a1,zero,1023
2005a09c:	307040ef          	jal	ra,2005eba2 <ChipInfo_GetLibVersion_ToBuf>
2005a0a0:	85a2                	c.mv	a1,s0
2005a0a2:	93c48513          	addi	a0,s1,-1732
2005a0a6:	35c050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a0aa:	8522                	c.mv	a0,s0
2005a0ac:	65a010ef          	jal	ra,2005b706 <rtos_mem_free>
2005a0b0:	200635b7          	lui	a1,0x20063
2005a0b4:	02000613          	addi	a2,zero,32
2005a0b8:	94058593          	addi	a1,a1,-1728 # 20062940 <pmap_func+0x574>
2005a0bc:	850a                	c.mv	a0,sp
2005a0be:	4cd070ef          	jal	ra,20061d8a <strncpy>
2005a0c2:	200635b7          	lui	a1,0x20063
2005a0c6:	02000613          	addi	a2,zero,32
2005a0ca:	94858593          	addi	a1,a1,-1720 # 20062948 <pmap_func+0x57c>
2005a0ce:	1008                	c.addi4spn	a0,sp,32
2005a0d0:	4bb070ef          	jal	ra,20061d8a <strncpy>
2005a0d4:	20063737          	lui	a4,0x20063
2005a0d8:	200635b7          	lui	a1,0x20063
2005a0dc:	20063537          	lui	a0,0x20063
2005a0e0:	1014                	c.addi4spn	a3,sp,32
2005a0e2:	860a                	c.mv	a2,sp
2005a0e4:	95070713          	addi	a4,a4,-1712 # 20062950 <pmap_func+0x584>
2005a0e8:	96458593          	addi	a1,a1,-1692 # 20062964 <pmap_func+0x598>
2005a0ec:	96c50513          	addi	a0,a0,-1684 # 2006296c <pmap_func+0x5a0>
2005a0f0:	312050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a0f4:	40b6                	c.lwsp	ra,76(sp)
2005a0f6:	4426                	c.lwsp	s0,72(sp)
2005a0f8:	4496                	c.lwsp	s1,68(sp)
2005a0fa:	6161                	c.addi16sp	sp,80
2005a0fc:	8082                	c.jr	ra

2005a0fe <at_log>:
2005a0fe:	7159                	c.addi16sp	sp,-112
2005a100:	d2a6                	c.swsp	s1,100(sp)
2005a102:	05c00613          	addi	a2,zero,92
2005a106:	84aa                	c.mv	s1,a0
2005a108:	4581                	c.li	a1,0
2005a10a:	0048                	c.addi4spn	a0,sp,4
2005a10c:	d4a2                	c.swsp	s0,104(sp)
2005a10e:	d0ca                	c.swsp	s2,96(sp)
2005a110:	d686                	c.swsp	ra,108(sp)
2005a112:	20063437          	lui	s0,0x20063
2005a116:	dffb0097          	auipc	ra,0xdffb0
2005a11a:	9b8080e7          	jalr	ra,-1608(ra) # 9ace <__wrap_memset>
2005a11e:	20063937          	lui	s2,0x20063
2005a122:	ec91                	c.bnez	s1,2005a13e <at_log+0x40>
2005a124:	200636b7          	lui	a3,0x20063
2005a128:	97c68693          	addi	a3,a3,-1668 # 2006297c <pmap_func+0x5b0>
2005a12c:	04100613          	addi	a2,zero,65
2005a130:	99040593          	addi	a1,s0,-1648 # 20062990 <pmap_func+0x5c4>
2005a134:	4505                	c.li	a0,1
2005a136:	040010ef          	jal	ra,2005b176 <rtk_log_write>
2005a13a:	4605                	c.li	a2,1
2005a13c:	aa35                	c.j	2005a278 <at_log+0x17a>
2005a13e:	8526                	c.mv	a0,s1
2005a140:	004c                	c.addi4spn	a1,sp,4
2005a142:	31dd                	c.jal	20059e28 <parse_param>
2005a144:	4785                	c.li	a5,1
2005a146:	84aa                	c.mv	s1,a0
2005a148:	1aa7d063          	bge	a5,a0,2005a2e8 <at_log+0x1ea>
2005a14c:	4522                	c.lwsp	a0,8(sp)
2005a14e:	18050d63          	beq	a0,zero,2005a2e8 <at_log+0x1ea>
2005a152:	1e7070ef          	jal	ra,20061b38 <atoi>
2005a156:	0ff57713          	andi	a4,a0,255
2005a15a:	4789                	c.li	a5,2
2005a15c:	16f70563          	beq	a4,a5,2005a2c6 <at_log+0x1c8>
2005a160:	00e7ef63          	bltu	a5,a4,2005a17e <at_log+0x80>
2005a164:	e771                	c.bnez	a4,2005a230 <at_log+0x132>
2005a166:	478d                	c.li	a5,3
2005a168:	00f49663          	bne	s1,a5,2005a174 <at_log+0x76>
2005a16c:	4532                	c.lwsp	a0,12(sp)
2005a16e:	00054783          	lbu	a5,0(a0)
2005a172:	e78d                	c.bnez	a5,2005a19c <at_log+0x9e>
2005a174:	200636b7          	lui	a3,0x20063
2005a178:	99468693          	addi	a3,a3,-1644 # 20062994 <pmap_func+0x5c8>
2005a17c:	bf45                	c.j	2005a12c <at_log+0x2e>
2005a17e:	468d                	c.li	a3,3
2005a180:	14d70f63          	beq	a4,a3,2005a2de <at_log+0x1e0>
2005a184:	200636b7          	lui	a3,0x20063
2005a188:	a2868693          	addi	a3,a3,-1496 # 20062a28 <pmap_func+0x65c>
2005a18c:	04100613          	addi	a2,zero,65
2005a190:	99040593          	addi	a1,s0,-1648
2005a194:	4505                	c.li	a0,1
2005a196:	7e1000ef          	jal	ra,2005b176 <rtk_log_write>
2005a19a:	b745                	c.j	2005a13a <at_log+0x3c>
2005a19c:	59f000ef          	jal	ra,2005af3a <rtk_log_level_get>
2005a1a0:	45b2                	c.lwsp	a1,12(sp)
2005a1a2:	862a                	c.mv	a2,a0
2005a1a4:	20063537          	lui	a0,0x20063
2005a1a8:	9b450513          	addi	a0,a0,-1612 # 200629b4 <pmap_func+0x5e8>
2005a1ac:	256050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a1b0:	200636b7          	lui	a3,0x20063
2005a1b4:	a4468693          	addi	a3,a3,-1468 # 20062a44 <pmap_func+0x678>
2005a1b8:	04100613          	addi	a2,zero,65
2005a1bc:	99040593          	addi	a1,s0,-1648
2005a1c0:	4505                	c.li	a0,1
2005a1c2:	7b5000ef          	jal	ra,2005b176 <rtk_log_write>
2005a1c6:	200636b7          	lui	a3,0x20063
2005a1ca:	a6068693          	addi	a3,a3,-1440 # 20062a60 <pmap_func+0x694>
2005a1ce:	04500613          	addi	a2,zero,69
2005a1d2:	99040593          	addi	a1,s0,-1648
2005a1d6:	4509                	c.li	a0,2
2005a1d8:	79f000ef          	jal	ra,2005b176 <rtk_log_write>
2005a1dc:	200636b7          	lui	a3,0x20063
2005a1e0:	a7c68693          	addi	a3,a3,-1412 # 20062a7c <pmap_func+0x6b0>
2005a1e4:	05700613          	addi	a2,zero,87
2005a1e8:	99040593          	addi	a1,s0,-1648
2005a1ec:	450d                	c.li	a0,3
2005a1ee:	789000ef          	jal	ra,2005b176 <rtk_log_write>
2005a1f2:	200636b7          	lui	a3,0x20063
2005a1f6:	99040593          	addi	a1,s0,-1648
2005a1fa:	a9868693          	addi	a3,a3,-1384 # 20062a98 <pmap_func+0x6cc>
2005a1fe:	04900613          	addi	a2,zero,73
2005a202:	4511                	c.li	a0,4
2005a204:	773000ef          	jal	ra,2005b176 <rtk_log_write>
2005a208:	20063537          	lui	a0,0x20063
2005a20c:	ab050513          	addi	a0,a0,-1360 # 20062ab0 <pmap_func+0x6e4>
2005a210:	1f2050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a214:	20063537          	lui	a0,0x20063
2005a218:	ac890593          	addi	a1,s2,-1336 # 20062ac8 <pmap_func+0x6fc>
2005a21c:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
2005a220:	1e2050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a224:	50b6                	c.lwsp	ra,108(sp)
2005a226:	5426                	c.lwsp	s0,104(sp)
2005a228:	5496                	c.lwsp	s1,100(sp)
2005a22a:	5906                	c.lwsp	s2,96(sp)
2005a22c:	6165                	c.addi16sp	sp,112
2005a22e:	8082                	c.jr	ra
2005a230:	4791                	c.li	a5,4
2005a232:	00f49a63          	bne	s1,a5,2005a246 <at_log+0x148>
2005a236:	47b2                	c.lwsp	a5,12(sp)
2005a238:	0007c783          	lbu	a5,0(a5)
2005a23c:	c789                	c.beqz	a5,2005a246 <at_log+0x148>
2005a23e:	4542                	c.lwsp	a0,16(sp)
2005a240:	00054783          	lbu	a5,0(a0)
2005a244:	e791                	c.bnez	a5,2005a250 <at_log+0x152>
2005a246:	200636b7          	lui	a3,0x20063
2005a24a:	9c868693          	addi	a3,a3,-1592 # 200629c8 <pmap_func+0x5fc>
2005a24e:	bdf9                	c.j	2005a12c <at_log+0x2e>
2005a250:	0e9070ef          	jal	ra,20061b38 <atoi>
2005a254:	0ff57593          	andi	a1,a0,255
2005a258:	4532                	c.lwsp	a0,12(sp)
2005a25a:	55d000ef          	jal	ra,2005afb6 <rtk_log_level_set>
2005a25e:	d929                	c.beqz	a0,2005a1b0 <at_log+0xb2>
2005a260:	200636b7          	lui	a3,0x20063
2005a264:	04100613          	addi	a2,zero,65
2005a268:	9e868693          	addi	a3,a3,-1560 # 200629e8 <pmap_func+0x61c>
2005a26c:	99040593          	addi	a1,s0,-1648
2005a270:	4505                	c.li	a0,1
2005a272:	705000ef          	jal	ra,2005b176 <rtk_log_write>
2005a276:	4609                	c.li	a2,2
2005a278:	20063537          	lui	a0,0x20063
2005a27c:	ac890593          	addi	a1,s2,-1336
2005a280:	ad050513          	addi	a0,a0,-1328 # 20062ad0 <pmap_func+0x704>
2005a284:	17e050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a288:	20063537          	lui	a0,0x20063
2005a28c:	87850513          	addi	a0,a0,-1928 # 20062878 <pmap_func+0x4ac>
2005a290:	172050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a294:	20063537          	lui	a0,0x20063
2005a298:	ae050513          	addi	a0,a0,-1312 # 20062ae0 <pmap_func+0x714>
2005a29c:	166050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a2a0:	20063537          	lui	a0,0x20063
2005a2a4:	b0c50513          	addi	a0,a0,-1268 # 20062b0c <pmap_func+0x740>
2005a2a8:	15a050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a2ac:	20063537          	lui	a0,0x20063
2005a2b0:	b4450513          	addi	a0,a0,-1212 # 20062b44 <pmap_func+0x778>
2005a2b4:	14e050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a2b8:	20063537          	lui	a0,0x20063
2005a2bc:	b7850513          	addi	a0,a0,-1160 # 20062b78 <pmap_func+0x7ac>
2005a2c0:	142050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a2c4:	b785                	c.j	2005a224 <at_log+0x126>
2005a2c6:	00e48763          	beq	s1,a4,2005a2d4 <at_log+0x1d6>
2005a2ca:	200636b7          	lui	a3,0x20063
2005a2ce:	a0468693          	addi	a3,a3,-1532 # 20062a04 <pmap_func+0x638>
2005a2d2:	bda9                	c.j	2005a12c <at_log+0x2e>
2005a2d4:	58418513          	addi	a0,gp,1412 # 20066f34 <rtk_log_tag_array>
2005a2d8:	3ef000ef          	jal	ra,2005aec6 <rtk_log_array_print>
2005a2dc:	bdd1                	c.j	2005a1b0 <at_log+0xb2>
2005a2de:	fef496e3          	bne	s1,a5,2005a2ca <at_log+0x1cc>
2005a2e2:	439000ef          	jal	ra,2005af1a <rtk_log_array_clear>
2005a2e6:	b5e9                	c.j	2005a1b0 <at_log+0xb2>
2005a2e8:	4711                	c.li	a4,4
2005a2ea:	bd69                	c.j	2005a184 <at_log+0x86>

2005a2ec <at_rreg>:
2005a2ec:	7159                	c.addi16sp	sp,-112
2005a2ee:	d4a2                	c.swsp	s0,104(sp)
2005a2f0:	05c00613          	addi	a2,zero,92
2005a2f4:	842a                	c.mv	s0,a0
2005a2f6:	4581                	c.li	a1,0
2005a2f8:	0048                	c.addi4spn	a0,sp,4
2005a2fa:	d2a6                	c.swsp	s1,100(sp)
2005a2fc:	d686                	c.swsp	ra,108(sp)
2005a2fe:	200634b7          	lui	s1,0x20063
2005a302:	dffaf097          	auipc	ra,0xdffaf
2005a306:	7cc080e7          	jalr	ra,1996(ra) # 9ace <__wrap_memset>
2005a30a:	ec05                	c.bnez	s0,2005a342 <at_rreg+0x56>
2005a30c:	200636b7          	lui	a3,0x20063
2005a310:	200635b7          	lui	a1,0x20063
2005a314:	05700613          	addi	a2,zero,87
2005a318:	99058593          	addi	a1,a1,-1648 # 20062990 <pmap_func+0x5c4>
2005a31c:	450d                	c.li	a0,3
2005a31e:	bac68693          	addi	a3,a3,-1108 # 20062bac <pmap_func+0x7e0>
2005a322:	655000ef          	jal	ra,2005b176 <rtk_log_write>
2005a326:	20063537          	lui	a0,0x20063
2005a32a:	4605                	c.li	a2,1
2005a32c:	bc848593          	addi	a1,s1,-1080 # 20062bc8 <pmap_func+0x7fc>
2005a330:	ad050513          	addi	a0,a0,-1328 # 20062ad0 <pmap_func+0x704>
2005a334:	0ce050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a338:	50b6                	c.lwsp	ra,108(sp)
2005a33a:	5426                	c.lwsp	s0,104(sp)
2005a33c:	5496                	c.lwsp	s1,100(sp)
2005a33e:	6165                	c.addi16sp	sp,112
2005a340:	8082                	c.jr	ra
2005a342:	004c                	c.addi4spn	a1,sp,4
2005a344:	8522                	c.mv	a0,s0
2005a346:	ae3ff0ef          	jal	ra,20059e28 <parse_param>
2005a34a:	ffe50713          	addi	a4,a0,-2
2005a34e:	4789                	c.li	a5,2
2005a350:	fae7eee3          	bltu	a5,a4,2005a30c <at_rreg+0x20>
2005a354:	157d                	c.addi	a0,-1
2005a356:	0542                	c.slli	a0,0x10
2005a358:	002c                	c.addi4spn	a1,sp,8
2005a35a:	8141                	c.srli	a0,0x10
2005a35c:	19e010ef          	jal	ra,2005b4fa <cmd_dump_word>
2005a360:	20063537          	lui	a0,0x20063
2005a364:	bc848593          	addi	a1,s1,-1080
2005a368:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
2005a36c:	096050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a370:	b7e1                	c.j	2005a338 <at_rreg+0x4c>

2005a372 <at_wreg>:
2005a372:	7159                	c.addi16sp	sp,-112
2005a374:	d4a2                	c.swsp	s0,104(sp)
2005a376:	05c00613          	addi	a2,zero,92
2005a37a:	842a                	c.mv	s0,a0
2005a37c:	4581                	c.li	a1,0
2005a37e:	0048                	c.addi4spn	a0,sp,4
2005a380:	d2a6                	c.swsp	s1,100(sp)
2005a382:	d686                	c.swsp	ra,108(sp)
2005a384:	200634b7          	lui	s1,0x20063
2005a388:	dffaf097          	auipc	ra,0xdffaf
2005a38c:	746080e7          	jalr	ra,1862(ra) # 9ace <__wrap_memset>
2005a390:	ec05                	c.bnez	s0,2005a3c8 <at_wreg+0x56>
2005a392:	200636b7          	lui	a3,0x20063
2005a396:	200635b7          	lui	a1,0x20063
2005a39a:	05700613          	addi	a2,zero,87
2005a39e:	99058593          	addi	a1,a1,-1648 # 20062990 <pmap_func+0x5c4>
2005a3a2:	450d                	c.li	a0,3
2005a3a4:	bd068693          	addi	a3,a3,-1072 # 20062bd0 <pmap_func+0x804>
2005a3a8:	5cf000ef          	jal	ra,2005b176 <rtk_log_write>
2005a3ac:	20063537          	lui	a0,0x20063
2005a3b0:	4605                	c.li	a2,1
2005a3b2:	bec48593          	addi	a1,s1,-1044 # 20062bec <pmap_func+0x820>
2005a3b6:	ad050513          	addi	a0,a0,-1328 # 20062ad0 <pmap_func+0x704>
2005a3ba:	048050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a3be:	50b6                	c.lwsp	ra,108(sp)
2005a3c0:	5426                	c.lwsp	s0,104(sp)
2005a3c2:	5496                	c.lwsp	s1,100(sp)
2005a3c4:	6165                	c.addi16sp	sp,112
2005a3c6:	8082                	c.jr	ra
2005a3c8:	004c                	c.addi4spn	a1,sp,4
2005a3ca:	8522                	c.mv	a0,s0
2005a3cc:	a5dff0ef          	jal	ra,20059e28 <parse_param>
2005a3d0:	478d                	c.li	a5,3
2005a3d2:	fcf510e3          	bne	a0,a5,2005a392 <at_wreg+0x20>
2005a3d6:	45a2                	c.lwsp	a1,8(sp)
2005a3d8:	4509                	c.li	a0,2
2005a3da:	07a010ef          	jal	ra,2005b454 <cmd_write_word>
2005a3de:	20063537          	lui	a0,0x20063
2005a3e2:	bec48593          	addi	a1,s1,-1044
2005a3e6:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
2005a3ea:	018050ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a3ee:	bfc1                	c.j	2005a3be <at_wreg+0x4c>

2005a3f0 <find_status>:
2005a3f0:	872a                	c.mv	a4,a0
2005a3f2:	4781                	c.li	a5,0
2005a3f4:	00c7c463          	blt	a5,a2,2005a3fc <find_status+0xc>
2005a3f8:	4501                	c.li	a0,0
2005a3fa:	8082                	c.jr	ra
2005a3fc:	852e                	c.mv	a0,a1
2005a3fe:	4594                	c.lw	a3,8(a1)
2005a400:	02458593          	addi	a1,a1,36
2005a404:	fee68be3          	beq	a3,a4,2005a3fa <find_status+0xa>
2005a408:	0785                	c.addi	a5,1
2005a40a:	b7ed                	c.j	2005a3f4 <find_status+0x4>

2005a40c <find_free_delta>:
2005a40c:	87aa                	c.mv	a5,a0
2005a40e:	4701                	c.li	a4,0
2005a410:	00b74463          	blt	a4,a1,2005a418 <find_free_delta+0xc>
2005a414:	4501                	c.li	a0,0
2005a416:	8082                	c.jr	ra
2005a418:	853e                	c.mv	a0,a5
2005a41a:	5794                	c.lw	a3,40(a5)
2005a41c:	03878793          	addi	a5,a5,56
2005a420:	dafd                	c.beqz	a3,2005a416 <find_free_delta+0xa>
2005a422:	0705                	c.addi	a4,1
2005a424:	b7f5                	c.j	2005a410 <find_free_delta+0x4>

2005a426 <update_status>:
2005a426:	7179                	c.addi16sp	sp,-48
2005a428:	d226                	c.swsp	s1,36(sp)
2005a42a:	eb41a783          	lw	a5,-332(gp) # 20066864 <task_status>
2005a42e:	6685                	c.lui	a3,0x1
2005a430:	d606                	c.swsp	ra,44(sp)
2005a432:	d422                	c.swsp	s0,40(sp)
2005a434:	d04a                	c.swsp	s2,32(sp)
2005a436:	ce4e                	c.swsp	s3,28(sp)
2005a438:	cc52                	c.swsp	s4,24(sp)
2005a43a:	ca56                	c.swsp	s5,20(sp)
2005a43c:	c85a                	c.swsp	s6,16(sp)
2005a43e:	c65e                	c.swsp	s7,12(sp)
2005a440:	c462                	c.swsp	s8,8(sp)
2005a442:	00d78733          	add	a4,a5,a3
2005a446:	20072603          	lw	a2,512(a4)
2005a44a:	eb418493          	addi	s1,gp,-332 # 20066864 <task_status>
2005a44e:	ea19                	c.bnez	a2,2005a464 <update_status+0x3e>
2005a450:	20472603          	lw	a2,516(a4)
2005a454:	ea01                	c.bnez	a2,2005a464 <update_status+0x3e>
2005a456:	90068693          	addi	a3,a3,-1792 # 900 <BKUP_Set+0x5a>
2005a45a:	20f72023          	sw	a5,512(a4)
2005a45e:	97b6                	c.add	a5,a3
2005a460:	20f72223          	sw	a5,516(a4)
2005a464:	28a030ef          	jal	ra,2005d6ee <uxTaskGetNumberOfTasks>
2005a468:	04000793          	addi	a5,zero,64
2005a46c:	02a7fd63          	bgeu	a5,a0,2005a4a6 <update_status+0x80>
2005a470:	27e030ef          	jal	ra,2005d6ee <uxTaskGetNumberOfTasks>
2005a474:	5422                	c.lwsp	s0,40(sp)
2005a476:	50b2                	c.lwsp	ra,44(sp)
2005a478:	5492                	c.lwsp	s1,36(sp)
2005a47a:	5902                	c.lwsp	s2,32(sp)
2005a47c:	49f2                	c.lwsp	s3,28(sp)
2005a47e:	4a62                	c.lwsp	s4,24(sp)
2005a480:	4ad2                	c.lwsp	s5,20(sp)
2005a482:	4b42                	c.lwsp	s6,16(sp)
2005a484:	4bb2                	c.lwsp	s7,12(sp)
2005a486:	4c22                	c.lwsp	s8,8(sp)
2005a488:	200636b7          	lui	a3,0x20063
2005a48c:	200635b7          	lui	a1,0x20063
2005a490:	872a                	c.mv	a4,a0
2005a492:	bf468693          	addi	a3,a3,-1036 # 20062bf4 <pmap_func+0x828>
2005a496:	05700613          	addi	a2,zero,87
2005a49a:	99058593          	addi	a1,a1,-1648 # 20062990 <pmap_func+0x5c4>
2005a49e:	450d                	c.li	a0,3
2005a4a0:	6145                	c.addi16sp	sp,48
2005a4a2:	4d50006f          	jal	zero,2005b176 <rtk_log_write>
2005a4a6:	409c                	c.lw	a5,0(s1)
2005a4a8:	6405                	c.lui	s0,0x1
2005a4aa:	21040913          	addi	s2,s0,528 # 1210 <CPU_ClkGet+0x6fa>
2005a4ae:	97a2                	c.add	a5,s0
2005a4b0:	20078793          	addi	a5,a5,512
2005a4b4:	47d8                	c.lw	a4,12(a5)
2005a4b6:	43d4                	c.lw	a3,4(a5)
2005a4b8:	4a01                	c.li	s4,0
2005a4ba:	c798                	c.sw	a4,8(a5)
2005a4bc:	4398                	c.lw	a4,0(a5)
2005a4be:	c394                	c.sw	a3,0(a5)
2005a4c0:	c3d8                	c.sw	a4,4(a5)
2005a4c2:	22c030ef          	jal	ra,2005d6ee <uxTaskGetNumberOfTasks>
2005a4c6:	409c                	c.lw	a5,0(s1)
2005a4c8:	85aa                	c.mv	a1,a0
2005a4ca:	4601                	c.li	a2,0
2005a4cc:	97a2                	c.add	a5,s0
2005a4ce:	20a7a623          	sw	a0,524(a5)
2005a4d2:	2047a503          	lw	a0,516(a5)
2005a4d6:	591030ef          	jal	ra,2005e266 <uxTaskGetSystemState>
2005a4da:	409c                	c.lw	a5,0(s1)
2005a4dc:	e0040613          	addi	a2,s0,-512
2005a4e0:	4581                	c.li	a1,0
2005a4e2:	00878733          	add	a4,a5,s0
2005a4e6:	20a72623          	sw	a0,524(a4)
2005a4ea:	01278533          	add	a0,a5,s2
2005a4ee:	dffaf097          	auipc	ra,0xdffaf
2005a4f2:	5e0080e7          	jalr	ra,1504(ra) # 9ace <__wrap_memset>
2005a4f6:	0004aa83          	lw	s5,0(s1)
2005a4fa:	9456                	c.add	s0,s5
2005a4fc:	20842783          	lw	a5,520(s0)
2005a500:	00f05963          	bge	zero,a5,2005a512 <update_status+0xec>
2005a504:	02400b93          	addi	s7,zero,36
2005a508:	9aca                	c.add	s5,s2
2005a50a:	20c42783          	lw	a5,524(s0)
2005a50e:	00fa4e63          	blt	s4,a5,2005a52a <update_status+0x104>
2005a512:	50b2                	c.lwsp	ra,44(sp)
2005a514:	5422                	c.lwsp	s0,40(sp)
2005a516:	5492                	c.lwsp	s1,36(sp)
2005a518:	5902                	c.lwsp	s2,32(sp)
2005a51a:	49f2                	c.lwsp	s3,28(sp)
2005a51c:	4a62                	c.lwsp	s4,24(sp)
2005a51e:	4ad2                	c.lwsp	s5,20(sp)
2005a520:	4b42                	c.lwsp	s6,16(sp)
2005a522:	4bb2                	c.lwsp	s7,12(sp)
2005a524:	4c22                	c.lwsp	s8,8(sp)
2005a526:	6145                	c.addi16sp	sp,48
2005a528:	8082                	c.jr	ra
2005a52a:	037a0933          	mul	s2,s4,s7
2005a52e:	20442b03          	lw	s6,516(s0)
2005a532:	20842603          	lw	a2,520(s0)
2005a536:	20042583          	lw	a1,512(s0)
2005a53a:	9b4a                	c.add	s6,s2
2005a53c:	008b2503          	lw	a0,8(s6)
2005a540:	3d45                	c.jal	2005a3f0 <find_status>
2005a542:	04000593          	addi	a1,zero,64
2005a546:	8c2a                	c.mv	s8,a0
2005a548:	8556                	c.mv	a0,s5
2005a54a:	35c9                	c.jal	2005a40c <find_free_delta>
2005a54c:	000b2783          	lw	a5,0(s6)
2005a550:	84aa                	c.mv	s1,a0
2005a552:	467d                	c.li	a2,31
2005a554:	d51c                	c.sw	a5,40(a0)
2005a556:	20442783          	lw	a5,516(s0)
2005a55a:	97ca                	c.add	a5,s2
2005a55c:	43cc                	c.lw	a1,4(a5)
2005a55e:	c14c                	c.sw	a1,4(a0)
2005a560:	20442783          	lw	a5,516(s0)
2005a564:	97ca                	c.add	a5,s2
2005a566:	479c                	c.lw	a5,8(a5)
2005a568:	00052023          	sw	zero,0(a0)
2005a56c:	0521                	c.addi	a0,8
2005a56e:	d15c                	c.sw	a5,36(a0)
2005a570:	20442783          	lw	a5,516(s0)
2005a574:	97ca                	c.add	a5,s2
2005a576:	00c7c783          	lbu	a5,12(a5)
2005a57a:	02f50623          	sb	a5,44(a0)
2005a57e:	20442783          	lw	a5,516(s0)
2005a582:	97ca                	c.add	a5,s2
2005a584:	4b9c                	c.lw	a5,16(a5)
2005a586:	d51c                	c.sw	a5,40(a0)
2005a588:	003070ef          	jal	ra,20061d8a <strncpy>
2005a58c:	c095                	c.beqz	s1,2005a5b0 <update_status+0x18a>
2005a58e:	20442783          	lw	a5,516(s0)
2005a592:	97ca                	c.add	a5,s2
2005a594:	4f9c                	c.lw	a5,24(a5)
2005a596:	000c0563          	beq	s8,zero,2005a5a0 <update_status+0x17a>
2005a59a:	018c2703          	lw	a4,24(s8)
2005a59e:	8f99                	c.sub	a5,a4
2005a5a0:	c09c                	c.sw	a5,0(s1)
2005a5a2:	20442783          	lw	a5,516(s0)
2005a5a6:	993e                	c.add	s2,a5
2005a5a8:	00c94783          	lbu	a5,12(s2)
2005a5ac:	02f48a23          	sb	a5,52(s1)
2005a5b0:	0a05                	c.addi	s4,1
2005a5b2:	bfa1                	c.j	2005a50a <update_status+0xe4>

2005a5b4 <print_delta>:
2005a5b4:	715d                	c.addi16sp	sp,-80
2005a5b6:	736277b7          	lui	a5,0x73627
2005a5ba:	c4a2                	c.swsp	s0,72(sp)
2005a5bc:	25278793          	addi	a5,a5,594 # 73627252 <__kr4_psram_text_start__+0x134a7232>
2005a5c0:	842a                	c.mv	s0,a0
2005a5c2:	20063737          	lui	a4,0x20063
2005a5c6:	200636b7          	lui	a3,0x20063
2005a5ca:	20063637          	lui	a2,0x20063
2005a5ce:	200635b7          	lui	a1,0x20063
2005a5d2:	20063537          	lui	a0,0x20063
2005a5d6:	c43e                	c.swsp	a5,8(sp)
2005a5d8:	c3870713          	addi	a4,a4,-968 # 20062c38 <pmap_func+0x86c>
2005a5dc:	04400793          	addi	a5,zero,68
2005a5e0:	c4068693          	addi	a3,a3,-960 # 20062c40 <pmap_func+0x874>
2005a5e4:	c4860613          	addi	a2,a2,-952 # 20062c48 <pmap_func+0x87c>
2005a5e8:	c5058593          	addi	a1,a1,-944 # 20062c50 <pmap_func+0x884>
2005a5ec:	c5c50513          	addi	a0,a0,-932 # 20062c5c <pmap_func+0x890>
2005a5f0:	c2a6                	c.swsp	s1,68(sp)
2005a5f2:	c0ca                	c.swsp	s2,64(sp)
2005a5f4:	de4e                	c.swsp	s3,60(sp)
2005a5f6:	dc52                	c.swsp	s4,56(sp)
2005a5f8:	da56                	c.swsp	s5,52(sp)
2005a5fa:	d85a                	c.swsp	s6,48(sp)
2005a5fc:	d65e                	c.swsp	s7,44(sp)
2005a5fe:	ee22                	c.fswsp	fs0,28(sp)
2005a600:	00f10623          	sb	a5,12(sp)
2005a604:	c686                	c.swsp	ra,76(sp)
2005a606:	00011223          	sh	zero,4(sp)
2005a60a:	00010323          	sb	zero,6(sp)
2005a60e:	5f5040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a612:	e501a407          	flw	fs0,-432(gp) # 20066800 <__sdata_start__>
2005a616:	6485                	c.lui	s1,0x1
2005a618:	4b81                	c.li	s7,0
2005a61a:	03800993          	addi	s3,zero,56
2005a61e:	06400b13          	addi	s6,zero,100
2005a622:	21848a13          	addi	s4,s1,536 # 1218 <CPU_ClkGet+0x702>
2005a626:	20063ab7          	lui	s5,0x20063
2005a62a:	460d                	c.li	a2,3
2005a62c:	4581                	c.li	a1,0
2005a62e:	0048                	c.addi4spn	a0,sp,4
2005a630:	dffaf097          	auipc	ra,0xdffaf
2005a634:	49e080e7          	jalr	ra,1182(ra) # 9ace <__wrap_memset>
2005a638:	033b8633          	mul	a2,s7,s3
2005a63c:	eb41a803          	lw	a6,-332(gp) # 20066864 <task_status>
2005a640:	d0047753          	fcvt.s.w	fa4,s0
2005a644:	4529                	c.li	a0,10
2005a646:	00c806b3          	add	a3,a6,a2
2005a64a:	96a6                	c.add	a3,s1
2005a64c:	2106a783          	lw	a5,528(a3)
2005a650:	d007f7d3          	fcvt.s.w	fa5,a5
2005a654:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a658:	18e7f7d3          	fdiv.s	fa5,fa5,fa4
2005a65c:	1087f7d3          	fmul.s	fa5,fa5,fs0
2005a660:	c0079753          	fcvt.w.s	a4,fa5,rtz
2005a664:	036767b3          	rem	a5,a4,s6
2005a668:	02a787b3          	mul	a5,a5,a0
2005a66c:	0367c5b3          	div	a1,a5,s6
2005a670:	0367e7b3          	rem	a5,a5,s6
2005a674:	03058593          	addi	a1,a1,48
2005a678:	00b10223          	sb	a1,4(sp)
2005a67c:	02a787b3          	mul	a5,a5,a0
2005a680:	0367c7b3          	div	a5,a5,s6
2005a684:	03078793          	addi	a5,a5,48
2005a688:	00f102a3          	sb	a5,5(sp)
2005a68c:	2386a783          	lw	a5,568(a3)
2005a690:	c78d                	c.beqz	a5,2005a6ba <print_delta+0x106>
2005a692:	03674733          	div	a4,a4,s6
2005a696:	2446c503          	lbu	a0,580(a3)
2005a69a:	014605b3          	add	a1,a2,s4
2005a69e:	2406a683          	lw	a3,576(a3)
2005a6a2:	01050793          	addi	a5,a0,16
2005a6a6:	00278533          	add	a0,a5,sp
2005a6aa:	ff854603          	lbu	a2,-8(a0)
2005a6ae:	005c                	c.addi4spn	a5,sp,4
2005a6b0:	95c2                	c.add	a1,a6
2005a6b2:	c6ca8513          	addi	a0,s5,-916 # 20062c6c <pmap_func+0x8a0>
2005a6b6:	54d040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a6ba:	0b85                	c.addi	s7,1
2005a6bc:	04000793          	addi	a5,zero,64
2005a6c0:	f6fb95e3          	bne	s7,a5,2005a62a <print_delta+0x76>
2005a6c4:	40b6                	c.lwsp	ra,76(sp)
2005a6c6:	4426                	c.lwsp	s0,72(sp)
2005a6c8:	4496                	c.lwsp	s1,68(sp)
2005a6ca:	4906                	c.lwsp	s2,64(sp)
2005a6cc:	59f2                	c.lwsp	s3,60(sp)
2005a6ce:	5a62                	c.lwsp	s4,56(sp)
2005a6d0:	5ad2                	c.lwsp	s5,52(sp)
2005a6d2:	5b42                	c.lwsp	s6,48(sp)
2005a6d4:	5bb2                	c.lwsp	s7,44(sp)
2005a6d6:	6472                	c.flwsp	fs0,28(sp)
2005a6d8:	6161                	c.addi16sp	sp,80
2005a6da:	8082                	c.jr	ra

2005a6dc <cpu_stat_thread>:
2005a6dc:	7179                	c.addi16sp	sp,-48
2005a6de:	c62a                	c.swsp	a0,12(sp)
2005a6e0:	4521                	c.li	a0,8
2005a6e2:	d226                	c.swsp	s1,36(sp)
2005a6e4:	d606                	c.swsp	ra,44(sp)
2005a6e6:	d422                	c.swsp	s0,40(sp)
2005a6e8:	d04a                	c.swsp	s2,32(sp)
2005a6ea:	ce4e                	c.swsp	s3,28(sp)
2005a6ec:	cc52                	c.swsp	s4,24(sp)
2005a6ee:	ca56                	c.swsp	s5,20(sp)
2005a6f0:	7ed000ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005a6f4:	45b2                	c.lwsp	a1,12(sp)
2005a6f6:	eb818493          	addi	s1,gp,-328 # 20066868 <top_exit_sema>
2005a6fa:	e131                	c.bnez	a0,2005a73e <cpu_stat_thread+0x62>
2005a6fc:	200635b7          	lui	a1,0x20063
2005a700:	20063537          	lui	a0,0x20063
2005a704:	f3858593          	addi	a1,a1,-200 # 20062f38 <__FUNCTION__.0>
2005a708:	c8050513          	addi	a0,a0,-896 # 20062c80 <pmap_func+0x8b4>
2005a70c:	4f7040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a710:	4088                	c.lw	a0,0(s1)
2005a712:	eb418413          	addi	s0,gp,-332 # 20066864 <task_status>
2005a716:	1de010ef          	jal	ra,2005b8f4 <rtos_sema_delete>
2005a71a:	4008                	c.lw	a0,0(s0)
2005a71c:	0004a023          	sw	zero,0(s1)
2005a720:	7e7000ef          	jal	ra,2005b706 <rtos_mem_free>
2005a724:	00042023          	sw	zero,0(s0)
2005a728:	5422                	c.lwsp	s0,40(sp)
2005a72a:	50b2                	c.lwsp	ra,44(sp)
2005a72c:	5492                	c.lwsp	s1,36(sp)
2005a72e:	5902                	c.lwsp	s2,32(sp)
2005a730:	49f2                	c.lwsp	s3,28(sp)
2005a732:	4a62                	c.lwsp	s4,24(sp)
2005a734:	4ad2                	c.lwsp	s5,20(sp)
2005a736:	4501                	c.li	a0,0
2005a738:	6145                	c.addi16sp	sp,48
2005a73a:	4940106f          	jal	zero,2005bbce <rtos_task_delete>
2005a73e:	4621                	c.li	a2,8
2005a740:	842a                	c.mv	s0,a0
2005a742:	dffaf097          	auipc	ra,0xdffaf
2005a746:	38e080e7          	jalr	ra,910(ra) # 9ad0 <__wrap_memcpy>
2005a74a:	410187b7          	lui	a5,0x41018
2005a74e:	4bdc                	c.lw	a5,20(a5)
2005a750:	3e800a93          	addi	s5,zero,1000
2005a754:	eaf1a423          	sw	a5,-344(gp) # 20066858 <last_tick>
2005a758:	5a7d                	c.li	s4,-1
2005a75a:	ea818913          	addi	s2,gp,-344 # 20066858 <last_tick>
2005a75e:	410189b7          	lui	s3,0x41018
2005a762:	400c                	c.lw	a1,0(s0)
2005a764:	4088                	c.lw	a0,0(s1)
2005a766:	02ba85b3          	mul	a1,s5,a1
2005a76a:	1a0010ef          	jal	ra,2005b90a <rtos_sema_take>
2005a76e:	03451463          	bne	a0,s4,2005a796 <cpu_stat_thread+0xba>
2005a772:	3955                	c.jal	2005a426 <update_status>
2005a774:	0149a503          	lw	a0,20(s3) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a778:	00092783          	lw	a5,0(s2)
2005a77c:	0149a703          	lw	a4,20(s3)
2005a780:	8d1d                	c.sub	a0,a5
2005a782:	00e92023          	sw	a4,0(s2)
2005a786:	353d                	c.jal	2005a5b4 <print_delta>
2005a788:	405c                	c.lw	a5,4(s0)
2005a78a:	00f05563          	bge	zero,a5,2005a794 <cpu_stat_thread+0xb8>
2005a78e:	17fd                	c.addi	a5,-1
2005a790:	c05c                	c.sw	a5,4(s0)
2005a792:	bfc1                	c.j	2005a762 <cpu_stat_thread+0x86>
2005a794:	f7f9                	c.bnez	a5,2005a762 <cpu_stat_thread+0x86>
2005a796:	8522                	c.mv	a0,s0
2005a798:	76f000ef          	jal	ra,2005b706 <rtos_mem_free>
2005a79c:	bf95                	c.j	2005a710 <cpu_stat_thread+0x34>

2005a79e <at_cpuload>:
2005a79e:	7119                	c.addi16sp	sp,-128
2005a7a0:	dca2                	c.swsp	s0,120(sp)
2005a7a2:	05c00613          	addi	a2,zero,92
2005a7a6:	842a                	c.mv	s0,a0
2005a7a8:	4581                	c.li	a1,0
2005a7aa:	0048                	c.addi4spn	a0,sp,4
2005a7ac:	de86                	c.swsp	ra,124(sp)
2005a7ae:	d8ca                	c.swsp	s2,112(sp)
2005a7b0:	d4d2                	c.swsp	s4,104(sp)
2005a7b2:	daa6                	c.swsp	s1,116(sp)
2005a7b4:	d6ce                	c.swsp	s3,108(sp)
2005a7b6:	d2d6                	c.swsp	s5,100(sp)
2005a7b8:	dffaf097          	auipc	ra,0xdffaf
2005a7bc:	316080e7          	jalr	ra,790(ra) # 9ace <__wrap_memset>
2005a7c0:	eac18793          	addi	a5,gp,-340 # 2006685c <para_in>
2005a7c4:	4705                	c.li	a4,1
2005a7c6:	c398                	c.sw	a4,0(a5)
2005a7c8:	004c                	c.addi4spn	a1,sp,4
2005a7ca:	577d                	c.li	a4,-1
2005a7cc:	8522                	c.mv	a0,s0
2005a7ce:	c3d8                	c.sw	a4,4(a5)
2005a7d0:	e58ff0ef          	jal	ra,20059e28 <parse_param>
2005a7d4:	ffe50713          	addi	a4,a0,-2
2005a7d8:	4789                	c.li	a5,2
2005a7da:	20063a37          	lui	s4,0x20063
2005a7de:	06e7f463          	bgeu	a5,a4,2005a846 <at_cpuload+0xa8>
2005a7e2:	20063537          	lui	a0,0x20063
2005a7e6:	c9450513          	addi	a0,a0,-876 # 20062c94 <pmap_func+0x8c8>
2005a7ea:	419040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a7ee:	4405                	c.li	s0,1
2005a7f0:	20063537          	lui	a0,0x20063
2005a7f4:	8622                	c.mv	a2,s0
2005a7f6:	e20a0593          	addi	a1,s4,-480 # 20062e20 <pmap_func+0xa54>
2005a7fa:	ad050513          	addi	a0,a0,-1328 # 20062ad0 <pmap_func+0x704>
2005a7fe:	9875                	c.andi	s0,-3
2005a800:	403040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a804:	14040b63          	beq	s0,zero,2005a95a <at_cpuload+0x1bc>
2005a808:	20063537          	lui	a0,0x20063
2005a80c:	87850513          	addi	a0,a0,-1928 # 20062878 <pmap_func+0x4ac>
2005a810:	3f3040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a814:	20063537          	lui	a0,0x20063
2005a818:	e2c50513          	addi	a0,a0,-468 # 20062e2c <pmap_func+0xa60>
2005a81c:	3e7040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a820:	20063537          	lui	a0,0x20063
2005a824:	e5c50513          	addi	a0,a0,-420 # 20062e5c <pmap_func+0xa90>
2005a828:	3db040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a82c:	20063537          	lui	a0,0x20063
2005a830:	e7050513          	addi	a0,a0,-400 # 20062e70 <pmap_func+0xaa4>
2005a834:	3cf040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a838:	20063537          	lui	a0,0x20063
2005a83c:	e8c50513          	addi	a0,a0,-372 # 20062e8c <pmap_func+0xac0>
2005a840:	3c3040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a844:	aa19                	c.j	2005a95a <at_cpuload+0x1bc>
2005a846:	84aa                	c.mv	s1,a0
2005a848:	4522                	c.lwsp	a0,8(sp)
2005a84a:	eac18993          	addi	s3,gp,-340 # 2006685c <para_in>
2005a84e:	4411                	c.li	s0,4
2005a850:	00054783          	lbu	a5,0(a0)
2005a854:	c789                	c.beqz	a5,2005a85e <at_cpuload+0xc0>
2005a856:	2e2070ef          	jal	ra,20061b38 <atoi>
2005a85a:	0ff57413          	andi	s0,a0,255
2005a85e:	4789                	c.li	a5,2
2005a860:	04f48463          	beq	s1,a5,2005a8a8 <at_cpuload+0x10a>
2005a864:	4532                	c.lwsp	a0,12(sp)
2005a866:	00054783          	lbu	a5,0(a0)
2005a86a:	cf81                	c.beqz	a5,2005a882 <at_cpuload+0xe4>
2005a86c:	2cc070ef          	jal	ra,20061b38 <atoi>
2005a870:	4785                	c.li	a5,1
2005a872:	00a05663          	bge	zero,a0,2005a87e <at_cpuload+0xe0>
2005a876:	4532                	c.lwsp	a0,12(sp)
2005a878:	2c0070ef          	jal	ra,20061b38 <atoi>
2005a87c:	87aa                	c.mv	a5,a0
2005a87e:	00f9a023          	sw	a5,0(s3)
2005a882:	4791                	c.li	a5,4
2005a884:	02f49263          	bne	s1,a5,2005a8a8 <at_cpuload+0x10a>
2005a888:	4542                	c.lwsp	a0,16(sp)
2005a88a:	00054783          	lbu	a5,0(a0)
2005a88e:	cf89                	c.beqz	a5,2005a8a8 <at_cpuload+0x10a>
2005a890:	2a8070ef          	jal	ra,20061b38 <atoi>
2005a894:	57fd                	c.li	a5,-1
2005a896:	00a05763          	bge	zero,a0,2005a8a4 <at_cpuload+0x106>
2005a89a:	4542                	c.lwsp	a0,16(sp)
2005a89c:	29c070ef          	jal	ra,20061b38 <atoi>
2005a8a0:	fff50793          	addi	a5,a0,-1
2005a8a4:	00f9a223          	sw	a5,4(s3)
2005a8a8:	eb418a93          	addi	s5,gp,-332 # 20066864 <task_status>
2005a8ac:	000aa783          	lw	a5,0(s5)
2005a8b0:	eb418493          	addi	s1,gp,-332 # 20066864 <task_status>
2005a8b4:	e79d                	c.bnez	a5,2005a8e2 <at_cpuload+0x144>
2005a8b6:	6509                	c.lui	a0,0x2
2005a8b8:	0541                	c.addi	a0,16
2005a8ba:	623000ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005a8be:	00aaa023          	sw	a0,0(s5)
2005a8c2:	e105                	c.bnez	a0,2005a8e2 <at_cpuload+0x144>
2005a8c4:	200636b7          	lui	a3,0x20063
2005a8c8:	200635b7          	lui	a1,0x20063
2005a8cc:	db068693          	addi	a3,a3,-592 # 20062db0 <pmap_func+0x9e4>
2005a8d0:	04900613          	addi	a2,zero,73
2005a8d4:	99058593          	addi	a1,a1,-1648 # 20062990 <pmap_func+0x5c4>
2005a8d8:	4511                	c.li	a0,4
2005a8da:	09d000ef          	jal	ra,2005b176 <rtk_log_write>
2005a8de:	4409                	c.li	s0,2
2005a8e0:	bf01                	c.j	2005a7f0 <at_cpuload+0x52>
2005a8e2:	0049a683          	lw	a3,4(s3)
2005a8e6:	0009a603          	lw	a2,0(s3)
2005a8ea:	20063537          	lui	a0,0x20063
2005a8ee:	85a2                	c.mv	a1,s0
2005a8f0:	dd450513          	addi	a0,a0,-556 # 20062dd4 <pmap_func+0xa08>
2005a8f4:	30f040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a8f8:	4789                	c.li	a5,2
2005a8fa:	08f40363          	beq	s0,a5,2005a980 <at_cpuload+0x1e2>
2005a8fe:	0687e763          	bltu	a5,s0,2005a96c <at_cpuload+0x1ce>
2005a902:	eb818793          	addi	a5,gp,-328 # 20066868 <top_exit_sema>
2005a906:	e825                	c.bnez	s0,2005a976 <at_cpuload+0x1d8>
2005a908:	439c                	c.lw	a5,0(a5)
2005a90a:	e3a1                	c.bnez	a5,2005a94a <at_cpuload+0x1ac>
2005a90c:	4088                	c.lw	a0,0(s1)
2005a90e:	6609                	c.lui	a2,0x2
2005a910:	0641                	c.addi	a2,16
2005a912:	4581                	c.li	a1,0
2005a914:	dffaf097          	auipc	ra,0xdffaf
2005a918:	1ba080e7          	jalr	ra,442(ra) # 9ace <__wrap_memset>
2005a91c:	b0bff0ef          	jal	ra,2005a426 <update_status>
2005a920:	4605                	c.li	a2,1
2005a922:	4581                	c.li	a1,0
2005a924:	eb818513          	addi	a0,gp,-328 # 20066868 <top_exit_sema>
2005a928:	741000ef          	jal	ra,2005b868 <rtos_sema_create>
2005a92c:	2005a637          	lui	a2,0x2005a
2005a930:	200635b7          	lui	a1,0x20063
2005a934:	47a9                	c.li	a5,10
2005a936:	6711                	c.lui	a4,0x4
2005a938:	eac18693          	addi	a3,gp,-340 # 2006685c <para_in>
2005a93c:	6dc60613          	addi	a2,a2,1756 # 2005a6dc <cpu_stat_thread>
2005a940:	e1058593          	addi	a1,a1,-496 # 20062e10 <pmap_func+0xa44>
2005a944:	4501                	c.li	a0,0
2005a946:	264010ef          	jal	ra,2005bbaa <rtos_task_create>
2005a94a:	20063537          	lui	a0,0x20063
2005a94e:	e20a0593          	addi	a1,s4,-480
2005a952:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
2005a956:	2ad040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a95a:	50f6                	c.lwsp	ra,124(sp)
2005a95c:	5466                	c.lwsp	s0,120(sp)
2005a95e:	54d6                	c.lwsp	s1,116(sp)
2005a960:	5946                	c.lwsp	s2,112(sp)
2005a962:	59b6                	c.lwsp	s3,108(sp)
2005a964:	5a26                	c.lwsp	s4,104(sp)
2005a966:	5a96                	c.lwsp	s5,100(sp)
2005a968:	6109                	c.addi16sp	sp,128
2005a96a:	8082                	c.jr	ra
2005a96c:	478d                	c.li	a5,3
2005a96e:	02f40c63          	beq	s0,a5,2005a9a6 <at_cpuload+0x208>
2005a972:	440d                	c.li	s0,3
2005a974:	bdb5                	c.j	2005a7f0 <at_cpuload+0x52>
2005a976:	4388                	c.lw	a0,0(a5)
2005a978:	d969                	c.beqz	a0,2005a94a <at_cpuload+0x1ac>
2005a97a:	7dd000ef          	jal	ra,2005b956 <rtos_sema_give>
2005a97e:	b7f1                	c.j	2005a94a <at_cpuload+0x1ac>
2005a980:	eb81a783          	lw	a5,-328(gp) # 20066868 <top_exit_sema>
2005a984:	f3f9                	c.bnez	a5,2005a94a <at_cpuload+0x1ac>
2005a986:	4088                	c.lw	a0,0(s1)
2005a988:	6609                	c.lui	a2,0x2
2005a98a:	0641                	c.addi	a2,16
2005a98c:	4581                	c.li	a1,0
2005a98e:	dffaf097          	auipc	ra,0xdffaf
2005a992:	140080e7          	jalr	ra,320(ra) # 9ace <__wrap_memset>
2005a996:	410187b7          	lui	a5,0x41018
2005a99a:	4bd8                	c.lw	a4,20(a5)
2005a99c:	eae1a423          	sw	a4,-344(gp) # 20066858 <last_tick>
2005a9a0:	a87ff0ef          	jal	ra,2005a426 <update_status>
2005a9a4:	b75d                	c.j	2005a94a <at_cpuload+0x1ac>
2005a9a6:	eb81a783          	lw	a5,-328(gp) # 20066868 <top_exit_sema>
2005a9aa:	f3c5                	c.bnez	a5,2005a94a <at_cpuload+0x1ac>
2005a9ac:	a7bff0ef          	jal	ra,2005a426 <update_status>
2005a9b0:	41018937          	lui	s2,0x41018
2005a9b4:	ea818413          	addi	s0,gp,-344 # 20066858 <last_tick>
2005a9b8:	401c                	c.lw	a5,0(s0)
2005a9ba:	01492503          	lw	a0,20(s2) # 41018014 <__km4_bd_boot_download_addr__+0x11006014>
2005a9be:	8d1d                	c.sub	a0,a5
2005a9c0:	bf5ff0ef          	jal	ra,2005a5b4 <print_delta>
2005a9c4:	01492783          	lw	a5,20(s2)
2005a9c8:	4088                	c.lw	a0,0(s1)
2005a9ca:	c01c                	c.sw	a5,0(s0)
2005a9cc:	53b000ef          	jal	ra,2005b706 <rtos_mem_free>
2005a9d0:	0004a023          	sw	zero,0(s1)
2005a9d4:	bf9d                	c.j	2005a94a <at_cpuload+0x1ac>

2005a9d6 <print_system_at>:
2005a9d6:	1141                	c.addi	sp,-16
2005a9d8:	c422                	c.swsp	s0,8(sp)
2005a9da:	ca018413          	addi	s0,gp,-864 # 20066650 <at_sys_items>
2005a9de:	c226                	c.swsp	s1,4(sp)
2005a9e0:	c04a                	c.swsp	s2,0(sp)
2005a9e2:	c606                	c.swsp	ra,12(sp)
2005a9e4:	0b040493          	addi	s1,s0,176
2005a9e8:	20063937          	lui	s2,0x20063
2005a9ec:	400c                	c.lw	a1,0(s0)
2005a9ee:	eac90513          	addi	a0,s2,-340 # 20062eac <pmap_func+0xae0>
2005a9f2:	0441                	c.addi	s0,16
2005a9f4:	20f040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005a9f8:	fe941ae3          	bne	s0,s1,2005a9ec <print_system_at+0x16>
2005a9fc:	40b2                	c.lwsp	ra,12(sp)
2005a9fe:	4422                	c.lwsp	s0,8(sp)
2005aa00:	4492                	c.lwsp	s1,4(sp)
2005aa02:	4902                	c.lwsp	s2,0(sp)
2005aa04:	0141                	c.addi	sp,16
2005aa06:	8082                	c.jr	ra

2005aa08 <at_list>:
2005aa08:	20063537          	lui	a0,0x20063
2005aa0c:	1141                	c.addi	sp,-16
2005aa0e:	eb450513          	addi	a0,a0,-332 # 20062eb4 <pmap_func+0xae8>
2005aa12:	c606                	c.swsp	ra,12(sp)
2005aa14:	1ef040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005aa18:	3f7d                	c.jal	2005a9d6 <print_system_at>
2005aa1a:	40b2                	c.lwsp	ra,12(sp)
2005aa1c:	200635b7          	lui	a1,0x20063
2005aa20:	20063537          	lui	a0,0x20063
2005aa24:	ecc58593          	addi	a1,a1,-308 # 20062ecc <pmap_func+0xb00>
2005aa28:	84c50513          	addi	a0,a0,-1972 # 2006284c <pmap_func+0x480>
2005aa2c:	0141                	c.addi	sp,16
2005aa2e:	1d50406f          	jal	zero,2005f402 <DiagPrintf_minimal>

2005aa32 <at_sys_init>:
2005aa32:	45ad                	c.li	a1,11
2005aa34:	ca018513          	addi	a0,gp,-864 # 20066650 <at_sys_items>
2005aa38:	aeaff06f          	jal	zero,20059d22 <atcmd_service_add_table>

2005aa3c <BOOT_Image1>:
2005aa3c:	20000537          	lui	a0,0x20000
2005aa40:	200017b7          	lui	a5,0x20001
2005aa44:	cac78793          	addi	a5,a5,-852 # 20000cac <__rom_bss_end__>
2005aa48:	00050613          	addi	a2,a0,0 # 20000000 <UserIrqFunTable>
2005aa4c:	1141                	c.addi	sp,-16
2005aa4e:	40c78633          	sub	a2,a5,a2
2005aa52:	4581                	c.li	a1,0
2005aa54:	00050513          	addi	a0,a0,0
2005aa58:	c606                	c.swsp	ra,12(sp)
2005aa5a:	dffaf097          	auipc	ra,0xdffaf
2005aa5e:	d30080e7          	jalr	ra,-720(ra) # 978a <_memset>
2005aa62:	dffa6097          	auipc	ra,0xdffa6
2005aa66:	0b4080e7          	jalr	ra,180(ra) # b16 <CPU_ClkGet>
2005aa6a:	000f47b7          	lui	a5,0xf4
2005aa6e:	24078793          	addi	a5,a5,576 # f4240 <__ap_sram_heap_size+0xb4240>
2005aa72:	02f55533          	divu	a0,a0,a5
2005aa76:	200017b7          	lui	a5,0x20001
2005aa7a:	86a7a023          	sw	a0,-1952(a5) # 20000860 <RBSS_UDELAY_CLK>
2005aa7e:	dffa6097          	auipc	ra,0xdffa6
2005aa82:	ea6080e7          	jalr	ra,-346(ra) # 924 <BOOT_ROM_InitDebugFlg>
2005aa86:	200587b7          	lui	a5,0x20058
2005aa8a:	04078793          	addi	a5,a5,64 # 20058040 <Img2EntryFun0>
2005aa8e:	0017c703          	lbu	a4,1(a5)
2005aa92:	0007c683          	lbu	a3,0(a5)
2005aa96:	40b2                	c.lwsp	ra,12(sp)
2005aa98:	0722                	c.slli	a4,0x8
2005aa9a:	8ed9                	c.or	a3,a4
2005aa9c:	0027c703          	lbu	a4,2(a5)
2005aaa0:	0037c783          	lbu	a5,3(a5)
2005aaa4:	0742                	c.slli	a4,0x10
2005aaa6:	8f55                	c.or	a4,a3
2005aaa8:	07e2                	c.slli	a5,0x18
2005aaaa:	8fd9                	c.or	a5,a4
2005aaac:	0141                	c.addi	sp,16
2005aaae:	8782                	c.jr	a5

2005aab0 <flash_read_word>:
2005aab0:	1141                	c.addi	sp,-16
2005aab2:	852e                	c.mv	a0,a1
2005aab4:	4591                	c.li	a1,4
2005aab6:	c606                	c.swsp	ra,12(sp)
2005aab8:	8bafe0ef          	jal	ra,20058b72 <FLASH_ReadStream>
2005aabc:	40b2                	c.lwsp	ra,12(sp)
2005aabe:	4505                	c.li	a0,1
2005aac0:	0141                	c.addi	sp,16
2005aac2:	8082                	c.jr	ra

2005aac4 <os_heap_init>:
2005aac4:	1141                	c.addi	sp,-16
2005aac6:	c606                	c.swsp	ra,12(sp)
2005aac8:	c422                	c.swsp	s0,8(sp)
2005aaca:	0800                	c.addi4spn	s0,sp,16
2005aacc:	00000793          	addi	a5,zero,0
2005aad0:	85be                	c.mv	a1,a5
2005aad2:	ca0007b7          	lui	a5,0xca000
2005aad6:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005aada:	2869                	c.jal	2005ab74 <os_heap_add>
2005aadc:	67dd                	c.lui	a5,0x17
2005aade:	64078793          	addi	a5,a5,1600 # 17640 <__bdram_heap_buffer_size__>
2005aae2:	85be                	c.mv	a1,a5
2005aae4:	200697b7          	lui	a5,0x20069
2005aae8:	9c078513          	addi	a0,a5,-1600 # 200689c0 <__bdram_heap_buffer_start__>
2005aaec:	2061                	c.jal	2005ab74 <os_heap_add>
2005aaee:	00000793          	addi	a5,zero,0
2005aaf2:	85be                	c.mv	a1,a5
2005aaf4:	ca0007b7          	lui	a5,0xca000
2005aaf8:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005aafc:	28a5                	c.jal	2005ab74 <os_heap_add>
2005aafe:	00000793          	addi	a5,zero,0
2005ab02:	85be                	c.mv	a1,a5
2005ab04:	ca0007b7          	lui	a5,0xca000
2005ab08:	00078513          	addi	a0,a5,0 # ca000000 <__ctrace_end__+0x0>
2005ab0c:	20a5                	c.jal	2005ab74 <os_heap_add>
2005ab0e:	67a1                	c.lui	a5,0x8
2005ab10:	00078793          	addi	a5,a5,0 # 8000 <__sram_bootloader_mp_size__>
2005ab14:	85be                	c.mv	a1,a5
2005ab16:	200127b7          	lui	a5,0x20012
2005ab1a:	00078513          	addi	a0,a5,0 # 20012000 <__sram_bootloader_mp_start__>
2005ab1e:	2899                	c.jal	2005ab74 <os_heap_add>
2005ab20:	ebc1c783          	lbu	a5,-324(gp) # 2006686c <heap_index>
2005ab24:	c391                	c.beqz	a5,2005ab28 <os_heap_init+0x64>
2005ab26:	2809                	c.jal	2005ab38 <os_heap_sort>
2005ab28:	55418513          	addi	a0,gp,1364 # 20066f04 <xHeapRegions>
2005ab2c:	2405                	c.jal	2005ad4c <vPortDefineHeapRegions>
2005ab2e:	0001                	c.addi	zero,0
2005ab30:	40b2                	c.lwsp	ra,12(sp)
2005ab32:	4422                	c.lwsp	s0,8(sp)
2005ab34:	0141                	c.addi	sp,16
2005ab36:	8082                	c.jr	ra

2005ab38 <os_heap_sort>:
2005ab38:	4681                	c.li	a3,0
2005ab3a:	ebc1c703          	lbu	a4,-324(gp) # 2006686c <heap_index>
2005ab3e:	177d                	c.addi	a4,-1
2005ab40:	02e6d963          	bge	a3,a4,2005ab72 <os_heap_sort+0x3a>
2005ab44:	55418793          	addi	a5,gp,1364 # 20066f04 <xHeapRegions>
2005ab48:	4601                	c.li	a2,0
2005ab4a:	8f15                	c.sub	a4,a3
2005ab4c:	a839                	c.j	2005ab6a <os_heap_sort+0x32>
2005ab4e:	438c                	c.lw	a1,0(a5)
2005ab50:	4788                	c.lw	a0,8(a5)
2005ab52:	0605                	c.addi	a2,1
2005ab54:	00b57a63          	bgeu	a0,a1,2005ab68 <os_heap_sort+0x30>
2005ab58:	0047a303          	lw	t1,4(a5)
2005ab5c:	c388                	c.sw	a0,0(a5)
2005ab5e:	47c8                	c.lw	a0,12(a5)
2005ab60:	c78c                	c.sw	a1,8(a5)
2005ab62:	0067a623          	sw	t1,12(a5)
2005ab66:	c3c8                	c.sw	a0,4(a5)
2005ab68:	07a1                	c.addi	a5,8
2005ab6a:	fee642e3          	blt	a2,a4,2005ab4e <os_heap_sort+0x16>
2005ab6e:	0685                	c.addi	a3,1
2005ab70:	b7e9                	c.j	2005ab3a <os_heap_sort+0x2>
2005ab72:	8082                	c.jr	ra

2005ab74 <os_heap_add>:
2005ab74:	ebc1c683          	lbu	a3,-324(gp) # 2006686c <heap_index>
2005ab78:	4795                	c.li	a5,5
2005ab7a:	00d7fc63          	bgeu	a5,a3,2005ab92 <os_heap_add+0x1e>
2005ab7e:	20063537          	lui	a0,0x20063
2005ab82:	1141                	c.addi	sp,-16
2005ab84:	02600593          	addi	a1,zero,38
2005ab88:	f4850513          	addi	a0,a0,-184 # 20062f48 <__FUNCTION__.0+0x10>
2005ab8c:	c606                	c.swsp	ra,12(sp)
2005ab8e:	57b040ef          	jal	ra,2005f908 <io_assert_failed>
2005ab92:	00369793          	slli	a5,a3,0x3
2005ab96:	55418713          	addi	a4,gp,1364 # 20066f04 <xHeapRegions>
2005ab9a:	97ba                	c.add	a5,a4
2005ab9c:	c388                	c.sw	a0,0(a5)
2005ab9e:	c3cc                	c.sw	a1,4(a5)
2005aba0:	4501                	c.li	a0,0
2005aba2:	c991                	c.beqz	a1,2005abb6 <os_heap_add+0x42>
2005aba4:	ec018793          	addi	a5,gp,-320 # 20066870 <total_heap_size>
2005aba8:	4398                	c.lw	a4,0(a5)
2005abaa:	0685                	c.addi	a3,1
2005abac:	ead18e23          	sb	a3,-324(gp) # 2006686c <heap_index>
2005abb0:	95ba                	c.add	a1,a4
2005abb2:	c38c                	c.sw	a1,0(a5)
2005abb4:	4505                	c.li	a0,1
2005abb6:	8082                	c.jr	ra

2005abb8 <__wrap_malloc>:
2005abb8:	3250006f          	jal	zero,2005b6dc <rtos_mem_malloc>

2005abbc <__wrap_free>:
2005abbc:	34b0006f          	jal	zero,2005b706 <rtos_mem_free>

2005abc0 <__wrap__free_r>:
2005abc0:	852e                	c.mv	a0,a1
2005abc2:	3450006f          	jal	zero,2005b706 <rtos_mem_free>

2005abc6 <prvInsertBlockIntoFreeList>:
2005abc6:	edc18793          	addi	a5,gp,-292 # 2006688c <xStart>
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
2005abee:	ec41a603          	lw	a2,-316(gp) # 20066874 <pxEnd>
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
2005ac1e:	ec81aa03          	lw	s4,-312(gp) # 20066878 <xBlockAllocatedBit>
2005ac22:	008a77b3          	and	a5,s4,s0
2005ac26:	c799                	c.beqz	a5,2005ac34 <pvPortMalloc+0x2a>
2005ac28:	37c030ef          	jal	ra,2005dfa4 <xTaskResumeAll>
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
2005ac56:	ecc1a483          	lw	s1,-308(gp) # 2006687c <xFreeBytesRemaining>
2005ac5a:	ecc18993          	addi	s3,gp,-308 # 2006687c <xFreeBytesRemaining>
2005ac5e:	fcf4e5e3          	bltu	s1,a5,2005ac28 <pvPortMalloc+0x1e>
2005ac62:	edc1a403          	lw	s0,-292(gp) # 2006688c <xStart>
2005ac66:	edc18713          	addi	a4,gp,-292 # 2006688c <xStart>
2005ac6a:	4054                	c.lw	a3,4(s0)
2005ac6c:	00f6f463          	bgeu	a3,a5,2005ac74 <pvPortMalloc+0x6a>
2005ac70:	4014                	c.lw	a3,0(s0)
2005ac72:	eaa5                	c.bnez	a3,2005ace2 <pvPortMalloc+0xd8>
2005ac74:	ec41a683          	lw	a3,-316(gp) # 20066874 <pxEnd>
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
2005aca0:	ed018693          	addi	a3,gp,-304 # 20066880 <xMinimumEverFreeBytesRemaining>
2005aca4:	4290                	c.lw	a2,0(a3)
2005aca6:	40e487b3          	sub	a5,s1,a4
2005acaa:	00f9a023          	sw	a5,0(s3)
2005acae:	00c7f363          	bgeu	a5,a2,2005acb4 <pvPortMalloc+0xaa>
2005acb2:	c29c                	c.sw	a5,0(a3)
2005acb4:	00ea6733          	or	a4,s4,a4
2005acb8:	ed418793          	addi	a5,gp,-300 # 20066884 <xNumberOfSuccessfulAllocations>
2005acbc:	c058                	c.sw	a4,4(s0)
2005acbe:	4398                	c.lw	a4,0(a5)
2005acc0:	00042023          	sw	zero,0(s0)
2005acc4:	0705                	c.addi	a4,1
2005acc6:	c398                	c.sw	a4,0(a5)
2005acc8:	2dc030ef          	jal	ra,2005dfa4 <xTaskResumeAll>
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
2005acf4:	ec81a783          	lw	a5,-312(gp) # 20066878 <xBlockAllocatedBit>
2005acf8:	842a                	c.mv	s0,a0
2005acfa:	00f776b3          	and	a3,a4,a5
2005acfe:	ce9d                	c.beqz	a3,2005ad3c <vPortFree+0x54>
2005ad00:	fe052683          	lw	a3,-32(a0)
2005ad04:	ee85                	c.bnez	a3,2005ad3c <vPortFree+0x54>
2005ad06:	fff7c793          	xori	a5,a5,-1
2005ad0a:	8ff9                	c.and	a5,a4
2005ad0c:	fef52223          	sw	a5,-28(a0)
2005ad10:	1cd020ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005ad14:	ecc18793          	addi	a5,gp,-308 # 2006687c <xFreeBytesRemaining>
2005ad18:	4394                	c.lw	a3,0(a5)
2005ad1a:	fe442703          	lw	a4,-28(s0)
2005ad1e:	fe040513          	addi	a0,s0,-32
2005ad22:	9736                	c.add	a4,a3
2005ad24:	c398                	c.sw	a4,0(a5)
2005ad26:	3545                	c.jal	2005abc6 <prvInsertBlockIntoFreeList>
2005ad28:	ed818793          	addi	a5,gp,-296 # 20066888 <xNumberOfSuccessfulFrees>
2005ad2c:	4398                	c.lw	a4,0(a5)
2005ad2e:	0705                	c.addi	a4,1
2005ad30:	c398                	c.sw	a4,0(a5)
2005ad32:	4422                	c.lwsp	s0,8(sp)
2005ad34:	40b2                	c.lwsp	ra,12(sp)
2005ad36:	0141                	c.addi	sp,16
2005ad38:	26c0306f          	jal	zero,2005dfa4 <xTaskResumeAll>
2005ad3c:	40b2                	c.lwsp	ra,12(sp)
2005ad3e:	4422                	c.lwsp	s0,8(sp)
2005ad40:	0141                	c.addi	sp,16
2005ad42:	8082                	c.jr	ra
2005ad44:	8082                	c.jr	ra

2005ad46 <xPortGetFreeHeapSize>:
2005ad46:	ecc1a503          	lw	a0,-308(gp) # 2006687c <xFreeBytesRemaining>
2005ad4a:	8082                	c.jr	ra

2005ad4c <vPortDefineHeapRegions>:
2005ad4c:	ec41a583          	lw	a1,-316(gp) # 20066874 <pxEnd>
2005ad50:	4701                	c.li	a4,0
2005ad52:	4301                	c.li	t1,0
2005ad54:	4681                	c.li	a3,0
2005ad56:	ec418613          	addi	a2,gp,-316 # 20066874 <pxEnd>
2005ad5a:	edc18813          	addi	a6,gp,-292 # 2006688c <xStart>
2005ad5e:	415c                	c.lw	a5,4(a0)
2005ad60:	ef81                	c.bnez	a5,2005ad78 <vPortDefineHeapRegions+0x2c>
2005ad62:	c311                	c.beqz	a4,2005ad66 <vPortDefineHeapRegions+0x1a>
2005ad64:	c20c                	c.sw	a1,0(a2)
2005ad66:	ecd1a823          	sw	a3,-304(gp) # 20066880 <xMinimumEverFreeBytesRemaining>
2005ad6a:	ecd1a623          	sw	a3,-308(gp) # 2006687c <xFreeBytesRemaining>
2005ad6e:	80000737          	lui	a4,0x80000
2005ad72:	ece1a423          	sw	a4,-312(gp) # 20066878 <xBlockAllocatedBit>
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
2005adda:	edc1a783          	lw	a5,-292(gp) # 2006688c <xStart>
2005adde:	cfb9                	c.beqz	a5,2005ae3c <vPortGetHeapStats+0x74>
2005ade0:	ec41a683          	lw	a3,-316(gp) # 20066874 <pxEnd>
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
2005ae02:	1a2030ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005ae06:	01342223          	sw	s3,4(s0)
2005ae0a:	01242423          	sw	s2,8(s0)
2005ae0e:	c444                	c.sw	s1,12(s0)
2005ae10:	60d020ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005ae14:	ecc1a783          	lw	a5,-308(gp) # 2006687c <xFreeBytesRemaining>
2005ae18:	40f2                	c.lwsp	ra,28(sp)
2005ae1a:	44d2                	c.lwsp	s1,20(sp)
2005ae1c:	c01c                	c.sw	a5,0(s0)
2005ae1e:	ed41a783          	lw	a5,-300(gp) # 20066884 <xNumberOfSuccessfulAllocations>
2005ae22:	4942                	c.lwsp	s2,16(sp)
2005ae24:	49b2                	c.lwsp	s3,12(sp)
2005ae26:	c85c                	c.sw	a5,20(s0)
2005ae28:	ed81a783          	lw	a5,-296(gp) # 20066888 <xNumberOfSuccessfulFrees>
2005ae2c:	cc1c                	c.sw	a5,24(s0)
2005ae2e:	ed01a783          	lw	a5,-304(gp) # 20066880 <xMinimumEverFreeBytesRemaining>
2005ae32:	c81c                	c.sw	a5,16(s0)
2005ae34:	4462                	c.lwsp	s0,24(sp)
2005ae36:	6105                	c.addi16sp	sp,32
2005ae38:	5ff0206f          	jal	zero,2005dc36 <vTaskExitCritical>
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
2005aed2:	ee418793          	addi	a5,gp,-284 # 20066894 <rtk_log_entry_count>
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
2005af0c:	fe498513          	addi	a0,s3,-28 # 20062fe4 <__FUNCTION__.0+0xac>
2005af10:	4f2040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005af14:	0485                	c.addi	s1,1
2005af16:	042d                	c.addi	s0,11
2005af18:	bfe9                	c.j	2005aef2 <rtk_log_array_print+0x2c>

2005af1a <rtk_log_array_clear>:
2005af1a:	1141                	c.addi	sp,-16
2005af1c:	02c00613          	addi	a2,zero,44
2005af20:	4581                	c.li	a1,0
2005af22:	58418513          	addi	a0,gp,1412 # 20066f34 <rtk_log_tag_array>
2005af26:	c606                	c.swsp	ra,12(sp)
2005af28:	dffaf097          	auipc	ra,0xdffaf
2005af2c:	862080e7          	jalr	ra,-1950(ra) # 978a <_memset>
2005af30:	40b2                	c.lwsp	ra,12(sp)
2005af32:	ee01a223          	sw	zero,-284(gp) # 20066894 <rtk_log_entry_count>
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
2005af48:	ee418793          	addi	a5,gp,-284 # 20066894 <rtk_log_entry_count>
2005af4c:	4394                	c.lw	a3,0(a5)
2005af4e:	470d                	c.li	a4,3
2005af50:	89aa                	c.mv	s3,a0
2005af52:	4a11                	c.li	s4,4
2005af54:	00d76463          	bltu	a4,a3,2005af5c <rtk_log_level_get+0x22>
2005af58:	0007aa03          	lw	s4,0(a5)
2005af5c:	00098d63          	beq	s3,zero,2005af76 <rtk_log_level_get+0x3c>
2005af60:	58418913          	addi	s2,gp,1412 # 20066f34 <rtk_log_tag_array>
2005af64:	0905                	c.addi	s2,1
2005af66:	4401                	c.li	s0,0
2005af68:	58418493          	addi	s1,gp,1412 # 20066f34 <rtk_log_tag_array>
2005af6c:	01441d63          	bne	s0,s4,2005af86 <rtk_log_level_get+0x4c>
2005af70:	e741c503          	lbu	a0,-396(gp) # 20066824 <rtk_log_default_level>
2005af74:	a03d                	c.j	2005afa2 <rtk_log_level_get+0x68>
2005af76:	20063537          	lui	a0,0x20063
2005af7a:	05000593          	addi	a1,zero,80
2005af7e:	ff850513          	addi	a0,a0,-8 # 20062ff8 <__FUNCTION__.0+0xc0>
2005af82:	187040ef          	jal	ra,2005f908 <io_assert_failed>
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
2005afb4:	bf65                	c.j	2005af6c <rtk_log_level_get+0x32>

2005afb6 <rtk_log_level_set>:
2005afb6:	1101                	c.addi	sp,-32
2005afb8:	cc22                	c.swsp	s0,24(sp)
2005afba:	ee418793          	addi	a5,gp,-284 # 20066894 <rtk_log_entry_count>
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
2005afd4:	ee418413          	addi	s0,gp,-284 # 20066894 <rtk_log_entry_count>
2005afd8:	4b11                	c.li	s6,4
2005afda:	00e7e463          	bltu	a5,a4,2005afe2 <rtk_log_level_set+0x2c>
2005afde:	00042b03          	lw	s6,0(s0)
2005afe2:	557d                	c.li	a0,-1
2005afe4:	06090b63          	beq	s2,zero,2005b05a <rtk_log_level_set+0xa4>
2005afe8:	4795                	c.li	a5,5
2005afea:	0737e863          	bltu	a5,s3,2005b05a <rtk_log_level_set+0xa4>
2005afee:	200635b7          	lui	a1,0x20063
2005aff2:	08c58593          	addi	a1,a1,140 # 2006308c <__FUNCTION__.0+0x154>
2005aff6:	854a                	c.mv	a0,s2
2005aff8:	dffaf097          	auipc	ra,0xdffaf
2005affc:	834080e7          	jalr	ra,-1996(ra) # 982c <_strcmp>
2005b000:	c939                	c.beqz	a0,2005b056 <rtk_log_level_set+0xa0>
2005b002:	58418a93          	addi	s5,gp,1412 # 20066f34 <rtk_log_tag_array>
2005b006:	0a85                	c.addi	s5,1
2005b008:	4a01                	c.li	s4,0
2005b00a:	58418493          	addi	s1,gp,1412 # 20066f34 <rtk_log_tag_array>
2005b00e:	076a1063          	bne	s4,s6,2005b06e <rtk_log_level_set+0xb8>
2005b012:	4018                	c.lw	a4,0(s0)
2005b014:	478d                	c.li	a5,3
2005b016:	00e7f863          	bgeu	a5,a4,2005b026 <rtk_log_level_set+0x70>
2005b01a:	20063537          	lui	a0,0x20063
2005b01e:	09050513          	addi	a0,a0,144 # 20063090 <__FUNCTION__.0+0x158>
2005b022:	3e0040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
2005b048:	543060ef          	jal	ra,20061d8a <strncpy>
2005b04c:	401c                	c.lw	a5,0(s0)
2005b04e:	4501                	c.li	a0,0
2005b050:	0785                	c.addi	a5,1
2005b052:	c01c                	c.sw	a5,0(s0)
2005b054:	a019                	c.j	2005b05a <rtk_log_level_set+0xa4>
2005b056:	e7318a23          	sb	s3,-396(gp) # 20066824 <rtk_log_default_level>
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
2005b0ce:	8bc50513          	addi	a0,a0,-1860 # 200628bc <pmap_func+0x4f0>
2005b0d2:	6105                	c.addi16sp	sp,32
2005b0d4:	32e0406f          	jal	zero,2005f402 <DiagPrintf_minimal>
2005b0d8:	ec11                	c.bnez	s0,2005b0f4 <rtk_log_memory_dump_word+0x64>
2005b0da:	85ca                	c.mv	a1,s2
2005b0dc:	0bca8513          	addi	a0,s5,188 # 200630bc <__FUNCTION__.0+0x184>
2005b0e0:	322040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005b0e4:	408c                	c.lw	a1,0(s1)
2005b0e6:	0d0b0513          	addi	a0,s6,208 # 200630d0 <__FUNCTION__.0+0x198>
2005b0ea:	0405                	c.addi	s0,1
2005b0ec:	316040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005b0f0:	0491                	c.addi	s1,4
2005b0f2:	b7d1                	c.j	2005b0b6 <rtk_log_memory_dump_word+0x26>
2005b0f4:	00747793          	andi	a5,s0,7
2005b0f8:	f7f5                	c.bnez	a5,2005b0e4 <rtk_log_memory_dump_word+0x54>
2005b0fa:	85a6                	c.mv	a1,s1
2005b0fc:	0c4a0513          	addi	a0,s4,196 # 200630c4 <__FUNCTION__.0+0x18c>
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
2005b13e:	8bc50513          	addi	a0,a0,-1860 # 200628bc <pmap_func+0x4f0>
2005b142:	6105                	c.addi16sp	sp,32
2005b144:	2be0406f          	jal	zero,2005f402 <DiagPrintf_minimal>
2005b148:	008489b3          	add	s3,s1,s0
2005b14c:	ec11                	c.bnez	s0,2005b168 <rtk_log_memory_dump_byte+0x66>
2005b14e:	85a6                	c.mv	a1,s1
2005b150:	0bca8513          	addi	a0,s5,188 # 200630bc <__FUNCTION__.0+0x184>
2005b154:	2ae040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005b158:	0009c583          	lbu	a1,0(s3)
2005b15c:	0d8b0513          	addi	a0,s6,216 # 200630d8 <__FUNCTION__.0+0x1a0>
2005b160:	0405                	c.addi	s0,1
2005b162:	2a0040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005b166:	b7c1                	c.j	2005b126 <rtk_log_memory_dump_byte+0x24>
2005b168:	00747793          	andi	a5,s0,7
2005b16c:	f7f5                	c.bnez	a5,2005b158 <rtk_log_memory_dump_byte+0x56>
2005b16e:	85ce                	c.mv	a1,s3
2005b170:	0c4a0513          	addi	a0,s4,196 # 200630c4 <__FUNCTION__.0+0x18c>
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
2005b1ac:	0e050513          	addi	a0,a0,224 # 200630e0 <__FUNCTION__.0+0x1a8>
2005b1b0:	232040ef          	jal	ra,2005f3e2 <DiagPrintf>
2005b1b4:	180c                	c.addi4spn	a1,sp,48
2005b1b6:	8522                	c.mv	a0,s0
2005b1b8:	ce2e                	c.swsp	a1,28(sp)
2005b1ba:	72b030ef          	jal	ra,2005f0e4 <DiagVprintf>
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
2005b24c:	ee01a423          	sw	zero,-280(gp) # 20066898 <use_hw_crypto_func>
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
2005b26c:	d90fe0ef          	jal	ra,200597fc <pmu_acquire_wakelock>
2005b270:	4100c7b7          	lui	a5,0x4100c
2005b274:	22c7a783          	lw	a5,556(a5) # 4100c22c <__km4_bd_boot_download_addr__+0x10ffa22c>
2005b278:	8b89                	c.andi	a5,2
2005b27a:	c38d                	c.beqz	a5,2005b29c <app_pmu_init+0x48>
2005b27c:	4509                	c.li	a0,2
2005b27e:	d7efe0ef          	jal	ra,200597fc <pmu_acquire_wakelock>
2005b282:	98dfe0ef          	jal	ra,20059c0e <SWR_CORE_Vol_Get>
2005b286:	4785                	c.li	a5,1
2005b288:	00f51a63          	bne	a0,a5,2005b29c <app_pmu_init+0x48>
2005b28c:	90cfe0ef          	jal	ra,20059398 <dsp_status_on>
2005b290:	c511                	c.beqz	a0,2005b29c <app_pmu_init+0x48>
2005b292:	40b2                	c.lwsp	ra,12(sp)
2005b294:	4511                	c.li	a0,4
2005b296:	0141                	c.addi	sp,16
2005b298:	d64fe06f          	jal	zero,200597fc <pmu_acquire_wakelock>
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
2005b2be:	0ec68693          	addi	a3,a3,236 # 200630ec <__FUNCTION__.0+0x1b4>
2005b2c2:	04900613          	addi	a2,zero,73
2005b2c6:	10040593          	addi	a1,s0,256 # 20063100 <__FUNCTION__.0+0x1c8>
2005b2ca:	4511                	c.li	a0,4
2005b2cc:	ce06                	c.swsp	ra,28(sp)
2005b2ce:	3565                	c.jal	2005b176 <rtk_log_write>
2005b2d0:	2005b7b7          	lui	a5,0x2005b
2005b2d4:	200635b7          	lui	a1,0x20063
2005b2d8:	1f478793          	addi	a5,a5,500 # 2005b1f4 <app_IWDG_refresh>
2005b2dc:	4705                	c.li	a4,1
2005b2de:	1f400693          	addi	a3,zero,500
2005b2e2:	4601                	c.li	a2,0
2005b2e4:	10858593          	addi	a1,a1,264 # 20063108 <__FUNCTION__.0+0x1d0>
2005b2e8:	0068                	c.addi4spn	a0,sp,12
2005b2ea:	c602                	c.swsp	zero,12(sp)
2005b2ec:	14f000ef          	jal	ra,2005bc3a <rtos_timer_create>
2005b2f0:	4532                	c.lwsp	a0,12(sp)
2005b2f2:	ed19                	c.bnez	a0,2005b310 <app_IWDG_int+0x6e>
2005b2f4:	200636b7          	lui	a3,0x20063
2005b2f8:	11468693          	addi	a3,a3,276 # 20063114 <__FUNCTION__.0+0x1dc>
2005b2fc:	04500613          	addi	a2,zero,69
2005b300:	10040593          	addi	a1,s0,256
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
2005b32e:	17250513          	addi	a0,a0,370 # 20059172 <IPC_INTHandler>
2005b332:	c606                	c.swsp	ra,12(sp)
2005b334:	61a040ef          	jal	ra,2005f94e <irq_register>
2005b338:	02b00513          	addi	a0,zero,43
2005b33c:	dffad097          	auipc	ra,0xdffad
2005b340:	c9c080e7          	jalr	ra,-868(ra) # 7fd8 <irq_enable>
2005b344:	35fd                	c.jal	2005b232 <app_mbedtls_rom_init>
2005b346:	41020537          	lui	a0,0x41020
2005b34a:	a2dfd0ef          	jal	ra,20058d76 <ipc_table_init>
2005b34e:	2005c537          	lui	a0,0x2005c
2005b352:	be050513          	addi	a0,a0,-1056 # 2005bbe0 <rtos_time_delay_ms>
2005b356:	ffffd0ef          	jal	ra,20059354 <IPC_SEMDelayStub>
2005b35a:	37c1                	c.jal	2005b31a <app_pre_example>
2005b35c:	72a030ef          	jal	ra,2005ea86 <wlan_initialize>
2005b360:	4581                	c.li	a1,0
2005b362:	4501                	c.li	a0,0
2005b364:	dffae097          	auipc	ra,0xdffae
2005b368:	ef0080e7          	jalr	ra,-272(ra) # 9254 <shell_init_rom>
2005b36c:	5f1010ef          	jal	ra,2005d15c <shell_init_ram>
2005b370:	3d51                	c.jal	2005b204 <app_init_debug>
2005b372:	35cd                	c.jal	2005b254 <app_pmu_init>
2005b374:	373d                	c.jal	2005b2a2 <app_IWDG_int>
2005b376:	375d                	c.jal	2005b31c <app_example>
2005b378:	200636b7          	lui	a3,0x20063
2005b37c:	200635b7          	lui	a1,0x20063
2005b380:	12868693          	addi	a3,a3,296 # 20063128 <__FUNCTION__.0+0x1f0>
2005b384:	04900613          	addi	a2,zero,73
2005b388:	10058593          	addi	a1,a1,256 # 20063100 <__FUNCTION__.0+0x1c8>
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
2005b3ac:	c8448793          	addi	a5,s1,-892 # 20064c84 <shell_cmd_table>
2005b3b0:	cc440413          	addi	s0,s0,-828 # 20064cc4 <ipc_tickless_table>
2005b3b4:	8c1d                	c.sub	s0,a5
2005b3b6:	8011                	c.srli	s0,0x4
2005b3b8:	20063537          	lui	a0,0x20063
2005b3bc:	85a2                	c.mv	a1,s0
2005b3be:	13850513          	addi	a0,a0,312 # 20063138 <__FUNCTION__.0+0x200>
2005b3c2:	c84a                	c.swsp	s2,16(sp)
2005b3c4:	c64e                	c.swsp	s3,12(sp)
2005b3c6:	ce06                	c.swsp	ra,28(sp)
2005b3c8:	c8448493          	addi	s1,s1,-892
2005b3cc:	036040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005b3d0:	4901                	c.li	s2,0
2005b3d2:	200639b7          	lui	s3,0x20063
2005b3d6:	02891163          	bne	s2,s0,2005b3f8 <CmdRamHelp+0x5a>
2005b3da:	20063537          	lui	a0,0x20063
2005b3de:	85ca                	c.mv	a1,s2
2005b3e0:	17c50513          	addi	a0,a0,380 # 2006317c <__FUNCTION__.0+0x244>
2005b3e4:	01e040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
2005b3fc:	17898513          	addi	a0,s3,376 # 20063178 <__FUNCTION__.0+0x240>
2005b400:	002040ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005b404:	0905                	c.addi	s2,1
2005b406:	04c1                	c.addi	s1,16
2005b408:	b7f9                	c.j	2005b3d6 <CmdRamHelp+0x38>

2005b40a <cmd_reboot>:
2005b40a:	1141                	c.addi	sp,-16
2005b40c:	c226                	c.swsp	s1,4(sp)
2005b40e:	84aa                	c.mv	s1,a0
2005b410:	20063537          	lui	a0,0x20063
2005b414:	1bc50513          	addi	a0,a0,444 # 200631bc <__FUNCTION__.0+0x284>
2005b418:	c422                	c.swsp	s0,8(sp)
2005b41a:	c606                	c.swsp	ra,12(sp)
2005b41c:	842e                	c.mv	s0,a1
2005b41e:	7e5030ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005b422:	c08d                	c.beqz	s1,2005b444 <cmd_reboot+0x3a>
2005b424:	4008                	c.lw	a0,0(s0)
2005b426:	200635b7          	lui	a1,0x20063
2005b42a:	1cc58593          	addi	a1,a1,460 # 200631cc <__FUNCTION__.0+0x294>
2005b42e:	dffae097          	auipc	ra,0xdffae
2005b432:	3fe080e7          	jalr	ra,1022(ra) # 982c <_strcmp>
2005b436:	e519                	c.bnez	a0,2005b444 <cmd_reboot+0x3a>
2005b438:	20000593          	addi	a1,zero,512
2005b43c:	dffa5097          	auipc	ra,0xdffa5
2005b440:	46a080e7          	jalr	ra,1130(ra) # 8a6 <BKUP_Set>
2005b444:	f7efe0ef          	jal	ra,20059bc2 <System_Reset>
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
2005b490:	1d850513          	addi	a0,a0,472 # 200631d8 <__FUNCTION__.0+0x2a0>
2005b494:	76f030ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
2005b524:	1e850513          	addi	a0,a0,488 # 200631e8 <__FUNCTION__.0+0x2b0>
2005b528:	6db030ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
2005b588:	20450513          	addi	a0,a0,516 # 20063204 <__FUNCTION__.0+0x2cc>
2005b58c:	677030ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
2005b678:	22850513          	addi	a0,a0,552 # 20063228 <__FUNCTION__.0+0x2f0>
2005b67c:	b575                	c.j	2005b528 <cmd_dump_word+0x2e>

2005b67e <rtos_critical_is_in_interrupt>:
2005b67e:	1141                	c.addi	sp,-16
2005b680:	c606                	c.swsp	ra,12(sp)
2005b682:	6e30a0ef          	jal	ra,20066564 <plic_get_active_irq_id>
2005b686:	40b2                	c.lwsp	ra,12(sp)
2005b688:	00a03533          	sltu	a0,zero,a0
2005b68c:	0141                	c.addi	sp,16
2005b68e:	8082                	c.jr	ra

2005b690 <rtos_critical_enter>:
2005b690:	1141                	c.addi	sp,-16
2005b692:	c606                	c.swsp	ra,12(sp)
2005b694:	37ed                	c.jal	2005b67e <rtos_critical_is_in_interrupt>
2005b696:	c919                	c.beqz	a0,2005b6ac <rtos_critical_enter+0x1c>
2005b698:	eec18793          	addi	a5,gp,-276 # 2006689c <uxCriticalNestingCnt>
2005b69c:	0007d703          	lhu	a4,0(a5)
2005b6a0:	0705                	c.addi	a4,1
2005b6a2:	00e79023          	sh	a4,0(a5)
2005b6a6:	40b2                	c.lwsp	ra,12(sp)
2005b6a8:	0141                	c.addi	sp,16
2005b6aa:	8082                	c.jr	ra
2005b6ac:	40b2                	c.lwsp	ra,12(sp)
2005b6ae:	0141                	c.addi	sp,16
2005b6b0:	56c0206f          	jal	zero,2005dc1c <vTaskEnterCritical>

2005b6b4 <rtos_critical_exit>:
2005b6b4:	1141                	c.addi	sp,-16
2005b6b6:	c606                	c.swsp	ra,12(sp)
2005b6b8:	37d9                	c.jal	2005b67e <rtos_critical_is_in_interrupt>
2005b6ba:	c919                	c.beqz	a0,2005b6d0 <rtos_critical_exit+0x1c>
2005b6bc:	eec18793          	addi	a5,gp,-276 # 2006689c <uxCriticalNestingCnt>
2005b6c0:	0007d703          	lhu	a4,0(a5)
2005b6c4:	177d                	c.addi	a4,-1
2005b6c6:	00e79023          	sh	a4,0(a5)
2005b6ca:	40b2                	c.lwsp	ra,12(sp)
2005b6cc:	0141                	c.addi	sp,16
2005b6ce:	8082                	c.jr	ra
2005b6d0:	40b2                	c.lwsp	ra,12(sp)
2005b6d2:	0141                	c.addi	sp,16
2005b6d4:	5620206f          	jal	zero,2005dc36 <vTaskExitCritical>

2005b6d8 <rtos_mem_init>:
2005b6d8:	becff06f          	jal	zero,2005aac4 <os_heap_init>

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
2005b79e:	4b460613          	addi	a2,a2,1204 # 200634b4 <__FUNCTION__.0>
2005b7a2:	46050513          	addi	a0,a0,1120 # 20063460 <__FUNCTION__.0+0x528>
2005b7a6:	45d030ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
2005b9c4:	efc18413          	addi	s0,gp,-260 # 200668ac <mutex_pool_init_flag>
2005b9c8:	401c                	c.lw	a5,0(s0)
2005b9ca:	02a78563          	beq	a5,a0,2005b9f4 <__reserved_get_from_poll+0x5a>
2005b9ce:	4601                	c.li	a2,0
2005b9d0:	4581                	c.li	a1,0
2005b9d2:	5b018513          	addi	a0,gp,1456 # 20066f60 <xISRStack>
2005b9d6:	dffae097          	auipc	ra,0xdffae
2005b9da:	0f8080e7          	jalr	ra,248(ra) # 9ace <__wrap_memset>
2005b9de:	ee01a823          	sw	zero,-272(gp) # 200668a0 <mutex_buf_used_num>
2005b9e2:	f2018513          	addi	a0,gp,-224 # 200668d0 <wrapper_mutex_buf_list>
2005b9e6:	ee01ac23          	sw	zero,-264(gp) # 200668a8 <mutex_max_buf_used_num>
2005b9ea:	c108                	c.sw	a0,0(a0)
2005b9ec:	c148                	c.sw	a0,4(a0)
2005b9ee:	ee01aa23          	sw	zero,-268(gp) # 200668a4 <mutex_dynamic_num>
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
2005ba4a:	f0c18b93          	addi	s7,gp,-244 # 200668bc <sema_pool_init_flag>
2005ba4e:	000ba783          	lw	a5,0(s7)
2005ba52:	fa8781e3          	beq	a5,s0,2005b9f4 <__reserved_get_from_poll+0x5a>
2005ba56:	4601                	c.li	a2,0
2005ba58:	4581                	c.li	a1,0
2005ba5a:	5b018513          	addi	a0,gp,1456 # 20066f60 <xISRStack>
2005ba5e:	dffae097          	auipc	ra,0xdffae
2005ba62:	070080e7          	jalr	ra,112(ra) # 9ace <__wrap_memset>
2005ba66:	f001a023          	sw	zero,-256(gp) # 200668b0 <sema_buf_used_num>
2005ba6a:	f2818513          	addi	a0,gp,-216 # 200668d8 <wrapper_sema_buf_list>
2005ba6e:	f001a423          	sw	zero,-248(gp) # 200668b8 <sema_max_buf_used_num>
2005ba72:	c108                	c.sw	a0,0(a0)
2005ba74:	c148                	c.sw	a0,4(a0)
2005ba76:	f001a223          	sw	zero,-252(gp) # 200668b4 <sema_dynamic_num>
2005ba7a:	008ba023          	sw	s0,0(s7)
2005ba7e:	bf9d                	c.j	2005b9f4 <__reserved_get_from_poll+0x5a>
2005ba80:	f1c18b93          	addi	s7,gp,-228 # 200668cc <timer_pool_init_flag>
2005ba84:	000ba783          	lw	a5,0(s7)
2005ba88:	f68786e3          	beq	a5,s0,2005b9f4 <__reserved_get_from_poll+0x5a>
2005ba8c:	4601                	c.li	a2,0
2005ba8e:	4581                	c.li	a1,0
2005ba90:	5b018513          	addi	a0,gp,1456 # 20066f60 <xISRStack>
2005ba94:	dffae097          	auipc	ra,0xdffae
2005ba98:	03a080e7          	jalr	ra,58(ra) # 9ace <__wrap_memset>
2005ba9c:	f001a823          	sw	zero,-240(gp) # 200668c0 <timer_buf_used_num>
2005baa0:	f3018513          	addi	a0,gp,-208 # 200668e0 <wrapper_timer_buf_list>
2005baa4:	f001ac23          	sw	zero,-232(gp) # 200668c8 <timer_max_buf_used_num>
2005baa8:	c108                	c.sw	a0,0(a0)
2005baaa:	c148                	c.sw	a0,4(a0)
2005baac:	f001aa23          	sw	zero,-236(gp) # 200668c4 <timer_dynamic_num>
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
2005baf2:	ef818793          	addi	a5,gp,-264 # 200668a8 <mutex_max_buf_used_num>
2005baf6:	ef018713          	addi	a4,gp,-272 # 200668a0 <mutex_buf_used_num>
2005bafa:	ef418693          	addi	a3,gp,-268 # 200668a4 <mutex_dynamic_num>
2005bafe:	efc18613          	addi	a2,gp,-260 # 200668ac <mutex_pool_init_flag>
2005bb02:	f2018593          	addi	a1,gp,-224 # 200668d0 <wrapper_mutex_buf_list>
2005bb06:	4505                	c.li	a0,1
2005bb08:	e93ff06f          	jal	zero,2005b99a <__reserved_get_from_poll>

2005bb0c <__reserved_release_mutex_to_poll>:
2005bb0c:	1141                	c.addi	sp,-16
2005bb0e:	c606                	c.swsp	ra,12(sp)
2005bb10:	b81ff0ef          	jal	ra,2005b690 <rtos_critical_enter>
2005bb14:	ef418793          	addi	a5,gp,-268 # 200668a4 <mutex_dynamic_num>
2005bb18:	4398                	c.lw	a4,0(a5)
2005bb1a:	177d                	c.addi	a4,-1
2005bb1c:	c398                	c.sw	a4,0(a5)
2005bb1e:	40b2                	c.lwsp	ra,12(sp)
2005bb20:	0141                	c.addi	sp,16
2005bb22:	b93ff06f          	jal	zero,2005b6b4 <rtos_critical_exit>

2005bb26 <__reserved_get_sema_from_poll>:
2005bb26:	f0818793          	addi	a5,gp,-248 # 200668b8 <sema_max_buf_used_num>
2005bb2a:	f0018713          	addi	a4,gp,-256 # 200668b0 <sema_buf_used_num>
2005bb2e:	f0418693          	addi	a3,gp,-252 # 200668b4 <sema_dynamic_num>
2005bb32:	f0c18613          	addi	a2,gp,-244 # 200668bc <sema_pool_init_flag>
2005bb36:	f2818593          	addi	a1,gp,-216 # 200668d8 <wrapper_sema_buf_list>
2005bb3a:	4509                	c.li	a0,2
2005bb3c:	e5fff06f          	jal	zero,2005b99a <__reserved_get_from_poll>

2005bb40 <__reserved_get_timer_from_poll>:
2005bb40:	f1818793          	addi	a5,gp,-232 # 200668c8 <timer_max_buf_used_num>
2005bb44:	f1018713          	addi	a4,gp,-240 # 200668c0 <timer_buf_used_num>
2005bb48:	f1418693          	addi	a3,gp,-236 # 200668c4 <timer_dynamic_num>
2005bb4c:	f1c18613          	addi	a2,gp,-228 # 200668cc <timer_pool_init_flag>
2005bb50:	f3018593          	addi	a1,gp,-208 # 200668e0 <wrapper_timer_buf_list>
2005bb54:	450d                	c.li	a0,3
2005bb56:	e45ff06f          	jal	zero,2005b99a <__reserved_get_from_poll>

2005bb5a <__reserved_release_timer_to_poll>:
2005bb5a:	1141                	c.addi	sp,-16
2005bb5c:	c606                	c.swsp	ra,12(sp)
2005bb5e:	b33ff0ef          	jal	ra,2005b690 <rtos_critical_enter>
2005bb62:	f1418793          	addi	a5,gp,-236 # 200668c4 <timer_dynamic_num>
2005bb66:	4398                	c.lw	a4,0(a5)
2005bb68:	177d                	c.addi	a4,-1
2005bb6a:	c398                	c.sw	a4,0(a5)
2005bb6c:	40b2                	c.lwsp	ra,12(sp)
2005bb6e:	0141                	c.addi	sp,16
2005bb70:	b45ff06f          	jal	zero,2005b6b4 <rtos_critical_exit>

2005bb74 <rtos_sched_start>:
2005bb74:	1141                	c.addi	sp,-16
2005bb76:	c606                	c.swsp	ra,12(sp)
2005bb78:	21c020ef          	jal	ra,2005dd94 <vTaskStartScheduler>
2005bb7c:	40b2                	c.lwsp	ra,12(sp)
2005bb7e:	4501                	c.li	a0,0
2005bb80:	0141                	c.addi	sp,16
2005bb82:	8082                	c.jr	ra

2005bb84 <rtos_sched_get_state>:
2005bb84:	1141                	c.addi	sp,-16
2005bb86:	c606                	c.swsp	ra,12(sp)
2005bb88:	6b3010ef          	jal	ra,2005da3a <xTaskGetSchedulerState>
2005bb8c:	4785                	c.li	a5,1
2005bb8e:	00f50b63          	beq	a0,a5,2005bba4 <rtos_sched_get_state+0x20>
2005bb92:	6a9010ef          	jal	ra,2005da3a <xTaskGetSchedulerState>
2005bb96:	c519                	c.beqz	a0,2005bba4 <rtos_sched_get_state+0x20>
2005bb98:	6a3010ef          	jal	ra,2005da3a <xTaskGetSchedulerState>
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
2005bbba:	242020ef          	jal	ra,2005ddfc <xTaskCreate>
2005bbbe:	40b2                	c.lwsp	ra,12(sp)
2005bbc0:	157d                	c.addi	a0,-1
2005bbc2:	00a03533          	sltu	a0,zero,a0
2005bbc6:	40a00533          	sub	a0,zero,a0
2005bbca:	0141                	c.addi	sp,16
2005bbcc:	8082                	c.jr	ra

2005bbce <rtos_task_delete>:
2005bbce:	1141                	c.addi	sp,-16
2005bbd0:	c606                	c.swsp	ra,12(sp)
2005bbd2:	2a2020ef          	jal	ra,2005de74 <vTaskDelete>
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
2005bbe8:	bc9fd0ef          	jal	ra,200597b0 <pmu_yield_os_check>
2005bbec:	cd19                	c.beqz	a0,2005bc0a <rtos_time_delay_ms+0x2a>
2005bbee:	64d010ef          	jal	ra,2005da3a <xTaskGetSchedulerState>
2005bbf2:	4789                	c.li	a5,2
2005bbf4:	00f51b63          	bne	a0,a5,2005bc0a <rtos_time_delay_ms+0x2a>
2005bbf8:	a87ff0ef          	jal	ra,2005b67e <rtos_critical_is_in_interrupt>
2005bbfc:	e519                	c.bnez	a0,2005bc0a <rtos_time_delay_ms+0x2a>
2005bbfe:	8522                	c.mv	a0,s0
2005bc00:	4422                	c.lwsp	s0,8(sp)
2005bc02:	40b2                	c.lwsp	ra,12(sp)
2005bc04:	0141                	c.addi	sp,16
2005bc06:	4880206f          	jal	zero,2005e08e <vTaskDelay>
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
2005bc2c:	b85fd0ef          	jal	ra,200597b0 <pmu_yield_os_check>
2005bc30:	d975                	c.beqz	a0,2005bc24 <rtos_time_get_current_system_time_ms+0xa>
2005bc32:	40b2                	c.lwsp	ra,12(sp)
2005bc34:	0141                	c.addi	sp,16
2005bc36:	6fe0206f          	jal	zero,2005e334 <xTaskGetTickCount>

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
2005bc56:	125020ef          	jal	ra,2005e57a <xTimerCreate>
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
2005bcb0:	133020ef          	jal	ra,2005e5e2 <xTimerCreateStatic>
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
2005bcd8:	16d020ef          	jal	ra,2005e644 <xTimerGenericCommand>
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
2005bd4c:	0f9020ef          	jal	ra,2005e644 <xTimerGenericCommand>
2005bd50:	4785                	c.li	a5,1
2005bd52:	fcf516e3          	bne	a0,a5,2005bd1e <rtos_timer_start+0x6>
2005bd56:	4532                	c.lwsp	a0,12(sp)
2005bd58:	d561                	c.beqz	a0,2005bd20 <rtos_timer_start+0x8>
2005bd5a:	2dd010ef          	jal	ra,2005d836 <vTaskSwitchContext>
2005bd5e:	4501                	c.li	a0,0
2005bd60:	b7c1                	c.j	2005bd20 <rtos_timer_start+0x8>
2005bd62:	5d2020ef          	jal	ra,2005e334 <xTaskGetTickCount>
2005bd66:	862a                	c.mv	a2,a0
2005bd68:	8726                	c.mv	a4,s1
2005bd6a:	4681                	c.li	a3,0
2005bd6c:	4585                	c.li	a1,1
2005bd6e:	8522                	c.mv	a0,s0
2005bd70:	0d5020ef          	jal	ra,2005e644 <xTimerGenericCommand>
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
2005bdae:	097020ef          	jal	ra,2005e644 <xTimerGenericCommand>
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
2005bdcc:	079020ef          	jal	ra,2005e644 <xTimerGenericCommand>
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
2005be10:	035020ef          	jal	ra,2005e644 <xTimerGenericCommand>
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
2005be2e:	017020ef          	jal	ra,2005e644 <xTimerGenericCommand>
2005be32:	157d                	c.addi	a0,-1
2005be34:	00a03533          	sltu	a0,zero,a0
2005be38:	40a00533          	sub	a0,zero,a0
2005be3c:	b765                	c.j	2005bde4 <rtos_timer_change_period+0x8>

2005be3e <rtos_timer_is_timer_active>:
2005be3e:	2e90206f          	jal	zero,2005e926 <xTimerIsTimerActive>

2005be42 <platform_calloc_uninit>:
2005be42:	4501                	c.li	a0,0
2005be44:	8082                	c.jr	ra

2005be46 <platform_free_uninit>:
2005be46:	8082                	c.jr	ra

2005be48 <mbedtls_platform_set_calloc_free>:
2005be48:	e6a1ac23          	sw	a0,-392(gp) # 20066828 <mbedtls_calloc_func>
2005be4c:	e6b1ae23          	sw	a1,-388(gp) # 2006682c <mbedtls_free_func>
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
2005bf24:	f581a283          	lw	t0,-168(gp) # 20066908 <pxCurrentTCB>
2005bf28:	0022a023          	sw	sp,0(t0)
2005bf2c:	e541a103          	lw	sp,-428(gp) # 20066804 <xISRStackTop>
2005bf30:	a0a5                	c.j	2005bf98 <task_context_switch>

2005bf32 <_crash_dump>:
2005bf32:	34102573          	csrrs	a0,mepc,zero
2005bf36:	022c                	c.addi4spn	a1,sp,264
2005bf38:	860a                	c.mv	a2,sp
2005bf3a:	c002                	c.swsp	zero,0(sp)
2005bf3c:	c42e                	c.swsp	a1,8(sp)
2005bf3e:	c60e                	c.swsp	gp,12(sp)
2005bf40:	c812                	c.swsp	tp,16(sp)
2005bf42:	e541a303          	lw	t1,-428(gp) # 20066804 <xISRStackTop>
2005bf46:	0000b397          	auipc	t2,0xb
2005bf4a:	01a38393          	addi	t2,t2,26 # 20066f60 <xISRStack>
2005bf4e:	08038393          	addi	t2,t2,128
2005bf52:	00615563          	bge	sp,t1,2005bf5c <not_isrstack>
2005bf56:	00714363          	blt	sp,t2,2005bf5c <not_isrstack>
2005bf5a:	a019                	c.j	2005bf60 <is_isrstack>

2005bf5c <not_isrstack>:
2005bf5c:	e541a103          	lw	sp,-428(gp) # 20066804 <xISRStackTop>

2005bf60 <is_isrstack>:
2005bf60:	00004297          	auipc	t0,0x4
2005bf64:	12228293          	addi	t0,t0,290 # 20060082 <crash_dump>
2005bf68:	9282                	c.jalr	t0

2005bf6a <crash_dump_end>:
2005bf6a:	a001                	c.j	2005bf6a <crash_dump_end>

2005bf6c <unrecoverable_error>:
2005bf6c:	9002                	c.ebreak
2005bf6e:	bffd                	c.j	2005bf6c <unrecoverable_error>

2005bf70 <handle_asynchronous>:
2005bf70:	c02e                	c.swsp	a1,0(sp)
2005bf72:	f581a283          	lw	t0,-168(gp) # 20066908 <pxCurrentTCB>
2005bf76:	0022a023          	sw	sp,0(t0)
2005bf7a:	e541a103          	lw	sp,-428(gp) # 20066804 <xISRStackTop>

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
2005bfa2:	f581a303          	lw	t1,-168(gp) # 20066908 <pxCurrentTCB>
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
2005c060:	f381a283          	lw	t0,-200(gp) # 200668e8 <pullMachineTimerCompareRegister>
2005c064:	e801a303          	lw	t1,-384(gp) # 20066830 <pullNextTime>
2005c068:	5efd                	c.li	t4,-1
2005c06a:	00032383          	lw	t2,0(t1) # 4000 <GPIO_Direction+0x36>
2005c06e:	00432e03          	lw	t3,4(t1)
2005c072:	01d2a023          	sw	t4,0(t0)
2005c076:	01c2a223          	sw	t3,4(t0)
2005c07a:	0072a023          	sw	t2,0(t0)
2005c07e:	f481a283          	lw	t0,-184(gp) # 200668f8 <uxTimerIncrementsForOneTick>
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
2005c0be:	3bc78793          	addi	a5,a5,956 # 20066476 <plic_interrupt_handler>
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
2005c0dc:	f581a383          	lw	t2,-168(gp) # 20066908 <pxCurrentTCB>
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
2005c542:	e601a783          	lw	a5,-416(gp) # 20066810 <SystemCoreClock>
2005c546:	3e800713          	addi	a4,zero,1000
2005c54a:	02e7d7b3          	divu	a5,a5,a4
2005c54e:	1141                	c.addi	sp,-16
2005c550:	f4f1a423          	sw	a5,-184(gp) # 200668f8 <uxTimerIncrementsForOneTick>
2005c554:	f14027f3          	csrrs	a5,mhartid,zero
2005c558:	c63e                	c.swsp	a5,12(sp)
2005c55a:	4632                	c.lwsp	a2,12(sp)
2005c55c:	10000737          	lui	a4,0x10000
2005c560:	00170793          	addi	a5,a4,1 # 10000001 <__km4_boot_text_start__+0x7fffe1>
2005c564:	963e                	c.add	a2,a5
2005c566:	060e                	c.slli	a2,0x3
2005c568:	f2c1ac23          	sw	a2,-200(gp) # 200668e8 <pullMachineTimerCompareRegister>
2005c56c:	47b2                	c.lwsp	a5,12(sp)
2005c56e:	f4818593          	addi	a1,gp,-184 # 200668f8 <uxTimerIncrementsForOneTick>
2005c572:	97ba                	c.add	a5,a4
2005c574:	078e                	c.slli	a5,0x3
2005c576:	f2f1ae23          	sw	a5,-196(gp) # 200668ec <pullMachineTimerCounterRegister>
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
2005c5a0:	f4018613          	addi	a2,gp,-192 # 200668f0 <ullNextTime>
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
2005c5e4:	456010ef          	jal	ra,2005da3a <xTaskGetSchedulerState>
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
2005c608:	4d058593          	addi	a1,a1,1232 # 200634d0 <__FUNCTION__.0+0x1c>
2005c60c:	4d450513          	addi	a0,a0,1236 # 200634d4 <__FUNCTION__.0+0x20>
2005c610:	5f3020ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005c614:	30047073          	csrrci	zero,mstatus,8
2005c618:	a001                	c.j	2005c618 <vApplicationMallocFailedHook+0x38>
2005c61a:	20063637          	lui	a2,0x20063
2005c61e:	4c860613          	addi	a2,a2,1224 # 200634c8 <__FUNCTION__.0+0x14>
2005c622:	bfd1                	c.j	2005c5f6 <vApplicationMallocFailedHook+0x16>

2005c624 <vApplicationStackOverflowHook>:
2005c624:	862e                	c.mv	a2,a1
2005c626:	20063537          	lui	a0,0x20063
2005c62a:	200635b7          	lui	a1,0x20063
2005c62e:	1141                	c.addi	sp,-16
2005c630:	53858593          	addi	a1,a1,1336 # 20063538 <__FUNCTION__.5>
2005c634:	51050513          	addi	a0,a0,1296 # 20063510 <__FUNCTION__.0+0x5c>
2005c638:	c606                	c.swsp	ra,12(sp)
2005c63a:	5c9020ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005c63e:	a001                	c.j	2005c63e <vApplicationStackOverflowHook+0x1a>

2005c640 <vApplicationGetIdleTaskMemory>:
2005c640:	200687b7          	lui	a5,0x20068
2005c644:	f6078793          	addi	a5,a5,-160 # 20067f60 <xIdleTaskTCB.4>
2005c648:	c11c                	c.sw	a5,0(a0)
2005c64a:	200657b7          	lui	a5,0x20065
2005c64e:	de078793          	addi	a5,a5,-544 # 20064de0 <uxIdleTaskStack.3>
2005c652:	c19c                	c.sw	a5,0(a1)
2005c654:	20000793          	addi	a5,zero,512
2005c658:	c21c                	c.sw	a5,0(a2)
2005c65a:	8082                	c.jr	ra

2005c65c <vApplicationGetTimerTaskMemory>:
2005c65c:	200687b7          	lui	a5,0x20068
2005c660:	0c878793          	addi	a5,a5,200 # 200680c8 <xTimerTaskTCB.2>
2005c664:	c11c                	c.sw	a5,0(a0)
2005c666:	200657b7          	lui	a5,0x20065
2005c66a:	5e078793          	addi	a5,a5,1504 # 200655e0 <uxTimerTaskStack.1>
2005c66e:	c19c                	c.sw	a5,0(a1)
2005c670:	20000793          	addi	a5,zero,512
2005c674:	c21c                	c.sw	a5,0(a2)
2005c676:	8082                	c.jr	ra

2005c678 <vPortSuppressTicksAndSleep>:
2005c678:	1141                	c.addi	sp,-16
2005c67a:	c422                	c.swsp	s0,8(sp)
2005c67c:	4501                	c.li	a0,0
2005c67e:	c606                	c.swsp	ra,12(sp)
2005c680:	e601a423          	sw	zero,-408(gp) # 20066818 <system_can_yield>
2005c684:	c26fc0ef          	jal	ra,20058aaa <Systick_Cmd>
2005c688:	300477f3          	csrrci	a5,mstatus,8
2005c68c:	378010ef          	jal	ra,2005da04 <eTaskConfirmSleepModeStatus>
2005c690:	c90d                	c.beqz	a0,2005c6c2 <vPortSuppressTicksAndSleep+0x4a>
2005c692:	924fd0ef          	jal	ra,200597b6 <pmu_ready_to_sleep>
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
2005c6d0:	e6f1a423          	sw	a5,-408(gp) # 20066818 <system_can_yield>
2005c6d4:	4422                	c.lwsp	s0,8(sp)
2005c6d6:	0141                	c.addi	sp,16
2005c6d8:	8082                	c.jr	ra

2005c6da <prvIsQueueEmpty>:
2005c6da:	1141                	c.addi	sp,-16
2005c6dc:	c422                	c.swsp	s0,8(sp)
2005c6de:	842a                	c.mv	s0,a0
2005c6e0:	c606                	c.swsp	ra,12(sp)
2005c6e2:	53a010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005c6e6:	5c00                	c.lw	s0,56(s0)
2005c6e8:	54e010ef          	jal	ra,2005dc36 <vTaskExitCritical>
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
2005c716:	3ec010ef          	jal	ra,2005db02 <xTaskPriorityDisinherit>
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
2005c7ba:	462010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005c7be:	04544483          	lbu	s1,69(s0)
2005c7c2:	02440913          	addi	s2,s0,36
2005c7c6:	04e2                	c.slli	s1,0x18
2005c7c8:	84e1                	c.srai	s1,0x18
2005c7ca:	02904b63          	blt	zero,s1,2005c800 <prvUnlockQueue+0x52>
2005c7ce:	57fd                	c.li	a5,-1
2005c7d0:	04f402a3          	sb	a5,69(s0)
2005c7d4:	462010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005c7d8:	444010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
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
2005c7fc:	43a0106f          	jal	zero,2005dc36 <vTaskExitCritical>
2005c800:	505c                	c.lw	a5,36(s0)
2005c802:	d7f1                	c.beqz	a5,2005c7ce <prvUnlockQueue+0x20>
2005c804:	854a                	c.mv	a0,s2
2005c806:	166010ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
2005c80a:	c119                	c.beqz	a0,2005c810 <prvUnlockQueue+0x62>
2005c80c:	1f0010ef          	jal	ra,2005d9fc <vTaskMissedYield>
2005c810:	14fd                	c.addi	s1,-1
2005c812:	04e2                	c.slli	s1,0x18
2005c814:	84e1                	c.srai	s1,0x18
2005c816:	bf55                	c.j	2005c7ca <prvUnlockQueue+0x1c>
2005c818:	481c                	c.lw	a5,16(s0)
2005c81a:	dbe9                	c.beqz	a5,2005c7ec <prvUnlockQueue+0x3e>
2005c81c:	854a                	c.mv	a0,s2
2005c81e:	14e010ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
2005c822:	c119                	c.beqz	a0,2005c828 <prvUnlockQueue+0x7a>
2005c824:	1d8010ef          	jal	ra,2005d9fc <vTaskMissedYield>
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
2005c83c:	3e0010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
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
2005c870:	0fc010ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
2005c874:	c119                	c.beqz	a0,2005c87a <xQueueGenericReset+0x4a>
2005c876:	00000073          	ecall
2005c87a:	3bc010ef          	jal	ra,2005dc36 <vTaskExitCritical>
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
2005c8a4:	378010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005c8a8:	401c                	c.lw	a5,0(s0)
2005c8aa:	4501                	c.li	a0,0
2005c8ac:	e391                	c.bnez	a5,2005c8b0 <xQueueGetMutexHolder+0x14>
2005c8ae:	4408                	c.lw	a0,8(s0)
2005c8b0:	c62a                	c.swsp	a0,12(sp)
2005c8b2:	384010ef          	jal	ra,2005dc36 <vTaskExitCritical>
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
2005c8e4:	338010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
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
2005c906:	066010ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
2005c90a:	c119                	c.beqz	a0,2005c910 <xQueueGenericSend+0x50>
2005c90c:	00000073          	ecall
2005c910:	326010ef          	jal	ra,2005dc36 <vTaskExitCritical>
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
2005c92e:	308010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005c932:	4501                	c.li	a0,0
2005c934:	b7cd                	c.j	2005c916 <xQueueGenericSend+0x56>
2005c936:	000a1563          	bne	s4,zero,2005c940 <xQueueGenericSend+0x80>
2005c93a:	0828                	c.addi4spn	a0,sp,24
2005c93c:	0b2010ef          	jal	ra,2005d9ee <vTaskInternalSetTimeOutState>
2005c940:	2f6010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005c944:	599000ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005c948:	2d4010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
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
2005c96c:	2ca010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005c970:	006c                	c.addi4spn	a1,sp,12
2005c972:	0828                	c.addi4spn	a0,sp,24
2005c974:	1db010ef          	jal	ra,2005e34e <xTaskCheckForTimeOut>
2005c978:	e121                	c.bnez	a0,2005c9b8 <xQueueGenericSend+0xf8>
2005c97a:	2a2010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005c97e:	5c18                	c.lw	a4,56(s0)
2005c980:	5c5c                	c.lw	a5,60(s0)
2005c982:	02f71063          	bne	a4,a5,2005c9a2 <xQueueGenericSend+0xe2>
2005c986:	2b0010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005c98a:	45b2                	c.lwsp	a1,12(sp)
2005c98c:	855a                	c.mv	a0,s6
2005c98e:	795000ef          	jal	ra,2005d922 <vTaskPlaceOnEventList>
2005c992:	8522                	c.mv	a0,s0
2005c994:	e1bff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005c998:	60c010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005c99c:	c919                	c.beqz	a0,2005c9b2 <xQueueGenericSend+0xf2>
2005c99e:	4a05                	c.li	s4,1
2005c9a0:	b791                	c.j	2005c8e4 <xQueueGenericSend+0x24>
2005c9a2:	294010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005c9a6:	8522                	c.mv	a0,s0
2005c9a8:	e07ff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005c9ac:	5f8010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005c9b0:	b7fd                	c.j	2005c99e <xQueueGenericSend+0xde>
2005c9b2:	00000073          	ecall
2005c9b6:	b7e5                	c.j	2005c99e <xQueueGenericSend+0xde>
2005c9b8:	8522                	c.mv	a0,s0
2005c9ba:	df5ff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005c9be:	5e6010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
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
2005ca10:	75d000ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
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
2005ca5c:	711000ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
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
2005caa0:	17c010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
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
2005cabc:	6b1000ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
2005cac0:	c119                	c.beqz	a0,2005cac6 <xQueueReceive+0x44>
2005cac2:	00000073          	ecall
2005cac6:	170010ef          	jal	ra,2005dc36 <vTaskExitCritical>
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
2005cae2:	154010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005cae6:	4501                	c.li	a0,0
2005cae8:	b7d5                	c.j	2005cacc <xQueueReceive+0x4a>
2005caea:	000a1563          	bne	s4,zero,2005caf4 <xQueueReceive+0x72>
2005caee:	0828                	c.addi4spn	a0,sp,24
2005caf0:	6ff000ef          	jal	ra,2005d9ee <vTaskInternalSetTimeOutState>
2005caf4:	142010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005caf8:	3e5000ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005cafc:	120010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
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
2005cb20:	116010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005cb24:	006c                	c.addi4spn	a1,sp,12
2005cb26:	0828                	c.addi4spn	a0,sp,24
2005cb28:	027010ef          	jal	ra,2005e34e <xTaskCheckForTimeOut>
2005cb2c:	e90d                	c.bnez	a0,2005cb5e <xQueueReceive+0xdc>
2005cb2e:	8522                	c.mv	a0,s0
2005cb30:	babff0ef          	jal	ra,2005c6da <prvIsQueueEmpty>
2005cb34:	cd19                	c.beqz	a0,2005cb52 <xQueueReceive+0xd0>
2005cb36:	45b2                	c.lwsp	a1,12(sp)
2005cb38:	8556                	c.mv	a0,s5
2005cb3a:	5e9000ef          	jal	ra,2005d922 <vTaskPlaceOnEventList>
2005cb3e:	8522                	c.mv	a0,s0
2005cb40:	c6fff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cb44:	460010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005cb48:	e119                	c.bnez	a0,2005cb4e <xQueueReceive+0xcc>
2005cb4a:	00000073          	ecall
2005cb4e:	4a05                	c.li	s4,1
2005cb50:	bf81                	c.j	2005caa0 <xQueueReceive+0x1e>
2005cb52:	8522                	c.mv	a0,s0
2005cb54:	c5bff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cb58:	44c010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005cb5c:	bfcd                	c.j	2005cb4e <xQueueReceive+0xcc>
2005cb5e:	8522                	c.mv	a0,s0
2005cb60:	c4fff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cb64:	440010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
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
2005cb8e:	08e010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005cb92:	5c1c                	c.lw	a5,56(s0)
2005cb94:	cf8d                	c.beqz	a5,2005cbce <xQueueSemaphoreTake+0x5c>
2005cb96:	17fd                	c.addi	a5,-1
2005cb98:	dc1c                	c.sw	a5,56(s0)
2005cb9a:	401c                	c.lw	a5,0(s0)
2005cb9c:	e781                	c.bnez	a5,2005cba4 <xQueueSemaphoreTake+0x32>
2005cb9e:	0d3010ef          	jal	ra,2005e470 <pvTaskIncrementMutexHeldCount>
2005cba2:	c408                	c.sw	a0,8(s0)
2005cba4:	481c                	c.lw	a5,16(s0)
2005cba6:	cb81                	c.beqz	a5,2005cbb6 <xQueueSemaphoreTake+0x44>
2005cba8:	01040513          	addi	a0,s0,16
2005cbac:	5c1000ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
2005cbb0:	c119                	c.beqz	a0,2005cbb6 <xQueueSemaphoreTake+0x44>
2005cbb2:	00000073          	ecall
2005cbb6:	080010ef          	jal	ra,2005dc36 <vTaskExitCritical>
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
2005cbd2:	064010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005cbd6:	4481                	c.li	s1,0
2005cbd8:	b7d5                	c.j	2005cbbc <xQueueSemaphoreTake+0x4a>
2005cbda:	00099563          	bne	s3,zero,2005cbe4 <xQueueSemaphoreTake+0x72>
2005cbde:	0828                	c.addi4spn	a0,sp,24
2005cbe0:	60f000ef          	jal	ra,2005d9ee <vTaskInternalSetTimeOutState>
2005cbe4:	052010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005cbe8:	2f5000ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005cbec:	030010ef          	jal	ra,2005dc1c <vTaskEnterCritical>
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
2005cc10:	026010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005cc14:	006c                	c.addi4spn	a1,sp,12
2005cc16:	0828                	c.addi4spn	a0,sp,24
2005cc18:	736010ef          	jal	ra,2005e34e <xTaskCheckForTimeOut>
2005cc1c:	e139                	c.bnez	a0,2005cc62 <xQueueSemaphoreTake+0xf0>
2005cc1e:	8522                	c.mv	a0,s0
2005cc20:	abbff0ef          	jal	ra,2005c6da <prvIsQueueEmpty>
2005cc24:	c90d                	c.beqz	a0,2005cc56 <xQueueSemaphoreTake+0xe4>
2005cc26:	401c                	c.lw	a5,0(s0)
2005cc28:	eb89                	c.bnez	a5,2005cc3a <xQueueSemaphoreTake+0xc8>
2005cc2a:	7f3000ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005cc2e:	4408                	c.lw	a0,8(s0)
2005cc30:	61f000ef          	jal	ra,2005da4e <xTaskPriorityInherit>
2005cc34:	84aa                	c.mv	s1,a0
2005cc36:	000010ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005cc3a:	45b2                	c.lwsp	a1,12(sp)
2005cc3c:	8552                	c.mv	a0,s4
2005cc3e:	4e5000ef          	jal	ra,2005d922 <vTaskPlaceOnEventList>
2005cc42:	8522                	c.mv	a0,s0
2005cc44:	b6bff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cc48:	35c010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005cc4c:	e119                	c.bnez	a0,2005cc52 <xQueueSemaphoreTake+0xe0>
2005cc4e:	00000073          	ecall
2005cc52:	4985                	c.li	s3,1
2005cc54:	bf2d                	c.j	2005cb8e <xQueueSemaphoreTake+0x1c>
2005cc56:	8522                	c.mv	a0,s0
2005cc58:	b57ff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cc5c:	348010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005cc60:	bfcd                	c.j	2005cc52 <xQueueSemaphoreTake+0xe0>
2005cc62:	8522                	c.mv	a0,s0
2005cc64:	b4bff0ef          	jal	ra,2005c7ae <prvUnlockQueue>
2005cc68:	33c010ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005cc6c:	8522                	c.mv	a0,s0
2005cc6e:	a6dff0ef          	jal	ra,2005c6da <prvIsQueueEmpty>
2005cc72:	d165                	c.beqz	a0,2005cc52 <xQueueSemaphoreTake+0xe0>
2005cc74:	f40484e3          	beq	s1,zero,2005cbbc <xQueueSemaphoreTake+0x4a>
2005cc78:	7a5000ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005cc7c:	504c                	c.lw	a1,36(s0)
2005cc7e:	c589                	c.beqz	a1,2005cc88 <xQueueSemaphoreTake+0x116>
2005cc80:	581c                	c.lw	a5,48(s0)
2005cc82:	45ad                	c.li	a1,11
2005cc84:	439c                	c.lw	a5,0(a5)
2005cc86:	8d9d                	c.sub	a1,a5
2005cc88:	4408                	c.lw	a0,8(s0)
2005cc8a:	6f5000ef          	jal	ra,2005db7e <vTaskPriorityDisinheritAfterTimeout>
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
2005ccd8:	495000ef          	jal	ra,2005d96c <xTaskRemoveFromEventList>
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
2005cd0a:	713000ef          	jal	ra,2005dc1c <vTaskEnterCritical>
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
2005cd32:	705000ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005cd36:	5c1c                	c.lw	a5,56(s0)
2005cd38:	e799                	c.bnez	a5,2005cd46 <vQueueWaitForMessageRestricted+0x4c>
2005cd3a:	864a                	c.mv	a2,s2
2005cd3c:	85a6                	c.mv	a1,s1
2005cd3e:	02440513          	addi	a0,s0,36
2005cd42:	401000ef          	jal	ra,2005d942 <vTaskPlaceOnEventListRestricted>
2005cd46:	8522                	c.mv	a0,s0
2005cd48:	4422                	c.lwsp	s0,8(sp)
2005cd4a:	40b2                	c.lwsp	ra,12(sp)
2005cd4c:	4492                	c.lwsp	s1,4(sp)
2005cd4e:	4902                	c.lwsp	s2,0(sp)
2005cd50:	0141                	c.addi	sp,16
2005cd52:	a5dff06f          	jal	zero,2005c7ae <prvUnlockQueue>

2005cd56 <QueueCreateTrace>:
2005cd56:	f4c1a603          	lw	a2,-180(gp) # 200668fc <Inited.1>
2005cd5a:	f4c18693          	addi	a3,gp,-180 # 200668fc <Inited.1>
2005cd5e:	200687b7          	lui	a5,0x20068
2005cd62:	23078713          	addi	a4,a5,560 # 20068230 <QueueTraceList>
2005cd66:	ee01                	c.bnez	a2,2005cd7e <QueueCreateTrace+0x28>
2005cd68:	23078793          	addi	a5,a5,560
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
2005ceda:	23070693          	addi	a3,a4,560 # 20068230 <QueueTraceList>
2005cede:	4781                	c.li	a5,0
2005cee0:	23070713          	addi	a4,a4,560
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
2005cf34:	9b0fc0ef          	jal	ra,200590e4 <ipc_get_message>
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
2005cfd0:	f501a503          	lw	a0,-176(gp) # 20066900 <shell_sema>
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
2005cff0:	55858593          	addi	a1,a1,1368 # 20063558 <__FUNCTION__.5+0x20>
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
2005d014:	38f010ef          	jal	ra,2005eba2 <ChipInfo_GetLibVersion_ToBuf>
2005d018:	20063537          	lui	a0,0x20063
2005d01c:	85a6                	c.mv	a1,s1
2005d01e:	93c50513          	addi	a0,a0,-1732 # 2006293c <pmap_func+0x570>
2005d022:	3e0020ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
2005d098:	f501a503          	lw	a0,-176(gp) # 20066900 <shell_sema>
2005d09c:	55fd                	c.li	a1,-1
2005d09e:	86dfe0ef          	jal	ra,2005b90a <rtos_sema_take>
2005d0a2:	f39ff0ef          	jal	ra,2005cfda <shell_loguartRx_dispatch>
2005d0a6:	00444783          	lbu	a5,4(s0)
2005d0aa:	0ff7f793          	andi	a5,a5,255
2005d0ae:	d7ed                	c.beqz	a5,2005d098 <shell_task_ram+0x4c>
2005d0b0:	4601                	c.li	a2,0
2005d0b2:	07f00593          	addi	a1,zero,127
2005d0b6:	270a8513          	addi	a0,s5,624 # 20068270 <atcmd_buf>
2005d0ba:	dffac097          	auipc	ra,0xdffac
2005d0be:	dc8080e7          	jalr	ra,-568(ra) # 8e82 <shell_array_init>
2005d0c2:	85ca                	c.mv	a1,s2
2005d0c4:	270a8513          	addi	a0,s5,624
2005d0c8:	3f5040ef          	jal	ra,20061cbc <strcpy>
2005d0cc:	270a8513          	addi	a0,s5,624
2005d0d0:	dfbfc0ef          	jal	ra,20059eca <atcmd_service>
2005d0d4:	ed39                	c.bnez	a0,2005d132 <shell_task_ram+0xe6>
2005d0d6:	854a                	c.mv	a0,s2
2005d0d8:	d97fc0ef          	jal	ra,20059e6e <mp_command_handler>
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
2005d122:	560c0513          	addi	a0,s8,1376 # 20063560 <__FUNCTION__.5+0x28>
2005d126:	2dc020ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005d12a:	834c8513          	addi	a0,s9,-1996 # 20062834 <pmap_func+0x468>
2005d12e:	2d4020ef          	jal	ra,2005f402 <DiagPrintf_minimal>
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
2005d160:	babfc0ef          	jal	ra,20059d0a <atcmd_service_init>
2005d164:	20065737          	lui	a4,0x20065
2005d168:	c8470693          	addi	a3,a4,-892 # 20064c84 <shell_cmd_table>
2005d16c:	20065737          	lui	a4,0x20065
2005d170:	200017b7          	lui	a5,0x20001
2005d174:	cc470713          	addi	a4,a4,-828 # 20064cc4 <ipc_tickless_table>
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
2005d196:	f5018513          	addi	a0,gp,-176 # 20066900 <shell_sema>
2005d19a:	cfd8                	c.sw	a4,28(a5)
2005d19c:	f2efe0ef          	jal	ra,2005b8ca <rtos_sema_create_binary>
2005d1a0:	2005d637          	lui	a2,0x2005d
2005d1a4:	200635b7          	lui	a1,0x20063
2005d1a8:	4795                	c.li	a5,5
2005d1aa:	6705                	c.lui	a4,0x1
2005d1ac:	4681                	c.li	a3,0
2005d1ae:	04c60613          	addi	a2,a2,76 # 2005d04c <shell_task_ram>
2005d1b2:	57858593          	addi	a1,a1,1400 # 20063578 <__FUNCTION__.5+0x40>
2005d1b6:	4501                	c.li	a0,0
2005d1b8:	9f3fe0ef          	jal	ra,2005bbaa <rtos_task_create>
2005d1bc:	c105                	c.beqz	a0,2005d1dc <shell_init_ram+0x80>
2005d1be:	40b2                	c.lwsp	ra,12(sp)
2005d1c0:	200636b7          	lui	a3,0x20063
2005d1c4:	200635b7          	lui	a1,0x20063
2005d1c8:	58868693          	addi	a3,a3,1416 # 20063588 <__FUNCTION__.5+0x50>
2005d1cc:	04500613          	addi	a2,zero,69
2005d1d0:	5a458593          	addi	a1,a1,1444 # 200635a4 <__FUNCTION__.5+0x6c>
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
2005d20a:	9a2fc0ef          	jal	ra,200593ac <ota_get_cur_index>
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
2005d270:	68898713          	addi	a4,s3,1672 # 20063688 <__func__.1>
2005d274:	88d2                	c.mv	a7,s4
2005d276:	5ac68693          	addi	a3,a3,1452 # 200635ac <__FUNCTION__.5+0x74>
2005d27a:	04100613          	addi	a2,zero,65
2005d27e:	5fc90593          	addi	a1,s2,1532 # 200635fc <__FUNCTION__.5+0xc4>
2005d282:	4505                	c.li	a0,1
2005d284:	c222                	c.swsp	s0,4(sp)
2005d286:	c026                	c.swsp	s1,0(sp)
2005d288:	ce42                	c.swsp	a6,28(sp)
2005d28a:	eedfd0ef          	jal	ra,2005b176 <rtk_log_write>
2005d28e:	1810                	c.addi4spn	a2,sp,48
2005d290:	85a2                	c.mv	a1,s0
2005d292:	00a8                	c.addi4spn	a0,sp,72
2005d294:	81dfd0ef          	jal	ra,2005aab0 <flash_read_word>
2005d298:	1850                	c.addi4spn	a2,sp,52
2005d29a:	00440593          	addi	a1,s0,4 # f8000004 <__ctrace_end__+0x2e000004>
2005d29e:	00a8                	c.addi4spn	a0,sp,72
2005d2a0:	811fd0ef          	jal	ra,2005aab0 <flash_read_word>
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
2005d2ec:	68898713          	addi	a4,s3,1672
2005d2f0:	60068693          	addi	a3,a3,1536 # 20063600 <__FUNCTION__.5+0xc8>
2005d2f4:	04500613          	addi	a2,zero,69
2005d2f8:	5fc90593          	addi	a1,s2,1532
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
2005d338:	66c98713          	addi	a4,s3,1644 # 2006366c <__func__.0>
2005d33c:	5fc48593          	addi	a1,s1,1532 # 200635fc <__FUNCTION__.5+0xc4>
2005d340:	65068693          	addi	a3,a3,1616 # 20063650 <__FUNCTION__.5+0x118>
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
2005d374:	838fc0ef          	jal	ra,200593ac <ota_get_cur_index>
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
2005d3cc:	66c98713          	addi	a4,s3,1644
2005d3d0:	0805                	c.addi	a6,1
2005d3d2:	4785                	c.li	a5,1
2005d3d4:	5ac68693          	addi	a3,a3,1452 # 200635ac <__FUNCTION__.5+0x74>
2005d3d8:	5fc48593          	addi	a1,s1,1532
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
2005d440:	f82fc06f          	jal	zero,20059bc2 <System_Reset>

2005d444 <prvAddCurrentTaskToDelayedList>:
2005d444:	1101                	c.addi	sp,-32
2005d446:	ca26                	c.swsp	s1,20(sp)
2005d448:	cc22                	c.swsp	s0,24(sp)
2005d44a:	c84a                	c.swsp	s2,16(sp)
2005d44c:	c64e                	c.swsp	s3,12(sp)
2005d44e:	ce06                	c.swsp	ra,28(sp)
2005d450:	f901a983          	lw	s3,-112(gp) # 20066940 <xTickCount>
2005d454:	f5818793          	addi	a5,gp,-168 # 20066908 <pxCurrentTCB>
2005d458:	842a                	c.mv	s0,a0
2005d45a:	4388                	c.lw	a0,0(a5)
2005d45c:	892e                	c.mv	s2,a1
2005d45e:	0511                	c.addi	a0,4
2005d460:	a43fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005d464:	f5818793          	addi	a5,gp,-168 # 20066908 <pxCurrentTCB>
2005d468:	ed09                	c.bnez	a0,2005d482 <prvAddCurrentTaskToDelayedList+0x3e>
2005d46a:	4398                	c.lw	a4,0(a5)
2005d46c:	f7c18693          	addi	a3,gp,-132 # 2006692c <uxTopReadyPriority>
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
2005d49e:	40850513          	addi	a0,a0,1032 # 20068408 <xSuspendedTaskList>
2005d4a2:	6105                	c.addi16sp	sp,32
2005d4a4:	9bbfd06f          	jal	zero,2005ae5e <vListInsertEnd>
2005d4a8:	4398                	c.lw	a4,0(a5)
2005d4aa:	944e                	c.add	s0,s3
2005d4ac:	c340                	c.sw	s0,4(a4)
2005d4ae:	01347e63          	bgeu	s0,s3,2005d4ca <prvAddCurrentTaskToDelayedList+0x86>
2005d4b2:	f601a503          	lw	a0,-160(gp) # 20066910 <pxOverflowDelayedTaskList>
2005d4b6:	4462                	c.lwsp	s0,24(sp)
2005d4b8:	438c                	c.lw	a1,0(a5)
2005d4ba:	40f2                	c.lwsp	ra,28(sp)
2005d4bc:	44d2                	c.lwsp	s1,20(sp)
2005d4be:	4942                	c.lwsp	s2,16(sp)
2005d4c0:	49b2                	c.lwsp	s3,12(sp)
2005d4c2:	0591                	c.addi	a1,4
2005d4c4:	6105                	c.addi16sp	sp,32
2005d4c6:	9affd06f          	jal	zero,2005ae74 <vListInsert>
2005d4ca:	f5c1a503          	lw	a0,-164(gp) # 2006690c <pxDelayedTaskList>
2005d4ce:	438c                	c.lw	a1,0(a5)
2005d4d0:	0591                	c.addi	a1,4
2005d4d2:	9a3fd0ef          	jal	ra,2005ae74 <vListInsert>
2005d4d6:	f8018793          	addi	a5,gp,-128 # 20066930 <xNextTaskUnblockTime>
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
2005d4f8:	7c4040ef          	jal	ra,20061cbc <strcpy>
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
2005d53a:	f7c1a703          	lw	a4,-132(gp) # 2006692c <uxTopReadyPriority>
2005d53e:	f581a783          	lw	a5,-168(gp) # 20066908 <pxCurrentTCB>
2005d542:	57c8                	c.lw	a0,44(a5)
2005d544:	e105                	c.bnez	a0,2005d564 <prvGetExpectedIdleTime+0x2a>
2005d546:	200687b7          	lui	a5,0x20068
2005d54a:	2f07a683          	lw	a3,752(a5) # 200682f0 <pxReadyTasksLists>
2005d54e:	4785                	c.li	a5,1
2005d550:	00d7eb63          	bltu	a5,a3,2005d566 <prvGetExpectedIdleTime+0x2c>
2005d554:	00e7e963          	bltu	a5,a4,2005d566 <prvGetExpectedIdleTime+0x2c>
2005d558:	f801a503          	lw	a0,-128(gp) # 20066930 <xNextTaskUnblockTime>
2005d55c:	f901a783          	lw	a5,-112(gp) # 20066940 <xTickCount>
2005d560:	8d1d                	c.sub	a0,a5
2005d562:	8082                	c.jr	ra
2005d564:	4501                	c.li	a0,0
2005d566:	8082                	c.jr	ra

2005d568 <prvResetNextTaskUnblockTime>:
2005d568:	f5c18713          	addi	a4,gp,-164 # 2006690c <pxDelayedTaskList>
2005d56c:	431c                	c.lw	a5,0(a4)
2005d56e:	4394                	c.lw	a3,0(a5)
2005d570:	f8018793          	addi	a5,gp,-128 # 20066930 <xNextTaskUnblockTime>
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
2005d590:	642040ef          	jal	ra,20061bd2 <_reclaim_reent>
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
2005d694:	87c78793          	addi	a5,a5,-1924 # 2006887c <__sf>
2005d698:	d83c                	c.sw	a5,112(s0)
2005d69a:	200697b7          	lui	a5,0x20069
2005d69e:	8e478793          	addi	a5,a5,-1820 # 200688e4 <__sf+0x68>
2005d6a2:	d87c                	c.sw	a5,116(s0)
2005d6a4:	200697b7          	lui	a5,0x20069
2005d6a8:	94c78793          	addi	a5,a5,-1716 # 2006894c <__sf+0xd0>
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
2005d6dc:	f7418793          	addi	a5,gp,-140 # 20066924 <uxSchedulerSuspended>
2005d6e0:	4398                	c.lw	a4,0(a5)
2005d6e2:	0705                	c.addi	a4,1
2005d6e4:	c398                	c.sw	a4,0(a5)
2005d6e6:	8082                	c.jr	ra

2005d6e8 <xTaskGetTickCountFromISR>:
2005d6e8:	f901a503          	lw	a0,-112(gp) # 20066940 <xTickCount>
2005d6ec:	8082                	c.jr	ra

2005d6ee <uxTaskGetNumberOfTasks>:
2005d6ee:	f6c1a503          	lw	a0,-148(gp) # 2006691c <uxCurrentNumberOfTasks>
2005d6f2:	8082                	c.jr	ra

2005d6f4 <pcTaskGetName>:
2005d6f4:	e119                	c.bnez	a0,2005d6fa <pcTaskGetName+0x6>
2005d6f6:	f581a503          	lw	a0,-168(gp) # 20066908 <pxCurrentTCB>
2005d6fa:	03450513          	addi	a0,a0,52
2005d6fe:	8082                	c.jr	ra

2005d700 <xTaskIncrementTick>:
2005d700:	f741a783          	lw	a5,-140(gp) # 20066924 <uxSchedulerSuspended>
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
2005d722:	f9018793          	addi	a5,gp,-112 # 20066940 <xTickCount>
2005d726:	0007aa03          	lw	s4,0(a5)
2005d72a:	0a05                	c.addi	s4,1
2005d72c:	0147a023          	sw	s4,0(a5)
2005d730:	020a1163          	bne	s4,zero,2005d752 <xTaskIncrementTick+0x52>
2005d734:	f5c18713          	addi	a4,gp,-164 # 2006690c <pxDelayedTaskList>
2005d738:	f6018793          	addi	a5,gp,-160 # 20066910 <pxOverflowDelayedTaskList>
2005d73c:	4314                	c.lw	a3,0(a4)
2005d73e:	4390                	c.lw	a2,0(a5)
2005d740:	c310                	c.sw	a2,0(a4)
2005d742:	c394                	c.sw	a3,0(a5)
2005d744:	f8418793          	addi	a5,gp,-124 # 20066934 <xNumOfOverflows>
2005d748:	4398                	c.lw	a4,0(a5)
2005d74a:	0705                	c.addi	a4,1
2005d74c:	c398                	c.sw	a4,0(a5)
2005d74e:	e1bff0ef          	jal	ra,2005d568 <prvResetNextTaskUnblockTime>
2005d752:	f8018793          	addi	a5,gp,-128 # 20066930 <xNextTaskUnblockTime>
2005d756:	439c                	c.lw	a5,0(a5)
2005d758:	200684b7          	lui	s1,0x20068
2005d75c:	f8018993          	addi	s3,gp,-128 # 20066930 <xNextTaskUnblockTime>
2005d760:	2f048493          	addi	s1,s1,752 # 200682f0 <pxReadyTasksLists>
2005d764:	f5818a93          	addi	s5,gp,-168 # 20066908 <pxCurrentTCB>
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
2005d786:	f881a783          	lw	a5,-120(gp) # 20066938 <xPendedTicks>
2005d78a:	e399                	c.bnez	a5,2005d790 <xTaskIncrementTick+0x90>
2005d78c:	e41fe0ef          	jal	ra,2005c5cc <vApplicationTickHook>
2005d790:	f941a783          	lw	a5,-108(gp) # 20066944 <xYieldPending>
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
2005d7ba:	f5c18793          	addi	a5,gp,-164 # 2006690c <pxDelayedTaskList>
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
2005d7f8:	f7c18713          	addi	a4,gp,-132 # 2006692c <uxTopReadyPriority>
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
2005d824:	f8818793          	addi	a5,gp,-120 # 20066938 <xPendedTicks>
2005d828:	4398                	c.lw	a4,0(a5)
2005d82a:	4401                	c.li	s0,0
2005d82c:	0705                	c.addi	a4,1
2005d82e:	c398                	c.sw	a4,0(a5)
2005d830:	d9dfe0ef          	jal	ra,2005c5cc <vApplicationTickHook>
2005d834:	b795                	c.j	2005d798 <xTaskIncrementTick+0x98>

2005d836 <vTaskSwitchContext>:
2005d836:	f741a703          	lw	a4,-140(gp) # 20066924 <uxSchedulerSuspended>
2005d83a:	f9418793          	addi	a5,gp,-108 # 20066944 <xYieldPending>
2005d83e:	c701                	c.beqz	a4,2005d846 <vTaskSwitchContext+0x10>
2005d840:	4705                	c.li	a4,1
2005d842:	c398                	c.sw	a4,0(a5)
2005d844:	8082                	c.jr	ra
2005d846:	1141                	c.addi	sp,-16
2005d848:	c422                	c.swsp	s0,8(sp)
2005d84a:	c606                	c.swsp	ra,12(sp)
2005d84c:	c226                	c.swsp	s1,4(sp)
2005d84e:	0007a023          	sw	zero,0(a5)
2005d852:	410187b7          	lui	a5,0x41018
2005d856:	4bd4                	c.lw	a3,20(a5)
2005d858:	f6818713          	addi	a4,gp,-152 # 20066918 <ulTotalRunTime>
2005d85c:	c314                	c.sw	a3,0(a4)
2005d85e:	f641a583          	lw	a1,-156(gp) # 20066914 <ulTaskSwitchedInTime>
2005d862:	4318                	c.lw	a4,0(a4)
2005d864:	f6818793          	addi	a5,gp,-152 # 20066918 <ulTotalRunTime>
2005d868:	f6418693          	addi	a3,gp,-156 # 20066914 <ulTaskSwitchedInTime>
2005d86c:	f5818413          	addi	s0,gp,-168 # 20066908 <pxCurrentTCB>
2005d870:	00e5f863          	bgeu	a1,a4,2005d880 <vTaskSwitchContext+0x4a>
2005d874:	4388                	c.lw	a0,0(a5)
2005d876:	4010                	c.lw	a2,0(s0)
2005d878:	5638                	c.lw	a4,104(a2)
2005d87a:	8f0d                	c.sub	a4,a1
2005d87c:	972a                	c.add	a4,a0
2005d87e:	d638                	c.sw	a4,104(a2)
2005d880:	439c                	c.lw	a5,0(a5)
2005d882:	a5a5a737          	lui	a4,0xa5a5a
2005d886:	5a570713          	addi	a4,a4,1445 # a5a5a5a5 <__ctrace_end__+0xdba5a5a5>
2005d88a:	c29c                	c.sw	a5,0(a3)
2005d88c:	401c                	c.lw	a5,0(s0)
2005d88e:	5b9c                	c.lw	a5,48(a5)
2005d890:	4390                	c.lw	a2,0(a5)
2005d892:	00e61b63          	bne	a2,a4,2005d8a8 <vTaskSwitchContext+0x72>
2005d896:	43d4                	c.lw	a3,4(a5)
2005d898:	00c69863          	bne	a3,a2,2005d8a8 <vTaskSwitchContext+0x72>
2005d89c:	4798                	c.lw	a4,8(a5)
2005d89e:	00d71563          	bne	a4,a3,2005d8a8 <vTaskSwitchContext+0x72>
2005d8a2:	47dc                	c.lw	a5,12(a5)
2005d8a4:	00e78863          	beq	a5,a4,2005d8b4 <vTaskSwitchContext+0x7e>
2005d8a8:	4008                	c.lw	a0,0(s0)
2005d8aa:	400c                	c.lw	a1,0(s0)
2005d8ac:	03458593          	addi	a1,a1,52
2005d8b0:	d75fe0ef          	jal	ra,2005c624 <vApplicationStackOverflowHook>
2005d8b4:	f541a703          	lw	a4,-172(gp) # 20066904 <FreeRTOS_errno>
2005d8b8:	401c                	c.lw	a5,0(s0)
2005d8ba:	f5418493          	addi	s1,gp,-172 # 20066904 <FreeRTOS_errno>
2005d8be:	16e7a223          	sw	a4,356(a5) # 41018164 <__km4_bd_boot_download_addr__+0x11006164>
2005d8c2:	f7c1a503          	lw	a0,-132(gp) # 2006692c <uxTopReadyPriority>
2005d8c6:	399030ef          	jal	ra,2006145e <__clzsi2>
2005d8ca:	47fd                	c.li	a5,31
2005d8cc:	40a78533          	sub	a0,a5,a0
2005d8d0:	4751                	c.li	a4,20
2005d8d2:	02e50733          	mul	a4,a0,a4
2005d8d6:	200687b7          	lui	a5,0x20068
2005d8da:	2f078693          	addi	a3,a5,752 # 200682f0 <pxReadyTasksLists>
2005d8de:	2f078793          	addi	a5,a5,752
2005d8e2:	00e68633          	add	a2,a3,a4
2005d8e6:	424c                	c.lw	a1,4(a2)
2005d8e8:	0721                	c.addi	a4,8
2005d8ea:	9736                	c.add	a4,a3
2005d8ec:	41cc                	c.lw	a1,4(a1)
2005d8ee:	c24c                	c.sw	a1,4(a2)
2005d8f0:	00e59463          	bne	a1,a4,2005d8f8 <vTaskSwitchContext+0xc2>
2005d8f4:	41d8                	c.lw	a4,4(a1)
2005d8f6:	c258                	c.sw	a4,4(a2)
2005d8f8:	4751                	c.li	a4,20
2005d8fa:	02e50533          	mul	a0,a0,a4
2005d8fe:	40b2                	c.lwsp	ra,12(sp)
2005d900:	97aa                	c.add	a5,a0
2005d902:	43dc                	c.lw	a5,4(a5)
2005d904:	47dc                	c.lw	a5,12(a5)
2005d906:	c01c                	c.sw	a5,0(s0)
2005d908:	401c                	c.lw	a5,0(s0)
2005d90a:	1647a783          	lw	a5,356(a5)
2005d90e:	c09c                	c.sw	a5,0(s1)
2005d910:	401c                	c.lw	a5,0(s0)
2005d912:	4422                	c.lwsp	s0,8(sp)
2005d914:	4492                	c.lwsp	s1,4(sp)
2005d916:	06c78793          	addi	a5,a5,108
2005d91a:	e8f1a623          	sw	a5,-372(gp) # 2006683c <_impure_ptr>
2005d91e:	0141                	c.addi	sp,16
2005d920:	8082                	c.jr	ra

2005d922 <vTaskPlaceOnEventList>:
2005d922:	1141                	c.addi	sp,-16
2005d924:	c422                	c.swsp	s0,8(sp)
2005d926:	842e                	c.mv	s0,a1
2005d928:	f581a583          	lw	a1,-168(gp) # 20066908 <pxCurrentTCB>
2005d92c:	c606                	c.swsp	ra,12(sp)
2005d92e:	05e1                	c.addi	a1,24
2005d930:	d44fd0ef          	jal	ra,2005ae74 <vListInsert>
2005d934:	8522                	c.mv	a0,s0
2005d936:	4422                	c.lwsp	s0,8(sp)
2005d938:	40b2                	c.lwsp	ra,12(sp)
2005d93a:	4585                	c.li	a1,1
2005d93c:	0141                	c.addi	sp,16
2005d93e:	b07ff06f          	jal	zero,2005d444 <prvAddCurrentTaskToDelayedList>

2005d942 <vTaskPlaceOnEventListRestricted>:
2005d942:	1141                	c.addi	sp,-16
2005d944:	c422                	c.swsp	s0,8(sp)
2005d946:	842e                	c.mv	s0,a1
2005d948:	f581a583          	lw	a1,-168(gp) # 20066908 <pxCurrentTCB>
2005d94c:	c226                	c.swsp	s1,4(sp)
2005d94e:	84b2                	c.mv	s1,a2
2005d950:	05e1                	c.addi	a1,24
2005d952:	c606                	c.swsp	ra,12(sp)
2005d954:	d0afd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005d958:	c091                	c.beqz	s1,2005d95c <vTaskPlaceOnEventListRestricted+0x1a>
2005d95a:	547d                	c.li	s0,-1
2005d95c:	8522                	c.mv	a0,s0
2005d95e:	4422                	c.lwsp	s0,8(sp)
2005d960:	40b2                	c.lwsp	ra,12(sp)
2005d962:	85a6                	c.mv	a1,s1
2005d964:	4492                	c.lwsp	s1,4(sp)
2005d966:	0141                	c.addi	sp,16
2005d968:	addff06f          	jal	zero,2005d444 <prvAddCurrentTaskToDelayedList>

2005d96c <xTaskRemoveFromEventList>:
2005d96c:	1101                	c.addi	sp,-32
2005d96e:	ce06                	c.swsp	ra,28(sp)
2005d970:	cc22                	c.swsp	s0,24(sp)
2005d972:	455c                	c.lw	a5,12(a0)
2005d974:	47c0                	c.lw	s0,12(a5)
2005d976:	01840593          	addi	a1,s0,24
2005d97a:	852e                	c.mv	a0,a1
2005d97c:	c62e                	c.swsp	a1,12(sp)
2005d97e:	d24fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005d982:	f741a783          	lw	a5,-140(gp) # 20066924 <uxSchedulerSuspended>
2005d986:	45b2                	c.lwsp	a1,12(sp)
2005d988:	efa1                	c.bnez	a5,2005d9e0 <xTaskRemoveFromEventList+0x74>
2005d98a:	00440593          	addi	a1,s0,4
2005d98e:	852e                	c.mv	a0,a1
2005d990:	c62e                	c.swsp	a1,12(sp)
2005d992:	d10fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005d996:	f7c18713          	addi	a4,gp,-132 # 2006692c <uxTopReadyPriority>
2005d99a:	5448                	c.lw	a0,44(s0)
2005d99c:	4314                	c.lw	a3,0(a4)
2005d99e:	4785                	c.li	a5,1
2005d9a0:	00a797b3          	sll	a5,a5,a0
2005d9a4:	8fd5                	c.or	a5,a3
2005d9a6:	c31c                	c.sw	a5,0(a4)
2005d9a8:	47d1                	c.li	a5,20
2005d9aa:	02f50533          	mul	a0,a0,a5
2005d9ae:	45b2                	c.lwsp	a1,12(sp)
2005d9b0:	200687b7          	lui	a5,0x20068
2005d9b4:	2f078793          	addi	a5,a5,752 # 200682f0 <pxReadyTasksLists>
2005d9b8:	953e                	c.add	a0,a5
2005d9ba:	ca4fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005d9be:	babff0ef          	jal	ra,2005d568 <prvResetNextTaskUnblockTime>
2005d9c2:	f581a783          	lw	a5,-168(gp) # 20066908 <pxCurrentTCB>
2005d9c6:	5458                	c.lw	a4,44(s0)
2005d9c8:	4501                	c.li	a0,0
2005d9ca:	57dc                	c.lw	a5,44(a5)
2005d9cc:	00e7f663          	bgeu	a5,a4,2005d9d8 <xTaskRemoveFromEventList+0x6c>
2005d9d0:	4705                	c.li	a4,1
2005d9d2:	f8e1aa23          	sw	a4,-108(gp) # 20066944 <xYieldPending>
2005d9d6:	4505                	c.li	a0,1
2005d9d8:	40f2                	c.lwsp	ra,28(sp)
2005d9da:	4462                	c.lwsp	s0,24(sp)
2005d9dc:	6105                	c.addi16sp	sp,32
2005d9de:	8082                	c.jr	ra
2005d9e0:	20068537          	lui	a0,0x20068
2005d9e4:	3f450513          	addi	a0,a0,1012 # 200683f4 <xPendingReadyList>
2005d9e8:	c76fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005d9ec:	bfd9                	c.j	2005d9c2 <xTaskRemoveFromEventList+0x56>

2005d9ee <vTaskInternalSetTimeOutState>:
2005d9ee:	f841a783          	lw	a5,-124(gp) # 20066934 <xNumOfOverflows>
2005d9f2:	c11c                	c.sw	a5,0(a0)
2005d9f4:	f901a783          	lw	a5,-112(gp) # 20066940 <xTickCount>
2005d9f8:	c15c                	c.sw	a5,4(a0)
2005d9fa:	8082                	c.jr	ra

2005d9fc <vTaskMissedYield>:
2005d9fc:	4705                	c.li	a4,1
2005d9fe:	f8e1aa23          	sw	a4,-108(gp) # 20066944 <xYieldPending>
2005da02:	8082                	c.jr	ra

2005da04 <eTaskConfirmSleepModeStatus>:
2005da04:	200687b7          	lui	a5,0x20068
2005da08:	3f47a783          	lw	a5,1012(a5) # 200683f4 <xPendingReadyList>
2005da0c:	4501                	c.li	a0,0
2005da0e:	e395                	c.bnez	a5,2005da32 <eTaskConfirmSleepModeStatus+0x2e>
2005da10:	f941a783          	lw	a5,-108(gp) # 20066944 <xYieldPending>
2005da14:	ef99                	c.bnez	a5,2005da32 <eTaskConfirmSleepModeStatus+0x2e>
2005da16:	f881a783          	lw	a5,-120(gp) # 20066938 <xPendedTicks>
2005da1a:	ef81                	c.bnez	a5,2005da32 <eTaskConfirmSleepModeStatus+0x2e>
2005da1c:	200687b7          	lui	a5,0x20068
2005da20:	4087a703          	lw	a4,1032(a5) # 20068408 <xSuspendedTaskList>
2005da24:	f6c1a783          	lw	a5,-148(gp) # 2006691c <uxCurrentNumberOfTasks>
2005da28:	4505                	c.li	a0,1
2005da2a:	17fd                	c.addi	a5,-1
2005da2c:	00f71363          	bne	a4,a5,2005da32 <eTaskConfirmSleepModeStatus+0x2e>
2005da30:	4509                	c.li	a0,2
2005da32:	8082                	c.jr	ra

2005da34 <xTaskGetCurrentTaskHandle>:
2005da34:	f581a503          	lw	a0,-168(gp) # 20066908 <pxCurrentTCB>
2005da38:	8082                	c.jr	ra

2005da3a <xTaskGetSchedulerState>:
2005da3a:	f8c1a783          	lw	a5,-116(gp) # 2006693c <xSchedulerRunning>
2005da3e:	4505                	c.li	a0,1
2005da40:	c791                	c.beqz	a5,2005da4c <xTaskGetSchedulerState+0x12>
2005da42:	f741a503          	lw	a0,-140(gp) # 20066924 <uxSchedulerSuspended>
2005da46:	00153513          	sltiu	a0,a0,1
2005da4a:	0506                	c.slli	a0,0x1
2005da4c:	8082                	c.jr	ra

2005da4e <xTaskPriorityInherit>:
2005da4e:	c945                	c.beqz	a0,2005dafe <xTaskPriorityInherit+0xb0>
2005da50:	1101                	c.addi	sp,-32
2005da52:	ca26                	c.swsp	s1,20(sp)
2005da54:	cc22                	c.swsp	s0,24(sp)
2005da56:	ce06                	c.swsp	ra,28(sp)
2005da58:	c84a                	c.swsp	s2,16(sp)
2005da5a:	f5818713          	addi	a4,gp,-168 # 20066908 <pxCurrentTCB>
2005da5e:	4314                	c.lw	a3,0(a4)
2005da60:	555c                	c.lw	a5,44(a0)
2005da62:	842a                	c.mv	s0,a0
2005da64:	56d4                	c.lw	a3,44(a3)
2005da66:	f5818493          	addi	s1,gp,-168 # 20066908 <pxCurrentTCB>
2005da6a:	08d7f463          	bgeu	a5,a3,2005daf2 <xTaskPriorityInherit+0xa4>
2005da6e:	4d18                	c.lw	a4,24(a0)
2005da70:	00074763          	blt	a4,zero,2005da7e <xTaskPriorityInherit+0x30>
2005da74:	4098                	c.lw	a4,0(s1)
2005da76:	5754                	c.lw	a3,44(a4)
2005da78:	472d                	c.li	a4,11
2005da7a:	8f15                	c.sub	a4,a3
2005da7c:	cd18                	c.sw	a4,24(a0)
2005da7e:	46d1                	c.li	a3,20
2005da80:	02d787b3          	mul	a5,a5,a3
2005da84:	20068537          	lui	a0,0x20068
2005da88:	2f050713          	addi	a4,a0,752 # 200682f0 <pxReadyTasksLists>
2005da8c:	2f050913          	addi	s2,a0,752
2005da90:	97ba                	c.add	a5,a4
2005da92:	4858                	c.lw	a4,20(s0)
2005da94:	04f71b63          	bne	a4,a5,2005daea <xTaskPriorityInherit+0x9c>
2005da98:	00440593          	addi	a1,s0,4
2005da9c:	852e                	c.mv	a0,a1
2005da9e:	c62e                	c.swsp	a1,12(sp)
2005daa0:	c02fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005daa4:	45b2                	c.lwsp	a1,12(sp)
2005daa6:	f7c18713          	addi	a4,gp,-132 # 2006692c <uxTopReadyPriority>
2005daaa:	e911                	c.bnez	a0,2005dabe <xTaskPriorityInherit+0x70>
2005daac:	5450                	c.lw	a2,44(s0)
2005daae:	4314                	c.lw	a3,0(a4)
2005dab0:	4785                	c.li	a5,1
2005dab2:	00c797b3          	sll	a5,a5,a2
2005dab6:	fff7c793          	xori	a5,a5,-1
2005daba:	8ff5                	c.and	a5,a3
2005dabc:	c31c                	c.sw	a5,0(a4)
2005dabe:	409c                	c.lw	a5,0(s1)
2005dac0:	4314                	c.lw	a3,0(a4)
2005dac2:	57c8                	c.lw	a0,44(a5)
2005dac4:	4785                	c.li	a5,1
2005dac6:	00a797b3          	sll	a5,a5,a0
2005daca:	8fd5                	c.or	a5,a3
2005dacc:	c31c                	c.sw	a5,0(a4)
2005dace:	47d1                	c.li	a5,20
2005dad0:	d448                	c.sw	a0,44(s0)
2005dad2:	02f50533          	mul	a0,a0,a5
2005dad6:	954a                	c.add	a0,s2
2005dad8:	b86fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005dadc:	4505                	c.li	a0,1
2005dade:	40f2                	c.lwsp	ra,28(sp)
2005dae0:	4462                	c.lwsp	s0,24(sp)
2005dae2:	44d2                	c.lwsp	s1,20(sp)
2005dae4:	4942                	c.lwsp	s2,16(sp)
2005dae6:	6105                	c.addi16sp	sp,32
2005dae8:	8082                	c.jr	ra
2005daea:	409c                	c.lw	a5,0(s1)
2005daec:	57dc                	c.lw	a5,44(a5)
2005daee:	d45c                	c.sw	a5,44(s0)
2005daf0:	b7f5                	c.j	2005dadc <xTaskPriorityInherit+0x8e>
2005daf2:	431c                	c.lw	a5,0(a4)
2005daf4:	4d68                	c.lw	a0,92(a0)
2005daf6:	57dc                	c.lw	a5,44(a5)
2005daf8:	00f53533          	sltu	a0,a0,a5
2005dafc:	b7cd                	c.j	2005dade <xTaskPriorityInherit+0x90>
2005dafe:	4501                	c.li	a0,0
2005db00:	8082                	c.jr	ra

2005db02 <xTaskPriorityDisinherit>:
2005db02:	e901                	c.bnez	a0,2005db12 <xTaskPriorityDisinherit+0x10>
2005db04:	4501                	c.li	a0,0
2005db06:	8082                	c.jr	ra
2005db08:	4501                	c.li	a0,0
2005db0a:	40f2                	c.lwsp	ra,28(sp)
2005db0c:	4462                	c.lwsp	s0,24(sp)
2005db0e:	6105                	c.addi16sp	sp,32
2005db10:	8082                	c.jr	ra
2005db12:	1101                	c.addi	sp,-32
2005db14:	cc22                	c.swsp	s0,24(sp)
2005db16:	ce06                	c.swsp	ra,28(sp)
2005db18:	513c                	c.lw	a5,96(a0)
2005db1a:	5554                	c.lw	a3,44(a0)
2005db1c:	4d78                	c.lw	a4,92(a0)
2005db1e:	17fd                	c.addi	a5,-1
2005db20:	d13c                	c.sw	a5,96(a0)
2005db22:	842a                	c.mv	s0,a0
2005db24:	fee682e3          	beq	a3,a4,2005db08 <xTaskPriorityDisinherit+0x6>
2005db28:	f3e5                	c.bnez	a5,2005db08 <xTaskPriorityDisinherit+0x6>
2005db2a:	00450593          	addi	a1,a0,4
2005db2e:	852e                	c.mv	a0,a1
2005db30:	c62e                	c.swsp	a1,12(sp)
2005db32:	b70fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005db36:	45b2                	c.lwsp	a1,12(sp)
2005db38:	f7c18713          	addi	a4,gp,-132 # 2006692c <uxTopReadyPriority>
2005db3c:	e911                	c.bnez	a0,2005db50 <xTaskPriorityDisinherit+0x4e>
2005db3e:	5450                	c.lw	a2,44(s0)
2005db40:	4314                	c.lw	a3,0(a4)
2005db42:	4785                	c.li	a5,1
2005db44:	00c797b3          	sll	a5,a5,a2
2005db48:	fff7c793          	xori	a5,a5,-1
2005db4c:	8ff5                	c.and	a5,a3
2005db4e:	c31c                	c.sw	a5,0(a4)
2005db50:	4c68                	c.lw	a0,92(s0)
2005db52:	47ad                	c.li	a5,11
2005db54:	4314                	c.lw	a3,0(a4)
2005db56:	8f89                	c.sub	a5,a0
2005db58:	cc1c                	c.sw	a5,24(s0)
2005db5a:	4785                	c.li	a5,1
2005db5c:	00a797b3          	sll	a5,a5,a0
2005db60:	8fd5                	c.or	a5,a3
2005db62:	c31c                	c.sw	a5,0(a4)
2005db64:	47d1                	c.li	a5,20
2005db66:	d448                	c.sw	a0,44(s0)
2005db68:	02f50533          	mul	a0,a0,a5
2005db6c:	200687b7          	lui	a5,0x20068
2005db70:	2f078793          	addi	a5,a5,752 # 200682f0 <pxReadyTasksLists>
2005db74:	953e                	c.add	a0,a5
2005db76:	ae8fd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005db7a:	4505                	c.li	a0,1
2005db7c:	b779                	c.j	2005db0a <xTaskPriorityDisinherit+0x8>

2005db7e <vTaskPriorityDisinheritAfterTimeout>:
2005db7e:	cd51                	c.beqz	a0,2005dc1a <vTaskPriorityDisinheritAfterTimeout+0x9c>
2005db80:	1101                	c.addi	sp,-32
2005db82:	cc22                	c.swsp	s0,24(sp)
2005db84:	ce06                	c.swsp	ra,28(sp)
2005db86:	ca26                	c.swsp	s1,20(sp)
2005db88:	4d7c                	c.lw	a5,92(a0)
2005db8a:	842a                	c.mv	s0,a0
2005db8c:	00b7f363          	bgeu	a5,a1,2005db92 <vTaskPriorityDisinheritAfterTimeout+0x14>
2005db90:	87ae                	c.mv	a5,a1
2005db92:	5458                	c.lw	a4,44(s0)
2005db94:	06f70e63          	beq	a4,a5,2005dc10 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005db98:	5030                	c.lw	a2,96(s0)
2005db9a:	4685                	c.li	a3,1
2005db9c:	06d61a63          	bne	a2,a3,2005dc10 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005dba0:	4c14                	c.lw	a3,24(s0)
2005dba2:	d45c                	c.sw	a5,44(s0)
2005dba4:	0006c663          	blt	a3,zero,2005dbb0 <vTaskPriorityDisinheritAfterTimeout+0x32>
2005dba8:	46ad                	c.li	a3,11
2005dbaa:	40f687b3          	sub	a5,a3,a5
2005dbae:	cc1c                	c.sw	a5,24(s0)
2005dbb0:	46d1                	c.li	a3,20
2005dbb2:	02d70733          	mul	a4,a4,a3
2005dbb6:	20068537          	lui	a0,0x20068
2005dbba:	2f050793          	addi	a5,a0,752 # 200682f0 <pxReadyTasksLists>
2005dbbe:	2f050493          	addi	s1,a0,752
2005dbc2:	973e                	c.add	a4,a5
2005dbc4:	485c                	c.lw	a5,20(s0)
2005dbc6:	04e79563          	bne	a5,a4,2005dc10 <vTaskPriorityDisinheritAfterTimeout+0x92>
2005dbca:	00440593          	addi	a1,s0,4
2005dbce:	852e                	c.mv	a0,a1
2005dbd0:	c62e                	c.swsp	a1,12(sp)
2005dbd2:	ad0fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005dbd6:	5454                	c.lw	a3,44(s0)
2005dbd8:	45b2                	c.lwsp	a1,12(sp)
2005dbda:	f7c18713          	addi	a4,gp,-132 # 2006692c <uxTopReadyPriority>
2005dbde:	e909                	c.bnez	a0,2005dbf0 <vTaskPriorityDisinheritAfterTimeout+0x72>
2005dbe0:	4310                	c.lw	a2,0(a4)
2005dbe2:	4785                	c.li	a5,1
2005dbe4:	00d797b3          	sll	a5,a5,a3
2005dbe8:	fff7c793          	xori	a5,a5,-1
2005dbec:	8ff1                	c.and	a5,a2
2005dbee:	c31c                	c.sw	a5,0(a4)
2005dbf0:	4551                	c.li	a0,20
2005dbf2:	02a68533          	mul	a0,a3,a0
2005dbf6:	4310                	c.lw	a2,0(a4)
2005dbf8:	4785                	c.li	a5,1
2005dbfa:	00d797b3          	sll	a5,a5,a3
2005dbfe:	8fd1                	c.or	a5,a2
2005dc00:	c31c                	c.sw	a5,0(a4)
2005dc02:	4462                	c.lwsp	s0,24(sp)
2005dc04:	40f2                	c.lwsp	ra,28(sp)
2005dc06:	9526                	c.add	a0,s1
2005dc08:	44d2                	c.lwsp	s1,20(sp)
2005dc0a:	6105                	c.addi16sp	sp,32
2005dc0c:	a52fd06f          	jal	zero,2005ae5e <vListInsertEnd>
2005dc10:	40f2                	c.lwsp	ra,28(sp)
2005dc12:	4462                	c.lwsp	s0,24(sp)
2005dc14:	44d2                	c.lwsp	s1,20(sp)
2005dc16:	6105                	c.addi16sp	sp,32
2005dc18:	8082                	c.jr	ra
2005dc1a:	8082                	c.jr	ra

2005dc1c <vTaskEnterCritical>:
2005dc1c:	30047073          	csrrci	zero,mstatus,8
2005dc20:	f8c1a783          	lw	a5,-116(gp) # 2006693c <xSchedulerRunning>
2005dc24:	cb81                	c.beqz	a5,2005dc34 <vTaskEnterCritical+0x18>
2005dc26:	f5818793          	addi	a5,gp,-168 # 20066908 <pxCurrentTCB>
2005dc2a:	4394                	c.lw	a3,0(a5)
2005dc2c:	439c                	c.lw	a5,0(a5)
2005dc2e:	4ab8                	c.lw	a4,80(a3)
2005dc30:	0705                	c.addi	a4,1
2005dc32:	cab8                	c.sw	a4,80(a3)
2005dc34:	8082                	c.jr	ra

2005dc36 <vTaskExitCritical>:
2005dc36:	f8c1a783          	lw	a5,-116(gp) # 2006693c <xSchedulerRunning>
2005dc3a:	cf99                	c.beqz	a5,2005dc58 <vTaskExitCritical+0x22>
2005dc3c:	f5818793          	addi	a5,gp,-168 # 20066908 <pxCurrentTCB>
2005dc40:	4398                	c.lw	a4,0(a5)
2005dc42:	4b38                	c.lw	a4,80(a4)
2005dc44:	cb11                	c.beqz	a4,2005dc58 <vTaskExitCritical+0x22>
2005dc46:	4394                	c.lw	a3,0(a5)
2005dc48:	439c                	c.lw	a5,0(a5)
2005dc4a:	4ab8                	c.lw	a4,80(a3)
2005dc4c:	177d                	c.addi	a4,-1
2005dc4e:	cab8                	c.sw	a4,80(a3)
2005dc50:	4bbc                	c.lw	a5,80(a5)
2005dc52:	e399                	c.bnez	a5,2005dc58 <vTaskExitCritical+0x22>
2005dc54:	30046073          	csrrsi	zero,mstatus,8
2005dc58:	8082                	c.jr	ra

2005dc5a <prvAddNewTaskToReadyList>:
2005dc5a:	1101                	c.addi	sp,-32
2005dc5c:	cc22                	c.swsp	s0,24(sp)
2005dc5e:	ca26                	c.swsp	s1,20(sp)
2005dc60:	c84a                	c.swsp	s2,16(sp)
2005dc62:	c64e                	c.swsp	s3,12(sp)
2005dc64:	ce06                	c.swsp	ra,28(sp)
2005dc66:	c452                	c.swsp	s4,8(sp)
2005dc68:	842a                	c.mv	s0,a0
2005dc6a:	fb3ff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005dc6e:	f6c18793          	addi	a5,gp,-148 # 2006691c <uxCurrentNumberOfTasks>
2005dc72:	4398                	c.lw	a4,0(a5)
2005dc74:	20068937          	lui	s2,0x20068
2005dc78:	0705                	c.addi	a4,1
2005dc7a:	c398                	c.sw	a4,0(a5)
2005dc7c:	f5818713          	addi	a4,gp,-168 # 20066908 <pxCurrentTCB>
2005dc80:	4318                	c.lw	a4,0(a4)
2005dc82:	f5818493          	addi	s1,gp,-168 # 20066908 <pxCurrentTCB>
2005dc86:	2f090993          	addi	s3,s2,752 # 200682f0 <pxReadyTasksLists>
2005dc8a:	e369                	c.bnez	a4,2005dd4c <prvAddNewTaskToReadyList+0xf2>
2005dc8c:	c080                	c.sw	s0,0(s1)
2005dc8e:	4398                	c.lw	a4,0(a5)
2005dc90:	4785                	c.li	a5,1
2005dc92:	06f71263          	bne	a4,a5,2005dcf6 <prvAddNewTaskToReadyList+0x9c>
2005dc96:	2f090913          	addi	s2,s2,752
2005dc9a:	0dc98a13          	addi	s4,s3,220
2005dc9e:	854a                	c.mv	a0,s2
2005dca0:	0951                	c.addi	s2,20
2005dca2:	9a2fd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dca6:	ff2a1ce3          	bne	s4,s2,2005dc9e <prvAddNewTaskToReadyList+0x44>
2005dcaa:	20068537          	lui	a0,0x20068
2005dcae:	3cc50a13          	addi	s4,a0,972 # 200683cc <xDelayedTaskList1>
2005dcb2:	3cc50513          	addi	a0,a0,972
2005dcb6:	98efd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dcba:	20068537          	lui	a0,0x20068
2005dcbe:	3e050913          	addi	s2,a0,992 # 200683e0 <xDelayedTaskList2>
2005dcc2:	3e050513          	addi	a0,a0,992
2005dcc6:	97efd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dcca:	20068537          	lui	a0,0x20068
2005dcce:	3f450513          	addi	a0,a0,1012 # 200683f4 <xPendingReadyList>
2005dcd2:	972fd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dcd6:	20068537          	lui	a0,0x20068
2005dcda:	41c50513          	addi	a0,a0,1052 # 2006841c <xTasksWaitingTermination>
2005dcde:	966fd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dce2:	20068537          	lui	a0,0x20068
2005dce6:	40850513          	addi	a0,a0,1032 # 20068408 <xSuspendedTaskList>
2005dcea:	95afd0ef          	jal	ra,2005ae44 <vListInitialise>
2005dcee:	f541ae23          	sw	s4,-164(gp) # 2006690c <pxDelayedTaskList>
2005dcf2:	f721a023          	sw	s2,-160(gp) # 20066910 <pxOverflowDelayedTaskList>
2005dcf6:	f7818713          	addi	a4,gp,-136 # 20066928 <uxTaskNumber>
2005dcfa:	431c                	c.lw	a5,0(a4)
2005dcfc:	5448                	c.lw	a0,44(s0)
2005dcfe:	00440593          	addi	a1,s0,4
2005dd02:	0785                	c.addi	a5,1
2005dd04:	c31c                	c.sw	a5,0(a4)
2005dd06:	f7c18713          	addi	a4,gp,-132 # 2006692c <uxTopReadyPriority>
2005dd0a:	4314                	c.lw	a3,0(a4)
2005dd0c:	c87c                	c.sw	a5,84(s0)
2005dd0e:	4785                	c.li	a5,1
2005dd10:	00a797b3          	sll	a5,a5,a0
2005dd14:	8fd5                	c.or	a5,a3
2005dd16:	c31c                	c.sw	a5,0(a4)
2005dd18:	47d1                	c.li	a5,20
2005dd1a:	02f50533          	mul	a0,a0,a5
2005dd1e:	954e                	c.add	a0,s3
2005dd20:	93efd0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005dd24:	f13ff0ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005dd28:	f8c1a783          	lw	a5,-116(gp) # 2006693c <xSchedulerRunning>
2005dd2c:	cb81                	c.beqz	a5,2005dd3c <prvAddNewTaskToReadyList+0xe2>
2005dd2e:	409c                	c.lw	a5,0(s1)
2005dd30:	57d8                	c.lw	a4,44(a5)
2005dd32:	545c                	c.lw	a5,44(s0)
2005dd34:	00f77463          	bgeu	a4,a5,2005dd3c <prvAddNewTaskToReadyList+0xe2>
2005dd38:	00000073          	ecall
2005dd3c:	40f2                	c.lwsp	ra,28(sp)
2005dd3e:	4462                	c.lwsp	s0,24(sp)
2005dd40:	44d2                	c.lwsp	s1,20(sp)
2005dd42:	4942                	c.lwsp	s2,16(sp)
2005dd44:	49b2                	c.lwsp	s3,12(sp)
2005dd46:	4a22                	c.lwsp	s4,8(sp)
2005dd48:	6105                	c.addi16sp	sp,32
2005dd4a:	8082                	c.jr	ra
2005dd4c:	f8c1a783          	lw	a5,-116(gp) # 2006693c <xSchedulerRunning>
2005dd50:	f3dd                	c.bnez	a5,2005dcf6 <prvAddNewTaskToReadyList+0x9c>
2005dd52:	409c                	c.lw	a5,0(s1)
2005dd54:	57d8                	c.lw	a4,44(a5)
2005dd56:	545c                	c.lw	a5,44(s0)
2005dd58:	f8e7efe3          	bltu	a5,a4,2005dcf6 <prvAddNewTaskToReadyList+0x9c>
2005dd5c:	c080                	c.sw	s0,0(s1)
2005dd5e:	bf61                	c.j	2005dcf6 <prvAddNewTaskToReadyList+0x9c>

2005dd60 <xTaskCreateStatic>:
2005dd60:	1101                	c.addi	sp,-32
2005dd62:	cc22                	c.swsp	s0,24(sp)
2005dd64:	ce06                	c.swsp	ra,28(sp)
2005dd66:	8442                	c.mv	s0,a6
2005dd68:	00080f63          	beq	a6,zero,2005dd86 <xTaskCreateStatic+0x26>
2005dd6c:	c395                	c.beqz	a5,2005dd90 <xTaskCreateStatic+0x30>
2005dd6e:	02f82823          	sw	a5,48(a6)
2005dd72:	4789                	c.li	a5,2
2005dd74:	16f800a3          	sb	a5,353(a6)
2005dd78:	007c                	c.addi4spn	a5,sp,12
2005dd7a:	841ff0ef          	jal	ra,2005d5ba <prvInitialiseNewTask.constprop.0>
2005dd7e:	8522                	c.mv	a0,s0
2005dd80:	edbff0ef          	jal	ra,2005dc5a <prvAddNewTaskToReadyList>
2005dd84:	4432                	c.lwsp	s0,12(sp)
2005dd86:	40f2                	c.lwsp	ra,28(sp)
2005dd88:	8522                	c.mv	a0,s0
2005dd8a:	4462                	c.lwsp	s0,24(sp)
2005dd8c:	6105                	c.addi16sp	sp,32
2005dd8e:	8082                	c.jr	ra
2005dd90:	4401                	c.li	s0,0
2005dd92:	bfd5                	c.j	2005dd86 <xTaskCreateStatic+0x26>

2005dd94 <vTaskStartScheduler>:
2005dd94:	1101                	c.addi	sp,-32
2005dd96:	0070                	c.addi4spn	a2,sp,12
2005dd98:	002c                	c.addi4spn	a1,sp,8
2005dd9a:	0048                	c.addi4spn	a0,sp,4
2005dd9c:	ce06                	c.swsp	ra,28(sp)
2005dd9e:	c202                	c.swsp	zero,4(sp)
2005dda0:	c402                	c.swsp	zero,8(sp)
2005dda2:	89ffe0ef          	jal	ra,2005c640 <vApplicationGetIdleTaskMemory>
2005dda6:	4812                	c.lwsp	a6,4(sp)
2005dda8:	47a2                	c.lwsp	a5,8(sp)
2005ddaa:	4632                	c.lwsp	a2,12(sp)
2005ddac:	200635b7          	lui	a1,0x20063
2005ddb0:	2005e537          	lui	a0,0x2005e
2005ddb4:	4701                	c.li	a4,0
2005ddb6:	4681                	c.li	a3,0
2005ddb8:	6a058593          	addi	a1,a1,1696 # 200636a0 <__func__.1+0x18>
2005ddbc:	0b850513          	addi	a0,a0,184 # 2005e0b8 <prvIdleTask>
2005ddc0:	fa1ff0ef          	jal	ra,2005dd60 <xTaskCreateStatic>
2005ddc4:	c51d                	c.beqz	a0,2005ddf2 <vTaskStartScheduler+0x5e>
2005ddc6:	76a000ef          	jal	ra,2005e530 <xTimerCreateTimerTask>
2005ddca:	4785                	c.li	a5,1
2005ddcc:	02f51363          	bne	a0,a5,2005ddf2 <vTaskStartScheduler+0x5e>
2005ddd0:	30047073          	csrrci	zero,mstatus,8
2005ddd4:	f581a783          	lw	a5,-168(gp) # 20066908 <pxCurrentTCB>
2005ddd8:	06c78793          	addi	a5,a5,108
2005dddc:	e8f1a623          	sw	a5,-372(gp) # 2006683c <_impure_ptr>
2005dde0:	577d                	c.li	a4,-1
2005dde2:	f8e1a023          	sw	a4,-128(gp) # 20066930 <xNextTaskUnblockTime>
2005dde6:	f8a1a623          	sw	a0,-116(gp) # 2006693c <xSchedulerRunning>
2005ddea:	f801a823          	sw	zero,-112(gp) # 20066940 <xTickCount>
2005ddee:	fc0fe0ef          	jal	ra,2005c5ae <xPortStartScheduler>
2005ddf2:	40f2                	c.lwsp	ra,28(sp)
2005ddf4:	e841a783          	lw	a5,-380(gp) # 20066834 <uxTopUsedPriority>
2005ddf8:	6105                	c.addi16sp	sp,32
2005ddfa:	8082                	c.jr	ra

2005ddfc <xTaskCreate>:
2005ddfc:	7179                	c.addi16sp	sp,-48
2005ddfe:	ce4e                	c.swsp	s3,28(sp)
2005de00:	89aa                	c.mv	s3,a0
2005de02:	00261513          	slli	a0,a2,0x2
2005de06:	d04a                	c.swsp	s2,32(sp)
2005de08:	cc52                	c.swsp	s4,24(sp)
2005de0a:	ca56                	c.swsp	s5,20(sp)
2005de0c:	c85a                	c.swsp	s6,16(sp)
2005de0e:	c65e                	c.swsp	s7,12(sp)
2005de10:	d606                	c.swsp	ra,44(sp)
2005de12:	d422                	c.swsp	s0,40(sp)
2005de14:	d226                	c.swsp	s1,36(sp)
2005de16:	8a2e                	c.mv	s4,a1
2005de18:	8932                	c.mv	s2,a2
2005de1a:	8ab6                	c.mv	s5,a3
2005de1c:	8b3a                	c.mv	s6,a4
2005de1e:	8bbe                	c.mv	s7,a5
2005de20:	debfc0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005de24:	c531                	c.beqz	a0,2005de70 <xTaskCreate+0x74>
2005de26:	84aa                	c.mv	s1,a0
2005de28:	16800513          	addi	a0,zero,360
2005de2c:	ddffc0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005de30:	842a                	c.mv	s0,a0
2005de32:	cd05                	c.beqz	a0,2005de6a <xTaskCreate+0x6e>
2005de34:	882a                	c.mv	a6,a0
2005de36:	d904                	c.sw	s1,48(a0)
2005de38:	160500a3          	sb	zero,353(a0)
2005de3c:	87de                	c.mv	a5,s7
2005de3e:	875a                	c.mv	a4,s6
2005de40:	86d6                	c.mv	a3,s5
2005de42:	864a                	c.mv	a2,s2
2005de44:	85d2                	c.mv	a1,s4
2005de46:	854e                	c.mv	a0,s3
2005de48:	f72ff0ef          	jal	ra,2005d5ba <prvInitialiseNewTask.constprop.0>
2005de4c:	8522                	c.mv	a0,s0
2005de4e:	e0dff0ef          	jal	ra,2005dc5a <prvAddNewTaskToReadyList>
2005de52:	4505                	c.li	a0,1
2005de54:	50b2                	c.lwsp	ra,44(sp)
2005de56:	5422                	c.lwsp	s0,40(sp)
2005de58:	5492                	c.lwsp	s1,36(sp)
2005de5a:	5902                	c.lwsp	s2,32(sp)
2005de5c:	49f2                	c.lwsp	s3,28(sp)
2005de5e:	4a62                	c.lwsp	s4,24(sp)
2005de60:	4ad2                	c.lwsp	s5,20(sp)
2005de62:	4b42                	c.lwsp	s6,16(sp)
2005de64:	4bb2                	c.lwsp	s7,12(sp)
2005de66:	6145                	c.addi16sp	sp,48
2005de68:	8082                	c.jr	ra
2005de6a:	8526                	c.mv	a0,s1
2005de6c:	e7dfc0ef          	jal	ra,2005ace8 <vPortFree>
2005de70:	557d                	c.li	a0,-1
2005de72:	b7cd                	c.j	2005de54 <xTaskCreate+0x58>

2005de74 <vTaskDelete>:
2005de74:	1141                	c.addi	sp,-16
2005de76:	c422                	c.swsp	s0,8(sp)
2005de78:	c226                	c.swsp	s1,4(sp)
2005de7a:	842a                	c.mv	s0,a0
2005de7c:	c606                	c.swsp	ra,12(sp)
2005de7e:	c04a                	c.swsp	s2,0(sp)
2005de80:	f5818493          	addi	s1,gp,-168 # 20066908 <pxCurrentTCB>
2005de84:	d99ff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005de88:	e011                	c.bnez	s0,2005de8c <vTaskDelete+0x18>
2005de8a:	4080                	c.lw	s0,0(s1)
2005de8c:	00440913          	addi	s2,s0,4
2005de90:	854a                	c.mv	a0,s2
2005de92:	810fd0ef          	jal	ra,2005aea2 <uxListRemove>
2005de96:	e515                	c.bnez	a0,2005dec2 <vTaskDelete+0x4e>
2005de98:	5450                	c.lw	a2,44(s0)
2005de9a:	4751                	c.li	a4,20
2005de9c:	02e606b3          	mul	a3,a2,a4
2005dea0:	20068737          	lui	a4,0x20068
2005dea4:	2f070713          	addi	a4,a4,752 # 200682f0 <pxReadyTasksLists>
2005dea8:	9736                	c.add	a4,a3
2005deaa:	431c                	c.lw	a5,0(a4)
2005deac:	eb99                	c.bnez	a5,2005dec2 <vTaskDelete+0x4e>
2005deae:	f7c18713          	addi	a4,gp,-132 # 2006692c <uxTopReadyPriority>
2005deb2:	4314                	c.lw	a3,0(a4)
2005deb4:	4785                	c.li	a5,1
2005deb6:	00c797b3          	sll	a5,a5,a2
2005deba:	fff7c793          	xori	a5,a5,-1
2005debe:	8ff5                	c.and	a5,a3
2005dec0:	c31c                	c.sw	a5,0(a4)
2005dec2:	541c                	c.lw	a5,40(s0)
2005dec4:	c789                	c.beqz	a5,2005dece <vTaskDelete+0x5a>
2005dec6:	01840513          	addi	a0,s0,24
2005deca:	fd9fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005dece:	f7818793          	addi	a5,gp,-136 # 20066928 <uxTaskNumber>
2005ded2:	4398                	c.lw	a4,0(a5)
2005ded4:	0705                	c.addi	a4,1
2005ded6:	c398                	c.sw	a4,0(a5)
2005ded8:	409c                	c.lw	a5,0(s1)
2005deda:	02879e63          	bne	a5,s0,2005df16 <vTaskDelete+0xa2>
2005dede:	20068537          	lui	a0,0x20068
2005dee2:	85ca                	c.mv	a1,s2
2005dee4:	41c50513          	addi	a0,a0,1052 # 2006841c <xTasksWaitingTermination>
2005dee8:	f77fc0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005deec:	f7018793          	addi	a5,gp,-144 # 20066920 <uxDeletedTasksWaitingCleanUp>
2005def0:	4398                	c.lw	a4,0(a5)
2005def2:	0705                	c.addi	a4,1
2005def4:	c398                	c.sw	a4,0(a5)
2005def6:	d41ff0ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005defa:	f8c1a783          	lw	a5,-116(gp) # 2006693c <xSchedulerRunning>
2005defe:	c791                	c.beqz	a5,2005df0a <vTaskDelete+0x96>
2005df00:	409c                	c.lw	a5,0(s1)
2005df02:	00879463          	bne	a5,s0,2005df0a <vTaskDelete+0x96>
2005df06:	00000073          	ecall
2005df0a:	40b2                	c.lwsp	ra,12(sp)
2005df0c:	4422                	c.lwsp	s0,8(sp)
2005df0e:	4492                	c.lwsp	s1,4(sp)
2005df10:	4902                	c.lwsp	s2,0(sp)
2005df12:	0141                	c.addi	sp,16
2005df14:	8082                	c.jr	ra
2005df16:	f6c18793          	addi	a5,gp,-148 # 2006691c <uxCurrentNumberOfTasks>
2005df1a:	4398                	c.lw	a4,0(a5)
2005df1c:	8522                	c.mv	a0,s0
2005df1e:	177d                	c.addi	a4,-1
2005df20:	c398                	c.sw	a4,0(a5)
2005df22:	e62ff0ef          	jal	ra,2005d584 <prvDeleteTCB>
2005df26:	e42ff0ef          	jal	ra,2005d568 <prvResetNextTaskUnblockTime>
2005df2a:	b7f1                	c.j	2005def6 <vTaskDelete+0x82>

2005df2c <eTaskGetState>:
2005df2c:	f581a783          	lw	a5,-168(gp) # 20066908 <pxCurrentTCB>
2005df30:	06a78863          	beq	a5,a0,2005dfa0 <eTaskGetState+0x74>
2005df34:	1101                	c.addi	sp,-32
2005df36:	ce06                	c.swsp	ra,28(sp)
2005df38:	cc22                	c.swsp	s0,24(sp)
2005df3a:	ca26                	c.swsp	s1,20(sp)
2005df3c:	842a                	c.mv	s0,a0
2005df3e:	c84a                	c.swsp	s2,16(sp)
2005df40:	c64e                	c.swsp	s3,12(sp)
2005df42:	cdbff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005df46:	f5c1a983          	lw	s3,-164(gp) # 2006690c <pxDelayedTaskList>
2005df4a:	4844                	c.lw	s1,20(s0)
2005df4c:	f601a903          	lw	s2,-160(gp) # 20066910 <pxOverflowDelayedTaskList>
2005df50:	ce7ff0ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005df54:	4509                	c.li	a0,2
2005df56:	03348463          	beq	s1,s3,2005df7e <eTaskGetState+0x52>
2005df5a:	03248263          	beq	s1,s2,2005df7e <eTaskGetState+0x52>
2005df5e:	200687b7          	lui	a5,0x20068
2005df62:	40878793          	addi	a5,a5,1032 # 20068408 <xSuspendedTaskList>
2005df66:	02f49363          	bne	s1,a5,2005df8c <eTaskGetState+0x60>
2005df6a:	541c                	c.lw	a5,40(s0)
2005df6c:	eb89                	c.bnez	a5,2005df7e <eTaskGetState+0x52>
2005df6e:	16044783          	lbu	a5,352(s0)
2005df72:	4705                	c.li	a4,1
2005df74:	0ff7f793          	andi	a5,a5,255
2005df78:	00e78363          	beq	a5,a4,2005df7e <eTaskGetState+0x52>
2005df7c:	450d                	c.li	a0,3
2005df7e:	40f2                	c.lwsp	ra,28(sp)
2005df80:	4462                	c.lwsp	s0,24(sp)
2005df82:	44d2                	c.lwsp	s1,20(sp)
2005df84:	4942                	c.lwsp	s2,16(sp)
2005df86:	49b2                	c.lwsp	s3,12(sp)
2005df88:	6105                	c.addi16sp	sp,32
2005df8a:	8082                	c.jr	ra
2005df8c:	200687b7          	lui	a5,0x20068
2005df90:	41c78793          	addi	a5,a5,1052 # 2006841c <xTasksWaitingTermination>
2005df94:	4511                	c.li	a0,4
2005df96:	fef484e3          	beq	s1,a5,2005df7e <eTaskGetState+0x52>
2005df9a:	d0f5                	c.beqz	s1,2005df7e <eTaskGetState+0x52>
2005df9c:	4505                	c.li	a0,1
2005df9e:	b7c5                	c.j	2005df7e <eTaskGetState+0x52>
2005dfa0:	4501                	c.li	a0,0
2005dfa2:	8082                	c.jr	ra

2005dfa4 <xTaskResumeAll>:
2005dfa4:	7139                	c.addi16sp	sp,-64
2005dfa6:	de06                	c.swsp	ra,60(sp)
2005dfa8:	dc22                	c.swsp	s0,56(sp)
2005dfaa:	da26                	c.swsp	s1,52(sp)
2005dfac:	d84a                	c.swsp	s2,48(sp)
2005dfae:	d64e                	c.swsp	s3,44(sp)
2005dfb0:	d452                	c.swsp	s4,40(sp)
2005dfb2:	d256                	c.swsp	s5,36(sp)
2005dfb4:	d05a                	c.swsp	s6,32(sp)
2005dfb6:	ce5e                	c.swsp	s7,28(sp)
2005dfb8:	c65ff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005dfbc:	f7418793          	addi	a5,gp,-140 # 20066924 <uxSchedulerSuspended>
2005dfc0:	4398                	c.lw	a4,0(a5)
2005dfc2:	177d                	c.addi	a4,-1
2005dfc4:	c398                	c.sw	a4,0(a5)
2005dfc6:	439c                	c.lw	a5,0(a5)
2005dfc8:	c38d                	c.beqz	a5,2005dfea <xTaskResumeAll+0x46>
2005dfca:	4501                	c.li	a0,0
2005dfcc:	c62a                	c.swsp	a0,12(sp)
2005dfce:	c69ff0ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005dfd2:	50f2                	c.lwsp	ra,60(sp)
2005dfd4:	5462                	c.lwsp	s0,56(sp)
2005dfd6:	4532                	c.lwsp	a0,12(sp)
2005dfd8:	54d2                	c.lwsp	s1,52(sp)
2005dfda:	5942                	c.lwsp	s2,48(sp)
2005dfdc:	59b2                	c.lwsp	s3,44(sp)
2005dfde:	5a22                	c.lwsp	s4,40(sp)
2005dfe0:	5a92                	c.lwsp	s5,36(sp)
2005dfe2:	5b02                	c.lwsp	s6,32(sp)
2005dfe4:	4bf2                	c.lwsp	s7,28(sp)
2005dfe6:	6121                	c.addi16sp	sp,64
2005dfe8:	8082                	c.jr	ra
2005dfea:	f6c1a783          	lw	a5,-148(gp) # 2006691c <uxCurrentNumberOfTasks>
2005dfee:	dff1                	c.beqz	a5,2005dfca <xTaskResumeAll+0x26>
2005dff0:	200684b7          	lui	s1,0x20068
2005dff4:	20068937          	lui	s2,0x20068
2005dff8:	4401                	c.li	s0,0
2005dffa:	3f448493          	addi	s1,s1,1012 # 200683f4 <xPendingReadyList>
2005dffe:	4985                	c.li	s3,1
2005e000:	2f090913          	addi	s2,s2,752 # 200682f0 <pxReadyTasksLists>
2005e004:	4ad1                	c.li	s5,20
2005e006:	a0b1                	c.j	2005e052 <xTaskResumeAll+0xae>
2005e008:	44dc                	c.lw	a5,12(s1)
2005e00a:	47c0                	c.lw	s0,12(a5)
2005e00c:	01840513          	addi	a0,s0,24
2005e010:	e93fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e014:	00440593          	addi	a1,s0,4
2005e018:	852e                	c.mv	a0,a1
2005e01a:	c62e                	c.swsp	a1,12(sp)
2005e01c:	e87fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e020:	5448                	c.lw	a0,44(s0)
2005e022:	f7c18713          	addi	a4,gp,-132 # 2006692c <uxTopReadyPriority>
2005e026:	4314                	c.lw	a3,0(a4)
2005e028:	00a997b3          	sll	a5,s3,a0
2005e02c:	03550533          	mul	a0,a0,s5
2005e030:	45b2                	c.lwsp	a1,12(sp)
2005e032:	8fd5                	c.or	a5,a3
2005e034:	c31c                	c.sw	a5,0(a4)
2005e036:	954a                	c.add	a0,s2
2005e038:	e27fc0ef          	jal	ra,2005ae5e <vListInsertEnd>
2005e03c:	f5818793          	addi	a5,gp,-168 # 20066908 <pxCurrentTCB>
2005e040:	439c                	c.lw	a5,0(a5)
2005e042:	5458                	c.lw	a4,44(s0)
2005e044:	57dc                	c.lw	a5,44(a5)
2005e046:	00f76663          	bltu	a4,a5,2005e052 <xTaskResumeAll+0xae>
2005e04a:	f9418793          	addi	a5,gp,-108 # 20066944 <xYieldPending>
2005e04e:	0137a023          	sw	s3,0(a5)
2005e052:	409c                	c.lw	a5,0(s1)
2005e054:	fbd5                	c.bnez	a5,2005e008 <xTaskResumeAll+0x64>
2005e056:	c019                	c.beqz	s0,2005e05c <xTaskResumeAll+0xb8>
2005e058:	d10ff0ef          	jal	ra,2005d568 <prvResetNextTaskUnblockTime>
2005e05c:	f8818713          	addi	a4,gp,-120 # 20066938 <xPendedTicks>
2005e060:	4304                	c.lw	s1,0(a4)
2005e062:	f8818413          	addi	s0,gp,-120 # 20066938 <xPendedTicks>
2005e066:	cc89                	c.beqz	s1,2005e080 <xTaskResumeAll+0xdc>
2005e068:	4985                	c.li	s3,1
2005e06a:	e96ff0ef          	jal	ra,2005d700 <xTaskIncrementTick>
2005e06e:	c509                	c.beqz	a0,2005e078 <xTaskResumeAll+0xd4>
2005e070:	f9418793          	addi	a5,gp,-108 # 20066944 <xYieldPending>
2005e074:	0137a023          	sw	s3,0(a5)
2005e078:	14fd                	c.addi	s1,-1
2005e07a:	f8e5                	c.bnez	s1,2005e06a <xTaskResumeAll+0xc6>
2005e07c:	00042023          	sw	zero,0(s0)
2005e080:	f941a783          	lw	a5,-108(gp) # 20066944 <xYieldPending>
2005e084:	d3b9                	c.beqz	a5,2005dfca <xTaskResumeAll+0x26>
2005e086:	00000073          	ecall
2005e08a:	4505                	c.li	a0,1
2005e08c:	b781                	c.j	2005dfcc <xTaskResumeAll+0x28>

2005e08e <vTaskDelay>:
2005e08e:	e501                	c.bnez	a0,2005e096 <vTaskDelay+0x8>
2005e090:	00000073          	ecall
2005e094:	8082                	c.jr	ra
2005e096:	1101                	c.addi	sp,-32
2005e098:	ce06                	c.swsp	ra,28(sp)
2005e09a:	c62a                	c.swsp	a0,12(sp)
2005e09c:	e40ff0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e0a0:	4532                	c.lwsp	a0,12(sp)
2005e0a2:	4581                	c.li	a1,0
2005e0a4:	ba0ff0ef          	jal	ra,2005d444 <prvAddCurrentTaskToDelayedList>
2005e0a8:	efdff0ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005e0ac:	e119                	c.bnez	a0,2005e0b2 <vTaskDelay+0x24>
2005e0ae:	00000073          	ecall
2005e0b2:	40f2                	c.lwsp	ra,28(sp)
2005e0b4:	6105                	c.addi16sp	sp,32
2005e0b6:	8082                	c.jr	ra

2005e0b8 <prvIdleTask>:
2005e0b8:	1101                	c.addi	sp,-32
2005e0ba:	ca26                	c.swsp	s1,20(sp)
2005e0bc:	200684b7          	lui	s1,0x20068
2005e0c0:	cc22                	c.swsp	s0,24(sp)
2005e0c2:	c452                	c.swsp	s4,8(sp)
2005e0c4:	c256                	c.swsp	s5,4(sp)
2005e0c6:	c05a                	c.swsp	s6,0(sp)
2005e0c8:	ce06                	c.swsp	ra,28(sp)
2005e0ca:	c84a                	c.swsp	s2,16(sp)
2005e0cc:	c64e                	c.swsp	s3,12(sp)
2005e0ce:	41c48493          	addi	s1,s1,1052 # 2006841c <xTasksWaitingTermination>
2005e0d2:	20068ab7          	lui	s5,0x20068
2005e0d6:	4405                	c.li	s0,1
2005e0d8:	f7018913          	addi	s2,gp,-144 # 20066920 <uxDeletedTasksWaitingCleanUp>
2005e0dc:	00092783          	lw	a5,0(s2)
2005e0e0:	eb8d                	c.bnez	a5,2005e112 <prvIdleTask+0x5a>
2005e0e2:	2f0a8793          	addi	a5,s5,752 # 200682f0 <pxReadyTasksLists>
2005e0e6:	439c                	c.lw	a5,0(a5)
2005e0e8:	00f47463          	bgeu	s0,a5,2005e0f0 <prvIdleTask+0x38>
2005e0ec:	00000073          	ecall
2005e0f0:	cdefe0ef          	jal	ra,2005c5ce <vApplicationIdleHook>
2005e0f4:	c46ff0ef          	jal	ra,2005d53a <prvGetExpectedIdleTime>
2005e0f8:	fea470e3          	bgeu	s0,a0,2005e0d8 <prvIdleTask+0x20>
2005e0fc:	de0ff0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e100:	c3aff0ef          	jal	ra,2005d53a <prvGetExpectedIdleTime>
2005e104:	00a47463          	bgeu	s0,a0,2005e10c <prvIdleTask+0x54>
2005e108:	d70fe0ef          	jal	ra,2005c678 <vPortSuppressTicksAndSleep>
2005e10c:	e99ff0ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005e110:	b7e1                	c.j	2005e0d8 <prvIdleTask+0x20>
2005e112:	b0bff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005e116:	44dc                	c.lw	a5,12(s1)
2005e118:	00c7a983          	lw	s3,12(a5)
2005e11c:	00498513          	addi	a0,s3,4
2005e120:	d83fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e124:	f6c18713          	addi	a4,gp,-148 # 2006691c <uxCurrentNumberOfTasks>
2005e128:	431c                	c.lw	a5,0(a4)
2005e12a:	17fd                	c.addi	a5,-1
2005e12c:	c31c                	c.sw	a5,0(a4)
2005e12e:	00092783          	lw	a5,0(s2)
2005e132:	17fd                	c.addi	a5,-1
2005e134:	00f92023          	sw	a5,0(s2)
2005e138:	affff0ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005e13c:	854e                	c.mv	a0,s3
2005e13e:	c46ff0ef          	jal	ra,2005d584 <prvDeleteTCB>
2005e142:	bf59                	c.j	2005e0d8 <prvIdleTask+0x20>

2005e144 <vTaskGetInfo>:
2005e144:	1141                	c.addi	sp,-16
2005e146:	c422                	c.swsp	s0,8(sp)
2005e148:	c226                	c.swsp	s1,4(sp)
2005e14a:	c04a                	c.swsp	s2,0(sp)
2005e14c:	c606                	c.swsp	ra,12(sp)
2005e14e:	842e                	c.mv	s0,a1
2005e150:	8932                	c.mv	s2,a2
2005e152:	84aa                	c.mv	s1,a0
2005e154:	e119                	c.bnez	a0,2005e15a <vTaskGetInfo+0x16>
2005e156:	f581a483          	lw	s1,-168(gp) # 20066908 <pxCurrentTCB>
2005e15a:	03448793          	addi	a5,s1,52
2005e15e:	c004                	c.sw	s1,0(s0)
2005e160:	c05c                	c.sw	a5,4(s0)
2005e162:	54dc                	c.lw	a5,44(s1)
2005e164:	c81c                	c.sw	a5,16(s0)
2005e166:	589c                	c.lw	a5,48(s1)
2005e168:	cc5c                	c.sw	a5,28(s0)
2005e16a:	48fc                	c.lw	a5,84(s1)
2005e16c:	c41c                	c.sw	a5,8(s0)
2005e16e:	4cfc                	c.lw	a5,92(s1)
2005e170:	c85c                	c.sw	a5,20(s0)
2005e172:	54bc                	c.lw	a5,104(s1)
2005e174:	cc1c                	c.sw	a5,24(s0)
2005e176:	4795                	c.li	a5,5
2005e178:	04f68c63          	beq	a3,a5,2005e1d0 <vTaskGetInfo+0x8c>
2005e17c:	f581a783          	lw	a5,-168(gp) # 20066908 <pxCurrentTCB>
2005e180:	02979963          	bne	a5,s1,2005e1b2 <vTaskGetInfo+0x6e>
2005e184:	00040623          	sb	zero,12(s0)
2005e188:	04090c63          	beq	s2,zero,2005e1e0 <vTaskGetInfo+0x9c>
2005e18c:	5890                	c.lw	a2,48(s1)
2005e18e:	4781                	c.li	a5,0
2005e190:	0a500713          	addi	a4,zero,165
2005e194:	00f606b3          	add	a3,a2,a5
2005e198:	0006c683          	lbu	a3,0(a3)
2005e19c:	04e68063          	beq	a3,a4,2005e1dc <vTaskGetInfo+0x98>
2005e1a0:	8389                	c.srli	a5,0x2
2005e1a2:	02f41023          	sh	a5,32(s0)
2005e1a6:	40b2                	c.lwsp	ra,12(sp)
2005e1a8:	4422                	c.lwsp	s0,8(sp)
2005e1aa:	4492                	c.lwsp	s1,4(sp)
2005e1ac:	4902                	c.lwsp	s2,0(sp)
2005e1ae:	0141                	c.addi	sp,16
2005e1b0:	8082                	c.jr	ra
2005e1b2:	00d40623          	sb	a3,12(s0)
2005e1b6:	478d                	c.li	a5,3
2005e1b8:	fcf698e3          	bne	a3,a5,2005e188 <vTaskGetInfo+0x44>
2005e1bc:	d20ff0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e1c0:	549c                	c.lw	a5,40(s1)
2005e1c2:	c781                	c.beqz	a5,2005e1ca <vTaskGetInfo+0x86>
2005e1c4:	4789                	c.li	a5,2
2005e1c6:	00f40623          	sb	a5,12(s0)
2005e1ca:	ddbff0ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005e1ce:	bf6d                	c.j	2005e188 <vTaskGetInfo+0x44>
2005e1d0:	8526                	c.mv	a0,s1
2005e1d2:	d5bff0ef          	jal	ra,2005df2c <eTaskGetState>
2005e1d6:	00a40623          	sb	a0,12(s0)
2005e1da:	b77d                	c.j	2005e188 <vTaskGetInfo+0x44>
2005e1dc:	0785                	c.addi	a5,1
2005e1de:	bf5d                	c.j	2005e194 <vTaskGetInfo+0x50>
2005e1e0:	02041023          	sh	zero,32(s0)
2005e1e4:	b7c9                	c.j	2005e1a6 <vTaskGetInfo+0x62>

2005e1e6 <prvListTasksWithinSingleList>:
2005e1e6:	7179                	c.addi16sp	sp,-48
2005e1e8:	d606                	c.swsp	ra,44(sp)
2005e1ea:	d422                	c.swsp	s0,40(sp)
2005e1ec:	d226                	c.swsp	s1,36(sp)
2005e1ee:	d04a                	c.swsp	s2,32(sp)
2005e1f0:	ce4e                	c.swsp	s3,28(sp)
2005e1f2:	cc52                	c.swsp	s4,24(sp)
2005e1f4:	ca56                	c.swsp	s5,20(sp)
2005e1f6:	c85a                	c.swsp	s6,16(sp)
2005e1f8:	c65e                	c.swsp	s7,12(sp)
2005e1fa:	4184                	c.lw	s1,0(a1)
2005e1fc:	c8a9                	c.beqz	s1,2005e24e <prvListTasksWithinSingleList+0x68>
2005e1fe:	41dc                	c.lw	a5,4(a1)
2005e200:	00858a13          	addi	s4,a1,8
2005e204:	892a                	c.mv	s2,a0
2005e206:	43dc                	c.lw	a5,4(a5)
2005e208:	842e                	c.mv	s0,a1
2005e20a:	89b2                	c.mv	s3,a2
2005e20c:	c1dc                	c.sw	a5,4(a1)
2005e20e:	01479463          	bne	a5,s4,2005e216 <prvListTasksWithinSingleList+0x30>
2005e212:	45dc                	c.lw	a5,12(a1)
2005e214:	c1dc                	c.sw	a5,4(a1)
2005e216:	405c                	c.lw	a5,4(s0)
2005e218:	4481                	c.li	s1,0
2005e21a:	02400b93          	addi	s7,zero,36
2005e21e:	00c7ab03          	lw	s6,12(a5)
2005e222:	405c                	c.lw	a5,4(s0)
2005e224:	43dc                	c.lw	a5,4(a5)
2005e226:	c05c                	c.sw	a5,4(s0)
2005e228:	00fa1563          	bne	s4,a5,2005e232 <prvListTasksWithinSingleList+0x4c>
2005e22c:	004a2783          	lw	a5,4(s4)
2005e230:	c05c                	c.sw	a5,4(s0)
2005e232:	037485b3          	mul	a1,s1,s7
2005e236:	405c                	c.lw	a5,4(s0)
2005e238:	86ce                	c.mv	a3,s3
2005e23a:	4605                	c.li	a2,1
2005e23c:	00c7aa83          	lw	s5,12(a5)
2005e240:	0485                	c.addi	s1,1
2005e242:	8556                	c.mv	a0,s5
2005e244:	95ca                	c.add	a1,s2
2005e246:	effff0ef          	jal	ra,2005e144 <vTaskGetInfo>
2005e24a:	fd5b1ce3          	bne	s6,s5,2005e222 <prvListTasksWithinSingleList+0x3c>
2005e24e:	50b2                	c.lwsp	ra,44(sp)
2005e250:	5422                	c.lwsp	s0,40(sp)
2005e252:	5902                	c.lwsp	s2,32(sp)
2005e254:	49f2                	c.lwsp	s3,28(sp)
2005e256:	4a62                	c.lwsp	s4,24(sp)
2005e258:	4ad2                	c.lwsp	s5,20(sp)
2005e25a:	4b42                	c.lwsp	s6,16(sp)
2005e25c:	4bb2                	c.lwsp	s7,12(sp)
2005e25e:	8526                	c.mv	a0,s1
2005e260:	5492                	c.lwsp	s1,36(sp)
2005e262:	6145                	c.addi16sp	sp,48
2005e264:	8082                	c.jr	ra

2005e266 <uxTaskGetSystemState>:
2005e266:	1101                	c.addi	sp,-32
2005e268:	cc22                	c.swsp	s0,24(sp)
2005e26a:	c84a                	c.swsp	s2,16(sp)
2005e26c:	c64e                	c.swsp	s3,12(sp)
2005e26e:	ce06                	c.swsp	ra,28(sp)
2005e270:	ca26                	c.swsp	s1,20(sp)
2005e272:	c452                	c.swsp	s4,8(sp)
2005e274:	c256                	c.swsp	s5,4(sp)
2005e276:	c05a                	c.swsp	s6,0(sp)
2005e278:	892a                	c.mv	s2,a0
2005e27a:	842e                	c.mv	s0,a1
2005e27c:	89b2                	c.mv	s3,a2
2005e27e:	c5eff0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e282:	f6c1a783          	lw	a5,-148(gp) # 2006691c <uxCurrentNumberOfTasks>
2005e286:	0af46563          	bltu	s0,a5,2005e330 <uxTaskGetSystemState+0xca>
2005e28a:	20068ab7          	lui	s5,0x20068
2005e28e:	4a2d                	c.li	s4,11
2005e290:	4401                	c.li	s0,0
2005e292:	4b51                	c.li	s6,20
2005e294:	2f0a8a93          	addi	s5,s5,752 # 200682f0 <pxReadyTasksLists>
2005e298:	02400493          	addi	s1,zero,36
2005e29c:	1a7d                	c.addi	s4,-1
2005e29e:	02940533          	mul	a0,s0,s1
2005e2a2:	4605                	c.li	a2,1
2005e2a4:	036a05b3          	mul	a1,s4,s6
2005e2a8:	954a                	c.add	a0,s2
2005e2aa:	95d6                	c.add	a1,s5
2005e2ac:	f3bff0ef          	jal	ra,2005e1e6 <prvListTasksWithinSingleList>
2005e2b0:	942a                	c.add	s0,a0
2005e2b2:	fe0a15e3          	bne	s4,zero,2005e29c <uxTaskGetSystemState+0x36>
2005e2b6:	02940533          	mul	a0,s0,s1
2005e2ba:	f5c1a583          	lw	a1,-164(gp) # 2006690c <pxDelayedTaskList>
2005e2be:	4609                	c.li	a2,2
2005e2c0:	954a                	c.add	a0,s2
2005e2c2:	f25ff0ef          	jal	ra,2005e1e6 <prvListTasksWithinSingleList>
2005e2c6:	942a                	c.add	s0,a0
2005e2c8:	02940533          	mul	a0,s0,s1
2005e2cc:	f601a583          	lw	a1,-160(gp) # 20066910 <pxOverflowDelayedTaskList>
2005e2d0:	4609                	c.li	a2,2
2005e2d2:	954a                	c.add	a0,s2
2005e2d4:	f13ff0ef          	jal	ra,2005e1e6 <prvListTasksWithinSingleList>
2005e2d8:	942a                	c.add	s0,a0
2005e2da:	02940533          	mul	a0,s0,s1
2005e2de:	200685b7          	lui	a1,0x20068
2005e2e2:	4611                	c.li	a2,4
2005e2e4:	41c58593          	addi	a1,a1,1052 # 2006841c <xTasksWaitingTermination>
2005e2e8:	954a                	c.add	a0,s2
2005e2ea:	efdff0ef          	jal	ra,2005e1e6 <prvListTasksWithinSingleList>
2005e2ee:	942a                	c.add	s0,a0
2005e2f0:	029404b3          	mul	s1,s0,s1
2005e2f4:	200685b7          	lui	a1,0x20068
2005e2f8:	460d                	c.li	a2,3
2005e2fa:	40858593          	addi	a1,a1,1032 # 20068408 <xSuspendedTaskList>
2005e2fe:	00990533          	add	a0,s2,s1
2005e302:	ee5ff0ef          	jal	ra,2005e1e6 <prvListTasksWithinSingleList>
2005e306:	942a                	c.add	s0,a0
2005e308:	00098763          	beq	s3,zero,2005e316 <uxTaskGetSystemState+0xb0>
2005e30c:	410187b7          	lui	a5,0x41018
2005e310:	4bdc                	c.lw	a5,20(a5)
2005e312:	00f9a023          	sw	a5,0(s3)
2005e316:	c8fff0ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005e31a:	40f2                	c.lwsp	ra,28(sp)
2005e31c:	8522                	c.mv	a0,s0
2005e31e:	4462                	c.lwsp	s0,24(sp)
2005e320:	44d2                	c.lwsp	s1,20(sp)
2005e322:	4942                	c.lwsp	s2,16(sp)
2005e324:	49b2                	c.lwsp	s3,12(sp)
2005e326:	4a22                	c.lwsp	s4,8(sp)
2005e328:	4a92                	c.lwsp	s5,4(sp)
2005e32a:	4b02                	c.lwsp	s6,0(sp)
2005e32c:	6105                	c.addi16sp	sp,32
2005e32e:	8082                	c.jr	ra
2005e330:	4401                	c.li	s0,0
2005e332:	b7d5                	c.j	2005e316 <uxTaskGetSystemState+0xb0>

2005e334 <xTaskGetTickCount>:
2005e334:	1101                	c.addi	sp,-32
2005e336:	ce06                	c.swsp	ra,28(sp)
2005e338:	8e5ff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005e33c:	f901a503          	lw	a0,-112(gp) # 20066940 <xTickCount>
2005e340:	c62a                	c.swsp	a0,12(sp)
2005e342:	8f5ff0ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005e346:	40f2                	c.lwsp	ra,28(sp)
2005e348:	4532                	c.lwsp	a0,12(sp)
2005e34a:	6105                	c.addi16sp	sp,32
2005e34c:	8082                	c.jr	ra

2005e34e <xTaskCheckForTimeOut>:
2005e34e:	1101                	c.addi	sp,-32
2005e350:	cc22                	c.swsp	s0,24(sp)
2005e352:	ce06                	c.swsp	ra,28(sp)
2005e354:	842e                	c.mv	s0,a1
2005e356:	c62a                	c.swsp	a0,12(sp)
2005e358:	8c5ff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005e35c:	f901a683          	lw	a3,-112(gp) # 20066940 <xTickCount>
2005e360:	401c                	c.lw	a5,0(s0)
2005e362:	577d                	c.li	a4,-1
2005e364:	02e78f63          	beq	a5,a4,2005e3a2 <xTaskCheckForTimeOut+0x54>
2005e368:	4532                	c.lwsp	a0,12(sp)
2005e36a:	f841a603          	lw	a2,-124(gp) # 20066934 <xNumOfOverflows>
2005e36e:	410c                	c.lw	a1,0(a0)
2005e370:	4158                	c.lw	a4,4(a0)
2005e372:	00c58f63          	beq	a1,a2,2005e390 <xTaskCheckForTimeOut+0x42>
2005e376:	00e6ed63          	bltu	a3,a4,2005e390 <xTaskCheckForTimeOut+0x42>
2005e37a:	00042023          	sw	zero,0(s0)
2005e37e:	4505                	c.li	a0,1
2005e380:	c62a                	c.swsp	a0,12(sp)
2005e382:	8b5ff0ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005e386:	40f2                	c.lwsp	ra,28(sp)
2005e388:	4462                	c.lwsp	s0,24(sp)
2005e38a:	4532                	c.lwsp	a0,12(sp)
2005e38c:	6105                	c.addi16sp	sp,32
2005e38e:	8082                	c.jr	ra
2005e390:	40e68633          	sub	a2,a3,a4
2005e394:	fef673e3          	bgeu	a2,a5,2005e37a <xTaskCheckForTimeOut+0x2c>
2005e398:	8f95                	c.sub	a5,a3
2005e39a:	97ba                	c.add	a5,a4
2005e39c:	c01c                	c.sw	a5,0(s0)
2005e39e:	e50ff0ef          	jal	ra,2005d9ee <vTaskInternalSetTimeOutState>
2005e3a2:	4501                	c.li	a0,0
2005e3a4:	bff1                	c.j	2005e380 <xTaskCheckForTimeOut+0x32>

2005e3a6 <vTaskList>:
2005e3a6:	7139                	c.addi16sp	sp,-64
2005e3a8:	dc22                	c.swsp	s0,56(sp)
2005e3aa:	de06                	c.swsp	ra,60(sp)
2005e3ac:	da26                	c.swsp	s1,52(sp)
2005e3ae:	d84a                	c.swsp	s2,48(sp)
2005e3b0:	d64e                	c.swsp	s3,44(sp)
2005e3b2:	d452                	c.swsp	s4,40(sp)
2005e3b4:	d256                	c.swsp	s5,36(sp)
2005e3b6:	d05a                	c.swsp	s6,32(sp)
2005e3b8:	ce5e                	c.swsp	s7,28(sp)
2005e3ba:	f6c18793          	addi	a5,gp,-148 # 2006691c <uxCurrentNumberOfTasks>
2005e3be:	438c                	c.lw	a1,0(a5)
2005e3c0:	842a                	c.mv	s0,a0
2005e3c2:	00050023          	sb	zero,0(a0)
2005e3c6:	4388                	c.lw	a0,0(a5)
2005e3c8:	02400793          	addi	a5,zero,36
2005e3cc:	c62e                	c.swsp	a1,12(sp)
2005e3ce:	02f50533          	mul	a0,a0,a5
2005e3d2:	839fc0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005e3d6:	c151                	c.beqz	a0,2005e45a <vTaskList+0xb4>
2005e3d8:	45b2                	c.lwsp	a1,12(sp)
2005e3da:	4601                	c.li	a2,0
2005e3dc:	892a                	c.mv	s2,a0
2005e3de:	e89ff0ef          	jal	ra,2005e266 <uxTaskGetSystemState>
2005e3e2:	89aa                	c.mv	s3,a0
2005e3e4:	00490493          	addi	s1,s2,4
2005e3e8:	4a01                	c.li	s4,0
2005e3ea:	4b91                	c.li	s7,4
2005e3ec:	e5818a93          	addi	s5,gp,-424 # 20066808 <CSWTCH.251>
2005e3f0:	20063b37          	lui	s6,0x20063
2005e3f4:	013a1f63          	bne	s4,s3,2005e412 <vTaskList+0x6c>
2005e3f8:	5462                	c.lwsp	s0,56(sp)
2005e3fa:	50f2                	c.lwsp	ra,60(sp)
2005e3fc:	54d2                	c.lwsp	s1,52(sp)
2005e3fe:	59b2                	c.lwsp	s3,44(sp)
2005e400:	5a22                	c.lwsp	s4,40(sp)
2005e402:	5a92                	c.lwsp	s5,36(sp)
2005e404:	5b02                	c.lwsp	s6,32(sp)
2005e406:	4bf2                	c.lwsp	s7,28(sp)
2005e408:	854a                	c.mv	a0,s2
2005e40a:	5942                	c.lwsp	s2,48(sp)
2005e40c:	6121                	c.addi16sp	sp,64
2005e40e:	8dbfc06f          	jal	zero,2005ace8 <vPortFree>
2005e412:	0084c783          	lbu	a5,8(s1)
2005e416:	4681                	c.li	a3,0
2005e418:	00fbe563          	bltu	s7,a5,2005e422 <vTaskList+0x7c>
2005e41c:	97d6                	c.add	a5,s5
2005e41e:	0007c683          	lbu	a3,0(a5) # 41018000 <__km4_bd_boot_download_addr__+0x11006000>
2005e422:	408c                	c.lw	a1,0(s1)
2005e424:	8522                	c.mv	a0,s0
2005e426:	c636                	c.swsp	a3,12(sp)
2005e428:	8c8ff0ef          	jal	ra,2005d4f0 <prvWriteNameToBuffer>
2005e42c:	0044a803          	lw	a6,4(s1)
2005e430:	01c4d783          	lhu	a5,28(s1)
2005e434:	44d8                	c.lw	a4,12(s1)
2005e436:	46b2                	c.lwsp	a3,12(sp)
2005e438:	6a8b0613          	addi	a2,s6,1704 # 200636a8 <__func__.1+0x20>
2005e43c:	40000593          	addi	a1,zero,1024
2005e440:	842a                	c.mv	s0,a0
2005e442:	17e010ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005e446:	8522                	c.mv	a0,s0
2005e448:	dffab097          	auipc	ra,0xdffab
2005e44c:	690080e7          	jalr	ra,1680(ra) # 9ad8 <__wrap_strlen>
2005e450:	942a                	c.add	s0,a0
2005e452:	0a05                	c.addi	s4,1
2005e454:	02448493          	addi	s1,s1,36
2005e458:	bf71                	c.j	2005e3f4 <vTaskList+0x4e>
2005e45a:	50f2                	c.lwsp	ra,60(sp)
2005e45c:	5462                	c.lwsp	s0,56(sp)
2005e45e:	54d2                	c.lwsp	s1,52(sp)
2005e460:	5942                	c.lwsp	s2,48(sp)
2005e462:	59b2                	c.lwsp	s3,44(sp)
2005e464:	5a22                	c.lwsp	s4,40(sp)
2005e466:	5a92                	c.lwsp	s5,36(sp)
2005e468:	5b02                	c.lwsp	s6,32(sp)
2005e46a:	4bf2                	c.lwsp	s7,28(sp)
2005e46c:	6121                	c.addi16sp	sp,64
2005e46e:	8082                	c.jr	ra

2005e470 <pvTaskIncrementMutexHeldCount>:
2005e470:	f5818713          	addi	a4,gp,-168 # 20066908 <pxCurrentTCB>
2005e474:	4318                	c.lw	a4,0(a4)
2005e476:	f5818793          	addi	a5,gp,-168 # 20066908 <pxCurrentTCB>
2005e47a:	c709                	c.beqz	a4,2005e484 <pvTaskIncrementMutexHeldCount+0x14>
2005e47c:	4394                	c.lw	a3,0(a5)
2005e47e:	52b8                	c.lw	a4,96(a3)
2005e480:	0705                	c.addi	a4,1
2005e482:	d2b8                	c.sw	a4,96(a3)
2005e484:	4388                	c.lw	a0,0(a5)
2005e486:	8082                	c.jr	ra

2005e488 <prvCheckForValidListAndQueue>:
2005e488:	1141                	c.addi	sp,-16
2005e48a:	c422                	c.swsp	s0,8(sp)
2005e48c:	c606                	c.swsp	ra,12(sp)
2005e48e:	c226                	c.swsp	s1,4(sp)
2005e490:	c04a                	c.swsp	s2,0(sp)
2005e492:	fa418413          	addi	s0,gp,-92 # 20066954 <xTimerQueue>
2005e496:	f86ff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005e49a:	401c                	c.lw	a5,0(s0)
2005e49c:	e3b9                	c.bnez	a5,2005e4e2 <prvCheckForValidListAndQueue+0x5a>
2005e49e:	20068537          	lui	a0,0x20068
2005e4a2:	4a850913          	addi	s2,a0,1192 # 200684a8 <xActiveTimerList1>
2005e4a6:	4a850513          	addi	a0,a0,1192
2005e4aa:	99bfc0ef          	jal	ra,2005ae44 <vListInitialise>
2005e4ae:	20068537          	lui	a0,0x20068
2005e4b2:	4bc50493          	addi	s1,a0,1212 # 200684bc <xActiveTimerList2>
2005e4b6:	4bc50513          	addi	a0,a0,1212
2005e4ba:	98bfc0ef          	jal	ra,2005ae44 <vListInitialise>
2005e4be:	200686b7          	lui	a3,0x20068
2005e4c2:	20068637          	lui	a2,0x20068
2005e4c6:	f921ac23          	sw	s2,-104(gp) # 20066948 <pxCurrentTimerList>
2005e4ca:	4701                	c.li	a4,0
2005e4cc:	4d068693          	addi	a3,a3,1232 # 200684d0 <xStaticTimerQueue.0>
2005e4d0:	43060613          	addi	a2,a2,1072 # 20068430 <ucStaticTimerQueueStorage.1>
2005e4d4:	45b1                	c.li	a1,12
2005e4d6:	4529                	c.li	a0,10
2005e4d8:	f891ae23          	sw	s1,-100(gp) # 2006694c <pxOverflowTimerList>
2005e4dc:	967fe0ef          	jal	ra,2005ce42 <xQueueGenericCreateStatic>
2005e4e0:	c008                	c.sw	a0,0(s0)
2005e4e2:	4422                	c.lwsp	s0,8(sp)
2005e4e4:	40b2                	c.lwsp	ra,12(sp)
2005e4e6:	4492                	c.lwsp	s1,4(sp)
2005e4e8:	4902                	c.lwsp	s2,0(sp)
2005e4ea:	0141                	c.addi	sp,16
2005e4ec:	f4aff06f          	jal	zero,2005dc36 <vTaskExitCritical>

2005e4f0 <prvInsertTimerInActiveList>:
2005e4f0:	1141                	c.addi	sp,-16
2005e4f2:	c606                	c.swsp	ra,12(sp)
2005e4f4:	c14c                	c.sw	a1,4(a0)
2005e4f6:	c908                	c.sw	a0,16(a0)
2005e4f8:	87aa                	c.mv	a5,a0
2005e4fa:	02b66163          	bltu	a2,a1,2005e51c <prvInsertTimerInActiveList+0x2c>
2005e4fe:	4d18                	c.lw	a4,24(a0)
2005e500:	8e15                	c.sub	a2,a3
2005e502:	4505                	c.li	a0,1
2005e504:	00e67963          	bgeu	a2,a4,2005e516 <prvInsertTimerInActiveList+0x26>
2005e508:	00478593          	addi	a1,a5,4
2005e50c:	f9c1a503          	lw	a0,-100(gp) # 2006694c <pxOverflowTimerList>
2005e510:	965fc0ef          	jal	ra,2005ae74 <vListInsert>
2005e514:	4501                	c.li	a0,0
2005e516:	40b2                	c.lwsp	ra,12(sp)
2005e518:	0141                	c.addi	sp,16
2005e51a:	8082                	c.jr	ra
2005e51c:	00d67563          	bgeu	a2,a3,2005e526 <prvInsertTimerInActiveList+0x36>
2005e520:	4505                	c.li	a0,1
2005e522:	fed5fae3          	bgeu	a1,a3,2005e516 <prvInsertTimerInActiveList+0x26>
2005e526:	00478593          	addi	a1,a5,4
2005e52a:	f981a503          	lw	a0,-104(gp) # 20066948 <pxCurrentTimerList>
2005e52e:	b7cd                	c.j	2005e510 <prvInsertTimerInActiveList+0x20>

2005e530 <xTimerCreateTimerTask>:
2005e530:	1101                	c.addi	sp,-32
2005e532:	ce06                	c.swsp	ra,28(sp)
2005e534:	f55ff0ef          	jal	ra,2005e488 <prvCheckForValidListAndQueue>
2005e538:	fa41a783          	lw	a5,-92(gp) # 20066954 <xTimerQueue>
2005e53c:	4501                	c.li	a0,0
2005e53e:	cb9d                	c.beqz	a5,2005e574 <xTimerCreateTimerTask+0x44>
2005e540:	0070                	c.addi4spn	a2,sp,12
2005e542:	002c                	c.addi4spn	a1,sp,8
2005e544:	0048                	c.addi4spn	a0,sp,4
2005e546:	c202                	c.swsp	zero,4(sp)
2005e548:	c402                	c.swsp	zero,8(sp)
2005e54a:	912fe0ef          	jal	ra,2005c65c <vApplicationGetTimerTaskMemory>
2005e54e:	47a2                	c.lwsp	a5,8(sp)
2005e550:	4812                	c.lwsp	a6,4(sp)
2005e552:	4632                	c.lwsp	a2,12(sp)
2005e554:	200635b7          	lui	a1,0x20063
2005e558:	2005e537          	lui	a0,0x2005e
2005e55c:	4729                	c.li	a4,10
2005e55e:	4681                	c.li	a3,0
2005e560:	6b858593          	addi	a1,a1,1720 # 200636b8 <__func__.1+0x30>
2005e564:	7b650513          	addi	a0,a0,1974 # 2005e7b6 <prvTimerTask>
2005e568:	ff8ff0ef          	jal	ra,2005dd60 <xTaskCreateStatic>
2005e56c:	faa1a423          	sw	a0,-88(gp) # 20066958 <xTimerTaskHandle>
2005e570:	00a03533          	sltu	a0,zero,a0
2005e574:	40f2                	c.lwsp	ra,28(sp)
2005e576:	6105                	c.addi16sp	sp,32
2005e578:	8082                	c.jr	ra

2005e57a <xTimerCreate>:
2005e57a:	1101                	c.addi	sp,-32
2005e57c:	c256                	c.swsp	s5,4(sp)
2005e57e:	8aaa                	c.mv	s5,a0
2005e580:	02c00513          	addi	a0,zero,44
2005e584:	cc22                	c.swsp	s0,24(sp)
2005e586:	ca26                	c.swsp	s1,20(sp)
2005e588:	c84a                	c.swsp	s2,16(sp)
2005e58a:	c64e                	c.swsp	s3,12(sp)
2005e58c:	c452                	c.swsp	s4,8(sp)
2005e58e:	ce06                	c.swsp	ra,28(sp)
2005e590:	8a2e                	c.mv	s4,a1
2005e592:	84b2                	c.mv	s1,a2
2005e594:	89b6                	c.mv	s3,a3
2005e596:	893a                	c.mv	s2,a4
2005e598:	e72fc0ef          	jal	ra,2005ac0a <pvPortMalloc>
2005e59c:	842a                	c.mv	s0,a0
2005e59e:	c905                	c.beqz	a0,2005e5ce <xTimerCreate+0x54>
2005e5a0:	02050423          	sb	zero,40(a0)
2005e5a4:	ee5ff0ef          	jal	ra,2005e488 <prvCheckForValidListAndQueue>
2005e5a8:	00440513          	addi	a0,s0,4
2005e5ac:	01542023          	sw	s5,0(s0)
2005e5b0:	01442c23          	sw	s4,24(s0)
2005e5b4:	01342e23          	sw	s3,28(s0)
2005e5b8:	03242023          	sw	s2,32(s0)
2005e5bc:	89dfc0ef          	jal	ra,2005ae58 <vListInitialiseItem>
2005e5c0:	c499                	c.beqz	s1,2005e5ce <xTimerCreate+0x54>
2005e5c2:	02844783          	lbu	a5,40(s0)
2005e5c6:	0047e793          	ori	a5,a5,4
2005e5ca:	02f40423          	sb	a5,40(s0)
2005e5ce:	40f2                	c.lwsp	ra,28(sp)
2005e5d0:	8522                	c.mv	a0,s0
2005e5d2:	4462                	c.lwsp	s0,24(sp)
2005e5d4:	44d2                	c.lwsp	s1,20(sp)
2005e5d6:	4942                	c.lwsp	s2,16(sp)
2005e5d8:	49b2                	c.lwsp	s3,12(sp)
2005e5da:	4a22                	c.lwsp	s4,8(sp)
2005e5dc:	4a92                	c.lwsp	s5,4(sp)
2005e5de:	6105                	c.addi16sp	sp,32
2005e5e0:	8082                	c.jr	ra

2005e5e2 <xTimerCreateStatic>:
2005e5e2:	1101                	c.addi	sp,-32
2005e5e4:	cc22                	c.swsp	s0,24(sp)
2005e5e6:	ce06                	c.swsp	ra,28(sp)
2005e5e8:	ca26                	c.swsp	s1,20(sp)
2005e5ea:	c84a                	c.swsp	s2,16(sp)
2005e5ec:	c64e                	c.swsp	s3,12(sp)
2005e5ee:	c452                	c.swsp	s4,8(sp)
2005e5f0:	c256                	c.swsp	s5,4(sp)
2005e5f2:	843e                	c.mv	s0,a5
2005e5f4:	cf95                	c.beqz	a5,2005e630 <xTimerCreateStatic+0x4e>
2005e5f6:	4789                	c.li	a5,2
2005e5f8:	02f40423          	sb	a5,40(s0)
2005e5fc:	8aaa                	c.mv	s5,a0
2005e5fe:	8a2e                	c.mv	s4,a1
2005e600:	84b2                	c.mv	s1,a2
2005e602:	89b6                	c.mv	s3,a3
2005e604:	893a                	c.mv	s2,a4
2005e606:	e83ff0ef          	jal	ra,2005e488 <prvCheckForValidListAndQueue>
2005e60a:	00440513          	addi	a0,s0,4
2005e60e:	01542023          	sw	s5,0(s0)
2005e612:	01442c23          	sw	s4,24(s0)
2005e616:	01342e23          	sw	s3,28(s0)
2005e61a:	03242023          	sw	s2,32(s0)
2005e61e:	83bfc0ef          	jal	ra,2005ae58 <vListInitialiseItem>
2005e622:	c499                	c.beqz	s1,2005e630 <xTimerCreateStatic+0x4e>
2005e624:	02844783          	lbu	a5,40(s0)
2005e628:	0047e793          	ori	a5,a5,4
2005e62c:	02f40423          	sb	a5,40(s0)
2005e630:	40f2                	c.lwsp	ra,28(sp)
2005e632:	8522                	c.mv	a0,s0
2005e634:	4462                	c.lwsp	s0,24(sp)
2005e636:	44d2                	c.lwsp	s1,20(sp)
2005e638:	4942                	c.lwsp	s2,16(sp)
2005e63a:	49b2                	c.lwsp	s3,12(sp)
2005e63c:	4a22                	c.lwsp	s4,8(sp)
2005e63e:	4a92                	c.lwsp	s5,4(sp)
2005e640:	6105                	c.addi16sp	sp,32
2005e642:	8082                	c.jr	ra

2005e644 <xTimerGenericCommand>:
2005e644:	7139                	c.addi16sp	sp,-64
2005e646:	dc22                	c.swsp	s0,56(sp)
2005e648:	da26                	c.swsp	s1,52(sp)
2005e64a:	d84a                	c.swsp	s2,48(sp)
2005e64c:	d452                	c.swsp	s4,40(sp)
2005e64e:	d256                	c.swsp	s5,36(sp)
2005e650:	de06                	c.swsp	ra,60(sp)
2005e652:	d64e                	c.swsp	s3,44(sp)
2005e654:	842a                	c.mv	s0,a0
2005e656:	84ae                	c.mv	s1,a1
2005e658:	8932                	c.mv	s2,a2
2005e65a:	8a36                	c.mv	s4,a3
2005e65c:	8aba                	c.mv	s5,a4
2005e65e:	bd6ff0ef          	jal	ra,2005da34 <xTaskGetCurrentTaskHandle>
2005e662:	fa81a783          	lw	a5,-88(gp) # 20066958 <xTimerTaskHandle>
2005e666:	06a79163          	bne	a5,a0,2005e6c8 <xTimerGenericCommand+0x84>
2005e66a:	ffd48793          	addi	a5,s1,-3
2005e66e:	4705                	c.li	a4,1
2005e670:	04f76c63          	bltu	a4,a5,2005e6c8 <xTimerGenericCommand+0x84>
2005e674:	cc1ff0ef          	jal	ra,2005e334 <xTaskGetTickCount>
2005e678:	485c                	c.lw	a5,20(s0)
2005e67a:	86aa                	c.mv	a3,a0
2005e67c:	c799                	c.beqz	a5,2005e68a <xTimerGenericCommand+0x46>
2005e67e:	c62a                	c.swsp	a0,12(sp)
2005e680:	00440513          	addi	a0,s0,4
2005e684:	81ffc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e688:	46b2                	c.lwsp	a3,12(sp)
2005e68a:	4711                	c.li	a4,4
2005e68c:	02844783          	lbu	a5,40(s0)
2005e690:	00e48f63          	beq	s1,a4,2005e6ae <xTimerGenericCommand+0x6a>
2005e694:	9bf9                	c.andi	a5,-2
2005e696:	02f40423          	sb	a5,40(s0)
2005e69a:	4505                	c.li	a0,1
2005e69c:	50f2                	c.lwsp	ra,60(sp)
2005e69e:	5462                	c.lwsp	s0,56(sp)
2005e6a0:	54d2                	c.lwsp	s1,52(sp)
2005e6a2:	5942                	c.lwsp	s2,48(sp)
2005e6a4:	59b2                	c.lwsp	s3,44(sp)
2005e6a6:	5a22                	c.lwsp	s4,40(sp)
2005e6a8:	5a92                	c.lwsp	s5,36(sp)
2005e6aa:	6121                	c.addi16sp	sp,64
2005e6ac:	8082                	c.jr	ra
2005e6ae:	0017e793          	ori	a5,a5,1
2005e6b2:	02f40423          	sb	a5,40(s0)
2005e6b6:	01242c23          	sw	s2,24(s0)
2005e6ba:	8636                	c.mv	a2,a3
2005e6bc:	00d905b3          	add	a1,s2,a3
2005e6c0:	8522                	c.mv	a0,s0
2005e6c2:	e2fff0ef          	jal	ra,2005e4f0 <prvInsertTimerInActiveList>
2005e6c6:	bfd1                	c.j	2005e69a <xTimerGenericCommand+0x56>
2005e6c8:	fa418993          	addi	s3,gp,-92 # 20066954 <xTimerQueue>
2005e6cc:	0009a783          	lw	a5,0(s3)
2005e6d0:	4501                	c.li	a0,0
2005e6d2:	d7e9                	c.beqz	a5,2005e69c <xTimerGenericCommand+0x58>
2005e6d4:	ca26                	c.swsp	s1,20(sp)
2005e6d6:	cc4a                	c.swsp	s2,24(sp)
2005e6d8:	ce22                	c.swsp	s0,28(sp)
2005e6da:	4715                	c.li	a4,5
2005e6dc:	02974163          	blt	a4,s1,2005e6fe <xTimerGenericCommand+0xba>
2005e6e0:	b5aff0ef          	jal	ra,2005da3a <xTaskGetSchedulerState>
2005e6e4:	872a                	c.mv	a4,a0
2005e6e6:	4789                	c.li	a5,2
2005e6e8:	0009a503          	lw	a0,0(s3)
2005e6ec:	4681                	c.li	a3,0
2005e6ee:	8656                	c.mv	a2,s5
2005e6f0:	00f70363          	beq	a4,a5,2005e6f6 <xTimerGenericCommand+0xb2>
2005e6f4:	4601                	c.li	a2,0
2005e6f6:	084c                	c.addi4spn	a1,sp,20
2005e6f8:	9c8fe0ef          	jal	ra,2005c8c0 <xQueueGenericSend>
2005e6fc:	b745                	c.j	2005e69c <xTimerGenericCommand+0x58>
2005e6fe:	4681                	c.li	a3,0
2005e700:	8652                	c.mv	a2,s4
2005e702:	084c                	c.addi4spn	a1,sp,20
2005e704:	853e                	c.mv	a0,a5
2005e706:	abefe0ef          	jal	ra,2005c9c4 <xQueueGenericSendFromISR>
2005e70a:	bf49                	c.j	2005e69c <xTimerGenericCommand+0x58>

2005e70c <prvSampleTimeNow>:
2005e70c:	7179                	c.addi16sp	sp,-48
2005e70e:	d226                	c.swsp	s1,36(sp)
2005e710:	d04a                	c.swsp	s2,32(sp)
2005e712:	ce4e                	c.swsp	s3,28(sp)
2005e714:	d606                	c.swsp	ra,44(sp)
2005e716:	d422                	c.swsp	s0,40(sp)
2005e718:	cc52                	c.swsp	s4,24(sp)
2005e71a:	ca56                	c.swsp	s5,20(sp)
2005e71c:	89aa                	c.mv	s3,a0
2005e71e:	c17ff0ef          	jal	ra,2005e334 <xTaskGetTickCount>
2005e722:	fa01a783          	lw	a5,-96(gp) # 20066950 <xLastTime.2>
2005e726:	84aa                	c.mv	s1,a0
2005e728:	fa018913          	addi	s2,gp,-96 # 20066950 <xLastTime.2>
2005e72c:	08f57263          	bgeu	a0,a5,2005e7b0 <prvSampleTimeNow+0xa4>
2005e730:	f9818a13          	addi	s4,gp,-104 # 20066948 <pxCurrentTimerList>
2005e734:	000a2703          	lw	a4,0(s4)
2005e738:	431c                	c.lw	a5,0(a4)
2005e73a:	e795                	c.bnez	a5,2005e766 <prvSampleTimeNow+0x5a>
2005e73c:	f9c18793          	addi	a5,gp,-100 # 2006694c <pxOverflowTimerList>
2005e740:	4394                	c.lw	a3,0(a5)
2005e742:	c398                	c.sw	a4,0(a5)
2005e744:	4785                	c.li	a5,1
2005e746:	00da2023          	sw	a3,0(s4)
2005e74a:	00f9a023          	sw	a5,0(s3)
2005e74e:	50b2                	c.lwsp	ra,44(sp)
2005e750:	5422                	c.lwsp	s0,40(sp)
2005e752:	00992023          	sw	s1,0(s2)
2005e756:	49f2                	c.lwsp	s3,28(sp)
2005e758:	5902                	c.lwsp	s2,32(sp)
2005e75a:	4a62                	c.lwsp	s4,24(sp)
2005e75c:	4ad2                	c.lwsp	s5,20(sp)
2005e75e:	8526                	c.mv	a0,s1
2005e760:	5492                	c.lwsp	s1,36(sp)
2005e762:	6145                	c.addi16sp	sp,48
2005e764:	8082                	c.jr	ra
2005e766:	475c                	c.lw	a5,12(a4)
2005e768:	47c0                	c.lw	s0,12(a5)
2005e76a:	4390                	c.lw	a2,0(a5)
2005e76c:	00440593          	addi	a1,s0,4
2005e770:	852e                	c.mv	a0,a1
2005e772:	c632                	c.swsp	a2,12(sp)
2005e774:	c42e                	c.swsp	a1,8(sp)
2005e776:	f2cfc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e77a:	501c                	c.lw	a5,32(s0)
2005e77c:	8522                	c.mv	a0,s0
2005e77e:	9782                	c.jalr	a5
2005e780:	02844783          	lbu	a5,40(s0)
2005e784:	45a2                	c.lwsp	a1,8(sp)
2005e786:	4632                	c.lwsp	a2,12(sp)
2005e788:	8b91                	c.andi	a5,4
2005e78a:	d3dd                	c.beqz	a5,2005e730 <prvSampleTimeNow+0x24>
2005e78c:	4c1c                	c.lw	a5,24(s0)
2005e78e:	97b2                	c.add	a5,a2
2005e790:	00f67963          	bgeu	a2,a5,2005e7a2 <prvSampleTimeNow+0x96>
2005e794:	000a2503          	lw	a0,0(s4)
2005e798:	c05c                	c.sw	a5,4(s0)
2005e79a:	c800                	c.sw	s0,16(s0)
2005e79c:	ed8fc0ef          	jal	ra,2005ae74 <vListInsert>
2005e7a0:	bf41                	c.j	2005e730 <prvSampleTimeNow+0x24>
2005e7a2:	4701                	c.li	a4,0
2005e7a4:	4681                	c.li	a3,0
2005e7a6:	4581                	c.li	a1,0
2005e7a8:	8522                	c.mv	a0,s0
2005e7aa:	e9bff0ef          	jal	ra,2005e644 <xTimerGenericCommand>
2005e7ae:	b749                	c.j	2005e730 <prvSampleTimeNow+0x24>
2005e7b0:	0009a023          	sw	zero,0(s3)
2005e7b4:	bf69                	c.j	2005e74e <prvSampleTimeNow+0x42>

2005e7b6 <prvTimerTask>:
2005e7b6:	7139                	c.addi16sp	sp,-64
2005e7b8:	d84a                	c.swsp	s2,48(sp)
2005e7ba:	20063937          	lui	s2,0x20063
2005e7be:	d64e                	c.swsp	s3,44(sp)
2005e7c0:	d256                	c.swsp	s5,36(sp)
2005e7c2:	d05a                	c.swsp	s6,32(sp)
2005e7c4:	de06                	c.swsp	ra,60(sp)
2005e7c6:	dc22                	c.swsp	s0,56(sp)
2005e7c8:	da26                	c.swsp	s1,52(sp)
2005e7ca:	d452                	c.swsp	s4,40(sp)
2005e7cc:	ce5e                	c.swsp	s7,28(sp)
2005e7ce:	cc62                	c.swsp	s8,24(sp)
2005e7d0:	6c090913          	addi	s2,s2,1728 # 200636c0 <__func__.1+0x38>
2005e7d4:	f981a783          	lw	a5,-104(gp) # 20066948 <pxCurrentTimerList>
2005e7d8:	f9818c13          	addi	s8,gp,-104 # 20066948 <pxCurrentTimerList>
2005e7dc:	4405                	c.li	s0,1
2005e7de:	4384                	c.lw	s1,0(a5)
2005e7e0:	c481                	c.beqz	s1,2005e7e8 <prvTimerTask+0x32>
2005e7e2:	47dc                	c.lw	a5,12(a5)
2005e7e4:	4401                	c.li	s0,0
2005e7e6:	4384                	c.lw	s1,0(a5)
2005e7e8:	ef5fe0ef          	jal	ra,2005d6dc <vTaskSuspendAll>
2005e7ec:	0048                	c.addi4spn	a0,sp,4
2005e7ee:	f1fff0ef          	jal	ra,2005e70c <prvSampleTimeNow>
2005e7f2:	4792                	c.lwsp	a5,4(sp)
2005e7f4:	8baa                	c.mv	s7,a0
2005e7f6:	fa418a13          	addi	s4,gp,-92 # 20066954 <xTimerQueue>
2005e7fa:	ebc5                	c.bnez	a5,2005e8aa <prvTimerTask+0xf4>
2005e7fc:	e449                	c.bnez	s0,2005e886 <prvTimerTask+0xd0>
2005e7fe:	08956963          	bltu	a0,s1,2005e890 <prvTimerTask+0xda>
2005e802:	fa2ff0ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005e806:	000c2783          	lw	a5,0(s8)
2005e80a:	47dc                	c.lw	a5,12(a5)
2005e80c:	47c0                	c.lw	s0,12(a5)
2005e80e:	00440513          	addi	a0,s0,4
2005e812:	e90fc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e816:	02844783          	lbu	a5,40(s0)
2005e81a:	0047f713          	andi	a4,a5,4
2005e81e:	c325                	c.beqz	a4,2005e87e <prvTimerTask+0xc8>
2005e820:	4c0c                	c.lw	a1,24(s0)
2005e822:	86a6                	c.mv	a3,s1
2005e824:	865e                	c.mv	a2,s7
2005e826:	95a6                	c.add	a1,s1
2005e828:	8522                	c.mv	a0,s0
2005e82a:	cc7ff0ef          	jal	ra,2005e4f0 <prvInsertTimerInActiveList>
2005e82e:	c901                	c.beqz	a0,2005e83e <prvTimerTask+0x88>
2005e830:	4701                	c.li	a4,0
2005e832:	4681                	c.li	a3,0
2005e834:	8626                	c.mv	a2,s1
2005e836:	4581                	c.li	a1,0
2005e838:	8522                	c.mv	a0,s0
2005e83a:	e0bff0ef          	jal	ra,2005e644 <xTimerGenericCommand>
2005e83e:	501c                	c.lw	a5,32(s0)
2005e840:	8522                	c.mv	a0,s0
2005e842:	9782                	c.jalr	a5
2005e844:	44a5                	c.li	s1,9
2005e846:	000a2503          	lw	a0,0(s4)
2005e84a:	4601                	c.li	a2,0
2005e84c:	004c                	c.addi4spn	a1,sp,4
2005e84e:	a34fe0ef          	jal	ra,2005ca82 <xQueueReceive>
2005e852:	d149                	c.beqz	a0,2005e7d4 <prvTimerTask+0x1e>
2005e854:	4792                	c.lwsp	a5,4(sp)
2005e856:	fe07c8e3          	blt	a5,zero,2005e846 <prvTimerTask+0x90>
2005e85a:	4432                	c.lwsp	s0,12(sp)
2005e85c:	485c                	c.lw	a5,20(s0)
2005e85e:	c789                	c.beqz	a5,2005e868 <prvTimerTask+0xb2>
2005e860:	00440513          	addi	a0,s0,4
2005e864:	e3efc0ef          	jal	ra,2005aea2 <uxListRemove>
2005e868:	850a                	c.mv	a0,sp
2005e86a:	ea3ff0ef          	jal	ra,2005e70c <prvSampleTimeNow>
2005e86e:	4792                	c.lwsp	a5,4(sp)
2005e870:	862a                	c.mv	a2,a0
2005e872:	fcf4eae3          	bltu	s1,a5,2005e846 <prvTimerTask+0x90>
2005e876:	078a                	c.slli	a5,0x2
2005e878:	97ca                	c.add	a5,s2
2005e87a:	439c                	c.lw	a5,0(a5)
2005e87c:	8782                	c.jr	a5
2005e87e:	9bf9                	c.andi	a5,-2
2005e880:	02f40423          	sb	a5,40(s0)
2005e884:	bf6d                	c.j	2005e83e <prvTimerTask+0x88>
2005e886:	f9c1a783          	lw	a5,-100(gp) # 2006694c <pxOverflowTimerList>
2005e88a:	4380                	c.lw	s0,0(a5)
2005e88c:	00143413          	sltiu	s0,s0,1
2005e890:	000a2503          	lw	a0,0(s4)
2005e894:	8622                	c.mv	a2,s0
2005e896:	417485b3          	sub	a1,s1,s7
2005e89a:	c60fe0ef          	jal	ra,2005ccfa <vQueueWaitForMessageRestricted>
2005e89e:	f06ff0ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005e8a2:	f14d                	c.bnez	a0,2005e844 <prvTimerTask+0x8e>
2005e8a4:	00000073          	ecall
2005e8a8:	bf71                	c.j	2005e844 <prvTimerTask+0x8e>
2005e8aa:	efaff0ef          	jal	ra,2005dfa4 <xTaskResumeAll>
2005e8ae:	bf59                	c.j	2005e844 <prvTimerTask+0x8e>
2005e8b0:	02844783          	lbu	a5,40(s0)
2005e8b4:	4c0c                	c.lw	a1,24(s0)
2005e8b6:	8522                	c.mv	a0,s0
2005e8b8:	0017e793          	ori	a5,a5,1
2005e8bc:	02f40423          	sb	a5,40(s0)
2005e8c0:	46a2                	c.lwsp	a3,8(sp)
2005e8c2:	95b6                	c.add	a1,a3
2005e8c4:	c2dff0ef          	jal	ra,2005e4f0 <prvInsertTimerInActiveList>
2005e8c8:	dd3d                	c.beqz	a0,2005e846 <prvTimerTask+0x90>
2005e8ca:	501c                	c.lw	a5,32(s0)
2005e8cc:	8522                	c.mv	a0,s0
2005e8ce:	9782                	c.jalr	a5
2005e8d0:	02844783          	lbu	a5,40(s0)
2005e8d4:	8b91                	c.andi	a5,4
2005e8d6:	dba5                	c.beqz	a5,2005e846 <prvTimerTask+0x90>
2005e8d8:	4c1c                	c.lw	a5,24(s0)
2005e8da:	4622                	c.lwsp	a2,8(sp)
2005e8dc:	4701                	c.li	a4,0
2005e8de:	4681                	c.li	a3,0
2005e8e0:	963e                	c.add	a2,a5
2005e8e2:	4581                	c.li	a1,0
2005e8e4:	8522                	c.mv	a0,s0
2005e8e6:	d5fff0ef          	jal	ra,2005e644 <xTimerGenericCommand>
2005e8ea:	bfb1                	c.j	2005e846 <prvTimerTask+0x90>
2005e8ec:	02844783          	lbu	a5,40(s0)
2005e8f0:	9bf9                	c.andi	a5,-2
2005e8f2:	02f40423          	sb	a5,40(s0)
2005e8f6:	bf81                	c.j	2005e846 <prvTimerTask+0x90>
2005e8f8:	02844783          	lbu	a5,40(s0)
2005e8fc:	86aa                	c.mv	a3,a0
2005e8fe:	0017e793          	ori	a5,a5,1
2005e902:	02f40423          	sb	a5,40(s0)
2005e906:	45a2                	c.lwsp	a1,8(sp)
2005e908:	cc0c                	c.sw	a1,24(s0)
2005e90a:	95aa                	c.add	a1,a0
2005e90c:	8522                	c.mv	a0,s0
2005e90e:	be3ff0ef          	jal	ra,2005e4f0 <prvInsertTimerInActiveList>
2005e912:	bf15                	c.j	2005e846 <prvTimerTask+0x90>
2005e914:	02844783          	lbu	a5,40(s0)
2005e918:	0027f713          	andi	a4,a5,2
2005e91c:	fb71                	c.bnez	a4,2005e8f0 <prvTimerTask+0x13a>
2005e91e:	8522                	c.mv	a0,s0
2005e920:	bc8fc0ef          	jal	ra,2005ace8 <vPortFree>
2005e924:	b70d                	c.j	2005e846 <prvTimerTask+0x90>

2005e926 <xTimerIsTimerActive>:
2005e926:	1101                	c.addi	sp,-32
2005e928:	ce06                	c.swsp	ra,28(sp)
2005e92a:	cc22                	c.swsp	s0,24(sp)
2005e92c:	842a                	c.mv	s0,a0
2005e92e:	aeeff0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005e932:	02844503          	lbu	a0,40(s0)
2005e936:	8905                	c.andi	a0,1
2005e938:	c62a                	c.swsp	a0,12(sp)
2005e93a:	afcff0ef          	jal	ra,2005dc36 <vTaskExitCritical>
2005e93e:	40f2                	c.lwsp	ra,28(sp)
2005e940:	4462                	c.lwsp	s0,24(sp)
2005e942:	4532                	c.lwsp	a0,12(sp)
2005e944:	6105                	c.addi16sp	sp,32
2005e946:	8082                	c.jr	ra

2005e948 <wifi_fast_connect_enable>:
2005e948:	fa01ac23          	sw	zero,-72(gp) # 20066968 <p_wifi_do_fast_connect>
2005e94c:	fa01aa23          	sw	zero,-76(gp) # 20066964 <p_store_fast_connect_info>
2005e950:	8082                	c.jr	ra

2005e952 <wifi_event_handle>:
2005e952:	1101                	c.addi	sp,-32
2005e954:	cc22                	c.swsp	s0,24(sp)
2005e956:	842a                	c.mv	s0,a0
2005e958:	ce06                	c.swsp	ra,28(sp)
2005e95a:	852e                	c.mv	a0,a1
2005e95c:	ca26                	c.swsp	s1,20(sp)
2005e95e:	85b2                	c.mv	a1,a2
2005e960:	05c00713          	addi	a4,zero,92
2005e964:	8636                	c.mv	a2,a3
2005e966:	ff840693          	addi	a3,s0,-8
2005e96a:	00d77663          	bgeu	a4,a3,2005e976 <wifi_event_handle+0x24>
2005e96e:	07600713          	addi	a4,zero,118
2005e972:	00877f63          	bgeu	a4,s0,2005e990 <wifi_event_handle+0x3e>
2005e976:	20063537          	lui	a0,0x20063
2005e97a:	85a2                	c.mv	a1,s0
2005e97c:	6e850513          	addi	a0,a0,1768 # 200636e8 <__func__.1+0x60>
2005e980:	283000ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005e984:	5579                	c.li	a0,-2
2005e986:	40f2                	c.lwsp	ra,28(sp)
2005e988:	4462                	c.lwsp	s0,24(sp)
2005e98a:	44d2                	c.lwsp	s1,20(sp)
2005e98c:	6105                	c.addi16sp	sp,32
2005e98e:	8082                	c.jr	ra
2005e990:	471d                	c.li	a4,7
2005e992:	00877463          	bgeu	a4,s0,2005e99a <wifi_event_handle+0x48>
2005e996:	4501                	c.li	a0,0
2005e998:	b7fd                	c.j	2005e986 <wifi_event_handle+0x34>
2005e99a:	20068737          	lui	a4,0x20068
2005e99e:	52070693          	addi	a3,a4,1312 # 20068520 <event_callback_list>
2005e9a2:	0412                	c.slli	s0,0x4
2005e9a4:	008687b3          	add	a5,a3,s0
2005e9a8:	0007a803          	lw	a6,0(a5)
2005e9ac:	52070493          	addi	s1,a4,1312
2005e9b0:	00080a63          	beq	a6,zero,2005e9c4 <wifi_event_handle+0x72>
2005e9b4:	43d4                	c.lw	a3,4(a5)
2005e9b6:	c632                	c.swsp	a2,12(sp)
2005e9b8:	c42e                	c.swsp	a1,8(sp)
2005e9ba:	c22a                	c.swsp	a0,4(sp)
2005e9bc:	9802                	c.jalr	a6
2005e9be:	4632                	c.lwsp	a2,12(sp)
2005e9c0:	45a2                	c.lwsp	a1,8(sp)
2005e9c2:	4512                	c.lwsp	a0,4(sp)
2005e9c4:	008487b3          	add	a5,s1,s0
2005e9c8:	4798                	c.lw	a4,8(a5)
2005e9ca:	d771                	c.beqz	a4,2005e996 <wifi_event_handle+0x44>
2005e9cc:	47d4                	c.lw	a3,12(a5)
2005e9ce:	9702                	c.jalr	a4
2005e9d0:	b7d9                	c.j	2005e996 <wifi_event_handle+0x44>

2005e9d2 <wifi_event_init>:
2005e9d2:	20068537          	lui	a0,0x20068
2005e9d6:	08000613          	addi	a2,zero,128
2005e9da:	4581                	c.li	a1,0
2005e9dc:	52050513          	addi	a0,a0,1312 # 20068520 <event_callback_list>
2005e9e0:	dffab317          	auipc	t1,0xdffab
2005e9e4:	0ee30067          	jalr	zero,238(t1) # 9ace <__wrap_memset>

2005e9e8 <wifi_indication>:
2005e9e8:	f6bff06f          	jal	zero,2005e952 <wifi_event_handle>

2005e9ec <_init_thread>:
2005e9ec:	1141                	c.addi	sp,-16
2005e9ee:	c422                	c.swsp	s0,8(sp)
2005e9f0:	c606                	c.swsp	ra,12(sp)
2005e9f2:	c226                	c.swsp	s1,4(sp)
2005e9f4:	4100c437          	lui	s0,0x4100c
2005e9f8:	08442783          	lw	a5,132(s0) # 4100c084 <__km4_bd_boot_download_addr__+0x10ffa084>
2005e9fc:	08440713          	addi	a4,s0,132
2005ea00:	8b85                	c.andi	a5,1
2005ea02:	cfb5                	c.beqz	a5,2005ea7e <_init_thread+0x92>
2005ea04:	431c                	c.lw	a5,0(a4)
2005ea06:	4505                	c.li	a0,1
2005ea08:	9bf9                	c.andi	a5,-2
2005ea0a:	c31c                	c.sw	a5,0(a4)
2005ea0c:	1b5010ef          	jal	ra,200603c0 <wifi_on>
2005ea10:	fac18493          	addi	s1,gp,-84 # 2006695c <heap_tmp>
2005ea14:	4080                	c.lw	s0,0(s1)
2005ea16:	cf9fc0ef          	jal	ra,2005b70e <rtos_mem_get_free_heap_size>
2005ea1a:	7779                	c.lui	a4,0xffffe
2005ea1c:	b0870713          	addi	a4,a4,-1272 # ffffdb08 <__ctrace_end__+0x35ffdb08>
2005ea20:	9722                	c.add	a4,s0
2005ea22:	200636b7          	lui	a3,0x20063
2005ea26:	20063437          	lui	s0,0x20063
2005ea2a:	8f09                	c.sub	a4,a0
2005ea2c:	71040593          	addi	a1,s0,1808 # 20063710 <__func__.1+0x88>
2005ea30:	6fc68693          	addi	a3,a3,1788 # 200636fc <__func__.1+0x74>
2005ea34:	04900613          	addi	a2,zero,73
2005ea38:	4511                	c.li	a0,4
2005ea3a:	c098                	c.sw	a4,0(s1)
2005ea3c:	f3afc0ef          	jal	ra,2005b176 <rtk_log_write>
2005ea40:	ccffc0ef          	jal	ra,2005b70e <rtos_mem_get_free_heap_size>
2005ea44:	6785                	c.lui	a5,0x1
2005ea46:	40078793          	addi	a5,a5,1024 # 1400 <CPU_ClkGet+0x8ea>
2005ea4a:	20063737          	lui	a4,0x20063
2005ea4e:	200636b7          	lui	a3,0x20063
2005ea52:	00f50833          	add	a6,a0,a5
2005ea56:	71040593          	addi	a1,s0,1808
2005ea5a:	4511                	c.li	a0,4
2005ea5c:	04a00793          	addi	a5,zero,74
2005ea60:	75470713          	addi	a4,a4,1876 # 20063754 <__FUNCTION__.0>
2005ea64:	71868693          	addi	a3,a3,1816 # 20063718 <__func__.1+0x90>
2005ea68:	04900613          	addi	a2,zero,73
2005ea6c:	f0afc0ef          	jal	ra,2005b176 <rtk_log_write>
2005ea70:	4422                	c.lwsp	s0,8(sp)
2005ea72:	40b2                	c.lwsp	ra,12(sp)
2005ea74:	4492                	c.lwsp	s1,4(sp)
2005ea76:	4501                	c.li	a0,0
2005ea78:	0141                	c.addi	sp,16
2005ea7a:	954fd06f          	jal	zero,2005bbce <rtos_task_delete>
2005ea7e:	4505                	c.li	a0,1
2005ea80:	960fd0ef          	jal	ra,2005bbe0 <rtos_time_delay_ms>
2005ea84:	bf95                	c.j	2005e9f8 <_init_thread+0xc>

2005ea86 <wlan_initialize>:
2005ea86:	1141                	c.addi	sp,-16
2005ea88:	c606                	c.swsp	ra,12(sp)
2005ea8a:	c85fc0ef          	jal	ra,2005b70e <rtos_mem_get_free_heap_size>
2005ea8e:	faa1a623          	sw	a0,-84(gp) # 2006695c <heap_tmp>
2005ea92:	041010ef          	jal	ra,200602d2 <wifi_set_rom2flash>
2005ea96:	118020ef          	jal	ra,20060bae <inic_host_init>
2005ea9a:	4505                	c.li	a0,1
2005ea9c:	eadff0ef          	jal	ra,2005e948 <wifi_fast_connect_enable>
2005eaa0:	6705                	c.lui	a4,0x1
2005eaa2:	2005f637          	lui	a2,0x2005f
2005eaa6:	200635b7          	lui	a1,0x20063
2005eaaa:	4789                	c.li	a5,2
2005eaac:	40070713          	addi	a4,a4,1024 # 1400 <CPU_ClkGet+0x8ea>
2005eab0:	4681                	c.li	a3,0
2005eab2:	9ec60613          	addi	a2,a2,-1556 # 2005e9ec <_init_thread>
2005eab6:	73458593          	addi	a1,a1,1844 # 20063734 <__func__.1+0xac>
2005eaba:	4501                	c.li	a0,0
2005eabc:	8eefd0ef          	jal	ra,2005bbaa <rtos_task_create>
2005eac0:	c105                	c.beqz	a0,2005eae0 <wlan_initialize+0x5a>
2005eac2:	40b2                	c.lwsp	ra,12(sp)
2005eac4:	200636b7          	lui	a3,0x20063
2005eac8:	200635b7          	lui	a1,0x20063
2005eacc:	73c68693          	addi	a3,a3,1852 # 2006373c <__func__.1+0xb4>
2005ead0:	04500613          	addi	a2,zero,69
2005ead4:	71058593          	addi	a1,a1,1808 # 20063710 <__func__.1+0x88>
2005ead8:	4509                	c.li	a0,2
2005eada:	0141                	c.addi	sp,16
2005eadc:	e9afc06f          	jal	zero,2005b176 <rtk_log_write>
2005eae0:	40b2                	c.lwsp	ra,12(sp)
2005eae2:	0141                	c.addi	sp,16
2005eae4:	8082                	c.jr	ra

2005eae6 <ChipInfo_GetSocName_ToBuf>:
2005eae6:	7139                	c.addi16sp	sp,-64
2005eae8:	da26                	c.swsp	s1,52(sp)
2005eaea:	84ae                	c.mv	s1,a1
2005eaec:	200645b7          	lui	a1,0x20064
2005eaf0:	d84a                	c.swsp	s2,48(sp)
2005eaf2:	4625                	c.li	a2,9
2005eaf4:	892a                	c.mv	s2,a0
2005eaf6:	86358593          	addi	a1,a1,-1949 # 20063863 <__FUNCTION__.0+0x10f>
2005eafa:	0068                	c.addi4spn	a0,sp,12
2005eafc:	dc22                	c.swsp	s0,56(sp)
2005eafe:	de06                	c.swsp	ra,60(sp)
2005eb00:	d64e                	c.swsp	s3,44(sp)
2005eb02:	d452                	c.swsp	s4,40(sp)
2005eb04:	dffab097          	auipc	ra,0xdffab
2005eb08:	fcc080e7          	jalr	ra,-52(ra) # 9ad0 <__wrap_memcpy>
2005eb0c:	00010aa3          	sb	zero,21(sp)
2005eb10:	4401                	c.li	s0,0
2005eb12:	c899                	c.beqz	s1,2005eb28 <ChipInfo_GetSocName_ToBuf+0x42>
2005eb14:	20064637          	lui	a2,0x20064
2005eb18:	0074                	c.addi4spn	a3,sp,12
2005eb1a:	89460613          	addi	a2,a2,-1900 # 20063894 <__FUNCTION__.0+0x140>
2005eb1e:	85a6                	c.mv	a1,s1
2005eb20:	854a                	c.mv	a0,s2
2005eb22:	29f000ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005eb26:	842a                	c.mv	s0,a0
2005eb28:	7f000993          	addi	s3,zero,2032
2005eb2c:	20064a37          	lui	s4,0x20064
2005eb30:	00b10593          	addi	a1,sp,11
2005eb34:	854e                	c.mv	a0,s3
2005eb36:	951fa0ef          	jal	ra,20059486 <OTP_Read8>
2005eb3a:	e131                	c.bnez	a0,2005eb7e <ChipInfo_GetSocName_ToBuf+0x98>
2005eb3c:	200646b7          	lui	a3,0x20064
2005eb40:	200635b7          	lui	a1,0x20063
2005eb44:	89c68693          	addi	a3,a3,-1892 # 2006389c <__FUNCTION__.0+0x148>
2005eb48:	04500613          	addi	a2,zero,69
2005eb4c:	7a858593          	addi	a1,a1,1960 # 200637a8 <__FUNCTION__.0+0x54>
2005eb50:	4509                	c.li	a0,2
2005eb52:	e24fc0ef          	jal	ra,2005b176 <rtk_log_write>
2005eb56:	00947c63          	bgeu	s0,s1,2005eb6e <ChipInfo_GetSocName_ToBuf+0x88>
2005eb5a:	20063637          	lui	a2,0x20063
2005eb5e:	87860613          	addi	a2,a2,-1928 # 20062878 <pmap_func+0x4ac>
2005eb62:	408485b3          	sub	a1,s1,s0
2005eb66:	00890533          	add	a0,s2,s0
2005eb6a:	257000ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005eb6e:	50f2                	c.lwsp	ra,60(sp)
2005eb70:	5462                	c.lwsp	s0,56(sp)
2005eb72:	54d2                	c.lwsp	s1,52(sp)
2005eb74:	5942                	c.lwsp	s2,48(sp)
2005eb76:	59b2                	c.lwsp	s3,44(sp)
2005eb78:	5a22                	c.lwsp	s4,40(sp)
2005eb7a:	6121                	c.addi16sp	sp,64
2005eb7c:	8082                	c.jr	ra
2005eb7e:	00947d63          	bgeu	s0,s1,2005eb98 <ChipInfo_GetSocName_ToBuf+0xb2>
2005eb82:	00b14683          	lbu	a3,11(sp)
2005eb86:	408485b3          	sub	a1,s1,s0
2005eb8a:	00890533          	add	a0,s2,s0
2005eb8e:	8b4a0613          	addi	a2,s4,-1868 # 200638b4 <__FUNCTION__.0+0x160>
2005eb92:	22f000ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005eb96:	942a                	c.add	s0,a0
2005eb98:	0985                	c.addi	s3,1
2005eb9a:	80098793          	addi	a5,s3,-2048
2005eb9e:	fbc9                	c.bnez	a5,2005eb30 <ChipInfo_GetSocName_ToBuf+0x4a>
2005eba0:	bf5d                	c.j	2005eb56 <ChipInfo_GetSocName_ToBuf+0x70>

2005eba2 <ChipInfo_GetLibVersion_ToBuf>:
2005eba2:	1101                	c.addi	sp,-32
2005eba4:	cc22                	c.swsp	s0,24(sp)
2005eba6:	ca26                	c.swsp	s1,20(sp)
2005eba8:	c64e                	c.swsp	s3,12(sp)
2005ebaa:	ce06                	c.swsp	ra,28(sp)
2005ebac:	c84a                	c.swsp	s2,16(sp)
2005ebae:	c452                	c.swsp	s4,8(sp)
2005ebb0:	c256                	c.swsp	s5,4(sp)
2005ebb2:	c05a                	c.swsp	s6,0(sp)
2005ebb4:	4100c7b7          	lui	a5,0x4100c
2005ebb8:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005ebbc:	89aa                	c.mv	s3,a0
2005ebbe:	84ae                	c.mv	s1,a1
2005ebc0:	4401                	c.li	s0,0
2005ebc2:	c1b5                	c.beqz	a1,2005ec26 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005ebc4:	8399                	c.srli	a5,0x6
2005ebc6:	8b8d                	c.andi	a5,3
2005ebc8:	e7d9                	c.bnez	a5,2005ec56 <ChipInfo_GetLibVersion_ToBuf+0xb4>
2005ebca:	200636b7          	lui	a3,0x20063
2005ebce:	4d068693          	addi	a3,a3,1232 # 200634d0 <__FUNCTION__.0+0x1c>
2005ebd2:	20064637          	lui	a2,0x20064
2005ebd6:	8c060613          	addi	a2,a2,-1856 # 200638c0 <__FUNCTION__.0+0x16c>
2005ebda:	85a6                	c.mv	a1,s1
2005ebdc:	854e                	c.mv	a0,s3
2005ebde:	1e3000ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005ebe2:	842a                	c.mv	s0,a0
2005ebe4:	04957163          	bgeu	a0,s1,2005ec26 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005ebe8:	200646b7          	lui	a3,0x20064
2005ebec:	20064637          	lui	a2,0x20064
2005ebf0:	40a485b3          	sub	a1,s1,a0
2005ebf4:	8d468693          	addi	a3,a3,-1836 # 200638d4 <__FUNCTION__.0+0x180>
2005ebf8:	8e860613          	addi	a2,a2,-1816 # 200638e8 <__FUNCTION__.0+0x194>
2005ebfc:	954e                	c.add	a0,s3
2005ebfe:	1c3000ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005ec02:	942a                	c.add	s0,a0
2005ec04:	02947163          	bgeu	s0,s1,2005ec26 <ChipInfo_GetLibVersion_ToBuf+0x84>
2005ec08:	200646b7          	lui	a3,0x20064
2005ec0c:	20064637          	lui	a2,0x20064
2005ec10:	408485b3          	sub	a1,s1,s0
2005ec14:	00898533          	add	a0,s3,s0
2005ec18:	8fc68693          	addi	a3,a3,-1796 # 200638fc <__FUNCTION__.0+0x1a8>
2005ec1c:	93c60613          	addi	a2,a2,-1732 # 2006393c <__FUNCTION__.0+0x1e8>
2005ec20:	1a1000ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005ec24:	942a                	c.add	s0,a0
2005ec26:	20065937          	lui	s2,0x20065
2005ec2a:	20065a37          	lui	s4,0x20065
2005ec2e:	d6c90913          	addi	s2,s2,-660 # 20064d6c <lib_pmc_git_rev>
2005ec32:	dcda0a13          	addi	s4,s4,-563 # 20064dcd <__git_ver_table_end__>
2005ec36:	20064ab7          	lui	s5,0x20064
2005ec3a:	20063b37          	lui	s6,0x20063
2005ec3e:	03496163          	bltu	s2,s4,2005ec60 <ChipInfo_GetLibVersion_ToBuf+0xbe>
2005ec42:	40f2                	c.lwsp	ra,28(sp)
2005ec44:	4462                	c.lwsp	s0,24(sp)
2005ec46:	44d2                	c.lwsp	s1,20(sp)
2005ec48:	4942                	c.lwsp	s2,16(sp)
2005ec4a:	49b2                	c.lwsp	s3,12(sp)
2005ec4c:	4a22                	c.lwsp	s4,8(sp)
2005ec4e:	4a92                	c.lwsp	s5,4(sp)
2005ec50:	4b02                	c.lwsp	s6,0(sp)
2005ec52:	6105                	c.addi16sp	sp,32
2005ec54:	8082                	c.jr	ra
2005ec56:	200646b7          	lui	a3,0x20064
2005ec5a:	8bc68693          	addi	a3,a3,-1860 # 200638bc <__FUNCTION__.0+0x168>
2005ec5e:	bf95                	c.j	2005ebd2 <ChipInfo_GetLibVersion_ToBuf+0x30>
2005ec60:	00094683          	lbu	a3,0(s2)
2005ec64:	e28d                	c.bnez	a3,2005ec86 <ChipInfo_GetLibVersion_ToBuf+0xe4>
2005ec66:	00194783          	lbu	a5,1(s2)
2005ec6a:	0905                	c.addi	s2,1
2005ec6c:	dfed                	c.beqz	a5,2005ec66 <ChipInfo_GetLibVersion_ToBuf+0xc4>
2005ec6e:	fc9478e3          	bgeu	s0,s1,2005ec3e <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ec72:	878b0613          	addi	a2,s6,-1928 # 20062878 <pmap_func+0x4ac>
2005ec76:	408485b3          	sub	a1,s1,s0
2005ec7a:	00898533          	add	a0,s3,s0
2005ec7e:	143000ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005ec82:	942a                	c.add	s0,a0
2005ec84:	bf6d                	c.j	2005ec3e <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ec86:	fa947ce3          	bgeu	s0,s1,2005ec3e <ChipInfo_GetLibVersion_ToBuf+0x9c>
2005ec8a:	94ca8613          	addi	a2,s5,-1716 # 2006394c <__FUNCTION__.0+0x1f8>
2005ec8e:	408485b3          	sub	a1,s1,s0
2005ec92:	00898533          	add	a0,s3,s0
2005ec96:	0905                	c.addi	s2,1
2005ec98:	129000ef          	jal	ra,2005f5c0 <DiagSnPrintf>
2005ec9c:	b7dd                	c.j	2005ec82 <ChipInfo_GetLibVersion_ToBuf+0xe0>

2005ec9e <ChipInfo_GetChipSram>:
2005ec9e:	1101                	c.addi	sp,-32
2005eca0:	ce06                	c.swsp	ra,28(sp)
2005eca2:	4100c7b7          	lui	a5,0x4100c
2005eca6:	2107a783          	lw	a5,528(a5) # 4100c210 <__km4_bd_boot_download_addr__+0x10ffa210>
2005ecaa:	40400737          	lui	a4,0x40400
2005ecae:	8ff9                	c.and	a5,a4
2005ecb0:	c39d                	c.beqz	a5,2005ecd6 <ChipInfo_GetChipSram+0x38>
2005ecb2:	00f10593          	addi	a1,sp,15
2005ecb6:	7fc00513          	addi	a0,zero,2044
2005ecba:	fccfa0ef          	jal	ra,20059486 <OTP_Read8>
2005ecbe:	00f14783          	lbu	a5,15(sp)
2005ecc2:	470d                	c.li	a4,3
2005ecc4:	8389                	c.srli	a5,0x2
2005ecc6:	8b8d                	c.andi	a5,3
2005ecc8:	00e79663          	bne	a5,a4,2005ecd4 <ChipInfo_GetChipSram+0x36>
2005eccc:	4501                	c.li	a0,0
2005ecce:	40f2                	c.lwsp	ra,28(sp)
2005ecd0:	6105                	c.addi16sp	sp,32
2005ecd2:	8082                	c.jr	ra
2005ecd4:	dfe5                	c.beqz	a5,2005eccc <ChipInfo_GetChipSram+0x2e>
2005ecd6:	00f10593          	addi	a1,sp,15
2005ecda:	7fc00513          	addi	a0,zero,2044
2005ecde:	fa8fa0ef          	jal	ra,20059486 <OTP_Read8>
2005ece2:	00f14783          	lbu	a5,15(sp)
2005ece6:	470d                	c.li	a4,3
2005ece8:	8399                	c.srli	a5,0x6
2005ecea:	00e78363          	beq	a5,a4,2005ecf0 <ChipInfo_GetChipSram+0x52>
2005ecee:	fff9                	c.bnez	a5,2005eccc <ChipInfo_GetChipSram+0x2e>
2005ecf0:	4505                	c.li	a0,1
2005ecf2:	bff1                	c.j	2005ecce <ChipInfo_GetChipSram+0x30>

2005ecf4 <vTaskStatus>:
2005ecf4:	7139                	c.addi16sp	sp,-64
2005ecf6:	de06                	c.swsp	ra,60(sp)
2005ecf8:	dc22                	c.swsp	s0,56(sp)
2005ecfa:	da26                	c.swsp	s1,52(sp)
2005ecfc:	f21fe0ef          	jal	ra,2005dc1c <vTaskEnterCritical>
2005ed00:	d35fe0ef          	jal	ra,2005da34 <xTaskGetCurrentTaskHandle>
2005ed04:	4695                	c.li	a3,5
2005ed06:	4605                	c.li	a2,1
2005ed08:	006c                	c.addi4spn	a1,sp,12
2005ed0a:	842a                	c.mv	s0,a0
2005ed0c:	200644b7          	lui	s1,0x20064
2005ed10:	c34ff0ef          	jal	ra,2005e144 <vTaskGetInfo>
2005ed14:	95048513          	addi	a0,s1,-1712 # 20063950 <__FUNCTION__.0+0x1fc>
2005ed18:	25e9                	c.jal	2005f3e2 <DiagPrintf>
2005ed1a:	45c2                	c.lwsp	a1,16(sp)
2005ed1c:	20064537          	lui	a0,0x20064
2005ed20:	97450513          	addi	a0,a0,-1676 # 20063974 <__FUNCTION__.0+0x220>
2005ed24:	2d7d                	c.jal	2005f3e2 <DiagPrintf>
2005ed26:	45d2                	c.lwsp	a1,20(sp)
2005ed28:	20064537          	lui	a0,0x20064
2005ed2c:	98850513          	addi	a0,a0,-1656 # 20063988 <__FUNCTION__.0+0x234>
2005ed30:	2d4d                	c.jal	2005f3e2 <DiagPrintf>
2005ed32:	20064537          	lui	a0,0x20064
2005ed36:	85a2                	c.mv	a1,s0
2005ed38:	99850513          	addi	a0,a0,-1640 # 20063998 <__FUNCTION__.0+0x244>
2005ed3c:	255d                	c.jal	2005f3e2 <DiagPrintf>
2005ed3e:	01814583          	lbu	a1,24(sp)
2005ed42:	200647b7          	lui	a5,0x20064
2005ed46:	abc78793          	addi	a5,a5,-1348 # 20063abc <TaskStateString.0>
2005ed4a:	00259713          	slli	a4,a1,0x2
2005ed4e:	97ba                	c.add	a5,a4
2005ed50:	4390                	c.lw	a2,0(a5)
2005ed52:	20064537          	lui	a0,0x20064
2005ed56:	9ac50513          	addi	a0,a0,-1620 # 200639ac <__FUNCTION__.0+0x258>
2005ed5a:	2561                	c.jal	2005f3e2 <DiagPrintf>
2005ed5c:	5582                	c.lwsp	a1,32(sp)
2005ed5e:	20064537          	lui	a0,0x20064
2005ed62:	9c850513          	addi	a0,a0,-1592 # 200639c8 <__FUNCTION__.0+0x274>
2005ed66:	2db5                	c.jal	2005f3e2 <DiagPrintf>
2005ed68:	45f2                	c.lwsp	a1,28(sp)
2005ed6a:	20064537          	lui	a0,0x20064
2005ed6e:	9dc50513          	addi	a0,a0,-1572 # 200639dc <__FUNCTION__.0+0x288>
2005ed72:	2d85                	c.jal	2005f3e2 <DiagPrintf>
2005ed74:	5592                	c.lwsp	a1,36(sp)
2005ed76:	20064537          	lui	a0,0x20064
2005ed7a:	9f450513          	addi	a0,a0,-1548 # 200639f4 <__FUNCTION__.0+0x2a0>
2005ed7e:	2595                	c.jal	2005f3e2 <DiagPrintf>
2005ed80:	4474                	c.lw	a3,76(s0)
2005ed82:	5818                	c.lw	a4,48(s0)
2005ed84:	5622                	c.lwsp	a2,40(sp)
2005ed86:	400c                	c.lw	a1,0(s0)
2005ed88:	40e68733          	sub	a4,a3,a4
2005ed8c:	8709                	c.srai	a4,0x2
2005ed8e:	20064537          	lui	a0,0x20064
2005ed92:	0705                	c.addi	a4,1
2005ed94:	a0c50513          	addi	a0,a0,-1524 # 20063a0c <__FUNCTION__.0+0x2b8>
2005ed98:	25a9                	c.jal	2005f3e2 <DiagPrintf>
2005ed9a:	02c15583          	lhu	a1,44(sp)
2005ed9e:	20064537          	lui	a0,0x20064
2005eda2:	a5c50513          	addi	a0,a0,-1444 # 20063a5c <__FUNCTION__.0+0x308>
2005eda6:	2d35                	c.jal	2005f3e2 <DiagPrintf>
2005eda8:	95048513          	addi	a0,s1,-1712
2005edac:	2d1d                	c.jal	2005f3e2 <DiagPrintf>
2005edae:	50f2                	c.lwsp	ra,60(sp)
2005edb0:	5462                	c.lwsp	s0,56(sp)
2005edb2:	54d2                	c.lwsp	s1,52(sp)
2005edb4:	6121                	c.addi16sp	sp,64
2005edb6:	8082                	c.jr	ra

2005edb8 <vTaskCrashCallback>:
2005edb8:	1141                	c.addi	sp,-16
2005edba:	c606                	c.swsp	ra,12(sp)
2005edbc:	dc9fc0ef          	jal	ra,2005bb84 <rtos_sched_get_state>
2005edc0:	4785                	c.li	a5,1
2005edc2:	00f50663          	beq	a0,a5,2005edce <vTaskCrashCallback+0x16>
2005edc6:	40b2                	c.lwsp	ra,12(sp)
2005edc8:	0141                	c.addi	sp,16
2005edca:	f2bff06f          	jal	zero,2005ecf4 <vTaskStatus>
2005edce:	40b2                	c.lwsp	ra,12(sp)
2005edd0:	0141                	c.addi	sp,16
2005edd2:	8082                	c.jr	ra

2005edd4 <print_unsigned_num>:
2005edd4:	715d                	c.addi16sp	sp,-80
2005edd6:	c4a2                	c.swsp	s0,72(sp)
2005edd8:	c2a6                	c.swsp	s1,68(sp)
2005edda:	c0ca                	c.swsp	s2,64(sp)
2005eddc:	de4e                	c.swsp	s3,60(sp)
2005edde:	dc52                	c.swsp	s4,56(sp)
2005ede0:	da56                	c.swsp	s5,52(sp)
2005ede2:	d85a                	c.swsp	s6,48(sp)
2005ede4:	d65e                	c.swsp	s7,44(sp)
2005ede6:	d462                	c.swsp	s8,40(sp)
2005ede8:	d266                	c.swsp	s9,36(sp)
2005edea:	d06a                	c.swsp	s10,32(sp)
2005edec:	c686                	c.swsp	ra,76(sp)
2005edee:	8c2a                	c.mv	s8,a0
2005edf0:	8cae                	c.mv	s9,a1
2005edf2:	8d32                	c.mv	s10,a2
2005edf4:	8936                	c.mv	s2,a3
2005edf6:	8bba                	c.mv	s7,a4
2005edf8:	89be                	c.mv	s3,a5
2005edfa:	84c2                	c.mv	s1,a6
2005edfc:	4401                	c.li	s0,0
2005edfe:	4a81                	c.li	s5,0
2005ee00:	4a25                	c.li	s4,9
2005ee02:	4b05                	c.li	s6,1
2005ee04:	866a                	c.mv	a2,s10
2005ee06:	4681                	c.li	a3,0
2005ee08:	8562                	c.mv	a0,s8
2005ee0a:	85e6                	c.mv	a1,s9
2005ee0c:	1ef020ef          	jal	ra,200617fa <__umoddi3>
2005ee10:	0ff57893          	andi	a7,a0,255
2005ee14:	04aa6863          	bltu	s4,a0,2005ee64 <print_unsigned_num+0x90>
2005ee18:	03088893          	addi	a7,a7,48 # f8000030 <__ctrace_end__+0x2e000030>
2005ee1c:	007c                	c.addi4spn	a5,sp,12
2005ee1e:	97a2                	c.add	a5,s0
2005ee20:	0ff8f893          	andi	a7,a7,255
2005ee24:	01178023          	sb	a7,0(a5)
2005ee28:	866a                	c.mv	a2,s10
2005ee2a:	4681                	c.li	a3,0
2005ee2c:	8562                	c.mv	a0,s8
2005ee2e:	85e6                	c.mv	a1,s9
2005ee30:	0405                	c.addi	s0,1
2005ee32:	668020ef          	jal	ra,2006149a <__udivdi3>
2005ee36:	039a9463          	bne	s5,s9,2005ee5e <print_unsigned_num+0x8a>
2005ee3a:	03ac7263          	bgeu	s8,s10,2005ee5e <print_unsigned_num+0x8a>
2005ee3e:	c8d5                	c.beqz	s1,2005eef2 <print_unsigned_num+0x11e>
2005ee40:	03000793          	addi	a5,zero,48
2005ee44:	1bfd                	c.addi	s7,-1
2005ee46:	0af91163          	bne	s2,a5,2005eee8 <print_unsigned_num+0x114>
2005ee4a:	02d00513          	addi	a0,zero,45
2005ee4e:	dffa5097          	auipc	ra,0xdffa5
2005ee52:	6f6080e7          	jalr	ra,1782(ra) # 4544 <LOGUART_PutChar>
2005ee56:	89a6                	c.mv	s3,s1
2005ee58:	09704b63          	blt	zero,s7,2005eeee <print_unsigned_num+0x11a>
2005ee5c:	a0b1                	c.j	2005eea8 <print_unsigned_num+0xd4>
2005ee5e:	8c2a                	c.mv	s8,a0
2005ee60:	8cae                	c.mv	s9,a1
2005ee62:	b74d                	c.j	2005ee04 <print_unsigned_num+0x30>
2005ee64:	01699563          	bne	s3,s6,2005ee6e <print_unsigned_num+0x9a>
2005ee68:	03788893          	addi	a7,a7,55
2005ee6c:	bf45                	c.j	2005ee1c <print_unsigned_num+0x48>
2005ee6e:	05788893          	addi	a7,a7,87
2005ee72:	b76d                	c.j	2005ee1c <print_unsigned_num+0x48>
2005ee74:	854a                	c.mv	a0,s2
2005ee76:	dffa5097          	auipc	ra,0xdffa5
2005ee7a:	6ce080e7          	jalr	ra,1742(ra) # 4544 <LOGUART_PutChar>
2005ee7e:	1a7d                	c.addi	s4,-1
2005ee80:	ff444ae3          	blt	s0,s4,2005ee74 <print_unsigned_num+0xa0>
2005ee84:	4781                	c.li	a5,0
2005ee86:	008bc463          	blt	s7,s0,2005ee8e <print_unsigned_num+0xba>
2005ee8a:	408b87b3          	sub	a5,s7,s0
2005ee8e:	97ce                	c.add	a5,s3
2005ee90:	c0a9                	c.beqz	s1,2005eed2 <print_unsigned_num+0xfe>
2005ee92:	84be                	c.mv	s1,a5
2005ee94:	0df97913          	andi	s2,s2,223
2005ee98:	00091863          	bne	s2,zero,2005eea8 <print_unsigned_num+0xd4>
2005ee9c:	02d00513          	addi	a0,zero,45
2005eea0:	dffa5097          	auipc	ra,0xdffa5
2005eea4:	6a4080e7          	jalr	ra,1700(ra) # 4544 <LOGUART_PutChar>
2005eea8:	8922                	c.mv	s2,s0
2005eeaa:	59fd                	c.li	s3,-1
2005eeac:	197d                	c.addi	s2,-1
2005eeae:	03391463          	bne	s2,s3,2005eed6 <print_unsigned_num+0x102>
2005eeb2:	00940533          	add	a0,s0,s1
2005eeb6:	40b6                	c.lwsp	ra,76(sp)
2005eeb8:	4426                	c.lwsp	s0,72(sp)
2005eeba:	4496                	c.lwsp	s1,68(sp)
2005eebc:	4906                	c.lwsp	s2,64(sp)
2005eebe:	59f2                	c.lwsp	s3,60(sp)
2005eec0:	5a62                	c.lwsp	s4,56(sp)
2005eec2:	5ad2                	c.lwsp	s5,52(sp)
2005eec4:	5b42                	c.lwsp	s6,48(sp)
2005eec6:	5bb2                	c.lwsp	s7,44(sp)
2005eec8:	5c22                	c.lwsp	s8,40(sp)
2005eeca:	5c92                	c.lwsp	s9,36(sp)
2005eecc:	5d02                	c.lwsp	s10,32(sp)
2005eece:	6161                	c.addi16sp	sp,80
2005eed0:	8082                	c.jr	ra
2005eed2:	84be                	c.mv	s1,a5
2005eed4:	bfd1                	c.j	2005eea8 <print_unsigned_num+0xd4>
2005eed6:	007c                	c.addi4spn	a5,sp,12
2005eed8:	97ca                	c.add	a5,s2
2005eeda:	0007c503          	lbu	a0,0(a5)
2005eede:	dffa5097          	auipc	ra,0xdffa5
2005eee2:	666080e7          	jalr	ra,1638(ra) # 4544 <LOGUART_PutChar>
2005eee6:	b7d9                	c.j	2005eeac <print_unsigned_num+0xd8>
2005eee8:	fb7056e3          	bge	zero,s7,2005ee94 <print_unsigned_num+0xc0>
2005eeec:	89a6                	c.mv	s3,s1
2005eeee:	8a5e                	c.mv	s4,s7
2005eef0:	bf41                	c.j	2005ee80 <print_unsigned_num+0xac>
2005eef2:	4981                	c.li	s3,0
2005eef4:	b795                	c.j	2005ee58 <print_unsigned_num+0x84>

2005eef6 <pad_char_control>:
2005eef6:	1141                	c.addi	sp,-16
2005eef8:	c422                	c.swsp	s0,8(sp)
2005eefa:	c226                	c.swsp	s1,4(sp)
2005eefc:	c04a                	c.swsp	s2,0(sp)
2005eefe:	c606                	c.swsp	ra,12(sp)
2005ef00:	842a                	c.mv	s0,a0
2005ef02:	892e                	c.mv	s2,a1
2005ef04:	84aa                	c.mv	s1,a0
2005ef06:	00904c63          	blt	zero,s1,2005ef1e <pad_char_control+0x28>
2005ef0a:	8522                	c.mv	a0,s0
2005ef0c:	00045363          	bge	s0,zero,2005ef12 <pad_char_control+0x1c>
2005ef10:	4501                	c.li	a0,0
2005ef12:	40b2                	c.lwsp	ra,12(sp)
2005ef14:	4422                	c.lwsp	s0,8(sp)
2005ef16:	4492                	c.lwsp	s1,4(sp)
2005ef18:	4902                	c.lwsp	s2,0(sp)
2005ef1a:	0141                	c.addi	sp,16
2005ef1c:	8082                	c.jr	ra
2005ef1e:	854a                	c.mv	a0,s2
2005ef20:	dffa5097          	auipc	ra,0xdffa5
2005ef24:	624080e7          	jalr	ra,1572(ra) # 4544 <LOGUART_PutChar>
2005ef28:	14fd                	c.addi	s1,-1
2005ef2a:	bff1                	c.j	2005ef06 <pad_char_control+0x10>

2005ef2c <print_decimal_num>:
2005ef2c:	1101                	c.addi	sp,-32
2005ef2e:	c64e                	c.swsp	s3,12(sp)
2005ef30:	200689b7          	lui	s3,0x20068
2005ef34:	cc22                	c.swsp	s0,24(sp)
2005ef36:	c84a                	c.swsp	s2,16(sp)
2005ef38:	c452                	c.swsp	s4,8(sp)
2005ef3a:	c05a                	c.swsp	s6,0(sp)
2005ef3c:	8936                	c.mv	s2,a3
2005ef3e:	ce06                	c.swsp	ra,28(sp)
2005ef40:	ca26                	c.swsp	s1,20(sp)
2005ef42:	c256                	c.swsp	s5,4(sp)
2005ef44:	882a                	c.mv	a6,a0
2005ef46:	8b32                	c.mv	s6,a2
2005ef48:	8a3a                	c.mv	s4,a4
2005ef4a:	4401                	c.li	s0,0
2005ef4c:	46a5                	c.li	a3,9
2005ef4e:	5a098993          	addi	s3,s3,1440 # 200685a0 <num_buf_temp>
2005ef52:	02b87733          	remu	a4,a6,a1
2005ef56:	0ff77793          	andi	a5,a4,255
2005ef5a:	04e6ee63          	bltu	a3,a4,2005efb6 <print_decimal_num+0x8a>
2005ef5e:	03078793          	addi	a5,a5,48
2005ef62:	0ff7f793          	andi	a5,a5,255
2005ef66:	00898733          	add	a4,s3,s0
2005ef6a:	00f70023          	sb	a5,0(a4) # 40400000 <__km4_bd_boot_download_addr__+0x103ee000>
2005ef6e:	00140493          	addi	s1,s0,1
2005ef72:	02b857b3          	divu	a5,a6,a1
2005ef76:	02b87d63          	bgeu	a6,a1,2005efb0 <print_decimal_num+0x84>
2005ef7a:	8aca                	c.mv	s5,s2
2005ef7c:	05204663          	blt	zero,s2,2005efc8 <print_decimal_num+0x9c>
2005ef80:	4901                	c.li	s2,0
2005ef82:	02d00513          	addi	a0,zero,45
2005ef86:	040a1e63          	bne	s4,zero,2005efe2 <print_decimal_num+0xb6>
2005ef8a:	14fd                	c.addi	s1,-1
2005ef8c:	0404d763          	bge	s1,zero,2005efda <print_decimal_num+0xae>
2005ef90:	00045363          	bge	s0,zero,2005ef96 <print_decimal_num+0x6a>
2005ef94:	547d                	c.li	s0,-1
2005ef96:	0405                	c.addi	s0,1
2005ef98:	01240533          	add	a0,s0,s2
2005ef9c:	40f2                	c.lwsp	ra,28(sp)
2005ef9e:	4462                	c.lwsp	s0,24(sp)
2005efa0:	44d2                	c.lwsp	s1,20(sp)
2005efa2:	4942                	c.lwsp	s2,16(sp)
2005efa4:	49b2                	c.lwsp	s3,12(sp)
2005efa6:	4a22                	c.lwsp	s4,8(sp)
2005efa8:	4a92                	c.lwsp	s5,4(sp)
2005efaa:	4b02                	c.lwsp	s6,0(sp)
2005efac:	6105                	c.addi16sp	sp,32
2005efae:	8082                	c.jr	ra
2005efb0:	8426                	c.mv	s0,s1
2005efb2:	883e                	c.mv	a6,a5
2005efb4:	bf79                	c.j	2005ef52 <print_decimal_num+0x26>
2005efb6:	05778793          	addi	a5,a5,87
2005efba:	b765                	c.j	2005ef62 <print_decimal_num+0x36>
2005efbc:	855a                	c.mv	a0,s6
2005efbe:	dffa5097          	auipc	ra,0xdffa5
2005efc2:	586080e7          	jalr	ra,1414(ra) # 4544 <LOGUART_PutChar>
2005efc6:	1afd                	c.addi	s5,-1
2005efc8:	ff54cae3          	blt	s1,s5,2005efbc <print_decimal_num+0x90>
2005efcc:	4781                	c.li	a5,0
2005efce:	00994463          	blt	s2,s1,2005efd6 <print_decimal_num+0xaa>
2005efd2:	409907b3          	sub	a5,s2,s1
2005efd6:	893e                	c.mv	s2,a5
2005efd8:	b76d                	c.j	2005ef82 <print_decimal_num+0x56>
2005efda:	009987b3          	add	a5,s3,s1
2005efde:	0007c503          	lbu	a0,0(a5)
2005efe2:	dffa5097          	auipc	ra,0xdffa5
2005efe6:	562080e7          	jalr	ra,1378(ra) # 4544 <LOGUART_PutChar>
2005efea:	b745                	c.j	2005ef8a <print_decimal_num+0x5e>

2005efec <pad_num_control>:
2005efec:	04088163          	beq	a7,zero,2005f02e <pad_num_control+0x42>
2005eff0:	1141                	c.addi	sp,-16
2005eff2:	c226                	c.swsp	s1,4(sp)
2005eff4:	84ba                	c.mv	s1,a4
2005eff6:	4701                	c.li	a4,0
2005eff8:	c422                	c.swsp	s0,8(sp)
2005effa:	c04a                	c.swsp	s2,0(sp)
2005effc:	c606                	c.swsp	ra,12(sp)
2005effe:	8936                	c.mv	s2,a3
2005f000:	dd5ff0ef          	jal	ra,2005edd4 <print_unsigned_num>
2005f004:	842a                	c.mv	s0,a0
2005f006:	00955d63          	bge	a0,s1,2005f020 <pad_num_control+0x34>
2005f00a:	02000793          	addi	a5,zero,32
2005f00e:	00f91963          	bne	s2,a5,2005f020 <pad_num_control+0x34>
2005f012:	02000593          	addi	a1,zero,32
2005f016:	40a48533          	sub	a0,s1,a0
2005f01a:	eddff0ef          	jal	ra,2005eef6 <pad_char_control>
2005f01e:	942a                	c.add	s0,a0
2005f020:	40b2                	c.lwsp	ra,12(sp)
2005f022:	8522                	c.mv	a0,s0
2005f024:	4422                	c.lwsp	s0,8(sp)
2005f026:	4492                	c.lwsp	s1,4(sp)
2005f028:	4902                	c.lwsp	s2,0(sp)
2005f02a:	0141                	c.addi	sp,16
2005f02c:	8082                	c.jr	ra
2005f02e:	da7ff06f          	jal	zero,2005edd4 <print_unsigned_num>

2005f032 <_rand>:
2005f032:	1141                	c.addi	sp,-16
2005f034:	450d                	c.li	a0,3
2005f036:	c606                	c.swsp	ra,12(sp)
2005f038:	ea0fa0ef          	jal	ra,200596d8 <PLL_State>
2005f03c:	c50d                	c.beqz	a0,2005f066 <_rand+0x34>
2005f03e:	65c1                	c.lui	a1,0x10
2005f040:	410167b7          	lui	a5,0x41016
2005f044:	453d                	c.li	a0,15
2005f046:	4685                	c.li	a3,1
2005f048:	00158713          	addi	a4,a1,1 # 10001 <mbedtls_ecp_check_pub_priv+0x123>
2005f04c:	47d0                	c.lw	a2,12(a5)
2005f04e:	00a61763          	bne	a2,a0,2005f05c <_rand+0x2a>
2005f052:	df94                	c.sw	a3,56(a5)
2005f054:	177d                	c.addi	a4,-1
2005f056:	fb7d                	c.bnez	a4,2005f04c <_rand+0x1a>
2005f058:	df94                	c.sw	a3,56(a5)
2005f05a:	b7fd                	c.j	2005f048 <_rand+0x16>
2005f05c:	de65                	c.beqz	a2,2005f054 <_rand+0x22>
2005f05e:	4b88                	c.lw	a0,16(a5)
2005f060:	40b2                	c.lwsp	ra,12(sp)
2005f062:	0141                	c.addi	sp,16
2005f064:	8082                	c.jr	ra
2005f066:	fb018693          	addi	a3,gp,-80 # 20066960 <rand_first.1>
2005f06a:	429c                	c.lw	a5,0(a3)
2005f06c:	20068737          	lui	a4,0x20068
2005f070:	5ac70713          	addi	a4,a4,1452 # 200685ac <rand_seed.0>
2005f074:	eb91                	c.bnez	a5,2005f088 <_rand+0x56>
2005f076:	678d                	c.lui	a5,0x3
2005f078:	03978793          	addi	a5,a5,57 # 3039 <FLASH_SetSpiMode+0x375>
2005f07c:	c31c                	c.sw	a5,0(a4)
2005f07e:	c35c                	c.sw	a5,4(a4)
2005f080:	c71c                	c.sw	a5,8(a4)
2005f082:	c75c                	c.sw	a5,12(a4)
2005f084:	4785                	c.li	a5,1
2005f086:	c29c                	c.sw	a5,0(a3)
2005f088:	4314                	c.lw	a3,0(a4)
2005f08a:	fff80537          	lui	a0,0xfff80
2005f08e:	474c                	c.lw	a1,12(a4)
2005f090:	00669793          	slli	a5,a3,0x6
2005f094:	8fb5                	c.xor	a5,a3
2005f096:	06ca                	c.slli	a3,0x12
2005f098:	8ee9                	c.and	a3,a0
2005f09a:	83b5                	c.srli	a5,0xd
2005f09c:	8fb5                	c.xor	a5,a3
2005f09e:	4354                	c.lw	a3,4(a4)
2005f0a0:	fff00837          	lui	a6,0xfff00
2005f0a4:	c31c                	c.sw	a5,0(a4)
2005f0a6:	00269613          	slli	a2,a3,0x2
2005f0aa:	00c6c533          	xor	a0,a3,a2
2005f0ae:	4714                	c.lw	a3,8(a4)
2005f0b0:	9a01                	c.andi	a2,-32
2005f0b2:	816d                	c.srli	a0,0x1b
2005f0b4:	8d31                	c.xor	a0,a2
2005f0b6:	00d69613          	slli	a2,a3,0xd
2005f0ba:	8e35                	c.xor	a2,a3
2005f0bc:	069e                	c.slli	a3,0x7
2005f0be:	8006f693          	andi	a3,a3,-2048
2005f0c2:	8255                	c.srli	a2,0x15
2005f0c4:	8e35                	c.xor	a2,a3
2005f0c6:	00359693          	slli	a3,a1,0x3
2005f0ca:	8ead                	c.xor	a3,a1
2005f0cc:	05b6                	c.slli	a1,0xd
2005f0ce:	c348                	c.sw	a0,4(a4)
2005f0d0:	82b1                	c.srli	a3,0xc
2005f0d2:	0105f5b3          	and	a1,a1,a6
2005f0d6:	8d3d                	c.xor	a0,a5
2005f0d8:	8ead                	c.xor	a3,a1
2005f0da:	8d31                	c.xor	a0,a2
2005f0dc:	c710                	c.sw	a2,8(a4)
2005f0de:	c754                	c.sw	a3,12(a4)
2005f0e0:	8d35                	c.xor	a0,a3
2005f0e2:	bfbd                	c.j	2005f060 <_rand+0x2e>

2005f0e4 <DiagVprintf>:
2005f0e4:	7139                	c.addi16sp	sp,-64
2005f0e6:	d452                	c.swsp	s4,40(sp)
2005f0e8:	20064a37          	lui	s4,0x20064
2005f0ec:	dc22                	c.swsp	s0,56(sp)
2005f0ee:	da26                	c.swsp	s1,52(sp)
2005f0f0:	d84a                	c.swsp	s2,48(sp)
2005f0f2:	d64e                	c.swsp	s3,44(sp)
2005f0f4:	d256                	c.swsp	s5,36(sp)
2005f0f6:	d05a                	c.swsp	s6,32(sp)
2005f0f8:	de06                	c.swsp	ra,60(sp)
2005f0fa:	ce5e                	c.swsp	s7,28(sp)
2005f0fc:	cc62                	c.swsp	s8,24(sp)
2005f0fe:	ca66                	c.swsp	s9,20(sp)
2005f100:	892a                	c.mv	s2,a0
2005f102:	842e                	c.mv	s0,a1
2005f104:	4981                	c.li	s3,0
2005f106:	4481                	c.li	s1,0
2005f108:	ad4a0a13          	addi	s4,s4,-1324 # 20063ad4 <TaskStateString.0+0x18>
2005f10c:	4aa5                	c.li	s5,9
2005f10e:	4b29                	c.li	s6,10
2005f110:	00094503          	lbu	a0,0(s2)
2005f114:	ed19                	c.bnez	a0,2005f132 <DiagVprintf+0x4e>
2005f116:	50f2                	c.lwsp	ra,60(sp)
2005f118:	5462                	c.lwsp	s0,56(sp)
2005f11a:	5942                	c.lwsp	s2,48(sp)
2005f11c:	59b2                	c.lwsp	s3,44(sp)
2005f11e:	5a22                	c.lwsp	s4,40(sp)
2005f120:	5a92                	c.lwsp	s5,36(sp)
2005f122:	5b02                	c.lwsp	s6,32(sp)
2005f124:	4bf2                	c.lwsp	s7,28(sp)
2005f126:	4c62                	c.lwsp	s8,24(sp)
2005f128:	4cd2                	c.lwsp	s9,20(sp)
2005f12a:	8526                	c.mv	a0,s1
2005f12c:	54d2                	c.lwsp	s1,52(sp)
2005f12e:	6121                	c.addi16sp	sp,64
2005f130:	8082                	c.jr	ra
2005f132:	02500793          	addi	a5,zero,37
2005f136:	0905                	c.addi	s2,1
2005f138:	28f51f63          	bne	a0,a5,2005f3d6 <DiagVprintf+0x2f2>
2005f13c:	4881                	c.li	a7,0
2005f13e:	4701                	c.li	a4,0
2005f140:	4681                	c.li	a3,0
2005f142:	4601                	c.li	a2,0
2005f144:	07a00e13          	addi	t3,zero,122
2005f148:	05700e93          	addi	t4,zero,87
2005f14c:	02200f13          	addi	t5,zero,34
2005f150:	03000593          	addi	a1,zero,48
2005f154:	04300813          	addi	a6,zero,67
2005f158:	04c00f93          	addi	t6,zero,76
2005f15c:	03900293          	addi	t0,zero,57
2005f160:	02000393          	addi	t2,zero,32
2005f164:	02500313          	addi	t1,zero,37
2005f168:	02d00b93          	addi	s7,zero,45
2005f16c:	00094783          	lbu	a5,0(s2)
2005f170:	02fe6063          	bltu	t3,a5,2005f190 <DiagVprintf+0xac>
2005f174:	02fee063          	bltu	t4,a5,2005f194 <DiagVprintf+0xb0>
2005f178:	22b78663          	beq	a5,a1,2005f3a4 <DiagVprintf+0x2c0>
2005f17c:	02f5eb63          	bltu	a1,a5,2005f1b2 <DiagVprintf+0xce>
2005f180:	20678963          	beq	a5,t1,2005f392 <DiagVprintf+0x2ae>
2005f184:	02f36263          	bltu	t1,a5,2005f1a8 <DiagVprintf+0xc4>
2005f188:	20078c63          	beq	a5,zero,2005f3a0 <DiagVprintf+0x2bc>
2005f18c:	24778163          	beq	a5,t2,2005f3ce <DiagVprintf+0x2ea>
2005f190:	54fd                	c.li	s1,-1
2005f192:	b751                	c.j	2005f116 <DiagVprintf+0x32>
2005f194:	fa878793          	addi	a5,a5,-88
2005f198:	0ff7f793          	andi	a5,a5,255
2005f19c:	feff6ae3          	bltu	t5,a5,2005f190 <DiagVprintf+0xac>
2005f1a0:	078a                	c.slli	a5,0x2
2005f1a2:	97d2                	c.add	a5,s4
2005f1a4:	439c                	c.lw	a5,0(a5)
2005f1a6:	8782                	c.jr	a5
2005f1a8:	ff7794e3          	bne	a5,s7,2005f190 <DiagVprintf+0xac>
2005f1ac:	0905                	c.addi	s2,1
2005f1ae:	0885                	c.addi	a7,1
2005f1b0:	bf75                	c.j	2005f16c <DiagVprintf+0x88>
2005f1b2:	19078363          	beq	a5,a6,2005f338 <DiagVprintf+0x254>
2005f1b6:	02f86563          	bltu	a6,a5,2005f1e0 <DiagVprintf+0xfc>
2005f1ba:	20f2fb63          	bgeu	t0,a5,2005f3d0 <DiagVprintf+0x2ec>
2005f1be:	04200593          	addi	a1,zero,66
2005f1c2:	fcb797e3          	bne	a5,a1,2005f190 <DiagVprintf+0xac>
2005f1c6:	4785                	c.li	a5,1
2005f1c8:	0ac7d063          	bge	a5,a2,2005f268 <DiagVprintf+0x184>
2005f1cc:	041d                	c.addi	s0,7
2005f1ce:	9861                	c.andi	s0,-8
2005f1d0:	4008                	c.lw	a0,0(s0)
2005f1d2:	404c                	c.lw	a1,4(s0)
2005f1d4:	00840b93          	addi	s7,s0,8
2005f1d8:	4801                	c.li	a6,0
2005f1da:	87ce                	c.mv	a5,s3
2005f1dc:	4609                	c.li	a2,2
2005f1de:	a889                	c.j	2005f230 <DiagVprintf+0x14c>
2005f1e0:	1bf78663          	beq	a5,t6,2005f38c <DiagVprintf+0x2a8>
2005f1e4:	04f00593          	addi	a1,zero,79
2005f1e8:	fab794e3          	bne	a5,a1,2005f190 <DiagVprintf+0xac>
2005f1ec:	4785                	c.li	a5,1
2005f1ee:	08c7d263          	bge	a5,a2,2005f272 <DiagVprintf+0x18e>
2005f1f2:	041d                	c.addi	s0,7
2005f1f4:	9861                	c.andi	s0,-8
2005f1f6:	4008                	c.lw	a0,0(s0)
2005f1f8:	404c                	c.lw	a1,4(s0)
2005f1fa:	00840b93          	addi	s7,s0,8
2005f1fe:	4801                	c.li	a6,0
2005f200:	87ce                	c.mv	a5,s3
2005f202:	4621                	c.li	a2,8
2005f204:	a035                	c.j	2005f230 <DiagVprintf+0x14c>
2005f206:	4785                	c.li	a5,1
2005f208:	02c7da63          	bge	a5,a2,2005f23c <DiagVprintf+0x158>
2005f20c:	041d                	c.addi	s0,7
2005f20e:	9861                	c.andi	s0,-8
2005f210:	4008                	c.lw	a0,0(s0)
2005f212:	404c                	c.lw	a1,4(s0)
2005f214:	00840b93          	addi	s7,s0,8
2005f218:	0405d163          	bge	a1,zero,2005f25a <DiagVprintf+0x176>
2005f21c:	00a037b3          	sltu	a5,zero,a0
2005f220:	40b005b3          	sub	a1,zero,a1
2005f224:	40a00533          	sub	a0,zero,a0
2005f228:	8d9d                	c.sub	a1,a5
2005f22a:	4805                	c.li	a6,1
2005f22c:	87ce                	c.mv	a5,s3
2005f22e:	4629                	c.li	a2,10
2005f230:	dbdff0ef          	jal	ra,2005efec <pad_num_control>
2005f234:	94aa                	c.add	s1,a0
2005f236:	0905                	c.addi	s2,1
2005f238:	845e                	c.mv	s0,s7
2005f23a:	bdd9                	c.j	2005f110 <DiagVprintf+0x2c>
2005f23c:	4008                	c.lw	a0,0(s0)
2005f23e:	00440b93          	addi	s7,s0,4
2005f242:	41f55593          	srai	a1,a0,0x1f
2005f246:	bfc9                	c.j	2005f218 <DiagVprintf+0x134>
2005f248:	4785                	c.li	a5,1
2005f24a:	00c7da63          	bge	a5,a2,2005f25e <DiagVprintf+0x17a>
2005f24e:	041d                	c.addi	s0,7
2005f250:	9861                	c.andi	s0,-8
2005f252:	4008                	c.lw	a0,0(s0)
2005f254:	404c                	c.lw	a1,4(s0)
2005f256:	00840b93          	addi	s7,s0,8
2005f25a:	4801                	c.li	a6,0
2005f25c:	bfc1                	c.j	2005f22c <DiagVprintf+0x148>
2005f25e:	4008                	c.lw	a0,0(s0)
2005f260:	4581                	c.li	a1,0
2005f262:	00440b93          	addi	s7,s0,4
2005f266:	bfd5                	c.j	2005f25a <DiagVprintf+0x176>
2005f268:	4008                	c.lw	a0,0(s0)
2005f26a:	4581                	c.li	a1,0
2005f26c:	00440b93          	addi	s7,s0,4
2005f270:	b7a5                	c.j	2005f1d8 <DiagVprintf+0xf4>
2005f272:	4008                	c.lw	a0,0(s0)
2005f274:	4581                	c.li	a1,0
2005f276:	00440b93          	addi	s7,s0,4
2005f27a:	b751                	c.j	2005f1fe <DiagVprintf+0x11a>
2005f27c:	4985                	c.li	s3,1
2005f27e:	4785                	c.li	a5,1
2005f280:	00c7dc63          	bge	a5,a2,2005f298 <DiagVprintf+0x1b4>
2005f284:	041d                	c.addi	s0,7
2005f286:	9861                	c.andi	s0,-8
2005f288:	4008                	c.lw	a0,0(s0)
2005f28a:	404c                	c.lw	a1,4(s0)
2005f28c:	00840b93          	addi	s7,s0,8
2005f290:	4801                	c.li	a6,0
2005f292:	87ce                	c.mv	a5,s3
2005f294:	4641                	c.li	a2,16
2005f296:	bf69                	c.j	2005f230 <DiagVprintf+0x14c>
2005f298:	4008                	c.lw	a0,0(s0)
2005f29a:	4581                	c.li	a1,0
2005f29c:	00440b93          	addi	s7,s0,4
2005f2a0:	bfc5                	c.j	2005f290 <DiagVprintf+0x1ac>
2005f2a2:	4008                	c.lw	a0,0(s0)
2005f2a4:	4801                	c.li	a6,0
2005f2a6:	4785                	c.li	a5,1
2005f2a8:	4721                	c.li	a4,8
2005f2aa:	03000693          	addi	a3,zero,48
2005f2ae:	4641                	c.li	a2,16
2005f2b0:	4581                	c.li	a1,0
2005f2b2:	d3bff0ef          	jal	ra,2005efec <pad_num_control>
2005f2b6:	00440b93          	addi	s7,s0,4
2005f2ba:	94aa                	c.add	s1,a0
2005f2bc:	4985                	c.li	s3,1
2005f2be:	bfa5                	c.j	2005f236 <DiagVprintf+0x152>
2005f2c0:	00042c83          	lw	s9,0(s0)
2005f2c4:	00440b93          	addi	s7,s0,4
2005f2c8:	4401                	c.li	s0,0
2005f2ca:	008c87b3          	add	a5,s9,s0
2005f2ce:	0007c783          	lbu	a5,0(a5)
2005f2d2:	e7a9                	c.bnez	a5,2005f31c <DiagVprintf+0x238>
2005f2d4:	02089063          	bne	a7,zero,2005f2f4 <DiagVprintf+0x210>
2005f2d8:	40870533          	sub	a0,a4,s0
2005f2dc:	00a05c63          	bge	zero,a0,2005f2f4 <DiagVprintf+0x210>
2005f2e0:	85b6                	c.mv	a1,a3
2005f2e2:	c646                	c.swsp	a7,12(sp)
2005f2e4:	c43a                	c.swsp	a4,8(sp)
2005f2e6:	c236                	c.swsp	a3,4(sp)
2005f2e8:	c0fff0ef          	jal	ra,2005eef6 <pad_char_control>
2005f2ec:	48b2                	c.lwsp	a7,12(sp)
2005f2ee:	4722                	c.lwsp	a4,8(sp)
2005f2f0:	4692                	c.lwsp	a3,4(sp)
2005f2f2:	94aa                	c.add	s1,a0
2005f2f4:	5c7d                	c.li	s8,-1
2005f2f6:	000c8863          	beq	s9,zero,2005f306 <DiagVprintf+0x222>
2005f2fa:	4c01                	c.li	s8,0
2005f2fc:	018c87b3          	add	a5,s9,s8
2005f300:	0007c503          	lbu	a0,0(a5)
2005f304:	ed11                	c.bnez	a0,2005f320 <DiagVprintf+0x23c>
2005f306:	94e2                	c.add	s1,s8
2005f308:	f20887e3          	beq	a7,zero,2005f236 <DiagVprintf+0x152>
2005f30c:	40870533          	sub	a0,a4,s0
2005f310:	85b6                	c.mv	a1,a3
2005f312:	f2a052e3          	bge	zero,a0,2005f236 <DiagVprintf+0x152>
2005f316:	be1ff0ef          	jal	ra,2005eef6 <pad_char_control>
2005f31a:	bf29                	c.j	2005f234 <DiagVprintf+0x150>
2005f31c:	0405                	c.addi	s0,1
2005f31e:	b775                	c.j	2005f2ca <DiagVprintf+0x1e6>
2005f320:	c646                	c.swsp	a7,12(sp)
2005f322:	c43a                	c.swsp	a4,8(sp)
2005f324:	c236                	c.swsp	a3,4(sp)
2005f326:	dffa5097          	auipc	ra,0xdffa5
2005f32a:	21e080e7          	jalr	ra,542(ra) # 4544 <LOGUART_PutChar>
2005f32e:	48b2                	c.lwsp	a7,12(sp)
2005f330:	4722                	c.lwsp	a4,8(sp)
2005f332:	4692                	c.lwsp	a3,4(sp)
2005f334:	0c05                	c.addi	s8,1
2005f336:	b7d9                	c.j	2005f2fc <DiagVprintf+0x218>
2005f338:	00440b93          	addi	s7,s0,4
2005f33c:	00044403          	lbu	s0,0(s0)
2005f340:	02089163          	bne	a7,zero,2005f362 <DiagVprintf+0x27e>
2005f344:	4785                	c.li	a5,1
2005f346:	00e7de63          	bge	a5,a4,2005f362 <DiagVprintf+0x27e>
2005f34a:	85b6                	c.mv	a1,a3
2005f34c:	fff70513          	addi	a0,a4,-1
2005f350:	c646                	c.swsp	a7,12(sp)
2005f352:	c436                	c.swsp	a3,8(sp)
2005f354:	c23a                	c.swsp	a4,4(sp)
2005f356:	ba1ff0ef          	jal	ra,2005eef6 <pad_char_control>
2005f35a:	48b2                	c.lwsp	a7,12(sp)
2005f35c:	46a2                	c.lwsp	a3,8(sp)
2005f35e:	4712                	c.lwsp	a4,4(sp)
2005f360:	94aa                	c.add	s1,a0
2005f362:	8522                	c.mv	a0,s0
2005f364:	c646                	c.swsp	a7,12(sp)
2005f366:	c43a                	c.swsp	a4,8(sp)
2005f368:	c236                	c.swsp	a3,4(sp)
2005f36a:	dffa5097          	auipc	ra,0xdffa5
2005f36e:	1da080e7          	jalr	ra,474(ra) # 4544 <LOGUART_PutChar>
2005f372:	48b2                	c.lwsp	a7,12(sp)
2005f374:	0485                	c.addi	s1,1
2005f376:	ec0880e3          	beq	a7,zero,2005f236 <DiagVprintf+0x152>
2005f37a:	4722                	c.lwsp	a4,8(sp)
2005f37c:	4785                	c.li	a5,1
2005f37e:	eae7dce3          	bge	a5,a4,2005f236 <DiagVprintf+0x152>
2005f382:	4692                	c.lwsp	a3,4(sp)
2005f384:	fff70513          	addi	a0,a4,-1
2005f388:	85b6                	c.mv	a1,a3
2005f38a:	b771                	c.j	2005f316 <DiagVprintf+0x232>
2005f38c:	0605                	c.addi	a2,1
2005f38e:	0905                	c.addi	s2,1
2005f390:	bbf1                	c.j	2005f16c <DiagVprintf+0x88>
2005f392:	02500513          	addi	a0,zero,37
2005f396:	dffa5097          	auipc	ra,0xdffa5
2005f39a:	1ae080e7          	jalr	ra,430(ra) # 4544 <LOGUART_PutChar>
2005f39e:	0485                	c.addi	s1,1
2005f3a0:	8ba2                	c.mv	s7,s0
2005f3a2:	bd51                	c.j	2005f236 <DiagVprintf+0x152>
2005f3a4:	0905                	c.addi	s2,1
2005f3a6:	03000693          	addi	a3,zero,48
2005f3aa:	4701                	c.li	a4,0
2005f3ac:	00094783          	lbu	a5,0(s2)
2005f3b0:	d60783e3          	beq	a5,zero,2005f116 <DiagVprintf+0x32>
2005f3b4:	fd078513          	addi	a0,a5,-48
2005f3b8:	0ff57513          	andi	a0,a0,255
2005f3bc:	daaae8e3          	bltu	s5,a0,2005f16c <DiagVprintf+0x88>
2005f3c0:	03670733          	mul	a4,a4,s6
2005f3c4:	0905                	c.addi	s2,1
2005f3c6:	fd070713          	addi	a4,a4,-48
2005f3ca:	973e                	c.add	a4,a5
2005f3cc:	b7c5                	c.j	2005f3ac <DiagVprintf+0x2c8>
2005f3ce:	0905                	c.addi	s2,1
2005f3d0:	02000693          	addi	a3,zero,32
2005f3d4:	bfd9                	c.j	2005f3aa <DiagVprintf+0x2c6>
2005f3d6:	dffa5097          	auipc	ra,0xdffa5
2005f3da:	16e080e7          	jalr	ra,366(ra) # 4544 <LOGUART_PutChar>
2005f3de:	0485                	c.addi	s1,1
2005f3e0:	bb05                	c.j	2005f110 <DiagVprintf+0x2c>

2005f3e2 <DiagPrintf>:
2005f3e2:	7139                	c.addi16sp	sp,-64
2005f3e4:	d22e                	c.swsp	a1,36(sp)
2005f3e6:	104c                	c.addi4spn	a1,sp,36
2005f3e8:	ce06                	c.swsp	ra,28(sp)
2005f3ea:	d432                	c.swsp	a2,40(sp)
2005f3ec:	d636                	c.swsp	a3,44(sp)
2005f3ee:	d83a                	c.swsp	a4,48(sp)
2005f3f0:	da3e                	c.swsp	a5,52(sp)
2005f3f2:	dc42                	c.swsp	a6,56(sp)
2005f3f4:	de46                	c.swsp	a7,60(sp)
2005f3f6:	c62e                	c.swsp	a1,12(sp)
2005f3f8:	cedff0ef          	jal	ra,2005f0e4 <DiagVprintf>
2005f3fc:	40f2                	c.lwsp	ra,28(sp)
2005f3fe:	6121                	c.addi16sp	sp,64
2005f400:	8082                	c.jr	ra

2005f402 <DiagPrintf_minimal>:
2005f402:	711d                	c.addi16sp	sp,-96
2005f404:	da26                	c.swsp	s1,52(sp)
2005f406:	00c4                	c.addi4spn	s1,sp,68
2005f408:	dc22                	c.swsp	s0,56(sp)
2005f40a:	d84a                	c.swsp	s2,48(sp)
2005f40c:	d64e                	c.swsp	s3,44(sp)
2005f40e:	d452                	c.swsp	s4,40(sp)
2005f410:	d256                	c.swsp	s5,36(sp)
2005f412:	de06                	c.swsp	ra,60(sp)
2005f414:	d05a                	c.swsp	s6,32(sp)
2005f416:	ce5e                	c.swsp	s7,28(sp)
2005f418:	892a                	c.mv	s2,a0
2005f41a:	c2ae                	c.swsp	a1,68(sp)
2005f41c:	c4b2                	c.swsp	a2,72(sp)
2005f41e:	c6b6                	c.swsp	a3,76(sp)
2005f420:	c8ba                	c.swsp	a4,80(sp)
2005f422:	cabe                	c.swsp	a5,84(sp)
2005f424:	ccc2                	c.swsp	a6,88(sp)
2005f426:	cec6                	c.swsp	a7,92(sp)
2005f428:	c626                	c.swsp	s1,12(sp)
2005f42a:	4401                	c.li	s0,0
2005f42c:	03000993          	addi	s3,zero,48
2005f430:	4a25                	c.li	s4,9
2005f432:	4aa9                	c.li	s5,10
2005f434:	00094503          	lbu	a0,0(s2)
2005f438:	cd1d                	c.beqz	a0,2005f476 <DiagPrintf_minimal+0x74>
2005f43a:	02500793          	addi	a5,zero,37
2005f43e:	0905                	c.addi	s2,1
2005f440:	16f51a63          	bne	a0,a5,2005f5b4 <DiagPrintf_minimal+0x1b2>
2005f444:	4681                	c.li	a3,0
2005f446:	4601                	c.li	a2,0
2005f448:	06300893          	addi	a7,zero,99
2005f44c:	03900593          	addi	a1,zero,57
2005f450:	02000313          	addi	t1,zero,32
2005f454:	00094783          	lbu	a5,0(s2)
2005f458:	05178163          	beq	a5,a7,2005f49a <DiagPrintf_minimal+0x98>
2005f45c:	06f8e363          	bltu	a7,a5,2005f4c2 <DiagPrintf_minimal+0xc0>
2005f460:	13378063          	beq	a5,s3,2005f580 <DiagPrintf_minimal+0x17e>
2005f464:	02f9e563          	bltu	s3,a5,2005f48e <DiagPrintf_minimal+0x8c>
2005f468:	14678263          	beq	a5,t1,2005f5ac <DiagPrintf_minimal+0x1aa>
2005f46c:	02500713          	addi	a4,zero,37
2005f470:	10e78063          	beq	a5,a4,2005f570 <DiagPrintf_minimal+0x16e>
2005f474:	547d                	c.li	s0,-1
2005f476:	50f2                	c.lwsp	ra,60(sp)
2005f478:	8522                	c.mv	a0,s0
2005f47a:	5462                	c.lwsp	s0,56(sp)
2005f47c:	54d2                	c.lwsp	s1,52(sp)
2005f47e:	5942                	c.lwsp	s2,48(sp)
2005f480:	59b2                	c.lwsp	s3,44(sp)
2005f482:	5a22                	c.lwsp	s4,40(sp)
2005f484:	5a92                	c.lwsp	s5,36(sp)
2005f486:	5b02                	c.lwsp	s6,32(sp)
2005f488:	4bf2                	c.lwsp	s7,28(sp)
2005f48a:	6125                	c.addi16sp	sp,96
2005f48c:	8082                	c.jr	ra
2005f48e:	12f5f063          	bgeu	a1,a5,2005f5ae <DiagPrintf_minimal+0x1ac>
2005f492:	04300713          	addi	a4,zero,67
2005f496:	fce79fe3          	bne	a5,a4,2005f474 <DiagPrintf_minimal+0x72>
2005f49a:	4785                	c.li	a5,1
2005f49c:	00448b13          	addi	s6,s1,4
2005f4a0:	0004c483          	lbu	s1,0(s1)
2005f4a4:	00d7d863          	bge	a5,a3,2005f4b4 <DiagPrintf_minimal+0xb2>
2005f4a8:	85b2                	c.mv	a1,a2
2005f4aa:	fff68513          	addi	a0,a3,-1
2005f4ae:	a49ff0ef          	jal	ra,2005eef6 <pad_char_control>
2005f4b2:	942a                	c.add	s0,a0
2005f4b4:	8526                	c.mv	a0,s1
2005f4b6:	dffa5097          	auipc	ra,0xdffa5
2005f4ba:	08e080e7          	jalr	ra,142(ra) # 4544 <LOGUART_PutChar>
2005f4be:	0405                	c.addi	s0,1
2005f4c0:	a825                	c.j	2005f4f8 <DiagPrintf_minimal+0xf6>
2005f4c2:	07300713          	addi	a4,zero,115
2005f4c6:	04e78e63          	beq	a5,a4,2005f522 <DiagPrintf_minimal+0x120>
2005f4ca:	02f76963          	bltu	a4,a5,2005f4fc <DiagPrintf_minimal+0xfa>
2005f4ce:	06400713          	addi	a4,zero,100
2005f4d2:	00e78663          	beq	a5,a4,2005f4de <DiagPrintf_minimal+0xdc>
2005f4d6:	06900713          	addi	a4,zero,105
2005f4da:	f8e79de3          	bne	a5,a4,2005f474 <DiagPrintf_minimal+0x72>
2005f4de:	4088                	c.lw	a0,0(s1)
2005f4e0:	00448b13          	addi	s6,s1,4
2005f4e4:	4701                	c.li	a4,0
2005f4e6:	00055563          	bge	a0,zero,2005f4f0 <DiagPrintf_minimal+0xee>
2005f4ea:	40a00533          	sub	a0,zero,a0
2005f4ee:	4705                	c.li	a4,1
2005f4f0:	45a9                	c.li	a1,10
2005f4f2:	a3bff0ef          	jal	ra,2005ef2c <print_decimal_num>
2005f4f6:	942a                	c.add	s0,a0
2005f4f8:	84da                	c.mv	s1,s6
2005f4fa:	a08d                	c.j	2005f55c <DiagPrintf_minimal+0x15a>
2005f4fc:	07500713          	addi	a4,zero,117
2005f500:	00e78b63          	beq	a5,a4,2005f516 <DiagPrintf_minimal+0x114>
2005f504:	07800713          	addi	a4,zero,120
2005f508:	f6e796e3          	bne	a5,a4,2005f474 <DiagPrintf_minimal+0x72>
2005f50c:	00448b13          	addi	s6,s1,4
2005f510:	4701                	c.li	a4,0
2005f512:	45c1                	c.li	a1,16
2005f514:	a029                	c.j	2005f51e <DiagPrintf_minimal+0x11c>
2005f516:	00448b13          	addi	s6,s1,4
2005f51a:	4701                	c.li	a4,0
2005f51c:	45a9                	c.li	a1,10
2005f51e:	4088                	c.lw	a0,0(s1)
2005f520:	bfc9                	c.j	2005f4f2 <DiagPrintf_minimal+0xf0>
2005f522:	0004ab03          	lw	s6,0(s1)
2005f526:	00448b93          	addi	s7,s1,4
2005f52a:	4781                	c.li	a5,0
2005f52c:	00fb0733          	add	a4,s6,a5
2005f530:	00074703          	lbu	a4,0(a4)
2005f534:	e715                	c.bnez	a4,2005f560 <DiagPrintf_minimal+0x15e>
2005f536:	40f68533          	sub	a0,a3,a5
2005f53a:	00a05663          	bge	zero,a0,2005f546 <DiagPrintf_minimal+0x144>
2005f53e:	85b2                	c.mv	a1,a2
2005f540:	9b7ff0ef          	jal	ra,2005eef6 <pad_char_control>
2005f544:	942a                	c.add	s0,a0
2005f546:	54fd                	c.li	s1,-1
2005f548:	000b0863          	beq	s6,zero,2005f558 <DiagPrintf_minimal+0x156>
2005f54c:	4481                	c.li	s1,0
2005f54e:	009b07b3          	add	a5,s6,s1
2005f552:	0007c503          	lbu	a0,0(a5)
2005f556:	e519                	c.bnez	a0,2005f564 <DiagPrintf_minimal+0x162>
2005f558:	9426                	c.add	s0,s1
2005f55a:	84de                	c.mv	s1,s7
2005f55c:	0905                	c.addi	s2,1
2005f55e:	bdd9                	c.j	2005f434 <DiagPrintf_minimal+0x32>
2005f560:	0785                	c.addi	a5,1
2005f562:	b7e9                	c.j	2005f52c <DiagPrintf_minimal+0x12a>
2005f564:	dffa5097          	auipc	ra,0xdffa5
2005f568:	fe0080e7          	jalr	ra,-32(ra) # 4544 <LOGUART_PutChar>
2005f56c:	0485                	c.addi	s1,1
2005f56e:	b7c5                	c.j	2005f54e <DiagPrintf_minimal+0x14c>
2005f570:	02500513          	addi	a0,zero,37
2005f574:	dffa5097          	auipc	ra,0xdffa5
2005f578:	fd0080e7          	jalr	ra,-48(ra) # 4544 <LOGUART_PutChar>
2005f57c:	0405                	c.addi	s0,1
2005f57e:	bff9                	c.j	2005f55c <DiagPrintf_minimal+0x15a>
2005f580:	0905                	c.addi	s2,1
2005f582:	03000613          	addi	a2,zero,48
2005f586:	4681                	c.li	a3,0
2005f588:	00094703          	lbu	a4,0(s2)
2005f58c:	ee0705e3          	beq	a4,zero,2005f476 <DiagPrintf_minimal+0x74>
2005f590:	fd070813          	addi	a6,a4,-48
2005f594:	0ff87813          	andi	a6,a6,255
2005f598:	eb0a6ee3          	bltu	s4,a6,2005f454 <DiagPrintf_minimal+0x52>
2005f59c:	03568533          	mul	a0,a3,s5
2005f5a0:	0905                	c.addi	s2,1
2005f5a2:	fd050513          	addi	a0,a0,-48 # fff7ffd0 <__ctrace_end__+0x35f7ffd0>
2005f5a6:	00a706b3          	add	a3,a4,a0
2005f5aa:	bff9                	c.j	2005f588 <DiagPrintf_minimal+0x186>
2005f5ac:	0905                	c.addi	s2,1
2005f5ae:	02000613          	addi	a2,zero,32
2005f5b2:	bfd1                	c.j	2005f586 <DiagPrintf_minimal+0x184>
2005f5b4:	dffa5097          	auipc	ra,0xdffa5
2005f5b8:	f90080e7          	jalr	ra,-112(ra) # 4544 <LOGUART_PutChar>
2005f5bc:	0405                	c.addi	s0,1
2005f5be:	bd9d                	c.j	2005f434 <DiagPrintf_minimal+0x32>

2005f5c0 <DiagSnPrintf>:
2005f5c0:	7159                	c.addi16sp	sp,-112
2005f5c2:	c6a2                	c.swsp	s0,76(sp)
2005f5c4:	c4a6                	c.swsp	s1,72(sp)
2005f5c6:	c2ca                	c.swsp	s2,68(sp)
2005f5c8:	c0ce                	c.swsp	s3,64(sp)
2005f5ca:	de52                	c.swsp	s4,60(sp)
2005f5cc:	dc56                	c.swsp	s5,56(sp)
2005f5ce:	da5a                	c.swsp	s6,52(sp)
2005f5d0:	d85e                	c.swsp	s7,48(sp)
2005f5d2:	d662                	c.swsp	s8,44(sp)
2005f5d4:	d466                	c.swsp	s9,40(sp)
2005f5d6:	d26a                	c.swsp	s10,36(sp)
2005f5d8:	d06e                	c.swsp	s11,32(sp)
2005f5da:	ceb6                	c.swsp	a3,92(sp)
2005f5dc:	d0ba                	c.swsp	a4,96(sp)
2005f5de:	d2be                	c.swsp	a5,100(sp)
2005f5e0:	d4c2                	c.swsp	a6,104(sp)
2005f5e2:	d6c6                	c.swsp	a7,108(sp)
2005f5e4:	28050463          	beq	a0,zero,2005f86c <DiagSnPrintf+0x2ac>
2005f5e8:	08fc                	c.addi4spn	a5,sp,92
2005f5ea:	c43e                	c.swsp	a5,8(sp)
2005f5ec:	537d                	c.li	t1,-1
2005f5ee:	c199                	c.beqz	a1,2005f5f4 <DiagSnPrintf+0x34>
2005f5f0:	00b50333          	add	t1,a0,a1
2005f5f4:	200643b7          	lui	t2,0x20064
2005f5f8:	6405                	c.lui	s0,0x1
2005f5fa:	882a                	c.mv	a6,a0
2005f5fc:	05000f13          	addi	t5,zero,80
2005f600:	06400913          	addi	s2,zero,100
2005f604:	06300993          	addi	s3,zero,99
2005f608:	4fa9                	c.li	t6,10
2005f60a:	03000293          	addi	t0,zero,48
2005f60e:	02d00a13          	addi	s4,zero,45
2005f612:	b6038393          	addi	t2,t2,-1184 # 20063b60 <TaskStateString.0+0xa4>
2005f616:	4aa5                	c.li	s5,9
2005f618:	0ef00b13          	addi	s6,zero,239
2005f61c:	eff40b93          	addi	s7,s0,-257 # eff <CPU_ClkGet+0x3e9>
2005f620:	6c3d                	c.lui	s8,0xf
2005f622:	7cc1                	c.lui	s9,0xffff0
2005f624:	00064783          	lbu	a5,0(a2)
2005f628:	e399                	c.bnez	a5,2005f62e <DiagSnPrintf+0x6e>
2005f62a:	8742                	c.mv	a4,a6
2005f62c:	a889                	c.j	2005f67e <DiagSnPrintf+0xbe>
2005f62e:	02500713          	addi	a4,zero,37
2005f632:	00e78b63          	beq	a5,a4,2005f648 <DiagSnPrintf+0x88>
2005f636:	00180713          	addi	a4,a6,1 # fff00001 <__ctrace_end__+0x35f00001>
2005f63a:	00f80023          	sb	a5,0(a6)
2005f63e:	04677063          	bgeu	a4,t1,2005f67e <DiagSnPrintf+0xbe>
2005f642:	0605                	c.addi	a2,1
2005f644:	883a                	c.mv	a6,a4
2005f646:	bff9                	c.j	2005f624 <DiagSnPrintf+0x64>
2005f648:	4722                	c.lwsp	a4,8(sp)
2005f64a:	00164e03          	lbu	t3,1(a2)
2005f64e:	07300693          	addi	a3,zero,115
2005f652:	431c                	c.lw	a5,0(a4)
2005f654:	00160593          	addi	a1,a2,1
2005f658:	0711                	c.addi	a4,4
2005f65a:	04de1463          	bne	t3,a3,2005f6a2 <DiagSnPrintf+0xe2>
2005f65e:	c43a                	c.swsp	a4,8(sp)
2005f660:	410787b3          	sub	a5,a5,a6
2005f664:	8742                	c.mv	a4,a6
2005f666:	00f706b3          	add	a3,a4,a5
2005f66a:	0006c683          	lbu	a3,0(a3)
2005f66e:	e299                	c.bnez	a3,2005f674 <DiagSnPrintf+0xb4>
2005f670:	862e                	c.mv	a2,a1
2005f672:	bfc1                	c.j	2005f642 <DiagSnPrintf+0x82>
2005f674:	0705                	c.addi	a4,1
2005f676:	fed70fa3          	sb	a3,-1(a4)
2005f67a:	fe6766e3          	bltu	a4,t1,2005f666 <DiagSnPrintf+0xa6>
2005f67e:	00070023          	sb	zero,0(a4)
2005f682:	40a70533          	sub	a0,a4,a0
2005f686:	4436                	c.lwsp	s0,76(sp)
2005f688:	44a6                	c.lwsp	s1,72(sp)
2005f68a:	4916                	c.lwsp	s2,68(sp)
2005f68c:	4986                	c.lwsp	s3,64(sp)
2005f68e:	5a72                	c.lwsp	s4,60(sp)
2005f690:	5ae2                	c.lwsp	s5,56(sp)
2005f692:	5b52                	c.lwsp	s6,52(sp)
2005f694:	5bc2                	c.lwsp	s7,48(sp)
2005f696:	5c32                	c.lwsp	s8,44(sp)
2005f698:	5ca2                	c.lwsp	s9,40(sp)
2005f69a:	5d12                	c.lwsp	s10,36(sp)
2005f69c:	5d82                	c.lwsp	s11,32(sp)
2005f69e:	6165                	c.addi16sp	sp,112
2005f6a0:	8082                	c.jr	ra
2005f6a2:	c43a                	c.swsp	a4,8(sp)
2005f6a4:	0dfe7693          	andi	a3,t3,223
2005f6a8:	473d                	c.li	a4,15
2005f6aa:	0de69563          	bne	a3,t5,2005f774 <DiagSnPrintf+0x1b4>
2005f6ae:	0cf75a63          	bge	a4,a5,2005f782 <DiagSnPrintf+0x1c2>
2005f6b2:	46a1                	c.li	a3,8
2005f6b4:	03000893          	addi	a7,zero,48
2005f6b8:	ff078e93          	addi	t4,a5,-16
2005f6bc:	4711                	c.li	a4,4
2005f6be:	05db7463          	bgeu	s6,t4,2005f706 <DiagSnPrintf+0x146>
2005f6c2:	f0078e93          	addi	t4,a5,-256
2005f6c6:	4721                	c.li	a4,8
2005f6c8:	03dbff63          	bgeu	s7,t4,2005f706 <DiagSnPrintf+0x146>
2005f6cc:	40878eb3          	sub	t4,a5,s0
2005f6d0:	4731                	c.li	a4,12
2005f6d2:	038eea63          	bltu	t4,s8,2005f706 <DiagSnPrintf+0x146>
2005f6d6:	01978eb3          	add	t4,a5,s9
2005f6da:	000f04b7          	lui	s1,0xf0
2005f6de:	4741                	c.li	a4,16
2005f6e0:	029ee363          	bltu	t4,s1,2005f706 <DiagSnPrintf+0x146>
2005f6e4:	fff00eb7          	lui	t4,0xfff00
2005f6e8:	9ebe                	c.add	t4,a5
2005f6ea:	00f004b7          	lui	s1,0xf00
2005f6ee:	4751                	c.li	a4,20
2005f6f0:	009eeb63          	bltu	t4,s1,2005f706 <DiagSnPrintf+0x146>
2005f6f4:	ff000eb7          	lui	t4,0xff000
2005f6f8:	9ebe                	c.add	t4,a5
2005f6fa:	0f0004b7          	lui	s1,0xf000
2005f6fe:	4761                	c.li	a4,24
2005f700:	009ee363          	bltu	t4,s1,2005f706 <DiagSnPrintf+0x146>
2005f704:	4771                	c.li	a4,28
2005f706:	005e1663          	bne	t3,t0,2005f712 <DiagSnPrintf+0x152>
2005f70a:	00260593          	addi	a1,a2,2
2005f70e:	03000893          	addi	a7,zero,48
2005f712:	0005c603          	lbu	a2,0(a1)
2005f716:	44a1                	c.li	s1,8
2005f718:	fcf60e13          	addi	t3,a2,-49
2005f71c:	0ffe7e13          	andi	t3,t3,255
2005f720:	03c4e163          	bltu	s1,t3,2005f742 <DiagSnPrintf+0x182>
2005f724:	4681                	c.li	a3,0
2005f726:	03f686b3          	mul	a3,a3,t6
2005f72a:	0585                	c.addi	a1,1
2005f72c:	96b2                	c.add	a3,a2
2005f72e:	0005c603          	lbu	a2,0(a1)
2005f732:	fd068693          	addi	a3,a3,-48
2005f736:	fd060e13          	addi	t3,a2,-48
2005f73a:	0ffe7e13          	andi	t3,t3,255
2005f73e:	ffcaf4e3          	bgeu	s5,t3,2005f726 <DiagSnPrintf+0x166>
2005f742:	0005c603          	lbu	a2,0(a1)
2005f746:	0d767e13          	andi	t3,a2,215
2005f74a:	0bee1463          	bne	t3,t5,2005f7f2 <DiagSnPrintf+0x232>
2005f74e:	02067493          	andi	s1,a2,32
2005f752:	0df67613          	andi	a2,a2,223
2005f756:	8ebe                	c.mv	t4,a5
2005f758:	00c10e13          	addi	t3,sp,12
2005f75c:	01e61a63          	bne	a2,t5,2005f770 <DiagSnPrintf+0x1b0>
2005f760:	0584e613          	ori	a2,s1,88
2005f764:	00510623          	sb	t0,12(sp)
2005f768:	00c106a3          	sb	a2,13(sp)
2005f76c:	00e10e13          	addi	t3,sp,14
2005f770:	4601                	c.li	a2,0
2005f772:	a831                	c.j	2005f78e <DiagSnPrintf+0x1ce>
2005f774:	4681                	c.li	a3,0
2005f776:	02000893          	addi	a7,zero,32
2005f77a:	f2f74fe3          	blt	a4,a5,2005f6b8 <DiagSnPrintf+0xf8>
2005f77e:	4701                	c.li	a4,0
2005f780:	b759                	c.j	2005f706 <DiagSnPrintf+0x146>
2005f782:	4701                	c.li	a4,0
2005f784:	46a1                	c.li	a3,8
2005f786:	b761                	c.j	2005f70e <DiagSnPrintf+0x14e>
2005f788:	0605                	c.addi	a2,1
2005f78a:	004ede93          	srli	t4,t4,0x4
2005f78e:	fe0e9de3          	bne	t4,zero,2005f788 <DiagSnPrintf+0x1c8>
2005f792:	8ef2                	c.mv	t4,t3
2005f794:	00de0db3          	add	s11,t3,a3
2005f798:	41dd8d33          	sub	s10,s11,t4
2005f79c:	05a64763          	blt	a2,s10,2005f7ea <DiagSnPrintf+0x22a>
2005f7a0:	4881                	c.li	a7,0
2005f7a2:	00c6c463          	blt	a3,a2,2005f7aa <DiagSnPrintf+0x1ea>
2005f7a6:	40c688b3          	sub	a7,a3,a2
2005f7aa:	9e46                	c.add	t3,a7
2005f7ac:	88f2                	c.mv	a7,t3
2005f7ae:	863a                	c.mv	a2,a4
2005f7b0:	40c7d6b3          	sra	a3,a5,a2
2005f7b4:	8abd                	c.andi	a3,15
2005f7b6:	969e                	c.add	a3,t2
2005f7b8:	0006c683          	lbu	a3,0(a3)
2005f7bc:	1671                	c.addi	a2,-4
2005f7be:	0885                	c.addi	a7,1
2005f7c0:	8ec5                	c.or	a3,s1
2005f7c2:	fed88fa3          	sb	a3,-1(a7)
2005f7c6:	fe0655e3          	bge	a2,zero,2005f7b0 <DiagSnPrintf+0x1f0>
2005f7ca:	8309                	c.srli	a4,0x2
2005f7cc:	0705                	c.addi	a4,1
2005f7ce:	9e3a                	c.add	t3,a4
2005f7d0:	8742                	c.mv	a4,a6
2005f7d2:	007c                	c.addi4spn	a5,sp,12
2005f7d4:	e9c7fee3          	bgeu	a5,t3,2005f670 <DiagSnPrintf+0xb0>
2005f7d8:	0007c683          	lbu	a3,0(a5)
2005f7dc:	0705                	c.addi	a4,1
2005f7de:	fed70fa3          	sb	a3,-1(a4)
2005f7e2:	e8677ee3          	bgeu	a4,t1,2005f67e <DiagSnPrintf+0xbe>
2005f7e6:	0785                	c.addi	a5,1
2005f7e8:	b7f5                	c.j	2005f7d4 <DiagSnPrintf+0x214>
2005f7ea:	011e8023          	sb	a7,0(t4) # ff000000 <__ctrace_end__+0x35000000>
2005f7ee:	0e85                	c.addi	t4,1
2005f7f0:	b765                	c.j	2005f798 <DiagSnPrintf+0x1d8>
2005f7f2:	07261363          	bne	a2,s2,2005f858 <DiagSnPrintf+0x298>
2005f7f6:	0407de63          	bge	a5,zero,2005f852 <DiagSnPrintf+0x292>
2005f7fa:	01410623          	sb	s4,12(sp)
2005f7fe:	40f007b3          	sub	a5,zero,a5
2005f802:	4885                	c.li	a7,1
2005f804:	00d10693          	addi	a3,sp,13
2005f808:	8e36                	c.mv	t3,a3
2005f80a:	03f7e633          	rem	a2,a5,t6
2005f80e:	0e05                	c.addi	t3,1
2005f810:	03f7c7b3          	div	a5,a5,t6
2005f814:	03060613          	addi	a2,a2,48
2005f818:	fece0fa3          	sb	a2,-1(t3)
2005f81c:	011e0633          	add	a2,t3,a7
2005f820:	8e15                	c.sub	a2,a3
2005f822:	f7e5                	c.bnez	a5,2005f80a <DiagSnPrintf+0x24a>
2005f824:	00c04563          	blt	zero,a2,2005f82e <DiagSnPrintf+0x26e>
2005f828:	005e0023          	sb	t0,0(t3)
2005f82c:	0e05                	c.addi	t3,1
2005f82e:	1771                	c.addi	a4,-4
2005f830:	167d                	c.addi	a2,-1
2005f832:	fe0759e3          	bge	a4,zero,2005f824 <DiagSnPrintf+0x264>
2005f836:	87f2                	c.mv	a5,t3
2005f838:	17fd                	c.addi	a5,-1
2005f83a:	f8f6fbe3          	bgeu	a3,a5,2005f7d0 <DiagSnPrintf+0x210>
2005f83e:	0006c603          	lbu	a2,0(a3)
2005f842:	0007c703          	lbu	a4,0(a5)
2005f846:	0685                	c.addi	a3,1
2005f848:	00c78023          	sb	a2,0(a5)
2005f84c:	fee68fa3          	sb	a4,-1(a3)
2005f850:	b7e5                	c.j	2005f838 <DiagSnPrintf+0x278>
2005f852:	4881                	c.li	a7,0
2005f854:	0074                	c.addi4spn	a3,sp,12
2005f856:	bf4d                	c.j	2005f808 <DiagSnPrintf+0x248>
2005f858:	01361763          	bne	a2,s3,2005f866 <DiagSnPrintf+0x2a6>
2005f85c:	00f10623          	sb	a5,12(sp)
2005f860:	00d10e13          	addi	t3,sp,13
2005f864:	b7b5                	c.j	2005f7d0 <DiagSnPrintf+0x210>
2005f866:	00c10623          	sb	a2,12(sp)
2005f86a:	bfdd                	c.j	2005f860 <DiagSnPrintf+0x2a0>
2005f86c:	4501                	c.li	a0,0
2005f86e:	bd21                	c.j	2005f686 <DiagSnPrintf+0xc6>

2005f870 <TRNG_get_random_bytes>:
2005f870:	7179                	c.addi16sp	sp,-48
2005f872:	d422                	c.swsp	s0,40(sp)
2005f874:	d226                	c.swsp	s1,36(sp)
2005f876:	d606                	c.swsp	ra,44(sp)
2005f878:	d04a                	c.swsp	s2,32(sp)
2005f87a:	ce4e                	c.swsp	s3,28(sp)
2005f87c:	4100c7b7          	lui	a5,0x4100c
2005f880:	2287a783          	lw	a5,552(a5) # 4100c228 <__km4_bd_boot_download_addr__+0x10ffa228>
2005f884:	84aa                	c.mv	s1,a0
2005f886:	842e                	c.mv	s0,a1
2005f888:	8399                	c.srli	a5,0x6
2005f88a:	8b8d                	c.andi	a5,3
2005f88c:	ef9d                	c.bnez	a5,2005f8ca <TRNG_get_random_bytes+0x5a>
2005f88e:	00357793          	andi	a5,a0,3
2005f892:	cf85                	c.beqz	a5,2005f8ca <TRNG_get_random_bytes+0x5a>
2005f894:	852e                	c.mv	a0,a1
2005f896:	e47fb0ef          	jal	ra,2005b6dc <rtos_mem_malloc>
2005f89a:	85a2                	c.mv	a1,s0
2005f89c:	892a                	c.mv	s2,a0
2005f89e:	dffa9097          	auipc	ra,0xdffa9
2005f8a2:	4cc080e7          	jalr	ra,1228(ra) # 8d6a <RandBytes_Get>
2005f8a6:	8622                	c.mv	a2,s0
2005f8a8:	85ca                	c.mv	a1,s2
2005f8aa:	8526                	c.mv	a0,s1
2005f8ac:	dffaa097          	auipc	ra,0xdffaa
2005f8b0:	d66080e7          	jalr	ra,-666(ra) # 9612 <_memcpy>
2005f8b4:	854a                	c.mv	a0,s2
2005f8b6:	e51fb0ef          	jal	ra,2005b706 <rtos_mem_free>
2005f8ba:	50b2                	c.lwsp	ra,44(sp)
2005f8bc:	5422                	c.lwsp	s0,40(sp)
2005f8be:	5492                	c.lwsp	s1,36(sp)
2005f8c0:	5902                	c.lwsp	s2,32(sp)
2005f8c2:	49f2                	c.lwsp	s3,28(sp)
2005f8c4:	4501                	c.li	a0,0
2005f8c6:	6145                	c.addi16sp	sp,48
2005f8c8:	8082                	c.jr	ra
2005f8ca:	00245913          	srli	s2,s0,0x2
2005f8ce:	4981                	c.li	s3,0
2005f8d0:	0329c463          	blt	s3,s2,2005f8f8 <TRNG_get_random_bytes+0x88>
2005f8d4:	5671                	c.li	a2,-4
2005f8d6:	02c90633          	mul	a2,s2,a2
2005f8da:	9432                	c.add	s0,a2
2005f8dc:	dc79                	c.beqz	s0,2005f8ba <TRNG_get_random_bytes+0x4a>
2005f8de:	f54ff0ef          	jal	ra,2005f032 <_rand>
2005f8e2:	c62a                	c.swsp	a0,12(sp)
2005f8e4:	00291513          	slli	a0,s2,0x2
2005f8e8:	8622                	c.mv	a2,s0
2005f8ea:	006c                	c.addi4spn	a1,sp,12
2005f8ec:	9526                	c.add	a0,s1
2005f8ee:	dffaa097          	auipc	ra,0xdffaa
2005f8f2:	d24080e7          	jalr	ra,-732(ra) # 9612 <_memcpy>
2005f8f6:	b7d1                	c.j	2005f8ba <TRNG_get_random_bytes+0x4a>
2005f8f8:	f3aff0ef          	jal	ra,2005f032 <_rand>
2005f8fc:	00299793          	slli	a5,s3,0x2
2005f900:	97a6                	c.add	a5,s1
2005f902:	c388                	c.sw	a0,0(a5)
2005f904:	0985                	c.addi	s3,1
2005f906:	b7e9                	c.j	2005f8d0 <TRNG_get_random_bytes+0x60>

2005f908 <io_assert_failed>:
2005f908:	862e                	c.mv	a2,a1
2005f90a:	85aa                	c.mv	a1,a0
2005f90c:	20064537          	lui	a0,0x20064
2005f910:	1141                	c.addi	sp,-16
2005f912:	b7450513          	addi	a0,a0,-1164 # 20063b74 <TaskStateString.0+0xb8>
2005f916:	c606                	c.swsp	ra,12(sp)
2005f918:	aebff0ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005f91c:	a001                	c.j	2005f91c <io_assert_failed+0x14>

2005f91e <irq_set_priority>:
2005f91e:	471d                	c.li	a4,7
2005f920:	02b77563          	bgeu	a4,a1,2005f94a <irq_set_priority+0x2c>
2005f924:	862e                	c.mv	a2,a1
2005f926:	85aa                	c.mv	a1,a0
2005f928:	20064537          	lui	a0,0x20064
2005f92c:	1141                	c.addi	sp,-16
2005f92e:	469d                	c.li	a3,7
2005f930:	c5050513          	addi	a0,a0,-944 # 20063c50 <TaskStateString.0+0x194>
2005f934:	c606                	c.swsp	ra,12(sp)
2005f936:	acdff0ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2005f93a:	20064537          	lui	a0,0x20064
2005f93e:	44800593          	addi	a1,zero,1096
2005f942:	ba850513          	addi	a0,a0,-1112 # 20063ba8 <TaskStateString.0+0xec>
2005f946:	fc3ff0ef          	jal	ra,2005f908 <io_assert_failed>
2005f94a:	73e00067          	jalr	zero,1854(zero) # 0 <Reset_Handler>

2005f94e <irq_register>:
2005f94e:	20000737          	lui	a4,0x20000
2005f952:	1141                	c.addi	sp,-16
2005f954:	00259893          	slli	a7,a1,0x2
2005f958:	882a                	c.mv	a6,a0
2005f95a:	00070713          	addi	a4,a4,0 # 20000000 <UserIrqFunTable>
2005f95e:	200007b7          	lui	a5,0x20000
2005f962:	c606                	c.swsp	ra,12(sp)
2005f964:	9746                	c.add	a4,a7
2005f966:	00186813          	ori	a6,a6,1
2005f96a:	1c078793          	addi	a5,a5,448 # 200001c0 <UserIrqDataTable>
2005f96e:	01072023          	sw	a6,0(a4)
2005f972:	97c6                	c.add	a5,a7
2005f974:	852e                	c.mv	a0,a1
2005f976:	c390                	c.sw	a2,0(a5)
2005f978:	85b6                	c.mv	a1,a3
2005f97a:	fa5ff0ef          	jal	ra,2005f91e <irq_set_priority>
2005f97e:	40b2                	c.lwsp	ra,12(sp)
2005f980:	4505                	c.li	a0,1
2005f982:	0141                	c.addi	sp,16
2005f984:	8082                	c.jr	ra

2005f986 <unwind_frame_rv32i>:
2005f986:	715d                	c.addi16sp	sp,-80
2005f988:	c686                	c.swsp	ra,76(sp)
2005f98a:	c4a2                	c.swsp	s0,72(sp)
2005f98c:	0880                	c.addi4spn	s0,sp,80
2005f98e:	faa42e23          	sw	a0,-68(s0)
2005f992:	fab42c23          	sw	a1,-72(s0)
2005f996:	fe042623          	sw	zero,-20(s0)
2005f99a:	fe042423          	sw	zero,-24(s0)
2005f99e:	fbc42783          	lw	a5,-68(s0)
2005f9a2:	43dc                	c.lw	a5,4(a5)
2005f9a4:	c789                	c.beqz	a5,2005f9ae <unwind_frame_rv32i+0x28>
2005f9a6:	fbc42783          	lw	a5,-68(s0)
2005f9aa:	439c                	c.lw	a5,0(a5)
2005f9ac:	e399                	c.bnez	a5,2005f9b2 <unwind_frame_rv32i+0x2c>
2005f9ae:	57fd                	c.li	a5,-1
2005f9b0:	a44d                	c.j	2005fc52 <unwind_frame_rv32i+0x2cc>
2005f9b2:	fbc42783          	lw	a5,-68(s0)
2005f9b6:	43dc                	c.lw	a5,4(a5)
2005f9b8:	fef42223          	sw	a5,-28(s0)
2005f9bc:	a435                	c.j	2005fbe8 <unwind_frame_rv32i+0x262>
2005f9be:	fe442783          	lw	a5,-28(s0)
2005f9c2:	8b89                	c.andi	a5,2
2005f9c4:	c385                	c.beqz	a5,2005f9e4 <unwind_frame_rv32i+0x5e>
2005f9c6:	fe442783          	lw	a5,-28(s0)
2005f9ca:	0007d783          	lhu	a5,0(a5)
2005f9ce:	873e                	c.mv	a4,a5
2005f9d0:	fe442783          	lw	a5,-28(s0)
2005f9d4:	0789                	c.addi	a5,2
2005f9d6:	0007d783          	lhu	a5,0(a5)
2005f9da:	07c2                	c.slli	a5,0x10
2005f9dc:	8fd9                	c.or	a5,a4
2005f9de:	fcf42c23          	sw	a5,-40(s0)
2005f9e2:	a031                	c.j	2005f9ee <unwind_frame_rv32i+0x68>
2005f9e4:	fe442783          	lw	a5,-28(s0)
2005f9e8:	439c                	c.lw	a5,0(a5)
2005f9ea:	fcf42c23          	sw	a5,-40(s0)
2005f9ee:	fe442783          	lw	a5,-28(s0)
2005f9f2:	0007d783          	lhu	a5,0(a5)
2005f9f6:	fcf41a23          	sh	a5,-44(s0)
2005f9fa:	fe442783          	lw	a5,-28(s0)
2005f9fe:	0789                	c.addi	a5,2
2005fa00:	0007d783          	lhu	a5,0(a5)
2005fa04:	fcf41823          	sh	a5,-48(s0)
2005fa08:	fd845783          	lhu	a5,-40(s0)
2005fa0c:	07f7f793          	andi	a5,a5,127
2005fa10:	01079713          	slli	a4,a5,0x10
2005fa14:	8341                	c.srli	a4,0x10
2005fa16:	47cd                	c.li	a5,19
2005fa18:	0af71c63          	bne	a4,a5,2005fad0 <unwind_frame_rv32i+0x14a>
2005fa1c:	fd845703          	lhu	a4,-40(s0)
2005fa20:	679d                	c.lui	a5,0x7
2005fa22:	8ff9                	c.and	a5,a4
2005fa24:	07c2                	c.slli	a5,0x10
2005fa26:	83c1                	c.srli	a5,0x10
2005fa28:	e7c5                	c.bnez	a5,2005fad0 <unwind_frame_rv32i+0x14a>
2005fa2a:	fd842703          	lw	a4,-40(s0)
2005fa2e:	6785                	c.lui	a5,0x1
2005fa30:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fa34:	8f7d                	c.and	a4,a5
2005fa36:	10000793          	addi	a5,zero,256
2005fa3a:	18f71f63          	bne	a4,a5,2005fbd8 <unwind_frame_rv32i+0x252>
2005fa3e:	fd842703          	lw	a4,-40(s0)
2005fa42:	000f87b7          	lui	a5,0xf8
2005fa46:	8f7d                	c.and	a4,a5
2005fa48:	67c1                	c.lui	a5,0x10
2005fa4a:	18f71763          	bne	a4,a5,2005fbd8 <unwind_frame_rv32i+0x252>
2005fa4e:	fd842783          	lw	a5,-40(s0)
2005fa52:	83d1                	c.srli	a5,0x14
2005fa54:	07c2                	c.slli	a5,0x10
2005fa56:	83c1                	c.srli	a5,0x10
2005fa58:	0792                	c.slli	a5,0x4
2005fa5a:	07c2                	c.slli	a5,0x10
2005fa5c:	87c1                	c.srai	a5,0x10
2005fa5e:	8791                	c.srai	a5,0x4
2005fa60:	07c2                	c.slli	a5,0x10
2005fa62:	87c1                	c.srai	a5,0x10
2005fa64:	873e                	c.mv	a4,a5
2005fa66:	6785                	c.lui	a5,0x1
2005fa68:	17fd                	c.addi	a5,-1
2005fa6a:	00f776b3          	and	a3,a4,a5
2005fa6e:	fcc45703          	lhu	a4,-52(s0)
2005fa72:	77fd                	c.lui	a5,0xfffff
2005fa74:	8ff9                	c.and	a5,a4
2005fa76:	873e                	c.mv	a4,a5
2005fa78:	87b6                	c.mv	a5,a3
2005fa7a:	8fd9                	c.or	a5,a4
2005fa7c:	fcf41623          	sh	a5,-52(s0)
2005fa80:	fcc42783          	lw	a5,-52(s0)
2005fa84:	0792                	c.slli	a5,0x4
2005fa86:	07c2                	c.slli	a5,0x10
2005fa88:	87c1                	c.srai	a5,0x10
2005fa8a:	8791                	c.srai	a5,0x4
2005fa8c:	07c2                	c.slli	a5,0x10
2005fa8e:	87c1                	c.srai	a5,0x10
2005fa90:	fef42023          	sw	a5,-32(s0)
2005fa94:	fe042783          	lw	a5,-32(s0)
2005fa98:	02f05363          	bge	zero,a5,2005fabe <unwind_frame_rv32i+0x138>
2005fa9c:	fe842783          	lw	a5,-24(s0)
2005faa0:	86be                	c.mv	a3,a5
2005faa2:	10d00613          	addi	a2,zero,269
2005faa6:	200647b7          	lui	a5,0x20064
2005faaa:	e7478593          	addi	a1,a5,-396 # 20063e74 <__FUNCTION__.3>
2005faae:	200647b7          	lui	a5,0x20064
2005fab2:	c7478513          	addi	a0,a5,-908 # 20063c74 <TaskStateString.0+0x1b8>
2005fab6:	92dff0ef          	jal	ra,2005f3e2 <DiagPrintf>
2005faba:	57fd                	c.li	a5,-1
2005fabc:	aa59                	c.j	2005fc52 <unwind_frame_rv32i+0x2cc>
2005fabe:	fe042783          	lw	a5,-32(s0)
2005fac2:	0007d463          	bge	a5,zero,2005faca <unwind_frame_rv32i+0x144>
2005fac6:	40f007b3          	sub	a5,zero,a5
2005faca:	fef42423          	sw	a5,-24(s0)
2005face:	a229                	c.j	2005fbd8 <unwind_frame_rv32i+0x252>
2005fad0:	fd845783          	lhu	a5,-40(s0)
2005fad4:	07f7f793          	andi	a5,a5,127
2005fad8:	01079713          	slli	a4,a5,0x10
2005fadc:	8341                	c.srli	a4,0x10
2005fade:	02300793          	addi	a5,zero,35
2005fae2:	0cf71463          	bne	a4,a5,2005fbaa <unwind_frame_rv32i+0x224>
2005fae6:	fd845703          	lhu	a4,-40(s0)
2005faea:	679d                	c.lui	a5,0x7
2005faec:	8ff9                	c.and	a5,a4
2005faee:	01079713          	slli	a4,a5,0x10
2005faf2:	8341                	c.srli	a4,0x10
2005faf4:	6789                	c.lui	a5,0x2
2005faf6:	0af71a63          	bne	a4,a5,2005fbaa <unwind_frame_rv32i+0x224>
2005fafa:	fd842703          	lw	a4,-40(s0)
2005fafe:	000f87b7          	lui	a5,0xf8
2005fb02:	8f7d                	c.and	a4,a5
2005fb04:	67c1                	c.lui	a5,0x10
2005fb06:	0cf71b63          	bne	a4,a5,2005fbdc <unwind_frame_rv32i+0x256>
2005fb0a:	fd842703          	lw	a4,-40(s0)
2005fb0e:	01f007b7          	lui	a5,0x1f00
2005fb12:	8f7d                	c.and	a4,a5
2005fb14:	001007b7          	lui	a5,0x100
2005fb18:	0cf71263          	bne	a4,a5,2005fbdc <unwind_frame_rv32i+0x256>
2005fb1c:	fd842783          	lw	a5,-40(s0)
2005fb20:	83e5                	c.srli	a5,0x19
2005fb22:	0ff7f793          	andi	a5,a5,255
2005fb26:	0796                	c.slli	a5,0x5
2005fb28:	01079713          	slli	a4,a5,0x10
2005fb2c:	8741                	c.srai	a4,0x10
2005fb2e:	fd842783          	lw	a5,-40(s0)
2005fb32:	839d                	c.srli	a5,0x7
2005fb34:	8bfd                	c.andi	a5,31
2005fb36:	0ff7f793          	andi	a5,a5,255
2005fb3a:	07c2                	c.slli	a5,0x10
2005fb3c:	87c1                	c.srai	a5,0x10
2005fb3e:	8fd9                	c.or	a5,a4
2005fb40:	07c2                	c.slli	a5,0x10
2005fb42:	87c1                	c.srai	a5,0x10
2005fb44:	0792                	c.slli	a5,0x4
2005fb46:	07c2                	c.slli	a5,0x10
2005fb48:	87c1                	c.srai	a5,0x10
2005fb4a:	8791                	c.srai	a5,0x4
2005fb4c:	07c2                	c.slli	a5,0x10
2005fb4e:	87c1                	c.srai	a5,0x10
2005fb50:	873e                	c.mv	a4,a5
2005fb52:	6785                	c.lui	a5,0x1
2005fb54:	17fd                	c.addi	a5,-1
2005fb56:	00f776b3          	and	a3,a4,a5
2005fb5a:	fc845703          	lhu	a4,-56(s0)
2005fb5e:	77fd                	c.lui	a5,0xfffff
2005fb60:	8ff9                	c.and	a5,a4
2005fb62:	873e                	c.mv	a4,a5
2005fb64:	87b6                	c.mv	a5,a3
2005fb66:	8fd9                	c.or	a5,a4
2005fb68:	fcf41423          	sh	a5,-56(s0)
2005fb6c:	fc842783          	lw	a5,-56(s0)
2005fb70:	0792                	c.slli	a5,0x4
2005fb72:	07c2                	c.slli	a5,0x10
2005fb74:	87c1                	c.srai	a5,0x10
2005fb76:	8791                	c.srai	a5,0x4
2005fb78:	07c2                	c.slli	a5,0x10
2005fb7a:	87c1                	c.srai	a5,0x10
2005fb7c:	fef42623          	sw	a5,-20(s0)
2005fb80:	fec42783          	lw	a5,-20(s0)
2005fb84:	0407dc63          	bge	a5,zero,2005fbdc <unwind_frame_rv32i+0x256>
2005fb88:	fec42783          	lw	a5,-20(s0)
2005fb8c:	86be                	c.mv	a3,a5
2005fb8e:	11d00613          	addi	a2,zero,285
2005fb92:	200647b7          	lui	a5,0x20064
2005fb96:	e7478593          	addi	a1,a5,-396 # 20063e74 <__FUNCTION__.3>
2005fb9a:	200647b7          	lui	a5,0x20064
2005fb9e:	cb078513          	addi	a0,a5,-848 # 20063cb0 <TaskStateString.0+0x1f4>
2005fba2:	841ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
2005fba6:	57fd                	c.li	a5,-1
2005fba8:	a06d                	c.j	2005fc52 <unwind_frame_rv32i+0x2cc>
2005fbaa:	fd842703          	lw	a4,-40(s0)
2005fbae:	67a1                	c.lui	a5,0x8
2005fbb0:	06778793          	addi	a5,a5,103 # 8067 <irq_unregister+0x41>
2005fbb4:	06f70063          	beq	a4,a5,2005fc14 <unwind_frame_rv32i+0x28e>
2005fbb8:	fd445703          	lhu	a4,-44(s0)
2005fbbc:	67a1                	c.lui	a5,0x8
2005fbbe:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fbc2:	00f70963          	beq	a4,a5,2005fbd4 <unwind_frame_rv32i+0x24e>
2005fbc6:	fd045703          	lhu	a4,-48(s0)
2005fbca:	67a1                	c.lui	a5,0x8
2005fbcc:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005fbd0:	00f71763          	bne	a4,a5,2005fbde <unwind_frame_rv32i+0x258>
2005fbd4:	57fd                	c.li	a5,-1
2005fbd6:	a8b5                	c.j	2005fc52 <unwind_frame_rv32i+0x2cc>
2005fbd8:	0001                	c.addi	zero,0
2005fbda:	a011                	c.j	2005fbde <unwind_frame_rv32i+0x258>
2005fbdc:	0001                	c.addi	zero,0
2005fbde:	fe442783          	lw	a5,-28(s0)
2005fbe2:	17f1                	c.addi	a5,-4
2005fbe4:	fef42223          	sw	a5,-28(s0)
2005fbe8:	fb842783          	lw	a5,-72(s0)
2005fbec:	078a                	c.slli	a5,0x2
2005fbee:	fe442703          	lw	a4,-28(s0)
2005fbf2:	973e                	c.add	a4,a5
2005fbf4:	fbc42783          	lw	a5,-68(s0)
2005fbf8:	43dc                	c.lw	a5,4(a5)
2005fbfa:	00e7fe63          	bgeu	a5,a4,2005fc16 <unwind_frame_rv32i+0x290>
2005fbfe:	fec42783          	lw	a5,-20(s0)
2005fc02:	c781                	c.beqz	a5,2005fc0a <unwind_frame_rv32i+0x284>
2005fc04:	fe842783          	lw	a5,-24(s0)
2005fc08:	e799                	c.bnez	a5,2005fc16 <unwind_frame_rv32i+0x290>
2005fc0a:	fe442783          	lw	a5,-28(s0)
2005fc0e:	da0798e3          	bne	a5,zero,2005f9be <unwind_frame_rv32i+0x38>
2005fc12:	a011                	c.j	2005fc16 <unwind_frame_rv32i+0x290>
2005fc14:	0001                	c.addi	zero,0
2005fc16:	fec42783          	lw	a5,-20(s0)
2005fc1a:	cb91                	c.beqz	a5,2005fc2e <unwind_frame_rv32i+0x2a8>
2005fc1c:	fbc42783          	lw	a5,-68(s0)
2005fc20:	4398                	c.lw	a4,0(a5)
2005fc22:	fec42783          	lw	a5,-20(s0)
2005fc26:	97ba                	c.add	a5,a4
2005fc28:	439c                	c.lw	a5,0(a5)
2005fc2a:	873e                	c.mv	a4,a5
2005fc2c:	a011                	c.j	2005fc30 <unwind_frame_rv32i+0x2aa>
2005fc2e:	4701                	c.li	a4,0
2005fc30:	fbc42783          	lw	a5,-68(s0)
2005fc34:	c798                	c.sw	a4,8(a5)
2005fc36:	fe842703          	lw	a4,-24(s0)
2005fc3a:	fbc42783          	lw	a5,-68(s0)
2005fc3e:	c7d8                	c.sw	a4,12(a5)
2005fc40:	fec42783          	lw	a5,-20(s0)
2005fc44:	c781                	c.beqz	a5,2005fc4c <unwind_frame_rv32i+0x2c6>
2005fc46:	fe842783          	lw	a5,-24(s0)
2005fc4a:	e399                	c.bnez	a5,2005fc50 <unwind_frame_rv32i+0x2ca>
2005fc4c:	57fd                	c.li	a5,-1
2005fc4e:	a011                	c.j	2005fc52 <unwind_frame_rv32i+0x2cc>
2005fc50:	4781                	c.li	a5,0
2005fc52:	853e                	c.mv	a0,a5
2005fc54:	40b6                	c.lwsp	ra,76(sp)
2005fc56:	4426                	c.lwsp	s0,72(sp)
2005fc58:	6161                	c.addi16sp	sp,80
2005fc5a:	8082                	c.jr	ra

2005fc5c <unwind_frame_rv32c>:
2005fc5c:	715d                	c.addi16sp	sp,-80
2005fc5e:	c686                	c.swsp	ra,76(sp)
2005fc60:	c4a2                	c.swsp	s0,72(sp)
2005fc62:	0880                	c.addi4spn	s0,sp,80
2005fc64:	faa42e23          	sw	a0,-68(s0)
2005fc68:	fab42c23          	sw	a1,-72(s0)
2005fc6c:	fe042623          	sw	zero,-20(s0)
2005fc70:	fe042423          	sw	zero,-24(s0)
2005fc74:	fbc42783          	lw	a5,-68(s0)
2005fc78:	43dc                	c.lw	a5,4(a5)
2005fc7a:	cb91                	c.beqz	a5,2005fc8e <unwind_frame_rv32c+0x32>
2005fc7c:	fbc42783          	lw	a5,-68(s0)
2005fc80:	439c                	c.lw	a5,0(a5)
2005fc82:	c791                	c.beqz	a5,2005fc8e <unwind_frame_rv32c+0x32>
2005fc84:	fbc42783          	lw	a5,-68(s0)
2005fc88:	43dc                	c.lw	a5,4(a5)
2005fc8a:	8b85                	c.andi	a5,1
2005fc8c:	c399                	c.beqz	a5,2005fc92 <unwind_frame_rv32c+0x36>
2005fc8e:	57fd                	c.li	a5,-1
2005fc90:	ae3d                	c.j	2005ffce <unwind_frame_rv32c+0x372>
2005fc92:	fbc42783          	lw	a5,-68(s0)
2005fc96:	43dc                	c.lw	a5,4(a5)
2005fc98:	fef42223          	sw	a5,-28(s0)
2005fc9c:	a4e1                	c.j	2005ff64 <unwind_frame_rv32c+0x308>
2005fc9e:	fe442783          	lw	a5,-28(s0)
2005fca2:	0007d783          	lhu	a5,0(a5)
2005fca6:	fcf41c23          	sh	a5,-40(s0)
2005fcaa:	fd845703          	lhu	a4,-40(s0)
2005fcae:	77f9                	c.lui	a5,0xffffe
2005fcb0:	8ff9                	c.and	a5,a4
2005fcb2:	07c2                	c.slli	a5,0x10
2005fcb4:	83c1                	c.srli	a5,0x10
2005fcb6:	e7e9                	c.bnez	a5,2005fd80 <unwind_frame_rv32c+0x124>
2005fcb8:	fd845783          	lhu	a5,-40(s0)
2005fcbc:	8b8d                	c.andi	a5,3
2005fcbe:	01079713          	slli	a4,a5,0x10
2005fcc2:	8341                	c.srli	a4,0x10
2005fcc4:	4785                	c.li	a5,1
2005fcc6:	0af71d63          	bne	a4,a5,2005fd80 <unwind_frame_rv32c+0x124>
2005fcca:	fd845703          	lhu	a4,-40(s0)
2005fcce:	6785                	c.lui	a5,0x1
2005fcd0:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fcd4:	8ff9                	c.and	a5,a4
2005fcd6:	01079713          	slli	a4,a5,0x10
2005fcda:	8341                	c.srli	a4,0x10
2005fcdc:	10000793          	addi	a5,zero,256
2005fce0:	0af71063          	bne	a4,a5,2005fd80 <unwind_frame_rv32c+0x124>
2005fce4:	fd842783          	lw	a5,-40(s0)
2005fce8:	83b1                	c.srli	a5,0xc
2005fcea:	8b85                	c.andi	a5,1
2005fcec:	0ff7f793          	andi	a5,a5,255
2005fcf0:	0796                	c.slli	a5,0x5
2005fcf2:	01879713          	slli	a4,a5,0x18
2005fcf6:	8761                	c.srai	a4,0x18
2005fcf8:	fd842783          	lw	a5,-40(s0)
2005fcfc:	8389                	c.srli	a5,0x2
2005fcfe:	8bfd                	c.andi	a5,31
2005fd00:	0ff7f793          	andi	a5,a5,255
2005fd04:	07e2                	c.slli	a5,0x18
2005fd06:	87e1                	c.srai	a5,0x18
2005fd08:	8fd9                	c.or	a5,a4
2005fd0a:	07e2                	c.slli	a5,0x18
2005fd0c:	87e1                	c.srai	a5,0x18
2005fd0e:	078a                	c.slli	a5,0x2
2005fd10:	07e2                	c.slli	a5,0x18
2005fd12:	87e1                	c.srai	a5,0x18
2005fd14:	8789                	c.srai	a5,0x2
2005fd16:	07e2                	c.slli	a5,0x18
2005fd18:	87e1                	c.srai	a5,0x18
2005fd1a:	03f7f693          	andi	a3,a5,63
2005fd1e:	fd444783          	lbu	a5,-44(s0)
2005fd22:	fc07f793          	andi	a5,a5,-64
2005fd26:	873e                	c.mv	a4,a5
2005fd28:	87b6                	c.mv	a5,a3
2005fd2a:	8fd9                	c.or	a5,a4
2005fd2c:	fcf40a23          	sb	a5,-44(s0)
2005fd30:	fd442783          	lw	a5,-44(s0)
2005fd34:	078a                	c.slli	a5,0x2
2005fd36:	07e2                	c.slli	a5,0x18
2005fd38:	87e1                	c.srai	a5,0x18
2005fd3a:	8789                	c.srai	a5,0x2
2005fd3c:	07e2                	c.slli	a5,0x18
2005fd3e:	87e1                	c.srai	a5,0x18
2005fd40:	fef42023          	sw	a5,-32(s0)
2005fd44:	fe042783          	lw	a5,-32(s0)
2005fd48:	02f05363          	bge	zero,a5,2005fd6e <unwind_frame_rv32c+0x112>
2005fd4c:	fe842783          	lw	a5,-24(s0)
2005fd50:	86be                	c.mv	a3,a5
2005fd52:	16900613          	addi	a2,zero,361
2005fd56:	200647b7          	lui	a5,0x20064
2005fd5a:	e8878593          	addi	a1,a5,-376 # 20063e88 <__FUNCTION__.2>
2005fd5e:	200647b7          	lui	a5,0x20064
2005fd62:	c7478513          	addi	a0,a5,-908 # 20063c74 <TaskStateString.0+0x1b8>
2005fd66:	e7cff0ef          	jal	ra,2005f3e2 <DiagPrintf>
2005fd6a:	57fd                	c.li	a5,-1
2005fd6c:	a48d                	c.j	2005ffce <unwind_frame_rv32c+0x372>
2005fd6e:	fe042783          	lw	a5,-32(s0)
2005fd72:	0007d463          	bge	a5,zero,2005fd7a <unwind_frame_rv32c+0x11e>
2005fd76:	40f007b3          	sub	a5,zero,a5
2005fd7a:	fef42423          	sw	a5,-24(s0)
2005fd7e:	aaf1                	c.j	2005ff5a <unwind_frame_rv32c+0x2fe>
2005fd80:	fd845703          	lhu	a4,-40(s0)
2005fd84:	77f9                	c.lui	a5,0xffffe
2005fd86:	8ff9                	c.and	a5,a4
2005fd88:	01079713          	slli	a4,a5,0x10
2005fd8c:	8341                	c.srli	a4,0x10
2005fd8e:	6799                	c.lui	a5,0x6
2005fd90:	10f71e63          	bne	a4,a5,2005feac <unwind_frame_rv32c+0x250>
2005fd94:	fd845783          	lhu	a5,-40(s0)
2005fd98:	8b8d                	c.andi	a5,3
2005fd9a:	01079713          	slli	a4,a5,0x10
2005fd9e:	8341                	c.srli	a4,0x10
2005fda0:	4785                	c.li	a5,1
2005fda2:	10f71563          	bne	a4,a5,2005feac <unwind_frame_rv32c+0x250>
2005fda6:	fd845703          	lhu	a4,-40(s0)
2005fdaa:	6785                	c.lui	a5,0x1
2005fdac:	f8078793          	addi	a5,a5,-128 # f80 <CPU_ClkGet+0x46a>
2005fdb0:	8ff9                	c.and	a5,a4
2005fdb2:	01079713          	slli	a4,a5,0x10
2005fdb6:	8341                	c.srli	a4,0x10
2005fdb8:	10000793          	addi	a5,zero,256
2005fdbc:	0ef71863          	bne	a4,a5,2005feac <unwind_frame_rv32c+0x250>
2005fdc0:	fd842783          	lw	a5,-40(s0)
2005fdc4:	83b1                	c.srli	a5,0xc
2005fdc6:	8b85                	c.andi	a5,1
2005fdc8:	0ff7f793          	andi	a5,a5,255
2005fdcc:	07a6                	c.slli	a5,0x9
2005fdce:	01079713          	slli	a4,a5,0x10
2005fdd2:	8741                	c.srai	a4,0x10
2005fdd4:	fd842783          	lw	a5,-40(s0)
2005fdd8:	8399                	c.srli	a5,0x6
2005fdda:	8b85                	c.andi	a5,1
2005fddc:	0ff7f793          	andi	a5,a5,255
2005fde0:	0792                	c.slli	a5,0x4
2005fde2:	07c2                	c.slli	a5,0x10
2005fde4:	87c1                	c.srai	a5,0x10
2005fde6:	8fd9                	c.or	a5,a4
2005fde8:	01079713          	slli	a4,a5,0x10
2005fdec:	8741                	c.srai	a4,0x10
2005fdee:	fd842783          	lw	a5,-40(s0)
2005fdf2:	8395                	c.srli	a5,0x5
2005fdf4:	8b85                	c.andi	a5,1
2005fdf6:	0ff7f793          	andi	a5,a5,255
2005fdfa:	079a                	c.slli	a5,0x6
2005fdfc:	07c2                	c.slli	a5,0x10
2005fdfe:	87c1                	c.srai	a5,0x10
2005fe00:	8fd9                	c.or	a5,a4
2005fe02:	01079713          	slli	a4,a5,0x10
2005fe06:	8741                	c.srai	a4,0x10
2005fe08:	fd842783          	lw	a5,-40(s0)
2005fe0c:	838d                	c.srli	a5,0x3
2005fe0e:	8b8d                	c.andi	a5,3
2005fe10:	0ff7f793          	andi	a5,a5,255
2005fe14:	079e                	c.slli	a5,0x7
2005fe16:	07c2                	c.slli	a5,0x10
2005fe18:	87c1                	c.srai	a5,0x10
2005fe1a:	8fd9                	c.or	a5,a4
2005fe1c:	01079713          	slli	a4,a5,0x10
2005fe20:	8741                	c.srai	a4,0x10
2005fe22:	fd842783          	lw	a5,-40(s0)
2005fe26:	8389                	c.srli	a5,0x2
2005fe28:	8b85                	c.andi	a5,1
2005fe2a:	0ff7f793          	andi	a5,a5,255
2005fe2e:	0796                	c.slli	a5,0x5
2005fe30:	07c2                	c.slli	a5,0x10
2005fe32:	87c1                	c.srai	a5,0x10
2005fe34:	8fd9                	c.or	a5,a4
2005fe36:	07c2                	c.slli	a5,0x10
2005fe38:	87c1                	c.srai	a5,0x10
2005fe3a:	079a                	c.slli	a5,0x6
2005fe3c:	07c2                	c.slli	a5,0x10
2005fe3e:	87c1                	c.srai	a5,0x10
2005fe40:	8799                	c.srai	a5,0x6
2005fe42:	07c2                	c.slli	a5,0x10
2005fe44:	87c1                	c.srai	a5,0x10
2005fe46:	3ff7f693          	andi	a3,a5,1023
2005fe4a:	fd045783          	lhu	a5,-48(s0)
2005fe4e:	c007f793          	andi	a5,a5,-1024
2005fe52:	873e                	c.mv	a4,a5
2005fe54:	87b6                	c.mv	a5,a3
2005fe56:	8fd9                	c.or	a5,a4
2005fe58:	fcf41823          	sh	a5,-48(s0)
2005fe5c:	fd042783          	lw	a5,-48(s0)
2005fe60:	079a                	c.slli	a5,0x6
2005fe62:	07c2                	c.slli	a5,0x10
2005fe64:	87c1                	c.srai	a5,0x10
2005fe66:	8799                	c.srai	a5,0x6
2005fe68:	07c2                	c.slli	a5,0x10
2005fe6a:	87c1                	c.srai	a5,0x10
2005fe6c:	fcf42e23          	sw	a5,-36(s0)
2005fe70:	fdc42783          	lw	a5,-36(s0)
2005fe74:	02f05363          	bge	zero,a5,2005fe9a <unwind_frame_rv32c+0x23e>
2005fe78:	fe842783          	lw	a5,-24(s0)
2005fe7c:	86be                	c.mv	a3,a5
2005fe7e:	18100613          	addi	a2,zero,385
2005fe82:	200647b7          	lui	a5,0x20064
2005fe86:	e8878593          	addi	a1,a5,-376 # 20063e88 <__FUNCTION__.2>
2005fe8a:	200647b7          	lui	a5,0x20064
2005fe8e:	c7478513          	addi	a0,a5,-908 # 20063c74 <TaskStateString.0+0x1b8>
2005fe92:	d50ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
2005fe96:	57fd                	c.li	a5,-1
2005fe98:	aa1d                	c.j	2005ffce <unwind_frame_rv32c+0x372>
2005fe9a:	fdc42783          	lw	a5,-36(s0)
2005fe9e:	0007d463          	bge	a5,zero,2005fea6 <unwind_frame_rv32c+0x24a>
2005fea2:	40f007b3          	sub	a5,zero,a5
2005fea6:	fef42423          	sw	a5,-24(s0)
2005feaa:	a845                	c.j	2005ff5a <unwind_frame_rv32c+0x2fe>
2005feac:	fd845703          	lhu	a4,-40(s0)
2005feb0:	77f9                	c.lui	a5,0xffffe
2005feb2:	8ff9                	c.and	a5,a4
2005feb4:	01079713          	slli	a4,a5,0x10
2005feb8:	8341                	c.srli	a4,0x10
2005feba:	67b1                	c.lui	a5,0xc
2005febc:	08f71663          	bne	a4,a5,2005ff48 <unwind_frame_rv32c+0x2ec>
2005fec0:	fd845783          	lhu	a5,-40(s0)
2005fec4:	8b8d                	c.andi	a5,3
2005fec6:	01079713          	slli	a4,a5,0x10
2005feca:	8341                	c.srli	a4,0x10
2005fecc:	4789                	c.li	a5,2
2005fece:	06f71d63          	bne	a4,a5,2005ff48 <unwind_frame_rv32c+0x2ec>
2005fed2:	fd845783          	lhu	a5,-40(s0)
2005fed6:	07c7f793          	andi	a5,a5,124
2005feda:	01079713          	slli	a4,a5,0x10
2005fede:	8341                	c.srli	a4,0x10
2005fee0:	4791                	c.li	a5,4
2005fee2:	06f71363          	bne	a4,a5,2005ff48 <unwind_frame_rv32c+0x2ec>
2005fee6:	fd842783          	lw	a5,-40(s0)
2005feea:	839d                	c.srli	a5,0x7
2005feec:	8b8d                	c.andi	a5,3
2005feee:	0ff7f793          	andi	a5,a5,255
2005fef2:	079a                	c.slli	a5,0x6
2005fef4:	01879713          	slli	a4,a5,0x18
2005fef8:	8761                	c.srai	a4,0x18
2005fefa:	fd842783          	lw	a5,-40(s0)
2005fefe:	83a5                	c.srli	a5,0x9
2005ff00:	8bbd                	c.andi	a5,15
2005ff02:	0ff7f793          	andi	a5,a5,255
2005ff06:	078a                	c.slli	a5,0x2
2005ff08:	07e2                	c.slli	a5,0x18
2005ff0a:	87e1                	c.srai	a5,0x18
2005ff0c:	8fd9                	c.or	a5,a4
2005ff0e:	07e2                	c.slli	a5,0x18
2005ff10:	87e1                	c.srai	a5,0x18
2005ff12:	fcf40623          	sb	a5,-52(s0)
2005ff16:	fcc40783          	lb	a5,-52(s0)
2005ff1a:	fef42623          	sw	a5,-20(s0)
2005ff1e:	fec42783          	lw	a5,-20(s0)
2005ff22:	0207db63          	bge	a5,zero,2005ff58 <unwind_frame_rv32c+0x2fc>
2005ff26:	fec42783          	lw	a5,-20(s0)
2005ff2a:	86be                	c.mv	a3,a5
2005ff2c:	19100613          	addi	a2,zero,401
2005ff30:	200647b7          	lui	a5,0x20064
2005ff34:	e8878593          	addi	a1,a5,-376 # 20063e88 <__FUNCTION__.2>
2005ff38:	200647b7          	lui	a5,0x20064
2005ff3c:	cb078513          	addi	a0,a5,-848 # 20063cb0 <TaskStateString.0+0x1f4>
2005ff40:	ca2ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
2005ff44:	57fd                	c.li	a5,-1
2005ff46:	a061                	c.j	2005ffce <unwind_frame_rv32c+0x372>
2005ff48:	fd845703          	lhu	a4,-40(s0)
2005ff4c:	67a1                	c.lui	a5,0x8
2005ff4e:	08278793          	addi	a5,a5,130 # 8082 <WDG_StructInit+0x6>
2005ff52:	02f70f63          	beq	a4,a5,2005ff90 <unwind_frame_rv32c+0x334>
2005ff56:	a011                	c.j	2005ff5a <unwind_frame_rv32c+0x2fe>
2005ff58:	0001                	c.addi	zero,0
2005ff5a:	fe442783          	lw	a5,-28(s0)
2005ff5e:	17f9                	c.addi	a5,-2
2005ff60:	fef42223          	sw	a5,-28(s0)
2005ff64:	fb842783          	lw	a5,-72(s0)
2005ff68:	0786                	c.slli	a5,0x1
2005ff6a:	fe442703          	lw	a4,-28(s0)
2005ff6e:	973e                	c.add	a4,a5
2005ff70:	fbc42783          	lw	a5,-68(s0)
2005ff74:	43dc                	c.lw	a5,4(a5)
2005ff76:	00e7fe63          	bgeu	a5,a4,2005ff92 <unwind_frame_rv32c+0x336>
2005ff7a:	fec42783          	lw	a5,-20(s0)
2005ff7e:	c781                	c.beqz	a5,2005ff86 <unwind_frame_rv32c+0x32a>
2005ff80:	fe842783          	lw	a5,-24(s0)
2005ff84:	e799                	c.bnez	a5,2005ff92 <unwind_frame_rv32c+0x336>
2005ff86:	fe442783          	lw	a5,-28(s0)
2005ff8a:	d0079ae3          	bne	a5,zero,2005fc9e <unwind_frame_rv32c+0x42>
2005ff8e:	a011                	c.j	2005ff92 <unwind_frame_rv32c+0x336>
2005ff90:	0001                	c.addi	zero,0
2005ff92:	fec42783          	lw	a5,-20(s0)
2005ff96:	cb91                	c.beqz	a5,2005ffaa <unwind_frame_rv32c+0x34e>
2005ff98:	fbc42783          	lw	a5,-68(s0)
2005ff9c:	4398                	c.lw	a4,0(a5)
2005ff9e:	fec42783          	lw	a5,-20(s0)
2005ffa2:	97ba                	c.add	a5,a4
2005ffa4:	439c                	c.lw	a5,0(a5)
2005ffa6:	873e                	c.mv	a4,a5
2005ffa8:	a011                	c.j	2005ffac <unwind_frame_rv32c+0x350>
2005ffaa:	4701                	c.li	a4,0
2005ffac:	fbc42783          	lw	a5,-68(s0)
2005ffb0:	c798                	c.sw	a4,8(a5)
2005ffb2:	fe842703          	lw	a4,-24(s0)
2005ffb6:	fbc42783          	lw	a5,-68(s0)
2005ffba:	c7d8                	c.sw	a4,12(a5)
2005ffbc:	fec42783          	lw	a5,-20(s0)
2005ffc0:	c781                	c.beqz	a5,2005ffc8 <unwind_frame_rv32c+0x36c>
2005ffc2:	fe842783          	lw	a5,-24(s0)
2005ffc6:	e399                	c.bnez	a5,2005ffcc <unwind_frame_rv32c+0x370>
2005ffc8:	57fd                	c.li	a5,-1
2005ffca:	a011                	c.j	2005ffce <unwind_frame_rv32c+0x372>
2005ffcc:	4781                	c.li	a5,0
2005ffce:	853e                	c.mv	a0,a5
2005ffd0:	40b6                	c.lwsp	ra,76(sp)
2005ffd2:	4426                	c.lwsp	s0,72(sp)
2005ffd4:	6161                	c.addi16sp	sp,80
2005ffd6:	8082                	c.jr	ra

2005ffd8 <do_stack_backtrace>:
2005ffd8:	7179                	c.addi16sp	sp,-48
2005ffda:	ce52                	c.swsp	s4,28(sp)
2005ffdc:	d606                	c.swsp	ra,44(sp)
2005ffde:	d426                	c.swsp	s1,40(sp)
2005ffe0:	d24a                	c.swsp	s2,36(sp)
2005ffe2:	d04e                	c.swsp	s3,32(sp)
2005ffe4:	cc56                	c.swsp	s5,24(sp)
2005ffe6:	c402                	c.swsp	zero,8(sp)
2005ffe8:	c602                	c.swsp	zero,12(sp)
2005ffea:	c02e                	c.swsp	a1,0(sp)
2005ffec:	8a32                	c.mv	s4,a2
2005ffee:	c525                	c.beqz	a0,20060056 <do_stack_backtrace+0x7e>
2005fff0:	c1bd                	c.beqz	a1,20060056 <do_stack_backtrace+0x7e>
2005fff2:	c22a                	c.swsp	a0,4(sp)
2005fff4:	4712                	c.lwsp	a4,4(sp)
2005fff6:	4682                	c.lwsp	a3,0(sp)
2005fff8:	200644b7          	lui	s1,0x20064
2005fffc:	200649b7          	lui	s3,0x20064
20060000:	ce848513          	addi	a0,s1,-792 # 20063ce8 <TaskStateString.0+0x22c>
20060004:	1cc00613          	addi	a2,zero,460
20060008:	e9c98593          	addi	a1,s3,-356 # 20063e9c <__FUNCTION__.1>
2006000c:	ce848493          	addi	s1,s1,-792
20060010:	bd2ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060014:	4901                	c.li	s2,0
20060016:	04848493          	addi	s1,s1,72
2006001a:	05490b63          	beq	s2,s4,20060070 <do_stack_backtrace+0x98>
2006001e:	40000593          	addi	a1,zero,1024
20060022:	850a                	c.mv	a0,sp
20060024:	963ff0ef          	jal	ra,2005f986 <unwind_frame_rv32i>
20060028:	ed15                	c.bnez	a0,20060064 <do_stack_backtrace+0x8c>
2006002a:	4792                	c.lwsp	a5,4(sp)
2006002c:	4702                	c.lwsp	a4,0(sp)
2006002e:	48a2                	c.lwsp	a7,8(sp)
20060030:	4832                	c.lwsp	a6,12(sp)
20060032:	86ca                	c.mv	a3,s2
20060034:	1d700613          	addi	a2,zero,471
20060038:	e9c98593          	addi	a1,s3,-356
2006003c:	8526                	c.mv	a0,s1
2006003e:	ba4ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060042:	47a2                	c.lwsp	a5,8(sp)
20060044:	4732                	c.lwsp	a4,12(sp)
20060046:	00190a93          	addi	s5,s2,1
2006004a:	c23e                	c.swsp	a5,4(sp)
2006004c:	4782                	c.lwsp	a5,0(sp)
2006004e:	8956                	c.mv	s2,s5
20060050:	97ba                	c.add	a5,a4
20060052:	c03e                	c.swsp	a5,0(sp)
20060054:	b7d9                	c.j	2006001a <do_stack_backtrace+0x42>
20060056:	00010793          	addi	a5,sp,0
2006005a:	00000717          	auipc	a4,0x0
2006005e:	c23a                	c.swsp	a4,4(sp)
20060060:	c03e                	c.swsp	a5,0(sp)
20060062:	bf49                	c.j	2005fff4 <do_stack_backtrace+0x1c>
20060064:	40000593          	addi	a1,zero,1024
20060068:	850a                	c.mv	a0,sp
2006006a:	bf3ff0ef          	jal	ra,2005fc5c <unwind_frame_rv32c>
2006006e:	dd55                	c.beqz	a0,2006002a <do_stack_backtrace+0x52>
20060070:	50b2                	c.lwsp	ra,44(sp)
20060072:	54a2                	c.lwsp	s1,40(sp)
20060074:	5982                	c.lwsp	s3,32(sp)
20060076:	4a72                	c.lwsp	s4,28(sp)
20060078:	4ae2                	c.lwsp	s5,24(sp)
2006007a:	854a                	c.mv	a0,s2
2006007c:	5912                	c.lwsp	s2,36(sp)
2006007e:	6145                	c.addi16sp	sp,48
20060080:	8082                	c.jr	ra

20060082 <crash_dump>:
20060082:	711d                	c.addi16sp	sp,-96
20060084:	c8ca                	c.swsp	s2,80(sp)
20060086:	c6ce                	c.swsp	s3,76(sp)
20060088:	c2d6                	c.swsp	s5,68(sp)
2006008a:	ce86                	c.swsp	ra,92(sp)
2006008c:	cca2                	c.swsp	s0,88(sp)
2006008e:	caa6                	c.swsp	s1,84(sp)
20060090:	c4d2                	c.swsp	s4,72(sp)
20060092:	c0da                	c.swsp	s6,64(sp)
20060094:	de5e                	c.swsp	s7,60(sp)
20060096:	dc62                	c.swsp	s8,56(sp)
20060098:	da66                	c.swsp	s9,52(sp)
2006009a:	d86a                	c.swsp	s10,48(sp)
2006009c:	d66e                	c.swsp	s11,44(sp)
2006009e:	89aa                	c.mv	s3,a0
200600a0:	892e                	c.mv	s2,a1
200600a2:	8ab2                	c.mv	s5,a2
200600a4:	34002c73          	csrrs	s8,mscratch,zero
200600a8:	34102cf3          	csrrs	s9,mepc,zero
200600ac:	34202a73          	csrrs	s4,mcause,zero
200600b0:	34302bf3          	csrrs	s7,mtval,zero
200600b4:	20064437          	lui	s0,0x20064
200600b8:	eb040513          	addi	a0,s0,-336 # 20063eb0 <__FUNCTION__.1+0x14>
200600bc:	200644b7          	lui	s1,0x20064
200600c0:	11200613          	addi	a2,zero,274
200600c4:	64c48593          	addi	a1,s1,1612 # 2006464c <__FUNCTION__.0>
200600c8:	0551                	c.addi	a0,20
200600ca:	b18ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
200600ce:	e881a783          	lw	a5,-376(gp) # 20066838 <crash_task_info>
200600d2:	eb040413          	addi	s0,s0,-336
200600d6:	c399                	c.beqz	a5,200600dc <crash_dump+0x5a>
200600d8:	8556                	c.mv	a0,s5
200600da:	9782                	c.jalr	a5
200600dc:	473d                	c.li	a4,15
200600de:	87a2                	c.mv	a5,s0
200600e0:	01476a63          	bltu	a4,s4,200600f4 <crash_dump+0x72>
200600e4:	20064737          	lui	a4,0x20064
200600e8:	002a1793          	slli	a5,s4,0x2
200600ec:	77070713          	addi	a4,a4,1904 # 20064770 <exception_code_string>
200600f0:	97ba                	c.add	a5,a4
200600f2:	439c                	c.lw	a5,0(a5)
200600f4:	8752                	c.mv	a4,s4
200600f6:	86ce                	c.mv	a3,s3
200600f8:	11700613          	addi	a2,zero,279
200600fc:	64c48593          	addi	a1,s1,1612
20060100:	05440513          	addi	a0,s0,84
20060104:	adeff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060108:	47bd                	c.li	a5,15
2006010a:	1547f463          	bgeu	a5,s4,20060252 <crash_dump+0x1d0>
2006010e:	12b00613          	addi	a2,zero,299
20060112:	64c48593          	addi	a1,s1,1612
20060116:	0a440513          	addi	a0,s0,164
2006011a:	ac8ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
2006011e:	86e2                	c.mv	a3,s8
20060120:	12c00613          	addi	a2,zero,300
20060124:	64c48593          	addi	a1,s1,1612
20060128:	0e840513          	addi	a0,s0,232
2006012c:	ab6ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060130:	86e6                	c.mv	a3,s9
20060132:	12d00613          	addi	a2,zero,301
20060136:	64c48593          	addi	a1,s1,1612
2006013a:	11840513          	addi	a0,s0,280
2006013e:	aa4ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060142:	86d2                	c.mv	a3,s4
20060144:	12e00613          	addi	a2,zero,302
20060148:	64c48593          	addi	a1,s1,1612
2006014c:	14840513          	addi	a0,s0,328
20060150:	a92ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060154:	86de                	c.mv	a3,s7
20060156:	12f00613          	addi	a2,zero,303
2006015a:	64c48593          	addi	a1,s1,1612
2006015e:	17840513          	addi	a0,s0,376
20060162:	20064b37          	lui	s6,0x20064
20060166:	a7cff0ef          	jal	ra,2005f3e2 <DiagPrintf>
2006016a:	7b0b0b13          	addi	s6,s6,1968 # 200647b0 <register_string>
2006016e:	4a01                	c.li	s4,0
20060170:	23440c13          	addi	s8,s0,564
20060174:	02000b93          	addi	s7,zero,32
20060178:	002a1793          	slli	a5,s4,0x2
2006017c:	97d6                	c.add	a5,s5
2006017e:	000b2703          	lw	a4,0(s6)
20060182:	439c                	c.lw	a5,0(a5)
20060184:	86d2                	c.mv	a3,s4
20060186:	13400613          	addi	a2,zero,308
2006018a:	64c48593          	addi	a1,s1,1612
2006018e:	8562                	c.mv	a0,s8
20060190:	0a05                	c.addi	s4,1
20060192:	a50ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060196:	0b11                	c.addi	s6,4
20060198:	ff7a10e3          	bne	s4,s7,20060178 <crash_dump+0xf6>
2006019c:	15a00613          	addi	a2,zero,346
200601a0:	64c48593          	addi	a1,s1,1612
200601a4:	26440513          	addi	a0,s0,612
200601a8:	a3aff0ef          	jal	ra,2005f3e2 <DiagPrintf>
200601ac:	4629                	c.li	a2,10
200601ae:	85ca                	c.mv	a1,s2
200601b0:	854e                	c.mv	a0,s3
200601b2:	e27ff0ef          	jal	ra,2005ffd8 <do_stack_backtrace>
200601b6:	e921                	c.bnez	a0,20060206 <crash_dump+0x184>
200601b8:	40000593          	addi	a1,zero,1024
200601bc:	850a                	c.mv	a0,sp
200601be:	c24e                	c.swsp	s3,4(sp)
200601c0:	ca4e                	c.swsp	s3,20(sp)
200601c2:	c402                	c.swsp	zero,8(sp)
200601c4:	c602                	c.swsp	zero,12(sp)
200601c6:	c04a                	c.swsp	s2,0(sp)
200601c8:	cc02                	c.swsp	zero,24(sp)
200601ca:	ce02                	c.swsp	zero,28(sp)
200601cc:	c84a                	c.swsp	s2,16(sp)
200601ce:	fb8ff0ef          	jal	ra,2005f986 <unwind_frame_rv32i>
200601d2:	40000593          	addi	a1,zero,1024
200601d6:	0808                	c.addi4spn	a0,sp,16
200601d8:	a85ff0ef          	jal	ra,2005fc5c <unwind_frame_rv32c>
200601dc:	46b2                	c.lwsp	a3,12(sp)
200601de:	4772                	c.lwsp	a4,28(sp)
200601e0:	89b6                	c.mv	s3,a3
200601e2:	e291                	c.bnez	a3,200601e6 <crash_dump+0x164>
200601e4:	89ba                	c.mv	s3,a4
200601e6:	16c00613          	addi	a2,zero,364
200601ea:	64c48593          	addi	a1,s1,1612
200601ee:	2a440513          	addi	a0,s0,676
200601f2:	9f0ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
200601f6:	004aa503          	lw	a0,4(s5)
200601fa:	ffc9f593          	andi	a1,s3,-4
200601fe:	4629                	c.li	a2,10
20060200:	95ca                	c.add	a1,s2
20060202:	dd7ff0ef          	jal	ra,2005ffd8 <do_stack_backtrace>
20060206:	64c48593          	addi	a1,s1,1612
2006020a:	17000613          	addi	a2,zero,368
2006020e:	30c40513          	addi	a0,s0,780
20060212:	9d0ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060216:	64c48593          	addi	a1,s1,1612
2006021a:	18300613          	addi	a2,zero,387
2006021e:	35440513          	addi	a0,s0,852
20060222:	9c0ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060226:	39c40513          	addi	a0,s0,924
2006022a:	4466                	c.lwsp	s0,88(sp)
2006022c:	40f6                	c.lwsp	ra,92(sp)
2006022e:	4946                	c.lwsp	s2,80(sp)
20060230:	49b6                	c.lwsp	s3,76(sp)
20060232:	4a26                	c.lwsp	s4,72(sp)
20060234:	4a96                	c.lwsp	s5,68(sp)
20060236:	4b06                	c.lwsp	s6,64(sp)
20060238:	5bf2                	c.lwsp	s7,60(sp)
2006023a:	5c62                	c.lwsp	s8,56(sp)
2006023c:	5cd2                	c.lwsp	s9,52(sp)
2006023e:	5d42                	c.lwsp	s10,48(sp)
20060240:	5db2                	c.lwsp	s11,44(sp)
20060242:	64c48593          	addi	a1,s1,1612
20060246:	44d6                	c.lwsp	s1,84(sp)
20060248:	18400613          	addi	a2,zero,388
2006024c:	6125                	c.addi16sp	sp,96
2006024e:	994ff06f          	jal	zero,2005f3e2 <DiagPrintf>
20060252:	20064737          	lui	a4,0x20064
20060256:	002a1793          	slli	a5,s4,0x2
2006025a:	73070713          	addi	a4,a4,1840 # 20064730 <exception_cause_string_tbl>
2006025e:	97ba                	c.add	a5,a4
20060260:	0007ab03          	lw	s6,0(a5)
20060264:	ea0b05e3          	beq	s6,zero,2006010e <crash_dump+0x8c>
20060268:	11d00613          	addi	a2,zero,285
2006026c:	64c48593          	addi	a1,s1,1612
20060270:	1a840513          	addi	a0,s0,424
20060274:	96eff0ef          	jal	ra,2005f3e2 <DiagPrintf>
20060278:	21040d13          	addi	s10,s0,528
2006027c:	1e040d93          	addi	s11,s0,480
20060280:	000b2683          	lw	a3,0(s6)
20060284:	e80685e3          	beq	a3,zero,2006010e <crash_dump+0x8c>
20060288:	004b2703          	lw	a4,4(s6)
2006028c:	cb19                	c.beqz	a4,200602a2 <crash_dump+0x220>
2006028e:	87de                	c.mv	a5,s7
20060290:	12000613          	addi	a2,zero,288
20060294:	64c48593          	addi	a1,s1,1612
20060298:	856e                	c.mv	a0,s11
2006029a:	948ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
2006029e:	0b21                	c.addi	s6,8
200602a0:	b7c5                	c.j	20060280 <crash_dump+0x1fe>
200602a2:	12300613          	addi	a2,zero,291
200602a6:	64c48593          	addi	a1,s1,1612
200602aa:	856a                	c.mv	a0,s10
200602ac:	936ff0ef          	jal	ra,2005f3e2 <DiagPrintf>
200602b0:	b7fd                	c.j	2006029e <crash_dump+0x21c>

200602b2 <_my_random>:
200602b2:	1141                	c.addi	sp,-16
200602b4:	852e                	c.mv	a0,a1
200602b6:	85b2                	c.mv	a1,a2
200602b8:	c606                	c.swsp	ra,12(sp)
200602ba:	db6ff0ef          	jal	ra,2005f870 <TRNG_get_random_bytes>
200602be:	40b2                	c.lwsp	ra,12(sp)
200602c0:	4501                	c.li	a0,0
200602c2:	0141                	c.addi	sp,16
200602c4:	8082                	c.jr	ra

200602c6 <_my_free>:
200602c6:	c40fb06f          	jal	zero,2005b706 <rtos_mem_free>

200602ca <_my_calloc>:
200602ca:	02b50533          	mul	a0,a0,a1
200602ce:	c12fb06f          	jal	zero,2005b6e0 <rtos_mem_zmalloc>

200602d2 <wifi_set_rom2flash>:
200602d2:	200697b7          	lui	a5,0x20069
200602d6:	85478793          	addi	a5,a5,-1964 # 20068854 <wifi_rom_func_map>
200602da:	fef1a623          	sw	a5,-20(gp) # 2006699c <p_wifi_rom_func_map>
200602de:	20060737          	lui	a4,0x20060
200602e2:	2ca70713          	addi	a4,a4,714 # 200602ca <_my_calloc>
200602e6:	c398                	c.sw	a4,0(a5)
200602e8:	20060737          	lui	a4,0x20060
200602ec:	2c670713          	addi	a4,a4,710 # 200602c6 <_my_free>
200602f0:	c3d8                	c.sw	a4,4(a5)
200602f2:	20060737          	lui	a4,0x20060
200602f6:	2b270713          	addi	a4,a4,690 # 200602b2 <_my_random>
200602fa:	c798                	c.sw	a4,8(a5)
200602fc:	200687b7          	lui	a5,0x20068
20060300:	6e078793          	addi	a5,a5,1760 # 200686e0 <wifi_rom2flash>
20060304:	fef1a423          	sw	a5,-24(gp) # 20066998 <p_wifi_rom2flash>
20060308:	2005c737          	lui	a4,0x2005c
2006030c:	81c70713          	addi	a4,a4,-2020 # 2005b81c <rtos_mutex_give>
20060310:	c3d8                	c.sw	a4,4(a5)
20060312:	2005b737          	lui	a4,0x2005b
20060316:	7d070713          	addi	a4,a4,2000 # 2005b7d0 <rtos_mutex_take>
2006031a:	c398                	c.sw	a4,0(a5)
2006031c:	2005b737          	lui	a4,0x2005b
20060320:	7b270713          	addi	a4,a4,1970 # 2005b7b2 <rtos_mutex_delete_static>
20060324:	d798                	c.sw	a4,40(a5)
20060326:	2005b737          	lui	a4,0x2005b
2006032a:	73870713          	addi	a4,a4,1848 # 2005b738 <rtos_mutex_create_static>
2006032e:	d7d8                	c.sw	a4,44(a5)
20060330:	2005c737          	lui	a4,0x2005c
20060334:	95670713          	addi	a4,a4,-1706 # 2005b956 <rtos_sema_give>
20060338:	c7d8                	c.sw	a4,12(a5)
2006033a:	2005c737          	lui	a4,0x2005c
2006033e:	90a70713          	addi	a4,a4,-1782 # 2005b90a <rtos_sema_take>
20060342:	cb98                	c.sw	a4,16(a5)
20060344:	2005b737          	lui	a4,0x2005b
20060348:	69070713          	addi	a4,a4,1680 # 2005b690 <rtos_critical_enter>
2006034c:	cbd8                	c.sw	a4,20(a5)
2006034e:	2005b737          	lui	a4,0x2005b
20060352:	6b470713          	addi	a4,a4,1716 # 2005b6b4 <rtos_critical_exit>
20060356:	cf98                	c.sw	a4,24(a5)
20060358:	2005c737          	lui	a4,0x2005c
2006035c:	be070713          	addi	a4,a4,-1056 # 2005bbe0 <rtos_time_delay_ms>
20060360:	d3d8                	c.sw	a4,36(a5)
20060362:	2005c737          	lui	a4,0x2005c
20060366:	c1a70713          	addi	a4,a4,-998 # 2005bc1a <rtos_time_get_current_system_time_ms>
2006036a:	c798                	c.sw	a4,8(a5)
2006036c:	20061737          	lui	a4,0x20061
20060370:	33870713          	addi	a4,a4,824 # 20061338 <rtw_init_timer>
20060374:	d7b8                	c.sw	a4,104(a5)
20060376:	20061737          	lui	a4,0x20061
2006037a:	34270713          	addi	a4,a4,834 # 20061342 <rtw_set_timer>
2006037e:	d7f8                	c.sw	a4,108(a5)
20060380:	20061737          	lui	a4,0x20061
20060384:	34670713          	addi	a4,a4,838 # 20061346 <rtw_cancel_timer>
20060388:	dbb8                	c.sw	a4,112(a5)
2006038a:	20061737          	lui	a4,0x20061
2006038e:	35670713          	addi	a4,a4,854 # 20061356 <rtw_del_timer>
20060392:	dbf8                	c.sw	a4,116(a5)
20060394:	20059737          	lui	a4,0x20059
20060398:	70470713          	addi	a4,a4,1796 # 20059704 <pmu_set_sysactive_time>
2006039c:	dff8                	c.sw	a4,124(a5)
2006039e:	20061737          	lui	a4,0x20061
200603a2:	38e70713          	addi	a4,a4,910 # 2006138e <rtw_wakelock_timeout>
200603a6:	08e7a023          	sw	a4,128(a5)
200603aa:	20061737          	lui	a4,0x20061
200603ae:	35a70713          	addi	a4,a4,858 # 2006135a <rtw_acquire_wakelock>
200603b2:	db98                	c.sw	a4,48(a5)
200603b4:	20061737          	lui	a4,0x20061
200603b8:	37470713          	addi	a4,a4,884 # 20061374 <rtw_release_wakelock>
200603bc:	dbd8                	c.sw	a4,52(a5)
200603be:	8082                	c.jr	ra

200603c0 <wifi_on>:
200603c0:	1101                	c.addi	sp,-32
200603c2:	ce06                	c.swsp	ra,28(sp)
200603c4:	cc22                	c.swsp	s0,24(sp)
200603c6:	ca26                	c.swsp	s1,20(sp)
200603c8:	842a                	c.mv	s0,a0
200603ca:	c84a                	c.swsp	s2,16(sp)
200603cc:	e06fe0ef          	jal	ra,2005e9d2 <wifi_event_init>
200603d0:	84df90ef          	jal	ra,20059c1c <wifi_set_user_config>
200603d4:	2df000ef          	jal	ra,20060eb2 <inic_msg_q_init>
200603d8:	41018793          	addi	a5,gp,1040 # 20066dc0 <wifi_user_config>
200603dc:	7c102773          	csrrs	a4,0x7c1,zero
200603e0:	10077713          	andi	a4,a4,256
200603e4:	cb15                	c.beqz	a4,20060418 <wifi_on+0x58>
200603e6:	0ff0000f          	fence	iorw,iorw
200603ea:	04f7f00b          	cache	dwb,(a5)
200603ee:	02078713          	addi	a4,a5,32
200603f2:	04e7700b          	cache	dwb,(a4)
200603f6:	04078713          	addi	a4,a5,64
200603fa:	04e7700b          	cache	dwb,(a4)
200603fe:	0ff0000f          	fence	iorw,iorw
20060402:	0001                	c.addi	zero,0
20060404:	0001                	c.addi	zero,0
20060406:	0001                	c.addi	zero,0
20060408:	0001                	c.addi	zero,0
2006040a:	0001                	c.addi	zero,0
2006040c:	0001                	c.addi	zero,0
2006040e:	0001                	c.addi	zero,0
20060410:	0001                	c.addi	zero,0
20060412:	0001                	c.addi	zero,0
20060414:	0001                	c.addi	zero,0
20060416:	0001                	c.addi	zero,0
20060418:	4605                	c.li	a2,1
2006041a:	006c                	c.addi4spn	a1,sp,12
2006041c:	451d                	c.li	a0,7
2006041e:	c63e                	c.swsp	a5,12(sp)
20060420:	2eb5                	c.jal	2006079c <inic_api_host_message_send>
20060422:	79a000ef          	jal	ra,20060bbc <inic_host_init_skb>
20060426:	4605                	c.li	a2,1
20060428:	006c                	c.addi4spn	a1,sp,12
2006042a:	4525                	c.li	a0,9
2006042c:	c622                	c.swsp	s0,12(sp)
2006042e:	26bd                	c.jal	2006079c <inic_api_host_message_send>
20060430:	fcc18793          	addi	a5,gp,-52 # 2006697c <wifi_boot.0>
20060434:	4398                	c.lw	a4,0(a5)
20060436:	84aa                	c.mv	s1,a0
20060438:	ef01                	c.bnez	a4,20060450 <wifi_on+0x90>
2006043a:	4905                	c.li	s2,1
2006043c:	0127a023          	sw	s2,0(a5)
20060440:	4fb000ef          	jal	ra,2006113a <init_timer_wrapper>
20060444:	fb81a783          	lw	a5,-72(gp) # 20066968 <p_wifi_do_fast_connect>
20060448:	c781                	c.beqz	a5,20060450 <wifi_on+0x90>
2006044a:	01241363          	bne	s0,s2,20060450 <wifi_on+0x90>
2006044e:	9782                	c.jalr	a5
20060450:	40f2                	c.lwsp	ra,28(sp)
20060452:	4462                	c.lwsp	s0,24(sp)
20060454:	4942                	c.lwsp	s2,16(sp)
20060456:	8526                	c.mv	a0,s1
20060458:	44d2                	c.lwsp	s1,20(sp)
2006045a:	6105                	c.addi16sp	sp,32
2006045c:	8082                	c.jr	ra

2006045e <_inic_ipc_ip_addr_update_in_wowlan>:
2006045e:	4505                	c.li	a0,1
20060460:	8082                	c.jr	ra

20060462 <inic_api_host_task_h>:
20060462:	7179                	c.addi16sp	sp,-48
20060464:	d226                	c.swsp	s1,36(sp)
20060466:	200654b7          	lui	s1,0x20065
2006046a:	d04a                	c.swsp	s2,32(sp)
2006046c:	ce4e                	c.swsp	s3,28(sp)
2006046e:	cc52                	c.swsp	s4,24(sp)
20060470:	ca56                	c.swsp	s5,20(sp)
20060472:	c85a                	c.swsp	s6,16(sp)
20060474:	c65e                	c.swsp	s7,12(sp)
20060476:	d606                	c.swsp	ra,44(sp)
20060478:	d422                	c.swsp	s0,40(sp)
2006047a:	20065a37          	lui	s4,0x20065
2006047e:	86448493          	addi	s1,s1,-1948 # 20064864 <register_string+0xb4>
20060482:	fd81a503          	lw	a0,-40(gp) # 20066988 <g_host_inic_api_task_wake_sema>
20060486:	55fd                	c.li	a1,-1
20060488:	c82fb0ef          	jal	ra,2005b90a <rtos_sema_take>
2006048c:	459d                	c.li	a1,7
2006048e:	4541                	c.li	a0,16
20060490:	c55f80ef          	jal	ra,200590e4 <ipc_get_message>
20060494:	4140                	c.lw	s0,4(a0)
20060496:	7c1027f3          	csrrs	a5,0x7c1,zero
2006049a:	1007f793          	andi	a5,a5,256
2006049e:	c3b9                	c.beqz	a5,200604e4 <inic_api_host_task_h+0x82>
200604a0:	01f47713          	andi	a4,s0,31
200604a4:	cb21                	c.beqz	a4,200604f4 <inic_api_host_task_h+0x92>
200604a6:	04070693          	addi	a3,a4,64
200604aa:	fe047793          	andi	a5,s0,-32
200604ae:	01f7f713          	andi	a4,a5,31
200604b2:	9736                	c.add	a4,a3
200604b4:	0ff0000f          	fence	iorw,iorw
200604b8:	973e                	c.add	a4,a5
200604ba:	00f7f00b          	cache	dinv,(a5)
200604be:	02078793          	addi	a5,a5,32
200604c2:	40f706b3          	sub	a3,a4,a5
200604c6:	fed04ae3          	blt	zero,a3,200604ba <inic_api_host_task_h+0x58>
200604ca:	0ff0000f          	fence	iorw,iorw
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
200604e2:	0001                	c.addi	zero,0
200604e4:	ec01                	c.bnez	s0,200604fc <inic_api_host_task_h+0x9a>
200604e6:	20065537          	lui	a0,0x20065
200604ea:	83050513          	addi	a0,a0,-2000 # 20064830 <register_string+0x80>
200604ee:	f15fe0ef          	jal	ra,2005f402 <DiagPrintf_minimal>
200604f2:	bf41                	c.j	20060482 <inic_api_host_task_h+0x20>
200604f4:	87a2                	c.mv	a5,s0
200604f6:	04000693          	addi	a3,zero,64
200604fa:	bf55                	c.j	200604ae <inic_api_host_task_h+0x4c>
200604fc:	400c                	c.lw	a1,0(s0)
200604fe:	472d                	c.li	a4,11
20060500:	fff58793          	addi	a5,a1,-1
20060504:	28f76363          	bltu	a4,a5,2006078a <inic_api_host_task_h+0x328>
20060508:	078a                	c.slli	a5,0x2
2006050a:	97a6                	c.add	a5,s1
2006050c:	439c                	c.lw	a5,0(a5)
2006050e:	8782                	c.jr	a5
20060510:	fc81a783          	lw	a5,-56(gp) # 20066978 <scan_user_callback_ptr>
20060514:	c3ad                	c.beqz	a5,20060576 <inic_api_host_task_h+0x114>
20060516:	440c                	c.lw	a1,8(s0)
20060518:	4048                	c.lw	a0,4(s0)
2006051a:	9782                	c.jalr	a5
2006051c:	a8a9                	c.j	20060576 <inic_api_host_task_h+0x114>
2006051e:	4048                	c.lw	a0,4(s0)
20060520:	7c1027f3          	csrrs	a5,0x7c1,zero
20060524:	1007f793          	andi	a5,a5,256
20060528:	c3b9                	c.beqz	a5,2006056e <inic_api_host_task_h+0x10c>
2006052a:	01f57713          	andi	a4,a0,31
2006052e:	c345                	c.beqz	a4,200605ce <inic_api_host_task_h+0x16c>
20060530:	00870693          	addi	a3,a4,8
20060534:	fe057793          	andi	a5,a0,-32
20060538:	01f7f713          	andi	a4,a5,31
2006053c:	9736                	c.add	a4,a3
2006053e:	0ff0000f          	fence	iorw,iorw
20060542:	973e                	c.add	a4,a5
20060544:	00f7f00b          	cache	dinv,(a5)
20060548:	02078793          	addi	a5,a5,32
2006054c:	40f706b3          	sub	a3,a4,a5
20060550:	fed04ae3          	blt	zero,a3,20060544 <inic_api_host_task_h+0xe2>
20060554:	0ff0000f          	fence	iorw,iorw
20060558:	0001                	c.addi	zero,0
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
2006056e:	fc01a783          	lw	a5,-64(gp) # 20066970 <scan_acs_report_user_callback_ptr>
20060572:	c391                	c.beqz	a5,20060576 <inic_api_host_task_h+0x114>
20060574:	9782                	c.jalr	a5
20060576:	00042023          	sw	zero,0(s0)
2006057a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006057e:	1007f793          	andi	a5,a5,256
20060582:	f00780e3          	beq	a5,zero,20060482 <inic_api_host_task_h+0x20>
20060586:	01f47793          	andi	a5,s0,31
2006058a:	04000713          	addi	a4,zero,64
2006058e:	c781                	c.beqz	a5,20060596 <inic_api_host_task_h+0x134>
20060590:	04078713          	addi	a4,a5,64
20060594:	9801                	c.andi	s0,-32
20060596:	01f47793          	andi	a5,s0,31
2006059a:	97ba                	c.add	a5,a4
2006059c:	0ff0000f          	fence	iorw,iorw
200605a0:	97a2                	c.add	a5,s0
200605a2:	0484700b          	cache	dwb,(s0)
200605a6:	02040413          	addi	s0,s0,32
200605aa:	40878733          	sub	a4,a5,s0
200605ae:	fee04ae3          	blt	zero,a4,200605a2 <inic_api_host_task_h+0x140>
200605b2:	0ff0000f          	fence	iorw,iorw
200605b6:	0001                	c.addi	zero,0
200605b8:	0001                	c.addi	zero,0
200605ba:	0001                	c.addi	zero,0
200605bc:	0001                	c.addi	zero,0
200605be:	0001                	c.addi	zero,0
200605c0:	0001                	c.addi	zero,0
200605c2:	0001                	c.addi	zero,0
200605c4:	0001                	c.addi	zero,0
200605c6:	0001                	c.addi	zero,0
200605c8:	0001                	c.addi	zero,0
200605ca:	0001                	c.addi	zero,0
200605cc:	bd5d                	c.j	20060482 <inic_api_host_task_h+0x20>
200605ce:	87aa                	c.mv	a5,a0
200605d0:	46a1                	c.li	a3,8
200605d2:	b79d                	c.j	20060538 <inic_api_host_task_h+0xd6>
200605d4:	4048                	c.lw	a0,4(s0)
200605d6:	440c                	c.lw	a1,8(s0)
200605d8:	7c1027f3          	csrrs	a5,0x7c1,zero
200605dc:	1007f793          	andi	a5,a5,256
200605e0:	c3b9                	c.beqz	a5,20060626 <inic_api_host_task_h+0x1c4>
200605e2:	01f57713          	andi	a4,a0,31
200605e6:	c729                	c.beqz	a4,20060630 <inic_api_host_task_h+0x1ce>
200605e8:	03b70693          	addi	a3,a4,59
200605ec:	fe057793          	andi	a5,a0,-32
200605f0:	01f7f713          	andi	a4,a5,31
200605f4:	9736                	c.add	a4,a3
200605f6:	0ff0000f          	fence	iorw,iorw
200605fa:	973e                	c.add	a4,a5
200605fc:	00f7f00b          	cache	dinv,(a5)
20060600:	02078793          	addi	a5,a5,32
20060604:	40f706b3          	sub	a3,a4,a5
20060608:	fed04ae3          	blt	zero,a3,200605fc <inic_api_host_task_h+0x19a>
2006060c:	0ff0000f          	fence	iorw,iorw
20060610:	0001                	c.addi	zero,0
20060612:	0001                	c.addi	zero,0
20060614:	0001                	c.addi	zero,0
20060616:	0001                	c.addi	zero,0
20060618:	0001                	c.addi	zero,0
2006061a:	0001                	c.addi	zero,0
2006061c:	0001                	c.addi	zero,0
2006061e:	0001                	c.addi	zero,0
20060620:	0001                	c.addi	zero,0
20060622:	0001                	c.addi	zero,0
20060624:	0001                	c.addi	zero,0
20060626:	fc41a783          	lw	a5,-60(gp) # 20066974 <scan_each_report_user_callback_ptr>
2006062a:	ee0798e3          	bne	a5,zero,2006051a <inic_api_host_task_h+0xb8>
2006062e:	b7a1                	c.j	20060576 <inic_api_host_task_h+0x114>
20060630:	87aa                	c.mv	a5,a0
20060632:	03b00693          	addi	a3,zero,59
20060636:	bf6d                	c.j	200605f0 <inic_api_host_task_h+0x18e>
20060638:	fd01a783          	lw	a5,-48(gp) # 20066980 <p_ap_channel_switch_callback>
2006063c:	df8d                	c.beqz	a5,20060576 <inic_api_host_task_h+0x114>
2006063e:	00840583          	lb	a1,8(s0)
20060642:	00444503          	lbu	a0,4(s0)
20060646:	9782                	c.jalr	a5
20060648:	b73d                	c.j	20060576 <inic_api_host_task_h+0x114>
2006064a:	440c                	c.lw	a1,8(s0)
2006064c:	4450                	c.lw	a2,12(s0)
2006064e:	4048                	c.lw	a0,4(s0)
20060650:	4814                	c.lw	a3,16(s0)
20060652:	87ae                	c.mv	a5,a1
20060654:	8832                	c.mv	a6,a2
20060656:	7c102773          	csrrs	a4,0x7c1,zero
2006065a:	10077713          	andi	a4,a4,256
2006065e:	c70d                	c.beqz	a4,20060688 <inic_api_host_task_h+0x226>
20060660:	00c5f733          	and	a4,a1,a2
20060664:	58fd                	c.li	a7,-1
20060666:	03171463          	bne	a4,a7,2006068e <inic_api_host_task_h+0x22c>
2006066a:	7c0167f3          	csrrsi	a5,0x7c0,2
2006066e:	0ff0000f          	fence	iorw,iorw
20060672:	0001                	c.addi	zero,0
20060674:	0001                	c.addi	zero,0
20060676:	0001                	c.addi	zero,0
20060678:	0001                	c.addi	zero,0
2006067a:	0001                	c.addi	zero,0
2006067c:	0001                	c.addi	zero,0
2006067e:	0001                	c.addi	zero,0
20060680:	0001                	c.addi	zero,0
20060682:	0001                	c.addi	zero,0
20060684:	0001                	c.addi	zero,0
20060686:	0001                	c.addi	zero,0
20060688:	b60fe0ef          	jal	ra,2005e9e8 <wifi_indication>
2006068c:	b5ed                	c.j	20060576 <inic_api_host_task_h+0x114>
2006068e:	01f5f713          	andi	a4,a1,31
20060692:	c709                	c.beqz	a4,2006069c <inic_api_host_task_h+0x23a>
20060694:	fe05f793          	andi	a5,a1,-32
20060698:	00e60833          	add	a6,a2,a4
2006069c:	ff0056e3          	bge	zero,a6,20060688 <inic_api_host_task_h+0x226>
200606a0:	01f7f713          	andi	a4,a5,31
200606a4:	9742                	c.add	a4,a6
200606a6:	0ff0000f          	fence	iorw,iorw
200606aa:	973e                	c.add	a4,a5
200606ac:	00f7f00b          	cache	dinv,(a5)
200606b0:	02078793          	addi	a5,a5,32
200606b4:	40f70833          	sub	a6,a4,a5
200606b8:	ff004ae3          	blt	zero,a6,200606ac <inic_api_host_task_h+0x24a>
200606bc:	bf4d                	c.j	2006066e <inic_api_host_task_h+0x20c>
200606be:	fbc1a783          	lw	a5,-68(gp) # 2006696c <promisc_user_callback_ptr>
200606c2:	4501                	c.li	a0,0
200606c4:	cbc1                	c.beqz	a5,20060754 <inic_api_host_task_h+0x2f2>
200606c6:	4058                	c.lw	a4,4(s0)
200606c8:	7c1027f3          	csrrs	a5,0x7c1,zero
200606cc:	1007f793          	andi	a5,a5,256
200606d0:	c3b9                	c.beqz	a5,20060716 <inic_api_host_task_h+0x2b4>
200606d2:	01f77693          	andi	a3,a4,31
200606d6:	c2c9                	c.beqz	a3,20060758 <inic_api_host_task_h+0x2f6>
200606d8:	00c68613          	addi	a2,a3,12
200606dc:	fe077793          	andi	a5,a4,-32
200606e0:	01f7f693          	andi	a3,a5,31
200606e4:	96b2                	c.add	a3,a2
200606e6:	0ff0000f          	fence	iorw,iorw
200606ea:	96be                	c.add	a3,a5
200606ec:	00f7f00b          	cache	dinv,(a5)
200606f0:	02078793          	addi	a5,a5,32
200606f4:	40f68633          	sub	a2,a3,a5
200606f8:	fec04ae3          	blt	zero,a2,200606ec <inic_api_host_task_h+0x28a>
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
20060716:	435c                	c.lw	a5,4(a4)
20060718:	4714                	c.lw	a3,8(a4)
2006071a:	7c102773          	csrrs	a4,0x7c1,zero
2006071e:	10077713          	andi	a4,a4,256
20060722:	c70d                	c.beqz	a4,2006074c <inic_api_host_task_h+0x2ea>
20060724:	577d                	c.li	a4,-1
20060726:	02e79c63          	bne	a5,a4,2006075e <inic_api_host_task_h+0x2fc>
2006072a:	02f69a63          	bne	a3,a5,2006075e <inic_api_host_task_h+0x2fc>
2006072e:	7c0167f3          	csrrsi	a5,0x7c0,2
20060732:	0ff0000f          	fence	iorw,iorw
20060736:	0001                	c.addi	zero,0
20060738:	0001                	c.addi	zero,0
2006073a:	0001                	c.addi	zero,0
2006073c:	0001                	c.addi	zero,0
2006073e:	0001                	c.addi	zero,0
20060740:	0001                	c.addi	zero,0
20060742:	0001                	c.addi	zero,0
20060744:	0001                	c.addi	zero,0
20060746:	0001                	c.addi	zero,0
20060748:	0001                	c.addi	zero,0
2006074a:	0001                	c.addi	zero,0
2006074c:	fbc1a783          	lw	a5,-68(gp) # 2006696c <promisc_user_callback_ptr>
20060750:	4048                	c.lw	a0,4(s0)
20060752:	9782                	c.jalr	a5
20060754:	d048                	c.sw	a0,36(s0)
20060756:	b505                	c.j	20060576 <inic_api_host_task_h+0x114>
20060758:	87ba                	c.mv	a5,a4
2006075a:	4631                	c.li	a2,12
2006075c:	b751                	c.j	200606e0 <inic_api_host_task_h+0x27e>
2006075e:	01f7f713          	andi	a4,a5,31
20060762:	c319                	c.beqz	a4,20060768 <inic_api_host_task_h+0x306>
20060764:	9b81                	c.andi	a5,-32
20060766:	96ba                	c.add	a3,a4
20060768:	fed052e3          	bge	zero,a3,2006074c <inic_api_host_task_h+0x2ea>
2006076c:	01f7f713          	andi	a4,a5,31
20060770:	9736                	c.add	a4,a3
20060772:	0ff0000f          	fence	iorw,iorw
20060776:	973e                	c.add	a4,a5
20060778:	00f7f00b          	cache	dinv,(a5)
2006077c:	02078793          	addi	a5,a5,32
20060780:	40f706b3          	sub	a3,a4,a5
20060784:	fed04ae3          	blt	zero,a3,20060778 <inic_api_host_task_h+0x316>
20060788:	b76d                	c.j	20060732 <inic_api_host_task_h+0x2d0>
2006078a:	848a0513          	addi	a0,s4,-1976 # 20064848 <register_string+0x98>
2006078e:	c75fe0ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20060792:	b3d5                	c.j	20060576 <inic_api_host_task_h+0x114>

20060794 <inic_api_host_int_hdl>:
20060794:	fd81a503          	lw	a0,-40(gp) # 20066988 <g_host_inic_api_task_wake_sema>
20060798:	9befb06f          	jal	zero,2005b956 <rtos_sema_give>

2006079c <inic_api_host_message_send>:
2006079c:	7139                	c.addi16sp	sp,-64
2006079e:	d452                	c.swsp	s4,40(sp)
200607a0:	d256                	c.swsp	s5,36(sp)
200607a2:	fca1ae23          	sw	a0,-36(gp) # 2006698c <latest_api_id>
200607a6:	d05a                	c.swsp	s6,32(sp)
200607a8:	8b2a                	c.mv	s6,a0
200607aa:	fd41a503          	lw	a0,-44(gp) # 20066984 <g_host_inic_api_message_send_sema>
200607ae:	cc62                	c.swsp	s8,24(sp)
200607b0:	20068c37          	lui	s8,0x20068
200607b4:	da26                	c.swsp	s1,52(sp)
200607b6:	d64e                	c.swsp	s3,44(sp)
200607b8:	ca66                	c.swsp	s9,20(sp)
200607ba:	89ae                	c.mv	s3,a1
200607bc:	600c0493          	addi	s1,s8,1536 # 20068600 <g_host_ipc_api_request_info>
200607c0:	55fd                	c.li	a1,-1
200607c2:	6c8d                	c.lui	s9,0x3
200607c4:	ce5e                	c.swsp	s7,28(sp)
200607c6:	c86a                	c.swsp	s10,16(sp)
200607c8:	c66e                	c.swsp	s11,12(sp)
200607ca:	de06                	c.swsp	ra,60(sp)
200607cc:	dc22                	c.swsp	s0,56(sp)
200607ce:	d84a                	c.swsp	s2,48(sp)
200607d0:	8bb2                	c.mv	s7,a2
200607d2:	fdc18a93          	addi	s5,gp,-36 # 2006698c <latest_api_id>
200607d6:	934fb0ef          	jal	ra,2005b90a <rtos_sema_take>
200607da:	fd418a13          	addi	s4,gp,-44 # 20066984 <g_host_inic_api_message_send_sema>
200607de:	02048d93          	addi	s11,s1,32
200607e2:	ee0c8c93          	addi	s9,s9,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
200607e6:	20065d37          	lui	s10,0x20065
200607ea:	4401                	c.li	s0,0
200607ec:	7c1027f3          	csrrs	a5,0x7c1,zero
200607f0:	1007f793          	andi	a5,a5,256
200607f4:	600c0913          	addi	s2,s8,1536
200607f8:	c785                	c.beqz	a5,20060820 <inic_api_host_message_send+0x84>
200607fa:	0ff0000f          	fence	iorw,iorw
200607fe:	0094f00b          	cache	dinv,(s1)
20060802:	01bdf00b          	cache	dinv,(s11)
20060806:	0ff0000f          	fence	iorw,iorw
2006080a:	0001                	c.addi	zero,0
2006080c:	0001                	c.addi	zero,0
2006080e:	0001                	c.addi	zero,0
20060810:	0001                	c.addi	zero,0
20060812:	0001                	c.addi	zero,0
20060814:	0001                	c.addi	zero,0
20060816:	0001                	c.addi	zero,0
20060818:	0001                	c.addi	zero,0
2006081a:	0001                	c.addi	zero,0
2006081c:	0001                	c.addi	zero,0
2006081e:	0001                	c.addi	zero,0
20060820:	00092783          	lw	a5,0(s2)
20060824:	cf89                	c.beqz	a5,2006083e <inic_api_host_message_send+0xa2>
20060826:	4505                	c.li	a0,1
20060828:	0405                	c.addi	s0,1
2006082a:	bb6fb0ef          	jal	ra,2005bbe0 <rtos_time_delay_ms>
2006082e:	03946433          	rem	s0,s0,s9
20060832:	fc4d                	c.bnez	s0,200607ec <inic_api_host_message_send+0x50>
20060834:	894d0513          	addi	a0,s10,-1900 # 20064894 <register_string+0xe4>
20060838:	bcbfe0ef          	jal	ra,2005f402 <DiagPrintf_minimal>
2006083c:	b77d                	c.j	200607ea <inic_api_host_message_send+0x4e>
2006083e:	04000613          	addi	a2,zero,64
20060842:	4581                	c.li	a1,0
20060844:	600c0513          	addi	a0,s8,1536
20060848:	dffa9097          	auipc	ra,0xdffa9
2006084c:	286080e7          	jalr	ra,646(ra) # 9ace <__wrap_memset>
20060850:	01692023          	sw	s6,0(s2)
20060854:	00098b63          	beq	s3,zero,2006086a <inic_api_host_message_send+0xce>
20060858:	002b9613          	slli	a2,s7,0x2
2006085c:	85ce                	c.mv	a1,s3
2006085e:	00490513          	addi	a0,s2,4
20060862:	dffa9097          	auipc	ra,0xdffa9
20060866:	26e080e7          	jalr	ra,622(ra) # 9ad0 <__wrap_memcpy>
2006086a:	7c1027f3          	csrrs	a5,0x7c1,zero
2006086e:	1007f793          	andi	a5,a5,256
20060872:	c795                	c.beqz	a5,2006089e <inic_api_host_message_send+0x102>
20060874:	0ff0000f          	fence	iorw,iorw
20060878:	0494f00b          	cache	dwb,(s1)
2006087c:	02048793          	addi	a5,s1,32
20060880:	04f7f00b          	cache	dwb,(a5)
20060884:	0ff0000f          	fence	iorw,iorw
20060888:	0001                	c.addi	zero,0
2006088a:	0001                	c.addi	zero,0
2006088c:	0001                	c.addi	zero,0
2006088e:	0001                	c.addi	zero,0
20060890:	0001                	c.addi	zero,0
20060892:	0001                	c.addi	zero,0
20060894:	0001                	c.addi	zero,0
20060896:	0001                	c.addi	zero,0
20060898:	0001                	c.addi	zero,0
2006089a:	0001                	c.addi	zero,0
2006089c:	0001                	c.addi	zero,0
2006089e:	200689b7          	lui	s3,0x20068
200608a2:	4641                	c.li	a2,16
200608a4:	4581                	c.li	a1,0
200608a6:	5c098513          	addi	a0,s3,1472 # 200685c0 <g_host_ipc_api_msg>
200608aa:	dffa9097          	auipc	ra,0xdffa9
200608ae:	224080e7          	jalr	ra,548(ra) # 9ace <__wrap_memset>
200608b2:	5c098b13          	addi	s6,s3,1472
200608b6:	04000793          	addi	a5,zero,64
200608ba:	009b2223          	sw	s1,4(s6)
200608be:	000b2023          	sw	zero,0(s6)
200608c2:	00fb2423          	sw	a5,8(s6)
200608c6:	7c1027f3          	csrrs	a5,0x7c1,zero
200608ca:	1007f793          	andi	a5,a5,256
200608ce:	c395                	c.beqz	a5,200608f2 <inic_api_host_message_send+0x156>
200608d0:	0ff0000f          	fence	iorw,iorw
200608d4:	056b700b          	cache	dwb,(s6)
200608d8:	0ff0000f          	fence	iorw,iorw
200608dc:	0001                	c.addi	zero,0
200608de:	0001                	c.addi	zero,0
200608e0:	0001                	c.addi	zero,0
200608e2:	0001                	c.addi	zero,0
200608e4:	0001                	c.addi	zero,0
200608e6:	0001                	c.addi	zero,0
200608e8:	0001                	c.addi	zero,0
200608ea:	0001                	c.addi	zero,0
200608ec:	0001                	c.addi	zero,0
200608ee:	0001                	c.addi	zero,0
200608f0:	0001                	c.addi	zero,0
200608f2:	5c098613          	addi	a2,s3,1472
200608f6:	459d                	c.li	a1,7
200608f8:	4501                	c.li	a0,0
200608fa:	698d                	c.lui	s3,0x3
200608fc:	eacf80ef          	jal	ra,20058fa8 <ipc_send_message>
20060900:	02048b93          	addi	s7,s1,32
20060904:	ee098993          	addi	s3,s3,-288 # 2ee0 <FLASH_SetSpiMode+0x21c>
20060908:	20065b37          	lui	s6,0x20065
2006090c:	00092783          	lw	a5,0(s2)
20060910:	cba9                	c.beqz	a5,20060962 <inic_api_host_message_send+0x1c6>
20060912:	4505                	c.li	a0,1
20060914:	accfb0ef          	jal	ra,2005bbe0 <rtos_time_delay_ms>
20060918:	7c1027f3          	csrrs	a5,0x7c1,zero
2006091c:	1007f793          	andi	a5,a5,256
20060920:	c785                	c.beqz	a5,20060948 <inic_api_host_message_send+0x1ac>
20060922:	0ff0000f          	fence	iorw,iorw
20060926:	0094f00b          	cache	dinv,(s1)
2006092a:	017bf00b          	cache	dinv,(s7)
2006092e:	0ff0000f          	fence	iorw,iorw
20060932:	0001                	c.addi	zero,0
20060934:	0001                	c.addi	zero,0
20060936:	0001                	c.addi	zero,0
20060938:	0001                	c.addi	zero,0
2006093a:	0001                	c.addi	zero,0
2006093c:	0001                	c.addi	zero,0
2006093e:	0001                	c.addi	zero,0
20060940:	0001                	c.addi	zero,0
20060942:	0001                	c.addi	zero,0
20060944:	0001                	c.addi	zero,0
20060946:	0001                	c.addi	zero,0
20060948:	0405                	c.addi	s0,1
2006094a:	03346433          	rem	s0,s0,s3
2006094e:	fc5d                	c.bnez	s0,2006090c <inic_api_host_message_send+0x170>
20060950:	000aa603          	lw	a2,0(s5)
20060954:	00092583          	lw	a1,0(s2)
20060958:	8acb0513          	addi	a0,s6,-1876 # 200648ac <register_string+0xfc>
2006095c:	aa7fe0ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20060960:	b775                	c.j	2006090c <inic_api_host_message_send+0x170>
20060962:	000a2503          	lw	a0,0(s4)
20060966:	02892403          	lw	s0,40(s2)
2006096a:	fedfa0ef          	jal	ra,2005b956 <rtos_sema_give>
2006096e:	50f2                	c.lwsp	ra,60(sp)
20060970:	8522                	c.mv	a0,s0
20060972:	5462                	c.lwsp	s0,56(sp)
20060974:	54d2                	c.lwsp	s1,52(sp)
20060976:	5942                	c.lwsp	s2,48(sp)
20060978:	59b2                	c.lwsp	s3,44(sp)
2006097a:	5a22                	c.lwsp	s4,40(sp)
2006097c:	5a92                	c.lwsp	s5,36(sp)
2006097e:	5b02                	c.lwsp	s6,32(sp)
20060980:	4bf2                	c.lwsp	s7,28(sp)
20060982:	4c62                	c.lwsp	s8,24(sp)
20060984:	4cd2                	c.lwsp	s9,20(sp)
20060986:	4d42                	c.lwsp	s10,16(sp)
20060988:	4db2                	c.lwsp	s11,12(sp)
2006098a:	6121                	c.addi16sp	sp,64
2006098c:	8082                	c.jr	ra

2006098e <inic_api_init_host>:
2006098e:	1141                	c.addi	sp,-16
20060990:	567d                	c.li	a2,-1
20060992:	4581                	c.li	a1,0
20060994:	fd818513          	addi	a0,gp,-40 # 20066988 <g_host_inic_api_task_wake_sema>
20060998:	c606                	c.swsp	ra,12(sp)
2006099a:	c422                	c.swsp	s0,8(sp)
2006099c:	ef1fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
200609a0:	567d                	c.li	a2,-1
200609a2:	4581                	c.li	a1,0
200609a4:	fd418413          	addi	s0,gp,-44 # 20066984 <g_host_inic_api_message_send_sema>
200609a8:	fd418513          	addi	a0,gp,-44 # 20066984 <g_host_inic_api_message_send_sema>
200609ac:	ee1fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
200609b0:	4008                	c.lw	a0,0(s0)
200609b2:	fa5fa0ef          	jal	ra,2005b956 <rtos_sema_give>
200609b6:	200605b7          	lui	a1,0x20060
200609ba:	4701                	c.li	a4,0
200609bc:	4681                	c.li	a3,0
200609be:	4601                	c.li	a2,0
200609c0:	45e58593          	addi	a1,a1,1118 # 2006045e <_inic_ipc_ip_addr_update_in_wowlan>
200609c4:	4505                	c.li	a0,1
200609c6:	dabf80ef          	jal	ra,20059770 <pmu_register_sleep_callback>
200609ca:	6705                	c.lui	a4,0x1
200609cc:	20060637          	lui	a2,0x20060
200609d0:	200655b7          	lui	a1,0x20065
200609d4:	478d                	c.li	a5,3
200609d6:	ce070713          	addi	a4,a4,-800 # ce0 <CPU_ClkGet+0x1ca>
200609da:	4681                	c.li	a3,0
200609dc:	46260613          	addi	a2,a2,1122 # 20060462 <inic_api_host_task_h>
200609e0:	8d858593          	addi	a1,a1,-1832 # 200648d8 <register_string+0x128>
200609e4:	4501                	c.li	a0,0
200609e6:	9c4fb0ef          	jal	ra,2005bbaa <rtos_task_create>
200609ea:	c10d                	c.beqz	a0,20060a0c <inic_api_init_host+0x7e>
200609ec:	4422                	c.lwsp	s0,8(sp)
200609ee:	40b2                	c.lwsp	ra,12(sp)
200609f0:	200656b7          	lui	a3,0x20065
200609f4:	200655b7          	lui	a1,0x20065
200609f8:	8ec68693          	addi	a3,a3,-1812 # 200648ec <register_string+0x13c>
200609fc:	04500613          	addi	a2,zero,69
20060a00:	90858593          	addi	a1,a1,-1784 # 20064908 <register_string+0x158>
20060a04:	4509                	c.li	a0,2
20060a06:	0141                	c.addi	sp,16
20060a08:	f6efa06f          	jal	zero,2005b176 <rtk_log_write>
20060a0c:	40b2                	c.lwsp	ra,12(sp)
20060a0e:	4422                	c.lwsp	s0,8(sp)
20060a10:	0141                	c.addi	sp,16
20060a12:	8082                	c.jr	ra

20060a14 <inic_mp_command>:
20060a14:	7179                	c.addi16sp	sp,-48
20060a16:	ce4e                	c.swsp	s3,28(sp)
20060a18:	89aa                	c.mv	s3,a0
20060a1a:	6505                	c.lui	a0,0x1
20060a1c:	d04a                	c.swsp	s2,32(sp)
20060a1e:	cc52                	c.swsp	s4,24(sp)
20060a20:	d606                	c.swsp	ra,44(sp)
20060a22:	d422                	c.swsp	s0,40(sp)
20060a24:	d226                	c.swsp	s1,36(sp)
20060a26:	892e                	c.mv	s2,a1
20060a28:	8a32                	c.mv	s4,a2
20060a2a:	cb7fa0ef          	jal	ra,2005b6e0 <rtos_mem_zmalloc>
20060a2e:	e50d                	c.bnez	a0,20060a58 <inic_mp_command+0x44>
20060a30:	5422                	c.lwsp	s0,40(sp)
20060a32:	50b2                	c.lwsp	ra,44(sp)
20060a34:	5492                	c.lwsp	s1,36(sp)
20060a36:	5902                	c.lwsp	s2,32(sp)
20060a38:	49f2                	c.lwsp	s3,28(sp)
20060a3a:	4a62                	c.lwsp	s4,24(sp)
20060a3c:	200656b7          	lui	a3,0x20065
20060a40:	200655b7          	lui	a1,0x20065
20060a44:	91068693          	addi	a3,a3,-1776 # 20064910 <register_string+0x160>
20060a48:	04500613          	addi	a2,zero,69
20060a4c:	90858593          	addi	a1,a1,-1784 # 20064908 <register_string+0x158>
20060a50:	4509                	c.li	a0,2
20060a52:	6145                	c.addi16sp	sp,48
20060a54:	f22fa06f          	jal	zero,2005b176 <rtk_log_write>
20060a58:	842a                	c.mv	s0,a0
20060a5a:	87ce                	c.mv	a5,s3
20060a5c:	c04e                	c.swsp	s3,0(sp)
20060a5e:	c24a                	c.swsp	s2,4(sp)
20060a60:	c452                	c.swsp	s4,8(sp)
20060a62:	84aa                	c.mv	s1,a0
20060a64:	c62a                	c.swsp	a0,12(sp)
20060a66:	7c102773          	csrrs	a4,0x7c1,zero
20060a6a:	10077713          	andi	a4,a4,256
20060a6e:	c721                	c.beqz	a4,20060ab6 <inic_mp_command+0xa2>
20060a70:	01f57693          	andi	a3,a0,31
20060a74:	10068363          	beq	a3,zero,20060b7a <inic_mp_command+0x166>
20060a78:	6605                	c.lui	a2,0x1
20060a7a:	9636                	c.add	a2,a3
20060a7c:	fe057713          	andi	a4,a0,-32
20060a80:	01f77693          	andi	a3,a4,31
20060a84:	96b2                	c.add	a3,a2
20060a86:	0ff0000f          	fence	iorw,iorw
20060a8a:	96ba                	c.add	a3,a4
20060a8c:	04e7700b          	cache	dwb,(a4)
20060a90:	02070713          	addi	a4,a4,32
20060a94:	40e68633          	sub	a2,a3,a4
20060a98:	fec04ae3          	blt	zero,a2,20060a8c <inic_mp_command+0x78>
20060a9c:	0ff0000f          	fence	iorw,iorw
20060aa0:	0001                	c.addi	zero,0
20060aa2:	0001                	c.addi	zero,0
20060aa4:	0001                	c.addi	zero,0
20060aa6:	0001                	c.addi	zero,0
20060aa8:	0001                	c.addi	zero,0
20060aaa:	0001                	c.addi	zero,0
20060aac:	0001                	c.addi	zero,0
20060aae:	0001                	c.addi	zero,0
20060ab0:	0001                	c.addi	zero,0
20060ab2:	0001                	c.addi	zero,0
20060ab4:	0001                	c.addi	zero,0
20060ab6:	7c102773          	csrrs	a4,0x7c1,zero
20060aba:	10077713          	andi	a4,a4,256
20060abe:	c70d                	c.beqz	a4,20060ae8 <inic_mp_command+0xd4>
20060ac0:	577d                	c.li	a4,-1
20060ac2:	0ae99f63          	bne	s3,a4,20060b80 <inic_mp_command+0x16c>
20060ac6:	0b391d63          	bne	s2,s3,20060b80 <inic_mp_command+0x16c>
20060aca:	7c0267f3          	csrrsi	a5,0x7c0,4
20060ace:	0ff0000f          	fence	iorw,iorw
20060ad2:	0001                	c.addi	zero,0
20060ad4:	0001                	c.addi	zero,0
20060ad6:	0001                	c.addi	zero,0
20060ad8:	0001                	c.addi	zero,0
20060ada:	0001                	c.addi	zero,0
20060adc:	0001                	c.addi	zero,0
20060ade:	0001                	c.addi	zero,0
20060ae0:	0001                	c.addi	zero,0
20060ae2:	0001                	c.addi	zero,0
20060ae4:	0001                	c.addi	zero,0
20060ae6:	0001                	c.addi	zero,0
20060ae8:	650d                	c.lui	a0,0x3
20060aea:	4611                	c.li	a2,4
20060aec:	858a                	c.mv	a1,sp
20060aee:	050d                	c.addi	a0,3
20060af0:	cadff0ef          	jal	ra,2006079c <inic_api_host_message_send>
20060af4:	060a0863          	beq	s4,zero,20060b64 <inic_mp_command+0x150>
20060af8:	7c1027f3          	csrrs	a5,0x7c1,zero
20060afc:	1007f793          	andi	a5,a5,256
20060b00:	c7a1                	c.beqz	a5,20060b48 <inic_mp_command+0x134>
20060b02:	01f47713          	andi	a4,s0,31
20060b06:	6785                	c.lui	a5,0x1
20060b08:	c701                	c.beqz	a4,20060b10 <inic_mp_command+0xfc>
20060b0a:	97ba                	c.add	a5,a4
20060b0c:	fe047493          	andi	s1,s0,-32
20060b10:	01f4f713          	andi	a4,s1,31
20060b14:	973e                	c.add	a4,a5
20060b16:	0ff0000f          	fence	iorw,iorw
20060b1a:	87a6                	c.mv	a5,s1
20060b1c:	9726                	c.add	a4,s1
20060b1e:	00f7f00b          	cache	dinv,(a5)
20060b22:	02078793          	addi	a5,a5,32 # 1020 <CPU_ClkGet+0x50a>
20060b26:	40f706b3          	sub	a3,a4,a5
20060b2a:	fed04ae3          	blt	zero,a3,20060b1e <inic_mp_command+0x10a>
20060b2e:	0ff0000f          	fence	iorw,iorw
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
20060b46:	0001                	c.addi	zero,0
20060b48:	200656b7          	lui	a3,0x20065
20060b4c:	200655b7          	lui	a1,0x20065
20060b50:	8722                	c.mv	a4,s0
20060b52:	92868693          	addi	a3,a3,-1752 # 20064928 <register_string+0x178>
20060b56:	04100613          	addi	a2,zero,65
20060b5a:	90858593          	addi	a1,a1,-1784 # 20064908 <register_string+0x158>
20060b5e:	4505                	c.li	a0,1
20060b60:	e16fa0ef          	jal	ra,2005b176 <rtk_log_write>
20060b64:	8522                	c.mv	a0,s0
20060b66:	ba1fa0ef          	jal	ra,2005b706 <rtos_mem_free>
20060b6a:	50b2                	c.lwsp	ra,44(sp)
20060b6c:	5422                	c.lwsp	s0,40(sp)
20060b6e:	5492                	c.lwsp	s1,36(sp)
20060b70:	5902                	c.lwsp	s2,32(sp)
20060b72:	49f2                	c.lwsp	s3,28(sp)
20060b74:	4a62                	c.lwsp	s4,24(sp)
20060b76:	6145                	c.addi16sp	sp,48
20060b78:	8082                	c.jr	ra
20060b7a:	872a                	c.mv	a4,a0
20060b7c:	6605                	c.lui	a2,0x1
20060b7e:	b709                	c.j	20060a80 <inic_mp_command+0x6c>
20060b80:	01f9f713          	andi	a4,s3,31
20060b84:	c701                	c.beqz	a4,20060b8c <inic_mp_command+0x178>
20060b86:	fe09f793          	andi	a5,s3,-32
20060b8a:	993a                	c.add	s2,a4
20060b8c:	f5205ee3          	bge	zero,s2,20060ae8 <inic_mp_command+0xd4>
20060b90:	01f7f593          	andi	a1,a5,31
20060b94:	992e                	c.add	s2,a1
20060b96:	0ff0000f          	fence	iorw,iorw
20060b9a:	993e                	c.add	s2,a5
20060b9c:	04f7f00b          	cache	dwb,(a5)
20060ba0:	02078793          	addi	a5,a5,32
20060ba4:	40f90733          	sub	a4,s2,a5
20060ba8:	fee04ae3          	blt	zero,a4,20060b9c <inic_mp_command+0x188>
20060bac:	b70d                	c.j	20060ace <inic_mp_command+0xba>

20060bae <inic_host_init>:
20060bae:	1141                	c.addi	sp,-16
20060bb0:	c606                	c.swsp	ra,12(sp)
20060bb2:	2239                	c.jal	20060cc0 <inic_host_init_priv>
20060bb4:	40b2                	c.lwsp	ra,12(sp)
20060bb6:	0141                	c.addi	sp,16
20060bb8:	dd7ff06f          	jal	zero,2006098e <inic_api_init_host>

20060bbc <inic_host_init_skb>:
20060bbc:	1141                	c.addi	sp,-16
20060bbe:	c422                	c.swsp	s0,8(sp)
20060bc0:	fe01a783          	lw	a5,-32(gp) # 20066990 <host_skb_buff>
20060bc4:	c606                	c.swsp	ra,12(sp)
20060bc6:	c226                	c.swsp	s1,4(sp)
20060bc8:	c395                	c.beqz	a5,20060bec <inic_host_init_skb+0x30>
20060bca:	4422                	c.lwsp	s0,8(sp)
20060bcc:	40b2                	c.lwsp	ra,12(sp)
20060bce:	4492                	c.lwsp	s1,4(sp)
20060bd0:	200656b7          	lui	a3,0x20065
20060bd4:	200655b7          	lui	a1,0x20065
20060bd8:	94068693          	addi	a3,a3,-1728 # 20064940 <register_string+0x190>
20060bdc:	04500613          	addi	a2,zero,69
20060be0:	90858593          	addi	a1,a1,-1784 # 20064908 <register_string+0x158>
20060be4:	4509                	c.li	a0,2
20060be6:	0141                	c.addi	sp,16
20060be8:	d8efa06f          	jal	zero,2005b176 <rtk_log_write>
20060bec:	66000793          	addi	a5,zero,1632
20060bf0:	fe418713          	addi	a4,gp,-28 # 20066994 <skb_buf_max_size>
20060bf4:	c31c                	c.sw	a5,0(a4)
20060bf6:	41018793          	addi	a5,gp,1040 # 20066dc0 <wifi_user_config>
20060bfa:	57dc                	c.lw	a5,44(a5)
20060bfc:	fe018413          	addi	s0,gp,-32 # 20066990 <host_skb_buff>
20060c00:	41018493          	addi	s1,gp,1040 # 20066dc0 <wifi_user_config>
20060c04:	c781                	c.beqz	a5,20060c0c <inic_host_init_skb+0x50>
20060c06:	07fd                	c.addi	a5,31
20060c08:	9b81                	c.andi	a5,-32
20060c0a:	c31c                	c.sw	a5,0(a4)
20060c0c:	549c                	c.lw	a5,40(s1)
20060c0e:	6c000513          	addi	a0,zero,1728
20060c12:	02f50533          	mul	a0,a0,a5
20060c16:	acbfa0ef          	jal	ra,2005b6e0 <rtos_mem_zmalloc>
20060c1a:	c008                	c.sw	a0,0(s0)
20060c1c:	e115                	c.bnez	a0,20060c40 <inic_host_init_skb+0x84>
20060c1e:	20065737          	lui	a4,0x20065
20060c22:	200656b7          	lui	a3,0x20065
20060c26:	200655b7          	lui	a1,0x20065
20060c2a:	98c70713          	addi	a4,a4,-1652 # 2006498c <__func__.1>
20060c2e:	95868693          	addi	a3,a3,-1704 # 20064958 <register_string+0x1a8>
20060c32:	04500613          	addi	a2,zero,69
20060c36:	90858593          	addi	a1,a1,-1784 # 20064908 <register_string+0x158>
20060c3a:	4509                	c.li	a0,2
20060c3c:	d3afa0ef          	jal	ra,2005b176 <rtk_log_write>
20060c40:	4681                	c.li	a3,0
20060c42:	6c000613          	addi	a2,zero,1728
20060c46:	5498                	c.lw	a4,40(s1)
20060c48:	401c                	c.lw	a5,0(s0)
20060c4a:	06e6c263          	blt	a3,a4,20060cae <inic_host_init_skb+0xf2>
20060c4e:	7c1026f3          	csrrs	a3,0x7c1,zero
20060c52:	1006f693          	andi	a3,a3,256
20060c56:	c6b9                	c.beqz	a3,20060ca4 <inic_host_init_skb+0xe8>
20060c58:	6c000693          	addi	a3,zero,1728
20060c5c:	02d70733          	mul	a4,a4,a3
20060c60:	01f7f693          	andi	a3,a5,31
20060c64:	c299                	c.beqz	a3,20060c6a <inic_host_init_skb+0xae>
20060c66:	9b81                	c.andi	a5,-32
20060c68:	9736                	c.add	a4,a3
20060c6a:	02e05d63          	bge	zero,a4,20060ca4 <inic_host_init_skb+0xe8>
20060c6e:	01f7f693          	andi	a3,a5,31
20060c72:	9736                	c.add	a4,a3
20060c74:	0ff0000f          	fence	iorw,iorw
20060c78:	973e                	c.add	a4,a5
20060c7a:	04f7f00b          	cache	dwb,(a5)
20060c7e:	02078793          	addi	a5,a5,32
20060c82:	40f706b3          	sub	a3,a4,a5
20060c86:	fed04ae3          	blt	zero,a3,20060c7a <inic_host_init_skb+0xbe>
20060c8a:	0ff0000f          	fence	iorw,iorw
20060c8e:	0001                	c.addi	zero,0
20060c90:	0001                	c.addi	zero,0
20060c92:	0001                	c.addi	zero,0
20060c94:	0001                	c.addi	zero,0
20060c96:	0001                	c.addi	zero,0
20060c98:	0001                	c.addi	zero,0
20060c9a:	0001                	c.addi	zero,0
20060c9c:	0001                	c.addi	zero,0
20060c9e:	0001                	c.addi	zero,0
20060ca0:	0001                	c.addi	zero,0
20060ca2:	0001                	c.addi	zero,0
20060ca4:	40b2                	c.lwsp	ra,12(sp)
20060ca6:	4422                	c.lwsp	s0,8(sp)
20060ca8:	4492                	c.lwsp	s1,4(sp)
20060caa:	0141                	c.addi	sp,16
20060cac:	8082                	c.jr	ra
20060cae:	02c68733          	mul	a4,a3,a2
20060cb2:	0685                	c.addi	a3,1
20060cb4:	97ba                	c.add	a5,a4
20060cb6:	c39c                	c.sw	a5,0(a5)
20060cb8:	401c                	c.lw	a5,0(s0)
20060cba:	97ba                	c.add	a5,a4
20060cbc:	c3dc                	c.sw	a5,4(a5)
20060cbe:	b761                	c.j	20060c46 <inic_host_init_skb+0x8a>

20060cc0 <inic_host_init_priv>:
20060cc0:	1141                	c.addi	sp,-16
20060cc2:	c226                	c.swsp	s1,4(sp)
20060cc4:	200684b7          	lui	s1,0x20068
20060cc8:	02800613          	addi	a2,zero,40
20060ccc:	4581                	c.li	a1,0
20060cce:	64048513          	addi	a0,s1,1600 # 20068640 <g_inic_host_priv>
20060cd2:	c606                	c.swsp	ra,12(sp)
20060cd4:	c422                	c.swsp	s0,8(sp)
20060cd6:	dffa9097          	auipc	ra,0xdffa9
20060cda:	df8080e7          	jalr	ra,-520(ra) # 9ace <__wrap_memset>
20060cde:	567d                	c.li	a2,-1
20060ce0:	4581                	c.li	a1,0
20060ce2:	64048513          	addi	a0,s1,1600
20060ce6:	64048413          	addi	s0,s1,1600
20060cea:	ba3fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060cee:	567d                	c.li	a2,-1
20060cf0:	4581                	c.li	a1,0
20060cf2:	00440513          	addi	a0,s0,4
20060cf6:	b97fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060cfa:	567d                	c.li	a2,-1
20060cfc:	4581                	c.li	a1,0
20060cfe:	00840513          	addi	a0,s0,8
20060d02:	b8bfa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060d06:	4408                	c.lw	a0,8(s0)
20060d08:	c4ffa0ef          	jal	ra,2005b956 <rtos_sema_give>
20060d0c:	00c40513          	addi	a0,s0,12
20060d10:	2ea5                	c.jal	20061088 <rtw_init_queue>
20060d12:	40b2                	c.lwsp	ra,12(sp)
20060d14:	00042c23          	sw	zero,24(s0)
20060d18:	00042e23          	sw	zero,28(s0)
20060d1c:	02042023          	sw	zero,32(s0)
20060d20:	02042223          	sw	zero,36(s0)
20060d24:	4422                	c.lwsp	s0,8(sp)
20060d26:	4492                	c.lwsp	s1,4(sp)
20060d28:	0141                	c.addi	sp,16
20060d2a:	8082                	c.jr	ra

20060d2c <inic_host_trx_event_hdl>:
20060d2c:	4791                	c.li	a5,4
20060d2e:	872a                	c.mv	a4,a0
20060d30:	00f50f63          	beq	a0,a5,20060d4e <inic_host_trx_event_hdl+0x22>
20060d34:	200656b7          	lui	a3,0x20065
20060d38:	200655b7          	lui	a1,0x20065
20060d3c:	97068693          	addi	a3,a3,-1680 # 20064970 <register_string+0x1c0>
20060d40:	04500613          	addi	a2,zero,69
20060d44:	90858593          	addi	a1,a1,-1784 # 20064908 <register_string+0x158>
20060d48:	4509                	c.li	a0,2
20060d4a:	c2cfa06f          	jal	zero,2005b176 <rtk_log_write>
20060d4e:	8082                	c.jr	ra

20060d50 <inic_host_trx_int_hdl>:
20060d50:	1141                	c.addi	sp,-16
20060d52:	4599                	c.li	a1,6
20060d54:	4541                	c.li	a0,16
20060d56:	c422                	c.swsp	s0,8(sp)
20060d58:	c606                	c.swsp	ra,12(sp)
20060d5a:	b8af80ef          	jal	ra,200590e4 <ipc_get_message>
20060d5e:	4140                	c.lw	s0,4(a0)
20060d60:	7c1027f3          	csrrs	a5,0x7c1,zero
20060d64:	1007f793          	andi	a5,a5,256
20060d68:	c3b9                	c.beqz	a5,20060dae <inic_host_trx_int_hdl+0x5e>
20060d6a:	01f47713          	andi	a4,s0,31
20060d6e:	c755                	c.beqz	a4,20060e1a <inic_host_trx_int_hdl+0xca>
20060d70:	04070693          	addi	a3,a4,64
20060d74:	fe047793          	andi	a5,s0,-32
20060d78:	01f7f713          	andi	a4,a5,31
20060d7c:	9736                	c.add	a4,a3
20060d7e:	0ff0000f          	fence	iorw,iorw
20060d82:	973e                	c.add	a4,a5
20060d84:	00f7f00b          	cache	dinv,(a5)
20060d88:	02078793          	addi	a5,a5,32
20060d8c:	40f706b3          	sub	a3,a4,a5
20060d90:	fed04ae3          	blt	zero,a3,20060d84 <inic_host_trx_int_hdl+0x34>
20060d94:	0ff0000f          	fence	iorw,iorw
20060d98:	0001                	c.addi	zero,0
20060d9a:	0001                	c.addi	zero,0
20060d9c:	0001                	c.addi	zero,0
20060d9e:	0001                	c.addi	zero,0
20060da0:	0001                	c.addi	zero,0
20060da2:	0001                	c.addi	zero,0
20060da4:	0001                	c.addi	zero,0
20060da6:	0001                	c.addi	zero,0
20060da8:	0001                	c.addi	zero,0
20060daa:	0001                	c.addi	zero,0
20060dac:	0001                	c.addi	zero,0
20060dae:	2455                	c.jal	20061052 <inic_msg_get_queue_status>
20060db0:	c92d                	c.beqz	a0,20060e22 <inic_host_trx_int_hdl+0xd2>
20060db2:	8522                	c.mv	a0,s0
20060db4:	2af1                	c.jal	20060f90 <inic_msg_enqueue>
20060db6:	157d                	c.addi	a0,-1
20060db8:	00a03533          	sltu	a0,zero,a0
20060dbc:	c408                	c.sw	a0,8(s0)
20060dbe:	00042023          	sw	zero,0(s0)
20060dc2:	7c1027f3          	csrrs	a5,0x7c1,zero
20060dc6:	1007f793          	andi	a5,a5,256
20060dca:	c7a1                	c.beqz	a5,20060e12 <inic_host_trx_int_hdl+0xc2>
20060dcc:	01f47793          	andi	a5,s0,31
20060dd0:	04000713          	addi	a4,zero,64
20060dd4:	c781                	c.beqz	a5,20060ddc <inic_host_trx_int_hdl+0x8c>
20060dd6:	04078713          	addi	a4,a5,64
20060dda:	9801                	c.andi	s0,-32
20060ddc:	01f47793          	andi	a5,s0,31
20060de0:	97ba                	c.add	a5,a4
20060de2:	0ff0000f          	fence	iorw,iorw
20060de6:	97a2                	c.add	a5,s0
20060de8:	0484700b          	cache	dwb,(s0)
20060dec:	02040413          	addi	s0,s0,32
20060df0:	40878733          	sub	a4,a5,s0
20060df4:	fee04ae3          	blt	zero,a4,20060de8 <inic_host_trx_int_hdl+0x98>
20060df8:	0ff0000f          	fence	iorw,iorw
20060dfc:	0001                	c.addi	zero,0
20060dfe:	0001                	c.addi	zero,0
20060e00:	0001                	c.addi	zero,0
20060e02:	0001                	c.addi	zero,0
20060e04:	0001                	c.addi	zero,0
20060e06:	0001                	c.addi	zero,0
20060e08:	0001                	c.addi	zero,0
20060e0a:	0001                	c.addi	zero,0
20060e0c:	0001                	c.addi	zero,0
20060e0e:	0001                	c.addi	zero,0
20060e10:	0001                	c.addi	zero,0
20060e12:	40b2                	c.lwsp	ra,12(sp)
20060e14:	4422                	c.lwsp	s0,8(sp)
20060e16:	0141                	c.addi	sp,16
20060e18:	8082                	c.jr	ra
20060e1a:	87a2                	c.mv	a5,s0
20060e1c:	04000693          	addi	a3,zero,64
20060e20:	bfa1                	c.j	20060d78 <inic_host_trx_int_hdl+0x28>
20060e22:	00044503          	lbu	a0,0(s0)
20060e26:	00c44603          	lbu	a2,12(s0)
20060e2a:	404c                	c.lw	a1,4(s0)
20060e2c:	f01ff0ef          	jal	ra,20060d2c <inic_host_trx_event_hdl>
20060e30:	4501                	c.li	a0,0
20060e32:	b769                	c.j	20060dbc <inic_host_trx_int_hdl+0x6c>

20060e34 <inic_msg_q_task>:
20060e34:	1101                	c.addi	sp,-32
20060e36:	cc22                	c.swsp	s0,24(sp)
20060e38:	20068437          	lui	s0,0x20068
20060e3c:	c84a                	c.swsp	s2,16(sp)
20060e3e:	c64e                	c.swsp	s3,12(sp)
20060e40:	6c040913          	addi	s2,s0,1728 # 200686c0 <g_ipc_msg_q_priv>
20060e44:	ce06                	c.swsp	ra,28(sp)
20060e46:	ca26                	c.swsp	s1,20(sp)
20060e48:	6c040413          	addi	s0,s0,1728
20060e4c:	4985                	c.li	s3,1
20060e4e:	00c92503          	lw	a0,12(s2)
20060e52:	55fd                	c.li	a1,-1
20060e54:	ab7fa0ef          	jal	ra,2005b90a <rtos_sema_take>
20060e58:	839fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
20060e5c:	8522                	c.mv	a0,s0
20060e5e:	2c25                	c.jal	20061096 <rtw_queue_empty>
20060e60:	03351063          	bne	a0,s3,20060e80 <inic_msg_q_task+0x4c>
20060e64:	851fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20060e68:	01444783          	lbu	a5,20(s0)
20060e6c:	f3ed                	c.bnez	a5,20060e4e <inic_msg_q_task+0x1a>
20060e6e:	4462                	c.lwsp	s0,24(sp)
20060e70:	40f2                	c.lwsp	ra,28(sp)
20060e72:	44d2                	c.lwsp	s1,20(sp)
20060e74:	4942                	c.lwsp	s2,16(sp)
20060e76:	49b2                	c.lwsp	s3,12(sp)
20060e78:	4501                	c.li	a0,0
20060e7a:	6105                	c.addi16sp	sp,32
20060e7c:	d53fa06f          	jal	zero,2005bbce <rtos_task_delete>
20060e80:	4004                	c.lw	s1,0(s0)
20060e82:	8526                	c.mv	a0,s1
20060e84:	2add                	c.jal	2006107a <rtw_list_delete>
20060e86:	82ffa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20060e8a:	dcf9                	c.beqz	s1,20060e68 <inic_msg_q_task+0x34>
20060e8c:	44c8                	c.lw	a0,12(s1)
20060e8e:	448c                	c.lw	a1,8(s1)
20060e90:	00155613          	srli	a2,a0,0x1
20060e94:	810d                	c.srli	a0,0x3
20060e96:	8a0d                	c.andi	a2,3
20060e98:	897d                	c.andi	a0,31
20060e9a:	e93ff0ef          	jal	ra,20060d2c <inic_host_trx_event_hdl>
20060e9e:	ff2fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
20060ea2:	00c4c783          	lbu	a5,12(s1)
20060ea6:	9bf9                	c.andi	a5,-2
20060ea8:	00f48623          	sb	a5,12(s1)
20060eac:	809fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20060eb0:	b765                	c.j	20060e58 <inic_msg_q_task+0x24>

20060eb2 <inic_msg_q_init>:
20060eb2:	1141                	c.addi	sp,-16
20060eb4:	c422                	c.swsp	s0,8(sp)
20060eb6:	20068437          	lui	s0,0x20068
20060eba:	02000613          	addi	a2,zero,32
20060ebe:	4581                	c.li	a1,0
20060ec0:	6c040513          	addi	a0,s0,1728 # 200686c0 <g_ipc_msg_q_priv>
20060ec4:	c606                	c.swsp	ra,12(sp)
20060ec6:	c226                	c.swsp	s1,4(sp)
20060ec8:	dffa9097          	auipc	ra,0xdffa9
20060ecc:	c06080e7          	jalr	ra,-1018(ra) # 9ace <__wrap_memset>
20060ed0:	20068537          	lui	a0,0x20068
20060ed4:	04000613          	addi	a2,zero,64
20060ed8:	4581                	c.li	a1,0
20060eda:	68050513          	addi	a0,a0,1664 # 20068680 <g_inic_ipc_ex_msg>
20060ede:	dffa9097          	auipc	ra,0xdffa9
20060ee2:	bf0080e7          	jalr	ra,-1040(ra) # 9ace <__wrap_memset>
20060ee6:	6c040513          	addi	a0,s0,1728
20060eea:	6c040493          	addi	s1,s0,1728
20060eee:	2a69                	c.jal	20061088 <rtw_init_queue>
20060ef0:	567d                	c.li	a2,-1
20060ef2:	4581                	c.li	a1,0
20060ef4:	00c48513          	addi	a0,s1,12
20060ef8:	995fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060efc:	567d                	c.li	a2,-1
20060efe:	4581                	c.li	a1,0
20060f00:	01048513          	addi	a0,s1,16
20060f04:	989fa0ef          	jal	ra,2005b88c <rtos_sema_create_static>
20060f08:	4888                	c.lw	a0,16(s1)
20060f0a:	6c040413          	addi	s0,s0,1728
20060f0e:	a49fa0ef          	jal	ra,2005b956 <rtos_sema_give>
20060f12:	4341a503          	lw	a0,1076(gp) # 20066de4 <wifi_user_config+0x24>
20060f16:	00a48e23          	sb	a0,28(s1)
20060f1a:	0ff57513          	andi	a0,a0,255
20060f1e:	0512                	c.slli	a0,0x4
20060f20:	fc0fa0ef          	jal	ra,2005b6e0 <rtos_mem_zmalloc>
20060f24:	cc88                	c.sw	a0,24(s1)
20060f26:	4781                	c.li	a5,0
20060f28:	01c44703          	lbu	a4,28(s0)
20060f2c:	04e7c763          	blt	a5,a4,20060f7a <inic_msg_q_init+0xc8>
20060f30:	20061637          	lui	a2,0x20061
20060f34:	200655b7          	lui	a1,0x20065
20060f38:	4799                	c.li	a5,6
20060f3a:	41800713          	addi	a4,zero,1048
20060f3e:	4681                	c.li	a3,0
20060f40:	e3460613          	addi	a2,a2,-460 # 20060e34 <inic_msg_q_task>
20060f44:	9a058593          	addi	a1,a1,-1632 # 200649a0 <__func__.1+0x14>
20060f48:	4501                	c.li	a0,0
20060f4a:	c61fa0ef          	jal	ra,2005bbaa <rtos_task_create>
20060f4e:	cd11                	c.beqz	a0,20060f6a <inic_msg_q_init+0xb8>
20060f50:	200656b7          	lui	a3,0x20065
20060f54:	200655b7          	lui	a1,0x20065
20060f58:	9b068693          	addi	a3,a3,-1616 # 200649b0 <__func__.1+0x24>
20060f5c:	04500613          	addi	a2,zero,69
20060f60:	90858593          	addi	a1,a1,-1784 # 20064908 <register_string+0x158>
20060f64:	4509                	c.li	a0,2
20060f66:	a10fa0ef          	jal	ra,2005b176 <rtk_log_write>
20060f6a:	4785                	c.li	a5,1
20060f6c:	40b2                	c.lwsp	ra,12(sp)
20060f6e:	00f40a23          	sb	a5,20(s0)
20060f72:	4422                	c.lwsp	s0,8(sp)
20060f74:	4492                	c.lwsp	s1,4(sp)
20060f76:	0141                	c.addi	sp,16
20060f78:	8082                	c.jr	ra
20060f7a:	4c18                	c.lw	a4,24(s0)
20060f7c:	00479693          	slli	a3,a5,0x4
20060f80:	0785                	c.addi	a5,1
20060f82:	9736                	c.add	a4,a3
20060f84:	00c74683          	lbu	a3,12(a4)
20060f88:	9af9                	c.andi	a3,-2
20060f8a:	00d70623          	sb	a3,12(a4)
20060f8e:	bf69                	c.j	20060f28 <inic_msg_q_init+0x76>

20060f90 <inic_msg_enqueue>:
20060f90:	1101                	c.addi	sp,-32
20060f92:	ca26                	c.swsp	s1,20(sp)
20060f94:	c84a                	c.swsp	s2,16(sp)
20060f96:	c64e                	c.swsp	s3,12(sp)
20060f98:	200684b7          	lui	s1,0x20068
20060f9c:	ce06                	c.swsp	ra,28(sp)
20060f9e:	cc22                	c.swsp	s0,24(sp)
20060fa0:	89aa                	c.mv	s3,a0
20060fa2:	eeefa0ef          	jal	ra,2005b690 <rtos_critical_enter>
20060fa6:	6c048793          	addi	a5,s1,1728 # 200686c0 <g_ipc_msg_q_priv>
20060faa:	01c7c683          	lbu	a3,28(a5)
20060fae:	4f90                	c.lw	a2,24(a5)
20060fb0:	6c048913          	addi	s2,s1,1728
20060fb4:	4781                	c.li	a5,0
20060fb6:	00d7c463          	blt	a5,a3,20060fbe <inic_msg_enqueue+0x2e>
20060fba:	4401                	c.li	s0,0
20060fbc:	a829                	c.j	20060fd6 <inic_msg_enqueue+0x46>
20060fbe:	00479413          	slli	s0,a5,0x4
20060fc2:	9432                	c.add	s0,a2
20060fc4:	00c44703          	lbu	a4,12(s0)
20060fc8:	00177593          	andi	a1,a4,1
20060fcc:	e1b1                	c.bnez	a1,20061010 <inic_msg_enqueue+0x80>
20060fce:	00176713          	ori	a4,a4,1
20060fd2:	00e40623          	sb	a4,12(s0)
20060fd6:	edefa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20060fda:	ec0d                	c.bnez	s0,20061014 <inic_msg_enqueue+0x84>
20060fdc:	200656b7          	lui	a3,0x20065
20060fe0:	200655b7          	lui	a1,0x20065
20060fe4:	9c868693          	addi	a3,a3,-1592 # 200649c8 <__func__.1+0x3c>
20060fe8:	04500613          	addi	a2,zero,69
20060fec:	90858593          	addi	a1,a1,-1784 # 20064908 <register_string+0x158>
20060ff0:	4509                	c.li	a0,2
20060ff2:	984fa0ef          	jal	ra,2005b176 <rtk_log_write>
20060ff6:	547d                	c.li	s0,-1
20060ff8:	00c92503          	lw	a0,12(s2)
20060ffc:	95bfa0ef          	jal	ra,2005b956 <rtos_sema_give>
20061000:	40f2                	c.lwsp	ra,28(sp)
20061002:	8522                	c.mv	a0,s0
20061004:	4462                	c.lwsp	s0,24(sp)
20061006:	44d2                	c.lwsp	s1,20(sp)
20061008:	4942                	c.lwsp	s2,16(sp)
2006100a:	49b2                	c.lwsp	s3,12(sp)
2006100c:	6105                	c.addi16sp	sp,32
2006100e:	8082                	c.jr	ra
20061010:	0785                	c.addi	a5,1
20061012:	b755                	c.j	20060fb6 <inic_msg_enqueue+0x26>
20061014:	0009a783          	lw	a5,0(s3)
20061018:	00379713          	slli	a4,a5,0x3
2006101c:	00c44783          	lbu	a5,12(s0)
20061020:	8b9d                	c.andi	a5,7
20061022:	8fd9                	c.or	a5,a4
20061024:	00f40623          	sb	a5,12(s0)
20061028:	0049a703          	lw	a4,4(s3)
2006102c:	9be5                	c.andi	a5,-7
2006102e:	c418                	c.sw	a4,8(s0)
20061030:	00c9a703          	lw	a4,12(s3)
20061034:	8b0d                	c.andi	a4,3
20061036:	0706                	c.slli	a4,0x1
20061038:	8fd9                	c.or	a5,a4
2006103a:	00f40623          	sb	a5,12(s0)
2006103e:	e52fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
20061042:	8522                	c.mv	a0,s0
20061044:	6c048593          	addi	a1,s1,1728
20061048:	201d                	c.jal	2006106e <rtw_list_insert_tail>
2006104a:	4405                	c.li	s0,1
2006104c:	e68fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
20061050:	b765                	c.j	20060ff8 <inic_msg_enqueue+0x68>

20061052 <inic_msg_get_queue_status>:
20061052:	200687b7          	lui	a5,0x20068
20061056:	6d47c503          	lbu	a0,1748(a5) # 200686d4 <g_ipc_msg_q_priv+0x14>
2006105a:	8082                	c.jr	ra

2006105c <rtw_init_listhead>:
2006105c:	c108                	c.sw	a0,0(a0)
2006105e:	c148                	c.sw	a0,4(a0)
20061060:	8082                	c.jr	ra

20061062 <rtw_list_insert_head>:
20061062:	419c                	c.lw	a5,0(a1)
20061064:	c3c8                	c.sw	a0,4(a5)
20061066:	c11c                	c.sw	a5,0(a0)
20061068:	c14c                	c.sw	a1,4(a0)
2006106a:	c188                	c.sw	a0,0(a1)
2006106c:	8082                	c.jr	ra

2006106e <rtw_list_insert_tail>:
2006106e:	41dc                	c.lw	a5,4(a1)
20061070:	c1c8                	c.sw	a0,4(a1)
20061072:	c10c                	c.sw	a1,0(a0)
20061074:	c15c                	c.sw	a5,4(a0)
20061076:	c388                	c.sw	a0,0(a5)
20061078:	8082                	c.jr	ra

2006107a <rtw_list_delete>:
2006107a:	415c                	c.lw	a5,4(a0)
2006107c:	4118                	c.lw	a4,0(a0)
2006107e:	c35c                	c.sw	a5,4(a4)
20061080:	c398                	c.sw	a4,0(a5)
20061082:	c108                	c.sw	a0,0(a0)
20061084:	c148                	c.sw	a0,4(a0)
20061086:	8082                	c.jr	ra

20061088 <rtw_init_queue>:
20061088:	c108                	c.sw	a0,0(a0)
2006108a:	fe81a783          	lw	a5,-24(gp) # 20066998 <p_wifi_rom2flash>
2006108e:	c148                	c.sw	a0,4(a0)
20061090:	0521                	c.addi	a0,8
20061092:	57dc                	c.lw	a5,44(a5)
20061094:	8782                	c.jr	a5

20061096 <rtw_queue_empty>:
20061096:	411c                	c.lw	a5,0(a0)
20061098:	40a78533          	sub	a0,a5,a0
2006109c:	00153513          	sltiu	a0,a0,1
200610a0:	8082                	c.jr	ra

200610a2 <rtw_end_of_queue_search>:
200610a2:	8d0d                	c.sub	a0,a1
200610a4:	00153513          	sltiu	a0,a0,1
200610a8:	8082                	c.jr	ra

200610aa <timer_wrapper>:
200610aa:	1101                	c.addi	sp,-32
200610ac:	ca26                	c.swsp	s1,20(sp)
200610ae:	cc22                	c.swsp	s0,24(sp)
200610b0:	c84a                	c.swsp	s2,16(sp)
200610b2:	c64e                	c.swsp	s3,12(sp)
200610b4:	c452                	c.swsp	s4,8(sp)
200610b6:	ce06                	c.swsp	ra,28(sp)
200610b8:	89aa                	c.mv	s3,a0
200610ba:	dd6fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
200610be:	ff41a403          	lw	s0,-12(gp) # 200669a4 <timer_table>
200610c2:	4901                	c.li	s2,0
200610c4:	ff418a13          	addi	s4,gp,-12 # 200669a4 <timer_table>
200610c8:	85a2                	c.mv	a1,s0
200610ca:	ff418513          	addi	a0,gp,-12 # 200669a4 <timer_table>
200610ce:	fd5ff0ef          	jal	ra,200610a2 <rtw_end_of_queue_search>
200610d2:	c90d                	c.beqz	a0,20061104 <timer_wrapper+0x5a>
200610d4:	de0fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
200610d8:	03441c63          	bne	s0,s4,20061110 <timer_wrapper+0x66>
200610dc:	4462                	c.lwsp	s0,24(sp)
200610de:	40f2                	c.lwsp	ra,28(sp)
200610e0:	44d2                	c.lwsp	s1,20(sp)
200610e2:	4942                	c.lwsp	s2,16(sp)
200610e4:	49b2                	c.lwsp	s3,12(sp)
200610e6:	4a22                	c.lwsp	s4,8(sp)
200610e8:	200656b7          	lui	a3,0x20065
200610ec:	200655b7          	lui	a1,0x20065
200610f0:	9e068693          	addi	a3,a3,-1568 # 200649e0 <__func__.1+0x54>
200610f4:	04500613          	addi	a2,zero,69
200610f8:	a1058593          	addi	a1,a1,-1520 # 20064a10 <__func__.1+0x84>
200610fc:	4509                	c.li	a0,2
200610fe:	6105                	c.addi16sp	sp,32
20061100:	876fa06f          	jal	zero,2005b176 <rtk_log_write>
20061104:	441c                	c.lw	a5,8(s0)
20061106:	8922                	c.mv	s2,s0
20061108:	fd3786e3          	beq	a5,s3,200610d4 <timer_wrapper+0x2a>
2006110c:	4000                	c.lw	s0,0(s0)
2006110e:	bf6d                	c.j	200610c8 <timer_wrapper+0x1e>
20061110:	01092783          	lw	a5,16(s2)
20061114:	cb99                	c.beqz	a5,2006112a <timer_wrapper+0x80>
20061116:	4462                	c.lwsp	s0,24(sp)
20061118:	00c92503          	lw	a0,12(s2)
2006111c:	40f2                	c.lwsp	ra,28(sp)
2006111e:	44d2                	c.lwsp	s1,20(sp)
20061120:	4942                	c.lwsp	s2,16(sp)
20061122:	49b2                	c.lwsp	s3,12(sp)
20061124:	4a22                	c.lwsp	s4,8(sp)
20061126:	6105                	c.addi16sp	sp,32
20061128:	8782                	c.jr	a5
2006112a:	40f2                	c.lwsp	ra,28(sp)
2006112c:	4462                	c.lwsp	s0,24(sp)
2006112e:	44d2                	c.lwsp	s1,20(sp)
20061130:	4942                	c.lwsp	s2,16(sp)
20061132:	49b2                	c.lwsp	s3,12(sp)
20061134:	4a22                	c.lwsp	s4,8(sp)
20061136:	6105                	c.addi16sp	sp,32
20061138:	8082                	c.jr	ra

2006113a <init_timer_wrapper>:
2006113a:	1141                	c.addi	sp,-16
2006113c:	ff418513          	addi	a0,gp,-12 # 200669a4 <timer_table>
20061140:	c606                	c.swsp	ra,12(sp)
20061142:	f1bff0ef          	jal	ra,2006105c <rtw_init_listhead>
20061146:	40b2                	c.lwsp	ra,12(sp)
20061148:	fe01ae23          	sw	zero,-4(gp) # 200669ac <timer_used_num>
2006114c:	fe01a823          	sw	zero,-16(gp) # 200669a0 <max_timer_used_num>
20061150:	0141                	c.addi	sp,16
20061152:	8082                	c.jr	ra

20061154 <init_timer>:
20061154:	491c                	c.lw	a5,16(a0)
20061156:	cbc1                	c.beqz	a5,200611e6 <init_timer+0x92>
20061158:	1141                	c.addi	sp,-16
2006115a:	c422                	c.swsp	s0,8(sp)
2006115c:	c606                	c.swsp	ra,12(sp)
2006115e:	842a                	c.mv	s0,a0
20061160:	4508                	c.lw	a0,8(a0)
20061162:	e535                	c.bnez	a0,200611ce <init_timer+0x7a>
20061164:	200617b7          	lui	a5,0x20061
20061168:	0aa78793          	addi	a5,a5,170 # 200610aa <timer_wrapper>
2006116c:	4701                	c.li	a4,0
2006116e:	56fd                	c.li	a3,-1
20061170:	4601                	c.li	a2,0
20061172:	00840513          	addi	a0,s0,8
20061176:	af9fa0ef          	jal	ra,2005bc6e <rtos_timer_create_static>
2006117a:	441c                	c.lw	a5,8(s0)
2006117c:	e38d                	c.bnez	a5,2006119e <init_timer+0x4a>
2006117e:	4422                	c.lwsp	s0,8(sp)
20061180:	40b2                	c.lwsp	ra,12(sp)
20061182:	200656b7          	lui	a3,0x20065
20061186:	200655b7          	lui	a1,0x20065
2006118a:	a1868693          	addi	a3,a3,-1512 # 20064a18 <__func__.1+0x8c>
2006118e:	04500613          	addi	a2,zero,69
20061192:	a1058593          	addi	a1,a1,-1520 # 20064a10 <__func__.1+0x84>
20061196:	4509                	c.li	a0,2
20061198:	0141                	c.addi	sp,16
2006119a:	fddf906f          	jal	zero,2005b176 <rtk_log_write>
2006119e:	cf2fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
200611a2:	ff418593          	addi	a1,gp,-12 # 200669a4 <timer_table>
200611a6:	8522                	c.mv	a0,s0
200611a8:	ebbff0ef          	jal	ra,20061062 <rtw_list_insert_head>
200611ac:	d08fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
200611b0:	ffc18713          	addi	a4,gp,-4 # 200669ac <timer_used_num>
200611b4:	431c                	c.lw	a5,0(a4)
200611b6:	0785                	c.addi	a5,1
200611b8:	c31c                	c.sw	a5,0(a4)
200611ba:	ff018713          	addi	a4,gp,-16 # 200669a0 <max_timer_used_num>
200611be:	4314                	c.lw	a3,0(a4)
200611c0:	00f6d363          	bge	a3,a5,200611c6 <init_timer+0x72>
200611c4:	c31c                	c.sw	a5,0(a4)
200611c6:	40b2                	c.lwsp	ra,12(sp)
200611c8:	4422                	c.lwsp	s0,8(sp)
200611ca:	0141                	c.addi	sp,16
200611cc:	8082                	c.jr	ra
200611ce:	c71fa0ef          	jal	ra,2005be3e <rtos_timer_is_timer_active>
200611d2:	4785                	c.li	a5,1
200611d4:	fef519e3          	bne	a0,a5,200611c6 <init_timer+0x72>
200611d8:	4408                	c.lw	a0,8(s0)
200611da:	4422                	c.lwsp	s0,8(sp)
200611dc:	40b2                	c.lwsp	ra,12(sp)
200611de:	55fd                	c.li	a1,-1
200611e0:	0141                	c.addi	sp,16
200611e2:	b9ffa06f          	jal	zero,2005bd80 <rtos_timer_stop>
200611e6:	8082                	c.jr	ra

200611e8 <mod_timer>:
200611e8:	1141                	c.addi	sp,-16
200611ea:	c422                	c.swsp	s0,8(sp)
200611ec:	c226                	c.swsp	s1,4(sp)
200611ee:	c606                	c.swsp	ra,12(sp)
200611f0:	842a                	c.mv	s0,a0
200611f2:	4508                	c.lw	a0,8(a0)
200611f4:	84ae                	c.mv	s1,a1
200611f6:	e915                	c.bnez	a0,2006122a <mod_timer+0x42>
200611f8:	20065537          	lui	a0,0x20065
200611fc:	a3050513          	addi	a0,a0,-1488 # 20064a30 <__func__.1+0xa4>
20061200:	a02fe0ef          	jal	ra,2005f402 <DiagPrintf_minimal>
20061204:	4408                	c.lw	a0,8(s0)
20061206:	cd05                	c.beqz	a0,2006123e <mod_timer+0x56>
20061208:	567d                	c.li	a2,-1
2006120a:	85a6                	c.mv	a1,s1
2006120c:	bd1fa0ef          	jal	ra,2005bddc <rtos_timer_change_period>
20061210:	57fd                	c.li	a5,-1
20061212:	02f51663          	bne	a0,a5,2006123e <mod_timer+0x56>
20061216:	4422                	c.lwsp	s0,8(sp)
20061218:	40b2                	c.lwsp	ra,12(sp)
2006121a:	4492                	c.lwsp	s1,4(sp)
2006121c:	20065537          	lui	a0,0x20065
20061220:	a4850513          	addi	a0,a0,-1464 # 20064a48 <__func__.1+0xbc>
20061224:	0141                	c.addi	sp,16
20061226:	9dcfe06f          	jal	zero,2005f402 <DiagPrintf_minimal>
2006122a:	c15fa0ef          	jal	ra,2005be3e <rtos_timer_is_timer_active>
2006122e:	4785                	c.li	a5,1
20061230:	fcf51ae3          	bne	a0,a5,20061204 <mod_timer+0x1c>
20061234:	4408                	c.lw	a0,8(s0)
20061236:	55fd                	c.li	a1,-1
20061238:	b49fa0ef          	jal	ra,2005bd80 <rtos_timer_stop>
2006123c:	b7e1                	c.j	20061204 <mod_timer+0x1c>
2006123e:	40b2                	c.lwsp	ra,12(sp)
20061240:	4422                	c.lwsp	s0,8(sp)
20061242:	4492                	c.lwsp	s1,4(sp)
20061244:	0141                	c.addi	sp,16
20061246:	8082                	c.jr	ra

20061248 <cancel_timer_ex>:
20061248:	451c                	c.lw	a5,8(a0)
2006124a:	c7ad                	c.beqz	a5,200612b4 <cancel_timer_ex+0x6c>
2006124c:	1101                	c.addi	sp,-32
2006124e:	c84a                	c.swsp	s2,16(sp)
20061250:	cc22                	c.swsp	s0,24(sp)
20061252:	ca26                	c.swsp	s1,20(sp)
20061254:	c64e                	c.swsp	s3,12(sp)
20061256:	ce06                	c.swsp	ra,28(sp)
20061258:	84aa                	c.mv	s1,a0
2006125a:	c36fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
2006125e:	ff41a403          	lw	s0,-12(gp) # 200669a4 <timer_table>
20061262:	ff418993          	addi	s3,gp,-12 # 200669a4 <timer_table>
20061266:	85a2                	c.mv	a1,s0
20061268:	ff418513          	addi	a0,gp,-12 # 200669a4 <timer_table>
2006126c:	e37ff0ef          	jal	ra,200610a2 <rtw_end_of_queue_search>
20061270:	e509                	c.bnez	a0,2006127a <cancel_timer_ex+0x32>
20061272:	4418                	c.lw	a4,8(s0)
20061274:	449c                	c.lw	a5,8(s1)
20061276:	02f71463          	bne	a4,a5,2006129e <cancel_timer_ex+0x56>
2006127a:	c3afa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
2006127e:	4488                	c.lw	a0,8(s1)
20061280:	03341163          	bne	s0,s3,200612a2 <cancel_timer_ex+0x5a>
20061284:	4462                	c.lwsp	s0,24(sp)
20061286:	40f2                	c.lwsp	ra,28(sp)
20061288:	44d2                	c.lwsp	s1,20(sp)
2006128a:	4942                	c.lwsp	s2,16(sp)
2006128c:	49b2                	c.lwsp	s3,12(sp)
2006128e:	85aa                	c.mv	a1,a0
20061290:	20065537          	lui	a0,0x20065
20061294:	a5850513          	addi	a0,a0,-1448 # 20064a58 <__func__.1+0xcc>
20061298:	6105                	c.addi16sp	sp,32
2006129a:	968fe06f          	jal	zero,2005f402 <DiagPrintf_minimal>
2006129e:	4000                	c.lw	s0,0(s0)
200612a0:	b7d9                	c.j	20061266 <cancel_timer_ex+0x1e>
200612a2:	4462                	c.lwsp	s0,24(sp)
200612a4:	40f2                	c.lwsp	ra,28(sp)
200612a6:	44d2                	c.lwsp	s1,20(sp)
200612a8:	4942                	c.lwsp	s2,16(sp)
200612aa:	49b2                	c.lwsp	s3,12(sp)
200612ac:	55fd                	c.li	a1,-1
200612ae:	6105                	c.addi16sp	sp,32
200612b0:	ad1fa06f          	jal	zero,2005bd80 <rtos_timer_stop>
200612b4:	8082                	c.jr	ra

200612b6 <del_timer_sync>:
200612b6:	451c                	c.lw	a5,8(a0)
200612b8:	cfbd                	c.beqz	a5,20061336 <del_timer_sync+0x80>
200612ba:	1101                	c.addi	sp,-32
200612bc:	c84a                	c.swsp	s2,16(sp)
200612be:	cc22                	c.swsp	s0,24(sp)
200612c0:	ca26                	c.swsp	s1,20(sp)
200612c2:	c64e                	c.swsp	s3,12(sp)
200612c4:	ce06                	c.swsp	ra,28(sp)
200612c6:	842a                	c.mv	s0,a0
200612c8:	bc8fa0ef          	jal	ra,2005b690 <rtos_critical_enter>
200612cc:	ff41a483          	lw	s1,-12(gp) # 200669a4 <timer_table>
200612d0:	ff418993          	addi	s3,gp,-12 # 200669a4 <timer_table>
200612d4:	85a6                	c.mv	a1,s1
200612d6:	ff418513          	addi	a0,gp,-12 # 200669a4 <timer_table>
200612da:	dc9ff0ef          	jal	ra,200610a2 <rtw_end_of_queue_search>
200612de:	e901                	c.bnez	a0,200612ee <del_timer_sync+0x38>
200612e0:	4498                	c.lw	a4,8(s1)
200612e2:	441c                	c.lw	a5,8(s0)
200612e4:	02f71563          	bne	a4,a5,2006130e <del_timer_sync+0x58>
200612e8:	8526                	c.mv	a0,s1
200612ea:	d91ff0ef          	jal	ra,2006107a <rtw_list_delete>
200612ee:	bc6fa0ef          	jal	ra,2005b6b4 <rtos_critical_exit>
200612f2:	03349063          	bne	s1,s3,20061312 <del_timer_sync+0x5c>
200612f6:	4462                	c.lwsp	s0,24(sp)
200612f8:	40f2                	c.lwsp	ra,28(sp)
200612fa:	44d2                	c.lwsp	s1,20(sp)
200612fc:	4942                	c.lwsp	s2,16(sp)
200612fe:	49b2                	c.lwsp	s3,12(sp)
20061300:	20065537          	lui	a0,0x20065
20061304:	a7050513          	addi	a0,a0,-1424 # 20064a70 <__func__.1+0xe4>
20061308:	6105                	c.addi16sp	sp,32
2006130a:	8f8fe06f          	jal	zero,2005f402 <DiagPrintf_minimal>
2006130e:	4084                	c.lw	s1,0(s1)
20061310:	b7d1                	c.j	200612d4 <del_timer_sync+0x1e>
20061312:	4408                	c.lw	a0,8(s0)
20061314:	55fd                	c.li	a1,-1
20061316:	9dbfa0ef          	jal	ra,2005bcf0 <rtos_timer_delete_static>
2006131a:	ffc18793          	addi	a5,gp,-4 # 200669ac <timer_used_num>
2006131e:	4398                	c.lw	a4,0(a5)
20061320:	00042423          	sw	zero,8(s0)
20061324:	177d                	c.addi	a4,-1
20061326:	c398                	c.sw	a4,0(a5)
20061328:	40f2                	c.lwsp	ra,28(sp)
2006132a:	4462                	c.lwsp	s0,24(sp)
2006132c:	44d2                	c.lwsp	s1,20(sp)
2006132e:	4942                	c.lwsp	s2,16(sp)
20061330:	49b2                	c.lwsp	s3,12(sp)
20061332:	6105                	c.addi16sp	sp,32
20061334:	8082                	c.jr	ra
20061336:	8082                	c.jr	ra

20061338 <rtw_init_timer>:
20061338:	c910                	c.sw	a2,16(a0)
2006133a:	c554                	c.sw	a3,12(a0)
2006133c:	85ba                	c.mv	a1,a4
2006133e:	e17ff06f          	jal	zero,20061154 <init_timer>

20061342 <rtw_set_timer>:
20061342:	ea7ff06f          	jal	zero,200611e8 <mod_timer>

20061346 <rtw_cancel_timer>:
20061346:	1141                	c.addi	sp,-16
20061348:	c606                	c.swsp	ra,12(sp)
2006134a:	effff0ef          	jal	ra,20061248 <cancel_timer_ex>
2006134e:	40b2                	c.lwsp	ra,12(sp)
20061350:	4501                	c.li	a0,0
20061352:	0141                	c.addi	sp,16
20061354:	8082                	c.jr	ra

20061356 <rtw_del_timer>:
20061356:	f61ff06f          	jal	zero,200612b6 <del_timer_sync>

2006135a <rtw_acquire_wakelock>:
2006135a:	1141                	c.addi	sp,-16
2006135c:	c606                	c.swsp	ra,12(sp)
2006135e:	c52f80ef          	jal	ra,200597b0 <pmu_yield_os_check>
20061362:	c511                	c.beqz	a0,2006136e <rtw_acquire_wakelock+0x14>
20061364:	40b2                	c.lwsp	ra,12(sp)
20061366:	4505                	c.li	a0,1
20061368:	0141                	c.addi	sp,16
2006136a:	c92f806f          	jal	zero,200597fc <pmu_acquire_wakelock>
2006136e:	40b2                	c.lwsp	ra,12(sp)
20061370:	0141                	c.addi	sp,16
20061372:	8082                	c.jr	ra

20061374 <rtw_release_wakelock>:
20061374:	1141                	c.addi	sp,-16
20061376:	c606                	c.swsp	ra,12(sp)
20061378:	c38f80ef          	jal	ra,200597b0 <pmu_yield_os_check>
2006137c:	c511                	c.beqz	a0,20061388 <rtw_release_wakelock+0x14>
2006137e:	40b2                	c.lwsp	ra,12(sp)
20061380:	4505                	c.li	a0,1
20061382:	0141                	c.addi	sp,16
20061384:	c9ef806f          	jal	zero,20059822 <pmu_release_wakelock>
20061388:	40b2                	c.lwsp	ra,12(sp)
2006138a:	0141                	c.addi	sp,16
2006138c:	8082                	c.jr	ra

2006138e <rtw_wakelock_timeout>:
2006138e:	1141                	c.addi	sp,-16
20061390:	c422                	c.swsp	s0,8(sp)
20061392:	c606                	c.swsp	ra,12(sp)
20061394:	842a                	c.mv	s0,a0
20061396:	c1af80ef          	jal	ra,200597b0 <pmu_yield_os_check>
2006139a:	c519                	c.beqz	a0,200613a8 <rtw_wakelock_timeout+0x1a>
2006139c:	8522                	c.mv	a0,s0
2006139e:	4422                	c.lwsp	s0,8(sp)
200613a0:	40b2                	c.lwsp	ra,12(sp)
200613a2:	0141                	c.addi	sp,16
200613a4:	b60f806f          	jal	zero,20059704 <pmu_set_sysactive_time>
200613a8:	40b2                	c.lwsp	ra,12(sp)
200613aa:	4422                	c.lwsp	s0,8(sp)
200613ac:	0141                	c.addi	sp,16
200613ae:	8082                	c.jr	ra

200613b0 <deregister_tm_clones>:
200613b0:	20058537          	lui	a0,0x20058
200613b4:	200587b7          	lui	a5,0x20058
200613b8:	06050713          	addi	a4,a0,96 # 20058060 <BOOT_InitGP>
200613bc:	06078793          	addi	a5,a5,96 # 20058060 <BOOT_InitGP>
200613c0:	00e78863          	beq	a5,a4,200613d0 <deregister_tm_clones+0x20>
200613c4:	00000793          	addi	a5,zero,0
200613c8:	c781                	c.beqz	a5,200613d0 <deregister_tm_clones+0x20>
200613ca:	06050513          	addi	a0,a0,96
200613ce:	8782                	c.jr	a5
200613d0:	8082                	c.jr	ra

200613d2 <register_tm_clones>:
200613d2:	200587b7          	lui	a5,0x20058
200613d6:	20058537          	lui	a0,0x20058
200613da:	06078593          	addi	a1,a5,96 # 20058060 <BOOT_InitGP>
200613de:	06050793          	addi	a5,a0,96 # 20058060 <BOOT_InitGP>
200613e2:	8d9d                	c.sub	a1,a5
200613e4:	8589                	c.srai	a1,0x2
200613e6:	4789                	c.li	a5,2
200613e8:	02f5c5b3          	div	a1,a1,a5
200613ec:	c599                	c.beqz	a1,200613fa <register_tm_clones+0x28>
200613ee:	00000793          	addi	a5,zero,0
200613f2:	c781                	c.beqz	a5,200613fa <register_tm_clones+0x28>
200613f4:	06050513          	addi	a0,a0,96
200613f8:	8782                	c.jr	a5
200613fa:	8082                	c.jr	ra

200613fc <__do_global_dtors_aux>:
200613fc:	200697b7          	lui	a5,0x20069
20061400:	8607c703          	lbu	a4,-1952(a5) # 20068860 <completed.1>
20061404:	eb05                	c.bnez	a4,20061434 <__do_global_dtors_aux+0x38>
20061406:	1141                	c.addi	sp,-16
20061408:	c422                	c.swsp	s0,8(sp)
2006140a:	c606                	c.swsp	ra,12(sp)
2006140c:	843e                	c.mv	s0,a5
2006140e:	374d                	c.jal	200613b0 <deregister_tm_clones>
20061410:	00000793          	addi	a5,zero,0
20061414:	cb89                	c.beqz	a5,20061426 <__do_global_dtors_aux+0x2a>
20061416:	20058537          	lui	a0,0x20058
2006141a:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
2006141e:	00000097          	auipc	ra,0x0
20061422:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20061426:	4785                	c.li	a5,1
20061428:	40b2                	c.lwsp	ra,12(sp)
2006142a:	86f40023          	sb	a5,-1952(s0)
2006142e:	4422                	c.lwsp	s0,8(sp)
20061430:	0141                	c.addi	sp,16
20061432:	8082                	c.jr	ra
20061434:	8082                	c.jr	ra

20061436 <frame_dummy>:
20061436:	00000793          	addi	a5,zero,0
2006143a:	c38d                	c.beqz	a5,2006145c <frame_dummy+0x26>
2006143c:	200695b7          	lui	a1,0x20069
20061440:	20058537          	lui	a0,0x20058
20061444:	1141                	c.addi	sp,-16
20061446:	86458593          	addi	a1,a1,-1948 # 20068864 <object.0>
2006144a:	06050513          	addi	a0,a0,96 # 20058060 <BOOT_InitGP>
2006144e:	c606                	c.swsp	ra,12(sp)
20061450:	00000097          	auipc	ra,0x0
20061454:	000000e7          	jalr	ra,0(zero) # 0 <Reset_Handler>
20061458:	40b2                	c.lwsp	ra,12(sp)
2006145a:	0141                	c.addi	sp,16
2006145c:	bf9d                	c.j	200613d2 <register_tm_clones>

2006145e <__clzsi2>:
2006145e:	67c1                	c.lui	a5,0x10
20061460:	02f57663          	bgeu	a0,a5,2006148c <__clzsi2+0x2e>
20061464:	0ff00793          	addi	a5,zero,255
20061468:	00a7b7b3          	sltu	a5,a5,a0
2006146c:	078e                	c.slli	a5,0x3
2006146e:	20065737          	lui	a4,0x20065
20061472:	02000693          	addi	a3,zero,32
20061476:	8e9d                	c.sub	a3,a5
20061478:	00f55533          	srl	a0,a0,a5
2006147c:	a8070793          	addi	a5,a4,-1408 # 20064a80 <__clz_tab>
20061480:	953e                	c.add	a0,a5
20061482:	00054503          	lbu	a0,0(a0)
20061486:	40a68533          	sub	a0,a3,a0
2006148a:	8082                	c.jr	ra
2006148c:	01000737          	lui	a4,0x1000
20061490:	47c1                	c.li	a5,16
20061492:	fce56ee3          	bltu	a0,a4,2006146e <__clzsi2+0x10>
20061496:	47e1                	c.li	a5,24
20061498:	bfd9                	c.j	2006146e <__clzsi2+0x10>

2006149a <__udivdi3>:
2006149a:	88aa                	c.mv	a7,a0
2006149c:	87ae                	c.mv	a5,a1
2006149e:	8832                	c.mv	a6,a2
200614a0:	8536                	c.mv	a0,a3
200614a2:	8346                	c.mv	t1,a7
200614a4:	20069663          	bne	a3,zero,200616b0 <__udivdi3+0x216>
200614a8:	200656b7          	lui	a3,0x20065
200614ac:	a8068693          	addi	a3,a3,-1408 # 20064a80 <__clz_tab>
200614b0:	0ac5ff63          	bgeu	a1,a2,2006156e <__udivdi3+0xd4>
200614b4:	6741                	c.lui	a4,0x10
200614b6:	0ae67563          	bgeu	a2,a4,20061560 <__udivdi3+0xc6>
200614ba:	0ff00713          	addi	a4,zero,255
200614be:	00c73733          	sltu	a4,a4,a2
200614c2:	070e                	c.slli	a4,0x3
200614c4:	00e65533          	srl	a0,a2,a4
200614c8:	96aa                	c.add	a3,a0
200614ca:	0006c683          	lbu	a3,0(a3)
200614ce:	02000513          	addi	a0,zero,32
200614d2:	9736                	c.add	a4,a3
200614d4:	40e506b3          	sub	a3,a0,a4
200614d8:	00e50b63          	beq	a0,a4,200614ee <__udivdi3+0x54>
200614dc:	00d795b3          	sll	a1,a5,a3
200614e0:	00e8d733          	srl	a4,a7,a4
200614e4:	00d61833          	sll	a6,a2,a3
200614e8:	8dd9                	c.or	a1,a4
200614ea:	00d89333          	sll	t1,a7,a3
200614ee:	01085893          	srli	a7,a6,0x10
200614f2:	0315d6b3          	divu	a3,a1,a7
200614f6:	01081613          	slli	a2,a6,0x10
200614fa:	8241                	c.srli	a2,0x10
200614fc:	01035793          	srli	a5,t1,0x10
20061500:	0315f733          	remu	a4,a1,a7
20061504:	8536                	c.mv	a0,a3
20061506:	02d605b3          	mul	a1,a2,a3
2006150a:	0742                	c.slli	a4,0x10
2006150c:	8fd9                	c.or	a5,a4
2006150e:	00b7fc63          	bgeu	a5,a1,20061526 <__udivdi3+0x8c>
20061512:	97c2                	c.add	a5,a6
20061514:	fff68513          	addi	a0,a3,-1
20061518:	0107e763          	bltu	a5,a6,20061526 <__udivdi3+0x8c>
2006151c:	00b7f563          	bgeu	a5,a1,20061526 <__udivdi3+0x8c>
20061520:	ffe68513          	addi	a0,a3,-2
20061524:	97c2                	c.add	a5,a6
20061526:	8f8d                	c.sub	a5,a1
20061528:	0317f733          	remu	a4,a5,a7
2006152c:	0342                	c.slli	t1,0x10
2006152e:	01035313          	srli	t1,t1,0x10
20061532:	0317d7b3          	divu	a5,a5,a7
20061536:	0742                	c.slli	a4,0x10
20061538:	00676333          	or	t1,a4,t1
2006153c:	02f606b3          	mul	a3,a2,a5
20061540:	863e                	c.mv	a2,a5
20061542:	00d37b63          	bgeu	t1,a3,20061558 <__udivdi3+0xbe>
20061546:	9342                	c.add	t1,a6
20061548:	fff78613          	addi	a2,a5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
2006154c:	01036663          	bltu	t1,a6,20061558 <__udivdi3+0xbe>
20061550:	00d37463          	bgeu	t1,a3,20061558 <__udivdi3+0xbe>
20061554:	ffe78613          	addi	a2,a5,-2
20061558:	0542                	c.slli	a0,0x10
2006155a:	8d51                	c.or	a0,a2
2006155c:	4581                	c.li	a1,0
2006155e:	a85d                	c.j	20061614 <__udivdi3+0x17a>
20061560:	01000537          	lui	a0,0x1000
20061564:	4741                	c.li	a4,16
20061566:	f4a66fe3          	bltu	a2,a0,200614c4 <__udivdi3+0x2a>
2006156a:	4761                	c.li	a4,24
2006156c:	bfa1                	c.j	200614c4 <__udivdi3+0x2a>
2006156e:	e601                	c.bnez	a2,20061576 <__udivdi3+0xdc>
20061570:	4705                	c.li	a4,1
20061572:	02c75833          	divu	a6,a4,a2
20061576:	6741                	c.lui	a4,0x10
20061578:	08e87f63          	bgeu	a6,a4,20061616 <__udivdi3+0x17c>
2006157c:	0ff00713          	addi	a4,zero,255
20061580:	01077363          	bgeu	a4,a6,20061586 <__udivdi3+0xec>
20061584:	4521                	c.li	a0,8
20061586:	00a85733          	srl	a4,a6,a0
2006158a:	96ba                	c.add	a3,a4
2006158c:	0006c703          	lbu	a4,0(a3)
20061590:	02000613          	addi	a2,zero,32
20061594:	972a                	c.add	a4,a0
20061596:	40e606b3          	sub	a3,a2,a4
2006159a:	08e61563          	bne	a2,a4,20061624 <__udivdi3+0x18a>
2006159e:	410787b3          	sub	a5,a5,a6
200615a2:	4585                	c.li	a1,1
200615a4:	01085893          	srli	a7,a6,0x10
200615a8:	01081613          	slli	a2,a6,0x10
200615ac:	8241                	c.srli	a2,0x10
200615ae:	01035713          	srli	a4,t1,0x10
200615b2:	0317f6b3          	remu	a3,a5,a7
200615b6:	0317d7b3          	divu	a5,a5,a7
200615ba:	06c2                	c.slli	a3,0x10
200615bc:	8f55                	c.or	a4,a3
200615be:	02f60e33          	mul	t3,a2,a5
200615c2:	853e                	c.mv	a0,a5
200615c4:	01c77c63          	bgeu	a4,t3,200615dc <__udivdi3+0x142>
200615c8:	9742                	c.add	a4,a6
200615ca:	fff78513          	addi	a0,a5,-1
200615ce:	01076763          	bltu	a4,a6,200615dc <__udivdi3+0x142>
200615d2:	01c77563          	bgeu	a4,t3,200615dc <__udivdi3+0x142>
200615d6:	ffe78513          	addi	a0,a5,-2
200615da:	9742                	c.add	a4,a6
200615dc:	41c70733          	sub	a4,a4,t3
200615e0:	031777b3          	remu	a5,a4,a7
200615e4:	0342                	c.slli	t1,0x10
200615e6:	01035313          	srli	t1,t1,0x10
200615ea:	03175733          	divu	a4,a4,a7
200615ee:	07c2                	c.slli	a5,0x10
200615f0:	0067e333          	or	t1,a5,t1
200615f4:	02e606b3          	mul	a3,a2,a4
200615f8:	863a                	c.mv	a2,a4
200615fa:	00d37b63          	bgeu	t1,a3,20061610 <__udivdi3+0x176>
200615fe:	9342                	c.add	t1,a6
20061600:	fff70613          	addi	a2,a4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061604:	01036663          	bltu	t1,a6,20061610 <__udivdi3+0x176>
20061608:	00d37463          	bgeu	t1,a3,20061610 <__udivdi3+0x176>
2006160c:	ffe70613          	addi	a2,a4,-2
20061610:	0542                	c.slli	a0,0x10
20061612:	8d51                	c.or	a0,a2
20061614:	8082                	c.jr	ra
20061616:	01000737          	lui	a4,0x1000
2006161a:	4541                	c.li	a0,16
2006161c:	f6e865e3          	bltu	a6,a4,20061586 <__udivdi3+0xec>
20061620:	4561                	c.li	a0,24
20061622:	b795                	c.j	20061586 <__udivdi3+0xec>
20061624:	00d81833          	sll	a6,a6,a3
20061628:	00e7d533          	srl	a0,a5,a4
2006162c:	00d89333          	sll	t1,a7,a3
20061630:	00d797b3          	sll	a5,a5,a3
20061634:	00e8d733          	srl	a4,a7,a4
20061638:	01085893          	srli	a7,a6,0x10
2006163c:	00f76633          	or	a2,a4,a5
20061640:	03157733          	remu	a4,a0,a7
20061644:	01081793          	slli	a5,a6,0x10
20061648:	83c1                	c.srli	a5,0x10
2006164a:	01065593          	srli	a1,a2,0x10
2006164e:	03155533          	divu	a0,a0,a7
20061652:	0742                	c.slli	a4,0x10
20061654:	8f4d                	c.or	a4,a1
20061656:	02a786b3          	mul	a3,a5,a0
2006165a:	85aa                	c.mv	a1,a0
2006165c:	00d77c63          	bgeu	a4,a3,20061674 <__udivdi3+0x1da>
20061660:	9742                	c.add	a4,a6
20061662:	fff50593          	addi	a1,a0,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20061666:	01076763          	bltu	a4,a6,20061674 <__udivdi3+0x1da>
2006166a:	00d77563          	bgeu	a4,a3,20061674 <__udivdi3+0x1da>
2006166e:	ffe50593          	addi	a1,a0,-2
20061672:	9742                	c.add	a4,a6
20061674:	40d706b3          	sub	a3,a4,a3
20061678:	0316f733          	remu	a4,a3,a7
2006167c:	0642                	c.slli	a2,0x10
2006167e:	8241                	c.srli	a2,0x10
20061680:	0316d6b3          	divu	a3,a3,a7
20061684:	0742                	c.slli	a4,0x10
20061686:	02d78533          	mul	a0,a5,a3
2006168a:	00c767b3          	or	a5,a4,a2
2006168e:	8736                	c.mv	a4,a3
20061690:	00a7fc63          	bgeu	a5,a0,200616a8 <__udivdi3+0x20e>
20061694:	97c2                	c.add	a5,a6
20061696:	fff68713          	addi	a4,a3,-1
2006169a:	0107e763          	bltu	a5,a6,200616a8 <__udivdi3+0x20e>
2006169e:	00a7f563          	bgeu	a5,a0,200616a8 <__udivdi3+0x20e>
200616a2:	ffe68713          	addi	a4,a3,-2
200616a6:	97c2                	c.add	a5,a6
200616a8:	05c2                	c.slli	a1,0x10
200616aa:	8f89                	c.sub	a5,a0
200616ac:	8dd9                	c.or	a1,a4
200616ae:	bddd                	c.j	200615a4 <__udivdi3+0x10a>
200616b0:	14d5e263          	bltu	a1,a3,200617f4 <__udivdi3+0x35a>
200616b4:	6741                	c.lui	a4,0x10
200616b6:	02e6ff63          	bgeu	a3,a4,200616f4 <__udivdi3+0x25a>
200616ba:	0ff00713          	addi	a4,zero,255
200616be:	00d735b3          	sltu	a1,a4,a3
200616c2:	058e                	c.slli	a1,0x3
200616c4:	20065737          	lui	a4,0x20065
200616c8:	00b6d533          	srl	a0,a3,a1
200616cc:	a8070713          	addi	a4,a4,-1408 # 20064a80 <__clz_tab>
200616d0:	972a                	c.add	a4,a0
200616d2:	00074703          	lbu	a4,0(a4)
200616d6:	02000513          	addi	a0,zero,32
200616da:	972e                	c.add	a4,a1
200616dc:	40e505b3          	sub	a1,a0,a4
200616e0:	02e51163          	bne	a0,a4,20061702 <__udivdi3+0x268>
200616e4:	4505                	c.li	a0,1
200616e6:	f2f6e7e3          	bltu	a3,a5,20061614 <__udivdi3+0x17a>
200616ea:	00c8b533          	sltu	a0,a7,a2
200616ee:	00154513          	xori	a0,a0,1
200616f2:	b70d                	c.j	20061614 <__udivdi3+0x17a>
200616f4:	01000737          	lui	a4,0x1000
200616f8:	45c1                	c.li	a1,16
200616fa:	fce6e5e3          	bltu	a3,a4,200616c4 <__udivdi3+0x22a>
200616fe:	45e1                	c.li	a1,24
20061700:	b7d1                	c.j	200616c4 <__udivdi3+0x22a>
20061702:	00e65833          	srl	a6,a2,a4
20061706:	00b696b3          	sll	a3,a3,a1
2006170a:	00d86833          	or	a6,a6,a3
2006170e:	00e7de33          	srl	t3,a5,a4
20061712:	01085e93          	srli	t4,a6,0x10
20061716:	03de76b3          	remu	a3,t3,t4
2006171a:	00b797b3          	sll	a5,a5,a1
2006171e:	00e8d733          	srl	a4,a7,a4
20061722:	00b61333          	sll	t1,a2,a1
20061726:	00f76633          	or	a2,a4,a5
2006172a:	01081793          	slli	a5,a6,0x10
2006172e:	83c1                	c.srli	a5,0x10
20061730:	01065713          	srli	a4,a2,0x10
20061734:	03de5e33          	divu	t3,t3,t4
20061738:	06c2                	c.slli	a3,0x10
2006173a:	8f55                	c.or	a4,a3
2006173c:	03c78f33          	mul	t5,a5,t3
20061740:	8572                	c.mv	a0,t3
20061742:	01e77c63          	bgeu	a4,t5,2006175a <__udivdi3+0x2c0>
20061746:	9742                	c.add	a4,a6
20061748:	fffe0513          	addi	a0,t3,-1
2006174c:	01076763          	bltu	a4,a6,2006175a <__udivdi3+0x2c0>
20061750:	01e77563          	bgeu	a4,t5,2006175a <__udivdi3+0x2c0>
20061754:	ffee0513          	addi	a0,t3,-2
20061758:	9742                	c.add	a4,a6
2006175a:	41e70733          	sub	a4,a4,t5
2006175e:	03d776b3          	remu	a3,a4,t4
20061762:	03d75733          	divu	a4,a4,t4
20061766:	06c2                	c.slli	a3,0x10
20061768:	02e78e33          	mul	t3,a5,a4
2006176c:	01061793          	slli	a5,a2,0x10
20061770:	83c1                	c.srli	a5,0x10
20061772:	8fd5                	c.or	a5,a3
20061774:	863a                	c.mv	a2,a4
20061776:	01c7fc63          	bgeu	a5,t3,2006178e <__udivdi3+0x2f4>
2006177a:	97c2                	c.add	a5,a6
2006177c:	fff70613          	addi	a2,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20061780:	0107e763          	bltu	a5,a6,2006178e <__udivdi3+0x2f4>
20061784:	01c7f563          	bgeu	a5,t3,2006178e <__udivdi3+0x2f4>
20061788:	ffe70613          	addi	a2,a4,-2
2006178c:	97c2                	c.add	a5,a6
2006178e:	0542                	c.slli	a0,0x10
20061790:	6ec1                	c.lui	t4,0x10
20061792:	8d51                	c.or	a0,a2
20061794:	fffe8693          	addi	a3,t4,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061798:	00d57833          	and	a6,a0,a3
2006179c:	01055613          	srli	a2,a0,0x10
200617a0:	00d376b3          	and	a3,t1,a3
200617a4:	01035313          	srli	t1,t1,0x10
200617a8:	41c787b3          	sub	a5,a5,t3
200617ac:	02d80e33          	mul	t3,a6,a3
200617b0:	02d606b3          	mul	a3,a2,a3
200617b4:	010e5713          	srli	a4,t3,0x10
200617b8:	02680833          	mul	a6,a6,t1
200617bc:	9836                	c.add	a6,a3
200617be:	9742                	c.add	a4,a6
200617c0:	02660633          	mul	a2,a2,t1
200617c4:	00d77363          	bgeu	a4,a3,200617ca <__udivdi3+0x330>
200617c8:	9676                	c.add	a2,t4
200617ca:	01075693          	srli	a3,a4,0x10
200617ce:	9636                	c.add	a2,a3
200617d0:	02c7e063          	bltu	a5,a2,200617f0 <__udivdi3+0x356>
200617d4:	d8c794e3          	bne	a5,a2,2006155c <__udivdi3+0xc2>
200617d8:	67c1                	c.lui	a5,0x10
200617da:	17fd                	c.addi	a5,-1
200617dc:	8f7d                	c.and	a4,a5
200617de:	0742                	c.slli	a4,0x10
200617e0:	00fe7e33          	and	t3,t3,a5
200617e4:	00b898b3          	sll	a7,a7,a1
200617e8:	9772                	c.add	a4,t3
200617ea:	4581                	c.li	a1,0
200617ec:	e2e8f4e3          	bgeu	a7,a4,20061614 <__udivdi3+0x17a>
200617f0:	157d                	c.addi	a0,-1
200617f2:	b3ad                	c.j	2006155c <__udivdi3+0xc2>
200617f4:	4581                	c.li	a1,0
200617f6:	4501                	c.li	a0,0
200617f8:	bd31                	c.j	20061614 <__udivdi3+0x17a>

200617fa <__umoddi3>:
200617fa:	8eaa                	c.mv	t4,a0
200617fc:	832e                	c.mv	t1,a1
200617fe:	8832                	c.mv	a6,a2
20061800:	87b6                	c.mv	a5,a3
20061802:	1c069263          	bne	a3,zero,200619c6 <__umoddi3+0x1cc>
20061806:	20065737          	lui	a4,0x20065
2006180a:	a8070713          	addi	a4,a4,-1408 # 20064a80 <__clz_tab>
2006180e:	0ac5f563          	bgeu	a1,a2,200618b8 <__umoddi3+0xbe>
20061812:	66c1                	c.lui	a3,0x10
20061814:	08d67b63          	bgeu	a2,a3,200618aa <__umoddi3+0xb0>
20061818:	0ff00693          	addi	a3,zero,255
2006181c:	00c6f363          	bgeu	a3,a2,20061822 <__umoddi3+0x28>
20061820:	47a1                	c.li	a5,8
20061822:	00f656b3          	srl	a3,a2,a5
20061826:	9736                	c.add	a4,a3
20061828:	00074703          	lbu	a4,0(a4)
2006182c:	97ba                	c.add	a5,a4
2006182e:	02000713          	addi	a4,zero,32
20061832:	40f708b3          	sub	a7,a4,a5
20061836:	00f70b63          	beq	a4,a5,2006184c <__umoddi3+0x52>
2006183a:	011315b3          	sll	a1,t1,a7
2006183e:	00fed7b3          	srl	a5,t4,a5
20061842:	01161833          	sll	a6,a2,a7
20061846:	8ddd                	c.or	a1,a5
20061848:	011e9533          	sll	a0,t4,a7
2006184c:	01085693          	srli	a3,a6,0x10
20061850:	02d5f7b3          	remu	a5,a1,a3
20061854:	01081613          	slli	a2,a6,0x10
20061858:	8241                	c.srli	a2,0x10
2006185a:	01055713          	srli	a4,a0,0x10
2006185e:	02d5d5b3          	divu	a1,a1,a3
20061862:	07c2                	c.slli	a5,0x10
20061864:	8f5d                	c.or	a4,a5
20061866:	02b605b3          	mul	a1,a2,a1
2006186a:	00b77863          	bgeu	a4,a1,2006187a <__umoddi3+0x80>
2006186e:	9742                	c.add	a4,a6
20061870:	01076563          	bltu	a4,a6,2006187a <__umoddi3+0x80>
20061874:	00b77363          	bgeu	a4,a1,2006187a <__umoddi3+0x80>
20061878:	9742                	c.add	a4,a6
2006187a:	8f0d                	c.sub	a4,a1
2006187c:	02d777b3          	remu	a5,a4,a3
20061880:	0542                	c.slli	a0,0x10
20061882:	8141                	c.srli	a0,0x10
20061884:	02d75733          	divu	a4,a4,a3
20061888:	07c2                	c.slli	a5,0x10
2006188a:	8d5d                	c.or	a0,a5
2006188c:	02e60733          	mul	a4,a2,a4
20061890:	00e57863          	bgeu	a0,a4,200618a0 <__umoddi3+0xa6>
20061894:	9542                	c.add	a0,a6
20061896:	01056563          	bltu	a0,a6,200618a0 <__umoddi3+0xa6>
2006189a:	00e57363          	bgeu	a0,a4,200618a0 <__umoddi3+0xa6>
2006189e:	9542                	c.add	a0,a6
200618a0:	8d19                	c.sub	a0,a4
200618a2:	01155533          	srl	a0,a0,a7
200618a6:	4581                	c.li	a1,0
200618a8:	8082                	c.jr	ra
200618aa:	010006b7          	lui	a3,0x1000
200618ae:	47c1                	c.li	a5,16
200618b0:	f6d669e3          	bltu	a2,a3,20061822 <__umoddi3+0x28>
200618b4:	47e1                	c.li	a5,24
200618b6:	b7b5                	c.j	20061822 <__umoddi3+0x28>
200618b8:	e601                	c.bnez	a2,200618c0 <__umoddi3+0xc6>
200618ba:	4685                	c.li	a3,1
200618bc:	02c6d833          	divu	a6,a3,a2
200618c0:	66c1                	c.lui	a3,0x10
200618c2:	0ed87b63          	bgeu	a6,a3,200619b8 <__umoddi3+0x1be>
200618c6:	0ff00693          	addi	a3,zero,255
200618ca:	0106f363          	bgeu	a3,a6,200618d0 <__umoddi3+0xd6>
200618ce:	47a1                	c.li	a5,8
200618d0:	00f856b3          	srl	a3,a6,a5
200618d4:	9736                	c.add	a4,a3
200618d6:	00074603          	lbu	a2,0(a4)
200618da:	410305b3          	sub	a1,t1,a6
200618de:	963e                	c.add	a2,a5
200618e0:	02000793          	addi	a5,zero,32
200618e4:	40c788b3          	sub	a7,a5,a2
200618e8:	06c78a63          	beq	a5,a2,2006195c <__umoddi3+0x162>
200618ec:	01181833          	sll	a6,a6,a7
200618f0:	00c357b3          	srl	a5,t1,a2
200618f4:	011315b3          	sll	a1,t1,a7
200618f8:	01085313          	srli	t1,a6,0x10
200618fc:	00ced633          	srl	a2,t4,a2
20061900:	8e4d                	c.or	a2,a1
20061902:	01081593          	slli	a1,a6,0x10
20061906:	81c1                	c.srli	a1,0x10
20061908:	011e9533          	sll	a0,t4,a7
2006190c:	0267d6b3          	divu	a3,a5,t1
20061910:	0267f733          	remu	a4,a5,t1
20061914:	01065793          	srli	a5,a2,0x10
20061918:	02d586b3          	mul	a3,a1,a3
2006191c:	0742                	c.slli	a4,0x10
2006191e:	8f5d                	c.or	a4,a5
20061920:	00d77863          	bgeu	a4,a3,20061930 <__umoddi3+0x136>
20061924:	9742                	c.add	a4,a6
20061926:	01076563          	bltu	a4,a6,20061930 <__umoddi3+0x136>
2006192a:	00d77363          	bgeu	a4,a3,20061930 <__umoddi3+0x136>
2006192e:	9742                	c.add	a4,a6
20061930:	40d706b3          	sub	a3,a4,a3
20061934:	0266f733          	remu	a4,a3,t1
20061938:	0642                	c.slli	a2,0x10
2006193a:	8241                	c.srli	a2,0x10
2006193c:	0266d6b3          	divu	a3,a3,t1
20061940:	0742                	c.slli	a4,0x10
20061942:	02d586b3          	mul	a3,a1,a3
20061946:	00c765b3          	or	a1,a4,a2
2006194a:	00d5f863          	bgeu	a1,a3,2006195a <__umoddi3+0x160>
2006194e:	95c2                	c.add	a1,a6
20061950:	0105e563          	bltu	a1,a6,2006195a <__umoddi3+0x160>
20061954:	00d5f363          	bgeu	a1,a3,2006195a <__umoddi3+0x160>
20061958:	95c2                	c.add	a1,a6
2006195a:	8d95                	c.sub	a1,a3
2006195c:	01085693          	srli	a3,a6,0x10
20061960:	02d5f733          	remu	a4,a1,a3
20061964:	01081793          	slli	a5,a6,0x10
20061968:	83c1                	c.srli	a5,0x10
2006196a:	01055613          	srli	a2,a0,0x10
2006196e:	02d5d5b3          	divu	a1,a1,a3
20061972:	0742                	c.slli	a4,0x10
20061974:	8f51                	c.or	a4,a2
20061976:	02b785b3          	mul	a1,a5,a1
2006197a:	00b77863          	bgeu	a4,a1,2006198a <__umoddi3+0x190>
2006197e:	9742                	c.add	a4,a6
20061980:	01076563          	bltu	a4,a6,2006198a <__umoddi3+0x190>
20061984:	00b77363          	bgeu	a4,a1,2006198a <__umoddi3+0x190>
20061988:	9742                	c.add	a4,a6
2006198a:	40b705b3          	sub	a1,a4,a1
2006198e:	02d5f733          	remu	a4,a1,a3
20061992:	0542                	c.slli	a0,0x10
20061994:	8141                	c.srli	a0,0x10
20061996:	02d5d5b3          	divu	a1,a1,a3
2006199a:	02b785b3          	mul	a1,a5,a1
2006199e:	01071793          	slli	a5,a4,0x10
200619a2:	8d5d                	c.or	a0,a5
200619a4:	00b57863          	bgeu	a0,a1,200619b4 <__umoddi3+0x1ba>
200619a8:	9542                	c.add	a0,a6
200619aa:	01056563          	bltu	a0,a6,200619b4 <__umoddi3+0x1ba>
200619ae:	00b57363          	bgeu	a0,a1,200619b4 <__umoddi3+0x1ba>
200619b2:	9542                	c.add	a0,a6
200619b4:	8d0d                	c.sub	a0,a1
200619b6:	b5f5                	c.j	200618a2 <__umoddi3+0xa8>
200619b8:	010006b7          	lui	a3,0x1000
200619bc:	47c1                	c.li	a5,16
200619be:	f0d869e3          	bltu	a6,a3,200618d0 <__umoddi3+0xd6>
200619c2:	47e1                	c.li	a5,24
200619c4:	b731                	c.j	200618d0 <__umoddi3+0xd6>
200619c6:	eed5e1e3          	bltu	a1,a3,200618a8 <__umoddi3+0xae>
200619ca:	67c1                	c.lui	a5,0x10
200619cc:	04f6f463          	bgeu	a3,a5,20061a14 <__umoddi3+0x21a>
200619d0:	0ff00893          	addi	a7,zero,255
200619d4:	00d8b7b3          	sltu	a5,a7,a3
200619d8:	078e                	c.slli	a5,0x3
200619da:	20065737          	lui	a4,0x20065
200619de:	00f6d833          	srl	a6,a3,a5
200619e2:	a8070713          	addi	a4,a4,-1408 # 20064a80 <__clz_tab>
200619e6:	9742                	c.add	a4,a6
200619e8:	00074883          	lbu	a7,0(a4)
200619ec:	98be                	c.add	a7,a5
200619ee:	02000793          	addi	a5,zero,32
200619f2:	41178833          	sub	a6,a5,a7
200619f6:	03179663          	bne	a5,a7,20061a22 <__umoddi3+0x228>
200619fa:	0066e463          	bltu	a3,t1,20061a02 <__umoddi3+0x208>
200619fe:	eacee5e3          	bltu	t4,a2,200618a8 <__umoddi3+0xae>
20061a02:	40ce8533          	sub	a0,t4,a2
20061a06:	40d305b3          	sub	a1,t1,a3
20061a0a:	00aebeb3          	sltu	t4,t4,a0
20061a0e:	41d585b3          	sub	a1,a1,t4
20061a12:	bd59                	c.j	200618a8 <__umoddi3+0xae>
20061a14:	01000737          	lui	a4,0x1000
20061a18:	47c1                	c.li	a5,16
20061a1a:	fce6e0e3          	bltu	a3,a4,200619da <__umoddi3+0x1e0>
20061a1e:	47e1                	c.li	a5,24
20061a20:	bf6d                	c.j	200619da <__umoddi3+0x1e0>
20061a22:	011657b3          	srl	a5,a2,a7
20061a26:	010696b3          	sll	a3,a3,a6
20061a2a:	00d7ee33          	or	t3,a5,a3
20061a2e:	01135f33          	srl	t5,t1,a7
20061a32:	011ed7b3          	srl	a5,t4,a7
20061a36:	010e9533          	sll	a0,t4,a6
20061a3a:	010e5e93          	srli	t4,t3,0x10
20061a3e:	010315b3          	sll	a1,t1,a6
20061a42:	03df5333          	divu	t1,t5,t4
20061a46:	8fcd                	c.or	a5,a1
20061a48:	010e1593          	slli	a1,t3,0x10
20061a4c:	81c1                	c.srli	a1,0x10
20061a4e:	0107d713          	srli	a4,a5,0x10
20061a52:	01061633          	sll	a2,a2,a6
20061a56:	03df76b3          	remu	a3,t5,t4
20061a5a:	02658f33          	mul	t5,a1,t1
20061a5e:	06c2                	c.slli	a3,0x10
20061a60:	8f55                	c.or	a4,a3
20061a62:	869a                	c.mv	a3,t1
20061a64:	01e77c63          	bgeu	a4,t5,20061a7c <__umoddi3+0x282>
20061a68:	9772                	c.add	a4,t3
20061a6a:	fff30693          	addi	a3,t1,-1
20061a6e:	01c76763          	bltu	a4,t3,20061a7c <__umoddi3+0x282>
20061a72:	01e77563          	bgeu	a4,t5,20061a7c <__umoddi3+0x282>
20061a76:	ffe30693          	addi	a3,t1,-2
20061a7a:	9772                	c.add	a4,t3
20061a7c:	41e70733          	sub	a4,a4,t5
20061a80:	03d77333          	remu	t1,a4,t4
20061a84:	07c2                	c.slli	a5,0x10
20061a86:	83c1                	c.srli	a5,0x10
20061a88:	03d75733          	divu	a4,a4,t4
20061a8c:	0342                	c.slli	t1,0x10
20061a8e:	00f36333          	or	t1,t1,a5
20061a92:	02e585b3          	mul	a1,a1,a4
20061a96:	87ba                	c.mv	a5,a4
20061a98:	00b37c63          	bgeu	t1,a1,20061ab0 <__umoddi3+0x2b6>
20061a9c:	9372                	c.add	t1,t3
20061a9e:	fff70793          	addi	a5,a4,-1 # ffffff <__ap_sram_heap_size+0xfbffff>
20061aa2:	01c36763          	bltu	t1,t3,20061ab0 <__umoddi3+0x2b6>
20061aa6:	00b37563          	bgeu	t1,a1,20061ab0 <__umoddi3+0x2b6>
20061aaa:	ffe70793          	addi	a5,a4,-2
20061aae:	9372                	c.add	t1,t3
20061ab0:	06c2                	c.slli	a3,0x10
20061ab2:	6f41                	c.lui	t5,0x10
20061ab4:	8edd                	c.or	a3,a5
20061ab6:	40b305b3          	sub	a1,t1,a1
20061aba:	ffff0313          	addi	t1,t5,-1 # ffff <mbedtls_ecp_check_pub_priv+0x121>
20061abe:	0066feb3          	and	t4,a3,t1
20061ac2:	01065713          	srli	a4,a2,0x10
20061ac6:	82c1                	c.srli	a3,0x10
20061ac8:	00667333          	and	t1,a2,t1
20061acc:	026e87b3          	mul	a5,t4,t1
20061ad0:	02668333          	mul	t1,a3,t1
20061ad4:	02ee8eb3          	mul	t4,t4,a4
20061ad8:	02e686b3          	mul	a3,a3,a4
20061adc:	9e9a                	c.add	t4,t1
20061ade:	0107d713          	srli	a4,a5,0x10
20061ae2:	9776                	c.add	a4,t4
20061ae4:	00677363          	bgeu	a4,t1,20061aea <__umoddi3+0x2f0>
20061ae8:	96fa                	c.add	a3,t5
20061aea:	01075313          	srli	t1,a4,0x10
20061aee:	969a                	c.add	a3,t1
20061af0:	6341                	c.lui	t1,0x10
20061af2:	137d                	c.addi	t1,-1
20061af4:	00677733          	and	a4,a4,t1
20061af8:	0742                	c.slli	a4,0x10
20061afa:	0067f7b3          	and	a5,a5,t1
20061afe:	97ba                	c.add	a5,a4
20061b00:	00d5e663          	bltu	a1,a3,20061b0c <__umoddi3+0x312>
20061b04:	00d59b63          	bne	a1,a3,20061b1a <__umoddi3+0x320>
20061b08:	00f57963          	bgeu	a0,a5,20061b1a <__umoddi3+0x320>
20061b0c:	40c78633          	sub	a2,a5,a2
20061b10:	00c7b7b3          	sltu	a5,a5,a2
20061b14:	97f2                	c.add	a5,t3
20061b16:	8e9d                	c.sub	a3,a5
20061b18:	87b2                	c.mv	a5,a2
20061b1a:	40f507b3          	sub	a5,a0,a5
20061b1e:	00f53533          	sltu	a0,a0,a5
20061b22:	8d95                	c.sub	a1,a3
20061b24:	8d89                	c.sub	a1,a0
20061b26:	011598b3          	sll	a7,a1,a7
20061b2a:	0107d533          	srl	a0,a5,a6
20061b2e:	00a8e533          	or	a0,a7,a0
20061b32:	0105d5b3          	srl	a1,a1,a6
20061b36:	bb8d                	c.j	200618a8 <__umoddi3+0xae>

20061b38 <atoi>:
20061b38:	4629                	c.li	a2,10
20061b3a:	4581                	c.li	a1,0
20061b3c:	a1a5                	c.j	20061fa4 <strtol>

20061b3e <_atoi_r>:
20061b3e:	46a9                	c.li	a3,10
20061b40:	4601                	c.li	a2,0
20061b42:	a991                	c.j	20061f96 <_strtol_r>

20061b44 <__errno>:
20061b44:	e8c1a503          	lw	a0,-372(gp) # 2006683c <_impure_ptr>
20061b48:	8082                	c.jr	ra

20061b4a <__libc_init_array>:
20061b4a:	1141                	c.addi	sp,-16
20061b4c:	c422                	c.swsp	s0,8(sp)
20061b4e:	c226                	c.swsp	s1,4(sp)
20061b50:	20065437          	lui	s0,0x20065
20061b54:	200654b7          	lui	s1,0x20065
20061b58:	dd048793          	addi	a5,s1,-560 # 20064dd0 <__frame_dummy_init_array_entry>
20061b5c:	dd040413          	addi	s0,s0,-560 # 20064dd0 <__frame_dummy_init_array_entry>
20061b60:	8c1d                	c.sub	s0,a5
20061b62:	c04a                	c.swsp	s2,0(sp)
20061b64:	c606                	c.swsp	ra,12(sp)
20061b66:	8409                	c.srai	s0,0x2
20061b68:	dd048493          	addi	s1,s1,-560
20061b6c:	4901                	c.li	s2,0
20061b6e:	02891763          	bne	s2,s0,20061b9c <__libc_init_array+0x52>
20061b72:	200654b7          	lui	s1,0x20065
20061b76:	20065437          	lui	s0,0x20065
20061b7a:	dd048793          	addi	a5,s1,-560 # 20064dd0 <__frame_dummy_init_array_entry>
20061b7e:	dd440413          	addi	s0,s0,-556 # 20064dd4 <__do_global_dtors_aux_fini_array_entry>
20061b82:	8c1d                	c.sub	s0,a5
20061b84:	8409                	c.srai	s0,0x2
20061b86:	dd048493          	addi	s1,s1,-560
20061b8a:	4901                	c.li	s2,0
20061b8c:	00891d63          	bne	s2,s0,20061ba6 <__libc_init_array+0x5c>
20061b90:	40b2                	c.lwsp	ra,12(sp)
20061b92:	4422                	c.lwsp	s0,8(sp)
20061b94:	4492                	c.lwsp	s1,4(sp)
20061b96:	4902                	c.lwsp	s2,0(sp)
20061b98:	0141                	c.addi	sp,16
20061b9a:	8082                	c.jr	ra
20061b9c:	409c                	c.lw	a5,0(s1)
20061b9e:	0905                	c.addi	s2,1
20061ba0:	0491                	c.addi	s1,4
20061ba2:	9782                	c.jalr	a5
20061ba4:	b7e9                	c.j	20061b6e <__libc_init_array+0x24>
20061ba6:	409c                	c.lw	a5,0(s1)
20061ba8:	0905                	c.addi	s2,1
20061baa:	0491                	c.addi	s1,4
20061bac:	9782                	c.jalr	a5
20061bae:	bff9                	c.j	20061b8c <__libc_init_array+0x42>

20061bb0 <cleanup_glue>:
20061bb0:	1141                	c.addi	sp,-16
20061bb2:	c422                	c.swsp	s0,8(sp)
20061bb4:	842e                	c.mv	s0,a1
20061bb6:	418c                	c.lw	a1,0(a1)
20061bb8:	c226                	c.swsp	s1,4(sp)
20061bba:	c606                	c.swsp	ra,12(sp)
20061bbc:	84aa                	c.mv	s1,a0
20061bbe:	c191                	c.beqz	a1,20061bc2 <cleanup_glue+0x12>
20061bc0:	3fc5                	c.jal	20061bb0 <cleanup_glue>
20061bc2:	85a2                	c.mv	a1,s0
20061bc4:	4422                	c.lwsp	s0,8(sp)
20061bc6:	40b2                	c.lwsp	ra,12(sp)
20061bc8:	8526                	c.mv	a0,s1
20061bca:	4492                	c.lwsp	s1,4(sp)
20061bcc:	0141                	c.addi	sp,16
20061bce:	ff3f806f          	jal	zero,2005abc0 <__wrap__free_r>

20061bd2 <_reclaim_reent>:
20061bd2:	e8c1a783          	lw	a5,-372(gp) # 2006683c <_impure_ptr>
20061bd6:	0ea78263          	beq	a5,a0,20061cba <_reclaim_reent+0xe8>
20061bda:	515c                	c.lw	a5,36(a0)
20061bdc:	1101                	c.addi	sp,-32
20061bde:	cc22                	c.swsp	s0,24(sp)
20061be0:	ce06                	c.swsp	ra,28(sp)
20061be2:	ca26                	c.swsp	s1,20(sp)
20061be4:	c84a                	c.swsp	s2,16(sp)
20061be6:	c64e                	c.swsp	s3,12(sp)
20061be8:	842a                	c.mv	s0,a0
20061bea:	cf81                	c.beqz	a5,20061c02 <_reclaim_reent+0x30>
20061bec:	47dc                	c.lw	a5,12(a5)
20061bee:	4481                	c.li	s1,0
20061bf0:	08000913          	addi	s2,zero,128
20061bf4:	efc9                	c.bnez	a5,20061c8e <_reclaim_reent+0xbc>
20061bf6:	505c                	c.lw	a5,36(s0)
20061bf8:	438c                	c.lw	a1,0(a5)
20061bfa:	c581                	c.beqz	a1,20061c02 <_reclaim_reent+0x30>
20061bfc:	8522                	c.mv	a0,s0
20061bfe:	fc3f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c02:	484c                	c.lw	a1,20(s0)
20061c04:	c581                	c.beqz	a1,20061c0c <_reclaim_reent+0x3a>
20061c06:	8522                	c.mv	a0,s0
20061c08:	fb9f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c0c:	504c                	c.lw	a1,36(s0)
20061c0e:	c581                	c.beqz	a1,20061c16 <_reclaim_reent+0x44>
20061c10:	8522                	c.mv	a0,s0
20061c12:	faff80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c16:	5c0c                	c.lw	a1,56(s0)
20061c18:	c581                	c.beqz	a1,20061c20 <_reclaim_reent+0x4e>
20061c1a:	8522                	c.mv	a0,s0
20061c1c:	fa5f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c20:	5c4c                	c.lw	a1,60(s0)
20061c22:	c581                	c.beqz	a1,20061c2a <_reclaim_reent+0x58>
20061c24:	8522                	c.mv	a0,s0
20061c26:	f9bf80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c2a:	402c                	c.lw	a1,64(s0)
20061c2c:	c581                	c.beqz	a1,20061c34 <_reclaim_reent+0x62>
20061c2e:	8522                	c.mv	a0,s0
20061c30:	f91f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c34:	0ec42583          	lw	a1,236(s0)
20061c38:	c581                	c.beqz	a1,20061c40 <_reclaim_reent+0x6e>
20061c3a:	8522                	c.mv	a0,s0
20061c3c:	f85f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c40:	0e842583          	lw	a1,232(s0)
20061c44:	c581                	c.beqz	a1,20061c4c <_reclaim_reent+0x7a>
20061c46:	8522                	c.mv	a0,s0
20061c48:	f79f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c4c:	443c                	c.lw	a5,72(s0)
20061c4e:	c799                	c.beqz	a5,20061c5c <_reclaim_reent+0x8a>
20061c50:	0887a583          	lw	a1,136(a5) # 10088 <mbedtls_ecp_check_pub_priv+0x1aa>
20061c54:	c581                	c.beqz	a1,20061c5c <_reclaim_reent+0x8a>
20061c56:	8522                	c.mv	a0,s0
20061c58:	f69f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c5c:	584c                	c.lw	a1,52(s0)
20061c5e:	c581                	c.beqz	a1,20061c66 <_reclaim_reent+0x94>
20061c60:	8522                	c.mv	a0,s0
20061c62:	f5ff80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c66:	4c1c                	c.lw	a5,24(s0)
20061c68:	c3b1                	c.beqz	a5,20061cac <_reclaim_reent+0xda>
20061c6a:	541c                	c.lw	a5,40(s0)
20061c6c:	8522                	c.mv	a0,s0
20061c6e:	9782                	c.jalr	a5
20061c70:	0d842583          	lw	a1,216(s0)
20061c74:	cd85                	c.beqz	a1,20061cac <_reclaim_reent+0xda>
20061c76:	8522                	c.mv	a0,s0
20061c78:	4462                	c.lwsp	s0,24(sp)
20061c7a:	40f2                	c.lwsp	ra,28(sp)
20061c7c:	44d2                	c.lwsp	s1,20(sp)
20061c7e:	4942                	c.lwsp	s2,16(sp)
20061c80:	49b2                	c.lwsp	s3,12(sp)
20061c82:	6105                	c.addi16sp	sp,32
20061c84:	b735                	c.j	20061bb0 <cleanup_glue>
20061c86:	95a6                	c.add	a1,s1
20061c88:	418c                	c.lw	a1,0(a1)
20061c8a:	e991                	c.bnez	a1,20061c9e <_reclaim_reent+0xcc>
20061c8c:	0491                	c.addi	s1,4
20061c8e:	505c                	c.lw	a5,36(s0)
20061c90:	47cc                	c.lw	a1,12(a5)
20061c92:	ff249ae3          	bne	s1,s2,20061c86 <_reclaim_reent+0xb4>
20061c96:	8522                	c.mv	a0,s0
20061c98:	f29f80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061c9c:	bfa9                	c.j	20061bf6 <_reclaim_reent+0x24>
20061c9e:	0005a983          	lw	s3,0(a1)
20061ca2:	8522                	c.mv	a0,s0
20061ca4:	f1df80ef          	jal	ra,2005abc0 <__wrap__free_r>
20061ca8:	85ce                	c.mv	a1,s3
20061caa:	b7c5                	c.j	20061c8a <_reclaim_reent+0xb8>
20061cac:	40f2                	c.lwsp	ra,28(sp)
20061cae:	4462                	c.lwsp	s0,24(sp)
20061cb0:	44d2                	c.lwsp	s1,20(sp)
20061cb2:	4942                	c.lwsp	s2,16(sp)
20061cb4:	49b2                	c.lwsp	s3,12(sp)
20061cb6:	6105                	c.addi16sp	sp,32
20061cb8:	8082                	c.jr	ra
20061cba:	8082                	c.jr	ra

20061cbc <strcpy>:
20061cbc:	00b56733          	or	a4,a0,a1
20061cc0:	8b0d                	c.andi	a4,3
20061cc2:	87aa                	c.mv	a5,a0
20061cc4:	e729                	c.bnez	a4,20061d0e <strcpy+0x52>
20061cc6:	7f7f86b7          	lui	a3,0x7f7f8
20061cca:	f7f68693          	addi	a3,a3,-129 # 7f7f7f7f <__kr4_psram_text_start__+0x1f677f5f>
20061cce:	587d                	c.li	a6,-1
20061cd0:	4190                	c.lw	a2,0(a1)
20061cd2:	00d67733          	and	a4,a2,a3
20061cd6:	9736                	c.add	a4,a3
20061cd8:	8f51                	c.or	a4,a2
20061cda:	8f55                	c.or	a4,a3
20061cdc:	03070463          	beq	a4,a6,20061d04 <strcpy+0x48>
20061ce0:	0005c603          	lbu	a2,0(a1)
20061ce4:	0015c683          	lbu	a3,1(a1)
20061ce8:	0025c703          	lbu	a4,2(a1)
20061cec:	00c78023          	sb	a2,0(a5)
20061cf0:	c615                	c.beqz	a2,20061d1c <strcpy+0x60>
20061cf2:	00d780a3          	sb	a3,1(a5)
20061cf6:	c29d                	c.beqz	a3,20061d1c <strcpy+0x60>
20061cf8:	00e78123          	sb	a4,2(a5)
20061cfc:	c305                	c.beqz	a4,20061d1c <strcpy+0x60>
20061cfe:	000781a3          	sb	zero,3(a5)
20061d02:	8082                	c.jr	ra
20061d04:	0791                	c.addi	a5,4
20061d06:	0591                	c.addi	a1,4
20061d08:	fec7ae23          	sw	a2,-4(a5)
20061d0c:	b7d1                	c.j	20061cd0 <strcpy+0x14>
20061d0e:	0005c703          	lbu	a4,0(a1)
20061d12:	0785                	c.addi	a5,1
20061d14:	0585                	c.addi	a1,1
20061d16:	fee78fa3          	sb	a4,-1(a5)
20061d1a:	fb75                	c.bnez	a4,20061d0e <strcpy+0x52>
20061d1c:	8082                	c.jr	ra

20061d1e <strncmp>:
20061d1e:	c625                	c.beqz	a2,20061d86 <strncmp+0x68>
20061d20:	00b567b3          	or	a5,a0,a1
20061d24:	8b8d                	c.andi	a5,3
20061d26:	e385                	c.bnez	a5,20061d46 <strncmp+0x28>
20061d28:	feff06b7          	lui	a3,0xfeff0
20061d2c:	80808837          	lui	a6,0x80808
20061d30:	488d                	c.li	a7,3
20061d32:	eff68693          	addi	a3,a3,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061d36:	08080813          	addi	a6,a6,128 # 80808080 <__ctrace_end__+0xb6808080>
20061d3a:	00c8f663          	bgeu	a7,a2,20061d46 <strncmp+0x28>
20061d3e:	411c                	c.lw	a5,0(a0)
20061d40:	4198                	c.lw	a4,0(a1)
20061d42:	02e78163          	beq	a5,a4,20061d64 <strncmp+0x46>
20061d46:	167d                	c.addi	a2,-1
20061d48:	4701                	c.li	a4,0
20061d4a:	00e507b3          	add	a5,a0,a4
20061d4e:	00e586b3          	add	a3,a1,a4
20061d52:	0007c783          	lbu	a5,0(a5)
20061d56:	0006c683          	lbu	a3,0(a3)
20061d5a:	02d78263          	beq	a5,a3,20061d7e <strncmp+0x60>
20061d5e:	40d78533          	sub	a0,a5,a3
20061d62:	8082                	c.jr	ra
20061d64:	1671                	c.addi	a2,-4
20061d66:	c205                	c.beqz	a2,20061d86 <strncmp+0x68>
20061d68:	00d78733          	add	a4,a5,a3
20061d6c:	fff7c793          	xori	a5,a5,-1
20061d70:	8ff9                	c.and	a5,a4
20061d72:	0107f7b3          	and	a5,a5,a6
20061d76:	eb81                	c.bnez	a5,20061d86 <strncmp+0x68>
20061d78:	0511                	c.addi	a0,4
20061d7a:	0591                	c.addi	a1,4
20061d7c:	bf7d                	c.j	20061d3a <strncmp+0x1c>
20061d7e:	00c70463          	beq	a4,a2,20061d86 <strncmp+0x68>
20061d82:	0705                	c.addi	a4,1
20061d84:	f3f9                	c.bnez	a5,20061d4a <strncmp+0x2c>
20061d86:	4501                	c.li	a0,0
20061d88:	8082                	c.jr	ra

20061d8a <strncpy>:
20061d8a:	00a5e733          	or	a4,a1,a0
20061d8e:	8b0d                	c.andi	a4,3
20061d90:	87aa                	c.mv	a5,a0
20061d92:	e701                	c.bnez	a4,20061d9a <strncpy+0x10>
20061d94:	470d                	c.li	a4,3
20061d96:	04c76163          	bltu	a4,a2,20061dd8 <strncpy+0x4e>
20061d9a:	ca09                	c.beqz	a2,20061dac <strncpy+0x22>
20061d9c:	0005c703          	lbu	a4,0(a1)
20061da0:	0585                	c.addi	a1,1
20061da2:	0785                	c.addi	a5,1
20061da4:	fee78fa3          	sb	a4,-1(a5)
20061da8:	167d                	c.addi	a2,-1
20061daa:	fb65                	c.bnez	a4,20061d9a <strncpy+0x10>
20061dac:	963e                	c.add	a2,a5
20061dae:	02f61f63          	bne	a2,a5,20061dec <strncpy+0x62>
20061db2:	8082                	c.jr	ra
20061db4:	1671                	c.addi	a2,-4
20061db6:	0107a023          	sw	a6,0(a5)
20061dba:	0791                	c.addi	a5,4
20061dbc:	0591                	c.addi	a1,4
20061dbe:	fcce7ee3          	bgeu	t3,a2,20061d9a <strncpy+0x10>
20061dc2:	0005a803          	lw	a6,0(a1)
20061dc6:	01180733          	add	a4,a6,a7
20061dca:	fff84313          	xori	t1,a6,-1
20061dce:	00677733          	and	a4,a4,t1
20061dd2:	8f75                	c.and	a4,a3
20061dd4:	d365                	c.beqz	a4,20061db4 <strncpy+0x2a>
20061dd6:	b7d1                	c.j	20061d9a <strncpy+0x10>
20061dd8:	feff08b7          	lui	a7,0xfeff0
20061ddc:	808086b7          	lui	a3,0x80808
20061de0:	eff88893          	addi	a7,a7,-257 # fefefeff <__ctrace_end__+0x34fefeff>
20061de4:	08068693          	addi	a3,a3,128 # 80808080 <__ctrace_end__+0xb6808080>
20061de8:	4e0d                	c.li	t3,3
20061dea:	bfe1                	c.j	20061dc2 <strncpy+0x38>
20061dec:	0785                	c.addi	a5,1
20061dee:	fe078fa3          	sb	zero,-1(a5)
20061df2:	bf75                	c.j	20061dae <strncpy+0x24>

20061df4 <strsep>:
20061df4:	862a                	c.mv	a2,a0
20061df6:	4108                	c.lw	a0,0(a0)
20061df8:	4681                	c.li	a3,0
20061dfa:	a009                	c.j	20061dfc <__strtok_r>

20061dfc <__strtok_r>:
20061dfc:	e119                	c.bnez	a0,20061e02 <__strtok_r+0x6>
20061dfe:	4208                	c.lw	a0,0(a2)
20061e00:	c939                	c.beqz	a0,20061e56 <__strtok_r+0x5a>
20061e02:	87aa                	c.mv	a5,a0
20061e04:	853e                	c.mv	a0,a5
20061e06:	0007c803          	lbu	a6,0(a5)
20061e0a:	0785                	c.addi	a5,1
20061e0c:	872e                	c.mv	a4,a1
20061e0e:	00074883          	lbu	a7,0(a4)
20061e12:	0705                	c.addi	a4,1
20061e14:	00089863          	bne	a7,zero,20061e24 <__strtok_r+0x28>
20061e18:	02081063          	bne	a6,zero,20061e38 <__strtok_r+0x3c>
20061e1c:	00062023          	sw	zero,0(a2)
20061e20:	4501                	c.li	a0,0
20061e22:	8082                	c.jr	ra
20061e24:	ff1815e3          	bne	a6,a7,20061e0e <__strtok_r+0x12>
20061e28:	fef1                	c.bnez	a3,20061e04 <__strtok_r+0x8>
20061e2a:	c21c                	c.sw	a5,0(a2)
20061e2c:	00050023          	sb	zero,0(a0)
20061e30:	8082                	c.jr	ra
20061e32:	4781                	c.li	a5,0
20061e34:	a839                	c.j	20061e52 <__strtok_r+0x56>
20061e36:	e689                	c.bnez	a3,20061e40 <__strtok_r+0x44>
20061e38:	0007c803          	lbu	a6,0(a5)
20061e3c:	0785                	c.addi	a5,1
20061e3e:	872e                	c.mv	a4,a1
20061e40:	00074683          	lbu	a3,0(a4)
20061e44:	0705                	c.addi	a4,1
20061e46:	fed818e3          	bne	a6,a3,20061e36 <__strtok_r+0x3a>
20061e4a:	fe0804e3          	beq	a6,zero,20061e32 <__strtok_r+0x36>
20061e4e:	fe078fa3          	sb	zero,-1(a5)
20061e52:	c21c                	c.sw	a5,0(a2)
20061e54:	8082                	c.jr	ra
20061e56:	8082                	c.jr	ra

20061e58 <strtok_r>:
20061e58:	4685                	c.li	a3,1
20061e5a:	b74d                	c.j	20061dfc <__strtok_r>

20061e5c <_strtol_l.constprop.0>:
20061e5c:	4785                	c.li	a5,1
20061e5e:	0af68863          	beq	a3,a5,20061f0e <_strtol_l.constprop.0+0xb2>
20061e62:	02400793          	addi	a5,zero,36
20061e66:	872e                	c.mv	a4,a1
20061e68:	0ad7e363          	bltu	a5,a3,20061f0e <_strtol_l.constprop.0+0xb2>
20061e6c:	20065837          	lui	a6,0x20065
20061e70:	8eaa                	c.mv	t4,a0
20061e72:	b8180813          	addi	a6,a6,-1151 # 20064b81 <_ctype_+0x1>
20061e76:	88ba                	c.mv	a7,a4
20061e78:	00074783          	lbu	a5,0(a4)
20061e7c:	0705                	c.addi	a4,1
20061e7e:	01078533          	add	a0,a5,a6
20061e82:	00054503          	lbu	a0,0(a0)
20061e86:	8921                	c.andi	a0,8
20061e88:	f57d                	c.bnez	a0,20061e76 <_strtol_l.constprop.0+0x1a>
20061e8a:	02d00513          	addi	a0,zero,45
20061e8e:	08a79a63          	bne	a5,a0,20061f22 <_strtol_l.constprop.0+0xc6>
20061e92:	00074783          	lbu	a5,0(a4)
20061e96:	4e05                	c.li	t3,1
20061e98:	00288713          	addi	a4,a7,2
20061e9c:	c6f5                	c.beqz	a3,20061f88 <_strtol_l.constprop.0+0x12c>
20061e9e:	4541                	c.li	a0,16
20061ea0:	02a69263          	bne	a3,a0,20061ec4 <_strtol_l.constprop.0+0x68>
20061ea4:	03000513          	addi	a0,zero,48
20061ea8:	00a79e63          	bne	a5,a0,20061ec4 <_strtol_l.constprop.0+0x68>
20061eac:	00074783          	lbu	a5,0(a4)
20061eb0:	05800513          	addi	a0,zero,88
20061eb4:	0df7f793          	andi	a5,a5,223
20061eb8:	0ca79363          	bne	a5,a0,20061f7e <_strtol_l.constprop.0+0x122>
20061ebc:	00174783          	lbu	a5,1(a4)
20061ec0:	46c1                	c.li	a3,16
20061ec2:	0709                	c.addi	a4,2
20061ec4:	800008b7          	lui	a7,0x80000
20061ec8:	fff8c893          	xori	a7,a7,-1
20061ecc:	98f2                	c.add	a7,t3
20061ece:	02d8f2b3          	remu	t0,a7,a3
20061ed2:	4301                	c.li	t1,0
20061ed4:	4501                	c.li	a0,0
20061ed6:	43a5                	c.li	t2,9
20061ed8:	4fe5                	c.li	t6,25
20061eda:	02d8df33          	divu	t5,a7,a3
20061ede:	fd078813          	addi	a6,a5,-48
20061ee2:	0503ea63          	bltu	t2,a6,20061f36 <_strtol_l.constprop.0+0xda>
20061ee6:	87c2                	c.mv	a5,a6
20061ee8:	06d7d763          	bge	a5,a3,20061f56 <_strtol_l.constprop.0+0xfa>
20061eec:	06034363          	blt	t1,zero,20061f52 <_strtol_l.constprop.0+0xf6>
20061ef0:	537d                	c.li	t1,-1
20061ef2:	00af6a63          	bltu	t5,a0,20061f06 <_strtol_l.constprop.0+0xaa>
20061ef6:	00af1463          	bne	t5,a0,20061efe <_strtol_l.constprop.0+0xa2>
20061efa:	00f2c663          	blt	t0,a5,20061f06 <_strtol_l.constprop.0+0xaa>
20061efe:	4305                	c.li	t1,1
20061f00:	02a68533          	mul	a0,a3,a0
20061f04:	953e                	c.add	a0,a5
20061f06:	0705                	c.addi	a4,1
20061f08:	fff74783          	lbu	a5,-1(a4)
20061f0c:	bfc9                	c.j	20061ede <_strtol_l.constprop.0+0x82>
20061f0e:	1141                	c.addi	sp,-16
20061f10:	c606                	c.swsp	ra,12(sp)
20061f12:	c33ff0ef          	jal	ra,20061b44 <__errno>
20061f16:	40b2                	c.lwsp	ra,12(sp)
20061f18:	47d9                	c.li	a5,22
20061f1a:	c11c                	c.sw	a5,0(a0)
20061f1c:	4501                	c.li	a0,0
20061f1e:	0141                	c.addi	sp,16
20061f20:	8082                	c.jr	ra
20061f22:	02b00513          	addi	a0,zero,43
20061f26:	4e01                	c.li	t3,0
20061f28:	f6a79ae3          	bne	a5,a0,20061e9c <_strtol_l.constprop.0+0x40>
20061f2c:	00074783          	lbu	a5,0(a4)
20061f30:	00288713          	addi	a4,a7,2 # 80000002 <__ctrace_end__+0xb6000002>
20061f34:	b7a5                	c.j	20061e9c <_strtol_l.constprop.0+0x40>
20061f36:	fbf78813          	addi	a6,a5,-65
20061f3a:	010fe563          	bltu	t6,a6,20061f44 <_strtol_l.constprop.0+0xe8>
20061f3e:	fc978793          	addi	a5,a5,-55
20061f42:	b75d                	c.j	20061ee8 <_strtol_l.constprop.0+0x8c>
20061f44:	f9f78813          	addi	a6,a5,-97
20061f48:	010fe763          	bltu	t6,a6,20061f56 <_strtol_l.constprop.0+0xfa>
20061f4c:	fa978793          	addi	a5,a5,-87
20061f50:	bf61                	c.j	20061ee8 <_strtol_l.constprop.0+0x8c>
20061f52:	537d                	c.li	t1,-1
20061f54:	bf4d                	c.j	20061f06 <_strtol_l.constprop.0+0xaa>
20061f56:	00035963          	bge	t1,zero,20061f68 <_strtol_l.constprop.0+0x10c>
20061f5a:	02200793          	addi	a5,zero,34
20061f5e:	00fea023          	sw	a5,0(t4)
20061f62:	8546                	c.mv	a0,a7
20061f64:	ea09                	c.bnez	a2,20061f76 <_strtol_l.constprop.0+0x11a>
20061f66:	8082                	c.jr	ra
20061f68:	000e0463          	beq	t3,zero,20061f70 <_strtol_l.constprop.0+0x114>
20061f6c:	40a00533          	sub	a0,zero,a0
20061f70:	c215                	c.beqz	a2,20061f94 <_strtol_l.constprop.0+0x138>
20061f72:	00030463          	beq	t1,zero,20061f7a <_strtol_l.constprop.0+0x11e>
20061f76:	fff70593          	addi	a1,a4,-1
20061f7a:	c20c                	c.sw	a1,0(a2)
20061f7c:	8082                	c.jr	ra
20061f7e:	03000793          	addi	a5,zero,48
20061f82:	f2a9                	c.bnez	a3,20061ec4 <_strtol_l.constprop.0+0x68>
20061f84:	46a1                	c.li	a3,8
20061f86:	bf3d                	c.j	20061ec4 <_strtol_l.constprop.0+0x68>
20061f88:	03000513          	addi	a0,zero,48
20061f8c:	f2a780e3          	beq	a5,a0,20061eac <_strtol_l.constprop.0+0x50>
20061f90:	46a9                	c.li	a3,10
20061f92:	bf0d                	c.j	20061ec4 <_strtol_l.constprop.0+0x68>
20061f94:	8082                	c.jr	ra

20061f96 <_strtol_r>:
20061f96:	b5d9                	c.j	20061e5c <_strtol_l.constprop.0>

20061f98 <strtol_l>:
20061f98:	86b2                	c.mv	a3,a2
20061f9a:	862e                	c.mv	a2,a1
20061f9c:	85aa                	c.mv	a1,a0
20061f9e:	e8c1a503          	lw	a0,-372(gp) # 2006683c <_impure_ptr>
20061fa2:	bd6d                	c.j	20061e5c <_strtol_l.constprop.0>

20061fa4 <strtol>:
20061fa4:	86b2                	c.mv	a3,a2
20061fa6:	862e                	c.mv	a2,a1
20061fa8:	85aa                	c.mv	a1,a0
20061faa:	e8c1a503          	lw	a0,-372(gp) # 2006683c <_impure_ptr>
20061fae:	b57d                	c.j	20061e5c <_strtol_l.constprop.0>
20061fb0:	6d6f682f          	0x6d6f682f
20061fb4:	2f65                	c.jal	2006276c <pmap_func+0x3a0>
20061fb6:	7561                	c.lui	a0,0xffff8
20061fb8:	6f74                	c.flw	fa3,92(a4)
20061fba:	6a5f 6e65 696b      	0x696b6e656a5f
20061fc0:	736e                	c.flwsp	ft6,248(sp)
20061fc2:	775f 616c 356e      	0x356e616c775f
20061fc8:	544f492f          	0x544f492f
20061fcc:	415f 5354 722f      	0x722f5354415f
20061fd2:	7065                	c.lui	zero,0xffff9
20061fd4:	722f736f          	jal	t1,201596f6 <__ap_sram_heap_start+0xd96f6>
20061fd8:	6c65                	c.lui	s8,0x19
20061fda:	6165                	c.addi16sp	sp,112
20061fdc:	6d2d6573          	csrrsi	a0,0x6d2,26
20061fe0:	7361                	c.lui	t1,0xffff8
20061fe2:	6574                	c.flw	fa3,76(a0)
20061fe4:	2f72                	c.fldsp	ft10,280(sp)
20061fe6:	72756f73          	csrrsi	t5,0x727,10
20061fea:	612f6563          	bltu	t5,s2,200625f4 <pmap_func+0x228>
20061fee:	656d                	c.lui	a0,0x1b
20061ff0:	6162                	c.flwsp	ft2,24(sp)
20061ff2:	696c                	c.flw	fa1,84(a0)
20061ff4:	6574                	c.flw	fa3,76(a0)
20061ff6:	675f 6363 705f      	0x705f6363675f
20061ffc:	6f72                	c.flwsp	ft10,28(sp)
20061ffe:	656a                	c.flwsp	fa0,152(sp)
20062000:	702f7463          	bgeu	t5,sp,20062708 <pmap_func+0x33c>
20062004:	6f72                	c.flwsp	ft10,28(sp)
20062006:	656a                	c.flwsp	fa0,152(sp)
20062008:	6b5f7463          	bgeu	t5,s5,200626b0 <pmap_func+0x2e4>
2006200c:	3472                	c.fldsp	fs0,312(sp)
2006200e:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
20062012:	2e2e2f6b          	0x2e2e2f6b
20062016:	2f2e2e2f          	0x2f2e2e2f
2006201a:	2e2e                	c.fldsp	ft8,200(sp)
2006201c:	6d6f632f          	0x6d6f632f
20062020:	6f70                	c.flw	fa2,92(a4)
20062022:	656e                	c.flwsp	fa0,216(sp)
20062024:	746e                	c.flwsp	fs0,248(sp)
20062026:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
2006202a:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
2006202e:	6162                	c.flwsp	ft2,24(sp)
20062030:	696c                	c.flw	fa1,84(a0)
20062032:	6574                	c.flw	fa3,76(a0)
20062034:	6c77662f          	0x6c77662f
20062038:	6269                	c.lui	tp,0x1a
2006203a:	6d61722f          	0x6d61722f
2006203e:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20062044:	2f6e                	c.fldsp	ft10,216(sp)
20062046:	6d61                	c.lui	s10,0x18
20062048:	6265                	c.lui	tp,0x19
2006204a:	5f61                	c.li	t5,-8
2006204c:	6c66                	c.flwsp	fs8,88(sp)
2006204e:	7361                	c.lui	t1,0xffff8
20062050:	5f68                	c.lw	a0,124(a4)
20062052:	6172                	c.flwsp	ft2,28(sp)
20062054:	2e6d                	c.jal	2006240e <pmap_func+0x42>
20062056:	68430063          	beq	t1,tp,200626d6 <pmap_func+0x30a>
2006205a:	6e61                	c.lui	t3,0x18
2006205c:	656e                	c.flwsp	fa0,216(sp)
2006205e:	206c                	c.fld	fa1,192(s0)
20062060:	666e6f43          	fmadd.q	ft10,ft8,ft6,fa2,unknown
20062064:	696c                	c.flw	fa1,84(a0)
20062066:	66207463          	bgeu	zero,sp,200626ce <pmap_func+0x302>
2006206a:	4320726f          	jal	tp,2006949c <__bdram_heap_buffer_start__+0xadc>
2006206e:	5550                	c.lw	a2,44(a0)
20062070:	2520                	c.fld	fs0,72(a0)
20062072:	756c                	c.flw	fa1,108(a0)
20062074:	4320                	c.lw	s0,64(a4)
20062076:	6168                	c.flw	fa0,68(a0)
20062078:	6e6e                	c.flwsp	ft8,216(sp)
2006207a:	6c65                	c.lui	s8,0x19
2006207c:	2520                	c.fld	fs0,72(a0)
2006207e:	756c                	c.flw	fa1,108(a0)
20062080:	2120                	c.fld	fs0,64(a0)
20062082:	4920                	c.lw	s0,80(a0)
20062084:	726f6e67          	0x726f6e67
20062088:	2065                	c.jal	20062130 <strtol+0x18c>
2006208a:	6649                	c.lui	a2,0x12
2006208c:	4320                	c.lw	s0,64(a4)
2006208e:	5550                	c.lw	a2,44(a0)
20062090:	4820                	c.lw	s0,80(s0)
20062092:	7361                	c.lui	t1,0xffff8
20062094:	5220                	c.lw	s0,96(a2)
20062096:	7365                	c.lui	t1,0xffff9
20062098:	7465                	c.lui	s0,0xffff9
2006209a:	0a0d                	c.addi	s4,3
2006209c:	0000                	c.unimp
2006209e:	0000                	c.unimp
200620a0:	5049                	c.li	zero,-14
200620a2:	49200043          	fmadd.s	ft0,ft0,fs2,fs1,rne
200620a6:	4350                	c.lw	a2,4(a4)
200620a8:	5220                	c.lw	s0,96(a2)
200620aa:	7165                	c.addi16sp	sp,-400
200620ac:	6575                	c.lui	a0,0x1d
200620ae:	54207473          	csrrci	s0,0x542,0
200620b2:	6d69                	c.lui	s10,0x1a
200620b4:	6f65                	c.lui	t5,0x19
200620b6:	7475                	c.lui	s0,0xffffd
200620b8:	0a0d                	c.addi	s4,3
200620ba:	0000                	c.unimp
200620bc:	4920                	c.lw	s0,80(a0)
200620be:	4350                	c.lw	a2,4(a4)
200620c0:	4720                	c.lw	s0,72(a4)
200620c2:	7465                	c.lui	s0,0xffff9
200620c4:	5320                	c.lw	s0,96(a4)
200620c6:	6d65                	c.lui	s10,0x19
200620c8:	7061                	c.lui	zero,0xffff8
200620ca:	6f68                	c.flw	fa0,92(a4)
200620cc:	6572                	c.flwsp	fa0,28(sp)
200620ce:	5420                	c.lw	s0,104(s0)
200620d0:	6d69                	c.lui	s10,0x1a
200620d2:	6f65                	c.lui	t5,0x19
200620d4:	7475                	c.lui	s0,0xffffd
200620d6:	0a0d                	c.addi	s4,3
200620d8:	0000                	c.unimp
200620da:	0000                	c.unimp
200620dc:	6d6f682f          	0x6d6f682f
200620e0:	2f65                	c.jal	20062898 <pmap_func+0x4cc>
200620e2:	7561                	c.lui	a0,0xffff8
200620e4:	6f74                	c.flw	fa3,92(a4)
200620e6:	6a5f 6e65 696b      	0x696b6e656a5f
200620ec:	736e                	c.flwsp	ft6,248(sp)
200620ee:	775f 616c 356e      	0x356e616c775f
200620f4:	544f492f          	0x544f492f
200620f8:	415f 5354 722f      	0x722f5354415f
200620fe:	7065                	c.lui	zero,0xffff9
20062100:	722f736f          	jal	t1,20159822 <__ap_sram_heap_start+0xd9822>
20062104:	6c65                	c.lui	s8,0x19
20062106:	6165                	c.addi16sp	sp,112
20062108:	6d2d6573          	csrrsi	a0,0x6d2,26
2006210c:	7361                	c.lui	t1,0xffff8
2006210e:	6574                	c.flw	fa3,76(a0)
20062110:	2f72                	c.fldsp	ft10,280(sp)
20062112:	72756f73          	csrrsi	t5,0x727,10
20062116:	612f6563          	bltu	t5,s2,20062720 <pmap_func+0x354>
2006211a:	656d                	c.lui	a0,0x1b
2006211c:	6162                	c.flwsp	ft2,24(sp)
2006211e:	696c                	c.flw	fa1,84(a0)
20062120:	6574                	c.flw	fa3,76(a0)
20062122:	675f 6363 705f      	0x705f6363675f
20062128:	6f72                	c.flwsp	ft10,28(sp)
2006212a:	656a                	c.flwsp	fa0,152(sp)
2006212c:	702f7463          	bgeu	t5,sp,20062834 <pmap_func+0x468>
20062130:	6f72                	c.flwsp	ft10,28(sp)
20062132:	656a                	c.flwsp	fa0,152(sp)
20062134:	6b5f7463          	bgeu	t5,s5,200627dc <pmap_func+0x410>
20062138:	3472                	c.fldsp	fs0,312(sp)
2006213a:	6473762f          	vamoandei64.v	v12,(t1),v7,v12,v0.t
2006213e:	2e2e2f6b          	0x2e2e2f6b
20062142:	2f2e2e2f          	0x2f2e2e2f
20062146:	2e2e                	c.fldsp	ft8,200(sp)
20062148:	6d6f632f          	0x6d6f632f
2006214c:	6f70                	c.flw	fa2,92(a4)
2006214e:	656e                	c.flwsp	fa0,216(sp)
20062150:	746e                	c.flwsp	fs0,248(sp)
20062152:	636f732f          	vamoandei64.v	zero,(t5),v22,v6
20062156:	656d612f          	vamoandei32.v	v2,(s10),v22,v2,v0.t
2006215a:	6162                	c.flwsp	ft2,24(sp)
2006215c:	696c                	c.flw	fa1,84(a0)
2006215e:	6574                	c.flw	fa3,76(a0)
20062160:	6c77662f          	0x6c77662f
20062164:	6269                	c.lui	tp,0x1a
20062166:	6d61722f          	0x6d61722f
2006216a:	635f 6d6f 6f6d      	0x6f6d6d6f635f
20062170:	2f6e                	c.fldsp	ft10,216(sp)
20062172:	6d61                	c.lui	s10,0x18
20062174:	6265                	c.lui	tp,0x19
20062176:	5f61                	c.li	t5,-8
20062178:	7069                	c.lui	zero,0xffffa
2006217a:	70615f63          	bge	sp,t1,20062898 <pmap_func+0x4cc>
2006217e:	2e69                	c.jal	20062518 <pmap_func+0x14c>
20062180:	00000063          	beq	zero,zero,20062180 <strtol+0x1dc>
20062184:	5049                	c.li	zero,-14
20062186:	69445f43          	fmadd.s	ft10,fs0,fs4,fa3,unknown
2006218a:	3a72                	c.fldsp	fs4,312(sp)
2006218c:	6425                	c.lui	s0,0x9
2006218e:	202c                	c.fld	fa1,64(s0)
20062190:	5049                	c.li	zero,-14
20062192:	58545f43          	fmadd.s	ft10,fs0,ft5,fa1,unknown
20062196:	445f 5441 2041      	0x20415441445f
2006219c:	7830                	c.flw	fa2,112(s0)
2006219e:	7825                	c.lui	a6,0xfffe9
200621a0:	4e20                	c.lw	s0,88(a2)
200621a2:	4320746f          	jal	s0,200695d4 <__bdram_heap_buffer_start__+0xc14>
200621a6:	656c                	c.flw	fa1,76(a0)
200621a8:	6e61                	c.lui	t3,0x18
200621aa:	0d21                	c.addi	s10,8
200621ac:	000a                	c.slli	zero,0x2
	...

200621b0 <CSWTCH.29>:
200621b0:	0008 0000 0000 0000 0000 0000 0000 0000     ................
200621c0:	0008 0000 0000 0000 0000 0000 0000 0000     ................
200621d0:	0008 0000                                   ....

200621d4 <CSWTCH.30>:
200621d4:	0000 4102 0000 4102 0000 0000 0000 0000     ...A...A........
	...
20062214:	0080 4102 0080 4102 0000 0000 0000 0000     ...A...A........
	...
20062254:	0100 4102 0100 4102 682f 6d6f 2f65 7561     ...A...A/home/au
20062264:	6f74 6a5f 6e65 696b 736e 775f 616c 356e     to_jenkins_wlan5
20062274:	492f 544f 415f 5354 722f 7065 736f 722f     /IOT_ATS/repos/r
20062284:	6c65 6165 6573 6d2d 7361 6574 2f72 6f73     elease-master/so
20062294:	7275 6563 612f 656d 6162 696c 6574 675f     urce/amebalite_g
200622a4:	6363 705f 6f72 656a 7463 702f 6f72 656a     cc_project/proje
200622b4:	7463 6b5f 3472 762f 6473 2f6b 2e2e 2e2f     ct_kr4/vsdk/../.
200622c4:	2f2e 2e2e 632f 6d6f 6f70 656e 746e 732f     ./../component/s
200622d4:	636f 612f 656d 6162 696c 6574 662f 6c77     oc/amebalite/fwl
200622e4:	6269 722f 6d61 635f 6d6f 6f6d 2f6e 6d61     ib/ram_common/am
200622f4:	6265 5f61 7069 5f63 6172 2e6d 0063 0000     eba_ipc_ram.c...
20062304:	5043 2055 6425 7420 6b61 2065 6573 616d     CPU %d take sema
20062314:	2520 2064 6f66 2072 7870 6469 2520 2064      %d for pxid %d 
20062324:	6166 6c69 203a 7063 6975 2064 6425 7020     fail: cpuid %d p
20062334:	6978 2064 6425 6820 7361 7420 6b61 6e65     xid %d has taken
20062344:	7420 6568 7320 6d65 0a61 0000 5043 2055      the sema...CPU 
20062354:	6425 6620 6572 2065 6573 616d 2520 2064     %d free sema %d 
20062364:	6f66 2072 7870 6469 2520 2064 6166 6c69     for pxid %d fail
20062374:	203a 7063 6975 2064 6425 202c 7870 6469     : cpuid %d, pxid
20062384:	2520 2064 7369 7420 6b61 6e69 2067 6874      %d is taking th
20062394:	2065 6573 616d 000a 544f 2050 6572 6461     e sema..OTP read
200623a4:	6720 7465 6820 2077 6573 616d 6620 6961      get hw sema fai
200623b4:	0a6c 0000 544f 5f50 6552 6461 2038 6146     l...OTP_Read8 Fa
200623c4:	6c69 2520 2078 000a                         il %x ..

200623cc <pmap_func>:
200623cc:	0000 0101 0001 0101 0002 0102 0003 0102     ................
200623dc:	0004 0202 0005 0102 0006 0202 0007 0102     ................
200623ec:	0008 0101 0009 0101 000a 0101 000b 0101     ................
200623fc:	000c 0101 000d 0101 000e 0101 000f 0101     ................
2006240c:	0010 0101 0011 0101 0012 0000 0013 0202     ................
2006241c:	0014 0202 0015 0101 0016 0000 0017 0101     ................
2006242c:	0018 0101 0019 0101 001a 0101 001b 0101     ................
2006243c:	001c 0101 001d 0101 001e 0101 001f 0101     ................
2006244c:	0020 0101 0021 0101 0022 0101 0023 0101      ...!..."...#...
2006245c:	0024 0101 0025 0101 0026 0101 0027 0101     $...%...&...'...
2006246c:	0028 0101 0029 0101 002a 0101 002b 0100     (...)...*...+...
2006247c:	002c 0100 002d 0100 002e 0100 002f 0100     ,...-......./...
2006248c:	0030 0100 0031 0100 0032 0100 0033 0100     0...1...2...3...
2006249c:	ffff ffff 766e 6369 695f 7172 2520 756c     ....nvic_irq %lu
200624ac:	4620 6e75 4e20 746f 4120 7373 6769 216e      Fun Not Assign!
200624bc:	2121 2121 000a 0000 4c50 4349 0000 0000     !!!!....PLIC....
200624cc:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
200624dc:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
200624ec:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
200624fc:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
2006250c:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
2006251c:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
2006252c:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
2006253c:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
2006254c:	696c 6574 6d2f 7369 2f63 6d61 6265 5f61     lite/misc/ameba_
2006255c:	6d70 2e75 0063 0000 6e49 6176 696c 2064     pmu.c...Invalid 
2006256c:	7350 6172 436d 6b6c 000a 0000 5350 4152     PsramClk....PSRA
2006257c:	004d 0000 7370 6172 206d 7974 6570 6920     M...psram type i
2006258c:	2073 7830 7825 000a 5353 4549 524e 3009     s 0x%x..SSIENR.0
2006259c:	2578 3d78 7830 7825 000a 0000 5443 4c52     x%x=0x%x....CTRL
200625ac:	3052 3009 2578 3d78 7830 7825 000a 0000     R0.0x%x=0x%x....
200625bc:	4142 4455 0952 7830 7825 303d 2578 0a78     BAUDR.0x%x=0x%x.
200625cc:	0000 0000 4156 494c 5f44 4d43 0944 7830     ....VALID_CMD.0x
200625dc:	7825 303d 2578 0a78 0000 0000 5443 4c52     %x=0x%x.....CTRL
200625ec:	3252 3009 2578 3d78 7830 7825 000a 0000     R2.0x%x=0x%x....
200625fc:	4544 4956 4543 495f 464e 094f 7830 7825     DEVICE_INFO.0x%x
2006260c:	303d 2578 0a78 0000 5054 3052 3009 2578     =0x%x...TPR0.0x%
2006261c:	3d78 7830 7825 000a 4552 4441 465f 5341     x=0x%x..READ_FAS
2006262c:	5f54 4953 474e 454c 3009 2578 3d78 7830     T_SINGLE.0x%x=0x
2006263c:	7825 000a 5257 5449 5f45 4953 4e47 454c     %x..WRITE_SIGNLE
2006264c:	3009 2578 3d78 7830 7825 000a 5541 4f54     .0x%x=0x%x..AUTO
2006265c:	4c5f 4e45 5447 0948 7830 7825 303d 2578     _LENGTH.0x%x=0x%
2006266c:	0a78 0000 5355 5245 4c5f 4e45 5447 0948     x...USER_LENGTH.
2006267c:	7830 7825 303d 2578 0a78 0000 5541 4f54     0x%x=0x%x...AUTO
2006268c:	4c5f 4e45 5447 3248 3009 2578 3d78 7830     _LENGTH2.0x%x=0x
2006269c:	7825 000a 5253 3009 2578 3d78 7830 7825     %x..SR.0x%x=0x%x
200626ac:	000a 0000 5350 4850 5f59 4143 5f4c 4150     ....PSPHY_CAL_PA
200626bc:	0952 7830 7825 303d 2578 0a78 0000 0000     R.0x%x=0x%x.....
200626cc:	5350 4850 5f59 4850 5f59 5443 4c52 3009     PSPHY_PHY_CTRL.0
200626dc:	2578 3d78 7830 7825 000a 0000 5350 4850     x%x=0x%x....PSPH
200626ec:	5f59 4244 5f47 4553 094c 7830 7825 303d     Y_DBG_SEL.0x%x=0
200626fc:	2578 0a78 0000 0000 5249 5b30 5d31 303d     x%x.....IR0[1]=0
2006270c:	2578 0a78 0000 0000 5249 5b30 5d30 303d     x%x.....IR0[0]=0
2006271c:	2578 0a78 0000 0000 5243 5b30 5d31 303d     x%x.....CR0[1]=0
2006272c:	2578 0a78 0000 0000 5243 5b30 5d30 303d     x%x.....CR0[0]=0
2006273c:	2578 0a78 0000 0000 5243 5b31 5d31 303d     x%x.....CR1[1]=0
2006274c:	2578 0a78 0000 0000 5243 5b31 5d30 303d     x%x.....CR1[0]=0
2006275c:	2578 0a78 0000 0000 3d3d 703d 6168 6573     x%x.....===phase
2006276c:	2520 786c 3d20 3d3d 3d3d 000a 6b6f 2520      %lx =====..ok %
2006277c:	786c 2520 786c 2520 786c 2520 786c 2520     lx %lx %lx %lx %
2006278c:	786c 2520 786c 2520 786c 000a 6166 6c69     lx %lx %lx..fail
2006279c:	2520 786c 2520 786c 2520 786c 2520 786c      %lx %lx %lx %lx
200627ac:	2520 786c 2520 786c 2520 786c 000a 0000      %lx %lx %lx....
200627bc:	6143 4e6c 696d 206e 203d 7825 4320 6c61     CalNmin = %x Cal
200627cc:	6d4e 7861 3d20 2520 2078 6957 646e 776f     Nmax = %x Window
200627dc:	6953 657a 3d20 2520 2078 6870 7361 3a65     Size = %x phase:
200627ec:	2520 2078 000a 0000 5441 0000 003d 0000      %x ....AT..=...
200627fc:	002c 0000 7769 7270 7669 0020 0d0a 4d5b     ,...iwpriv ...[M
2006280c:	4d45 205d 6641 6574 2072 6f64 6320 646d     EM] After do cmd
2006281c:	202c 7661 6961 616c 6c62 2065 6568 7061     , available heap
2006282c:	2520 0a64 000d 0000 0a0d 230a 0a0d 0000      %d........#....
2006283c:	0000 0000 4f2b 4154 4c43 4145 3a52 0000     ....+OTACLEAR:..
2006284c:	0a0d 7325 4b4f 0a0d 0000 0000 4f2b 4154     ..%sOK......+OTA
2006285c:	4552 4f43 4556 3a52 0000 0000 0a0d 6120     RECOVER:...... a
2006286c:	6772 6c20 6e65 3d20 2520 2064 0a0d 0000     rg len = %d ....
2006287c:	0a0d 6120 6772 3d20 2520 2073 0a0d 0000     .. arg = %s ....
2006288c:	542b 5345 3a54 0000 522b 5453 003a 0000     +TEST:..+RST:...
2006289c:	6154 6b73 4c20 7369 3a74 0d20 250a 0d73     Task List: ..%s.
200628ac:	000a 0000 6548 7061 7453 7461 3a73 0d20     ....HeapStats: .
200628bc:	000a 0000 6f54 6174 206c 6548 7061 093a     ....Total Heap:.
200628cc:	7525 0a0d 0000 0000 6548 7061 4620 6572     %u......Heap Fre
200628dc:	2065 6f4e 3a77 2509 0d75 000a 6548 7061     e Now:.%u...Heap
200628ec:	5520 6573 2064 6f4e 3a77 2509 0d75 000a      Used Now:.%u...
200628fc:	6548 7061 5520 6573 2064 614d 3a78 2509     Heap Used Max:.%
2006290c:	0d75 000a 532b 4154 4554 003a 4d41 4245     u...+STATE:.AMEB
2006291c:	2d41 5452 534f 5320 4b44 5620 5245 4953     A-RTOS SDK VERSI
2006292c:	4e4f 203a 6425 252e 2e64 6425 000a 0000     ON: %d.%d.%d....
2006293c:	7325 0000 3056 2e32 2e31 0031 3376 352e     %s..V02.1.1.v3.5
2006294c:	0000 0000 3032 3432 312f 2f30 3131 312d     ....2024/10/11-1
2006295c:	3a35 3635 333a 0030 472b 524d 003a 0000     5:56:30.+GMR:...
2006296c:	0a0d 7325 7325 252c 2c73 7325 0a0d 0000     ..%s%s,%s,%s....
2006297c:	4c5b 474f 205d 5245 4f52 2052 7261 3a67     [LOG] ERROR arg:
2006298c:	0d20 000a 0023 0000 4c5b 474f 205d 6e49      ...#...[LOG] In
2006299c:	6176 696c 2064 6567 2074 6170 6172 656d     valid get parame
200629ac:	6574 7372 0d2e 000a 253c 3e73 6c20 7665     ters....<%s> lev
200629bc:	6c65 6920 2073 6425 0a0d 0000 4c5b 474f     el is %d....[LOG
200629cc:	205d 6e49 6176 696c 2064 6573 2074 6170     ] Invalid set pa
200629dc:	6172 656d 6574 7372 0d2e 000a 4c5b 474f     rameters....[LOG
200629ec:	205d 6146 6c69 6465 7720 6568 206e 6573     ] Failed when se
200629fc:	2e74 0a0d 0000 0000 4c5b 474f 205d 6e49     t.......[LOG] In
20062a0c:	6176 696c 2064 6170 6172 656d 6574 2072     valid parameter 
20062a1c:	756e 626d 7265 0d2e 000a 0000 4c5b 474f     number......[LOG
20062a2c:	205d 6e49 6176 696c 2064 6f6d 6564 2520     ] Invalid mode %
20062a3c:	2e64 0a0d 0000 0000 4c5b 474f 205d 6554     d.......[LOG] Te
20062a4c:	7473 6120 776c 7961 2073 656c 6576 0d6c     st always level.
20062a5c:	000a 0000 4c5b 474f 205d 6554 7473 6520     ....[LOG] Test e
20062a6c:	7272 726f 6c20 7665 6c65 0a0d 0000 0000     rror level......
20062a7c:	4c5b 474f 205d 6554 7473 7720 7261 696e     [LOG] Test warni
20062a8c:	676e 6c20 7665 6c65 0a0d 0000 4c5b 474f     ng level....[LOG
20062a9c:	205d 6554 7473 6920 666e 206f 656c 6576     ] Test info leve
20062aac:	0d6c 000a 4c5b 474f 205d 6554 7473 4c20     l...[LOG] Test L
20062abc:	474f 495f 4554 534d 0a0d 0000 4c2b 474f     OG_ITEMS....+LOG
20062acc:	003a 0000 0a0d 7325 5245 4f52 3a52 6425     :.....%sERROR:%d
20062adc:	0a0d 0000 5441 4c2b 474f 3c3d 6567 5f74     ....AT+LOG=<get_
20062aec:	6573 3e74 3c2c 6f6d 7564 656c 5b3e 3c2c     set>,<module>[,<
20062afc:	6f6c 5f67 656c 6576 3e6c 0d5d 000a 0000     log_level>].....
20062b0c:	3c09 6567 5f74 6573 3e74 093a 2d30 6567     .<get_set>:.0-ge
20062b1c:	2c74 3120 732d 7465 202c 2d32 7270 6e69     t, 1-set, 2-prin
20062b2c:	2074 6c61 2c6c 3320 632d 656c 7261 6120     t all, 3-clear a
20062b3c:	6c6c 0a0d 0000 0000 3c09 6f6d 7564 656c     ll.......<module
20062b4c:	3a3e 2a09 652d 6361 2068 6f6d 7564 656c     >:.*-each module
20062b5c:	202c 746f 6568 7372 732d 6570 6963 6966     , others-specifi
20062b6c:	2063 6f6d 7564 656c 0a0d 0000 3c09 6f6c     c module.....<lo
20062b7c:	5f67 656c 6576 3e6c 093a 305b 352c 2c5d     g_level>:.[0,5],
20062b8c:	6f20 6c6e 2079 7061 6c70 6369 6261 656c      only applicable
20062b9c:	6620 726f 7320 7465 6d20 646f 0d65 000a      for set mode...
20062bac:	525b 4552 5d47 4520 7272 726f 7020 7261     [RREG] Error par
20062bbc:	6d61 7465 7265 0d73 000a 0000 522b 4552     ameters.....+RRE
20062bcc:	3a47 0000 575b 4552 5d47 4520 7272 726f     G:..[WREG] Error
20062bdc:	7020 7261 6d61 7465 7265 0d73 000a 0000      parameters.....
20062bec:	572b 4552 3a47 0000 756e 626d 7265 6f20     +WREG:..number o
20062bfc:	2066 6174 6b73 2073 203a 6425 6528 6378     f tasks : %d(exc
20062c0c:	6565 2064 4154 4b53 435f 544e 2129 5020     eed TASK_CNT)! P
20062c1c:	656c 7361 2065 6e65 616c 6772 2065 4154     lease enlarge TA
20062c2c:	4b53 435f 544e 0a0d 0000 0000 5043 2555     SK_CNT......CPU%
20062c3c:	0000 0000 7270 6f69 0000 0000 7473 7461     ....prio....stat
20062c4c:	0065 0000 6174 6b73 6e20 6d61 0065 0000     e...task name...
20062c5c:	7325 2509 0973 7325 2509 0a73 000d 0000     %s.%s.%s.%s.....
20062c6c:	7325 2509 0963 6425 2509 2e64 7325 0a0d     %s.%c.%d.%d.%s..
20062c7c:	0000 0000 7325 6d20 6c61 6f6c 2063 6166     ....%s malloc fa
20062c8c:	6c69 6465 0a0d 0000 745b 706f 555d 6173     iled....[top]Usa
20062c9c:	6567 203a 6f74 3d70 6f6d 6564 742c 6d69     ge: top=mode,tim
20062cac:	2c65 6f63 6e75 0a74 200d 6f6d 6564 203a     e,count.. mode: 
20062cbc:	2c30 7320 6174 7472 6320 756f 746e 6320     0, start count c
20062ccc:	7570 7520 6173 6567 6520 6576 7972 5b20     pu usage every [
20062cdc:	6974 656d 205d 6573 6f63 646e 0d2e 200a     time] second... 
20062cec:	6f6d 6564 203a 2c31 7320 6f74 2070 6f6d     mode: 1, stop mo
20062cfc:	6564 3020 0d2e 200a 6f6d 6564 203a 3a32     de 0... mode: 2:
20062d0c:	7320 6174 7472 6320 756f 746e 6320 7570      start count cpu
20062d1c:	7520 6173 6567 0d2e 200a 6f6d 6564 203a      usage... mode: 
20062d2c:	3a33 7320 6f74 2070 6f6d 6564 3220 0d2e     3: stop mode 2..
20062d3c:	200a 6974 656d 203a 5043 2055 7473 7461     . time: CPU stat
20062d4c:	7369 6974 7363 6920 746e 7265 6176 2e6c     istics interval.
20062d5c:	6544 6166 6c75 2074 2e31 7520 696e 2874     Default 1. unit(
20062d6c:	2973 0d20 200a 6f63 6e75 3a74 4320 5550     s) .. count: CPU
20062d7c:	7320 6174 6974 7473 6369 2073 6f63 6e75      statistics coun
20062d8c:	2c74 6420 6665 7561 746c 7520 746e 6c69     t, default until
20062d9c:	7320 6f74 2070 726f 3120 322c 332c 2e2e      stop or 1,2,3..
20062dac:	002e 0000 654d 6f6d 7972 6620 6961 756c     ....Memory failu
20062dbc:	6572 6620 726f 7420 7361 5f6b 7473 7461     re for task_stat
20062dcc:	7375 0a0d 0000 0000 7563 7272 6e65 2074     us......current 
20062ddc:	6f6d 6564 203a 6425 7420 6d69 3a65 2520     mode: %d time: %
20062dec:	2864 2973 6320 756f 746e 203a 6425 2820     d(s) count: %d (
20062dfc:	312d 6620 726f 6920 666e 6e69 7469 2965     -1 for infinite)
20062e0c:	0a0d 0000 7063 5f75 7473 7461 745f 7268     ....cpu_stat_thr
20062e1c:	6165 0064 432b 5550 4f4c 4441 003a 0000     ead.+CPULOAD:...
20062e2c:	5441 432b 5550 4f4c 4441 3c3d 6f6d 6564     AT+CPULOAD=<mode
20062e3c:	5b3e 3c2c 6974 656d 695f 746e 6176 3e6c     >[,<time_intval>
20062e4c:	3c2c 6f63 6e75 6574 3e72 0d5d 000a 0000     ,<counter>].....
20062e5c:	3c09 6f6d 6564 3a3e 5b09 2c30 5d32 0a0d     .<mode>:.[0,2]..
20062e6c:	0000 0000 3c09 6974 656d 695f 746e 6176     .....<time_intva
20062e7c:	3e6c 093a 6e49 7320 6365 0a0d 0000 0000     l>:.In sec......
20062e8c:	3c09 6f63 6e75 6574 3e72 3009 6d20 6165     .<counter>.0 mea
20062e9c:	736e 6920 666e 6e69 7469 0a0d 0000 0000     ns infinit......
20062eac:	5441 7325 0a0d 0000 6f43 6d6d 6e6f 4120     AT%s....Common A
20062ebc:	2054 6f43 6d6d 6e61 3a64 0a0d 0000 0000     T Command:......
20062ecc:	4c2b 5349 3a54 0000 4f2b 4154 4c43 4145     +LIST:..+OTACLEA
20062edc:	0052 0000 4f2b 4154 4552 4f43 4556 0052     R...+OTARECOVER.
20062eec:	432b 5550 4f4c 4441 0000 0000 542b 5345     +CPULOAD....+TES
20062efc:	0054 0000 4c2b 5349 0054 0000 522b 5453     T...+LIST...+RST
20062f0c:	0000 0000 532b 4154 4554 0000 472b 524d     ....+STATE..+GMR
20062f1c:	0000 0000 4c2b 474f 0000 0000 522b 4552     ....+LOG....+RRE
20062f2c:	0047 0000 572b 4552 0047 0000               G...+WREG...

20062f38 <__FUNCTION__.0>:
20062f38:	7063 5f75 7473 7461 745f 7268 6165 0064     cpu_stat_thread.
20062f48:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20062f58:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20062f68:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20062f78:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20062f88:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20062f98:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20062fa8:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20062fb8:	6f70 656e 746e 6f2f 2f73 7266 6565 7472     ponent/os/freert
20062fc8:	736f 662f 6572 7265 6f74 5f73 6568 7061     os/freertos_heap
20062fd8:	5f35 6f63 666e 6769 632e 0000 255b 5d73     5_config.c..[%s]
20062fe8:	6c20 7665 6c65 3d20 2520 0a64 0000 0000      level = %d.....
20062ff8:	682f 6d6f 2f65 7561 6f74 6a5f 6e65 696b     /home/auto_jenki
20063008:	736e 775f 616c 356e 492f 544f 415f 5354     ns_wlan5/IOT_ATS
20063018:	722f 7065 736f 722f 6c65 6165 6573 6d2d     /repos/release-m
20063028:	7361 6574 2f72 6f73 7275 6563 612f 656d     aster/source/ame
20063038:	6162 696c 6574 675f 6363 705f 6f72 656a     balite_gcc_proje
20063048:	7463 702f 6f72 656a 7463 6b5f 3472 762f     ct/project_kr4/v
20063058:	6473 2f6b 2e2e 2e2f 2f2e 2e2e 632f 6d6f     sdk/../../../com
20063068:	6f70 656e 746e 732f 636f 612f 656d 6162     ponent/soc/ameba
20063078:	696c 6574 732f 6c77 6269 6c2f 676f 632e     lite/swlib/log.c
20063088:	0000 0000 002a 0000 6143 6863 2065 7261     ....*...Cache ar
20063098:	6172 2079 7369 6620 6c75 2c6c 6120 646e     ray is full, and
200630a8:	7220 7065 616c 6563 6f20 646c 6520 746e      replace old ent
200630b8:	7972 000a 255b 3830 5d78 0020 0a0d 255b     ry..[%08x] ...[%
200630c8:	3830 5d78 0020 0000 3025 7838 0020 0000     08x] ...%08x ...
200630d8:	3025 7832 0020 0000 255b 2d73 6325 205d     %02x ...[%s-%c] 
200630e8:	0000 0000 5749 4744 7220 6665 6572 6873     ....IWDG refresh
200630f8:	6f20 216e 000a 0000 414d 4e49 0000 0000      on!....MAIN....
20063108:	4457 5f47 6954 656d 0072 0000 5749 4744     WDG_Timer...IWDG
20063118:	7220 6665 6572 6873 6520 7272 726f 000a      refresh error..
20063128:	524b 2034 534f 5320 4154 5452 0a20 0000     KR4 OS START ...
20063138:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
20063148:	202d 4554 5453 4320 4d4f 414d 444e 4d20     - TEST COMMAND M
20063158:	444f 2045 4548 504c 2520 2d78 2d2d 2d2d     ODE HELP %x-----
20063168:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 0a2d 0000     -------------...
20063178:	7325 000a 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     %s..------------
20063188:	2d2d 2d2d 202d 4554 5453 4320 4d4f 414d     ----- TEST COMMA
20063198:	444e 4d20 444f 2045 4e45 2044 2520 2d78     ND MODE END  %x-
200631a8:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
200631b8:	0a2d 0000 6552 6f62 746f 6e69 2067 2e2e     -...Rebooting ..
200631c8:	0a2e 000d 6175 7472 7562 6e72 0000 0000     ....uartburn....
200631d8:	255b 3830 5d78 2520 3830 2078 000a 0000     [%08x] %08x ....
200631e8:	7257 6e6f 2067 7261 7567 656d 746e 6e20     Wrong argument n
200631f8:	6d75 6562 2172 0a0d 0000 0000 5352 5049     umber!......RSIP
20063208:	6520 616e 6c62 6465 202c 6143 276e 2074      enabled, Can't 
20063218:	6552 6461 4620 616c 6873 0a2e 0000 0000     Read Flash......
20063228:	7257 6e6f 2067 6d63 2164 0a0d 0000 0000     Wrong cmd!......
20063238:	003f 0000 4809 4c45 2050 7e28 293f 0a20     ?....HELP (~?) .
20063248:	0909 5020 6972 746e 7420 6968 2073 6568     .. Print this he
20063258:	706c 6d20 7365 6173 0a67 0000 5744 0000     lp messag...DW..
20063268:	4409 2057 413c 6464 6572 7373 202c 654c     .DW <Address, Le
20063278:	676e 6874 0a3e 0909 4420 6d75 2070 656d     ngth>... Dump me
20063288:	6f6d 7972 7720 726f 2064 726f 5220 6165     mory word or Rea
20063298:	2064 7748 7720 726f 2064 6572 6967 7473     d Hw word regist
200632a8:	7265 203b 090a 2009 5744 3c20 6461 7264     er; ... DW <addr
200632b8:	203e 0909 4420 6d75 2070 6e6f 796c 6f20     > .. Dump only o
200632c8:	656e 7720 726f 2064 7461 7420 6568 7320     ne word at the s
200632d8:	6d61 2065 6974 656d 202c 6e75 7469 203a     ame time, unit: 
200632e8:	4234 7479 7365 090a 2009 5744 3c20 6461     4Bytes... DW <ad
200632f8:	7264 203e 6c3c 6e65 093e 4420 6d75 2070     dr> <len>. Dump 
20063308:	6874 2065 7073 6365 6669 6569 2064 656c     the specified le
20063318:	676e 6874 6f20 2066 6874 2065 6f77 6472     ngth of the word
20063328:	202c 6e75 7469 203a 4234 7479 7365 090a     , unit: 4Bytes..
20063338:	2009 5744 3c20 6461 7264 203e 6c3c 6e65     . DW <addr> <len
20063348:	203e 2062 4420 6d75 2070 6874 2065 7073     > b  Dump the sp
20063358:	6365 6669 6569 2064 656c 676e 6874 6f20     ecified length o
20063368:	2066 6874 2065 7962 6574 202c 6e75 7469     f the byte, unit
20063378:	203a 4231 7479 0a65 0000 0000 5745 0000     : 1Byte.....EW..
20063388:	4509 2057 413c 6464 6572 7373 202c 6548     .EW <Address, He
20063398:	3e78 090a 2009 7257 7469 2065 656d 6f6d     x>... Write memo
200633a8:	7972 6420 6f77 6472 6f20 2072 7257 7469     ry dword or Writ
200633b8:	2065 7748 6420 6f77 6472 7220 6765 7369     e Hw dword regis
200633c8:	6574 2072 090a 2009 6143 206e 7277 7469     ter ... Can writ
200633d8:	2065 6e6f 796c 6f20 656e 6420 6f77 6472     e only one dword
200633e8:	6120 2074 6874 2065 6173 656d 7420 6d69      at the same tim
200633f8:	2065 090a 2009 7845 203a 5745 4120 6464     e ... Ex: EW Add
20063408:	6572 7373 5620 6c61 6575 0a20 0000 0000     ress Value .....
20063418:	4552 4f42 544f 0000 5209 4245 4f4f 2054     REBOOT...REBOOT 
20063428:	090a 2009 693c 6574 2c6d 7320 7274 6e69     ... <item, strin
20063438:	3e67 3a20 0a20 0909 6920 6574 3a6d 7520     g> : ... item: u
20063448:	7261 6274 7275 206e 726f 4e20 412f 0a20     artburn or N/A .
20063458:	0909 0a20 0000 0000 255b 5d73 2520 2073     .. .....[%s] %s 
20063468:	3c3c 203c 6854 2065 756d 6574 2078 6168     <<< The mutex ha
20063478:	2073 6f6e 2074 6562 6e65 7220 6c65 6165     s not been relea
20063488:	6573 2c64 6220 7475 7420 6568 6d20 7475     sed, but the mut
20063498:	7865 6820 7361 6220 6565 206e 6564 656c     ex has been dele
200634a8:	6574 2e64 3e20 3e3e 000a 0000               ted. >>>....

200634b4 <__FUNCTION__.0>:
200634b4:	7472 736f 6d5f 7475 7865 645f 6c65 7465     rtos_mutex_delet
200634c4:	0065 0000 6f4e 7354 006b 0000 524b 0034     e...NoTsk...KR4.
200634d4:	614d 6c6c 636f 6620 6961 656c 2e64 4320     Malloc failed. C
200634e4:	726f 3a65 255b 5d73 202c 6154 6b73 5b3a     ore:[%s], Task:[
200634f4:	7325 2c5d 5b20 7266 6565 6820 6165 2070     %s], [free heap 
20063504:	6973 657a 203a 6425 0d5d 000a 0d0a 255b     size: %d].....[%
20063514:	5d73 5320 4154 4b43 4f20 4556 4652 4f4c     s] STACK OVERFLO
20063524:	2057 202d 6154 6b73 614e 656d 2528 2973     W - TaskName(%s)
20063534:	0d0a 0000                                   ....

20063538 <__FUNCTION__.5>:
20063538:	4176 7070 696c 6163 6974 6e6f 7453 6361     vApplicationStac
20063548:	4f6b 6576 6672 6f6c 4877 6f6f 006b 0000     kOverflowHook...
20063558:	5441 3f53 0000 0000 0a0d 6e75 6e6b 776f     ATS?......unknow
20063568:	206e 6f63 6d6d 6e61 2064 2527 2773 0000     n command '%s'..
20063578:	4f4c 5547 5241 5f54 4154 4b53 0000 0000     LOGUART_TASK....
20063588:	7243 6165 6574 4c20 676f 5520 5241 2054     Create Log UART 
20063598:	6154 6b73 4520 7272 2121 000a 4853 4c45     Task Err!!..SHEL
200635a8:	004c 0000 255b 5d73 4920 474d 4449 203a     L...[%s] IMGID: 
200635b8:	6425 202c 7563 7272 6e65 2074 544f 2541     %d, current OTA%
200635c8:	2064 6441 7264 7365 3a73 3020 2578 3830     d Address: 0x%08
200635d8:	2c78 7420 7261 6567 2074 544f 2541 2064     x, target OTA%d 
200635e8:	6441 7264 7365 3a73 3020 2578 3830 0a78     Address: 0x%08x.
200635f8:	0000 0000 5953 0053 255b 5d73 4920 474d     ....SYS.[%s] IMG
20063608:	4449 203a 6425 202c 7563 7272 6e65 2074     ID: %d, current 
20063618:	6966 6d72 6177 6572 6920 2073 544f 2541     firmware is OTA%
20063628:	2c64 7420 7261 6567 2074 6966 6d72 6177     d, target firmwa
20063638:	6572 4f20 4154 6425 6920 2073 6e69 6176     re OTA%d is inva
20063648:	696c 0a64 0000 0000 255b 5d73 6220 6361     lid.....[%s] bac
20063658:	756b 2070 616d 6c6c 636f 6620 6961 646c     kup malloc faild
20063668:	6465 000a                                   ed..

2006366c <__func__.0>:
2006366c:	7973 5f73 6572 6f63 6576 5f72 746f 5f61     sys_recover_ota_
2006367c:	6973 6e67 7461 7275 0065 0000               signature...

20063688 <__func__.1>:
20063688:	7973 5f73 6c63 6165 5f72 746f 5f61 6973     sys_clear_ota_si
20063698:	6e67 7461 7275 0065 4449 454c 0000 0000     gnature.IDLE....
200636a8:	2509 0963 6425 2509 0964 6425 0a0d 0000     .%c.%d.%d.%d....
200636b8:	6d54 2072 7653 0063 e8b0 2005 e8b0 2005     Tmr Svc.... ... 
200636c8:	e8b0 2005 e8ec 2005 e8f8 2005 e914 2005     ... ... ... ... 
200636d8:	e8b0 2005 e8b0 2005 e8ec 2005 e8f8 2005     ... ... ... ... 
200636e8:	6e69 6176 696c 2064 7665 3a74 2520 2064     invalid evt: %d 
200636f8:	000a 0000 5041 6320 6e6f 7573 656d 6820     ....AP consume h
20063708:	6165 2070 6425 000a 4c57 4e41 0000 0000     eap %d..WLAN....
20063718:	7325 2528 2964 202c 7641 6961 616c 6c62     %s(%d), Availabl
20063728:	2065 6568 7061 2520 0a64 0000 6e69 7469     e heap %d...init
20063738:	0000 0000 6c77 6e61 695f 696e 6974 6c61     ....wlan_initial
20063748:	7a69 2065 6166 6c69 6465 000a               ize failed..

20063754 <__FUNCTION__.0>:
20063754:	695f 696e 5f74 6874 6572 6461 0000 0000     _init_thread....
20063764:	6f46 2072 6564 7562 2067 6170 6b63 6761     For debug packag
20063774:	2e65 5020 656c 7361 2065 6573 656c 7463     e. Please select
20063784:	7020 7273 6d61 7420 7079 2065 6361 6f63      psram type acco
20063794:	6472 6e69 2067 6f74 7920 756f 2072 6863     rding to your ch
200637a4:	7069 000a 4843 5049 4e49 4f46 0000 0000     ip..CHIPINFO....
200637b4:	3230 6620 726f 5720 2042 3233 624d 300a     02 for WB 32Mb.0
200637c4:	2034 6f66 2072 4257 3120 3832 624d 300a     4 for WB 128Mb.0
200637d4:	2033 6f66 2072 4257 3220 3635 624d 0a0a     3 for WB 256Mb..
200637e4:	0000 0000 682f 6d6f 2f65 7561 6f74 6a5f     ..../home/auto_j
200637f4:	6e65 696b 736e 775f 616c 356e 492f 544f     enkins_wlan5/IOT
20063804:	415f 5354 722f 7065 736f 722f 6c65 6165     _ATS/repos/relea
20063814:	6573 6d2d 7361 6574 2f72 6f73 7275 6563     se-master/source
20063824:	612f 656d 6162 696c 6574 675f 6363 705f     /amebalite_gcc_p
20063834:	6f72 656a 7463 702f 6f72 656a 7463 6b5f     roject/project_k
20063844:	3472 762f 6473 2f6b 2e2e 2e2f 2f2e 2e2e     r4/vsdk/../../..
20063854:	632f 6d6f 6f70 656e 746e 732f 636f 612f     /component/soc/a
20063864:	656d 6162 696c 6574 6c2f 6269 722f 6d61     mebalite/lib/ram
20063874:	635f 6d6f 6f6d 2f6e 6d61 6265 5f61 6863     _common/ameba_ch
20063884:	7069 6e69 6f66 6c5f 6269 632e 0000 0000     ipinfo_lib.c....
20063894:	7325 203a 0000 0000 5245 3a52 4720 7465     %s: ....ERR: Get
200638a4:	5320 636f 4e20 6d61 2065 6146 6c69 000a      Soc Name Fail..
200638b4:	3025 7832 0000 0000 4d4b 0034 7325 4c20     %02x....KM4.%s L
200638c4:	6269 5620 7265 6973 6e6f 0a3a 0000 0000     ib Version:.....
200638d4:	3032 3432 312f 2f30 3131 312d 3a35 3635     2024/10/11-15:56
200638e4:	313a 0035 4f43 504d 4c49 2045 4954 454d     :15.COMPILE TIME
200638f4:	203a 7325 000a 0000 6367 2063 6576 7372     : %s....gcc vers
20063904:	6f69 206e 3031 332e 312e 3220 3230 3032     ion 10.3.1 20220
20063914:	3335 2030 5228 6165 746c 6b65 5620 4453     530 (Realtek VSD
20063924:	2d4b 3031 332e 312e 4220 6975 646c 3420     K-10.3.1 Build 4
20063934:	3730 2b33 2029 0000 4f43 504d 4c49 5245     073+) ..COMPILER
20063944:	203a 7325 000a 0000 6325 0000 2d2d 2d2d     : %s....%c..----
20063954:	2d2d 2d2d 2d2d 2d2d 6154 6b73 4920 666e     --------Task Inf
20063964:	2d6f 2d2d 2d2d 2d2d 2d2d 2d2d 0d2d 000a     o------------...
20063974:	6146 6c75 2074 6e6f 7420 7361 206b 253c     Fault on task <%
20063984:	3e73 000a 6154 6b73 4920 3a44 2520 0d64     s>..Task ID: %d.
20063994:	000a 0000 6154 6b73 5420 4243 303a 2578     ....Task TCB:0x%
200639a4:	3830 786c 0a0d 0000 7543 7272 6e65 2074     08lx....Current 
200639b4:	7453 7461 3a65 2520 2064 2528 2973 0a0d     State: %d (%s)..
200639c4:	0000 0000 6142 6573 5020 6972 726f 7469     ....Base Priorit
200639d4:	3a79 2520 0d64 000a 7543 7272 6e65 2074     y: %d...Current 
200639e4:	7250 6f69 6972 7974 203a 6425 0a0d 0000     Priority: %d....
200639f4:	7552 206e 6954 656d 4320 756f 746e 7265     Run Time Counter
20063a04:	203a 6c25 0d75 000a 7453 6361 546b 706f     : %lu...StackTop
20063a14:	203a 7830 3025 6c38 2c78 5320 6174 6b63     : 0x%08lx, Stack
20063a24:	6142 6573 203a 7830 3025 6c38 2c78 5320     Base: 0x%08lx, S
20063a34:	6174 6b63 6e45 3a64 3020 2578 3830 786c     tackEnd: 0x%08lx
20063a44:	202c 7453 6361 536b 7a69 3d65 6c25 2875     , StackSize=%lu(
20063a54:	6f77 6472 0d29 000a 7453 6361 206b 6948     word)...Stack Hi
20063a64:	6867 5720 7461 7265 614d 6b72 203a 6425     gh WaterMark: %d
20063a74:	7728 726f 2964 0a0d 0000 0000 7552 6e6e     (word)......Runn
20063a84:	6e69 0067 6552 6461 2079 202f 6550 646e     ing.Ready / Pend
20063a94:	6e69 0067 6c42 636f 656b 0064 7553 7073     ing.Blocked.Susp
20063aa4:	6e65 6564 0064 0000 6544 656c 6574 0064     ended...Deleted.
20063ab4:	6e49 6176 696c 0064                         Invalid.

20063abc <TaskStateString.0>:
20063abc:	3a80 2006 3a88 2006 3a98 2006 3aa0 2006     .:. .:. .:. .:. 
20063acc:	3aac 2006 3ab4 2006 f27c 2005 f190 2005     .:. .:. |.. ... 
20063adc:	f190 2005 f190 2005 f190 2005 f190 2005     ... ... ... ... 
20063aec:	f190 2005 f190 2005 f190 2005 f190 2005     ... ... ... ... 
20063afc:	f1c6 2005 f338 2005 f206 2005 f190 2005     ... 8.. ... ... 
20063b0c:	f190 2005 f190 2005 f190 2005 f206 2005     ... ... ... ... 
20063b1c:	f190 2005 f190 2005 f38c 2005 f190 2005     ... ... ... ... 
20063b2c:	f190 2005 f1ec 2005 f2a2 2005 f190 2005     ... ... ... ... 
20063b3c:	f190 2005 f2c0 2005 f190 2005 f248 2005     ... ... ... H.. 
20063b4c:	f190 2005 f190 2005 f27e 2005 f190 2005     ... ... ~.. ... 
20063b5c:	f38e 2005 3130 3332 3534 3736 3938 4241     ... 0123456789AB
20063b6c:	4443 4645 0000 0000 6f69 6420 6972 6576     CDEF....io drive
20063b7c:	2072 6170 6172 656d 6574 7372 6520 7272     r parameters err
20063b8c:	726f 2021 6966 656c 6e5f 6d61 3a65 2520     or! file_name: %
20063b9c:	2c73 6c20 6e69 3a65 2520 0064 682f 6d6f     s, line: %d./hom
20063bac:	2f65 7561 6f74 6a5f 6e65 696b 736e 775f     e/auto_jenkins_w
20063bbc:	616c 356e 492f 544f 415f 5354 722f 7065     lan5/IOT_ATS/rep
20063bcc:	736f 722f 6c65 6165 6573 6d2d 7361 6574     os/release-maste
20063bdc:	2f72 6f73 7275 6563 612f 656d 6162 696c     r/source/amebali
20063bec:	6574 675f 6363 705f 6f72 656a 7463 702f     te_gcc_project/p
20063bfc:	6f72 656a 7463 6b5f 3472 762f 6473 2f6b     roject_kr4/vsdk/
20063c0c:	2e2e 2e2f 2f2e 2e2e 632f 6d6f 6f70 656e     ../../../compone
20063c1c:	746e 732f 636f 612f 656d 6162 696c 6574     nt/soc/amebalite
20063c2c:	6c2f 6269 722f 6d61 635f 6d6f 6f6d 2f6e     /lib/ram_common/
20063c3c:	6d61 6265 5f61 6f72 5f6d 6170 6374 2e68     ameba_rom_patch.
20063c4c:	0063 0000 7269 5b71 6425 205d 7270 6f69     c...irq[%d] prio
20063c5c:	6972 7974 2520 2078 6873 6c61 206c 3d3c     rity %x shall <=
20063c6c:	2520 0a64 0000 0000 5b1b 3b31 3333 5b6d      %d......[1;33m[
20063c7c:	5245 4f52 5d52 5b20 7325 3a3a 6425 205d     ERROR] [%s::%d] 
20063c8c:	7473 6361 5f6b 6973 657a 2528 2964 6920     stack_size(%d) i
20063c9c:	2073 6f70 6973 6974 6576 3f20 0a3f 5b1b     s positive ??..[
20063cac:	6d30 0000 5b1b 3b31 3333 5b6d 5245 4f52     0m...[1;33m[ERRO
20063cbc:	5d52 5b20 7325 3a3a 6425 205d 6172 6f5f     R] [%s::%d] ra_o
20063ccc:	6666 6573 2874 6425 2029 7369 6e20 6765     ffset(%d) is neg
20063cdc:	7461 7669 3f65 1b3f 305b 006d 5b1b 3b31     ative??.[0m..[1;
20063cec:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063cfc:	6425 205d 7453 7261 2074 7473 6361 206b     %d] Start stack 
20063d0c:	6162 6b63 7274 6361 6e69 2067 6f66 2072     backtracing for 
20063d1c:	7073 2520 2c70 7020 2063 7025 1b0a 305b     sp %p, pc %p..[0
20063d2c:	006d 0000 5b1b 3b31 3333 5b6d 5254 4341     m....[1;33m[TRAC
20063d3c:	5d45 5b20 7325 3a3a 6425 205d 665b 6172     E] [%s::%d] [fra
20063d4c:	656d 2320 6425 205d 7073 3e2d 2520 2c70     me #%d] sp-> %p,
20063d5c:	7020 2d63 203e 7025 202c 7473 6361 5f6b      pc-> %p, stack_
20063d6c:	6973 657a 3e2d 2520 2c64 7220 2d61 203e     size-> %d, ra-> 
20063d7c:	7025 1b0a 305b 006d 5b1b 3b31 3333 5b6d     %p..[0m..[1;33m[
20063d8c:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063d9c:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
20063dac:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
20063dbc:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
20063dcc:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063ddc:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
20063dec:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
20063dfc:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
20063e0c:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
20063e1c:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
20063e2c:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
20063e3c:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
20063e4c:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
20063e5c:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
20063e6c:	1b0a 305b 006d 0000                         ..[0m...

20063e74 <__FUNCTION__.3>:
20063e74:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063e84:	6932 0000                                   2i..

20063e88 <__FUNCTION__.2>:
20063e88:	6e75 6977 646e 665f 6172 656d 725f 3376     unwind_frame_rv3
20063e98:	6332 0000                                   2c..

20063e9c <__FUNCTION__.1>:
20063e9c:	6f64 735f 6174 6b63 625f 6361 746b 6172     do_stack_backtra
20063eac:	6563 0000 6e55 6e6b 776f 206e 7865 6563     ce..Unknown exce
20063ebc:	7470 6f69 006e 0000 5b1b 3b31 3333 5b6d     ption....[1;33m[
20063ecc:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063edc:	3d3d 3d3d 3d3d 3d3d 3d3d 4320 6172 6873     ========== Crash
20063eec:	4420 6d75 2070 3d3d 3d3d 3d3d 3d3d 3d3d      Dump ==========
20063efc:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
20063f0c:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063f1c:	7845 6563 7470 6f69 206e 6163 6775 7468     Exception caught
20063f2c:	6f20 206e 7025 7720 7469 2068 6572 7361      on %p with reas
20063f3c:	6e6f 5b20 7830 7825 205d 3e2d 5b20 7325     on [0x%x] -> [%s
20063f4c:	0a5d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     ]..[0m...[1;33m[
20063f5c:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
20063f6c:	3d3d 3d3d 3d3d 3d3d 3d3d 5220 6765 7369     ========== Regis
20063f7c:	6574 2072 7544 706d 3d20 3d3d 3d3d 3d3d     ter Dump =======
20063f8c:	3d3d 0a3d 5b1b 6d30 0000 0000 5b1b 3b31     ===..[0m.....[1;
20063f9c:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063fac:	6425 205d 6d5b 6373 6172 6374 5d68 3020     %d] [mscratch] 0
20063fbc:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063fcc:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
20063fdc:	6425 205d 6d5b 7065 5d63 2020 2020 3020     %d] [mepc]     0
20063fec:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
20063ffc:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
2006400c:	6425 205d 6d5b 6163 7375 5d65 2020 3020     %d] [mcause]   0
2006401c:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
2006402c:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
2006403c:	6425 205d 6d5b 7674 6c61 205d 2020 3020     %d] [mtval]    0
2006404c:	2578 3830 0a78 5b1b 6d30 0000 5b1b 3b31     x%08x..[0m...[1;
2006405c:	3333 5b6d 5254 4341 5d45 5b20 7325 3a3a     33m[TRACE] [%s::
2006406c:	6425 205d 7845 6563 7470 6f69 206e 6163     %d] Exception ca
2006407c:	7375 2065 6f63 6c75 2064 6562 0d3a 1b0a     use could be:...
2006408c:	305b 006d 5b1b 3b31 3333 5b6d 5254 4341     [0m..[1;33m[TRAC
2006409c:	5d45 5b20 7325 3a3a 6425 205d 2509 2c73     E] [%s::%d] .%s,
200640ac:	2520 2073 203d 7830 7825 0a0d 5b1b 6d30      %s = 0x%x...[0m
200640bc:	0000 0000 5b1b 3b31 3333 5b6d 5254 4341     .....[1;33m[TRAC
200640cc:	5d45 5b20 7325 3a3a 6425 205d 2509 0d73     E] [%s::%d] .%s.
200640dc:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
200640ec:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
200640fc:	785b 6425 2d20 203e 7325 205d 7830 3025     [x%d -> %s] 0x%0
2006410c:	7838 1b0a 305b 006d 5b1b 3b31 3333 5b6d     8x..[0m..[1;33m[
2006411c:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
2006412c:	3d3d 3d3d 3d3d 3d3d 3d3d 5320 6174 6b63     ========== Stack
2006413c:	5420 6172 6563 3d20 3d3d 3d3d 3d3d 3d3d      Trace =========
2006414c:	0a3d 5b1b 6d30 0000 5b1b 3b31 3333 5b6d     =..[0m...[1;33m[
2006415c:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
2006416c:	7349 7420 6968 2073 656c 6661 6620 6e75     Is this leaf fun
2006417c:	7463 6f69 3f6e 5520 6973 676e 5220 2041     ction? Using RA 
2006418c:	7361 5020 2043 6f74 6420 206f 6162 6b63     as PC to do back
2006419c:	7274 6361 2c65 7320 6174 6b63 735f 7a69     trace, stack_siz
200641ac:	2865 6425 252f 2964 1b0a 305b 006d 0000     e(%d/%d)..[0m...
200641bc:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
200641cc:	7325 3a3a 6425 205d 3d3d 3d3d 3d3d 3d3d     %s::%d] ========
200641dc:	3d3d 4520 646e 6f20 2066 7453 6361 206b     == End of Stack 
200641ec:	7254 6361 2065 3d3d 3d3d 3d3d 3d3d 3d3d     Trace ==========
200641fc:	1b0a 305b 006d 0000 5b1b 3b31 3333 5b6d     ..[0m....[1;33m[
2006420c:	5254 4341 5d45 5b20 7325 3a3a 6425 205d     TRACE] [%s::%d] 
2006421c:	3d3d 3d3d 3d3d 3d3d 3d3d 4520 646e 6f20     ========== End o
2006422c:	2066 7243 7361 2068 7544 706d 3d20 3d3d     f Crash Dump ===
2006423c:	3d3d 3d3d 3d3d 0a3d 5b1b 6d30 0000 0000     =======..[0m....
2006424c:	5b1b 3b31 3333 5b6d 5254 4341 5d45 5b20     .[1;33m[TRACE] [
2006425c:	7325 3a3a 6425 205d 0a0d 0a0d 5b1b 6d30     %s::%d] .....[0m
2006426c:	0000 0000 6f53 6572 412f 4f4d 7020 6761     ....Sore/AMO pag
2006427c:	2065 6177 6b6c 6320 6568 6b63 6620 6961     e walk check fai
2006428c:	006c 0000 7453 726f 2065 6461 7264 7365     l...Store addres
2006429c:	0073 0000 6f4c 6461 7020 6761 2065 6177     s...Load page wa
200642ac:	6b6c 6320 6568 6b63 6620 6961 006c 0000     lk check fail...
200642bc:	6f4c 6461 6120 6464 6572 7373 0000 0000     Load address....
200642cc:	6e49 7473 7572 7463 6f69 206e 6170 6567     Instruction page
200642dc:	7720 6c61 206b 6863 6365 206b 6166 6c69      walk check fail
200642ec:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
200642fc:	6461 7264 7365 0073 7253 726f 2f65 4d41     address.Srore/AM
2006430c:	204f 4d50 2050 6863 6365 206b 6166 6c69     O PMP check fail
2006431c:	0000 0000 7453 726f 2f65 4d41 204f 6170     ....Store/AMO pa
2006432c:	6567 7720 6c61 206b 7562 2073 7265 6f72     ge walk bus erro
2006433c:	0072 0000 7453 726f 2065 7562 2073 7265     r...Store bus er
2006434c:	6f72 0072 7453 726f 2f65 4d41 204f 6461     ror.Store/AMO ad
2006435c:	7264 7365 2073 696d 6173 696c 6e67 6465     dress misaligned
2006436c:	0000 0000 6f4c 6461 5020 504d 6320 6568     ....Load PMP che
2006437c:	6b63 6620 6961 006c 6f4c 6461 5020 6761     ck fail.Load Pag
2006438c:	2065 6177 6b6c 6220 7375 6520 7272 726f     e walk bus error
2006439c:	0000 0000 6f4c 6461 6220 7375 6520 7272     ....Load bus err
200643ac:	726f 0000 6f4c 6461 6120 6464 6572 7373     or..Load address
200643bc:	6d20 7369 6c61 6769 656e 0064 6c49 656c      misaligned.Ille
200643cc:	6167 206c 6e69 7473 7572 7463 6f69 006e     gal instruction.
200643dc:	6e49 7473 7572 7463 6f69 206e 706f 6f63     Instruction opco
200643ec:	6564 0000 6e49 7473 7572 7463 6f69 206e     de..Instruction 
200643fc:	4d50 2050 6863 6365 206b 6166 6c69 0000     PMP check fail..
2006440c:	6e49 7473 7572 7463 6f69 206e 6150 6567     Instruction Page
2006441c:	7720 6c61 206b 7562 2073 7265 6f72 0072      walk bus error.
2006442c:	6e49 7473 7572 7463 6f69 206e 7562 2073     Instruction bus 
2006443c:	7265 6f72 0072 0000 7242 6e61 6863 7420     error...Branch t
2006444c:	7261 6567 2074 696d 6173 696c 6e67 6465     arget misaligned
2006445c:	0000 0000 6154 6772 7465 6120 6464 6572     ....Target addre
2006446c:	7373 0000 6e49 7473 7572 7463 6f69 206e     ss..Instruction 
2006447c:	6461 7264 7365 2073 696d 6173 696c 6e67     address misalign
2006448c:	6465 0000 657a 6f72 0000 0000 6172 0000     ed..zero....ra..
2006449c:	7073 0000 7067 0000 7074 0000 3074 0000     sp..gp..tp..t0..
200644ac:	3174 0000 3274 0000 3073 662f 0070 0000     t1..t2..s0/fp...
200644bc:	3173 0000 3061 0000 3161 0000 3261 0000     s1..a0..a1..a2..
200644cc:	3361 0000 3461 0000 3561 0000 3661 0000     a3..a4..a5..a6..
200644dc:	3761 0000 3273 0000 3373 0000 3473 0000     a7..s2..s3..s4..
200644ec:	3573 0000 3673 0000 3773 0000 3873 0000     s5..s6..s7..s8..
200644fc:	3973 0000 3173 0030 3173 0031 3374 0000     s9..s10.s11.t3..
2006450c:	3474 0000 3574 0000 3674 0000 6e49 7473     t4..t5..t6..Inst
2006451c:	7572 7463 6f69 206e 6361 6563 7373 6620     ruction access f
2006452c:	7561 746c 0000 0000 7242 6165 706b 696f     ault....Breakpoi
2006453c:	746e 0000 6f4c 6461 6120 6464 7365 2073     nt..Load addess 
2006454c:	696d 6173 696c 6e67 6465 0000 6f4c 6461     misaligned..Load
2006455c:	6120 6363 7365 2073 6166 6c75 0074 0000      access fault...
2006456c:	7453 726f 2065 6461 7264 7365 2073 696d     Store address mi
2006457c:	6173 696c 6e67 6465 0000 0000 7453 726f     saligned....Stor
2006458c:	2065 6461 7264 7365 2073 6166 6c75 0074     e address fault.
2006459c:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
200645ac:	6620 6f72 206d 2055 6f6d 6564 0000 0000      from U mode....
200645bc:	6e45 6976 6f72 6d6e 6e65 2074 6163 6c6c     Environment call
200645cc:	6620 6f72 206d 2053 6f6d 6564 0000 0000      from S mode....
200645dc:	6552 6573 7672 6465 6520 6378 7065 6974     Reserved excepti
200645ec:	6e6f 0000 6e45 6976 6f72 6d6e 6e65 2074     on..Environment 
200645fc:	6163 6c6c 6620 6f72 206d 204d 6f6d 6564     call from M mode
2006460c:	0000 0000 6e49 7473 7572 7463 6f69 206e     ....Instruction 
2006461c:	6170 6567 6620 7561 746c 0000 6f4c 6461     page fault..Load
2006462c:	7020 6761 2065 6166 6c75 0074 7453 726f      page fault.Stor
2006463c:	2065 6170 6567 6620 7561 746c 0000 0000     e page fault....

2006464c <__FUNCTION__.0>:
2006464c:	7263 7361 5f68 7564 706d 0000               crash_dump..

20064658 <exception_cause_0_string>:
20064658:	4444 2006 4460 2006 4470 2006 42f0 2006     DD. `D. pD. .B. 
	...

20064670 <exception_cause_12_string>:
20064670:	42cc 2006 42f0 2006 0000 0000 0000 0000     .B. .B. ........

20064680 <exception_cause_13_string>:
20064680:	42a0 2006 42bc 2006 0000 0000 0000 0000     .B. .B. ........

20064690 <exception_cause_15_string>:
20064690:	4270 2006 4290 2006 0000 0000 0000 0000     pB. .B. ........

200646a0 <exception_cause_1_string>:
200646a0:	43f0 2006 42f0 2006 440c 2006 42f0 2006     .C. .B. .D. .B. 
200646b0:	442c 2006 42f0 2006 0000 0000 0000 0000     ,D. .B. ........

200646c0 <exception_cause_2_string>:
200646c0:	43c8 2006 43dc 2006 0000 0000 0000 0000     .C. .C. ........

200646d0 <exception_cause_4_string>:
200646d0:	43b0 2006 42bc 2006 0000 0000 0000 0000     .C. .B. ........

200646e0 <exception_cause_5_string>:
200646e0:	4370 2006 42bc 2006 4384 2006 42bc 2006     pC. .B. .C. .B. 
200646f0:	43a0 2006 0000 0000 0000 0000 0000 0000     .C. ............

20064700 <exception_cause_6_string>:
20064700:	4350 2006 4290 2006 0000 0000 0000 0000     PC. .B. ........

20064710 <exception_cause_7_string>:
20064710:	4304 2006 4290 2006 4320 2006 4290 2006     .C. .B.  C. .B. 
20064720:	4340 2006 0000 0000 0000 0000 0000 0000     @C. ............

20064730 <exception_cause_string_tbl>:
20064730:	4658 2006 46a0 2006 46c0 2006 0000 0000     XF. .F. .F. ....
20064740:	46d0 2006 46e0 2006 4700 2006 4710 2006     .F. .F. .G. .G. 
	...
20064760:	4670 2006 4680 2006 0000 0000 4690 2006     pF. .F. .....F. 

20064770 <exception_code_string>:
20064770:	4470 2006 4518 2006 43c8 2006 4534 2006     pD. .E. .C. 4E. 
20064780:	4540 2006 4558 2006 456c 2006 4588 2006     @E. XE. lE. .E. 
20064790:	459c 2006 45bc 2006 45dc 2006 45f0 2006     .E. .E. .E. .E. 
200647a0:	4610 2006 4628 2006 0000 0000 4638 2006     .F. (F. ....8F. 

200647b0 <register_string>:
200647b0:	4490 2006 4498 2006 449c 2006 44a0 2006     .D. .D. .D. .D. 
200647c0:	44a4 2006 44a8 2006 44ac 2006 44b0 2006     .D. .D. .D. .D. 
200647d0:	44b4 2006 44bc 2006 44c0 2006 44c4 2006     .D. .D. .D. .D. 
200647e0:	44c8 2006 44cc 2006 44d0 2006 44d4 2006     .D. .D. .D. .D. 
200647f0:	44d8 2006 44dc 2006 44e0 2006 44e4 2006     .D. .D. .D. .D. 
20064800:	44e8 2006 44ec 2006 44f0 2006 44f4 2006     .D. .D. .D. .D. 
20064810:	44f8 2006 44fc 2006 4500 2006 4504 2006     .D. .D. .E. .E. 
20064820:	4508 2006 450c 2006 4510 2006 4514 2006     .E. .E. .E. .E. 
20064830:	4544 2056 5049 2043 5041 2049 736d 2067     DEV IPC API msg 
20064840:	554e 4c4c 000a 0000 6f48 7473 4120 4950     NULL....Host API
20064850:	5520 6b6e 6f6e 6e77 6520 7476 2528 2978      Unknown evt(%x)
20064860:	000a 0000 0510 2006 05d4 2006 078a 2006     ....... ... ... 
20064870:	078a 2006 0638 2006 064a 2006 06be 2006     ... 8.. J.. ... 
20064880:	0576 2006 0576 2006 078a 2006 078a 2006     v.. v.. ... ... 
20064890:	051e 2006 616c 7473 6920 696e 2063 7069     ... last inic ip
200648a0:	2063 6f6e 2074 6468 206c 000a 7348 4d74     c not hdl ..HstM
200648b0:	6773 6553 646e 7720 6961 2074 6e69 6369     sgSend wait inic
200648c0:	6920 6370 6420 6e6f 2065 7830 7825 202c      ipc done 0x%x, 
200648d0:	7830 7825 000a 0000 6e69 6369 615f 6970     0x%x....inic_api
200648e0:	685f 736f 5f74 6174 6b73 0000 7243 6165     _host_task..Crea
200648f0:	6574 6120 6970 685f 736f 5f74 6174 6b73     te api_host_task
20064900:	4520 7272 000a 0000 4e49 4349 0000 0000      Err....INIC....
20064910:	6e69 6369 6d5f 5f70 6f63 6d6d 6e61 2064     inic_mp_command 
20064920:	6166 6c69 000a 0000 7250 7669 7461 2065     fail....Private 
20064930:	654d 7373 6761 3a65 2520 0a73 0000 0000     Message: %s.....
20064940:	6f68 7473 735f 626b 785f 2078 6f6e 2074     host_skb_xx not 
20064950:	7266 6565 000a 0000 7325 3e3d 6b73 2062     free....%s=>skb 
20064960:	616d 6c6c 636f 6620 6961 216c 0d0a 0000     malloc fail!....
20064970:	6f48 7473 5520 6b6e 6f6e 6e77 6520 6576     Host Unknown eve
20064980:	746e 2528 786c 2129 0d0a 0000               nt(%lx)!....

2006498c <__func__.1>:
2006498c:	6e69 6369 685f 736f 5f74 6e69 7469 735f     inic_host_init_s
2006499c:	626b 0000 6e69 6369 6d5f 6773 715f 745f     kb..inic_msg_q_t
200649ac:	7361 006b 7243 6165 6574 6d20 6773 715f     ask.Create msg_q
200649bc:	745f 7361 206b 7245 2172 000a 4f4e 6220     _task Err!..NO b
200649cc:	6675 6620 726f 6e20 7765 6e20 646f 7365     uf for new nodes
200649dc:	0a21 0000 6146 6c69 7420 206f 6966 646e     !...Fail to find
200649ec:	7420 6568 7420 6d69 7265 655f 746e 7972      the timer_entry
200649fc:	6920 206e 6974 656d 2072 6174 6c62 2e65      in timer table.
20064a0c:	000a 0000 4954 454d 0052 0000 6146 6c69     ....TIMER...Fail
20064a1c:	7420 206f 6e69 7469 7420 6d69 7265 0a2e      to init timer..
20064a2c:	0000 0000 6f4d 5464 6d69 7265 203a 6f6e     ....ModTimer: no
20064a3c:	2074 6e69 7469 0a2e 0000 0000 6f4d 5464     t init......ModT
20064a4c:	6d69 7265 6620 6961 0a6c 0000 6143 636e     imer fail...Canc
20064a5c:	6c65 6954 656d 2072 6146 6c69 2528 2978     elTimer Fail(%x)
20064a6c:	000a 0000 6544 546c 6d69 7265 4620 6961     ....DelTimer Fai
20064a7c:	0a6c 0000                                   l...

20064a80 <__clz_tab>:
20064a80:	0100 0202 0303 0303 0404 0404 0404 0404     ................
20064a90:	0505 0505 0505 0505 0505 0505 0505 0505     ................
20064aa0:	0606 0606 0606 0606 0606 0606 0606 0606     ................
20064ab0:	0606 0606 0606 0606 0606 0606 0606 0606     ................
20064ac0:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064ad0:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064ae0:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064af0:	0707 0707 0707 0707 0707 0707 0707 0707     ................
20064b00:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b10:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b20:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b30:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b40:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b50:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b60:	0808 0808 0808 0808 0808 0808 0808 0808     ................
20064b70:	0808 0808 0808 0808 0808 0808 0808 0808     ................

20064b80 <_ctype_>:
20064b80:	2000 2020 2020 2020 2020 2828 2828 2028     .         ((((( 
20064b90:	2020 2020 2020 2020 2020 2020 2020 2020                     
20064ba0:	8820 1010 1010 1010 1010 1010 1010 1010      ...............
20064bb0:	0410 0404 0404 0404 0404 1004 1010 1010     ................
20064bc0:	1010 4141 4141 4141 0101 0101 0101 0101     ..AAAAAA........
20064bd0:	0101 0101 0101 0101 0101 0101 1010 1010     ................
20064be0:	1010 4242 4242 4242 0202 0202 0202 0202     ..BBBBBB........
20064bf0:	0202 0202 0202 0202 0202 0202 1010 1010     ................
20064c00:	0020 0000 0000 0000 0000 0000 0000 0000      ...............
	...

20064c84 <shell_cmd_table>:
20064c84:	3238 2006 0000 0000 b39e 2005 323c 2006     82. ....... <2. 
20064c94:	3264 2006 0004 0000 b4fa 2005 3268 2006     d2. ....... h2. 
20064ca4:	3384 2006 0002 0000 b454 2005 3388 2006     .3. ....T.. .3. 
20064cb4:	3418 2006 0004 0000 b40a 2005 3420 2006     .4. .......  4. 

20064cc4 <ipc_tickless_table>:
20064cc4:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064cd4:	0000 0000 0010 0000 0000 0000               ............

20064ce0 <ipc_kr4cdsp_table>:
20064ce0:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064cf0:	0000 0000 0020 0000 0000 0000               .... .......

20064cfc <ipc_KM4WKR4_table>:
20064cfc:	0001 0000 0000 0000 0000 0000 8eac 2005     ............... 
20064d0c:	0000 0000 0010 0000 0002 0000               ............

20064d18 <ipc_shell_table>:
20064d18:	0001 0000 cf26 2005 0000 0000 8eac 2005     ....&.. ....... 
20064d28:	0000 0000 0010 0000 0003 0000               ............

20064d34 <ipc_api_host_table>:
20064d34:	0000 0000 0794 2006 0000 0000 8eac 2005     ....... ....... 
20064d44:	0000 0000 0010 0000 0007 0000               ............

20064d50 <ipc_host_event_table>:
20064d50:	0000 0000 0d50 2006 0000 0000 8eac 2005     ....P.. ....... 
20064d60:	0000 0000 0010 0000 0006 0000               ............

20064d6c <lib_pmc_git_rev>:
20064d6c:	6c20 6269 705f 636d 765f 7265 365f 6466      lib_pmc_ver_6fd
20064d7c:	3461 3538 3931 5f66 3032 3432 312f 2f30     a48519f_2024/10/
20064d8c:	3131 312d 3a35 3635 323a 0032               11-15:56:22.

20064d98 <lib_wifi_com_sec_git_rev>:
20064d98:	6c20 6269 775f 6669 5f69 6f63 5f6d 6573      lib_wifi_com_se
20064da8:	5f63 6576 5f72 6636 6164 3834 3135 6639     c_ver_6fda48519f
20064db8:	325f 3230 2f34 3031 312f 2d31 3531 323a     _2024/10/11-15:2
20064dc8:	3a38 3835                                    8:58.

20064dcd <__git_ver_table_end__>:
20064dcd:	0000                	c.unimp
	...

20064dd0 <__frame_dummy_init_array_entry>:
20064dd0:	1436 2006                                   6.. 

20064dd4 <__do_global_dtors_aux_fini_array_entry>:
20064dd4:	13fc 2006                                   ... 

20064dd8 <__fini_array_end>:
	...

Disassembly of section .sram_image2.text.data:

20065de0 <FLASH_ClockSwitch>:
20065de0:	1101                	c.addi	sp,-32
20065de2:	ca26                	c.swsp	s1,20(sp)
20065de4:	c452                	c.swsp	s4,8(sp)
20065de6:	ce06                	c.swsp	ra,28(sp)
20065de8:	cc22                	c.swsp	s0,24(sp)
20065dea:	c84a                	c.swsp	s2,16(sp)
20065dec:	c64e                	c.swsp	s3,12(sp)
20065dee:	c256                	c.swsp	s5,4(sp)
20065df0:	84aa                	c.mv	s1,a0
20065df2:	8a2e                	c.mv	s4,a1
20065df4:	c1dd                	c.beqz	a1,20065e9a <FLASH_ClockSwitch+0xba>
20065df6:	ce1f20ef          	jal	ra,20058ad6 <Systick_State>
20065dfa:	89aa                	c.mv	s3,a0
20065dfc:	cfbf20ef          	jal	ra,20058af6 <irq_disable_save>
20065e00:	892a                	c.mv	s2,a0
20065e02:	4501                	c.li	a0,0
20065e04:	ca7f20ef          	jal	ra,20058aaa <Systick_Cmd>
20065e08:	20001ab7          	lui	s5,0x20001
20065e0c:	4789                	c.li	a5,2
20065e0e:	864a8413          	addi	s0,s5,-1948 # 20000864 <flash_init_para>
20065e12:	08f49e63          	bne	s1,a5,20065eae <FLASH_ClockSwitch+0xce>
20065e16:	4509                	c.li	a0,2
20065e18:	dffa0097          	auipc	ra,0xdffa0
20065e1c:	c26080e7          	jalr	ra,-986(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065e20:	4100c6b7          	lui	a3,0x4100c
20065e24:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065e28:	7775                	c.lui	a4,0xffffd
20065e2a:	177d                	c.addi	a4,-1
20065e2c:	8ff9                	c.and	a5,a4
20065e2e:	24f6ae23          	sw	a5,604(a3)
20065e32:	06344783          	lbu	a5,99(s0) # 9063 <shell_get_argv+0x18f>
20065e36:	c7ad                	c.beqz	a5,20065ea0 <FLASH_ClockSwitch+0xc0>
20065e38:	4501                	c.li	a0,0
20065e3a:	dff9e097          	auipc	ra,0xdff9e
20065e3e:	b40080e7          	jalr	ra,-1216(ra) # 397a <FLASH_CalibrationNewCmd>
20065e42:	4505                	c.li	a0,1
20065e44:	dff9e097          	auipc	ra,0xdff9e
20065e48:	8f4080e7          	jalr	ra,-1804(ra) # 3738 <FLASH_CalibrationPLLSel>
20065e4c:	dff9e097          	auipc	ra,0xdff9e
20065e50:	95e080e7          	jalr	ra,-1698(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065e54:	4501                	c.li	a0,0
20065e56:	dff9e097          	auipc	ra,0xdff9e
20065e5a:	8e2080e7          	jalr	ra,-1822(ra) # 3738 <FLASH_CalibrationPLLSel>
20065e5e:	dff9e097          	auipc	ra,0xdff9e
20065e62:	94c080e7          	jalr	ra,-1716(ra) # 37aa <FLASH_Calibration_PSPLL_Close>
20065e66:	04040ea3          	sb	zero,93(s0)
20065e6a:	00444583          	lbu	a1,4(s0)
20065e6e:	864a8513          	addi	a0,s5,-1948
20065e72:	dff9d097          	auipc	ra,0xdff9d
20065e76:	e52080e7          	jalr	ra,-430(ra) # 2cc4 <FLASH_SetSpiMode>
20065e7a:	0c0a0c63          	beq	s4,zero,20065f52 <FLASH_ClockSwitch+0x172>
20065e7e:	854e                	c.mv	a0,s3
20065e80:	c2bf20ef          	jal	ra,20058aaa <Systick_Cmd>
20065e84:	4462                	c.lwsp	s0,24(sp)
20065e86:	40f2                	c.lwsp	ra,28(sp)
20065e88:	44d2                	c.lwsp	s1,20(sp)
20065e8a:	49b2                	c.lwsp	s3,12(sp)
20065e8c:	4a22                	c.lwsp	s4,8(sp)
20065e8e:	4a92                	c.lwsp	s5,4(sp)
20065e90:	854a                	c.mv	a0,s2
20065e92:	4942                	c.lwsp	s2,16(sp)
20065e94:	6105                	c.addi16sp	sp,32
20065e96:	c69f206f          	jal	zero,20058afe <irq_enable_restore>
20065e9a:	4901                	c.li	s2,0
20065e9c:	4981                	c.li	s3,0
20065e9e:	b7ad                	c.j	20065e08 <FLASH_ClockSwitch+0x28>
20065ea0:	4581                	c.li	a1,0
20065ea2:	4501                	c.li	a0,0
20065ea4:	dff9e097          	auipc	ra,0xdff9e
20065ea8:	cda080e7          	jalr	ra,-806(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
20065eac:	bf59                	c.j	20065e42 <FLASH_ClockSwitch+0x62>
20065eae:	ccb5                	c.beqz	s1,20065f2a <FLASH_ClockSwitch+0x14a>
20065eb0:	4501                	c.li	a0,0
20065eb2:	dff9e097          	auipc	ra,0xdff9e
20065eb6:	886080e7          	jalr	ra,-1914(ra) # 3738 <FLASH_CalibrationPLLSel>
20065eba:	dff9e097          	auipc	ra,0xdff9e
20065ebe:	892080e7          	jalr	ra,-1902(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065ec2:	4505                	c.li	a0,1
20065ec4:	dff9e097          	auipc	ra,0xdff9e
20065ec8:	874080e7          	jalr	ra,-1932(ra) # 3738 <FLASH_CalibrationPLLSel>
20065ecc:	dff9e097          	auipc	ra,0xdff9e
20065ed0:	880080e7          	jalr	ra,-1920(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065ed4:	06344503          	lbu	a0,99(s0)
20065ed8:	891d                	c.andi	a0,7
20065eda:	dff9e097          	auipc	ra,0xdff9e
20065ede:	950080e7          	jalr	ra,-1712(ra) # 382a <FLASH_CalibrationPLLPS_Shift>
20065ee2:	4100c737          	lui	a4,0x4100c
20065ee6:	25c72783          	lw	a5,604(a4) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065eea:	668d                	c.lui	a3,0x3
20065eec:	8fd5                	c.or	a5,a3
20065eee:	24f72e23          	sw	a5,604(a4)
20065ef2:	4100c6b7          	lui	a3,0x4100c
20065ef6:	470d                	c.li	a4,3
20065ef8:	25c6a783          	lw	a5,604(a3) # 4100c25c <__km4_bd_boot_download_addr__+0x10ffa25c>
20065efc:	8b8d                	c.andi	a5,3
20065efe:	fee79de3          	bne	a5,a4,20065ef8 <FLASH_ClockSwitch+0x118>
20065f02:	06344783          	lbu	a5,99(s0)
20065f06:	cf8d                	c.beqz	a5,20065f40 <FLASH_ClockSwitch+0x160>
20065f08:	4505                	c.li	a0,1
20065f0a:	dff9e097          	auipc	ra,0xdff9e
20065f0e:	a70080e7          	jalr	ra,-1424(ra) # 397a <FLASH_CalibrationNewCmd>
20065f12:	4505                	c.li	a0,1
20065f14:	e091                	c.bnez	s1,20065f18 <FLASH_ClockSwitch+0x138>
20065f16:	4501                	c.li	a0,0
20065f18:	dffa0097          	auipc	ra,0xdffa0
20065f1c:	b26080e7          	jalr	ra,-1242(ra) # 5a3e <RCC_PeriphClockSource_SPIC>
20065f20:	05c44783          	lbu	a5,92(s0)
20065f24:	04f40ea3          	sb	a5,93(s0)
20065f28:	b789                	c.j	20065e6a <FLASH_ClockSwitch+0x8a>
20065f2a:	4505                	c.li	a0,1
20065f2c:	dff9e097          	auipc	ra,0xdff9e
20065f30:	80c080e7          	jalr	ra,-2036(ra) # 3738 <FLASH_CalibrationPLLSel>
20065f34:	dff9e097          	auipc	ra,0xdff9e
20065f38:	818080e7          	jalr	ra,-2024(ra) # 374c <FLASH_Calibration_PSPLL_Open>
20065f3c:	4501                	c.li	a0,0
20065f3e:	b759                	c.j	20065ec4 <FLASH_ClockSwitch+0xe4>
20065f40:	05c44503          	lbu	a0,92(s0)
20065f44:	4585                	c.li	a1,1
20065f46:	1579                	c.addi	a0,-2
20065f48:	dff9e097          	auipc	ra,0xdff9e
20065f4c:	c36080e7          	jalr	ra,-970(ra) # 3b7e <FLASH_Read_HandShake_Cmd>
20065f50:	b7c9                	c.j	20065f12 <FLASH_ClockSwitch+0x132>
20065f52:	40f2                	c.lwsp	ra,28(sp)
20065f54:	4462                	c.lwsp	s0,24(sp)
20065f56:	44d2                	c.lwsp	s1,20(sp)
20065f58:	4942                	c.lwsp	s2,16(sp)
20065f5a:	49b2                	c.lwsp	s3,12(sp)
20065f5c:	4a22                	c.lwsp	s4,8(sp)
20065f5e:	4a92                	c.lwsp	s5,4(sp)
20065f60:	6105                	c.addi16sp	sp,32
20065f62:	8082                	c.jr	ra

20065f64 <FLASH_UserMode_Exit>:
20065f64:	4100a737          	lui	a4,0x4100a
20065f68:	431c                	c.lw	a5,0(a4)
20065f6a:	0786                	c.slli	a5,0x1
20065f6c:	8385                	c.srli	a5,0x1
20065f6e:	c31c                	c.sw	a5,0(a4)
20065f70:	431c                	c.lw	a5,0(a4)
20065f72:	fe07cfe3          	blt	a5,zero,20065f70 <FLASH_UserMode_Exit+0xc>
20065f76:	0ff0000f          	fence	iorw,iorw
20065f7a:	0001                	c.addi	zero,0
20065f7c:	0001                	c.addi	zero,0
20065f7e:	0001                	c.addi	zero,0
20065f80:	0001                	c.addi	zero,0
20065f82:	0001                	c.addi	zero,0
20065f84:	0001                	c.addi	zero,0
20065f86:	0001                	c.addi	zero,0
20065f88:	0001                	c.addi	zero,0
20065f8a:	0001                	c.addi	zero,0
20065f8c:	0001                	c.addi	zero,0
20065f8e:	0001                	c.addi	zero,0
20065f90:	8082                	c.jr	ra

20065f92 <FLASH_WaitBusy_InUserMode>:
20065f92:	7179                	c.addi16sp	sp,-48
20065f94:	d422                	c.swsp	s0,40(sp)
20065f96:	20001437          	lui	s0,0x20001
20065f9a:	d226                	c.swsp	s1,36(sp)
20065f9c:	d04a                	c.swsp	s2,32(sp)
20065f9e:	ce4e                	c.swsp	s3,28(sp)
20065fa0:	cc52                	c.swsp	s4,24(sp)
20065fa2:	ca56                	c.swsp	s5,20(sp)
20065fa4:	d606                	c.swsp	ra,44(sp)
20065fa6:	84aa                	c.mv	s1,a0
20065fa8:	000107a3          	sb	zero,15(sp)
20065fac:	4781                	c.li	a5,0
20065fae:	4911                	c.li	s2,4
20065fb0:	4985                	c.li	s3,1
20065fb2:	4a0d                	c.li	s4,3
20065fb4:	86440413          	addi	s0,s0,-1948 # 20000864 <flash_init_para>
20065fb8:	4100aab7          	lui	s5,0x4100a
20065fbc:	ec91                	c.bnez	s1,20065fd8 <FLASH_WaitBusy_InUserMode+0x46>
20065fbe:	028aa783          	lw	a5,40(s5) # 4100a028 <__km4_bd_boot_download_addr__+0x10ff8028>
20065fc2:	8b85                	c.andi	a5,1
20065fc4:	ffe5                	c.bnez	a5,20065fbc <FLASH_WaitBusy_InUserMode+0x2a>
20065fc6:	50b2                	c.lwsp	ra,44(sp)
20065fc8:	5422                	c.lwsp	s0,40(sp)
20065fca:	5492                	c.lwsp	s1,36(sp)
20065fcc:	5902                	c.lwsp	s2,32(sp)
20065fce:	49f2                	c.lwsp	s3,28(sp)
20065fd0:	4a62                	c.lwsp	s4,24(sp)
20065fd2:	4ad2                	c.lwsp	s5,20(sp)
20065fd4:	6145                	c.addi16sp	sp,48
20065fd6:	8082                	c.jr	ra
20065fd8:	01249563          	bne	s1,s2,20065fe2 <FLASH_WaitBusy_InUserMode+0x50>
20065fdc:	008aa783          	lw	a5,8(s5)
20065fe0:	b7cd                	c.j	20065fc2 <FLASH_WaitBusy_InUserMode+0x30>
20065fe2:	01349d63          	bne	s1,s3,20065ffc <FLASH_WaitBusy_InUserMode+0x6a>
20065fe6:	05244503          	lbu	a0,82(s0)
20065fea:	00f10613          	addi	a2,sp,15
20065fee:	4585                	c.li	a1,1
20065ff0:	288d                	c.jal	20066062 <FLASH_RxCmd_InUserMode>
20065ff2:	00f14783          	lbu	a5,15(sp)
20065ff6:	4818                	c.lw	a4,16(s0)
20065ff8:	8ff9                	c.and	a5,a4
20065ffa:	b7e9                	c.j	20065fc4 <FLASH_WaitBusy_InUserMode+0x32>
20065ffc:	fd4494e3          	bne	s1,s4,20065fc4 <FLASH_WaitBusy_InUserMode+0x32>
20066000:	05244503          	lbu	a0,82(s0)
20066004:	00f10613          	addi	a2,sp,15
20066008:	4585                	c.li	a1,1
2006600a:	28a1                	c.jal	20066062 <FLASH_RxCmd_InUserMode>
2006600c:	00f14703          	lbu	a4,15(sp)
20066010:	4854                	c.lw	a3,20(s0)
20066012:	4785                	c.li	a5,1
20066014:	8ef9                	c.and	a3,a4
20066016:	d6dd                	c.beqz	a3,20065fc4 <FLASH_WaitBusy_InUserMode+0x32>
20066018:	481c                	c.lw	a5,16(s0)
2006601a:	8ff9                	c.and	a5,a4
2006601c:	00f037b3          	sltu	a5,zero,a5
20066020:	b755                	c.j	20065fc4 <FLASH_WaitBusy_InUserMode+0x32>

20066022 <FLASH_UserMode_Enter>:
20066022:	1141                	c.addi	sp,-16
20066024:	c606                	c.swsp	ra,12(sp)
20066026:	0ff0000f          	fence	iorw,iorw
2006602a:	0ff0000f          	fence	iorw,iorw
2006602e:	0001                	c.addi	zero,0
20066030:	0001                	c.addi	zero,0
20066032:	0001                	c.addi	zero,0
20066034:	0001                	c.addi	zero,0
20066036:	0001                	c.addi	zero,0
20066038:	0001                	c.addi	zero,0
2006603a:	0001                	c.addi	zero,0
2006603c:	0001                	c.addi	zero,0
2006603e:	0001                	c.addi	zero,0
20066040:	0001                	c.addi	zero,0
20066042:	0001                	c.addi	zero,0
20066044:	4501                	c.li	a0,0
20066046:	37b1                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
20066048:	4100a737          	lui	a4,0x4100a
2006604c:	431c                	c.lw	a5,0(a4)
2006604e:	800006b7          	lui	a3,0x80000
20066052:	8fd5                	c.or	a5,a3
20066054:	c31c                	c.sw	a5,0(a4)
20066056:	431c                	c.lw	a5,0(a4)
20066058:	fe07dfe3          	bge	a5,zero,20066056 <FLASH_UserMode_Enter+0x34>
2006605c:	40b2                	c.lwsp	ra,12(sp)
2006605e:	0141                	c.addi	sp,16
20066060:	8082                	c.jr	ra

20066062 <FLASH_RxCmd_InUserMode>:
20066062:	1101                	c.addi	sp,-32
20066064:	cc22                	c.swsp	s0,24(sp)
20066066:	20001437          	lui	s0,0x20001
2006606a:	ca26                	c.swsp	s1,20(sp)
2006606c:	c64e                	c.swsp	s3,12(sp)
2006606e:	84ae                	c.mv	s1,a1
20066070:	89aa                	c.mv	s3,a0
20066072:	4581                	c.li	a1,0
20066074:	86440513          	addi	a0,s0,-1948 # 20000864 <flash_init_para>
20066078:	c84a                	c.swsp	s2,16(sp)
2006607a:	ce06                	c.swsp	ra,28(sp)
2006607c:	8932                	c.mv	s2,a2
2006607e:	dff9d097          	auipc	ra,0xdff9d
20066082:	c46080e7          	jalr	ra,-954(ra) # 2cc4 <FLASH_SetSpiMode>
20066086:	4100a7b7          	lui	a5,0x4100a
2006608a:	10078613          	addi	a2,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
2006608e:	4e18                	c.lw	a4,24(a2)
20066090:	fff106b7          	lui	a3,0xfff10
20066094:	fff68593          	addi	a1,a3,-1 # fff0ffff <__ctrace_end__+0x35f0ffff>
20066098:	8f6d                	c.and	a4,a1
2006609a:	ce18                	c.sw	a4,24(a2)
2006609c:	00849713          	slli	a4,s1,0x8
200660a0:	8321                	c.srli	a4,0x8
200660a2:	c3d8                	c.sw	a4,4(a5)
200660a4:	1207a823          	sw	zero,304(a5)
200660a8:	4398                	c.lw	a4,0(a5)
200660aa:	cff68693          	addi	a3,a3,-769
200660ae:	8f75                	c.and	a4,a3
200660b0:	30076713          	ori	a4,a4,768
200660b4:	c398                	c.sw	a4,0(a5)
200660b6:	07378023          	sb	s3,96(a5)
200660ba:	4705                	c.li	a4,1
200660bc:	c798                	c.sw	a4,8(a5)
200660be:	4100a6b7          	lui	a3,0x4100a
200660c2:	4781                	c.li	a5,0
200660c4:	02979463          	bne	a5,s1,200660ec <FLASH_RxCmd_InUserMode+0x8a>
200660c8:	4511                	c.li	a0,4
200660ca:	35e1                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
200660cc:	86440793          	addi	a5,s0,-1948
200660d0:	86440513          	addi	a0,s0,-1948
200660d4:	4462                	c.lwsp	s0,24(sp)
200660d6:	40f2                	c.lwsp	ra,28(sp)
200660d8:	44d2                	c.lwsp	s1,20(sp)
200660da:	4942                	c.lwsp	s2,16(sp)
200660dc:	49b2                	c.lwsp	s3,12(sp)
200660de:	0047c583          	lbu	a1,4(a5)
200660e2:	6105                	c.addi16sp	sp,32
200660e4:	dff9d317          	auipc	t1,0xdff9d
200660e8:	be030067          	jalr	zero,-1056(t1) # 2cc4 <FLASH_SetSpiMode>
200660ec:	5698                	c.lw	a4,40(a3)
200660ee:	8b21                	c.andi	a4,8
200660f0:	db71                	c.beqz	a4,200660c4 <FLASH_RxCmd_InUserMode+0x62>
200660f2:	0606c603          	lbu	a2,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
200660f6:	00f90733          	add	a4,s2,a5
200660fa:	0785                	c.addi	a5,1
200660fc:	00c70023          	sb	a2,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20066100:	b7d1                	c.j	200660c4 <FLASH_RxCmd_InUserMode+0x62>

20066102 <FLASH_RxCmd>:
20066102:	1101                	c.addi	sp,-32
20066104:	ce06                	c.swsp	ra,28(sp)
20066106:	c62a                	c.swsp	a0,12(sp)
20066108:	c42e                	c.swsp	a1,8(sp)
2006610a:	c232                	c.swsp	a2,4(sp)
2006610c:	3f19                	c.jal	20066022 <FLASH_UserMode_Enter>
2006610e:	4612                	c.lwsp	a2,4(sp)
20066110:	45a2                	c.lwsp	a1,8(sp)
20066112:	4532                	c.lwsp	a0,12(sp)
20066114:	37b9                	c.jal	20066062 <FLASH_RxCmd_InUserMode>
20066116:	40f2                	c.lwsp	ra,28(sp)
20066118:	6105                	c.addi16sp	sp,32
2006611a:	b5a9                	c.j	20065f64 <FLASH_UserMode_Exit>

2006611c <FLASH_TxCmd_InUserMode>:
2006611c:	4100a7b7          	lui	a5,0x4100a
20066120:	1141                	c.addi	sp,-16
20066122:	c606                	c.swsp	ra,12(sp)
20066124:	c422                	c.swsp	s0,8(sp)
20066126:	c226                	c.swsp	s1,4(sp)
20066128:	c04a                	c.swsp	s2,0(sp)
2006612a:	10078713          	addi	a4,a5,256 # 4100a100 <__km4_bd_boot_download_addr__+0x10ff8100>
2006612e:	01872903          	lw	s2,24(a4)
20066132:	4384                	c.lw	s1,0(a5)
20066134:	4394                	c.lw	a3,0(a5)
20066136:	ffc10837          	lui	a6,0xffc10
2006613a:	cff80813          	addi	a6,a6,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
2006613e:	0106f6b3          	and	a3,a3,a6
20066142:	c394                	c.sw	a3,0(a5)
20066144:	4f14                	c.lw	a3,24(a4)
20066146:	fff10837          	lui	a6,0xfff10
2006614a:	187d                	c.addi	a6,-1
2006614c:	0106f6b3          	and	a3,a3,a6
20066150:	cf14                	c.sw	a3,24(a4)
20066152:	0007a223          	sw	zero,4(a5)
20066156:	db0c                	c.sw	a1,48(a4)
20066158:	06a78023          	sb	a0,96(a5)
2006615c:	4100a737          	lui	a4,0x4100a
20066160:	4781                	c.li	a5,0
20066162:	02b7e163          	bltu	a5,a1,20066184 <FLASH_TxCmd_InUserMode+0x68>
20066166:	4100a437          	lui	s0,0x4100a
2006616a:	4785                	c.li	a5,1
2006616c:	c41c                	c.sw	a5,8(s0)
2006616e:	4511                	c.li	a0,4
20066170:	350d                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
20066172:	11242c23          	sw	s2,280(s0) # 4100a118 <__km4_bd_boot_download_addr__+0x10ff8118>
20066176:	c004                	c.sw	s1,0(s0)
20066178:	40b2                	c.lwsp	ra,12(sp)
2006617a:	4422                	c.lwsp	s0,8(sp)
2006617c:	4492                	c.lwsp	s1,4(sp)
2006617e:	4902                	c.lwsp	s2,0(sp)
20066180:	0141                	c.addi	sp,16
20066182:	8082                	c.jr	ra
20066184:	00f606b3          	add	a3,a2,a5
20066188:	0006c683          	lbu	a3,0(a3)
2006618c:	0785                	c.addi	a5,1
2006618e:	06d70023          	sb	a3,96(a4) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20066192:	bfc1                	c.j	20066162 <FLASH_TxCmd_InUserMode+0x46>

20066194 <FLASH_WriteEn_InUserMode>:
20066194:	1141                	c.addi	sp,-16
20066196:	4505                	c.li	a0,1
20066198:	c606                	c.swsp	ra,12(sp)
2006619a:	3be5                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
2006619c:	200017b7          	lui	a5,0x20001
200661a0:	8b47c503          	lbu	a0,-1868(a5) # 200008b4 <flash_init_para+0x50>
200661a4:	4601                	c.li	a2,0
200661a6:	4581                	c.li	a1,0
200661a8:	3f95                	c.jal	2006611c <FLASH_TxCmd_InUserMode>
200661aa:	40b2                	c.lwsp	ra,12(sp)
200661ac:	450d                	c.li	a0,3
200661ae:	0141                	c.addi	sp,16
200661b0:	b3cd                	c.j	20065f92 <FLASH_WaitBusy_InUserMode>

200661b2 <FLASH_TxData>:
200661b2:	1101                	c.addi	sp,-32
200661b4:	0ff57793          	andi	a5,a0,255
200661b8:	ce06                	c.swsp	ra,28(sp)
200661ba:	cc22                	c.swsp	s0,24(sp)
200661bc:	ca26                	c.swsp	s1,20(sp)
200661be:	c84a                	c.swsp	s2,16(sp)
200661c0:	c64e                	c.swsp	s3,12(sp)
200661c2:	97ae                	c.add	a5,a1
200661c4:	10000713          	addi	a4,zero,256
200661c8:	00f77a63          	bgeu	a4,a5,200661dc <FLASH_TxData+0x2a>
200661cc:	20062537          	lui	a0,0x20062
200661d0:	23a00593          	addi	a1,zero,570
200661d4:	fb050513          	addi	a0,a0,-80 # 20061fb0 <strtol+0xc>
200661d8:	f30f90ef          	jal	ra,2005f908 <io_assert_failed>
200661dc:	84ae                	c.mv	s1,a1
200661de:	8932                	c.mv	s2,a2
200661e0:	842a                	c.mv	s0,a0
200661e2:	3581                	c.jal	20066022 <FLASH_UserMode_Enter>
200661e4:	3f45                	c.jal	20066194 <FLASH_WriteEn_InUserMode>
200661e6:	4100a7b7          	lui	a5,0x4100a
200661ea:	0007a983          	lw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200661ee:	4398                	c.lw	a4,0(a5)
200661f0:	ffc106b7          	lui	a3,0xffc10
200661f4:	cff68693          	addi	a3,a3,-769 # ffc0fcff <__ctrace_end__+0x35c0fcff>
200661f8:	8f75                	c.and	a4,a3
200661fa:	200016b7          	lui	a3,0x20001
200661fe:	c398                	c.sw	a4,0(a5)
20066200:	86468693          	addi	a3,a3,-1948 # 20000864 <flash_init_para>
20066204:	0646c703          	lbu	a4,100(a3)
20066208:	1187a603          	lw	a2,280(a5)
2006620c:	000f05b7          	lui	a1,0xf0
20066210:	0742                	c.slli	a4,0x10
20066212:	8f6d                	c.and	a4,a1
20066214:	fff105b7          	lui	a1,0xfff10
20066218:	15fd                	c.addi	a1,-1
2006621a:	8e6d                	c.and	a2,a1
2006621c:	8f51                	c.or	a4,a2
2006621e:	10e7ac23          	sw	a4,280(a5)
20066222:	00849713          	slli	a4,s1,0x8
20066226:	0007a223          	sw	zero,4(a5)
2006622a:	8321                	c.srli	a4,0x8
2006622c:	12e7a823          	sw	a4,304(a5)
20066230:	4709                	c.li	a4,2
20066232:	06e78023          	sb	a4,96(a5)
20066236:	0646c683          	lbu	a3,100(a3)
2006623a:	4711                	c.li	a4,4
2006623c:	00e69663          	bne	a3,a4,20066248 <FLASH_TxData+0x96>
20066240:	01845713          	srli	a4,s0,0x18
20066244:	06e78023          	sb	a4,96(a5)
20066248:	01045713          	srli	a4,s0,0x10
2006624c:	4100a7b7          	lui	a5,0x4100a
20066250:	0ff77713          	andi	a4,a4,255
20066254:	06e78023          	sb	a4,96(a5) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
20066258:	00845713          	srli	a4,s0,0x8
2006625c:	0ff77713          	andi	a4,a4,255
20066260:	06e78023          	sb	a4,96(a5)
20066264:	0ff47413          	andi	s0,s0,255
20066268:	06878023          	sb	s0,96(a5)
2006626c:	4100a6b7          	lui	a3,0x4100a
20066270:	4781                	c.li	a5,0
20066272:	00978563          	beq	a5,s1,2006627c <FLASH_TxData+0xca>
20066276:	5698                	c.lw	a4,40(a3)
20066278:	8b09                	c.andi	a4,2
2006627a:	eb05                	c.bnez	a4,200662aa <FLASH_TxData+0xf8>
2006627c:	4685                	c.li	a3,1
2006627e:	4100a737          	lui	a4,0x4100a
20066282:	c714                	c.sw	a3,8(a4)
20066284:	4100a6b7          	lui	a3,0x4100a
20066288:	0297e963          	bltu	a5,s1,200662ba <FLASH_TxData+0x108>
2006628c:	4511                	c.li	a0,4
2006628e:	3311                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
20066290:	4100a7b7          	lui	a5,0x4100a
20066294:	0137a023          	sw	s3,0(a5) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
20066298:	4505                	c.li	a0,1
2006629a:	39e5                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
2006629c:	4462                	c.lwsp	s0,24(sp)
2006629e:	40f2                	c.lwsp	ra,28(sp)
200662a0:	44d2                	c.lwsp	s1,20(sp)
200662a2:	4942                	c.lwsp	s2,16(sp)
200662a4:	49b2                	c.lwsp	s3,12(sp)
200662a6:	6105                	c.addi16sp	sp,32
200662a8:	b975                	c.j	20065f64 <FLASH_UserMode_Exit>
200662aa:	00f90733          	add	a4,s2,a5
200662ae:	00074703          	lbu	a4,0(a4) # 4100a000 <__km4_bd_boot_download_addr__+0x10ff8000>
200662b2:	0785                	c.addi	a5,1
200662b4:	06e68023          	sb	a4,96(a3) # 4100a060 <__km4_bd_boot_download_addr__+0x10ff8060>
200662b8:	bf6d                	c.j	20066272 <FLASH_TxData+0xc0>
200662ba:	5698                	c.lw	a4,40(a3)
200662bc:	8b09                	c.andi	a4,2
200662be:	d769                	c.beqz	a4,20066288 <FLASH_TxData+0xd6>
200662c0:	00f90733          	add	a4,s2,a5
200662c4:	00074703          	lbu	a4,0(a4)
200662c8:	0785                	c.addi	a5,1
200662ca:	06e68023          	sb	a4,96(a3)
200662ce:	bf6d                	c.j	20066288 <FLASH_TxData+0xd6>

200662d0 <FLASH_SetStatus>:
200662d0:	1101                	c.addi	sp,-32
200662d2:	ce06                	c.swsp	ra,28(sp)
200662d4:	cc22                	c.swsp	s0,24(sp)
200662d6:	c62a                	c.swsp	a0,12(sp)
200662d8:	842e                	c.mv	s0,a1
200662da:	c432                	c.swsp	a2,8(sp)
200662dc:	3399                	c.jal	20066022 <FLASH_UserMode_Enter>
200662de:	3d5d                	c.jal	20066194 <FLASH_WriteEn_InUserMode>
200662e0:	4622                	c.lwsp	a2,8(sp)
200662e2:	4532                	c.lwsp	a0,12(sp)
200662e4:	0ff47593          	andi	a1,s0,255
200662e8:	3d15                	c.jal	2006611c <FLASH_TxCmd_InUserMode>
200662ea:	4505                	c.li	a0,1
200662ec:	315d                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
200662ee:	4462                	c.lwsp	s0,24(sp)
200662f0:	40f2                	c.lwsp	ra,28(sp)
200662f2:	6105                	c.addi16sp	sp,32
200662f4:	b985                	c.j	20065f64 <FLASH_UserMode_Exit>

200662f6 <FLASH_SetStatusBits>:
200662f6:	7179                	c.addi16sp	sp,-48
200662f8:	d606                	c.swsp	ra,44(sp)
200662fa:	d422                	c.swsp	s0,40(sp)
200662fc:	d226                	c.swsp	s1,36(sp)
200662fe:	ce4e                	c.swsp	s3,28(sp)
20066300:	842a                	c.mv	s0,a0
20066302:	89ae                	c.mv	s3,a1
20066304:	200014b7          	lui	s1,0x20001
20066308:	d04a                	c.swsp	s2,32(sp)
2006630a:	3b21                	c.jal	20066022 <FLASH_UserMode_Enter>
2006630c:	86448793          	addi	a5,s1,-1948 # 20000864 <flash_init_para>
20066310:	0527c503          	lbu	a0,82(a5)
20066314:	0070                	c.addi4spn	a2,sp,12
20066316:	4585                	c.li	a1,1
20066318:	33a9                	c.jal	20066062 <FLASH_RxCmd_InUserMode>
2006631a:	4785                	c.li	a5,1
2006631c:	00c14703          	lbu	a4,12(sp)
20066320:	86448493          	addi	s1,s1,-1948
20066324:	04f99d63          	bne	s3,a5,2006637e <FLASH_SetStatusBits+0x88>
20066328:	00e467b3          	or	a5,s0,a4
2006632c:	0ff7f793          	andi	a5,a5,255
20066330:	00f10623          	sb	a5,12(sp)
20066334:	4c9c                	c.lw	a5,24(s1)
20066336:	4905                	c.li	s2,1
20066338:	c385                	c.beqz	a5,20066358 <FLASH_SetStatusBits+0x62>
2006633a:	0534c503          	lbu	a0,83(s1)
2006633e:	00d10613          	addi	a2,sp,13
20066342:	4585                	c.li	a1,1
20066344:	3b39                	c.jal	20066062 <FLASH_RxCmd_InUserMode>
20066346:	00d14503          	lbu	a0,13(sp)
2006634a:	8021                	c.srli	s0,0x8
2006634c:	03299d63          	bne	s3,s2,20066386 <FLASH_SetStatusBits+0x90>
20066350:	8c49                	c.or	s0,a0
20066352:	008106a3          	sb	s0,13(sp)
20066356:	4909                	c.li	s2,2
20066358:	0554c783          	lbu	a5,85(s1)
2006635c:	eb8d                	c.bnez	a5,2006638e <FLASH_SetStatusBits+0x98>
2006635e:	3d1d                	c.jal	20066194 <FLASH_WriteEn_InUserMode>
20066360:	0544c503          	lbu	a0,84(s1)
20066364:	0070                	c.addi4spn	a2,sp,12
20066366:	85ca                	c.mv	a1,s2
20066368:	3b55                	c.jal	2006611c <FLASH_TxCmd_InUserMode>
2006636a:	4505                	c.li	a0,1
2006636c:	311d                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
2006636e:	3edd                	c.jal	20065f64 <FLASH_UserMode_Exit>
20066370:	50b2                	c.lwsp	ra,44(sp)
20066372:	5422                	c.lwsp	s0,40(sp)
20066374:	5492                	c.lwsp	s1,36(sp)
20066376:	5902                	c.lwsp	s2,32(sp)
20066378:	49f2                	c.lwsp	s3,28(sp)
2006637a:	6145                	c.addi16sp	sp,48
2006637c:	8082                	c.jr	ra
2006637e:	fff44793          	xori	a5,s0,-1
20066382:	8ff9                	c.and	a5,a4
20066384:	b765                	c.j	2006632c <FLASH_SetStatusBits+0x36>
20066386:	fff44413          	xori	s0,s0,-1
2006638a:	8c69                	c.and	s0,a0
2006638c:	b7d9                	c.j	20066352 <FLASH_SetStatusBits+0x5c>
2006638e:	3519                	c.jal	20066194 <FLASH_WriteEn_InUserMode>
20066390:	0544c503          	lbu	a0,84(s1)
20066394:	0070                	c.addi4spn	a2,sp,12
20066396:	4585                	c.li	a1,1
20066398:	3351                	c.jal	2006611c <FLASH_TxCmd_InUserMode>
2006639a:	4505                	c.li	a0,1
2006639c:	3edd                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
2006639e:	3bdd                	c.jal	20066194 <FLASH_WriteEn_InUserMode>
200663a0:	0554c503          	lbu	a0,85(s1)
200663a4:	00d10613          	addi	a2,sp,13
200663a8:	4585                	c.li	a1,1
200663aa:	bf7d                	c.j	20066368 <FLASH_SetStatusBits+0x72>

200663ac <FLASH_Erase>:
200663ac:	7179                	c.addi16sp	sp,-48
200663ae:	d606                	c.swsp	ra,44(sp)
200663b0:	d422                	c.swsp	s0,40(sp)
200663b2:	d226                	c.swsp	s1,36(sp)
200663b4:	4709                	c.li	a4,2
200663b6:	00a77a63          	bgeu	a4,a0,200663ca <FLASH_Erase+0x1e>
200663ba:	20062537          	lui	a0,0x20062
200663be:	2d700593          	addi	a1,zero,727
200663c2:	fb050513          	addi	a0,a0,-80 # 20061fb0 <strtol+0xc>
200663c6:	d42f90ef          	jal	ra,2005f908 <io_assert_failed>
200663ca:	20001437          	lui	s0,0x20001
200663ce:	86440713          	addi	a4,s0,-1948 # 20000864 <flash_init_para>
200663d2:	87ae                	c.mv	a5,a1
200663d4:	06474583          	lbu	a1,100(a4)
200663d8:	84aa                	c.mv	s1,a0
200663da:	0107d713          	srli	a4,a5,0x10
200663de:	0087d693          	srli	a3,a5,0x8
200663e2:	4511                	c.li	a0,4
200663e4:	0ff77713          	andi	a4,a4,255
200663e8:	0ff6f693          	andi	a3,a3,255
200663ec:	0ff7f613          	andi	a2,a5,255
200663f0:	86440413          	addi	s0,s0,-1948
200663f4:	04a59263          	bne	a1,a0,20066438 <FLASH_Erase+0x8c>
200663f8:	0187d693          	srli	a3,a5,0x18
200663fc:	00e10ea3          	sb	a4,29(sp)
20066400:	00879713          	slli	a4,a5,0x8
20066404:	07c2                	c.slli	a5,0x10
20066406:	83c1                	c.srli	a5,0x10
20066408:	83a1                	c.srli	a5,0x8
2006640a:	8fd9                	c.or	a5,a4
2006640c:	00d10e23          	sb	a3,28(sp)
20066410:	00f11f23          	sh	a5,30(sp)
20066414:	c62e                	c.swsp	a1,12(sp)
20066416:	3131                	c.jal	20066022 <FLASH_UserMode_Enter>
20066418:	3bb5                	c.jal	20066194 <FLASH_WriteEn_InUserMode>
2006641a:	45b2                	c.lwsp	a1,12(sp)
2006641c:	e495                	c.bnez	s1,20066448 <FLASH_Erase+0x9c>
2006641e:	05644503          	lbu	a0,86(s0)
20066422:	4601                	c.li	a2,0
20066424:	4581                	c.li	a1,0
20066426:	39dd                	c.jal	2006611c <FLASH_TxCmd_InUserMode>
20066428:	4505                	c.li	a0,1
2006642a:	36a5                	c.jal	20065f92 <FLASH_WaitBusy_InUserMode>
2006642c:	3e25                	c.jal	20065f64 <FLASH_UserMode_Exit>
2006642e:	50b2                	c.lwsp	ra,44(sp)
20066430:	5422                	c.lwsp	s0,40(sp)
20066432:	5492                	c.lwsp	s1,36(sp)
20066434:	6145                	c.addi16sp	sp,48
20066436:	8082                	c.jr	ra
20066438:	00e10e23          	sb	a4,28(sp)
2006643c:	00d10ea3          	sb	a3,29(sp)
20066440:	00c10f23          	sb	a2,30(sp)
20066444:	458d                	c.li	a1,3
20066446:	b7f9                	c.j	20066414 <FLASH_Erase+0x68>
20066448:	4785                	c.li	a5,1
2006644a:	0870                	c.addi4spn	a2,sp,28
2006644c:	00f49563          	bne	s1,a5,20066456 <FLASH_Erase+0xaa>
20066450:	05744503          	lbu	a0,87(s0)
20066454:	bfc9                	c.j	20066426 <FLASH_Erase+0x7a>
20066456:	05844503          	lbu	a0,88(s0)
2006645a:	b7f1                	c.j	20066426 <FLASH_Erase+0x7a>

2006645c <plic_get_pending>:
2006645c:	f1402773          	csrrs	a4,mhartid,zero
20066460:	00251793          	slli	a5,a0,0x2
20066464:	80001537          	lui	a0,0x80001
20066468:	c0050513          	addi	a0,a0,-1024 # 80000c00 <__ctrace_end__+0xb6000c00>
2006646c:	97aa                	c.add	a5,a0
2006646e:	0722                	c.slli	a4,0x8
20066470:	97ba                	c.add	a5,a4
20066472:	4388                	c.lw	a0,0(a5)
20066474:	8082                	c.jr	ra

20066476 <plic_interrupt_handler>:
20066476:	7179                	c.addi16sp	sp,-48
20066478:	d422                	c.swsp	s0,40(sp)
2006647a:	d226                	c.swsp	s1,36(sp)
2006647c:	ce4e                	c.swsp	s3,28(sp)
2006647e:	d606                	c.swsp	ra,44(sp)
20066480:	d04a                	c.swsp	s2,32(sp)
20066482:	0044                	c.addi4spn	s1,sp,4
20066484:	4401                	c.li	s0,0
20066486:	498d                	c.li	s3,3
20066488:	8522                	c.mv	a0,s0
2006648a:	3fc9                	c.jal	2006645c <plic_get_pending>
2006648c:	892a                	c.mv	s2,a0
2006648e:	8522                	c.mv	a0,s0
20066490:	dff9f097          	auipc	ra,0xdff9f
20066494:	292080e7          	jalr	ra,658(ra) # 5722 <plic_get_enable>
20066498:	c088                	c.sw	a0,0(s1)
2006649a:	00a975b3          	and	a1,s2,a0
2006649e:	8522                	c.mv	a0,s0
200664a0:	0405                	c.addi	s0,1
200664a2:	dff9f097          	auipc	ra,0xdff9f
200664a6:	268080e7          	jalr	ra,616(ra) # 570a <plic_set_enable>
200664aa:	0491                	c.addi	s1,4
200664ac:	fd341ee3          	bne	s0,s3,20066488 <plic_interrupt_handler+0x12>
200664b0:	0ff0000f          	fence	iorw,iorw
200664b4:	dff9f097          	auipc	ra,0xdff9f
200664b8:	322080e7          	jalr	ra,802(ra) # 57d6 <plic_claim_irq>
200664bc:	4592                	c.lwsp	a1,4(sp)
200664be:	e9818493          	addi	s1,gp,-360 # 20066848 <plic_irq_id>
200664c2:	c088                	c.sw	a0,0(s1)
200664c4:	4501                	c.li	a0,0
200664c6:	dff9f097          	auipc	ra,0xdff9f
200664ca:	244080e7          	jalr	ra,580(ra) # 570a <plic_set_enable>
200664ce:	45a2                	c.lwsp	a1,8(sp)
200664d0:	4505                	c.li	a0,1
200664d2:	e9818413          	addi	s0,gp,-360 # 20066848 <plic_irq_id>
200664d6:	dff9f097          	auipc	ra,0xdff9f
200664da:	234080e7          	jalr	ra,564(ra) # 570a <plic_set_enable>
200664de:	45b2                	c.lwsp	a1,12(sp)
200664e0:	4509                	c.li	a0,2
200664e2:	dff9f097          	auipc	ra,0xdff9f
200664e6:	228080e7          	jalr	ra,552(ra) # 570a <plic_set_enable>
200664ea:	4084                	c.lw	s1,0(s1)
200664ec:	c4b1                	c.beqz	s1,20066538 <plic_interrupt_handler+0xc2>
200664ee:	02048493          	addi	s1,s1,32
200664f2:	8526                	c.mv	a0,s1
200664f4:	77e000e7          	jalr	ra,1918(zero) # 0 <Reset_Handler>
200664f8:	892a                	c.mv	s2,a0
200664fa:	c531                	c.beqz	a0,20066546 <plic_interrupt_handler+0xd0>
200664fc:	dff9f097          	auipc	ra,0xdff9f
20066500:	316080e7          	jalr	ra,790(ra) # 5812 <plic_get_threshold>
20066504:	89aa                	c.mv	s3,a0
20066506:	4008                	c.lw	a0,0(s0)
20066508:	dff9f097          	auipc	ra,0xdff9f
2006650c:	29e080e7          	jalr	ra,670(ra) # 57a6 <plic_get_irq_priority>
20066510:	dff9f097          	auipc	ra,0xdff9f
20066514:	2ea080e7          	jalr	ra,746(ra) # 57fa <plic_set_threshold>
20066518:	8526                	c.mv	a0,s1
2006651a:	7b2000e7          	jalr	ra,1970(zero) # 0 <Reset_Handler>
2006651e:	9902                	c.jalr	s2
20066520:	854e                	c.mv	a0,s3
20066522:	dff9f097          	auipc	ra,0xdff9f
20066526:	2d8080e7          	jalr	ra,728(ra) # 57fa <plic_set_threshold>
2006652a:	4008                	c.lw	a0,0(s0)
2006652c:	dff9f097          	auipc	ra,0xdff9f
20066530:	2bc080e7          	jalr	ra,700(ra) # 57e8 <plic_complete_irq>
20066534:	00042023          	sw	zero,0(s0)
20066538:	50b2                	c.lwsp	ra,44(sp)
2006653a:	5422                	c.lwsp	s0,40(sp)
2006653c:	5492                	c.lwsp	s1,36(sp)
2006653e:	5902                	c.lwsp	s2,32(sp)
20066540:	49f2                	c.lwsp	s3,28(sp)
20066542:	6145                	c.addi16sp	sp,48
20066544:	8082                	c.jr	ra
20066546:	200626b7          	lui	a3,0x20062
2006654a:	200625b7          	lui	a1,0x20062
2006654e:	8726                	c.mv	a4,s1
20066550:	4a068693          	addi	a3,a3,1184 # 200624a0 <pmap_func+0xd4>
20066554:	05700613          	addi	a2,zero,87
20066558:	4c458593          	addi	a1,a1,1220 # 200624c4 <pmap_func+0xf8>
2006655c:	450d                	c.li	a0,3
2006655e:	c19f40ef          	jal	ra,2005b176 <rtk_log_write>
20066562:	b7e1                	c.j	2006652a <plic_interrupt_handler+0xb4>

20066564 <plic_get_active_irq_id>:
20066564:	e981a503          	lw	a0,-360(gp) # 20066848 <plic_irq_id>
20066568:	8082                	c.jr	ra

2006656a <__NVIC_GetActive_IRQ>:
2006656a:	06f00793          	addi	a5,zero,111
2006656e:	00a7d463          	bge	a5,a0,20066576 <__NVIC_GetActive_IRQ+0xc>
20066572:	4501                	c.li	a0,0
20066574:	8082                	c.jr	ra
20066576:	34202773          	csrrs	a4,mcause,zero
2006657a:	00171793          	slli	a5,a4,0x1
2006657e:	8385                	c.srli	a5,0x1
20066580:	00055763          	bge	a0,zero,2006658e <__NVIC_GetActive_IRQ+0x24>
20066584:	0541                	c.addi	a0,16
20066586:	fe0746e3          	blt	a4,zero,20066572 <__NVIC_GetActive_IRQ+0x8>
2006658a:	8d1d                	c.sub	a0,a5
2006658c:	a821                	c.j	200665a4 <__NVIC_GetActive_IRQ+0x3a>
2006658e:	fe0752e3          	bge	a4,zero,20066572 <__NVIC_GetActive_IRQ+0x8>
20066592:	477d                	c.li	a4,31
20066594:	fea75be3          	bge	a4,a0,2006658a <__NVIC_GetActive_IRQ+0x20>
20066598:	e981a783          	lw	a5,-360(gp) # 20066848 <plic_irq_id>
2006659c:	02078793          	addi	a5,a5,32
200665a0:	40a78533          	sub	a0,a5,a0
200665a4:	00153513          	sltiu	a0,a0,1
200665a8:	8082                	c.jr	ra

200665aa <__sram_image2_end__>:
	...

200665c0 <Flash_Layout>:
200665c0:	0000 0000 0000 0800 3fff 0801 0002 0000     .........?......
200665d0:	4000 0801 3fff 081f 0001 0000 0000 0820     .@...?........ .
200665e0:	3fff 0821 0003 0000 4000 0821 cfff 083d     .?!......@!...=.
200665f0:	0005 0000 d000 083d ffff 083d 0006 0000     ......=...=.....
20066600:	0000 083e ffff 083f 0004 0000 0000 0840     ..>...?.......@.
20066610:	ffff 086f 0007 0000 ffff ffff ffff ffff     ..o.............
20066620:	0008 0000 ffff ffff ffff ffff 00ff 0000     ................
20066630:	ffff ffff ffff ffff                         ........

20066638 <PSRAM_CALIB_PATTERN>:
20066638:	3344 1122 a55a a55a 5aa5 5aa5 2211 4433     D3".Z.Z..Z.Z."3D
20066648:	6996 9669 9669 6996                         .ii.i..i

20066650 <at_sys_items>:
20066650:	2ed4 2006 9efc 2005 0000 0000 0000 0000     ... ... ........
20066660:	2ee0 2006 9f1c 2005 0000 0000 0000 0000     ... ... ........
20066670:	2eec 2006 a79e 2005 0000 0000 0000 0000     ... ... ........
20066680:	2ef8 2006 9f3c 2005 0000 0000 0000 0000     ... <.. ........
20066690:	2f00 2006 aa08 2005 0000 0000 0000 0000     ./. ... ........
200666a0:	2f08 2006 9f96 2005 0000 0000 0000 0000     ./. ... ........
200666b0:	2f10 2006 9fb6 2005 0000 0000 0000 0000     ./. ... ........
200666c0:	2f18 2006 a05a 2005 0000 0000 0000 0000     ./. Z.. ........
200666d0:	2f20 2006 a0fe 2005 0000 0000 0000 0000      /. ... ........
200666e0:	2f28 2006 a2ec 2005 0000 0000 0000 0000     (/. ... ........
200666f0:	2f30 2006 a372 2005 0000 0000 0000 0000     0/. r.. ........

20066700 <impure_data>:
20066700:	0000 0000 887c 2006 88e4 2006 894c 2006     ....|.. ... L.. 
	...

20066800 <__sdata_start__>:
20066800:	0000                	c.unimp
20066802:	42c8                	c.lw	a0,4(a3)

20066804 <xISRStackTop>:
20066804:	7f60 2006                                   `.. 

20066808 <CSWTCH.251>:
20066808:	5258 5342 0044 0000                         XRBSD...

20066810 <SystemCoreClock>:
20066810:	d180 08f0                                   ....

20066814 <PXID_Idx>:
20066814:	0101 0101                                   ....

20066818 <system_can_yield>:
20066818:	0001 0000                                   ....

2006681c <wakelock>:
2006681c:	0001 0000                                   ....

20066820 <log_init_table>:
20066820:	aa32 2005                                   2.. 

20066824 <rtk_log_default_level>:
20066824:	0004 0000                                   ....

20066828 <mbedtls_calloc_func>:
20066828:	be42 2005                                   B.. 

2006682c <mbedtls_free_func>:
2006682c:	be46 2005                                   F.. 

20066830 <pullNextTime>:
20066830:	68f0 2006                                   .h. 

20066834 <uxTopUsedPriority>:
20066834:	000a 0000                                   ....

20066838 <crash_task_info>:
20066838:	edb8 2005                                   ... 

2006683c <_impure_ptr>:
2006683c:	6700 2006                                   .g. 
