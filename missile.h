
class missile: public selfControlledObject
{
public:
	float life;//life in seconds
	int target;
	Angle difAng;
	Angle lastAng;
	Vec3f velocity;
	Vec3f accel;
	int displayList;
	int owner;
	objectPath path;

	missile(missileType Type, teamNum Team,Vec3f sPos,Vec3f Vel, int dispList, int Owner, int Target);
	void init();
	//void findTarget();
	void update(double time, double ms);
};