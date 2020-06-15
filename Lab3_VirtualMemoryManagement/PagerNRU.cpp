#include "PagerNRU.h"
#include <iostream>

using namespace std;

PagerNRU::PagerNRU()
	: Pager() {
	last_inst = -1;
}

shared_ptr<frame_t> PagerNRU::select_victim_frame(shared_ptr<frame_t> frame_table[], int size, vector<shared_ptr<Process>> processes, int time) {
	if (time == 62) {
		int aaa = 1;
	}

	int class_index = 4;
	int tmp_hand = hand;
	int victim_hand;
	int cur_class;

	shared_ptr<pte_t> pte;
	shared_ptr<frame_t> fte;
	shared_ptr<frame_t> victim_fte;

	fte = frame_table[hand];
	pte = processes[fte->proc_id]->page_table[fte->vpage];

	int overtime = 0;
	if (time - last_inst >= 50) {
		overtime = 1;
	}
	
	do {
		fte = frame_table[tmp_hand];
		pte = processes[fte->proc_id]->page_table[fte->vpage];

		if (pte->referenced == 0 && pte->modified == 0) {
			cur_class = 0;
			//cout << "ASSELECT " << hand << " " << overtime << " | " << 0 << endl;
			hand = (tmp_hand + 1) % size;
			if (overtime) {
				for (int i = 0; i < size; i++) {
					pte = processes[frame_table[i]->proc_id]->page_table[frame_table[i]->vpage];
					pte->referenced = 0;
				}
				last_inst = time;
			}
			return fte;
		} else if (pte->referenced == 0 && pte->modified == 1) {
			cur_class = 1;
		} else if (pte->referenced == 1 && pte->modified == 0) {
			cur_class = 2;
		} else{
			cur_class = 3;
		}

		if (cur_class < class_index) {
			class_index = cur_class;
			victim_fte = fte;
			victim_hand = tmp_hand;
		}
		tmp_hand = (tmp_hand + 1) % size;
	} while (tmp_hand != hand);

	//cout << "ASSELECT " << hand << " " << overtime << " | " << class_index << endl;
	hand = (victim_hand + 1) % size;
	if (overtime) {
		for (int i = 0; i < size; i++) {
			pte = processes[frame_table[i]->proc_id]->page_table[frame_table[i]->vpage];
			pte->referenced = 0;
		}
		last_inst = time;
	}
	return victim_fte;
}