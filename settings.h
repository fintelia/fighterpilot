
class SettingsManager
{
	SettingsManager(){}
public:
	static SettingsManager& getInstance()
	{
		static SettingsManager* pInstance = new SettingsManager();
		return *pInstance;
	}
	map<string,map<string,string>> categories;

	struct planeStat
	{
		struct hardpoint
		{
			objectType mType;
			Vec3f offset;
			hardpoint(): mType(MISSILE), offset(0,0,0) {}
			hardpoint(objectType t,Vec3f Offset): mType(t), offset(Offset) {}
		};
		vector<hardpoint> hardpoints;//offsets
		vector<Vec3f> machineGuns;
		planeStat(){}
	};
	map<planeType,planeStat> planeStats;
	template <class T> T get (string category, string name)
	{
		try{
			if(categories.find(category)!=categories.end() && categories[category].find(name)!=categories[category].end())
				return lexical_cast<T>(categories[category][name]);
			else
				return T();
		}catch(...){
			return T();
		}
	}
	string operator() (string category, string name)
	{
		if(categories.find(category)!=categories.end() && categories[category].find(name)!=categories[category].end())
			return categories[category][name];
		else
			return "";
	}
	void load(string filename);
	void load(const map<string,map<string,string>>& m);
};
extern SettingsManager& settings;