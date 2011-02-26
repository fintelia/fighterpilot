
class bullet
{
public:
	double life;
	double startTime;
	Vec3f startPos;
	Vec3f velocity;//in m/s
	int owner;
	bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime);
	bullet(Vec3f pos,Vec3f vel,int Owner);
};
