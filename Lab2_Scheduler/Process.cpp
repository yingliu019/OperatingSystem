#include "Process.h"

Process::Process(int p, int at, int tc, int cb, int io, int s_pr) {
	pid = p;
	arrivalTime = at;
	CPUTime = tc;
	CPUBurst = cb;
	IOBurst = io;

	static_priority = s_pr;
	dynamic_priority = static_priority - 1;
	state = CREATED;

	// helper
	remCPU = CPUTime;
	//nxt_cpu_burst = 0;
	nxt_io_burst = 0;
	ideal_cpu_burst = 0;
	ts = 0;
	lastUpdateTime = arrivalTime;

	// result
	blocktime = 0;  // IT
	readyTime = 0; // CW
	finishTime = arrivalTime;  // FT
	
}
