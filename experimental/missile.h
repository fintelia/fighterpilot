

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

	unsigned int getSize()const{return sizeof(*this);}
};

class SAMmissile: public missileBase
{
protected:
	float minAngle;
public:
	int target;

	void init();
	SAMmissile(missileType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner, int Target);
	void updateSimulation(double time, double ms);
};