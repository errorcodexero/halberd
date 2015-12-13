#include "arm.h"

using namespace std;

#define TOP_LIMIT_DIO 6
#define BOTTOM_LIMIT_DIO 5
#define ARM_RELAY 0

ostream& operator<<(ostream& o,Arm::Input i){
	return o<<"Arm::Input("<<i.topLimit<<" "<<i.bottomLimit<<")";
}
ostream& operator<<(ostream& o,Arm::Output_applicator){
	return o<<"Arm::Output_applicator()";
}
ostream& operator<<(ostream& o,Arm::Output out){
	if(out==Arm::Output::UP)return o<<"Arm::Output(UP)";
	if(out==Arm::Output::OFF)return o<<"Arm::Output(OFF)";
	if(out==Arm::Output::DOWN)return o<<"Arm::Output(DOWN)";
	assert(0);
}
ostream& operator<<(ostream& o,Arm::Status s){
	if(s==Arm::Status::UP)return o<<"Arm::Status(UP)";
	if(s==Arm::Status::MID)return o<<"Arm::Status(MID)";
	if(s==Arm::Status::DOWN)return o<<"Arm::Status(DOWN)";
	assert(0);
}
ostream& operator<<(ostream& o,Arm::Goal g){
	if(g==Arm::Goal::UP)return o<<"Arm::Goal(UP)";
	if(g==Arm::Goal::DOWN)return o<<"Arm::Goal(DOWN)";
	assert(0);
}
ostream& operator<<(ostream& o,Arm::Estimator e){
	o<<"Arm::Estimator(";
	o<<"Last: "<<e.last;
	return o<<")";
}
ostream& operator<<(ostream& o,Arm a) {
	return o<<"Arm("<<a.estimator<<" "<<a.output_applicator<<")";
}

bool operator==(Arm::Input a,Arm::Input b){
	return (a.topLimit==b.topLimit) && (a.bottomLimit==b.bottomLimit);
}
bool operator<(Arm::Input a,Arm::Input b){
	return (a.topLimit<b.topLimit) || ((a.topLimit==b.topLimit) && (a.bottomLimit<b.bottomLimit));
}

Robot_inputs Arm::Input_reader::operator()(Robot_inputs a, Arm::Input i)const {
	a.digital_io.in[TOP_LIMIT_DIO] = i.topLimit ? Digital_in::_1 : Digital_in::_0;
	a.digital_io.in[BOTTOM_LIMIT_DIO] = i.bottomLimit ? Digital_in::_1 : Digital_in::_0;
	return a;
}

Arm::Input Arm::Input_reader::operator()(Robot_inputs r)const {
	return Arm::Input{
		r.digital_io.in[TOP_LIMIT_DIO]==Digital_in::_1,
		r.digital_io.in[BOTTOM_LIMIT_DIO]==Digital_in::_1
	};
}

Robot_outputs Arm::Output_applicator::operator()(Robot_outputs r, Output out)const{
	r.relay[ARM_RELAY] = [&]{
		switch (out) {
			case Output::OFF:
				return Relay_output::_00;
			case Output::UP:
				return Relay_output::_10;
			case Output::DOWN:
				return Relay_output::_01;
			default:
				assert(0);
		}
	}();
	return r;
}

Arm::Output Arm::Output_applicator::operator()(Robot_outputs r)const{
	if (r.relay[ARM_RELAY] == Relay_output::_10) return Output::UP;
	if (r.relay[ARM_RELAY] == Relay_output::_01) return Output::DOWN;
	return Output::OFF;
}

void Arm::Estimator::update(Time t, Arm::Input in, Arm::Output out) {
	if (in.topLimit) last = Status_detail::UP;
	if (in.bottomLimit) last = Status_detail::DOWN;
	if (in.topLimit && in.bottomLimit) last = Status_detail::MID;
} 

Arm::Status_detail Arm::Estimator::get()const{
	return last;
}

set<Arm::Input> examples(Arm::Input*){
	return set<Arm::Input>{Arm::Input{false, false},Arm::Input{true,false},Arm::Input{false,true},Arm::Input{true,true}};
}
set<Arm::Output> examples(Arm::Output*){
	return set<Arm::Output>{Arm::Output::UP,Arm::Output::OFF,Arm::Output::DOWN};
}
set<Arm::Goal> examples(Arm::Goal*){
	return set<Arm::Goal>{Arm::Goal::UP,Arm::Goal::DOWN};
}
set<Arm::Status> examples(Arm::Status*){
	return set<Arm::Status_detail>{Arm::Status::UP,Arm::Status::MID,Arm::Status::DOWN};
}

Arm::Output control(Arm::Status_detail status,Arm::Goal goal) {
	if ((status==Arm::Status::DOWN && goal==Arm::Goal::DOWN) || (status==Arm::Status::UP && goal==Arm::Goal::UP))return Arm::Output::OFF;
	return (goal==Arm::Goal::UP)?Arm::Output::UP:Arm::Output::DOWN;
}

Arm::Status status(Arm::Status_detail a) {
	return a;
}

bool ready(Arm::Status status,Arm::Goal goal) {
	return ((status==Arm::Status::DOWN && goal==Arm::Goal::DOWN) || (status==Arm::Status::UP && goal==Arm::Goal::UP));
}

#ifdef ARM_TEST
#include "formal.h"

int main(){
	Arm arm;
	tester(arm);
}

#endif
