 
class antiAircraftArtilleryBase: public object
{
protected:
	double lastUpdateTime;
	double extraShootTime;//time since it last shot;
	unsigned long shotsFired;
public:
//////////////functions////////////
	void die();
	void loseHealth(float healthLoss);
	virtual void spawn();
	antiAircraftArtilleryBase(Vec3f sPos, Quat4f sRot, objectType Type, int Team);
	void updateFrame(float interpolation) const;
///////////orientation/////////////
	Vec3f targeter;
	int target;
////////////life///////////////////
	//bool dead; (from entity)
	float health;
	float maxHealth;
///////////camera view/////////////
	Vec3f camera;
	Vec3f center;
};

class AAgun: public antiAircraftArtilleryBase
{
public:
	struct armament
	{
		bool firing;
		unsigned int max, left;
		unsigned int roundsMax, roundsLeft;//number fired before recharge must take place
		float rechargeTime, rechargeLeft;//all in milliseconds
		float coolDown, coolDownLeft;
	}machineGun;
	void spawn();
	void initArmaments();
	void updateSimulation(double time, double ms);
	AAgun(Vec3f sPos, Quat4f sRot, objectType Type, int Team):antiAircraftArtilleryBase(sPos, sRot, Type, Team){initArmaments();}
};

class SAMbattery: public antiAircraftArtilleryBase
{
private:
	float missileCoolDown;
public:
	void updateSimulation(double time, double ms);
	SAMbattery(Vec3f sPos, Quat4f sRot, objectType Type, int Team):antiAircraftArtilleryBase(sPos, sRot, Type, Team), missileCoolDown(random<float>(7000.0)){}
};

class flakCannon: public antiAircraftArtilleryBase
{
private:
	float missileCoolDown;
public:
	void updateSimulation(double time, double ms);
	flakCannon(Vec3f sPos, Quat4f sRot, objectType Type, int Team):antiAircraftArtilleryBase(sPos, sRot, Type, Team), missileCoolDown(random<float>(2000.0)){}
};