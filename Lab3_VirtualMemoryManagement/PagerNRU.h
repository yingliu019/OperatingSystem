#pragma once

#include "Pager.h"

class PagerNRU : public Pager {
public:
	int last_inst;
	PagerNRU();
	shared_ptr<frame_t> select_victim_frame(shared_ptr<frame_t> frame_table[], int, vector<shared_ptr<Process>>, int);
};
