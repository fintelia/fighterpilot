#include "main.h"

void twoPlayerVs::draw2D()
{
	for(int acplayer=0; acplayer <= 1; acplayer++)
	{
		plane p=*(plane*)planes[players[acplayer].planeNum()];
		if(players[acplayer].firstPerson() && !p.controled)
		{
			graphics->drawOverlay(Vec2f(0,sh/2*acplayer),Vec2f(sw,sh/2+sh/2*acplayer),"cockpit");
			targeter(400,150+300*acplayer,50,p.turn);
			drawRadar(176, 200+300*acplayer, 64, 64, true);
			healthBar(140, 40+300*acplayer, 200, 200, p.health/p.maxHealth,true);

			//speedMeter(16,16,80,80,p.accel.magnitude()*30.5+212);
			//altitudeMeter(368,215,432,279,p.altitude);

		}
		else
		{
			drawRadar(730, 230+300*acplayer, 64, 64, false);	
			healthBar(614, 25+300*acplayer, 150, 25, p.health/p.maxHealth,false);
		}
		//planeIdBoxes(p,eye);
	}
}
void onePlayer::draw2D()
{
	plane p=*(plane*)planes[players[0].planeNum()];
	if(players[0].firstPerson() && !p.controled)
	{
		graphics->drawOverlay(Vec2f(0,0),Vec2f(sw,sh),"cockpit square");
		targeter(400,300,50,p.turn);
		drawRadar(176, 350, 64, 64, true);
		

		healthBar(140, 190, 200, 200, p.health/p.maxHealth,true);

		//speedMeter(280,533,344,597,p.accel.magnitude()*30.5+212);
		//altitudeMeter(456,533,520,597,p.altitude);
	}
	else
	{
		drawRadar(700, 500, 96, 96, false);

		glDisable(GL_MULTISAMPLE);
		healthBar(614, 25, 150, 25, p.health/p.maxHealth,false);
		glEnable(GL_MULTISAMPLE);

	}
	//planeIdBoxes(p,eye);

	if(levelup)
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Vec2f(sw/2-v*sw/2,sh-v*sh),Vec2f(sw,sh)*v,"next level");
	}
}