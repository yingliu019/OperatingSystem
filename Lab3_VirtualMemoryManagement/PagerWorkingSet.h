#pragma once

#include "Pager.h"
#include <climits>

class PagerWorkingSet : public Pager {
public:
	PagerWorkingSet();
	shared_ptr<frame_t> select_victim_frame(shared_ptr<frame_t> frame_table[], int, vector<shared_ptr<Process>>, int);
};

