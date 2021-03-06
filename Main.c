#include "device_driver.h"
#include "2440addr.h"
#include "GalaxyMaestro.h"
#include <stdlib.h>

typedef struct
{
	int day;
	int year;
	int mon;
	int date;
	int hour;
	int min;
	int sec;
}RTC_TIME;

int RTC_Get_Time(void)
{
	RTC_TIME a;
	/*
	a.day = (int)rBCDDAY;
	a.year = (int)rBCDYEAR;
	a.mon = (int)rBCDMON;
	a.date = (int)rBCDDATE;
	a.hour = (int)rBCDHOUR;
	a.min = (int)rBCDMIN;
	a.sec = (int)rBCDSEC;

	if(!a.sec)
	{
		a.day = (int)rBCDDAY;
		a.year = (int)rBCDYEAR;
		a.mon = (int)rBCDMON;
		a.date = (int)rBCDDATE;
		a.hour = (int)rBCDHOUR;
		a.min = (int)rBCDMIN;
	}

	time->day = a.day;
	time->year = ((a.year >> 4) & 0xf) * 10 + (a.year & 0xf);
	time->mon = ((a.mon >> 4) & 0xf) * 10 + (a.mon & 0xf);
	time->date = ((a.date >> 4) & 0xf) * 10 + (a.date & 0xf);
	time->hour = ((a.hour >> 4) & 0xf) * 10 + (a.hour & 0xf);
	time->min = ((a.min >> 4) & 0xf) * 10 + (a.min & 0xf);
	time->sec = ((a.sec >> 4) & 0xf) * 10 + (a.sec & 0xf);
	*/
	a.hour = (int)rBCDHOUR;
	a.min = (int)rBCDMIN;
	a.sec = (int)rBCDSEC;
	return  ((a.hour >> 4) & 0xf) * 10 + (a.hour & 0xf) * 360 + \
			((a.min >> 4) & 0xf) * 10 + (a.min & 0xf) * 60 + \
			((a.sec >> 4) & 0xf) * 10 + (a.sec & 0xf);

}

void User_Main(void);

void Main(void)
{
	MMU_Init();
	Led_Init();
	Key_Poll_Init();
	Uart_Init(115200);
	Timer0_Init();
	Timer3_Buzzer_Init();
	Timer4_Init();
	Lcd_Graphic_Init();
	Uart_Printf("Welcome to Galaxy Maestro Game!\n");

	User_Main();
	Uart_Printf("Good Bye~\n");

}


void User_Main(void)
{
	srand(RTC_Get_Time());
// graphic test
//	Lcd_Draw_Rect(20,20,120,120,0xf800);
//	Lcd_Draw_Rect(70,70,220,220,0x003e);

	for(;;){
		Lcd_Control_Brightness(5);
		Lcd_Select_Buffer_Mode(LAYER_MODE);
		Lcd_Select_Draw_Frame_Buffer(0);
		Lcd_Select_Display_Frame_Buffer(0);
		Galaxy_Maestro();
	}
}

