#include "PREPRIOScheduler.h"

PREPRIOScheduler::PREPRIOScheduler(int q)
	: Scheduler(q) {
	vector<list<shared_ptr<Process>>> activeQueue, expiredQueue;
}

void PREPRIOScheduler::add_process(shared_ptr<Process> proc) {

	if (activeQueue.size() < proc->static_priority) {
		activeQueue.resize(proc->static_priority);
	}
	int level = proc->dynamic_priority;
	activeQueue[level].push_back(proc);
}

// pull an event from the event queue
shared_ptr<Process> PREPRIOScheduler::get_next_process() {
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

bool PREPRIOScheduler::change_process_dp(shared_ptr<Process> proc, bool minus) {
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
	}
	else {
		proc->dynamic_priority = proc->static_priority - 1;
	}
	return false;
}

bool PREPRIOScheduler::test_preempt(shared_ptr<Process> cur_running, shared_ptr<Process> just_wakeup, int curtime, bool verbose) {
	if (cur_running == nullptr) {
		return false;
	}
	int ts = cur_running->ts;  // next event associated with cur_running
	bool check0 = just_wakeup->dynamic_priority > cur_running->dynamic_priority;
	bool check1 = ts > curtime;

	if ((check0 && check1) == true) {
		if (verbose) {
			cout << "---> PRIO preemption " << cur_running->pid << " by " << just_wakeup->pid << " ? " << check0 << " TS=" << ts << " now=" << curtime << ") --> YES" << endl;
		}
		return true;
	}
	else {
		if (verbose) {
			cout << "---> PRIO preemption " << cur_running->pid << " by " << just_wakeup->pid << " ? " << check0 << " TS=" << ts << " now=" << curtime << ") --> NO" << endl;
		}
		return false;
	}
}
