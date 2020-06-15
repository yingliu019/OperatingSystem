#include "Scheduler.h"

Scheduler::Scheduler() {
	list<shared_ptr<IORequest>> IOqueue;
	list<shared_ptr<IORequest>> active_queue;
	list<shared_ptr<IORequest>> add_queue;
	HEAD_POS = 0;
	DIR = 1;
}

