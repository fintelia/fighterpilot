
const int TOTAL_LEVELS=3;

class onePlayer: public dogFight
{
public:
	int levelNum;
	int enemies_left;
	float countdown;
	bool levelup;
//	void drawOrthoView(int acplayer)
//	{
//
//		viewOrtho(sw,sh); //set view to ortho - start 2d
//
//		glDisable(GL_DEPTH_TEST);
//		glEnable(GL_BLEND);
//		glDisable(GL_LIGHTING);
//		glColor4f(1,1,1,1);
//		
//		if(players[acplayer].firstPerson())
//		{
//			//targeter start
//			Vec3f fwd=(frustum.ntl+frustum.ntr+frustum.nbl+frustum.nbr)/4.0-planes[players[acplayer].planeNum()]->pos;
//			float length=1.0/fwd.dot(planes[players[acplayer].planeNum()]->targeter);
//			Vec3f tar=planes[players[acplayer].planeNum()]->targeter*length;
//			tar+=planes[players[acplayer].planeNum()]->pos;
//			float tarX=dist_Point_to_Segment(tar,frustum.ntl,frustum.nbl)/frustum.ntl.distance(frustum.ntr);
//			float tarY=dist_Point_to_Segment(tar,frustum.ntl,frustum.ntr)/frustum.ntl.distance(frustum.nbl);
//			//targeter end
//			float s=(float)sw/800.0;
//			texturedQuad("cockpit",0,0,sw,sh);//cockpit
//			texturedQuad("aimer",tarX*(float)sw-16,tarY*(float)sh-16,32,32);
//			drawgauge(16*s,504*s,80*s,568*s,"tilt back","tilt front",-planes[players[acplayer].planeNum()]->turn,0);
//			drawgauge(16*s,32*s,80*s,96*s,"speed","needle",0,planes[players[acplayer].planeNum()]->velocity.magnitude()*45-20);
//			drawgauge(368*s,504*s,432*s,568*s,"altitude","needle",0,planes[players[acplayer].planeNum()]->altitude);//(p.y-_terrain->getHeight(int(p.x/size), int(p.z/size)) * 10));
//			glColor3f(1,1,1);
//			//if(p.dead)
//			//	render_tex_quad(tex[2],0,800*s,0,(float)sh);
//		}
//		drawRadar(acplayer,1.0,1.0);
//		glDisable(GL_BLEND);
//		glBindTexture(GL_TEXTURE_2D, 0);
//
//#ifdef _DEBUG
//		glColor4f(0,0,0,1.0);
//		if(acplayer==0)
//			textManager->renderText(lexical_cast<string>(fps),sw/2-25,25);
//#endif
//
//		glEnable(GL_DEPTH_TEST);
//		glEnable(GL_LIGHTING);
//		viewPerspective();//end 2d
//	}
	void drawHUD(int acplayer, Vec3f eye, Vec3f center, Vec3f up);
	void draw()//MAIN DRAW FUNCTION
	{
		glClearColor(0.5f,0.8f,0.9f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, sw, sh);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(80.0, (double)sw / ((double)sh),1.0, 25000.0);
		frustum.setCamInternals(80.0, (double)sw / ((double)sh),1.0, 25000.0);
		drawScene2(0);
		
		
		if(levelup)
		{
			viewOrtho(sw,sh/2-2); //set view to ortho - start 2d
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glDisable(GL_LIGHTING);
			glColor4f(1,1,1,1);
			
			glPushMatrix();
				glTranslatef(sw/2,sh/2,0);
				dataManager.bind("next level");
				float v=(1000-countdown)/10;
				glBegin(GL_QUADS);
					glVertex2f(v,v*0.75);
					glVertex2f(v,-v*0.75);
					glVertex2f(-v,-v*0.75);
					glVertex2f(-v,v*0.75);
				glEnd();
			glPopMatrix();

			glDisable(GL_BLEND);
			glBindTexture(GL_TEXTURE_2D, 0);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			viewPerspective();//end 2d
			char* i=(char*)gluErrorString(glGetError());
			i=i;
		}
	}

	int update(float value) {
		if(levelup)
		{
			countdown-=value;
			if(countdown<0)
			{
				newLevel(levelNum+1);
			}
		}
		if(input->getKey(F1)){players[0].toggleFirstPerson();input->getKey(F1);input->up(F1);}
		//for(int i=0;i<1;i++)
		//{
		//	planes[i]->Accelerate	(value*controls[i][CON_ACCEL]);
		//	planes[i]->Brake		(value*controls[i][CON_BRAKE]);
		//	planes[i]->Climb		(value*controls[i][CON_CLIMB]);
		//	planes[i]->Dive			(value*controls[i][CON_DIVE]);
		//	planes[i]->Turn_Right	(value*controls[i][CON_RIGHT]);
		//	planes[i]->Turn_Left	(value*controls[i][CON_LEFT]);
		//	if(controls[i][CON_MISSILE]>0.75)	planes[i]->ShootMissile();
		//	if(controls[i][CON_SHOOT]>0.75)		planes[i]->Shoot();
		//}
		((plane*)planes[0])->setControlState(players[0].getControlState());
		updateWorld(value);

		enemies_left=0;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			if((*i).second->team != planes[0]->team)
				enemies_left++;
		}

		if(settings.ON_HIT==RESTART && planes[0]->dead)
		{
			newLevel(levelNum);
		}
		if(100-enemies_left*100/settings.ENEMY_PLANES>=settings.KILL_PERCENT_NEEDED && levelNum<TOTAL_LEVELS && !levelup)
		{		
			levelup=true;//newLevel(level+1);
			countdown=1500;
		}

		radarAng+=45.0*value/1000;
		if(radarAng>=360)
			radarAng-=360;

		Redisplay=true;
		return 33;
	}
	void newLevel(int nextlevel)
	{
		levelup=false;
		terrain=NULL;
		planes.clear();
		bullets.clear();
		missiles.clear();
		//fire.clear();
		//smoke.clear();
		newExaust.clear();

		levelNum=nextlevel;
		string filepath=string("levels/Level ") + lexical_cast<string>(levelNum) + "/mapfile.txt";


		prepareMap((char*)(filepath.c_str()));
		//settings.loadMap((char*)(filepath.c_str()));
		//terrain = loadTerrain(settings.MAP_FILE.c_str(), settings.HEIGHT_RANGE);
		//level = new Level(settings.MAP_FILE.c_str());


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
		//prepareMap("media/mapfile.txt");
		levelup=false;
		players[1].planeNum(0);
		players[1].active(false);
		newMode=(modeType)0;
		Cmenu=new closedMenu;
		newLevel(1);
	}
};