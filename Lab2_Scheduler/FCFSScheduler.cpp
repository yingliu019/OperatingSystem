#include "FCFSScheduler.h"

FCFSScheduler::FCFSScheduler(int q)
	: Scheduler(q) {
}

void FCFSScheduler::add_process(shared_ptr<Process> proc) {
	readyQueue.push_back(proc);
}

// pull an event from the event queue
shared_ptr<Process> FCFSScheduler::get_next_process() {
	if (!readyQueue.empty()) {
		shared_ptr<Process> tmp = readyQueue.front();
		readyQueue.pop_front();
		return tmp;
	}
	return nullptr;
}