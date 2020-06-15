#include "SchedulerSSTF.h"

SchedulerSSTF::SchedulerSSTF()
	: Scheduler() {
}

void SchedulerSSTF::add_request(shared_ptr<IORequest> r) {
	IOqueue.push_back(r);
}

// pull an event from the event queue
shared_ptr<IORequest> SchedulerSSTF::get_next_request() {
	if (IOqueue.empty()) {
		return nullptr;
	}

	list<shared_ptr<IORequest>>::iterator it;
	shared_ptr<IORequest> candidate = nullptr;
	int distance = INT_MAX;
	for (it = IOqueue.begin(); it != IOqueue.end(); it++) {
		if (abs((*it)->track - HEAD_POS) < distance) {
			distance = abs((*it)->track - HEAD_POS);
			candidate = *it;
		}
	}

	IOqueue.remove(candidate);
	return candidate;
}

void SchedulerSSTF::set_direction(shared_ptr<IORequest> nxt_target) {
	if (nxt_target->track > HEAD_POS) {
		DIR = 1;
		return;
	}

	if (nxt_target->track < HEAD_POS) {
		DIR = -1;
		return;
	}
}

bool SchedulerSSTF::is_empty() {
	return IOqueue.empty();
}