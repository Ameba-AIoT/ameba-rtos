
#include "whc_host_tsf_sync.h"

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/ktime.h>

enum {
	TSF_WAIT_PULSE = 0,
	TSF_WAIT_TSF,
};

static struct tsf_latch *g_latch;

static DEFINE_SPINLOCK(g_sm_lock);

static int g_state = TSF_WAIT_PULSE;
static u64 g_H_mono_ns;
static u32 g_last_done_seq = 0xFFFFFFFFu;

static struct workqueue_struct *g_ack_wq;
static struct work_struct g_ack_work;
static u32 g_ack_seq;
static u32 g_ack_result;

static bool g_gpio_ready;

/* Required: misc_deregister() on an unregistered miscdevice list_del()s a
 * zeroed list_head and oopses. */
static bool g_misc_ready;

/* Running totals, only used to qualify the log messages below. */
static struct {
	u32 pair_cnt;
	u32 no_pulse_cnt;
	u32 ack_resend_cnt;
} g_stat;

/* Called under g_sm_lock. Both smp_wmb() are mandatory on weakly ordered ARM:
 * without them a reader can observe an even seq with mixed old/new fields. */
static void tsf_latch_publish(u64 H_mono_ns, u64 tsf_us, u32 seq)
{
	if (!g_latch) {
		return;
	}

	g_latch->seq++;                 /* odd: write in progress */
	smp_wmb();
	g_latch->H_mono_ns      = H_mono_ns;
	g_latch->TSF_latch_us   = tsf_us;
	g_latch->sample_seq     = seq;
	g_latch->update_mono_ns = ktime_to_ns(ktime_get_raw());
	g_latch->valid          = 1;
	smp_wmb();
	g_latch->seq++;                 /* even: write complete */
}

/* Runs on a workqueue because whc_host_send_data_to_dev() may block up to ~1s and
 * takes the SDIO TX lock, which must not happen from the RX path. */
static void tsf_ack_work_fn(struct work_struct *work)
{
	u8 buf[TSF_HDR_LEN + sizeof(struct tsf_ack)];
	struct tsf_ack ack;
	unsigned long flags;

	(void)work;

	/* Guards against the device being removed after this work was queued. */
	if (!global_idev.intf_ops) {
		pr_warn_ratelimited("tsf_sync: link not ready, dropping ACK\n");
		return;
	}

	spin_lock_irqsave(&g_sm_lock, flags);
	ack.seq    = g_ack_seq;
	ack.result = g_ack_result;
	spin_unlock_irqrestore(&g_sm_lock, flags);

	*(u32 *)buf = WHC_WIFI_TEST;
	buf[sizeof(u32)] = WHC_WIFI_TEST_TSF_ACK;
	memcpy(buf + TSF_HDR_LEN, &ack, sizeof(ack));

	/* with_txdesc = 0: the cmd path allocates its own buffer and prepends
	 * SIZE_TX_DESC for us, so buf may live on the stack. */
	whc_host_send_data_to_dev(buf, sizeof(buf), 0);
}

static irqreturn_t tsf_pulse_isr(int irq, void *dev_id)
{
	/* Timestamp first so nothing after IRQ entry counts towards accuracy.
	 * ktime_get_raw() is CLOCK_MONOTONIC_RAW, same time base the userspace
	 * interpolation uses. */
	u64 H = ktime_to_ns(ktime_get_raw());

	(void)irq;
	(void)dev_id;

	spin_lock(&g_sm_lock);
	/* Same action in both states; in WAIT_TSF this is latest-edge-wins, which
	 * discards a previously held spurious edge. */
	g_H_mono_ns = H;
	g_state     = TSF_WAIT_TSF;
	spin_unlock(&g_sm_lock);

	return IRQ_HANDLED;
}

/* Runs on RTW_RX_THREAD. Must use spin_lock_irqsave(): holding g_sm_lock with
 * interrupts enabled would self-deadlock against tsf_pulse_isr(). */
