
class bomb: public selfControlledObject
{
public:
	double launchTime;
	Vec3f velocity;//does not include acceleration due to gravity
	int owner;

	bomb(bombType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner);
	void update(double time, double ms);
};