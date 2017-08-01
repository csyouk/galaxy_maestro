#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>
#include "constants.h"

#define BG_COLOR 	BLACK
#define X 0
#define Y 1

#define DEFAULT 0
#define NOT_FIRED 0
#define FIRED 1
#define ZERO 0
#define NOT_MOVED 0
#define MOVED 1
#define OBJECT_NOT_CRASHED 0
#define OBJECT_CRASHED 1
#define NOT_PRESSED_YET -1

#define printf 	Uart_Printf



void Game_Init(void);
void Check_Explosion(void);
void Update_Object(void);
void collision_detect(void);
void Draw_Object(void);
void Galaxy_Maestro(void);
void Update_Tank(int);
void Update_Ufo(void);
void Update_Missiles(int);
void Draw_Tank(void);
void Draw_Missiles(void);
void Draw_Ufo(void);
void Draw_Missile_Crashed(void);
void Draw_Ufo_Crashed(void);



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
	int key = DEFAULT;
	if(Timer0_Check_Expired())
	{
		// DO NOT CHANGE PROCEDURE ORDER!!!!
		key = Key_Get_Pressed();
		Update_Ufo();
		Update_Tank(key);
		Update_Missiles(key);
	}
}


void collision_detect(void)
{
	int i;
	for(i = 0; i < sizeof(Missiles)/sizeof(Missiles[0]); i++){
		if(Missiles[i].missile_flag != NOT_FIRED &&
				Missiles[i].cd_flag == OBJECT_NOT_CRASHED)
		{
			if(
				(Missiles[i].pos[X] + MISSILE_WIDTH/2 > Ufo.pos[X]) &&
				(Missiles[i].pos[X] + MISSILE_WIDTH/2 < Ufo.pos[X]+Ufo.size[X]) &&
				(Missiles[i].pos[Y] + MISSILE_HEIGHT/2 > Ufo.pos[Y] - Ufo.size[Y]) &&
				(Missiles[i].pos[Y] + MISSILE_HEIGHT/2 < Ufo.pos[Y] + Ufo.size[Y] - Ufo.size[Y])
			  )
			{
					Missiles[i].move_flag = MOVED;
					Ufo.move_flag = MOVED;

					Ufo.timer = ZERO;
					Missiles[i].timer = ZERO;

					Missiles[i].cd_flag = OBJECT_CRASHED;
					Ufo.cd_flag = OBJECT_CRASHED;

					score++;
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

	Tank.pos_back[X] = Tank.pos[X];
	Tank.pos_back[Y] = Tank.pos[Y];
	Tank.dir = _direction;
	Tank.move_flag = MOVED;

	switch(Tank.dir){
		case UP:
			Tank.pos[Y] = Tank.pos[Y] - Tank.move_step; // move_step
			break;
		case LEFT:
			Tank.pos[X] = Tank.pos[X] - Tank.move_step; // move_step
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


void Update_Missiles(int key)
{
	int i;
	key_seq.cur = key;
	int _is_missile_moving = NOT_MOVED;

	for(i=0; i<sizeof(Missiles)/sizeof(Missiles[0]); i++){
		if(!Missiles[i].missile_flag) Missiles[i].dir = Tank.dir;
		Missiles[i].timer++;
	}

	if(key_seq.pre == NOT_PRESSED_YET || key_seq.pre != key_seq.cur){
		if(key == FIRE && Tank.fired_cnt < sizeof(Missiles)/sizeof(Missiles[0]) + 1) {
			if(Tank.fired_cnt > sizeof(Missiles)/sizeof(Missiles[0])) {
				Tank.fired_cnt = sizeof(Missiles)/sizeof(Missiles[0]);
			}
			if(Missiles[Tank.fired_cnt].missile_flag == NOT_FIRED){
				Missiles[Tank.fired_cnt].missile_flag = FIRED;

				Missiles[Tank.fired_cnt].pos_init[X] = Tank.pos[X];
				Missiles[Tank.fired_cnt].pos_init[Y] = Tank.pos[Y];

				Missiles[Tank.fired_cnt].pos_back[X] = Missiles[Tank.fired_cnt].pos_init[X];
				Missiles[Tank.fired_cnt].pos_back[Y] = Missiles[Tank.fired_cnt].pos_init[Y];

				Missiles[Tank.fired_cnt].pos[X] = Missiles[Tank.fired_cnt].pos_init[X];
				Missiles[Tank.fired_cnt].pos[Y] = Missiles[Tank.fired_cnt].pos_init[Y];
				Tank.fired_cnt++;
			}
		}
		key_seq.pre = key_seq.cur;
	}


	for(i=0; i < sizeof(Missiles)/sizeof(Missiles[0]); i++){
		_is_missile_moving = (Missiles[i].missile_flag != NOT_FIRED && Missiles[i].timer >= Missiles[i].speed_step);

		if(_is_missile_moving)
		{
//			Uart_Printf("moving\n");
			Missiles[i].timer = ZERO;
			Missiles[i].move_flag = MOVED;

			Missiles[i].pos_back[X] = Missiles[i].pos[X];			 // ������ ��ġ�� ������ ���´�.
			Missiles[i].pos_back[Y] = Missiles[i].pos[Y];			 // ������ ��ġ�� ������ ���´�.

			switch(Missiles[i].dir){
				case UP:
					Missiles[i].pos[Y] = Missiles[i].pos[Y] - Missiles[i].move_step;
					break;
				case LEFT:
					Missiles[i].pos[X] = Missiles[i].pos[X] - Missiles[i].move_step;
					break;
				case DOWN:
					Missiles[i].pos[Y] = Missiles[i].pos[Y] + Missiles[i].move_step;
					break;
				case RIGHT:
					Missiles[i].pos[X] = Missiles[i].pos[X] + Missiles[i].move_step;
					break;
			}
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
	Draw_Missiles();
	Check_Explosion();
	Draw_Missile_Crashed();
	Draw_Ufo_Crashed();
}

void Check_Explosion(void)
{
	int i;
	for(i=0; i < sizeof(Missiles)/sizeof(Missiles[0]); i++){
		if(Missiles[i].cd_flag == OBJECT_CRASHED || Ufo.cd_flag == OBJECT_CRASHED)
		{
			Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], WHITE);
			Timer4_Delay(100);
			Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], BLUE);
			Timer4_Delay(100);
			Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], RED);
			Timer4_Delay(100);
			Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], BLACK);
			Tank.fired_cnt--;
			break;
		}
	}
}


