
#include "main.h"

const Color white(1.0f,1.0f,1.0f);
const Color black(0.0f,0.0f,0.0f);
const Color darkBlue(0.11f,0.35f,0.52f);
const Color lightBlue(0.19f,0.58f,0.78f);
const Color darkGreen(0.49f,0.57f,0.06f);
const Color lightGreen(0.68f,0.76f,0.12f);


unsigned __int8 uPowerOfTwo(unsigned __int8 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    return i++;
}
unsigned __int16 uPowerOfTwo(unsigned __int16 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    return i++;
}
unsigned __int32 uPowerOfTwo(unsigned __int32 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    return i++;
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
    return i++;
}
__int8 uPowerOfTwo(__int8 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    return i++;
}
__int16 uPowerOfTwo(__int16 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    return i++;
}
__int32 uPowerOfTwo(__int32 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    return i++;
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
    return i++;
}