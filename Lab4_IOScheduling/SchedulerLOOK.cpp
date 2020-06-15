#include "SchedulerLOOK.h"

SchedulerLOOK::SchedulerLOOK()
	: Scheduler() {
}

void SchedulerLOOK::add_request(shared_ptr<IORequest> r) {
	IOqueue.push_back(r);
}

// pull an event from the event queue
shared_ptr<IORequest> SchedulerLOOK::get_next_request() {
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
		if (DIR == 1 && (*it)->track - HEAD_POS > 0 && (*it)->track - HEAD_POS < distance) {
			distance = (*it)->track - HEAD_POS;
			candidate = *it;
		}
		if (DIR == -1 && HEAD_POS - (*it)->track > 0 && HEAD_POS - (*it)->track < distance) {
			distance = HEAD_POS - (*it)->track;
			candidate = *it;
		}
	}

	if (distance == INT_MAX) {
		DIR = -DIR;
		for (it = IOqueue.begin(); it != IOqueue.end(); it++) {
			if (DIR == 1 && (*it)->track - HEAD_POS > 0 && (*it)->track - HEAD_POS < distance) {
				distance = (*it)->track - HEAD_POS;
				candidate = *it;
			}
			if (DIR == -1 && HEAD_POS - (*it)->track > 0 && HEAD_POS - (*it)->track < distance) {
				distance = HEAD_POS - (*it)->track;
				candidate = *it;
			}
		}
	}
	IOqueue.remove(candidate);
	return candidate;
}

void SchedulerLOOK::set_direction(shared_ptr<IORequest> nxt_target) {
	if (IOqueue.empty()) return;
	/*
	if (nxt_target->track > HEAD_POS) {
		DIR = 1;
		return;
	}

	if (nxt_target->track < HEAD_POS) {
		DIR = -1;
		return;
	}
	*/
}

bool SchedulerLOOK::is_empty() {
	return IOqueue.empty();
}