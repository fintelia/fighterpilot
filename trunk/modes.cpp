#include "main.h"

void twoPlayerVs::drawHUD(int acplayer, Vec3f eye, Vec3f center, Vec3f up)
{
	viewOrtho(sw,sh/2);
	plane p=*(plane*)planes[players[acplayer].planeNum()];
	if(players[acplayer].firstPerson() && !p.controled)
	{
		texturedQuad("cockpit",0,0,sw,sh/2);
		targeter(400,150,50,0.0);
		drawRadar(176, 200, 64, 64, true);
		healthBar(140, 40, 200, 200, p.health/p.maxHealth,true);


		//speedMeter(16,16,80,80,p.accel.magnitude()*30.5+212);
		//altitudeMeter(368,215,432,279,p.altitude);

	}
	else
	{
		drawRadar(730, 230, 64, 64, false);
		healthBar(614, 25, 150, 25, p.health/p.maxHealth,false);
	}
	planeIdBoxes(p,eye);
}
void onePlayer::drawHUD(int acplayer, Vec3f eye, Vec3f center, Vec3f up)
{
	viewOrtho(sw,sh);
	plane p=*(plane*)planes[players[acplayer].planeNum()];
	if(players[acplayer].firstPerson() && !p.controled)
	{
		texturedQuad("cockpit square",0,0,sw,sh);
		targeter(400,300,50,0.0);
		drawRadar(176, 350, 64, 64, true);
		healthBar(140, 190, 200, 200, p.health/p.maxHealth,true);

		//speedMeter(280,533,344,597,p.accel.magnitude()*30.5+212);
		//altitudeMeter(456,533,520,597,p.altitude);
	}
	else
	{
		drawRadar(700, 500, 96, 96, false);
		healthBar(614, 25, 150, 25, p.health/p.maxHealth,false);
	}
	planeIdBoxes(p,eye);

}