void whc_host_tsf_sync_push(const u8 *buf, u32 len)
{
	struct tsf_sync_sample s;
	unsigned long flags;
	u32 ack_res;
	bool paired = false;
	u32 log_seq, log_cnt;
	u64 log_tsf, log_H;

	if (!buf || len != sizeof(struct tsf_sync_sample)) {
		return;
	}
	/* Copy out: buf points into an skb that gets recycled, and the packed u64
	 * sits at a 4-byte offset. */
	memcpy(&s, buf, sizeof(s));

	spin_lock_irqsave(&g_sm_lock, flags);

	if (g_state == TSF_WAIT_TSF) {
		tsf_latch_publish(g_H_mono_ns, s.tsf, s.seq);
		g_last_done_seq = s.seq;
		g_state         = TSF_WAIT_PULSE;

		g_stat.pair_cnt++;
		ack_res = TSF_ACK_DONE;
		paired  = true;
		log_cnt = g_stat.pair_cnt;
	} else if (s.seq == g_last_done_seq) {
		/* Already paired, the ACK was lost downstream: resend it (idempotent).
		 * Not replying would make the device retransmit forever. */
		g_stat.ack_resend_cnt++;
		ack_res = TSF_ACK_DONE;
		log_cnt = g_stat.ack_resend_cnt;
	} else {
		/* New seq with no pulse in hand: drop this round but still ACK so the
		 * device can advance. */
		g_stat.no_pulse_cnt++;
		ack_res = TSF_ACK_NO_PULSE;
		log_cnt = g_stat.no_pulse_cnt;
	}

	g_ack_seq    = s.seq;
	g_ack_result = ack_res;

	log_seq = s.seq;
	log_tsf = s.tsf;
	log_H   = g_H_mono_ns;
	spin_unlock_irqrestore(&g_sm_lock, flags);

	if (g_ack_wq) {
		queue_work(g_ack_wq, &g_ack_work);
	}

	if (paired) {
		pr_info_ratelimited("tsf_sync: seq=%u TSF=%llu us H_mono=%llu ns (pair=%u)\n",
							log_seq, log_tsf, log_H, log_cnt);
	} else if (ack_res == TSF_ACK_NO_PULSE) {
		pr_warn_ratelimited("tsf_sync: seq=%u no pulse, replying NO_PULSE (total %u), "
							"check the calibration wire on pin %d (BCM GPIO18)\n",
							log_seq, log_cnt, TSF_PULSE_PIN);
	} else {
		pr_warn_ratelimited("tsf_sync: seq=%u ACK lost, resending DONE (total %u)\n",
							log_seq, log_cnt);
	}
}

static int tsf_latch_mmap(struct file *f, struct vm_area_struct *vma)
{
	unsigned long sz = vma->vm_end - vma->vm_start;

	(void)f;

	if (!g_latch) {
		return -ENODEV;
	}
	if (vma->vm_pgoff != 0) {
		return -EINVAL;
	}
	if (sz > PAGE_SIZE) {
		return -EINVAL;
	}
	/* Read-only only: the kernel state machine is the sole seqlock writer. */
	if (vma->vm_flags & VM_WRITE) {
		return -EPERM;
	}

	return remap_pfn_range(vma, vma->vm_start,
						   virt_to_phys(g_latch) >> PAGE_SHIFT,
						   sz, vma->vm_page_prot);
}

/* .owner makes rmmod return -EBUSY while an application still holds the
 * mapping, so the page is never freed under a reader. */
static const struct file_operations g_latch_fops = {
	.owner = THIS_MODULE,
	.mmap  = tsf_latch_mmap,
};

static struct miscdevice g_latch_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "tsf_latch",
	.fops  = &g_latch_fops,
};

