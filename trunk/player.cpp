
#include <fstream>
#include <vector>
#include <windows.h>

using namespace std;

//#include <boost\python.hpp>
//using namespace boost;

#include "enums.h"
#include "input.h"
#include "player.h"

int player::TotalPlayers=1;

//BOOST_PYTHON_MODULE(controllerStructs)
//{
//    python::class_<controlState>("controlState") 
//        .def_readwrite("accelerate", &controlState::accelerate)
//        .def_readwrite("brake",	&controlState::brake)
//		.def_readwrite("climb", &controlState::climb)
//		.def_readwrite("dive", &controlState::dive)
//		.def_readwrite("left", &controlState::left)
//		.def_readwrite("right", &controlState::right)
//		.def_readwrite("shoot1", &controlState::shoot1)
//		.def_readwrite("shoot2", &controlState::shoot2)
//    ;
//}