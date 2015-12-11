#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <iostream>
#include "formal.h"

using namespace std;

struct Collector{
	enum class Goal{FORWARD,OFF,REVERSE};
	Goal goal;
	typedef Goal Status_detail;
	Status_detail status; 
	typedef Goal Output;	
	Output output;
	
	Collector():goal(Goal::OFF),status(Status_detail::OFF),output(Output::OFF){}
};


ostream& operator<<(ostream& o, Collector::Goal a);
ostream& operator<<(ostream& o, Collector a);

#endif