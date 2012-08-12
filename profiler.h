
class profiler
{
#ifdef _DEBUG
	map<string,vector<float> > elements;
	map<string,float> start;

	map<string,int> output;
	map<string,double> outputd;

	map<string,unsigned int> counter;
public:

	inline void startElement(string n)
	{
		start[n]=GetTime();
	}
	inline void endElement(string n)
	{
		if(start.find(n)==start.end())
			start[n]=0;
		elements[n].push_back(GetTime()-start[n]);
		if(elements[n].size()>3)
			elements[n].erase(elements[n].begin());
	}
	inline void setOutput(string n, int i)
	{
		output[n] = i;
	}
	inline void setOutput(string n, double d)
	{
		outputd[n] = d;
	}
	inline void increaseCounter(string n)
	{
		if(counter.find(n) == counter.end())
			counter[n]=0;
		counter[n]++;
	}
	void draw()
	{
		float y=0.015f;
		float v=0.0f;
		for(auto i=elements.begin();i!=elements.end();i++,y+=0.02f)
		{
			graphics->drawText(i->first,Vec2f(0.01f,y));
			v=0;
			for(auto s=i->second.begin();s!=i->second.end();s++)
				v+=*s;
			std::ostringstream buffer;
			buffer << std::fixed << std::setprecision(1) << max(v/i->second.size(),0.0f);
			graphics->drawText(buffer.str(),Vec2f(0.2f,y));
		}
		for(auto i=output.begin();i!=output.end();i++,y+=0.02f)
		{
			graphics->drawText(i->first, Vec2f(0.01f,y));
			graphics->drawText(lexical_cast<string>(i->second),Vec2f(0.2f,y));
		}
		for(auto i=outputd.begin();i!=outputd.end();i++,y+=0.02)
		{
			graphics->drawText(i->first,Vec2f(0.01f,y));
			std::ostringstream buffer;
			buffer << std::fixed << std::setprecision(2) << i->second;
			graphics->drawText(buffer.str(),Vec2f(0.2f,y));
		}
		for(auto i=counter.begin();i!=counter.end();i++,y+=0.02)
		{
			graphics->drawText(i->first,Vec2f(0.01f,y));
			graphics->drawText(lexical_cast<string>(i->second),Vec2f(0.2f,y));
		}
		output.clear();
	}
#else
public:
	inline void startElement(string n){}
	inline void endElement(string n){}
	inline void setOutput(string n, int i){}
	inline void setOutput(string n, double d){}
	inline void increaseCounter(string n){}
	inline void draw(){}
#endif

};
extern profiler Profiler;