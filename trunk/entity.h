
class entity
{
public:
	const int	id;
	int			type;
	int			team;
	bool		dead;
	Vec3f		pos;

	virtual bool Update(float value)=0;
	entity(int Id, int Type, int Team, Vec3f Pos=Vec3f(0,0,0)): id(Id), type(Type), team(Team), dead(false), pos(Pos){} 
};