#pragma once 

template <class T,class U,class V>
T clamp(T value, U v1, V v2)
{
	if(v1<v2)
	{
		if(value<v1) return (T)v1;
		if(value>v2) return (T)v2;
		else return value;
	}
	else
	{
		if(value>v1) return (T)v1;
		if(value<v2) return (T)v2;
		else return value;
	}
}
