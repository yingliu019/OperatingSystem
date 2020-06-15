#include "PagerWorkingSet.h"
#include <climits>

PagerWorkingSet::PagerWorkingSet()
	: Pager() {
}

shared_ptr<frame_t> PagerWorkingSet::select_victim_frame(shared_ptr<frame_t> frame_table[], int size, vector<shared_ptr<Process>> processes, int time) {

	shared_ptr<pte_t> pte;
	shared_ptr<frame_t> fte;
	shared_ptr<frame_t> victim_fte;

	for (int i = 0; i < 2; i++) {
		int tmp_hand = hand;
		int victim_hand = INT_MAX;
		int smallest_time = INT_MAX;

		do {
			fte = frame_table[tmp_hand];
			pte = processes[fte->proc_id]->page_table[fte->vpage];

			if (pte->referenced == 1) {
				pte->referenced = 0;
				fte->last_used = time;
			}
			else if (pte->referenced == 0 && time - fte->last_used >= 50) {
				hand = (tmp_hand + 1) % size;
				return fte;
			}
			else {
				if (fte->last_used < smallest_time) {
					victim_fte = fte;
					victim_hand = tmp_hand;
					smallest_time = fte->last_used;
				}
			}
			tmp_hand = (tmp_hand + 1) % size;
		} while (tmp_hand != hand);

		if (victim_hand != INT_MAX) {
			hand = (victim_hand + 1) % size;
			return victim_fte;
		}
	}
}