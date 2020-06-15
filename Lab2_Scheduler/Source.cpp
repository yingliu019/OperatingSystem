#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <list>
#include <iomanip>
#include <getopt.h>

#include "Process.h"
#include "DESLayer.h"
#include "FCFSScheduler.h"
#include "LCFSScheduler.h"
#include "SRTFScheduler.h"
#include "RRScheduler.h"
#include "PRIOScheduler.h"
#include "PREPRIOScheduler.h"

using namespace std;

int myrandom(int);

void file_handler(const char*, list<shared_ptr<Process>>*);

void set_initial_event_queue(shared_ptr<DESLayer>, list<shared_ptr<Process>>*);

void simulation(shared_ptr<DESLayer>, shared_ptr<Scheduler>);

// some global variable
int randCounter = 0;
int RAND_MOD;
std::vector<int> randvals;
int maxprio = 4;
int q = 10000;
int io_counter = 0;
int io_start_time = 0;
int io_total_time = 0;
bool v_arg = false;
bool e_arg = false;
bool t_arg = false;

const char* EventValueNames[] = {
stringify(CREATED),
stringify(READY),
stringify(RUNNG),
stringify(BLOCK),
stringify(PREEMPT),
stringify(DONE)
};

int main(int argc, char* argv[]) {
	// arg parser pc
	/*
	shared_ptr<Scheduler> scheduler;
	const char* inputpath = "input6";
	const char* randpath = "rfile";
	char s = 'E';
	v_arg = true;

	bool first = true;
	string name;

	if (s == 'F') {
		scheduler = make_shared<FCFSScheduler>(q);
		name = "FCFS";
	} else if (s == 'L') {
		scheduler = make_shared<LCFSScheduler>(q);
		name = "LCFS";
	} else if (s == 'S') {
		scheduler = make_shared<SRTFScheduler>(q);
		name = "SRTF";
	} else if (s == 'R') {
		q = 2;
		scheduler = make_shared<RRScheduler>(q);
		name = "RR ";
		name += to_string(q);
	} else if (s == 'P') {
		q = 4;
		maxprio = 4;
		scheduler = make_shared<PRIOScheduler>(q);
		name = "PRIO ";
		name += to_string(q);
	} else if (s == 'E') {
		q = 2;
		maxprio = 5;
		scheduler = make_shared<PREPRIOScheduler>(q);
		name = "PREPRIO ";
		name += to_string(q);
	} else {
		scheduler = make_shared<FCFSScheduler>(q);
	}
	*/


	// arg parser sys
	int c;
	const char* inputpath;
	const char* randpath;
	string name;
	shared_ptr<Scheduler> scheduler;
	while ((c = getopt(argc, argv, "vets:")) != -1) {
		switch (c) {
		case 's':
			if (optarg[0] == 'F') {
				scheduler = make_shared<FCFSScheduler>(q);
				name = "FCFS";
			} else if (optarg[0] == 'L') {
				scheduler = make_shared<LCFSScheduler>(q);
				name = "LCFS";
			} else if (optarg[0] == 'S') {
				scheduler = make_shared<SRTFScheduler>(q);
				name = "SRTF";
			} else if (optarg[0] == 'R') {
				sscanf(optarg + 1, "%d", &q);
				scheduler = make_shared<RRScheduler>(q);
				name = "RR ";
				name += to_string(q);
			} else if (optarg[0] == 'P') {
				sscanf(optarg + 1, "%d:%d", &q, &maxprio);
				scheduler = make_shared<PRIOScheduler>(q);
				name = "PRIO ";
				name += to_string(q);
			} else if (optarg[0] == 'E') {
				sscanf(optarg + 1, "%d:%d", &q, &maxprio);
				scheduler = make_shared<PREPRIOScheduler>(q);
				name = "PREPRIO ";
				name += to_string(q);
			}
			break;
		case 'v':
			v_arg = true;
			break;
		case 't':
			t_arg = true;
			break;
		case 'e':
			e_arg = true;
			break;
		default:
			abort();
		}
	}
	
	inputpath = argv[argc-2]; // file containing processes' info
    randpath = argv[argc-1]; // file containing random numbers

	// START 
	ifstream randfile;
	randfile.open(randpath);
	int num;
	randfile >> num;
	RAND_MOD = num;
	while (randfile >> num) {
		randvals.push_back(num);
	}
	randfile.close();

	// initialization set up
	list<shared_ptr<Process>>* processes = new list<shared_ptr<Process>>();
	shared_ptr<DESLayer> desLayer = make_shared<DESLayer>();

	// some calculation
	file_handler(inputpath, processes);
	set_initial_event_queue(desLayer, processes);
	simulation(desLayer, scheduler);

	// result 
	// focus on processes to get result
	cout << name << "\n";
	list<shared_ptr<Process>>::iterator it;
	int lastFinish;
	int total_cpu = 0;
	int total_cw = 0;
	int total_turnaround = 0;

	for (it = processes->begin(); it != processes->end(); it++) {
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", (*it)->pid, (*it)->arrivalTime, (*it)->CPUTime, (*it)->CPUBurst, (*it)->IOBurst, (*it)->static_priority, (*it)->finishTime, (*it)->finishTime - (*it)->arrivalTime, (*it)->blocktime, (*it)->readyTime);
		lastFinish = max(lastFinish, (*it)->finishTime);
		total_cpu += (*it)->CPUTime;
		total_turnaround += (*it)->finishTime - (*it)->arrivalTime;
		total_cw += (*it)->readyTime;
	}
	double cpu_utilization = (double)total_cpu / lastFinish;
	double io_utilization = (double)io_total_time / lastFinish; 
	double avg_turnaround = (double)total_turnaround / processes->size();
	double avg_cw = (double)total_cw / processes->size();
	double throughput = (double)processes->size() / lastFinish * 100; 
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", lastFinish, 100 * cpu_utilization, 100 * io_utilization, avg_turnaround, avg_cw, throughput);

	// post process
	delete processes;
}

