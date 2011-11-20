
struct scriptVar
{
	string value;

	template<class T>
	T operator() ()
	{
		try{
			return lexical_cast<T>(value);
		}catch(boost::bad_lexical_cast &){
			debugBreak();//boost type conversion failed
			return T();
		}
	}
	scriptVar(string v=""): value(v){}
};

map<string,scriptVar> loadScript(string filename);
