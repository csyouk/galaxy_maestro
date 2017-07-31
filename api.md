---------------------------------------------------------------
# API list
## Uart communication (I/O)
- Uart_Printf("문자열")
  - tera term 화면에 문자열 출력.
- Uart_Get_Pressed()
  - PC의 키 입력을 받아들임.
  - 아무 입력이 없으면 0, 입력이 있다면 그에 해당하는 캐릭터 값 리턴.
- Lcd_Clr_Screen(BLUE);
  - LCD를 정해진 색으로 채움.

## 선, 면
- void Lcd_Draw_Rect(int x1, int y1, int x2, int y2, int color)
  - 속이 빈 사각형.
- void Lcd_Draw_Bar(int x1, int y1, int x2, int y2, int color)
  - 속이 찬 사각형.
- void Lcd_Draw_Line(int x1,int y1,int x2,int y2,int color)
- void Lcd_Draw_Vline(int x, int y1, int y2, int color)
- void Lcd_Draw_Hline(int y, int x1, int x2, int color)

## BMP 이미지
- void Lcd_Get_Info_BMP(int * x, int  * y, const unsigned short int \*fp)
- void Lcd_Draw_BMP(int x, int y, const unsigned short int \*fp)

## 문자
- void Lcd_Puts(int x, int y, int color, int bkcolor, char \*str, int zx, int zy)
  - 원하는 위치 x,y에 원하는 색(color)으로 문자열 인쇄.

## 키 스위치(단일 키 입력)
- int Key_Wait_Key_Pressed(void)
  - 키가 눌릴 떄까지 기다린 후 눌린 키의 번호를 리턴.
  - 아무 입력이 없으면 무한 대기.
- void Key_Wait_Key_Released(void)
  - 키가 안눌릴 떄까지 무한히 기다리는 함수.
- int Key_Get_Pressed(void)
  - 함수 호출하면 눌린 키가 있으면 키 번호를 리턴.

## 키 스위치(멀티 키 입력.)
- int Key_Wait_Key_Pressed_Multi(void)
  - 눌린 키들의 정보를 리턴.

## 타이머
- void Timer4_Delay(unsigned int msec)
  - 주어진 msec 만큼 시간을 지연.
  - 게임 종료시, 화면정지시. 사용

### 타이머 1은 스탑워치 용이다.
- void Timer1_Stopwatch_Run(void)
  - 타이머 1번을 구동. 최대 측정 시간 1.3초
- int Timer1_Stopwatch_Stop(void)
  - 타이머 1번을 세움. 경과시간 msec로 리턴.

### 타이머 0은 timeout 함수이다.
- void Timer0_Repeat(int time)
  - 타이머 0번을 msec 단위 시간으로 계속 반복하도록 함.(최대 1.3초)
  - 시간을 배분할 때 사용.
- int Timer0_Check_Expired(void)
  - 타이머 0번에게 주어진 시간이 경과했는지 알려줌.
- void Timer0_Change_Delay(int msec)
  - 타이머 0을 세우지 않고, 주기만 바꿔줌.
  - 주기는 인자로 넘기는  signed int이다.
  - 단위는 msec



---------------------------------------------------------------
# Code
```cpp
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
};
```
