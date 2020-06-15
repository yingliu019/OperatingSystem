#include "RRScheduler.h"

RRScheduler::RRScheduler(int q)
	: Scheduler(q) {
}

void RRScheduler::add_process(shared_ptr<Process> proc) {
	readyQueue.push_back(proc);
}

// pull an event from the event queue
shared_ptr<Process> RRScheduler::get_next_process() {
	if (!readyQueue.empty()) {
		shared_ptr<Process> tmp = readyQueue.front();
		readyQueue.pop_front();
		return tmp;
	}
	return nullptr;
}