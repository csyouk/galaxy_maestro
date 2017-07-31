#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>

#define BG_COLOR 	BLACK
#define WINDOW_WIDTH 		320
#define WINDOW_HEIGHT 		240
#define X 0
#define Y 1
#define X_COMMA_Y 2
#define DEFAULT 0
#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4
#define FIRE 5
#define NOT_FIRED 0
#define FIRED 1
#define ZERO 0
#define NOT_MOVED 0
#define MOVED 1
#define OBJECT_NOT_CRASHED 0
#define OBJECT_CRASHED 1

/*
terms about window edges
|-------1-----|
|             |
2             4
|             |
|-------3-----|

line 1 : W_
line 2 : W_
line 3 : W_
line 4 : W_

*/


void Game_Init(void);
void Check_Explosion(void);
void Update_Object(void);
void collision_detect(void);
void Draw_Object(void);
void Galaxy_Maestro(void);
void Update_Tank_Depend_On_Key(int);
void Update_Ufo(void);
void Update_TankBeam(int);
void Draw_Tank(void);
void Draw_Tank_Beam(void);
void Draw_Ufo(void);
void Draw_Tank_Beam_Crashed(void);
void Draw_Ufo_Crashed(void);

struct Object
{
	int timer;     	 	  // 몇번 타이머를 쓸 것인지?
	int move_flag;   	  // 움직이고 있는 상태인지 아니지,
	int pos[X_COMMA_Y]; 		  // x,y
	int pos_init[X_COMMA_Y];  	  // 초기 좌표.
	int pos_back[X_COMMA_Y];      // 이전 좌표의 정보. 이미지를 사용할 시, 이 좌표를 토대로 이전의 이미지를 지워야함.
	int size[X_COMMA_Y];	      // width, height
	unsigned short color; // 나중에 이미지로 대체.
	int speed_step;	 	  // 이미지가 얼마나 빨리 이동되게 할 것인지.
	int move_step;		  // 이미지를 얼마나 이동시킬 것인가?
	int beam_flag;		  // beam 발사 됐는지 여부 flag
	int cd_flag;		  // collision detection flag
	int dir[X_COMMA_Y];           // x,y 방향의 방향정보. 1과 -1 값이 있다.
	int missile_dir;      // 1,2,3,4번 키를 누름에 따라 미사일의 방향이 정해진다. 차례대로, up, left, down, right
};

/* 5:5:5:I Color Definition */

enum Color{
	BLACK=0x0000,
	WHITE=0xfffe,
	BLUE=0x003e,
	GREEN=0x07c0,
	RED=0xf800
};

struct Object Tank = {
	0,
	1,
	{0,10},
	{0,10},
	{0,10},
	{30,10},
	RED,
	4,
	8,
	0,
	0,
	{1,1},
	1
};
struct Object Ufo = {
	0,
	1,
	{160,300},
	{160,300},
	{160,300},
	{30,30},
	BLUE,
	4,
	7,
	0,
	0,
	{1,1}
};
struct Object Tank_beam = {
	0,
	1,
	{319,239},
	{319,239},
	{319,239},
	{10,10},
	GREEN,
	3,
	10,
	0,
	0,
	{1,1}
};



void Game_Init(void)
{
	Lcd_Clr_Screen(BG_COLOR);
	Timer0_Repeat(20);
}


void Galaxy_Maestro(void)
{
	Game_Init();
	Draw_Object();

	for(;;)
	{
		Update_Object();
		collision_detect();
		Draw_Object();
	}
}




/* ============================================
* 객체들의 상태 정보를 업데이트 시킨다.
* ============================================
*/
void Update_Object(void)
{
	int key = DEFAULT;
	if(Timer0_Check_Expired())
	{
		// DO NOT CHANGE PROCEDURE ORDER!!!!
		key = Key_Get_Pressed();
		Update_Ufo();
		Update_Tank_Depend_On_Key(key);
		Update_TankBeam(key);
	}
}


