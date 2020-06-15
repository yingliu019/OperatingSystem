#include "LCFSScheduler.h"

LCFSScheduler::LCFSScheduler(int q)
	: Scheduler(q) {
}

void LCFSScheduler::add_process(shared_ptr<Process> proc) {
	readyQueue.push_front(proc);
}

// pull an event from the event queue
shared_ptr<Process> LCFSScheduler::get_next_process() {
	if (!readyQueue.empty()) {
		shared_ptr<Process> tmp = readyQueue.front();
		readyQueue.pop_front();
		return tmp;
	}
	return nullptr;
}