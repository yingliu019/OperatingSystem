#include "PagerFIFO.h"

PagerFIFO::PagerFIFO()
	: Pager() {
}

shared_ptr<frame_t> PagerFIFO::select_victim_frame(shared_ptr<frame_t> frame_table[], int size, vector<shared_ptr<Process>> processes, int time) {
	shared_ptr<frame_t> victim_frame = frame_table[hand];
	hand = (hand + 1) % size;
	return victim_frame;
}