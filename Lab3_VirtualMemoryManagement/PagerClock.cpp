#include "PagerClock.h"
#include "Process.h"

PagerClock::PagerClock()
	: Pager() {
}

shared_ptr<frame_t> PagerClock::select_victim_frame(shared_ptr<frame_t> frame_table[], int size, vector<shared_ptr<Process>> processes, int time) {
	shared_ptr<pte_t> pte;
	shared_ptr<frame_t> fte;

	while (true) {
		fte = frame_table[hand];
		pte = processes[fte->proc_id]->page_table[fte->vpage];
		hand = (hand + 1) % size;

		if (pte->referenced == 0) {
			return fte;
		} else {
			pte->referenced = 0;
		}
	}
}