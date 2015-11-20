#include "main.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <math.h>
#include <stdlib.h>
#include "toplevel.h"
#include "../util/util.h"
#include "toplevel_mode.h"
#include "../input/util.h"

using namespace std;

ostream& operator<<(ostream& o,Main::Mode a){
	#define X(NAME) if(a==Main::Mode::NAME) return o<<""#NAME;
	MODES
	#undef X
	assert(0);
}


//todo: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():mode(Mode::TELEOP),autonomous_start(0){}

double set_drive_speed(Joystick_data joystick,int axis,double boost,double slow,bool turning=0){
	static const float DEFAULT_SPEED=.45;//Change these value to change the default speed
	static const float TURNING_SLOW=.33;
	static const float SLOW_BY=.5;//Change this value to change the percentage of the default speed the slow button slows
	return pow(joystick.axis[axis],3)*((DEFAULT_SPEED+(1-DEFAULT_SPEED)*boost)-(((DEFAULT_SPEED*SLOW_BY)*slow)+((DEFAULT_SPEED*TURNING_SLOW)*turning)));
}

template<typename T>//Compares two types to see if one is within a range
bool in_range(T a,T b,T c){
	return a<b+c&&a>b-c;
}

template<size_t LEN>
array<double,LEN> floats_to_doubles(array<float,LEN> a){
	array<double,LEN> r;
	for(size_t i=0;i<LEN;i++) r[i]=a[i];
	return r;
}


Toplevel::Goal Main::teleop(
	Robot_inputs const& in,
	Joystick_data const& main_joystick,
	Joystick_data const& /* gunner_joystick*/,
	Panel const&  /*oi_panel*/,
	Toplevel::Status_detail& /*toplevel_status*/
){
	//cout<<toplevel_status<<"\n";
	Toplevel::Goal goals;

	//Change these nudge values to adjust the nudge speeds/amounts
	static const float Y_NUDGE_POWER=.2;
	static const float ROTATE_NUDGE_POWER=.5;

	static const bool SLOW_TURNING=0;//Slows drive turning
	
	const double turbo_button=main_joystick.axis[Gamepad_axis::LTRIGGER], slow_button=main_joystick.axis[Gamepad_axis::RTRIGGER];
	
	Drivebase::Goal &goal=goals.drive;	
	if(!nudges[2].timer.done())goal.y=-Y_NUDGE_POWER;
	else if(!nudges[3].timer.done())goal.y=Y_NUDGE_POWER;
	else goal.y=set_drive_speed(main_joystick,1,turbo_button,main_joystick.axis[Gamepad_axis::RTRIGGER]);
	
	if(!nudges[4].timer.done())goal.theta=-ROTATE_NUDGE_POWER;
	else if(!nudges[5].timer.done()) goal.theta=ROTATE_NUDGE_POWER;
	else goal.theta=-set_drive_speed(main_joystick,4,turbo_button,slow_button,SLOW_TURNING);//theta is /2 so rotation is reduced to prevent bin tipping.

	const bool normal_nudge_enable=turbo_button<.25;			
	static const auto NUDGE_CCW_BUTTON=Gamepad_button::RB,NUDGE_CW_BUTTON=Gamepad_button::LB;
	static const auto NUDGE_FWD_BUTTON=Gamepad_button::Y,NUDGE_BACK_BUTTON=Gamepad_button::A;
	static const unsigned int nudge_buttons[4]={NUDGE_FWD_BUTTON,NUDGE_BACK_BUTTON,NUDGE_CCW_BUTTON,NUDGE_CW_BUTTON};
	for(int i=0;i<4;i++){
		bool start=nudges[i].trigger(normal_nudge_enable&&main_joystick.button[nudge_buttons[i]]);
		if(start)nudges[i].timer.set(.1);
		nudges[i].timer.update(in.now,1);
	}
	
	
	if(!normal_nudge_enable){
		if(main_joystick.button[NUDGE_FWD_BUTTON]){
			bool left=in.digital_io.in[7]==Digital_in::_1, right=in.digital_io.in[8]==Digital_in::_1;
			if(!left&&!right){
				goal.y=-Y_NUDGE_POWER*1.5;
			}else{
				if(!left){
					goal.theta=ROTATE_NUDGE_POWER/2;
					goal.y=-Y_NUDGE_POWER/2;
				}
				if(!right){
					goal.theta=-ROTATE_NUDGE_POWER/2;
					goal.y=-Y_NUDGE_POWER/2;
				}
			}
		}
	}
	
	goals.drive=goal;
		
	return goals;
}

unsigned pdb_location1(Drivebase::Motor m){
	#define X(NAME,INDEX) if(m==Drivebase::NAME) return INDEX;
	//WILL NEED CORRECT VALUES
	X(LEFT,12)
	X(RIGHT,13)
	#undef X
	assert(0);
	//assert(m>=0 && m<Drivebase::MOTORS);
}

