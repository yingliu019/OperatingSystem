#include "PRIOScheduler.h"

PRIOScheduler::PRIOScheduler(int q)
	: Scheduler(q) {
	vector<list<shared_ptr<Process>>> activeQueue, expiredQueue;
}

void PRIOScheduler::add_process(shared_ptr<Process> proc) {

	if (activeQueue.size() < proc->static_priority) {
		activeQueue.resize(proc->static_priority);
	}
	int level = proc->dynamic_priority;
	activeQueue[level].push_back(proc);
}

// pull an event from the event queue
shared_ptr<Process> PRIOScheduler::get_next_process() {
	// only swap in get next process
	bool swap = true;

	for (auto it = activeQueue.rbegin(); it != activeQueue.rend(); ++it) {
		if (!(*it).empty()) {
			swap = false;
			break;
		}
	}

	if (swap == true) {
		// if active queue empty, SWAP
		activeQueue = expiredQueue;
		expiredQueue.clear();
		// swap(activeQueue, expiredQueue);
	}

	for (auto it = activeQueue.rbegin(); it != activeQueue.rend(); ++it) {
		if (!(*it).empty()) {
			shared_ptr<Process> tmp = (*it).front();
			(*it).pop_front();
			return tmp;
		}
	}

	// if both empty
	return nullptr;
}

bool PRIOScheduler::change_process_dp(shared_ptr<Process> proc, bool minus) {
	if (minus == true) {
		proc->dynamic_priority -= 1;
		if (proc->dynamic_priority == -1) {
			proc->dynamic_priority = proc->static_priority - 1;
			// add to expired queue
			if (expiredQueue.size() < proc->static_priority) {
				expiredQueue.resize(proc->static_priority);
			}
			int level = proc->dynamic_priority;
			expiredQueue[level].push_back(proc);
			return true;
		}
	} else {
		proc->dynamic_priority = proc->static_priority - 1;
	}
	return false;
}