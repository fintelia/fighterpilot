
class missile: public entity
{
public:
	float life;//life in seconds
	int owner;
	int target;
	//Angle angle;
	//Angle climbAngle;
	//vector<spark> *sparks;
	//map<int,entity*> *planes;
	Angle difAng;
	Angle lastAng;
	//queue<Vec3f> enemyLoc;
	Vec3f velocity;
	Vec3f accel;
	int displayList;
	missile(Vec3f pos,Vec3f velocity,int Owner, int dispList);
	void findTarget();
	bool Update(float value);
};