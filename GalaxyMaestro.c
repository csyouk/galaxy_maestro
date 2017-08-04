#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>
#include "constants.h"

#define BG_COLOR WHITE
#define printf 	Uart_Printf
#define NO_OF_BANANS sizeof(Bananas)/sizeof(Bananas[0])

void Show_Intro(void);
void Show_Next(void);
void Show_Game_Over(void);
void Show_Finish(void);
void Show_Loading_Animation_And_Stage(int);
void Stage_With_Minion(int);

void Init_Game(void);
void Init_Minions(int);
void Init_Ufo(void);

void Reset_State(void);

void Check_Game_State(void);
void Check_Explosion(void);
void Collision_Detect(void);
void Galaxy_Maestro(void);

void Update_Object(void);
void Update_Ufo(int);
void Update_Minion(void);
void Update_Bananas(int);

void Draw_Object(void);
void Draw_State_Disp(void);
void Draw_Ufo(void);
void Draw_Bananas(void);
void Draw_Minion(void);
void Remove_Crashed_Missile(void);
void Remove_Crashed_Minion(void);
void Remove_Crashed_Ufo(void);

void Draw_Curr_Frame(struct Object *obj);
void Blinking(struct Object *obj , int color);
void Remove_Curr_Frame(struct Object *obj);
void Remove_Prev_Frame(struct Object *obj);
void Remove_All_Frame(struct Object *obj);

void print_ufo(void);
void print_minion(int);
void print_banana(int);
void print_game_state(void);

int Missile_In_Minion(int);
int Ufo_In_Minion(int j);

void disp_score(char*);
void disp_life(char*);

void YMCA_Song(void);

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

	if(game_state == FINISH || curr_stage == THREE){
		Show_Finish();
		return;
	}

	while(game_state){
		if(game_state == WAIT_TO_START) {
			Show_Intro();
		}
		if(game_state == PENDING){
			Show_Next();
		}
		if(curr_stage == THREE) return;
		if(Key_Get_Pressed()){
			Show_Loading_Animation_And_Stage(curr_stage);
			Stage_With_Minion(minions_in_stage[curr_stage]);
		}
	}
}

void Show_Intro(){
	Lcd_Draw_BMP(ZERO, ZERO, minions);
	Lcd_Set_Shape_Mode(1, TRANSPARENCY);
	Lcd_Printf(W_F_WIDTH/10, W_F_HEIGHT/11, YELLOW, TRANSPARENCY, 2, 2, "GIVE ME BANANA!!");

	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*10/11, YELLOW, TRANSPARENCY, 2, 1, "PRESS TO START!");
	Timer4_Delay(100);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*10/11, BLUE, TRANSPARENCY, 2, 1, "PRESS TO START!");
	Timer4_Delay(100);
	if(is_song_played == NO){
		YMCA_Song();
		is_song_played = !is_song_played;
	}
}

void Show_Next(){
	Lcd_Draw_BMP(ZERO, ZERO, next);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, RED, TRANSPARENCY, 2, 1, "PRESS TO NEXT!!");
	Timer4_Delay(100);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, BLUE, TRANSPARENCY, 2, 1, "PRESS TO NEXT!!");
	Timer4_Delay(100);
}

void Show_Game_Over(){
	Lcd_Draw_BMP(ZERO, ZERO, game_over);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*7/8, RED, TRANSPARENCY, 2, 1, "===GAME OVER===");
	Timer4_Delay(100);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*7/8, BLUE, TRANSPARENCY, 2, 1, "===GAME OVER===");
	Timer4_Delay(100);
	if(Key_Get_Pressed() == RESET){
		game_state = WAIT_TO_START;
		Reset_State();
	}
}

void Show_Finish(){
	Lcd_Draw_BMP(ZERO, ZERO, finish);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, RED, TRANSPARENCY, 2, 1, "==GAME FINISH==");
	Timer4_Delay(100);
	Lcd_Printf(W_F_WIDTH/7, W_F_HEIGHT*6/7, BLUE, TRANSPARENCY, 2, 1, "==GAME FINISH==");
	Timer4_Delay(100);
