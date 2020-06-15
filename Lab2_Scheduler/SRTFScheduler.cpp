#include "SRTFScheduler.h"

SRTFScheduler::SRTFScheduler(int q)
	: Scheduler(q) {
}

void SRTFScheduler::add_process(shared_ptr<Process> proc) { 
	list<shared_ptr<Process>>::iterator it;
	for (it = readyQueue.begin(); it != readyQueue.end(); it++) {
		if (proc->remCPU < (*it)->remCPU) {
			readyQueue.insert(it, proc);
			return;
		}
	}
	readyQueue.push_back(proc);
}


// pull an event from the event queue
shared_ptr<Process> SRTFScheduler::get_next_process() { 
	if (!readyQueue.empty()) {
		shared_ptr<Process> tmp = readyQueue.front();
		readyQueue.pop_front();
		return tmp;
	}
	return nullptr;
}