#include "SchedulerFIFO.h"

SchedulerFIFO::SchedulerFIFO()
	: Scheduler() {
}

void SchedulerFIFO::add_request(shared_ptr<IORequest> r) {
	IOqueue.push_back(r);
}

// pull an event from the event queue
shared_ptr<IORequest> SchedulerFIFO::get_next_request() {
	if (!IOqueue.empty()) {
		shared_ptr<IORequest> tmp = IOqueue.front();
		IOqueue.pop_front();
		return tmp;
	}
	return nullptr;
}

void SchedulerFIFO::set_direction(shared_ptr<IORequest> nxt_target) {
	if (nxt_target->track > HEAD_POS) {
		DIR = 1;
		return;
	}

	if (nxt_target->track < HEAD_POS) {
		DIR = -1;
		return;
	}
}

bool SchedulerFIFO::is_empty() {
	return IOqueue.empty();
}