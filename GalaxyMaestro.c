#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>

#define BACK_COLOR 	BLACK
#define WIDTH 		320
#define HEIGHT 		240
#define X 0
#define Y 1
#define X_AND_Y 2

void Game_Init(void);
void explosion(void);
void Move_Object(void);
void collision_detect(void);
void Draw_Object(void);
void Galaxy_Maestro(void);
void Move_Tank_Depend_On_Key(int);

struct Object
{
	int timer;     	 	  // 몇번 타이머를 쓸 것인지?
	int move_flag;   	  // 움직이고 있는 상태인지 아니지,
	int pos[X_AND_Y]; 		  // x,y
	int pos_init[X_AND_Y];  	  // 초기 좌표.
	int pos_back[X_AND_Y];      // 이전 좌표의 정보. 이미지를 사용할 시, 이 좌표를 토대로 이전의 이미지를 지워야함.
	int size[X_AND_Y];	      // width, height

	unsigned short color; // 나중에 이미지로 대체.
	int speed_step;	 	  // 이미지가 얼마나 빨리 이동되게 할 것인지.
	int move_step;		  // 이미지를 얼마나 이동시킬 것인가?
	int beam_flag;		  // beam 발사 됐는지 여부 flag
	int cd_flag;		  // collision detection flag
	int dir[X_AND_Y];           // x,y 방향의 방향정보. 1과 -1 값이 있다.
	int missile_dir;      // 1,2,3,4 키 입력값을 방향값으로 설정한다. 이 값이 미사일의 방향과 동일하다.
};

/* 5:5:5:I Color Definition */

enum Color{
	BLACK=0x0000,
	WHITE=0xfffe,
	BLUE=0x003e,
	GREEN=0x07c0,
	RED=0xf800
};

struct Object Tank = {0,1,{0,10},{0,10},{0,10},{30,10},
					RED, 4, 8, 0, 0, {1,1}, 1};
struct Object Ufo = {0,1,{160,300},{160,300},{160,300},{30,30},
					BLUE, 4, 7, 0, 0, {1,1}};
struct Object Tank_beam = {0,1,{319,239},{319,239},{319,239},{10,10},
					GREEN, 3, 10, 0, 0, {1,1}};



void Game_Init(void)
{
	Lcd_Clr_Screen(BACK_COLOR);
	Timer0_Repeat(20);
}


void Galaxy_Maestro(void)
{
	Game_Init();
	Draw_Object();

	for(;;)
	{
		Move_Object();
		collision_detect();
		Draw_Object();
	}
}




/* ============================================
 * 이 곳에는 객체의 행위(멤버값 갱신 등)와 관련된 코드를 작성한다..
 * ============================================
*/
void Move_Object(void)
{
	int key = 0;
	if(Timer0_Check_Expired())
	{
		Ufo.timer++;
		Tank_beam.timer++;

		key = Key_Get_Pressed();
		Move_Tank_Depend_On_Key(key);


		if(Ufo.timer >= Ufo.speed_step)
		{
			Ufo.timer = 0;
			Ufo.pos_back[Y] = Ufo.pos[Y];
			Ufo.pos[Y] = Ufo.pos[Y] - Ufo.move_step;
			Ufo.move_flag = 1;
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
			Tank_beam.pos_init[X] = Tank.pos[X]+13;
			Tank_beam.pos_init[Y] = Tank.pos[Y]+12;
			Tank_beam.pos_back[X] = Tank_beam.pos_init[X];
			Tank_beam.pos_back[Y] = Tank_beam.pos_init[Y];
			Tank_beam.pos[X] = Tank_beam.pos_init[X];
			Tank_beam.pos[Y] = Tank_beam.pos_init[Y];
		}
		if(Tank_beam.beam_flag != 0 && Tank_beam.timer >= Tank_beam.speed_step)
		{
			Tank_beam.timer = 0;
			Tank_beam.pos_back[Y] = Tank_beam.pos[Y];
			Tank_beam.pos[Y] = Tank_beam.pos[Y] + Tank_beam.move_step;
			Tank_beam.move_flag = 1;
		}

//		Uart_Printf("Tank.timer %d / Tank.speed_step %d\n", Tank.timer, Tank.speed_step);
	}
}


void collision_detect(void)
{
	if(Tank_beam.beam_flag != 0 && Tank_beam.cd_flag == 0)
	{
		if((Tank_beam.pos[X] > Ufo.pos[X]) && \
		   (Tank_beam.pos[X] + Tank_beam.size[X] < Ufo.pos[X] + Ufo.size[X]) )
		{
			if((Tank_beam.pos[Y] + Tank_beam.size[Y]  >= Ufo.pos[Y]))
			{

				Tank_beam.move_flag = 1;
				Ufo.move_flag = 1;

				Ufo.timer = 0;
				Tank_beam.timer = 0;

				Tank_beam.cd_flag = 1;
				Ufo.cd_flag = 1;
			}
		}
	}
}


