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
