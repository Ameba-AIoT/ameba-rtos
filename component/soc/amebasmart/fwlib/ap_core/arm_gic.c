/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "arm_gic.h"
#include "sys_io.h"
#include "ameba.h"

void arm_gic_irq_enable(uint32_t irq)
{
	int int_grp, int_off;

	int_grp = irq / 32;
	int_off = irq % 32;

	sys_write32((1 << int_off), (GICD_ISENABLERn + int_grp * 4));
}

void arm_gic_irq_disable(uint32_t irq)
{
	int int_grp, int_off;

	int_grp = irq / 32;
	int_off = irq % 32;

	sys_write32((1 << int_off), (GICD_ICENABLERn + int_grp * 4));
}

int arm_gic_irq_is_enabled(uint32_t irq)
{
	int int_grp, int_off;
	uint32_t enabler;

	int_grp = irq / 32;
	int_off = irq % 32;

	enabler = sys_read32(GICD_ISENABLERn + int_grp * 4);

	return (enabler & (1 << int_off)) != 0;
}

void arm_gic_irq_set_priority(
	uint32_t irq, uint32_t prio, uint32_t flags)
{
	int int_grp, int_off;
	uint32_t val;

	/* Set priority */
	sys_write8(prio & 0xff, GICD_IPRIORITYRn + irq);

	/* Set interrupt type */
	int_grp = (irq / 16) * 4;
	int_off = (irq % 16) * 2;

	val = sys_read32(GICD_ICFGRn + int_grp);
	val &= ~(GICD_ICFGR_MASK << int_off);
	if (flags & IRQ_TYPE_EDGE) {
		val |= (GICD_ICFGR_TYPE << int_off);
	}

	sys_write32(val, GICD_ICFGRn + int_grp);

}

void arm_gic_deactive_irq(uint32_t irq)
{
	sys_write32(irq, GICC_DIR);
}

uint32_t arm_gic_get_active(void)
{
	int irq;
	irq = sys_read32(GICC_IAR) & 0x3ff;
	return irq;
}

int arm_gic_irq_is_pending(uint32_t irq)
{
	int int_grp, int_off;
	uint32_t enabler;

	int_grp = irq / 32;
	int_off = irq % 32;

	enabler = sys_read32(GICD_ISPENDRn + int_grp * 4);

	return (enabler & (1 << int_off)) != 0;
}

void arm_gic_set_pending_irq(uint32_t irq)
{
	int int_grp, int_off;

	int_grp = irq / 32;
	int_off = irq % 32;
	sys_write32((1 << int_off), (GICD_ISPENDRn + int_grp * 4));
}

void arm_gic_clear_pending_irq(uint32_t irq)
{
	int int_grp, int_off;

	int_grp = irq / 32;
	int_off = irq % 32;
	sys_write32((1 << int_off), (GICD_ICPENDRn + int_grp * 4));
}

void arm_gic_eoi(uint32_t irq)
{
	/* set to inactive */
	sys_write32(irq, GICC_EOIR);
}

#if (CONFIG_CPUS_NUM > 1)
void arm_gic_raise_softirq(uint32_t cpu, uint32_t irq)
{
	if (irq > 15 || cpu > 7) {
		return;
	}
	sys_write32(1 << (16 + cpu) | irq, GICD_SGIR);
}
#endif

static void gic_dist_init(void)
{
	uint32_t gic_irqs, i;

	gic_irqs = sys_read32(GICD_TYPER) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	gic_irqs = gic_irqs < GIC_MAX_NUM_INTR ? gic_irqs : GIC_MAX_NUM_INTR;
	if (gic_irqs > 1020) {
		gic_irqs = 1020;
	}

	/*
	 * Disable the forwarding of pending interrupts
	 * from the Distributor to the CPU interfaces
	 */
	sys_write32(0, GICD_CTLR);

	/*
	 * Set all global interrupts to this CPU only.
	 */
	for (i = GIC_SPI_INT_BASE; i < gic_irqs; i += 4) {
		sys_write32(0x01010101, GICD_ITARGETSRn + i);
	}

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for (i = GIC_SPI_INT_BASE; i < gic_irqs; i += 16) {
		sys_write32(0, GICD_ICFGRn + i / 4);
	}

	/*  Set priority on all global interrupts.   */
	for (i = GIC_SPI_INT_BASE; i < gic_irqs; i += 4) {
		sys_write32(0xa0a0a0a0, GICD_IPRIORITYRn + i);
	}

	/* Set all interrupts to non-secure group-1 */
	for (i = GIC_SPI_INT_BASE; i < gic_irqs; i += 32) {
		sys_write32(0xffffffff, GICD_IGROUPRn + i / 8);
	}

	/*
	 * Disable all interrupts.  Leave the PPI and SGIs alone
	 * as these enables are banked registers.
	 */
	for (i = GIC_SPI_INT_BASE; i < gic_irqs; i += 32) {
#ifndef CONFIG_GIC_V1
		sys_write32(0xffffffff, GICD_ICACTIVERn + i / 8);
#endif
		sys_write32(0xffffffff, GICD_ICENABLERn + i / 8);
	}

	/*
	 * Enable the forwarding of pending interrupts
	 * from the Distributor to the CPU interfaces
	 */
	sys_write32(GICD_CTLR_ENABLE_MASK, GICD_CTLR);
}

static void gic_cpu_init(void)
{
	int i;
	uint32_t val;

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 */
#ifndef CONFIG_GIC_V1
	sys_write32(0xffffffff, GICD_ICACTIVERn);
#endif
	sys_write32(0xffff0000, GICD_ICENABLERn);
	sys_write32(0x0000ffff, GICD_ISENABLERn);

	/* Configure SGIs/PPIs as non-secure Group-1 */
	sys_write32(0xffffffff, GICD_IGROUPRn);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for (i = 0; i < 32; i += 4) {
		sys_write32(0xa0a0a0a0, GICD_IPRIORITYRn + i);
	}

	/*Writing 255 to the GICC_PMR always sets it to the largest supported priority field value. */
	sys_write32(0xff, GICC_PMR);

	/*
	 * Enable interrupts and signal them using the IRQ signal.
	 */
	val = sys_read32(GICC_CTLR);
#ifndef CONFIG_GIC_V1
	val &= ~GICC_CTLR_BYPASS_MASK;
#endif
	val &= ~GICC_CTLR_ENABLE_MASK;
	val |= GICC_CTLR_ENABLE_MASK;

	/* Set IGCC_CTLR.AckCtl if gicv2 implements without security Eetensions */
	val |= GICC_CTLR_ACKCTL;

	sys_write32(val, GICC_CTLR);
}

/**
 *
 * @brief Initialize the GIC device driver
 *
 *
 * @return N/A
 */

int arm_gic_init_secondary(void)
{
	gic_cpu_init();

	return 0;
}

__attribute__((noinline))
int arm_gic_init(void)
{
	/* Init of Distributor interface registers */
	gic_dist_init();

	/* Init CPU interface registers */
	gic_cpu_init();
	return 0;
}
