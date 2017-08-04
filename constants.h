#include "./Image/intro_bob.h"
#include "./Image/minions.h"
#include "./Image/ufo_ico.h"
#include "./Image/banana.h"
#include "./Image/trace.h"
#include "./Image/game_over.h"
#include "./Image/bob.h"
#include "./Image/finish.h"
#include "./Image/wait.h"
#include "./Image/next.h"
#include "./Image/kb_0.h"
#include "./Image/kb_1.h"
#include "./Image/kb_2.h"
#include "./Image/kb_3.h"
#include "./Image/kb_4.h"
#include "./Image/kb_5.h"
#include "./Image/kb_6.h"
#include "./Image/kb_7.h"
#include "./Image/kb_8.h"
#include "./Image/kb_9.h"
#include "./Image/kb_10.h"
#include "./Image/kb_11.h"
#include "./Image/kb_12.h"
#include "./Image/kb_13.h"
#include "./Image/kb_14.h"
#include "./Image/kb_15.h"
#include "./Image/kb_16.h"
#include "./Image/kb_17.h"
#include "./Image/kb_18.h"
#include "./Image/kb_19.h"
#include "./Image/kb_20.h"
#include "./Image/kb_21.h"
#include "./Image/kb_22.h"
#include "./Image/kb_23.h"
#include "./Image/kb_24.h"
#include "./Image/kb_25.h"
#include "./Image/kb_26.h"
#include "./Image/kb_27.h"
#include "./Image/kb_28.h"
#include "./Image/kb_29.h"
#include "./Image/kb_30.h"
#include "./Image/kb_31.h"
#include "./Image/kb_32.h"
#include "./Image/kb_33.h"
#include "./Image/kb_34.h"
#include "./Image/kb_35.h"
#include "./Image/kb_36.h"
#include "./Image/kb_37.h"
#include "./Image/kb_38.h"
#include "./Image/kb_39.h"
#include "./Image/kb_40.h"
#include "./Image/kb_41.h"


#define X_AND_Y 2
#define NOT_PRESSED_YET -1
#define DEFAULT 0
#define DESTROYED 1
#define UNDESTROYED 0
#define X 0
#define Y 1

#define DEFAULT 0
#define NOT_FIRED 0
#define FIRED 1
#define N_MOVE 0
#define MOVE 1
#define OBJECT_NOT_CRASHED 0
#define OBJECT_CRASHED 1
#define NOT_PRESSED_YET -1
#define WAIT_TIME 2

#define MARGIN 4

#define GAME_SPEED 20
#define YES 1
#define NO 0

#define BG_COLOR WHITE
#define printf 	Uart_Printf

char* conv_int_to_string(int);
char convert_intnum_to_charnum(int);
void Sound_Effect(int);

struct Object
{
	short timer;     	 	  // 몇번 타이머를 쓸 것인지?
	short move_flag;   	  // 움직이고 있는 상태인지 아니지,
	short pos[X_AND_Y]; 		  // x,y
	short pos_init[X_AND_Y];  	  // 초기 좌표.
	short pos_back[X_AND_Y];      // 이전 좌표의 정보. 이미지를 사용할 시, 이 좌표를 토대로 이전의 이미지를 지워야함.
	short size[X_AND_Y];	      // width, height
	short color; // 나중에 이미지로 대체.
	short speed_step;	 	  // 이미지가 얼마나 빨리 이동되게 할 것인지.
	short move_step;		  // 이미지를 얼마나 이동시킬 것인가?
	short banana_flag;		  // banana 발사 됐는지 여부 flag
	short cd_flag;		  // collision detection flag
	short dir;           // 1,2,3,4번 키를 누름에 따라 미사일의 방향이 정해진다. 차례대로, up, left, down, right
	short fired_cnt;        // ufo 객체에서 미사일이 발사된 횟수를 관리한다.
	short fly_dir[X_AND_Y]; // minion의 x,y방향을 결정.
	short fly_dir_back[X_AND_Y]; // minion의 x,y방향을 결정.
	short destroyed;              // minion가 파괴되었는지 결정.
	short launch_order;			// banana 발사 순서
};


struct KEY {
	char pre;
	char cur;
};

typedef enum {false, true} bool;

enum Game{OVER, START};

enum Key{UP=1, LEFT, DOWN, RIGHT, FIRE, RESET};

enum Number{ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX};
/* 5:5:5:I Color Definition */
enum Color{
	BLACK=0x0000,
	WHITE=0xfffe,
	BLUE=0x003e,
	GREEN=0x07c0,
	RED=0xf800,
	DARK_GREEN=0x534d,
	YELLOW=0xfed9,
	TRANSPARENCY=0xf0f1
};

