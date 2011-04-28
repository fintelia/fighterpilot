
class objectPath
{
public:
	struct point
	{
		Vec3f	position;
		Quat4f	rotation;
		float	time;
		point(Vec3f pos, Quat4f rot, float t): position(pos), rotation(rot), time(t){}
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
		point getPoint(float time)
		{
			float t = 1.0- (end.time-time)/(end.time-start.time);
			if(t < 0.0)		return start;
			if(t > 1.0)		return end;

			point p;
			p.position = lerp(start.position, end.position, t);
			p.rotation = slerp(start.rotation, end.rotation, t);
			p.time = time;
			return p;
		}
		float startTime()
		{
			return start.time;
		}
		float endTime()
		{
			return end.time;
		}
	};

private:
	bool pointsValid;
	point startPoint;
	bool endValid;
	point endPoint;
	vector<pathSegment*> segments;
	float timeResolution;

public:
	objectPath(float tResolution=1000): pointsValid(false),timeResolution(tResolution){}
	objectPath(Vec3f sPos,Quat4f sRot, float tResolution=1000);
	objectPath(Vec3f sPos,Quat4f sRot, float time, float tResolution);

	void addWaypoint(point p);
	void addWaypoint(Vec3f sPos,Quat4f sRot, float time);
	void addWaypoint(Vec3f sPos,Quat4f sRot);
	point operator() (float time);
	float startTime();
	float endTime();
	void currentPoint(point p);
	void currentPoint(Vec3f sPos,Quat4f sRot);
	void currentPoint(Vec3f sPos,Quat4f sRot, float time);
	const vector<pathSegment*>& getPath() {return segments;}
};
objectPath& operator<<( objectPath& op, const objectPath::point& p );