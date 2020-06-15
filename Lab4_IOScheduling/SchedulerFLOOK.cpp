#include "SchedulerFLOOK.h"

SchedulerFLOOK::SchedulerFLOOK()
	: Scheduler() {
}

void SchedulerFLOOK::add_request(shared_ptr<IORequest> r) {
	add_queue.push_back(r);
}

// pull an event from the event queue
shared_ptr<IORequest> SchedulerFLOOK::get_next_request() {
	if (active_queue.empty() && add_queue.empty()) {
		return nullptr;
	}

	if (active_queue.empty()) {
		active_queue = add_queue;
		add_queue.clear();
	}

	list<shared_ptr<IORequest>>::iterator it;
	shared_ptr<IORequest> candidate = nullptr;
	int distance = INT_MAX;
	for (it = active_queue.begin(); it != active_queue.end(); it++) {
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
		for (it = active_queue.begin(); it != active_queue.end(); it++) {
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
	active_queue.remove(candidate);
	return candidate;
}

void SchedulerFLOOK::set_direction(shared_ptr<IORequest> nxt_target) {
	return;
}

bool SchedulerFLOOK::is_empty() {
	return active_queue.empty() && add_queue.empty();
}