#include "./Image/intro_bob.h"
#include "./Image/minions.h"
#include "./Image/tank_.h"
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

char* conv_int_to_string(int);
char convert_intnum_to_charnum(int);

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
	short missile_flag;		  // missile 발사 됐는지 여부 flag
	short cd_flag;		  // collision detection flag
	short dir;           // 1,2,3,4번 키를 누름에 따라 미사일의 방향이 정해진다. 차례대로, up, left, down, right
	short fired_cnt;        // tank 객체에서 미사일이 발사된 횟수를 관리한다.
	short fly_dir[X_AND_Y]; // ufo의 x,y방향을 결정.
	short fly_dir_back[X_AND_Y]; // ufo의 x,y방향을 결정.
	short destroyed;              // ufo가 파괴되었는지 결정.
	short launch_order;			// missile 발사 순서
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
enum note{N16, N8, N4, N2, N1};
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


enum TANK_DATA{
	TANK_TIMER=0,
	TANK_WIDTH=24,
	TANK_HEIGHT=24,
	TANK_POS_INIT_X=W_X_MAX/2 - TANK_WIDTH/2,
	TANK_POS_INIT_Y=W_Y_MAX/2 - TANK_HEIGHT/2,
	TANK_SPEED_RATE=1,
	TANK_FOOTSTEP=5,
	TANK_DIR=3,
	TANK_FIRED_CNT=0
};

enum UFO_DATA{
	UFO_WIDTH=20,
	UFO_HEIGHT=20,
	UFO_POS_INIT_X=120,
	UFO_POS_INIT_Y=239,
	UFO_TIMER=0,
	UFO_SPEED_RATE=2,
	UFO_FOOTSTEP=2,
	UFO_DIR=3,
	UFO_DIR_X=-1,
	UFO_DIR_Y=-2,
	UFO_DEST_BOUND=15
};

enum MISSILE_DATA{
	MISSILE_WIDTH=10,
	MISSILE_HEIGHT=10,
	MISSILE_TIMER=0,
	MISSILE_SPEED_RATE=1,
	MISSILE_FOOTSTEP=4,
	MISSILE_DIR=3,
	MISSILE_DEFAULT=-20
};

enum STAGE_WITH_UFO{
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

int ufos_in_stage[3] = {5,7,10};

struct KEY key_seq = {NOT_PRESSED_YET, DEFAULT};

struct Object Tank = {
	TANK_TIMER,
	1,
	{W_X_MAX/2 - TANK_WIDTH/2, W_Y_MAX/2 - TANK_HEIGHT/2},  // 현재 탱크 위치.
	{TANK_POS_INIT_X,TANK_POS_INIT_Y},  // 초기화시킬 시 탱크 위치.
	{0,10},  // 탱크의 위치가 움직이게 될 때, 탱크의 위치를 벡업.
	{TANK_WIDTH,TANK_HEIGHT}, // 탱크의 크기. 가로 15, 세로 10
	RED,
	TANK_SPEED_RATE,
	TANK_FOOTSTEP,
	0,
	0,
	TANK_DIR,
	TANK_FIRED_CNT,
	{0,0},
};

struct Object Ufo = {
	0,
	1,
	{UFO_POS_INIT_X,UFO_POS_INIT_Y},
	{UFO_POS_INIT_X,UFO_POS_INIT_Y},
	{160,300},
	{UFO_WIDTH,UFO_HEIGHT},
	BLUE,
	UFO_SPEED_RATE,
	UFO_FOOTSTEP,
	0,
	0,
	0,
	0,
	{UFO_DIR_X,UFO_DIR_Y},
	{1,1}
};

struct Object Missiles[5] = {
		{
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
		},
		{
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
		},
		{
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
		},
		{
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
		},
		{
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
		}
};

struct Object Ufos[10] = {
		{
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
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
			{UFO_WIDTH,UFO_HEIGHT},
			BLUE,
			UFO_SPEED_RATE,
			UFO_FOOTSTEP,
			0,
			0,
			0,
			0,
			{0,0},
		}
};


char s_score[3]={0};
int score = 0;
int life = 4;
int game_state = WAIT_TO_START;
int num_of_ufos = 3;
int curr_stage = ZERO;
int curr_ufo_cnt;
int is_song_played = NO;

