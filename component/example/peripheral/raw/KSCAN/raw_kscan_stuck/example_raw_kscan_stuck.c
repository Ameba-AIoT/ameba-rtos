#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>


u32 key_row[8] = {_PA_25, _PA_22, _PA_26, _PA_20, _PB_9, _PB_8, _PA_24, _PB_7};
u32 key_col[8] = {_PA_23, _PA_21, _PA_19, _PB_6, _PA_30, _PB_1, _PB_2, _PB_3};

u32 keyscan_irq_handle(void *Data)
{
	(void)Data;

	KSCAN_TypeDef *KeyScan = KEYSCAN_DEV;
	u32 intr_status;
	u32 temp;
	u32 tempi;
	u32 ksdata[16];
	u32 row_status = 0;

	intr_status = KeyScan_GetINT(KeyScan);

	if (intr_status & KS_BIT_SCAN_EVENT_INT_STATUS) { //auto cleared
		printf("Scan Intr\n");
		temp = KeyScan_GetDataNum(KeyScan);
		KeyScan_Read(KeyScan, ksdata, temp);

		for (tempi = 0; tempi < temp; tempi++) {
			printf(" %lx ", ksdata[tempi]);
		}
		printf("\n");
	}

	if (intr_status & KS_BIT_ALL_RELEASE_INT_STATUS) {
		printf("Release Intr\n");
		/*clear intr*/
		KeyScan_ClearINT(KeyScan, KS_BIT_ALL_RELEASE_INT_CLR);
		KeyScan_INTConfig(KeyScan, KS_BIT_ALL_RELEASE_INT_MASK | KS_BIT_SCAN_EVENT_INT_MASK, DISABLE);
		KeyScan_INTConfig(KeyScan, KS_BIT_ALL_DEFAULT_INT_MASK, ENABLE);
		KeyScan_SetStuckRow(KeyScan, 0);
	}

	if (intr_status & KS_BIT_ALL_DEFAULT_INT_STATUS) {
		printf("Default Intr\n");
		KeyScan_ClearINT(KeyScan, KS_BIT_ALL_DEFAULT_INT_CLR);
		/*clear intr*/
		temp = KeyScan_GetDataNum(KeyScan);
		if (temp > 0) {
			KeyScan_Read(KeyScan, ksdata, temp);
			for (tempi = 0; tempi < temp; tempi++) {
				printf(" %lx ", ksdata[tempi]);
			}
			printf("\n");

			KeyScan_INTConfig(KeyScan, KS_BIT_ALL_DEFAULT_INT_MASK, DISABLE);
			KeyScan_INTConfig(KeyScan, KS_BIT_ALL_RELEASE_INT_MASK | KS_BIT_SCAN_EVENT_INT_MASK, ENABLE);
		}
	}

	if (intr_status & KS_BIT_STUCK_EVENT_INT_STATUS) {
		printf("Stuck Intr\n");
		/* set row default status */
		row_status = KeyScan_GetStuckRow(KeyScan);
		KeyScan_SetStuckRow(KeyScan, row_status);

		/*clear intr*/
		KeyScan_ClearINT(KeyScan, KS_BIT_STUCK_INT_CLR);
	}
	return 0;
}


void keyscan_stuck(void)
{
	int i = 0;
	KeyScan_InitTypeDef  KeyScan_InitStruct;
	KSCAN_TypeDef *KeyScan = KEYSCAN_DEV;
	Pinmux_Swdoff();
	/* pinmux config */
	for (i = 0; i < 8; i++) {
		PAD_PullCtrl(key_col[i], GPIO_PuPd_NOPULL);
		Pinmux_Config(key_col[i], (PINMUX_FUNCTION_KEY_COL0 + i));
		PAD_PullCtrl(key_row[i], GPIO_PuPd_UP);
		Pinmux_Config(key_row[i], (PINMUX_FUNCTION_KEY_ROW0 + i));
	}

	/* enable function and clk */
	RCC_PeriphClockCmd(APBPeriph_KSCAN, APBPeriph_KSCAN_CLOCK, ENABLE);

	/* Init keyscan */
	KeyScan_StructInit(&KeyScan_InitStruct);
	KeyScan_InitStruct.KS_ColSel = 0xFF;
	KeyScan_InitStruct.KS_RowSel = 0xFF;
	KeyScan_InitStruct.KS_WorkMode = KS_EVENT_TRIGGER_MODE;
	KeyScan_InitStruct.KS_LimitLevel = 6;
	KeyScan_Init(KeyScan, &KeyScan_InitStruct);

	KeyScan_StuckAutoCmd(KeyScan, ENABLE);
	KeyScan_SetStuckThreshold(KeyScan, 300);

	KeyScan_StuckPeriodicalPull(KeyScan, 2000, 4000);

	KeyScan_INTConfig(KeyScan, KS_BIT_ALL_DEFAULT_INT_MASK | KS_BIT_STUCK_EVENT_INT_MASK, ENABLE);

	InterruptRegister((IRQ_FUN) keyscan_irq_handle, KEYSCAN_IRQ, (u32)NULL, 3);
	InterruptEn(KEYSCAN_IRQ, 3);
	KeyScan_Cmd(KeyScan, ENABLE);

	while (1);
}

int example_raw_kscan_stuck(void)
{
	if (rtos_task_create(NULL, "keyscan_stuck", (rtos_task_t)keyscan_stuck, NULL, (2048), (1)) != RTK_SUCCESS) {
		printf("Cannot create keyscan_stuck demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}
