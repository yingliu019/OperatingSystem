#pragma once

#include "Process.h"
#include <list> 
#include <memory>

struct Event {
	int evtTimeStamp;
	shared_ptr<Process> evtProcess;
	process_state_t oldstate;
	process_state_t newstate;
	// struct constructor
	Event(int e, shared_ptr<Process> ep, process_state_t os, process_state_t ns) 
		: evtTimeStamp(e), evtProcess(ep), oldstate(os), newstate(ns) {}
};


class DESLayer {
public:
	DESLayer();
	list<shared_ptr<Event>> eventQueue;
	void put_event(shared_ptr<Event>);
	void remove_event(shared_ptr<Process>);
	shared_ptr<Event> get_event();
	int get_next_event_time();
};