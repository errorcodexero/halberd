#ifndef ARM_H
#define ARM_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/driver_station_interface.h"
#include "../util/util.h"

using namespace std;

struct Arm{
	enum class Goal{UP,DOWN};
	enum class Status{UP,MID,DOWN};

	struct Status_detail{
		
	};
	
	struct Input{
		bool topLimit, bottomLimit;
	};

	struct Input_reader{
		Input operator()(Robot_inputs)const{
			return Input{0,0};
		}
		Robot_inputs operator()(Robot_inputs a,Input)const{
			return a;
		}
	};

	typedef double Output;

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs r, Output out)const{return r;}
		Output operator()(Robot_outputs r)const{ return 0;}
	};

	struct Estimator{
		Status_detail get()const{
			return Status_detail{};
		}
		void update(Time t, Input in, Output out){};
	};
	Estimator estimator;
	Input_reader input_reader;
	//Goal goal;
	Output_applicator output_applicator;
};

ostream& operator<<(ostream&,Arm::Input);
ostream& operator<<(ostream&,Arm::Input_reader);
ostream& operator<<(ostream&,Arm::Output_applicator);
ostream& operator<<(ostream&,Arm::Status);
ostream& operator<<(ostream&,Arm::Status_detail);
ostream& operator<<(ostream&,Arm::Estimator);
ostream& operator<<(ostream&,Arm::Goal);
ostream& operator<<(ostream&,Arm);

bool operator==(Arm::Input,Arm::Input);

set<Arm::Input> examples(Arm::Input*);
set<Arm::Output> examples(Arm::Output*);
set<Arm::Goal> examples(Arm::Goal*);
set<Arm::Status> examples(Arm::Status*);
set<Arm::Status_detail> examples(Arm::Status_detail*);

Arm::Output control(Arm::Status_detail,Arm::Goal goal);
Arm::Status status(Arm::Status_detail);
bool ready(Arm::Status,Arm::Goal);

#endif
