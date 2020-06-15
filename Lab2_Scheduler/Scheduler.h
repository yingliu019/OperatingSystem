#pragma once

#include <list> 
#include <vector>
#include <iostream>
#include "Process.h"

using namespace std;

class Scheduler {

public:
	Scheduler(int);

	int quantum;

	virtual void add_process(shared_ptr<Process>) = 0;
	virtual shared_ptr<Process> get_next_process() = 0;
	virtual bool change_process_dp(shared_ptr<Process>, bool);
	virtual bool test_preempt(shared_ptr<Process>, shared_ptr<Process>, int, bool); // typically NULL but for ‘E’

//protected:
	list<shared_ptr<Process>> readyQueue;
	vector<list<shared_ptr<Process>>> activeQueue;
	vector<list<shared_ptr<Process>>> expiredQueue;
};
