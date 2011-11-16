 
class aaGun: public object
{
private:
	double lastUpdateTime;
	double extraShootTime;//time since it last shot;
	unsigned long shotsFired;
public:
//////////////functions////////////
	void findTargetVector();
	void die();
	void loseHealth(float healthLoss);
	void spawn();
	void initArmaments();
	aaGun(Vec3f sPos, Quat4f sRot, objectType Type);
	void updateSimulation(double time, double ms);
	void updateFrame(float interpolation) const;
//////////////structs//////////////
	struct wayPoint
	{
		double time;
		Quat4f rotation;
		wayPoint(double Time,Vec3f pos,Quat4f rot): time(Time), rotation(rot){}
		wayPoint(){}
	};
	struct armament
	{
		bool firing;
		unsigned int max, left;
		unsigned int roundsMax, roundsLeft;//number fired before recharge must take place
		float rechargeTime, rechargeLeft;//all in milliseconds
		float coolDown, coolDownLeft;
	};
///////////orientation/////////////
	//Angle angle;
	//Angle elevation;
	Vec3f targeter;
	int target;
/////////////weapons///////////////
	armament machineGun;
////////////life///////////////////
	//bool dead; (from entity)
	//int respawn;
	float health;
	float maxHealth;
////////////auto-pilot/////////////
	objectPath planePath;
///////////camera view/////////////
	Vec3f camera;
	Vec3f center;
};