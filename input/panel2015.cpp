#include "panel2015.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"

using namespace std;

Panel::Panel():
	auto_mode(Auto_mode::DO_NOTHING),
	level_button(Level_button::DEFAULT),
	operation_buttons(Operation_buttons::DROP_CURRENT),
	slide_pos(0.0),
	move_arm_to_pos(0),
	current_collect(0),
	move_drop(0),
	move_collect(0),
	chute_collect(0),
	lifter_off(0),
	piston_aligner(0),
	kicker_activate(0),
	target_type(0),
	move_arm_one(0),
	move_arm_cont(0),
	collect_mode(0),
	drop_mode(0)
{}

ostream& operator<<(ostream& o,Panel::Auto_mode a){
	o<<" Auto_Mode(";
	if(a==Panel::Auto_mode::DO_NOTHING)o<<"do_nothing";
	if(a==Panel::Auto_mode::MOVE)o<<"move";
	if(a==Panel::Auto_mode::CAN_GRAB)o<<"can_grab";
	o<<")";
	return o;
}

ostream& operator<<(ostream& o,Panel::Level_button a){
	o<<" Level(";
	if(a==Panel::Level_button::LEVEL0)o<<"0";
	if(a==Panel::Level_button::LEVEL1)o<<"1";
	if(a==Panel::Level_button::LEVEL2)o<<"2";
	if(a==Panel::Level_button::LEVEL3)o<<"3";
	if(a==Panel::Level_button::LEVEL4)o<<"4";
	if(a==Panel::Level_button::LEVEL5)o<<"5";	
	if(a==Panel::Level_button::LEVEL6)o<<"6";
	o<<")";
	return o;
}

ostream& operator<<(ostream& o,Panel::Operation_buttons a){
	o<<" Operation_buttons(";
	if(a==Panel::Operation_buttons::COLLECT_CURRENT)o<<"collect_current";
	if(a==Panel::Operation_buttons::DROP_CURRENT)o<<"drop_current";
	if(a==Panel::Operation_buttons::MOVE_COLLECT)o<<"move_collect";
	if(a==Panel::Operation_buttons::MOVE_DROP)o<<"move_drop";
	o<<")";
	return o;
}

ostream& operator<<(ostream& o,Panel p){
	o<<"Panel(";
	o<<" "<<p.auto_mode;
	o<<", "<<p.level_button;
	o<<", "<<p.operation_buttons;
	o<<", slide_pos:"<<p.slide_pos;
	o<<", Buttons(";
	o<<" move_arm_to_pos:"<<p.move_arm_to_pos;
	o<<", collect_current:"<<p.current_collect;
	o<<", current_drop:"<<p.current_drop;
	o<<", move_drop:"<<p.move_drop;
	o<<", move_collect:"<<p.move_collect;
	o<<", chute_collect:"<<p.chute_collect;
	o<<", lifter_off:"<<p.lifter_off;
	o<<", piston_aligner:"<<p.piston_aligner;
	o<<", kicker_activate:"<<p.kicker_activate<<")";
	o<<", 3_pos_switches(";
	o<<" target_type:"<<p.target_type;
	o<<", move_arm_one"<<p.move_arm_one;
	o<<", move_arm_cont"<<p.move_arm_cont;
	o<<", collect_mode"<<p.collect_mode;
	o<<", drop_mode"<<p.drop_mode<<")";
	o<<")";
    return o;
}

Panel::Auto_mode auto_mode_convert(int potin){
	if(potin==0)return Panel::Auto_mode::DO_NOTHING;
	if(potin==1)return Panel::Auto_mode::MOVE;
	if(potin==2)return Panel::Auto_mode::CAN_GRAB;
	else return Panel::Auto_mode::DO_NOTHING;
}

