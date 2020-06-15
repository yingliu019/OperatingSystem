#pragma once

#include "Scheduler.h"

class PRIOScheduler : public Scheduler {

public:
	PRIOScheduler(int);

	void add_process(shared_ptr<Process>);
	shared_ptr<Process> get_next_process();
	bool change_process_dp(shared_ptr<Process>, bool);

//private:
//	vector<list<shared_ptr<Process>>> activeQueue, expiredQueue;
};