//	while(ONE){
	if(Key_Get_Pressed() == RESET){
		game_state = WAIT_TO_START;
		Reset_State();
	}
//	}
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

void Stage_With_Minion(int minion_cnt){
	num_of_minions = minion_cnt;

	Init_Game();
	Init_Ufo();
	Init_Minions(minion_cnt);
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
	Timer0_Repeat(GAME_SPEED);
	Lcd_Draw_Bar(W_X_MAX, W_Y_MIN, W_F_WIDTH-1, W_F_HEIGHT, WHITE);
	Lcd_Printf(W_X_MAX, MARGIN, YELLOW, WHITE, 1, 1, "SCORE");
	Lcd_Printf(W_X_MAX + MARGIN, MARGIN*10, YELLOW, WHITE, 1, 1, "LIFE");
	disp_score(conv_int_to_string(score));
	disp_life(conv_int_to_string(life));
	curr_minion_cnt = minions_in_stage[curr_stage];
	game_state = (game_state == GAME_OVER) ? GAME_OVER : ON_GOING;
}

void Init_Ufo(void)
{
	Ufo.timer = ZERO;
	Ufo.move_flag = N_MOVE;
	Ufo.pos[X] = UFO_POS_INIT_X;
	Ufo.pos[Y] = UFO_POS_INIT_Y;
	Ufo.pos_init[X] = UFO_POS_INIT_X;
	Ufo.pos_init[Y] = UFO_POS_INIT_Y;
	Ufo.pos_back[X] = UFO_POS_INIT_X;
	Ufo.pos_back[Y] = UFO_POS_INIT_Y;
	Ufo.size[X] = UFO_WIDTH;
	Ufo.size[Y] = UFO_HEIGHT;
	Ufo.color = RED;
	Ufo.speed_step = UFO_SPEED_RATE;
	Ufo.move_step =  UFO_FOOTSTEP;
	Ufo.cd_flag = OBJECT_NOT_CRASHED;
	Ufo.dir = DOWN;
	Ufo.fired_cnt = ZERO;
	Ufo.destroyed = UNDESTROYED;
}

void Init_Minions(int minion_cnt)
{
	int i, dir_x, dir_y;
	for(i = ZERO; i < num_of_minions; i++){
		dir_x = rand() % TWO - TWO;
		dir_y = rand() % FIVE - TWO;
		Minions[i].speed_step = rand() % TWO + ONE;
		Minions[i].move_step = rand() % (minion_cnt*2/3) + ONE;
		Minions[i].cd_flag = OBJECT_NOT_CRASHED;
		Minions[i].pos_back[X] = TWO;
		Minions[i].pos_back[Y] = TWO;
		Minions[i].destroyed = UNDESTROYED;
		Minions[i].pos[X] = rand() % W_X_MAX/FIVE + MINION_WIDTH * TWO;
		Minions[i].pos[Y] = rand() % W_Y_MAX/FIVE + MINION_HEIGHT * TWO;
		Minions[i].fly_dir[X] = (dir_x == ZERO)? THREE : dir_x;
		Minions[i].fly_dir[Y] = (dir_y == ZERO)? -THREE : dir_y;
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
		Update_Minion();
		Update_Ufo(key);
		Update_Bananas(key);
	}
}


