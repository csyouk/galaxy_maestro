#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>
#include "constants.h"

#define BG_COLOR WHITE
#define printf 	Uart_Printf
#define NO_OF_MISSILES sizeof(Missiles)/sizeof(Missiles[0])

void Show_Intro(void);
void Show_Next(void);
void Show_Game_Over(void);
void Show_Finish(void);
void Show_Loading_Animation_And_Stage(int);
void Stage_With_Ufo(int);

void Init_Game(void);
void Init_Ufos(void);
void Init_Tank(void);

void Check_Game_State(void);
void Check_Explosion(void);
void Collision_Detect(void);
void Galaxy_Maestro(void);

void Update_Object(void);
void Update_Tank(int);
void Update_Ufo(void);
void Update_Missiles(int);

void Draw_Object(void);
void Draw_State_Disp(void);
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
void print_game_state(void);

int Missile_In_Ufo(int);
int Tank_In_Ufo(int j);

void disp_score(char*);
void disp_life(char*);

bool is_object_in_lcd(struct Object *obj);


const unsigned short * Loading[] = { kb_0, kb_1, kb_2, kb_3, kb_4, kb_5, kb_6,
		kb_7, kb_8, kb_9, kb_10, kb_11, kb_12, kb_13, kb_14, kb_15, kb_16,
		kb_17, kb_18, kb_19, kb_20, kb_21, kb_22, kb_23, kb_24, kb_25, kb_26,
		kb_27, kb_28, kb_29, kb_30, kb_31, kb_32, kb_33, kb_34, kb_35, kb_36,
		kb_37, kb_38, kb_39, kb_40, kb_41};


/* ====================================
 *  Bootstraping.
 * ====================================
 */


void Galaxy_Maestro(void)
{
	if(game_state == GAME_OVER){
		Show_Game_Over();
		return;
	}
	if(game_state == FINISH || curr_stage == (STAGE_3 + 1)){
		Show_Finish();
		return;
	}

	while(game_state){
		if(game_state == WAIT_TO_START) {
			Show_Intro();
		} else {
			Show_Next();
		}
		if(curr_stage == (STAGE_3 + 1)) return;
		if(Key_Get_Pressed()){
			Show_Loading_Animation_And_Stage(curr_stage);
			Stage_With_Ufo(ufos_in_stage[curr_stage]);
		}
	}
}

void Show_Intro(){
	Lcd_Draw_BMP(ZERO, ZERO, wait);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, RED, YELLOW, 2, 1, "PRESS TO START!");
	Timer4_Delay(100);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, BLUE, YELLOW, 2, 1, "PRESS TO START!");
	Timer4_Delay(100);
}

void Show_Next(){
	Lcd_Draw_BMP(ZERO, ZERO, next);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, RED, WHITE, 2, 1, "PRESS TO NEXT!!");
	Timer4_Delay(100);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, BLUE, WHITE, 2, 1, "PRESS TO NEXT!!");
	Timer4_Delay(100);
}

void Show_Game_Over(){
	Lcd_Draw_BMP(ZERO, ZERO, game_over);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, RED, WHITE, 2, 1, "===GAME OVER===");
	Timer4_Delay(100);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, BLUE, WHITE, 2, 1, "===GAME OVER===");
	Timer4_Delay(100);
}

void Show_Finish(){
	Lcd_Draw_BMP(ZERO, ZERO, finish);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, RED, WHITE, 2, 1, "==GAME FINISH==");
	Timer4_Delay(100);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, BLUE, WHITE, 2, 1, "==GAME FINISH==");
	Timer4_Delay(100);
}

