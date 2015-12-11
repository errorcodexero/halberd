#include <iostream>
#include "collector.h"

using namespace std;

ostream& operator<<(ostream& o, Collector::Goal a){
	if(a==Collector::Goal::OFF) return o<<"Goal(off)";
	if(a==Collector::Goal::REVERSE) return o<<"Goal(reverse)";
	if(a==Collector::Goal::FORWARD) return o<<"Goal(forward)";
	assert(0);
}

ostream& operator<<(ostream& o, Collector a){
	return o<<"Collector("<<")";
}

ostream& operator<<(ostream& o, Collector::Status_detail){ return o<<" ";}
ostream& operator<<(ostream& o, Collector::Input){ return o<<" ";}

bool operator==(Collector::Input,Collector::Input){ return true;}
bool operator<(Collector::Status_detail, Collector::Status_detail){ return 1;}
bool operator==(Collector::Status_detail, Collector::Status_detail){ return 1;}

Collector::Output control(Collector::Status_detail, Collector::Goal goal){
	return goal;
}

Collector::Status status(Collector::Status_detail){return {};}

bool ready(Collector::Status, Collector::Goal){return 1;}

set<Collector::Goal> examples(Collector::Goal*){ 
	return {Collector::Goal::FORWARD,Collector::Goal::OFF,Collector::Goal::REVERSE};
}
set<Collector::Input> examples(Collector::Input*){ return{};}
set<Collector::Input_reader> examples(Collector::Input_reader){ return {};}
set<Collector::Status> examples(Collector::Status*){ return {};}

#ifdef COLLECTOR_TEST
#include "formal.h"

int main(){
	Collector a;
	tester(a);
	return 0;
}

#endif
