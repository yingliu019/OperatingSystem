#include "PagerAging.h"

PagerAging::PagerAging()
	: Pager() {
}

shared_ptr<frame_t> PagerAging::select_victim_frame(shared_ptr<frame_t> frame_table[], int size, vector<shared_ptr<Process>> processes, int time) {
	shared_ptr<pte_t> pte;
	shared_ptr<frame_t> fte;

	int tmp_hand = hand;
	int victim_hand;
	unsigned int victim_age = UINT_MAX;

	shared_ptr<frame_t> victim_fte;

	do {
		fte = frame_table[tmp_hand];
		pte = processes[fte->proc_id]->page_table[fte->vpage];

		if (fte->age < victim_age) {
			victim_fte = fte;
			victim_age = fte->age;
			victim_hand = tmp_hand;
		}
		tmp_hand = (tmp_hand + 1) % size;
	} while (tmp_hand != hand);

	hand = (victim_hand + 1) % size;

	for (int i = 0; i < size; i++) {
		fte = frame_table[i];
		pte = processes[fte->proc_id]->page_table[fte->vpage];
		fte->age = fte->age >> 1;
		pte->referenced = 0;
	}
	return victim_fte;
}
