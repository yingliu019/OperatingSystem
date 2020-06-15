#pragma once

#include "Scheduler.h"

class PREPRIOScheduler : public Scheduler {

public:
	PREPRIOScheduler(int);

	void add_process(shared_ptr<Process>);
	shared_ptr<Process> get_next_process();
	bool change_process_dp(shared_ptr<Process>, bool);
	bool test_preempt(shared_ptr<Process>, shared_ptr<Process>, int, bool);
	//private:
	//	vector<list<shared_ptr<Process>>> activeQueue, expiredQueue;
};