
class gridFloatTerrain: public Terrain
{
public:
	bool load(string filename);
	gridFloatTerrain(string filename)
	{
		if(!load(filename))
		{
			w=0;
			l=0;
		}
		computedNormals=false;
	}
	gridFloatTerrain()
	{
		w=0;
		l=0;
		computedNormals=false;
	}
};
