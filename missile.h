
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

	missile(missileType Type, teamNum Team,Vec3f sPos,Vec3f Vel, int dispList, int Owner):selfControlledObject(sPos, Quat4f(), Type), life(15.0), target(0), difAng(0), lastAng(0), velocity(Vel), accel(Vel.normalize()*MISSILE_SPEED/3.0), displayList(dispList),path(sPos,Quat4f()),owner(Owner){}

	void findTarget();
	void update(double time, double ms);
};