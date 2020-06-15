#include "Scheduler.h"

Scheduler::Scheduler(int q) {
	quantum = q;
	list<shared_ptr<Process>> readyQueue;
	vector<list<shared_ptr<Process>>> activeQueue;
	vector<list<shared_ptr<Process>>> expiredQueue;
}

bool Scheduler::test_preempt(shared_ptr<Process> p, shared_ptr<Process> q, int curtime, bool verbose) {
	return false;
}

bool Scheduler::change_process_dp(shared_ptr<Process> p, bool m) {
	return false;
}