void collision_detect(void)
{
	if(Tank_beam.beam_flag != NOT_FIRED &&
	   Tank_beam.cd_flag == OBJECT_NOT_CRASHED)
	{
		if((Tank_beam.pos[X] > Ufo.pos[X]) && \
		(Tank_beam.pos[X] + Tank_beam.size[X] < Ufo.pos[X] + Ufo.size[X]) )
		{
			if((Tank_beam.pos[Y] + Tank_beam.size[Y]  >= Ufo.pos[Y]))
			{
				Tank_beam.move_flag = MOVED;
				Ufo.move_flag = MOVED;

				Ufo.timer = ZERO;
				Tank_beam.timer = ZERO;

				Tank_beam.cd_flag = OBJECT_CRASHED;
				Ufo.cd_flag = OBJECT_CRASHED;
			}
		}
	}
}

void Update_Ufo(void)
{
	Ufo.timer++;
	if(Ufo.timer >= Ufo.speed_step)
	{
		Ufo.timer = ZERO;
		Ufo.pos_back[Y] = Ufo.pos[Y];
		Ufo.pos[Y] = Ufo.pos[Y] - Ufo.move_step;
		Ufo.move_flag = MOVED;
	}
}


void Update_Tank_Depend_On_Key(int key)
{
	// only key input 1~4 can pass.
	if(key < UP || key > RIGHT) return;
	// ��ũ�� ���� ������.
	if(key == UP)
	{
		Tank.pos_back[Y] = Tank.pos[Y];			 // ������ ��ġ�� ������ ���´�.
		Tank.pos[Y] = Tank.pos[Y] - Tank.move_step; // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
		Tank.move_flag = MOVED;						 //
	}
	// ��ũ�� �������� ������.
	if(key == LEFT)
	{
		Tank.pos_back[X] = Tank.pos[X];			 // ������ ��ġ�� ������ ���´�.
		Tank.pos[X] = Tank.pos[X] - Tank.move_step; // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
		Tank.move_flag = MOVED;						 //
	}
	// ��ũ�� ���� ������.
	if(key == DOWN)
	{
		Tank.pos_back[Y] = Tank.pos[Y];			 // ������ ��ġ�� ������ ���´�.
		Tank.pos[Y] = Tank.pos[Y] + Tank.move_step; // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
		Tank.move_flag = MOVED;						 //
	}
	// ��ũ�� �������� ������.
	if(key == RIGHT)
	{

		Tank.pos_back[X] = Tank.pos[X];			 // ������ ��ġ�� ������ ���´�.
		Tank.pos[X] = Tank.pos[X] + Tank.move_step; // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
		Tank.move_flag = MOVED;						 //
	}
}


void Update_TankBeam(int key)
{
	Tank_beam.timer++;
	if(key == FIRE)
	{
		Tank_beam.beam_flag = FIRED;
		// useless if statement
		// if(Tank_beam.beam_flag == NOT_FIRED)
		// {
		// 	Tank_beam.beam_flag = FIRED;
		// }
	}
	if(Tank_beam.beam_flag == NOT_FIRED)
	{
		Tank_beam.pos_init[X] = Tank.pos[X] + 13;
		Tank_beam.pos_init[Y] = Tank.pos[Y] + 12;
		Tank_beam.pos_back[X] = Tank_beam.pos_init[X];
		Tank_beam.pos_back[Y] = Tank_beam.pos_init[Y];
		Tank_beam.pos[X] = Tank_beam.pos_init[X];
		Tank_beam.pos[Y] = Tank_beam.pos_init[Y];
	}
	if(Tank_beam.beam_flag != NOT_FIRED && Tank_beam.timer >= Tank_beam.speed_step)
	{
		Tank_beam.timer = ZERO;
		Tank_beam.pos_back[Y] = Tank_beam.pos[Y];
		Tank_beam.pos[Y] = Tank_beam.pos[Y] + Tank_beam.move_step;
		Tank_beam.move_flag = MOVED;
	}
}

/*
* ============================================
* draw objects.
* ============================================
*/

void Draw_Object(void)
{
	// DO NOT CHANGE PROCEDURE ORDER!!!!
	Draw_Tank();
	Draw_Ufo();
	Draw_Tank_Beam();
	Check_Explosion();
	Draw_Tank_Beam_Crashed();
	Draw_Ufo_Crashed();
}

