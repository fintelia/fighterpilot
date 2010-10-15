
class Terrain {
protected:
	int w; //Width
	int l; //Length
	vector<vector<float>> hs; //Heights
	vector<vector<Vec3f>> normals;
	bool computedNormals; //Whether normals is up-to-date
public:
	int textureId;
	Terrain();
	Terrain(int w2, int l2);
	~Terrain() {
		hs.clear();
		normals.clear();
	}
	int width();
	int length();
	void setHeight(int x, int z, float y);
	float getHeight(int x, int z);
	void computeNormals();
	Vec3f getNormal(int x, int z);
	float getInterpolatedHeight(float x,float z);
};
