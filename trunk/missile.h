

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
	double launchTime;

	bool engineStarted;
	bool contrailStarted;
	bool smallContrailStarted;

	missile(missileType Type, teamNum Team,Vec3f sPos, Quat4f sRot, float speed, int Owner, int Target);
	void updateSimulation(double time, double ms);
	//void updateFrame(float interpolation) const;
};

class SAMmissile: public missileBase
{
public:
	int target;

	SAMmissile(missileType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner, int Target): missileBase(Type, Team, sPos, sRot, speed, Owner), target(Target){}
	
	void init();
	void updateSimulation(double time, double ms);
};