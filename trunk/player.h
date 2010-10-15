

//class viewport
//{
//	Vec3f pos, fwd, up;
//	float x, y, width, height;
//	class dial
//	{
//		float x,y,width, height;
//		bool firstP;
//		enum dialType{RADAR, HEALTH, ROCKTES, BULLETS}type;
//	};
//};

class player
{
private:
	int				PlaneNum,
					PlayerNum,
					Kills;
	bool			Active,
					FirstPerson;
	playerControls	controls;

	static int		TotalPlayers;
public:
	player(): PlayerNum(TotalPlayers), controls(TotalPlayers){TotalPlayers++;}

	int planeNum()				{return PlaneNum;}
	void planeNum(int value)	{PlaneNum=value;}

	int playerNum()				{return PlayerNum;}

	int kills()					{return Kills;}
	void addKill()				{Kills++;}

	bool firstPerson()			{return FirstPerson;}
	void toggleFirstPerson()	{FirstPerson=!FirstPerson;}
	void firstPerson(bool value){FirstPerson=value;}

	void active(bool value)		{Active=value;}
	bool active()				{return Active;}

	controlState getControlState(){
		controlState c;
		c.accelerate=controls[CON_ACCEL];
		c.brake=controls[CON_BRAKE];
		c.climb=controls[CON_CLIMB];
		c.dive=controls[CON_DIVE];
		c.left=controls[CON_LEFT];
		c.right=controls[CON_RIGHT];
		c.shoot1=controls[CON_SHOOT];
		c.shoot2=controls[CON_MISSILE];
		return c;
	}

};
