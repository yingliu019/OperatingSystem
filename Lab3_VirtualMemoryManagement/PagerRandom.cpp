#include "PagerRandom.h"
#include <iostream>
#include <fstream>

using namespace std;

PagerRandom::PagerRandom(const char* randpath)
	: Pager() {
	randCounter = 0;
	ifstream randfile;
	randfile.open(randpath);
	int num;
	randfile >> num;
	RAND_MOD = num;
	while (randfile >> num) {
		randvals.push_back(num);
	}
	randfile.close();
}

shared_ptr<frame_t> PagerRandom::select_victim_frame(shared_ptr<frame_t> frame_table[], int size, vector<shared_ptr<Process>> processes, int time) {
	int index = myrandom(size);
	return frame_table[index % size];
}

int PagerRandom::myrandom(int size) {
	// cout << "rancounter " << randCounter << " vals " << randvals[randCounter] << endl;
	int tmp = randvals[randCounter] % size;
	randCounter = (randCounter + 1) % RAND_MOD;
	return tmp;
}
