

void Galaxy_Maestro(void)
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
