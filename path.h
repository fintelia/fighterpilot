
class objectPath
{


public:
	struct point
	{
		Vec3f	position;
		Quat4f	rotation;
		float	ms;
		point(Vec3f pos, Quat4f rot, float time): position(pos), rotation(rot), ms(time){}
		point(){}
	};
	class pathSegment
	{
	protected:
		short size;
		enum SegmentType{LINEAR} type;
	public:

		SegmentType getType(){return type;}

		virtual float startTime()=0;
		virtual float endTime()=0;
		virtual point getPoint(float ms)=0;
	};
	class linearPath: public pathSegment
	{
	private:
		point start, end;
	public:
		linearPath(point Start, point End): start(Start), end(End)
		{
			size = sizeof(*this);
			type = LINEAR;
		}
		point getPoint(float ms)
		{
			float t = 1.0- (end.ms-ms)/(end.ms-start.ms);
			if(t < 0.0)		return start;
			if(t > 1.0)		return end;

			point p;
			p.position = lerp(start.position, end.position, t);
			p.rotation = slerp(start.rotation, end.rotation, t);
			p.ms = ms;
			return p;
		}
		float startTime()
		{
			return start.ms;
		}
		float endTime()
		{
			return end.ms;
		}
	};
private:
	bool startValid;
	point startPoint;
	vector<pathSegment*> segments;

public:
	objectPath(): startValid(false)
	{
	
	}
	void addWaypoint(point p)
	{
		if(!startValid)
		{
			startPoint=p;
			startValid=true;
		}
		else if(segments.empty())
		{
			segments.push_back(new linearPath(startPoint,p));
		}
		else
		{
			segments.push_back(new linearPath(segments.back()->getPoint(segments.back()->endTime()),p));
		}
	}
	point operator() (float ms)
	{
		if(ms > segments.back()->endTime())
		{
			return segments.back()->getPoint(ms);
		}
		else if(ms < segments.front()->startTime() + 0.1)
		{
			return segments.front()->getPoint(ms);
		}

		for(vector<pathSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
		{
			if(ms > (*i)->startTime() && ms < (*i)->endTime())
				return (*i)->getPoint(ms);
		}
		
		__debugbreak();
		return point();
	}
	float startTime()
	{
		if(!startValid)
		{
			return 0;
		}
		else if(segments.empty())
		{
			return startPoint.ms;
		}
		else
		{
			return segments.front()->startTime();
		}
	}
	float endTime()
	{
		if(!startValid)
		{
			return 0;
		}
		else if(segments.empty())
		{
			return startPoint.ms;
		}
		else
		{
			return segments.back()->endTime();
		}
	}
};
objectPath& operator<<( objectPath& op, const objectPath::point& p );