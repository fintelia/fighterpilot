
using namespace std;
//const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;

void spawn();
void die();


extern const int PLANE;
extern const int PLAYER_PLANE;
extern const int AI_PLANE;

class planeBase:public entity
{
private:
	double lastUpdateTime;
	double extraShootTime;//time since it last shot;
	controlState lastController;
protected:
	controlState controller;
public:
//////////////functions////////////
	//void updatePos(float ms);
	void updateAll();
	void findTargetVector();
	//virtual void Accelerate(float value);
	//virtual void Brake(float value);
	//virtual void Turn_Right(float value);
	//virtual void Turn_Left(float value);
	//virtual void Climb(float value);
	//virtual void Dive(float value);
	virtual void Level(float value);
	//virtual void Shoot();
	virtual void ShootMissile();
	virtual void die();
	virtual void loseHealth(float healthLoss);
	virtual void autoPilotUpdate(float value);
	virtual void exitAutoPilot();
	virtual void returnToBattle();
	virtual void returnToBattle2();
	virtual bool Update(float value)=0;
	virtual void spawn()=0;
	void drawExplosion(bool flash);
	void initArmaments();
	planeBase():explode(NULL), lastUpdateTime(gameTime()), extraShootTime(0.0){}
//////////////structs//////////////
	struct wayPoint
	{
		double time;
		Vec3f position;
		Vec3f fwd;
		Angle roll;
		wayPoint(double Time,Vec3f Position,Vec3f Fwd,Angle Roll): time(Time), position(Position), fwd(Fwd), roll(Roll){}
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
//////////////flight specs/////////
	float turn; // -50 to 50
	//Vec3f pos; (from entity)
	Vec3f velocity;
	Vec3f accel;
	Vec3f normal;
	Angle roll;
	float altitude;
/////////////weapons///////////////
	Vec3f targeter;
	armament machineGun;
	armament rockets;
////////////identity///////////////
	//int id; (from entity)
	int team;
	planeType type;
////////////life///////////////////
	//bool dead; (from entity)
	//int respawn;
	float health;
	float maxHealth;
	bool hitGround;
	explosion* explode;
	bool respawning;
	float respawnTime;
////////////auto-pilot/////////////
	int maneuver;
	bool controled;
	vector<wayPoint> wayPoints;
///////////camera view/////////////
	Vec3f camera;
	Vec3f center;
};

class plane: public planeBase
{
private:

public:
	void setControlState(controlState c);
	bool Update(float value);
	void spawn();
	void die();
	plane(int Player,int Team);
	plane();
};

class AIplane:public planeBase
{
public:
	int target; //plane being hunted
	vector<Vec3f> path;//a general path to get the plane to its target

	AIplane();
	AIplane(int _player,int _team);

	bool Update(float value);
	void spawn();
	void calcMove(int value);
	void freeForAll_calcMove(int value);
	void playerVs_calcMove(int value);
	void teams_calcMove(int value);
};