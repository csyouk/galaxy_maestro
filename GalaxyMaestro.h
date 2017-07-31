void Galaxy_Maestro(void);


struct Object
{
	int timer;     	 	  // ��� Ÿ�̸Ӹ� �� ������?
	int move_flag;   	  // �����̰� �ִ� �������� �ƴ���,
	int pos[2]; 		  // x,y
	int pos_init[2];  	  // �ʱ� ��ǥ.
	int pos_back[2];      // ���� ��ǥ�� ����. �̹����� ����� ��, �� ��ǥ�� ���� ������ �̹����� ��������.
	int size[2];	      // width, height

	unsigned short color; // ���߿� �̹����� ��ü.
	int speed_step;	 	  // �̹����� �󸶳� ���� �̵��ǰ� �� ������.
	int move_step;		  // �̹����� �󸶳� �̵���ų ���ΰ�?
	int beam_flag;		  // beam �߻� �ƴ��� ���� flag
	int cd_flag;		  // collision detection flag
	int dir[2];           // x,y ������ ��������. 1�� -1 ���� �ִ�.
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
