#include "Process.h"

Process::Process(int v_size, int p_size, int p) {
	proc_id = p;
	unmaps = maps = ins = outs = fins = fouts = zeros = segv = segprot = 0;
	ctx_switch = access = exits = 0;

	vector<shared_ptr<vma_t>> vmas;

	for (int i = 0; i < p_size; i++) {
		page_table.push_back(make_shared<pte_t>());
	}
}

void Process::put_vma(int start, int end, int write_protected, int file_mapped) {
	shared_ptr<vma_t> nxt_vma_t = make_shared<vma_t>(start, end, write_protected, file_mapped);
	vmas.push_back(nxt_vma_t);
}