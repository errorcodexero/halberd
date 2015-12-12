#include "arm.h"

using namespace std;

ostream& operator<<(ostream& o,Arm::Input) {return o<<" ";}
ostream& operator<<(ostream& o,Arm::Input_reader) {return o<<" ";}
ostream& operator<<(ostream& o,Arm::Output_applicator) {return o<<" ";}
ostream& operator<<(ostream& o,Arm::Status) {return o<<" ";} 
ostream& operator<<(ostream& o,Arm::Status_detail) {return o<<" ";}
ostream& operator<<(ostream& o,Arm::Estimator) {return o<<" ";}
ostream& operator<<(ostream& o,Arm::Goal) {return o<<" ";}
ostream& operator<<(ostream& o,Arm) {
	return o<<"Arm()";
}

bool operator==(Arm::Input a,Arm::Input b){
	return a.topLimit==b.topLimit;
}
bool operator<(Arm::Input a,Arm::Input b){
	return 1;
}

bool operator<(Arm::Status_detail, Arm::Status_detail){
	return true;
}

set<Arm::Input> examples(Arm::Input*){
	return set<Arm::Input>{Arm::Input{}};
}
set<Arm::Output> examples(Arm::Output*){
	return set<Arm::Output>{1,0,-1};//WILL NEED REAL VALUES
}
set<Arm::Goal> examples(Arm::Goal*){
	return set<Arm::Goal>{Arm::Goal::UP,Arm::Goal::DOWN};
}
set<Arm::Status> examples(Arm::Status*){
	return set<Arm::Status>{Arm::Status{}};
}
set<Arm::Status_detail> examples(Arm::Status_detail*){
	return set<Arm::Status_detail>{Arm::Status_detail{}};
}

Arm::Output control(Arm::Status_detail,Arm::Goal goal) {
	return 0;
}

Arm::Status status(Arm::Status_detail) {
	return {};
}

bool ready(Arm::Status,Arm::Goal) {
	return true;
}

#ifdef ARM_TEST
#include "formal.h"

int main(){
	Arm arm;
	tester(arm);
}

#endif
