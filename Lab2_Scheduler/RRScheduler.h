#pragma once
#include "Scheduler.h"

class RRScheduler : public Scheduler {

public:
	RRScheduler(int);

	void add_process(shared_ptr<Process>);
	shared_ptr<Process> get_next_process();
};