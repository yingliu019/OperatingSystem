#pragma once

#include <list> 
#include <vector>
#include <iostream>
#include <climits>
#include "IORequest.h"

using namespace std;

class Scheduler {

public:
	Scheduler();
	int HEAD_POS;
	int DIR;
	list<shared_ptr<IORequest>> IOqueue;
	list<shared_ptr<IORequest>> active_queue;
	list<shared_ptr<IORequest>> add_queue;

	virtual void add_request(shared_ptr<IORequest>) = 0;
	virtual shared_ptr<IORequest> get_next_request() = 0;
	virtual void set_direction(shared_ptr<IORequest>) = 0;
	virtual bool is_empty() = 0;
};
