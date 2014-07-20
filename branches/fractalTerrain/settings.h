
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
	void load(const map<string,map<string,string>>& m);
};
extern SettingsManager& settings;