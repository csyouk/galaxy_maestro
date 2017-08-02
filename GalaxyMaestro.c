#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>
#include "constants.h"

#define BG_COLOR BLACK
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

#define NO_OF_MISSILES sizeof(Missiles)/sizeof(Missiles[0])
#define NO_OF_UFOS sizeof(Ufos)/sizeof(Ufos[0])


void Init_Game(void);
void Init_Ufos(void);

void Check_Explosion(void);
void Collision_Detect(void);
void Galaxy_Maestro(void);

void Update_Object(void);
void Update_Tank(int);
void Update_Ufo(void);
void Update_Missiles(int);

void Draw_Object(void);
void Draw_Tank(void);
void Draw_Missiles(void);
void Draw_Ufo(void);
void Draw_Missile_Crashed(void);
void Draw_Ufo_Crashed(void);

void Draw_Curr_Frame(struct Object *obj);
void Draw_Curr_Mis_Tank_Frame(struct Object *obj);
void Remove_Curr_Frame(struct Object *obj);
void Remove_Prev_Frame(struct Object *obj);
void Remove_Prev_Ufo_Frame(struct Object *obj);

void print_tank(void);
void print_ufo(int);
void print_missile(int);

/* ====================================
 *  Bootstraping.
 * ====================================
 */


void Galaxy_Maestro(void)
{
	Init_Game();
	Init_Ufos();
	Draw_Object();

	for(;;)
	{
		Update_Object();
		Collision_Detect();
		Draw_Object();
	}
}

void Init_Game(void)
{
	Lcd_Clr_Screen(BG_COLOR);
	Timer0_Repeat(20);
}


void Init_Ufos(void)
{
	int i, position, dir_x, dir_y;
	for(i = ZERO; i < NO_OF_UFOS; i++){

		position = rand() % 2;
		dir_x = rand() % 2 - 2;
		dir_y = rand() % 5 - 2;
		Ufos[i].speed_step = rand() % 2 + 1;
		Ufos[i].move_step = rand() % 3 + 1;

		if(position){
			Ufos[i].pos[!position] = rand() % 320;
			Ufos[i].pos[position] = W_Y_MIN + UFO_HEIGHT;

			Ufos[i].fly_dir[!position] = (dir_x == 0)? 1 : dir_x;
			Ufos[i].fly_dir[position] = (dir_y == 0)? -1 : dir_y;
		} else {
			Ufos[i].pos[position] = rand() % 320;
			Ufos[i].pos[!position] = W_Y_MAX - UFO_HEIGHT - 1;

			Ufos[i].fly_dir[position] = (dir_x == 0)? -1 : dir_x;
			Ufos[i].fly_dir[!position] = (dir_y == 0)? 1 : dir_y;
		}
		print_ufo(i);
	}
}

