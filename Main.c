/***********************************************************/
// 다음은 사용자 함수를 호출하는 루틴임 지우지 말것!
/***********************************************************/
#if 1
#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>
typedef struct
{
	int day;
	int year;
	int mon;
	int date;
	int hour;
	int min;
	int sec;
}RTC_TIME;

int RTC_Get_Time(void)
{
	RTC_TIME a;
	/*
	a.day = (int)rBCDDAY;
	a.year = (int)rBCDYEAR;
	a.mon = (int)rBCDMON;
	a.date = (int)rBCDDATE;
	a.hour = (int)rBCDHOUR;
	a.min = (int)rBCDMIN;
	a.sec = (int)rBCDSEC;

	if(!a.sec)
	{
		a.day = (int)rBCDDAY;
		a.year = (int)rBCDYEAR;
		a.mon = (int)rBCDMON;
		a.date = (int)rBCDDATE;
		a.hour = (int)rBCDHOUR;
		a.min = (int)rBCDMIN;
	}

	time->day = a.day;
	time->year = ((a.year >> 4) & 0xf) * 10 + (a.year & 0xf);
	time->mon = ((a.mon >> 4) & 0xf) * 10 + (a.mon & 0xf);
	time->date = ((a.date >> 4) & 0xf) * 10 + (a.date & 0xf);
	time->hour = ((a.hour >> 4) & 0xf) * 10 + (a.hour & 0xf);
	time->min = ((a.min >> 4) & 0xf) * 10 + (a.min & 0xf);
	time->sec = ((a.sec >> 4) & 0xf) * 10 + (a.sec & 0xf);
	*/
	a.hour = (int)rBCDHOUR;
	a.min = (int)rBCDMIN;
	a.sec = (int)rBCDSEC;
	return  ((a.hour >> 4) & 0xf) * 10 + (a.hour & 0xf) * 360 + \
			((a.min >> 4) & 0xf) * 10 + (a.min & 0xf) * 60 + \
			((a.sec >> 4) & 0xf) * 10 + (a.sec & 0xf);

}

#define printf 	Uart_Printf

void User_Main(void);

void Main(void)
{
	MMU_Init();
	Led_Init();
	Key_Poll_Init();
	Uart_Init(115200);
	Timer0_Init();
	Timer3_Buzzer_Init();
	Timer4_Init();
	Lcd_Graphic_Init();
	Lcd_Clr_Screen(0xf800);

	Uart_Printf("Welcome GBOX World!\n");
	User_Main();
	Uart_Printf("Good Bye~\n");
}

/*===== << 여기부터 사용자 프로그램을 작성함 >> =====*/

#include "./Image/img01.h"
#include "./Image/img02.h"
#include "./Image/img03.h"
#include "./Image/img04.h"
#include "./Image/img05.h"
#include "./Image/shape.h"
#include "./Image/perfect.h"

const unsigned short * Disp[] = {img01, img02, img03, img04};

/* 5:5:5:I Color Definition */

#define BLACK	0x0000
#define WHITE	0xfffe
#define BLUE	0x003e
#define GREEN	0x07c0
#define RED		0xf800

void Basic_Graphics_Test(void);
void Special_Mode_Test(void);
void Font_Test(void);
void Brightness_Test(void);
void Key_Test(void);
void Buzzer_Test(void);
void Timer_Test(void);
void Simple_Example(void);
//void My_Game(void);

