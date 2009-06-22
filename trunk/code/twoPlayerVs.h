
class twoPlayerVs: public dogFight
{
public:
	void draw()//MAIN DRAW FUNCTION
	{
		glClearColor(0.5f,0.8f,0.9f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, sh/2 + 2, sw, sh/2);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(80.0, (double)sw / ((double)sh/2),1.0, 5000.0);
		frustum.setCamInternals(80.0, (double)sw / ((double)sh/2),1.0, 5000.0);
		acplayer=0;
		drawScene2();

		glViewport(0, 0, sw, sh/2 - 2);
		acplayer=1;
		drawScene2();
	}

	int update(int value) {
		for(int i=0;i<2;i++)
		{
			planes[i]->Accelerate	((float)value*controls[i][CON_ACCEL]/33000);
			planes[i]->Brake		((float)value*controls[i][CON_BRAKE]/33000);
			planes[i]->Climb		((float)value*controls[i][CON_CLIMB]/33000);
			planes[i]->Dive			((float)value*controls[i][CON_DIVE]/33000);
			planes[i]->Turn_Right	((float)value*controls[i][CON_RIGHT]/33000);
			planes[i]->Turn_Left	((float)value*controls[i][CON_LEFT]/33000);
			if(controls[i][CON_MISSILE]>300)	planes[i]->ShootMissile();
			if(controls[i][CON_SHOOT]>300)		planes[i]->Shoot();
		}
		if(input->keys[0x4C]) planes[0]->die();
		if(input->keys[F1]){firstP[0] = !firstP[0];input->keys[F1]=false;}
		if(input->keys[F2]){firstP[1] = !firstP[1];input->keys[F2]=false;}
		updateWorld(value);
		radarAng+=45.0*value/1000;
		if(radarAng>=360)
			radarAng-=360;

		Redisplay=true;
		return 33;
	}
	twoPlayerVs()
	{
		prepareMap("media2/mapfile.txt");
		newMode=0;
		Cmenu=new closedMenu;
		int i;
		for(i=0;i<2;i++)
		{
			planes.insert(pair<int,planeBase*>(i,new plane(i,1)));	
		}
		for(i=i;i<settings.ENEMY_PLANES;i++)
		{
			planes.insert(pair<int,planeBase*>(i,new AIplane(i,i)));	
		}
	}
};