enum key{C1, C1_, D1, D1_, E1, F1, F1_, G1, G1_, A1, A1_, B1, C2, C2_, D2, D2_, E2, F2, F2_, G2, G2_, A2, A2_, B2};
enum note{N16, N8, N4, N2, N1, BEEP, CRASHED};
const int song[][2] = {{G1,N4},{G1,N4},{E1,N8},{F1,N8},{G1,N4},{A1,N4},{A1,N4},{G1,N2},{G1,N4},{C2,N4},{E2,N4},{D2,N8},{C2,N8},{D2,N2},{0xff, 0xff}};


/*
terms about window edges
 ||       || ||
X------1------|
|             |
2             4
|             |
|------3------|

X point : origin

line 1 : W_X_MIN
line 2 : W_Y_MIN
line 3 : W_X_MAX
line 4 : W_Y_MAX

*/
enum WINDOW{
	W_F_WIDTH=320,
	W_F_HEIGHT=240,
	W_X_MIN=0,
	W_X_MAX=280,
	W_Y_MIN=0,
	W_Y_MAX=240,
	W_DISP_WIDTH=W_F_WIDTH-W_X_MAX
};


enum UFO_DATA{
	UFO_TIMER=0,
	UFO_WIDTH=24,
	UFO_HEIGHT=24,
	UFO_POS_INIT_X=W_X_MAX/2 - UFO_WIDTH/2,
	UFO_POS_INIT_Y=W_Y_MAX/2 - UFO_HEIGHT/2,
	UFO_SPEED_RATE=1,
	UFO_FOOTSTEP=5,
	UFO_DIR=3,
	UFO_FIRED_CNT=0
};

enum MINION_DATA{
	MINION_WIDTH=20,
	MINION_HEIGHT=20,
	MINION_POS_INIT_X=120,
	MINION_POS_INIT_Y=239,
	MINION_TIMER=0,
	MINION_SPEED_RATE=2,
	MINION_FOOTSTEP=2,
	MINION_DIR=3,
	MINION_DIR_X=-1,
	MINION_DIR_Y=-2,
	MINION_DEST_BOUND=15
};

enum BANANA_DATA{
	BANANA_WIDTH=10,
	BANANA_HEIGHT=10,
	BANANA_TIMER=0,
	BANANA_SPEED_RATE=1,
	BANANA_FOOTSTEP=4,
	BANANA_DIR=3,
	BANANA_DEFAULT=-20
};

enum STAGE_WITH_MINION{
	GAME_OVER,
	STAGE_1,
	STAGE_2,
	STAGE_3,
	PENDING,
	WAIT_TO_START,
	ON_GOING,
	FINISH,
	NO_MORE_STAGE=4
};

int minions_in_stage[3] = {5,7,10};

struct KEY key_seq = {NOT_PRESSED_YET, DEFAULT};

struct Object Ufo = {
	UFO_TIMER,
	1,
	{W_X_MAX/2 - UFO_WIDTH/2, W_Y_MAX/2 - UFO_HEIGHT/2},  // 현재 탱크 위치.
	{UFO_POS_INIT_X,UFO_POS_INIT_Y},  // 초기화시킬 시 탱크 위치.
	{0,10},  // 탱크의 위치가 움직이게 될 때, 탱크의 위치를 벡업.
	{UFO_WIDTH,UFO_HEIGHT}, // 탱크의 크기. 가로 15, 세로 10
	RED,
	UFO_SPEED_RATE,
	UFO_FOOTSTEP,
	0,
	0,
	UFO_DIR,
	UFO_FIRED_CNT,
	{0,0},
};

struct Object Minion = {
	0,
	1,
	{MINION_POS_INIT_X,MINION_POS_INIT_Y},
	{MINION_POS_INIT_X,MINION_POS_INIT_Y},
	{160,300},
	{MINION_WIDTH,MINION_HEIGHT},
	BLUE,
	MINION_SPEED_RATE,
	MINION_FOOTSTEP,
	0,
	0,
	0,
	0,
	{MINION_DIR_X,MINION_DIR_Y},
	{1,1}
};

