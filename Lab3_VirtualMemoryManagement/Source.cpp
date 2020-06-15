#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iomanip>
#include <deque>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <climits>

#include "Process.h"
#include "PagerFIFO.h"
#include "PagerRandom.h"
#include "PagerClock.h"
#include "PagerNRU.h"
#include "PagerAging.h"
#include "PagerWorkingSet.h"

using namespace std;


/* main function incluse simulation */
int main(int argc, char* argv[]) {

	const int MAX_PAGES = 64;

	// arg parser sys
	int c;
	bool O_arg = false;
	bool P_arg = false;
	bool F_arg = false;
	bool S_arg = false;
	const char* inputpath;
	const char* randpath;
	shared_ptr<Pager> THE_PAGER;
	int numOfFrame = 32;
	int optLen;
	inputpath = argv[argc - 2]; // file containing processes' info
	randpath = argv[argc - 1]; // file containing random numbers
	while ((c = getopt(argc, argv, "a:o:f:")) != -1) {
		switch (c) {
		case 'a':
			if (optarg[0] == 'f') {
				THE_PAGER = make_shared<PagerFIFO>();
			}
			else if (optarg[0] == 'r') {
				THE_PAGER = make_shared<PagerRandom>(randpath);
			}
			else if (optarg[0] == 'c') {
				THE_PAGER = make_shared<PagerClock>();
			}
			else if (optarg[0] == 'e') {
				THE_PAGER = make_shared<PagerNRU>();
			}
			else if (optarg[0] == 'a') {
				THE_PAGER = make_shared<PagerAging>();
			}
			else if (optarg[0] == 'w') {
				THE_PAGER = make_shared<PagerWorkingSet>();
			}
			break;
		case 'o':
			optLen = strlen(optarg);
			for (int i = 0; i < optLen; i++) {
				switch (optarg[i]) {
				case 'O':
					O_arg = true;
					break;
				case 'P':
					P_arg = true;
					break;
				case 'F':
					F_arg = true;
					break;
				case 'S':
					S_arg = true;
					break;
				}
			}
			break;
		case 'f':
			sscanf(optarg, "%d", &numOfFrame);
			break;
		}
	}
	const int MAX_FRAMES = numOfFrame;

	/* initial data set up */
	vector<shared_ptr<Process>> processes;
	shared_ptr<frame_t> frame_table[MAX_FRAMES];
	deque<int> FREE_POOL;  // pop the front, append the end

	for (int i = 0; i < MAX_FRAMES; i++) {
		FREE_POOL.push_back(i); // just initial with 0, doesn't matter
		frame_table[i] = make_shared<frame_t>(0, 0, i, 0, 0, false);
	}


	/* input files read */
	ifstream inputfile(inputpath);
	string line;
	int num_process = 0;
	int count_process = 0;
	int num_vmas = 0;
	int count_vmas = 0;

	while (getline(inputfile, line)) {
		stringstream linestream(line);
		string var1, var2, var3, var4;
		linestream >> var1 >> var2 >> var3 >> var4;
		if (!var1.empty() && var1.at(0) == '#') continue;
		//cout << var1 << " " << var2 << " " << var3 << " " << var4 << endl;
		if (num_process == 0) {
			num_process = stoi(var1);
			// intial a process list
			continue;
		}
		if (num_vmas == 0) {
			num_vmas = stoi(var1);
			// intial a process
			shared_ptr<Process> p_ptr = make_shared<Process>(num_vmas, MAX_PAGES, count_process);
			processes.push_back(p_ptr);
			continue;
		}
		if (count_vmas < num_vmas) {
			// create vma of process
			processes[count_process]->put_vma(stoi(var1), stoi(var2), stoi(var3), stoi(var4));
			//cout << var1 << " " << var2 << " " << var3 << " " << var4 << endl;
			count_vmas++;
			if (count_vmas == num_vmas) {
				num_vmas = 0;
				count_vmas = 0;
				count_process++;
			}
			if (count_process == num_process) {
				break;
			}
		}
	}

	/* simulation part */
	shared_ptr<Process> current_process;
	int inst_count = 0;

	while (getline(inputfile, line)) {
		stringstream linestream(line);
		string var1;
		int var2;
		linestream >> var1 >> var2;
		if (!var1.empty() && var1.at(0) == '#') continue;

		if (O_arg) {
			cout << inst_count << ": ==> " << var1 << " " << var2 << endl;
		}
		// after parse now start process
		// handle special case of “c” and “e” instruction
		if (var1 == "c") {
			current_process = processes[var2];
			current_process->ctx_switch++;
			inst_count++;
			continue;
		}
		if (var1 == "e") {
			vector<shared_ptr<pte_t>> current_page_table = current_process->page_table;
			shared_ptr<pte_t> pte;
			shared_ptr<frame_t> fte;
			cout << "EXIT current process " << current_process->proc_id << endl;
			for (int i = 0; i < MAX_PAGES; i++) {
				pte = current_page_table[i];
				pte->pagedout = 0;
				if (pte->present == 1) {
					pte->present = 0;
					fte = frame_table[pte->physical_frame];
					fte->used = false;
					FREE_POOL.push_back(pte->physical_frame);
					if (O_arg) {
						cout << " UNMAP " << current_process->proc_id << ":" << i << endl;
					}
					current_process->unmaps++;
					if (pte->modified && pte->file_mapped) {
						if (O_arg) {
							cout << " FOUT" << endl;
						}
						processes[fte->proc_id]->fouts++;
					}
				}
			}
			current_process->exits++;
			inst_count++;
			continue;
		}
		// now the real instructions for read and write
		shared_ptr<pte_t> pte = current_process->page_table[var2]; // in reality this is done by hardware
		current_process->access++;
		if (!pte->present) {
			// in reality os generates the page fault exception, here within the block is page fault handler simulation
			// 1.1. determine that the vpage can be accessed, i.e.it is part of one of the VMAs.
			bool in_vmas = false;
			vector<shared_ptr<vma_t>> current_vmas = current_process->vmas;
			for (int i = 0; i < current_vmas.size(); i++) {
				if (current_vmas[i]->start_vpage <= var2 && var2 <= current_vmas[i]->end_vpage) {
					in_vmas = true;
					pte->vma = i;
					break;
				}
			}

			// 1.2. initialization
			if (!pte->is_initialed) {
				shared_ptr<vma_t> current_vma = current_vmas[pte->vma];
				vector<shared_ptr<pte_t>> current_page_table = current_process->page_table;
				for (int i = current_vma->start_vpage; i <= current_vma->end_vpage; i++) {
					current_page_table[i]->file_mapped = current_vma->file_mapped;
					current_page_table[i]->write_protect = current_vma->write_protected;
					current_page_table[i]->is_initialed = 1;
				}
			}

			// 2. If not, a SEGV output line must be created and you move on to the next instruction.
			if (!in_vmas) {
				if (O_arg) {
					cout << " SEGV" << endl;
				}
				current_process->segv++;
				inst_count++;
				continue;
			}

			// 3. select a frame to replace(make pluggable with different replacement algorithms)
			shared_ptr<frame_t> fte;
			if (!FREE_POOL.empty()) {
				int free_index = FREE_POOL.front();
				FREE_POOL.pop_front();
				fte = frame_table[free_index];
			}
			else {
				fte = THE_PAGER->select_victim_frame(frame_table, MAX_FRAMES, processes, inst_count);
				// 4.1 victim frame Unmap from its current user(UNMAP): <address space:vpage>, its entry in the owning process’s page_table must be removed  
				fte->used = false;
				shared_ptr<pte_t> fte_prev_user = processes[fte->proc_id]->page_table[fte->vpage];
				fte_prev_user->physical_frame = 0;
				fte_prev_user->present = 0;
				if (O_arg) {
					cout << " UNMAP " << fte->proc_id << ":" << fte->vpage << endl;
				}
				processes[fte->proc_id]->unmaps++;

				// 4.2 Save frame to disk if necessary(OUT / FOUT)
				// inspect the state of the R and M bits, if the page was modified, then the page frame must be paged out to the swap device (“OUT”) or in case it was file mapped written back to the file (“FOUT”). 
				// First the PTE must be reset (note once the PAGEDOUT flag is set it will never be reset as it indicates there is content on the swap device) and then the PTE’s frame must be set.
				if (fte_prev_user->modified && fte_prev_user->file_mapped) {
					if (O_arg) {
						cout << " FOUT" << endl;
					}
					processes[fte->proc_id]->fouts++;
					fte_prev_user->modified = 0;
				}
				else if (fte_prev_user->modified) {
					fte_prev_user->pagedout = 1;
					if (O_arg) {
						cout << " OUT" << endl;
					}
					processes[fte->proc_id]->outs++;
					fte_prev_user->modified = 0;
				}
			}


			// 5.1 page must be instantiated, Map its new user(MAP)
			// i.e. a frame must be allocated, assigned to the PTE of this instruction
			pte->physical_frame = fte->frame_index;
			pte->present = 1;
			frame_table[pte->physical_frame]->used = true;
			// 5.2 populated with the proper content.
			// if this page was previously paged out, the page must be brought back from the swap space “IN” or “FIN”
			if (pte->file_mapped) {
				if (O_arg) {
					cout << " FIN" << endl;
				}
				current_process->fins++;
			}
			else if (pte->pagedout) {
				if (O_arg) {
					cout << " IN" << endl;
				}
				current_process->ins++;
			}
			else if (pte->file_mapped == 0) {
				// If the vpage was never swapped out and is not file mapped, then by definition it still has a zero filled content and you issue the “ZERO” output.
				if (O_arg) {
					cout << " ZERO" << endl;
				}
				current_process->zeros++;
			}
			if (O_arg) {
				cout << " MAP " << fte->frame_index << endl;
			}
			processes[current_process->proc_id]->maps++;
			fte->proc_id = current_process->proc_id;
			fte->vpage = var2;
			fte->age = 0;
		}

		// it is guaranteed the vpage is backed by a frame and the instruction can proceed in hardware 
		// 6. check write protection

		if (var1 == "r") {
			pte->referenced = 1;
		}
		else if (var1 == "w" && pte->write_protect == 1) {
			// The page is considered referenced but not modified in this case.
			if (O_arg) {
				cout << " SEGPROT" << endl;
			}
			current_process->segprot++;
			pte->referenced = 1;
		}
		else if (var1 == "w" && pte->write_protect == 0) {
			pte->modified = 1;
			pte->referenced = 1;
		}
		shared_ptr<frame_t> fte = frame_table[pte->physical_frame];
		fte->last_used = inst_count;
		fte->age = (fte->age | 0x80000000);
		inst_count++;
	}

	/* summary option output */
	// P
	if (P_arg) {
		for (int i = 0; i < processes.size(); i++) {
			cout << "PT[" << i << "]: ";
			vector<shared_ptr<pte_t>> cur_page_table = processes[i]->page_table;
			for (int j = 0; j < cur_page_table.size(); j++) {
				if (cur_page_table[j]->present == 1) {
					cout << j << ":" << (cur_page_table[j]->referenced ? "R" : "-") << (cur_page_table[j]->modified ? "M" : "-") << (cur_page_table[j]->pagedout ? "S" : "-") << " ";
				}
				else if (cur_page_table[j]->pagedout) {
					cout << "# ";
				}
				else if (cur_page_table[j]->pagedout == 0) {
					cout << "* ";
				}
			}
			cout << endl;
		}
	}

	// F
	if (F_arg) {
		cout << "FT: ";
		for (int i = 0; i < MAX_FRAMES; i++) {
			if (frame_table[i]->used) {
				cout << frame_table[i]->proc_id << ":" << frame_table[i]->vpage << " ";
			} else {
				cout << "* ";
			}
		}
		cout << endl;
	}

	// S
	if (S_arg) {
		unsigned long long cost = 0;
		int ctx_switches = 0;
		int process_exits = 0;

		vector<shared_ptr<Process>>::iterator it;
		int proc_id, unmaps, maps, ins, outs, fins, fouts, zeros, segv, segprot;
		int ctx_switch, access, exits;

		for (it = processes.begin(); it != processes.end(); it++) {
			printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
				(*it)->proc_id, (*it)->unmaps, (*it)->maps, (*it)->ins, (*it)->outs, (*it)->fins, (*it)->fouts, (*it)->zeros, (*it)->segv, (*it)->segprot);
			cost += 400 * (*it)->unmaps;
			cost += 400 * (*it)->maps;
			cost += 3000 * (*it)->ins;
			cost += 3000 * (*it)->outs;
			cost += 2500 * (*it)->fins;
			cost += 2500 * (*it)->fouts;
			cost += 150 * (*it)->zeros;
			cost += 240 * (*it)->segv;
			cost += 300 * (*it)->segprot;
			cost += (*it)->access;
			cost += 121 * (*it)->ctx_switch;
			cost += 175 * (*it)->exits;

			ctx_switches += (*it)->ctx_switch;
			process_exits += (*it)->exits;
		}
		printf("TOTALCOST %lu %lu %lu %llu\n", inst_count, ctx_switches, process_exits, cost);
	}
	/* post process */
	inputfile.close();
	//cin.get();
}
