
class profiler
{
	map<string,vector<float> > elements;
	map<string,float> start;
public:
	void startElement(string n)
	{
		start[n]=GetTime();
	}
	void endElement(string n)
	{
		//glFinish();
		if(start.find(n)==start.end())
			start[n]=0;
		elements[n].push_back(GetTime()-start[n]);
		if(elements[n].size()>30)
			elements[n].erase(elements[n].begin());
	}
	void draw()
	{
		glColor3f(0.3,1,0.3);
		int y=25;
		float v=0;
		for(map<string,vector<float> >::iterator i=elements.begin();i!=elements.end();i++,y+=20)
		{
			textManager->renderText(i->first,10,y);
			v=0;
			for(vector<float>::iterator s=i->second.begin();s!=i->second.end();s++)
				v+=*s;
			textManager->renderText(lexical_cast<string>(v/i->second.size()),200,y);
		}
	}
};
extern profiler Profiler;