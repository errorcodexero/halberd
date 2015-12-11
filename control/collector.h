#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <iostream>
#include "formal.h"
#include <set>
#include "../util/interface.h"
#include "../util/driver_station_interface.h"
#include "../util/util.h"

using namespace std;

struct Collector{
	enum class Goal{FORWARD,OFF,REVERSE};
	Goal goal; 
	
	struct Status_detail{};
	Status_detail status_detail;

	typedef Status_detail Status;
	Status status;

	struct Input{};
	Input input;
	
	struct Input_reader{
		Collector::Input operator()(Robot_inputs)const{ return Collector::Input{};}
		Robot_inputs operator()(Robot_inputs a,Collector::Input)const{ return a;}
	};
	Input_reader input_reader;

	typedef Goal Output;
	Output output;
	
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const{ return Robot_outputs{}; }
		Collector::Output operator()(Robot_outputs)const{ return Collector::Output{}; }	
	};
	Output_applicator output_applicator;

	struct Estimator{
		Status_detail get()const{ return Status_detail{};}
		void update(Time,Input,Output){};	
	};
	Estimator estimator;
	
	Collector():goal(Goal::OFF){}
};

ostream& operator<<(ostream& o, Collector::Goal a);
ostream& operator<<(ostream& o, Collector a);

#endif
