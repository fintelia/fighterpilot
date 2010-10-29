
#include "main.h"

objectPath& operator<<( objectPath& op, const objectPath::point& p ) 
{
	op.addWaypoint(p);
	return op;
}