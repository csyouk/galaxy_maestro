#define X_AND_Y 2
#define NOT_PRESSED_YET -1
#define DEFAULT 0

void print_tank(void);
void print_ufo(void);
void print_missile(int);

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
};


struct KEY {
	char pre;
	char cur;
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
 W_X_MIN=0,
 W_X_MAX=320,
 W_Y_MIN=0,
 W_Y_MAX=240
};


enum TANK_DATA{
	TANK_TIMER=0,
	TANK_WIDTH=10,
	TANK_HEIGHT=10,
	TANK_POS_INIT_X=W_X_MAX/2 - TANK_WIDTH/2,
	TANK_POS_INIT_Y=W_Y_MAX/2 - TANK_HEIGHT/2,
	TANK_SPEED_RATE=5,
	TANK_FOOTSTEP=TANK_WIDTH/8,
	TANK_DIR=3,
	TANK_FIRED_CNT=0
};

enum UFO_DATA{
	UFO_WIDTH=10,
	UFO_HEIGHT=10,
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
	MISSILE_WIDTH=6,
	MISSILE_HEIGHT=6,
	MISSILE_TIMER=0,
	MISSILE_SPEED_RATE=1,
	MISSILE_FOOTSTEP=4,
	MISSILE_DIR=3,
	MISSILE_DEFAULT=-20
};

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

struct Object Ufos[5] = {
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


int score = 0;