void User_Main(void)
{
	int r;
	srand(RTC_Get_Time());

	for(;;)
	{
		Lcd_Control_Brightness(5);
		Lcd_Select_Buffer_Mode(LAYER_MODE);
	 	Lcd_Select_Draw_Frame_Buffer(0);
	 	Lcd_Select_Display_Frame_Buffer(0);
	 	Lcd_Clr_Screen(RED);
		Lcd_Printf(10,10,GREEN,RED,1,1,"Willtek 임베디드 전문가 그룹");

		Uart_Printf("===================================================\n");
		Uart_Printf("GBOX WT2440L API Function Test    (c)Willtek Corp. \n");
		Uart_Printf("===================================================\n");
		Uart_Printf("[1] Basic Graphics Test		                   			\n");
		Uart_Printf("[2] Special Graphics Mode Test                    	\n");
		Uart_Printf("[3] Font Display Test                            	\n");
		Uart_Printf("[4] Brightness Test                              	\n");
		Uart_Printf("[5] Buzzer Test                    								\n");
		Uart_Printf("[6] Key Test		                   									\n");
		Uart_Printf("[7] Timer Test			                            		\n");
		Uart_Printf("[8] Simple Example		                            	\n");
		Uart_Printf("===================================================\n");
		Uart_Printf(">> ");

		r = Uart_GetIntNum();
		Uart_Get_Pressed();

		if((unsigned int)r >= 11)
		{
			Uart_Printf("Wrong Number!\n");
			continue;
		}

		switch(r)
		{
			case 1 : Basic_Graphics_Test(); break;
			case 2 : Special_Mode_Test(); break;
			case 3 : Font_Test(); break;
			case 4 : Brightness_Test(); break;
			case 5 : Buzzer_Test(); break;
			case 6 : Key_Test(); break;
			case 7 : Timer_Test(); break;
			case 8 : Simple_Example(); break;
//			case 9 : My_Game(); break;
			default : Uart_Printf("Wrong Number!\n"); break;
		}

		Uart_Printf("Test Finished\n\n");
	}
}

void Basic_Graphics_Test(void)
{
//	for(;;)
//	{
//		Uart_Printf("random num = %d\n", rand()%100);
//	}
	int i;
	int xtmp, ytmp;

	Uart_Printf("\n그래픽 기본 함수 실습\n");
	Uart_Printf("키보드 누를때마다 다음단계로 진행\n");

	while(!Uart_Get_Pressed());

	Uart_Printf("Clear Screen\n");
	Lcd_Clr_Screen(BLUE);

	while(!Uart_Get_Pressed());

	Uart_Printf("Draw HLINE, VLINE, LINE\n");
	Lcd_Draw_Hline(100,50,200,RED);
	Lcd_Draw_Vline(80,100,200,RED);
	Lcd_Draw_Line(0,0,319,239,GREEN);

	while(!Uart_Get_Pressed());

	Uart_Printf("Draw Rectangular\n");
	Lcd_Clr_Screen(BLACK);
	Lcd_Draw_Rect(20,20,120,120,RED);
	Lcd_Draw_Rect(70,70,220,220,BLUE);

	while(!Uart_Get_Pressed());

	Uart_Printf("Draw BAR\n");
	Lcd_Clr_Screen(BLACK);
	Lcd_Draw_Bar(0,0,100,100,RED);
	Lcd_Draw_Bar(50,50,200,200,BLUE);

	while(!Uart_Get_Pressed());

	Uart_Printf("Draw BMP\n");
	Uart_Printf("Get BMP Information\n");
	Lcd_Get_Info_BMP(&xtmp, &ytmp, perfect);
	Uart_Printf("X=[%d], Y=[%d]\n", xtmp, ytmp);
	Lcd_Draw_BMP((160-(xtmp/2)), (120-(ytmp/2)), perfect);

	for(i=0; i<(sizeof(Disp)/sizeof(Disp[0])); i++)
	{
		Lcd_Get_Info_BMP(&xtmp, &ytmp, Disp[i]);
		Uart_Printf("X=[%d], Y=[%d]\n", xtmp, ytmp);
		Lcd_Draw_BMP((160-(xtmp/2)), (120-(ytmp/2)), Disp[i]);
		Timer4_Delay(1000);
	}

	Uart_Printf("키보드 누르면 종료\n");
	while(!Uart_Get_Pressed());
}

