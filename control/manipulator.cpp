#include <iostream>
#include "formal.h"

using namespace std;

struct Arm{
	enum class Goal{UP,DOWN};
	enum class Status{UP,MID,DOWN};

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

	struct Output{
		Goal armMotor;
	};

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs r, Output out)const;
		Output operator()(Robot_outputs r)const;
	};

	struct Estimator{
		Estimator get()const{
			Estimator status;
			return status;
		}
		void update(Time t, Input in, Output out);
	};
	Estimator estimator;
	Input_reader input_reader;
	//Goal goal;
	Output_applicator output_applicator;
};

Arm::Output control(Arm::Estimator e,Arm::Goal goal);
Arm::Status status(Arm::Estimator);
bool ready(Arm::Status,Arm::Goal);

ostream& operator<<(ostream&,Arm::Input);
ostream& operator<<(ostream&,Arm::Input_reader);
ostream& operator<<(ostream&,Arm::Output);
ostream& operator<<(ostream&,Arm::Output_applicator);
ostream& operator<<(ostream&,Arm::Status);
ostream& operator<<(ostream&,Arm::Estimator);
ostream& operator<<(ostream& o,Arm){
	o<<"Arm[]";
	return o;
}
ostream& operator<<(ostream&,Arm::Goal);

bool operator==(Arm::Input a,Arm::Input b){
	return a.topLimit==b.topLimit;
}

bool operator==(Arm::Output a,Arm::Output b){
	return a.armMotor==b.armMotor;
}

bool operator!=(Arm::Output a,Arm::Output b){
	return !(a.armMotor==b.armMotor);
}

bool operator<(Arm::Output a,Arm::Output b);
bool operator<(Arm::Estimator a,Arm::Estimator b);

set<Arm::Input> examples(Arm::Input*){return {};}
set<Arm::Input_reader> examples(Arm::Input_reader*){return {};}
set<Arm::Output> examples(Arm::Output*){return {};}
set<Arm::Output_applicator> examples(Arm::Output_applicator*){return {};}
set<Arm::Goal> examples(Arm::Goal*){return {};}
set<Arm::Status> examples(Arm::Status*){return {};}
set<Arm::Estimator> examples(Arm::Estimator*){return {};}

bool askBool(string prompt){
	cout<<prompt<<" (t/f)\n";
	string answer;
	getline(cin,answer);
	return answer == "t";
}

#if 0
int main(){
	Arm arm;
	tester(arm);
}
#endif
