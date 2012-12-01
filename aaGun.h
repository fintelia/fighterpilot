 
class antiAircraftArtilleryBase: public object
{
protected:
	double lastUpdateTime;
	double extraShootTime;//time since it last shot;
	unsigned long shotsFired;
public:
	Vec3f targeter;
	weak_ptr<plane> target;

	//bool dead; (from entity)
	float health;

	void die();
	void loseHealth(float healthLoss);
	antiAircraftArtilleryBase(Vec3f sPos, Quat4f sRot, objectType Type, int Team);
	virtual void updateFrame(float interpolation) const;
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
	void initArmaments();
	void updateSimulation(double time, double ms);
	AAgun(Vec3f sPos, Quat4f sRot, objectType Type, int Team);
};

class SAMbattery: public antiAircraftArtilleryBase
{
private:
	float missileCoolDown;
public:
	void updateSimulation(double time, double ms);
	SAMbattery(Vec3f sPos, Quat4f sRot, objectType Type, int Team):antiAircraftArtilleryBase(sPos, sRot, Type, Team), missileCoolDown(random<float>(7000.0)){}
	void updateFrame(float interpolation) const;
};

class flakCannon: public antiAircraftArtilleryBase
{
private:
	float missileCoolDown;
public:
	void updateSimulation(double time, double ms);
	flakCannon(Vec3f sPos, Quat4f sRot, objectType Type, int Team):antiAircraftArtilleryBase(sPos, sRot, Type, Team), missileCoolDown(random<float>(2000.0)){}
};