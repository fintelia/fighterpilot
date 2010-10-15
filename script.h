
extern void loadScript(string filename);

class scriptVar
{
private:
	string name;
	enum varType{
		FLOAT,
		INT,
		STRING,
		C_STRING
	}type;
	void* ptr;
public:
	scriptVar(string Name,varType t, void* Ptr): name(Name), type(t), ptr(Ptr) {}
	void set(string val)
	{
		if(type==FLOAT)		*(float*)ptr=lexical_cast<float>(val);
		if(type==INT)		*(int*)ptr=lexical_cast<int>(val);
		if(type==STRING)	*(string*)ptr=lexical_cast<string>(val);
		if(type==C_STRING)	*(char**)ptr=(char*)lexical_cast<string>(val).c_str();
	}
	string getName()
	{
		return name;
	}
};
