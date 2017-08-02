#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>
#include "constants.h"

#define BG_COLOR BLACK

#define printf 	Uart_Printf

#define NO_OF_MISSILES sizeof(Missiles)/sizeof(Missiles[0])
#define NO_OF_UFOS sizeof(Ufos)/sizeof(Ufos[0])


void Init_Game(void);
void Init_Ufos(void);
void Init_Tank(void);

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
void Remove_Crashed_Missile(void);
void Remove_Crashed_Ufo(void);
void Remove_Crashed_Tank(void);

void Draw_Curr_Frame(struct Object *obj);
void Blinking(struct Object *obj , int color);
void Remove_Curr_Frame(struct Object *obj);
void Remove_Prev_Frame(struct Object *obj);
void Remove_All_Frame(struct Object *obj);

void print_tank(void);
void print_ufo(int);
void print_missile(int);

int Missile_In_Ufo(int);
int Tank_In_Ufo(int j);


/* ====================================
 *  Bootstraping.
 * ====================================
 */


void Galaxy_Maestro(void)
{
	Init_Game();
	Init_Tank();
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
	Lcd_Draw_Bar(W_X_MAX, W_Y_MIN, W_F_WIDTH-1, W_F_HEIGHT, RED);
	Lcd_Printf(W_X_MAX, MARGIN, BLACK, RED, 1, 1, "SCORE");
	Lcd_Printf(W_X_MAX + 5*MARGIN, 5*MARGIN, BLACK, RED, 1, 1, INIT_SCORE);
}

void Init_Tank(void)
{
	Tank.timer = ZERO;
	Tank.move_flag = N_MOVE;
	Tank.pos[X] = TANK_POS_INIT_X;
	Tank.pos[Y] = TANK_POS_INIT_Y;
	Tank.pos_init[X] = TANK_POS_INIT_X;
	Tank.pos_init[Y] = TANK_POS_INIT_Y;
	Tank.pos_back[X] = TANK_POS_INIT_X;
	Tank.pos_back[Y] = TANK_POS_INIT_Y;
	Tank.size[X] = TANK_WIDTH;
	Tank.size[Y] = TANK_HEIGHT;
	Tank.color = RED;
	Tank.speed_step = TANK_SPEED_RATE;
	Tank.move_step =  TANK_FOOTSTEP;
	Tank.cd_flag = OBJECT_NOT_CRASHED;
	Tank.dir = DOWN;
	Tank.fired_cnt = ZERO;
	Tank.destroyed = UNDESTROYED;
}

