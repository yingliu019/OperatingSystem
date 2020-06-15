#include "SchedulerCLOOK.h"

SchedulerCLOOK::SchedulerCLOOK()
	: Scheduler() {
}

void SchedulerCLOOK::add_request(shared_ptr<IORequest> r) {
	IOqueue.push_back(r);
}

// pull an event from the event queue
shared_ptr<IORequest> SchedulerCLOOK::get_next_request() {
	if (IOqueue.empty()) {
		return nullptr;
	}

	list<shared_ptr<IORequest>>::iterator it;
	shared_ptr<IORequest> candidate = nullptr;
	int distance = INT_MAX;
	for (it = IOqueue.begin(); it != IOqueue.end(); it++) {
		if ((*it)->track == HEAD_POS) {
			distance = (*it)->track - HEAD_POS;
			candidate = *it;
			break;
		}
		if (HEAD_POS < (*it)->track && (*it)->track - HEAD_POS < distance) {
			distance = (*it)->track - HEAD_POS;
			candidate = *it;
		}
	}

	if (distance == INT_MAX) {
		for (it = IOqueue.begin(); it != IOqueue.end(); it++) {
			if ((*it)->track < distance) {
				distance = (*it)->track;
				candidate = *it;
			}
		}
	}
	IOqueue.remove(candidate);
	return candidate;
}

void SchedulerCLOOK::set_direction(shared_ptr<IORequest> nxt_target) {
	if (nxt_target->track < HEAD_POS) {
		DIR = -1;
	} else{
		DIR = 1;
	}
}

bool SchedulerCLOOK::is_empty() {
	return IOqueue.empty();
}