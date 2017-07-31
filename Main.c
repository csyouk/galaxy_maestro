/***********************************************************/
// 다음은 사용자 함수를 호출하는 루틴임 지우지 말것!
/***********************************************************/
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

#define printf 	Uart_Printf


/* =========== 사용자 정의 상수 =========*/
#define BACK_COLOR 	BLACK
#define WIDTH 		320
#define HEIGHT 		240

#include "./Image/img01.h"
#include "./Image/img02.h"
#include "./Image/img03.h"
#include "./Image/img04.h"
#include "./Image/img05.h"
#include "./Image/shape.h"
#include "./Image/perfect.h"

const unsigned short * Disp[] = {img01, img02, img03, img04};

extern struct Object Tank, Gun,Tank_beam;


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
	Lcd_Clr_Screen(0xf800);
	Uart_Printf("Welcome to Galaxy Maestro Game!\n");

	User_Main();
	Uart_Printf("Good Bye~\n");

}


void User_Main(void)
{
	srand(RTC_Get_Time());

	for(;;){
		Lcd_Control_Brightness(5);
		Lcd_Select_Buffer_Mode(LAYER_MODE);
		Lcd_Select_Draw_Frame_Buffer(0);
		Lcd_Select_Display_Frame_Buffer(0);
		Galaxy_Maestro();
	}
}


void explosion(void)
{
	if(Tank_beam.cd_flag == 1 || Gun.cd_flag == 1)
	{
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1]-40, Gun.pos_back[0] + 30, Gun.pos_back[1], WHITE);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1]-40, Gun.pos_back[0] + 30, Gun.pos_back[1], BLUE);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1]-40, Gun.pos_back[0] + 30, Gun.pos_back[1], RED);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1]-40, Gun.pos_back[0] + 30, Gun.pos_back[1], BLACK);
	}
}

void Move_Object(void)
{
	int key = 0;
	if(Timer0_Check_Expired())
	{
//		Tank.timer++;
		Gun.timer++;
		Tank_beam.timer++;

		key = Key_Get_Pressed();
//		Uart_Printf("key : %d\n", key);
//		if(key == 2){
//			Tank.timer = 0;
//			Tank.pos_back[0] = Tank.pos[0];			 // 현재의 위치를 벡업해 놓는다.
//			Tank.pos[0] = Tank.pos[0] + Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
//			Tank.move_flag = 1;						 //
//		}

		// 탱크를 우측으로 움직임.
		if( key == 4)
		{

			Tank.pos_back[0] = Tank.pos[0];			 // 현재의 위치를 벡업해 놓는다.
			Tank.pos[0] = Tank.pos[0] + Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
			Tank.move_flag = 1;						 //
		}
		// 탱크를 좌측으로 움직임.
		if(key == 2)
		{
			Tank.pos_back[0] = Tank.pos[0];			 // 현재의 위치를 벡업해 놓는다.
			Tank.pos[0] = Tank.pos[0] - Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
			Tank.move_flag = 1;						 //
		}

		if(Gun.timer >= Gun.speed_step)
		{
			Gun.timer = 0;
			Gun.pos_back[1] = Gun.pos[1];
			Gun.pos[1] = Gun.pos[1] - Gun.move_step;
			Gun.move_flag = 1;
		}
		if(key == 5)
		{
			if(Tank_beam.beam_flag == 0)
			{
				Tank_beam.beam_flag = 1;
			}
		}
		if(Tank_beam.beam_flag == 0)
		{
			Tank_beam.pos_init[0] = Tank.pos[0]+13;
			Tank_beam.pos_init[1] = Tank.pos[1]+12;
			Tank_beam.pos_back[0] = Tank_beam.pos_init[0];
			Tank_beam.pos_back[1] = Tank_beam.pos_init[1];
			Tank_beam.pos[0] = Tank_beam.pos_init[0];
			Tank_beam.pos[1] = Tank_beam.pos_init[1];
		}
		if(Tank_beam.beam_flag != 0 && Tank_beam.timer >= Tank_beam.speed_step)
		{
			Tank_beam.timer = 0;
			Tank_beam.pos_back[1] = Tank_beam.pos[1];
			Tank_beam.pos[1] = Tank_beam.pos[1] + Tank_beam.move_step;
			Tank_beam.move_flag = 1;
		}

//		Uart_Printf("Tank.timer %d / Tank.speed_step %d\n", Tank.timer, Tank.speed_step);
	}
}