void Draw_Tank(void)
{
	// draw tank - edge cases
	// if tank position exceed window then block
	if((Tank.pos[X] > W_X_MAX - TANK_WIDTH - 1) ||
	   (Tank.pos[X] < W_X_MIN)){
		Tank.pos[X] = Tank.pos_back[X];
	}

	if((Tank.pos[Y] > W_X_MAX - TANK_HEIGHT) ||
	   (Tank.pos[Y] < W_Y_MIN)	){
		Tank.pos[Y] = Tank.pos_back[Y];
	}

	// draw tank
	// if tank moved, then flag will be enabled. after enabled, updated.
	if(Tank.move_flag != NOT_MOVED)
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
	if(Ufo.pos[Y] < W_Y_MIN)
	{
		Ufo.pos[Y] = Ufo.pos_init[Y];
		Lcd_Draw_Bar(Ufo.pos_back[X], 0,
					 Ufo.pos_back[X] + Ufo.size[X], 20,
					 BG_COLOR);
	}

	// draw Ufo
	// if tank moved, then flag will be enabled. after enabled, updated.
	if(Ufo.move_flag != NOT_MOVED)
	{
		// remove previous state in lcd
		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y],
				     Ufo.pos_back[X] + Ufo.size[X], Ufo.pos_back[Y] + Ufo.size[Y],
				     BG_COLOR);
		// draw current state in lcd
		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y],
					 Ufo.pos[X] + Ufo.size[X], Ufo.pos[Y] - Ufo.size[Y],
					 Ufo.color);
		// set to ufo flag not moved.
		Ufo.move_flag = NOT_MOVED;
	}
}

