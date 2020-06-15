#include "IORequest.h"

IORequest::IORequest(int id, int time, int t) {
	r_id = id;
	issue_time = time;
	track = t;
	start_time = end_time = 0;
}

