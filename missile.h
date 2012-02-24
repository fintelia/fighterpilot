

class missileBase: public object
{
public:
	float life;//life in seconds
	Angle difAng;
	Angle lastAng;

	float speed;
	float acceleration;

	int owner;

	missileBase(missileType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner);

	virtual void init(){}
	virtual void updateSimulation(double time, double ms)=0;
	void updateFrame(float interpolation) const;
};

class missile: public missileBase
{
public:
	int target;
	//Angle difAng;
	//Angle lastAng;

	//float speed;
	//float acceleration;

	//int owner;

	missile(missileType Type, teamNum Team,Vec3f sPos, Quat4f sRot, float speed, int Owner, int Target):missileBase(Type, Team, sPos, sRot, speed, Owner), target(Target){}

	void init();
	void updateSimulation(double time, double ms);
	//void updateFrame(float interpolation) const;
};

class flakMissile: public missileBase
{
public:
	Vec3f target;

	flakMissile(missileType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner, Vec3f Target): missileBase(Type, Team, sPos, sRot, speed, Owner), target(Target){}
	void updateSimulation(double time, double ms);
};