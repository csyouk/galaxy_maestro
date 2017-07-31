void Galaxy_Maestro(void);


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

/* 5:5:5:I Color Definition */

extern enum Color{
	BLACK=0x0000,
	WHITE=0xfffe,
	BLUE=0x003e,
	GREEN=0x07c0,
	RED=0xf800
};

struct Object Tank = {0,1,{0,10},{0,10},{0,10},{30,10},
					RED, 4, 8, 0, 0, {1,1}};
struct Object Gun = {0,1,{160,300},{160,300},{160,300},{30,30},
					BLUE, 4, 7, 0, 0, {1,1}};
struct Object Tank_beam = {0,1,{319,239},{319,239},{319,239},{10,10},
					GREEN, 3, 10, 0, 0, {1,1}};