Panel interpret(Joystick_data d){
	Panel panel;
	{
	panel.values=d;
	}
	{
	Volt auto_mode=d.axis[0]/3.3*5;
	panel.auto_mode=auto_mode_convert(interpret_10_turn_pot(auto_mode));
	}
	{
		float lev=d.axis[1];//default: -1
		//cout<<"\n\n\n lev:"<<lev<<"  2:"<<d.digital[2]<<"\n\n\n";
		static const float DEFAULT=-1,LEVEL0=-.75,LEVEL1=-.5,LEVEL2=-.25,LEVEL3=0,LEVEL4=.32,LEVEL5=.65,LEVEL6=1;
		//cout<<endl<<lev<<endl;
		if(!d.button[2]){//tests if override is being pushed
			if(lev==DEFAULT)panel.level_button=Panel::Level_button::DEFAULT;
			else if(lev>LEVEL0-(LEVEL0-DEFAULT)/2&&lev<LEVEL0+(LEVEL1-LEVEL0)/2)panel.level_button=Panel::Level_button::LEVEL0;//-.75
			else if(lev>LEVEL1-(LEVEL1-LEVEL0)/2&&lev<LEVEL1+(LEVEL2-LEVEL1)/2)panel.level_button=Panel::Level_button::LEVEL1;//-.5
			else if(lev>LEVEL2-(LEVEL2-LEVEL1)/2&&lev<LEVEL2+(LEVEL3-LEVEL2)/2)panel.level_button=Panel::Level_button::LEVEL2;//-.25
			else if(lev>LEVEL3-(LEVEL3-LEVEL2)/2&&lev<LEVEL3+(LEVEL4-LEVEL3)/2)panel.level_button=Panel::Level_button::LEVEL3;//0
			else if(lev>LEVEL4-(LEVEL4-LEVEL3)/2&&lev<LEVEL4+(LEVEL5-LEVEL4)/2)panel.level_button=Panel::Level_button::LEVEL4;//.32
			else if(lev>LEVEL5-(LEVEL5-LEVEL4)/2&&lev<LEVEL5+(LEVEL6-LEVEL5)/2)panel.level_button=Panel::Level_button::LEVEL5;//.65
			else if(lev>LEVEL6-(LEVEL6-LEVEL5)/2&&lev<LEVEL6+.25)panel.level_button=Panel::Level_button::LEVEL6;//1
		}
		else{//This sets it to the SlipnSlide
			panel.override_height=(d.axis[2]+1)*((65-5)/2);
		}
	}
	{
		float op=d.axis[0];//default: -1
		static const float COLLECT_CURRENT=1,MOVE_COLLECT=.65,MOVE_DROP=.32,DROP_CURRENT=0, DEFAULT=-1;
		if(op>DROP_CURRENT-(DROP_CURRENT-DEFAULT)/2&&op<DROP_CURRENT+(MOVE_DROP-DROP_CURRENT)/2)panel.operation_buttons=Panel::Operation_buttons::DROP_CURRENT;//0
		else if(op>MOVE_DROP-(MOVE_DROP-DROP_CURRENT)/2&&op<MOVE_DROP+(MOVE_COLLECT-MOVE_DROP)/2)panel.operation_buttons=Panel::Operation_buttons::MOVE_DROP;//.32
		else if(op>MOVE_COLLECT-(MOVE_COLLECT-MOVE_DROP)/2&&op<MOVE_COLLECT+(COLLECT_CURRENT-MOVE_COLLECT)/2)panel.operation_buttons=Panel::Operation_buttons::MOVE_COLLECT;//.65
		else if(op>COLLECT_CURRENT-(COLLECT_CURRENT-MOVE_COLLECT)/2&&op<COLLECT_CURRENT+.25)panel.operation_buttons=Panel::Operation_buttons::COLLECT_CURRENT;//1
	}
	//panel.slide_pos=(d.analog[2]+1)*((65-5)/2);//May be useless due to previous things
	if(d.button[3])panel.lifter_off=1;
	if(!d.button[3])panel.lifter_off=0;
	{	
		static const float DOWN=1, UP=.48, DEFAULT=-1;
		float updowncontrol=d.axis[4];
		if(updowncontrol>UP-(UP-DEFAULT)/2 && updowncontrol<UP+(DOWN-UP)/2) panel.move_arm_cont=1;
		else if(updowncontrol>DOWN-(DOWN-UP)/2 && updowncontrol<DOWN+.25 ) panel.move_arm_cont=-1;
		else panel.move_arm_cont=0;		
	}
	{
		static const float DOWN=0,UP=-.5,DEFAULT=-1;;
		float level_up_down_control=d.axis[4];
		if(level_up_down_control>UP-(UP-DEFAULT)/2 && level_up_down_control<UP+(DOWN-UP)/2)panel.move_arm_one=1;
		if(level_up_down_control>DOWN-(DOWN-UP)/2 && level_up_down_control<DOWN+.25)panel.move_arm_one=-1;
		else panel.move_arm_one=0;
	}
	return panel;
}

#ifdef PANEL2015_TEST
Joystick_data driver_station_input_rand(){//Copied over from hammer. Adrian update this if needed.
	Joystick_data r;
	for(unsigned i=0;i<JOY_AXES;i++){
		r.axis[i]=(0.0+rand()%101)/100;
	}
	for(unsigned i=0;i<JOY_BUTTONS;i++){
		r.button[i]=rand()%2;
	}
	return r;
}

int main(){
	Panel p;
	for(unsigned i=0;i<50;i++){
		interpret(driver_station_input_rand());
	}
	cout<<p<<"\n";
	return 0;
}
#endif
