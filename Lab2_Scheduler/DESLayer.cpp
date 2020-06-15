#include <iostream>
#include "DESLayer.h"

DESLayer::DESLayer() {
	list<shared_ptr<Event>> eventQueue;
}

void DESLayer::put_event(shared_ptr<Event> e) {
	list<shared_ptr<Event>>::iterator it;
	for (it = eventQueue.begin(); it != eventQueue.end(); it++) {
		if (e->evtTimeStamp < (*it)->evtTimeStamp) {
			eventQueue.insert(it, e);
			return;
		}
	}
	eventQueue.push_back(e);
}

void DESLayer::remove_event(shared_ptr<Process> proc) {
	list<shared_ptr<Event>>::iterator i = eventQueue.begin();
	while (i != eventQueue.end()) {
		bool willErase = (*i)->evtProcess == proc;
		if (willErase) {
			eventQueue.erase(i++);  // alternatively, i = items.erase(i);
		} else {
			++i;
		}
	}
}

int DESLayer::get_next_event_time() {  // todo
	if (!eventQueue.empty()) {
		shared_ptr<Event> e = eventQueue.front();
		return e->evtTimeStamp;
	}
	return -1;
}

// pull an event from the event queue
shared_ptr<Event> DESLayer::get_event() {
	if (!eventQueue.empty()) {
		shared_ptr<Event> e = eventQueue.front();
		eventQueue.pop_front();
		return e;
	} 
	return nullptr;
}