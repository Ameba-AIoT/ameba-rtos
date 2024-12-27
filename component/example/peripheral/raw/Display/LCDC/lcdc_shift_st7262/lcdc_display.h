#ifndef LCDC_DISPLAY_H
#define LCDC_DISPLAY_H

#define LCD_BLEN_RGB		_PA_31
#define LCD_DISPLAY_RGB 	_PB_30

enum LCDC_INTR_EVENT {
	EVENT_FRM_START = 0,
	EVENT_FRM_DONE,
	EVENT_LINE_HIT,
	EVENT_PANEL_TE,
};

struct LCDC_IRQInfoDef {
	u32 IrqNum;
	u32 IrqData;
	u32 IrqPriority;
};


#endif