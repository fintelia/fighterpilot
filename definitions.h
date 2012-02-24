
///////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef int objectType;
typedef int teamNum;
typedef int inputCallbackType;
typedef int mouseButton;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const double MAX_FPS;
extern const double UPDATE_LENGTH;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
const int MAJOR_OBJECT_TYPE = 0xff00;
const int MINOR_OBJECT_TYPE = 0x00ff;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
const int NEUTRAL			= 0x00;
const int TEAM1				= 0x01;
const int TEAM2				= 0x02;
const int TEAM3				= 0x03;
const int TEAM4				= 0x04;
const int TEAM5				= 0x05;
const int TEAM6				= 0x06;
const int TEAM7				= 0x07;

const int INPUT_CALLBACK	= 0x100;
const int KEY_STROKE		= 0x101;
const int MOUSE_CLICK		= 0x102;
const int MOUSE_SCROLL		= 0x103;

const int MOUSE_BUTTON		= 0x200;
const int LEFT_BUTTON		= 0x201;
const int MIDDLE_BUTTON		= 0x202;
const int RIGHT_BUTTON		= 0x203;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
extern objectType objectTypeFromString(string s);
extern string objectTypeString(objectType t);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Color
{
	float r,g,b,a;
	Color(float R, float G, float B, float A=1.0): r(R), g(G), b(B), a(A){}
	Color(): r(0), g(0), b(0),a(0){}
};

extern const Color white;
extern const Color black;
extern const Color red;
extern const Color green;
extern const Color blue;
extern const Color darkBlue;
extern const Color lightBlue;
extern const Color darkGreen;
extern const Color lightGreen;
extern const Color darkGray;
extern const Color lightGray;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
T uPowerOfTwo(T i)
{
    static_assert(std::is_integral<T>::value,"value must be of integral type");
    --i;
    unsigned char n=1;
    while(sizeof(T)*8 > n)
    {
        i |= i >> n;
        n = n<<1;
    }
    return ++i;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WINDOWS
class mutex
{
private:
	void* handle;
public:
	mutex();
	~mutex();
	bool lock(unsigned long timeout=0xffffffff);
	void unlock();
};
void sleep(unsigned long milliseconds);
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////
extern unsigned __int8 uPowerOfTwo(unsigned __int8 i);
extern unsigned __int16 uPowerOfTwo(unsigned __int16 i);
extern unsigned __int32 uPowerOfTwo(unsigned __int32 i);
extern unsigned __int64 uPowerOfTwo(unsigned __int64 i);

extern __int8 uPowerOfTwo(__int8 i);
extern __int16 uPowerOfTwo(__int16 i);
extern __int32 uPowerOfTwo(__int32 i);
extern __int64 uPowerOfTwo(__int64 i);
///////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class U> inline
T (max)(const T& a,const U& b)
{
	return (a > b) ? a : b;
}

template<class T,class U> inline
T (min)(const T& a, const U& b)
{
	return (a < b) ? a : b;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
inline void setBit(T& data, const T& bit, bool value)
{
	if(value)
		data |= bit;
	else
		data &= ~bit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class A, class B>
class functor
{
public:
	virtual A operator() (B b)=0;
};

template<class A>
class functor<A,void>
{
public:
	virtual A operator() ()=0;
};

template<class B>
class functor<void,B>
{
public:
	virtual void operator() (B b)=0;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, unsigned int totalSize>
class slidingList
{
private:
	T list[totalSize];
	unsigned int position;
	unsigned int count;
public:
	slidingList():position(0), count(0)
	{
		for(unsigned int i=0; i<totalSize; i++)
		{
			list[i] = T();
		}
	}
	void insert(T t)
	{
		if(count < totalSize)
		{
			list[count++] = t;
			position = count % totalSize;
		}
		else
		{
			list[position] = t;
			position = (position+1) % totalSize;
		}
	}
	T sum()
	{
		T s = T();
		for(unsigned int i=0; i<count; i++)
		{
			s += list[i];
		}
		return s;
	}
	T average()
	{
		T s = T();
		for(unsigned int i=0; i<count; i++)
		{
			s += list[i];
		}
		return s / count;
	}
	T operator[] (unsigned int i)
	{
		return list[i % count];
	}
	unsigned int size()
	{
		return count;
	}
	void reset()
	{
		count = 0;
	}
};