#ifndef _HIGHTIMER_H_
#define _HIGHTIMER_H_

#include <chrono>

class HighTimer {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _begin;

public:
	HighTimer();

	void upData();

	float getSec();

	float getMSec();

	long long getUSec();
};

#endif // !_HIGHTIMER_H_
