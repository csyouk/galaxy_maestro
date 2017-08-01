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
#define NOT_FIRED 0
#define FIRED 1
#define ZERO 0
#define NOT_MOVED 0
#define MOVED 1
#define OBJECT_NOT_CRASHED 0
#define OBJECT_CRASHED 1
#define UNLOCK 1



/*
terms about window edges
 ||       || ||
|------1------|
|             |
2             4
|             |
|------3------|

line 1 : W_X_MIN
line 2 : W_Y_MIN
line 3 : W_X_MAX
line 4 : W_Y_MAX

*/


void Game_Init(void);
void Check_Explosion(void);
void Update_Object(void);
void collision_detect(void);
void Draw_Object(void);
void Galaxy_Maestro(void);
void Update_Tank(int);
void Update_Ufo(void);
void Update_Missile(int);
void Draw_Tank(void);
void Draw_Missile(void);
void Draw_Ufo(void);
void Draw_Missile_Crashed(void);
void Draw_Ufo_Crashed(void);
void print_tank(void);
void print_ufo(void);
void print_missile(void);

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
	int missile_flag;		  // missile 발사 됐는지 여부 flag
	int cd_flag;		  // collision detection flag
	int dir;           // 1,2,3,4번 키를 누름에 따라 미사일의 방향이 정해진다. 차례대로, up, left, down, right
};

enum Key{UP=1, LEFT, DOWN, RIGHT, FIRE};
/* 5:5:5:I Color Definition */
enum Color{
	BLACK=0x0000,
	WHITE=0xfffe,
	BLUE=0x003e,
	GREEN=0x07c0,
	RED=0xf800
};

enum WINDOW{
 W_X_MIN=0,
 W_X_MAX=320,
 W_Y_MIN=0,
 W_Y_MAX=240
};

enum TANK_DATA{
	TANK_TIMER=0,
	TANK_POS_INIT_X=0,
	TANK_POS_INIT_Y=10,
	TANK_WIDTH=16,
	TANK_HEIGHT=10,
	TANK_SPEED_RATE=5,
	TANK_FOOTSTEP=TANK_WIDTH/8,
	TANK_DIR=3
};
enum UFO_DATA{
	UFO_WIDTH=30,
	UFO_HEIGHT=30,
	UFO_TIMER=0,
	UFO_SPEED_RATE=1,
	UFO_FOOTSTEP=2,
	UFO_DIR=3
};

enum MISSILE_DATA{
	MISSILE_WIDTH=6,
	MISSILE_HEIGHT=6,
	MISSILE_TIMER=0,
	MISSILE_SPEED_RATE=1,
	MISSILE_FOOTSTEP=4,
	MISSILE_DIR=3
};

struct Object Tank = {
	TANK_TIMER,
	1,
	{0,10},  // 현재 탱크 위치.
	{TANK_POS_INIT_X,TANK_POS_INIT_Y},  // 초기화시킬 시 탱크 위치.
	{0,10},  // 탱크의 위치가 움직이게 될 때, 탱크의 위치를 벡업.
	{TANK_WIDTH,TANK_HEIGHT}, // 탱크의 크기. 가로 15, 세로 10
	RED,
	TANK_SPEED_RATE,
	TANK_FOOTSTEP,
	0,
	0,
	TANK_DIR
};

struct Object Ufo = {
	0,
	1,
	{160,300},
	{160,300},
	{160,300},
	{UFO_WIDTH,UFO_HEIGHT},
	BLUE,
	UFO_SPEED_RATE,
	UFO_FOOTSTEP,
	0,
	0,
	0
};

struct Object Missile = {
	MISSILE_TIMER,
	1,
	{319,239},
	{319,239},
	{319,239},
	{MISSILE_WIDTH,MISSILE_HEIGHT},
	GREEN,
	MISSILE_SPEED_RATE,
	MISSILE_FOOTSTEP,
	0,
	0,
	MISSILE_DIR
};

int lock = UNLOCK;

/* ====================================
 *  Bootstraping.
 * ====================================
 */

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
	int direction = DEFAULT;
	if(Timer0_Check_Expired())
	{
		// DO NOT CHANGE PROCEDURE ORDER!!!!
		direction = Key_Get_Pressed();
		Update_Ufo();
		Update_Tank(direction);
		Update_Missile(direction);
	}
}


