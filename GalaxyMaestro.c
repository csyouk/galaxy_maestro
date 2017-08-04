#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>
#include "constants.h"


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
void Collision_Detect(void);
void Galaxy_Maestro(void);

void Update_Objects(void);
void Update_Ufo(int);
void Update_Minions(void);
void Update_Bananas(int);

void Draw_Object(void);
void Draw_State_Disp(void);
void Draw_Ufo(void);
void Draw_Bananas(void);
void Draw_Minion(void);

void Remove_Crashed_Banana(void);
void Remove_Crashed_Minion(void);
void Remove_Crashed_Ufo(void);

int Banana_In_Minion(int);
int Ufo_In_Minion(int j);

void YMCA_Song(void);

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
	Lcd_Printf(W_F_WIDTH/10, W_F_HEIGHT/11, YELLOW, TRANSPARENCY, 2, 2, "CATCHING THE BOB");

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
	if(Key_Get_Pressed() == RESET){
		game_state = WAIT_TO_START;
		Reset_State();
	}
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
	Init_Bananas();
}

void Stage_With_Minion(int minion_cnt){
	num_of_minions = minion_cnt;

	Init_Game();
	Init_Ufo();
	Init_Minions(minion_cnt);
	Draw_Object();

	while(game_state){
		Update_Objects();
		Collision_Detect();
		Draw_Object();
		Check_Game_State();
		if((game_state == PENDING) ||
		   (game_state == OVER)    ||
		   (game_state == FINISH)) return;
	}
}


/*============================================
* update states of object
* ============================================
*/
void Update_Objects(void)
{
	int key = DEFAULT;
	if(Timer0_Check_Expired())
	{
		// DO NOT CHANGE PROCEDURE ORDER!!!!
		key = Key_Get_Pressed();
		Update_Minions();
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
		crashed_banana = Banana_In_Minion(i);
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

int Banana_In_Minion(int j){
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

void Update_Minions(void)
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

		if(Minions[i].pos[Y] < W_Y_MIN + MINION_HEIGHT || Minions[i].pos[Y] > W_Y_MAX - MINION_HEIGHT){
			Minions[i].fly_dir[Y] = -Minions[i].fly_dir[Y];
			Minions[i].pos[Y] = Minions[i].pos_back[Y];
			return;
		}
	}
}


void Update_Ufo(int _direction)
{
	if(_direction < UP || _direction > RIGHT) return;
	Backup_Pos(&Ufo);
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
	int is_banana_moving = N_MOVE;

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
				Ufo.fired_cnt = NO_OF_BANANS - 1;
			}
			if(Bananas[Ufo.fired_cnt].banana_flag == NOT_FIRED){
				Init_Banana_Pos();
				Ufo.fired_cnt++;
				Sound_Effect(BEEP);
			}
		}
		key_seq.pre = key_seq.cur;
	}


	for(i = ZERO; i < NO_OF_BANANS; i++){
		is_banana_moving = (Bananas[i].banana_flag != NOT_FIRED && Bananas[i].timer >= Bananas[i].speed_step);

		if(is_banana_moving)
		{
			Bananas[i].timer = ZERO;
			Bananas[i].move_flag = MOVE;
			Backup_Pos(&Bananas[i]);

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
	Remove_Crashed_Banana();
	Remove_Crashed_Minion();
	Remove_Crashed_Ufo();
}

void Draw_State_Disp(void)
{
	disp_life(conv_int_to_string(life + ONE));
	disp_score(conv_int_to_string(score));
}

void Draw_Ufo(void)
{
	if((Ufo.pos[X] > W_X_MAX - UFO_WIDTH - 1) ||
	   (Ufo.pos[X] < W_X_MIN) ||
	   (Ufo.pos[Y] > W_Y_MAX - UFO_HEIGHT) ||
	   (Ufo.pos[Y] < W_Y_MIN + 1))
	{
		Ufo.pos[X] = Ufo.pos_back[X];
		Ufo.pos[Y] = Ufo.pos_back[Y];
	}

	if(Ufo.move_flag != N_MOVE)
	{
		Remove_Prev_BMP(&Ufo);
		Ufo.move_flag = N_MOVE;
	}

	Draw_Curr_BMP(&Ufo, ufo_ico);

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
			Remove_Prev_BMP(&Minions[i]);
		}

		if(Minions[i].move_flag != N_MOVE &&
		   Minions[i].cd_flag == OBJECT_NOT_CRASHED &&
		   Minions[i].destroyed == UNDESTROYED)
		{
			Remove_Prev_BMP(&Minions[i]);
			Draw_Curr_BMP(&Minions[i], bob);

		}

	}
}

void Draw_Bananas()
{
	int i;
	for(i = ZERO; i < NO_OF_BANANS; i++){
		if(
		    (Bananas[i].pos[Y] > W_Y_MAX - BANANA_HEIGHT) ||
			(Bananas[i].pos[Y] < W_Y_MIN - BANANA_HEIGHT) ||
			(Bananas[i].pos[X] < W_X_MIN + BANANA_WIDTH) ||
			(Bananas[i].pos[X] > W_X_MAX - BANANA_WIDTH*2 - 1)
			)
		{
			if(Bananas[i].banana_flag == FIRED){
				Bananas[i].move_flag = N_MOVE;
				Remove_All_BMP(&Bananas[i]);
				Ufo.fired_cnt--;
			}
			Bananas[i].banana_flag = NOT_FIRED;
			Bananas[i].dir = Ufo.dir;

			if(Ufo.fired_cnt < ZERO) Ufo.fired_cnt = ZERO;
//			if(Bananas[i].move_flag == N_MOVE){
//
//			}
		}

		if(Bananas[i].move_flag != N_MOVE)
		{
			Remove_Prev_BMP(&Bananas[i]);
			Draw_Curr_BMP(&Bananas[i], banana);
			Bananas[i].move_flag = N_MOVE;
		}
		if(Bananas[i].cd_flag != OBJECT_NOT_CRASHED){
			Remove_All_Frame(&Bananas[i]);
		}
	}
}


void Remove_Crashed_Banana(void)
{
	int i;
	for(i = ZERO; i < NO_OF_BANANS; i++){
		if(Bananas[i].cd_flag == OBJECT_CRASHED)
		{
			Remove_Prev_BMP(&Bananas[i]);
			Bananas[i].cd_flag = OBJECT_NOT_CRASHED;
			Bananas[i].banana_flag = NOT_FIRED;
		}

	}
}

void Remove_Crashed_Minion(void)
{
	int i;
	for(i = 0; i < num_of_minions; i++){
		if(Minions[i].cd_flag)
		{
			Remove_All_BMP(&Minions[i]);
			Minions[i].cd_flag = !Minions[i].cd_flag;
		}

		if(Minions[i].move_flag == N_MOVE && Minions[i].destroyed == DESTROYED ){
			Minions[i].move_flag = N_MOVE;
			Remove_All_BMP(&Minions[i]);
		}
	}
}

void Remove_Crashed_Ufo(void)
{
	if(Ufo.destroyed){
		Remove_All_BMP(&Ufo);
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
 * display character
 * =====================================
 */
void disp_score(char *score){
	Lcd_Printf(W_X_MAX + 5*MARGIN, 5*MARGIN, BLACK, WHITE, 1, 1, score);
}
void disp_life(char *life){
	Lcd_Printf(W_X_MAX + 5*MARGIN, 14*MARGIN, BLACK, WHITE, 1, 1, life);
}
