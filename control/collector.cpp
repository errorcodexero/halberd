#include <iostream>
#include "collector.h"

using namespace std;

ostream& operator<<(ostream& o, Collector::Goal a){
	if(a==Collector::Goal::OFF)return o<<"Goal(off)";
	if(a==Collector::Goal::REVERSE)return o<<"Goal(reverse)";
	if(a==Collector::Goal::FORWARD)return o<<"Goal(forward)";
	assert(0);
}

ostream& operator<<(ostream& o, Collector a){
	return o<<"Collector(goal:"<<a.goal<<" status:"<<a.status<<" output:"<<a.output<<")";
}

#ifdef KICKER_TEST
#include "formal.h"

int main(){
	Collector a;
	tester(a);
	return 0;
}

#endif