void collision_detect(void)
{
	if(Missile.missile_flag != NOT_FIRED &&
	   Missile.cd_flag == OBJECT_NOT_CRASHED)
	{
		if((Missile.pos[X] > Ufo.pos[X]) && \
		(Missile.pos[X] + Missile.size[X] < Ufo.pos[X] + Ufo.size[X]))
		{
			if((Missile.pos[Y]  > Ufo.pos[Y]) && \
			(Missile.pos[Y] + Missile.size[Y] < Ufo.pos[Y] + Ufo.size[Y]))
			{
				Uart_Printf("COOOOOOOOOOOO=\n");
				print_ufo();
				print_missile();
				Missile.move_flag = MOVED;
				Ufo.move_flag = MOVED;

				Ufo.timer = ZERO;
				Missile.timer = ZERO;

				Missile.cd_flag = OBJECT_CRASHED;
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


void Update_Tank(int _direction)
{
	// only key input 1~4 can pass.
	if(_direction < UP || _direction > RIGHT) return;

	Tank.pos_back[X] = Tank.pos[X];			 // ������ ��ġ�� ������ ���´�.
	Tank.pos_back[Y] = Tank.pos[Y];			 // ������ ��ġ�� ������ ���´�.
	Tank.dir = _direction;
	Tank.move_flag = MOVED;

	switch(Tank.dir){
		case UP:
			Tank.pos[Y] = Tank.pos[Y] - Tank.move_step; // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
			break;
		case LEFT:
			Tank.pos[X] = Tank.pos[X] - Tank.move_step; // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
			break;
		case DOWN:
			Tank.pos[Y] = Tank.pos[Y] + Tank.move_step;  // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
			break;
		case RIGHT:
			Tank.pos[X] = Tank.pos[X] + Tank.move_step;  // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
			break;
		default: break;
	}
}


void Update_Missile(int _key)
{
	int _is_missile_moving = NOT_MOVED;
	if(!Missile.missile_flag) Missile.dir = Tank.dir;

	Missile.timer++;

	if(_key == FIRE) Missile.missile_flag = FIRED;

	if(Missile.missile_flag == NOT_FIRED)
	{
		Missile.pos_init[X] = Tank.pos[X] + TANK_WIDTH/2;
		Missile.pos_init[Y] = Tank.pos[Y] + 2*TANK_HEIGHT;

		Missile.pos_back[X] = Missile.pos_init[X];
		Missile.pos_back[Y] = Missile.pos_init[Y];

		Missile.pos[X] = Missile.pos_init[X];
		Missile.pos[Y] = Missile.pos_init[Y];
		return;
	}

	_is_missile_moving = (Missile.missile_flag != NOT_FIRED && Missile.timer >= Missile.speed_step);

	if(_is_missile_moving)
	{

		Missile.timer = ZERO;
		Missile.move_flag = MOVED;

		Missile.pos_back[X] = Missile.pos[X];			 // ������ ��ġ�� ������ ���´�.
		Missile.pos_back[Y] = Missile.pos[Y];			 // ������ ��ġ�� ������ ���´�.

		switch(Missile.dir){
			case UP:
				Missile.pos[Y] = Missile.pos[Y] - Missile.move_step;
				break;
			case LEFT:
				Missile.pos[X] = Missile.pos[X] - Missile.move_step;
				break;
			case DOWN:
				Missile.pos[Y] = Missile.pos[Y] + Missile.move_step;
				break;
			case RIGHT:
				Missile.pos[X] = Missile.pos[X] + Missile.move_step;
				break;
		}
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
	Draw_Missile();
	Check_Explosion();
	Draw_Missile_Crashed();
	Draw_Ufo_Crashed();
}

void Check_Explosion(void)
{
	if(Missile.cd_flag == OBJECT_CRASHED || Ufo.cd_flag == OBJECT_CRASHED)
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
	// draw tank - edge cases
	// if tank position exceed window then block
	if(Tank.pos[X] > WINDOW_WIDTH - TANK_WIDTH - 1){
		Tank.pos[X] = Tank.pos_back[X];
	}
	if(Tank.pos[X] < W_X_MIN){
		Tank.pos[X] = Tank.pos_back[X];
	}
	if(Tank.pos[Y] > WINDOW_HEIGHT - TANK_HEIGHT){
		Tank.pos[Y] = Tank.pos_back[Y];
	}
	if(Tank.pos[Y] < W_Y_MIN){
		Tank.pos[Y] = Tank.pos_back[Y];
	}

	// draw tank
	// if tank moved, then flag will be enabled. after enabled, updated.
	if(Tank.move_flag != NOT_MOVED) // �̹����� ����������, ���ο� ���� �׸���, �� ���� ��ǥ�� ������.
	{
		// remove previous state in lcd
		Lcd_Draw_Bar(Tank.pos_back[X], Tank.pos_back[Y], Tank.pos_back[X] + Tank.size[X], Tank.pos_back[Y] + Tank.size[Y], BG_COLOR);
//		Uart_Printf("remove state ==>\n");
//		print_tank();
		// draw current state in lcd
		Lcd_Draw_Bar(Tank.pos[X], Tank.pos[Y], Tank.pos[X] + Tank.size[X], Tank.pos[Y] + Tank.size[Y], Tank.color);
//		Uart_Printf("draw state ==>\n");
//		print_tank();
		// set to tank flag not moved.
		Tank.move_flag = NOT_MOVED;
	}
}


void Draw_Ufo(void)
{
	// draw ufo - edge case
	// if ufo position off from the window, then set to beginning
	if((Ufo.pos[Y] < W_Y_MIN))
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

void Draw_Missile()
{
	// draw tank beam - edge case
	// if fired beam off from the window, then set the tank beam member flag as not fired.
	if((Missile.pos[Y] > W_Y_MAX - 1))
	{
		Missile.missile_flag = NOT_FIRED;
		Missile.dir = Tank.dir;
	}
	if(Missile.pos[Y] < W_Y_MIN - MISSILE_HEIGHT + 1)
	{
		Missile.missile_flag = NOT_FIRED;
		Missile.dir = Tank.dir;
	}
	if(Missile.pos[X] < W_X_MIN - MISSILE_WIDTH)
	{
		Missile.missile_flag = NOT_FIRED;
		Missile.dir = Tank.dir;
	}
	if(Missile.pos[X] > W_X_MAX + MISSILE_WIDTH - 1)
	{
		Missile.missile_flag = NOT_FIRED;
		Missile.dir = Tank.dir;
	}


	// draw tank beam
	// if tank beam moved, then flag will be enabled. after enabled, updated.
	if(Missile.move_flag != NOT_MOVED)
	{
		// remove previous state in lcd
		Lcd_Draw_Bar(
				Missile.pos_back[X], Missile.pos_back[Y],
				Missile.pos_back[X] + Missile.size[X], Missile.pos_back[Y] + Missile.size[Y],
				BG_COLOR);
		// draw current state in lcd
		Lcd_Draw_Bar(
				Missile.pos[X], Missile.pos[Y],
				Missile.pos[X] + Missile.size[X], Missile.pos[Y] + Missile.size[Y],
				Missile.color);
		// set to tank beam flag not moved.
		Missile.move_flag = NOT_MOVED;
	}
}


void Draw_Missile_Crashed(void)
{
	// draw tank beam object when crashed.
	if(Missile.cd_flag == OBJECT_CRASHED)
	{
		// remove tank beam object, set color to black.
		Lcd_Draw_Bar(Missile.pos[X],
								 Missile.pos[Y],
								 Missile.pos[X] + Missile.size[X],
								 Missile.pos[Y] + Missile.size[Y],
								 BG_COLOR
							 );
		// reverse state
		Missile.cd_flag = OBJECT_NOT_CRASHED;
		// set tank beam as not fired
		Missile.missile_flag = NOT_FIRED;
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


/* =====================================
 * print states of objects
 * =====================================
 */
void print_tank(void){
	Uart_Printf("Timer : %d\n", Tank.timer);
	Uart_Printf("move_flag : %d\n", Tank.move_flag);
	Uart_Printf("pos[X] %d / pos[Y] %d\n", Tank.pos[X], Tank.pos[Y]);
	Uart_Printf("pos_back[X] %d / pos_back[Y] %d\n", Tank.pos_back[X], Tank.pos_back[Y]);
	Uart_Printf("size[X] %d / size[Y] %d\n", Tank.size[X], Tank.size[Y]);
	Uart_Printf("tank direction %d\n", Tank.dir);
}
void print_ufo(void){
	Uart_Printf("Ufo Timer : %d\n", Ufo.timer);
	Uart_Printf("Ufo move_flag : %d\n", Ufo.move_flag);
	Uart_Printf("Ufo pos[X] %d / pos[Y] %d\n", Ufo.pos[X], Ufo.pos[Y]);
	Uart_Printf("Ufo pos_back[X] %d / pos_back[Y] %d\n", Ufo.pos_back[X], Ufo.pos_back[Y]);
	Uart_Printf("Ufo size[X] %d / size[Y] %d\n", Ufo.size[X], Ufo.size[Y]);
}
void print_missile(void){
	Uart_Printf("Missile Timer : %d\n", Missile.timer);
	Uart_Printf("Missile move_flag : %d\n", Missile.move_flag);
	Uart_Printf("Missile pos[X] %d / pos[Y] %d\n", Missile.pos[X], Missile.pos[Y]);
	Uart_Printf("Missile pos_back[X] %d / pos_back[Y] %d\n", Missile.pos_back[X], Missile.pos_back[Y]);
	Uart_Printf("Missile size[X] %d / size[Y] %d\n", Missile.size[X], Missile.size[Y]);
	Uart_Printf("Missile dir : %d\n", Missile.dir);
}