void Draw_Missiles()
{
	int i;
	for(i=0; i<5; i++){
		// draw tank beam - edge case
		// if fired beam off from the window, then set the tank beam member flag as not fired.
		if((Missiles[i].pos[Y] > W_Y_MAX - 1) ||
			(Missiles[i].pos[Y] < W_Y_MIN - MISSILE_HEIGHT + 1) ||
			(Missiles[i].pos[X] < W_X_MIN + 1) ||
			(Missiles[i].pos[X] > W_X_MAX - MISSILE_WIDTH - 1))
		{

			if(Missiles[i].missile_flag == FIRED){
				Tank.fired_cnt--;
				Uart_Printf("cnt : %d\n", Tank.fired_cnt);
			}
			Missiles[i].missile_flag = NOT_FIRED;
			Missiles[i].dir = Tank.dir;

			if(Tank.fired_cnt < 0) Tank.fired_cnt=0;

			Lcd_Draw_Bar(
					Missiles[i].pos_back[X], Missiles[i].pos_back[Y],
					Missiles[i].pos_back[X] + Missiles[i].size[X], Missiles[i].pos_back[Y] + Missiles[i].size[Y],
					BG_COLOR);
			// draw current state in lcd
			Lcd_Draw_Bar(
					Missiles[i].pos[X], Missiles[i].pos[Y],
					Missiles[i].pos[X] + Missiles[i].size[X], Missiles[i].pos[Y] + Missiles[i].size[Y],
					BG_COLOR);
		}


		// draw tank beam
		// if tank beam moved, then flag will be enabled. after enabled, updated.
		if(Missiles[i].move_flag != NOT_MOVED)
		{
			// remove previous state in lcd
			Lcd_Draw_Bar(
					Missiles[i].pos_back[X], Missiles[i].pos_back[Y],
					Missiles[i].pos_back[X] + Missiles[i].size[X], Missiles[i].pos_back[Y] + Missiles[i].size[Y],
					BG_COLOR);
			// draw current state in lcd
			Lcd_Draw_Bar(
					Missiles[i].pos[X], Missiles[i].pos[Y],
					Missiles[i].pos[X] + Missiles[i].size[X], Missiles[i].pos[Y] + Missiles[i].size[Y],
					Missiles[i].color);
			// set to tank beam flag not moved.
			Missiles[i].move_flag = NOT_MOVED;
		}
	}
}


void Draw_Missile_Crashed(void)
{
	int i;
	for(i=0; i<sizeof(Missiles)/sizeof(Missiles[0]); i++){
		// draw tank beam object when crashed.
		if(Missiles[i].cd_flag == OBJECT_CRASHED)
		{
			// remove tank beam object, set color to black.
			Lcd_Draw_Bar(Missiles[i].pos[X],
					 	 Missiles[i].pos[Y],
						 Missiles[i].pos[X] + Missiles[i].size[X],
						 Missiles[i].pos[Y] + Missiles[i].size[Y],
						 BG_COLOR
					 );
			// reverse state
			Missiles[i].cd_flag = OBJECT_NOT_CRASHED;
			// set tank beam as not fired
			Missiles[i].missile_flag = NOT_FIRED;
		}

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
	Uart_Printf("==================tank===============\n");
	Uart_Printf("Timer : %d\n", Tank.timer);
	Uart_Printf("move_flag : %d\n", Tank.move_flag);
	Uart_Printf("pos[X] %d / pos[Y] %d\n", Tank.pos[X], Tank.pos[Y]);
	Uart_Printf("pos_back[X] %d / pos_back[Y] %d\n", Tank.pos_back[X], Tank.pos_back[Y]);
	Uart_Printf("size[X] %d / size[Y] %d\n", Tank.size[X], Tank.size[Y]);
	Uart_Printf("tank direction %d\n", Tank.dir);
	Uart_Printf("==================tank===============\n");
}
void print_ufo(void){
	Uart_Printf("==================ufo===============\n");
	Uart_Printf("Ufo Timer : %d\n", Ufo.timer);
	Uart_Printf("Ufo move_flag : %d\n", Ufo.move_flag);
	Uart_Printf("Ufo pos[X] %d / pos[Y] %d\n", Ufo.pos[X], Ufo.pos[Y]);
	Uart_Printf("Ufo pos_back[X] %d / pos_back[Y] %d\n", Ufo.pos_back[X], Ufo.pos_back[Y]);
	Uart_Printf("Ufo size[X] %d / size[Y] %d\n", Ufo.size[X], Ufo.size[Y]);
	Uart_Printf("==================ufo===============\n");
}

void print_missile(int i){
	Uart_Printf("==================missile===============\n");
	Uart_Printf("Missiles[%d] Timer : %d\n", i, Missiles[i].timer);
	Uart_Printf("Missiles[%d] move_flag : %d\n", i, Missiles[i].move_flag);
	Uart_Printf("Missiles[%d] pos[X] %d / pos[Y] %d\n", i, Missiles[i].pos[X], Missiles[i].pos[Y]);
	Uart_Printf("Missiles[%d] pos_back[X] %d / pos_back[Y] %d\n", i, Missiles[i].pos_back[X], Missiles[i].pos_back[Y]);
	Uart_Printf("Missiles[%d] size[X] %d / size[Y] %d\n", i, Missiles[i].size[X], Missiles[i].size[Y]);
	Uart_Printf("Missiles[%d] dir : %d\n", i, Missiles[i].dir);
	Uart_Printf("==================missile===============\n");

}

