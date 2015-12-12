#include "collector.h"

using namespace std;

ostream& operator<<(ostream& o, Collector::Goal a){
	if(a==Collector::Goal::OFF) return o<<"Collector::Goal(OFF)";
	if(a==Collector::Goal::REVERSE) return o<<"Collector::Goal(REVERSE)";
	if(a==Collector::Goal::FORWARD) return o<<"Collector::Goal(FORWARD)";
	assert(0);
}

ostream& operator<<(ostream& o, Collector a){
	return o<<"Collector()";
}

ostream& operator<<(ostream& o, Collector::Status_detail){ return o<<" ";}
ostream& operator<<(ostream& o, Collector::Input){ return o<<" ";}

bool operator==(Collector::Input,Collector::Input){ return 1;}
bool operator<(Collector::Input, Collector::Input){ return 1;}
bool operator<(Collector::Status_detail, Collector::Status_detail){ return 1;}
bool operator==(Collector::Status_detail, Collector::Status_detail){ return 1;}
bool operator<(Collector::Input_reader, Collector::Input_reader){ return 1;}
	
set<Collector::Input> examples(Collector::Input*){
	return set<Collector::Input>{Collector::Input{}};
}

set<Collector::Goal> examples(Collector::Goal*){ 
	return set<Collector::Goal>{Collector::Goal::FORWARD,Collector::Goal::OFF,Collector::Goal::REVERSE};
}

set<Collector::Status_detail> examples(Collector::Status_detail*){ 
	return set<Collector::Status_detail>{Collector::Status_detail{}};
}
set<Collector::Output> examples(Collector::Output*){ 
	return set<Collector::Output>{1,0,-1};
}


Collector::Output control(Collector::Status_detail, Collector::Goal goal){
	if(goal==Collector::Goal::FORWARD)return Collector::Output{1.0};
	if(goal==Collector::Goal::OFF)return Collector::Output{0.0};
	if(goal==Collector::Goal::REVERSE)return Collector::Output{-1.0};
	assert(0);
}

Collector::Status status(Collector::Status_detail){ return Collector::Status{};}

bool ready(Collector::Status, Collector::Goal){return 1;}


#ifdef COLLECTOR_TEST
#include "formal.h"

int main(){
	Collector a;
	tester(a);
	return 0;
}

#endif