void Collision_Detect(void)
{
	int i, crashed_banana;


	for(i = ZERO; i < num_of_minions; i++){
		// minion(기준)와 미사일(비교대상)이 충돌했는가?
		// 충돌하면 3가지 멤버의 속성을 서로 일치시켜줘야 한다.
		// move_flag, timer, cd_flag
		crashed_banana = Missile_In_Minion(i);
		if(Minions[i].cd_flag != OBJECT_CRASHED && crashed_banana){
			Bananas[crashed_banana-1].move_flag = N_MOVE;
			Bananas[crashed_banana-1].timer = ZERO;
			Bananas[crashed_banana-1].cd_flag = OBJECT_CRASHED;

			Minions[i].move_flag = N_MOVE;
			Minions[i].timer = ZERO;
			Minions[i].cd_flag = OBJECT_CRASHED;
			Minions[i].destroyed = DESTROYED;
			curr_minion_cnt--;
			Ufo.fired_cnt--;
			score++;
			Sound_Effect(CRASHED);
		}


		// minion(기준)와 탱크(비교대상)가 충돌했는가?
		// 충돌하면 3가지 멤버의 속성을 서로 일치시켜줘야 한다.
		// move_flag, timer, cd_flag
		if(Minions[i].cd_flag != OBJECT_CRASHED && Ufo_In_Minion(i)){
			Ufo.move_flag = N_MOVE;
			Ufo.timer = ZERO;
			Ufo.cd_flag = OBJECT_CRASHED;
			Ufo.destroyed = DESTROYED;
			life--;
			Sound_Effect(CRASHED);
			if(life < ZERO) {
				life = ZERO;
				game_state = GAME_OVER;
			}

		}
	}
}

int Missile_In_Minion(int j){
	if(Minions[j].destroyed) return OBJECT_NOT_CRASHED;
	int crashed_banana;
	for(crashed_banana = ZERO; crashed_banana < NO_OF_BANANS; crashed_banana++){
		if( (Bananas[crashed_banana].banana_flag != NOT_FIRED) &&
			(Bananas[crashed_banana].pos[X] + BANANA_WIDTH / 2 > Minions[j].pos[X]) &&
			(Bananas[crashed_banana].pos[X] + BANANA_WIDTH / 2 < Minions[j].pos[X] + Minions[j].size[X]) &&
			(Bananas[crashed_banana].pos[Y] + BANANA_HEIGHT / 2 > Minions[j].pos[Y]) &&
			(Bananas[crashed_banana].pos[Y] + BANANA_HEIGHT / 2 < Minions[j].pos[Y] + Minions[j].size[Y])
		  )
		{
			return (crashed_banana+1);
		}
	}
	return OBJECT_NOT_CRASHED;
}

int Ufo_In_Minion(int j){
	if(Minions[j].destroyed) return OBJECT_NOT_CRASHED;
	if(
		(Ufo.pos[X] + Ufo.size[X]/2 > Minions[j].pos[X]) &&
		(Ufo.pos[X] + Ufo.size[X]/2 < Minions[j].pos[X] + Minions[j].size[Y]) &&
		(Ufo.pos[Y] + Ufo.size[Y]/2 > Minions[j].pos[Y]) &&
		(Ufo.pos[Y] + Ufo.size[Y]/2 < Minions[j].pos[Y] + Minions[j].size[Y])

	  )
	{
		return OBJECT_CRASHED;
	}
	return OBJECT_NOT_CRASHED;
}

void Update_Minion(void)
{
	int i;
	for(i=0; i < num_of_minions; i++){
		Minions[i].timer++;

		if(Minions[i].timer < Minions[i].speed_step) return;
		Minions[i].timer = ZERO;
		Minions[i].pos_back[X] = Minions[i].pos[X];
		Minions[i].pos_back[Y] = Minions[i].pos[Y];
		Minions[i].fly_dir_back[X] = Minions[i].fly_dir[X];
		Minions[i].fly_dir_back[Y] = Minions[i].fly_dir[Y];
		Minions[i].pos[X] += Minions[i].fly_dir[X] * Minions[i].move_step;
		Minions[i].pos[Y] += Minions[i].fly_dir[Y] * Minions[i].move_step;
		Minions[i].move_flag = MOVE;

		if(Minions[i].pos[X] < W_X_MIN || Minions[i].pos[X] > W_X_MAX - MINION_WIDTH - 1){
			Minions[i].fly_dir[X] = -Minions[i].fly_dir[X];
			Minions[i].pos[X] = Minions[i].pos_back[X];
			return;
		}

		if(Minions[i].pos[Y] < W_Y_MIN + MINION_HEIGHT || Minions[i].pos[Y] > W_Y_MAX){
			Minions[i].fly_dir[Y] = -Minions[i].fly_dir[Y];
			Minions[i].pos[Y] = Minions[i].pos_back[Y];
			return;
		}
	}
}


