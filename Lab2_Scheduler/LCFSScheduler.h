#pragma once
#include "Scheduler.h"

class LCFSScheduler : public Scheduler {

public:
	LCFSScheduler(int);

	void add_process(shared_ptr<Process>);
	shared_ptr<Process> get_next_process();
};