
const int TOTAL_LEVELS=2;

class onePlayer: public dogFight
{
public:
	int level;
	int enemies_left;
	void draw()//MAIN DRAW FUNCTION
	{
		glClearColor(0.5f,0.8f,0.9f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, sw, sh);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(80.0, (double)sw / ((double)sh),1.0, 10000.0);
		frustum.setCamInternals(80.0, (double)sw / ((double)sh),1.0, 10000.0);
		acplayer=0;
		drawScene2();
	}

	int update(int value) {
		for(int i=0;i<1;i++)
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
		if(input->keys[F1]){firstP[0] = !firstP[0];input->keys[F1]=false;}
		updateWorld(value);

		enemies_left=0;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			if((*i).second->team != planes[0]->team)
				enemies_left++;
		}

		if(settings.ON_HIT==RESTART && planes[0]->dead)
			newLevel(level);
		if(100-enemies_left*100/settings.ENEMY_PLANES>=settings.KILL_PERCENT_NEEDED && level<TOTAL_LEVELS)
			newLevel(level+1);

		radarAng+=45.0*value/1000;
		if(radarAng>=360)
			radarAng-=360;

		Redisplay=true;
		return 33;
	}
	void newLevel(int nextlevel)
	{
		terrain=NULL;
		planes.clear();
		bullets.clear();
		missiles.clear();
		fire.clear();
		smoke.clear();

		level=nextlevel;
		string filepath("levels/Level ");
		stringstream out;
		out << nextlevel;
		filepath +=	out.str()+"/mapfile.txt";
		prepareMap((char*)(filepath.c_str()));

		if(settings.GAME_TYPE==FFA || settings.GAME_TYPE==TEAMS)
		{
			planes.insert(pair<int,planeBase*>(0,new plane(0,0)));
			for(int i=1;i<settings.ENEMY_PLANES;i++)
			{
				planes.insert(pair<int,planeBase*>(i,new AIplane(i,i)));
			}
		}
		else if(settings.GAME_TYPE==PLAYER_VS)
		{
			planes.insert(pair<int,planeBase*>(0,new plane(0,0)));
			for(int i=1;i<settings.ENEMY_PLANES+1;i++)
			{
				planes.insert(pair<int,planeBase*>(i,new AIplane(i,1)));
			}
		}
	}
	onePlayer()
	{
		//prepareMap("media2/mapfile.txt");
		newMode=0;
		Cmenu=new closedMenu;
		newLevel(1);
	}
};