==== 1.

1. Tank 좌우 이동
1.1 플레이어 4방향 이동.
2. 미사일 발사(단발 발사)

==== 2.

3. UFO 왕복이동      
4. UFO 가변 동작. 랜덤으로 ufo 출현.

==== 3.

5. 플레이어가 발사한 미사일이 적 비행기와 충돌했을 때의 충돌감지.
6. 충돌 감지 후, 파괴 행위 구현.

==== 4.

7. 점수체계 구비 후 화면에 보임.

---------------------------------------------------------------
```cpp
struct Object
{
	int timer;     	 	  // 몇번 타이머를 쓸 것인지?
	int move_flag;   	  // 움직이고 있는 상태인지 아니지,
	int pos[X_COMMA_Y]; 		  // x,y
	int pos_init[X_COMMA_Y];  	  // 초기 좌표.
	int pos_back[X_COMMA_Y];      // 이전 좌표의 정보. 이미지를 사용할 시, 이 좌표를 토대로 이전의 이미지를 지워야함.
	int size[X_COMMA_Y];	      // width, height
	unsigned short color; // 나중에 이미지로 대체.
	int speed_step;	 	  // 이미지가 얼마나 빨리 이동되게 할 것인지.
	int move_step;		  // 이미지를 얼마나 이동시킬 것인가?
	int beam_flag;		  // beam 발사 됐는지 여부 flag
	int cd_flag;		  // collision detection flag
  int dir[X_COMMA_Y];           // x,y 방향의 방향정보. 1과 -1 값이 있다.
  int missile_dir;      // 1,2,3,4번 키를 누름에 따라 미사일의 방향이 정해진다. 차례대로, up, left, down, right
};
```


-------------------------------------------
### logs
- ufo, tank, tank_missile 3개의 객체가 있다.
- 170731 파일 실행부를 따로 만들어서 파일로 따로 빼냈다.

#### UFO
- ufo가 벽에 부딪혔을 떄, 반사.
- ufo의 초기 비행경로는 랜덤함.
- 화면의 경계지점에 다가가면 반사됨.

#### Tank
- 탱크는 위,아래 좌,우로 움직여야 한다.
- 1,2,3,4번 각 키를 누를때마다 방향이 변한다.
- 탱크는 타이머가 필요 없다. 왜냐하면 오직 키의 입력값에 의해서만 움직이기 때문이다.

#### Tank_Missile
- 5번 키를 누르면 빔이 나간다.

------------------------------------
1. 탱크가 화면을 못벗어나게 만든다.(o)
2. 탱크가 위, 아래로 움직일 시 궤적을 없앤다.
3. 랜덤 함수.
  - #include <stdlib.h>
  - #include "2440addr.h"
  - srand(RTC_Get_Time())
4. 그림
  - 이미지 1개 디스플레이.
  - 헤더 파일 변환. image/BMP2RGB.exe파일 실행.
     *.bmp를 *.h로 변환한다. **
  - 이미지 그리기 : 헤더파일 첨부 후
  - Lcd_Draw_BMP(50,50,img_name) 로 이미지를 그림
  - 포토샵 서비스 가서 사용.
  - 한 픽셀달 2바이트의 정보가 들어가 있음.
5. ufo를 여러개 만든다. 초기값은 서로 다르게 줌.
