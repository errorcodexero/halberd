#include "collector.h"

using namespace std;

ostream& operator<<(ostream& o, Collector::Goal a){
	if(a==Collector::Goal::OFF) return o<<"Collector::Goal(OFF)";
	if(a==Collector::Goal::REVERSE) return o<<"Collector::Goal(REVERSE)";
	if(a==Collector::Goal::FORWARD) return o<<"Collector::Goal(FORWARD)";
	assert(0);
}

ostream& operator<<(ostream& o, Collector::Status){ return o<<"Collector::Status()";}
ostream& operator<<(ostream& o, Collector::Input){ return o<<"Collector::Input()";}
ostream& operator<<(ostream& o, Collector){ return o<<"Collector()";}


bool operator==(Collector::Input,Collector::Input){ return 1;}
bool operator<(Collector::Input, Collector::Input){ return 1;}
bool operator<(Collector::Status_detail, Collector::Status_detail){ return 1;}
bool operator==(Collector::Status_detail, Collector::Status_detail){ return 1;}
bool operator<(Collector::Input_reader, Collector::Input_reader){ return 1;}

Collector::Input Collector::Input_reader::operator()(Robot_inputs)const{ return Collector::Input{};}

Robot_inputs Collector::Input_reader::operator()(Robot_inputs a, Collector::Input)const{ return a;}

Robot_outputs Collector::Output_applicator::operator()(Robot_outputs r, Collector::Output out)const{
	r.pwm[COLLECTOR_ADDRESS]=out;
	return r;
}

Collector::Status_detail Collector::Estimator::get()const{ return Collector::Status_detail{};}

Collector::Output Collector::Output_applicator::operator()(Robot_outputs r)const{
	return r.pwm[COLLECTOR_ADDRESS];
}
	
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
	return set<Collector::Output>{FORWARD,OFF,REVERSE};
}


Collector::Output control(Collector::Status_detail, Collector::Goal goal){
	if(goal==Collector::Goal::FORWARD)return Collector::Output{FORWARD};
	if(goal==Collector::Goal::OFF)return Collector::Output{OFF};
	if(goal==Collector::Goal::REVERSE)return Collector::Output{REVERSE};
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
