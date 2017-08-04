//#define BEEP 5
//#define CRASHED 6

enum key{C1, C1_, D1, D1_, E1, F1, F1_, G1, G1_, A1, A1_, B1, C2, C2_, D2, D2_, E2, F2, F2_, G2, G2_, A2, A2_, B2};
enum note{N16, N8, N4, N2, N1, BEEP, CRASHED};


extern char s_score[3];

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

void Sound_Effect(int effect)
{
	switch(effect){
	default:
		Uart_Printf("effect : %d\n", effect);
	case BEEP:
		Timer3_Buzzer_Beep(E2,BEEP);
		Timer3_Buzzer_Beep(D2,BEEP);
		break;
	case CRASHED:
		Timer3_Buzzer_Beep(D1,CRASHED);
		Timer3_Buzzer_Beep(C1,CRASHED);
		break;
	}

}
