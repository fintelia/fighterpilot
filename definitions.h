
///////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef int objectType;
typedef int teamNum;
typedef int inputCallbackType;
typedef int mouseButton;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
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
const int MENU_KEY_STROKE	= 0x104;

const int MOUSE_BUTTON		= 0x200;
const int LEFT_BUTTON		= 0x201;
const int MIDDLE_BUTTON		= 0x202;
const int RIGHT_BUTTON		= 0x203;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
extern objectType objectTypeFromString(string s);
extern string objectTypeString(objectType t);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Color3
{
	float r,g,b;
	Color3(float R, float G, float B): r(R), g(G), b(B){}
	Color3(): r(0), g(0), b(0){}
};
struct Color4
{
	float r,g,b,a;
	Color4(float R, float G, float B, float A=1.0): r(R), g(G), b(B), a(A){}
	Color4(Color3 c): r(c.r), g(c.g), b(c.b), a(1.0){}
	Color4(): r(0), g(0), b(0),a(0){}
};

typedef Color4 Color;

extern const Color3 white;
extern const Color3 black;
extern const Color3 red;
extern const Color3 green;
extern const Color3 blue;
extern const Color3 darkBlue;
extern const Color3 lightBlue;
extern const Color3 darkGreen;
extern const Color3 lightGreen;
extern const Color3 darkGray;
extern const Color3 lightGray;
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
void threadSleep(unsigned long milliseconds);

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

#ifndef WINDOWS
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
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
template <class T>
bool isPowerOfTwo(T i)
{
	return !((i != 1) && (i & (i - 1)));
}
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
template <typename T>
typename std::enable_if<std::is_unsigned<T>::value, T>::type abs(T t)
{
    return t;  
}

template <typename T>
typename std::enable_if<!std::is_unsigned<T>::value, T>::type abs(T t)
{
	return (t < 0) ? -t : t;
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
