
class entity
{
public:
	int id;
	float x,y,z;
	bool dead;
	virtual bool Update(float value)=0;
};