struct Object Bananas[5] = {
		{
			BANANA_TIMER,
			1,
			{319,239},
			{319,239},
			{319,239},
			{BANANA_WIDTH,BANANA_HEIGHT},
			GREEN,
			BANANA_SPEED_RATE,
			BANANA_FOOTSTEP,
			0,
			0,
			BANANA_DIR
		},
		{
			BANANA_TIMER,
			1,
			{319,239},
			{319,239},
			{319,239},
			{BANANA_WIDTH,BANANA_HEIGHT},
			GREEN,
			BANANA_SPEED_RATE,
			BANANA_FOOTSTEP,
			0,
			0,
			BANANA_DIR
		},
		{
			BANANA_TIMER,
			1,
			{319,239},
			{319,239},
			{319,239},
			{BANANA_WIDTH,BANANA_HEIGHT},
			GREEN,
			BANANA_SPEED_RATE,
			BANANA_FOOTSTEP,
			0,
			0,
			BANANA_DIR
		},
		{
			BANANA_TIMER,
			1,
			{319,239},
			{319,239},
			{319,239},
			{BANANA_WIDTH,BANANA_HEIGHT},
			GREEN,
			BANANA_SPEED_RATE,
			BANANA_FOOTSTEP,
			0,
			0,
			BANANA_DIR
		},
		{
			BANANA_TIMER,
			1,
			{319,239},
			{319,239},
			{319,239},
			{BANANA_WIDTH,BANANA_HEIGHT},
			GREEN,
			BANANA_SPEED_RATE,
			BANANA_FOOTSTEP,
			0,
			0,
			BANANA_DIR
		}
};

struct Object Minions[10] = {
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		},
		{
			0,
			1,
			{160,300},
			{160,300},
			{160,300},
			{MINION_WIDTH,MINION_HEIGHT},
			BLUE,
			MINION_SPEED_RATE,
			MINION_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		}
};

const unsigned short * Loading[] = { kb_0, kb_1, kb_2, kb_3, kb_4, kb_5, kb_6,
		kb_7, kb_8, kb_9, kb_10, kb_11, kb_12, kb_13, kb_14, kb_15, kb_16,
		kb_17, kb_18, kb_19, kb_20, kb_21, kb_22, kb_23, kb_24, kb_25, kb_26,
		kb_27, kb_28, kb_29, kb_30, kb_31, kb_32, kb_33, kb_34, kb_35, kb_36,
		kb_37, kb_38, kb_39, kb_40, kb_41};

char s_score[3]={0};
int score = 0;
int life = 3; // 4
int game_state = WAIT_TO_START;
int num_of_minions = 3;
int curr_stage = ZERO;
int curr_minion_cnt;
int is_song_played = NO;

/* =====================================
 * Initialize object properties.
 * =====================================
 */

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

void Init_Banana_Pos(){
	Bananas[Ufo.fired_cnt].banana_flag = FIRED;

	Bananas[Ufo.fired_cnt].pos_init[X] = Ufo.pos[X];
	Bananas[Ufo.fired_cnt].pos_init[Y] = Ufo.pos[Y];

	Bananas[Ufo.fired_cnt].pos_back[X] = Bananas[Ufo.fired_cnt].pos_init[X];
	Bananas[Ufo.fired_cnt].pos_back[Y] = Bananas[Ufo.fired_cnt].pos_init[Y];

	Bananas[Ufo.fired_cnt].pos[X] = Bananas[Ufo.fired_cnt].pos_init[X];
	Bananas[Ufo.fired_cnt].pos[Y] = Bananas[Ufo.fired_cnt].pos_init[Y];

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


/* =====================================
 * effect
 * =====================================
 */

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

void Draw_Curr_BMP(struct Object *obj, const unsigned short *bmp)
{
	Lcd_Draw_BMP(
		obj->pos[X],
		obj->pos[Y],
		bmp);
}

void Remove_Prev_BMP(struct Object *obj)
{
	Lcd_Draw_BMP(
		obj->pos_back[X],
		obj->pos_back[Y],
		trace);
}

void Remove_Curr_BMP(struct Object *obj)
{
	Lcd_Draw_BMP(
		obj->pos[X],
		obj->pos[Y],
		trace);
}

void Remove_All_BMP(struct Object *obj)
{
	Remove_Prev_BMP(obj);
	Remove_Curr_BMP(obj);
}

/* =====================================
 * common logics
 * =====================================
 */

bool is_object_in_lcd(struct Object *obj)
{
	return false;
}

void Backup_Pos(struct Object *obj)
{
	obj-> pos_back[X] = obj->pos[X];
	obj-> pos_back[Y] = obj->pos[Y];
}

char convert_intnum_to_charnum(int num)
{
	return num + '0';
}

char* conv_int_to_string(int score)
{
	s_score[0] = convert_intnum_to_charnum(score/10);
	s_score[1] = convert_intnum_to_charnum(score%10);
	return s_score;
}