void Special_Mode_Test(void)
{
	int xtmp, ytmp;

	Uart_Printf("\n그래픽 기본 함수 실습\n");
	Uart_Printf("키보드 누를때마다 다음단계로 진행\n");

	Uart_Printf("Non Transparency Mode\n");

	Lcd_Clr_Screen(BLACK);
	Lcd_Draw_Bar(0,0,100,100,RED);
	Lcd_Draw_Bar(50,50,200,200,BLUE);

	while(!Uart_Get_Pressed());

	Uart_Printf("Transparency Mode\n");

	Lcd_Clr_Screen(BLACK);
	Lcd_Set_Trans_Mode(1);
	Lcd_Draw_Bar(0,0,100,100,RED);
	Lcd_Draw_Bar(50,50,200,200,BLUE);
	Lcd_Set_Trans_Mode(0);

	while(!Uart_Get_Pressed());

	Uart_Printf("Non Shape Mode\n");

	Lcd_Clr_Screen(GREEN);
    Lcd_Get_Info_BMP(&xtmp, &ytmp, shape);
    Lcd_Draw_BMP((160-(xtmp/2)), (120-(ytmp/2)), shape);

	while(!Uart_Get_Pressed());

	Uart_Printf("Shape Mode(Chroma Key Mode)\n");

    Lcd_Set_Shape_Mode(1,0xFFFE);
	Lcd_Clr_Screen(GREEN);
    Lcd_Get_Info_BMP(&xtmp, &ytmp, shape);
    Lcd_Draw_BMP((160-(xtmp/2)), (120-(ytmp/2)), shape);
    Lcd_Set_Shape_Mode(0,0);

	Uart_Printf("키보드 누르면 종료\n");
	while(!Uart_Get_Pressed());
}

void Font_Test(void)
{
	int i, j;
	char str[20];

	Uart_Printf("영문 Nickname(20글자 이내)");
	Uart_GetString(str);
	Uart_Printf("\nFont Display\n");
	Lcd_Printf(2,2,BLACK,RED,2,2,"윌텍 임베디드 그룹");
	Lcd_Printf(0,150,GREEN,BLACK,1,1,"%s", "(주)윌텍,(주)행복한기술");
	Lcd_Printf(0,170,GREEN,BLACK,1,1,"%s", "(주)기쁜기술,(주)임베딕스,(주)에듀닉스");
	Lcd_Printf(30,100,RED,GREEN,2,1,"윌텍은 %d개 회사", 5);
	Lcd_Printf(0,200,BLUE,GREEN,1,2,"화이팅! %s", str);

	for(i=0; i<10; i++)
	{
		for(j=0; j<10; j++)
		{
			Lcd_Put_Pixel(50+i, 80+j, BLUE);
		}
	}

	Uart_Printf("키보드 누르면 종료\n");
	while(!Uart_Get_Pressed());
}

void Brightness_Test(void)
{
	int b;

	Uart_Printf("Brightness Test\n");

	Lcd_Clr_Screen(BLACK);
	Lcd_Draw_BMP(0,0,img01);

	for(;;)
	{
		Uart_Printf("0 누르면 종료\n");
		Uart_Printf("밝기 입력 [1~9] => ");

		b = Uart_GetIntNum();
		if(b == 0) break;
		if(b > 9)
		{
			Uart_Printf("똑바로 눌러라 잉~\n");
			continue;
		}
		Lcd_Control_Brightness(b);
	}
}

enum key{C1, C1_, D1, D1_, E1, F1, F1_, G1, G1_, A1, A1_, B1, C2, C2_, D2, D2_, E2, F2, F2_, G2, G2_, A2, A2_, B2};
enum note{N16, N8, N4, N2, N1};
const int song[][2] = {{G1,N4},{G1,N4},{E1,N8},{F1,N8},{G1,N4},{A1,N4},{A1,N4},{G1,N2},{G1,N4},{C2,N4},{E2,N4},{D2,N8},{C2,N8},{D2,N2},{0xff, 0xff}};

void Buzzer_Test(void)
{
	int i = 0;

	Uart_Printf("Buzzer Test\n");

	Timer3_Buzzer_Beep(C1,N4);
	Timer3_Buzzer_Beep(D1,N4);
	Timer3_Buzzer_Beep(E1,N4);
	Timer3_Buzzer_Beep(F1,N4);
	Timer3_Buzzer_Beep(G1,N4);
	Timer3_Buzzer_Beep(A1,N4);
	Timer3_Buzzer_Beep(B1,N4);
	Timer3_Buzzer_Beep(C2,N4);

	Timer4_Delay(1000);

	while(song[i][0] != 0xff)
	{
		Timer3_Buzzer_Beep(song[i][0], song[i][1]);
		Uart_Printf(".");
		i++;
	}

	Uart_Printf("\n");
}

