
class Smoke
{
public:

	Vec3f *positions;
	Vec3f *velocity;
	float *startTime;
	float *endTime;
	SVertex *mem;

private:

	int size;
	int compacity;

	void resize(int newItems);
public:

	Smoke();
	~Smoke();
	void clean();
	void update(float ms);
	void insert(Vec3f pos,float random,float spread,float startT, float life);
	void clear();
	int getSize();
	SVertex* const getMem();
};

