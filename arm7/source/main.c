#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ipc2.h>

#include "sound.h"
#include "wheel.h"

#define KEY_X BIT(0)
#define KEY_Y BIT(1)
#define PEN_DOWN BIT(6)
#define HINGE BIT(7)

void InterruptHandler_IPC_SYNC(void) {
	u8 sync;

	sync = IPC_GetSync();

	if(sync == IPC2_REQUEST_STOP_PLAYING)
		pcmstop();
	else {
		int oldval, newval;
		
		oldval = readPowerManagement(PM_CONTROL_REG);

		if(sync == IPC2_REQUEST_SET_BACKLIGHTS_OFF)
			newval = oldval & ~PM_BACKLIGHT_TOP & ~PM_BACKLIGHT_BOTTOM;

		else if(sync == IPC2_REQUEST_SET_BACKLIGHTS_ON)
			newval = oldval | PM_BACKLIGHT_TOP | PM_BACKLIGHT_BOTTOM;

		else if(sync == IPC2_REQUEST_LEDBLINK_OFF)
			newval = oldval & ~PM_LED_BLINK;

		else if(sync == IPC2_REQUEST_LEDBLINK_ON)
			newval = oldval | PM_LED_BLINK;

			writePowerManagement(PM_CONTROL_REG, newval);
	}
}

void InterruptHandler_VBLANK(void) {
	uint16 buttons;
	int16 x = 0, y = 0, xpx = 0, ypx = 0, z1 = 0, z2 = 0;
	uint16 battery, aux;
	touchPosition touchpos;

	buttons = REG_KEYXY;

	battery = touchRead(TSC_MEASURE_BATTERY);
	aux = touchRead(TSC_MEASURE_AUX);

	if (!(buttons & PEN_DOWN)) {
		touchpos = touchReadXY();

		x = touchpos.x;
		y = touchpos.y;
		xpx = touchpos.px;
		ypx = touchpos.py;
		z1 = touchpos.z1;
		z2 = touchpos.z2;
	}

	IPC->mailBusy = 1;
	IPC->touchX		= x;
	IPC->touchY		= y;
	IPC->touchXpx = xpx;
	IPC->touchYpx	= ypx;
	IPC->touchZ1	= z1;
	IPC->touchZ2	= z2;
	IPC->buttons	= buttons;
	IPC->battery  = (u16)(readPowerManagement(PM_BATTERY_REG) & 0x01);
	if(readPowerManagement(4) & (1<<6))
		IPC->aux    = (u16)(readPowerManagement(4) & 0x08);
	else
		IPC->aux    = 0;
	IPC->mailBusy = 0;

	scanwheel();
	IPC2->wheel_status |= readwheel();
}

int main(void) {
	uint8 ct[sizeof(IPC->time)];
	int i;

	// Reset the clock if needed
	rtcReset();

	rtcGetTime((uint8 *)ct);
	BCDToInteger((uint8 *)&(ct[1]), 7);

	for(i=0; i<sizeof(ct); i++)
		IPC->time.curtime[i] = ct[i];

	irqInit();
	irqEnable(IRQ_VBLANK);
	irqSet(IRQ_VBLANK, InterruptHandler_VBLANK);

	InitSoundDevice();

	irqSet(IRQ_IPC_SYNC, InterruptHandler_IPC_SYNC);
	REG_IPC_SYNC = IPC_SYNC_IRQ_ENABLE;

	// Keep the ARM7 idle
	while (1) {
		swiWaitForVBlank();

		if(IPC2->sound_control == IPC2_SOUND_START)
			pcmplay();
		else if(IPC2->sound_control == IPC2_SOUND_CLICK)
			playclick();
	}
}
