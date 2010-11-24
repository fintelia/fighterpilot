
class explosion
{
private:

	float	coreSize,
			fireballSize,
			StartTime;

	class smokeParticle
	{
	public:
		Vec3f initialVelocity;
	};
	vector<smokeParticle> smokeParticles;
	vector<smokeParticle> fireParticles;
public:
	Vec3f pos;
	explosion(Vec3f Pos);
	void l3dBillboardGetUpRightVector(Vec3f &up,Vec3f &right) 
	{ //http://www.lighthouse3d.com/opengl/billboarding/
		
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		right.set(	modelview[0],modelview[4],modelview[8]);
		up.set(		modelview[1],modelview[5],modelview[9]);
	}
	void render(bool flash);
	void render2();
	void render3();
};