void Key_Test(void)
{
	int i;
	int key;

	Uart_Printf("9개의 KEY를 눌러봅시다\n");

	for(i=0; i<9; i++)
	{
		key = Key_Wait_Key_Pressed();
		Uart_Printf("Key = %d\n", key);
		Timer4_Delay(400);

		// 다음 코드의 유무에 따른 동작을 확인하라
		Key_Wait_Key_Released();
	}

	Uart_Printf("\n5번 버튼([1]) 누를때까지 계속 LED 깜박임\n");

	do
	{
		Led_Display(0x5);
		Timer4_Delay(100);
		Led_Display(0xA);
		Timer4_Delay(100);
	}while(Key_Get_Pressed() != 5);

	Uart_Printf("여러 키를 동시에 눌러 봅시다 총 5번 동작\n");
	Key_Wait_Key_Released();

	for(i=0; i<5; i++)
	{
		key = Key_Wait_Key_Pressed_Multi();
		Uart_Printf("Key = %#X\n", key);
		Timer4_Delay(400);

		Key_Wait_Key_Released_Multi();
	}

	Uart_Printf("\n5번 버튼([1])만 눌릴때까지 계속 key 상태를 표시함\n");

	do
	{
		Timer4_Delay(400);
		key = Key_Get_Pressed_Multi();
		Uart_Printf("Key = %#X\n", key);
	}while(key != 0x10);
}

void Timer_Test(void)
{
	int i;
	int r;
	int disp;

	Uart_Printf("Timer Test\n");

	Uart_Printf("Timer4_Delay\n");

	for(i=1; i<=5; i++)
	{
		Timer4_Delay(1000);
		Uart_Printf("%d\r", i);
	}

	Uart_Printf("\n");

	Uart_Printf("\nTimer1_Stopwatch\n");
	Uart_Printf("키보드 누르면 시작\n");
	while(!Uart_Get_Pressed());

	Timer4_Delay(1000);

	for(i=1; i<=5; i++)
	{
		Uart_Printf("Stopwatch Run\n");
		Timer1_Stopwatch_Run();
		Timer4_Delay(200*i);
		r = Timer1_Stopwatch_Stop();
		Uart_Printf("Elapsed Time = %d msec\r", r);
	}

	Uart_Printf("\nTimer0_Repeat(최대 1.3초)\n");
	Uart_Printf("키보드 누르면 시작\n");
	while(!Uart_Get_Pressed());

	Timer0_Repeat(500);
	disp = 0xf;

	for(i=1;;)
	{
		if(Timer0_Check_Expired())
		{
			Led_Display(disp = ~disp);
			Uart_Printf("[%d]\n", i);
			i++;
			if(i>10) break;
		}
	}

	Timer0_Stop();

	Uart_Printf("\nTimer0_Repeat => 주기 변경(최대 약 1.3초)\n");
	Uart_Printf("주기 설정이 1300msec가 넘어가면 어떻게 되는가?\n");
	Uart_Printf("키보드 누르면 시작\n");
	while(!Uart_Get_Pressed());

	Timer0_Repeat(100);
	disp = 0xf;

	for(i=1;;)
	{
		if(Timer0_Check_Expired())
		{
			Led_Display(disp = ~disp);
			Timer0_Change_Delay(100*(i+1));
			Uart_Printf("[%d] Interval=%d msec\n", i, 100*(i+1));
			i++;
			if(i>20) break;
		}
	}
}

#define BACK_COLOR 	BLACK
#define WIDTH 		320
#define HEIGHT 		240

unsigned short make_pixel(int r, int g, int b)
{
	return (r>>3)<<11 | (g>>3)<<6 | (b>>3) << 1 ;
}

struct Object
{
	int timer;     	 	  // 몇번 타이머를 쓸 것인지?
	int move_flag;   	  // 움직이고 있는 상태인지 아니지,
	int pos[2]; 		  // x,y
	int pos_init[2];  	  // 초기 좌표.
	int pos_back[2];      // 이전 좌표의 정보. 이미지를 사용할 시, 이 좌표를 토대로 이전의 이미지를 지워야함.
	int size[2];	      // width, height

	unsigned short color; // 나중에 이미지로 대체.
	int speed_step;	 	  // 이미지가 얼마나 빨리 이동되게 할 것인지.
	int move_step;		  // 이미지를 얼마나 이동시킬 것인가?
	int beam_flag;		  // beam 발사 됐는지 여부 flag
	int cd_flag;		  // collision detection flag
	int dir[2];           // x,y 방향의 방향정보. 1과 -1 값이 있다.
};