void Draw_Object(void)
{
	if((Tank.pos[0] >= WIDTH))
	{
		Tank.pos[0] = Tank.pos_init[0];
	}

	if((Gun.pos[1] < 0))
	{
		Gun.pos[1] = Gun.pos_init[1];
		Lcd_Draw_Bar(Gun.pos_back[0], 0, Gun.pos_back[0] + Gun.size[0], 20, BACK_COLOR);
	}
	if((Tank_beam.pos[1] > 239))
	{
		Tank_beam.beam_flag = 0;
	}

	if(Tank.move_flag != 0) // 이미지가 움직였으면, 새로운 곳을 그리고, 그 이전 좌표는 지운다.
	{
		Lcd_Draw_Bar(Tank.pos_back[0], Tank.pos_back[1], Tank.pos_back[0] + Tank.size[0], Tank.pos_back[1] + Tank.size[1], BACK_COLOR);
		Lcd_Draw_Bar(Tank.pos[0], Tank.pos[1], Tank.pos[0] + Tank.size[0], Tank.pos[1] + Tank.size[1], Tank.color);
		Tank.move_flag = 0;
	}
	if(Gun.move_flag != 0)
	{
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1], Gun.pos_back[0] + Gun.size[0], Gun.pos_back[1] + Gun.size[1], BACK_COLOR);
		Lcd_Draw_Bar(Gun.pos[0], Gun.pos[1], Gun.pos[0] + Gun.size[0], Gun.pos[1] - Gun.size[1], Gun.color);
		Gun.move_flag = 0;
	}
	if(Tank_beam.move_flag != 0)
	{
		Lcd_Draw_Bar(Tank_beam.pos_back[0], Tank_beam.pos_back[1], Tank_beam.pos_back[0] + Tank_beam.size[0], Tank_beam.pos_back[1] + Tank_beam.size[1], BACK_COLOR);
		Lcd_Draw_Bar(Tank_beam.pos[0], Tank_beam.pos[1], Tank_beam.pos[0] + Tank_beam.size[0], Tank_beam.pos[1] + Tank_beam.size[1], Tank_beam.color);
		Tank_beam.move_flag = 0;
	}
	explosion();
	if(Tank_beam.cd_flag == 1)
	{
		Lcd_Draw_Bar(Tank_beam.pos[0], Tank_beam.pos[1], Tank_beam.pos[0] + Tank_beam.size[0], Tank_beam.pos[1] + Tank_beam.size[1], BACK_COLOR);
		Tank_beam.cd_flag = 0;
		Tank_beam.beam_flag = 0;
	}
	if(Gun.cd_flag == 1)
	{
		Lcd_Draw_Bar(Gun.pos[0], Gun.pos[1],
				Gun.pos[0] + Gun.size[0],
				Gun.pos[1] + Gun.size[1],
				BACK_COLOR);
		Gun.cd_flag = 0;
		Gun.pos[1] = Gun.pos_init[1];
	}
}

void collision_detect(void)
{
	if(Tank_beam.beam_flag != 0 && Tank_beam.cd_flag == 0)
	{
		if((Tank_beam.pos[0] > Gun.pos[0]) && \
		   (Tank_beam.pos[0] + Tank_beam.size[0] < Gun.pos[0] + Gun.size[0]) )
		{
			if((Tank_beam.pos[1] + Tank_beam.size[1]  >= Gun.pos[1]))
			{

				Tank_beam.move_flag = 1;
				Gun.move_flag = 1;

				Gun.timer = 0;
				Tank_beam.timer = 0;

				Tank_beam.cd_flag = 1;
				Gun.cd_flag = 1;
			}
		}
	}

}

void Game_Init(void)
{
	Lcd_Clr_Screen(BACK_COLOR);
	Timer0_Repeat(20);
}
