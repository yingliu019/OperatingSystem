#pragma once
#include "Scheduler.h"

class FCFSScheduler : public Scheduler {

public:
	FCFSScheduler(int);

	void add_process(shared_ptr<Process>);
	shared_ptr<Process> get_next_process();
};