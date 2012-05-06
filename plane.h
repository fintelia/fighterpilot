
using namespace std;
//const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;

void spawn();
void die();

class nPlane: public object
{
private:
	double lastUpdateTime;
	double extraShootTime;//time since it last shot;
	unsigned long shotsFired;

	shared_ptr<particle::smokeTrail> smokeTrail;
public:
//////////////structs//////////////
	struct wayPoint
	{
		double time;
		Vec3f position;
		Quat4f rotation;
		wayPoint(double Time,Vec3f pos,Quat4f rot): time(Time), position(pos), rotation(rot){}
		wayPoint(){}
	};
	struct armament
	{
		bool firing;
		unsigned int max, left;
		unsigned int roundsMax, roundsLeft;//number fired before recharge must take place
		float rechargeTime, rechargeLeft;//all in milliseconds
		float coolDown, coolDownLeft;
		struct ammo{
			Vec3f offset;
			objectType type;
			meshInstancePtr meshInstance;
		};
		vector<ammo> ammoRounds;
	};
	struct cameraState{
		float time;
		float angle;
		Vec3f position;
		Vec3f velocity;
		//Quat4f rot;
	};
	struct objectCamera //for interpolating between frames
	{
		camera currentFrame;
		camera lastFrame;
	};
//////////////flight specs/////////
	float speed;
	Angle roll;
	Angle climb;
	Angle direction;
	float altitude;
	bool lockRollRange;
/////////////weapons///////////////
	Vec3f targeter;
	armament machineGun;
	armament rockets;
	armament bombs;
////////////life///////////////////
	//bool dead; (from entity)
	float health;
	float maxHealth;
	enum deathType{DEATH_NONE=0,DEATH_HIT_GROUND,DEATH_HIT_WATER,DEATH_EXPLOSION,DEATH_TRAILING_SMOKE,DEATH_MISSILE,DEATH_BULLETS}death;
	bool respawning;
	float respawnTime;
////////////auto-pilot/////////////
	int maneuver;
	bool controled;
	vector<wayPoint> wayPoints;
	//objectPath planePath;

///////////camera view/////////////
	vector<cameraState> cameraStates;
	Quat4f cameraRotation;
	objectCamera observer;
	float cameraShake;

/////////////control///////////////
	struct ControlState
	{
		float climb;
		float dive;
		float accelerate;
		float brake;
		float right;
		float left;
		bool shoot1;
		bool shoot2;
		bool shoot3;
		ControlState(): climb(0), dive(0), accelerate(0), brake(0), right(0), left(0), shoot1(false), shoot2(false), shoot3(false) {}
	}controls;
	enum ControlType{CONTROL_TYPE_SIMPLE, CONTROL_TYPE_ADVANCED, CONTROL_TYPE_AI}controlType;
////////////methods////////////////
	void findTargetVector();
	void shootMissile();
	void dropBomb();
	void die();
	void loseHealth(float healthLoss);
	void autoPilotUpdate(float value);
	void exitAutoPilot();
	void returnToBattle();
	void spawn();
	void initArmaments();
	void init();

	void updateSimulation(double time, double ms);
	void updateFrame(float interpolation) const;

	nPlane(Vec3f sPos, Quat4f sRot, objectType Type, int Team);

private:
	void smoothCamera();
};

//class plane: public planeBase
//{
//private:
//
//public:
//	void setControlState(controlState c);
//	bool Update(float value);
//	void spawn();
//	void die();
//	plane(int Id, planeType Type, int Team);
//};
//
//class AIplane:public planeBase
//{
//public:
//	enum planeState{PARTOL,FOLLOW_TARGET,FINISH_TARGET} state;
//	int target; //plane being hunted
//	vector<Vec3f> path;//a general path to get the plane to its target
//
//	AIplane(int Id, planeType Type, int Team);
//
//	bool Update(float value);
//	void spawn();
//	void calcMove(int value);
//	void freeForAll_calcMove(int value);
//	void playerVs_calcMove(int value);
//	void teams_calcMove(int value);
//};