int myrandom(int burst) {
	int tmp = 1 + randvals[randCounter] % burst;
	randCounter = (randCounter + 1) % RAND_MOD;
	return tmp;
}

void file_handler(const char* inputpath, list<shared_ptr<Process>>* processes) {
	ifstream inputfile(inputpath);
	string line;
	int arrivalTime, CPUTime, CPUBurst, IOBurst, static_prior;

	// start to process file
	while (getline(inputfile, line)) {
		vector<string> out;
		istringstream iss(line);
		copy(istream_iterator<string>(iss),
			istream_iterator<string>(),
			back_inserter(out));

		//tokenize(line, out);
		arrivalTime = stoi(out[0]);
		CPUTime = stoi(out[1]);
		CPUBurst = stoi(out[2]);
		IOBurst = stoi(out[3]);
		static_prior = myrandom(maxprio);
		shared_ptr<Process> p_ptr = make_shared<Process>(processes->size(), arrivalTime, CPUTime, CPUBurst, IOBurst, static_prior);
		processes->push_back(p_ptr);
	}
	inputfile.close();
}

void set_initial_event_queue(shared_ptr<DESLayer> desLayer, list<shared_ptr<Process>>* processes) {
	list<shared_ptr<Process>>::iterator it;
	for (it = processes->begin(); it != processes->end(); it++) {
		shared_ptr<struct Event> evt = make_shared<struct Event>((*it)->arrivalTime, *it, CREATED, READY);
		desLayer->put_event(evt);
	}
}

