
#include "engine.h"
#if defined(WINDOWS)
#include <Windows.h>
#elif defined(LINUX)
#include <pthread.h>
#endif

//const double UPDATE_LENGTH	= 8.333;

const Color3 white(1.0f,1.0f,1.0f);
const Color3 black(0.0f,0.0f,0.0f);
const Color3 red(1.0f,0.0f,0.0f);
const Color3 green(0.0f,1.0f,0.0f);
const Color3 blue(0.0f,0.0f,1.0f);
const Color3 darkBlue(0.11f,0.35f,0.52f);
const Color3 lightBlue(0.19f,0.58f,0.78f);
const Color3 darkGreen(0.49f,0.57f,0.06f);
const Color3 lightGreen(0.68f,0.76f,0.12f);
const Color3 darkGray(0.3f,0.3f,0.3f);
const Color3 lightGray(0.6f,0.6f,0.6f);

unsigned __int8 uPowerOfTwo(unsigned __int8 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    return ++i;
}
unsigned __int16 uPowerOfTwo(unsigned __int16 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    return ++i;
}
unsigned __int32 uPowerOfTwo(unsigned __int32 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    return ++i;
}
unsigned __int64 uPowerOfTwo(unsigned __int64 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
	i |= i >> 32;
    return ++i;
}
__int8 uPowerOfTwo(__int8 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    return ++i;
}
__int16 uPowerOfTwo(__int16 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    return ++i;
}
__int32 uPowerOfTwo(__int32 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    return ++i;
}
__int64 uPowerOfTwo(__int64 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
	i |= i >> 32;
    return ++i;
}
//objectType objectTypeFromString(string s)
//{
//	if(s=="f12")			return F12;
//	if(s=="f16")			return F16;
//	if(s=="f18")			return F18;
//	if(s=="f22")			return F22;
//	if(s=="UAV")			return UAV;
//	if(s=="B2")				return B2;
//	if(s=="missile1")		return MISSILE1;
//	if(s=="missile2")		return MISSILE2;
//	if(s=="missile3")		return MISSILE3;
//	if(s=="missile4")		return MISSILE4;
//	if(s=="missile1")		return BOMB1;
//	if(s=="missile2")		return BOMB2;
//	if(s=="AA gun")			return AA_GUN;
//	if(s=="bullet cloud")	return BULLET_CLOUD;
//	debugBreak();
//	return 0;
//}
//string objectTypeString(objectType t)
//{
//	if(t==F12)			return "f12";
//	if(t==F16)			return "f16";
//	if(t==F18)			return "f18";
//	if(t==F22)			return "f22";
//	if(t==UAV)			return "UAV";
//	if(t==B2)			return "B2";
//	if(t==MISSILE1)		return "missile1";
//	if(t==MISSILE2)		return "missile2";
//	if(t==MISSILE3)		return "missile3";
//	if(t==MISSILE4)		return "missile4";
//	if(t==BOMB1)		return "missile1";
//	if(t==BOMB2)		return "missile2";
//	if(t==AA_GUN)		return "AA gun";
//	if(t==BULLET_CLOUD)	return "bullet cloud";
//	debugBreak();
//	return "";
//}
#if defined(WINDOWS)
void threadSleep(unsigned long milliseconds)
{
	Sleep(milliseconds);
}
mutex::mutex()
{
	handle = CreateMutex(nullptr, false, nullptr);
}
mutex::~mutex()
{
	CloseHandle(handle);
}
bool mutex::lock(unsigned long timeout)
{
	return WaitForSingleObject(handle, timeout)==WAIT_OBJECT_0;
}
void mutex::unlock()
{
	ReleaseMutex(handle);
}
#elif defined(LINUX)
void threadSleep(unsigned long milliseconds)
{
	timespec t;
	t.tv_sec = milliseconds / 1000;
	t.tv_nsec = (milliseconds % 1000) * 1000;
	nanosleep(&t, &t);
}
mutex::mutex(): handle(nullptr)
{
	handle = new pthread_mutex_t;
	pthread_mutex_init(static_cast<pthread_mutex_t*>(handle), nullptr);
}
mutex::~mutex()
{
	pthread_mutex_destroy(static_cast<pthread_mutex_t*>(handle));
	delete static_cast<pthread_mutex_t*>(handle);
}
bool mutex::lock(unsigned long timeout)
{
	pthread_mutex_lock(static_cast<pthread_mutex_t*>(handle));
	return true;
}
void mutex::unlock()
{
	pthread_mutex_unlock(static_cast<pthread_mutex_t*>(handle));
}
#endif
