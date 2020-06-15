#pragma once

#include <memory>
#define stringify( name ) # name

using namespace std;

//typedef enum { CREATED, READY, RUNNG, BLOCKED, PREEMPT, FINISHED } process_state_t;
enum process_state_t { CREATED = 0, READY, RUNNG, BLOCK, PREEMPT, DONE };



class Process {
public:
	Process(int, int, int, int, int, int);

	// for constructor
	int pid, arrivalTime, CPUTime, CPUBurst, IOBurst;

	// some other derived stuff
	int lastUpdateTime;
	int finishTime;  // FT
	int blocktime;  // IT
	int remCPU;
	int readyTime; // CW
	int static_priority;
	int dynamic_priority;
	int ts;

	// some helper
	int nxt_io_burst;
	int ideal_cpu_burst;

	process_state_t state;
};