void Move_Tank_Depend_On_Key(int key)
{
	// 입력은 오직 1~4까지만 받는다.
	if(key < 1 || key > 4) return;
	// 탱크를 위로 움직임.
	if(key == 1)
	{
		Tank.pos_back[Y] = Tank.pos[Y];			 // 현재의 위치를 벡업해 놓는다.
		Tank.pos[Y] = Tank.pos[Y] - Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
		Tank.move_flag = 1;						 //
	}
	// 탱크를 좌측으로 움직임.
	if(key == 2)
	{
		Tank.pos_back[X] = Tank.pos[X];			 // 현재의 위치를 벡업해 놓는다.
		Tank.pos[X] = Tank.pos[X] - Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
		Tank.move_flag = 1;						 //
	}
	// 탱크를 위로 움직임.
	if(key == 3)
	{
		Tank.pos_back[Y] = Tank.pos[Y];			 // 현재의 위치를 벡업해 놓는다.
		Tank.pos[Y] = Tank.pos[Y] + Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
		Tank.move_flag = 1;						 //
	}
	// 탱크를 우측으로 움직임.
	if(key == 4)
	{

		Tank.pos_back[X] = Tank.pos[X];			 // 현재의 위치를 벡업해 놓는다.
		Tank.pos[X] = Tank.pos[X] + Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
		Tank.move_flag = 1;						 //
	}
}




/* ============================================
 * 이 곳에는 화면을 그리는 곳과 관련된 코드를 작성한다.
 * ============================================
*/

void Draw_Object(void)
{
	if((Tank.pos[X] >= WIDTH))
	{
		Tank.pos[X] = Tank.pos_init[X];
	}

	if((Ufo.pos[Y] < 0))
	{
		Ufo.pos[Y] = Ufo.pos_init[Y];
		Lcd_Draw_Bar(Ufo.pos_back[X], 0, Ufo.pos_back[X] + Ufo.size[X], 20, BACK_COLOR);
	}
	if((Tank_beam.pos[Y] > 239))
	{
		Tank_beam.beam_flag = 0;
	}

	if(Tank.move_flag != 0) // 이미지가 움직였으면, 새로운 곳을 그리고, 그 이전 좌표는 지운다.
	{
		Lcd_Draw_Bar(Tank.pos_back[X], Tank.pos_back[Y], Tank.pos_back[X] + Tank.size[X], Tank.pos_back[Y] + Tank.size[Y], BACK_COLOR);
		Lcd_Draw_Bar(Tank.pos[X], Tank.pos[Y], Tank.pos[X] + Tank.size[X], Tank.pos[Y] + Tank.size[Y], Tank.color);
		Tank.move_flag = 0;
	}
	if(Ufo.move_flag != 0)
	{
		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y], Ufo.pos_back[X] + Ufo.size[X], Ufo.pos_back[Y] + Ufo.size[Y], BACK_COLOR);
		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y], Ufo.pos[X] + Ufo.size[X], Ufo.pos[Y] - Ufo.size[Y], Ufo.color);
		Ufo.move_flag = 0;
	}
	if(Tank_beam.move_flag != 0)
	{
		Lcd_Draw_Bar(Tank_beam.pos_back[X], Tank_beam.pos_back[Y], Tank_beam.pos_back[X] + Tank_beam.size[X], Tank_beam.pos_back[Y] + Tank_beam.size[Y], BACK_COLOR);
		Lcd_Draw_Bar(Tank_beam.pos[X], Tank_beam.pos[Y], Tank_beam.pos[X] + Tank_beam.size[X], Tank_beam.pos[Y] + Tank_beam.size[Y], Tank_beam.color);
		Tank_beam.move_flag = 0;
	}
	explosion();
	if(Tank_beam.cd_flag == 1)
	{
		Lcd_Draw_Bar(Tank_beam.pos[X], Tank_beam.pos[Y], Tank_beam.pos[X] + Tank_beam.size[X], Tank_beam.pos[Y] + Tank_beam.size[Y], BACK_COLOR);
		Tank_beam.cd_flag = 0;
		Tank_beam.beam_flag = 0;
	}
	if(Ufo.cd_flag == 1)
	{
		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y],
				Ufo.pos[X] + Ufo.size[X],
				Ufo.pos[Y] + Ufo.size[Y],
				BACK_COLOR);
		Ufo.cd_flag = 0;
		Ufo.pos[Y] = Ufo.pos_init[Y];
	}
}

void explosion(void)
{
	if(Tank_beam.cd_flag == 1 || Ufo.cd_flag == 1)
	{
		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], WHITE);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], BLUE);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], RED);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], BLACK);
	}
}