void simulation(shared_ptr<DESLayer> desLayer, shared_ptr<Scheduler> scheduler) {
	shared_ptr<Event> evt;
	int CURRENT_TIME = desLayer->get_next_event_time();
	int timeInPrevState; 
	bool CALL_SCHEDULER; // conditional on whether something is run
	shared_ptr<Process> CURRENT_RUNNING_PROCESS = nullptr;
	shared_ptr<struct Event> nxt_evt;
	int cpu_burst, io_burst;
	bool add_to_expired_queue;

	while (evt = desLayer->get_event()) {
		// select a event to run
		shared_ptr<Process> proc = evt->evtProcess;
		CURRENT_TIME = evt->evtTimeStamp;  // time we finish the old state
		timeInPrevState = CURRENT_TIME - proc->lastUpdateTime;  // time in old state
		proc->state = evt->newstate;
		
		// now assume we finish the event and we reach new state
		switch (evt->newstate) {
		case READY:
			// transation 1, 4: must come from BLOCK or CREATED
			// 1. pre process
			if (evt->oldstate == BLOCK) {
				io_counter -= 1;
				if (io_counter == 0) {
					io_total_time += CURRENT_TIME - io_start_time;
				}
			}
			// 2. process: whenever the process is ready, it must add to ready queue
			scheduler->add_process(proc);
			// 3. print
			if (v_arg) {
				cout << CURRENT_TIME << " " << proc->pid << " " << timeInPrevState << ": " << EventValueNames[evt->oldstate] << " -> " << EventValueNames[evt->newstate] << endl;
			}
			// 4. post process
			if (scheduler->test_preempt(CURRENT_RUNNING_PROCESS, proc, CURRENT_TIME, v_arg)) {
				// the future event on the running processes must be cancelled (rm_event()) 
				desLayer->remove_event(CURRENT_RUNNING_PROCESS);
				// change process stats
				CURRENT_RUNNING_PROCESS->remCPU += CURRENT_RUNNING_PROCESS->ts - CURRENT_TIME;
				//CURRENT_RUNNING_PROCESS->ideal_cpu_burst = q; // ????? todo, not same????
				CURRENT_RUNNING_PROCESS->ideal_cpu_burst += cpu_burst - (CURRENT_TIME - CURRENT_RUNNING_PROCESS->lastUpdateTime);
				CURRENT_RUNNING_PROCESS->ts = CURRENT_TIME;
				// a preemption event for the current time must be generated for that process.
				nxt_evt = make_shared<struct Event>(CURRENT_TIME, CURRENT_RUNNING_PROCESS, RUNNG, PREEMPT);
				desLayer->put_event(nxt_evt);
				//CURRENT_RUNNING_PROCESS = proc;
			}
			CALL_SCHEDULER = true; // conditional on whether something is run
			proc->lastUpdateTime = CURRENT_TIME;
			//if (CURRENT_TIME == 51 & proc->pid == 1)
			//	int a = 1;
			break;
		case RUNNG:
			// transition 2, whenever the process is running, it must exit ready queue sentimentally
			// 1. pre process
			proc->readyTime += CURRENT_TIME - proc->lastUpdateTime;
			CURRENT_RUNNING_PROCESS = proc;
			// 2. process
			if (proc->ideal_cpu_burst == 0) {
				proc->ideal_cpu_burst = myrandom(proc->CPUBurst);
			}
			cpu_burst = min({ proc->ideal_cpu_burst, proc->remCPU, scheduler->quantum }); // compare remaining time to curst time
			if (proc->remCPU == cpu_burst) {
				// go to finish
				nxt_evt = make_shared<struct Event>(CURRENT_TIME + cpu_burst, evt->evtProcess, RUNNG, DONE);
				desLayer->put_event(nxt_evt);
				// 3. print
				if (v_arg) {
					cout << CURRENT_TIME << " " << proc->pid << " " << timeInPrevState << ": " << EventValueNames[evt->oldstate] << " -> " << EventValueNames[evt->newstate] << " cb=" << cpu_burst << " rem=" << proc->remCPU << " prio=" << proc->dynamic_priority << endl;
				}
			}
			else if (cpu_burst == proc->ideal_cpu_burst) {
				// go to io burst
				nxt_evt = make_shared<struct Event>(CURRENT_TIME + cpu_burst, evt->evtProcess, RUNNG, BLOCK);
				desLayer->put_event(nxt_evt);
				// 3. print
				if (v_arg) {
					cout << CURRENT_TIME << " " << proc->pid << " " << timeInPrevState << ": " << EventValueNames[evt->oldstate] << " -> " << EventValueNames[evt->newstate] << " cb=" << cpu_burst << " rem=" << proc->remCPU << " prio=" << proc->dynamic_priority << endl;
				}
			}
			else {
				// preemption
				nxt_evt = make_shared<struct Event>(CURRENT_TIME + cpu_burst, evt->evtProcess, RUNNG, PREEMPT);
				desLayer->put_event(nxt_evt);
				// 3. print
				if (v_arg) {
					cout << CURRENT_TIME << " " << proc->pid << " " << timeInPrevState << ": " << EventValueNames[evt->oldstate] << " -> " << EventValueNames[evt->newstate] << " cb=" << min(proc->ideal_cpu_burst, proc->remCPU) << " rem=" << proc->remCPU << " prio=" << proc->dynamic_priority << endl;
				}
			}
			// 4. post process
			//if (CURRENT_TIME == 51 & proc->pid == 0)
			//	int a = 1;
			proc->ideal_cpu_burst -= cpu_burst;
			proc->lastUpdateTime = CURRENT_TIME;
			proc->ts = CURRENT_TIME + cpu_burst;
			proc->remCPU -= cpu_burst;
			break;
		case BLOCK:
			// transition 3
			// 1. pre process
			// 2. process
			io_burst = myrandom(proc->IOBurst);
			// create an event for when process becomes READY again
			nxt_evt = make_shared<struct Event>(CURRENT_TIME + io_burst, evt->evtProcess, BLOCK, READY);
			desLayer->put_event(nxt_evt);
			// 3. print
			if (v_arg) {
				cout << CURRENT_TIME << " " << proc->pid << " " << timeInPrevState << ": " << EventValueNames[evt->oldstate] << " -> " << EventValueNames[evt->newstate] << " ib=" << io_burst << " rem=" << proc->remCPU << endl;
			}
			// 4. post process
			//if (io_burst == 7 & proc->pid == 0)
			//	int a = 1;
			CALL_SCHEDULER = true;
			proc->nxt_io_burst = io_burst;
			proc->blocktime += io_burst;
			proc->lastUpdateTime = CURRENT_TIME;
			CURRENT_RUNNING_PROCESS = nullptr;
			scheduler->change_process_dp(proc, false);
			io_counter += 1;
			if (io_counter == 1) {
				io_start_time = CURRENT_TIME;
			}
			break;
		case PREEMPT:
			// transition 5: come from PREEMPTION, treat prempt keyword as ready, add to runqueue (no event is generated)
			// 1. pre process
			// 2. process
			// 3. print
			if (v_arg) {
				cout << CURRENT_TIME << " " << proc->pid << " " << timeInPrevState << ": " << EventValueNames[evt->oldstate] << " -> " << EventValueNames[READY] << " cb=" << min(proc->ideal_cpu_burst, proc->remCPU) << " rem=" << proc->remCPU << " prio=" << proc->dynamic_priority << endl;  
			}
			// 4. post process
			//if (evt->oldstate == RUNNG & CURRENT_TIME == 109 & proc->pid == 0)
			//	int a = 1;
			CURRENT_RUNNING_PROCESS = nullptr;
			CALL_SCHEDULER = true;
			proc->lastUpdateTime = CURRENT_TIME;
			add_to_expired_queue = scheduler->change_process_dp(proc, true); 
			if (add_to_expired_queue == false) {
				scheduler->add_process(proc);  // PRIO must add after change dynamic prio
			}
			break;
		case DONE:
			// 1. pre process
			// 2. process
			// 3. print
			if (v_arg) {
				cout << CURRENT_TIME << " " << proc->pid << " " << timeInPrevState << ": Done" << endl;
			}
			// 4. post process
			proc->finishTime = CURRENT_TIME;
			CALL_SCHEDULER = true;
			CURRENT_RUNNING_PROCESS = nullptr;
			proc->ideal_cpu_burst = 0;
			break;
		}
		// remove current event object from Memory, share_ptr will do the job
		// delete evt; 
		// evt = nullptr;

		if (CALL_SCHEDULER) {
			if (desLayer->get_next_event_time() == CURRENT_TIME) {  
				//process next event from Event queue
				continue; 
			}

			if (CURRENT_RUNNING_PROCESS == nullptr) {
				CURRENT_RUNNING_PROCESS = scheduler->get_next_process();
				if (CURRENT_RUNNING_PROCESS == nullptr) {
					// time lap, wait for next event
					continue;
				}

				// create event to make this process runnable for same time.
				// 1. if it's no preemption, only when CPU is not running these events can be created,
				//	  such that running time will be postponed till current time
				if (CURRENT_RUNNING_PROCESS->state == READY) {
					nxt_evt = make_shared<struct Event>(CURRENT_TIME, CURRENT_RUNNING_PROCESS, READY, RUNNG);
					desLayer->put_event(nxt_evt);
				} else if (CURRENT_RUNNING_PROCESS->state == PREEMPT) {
					nxt_evt = make_shared<struct Event>(CURRENT_TIME, CURRENT_RUNNING_PROCESS, READY, RUNNG);
					desLayer->put_event(nxt_evt);
				}
			}
			CALL_SCHEDULER = false; // reset global flag
		}
	}
}

