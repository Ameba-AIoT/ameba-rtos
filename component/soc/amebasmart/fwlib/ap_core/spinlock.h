#ifndef CORTEX_A_SPINLOCK_H
#define CORTEX_A_SPINLOCK_H

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spinlock {
	volatile uint32_t lock __attribute__((aligned(64)));
	volatile uint32_t reserved[15];
} spinlock_t;

static inline void spin_lock(spinlock_t *lock)
{
	unsigned long tmp;

	__asm__ __volatile__(
		"1:	ldrex	%0, [%1]\n"
		"	cmp		%0, #0\n"
		"	wfene\n"
		"	strexeq	%0, %2, [%1]\n"
		"	cmpeq	%0, #0\n"
		"	bne		1b\n"
		"	dmb"
		: "=&r"(tmp)
		: "r"(&lock->lock), "r"(0x1)
		: "cc");
}

static inline void spin_unlock(spinlock_t *lock)
{
	__asm__ __volatile__(
		"dmb\n"
		"str	%1, [%0]\n"
		"dsb\n"
		"sev"
		:
		: "r"(&lock->lock), "r"(0x0)
		: "cc");

}

static inline unsigned int spin_lock_irqsave(spinlock_t *lock)
{
	uint32_t flags;
	unsigned long tmp;

	__asm volatile("mrs	%0, cpsr	\n": "=r"(flags) :: "memory");
	__asm volatile("CPSID i" ::: "memory");
	__asm volatile("DSB");
	__asm volatile("ISB");

	__asm__ __volatile__(
		"1:	ldrex	%0, [%1]\n"
		"	cmp		%0, #0\n"
		"	wfene\n"
		"	strexeq	%0, %2, [%1]\n"
		"	cmpeq	%0, #0\n"
		"	bne		1b\n"
		"	dmb"
		: "=&r"(tmp)
		: "r"(&lock->lock), "r"(0x1)
		: "cc");

	return flags;
}

static inline void spin_unlock_irqrestore(spinlock_t *lock, unsigned int flags)
{
	__asm__ __volatile__(
		"dmb\n"
		"str	%1, [%0]\n"
		"dsb\n"
		"sev"
		:
		: "r"(&lock->lock), "r"(0x0)
		: "cc");

	__asm volatile("msr	cpsr_c, %0	\n" :: "r"(flags) : "memory");
}

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */

#endif /* CORTEX_A_SPINLOCK_H */
