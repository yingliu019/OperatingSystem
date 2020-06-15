#pragma once

#include <memory>

class IORequest {
public:
	IORequest(int, int, int);

	int issue_time;
	int track;
	int r_id;
	int start_time;
	int end_time;
	int track_move = 0;
};