static int tsf_gpio_init(void)
{
	int ret;

	ret = gpio_request(TSF_PULSE_PIN, "tsf_pulse");
	if (ret) {
		pr_err("tsf_sync: gpio_request(%d) failed %d\n", TSF_PULSE_PIN, ret);
		if (ret == -EBUSY) {
			pr_err("tsf_sync: pin already claimed -- comment out "
				   "dtoverlay=pps-gpio,gpiopin=18 in config.txt and reboot\n");
		}
		pr_err("tsf_sync: if the pin number is wrong, check it with "
			   "'cat /sys/kernel/debug/gpio | grep GPIO18' and fix TSF_PULSE_PIN\n");
		return ret;
	}

	ret = gpio_direction_input(TSF_PULSE_PIN);
	if (ret) {
		pr_err("tsf_sync: gpio_direction_input(%d) failed %d\n", TSF_PULSE_PIN, ret);
		goto free_pin;
	}

	/* IRQF_NO_THREAD keeps this a hard IRQ: PREEMPT_RT or the threadirqs boot
	 * parameter would otherwise force threading and add tens of us of jitter. */
	ret = request_irq(TSF_PULSE_IRQ, tsf_pulse_isr,
					  IRQF_TRIGGER_RISING | IRQF_NO_THREAD,
					  "tsf_pulse", NULL);
	if (ret) {
		pr_err("tsf_sync: request_irq(gpio %d) failed %d\n", TSF_PULSE_PIN, ret);
		goto free_pin;
	}

	g_gpio_ready = true;
	pr_info("tsf_sync: calibration pulse irq ready (gpio %d, irq %d)\n",
			TSF_PULSE_PIN, TSF_PULSE_IRQ);
	return 0;

free_pin:
	gpio_free(TSF_PULSE_PIN);
	return ret;
}

static void tsf_gpio_deinit(void)
{
	if (!g_gpio_ready) {
		return;
	}
	free_irq(TSF_PULSE_IRQ, NULL);
	gpio_free(TSF_PULSE_PIN);
	g_gpio_ready = false;
}

static int tsf_ack_wq_init(void)
{
	if (g_ack_wq) {
		return 0;                      /* idempotent, guards a repeated probe */
	}

	g_ack_wq = alloc_ordered_workqueue("tsf_ack", 0);
	if (!g_ack_wq) {
		pr_err("tsf_sync: failed to create ACK workqueue, ACKs cannot be sent\n");
		return -ENOMEM;
	}
	INIT_WORK(&g_ack_work, tsf_ack_work_fn);

	return 0;
}

static void tsf_ack_wq_deinit(void)
{
	if (!g_ack_wq) {
		return;
	}

	cancel_work_sync(&g_ack_work);
	destroy_workqueue(g_ack_wq);
	g_ack_wq = NULL;
}

/* Called at the end of rtw_netdev_probe(), i.e. once the device is up and
 * intf_ops is valid, so the ACK workqueue can be created here as well. */
int whc_host_tsf_sync_register(void)
{
	int ret;

	/* Must be page aligned and physically contiguous for remap_pfn_range();
	 * __GFP_ZERO gives the initial valid=0. */
	g_latch = (struct tsf_latch *)__get_free_page(GFP_KERNEL | __GFP_ZERO);
	if (!g_latch) {
		pr_err("tsf_sync: latch page allocation failed\n");
		return -ENOMEM;
	}

	ret = misc_register(&g_latch_miscdev);
	if (ret) {
		pr_err("tsf_sync: failed to register %s: %d\n", TSF_LATCH_DEV, ret);
		goto free_latch;
	}
	g_misc_ready = true;

	/* A GPIO failure must not block the driver: wifi does not depend on TSF
	 * sync. The latch just stays invalid and tsf_now_us() returns < 0. */
	if (tsf_gpio_init()) {
		pr_err("tsf_sync: pulse irq not ready, TSF sync unavailable (latch stays invalid)\n");
	}

	/* Same reasoning: a workqueue failure only means ACKs cannot be sent, the
	 * device will retransmit. Do not fail the caller over it. */
	tsf_ack_wq_init();

	pr_info("tsf_sync: %s registered (in-kernel stop-and-wait state machine, no daemon needed)\n",
			TSF_LATCH_DEV);
	return 0;

free_latch:
	free_page((unsigned long)g_latch);
	g_latch = NULL;
	return ret;
}

void whc_host_tsf_sync_unregister(void)
{
	/* Drain the workqueue first: it is the only thing that can still be running
	 * concurrently with the teardown below. */
	tsf_ack_wq_deinit();

	tsf_gpio_deinit();

	if (g_misc_ready) {
		misc_deregister(&g_latch_miscdev);
		g_misc_ready = false;
	}

	if (g_latch) {
		free_page((unsigned long)g_latch);
		g_latch = NULL;
	}
}
