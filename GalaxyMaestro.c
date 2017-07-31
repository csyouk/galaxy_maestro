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
void explosion(void);
void Update_Object(void);
void collision_detect(void);
void Draw_Object(void);
void Galaxy_Maestro(void);
void Update_Tank_Depend_On_Key(int);
void Update_Ufo(void);
void Update_TankBeam_Depend_On_Key(int);

struct Object
{
	int timer;     	 	  // ���� Ÿ�̸Ӹ� �� ������?
	int move_flag;   	  // �����̰� �ִ� �������� �ƴ���,
	int pos[X_COMMA_Y]; 		  // x,y
	int pos_init[X_COMMA_Y];  	  // �ʱ� ��ǥ.
	int pos_back[X_COMMA_Y];      // ���� ��ǥ�� ����. �̹����� ������ ��, �� ��ǥ�� ������ ������ �̹����� ��������.
	int size[X_COMMA_Y];	      // WINDOW_WIDTH, WINDOW_HEIGHT

	unsigned short color; // ���߿� �̹����� ��ü.
	int speed_step;	 	  // �̹����� �󸶳� ���� �̵��ǰ� �� ������.
	int move_step;		  // �̹����� �󸶳� �̵���ų ���ΰ�?
	int beam_flag;		  // beam �߻� �ƴ��� ���� flag
	int cd_flag;		  // collision detection flag
	int dir[X_COMMA_Y];           // x,y ������ ��������. 1�� -1 ���� �ִ�.
	int missile_dir;      // 1,2,3,4 Ű �Է°��� ���Ⱚ���� �����Ѵ�. �� ���� �̻����� ������ �����ϴ�.
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
* �� ������ ��ü�� ����(������ ���� ��)�� ���õ� �ڵ带 �ۼ��Ѵ�..
* ============================================
*/
void Update_Object(void)
{
	int key = DEFAULT;
	if(Timer0_Check_Expired())
	{

		key = Key_Get_Pressed();
		Update_Ufo();
		Update_Tank_Depend_On_Key(key);
		Update_TankBeam_Depend_On_Key(key);

		//		Uart_Printf("Tank.timer %d / Tank.speed_step %d\n", Tank.timer, Tank.speed_step);
	}
}


void collision_detect(void)
{
	if(Tank_beam.beam_flag != NOT_FIRED && Tank_beam.cd_flag == OBJECT_NOT_CRASHED)
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
	// �Է��� ���� 1~4������ �޴´�.
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


void Update_TankBeam_Depend_On_Key(int key)
{
	Tank_beam.timer++;
	if(key == FIRE)
	{
		if(Tank_beam.beam_flag == NOT_FIRED)
		{
			Tank_beam.beam_flag = FIRED;
		}
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

/* ============================================
* �� ������ ȭ���� �׸��� ���� ���õ� �ڵ带 �ۼ��Ѵ�.
* ============================================
*/

void Draw_Object(void)
{
	if((Tank.pos[X] >= WINDOW_WIDTH))
	{
		Tank.pos[X] = Tank.pos_init[X];
	}

	if((Ufo.pos[Y] < 0))
	{
		Ufo.pos[Y] = Ufo.pos_init[Y];
		Lcd_Draw_Bar(Ufo.pos_back[X], 0, Ufo.pos_back[X] + Ufo.size[X], 20, BG_COLOR);
	}
	if((Tank_beam.pos[Y] > WINDOW_HEIGHT - 1))
	{
		Tank_beam.beam_flag = NOT_FIRED;
	}

	if(Tank.move_flag != NOT_MOVED) // �̹����� ����������, ���ο� ���� �׸���, �� ���� ��ǥ�� ������.
	{
		Lcd_Draw_Bar(Tank.pos_back[X], Tank.pos_back[Y], Tank.pos_back[X] + Tank.size[X], Tank.pos_back[Y] + Tank.size[Y], BG_COLOR);
		Lcd_Draw_Bar(Tank.pos[X], Tank.pos[Y], Tank.pos[X] + Tank.size[X], Tank.pos[Y] + Tank.size[Y], Tank.color);
		Tank.move_flag = NOT_MOVED;
	}
	if(Ufo.move_flag != NOT_MOVED)
	{
		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y], Ufo.pos_back[X] + Ufo.size[X], Ufo.pos_back[Y] + Ufo.size[Y], BG_COLOR);
		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y], Ufo.pos[X] + Ufo.size[X], Ufo.pos[Y] - Ufo.size[Y], Ufo.color);
		Ufo.move_flag = NOT_MOVED;
	}
	if(Tank_beam.move_flag != NOT_MOVED)
	{
		Lcd_Draw_Bar(Tank_beam.pos_back[X], Tank_beam.pos_back[Y], Tank_beam.pos_back[X] + Tank_beam.size[X], Tank_beam.pos_back[Y] + Tank_beam.size[Y], BG_COLOR);
		Lcd_Draw_Bar(Tank_beam.pos[X], Tank_beam.pos[Y], Tank_beam.pos[X] + Tank_beam.size[X], Tank_beam.pos[Y] + Tank_beam.size[Y], Tank_beam.color);
		Tank_beam.move_flag = NOT_MOVED;
	}
	explosion();
	if(Tank_beam.cd_flag == OBJECT_CRASHED)
	{
		Lcd_Draw_Bar(Tank_beam.pos[X],
								 Tank_beam.pos[Y],
								 Tank_beam.pos[X] + Tank_beam.size[X],
								 Tank_beam.pos[Y] + Tank_beam.size[Y],
								 BG_COLOR
							 );
		Tank_beam.cd_flag = OBJECT_NOT_CRASHED;
		Tank_beam.beam_flag = NOT_FIRED;
	}
	if(Ufo.cd_flag == OBJECT_CRASHED)
	{
		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y],
								 Ufo.pos[X] + Ufo.size[X],
								 Ufo.pos[Y] + Ufo.size[Y],
								 BG_COLOR
							 );
			Ufo.cd_flag = OBJECT_NOT_CRASHED;
			Ufo.pos[Y] = Ufo.pos_init[Y];
	}
}

void explosion(void)
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
