#pragma once
#include <climits>
#include "Pager.h"

class PagerAging : public Pager {
public:
	PagerAging();
	shared_ptr<frame_t> select_victim_frame(shared_ptr<frame_t> frame_table[], int, vector<shared_ptr<Process>>, int);
};
