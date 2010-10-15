
class entity
{
public:
	int id;
	Vec3f pos;
	bool dead;
	virtual bool Update(float value)=0;
};