#pragma once

#include <memory>
#include <vector>

using namespace std;

struct pte_t {
	// can only be total of 32-bit size !!!! 
	unsigned present : 1;
	unsigned write_protect : 1;
	unsigned file_mapped : 1;
	unsigned modified : 1;
	unsigned referenced : 1;
	unsigned pagedout : 1;
	unsigned is_initialed : 1;
	unsigned physical_frame : 7;
	unsigned vma : 6;
	// somw other stuff
};

struct vma_t { 
	int start_vpage; 
	int end_vpage;
	int write_protected; // todo, use short
	int file_mapped;
	vma_t(int s, int e, int w, int f)
		: start_vpage(s), end_vpage(e), write_protected(w), file_mapped(f) {}
};

class Process {
public:
	Process(int, int, int);

	unsigned long proc_id, unmaps, maps, ins, outs, fins, fouts, zeros, segv, segprot;
	unsigned long ctx_switch, access, exits;

	vector<shared_ptr<vma_t>> vmas;
	vector<shared_ptr<pte_t>> page_table;

	void put_vma(int, int, int, int);
};
