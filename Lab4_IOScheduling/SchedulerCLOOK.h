#pragma once

#include "Scheduler.h"

class SchedulerCLOOK : public Scheduler {

public:
	SchedulerCLOOK();

	void add_request(shared_ptr<IORequest>);
	shared_ptr<IORequest> get_next_request();
	void set_direction(shared_ptr<IORequest>);
	bool is_empty();
};