void Update_Ufo(int _direction)
{
	// only key input 1~4 can pass.
	if(_direction < UP || _direction > RIGHT) return;

	Ufo.pos_back[X] = Ufo.pos[X];
	Ufo.pos_back[Y] = Ufo.pos[Y];
	Ufo.dir = _direction;
	Ufo.move_flag = MOVE;

	switch(Ufo.dir){
		case UP:
			Ufo.pos[Y] -= Ufo.move_step;
			break;
		case LEFT:
			Ufo.pos[X] -= Ufo.move_step;
			break;
		case DOWN:
			Ufo.pos[Y] += Ufo.move_step;
			break;
		case RIGHT:
			Ufo.pos[X] += Ufo.move_step;
			break;
		default: break;
	}
}


void Update_Bananas(int key)
{
	int i;
	key_seq.cur = key;
	int _is_banana_moving = N_MOVE;

	for(i = ZERO; i<NO_OF_BANANS; i++){
		if(!Bananas[i].banana_flag) Bananas[i].dir = Ufo.dir;
		Bananas[i].timer++;
	}

	if(key_seq.pre == NOT_PRESSED_YET || key_seq.pre != key_seq.cur){
		if(key == FIRE &&
			Ufo.fired_cnt < NO_OF_BANANS &&
			Ufo.pos[X] > BANANA_WIDTH +1 &&
			Ufo.pos[X] < W_X_MAX - BANANA_WIDTH -1) {
			if(Ufo.fired_cnt > NO_OF_BANANS - 1) {
//				printf("exceed cnt : %d\n", Ufo.fired_cnt);
				Ufo.fired_cnt = NO_OF_BANANS - 1;
			}
			if(Bananas[Ufo.fired_cnt].banana_flag == NOT_FIRED){
				Bananas[Ufo.fired_cnt].banana_flag = FIRED;

				Bananas[Ufo.fired_cnt].pos_init[X] = Ufo.pos[X];
				Bananas[Ufo.fired_cnt].pos_init[Y] = Ufo.pos[Y];

				Bananas[Ufo.fired_cnt].pos_back[X] = Bananas[Ufo.fired_cnt].pos_init[X];
				Bananas[Ufo.fired_cnt].pos_back[Y] = Bananas[Ufo.fired_cnt].pos_init[Y];

				Bananas[Ufo.fired_cnt].pos[X] = Bananas[Ufo.fired_cnt].pos_init[X];
				Bananas[Ufo.fired_cnt].pos[Y] = Bananas[Ufo.fired_cnt].pos_init[Y];
				Ufo.fired_cnt++;
				Sound_Effect(BEEP);
			}
		}
		key_seq.pre = key_seq.cur;
	}


	for(i = ZERO; i < NO_OF_BANANS; i++){
		_is_banana_moving = (Bananas[i].banana_flag != NOT_FIRED && Bananas[i].timer >= Bananas[i].speed_step);

		if(_is_banana_moving)
		{
			Bananas[i].timer = ZERO;
			Bananas[i].move_flag = MOVE;

			Bananas[i].pos_back[X] = Bananas[i].pos[X];
			Bananas[i].pos_back[Y] = Bananas[i].pos[Y];

			switch(Bananas[i].dir){
				case UP:
					Bananas[i].pos[Y] -= Bananas[i].move_step;
					break;
				case LEFT:
					Bananas[i].pos[X] -= Bananas[i].move_step;
					break;
				case DOWN:
					Bananas[i].pos[Y] += Bananas[i].move_step;
					break;
				case RIGHT:
					Bananas[i].pos[X] += Bananas[i].move_step;
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
	Draw_Ufo();
	Draw_Minion();
	Draw_Bananas();
//	Check_Explosion();
	Remove_Crashed_Missile();
	Remove_Crashed_Minion();
	Remove_Crashed_Ufo();
}

void Draw_State_Disp(void)
{
//	if(!Timer0_Check_Expired()) return;
	disp_life(conv_int_to_string(life + ONE));
	disp_score(conv_int_to_string(score));
}

void Draw_Ufo(void)
{
	// draw ufo - edge cases
	// if ufo position exceed window then block
	if((Ufo.pos[X] > W_X_MAX - UFO_WIDTH - 1) ||
	   (Ufo.pos[X] < W_X_MIN) ||
	   (Ufo.pos[Y] > W_Y_MAX - UFO_HEIGHT) ||
	   (Ufo.pos[Y] < W_Y_MIN + 1))
	{
		Ufo.pos[X] = Ufo.pos_back[X];
		Ufo.pos[Y] = Ufo.pos_back[Y];
	}

	// draw ufo
	if(Ufo.move_flag != N_MOVE)
	{
//		Remove_Prev_Frame(&Ufo);
		Lcd_Draw_BMP(Ufo.pos_back[X],Ufo.pos_back[Y], trace);
		Ufo.move_flag = N_MOVE;
	}

//	Draw_Curr_Frame(&Ufo);
	Lcd_Draw_BMP(Ufo.pos[X], Ufo.pos[Y], ufo_ico);
}


void Draw_Minion(void)
{
	int i;
	for(i=0; i < num_of_minions; i++){
		if( Minions[i].pos[X] > W_X_MAX - MINION_WIDTH - 1 ||
			Minions[i].pos[X] < W_X_MIN + 1 ||
			Minions[i].pos[Y] > W_Y_MAX ||
			Minions[i].pos[Y] < W_Y_MIN - MINION_HEIGHT)
		{
			Minions[i].pos[Y] = Minions[i].pos_back[Y];
//			Remove_Prev_Frame(&Minions[i]);
			Lcd_Draw_BMP(Minions[i].pos_back[X], Minions[i].pos_back[Y], trace);
		}

		// draw Minion
		if(Minions[i].move_flag != N_MOVE &&
		   Minions[i].cd_flag == OBJECT_NOT_CRASHED &&
		   Minions[i].destroyed == UNDESTROYED)
		{
//			minion가 충돌없이 움직이고 있는 상태.

			Lcd_Draw_BMP(Minions[i].pos_back[X], Minions[i].pos_back[Y], trace);
			Lcd_Draw_BMP(Minions[i].pos[X],Minions[i].pos[Y], bob);
			//			Remove_Prev_Frame(&Minions[i]);
			//			Draw_Curr_Frame(&Minions[i]);

		}

	}
}

void Draw_Bananas()
{
	int i;
	for(i = ZERO; i < NO_OF_BANANS; i++){
		// draw ufo beam - edge case
		// if fired beam off from the window, then set the ufo beam member flag as not fired.
		if(
		    (Bananas[i].pos[Y] > W_Y_MAX - BANANA_HEIGHT) ||
			(Bananas[i].pos[Y] < W_Y_MIN - BANANA_HEIGHT) ||
			(Bananas[i].pos[X] < W_X_MIN + BANANA_WIDTH) ||
			(Bananas[i].pos[X] > W_X_MAX - BANANA_WIDTH*2 - 1)
			)
		{
			if(Bananas[i].banana_flag == FIRED){
				Bananas[i].move_flag = N_MOVE;
				Ufo.fired_cnt--;
//				printf("cnt : %d\n", Ufo.fired_cnt);
			}
			Bananas[i].banana_flag = NOT_FIRED;
			Bananas[i].dir = Ufo.dir;

			if(Ufo.fired_cnt < ZERO) Ufo.fired_cnt = ZERO;
//			Remove_All_Frame(&Bananas[i]);
			Lcd_Draw_BMP(Bananas[i].pos_back[X],Bananas[i].pos_back[Y],trace);
			Lcd_Draw_BMP(Bananas[i].pos[X],Bananas[i].pos[Y],trace);
		}


		// draw ufo beam
		if(Bananas[i].move_flag != N_MOVE)
		{
			// not crashed case
//			Remove_Prev_Frame(&Bananas[i]);
//			Draw_Curr_Frame(&Bananas[i]);
			Lcd_Draw_BMP(Bananas[i].pos_back[X],Bananas[i].pos_back[Y],trace);
			Lcd_Draw_BMP(Bananas[i].pos[X],Bananas[i].pos[Y],banana);
			Bananas[i].move_flag = N_MOVE;
		}
		if(Bananas[i].cd_flag != OBJECT_NOT_CRASHED){
			// crashed case
//			Remove_All_Frame(&Bananas[i]);
			Lcd_Draw_BMP(Bananas[i].pos_back[X],Bananas[i].pos_back[Y],trace);
			Lcd_Draw_BMP(Bananas[i].pos[X],Bananas[i].pos[Y],trace);

		}
	}
}


void Check_Explosion(void)
{
	int i;
	for(i = ZERO; i < num_of_minions; i++){
		if(Minions[i].destroyed && Minions[i].cd_flag){
			Blinking(&Minions[i], WHITE);
			Blinking(&Minions[i], BLUE);
			Blinking(&Minions[i], RED);
			Blinking(&Minions[i], BLACK);
		}
	}
}

void Remove_Crashed_Missile(void)
{
	int i;
	for(i = ZERO; i < NO_OF_BANANS; i++){
		// draw ufo beam object when crashed.
		if(Bananas[i].cd_flag == OBJECT_CRASHED)
		{
//			Remove_Prev_Frame(&Bananas[i]);
			Lcd_Draw_BMP(Bananas[i].pos_back[X],Bananas[i].pos_back[Y],trace);

			// reverse state
			Bananas[i].cd_flag = OBJECT_NOT_CRASHED;

			// set ufo beam as not fired
			Bananas[i].banana_flag = NOT_FIRED;
		}

	}
}

void Remove_Crashed_Minion(void)
{
	int i;
	for(i = 0; i < num_of_minions; i++){
		// remove minion object when crashed.
		if(Minions[i].cd_flag)
		{
//			Remove_All_Frame(&Minions[i]);
			Lcd_Draw_BMP(Minions[i].pos_back[X],Minions[i].pos_back[Y],trace);
			Lcd_Draw_BMP(Minions[i].pos[X],Minions[i].pos[Y],trace);
			Minions[i].cd_flag = !Minions[i].cd_flag;
		}

		if(Minions[i].move_flag == N_MOVE && Minions[i].destroyed == DESTROYED ){
			// minion가 충돌한 상태.
			Minions[i].move_flag = N_MOVE;
//			Remove_All_Frame(&Minions[i]);
			Lcd_Draw_BMP(Minions[i].pos_back[X],Minions[i].pos_back[Y],trace);
			Lcd_Draw_BMP(Minions[i].pos[X],Minions[i].pos[Y],trace);
		}
	}
}

void Remove_Crashed_Ufo(void)
{
	if(Ufo.destroyed){
//		Remove_All_Frame(&Ufo);
		Lcd_Draw_BMP(Ufo.pos_back[X],Ufo.pos_back[Y], trace);
		Lcd_Draw_BMP(Ufo.pos[X],Ufo.pos[Y], trace);
		Init_Ufo();
	}
}


void Check_Game_State()
{
	if(curr_minion_cnt != ZERO) return;
	if(curr_stage >= NO_MORE_STAGE) {
		game_state = FINISH;
		return;
	}
	game_state = PENDING;
	curr_stage++;
	if(curr_stage == NO_MORE_STAGE){
		game_state = FINISH;
	}
}

void Reset_State()
{
	printf("reset!\n");
	game_state = WAIT_TO_START;
	score = ZERO;
	life = 4;
	curr_stage = ZERO;
	curr_minion_cnt = num_of_minions = minions_in_stage[curr_stage];
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

void YMCA_Song(void)
{
	Timer3_Buzzer_Beep(G1,N16);
	Timer4_Delay(50);
	Timer3_Buzzer_Beep(E1,N8);
	Timer4_Delay(150);
	Timer3_Buzzer_Beep(E1,N16);
	Timer4_Delay(30);
	Timer3_Buzzer_Beep(D1,N16);
	Timer4_Delay(50);
	Timer3_Buzzer_Beep(C1,N16);
	Timer4_Delay(30);
	Timer3_Buzzer_Beep(D1,N16);
	Timer4_Delay(30);
	Timer3_Buzzer_Beep(E1,N16);
	Timer4_Delay(30);
	Timer3_Buzzer_Beep(G1,N8);
	Timer4_Delay(30);
	Timer3_Buzzer_Beep(E1,N16);
	Timer4_Delay(30);
	Timer3_Buzzer_Beep(G1,N8);
	Timer4_Delay(200);

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

//void Draw_Curr_BMP(struct Object *obj, unsigned short *bmp)
//{
//	Lcd_Draw_BMP(
//		obj->pos[X],
//		obj->pos[Y],
//		bmp
//			);
//}

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
void print_ufo(void){
	printf("==================ufo===============\n");
	printf("Timer : %d\n", Ufo.timer);
	printf("move_flag : %d\n", Ufo.move_flag);
	printf("pos[X] %d / pos[Y] %d\n", Ufo.pos[X], Ufo.pos[Y]);
	printf("pos_back[X] %d / pos_back[Y] %d\n", Ufo.pos_back[X], Ufo.pos_back[Y]);
	printf("size[X] %d / size[Y] %d\n", Ufo.size[X], Ufo.size[Y]);
	printf("ufo direction %d\n", Ufo.dir);
}
void print_minion(int i){
	printf("==================minion===============\n");
	printf("Minion Timer : %d\n", Minions[i].timer);
	printf("Minion move_flag : %d\n", Minions[i].move_flag);
	printf("Minion cd_flag : %d\n", Minions[i].cd_flag);
	printf("Minion pos[X] %d / pos[Y] %d\n", Minions[i].pos[X], Minions[i].pos[Y]);
	printf("Minion pos_back[X] %d / pos_back[Y] %d\n", Minions[i].pos_back[X], Minions[i].pos_back[Y]);
	printf("Minion size[X] %d / size[Y] %d\n", Minions[i].size[X], Minions[i].size[Y]);
	printf("Minion fly_dir[X] %d / fly_di[Y] %d\n", Minions[i].fly_dir[X], Minions[i].fly_dir[Y]);
}

void print_banana(int i){
	printf("==================banana===============\n");
	printf("Bananas[%d] Timer : %d\n", i, Bananas[i].timer);
	printf("Bananas[%d] move_flag : %d\n", i, Bananas[i].move_flag);
	printf("Bananas[%d] pos[X] %d / pos[Y] %d\n", i, Bananas[i].pos[X], Bananas[i].pos[Y]);
	printf("Bananas[%d] pos_back[X] %d / pos_back[Y] %d\n", i, Bananas[i].pos_back[X], Bananas[i].pos_back[Y]);
	printf("Bananas[%d] size[X] %d / size[Y] %d\n", i, Bananas[i].size[X], Bananas[i].size[Y]);
	printf("Bananas[%d] dir : %d\n", i, Bananas[i].dir);
}

void print_game_state(){
	printf("game_state : %d\n", game_state);
	printf("num of minion : %d\n", num_of_minions);
	printf("curr stage : %d\n", curr_stage + 1);
	printf("curr minion cnt : %d\n", curr_minion_cnt);
}

/* =====================================
 * display character
 * =====================================
 */
void disp_score(char *score){
	Lcd_Printf(W_X_MAX + 5*MARGIN, 5*MARGIN, BLACK, WHITE, 1, 1, score);
}
void disp_life(char *life){
	Lcd_Printf(W_X_MAX + 5*MARGIN, 14*MARGIN, BLACK, WHITE, 1, 1, life);
}
