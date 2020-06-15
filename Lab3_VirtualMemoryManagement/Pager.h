#pragma once

#include <memory>
#include <vector>
#include "Process.h"

using namespace std;

struct frame_t {
	// for reverse mapping
	int proc_id;
	int vpage;
	const int frame_index;
	int last_used;
	unsigned int age;
	bool used;
	frame_t(int p, int v, int f, int t, int a, bool b)
		: proc_id(p), vpage(v), frame_index(f), last_used(t), age(a), used(b) {}
};


class Pager {
public:
	Pager();
	int hand;
	virtual shared_ptr<frame_t> select_victim_frame(shared_ptr<frame_t> frame_table[], int, vector<shared_ptr<Process>>, int) = 0; // virtual base class
};