struct Object Tank = {0,1,{0,10},{0,10},{0,10},{30,10},
					RED, 4, 8, 0, 0, {1,1}};
struct Object Gun = {0,1,{160,300},{160,300},{160,300},{30,30},
					BLUE, 4, 7, 0, 0, {1,1}};
struct Object Tank_beam = {0,1,{319,239},{319,239},{319,239},{10,10},
					GREEN, 3, 10, 0, 0, {1,1}};

void explosion(void)
{
	if(Tank_beam.cd_flag == 1 || Gun.cd_flag == 1)
	{
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1]-40, Gun.pos_back[0] + 30, Gun.pos_back[1], WHITE);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1]-40, Gun.pos_back[0] + 30, Gun.pos_back[1], BLUE);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1]-40, Gun.pos_back[0] + 30, Gun.pos_back[1], RED);
		Timer4_Delay(100);
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1]-40, Gun.pos_back[0] + 30, Gun.pos_back[1], BLACK);
	}
}

void Move_Object(void)
{
	int key = 0;
	if(Timer0_Check_Expired())
	{
//		Tank.timer++;
		Gun.timer++;
		Tank_beam.timer++;

		key = Key_Get_Pressed();
//		Uart_Printf("key : %d\n", key);
//		if(key == 2){
//			Tank.timer = 0;
//			Tank.pos_back[0] = Tank.pos[0];			 // 현재의 위치를 벡업해 놓는다.
//			Tank.pos[0] = Tank.pos[0] + Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
//			Tank.move_flag = 1;						 //
//		}

		// 탱크를 우측으로 움직임.
		if(Tank.timer >= Tank.speed_step || key == 4)
		{
			Tank.timer++;
			Tank.timer = 0;
			Tank.pos_back[0] = Tank.pos[0];			 // 현재의 위치를 벡업해 놓는다.
			Tank.pos[0] = Tank.pos[0] + Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
			Tank.move_flag = 1;						 //
		}
		// 탱크를 좌측으로 움직임.
		if(Tank.timer >= Tank.speed_step || key == 2)
		{
			Tank.timer++;
			Tank.timer = 0;
			Tank.pos_back[0] = Tank.pos[0];			 // 현재의 위치를 벡업해 놓는다.
			Tank.pos[0] = Tank.pos[0] - Tank.move_step; // move_step 이미지를 얼마나 이동시킬 것인가?
			Tank.move_flag = 1;						 //
		}

		if(Gun.timer >= Gun.speed_step)
		{
			Gun.timer = 0;
			Gun.pos_back[1] = Gun.pos[1];
			Gun.pos[1] = Gun.pos[1] - Gun.move_step;
			Gun.move_flag = 1;
		}
		if(key == 5)
		{
			if(Tank_beam.beam_flag == 0)
			{
				Tank_beam.beam_flag = 1;
			}
		}
		if(Tank_beam.beam_flag == 0)
		{
			Tank_beam.pos_init[0] = Tank.pos[0]+13;
			Tank_beam.pos_init[1] = Tank.pos[1]+12;
			Tank_beam.pos_back[0] = Tank_beam.pos_init[0];
			Tank_beam.pos_back[1] = Tank_beam.pos_init[1];
			Tank_beam.pos[0] = Tank_beam.pos_init[0];
			Tank_beam.pos[1] = Tank_beam.pos_init[1];
		}
		if(Tank_beam.beam_flag != 0 && Tank_beam.timer >= Tank_beam.speed_step)
		{
			Tank_beam.timer = 0;
			Tank_beam.pos_back[1] = Tank_beam.pos[1];
			Tank_beam.pos[1] = Tank_beam.pos[1] + Tank_beam.move_step;
			Tank_beam.move_flag = 1;
		}

//		Uart_Printf("Tank.timer %d / Tank.speed_step %d\n", Tank.timer, Tank.speed_step);
	}
}

