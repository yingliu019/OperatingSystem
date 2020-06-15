#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iomanip>
#include <list>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <climits>

#include "IORequest.h"
#include "SchedulerFIFO.h"
#include "SchedulerSSTF.h"
#include "SchedulerLOOK.h"
#include "SchedulerCLOOK.h"
#include "SchedulerFLOOK.h"

using namespace std;


/* main function incluse simulation */
int main(int argc, char* argv[]) {
	const char* inputpath;
	shared_ptr<Scheduler> IO_SCHEDULER;
	inputpath = argv[argc - 1]; // file containing processes' info
	int c;
	while ((c = getopt(argc, argv, "s:")) != -1) {
		switch (c) {
		case 's':
			if (optarg[0] == 'i') {
				IO_SCHEDULER = make_shared<SchedulerFIFO>();
			}
			else if (optarg[0] == 'j') {
				IO_SCHEDULER = make_shared<SchedulerSSTF>();
			}
			else if (optarg[0] == 's') {
				IO_SCHEDULER = make_shared<SchedulerLOOK>();
			}
			else if (optarg[0] == 'c') {
				IO_SCHEDULER = make_shared<SchedulerCLOOK>();
			}
			else if (optarg[0] == 'f') {
				IO_SCHEDULER = make_shared<SchedulerFLOOK>();
			}
			break;
		}
	}

	/* initial data set up */
	vector<shared_ptr<IORequest>> requests;

	/* input files read */
	ifstream inputfile(inputpath);
	string line;
	int counter = 0;

	while (getline(inputfile, line)) {
		stringstream linestream(line);
		string var1, var2;
		linestream >> var1 >> var2;
		if (!var1.empty() && var1.at(0) == '#') continue;
		//cout << var1 << " " << var2 << endl;

		shared_ptr<IORequest> r_ptr = make_shared<IORequest>(counter, stoi(var1), stoi(var2));
		requests.push_back(r_ptr);
		counter++;
	}

	// simulation part 
	int time = 0;
	int nxt_request = 0;
	bool active_io = false;
	shared_ptr<IORequest> current_io;

	while (true) {
		//1) If a new I/O arrived to the system at this current time → add request to IO-queue 
		//cout << nxt_request << " ready queue " << IO_SCHEDULER->IOqueue.size() << endl;
		if (nxt_request < requests.size() && requests[nxt_request]->issue_time == time) {
			IO_SCHEDULER->add_request(requests[nxt_request]);
			nxt_request++;
		}

		//2) If an IO is active and completed at this time → Compute relevant info and store in IO request for final summary
		if (active_io && current_io->track == IO_SCHEDULER->HEAD_POS) {
			current_io->end_time = time;
			//printf("111 %5d: %5d %5d %5d\n", current_io->r_id, current_io->issue_time, current_io->start_time, current_io->end_time);
			//cout << IO_SCHEDULER->HEAD_POS << " " << time <<endl;
			current_io = nullptr;
			active_io = false;
		}

		//3) If an IO is active but did not yet complete → Move the head by one sector / track / unit in the direction it is going(to simulate seek)
		if (active_io && current_io->track != IO_SCHEDULER->HEAD_POS) {
			IO_SCHEDULER->HEAD_POS += IO_SCHEDULER->DIR;
			current_io->track_move++;
		}

		//4) If no IO request active now(after(2)) but IO requests are pending → Fetch the next request and start the new IO.
		if (!active_io && !IO_SCHEDULER->is_empty()) {
			current_io = IO_SCHEDULER->get_next_request();
			IO_SCHEDULER->set_direction(current_io);
			current_io->start_time = time;
			active_io = true;
			continue;
		} 

		//5) If no IO request is active now and no IO requests pending → exit simulation
		if (!active_io && IO_SCHEDULER->IOqueue.empty() && nxt_request >= requests.size()) {
			break;
		}
		time++;
		//cout << IO_SCHEDULER->HEAD_POS << " ### " << time << endl;
	}

	int total_time = 0;
	int tot_movement = 0;
	double avg_turnaround = 0;
	double avg_waittime = 0;
	int max_waittime = 0;

	vector<shared_ptr<IORequest>>::iterator it;
	for (it = requests.begin(); it != requests.end(); it++) {
		printf("%5d: %5d %5d %5d\n", (*it)->r_id, (*it)->issue_time, (*it)->start_time, (*it)->end_time);
		total_time = max((*it)->end_time, total_time); 
		tot_movement += (*it)->track_move;
		avg_turnaround += (*it)->end_time - (*it)->issue_time;
		max_waittime = max((*it)->start_time - (*it)->issue_time, max_waittime);
		avg_waittime += (*it)->start_time - (*it)->issue_time;
	}
	avg_waittime = avg_waittime / requests.size();
	avg_turnaround = avg_turnaround / requests.size();
	printf("SUM: %d %d %.2lf %.2lf %d\n", total_time, tot_movement, avg_turnaround, avg_waittime, max_waittime);

	/* post process */
	inputfile.close();
	//cin.get();
}
