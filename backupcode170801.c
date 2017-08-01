//#include "device_driver.h"
//#include "2440addr.h"
//#include <stdlib.h>
//
//#define BG_COLOR 	BLACK
//#define WINDOW_WIDTH 		320
//#define WINDOW_HEIGHT 		240
//#define X 0
//#define Y 1
//#define X_COMMA_Y 2
//#define DEFAULT 0
//#define NOT_FIRED 0
//#define FIRED 1
//#define ZERO 0
//#define NOT_MOVED 0
//#define MOVED 1
//#define OBJECT_NOT_CRASHED 0
//#define OBJECT_CRASHED 1
//#define NOT_PRESSED_YET -1
//
///*
//terms about window edges
// ||       || ||
//|------1------|
//|             |
//2             4
//|             |
//|------3------|
//
//line 1 : W_X_MIN
//line 2 : W_Y_MIN
//line 3 : W_X_MAX
//line 4 : W_Y_MAX
//
//*/
//
//
//void Game_Init(void);
//void Check_Explosion(void);
//void Update_Object(void);
//void collision_detect(void);
//void Draw_Object(void);
//void Galaxy_Maestro(void);
//void Update_Tank(int);
//void Update_Ufo(void);
//void Update_Missile(int);
//void Draw_Tank(void);
//void Draw_Missiles(void);
//void Draw_Ufo(void);
//void Draw_Missile_Crashed(void);
//void Draw_Ufo_Crashed(void);
//void print_tank(void);
//void print_ufo(void);
//void print_missile(int);
//void Missile_Fire(int);
//
//struct Object
//{
//	int timer;     	 	  // 몇번 타이머를 쓸 것인지?
//	int move_flag;   	  // 움직이고 있는 상태인지 아니지,
//	int pos[X_COMMA_Y]; 		  // x,y
//	int pos_init[X_COMMA_Y];  	  // 초기 좌표.
//	int pos_back[X_COMMA_Y];      // 이전 좌표의 정보. 이미지를 사용할 시, 이 좌표를 토대로 이전의 이미지를 지워야함.
//	int size[X_COMMA_Y];	      // width, height
//	unsigned short color; // 나중에 이미지로 대체.
//	int speed_step;	 	  // 이미지가 얼마나 빨리 이동되게 할 것인지.
//	int move_step;		  // 이미지를 얼마나 이동시킬 것인가?
//	int missile_flag;		  // missile 발사 됐는지 여부 flag
//	int cd_flag;		  // collision detection flag
//	int dir;           // 1,2,3,4번 키를 누름에 따라 미사일의 방향이 정해진다. 차례대로, up, left, down, right
//	int fired_cnt;        // tank 객체에서 미사일이 발사된 횟수를 관리한다.
//	int fly_dir[X_COMMA_Y]; // ufo의 x,y방향을 결정.
//	int launch_order;     // 발사된 순서 저장.
//};
//
//struct KEY {
//	char pre;
//	char cur;
//};
//
//struct Object Ufos[5];
//
//enum Key{UP=1, LEFT, DOWN, RIGHT, FIRE};
//
///* 5:5:5:I Color Definition */
//enum Color{
//	BLACK=0x0000,
//	WHITE=0xfffe,
//	BLUE=0x003e,
//	GREEN=0x07c0,
//	RED=0xf800
//};
//
//enum WINDOW{
// W_X_MIN=0,
// W_X_MAX=320,
// W_Y_MIN=0,
// W_Y_MAX=240
//};
//
//enum TANK_DATA{
//	TANK_TIMER=0,
//	TANK_POS_INIT_X=0,
//	TANK_POS_INIT_Y=10,
//	TANK_WIDTH=16,
//	TANK_HEIGHT=10,
//	TANK_SPEED_RATE=5,
//	TANK_FOOTSTEP=TANK_WIDTH/8,
//	TANK_DIR=3,
//	TANK_FIRED_CNT=0
//};
//
//enum UFO_DATA{
//	UFO_WIDTH=30,
//	UFO_HEIGHT=30,
//	UFO_TIMER=0,
//	UFO_SPEED_RATE=1,
//	UFO_FOOTSTEP=2,
//	UFO_DIR=3
//};
//
//enum MISSILE_DATA{
//	MISSILE_WIDTH=6,
//	MISSILE_HEIGHT=6,
//	MISSILE_TIMER=0,
//	MISSILE_SPEED_RATE=1,
//	MISSILE_FOOTSTEP=4,
//	MISSILE_DIR=3
//};
//
//
//struct KEY key_seq = {NOT_PRESSED_YET, DEFAULT};
//
//struct Object Tank = {
//	TANK_TIMER,
//	1,
//	{0,10},  // 현재 탱크 위치.
//	{TANK_POS_INIT_X,TANK_POS_INIT_Y},  // 초기화시킬 시 탱크 위치.
//	{0,10},  // 탱크의 위치가 움직이게 될 때, 탱크의 위치를 벡업.
//	{TANK_WIDTH,TANK_HEIGHT}, // 탱크의 크기. 가로 15, 세로 10
//	RED,
//	TANK_SPEED_RATE,
//	TANK_FOOTSTEP,
//	0,
//	0,
//	TANK_DIR,
//	TANK_FIRED_CNT,
//	{0,0},
//	-1
//};
//
//struct Object Ufo = {
//	0,
//	1,
//	{160,300},
//	{160,300},
//	{160,300},
//	{UFO_WIDTH,UFO_HEIGHT},
//	BLUE,
//	UFO_SPEED_RATE,
//	UFO_FOOTSTEP,
//	0,
//	0,
//	0,
//	{0,0}
//};
//
//struct Object Missiles[5] = {
//		{
//			MISSILE_TIMER,
//			1,
//			{319,239},
//			{319,239},
//			{319,239},
//			{MISSILE_WIDTH,MISSILE_HEIGHT},
//			GREEN,
//			MISSILE_SPEED_RATE,
//			MISSILE_FOOTSTEP,
//			0,
//			0,
//			MISSILE_DIR
//		},
//		{
//			MISSILE_TIMER,
//			1,
//			{319,239},
//			{319,239},
//			{319,239},
//			{MISSILE_WIDTH,MISSILE_HEIGHT},
//			GREEN,
//			MISSILE_SPEED_RATE,
//			MISSILE_FOOTSTEP,
//			0,
//			0,
//			MISSILE_DIR
//		},
//		{
//			MISSILE_TIMER,
//			1,
//			{319,239},
//			{319,239},
//			{319,239},
//			{MISSILE_WIDTH,MISSILE_HEIGHT},
//			GREEN,
//			MISSILE_SPEED_RATE,
//			MISSILE_FOOTSTEP,
//			0,
//			0,
//			MISSILE_DIR
//		},
//		{
//			MISSILE_TIMER,
//			1,
//			{319,239},
//			{319,239},
//			{319,239},
//			{MISSILE_WIDTH,MISSILE_HEIGHT},
//			GREEN,
//			MISSILE_SPEED_RATE,
//			MISSILE_FOOTSTEP,
//			0,
//			0,
//			MISSILE_DIR
//		},
//		{
//			MISSILE_TIMER,
//			1,
//			{319,239},
//			{319,239},
//			{319,239},
//			{MISSILE_WIDTH,MISSILE_HEIGHT},
//			GREEN,
//			MISSILE_SPEED_RATE,
//			MISSILE_FOOTSTEP,
//			0,
//			0,
//			MISSILE_DIR
//		}
//};
//
//int score = 0;
//
///* ====================================
// *  Bootstraping.
// * ====================================
// */
//
//void Game_Init(void)
//{
//	Lcd_Clr_Screen(BG_COLOR);
//	Timer0_Repeat(20);
//}
//
//
//void Galaxy_Maestro(void)
//{
//	Game_Init();
//	Draw_Object();
//
//	for(;;)
//	{
//		Update_Object();
//		collision_detect();
//		Draw_Object();
//	}
//}
//
//
//
//
///* ============================================
//* 객체들의 상태 정보를 업데이트 시킨다.
//* ============================================
//*/
//void Update_Object(void)
//{
//	int _key = DEFAULT;
//	if(Timer0_Check_Expired())
//	{
//		// DO NOT CHANGE PROCEDURE ORDER!!!!
//		_key = Key_Get_Pressed();
//		Update_Ufo();
//		Update_Tank(_key);
//		Update_Missiles(_key);
//	}
//}
//
//
//void collision_detect(void)
//{
////
////	if(Missile.missile_flag != NOT_FIRED &&
////	   Missile.cd_flag == OBJECT_NOT_CRASHED)
////	{
////		if(
////			(Missile.pos[X] + MISSILE_WIDTH/2 > Ufo.pos[X]) &&
////			(Missile.pos[X] + MISSILE_WIDTH/2 < Ufo.pos[X]+Ufo.size[X]) &&
////			(Missile.pos[Y] + MISSILE_HEIGHT/2 > Ufo.pos[Y] - Ufo.size[Y]) &&
////			(Missile.pos[Y] + MISSILE_HEIGHT/2 < Ufo.pos[Y] + Ufo.size[Y] - Ufo.size[Y])
////		  )
////		{
////
////				Missile.move_flag = MOVED;
////				Ufo.move_flag = MOVED;
////
////				Ufo.timer = ZERO;
////				Missile.timer = ZERO;
////
////				Missile.cd_flag = OBJECT_CRASHED;
////				Ufo.cd_flag = OBJECT_CRASHED;
////
////				score++;
////		}
////	}
//}
//
//void Update_Ufo(void)
//{
//	Ufo.timer++;
//	if(Ufo.timer >= Ufo.speed_step)
//	{
//		Ufo.timer = ZERO;
//		Ufo.pos_back[Y] = Ufo.pos[Y];
//		Ufo.pos[Y] = Ufo.pos[Y] - Ufo.move_step;
//		Ufo.move_flag = MOVED;
//	}
//}
//
//
//void Update_Tank(int _direction)
//{
//	// only key input 1~4 can pass.
//	if(_direction < UP || _direction > RIGHT) return;
//
//	Tank.pos_back[X] = Tank.pos[X];			 // ������ ��ġ�� ������ ���´�.
//	Tank.pos_back[Y] = Tank.pos[Y];			 // ������ ��ġ�� ������ ���´�.
//	Tank.dir = _direction;
//	Tank.move_flag = MOVED;
//
//	switch(Tank.dir){
//		case UP:
//			Tank.pos[Y] = Tank.pos[Y] - Tank.move_step; // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
//			break;
//		case LEFT:
//			Tank.pos[X] = Tank.pos[X] - Tank.move_step; // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
//			break;
//		case DOWN:
//			Tank.pos[Y] = Tank.pos[Y] + Tank.move_step;  // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
//			break;
//		case RIGHT:
//			Tank.pos[X] = Tank.pos[X] + Tank.move_step;  // move_step �̹����� �󸶳� �̵���ų ���ΰ�?
//			break;
//		default: break;
//	}
//}
//
//
//void Update_Missiles(int _key)
//{
//	int i;
//	key_seq.cur = _key;
//	int _is_missile_moving = NOT_MOVED;
//
//	if(key_seq.pre == NOT_PRESSED_YET || key_seq.pre != key_seq.cur){
//
//		for(i=0; i<5; i++){
//			if(!Missiles[i].missile_flag) Missiles[i].dir = Tank.dir;
//			Missiles[i].timer++;
//		}
//
//		if(_key == FIRE && Tank.fired_cnt < 5 + 1) {
//			Tank.fired_cnt++;
//			if(Tank.fired_cnt > 5) Tank.fired_cnt = 5;
////			Uart_Printf("fired : %d\n", Tank.fired_cnt);
//			if(Missiles[Tank.fired_cnt - 1].missile_flag == NOT_FIRED){
//				Missiles[Tank.fired_cnt - 1].missile_flag = FIRED;
//				Missiles[Tank.fired_cnt - 1].launch_order = Tank.fired_cnt - 1;
//
//
//				Missiles[Tank.fired_cnt - 1].pos_init[X] = Tank.pos[X];
//				Missiles[Tank.fired_cnt - 1].pos_init[Y] = Tank.pos[Y];
//
//				Missiles[Tank.fired_cnt - 1].pos_back[X] = Missiles[Tank.fired_cnt - 1].pos_init[X];
//				Missiles[Tank.fired_cnt - 1].pos_back[Y] = Missiles[Tank.fired_cnt - 1].pos_init[Y];
//
//				Missiles[Tank.fired_cnt - 1].pos[X] = Missiles[Tank.fired_cnt - 1].pos_init[X];
//				Missiles[Tank.fired_cnt - 1].pos[Y] = Missiles[Tank.fired_cnt - 1].pos_init[Y];
//			}
//		}
//		key_seq.pre = key_seq.cur;
//	}
//
//
//	for(i=0; i < 5; i++){
//		_is_missile_moving = (Missiles[i].missile_flag != NOT_FIRED && Missiles[i].timer >= Missiles[i].speed_step);
//
//		if(_is_missile_moving)
//		{
//			printf("moving!! %d\n", i);
//			Missiles[i].timer = ZERO;
//			Missiles[i].move_flag = MOVED;
//
//			Missiles[i].pos_back[X] = Missiles[i].pos[X];			 // ������ ��ġ�� ������ ���´�.
//			Missiles[i].pos_back[Y] = Missiles[i].pos[Y];			 // ������ ��ġ�� ������ ���´�.
//
//			switch(Missiles[i].dir){
//				case UP:
//					Missiles[i].pos[Y] = Missiles[i].pos[Y] - Missiles[i].move_step;
//					break;
//				case LEFT:
//					Missiles[i].pos[X] = Missiles[i].pos[X] - Missiles[i].move_step;
//					break;
//				case DOWN:
//					Missiles[i].pos[Y] = Missiles[i].pos[Y] + Missiles[i].move_step;
//					break;
//				case RIGHT:
//					Missiles[i].pos[X] = Missiles[i].pos[X] + Missiles[i].move_step;
//					break;
//			}
//		}
//	}
//}
//
//
///*
//* ============================================
//* draw objects.
//* ============================================
//*/
//
//void Draw_Object(void)
//{
//	// DO NOT CHANGE PROCEDURE ORDER!!!!
//	Draw_Tank();
//	Draw_Ufo();
//	Draw_Missiles();
//	Check_Explosion();
////	Draw_Missile_Crashed();
////	Draw_Ufo_Crashed();
//}
//
//void Check_Explosion(void)
//{
////	if(Missile.cd_flag == OBJECT_CRASHED || Ufo.cd_flag == OBJECT_CRASHED)
////	{
////		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], WHITE);
////		Timer4_Delay(100);
////		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], BLUE);
////		Timer4_Delay(100);
////		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], RED);
////		Timer4_Delay(100);
////		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y]-40, Ufo.pos_back[X] + 30, Ufo.pos_back[Y], BLACK);
////	}
//}
//
//
//void Draw_Tank(void)
//{
//	// draw tank - edge cases
//	// if tank position exceed window then block
//	if((Tank.pos[X] > WINDOW_WIDTH - TANK_WIDTH - 1) ||
//	   (Tank.pos[X] < W_X_MIN)){
//		Tank.pos[X] = Tank.pos_back[X];
//	}
//
//	if((Tank.pos[Y] > WINDOW_HEIGHT - TANK_HEIGHT) ||
//	   (Tank.pos[Y] < W_Y_MIN)	){
//		Tank.pos[Y] = Tank.pos_back[Y];
//	}
//
//	// draw tank
//	// if tank moved, then flag will be enabled. after enabled, updated.
//	if(Tank.move_flag != NOT_MOVED) // �̹����� ����������, ���ο� ���� �׸���, �� ���� ��ǥ�� ������.
//	{
//		// remove previous state in lcd
//		Lcd_Draw_Bar(Tank.pos_back[X], Tank.pos_back[Y], Tank.pos_back[X] + Tank.size[X], Tank.pos_back[Y] + Tank.size[Y], BG_COLOR);
////		Uart_Printf("remove state ==>\n");
////		print_tank();
//		// draw current state in lcd
//		Lcd_Draw_Bar(Tank.pos[X], Tank.pos[Y], Tank.pos[X] + Tank.size[X], Tank.pos[Y] + Tank.size[Y], Tank.color);
////		Uart_Printf("draw state ==>\n");
////		print_tank();
//		// set to tank flag not moved.
//		Tank.move_flag = NOT_MOVED;
//	}
//}
//
//
//void Draw_Ufo(void)
//{
//	// draw ufo - edge case
//	// if ufo position off from the window, then set to beginning
//	if((Ufo.pos[Y] < W_Y_MIN))
//	{
//		Ufo.pos[Y] = Ufo.pos_init[Y];
//		Lcd_Draw_Bar(Ufo.pos_back[X], 0, Ufo.pos_back[X] + Ufo.size[X], 20, BG_COLOR);
//	}
//
//	// draw Ufo
//	// if tank moved, then flag will be enabled. after enabled, updated.
//	if(Ufo.move_flag != NOT_MOVED)
//	{
//		// remove previous state in lcd
//		Lcd_Draw_Bar(Ufo.pos_back[X], Ufo.pos_back[Y], Ufo.pos_back[X] + Ufo.size[X], Ufo.pos_back[Y] + Ufo.size[Y], BG_COLOR);
//		// draw current state in lcd
//		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y], Ufo.pos[X] + Ufo.size[X], Ufo.pos[Y] - Ufo.size[Y], Ufo.color);
//		// set to ufo flag not moved.
//		Ufo.move_flag = NOT_MOVED;
//	}
//}
//
//void Draw_Missiles()
//{
//	int i;
//	for(i=0; i<5; i++){
//		// draw tank beam - edge case
//		// if fired beam off from the window, then set the tank beam member flag as not fired.
//		if((Missiles[i].pos[Y] > W_Y_MAX - 1) ||
//			(Missiles[i].pos[Y] < W_Y_MIN + 1) ||
//			(Missiles[i].pos[X] < W_X_MIN + 1) ||
//			(Missiles[i].pos[X] > W_X_MAX - MISSILE_WIDTH - 1))
//		{
//			Missiles[i].missile_flag = NOT_FIRED;
//			Missiles[i].dir = Tank.dir;
//			Lcd_Draw_Bar(
//					Missiles[i].pos_back[X], Missiles[i].pos_back[Y],
//					Missiles[i].pos_back[X] + Missiles[i].size[X], Missiles[i].pos_back[Y] + Missiles[i].size[Y],
//					BG_COLOR);
//			// draw current state in lcd
//			Lcd_Draw_Bar(
//					Missiles[i].pos[X], Missiles[i].pos[Y],
//					Missiles[i].pos[X] + Missiles[i].size[X], Missiles[i].pos[Y] + Missiles[i].size[Y],
//					BG_COLOR);
//		}
//
//
//		// draw tank beam
//		// if tank beam moved, then flag will be enabled. after enabled, updated.
//		if(Missiles[i].move_flag != NOT_MOVED)
//		{
//			// remove previous state in lcd
//			Lcd_Draw_Bar(
//					Missiles[i].pos_back[X], Missiles[i].pos_back[Y],
//					Missiles[i].pos_back[X] + Missiles[i].size[X], Missiles[i].pos_back[Y] + Missiles[i].size[Y],
//					BG_COLOR);
//			// draw current state in lcd
//			Lcd_Draw_Bar(
//					Missiles[i].pos[X], Missiles[i].pos[Y],
//					Missiles[i].pos[X] + Missiles[i].size[X], Missiles[i].pos[Y] + Missiles[i].size[Y],
//					Missiles[i].color);
//			// set to tank beam flag not moved.
//			Missiles[i].move_flag = NOT_MOVED;
//		}
//	}
//}
////
////
////void Draw_Missile_Crashed(void)
////{
////	// draw tank beam object when crashed.
////	if(Missile.cd_flag == OBJECT_CRASHED)
////	{
////		// remove tank beam object, set color to black.
////		Lcd_Draw_Bar(Missile.pos[X],
////								 Missile.pos[Y],
////								 Missile.pos[X] + Missile.size[X],
////								 Missile.pos[Y] + Missile.size[Y],
////								 BG_COLOR
////							 );
////		// reverse state
////		Missile.cd_flag = OBJECT_NOT_CRASHED;
////		// set tank beam as not fired
////		Missile.missile_flag = NOT_FIRED;
////	}
////}
//void Draw_Ufo_Crashed(void)
//{
//	// draw ufo object when crashed.
//	if(Ufo.cd_flag == OBJECT_CRASHED)
//	{
//		// remove ufo object, set color to black.
//		Lcd_Draw_Bar(Ufo.pos[X], Ufo.pos[Y],
//								 Ufo.pos[X] + Ufo.size[X],
//								 Ufo.pos[Y] + Ufo.size[Y],
//								 BG_COLOR
//							 );
//		// reverse state
//		Ufo.cd_flag = OBJECT_NOT_CRASHED;
//		// set ufo position to beginning.
//		Ufo.pos[Y] = Ufo.pos_init[Y];
//	}
//}
//
//
///* =====================================
// * print states of objects
// * =====================================
// */
//void print_tank(void){
//	Uart_Printf("Timer : %d\n", Tank.timer);
//	Uart_Printf("move_flag : %d\n", Tank.move_flag);
//	Uart_Printf("pos[X] %d / pos[Y] %d\n", Tank.pos[X], Tank.pos[Y]);
//	Uart_Printf("pos_back[X] %d / pos_back[Y] %d\n", Tank.pos_back[X], Tank.pos_back[Y]);
//	Uart_Printf("size[X] %d / size[Y] %d\n", Tank.size[X], Tank.size[Y]);
//	Uart_Printf("tank direction %d\n", Tank.dir);
//}
//void print_ufo(void){
//	Uart_Printf("Ufo Timer : %d\n", Ufo.timer);
//	Uart_Printf("Ufo move_flag : %d\n", Ufo.move_flag);
//	Uart_Printf("Ufo pos[X] %d / pos[Y] %d\n", Ufo.pos[X], Ufo.pos[Y]);
//	Uart_Printf("Ufo pos_back[X] %d / pos_back[Y] %d\n", Ufo.pos_back[X], Ufo.pos_back[Y]);
//	Uart_Printf("Ufo size[X] %d / size[Y] %d\n", Ufo.size[X], Ufo.size[Y]);
//}
//
//void print_missile(int i){
//	Uart_Printf("Missiles[%d] Timer : %d\n", Missiles[i].timer);
//	Uart_Printf("Missiles[%d] move_flag : %d\n", Missiles[i].move_flag);
//	Uart_Printf("Missiles[%d] pos[X] %d / pos[Y] %d\n", Missiles[i].pos[X], Missiles[i].pos[Y]);
//	Uart_Printf("Missiles[%d] pos_back[X] %d / pos_back[Y] %d\n", Missiles[i].pos_back[X], Missiles[i].pos_back[Y]);
//	Uart_Printf("Missiles[%d] size[X] %d / size[Y] %d\n", Missiles[i].size[X], Missiles[i].size[Y]);
//	Uart_Printf("Missiles[%d] dir : %d\n", Missiles[i].dir);
//	Uart_Printf("Missiles[%d] launch_order : %d\n", Missiles[i].launch_order);
//}
