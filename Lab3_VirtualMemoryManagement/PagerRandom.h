#pragma once

#include "Pager.h"
#include <vector>

class PagerRandom : public Pager {
public:
	int randCounter;
	int RAND_MOD;
	vector<int> randvals;

	PagerRandom(const char* randpath);
	shared_ptr<frame_t> select_victim_frame(shared_ptr<frame_t> frame_table[], int, vector<shared_ptr<Process>>, int);
	int myrandom(int);
};