Main::Mode next_mode(Main::Mode m,bool autonomous,bool autonomous_start,Toplevel::Status_detail /*status*/,Time since_switch, Panel oi_panel){
	switch(m){
		case Main::Mode::TELEOP:
			if(autonomous_start){
				if (oi_panel.in_use) {
					switch(oi_panel.auto_mode){ 
						case Panel::Auto_mode::CAN_GRAB:
							return Main::Mode::AUTO_GRAB;
						case Panel::Auto_mode::MOVE:
							return Main::Mode::AUTO_MOVE;
						case Panel::Auto_mode::DO_NOTHING:
							return Main::Mode::TELEOP;
						default: assert(0);
					}
				} else {
					return Main::Mode::AUTO_GRAB;
				}
			}
			return m;
		case Main::Mode::AUTO_MOVE:
			//encoders? going to use time for now
			if(!autonomous || since_switch>1) return Main::Mode::TELEOP;
			return m;
		case Main::Mode::AUTO_GRAB:
			if(!autonomous) return Main::Mode::TELEOP;
			return m;
		case Main::Mode::AUTO_BACK:
			if(!autonomous) return Main::Mode::TELEOP;
			//timer is up - could use encoders once those work
			if(since_switch>1.5) return Main::Mode::AUTO_RELEASE;
			return m;
		case Main::Mode::AUTO_RELEASE:
			return m;	
		default: assert(0);
	}
}

Robot_outputs Main::operator()(Robot_inputs in,ostream&){
	perf.update(in.now);
	Joystick_data main_joystick=in.joystick[0];
	Joystick_data gunner_joystick=in.joystick[1];
	Panel oi_panel=interpret(in.joystick[2]);
	if(!in.robot_mode.enabled || in.robot_mode.autonomous) oi_panel.level_button=Panel::Level_button::DEFAULT;
	if(!in.robot_mode.enabled || in.robot_mode.autonomous) oi_panel.operation_buttons=Panel::Operation_buttons::DEFAULT;
		
	force.update(
		main_joystick.button[Gamepad_button::A],
		main_joystick.button[Gamepad_button::LB],
		main_joystick.button[Gamepad_button::RB],
		main_joystick.button[Gamepad_button::BACK],
		main_joystick.button[Gamepad_button::B],
		main_joystick.button[Gamepad_button::X]
	);
	
	Toplevel::Status_detail toplevel_status=toplevel.estimator.get();
	
	static int print_out_speed=0;	
	if(print_out_speed%300==0)cout<<"panel: "<<oi_panel<<"\n";	
	print_out_speed++;
	
	if (in.digital_io.encoder[0]) cout<<"Wheel 1: "<<*in.digital_io.encoder[0]<<"\n";
	if (in.digital_io.encoder[1]) cout<<"Wheel 2: "<<*in.digital_io.encoder[1]<<"\n";
	if (in.digital_io.encoder[2]) cout<<"Wheel 3: "<<*in.digital_io.encoder[2]<<"\n";
	bool autonomous_start_now=autonomous_start(in.robot_mode.autonomous && in.robot_mode.enabled);
	since_auto_start.update(in.now,autonomous_start_now);
		
	Toplevel::Goal goals;
	//Drivebase::Status_detail status_detail = drivebase.estimator.get();
	
	switch(mode){
		case Mode::TELEOP:
			goals=teleop(in,main_joystick,gunner_joystick,oi_panel,toplevel_status);
			break;
		case Mode::AUTO_MOVE:
			goals.drive.y=-.45;
			goals.drive.theta=0;
			break;
		case Mode::AUTO_GRAB:
			
			break;
		case Mode::AUTO_BACK:
			goals.drive.y=-.6;
			goals.drive.theta=0;
			break;
		case Mode::AUTO_RELEASE:
			break;	
		default: assert(0);
	}
	auto next=next_mode(mode,in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),oi_panel);
	since_switch.update(in.now,mode!=next);
	mode=next;

	Toplevel::Output r_out=control(toplevel_status,goals); 

	auto r=toplevel.output_applicator(Robot_outputs{},r_out);
	r=force(r);
	auto input=toplevel.input_reader(in);

	toplevel.estimator.update(
		in.now,
		input,
		/*Toplevel::Input{
			{can_input,tote_input},
			Kicker::Input{},
			drive_in,
			Pump::Input::NOT_FULL,
			Can_grabber::Input{1}, //todo: make this actually ready from a digital io
			Tote_sensors::Input{0,0,0}
		},*/
		toplevel.output_applicator(r)
	);
	return r;
}

bool operator==(Main a,Main b){
	return a.force==b.force && 
		a.perf==b.perf && 
		a.toplevel==b.toplevel && 
		a.since_switch==b.since_switch && 
		a.since_auto_start==b.since_auto_start &&
		a.autonomous_start==b.autonomous_start;
}

bool operator!=(Main a,Main b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Main m){
	o<<"Main(";
	o<<m.mode;
	o<<m.force;
	o<<m.perf;
	o<<m.toplevel;
	o<<m.since_switch;
	//since_auto_start
	//o<<m.control;
	//ball collector
	//print button
	//relative
	//field relative
	return o<<")";
}

bool approx_equal(Main a,Main b){
	if(a.force!=b.force) return 0;
	if(a.toplevel!=b.toplevel) return 0;
	return 1;
}

#ifdef MAIN_TEST
#include<fstream>
#include "monitor.h"

template<typename T>
vector<T> uniq(vector<T> v){
	vector<T> r;
	for(auto a:v){
		if(!(r.size()&&r[r.size()-1]==a)){
			r|=a;
		}
	}
	return r;
}

int main(){
}
#endif
