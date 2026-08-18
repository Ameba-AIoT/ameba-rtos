/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * helper_gpio_out — 通用 GPIO 输出激励固件
 *
 * 在双板测试中，Board B 运行此固件，按配置延迟后驱动指定 GPIO 引脚输出指定电平，
 * 为 Board A（DUT）提供外部 GPIO 信号（中断触发、唤醒信号、模拟按键等）。
 *
 * 编译宏配置（通过 CMake -D 或 prj.conf 传入）：
 *   HELPER_GPIO_PIN     必填，要驱动的 GPIO pin（如 _PA_19）
 *   HELPER_DELAY_MS     启动后延迟毫秒数（默认 15000）
 *   HELPER_LEVEL        输出电平：0=LOW, 1=HIGH（默认 1）
 *   HELPER_PULSE_MS     可选，脉冲宽度（ms）。输出 HELPER_LEVEL 保持该时长后翻转回初始电平
 *   HELPER_GPIO_PIN2    可选，第二个要驱动的 GPIO pin
 *   HELPER_LEVEL2       第二个 GPIO 的输出电平（默认 0）
 *
 * 工作流程：
 *   1. 初始化各 GPIO 为推挽输出
 *   2. 各 GPIO 立即输出反相电平（1 - target），确保后续有确切的电平跳变
 *   3. 延迟 HELPER_DELAY_MS
 *   4. 输出目标电平（触发 DUT 动作）
 *   5. 若定义了 HELPER_PULSE_MS，延迟后翻转回反相电平
 *   6. 删除任务退出
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

/* ── 编译时配置 ────────────────────────────────────────── */

#ifndef HELPER_GPIO_PIN
#error "HELPER_GPIO_PIN must be defined (e.g. -DHELPER_GPIO_PIN=_PA_19)"
#endif

#ifndef HELPER_DELAY_MS
#define HELPER_DELAY_MS  15000
#endif

#ifndef HELPER_LEVEL
#define HELPER_LEVEL     1
#endif

/* 第二个 GPIO（可选） */
#ifdef HELPER_GPIO_PIN2
#ifndef HELPER_LEVEL2
#define HELPER_LEVEL2     0
#endif
#endif

/* ── 参数结构与公共任务函数 ──────────────────────────────── */

typedef struct {
	const char *name;
	u32         pin;
	int         target_level;
} helper_ctx_t;

static void helper_task(void *param)
{
	const helper_ctx_t *ctx = (const helper_ctx_t *)param;
	int init_level = 1 - ctx->target_level;

	GPIO_WriteBit(ctx->pin, init_level);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s: init level=%d\n", ctx->name, init_level);

	if (HELPER_DELAY_MS > 0) {
		rtos_time_delay_ms(HELPER_DELAY_MS);
	}

	GPIO_WriteBit(ctx->pin, ctx->target_level);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s: drive level=%d\n", ctx->name, ctx->target_level);

#ifdef HELPER_PULSE_MS
	if (HELPER_PULSE_MS > 0) {
		rtos_time_delay_ms(HELPER_PULSE_MS);
		GPIO_WriteBit(ctx->pin, init_level);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s: pulse end, level=%d\n", ctx->name, init_level);
	}
#endif

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s: done\n", ctx->name);
	rtos_task_delete(NULL);
}

static const helper_ctx_t helper_ctx1 = {
	.name = "HELPER1",
	.pin = HELPER_GPIO_PIN,
	.target_level = HELPER_LEVEL,
};

#ifdef HELPER_GPIO_PIN2
static const helper_ctx_t helper_ctx2 = {
	.name = "HELPER2",
	.pin = HELPER_GPIO_PIN2,
	.target_level = HELPER_LEVEL2,
};
#endif

/* ── 入口 ──────────────────────────────────────────────── */

int example_helper_gpio_out(void)
{
	GPIO_InitTypeDef gpio_init;
	int ret;

	/* 初始化引脚 1 */
	gpio_init.GPIO_Pin = HELPER_GPIO_PIN;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&gpio_init);

	ret = rtos_task_create(NULL, "helper_gpio1",
						   (rtos_task_t)helper_task, (void *)&helper_ctx1,
						   512, 1);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "HELPER1: task create failed\n");
		return -1;
	}

	/* 可选：初始化引脚 2 */
#ifdef HELPER_GPIO_PIN2
	gpio_init.GPIO_Pin = HELPER_GPIO_PIN2;
	GPIO_Init(&gpio_init);

	ret = rtos_task_create(NULL, "helper_gpio2",
						   (rtos_task_t)helper_task, (void *)&helper_ctx2,
						   512, 1);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "HELPER2: task create failed\n");
		return -1;
	}
#endif

	return 0;
}