/*============================================
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


void Collision_Detect(void)
{
	int i;
	// 미사일이 ufo와 충돌했는지 파악한다.
	for(i = ZERO; i < NO_OF_MISSILES; i++){
		if(Missiles[i].missile_flag != NOT_FIRED &&
		   Missiles[i].cd_flag == OBJECT_NOT_CRASHED)
		{
			if(
				(Missiles[i].pos[X] + MISSILE_WIDTH / 2 > Ufos[i].pos[X]) &&
				(Missiles[i].pos[X] + MISSILE_WIDTH / 2 < Ufos[i].pos[X] + Ufos[i].size[X]) &&
				(Missiles[i].pos[Y] + MISSILE_HEIGHT / 2 > Ufos[i].pos[Y] - Ufos[i].size[Y]) &&
				(Missiles[i].pos[Y] + MISSILE_HEIGHT / 2 < Ufos[i].pos[Y] + Ufos[i].size[Y] - Ufos[i].size[Y])
			  )
			{
				Missiles[i].move_flag = MOVED;
				Ufos[i].move_flag = MOVED;

				Ufos[i].timer = ZERO;
				Missiles[i].timer = ZERO;

				Missiles[i].cd_flag = OBJECT_CRASHED;
				Ufos[i].cd_flag = OBJECT_CRASHED;

				score++;
			}
		}
	}
}

void Update_Ufo(void)
{
	int i;
	for(i=0; i < NO_OF_UFOS; i++){
		if(Ufos[i].cd_flag == OBJECT_CRASHED) continue;
		Ufos[i].timer++;

		if(Ufos[i].timer < Ufos[i].speed_step) return;
		Ufos[i].timer = ZERO;
		Ufos[i].pos_back[X] = Ufos[i].pos[X];
		Ufos[i].pos_back[Y] = Ufos[i].pos[Y];
		Ufos[i].fly_dir_back[X] = Ufos[i].fly_dir[X];
		Ufos[i].fly_dir_back[Y] = Ufos[i].fly_dir[Y];
		Ufos[i].pos[X] += Ufos[i].fly_dir[X] * Ufos[i].move_step;
		Ufos[i].pos[Y] += Ufos[i].fly_dir[Y] * Ufos[i].move_step;
		Ufos[i].move_flag = MOVED;

		if(Ufos[i].pos[X] < W_X_MIN || Ufos[i].pos[X] > W_X_MAX - UFO_WIDTH - 1){
			Ufos[i].fly_dir[X] = -Ufos[i].fly_dir[X];
			Ufos[i].pos[X] = Ufos[i].pos_back[X];
			return;
		}

		if(Ufos[i].pos[Y] < W_Y_MIN + UFO_HEIGHT || Ufos[i].pos[Y] > W_Y_MAX){
			Ufos[i].fly_dir[Y] = -Ufos[i].fly_dir[Y];
			Ufos[i].pos[Y] = Ufos[i].pos_back[Y];
			return;
		}
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
			Tank.pos[Y] -= Tank.move_step;
			break;
		case LEFT:
			Tank.pos[X] -= Tank.move_step;
			break;
		case DOWN:
			Tank.pos[Y] += Tank.move_step;
			break;
		case RIGHT:
			Tank.pos[X] += Tank.move_step;
			break;
		default: break;
	}
}


void Update_Missiles(int key)
{
	int i;
	key_seq.cur = key;
	int _is_missile_moving = NOT_MOVED;

	for(i = ZERO; i<NO_OF_MISSILES; i++){
		if(!Missiles[i].missile_flag) Missiles[i].dir = Tank.dir;
		Missiles[i].timer++;
	}

	if(key_seq.pre == NOT_PRESSED_YET || key_seq.pre != key_seq.cur){
		if(key == FIRE && Tank.fired_cnt < NO_OF_MISSILES + 1) {
			if(Tank.fired_cnt > NO_OF_MISSILES) {
				Tank.fired_cnt = NO_OF_MISSILES;
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


	for(i = ZERO; i < NO_OF_MISSILES; i++){
		_is_missile_moving = (Missiles[i].missile_flag != NOT_FIRED && Missiles[i].timer >= Missiles[i].speed_step);

		if(_is_missile_moving)
		{
			Missiles[i].timer = ZERO;
			Missiles[i].move_flag = MOVED;

			Missiles[i].pos_back[X] = Missiles[i].pos[X];
			Missiles[i].pos_back[Y] = Missiles[i].pos[Y];

			switch(Missiles[i].dir){
				case UP:
					Missiles[i].pos[Y] -= Missiles[i].move_step;
					break;
				case LEFT:
					Missiles[i].pos[X] -= Missiles[i].move_step;
					break;
				case DOWN:
					Missiles[i].pos[Y] += Missiles[i].move_step;
					break;
				case RIGHT:
					Missiles[i].pos[X] += Missiles[i].move_step;
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
//	if(Timer0_Check_Expired()){
	// DO NOT CHANGE PROCEDURE ORDER!!!!
	Draw_Tank();
	Draw_Ufo();
	Draw_Missiles();
	Check_Explosion();
	Draw_Missile_Crashed();
	Draw_Ufo_Crashed();
//	}
}

void Check_Explosion(void)
{
	int i;
	for(i = ZERO; i < NO_OF_MISSILES; i++){
		if(Missiles[i].cd_flag == OBJECT_CRASHED || Ufos[i].cd_flag == OBJECT_CRASHED)
		{
			Lcd_Draw_Bar(Ufos[i].pos_back[X], Ufos[i].pos_back[Y]-UFO_DEST_BOUND, Ufos[i].pos_back[X] + UFO_DEST_BOUND, Ufos[i].pos_back[Y], WHITE);
			Timer4_Delay(100);
			Lcd_Draw_Bar(Ufos[i].pos_back[X], Ufos[i].pos_back[Y]-UFO_DEST_BOUND, Ufos[i].pos_back[X] + UFO_DEST_BOUND, Ufos[i].pos_back[Y], BLUE);
			Timer4_Delay(100);
			Lcd_Draw_Bar(Ufos[i].pos_back[X], Ufos[i].pos_back[Y]-UFO_DEST_BOUND, Ufos[i].pos_back[X] + UFO_DEST_BOUND, Ufos[i].pos_back[Y], RED);
			Timer4_Delay(100);
			Lcd_Draw_Bar(Ufos[i].pos_back[X], Ufos[i].pos_back[Y]-UFO_DEST_BOUND, Ufos[i].pos_back[X] + UFO_DEST_BOUND, Ufos[i].pos_back[Y], BLACK);
			Tank.fired_cnt--;
			break;
		}
	}
}


void Draw_Tank(void)
{
//	printf("draw tank\n");
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
//		// remove previous state in lcd
		Remove_Prev_Frame(&Tank);
//
//		// draw current state in lcd
//		Draw_Curr_Mis_Tank_Frame(&Tank);

		// set to tank flag not moved.
		Tank.move_flag = NOT_MOVED;
	}
	// remove previous state in lcd
//	Remove_Prev_Frame(&Tank);

	// draw current state in lcd
	Draw_Curr_Mis_Tank_Frame(&Tank);
}


void Draw_Ufo(void)
{
	int i;
	for(i=0; i < NO_OF_UFOS; i++){
		// draw ufo - edge case
		if(Ufos[i].pos[Y] < W_Y_MIN + UFO_HEIGHT)
		{
//			printf("less than y min\n");
			Ufos[i].pos[Y] = Ufos[i].pos_back[Y];
			Remove_Prev_Frame(&Ufos[i]);
		}

		// draw Ufo
		if(Ufos[i].move_flag != NOT_MOVED)
		{
//			printf("x,y = (%d, %d), back: x,y = (%d, %d)\n", Ufo.pos[X],Ufo.pos[Y],Ufo.pos_back[X], Ufo.pos_back[Y]);
			Remove_Prev_Ufo_Frame(&Ufos[i]);
			Draw_Curr_Frame(&Ufos[i]);

			Ufos[i].move_flag = NOT_MOVED;
		}
	}
}

void Draw_Missiles()
{
	int i;
	for(i = ZERO; i < NO_OF_MISSILES; i++){
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

			if(Tank.fired_cnt < ZERO) Tank.fired_cnt = ZERO;
			Remove_Prev_Frame(&Missiles[i]);
			Remove_Curr_Frame(&Missiles[i]);
		}


		// draw tank beam
		// if tank beam moved, then flag will be enabled. after enabled, updated.
		if(Missiles[i].move_flag != NOT_MOVED)
		{
			Remove_Prev_Frame(&Missiles[i]);
			Draw_Curr_Mis_Tank_Frame(&Missiles[i]);
			Missiles[i].move_flag = NOT_MOVED;
		}
	}
}


void Draw_Missile_Crashed(void)
{
	int i;
	for(i = ZERO; i < NO_OF_MISSILES; i++){
		// draw tank beam object when crashed.
		if(Missiles[i].cd_flag == OBJECT_CRASHED)
		{
			// remove tank beam object, set color to black.
			Remove_Prev_Frame(&Missiles[i]);

			// reverse state
			Missiles[i].cd_flag = OBJECT_NOT_CRASHED;

			// set tank beam as not fired
			Missiles[i].missile_flag = NOT_FIRED;
		}

	}
}

void Draw_Ufo_Crashed(void)
{
	int i;
	for(i = 0; i < NO_OF_UFOS; i++){
		// draw ufo object when crashed.
		if(Ufos[i].cd_flag == OBJECT_CRASHED)
		{
			// remove ufo object, set color to black.
			Remove_Prev_Frame(&Ufos[i]);

			// reverse state
			Ufos[i].cd_flag = OBJECT_NOT_CRASHED;

			// set ufo position to beginning.
			Ufos[i].pos[Y] = Ufos[i].pos_init[Y];
		}
	}
}

void Remove_Prev_Ufo_Frame(struct Object *obj){
	Lcd_Draw_Bar(
		obj->pos_back[X],
		obj->pos_back[Y],
		obj->pos_back[X] + obj->size[X],
		obj->pos_back[Y] - obj->size[Y],
		BG_COLOR);
}

void Remove_Prev_Frame(struct Object *obj)
{
	Lcd_Draw_Bar(
		obj->pos_back[X],
		obj->pos_back[Y],
		obj->pos_back[X] + obj->size[X],
		obj->pos_back[Y] + obj->size[Y],
		BG_COLOR);
}

void Remove_Curr_Frame(struct Object *obj)
{
	// remove previous state in lcd
	Lcd_Draw_Bar(
		obj->pos[X],
		obj->pos[Y],
		obj->pos[X] + obj->size[X],
		obj->pos[Y] + obj->size[Y],
		BG_COLOR);
}

void Draw_Curr_Frame(struct Object *obj)
{
	Lcd_Draw_Bar(
		obj->pos[X],
		obj->pos[Y],
		obj->pos[X] + obj->size[X],
		obj->pos[Y] - obj->size[Y],
		obj->color);
}

void Draw_Curr_Mis_Tank_Frame(struct Object *obj)
{
	Lcd_Draw_Bar(
		obj->pos[X],
		obj->pos[Y],
		obj->pos[X] + obj->size[X],
		obj->pos[Y] + obj->size[Y],
		obj->color);
}
//
///* =====================================
// * print states of objects
// * =====================================
// */
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
void print_ufo(int i){
	Uart_Printf("==================ufo===============\n");
	Uart_Printf("Ufo Timer : %d\n", Ufos[i].timer);
	Uart_Printf("Ufo move_flag : %d\n", Ufos[i].move_flag);
	Uart_Printf("Ufo pos[X] %d / pos[Y] %d\n", Ufos[i].pos[X], Ufos[i].pos[Y]);
	Uart_Printf("Ufo pos_back[X] %d / pos_back[Y] %d\n", Ufos[i].pos_back[X], Ufos[i].pos_back[Y]);
	Uart_Printf("Ufo size[X] %d / size[Y] %d\n", Ufos[i].size[X], Ufos[i].size[Y]);
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