void Show_Loading_Animation_And_Stage(int stage){
	int i,j;
	char s_stage[TWO] = {0};
	s_stage[ZERO] = convert_intnum_to_charnum(stage + ONE);
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(W_F_WIDTH/6, W_F_HEIGHT*4/5, BLACK, WHITE, 2, 1, "Loading STAGE");
	Lcd_Printf(W_F_WIDTH*5/6, W_F_HEIGHT*4/5, BLACK, WHITE, 2, 1, s_stage);
	for(i=ZERO; i < WAIT_TIME; i++){
		for(j=0; j < (sizeof(Loading) / sizeof(Loading[0])); j++){
			Lcd_Draw_BMP(W_F_WIDTH/2-50, W_F_HEIGHT/2-50, Loading[j]);
			Timer4_Delay(30);

		}
	}
}

void Stage_With_Ufo(int ufo_cnt){
	num_of_ufos = ufo_cnt;

	Init_Game();
	Init_Tank();
	Init_Ufos();
	Draw_Object();

	while(game_state){
		Update_Object();
		Collision_Detect();
		Draw_Object();
		Check_Game_State();
		if((game_state == PENDING) ||
		   (game_state == OVER)    ||
		   (game_state == FINISH)) return;
	}
}

void Init_Game(void)
{
	Lcd_Clr_Screen(BG_COLOR);
	Timer0_Repeat(FREQ);
	Lcd_Draw_Bar(W_X_MAX, W_Y_MIN, W_F_WIDTH-1, W_F_HEIGHT, RED);
	Lcd_Printf(W_X_MAX, MARGIN, BLACK, RED, 1, 1, "SCORE");
	Lcd_Printf(W_X_MAX + MARGIN, MARGIN*10, BLACK, RED, 1, 1, "LIFE");
	disp_score(conv_int_to_string(score));
	disp_life(conv_int_to_string(life));
	curr_ufo_cnt = ufos_in_stage[curr_stage];
	game_state = (game_state == GAME_OVER) ? GAME_OVER : ON_GOING;
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

void Init_Ufos()
{
	int i, dir_x, dir_y;
	for(i = ZERO; i < num_of_ufos; i++){

		dir_x = rand() % TWO - TWO;
		dir_y = rand() % FIVE - TWO;
		Ufos[i].speed_step = rand() % TWO + ONE;
		Ufos[i].move_step = rand() % TWO + ONE;
		Ufos[i].cd_flag = OBJECT_NOT_CRASHED;
		Ufos[i].pos_back[X] = TWO;
		Ufos[i].pos_back[Y] = TWO;
		Ufos[i].destroyed = UNDESTROYED;
		Ufos[i].pos[X] = rand() % W_X_MAX/FIVE + UFO_WIDTH * TWO;
		Ufos[i].pos[Y] = rand() % W_Y_MAX/FIVE + UFO_HEIGHT * TWO;
		Ufos[i].fly_dir[X] = (dir_x == ZERO)? THREE : dir_x;
		Ufos[i].fly_dir[Y] = (dir_y == ZERO)? -THREE : dir_y;
	}
}

/*============================================
* update states of object
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


	for(i = ZERO; i < num_of_ufos; i++){
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
			curr_ufo_cnt--;
			Tank.fired_cnt--;
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
			life--;
			if(life < ZERO) {
				life = ZERO;
				game_state = GAME_OVER;
			}

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
	for(i=0; i < num_of_ufos; i++){
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
		if(key == FIRE && Tank.fired_cnt < NO_OF_MISSILES && Tank.pos[X] > MISSILE_WIDTH) {
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
	Draw_State_Disp();
	Draw_Tank();
	Draw_Ufo();
	Draw_Missiles();
	Check_Explosion();
	Remove_Crashed_Missile();
	Remove_Crashed_Ufo();
	Remove_Crashed_Tank();
}

void Draw_State_Disp(void)
{
//	if(!Timer0_Check_Expired()) return;
	disp_life(conv_int_to_string(life));
	disp_score(conv_int_to_string(score));
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
	for(i=0; i < num_of_ufos; i++){
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
			Remove_Prev_Frame(&Ufos[i]);
			Draw_Curr_Frame(&Ufos[i]);
		}

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
	for(i = ZERO; i < num_of_ufos; i++){
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
	for(i = 0; i < num_of_ufos; i++){
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
	}
}


void Check_Game_State()
{
	if(curr_ufo_cnt >= (STAGE_3 + 1)) {
		game_state=FINISH;
	}
	if(curr_ufo_cnt != ZERO) return;
	game_state=PENDING;
	curr_stage++;
	print_game_state();
	if(curr_stage > STAGE_3){
		game_state=FINISH;
	}
}


/* =====================================
 * check edge cases
 * =====================================
 */

bool is_object_in_lcd(struct Object *obj)
{
	return false;
}

/* =====================================
 * remove / draw frame
 * =====================================
 */

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


/* =====================================
 * print states of objects
 * =====================================
 */
void print_tank(void){
	printf("==================tank===============\n");
	printf("Timer : %d\n", Tank.timer);
	printf("move_flag : %d\n", Tank.move_flag);
	printf("pos[X] %d / pos[Y] %d\n", Tank.pos[X], Tank.pos[Y]);
	printf("pos_back[X] %d / pos_back[Y] %d\n", Tank.pos_back[X], Tank.pos_back[Y]);
	printf("size[X] %d / size[Y] %d\n", Tank.size[X], Tank.size[Y]);
	printf("tank direction %d\n", Tank.dir);
}
void print_ufo(int i){
	printf("==================ufo===============\n");
	printf("Ufo Timer : %d\n", Ufos[i].timer);
	printf("Ufo move_flag : %d\n", Ufos[i].move_flag);
	printf("Ufo cd_flag : %d\n", Ufos[i].cd_flag);
	printf("Ufo pos[X] %d / pos[Y] %d\n", Ufos[i].pos[X], Ufos[i].pos[Y]);
	printf("Ufo pos_back[X] %d / pos_back[Y] %d\n", Ufos[i].pos_back[X], Ufos[i].pos_back[Y]);
	printf("Ufo size[X] %d / size[Y] %d\n", Ufos[i].size[X], Ufos[i].size[Y]);
	printf("Ufo fly_dir[X] %d / fly_di[Y] %d\n", Ufos[i].fly_dir[X], Ufos[i].fly_dir[Y]);
}

void print_missile(int i){
	printf("==================missile===============\n");
	printf("Missiles[%d] Timer : %d\n", i, Missiles[i].timer);
	printf("Missiles[%d] move_flag : %d\n", i, Missiles[i].move_flag);
	printf("Missiles[%d] pos[X] %d / pos[Y] %d\n", i, Missiles[i].pos[X], Missiles[i].pos[Y]);
	printf("Missiles[%d] pos_back[X] %d / pos_back[Y] %d\n", i, Missiles[i].pos_back[X], Missiles[i].pos_back[Y]);
	printf("Missiles[%d] size[X] %d / size[Y] %d\n", i, Missiles[i].size[X], Missiles[i].size[Y]);
	printf("Missiles[%d] dir : %d\n", i, Missiles[i].dir);
}

void print_game_state(){
//	STAGE_1=1,
//	STAGE_2,
//	STAGE_3,
//	GAME_OVER,
//	PENDING,
//	WAIT_TO_START
	printf("game_state : %d\n", game_state);
	printf("num of ufo : %d\n", num_of_ufos);
	printf("curr stage : %d\n", curr_stage + 1);
	printf("curr ufo cnt : %d\n", curr_ufo_cnt);
}

/* =====================================
 * display character
 * =====================================
 */
void disp_score(char *score){
	Lcd_Printf(W_X_MAX + 5*MARGIN, 5*MARGIN, BLACK, RED, 1, 1, score);
}
void disp_life(char *life){
	Lcd_Printf(W_X_MAX + 5*MARGIN, 14*MARGIN, BLACK, RED, 1, 1, life);
}