void Draw_Object(void)
{
	if((Tank.pos[0] >= WIDTH))
	{
		Tank.pos[0] = Tank.pos_init[0];
	}

	if((Gun.pos[1] < 0))
	{
		Gun.pos[1] = Gun.pos_init[1];
		Lcd_Draw_Bar(Gun.pos_back[0], 0, Gun.pos_back[0] + Gun.size[0], 20, BACK_COLOR);
	}
	if((Tank_beam.pos[1] > 239))
	{
		Tank_beam.beam_flag = 0;
	}

	if(Tank.move_flag != 0) // 이미지가 움직였으면, 새로운 곳을 그리고, 그 이전 좌표는 지운다.
	{
		Lcd_Draw_Bar(Tank.pos_back[0], Tank.pos_back[1], Tank.pos_back[0] + Tank.size[0], Tank.pos_back[1] + Tank.size[1], BACK_COLOR);
		Lcd_Draw_Bar(Tank.pos[0], Tank.pos[1], Tank.pos[0] + Tank.size[0], Tank.pos[1] + Tank.size[1], Tank.color);
		Tank.move_flag = 0;
	}
	if(Gun.move_flag != 0)
	{
		Lcd_Draw_Bar(Gun.pos_back[0], Gun.pos_back[1], Gun.pos_back[0] + Gun.size[0], Gun.pos_back[1] + Gun.size[1], BACK_COLOR);
		Lcd_Draw_Bar(Gun.pos[0], Gun.pos[1], Gun.pos[0] + Gun.size[0], Gun.pos[1] - Gun.size[1], Gun.color);
		Gun.move_flag = 0;
	}
	if(Tank_beam.move_flag != 0)
	{
		Lcd_Draw_Bar(Tank_beam.pos_back[0], Tank_beam.pos_back[1], Tank_beam.pos_back[0] + Tank_beam.size[0], Tank_beam.pos_back[1] + Tank_beam.size[1], BACK_COLOR);
		Lcd_Draw_Bar(Tank_beam.pos[0], Tank_beam.pos[1], Tank_beam.pos[0] + Tank_beam.size[0], Tank_beam.pos[1] + Tank_beam.size[1], Tank_beam.color);
		Tank_beam.move_flag = 0;
	}
	explosion();
	if(Tank_beam.cd_flag == 1)
	{
		Lcd_Draw_Bar(Tank_beam.pos[0], Tank_beam.pos[1], Tank_beam.pos[0] + Tank_beam.size[0], Tank_beam.pos[1] + Tank_beam.size[1], BACK_COLOR);
		Tank_beam.cd_flag = 0;
		Tank_beam.beam_flag = 0;
	}
	if(Gun.cd_flag == 1)
	{
		Lcd_Draw_Bar(Gun.pos[0], Gun.pos[1],
				Gun.pos[0] + Gun.size[0],
				Gun.pos[1] + Gun.size[1],
				BACK_COLOR);
		Gun.cd_flag = 0;
		Gun.pos[1] = Gun.pos_init[1];
	}
}

void collision_detect(void)
{
	if(Tank_beam.beam_flag != 0 && Tank_beam.cd_flag == 0)
	{
		if((Tank_beam.pos[0] > Gun.pos[0]) && \
		   (Tank_beam.pos[0] + Tank_beam.size[0] < Gun.pos[0] + Gun.size[0]) )
		{
			if((Tank_beam.pos[1] + Tank_beam.size[1]  >= Gun.pos[1]))
			{

				Tank_beam.move_flag = 1;
				Gun.move_flag = 1;

				Gun.timer = 0;
				Tank_beam.timer = 0;

				Tank_beam.cd_flag = 1;
				Gun.cd_flag = 1;
			}
		}
	}

}

void Game_Init(void)
{
	Lcd_Clr_Screen(BACK_COLOR);
	Timer0_Repeat(20);
}

void Simple_Example(void)
{
	Game_Init();
	Draw_Object();

	for(;;)
	{
		Move_Object();
		collision_detect();
		Draw_Object();


	}

}
#endif







//#if 0
//
//#include "2440addr.h"
//#include "device_driver.h"
//#include "macro.h"
//
//extern volatile int Touch_pressed;
//extern volatile int Touch_x, Touch_y;
//
//void Main(void)
//{
//	Uart_Init(115200);
//	Uart_Printf("Touch Interrupt Test\n");
//
//	Touch_ISR_Enable(1);
//
//	for(;;)
//	{
//		if(Touch_pressed)
//		{
//			Uart_Printf("Pressed: X=%u,Y=%u\n", Touch_x, Touch_y);
//			while(Touch_pressed);
//			Uart_Printf("Released\n");
//		}
//	}
//}
//
//#endif
