#pragma once

#include "Pager.h"

class PagerFIFO : public Pager {
public:
	PagerFIFO();
	shared_ptr<frame_t> select_victim_frame(shared_ptr<frame_t> frame_table[], int, vector<shared_ptr<Process>>, int);
};