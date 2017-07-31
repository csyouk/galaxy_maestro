#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>

#define BACK_COLOR 	BLACK
#define WIDTH 		320
#define HEIGHT 		240

void Game_Init(void);
void explosion(void);
void Move_Object(void);
void collision_detect(void);
void Draw_Object(void);
void Galaxy_Maestro(void);

struct Object
{
	int timer;     	 	  // 몇번 타이머를 쓸 것인지?
	int move_flag;   	  // 움직이고 있는 상태인지 아니지,
	int pos[2]; 		  // x,y
	int pos_init[2];  	  // 초기 좌표.
	int pos_back[2];      // 이전 좌표의 정보. 이미지를 사용할 시, 이 좌표를 토대로 이전의 이미지를 지워야함.
	int size[2];	      // width, height

	unsigned short color; // 나중에 이미지로 대체.
	int speed_step;	 	  // 이미지가 얼마나 빨리 이동되게 할 것인지.
	int move_step;		  // 이미지를 얼마나 이동시킬 것인가?
	int beam_flag;		  // beam 발사 됐는지 여부 flag
	int cd_flag;		  // collision detection flag
	int dir[2];           // x,y 방향의 방향정보. 1과 -1 값이 있다.
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
struct Object Gun = {0,1,{160,300},{160,300},{160,300},{30,30},
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
		Gun.timer++;
		Tank_beam.timer++;

		key = Key_Get_Pressed();

		// 탱크를 위로 움직임.
		if(key == 1)
		{
			Tank.pos_back[1] = Tank.pos[1];			 // 현재의 위치를 벡업해 놓는다.
			Tank.pos[1] = Tank.pos[1] - Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
			Tank.move_flag = 1;						 //
		}
		// 탱크를 좌측으로 움직임.
		if(key == 2)
		{
			Tank.pos_back[0] = Tank.pos[0];			 // 현재의 위치를 벡업해 놓는다.
			Tank.pos[0] = Tank.pos[0] - Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
			Tank.move_flag = 1;						 //
		}
		// 탱크를 위로 움직임.
		if(key == 3)
		{
			Tank.pos_back[1] = Tank.pos[1];			 // 현재의 위치를 벡업해 놓는다.
			Tank.pos[1] = Tank.pos[1] + Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
			Tank.move_flag = 1;						 //
		}
		// 탱크를 우측으로 움직임.
		if(key == 4)
		{

			Tank.pos_back[0] = Tank.pos[0];			 // 현재의 위치를 벡업해 놓는다.
			Tank.pos[0] = Tank.pos[0] + Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
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
