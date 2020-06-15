#pragma once

#include "Scheduler.h"

class SRTFScheduler : public Scheduler {

public:
	SRTFScheduler(int);

	void add_process(shared_ptr<Process>);
	shared_ptr<Process> get_next_process();
};