void Check_Explosion(void)
{
	if(Tank_beam.cd_flag == OBJECT_CRASHED || Ufo.cd_flag == OBJECT_CRASHED)
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


void Draw_Tank(void)
{
	// draw tank - edge case
	// if tank position exceed window width, then set to beginning
	if((Tank.pos[X] >= WINDOW_WIDTH))
	{
		Tank.pos[X] = Tank.pos_init[X];
	}
	// draw tank
	// if tank moved, then flag will be enabled. after enabled, updated.
	if(Tank.move_flag != NOT_MOVED) // �̹����� ����������, ���ο� ���� �׸���, �� ���� ��ǥ�� ������.
	{
		// remove previous state in lcd
		Lcd_Draw_Bar(Tank.pos_back[X], Tank.pos_back[Y], Tank.pos_back[X] + Tank.size[X], Tank.pos_back[Y] + Tank.size[Y], BG_COLOR);
		// draw current state in lcd
		Lcd_Draw_Bar(Tank.pos[X], Tank.pos[Y], Tank.pos[X] + Tank.size[X], Tank.pos[Y] + Tank.size[Y], Tank.color);
		// set to tank flag not moved.
		Tank.move_flag = NOT_MOVED;
	}
}


void Draw_Ufo(void)
{
	// draw ufo - edge case
	// if ufo position off from the window, then set to beginning
	if((Ufo.pos[Y] < 0))
	{
		Ufo.pos[Y] = Ufo.pos_init[Y];
		Lcd_Draw_Bar(Ufo.pos_back[X], 0, Ufo.pos_back[X] + Ufo.size[X], 20, BG_COLOR);
	}


	// draw Ufo
	// if tank moved, then flag will be enabled. after enabled, updated.
	if(Ufo.move_flag != NOT_MOVED)
	{
		// remove previous state in lcd
		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y], Ufo.pos_back[X] + Ufo.size[X], Ufo.pos_back[Y] + Ufo.size[Y], BG_COLOR);
		// draw current state in lcd
		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y], Ufo.pos[X] + Ufo.size[X], Ufo.pos[Y] - Ufo.size[Y], Ufo.color);
		// set to ufo flag not moved.
		Ufo.move_flag = NOT_MOVED;
	}
}

void Draw_Tank_Beam()
{
	// draw tank beam - edge case
	// if fired beam off from the window, then set the tank beam member flag as not fired.
	if((Tank_beam.pos[Y] > WINDOW_HEIGHT - 1))
	{
		Tank_beam.beam_flag = NOT_FIRED;
	}

	// draw tank beam
	// if tank beam moved, then flag will be enabled. after enabled, updated.
	if(Tank_beam.move_flag != NOT_MOVED)
	{
		// remove previous state in lcd
		Lcd_Draw_Bar(Tank_beam.pos_back[X], Tank_beam.pos_back[Y], Tank_beam.pos_back[X] + Tank_beam.size[X], Tank_beam.pos_back[Y] + Tank_beam.size[Y], BG_COLOR);
		// draw current state in lcd
		Lcd_Draw_Bar(Tank_beam.pos[X], Tank_beam.pos[Y], Tank_beam.pos[X] + Tank_beam.size[X], Tank_beam.pos[Y] + Tank_beam.size[Y], Tank_beam.color);
		// set to tank beam flag not moved.
		Tank_beam.move_flag = NOT_MOVED;
	}
}

void Draw_Tank_Beam_Crashed(void)
{
	// draw tank beam object when crashed.
	if(Tank_beam.cd_flag == OBJECT_CRASHED)
	{
		// remove tank beam object, set color to black.
		Lcd_Draw_Bar(Tank_beam.pos[X],
								 Tank_beam.pos[Y],
								 Tank_beam.pos[X] + Tank_beam.size[X],
								 Tank_beam.pos[Y] + Tank_beam.size[Y],
								 BG_COLOR
							 );
		// reverse state
		Tank_beam.cd_flag = OBJECT_NOT_CRASHED;
		// set tank beam as not fired
		Tank_beam.beam_flag = NOT_FIRED;
	}
}
void Draw_Ufo_Crashed(void)
{
	// draw ufo object when crashed.
	if(Ufo.cd_flag == OBJECT_CRASHED)
	{
		// remove ufo object, set color to black.
		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y],
								 Ufo.pos[X] + Ufo.size[X],
								 Ufo.pos[Y] + Ufo.size[Y],
								 BG_COLOR
							 );
		// reverse state
		Ufo.cd_flag = OBJECT_NOT_CRASHED;
		// set ufo position to beginning.
		Ufo.pos[Y] = Ufo.pos_init[Y];
	}
}
