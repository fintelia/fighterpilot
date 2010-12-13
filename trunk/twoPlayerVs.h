
class twoPlayerVs: public dogFight
{
public:
	void draw3D()//MAIN DRAW FUNCTION
	{
		glClearColor(0.5f,0.8f,0.9f,1.0f);
		glViewport(0, sh/2, sw, sh/2);
		gluPerspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
		frustum.setCamInternals(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
		drawScene2(0);

		glViewport(0, 0, sw, sh/2);
		drawScene2(1);
	}
	void draw2D();

	int update(float ms) {
		if(input->getKey(F1))	{	players[0].toggleFirstPerson(); input->up(F1);}
		if(input->getKey(F2))	{	players[1].toggleFirstPerson(); input->up(F2);}
		if(input->getKey(0x31))	{	menuManager.setMenu("menuInGame"); input->up(0x31);}

		//for(int i=0;i<2;i++)
		//{
		//	planes[i]->Accelerate	(ms*controls[i][CON_ACCEL]);
		//	planes[i]->Brake		(ms*controls[i][CON_BRAKE]);
		//	planes[i]->Climb		(ms*controls[i][CON_CLIMB]);
		//	planes[i]->Dive			(ms*controls[i][CON_DIVE]);
		//	planes[i]->Turn_Right	(ms*controls[i][CON_RIGHT]);
		//	planes[i]->Turn_Left	(ms*controls[i][CON_LEFT]);
		//	if(controls[i][CON_MISSILE]>0.75)	planes[i]->ShootMissile();
		//	if(controls[i][CON_SHOOT]>0.75)		planes[i]->Shoot();
		//}

#ifdef _DEBUG
		if(input->getKey(0x4C)) planes[0]->loseHealth(2.5);
#endif

		((plane*)planes[0])->setControlState(players[0].getControlState());
		((plane*)planes[1])->setControlState(players[1].getControlState());

		updateWorld(ms);
		radarAng+=45.0*ms/1000;
		if(radarAng>=360)
			radarAng-=360;

		Redisplay=true;
		return 7;
	}
	twoPlayerVs()
	{
		prepareMap("media/mapfile.txt");
		//waterDisp();
		//treesDisp();
		//level = new Level(settings.MAP_FILE.c_str());


		int i;
		for(i=0;i<2;i++)
		{
			planes.insert(pair<int,planeBase*>(i,new plane(i,1)));	
			players[i].active(true);
			players[i].planeNum(i);
		}
		for(i=i;i<settings.ENEMY_PLANES;i++)
		{
			planes.insert(pair<int,planeBase*>(i,new AIplane(i,i)));	
		}
		//for(i=0;i<250;i++)
		//{
		//	turrets.push_back(new turret(i));
		//}
	}
	~twoPlayerVs()
	{
		terrain=NULL;
		planes.clear();
		bullets.clear();
		missiles.clear();
		newExaust.clear();
	}
};