void Init_Ufos(void)
{
	int i, dir_x, dir_y;
	for(i = ZERO; i < NO_OF_UFOS; i++){

		dir_x = rand() % 2 - 2;
		dir_y = rand() % 5 - 2;
		Ufos[i].speed_step = rand() % 2 + 1;
		Ufos[i].move_step = rand() % 3 + 1;
		Ufos[i].cd_flag = OBJECT_NOT_CRASHED;
		Ufos[i].pos_back[X] = 1;
		Ufos[i].pos_back[Y] = 1;
		Ufos[i].destroyed = UNDESTROYED;
		Ufos[i].pos[X] = rand() % W_X_MAX/2 + UFO_WIDTH*2;
		Ufos[i].pos[Y] = rand() % W_Y_MAX/2 + UFO_HEIGHT*2;
		Ufos[i].fly_dir[X] = (dir_x == ZERO)? 3 : dir_x;
		Ufos[i].fly_dir[Y] = (dir_y == ZERO)? -3 : dir_y;

//		print_ufo(i);
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
	int i, crashed_missile;

	for(i = ZERO; i < NO_OF_UFOS; i++){
		// ufo(기준)와 미사일(비교대상)이 충돌했는가?
		// 충돌하면 3가지 멤버의 속성을 서로 일치시켜줘야 한다.
		// move_flag, timer, cd_flag
		crashed_missile = Missile_In_Ufo(i);
		if(Ufos[i].cd_flag != OBJECT_CRASHED && crashed_missile){
			Missiles[crashed_missile-1].move_flag = N_MOVE;
			Missiles[crashed_missile-1].timer = ZERO;
			Missiles[crashed_missile-1].cd_flag = OBJECT_CRASHED;

			Ufos[i].move_flag = N_MOVE;
			Ufos[i].timer = ZERO;
			Ufos[i].cd_flag = OBJECT_CRASHED;
			Ufos[i].destroyed = DESTROYED;

			Tank.fired_cnt--;
			printf("cnt : %d\n", Tank.fired_cnt);
			score++;
		}


		// ufo(기준)와 탱크(비교대상)가 충돌했는가?
		// 충돌하면 3가지 멤버의 속성을 서로 일치시켜줘야 한다.
		// move_flag, timer, cd_flag
		if(Ufos[i].cd_flag != OBJECT_CRASHED && Tank_In_Ufo(i)){
			Tank.move_flag = N_MOVE;
			Tank.timer = ZERO;
			Tank.cd_flag = OBJECT_CRASHED;
			Tank.destroyed = DESTROYED;
			printf("tank destroyed!!\n");
		}
	}


}

int Missile_In_Ufo(int j){
	if(Ufos[j].destroyed) return OBJECT_NOT_CRASHED;
	int crashed_missile;
	for(crashed_missile = ZERO; crashed_missile < NO_OF_MISSILES; crashed_missile++){
		if( (Missiles[crashed_missile].missile_flag != NOT_FIRED) &&
			(Missiles[crashed_missile].pos[X] + MISSILE_WIDTH / 2 > Ufos[j].pos[X]) &&
			(Missiles[crashed_missile].pos[X] + MISSILE_WIDTH / 2 < Ufos[j].pos[X] + Ufos[j].size[X]) &&
			(Missiles[crashed_missile].pos[Y] + MISSILE_HEIGHT / 2 > Ufos[j].pos[Y]) &&
			(Missiles[crashed_missile].pos[Y] + MISSILE_HEIGHT / 2 < Ufos[j].pos[Y] + Ufos[j].size[Y])
		  )
		{
			return (crashed_missile+1);
		}
	}
	return OBJECT_NOT_CRASHED;
}

int Tank_In_Ufo(int j){
	if(Ufos[j].destroyed) return OBJECT_NOT_CRASHED;
	if(
		(Tank.pos[X] + Tank.size[X]/2 > Ufos[j].pos[X]) &&
		(Tank.pos[X] + Tank.size[X]/2 < Ufos[j].pos[X] + Ufos[j].size[Y]) &&
		(Tank.pos[Y] + Tank.size[Y]/2 > Ufos[j].pos[Y]) &&
		(Tank.pos[Y] + Tank.size[Y]/2 < Ufos[j].pos[Y] + Ufos[j].size[Y])

	  )
	{
		return OBJECT_CRASHED;
	}
	return OBJECT_NOT_CRASHED;
}

void Update_Ufo(void)
{
	int i;
	for(i=0; i < NO_OF_UFOS; i++){
		Ufos[i].timer++;

		if(Ufos[i].timer < Ufos[i].speed_step) return;
		Ufos[i].timer = ZERO;
		Ufos[i].pos_back[X] = Ufos[i].pos[X];
		Ufos[i].pos_back[Y] = Ufos[i].pos[Y];
		Ufos[i].fly_dir_back[X] = Ufos[i].fly_dir[X];
		Ufos[i].fly_dir_back[Y] = Ufos[i].fly_dir[Y];
		Ufos[i].pos[X] += Ufos[i].fly_dir[X] * Ufos[i].move_step;
		Ufos[i].pos[Y] += Ufos[i].fly_dir[Y] * Ufos[i].move_step;
		Ufos[i].move_flag = MOVE;

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
	Tank.move_flag = MOVE;

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
	int _is_missile_moving = N_MOVE;

	for(i = ZERO; i<NO_OF_MISSILES; i++){
		if(!Missiles[i].missile_flag) Missiles[i].dir = Tank.dir;
		Missiles[i].timer++;
	}

	if(key_seq.pre == NOT_PRESSED_YET || key_seq.pre != key_seq.cur){
		if(key == FIRE && Tank.fired_cnt < NO_OF_MISSILES) {
			if(Tank.fired_cnt > NO_OF_MISSILES - 1) {
				printf("exceed cnt : %d\n", Tank.fired_cnt);
				Tank.fired_cnt = NO_OF_MISSILES - 1;
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
			Missiles[i].move_flag = MOVE;

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
	// DO NOT CHANGE PROCEDURE ORDER!!!!
	Draw_Tank();
	Draw_Ufo();
	Draw_Missiles();
	Check_Explosion();
	Remove_Crashed_Missile();
	Remove_Crashed_Ufo();
	Remove_Crashed_Tank();
}



void Draw_Tank(void)
{
	// draw tank - edge cases
	// if tank position exceed window then block
	if((Tank.pos[X] > W_X_MAX - TANK_WIDTH - 1) ||
	   (Tank.pos[X] < W_X_MIN) ||
	   (Tank.pos[Y] > W_Y_MAX - TANK_HEIGHT) ||
	   (Tank.pos[Y] < W_Y_MIN + 1))
	{
		Tank.pos[X] = Tank.pos_back[X];
		Tank.pos[Y] = Tank.pos_back[Y];
	}

	// draw tank
	if(Tank.move_flag != N_MOVE)
	{
		Remove_Prev_Frame(&Tank);
		Tank.move_flag = N_MOVE;
	}

	Draw_Curr_Frame(&Tank);
}


void Draw_Ufo(void)
{
	int i;
	for(i=0; i < NO_OF_UFOS; i++){
		if( Ufos[i].pos[X] > W_X_MAX - UFO_WIDTH - 1 ||
			Ufos[i].pos[X] < W_X_MIN + 1 ||
			Ufos[i].pos[Y] > W_Y_MAX ||
			Ufos[i].pos[Y] < W_Y_MIN - UFO_HEIGHT)
		{
			Ufos[i].pos[Y] = Ufos[i].pos_back[Y];
			Remove_Prev_Frame(&Ufos[i]);
		}

		// draw Ufo
		if(Ufos[i].move_flag != N_MOVE &&
		   Ufos[i].cd_flag == OBJECT_NOT_CRASHED &&
		   Ufos[i].destroyed == UNDESTROYED)
		{
//			ufo가 충돌없이 움직이고 있는 상태.
//			printf("ufo move without crashing\n");
			Remove_Prev_Frame(&Ufos[i]);
			Draw_Curr_Frame(&Ufos[i]);
//			Ufos[i].move_flag = N_MOVE;
		}
//		if(Ufos[i].move_flag == N_MOVE && Ufos[i].destroyed == DESTROYED ){
//			printf("ufo crahsed\n");
//			// ufo가 충돌한 상태.
//			Ufos[i].move_flag = N_MOVE;
//			Remove_All_Frame(&Ufos[i]);
//		}
	}
}

void Draw_Missiles()
{
	int i;
	for(i = ZERO; i < NO_OF_MISSILES; i++){
		// draw tank beam - edge case
		// if fired beam off from the window, then set the tank beam member flag as not fired.
		if(
		    (Missiles[i].pos[Y] > W_Y_MAX - MISSILE_HEIGHT) ||
			(Missiles[i].pos[Y] < W_Y_MIN - MISSILE_HEIGHT) ||
			(Missiles[i].pos[X] < W_X_MIN + MISSILE_WIDTH/2) ||
			(Missiles[i].pos[X] > W_X_MAX - 2*MISSILE_WIDTH)
			)
		{
			if(Missiles[i].missile_flag == FIRED){
				Tank.fired_cnt--;
				printf("cnt : %d\n", Tank.fired_cnt);
			}
			Missiles[i].missile_flag = NOT_FIRED;
			Missiles[i].dir = Tank.dir;

			if(Tank.fired_cnt < ZERO) Tank.fired_cnt = ZERO;
			Remove_All_Frame(&Missiles[i]);
		}


		// draw tank beam
		if(Missiles[i].move_flag != N_MOVE)
		{
			// not crashed case
			Remove_Prev_Frame(&Missiles[i]);
			Draw_Curr_Frame(&Missiles[i]);
			Missiles[i].move_flag = N_MOVE;
		}
		if(Missiles[i].cd_flag != OBJECT_NOT_CRASHED){
			// crashed case
			Remove_All_Frame(&Missiles[i]);
		}
	}
}


void Check_Explosion(void)
{
	int i;
	for(i = ZERO; i < NO_OF_UFOS; i++){
		if(Ufos[i].destroyed && Ufos[i].cd_flag){
			Blinking(&Ufos[i], WHITE);
			Blinking(&Ufos[i], BLUE);
			Blinking(&Ufos[i], RED);
			Blinking(&Ufos[i], BLACK);
		}
	}
}

void Remove_Crashed_Missile(void)
{
	int i;
	for(i = ZERO; i < NO_OF_MISSILES; i++){
		// draw tank beam object when crashed.
		if(Missiles[i].cd_flag == OBJECT_CRASHED)
		{
			Remove_Prev_Frame(&Missiles[i]);

			// reverse state
			Missiles[i].cd_flag = OBJECT_NOT_CRASHED;

			// set tank beam as not fired
			Missiles[i].missile_flag = NOT_FIRED;
		}

	}
}

void Remove_Crashed_Ufo(void)
{
	int i;
	for(i = 0; i < NO_OF_UFOS; i++){
		// remove ufo object when crashed.
		if(Ufos[i].cd_flag)
		{
			Remove_All_Frame(&Ufos[i]);
			Ufos[i].cd_flag = !Ufos[i].cd_flag;
		}

		if(Ufos[i].move_flag == N_MOVE && Ufos[i].destroyed == DESTROYED ){
			// ufo가 충돌한 상태.
			Ufos[i].move_flag = N_MOVE;
			Remove_All_Frame(&Ufos[i]);
		}
	}
}

void Remove_Crashed_Tank(void)
{
	if(Tank.destroyed){
		Remove_All_Frame(&Tank);
		Init_Tank();
		printf("tank re init\n");
	}
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

void Remove_All_Frame(struct Object *obj)
{
	Lcd_Draw_Bar(
		obj->pos[X],
		obj->pos[Y],
		obj->pos[X] + obj->size[X],
		obj->pos[Y] + obj->size[Y],
		BG_COLOR);
	Lcd_Draw_Bar(
		obj->pos_back[X],
		obj->pos_back[Y],
		obj->pos_back[X] + obj->size[X],
		obj->pos_back[Y] + obj->size[Y],
		BG_COLOR);
}

void Draw_Curr_Frame(struct Object *obj)
{
	Lcd_Draw_Bar(
		obj->pos[X],
		obj->pos[Y],
		obj->pos[X] + obj->size[X],
		obj->pos[Y] + obj->size[Y],
		obj->color);
}

void Blinking(struct Object *obj , int color)
{
	Lcd_Draw_Bar(
		obj->pos[X],
		obj->pos[Y],
		obj->pos[X] + obj->size[X],
		obj->pos[Y] + obj->size[Y],
		color);
	Timer4_Delay(50);
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
}
void print_ufo(int i){
	Uart_Printf("==================ufo===============\n");
	Uart_Printf("Ufo Timer : %d\n", Ufos[i].timer);
	Uart_Printf("Ufo move_flag : %d\n", Ufos[i].move_flag);
	Uart_Printf("Ufo cd_flag : %d\n", Ufos[i].cd_flag);
	Uart_Printf("Ufo pos[X] %d / pos[Y] %d\n", Ufos[i].pos[X], Ufos[i].pos[Y]);
	Uart_Printf("Ufo pos_back[X] %d / pos_back[Y] %d\n", Ufos[i].pos_back[X], Ufos[i].pos_back[Y]);
	Uart_Printf("Ufo size[X] %d / size[Y] %d\n", Ufos[i].size[X], Ufos[i].size[Y]);
	Uart_Printf("Ufo fly_dir[X] %d / fly_di[Y] %d\n", Ufos[i].fly_dir[X], Ufos[i].fly_dir[Y]);
}

void print_missile(int i){
	Uart_Printf("==================missile===============\n");
	Uart_Printf("Missiles[%d] Timer : %d\n", i, Missiles[i].timer);
	Uart_Printf("Missiles[%d] move_flag : %d\n", i, Missiles[i].move_flag);
	Uart_Printf("Missiles[%d] pos[X] %d / pos[Y] %d\n", i, Missiles[i].pos[X], Missiles[i].pos[Y]);
	Uart_Printf("Missiles[%d] pos_back[X] %d / pos_back[Y] %d\n", i, Missiles[i].pos_back[X], Missiles[i].pos_back[Y]);
	Uart_Printf("Missiles[%d] size[X] %d / size[Y] %d\n", i, Missiles[i].size[X], Missiles[i].size[Y]);
	Uart_Printf("Missiles[%d] dir : %d\n", i, Missiles[i].dir);

}
