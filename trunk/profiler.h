
class profiler
{
	map<string,vector<float> > elements;
	map<string,float> start;

	map<string,int> output;
	map<string,double> outputd;

	map<string,unsigned int> counter;
public:
	void startElement(string n)
	{
		start[n]=GetTime();
	}
	void endElement(string n)
	{
		glFinish();
		if(start.find(n)==start.end())
			start[n]=0;
		elements[n].push_back(GetTime()-start[n]);
		if(elements[n].size()>3)
			elements[n].erase(elements[n].begin());
	}
	void setOutput(string n, int i)
	{
		output[n] = i;
	}
	void setOutput(string n, double d)
	{
		outputd[n] = d;
	}
	void increaseCounter(string n)
	{
		if(counter.find(n) == counter.end())
			counter[n]=0;
		counter[n]++;
	}
	void draw()
	{
		glColor3f(1,0,0);
		int y=25;
		float v=0;
		for(auto i=elements.begin();i!=elements.end();i++,y+=20)
		{
			graphics->drawText(i->first,Vec2f(10,y));
			v=0;
			for(auto s=i->second.begin();s!=i->second.end();s++)
				v+=*s;
			ostringstream buffer;
			buffer << std::fixed << std::setprecision(1) << max(v/i->second.size(),0.0f);
			graphics->drawText(buffer.str(),Vec2f(200,y));
		}
		for(auto i=output.begin();i!=output.end();i++,y+=20)
		{
			graphics->drawText(i->first, Vec2f(10,y));
			graphics->drawText(lexical_cast<string>(i->second),Vec2f(200,y));
		}
		for(auto i=outputd.begin();i!=outputd.end();i++,y+=20)
		{
			graphics->drawText(i->first,Vec2f(10,y));
			ostringstream buffer;
			buffer << std::fixed << std::setprecision(2) << i->second;
			graphics->drawText(buffer.str(),Vec2f(200,y));
		}
		for(auto i=counter.begin();i!=counter.end();i++,y+=20)
		{
			graphics->drawText(i->first,Vec2f(10,y));
			graphics->drawText(lexical_cast<string>(i->second),Vec2f(200,y));
		}
		output.clear();
		glColor3f(1,1,1);
	}
};
extern profiler Profiler;